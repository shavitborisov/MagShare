#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    socket(new QTcpSocket(parent)) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::connectToTracker() {
    ui->console->append("connecting");

    QObject::connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    QObject::connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    socket->connectToHost("127.0.0.1", 1337);

    if (!socket->waitForConnected())
        ui->console->append("error connecting");
}

void MainWindow::connected() {
//    std::string cmd = ui->cmdBox->text().toStdString();

//    ui->console->append("connected, now saying hello");

//    socket->write(cmd);

//    socket->flush();
}

void MainWindow::disconnected() {
    ui->console->append("disconnected");
}

void MainWindow::bytesWritten(qint64 bytes) {
    ui->console->append("bytes written");
}

void MainWindow::readyRead() {
    ui->console->append("ready to read");

    ui->console->append(socket->readAll());
}

void MainWindow::on_sendCmd_clicked() {
    socket->write(ui->cmdBox->text().toStdString().data());
    socket->flush();
}
