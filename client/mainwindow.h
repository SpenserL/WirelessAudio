#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include "audiomanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_playButton_released();

    void on_pauseButton_released();
    void handleStateChanged(QAudio::State);
    void on_resumeButton_released();

    void on_skipForwardButton_released();

    void on_skipBackwardsButton_released();

    void on_sendBtn_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_connectOutgoing_clicked();

    void on_disconnectOutgoing_clicked();

    void on_openIncoming_clicked();

    void on_disconnectIncoming_clicked();

private:
    Ui::MainWindow *ui;
    AudioManager *audioManager;
};

#endif // MAINWINDOW_H
