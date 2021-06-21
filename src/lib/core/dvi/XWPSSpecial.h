/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSSPECIAL_H
#define XWPSSPECIAL_H

#include "XWDVISpecial.h"

class XWPSSpecial : public XWDVISpecial
{
	Q_OBJECT
	
public:	
	XWPSSpecial(XWDVICore * coreA,
	            XWDVIRef * xrefA,
	            XWDVIDev * devA,
	            const char *buffer, 
	            long sizeA,
		        double x_userA, 
		        double y_userA, 
		        double magA,
		        int typeA,
		        QObject * parent = 0);
	virtual ~XWPSSpecial() {}
	
	static int check(XWLexer * lexerA); 
		
	bool exec(int hDPIA, int vDPIA);
	
protected:
	int doDefault();
	int doFile();
	int doLiteral();
	int doPlotFile();
	
	char * parseFileName();
	
protected:
	int hDPI;
	int vDPI;
};

#endif //XWPSSPECIAL_H
