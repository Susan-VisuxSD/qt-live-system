#include "pack.h"

Pack::Pack(Type type):
    m_type(type)
{

}

//内容
void Pack::set_data(const QJsonObject &data)
{
    m_data = data;
    return;
}

//设置包类型，是聊天还是登录还是注册
void Pack::set_type(Type type)
{
    m_type = type;
}

//序列化
QByteArray Pack::serialize()
{
    QJsonObject obj;

    obj["type"] = m_type;//表示给这个JSON对象添加/修改一个键（key）为type的项,并把m_type的值赋给他
    obj["data"] = m_data;

    QJsonDocument doc(obj);//创建一个QJsonDocument对象doc，并把已构建好的QJsonObject对象obj作为内容填充进去。
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    //把doc中的JSON内容（比如 {"type":"login","username":"test"}）转换成紧凑格式的字节数组，赋值给jsonData，用于网络传输。

    //解决粘包：加长度头
    QByteArray packet;
    qint32 len = jsonData.size();
    packet.append(reinterpret_cast<char*>(&len), sizeof(qint32));
    //把存储业务数据长度的4字节整数len，以原始字节形式追加到数据包packet开头，作为包头（长度标识）。
    packet.append(jsonData);//把JSON业务数据（包体）拼接到长度包头后面，形成包头(4字节)+包体(JSON数据)的完整数据包。

    return packet;
}

//反序列化
QJsonObject Pack::deserialize(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(!doc.isObject()){
        return QJsonObject();
    }

    return doc.object();
}

void Pack::clear()
{
    m_data = QJsonObject();
    m_type = TYPE_NONE;

    return;
}

