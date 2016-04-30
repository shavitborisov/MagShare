#include "metainfo.h"

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <QUrlQuery>
#include "settings.h"

MetaInfo::MetaInfo()
{
    clear();
}

void MetaInfo::clear()
{
    errString = "Unknown error";
    content.clear();
    infoData.clear();
    metaInfoMultiFiles.clear();
    metaInfoAnnounce.clear();
    metaInfoAnnounceList.clear();
    metaInfoCreationDate = QDateTime();
    metaInfoComment.clear();
    metaInfoCreatedBy.clear();
    metaInfoName.clear();
    metaInfoPieceLength = 0;
    metaInfoSha1Sums.clear();
}
/* our simple parser for now */
bool MetaInfo::magparse(const QByteArray &data)
{
    clear();
    content = data;
    QUrlQuery query = QUrlQuery(content);

    if (!query.hasQueryItem("mfile-name"))
    {
        errString = "mfile-name not found in .msf";
        return false;
    }

    // since we do not have anything but info only
    //infoData = query.toString().toLatin1();

    infoData = query.query(QUrl::EncodeReserved).toUtf8();

    metaInfoFileForm = SingleFileForm;
    metaInfoSingleFile.length = query.queryItemValue("mfile-length").toLongLong();
    metaInfoSingleFile.md5sum = 0;
    metaInfoSingleFile.name = query.queryItemValue("mfile-name");
    metaInfoSingleFile.pieceLength = query.queryItemValue("mfile-piece-length").toInt();

    QByteArray tmp = query.queryItemValue("mfile-pieces",QUrl::FullyEncoded).toUtf8();

    QByteArray pieces = QByteArray::fromBase64(tmp, QByteArray::Base64Encoding);

    for (int i = 0; i < pieces.size(); i += 20)
        metaInfoSingleFile.sha1Sums << pieces.mid(i, 20);

    metaInfoAnnounce = "";

    return true;

}

QByteArray MetaInfo::infoValue() const
{
    return infoData;
}

QString MetaInfo::errorString() const
{
    return errString;
}

MetaInfo::FileForm MetaInfo::fileForm() const
{
    return metaInfoFileForm;
}

QString MetaInfo::announceUrl() const
{
    return metaInfoAnnounce;
}

QStringList MetaInfo::announceList() const
{
    return metaInfoAnnounceList;
}

QDateTime MetaInfo::creationDate() const
{
    return metaInfoCreationDate;
}

QString MetaInfo::comment() const
{
    return metaInfoComment;
}

QString MetaInfo::createdBy() const
{
    return metaInfoCreatedBy;
}

MetaInfoSingleFile MetaInfo::singleFile() const
{
    return metaInfoSingleFile;
}

QList<MetaInfoMultiFile> MetaInfo::multiFiles() const
{
    return metaInfoMultiFiles;
}

QString MetaInfo::name() const
{
    return metaInfoName;
}

int MetaInfo::pieceLength() const
{
    return metaInfoPieceLength;
}

QList<QByteArray> MetaInfo::sha1Sums() const
{
    return metaInfoSha1Sums;
}

qint64 MetaInfo::totalSize() const
{
    if (fileForm() == SingleFileForm)
        return singleFile().length;

    qint64 size = 0;
    foreach (MetaInfoMultiFile file, multiFiles())
        size += file.length;
    return size;
}
