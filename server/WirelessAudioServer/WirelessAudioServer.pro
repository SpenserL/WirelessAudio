#-------------------------------------------------
#
# Project created by QtCreator 2016-03-30T15:05:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WirelessAudioServer
TEMPLATE = app
LIBS = -lws2_32

SOURCES += main.cpp\
        mainwindow.cpp \
    Server.cpp \
    ../../client/circularbuffer.cpp

HEADERS  += mainwindow.h \
    Server.h \
    ../../client/circularbuffer.h

FORMS    += mainwindow.ui
