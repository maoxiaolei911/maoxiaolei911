/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "XWApplication.h"
#include "XWString.h"
#include "XWFontCache.h"
#include "XWFontSetting.h"
#include "XWDocSetting.h"
#include "XWDocSea.h"
#include "XWFontSea.h"
#include "XWHelpSea.h"
#include "XWMediaSea.h"
#include "XWPictureSea.h"
#include "XWTexSea.h"
#include "XWPage.h"
#include "XWCatalog.h"
#include "XWStream.h"
#include "XWRef.h"
#include "XWLink.h"
#include "XWOutputDev.h"
#include "XWPDFLexer.h"
#include "XWPDFParser.h"
#include "XWOutline.h"
#include "XWOptionalContent.h"
#include "XWDoc.h"

XWDoc::XWDoc(QObject * parent)
	:QObject(parent),
	 ok(false),
	 fileName(0),
	 file(0),
	 xref(0),
	 catalog(0),
	 outline(0),
	 optContent(0)
{
}

XWDoc::~XWDoc()
{
	if (optContent)
		delete optContent;
		
	if (outline)
		delete outline;
		
	if (catalog)
		delete catalog;
		
	if (xref)
		delete xref;
		
	if (file)
	{
		file->close();
		delete file;
	}
	
	if (fileName)
		delete fileName;
}

void XWDoc::displayPage(XWOutputDev *out, 
                        int page,
		                double hDPI, 
		                double vDPI, 
		                int rotate,
		                bool useMediaBox, 
		                bool crop, 
		                bool printing,
		                bool (*abortCheckCbk)(void *data),
		                void *abortCheckCbkData)
{
	catalog->getPage(page)->display(out, hDPI, vDPI,
				  	 rotate, useMediaBox, crop, printing,
				     abortCheckCbk, abortCheckCbkData);
}

void XWDoc::displayPages(XWOutputDev *out, 
	                     int firstPage, 
	                     int lastPage,
		                 double hDPI, 
		                 double vDPI, 
		                 int rotate,
		                 bool useMediaBox, 
		                 bool crop, 
		                 bool printing,
		                 bool (*abortCheckCbk)(void *data),
		                 void *abortCheckCbkData)
{
	for (int page = firstPage; page <= lastPage; ++page) 
	{
    	displayPage(out, page, hDPI, vDPI, rotate, useMediaBox, crop, printing,
					abortCheckCbk, abortCheckCbkData);
  	}
}

void XWDoc::displayPageSlice(XWOutputDev *out, 
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
			                 bool (*abortCheckCbk)(void *data),
			                 void *abortCheckCbkData)
{
	catalog->getPage(page)->displaySlice(out, hDPI, vDPI,
				     rotate, useMediaBox, crop,
				     sliceX, sliceY, sliceW, sliceH,
				     printing, abortCheckCbk, abortCheckCbkData);
}

XWLinkDest * XWDoc::findDest(XWString * name)
{
	return catalog->findDest(name);
}

int XWDoc::findPage(int num, int gen)
{
	return catalog->findPage(num, gen);
}

XWObject * XWDoc::getDocInfo(XWObject *obj)
{
	return xref->getDocInfo(obj);
}

XWLinks * XWDoc::getLinks(int page)
{
	return catalog->getPage(page)->getLinks(catalog);
}

int XWDoc::getNumPages()
{
	return catalog->getNumPages();
}

double XWDoc::getPageCropHeight(int page)
{
	return catalog->getPage(page)->getCropHeight();
}

double XWDoc::getPageCropWidth(int page)
{
	return catalog->getPage(page)->getCropWidth();
}

double XWDoc::getPageMediaHeight(int page)
{
	return catalog->getPage(page)->getMediaHeight();
}

double XWDoc::getPageMediaWidth(int page)
{
	return catalog->getPage(page)->getMediaWidth();
}

int XWDoc::getPageRotate(int page)
{
	return catalog->getPage(page)->getRotate();
}

void XWDoc::processLinks(XWOutputDev *out, int page)
{
	catalog->getPage(page)->processLinks(out, catalog);
}

void XWDoc::quit()
{
	XWFontCache::close();
	XWFontSetting::quit();
	XWDocSetting::quit();
	XWDocSea::quit();
	XWFontSea::quit();
	XWHelpSea::quit();
	XWMediaSea::quit();
	XWPictureSea::quit();
	XWTexSea::quit();
	XWBibTexSea::quit();
	XWMakeIndexSea::quit();
	XWXWTexSea::quit();
}

bool XWDoc::saveAs(const QString & name)
{
	QFile fp(name);
	if (!fp.open(QIODevice::WriteOnly))
		return xwApp->openError(name, false);
		
	bool ret = saveTo(&fp);
	fp.close();
	return ret;
}

bool XWDoc::saveTo(QIODevice * dev)
{
	qint64 oldpos = file->pos();
	file->seek(0);
	char buf[1024];
	
	qint64 len = file->read(buf, 1024);
	while (len > 0)
	{
		dev->write(buf, len);
		len = file->read(buf, 1024);
	}
	
	file->seek(oldpos);
	return true;
}

void XWDoc::setCatalog(XWCatalog * catalogA)
{
	if (catalog)
		delete catalog;
		
	catalog = catalogA;
}

void XWDoc::setOptContent(XWOptionalContent * optcontentA)
{
	if (optContent)
		delete optContent;
		
	optContent = optcontentA;
}

void XWDoc::setOutline(XWOutline * outlineA)
{
	if (outline)
		delete outline;
		
	outline = outlineA;
}

void XWDoc::setXRef(XWRef * xrefA)
{
	if (xref)
		delete xref;
		
	xref = xrefA;
}

