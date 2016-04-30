#include "serverthread.h"
#include "qtcpsocket.h"
#include "qhostaddress.h"
#include <QsLog/QsLog.h>
#include <QsLog/QsLogDest.h>
#include <servermetainfo.h>
#include <server.h>

ServerMetaInfo SrvMetaInfo;
MetaInfoStruct mInfoStr;
Server* tracker;
int requestInterval = 5 * 60;

ServerThread::ServerThread(int descriptor, QObject *parent) : QThread(parent), descriptor(descriptor)
{
    tracker = qobject_cast<Server*>(parent);
}

void ServerThread::run() {
    QTcpSocket socket;

    if (!socket.setSocketDescriptor(descriptor)) {
        QLOG_ERROR() << "Socket error";
        return;
    }

    while (socket.waitForReadyRead()) {
        auto data = socket.readAll();

        qDebug() << "[Client] ip:" << socket.peerAddress().toString() << "\ndata:" << data;
        QLOG_TRACE() << "[Client] ip:" << socket.peerAddress().toString() << "\ndata:" << data;

        QByteArray msg;

        QLOG_TRACE() << "ServerThread::run - SrvMetaInfo.magTrackerQueryParser";

        if (  !SrvMetaInfo.magTrackerQueryParser(data))
        {
            qDebug() << SrvMetaInfo.errorString();
            QLOG_ERROR() << SrvMetaInfo.errorString();
        }
        else
        {
            mInfoStr = tracker->getClient(SrvMetaInfo.infoHash());

            mInfoStr.ip = socket.peerAddress().toString();

            if ( mInfoStr.lastUpd.msecsTo(QDateTime::currentDateTime()) < requestInterval) {
                // keep client info - update timestamp
               tracker->addClient(SrvMetaInfo.infoHash(),SrvMetaInfo.metaStruct());
               QLOG_DEBUG() << "[Client] added";
            }
            else
            {
                // remove stale client
                tracker->removeClient(SrvMetaInfo.infoHash());
                QLOG_DEBUG() << "[Client] removed";
            }

        }
        if (SrvMetaInfo.errorString() > "")
        {
            msg = "failure-reason=";
            msg += SrvMetaInfo.errorString().toLatin1();
        }
        else
        {
            msg = magTrackerResponseBuilder().toLatin1();
        }
        socket.write(msg);
        qDebug() << "[Tracker] connected:" << tracker->countClients();
        QLOG_TRACE() << "[Tracker] connected:" << tracker->countClients();


    }

//    socket.disconnectFromHost();
//    socket.waitForDisconnected();
    socket.close();
}

QString ServerThread::magTrackerResponseBuilder()
{
    QLOG_ERROR() << "ServerThread::magTrackerResponseBuilder";
    QUrlQuery response;

    //response.addQueryItem("warning-message", "");
    response.addQueryItem("trackerid", SrvMetaInfo.metaStruct().trackerid);
    response.addQueryItem("interval", QString::number(requestInterval));
    response.addQueryItem("complete", QString::number(tracker->countClients()));
    //response.addQueryItem("incomplete", "");

    mInfoStr = tracker->getClient(SrvMetaInfo.infoHash());
    response.addQueryItem("peers", QString("peerid:%1;ip:%2;port:%3").arg(mInfoStr.peerid,mInfoStr.ip,QString::number(mInfoStr.port)));

    return response.toString();

}

