/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <QByteArray>
#include "XWFontFileID.h"

XWFontFileID::XWFontFileID()
	:num(-1),
	 gen(-1),
	 substIdx(-1),
	 name(0)
{
}

XWFontFileID::XWFontFileID(const char * nameA) 
	:num(-1),
	 gen(-1),
	 substIdx(-1),
	 name(0)
{
	if (nameA)
		name = qstrdup(nameA);
}

XWFontFileID::XWFontFileID(int numA, int genA)
	:num(numA),
	 gen(genA),
	 substIdx(-1),
	 name(0)
{
}

XWFontFileID::~XWFontFileID() 
{
	if (name)
		delete [] name;
}

bool XWFontFileID::matches(XWFontFileID *id)
{
	if (name && id->name)
		return (0 == strcmp(name, id->name));
		
	return (id->num == num && id->gen == gen);
}

