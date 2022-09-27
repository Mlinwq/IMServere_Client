#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>
#include<QMap>
#include"mychatdialog.h"
#include"INetMediator.h"
#include"PackDef.h"
#include"logindialog.h"
#include"useritem.h"
#include"chatdialog.h"

class CKernel;
//函数指针
typedef void (CKernel::*PFUN)(char* buf, int nLen) ;

class CKernel : public QObject
{
    Q_OBJECT
public:
    explicit CKernel(QObject *parent = nullptr);
    ~CKernel();

signals:

public slots:
    //处理接收到的数据
    void slot_ReadyData(long lSendIp, char* buf, int nLen);
    //处理注册请求回复
    void slot_RegisterRs(char* buf, int nLen);
    //处理登录请求回复
    void slot_LoginRs(char* buf, int nLen);
    //处理好友信息请求
    void slot_FriendInfoRq(char* buf, int nLen);
    //处理聊天信息请求
    void slot_ChatRq(char* buf, int nLen);
    //处理聊天信息回复
    void slot_ChatRs(char* buf, int nLen);
    //处理好友添加请求
    void slot_AddFriendRq(char* buf, int nLen);
    //处理好友添加回复
    void slot_AddFriendRs(char* buf, int nLen);
    //处理好友下线请求
    void slot_offlineRq(char* buf, int nLen);


    //处理聊天内容请求
    void slot_SendChatMsg(int id, QString content);
    //处理登录信息提交
    void slot_LoginCommit(QString tel, QString password);
    //处理注册信息提交
    void slot_RegisterCommit(QString tel, QString password, QString name);
    //处理点击用户头像的请求
    void slot_UserItemClicked(int id);
    //处理添加好友的请求
    void slot_AddFriend();
    //处理登录窗口关闭事件
    void slot_destroyInstance();
    //处理好友界面窗口关闭事件
    void slot_offline();

private:
    //初始化协议映射表
    void setProtocalMap();
    //QString->GB2312
    static void Utf8ToGB2312(char* gbBuf, int nLen, QString &utf8);
    //GB2312->QString
    static QString GB2312ToUtf8(char* gbBuf);

public:
    QString m_name;

private:
    MyChatDialog* m_mainWnd;
    LoginDialog* m_loginDialog;
    INetMediator* m_tcpClientMediator;
    //协议映射表(函数指针数组)[协议头-base] -> 处理函数
    PFUN m_netProtocalMap[_DEF_PROTOCAL_COUNT];
    //保存UserItem和id的映射关系
    QMap<int, UserItem*> m_mapIdToUserItem;
    //保存聊天窗口和id的映射关系
    QMap<int, ChatDialog*> m_mapIdToChatDialog;
    int m_id;
};

#endif // CKERNEL_H
