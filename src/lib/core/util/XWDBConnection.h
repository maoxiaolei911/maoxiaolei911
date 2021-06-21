/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDBCONNECTION_H
#define XWDBCONNECTION_H

#include <QObject>
#include <QString>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWDBConnection : public QObject
{
    Q_OBJECT
    
public:
    XWDBConnection(QObject * parent = 0);
    ~XWDBConnection();
    
    bool commit();
    
    QString database();
    QString driver();
    
    static QStringList drivers();
    
    QString host();
    
    QSqlError lastError();
    
    QString name();
    
    bool open();
    
    QString password();
    int     port();
    
    QSqlQuery exec(const QString & query, bool * ok = 0);
    
    void setDatabase(const QString & db);
    void setDriver(const QString & d);
    void setHost(const QString & h);
    void setPassword(const QString & p);
    void setPort(const int p);
    void setUser(const QString & u);
    
    bool transaction();
    
    QString user();
    
private:
    static int m_connectCount;
    
    QString m_driver;
    QString m_database;
    QString m_user;
    QString m_password;
    QString m_host;
    int     m_port;
    
    QString m_name;
    
    QString m_query;
    
    QSqlError m_error;
    
    QSqlDatabase m_db;
};

inline QString XWDBConnection::database()
{
    return m_database;
}

inline QString XWDBConnection::host()
{
    return m_host;
}

inline QSqlError XWDBConnection::lastError()
{
	return m_error;
}

inline QString XWDBConnection::name()
{
    return m_name;
}

inline QString XWDBConnection::password()
{
    return m_password;
}

inline int XWDBConnection::port()
{
    return m_port;
}

inline void XWDBConnection::setDatabase(const QString & db)
{
    m_database = db;
}

inline void XWDBConnection::setHost(const QString & h)
{
    m_host = h;
}

inline void XWDBConnection::setPassword(const QString & p)
{
    m_password = p;
}

inline void XWDBConnection::setPort(const int p)
{
    m_port = p;
}

inline void XWDBConnection::setUser(const QString & u)
{
    m_user = u;
}

inline QString XWDBConnection::user()
{
    return m_user;
}

#endif // XWDBCONNECTION_H

