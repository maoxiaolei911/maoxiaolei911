/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWVARIABLE_H
#define XWVARIABLE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QChar>

#include "XWGlobal.h"

class XWCnf;

class XW_UTIL_EXPORT XWVariable : public QObject
{
    Q_OBJECT
    
public:    
    XWVariable(XWCnf * cnf = 0, QObject * parent = 0);
    ~XWVariable();

    QString varExpand(const QString & src);
    QString varValue(const QString & var);
        
private:
    void expand(QString & expansion, const QString & var);
    void expanding(const QString & var, bool xp);
    bool expanding(const QString & var);
    
private:
    struct Expansion
    {
        QString m_var;
        bool m_expanding;
    };
    
    XWCnf * m_cnf;
    
    QList<Expansion *> m_expansions;
};

#endif // XWVARIABLE_H
