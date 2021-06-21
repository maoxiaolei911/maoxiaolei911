/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDICT_H
#define XWDICT_H

#include "XWObject.h"

struct DictEntry;

class XW_CORE_EXPORT XWDict
{
public:
    XWDict(XWRef * xrefA);
    ~XWDict();

	void add(char *key, XWObject *val);
	
	int decRef() { return --ref; }
	
	char     * getKey(int i);
	int        getLength() { return length; }
	XWObject * getVal(int i, XWObject * obj);
  	XWObject * getValNF(int i, XWObject * obj);
  	XWObject * getValOrg(int i);
	
	int  incRef() { return ++ref; }
	bool is(const char *type);
	
	XWObject *lookup(const char *key, XWObject * obj, int recursion = 0);
  	XWObject *lookupNF(const char *key, XWObject * obj);
  	XWObject *lookupNFOrg(const char *key);
  	
  	void merge(XWDict * src);
  	
  	void remove(const char *key);
  	
  	void setXRef(XWRef *xrefA) { xref = xrefA; }
  
private:
	DictEntry *find(const char *key);
	    
private:
    XWRef *xref;
    DictEntry *entries;
    int size;	
    int length;
    int ref;
};

#endif // XWDICT_H

