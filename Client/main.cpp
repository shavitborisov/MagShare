#include "mainwindow.h"
#include <QApplication>
#include <QtCore>
#include <QtGui>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QMessageBox::information(nullptr, "Window", "Text");

    return a.exec();
}
