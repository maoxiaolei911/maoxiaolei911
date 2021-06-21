/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWMEDIASEA_H
#define XWMEDIASEA_H

#include <QObject>
#include <QString>
#include <QStringList>

#include "XWGlobal.h"
#include "XWPathSea.h"

class XW_UTIL_EXPORT XWMediaSea : public QObject
{
	Q_OBJECT
    
public:
	enum FileFormat
	{
		CD = 0,
		DVD,
		Flash,
		GIF,
		MNG,
		MpegAudio,
		MpegVideo,
		MP3,
		QuickTime,
		Real,
		WinAudio,
		WinVideo,
		Last
	};
	
	XWMediaSea(QObject * parent = 0);
	
	void addConfig(const QString & key,
                   const QString & value,
                   const QString & prog);
	void addFile(const QString & path);
	QStringList allCD();
	QStringList allDVD();
	QStringList allFileNames(FileFormat format);
	QStringList allFlash();
	QStringList allGIF();
	QStringList allMNG();
	QStringList allMpegAudio();
	QStringList allMpegVideo();
	QStringList allMP3();
	QStringList allQuickTime();
	QStringList allReal();
	QStringList allWinAudio();
	QStringList allWinVideo();
	
	void createAllDefaultDir(const QString & topdir = QString());
	void createDefaultDir(FileFormat format,
	                      const QString & topdir = QString());
	
	QStringList findAll(const QString & name,
	                    FileFormat format);
	QStringList findAll(const QStringList & names,
	                    FileFormat format);
	QString findCD(const QString & name);
	QString findDVD(const QString & name);
	QString findFile(const QString & name);
	QString findFile(const QString & name,
                     FileFormat format,
                     bool must_exist);
	QString findFlash(const QString & name);
	FileFormat findFormat(const QString & name);
	QString findGIF(const QString & name);
	QString findMNG(const QString & name);
	QString findMpegAudio(const QString & name);
	QString findMpegVideo(const QString & name);
	QString findMP3(const QString & name);
	QString findQuickTime(const QString & name);
	QString findReal(const QString & name);
	QString findWinAudio(const QString & name);
	QString findWinVideo(const QString & name);
	
	QStringList getConfigDir(FileFormat format);
	QStringList getSuffixes(FileFormat format);
	QStringList getTopDirs();
	
	static void init();
	void insertFile(const QString & passedfname);
	
	QFile * openFile(const QString & filename, FileFormat format);
	
	static void quit();
	
	void removeConfig(const QString & key);
    
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

class XWMediaSeaPrivate : public XWPathSea
{
    Q_OBJECT

public:
    XWMediaSeaPrivate(QObject * parent = 0);
    ~XWMediaSeaPrivate() {}

    QString initFormat(int format);
};

#endif //XWMEDIASEA_H

