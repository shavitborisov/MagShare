#include "serverthread.h"
#include "qtcpsocket.h"

ServerThread::ServerThread(int descriptor, QObject *parent) : QThread(parent), descriptor(descriptor) { }

void ServerThread::run() {
    QTcpSocket socket;

    if (!socket.setSocketDescriptor(descriptor)) {
        qDebug() << "socket error!";
        return;
    }

    while (socket.waitForReadyRead()) {
        auto data = socket.readAll();
        auto msg = QString("Tracker received command: %1").arg(QString(data));

        qDebug() << "The data: " << data;
        socket.write(msg.toStdString().data());
    }

//    socket.disconnectFromHost();
//    socket.waitForDisconnected();
    socket.close();
}
