/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWHTMLSPECIAL_H
#define XWHTMLSPECIAL_H

#include "XWDVISpecial.h"

class HTMLState;

class XWHTMLSpecial : public XWDVISpecial
{
	Q_OBJECT
	
public:	
	XWHTMLSpecial(XWDVICore * coreA,
	               XWDVIRef * xrefA,
	               XWDVIDev * devA,
	               const char *buffer, 
	               long sizeA,
		           double x_userA, 
		           double y_userA, 
		           double magA,
		           int typeA,
		           QObject * parent = 0);
	virtual ~XWHTMLSpecial() {}
		
	static int check(XWLexer *); 
	
	bool exec(int , int);
	
protected:
	int  anchorClose(HTMLState *sd);
	int  anchorOpen(XWObject *attr, HTMLState *sd);
	
	int  baseEmpty(XWObject *attr, HTMLState *sd);
	
	int        cvtAToTMatrix(PDFTMatrix *M, const char *ptr, char **nextptr);

	char * fqurl(const char *baseurl, const char *name);
	
	int imgEmpty(XWObject *attr);

	int openDest(const char *name, HTMLState *sd);
	int openLink(const char *name, HTMLState *sd);

	int parseKeyVal(char **kp, char **vp);
	
	int readHTMLTag(char *name, 
	                XWObject *attr, 
	                int *type);
};

#endif //XWHTMLSPECIAL_H
