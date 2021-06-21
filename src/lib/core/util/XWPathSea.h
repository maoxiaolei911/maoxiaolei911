/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPATHSEA_H
#define XWPATHSEA_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QChar>
#include <QHash>
#include <QFile>

#include "XWGlobal.h"

class XWFileDB;
class XWCnf;
class XWVariable;

class XWPathCache;


struct XW_UTIL_EXPORT PathSeaParams
{
	char * type;
	char * maketex;
	char * args;
	char * subdir;
	char * envs;
	char * suffixes;
	char * altsuffixes;
	bool   suffixonly;
	bool   binmode;
};


class XW_UTIL_EXPORT XWPathSea : public QObject
{
	Q_OBJECT
    
public:
	enum SrcType
    {
        Implicit = 0,
        Compile,
        TexmfCnf,
        ClientCnf,
        Environment,
        XWin,
        CmdLine
    };
	
	struct FormatInfo;
	
	friend class XWPathCache;
	
	XWPathSea(int   formatsize,
	          QObject * parent = 0);
	XWPathSea(const QString & typeA, 
	          int   formatsize,
	          const QString & cnfname,
	          const QString & dbname, 
	          QObject * parent = 0);
	virtual ~XWPathSea();
	
	virtual void addConfig(const QString & key, 
                           const QString & value, 
                           const QString & prog = QString());
                           
	virtual void addFile(const QString & path);
	
	virtual QStringList allFileNames();
	virtual QStringList allFileNames(const QStringList & suffixes);
	virtual QStringList allFileNames(PathSeaParams * params);
	virtual QStringList allPathSearch(const QString & path, 
	                                  const QString & name);
	virtual QStringList allPathSearchList(const QString & path, 
                                          const QStringList & names);
	
	virtual QString braceExpand(const QString & path);
	virtual QStringList braceExpand(const QChar ** text);
	virtual QString braceExpandElement(const QString & elt);
	
	virtual void concatElements(QStringList & target, 
                                const QStringList & more);
                                
	virtual QString createDefaultTopDir();
	virtual void    createDefaultDir(PathSeaParams * params,
	                                 const QString & topdir = QString());
                                 
    virtual QString expand(const QString & s);    
    virtual void expandAppend(QStringList & partial, 
                              const QChar * text, 
                              const QChar * p);
	virtual void expandDefault(FormatInfo * info, 
                               const QString & tryPath, 
                               const QString & source);
	virtual QString expandDefault(const QString & path, 
                                  const QString & fallback);
	virtual QString expandDot(QChar * path);
	
	        QChar * fileNameComponent(const QChar * p);
	
	virtual QStringList findAll(int format, 
	                            const QString & name);
	virtual QStringList findAll(int format, 
                                const QStringList & names);
	virtual QString findFile(const QString & name);
	virtual QString findFile(const QString & name,
	                         int format,
                             bool must_exist);
	virtual QString findFile(FormatInfo * f, 
	                         const QString & name, 
	                         bool must_exist);
	                         
	virtual int findFormat(const QString & filename);
    
    
    virtual QStringList getConfigDir(PathSeaParams * params);
    virtual QStringList getSuffixes(PathSeaParams * params);
    virtual QStringList getTopDirs();
    
    virtual QString initFormat(int);
    virtual void initFormatByParams(int fmt, 
                                    PathSeaParams * params, 
                                    const QString & extrapath = QString());
    virtual void initMakeTex(int fmt, 
                             const QString & dflt_prog,
                             const QStringList & args);
    virtual void initPath(FormatInfo * info, 
                          const QString & defaultPath,
                          const QStringList & envList);
	virtual void insertFile(const QString & passedfname);
    
    virtual QFile * openFile(const QString & name);
    virtual QFile * openFile(int fmt, 
	                         const QString & name);
    		QChar * pathElement(const QChar * p);
    virtual QString pathExpand(const QString & path);
    virtual QString pathSearch(const QString & path,  
                               const QString & name,
                     		   bool must_exist);
	virtual QString pathSearchList(const QString & path,  
                                   const QStringList & names,
                     		       bool must_exist);
    
    virtual void removeConfig(const QString & key,
                              const QString & prog = QString());
    
   	virtual void save();
   	virtual void setDirs(const QString & envvar, 
   	                     const QString & dirs = QString());
    virtual void setProgramEnabled(int fmt, 
                                   bool value, 
                                   SrcType level);
	
    virtual QString tildeExpand(const QString & name);
    
    virtual void updateDBs();
    
    virtual QString varExpand(const QString & s);
    virtual QString varValue(const QString & s);
    
signals:
	void directoryChangrd(const QString & path);
	
public:
    struct FormatInfo
    {        
        QString type;
        QString path;
        QString rawPath;
        QString pathSource;
        QString overridePath;
        QString clientPath;
        QString cnfPath;
        QString defaultPath;
        
        QStringList  suffix;
        QStringList  altSuffix;
        
        bool suffixSearchOnly;
        
        QString program;
        
        QStringList  argv;
        
        bool programEnabled;
        SrcType programEnableLevel;
        
        bool binMode;   
    };
    
    QString seaType;
    QString topDir;
    QString topDirEnv;
    QString configDir;
    int maxTopDir;
    
    int          formatSize;
    FormatInfo * formatInfo;
    XWFileDB   * fileDB;
    XWCnf      * cnf;
    XWVariable * variable;
    XWPathCache * cache;
    
    bool cnfDirty;
    
    QHash<QString, QString> topDirHash;

private:
	QChar * element(const QChar * passed_path,  bool env_p);
    
private:
	QChar * pathSeaElt;
	int pathSeaEltAlloc;
	const QChar * pathSeaPath;
};

#endif //XWPATHSEA_H