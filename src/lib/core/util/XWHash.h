/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
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
    
    //׷��
    void add(XWString *key, void *val);
    void add(XWString *key, int val);
    
    //�������
    int  getLength() { return len; }
    //������һ������
    bool getNext(XWHashIter **iter, XWString **key, void **val);
    bool getNext(XWHashIter **iter, XWString **key, int *val);
    
    //ɾ��������
    void killIter(XWHashIter **iter);
    
    //��ѯ����
    int   lookupInt(char *key);
    int   lookupInt(XWString *key);    
    void *lookup(char *key);
    void *lookup(XWString *key);
  
  	//�Ƴ�����
    void *remove(char *key);
    void *remove(XWString *key);
    int   removeInt(char *key);
    int   removeInt(XWString *key);
  
  	//�滻����(��������ڣ���׷�ӣ� �����滻����key����ɾ��)
    void replace(XWString *key, void *val);
    void replace(XWString *key, int val);
    
    //����һ���µ�����
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

//ɾ��ɢ�б��еĶ���ɢ�б�
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

