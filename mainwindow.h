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
#include <QObject>
#include <QAction>
#include <QToolBar>
#include "tetris.h"
#include "tcpclient.h"
#include "namedialog.h"
#include "joindialog.h"

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
    void nameAccepted();//用户名被接受
private:
    void init();
    void readUserName();//读取用户名窗口
    void createJoinDialog();//加入对战窗口

    Ui::MainWindow *ui;
    Tetris *game;
    tcpClient *client;
    nameDialog *dialog;
    joinDialog *join_dialog;

    int paint_timer; //渲染刷新计时器
    int game_timer;
    QString user_name;

    bool has_user_name;
    bool is_game_start;

    const int MAX_ROW=20;
    const int MAX_COL=12;
    const int BLOCK_SIZE=30;
    const int LEFT_FRAMEX=50;
    const int LEFT_FRAMEY=120;
    const int RIGHT_FRAMEX=790;
    const int RIGHT_FRAMEY=LEFT_FRAMEY;
    const int LEFT_NEXTX=460;
    const int LEFT_NEXTY=LEFT_FRAMEY;
    const int RIGHT_NEXTX=620;
    const int RIGHT_NEXTY=LEFT_FRAMEY;
    const int refresh_time=100;
    const int down_time=1000;
};
#endif // MAINWINDOW_H
