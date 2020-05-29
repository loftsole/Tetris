//界面 目前为测试用
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1200,800);
    game=new Tetris();
    client=new tcpClient(this);
    has_user_name=false;

    //添加工具栏
    QAction *name_action=new QAction(tr("&登录"),this);
    name_action->setStatusTip(tr("输入你的用户名"));
    connect(name_action,&QAction::triggered,
           this,&MainWindow::readUserName);
    QToolBar *name_toolbar=addToolBar(tr("&name"));
    name_toolbar->addAction(name_action);

    QAction *join_action=new QAction(tr("&对战"),this);
    join_action->setStatusTip(tr("寻找你的对手"));
    connect(join_action,&QAction::triggered,
            this,&MainWindow::createJoinDialog);
    QToolBar *join_toolbar=addToolBar(tr("&join"));
    join_toolbar->addAction(join_action);

    readUserName();

    //gameStart();
}

MainWindow::~MainWindow()
{
    delete game;
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setBrush(QBrush(Qt::white,Qt::SolidPattern));
    painter.drawRect(LEFT_FRAMEX,LEFT_FRAMEY,MAX_COL*BLOCK_SIZE,MAX_ROW*BLOCK_SIZE);//左边框
    painter.drawRect(RIGHT_FRAMEX,RIGHT_FRAMEY,MAX_COL*BLOCK_SIZE,MAX_ROW*BLOCK_SIZE);//右边框
    painter.drawRect(LEFT_NEXTX,LEFT_NEXTY,4*BLOCK_SIZE,4*BLOCK_SIZE);
    painter.drawRect(RIGHT_NEXTX,RIGHT_NEXTY,4*BLOCK_SIZE,4*BLOCK_SIZE);//下一个方块边框

    for (int i=0;i<MAX_COL;i++)
        for (int j=0;j<MAX_ROW;j++)
        {
            if (game->getBox(i,j)==1)
            {
                painter.setBrush(QBrush(Qt::blue,Qt::SolidPattern));
                painter.drawRect(i*30+200,j*30+200,30,30);
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
        game->moveToBottom();
    }
}
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
        {
        case Qt::Key_Up:
            game->rotate();
            break;
        case Qt::Key_Down:
            game->moveToBottom();
            break;
        case Qt::Key_Left:
            game->moveToLeft();
            break;
        case Qt::Key_Right:
            game->moveToRight();
            break;
        default:
            break;
        }
}

void MainWindow::init()
{
    has_user_name=false;
    is_game_start=false;
}
void MainWindow::readUserName()//创建输入用户名窗口
{
    dialog=new nameDialog(this);

    connect(client,&tcpClient::nameAccept,
            this,&MainWindow::nameAccepted);
    connect(client,SIGNAL(nameUsed()),
            dialog,SLOT(nameUsed()));
    connect(dialog,&nameDialog::sendName,
            client,&tcpClient::inputUserName);
    dialog->show();
}
void MainWindow::nameAccepted()//槽 用户名接受完毕
{
    dialog->close();
    has_user_name=true;
    createJoinDialog();
}
void MainWindow::createJoinDialog()//创建联机窗口
{
    if (!has_user_name)
    {
        QMessageBox *msgBox=new QMessageBox(QMessageBox::Warning, "warning", "请先输入用户名", QMessageBox::NoButton);
        msgBox->setModal(false);
        msgBox->show();
        return;
    }

    join_dialog=new joinDialog(this);
    connect(client,&tcpClient::userListFinish,
            join_dialog,&joinDialog::refresh);//客户端收到消息后在join窗口打印
    connect(join_dialog->refresh_button,&QPushButton::clicked,
            client,&tcpClient::refreshUserList);//按刷新按钮后刷新
    connect(join_dialog,&joinDialog::connectRequest,
            client,&tcpClient::sendConnectRequest);//发出连接请求
    connect(client,&tcpClient::newConnectRequest,
            join_dialog,&joinDialog::ask_for_request);//收到时询问
    connect(join_dialog,&joinDialog::acceptRequest,
            client,&tcpClient::acceptConnect);//接受
    connect(join_dialog,&joinDialog::refuseRequest,
            client,&tcpClient::refuseConnect);//拒绝

    connect(client,&tcpClient::gameStart,
            [](){qDebug()<<"game start!";});

    client->refreshUserList();//创建时刷新一次
    join_dialog->show();
}

void MainWindow::gameStart()
{
     paint_timer=startTimer(refresh_time);
     game_timer=startTimer(down_time);
     game->gameStart();
}

