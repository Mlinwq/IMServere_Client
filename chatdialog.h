#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QWidget>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QWidget
{
    Q_OBJECT
signals:
    void SIG_SendChatMsg(int id, QString content);
public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    //设置窗口信息
    void slot_setInfo(QString name, int id);
    //在tb控件上显示聊天内容
    void slot_setChatMsg(QString content);
    //处理聊天对象不在线
    void slot_setUserOffline();

private slots:
    //点击发送按钮槽函数
    void on_pb_send_clicked();

private:
    Ui::ChatDialog *ui;
    QString m_name;
    int m_id;

};

#endif // CHATDIALOG_H
