/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <QBuffer>
#include <QCoreApplication>
#include <QFile>
#include "XWApplication.h"
#include "XWConst.h"
#include "XWUtil.h"
#include "XWNumberUtil.h"
#include "XWFileName.h"
#include "XWDocSea.h"
#include "XWDocSetting.h"
#include "XWDoc.h"
#include "XWDVIDoc.h"
#include "XWOptionalContent.h"
#include "XWOutline.h"
#include "XWDVIEncrypt.h"
#include "XWDVIRef.h"
#include "XWDVICore.h"
#include "XWTeXGuiDev.h"

XWTeXGuiDev::XWTeXGuiDev(XWDoc * docA,
	                       QObject * parent)
	:XWTeXIODev(parent),
	 doc(docA)
{
	dviCore = 0;
	pageLoc = 0;
	numPages = 0;
	mainFile = 0;
	curFile = 0;
	
	setIniVersion(false);
}

XWTeXGuiDev::~XWTeXGuiDev()
{
	if (dviCore)
		delete dviCore;
		
	if (pageLoc)
		free(pageLoc);
			
	if (mainFile)
		delete mainFile;
}

bool XWTeXGuiDev::atEnd()
{
	return (!curFile || curFile->atEnd());
}

void XWTeXGuiDev::compile()
{
	numPages = 0;
	if (pageLoc)
	{
		free(pageLoc);
		pageLoc = 0;
	}
	
	if (dviCore)
	{
		delete dviCore;
		dviCore = 0;
	}
	
	if (dvi_file)
	{
		dvi_file->close();
		delete dvi_file;
	}
	
	dvi_file = new QBuffer;	
	XWTeXIODev::compile();
}

void XWTeXGuiDev::doPage(int page_no)
{	
	dviCore->setPageNo(page_no);	
	dvi_file->seek(pageLoc[page_no]);
  uchar opcode = 0;
  uint  slen = 0;
  uchar sbuf[1024];
  for (;;)
  {
  	slen  = 0;
  	while (dvi_file->getChar((char*)&opcode) && opcode < DVI_SET1 && slen < 1024) 
   			sbuf[slen++] = opcode;
      		
   	if (slen > 0) 
   		doString(sbuf, slen);
      			
   	if (slen == 1024)
   		continue;
      		
   	if (opcode >= DVI_FNTNUM0 && opcode < DVI_FNT1) 
   	{
   		dviCore->doFnt(opcode - DVI_FNTNUM0);
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
   			dviCore->doEop();
   			return;
   			break;
      			
   		case DVI_PUSH:
   			doPush();
   			dviCore->doMarkDepth();
   			break;
      			
   		case DVI_POP:
   			doPop();
   			dviCore->doMarkDepth();
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
   			doW0(); 
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
   			doX0(); 
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
   			doY0(); 
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
   			doZ0(); 
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
      	dviCore->doGlyphArray(dvi_file, 0);
      	break;
      		
      case XDV_GLYPH_ARRAY:
      	dviCore->doGlyphArray(dvi_file, 1);
      	break;
      		
      case XDV_NATIVE_FONT_DEF:
      	doNativeFontDef(0);
      	break;
      		
      case XDV_PIC_FILE:
      	dviCore->doPicFile(dvi_file);
      	break;
      			
  		default:
   			return ;
   			break;
    }
  }
}

void XWTeXGuiDev::endInput()
{
	if (!fileStack.isEmpty())
		curFile = fileStack.pop();
	else
		curFile = mainFile;
}

QByteArray  XWTeXGuiDev::getByteArray(const QString & txt)
{
	if (curFile)
		return curFile->getByteArray(txt);
		
	return QByteArray();
}

TeXCategoryCode XWTeXGuiDev::getCategory()
{
	if (curFile)
		return curFile->getCategory();
		
	return EndFile;
}

qint32  XWTeXGuiDev::getChar()
{
	if (curFile)
		return curFile->getChar();
		
	return 0;
}

qint32  XWTeXGuiDev::getControlSequence(qint32 * buf)
{
	if (curFile)
		return curFile->getControlSequence(buf);
		
	return 0;
}

QString  XWTeXGuiDev::getCoord()
{
	if (curFile)
		return curFile->getCoord();
		
	return QString();
}

TeXFileMode  XWTeXGuiDev::getFileMode()
{
	if (curFile)
		return curFile->getFileMode();
		
	return TwoByteMode;
}

DVIFontDef * XWTeXGuiDev::getFontDefs()
{
	if (!dviCore)
		return 0;
		
	return dviCore->getFontDefs();
}

int  XWTeXGuiDev::getNumberOfDefFont()
{
	if (!dviCore)
		return 0;
		
	return dviCore->getNumberOfDefFont();
}

QString XWTeXGuiDev::getKey()
{
	if (curFile)
		return curFile->getKey();
		
	return QString();
}

QString XWTeXGuiDev::getOption()
{
	if (curFile)
		return curFile->getOption();
		
	return QString();
}

QString  XWTeXGuiDev::getParam()
{
	if (curFile)
		return curFile->getParam();
		
	return QString();
}

qint32 XWTeXGuiDev::getPos()
{
	if (curFile)
		return curFile->getPos();
		
	return -1;
}

QString  XWTeXGuiDev::getString(qint32 filepos, qint32 len)
{
	if (!seek(filepos))
		return QString();
		
	return curFile->getString(len);
}

qint32  XWTeXGuiDev::getSize()
{
	if (curFile)
		return curFile->getSize();
		
	return 0;
}

QString  XWTeXGuiDev::getText()
{
	if (curFile)
		return curFile->getText();
		
	return QString();
}

QString XWTeXGuiDev::getValue()
{
	if (curFile)
		return curFile->getValue();
		
	return QString();
}

QString  XWTeXGuiDev::getWord()
{
	if (curFile)
		return curFile->getWord();
		
	return QString();
}

qint32 XWTeXGuiDev::insert(qint32 filepos, 
	                       const QByteArray & txt)
{
	if (!seek(filepos))
		return 0;
		
	curFile->insert(txt);
	updateFilePos(filepos, txt.size());
	
	return txt.size();
}

qint32 XWTeXGuiDev::insert(qint32 filepos, 
	                       const QString & txt)
{
	if (!seek(filepos))
		return 0;
		
	qint32 len = curFile->insert(txt);
	updateFilePos(filepos, len);	
	return len;
}

qint32 XWTeXGuiDev::insertFile(qint32 filepos, const QString & filename)
{
	if (!curFile)
		return 0;
	
	XWTeXFile * file = newTeXFile();
	file->setTotalPos(filepos);
	if (!file->load(filename))
	{
		delete file;
		xwApp->openError(filename, false);
		return 0;
	}
	
	file->setParentFile(curFile);	
	qint32 len = file->getSize();
	updateFilePos(filepos, len);	
	curFile->addChild(curFile);
	fileStack.push(curFile);
	curFile = file;
	return len;
}

bool XWTeXGuiDev::loadFile(const QString & filename)
{
	curFile = 0;
	if (mainFile)
		delete mainFile;
		
	mainFile = 0;
	curFile = newTeXFile();
	if (!curFile->load(filename))
	{
		delete curFile;
		xwApp->openError(filename, false);
		curFile = 0;
		return false;
	}
	
	mainFile = curFile;
	setMainInputFile(filename);
	return true;
}

bool XWTeXGuiDev::loadFmt(const QString & fmtname)
{
	if (!XWTeXIODev::loadFmt(fmtname))
		return false;
		
	return XWTeXIODev::loadFmt();
}

QByteArray XWTeXGuiDev::read(qint32 filepos, qint32 len)
{
	if (!seek(filepos))
		return QByteArray();
		
	return curFile->read(len);
}

QString XWTeXGuiDev::readLine()
{
	if (curFile)
		return curFile->readLine();
		
	return QString();
}

void XWTeXGuiDev::remove(qint32 filepos, 
	                       qint32 len)
{
	if (!seek(filepos))
		return ;
		
	curFile->remove(len);	
	updateFilePos(filepos, -len);
}

qint32 XWTeXGuiDev::removeFile(qint32 filepos, const QString & filename)
{
	XWTeXFile * f = mainFile->findFile(filename);
	if (!f)	
		return 0;
		
	XWTeXFile * file = f->getParentFile();
	qint32 len = f->getSize();
	len = -len;
	file->removeFile(f);
	updateFilePos(filepos, len);
	
	return len;
}

qint32 XWTeXGuiDev::replace(qint32 filepos, 
	                          qint32 len, 
	                          const QByteArray & txt)
{
	if (!seek(filepos))
		return 0;
		
	qint32 l = curFile->replace(len, txt);	
	updateFilePos(filepos, l);	
	return l;
}

qint32 XWTeXGuiDev::replace(qint32 filepos, 
	                          qint32 len, 
	                          const QString & txt)
{
	if (!seek(filepos))
		return 0;
		
	qint32 l = curFile->replace(len, txt);	
	updateFilePos(filepos, l);		
	return l;
}

void XWTeXGuiDev::save()
{
	if (mainFile)
		mainFile->save();
}

void XWTeXGuiDev::save(const QString & filename)
{
	if (mainFile)
		mainFile->save(filename);
}

void XWTeXGuiDev::saveToDVI(QIODevice * outfileA)
{
	if (!dvi_file)
		return ;
		
	dvi_file->seek(0);
	QByteArray ba = dvi_file->readAll();
	outfileA->write(ba);
}

void XWTeXGuiDev::saveToPDF(QIODevice * outfileA,
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
	
	dvi_file->seek(0);
	int lenA = (int)(dvi_file->size());
	uchar * buf = (uchar*)malloc((lenA + 1) * sizeof(uchar));
	dvi_file->read((char*)buf, lenA);	
	QBuffer * fileA = new QBuffer;
	fileA->open(QIODevice::WriteOnly);
	fileA->write((char*)buf, lenA);
	fileA->close();
	fileA->open(QIODevice::ReadOnly);
	free(buf);
	XWDVIDoc * dvidoc = new XWDVIDoc;
	dvidoc->load(fileA, dviCore->isMPs() ? 1 : 0, outfileA);	
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
	
	int p = pages[0];
	char lm = landscape;
	double w = page_width;
	double h = page_height;
	double xoff = x_offset;
	double yoff = y_offset;
		
	for (int i = 0; i < pages.size(); i++)
	{		
		p = pages[i];
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
	
	dvidoc->close(false);
	for (int i = 0; i < pages.size(); i++)
	{		
		p = pages[i];
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
  dvidoc->close(true);
	delete dvidoc;
	dviCore->resetFlags();
}

void XWTeXGuiDev::scanSpecials(int page_no, 
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
  
  dvi_file->seek(pageLoc[page_no]);
  uchar opcode = 0;
  char sbuf[1024];
  while (dvi_file->getChar((char*)&opcode) && opcode != DVI_EOP)
  {
  	if (opcode < DVI_SET1 || (opcode >= DVI_FNTNUM0 && opcode < DVI_FNT1))
      continue;
    else if (opcode == DVI_XXX1 || opcode == DVI_XXX2 ||
             opcode == DVI_XXX3 || opcode == DVI_XXX4)
		{
			long  size = 0;
      char  buf[1024];
      switch (opcode) 
      {
      	case DVI_XXX1: 
      		size = getUnsignedByte(dvi_file);   
      		break;
      		
      	case DVI_XXX2: 
      		size = getUnsignedPair(dvi_file);   
      		break;
      		
      	case DVI_XXX3: 
      		size = getUnsignedTriple(dvi_file); 
      		break;
      		
      	case DVI_XXX4: 
      		size = getUnsignedQuad(dvi_file);   
      		break;
      }
      
      size = qMin(size, (long)1024);
      if (dvi_file->read(buf, size) != size)
      {
      	xwApp->error(tr("Reading DVI file failed!"));
      	return ;
      }
      
      if (dviCore->scanSpecial(page_width, page_height, x_offset, y_offset, landscape,
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
     		dvi_file->read(sbuf, 44);
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
     		getUnsignedByte(dvi_file);
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
     		getSignedPair(dvi_file);
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
     		getSignedTriple(dvi_file);
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
     		getSignedQuad(dvi_file);
     		break;

    	case DVI_SETRULE: 
    	case DVI_PUTRULE:
     		dvi_file->read(sbuf, 8);
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
     		getUnsignedByte(dvi_file);
     		break;

    	default: 
      	break;
		}
  }
}

bool XWTeXGuiDev::seek(qint32 filepos)
{
	curFile = mainFile->hitTest(filepos);	
	return (curFile != 0);
}

void XWTeXGuiDev::setCIDFontFixedPitch(bool e)
{
	dviCore->setCIDFontFixedPitch(e);
}

void XWTeXGuiDev::setDecimalDigits(int i)
{
	dviCore->setDecimalDigits(i);
}

void XWTeXGuiDev::setDPI(int hDPIA, int vDPIA)
{
	dviCore->setDPI(hDPIA, vDPIA);
}

void XWTeXGuiDev::setFontMapFirstMatch(bool e)
{
	dviCore->setFontMapFirstMatch(e);
}

void XWTeXGuiDev::setIgnoreColors(int i)
{
	dviCore->setIgnoreColors(i);
}

void XWTeXGuiDev::setMag(double m)
{
	dviCore->setMag(m);
}

void XWTeXGuiDev::setOffset(double hoff, double voff)
{
	dviCore->setOffset(hoff, voff);
}

void XWTeXGuiDev::setPaperSize(double w, double h)
{
	dviCore->setPaperSize(w, h);
}

void XWTeXGuiDev::setTPICTransFill(bool e)
{
	dviCore->setTPICTransFill(e);
}

void XWTeXGuiDev::skipComment()
{
	if (curFile)
		curFile->skipComment();
}

void XWTeXGuiDev::skipControlSequence()
{
	if (curFile)
		curFile->skipControlSequence();
}

void XWTeXGuiDev::skipCoord()
{
	if (curFile)
		curFile->skipCoord();
}

void XWTeXGuiDev::skipFormula()
{
	if (curFile)
		curFile->skipFormula();
}

void XWTeXGuiDev::skipGroup()
{
	if (curFile)
		curFile->skipGroup();
}

void XWTeXGuiDev::skipOption()
{
	if (curFile)
		curFile->skipOption();
}

void XWTeXGuiDev::skipSpacer()
{
	if (curFile)
		curFile->skipSpacer();
}

void XWTeXGuiDev::skipSpacerAndComment()
{
	if (curFile)
		curFile->skipSpacerAndComment();
}

void XWTeXGuiDev::skipWord()
{
	if (curFile)
		curFile->skipWord();
}

void XWTeXGuiDev::doBop()
{
	for (int i = 0; i < 10; i++)
		getSignedQuad(dvi_file);
		
	getSignedQuad(dvi_file);
	dviCore->doBop();
}

void XWTeXGuiDev::doDir()
{
	dviCore->doDir(getUnsignedByte(dvi_file));
}

void XWTeXGuiDev::doDown(long y)
{
	dviCore->doDown(y);
}

void XWTeXGuiDev::doDown1()
{
	doDown(getSignedByte(dvi_file));
}

void XWTeXGuiDev::doDown2()
{
	doDown(getSignedPair(dvi_file));
}

void XWTeXGuiDev::doDown3()
{
	doDown(getSignedTriple(dvi_file));
}

void XWTeXGuiDev::doDown4()
{
	doDown(getSignedQuad(dvi_file));
}

void XWTeXGuiDev::doFntDef()
{
	getSignedQuad(dvi_file);
  getSignedQuad(dvi_file);
  getSignedQuad(dvi_file);
  int area_len = getUnsignedByte(dvi_file);
  int name_len = getUnsignedByte(dvi_file);
  for (int i = 0; i < area_len + name_len; i++) 
   	getUnsignedByte(dvi_file);
}

void XWTeXGuiDev::doFnt1()
{
	long tex_id = getUnsignedByte(dvi_file);
  	dviCore->doFnt(tex_id);
}

void XWTeXGuiDev::doFnt2()
{
	long tex_id = getUnsignedPair(dvi_file);
  	dviCore->doFnt(tex_id);
}

void XWTeXGuiDev::doFnt3()
{
	long tex_id = getUnsignedTriple(dvi_file);
  	dviCore->doFnt(tex_id);
}

void XWTeXGuiDev::doFnt4()
{
	long tex_id = getSignedQuad(dvi_file);
  	dviCore->doFnt(tex_id);
}

void XWTeXGuiDev::doFntDef1()
{
	getUnsignedByte(dvi_file);
  	doFntDef();
}

void XWTeXGuiDev::doFntDef2()
{
	getUnsignedPair(dvi_file);
  	doFntDef();
}

void XWTeXGuiDev::doFntDef3()
{
	getUnsignedTriple(dvi_file);
  	doFntDef();
}

void XWTeXGuiDev::doFntDef4()
{
	getSignedQuad(dvi_file);
  	doFntDef();
}

void XWTeXGuiDev::doNativeFontDef(int scanningA)
{
	if (scanningA)
	{
		getUnsignedQuad(dvi_file);
		ushort flags = getUnsignedPair(dvi_file);
		if ((flags & XDV_FLAG_FONTTYPE_ICU) || (flags & XDV_FLAG_FONTTYPE_ATSUI))
		{
			int name_length = (int) getUnsignedByte(dvi_file);
      name_length += (int) getUnsignedByte(dvi_file);
      name_length += (int) getUnsignedByte(dvi_file);
      
      for (int i = 0; i < name_length; ++i)
        getUnsignedByte(dvi_file);
        
      if (flags & XDV_FLAG_COLORED)
      	getUnsignedQuad(dvi_file);
      	
       if (flags & XDV_FLAG_VARIATIONS)
       {
       		int nvars = getUnsignedPair(dvi_file);
       		for (int i = 0; i < nvars * 2; ++i)
       			getUnsignedQuad(dvi_file);
      	}
		}
	}
}

void XWTeXGuiDev::doPop()
{
	dviCore->doPop();
}

void XWTeXGuiDev::doPush()
{
	dviCore->doPush();
}

void XWTeXGuiDev::doPut(long ch)
{
	dviCore->doPut(ch);
}

void XWTeXGuiDev::doPut1()
{
	doPut(getUnsignedByte(dvi_file));
}

void XWTeXGuiDev::doPut2()
{
	doPut(getUnsignedPair(dvi_file));
}

void XWTeXGuiDev::doPut3()
{
	doPut(getUnsignedTriple(dvi_file));
}

void XWTeXGuiDev::doPut4()
{
	doPut(getUnsignedQuad(dvi_file));
}

void XWTeXGuiDev::doPutRule()
{
	long height = getSignedQuad(dvi_file);
  	long width  = getSignedQuad(dvi_file);
  	if (width > 0 && height > 0) 
		dviCore->doRule(width, height);
}

void XWTeXGuiDev::doRight(long x)
{
	dviCore->doRight(x);
}

void XWTeXGuiDev::doRight1()
{
	doRight(getSignedByte(dvi_file));
}

void XWTeXGuiDev::doRight2()
{
	doRight(getSignedPair(dvi_file));
}

void XWTeXGuiDev::doRight3()
{
	doRight(getSignedTriple(dvi_file));
}

void XWTeXGuiDev::doRight4()
{
	doRight(getSignedQuad(dvi_file));
}

void XWTeXGuiDev::doSet(long ch)
{	
	dviCore->doSet(ch);
}

void XWTeXGuiDev::doSet1()
{
	doSet(getUnsignedByte(dvi_file));
}

void XWTeXGuiDev::doSet2()
{
	doSet(getUnsignedPair(dvi_file));
}

void XWTeXGuiDev::doSet3()
{
	doSet(getUnsignedTriple(dvi_file));
}

void XWTeXGuiDev::doSet4()
{
	doSet(getUnsignedQuad(dvi_file));
}

void XWTeXGuiDev::doSetRule()
{
	long height = getSignedQuad(dvi_file);
  	long width  = getSignedQuad(dvi_file);
  	if (width > 0 && height > 0) 
			dviCore->doRule(width, height);
		
	dviCore->doRight(width);
}

void XWTeXGuiDev::doString(const uchar *s, int len)
{
	dviCore->doString(s, len);
}

void XWTeXGuiDev::doW(long ch)
{
	dviCore->doW(ch);
}

void XWTeXGuiDev::doW0()
{
	dviCore->doW0();
}

void XWTeXGuiDev::doW1()
{
	doW(getSignedByte(dvi_file));
}

void XWTeXGuiDev::doW2()
{
	doW(getSignedPair(dvi_file));
}

void XWTeXGuiDev::doW3()
{
	doW(getSignedTriple(dvi_file));
}

void XWTeXGuiDev::doW4()
{
	doW(getSignedQuad(dvi_file));
}

void XWTeXGuiDev::doX(long ch)
{
	dviCore->doX(ch);
}

void XWTeXGuiDev::doX0()
{
	dviCore->doX0();
}

void XWTeXGuiDev::doX1()
{
	doX(getSignedByte(dvi_file));
}

void XWTeXGuiDev::doX2()
{
	doX(getSignedPair(dvi_file));
}

void XWTeXGuiDev::doX3()
{
	doX(getSignedTriple(dvi_file));
}

void XWTeXGuiDev::doX4()
{
	doX(getSignedQuad(dvi_file));
}

void XWTeXGuiDev::doXXX(ulong size)
{
	
	uchar * buffer = new uchar[size+1];
  	for (ulong i = 0; i < size; i++) 
    	buffer[i] = getUnsignedByte(dvi_file);
    	
	dviCore->doSpecial((char*)buffer, size);
		
	delete [] buffer;
}

void XWTeXGuiDev::doXXX1()
{
	long size = getUnsignedByte(dvi_file);
  	doXXX(size);
}

void XWTeXGuiDev::doXXX2()
{
	long size = getUnsignedPair(dvi_file);
  	doXXX(size);
}

void XWTeXGuiDev::doXXX3()
{
	long size = getUnsignedTriple(dvi_file);
  	doXXX(size);
}

void XWTeXGuiDev::doXXX4()
{
	ulong size = getUnsignedQuad(dvi_file);
  	doXXX(size);
}

void XWTeXGuiDev::doY(long ch)
{
	dviCore->doY(ch);
}

void XWTeXGuiDev::doY0()
{
	dviCore->doY0();
}

void XWTeXGuiDev::doY1()
{
	doY(getSignedByte(dvi_file));
}

void XWTeXGuiDev::doY2()
{
	doY(getSignedPair(dvi_file));
}

void XWTeXGuiDev::doY3()
{
	doY(getSignedTriple(dvi_file));
}

void XWTeXGuiDev::doY4()
{
	doY(getSignedQuad(dvi_file));
}

void XWTeXGuiDev::doZ(long ch)
{
	dviCore->doZ(ch);
}

void XWTeXGuiDev::doZ0()
{
	dviCore->doZ0();
}

void XWTeXGuiDev::doZ1()
{
	doZ(getSignedByte(dvi_file));
}

void XWTeXGuiDev::doZ2()
{
	doZ(getSignedPair(dvi_file));
}

void XWTeXGuiDev::doZ3()
{
	doZ(getSignedTriple(dvi_file));
}

void XWTeXGuiDev::doZ4()
{
	doZ(getSignedQuad(dvi_file));
}

long XWTeXGuiDev::findPost()
{
	ulong  current = (ulong)(dvi_file->size());
	if (current == 0)
		return -1;
		
	ulong filesize = current;
	uchar ch = 0;	
	do 
	{
    	current--;
    	dvi_file->seek(current);
  	} while (dvi_file->getChar((char*)&ch) && (ch == DVI_PADDING) && current > 0);
  	
  	if (filesize - current < 4 || 
  		  current == 0 || 
  		  !(ch == DVI_ID_BYTE || ch == DVI_V_ID || ch == XDVI_ID)) 
    {
    	return -1;
  	} 
  	
  	current = current - 5;
  	dvi_file->seek(current);
  	
  	if (!dvi_file->getChar((char*)&ch) || (ch != DVI_POSTPOST)) 
  	{
    	return -1;
  	}
  	
  	current = getSignedQuad(dvi_file);
  	dvi_file->seek(current);
  	if (!dvi_file->getChar((char*)&ch) || (ch  != DVI_POST)) 
  	{
    	return -1;
  	}

  	return (long)current;
}

bool XWTeXGuiDev::getDVIFonts(long post_location)
{
	int  code = 0;
  	long tex_id = 0;
  	dvi_file->seek(post_location + 29);
  	while ((code = getUnsignedByte(dvi_file)) != DVI_POSTPOST)
  	{
  		switch (code) 
  		{
    		case DVI_FNTDEF1:
      			tex_id = getUnsignedByte(dvi_file);
      			break;
      			
    		case DVI_FNTDEF2:
      			tex_id = getUnsignedPair(dvi_file);
      			break;
      			
    		case DVI_FNTDEF3:
      			tex_id = getUnsignedTriple(dvi_file);
      			break;
      			
    		case DVI_FNTDEF4:
      			tex_id = getSignedQuad(dvi_file);
      			break;
      			
      	case XDV_NATIVE_FONT_DEF:
      		tex_id = getSignedQuad(dvi_file);
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

bool XWTeXGuiDev::getDVIInfo(long post_location)
{
	DVIHeader * header = dviCore->getHeader();
	dvi_file->seek(14);
	int length = getUnsignedByte(dvi_file);
  	if (dvi_file->read(header->comment, length) != length) 
		return false;
  	
  	header->comment[length] = '\0';	
	dvi_file->seek(post_location + 5);
	
  	header->unitNum = getUnsignedQuad(dvi_file);
  	header->unitDen = getUnsignedQuad(dvi_file);
  	header->mag      = getUnsignedQuad(dvi_file);
  	header->mediaHeight = getUnsignedQuad(dvi_file);
  	header->mediaWidth  = getUnsignedQuad(dvi_file);
  	header->stackDepth = getUnsignedPair(dvi_file);
  	
  	if (header->stackDepth > DVI_STACK_DEPTH_MAX) 
  	{
    	return false;
  	}
  	
  	return true;
}

bool  XWTeXGuiDev::getPageInfo(long post_location)
{
	dvi_file->seek(post_location + 27);
  	numPages = getUnsignedPair(dvi_file);
  	if (numPages == 0) 
  		return false;
  		
  	pageLoc = (ulong * )malloc(numPages * sizeof(ulong));
  	dvi_file->seek(post_location + 1);
  	pageLoc[numPages - 1] = getUnsignedQuad(dvi_file);
  	if ((pageLoc[numPages - 1] + 41) > dvi_file->size())
  		return false;
  		
  	for (int i = numPages - 2; i >= 0; i--) 
  	{
    	dvi_file->seek(pageLoc[i + 1] + 41);
    	pageLoc[i] = getUnsignedQuad(dvi_file);
    	if ((pageLoc[numPages-1] + 41) > dvi_file->size())
    		return false;
  	}
  	
  	return true;
}

QIODevice * XWTeXGuiDev::openInput(const QString & filename)
{
	if (mainFile)
	{
		curFile = mainFile->findFile(filename);
		if (curFile)
			return curFile->open();
	}
	
	return XWTeXIODev::openInput(filename);
}

bool XWTeXGuiDev::readFontRecord(long tex_id)
{
	DVIFontDef * font = dviCore->getFontDef();
  	
  getUnsignedQuad(dvi_file);
  ulong point_size  = getUnsignedQuad(dvi_file);
  ulong design_size = getUnsignedQuad(dvi_file);
  int dir_length  = getUnsignedByte(dvi_file);
  int name_length = getUnsignedByte(dvi_file);
  	
  char * directory   = new char[dir_length + 1];
  if (dvi_file->read(directory, dir_length) != dir_length) 
  {
  	delete [] directory;
	  return false;
  }
  	
  directory[dir_length] = '\0';
  delete [] directory;

  char * font_name   = new char[name_length + 1];
  if (dvi_file->read(font_name, name_length) != name_length) 
  {
  	delete [] font_name;
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

bool XWTeXGuiDev::readNativeFontRecord(long tex_id)
{
	DVIFontDef * font = dviCore->getFontDef();
	ulong  point_size  = getUnsignedQuad(dvi_file);
  ushort flags       = getUnsignedPair(dvi_file);
  if ((flags & XDV_FLAG_FONTTYPE_ICU) || (flags & XDV_FLAG_FONTTYPE_ATSUI))
  {
  	int plen = getUnsignedByte(dvi_file);
  	int flen = getUnsignedByte(dvi_file);
  	int slen = getUnsignedByte(dvi_file);
  	char * font_name = new char[plen + 1];
  	if (dvi_file->read(font_name, plen) != plen)
  	{
  		delete [] font_name;
  		return false;
  	}
  	
  	font_name[plen] = '\0';
  	char * fam_name = new char[flen + 1];
  	if (dvi_file->read(fam_name, flen) != flen)
  	{
  		delete [] fam_name;
  		return false;
  	}
  	
  	fam_name[flen] = '\0';
    char * sty_name = new char[slen + 1];
    if (dvi_file->read(sty_name, slen) != slen)
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
  		font->rgbaColor = getUnsignedQuad(dvi_file);
  	else
  		font->rgbaColor = 0xffffffff;
  		
  	if (flags & XDV_FLAG_VARIATIONS)
  	{
  		int nvars = getUnsignedPair(dvi_file);
  		for (int i = 0; i < nvars * 2; i++)
  			getUnsignedQuad(dvi_file);
  	}
  }
  
  return true;
}

void XWTeXGuiDev::run()
{
	XWTeXIODev::run();
	dvi_file->open(QIODevice::ReadOnly);
	dvi_file->seek(0);
	if (!isOk())
		return ;
		
	xref = new XWDVIRef;	
	doc->setXRef(xref);
		
	dviCore = new XWDVICore(xref);
		
	long post_location = findPost();
  if (post_location < 0)
  	return ;
  	
  if (!getDVIInfo(post_location))
  	return ;
  		
  if (!getPageInfo(post_location))
  	return ;  			
	
	if (!getDVIFonts(post_location))
  	return ;
  	
  dviCore->beginDoc();
  for (long i = 0; i < numPages; i++)
		doPage(i);
			
	dviCore->endDoc(false);
		
	XWCatalog * catalog = new XWCatalog(doc);
	doc->setCatalog(catalog);
	XWOutline * outline = new XWOutline(catalog->getOutline(), xref);
	doc->setOutline(outline);		
	XWOptionalContent * optContent = new XWOptionalContent(doc);
	doc->setOptContent(optContent);
}

void XWTeXGuiDev::updateFilePos(qint32 filepos, qint32 l)
{
	mainFile->updateFilePos(filepos, l);
}
