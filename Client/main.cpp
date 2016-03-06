#include "mainwindow.h"
#include <QApplication>
#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include "msp.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    MSP msp;
    msp.connect();

    return a.exec();
}
