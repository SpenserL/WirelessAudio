#-------------------------------------------------
#
# Project created by QtCreator 2016-03-25T15:54:09
#
#-------------------------------------------------

QT += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WirelessAudio
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    audiomanager.cpp \
    circularbuffer.cpp

HEADERS  += mainwindow.h \
    audiomanager.h \
    wavheader.h \
    songstate.h \
    circularbuffer.h

FORMS    += mainwindow.ui

DISTFILES += \
    test.wav
