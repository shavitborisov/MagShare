QT += core network
QT -= gui

CONFIG += c++11

TARGET = Tracker
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    server.cpp \
    serverthread.cpp \
    servermetainfo.cpp

HEADERS += \
    server.h \
    serverthread.h \
    servermetainfo.h

INCLUDEPATH += $$PWD/../

LIBS += -L$$PWD/../QsLog/build-QsLogShared
win32 {
    LIBS += -lQsLog2
} else {
    LIBS += -lQsLog
}
