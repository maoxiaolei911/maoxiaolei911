/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWARRAY_H
#define XWARRAY_H

#include "XWObject.h"

class XWRef;

class XW_CORE_EXPORT XWArray
{
public:
    XWArray(XWRef * xrefA);
    ~XWArray();
    
    void add(XWObject * obj);
    
    int  decRef() { return --ref; }
    
    XWObject * get(int i, XWObject *obj);
    int       getLength() {return length;}
  	XWObject * getNF(int i, XWObject *obj);
  	XWObject * getValOrg(int i);
    
    int  incRef() { return ++ref; }
    
    void unshift(const char * name);
    
private:
    XWRef    * xref;
    XWObject * elems; 
    int  length;
    int  size;
    int   ref;
};

#endif // XWARRAY_H

