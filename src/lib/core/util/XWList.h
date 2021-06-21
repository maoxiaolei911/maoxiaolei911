/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWLIST_H
#define XWLIST_H

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWList
{
public:   
	//����һ�����б�
    XWList();
    //����һ���ܴ��sizeA������ָ����б�
    XWList(int sizeA);
    ~XWList();
    
    //׷�ӵ��б�β��
    void append(void *p);
    void append(XWList *list);
    
    XWList *copy();
    
    //�Ƴ���i����������ָ��
    void *del(int i);
    
    void * get(int i) { return data[i]; }
    //�������
    int    getLength() { return length; }
    
    //�ڵ�i��λ���ϲ������p
    void insert(int i, void *p);
    
    void reverse();
    
    //���÷���ռ�ʱ�����������incA>0���ռ�
    //���ӣ������С
    void setAllocIncr(int incA) { inc = incA; }
    
    //��������
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

//ɾ���б��еĶ����б���

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

