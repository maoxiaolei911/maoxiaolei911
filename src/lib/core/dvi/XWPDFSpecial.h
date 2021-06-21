/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPDFSPECIAL_H
#define XWPDFSPECIAL_H

#include "XWDVISpecial.h"

class XWCMap;

class XWPDFSpecial : public XWDVISpecial
{
	Q_OBJECT
	
public:	
	XWPDFSpecial(XWDVICore * coreA,
	               XWDVIRef * xrefA,
	               XWDVIDev * devA,
	               const char *buffer, 
	               long sizeA,
		           double x_userA, 
		           double y_userA, 
		           double magA,
		           int typeA,
		           QObject * parent = 0);
	~XWPDFSpecial() {}
	
	static int check(XWLexer * lexerA); 
	
	bool exec(int hDPIA, int vDPIA);
	
protected:
	int doAnnot();
	int doArticle();
	int doBAnn();
	int doBColor();
	int doBContent();
	int doBead();
	int doBForm();
	int doBGColor();
	int doBop();
	int doBTrans();
	int doClose();
	int doCode();
	int doContent();
	int doDest();
	int doDocInfo();
	int doDocView();
	int doEAnn();
	int doEColor();
	int doEContent();
	int doEForm();
	int doEop();
	int doETrans();
	int doFStream();
	int doImage();
	int doLink();
	int doLiteral();
	int doMapFile();
	int doMapLine();
	int doNames();
	int doNoLink();
	int doObject();
	int doOutline();
	int doPut();
	int doSColor();
	int doStream();
	int doStreamWithType(int type);
	int doToUnicode();
	int doUXObj();
	
	int safePutResdent(const char * k, XWObject *vp, XWObject *dp);
	int safePutResdict(const char * k, XWObject *vp, XWObject *dp);
	
protected:
	int hDPI;
	int vDPI;
};

#endif //XWPDFSPECIAL_H