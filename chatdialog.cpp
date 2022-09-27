#include "chatdialog.h"
#include "ui_chatdialog.h"
#include<QTime>

ChatDialog::ChatDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::slot_setInfo(QString name, int id)
{
    //保存用户信息
    m_name = name;
    m_id = id;
    //设置窗口标题  与[ip]的聊天
    setWindowTitle(QString("%1").arg(m_name));
}

//显示聊天内容
void ChatDialog::slot_setChatMsg(QString content)
{
    ui->tb_chat->append(QString("[%1] %2").arg(m_name).arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
}

//处理聊天对象不在线
void ChatDialog::slot_setUserOffline()
{
    //格式：[用户名] 时间 用户不在线
    QString str = QString("[%1] %2 用户不在线").arg(m_name).arg(QTime::currentTime().toString("hh:mm:ss"));
    ui->tb_chat->append(str);
}

void ChatDialog::on_pb_send_clicked()
{
    //1.获取输入的内容
    QString content = ui->te_chat->toPlainText();
    if(content.isEmpty()){
        return;
    }

    //2.清空textedit控件
    ui->te_chat->clear();

    //3.显示到上面tb控件
    //append换行追加
    ui->tb_chat->append(QString("[我] %1").arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);

    //4.发送数据到CKernel
    Q_EMIT SIG_SendChatMsg(m_id, content);

}
