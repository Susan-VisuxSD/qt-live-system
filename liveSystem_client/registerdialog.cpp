#include "registerdialog.h"
#include "ui_RegisterDialog.h"
#include <QMessageBox>

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this); // 初始化 UI，绑定控件
//    ui->lineEdit_pwd->setEchoMode(QLineEdit::Password);
//    ui->lineEdit_checkpwd->setEchoMode(QLineEdit::Password);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_pushButton_register_clicked()
{
    QString username = ui->lineEdit_username->text();
    QString pwd1 = ui->lineEdit_pwd->text();
    QString pwd2 = ui->lineEdit_checkpwd->text();

    if(username.length() < 3 || username.length() > 12){
        QMessageBox::warning(this, "注册", "用户名不符合规范[3, 12]");
        return;
    }

    if(pwd1.length() < 3 || pwd1.length() > 12){
        QMessageBox::warning(this, "注册", "密码不符合规范[3, 12]");
        return;
    }

    if(pwd1 != pwd2){
        QMessageBox::warning(this, "注册", "两次密码不一致");
        return;
    }

    // TODO: 调用 Pack/Socket 发送注册数据
//    QMessageBox::information(this, "注册", "注册成功");

    emit registerRequest(username, pwd1);

    this->close();

    return;
}


