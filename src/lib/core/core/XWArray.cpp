/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <stddef.h>
#include "XWObject.h"
#include "XWArray.h"

XWArray::XWArray(XWRef * xrefA)
    : xref(xrefA),
      elems(0),
      length(0),
      size(0),
      ref(1)
{
}

XWArray::~XWArray()
{
	if (elems)
	{
    	for (int i = 0; i < length; ++i)
    		elems[i].free();
    	free(elems);
	}
}

void XWArray::add(XWObject * obj)
{
    if (length == size) 
    {
    	if (length == 0) 
      		size = 8;
    	else 
      		size *= 2;
    	elems = (XWObject *)realloc(elems, size * sizeof(XWObject));
  	}
  	
  	elems[length] = *obj;
  	++length;
}

XWObject * XWArray::get(int i, XWObject *obj)
{
	if (i < 0 || i >= length)
		return obj->initNull();
	
	return elems[i].fetch(xref, obj);
}

XWObject * XWArray::getNF(int i, XWObject *obj)
{
	if (i < 0 || i >= length)
		return obj->initNull();
		
	return elems[i].copy(obj);
}

XWObject * XWArray::getValOrg(int i)
{
	if (i < 0 || i >= length)
		return 0;
		
	return &elems[i];
}

void XWArray::unshift(const char * name)
{
	if (length == size) 
    {
    	if (length == 0) 
      		size = 8;
    	else 
      		size *= 2;
    	elems = (XWObject *)realloc(elems, size * sizeof(XWObject));
  	}
  	
	for (int i = 0; i < length; i++)
		elems[i + 1] = elems[i];
		
	elems[0].initName(name);
	++length;
}

