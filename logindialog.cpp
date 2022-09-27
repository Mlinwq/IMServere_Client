#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowTitle("MyChat");
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_pb_clear_clicked()
{
    ui->le_tel->setText("");
    ui->le_password->setText("");
}

void LoginDialog::on_pb_commit_clicked()
{
    //1.获取控件中的数据
    QString tel = ui->le_tel->text();
    QString password = ui->le_password->text();

    //2.校验数据合法性
    //tel不能为空，不能为空格。密码不能为空

    //长度限制，手机号必须为11位。密码不超过10位
    QString telTemp = tel;
    if(tel.isEmpty() || password.isEmpty() || telTemp.remove(" ").isEmpty()){
        QMessageBox::about(this, "提示", "手机号或密码不能为空");
        return;
    }
    if(tel.length()!=11 || password.length()>10){
        QMessageBox::about(this, "提示", "手机号必须为11位，密码不能超过10位");
        return;
    }

    //3.发送数据到kernel类
    Q_EMIT SIG_LoginCommit(tel, password);
}

void LoginDialog::on_pb_clear_register_clicked()
{
    ui->le_name_register->setText("");
    ui->le_tel_register->setText("");
    ui->le_password_register->setText("");
}

void LoginDialog::on_pb_commit_register_clicked()
{
    //1.获取控件中的数据
    QString tel = ui->le_tel_register->text();
    QString password = ui->le_password_register->text();
    QString name = ui->le_name_register->text();

    //2.校验数据合法性
    //tel不能为空，不能为空格。密码不能为空。昵称不能为空，不能为空格

    //长度限制，手机号必须为11位。密码不超过10位。昵称长度不超过20位

    //内容校验，手机号只能为数字，密码只能是数字、字母、下划线
    QString telTemp = tel;
    QString nameTemp = name;
    if(tel.isEmpty() || password.isEmpty() || name.isEmpty() || telTemp.remove(" ").isEmpty()
            || nameTemp.remove(" ").isEmpty()){
        QMessageBox::about(this, "提示", "手机号或密码或昵称不能为空");
        return;
    }
    if(tel.length()!=11 || password.length()>10 || name.length()>20){
        QMessageBox::about(this, "提示", "手机号必须为11位，密码不能超过10位，昵称不能超过20位");
        return;
    }

    //3.发送数据到kernel类
    Q_EMIT SIG_RegisterCommit(tel, password, name);
}

//重写关闭窗口事件
void LoginDialog::closeEvent(QCloseEvent *event)
{
    //1.接受关闭事件
    event->accept();
    //2.给Kernel发送信号，通知窗口关闭
    Q_EMIT SIG_Close();
}
