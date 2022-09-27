#include "mychatdialog.h"
#include "ui_mychatdialog.h"
#include<QICon>
#include<QPoint>
#include<QCursor>
#include<QMessageBox>

MyChatDialog::MyChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MyChatDialog)
{
    ui->setupUi(this);
    setWindowTitle("通信软件v1.0");

    //初始化QVBoxLayout指针
    m_layout = new QVBoxLayout;
    m_layout->setContentsMargins(0, 0, 0, 0);//设置上下左右间距
    m_layout->setSpacing(3);//设置每个控件彼此间的间距
    //设置控件的层
    ui->wdg_list->setLayout(m_layout);

    //创建Menu对象,指定父控件，负责子控件回收
    m_menu = new QMenu(this);
    //添加菜单项
    m_menu->addAction("添加好友");
    m_menu->addAction("拓展");
    //绑定菜单项的点击事件和处理函数
    connect(m_menu, SIGNAL(triggered(QAction*)),
            this, SLOT(slot_dealMenu(QAction*)));

}

MyChatDialog::~MyChatDialog()
{
    delete ui;
}

//添加好友
void MyChatDialog::slot_addFriend(UserItem *userItem)
{
    //往层上添加控件
    m_layout->addWidget(userItem);
}

//设置ui
void MyChatDialog::slot_setInfo(QString name, int icon, QString feeling)
{
    ui->lb_name->setText(name);
    ui->lb_feeling->setText(feeling);
    ui->pb_icon->setIcon(QIcon(QString(":/tx/%1.png").arg(icon)));
}

//重写关闭窗口事件
void MyChatDialog::closeEvent(QCloseEvent *event)
{
    //忽略关闭事件，在kernel里控制窗口关闭
    event->ignore();
    //提示窗口，确认是否关闭窗口
    if(QMessageBox::Yes == QMessageBox::question(this, "提示", "是否退出？")) {
        //通知kernel类关闭窗口
        Q_EMIT SIG_Close();
    }
}

//单击菜单
void MyChatDialog::on_pb_tool1_clicked()
{
    //按照鼠标点击位置显示菜单
    QPoint p = QCursor::pos();
    //获取菜单绝对高度并显示菜单
    QSize size = m_menu->sizeHint();
    m_menu->exec(QPoint(p.x(),p.y()-size.height()));
}

//点击菜单项的处理函数
void MyChatDialog::slot_dealMenu(QAction *action)
{
    if(action->text() == "添加好友"){
        //添加好友，发送信号给Kernel
        Q_EMIT SIG_AddFriend();
    }
    else if(action->text() == "拓展"){
        QMessageBox::about(this, "提示", "功能尚未拓展");
    }
}
