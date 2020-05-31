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
#include <QLineEdit>
#include <QTextBrowser>
#include <QFont>
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

protected:
    void paintEvent(QPaintEvent*);
    void timerEvent(QTimerEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private slots:
    void print();
    void nameAccepted();//用户名被接受
    void receiveOpMsg(QString op_msg);//接收对方发来的消息
    void useItem(int num);
    void opUseItem(int num);//打印使用道具信息
    void relieveItem(int num);
    void sendChatMessage();
    void changeTimer(int count);
    void gameStart();
    void gameOver();

private:
    void init();
    void showLabel();
    void showInformation();
    void readUserName();//读取用户名窗口
    void createJoinDialog();//加入对战窗口
    void setColour(QPainter &painter,int colour);

    Ui::MainWindow *ui;
    Tetris *game;
    tcpClient *client;
    nameDialog *dialog;
    joinDialog *join_dialog;
    QLineEdit *edit;
    QTextBrowser *browser;
    QLabel *name_label,*op_name_label;
    QLabel *item_label[3];
    QPushButton *button[5];

    int paint_timer; //渲染刷新计时器
    int game_timer;
    QString user_name;

    bool has_user_name;
    bool is_game_start;

    const int MAX_ROW=20;
    const int MAX_COL=12;
    const int BLOCK_SIZE=30;
    const int LEFT_FRAMEX=50;             //游戏边框
    const int LEFT_FRAMEY=120;
    const int RIGHT_FRAMEX=790;
    const int RIGHT_FRAMEY=LEFT_FRAMEY;
    const int LEFT_NEXTX=460;             //下一个方块
    const int LEFT_NEXTY=LEFT_FRAMEY;
    const int RIGHT_NEXTX=620;
    const int RIGHT_NEXTY=LEFT_FRAMEY;
    const int BROWSERX=LEFT_NEXTX;        //聊天显示框
    const int BROWSERY=290;
    const int BROWSERW=RIGHT_NEXTX+4*BLOCK_SIZE-LEFT_NEXTX;
    const int BROWSERH=360;
    const int EDITX=LEFT_NEXTX;           //聊天编辑框
    const int EDITY=680;
    const int EDITW=BROWSERW;
    const int EDITH=40;
    const int ITEM_LABELX=300;            //道具栏
    const int ITEM_LABELY=60;

    int down_time=500;
};
#endif // MAINWINDOW_H
