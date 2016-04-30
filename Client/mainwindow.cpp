#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "aboutdialog.h"
#include "settingsdialog.h"
#include <settings.h>
#include <QsLog/QsLog.h>
#include <QsLog/QsLogDest.h>
#include "torrentclient.h"
#include "metainfo.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QUrlQuery>
#include <QCryptographicHash>


bool isConn = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    Settings::load();
    lastDirectory = Settings::msfFolder();

    using namespace QsLogging;

    // 1. init the logging mechanism
    Logger& logger = Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);
    // 2. add two destinations
    DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
     Settings::logFile(), EnableLogRotation, MaxSizeBytes(102400), MaxOldLogCount(3))); //max log size 100KB, 3 rolling files created

    DestinationPtr debugDestination(DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination);
    logger.addDestination(fileDestination);
    // 3. start logging
    QLOG_INFO() << "*** MagShare Client started ***";

    QLOG_INFO() << "Loading settings from" << Settings::INI_FILE << ": Tracker host:" << Settings::trackerHost() << ", port:" << QString::number(Settings::trackerPort());
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::torrentStart() {

    QLOG_INFO() << "Start connecting to Tracker";

    /* currently always the same file */
    QString fileName = ui->lineFile->text();
    if (fileName.isEmpty())
    {
        QLOG_ERROR() << "MagShare File " << fileName << " can't be found";
        return;
    }

    QLOG_INFO() << "MagShare file:" << fileName << ", destination:" << Settings::destinationFolder();

    addTorrent(fileName,Settings::destinationFolder());

    isConn = true;

}

bool MainWindow::addTorrent(const QString &fileName, const QString &destinationFolder,
                            const QByteArray &resumeState)
{
    QLOG_DEBUG() << "MainWindow::addTorrent";

    // Check if the torrent is already being downloaded.
    foreach (Job job, jobs) {
        if (job.torrentFileName == fileName && job.destinationDirectory == destinationFolder) {
            QLOG_WARN() << "MagShare file " << fileName << " is already being downloaded.";
            return false;
        }
    }

    // Create a new torrent client and attempt to parse the torrent data.
    TorrentClient *client = new TorrentClient(this);
    if (!client->setTorrent(fileName)) {
        QLOG_ERROR() << "MagShare file " << fileName << " cannot not be opened/resumed (" << client->errorString() << ")" ;
        delete client;
        return false;
    }
    client->setDestinationFolder(destinationFolder);
    client->setDumpedState(resumeState);

    // Setup the client connections.
    connect(client, SIGNAL(stateChanged(TorrentClient::State)), this, SLOT(updateState(TorrentClient::State)));
    connect(client, SIGNAL(peerInfoUpdated()), this, SLOT(updatePeerInfo()));
    connect(client, SIGNAL(progressUpdated(int)), this, SLOT(updateProgress(int)));
    connect(client, SIGNAL(downloadRateUpdated(int)), this, SLOT(updateDownloadRate(int)));
    connect(client, SIGNAL(uploadRateUpdated(int)), this, SLOT(updateUploadRate(int)));

    connect(client, SIGNAL(error(TorrentClient::Error)), this, SLOT(torrentError(TorrentClient::Error)));
    connect(client, SIGNAL(stopped()), this, SLOT(torrentStopped()));

    // Add the client to the list of downloading jobs.
    Job job;
    job.client = client;
    job.torrentFileName = fileName;
    job.destinationDirectory = destinationFolder;
    jobs << job;

    client->start();
    return true;
}
int MainWindow::rowOfClient(TorrentClient *client) const
{
    // Return the row that displays this client's status, or -1 if the
    // client is not known.
    int row = 0;
    foreach (Job job, jobs) {
        if (job.client == client)
            return row;
        ++row;
    }
    return -1;
}

void MainWindow::removeTorrent()
{
    // Find the row of the current item, and find the torrent client
    // for that row.

    QLOG_DEBUG() << "MainWindow::removeTorrent";

    // for now let's use count as there is only one job
    int row = jobs.count() - 1;
    if (row >= 0)
    {

    TorrentClient *client = jobs.at(row).client;

    // Stop the client.
    client->disconnect();
    connect(client, SIGNAL(stopped()), this, SLOT(torrentStopped()));
    client->stop();

    // Remove the row from the jobs
    jobs.removeAt(row);
    }
}

void MainWindow::torrentStopped()
{
    QLOG_DEBUG() << "MainWindow::torrentStopped";

    // Schedule the client for deletion.
    TorrentClient *client = qobject_cast<TorrentClient *>(sender());

    // ?
    client->stop();

    client->deleteLater();

}

void MainWindow::torrentError(TorrentClient::Error)
{
    // Delete the client.
    TorrentClient *client = qobject_cast<TorrentClient *>(sender());

    QString fileName = client->metaInfo().singleFile().name;

    // Display the warning.
    QLOG_ERROR() << "An error occurred while downloading " << fileName << ": "  << client->errorString();

    client->deleteLater();
}

void MainWindow::updateState(TorrentClient::State)
{
    // Update the state string whenever the client's state changes.
    TorrentClient *client = qobject_cast<TorrentClient *>(sender());
    int row = rowOfClient(client);

    QLOG_DEBUG() << "MainWindow::updateState row:" << row;

    ui->lineStatus->setText(client->stateString());


    /*

    //headers << tr("Torrent") << tr("Peers/Seeds") << tr("Progress")  << tr("Down rate") << tr("Up rate") << tr("Status");

    QTreeWidgetItem *item = torrentView->topLevelItem(row);
    if (item) {
        item->setToolTip(0, tr("Torrent: %1<br>Destination: %2<br>State: %3")
                         .arg(jobs.at(row).torrentFileName)
                         .arg(jobs.at(row).destinationDirectory)
                         .arg(client->stateString()));

        item->setText(5, client->stateString());
    }
    setActionsEnabled();
    */
}

void MainWindow::updatePeerInfo()
{
    // Update the number of connected, visited, seed and leecher peers.
    TorrentClient *client = qobject_cast<TorrentClient *>(sender());
    int row = rowOfClient(client);

    QLOG_DEBUG() << "MainWindow::updatePeerInfo row:" << row;

    ui->lineSeeds->setText(QString::number(client->seedCount()));
    /*
    QTreeWidgetItem *item = torrentView->topLevelItem(row);
    item->setText(1, tr("%1/%2").arg(client->connectedPeerCount())
                  .arg(client->seedCount()));
    */
}

void MainWindow::updateProgress(int percent)
{
    TorrentClient *client = qobject_cast<TorrentClient *>(sender());
    int row = rowOfClient(client);

    QLOG_DEBUG() << "MainWindow::updateProgress row:" << row;
    ui->linePercent->setText(QString::number(percent));

    /*
    // Update the progressbar.
    QTreeWidgetItem *item = torrentView->topLevelItem(row);
    if (item)
        item->setText(2, QString::number(percent));

    */
}

void MainWindow::updateDownloadRate(int bytesPerSecond)
{
    // Update the download rate.
    TorrentClient *client = qobject_cast<TorrentClient *>(sender());
    int row = rowOfClient(client);

    QLOG_DEBUG() << "MainWindow::updateDownloadRate row:" << row << " rate:" << bytesPerSecond;

    /*
    const QString num = QString::asprintf("%.1f KB/s", bytesPerSecond / 1024.0);
    torrentView->topLevelItem(row)->setText(3, num);

    if (!saveChanges) {
        saveChanges = true;
        QTimer::singleShot(5000, this, SLOT(saveSettings()));
    }
    */
}

void MainWindow::updateUploadRate(int bytesPerSecond)
{
    // Update the upload rate.
    TorrentClient *client = qobject_cast<TorrentClient *>(sender());
    int row = rowOfClient(client);

    QLOG_DEBUG() << "MainWindow::updateUploadRate row:" << row << " rate:" << bytesPerSecond;

    /*
    const QString num = QString::asprintf("%.1f KB/s", bytesPerSecond / 1024.0);
    torrentView->topLevelItem(row)->setText(4, num);

    if (!saveChanges) {
        saveChanges = true;
        QTimer::singleShot(5000, this, SLOT(saveSettings()));
    }
    */
}


void MainWindow::torrentStop() {

    removeTorrent();

    QLOG_INFO() << "Successfully disconnected from Tracker";
    isConn = false;
}

void MainWindow::on_pushStart_clicked()
{
    torrentStart();
    ui->pushStop->setEnabled(isConn);
    ui->pushStart->setEnabled(!isConn);
}

void MainWindow::on_pushStop_clicked()
{
    torrentStop();
    ui->pushStart->setEnabled(!isConn);
    ui->pushStop->setEnabled(isConn);

}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog;
    dialog.exec();
}

void MainWindow::on_actionExit_triggered()
{
    QLOG_INFO() << "*** MagShare Client ended ***";
    QsLogging::Logger::destroyInstance();

    Settings::save();

    MainWindow::close();
}


void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog dialog;
    dialog.exec();
}

void MainWindow::on_actionAdd_MagFile_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose a MagShare file"),
                                                    lastDirectory,
                                                    tr("Magshare Files (*.msf);;"
                                                       " All files (*.*)"));
    if (!fileName.isEmpty())
    {
        ui->lineFile->setText(fileName);
        lastDirectory = QFileInfo(fileName).absolutePath();
    }

}

void MainWindow::on_actionCreate_MagFile_triggered()
{
    int mFilePieceLength = 0; // bytes
    int mFileLength = 0;
    int mFileNumPieces = 0;
    QByteArray inblob;
    QByteArray oneHash;
    QString allHash;


    QString inFileName = QFileDialog::getOpenFileName(this, tr("Choose a file to share"),
                                                    Settings::destinationFolder(), " All files (*.*)");
    if (!inFileName.isEmpty())
    {

        QUrlQuery msf = QUrlQuery();
        msf.addQueryItem("mfile-name",QFileInfo(inFileName).fileName());
        mFileLength = QFileInfo(inFileName).size();
        msf.addQueryItem("mfile-length",QString::number(mFileLength));



        if ((mFileLength > 102400)&(mFileLength < 1048576))
        {
               // from 100KB to 1MB
               mFilePieceLength = 102400; //100KB
        }
        else if (mFileLength > 1048576)
                mFilePieceLength = 1048576; //1MB
        else
               mFilePieceLength = 512;

        msf.addQueryItem("mfile-piece-length", QString::number(mFilePieceLength));


        QFile infile(inFileName);
        if (!infile.open(QIODevice::ReadOnly))
        {
            QLOG_ERROR() << "Failed to open file " << infile.fileName() << ": " << infile.errorString() ;
            return;
        }
        else
        {
            inblob = infile.readAll();
        }

        for (int i = 0; i < mFileLength; i += mFilePieceLength)
        {
            mFileNumPieces++;
            // first SHA1 hash
            oneHash = QCryptographicHash::hash(inblob.mid(i,mFilePieceLength), QCryptographicHash::Sha1);
            // second Base64 encoding - to use strings without worrying
            allHash += oneHash.toBase64(QByteArray::Base64Encoding);

        }
        msf.addQueryItem("mfile-num-pieces", QString::number(mFileNumPieces));
        msf.addQueryItem("mfile-pieces", allHash );


        QString outFileName = QFileDialog::getSaveFileName(this,"Save new MagShare file", Settings::msfFolder(),"Magshare Files (*.msf);");
        if (!outFileName.isEmpty())
        {
            QFile *outfile = new QFile(outFileName);
            if (!outfile->open(QIODevice::WriteOnly)) {

                QLOG_ERROR() << "Failed to write into file " << outfile->fileName() << ": " << outfile->errorString() ;
                delete outfile;
                return;
            }
            else
            {
                QTextStream out(outfile);
                out << msf.toString();
                outfile->close();
                return;
            }

        }
    }
    else return;

}
