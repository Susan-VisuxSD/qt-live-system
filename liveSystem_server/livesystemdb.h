#ifndef LIVESYSTEMDB_H
#define LIVESYSTEMDB_H

#include <QSqlDatabase>


class Livesystemdb
{
private:
    Livesystemdb();
    Livesystemdb(const Livesystemdb&);

public:
    bool init_db();//数据库初始化
    const QString& last_error();//报错

public:
    static Livesystemdb& get_instance(){
        static Livesystemdb instance;
        return instance;
    }

public:
    int user_login(const QString& name, const QString& pwd);//处理用户登录逻辑
    int user_res(const QString& name, const QString& pwd);//处理用户注册逻辑

private:
    QSqlDatabase m_db;
    QString m_error;
};

#endif // LIVESYSTEMDB_H
