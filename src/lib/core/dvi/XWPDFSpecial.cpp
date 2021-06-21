/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <QTextCodec>
#include "XWLexer.h"
#include "XWApplication.h"
#include "XWDocSea.h"
#include "XWCMap.h"
#include "XWTexFontMap.h"
#include "XWFontCache.h"
#include "XWObject.h"
#include "XWDVIRef.h"
#include "XWDVICore.h"
#include "XWDVIDev.h"
#include "XWDVIParser.h"
#include "XWPDFSpecial.h"

#define STRING_STREAM 0
#define FILE_STREAM   1

#define SP_PDF_DOANN     0
#define SP_PDF_OUT       1
#define SP_PDF_ART       2
#define SP_PDF_DOBEAD    3
#define SP_PDF_DODEST    4
#define SP_PDF_OBJ       5
#define SP_PDF_DOCINFO   6
#define SP_PDF_DOCVIEW   7
#define SP_PDF_CONTENT   8
#define SP_PDF_PUT       9
#define SP_PDF_CLOSE     10
#define SP_PDF_BOP       11
#define SP_PDF_EOP       12
#define SP_PDF_IMG       13
#define SP_PDF_LINK      14
#define SP_PDF_NOLINK    15
#define SP_PDF_BCOLOR    16
#define SP_PDF_SCOLOR    17
#define SP_PDF_ECOLOR    18
#define SP_PDF_BGCOLOR   19
#define SP_PDF_PAGESIZE  20
#define SP_PDF_BANN      21
#define SP_PDF_EANN      22
#define SP_PDF_BTRANS    23
#define SP_PDF_ETRANS    24
#define SP_PDF_BFORM     25
#define SP_PDF_EFORM     26
#define SP_PDF_UXOBJ     27
#define SP_PDF_TOUNICODE 28
#define SP_PDF_LITERAL   29
#define SP_PDF_STREAM    30
#define SP_PDF_FSTREAM   31
#define SP_PDF_NAMES     32
#define SP_PDF_MAPLINE   33
#define SP_PDF_MAPFILE   34
#define SP_PDF_BCONTENT  35
#define SP_PDF_ECONTENT  36
#define SP_PDF_CODE      37

static SpecialHandler pdfHandler[] = 
{
	{"annotation", SP_PDF_DOANN},
	{"annotate", SP_PDF_DOANN},
	{"annot", SP_PDF_DOANN},
	{"ann", SP_PDF_DOANN},
		
	{"outline", SP_PDF_OUT},
	{"out", SP_PDF_OUT},
		
	{"article", SP_PDF_ART},
	{"art", SP_PDF_ART},
		
	{"bead", SP_PDF_DOBEAD},
	{"thread", SP_PDF_DOBEAD},
		
	{"destination", SP_PDF_DODEST},
	{"dest", SP_PDF_DODEST},
		
	{"object", SP_PDF_OBJ},
	{"obj", SP_PDF_OBJ},
		
	{"docinfo", SP_PDF_DOCINFO},
	{"docview", SP_PDF_DOCVIEW},
		
	{"content", SP_PDF_CONTENT},
		
	{"put", SP_PDF_PUT},
	{"close", SP_PDF_CLOSE},
		
	{"bop", SP_PDF_BOP},
	{"eop", SP_PDF_EOP},
		
	{"image", SP_PDF_IMG},
	{"img", SP_PDF_IMG},
	{"epdf", SP_PDF_IMG},
		
	{"link", SP_PDF_LINK},
	{"nolink", SP_PDF_NOLINK},
		
	{"begincolor", SP_PDF_BCOLOR},
	{"bcolor", SP_PDF_BCOLOR},
	{"bc", SP_PDF_BCOLOR},
		
	{"setcolor", SP_PDF_SCOLOR},
	{"scolor", SP_PDF_SCOLOR},
	{"sc", SP_PDF_SCOLOR},
		
	{"endcolor", SP_PDF_ECOLOR},
	{"ecolor", SP_PDF_ECOLOR},
	{"ec", SP_PDF_ECOLOR},
		
	{"begingray", SP_PDF_BCOLOR},
	{"bgray", SP_PDF_BCOLOR},
	{"bg", SP_PDF_BCOLOR},
		
	{"endgray", SP_PDF_ECOLOR},
	{"egray", SP_PDF_ECOLOR},
	{"eg", SP_PDF_ECOLOR},
		
	{"bgcolor", SP_PDF_BGCOLOR},
	{"bgc", SP_PDF_BGCOLOR},
	{"bbc", SP_PDF_BGCOLOR},
	{"bbg", SP_PDF_BGCOLOR},
		
	{"bannot", SP_PDF_BANN},
	{"beginann", SP_PDF_BANN},
	{"bann", SP_PDF_BANN},
		
	{"eannot", SP_PDF_EANN},
	{"endann", SP_PDF_EANN},
	{"eann", SP_PDF_EANN},
		
	{"btrans", SP_PDF_BTRANS},
	{"begintransform", SP_PDF_BTRANS},
	{"begintrans", SP_PDF_BTRANS},
	{"bt", SP_PDF_BTRANS},
		
	{"etrans", SP_PDF_ETRANS},
	{"endtransform", SP_PDF_ETRANS},
	{"endtrans", SP_PDF_ETRANS},
	{"et", SP_PDF_ETRANS},
		
	{"bform", SP_PDF_BFORM},
	{"beginxobj", SP_PDF_BFORM},
	{"bxobj", SP_PDF_BFORM},
		
	{"eform", SP_PDF_EFORM},
	{"endxobj", SP_PDF_EFORM},
	{"exobj", SP_PDF_EFORM},
		
	{"usexobj", SP_PDF_UXOBJ},
	{"uxobj", SP_PDF_UXOBJ},
		
	{"tounicode", SP_PDF_TOUNICODE},
		
	{"literal", SP_PDF_LITERAL},
		
	{"stream", SP_PDF_STREAM},
		
	{"fstream", SP_PDF_FSTREAM},
		
	{"names", SP_PDF_NAMES},
		
	{"mapline", SP_PDF_MAPLINE},
	{"mapfile", SP_PDF_MAPFILE},
		
		
	{"bcontent", SP_PDF_BCONTENT},
	{"econtent", SP_PDF_ECONTENT},
		
	{"code", SP_PDF_CODE}
};

#define numPDFOps (sizeof(pdfHandler) / sizeof(SpecialHandler))

XWPDFSpecial::XWPDFSpecial(XWDVICore * coreA,
	                       XWDVIRef * xrefA,
	                       XWDVIDev * devA,
	                       const char *buffer, 
	                       long sizeA,
		                   double x_userA, 
		                   double y_userA, 
		                   double magA,
		                   int typeA,
		                   QObject * parent)
	:XWDVISpecial(coreA, xrefA, devA, buffer, sizeA, x_userA, y_userA, magA, typeA, parent)
{
	hDPI = 600;
	vDPI = 600;
}

int XWPDFSpecial::check(XWLexer * lexerA)
{
	lexerA->skipWhite();
	char * q = lexerA->getCIdent();
	if (!q)
		return -1;
		
	int typeA = -1;
	for (int i = 0; i < numPDFOps; i++)
	{
		SpecialHandler * handler = &pdfHandler[i];
		if (!strcmp(q, handler->key))
		{
			typeA = handler->type;
			break;
		}
	}
	
	delete [] q;
	return typeA;
}

bool XWPDFSpecial::exec(int hDPIA, int vDPIA)
{
	hDPI = hDPIA;
	hDPI = vDPIA;
	
	int ret = 0;
	switch (type)
	{
		case SP_PDF_DOANN:
			ret = doAnnot();
			break;
			
		case SP_PDF_OUT:
			ret = doOutline();
			break;
			
		case SP_PDF_ART:
			ret = doArticle();
			break;
			
		case SP_PDF_DOBEAD:
			ret = doBead();
			break;
		
		case SP_PDF_DODEST:
			ret = doDest();
			break;
			
		case SP_PDF_OBJ:
			ret = doObject();
			break;
			
		case SP_PDF_DOCINFO:
			ret = doDocInfo();
			break;
			
		case SP_PDF_DOCVIEW:
			ret = doDocView();
			break;
			
		case SP_PDF_CONTENT:
			ret = doContent();
			break;
			
		case SP_PDF_PUT:
			ret = doPut();
			break;
			
		case SP_PDF_CLOSE:
			ret = doClose();
			break;
			
		case SP_PDF_BOP:
			ret = doBop();
			break;
			
		case SP_PDF_EOP:
			ret = doEop();
			break;
			
		case SP_PDF_IMG:
			ret = doImage();
			break;
			
		case SP_PDF_LINK:
			ret = doLink();
			break;
			
		case SP_PDF_NOLINK:
			ret = doNoLink();
			break;
			
		case SP_PDF_BCOLOR:
			ret = doBColor();
			break;
			
		case SP_PDF_SCOLOR:
			ret = doSColor();
			break;
			
		case SP_PDF_ECOLOR:
			ret = doEColor();
			break;
			
		case SP_PDF_BGCOLOR:
			ret = doBGColor();
			break;
			
		case SP_PDF_PAGESIZE:
			break;
			
		case SP_PDF_BANN:
			ret = doBAnn();
			break;
			
		case SP_PDF_EANN:
			ret = doEAnn();
			break;
			
		case SP_PDF_BTRANS:
			ret = doBTrans();
			break;
			
		case SP_PDF_ETRANS:
			ret = doETrans();
			break;
			
		case SP_PDF_BFORM:
			ret = doBForm();
			break;
			
		case SP_PDF_EFORM:
			ret = doEForm();
			break;
			
		case SP_PDF_UXOBJ:
			ret = doUXObj();
			break;
			
		case SP_PDF_TOUNICODE:
			ret = doToUnicode();
			break;
			
		case SP_PDF_LITERAL:
			ret = doLiteral();
			break;
			
		case SP_PDF_STREAM:
			ret = doStream();
			break;
			
		case SP_PDF_FSTREAM:
			ret = doFStream();
			break;
			
		case SP_PDF_NAMES:
			ret = doNames();
			break;
			
		case SP_PDF_MAPLINE:
			ret = doMapLine();
			break;
			
		case SP_PDF_MAPFILE:
			ret = doMapFile();
			break;
			
		case SP_PDF_BCONTENT:
			ret = doBContent();
			break;
			
		case SP_PDF_ECONTENT:
			ret = doEContent();
			break;
			
		case SP_PDF_CODE:
			ret = doCode();
			break;
			
		default:
			return false;
			break;
	}
	
	return ret < 0 ? false : true;
}

int XWPDFSpecial::doAnnot()
{
	const char * p = lexer->skipWhite();
	char * ident = 0;
	if (p[0] == '@') 
	{
    	ident = lexer->getOptIdent();
    	p = lexer->skipWhite();
  	}
  	
  	TransformInfo ti;
  	transform_info_clear(&ti);
  	if (readDimTrns(&ti, NULL, 0) < 0) 
  	{
    	if (ident)
      		delete [] ident;
    	return  -1;
  	}
  	
  	if ((ti.flags & INFO_HAS_USER_BBOX) && 
      	((ti.flags & INFO_HAS_WIDTH) || (ti.flags & INFO_HAS_HEIGHT))) 
    {
    	xwApp->warning(tr("you can't specify both bbox and width/height.\n"));
    	if (ident)
      		delete [] ident;
    	return  -1;
  	}
  	
  	XWObject annot_dict;
  	if (xref->isScanning())
  	{
  		PDFState *sd = core->getPDFState();  		
  		if (!(sd->parsePDFDict(core, xref, lexer, &annot_dict)))
  		{
    		xwApp->warning(tr("could not find dictionary object.\n"));
    		if (ident)
      			delete [] ident;
    		return  -1;
  		}
  		else if (!annot_dict.isDict())
  		{
  			xwApp->warning(tr("invalid type: not dictionary object.\n"));
    		if (ident)
      			delete [] ident;
    		annot_dict.free();
    		return  -1;
  		}
  	}
  	
  	PDFCoord cp;
  	cp.x = x_user; 
  	cp.y = y_user;
  	dev->transform(&cp, NULL);
  	PDFRect rect;
  	if (ti.flags & INFO_HAS_USER_BBOX) 
  	{
    	rect.llx = ti.bbox.llx + cp.x;
    	rect.lly = ti.bbox.lly + cp.y;
    	rect.urx = ti.bbox.urx + cp.x;
    	rect.ury = ti.bbox.ury + cp.y;
  	} 
  	else 
  	{
    	rect.llx = cp.x;
    	rect.lly = cp.y - mag * ti.depth;
    	rect.urx = cp.x + mag * ti.width;
    	rect.ury = cp.y + mag * ti.height;
  	}
  	
  	if (ident)
  	{
  		XWObject obj;
  		annot_dict.copy(&obj);
  		core->pushObject(ident, &obj);
  	}
    
    xref->addAnnot(dev, xref->currentPageNumber(), &rect, &annot_dict, 1);
        	
    if (ident)
    {
    	core->flushObject(ident);
    	delete [] ident;
    }
    
    if (xref->isScanning())
    	xref->releaseObj(&annot_dict);
    else
    	annot_dict.free();
    
  	return  0;
}

int XWPDFSpecial::doArticle()
{
	if (!(xref->isScanning()))
		return 0;
		
	lexer->skipWhite();
	char * ident = lexer->getOptIdent();
	if (!ident) 
	{
    	xwApp->warning(tr("Article name expected but not found.\n"));
    	return -1;
  	}
  	
	PDFState *sd = core->getPDFState();
	XWObject info_dict;
	info_dict.initNull();	
  	if (!(sd->parsePDFDict(core, xref, lexer, &info_dict)))
  	{
  		xwApp->warning(tr("ignoring article with invalid info dictionary.\n"));
    	delete [] ident;
    	return  -1;
  	}
  	
  	XWObject obj;
  	info_dict.copy(&obj);
  	xref->beginArticle(ident, &obj);
  	core->pushObject(ident, &info_dict);
  	delete [] ident;

  	return  0;
}

int XWPDFSpecial::doBAnn()
{
	PDFState *sd = core->getPDFState();
	if (xref->isScanning())
	{
		if (!(sd->annot_dict.isNull()))
		{
    		xwApp->warning(tr("can't begin an annotation when one is pending.\n"));
    		return  -1;
  		}
  
		lexer->skipWhite();
  		if (!(sd->parsePDFDict(core, xref, lexer, &(sd->annot_dict))))
  		{
    		xwApp->warning(tr("ignoring annotation with invalid dictionary.\n"));
    		return  -1;
  		} 
  		else if (!sd->annot_dict.isDict()) 
  		{
    		xwApp->warning(tr("invalid type: not a dictionary object.\n"));
    		sd->annot_dict.free();
    		sd->annot_dict.initNull();
    		return  -1;
  		}
  	}

  	return beginAnnot(&(sd->annot_dict));
}

int XWPDFSpecial::doBColor()
{
	XWDVIColor fc, sc;
  	XWDVIColor *pfc, *psc;

  	dev->getCurrentColor(&psc, &pfc);  	
  	int error = readPDFColor(&fc, pfc);
  	if (!error) 
  	{
  		const char * p = lexer->skipWhite();
		const char * endptr = lexer->getEndPtr();
    	if (p < endptr) 
      		error = readPDFColor(&sc, psc);
    	else 
      		sc.copy(&fc);
  	}

  	if (error)
    	xwApp->warning(tr("invalid color specification?\n"));
  	else 
    	dev->pushColor(xref, &sc, &fc);

  	return  error;
}

int XWPDFSpecial::doBContent()
{
	PDFTMatrix M;
  	double xpos, ypos;

  	dev->gsave(xref);
  	dev->getCoord(&xpos, &ypos);
  	pdf_setmatrix(&M, 1.0, 0.0, 0.0, 1.0, x_user - xpos, y_user - ypos);
  	dev->concat(xref, &M);
  	dev->pushCoord(x_user, y_user);
  	return  0;
}

int XWPDFSpecial::doBead()
{
	if (!(xref->isScanning()))
		return 0;
		
	const char * p = lexer->skipWhite();
	if (p[0] != '@') 
	{
    	xwApp->warning(tr("Article identifier expected but not found.\n"));
    	return  -1;
  	}
  	
  	char * article_name = lexer->getOptIdent();
  	if (!article_name) 
  	{
    	xwApp->warning(tr("Article reference expected but not found.\n"));
    	return  -1;
  	}
  	
  	TransformInfo   ti;
  	transform_info_clear(&ti);
  	if (readDimTrns(&ti, NULL, 0) < 0) 
  	{
    	delete [] article_name;
    	return  -1;
  	}

  	if ((ti.flags & INFO_HAS_USER_BBOX) && 
  		((ti.flags & INFO_HAS_WIDTH) || (ti.flags & INFO_HAS_HEIGHT))) 
  	{
    	xwApp->warning(tr("you can't specify both bbox and width/height.\n"));
    	delete [] article_name;
    	return  -1;
  	}
  	
  	PDFCoord cp;
  	PDFRect  rect;
  	cp.x = x_user; 
  	cp.y = y_user;
  	dev->transform(&cp, NULL);
  	if (ti.flags & INFO_HAS_USER_BBOX) 
  	{
    	rect.llx = ti.bbox.llx + cp.x;
    	rect.lly = ti.bbox.lly + cp.y;
    	rect.urx = ti.bbox.urx + cp.x;
    	rect.ury = ti.bbox.ury + cp.y;
  	} 
  	else 
  	{
    	rect.llx = cp.x;
    	rect.lly = cp.y - mag * ti.depth;
    	rect.urx = cp.x + mag * ti.width;
    	rect.ury = cp.y + mag * ti.height;
  	}
  	
  	p = lexer->skipWhite();
  	PDFState *sd = core->getPDFState();
  	XWObject article_info;
  	if (p[0] != '<') 
    	article_info.initDict(xref);
    else if (!sd->parsePDFDict(core, xref, lexer, &article_info))
    {
    	xwApp->warning(tr("error in reading dictionary.\n"));
      	delete [] article_name;
      	return -1;
    }
    
    XWObject article;
    article.initNull();
    if (core->lookupObject(article_name, &article)) 
    {
    	article.dictMerge(&article_info);
    	article_info.free();
    	article.free();
  	} 
  	else 
  	{
  		XWObject obj;  		
    	xref->beginArticle(article_name, &article_info);
  		article_info.copy(&obj);
    	core->pushObject(article_name, &obj);
  	}
  	
  	int page_no = xref->currentPageNumber();
  	xref->addBead(article_name, NULL, page_no, &rect);

  	delete [] article_name;
  	return  0;
}

int XWPDFSpecial::doBForm()
{
	if (xref->isScanning())
	{
		lexer->skipWhite();
		char * ident = lexer->getOptIdent();
		if (!ident) 
		{
    		xwApp->warning(tr("A form XObject must have name.\n"));
    		return  -1;
  		}
  	
  		TransformInfo  ti;
  		transform_info_clear(&ti);
  		if (readDimTrns(&ti, NULL, 0) < 0) 
  		{
    		delete [] ident;
    		return  -1;
  		}
  	
  		PDFRect cropbox;
  		if (ti.flags & INFO_HAS_USER_BBOX) 
  		{
    		if (ti.bbox.urx - ti.bbox.llx == 0.0 || ti.bbox.ury - ti.bbox.lly == 0.0) 
    		{
      			xwApp->warning(tr("Bounding box has a zero dimension.\n"));
      			delete [] ident;
      			return -1;
    		}
    	
    		cropbox.llx = ti.bbox.llx;
    		cropbox.lly = ti.bbox.lly;
    		cropbox.urx = ti.bbox.urx;
    		cropbox.ury = ti.bbox.ury;
  		} 
  		else 
  		{
    		if (ti.width == 0.0 || ti.depth + ti.height == 0.0) 
    		{
      			xwApp->warning(tr("Bounding box has a zero dimension.\n"));
      			delete [] ident;
      			return -1;
    		}
    	
    		cropbox.llx = 0.0;
    		cropbox.lly = -ti.depth;
    		cropbox.urx = ti.width;
    		cropbox.ury = ti.height;
  		}

  		int xobj_id = xref->beginGrabbing(core, dev, ident, x_user, y_user, &cropbox);
  		if (xobj_id < 0) 
  		{
    		delete [] ident;
    		xwApp->warning(tr("couldn't start form object.\n"));
    		return -1;
  		}
  	
  		XWObject obj;
		core->getImageReference(xobj_id, &obj);
  		core->pushObject(ident, &obj);
  		delete [] ident;
  	}
  	else
  		xref->beginGrabbing(core, dev, 0, x_user, y_user, 0);

  	return  0;
}

int XWPDFSpecial::doBGColor()
{
	XWDVIColor colorspec;
	int error = readPDFColor(&colorspec, NULL);
  	if (error)
    	xwApp->warning(tr("no valid color specified?\n"));
  	else 
    	xref->setBGColor(&colorspec);

  	return  error;
}

int XWPDFSpecial::doBop()
{
	if (!(xref->isScanning()))
		return 0;
		
	const char * p = lexer->skipBlank();
	const char * endptr = lexer->getEndPtr();
	if (p < endptr) 
    	xref->setBopContent(p, (long) (endptr - p));

  	return  0;
}

int XWPDFSpecial::doBTrans()
{
	TransformInfo ti;
	transform_info_clear(&ti);
  	if (readDimTrns(&ti, NULL, 0) < 0) 
    	return -1;

	PDFTMatrix     M;
  	/* Create transformation matrix */
  	pdf_copymatrix(&M, &(ti.matrix));
  	M.e += ((1.0 - M.a) * x_user - M.c * y_user);
  	M.f += ((1.0 - M.d) * y_user - M.b * x_user);

  	dev->gsave(xref);
  	dev->concat(xref, &M);

  	return  0;
}

int XWPDFSpecial::doClose()
{
	if (!(xref->isScanning()))
		return 0;
		
	lexer->skipWhite();
	char * ident = lexer->getOptIdent();
  	if (ident) 
  	{
    	core->flushObject(ident);
    	delete [] ident;
  	} 
  	else 
    	core->clearObjects();

  	return  0;
}

int XWPDFSpecial::doCode()
{
	const char * p = lexer->skipWhite();
	const char * endptr = lexer->getEndPtr();
	if (p < endptr) 
	{
    	xref->addPageContent(" ", 1);  /* op: */
    	xref->addPageContent(p, (long) (endptr - p));  /* op: ANY */
    	lexer->setCurPtr(endptr);
  	}

  	return  0;
}

int XWPDFSpecial::doContent()
{
	char buf[1024];
	const char * p = lexer->skipWhite();
	const char * endptr = lexer->getEndPtr();
	int len = 0;
	if (p < endptr) 
	{
    	PDFTMatrix M;

    	pdf_setmatrix(&M, 1.0, 0.0, 0.0, 1.0, x_user, y_user);
    	buf[len++] = ' ';
    	buf[len++] = 'q';
    	buf[len++] = ' ';
    	len += dev->sprintMatrix(buf + len, &M);
    	buf[len++] = ' ';
    	buf[len++] = 'c';
    	buf[len++] = 'm';
    	buf[len++] = ' ';

    	xref->addPageContent(buf, len);  /* op: q cm */
    	len = (long) (endptr - p);
    	xref->addPageContent(p, len);  /* op: ANY */
    	xref->addPageContent(" Q", 2);  /* op: Q */
  	}

  	return  0;
}

int XWPDFSpecial::doDest()
{
	if (!(xref->isScanning()))
		return 0;
		
	lexer->skipWhite();
	XWObject name;
	name.initNull();
	XWDVIParser parser(core, xref, lexer, 0);
	if (!parser.parsePDFObject(&name))
	{
		xwApp->warning(tr("PDF string expected for destination name but not found.\n"));
    	return  -1;
	}
	else if (!name.isString())
	{
		xwApp->warning(tr("PDF string expected for destination name but invalid type.\n"));
    	name.free();
    	return  -1;
	}
	
	XWObject array;
	array.initNull();
	if (!parser.parsePDFObject(&array))
	{
		xwApp->warning(tr("No destination not specified for pdf:dest.\n"));
    	name.free();
    	return  -1;
	}
	else if (!array.isArray())
	{
		xwApp->warning(tr("Destination not specified as an array object!\n"));
    	name.free();
    	array.free();
    	return  -1;
	}
	
	XWString * str = name.getString();
	int error = xref->addNames("Dests", str->getCString(), str->getLength(), &array);
  	name.free();

  	return  error;
}

int XWPDFSpecial::doDocInfo()
{
	if (!(xref->isScanning()))
		return 0;
		
	PDFState *sd = core->getPDFState();
	XWObject dict;
	dict.initNull();
	if (!(sd->parsePDFDict(core, xref, lexer, &dict)))
	{
		xwApp->warning(tr("Dictionary object expected but not found.\n"));
    	return  -1;
	}
	
	XWObject  * docinfo = xref->getDocInfo();
  	docinfo->dictMerge(&dict);
  	dict.free();
  	return  0;
}

int XWPDFSpecial::doDocView()
{
	if (!(xref->isScanning()))
		return 0;
		
	PDFState *sd = core->getPDFState();
	XWObject dict;
	dict.initNull();
	if (!(sd->parsePDFDict(core, xref, lexer, &dict)))
	{
		xwApp->warning(tr("Dictionary object expected but not found.\n"));
    	return  -1;
	}
	
	XWObject * catalog = xref->getCatalog();
	XWObject pref_old, pref_add;
	catalog->dictLookupNF("ViewerPreferences", &pref_old);
  	dict.dictLookupNF("ViewerPreferences", &pref_add);
  	if (!pref_old.isNull() && !pref_add.isNull()) 
  	{
    	pref_old.dictMerge(&pref_add);
    	dict.dictRemove("ViewerPreferences");
  	}
  	
  	catalog->dictMerge(&dict);
  	dict.free();
  	pref_old.free();
  	pref_add.free();

  	return  0;
}

int XWPDFSpecial::doEAnn()
{
	int error = 0;
	if (xref->isScanning())
	{
		PDFState *sd = core->getPDFState();
  		if (sd->annot_dict.isNull()) 
  		{
    		xwApp->warning(tr("tried to end an annotation without starting one!\n"));
    		return  -1;
  		}

		error = endAnnot();
		sd->annot_dict.free();
		sd->annot_dict.initNull();
	}
	else
	{
		error = endAnnot();
	}
  	
  	return error;
}

int XWPDFSpecial::doEColor()
{
	dev->popColor(xref);
  	return  0;
}

int XWPDFSpecial::doEContent()
{
	dev->popCoord();
  	dev->grestore(xref);
  	dev->resetColor(xref, 0);

  	return  0;
}

int XWPDFSpecial::doEForm()
{
	if (xref->isScanning())
	{
		const char * p = lexer->skipWhite();
		const char * endptr = lexer->getEndPtr();
		XWObject attrib;
		attrib.initNull();
		if (p < endptr) 
		{
			XWDVIParser parser(core, xref, lexer, 0);
			if (parser.parsePDFDict(&attrib) && !attrib.isDict())
			{
      			attrib.free();
      			attrib.initNull();
      		}
  		}
  		xref->endGrabbing(dev, &attrib);
  	}
  	else
  		xref->endGrabbing(dev, 0);

  	return  0;
}

int XWPDFSpecial::doEop()
{
	if (!(xref->isScanning()))
		return 0;
		
	const char * p = lexer->skipBlank();
	const char * endptr = lexer->getEndPtr();
	if (p < endptr) 
    	xref->setEopContent(p, (long) (endptr - p));

  	return  0;
}

int XWPDFSpecial::doETrans()
{
	dev->grestore(xref);
  	dev->resetColor(xref, 0);

  	return  0;
}

int XWPDFSpecial::doFStream()
{
	if (!(xref->isScanning()))
		return 0;
	return doStreamWithType(FILE_STREAM);
}

int XWPDFSpecial::doImage()
{
	const char * p = lexer->skipWhite();
	const char * endptr = lexer->getEndPtr();
	char * ident = 0;
	PDFState *sd = core->getPDFState();
	int xobj_id = -1;
	if (p[0] == '@') 
	{
    	ident = lexer->getOptIdent();
    	xobj_id = sd->findResource(ident);
    	if (xobj_id >= 0 && (xref->isScanning())) 
    	{
    		QString msg = QString(tr("Object reference name for image \"%1\" already used.\n")).arg(ident);
      		xwApp->warning(msg);
      		delete [] ident;
      		return  -1;
    	}
  	}
  	
  	TransformInfo ti;
  	transform_info_clear(&ti);
  	long page_no = 1;
  	if (readDimTrns(&ti, &page_no, 0) < 0) 
  	{
    	if (ident)
      		delete [] ident;
    	return  -1;
  	}
  	
  	p = lexer->skipWhite();
  	XWDVIParser parser(core, xref, lexer, 0);
  	XWObject fspec;
  	fspec.initNull();
  	if (!parser.parsePDFObject(&fspec))
  	{
  		xwApp->warning(tr("missing filename string for pdf:image.\n"));
    	if (ident)
      		delete [] ident;
    	return  -1;
  	}
  	else if (!fspec.isString())
  	{
  		fspec.free();
  		xwApp->warning(tr("missing filename string for pdf:image.\n"));
    	if (ident)
      		delete [] ident;
    	return  -1;
  	}
  	
  	if (xobj_id < 0)
  	{
  		p = lexer->skipWhite();  	
  		XWObject attr;
  		attr.initNull();
  		if (p < endptr) 
  		{
    		if (!parser.parsePDFObject(&attr) || !attr.isDict())
    		{
      			xwApp->warning(tr("ignore invalid attribute dictionary."));
      			attr.free();
      			attr.initNull();
      		}
    	}
    	
    	xobj_id = core->findImageResource(fspec.getString()->getCString(), page_no, &attr);
    	
    	if (xobj_id < 0) 
  		{
    		xwApp->warning(tr("Could not find image resource..."));
    		fspec.free();
    		attr.free();
    		if (ident)
    			delete [] ident;
    		return  -1;
  		}
    }

  	if (!(ti.flags & INFO_DO_HIDE))
    	dev->putImage(xref, xobj_id, &ti, x_user, y_user);

  	if (ident && (xref->isScanning())) 
  	{
    	sd->addResource(core, ident, xobj_id);
    	delete [] ident;
  	}

  	fspec.free();

  	return  0;
}

int XWPDFSpecial::doLink()
{
	return resumeAnnot();
}

int XWPDFSpecial::doLiteral()
{
	const char * p = lexer->skipWhite();
	const char * endptr = lexer->getEndPtr();
	int direct = 0;
	while (p < endptr) 
	{
    	if (p + 7 <= endptr && !strncmp(p, "reverse", 7)) 
    	{
      		p = lexer->skip(7);
      		xwApp->warning(tr("The special \"pdf:literal reverse ...\" is no longer supported.\nIgnore the \"reverse\" option.\n"));
    	} 
    	else if (p + 6 <= endptr && !strncmp(p, "direct", 6)) 
    	{
      		direct = 1;
      		p = lexer->skip(6);
    	} 
    	else 
      		break;
      		
    	p = lexer->skipWhite();
  	}

  	if (p < endptr) 
  	{
    	PDFTMatrix M;
    	if (!direct) 
    	{
      		M.a = M.d = 1.0; M.b = M.c = 0.0;
      		M.e = x_user; M.f = y_user;
      		dev->concat(xref, &M);
    	}
    	
    	xref->addPageContent(" ", 1);  /* op: */
    	xref->addPageContent(p, (long) (endptr - p));  /* op: ANY */
    	
    	if (!direct) 
    	{
      		M.e = -x_user; M.f = -y_user;
      		dev->concat(xref, &M);
    	}
  	}

  	return  0;
}

int XWPDFSpecial::doMapFile()
{
	if (!(xref->isScanning()))
		return 0;
		
	const char * p = lexer->skipWhite();
	const char * endptr = lexer->getEndPtr();
	if (p >= endptr)
    	return 0;

	int mode = 0;
  	switch (p[0]) 
  	{
  		case  '-':
    		mode = FONTMAP_RMODE_REMOVE;
    		p = lexer->skip(1);
    		break;
    		
  		case  '+':
    		mode = FONTMAP_RMODE_APPEND;
    		p = lexer->skip(1);
    		break;
    		
  		default:
    		mode = FONTMAP_RMODE_REPLACE;
    		break;
  	}

  	char * mapfile = lexer->getValIdent();
  	int error = 0;
  	XWFontCache fcache(true);
  	if (!mapfile) 
  	{
    	xwApp->warning(tr("no fontmap file specified.\n"));
    	return  -1;
  	} 
  	else 
    	error = fcache.loadFontMapFile(mapfile, mode);
    	
  	delete [] mapfile;

  	return  error;
}

int XWPDFSpecial::doMapLine()
{
	if (!(xref->isScanning()))
		return 0;
		
	const char * p = lexer->skipWhite();
	const char * endptr = lexer->getEndPtr();
	if (p >= endptr) 
	{
    	xwApp->warning(tr("empty mapline special?\n"));
    	return  -1;
  	}
  	
  	char opchr = p[0];
  	if (opchr == '-' || opchr == '+')
    	p = lexer->skip(1);

  	p = lexer->skipWhite();
  	XWTexFontMap *mrec = 0;
  	char        *map_name = 0;
  	int error = 0;
  	XWFontCache fcache(true);
  	switch (opchr)
  	{
  		case  '-':
    		map_name = lexer->getIdent();
    		if (map_name) 
    		{
      			fcache.removeFontmapRecord(map_name);
      			delete [] map_name;
    		} 
    		else 
    		{
      			xwApp->warning(tr("invalid fontmap line: Missing TFM name.\n"));
      			error = -1;
    		}
    		break;
    		
    	case  '+':
    		mrec  = new XWTexFontMap;
    		error = mrec->readLine(p, (long)(endptr - p), XWTexFontMap::isPDFMMapLine(p));
    		if (error)
      			xwApp->warning(tr("invalid fontmap line.\n"));
    		else 
      			fcache.appendFontMapRecord(mrec->map_name, mrec);
    		delete mrec;
    		break;
    		
    	default:
    		mrec  = new XWTexFontMap;
    		error = mrec->readLine(p, (long)(endptr - p), XWTexFontMap::isPDFMMapLine(p));
    		if (error)
      			xwApp->warning(tr("invalid fontmap line.\n"));
    		else 
      			fcache.insertFontMapRecord(mrec->map_name, mrec);
    		delete mrec;
    		break;
  	}
  	
  	return  0;
}

int XWPDFSpecial::doNames()
{
	if (!(xref->isScanning()))
		return 0;
		
	XWDVIParser parser(core, xref, lexer, 0);
	XWObject category;
	if (!parser.parsePDFObject(&category))
	{
		xwApp->warning(tr("PDF name expected but not found.\n"));
    	return  -1;
	}
	else if (!category.isName())
	{
		xwApp->warning(tr("PDF name expected but not found.\n"));
    	category.free();
    	return  -1;
	}
	
	XWObject tmp;
	tmp.initNull();
	if (!parser.parsePDFObject(&tmp))
	{
		xwApp->warning(tr("PDF object expected but not found.\n"));
    	category.free();
    	return  -1;
	}
	else if (tmp.isArray())
	{
		int size = tmp.arrayGetLength();
    	if (size % 2 != 0) 
    	{
      		xwApp->warning(tr("Array size not multiple of 2 for pdf:names.\n"));
      		category.free();
      		tmp.free();
      		return  -1;
    	}
		
		XWObject obj;
    	for (int i = 0; i < size / 2; i++) 
    	{
      		XWObject * key   = tmp.arrayGetOrg(2 * i);
      		XWObject * value = tmp.arrayGetOrg(2 * i + 1);
      		if (!key->isString()) 
      		{
        		xwApp->warning(tr("Name tree key must be string.\n"));
        		category.free();
      			tmp.free();
        		return -1;
      		} 
      		else
			{
				value->copy(&obj);
				XWString * str = key->getString();
				if (xref->addNames(category.getName(), str->getCString(), str->getLength(), &obj) < 0)
				{
        			xwApp->warning(tr("failed to add Name tree entry...\n"));
        			category.free();
      				tmp.free();
        			return -1;
        		}
      		}
    	}
    	tmp.free();
	}
	else if (tmp.isString())
	{
		XWObject value;
		value.initNull();
		if (!parser.parsePDFObject(&value))
		{
			category.free();
      		tmp.free();
      		xwApp->warning(tr("PDF object expected but not found.\n"));
      		return -1;
		}
		
		XWString * str = tmp.getString();
		if (xref->addNames(category.getName(), str->getCString(), str->getLength(), &value) < 0) 
		{
      		xwApp->warning(tr("failed to add Name tree entry...\n"));
      		category.free();
      		tmp.free();
      		return -1;
    	}
    	tmp.free();
	}
	else
	{
		category.free();
      	tmp.free();
    	xwApp->warning(tr("invalid object type for pdf:names.\n"));
    	return  -1;
	}
	
	category.free();

  	return  0;
}

int XWPDFSpecial::doNoLink()
{
	return suspendAnnot();
}

int XWPDFSpecial::doObject()
{
	if (!(xref->isScanning()))
		return 0;
		
	lexer->skipWhite();
	char * ident = lexer->getOptIdent();
  	if (!ident) 
  	{
    	xwApp->warning(tr("could not find a object identifier.\n"));
    	return  -1;
  	}
  	
  	XWDVIParser parser(core, xref, lexer, 0);
  	XWObject object;
  	if (!parser.parsePDFObject(&object))
  	{
  		QString msg = QString(tr("could not find an object definition for \"%1\".\n")).arg(ident);
    	xwApp->warning(msg);
    	delete [] ident;
    	return  -1;
  	} 
  	else 
    	core->pushObject(ident, &object);
    	
  	delete [] ident;

  	return  0;
}

int XWPDFSpecial::doOutline()
{
	if (!(xref->isScanning()))
		return 0;
		
	const char * p = lexer->skipWhite();
	const char * endptr = lexer->getEndPtr();
	int is_open = 0;
	if (p + 3 < endptr && *p == '[') 
	{
    	p = lexer->skip(1);
    	if (*p == '-') 
      		p = lexer->skip(1);
    	else 
      		is_open = 1;
    	p = lexer->skip(1);
  	}
  	
  	p = lexer->skipWhite();
  	XWDVIParser parser(core, xref, lexer, 0);
  	XWObject tmp;
  	tmp.initNull();
  	if (!parser.parsePDFObject(&tmp))
  	{
    	xwApp->warning(tr("missing number for outline item depth.\n"));
    	return  -1;
  	} 
  	else if (!tmp.isNum()) 
  	{
    	tmp.free();
    	xwApp->warning(tr("expecting number for outline item depth.\n"));
    	return  -1;
  	}
  	
  	int level = (int) tmp.getNum();
  	PDFState *sd = core->getPDFState();
  	sd->lowest_level = qMin(sd->lowest_level, level);

	XWObject item_dict;
  	item_dict.initNull();
  	level  +=  1 - sd->lowest_level;
  	if (!(sd->parsePDFDict(core, xref, lexer, &item_dict)))
  	{
  		xwApp->warning(tr("ignoring invalid dictionary.\n"));
    	return  -1;
  	}
  	
  	int current_depth = xref->bookmarksDepth();
  	if (current_depth > level) 
  	{
    	while (current_depth-- > level)
      		xref->bookmarksUp();
  	} 
  	else if (current_depth < level) 
  	{
    	while (current_depth++ < level)
      		xref->bookmarksDown();
  	}

  	xref->bookmarksAdd(&item_dict, is_open);

  	return  0;
}

int XWPDFSpecial::doPut()
{
	if (!(xref->isScanning()))
		return 0;
		
	const char * p = lexer->skipWhite();
	const char * endptr = lexer->getEndPtr();
	char * ident = lexer->getOptIdent();
  	if (!ident) 
  	{
    	xwApp->warning(tr("missing object identifier.\n"));
    	return  -1;
  	}
  	
  	XWObject obj1, obj2;
  	if (!core->lookupObject(ident, &obj1))
  	{
  		QString msg = QString(tr("specified object not exist: %1")).arg(ident);
  		xwApp->warning(msg);
    	delete [] ident;
    	return  -1;
  	}
  	
  	p = lexer->skipWhite();
  	XWDVIParser parser(core, xref, lexer, 0);
  	if (!parser.parsePDFObject(&obj2))
  	{
  		obj1.free();
  		QString msg = QString(tr("missing (an) object(s) to put into \"%1\"!\n")).arg(ident);
  		xwApp->warning(msg);
    	delete [] ident;
    	return  -1;
  	}
  	
  	int error = 0;
  	XWObject obj;
  	switch (obj1.getType())
  	{
  		case  XWObject::Dict:
    		if (!obj2.isDict()) 
    		{
    			QString msg = QString(tr("inconsistent object type for \"put\" (expecting DICT): %1\n")).arg(ident);
  				xwApp->warning(msg);
      			error = -1;
    		} 
    		else 
    		{
      			if (!strcmp(ident, "resources"))
      			{
      				int len = obj2.dictGetLength();
      				for (int i = 0; i < len; i++)
      				{
      					char * key = obj2.dictGetKey(i);
      					obj2.dictGetValNF(i, &obj);
      					error = safePutResdict(key, &obj, &obj1);
      					if (error)
      						break;
      					obj.free();
      				}
        		}
      			else 
        			obj1.dictMerge(&obj2);
    		}
    		break;
    		
    	case  XWObject::Stream:
    		if (obj2.isDict())
    			obj1.streamGetDict()->merge(obj2.getDict());
    		else
    		{
    			QString msg = QString(tr("invalid type: expecting a DICT or STREAM: %1\n")).arg(ident);
  				xwApp->warning(msg);
      			error = -1;
    		}
    		break;
    		
    	case XWObject::Array:
    		obj2.copy(&obj);
    		obj1.arrayAdd(&obj);
    		while (p < endptr) 
    		{
    			if (!parser.parsePDFObject(&obj))
					break;
      			obj1.arrayAdd(&obj);
      			p = lexer->skipWhite();
    		}
    		break;
    		
    	default:    		
    		{
    			QString msg = QString(tr("can't \"put\" object into non-DICT/STREAM/ARRAY type object: %1\n")).arg(ident);
  				xwApp->warning(msg);
    			error = -1;
    		}
    		break;
  	}
  	
  	obj2.free();
  	obj1.free();
  	delete [] ident;

  	return  error;
}

int XWPDFSpecial::doSColor()
{
	XWDVIColor fc, sc;
  	XWDVIColor *pfc, *psc;
	
  	dev->getCurrentColor(&psc, &pfc);
  	int error = readPDFColor(&fc, pfc);
  	if (!error) 
  	{
  		const char * p = lexer->skipWhite();
		const char * endptr = lexer->getEndPtr();
    	if (p < endptr) 
      		error = readPDFColor(&sc, psc);
    	else 
      		sc.copy(&fc);
  	}

  	if (error)
    	xwApp->warning(tr("invalid color specification?\n"));
  	else 
    	dev->setColor(xref, &sc, &fc);

  	return  error;
}

int XWPDFSpecial::doStream()
{
	if (!(xref->isScanning()))
		return 0;
	return doStreamWithType(STRING_STREAM);
}

int XWPDFSpecial::doStreamWithType(int type)
{
	const char * p = lexer->skipWhite();
	char * ident = lexer->getOptIdent();
  	if (!ident) 
  	{
    	xwApp->warning(tr("missing objname for pdf:(f)stream.\n"));
    	return  -1;
  	}
  	
  	p = lexer->skipWhite();
  	XWDVIParser parser(core, xref, lexer, 0);
  	XWObject tmp;
  	if (!parser.parsePDFObject(&tmp))
  	{
  		xwApp->warning(tr("missing input string for pdf:(f)stream.\n"));
    	delete [] ident;
    	return  -1;
  	}
  	else if (!tmp.isString())
  	{
  		xwApp->warning(tr("invalid type of input string for pdf:(f)stream.\n"));
    	tmp.free();
    	delete [] ident;
    	return  -1;
  	}
  	
  	XWString * str = tmp.getString();
  	char * instring = str->getCString();
  	XWObject fstream, dict;  
  	fstream.initNull();	
  	QFile * fp = 0;  	
  	char buf[1024];
  	int nb_read = 0;
  	switch (type)
  	{
  		case FILE_STREAM:
  			if (!instring) 
  			{
      			xwApp->warning(tr("missing filename for pdf:fstream.\n"));
      			tmp.free();
    			delete [] ident;
    			return  -1;
    		}
    		{
    			QTextCodec * codec = QTextCodec::codecForLocale();
				QString fn = codec->toUnicode(instring);
    			XWDocSea sea;
    			fp = sea.openFile(fn);
    			if (!fp) 
    			{
    				QString msg = QString(tr("could not open file: %1\n")).arg(fn);
      				xwApp->warning(msg);
      				tmp.free();
    				delete [] ident;
    				return  -1;
    			}
    		}
    		fstream.initStream(STREAM_COMPRESS, xref);
    		while ((nb_read = fp->read(buf, 1024)) > 0)
      			fstream.streamAdd(buf, nb_read);
    		fp->close();
    		delete fp;
  			break;
  			
  		case STRING_STREAM:
  			fstream.initStream(STREAM_COMPRESS, xref);
    		if (instring)
      			fstream.streamAdd(instring, strlen(instring));
    		break;
    		
    	default:
    		tmp.free();
    		delete [] ident;
    		return -1;
  	}
  	
  	tmp.free();
  	p = lexer->skipWhite();
  	if (p[0] == '<')
  	{
  		if (!parser.parsePDFObject(&tmp))
  		{
  			dict.free();
  			xwApp->warning(tr("parsing dictionary failed.\n"));
      		fstream.free();
      		delete [] ident;
      		return -1;
  		}
  		
  		XWObject obj;
  		obj.initNull();
  		tmp.dictLookupNF("Length", &obj);
  		if (!obj.isNull())
  		{
  			obj.free();
  			tmp.dictRemove("Length");
  		}
  		
  		tmp.dictLookupNF("Filter", &obj);
  		if (!obj.isNull())
  		{
  			obj.free();
  			tmp.dictRemove("Filter");
  		}
  		
  		fstream.streamGetDict()->merge(tmp.getDict());
  		tmp.free();
  	}
  	core->pushObject(ident, &fstream);
  	delete [] ident;

  	return  0;
}

int XWPDFSpecial::doToUnicode()
{
	if (!(xref->isScanning()))
		return 0;
		
	PDFState *sd = core->getPDFState();
	sd->cd.cmap_id = -1;
  	sd->cd.unescape_backslash = 0;
  	
  	const char * p = lexer->skipWhite();
	const char * endptr = lexer->getEndPtr();
	if (p >= endptr) 
	{
    	xwApp->warning(tr("missing CMap name for pdf:tounicode.\n"));
    	return  -1;
  	}
  	
  	char * cmap_name = lexer->getIdent();
  	if (!cmap_name) 
  	{
    	xwApp->warning(tr("missing ToUnicode mapping name...\n"));
    	return -1;
  	}

	XWFontCache fcache(true);
  	sd->cd.cmap_id = fcache.findCMap(cmap_name);
  	if (sd->cd.cmap_id < 0) 
  	{
  		QString msg = QString(tr("failed to load ToUnicode mapping: %1\n")).arg(cmap_name);
    	xwApp->warning(msg);
    	delete [] cmap_name;
    	return -1;
  	}
  	
  	if (sd->cd.cmap_id >= 0) 
  	{
    	if (strstr(cmap_name, "RKSJ") ||
        	strstr(cmap_name, "B5")   ||
        	strstr(cmap_name, "GBK")  ||
        	strstr(cmap_name, "KSC"))
      	sd->cd.unescape_backslash = 1;
  	}
  	delete [] cmap_name;
  	return 0;
}

int XWPDFSpecial::doUXObj()
{
	const char * p = lexer->skipWhite();
	const char * endptr = lexer->getEndPtr();
	char * ident = lexer->getOptIdent();
	if (!ident) 
	{
    	xwApp->warning(tr("no object identifier given.\n"));
    	return  -1;
  	}
  	
  	TransformInfo ti;
  	transform_info_clear(&ti);
  	p = lexer->skipWhite();
  	if (p < endptr) 
  	{
    	if (readDimTrns(&ti, NULL, 0) < 0) 
    	{
      		delete [] ident;
      		return  -1;
    	}
  	}
  
  	PDFState *sd = core->getPDFState();
  	int xobj_id = sd->findResource(ident);
  	if (xobj_id < 0) 
  	{
    	xobj_id = core->findImageResource(ident, 0, 0);
    	if (xobj_id < 0) 
    	{
    		QString msg = QString(tr("specified (image) object doesn't exist: %1\n")).arg(ident);
      		xwApp->warning(msg);
      		delete [] ident;
      		return  -1;
    	}
  	}

  	dev->putImage(xref, xobj_id, &ti, x_user, y_user);
  	delete [] ident;

  	return  0;
}

int XWPDFSpecial::safePutResdent(const char * k, XWObject *vp, XWObject *dp)
{
	XWObject obj;
	obj.initNull();
	dp->dictLookupNF(k, &obj);
	if (!obj.isNull())
		obj.free();
	else
	{
		vp->copy(&obj);
		dp->dictAdd(qstrdup(k), &obj);
	}
	
	return 0;
}

int XWPDFSpecial::safePutResdict(const char * k, XWObject *vp, XWObject *dp)
{
	XWObject obj, dict;
	dp->dictLookupNF(k, &dict);	
	if (vp->isIndirect())
	{
		vp->copy(&obj);
		dp->dictAdd(qstrdup(k), &obj);
	}
	else if (vp->isDict())
	{
		if (!dict.isNull())
		{
			int len = vp->dictGetLength();
			for (int i = 0; i < len; i++)
			{
				char * key = vp->dictGetKey(i);
				vp->dictGetValNF(i, &obj);
				safePutResdent(key, &obj, &dict);
				obj.free();
			}
		}
		else
		{
			vp->copy(&obj);
			dp->dictAdd(qstrdup(k), &obj);
		}
	}
	else
	{
		dict.free();
		QString msg = QString(tr("invalid type (not DICT) for page/form resource dict entry: key=\"%1\"\n")).arg(k);
		xwApp->warning(msg);
    	return  -1;
	}
	
	dict.free();
	return 0;
}

