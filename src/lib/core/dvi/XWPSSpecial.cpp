/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWApplication.h"
#include "XWLexer.h"
#include "XWObject.h"
#include "XWDVIType.h"
#include "XWDVIRef.h"
#include "XWDVICore.h"
#include "XWDVIDev.h"
#include "XWMPost.h"
#include "XWPSSpecial.h"

#define SP_PS_FILE      0
#define SP_PS_PLOTFILE  1
#define SP_PS_LITERAL   2
#define SP_PS_DEFAULT   3

SpecialHandler psHandler[] = {
	{"PSfile", SP_PS_FILE},
	{"psfile", SP_PS_FILE},
	{"ps: plotfile", SP_PS_PLOTFILE},
	{"PS: plotfile", SP_PS_PLOTFILE},
	{"PS:", SP_PS_LITERAL},
	{"ps:", SP_PS_LITERAL},
	{"\" ", SP_PS_DEFAULT}
};

#define numPSOps (sizeof(psHandler) / sizeof(SpecialHandler))


static int    block_pending = 0;
static double pending_x     = 0.0;
static double pending_y     = 0.0;
static int    position_set  = 0;


XWPSSpecial::XWPSSpecial(XWDVICore * coreA,
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

int XWPSSpecial::check(XWLexer * lexerA)
{
	int typeA = -1;
	const char * p = lexerA->getCurPtr();
	for (int i = 0; i < numPSOps; i++)
	{
		SpecialHandler * handler = &psHandler[i];
		if (!strncmp(p, handler->key, strlen(handler->key)))
		{
			typeA = handler->type;
			lexerA->skip(strlen(handler->key));
			break;
		}
	}
	
	return typeA;
}

bool XWPSSpecial::exec(int hDPIA, int vDPIA)
{
	hDPI = hDPIA;
	vDPI = vDPIA;
	int ret = 0;
	switch (type)
	{
		case SP_PS_FILE:
			ret = doFile();
			break;
			
		case SP_PS_PLOTFILE:
			ret = doPlotFile();
			break;
			
		case SP_PS_LITERAL:
			ret = doLiteral();
			break;
			
		case SP_PS_DEFAULT:
			ret = doDefault();
			break;
			
		default:
			return false;
			break;
	}
	
	return ret < 0 ? false : true;
}

int XWPSSpecial::doDefault()
{
	dev->gsave(xref);

  	int st_depth = XWMPost::stackDepth();
  	int error = 0;
  	int gs_depth = dev->currentDepth();
  	
  	{
    	PDFTMatrix M;
    	M.a = M.d = 1.0; 
    	M.b = M.c = 0.0; 
    	M.e = x_user; 
    	M.f = y_user;
    	dev->concat(xref, &M);
    	char * p = (char*)(lexer->getCurPtr());
    	char * endptr = (char*)(lexer->getEndPtr());
  		error = XWMPost::execInline(core, xref, dev, &p, endptr, x_user, y_user);
    	M.e = -x_user; M.f = -y_user;
    	dev->concat(xref, &M);
  	}
  
  	if (error)
    	xwApp->warning(tr("interpreting PS code failed!!! Output might be broken!!!\n"));
  	else 
  	{
    	if (st_depth != XWMPost::stackDepth()) 
    	{
      		xwApp->warning(tr("stack not empty after execution of inline PostScript code.\n"));
      		xwApp->warning(tr(">> Your macro package makes some assumption on internal behaviour of DVI drivers.\n"));
      		xwApp->warning(tr(">> It may not compatible with dvipdfmx."));
    	}
  	}

  	dev->grestoreTo(xref, gs_depth);
  	dev->grestore(xref);

  	return  error;
}

int XWPSSpecial::doFile()
{
	const char * p = lexer->skipWhite();	
	const char * endptr = lexer->getEndPtr();
	if (p + 1 >= endptr || p[0] != '=') 
	{
    	xwApp->warning(tr("no filename specified for PSfile special.\n"));
    	return  -1;
  	}
  	p = lexer->skip(1);
  	char * filename = parseFileName();
  	if (!filename) 
  	{
    	xwApp->warning(tr("no filename specified for PSfile special.\n"));
    	return  -1;
  	}
  	
  	TransformInfo ti;
  	transform_info_clear(&ti);
  	if (readDimTrns(&ti, NULL, 1) < 0) 
  	{
    	delete [] filename;
    	return  -1;
  	}
  	
  	int form_id = core->findImageResource(filename, 1, NULL);
  	if (form_id < 0) 
  	{
  		QString msg = QString(tr("failed to read image file: %1\n")).arg(filename);
    	xwApp->warning(msg);
    	delete [] filename;
    	return  -1;
  	}
  	delete [] filename;

  	dev->putImage(xref, form_id, &ti, x_user, y_user);

  	return  0;
}

int XWPSSpecial::doLiteral()
{
	const char * p = lexer->skipWhite();	
	const char * endptr = lexer->getEndPtr();
	if (p >= endptr)
    	return  -1;

	double x_userA, y_userA;
  	if (p + strlen(":[begin]") <= endptr && 
  	 	!strncmp(p, ":[begin]", strlen(":[begin]"))) 
  	{
    	block_pending++;
    	position_set = 1;

    	pending_x = x_user;
    	pending_y = y_user;
    	x_userA = x_user;
    	y_userA = y_user;
    	p = lexer->skip(strlen(":[begin]"));
  	} 
  	else if (p + strlen(":[end]") <= p && 
  		    !strncmp(p, ":[end]", strlen(":[end]"))) 
  	{
    	if (block_pending <= 0) 
    	{
      		xwApp->warning(tr("no corresponding ::[begin] found.\n"));
      		return -1;
    	}
    	
    	block_pending--;

    	position_set = 0;

    	x_userA = pending_x;
    	y_userA = pending_y;
    	p = lexer->skip(strlen(":[end]"));
  	} 
  	else if (p < endptr && p[0] == ':') 
  	{
    	x_userA = position_set ? pending_x : x_user;
    	y_userA = position_set ? pending_y : y_user;
    	p = lexer->skip(1);
  	} 
  	else 
  	{
    	position_set = 1;
    	x_userA = pending_x = x_user;
    	y_userA = pending_y = y_user;
  	}

  	p = lexer->skipWhite();	
  	int error = 0;
  	if (p < endptr) 
  	{
    	int st_depth = XWMPost::stackDepth();
    	int gs_depth = dev->currentDepth();

    	error = XWMPost::execInline(core, xref, dev, (char**)&p, (char*)endptr, x_userA, y_userA);
    	if (error) 
    	{
      		xwApp->warning(tr("interpreting PS code failed!!! Output might be broken!!!\n"));
      		dev->grestoreTo(xref, gs_depth);
    	} 
    	else if (st_depth != XWMPost::stackDepth()) 
    	{
      		xwApp->warning(tr("stack not empty after execution of inline PostScript code.\n"));
      		xwApp->warning(tr(">> Your macro package makes some assumption on internal behaviour of DVI drivers.\n"));
      		xwApp->warning(tr(">> It may not compatible with dvipdfmx.\n"));
    	}
  	}

  	return  error;
}

int XWPSSpecial::doPlotFile()
{
	lexer->skipWhite();	
	char * filename = parseFileName();
  	if (!filename) 
  	{
    	xwApp->warning(tr("expecting filename but not found...\n"));
    	return -1;
  	}
  	
  	int form_id = core->findImageResource(filename, 1, NULL);
  	if (form_id < 0) 
  	{
  		QString msg = QString(tr("could not open PS file: %1\n")).arg(filename);
    	xwApp->warning(msg);
    	delete [] filename;
    	return  -1;
  	}
  	else
  	{
  		TransformInfo p;
  		transform_info_clear(&p);
    	p.matrix.d = -1.0;
    	dev->putImage(xref, form_id, &p, 0, 0);
  	}
  	
  	delete [] filename;

  	return  0;
}

char * XWPSSpecial::parseFileName()
{
	const char * p = lexer->skipWhite();	
	const char * endptr = lexer->getEndPtr();
	char   qchar = 0;
	if (!p || p >= endptr)
    	return  0;
  	else if (*p == '\"' || *p == '\'')
  	{
    	qchar = *p;
    	p = lexer->skip(1);
    }
  	else 
    	qchar = ' ';
    	
    int n = 0;
    const char * q = p;
    while (p < endptr && *p != qchar)
    {
    	n++;
    	p = lexer->skip(1);
    }
    
    if (qchar != ' ') 
    {
    	if (*p != qchar)
      		return  0;
    	p = lexer->skip(1);
  	}
  	
  	if (!q || n == 0)
    	return  0;
    	
    char * r = new char[n + 1];
  	memcpy(r, q, n); 
  	r[n] = '\0';

  	return  r;
}

