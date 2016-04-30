//#include "bencodeparser.h"
#include "connectionmanager.h"
#include "torrentclient.h"
#include "torrentserver.h"
#include "trackerclient.h"

#include <QsLog/QsLog.h>
#include <QsLog/QsLogDest.h>
#include <settings.h>

#include <QtCore>

TrackerClient::TrackerClient(TorrentClient *downloader, QObject *parent)
    : QObject(parent), torrentDownloader(downloader)
{
    length = 0;
    requestInterval = 5 * 60;
    requestIntervalTimer = -1;
    firstTrackerRequest = true;
    lastTrackerRequest = false;
    firstSeeding = true;

    //connect(&http, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpRequestDone(QNetworkReply*)));

    socket = new QTcpSocket(parent);

    //QObject::connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    //QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    QObject::connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(trackerResponse()));

}

void TrackerClient::start(const MetaInfo &info)
{
    QLOG_DEBUG() << "TrackerClient::start";
    metaInfo = info;
    QTimer::singleShot(0, this, SLOT(fetchPeerList()));

    // only working one file at a time
    length = metaInfo.singleFile().length;

    socket->connectToHost(Settings::trackerHost(), Settings::trackerPort());

    if (!socket->waitForConnected())
    {
        QLOG_ERROR() << "Error connecting to Tracker on host:" << Settings::trackerHost() << ", port: " << Settings::trackerPort() << " (error:" << socket->errorString() << ")";
    }
    else
    {
        QLOG_INFO() << "Successfully connected to Tracker on host:" << Settings::trackerHost() << ", port: " << Settings::trackerPort() ;
    }

}

void TrackerClient::startSeeding()
{
    QLOG_DEBUG() << "TrackerClient::startSeeding";
    firstSeeding = true;
    fetchPeerList();
}

void TrackerClient::stop()
{
    QLOG_DEBUG() << "TrackerClient::stop";
    lastTrackerRequest = true;
    //fetchPeerList();
}

void TrackerClient::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == requestIntervalTimer) {
        fetchPeerList();
    } else {
        QObject::timerEvent(event);
    }
}

void TrackerClient::fetchPeerList()
{

    QLOG_DEBUG() << "TrackerClient::fetchPeerList";

    // full .msf info
    QByteArray infoValue = torrentDownloader->metaInfo().infoValue();
    // initiating query with full file info
    QUrlQuery query(infoValue);

    // Percent encode the hash
    QByteArray infoHash = torrentDownloader->infoHash();
    QByteArray encodedSum;
    for (int i = 0; i < infoHash.size(); ++i) {
        encodedSum += '%';
        encodedSum += QByteArray::number(infoHash[i], 16).right(2).rightJustified(2, '0');
    }

    bool seeding = (torrentDownloader->state() == TorrentClient::Seeding);

    // add later
    query.addQueryItem("info_hash", encodedSum);

    query.addQueryItem("peer_id", ConnectionManager::instance()->clientId());
    query.addQueryItem("uploaded", QByteArray::number(torrentDownloader->uploadedBytes()));
    query.addQueryItem("port", QByteArray::number(TorrentServer::instance()->serverPort()));


    if (!firstSeeding) {
        query.addQueryItem("downloaded", "0");
        query.addQueryItem("left", "0");
    } else {
        query.addQueryItem("downloaded",
                           QByteArray::number(torrentDownloader->downloadedBytes()));
        int left = qMax<int>(0, metaInfo.totalSize() - torrentDownloader->downloadedBytes());
        query.addQueryItem("left", QByteArray::number(seeding ? 0 : left));
    }

    if (seeding && firstSeeding) {
        query.addQueryItem("event", "completed");
        firstSeeding = false;
    } else if (firstTrackerRequest) {
        firstTrackerRequest = false;
        query.addQueryItem("event", "started");
    } else if(lastTrackerRequest) {
        query.addQueryItem("event", "stopped");
    }

    // not sure we need to pass local address
    if (socket->state() == socket->ConnectedState)
    {
        query.addQueryItem("ip", socket->localAddress().toString());
    }
    else QLOG_DEBUG() << "Can't obtain local host address" << " error:" << socket->errorString();


    if (!trackerId.isEmpty())
        query.addQueryItem("trackerid", trackerId);

    //QByteArray qry = query.toString().toLatin1();
    QByteArray qry = query.query(QUrl::FullyEncoded).toUtf8();

    socket->write(qry);

    socket->flush();

}

void TrackerClient::bytesWritten(qint64 bytes) {
    QLOG_TRACE() << "Sent to Tracker" << bytes << " bytes";
}

void TrackerClient::trackerResponse() {

    QByteArray response = socket->readAll();

    QLOG_DEBUG() << "[Tracker] All:" << response;
    socket->deleteLater();
    if (lastTrackerRequest) {
        emit stopped();
        return;
    }


    if (socket->error() != socket->UnknownSocketError)
    {
        emit connectionError(socket->errorString());
        return;
    }

    socket->close();

    QUrlQuery reply = QUrlQuery(response);

    if (reply.hasQueryItem("failure-reason")) {
        // if failure, no more action
        QLOG_DEBUG() << "[Tracker] Failure:" << reply.queryItemValue("failure-reason");
        emit failure(reply.queryItemValue("mag-failure-reason"));
        return;
    }

    if (reply.hasQueryItem("warning-message")) {
        // if warning, show and go ahead
        emit warning(reply.queryItemValue("warning-message"));
        QLOG_DEBUG() << "[Tracker] Warning:" << reply.queryItemValue("warning-message");
    }

    if (reply.hasQueryItem("trackerid")) {
        //trackerId = reply.queryItemValue("trackerid").toLatin1();
        trackerId = reply.query(QUrl::FullyEncoded).toUtf8();
    }

    if (reply.hasQueryItem("interval")) {
        if (requestIntervalTimer != -1)
            killTimer(requestIntervalTimer);
        requestIntervalTimer = startTimer(reply.queryItemValue("interval").toInt() * 1000);
    }

    // Apparently, no use of these 2 params so far
    if (reply.hasQueryItem("complete")) {
        QLOG_DEBUG() << "[Tracker] seeders:" << reply.queryItemValue("complete");
    }
    if (reply.hasQueryItem("incomplete")) {
        QLOG_DEBUG() << "[Tracker] leechers:" << reply.queryItemValue("incomplete");
    }

    // getting peers list
    if (reply.hasQueryItem("peers")) {
        // store it
        peers.clear();
        QUrlQuery peerEntry = QUrlQuery(reply.queryItemValue("peers"));
        peerEntry.setQueryDelimiters(QString(":").at(0),QString(";").at(0));
        TorrentPeer tmp;
        //                QMap<QByteArray, QVariant> peer = qvariant_cast<QMap<QByteArray, QVariant> >(peerTmp.at(i));
        tmp.id = peerEntry.queryItemValue("peerid");
        tmp.address.setAddress(peerEntry.queryItemValue("ip"));
        tmp.port = peerEntry.queryItemValue("port").toInt();
        peers << tmp;
//        QVariant peerEntry = ;
//        if (peerEntry.type() == QVariant::List) {
//            QList<QVariant> peerTmp = peerEntry.toList();
//            for (int i = 0; i < peerTmp.size(); ++i) {
//                TorrentPeer tmp;
//                QMap<QByteArray, QVariant> peer = qvariant_cast<QMap<QByteArray, QVariant> >(peerTmp.at(i));
//                tmp.id = QString::fromUtf8(peer.value("peer id").toByteArray());
//                tmp.address.setAddress(QString::fromUtf8(peer.value("ip").toByteArray()));
//                tmp.port = peer.value("port").toInt();
//                peers << tmp;
//            }
//        }
    }


    emit peerListUpdated(peers);
}
