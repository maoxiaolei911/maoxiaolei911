/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWCORE_H
#define XWCORE_H

#include <QObject>
#include <QString>
#include <QIODevice>

#include "XWGlobal.h"
#include "XWRasterType.h"

class XWString;
class XWList;
class XWDoc;
class XWLinks;
class XWLinkDest;
class XWLinkAction;
class XWOutputDev;

class XW_CORE_EXPORT XWCore : public QObject
{
	Q_OBJECT
	
public:
	XWCore(XWDoc * docA, QObject * parent = 0);
    virtual ~XWCore();
        
    virtual void displayPage(XWOutputDev * out, 
                             int pgnum, 
                             int hDPI, 
                             int vDPI,
                             int rotate,
			 				 bool useMediaBox, 
			 				 bool crop, 
			 				 bool printing);
			 				 
	virtual void displayPages(XWOutputDev * out, 
                             int firstPage, 
                             int lastPage, 
                             int hDPI, 
                             int vDPI,
                             int rotate,
			 				 bool useMediaBox, 
			 				 bool crop, 
			 				 bool printing);
        
    virtual void displayPageSlice(XWOutputDev *out, 
	                              int pgnum,
			                      double hDPI, 
			                      double vDPI, 
			                      int rotate,
			                      bool useMediaBox, 
			                      bool crop, 
			                      bool printing,
			                      int sliceX, 
			                      int sliceY, 
			                      int sliceW, 
			                      int sliceH);
			   
    XWDoc * getDoc() {return doc;}
    
    virtual bool loadDoc(XWDoc * docA, XWOutputDev *);
		                  
    virtual bool loadFile(XWString *fileName, 
                          XWString *ownerPassword = 0,
		                  XWString *userPassword = NULL) = 0;
	virtual bool loadFile(QIODevice *dev, 
                          XWString *ownerPassword = 0,
		                  XWString *userPassword = NULL) = 0;
    
protected:
	XWDoc * doc;
};

#endif // XWCORE_H

