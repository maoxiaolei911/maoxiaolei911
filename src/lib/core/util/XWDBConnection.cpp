/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWDBConnection.h"

int XWDBConnection::m_connectCount = 0;

XWDBConnection::XWDBConnection(QObject * parent)
    :QObject(parent)
{
    m_name = QString("DBCON%1").arg(m_connectCount);
    m_connectCount++;
}

XWDBConnection::~XWDBConnection()
{
    if (m_db.isValid() && m_db.isOpen())
        m_db.close();
    
    QSqlDatabase::removeDatabase(m_name);
}

bool XWDBConnection::commit()
{
    bool ret = m_db.commit();
    if (!ret)
        m_error = m_db.lastError();
        
    return ret;
}

QStringList XWDBConnection::drivers()
{
    QStringList tmp = QSqlDatabase::drivers();
    QStringList ret;
    for (int i = 0; i < tmp.size(); i++)
    {
        QString t = tmp.at(i);
        if (t.isEmpty())
            continue;
            
        if (t[0] == QChar('Q'))
            t.remove(0, 1);
            
        ret << t;
    }
    
    return ret;
}

QString XWDBConnection::driver()
{
    QString ret = m_driver;
    if (ret[0] == QChar('Q'))
        ret.remove(0, 1);
        
    return ret;
}

QSqlQuery XWDBConnection::exec(const QString & query, bool * ok)
{
    m_query = query;
    QSqlQuery q(m_db);
    bool r = q.exec(query);
    if (!r)
        m_error = m_db.lastError();
        
    if (ok)
        *ok = r;
    
    return q;
}

bool XWDBConnection::open()
{
    m_db = QSqlDatabase::addDatabase(m_driver, m_name);
    if (!m_host.isEmpty())
        m_db.setHostName(m_host);
        
    m_db.setDatabaseName(m_database);
    
    if (!m_user.isEmpty())
        m_db.setUserName(m_user);
    
    if (!m_password.isEmpty())
        m_db.setPassword(m_password);
        
    if ((m_port > -1) && (m_port < 65536))
        m_db.setPort(m_port);
        
    bool ret = true;
    if (!m_db.open())
        ret = false;
        
    m_error = m_db.lastError();
        
    return ret;
}

void XWDBConnection::setDriver(const QString & d)
{
    if ((d == "DB2") || (d == "IBASE") || (d == "MYSQL") || 
        (d == "OCI") || (d == "ODBC") || (d == "PSQL") || 
        (d == "SQLITE") || (d == "SQLITE2") || (d == "TDS"))
    {
        m_driver = QString("Q") + d;
    }
    else
        m_driver = d;
}

bool XWDBConnection::transaction()
{
    bool ret = m_db.transaction();
    if (!ret)
        m_error = m_db.lastError();
        
    return ret;
}

