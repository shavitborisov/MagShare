#ifndef MSP_H
#define MSP_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class MSP : public QObject
{
    Q_OBJECT

public:
    explicit MSP(QObject* parent = nullptr);
    void listen();

public slots:
    void newConnection();

private:
    QTcpServer server;
};

#endif // MSP_H
