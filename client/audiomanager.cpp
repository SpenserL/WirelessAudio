#include "audiomanager.h"

//Carson
AudioManager::AudioManager(QObject * par) {
  parent = par;
  songState = Stopped;
  circularBuffer = new CircularBuffer(CIRCULARBUFFERSIZE, BUFFERSIZE, par);
  buffer = new QBuffer(parent);
  qRegisterMetaType<wav_hdr>("wav_hdr");

  populateBufferWorker = new PopulateBufferWorker(circularBuffer, buffer);
  populateBufferWorker->moveToThread(&populateBufferThread);
  connect(&populateBufferThread, SIGNAL(started()), populateBufferWorker, SLOT(doWork()));
  connect(&populateBufferThread, &QThread::finished, populateBufferWorker, &QObject::deleteLater);
  populateBufferThread.start();
}

AudioManager::~AudioManager() {
    delete audio;
    delete circularBuffer;
    delete buffer;
    delete readFileWorker;
    delete populateBufferWorker;
}

void AudioManager::loadSong(QFile * f) {
    char data[BUFFERSIZE];
    if (songState != Stopped) {
        stop();
    }
    readFileWorker = new ReadFileWorker(f, circularBuffer);
    readFileWorker->moveToThread(&readWorkerThread);
    connect(&readWorkerThread, &QThread::finished, readFileWorker, &QObject::deleteLater);
    connect(&readWorkerThread, SIGNAL(started()), readFileWorker, SLOT(doWork()));
    connect(readFileWorker, SIGNAL(gotWavHeader(wav_hdr)), this, SLOT(receivedWavHeader(wav_hdr)));
    readWorkerThread.start();
}

void AudioManager::receivedWavHeader(wav_hdr wavHeader) {
    qDebug() << "Received Header";
    bytesPerSecond = wavHeader.bytesPerSec;
    format.setSampleRate(wavHeader.SamplesPerSec);
    format.setChannelCount(wavHeader.NumOfChan);
    format.setSampleSize(wavHeader.bitsPerSample);

    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    audio = new QAudioOutput(format, parent);
    audio->setVolume(volume);
    songState = Playing;
    play();
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
    //pause();
    int curPos = buffer->pos();
    int newPos = curPos + seconds * bytesPerSecond;
    if (newPos < 0)
        newPos = 0;
    if (newPos > buffer->size()-2)
        newPos = buffer->size()-2;
    buffer->seek(newPos);
    //resume();
}

QIODevice * AudioManager::play() {
    device = buffer;
    audio->start(device);
    qDebug() << "Playing device, current buffered amount: " << buffer->size();
    return device;
}

void AudioManager::setVolume(float vol) {
    volume = vol;
    audio->setVolume(volume);
}
