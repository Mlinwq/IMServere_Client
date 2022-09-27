#ifndef MYCHATDIALOG_H
#define MYCHATDIALOG_H

#include<QDialog>
#include<QVBoxLayout>
#include<QMenu>
#include<QCloseEvent>
#include"useritem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MyChatDialog; }
QT_END_NAMESPACE

class MyChatDialog : public QDialog
{
    Q_OBJECT

signals:
    //添加好友
    void SIG_AddFriend();
    //关闭窗口
    void SIG_Close();

public:
    MyChatDialog(QWidget *parent = nullptr);
    ~MyChatDialog();
    //添加好友
    void slot_addFriend(UserItem* userItem);
    //设置ui
    void slot_setInfo(QString name, int icon, QString feeling);
    //重写关闭窗口事件
    void closeEvent(QCloseEvent* event);

private slots:
    void on_pb_tool1_clicked();
    //点击菜单项的处理函数
    void slot_dealMenu(QAction* action);

private:
    Ui::MyChatDialog *ui;
    //QVBoxLayout是一个垂直布局的层
    QVBoxLayout* m_layout;
    QMenu* m_menu;
};
#endif // MYCHATDIALOG_H
