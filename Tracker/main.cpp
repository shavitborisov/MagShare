#include <QCoreApplication>
#include "msp.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    MSP server;
    server.listen();

    return a.exec();
}
