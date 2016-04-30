#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <servermetainfo.h>

class Server : public QTcpServer {
    Q_OBJECT
public:
    Server();

    static void addClient(const QByteArray& infoHash, const MetaInfoStruct& minfostruct);
    static void removeClient(const QByteArray& infoHash);
    static MetaInfoStruct getClient(const QByteArray& infoHash);
    static int countClients();

protected:
    void incomingConnection(int descriptor) override;

private:
    static QMap<QByteArray, MetaInfoStruct> clients;
};

#endif // SERVER_H
