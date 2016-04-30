#ifndef TRACKERCLIENT_H
#define TRACKERCLIENT_H

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QHostAddress>
#include "metainfo.h"
#include "torrentclient.h"

#include <QTcpSocket>

class TorrentClient;

class TrackerClient : public QObject
{
    Q_OBJECT

public:
    explicit TrackerClient(TorrentClient *downloader, QObject *parent = 0);

    void start(const MetaInfo &info);
    void stop();
    void startSeeding();

public slots:
    void bytesWritten(qint64 bytes);
    void trackerResponse();

signals:
    //void connectionError(QNetworkReply::NetworkError error);
    void connectionError(QString error);

    void failure(const QString &reason);
    void warning(const QString &message);
    void peerListUpdated(const QList<TorrentPeer> &peerList);

    void uploadCountUpdated(qint64 newUploadCount);
    void downloadCountUpdated(qint64 newDownloadCount);

    void stopped();

protected:
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;

private slots:
    void fetchPeerList();
private:

    QTcpSocket* socket;

    TorrentClient *torrentDownloader;

    int requestInterval;
    int requestIntervalTimer;

    MetaInfo metaInfo;
    QByteArray trackerId;
    QList<TorrentPeer> peers;
    qint64 uploadedBytes;
    qint64 downloadedBytes;
    qint64 length;
    QString uname;
    QString pwd;

    bool firstTrackerRequest;
    bool lastTrackerRequest;
    bool firstSeeding;
};

#endif
