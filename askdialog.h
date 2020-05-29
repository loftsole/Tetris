#ifndef ASKDIALOG_H
#define ASKDIALOG_H

#include <QObject>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QLayout>

class askDialog:public QDialog
{
    Q_OBJECT
public:
    askDialog();
    askDialog(QString user_name);
signals:
    void accept(QString user_name);
    void refuse(QString user_name);
};

#endif // ASKDIALOG_H
