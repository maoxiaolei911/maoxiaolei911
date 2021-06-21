/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWApplication.h"
#include "XWLexer.h"
#include "XWObject.h"
#include "XWDVIRef.h"
#include "XWDVIDev.h"
#include "XWDVICore.h"
#include "XWPDFNames.h"
#include "XWDVISpecial.h"
#include "XWColorSpecial.h"
#include "XWHTMLSpecial.h"
#include "XWMISCSpecial.h"
#include "XWPDFSpecial.h"
#include "XWPSSpecial.h"

#ifdef XW_BUILD_PS_LIB
#include "XWDviPsSpecial.h"
#endif //XW_BUILD_PS_LIB

#include "XWTPICSpecial.h"

XWDVISpecial::XWDVISpecial(XWDVICore * coreA,
	                       XWDVIRef * xrefA,
	                       XWDVIDev * devA,
	                       const char *buffer, 
	                       long sizeA,
		                   double x_userA, 
		                   double y_userA, 
		                   double magA,
		                   int typeA,
		                   QObject * parent)
	:QObject(parent)
{
	core = coreA;
	xref = xrefA;
	dev = devA;
	lexer = new XWLexer(buffer, sizeA);
	x_user = x_userA;
	y_user = y_userA;
	mag = magA;
	type = typeA;
}

XWDVISpecial::~XWDVISpecial()
{
	if (lexer)
		delete lexer;
}

XWDVISpecial * XWDVISpecial::parse(XWDVICore * coreA,
	                               XWDVIRef * xrefA,
	                               XWDVIDev * devA,
	                               const char *buffer, 
	                               long sizeA,
		                           double x_userA, 
		                           double y_userA, 
		                           double magA)
{
	XWLexer lexerA(buffer, sizeA);
	const char * p = lexerA.skipWhite();	
	const char * endptr = lexerA.getEndPtr();
	XWDVISpecial * ret = 0;
	if (!strncmp(p, "pdf:", 4))
	{
		p = lexerA.skip(4);
		int typeA = XWPDFSpecial::check(&lexerA);
		p = lexerA.getCurPtr();
		if (typeA >= 0)
			ret = new XWPDFSpecial(coreA, xrefA, devA, p, endptr - p, x_userA, y_userA, magA, typeA);
		else
			return 0;
	}
	else if (!strncmp(p, "html:", 5))
	{
		p = lexerA.skip(5);
		int typeA = XWHTMLSpecial::check(&lexerA);
		p = lexerA.getCurPtr();
		if (typeA >= 0)
			ret = new XWHTMLSpecial(coreA, xrefA, devA, p, endptr - p, x_userA, y_userA, magA, typeA);
		else
			return 0;
	}	
#ifdef XW_BUILD_PS_LIB
	else if (!strncmp(p, "ps:", 3) || 
		       !strncmp(p, "header", 6) || 
		       *p =='!' || 
		       (p[0] == '"'))
	{
		int typeA = XWDviPsSpecial::check(&lexerA);
		p = lexerA.getCurPtr();
		if (typeA >= 0)
			ret = new XWDviPsSpecial(coreA, xrefA, devA, p, endptr - p, x_userA, y_userA, magA, typeA);
		else
			return 0;
	}
#else
  else if (!strncmp(p, "ps:", 3) || (p[0] == '"'))
	{
		int typeA = XWPSSpecial::check(&lexerA);
		p = lexerA.getCurPtr();
		if (typeA >= 0)
			ret = new XWPSSpecial(coreA, xrefA, devA, p, endptr - p, x_userA, y_userA, magA, typeA);
    return ret;
	}
#endif //XW_BUILD_PS_LIB
  else if (!strncmp(p, "PSfile", 5) || !strncmp(p, "psfile", 5) || !strncmp(p, "PS:", 3))
  {
  	int typeA = XWPSSpecial::check(&lexerA);
		p = lexerA.getCurPtr();
		if (typeA >= 0)
			ret = new XWPSSpecial(coreA, xrefA, devA, p, endptr - p, x_userA, y_userA, magA, typeA);
    return ret;
  }
	
	if (!ret)
	{
		XWLexer lexert(buffer, endptr);
		p = lexert.skipWhite();	
		int typeA = XWColorSpecial::check(&lexert);
		p = lexert.getCurPtr();	
		if (typeA >= 0)
			ret = new XWColorSpecial(coreA, xrefA, devA, p, endptr - p, x_userA, y_userA, magA, typeA);
	}
	
	if (!ret)
	{
		XWLexer lexert(buffer, endptr);
		p = lexert.skipWhite();	
		int typeA = XWTPICSpecial::check(&lexert);
		p = lexert.getCurPtr();	
		if (typeA >= 0)
			ret = new XWTPICSpecial(coreA, xrefA, devA, p, endptr - p, x_userA, y_userA, magA, typeA);
	}
	
	if (!ret)
	{
		XWLexer lexert(buffer, endptr);
		p = lexert.skipWhite();	
		int typeA = XWMISCSpecial::check(&lexert);
		p = lexert.getCurPtr();	
		if (typeA >= 0)
			ret = new XWMISCSpecial(coreA, xrefA, devA, p, endptr - p, x_userA, y_userA, magA, typeA);
	}
	
	return ret;
}

int XWDVISpecial::beginAnnot(XWObject *dict)
{
	xref->beginAnnot(dict);
  	core->doTagDepth();
  	return  0;
}

int XWDVISpecial::checkResourceStatus(const char *category, const char *resname)
{
	XWObject * dict1 = xref->currentPageResources();
	if (!dict1)
		return 0;
		
  	XWObject dict2;
  	dict1->dictLookupNF(category, &dict2);
  	if (dict2.isDict()) 
  	{
  		XWObject obj;
  		dict2.dictLookupNF(resname, &obj);
    	if (!obj.isNull())
    	{
    		obj.free();
    		dict2.free();
      		return  1;
      	}
      	
      	obj.free();
  	}
  	dict2.free();
  	return  0;
}

XWObject * XWDVISpecial::createXGState(double a, 
	                                   int f_ais, 
	                                   char * resname, 
	                                   XWObject * dict)
{
	XWObject obj;
	dict->initDict(xref);
	obj.initName("ExtGState");
	dict->dictAdd(qstrdup("Type"), &obj);
  	if (f_ais) 
  	{
  		obj.initBool(true);
  		dict->dictAdd(qstrdup("AIS"), &obj);
  	}
  	obj.initReal(a);
  	dict->dictAdd(qstrdup("ca"), &obj);
	xref->addGState(resname, dict);
  	return  dict;
}

int XWDVISpecial::endAnnot()
{
	core->doUntagDepth();
  	xref->endAnnot(dev);
  	return  0;
}

void XWDVISpecial::makeTransmatrix(PDFTMatrix *M,
                                   double xoffset, 
                                   double yoffset,
                                   double xscale,  
                                   double yscale,
                                   double rotate)
{
	double c = cos(rotate);
  	double s = sin(rotate);

  	M->a =  xscale * c; M->b = xscale * s;
  	M->c = -yscale * s; M->d = yscale * c;
  	M->e = xoffset;     M->f = yoffset;
}

int XWDVISpecial::readColorColor(XWDVIColor *colorspec)
{
	double   cv[4];
  	int      nc = 0;
  	int      error = 0;
  	char * q = lexer->getCIdent();
  	if (!q) 
  	{
    	xwApp->warning(tr("no valid color specified?\n"));
    	return  -1;
  	}
  	
  	lexer->skipBlank();
	if (!strcmp(q, "rgb"))
	{
		nc = readNumbers(cv, 3);
    	if (nc != 3) 
    	{
      		xwApp->warning(tr("invalid value for RGB color specification.\n"));
      		error = -1;
    	} 
    	else 
      		colorspec->setRGB(cv[0], cv[1], cv[2]);
	}
	else if (!strcmp(q, "cmyk"))
	{
		nc = readNumbers(cv, 4);
    	if (nc != 4) 
    	{
      		xwApp->warning(tr("invalid value for CMYK color specification.\n"));
      		error = -1;
    	} 
    	else 
      		colorspec->setCMYK(cv[0], cv[1], cv[2], cv[3]);
	}
	else if (!strcmp(q, "gray"))
	{
		nc = readNumbers(cv, 1);
    	if (nc != 1) 
    	{
      		xwApp->warning(tr("invalid value for gray color specification.\n"));
      		error = -1;
    	} 
    	else 
      		colorspec->setGray(cv[0]);
	}
	else if (!strcmp(q, "hsb"))
	{
		nc = readNumbers(cv, 3);
    	if (nc != 3) 
    	{
      		xwApp->warning(tr("invalid value for HSB color specification.\n"));
      		error = -1;
    	} 
    	else 
      		colorspec->setRGBFromHSV(cv[0], cv[1], cv[2]);
	}
	else
	{
		if (colorspec->setName(q) < 0)
		{
			QString msg = QString(tr("unrecognized color name: %1\n")).arg(q);
			xwApp->warning(msg);
		}
	}
	
	delete [] q;

  	return  error;
}

int XWDVISpecial::readColorPDF(XWDVIColor *colorspec)
{
	int isarry = 0;
	double  cv[4];
	const char * p = lexer->skipBlank();
	const char * endptr = lexer->getEndPtr();
	if (p[0] == '[')
	{
		p = lexer->skip(1);
		p = lexer->skipBlank();
		isarry = 1;
	}
	
	int nc = readNumbers(cv, 4);
	char * q = 0;
	int error = 0;
  	switch (nc) 
  	{
  		case  1:
    		colorspec->setGray(cv[0]);
    		break;
    		
  		case  3:
    		colorspec->setRGB (cv[0], cv[1], cv[2]);
    		break;
    		
  		case  4:
    		colorspec->setCMYK(cv[0], cv[1], cv[2], cv[3]);
    		break;
    		
  		default:
    		q = lexer->getCIdent();
    		if (q) 
    		{
      			error = colorspec->setName(q);
      			if (error)
      			{
      				QString msg = QString(tr("unrecognized color name: %1, keep the current color\n")).arg(q);
					xwApp->warning(msg);
        		}
      			delete [] q;
    		} 
    		else 
      			error = -1;
    		break;
  	}

  	if (isarry) 
  	{
    	p = lexer->skipBlank();
    	if (p >= endptr || p[0] != ']') 
    	{
      		xwApp->warning(tr("unbalanced '[' and ']' in color specification.\n"));
      		error = -1;
    	} 
    	else 
      		p = lexer->skip(1);
  	}

  	return  error;
}

int XWDVISpecial::readColorSpec(XWDVIColor *colorspec)
{
	const char * p = lexer->skipBlank();
	const char * endptr = lexer->getEndPtr();
  	if (p >= endptr) 
    	return -1;
    	
  	return readColorColor(colorspec);
}

int XWDVISpecial::readDimTrns(TransformInfo *ti, long *page_no, int syntax)
{
	if (syntax) 
    	return  readDimTrnsDvips(ti);
  	else 
    	return  readDimTrnsPDFM(ti, page_no);

  	return  -1;
}

int XWDVISpecial::readDimTrnsDvips(TransformInfo *t)
{
	static const char *_dtkeys[] = {
#define  K_TRN__HOFFSET  0
#define  K_TRN__VOFFSET  1
    	"hoffset", "voffset",
#define  K_DIM__HSIZE    2
#define  K_DIM__VSIZE    3
    	"hsize", "vsize",
#define  K_TRN__HSCALE   4
#define  K_TRN__VSCALE   5
    	"hscale", "vscale",
#define  K_TRN__ANGLE    6
    	"angle",
#define  K__CLIP         7
    	"clip",
#define  K_DIM__LLX      8
#define  K_DIM__LLY      9
#define  K_DIM__URX     10
#define  K_DIM__URY     11
    	"llx", "lly", "urx", "ury",
#define  K_DIM__RWI     12
#define  K_DIM__RHI     13
    	"rwi", "rhi",
    	NULL
  	};
  	
  	double xoffset, yoffset, xscale, yscale, rotate;
  	int    error  = 0;

  	xoffset = yoffset = rotate = 0.0; xscale = yscale = 1.0;
  	const char * p = lexer->skipBlank();
	const char * endptr = lexer->getEndPtr();
	while (!error && p < endptr)
	{
		char * kp = lexer->getCIdent();
    	if (!kp)
      		break;

		int k = 0;
    	for (; _dtkeys[k] && strcmp(kp, _dtkeys[k]); k++);
    	
    	if (!_dtkeys[k]) 
    	{
    		QString msg = QString(tr("unrecognized dimension/transformation key: %1\n")).arg(kp);
      		xwApp->warning(msg);
      		error = -1;
      		delete [] kp;
      		break;
    	}
    	
    	p = lexer->skipBlank();
    	if (k == K__CLIP) 
    	{
      		t->flags |= INFO_DO_CLIP;
      		delete [] kp;
      		continue; /* not key-value */
    	}
    	
    	if (p < endptr && p[0] == '=') 
    	{
      		p = lexer->skip(1);
      		p = lexer->skipBlank();
    	}
    	
    	char * vp = NULL;
    	if (p[0] == '\'' || p[0] == '\"') 
    	{
      		char  qchr = p[0];
      		p = lexer->skip(1);
      		p = lexer->skipBlank();
      		vp = lexer->getFloatDecimal();
      		p = lexer->skipBlank();
      		if (vp && qchr != p[0]) 
      		{
        		xwApp->warning(tr("syntax error in dimension/transformation specification.\n"));
        		error = -1;
        		delete [] vp; 
        		vp = NULL;
      		}
      		p = lexer->skip(1);
    	} 
    	else 
      		vp = lexer->getFloatDecimal();
      		
      	if (!error && !vp) 
      	{
      		QString msg = QString(tr("Missing value for dimension/transformation: %1\n")).arg(kp);
      		xwApp->warning(msg);
      		error = -1;
    	}
    	
    	delete [] kp;
    	if (!vp || error) 
      		break;
      		
      	switch (k) 
      	{
    		case  K_TRN__HOFFSET:
      			xoffset = atof(vp);
      			break;
      			
    		case  K_TRN__VOFFSET:
      			yoffset = atof(vp);
      			break;
      			
    		case  K_DIM__HSIZE:
      			t->width   = atof(vp);
      			t->flags  |= INFO_HAS_WIDTH;
      			break;
      			
    		case  K_DIM__VSIZE:
      			t->height  = atof(vp);
      			t->flags  |= INFO_HAS_HEIGHT;
      			break;
      			
    		case  K_TRN__HSCALE:
      			xscale  = atof(vp) / 100.0;
      			break;
      			
    		case  K_TRN__VSCALE:
      			yscale  = atof(vp) / 100.0;
      			break;
      			
    		case  K_TRN__ANGLE:
      			rotate  = M_PI * atof(vp) / 180.0;
      			break;
      			
    		case  K_DIM__LLX:
      			t->bbox.llx = atof(vp);
      			t->flags   |= INFO_HAS_USER_BBOX;
      			break;
      			
    		case  K_DIM__LLY:
      			t->bbox.lly = atof(vp);
      			t->flags   |= INFO_HAS_USER_BBOX;
      			break;
      			
    		case  K_DIM__URX:
      			t->bbox.urx = atof(vp);
      			t->flags   |= INFO_HAS_USER_BBOX;
      			break;
      			
    		case  K_DIM__URY:
      			t->bbox.ury = atof(vp);
      			t->flags   |= INFO_HAS_USER_BBOX;
      			break;
      			
    		case  K_DIM__RWI:
      			t->width  = atof(vp) / 10.0;
      			t->flags |= INFO_HAS_WIDTH;
      			break;
      			
    		case  K_DIM__RHI:
      			t->height = atof(vp) / 10.0;
      			t->flags |= INFO_HAS_HEIGHT;
      			break;
      			
    	}
    	p = lexer->skipBlank();
    	delete [] vp;
	}
	
	makeTransmatrix(&(t->matrix), xoffset, yoffset, xscale, yscale, rotate);

  	return  error;
}

int XWDVISpecial::readDimTrnsPDFM(TransformInfo *trans, long *page_no)
{
	const char *_dtkeys[] = {
#define  K_DIM__WIDTH  0
#define  K_DIM__HEIGHT 1
#define  K_DIM__DEPTH  2
    	"width", "height", "depth",
#define  K_TRN__SCALE  3
#define  K_TRN__XSCALE 4
#define  K_TRN__YSCALE 5
#define  K_TRN__ROTATE 6
    	"scale", "xscale", "yscale", "rotate", /* See "Dvipdfmx User's Manual", p.5 */
#define  K_TRN__BBOX   7
    	"bbox",
#define  K_TRN__MATRIX 8
    	"matrix",
#undef  K__CLIP
#define  K__CLIP       9
    	"clip",
#define  K__PAGE       10
    	"page",
#define  K__HIDE       11
    	"hide",
     	NULL
  	};
  	
  	int has_scale, has_xscale, has_yscale, has_rotate, has_matrix;
  	double xscale, yscale, rotate;
  	int    error = 0;

  	has_xscale = has_yscale = has_scale = has_rotate = has_matrix = 0;
  	xscale = yscale = 1.0; rotate = 0.0;
  	trans->flags |= INFO_DO_CLIP;   /* default: do clipping */
  	trans->flags &= ~INFO_DO_HIDE;
  	
  	const char * p = lexer->skipBlank();
	const char * endptr = lexer->getEndPtr();
	
	while (!error && p < endptr)
	{
		char * kp = lexer->getCIdent();
    	if (!kp)
      		break;

    	p = lexer->skipBlank();
    	int k = 0;
    	for (; _dtkeys[k] && strcmp(_dtkeys[k], kp); k++);
    	
    	char * vp = 0;
    	switch (k) 
    	{
    		case  K_DIM__WIDTH:
      			error = lexer->getLength(&trans->width , mag) ? 0 : -1;
      			trans->flags |= INFO_HAS_WIDTH;
      			break;
      			
    		case  K_DIM__HEIGHT:
      			error = lexer->getLength(&trans->height, mag) ? 0 : -1;
      			trans->flags |= INFO_HAS_HEIGHT;
      			break;
      			
    		case  K_DIM__DEPTH:
      			error = lexer->getLength(&trans->depth, mag) ? 0 : -1;
      			trans->flags |= INFO_HAS_HEIGHT;
      			break;
      			
    		case  K_TRN__SCALE:
      			vp = lexer->getFloatDecimal();
      			if (!vp)
        			error = -1;
      			else 
      			{
        			xscale = yscale = atof(vp);
        			has_scale = 1;
        			delete [] vp;
      			}
      			break;
      			
    		case  K_TRN__XSCALE:
      			vp = lexer->getFloatDecimal();
      			if (!vp)
        			error = -1;
      			else 
      			{
        			xscale  = atof(vp);
        			has_xscale = 1;
        			delete [] vp;
      			}
      			break;
      			
    		case  K_TRN__YSCALE:
      			vp = lexer->getFloatDecimal();
      			if (!vp)
        			error = -1;
      			else 
      			{
        			yscale  = atof(vp);
        			has_yscale = 1;
        			delete [] vp;
      			}
      			break;
      			
    		case  K_TRN__ROTATE:
      			vp = lexer->getFloatDecimal();
      			if (!vp)
        			error = -1;
      			else 
      			{
        			rotate = M_PI * atof(vp) / 180.0;
        			has_rotate = 1;
        			delete [] vp;
      			}
      			break;
      			
    		case  K_TRN__BBOX:
      			{
        			double  v[4];
        			if (readNumbers(v, 4) != 4)
          				error = -1;
        			else 
        			{
          				trans->bbox.llx = v[0];
          				trans->bbox.lly = v[1];
          				trans->bbox.urx = v[2];
          				trans->bbox.ury = v[3];
          				trans->flags   |= INFO_HAS_USER_BBOX;
        			}
      			}
      			break;
      			
    		case  K_TRN__MATRIX:
      			{
        			double  v[6];
        			if (readNumbers(v, 6) != 6)
          				error = -1;
        			else 
        			{
          				pdf_setmatrix(&(trans->matrix), v[0], v[1], v[2], v[3], v[4], v[5]);
          				has_matrix = 1;
        			}
      			}
      			break;
      			
    		case  K__CLIP:
      			vp = lexer->getFloatDecimal();
      			if (!vp)
        			error = -1;
      			else 
      			{
					if (atof(vp))
	  					trans->flags |= INFO_DO_CLIP;
					else
	  					trans->flags &= ~INFO_DO_CLIP;
					delete [] vp;
      			}
      			break;
      			
    		case  K__PAGE:
      			{
					double page;
					if (page_no && readNumbers(&page, 1) == 1)
	  					*page_no = (long) page;
					else
	  					error = -1;
      			}
      			break;
      			
    		case  K__HIDE:
      			trans->flags |= INFO_DO_HIDE;
      			break;
      			
    		default:
      			error = -1;
      			break;
    	}
    	
    	if (error)
    	{
      		QString msg = QString(tr("unrecognized key or invalid value for dimension/transformation: %1\n")).arg(kp);
      		xwApp->warning(msg);
      	}
    	else
      		p = lexer->skipBlank();
    	delete [] kp;
	}
	
	if (!error) 
	{
    	if (has_xscale && (trans->flags & INFO_HAS_WIDTH)) 
    	{
      		xwApp->warning(tr("can't supply both width and xscale. Ignore xscale.\n"));
      		xscale = 1.0;
    	} 
    	else if (has_yscale && (trans->flags & INFO_HAS_HEIGHT)) 
    	{
      		xwApp->warning(tr("can't supply both height/depth and yscale. Ignore yscale.\n"));
      		yscale = 1.0;
    	} 
    	else if (has_scale && (has_xscale || has_yscale)) 
    	{
      		xwApp->warning(tr("can't supply overall scale along with axis scales.\n"));
      		error = -1;
    	} 
    	else if (has_matrix && (has_scale || has_xscale || has_yscale || has_rotate)) 
    	{
      		xwApp->warning(tr("can't supply transform matrix along with scales or rotate. Ignore scales and rotate.\n"));
    	}
  	}

  	if (!has_matrix) 
    	makeTransmatrix(&(trans->matrix), 0.0, 0.0, xscale, yscale, rotate);

  	if (!(trans->flags & INFO_HAS_USER_BBOX)) 
    	trans->flags &= ~INFO_DO_CLIP;

  	return  error;
}

int XWDVISpecial::readNumbers(double *values, int num_values)
{
	const char * p = lexer->skipBlank();
	const char * endptr = lexer->getEndPtr();
	int count = 0;
	for (; count < num_values && p < endptr;)
	{
		char * q = lexer->getFloatDecimal();
    	if (!q)
      		break;
    	else 
    	{
      		values[count] = atof(q);
      		delete [] q;
      		p = lexer->skipBlank();
      		count++;
    	}
	}
	
	return  count;
}

int XWDVISpecial::readPDFColor(XWDVIColor *colorspec, XWDVIColor *defaultcolor)
{
	const char * p = lexer->skipBlank();
	const char * endptr = lexer->getEndPtr();
  	if (p >= endptr) 
    	return -1;
    	
    int error = readColorPDF(colorspec);
  	if (error < 0 && defaultcolor) 
  	{
    	colorspec->copy(defaultcolor);
    	error = 0;
  	}
  	
  	return error;
}

int XWDVISpecial::resumeAnnot()
{
	core->doLinkAnnot(1);
  	return  0;
}

int XWDVISpecial::suspendAnnot()
{
	core->doLinkAnnot(0);
  	return  0;
}

