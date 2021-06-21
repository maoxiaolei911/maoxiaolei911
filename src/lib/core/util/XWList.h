/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWLIST_H
#define XWLIST_H

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWList
{
public:   
	//构造一个空列表
    XWList();
    //构造一个能存放sizeA个对象指针的列表
    XWList(int sizeA);
    ~XWList();
    
    //追加到列表尾部
    void append(void *p);
    void append(XWList *list);
    
    XWList *copy();
    
    //移出第i个，返回其指针
    void *del(int i);
    
    void * get(int i) { return data[i]; }
    //对象个数
    int    getLength() { return length; }
    
    //在第i个位置上插入对象p
    void insert(int i, void *p);
    
    void reverse();
    
    //设置分配空间时的增量。如果incA>0，空间
    //增加，否则减小
    void setAllocIncr(int incA) { inc = incA; }
    
    //快速排序
    void sort(int (*cmp)(const void *ptr1, const void *ptr2));
    
private:
    void expand();
    
    void shrink();
    
private:
    void **data;
    int size;	
    int length;
    int inc;
};

//删除列表中的对象及列表本身

#define deleteXWList(list, T)                        \
  do {                                              \
    XWList *_list = (list);                          \
    {                                               \
      int _i;                                       \
      for (_i = 0; _i < _list->getLength(); ++_i) { \
        delete (T*)_list->get(_i);                  \
      }                                             \
      delete _list;                                 \
    }                                               \
  } while (0)


#endif // XWLIST_H

