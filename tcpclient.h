//客户端
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QtNetwork>

class tcpClient:public QObject
{
    Q_OBJECT
public:
    tcpClient(QObject *parent=nullptr);
    void initClient(QString IpAdress,quint16 port);
    void sendTestMessage();
    void send(QString msg);
public slots:
    void readServerMessage();
private:
    QTcpSocket *client;
};

#endif // TCPCLIENT_H
