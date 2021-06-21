/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "XWString.h"
#include "XWHash.h"


struct XWHashBucket 
{
    XWString *key;
    union 
    {
        void *p;
        int i;
    } val;
    XWHashBucket *next;
};

struct XWHashIter 
{
    int h;
    XWHashBucket *p;
};


XWHash::XWHash(bool deleteKeysA)
{
    deleteKeys = deleteKeysA;
    size = 7;
    tab = (XWHashBucket **)malloc(size * sizeof(XWHashBucket *));
    for (int h = 0; h < size; ++h) 
        tab[h] = 0;
    len = 0;
}

XWHash::~XWHash()
{
  XWHashBucket *p;
  int h;

  for (h = 0; h < size; ++h) {
    while (tab[h]) {
      p = tab[h];
      tab[h] = p->next;
      if (deleteKeys) {
	delete p->key;
      }
      delete p;
    }
  }
  if (tab)
  free(tab);
}

void XWHash::add(XWString *key, void *val)
{
  XWHashBucket *p;
  int h;

  // expand the table if necessary
  if (len >= size) {
    expand();
  }

  // add the new symbol
  p = new XWHashBucket;
  p->key = key;
  p->val.p = val;
  h = hash(key);
  p->next = tab[h];
  tab[h] = p;
  ++len;
}

void XWHash::add(XWString *key, int val)
{
  XWHashBucket *p;
  int h;

  // expand the table if necessary
  if (len >= size) {
    expand();
  }

  // add the new symbol
  p = new XWHashBucket;
  p->key = key;
  p->val.i = val;
  h = hash(key);
  p->next = tab[h];
  tab[h] = p;
  ++len;
}

bool XWHash::getNext(XWHashIter **iter, XWString **key, void **val)
{
  if (!*iter) {
    return false;
  }
  if ((*iter)->p) {
    (*iter)->p = (*iter)->p->next;
  }
  while (!(*iter)->p) {
    if (++(*iter)->h == size) {
      delete *iter;
      *iter = NULL;
      return false;
    }
    (*iter)->p = tab[(*iter)->h];
  }
  *key = (*iter)->p->key;
  *val = (*iter)->p->val.p;
  return true;
}

bool XWHash::getNext(XWHashIter **iter, XWString **key, int *val)
{
  if (!*iter) {
    return false;
  }
  if ((*iter)->p) {
    (*iter)->p = (*iter)->p->next;
  }
  while (!(*iter)->p) {
    if (++(*iter)->h == size) {
      delete *iter;
      *iter = NULL;
      return false;
    }
    (*iter)->p = tab[(*iter)->h];
  }
  *key = (*iter)->p->key;
  *val = (*iter)->p->val.i;
  return true;
}

void XWHash::killIter(XWHashIter **iter)
{
  delete *iter;
  *iter = NULL;
}

void * XWHash::lookup(char *key)
{
  XWHashBucket *p;
  int h;

  if (!(p = find(key, &h))) {
    return NULL;
  }
  return p->val.p;
}

void * XWHash::lookup(XWString *key)
{
  XWHashBucket *p;
  int h;

  if (!(p = find(key, &h))) {
    return NULL;
  }
  return p->val.p;
}

int XWHash::lookupInt(char *key)
{
  XWHashBucket *p;
  int h;

  if (!(p = find(key, &h))) {
    return 0;
  }
  return p->val.i;
}

int XWHash::lookupInt(XWString *key)
{
    XWHashBucket *p = 0;
    int h = 0;

    if (!(p = find(key, &h))) 
        return 0;
    return p->val.i;
}

void * XWHash::remove(char *key)
{
    XWHashBucket *p = 0;
    int h = 0;
    
    if (!(p = find(key, &h)))
        return 0;
        
    XWHashBucket ** q = &tab[h];
    while (*q != p) 
        q = &((*q)->next);
    
    *q = p->next;
    if (deleteKeys) 
        delete p->key;
        
    void * val = p->val.p;
    delete p;
    --len;
    return val;
}

void * XWHash::remove(XWString *key)
{
  XWHashBucket *p;
  XWHashBucket **q;
  void *val;
  int h;

  if (!(p = find(key, &h))) {
    return NULL;
  }
  q = &tab[h];
  while (*q != p) {
    q = &((*q)->next);
  }
  *q = p->next;
  if (deleteKeys) {
    delete p->key;
  }
  val = p->val.p;
  delete p;
  --len;
  return val;
}

int XWHash::removeInt(char *key)
{
  XWHashBucket *p;
  XWHashBucket **q;
  int val;
  int h;

  if (!(p = find(key, &h))) {
    return 0;
  }
  q = &tab[h];
  while (*q != p) {
    q = &((*q)->next);
  }
  *q = p->next;
  if (deleteKeys) {
    delete p->key;
  }
  val = p->val.i;
  delete p;
  --len;
  return val;
}

int XWHash::removeInt(XWString *key)
{
  XWHashBucket *p;
  XWHashBucket **q;
  int val;
  int h;

  if (!(p = find(key, &h))) {
    return 0;
  }
  q = &tab[h];
  while (*q != p) {
    q = &((*q)->next);
  }
  *q = p->next;
  if (deleteKeys) {
    delete p->key;
  }
  val = p->val.i;
  delete p;
  --len;
  return val;
}

void XWHash::replace(XWString *key, void *val)
{
  XWHashBucket *p;
  int h;

  if ((p = find(key, &h))) {
    p->val.p = val;
    if (deleteKeys) {
      delete key;
    }
  } else {
    add(key, val);
  }
}

void XWHash::replace(XWString *key, int val)
{
  XWHashBucket *p;
  int h;

  if ((p = find(key, &h))) {
    p->val.i = val;
    if (deleteKeys) {
      delete key;
    }
  } else {
    add(key, val);
  }
}

void XWHash::startIter(XWHashIter **iter)
{
  *iter = new XWHashIter;
  (*iter)->h = -1;
  (*iter)->p = NULL;
}

void XWHash::expand()
{
  XWHashBucket **oldTab;
  XWHashBucket *p;
  int oldSize, h, i;

  oldSize = size;
  oldTab = tab;
  size = 2*size + 1;
  tab = (XWHashBucket **)malloc(size * sizeof(XWHashBucket *));
  for (h = 0; h < size; ++h) {
    tab[h] = NULL;
  }
  for (i = 0; i < oldSize; ++i) {
    while (oldTab[i]) {
      p = oldTab[i];
      oldTab[i] = oldTab[i]->next;
      h = hash(p->key);
      p->next = tab[h];
      tab[h] = p;
    }
  }
  free(oldTab);
}

XWHashBucket * XWHash::find(XWString *key, int *h)
{
  XWHashBucket *p;

  *h = hash(key);
  for (p = tab[*h]; p; p = p->next) {
    if (!p->key->cmp(key)) {
      return p;
    }
  }
  return NULL;
}

XWHashBucket * XWHash::find(char *key, int *h)
{
  XWHashBucket *p;

  *h = hash(key);
  for (p = tab[*h]; p; p = p->next) {
    if (!p->key->cmp(key)) {
      return p;
    }
  }
  return NULL;
}

int XWHash::hash(XWString *key)
{
  const char *p;
  unsigned int h;
  int i;

  h = 0;
  for (p = key->getCString(), i = 0; i < key->getLength(); ++p, ++i) {
    h = 17 * h + (int)(*p & 0xff);
  }
  return (int)(h % size);
}

int XWHash::hash(char *key)
{
  const char *p;
  unsigned int h;

  h = 0;
  for (p = key; *p; ++p) {
    h = 17 * h + (int)(*p & 0xff);
  }
  return (int)(h % size);
}

