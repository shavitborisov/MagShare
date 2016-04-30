#include "settings.h"
#include <QSettings>

int Settings::theTrackerPort;
QString Settings::theTrackerHost;
QString Settings::theLogFile;
QString Settings::theLogLevel;
QString Settings::theDestinationFolder;
QString Settings::theMSFFolder;

const QString Settings::INI_FILE = "magshare.conf";

void Settings::load() {
    QSettings settings(INI_FILE, QSettings::IniFormat);

    settings.beginGroup("Tracker");
    theTrackerHost = settings.value("Host", "localhost").toString();
    theTrackerPort = settings.value("Port", 1337).toInt();
    settings.endGroup();

    settings.beginGroup("Local");
    theLogFile = settings.value("LogFile", "magshare.log").toString();
    theLogLevel = settings.value("LogLevel", "TRACE").toString();
    theDestinationFolder = settings.value("DestinationFolder","").toString();
    theMSFFolder = settings.value("MSFFolder","").toString();
    settings.endGroup();

}

void Settings::save() {
    QSettings settings(INI_FILE, QSettings::IniFormat);

    settings.beginGroup("Tracker");
    settings.setValue("Host", theTrackerHost);
    settings.setValue("Port", theTrackerPort);
    settings.endGroup();

    settings.beginGroup("Local");
    settings.setValue("LogFile", theLogFile);
    settings.setValue("LogLevel", theLogLevel);
    settings.setValue("DestinationFolder", theDestinationFolder);
    settings.setValue("MSFFolder", theMSFFolder);
    settings.endGroup();
}

void Settings::assign(QString aHost, int aPort, QString aFile, QString aLevel, QString aFolder, QString aMSF){
    theTrackerHost = aHost;
    theTrackerPort = aPort;
    theLogFile = aFile;
    theLogLevel = aLevel;
    theDestinationFolder = aFolder;
    theMSFFolder = aMSF;
}
