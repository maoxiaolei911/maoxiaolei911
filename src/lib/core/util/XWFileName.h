/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFILENAME_H
#define XWFILENAME_H

#include <QObject>
#include <QString>

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWFileName : public QObject
{
    Q_OBJECT
    
public:
	//用指定的路径构造标准文件路径，多余的路径分隔符将被去除
    XWFileName(const char * _path, QObject * parent = 0);
    XWFileName(const QString & _path, QObject * parent = 0);
    XWFileName(const QChar * _path, QObject * parent = 0);
    ~XWFileName() {}
    
    //文件基本名，如c:/latex/dvipdfmx.pdf的基本名为dvipdfmx
    QString baseName();
    
    //文件名的路径部分，不含文件名，如c:/latex/dvipdfmx的路径部分为c:/latex，
    //http://www.xiuwen/product/index的路径部分为/product
    QString dirPart();
    
    //如果文件可执行，用参数param运行该程序，多个参数用空格分隔
    bool execute(const QString & param);
    
    //文件扩展名，hasdot指明在返回结果中是否包含'.'
    QString ext(bool hasdot = false);
    //给文件加上扩展名，如果已经有扩展名，则不加
    QString extendFilename(const QString & defaultSuffix);
    
    //最后一个路径分隔符后的部分
    QString fileName();
    QString fullName() {return path;}
    
    //文件的路径部分，含文件名
    QString getPath();
    
    //文件名的主机名
    QString host();
        
    bool ignoreDir();
    bool isAbsolute();
    bool isAbsolute(bool relative);
    bool isExecutable();
    //路径是否以设备符开始
    bool isNameBeginWithDev();
    //是否为本地文件(以设备符、'\\'、file://开始的文件路径被认为是本地的)
    bool isLocal();
    bool isReadable();
    //是否为两个连续的路径分隔符开始
    bool isUNCName();
    bool isNull() {return path.isEmpty();}
    bool isValid();
    
    //去除文件扩展名，保留'.'(如果有的话)
    QString noExt();
    
    //用系统中关联程序打开文件
    bool open();
    
    QString password();
    int     port();
        
    //与fn是否为同一文件
    bool    sameFile(const QString & fn);
    QString scheme();
    void    setPath(const QString & _path);
    
    //文件扩展名是否为suffix
    bool trySuffix(const QString & suffix);
    
    QString userName();
    
private:
    void normalize();
    
private:
    QString path;
};


#endif // XWFILENAME_H
