/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWString.h"
#include "XWList.h"
#include "XWDoc.h"
#include "XWLink.h"
#include "XWOutputDev.h"
#include "XWCore.h"

XWCore::XWCore(XWDoc * docA, QObject * parent)
	:QObject(parent),
	 doc(docA)
{
}

XWCore::~XWCore()
{
	if (doc)
		delete doc;
}

void XWCore::displayPage(XWOutputDev * out, 
                         int pgnum, 
                         int hDPI, 
                         int vDPI,
                         int rotate,
			 			 bool useMediaBox, 
			 			 bool crop, 
			 			 bool printing)
{
	doc->displayPage(out, pgnum, hDPI, vDPI, rotate, useMediaBox, crop, printing, 0, 0);
}

void XWCore::displayPages(XWOutputDev * out, 
                          int firstPage, 
                          int lastPage, 
                          int hDPI, 
                          int vDPI,
                          int rotate,
			 			  bool useMediaBox, 
			 			  bool crop, 
			 			  bool printing)
{
	doc->displayPages(out, firstPage, lastPage, hDPI, vDPI, rotate, useMediaBox, crop, printing, 0, 0);
}

void XWCore::displayPageSlice(XWOutputDev *out, 
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
			                  int sliceH)
{
	doc->displayPageSlice(out, 
	                      pgnum, 
	                      hDPI, 
	                      vDPI, 
	                      rotate, 
	                      useMediaBox, 
			              crop, 
			              printing, 
			              sliceX, 
			              sliceY, 
			              sliceW, 
			              sliceH, 0, 0);
}

bool XWCore::loadDoc(XWDoc * docA, XWOutputDev *)
{
	if (docA || !docA->isOk())
	{
		if (docA)
			delete docA;
			
		return false;
	}
	
	if (doc) 
    	delete doc;
    	
  	doc = docA;
    	
    return true;
}

