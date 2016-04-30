#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "torrentclient.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void torrentStart();
    void torrentStop();

private slots:

    // menus
    void on_actionAbout_triggered();
    void on_actionExit_triggered();
    void on_actionSettings_triggered();
    void on_actionAdd_MagFile_triggered();
    void on_actionCreate_MagFile_triggered();

    // buttons
    void on_pushStart_clicked();
    void on_pushStop_clicked();

    // torrent
    bool addTorrent(const QString &fileName, const QString &destinationFolder,
                    const QByteArray &resumeState = QByteArray());
    void removeTorrent();
    void torrentError(TorrentClient::Error error);
    void torrentStopped();

    void updateState(TorrentClient::State state);
    void updatePeerInfo();
    void updateProgress(int percent);
    void updateDownloadRate(int bytesPerSecond);
    void updateUploadRate(int bytesPerSecond);

private:
    Ui::MainWindow *ui;

    int rowOfClient(TorrentClient *client) const;
    int uploadLimit;
    int downloadLimit;
    struct Job {
        TorrentClient *client;
        QString torrentFileName;
        QString destinationDirectory;
    };

    QList<Job> jobs;
    int jobsStopped;
    int jobsToStop;

    QString lastDirectory;

};

#endif // MAINWINDOW_H
