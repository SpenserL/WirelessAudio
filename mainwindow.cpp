#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
    delete audioManager;
}

void MainWindow::on_pushButton_released()
{
    QFile *file = new QFile(QFileDialog::getOpenFileName(this, tr("Pick A Song"), 0, tr("Music (*.wav)")));
    qDebug() << "Have file";
    audioManager = new AudioManager(this);
    qDebug() << "New manager";
    audioManager->loadSong(file);
    qDebug() << "Load Song";
    audioManager->play();
    qDebug() << "Played Song";
}
