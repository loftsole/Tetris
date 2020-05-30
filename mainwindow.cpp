//界面 目前为测试用
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Tetris Online");
    setFixedSize(1200,800);
    init();
    //this->grabKeyboard();
    game=new Tetris();
    client=new tcpClient(this);
    connect(client,&tcpClient::newOpponentMessage,
            this,&MainWindow::receiveOpMsg);//接收对方消息
    connect(client,&tcpClient::gameStart,
            this,&MainWindow::gameStart);//游戏开始
    connect(game,&Tetris::gameOver,
            this,&MainWindow::gameOver);//游戏结束
    connect(game,&Tetris::print,
            this,&MainWindow::print);//刷新界面
    connect(game,&Tetris::send,
            client,&tcpClient::send);//给对方发送消息
    connect(game,&Tetris::changeTimer,
            this,&MainWindow::changeTimer);//加速
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

    //添加聊天区
    browser=new QTextBrowser(this);
    browser->move(BROWSERX,BROWSERY);
    browser->resize(BROWSERW,BROWSERH);

    edit=new QLineEdit(this);
    edit->move(EDITX,EDITY);
    edit->resize(EDITW,EDITH);
    connect(edit,SIGNAL(returnPressed()),
            this,SLOT(sendChatMessage()));//回车发送

    QFont font;
    font.setPointSize(16);
    edit->setFont(font);
    browser->setFont(font);

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

    for (int i=0;i<MAX_ROW;i++)
        for (int j=0;j<MAX_COL;j++)
        {
            int colour=game->getBox(i,j);
            if (colour>0)
            {
                setColour(painter,colour);
                painter.drawRect(j*BLOCK_SIZE+LEFT_FRAMEX,i*BLOCK_SIZE+LEFT_FRAMEY,BLOCK_SIZE,BLOCK_SIZE);
            }
            colour=game->getOpBox(i,j);
            if (colour>0)
            {
                setColour(painter,colour);
                painter.drawRect(j*BLOCK_SIZE+RIGHT_FRAMEX,i*BLOCK_SIZE+RIGHT_FRAMEY,BLOCK_SIZE,BLOCK_SIZE);
            }
        }
    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
        {
            int colour=game->next_block[i][j];
            if (colour>0)
            {
                setColour(painter,colour);
                painter.drawRect(j*BLOCK_SIZE+LEFT_NEXTX,i*BLOCK_SIZE+LEFT_NEXTY,BLOCK_SIZE,BLOCK_SIZE);
            }
            colour=game->op_next_block[i][j];
            if (colour>0)
            {
                setColour(painter,colour);
                painter.drawRect(j*BLOCK_SIZE+RIGHT_NEXTX,i*BLOCK_SIZE+RIGHT_NEXTY,BLOCK_SIZE,BLOCK_SIZE);
            }
        }
}
void MainWindow::setColour(QPainter &painter,int colour)
{
    if (colour==1)
        painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
    else if (colour==2)
        painter.setBrush(QBrush(Qt::magenta,Qt::SolidPattern));
    else if (colour==3)
        painter.setBrush(QBrush(Qt::yellow,Qt::SolidPattern));
    else if (colour==4)
        painter.setBrush(QBrush(Qt::green,Qt::SolidPattern));
    else if (colour==5)
        painter.setBrush(QBrush(Qt::cyan,Qt::SolidPattern));
    else if (colour==6)
        painter.setBrush(QBrush(Qt::blue,Qt::SolidPattern));
    else if (colour==7)
        painter.setBrush(QBrush(Qt::darkBlue,Qt::SolidPattern));
    else
        painter.setBrush(QBrush(Qt::black,Qt::SolidPattern));
}
void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId()==game_timer && is_game_start)
    {
        game->moveDown();
    }
}
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key()==Qt::Key_W)
        game->rotate();
    if (event->key()==Qt::Key_A)
        game->moveLeft();
    if (event->key()==Qt::Key_S)
        game->moveDown();
    if (event->key()==Qt::Key_D)
        game->moveRight();
}

void MainWindow::receiveOpMsg(QString op_msg)
{
    if (op_msg.startsWith("m"))//聊天消息
    {
        QString msg=client->getOpUserName()+": "+op_msg.mid(1);
        browser->append(msg);
    }
    else//游戏消息
    {
        game->dealOpMsg(op_msg);
        if (op_msg=="over")
        {
            browser->append("游戏结束,你赢了!");
            is_game_start=false;
            client->send("end");
            //killTimer(game_timer);
        }
    }
}

void MainWindow::init()
{
    has_user_name=false;
    is_game_start=false;
}
void MainWindow::showLabel()
{
    QFont font;
    font.setPointSize(16);

    name_label=new QLabel(this);
    name_label->setText(user_name);
    name_label->move(LEFT_FRAMEX,LEFT_FRAMEY-50);
    name_label->setFont(font);
    name_label->show();

    op_name_label=new QLabel(this);
    op_name_label->setText(client->getOpUserName());
    op_name_label->move(RIGHT_FRAMEX,RIGHT_FRAMEY-50);
    op_name_label->setFont(font);
    op_name_label->show();
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
    user_name=dialog->getName();
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
    is_game_start=true;
    showLabel();

    game_timer=startTimer(down_time);
    game->gameStart();
}
void MainWindow::changeTimer(int count)
{
    if (count==10)
        game_timer=startTimer(down_time-50);
    if (count==30)
        game_timer=startTimer(down_time-100);
    if (count==50)
        game_timer=startTimer(down_time-200);
    if (count==100)
        game_timer=startTimer(down_time-300);
}
void MainWindow::sendChatMessage()
{
    QString msg=edit->text();
    if (msg.isEmpty())
        return;
    browser->append(user_name+": "+msg);
    if (is_game_start)
    {
        msg="m"+msg;
        client->send(msg);
    }
    edit->clear();
    edit->clearFocus();
}
void MainWindow::gameOver()
{
    is_game_start=false;
    client->gameOver();
    browser->append("游戏结束,你输了!");
    //killTimer(game_timer);
}
void MainWindow::print()
{
    if (is_game_start)
        update();
}
