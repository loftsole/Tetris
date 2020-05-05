#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QtNetwork>
#include <QEventLoop>

class tcpServer:public QObject
{
    Q_OBJECT
public:
    tcpServer(QObject *parent=nullptr);
    void initServer(quint16 port);
    QString returnOutIP();
public slots:
    void acceptConnection();
    void replyToClient();
private:
    QString GetHtml(QString url);
    QString GetNetIP(QString webCode);

    QTcpServer *server;
    QTcpSocket *serverSocket;

};

#endif // TCPSERVER_H
