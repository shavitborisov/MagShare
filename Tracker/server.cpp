#include "server.h"
#include "serverthread.h"

QMap<QByteArray, MetaInfoStruct> Server::clients;

Server::Server() : QTcpServer() { }

void Server::incomingConnection(int descriptor) {
    auto thread = new ServerThread(descriptor, this);

    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();


}
void Server::addClient(const QByteArray& infoHash, const MetaInfoStruct& minfostruct){
    clients.insert(infoHash,minfostruct);
}

void Server::removeClient(const QByteArray &infoHash){
    clients.remove(infoHash);
}

MetaInfoStruct Server::getClient(const QByteArray &infoHash){
    MetaInfoStruct myInfo;

    if (clients.contains(infoHash))
          myInfo = clients.value(infoHash);

    return myInfo;
    //clients.find(infoHash);
}

int Server::countClients(){
    return clients.count();
}
