#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include "mypcsocket.h"
#include "livesystemdb.h"
#include <QMap>

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
    void init_network();//initialize初始化，初始化函数
    void init_db();//数据库初始化函数

public slots:
    void new_connection();//初始化函数中，和客户端连接的槽函数
    void ready_read();//读取客户端信息槽函数
    void dis_connected();//客户端断开槽函数

public:
    void user_login(Mypcsocket* pmypcsocket, QJsonObject data);//处理客户端登录逻辑
    void user_res(Mypcsocket* pmypcsocket, QJsonObject data);//处理客户端注册逻辑
    void user_createroom(Mypcsocket* pmypcsocket, QJsonObject data);//处理客户端创建房间逻辑
    void user_joinroom(Mypcsocket* pmypcsocket, QJsonObject data);//处理客户端加入房间逻辑
    void user_flushroom(Mypcsocket* pmypcsocket, QJsonObject data);//处理新客户登录拉取已创建的房间逻辑
    void user_endroom(Mypcsocket* pmypcsocket, QJsonObject data);//处理客户端关闭房间逻辑
    void user_exitroom(Mypcsocket* pmypcsocket, QJsonObject data);//处理客户端加入者退出房间逻辑
    void user_chatmsg(Mypcsocket* pmypcsocket, QJsonObject data);//处理客户端发送聊天消息逻辑

public:
    Mypcsocket* find_pmypcsocket(QTcpSocket* pcsocket);

private:
    Ui::Widget *ui;
    QTcpServer* m_pcserver;
    QList<Mypcsocket*> m_clients;
    QMap<QString, Mypcsocket*> m_rooms;// 房间号 -> 主播socket
    QMap<QString, QList<Mypcsocket*>> m_roomUsers;// 房间号 -> 房间内用户
    quint32 m_roomid;
};
#endif // WIDGET_H
