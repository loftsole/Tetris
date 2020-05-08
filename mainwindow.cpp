#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpclient.h"
#include "tcpserver.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_joinButton_clicked()
{
    QString ipAdress="49.235.207.33";
    client=new tcpClient(this);
    client->initClient(ipAdress,16555);
    QString userName=ui->userNameEdit->text();
    userName="n"+userName;
    qDebug()<<"在线玩家列表:";
    client->send(userName);
    //client->sendTestMessage();
}


void MainWindow::on_sendButton_clicked()
{
    QString msg=ui->sendMessageEdit->text();
    client->send(msg);
}
