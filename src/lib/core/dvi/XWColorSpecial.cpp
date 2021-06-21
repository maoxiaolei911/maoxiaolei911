/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWLexer.h"
#include "XWDVIRef.h"
#include "XWDVIDev.h"
#include "XWColorSpecial.h"

#define SP_COLOR_BG       0
#define SP_COLOR_PUSH     1
#define SP_COLOR_POP      2
#define SP_COLOR_DEFAULT  3

SpecialHandler colorHandler[] = {
	{"background", SP_COLOR_BG},
	{"push", SP_COLOR_PUSH},
	{"pop", SP_COLOR_POP}
};

XWColorSpecial::XWColorSpecial(XWDVICore * coreA,
	                           XWDVIRef * xrefA,
	                           XWDVIDev * devA,
	                           const char *buffer, 
	                           long sizeA,
		                       double x_userA, 
		                       double y_userA, 
		                       double magA,
		                       int typeA,
		                       QObject * parent)
	:XWDVISpecial(coreA, xrefA, devA, buffer, sizeA, x_userA, y_userA, magA, typeA, parent)
{
}

int XWColorSpecial::check(XWLexer * lexerA)
{
	lexerA->skipBlank();
	char * q = lexerA->getCIdent();
	if (!q)
		return -1;
		
	lexerA->skipBlank();
	int typeA = -1;
	if (!strcmp(q, "background"))
	{
		typeA = SP_COLOR_BG;
		delete [] q;
	}
	else if (!strcmp(q, "color"))
	{
		delete [] q;
		q = lexerA->getCIdent();
		if (!q)
			return -1;
			
		if (!strcmp(q, "push"))
			typeA = SP_COLOR_PUSH;
		else if (!strcmp(q, "pop"))
			typeA = SP_COLOR_POP;
		else
			typeA = SP_COLOR_DEFAULT;
			
		
	}
	else
		delete [] q;
		
	return typeA;
}

bool XWColorSpecial::exec(int, int)
{
	int ret = 0;
	switch (type)
	{
		case SP_COLOR_BG:
			ret = doBackground();
			break;
			
		case SP_COLOR_PUSH:
			ret = doPush();
			break;
			
		case SP_COLOR_POP:
			ret = doPop();
			break;
			
		case SP_COLOR_DEFAULT:
			ret = doDefault();
			break;
			
		default:
			return false;
			break;
	}
		
    return ret < 0 ? false : true;
}

int XWColorSpecial::doBackground()
{
	XWDVIColor  colorspec;
  	int error = readColorSpec(&colorspec);
  	if (!error) 
    	xref->setBGColor(&colorspec);

  	return  error;
}

int XWColorSpecial::doDefault()
{
	XWDVIColor  colorspec;
  	int error = readColorSpec(&colorspec);
  	if (!error) 
  	{
    	dev->clearColorStack();
    	dev->setColor(xref, &colorspec, &colorspec);
  	}

  	return  error;
}

int XWColorSpecial::doPop()
{
	dev->popColor(xref);
  	return  0;
}

int XWColorSpecial::doPush()
{
	XWDVIColor  colorspec;
  	int error = readColorSpec(&colorspec);
  	if (!error) 
    	dev->pushColor(xref, &colorspec, &colorspec);

  	return  error;
}
