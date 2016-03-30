#include "audiomanager.h"

//Carson
AudioManager::AudioManager(QObject * par) : parent(par) {
  songState = Stopped;
  circularBuffer = new CircularBuffer(CIRCULARBUFFERSIZE, BUFFERSIZE, par);
  //for(int i = 0; i < 100000; i++)
  //  circularBuffer->pushBack((void*)"1");

  buffer = new QBuffer(parent);
  //while(circularBuffer->pop(buffer));
  /*while(circularBuffer->pop(buffer)) {
    char data[BUFFERSIZE];
    buffer->seek(0);
    int size = buffer->read(data, BUFFERSIZE);
    if (size > 0) {
        qDebug() << "Message:" << data;
    }
  }*/
}

AudioManager::~AudioManager() {
    delete audio;
    delete circularBuffer;
    delete buffer;
}

void AudioManager::loadSong(QFile * f) {
    char data[BUFFERSIZE];
    if (songState != Stopped) {
        stop();
    }

    file = f;
    wav_hdr wavHeader;
    file->open(QIODevice::ReadOnly);
    buffer->open(QIODevice::WriteOnly);
    //buffer->open(QIODevice::ReadOnly);
    int bytesRead = file->read((char*)&wavHeader, sizeof(wav_hdr));
    while(file->read(data, BUFFERSIZE) > 0) {
        circularBuffer->pushBack(data);
        circularBuffer->pop(buffer);
    }
    file->close();
    buffer->close();
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
    buffer->open(QIODevice::ReadOnly);
    device = buffer;
    audio->start(device);
    return device;
}

void AudioManager::setVolume(float vol) {
    volume = vol;
    audio->setVolume(volume);
}
