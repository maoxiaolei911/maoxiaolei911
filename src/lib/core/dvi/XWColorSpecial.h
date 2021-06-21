/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWCOLORSPECIAL_H
#define XWCOLORSPECIAL_H

#include "XWDVISpecial.h"

class XWColorSpecial : public XWDVISpecial
{
	Q_OBJECT
	
public:	
	XWColorSpecial(XWDVICore * coreA,
	               XWDVIRef * xrefA,
	               XWDVIDev * devA,
	               const char *buffer, 
	               long sizeA,
		           double x_userA, 
		           double y_userA, 
		           double magA,
		           int typeA,
		           QObject * parent = 0);
	virtual ~XWColorSpecial() {}
	
	static int check(XWLexer * lexerA); 
	
	bool exec(int, int);
	
protected:
	int doBackground();
	int doDefault();
	int doPop();
	int doPush();
};

#endif //XWCOLORSPECIAL_H
