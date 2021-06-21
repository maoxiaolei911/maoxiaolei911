/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWCNF_H
#define XWCNF_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWCnf : public QObject
{
    Q_OBJECT
    
public:
	XWCnf(QObject * parent = 0);
    XWCnf(const QString & nameA, 
          QObject * parent = 0);
    ~XWCnf() {}
    
    void add(const QString & key, 
             const QString & value, 
             const QString & prog = QString());
    
    QString get(const QString & name, const QString & prog = QString());
    
    bool readCnf(const QString & topdir);
    void remove(const QString & key, const QString & prog = QString());
    
    bool save(const QString & topdir);
    
private:
    void doLine(const QString & line);
    
private:
	QString cnfName;
    QHash<QString, QString> hash;
};

#endif // XWCNF_H
