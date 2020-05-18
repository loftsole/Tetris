//界面 目前为测试用
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLayout>
#include <QTextEdit>
#include "tetris.h"
#include "tcpserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_joinButton_clicked();

    void on_sendButton_clicked();

private:
    Ui::MainWindow *ui;
    Tetris *game;
    tcpServer *server;
};
#endif // MAINWINDOW_H
