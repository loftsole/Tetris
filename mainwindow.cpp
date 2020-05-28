//界面 目前为测试用
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1600,900);
    //game=new Tetris(this);

    //client=new tcpClient(parent);
    //QString ipAdress="49.235.207.33";//服务器地址
    //client->initClient(ipAdress,16555);//绑定ip和端口
    //client->initClient("127.0.0.1",16555);//使用本地测试

    gameStart();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setBrush(QBrush(Qt::white,Qt::SolidPattern));
    painter.drawRect(200,200,MAX_COL*30,MAX_ROW*30);

    for (int i=1;i<=MAX_ROW;i++)
        for (int j=1;j<=MAX_COL;j++)
        {
            if (game.getBox(i,j)==1)
            {
                painter.setBrush(QBrush(Qt::blue,Qt::SolidPattern));
                painter.drawRect(i*30+200,j*30+200,30,30);
                //qDebug()<<i<<j;
            }
        }
}
void MainWindow::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==paint_timer)
    {
        update();
    }
    if (event->timerId()==game_timer)
    {
        game.moveToBottom();
    }
}

void MainWindow::gameStart()
{
     paint_timer=startTimer(refresh_time);
     game_timer=startTimer(down_time);
     game.gameStart();
}
