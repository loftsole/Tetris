//客户端
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QtNetwork>
#include <QMessageBox>
#include <vector>
#include <queue>

using namespace std;

class tcpClient:public QObject
{
    Q_OBJECT
public:
    tcpClient(QObject *parent=nullptr);
    void initClient(QString IpAdress,quint16 port);
    void sendTestMessage();
    void send(QString msg);

    void sendUserName(QString user_name);//发送自己的用户名

    vector<QString> getUserName();//返回在线玩家列表
    QString getConnectRequest();//返回队列中下一个连接请求
    QString getOpponentMessage();//返回对方发送的信息
    bool isNameOk();//用户名是否接受
public slots:
    void readServerMessage();
    void inputUserName(QString name);
    void refreshUserList();//刷新在线用户列表
    void sendConnectRequest(QString user_name);//向user_name发送连接请求
    void acceptConnect(QString user_name);//接受user_name的连接请求
    void refuseConnect(QString user_name);//拒绝user_name的连接请求
signals:
    void nameAccept();
    void nameUsed();
    void userListFinish(vector<QString>);//可更新玩家列表
    void newConnectRequest(QString user_name);//有新的连接请求
    void gameStart();//游戏开始
    void newOpponentMessage();//接收到对方游戏信息
private:
    void msgProcessing(QString data);
    void refuseAllRequest();

    QTcpSocket *client;
    vector<QString> online_user;
    queue<QString> connect_request;
    QString op_user_name;
    QString op_msg;
    bool is_game_start;
};

#endif // TCPCLIENT_H
