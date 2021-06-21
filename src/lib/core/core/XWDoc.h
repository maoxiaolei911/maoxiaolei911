/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDOC_H
#define XWDOC_H

#include <QObject>
#include <QString>
#include <QIODevice>
#include <QFile>

#include <stdio.h>
#include "XWRef.h"
#include "XWCatalog.h"
#include "XWPage.h"

class XWString;
class XWBaseStream;
class XWOutputDev;
class XWLinks;
class XWLinkAction;
class XWLinkDest;
class XWOutline;
class XWOptionalContent;

class XW_CORE_EXPORT XWDoc : public QObject
{
	Q_OBJECT
	
public:   
    XWDoc(QObject * parent = 0);
    virtual ~XWDoc();
    
    virtual void displayPage(XWOutputDev *out, 
                             int page,
		                     double hDPI, 
		                     double vDPI, 
		                     int rotate,
		                     bool useMediaBox, 
		                     bool crop, 
		                     bool printing,
		                     bool (*abortCheckCbk)(void *data) = 0,
		                     void *abortCheckCbkData = 0);
	virtual void displayPages(XWOutputDev *out, 
	                          int firstPage, 
	                          int lastPage,
		                      double hDPI, 
		                      double vDPI, 
		                      int rotate,
		                      bool useMediaBox, 
		                      bool crop, 
		                      bool printing,
		                      bool (*abortCheckCbk)(void *data) = 0,
		                      void *abortCheckCbkData = 0);
	virtual void displayPageSlice(XWOutputDev *out, 
	                              int page,
			                      double hDPI, 
			                      double vDPI, 
			                      int rotate,
			                      bool useMediaBox, 
			                      bool crop, 
			                      bool printing,
			                      int sliceX, 
			                      int sliceY, 
			                      int sliceW, 
			                      int sliceH,
			                      bool (*abortCheckCbk)(void *data) = 0,
			                      void *abortCheckCbkData = 0);
				   
    virtual XWLinkDest * findDest(XWString * name);
    virtual int findPage(int num, int gen);
    
    virtual XWCatalog * getCatalog() { return catalog; }
    virtual XWObject *getDocInfo(XWObject *obj);
    virtual XWString  * getFileName() { return fileName; }
    virtual XWLinks   * getLinks(int page);
    virtual int getNumPages();
    XWOptionalContent *getOptionalContent() { return optContent; }
    virtual XWOutline * getOutline() { return outline; }
    virtual double      getPageCropHeight(int page);
    virtual double      getPageCropWidth(int page);
    virtual double      getPageMediaHeight(int page);
    virtual double      getPageMediaWidth(int page);
    virtual int         getPageRotate(int page);
    virtual XWRef * getXRef() { return xref; }
    
    virtual bool isOk() { return ok; }
    
    virtual bool okToPrint(bool) { return true; }
  	virtual bool okToChange(bool) { return true; }
  	virtual bool okToCopy(bool) { return true; }
  	virtual bool okToAddNotes(bool) { return true; }
        
    virtual void processLinks(XWOutputDev *out, int page);
    
    static void quit();
    
    virtual bool saveAs(const QString & name);
    virtual bool saveTo(QIODevice * dev);
    
    virtual void setCatalog(XWCatalog * catalogA);
    virtual void setOptContent(XWOptionalContent * optcontentA);
    virtual void setOutline(XWOutline * outlineA);
    virtual void setXRef(XWRef * xrefA);
	
protected:
    bool ok;
    XWString *fileName;
    QIODevice *file;
    XWRef *xref;
    XWCatalog *catalog;
    XWOutline *outline;
    XWOptionalContent *optContent;
};

#endif // XWDOC_H

