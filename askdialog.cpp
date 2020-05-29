#include "askdialog.h"

askDialog::askDialog()
{

}
askDialog::askDialog(QString user_name)
{
    setWindowTitle("新的对战请求");

    QLabel *label=new QLabel(this);
    QString msg=user_name+"向您发送了对战请求,是否接受?";
    label->setText(msg);

    QPushButton *accept_button=new QPushButton(this);
    accept_button->setText("接受");
    QPushButton *refuse_button=new QPushButton(this);
    refuse_button->setText("拒绝");

    connect(accept_button,&QPushButton::clicked,
            [=](){emit accept(user_name);close();});
    connect(refuse_button,&QPushButton::clicked,
            [=](){emit refuse(user_name);close();});

    QGridLayout *layout=new QGridLayout;
    layout->addWidget(label);
    layout->addWidget(accept_button);
    layout->addWidget(refuse_button);
    setLayout(layout);


}
