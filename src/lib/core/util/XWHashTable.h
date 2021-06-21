/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWHASHTABLE_H
#define XWHASHTABLE_H

#include <QObject>
#include <QString>
#include <QStringList>

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWHashTable : public QObject
{
    Q_OBJECT
    
public:
           enum HashType
           {
              NormalHash = 0,
              LowerHash = 1,
              UpperHash = 2
           };
    
           XWHashTable(int size = 503, QObject * parent = 0);
           XWHashTable(QObject * parent = 0);
           virtual ~XWHashTable();
    
           virtual bool append(XWHashKey * key, XWHashValue * value);
           virtual bool append(const QString & key, XWHashValue * value, HashType ht = NormalHash);
           virtual bool append(const QString & key, const QString & value, HashType ht = NormalHash);
           virtual bool append(const QString & key, int value, HashType ht = NormalHash);
           virtual bool append(const QString & key, const QStringList & value, HashType ht = NormalHash);
           virtual bool append(const char * key, int len, XWHashValue * value);
           virtual bool append(const char * key, int len, const char * value, int vlen);
           virtual bool append(int key, XWHashValue * value);
           virtual bool append(int key, const QString & value);
           virtual bool append(int key, const QStringList & value);
           virtual bool append(int key, int value);
    
           virtual void clear();
    
           virtual XWHashEntry * getEntry(int n);    
           virtual int           getTableSize() {return tableSize;}
           
    static quint32 hash(const QString & key);
           virtual bool  hasEntry(int n);
    
           virtual bool init(int size = 503);
           virtual bool insert(XWHashKey * key, XWHashValue * value);
           virtual bool insert(const QString & key, XWHashValue * value, HashType ht = NormalHash);
           virtual bool insert(const QString & key, const QString & value, HashType ht = NormalHash);
           virtual bool insert(const QString & key, int value, HashType ht = NormalHash);
           virtual bool insert(const QString & key, const QStringList & value, HashType ht = NormalHash);
           virtual bool insert(const char * key, int len, XWHashValue * value);
           virtual bool insert(const char * key, int len, const char * value, int vlen);
           virtual bool insert(int key, XWHashValue * value);
           virtual bool insert(int key, const QString & value);
           virtual bool insert(int key, const QStringList & value);
           virtual bool insert(int key, int value);
    
           virtual XWHashValue * lookup(XWHashKey * key);
           virtual XWHashValue * lookup(const QString & key, HashType ht = NormalHash);
           virtual XWHashValue * lookup(const char * key, int klen);
           virtual XWHashValue * lookup(int key);
           virtual QStringList   lookupAll(const QString & key, HashType ht = NormalHash);
    static quint32 lowerHash(const QString & key);
    
           virtual int number() {return count;}
    
           virtual void remove(XWHashKey * key);
           virtual void remove(const QString & key, HashType ht = NormalHash);
           virtual void remove(const char * key, int klen);
           virtual void remove(int key);
    
    static quint32 upperHash(const QString & key);
    
public:
    int tableSize;
    int count;
    XWHashEntry ** table;
        
private:
    bool append(const QString & key, HashType ht, XWHashValue * value);
    
    quint32 hash(const char * key, int len);
    quint32 hash(const QString & key, HashType t);
    
    bool insert(const QString & key, HashType ht, XWHashValue * value);
    
    void release(XWHashKey * key, XWHashValue * value);
};

#endif // XWHASHTABLE_H
