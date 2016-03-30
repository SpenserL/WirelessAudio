#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    audioManager = new AudioManager(this);
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
    ClientSetup(true);
}
