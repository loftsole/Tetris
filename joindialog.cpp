#include "joindialog.h"

joinDialog::joinDialog(QObject *p)
{
    parent=p;
    setWindowTitle("加入");
    refresh_button=new QPushButton(this);
    refresh_button->setText("刷新");

    scroll_layout=new QGridLayout;
    layout=new QVBoxLayout;

}
void joinDialog::refresh(vector<QString> user_list)//收到新的用户列表,刷新
{
    //清空
    QLayoutItem *child;
    while ((child = scroll_layout->takeAt(0)) != nullptr)
    {
        scroll_layout->removeWidget(child->widget());
        child->widget()->setParent(nullptr);
        delete child;
    }
    while ((child = layout->takeAt(0)) != nullptr)
    {
        layout->removeWidget(child->widget());
        child->widget()->setParent(nullptr);
        delete child;
    }
    //重新布置
    for (unsigned long i=0;i<user_list.size();i++)
    {
        label[i]=new QLabel(this);
        label[i]->setText(user_list[i]);
        button[i]=new QPushButton(this);
        button[i]->setText("发送");
        scroll_layout->addWidget(label[i],int(i),0);
        scroll_layout->addWidget(button[i],int(i),1);

        connect(button[i],&QPushButton::clicked,
                [=](){emit connectRequest(user_list[i]);});
    }
    scroll_widget=new QWidget(this);
    scroll=new QScrollArea(this);
    scroll_widget->setLayout(scroll_layout);
    scroll->setWidget(scroll_widget);
    info_label=new QLabel(this);
    info_label->setText("选择你想进行对战的玩家:");
    layout->addWidget(info_label);
    layout->addWidget(scroll);
    layout->addWidget(refresh_button);
    setLayout(layout);

    update();
}
void joinDialog::ask_for_request(QString user_name)//收到对战请求,询问用户
{
    askDialog *ask_dialog=new askDialog(user_name);

    connect(ask_dialog,&askDialog::accept,
            [=](QString user_name){emit acceptRequest(user_name);close();});//接受 关闭窗口
    connect(ask_dialog,&askDialog::refuse,
            [=](QString user_name){emit refuseRequest(user_name);});//拒绝

    ask_dialog->show();
}
