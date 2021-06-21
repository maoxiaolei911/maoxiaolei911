/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QMutableHashIterator>
#include <QFileSystemModel>
#include <QModelIndex>
#include <QDateTime>

#include "XWStringUtil.h"
#include "XWFileName.h"

#include "XWFileDB.h"

const QString lsRmagic = "% filename database; do not change this line.";
    
XWFileDB::XWFileDB(QObject * parent)
	:QObject(parent)
{
}

XWFileDB::XWFileDB(const QString & nameA, 
	               QObject * parent)
    :QObject(parent),
     dbName(nameA)
{
}

XWFileDB::XWFileDB(const QString & nameA, 
                   const QString & aA, 
                   QObject * parent)
	:QObject(parent),
     dbName(nameA),
     aliasesName(aA)
{
}

void XWFileDB::addDir(const QString & dir)
{
	QHash<QString, int>::iterator i = dbDirList.begin();
	while (i != dbDirList.end())
	{
		QString tmp = i.key();
		if (dir == tmp || dir.startsWith(tmp))
    		return ;
    		
    	++i;
	}
	    
    dbDirList[dir] = 1;
    QString curDir = "";
    buildDir(dir, curDir);
}

QStringList XWFileDB::allTopDir()
{
	QStringList ret = dbDirList.keys();
	return ret;
}

QStringList XWFileDB::files()
{
	QStringList ret;
	QMutableHashIterator<QString, QString> i(db);
	while (i.hasNext())
	{
		i.next();
		QString tmp = QString("%1%2").arg(i.value()).arg(i.key());
		ret << tmp;
	}
	return ret;
}

QStringList XWFileDB::files(const QStringList & suffixes)
{
	QStringList ret;
	QMutableHashIterator<QString, QString> itor(db);
	while (itor.hasNext())
	{
		itor.next();
		QString name = itor.key();
		for (int i = 0; i < suffixes.size(); i++)
		{
			if (name.endsWith(suffixes.at(i), Qt::CaseInsensitive))
			{
				QString tmp = QString("%1%2").arg(itor.value()).arg(itor.key());
				ret << tmp;
			}
		}
	}
	return ret;
}
    
QStringList XWFileDB::fileNames()
{
	QStringList ret;
	QMutableHashIterator<QString, QString> i(db);
	while (i.hasNext())
	{
		i.next();
		ret << i.key();
	}
	return ret;
}

QStringList XWFileDB::fileNames(const QStringList & suffixes)
{
	QStringList ret;
	QMutableHashIterator<QString, QString> itor(db);
	while (itor.hasNext())
	{
		itor.next();
		QString name = itor.key();
		for (int i = 0; i < suffixes.size(); i++)
		{
			if (name.endsWith(suffixes.at(i), Qt::CaseInsensitive))
				ret << itor.key();
		}
	}
	return ret;
}

void XWFileDB::insert(const QString & passedfname)
{
    XWFileName fn(passedfname);
            
    QString name = fn.fileName();
    QString dir = fn.dirPart() + "/";
    db.insert(name, dir);
}
    
void XWFileDB::init(const QString & topdir)
{
    if (topdir.isEmpty() || dbName.isEmpty())
        return ;
        
    QString filename = QString("%1/%2").arg(topdir).arg(dbName);
    if (QFile::exists(filename))
    {
    	if (!checkFile(filename))
    		updateFile(filename);
    	else
    	{
    		QFileSystemModel model;
    		QModelIndex idx = model.setRootPath(topdir);
    		QDateTime dt = model.lastModified(idx);
    		QFileInfo finfo(filename);
    		QDateTime ft = finfo.lastModified();
    		if (ft < dt)
    			updateFile(filename);
    	}
    }
    else
    	updateFile(filename);
    	
    build(db, filename);
    
    if (aliasesName.isEmpty())
        return ;
        
    filename = QString("%1/%2").arg(topdir).arg(aliasesName);
        
    if (QFile::exists(filename))
    {
    	if (!checkFile(filename))
    		return ;
    }
    
    aliasBuild(aliasesdb, filename);
}
    
QStringList XWFileDB::search(const QString & name, 
                             const QString & origpathelt,
                             bool  all)
{
    QStringList ret;
    if (db.isEmpty())
    	return ret;
            
    XWFileName fn(name);
    QString dir = fn.dirPart();
    QString pathelt;
    if (dir.isEmpty())
        pathelt = origpathelt;
    else
        pathelt = origpathelt + "/" + dir;
        
    bool relevant = false;
    QHash<QString, int>::iterator itor = dbDirList.begin();
    while (!relevant && itor != dbDirList.end())
    {
    	relevant = eltInDb(itor.key(), pathelt);
    	itor++;
    }
        
    if (!relevant)
        return ret;
        
    QStringList aliases;
    QString f = fn.fileName();
    if (aliasesdb.size() > 0)
        aliases = lookup(aliasesdb, f);
    
    if (!aliases.contains(f))
        aliases.insert(0, f);
    else
    {
        int idx = aliases.indexOf(f);
        if (idx != 0)
            aliases.move(idx, 0);
    }
    
    bool done = false;
    int i = 0;
    int as = aliases.size();
    while (!done && (i < as))
    {
        QString ctry = aliases.at(i);
        QStringList origdirs = lookup(db, ctry);
        
        int j = 0;
        int ods = origdirs.size();
        while (!done && (j < ods))
        {
            QString dbfile = origdirs.at(j) + ctry;
            bool matched = match(dbfile.constData(), pathelt.constData());
            if (matched)
            {
                QString found;
                XWFileName dbf(dbfile);
                if (dbf.isReadable())
                    found = dbfile;
                else
                {
                    int k = 1;
                    while ((k < as) && found.isEmpty())
                    {
                        QString atry = origdirs.at(j) + aliases.at(k);
                        XWFileName afn(atry);
                        if (afn.isReadable())
                            found = atry;
                            
                        k++;
                    }
                }
                
                if (!found.isEmpty())
                    ret << found;
                    
                if (!all && !found.isEmpty())
                    done = true;
            }
            j++;
        }
        
        i++;
    }
    
    return ret;
}
    
QStringList XWFileDB::searchList(const QStringList & names, 
                                 const QString & pathelt,
                                 bool  all)
{
    QStringList ret;
        
    bool relevant = false;
    QHash<QString, int>::iterator itor = dbDirList.begin();
    while (!relevant && itor != dbDirList.end())
    {
    	relevant = eltInDb(itor.key(), pathelt);
    	itor++;
    }
    
    if (!relevant)
        return ret;
        
    bool done = false;
    
    int i = 0;
    int ns = names.size();
    while (!done && (i < ns))
    {
        if (isAbsolute(names.at(i), true))
            continue;
            
        XWFileName fn(names.at(i));
        QString name = fn.fileName();
        QString dir = fn.dirPart();
        QString path;
        if (dir.isEmpty())
            path = pathelt;
        else
            path = pathelt + "/" + dir;
            
        QStringList aliases;
        if (aliasesdb.size() > 0)
            aliases = lookup(aliasesdb, name);
        
        if (!aliases.contains(name))
            aliases.insert(0, name);
        else
        {
            int idx = aliases.indexOf(name);
            if (idx != 0)
                aliases.move(idx, 0);
        }
            
        int j = 0;
        int as = aliases.size();
        while (!done && (j < as))
        {
            QString ctry = aliases.at(j);
            QStringList origdirs = lookup(db, ctry);
            int k = 0;
            int ods = origdirs.size();
            while (!done && (k < ods))
            {
                QString dbfile = origdirs.at(k) + ctry;
                bool matched = match(dbfile.constData(), path.constData());
                if (matched)
                {
                    QString found;
                    XWFileName dbf(dbfile);
                    if (dbf.isReadable())
                        found = dbfile;
                    else
                    {
                        int n = 1;
                        while ((n < as) && found.isEmpty())
                        {
                            QString atry = origdirs.at(k) + aliases.at(n);
                            XWFileName afn(atry);
                            if (afn.isReadable())
                                found = atry;
                                
                             n++;
                        }
                    }
                
                    if (!found.isEmpty())
                        ret << found;
                    
                    if (!all && !found.isEmpty())
                        done = true;
                }
                k++;
            }
            
            j++;
        }
            
        i++;
    }
    
    return ret;
}

void XWFileDB::update(const QString & topdir)
{
	QString filename = QString("%1/%2").arg(topdir).arg(dbName);
	updateFile(filename);
}

void XWFileDB::aliasBuild(QMultiHash<QString, QString> & table, 
	                      const QString & filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return ;
        
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        line = line.simplified();
        int len = line.length();
        if ((len > 0) && (line[0] != QChar('%')) && (line[0] != QChar('#')))
        {
            QStringList tmp = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if (tmp.size() == 2)
                table.insert(tmp.at(1), tmp.at(0));
        }
    }
    
    file.close();
}
    
void XWFileDB::build(QMultiHash<QString, QString> & table, 
	                 const QString & filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return ;
        
    int fileCount = 0;
    QString curDir;
    XWFileName fn(filename);
    QString topDir = fn.dirPart();
    topDir += "/";
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        int len = line.length();
        if ((len > 0) && (line[len - 1] == QChar(':')))
        {
        	if (isAbsolute(line, true))
        	{
            	if (line.indexOf(QRegExp("/+\\.[^/]"), 1) == -1)
            	{
                	line[len - 1] = QChar('/');
                	if (line == ".//")
                		curDir = topDir;
                	else if (line[0] == QChar('.'))
                        curDir = topDir + line.right(len - 2);
                	else
                    	curDir = line;
            	}
            	else
                	curDir.clear();
            }
        }
        else if ((!line.isEmpty()) && (!curDir.isEmpty()) && 
                  (line != ".") && (line != ".."))
        {
        	table.insert(line, curDir);
            fileCount++;
        }
    }
    
    file.close();
    
    if (fileCount > 0)
        dbDirList[topDir] = 1;
}

void XWFileDB::buildDir(const QString & parentDir, 
                        const QString & curDir)
{
	QString path = parentDir;
    if (curDir.length() > 0)
        path = path + curDir;
    QDir dir(path);
    
    QStringList files = dir.entryList(QDir::Files);
    for (int i = 0; i < files.size(); i++)
    {
        QString name = files.at(i);
        QString p = QString("%1/").arg(path);
        db.insert(name, p);
    }
    
    QStringList dirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
//    for (int i = 0; i < dirs.size(); i++)
//    {
//        QString name = dirs.at(i);
//        stream << name << "\n";
//    }
    
    for (int i = 0; i < dirs.size(); i++)
    {
        QString cur = curDir + "/" + dirs.at(i);
        buildDir(parentDir, cur);
    }
}

bool XWFileDB::checkFile(const QString & path)
{
    QFile f(path);        
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    
    QString magic  = f.readLine();
    if (!magic.isEmpty())
    {
    	magic = magic.trimmed();
        if (magic != lsRmagic)
        {
            f.close();
            return false;
        }
    }
    
    f.close();
    return true;
}

bool XWFileDB::eltInDb(const QString & dbdir, const QString & pathelt)
{
    return pathelt.startsWith(dbdir, Qt::CaseInsensitive);
}

bool XWFileDB::isAbsolute(const QString & filename, bool relative_ok)
{
	int len = filename.length();
	bool absolute = isDirSep(filename[0]) || ((len > 1) && isDevSep(filename[1]));
	bool explicit_relative = relative_ok && 
	                         ((len > 1) && filename[0] == QChar('.') && (isDirSep(filename[1])
                              || ((len > 2) && filename[1] == QChar('.') && 
                              isDirSep(filename[2]))));
                              
	return absolute || explicit_relative;
}

QStringList XWFileDB::lookup(QMultiHash<QString, QString> & table, 
                             const QString & key)
{
	QMultiHash<QString, QString>::iterator i = table.find(key);
	QStringList ret;
	while (i != table.end() && i.key() == key)
	{
		ret << i.value();
		i++;
	}
		
	return ret;
}

//filename的路径部分是否与pathelt匹配
bool XWFileDB::match(const QChar * filename, const QChar * pathelt)
{
    bool matched = false;
    const QChar * originalFileName = filename;
    
    for (; !atStringEnd(filename) && !atStringEnd(pathelt); filename++, pathelt++)
    {
        //比较不分大小写
        if (filename[0].toUpper() == pathelt[0].toUpper())
            ;
        else if (isDirSep(pathelt[0]) && //两个连续的路径分隔符
                 (originalFileName < filename) && 
                 isDirSep(pathelt[-1]))
        {
            while (isDirSep(pathelt[0]))
                pathelt++;
                
            if (atStringEnd(pathelt))
            {
                //pathelt正好是filename部分路径
                matched = true;
                break;
            }
            else
            {
                for (; !matched && !atStringEnd(filename); filename++)
                {
                    if (isDirSep(filename[-1]) && 
                        (filename[0].toUpper() == pathelt[0].toUpper()))
                    {
                        matched = match(filename, pathelt);
                    }
                }
                
                break;
            }
        }
        else
            break;
    }
    
    if (!matched && atStringEnd(pathelt))
    {
        if (isDirSep(filename[0]))
            filename++;
            
        while (!atStringEnd(filename) && !isDirSep(filename[0]))
            filename++;
            
        matched = atStringEnd(filename);
    }
    
    return matched;
}

void XWFileDB::travers(QTextStream & stream, 
                       const QString & parentDir, 
                       const QString & curDir, 
                       const QString & dbtmpfile)
{
    QString path = parentDir;
    if (curDir.length() > 0)
        path = path + curDir;
    QDir dir(path);
    
    QStringList files = dir.entryList(QDir::Files);
    for (int i = 0; i < files.size(); i++)
    {
        QString name = files.at(i);
        if (name != dbtmpfile)
            stream << name << "\n";
    }
    
    QStringList dirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
//    for (int i = 0; i < dirs.size(); i++)
//    {
//        QString name = dirs.at(i);
//        stream << name << "\n";
//    }
    
    for (int i = 0; i < dirs.size(); i++)
    {
        QString cur = curDir + "/" + dirs.at(i);
        QString d = "\n." + cur + ":\n";
        stream << d;
        travers(stream, parentDir, cur, dbtmpfile);
    }
}

void XWFileDB::updateFile(const QString & file)
{
    XWFileName fn(file);
    QString dbfile = fn.fileName();
    QString parentDir = fn.dirPart();
    
    QString filetmp = parentDir + "/" + dbfile + ".tmp";
    QFile dbf(filetmp);
    if (!dbf.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    
    QTextStream out(&dbf);
    out << lsRmagic << "\n" << "./:\n";
    
    QString curDir = "";
    travers(out, parentDir, curDir, dbfile + ".tmp");
    dbf.close();
    if (QFile::exists(file) && !QFile::remove(file))
    {
        QFile::remove(filetmp);
        return;
    }
    
    dbf.rename(file);
}
