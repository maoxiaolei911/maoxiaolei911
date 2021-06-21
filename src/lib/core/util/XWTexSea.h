/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXSEA_H
#define XWTEXSEA_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QSettings>

#include "XWGlobal.h"
#include "XWPathSea.h"

class XW_UTIL_EXPORT XWTexSea : public QObject
{
	Q_OBJECT
    
public:
	enum FileFormat
	{
		TEX = 0,
		OCP,
		OTP,
		Last
	};
	XWTexSea(QObject * parent = 0);
	
	void addConfig(const QString & key, 
                   const QString & value, 
                   const QString & prog = QString());
	void addFile(const QString & path);
	
	void createAllDefaultDir(const QString & topdir = QString());
	void createDefaultDir(FileFormat format,
	                      const QString & topdir = QString());
	
	QStringList findAll(const QString & name);
	QString findFile(const QString & name, const QString & fmt);
	QString findFile(const QString & name,
                     bool must_exist);
	QStringList getConfigDir(FileFormat format);
	QStringList getTopDirs();
	
	static void init();
	void insertFile(const QString & passedfname);
                     
	QFile * openFile(const QString & filename);
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

class XWTexSeaPrivate : public XWPathSea
{
	Q_OBJECT
    
public:
	XWTexSeaPrivate(QObject * parent = 0);
	~XWTexSeaPrivate() {}
	
	QString findFmtFile(const QString & name, const QString & fmt);
			             
	QString initFormat(int format);	
	
public:
	QString curFmt;
};

class XW_UTIL_EXPORT XWBibTexSea : public QObject
{
	Q_OBJECT
    
public:
	enum FileFormat
	{
		BIB = 0,
		BST, 
		Last
	};
	
	XWBibTexSea(QObject * parent = 0);
	
	void addConfig(const QString & key, 
                   const QString & value, 
                   const QString & prog = QString());
	void addFile(const QString & path);        
	QStringList allFileNames(FileFormat format);
	
	void createAllDefaultDir(const QString & topdir = QString());
	void createDefaultDir(FileFormat format,
	                      const QString & topdir = QString());
	
	QStringList findAll(const QString & name,
	                    FileFormat format);
	QStringList findAll(const QStringList & names,
	                    FileFormat format);
	QString findFile(const QString & name,
	                 FileFormat format,
                     bool must_exist);
	FileFormat findFormat(const QString & name);
	
	QStringList getConfigDir(FileFormat format);
    QStringList getSuffixes(FileFormat format);
    QStringList getTopDirs();
    
    static void init();
    void insertFile(const QString & passedfname);
    
    QFile * openFile(const QString & filename);
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

class XWBibTexSeaPrivate : public XWPathSea
{
	Q_OBJECT
    
public:
	XWBibTexSeaPrivate(QObject * parent = 0);
	~XWBibTexSeaPrivate() {}
    	                     
	QString initFormat(int format);
};

class XW_UTIL_EXPORT XWMakeIndexSea : public QObject
{
	Q_OBJECT
    
public:
	enum FileFormat
	{
		IST = 0,
		Last
	};
	
	XWMakeIndexSea(QObject * parent = 0);
	
	void addConfig(const QString & key, 
                   const QString & value, 
                   const QString & prog = QString());
	void addFile(const QString & path);
	QStringList allFileNames(FileFormat format);
	
	void createAllDefaultDir(const QString & topdir = QString());
	void createDefaultDir(FileFormat format,
	                      const QString & topdir = QString());
	
	QStringList findAll(const QString & name,
	                    FileFormat format);
	QStringList findAll(const QStringList & names,
	                    FileFormat format);
	QString findFile(const QString & name,
	                 FileFormat format,
                     bool must_exist);
	FileFormat findFormat(const QString & name);
	
	QStringList getConfigDir(FileFormat format);
    QStringList getSuffixes(FileFormat format);
    QStringList getTopDirs();
    
    static void init();
    void insertFile(const QString & passedfname);
    
    QFile * openFile(const QString & filename);
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

class XWMakeIndexSeaPrivate : public XWPathSea
{
	Q_OBJECT
    
public:
	XWMakeIndexSeaPrivate(QObject * parent = 0);
	~XWMakeIndexSeaPrivate() {}

    QString initFormat(int format);
};

class XW_UTIL_EXPORT XWXWTexSea : public QObject
{
	Q_OBJECT
    
public:
	enum FileFormat
	{
		Bin = 0,
		FMT,
		Font,
		Format,
		Language,
		Pool,
		Last
	};
		
	XWXWTexSea(QObject * parent = 0);
	
	void addConfig(const QString & key, 
                   const QString & value, 
                   const QString & prog = QString());
	void addFile(const QString & path);
	QStringList allFileNames(FileFormat format);
	
	void createAllDefaultDir(const QString & topdir = QString());
	void createDefaultDir(FileFormat format,
	                      const QString & topdir = QString());
	
	QStringList findAll(const QString & name,
	                    FileFormat format);
	QStringList findAll(const QStringList & names,
	                    FileFormat format);
	QString findFile(const QString & name,
	                 FileFormat format,
                     bool must_exist);
	FileFormat findFormat(const QString & name);
	
	QStringList getConfigDir(FileFormat format);
    
    static void init();
    void insertFile(const QString & passedfname);
    
    QFile * openFile(const QString & filename);
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

class XWXWTexSeaPrivate : public XWPathSea
{
	Q_OBJECT
    
public:
	XWXWTexSeaPrivate(QObject * parent = 0);
	~XWXWTexSeaPrivate() {}
	
    
    QString initFormat(int format);
};

class XW_UTIL_EXPORT XWTeXFontSetting : public QObject
{
	Q_OBJECT
    
public:
	XWTeXFontSetting(QObject * parent = 0);
	~XWTeXFontSetting();
	
	QStringList allFonts();
	
	double  getBold();
	double  getCapHeight();
	QString getCMap();
	QString getCollection();
	QString getDescription();
	double  getEFactor();
	int     getEncodingID();
	QString getFont();
	int     getIndex();
	QString getInEncoding();
	double  getItalicAngle();
	int     getLevel();
	QString getLigName(QString & infix);
	QString getMetricName();
	QString getOutEncoding();
	int     getPlane();
	int     getPlatID();
	double  getSize();
	double  getSlant();
	int     getStemV();
	QString getSubFont();
	QString getVF();
	int     getWMode();
	double  getYOffset();
	
	static void init();
	
	bool isBold();
	bool isItalic();
	bool isNoEmbed();
	bool isRotate();
	bool isSubfontLigs();
	bool isToUnicode();
	bool useSmallCaps();
	bool useVF();
	
	void remove(const QString & texname);
	
	void setBold(bool e);
	void setBoldF(double f);
	void setCapHeight(double c);
	void setCMap(const QString & name); 
	void setCollection(const QString & name); 
	void setDescription(const QString & str);
	void setEFactor(double factor);
	void setEncodingID(int id = -1);
	void setFont(const QString & name);
	void setIndex(int idx = -1);
	void setItalic(bool e);
	void setInEncoding(const QString & name);
	void setItalicAngle(double a);
	void setLevel(int l);
	void setLigName(const QString & name, 
	                const QString & subfont,
	                const QString & subfontid);
	void setMetricName(const QString & name);
	void setNoEmbed(bool e);
	void setOutEncoding(const QString & name);
	void setPlane(const QString & p);
	void setPlatID(int id = -1);
	void setRotate(bool e);
	void setSize(double s);
	void setSlant(double s);
	void setSmallCaps(bool e);
	void setStemV(int v);
	void setSubFont(const QString & name);
	void setSubfontLigs(bool e);
	QString setTexName(const QString & name);
	void setToUnicode(bool e);
	void setVF(bool e, const QString & vf);
	void setWMode(int wm);
	void setYOfsset(double o);
};

class XW_UTIL_EXPORT XWTeXFormatSetting : public QObject
{
	Q_OBJECT
    
public:
	XWTeXFormatSetting(QObject * parent = 0);
	~XWTeXFormatSetting();
	
	QStringList allFormats();
	
	QString getArguments();
	QString getAttributes();
	QString getCompiler();
	QString getDescription();
	QString getFmtName();
	QString getInput();
	QString getPreload();
	
	void remove(const QString & name);
	
	void setFormat(const QString & name);
	
	void setArguments(const QString & name = QString());
	void setAttributes(const QString & name = QString());
	void setBound(const QString & var, qint32 v);
	void setCompiler(const QString & name = QString());
	void setDescription(const QString & name = QString());
	void setFmtName(const QString & name = QString());
	void setInput(const QString & name = QString());
	void setPreload(const QString & name = QString());
	qint32 setupBoundVar(const QString & v, qint32 dflt);
	
private:
	bool hasGroup;
	QSettings * settings;
};

class XW_UTIL_EXPORT XWTeXLanguageSetting : public QObject
{
	Q_OBJECT
    
public:
	XWTeXLanguageSetting(QObject * parent = 0);
	~XWTeXLanguageSetting();
	
	QStringList allLanguages();
	
	QString getHyphenation();
	int     getLeftHyphenMin();
	QString getLoader();
	QString getPatterns();
	int     getRightHyphenMin();
	QString getSpecial();
	QString getSynonyms();
	
	void remove(const QString & name);
	
	void setHyphenation(const QString & name = QString());	
	void setLanguage(const QString & name);	
	void setLeftHyphenMin(int i);
	void setLoader(const QString & name = QString());
	void setPatterns(const QString & name = QString());
	void setRightHyphenMin(int i);
	void setSpecial(const QString & name = QString());
	void setSynonyms(const QString & name = QString());
	
private:
	bool hasGroup;
	QSettings * settings;
};

#endif //XWTEXSEA_H

