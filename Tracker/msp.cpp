#include "msp.h"

MSP::MSP(QObject* parent) : QObject(parent) { }

void MSP::listen() {
    connect(&server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    server.listen(QHostAddress::Any, 1337);
}

void MSP::newConnection() {
    QTcpSocket* socket = server.nextPendingConnection();

    qDebug() << "Received: " << socket->readAll();

    qDebug() << "Now sending back";
    socket->write("bla bla bla");
    socket->flush();
    socket->waitForBytesWritten();

    socket->close();
}
