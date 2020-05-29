#include "tcpclient.h"
#include <QMessageBox>

tcpClient::tcpClient(QObject *parent)
{
    client=new QTcpSocket(parent);
    QString ipAdress="49.235.207.33";//服务器地址
    initClient(ipAdress,16555);
    //initClient("127.0.0.1",16555);
}
void tcpClient::initClient(QString ipAdress,quint16 port)
{
    client->connectToHost(ipAdress,port);
    connect(client,SIGNAL(readyRead()),
            this,SLOT(readServerMessage()));
    is_game_start=false;
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
    else if (data=="finish")
    {
        emit userListFinish(online_user);
    }
    else if (data=="nameok")
    {
        emit nameAccept();
    }
    else if (data=="nameused")
    {
        emit nameUsed();
    }
    else if (data.startsWith("<"))//接收到连接请求
    {
        emit newConnectRequest(data.mid(1));
    }
    else if (data=="decline")//对方拒绝
    {
        //qDebug()<<"对方拒绝";
        QMessageBox *msgBox=new QMessageBox(QMessageBox::Warning, "reject", "对方拒绝了你的请求", QMessageBox::NoButton);
        msgBox->setModal(false);
        msgBox->show();
        //QMessageBox::information(nullptr, " ", "对方拒绝了你的请求");
    }
    else if (data.startsWith("go"))//确认游戏开始
    {
        //send("go");
        emit gameStart();
        refuseAllRequest();
    }
    else
    {
        op_msg=data;
        emit newOpponentMessage();
    }
}
void tcpClient::inputUserName(QString name)
{
    sendUserName(name);
}

vector<QString> tcpClient::getUserName()
{
    return online_user;
}
QString tcpClient::getConnectRequest()
{
    QString front=connect_request.front();
    connect_request.pop();
    return front;
}
QString tcpClient::getOpponentMessage()
{
    return op_msg;
}

void tcpClient::sendUserName(QString user_name)
{
    QString msg="n"+user_name;
    send(msg);
}
void tcpClient::refreshUserList()
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
void tcpClient::refuseAllRequest()
{
    while (!connect_request.empty())
    {
        QString msg="!"+connect_request.front();
        send(msg);
        connect_request.pop();
    }
}
void tcpClient::sendTestMessage()
{
    QByteArray msg="hello server";
    client->write(msg);
}
void tcpClient::send(QString msg)
{
    msg="/"+msg;
    client->write(msg.toUtf8());
}
