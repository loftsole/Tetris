#include "tcpclient.h"
#include <QMessageBox>

tcpClient::tcpClient(QObject *parent)
{
    client=new QTcpSocket(parent);
    QString ipAdress="49.235.207.33";//服务器地址
    connectTo(ipAdress,16555);
    //connectTo("127.0.0.1",16555);
    init();
}
void tcpClient::connectTo(QString ipAdress,quint16 port)
{
    client->connectToHost(ipAdress,port);
    connect(client,SIGNAL(readyRead()),
            this,SLOT(readServerMessage()));
}
void tcpClient::init()
{
    is_game_start=false;
    has_user_name=false;
}
void tcpClient::readServerMessage()//提取消息
{
    QString msg=QString(client->readAll());
    //处理
    //msg="/-123/finish/<123";
    while (msg.startsWith("/"))
    {
        msg=msg.mid(1);
        int pos=msg.indexOf("/");
        if (pos>0)
        {
            msgProcessing(msg.left(pos));
            msg=msg.mid(pos);
        }
        else
            msgProcessing(msg);
    }
}
void tcpClient::msgProcessing(QString data)//处理消息
{
    //qDebug()<<data;
    if (data.startsWith("-"))//接收在线玩家信息
    {
        online_user.push_back(data.mid(1));
        //qDebug()<<data.mid(1);
    }
    else if (data=="finish")//在线用户列表接受完毕
    {
        emit userListFinish(online_user);
    }
    else if (data=="nameok")//用户名可使用
    {
        emit nameAccept();
        has_user_name=true;
    }
    else if (data=="nameused")//用户名已被使用
    {
        emit nameUsed();
    }
    else if (data.startsWith("<") && is_game_start==false)//接收到连接请求
    {
        emit newConnectRequest(data.mid(1));
    }
    else if (data=="decline")//对方拒绝
    {
        QMessageBox *msgBox=new QMessageBox(QMessageBox::Warning, "reject", "对方拒绝了你的请求", QMessageBox::NoButton);
        msgBox->setModal(false);
        msgBox->show();
    }
    else if (data=="e1")//对方不在线
    {
        QMessageBox *msgBox=new QMessageBox(QMessageBox::Warning, "error", "对方已离线或正在游戏中,请刷新在线列表", QMessageBox::NoButton);
        msgBox->setModal(false);
        msgBox->show();
    }
    else if (data.startsWith("g") && is_game_start==false)//确认游戏开始
    {
        send("go");
        op_user_name=data.mid(1);//保存对方用户名
        is_game_start=true;
        emit gameStart();
    }
    else
    {
        emit newOpponentMessage(data);
    }
}
void tcpClient::inputUserName(QString name)//槽 向服务器发送用户名
{
    if (has_user_name)
    {
        QMessageBox *msgBox=new QMessageBox(QMessageBox::Warning, "warning", "你已经输入了用户名", QMessageBox::NoButton);
        msgBox->setModal(false);
        msgBox->show();
    }
    else
    {
        QString msg="n"+name;
        send(msg);
    }
}

vector<QString> tcpClient::getUserName()
{
    return online_user;
}
QString tcpClient::getOpUserName()
{
    return op_user_name;
}
void tcpClient::refreshUserList()//槽 发送刷新请求
{
    send("refresh");
    //清空本地在线列表
    vector<QString>().swap(online_user);
}
void tcpClient::sendConnectRequest(QString user_name)//向user_name发送连接请求
{
    QString msg=">"+user_name;
    send(msg);
}
void tcpClient::acceptConnect(QString user_name)//接受user_name的连接请求
{
    QString msg="<"+user_name;
    send(msg);
}
void tcpClient::refuseConnect(QString user_name)//拒绝user_name的连接请求
{
    QString msg="!"+user_name;
    send(msg);
}
void tcpClient::gameOver()
{
    send("over");
    is_game_start=false;
}
void tcpClient::send(QString msg)
{
    msg="/"+msg;
    client->write(msg.toUtf8());
}
