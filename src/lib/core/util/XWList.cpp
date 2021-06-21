/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "XWList.h"

XWList::XWList()
{
    size = 8;
    data = (void **)malloc(size * sizeof(void*));
    length = 0;
    inc = 0;
}

XWList::XWList(int sizeA)
{
    size = sizeA;
    data = (void **)malloc(size * sizeof(void*));
    length = 0;
    inc = 0;
}

XWList::~XWList()
{
    free(data);
}

void XWList::append(void *p)
{
    if (length >= size) 
        expand();
        
    data[length++] = p;
}

void XWList::append(XWList *list)
{
    while (length + list->length > size) 
        expand();
    
    for (int i = 0; i < list->length; ++i) 
        data[length++] = list->data[i];
}

XWList * XWList::copy()
{
	XWList *ret;

  ret = new XWList(length);
  ret->length = length;
  memcpy(ret->data, data, length * sizeof(void *));
  ret->inc = inc;
  return ret;
}

void * XWList::del(int i)
{
 void *p;

  p = data[i];
  if (i < length - 1) {
    memmove(data+i, data+i+1, (length - i - 1) * sizeof(void *));
  }
  --length;
  if (size - length >= ((inc > 0) ? inc : size/2)) {
    shrink();
  }
  return p;
}

void XWList::insert(int i, void *p)
{
  if (length >= size) {
    expand();
  }
  if (i < 0) {
    i = 0;
  }
  if (i < length) {
    memmove(data+i+1, data+i, (length - i) * sizeof(void *));
  }
  data[i] = p;
  ++length;
}

void XWList::reverse()
{
	void *t;
  int n, i;

  n = length / 2;
  for (i = 0; i < n; ++i) {
    t = data[i];
    data[i] = data[length - 1 - i];
    data[length - 1 - i] = t;
  }
}

void XWList::sort(int (*cmp)(const void *ptr1, const void *ptr2))
{
    qsort(data, length, sizeof(void *), cmp);
}

void XWList::expand()
{
    size += (inc > 0) ? inc : size;
    data = (void **)realloc(data, size * sizeof(void*));
}

void XWList::shrink()
{
    size -= (inc > 0) ? inc : size/2;
    data = (void **)realloc(data, size * sizeof(void*));
}

