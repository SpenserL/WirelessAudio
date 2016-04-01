#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Server.h"
#include <QDebug>

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
    ServerSetup();
}

void MainWindow::on_pushButton_2_clicked()
{
    qDebug() << "Clicked disconnect";
    ServerCleanup();
}
