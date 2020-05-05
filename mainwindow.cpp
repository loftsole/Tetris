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
    QString ipAdress=ui->ipInput->text();
    tcpClient *client=new tcpClient(this);
    client->initClient(ipAdress,9999);
    client->sendTestMessage();
}

void MainWindow::on_createButton_clicked()
{
    tcpServer *server=new tcpServer(this);
    server->initServer(9999);
    ui->myIpAdress->setText(server->returnOutIP());
    ui->myIpAdress->setReadOnly(true);
}
