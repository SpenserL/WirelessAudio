#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Client.h"
#include <QDebug>
#include <QAudioInput>
#include<QIODevice>
#include <QTimer>
#include <io.h>

QFile dFile;
QAudioInput * audio;
CircularBuffer * cb, *circularBufferRecv;
QBuffer *buffer;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    audioManager = new AudioManager(this);
    buffer = new QBuffer(parent);
    audioManager->Init(buffer);

    circularBufferRecv = new CircularBuffer(CIRCULARBUFFERSIZE, SERVER_PACKET_SIZE, this);
    QRegExp regex;
    regex.setPattern("^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$");
    QValidator* val = new QRegExpValidator(regex, this);
    ui->setupUi(this);
    ui->ipAddr->setValidator(val);
    ui->ipAddr->setText("192.168.0.6");
    //ui->ipAddr->setText("127.0.0.1");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete audioManager;

}

void MainWindow::on_playButton_released()
{
    //audioManager->stop();
    QFile *file = new QFile(QFileDialog::getOpenFileName(this, tr("Pick A Song"), 0, tr("Music (*.wav)")));
    audioManager->loadSong(file);
    audioManager->play();
}

void MainWindow::on_pauseButton_released()
{
    audioManager->pause();
}

void MainWindow::handleStateChanged(QAudio::State)
{

}

void MainWindow::on_resumeButton_released()
{
    audioManager->resume();
}

void MainWindow::on_skipForwardButton_released()
{
    audioManager->skip(10);
}

void MainWindow::on_skipBackwardsButton_released()
{
    audioManager->skip(-10);
}

void MainWindow::on_pushButton_clicked()
{
      QIODevice *QID;
      //QID->open( QIODevice::WriteOnly);
      QBuffer myQB;

     //QID(myQB);
    //cb(128000,64000);
     //dFile.setFileName("../RecordTest.raw");
     buffer->open( QIODevice::ReadWrite);
     QAudioFormat format;
     // Set up the desired format, for example:
     format.setSampleRate(16000);
     format.setChannelCount(1);
     format.setSampleSize(16);
     format.setCodec("audio/pcm");
     format.setByteOrder(QAudioFormat::LittleEndian);
     format.setSampleType(QAudioFormat::UnSignedInt);

     QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
     if (!info.isFormatSupported(format))
     {
         qWarning() << "Default format not supported, trying to use the nearest.";
         format = info.nearestFormat(format);
     }

     audio = new QAudioInput(format, this);
     connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));

     QTimer::singleShot(5000, this, SLOT(on_pushButton_2_clicked()));
     audio->start(buffer);
}

void MainWindow::on_pushButton_2_clicked()
{
    qDebug()<<"StopRecordTriggered";
    audio->stop();
    audioManager->playRecord();
    //dFile.close();
    //delete audio;
}

void MainWindow::on_connectOutgoing_clicked()
{
    if (ClientSendSetup(ui->ipAddr->text().toLatin1().data()) == 0)
    {
        ui->connectOutgoing->setEnabled(false);
        ui->ipAddr->setEnabled(false);
        ui->disconnectOutgoing->setEnabled(true);
        ui->sendBtn->setEnabled(true);
    }
}

void MainWindow::on_sendBtn_clicked()
{
    QFile *file = new QFile(QFileDialog::getOpenFileName(this, tr("Pick A Song To Send"), 0, tr("Music (*.wav)")));
    if (file->exists())
    {
        file->open(QIODevice::ReadOnly);
        ClientSend((HANDLE) _get_osfhandle(file->handle()));
    }
}

void MainWindow::on_disconnectOutgoing_clicked()
{
    ClientCleanup();
    ui->connectOutgoing->setEnabled(true);
    ui->ipAddr->setEnabled(true);
    ui->disconnectOutgoing->setEnabled(false);
    ui->sendBtn->setEnabled(false);
}

void MainWindow::on_openIncoming_clicked()
{
    if (ClientReceiveSetup() == 0)
    {
        QFile *file = new QFile(QFileDialog::getSaveFileName(this, tr("Save song as"), 0, tr("Music (*.wav)")));
        if (file->fileName() != NULL)
        {
            ui->openIncoming->setEnabled(false);
            ui->disconnectIncoming->setEnabled(true);
            file->open(QIODevice::WriteOnly);
            ClientListen((HANDLE) _get_osfhandle(file->handle()));
        } else
        {
            ClientCleanup();
        }
    }
}

void MainWindow::on_disconnectIncoming_clicked()
{
    ui->openIncoming->setEnabled(true);
    ui->disconnectIncoming->setEnabled(false);
    ClientCleanup();
}
