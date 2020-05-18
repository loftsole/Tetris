//界面 目前为测试用
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    game=new Tetris(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_joinButton_clicked()
{
    QString user_name=ui->userNameEdit->text();
    game->input_user_name(user_name);
}


void MainWindow::on_sendButton_clicked()
{
    QString msg=ui->sendMessageEdit->text();
    game->send_test_msg(msg);
}
