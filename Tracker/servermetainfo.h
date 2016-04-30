#ifndef SERVERMETAINFO_H
#define SERVERMETAINFO_H

#include <QtCore>
#include <QByteArray>
#include <QDateTime>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>

struct MetaInfoStruct
{
    qint64 length;
    QString name;
    int pieceLength;
    QList<QByteArray> sha1Sums;
    QString peerid;
    int port;
    QString ip;
    QByteArray trackerid;
    QDateTime lastUpd;
};

class ServerMetaInfo
{
public:
    ServerMetaInfo();
    void clear();
    QString errorString() const;
    QByteArray infoHash() const;
    MetaInfoStruct metaStruct() const;

    bool magTrackerQueryParser (QByteArray &data);

private:

    MetaInfoStruct metaInfo;
    QString errString;
    QByteArray content;
    QByteArray infoData;
//    QString metaInfoName;
//    int metaInfoPieceLength;
//    QList<QByteArray> metaInfoSha1Sums;
};

#endif // SERVERMETAINFO_H
