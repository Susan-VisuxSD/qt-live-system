#ifndef LIVINGROOM_H
#define LIVINGROOM_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class LivingRoom;
}

class LivingRoom : public QWidget
{
    Q_OBJECT

public:
    explicit LivingRoom(QTcpSocket* ptcpsocket, const QString& roomid,
                        const QString& ownername, const QString& name,
                        bool isowner, QWidget *parent = nullptr);
    ~LivingRoom();

public:
    void init_window();
    void closeEvent(QCloseEvent *event);//用户退出直播间

public:
    void add_name(const QString& name);
    void remove_name(const QString& name);
    void add_chatmsg(const QString& name, const QString& msg);

public:
    const QString& get_roomid()const;

public slots:
    void startlivingbtn_clicked();//用户点击开启直播按钮槽函数
    void closelivingbtn_clicked();//用户点击关闭直播按钮槽函数
    void sendmsg_clicked();//用户点击发送按钮

signals:
    void roomClosedbyOwner(const QString& roomid);//直播结束的房间
    void userExitRoom();//用户退出房间

private:
    Ui::LivingRoom *ui;
    QTcpSocket* m_ptcpsocket;
    QString m_roomid;
    QString m_pownername;
    QString m_name;
    bool m_isowner;
};

#endif // LIVINGROOM_H
