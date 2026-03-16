#include "mypcsocket.h"

Mypcsocket::Mypcsocket(QTcpSocket* pclient):
    m_pclient(pclient),
    m_buffer(),
    m_createroomid()
{

}

QTcpSocket *Mypcsocket::get_pclient()
{
    return m_pclient;
}

const QString &Mypcsocket::get_name()
{
    return m_name;
}

void Mypcsocket::set_name(const QString &name)
{
    m_name = name;
    return;
}

void Mypcsocket::set_createroomid(const QString &createroomid)
{
    m_createroomid = createroomid;
    return;
}

const QString &Mypcsocket::get_createroomid()
{
    return m_createroomid;
}

void Mypcsocket::set_joinroomid(const QString &joinroomid)
{
    m_joinroomid = joinroomid;
    return;
}

const QString &Mypcsocket::get_joinroomid()
{
    return m_joinroomid;
}

void Mypcsocket::close()
{
    return m_pclient->close();
}

QByteArray Mypcsocket::readAll()
{
    return m_pclient->readAll();
}

qint64 Mypcsocket::write(const QByteArray &data)
{
    return m_pclient->write(data);
}
