/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include "XWNumberUtil.h"
#include "XWLexer.h"
#include "XWApplication.h"
#include "XWObject.h"
#include "XWDVIType.h"
#include "XWDVIRef.h"
#include "XWDVICore.h"
#include "XWDVIDev.h"
#include "XWTPICSpecial.h"

#define MI2DEV (0.072/ dev->scale())


#define CLOSED_PATH(s) (\
  (s)->points[0].x == (s)->points[(s)->num_points-1].x && \
  (s)->points[0].y == (s)->points[(s)->num_points-1].y \
)

#define SP_TPIC_PN 0
#define SP_TPIC_PA 1
#define SP_TPIC_FP 2
#define SP_TPIC_IP 3
#define SP_TPIC_DA 4
#define SP_TPIC_DT 5
#define SP_TPIC_SP 6
#define SP_TPIC_AR 7
#define SP_TPIC_IA 8
#define SP_TPIC_SH 9
#define SP_TPIC_WH 10
#define SP_TPIC_BK 11
#define SP_TPIC_TX 12

SpecialHandler tpicHandler[] = {
	{"pn", SP_TPIC_PN},
	{"pa", SP_TPIC_PA},
	{"fp", SP_TPIC_FP},
	{"ip", SP_TPIC_IP},
	{"da", SP_TPIC_DA},
	{"dt", SP_TPIC_DT},
	{"sp", SP_TPIC_SP},
	{"ar", SP_TPIC_AR},
	{"ia", SP_TPIC_IA},
	{"sh", SP_TPIC_SH},
	{"wh", SP_TPIC_WH},
	{"bk", SP_TPIC_BK},
	{"tx", SP_TPIC_TX}
};

#define numTPICOps (sizeof(tpicHandler) / sizeof(SpecialHandler))

XWTPICSpecial::XWTPICSpecial(XWDVICore * coreA,
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

int XWTPICSpecial::check(XWLexer * lexerA)
{
	const char * p = lexerA->skipWhite();
	if (!memcmp(p, "tpic:", strlen("tpic:")))
	{
		p = lexerA->skip(5);
		p = lexerA->skipWhite();
	}
	
	char * q = lexerA->getCIdent();
	if (!q)
		return -1;
		
	int typeA = -1;
	for (int i = 0; i < numTPICOps; i++)
	{
		SpecialHandler * handler = &tpicHandler[i];
		if (!strcmp(q, handler->key))
		{
			typeA = handler->type;
			break;
		}
	}
	
	delete [] q;
	return typeA;
}

bool XWTPICSpecial::exec(int , int )
{
	int ret = 0;
	switch (type)
	{
		case SP_TPIC_PN:
			ret = doPn();
			break;
			
		case SP_TPIC_PA:
			ret = doPa();
			break;
			
		case SP_TPIC_FP:
			ret = doFp();
			break;
			
		case SP_TPIC_IP:
			ret = doIp();
			break;
			
		case SP_TPIC_DA:
			ret = doDa();
			break;
			
		case SP_TPIC_DT:
			ret = doDt();
			break;
			
		case SP_TPIC_SP:
			ret = doSp();
			break;
			
		case SP_TPIC_AR:
			ret = doAr();
			break;
			
		case SP_TPIC_IA:
			ret = doIa();
			break;
			
		case SP_TPIC_SH:
			ret = doSh();
			break;
			
		case SP_TPIC_WH:
			ret = doWh();
			break;
			
		case SP_TPIC_BK:
			ret = doBk();
			break;
			
		case SP_TPIC_TX:
			ret = doTx();
			break;
			
		default:
			return false;
			break;
	}
	
	return ret < 0 ? false : true;
}

int  XWTPICSpecial::arc(TPICState *tp,
                        const PDFCoord  *c,
           	            int              f_vp,
                        double           da,
                        double          *v)
{
	double pn   = tp->pen_size;
  	int    f_fs = tp->fill_shape;

  	f_fs  = (round(fabs(v[4] - v[5]) + 0.5) >= 360) ? f_fs : 0;
  	f_vp  = (pn > 0.0) ? f_vp : 0;

  	if (f_vp || f_fs) 
  	{
    	dev->gsave(xref);

    	setStyles(tp, c, f_fs, f_vp, pn, da);

    	dev->arcx(v[0], v[1], v[2], v[3], v[4], v[5], +1, 0.0);

    	showPath(f_vp, f_fs);

    	dev->grestore(xref);
  	}
  	
  	tp->clear();

  	return  0;
}

int XWTPICSpecial::currentPoint(long *pg, PDFCoord *cp)
{
	*pg = 0;
  	cp->x = x_user;
  	cp->y = y_user;
  	return  0;
}

int XWTPICSpecial::doAr()
{
	TPICState *tp = core->getTPICState();
	const char * p = lexer->skipBlank();
	const char * endptr = lexer->getEndPtr();
	double v[6];
	int i = 0;
	for (; i < 6 && p < endptr; i++) 
	{
    	char * q = lexer->getFloatDecimal();
    	if (!q) 
    	{
      		xwApp->warning(tr("invalid args. in TPIC \"ar\" command.\n"));
      		return  -1;
    	}
    	
    	v[i] = atof(q);
    	delete [] q;
    	p = lexer->skipBlank();
  	}
  	
  	if (i != 6) 
  	{
    	xwApp->warning(tr("invalid arg for TPIC \"ar\" command.\n"));
    	return  -1;
  	}

  	v[0] *= MI2DEV; 
  	v[1] *= MI2DEV;
  	v[2] *= MI2DEV; 
  	v[3] *= MI2DEV;
  	v[4] *= 180.0 / M_PI;
  	v[5] *= 180.0 / M_PI;

  	PDFCoord  cp;
  	long       pg;
  	currentPoint(&pg, &cp);

  	return  arc(tp, &cp, 1, 0.0, v);
}

int XWTPICSpecial::doBk()
{
	TPICState *tp = core->getTPICState();
	tp->fill_shape = 1;
  	tp->fill_color = 1.0;

  	return  0;
}

int XWTPICSpecial::doDa()
{
	TPICState *tp = core->getTPICState();
	lexer->skipBlank();
	double da = 0.0;
	char * q = lexer->getFloatDecimal();
  	if (q) 
  	{
    	da = atof(q);
    	delete [] q;
  	}
  	
  	if (tp->num_points <= 1) 
  	{
    	xwApp->warning(tr("too few points (< 2) for polyline path.\n"));
    	return  -1;
  	}

	PDFCoord  cp;
  	long       pg;
  	currentPoint(&pg, &cp);

  	return  polyLine(tp, &cp, 1, da);
}

int XWTPICSpecial::doDt()
{
	TPICState *tp = core->getTPICState();
	lexer->skipBlank();
	double     da = 0.0;
	char * q = lexer->getFloatDecimal();	
	if (q) 
	{
    	da = -atof(q);
    	delete [] q;
  	}
  	
  	if (tp->num_points <= 1) 
  	{
    	xwApp->warning(tr("too few points (< 2) for polyline path.\n"));
    	return  -1;
  	}

  	PDFCoord  cp;
  	long       pg;
  	currentPoint(&pg, &cp);

  	return  polyLine(tp, &cp, 1, da);
}

int XWTPICSpecial::doFp()
{
	TPICState *tp = core->getTPICState();
  	if (tp->num_points <= 1) 
  	{
    	xwApp->warning(tr("too few points (< 2) for polyline path.\n"));
    	return  -1;
  	}
  	
  	PDFCoord  cp;
  	long       pg;
  	currentPoint(&pg, &cp);	
  	return  polyLine(tp, &cp, 1, 0.0);
}

int XWTPICSpecial::doIp()
{
	TPICState *tp = core->getTPICState();
	if (tp->num_points <= 1) 
	{
    	xwApp->warning(tr("too few points (< 2) for polyline path.\n"));
    	return  -1;
  	}
  	
  	PDFCoord  cp;
  	long       pg;
  	currentPoint(&pg, &cp);  	
  	return  polyLine(tp, &cp, 0, 0.0);
}

int XWTPICSpecial::doIa()
{
	TPICState *tp = core->getTPICState();
	const char * p = lexer->skipBlank();
	const char * endptr = lexer->getEndPtr();
	double     v[6];
	int i = 0;
	for (; i < 6 && p < endptr; i++) 
	{
		char * q = lexer->getFloatDecimal();
    	if (!q) 
    	{
      		xwApp->warning(tr("invalid args. in TPIC \"ia\" command.\n"));
      		return  -1;
    	}
    	v[i] = atof(q);
    	delete [] q;
    	p = lexer->skipBlank();
	}
	
	if (i != 6) 
	{
    	xwApp->warning(tr("invalid arg for TPIC \"ia\" command.\n"));
    	return  -1;
  	}

  	v[0] *= MI2DEV; 
  	v[1] *= MI2DEV;
  	v[2] *= MI2DEV; 
  	v[3] *= MI2DEV;
  	v[4] *= 180.0 / M_PI;
  	v[5] *= 180.0 / M_PI;

  	PDFCoord  cp;
  	long       pg;
  	currentPoint(&pg, &cp);

  	return  arc(tp, &cp, 0, 0.0, v);
}

int XWTPICSpecial::doPa()
{
	TPICState *tp = core->getTPICState();
	double  v[2];
	const char * p = lexer->skipBlank();
	const char * endptr = lexer->getEndPtr();
	int i = 0;
	
	for (; i < 2 && p < endptr; i++) 
	{
    	char * q = lexer->getFloatDecimal();
    	if (!q) 
    	{
      		xwApp->warning(tr("missing numbers for TPIC \"pa\" command.\n"));
      		return  -1;
    	}
    	
    	v[i] = atof(q);
    	delete [] q;
    	p = lexer->skipBlank();
  	}
  	
  	if (i != 2) 
  	{
    	xwApp->warning(tr("invalid arg for TPIC \"pa\" command.\n"));
    	return  -1;
  	}

  	if (tp->num_points >= tp->max_points) 
  	{
    	tp->max_points += 256;
    	tp->points = (PDFCoord*)realloc(tp->points, tp->max_points * sizeof(PDFCoord));
  	}
  	tp->points[tp->num_points].x = v[0] * MI2DEV;
  	tp->points[tp->num_points].y = v[1] * MI2DEV;
  	tp->num_points += 1;

  	return  0;
}

int XWTPICSpecial::doPn()
{
	TPICState *tp = core->getTPICState();
  	lexer->skipBlank();
  	char * q = lexer->getFloatDecimal();
  	if (!q) 
  	{
    	xwApp->warning(tr("invalid pen size specified?\n"));
    	return -1;
  	}
  	tp->pen_size = atof(q) * MI2DEV;
  	delete [] q;

  	return  0;
}

int XWTPICSpecial::doSh()
{
	TPICState *tp = core->getTPICState();
	tp->fill_shape = 1;
  	tp->fill_color = 0.5;

  	lexer->skipBlank();
  	char * q = lexer->getFloatDecimal();
  	if (q) 
  	{
    	double g = atof(q);
    	delete [] q;
    	if (g >= 0.0 && g <= 1.0)
      		tp->fill_color = g;
    	else 
    	{
    		QString msg = QString(tr("invalid fill color specified: %1\n")).arg(g);
      		xwApp->warning(msg);
      		return -1;
    	}      
  	}

  	return  0;
}

int XWTPICSpecial::doSp()
{
	TPICState *tp = core->getTPICState();
  	lexer->skipBlank();
  	char * q = lexer->getFloatDecimal();
  	double da = 0.0;
  	if (q) 
  	{
    	da = atof(q);
    	delete [] q;
  	}
  	
  	if (tp->num_points <= 2) 
  	{
    	xwApp->warning(tr("too few points (< 3) for spline path.\n"));
    	return  -1;
  	}

  	PDFCoord  cp;
  	long      pg;
  	currentPoint(&pg, &cp);

  	return  spline(tp, &cp, 1, da);
}

int XWTPICSpecial::doTx()
{
	xwApp->warning(tr("TPIC command \"tx\" not supported.\n"));

  	return  -1;
}

int XWTPICSpecial::doWh()
{
	TPICState *tp = core->getTPICState();
	tp->fill_shape = 1;
  	tp->fill_color = 0.0;

  	return  0;
}

int  XWTPICSpecial::polyLine(TPICState *tp,
                             const PDFCoord  *c,
                             int              f_vp,
                             double           da)
{
	double pn    = tp->pen_size;
  	int    f_fs  = tp->fill_shape;
  	int    error = 0;
  	
  	f_fs  = CLOSED_PATH(tp) ? f_fs : 0;
  	f_vp  = (pn > 0.0) ? f_vp : 0;

  	if (f_vp || f_fs) 
  	{
    	dev->gsave(xref);

    	setStyles(tp, c, f_fs, f_vp, pn, da);

    	dev->moveTo(tp->points[0].x, tp->points[0].y);
    	for (int i = 0; i < tp->num_points; i++)
      		dev->lineTo(tp->points[i].x, tp->points[i].y);

    	showPath(f_vp, f_fs);

    	dev->grestore(xref);
  	}

  	tp->clear();

  	return  error;
}

int  XWTPICSpecial::setFillStyle(double g, double a, int f_ais)
{
	if (a > 0.0) 
	{
		char     resname[32];  		
    	double alp = round(100.0 * a);
    	XWObject dict;
      	createXGState(round(0.01 * alp, 0.01), f_ais, resname, &dict);
      		XWObject obj;
      		xref->refObj(&dict, &obj);
      	if (xref->isScanning())
      	{
      		xref->addPageResource("ExtGState", resname, &obj);
      		xref->releaseObj(&dict);
      	}
      	else
      		dict.free();
    	
    	char     buf[32];
  		int len = 0;
    	len += sprintf(buf + len, " /%s gs", resname);
    	xref->addPageContent(buf, len);  /* op: gs */
  	}

  	{
    	XWDVIColor *sc, *fc;

    	dev->getCurrentColor(&sc, &fc); /* get stroking and fill colors */
    	XWDVIColor new_fc(fc, g);
    	dev->setNonStrokingColor(xref, &new_fc);
  	}

  	return  0;
}

int XWTPICSpecial::setLineStyle(double pn, double da)
{
	double  dp[2];
	dev->setLineJoin(xref, 1);
  	dev->setMiterLimit(xref, 1.4);
  	dev->setLineWidth(xref, pn);
  	if (da > 0.0) 
  	{
    	dp[0] =  da * 72.0;
    	dev->setLineDash(xref, 1, dp, 0);
    	dev->setLineCap(xref, 0);
  	} 
  	else if (da < 0.0) 
  	{
    	dp[0] =  pn;
    	dp[1] = -da * 72.0;
    	dev->setLineDash(xref, 2, dp, 0);
    	dev->setLineCap(xref, 1);
  	} 
  	else 
    	dev->setLineCap(xref, 0);

  	return  0;
}

void XWTPICSpecial::setStyles(TPICState *tp,
	    		              const PDFCoord  *c,
	    		              int              f_fs,
	                          int              f_vp,
	                          double           pn,
	                          double           da)
{
	PDFTMatrix M;

  	pdf_setmatrix (&M, 1.0, 0.0, 0.0, -1.0, c->x, c->y);
  	dev->concat(xref, &M);

  	if (f_vp)
    	setLineStyle(pn, da);

  	if (f_fs) 
  	{
    	double g, a;
    	if (tp->mode.fill == TPIC_MODE__FILL_SOLID || !tp->fill_color) 
    	{
      		g = 1.0 - tp->fill_color;
      		a = 0.0;
    	} 
    	else 
    	{
      		g = 0.0;
      		a = tp->fill_color;
    	}

    	int f_ais = (tp->mode.fill == TPIC_MODE__FILL_SHAPE) ? 1 : 0;

    	setFillStyle(g, a, f_ais);
  	}
}

void XWTPICSpecial::showPath(int f_vp, int f_fs)
{
	if (f_vp) 
	{
    	if (f_fs)
      		dev->flushPath(xref, 'b', PDF_FILL_RULE_NONZERO);
    	else 
      		dev->flushPath(xref, 'S', PDF_FILL_RULE_NONZERO);
  	} 
  	else 
  	{
    	if (f_fs)
      		dev->flushPath(xref, 'f', PDF_FILL_RULE_NONZERO);
    	else 
      		dev->newPath(xref);
  	}
}

int XWTPICSpecial::spline(TPICState *tp,
                          const PDFCoord  *c,
                          int              f_vp,
                          double           da)
{
	
  	double pn    = tp->pen_size;
  	int    f_fs  = tp->fill_shape;
  	int    error = 0;

  	f_fs  = CLOSED_PATH(tp) ? f_fs : 0;
  	f_vp  = (pn > 0.0) ? f_vp : 0;

  	if (f_vp || f_fs) 
  	{
    	dev->gsave(xref);

    	setStyles(tp, c, f_fs, f_vp, pn, da);

    	dev->moveTo(tp->points[0].x, tp->points[0].y);
		double v[6];
    	v[0] = 0.5 * (tp->points[0].x + tp->points[1].x);
    	v[1] = 0.5 * (tp->points[0].y + tp->points[1].y);
    	dev->lineTo(v[0], v[1]);
    	int i = 1;
    	for (; i < tp->num_points - 1; i++) 
    	{
      		/* B-spline control points */
      		v[0] = 0.5 * (tp->points[i-1].x + tp->points[i].x);
      		v[1] = 0.5 * (tp->points[i-1].y + tp->points[i].y);
      		v[2] = tp->points[i].x;
      		v[3] = tp->points[i].y;
      		v[4] = 0.5 * (tp->points[i].x + tp->points[i+1].x);
      		v[5] = 0.5 * (tp->points[i].y + tp->points[i+1].y);
      		dev->bspline(v[0], v[1], v[2], v[3], v[4], v[5]);
    	}
    	
    	dev->lineTo(tp->points[i].x, tp->points[i].y);

    	showPath(f_vp, f_fs);

    	dev->grestore(xref);
  	}
  	
  	tp->clear();

  	return  error;
}

