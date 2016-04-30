#ifndef SETTINGS_H
#define SETTINGS_H
#include <QString>


class Settings {
public:
    static void load();
    static void save();
    static void assign(QString host, int aPort, QString aFile, QString aLevel, QString aFolder, QString aMSF);

    static int trackerPort() {
        return theTrackerPort;
    }

    static QString trackerHost() {
        return theTrackerHost;
    }

    static QString logFile() {
        return theLogFile;
    }

    static QString logLevel() {
        return theLogLevel;
    }

    static QString destinationFolder() {
        return theDestinationFolder;
    }

    static QString msfFolder() {
        return theMSFFolder;
    }

    static const QString INI_FILE;

private:
    static int theTrackerPort;

    static QString theTrackerHost;

    static QString theLogFile;

    static QString theLogLevel;

    static QString theDestinationFolder;

    static QString theMSFFolder;
};


#endif // SETTINGS_H
