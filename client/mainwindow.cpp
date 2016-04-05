#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Client.h"
#include <QDebug>
#include <io.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    //audioManager = new AudioManager(this);
    QRegExp regex;
    regex.setPattern("^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$");
    QValidator* val = new QRegExpValidator(regex, this);
    ui->ipAddr->setValidator(val);
    ui->ipAddr->setText("192.168.1.147");
}

MainWindow::~MainWindow() {
    delete ui;
    delete audioManager;
}

void MainWindow::on_playButton_released()
{
    audioManager->stop();
    QFile *file = new QFile(QFileDialog::getOpenFileName(this, tr("Pick A Song"), 0, tr("Music (*.wav)")));
    audioManager->loadSong(file);
    //audioManager->play();
}

void MainWindow::on_pauseButton_released()
{
    audioManager->pause();
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

void MainWindow::on_actionConnect_triggered()
{
    qDebug() << "Clicked";
    if (ClientSetup(ui->ipAddr->text().toLatin1().data()) == 0) {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->sendBtn->setEnabled(true);
    }
}

void MainWindow::on_actionDisconnect_triggered()
{
    ClientCleanup();
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->sendBtn->setEnabled(false);
}

void MainWindow::on_sendBtn_clicked()
{
    QFile *file = new QFile(QFileDialog::getOpenFileName(this, tr("Pick A Song To Send"), 0, tr("Music (*.wav)")));
    if (file->exists()) {
        file->open(QIODevice::ReadOnly);
        ClientSend((HANDLE) _get_osfhandle(file->handle()));
    }
}
