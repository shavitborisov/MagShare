QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    msp.cpp

HEADERS  += mainwindow.h \
    msp.h

FORMS    += mainwindow.ui
