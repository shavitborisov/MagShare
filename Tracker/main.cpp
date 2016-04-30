#include <QCoreApplication>
#include <QtCore>
#include "server.h"
#include <QsLog/QsLog.h>
#include <QsLog/QsLogDest.h>
#include <windows.h>

static const int TrackerPort = 1337;
static const QString LogFile = "magshare_tracker.log";

// Forced to add Windows-specific code
static BOOL WINAPI exitHandler(DWORD type) {
    if (type == CTRL_CLOSE_EVENT || type == CTRL_C_EVENT) {
        QLOG_INFO() << "About to quit...";
        QCoreApplication::quit();
        return TRUE;
    }

    return FALSE;
}

int main(int argc, char *argv[]) {
    auto trackerApp = new QCoreApplication(argc, argv);

    Server server;
    QCoreApplication::setApplicationName("MagShare Tracker");

    using namespace QsLogging;

    // 1. init the logging mechanism
    Logger& logger = Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);
    // 2. add two destinations
    DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
     LogFile, EnableLogRotation, MaxSizeBytes(102400), MaxOldLogCount(3))); //max log size 100KB, 3 rolling files created

    DestinationPtr debugDestination(DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination);
    logger.addDestination(fileDestination);
    // 3. start logging
    QLOG_INFO() << "*** MagShare Tracker started ***";

    if (!server.listen(QHostAddress::AnyIPv4, TrackerPort)) {
        qDebug() << "error listening";
        QLOG_ERROR() << "Error listening on port " << TrackerPort;
        return -1;
    }
    else {
        qDebug() << "Tracker server listening on port " << TrackerPort;
        QLOG_INFO() << "Tracker server listening on port " << TrackerPort;
    }

    // Forced to add Windows-specific code
    SetConsoleCtrlHandler(exitHandler, TRUE);

    QObject::connect(trackerApp, &QCoreApplication::aboutToQuit, [] {
        QLOG_INFO() << "*** MagShare Tracker stopped ***";
    });


    int ret = trackerApp->exec();

    return ret;
}

