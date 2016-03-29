#include "audiomanager.h"

//Carson
AudioManager::AudioManager(QObject * par) : parent(par) {
  audio = new QAudioOutput(QAudioFormat(), par);
  songState = Stopped;
}

AudioManager::~AudioManager() {
    delete audio;
}

void AudioManager::loadSong(QFile * f) {
    if (songState != Stopped) {
        stop();
    }

    file = f;
    wav_hdr wavHeader;
    file->open(QIODevice::ReadOnly);
    int bytesRead = file->read((char*)&wavHeader, sizeof(wav_hdr));
    file->close();
    if (bytesRead == -1) {
        qDebug() << "[ERROR] audiomanager.cpp's loadSong()";
        return;
    }

    format.setSampleRate(wavHeader.SamplesPerSec);
    format.setChannelCount(wavHeader.NumOfChan);
    format.setSampleSize(wavHeader.bitsPerSample);

    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    audio = new QAudioOutput(format, parent);
    audio->setVolume(volume);
    songState = Stopped;
}

void AudioManager::pause() {
    audio->suspend();
    songState = Paused;
}

void AudioManager::resume() {
    audio->resume();
    songState = Playing;
}

void AudioManager::stop() {
    if (songState == Stopped)
        return;
    audio->stop();
    file->close();
    delete audio;
    songState = Stopped;
}

void AudioManager::skip(float seconds) {

}

QIODevice * AudioManager::play() {
    file->open(QIODevice::ReadOnly);
    device = file;
    audio->start(device);
    return device;
}

void AudioManager::setVolume(float vol) {
    volume = vol;
    audio->setVolume(volume);
}
