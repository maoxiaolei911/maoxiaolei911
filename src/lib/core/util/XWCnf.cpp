/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QFile>
#include <QTextStream>

#include "XWUtil.h"
#include "XWStringUtil.h"

#include "XWCnf.h"

XWCnf::XWCnf(QObject * parent)
	:QObject(parent)
{
}

XWCnf::XWCnf(const QString & nameA, 
             QObject * parent)
    :QObject(parent),
     cnfName(nameA)
{
}

void XWCnf::add(const QString & key, 
                const QString & value, 
                const QString & prog)
{
	if (key.isEmpty() || value.isEmpty())
		return ;
		
	QString var = key;
	if (!prog.isEmpty())
	{
		var += ".";
		var += prog;
	}
	
	hash.insert(var, value);
}

QString XWCnf::get(const QString & name, const QString & prog)
{
    QString var = name;
    if (!prog.isEmpty())
        var = var + "." + prog;
        
    return hash.value(var, QString());
}
    
bool XWCnf::readCnf(const QString & topdir)
{
	if (topdir.isEmpty() || cnfName.isEmpty())
        return false;
        
    QString name = QString("%1/%2").arg(topdir).arg(cnfName);
    QFile file(name);
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
           
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        int len = line.length();
        if ((len == 0) || (line[0] == QChar('%')) || (line[0] == QChar('#')))
            continue;
                
        if (line[len - 1] == QChar('\\'))
        {
            line[len - 1] = QChar(' ');
            QString tmp = in.readLine();
            tmp = tmp.trimmed();
            while (!tmp.isEmpty())
            {
                line += tmp;
                if (tmp[tmp.length() - 1] != QChar('\\') || in.atEnd())
                    break;
                        
                line[line.length() - 1] = QChar(' ');
                tmp = in.readLine();
            }
                
            line = line.trimmed();
        }
            
        doLine(line);
    }
        
    file.close();
    
    return true;
}

void XWCnf::remove(const QString & key, const QString & prog)
{
	QString var = key;
    if (!prog.isEmpty())
        var = var + "." + prog;
        
	hash.remove(var);
}

bool XWCnf::save(const QString & topdir)
{
	if (topdir.isEmpty() || cnfName.isEmpty())
        return false;
		
	QString name = QString("%1/%2").arg(topdir).arg(cnfName);
    QString tmpname = QString("%1.tmp").arg(name);
    QFile file(tmpname);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
        
    QTextStream out(&file);
    QHashIterator<QString, QString> i(hash);
    while (i.hasNext())
    {
    	i.next();
    	QString tmp = i.key();
    	if (tmp.contains(QChar('.')))
    	{
    		QStringList tmplist = tmp.split(QChar('.'), QString::SkipEmptyParts);
    		if (tmplist.size() == 2)
    		{
    			out << tmplist.at(0);
    			out << ".";
    			out << tmplist.at(1);
    			out << " = ";
    			out << i.value();
    		}
    	}
    	else
    	{
    		out << tmp;
    		out << " = ";
    		out << i.value();
    	}
    	
    	out << "\n\n";
    }
    
    file.close();
    
    bool ret = true;
    if (QFile::exists(name))
    	ret = QFile::remove(name);
    		
    if (ret)
    	ret = file.rename(name);
    else
    	ret = file.remove();
    	
    return ret;
}

void XWCnf::doLine(const QString & line)
{        
    //查找变量名
    const QChar * s = line.data();
    const QChar * p = s;
    while (!(p[0].isSpace()) && (p[0] != QChar('=')) && (p[0] != QChar('.')))
        p++;
        
    QString var(s, p - s);
    
    while (p[0].isSpace())
        p++;
        
    QString prog;
    if (p[0]== QChar('.'))
    {
    	p++;
    	while (p[0].isSpace())
        	p++;
        	
        s = p;
        while (!p[0].isNull() && !(p[0].isSpace()))
        	p++;
        	
        prog = QString(s, p - s);
    }
    
    while (p[0].isSpace())
        p++;
        	
    if (p[0] != QChar('='))
    	return ;
    
    p++;
    while (p[0].isSpace())
        p++;
        
    QString value(p);
    
    if (!prog.isEmpty())
        var = var + "." + prog;
        
    hash.insert(var, value);
}

