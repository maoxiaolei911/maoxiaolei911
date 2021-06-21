/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPICTURESEA_H
#define XWPICTURESEA_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QSettings>

#include "XWGlobal.h"
#include "XWPathSea.h"

class XW_UTIL_EXPORT XWPictureSea : public QObject
{
	Q_OBJECT
    
public:
	enum FileFormat
	{
		BMP = 0, 
		EPS,
		EXR,
		GIF,
		ICON,
		IFF,
		JPEG,
		MP,
		PBM,
		PCX,
		PDF,
		PDP,
		PICT,
		PNG,
		PS,
		PSD,
		PXR,
		Targa,
		TIFF,
		XBM,
		XPM,
		Last
	};
	
	XWPictureSea(QObject * parent = 0);
	
	void addConfig(const QString & key,
                   const QString & value,
                   const QString & prog);
	void addFile(const QString & path);
	QStringList allBMP();
	QStringList allEPS();
	QStringList allEXR();
	QStringList allFileNames(FileFormat format);
	QStringList allGIF();
	QStringList allICON();
	QStringList allIFF();
	QStringList allJPEG();
	QStringList allMP();
	QStringList allPBM();
	QStringList allPCX();
	QStringList allPDP();
	QStringList allPICT();
	QStringList allPNG();
	QStringList allPS();
	QStringList allPSD();
	QStringList allPXR();
	QStringList allTarga();
	QStringList allXBM();
	QStringList allXPM();
	
	void createAllDefaultDir(const QString & topdir = QString());
	void createDefaultDir(FileFormat format,
	                      const QString & topdir = QString());
	
	QStringList findAll(const QString & name,
	                    FileFormat format);
	QStringList findAll(const QStringList & names,
	                    FileFormat format);
	QString findFile(const QString & name);
	QString findFile(const QString & name,
	                 FileFormat format,
                     bool must_exist);
	FileFormat findFormat(const QString & name);
	
	QStringList getConfigDir(FileFormat format);
	void        getFilter(FileFormat format, 
	                      QString & cmd,
	                      QString & infilep,
	                      QString & outfilep,
	                      QString & outext);
    QStringList getSuffixes(FileFormat format);
    QStringList getTopDirs();
    
    static void init();
    void insertFile(const QString & passedfname);
    
    QFile * openFile(const QString & filename, bool * tmp);
    QFile * openFile(const QString & filename, FileFormat format);
    
    static void quit();
    
    void removeConfig(const QString & key);
    
    void setDirs(FileFormat format, 
                 const QString & dirs = QString());
	void setFilter(FileFormat format, 
	               const QString & cmd,
	               const QString & infilep,
	               const QString & outfilep,
	               const QString & outext);
    
    QString varExpand(const QString & s);
    QString varValue(const QString & s);
    
public slots:
	void save();
	void updateDBs();
    
signals:
	void directoryChangrd(const QString & path);
};

class XWPictureSeaPrivate : public XWPathSea
{
    Q_OBJECT

public:
    XWPictureSeaPrivate(QObject * parent = 0);
    ~XWPictureSeaPrivate();

    QString initFormat(int format);
    
public:
	QSettings * settings;
};

#endif //XWPICTURESEA_H

