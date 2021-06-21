/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stddef.h>
#include <string.h>
#include "XWObject.h"
#include "XWRef.h"
#include "XWDict.h"

struct DictEntry
{
    char * key;
    XWObject val;
};

XWDict::XWDict(XWRef * xrefA)
    :xref(xrefA),
     entries(0),
     size(0),
     length(0),
     ref(1)
{
}

XWDict::~XWDict()
{
	if (entries)
	{
		for (int i = 0; i < length; ++i) 
		{
    		delete [] entries[i].key;
    		entries[i].val.free();
  		}
  		
  		free(entries);
  	}
}

inline DictEntry * XWDict::find(const char *key)
{
	for (int i = 0; i < length; ++i) 
	{
    	if (!strcmp(key, entries[i].key))
      		return &entries[i];
  	}
  	return 0;
}

void XWDict::add(char *key, XWObject *val)
{
	if (length == size) 
	{
    	if (length == 0) 
      		size = 8;
    	else 
      		size *= 2;
    	entries = (DictEntry *)realloc(entries, size * sizeof(DictEntry));
  	}
  	entries[length].key = key;
  	entries[length].val = *val;
  	++length;
}

char * XWDict::getKey(int i)
{
	return entries[i].key;
}

XWObject * XWDict::getVal(int i, XWObject * obj)
{
	return entries[i].val.fetch(xref, obj);
}

XWObject * XWDict::getValNF(int i, XWObject * obj)
{
	return entries[i].val.copy(obj);
}

XWObject * XWDict::getValOrg(int i)
{
	return (&(entries[i].val));
}

bool XWDict::is(const char *type)
{
	DictEntry * e = find("Type");
	if (!e)
		return false;

  	return e->val.isName(type);
}

XWObject * XWDict::lookup(const char *key, XWObject * obj, int recursion)
{
	DictEntry *e = find(key);

	return e? e->val.fetch(xref, obj, recursion) : obj->initNull();
}

XWObject * XWDict::lookupNF(const char *key, XWObject * obj)
{
	DictEntry *e = find(key);
	if (!e)
		return obj->initNull();

  	return e->val.copy(obj);
}

XWObject * XWDict::lookupNFOrg(const char *key)
{
	DictEntry *e = find(key);
	if (!e)
		return 0;
		
	return &(e->val);
}

void XWDict::merge(XWDict * src)
{
	if ((length + src->length) > size)
	{
		size = length + src->length + 8;
		entries = (DictEntry *)realloc(entries, size * sizeof(DictEntry));
	}
	
	for (int i = 0; i < src->length; i++)
	{
		entries[length + i].key = qstrdup((src->entries[i].key));
		XWObject * val = &(src->entries[i].val);
		val->copy(&(entries[length + i].val));
	}
	
	length += src->length;
}

void XWDict::remove(const char *key)
{
	int i = 0;
	for (; i < length; ++i) 
	{
    	if (!strcmp(key, entries[i].key))
  			break;
  	}
  	
  	if (i != length)
  	{
  		delete [] entries[i].key;
  		entries[i].val.free();
  			
  		for (int k = i; k < length - 1; k++)
  		{
  			entries[k].key = entries[k + 1].key;
  			entries[k].val = entries[k + 1].val;
  		}
  			
  		length--;
  	}
}

