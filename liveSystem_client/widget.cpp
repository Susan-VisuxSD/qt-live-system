#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include "pack.h"
#include <QDebug>
#include "registerdialog.h"
#include <QJsonArray>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , m_pcsocket(new QTcpSocket(this))
    , m_name()
    , m_buffer()
    , m_puserinterface(nullptr)
    , m_plivingroom(nullptr)
{
    ui->setupUi(this);

    //    ui->lineEdit_password->setEchoMode(QLineEdit::Password);

    //点击登录按钮
    connect(ui->pushButton_login, &QPushButton::clicked, this, &Widget::loginbtn_clicked);

    //点击注册按钮
    connect(ui->pushButton_regis, &QPushButton::clicked, this, &Widget::registerbtn_clicked);

    init_network();
}

Widget::~Widget()
{
    delete ui;
    m_pcsocket->close();
}

//初始化函数
void Widget::init_network()
{
    //初始化函数中连接成功后发射信号的槽函数
    connect(m_pcsocket, &QTcpSocket::connected, this, &Widget::connect_ok);
    m_pcsocket->connectToHost("127.0.0.1", 7077);

    //接收服务器发来的信息
    connect(m_pcsocket, &QTcpSocket::readyRead, this, &Widget::ready_read);

    return;
}

//初始化函数中连接成功后发射信号的槽函数
void Widget::connect_ok()
{
    ui->pushButton_login->setEnabled(true);//连接失败按钮不可用
    ui->pushButton_regis->setEnabled(true);
    return;
}

void Widget::ready_read()
{
    m_buffer.append(m_pcsocket->readAll());
    while(true){
        if(m_buffer.size() <sizeof (qint32)) return;

        qint32 len;
        memcpy(&len, m_buffer.constData(), sizeof (qint32));

        if(m_buffer.size() < len + sizeof (qint32)) return;

        QByteArray jsonData = m_buffer.mid(sizeof (qint32), len);
        QJsonObject obj = Pack::deserialize(jsonData);
        int type = obj["type"].toInt();
        QJsonObject data = obj["data"].toObject();

        switch((Type)type){
        case TYPE_LOGIN:
            handle_login_result(data);
            break;
        case TYPE_REGISTER:
            handel_register_result(data);
            break;
        case TYPE_CREATEROOM:
            handel_createroom_result(data);
            break;
        case TYPE_JOINROOM:
            handel_joinroom_result(data);
            break;
        case TYPE_FLUSHROOM:
            handel_flushroom_result(data);
            break;
        case TYPE_GETNEWROOM:
            handel_getnewroom_result(data);
            break;
        case TYPE_GETNEWUSER:
            handel_getnewuser_result(data);
            break;
        case TYPE_FLUSHUSERS:
            handel_flushusers_result(data);
            break;
        case TYPE_ENDROOM:
            handel_endroom_result(data);
            break;
        case TYPE_EXITROOM:
            handel_exitroom_result(data);
            break;
        case TYPE_CHATMSG:
            handel_chatmsg_result(data);
            break;
        default:
            qDebug() << "未知枚举";
            break;
        }
        m_buffer.remove(0, len + sizeof (qint32));
    }
}

//点击登录按钮槽函数
void Widget::loginbtn_clicked()
{
    QString name = ui->lineEdit_name->text();
    QString pwd = ui->lineEdit_password->text();

    m_name = name;

    if(name.length() < 3 || name.length() > 12){
        QMessageBox::warning(this, "登录", "用户名不符合规范[3, 12]");
        return;
    }

    if(pwd.length() < 3 || pwd.length() > 12){
        QMessageBox::warning(this, "登录", "密码不符合规范[3, 12]");
        return;
    }

    QJsonObject data;
    data["username"] = name;
    data["password"] = pwd;

    Pack pack(TYPE_LOGIN);
    pack.set_data(data);

    QByteArray packet = pack.serialize();
    m_pcsocket->write(packet);

    return;
}

//点击注册按钮槽函数
void Widget::registerbtn_clicked()
{
    this->hide();
    RegisterDialog* regDialog = new RegisterDialog(this);
    regDialog->setAttribute(Qt::WA_DeleteOnClose);
    regDialog->show();

    connect(regDialog, &RegisterDialog::registerRequest, this, &Widget::handleRegister);

    connect(regDialog, &RegisterDialog::finished, this, [=](){
        this->show();//注册成功后显示登录界面
    });

    return;
}

//处理注册返回的数据
void Widget::handleRegister(QString name, QString pwd)
{
    QJsonObject data;
    data["username"] = name;
    data["password"] = pwd;

    Pack pack(TYPE_REGISTER);
    pack.set_data(data);

    QByteArray packet = pack.serialize();
    m_pcsocket->write(packet);

    return;
}

//处理直播间关闭的槽函数
void Widget::handleRoomClosed(const QString &roomid)
{
    m_puserinterface->remove_room(roomid);
    return;
}

//收到服务器返回登录结果
void Widget::handle_login_result(QJsonObject data)
{
    int log_rst = data["login"].toInt();
    switch(log_rst){
    case 0:
        QMessageBox::information(this, "登录", "登录成功");
        hide();//隐藏登录注册框

        //显示房间列表
        m_puserinterface = new UserInterFace(m_pcsocket, m_name,nullptr);
        m_puserinterface->show();

        break;
    case 1:
        QMessageBox::warning(this, "登录", "用户名不合符规范[3,12]");
        break;
    case 2:
        QMessageBox::warning(this, "登录", "密码不符合规范[3,12]");
        break;
    case 3:
        QMessageBox::warning(this, "登录", "用户名错误");
        break;
    case 4:
        QMessageBox::warning(this, "登录", "密码错误");
        break;
    case 5:
        QMessageBox::warning(this, "登录", "重复登录");
        break;
    case 6:
        QMessageBox::warning(this, "登录", "数据库错误");
        break;
    default:
        QMessageBox::warning(this, "登录", "未知错误");
    }
    return;
}

//收到服务器返回注册结果
void Widget::handel_register_result(QJsonObject data)
{
    int res_rst = data["register"].toInt();
    switch(res_rst){
    case 0:
        QMessageBox::information(this, "注册", "注册成功");
        //        hide();//隐藏登录注册框
        break;
    case 1:
        QMessageBox::warning(this, "注册", "用户名不合符规范[3,12]");
        break;
    case 2:
        QMessageBox::warning(this, "注册", "密码不符合规范[3,12]");
        break;
    case 3:
        QMessageBox::warning(this, "注册", "重复注册");
        break;
    case 4:
        QMessageBox::warning(this, "注册", "数据库错误");
        break;
    default:
        QMessageBox::warning(this, "注册", "未知错误");
    }
    return;
}

//收到服务器返回创建房间的结果
void Widget::handel_createroom_result(QJsonObject data)
{
    QString createroomid = data["newroomid"].toString();
    m_puserinterface->add_createroomid(createroomid);

    m_puserinterface->hide();//隐藏房间列表
    //进入直播界面
    m_plivingroom = new LivingRoom(m_pcsocket, createroomid, m_name, m_name, true);

    //当创建直播间或加入直播间后，连接信号
    connect(m_plivingroom, &LivingRoom::roomClosedbyOwner,
            this, &Widget::handleRoomClosed);

    connect(m_plivingroom, &LivingRoom::userExitRoom, [=](){
        if(m_puserinterface){
            m_puserinterface->show();
        }
        m_plivingroom = nullptr;// 清掉直播间指针
    });

    m_plivingroom->show();

    return;
}

//收到服务器返回加入房间的结果
void Widget::handel_joinroom_result(QJsonObject data)
{
    QString ownername = data["getname"].toString();
    QString roomid = data["roomid"].toString();

    m_puserinterface->hide();//隐藏房间列表
    //进入直播界面
    m_plivingroom = new LivingRoom(m_pcsocket, roomid, ownername, m_name, false);

    //当创建直播间或加入直播间后，连接信号
    connect(m_plivingroom, &LivingRoom::roomClosedbyOwner,
            this, &Widget::handleRoomClosed);

    connect(m_plivingroom, &LivingRoom::userExitRoom, [=](){
        if(m_puserinterface){
            m_puserinterface->show();
        }
        m_plivingroom = nullptr;// 清掉直播间指针
    });

    m_plivingroom->show();

    return;
}

//收到服务器返回的所有主播房间号的结果
void Widget::handel_flushroom_result(QJsonObject data)
{
    QJsonArray roomArray = data["rooms"].toArray();
    for(auto room: roomArray){
        QString roomid = room.toString();
        m_puserinterface->add_createroomid(roomid);
    }
    return;
}

//收到服务器返回的其他客户创建房间结果
void Widget::handel_getnewroom_result(QJsonObject data)
{
    QString getNewId = data["getnewid"].toString();
    m_puserinterface->add_createroomid(getNewId);

    return;
}

//收到服务器返回的新加入者名字结果
void Widget::handel_getnewuser_result(QJsonObject data)
{
    if(m_plivingroom){
        m_plivingroom->add_name(data["getnewname"].toString());
    }
    return;
}

//收到服务器返回的同房间其他用户结果(主播除外)
void Widget::handel_flushusers_result(QJsonObject data)
{
    QJsonArray usersArray = data["users"].toArray();

    if(!m_plivingroom) return;

    for(auto user: usersArray){
        QString joinname = user.toString();
        m_plivingroom->add_name(joinname);
    }

    return;
}

//收到服务器返回的关闭房间的结果
void Widget::handel_endroom_result(QJsonObject data)
{
    QString roomid = data["endroomid"].toString();

    //只有在这个直播间的人才提示
    if(m_plivingroom && m_plivingroom->get_roomid() == roomid){
        QMessageBox::information(this, "提示", "直播已结束");

        if(m_plivingroom){
            m_plivingroom->close();
            m_plivingroom = nullptr;
        }

        if(m_puserinterface){
            m_puserinterface->remove_room(roomid);
            m_puserinterface->show();
        }
    }

    //所有人都需要更新房间列表
    if(m_puserinterface){
        m_puserinterface->remove_room(roomid);
    }

    return;
}

//收到服务器返回的用户退出房间的结果
void Widget::handel_exitroom_result(QJsonObject data)
{
    QString exitname = data["exitname"].toString();
    QString exitroomid = data["exitroomid"].toString();

    if(m_plivingroom && m_plivingroom->get_roomid() == exitroomid){
        m_plivingroom->remove_name(exitname);
    }

    if(exitname == m_name && m_puserinterface){
        m_puserinterface->show();
        m_plivingroom = nullptr;
    }

    return;
}

//收到服务器返回的用户发送聊天消息的结果
void Widget::handel_chatmsg_result(QJsonObject data)
{
    QString roomid = data["roomid"].toString();
    QString name = data["name"].toString();
    QString msg = data["msg"].toString();

    if(m_plivingroom && m_plivingroom->get_roomid() == roomid){
        m_plivingroom->add_chatmsg(name, msg);
    }

    return;
}


