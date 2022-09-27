 #include "ckernel.h"
#include"TcpClientMediator.h"
#include"PackDef.h"
#include"useritem.h"
#include<QMessageBox>
#include<QDebug>
#include<QTextCodec>
#include<QInputDialog>

#define NetProtocalMap(a) m_netProtocalMap[a-_DEF_PROTOCAL_BASE-10]

//QString->GB2312
void CKernel::Utf8ToGB2312(char* gbBuf, int nLen, QString &utf8)
{
    QTextCodec* gb2312 = QTextCodec::codecForName("gb2312");
    QByteArray ba = gb2312->fromUnicode(utf8);
    strcpy_s(gbBuf, nLen, ba.data());
}
//GB2312->QString
QString CKernel::GB2312ToUtf8(char* gbBuf)
{
    QTextCodec* gb2312 = QTextCodec::codecForName("gb2312");
    return gb2312->toUnicode(gbBuf);
}

//初始化协议映射表
void CKernel::setProtocalMap()
{
    memset(m_netProtocalMap, 0, _DEF_PROTOCAL_COUNT);
    NetProtocalMap(_DEF_PACK_REGISTER_RS) = &CKernel::slot_RegisterRs;
    NetProtocalMap(_DEF_PACK_LOGIN_RS) = &CKernel::slot_LoginRs;
    NetProtocalMap(_DEF_PACK_FRIEND_INFO) = &CKernel::slot_FriendInfoRq;
    NetProtocalMap(_DEF_PACK_CHAT_RQ) = &CKernel::slot_ChatRq;
    NetProtocalMap(_DEF_PACK_CHAT_RS) = &CKernel::slot_ChatRs;
    NetProtocalMap(_DEF_PACK_ADDFRIEND_RQ) = &CKernel::slot_AddFriendRq;
    NetProtocalMap(_DEF_PACK_ADDFRIEND_RS) = &CKernel::slot_AddFriendRs;
    NetProtocalMap(_DEF_PACK_OFFLINE_RQ) = &CKernel::slot_offlineRq;
}

CKernel::CKernel(QObject *parent) : QObject(parent), m_id(0)
{
    //new主窗口对象
    m_mainWnd = new MyChatDialog;
    //绑定添加好友信号的槽函数
    connect(m_mainWnd, SIGNAL(SIG_AddFriend()),
            this, SLOT(slot_AddFriend()));
    //绑定好友界面窗口关闭的信号和槽函数
    connect(m_mainWnd, SIGNAL(SIG_Close()),
            this, SLOT(slot_offline()));

    //new登录界面对象并显示
    m_loginDialog = new LoginDialog;
    m_loginDialog->showNormal();
    //绑定处理登录信息提交的信号和槽函数
    connect(m_loginDialog, SIGNAL(SIG_LoginCommit(QString,QString)),
            this, SLOT(slot_LoginCommit(QString,QString)));
    //绑定处理注册信息提交的信号和槽函数
    connect(m_loginDialog, SIGNAL(SIG_RegisterCommit(QString,QString,QString)),
            this, SLOT(slot_RegisterCommit(QString,QString,QString)));
    //绑定处理关闭窗口的信号和槽函数
    connect(m_loginDialog, SIGNAL(SIG_Close()),
            this, SLOT(slot_destroyInstance()));

    //中介者类对象
    m_tcpClientMediator = new TcpClientMediator;
    //绑定处理数据的信号和槽函数
    connect(m_tcpClientMediator,SIGNAL(SIG_ReadyData(long,char*,int)),
            this, SLOT(slot_ReadyData(long,char*,int)));

    //打开网络
    if(!m_tcpClientMediator->OpenNet()){
        QMessageBox::about(m_mainWnd, "提示", "打开网络失败");
        exit(0);
    }

    //初始化协议映射表
    setProtocalMap();

}

CKernel::~CKernel()
{
    //回收主窗口
    if(m_mainWnd){
        m_mainWnd->hide();
        delete m_mainWnd;
        m_mainWnd = NULL;
    }

    //回收中介者对象
    if(m_tcpClientMediator){
        m_tcpClientMediator->CloseNet();
        delete m_tcpClientMediator;
        m_tcpClientMediator = NULL;
    }
}

//处理接收到的数据
void CKernel::slot_ReadyData(long lSendIp, char *buf, int nLen)
{
    qDebug() << __func__;
    //1.取出协议头
    int nType = *(int*)buf;
    //2.判断协议头是否在映射范围内
    if(nType >= (_DEF_PROTOCAL_BASE + 10) &&
       nType <= (_DEF_PROTOCAL_BASE + 10 + _DEF_PROTOCAL_COUNT)){
        //3.计算数组下标，获取对应函数
        PFUN pf = NetProtocalMap(nType);
        //4.如果指针存在，调用对应处理函数
        if(pf){
            (this->*pf)(buf, nLen);
        }
    }
    //5.回收buf
    delete[] buf;
}

//处理注册请求回复
void CKernel::slot_RegisterRs(char *buf, int nLen)
{
    qDebug() << __func__;
    //1.拆包
    STRU_REGISTER_RS* rs = (STRU_REGISTER_RS*)buf;

    //2.根据注册结果提示用户
    switch(rs->result){
    case user_is_exist:
        QMessageBox::about(m_loginDialog, "提示", "注册失败，用户已存在");
        break;
    case register_sucess:
        QMessageBox::about(m_loginDialog, "提示", "注册成功");
        break;
    default:
        break;
    }
}

//处理登录请求回复
void CKernel::slot_LoginRs(char *buf, int nLen)
{
    qDebug() << __func__;
    //1.拆包
    STRU_LOGIN_RS* rs = (STRU_LOGIN_RS*)buf;
    //2.根据登录结果提示用户
    switch(rs->result){
    case user_not_exist:
        QMessageBox::about(m_loginDialog, "提示", "登陆失败，用户不存在");
        break;
    case password_error:
        QMessageBox::about(m_loginDialog, "提示", "登录失败，密码错误");
        break;
    case login_success:
        //QMessageBox::about(m_loginDialog, "提示", "登录成功");
        {
            m_loginDialog->hide();
            m_mainWnd->showNormal();
            m_id = rs->userid;
        }
        break;
    default:
        break;
    }
}

//处理好友信息请求
void CKernel::slot_FriendInfoRq(char *buf, int nLen)
{
    qDebug() << __func__;
    //1.拆包
    STRU_FRIEND_INFO* info = (STRU_FRIEND_INFO*)buf;

    //2.进行编码转换
    QString name = GB2312ToUtf8(info->name);
    QString feeling = GB2312ToUtf8(info->feeling);

    //3.判断是不是自己
    if(m_id == info->userid){
        //3.1是自己，更新登录用户界面
        m_mainWnd->slot_setInfo(name, info->icon, feeling);
        //3.2保存用户信息
        m_name = name;
        //3.3处理完直接返回
        return;
    }
    //4.如果不是自己，查看当前窗口是否有这个好友
    else{
        //5如果没有这个好友，则添加到控件上
        if(m_mapIdToUserItem.find(info->userid) == m_mapIdToUserItem.end()){
            //5.1创建一个useritem
            UserItem* item = new UserItem;
            //5.2给控件赋值
            item->slot_setInfo(info->userid, name, info->state, info->icon, feeling);
            //5.3绑定控件点击事件的信号与槽函数
            connect(item,SIGNAL(SIG_UserItemClicked(int)),
                    this,SLOT(slot_UserItemClicked(int)));
            //5.4创建聊天窗口
            ChatDialog* chat = new ChatDialog;
            //聊天窗口设置
            chat->slot_setInfo(name, info->userid);
            //5.5绑定聊天窗口发送数据的信号和槽函数
            connect(chat, SIGNAL(SIG_SendChatMsg(int,QString)),
                    this, SLOT(slot_SendChatMsg(int,QString)));
            //5.6把聊天窗口放map中管理
            m_mapIdToChatDialog[info->userid] = chat;
            //5.7把useritem添加到好友列表中
            m_mainWnd->slot_addFriend(item);
            //5.7把useritem放map中管理
            m_mapIdToUserItem[info->userid] = item;
        }
        //6如果有这个好友，则更新控件上的信息
        else{
            UserItem* item = m_mapIdToUserItem[info->userid];
            if(item && item->m_state == 0 && info->state == 1){
                //如果之前下线状态，现在上线，提示用户好友上线
                //qDebug() << QString("用户[%1]已上线").arg(name);
                QMessageBox::about(m_mainWnd, "提示", QString("用户[%1]已上线").arg(name));
            }
            item->slot_setInfo(info->userid, name, info->state, info->icon, feeling);
        }
    }
}

//处理聊天信息请求
void CKernel::slot_ChatRq(char *buf, int nLen)
{
    qDebug() << __func__;
    //1.拆包
    STRU_TCP_CHAT_RQ* rq = (STRU_TCP_CHAT_RQ*)buf;

    //2.查看聊天窗口是否存在
    if(m_mapIdToChatDialog.find(rq->userid) != m_mapIdToChatDialog.end()){
        //3.如果存在，根据id查找窗口并显示
        ChatDialog* chat = m_mapIdToChatDialog[rq->userid];
        //4.设置聊天内容
        chat->slot_setChatMsg(rq->content);
    }
}

//处理聊天信息回复
void CKernel::slot_ChatRs(char *buf, int nLen)
{
    qDebug() << __func__;
    //1.拆包
    STRU_TCP_CHAT_RS* rs= (STRU_TCP_CHAT_RS*)buf;

    //2.查看聊天窗口是否存在
    if(m_mapIdToChatDialog.count(rs->friendid) > 0){
        //3.如果存在，根据id查找窗口并显示
        ChatDialog* chat = m_mapIdToChatDialog[rs->friendid];
        //4.显示该好友不在线
        chat->slot_setUserOffline();
    }
}

//处理好友添加请求
void CKernel::slot_AddFriendRq(char *buf, int nLen)
{
    qDebug() << __func__;
    //1.拆包
    STRU_ADD_FRIEND_RQ* rq = (STRU_ADD_FRIEND_RQ*)buf;

    //2.弹窗提示用户是否添加好友
    STRU_ADD_FRIEND_RS rs;
    QString str = QString("用户[%1]想添加你为好友，是否同意？").arg(rq->username);
    if(QMessageBox::question(m_mainWnd, "添加好友", str) == QMessageBox::Yes){
        rs.result = add_success;
    }
    else{
        rs.result = user_refuse;
    }
    //3.构造请求结果，发给服务端
    rs.friendid = m_id;
    rs.userid = rq->userid;
    strcpy(rs.friendname, rq->friendname);
    m_tcpClientMediator->SendData(0, (char*)&rs, sizeof(rs));
}

//处理好友添加回复
void CKernel::slot_AddFriendRs(char *buf, int nLen)
{
    qDebug() << __func__;
    //1.拆包
    STRU_ADD_FRIEND_RS* rs = (STRU_ADD_FRIEND_RS*)buf;

    //2.好友昵称解码
    QString name = GB2312ToUtf8(rs->friendname);

    //3.根据结果提示不同信息
    switch(rs->result){
    case no_this_user:
        QMessageBox::about(m_loginDialog, "提示", QString("添加失败，没有用户[%1]").arg(name));
        break;
    case user_refuse:
        QMessageBox::about(m_loginDialog, "提示", QString("添加失败，用户[%1]拒绝添加好友").arg(name));
        break;
    case user_offline:
        QMessageBox::about(m_loginDialog, "提示", QString("添加失败，用户[%1]不在线").arg(name));
        break;
    case add_success:
        QMessageBox::about(m_loginDialog, "提示", "添加好友成功");
        break;
    default:
        break;
    }

}

//处理好友下线请求
void CKernel::slot_offlineRq(char *buf, int nLen)
{
    qDebug() << __func__;

    //1.拆包
    STRU_TCP_OFFLINE_RQ* rq = (STRU_TCP_OFFLINE_RQ*)buf;

    //2.根据好友id获取对应useritem，把好友头像置灰
    if(m_mapIdToUserItem.count(rq->userid) > 0){
        UserItem* ite = m_mapIdToUserItem[rq->userid];
        ite->slot_setOffline();
        QMessageBox::about(m_mainWnd, "提示", QString("用户[%1]已下线").arg(ite->m_name));
    }
}

//处理聊天内容请求
void CKernel::slot_SendChatMsg(int id, QString content)
{
    qDebug() << __func__;
    //1.封包
    STRU_TCP_CHAT_RQ rq;
    rq.userid = m_id;
    rq.friendid = id;

    //2.兼容中文
    std::string strContent = content.toStdString();
    strcpy(rq.content,strContent.c_str());

    //3.将聊天内容发送给服务器
    m_tcpClientMediator->SendData(0, (char*)&rq, sizeof(rq));
}

//处理登录信息提交
void CKernel::slot_LoginCommit(QString tel, QString password)
{
    qDebug() << __func__;
    //1.QString 转换为std::string
    std::string strTel = tel.toStdString();
    std::string strPassword = password.toStdString();

    //2.封包
    STRU_LOGIN_RQ rq;
    strcpy(rq.tel, strTel.c_str());
    strcpy(rq.password, strPassword.c_str());

    //3.发送登录请求
    m_tcpClientMediator->SendData(0, (char*)&rq, sizeof(rq));
}

//处理注册信息提交
void CKernel::slot_RegisterCommit(QString tel, QString password, QString name)
{
    qDebug() << __func__;
    //1.QString 转换为std::string
    std::string strTel = tel.toStdString();
    std::string strPassword = password.toStdString();
    //std::string strName = name.toStdString();

    //2.封包
    STRU_REGISTER_RQ rq;
    strcpy(rq.tel, strTel.c_str());
    strcpy(rq.password, strPassword.c_str());
    //strcpy(rq.name, strName.c_str());
    Utf8ToGB2312(rq.name, sizeof(rq.name), name);

    //3.发送注册请求
    m_tcpClientMediator->SendData(0, (char*)&rq, sizeof(rq));

}

//处理点击用户头像的请求
void CKernel::slot_UserItemClicked(int id)
{
    //显示聊天窗口
    if(m_mapIdToChatDialog.find(id) != m_mapIdToChatDialog.end()){
        ChatDialog* chat = m_mapIdToChatDialog[id];
        chat->showNormal();
    }
}

//处理添加好友的请求
void CKernel::slot_AddFriend()
{
    //1.弹出窗口输入好友昵称
    QString strName = QInputDialog::getText(m_mainWnd, "添加好友", "输入好友昵称");
    //如果点击cancel或者x会返回空，此时直接退出
    if(strName == NULL)
        return;

    //2.校验用户输入的昵称，昵称不能为空，长度不超过20
    if(strName.length() <= 0 || strName.length() > 20){
        QMessageBox::about(m_loginDialog, "提示", "输入名字不合法");
        return;
    }

    //3.校验昵称是不是自己，如果是自己，提示
    if(strName == m_name){
        QMessageBox::about(m_loginDialog, "提示", "不能添加自己");
        return;
    }

    //4.校验昵称是不是已经是好友，如果是，提示
    for(auto ite = m_mapIdToUserItem.begin(); ite != m_mapIdToUserItem.end(); ite++){
        UserItem* item = *ite;
        if(strName == item->m_name){
            QMessageBox::about(m_loginDialog, "提示", "已经是好友了，请勿重复添加");
            return;
        }
    }

    //5.发送添加好友请求到服务端
    STRU_ADD_FRIEND_RQ rq;
    rq.userid = m_id;
    //好友昵称需要编码，因为要查服务器端的数据库
    Utf8ToGB2312(rq.friendname, sizeof(rq.friendname), strName);

    //用户名称不需要编码，只用于客户端显示，不需要服务器和数据库处理
    strcpy(rq.username, m_name.toStdString().c_str());
    m_tcpClientMediator->SendData(0, (char*)&rq, sizeof(rq));
}

//处理登录窗口关闭事件
void CKernel::slot_destroyInstance()
{
    qDebug() << __func__;

    //好友列表对象
    if(m_mainWnd){
        m_mainWnd->hide();
        delete m_mainWnd;
        m_mainWnd = NULL;
    }

    //登录/注册窗口对象
    if(m_loginDialog){
        m_loginDialog->hide();
        delete m_loginDialog;
        m_loginDialog = NULL;
    }

    //中介者类对象
    if(m_tcpClientMediator){
        m_tcpClientMediator->CloseNet();
        delete m_tcpClientMediator;
        m_tcpClientMediator = NULL;
    }

    //聊天窗口对象
    for(auto ite = m_mapIdToChatDialog.begin(); ite != m_mapIdToChatDialog.end();){
        ChatDialog* chat = *ite;
        chat->hide();
        delete chat;
        chat = NULL;
        ite = m_mapIdToChatDialog.erase(ite);
    }

    exit(0);
}

//处理好友界面窗口关闭事件
void CKernel::slot_offline()
{
    qDebug() << __func__;

    //1.通知服务器下线了
    STRU_TCP_OFFLINE_RQ rq;
    rq.userid = m_id;
    m_tcpClientMediator->SendData(0, (char*)&rq, sizeof(rq));

    //2.回收资源
    slot_destroyInstance();
}


