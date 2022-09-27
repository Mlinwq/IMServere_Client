#include "useritem.h"
#include "ui_useritem.h"
#include<QIcon>
#include<QBitmap>

UserItem::UserItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserItem)
{
    ui->setupUi(this);
}

UserItem::~UserItem()
{
    delete ui;
}

//设置用户信息
void UserItem::slot_setInfo(int id, QString name, int state, int icon , QString feeling)
{
    //1.保存用户信息
    m_id = id;//用户id
    m_name = name;//昵称
    m_state = state;//用户状态
    m_icon = icon;//用户头像
    m_feeling = feeling;//用户签名

    //2.设置昵称和签名
    ui->lb_name->setText(m_name);
    ui->lb_feeling->setText(m_feeling);

    //3.设置头像
    QString iconPath = QString(":/tx/%1.png").arg(m_icon);

    //4.判断用户在线状态
    if(m_state == 1){
        //在线，彩色图标
        ui->pb_icon->setIcon(QIcon(iconPath));
    }
    else{
        //不在线，灰色图标
        QBitmap bmp;
        bmp.load(iconPath);
        ui->pb_icon->setIcon(bmp);
    }
    //5.重绘
    this->repaint();
}

//设置用户下线
void UserItem::slot_setOffline()
{
    //1.设置用户状态为下线
    m_state = 0;

    //2.将用户头像置灰
    QString iconPath = QString(":/tx/%1.png").arg(m_icon);
    QBitmap bmp;
    bmp.load(iconPath);
    ui->pb_icon->setIcon(bmp);

    //3.重绘
    this->repaint();
}

//好友控件头像单击事件
void UserItem::on_pb_icon_clicked()
{
    //发送信号，通知kernel类，发送用户id
    Q_EMIT SIG_UserItemClicked(m_id);
}
