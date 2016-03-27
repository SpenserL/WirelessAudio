#include "audiomanager.h"

AudioManager::AudioManager(QObject * par) : parent(par) {
  audio = new QAudioOutput(QAudioFormat(), par);
}

AudioManager::~AudioManager() {
    delete audio;
}

void AudioManager::loadSong(QFile * f) {
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
}

void AudioManager::pause() {
    audio->suspend();
}

void AudioManager::resume() {
    audio->resume();
}

void AudioManager::stop() {
    audio->stop();
    file->close();
    delete audio;
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
