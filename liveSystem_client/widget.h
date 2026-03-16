#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include "userinterface.h"
#include "livingroom.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public:
    void init_network();//初始化函数

public slots:
    void connect_ok();//初始化函数中连接成功后发射信号的槽函数
    void ready_read();//接收服务器发来的信息

public slots:
    void loginbtn_clicked();//点击登录按钮槽函数
    void registerbtn_clicked();//点击注册按钮槽函数
    void handleRegister(QString name, QString pwd);//处理注册返回的数据
    void handleRoomClosed(const QString& roomid);//处理直播间关闭的槽函数

public:
    void handle_login_result(QJsonObject data);//收到服务器返回登录结果
    void handel_register_result(QJsonObject data);//收到服务器返回注册结果
    void handel_createroom_result(QJsonObject data);//收到服务器返回创建房间的结果
    void handel_joinroom_result(QJsonObject data);//收到服务器返回加入房间的结果
    void handel_flushroom_result(QJsonObject data);//收到服务器返回的所有主播房间号的结果
    void handel_getnewroom_result(QJsonObject data);//收到服务器返回的其他客户创建房间结果
    void handel_getnewuser_result(QJsonObject data);//收到服务器返回的新加入者名字结果
    void handel_flushusers_result(QJsonObject data);//收到服务器返回的同房间其他用户结果(主播除外)
    void handel_endroom_result(QJsonObject data);//收到服务器返回的关闭房间的结果
    void handel_exitroom_result(QJsonObject data);//收到服务器返回的用户退出房间的结果
    void handel_chatmsg_result(QJsonObject data);//收到服务器返回的用户发送聊天消息的结果

private:
    Ui::Widget *ui;
    QTcpSocket* m_pcsocket;
    QString m_name;
    QByteArray m_buffer;
    UserInterFace* m_puserinterface;
    LivingRoom* m_plivingroom;
};
#endif // WIDGET_H
