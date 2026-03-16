#include "userinterface.h"
#include "ui_userinterface.h"
#include "pack.h"
#include <QMessageBox>

UserInterFace::UserInterFace(QTcpSocket* pcsocket, const QString& name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserInterFace),
    m_ptpcsocket(pcsocket),
    m_name(name),
    m_joinroomid()
{
    ui->setupUi(this);

    //点击创建房间按钮
    connect(ui->pushButton_createroom, &QPushButton::clicked, this, &UserInterFace::createRoombtn_clicked);

    //点击加入房间按钮
    connect(ui->pushButton_joinroom, &QPushButton::clicked, this, &UserInterFace::joinRoombtn_clicked);

    //点击房间号
    connect(ui->listWidget_rooms, &QListWidget::itemClicked, this, &UserInterFace::itembtn_clicked);

    init_window();
    flush_rooms();
}

UserInterFace::~UserInterFace()
{
    delete ui;
}

//点击创建房间按钮的槽函数
void UserInterFace::createRoombtn_clicked()
{
    Pack pack(TYPE_CREATEROOM);

    QByteArray packet = pack.serialize();
    m_ptpcsocket->write(packet);
    //把枚举值传给服务器，让服务器创建id再传回来

    return;
}

//点击加入房间按钮的槽函数
void UserInterFace::joinRoombtn_clicked()
{
    if(m_joinroomid.isEmpty()){
        QMessageBox::warning(this, "警告", "请先选中要加入的房间");
        return;
    }

    QJsonObject data;
    data["joinroomid"] = m_joinroomid;

    Pack pack(TYPE_JOINROOM);
    pack.set_data(data);
    QByteArray packet = pack.serialize();
    m_ptpcsocket->write(packet);

    return;
}

//点击房间列表的房间号,并获取要加入房间的房间号的槽函数
void UserInterFace::itembtn_clicked(QListWidgetItem* item)
{
    m_joinroomid = item->text();
    return;
}

//修改窗口图标和名字
void UserInterFace::init_window()
{
    setWindowTitle("UserName: " + m_name);//修改窗口名字
    return;
}

//新用户登录成功向服务器拉取所有主播房间号
void UserInterFace::flush_rooms()
{
    ui->listWidget_rooms->clear();

    Pack pack;
    pack.set_type(TYPE_FLUSHROOM);
    m_ptpcsocket->write(pack.serialize());
}

void UserInterFace::closeEvent(QCloseEvent *event)
{
    QApplication::quit();
}

//往名为listWidget_rooms的列表控件里添加一个新的列表项
void UserInterFace::add_createroomid(const QString &createroomid)
{
    for(int i = 0; i < ui->listWidget_rooms->count(); i++){
        if(ui->listWidget_rooms->item(i)->text() == createroomid){
            return;
        }
    }
    ui->listWidget_rooms->addItem(new QListWidgetItem(createroomid));
    return;
}

const QString &UserInterFace::get_joinroomid() const
{
    return m_joinroomid;
}

//移除结束直播的房间
void UserInterFace::remove_room(const QString &roomid)
{
    for(int i = 0; i < ui->listWidget_rooms->count(); ++i){
        QListWidgetItem* item = ui->listWidget_rooms->item(i);
        if(item->text() == roomid){
            delete ui->listWidget_rooms->takeItem(i);//从界面上移除已结束的直播房间号
            return;
        }
    }
}
