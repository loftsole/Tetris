#ifndef NAMEDIALOG_H
#define NAMEDIALOG_H

#include <QObject>
#include <QDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QDebug>

class nameDialog:public QDialog
{
    Q_OBJECT
public:
    nameDialog(QObject *parent=nullptr);
    QString getName();

signals:
    void sendName(QString user_name);
public slots:
    void nameUsed();

private slots:
    void readName();
private:
    bool checkName();

    bool name_ok;
    QString name;

    QPushButton *button;
    QLineEdit *edit;
    QLabel *label;
};

#endif // NAMEDIALOG_H
