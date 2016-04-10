#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_actionSend_triggered();

    void on_openIncoming_clicked();

    void on_disconnectIncoming_clicked();

    void on_connectOutgoing_clicked();

    void on_sendBtn_clicked();

    void on_disconnectOutgoing_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
