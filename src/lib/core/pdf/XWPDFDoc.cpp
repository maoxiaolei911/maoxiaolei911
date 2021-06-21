/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <QString>
#include <QFile>
#include "XWApplication.h"
#include "XWString.h"
#include "XWDocSea.h"
#include "XWObject.h"
#include "XWStream.h"
#include "XWLink.h"
#include "XWOutputDev.h"
#include "XWPDFLexer.h"
#include "XWPDFParser.h"
#include "XWOutline.h"
#include "XWPDFXRef.h"
#include "XWSecurityHandler.h"
#include "XWOptionalContent.h"
#include "XWPDFDoc.h"

#define headerSearchSize 1024

XWPDFDoc::XWPDFDoc(QObject * parent)
	:XWDoc(parent),
	 str(0),
	 guiData(0),
	 pdfVersion(0.0)
{
}

XWPDFDoc::~XWPDFDoc()
{
    if (str)
        delete str;
}

bool XWPDFDoc::isLinearized()
{
    bool lin = false;
    XWObject obj1, obj2, obj3, obj4, obj5;;
    obj1.initNull();    
    XWPDFParser * parser = new XWPDFParser(xref, 
                                           new XWPDFLexer(xref, str->makeSubStream(str->getStart(), false, 0, &obj1)),
	                                       true);
    parser->getObj(&obj1);    
    parser->getObj(&obj2);
    parser->getObj(&obj3);
    parser->getObj(&obj4);
    if (obj1.isInt() && obj2.isInt() && obj3.isCmd("obj") && obj4.isDict()) 
    {
        obj4.dictLookup("Linearized", &obj5);
        if (obj5.isNum() && obj5.getNum() > 0) 
            lin = true;
            
        obj5.free();
    }
    obj4.free();
    obj3.free();
    obj2.free();
    obj1.free();
    delete parser;
    return lin;
}

bool XWPDFDoc::load(XWString *fileNameA, 
                    XWString *ownerPassword,
	                XWString *userPassword, 
	                void *guiDataA)
{
	fileName = fileNameA;
    guiData = guiDataA; 
	QString fn = fileNameA->toQString();
	XWDocSea sea;
    file = sea.openPdf(fn);
    if (!file)
    	return false;
    
    XWObject obj;
    obj.initNull();
    str = new XWFileStream(file, 0, false, 0, &obj);
    ok = setup(ownerPassword, userPassword);
    return ok;
}

bool XWPDFDoc::load(XWBaseStream *strA, 
	                XWString *ownerPassword,
	                XWString *userPassword, 
	                void *guiDataA)
{
	guiData = guiDataA;
	str = strA;
	ok = setup(ownerPassword, userPassword);
	
	return ok;
}

bool XWPDFDoc::checkEncryption(XWString *ownerPassword, XWString *userPassword)
{
    XWObject encrypt;
    bool encrypted;    
    bool ret = true;
    
    xref->getTrailerDict()->dictLookup("Encrypt", &encrypt);
    if ((encrypted = encrypt.isDict())) 
    {
    	XWSecurityHandler *secHdlr = XWSecurityHandler::make(this, &encrypt);
        if (secHdlr) 
        {
            if (secHdlr->checkEncryption(ownerPassword, userPassword)) 
            {
       	        xref->setEncryption(secHdlr->getPermissionFlags(),
			                        secHdlr->getOwnerPasswordOk(),
			                        secHdlr->getFileKey(),
			                        secHdlr->getFileKeyLength(),
			                        secHdlr->getEncVersion(),
			                        secHdlr->getEncAlgorithm());
	            ret = true;
            } 
            else 
	            ret = false;
	            
            delete secHdlr;
        } 
        else 
            ret = false;
    } 
    else 
        ret = true;
    encrypt.free();
    return ret;
}

bool XWPDFDoc::checkHeader()
{
    pdfVersion = 0;
    char hdrBuf[headerSearchSize+1];
    for (int i = 0; i < headerSearchSize; ++i) 
        hdrBuf[i] = str->getChar();
    
    hdrBuf[headerSearchSize] = '\0';
    int i = 0;
    for (; i < headerSearchSize - 5; ++i) 
    {
        if (!strncmp(&hdrBuf[i], "%PDF-", 5)) 
            break;
    }
    
    if (i >= headerSearchSize - 5) 
        return false;
    
    str->moveStart(i);
    char * p = strtok(&hdrBuf[i+5], " \t\n\r");
    if (!p) 
        return false;
    
    pdfVersion = atof(p);
    if (!(hdrBuf[i+5] >= '0' && hdrBuf[i+5] <= '9') || pdfVersion > supportedPDFVersionNum + 0.0001) 
    {
        QString e = QString(tr("unsupported version: %1!\n")).arg(pdfVersion);
        xwApp->error(e);
        
        return false;
    }
    
    return true;
}

bool XWPDFDoc::setup(XWString *ownerPassword, XWString *userPassword)
{
    str->reset();
    if (!checkHeader())
    	return false;
    	
    xref = new XWPDFXRef(str);
    if (!xref->isOk())
    {
    	xwApp->error(tr("couldn't read xref table!\n"));
        return false;
    }
        
    if (!checkEncryption(ownerPassword, userPassword))
        return false;
        
    catalog = new XWCatalog(this);
    if (!catalog->isOk()) 
    {
        QString e = QString(tr("couldn't read page catalog!\n"));
        xwApp->error(e);
        return false;
    }
    
    outline = new XWOutline(catalog->getOutline(), xref);
    optContent = new XWOptionalContent(this);
    return true;
}

