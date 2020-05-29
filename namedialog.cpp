#include "namedialog.h"

nameDialog::nameDialog(QObject *parent)
{
    setWindowTitle("请输入用户名");

    name_ok=false;
    button=new QPushButton(this);
    edit=new QLineEdit(this);
    label=new QLabel(this);
    label->setText("请输入用户名");
    button->setText("确认");
    connect(button,&QPushButton::clicked,
            this,&nameDialog::readName);

    QVBoxLayout *layout=new QVBoxLayout;
    layout->addWidget(label);
    layout->addWidget(edit);
    layout->addWidget(button);
    setLayout(layout);
}
void nameDialog::nameUsed()
{
    QMessageBox::information(nullptr,"error","用户名已被使用");
}

void nameDialog::readName()//槽 按键后读取
{
    name=edit->text();
    if (checkName())
        emit sendName(name);

}
bool nameDialog::checkName()
{
    if(name.indexOf("/")!=-1 || name.indexOf("-")!=-1 || name.indexOf(">")!=-1 || name.indexOf("<")!=-1 || name.indexOf("!")!=-1)
    {
        QMessageBox::information(nullptr,"error","用户名不能包含特殊字符,请重新输入");
        return false;
    }
    else
        return true;
}
