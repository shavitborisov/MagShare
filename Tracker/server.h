#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>

class Server : public QTcpServer {
public:
    Server();

protected:
    void incomingConnection(int descriptor) override;
};

#endif // SERVER_H
