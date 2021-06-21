/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QRegExp>
#include <QUrl>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>

#include "XWStringUtil.h"
#include "XWFileName.h"

XWFileName::XWFileName(const char * _path, QObject * parent)
    :QObject(parent), path(_path)
{
    if (isLocal())
        normalize();
}

XWFileName::XWFileName(const QString & _path, QObject * parent)
    :QObject(parent), path(_path)
{
    if (isLocal())
        normalize();
}

XWFileName::XWFileName(const QChar * _path, QObject * parent)
	:QObject(parent)
{
	path = QString(_path);
	if (isLocal())
        normalize();
}

QString XWFileName::baseName()
{
    int len = path.length();
    if (len <= 0)
        return QString();
        
    int sd = path.lastIndexOf(QChar('/'));
    QString tmp;
    if (sd == -1)
        tmp = path;
    else
        tmp = path.mid(sd + 1);
        
    int ed = tmp.lastIndexOf(QChar('.'));
    if (ed == -1)
        return tmp;
        
    return tmp.left(ed);
}

QString XWFileName::dirPart()
{
    QString tmp = path;
    if (!isLocal())
    {
        QUrl url(path);
        tmp = url.path();
    }
    
    int len = tmp.length();
    if (len <= 0)
        return QString();
        
    int sd = tmp.lastIndexOf(QChar('/'));        
    if (sd == -1)
        return QString();
        
    return tmp.left(sd);
}

bool XWFileName::execute(const QString & param)
{
    if (!isExecutable())
        return false;
        
    QStringList arguments = param.split(' ', QString::SkipEmptyParts);
    QProcess process;
    process.start(path, arguments);
    if (!process.waitForStarted())
    	return false;
    	
    return true;
}

QString XWFileName::ext(bool hasdot)
{
    QString tmp = path;
    if (!isLocal())
    {
        QUrl url(path);
        tmp = url.path();
    }
    
    int len = tmp.length();
    if (len <= 0)
        return QString();
        
    int dotpos = tmp.lastIndexOf(QChar('.'));
    if (dotpos == -1)
        return QString();
        
    int slashpos = tmp.lastIndexOf(QChar('/'));
    if (slashpos > dotpos)
        return QString();
        
    if (hasdot)
    	return tmp.mid(dotpos);
    	
    return tmp.mid(dotpos + 1);
}

QString XWFileName::extendFilename(const QString & defaultSuffix)
{
	QString ex = ext();
    if (!ex.isEmpty())
        return path;
        
    if (defaultSuffix.isEmpty())
    	return path;
    	
    if (defaultSuffix[0] == QChar('.'))
    	return QString("%1%2").arg(path).arg(defaultSuffix);
        
    return QString("%1.%2").arg(path).arg(defaultSuffix);
}

QString XWFileName::fileName()
{
    QString tmp = path;
    if (!isLocal())
    {
        QUrl url(path);
        tmp = url.path();
    }
    
    int len = tmp.length();
    if (len <= 0)
        return QString();
        
    int sd = tmp.lastIndexOf(QChar('/'));
    if (sd == -1)
        return tmp;
        
    return tmp.mid(sd + 1);
}

QString XWFileName::getPath()
{
    if (isLocal())
        return path;
        
    QUrl url(path);
    
    return url.path();
}

QString XWFileName::host()
{
    if (isLocal())
        return QString();
        
    QUrl url(path);
    
    return url.host();
}

bool XWFileName::ignoreDir()
{
    return (path.indexOf(QRegExp("/+\\.[^/]"), 1) != -1);
}

bool XWFileName::isAbsolute()
{
    if (!isLocal())
        return false;
        
    QFileInfo info(path);
    
    return info.isAbsolute();
}

bool XWFileName::isAbsolute(bool relative)
{
    int len = path.length();
	bool absolute = isDirSep(path[0]) || ((len > 1) && isDevSep(path[1]));
	bool explicit_relative = relative && 
	                         ((len > 1) && path[0] == QChar('.') && (isDirSep(path[1])
                              || ((len > 2) && path[1] == QChar('.') && 
                              isDirSep(path[2]))));
                              
	return absolute || explicit_relative;
}

bool XWFileName::isExecutable()
{
    if (!isLocal())
        return false;
        
    QFileInfo info(path);
    return info.isExecutable();
}

bool XWFileName::isNameBeginWithDev()
{
    if (!isLocal())
        return false;
        
    return ((path.length() > 1) && isDevSep(path[1]));
}

bool XWFileName::isLocal()
{
    if (path.length() == 0)
        return false;
        
    if (path.startsWith("file:", Qt::CaseInsensitive))
        return true;
                
    if (path.contains(QRegExp("[a-z|A-Z]{2,}://")))
        return false;
        
    if (path.contains(QRegExp("\\d+\\.\\d+\\.\\d+\\.\\d+")))
        return false;
        
    if (path.contains(QRegExp("\\w+\\.\\w+\\.\\w+\\.\\w+\\.\\w+\\.\\w+\\.\\w+\\.\\w+")))
        return false;
        
    return true;
}

bool XWFileName::isReadable()
{
    if (!isLocal())
        return false;
        
    QFileInfo info(path);
    return (info.isFile() && info.isReadable());
}

bool XWFileName::isUNCName()
{
    return ((path.length() > 2) && 
            (isDirSep(path[0]) && 
            isDirSep(path[1]) && 
            (path[2].isLetterOrNumber())));
}

bool XWFileName::isValid()
{
    if (isLocal())
        return true;
        
    QUrl url(path);
    return url.isValid();
}

QString XWFileName::noExt()
{
    QString ex = ext();
    if (ex.isEmpty())
    	return path;
    	
    int idx = path.indexOf(ex);
    return path.left(idx);
}

bool XWFileName::open()
{
	QUrl url(path);
	
	return QDesktopServices::openUrl(url);
}

QString XWFileName::password()
{
    if (isLocal())
        return QString();
        
    QUrl url(path);
    
    return url.password();
}

int XWFileName::port()
{
    if (isLocal())
        return -1;
        
    QUrl url(path);
    
    return url.port();
}

bool XWFileName::sameFile(const QString & fn)
{
    QFileInfo info1(path);
    QFileInfo info2(fn);
    
    return (info1.absoluteFilePath() == info2.absoluteFilePath());
}

QString XWFileName::scheme()
{
    if (isLocal())
        return QString();
        
    QUrl url(path);
    
    return url.scheme();
}

void XWFileName::setPath(const QString & _path)
{
    path = _path;
    normalize();
}

bool XWFileName::trySuffix(const QString & suffix)
{
    return ((path.length() >= suffix.length()) && (path.endsWith(suffix, Qt::CaseInsensitive)));
}

QString XWFileName::userName()
{
    if (isLocal())
        return QString();
        
    QUrl url(path);
    
    return url.userName();
}

void XWFileName::normalize()
{
    int i = 0;
    if (isNameBeginWithDev())
    {
        QChar lc = path[0].toLower();
        path[0] = lc;
        
        while (isDirSep(path[3]))
            path.remove(3, 1);
        
        i = 2;
    }
    else if (isUNCName())
    {
        int p = 2;
        while ((p < path.length()) && (!isDirSep(path[p])))
            p++;
            
        i = p;
        p++;
        while (isDirSep(path[p]))
            path.remove(p, 1);
    }
    else if (path.startsWith("file://", Qt::CaseInsensitive))
    {
        path.remove(0, 7);
        int d = path.indexOf(QRegExp("[a-z|A-Z]:"));
        if (d != -1)
        {
            if (d > 0)
                path.remove(0, d);
            QChar lc = path[0].toLower();
            path[0] = lc; 
        }
    }
    
    for (i = 0; i < path.length(); i++)
    {
    	if (path[i] == QChar('\\'))
    		path[i] = QChar('/');
    }
}

