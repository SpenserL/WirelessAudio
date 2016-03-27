#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QAudioFormat>
#include <QAudioOutput>
#include <QFile>
#include <QDebug>
#include "wavheader.h"

class AudioManager {
public:
    AudioManager(QObject * parent);
    ~AudioManager();

    void pause();
    void resume();
    void skip(float seconds);
    QIODevice * play();
    void setVolume(float volume);
    void loadSong(QFile * file);
    void stop();

private:
    QAudioFormat format;
    QAudioOutput *audio;
    QObject *parent;
    QFile *file;
    QIODevice *device;
    float volume = 10;
};

#endif // AUDIOMANAGER_H
