#include "livingroom.h"
#include "ui_livingroom.h"
#include <QMessageBox>
#include "pack.h"
#include <QCloseEvent>

LivingRoom::LivingRoom(QTcpSocket* ptcpsocket, const QString& roomid,
                       const QString& ownername, const QString& name,
                       bool isowner, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LivingRoom),
    m_ptcpsocket(ptcpsocket),
    m_roomid(roomid),
    m_pownername(ownername),
    m_name(name),
    m_isowner(isowner)
{
    ui->setupUi(this);

    //用户点击开启直播按钮
    connect(ui->pushButton_openlive, &QPushButton::clicked, this, &LivingRoom::startlivingbtn_clicked);

    //用户点击关闭直播按钮
    connect(ui->pushButton_closelive, &QPushButton::clicked, this, &LivingRoom::closelivingbtn_clicked);

    //用户点击发送按钮
    connect(ui->pushButton_sendInfor, &QPushButton::clicked, this, &LivingRoom::sendmsg_clicked);

    init_window();
}

LivingRoom::~LivingRoom()
{
    delete ui;
}

void LivingRoom::init_window()
{
    setWindowTitle("用户" + m_name);
    ui->label_roomid->setText("房间" + m_roomid);

    if(m_isowner){
        ui->listWidget_allusers->addItem(new QListWidgetItem(m_pownername));
        return;
    }

    ui->listWidget_allusers->addItem(new QListWidgetItem(m_pownername));
    ui->listWidget_allusers->addItem(new QListWidgetItem(m_name));

    return;
}

//用户退出直播间
void LivingRoom::closeEvent(QCloseEvent *event)
{
    Pack pack;
    if(m_isowner){
        //如果是主播关闭房间，通知服务器结束直播
        pack.set_type(TYPE_ENDROOM);

        QJsonObject data;
        data["endroomid"] = m_roomid;

        pack.set_data(data);
        m_ptcpsocket->write(pack.serialize());

        emit roomClosedbyOwner(m_roomid);//通知widget移除房间
    }else{
        //普通用户退出房间
        pack.set_type(TYPE_EXITROOM);

        QJsonObject data;
        data["exitroomid"] = m_roomid;
        data["exitname"] = m_name;

        pack.set_data(data);
        m_ptcpsocket->write(pack.serialize());

        emit userExitRoom();//通知widget显示自己的userinterface
    }

    this->hide();

    event->accept();

    return;
}

void LivingRoom::add_name(const QString &name)
{
    ui->listWidget_allusers->addItem(new QListWidgetItem(name));
    return;
}

void LivingRoom::remove_name(const QString &name)
{
    for(int i = 0; i < ui->listWidget_allusers->count(); i++){
        QListWidgetItem* item = ui->listWidget_allusers->item(i);

        if(item->text() == name){
            delete ui->listWidget_allusers->takeItem(i);
            return;
        }
    }
}

void LivingRoom::add_chatmsg(const QString &name, const QString &msg)
{
    QString line = name + ": " + msg;
    ui->textEdit_showtalksInfo->append(line);

    return;
}

const QString &LivingRoom::get_roomid() const
{
    return m_roomid;
}

//用户点击开启直播按钮槽函数
void LivingRoom::startlivingbtn_clicked()
{
    if(!m_isowner){
        QMessageBox::warning(this, "警告", "只有主播才能开启直播");
        return;
    }

    QMessageBox::information(this, "提示", "直播已开启");

    return;
}

//用户点击关闭直播按钮槽函数
void LivingRoom::closelivingbtn_clicked()
{
    if(!m_isowner){
        QMessageBox::warning(this, "警告", "只有主播才能关闭直播");
        return;
    }

    emit roomClosedbyOwner(m_roomid);

    Pack pack;
    pack.set_type(TYPE_ENDROOM);

    QJsonObject data;
    data["endroomid"] = m_roomid;

    pack.set_data(data);

    m_ptcpsocket->write(pack.serialize());
    this->close();

    return;
}

//用户点击发送按钮
void LivingRoom::sendmsg_clicked()
{
    QString msg = ui->textEdit_input->toPlainText();
    if(msg.isEmpty()) return;

    Pack pack;
    pack.set_type(TYPE_CHATMSG);

    QJsonObject data;
    data["roomid"] = m_roomid;
    data["name"] = m_name;
    data["msg"] = msg;

    pack.set_data(data);

    m_ptcpsocket->write(pack.serialize());

    ui->textEdit_input->clear();

    return;
}
