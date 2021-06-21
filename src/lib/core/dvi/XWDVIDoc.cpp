/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <QtDebug>
#include <QCoreApplication>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <QUrl>
#include <QCryptographicHash>
#include "XWConst.h"
#include "XWUtil.h"
#include "XWNumberUtil.h"
#include "XWString.h"
#include "XWAes.h"
#include "XWApplication.h"
#include "XWFileName.h"
#include "XWLexer.h"
#include "XWDocSea.h"
#include "XWDocSetting.h"
#include "XWPictureSea.h"
#include "XWOutline.h"
#include "XWDVIEncrypt.h"
#include "XWDVIParser.h"
#include "XWDVIRef.h"
#include "XWDVICore.h"
#include "XWPDFSpecial.h"
#include "XWOptionalContent.h"
#include "XWDVIDoc.h"

XWDVIDoc::XWDVIDoc(QObject * parent)
	:XWDoc(parent),
	 core(0),
	 pageLoc(0),
	 numPages(0),
	 processing_page(0),
	 linear(0)
{
}

XWDVIDoc::~XWDVIDoc()
{
	if (core)
		delete core;
		
	if (pageLoc)
		free(pageLoc);
}

void XWDVIDoc::close(bool finished)
{
	core->endDoc(finished);
}

void XWDVIDoc::displayPage(XWOutputDev *out, 
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
	core->setPageNo(page - 1);
	core->setDPI(hDPI, vDPI);
	doPage(page - 1);
	
	XWDoc::displayPage(out, page, 
		               hDPI, vDPI, 
		               rotate, useMediaBox, 
		               crop, printing, 
		               abortCheckCbk, abortCheckCbkData);
}

void XWDVIDoc::displayPage(int page)
{
	if (core->isMPs())
		core->doMPSPage(file);
	else
	{
		core->setPageNo(page);
		doPage(page);
	}
}

void XWDVIDoc::displayPageSlice(XWOutputDev *out, 
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
	core->setPageNo(page - 1);
	core->setDPI(hDPI, vDPI);
	doPage(page - 1);
	
	XWDoc::displayPageSlice(out, page, 
		                    hDPI, vDPI, 
		                    rotate, useMediaBox, 
		                    crop, printing, 
		                    sliceX, sliceY,
		                    sliceW, sliceH,
		                    abortCheckCbk, abortCheckCbkData);
}

XWDVIDoc * XWDVIDoc::findDoc(const QString & filename, int pg)
{
	pg = pg - 1;
	bool mp = false;
	XWFileName fn(filename);
	bool local = fn.isLocal();
	if (!local)
	{
		QUrl url(filename);
		QString path = url.path();
		if (path.endsWith(".mps", Qt::CaseInsensitive))
			mp = true;
	}
	else if (filename.endsWith(".mps", Qt::CaseInsensitive))
		mp = true;
		
	if (mp || pg < 0)
	{
		XWDVIDoc * docA = new XWDVIDoc;
		if (!docA->load(filename, 0))
		{
			delete docA;
			docA = 0;
		}
		
		return docA;
	}
	
	
	if (!local)
	{
		QUrl url(filename);
		if (!url.hasQueryItem("Page"))
		{
			QString p = QString("%1").arg(pg);
			url.addQueryItem("Page", p);
		}
		QString tmpfilename = xwApp->getFile(url);
		if (tmpfilename.isEmpty())
			return 0;
			
		QFile fileIn(tmpfilename);
		if (!fileIn.open(QIODevice::ReadOnly))
			return 0;
		
		QByteArray ba = fileIn.readAll();
		fileIn.close();
		fileIn.remove();
		QBuffer * fileA = new QBuffer;
		fileA->open(QBuffer::ReadWrite);
		fileA->write(ba);
		fileA->close();
		fileA->open(QIODevice::ReadOnly);
		XWDVIDoc * docA = new XWDVIDoc;
		QString ss = url.toString(QUrl::RemoveAuthority | QUrl::RemoveQuery);
		QByteArray fba = ss.toLocal8Bit();
		docA->fileName = new XWString(fba.data(), fba.size());
		if (!docA->load(fileA, 0, 0))
		{
			delete docA;
			docA = 0;
		}
		
		return docA;
	}
	
	XWDocSea sea;
	QIODevice * tmpfile = sea.openDvi(filename);
	char buf[5];
	tmpfile->read(buf, 3);
	buf[3] = '\0';
	if (!strncmp(buf, "dvx", 3))
		tmpfile = readFromX(tmpfile);
	else 
		tmpfile->seek(0);
			
	QBuffer * fileA = new QBuffer;
	fileA->open(QBuffer::ReadWrite);
	bool res = loadPage(tmpfile, fileA, pg, 0);
	tmpfile->close();
	delete tmpfile;
	if (!res)
	{
		fileA->close();
		delete fileA;
		return 0;
	}
		
	XWDVIDoc * docA = new XWDVIDoc;
	QByteArray fba = filename.toLocal8Bit();
	docA->fileName = new XWString(fba.data(), fba.size());
	if (!docA->load(fileA, 0, 0))
	{
		delete docA;
		docA = 0;
	}
		
	return docA;
}

XWDVIDoc * XWDVIDoc::findDoc(const QString & filename, XWString * namedDest)
{
	bool mp = false;
	XWFileName fn(filename);
	bool local = fn.isLocal();
	if (!local)
	{
		QUrl url(filename);
		QString path = url.path();
		if (path.endsWith(".mps", Qt::CaseInsensitive))
			mp = true;
	}
	else if (filename.endsWith(".mps", Qt::CaseInsensitive))
		mp = true;
	
	if (mp || !namedDest)
	{
		XWDVIDoc * docA = new XWDVIDoc;
		if (!docA->load(filename, 0))
		{
			delete docA;
			docA = 0;
		}
		
		return docA;
	}
	
	if (!local)
	{
		QUrl url(filename);
		if (!url.hasQueryItem("Destination"))
		{
			QString p = namedDest->toQString();
			url.addQueryItem("Destination", p);
		}
		QString tmpfilename = xwApp->getFile(url);
		QFile fileIn(tmpfilename);
		if (!fileIn.open(QIODevice::ReadOnly))
			return 0;
		
		QByteArray ba = fileIn.readAll();
		fileIn.close();
		fileIn.remove();
		QBuffer * fileA = new QBuffer;
		fileA->open(QBuffer::ReadWrite);
		fileA->write(ba);
		fileA->close();
		fileA->open(QIODevice::ReadOnly);
		XWDVIDoc * docA = new XWDVIDoc;
		QString ss = url.toString(QUrl::RemoveAuthority | QUrl::RemoveQuery);
		QByteArray fba = ss.toLocal8Bit();
		docA->fileName = new XWString(fba.data(), fba.size());
		if (!docA->load(fileA, 0, 0))
		{
			delete docA;
			docA = 0;
		}
		
		return docA;
	}
	
	XWDocSea sea;
	QIODevice * tmpfile = sea.openDvi(filename);
	if (!tmpfile)
		return 0;
		
	char buf[5];
	tmpfile->read(buf, 3);
	buf[3] = '\0';
	if (!strncmp(buf, "dvx", 3))
		tmpfile = readFromX(tmpfile);
	else 
		tmpfile->seek(0);
			
	QBuffer * fileA = new QBuffer;
	fileA->open(QBuffer::ReadWrite);
	bool res = loadPage(tmpfile, fileA, -1, namedDest);
	tmpfile->close();
	delete tmpfile;
	if (!res)
	{
		fileA->close();
		delete fileA;
		return 0;
	}
		
	XWDVIDoc * docA = new XWDVIDoc;
	QByteArray fba = filename.toLocal8Bit();
	docA->fileName = new XWString(fba.data(), fba.size());
	if (!docA->load(fileA, 0, 0))
	{
		delete docA;
		docA = 0;
	}
		
	return docA;
}

DVIFontDef * XWDVIDoc::getFontDefs()
{
	if (!core)
		return 0;
		
	return core->getFontDefs();
}

int  XWDVIDoc::getNumberOfDefFont()
{
	if (!core)
		return 0;
		
	return core->getNumberOfDefFont();
}

bool XWDVIDoc::load(const QString & filename, QIODevice * outfileA)
{
	QIODevice * fileA = 0;
	int mpmode = 0;
	if (filename.endsWith(".mps", Qt::CaseInsensitive))
	{
		XWPictureSea sea;
		fileA = sea.openFile(filename, XWPictureSea::MP);
		if (!fileA)
			return false;
			
		mpmode = 1;
	}
	else
	{
		XWDocSea sea;
		fileA = sea.openDvi(filename);
		if (!fileA)
			return false;
			
		char buf[5];
		fileA->read(buf, 3);
		buf[3] = '\0';
		if (!strncmp(buf, "dvx", 3))
			fileA = readFromX(fileA);
		else
			fileA->seek(0);
	}
	
	QByteArray fba = filename.toLocal8Bit();
	fileName = new XWString(fba.data(), fba.size());
	
	return load(fileA, mpmode, outfileA);
}

bool XWDVIDoc::load(QIODevice * fileA, int mpmode, QIODevice * outfileA)
{
	if (!fileA)
		return false;
		
	if (file)
	{
		file->close();
		delete file;
	}
	
	ok = false;
	file = fileA;
	file->seek(0);
	
	if (outline)
		delete outline;
		
	if (optContent)
		delete optContent;
		
	if (catalog)
		delete catalog;
		
	if (core)
		delete core;
		
	if (xref)
		delete xref;
		
	if (!outfileA)
		xref = new XWDVIRef;
	else
		xref = new XWDVIRef(outfileA);
		
	core = new XWDVICore((XWDVIRef*)xref, mpmode);
	if (!mpmode)
	{
		long post_location = findPost();
  	if (post_location < 0)
  		return ok;
  			
  	if (!getDVIInfo(post_location))
  		return ok;
  			
  	if (!getPageInfo(post_location))
  		return ok;
  			
  	if (!getDVIFonts(post_location))
  		return ok;
	}
	
	if (!outfileA)
	{
		core->beginDoc();
		for (long i = 0; i < numPages; i++)
			doPage(i);
			
		core->endDoc(false);
		
		catalog = new XWCatalog(this);
		outline = new XWOutline(catalog->getOutline(), xref);
    optContent = new XWOptionalContent(this);
	}
	else
		core->initDev();
	
	ok = true;
	
	return ok;
}

void XWDVIDoc::loadFontMapFile(const char  *filename)
{
	core->loadFontMapFile(filename);
}

void XWDVIDoc::saveToPDF(QIODevice * outfileA,
	                     char *dviname, 
	                     char *pdfname,
	                     const QString & thumbbasename,
	                     const QList<int> & pages,
	                     char * owner_pw,
	                     char * user_pw)
{
	XWDVIEncrypt::computeIdString(dviname, pdfname);
		
	XWDocSetting setting;
	
	double page_width = setting.getPDFPaperWidth();
	double page_height = setting.getPDFPaperHeight();
  double x_offset = setting.getPDFHoff();
  double y_offset = setting.getPDFVoff(); 
  char   landscape = 0;
	unsigned minorversion = setting.getPDFVersion();
	int  do_enc =  setting.getPDFEncrypt() ? 1 : 0;
	unsigned  key_bits =  setting.getPDFKeyBits();
	unsigned permission = setting.getPDFPermission();
	
	XWDVIRef * dvixref = (XWDVIRef*)xref;
	XWDVIDoc * dvidoc = 0;
	file->seek(0);
	int lenA = (int)(file->size());
	uchar * buf = (uchar*)malloc((lenA + 1) * sizeof(uchar));
	file->read((char*)buf, lenA);	
	QBuffer * fileA = new QBuffer;
	fileA->open(QIODevice::WriteOnly);
	fileA->write((char*)buf, lenA);
	fileA->close();
	fileA->open(QIODevice::ReadOnly);
	free(buf);
	dvidoc = new XWDVIDoc;
	dvidoc->load(fileA, core->isMPs() ? 1 : 0, outfileA);	
	
	dvidoc->setMag(setting.getPDFMag());
	dvidoc->setDecimalDigits(setting.getPDFPrecision());
	dvidoc->setIgnoreColors(setting.getPDFIgnoreColors());
	dvidoc->setDPI(600, 600);	
	dvidoc->setAnnotGrow(setting.getPDFAnnotGrow());
	dvidoc->setCIDFontFixedPitch(setting.getPDFCIDFontFixedPitch());
	dvidoc->setFontMapFirstMatch(setting.getPDFMapFirst());
	dvidoc->setNoDestRemove(setting.getPDFNoDestRemove());
	dvidoc->setTPICTransFill(setting.getPDFTPICTransparent());
	dvidoc->setCompressionLevel(setting.getPDFCompressLevel());
	dvidoc->setThumbBaseName(thumbbasename);
	scanSpecials(0, 
	             &page_width, 
	             &page_height, 
	             &x_offset, 
	             &y_offset, 
	             &landscape, 
	             &minorversion, 
	             &do_enc, 
	             &key_bits, 
	             &permission, 
	              owner_pw,
	              user_pw);
	dvidoc->setEncryption(do_enc == 1 ? true : false);
	if (do_enc == 1)
	{
		if ((key_bits > 40 && minorversion < 4) || 
			  key_bits < 40 || key_bits > 128 || (key_bits & 0x7))
		{
			XWDVIEncrypt::setPasswd((unsigned)(key_bits), 
		                    		 (unsigned)(permission),
		                    		 0, 0);
		}
		else
		{
			XWDVIEncrypt::setPasswd((unsigned)(key_bits), 
		                    		(unsigned)(permission),
		                    		owner_pw, user_pw);
		}
	}
	
	if (landscape ==  1)
	{
		double t = page_width;
		page_width = page_height;
		page_height = t;
	}
	
	dvidoc->setPDFVersion(minorversion);
	dvidoc->setPaperSize(page_width, page_height);	
	dvidoc->setOffset(x_offset, y_offset);	
	
	char lm = landscape;
	double w = page_width;
	double h = page_height;
	double xoff = x_offset;
	double yoff = y_offset;
		
	for (int i = 0; i < pages.size(); i++)
	{
		int p = pages[i];
		scanSpecials(p, 
	             	&w, 
	             	&h, 
	             	&xoff, 
	            	&yoff, 
	             	&lm, 
	             	0, 
	             	0, 
	             	0, 
	             	0, 
	             	0,
	             	0);
	             	
		if (w != page_width || 
			 h != page_height || 
			 xoff != x_offset || 
			 yoff != y_offset || 
			 lm != landscape)
		{
			page_width = w;
			page_height = h;
			x_offset = xoff;
			y_offset = yoff;
			landscape = lm;
			
			if (lm ==  1)
			{
				double t = page_width;
				page_width = page_height;
				page_height = t;
			}
			
			dvidoc->setPaperSize(page_width, page_height);	
			dvidoc->setOffset(x_offset, y_offset);	
		}
		
		dvidoc->displayPage(p);
	}
	
#ifdef XW_BUILD_PS_LIB
  dvidoc->close(false);
  for (int i = 0; i < pages.size(); i++)
  {
  	int p = pages[i];
  	scanSpecials(p, 
	             	&w, 
	             	&h, 
	             	&xoff, 
	            	&yoff, 
	             	&lm, 
	             	0, 
	             	0, 
	             	0, 
	             	0, 
	             	0,
	             	0);
	             	
		if (w != page_width || 
			 h != page_height || 
			 xoff != x_offset || 
			 yoff != y_offset || 
			 lm != landscape)
		{
			page_width = w;
			page_height = h;
			x_offset = xoff;
			y_offset = yoff;
			landscape = lm;
			
			if (lm ==  1)
			{
				double t = page_width;
				page_width = page_height;
				page_height = t;
			}
			
			dvidoc->setPaperSize(page_width, page_height);	
			dvidoc->setOffset(x_offset, y_offset);	
		}
  	dvidoc->displayPage(p);
  }
#endif //XW_BUILD_PS_LIB
		
	dvidoc->close(true);
	delete dvidoc;
	core->resetFlags();
}

void XWDVIDoc::scanSpecials(int page_no, 
										        double *page_width, 
										        double *page_height,
                   	        double *x_offset, 
                   	        double *y_offset, 
                   	        char   *landscape,
	                          unsigned *minorversion,
		                        int *do_enc, 
		                        unsigned *key_bits, 
		                        unsigned *permission, 
		                        char *owner_pw, 
		                        char *user_pw)
{
  if (page_no >= numPages)
  {
  	QString msg = QString(tr("Invalid page number: %1")).arg(page_no);
    xwApp->error(msg);
    return ;
  }
  
  file->seek(pageLoc[page_no]);
  uchar opcode = 0;
  char sbuf[1024];
  while (file->getChar((char*)&opcode) && opcode != DVI_EOP)
  {
  	if (opcode < DVI_SET1 || (opcode >= DVI_FNTNUM0 && opcode < DVI_FNT1))
      continue;
    else if (opcode == DVI_XXX1 || opcode == DVI_XXX2 ||
             opcode == DVI_XXX3 || opcode == DVI_XXX4)
		{
			ulong  size = 0;
      char  buf[1024];
      switch (opcode) 
      {
      	case DVI_XXX1: 
      		size = getUnsignedByte(file);   
      		break;
      		
      	case DVI_XXX2: 
      		size = getUnsignedPair(file);   
      		break;
      		
      	case DVI_XXX3: 
      		size = getUnsignedTriple(file); 
      		break;
      		
      	case DVI_XXX4: 
      		size = getUnsignedQuad(file);   
      		break;
      }
      
      size = qMin(size, (ulong)(1024));
      if (file->read(buf, size) != size)
      {
      	xwApp->error(tr("Reading DVI file failed!"));
      	return ;
      }
      
      if (core->scanSpecial(page_width, page_height, x_offset, y_offset, landscape,
		                        minorversion, do_enc, key_bits, permission, 
		                        owner_pw, user_pw, buf, size))
		  {
		  	QString msg = QString(tr("Reading special command failed: \"%1\"")).arg(buf);
		  	xwApp->warning(msg);
		  }
		  
		  continue;
		}
		  
		switch (opcode)
		{
		 	case DVI_BOP:
     		file->read(sbuf, 44);
     		break;
      		
     	case DVI_NOP: 
     	case DVI_PUSH: 
     	case DVI_POP:
    	case DVI_W0: 
    	case DVI_X0: 
    	case DVI_Y0: 
    	case DVI_Z0:
     		break;
      		
    	case DVI_SET1: 
    	case DVI_PUT1: 
    	case DVI_RIGHT1:  
    	case DVI_DOWN1:
    	case DVI_W1: 
    	case DVI_X1: 
    	case DVI_Y1: 
    	case DVI_Z1: 
    	case DVI_FNT1:
     		getUnsignedByte(file);
     		break;

    	case DVI_SET2: 
    	case DVI_PUT2: 
    	case DVI_RIGHT2: 
    	case DVI_DOWN2:
    	case DVI_W2: 
    	case DVI_X2: 
    	case DVI_Y2: 
    	case DVI_Z2: 
    	case DVI_FNT2:
     		getSignedPair(file);
     		break;

    	case DVI_SET3: 
    	case DVI_PUT3: 
    	case DVI_RIGHT3: 
    	case DVI_DOWN3:
    	case DVI_W3: 
    	case DVI_X3: 
    	case DVI_Y3: 
    	case DVI_Z3: 
    	case DVI_FNT3:
     		getSignedTriple(file);
     		break;

    	case DVI_SET4: 
    	case DVI_PUT4: 
    	case DVI_RIGHT4: 
    	case DVI_DOWN4:
    	case DVI_W4: 
    	case DVI_X4: 
    	case DVI_Y4: 
    	case DVI_Z4: 
    	case DVI_FNT4:
     		getSignedQuad(file);
     		break;

    	case DVI_SETRULE: 
    	case DVI_PUTRULE:
     		file->read(sbuf, 8);
     		break;
      		
     	case DVI_FNTDEF1: 
     		doFntDef1(); 
     		break;
     		
    	case DVI_FNTDEF2: 
    		doFntDef2(); 
    		break;
    			
    	case DVI_FNTDEF3: 
    		doFntDef3(); 
    		break;
    			
    	case DVI_FNTDEF4: 
    		doFntDef4(); 
    		break;

    	case DVI_DIR:
     		getUnsignedByte(file);
     		break;

    	default: 
     		break;
		}
  }
}

void XWDVIDoc::setAnnotGrow(int annot_grow)
{
	XWDVIRef * dvixref = (XWDVIRef*)xref;
	dvixref->setAnnotGrow(annot_grow);
}

void XWDVIDoc::setBookMarkOpen(int i)
{
	XWDVIRef * dvixref = (XWDVIRef*)xref;
	dvixref->setBookMarkOpen(i);
}

void XWDVIDoc::setCIDFontFixedPitch(bool e)
{
	core->setCIDFontFixedPitch(e);
}

void XWDVIDoc::setCompressionLevel(int level)
{
	XWDVIRef * dvixref = (XWDVIRef*)xref;
	dvixref->setCompressionLevel(level);
}

void XWDVIDoc::setDecimalDigits(int i)
{
	core->setDecimalDigits(i);
}

void XWDVIDoc::setDPI(int hDPIA, int vDPIA)
{
	core->setDPI(hDPIA, vDPIA);
}

void XWDVIDoc::setEncryption(bool e)
{
	XWDVIRef * dvixref = (XWDVIRef*)xref;
	dvixref->setEncryption(e);
}

void XWDVIDoc::setFontMapFirstMatch(bool e)
{
	core->setFontMapFirstMatch(e);
}

void XWDVIDoc::setIgnoreColors(int i)
{
	core->setIgnoreColors(i);
}

void XWDVIDoc::setMag(double m)
{
	core->setMag(m);
}

void XWDVIDoc::setNoDestRemove(bool e)
{
	XWDVIRef * dvixref = (XWDVIRef*)xref;
	dvixref->setCheckGotos(e);
}

void XWDVIDoc::setOffset(double hoff, double voff)
{
	core->setOffset(hoff, voff);
}

void XWDVIDoc::setPaperSize(double w, double h)
{
	core->setPaperSize(w, h);
}

void XWDVIDoc::setPDFVersion(int v)
{
	XWDVIRef * dvixref = (XWDVIRef*)xref;
	dvixref->setVersion(v);
}

void XWDVIDoc::setThumbBaseName(const QString & thumbbasename)
{
	if (!thumbbasename.isEmpty())
	{
		XWDVIRef * dvixref = (XWDVIRef*)xref;
		QByteArray ba = thumbbasename.toLocal8Bit();
		dvixref->setThumbBaseName(ba.constData());
	}
}

void XWDVIDoc::setTPICTransFill(bool e)
{
	core->setTPICTransFill(e);
}

void XWDVIDoc::doBop()
{
	for (int i = 0; i < 10; i++)
		getSignedQuad(file);
		
	getSignedQuad(file);
	core->doBop();
}

void XWDVIDoc::doDir()
{
	core->doDir(getUnsignedByte(file));
}

void XWDVIDoc::doDown1()
{
	core->doDown(getSignedByte(file));
}

void XWDVIDoc::doDown2()
{
	core->doDown(getSignedPair(file));
}

void XWDVIDoc::doDown3()
{
	core->doDown(getSignedTriple(file));
}

void XWDVIDoc::doDown4()
{
	core->doDown(getSignedQuad(file));
}

void XWDVIDoc::doFnt1()
{
	long tex_id = getUnsignedByte(file);
  	core->doFnt(tex_id);
}

void XWDVIDoc::doFnt2()
{
	long tex_id = getUnsignedPair(file);
  	core->doFnt(tex_id);
}

void XWDVIDoc::doFnt3()
{
	long tex_id = getUnsignedTriple(file);
  	core->doFnt(tex_id);
}

void XWDVIDoc::doFnt4()
{
	long tex_id = getSignedQuad(file);
  	core->doFnt(tex_id);
}    

void XWDVIDoc::doFntDef()
{
	getSignedQuad(file);
  	getSignedQuad(file);
  	getSignedQuad(file);
  	int area_len = getUnsignedByte(file);
  	int name_len = getUnsignedByte(file);
  	for (int i = 0; i < area_len + name_len; i++) 
    	getUnsignedByte(file);
}

void XWDVIDoc::doFntDef1()
{
	getUnsignedByte(file);
  	doFntDef();
}

void XWDVIDoc::doFntDef2()
{
	getUnsignedPair(file);
  	doFntDef();
}

void XWDVIDoc::doFntDef3()
{
	getUnsignedTriple(file);
  	doFntDef();
}

void XWDVIDoc::doFntDef4()
{
	getSignedQuad(file);
  	doFntDef();
}

void XWDVIDoc::doNativeFontDef(QIODevice * fileA, int scanning)
{
	if (scanning)
	{
		getUnsignedQuad(fileA);
		ushort flags = getUnsignedPair(fileA);
		if ((flags & XDV_FLAG_FONTTYPE_ICU) || (flags & XDV_FLAG_FONTTYPE_ATSUI))
		{
			int name_length = (int) getUnsignedByte(fileA);
      name_length += (int) getUnsignedByte(fileA);
      name_length += (int) getUnsignedByte(fileA);
      
      for (int i = 0; i < name_length; ++i)
        getUnsignedByte(fileA);
        
      if (flags & XDV_FLAG_COLORED)
      	getUnsignedQuad(fileA);
      	
       if (flags & XDV_FLAG_VARIATIONS)
       {
       		int nvars = getUnsignedPair(fileA);
       		for (int i = 0; i < nvars * 2; ++i)
       			getUnsignedQuad(fileA);
      	}
		}
	}
}

void XWDVIDoc::doPage(int page_no)
{
  	file->seek(pageLoc[page_no]);
  	uchar opcode = 0;
  	uint  slen = 0;
  	uchar sbuf[1024];
  	for (;;)
  	{
  		slen  = 0;
    	while (file->getChar((char*)&opcode) && opcode < DVI_SET1 && slen < 1024) 
      		sbuf[slen++] = opcode;
      			
    	if (slen > 0) 
      		core->doString(sbuf, slen);
      			
    	if (slen == 1024)
      		continue;
      			
      	if (opcode >= DVI_FNTNUM0 && opcode < DVI_FNT1) 
      	{
      		core->doFnt(opcode - DVI_FNTNUM0);
      		continue;
    	}
      	
      	switch (opcode)
      	{
      		case DVI_SET1:
      			doSet1(); 
      			break;
      			
    		case DVI_SET2:
    			doSet2(); 
    			break;
    			
    		case DVI_SET3: 
      			doSet3();
      			break;
      			
    		case DVI_SET4:
      			doSet4(); 
      			break;
      			
      		case DVI_SETRULE:
      			doSetRule();
      			break;
      			
      		case DVI_PUT1: 
      			doPut1(); 
      			break;
      			
    		case DVI_PUT2:
    			doPut2(); 
    			break;
    			
    		case DVI_PUT3: 
    			doPut3(); 
    			break;
    			
    		case DVI_PUT4:
    			doPut4(); 
      			break;
      			
      		case DVI_PUTRULE:
      			doPutRule();
      			break;

    		case DVI_NOP:
      			break;

    		case DVI_BOP:
      			doBop();
      			break;
      			
    		case DVI_EOP:
      			core->doEop();
      			return;
      			break;
      			
      		case DVI_PUSH:
      			core->doPush();
      			core->doMarkDepth();
      			break;
      			
      		case DVI_POP:
      			core->doPop();
      			core->doMarkDepth();
      			break;
      			
      		case DVI_RIGHT1: 
      			doRight1(); 
      			break;
      			
    		case DVI_RIGHT2: 
    			doRight2(); 
    			break;
    			
    		case DVI_RIGHT3: 
    			doRight3(); 
    			break;
    			
    		case DVI_RIGHT4: 
    			doRight4(); 
    			break;
    			
    		case DVI_W0: 
    			core->doW0(); 
    			break;
    			
    		case DVI_W1: 
    			doW1(); 
    			break;
    			
    		case DVI_W2: 
    			doW2(); 
    			break;
    			
    		case DVI_W3: 
    			doW3(); 
    			break;
    			
    		case DVI_W4: 
    			doW4(); 
    			break;

    		case DVI_X0: 
    			core->doX0(); 
    			break;
    			
    		case DVI_X1: 
    			doX1(); 
    			break;
    			
    		case DVI_X2: 
    			doX2(); 
    			break;
    			
    		case DVI_X3: 
    			doX3(); 
    			break;
    			
    		case DVI_X4: 
    			doX4(); 
    			break;
    			

    		case DVI_DOWN1: 
    			doDown1(); 
    			break;
    			
    		case DVI_DOWN2: 
    			doDown2(); 
    			break;
    			
    		case DVI_DOWN3: 
    			doDown3(); 
    			break;
    			
    		case DVI_DOWN4: 
    			doDown4(); 
    			break;
    			
    		case DVI_Y0: 
    			core->doY0(); 
    			break;
    			
    		case DVI_Y1: 
    			doY1(); 
    			break;
    			
    		case DVI_Y2: 
    			doY2(); 
    			break;
    			
    		case DVI_Y3: 
    			doY3(); 
    			break;
    			
    		case DVI_Y4: 
    			doY4(); 
    			break;

    		case DVI_Z0: 
    			core->doZ0(); 
    			break;
    			
    		case DVI_Z1: 
    			doZ1(); 
    			break;
    			
    		case DVI_Z2: 
    			doZ2(); 
    			break;
    			
    		case DVI_Z3: 
    			doZ3(); 
    			break;
    			
    		case DVI_Z4: 
    			doZ4(); 
    			break;

    		case DVI_FNT1: 
    			doFnt1(); 
    			break;
    			
    		case DVI_FNT2: 
    			doFnt2(); 
    			break;
    			
    		case DVI_FNT3: 
    			doFnt3(); 
    			break;
    			
    		case DVI_FNT4: 
    			doFnt4(); 
    			break;

      		/* Specials */
    		case DVI_XXX1: 
    			doXXX1(); 
    			break;
    			
    		case DVI_XXX2: 
    			doXXX2(); 
    			break;
    			
    		case DVI_XXX3: 
    			doXXX3(); 
    			break;
    			
    		case DVI_XXX4: 
    			doXXX4(); 
    			break;
    			
    		case DVI_FNTDEF1: 
    			doFntDef1(); 
    			break;
    			
    		case DVI_FNTDEF2: 
    			doFntDef2(); 
    			break;
    			
    		case DVI_FNTDEF3: 
    			doFntDef3(); 
    			break;
    			
    		case DVI_FNTDEF4: 
    			doFntDef4(); 
    			break;
    			
    		case DVI_DIR:
      			doDir();
      			break;

    		case DVI_PRE: 
    		case DVI_POST: 
    		case DVI_POSTPOST:
    			return ;
      			break;
      			
      	case XDV_GLYPH_STRING:
      		core->doGlyphArray(file, 0);
      		break;
      		
      	case XDV_GLYPH_ARRAY:
      		core->doGlyphArray(file, 1);
      		break;
      		
      	case XDV_NATIVE_FONT_DEF:
      		doNativeFontDef(file, 0);
      		break;
      		
      	case XDV_PIC_FILE:
      		core->doPicFile(file);
      		break;
      			
    		default:
      			return ;
      			break;
      	}
  	}
}

void XWDVIDoc::doPut1()
{
	core->doPut(getUnsignedByte(file));
}

void XWDVIDoc::doPut2()
{
	core->doPut(getUnsignedPair(file));
}

void XWDVIDoc::doPut3()
{
	core->doPut(getUnsignedTriple(file));
}

void XWDVIDoc::doPut4()
{
	core->doPut(getUnsignedQuad(file));
}

void XWDVIDoc::doPutRule()
{
	long height = getSignedQuad(file);
  	long width  = getSignedQuad(file);
  	if (width > 0 && height > 0) 
    	core->doRule(width, height);
}

void XWDVIDoc::doRight1()
{
	core->doRight(getSignedByte(file));
}

void XWDVIDoc::doRight2()
{
	core->doRight(getSignedPair(file));
}

void XWDVIDoc::doRight3()
{
	core->doRight(getSignedTriple(file));
}

void XWDVIDoc::doRight4()
{
	core->doRight(getSignedQuad(file));
}
    
void XWDVIDoc::doSet1()
{
	core->doSet(getUnsignedByte(file));
}

void XWDVIDoc::doSet2()
{
	core->doSet(getUnsignedPair(file));
}

void XWDVIDoc::doSet3()
{
	core->doSet(getUnsignedTriple(file));
}

void XWDVIDoc::doSet4()
{
	core->doSet(getUnsignedQuad(file));
}

void XWDVIDoc::doSetRule()
{
	long height = getSignedQuad(file);
  	long width  = getSignedQuad(file);
  	if (width > 0 && height > 0) 
    	core->doRule(width, height);
  	core->doRight(width);
}

void XWDVIDoc::doW1()
{
	core->doW(getSignedByte(file));
}

void XWDVIDoc::doW2()
{
	core->doW(getSignedPair(file));
}

void XWDVIDoc::doW3()
{
	core->doW(getSignedTriple(file));
}

void XWDVIDoc::doW4()
{
	core->doW(getSignedQuad(file));
}

void XWDVIDoc::doX1()
{
	core->doX(getSignedByte(file));
}

void XWDVIDoc::doX2()
{
	core->doX(getSignedPair(file));
}

void XWDVIDoc::doX3()
{
	core->doX(getSignedTriple(file));
}

void XWDVIDoc::doX4()
{
	core->doX(getSignedQuad(file));
}

void XWDVIDoc::doXXX(ulong size)
{
	uchar * buffer = new uchar[size+1];
  	for (ulong i = 0; i < size; i++) 
    	buffer[i] = getUnsignedByte(file);
    	
  	core->doSpecial((char*)buffer, size);
  	delete [] buffer;
}

void XWDVIDoc::doXXX1()
{
	long size = getUnsignedByte(file);
  	doXXX(size);
}

void XWDVIDoc::doXXX2()
{
	long size = getUnsignedPair(file);
  	doXXX(size);
}

void XWDVIDoc::doXXX3()
{
	long size = getUnsignedTriple(file);
  	doXXX(size);
}

void XWDVIDoc::doXXX4()
{
	ulong size = getUnsignedQuad(file);
  	doXXX(size);
}
    
void XWDVIDoc::doY1()
{
	core->doY(getSignedByte(file));
}

void XWDVIDoc::doY2()
{
	core->doY(getSignedPair(file));
}

void XWDVIDoc::doY3()
{
	core->doY(getSignedTriple(file));
}

void XWDVIDoc::doY4()
{
	core->doY(getSignedQuad(file));
}

void XWDVIDoc::doZ1()
{
	core->doZ(getSignedByte(file));
}

void XWDVIDoc::doZ2()
{
	core->doZ(getSignedPair(file));
}

void XWDVIDoc::doZ3()
{
	core->doZ(getSignedTriple(file));
}

void XWDVIDoc::doZ4()
{
	core->doZ(getSignedQuad(file));
}

long XWDVIDoc::findPost()
{
	ulong  current = (ulong)(file->size());
	if (current == 0)
		return -1;
		
	ulong filesize = current;
	uchar ch = 0;	
	do 
	{
    	current--;
    	file->seek(current);
  	} while (file->getChar((char*)&ch) && (ch == DVI_PADDING) && current > 0);
  	
  	if (filesize - current < 4 || 
  		  current == 0 || 
  		  !(ch == DVI_ID_BYTE || ch == DVI_V_ID || ch == XDVI_ID)) 
    {
    	QString msg = QString(tr("DVI ID = %1.\n")).arg(ch);
    	xwApp->error(msg);
    	return -1;
  	} 
  	
  	current = current - 5;
  	file->seek(current);
  	
  	if (!file->getChar((char*)&ch) || (ch != DVI_POSTPOST)) 
  	{
  		QString msg = QString(tr("found %1 where post_post opcode should be.\n")).arg(ch);
  		xwApp->error(msg);
    	return -1;
  	}
  	
  	current = getSignedQuad(file);
  	file->seek(current);
  	if (!file->getChar((char*)&ch) || (ch  != DVI_POST)) 
  	{
  		QString msg = QString(tr("found %1 where post opcode should be.\n")).arg(ch);
    	xwApp->error(msg);
    	return -1;
  	}

  	return (long)current;
}

bool XWDVIDoc::getDVIFonts(long post_location)
{
	int  code = 0;
  	long tex_id = 0;
  	file->seek(post_location + 29);
  	while ((code = getUnsignedByte(file)) != DVI_POSTPOST)
  	{
  		switch (code) 
  		{
    		case DVI_FNTDEF1:
      			tex_id = getUnsignedByte(file);
      			break;
      			
    		case DVI_FNTDEF2:
      			tex_id = getUnsignedPair(file);
      			break;
      			
    		case DVI_FNTDEF3:
      			tex_id = getUnsignedTriple(file);
      			break;
      			
    		case DVI_FNTDEF4:
      			tex_id = getSignedQuad(file);
      			break;
      			
      	case XDV_NATIVE_FONT_DEF:
      		tex_id = getSignedQuad(file);
      		break;
      			
    		default:
      			return false;
      			break;
    	}
    	
    	if (code != XDV_NATIVE_FONT_DEF)
    	{
    		if (!readFontRecord(tex_id))
    			return false;
    	}
    	else
    	{
    		if (!readNativeFontRecord(tex_id))
    			return false;
    	}    	
  	}
  	
  	return true;
}

bool XWDVIDoc::getDVIInfo(long post_location)
{
	DVIHeader * header = core->getHeader();
	file->seek(14);
	int length = getUnsignedByte(file);
  if (file->read(header->comment, length) != length) 
		return false;
  	
  header->comment[length] = '\0';	
	file->seek(post_location + 5);
	
  header->unitNum = getUnsignedQuad(file);
  header->unitDen = getUnsignedQuad(file);
  header->mag      = getUnsignedQuad(file);
  header->mediaHeight = getUnsignedQuad(file);
  header->mediaWidth  = getUnsignedQuad(file);
  header->stackDepth = getUnsignedPair(file);
  	
  if (header->stackDepth > DVI_STACK_DEPTH_MAX) 
  {
  	QString msg = QString(tr("need stack depth of %1, "
  	                         "but DVI_STACK_DEPTH_MAX is %2."
  	                         "capacity exceeded.\n"))
  	                         .arg(header->stackDepth).arg(DVI_STACK_DEPTH_MAX);
  	xwApp->error(msg);
   	return false;
  }
  
  return true;
}

bool XWDVIDoc::getPageInfo(long post_location)
{
	file->seek(post_location + 27);
  	numPages = getUnsignedPair(file);
  	if (numPages == 0) 
  		return false;
  		
  	pageLoc = (ulong * )malloc(numPages * sizeof(ulong));
  	file->seek(post_location + 1);
  	pageLoc[numPages - 1] = getUnsignedQuad(file);
  	if ((pageLoc[numPages - 1] + 41) > file->size())
  		return false;
  		
  	for (int i = numPages - 2; i >= 0; i--) 
  	{
    	file->seek(pageLoc[i + 1] + 41);
    	pageLoc[i] = getUnsignedQuad(file);
    	if ((pageLoc[numPages-1] + 41) > file->size())
    		return false;
  	}
  	
  	return true;
}

bool XWDVIDoc::loadPage(QIODevice * in, 
                        QIODevice * out, 
                        int pg, 
                        XWString *namedDest)
{
	if (in->atEnd() || in->size() < 14)
		return false;
		
	in->seek(0);
	char cbuf[257];
	in->read(cbuf, 14);
	out->write(cbuf, 14);
	if (in->atEnd())
		return false;

	int length = getUnsignedByte(in);
  	if (in->read(cbuf, length) != length) 
		return false;
	
	out->write(cbuf, length);
	long obop = (long)(out->pos());
	uchar opcode = DVI_BOP;
	out->write((char*)&opcode, 1);
	
	bool found = false;
	int i = 0;	
	ulong boppos = 0;
	ulong spos = 0;
	ulong pos = 0;
	int area_len = 0;
	int name_len = 0;
	ulong size = 0;
	char * buf = 0;
	int infox = 0;
	while (!in->atEnd() && !found)
	{
		pos = (ulong)(in->pos());
		if (!in->getChar((char*)&opcode))
			break;
			
		if (opcode == DVI_BOP)
		{			
			boppos = pos;
		
			if (!namedDest && i == pg)
				found = true;
				
			i++;
			continue;
		}
		
		size = 0;
		spos = 0;
		switch (opcode)
		{
			case DVI_SET1:
			case DVI_PUT1: 
			case DVI_RIGHT1: 
      		case DVI_W1: 
      		case DVI_X1: 
      		case DVI_DOWN1: 
      		case DVI_Y1: 
      		case DVI_Z1: 
      		case DVI_FNT1: 
      		case DVI_DIR:
				in->read(cbuf, 1);
      			break;
      			
    		case DVI_SET2:
    		case DVI_PUT2:
    		case DVI_RIGHT2: 
    		case DVI_W2: 
    		case DVI_X2: 
    		case DVI_DOWN2: 
    		case DVI_Y2: 
    		case DVI_Z2: 
    		case DVI_FNT2: 
    			in->read(cbuf, 2);
    			break;
    			
    		case DVI_SET3: 
    		case DVI_PUT3: 
    		case DVI_RIGHT3: 
    		case DVI_W3: 
    		case DVI_X3: 
    		case DVI_DOWN3: 
    		case DVI_Y3: 
    		case DVI_Z3: 
    		case DVI_FNT3: 
    			in->read(cbuf, 3);
      			break;
      			
    		case DVI_SET4:
    		case DVI_PUT4:
    		case DVI_RIGHT4: 
    		case DVI_W4: 
    		case DVI_X4: 
    		case DVI_DOWN4: 
    		case DVI_Y4: 
    		case DVI_Z4: 
    		case DVI_FNT4: 
    			in->read(cbuf, 4);
      			break;
      			
      		case DVI_SETRULE:
      		case DVI_PUTRULE:
      			in->read(cbuf, 8);
      			break;
    		      			  
    		case DVI_FNTDEF1: 
    			in->read(cbuf, 13);
    			area_len = getUnsignedByte(in);
    			name_len = getUnsignedByte(in) + area_len;
    			if (name_len > 0)
    			{
    				buf = new char[name_len + 1];
    				in->read(buf, name_len);
    				delete [] buf;
    			}
    			break;
    			
    		case DVI_FNTDEF2: 
    			in->read(cbuf, 14);
    			area_len = getUnsignedByte(in);
    			name_len = getUnsignedByte(in) + area_len;
    			if (name_len > 0)
    			{
    				buf = new char[name_len + 1];
    				in->read(buf, name_len);
    				delete [] buf;
    			} 
    			break;
    			
    		case DVI_FNTDEF3: 
    			in->read(cbuf, 15);
    			area_len = getUnsignedByte(in);
    			name_len = getUnsignedByte(in) + area_len;
    			if (name_len > 0)
    			{
    				buf = new char[name_len + 1];
    				in->read(buf, name_len);
    				delete [] buf;
    			} 
    			break;
    			
    		case DVI_FNTDEF4: 
    			in->read(cbuf, 16);
    			area_len = getUnsignedByte(in);
    			name_len = getUnsignedByte(in) + area_len;
    			if (name_len > 0)
    			{
    				buf = new char[name_len + 1];
    				in->read(buf, name_len);
    				delete [] buf;
    			} 
    			break;
    			
    		case DVI_PRE: 
    		case DVI_POST: 
    		case DVI_POSTPOST:
    			return false;
      			break;

      		/* Specials */
    		case DVI_XXX1: 
    			spos = (ulong)(in->pos());
    			infox = 1;
    			size = getUnsignedByte(in);
    			break;
    			
    		case DVI_XXX2: 
    			spos = (ulong)(in->pos());
    			infox = 2;
    			size = getUnsignedPair(in);
    			break;
    			
    		case DVI_XXX3: 
    			spos = (ulong)(in->pos());
    			infox = 3;
    			size = getUnsignedTriple(in);
    			break;
    			
    		case DVI_XXX4: 
    			spos = (ulong)(in->pos());
    			infox = 4;
    			size = getUnsignedQuad(in);
    			break;
    			
    		case XDV_GLYPH_STRING:
    			getUnsignedQuad(in);
    			size = getUnsignedPair(in);
    			size *= 6;
    			buf = new char[size + 1];
    			in->read(buf, size);
    			delete [] buf;
    			size = 0;
    			break;
    			
    		case XDV_GLYPH_ARRAY:
    			getUnsignedQuad(in);
    			size = getUnsignedPair(in);
    			size *= 10;
    			buf = new char[size + 1];
    			in->read(buf, size);
    			delete [] buf;
    			size = 0;
    			break;
    			
    		case XDV_NATIVE_FONT_DEF:
    			doNativeFontDef(in, 1);
    			break;
    			
    		case XDV_PIC_FILE:
    			size = 1 + 4 * 6 + 2;
    			buf = new char[size + 1];
    			in->read(buf, size);
    			delete [] buf;
    			size = getUnsignedPair(in);
    			buf = new char[size + 1];
    			in->read(buf, size);
    			delete [] buf;
    			break;
    			
    		default:
      			break;
		}
		
		if (size > 0)
		{
			buf = new char[size + 1];
			in->read(buf, size);
			XWLexer lexerA(buf, size);
			const char * p = lexerA.skipWhite();
			if (!strncmp(p, "pdf:", 4))
			{
				p = lexerA.skip(4);
				int typeA = XWPDFSpecial::check(&lexerA);
				if (typeA == 6)
				{
					out->write((char*)&opcode, 1);
					in->seek(spos);
					in->read(cbuf, infox);
					out->write(cbuf, infox);
					out->write(buf, size);
				}
				else
				{
					if (namedDest && typeA == 4)
					{
						p = lexerA.skipWhite();
						XWObject nameObj;
						XWDVIParser parser(0, &lexerA);
						if (parser.parsePDFObject(&nameObj))
						{
							if (nameObj.isString())
							{
								if (!namedDest->cmp(nameObj.getString()))
									found = true;
							}
						}
						
						nameObj.free();
					}
				}
				
			}
							
			delete [] buf;
			buf = 0;
		}
	}
	
	if (!found)
		return false;
	
	in->seek(boppos);
	while (!in->atEnd())
	{
		in->getChar((char*)&opcode);
		out->write((char*)&opcode, 1);
		if (opcode == DVI_EOP)
			break;
			
		spos = 0;
		size = 0;
		switch (opcode)
		{
			case DVI_SET1:
			case DVI_PUT1: 
			case DVI_RIGHT1: 
      		case DVI_W1: 
      		case DVI_X1: 
      		case DVI_DOWN1: 
      		case DVI_Y1: 
      		case DVI_Z1: 
      		case DVI_FNT1: 
      		case DVI_DIR:
				in->read(cbuf, 1);
				out->write(cbuf, 1);
      			break;
      			
    		case DVI_SET2:
    		case DVI_PUT2:
    		case DVI_RIGHT2: 
    		case DVI_W2: 
    		case DVI_X2: 
    		case DVI_DOWN2: 
    		case DVI_Y2: 
    		case DVI_Z2: 
    		case DVI_FNT2: 
    			in->read(cbuf, 2);
    			out->write(cbuf, 2);
    			break;
    			
    		case DVI_SET3: 
    		case DVI_PUT3: 
    		case DVI_RIGHT3: 
    		case DVI_W3: 
    		case DVI_X3: 
    		case DVI_DOWN3: 
    		case DVI_Y3: 
    		case DVI_Z3: 
    		case DVI_FNT3: 
    			in->read(cbuf, 3);
    			out->write(cbuf, 3);
      			break;
      			
    		case DVI_SET4:
    		case DVI_PUT4:
    		case DVI_RIGHT4: 
    		case DVI_W4: 
    		case DVI_X4: 
    		case DVI_DOWN4: 
    		case DVI_Y4: 
    		case DVI_Z4: 
    		case DVI_FNT4: 
    			in->read(cbuf, 4);
    			out->write(cbuf, 4);
      			break;
      			
      		case DVI_SETRULE:
      		case DVI_PUTRULE:
      			in->read(cbuf, 8);
      			out->write(cbuf, 8);
      			break;
    		      			  
    		case DVI_FNTDEF1: 
    			in->read(cbuf, 13);
    			area_len = getUnsignedByte(in);
    			name_len = getUnsignedByte(in) + area_len;
    			if (name_len > 0)
    			{
    				buf = new char[name_len + 1];
    				in->read(buf, name_len);
    				delete [] buf;
    			}
    			break;
    			
    		case DVI_FNTDEF2: 
    			in->read(cbuf, 14);
    			area_len = getUnsignedByte(in);
    			name_len = getUnsignedByte(in) + area_len;
    			if (name_len > 0)
    			{
    				buf = new char[name_len + 1];
    				in->read(buf, name_len);
    				delete [] buf;
    			} 
    			break;
    			
    		case DVI_FNTDEF3: 
    			in->read(cbuf, 15);
    			area_len = getUnsignedByte(in);
    			name_len = getUnsignedByte(in) + area_len;
    			if (name_len > 0)
    			{
    				buf = new char[name_len + 1];
    				in->read(buf, name_len);
    				delete [] buf;
    			} 
    			break;
    			
    		case DVI_FNTDEF4: 
    			in->read(cbuf, 16);
    			area_len = getUnsignedByte(in);
    			name_len = getUnsignedByte(in) + area_len;
    			if (name_len > 0)
    			{
    				buf = new char[name_len + 1];
    				in->read(buf, name_len);
    				delete [] buf;
    			} 
    			break;

      		/* Specials */
    		case DVI_XXX1: 
    			spos = (ulong)(in->pos());
    			infox = 1;
    			size = getUnsignedByte(in);
    			break;
    			
    		case DVI_XXX2: 
    			spos = (ulong)(in->pos());
    			infox = 2;
    			size = getUnsignedPair(in);
    			break;
    			
    		case DVI_XXX3: 
    			spos = (ulong)(in->pos());
    			infox = 3;
    			size = getUnsignedTriple(in);
    			break;
    			
    		case DVI_XXX4: 
    			spos = (ulong)(in->pos());
    			infox = 4;
    			size = getUnsignedQuad(in);
    			break;
    			
    		default:
      			break;
		}
		
		if (size > 0)
		{
			in->seek(spos);
			in->read(cbuf, infox);
			out->write(cbuf, infox);
			buf = new char[size + 1];
			in->read(buf, size);
			out->write(buf, size);
			delete [] buf;
		}
	}
	
	ulong filesize = ulong(in->size());
	long current = (long)filesize;
	do 
	{
    	current--;
    	in->seek(current);
  	} while (in->getChar((char*)&opcode) && (opcode == DVI_PADDING) && current > 0);
  	
  	if (filesize - current < 4 || 
  		current == 0 || 
  		!(opcode == DVI_ID_BYTE || 
  		  opcode == DVI_V_ID ||
  		  opcode == XDVI_ID)) 
    {
    	return false;
  	} 
  	
  	current = current - 5;
  	in->seek(current);
  	
  	if (!in->getChar((char*)&opcode) || (opcode != DVI_POSTPOST))
    	return false;
  	
  	current = getSignedQuad(in);
  	in->seek(current);
  	if (!in->getChar((char*)&opcode) || (opcode  != DVI_POST)) 
    	return false;

  	current--;
  	in->seek(current);
  	in->getChar((char*)&opcode);
  	in->read(cbuf, 4);
  	long postpos = (long)(out->pos());
  	out->write((char*)&opcode, 1);
  	out->putChar((char)(obop / 16777216));
    out->putChar((char)((obop % 16777216) / 65536));
    out->putChar((char)((obop % 65536) / 256));
    out->putChar((char)(obop % 256));
    
    in->read(cbuf, 22);
    out->write(cbuf, 22);
    out->putChar((char)(1 >> 8));
    out->putChar((char)(1 & 0xFF));
    in->read(cbuf, 2);
  	
  	while (!in->atEnd() && opcode != DVI_POSTPOST)
  	{
  		in->getChar((char*)&opcode);  		
			out->write((char*)&opcode, 1);
			switch (opcode)
			{
				case DVI_FNTDEF1: 
    			in->read(cbuf, 13);
    			obop = (long)(in->pos());
    			out->write(cbuf, 13);
    			area_len = getUnsignedByte(in);
    			name_len = getUnsignedByte(in) + area_len;
    			in->seek(obop);
    			in->read(cbuf, 2);
    			out->write(cbuf, 2);
    			if (name_len > 0)
    			{
    				buf = new char[name_len + 1];
    				in->read(buf, name_len);
    				out->write(buf, name_len);
    				delete [] buf;
    			}
    			break;
    			
    		case DVI_FNTDEF2: 
    			in->read(cbuf, 14);
    			obop = (long)(in->pos());
    			out->write(cbuf, 14);
    			area_len = getUnsignedByte(in);
    			name_len = getUnsignedByte(in) + area_len;
    			in->seek(obop);
    			in->read(cbuf, 2);
    			out->write(cbuf, 2);
    			if (name_len > 0)
    			{
    				buf = new char[name_len + 1];
    				in->read(buf, name_len);
    				out->write(buf, name_len);
    				delete [] buf;
    			} 
    			break;
    			
    		case DVI_FNTDEF3: 
    			in->read(cbuf, 15);
    			obop = (long)(in->pos());
    			out->write(cbuf, 15);
    			area_len = getUnsignedByte(in);
    			name_len = getUnsignedByte(in) + area_len;
    			in->seek(obop);
    			in->read(cbuf, 2);
    			if (name_len > 0)
    			{
    				buf = new char[name_len + 1];
    				in->read(buf, name_len);
    				out->write(buf, name_len);
    				delete [] buf;
    			} 
    			break;
    			
    		case DVI_FNTDEF4: 
    			in->read(cbuf, 16);
    			obop = (long)(in->pos());
    			out->write(cbuf, 16);
    			area_len = getUnsignedByte(in);
    			name_len = getUnsignedByte(in) + area_len;
    			in->seek(obop);
    			in->read(cbuf, 2);
    			if (name_len > 0)
    			{
    				buf = new char[name_len + 1];
    				in->read(buf, name_len);
    				out->write(buf, name_len);
    				delete [] buf;
    			} 
    			break;
    			
    		default:
    			break;
			}
  	}
  	
  	in->read(cbuf, 4);
  	out->putChar((char)(postpos / 16777216));
    out->putChar((char)((postpos % 16777216) / 65536));
    out->putChar((char)((postpos % 65536) / 256));
    out->putChar((char)(postpos % 256));
    in->getChar((char*)&opcode);
		out->write((char*)&opcode, 1);
  	
  	long k = (long)(out->size()) % 4;
  	while (k > 0)
  	{
  		opcode = DVI_PADDING;
  		out->write((char*)&opcode, 1);
  		k--;
  	}
  	
  	return true;
}

bool XWDVIDoc::readFontRecord(long tex_id)
{
	DVIFontDef * font = core->getFontDef();
  	
  	getUnsignedQuad(file);
  	ulong point_size  = getUnsignedQuad(file);
  	ulong design_size = getUnsignedQuad(file);
  	int dir_length  = getUnsignedByte(file);
  	int name_length = getUnsignedByte(file);
  	
  	char * directory   = new char[dir_length + 1];
  	if (file->read(directory, dir_length) != dir_length) 
  	{
  		delete [] directory;
  		directory = 0;
		return false;
  	}
  	
  	directory[dir_length] = '\0';
  	delete [] directory;

  	char * font_name   = new char[name_length + 1];
  	if (file->read(font_name, name_length) != name_length) 
  	{
  		delete [] font_name;
  		font_name = 0;
		return false;
  	}
  	
  	font_name[name_length] = '\0';
  	font->texId      = tex_id;
  	font->fontName   = font_name;
  	font->pointSize  = point_size;
  	font->designSize = design_size;
  	font->used       = 0;
  	font->familyName = 0;
  	font->styleName = 0;
  	font->rgbaColor = 0xffffffff;
  	font->layoutDir = 0;

  	return true;
}

bool XWDVIDoc::readNativeFontRecord(long tex_id)
{
	DVIFontDef * font = core->getFontDef();
	ulong  point_size  = getUnsignedQuad(file);
  ushort flags       = getUnsignedPair(file);
  if ((flags & XDV_FLAG_FONTTYPE_ICU) || (flags & XDV_FLAG_FONTTYPE_ATSUI))
  {
  	int plen = getUnsignedByte(file);
  	int flen = getUnsignedByte(file);
  	int slen = getUnsignedByte(file);
  	char * font_name = new char[plen + 1];
  	if (file->read(font_name, plen) != plen)
  	{
  		delete [] font_name;
  		return false;
  	}
  	
  	font_name[plen] = '\0';
  	char * fam_name = new char[flen + 1];
  	if (file->read(fam_name, flen) != flen)
  	{
  		delete [] fam_name;
  		return false;
  	}
  	
  	fam_name[flen] = '\0';
    char * sty_name = new char[slen + 1];
    if (file->read(sty_name, slen) != slen)
  	{
  		delete [] sty_name;
  		return false;
  	}
  	
  	sty_name[slen] = '\0';
  	font->texId      = tex_id;
  	font->fontName   = font_name;
  	font->pointSize  = point_size;
  	font->designSize = 655360;
  	font->used       = 0;
  	font->familyName = fam_name;
  	font->styleName = sty_name;
  	if (flags & XDV_FLAG_VERTICAL)
  		font->layoutDir = 1;
  	else
  		font->layoutDir = 0;
  		
  	if (flags & XDV_FLAG_COLORED)
  		font->rgbaColor = getUnsignedQuad(file);
  	else
  		font->rgbaColor = 0xffffffff;
  		
  	if (flags & XDV_FLAG_VARIATIONS)
  	{
  		int nvars = getUnsignedPair(file);
  		for (int i = 0; i < nvars * 2; i++)
  			getUnsignedQuad(file);
  	}
  }
  
  return true;
}

QIODevice * XWDVIDoc::readFromX(QIODevice * fileA)
{
	if (!fileA)
		return 0;
		
	QDataStream in(fileA);
	int len = 0;
	in >> len;
	char * data = new char[len + 1];
	if (in.readRawData(data, len) != len)
	{
		delete [] data;
		return 0;
	}
	
	QByteArray uba(data, len);	
	delete [] data;
	
	in >> len;
	data = new char[len + 1];
	if (in.readRawData(data, len) != len)
	{
		delete [] data;
		return 0;
	}
	
	QByteArray pba(data, len);	
	delete [] data;
		
	in >> len;
	data = new char[len + 1];
	if (in.readRawData(data, len) != len)
	{
		delete [] data;
		return 0;
	}
	
	QByteArray dba(data, len);
	delete [] data;
	
	in >> len; 
	data = new char[len + 1];
	if (in.readRawData(data, len) != len)
	{
		delete [] data;
		return 0;
	}
	
	QByteArray dchecksumba(data, len);
	delete [] data;
	
	QByteArray keyba;
	len = 3;
	QString msg = tr("Please input user name and passward for Encrypted dvi file!");
    QString user;
    QString passwd;
    XWAes aes;	
	while (len > 0)
	{
		if (!xwApp->getUserAndPassward(msg, user, passwd))
			return 0;
			
		QByteArray tuba = user.toAscii();
		QByteArray tpba = passwd.toAscii();
		QByteArray ttpba = tpba;
		QCryptographicHash uhash(QCryptographicHash::Md5);
		uhash.addData(tuba);
		tuba = uhash.result();
		QCryptographicHash phash(QCryptographicHash::Md5);
		phash.addData(tpba);
		tpba = phash.result();
		if (tuba == uba && tpba == pba)
		{
			keyba = aes.decrypt128(dba, ttpba);
			QCryptographicHash khash(QCryptographicHash::Md5);
			khash.addData(keyba);
			QByteArray tmpchecksum = khash.result();
			if (dchecksumba == tmpchecksum)
				break;
		}
			
		len--;
	}
	
	if (len <= 0)
		return 0;
	
	QBuffer * tmpfile = new QBuffer;
	tmpfile->open(QIODevice::WriteOnly);	
	while (!in.atEnd())
	{
		in >> len;
		data = new char[len + 1];
		if (len != in.readRawData(data, len))
		{
			delete [] data;
			tmpfile->close();
			delete tmpfile;
			fileA->close();
			delete fileA;
			return 0;
		}
				
		QByteArray ba(data, len);
		delete [] data;
		QByteArray ba2 = aes.decrypt128(ba, keyba);
		tmpfile->write(ba2);
	}
			
	fileA->close();
	delete fileA;
	tmpfile->close();
	tmpfile->open(QBuffer::ReadOnly);
	return tmpfile;
}
