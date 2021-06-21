/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFILEDB_H
#define XWFILEDB_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMultiHash>
#include <QHash>
#include <QTextStream>

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWFileDB : public QObject
{
    Q_OBJECT
    
public:
	XWFileDB(QObject * parent = 0);
    XWFileDB(const QString & nameA, QObject * parent = 0);
    XWFileDB(const QString & nameA, 
             const QString & aA, 
             QObject * parent = 0);
    ~XWFileDB() {}
    
    void addDir(const QString & dir);
    QStringList allTopDir();
    
    QStringList files();
    QStringList files(const QStringList & suffixes);
    
    QStringList fileNames();
    QStringList fileNames(const QStringList & suffixes);
    
    void insert(const QString & passedfname);
    void init(const QString & topdir);
    
    QStringList search(const QString & name, 
                       const QString & origpathelt,
                       bool  all);
                              
    QStringList searchList(const QStringList & names, 
                           const QString & pathelt,
                           bool  all);
                           
	void update(const QString & topdir);
        
private:
    void aliasBuild(QMultiHash<QString, QString> & table, 
                    const QString & filename);
    
    void build(QMultiHash<QString, QString> & table, 
               const QString & filename);
    void buildDir(const QString & parentDir, 
                  const QString & curDir);
    
    bool checkFile(const QString & path);
    
    bool eltInDb(const QString & dbdir, const QString & pathelt);
    
    bool isAbsolute(const QString & filename, bool relative_ok);
    
    QStringList lookup(QMultiHash<QString, QString> & table, 
                       const QString & key);
    
    bool match(const QChar * filename, const QChar * pathelt);
    
    void travers(QTextStream & stream, 
                 const QString & parentDir, 
                 const QString & curDir, 
                 const QString & dbtmpfile);
                 
    void updateFile(const QString & file);
    
private:
    QString dbName;
    QString aliasesName;
    
    QMultiHash<QString, QString>  db;
    QMultiHash<QString, QString>  aliasesdb;
    QHash<QString, int> dbDirList;
};

#endif // XWFILEDB_H
