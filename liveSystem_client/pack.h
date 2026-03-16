#ifndef PACK_H
#define PACK_H

#include <QString>
#include <QJsonObject> //JSON 键值对的集合
#include <QJsonDocument> //Qt 中专门用于封装 / 解析完整 JSON 数据的类
#include <QByteArray>

typedef enum en_type{
    TYPE_NONE = 0,
    TYPE_LOGIN,
    TYPE_REGISTER,
    TYPE_CREATEROOM,
    TYPE_JOINROOM,
    TYPE_FLUSHROOM,
    TYPE_GETNEWROOM,
    TYPE_GETNEWUSER,
    TYPE_FLUSHUSERS,
    TYPE_ENDROOM,
    TYPE_EXITROOM,
    TYPE_CHATMSG
}Type;

class Pack
{
public:
    Pack(Type type = TYPE_LOGIN);

public:
    void set_data(const QJsonObject& data);//内容
    void set_type(Type type);//设置包类型，是聊天还是登录还是注册
    QByteArray serialize();//序列化
    static QJsonObject deserialize(const QByteArray& data);//反序列化

private:
    QJsonObject m_data;
    Type m_type;

};

#endif // PACK_H
