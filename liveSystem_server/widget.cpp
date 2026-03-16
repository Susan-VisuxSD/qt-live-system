#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include "pack.h"
#include <QJsonArray>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , m_pcserver(new QTcpServer(this))
    , m_roomid(0)
{
    ui->setupUi(this);
    init_network();
    init_db();
}

Widget::~Widget()
{
    delete ui;
}

//initialize初始化，初始化函数
void Widget::init_network()
{
    if(!m_pcserver->listen(QHostAddress::Any, 7077)){
        QMessageBox::warning(this, "监听", "监听失败");
        return;
    }
    connect(m_pcserver, &QTcpServer::newConnection, this, &Widget::new_connection);
}

//数据库初始化函数
void Widget::init_db()
{
    if(!Livesystemdb::get_instance().init_db()){
        QMessageBox::warning(this, "数据库连接失败", Livesystemdb::get_instance().last_error());
        return;
    }
//    qDebug() << "连接数据库成功";
    return;
}

//初始化函数中，和客户端连接的槽函数,创建客户端对象
void Widget::new_connection()
{
    Mypcsocket* pmypcsocket = new Mypcsocket(m_pcserver->nextPendingConnection());
    m_clients.append(pmypcsocket);//第一个出现的问题，容器中数据的存和删是配套的，有存储，就要有释放

    //readyread: socket 接收到数据并且数据已经到达缓冲区，可以读取了
    connect(pmypcsocket->get_pclient(), &QTcpSocket::readyRead, this, &Widget::ready_read);

    //客户端连接断开
    connect(pmypcsocket->get_pclient(), &QTcpSocket::disconnected, this, &Widget::dis_connected);
}

//读取客户端信息槽函数,找到对应客户端
void Widget::ready_read()
{
    Mypcsocket* pmypcsocket = find_pmypcsocket(qobject_cast<QTcpSocket*>(sender()));
    if(pmypcsocket == nullptr){
        return;
    }

    pmypcsocket->m_buffer.append(pmypcsocket->readAll());

    while(true){
        if(pmypcsocket->m_buffer.size() < sizeof (qint32)) return;

        qint32 len;
        memcpy(&len, pmypcsocket->m_buffer.data(), sizeof (qint32));

        if(pmypcsocket->m_buffer.size() < len + sizeof (qint32)) return;

        QByteArray jsonData = pmypcsocket->m_buffer.mid(sizeof (qint32), len);
        QJsonObject obj  = Pack::deserialize(jsonData);

        int type = obj["type"].toInt();
        QJsonObject data = obj["data"].toObject();

        switch ((Type)type) {
        case TYPE_LOGIN:
            user_login(pmypcsocket, data);
            break;
        case TYPE_REGISTER:
            user_res(pmypcsocket, data);
            break;
        case TYPE_CREATEROOM:
            user_createroom(pmypcsocket, data);
            break;
        case TYPE_JOINROOM:
            user_joinroom(pmypcsocket, data);
            break;
        case TYPE_FLUSHROOM:
            user_flushroom(pmypcsocket, data);
            break;
        case TYPE_ENDROOM:
            user_endroom(pmypcsocket, data);
            break;
        case TYPE_EXITROOM:
            user_exitroom(pmypcsocket, data);
            break;
        case TYPE_CHATMSG:
            user_chatmsg(pmypcsocket, data);
            break;
        default:
            qDebug() << "未知枚举值";
            break;
        }

        // 删除已经解析的数据
        pmypcsocket->m_buffer.remove(0, len + sizeof (qint32));
    }
}

//客户端断开槽函数，找到客户端，删除客户信息，释放资源
void Widget::dis_connected()
{
    Mypcsocket* pmypcsocket = find_pmypcsocket(qobject_cast<QTcpSocket*>(sender()));
    if(pmypcsocket == nullptr){
        return;
    }

    QString roomid = pmypcsocket->get_joinroomid();
    QString createroomid = pmypcsocket->get_createroomid();

    if(!createroomid.isEmpty()){
        user_endroom(pmypcsocket, QJsonObject());
    }else if(!roomid.isEmpty() && m_roomUsers.contains(roomid)){
        m_roomUsers[roomid].removeOne(pmypcsocket);//需要判断一下
    }

    m_clients.removeOne(pmypcsocket);

    pmypcsocket->close();
    delete pmypcsocket;

    return;
}

//处理客户端登录逻辑
void Widget::user_login(Mypcsocket *pmypcsocket, QJsonObject data)
{
    QString username = data["username"].toString();
    QString password = data["password"].toString();
    int login_rst = Livesystemdb::get_instance().user_login(username, password);

    //数据库
    //判断是否有已经登录
    for(auto potherpmytcpsocket: m_clients){
        if(potherpmytcpsocket->get_name() == username){
            login_rst = 5;
            break;
        }
    }
    if(0 == login_rst){
        pmypcsocket->set_name(username);
    }

    //构造返回json
    QJsonObject rst_data;
    rst_data["login"] = login_rst;

    Pack pack;
    pack.set_type(TYPE_LOGIN);
    pack.set_data(rst_data);
    pmypcsocket->write(pack.serialize());

    return;
}

//处理客户端注册逻辑
void Widget::user_res(Mypcsocket *pmypcsocket, QJsonObject data)
{
    QString username = data["username"].toString();
    QString password = data["password"].toString();
    int register_rst = Livesystemdb::get_instance().user_res(username, password);

    //构造返回json
    QJsonObject rst_data;
    rst_data["register"] = register_rst;

    Pack pack;
    pack.set_type(TYPE_REGISTER);
    pack.set_data(rst_data);
    pmypcsocket->write(pack.serialize());

//    qDebug() << "已返回用户注册信息给客户端";
    return;
}

//处理客户端创建房间逻辑
void Widget::user_createroom(Mypcsocket *pmypcsocket, QJsonObject data)
{
    m_roomid++;
    QString roomid = QString::number(m_roomid);

    //保存房间
    m_rooms[roomid] = pmypcsocket;

    //主播加入房间
    m_roomUsers[roomid].append(pmypcsocket);
    pmypcsocket->set_createroomid(roomid);
    pmypcsocket->set_joinroomid(roomid);

    QJsonObject roomdata;
    roomdata["newroomid"] = roomid;

    Pack pack;
    pack.set_data(roomdata);
    pack.set_type(TYPE_CREATEROOM);
    QByteArray packet = pack.serialize();

    //和客户端通信的子套接字中保存客户端创建的房间号
    pmypcsocket->write(pack.serialize());

    //把房间号返回给已经登录成功的用户
    QJsonObject getidData;
    getidData["getnewid"] = roomid;

    pack.clear();//清理pack
    pack.set_type(TYPE_GETNEWROOM);//装入新的type
    pack.set_data(getidData);//装入新的roomid

    for(auto pothermytcpsocket: m_clients){
        if((!pothermytcpsocket->get_name().isEmpty()) && (pothermytcpsocket != pmypcsocket)){
            pothermytcpsocket->write(pack.serialize());
        }
    }

    return;
}

//处理客户端加入房间逻辑
void Widget::user_joinroom(Mypcsocket *pmypcsocket, QJsonObject data)
{
    QString joinroom_id = data["joinroomid"].toString();

    if(!m_rooms.contains(joinroom_id)) return;//需要判断房间号是否已经存在
    Mypcsocket* anchorroomid = m_rooms[joinroom_id];
    if(anchorroomid == nullptr) return;

    pmypcsocket->set_joinroomid(joinroom_id);

    if(!m_roomUsers[joinroom_id].contains(pmypcsocket)){
        m_roomUsers[joinroom_id].append(pmypcsocket);//需要判断房间号是否已经存在
    }

    //给加入者发送主播名字
    Pack pack;
    pack.set_type(TYPE_JOINROOM);
    Mypcsocket* anchor = m_rooms.value(joinroom_id, nullptr);//anchor锚点
    if(anchor == nullptr) return;

    QJsonObject nameData;
    nameData["getname"] = anchor->get_name();
    nameData["roomid"] = joinroom_id;
    pack.set_data(nameData);
    pmypcsocket->write(pack.serialize());

    //把房间已有成员发送给新加入者
    pack.clear();
    pack.set_type(TYPE_FLUSHUSERS);
    //找到同直播间内其他用户的名字给加入者发过去（主播除外）
    QJsonArray userArray;
    for(auto user: m_roomUsers[joinroom_id]){
        if(user != pmypcsocket && user != anchor){
            userArray.append(user->get_name());
        }
    }
    QJsonObject usersData;
    usersData["users"] = userArray;
    pack.set_data(usersData);
    pmypcsocket->write(pack.serialize());

    //通知房间其他人有新用户加入
    pack.clear();
    pack.set_type(TYPE_GETNEWUSER);
    QJsonObject nameDatas;
    nameDatas["getnewname"] = pmypcsocket->get_name();
    pack.set_data(nameDatas);
    for(auto user: m_roomUsers[joinroom_id]){
        if(user != pmypcsocket){
            user->write(pack.serialize());
        }
    }

    return;
}

//处理新客户登录拉取已创建的房间逻辑
void Widget::user_flushroom(Mypcsocket *pmypcsocket, QJsonObject data)
{
    Pack pack;
    pack.set_type(TYPE_FLUSHROOM);

    //用QJsonArray存放所有房间号
    QJsonArray roomArray;

    for(auto roomid: m_rooms.keys()){
        roomArray.append(roomid);
    }

    //把房间号数组放到一个QJsonObject里，然后作为Pack的数据
    QJsonObject sendData;
    sendData["rooms"] = roomArray;
    pack.set_data(sendData);

    pmypcsocket->write(pack.serialize());

    return;
}

//处理客户端关闭房间逻辑
void Widget::user_endroom(Mypcsocket *pmypcsocket, QJsonObject data)
{
    QString roomid = data["endroomid"].toString();

    Pack pack;
    pack.set_type(TYPE_ENDROOM);

    QJsonObject sendData;
    sendData["endroomid"] = roomid;
    pack.set_data(sendData);

    //通知所有在线用户
    for(auto client: m_clients){
        client->write(pack.serialize());
    }

    //通知房间所有人直播结束
    for(auto user: m_roomUsers[roomid]){

        //清空房间用户记录
        user->set_joinroomid("");//给joinroomid这个成员变量赋值为空字符串
    }

    //主播也要清空
    pmypcsocket->set_createroomid("");

    //删除房间
    m_roomUsers.remove(roomid);
    m_rooms.remove(roomid);

    return;
}

//处理客户端加入者退出房间逻辑
void Widget::user_exitroom(Mypcsocket *pmypcsocket, QJsonObject data)
{
    QString roomid = pmypcsocket->get_joinroomid();

    if(roomid.isEmpty()) return;
    if(!m_roomUsers.contains(roomid)) return;

    //从房间用户列表删除
    m_roomUsers[roomid].removeOne(pmypcsocket);

    //通知房间其他人
    Pack pack;
    pack.set_type(TYPE_EXITROOM);

    QJsonObject sendData;
    sendData["exitname"] = pmypcsocket->get_name();
    sendData["exitroomid"] = roomid;
    pack.set_data(sendData);

    for(auto user: m_roomUsers[roomid]){
        user->write(pack.serialize());
    }

    //清空用户房间信息
    pmypcsocket->set_joinroomid("");

    return;
}

//处理客户端发送聊天消息逻辑
void Widget::user_chatmsg(Mypcsocket *pmypcsocket, QJsonObject data)
{
    QString roomid = data["roomid"].toString();

    Pack pack;
    pack.set_type(TYPE_CHATMSG);
    pack.set_data(data);

    //广播给房间所有人
    for(auto user: m_roomUsers[roomid]){
        user->write(pack.serialize());
    }

    return;
}

Mypcsocket *Widget::find_pmypcsocket(QTcpSocket *pcsocket)
{
    for(auto pmypcsocket: m_clients){
        if(pmypcsocket->get_pclient() == pcsocket){
            return pmypcsocket;
        }
    }
    return NULL;
}

