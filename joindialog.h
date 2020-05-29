#ifndef JOINDIALOG_H
#define JOINDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QDebug>
#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <vector>
#include "askdialog.h"
using namespace std;

class joinDialog:public QDialog
{
    Q_OBJECT
public:
    joinDialog(QObject *parent=nullptr);

    QPushButton *refresh_button;
signals:
    void connectRequest(QString user_name);
    void acceptRequest(QString user_name);
    void refuseRequest(QString user_name);
public slots:
    void refresh(vector<QString> user_list);//收到用户列表
    void ask_for_request(QString user_name);//收到连接请求时询问用户
private:
    QPushButton *button[100];
    QLabel *label[100];
    QLabel *info_label;
    QScrollArea *scroll;
    QGridLayout *scroll_layout;
    QVBoxLayout *layout;
    QWidget *scroll_widget;
    QObject *parent;
};

#endif // JOINDIALOG_H
