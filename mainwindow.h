//界面 目前为测试用
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLayout>
#include <QTextEdit>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include "tetris.h"
#include "tcpclient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void gameStart();

protected:
    void paintEvent(QPaintEvent*);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:


private:
    Ui::MainWindow *ui;
    Tetris *game;
    tcpClient *client;

    int paint_timer; //渲染刷新计时器
    int game_timer;

    const int MAX_ROW=20;
    const int MAX_COL=10;
    const int refresh_time=100;
    const int down_time=1000;
};
#endif // MAINWINDOW_H
