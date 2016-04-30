#include "mainwindow.h"
#include <QApplication>
#include <QtCore>
#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("MagShare Corp.");
    QApplication::setApplicationName("MagShare");

    MainWindow w;
    w.show();

    return app.exec();
}
