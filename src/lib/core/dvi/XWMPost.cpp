/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QString>
#include <QByteArray>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <ctype.h>
#include "XWLexer.h"
#include "XWSubfont.h"
#include "XWTFMFile.h"
#include "XWTexFontMap.h"
#include "XWFontCache.h"
#include "XWObject.h"
#include "XWDVIRef.h"
#include "XWDVICore.h"
#include "XWDVIDev.h"
#include "XWDVIParser.h"
#include "XWMPost.h"

static struct mp_font
{
  char   *font_name;
  int     font_id;
  int     tfm_id;     /* Used for text width calculation */
  int     subfont_file_id;
  int     subfont_id;
  double  pt_size;
} font_stack[PDF_GSAVE_MAX] = {
  {NULL, -1, -1, -1, -1, 0}
};
static int currentfont = -1;

#define CURRENT_FONT() ((currentfont < 0) ? NULL : &font_stack[currentfont])

/* Compatibility */
#define MP_CMODE_MPOST    0
#define MP_CMODE_DVIPSK   1
#define MP_CMODE_PTEXVERT 2
static int mp_cmode = MP_CMODE_MPOST;

#define DEVICE_RESOLUTION 1000

#define ADD          	1
#define SUB		2
#define MUL		3
#define DIV		4
#define NEG    	        5
#define TRUNCATE	6

#define CLEAR		10
#define EXCH		11
#define POP		12

#define NEWPATH		31
#define CLOSEPATH    	32
#define MOVETO		33
#define RMOVETO         34
#define CURVETO   	35
#define RCURVETO        36
#define LINETO		37
#define RLINETO		38
#define ARC             39
#define ARCN            40

#define FILL		41
#define STROKE		42
#define SHOW		43

#define CLIP         	44
#define EOCLIP         	45

#define SHOWPAGE	49

#define GSAVE		50
#define GRESTORE	51

#define CONCAT       	52
#define SCALE		53
#define TRANSLATE	54
#define ROTATE          55

#define SETLINEWIDTH	60
#define SETDASH		61
#define SETLINECAP 	62
#define SETLINEJOIN	63
#define SETMITERLIMIT	64

#define SETGRAY		70
#define SETRGBCOLOR	71
#define SETCMYKCOLOR	72

#define CURRENTPOINT    80
#define IDTRANSFORM	81
#define DTRANSFORM	82

#define FINDFONT        201
#define SCALEFONT       202
#define SETFONT         203
#define CURRENTFONT     204

#define STRINGWIDTH     210

#define DEF             999

#define FSHOW		1001
#define STEXFIG         1002
#define ETEXFIG         1003
#define HLW             1004
#define VLW             1005
#define RD              1006
#define B               1007

static struct operators 
{
  	const char *token;
  	int         opcode;
} ps_operators[] = {
  	{"add",          ADD},
  	{"mul",          MUL},
  	{"div",          DIV},
  	{"neg",          NEG},
  	{"sub",          SUB},  
  	{"truncate",     TRUNCATE},

  	{"clear",        CLEAR},
  	{"exch",         EXCH},
  	{"pop",          POP},

  	{"clip",         CLIP},
  	{"eoclip",       EOCLIP},
  	{"closepath",    CLOSEPATH},
  	{"concat",       CONCAT},

  	{"newpath",      NEWPATH},
  	{"moveto",       MOVETO},
  	{"rmoveto",      RMOVETO},
  	{"lineto",       LINETO},
  	{"rlineto",      RLINETO},
  	{"curveto",      CURVETO},
  	{"rcurveto",     RCURVETO},
  	{"arc",          ARC},
  	{"arcn",         ARCN},

  	{"stroke",       STROKE},  
  	{"fill",         FILL},
  	{"show",         SHOW},
  	{"showpage",     SHOWPAGE},

  	{"gsave",        GSAVE},
  	{"grestore",     GRESTORE},
  	{"translate",    TRANSLATE},
  	{"rotate",       ROTATE},
  	{"scale",        SCALE},

  	{"setlinecap",    SETLINECAP},
  	{"setlinejoin",   SETLINEJOIN},
  	{"setlinewidth",  SETLINEWIDTH},
  	{"setmiterlimit", SETMITERLIMIT},
  	{"setdash",       SETDASH},

  	{"setgray",      SETGRAY},
  	{"setrgbcolor",  SETRGBCOLOR},
  	{"setcmykcolor", SETCMYKCOLOR},

  	{"currentpoint", CURRENTPOINT}, /* This is here for rotate support
				     in graphics package-not MP support */
  	{"dtransform",   DTRANSFORM},
  	{"idtransform",  IDTRANSFORM},

  	{"findfont",     FINDFONT},
  	{"scalefont",    SCALEFONT},
  	{"setfont",      SETFONT},
  	{"currentfont",  CURRENTFONT},

  	{"stringwidth",  STRINGWIDTH},

  	{"def", DEF} /* not implemented yet; just work with mptopdf */
};

static struct operators mps_operators[] = {
  	{"fshow",       FSHOW}, /* exch findfont exch scalefont setfont show */
  	{"startTexFig", STEXFIG},
  	{"endTexFig",   ETEXFIG},
  	{"hlw",         HLW}, /* 0 dtransform exch truncate exch idtransform pop setlinewidth */
  	{"vlw",         VLW}, /* 0 exch dtransform truncate idtransform pop setlinewidth pop */
  	{"l",           LINETO},
  	{"r",           RLINETO},
  	{"c",           CURVETO},
  	{"m",           MOVETO},
  	{"p",           CLOSEPATH},
  	{"n",           NEWPATH},
  	{"C",           SETCMYKCOLOR},
  	{"G",           SETGRAY},
  	{"R",           SETRGBCOLOR},
  	{"lj",          SETLINEJOIN},
  	{"ml",          SETMITERLIMIT},
  	{"lc",          SETLINECAP},
  	{"S",           STROKE},
  	{"F",           FILL},
  	{"q",           GSAVE},
  	{"Q",           GRESTORE},
  	{"s",           SCALE},
  	{"t",           CONCAT},
  	{"sd",          SETDASH},
  	{"rd",          RD}, /* [] 0 setdash */
  	{"P",           SHOWPAGE},
  	{"B",           B}, /* gsave fill grestore */
  	{"W",           CLIP}
};

#define NUM_PS_OPERATORS  (sizeof(ps_operators)/sizeof(ps_operators[0]))
#define NUM_MPS_OPERATORS (sizeof(mps_operators)/sizeof(mps_operators[0]))

#define PS_STACK_SIZE 1024

XWObject * XWMPost::stack = 0;
unsigned XWMPost::top_stack = 0;

XWMPost::XWMPost(XWDVIRef * xref,
	             const char *identA, 
	             int  subtypeA,
	             int  formatA,
	             long page_noA, 	           
	             const QString & filenameA,
	             XWObject *dictA)
	:XWDVIImage(xref, identA, subtypeA, formatA, page_noA, filenameA, dictA)
{
	init();
}

int XWMPost::checkForMP(QIODevice *fp)
{
	fp->seek(0);
	char buf[1024];
	fp->readLine(buf, 1024);
  	if (strncmp(buf, "%!PS", 4))
    	return 0;
    	
    int try_count = 10;
    while (try_count > 0) 
    {
    	fp->readLine(buf, 1024);
    	if (!strncmp(buf, "%%Creator:", 10)) 
    	{
      		if (strlen(buf + 10) >= 8 && strstr(buf + 10, "MetaPost"))
				break;
    	}
    	try_count--;
  	}

  	return ((try_count > 0) ? 1 : 0);
}

int XWMPost::doPage(XWDVICore * core,
	                XWDVIRef * xref, 
	                XWDVIDev * dev,
	                QIODevice *image_file)
{
	init();
	
	image_file->seek(0);
	long size = 0;
	if ((size = image_file->size()) == 0) 
    	return -1;

  	char * buffer = new char[size+1];
  	image_file->read(buffer, size);
  	buffer[size] = 0;
  	char * start = buffer;
  	char * end   = buffer + size;
	PDFRect  bbox;
  	int error = scanBBox(&start, end, &bbox);
  	if (error) 
  	{
    	delete [] buffer;
    	return -1;
  	}

  	mp_cmode = MP_CMODE_MPOST;
  	
  	xref->beginPage(dev, 1.0, 0.0, 0.0); 
  	xref->setMediaBox(xref->currentPageNumber(), &bbox);
  	int dir_mode = dev->getDirMode();
  	dev->setAutoRotate(0);
	XWLexer lexer(start, end);
  	skipProlog(&lexer);

  	error = parseBody(core, xref, dev, &lexer, 0.0, 0.0);

  	dev->setAutoRotate(1);
  	dev->setDirMode(dir_mode);

  	xref->endPage(core, dev);

  	delete [] buffer;
  	return (error ? -1 : 0);
}

void XWMPost::eopCleanup(XWDVIRef * xref)
{
	clearFonts();
  	doClear(xref);
}

int XWMPost::execInline(XWDVICore * core,
	                    XWDVIRef * xref, 
	                    XWDVIDev * dev,
	                    char **p, 
	                    char *endptr,
		 				double x_user, 
		 				double y_user)
{
	init();
	
	int dirmode = dev->getDirMode();
  	if (dirmode) 
    	mp_cmode = MP_CMODE_PTEXVERT;
  	else 
    	mp_cmode = MP_CMODE_DVIPSK;
    	
    int autorotate = dev->getParam(PDF_DEV_PARAM_AUTOROTATE);
  	dev->setParam(PDF_DEV_PARAM_AUTOROTATE, 0);
  	dev->moveTo(x_user, y_user);
  	XWLexer lexer((const char*)(*p), endptr);
  	int error = parseBody(core, xref, dev, &lexer, x_user, y_user);
  	*p = (char*)(lexer.getCurPtr());
  	
  	dev->setParam(PDF_DEV_PARAM_AUTOROTATE, autorotate);
  	dev->setDirMode(dirmode);

  	return error;
}

int XWMPost::load(XWDVICore * core,
	              XWDVIRef * xref, 
	              XWDVIDev * dev,
	              QIODevice *fp)
{
	fp->seek(0);
	long length = fp->size();
  	if (length < 1) 
    	return -1;
    	
    char * buffer = new char[length + 1];
  	char * p      = buffer;
  	char * endptr = p + length;
  	endptr[0] = '\0';
  	while (length > 0) 
  	{
    	long nb_read = fp->read(buffer, length);
    	if (nb_read < 0) 
    	{
      		delete [] buffer;
      		return -1;
    	}
    	length -= nb_read;
  	}
  	
  	XWDVIXFormInfo info;
  	int error = scanBBox(&p, endptr, &(info.bbox));
  	if (error)
  	{
  		delete [] buffer;
    	return -1;
  	}
  	
  	XWLexer lexer((const char*)p, (const char*)endptr);
  	skipProlog(&lexer);
  	int dirmode    = dev->getDirMode();
  	int autorotate = dev->getParam(PDF_DEV_PARAM_AUTOROTATE);
  	dev->setParam(PDF_DEV_PARAM_AUTOROTATE, 0);
  	int form_id  = xref->beginGrabbing(core, dev, ident, 0.0, 0.0, &(info.bbox));

  	mp_cmode = MP_CMODE_MPOST;
  	int gs_depth = dev->currentDepth();
  	int st_depth = stackDepth();
  	dev->pushGState();

  	error = parseBody(core, xref, dev, &lexer, 0.0, 0.0);
  	delete [] buffer;
  	
  	if (error) 
    	form_id = -1;
  	
  	dev->popGState();
  	stackClearTo(xref, st_depth);
  	dev->grestoreTo(xref, gs_depth);

  	xref->endGrabbing(dev, NULL);

  	dev->setParam(PDF_DEV_PARAM_AUTOROTATE, autorotate);
  	dev->setDirMode(dirmode);

  	return form_id;
}

void XWMPost::quit()
{
	if (stack)
		free(stack);
}

int XWMPost::scanBBox(char **pp, char *endptr, PDFRect *bbox)
{
	init();
	
	while (*pp < endptr && isspace(**pp))
    	(*pp)++;
    
	XWLexer lexer(*pp, endptr);
	char * p = *pp;
	char  *number = 0;
  	double values[4];
	while (p < endptr && *p == '%')
	{
		if (p + 14 < endptr && !strncmp(p, "%%BoundingBox:", 14))
		{
			p = (char*)(lexer.skip(14));
			int i = 0;
			for (; i < 4; i++) 
			{
				p = (char*)(lexer.skipWhite());
				number = lexer.getNumber();
				if (!number) 
	  				break;
				values[i] = atof(number);
				delete [] number;
      		}
      		
      		if (i < 4)
      		{
      			*pp = (char*)(lexer.getCurPtr());
      			return -1;
      		}
      		
      		bbox->llx = values[0];
			bbox->lly = values[1];
			bbox->urx = values[2];
			bbox->ury = values[3];
			
			*pp = (char*)(lexer.getCurPtr());
			return 0;
		}
		
		p = (char*)(lexer.skipLine());
		while (p < endptr && isspace(*p))
      		p = (char*)(lexer.skip(1));
	}
	
	*pp = (char*)(lexer.getCurPtr());
	return -1;
}

void XWMPost::clearFonts()
{
	while (currentfont >= 0) 
	{
    	if (font_stack[currentfont].font_name)
      		delete [] (font_stack[currentfont].font_name);
      	font_stack[currentfont].font_name = 0;
    	currentfont--;
  	}
}

int XWMPost::cvrArray(XWDVIRef * xref ,
	                  XWObject *array, 
	                  double *values, 
	                  int count)
{
	if (array && array->isArray())
	{
		XWObject obj;
		while (count-- > 0) 
		{
      		array->arrayGetNF(count, &obj);
      		if (!obj.isNum()) 
      		{
      			obj.free();
				break;
			}
      		values[count] = obj.getNum();
    	}
	}
	
	if (array)
    	xref->releaseObj(array);

  	return (count + 1);
}

int XWMPost::devCTM(XWDVIDev * dev, PDFTMatrix *M)
{
	dev->currentMatrix(M);
  	M->a *= DEVICE_RESOLUTION; M->b *= DEVICE_RESOLUTION;
  	M->c *= DEVICE_RESOLUTION; M->d *= DEVICE_RESOLUTION;
  	M->e *= DEVICE_RESOLUTION; M->f *= DEVICE_RESOLUTION;

  	return 0;
}

int XWMPost::doClear(XWDVIRef * xref)
{
	while (top_stack > 0) 
	{
    	XWObject * tmp = pop();
    	if (tmp)
      		xref->releaseObj(tmp);
  	}

  	return 0;
}

int XWMPost::doCurrentFont(XWDVIRef * xref)
{
	mp_font * font = CURRENT_FONT();
  	if (!font || !font->font_name) 
    	return 1;
    	
    int error = 0;
    XWObject font_dict, obj;
    font_dict.initDict(xref);
    obj.initName("Font");
    font_dict.dictAdd(qstrdup("Type"), &obj);
    obj.initName(font->font_name);
    font_dict.dictAdd(qstrdup("FontName"), &obj);
    obj.initReal(font->pt_size);
    font_dict.dictAdd(qstrdup("FontScale"), &obj);
    if (top_stack < PS_STACK_SIZE) 
    {
    	stack[top_stack].free();
    	stack[top_stack++] = font_dict;
  	}
    else 
    {
      	font_dict.free();
      	error = 1;
    }
    
    return error;
}

int XWMPost::doExch()
{
	if (top_stack < 2)
    	return -1;
    	
    XWObject tmp = stack[top_stack-1];
  	stack[top_stack-1] = stack[top_stack-2];
  	stack[top_stack-2] = tmp;

  	return 0;
}

int  XWMPost::doFindFont(XWDVIRef * xref)
{
	int error = 0;
	XWObject * font_name = pop();
  	if (!font_name)
    	return 1;
    else if (font_name->isString() || font_name->isName())
    {
    	XWObject font_dict, obj;
    	font_dict.initDict(xref);
    	obj.initName("Font");
    	font_dict.dictAdd(qstrdup("Type"), &obj);
    	if (font_name->isString()) 
    	{
    		obj.initString(new XWString(font_name->getString()));
    		font_dict.dictAdd(qstrdup("FontName"), &obj);
      		xref->releaseObj(font_name);
    	} 
    	else 
    	{
    		obj.initName(font_name->getName());
    		font_dict.dictAdd(qstrdup("FontName"), &obj);
    	}
    	obj.initReal(1.0);
    	font_dict.dictAdd(qstrdup("FontScale"), &obj);
    	if (top_stack < PS_STACK_SIZE) 
    	{
    		stack[top_stack].free();
      		stack[top_stack++] = font_dict;
      	}
    	else 
    	{
      		font_dict.free();
      		error = 1;
    	}
    }
    else
    	error = 1;
    	
    return error;
}

int XWMPost::doMpostBindDef(XWDVICore * core,
	                        XWDVIRef * xref, 
	                        XWDVIDev * dev,
	                        const char *ps_code, 
	                        double x_user, 
	                        double y_user)
{
	XWLexer lexer(ps_code);
	return parseBody(core, xref, dev, &lexer, x_user, y_user);
}

int XWMPost::doOperator(XWDVICore * core,
	                    XWDVIRef * xref, 
	                    XWDVIDev * dev,
	                    const char *token, 
	                    double x_user, 
	                    double y_user)
{
	int error  = 0;
	int opcode = getOpCode(token);
	double  values[12];
	PDFTMatrix matrix;
  	PDFCoord   cp;
  	XWDVIColor color;
  	XWObject obj, *tmp;
	switch (opcode)
	{
		case ADD:
			error = popGetNumbers(xref, values, 2);
    		if (!error)
    		{
    			obj.initReal(values[0] + values[1]);
      			error = push(&obj);
      		}
    		break;
    		
    	case MUL:
    		error = popGetNumbers(xref, values, 2);
    		if (!error)
    		{
    			obj.initReal(values[0]*values[1]);
      			error = push(&obj);
    		}
    		break;
    		
  		case NEG:
    		error = popGetNumbers(xref, values, 1);
    		if (!error)
    		{
    			obj.initReal(-values[0]);
      			error = push(&obj);
      		}
    		break;
    		
  		case SUB:
    		error = popGetNumbers(xref, values, 2);
    		if (!error)
    		{
    			obj.initReal(values[0] - values[1]);
      			error = push(&obj);
      		}
    		break;
    		
  		case DIV:
    		error = popGetNumbers(xref, values, 2);
    		if (!error)
    		{
    			obj.initReal(values[0]/values[1]);
      			error = push(&obj);
      		}
    		break;
    		
  		case TRUNCATE: /* Round toward zero. */
    		error = popGetNumbers(xref, values, 1);
    		if (!error)
    		{
    			obj.initReal((values[0] > 0) ? floor(values[0]) : ceil(values[0]));
      			error = push(&obj);
      		}
    		break;
    		
    	case CLEAR:
    		error = doClear(xref); 
    		break;
    		
  		case POP:
    		tmp = pop();
    		if (tmp)
      			xref->releaseObj(tmp);
    		break;
    		
  		case EXCH:
    		error = doExch();  
    		break;
    		
    	case MOVETO:
    		error = popGetNumbers(xref, values, 2);
    		if (!error)
      			error = dev->moveTo(values[0], values[1]);
    		break;
    		
  		case RMOVETO:
    		error = popGetNumbers(xref, values, 2);
    		if (!error)
      			error = dev->rmoveTo(values[0], values[1]);
    		break;
    		
  		case LINETO:
    		error = popGetNumbers(xref, values, 2);
    		if (!error)
      			error = dev->lineTo(values[0], values[1]);
    		break;
    		
  		case RLINETO:
    		error = popGetNumbers(xref, values, 2);
    		if (!error)
      			error = dev->rlineTo(values[0], values[1]);
    		break;
    		
  		case CURVETO:
    		error = popGetNumbers(xref, values, 6);
    		if (!error)
      			error = dev->curveTo(values[0], values[1], values[2], values[3], values[4], values[5]);
    		break;
    		
  		case RCURVETO:
    		error = popGetNumbers(xref, values, 6);
    		if (!error)
      			error = dev->rcurveTo(values[0], values[1], values[2], values[3], values[4], values[5]);
    		break;
    		
  		case CLOSEPATH:
    		error = dev->closePath();
    		break;
    		
  		case ARC:
    		error = popGetNumbers(xref, values, 5);
    		if (!error)
      			error = dev->arc(values[0], values[1], values[2], values[3], values[4]);
    		break;
    		
  		case ARCN:
    		error = popGetNumbers(xref, values, 5);
    		if (!error)
      			error = dev->arcn(values[0], values[1], values[2], values[3], values[4]);
    		break;
    
  		case NEWPATH:
    		dev->newPath(xref);
    		break;
    		
  		case STROKE:
    		/* fill rule not supported yet */
    		dev->flushPath(xref, 'S', PDF_FILL_RULE_NONZERO);
    		break;
    		
  		case FILL:
    		dev->flushPath(xref, 'f', PDF_FILL_RULE_NONZERO);
    		break;

  		case CLIP:
    		error = dev->clip(xref);
    		break;
    		
  		case EOCLIP:
    		error = dev->eoclip(xref);
    		break;
    		
		case GSAVE:
    		error = dev->gsave(xref);
    		saveFont();
    		break;
    		
  		case GRESTORE:
    		error = dev->grestore(xref);
    		restoreFont();
    		break;
    		
    	case CONCAT:
    		tmp   = pop();
    		error = cvrArray(xref, tmp, values, 6);
    		tmp   = NULL;
    		if (!error)
    		{
      			pdf_setmatrix(&matrix, values[0], values[1], values[2], values[3], values[4], values[5]);
      			error = dev->concat(xref, &matrix);
    		}
    		break;
    		
    	case SCALE:
    		error = popGetNumbers(xref, values, 2);
    		if (!error)
    		{
    			switch (mp_cmode)
    			{
    				case MP_CMODE_PTEXVERT:
						pdf_setmatrix(&matrix, values[1], 0.0, 0.0, values[0], 0.0, 0.0);
						break;
						
					default:
						pdf_setmatrix(&matrix, values[0], 0.0, 0.0, values[1], 0.0, 0.0);
						break;
    			}
    			
    			error = dev->concat(xref, &matrix);
    		}
    		break;
    		
    	case ROTATE:
    		error = popGetNumbers(xref, values, 1);
    		if (!error)
    		{
    			values[0] = values[0] * M_PI / 180;
      			switch (mp_cmode)
      			{
      				case MP_CMODE_DVIPSK:
      				case MP_CMODE_MPOST:
      				case MP_CMODE_PTEXVERT:
      					pdf_setmatrix(&matrix, cos(values[0]), 
      					              -sin(values[0]), 
      					              sin(values[0]),  
      					              cos(values[0]),
		                              0.0, 0.0);
	                    break;
	                    
	                default:
						pdf_setmatrix(&matrix,
		      						  cos(values[0]), 
		      						  sin(values[0]),
		                              -sin(values[0]), 
		                              cos(values[0]),
		      						  0.0, 0.0);
						break;
      			}
      			
      			error = dev->concat(xref, &matrix);
    		}
    		break;
    		
    	case TRANSLATE:
    		error = popGetNumbers(xref, values, 2);
    		if (!error) 
    		{
      			pdf_setmatrix(&matrix, 1.0, 0.0, 0.0, 1.0, values[0], values[1]);
      			error = dev->concat(xref, &matrix);
    		}
    		break;

  		case SETDASH:
    		error = popGetNumbers(xref, values, 1);
    		if (!error) 
    		{
      			double dash_values[PDF_DASH_SIZE_MAX];
      			double offset  = values[0];
      			XWObject * pattern = pop();
      			if (!pattern->isArray()) 
      			{
					if (pattern)
	  					xref->releaseObj(pattern);
					error = 1;
					break;
      			}
      			int num_dashes = pattern->arrayGetLength();
      			if (num_dashes > PDF_DASH_SIZE_MAX) 
      			{
					xref->releaseObj(pattern);
					error = 1;
					break;
      			}
      			XWObject dash;
      			for (int i = 0; i < num_dashes && !error ; i++) 
      			{
					pattern->arrayGetNF(i, &dash);
					if (!dash.isNum())
	  					error = 1;
					else 
	  					dash_values[i] = dash.getNum();
	  				dash.free();
      			}
      			
      			xref->releaseObj(pattern);
      			if (!error) 
					error = dev->setLineDash(xref, num_dashes, dash_values, offset);
    		}
    		break;
    		
  		case SETLINECAP:
    		error = popGetNumbers(xref, values, 1);
    		if (!error)
      			error = dev->setLineCap(xref, (int)values[0]);
    		break;
    		
  		case SETLINEJOIN:
    		error = popGetNumbers(xref, values, 1);
    		if (!error)
      			error = dev->setLineJoin(xref, (int)values[0]);
    		break;
    		
  		case SETLINEWIDTH:
    		error = popGetNumbers(xref, values, 1);
    		if (!error)
      			error = dev->setLineWidth(xref, values[0]);
    		break;
    		
  		case SETMITERLIMIT:
    		error = popGetNumbers(xref, values, 1);
    		if (!error)
      			error = dev->setMiterLimit(xref, values[0]);
    		break;

  		case SETCMYKCOLOR:
    		error = popGetNumbers(xref, values, 4);
    		/* Not handled properly */
    		if (!error) 
    		{
      			color.setCMYK(values[0], values[1], values[2], values[3]);
      			dev->setStrokingColor(xref, &color);
      			dev->setNonStrokingColor(xref, &color);
    		}
    		break;
    		
  		case SETGRAY:
    		/* Not handled properly */
    		error = popGetNumbers(xref, values, 1);
    		if (!error) 
    		{
      			color.setGray(values[0]);
      			dev->setStrokingColor(xref, &color);
      			dev->setNonStrokingColor(xref, &color);
    		}
    		break;
    		
  		case SETRGBCOLOR:
    		error = popGetNumbers(xref, values, 3);
    		if (!error) 
    		{
      			color.setRGB(values[0], values[1], values[2]);
      			dev->setStrokingColor(xref, &color);
      			dev->setNonStrokingColor(xref, &color);
    		}
    		break;
    		
    	case SHOWPAGE:
    		break;
    		
    	case CURRENTPOINT:
    		error = dev->currentPoint(&cp);
    		if (!error) 
    		{
    			obj.initReal(cp.x);
    			push(&obj);
    			obj.initReal(cp.y);
    			push(&obj);
    		}
    		break;
    		
    	case DTRANSFORM:
    		{
      			int  has_matrix = 0;

      			tmp = pop();
      			if (tmp && tmp->isArray()) 
      			{
					error = cvrArray(xref, tmp, values, 6); /* This does pdf_release_obj() */
					tmp   = NULL;
					if (error)
	  					break;
					pdf_setmatrix(&matrix, values[0], values[1],values[2], values[3],values[4], values[5]);
					tmp = pop();
					has_matrix = 1;
      			}
      
      			if (!tmp || !tmp->isNum()) 
      			{
					error = 1;
					break;
      			}
      			cp.y = tmp->getNum();
      			xref->releaseObj(tmp);

      			tmp = pop();
      			if (!tmp || !tmp->isNum()) 
      			{
					error = 1;
					break;
      			}
      			cp.x = tmp->getNum();
      			xref->releaseObj(tmp);

      			if (!has_matrix) 
					devCTM(dev, &matrix);
					
      			dev->dtransform(&cp, &matrix);
      			obj.initReal(cp.x);
      			push(&obj);
      			obj.initReal(cp.y);
      			push(&obj);
    		}
    		break;
    		
    	case IDTRANSFORM:
    		{
      			int  has_matrix = 0;

      			tmp = pop();
      			if (tmp && tmp->isArray()) 
      			{
					error = cvrArray(xref, tmp, values, 6); /* This does pdf_release_obj() */
					tmp   = 0;
					if (error)
	  					break;
					pdf_setmatrix(&matrix, values[0], values[1], values[2], values[3], values[4], values[5]);
					tmp = pop();
					has_matrix = 1;
      			}
      
      			if (!tmp || !tmp->isNum()) 
      			{
					error = 1;
					break;
      			}
      			cp.y = tmp->getNum();
      			xref->releaseObj(tmp);

      			tmp = pop();
      			if (!tmp || !tmp->isNum()) 
      			{
					error = 1;
					break;
      			}
      			cp.x = tmp->getNum();
      			xref->releaseObj(tmp);

      			if (!has_matrix) 
					devCTM(dev, &matrix);
      			dev->idtransform(&cp, &matrix);
      			obj.initReal(cp.x);
      			push(&obj);
      			obj.initReal(cp.y);
      			push(&obj);
      			break;
    		}
    		
    	case FINDFONT:
    		error = doFindFont(xref);
    		break;
    			
  		case SCALEFONT:
    		error = doScaleFont(xref);
    		break;
    			
  		case SETFONT:
    		error = doSetFont(xref, dev);
    		break;
    			
  		case CURRENTFONT:
    		error = doCurrentFont(xref);
    		break;

  		case SHOW:
    		error = doShow(xref, dev);
    		break;

  		case STRINGWIDTH:
    		error = 1;
    		break;
    			
    	case FSHOW:
    		error = doMpostBindDef(core, xref, dev, "exch findfont exch scalefont setfont show", x_user, y_user);
    		break;
    		
  		case STEXFIG:
  		case ETEXFIG:
    		error = doTexFigOperator(core, xref, dev, opcode, x_user, y_user);
    		break;
    		
  		case HLW:
    		error = doMpostBindDef(core, xref, dev, "0 dtransform exch truncate exch idtransform pop setlinewidth", x_user, y_user);
    		break;
    		
  		case VLW:
    		error = doMpostBindDef(core, xref, dev, "0 exch dtransform truncate idtransform setlinewidth pop", x_user, y_user);
    		break;
    		
  		case RD:
    		error = doMpostBindDef(core, xref, dev, "[] 0 setdash", x_user, y_user);
    		break;
    		
  		case B:
    		error = doMpostBindDef(core, xref, dev, "gsave fill grestore", x_user, y_user);
    		break;

  		case DEF:
    		tmp = pop();
    		tmp = pop();
    		/* do nothing; not implemented yet */
    		break;

  		default:
    		if (isFontName(token)) 
    		{
    			obj.initName(token);
    			push(&obj);
    		} 
    		else 
      			error = 1;
    		break;
	}
	
	return error;
}

int XWMPost::doScaleFont(XWDVIRef * xref)
{
	double   scale;
	int error = popGetNumbers(xref, &scale, 1);
  	if (error)
    	return error;
    	
    XWObject * font_dict = pop();
  	if (!font_dict)
    	error = 1;
  	else if (isFontDict(font_dict)) 
  	{
  		XWObject * font_scale = font_dict->dictLookupNFOrg("FontScale");
  		if (font_scale)
  		{
  			double tmps = font_scale->getNum() * scale;
  			font_scale->initReal(tmps);
  		}
  		else
  		{
  			XWObject obj;
  			obj.initReal(scale);
  			font_dict->dictAdd(qstrdup("FontScale"), &obj);
  		}
  			
    	if (top_stack < PS_STACK_SIZE) 
    	{
    		stack[top_stack].free();
      		stack[top_stack++] = *font_dict;
      	}
    	else 
    	{
      		xref->releaseObj(font_dict);
      		error = 1;
    	}
  	} 
  	else 
    	error = 1;

  	return error;
}

int  XWMPost::doSetFont(XWDVIRef * xref, 
	                    XWDVIDev * dev)
{
	int error = 0;
	XWObject * font_dict = pop();
  	if (!isFontDict(font_dict))
    	error = 1;
  	else
  	{
  		XWObject obj, obj1;
  		font_dict->dictLookupNF("FontName", &obj);
  		const char * font_name  = obj.getName();  		
  		font_dict->dictLookupNF("FontScale", &obj1);
  		double font_scale = obj1.getNum();
  		error = setFont(dev, font_name, font_scale);
  		obj.free();
  	}
  	
  	if (font_dict)
  		xref->releaseObj(font_dict);

  	return error;
}

int XWMPost::doShow(XWDVIRef * xref, XWDVIDev * dev)
{
	mp_font * font = CURRENT_FONT();
  	if (!font) 
    	return 1;
    	
    PDFCoord  cp;
    dev->currentPoint(&cp);

  	XWObject * text_str = pop();
  	if (!text_str || !text_str->isString()) 
  	{
    	if (text_str)
      		xref->releaseObj(text_str);
    	return 1;
  	}
  	
  	if (font->font_id < 0) 
  	{
    	xref->releaseObj(text_str);
    	return 1;
  	}
  	
  	XWString * str = text_str->getString();
  	uchar * strptr = (uchar*)(str->getCString());
  	int length = str->getLength();
  	
  	double text_width = 0.0;  	
  	if (font->subfont_id >= 0)
  	{
  		XWSubfont subfont(font->subfont_file_id);
  		uchar * ustr = (uchar*)new uchar[length * 2];
    	for (int i = 0; i < length; i++) 
    	{
      		ushort uch = subfont.lookupRecord(font->subfont_id, strptr[i]);
      		ustr[2*i  ] = uch >> 8;
      		ustr[2*i+1] = uch & 0xff;
      		if (font->tfm_id >= 0) 
      		{
      			XWTFMFile tfm(font->tfm_id);
				text_width += tfm.getWidth(strptr[i]);
			}
    	}
    	text_width *= font->pt_size;

    	dev->setString(xref, (long)(cp.x * dev->unitDVIUnit()),
		       		  (long)(cp.y * dev->unitDVIUnit()),
		       		  ustr, length * 2,
		              (long)(text_width * dev->unitDVIUnit()),
		              font->font_id, 0);
    	delete [] ustr;
  	}
  	else
  	{
  		if (font->tfm_id >= 0)
  		{
  			XWTFMFile tfm(font->tfm_id);
  			text_width = (double) tfm.getStringWidth(strptr, length)/(1<<20);
      		text_width *= font->pt_size;
  		}
  		
  		dev->setString(xref, (long)(cp.x * dev->unitDVIUnit()),
		              (long)(cp.y * dev->unitDVIUnit()),
		       		  strptr, length,
		              (long)(text_width*dev->unitDVIUnit()),
		              font->font_id, 0);
  	}
  	
  	if (dev->getFontWMode(font->font_id)) 
    	dev->rmoveTo(0.0, -text_width);
  	else 
    	dev->rmoveTo(text_width, 0.0);

  	dev->graphicsMode(xref);
  	xref->releaseObj(text_str);

  	return 0;
}

int XWMPost::doTexFigOperator(XWDVICore * core,
	                          XWDVIRef * xref, 
	                          XWDVIDev * dev, 
	                          int opcode, 
	                          double x_user, 
	                          double y_user)
{
	static TransformInfo fig_p;
  	static int in_tfig = 0;
  	static int xobj_id = -1;
  	static int count   = 0;
  	double values[6];
  	int    error = 0;
  	
  	switch (opcode)
  	{
  		case STEXFIG:
  			error = popGetNumbers(xref, values, 6);
    		if (!error) 
    		{
      			transform_info_clear(&fig_p);
      			double dvi2pts = 1.0 / dev->unitDVIUnit();

      			fig_p.width    =  values[0] * dvi2pts;
      			fig_p.height   =  values[1] * dvi2pts;
      			fig_p.bbox.llx =  values[2] * dvi2pts;
      			fig_p.bbox.lly = -values[3] * dvi2pts;
      			fig_p.bbox.urx =  values[4] * dvi2pts;
      			fig_p.bbox.ury = -values[5] * dvi2pts;
      			fig_p.flags   |= INFO_HAS_USER_BBOX;
      			
				char     resname[256];
      			sprintf(resname, "__tf%d__", count);
      			xobj_id = xref->beginGrabbing(core, dev, resname, fig_p.bbox.llx, fig_p.bbox.ury, &fig_p.bbox);
      
      			in_tfig = 1;
      			count++;
    		}
  			break;
  			
  		case ETEXFIG:
  			if (!in_tfig)
  				return 1;
  			xref->endGrabbing(dev, NULL);
    		dev->putImage(xref, xobj_id, &fig_p, x_user, y_user);
    		in_tfig = 0;
  			break;
  			
  		default:
    		error = 1;
    		break;
  	}
  	
  	return error;
}

int XWMPost::getOpCode(const char *token)
{
	for (int i = 0; i < NUM_PS_OPERATORS; i++) 
	{
    	if (!strcmp(token, ps_operators[i].token)) 
      		return ps_operators[i].opcode;
  	}

  	for (int i = 0; i < NUM_MPS_OPERATORS; i++) 
  	{
    	if (!strcmp(token, mps_operators[i].token)) 
      		return mps_operators[i].opcode;
  	}

  	return -1;
}

void XWMPost::init()
{
	if (!stack)
	{
		top_stack = 0;
		stack = (XWObject*)malloc(PS_STACK_SIZE * sizeof(XWObject));
		for (int i = 0; i < PS_STACK_SIZE; i++)
			stack[i].initNull();
			
		currentfont = -1;
		for (int i = 0; i < PDF_GSAVE_MAX; i++)
			font_stack[0].font_name = 0;
	}
}

int XWMPost::isFontDict(XWObject *dict)
{
	if (!dict || !dict->isDict())
		return 0;
		
	XWObject obj;
	dict->dictLookupNF("Type", &obj);
  	if (!obj.isName("Font"))
  	{
  		obj.free();
    	return 0;
    }
    	
    obj.free();
    	
    dict->dictLookupNF("FontName", &obj);
    if (!obj.isName())
    {
    	obj.free();
    	return 0;
    }
    
    obj.free();

  	dict->dictLookupNF("FontName", &obj);
  	if (!obj.isNum()) 
  	{
    	obj.free();
    	return 0;
  	}
	obj.free();
  	return 1;
}

int XWMPost::isFontName(const char *token)
{
	XWFontCache fcache(true);
  	XWTexFontMap * mrec = fcache.lookupFontMapRecord(token);
  	if (mrec)
  		return 1;
  		
  	XWTFMFile tfm(token);
  	return (tfm.getID() != -1);
}

int XWMPost::parseBody(XWDVICore * core,
	                   XWDVIRef * xref, 
	                   XWDVIDev * dev, 
	                   XWLexer * lexer,
	                   double x_user, 
	                   double y_user)
{
	XWObject obj;
  	int  error = 0;
  	const char * p = lexer->skipWhite();
  	const char * end = lexer->getEndPtr();
  	XWDVIParser parser(xref, lexer);
  	while (p < end && !error)
  	{
  		if (isdigit(*p) || 
  			(p < end - 1 && 
  			(*p == '+' || 
  			*p == '-' || 
  			*p == '.' )))
  		{
  			double value;
      		char  *next = 0;
      		value = strtod(p, &next);
      		if (next < end && !strchr("<([{/%", *next) && !isspace(*next))
				error = 1;
			else
			{
				obj.initReal(value);
				push(&obj);
				lexer->setCurPtr(next);
			}
  		}
  		else if (*p == '[' && (parser.parsePDFArray(&obj)))
  			push(&obj);
  		else if (p < end - 1 && (*p == '<' && *(p+1) == '<') && (parser.parsePDFDict(&obj)))
  			push(&obj);
  		else if ((*p == '(' || *p == '<') && (parser.parsePDFString(&obj)))
  			push(&obj);
  		else if (*p == '/' && (parser.parsePDFName(&obj)))
  			push(&obj);
  		else
  		{
  			char * token = lexer->getIdent(0);
  			if (!token)
				error = 1;
			else
			{
				error = doOperator(core, xref, dev, token, x_user, y_user);
				delete [] token;
			}
  		}
  		
  		p = lexer->skipWhite();
  	}
  	
  	return error;
}

int XWMPost::popGetNumbers(XWDVIRef * xref, double *values, int count)
{
	while (count-- > 0)
	{
		XWObject * tmp = pop();
		if (!tmp)
			break;
			
		if (!tmp->isNum())
		{
			xref->releaseObj(tmp);
      		break;
		}
		
		values[count] = tmp->getNum();
		xref->releaseObj(tmp);
	}
	
	return (count + 1);
}

int XWMPost::push(XWObject * obj)
{
	if (top_stack < PS_STACK_SIZE)
	{
		if (!stack[top_stack].isNull())
			stack[top_stack].free();
		stack[top_stack++] = *obj;
		return 0;
	}
	
	obj->free();
	obj->initNull();
	return -1;
}

void XWMPost::restoreFont()
{
	mp_font * current = CURRENT_FONT();
  	if (current) 
  	{
    	if (current->font_name)
      		delete [] current->font_name;
    	current->font_name = 0;
  	} 
  	else 
  		return ;

  	currentfont--;
}

void XWMPost::saveFont()
{
	if (currentfont < 0) 
	{
		if (font_stack[0].font_name)
			delete [] font_stack[0].font_name;
    	font_stack[0].font_name  = qstrdup("Courier");
    	font_stack[0].pt_size    = 1;
    	font_stack[0].tfm_id     = 0;
    	font_stack[0].subfont_id = 0;
    	currentfont = 0;
  	}
  	
  	mp_font * current = &font_stack[currentfont++];
  	mp_font * next    = &font_stack[currentfont  ];
  	if (next->font_name)
  		delete [] next->font_name;
  	next->font_name  = qstrdup(current->font_name);
  	next->pt_size    = current->pt_size;

	next->subfont_file_id = current->subfont_file_id;
  	next->subfont_id = current->subfont_id;
  	next->tfm_id     = current->tfm_id;
}

int XWMPost::setFont(XWDVIDev * dev,
	                 const char *font_name, 
	                 double pt_size)
{
	mp_font * font = CURRENT_FONT();
	if (font) 
	{
    	if (!strcmp(font->font_name, font_name) && font->pt_size == pt_size)
      		return  0;
  	}
  	else
  	{
  		font = &font_stack[0];
  		if (font->font_name)
  			delete [] font->font_name;
    	font->font_name = 0;
    	currentfont = 0;
  	}
  	
  	XWFontCache fcache(true);
  	XWTexFontMap * mrec = fcache.lookupFontMapRecord(font_name);
  	int subfont_id = -1;
  	if (mrec && mrec->charmap.sfd_name && mrec->charmap.subfont_id) 
  	{
  		QString tmp(mrec->charmap.sfd_name);
  		XWSubfont subfont(tmp);
  		QString id(mrec->charmap.subfont_id);
    	subfont_id = subfont.loadRecord(id);
    	font->subfont_file_id = subfont.getFileID();
  	}
  	
  	const char *name = font_name;
  	if (mrec && mrec->map_name)
  		name = mrec->map_name;
  		
  	if (font->font_name)
    	delete [] font->font_name;
    	
    font->font_name = qstrdup(font_name);
    font->subfont_id = subfont_id;
  	font->pt_size    = pt_size;
  	XWTFMFile tfm(font_name);
  	font->tfm_id     = tfm.getID(); /* Need not exist in MP mode */
  	font->font_id    = dev->locateFont(name, (long) (pt_size * dev->unitDVIUnit()));
  	if (font->font_id < 0)
  		return -1;
  		
  	return  0;
}

void XWMPost::skipProlog(XWLexer * lexer)
{
	int   found_prolog = 0;
	const char * save = lexer->getCurPtr();
	const char * p = save;
	const char * end = lexer->getEndPtr();
	while (p < end) 
	{
    	if (*p != '%')
      		p = lexer->skipWhite();
    	if (p >= end)
      		break;
    	if (!strncmp(p, "%%EndProlog", 11)) 
    	{
      		found_prolog = 1;
      		p = lexer->skipLine();
      		break;
    	} 
    	else if (!strncmp(p, "%%Page:", 7)) 
    	{
      		p = lexer->skipLine();
      		break;
    	}
    	p = lexer->skipLine();
  	}
  	
  	if (!found_prolog) 
  		lexer->setCurPtr(save);
}

void XWMPost::stackClearTo(XWDVIRef * xref, int depth)
{
	while (top_stack > (uint)depth) 
	{
    	XWObject * tmp = pop();
    	if (tmp)
      		xref->releaseObj(tmp);
  	}
}

