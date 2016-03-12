#include "server.h"
#include "serverthread.h"

Server::Server() : QTcpServer() { }

void Server::incomingConnection(int descriptor) {
    auto thread = new ServerThread(descriptor, this);

    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
