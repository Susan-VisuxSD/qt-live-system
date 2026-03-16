#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
//注册弹窗，临时的

namespace Ui{
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

signals:
     void registerRequest(QString username, QString password);//发送用户名密码给widget

private slots:
    void on_pushButton_register_clicked();
    //Qt在执行ui->setupUi(this);的时候，会调用：QMetaObject::connectSlotsByName(this);
    //这个函数会自动寻找 符合命名规则的槽函数。规则是：on_对象名_信号名()

private:
    Ui::RegisterDialog* ui;
};

#endif // REGISTERDIALOG_H
