/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPDFDOC_H
#define XWPDFDOC_H

#include "XWDoc.h"

#define supportedPDFVersionNum 1.7

class XW_PDF_EXPORT XWPDFDoc : public XWDoc
{
	Q_OBJECT
	
public:   
    XWPDFDoc(QObject * parent = 0);
    ~XWPDFDoc();
    
    XWBaseStream *getBaseStream() { return str; }
    
    bool isLinearized();
    
    bool load(XWString *fileNameA, 
              XWString *ownerPassword = NULL,
	          XWString *userPassword = NULL, 
	          void *guiDataA = NULL);
	bool load(XWBaseStream *strA, 
	          XWString *ownerPassword = NULL,
	          XWString *userPassword = NULL, 
	          void *guiDataA = NULL);
	          
	virtual bool okToPrint(bool ignoreOwnerPW)
    	{ return xref->okToPrint(ignoreOwnerPW); }
  	virtual bool okToChange(bool ignoreOwnerPW)
    	{ return xref->okToChange(ignoreOwnerPW); }
  	virtual bool okToCopy(bool ignoreOwnerPW)
    	{ return xref->okToCopy(ignoreOwnerPW); }
  	virtual bool okToAddNotes(bool ignoreOwnerPW)
    	{ return xref->okToAddNotes(ignoreOwnerPW); }
    
protected:
    bool checkEncryption(XWString *ownerPassword, XWString *userPassword);
    bool checkHeader();    
    
    bool setup(XWString *ownerPassword, XWString *userPassword);
    
protected:
    XWBaseStream *str;
    void *guiData;
    double pdfVersion;
};

#endif // XWPDFDOC_H

