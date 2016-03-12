#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    Server server;
    if (!server.listen(QHostAddress::Any, 1337)) {
        qDebug() << "error listening";
        return -1;
    }

    return a.exec();
}
