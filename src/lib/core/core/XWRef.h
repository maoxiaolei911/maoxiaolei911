/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWREF_H
#define XWREF_H

#include "XWObject.h"

class XWDict;
class XWStream;
class XWPDFParser;

class XW_CORE_EXPORT XWRef
{
public:
    XWRef();
    virtual ~XWRef() {}

	virtual XWObject *fetch(int , int , XWObject *, int) {return 0;}
	
	virtual XWObject * getCatalog(XWObject *) { return 0; }
	virtual XWObject * getDocInfo(XWObject * ) {return 0;}
  	virtual XWObject * getDocInfoNF(XWObject * ) {return 0;}
  	virtual uint       getLastXRefPos() { return 0; }
  	virtual int        getNumObjects() { return 0; }
  	virtual int        getRootGen() { return -1; }
  	virtual int        getRootNum() { return -1; }
  	virtual int        getSize() { return 0; }
  	virtual bool       getStreamEnd(uint , uint * ) {return false;}
  	virtual XWObject * getTrailerDict() {return 0;}
  
    virtual bool isEncrypted() { return false; }
    virtual bool isOk() { return false; }
    
    virtual bool okToAddNotes(bool) {return true;}
    virtual bool okToChange(bool) {return true;}
    virtual bool okToCopy(bool) {return true;}
    virtual bool okToPrint(bool) {return true;}

    virtual void setEncryption(int,
                               bool,
                               uchar *,
                               int ,
                               int ,
                               CryptAlgorithm) {}

protected:	
	virtual uint strToUnsigned(char *s);
};

#endif // XWREF_H

