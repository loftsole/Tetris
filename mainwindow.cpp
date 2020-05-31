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
    game=new Tetris();
    game->init();
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
    connect(game,&Tetris::opUseItem,
            this,&MainWindow::opUseItem);//打印对方使用道具信息
    connect(game,&Tetris::useItemSuccess,
            this,&MainWindow::useItem);//打印道具信息
    connect(game,&Tetris::relieveItem,
            this,&MainWindow::relieveItem);//道具效果消失
    has_user_name=false;

    //背景
    QPixmap pixmap = QPixmap(":/images/main_background").scaled(this->size());
    QPalette palette(this->palette());
    palette.setBrush(QPalette::Background, QBrush(pixmap));
    this->setPalette(palette);
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

    QAction *info_action=new QAction(tr("&介绍"),this);
    info_action->setStatusTip(tr("查看游戏规则"));
    connect(info_action,&QAction::triggered,
            this,&MainWindow::showInformation);
    QToolBar *info_toolbar=addToolBar(tr("&info"));
    info_toolbar->addAction(info_action);

    //添加聊天区
    browser=new QTextBrowser(this);
    browser->move(BROWSERX,BROWSERY);
    browser->resize(BROWSERW,BROWSERH);

    edit=new QLineEdit(this);
    edit->move(EDITX,EDITY);
    edit->resize(EDITW,EDITH);
    connect(edit,SIGNAL(returnPressed()),
            this,SLOT(sendChatMessage()));//回车发送

    QPalette palette2;
    palette2.setColor(QPalette::Background, QColor(192,253,123,50)); // 最后一项为透明度
    browser->setPalette(palette2);

    QFont font;
    font.setPointSize(14);
    edit->setFont(font);
    browser->setFont(font);
    //用户名显示
    //font.setPointSize(18);

    name_label=new QLabel(this);
    op_name_label=new QLabel(this);
    name_label->move(LEFT_FRAMEX,LEFT_FRAMEY-50);
    op_name_label->move(RIGHT_FRAMEX,RIGHT_FRAMEY-50);
    //name_label->setFont(font);
    //op_name_label->setFont(font);
    name_label->setStyleSheet("font-size:24px;color:white");
    op_name_label->setStyleSheet("font-size:24px;color:white");



    setButton();

    //道具栏
    item_label[0]=new QLabel(this);
    item_label[0]->resize(BLOCK_SIZE,BLOCK_SIZE);
    item_label[0]->move(ITEM_LABELX,ITEM_LABELY);
    item_label[1]=new QLabel(this);
    item_label[1]->resize(BLOCK_SIZE,BLOCK_SIZE);
    item_label[1]->move(ITEM_LABELX+40,ITEM_LABELY);
    item_label[2]=new QLabel(this);
    item_label[2]->resize(BLOCK_SIZE,BLOCK_SIZE);
    item_label[2]->move(ITEM_LABELX+80,ITEM_LABELY);
    for (int i=0;i<3;i++)
        item_label[i]->setFont(font);



    //join window
    join_dialog=new joinDialog(this);
    join_dialog->resize(200,300);
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

    //name window
    dialog=new nameDialog(this);

    connect(client,&tcpClient::nameAccept,
            this,&MainWindow::nameAccepted);
    connect(client,SIGNAL(nameUsed()),
            dialog,SLOT(nameUsed()));
    connect(dialog,&nameDialog::sendName,
            client,&tcpClient::inputUserName);


    update();
    //readUserName();

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
    painter.setBrush(QBrush(Qt::darkBlue,Qt::SolidPattern));
    painter.drawRect(LEFT_FRAMEX-10,LEFT_FRAMEY-10,MAX_COL*BLOCK_SIZE+20,10);
    painter.drawRect(LEFT_FRAMEX-10,LEFT_FRAMEY+BLOCK_SIZE*MAX_ROW,MAX_COL*BLOCK_SIZE+20,10);
    painter.drawRect(LEFT_FRAMEX-10,LEFT_FRAMEY-10,10,MAX_ROW*BLOCK_SIZE+20);
    painter.drawRect(LEFT_FRAMEX+BLOCK_SIZE*MAX_COL,LEFT_FRAMEY-10,10,MAX_ROW*BLOCK_SIZE+20);
    painter.setBrush(QBrush(Qt::darkRed,Qt::SolidPattern));
    painter.drawRect(RIGHT_FRAMEX-10,RIGHT_FRAMEY-10,MAX_COL*BLOCK_SIZE+20,10);
    painter.drawRect(RIGHT_FRAMEX-10,RIGHT_FRAMEY+BLOCK_SIZE*MAX_ROW,MAX_COL*BLOCK_SIZE+20,10);
    painter.drawRect(RIGHT_FRAMEX-10,RIGHT_FRAMEY-10,10,MAX_ROW*BLOCK_SIZE+20);
    painter.drawRect(RIGHT_FRAMEX+BLOCK_SIZE*MAX_COL,RIGHT_FRAMEY-10,10,MAX_ROW*BLOCK_SIZE+20);

    painter.setBrush(QColor(255,255,255,225));
    painter.drawRect(LEFT_FRAMEX,LEFT_FRAMEY,MAX_COL*BLOCK_SIZE,MAX_ROW*BLOCK_SIZE);//左边框
    painter.drawRect(RIGHT_FRAMEX,RIGHT_FRAMEY,MAX_COL*BLOCK_SIZE,MAX_ROW*BLOCK_SIZE);//右边框
    painter.setBrush(QColor(0,170,255,80));
    painter.drawRect(LEFT_NEXTX,LEFT_NEXTY,4*BLOCK_SIZE,4*BLOCK_SIZE);
    painter.drawRect(RIGHT_NEXTX,RIGHT_NEXTY,4*BLOCK_SIZE,4*BLOCK_SIZE);//下一个方块边框
    painter.drawRect(ITEM_LABELX,ITEM_LABELY,BLOCK_SIZE,BLOCK_SIZE);//道具栏
    painter.drawRect(ITEM_LABELX+40,ITEM_LABELY,BLOCK_SIZE,BLOCK_SIZE);
    painter.drawRect(ITEM_LABELX+80,ITEM_LABELY,BLOCK_SIZE,BLOCK_SIZE);


    for (int i=0;i<3;i++)
    {
        if (game->item[i]==1)
            item_label[i]->setText("巨");
        else if (game->item[i]==2)
            item_label[i]->setText("反");
        else if (game->item[i]==3)
            item_label[i]->setText("墨");
        else
            item_label[i]->setText("");
    }

    for (int i=0;i<MAX_ROW;i++)
        for (int j=0;j<MAX_COL;j++)
        {
            int colour=game->getBox(i,j);
            if (colour>0)
            {
                setOutColour(painter,colour);
                painter.drawRect(j*BLOCK_SIZE+LEFT_FRAMEX,i*BLOCK_SIZE+LEFT_FRAMEY,BLOCK_SIZE,BLOCK_SIZE);

                setColour(painter,colour);
                painter.drawRect(j*BLOCK_SIZE+LEFT_FRAMEX+4,i*BLOCK_SIZE+LEFT_FRAMEY+4,BLOCK_SIZE-8,BLOCK_SIZE-8);
            }
            colour=game->getOpBox(i,j);
            if (colour>0)
            {
                setOutColour(painter,colour);
                painter.drawRect(j*BLOCK_SIZE+RIGHT_FRAMEX,i*BLOCK_SIZE+RIGHT_FRAMEY,BLOCK_SIZE,BLOCK_SIZE);

                setColour(painter,colour);
                painter.drawRect(j*BLOCK_SIZE+RIGHT_FRAMEX+4,i*BLOCK_SIZE+RIGHT_FRAMEY+4,BLOCK_SIZE-8,BLOCK_SIZE-8);
            }
        }
    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
        {
            int colour=game->next_block[i][j];
            if (colour>0)
            {
                setOutColour(painter,colour);
                painter.drawRect(j*BLOCK_SIZE+LEFT_NEXTX,i*BLOCK_SIZE+LEFT_NEXTY,BLOCK_SIZE,BLOCK_SIZE);

                setColour(painter,colour);
                painter.drawRect(j*BLOCK_SIZE+LEFT_NEXTX+4,i*BLOCK_SIZE+LEFT_NEXTY+4,BLOCK_SIZE-8,BLOCK_SIZE-8);
            }
            colour=game->op_next_block[i][j];
            if (colour>0)
            {
                setOutColour(painter,colour);
                painter.drawRect(j*BLOCK_SIZE+RIGHT_NEXTX,i*BLOCK_SIZE+RIGHT_NEXTY,BLOCK_SIZE,BLOCK_SIZE);

                setColour(painter,colour);
                painter.drawRect(j*BLOCK_SIZE+RIGHT_NEXTX+4,i*BLOCK_SIZE+RIGHT_NEXTY+4,BLOCK_SIZE-8,BLOCK_SIZE-8);
            }
        }
    if (game->is_ink)
    {
        painter.setBrush(QBrush(Qt::black,Qt::SolidPattern));
        painter.drawRect(LEFT_NEXTX,LEFT_NEXTY,4*BLOCK_SIZE,4*BLOCK_SIZE);
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
void MainWindow::setOutColour(QPainter &painter,int colour)
{
    if (colour==1)
        painter.setBrush(QBrush(Qt::darkRed,Qt::SolidPattern));
    else if (colour==2)
        painter.setBrush(QBrush(Qt::darkMagenta,Qt::SolidPattern));
    else if (colour==3)
        painter.setBrush(QBrush(Qt::darkYellow,Qt::SolidPattern));
    else if (colour==4)
        painter.setBrush(QBrush(Qt::darkGreen,Qt::SolidPattern));
    else if (colour==5)
        painter.setBrush(QBrush(Qt::darkCyan,Qt::SolidPattern));
    else if (colour==6)
        painter.setBrush(QBrush(Qt::darkBlue,Qt::SolidPattern));
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
    {
        if (game->is_reverse)
            game->moveDown();
        else
            game->rotate();
    }
    if (event->key()==Qt::Key_A)
    {
        if (game->is_reverse)
            game->moveRight();
        else
            game->moveLeft();
    }
    if (event->key()==Qt::Key_S)
    {
        if (game->is_reverse)
            game->rotate();
        else
            game->moveDown();
    }
    if (event->key()==Qt::Key_D)
    {
        if (game->is_reverse)
            game->moveLeft();
        else
            game->moveRight();
    }
    if (event->key()==Qt::Key_1)
        game->useItem(0);
    if (event->key()==Qt::Key_2)
        game->useItem(1);
    if (event->key()==Qt::Key_3)
        game->useItem(2);
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
            client->gameOver();
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
    name_label->setText(user_name);
    op_name_label->setText(client->getOpUserName());
}
void MainWindow::readUserName()//创建输入用户名窗口
{
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

    client->refreshUserList();//创建时刷新一次
    join_dialog->show();
}

void MainWindow::gameStart()
{
    is_game_start=true;
    showLabel();
    browser->append("游戏开始!");
    join_dialog->close();

    game_timer=startTimer(down_time);
    game->gameStart();
}
void MainWindow::changeTimer(int count)//加速计时器
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
void MainWindow::sendChatMessage()//发送聊天消息
{
    QString msg=edit->text();
    if (msg.isEmpty())
        return;
    while (msg.indexOf("/")!=-1)
        msg.remove(msg.indexOf("/"),1);
    browser->append(user_name+": "+msg);
    msg="m"+msg;
    client->send(msg);
    edit->clear();
    edit->clearFocus();
}
void MainWindow::useItem(int num)
{
    if (num==1)
        browser->append("向"+client->getOpUserName()+"丢了一个巨大方块!");
    if (num==2)
        browser->append("交换了"+client->getOpUserName()+"的方向!");
    if (num==3)
        browser->append("向"+client->getOpUserName()+"泼了一瓶墨水!");
}
void MainWindow::opUseItem(int num)
{
    if (num==1)
        browser->append(client->getOpUserName()+"向你丢了一个巨大的方块!");
    if (num==2)
        browser->append(client->getOpUserName()+"交换了你的方向!");
    if (num==3)
        browser->append(client->getOpUserName()+"向你泼了一瓶墨水!");
}
void MainWindow::relieveItem(int num)
{
    if (num==2)
        browser->append("你的方向恢复了正常");
}
void MainWindow::gameOver()
{
    is_game_start=false;
    client->send("over");
    client->gameOver();
    browser->append("游戏结束,你输了!");
    //killTimer(game_timer);
}
void MainWindow::showInformation()
{
    QTextBrowser *info_browser=new QTextBrowser;
    info_browser->resize(420,360);
    QFont font;
    font.setPointSize(16);
    info_browser->setFont(font);
    info_browser->setText("-----游戏规则介绍-----");
    info_browser->append("使用WASD进行控制,AD左右移动,W旋转,S加速下落一格(不会直接落到底部).");
    info_browser->append("消去时有概率获得道具,道具栏显示在上方,共三格,按123数字键使用.道具栏满后无法再获得道具.");
    info_browser->show();
}
void MainWindow::setButton()
{
    for (int i=0;i<5;i++)
    {
        button[i]=new QPushButton(this);
        button[i]->resize(BLOCK_SIZE+10,BLOCK_SIZE+10);
        button[i]->move(BUTTONX+i*60,BUTTONY);
    }
    connect(button[0],&QPushButton::clicked,
            this,&MainWindow::chatButton1);
    connect(button[1],&QPushButton::clicked,
            this,&MainWindow::chatButton2);
    connect(button[2],&QPushButton::clicked,
            this,&MainWindow::chatButton3);
    connect(button[3],&QPushButton::clicked,
            this,&MainWindow::chatButton4);
    connect(button[4],&QPushButton::clicked,
            this,&MainWindow::chatButton5);
    button[0]->setStyleSheet("border-image:url(:/images/btn1)");
    button[1]->setStyleSheet("border-image:url(:/images/btn2)");
    button[2]->setStyleSheet("border-image:url(:/images/btn3)");
    button[3]->setStyleSheet("border-image:url(:/images/btn4)");
    button[4]->setStyleSheet("border-image:url(:/images/btn5)");
}
void MainWindow::chatButton1()
{
    browser->append(user_name+": ?");
    QString msg="m?";
    client->send(msg);
}
void MainWindow::chatButton2()
{
    browser->append(user_name+": gg");
    QString msg="mgg";
    client->send(msg);
}
void MainWindow::chatButton3()
{
    browser->append(user_name+": 就这");
    QString msg="m就这";
    client->send(msg);
}
void MainWindow::chatButton4()
{
    browser->append(user_name+": 你不会以为我在认真玩吧");
    QString msg="m你不会以为我在认真玩吧";
    client->send(msg);
}
void MainWindow::chatButton5()
{
    browser->append(user_name+": 难受,宁愿输的人是我");
    QString msg="m难受,宁愿输的人是我";
    client->send(msg);
}
void MainWindow::print()
{
    if (is_game_start)
        update();
}
