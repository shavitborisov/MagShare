QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp \
    settingsdialog.cpp \
    settings.cpp \
    connectionmanager.cpp \
    filemanager.cpp \
    metainfo.cpp \
    peerwireclient.cpp \
    torrentclient.cpp \
    torrentserver.cpp \
    trackerclient.cpp \
    ratecontroller.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    settingsdialog.h \
    settings.h \
    connectionmanager.h \
    filemanager.h \
    metainfo.h \
    peerwireclient.h \
    torrentclient.h \
    torrentserver.h \
    trackerclient.h \
    ratecontroller.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    settings.ui
INCLUDEPATH += $$PWD/../

LIBS += -L$$PWD/../QsLog/build-QsLogShared
win32 {
    LIBS += -lQsLog2
} else {
    LIBS += -lQsLog
}
