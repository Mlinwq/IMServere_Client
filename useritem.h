#ifndef USERITEM_H
#define USERITEM_H

#include <QWidget>

namespace Ui {
class UserItem;
}

class UserItem : public QWidget
{
    Q_OBJECT

signals:
    void SIG_UserItemClicked(int id);

public:
    explicit UserItem(QWidget *parent = nullptr);
    ~UserItem();

    //设置用户信息
    void slot_setInfo(int id, QString name, int state, int icon = 1,
                      QString feeling = "......");
    //设置用户下线
    void slot_setOffline();

private slots:
    //好友控件头像单击事件
    void on_pb_icon_clicked();

public:
    int m_state;//用户状态
    QString m_name;//昵称

private:
    Ui::UserItem *ui;
    int m_id;//用户id

    int m_icon;//用户头像
    QString m_feeling;//用户签名
};

#endif // USERITEM_H
