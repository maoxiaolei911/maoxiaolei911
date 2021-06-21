/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <math.h>
#include "XWNumberUtil.h"
#include "XWLexer.h"
#include "XWApplication.h"
#include "XWObject.h"
#include "XWDVICore.h"
#include "XWDVIRef.h"
#include "XWDVIDev.h"
#include "XWHTMLSpecial.h"


#define  ANCHOR_TYPE_HREF  0
#define  ANCHOR_TYPE_NAME  1


#define \
downcasify(s) \
if ((s)) { \
  char  *_p = (char *) (s); \
  while (*(_p) != 0) { \
    if (*(_p) >= 'A' && *(_p) <= 'Z') { \
      *(_p) = (*(_p) - 'A') + 'a'; \
    } \
    _p++; \
  } \
}


#define  HTML_TAG_NAME_MAX    127
#define  HTML_TAG_TYPE_EMPTY  1
#define  HTML_TAG_TYPE_OPEN   1
#define  HTML_TAG_TYPE_CLOSE  2

static double
atopt (const char *a)
{
  	const char *_ukeys[] = {
#define K_UNIT__PT  0
#define K_UNIT__IN  1
#define K_UNIT__CM  2
#define K_UNIT__MM  3
#define K_UNIT__BP  4
    	"pt", "in", "cm", "mm", "bp",
#define K_UNIT__PX  5
    	"px",
     	NULL
  	};

	XWLexer lexerA(a);
  	char * q = lexerA.getFloatDecimal();
  	if (!q) 
    	return  0.0;

  	double v = atof(q);
  	delete [] q;
  	double u = 1.0;

  	q = lexerA.getCIdent();
  	if (q) 
  	{
  		int k = 0;
    	for (; _ukeys[k] && strcmp(_ukeys[k], q); k++);
    	switch (k) 
    	{
    		case K_UNIT__PT: 
    			u *= 72.0 / 72.27; 
    			break;
    			
    		case K_UNIT__IN: 
    			u *= 72.0; 
    			break;
    			
    		case K_UNIT__CM: 
    			u *= 72.0 / 2.54 ; 
    			break;
    			
    		case K_UNIT__MM: 
    			u *= 72.0 / 25.4 ; 
    			break;
    			
    		case K_UNIT__BP: 
    			u *= 1.0 ; 
    			break;
    			
    		case K_UNIT__PX: 
    			u *= 1.0 ; 
    			break; /* 72dpi */
    			
    		default:
      			break;
    	}
    	delete [] q;
  	}

  	return  v * u;
}


XWHTMLSpecial::XWHTMLSpecial(XWDVICore * coreA,
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
}

int XWHTMLSpecial::check(XWLexer *)
{
	return 0;
}

bool XWHTMLSpecial::exec(int , int)
{
	const char * p = lexer->getCurPtr();
	const char * endptr = lexer->getEndPtr();
	if (p >= endptr)
    	return  false;
    	
    char  name[HTML_TAG_NAME_MAX + 1];
    XWObject attr;
    attr.initDict(xref);
    int type = HTML_TAG_TYPE_OPEN;
    int error = readHTMLTag(name, &attr, &type);
  	if (error) 
  	{
    	attr.free();
    	return  false;
  	}
  	
  	HTMLState *sd = core->getHTMLState();
  	if (!strcmp(name, "a"))
  	{
  		switch (type) 
  		{
    		case  HTML_TAG_TYPE_OPEN:
      			error = anchorOpen(&attr, sd);
      			break;
      		
    		case  HTML_TAG_TYPE_CLOSE:
      			error = anchorClose(sd);
      			break;
      		
    		default:
      			xwApp->warning(tr("empty html anchor tag???\n"));
      			error = -1;
      			break;
    	}
  	}
  	else if (!strcmp(name, "base"))
  	{
  		if (type == HTML_TAG_TYPE_CLOSE) 
  		{
      		xwApp->warning(tr("close tag for \"base\"???\n"));
      		error = -1;
    	} 
    	else 
      		error = baseEmpty(&attr, sd);
  	}
  	else if (!strcmp(name, "img"))
  	{
  		if (type == HTML_TAG_TYPE_CLOSE) 
  		{
      		xwApp->warning(tr("close tag for \"img\"???\n"));
      		error = -1;
    	} 
    	else 
      		error = imgEmpty(&attr);
  	}
  	
  	attr.free();

  	return  error ? false : true;
}

int XWHTMLSpecial::anchorClose(HTMLState *sd)
{
	int  error = 0;

  	switch (sd->pending_type) 
  	{
  		case  ANCHOR_TYPE_HREF:
  			if (!xref->isScanning())
  			{
  				endAnnot();
  				sd->pending_type = -1;
  			}
    		else if (!sd->link_dict.isNull()) 
    		{
      			endAnnot();
      			sd->link_dict.free();
      			sd->link_dict.initNull();
      			sd->pending_type = -1;
    		} 
    		else 
    		{
      			xwApp->warning(tr("closing html anchor (link) without starting!\n"));
      			error = -1;
    		}
    		break;
    	
  		case  ANCHOR_TYPE_NAME:
    		sd->pending_type = -1;
    		break;
    		
  		default:
    		xwApp->warning(tr("no corresponding opening tag for html anchor.\n"));
    		error = -1;
    		break;
  	}

  	return  error;
}

int XWHTMLSpecial::anchorOpen(XWObject *attr, HTMLState *sd)
{
	if ((sd->pending_type >= 0 || !sd->link_dict.isNull()) && 
		(xref->isScanning()))
	{
    	xwApp->warning(tr("nested html anchors found!\n"));
    	return  -1;
  	}
  	
  	XWObject href, name;  	
  	attr->dictLookupNF("href", &href);
  	attr->dictLookupNF("name", &name);
  	int error = 0;
  	if (!href.isNull() && !name.isNull()) 
  	{
    	xwApp->warning(tr("sorry, you can't have both \"href\" and \"name\" in anchor tag...\n"));
    	error = -1;
  	} 
  	else if (!href.isNull()) 
    	error = openLink(href.getString()->getCString(), sd);
  	else if (!name.isNull()) 
    	error = openDest(name.getString()->getCString(), sd);
  	else 
  	{
    	xwApp->warning(tr("you should have \"href\" or \"name\" in anchor tag!\n"));
    	error = -1;
  	}
	href.free();
	name.free();
  	return  error;
}

int XWHTMLSpecial::baseEmpty(XWObject *attr, HTMLState *sd)
{
	if (!(xref->isScanning()))
		return 0;
		
	XWObject href;
	attr->dictLookupNF("href", &href);
  	if (href.isNull()) 
  	{
    	xwApp->warning(tr("\"href\" not found for \"base\" tag!\n"));
    	return  -1;
  	}
  	
  	char * vp = (char *) href.getString()->getCString();
  	if (sd->baseurl) 
  	{
  		QString msg = QString(tr("\"baseurl\" changed: \"%1\" --> \"%2\"")).arg(sd->baseurl).arg(vp);
    	xwApp->warning(msg);
    	delete [] sd->baseurl;
  	}
  	sd->baseurl = qstrdup(vp);
  	href.free();

  	return  0;
}

int XWHTMLSpecial::cvtAToTMatrix(PDFTMatrix *M, const char *ptr, char **nextptr)
{
	static const char *_tkeys[] = {
#define  K_TRNS__MATRIX     0
    	"matrix",    /* a b c d e f */
#define  K_TRNS__TRANSLATE  1
    	"translate", /* tx [ty] : dflt. tf = 0 */
#define  K_TRNS__SCALE      2
    	"scale",     /* sx [sy] : dflt. sy = sx */
#define  K_TRNS__ROTATE     3
    	"rotate",    /* ang [cx cy] : dflt. cx, cy = 0 */
#define  K_TRNS__SKEWX      4
#define  K_TRNS__SKEWY      5
    	"skewX",     /* ang */
    	"skewY",     /* ang */
    	NULL
  	};
  	
  	XWLexer lexerA(ptr);
  	const char * p = lexerA.getCurPtr();
  	const char * endptr = lexerA.getEndPtr();
  	while (p < endptr && isspace(*p))
  		p = lexerA.skip(1);
  		
  	char * q = lexerA.getCIdent();
  	if (!q)
    	return -1;
    	
    int k = 0;
    for (; _tkeys[k] && strcmp(q, _tkeys[k]); k++);
    delete [] q;
    
    while (p < endptr && isspace(*p))
  		p = lexerA.skip(1);
  		
  	if (*p != '(' || *(p + 1) == 0)
    	return  -1;
    	
    p = lexerA.skip(1);
    while (p < endptr && isspace(*p))
  		p = lexerA.skip(1);
  		
  	int n = 0;
  	double v[6];
  	while (n < 6 && p < endptr && *p != ')')
  	{
  		q = lexerA.getFloatDecimal();
  		if (!q)
      		break;
      	else
      	{
      		v[n] = atof(q);
      		if (*p == ',')
      			p = lexerA.skip(1);
      			
      		while (p < endptr && isspace(*p))
  				p = lexerA.skip(1);
  				
  			if (*p == ',')
  			{
  				p = lexerA.skip(1);
  				while (p < endptr && isspace(*p))
  					p = lexerA.skip(1);
  			}
  			
  			delete [] q;
      	}
      	
      	n++;
  	}
  	
  	if (*p != ')')
    	return  -1;
  	p = lexerA.skip(1);
  	
  	switch (k) 
  	{
  		case  K_TRNS__MATRIX:
    		if (n != 6)
      			return  -1;
    		M->a = v[0]; 
    		M->c = v[1];
    		M->b = v[2]; 
    		M->d = v[3];
    		M->e = v[4]; 
    		M->f = v[5];
    		break;
    		
  		case  K_TRNS__TRANSLATE:
    		if (n != 1 && n != 2)
      			return  -1;      			
    		M->a = M->d = 1.0;
    		M->c = M->b = 0.0;
    		M->e = v[0]; 
    		M->f = (n == 2) ? v[1] : 0.0;
    		break;
    		
  		case  K_TRNS__SCALE:
    		if (n != 1 && n != 2)
      			return  -1;
    		M->a = v[0]; 
    		M->d = (n == 2) ? v[1] : v[0];
    		M->c = M->b = 0.0;
    		M->e = M->f = 0.0;
    		break;
    		
  		case  K_TRNS__ROTATE:
    		if (n != 1 && n != 3)
      			return  -1;
    		M->a = cos(v[0] * M_PI / 180.0);
    		M->c = sin(v[0] * M_PI / 180.0);
    		M->b = -M->c; 
    		M->d = M->a;
    		M->e = (n == 3) ? v[1] : 0.0;
    		M->f = (n == 3) ? v[2] : 0.0;
    		break;
    		
  		case  K_TRNS__SKEWX:
    		if (n != 1)
       			return  -1;
    		M->a = M->d = 1.0;
    		M->c = 0.0;
    		M->b = tan(v[0] * M_PI / 180.0);
    		break;
    		
  		case  K_TRNS__SKEWY:
    		if (n != 1)
       			return  -1;
    		M->a = M->d = 1.0;
    		M->c = tan(v[0] * M_PI / 180.0);
    		M->b = 0.0;
    		break;
  	}

  	if (nextptr)
    	*nextptr = (char*)p;
    	
  	return  0;
}

char * XWHTMLSpecial::fqurl(const char *baseurl, const char *name)
{
	int len = strlen(name);
  	if (baseurl)
    	len += strlen(baseurl) + 1; /* we may want to add '/' */

  	char * q = new char[len + 1];
  	*q = '\0';
  	if (baseurl && baseurl[0]) 
  	{
    	strcpy(q, baseurl);
    	char * p = q + strlen(q) - 1;
    	if (*p == '/')
      		*p = '\0';
    	if (name[0] && name[0] != '/')
      		strcat(q, "/");
  	}
  	strcat(q, name);

  	return  q;
}

int XWHTMLSpecial::imgEmpty(XWObject *attr)
{
	XWObject src;
	attr->dictLookupNF("src", &src);
	if (src.isNull()) 
	{
    	xwApp->warning(tr("\"src\" attribute not found for \"img\" tag!\n"));
    	return  -1;
  	}
  	
  	XWObject obj;
  	TransformInfo ti;
  	transform_info_clear(&ti);
  	attr->dictLookupNF("width", &obj);
  	if (!obj.isNull()) 
  	{
    	ti.width  = atopt(obj.getString()->getCString());
    	ti.flags |= INFO_HAS_WIDTH;
    	obj.free();
  		obj.initNull();
  	}
  	
  	attr->dictLookupNF("height", &obj);
  	if (!obj.isNull()) 
  	{
    	ti.height  = atopt(obj.getString()->getCString());
    	ti.flags |= INFO_HAS_HEIGHT;
    	obj.free();
  		obj.initNull();
  	}
  	
  	double alpha = 1.0;
  	attr->dictLookupNF("svg:opacity", &obj);
  	if (!obj.isNull())  
  	{
    	alpha = atof(obj.getString()->getCString());
    	if (alpha < 0.0 || alpha > 1.0) 
      		alpha = 1.0;
      		
      	obj.free();
  		obj.initNull();
  	}
  	
  	attr->dictLookupNF("svg:transform", &obj);
  	PDFTMatrix M, M1;
  	pdf_setmatrix(&M, 1.0, 0.0, 0.0, 1.0, x_user, y_user);
  	int error = 0;
  	if (!obj.isNull())  
  	{
  		char  *p = (char *) obj.getString()->getCString();
    	PDFTMatrix  N;
    	for ( ; *p && isspace(*p); p++);
    	while (*p && !error) 
    	{
      		pdf_setmatrix(&N, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
      		error = cvtAToTMatrix(&N, p, &p);
      		if (!error) 
      		{
        		N.f = -N.f;
        		pdf_concatmatrix(&M, &N);
        		for ( ; *p && isspace(*p); p++);
        		if (*p == ',')
          			for (++p; *p && isspace(*p); p++);
      		}
    	}
    	obj.free();
  		obj.initNull();
  	}
  	
  	if (error) 
  	{
    	xwApp->warning(tr("error in html \"img\" tag attribute.\n"));
    	src.free();
    	return  error;
  	}
  	
  	int id = core->findImageResource(src.getString()->getCString(), 0, NULL);
  	if (id < 0) 
  	{
  		QString msg = QString(tr("could not find/load image: %1")).arg(src.getString()->getCString());
    	xwApp->warning(msg); 
    	error = -1;
  	}
  	else
  	{
  		dev->graphicsMode(xref);
      	dev->gsave(xref);
      	
      	{
      		char res_nameA[32];
      		int  a = round(100.0 * alpha);
        	if (a != 0) 
        	{
          		XWObject dict;
          		dict.initNull();
            	createXGState(roundAt(0.01 * a, 0.01), 0, res_nameA, &dict);
            	xref->refObj(&dict, &obj);
            	if (xref->isScanning())
            	{
            		xref->addPageResource("ExtGState", res_nameA, &obj);
            		xref->releaseObj(&dict);
            	}
            	else
            		dict.free();
            		
          		xref->addPageContent(" /", 2);
          		xref->addPageContent(res_nameA, strlen(res_nameA));
          		xref->addPageContent(" gs", 3);
        	}
      	}
      	
      	PDFRect  r;
      	core->scaleImage(id, &M1, &r, &ti);
      	pdf_concatmatrix(&M, &M1);
      	dev->concat(xref, &M);

      	dev->rectClip(xref, r.llx, r.lly, r.urx - r.llx, r.ury - r.lly);

		char * res_name = core->getImageResName(id);
      	xref->addPageContent(" /", 2);  /* op: */
      	xref->addPageContent(res_name, strlen(res_name));  /* op: */
      	xref->addPageContent(" Do", 3);  /* op: Do */

      	dev->grestore(xref);
      	if (xref->isScanning())
      	{
      		core->getImageReference(id, &obj);      	
      		xref->addPageResource("XObject", res_name, &obj);
      	}
  	}
  	src.free();
  	return  error;
}

int XWHTMLSpecial::openDest(const char *name, HTMLState *sd)
{
	int error = 0;
	if (xref->isScanning())
	{
		PDFCoord  cp;
  		cp.x = x_user; 
  		cp.y = y_user;
  		dev->transform(&cp, 0);
  		XWObject obj, array;
  		xref->thisPageRef(&obj);
  		array.initArray(xref);
  		array.arrayAdd(&obj);
  		obj.initName("XYZ");
  		array.arrayAdd(&obj);
  		obj.initNull();
  		array.arrayAdd(&obj);
  		obj.initReal(cp.y + 24.0);
  		array.arrayAdd(&obj);
  		obj.initNull();
  		array.arrayAdd(&obj);
  	
  		error = xref->addNames("Dests", name, strlen(name), &array);
  		if (error)
  		{
  			QString msg = QString(tr("failed to add named destination: %1\n")).arg(name);
    		xwApp->warning(msg);
    	}
    }

  	sd->pending_type = ANCHOR_TYPE_NAME;

  	return  error;
}

int XWHTMLSpecial::openLink(const char *name, HTMLState *sd)
{
	if (xref->isScanning())
	{
		sd->link_dict.free();
		sd->link_dict.initDict(xref);
		XWObject obj;
		obj.initName("Annot");
		sd->link_dict.dictAdd(qstrdup("Type"), &obj);
		obj.initName("Link");
		sd->link_dict.dictAdd(qstrdup("Subtype"), &obj);
	
		XWObject color;
		color.initArray(xref);
		obj.initReal(0.0);
		color.arrayAdd(&obj);
		obj.initReal(0.0);
		color.arrayAdd(&obj);
		obj.initReal(1.0);
		color.arrayAdd(&obj);
		sd->link_dict.dictAdd(qstrdup("C"), &color);
		char * url = fqurl(sd->baseurl, name);
		if (url[0] == '#')
		{
			obj.initString(new XWString(url+1, strlen(url+1)));
			sd->link_dict.dictAdd(qstrdup("Dest"), &obj);
		}
		else
		{
			XWObject action;
			action.initDict(xref);
			obj.initName("Action");
			action.dictAdd(qstrdup("Type"), &obj);
			obj.initName("URI");
			action.dictAdd(qstrdup("S"), &obj);
			obj.initString(new XWString(url, strlen(url)));
			action.dictAdd(qstrdup("URI"), &obj);
			action.copy(&obj);
			sd->link_dict.dictAdd(qstrdup("A"), &obj);
			action.free();
		}
	
		delete [] url;
	}
	
	beginAnnot(&(sd->link_dict));

  	sd->pending_type = ANCHOR_TYPE_HREF;

  	return  0;
}

int XWHTMLSpecial::parseKeyVal(char **kp, char **vp)
{
	const char * p = lexer->getCurPtr();
	const char * endptr = lexer->getEndPtr();
	char  *k = 0, *v = 0;
	int    n = 0, error = 0;
	
	const char * q = p; 
	while (p < endptr && 
	      ((*p >= 'a' && *p <= 'z') || 
          (*p >= 'A' && *p <= 'Z') ||
          (*p >= '0' && *p <= '9') ||
          *p == '-' || *p == ':'))
    {
      	n++;
       	p = lexer->skip(1);
    }
    
    if (n == 0)
    {
      	*kp = *vp = 0;
    	return  -1;
    }
    k = new char[n + 1];
    memcpy(k, q, n); 
    k[n] = '\0';
    if (p + 2 >= endptr || p[0] != '=' || (p[1] != '\"' && p[1] != '\'')) 
    {
    	delete [] k; 
      	k = 0;
      	error = -1;
    }
    else
    {
    	char  qchr = p[1];
    	p = lexer->skip(2);
    	q = p; 
    	n = 0;
    	while (p < endptr && *p != qchr)
    	{
    		n++;
        	p = lexer->skip(1);
      	}
      		
      	if (p == endptr || *p != qchr)
        	error = -1;
      	else
      	{
      		v = new char[n + 1];
        	memcpy(v, q, n); 
        	v[n] = '\0';
        	p = lexer->skip(1);
      	}
    }
	
	*kp = k; 
	*vp = v;
	
  	return  error;
}

int XWHTMLSpecial::readHTMLTag(char *name, 
	                           XWObject *attr, 
	                           int *type)
{
	const char * p = lexer->getCurPtr();
	const char * endptr = lexer->getEndPtr();
	while (p < endptr && isspace(*p))
		p = lexer->skip(1);
		
	if (p >= endptr || *p != '<')
    	return  -1;
    	
    *type = HTML_TAG_TYPE_OPEN;    	
    p = lexer->skip(1);
    while (p < endptr && isspace(*p))
		p = lexer->skip(1);
		
	if (p < endptr && *p == '/') 
	{
    	*type = HTML_TAG_TYPE_CLOSE;
    	p = lexer->skip(1);
    	while (p < endptr && isspace(*p))
			p = lexer->skip(1);
  	}
  	
#define ISDELIM(c) ((c) == '>' || (c) == '/' || isspace(c))
	
	int n = 0;
	while (p < endptr && n < HTML_TAG_NAME_MAX && !ISDELIM(*p))
	{
		name[n] = *p;
		n++;
		p = lexer->skip(1);
	}
	
	name[n] = '\0';
  	if (n == 0 || p == endptr || !ISDELIM(*p)) 
    	return  -1;
    	
    while (p < endptr && isspace(*p))
		p = lexer->skip(1);
		
	XWObject obj;
	int error = 0;
	while (p < endptr && !error && *p != '/' && *p != '>')
	{
		char  *kp = 0, *vp = 0;
		error = parseKeyVal(&kp, &vp);
    	if (!error) 
    	{
      		downcasify(kp);
      		obj.initString(new XWString(vp, strlen(vp) + 1));
      		attr->dictAdd(kp, &obj);
      		delete [] vp;
    	}
    	else
    	{
    		if (kp)
    			delete [] kp;
    			
    		if (vp)
    			delete [] vp;
    	}
    	
    	while (p < endptr && isspace(*p))
			p = lexer->skip(1);
	}
	
	if (error)
		return error;
		
	if (p < endptr && *p == '/') 
	{
    	*type = HTML_TAG_TYPE_EMPTY;
    	p = lexer->skip(1);
    	while (p < endptr && isspace(*p))
			p = lexer->skip(1);
    	
  	}
  	
  	if (p == endptr || *p != '>') 
    	return  -1;
    	
  	p = lexer->skip(1);

  	downcasify(name);
  	return  0;
}

