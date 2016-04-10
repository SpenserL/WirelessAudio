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
<<<<<<< HEAD
=======
    Server.cpp \
>>>>>>> ea4a6d8aa89bb9ea7a1cf5c7b52d68764ea98d44
    ../../client/circularbuffer.cpp \
    serverreceive.cpp \
    serversend.cpp

HEADERS  += mainwindow.h \
    Server.h \
    ../../client/circularbuffer.h

FORMS    += mainwindow.ui
