#ifndef MYPCSOCKET_H
#define MYPCSOCKET_H

#include <QTcpSocket>
//用来单独储存用户信息

class Mypcsocket
{
public:
    Mypcsocket(QTcpSocket* pclient);

public:
    QTcpSocket* get_pclient();
    const QString& get_name();
    void set_name(const QString& name);
    void set_createroomid(const QString& createroomid);
    const QString& get_createroomid();
    void set_joinroomid(const QString& joinroomid);
    const QString& get_joinroomid();

public:
    void close();
    QByteArray readAll();
    qint64 write(const QByteArray& data);

public:
    QByteArray m_buffer;

private:
    QTcpSocket* m_pclient;
    QString m_name;
    QString m_createroomid;
    QString m_joinroomid;
};

#endif // MYPCSOCKET_H
