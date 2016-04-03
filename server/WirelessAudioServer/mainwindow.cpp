#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Server.h"
#include <QDebug>
#include <QFileDialog>
#include <io.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    qDebug() << "Clicked connect";
    if (ServerSetup() == 0) {
        QFile *file = new QFile(QFileDialog::getSaveFileName(this, tr("Pick The Destination Song Name"), 0, tr("Music (*.wav)")));
        if (file->fileName() != NULL) {
            qDebug() << "file opened for writing";
            ui->pushButton->setEnabled(false);
            ui->pushButton_2->setEnabled(true);
            file->open(QIODevice::WriteOnly);
            ServerListen((HANDLE) _get_osfhandle(file->handle()));
        } else {
            ServerCleanup();
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    qDebug() << "Clicked disconnect";
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
    ServerCleanup();
}
