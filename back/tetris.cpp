#include "tetris.h"

Tetris::Tetris(QObject *parent)
{
    client=new tcpClient(parent);
    //QString ipAdress="49.235.207.33";//服务器地址
    //client->initClient(ipAdress,16555);//绑定ip和端口
    //client->initClient("127.0.0.1",16555);//使用本地测试
}


void Tetris::input_user_name(QString name)//输入用户名
{
    user_name=name;

    test_client();
}


//测试用
void Tetris::test_client()
{
    qDebug()<<"在线玩家列表:";
    QString user_name_msg="n"+user_name;
    client->send(user_name_msg);
    client->send("refresh");

    connect(client,SIGNAL(userListFinish()),
            this,SLOT(print_user()));

}
void Tetris::print_user()
{
    vector<QString> list=client->getUserName();
    for (unsigned long i=0;i<list.size();i++)
        qDebug()<<list[i];
}
void Tetris::send_test_msg(QString msg)
{
    client->send(msg);
}
