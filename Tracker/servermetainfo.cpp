#include "servermetainfo.h"

ServerMetaInfo::ServerMetaInfo()
{
    clear();
}
void ServerMetaInfo::clear()
{
    errString = "";
    content.clear();
    infoData.clear();
//    metaInfoName.clear();
//    metaInfoPieceLength = 0;
//    metaInfoSha1Sums.clear();
}
QString ServerMetaInfo::errorString() const
{
    return errString;
}
QByteArray ServerMetaInfo::infoHash() const
{
    return infoData;
}
MetaInfoStruct ServerMetaInfo::metaStruct() const
{
    return metaInfo;
}
bool ServerMetaInfo::magTrackerQueryParser(QByteArray &data)
{

    clear();

    content = data;
    QUrlQuery query = QUrlQuery(content);
    if (!query.hasQueryItem("info_hash"))
    {
        errString = "info_hash not found in request";
        return false;
    }

    // info_hash is our unique key
    infoData = query.queryItemValue("info_hash").toLatin1();


    metaInfo.length = query.queryItemValue("mfile-length").toLongLong();
    metaInfo.name = query.queryItemValue("mfile-name");
    metaInfo.pieceLength = query.queryItemValue("mfile-piece-length").toInt();
    QByteArray pieces = query.queryItemValue("mfile-pieces").toLatin1();
    for (int i = 0; i < pieces.size(); i += 20)
        metaInfo.sha1Sums << pieces.mid(i, 20);

    metaInfo.peerid = query.queryItemValue("peer_id");
    metaInfo.ip = query.queryItemValue("ip");
    metaInfo.port = query.queryItemValue("port").toInt();

    QByteArray trackerid;

    if (query.hasQueryItem("trackerid"))
    {
        trackerid = query.queryItemValue("trackerid").toLatin1();
    }
    else
    {
        // Generate trackerid
        int startupTime = int(QDateTime::currentDateTime().toTime_t());

        trackerid += QString::asprintf("$magtrackerid$").toLatin1();
        trackerid += QByteArray::number(startupTime, 10);
        trackerid += QByteArray(20 - trackerid.size(), '$');
    }
    metaInfo.trackerid = trackerid;

    metaInfo.lastUpd = QDateTime();
    return true;
}
