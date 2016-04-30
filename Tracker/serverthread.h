#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QObject>
#include <QThread>

class ServerThread : public QThread {
public:
    ServerThread(int descriptor, QObject* parent);
    void run() override;
    bool magTrackerQueryParser(QByteArray &data);
    QString magTrackerResponseBuilder();

private:
    int descriptor;
};

#endif // SERVERTHREAD_H
