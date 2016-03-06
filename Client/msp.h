#ifndef MSP_H
#define MSP_H

#include <QObject>
#include <QTcpSocket>

class MSP : public QObject {
    Q_OBJECT

public:
    explicit MSP(QObject* parent = nullptr);
    void connect();

public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();

private:
    QTcpSocket socket;
};

#endif // MSP_H
