/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWMISCSPECIAL_H
#define XWMISCSPECIAL_H

#include "XWDVISpecial.h"

class XWMISCSpecial : public XWDVISpecial
{
	Q_OBJECT
	
public:	
	XWMISCSpecial(XWDVICore * coreA,
	               XWDVIRef * xrefA,
	               XWDVIDev * devA,
	               const char *buffer, 
	               long sizeA,
		           double x_userA, 
		           double y_userA, 
		           double magA,
		           int typeA,
		           QObject * parent = 0);
	virtual ~XWMISCSpecial() {}
	
	static int check(XWLexer * lexerA); 
	
	bool exec(int, int);
	
protected:
	int doPostScriptBox();
};

#endif //XWMISCSPECIAL_H
