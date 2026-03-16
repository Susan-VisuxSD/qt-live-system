#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QWidget>
#include <QTcpSocket>
#include <QListWidgetItem>

namespace Ui {
class UserInterFace;
}

class UserInterFace : public QWidget
{
    Q_OBJECT

public:
    explicit UserInterFace(QTcpSocket* pcsocket, const QString& name, QWidget *parent = nullptr);
    ~UserInterFace();

public slots:
    void createRoombtn_clicked();//点击创建房间按钮的槽函数
    void joinRoombtn_clicked();//点击加入房间按钮的槽函数
    void itembtn_clicked(QListWidgetItem* item);//点击房间列表的房间号,并获取要加入房间的房间号的槽函数

public:
    void init_window();//修改窗口图标和名字
    void flush_rooms();//新用户登录成功向服务器拉取所有主播房间号
    void closeEvent(QCloseEvent *event);

public:
    void add_createroomid(const QString& createroomid);//往名为listWidget_rooms的列表控件里添加一个新的列表项
    const QString& get_joinroomid()const;
    void remove_room(const QString& roomid);//移除结束直播的房间

private:
    Ui::UserInterFace *ui;
    QTcpSocket* m_ptpcsocket;
    QString m_name;
    QString m_joinroomid;
};

#endif // USERINTERFACE_H
