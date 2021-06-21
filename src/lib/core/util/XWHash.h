/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWHASH_H
#define XWHASH_H

#include "XWGlobal.h"

class XWString;
struct XWHashBucket;
struct XWHashIter;

class XW_UTIL_EXPORT XWHash
{
public:   
    XWHash(bool deleteKeysA = false);
    ~XWHash();
    
    //追加
    void add(XWString *key, void *val);
    void add(XWString *key, int val);
    
    //对象个数
    int  getLength() { return len; }
    //迭代下一个对象
    bool getNext(XWHashIter **iter, XWString **key, void **val);
    bool getNext(XWHashIter **iter, XWString **key, int *val);
    
    //删除迭代器
    void killIter(XWHashIter **iter);
    
    //查询对象
    int   lookupInt(char *key);
    int   lookupInt(XWString *key);    
    void *lookup(char *key);
    void *lookup(XWString *key);
  
  	//移出对象
    void *remove(char *key);
    void *remove(XWString *key);
    int   removeInt(char *key);
    int   removeInt(XWString *key);
  
  	//替换对象(如果不存在，将追加， 否则替换，而key将被删除)
    void replace(XWString *key, void *val);
    void replace(XWString *key, int val);
    
    //构造一个新迭代器
    void startIter(XWHashIter **iter);
    
private:
    void expand();
    
    XWHashBucket *find(XWString *key, int *h);
    XWHashBucket *find(char *key, int *h);
    
    int hash(XWString *key);
    int hash(char *key);
    
private:
    bool deleteKeys;
    int  size;
    int  len;
    XWHashBucket **tab;
};

//删除散列表中的对象及散列表
#define deleteXWHash(hash, T)                       \
  do {                                             \
    XWHash *_hash = (hash);                         \
    {                                              \
      XWHashIter *_iter;                            \
      XWString *_key;                               \
      void *_p;                                    \
      _hash->startIter(&_iter);                    \
      while (_hash->getNext(&_iter, &_key, &_p)) { \
        delete (T*)_p;                             \
      }                                            \
      delete _hash;                                \
    }                                              \
  } while(0)


#endif // XWHASH_H

