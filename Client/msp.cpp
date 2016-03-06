#include "msp.h"

MSP::MSP(QObject *parent) : QObject(parent) { }

void MSP::connect() {
    QObject::connect(&socket, SIGNAL(connected()), this, SLOT(connected()));
    QObject::connect(&socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    QObject::connect(&socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    QObject::connect(&socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    qDebug() << "connecting...";

    socket.connectToHost("127.0.0.1", 1337);
    if (!socket.waitForConnected())
        qDebug() << "error connecting";
}

void MSP::connected() {
    qDebug() << "connected, now saying hello";
    socket.write("hello server");
}

void MSP::disconnected() {
    qDebug() << "disconnected";
}

void MSP::bytesWritten(qint64 bytes) {
    qDebug() << bytes << " bytes written";
}

void MSP::readyRead() {
    qDebug() << "Ready to read data";

    qDebug() << "Data: " << socket.readAll();
}

