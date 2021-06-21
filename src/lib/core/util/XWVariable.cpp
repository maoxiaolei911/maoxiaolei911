/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QCoreApplication>

#include "XWUtil.h"
#include "XWStringUtil.h"
#include "XWCnf.h"
#include "XWVariable.h"

XWVariable::XWVariable(XWCnf * cnf, QObject * parent)
    :QObject(parent),
     m_cnf(cnf)
{
}

XWVariable::~XWVariable()
{
	while (!m_expansions.isEmpty())
		delete m_expansions.takeFirst();
}

QString XWVariable::varExpand(const QString & src)
{
    QString expansion;
    
    int len = src.length();
    for (int i = 0; i < len; i++)
    {
        if (isVarStart(src[i]))
        {
            i++;
            if (isVarChar(src[i]))
            {
            	int var_end = i;
                do
                {
                	var_end++;
                } while ((var_end < len) && isVarChar(src[var_end]));
                
                QString env = src.mid(i, var_end - i);
                expand(expansion, env);
                var_end--;
                i = var_end;
            }
            else if (isVarBeginDelimiter(src[i]))
            {
                int var_end = ++i;
                while ((var_end < len) && !(isVarEndDelimiter(src[var_end])))
                    var_end++;
                    
                QString env = src.mid(i, var_end - i);
                expand(expansion, env);
                i = var_end;
            }
        }
        else
            expansion += src[i];
    }
    
    return expansion;
}

QString XWVariable::varValue(const QString & var)
{
	QString programname = qApp->applicationName();
    QString vtry = var + "." + programname;
    QString ret = getEnv(vtry);
    if (ret.isEmpty())
    {
        vtry = var + "_" + programname;
        ret = getEnv(vtry);
    }
    
    if (ret.isEmpty())
        ret = getEnv(var);
            
    if (ret.isEmpty() && m_cnf)
        ret = m_cnf->get(var);
            
    if (!ret.isEmpty())
        ret = varExpand(ret);
        
    return ret;
}

void XWVariable::expand(QString & expansion, const QString & var)
{
    if (expanding(var))
        return;
    
    QString programname = qApp->applicationName();
    QString vtry = var + "_" + programname;
    QString value = getEnv(vtry);
        
    if (value.isEmpty())
        value = getEnv(var);
            
    if (value.isEmpty() && m_cnf)
        value = m_cnf->get(var);
            
    if (!value.isEmpty())
    {
        expanding(var, true);
        value = varExpand(value);
        expanding(var, false);
        expansion += value;
    }
}

void XWVariable::expanding(const QString & var, bool xp)
{
    int s = m_expansions.size();
    if (s > 0)
    {
        for (int i = 0; i < s; i++)
        {
            if (m_expansions.at(i)->m_var == var)
            {
                m_expansions.at(i)->m_expanding = xp;
                return;
            }
        }
    }
    
    Expansion * v = new Expansion;
    if (v)
    {
        v->m_var = var;
        v->m_expanding = xp;
        m_expansions.append(v);
    }
}

bool XWVariable::expanding(const QString & var)
{
    int s = m_expansions.size();
    if (s > 0)
    {
        for (int i = 0; i < s; i++)
            if (m_expansions.at(i)->m_var == var)
                return m_expansions.at(i)->m_expanding;
    }
    
    return false;
}
