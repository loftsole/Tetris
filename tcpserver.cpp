#include "tcpserver.h"

tcpServer::tcpServer(QObject *parent)
{
    server=new QTcpServer(parent);
}
void tcpServer::initServer(quint16 port)
{
    server->listen(QHostAddress::Any,port);
    connect(server,SIGNAL(newConnection()),
            this,SLOT(acceptConnection()));
}
void tcpServer::acceptConnection()
{
    serverSocket=server->nextPendingConnection();
    connect(serverSocket,SIGNAL(readyRead()),
            this,SLOT(replyToClient()));
}
void tcpServer::replyToClient()
{
    QString msg=QString(serverSocket->readAll());
    qDebug()<<"来自客户端的消息:"<<msg;
    QByteArray data="hello client,i got your message.";
    serverSocket->write(data);
}
QString tcpServer::GetHtml(QString url)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
    QByteArray responseData;
    QEventLoop eventLoop;
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    responseData = reply->readAll();
    return QString(responseData);
}
QString tcpServer::GetNetIP(QString webCode)
{
    QString tmp = webCode.split("[").last();
    return tmp.split("]").first();
}
QString tcpServer::returnOutIP()
{
    return GetNetIP(GetHtml("http://www.3322.org/dyndns/getip"));
}
