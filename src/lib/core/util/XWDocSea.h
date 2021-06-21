/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDOCSEA_H
#define XWDOCSEA_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>

#include "XWGlobal.h"
#include "XWPathSea.h"

class XW_UTIL_EXPORT XWDocSea : public QObject
{
	Q_OBJECT
    
public:
	enum FileFormat
	{
		DVI = 0,
		PDF,
		TEX,
		HTML,
		XML,
		Doc,
		Dot,
		PPT,
		RTF,
		XLS,
		TXT,
		PS,
		Last
	};
	
	XWDocSea(QObject * parent = 0);
	
	void addConfig(const QString & key, 
                   const QString & value, 
                   const QString & prog = QString());
	void addFile(const QString & path);
	QStringList allDvi();
	QStringList allFileNames(FileFormat format);
	QStringList allHtml();
	QStringList allPdf();
	QStringList allTex();
	
	void createAllDefaultDir(const QString & topdir = QString());
	void createDefaultDir(FileFormat format,
	                      const QString & topdir = QString());
		
	QStringList findAll(const QString & name,
	                    FileFormat format);
	QStringList findAll(const QStringList & names,
	                    FileFormat format);
	QString findDvi(const QString & name);
	QString findFile(const QString & name);
	QString findFile(const QString & name,
	                 FileFormat format,
                     bool must_exist);
	FileFormat findFormat(const QString & name);
	QString findPdf(const QString & name);
	QString findTex(const QString & name);
	QString findHtml(const QString & name);
	
	QStringList getConfigDir(FileFormat format);
    QStringList getSuffixes(FileFormat format);
    QStringList getTopDirs();
    
    static void init();
    
    void insertFile(const QString & passedfname);
	
	QFile * openDvi(const QString & filename);
	QFile * openFile(const QString & filename);
    QFile * openFile(const QString & filename, FileFormat format);
    QFile * openPdf(const QString & filename);
    QFile * openTex(const QString & filename);
    QFile * openHtml(const QString & filename);
    
    static void quit();
    
    void removeConfig(const QString & key);
    
    void setCurDocPath(const QString & filename);
    void setDirs(FileFormat format, 
                 const QString & dirs = QString());
    
    QString varExpand(const QString & s);
    QString varValue(const QString & s);
    
public slots:
	void save();
	void updateDBs();
    
signals:
	void directoryChangrd(const QString & path);
};

class XWDocSeaPrivate : public XWPathSea
{
    Q_OBJECT

public:
    XWDocSeaPrivate(QObject * parent = 0);
    ~XWDocSeaPrivate() {}

    QString initFormat(int format);
};

#endif //XWDOCSEA_H

