#include "livesystemdb.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

Livesystemdb::Livesystemdb():
    m_db(QSqlDatabase::addDatabase("QODBC")),
    m_error()
{

}

//数据库初始化
bool Livesystemdb::init_db()
{
    m_db.setDatabaseName("livesystemdb");
    if(!m_db.open()){
        m_error = m_db.lastError().text();
        qDebug() << "数据库连接失败" <<m_error;
        return false;
    }

    m_error.clear();
    qDebug() << "数据库连接成功";
    return  true;
}

//报错
const QString &Livesystemdb::last_error()
{
    return m_error;
}

//处理用户登录逻辑
//0.登录成功；1.用户名不合符规范[3,12]；2.密码不符合规范[3,12]；3.用户名错误；4.密码错误；5.重复登录；6.数据库错误
//重复登录：新建一个字段，默认值都是false，一旦登陆过，把值改成true
int Livesystemdb::user_login(const QString &name, const QString &pwd)
{
    if(name.length() < 3 || name.length() > 12){
        return 1;
    }

    if(pwd.length() < 3 || pwd.length() > 12){
        return 2;
    }

    QString sql = QString("select pwd from livess where name = '%1';").arg(name);
    QSqlQuery query;//QSqlQuery用于执行SQL语句、操作数据库查询结果的核心类
    if(query.exec(sql)){ //exec将传入的SQL语句发送给已连接的数据库执行，并返回执行结果的成功/失败状态。true表示SQL执行成功，false表示失败
        if(query.next()){ //next用于遍历数据库查询结果集,把结果集的 “读取指针” 移动到下一条记录的位置，并返回ture移动成功。当指针移动到最后一条记录的后面时，next返回false表示遍历结束
            if(query.value("pwd").toString() == pwd){
                return 0;
            }else{
                return 4;
            }
        }else{
            return 3;
        }
    }
    m_error = query.lastError().text();
    qDebug() << "数据库查询失败" <<m_error;
    return 6;
}

//处理用户注册逻辑
//0.注册成功；1.用户名不合符规范[3,12]；2.密码不符合规范[3,12]；3.重复注册；4.数据库错误
int Livesystemdb::user_res(const QString &name, const QString &pwd)
{
    if(name.length() < 3 || name.length() > 12){
        return 1;
    }

    if(pwd.length() < 3 || pwd.length() > 12){
        return 2;
    }

    m_error.clear();

    QString sql = QString("select name from livess where name = '%1';").arg(name);
    QSqlQuery query;
    if(query.exec(sql)){
        if(query.next()){
            return 3;
        }
    }else{
        m_error = query.lastError().text();
        return 4;
    }

    sql = QString("insert into livess(name, pwd)values('%1', '%2');").arg(name).arg(pwd);
    if(query.exec(sql)){
        qDebug() << "已将用户注册信息保存到数据库";
        return 0;
    }else{
        m_error = query.lastError().text();
        qDebug() << "数据库保存失败" <<m_error;
        return 4;
    }
}
