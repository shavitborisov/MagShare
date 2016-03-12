QT += core network
QT -= gui

CONFIG += c++11

TARGET = Tracker
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    server.cpp \
    serverthread.cpp

HEADERS += \
    server.h \
    serverthread.h
