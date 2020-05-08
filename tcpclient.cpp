#include "tcpclient.h"
#include <QMessageBox>

tcpClient::tcpClient(QObject *parent)
{
    client=new QTcpSocket(parent);
}
void tcpClient::initClient(QString ipAdress,quint16 port)
{
    client->connectToHost(ipAdress,port);
    connect(client,SIGNAL(readyRead()),
            this,SLOT(readServerMessage()));
}
void tcpClient::readServerMessage()
{
    QString msg=QString(client->readAll());
    qDebug()<<msg;
}
void tcpClient::sendTestMessage()
{
    QByteArray msg="hello server";
    client->write(msg);
}
void tcpClient::send(QString msg)
{
    client->write(msg.toUtf8());
}
