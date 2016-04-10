#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Server.h"
#include <QDebug>
#include <QFileDialog>
#include <io.h>

CircularBuffer *circularBufferRecv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    circularBufferRecv = new CircularBuffer(CIRCULARBUFFERSIZE, CLIENT_PACKET_SIZE, this);
    QRegExp regex;
    regex.setPattern("^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$");
    QValidator* val = new QRegExpValidator(regex, this);
    ui->setupUi(this);
    ui->ipAddr->setValidator(val);
    ui->ipAddr->setText("192.168.0.5");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openIncoming_clicked()
{
    if (ServerReceiveSetup() == 0)
    {
        QFile *file = new QFile(QFileDialog::getSaveFileName(this, tr("Pick The Destination Song Name"), 0, tr("Music (*.wav)")));
        if (file->fileName() != NULL)
        {
            ui->openIncoming->setEnabled(false);
            ui->disconnectIncoming->setEnabled(true);
            file->open(QIODevice::WriteOnly);
            ServerListen((HANDLE) _get_osfhandle(file->handle()));
        } else
        {
            ServerCleanup();
        }
    }
}

void MainWindow::on_disconnectIncoming_clicked()
{
    ui->openIncoming->setEnabled(true);
    ui->disconnectIncoming->setEnabled(false);
    ServerCleanup();
}

void MainWindow::on_connectOutgoing_clicked()
{
    if (ServerSendSetup(ui->ipAddr->text().toLatin1().data()) == 0)
    {
        ui->connectOutgoing->setEnabled(false);
        ui->disconnectOutgoing->setEnabled(true);
        ui->sendBtn->setEnabled(true);
    }
}

void MainWindow::on_sendBtn_clicked()
{
    QFile *file = new QFile(QFileDialog::getOpenFileName(this, tr("Pick a Song to Send"), 0, tr("Music (*.wav)")));
    if (file->exists())
    {
        file->open(QIODevice::ReadOnly);
        ServerSend((HANDLE) _get_osfhandle(file->handle()));
    }
}



void MainWindow::on_disconnectOutgoing_clicked()
{
    ServerCleanup();
    ui->connectOutgoing->setEnabled(true);
    ui->disconnectOutgoing->setEnabled(false);
    ui->sendBtn->setEnabled(false);
}
