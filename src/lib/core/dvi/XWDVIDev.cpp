/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <QCoreApplication>
#include <QString>
#include  <QtDebug>
#include "XWApplication.h"
#include "XWCMap.h"
#include "XWType0Font.h"
#include "XWTexFontMap.h"
#include "XWTexFont.h"
#include "XWFontCache.h"
#include "XWDVIRef.h"
#include "XWDVIFonts.h"
#include "XWDVICore.h"
#include "XWDVIDev.h"


#define DVICOLORSTACK_NESTED    0
#define DVICOLORSTACK_OVERFLOW  1
#define DVICOLORSTACK_UNDERFLOW 2


static const char *colorstackerror_strings[] = {
	QT_TRANSLATE_NOOP("XWDVIColorStack", "you've mistakenly made a global color change within nested colors.\n"),
	QT_TRANSLATE_NOOP("XWDVIColorStack", "color stack overflow. Just ignore.\n"),
	QT_TRANSLATE_NOOP("XWDVIColorStack", "color stack underflow. Just ignore.\n")
};

static QString getClolorStackErrStr(int type)
{
	return qApp->translate("XWDVIColorStack", colorstackerror_strings[type]);
}

XWDVIColorStack::XWDVIColorStack(XWDVIDev * devA)
	:dev(devA)
{
	stroke = (XWDVIColor*)malloc(DEV_COLOR_STACK_MAX * sizeof(XWDVIColor));
	fill = (XWDVIColor*)malloc(DEV_COLOR_STACK_MAX * sizeof(XWDVIColor));
	current = 0;
	clear();
}

XWDVIColorStack::~XWDVIColorStack()
{
	free(stroke);
	free(fill);
}

void XWDVIColorStack::clear()
{
	if (current > 0)
	{
		QString msg = getClolorStackErrStr(DVICOLORSTACK_NESTED);
		xwApp->warning(msg);
	}
	
	current = 0;
	stroke[current].black();
	fill[current].black();
}

void XWDVIColorStack::getCurrent(XWDVIColor **sc, XWDVIColor **fc)
{
	*sc = &stroke[current];
  	*fc = &fill[current];
}

void XWDVIColorStack::pop(XWDVIRef * xref)
{
	if (current <= 0) 
	{
		QString msg = getClolorStackErrStr(DVICOLORSTACK_UNDERFLOW);
		xwApp->warning(msg);
  	} 
  	else 
  	{
    	current--;
    	dev->resetColor(xref, 0);
  	}
}

void XWDVIColorStack::push(XWDVIRef * xref, XWDVIColor *sc, XWDVIColor *fc)
{
	if (current >= DEV_COLOR_STACK_MAX-1) 
	{
		QString msg = getClolorStackErrStr(DVICOLORSTACK_OVERFLOW);
		xwApp->warning(msg);
  	} 
  	else 
  	{
    	current++;
    	set(xref, sc, fc);
  	}
}

void XWDVIColorStack::set(XWDVIRef * xref, XWDVIColor *sc, XWDVIColor *fc)
{
	if (sc)
		stroke[current].copy(sc);
					
	if (fc)
		fill[current].copy(fc);
		
	dev->resetColor(xref, 0);
}

#define detM(M) ((M).a * (M).d - (M).b * (M).c)
#define detP(M) ((M)->a * (M)->d - (M)->b * (M)->c)

static int
inversematrix(PDFTMatrix *W, const PDFTMatrix *M)
{
  	double det = detP(M);
  	if (fabs(det) < 1.e-8) 
    	return -1;

  	W->a =  (M->d) / det;  W->b = -(M->b) / det;
  	W->c = -(M->c) / det;  W->d =  (M->a) / det;
  	W->e =  (M->c) * (M->f) - (M->d) * (M->e);
  	W->f =  (M->b) * (M->e) - (M->a) * (M->f);

  	return 0;
}

#define vecprd(v,w) ((v).x * (w).x + (v).y * (w).y)
#define vecrot(v,w) ((v).x * (w).y - (v).y * (w).x)
#define dsign(v)    (((v) >= 0.0) ? 1.0 : -1.0)
/* acos => [0, pi] */
#define vecang(v,w) ( \
  dsign(vecrot((v),(w))) * \
    acos(vecprd((v),(w)) / sqrt(vecprd((v),(v)) * vecprd((w),(w)))) \
)

static int
pdf_coord__equal(const PDFCoord *p1, const PDFCoord *p2)
{
  	if (fabs(p1->x - p2->x) < 1.e-7 &&
      	fabs(p1->y - p2->y) < 1.e-7)
    	return 1;
  	return 0;
}

#define COORD_EQUAL(p,q) pdf_coord__equal((p),(q))


static int
pdf_coord__transform(PDFCoord *p, const PDFTMatrix *M)
{
  	double x = p->x; 
  	double y = p->y;
  	p->x = x * M->a + y * M->c + M->e;
  	p->y = x * M->b + y * M->d + M->f;

  	return 0;
}

static /* __inline__ */ int
pdf_coord__dtransform(PDFCoord *p, const PDFTMatrix *M)
{
  	double x = p->x; 
  	double y = p->y;
  	p->x = x * M->a + y * M->c;
  	p->y = x * M->b + y * M->d;

  	return 0;
}

static int
pdf_coord__idtransform(PDFCoord *p, const PDFTMatrix *M)
{
  	PDFTMatrix W;  
  	int error = inversematrix(&W, M);
  	if (error) 
    	return error;

  	double x = p->x;  
  	double y = p->y;
  	p->x = x * W.a + y * W.c;
  	p->y = x * W.b + y * W.d;

  	return 0;
}

static void
pdf_invertmatrix(PDFTMatrix *M)
{
  	PDFTMatrix W;  
  	double det = detP(M);
  	
  	if (fabs(det) < 1.e-8) 
  	{
    	W.a = 1.0; W.c = 0.0;
    	W.b = 0.0; W.d = 1.0;
    	W.e = 0.0; W.f = 0.0;
  	} 
  	else 
  	{
    	W.a =  (M->d) / det;  W.b = -(M->b) / det;
    	W.c = -(M->c) / det;  W.d =  (M->a) / det;
    	W.e =  (M->c) * (M->f) - (M->d) * (M->e);
    	W.f =  (M->b) * (M->e) - (M->a) * (M->f);
  	}

  	pdf_copymatrix(M, &W);
}

static const struct {
  char        opchr;  /* PDF operator char  */
  int         n_pts;  /* number of *points* */
  const char *strkey;
} petypes[] = {
#define PE_TYPE__INVALID  -1
#define PE_TYPE__MOVETO    0
  {'m', 1, "moveto"  },
#define PE_TYPE__LINETO    1 
  {'l', 1, "lineto"  },
#define PE_TYPE__CURVETO   2 
  {'c', 3, "curveto" },
  /* no PS correspondence for v and y */
#define PE_TYPE__CURVETO_V 3 
  {'v', 2, "vcurveto"}, /* current point replicated */
#define PE_TYPE__CURVETO_Y 4 
  {'y', 2, "ycurveto"}, /* last point replicated */
#define PE_TYPE__CLOSEPATH 5
  {'h', 0, "closepath"}, 
#define PE_TYPE__TERMINATE 6
  {' ', 0,  NULL}
};


#define PE_VALID(p) ((p) && \
  (p)->type > PE_TYPE__INVALID && (p)->type < PE_TYPE__TERMINATE)
#define PE_N_PTS(p)  (PE_VALID((p)) ? petypes[(p)->type].n_pts : 0)
#define PE_OPCHR(p)  (PE_VALID((p)) ? petypes[(p)->type].opchr : ' ')

#define GS_FLAG_CURRENTPOINT_SET (1 << 0)

#define PT_OP_VALID(c) ( \
 (c) == 'f' || (c) == 'F' || \
 (c) == 's' || (c) == 'S' || \
 (c) == 'b' || (c) == 'B' || \
 (c) == 'W' \
)

static int
INVERTIBLE_MATRIX(const PDFTMatrix *M)
{
  	if (fabs(detP(M)) < 1.e-8) 
    	return -1;
  	return 0;
}

XWDVIPath::XWDVIPath()
	:num_paths(0),
	 max_paths(0),
	 path(0)
{
}

XWDVIPath::XWDVIPath(const XWDVIPath * p0)
	:num_paths(0),
	 max_paths(0),
	 path(0)
{
	copy(p0);
}

XWDVIPath::~XWDVIPath()
{
	clear();
}

void XWDVIPath::clear()
{
	if (path)
    	free(path);
  	path = 0;
  	num_paths = 0;
  	max_paths = 0;
}

int XWDVIPath::close(PDFCoord *cp)
{
	PathElement * pe = 0;
	int i = num_paths - 1;
	for (; i >= 0; i--) 
	{
    	pe = &path[i];
    	if (pe->type == PE_TYPE__MOVETO)
      		break;
  	}
  	
  	if (!pe || i < 0)
    	return -1;
    	
    cp->x = pe->p[0].x;
  	cp->y = pe->p[0].y;

  	grow(num_paths + 1);
  	pe = &path[num_paths++];
  	pe->type = PE_TYPE__CLOSEPATH;

  	return 0;
}

void XWDVIPath::copy(const XWDVIPath * p0)
{
	grow(p0->num_paths);
  	for (int i = 0; i < p0->num_paths; i++) 
  	{
    	PathElement * pe1 = &(path[i]);
    	PathElement * pe0 = &(p0->path[i]);
    	/* FIXME */
    	pe1->type   = pe0->type;
    	pe1->p[0].x = pe0->p[0].x;
    	pe1->p[0].y = pe0->p[0].y;
    	pe1->p[1].x = pe0->p[1].x;
    	pe1->p[1].y = pe0->p[1].y;
    	pe1->p[2].x = pe0->p[2].x;
    	pe1->p[2].y = pe0->p[2].y;
  	}
  	num_paths = p0->num_paths;
}

int XWDVIPath::curveTo(PDFCoord       *cp,
                       const PDFCoord *p0,
                       const PDFCoord *p1,
                       const PDFCoord *p2)
{
	PathElement * pe = nextPE(cp);
  	if (COORD_EQUAL(cp, p0)) 
  	{
    	pe->type   = PE_TYPE__CURVETO_V;
    	pe->p[0].x = p1->x;
    	pe->p[0].y = p1->y;
    	pe->p[1].x = cp->x = p2->x;
    	pe->p[1].y = cp->y = p2->y;
  	} 
  	else if (COORD_EQUAL(p1, p2)) 
  	{
    	pe->type   = PE_TYPE__CURVETO_Y;
    	pe->p[0].x = p0->x;
    	pe->p[0].y = p0->y;
    	pe->p[1].x = cp->x = p1->x;
    	pe->p[1].y = cp->y = p1->y;
  	} 
  	else 
  	{
    	pe->type   = PE_TYPE__CURVETO;
    	pe->p[0].x = p0->x;
    	pe->p[0].y = p0->y;
    	pe->p[1].x = p1->x;
    	pe->p[1].y = p1->y;
    	pe->p[2].x = cp->x = p2->x;
    	pe->p[2].y = cp->y = p2->y;
  	}

  	return 0;
}

int  XWDVIPath::ellipTArc(PDFCoord        *cp,
                          const PDFCoord *ca, 
                          double          r_x,
                          double          r_y,
                          double          xar,
                          double          a_0,
                          double          a_1,
                          int             a_d)
{
	if (fabs(r_x) < 1.e-8 || fabs(r_y) < 1.e-8)
    	return -1;
    	
    if (a_d < 0) 
    {
    	for ( ; a_1 > a_0; a_1 -= 360.0);
  	} 
  	else 
  	{
    	for ( ; a_1 < a_0; a_0 -= 360.0);
  	}
  	
  	double d_a  = a_1 - a_0;
  	int n_c = 1;
  	for (; fabs(d_a) > 90.0 * n_c; n_c++);
  	d_a /= n_c;
  	if (fabs(d_a) < 1.e-8)
    	return -1;

  	a_0 *= M_PI / 180.0;
  	a_1 *= M_PI / 180.0;
  	d_a *= M_PI / 180.0;
  	xar *= M_PI / 180.0;
  	PDFTMatrix T;
  	T.a  = cos(xar);  T.c = -sin(xar);
  	T.b  = -T.c    ;  T.d = T.a;
  	T.e  = 0.0     ;  T.f = 0.0;
  	
  	double b    = 4.0 * (1.0 - cos(.5 * d_a)) / (3.0 * sin(.5 * d_a));
  	double b_x  = r_x * b;
  	double b_y  = r_y * b;
  	
  	PDFCoord p0;
  	p0.x = r_x * cos(a_0);
  	p0.y = r_y * sin(a_0);
  	pdf_coord__transform(&p0, &T);
  	p0.x += ca->x; p0.y += ca->y;
  	if (num_paths == 0) 
    	moveTo(cp, &p0);
  	else if (!COORD_EQUAL(cp, &p0)) 
    	lineTo(cp, &p0);
    	
  	int error = 0;
  	PDFCoord p1, p2, p3, e0, e1;
  	for (int i = 0; !error && i < n_c; i++) 
  	{
    	double q = a_0 + i * d_a;
    	e0.x = cos(q); e0.y = sin(q);
    	e1.x = cos(q + d_a); e1.y = sin(q + d_a);
    	p0.x = r_x * e0.x; /* s.p. */
    	p0.y = r_y * e0.y;
    	p3.x = r_x * e1.x; /* e.p. */
    	p3.y = r_y * e1.y;

    	p1.x = -b_x * e0.y;
    	p1.y =  b_y * e0.x;
    	p2.x =  b_x * e1.y;
    	p2.y = -b_y * e1.x;

    	pdf_coord__transform(&p0, &T);
    	pdf_coord__transform(&p1, &T);
    	pdf_coord__transform(&p2, &T);
    	pdf_coord__transform(&p3, &T);
    	p0.x += ca->x; p0.y += ca->y;
    	p3.x += ca->x; p3.y += ca->y;
    	p1.x += p0.x ; p1.y += p0.y ;
    	p2.x += p3.x ; p2.y += p3.y ;

    	error = curveTo(&p0, &p1, &p2, &p3);
    	cp->x = p3.x; cp->y = p3.y;
  	}

  	return error;
}

int XWDVIPath::flush(XWDVIRef * xref,
	                 XWDVIDev * dev,
	                 char      *b,
	                 char       opchr,
                     int        rule,
                     int        ignore_rule)
{
	int len = 0;
	PDFRect r;
	if (isRect(ignore_rule))
	{
		PathElement * pe  = &(path[0]);
    	PathElement * pe1 = &(path[2]);

    	r.llx = pe->p[0].x;
    	r.lly = pe->p[0].y;
    	r.urx = pe1->p[0].x - pe->p[0].x; /* width...  */
    	r.ury = pe1->p[0].y - pe->p[0].y; /* height... */

   		b[len++] = ' ';
   		len += dev->sprintRect(b + len, &r);
   		b[len++] = ' ';
   		b[len++] = 'r';
   		b[len++] = 'e';
   		xref->addPageContent(b, len);  /* op: re */
   		len = 0;
	}
	else
	{
		int n_seg = length();
		PathElement * pe = &path[0];
    	for (int i = 0; i < n_seg; i++) 
    	{
      		int n_pts = PE_N_PTS(pe);
      		PDFCoord * pt = &pe->p[0];
      		for (int j = 0; j < n_pts; j++) 
      		{
        		b[len++] = ' ';
        		len += dev->sprintCoord(b + len, pt);
        		pt++;
      		}
      		b[len++] = ' ';
      		b[len++] = PE_OPCHR(pe);
      		if (len + 128 > 1024) 
      		{
        		xref->addPageContent(b, len);  /* op: m l c v y h */
				len = 0;
      		}
      		
      		pe++;
    	}
    	
    	if (len > 0) 
    	{
      		xref->addPageContent(b, len);  /* op: m l c v y h */
      		len = 0;
    	}
	}
	
	b[len++] = ' ';
  	b[len++] = opchr;
  	if (rule == PDF_FILL_RULE_EVENODD)
    	b[len++] = '*';

  	xref->addPageContent(b, len);
  	
  	return 0;
}

int XWDVIPath::isRect(int f_ir)
{
	if (num_paths == 5)
	{
		PathElement * pe0 = &(path[0]);
    	PathElement * pe1 = &(path[1]);
    	PathElement * pe2 = &(path[2]);
    	PathElement * pe3 = &(path[3]);
    	PathElement * pe4 = &(path[4]);
    	if (pe0->type == PE_TYPE__MOVETO &&
        	pe1->type == PE_TYPE__LINETO &&
        	pe2->type == PE_TYPE__LINETO &&
        	pe3->type == PE_TYPE__LINETO &&
        	pe4->type == PE_TYPE__CLOSEPATH)
        {
        	if (pe1->p[0].y - pe0->p[0].y == 0 &&
          		pe2->p[0].x - pe1->p[0].x == 0 &&
          		pe3->p[0].y - pe2->p[0].y == 0)
          	{
          		if (pe1->p[0].x - pe0->p[0].x == pe2->p[0].x - pe3->p[0].x) 
          			return 1;
          	}
          	else if (f_ir && 
                 	 pe1->p[0].x - pe0->p[0].x == 0 &&
                 	 pe2->p[0].y - pe1->p[0].y == 0 &&
                     pe3->p[0].x - pe2->p[0].x == 0)
			{
				if (pe1->p[0].y - pe0->p[0].y == pe2->p[0].y - pe3->p[0].y) 
          			return 1;
			}
        }
	}
	
	return 0;
}

int XWDVIPath::lineTo(PDFCoord *cp, const PDFCoord *p0)
{
	PathElement * pe = nextPE(cp);
  	pe->type   = PE_TYPE__LINETO;
  	pe->p[0].x = cp->x = p0->x;
  	pe->p[0].y = cp->y = p0->y;

  	return 0;
}

int XWDVIPath::moveTo(PDFCoord *cp, const PDFCoord *p0)
{
	grow(num_paths + 1);
	PathElement * pe = 0;
  	if (num_paths > 0) 
  	{
    	pe = &path[num_paths-1];
    	if (pe->type == PE_TYPE__MOVETO) 
    	{
      		pe->p[0].x = cp->x = p0->x;
      		pe->p[0].y = cp->y = p0->y;
      		return 0;
    	}
  	}
  	pe = &path[num_paths++];
  	pe->type   = PE_TYPE__MOVETO;
  	pe->p[0].x = cp->x = p0->x;
  	pe->p[0].y = cp->y = p0->y;

  	return 0;  
}

int XWDVIPath::transform(const PDFTMatrix *M)
{
	for (int i = 0; i < num_paths; i++) 
	{
    	PathElement * pe = &(path[i]);
    	int n  = PE_N_PTS(pe);
    	while (n-- > 0)
      		pdf_coord__transform(&(pe->p[n]), M);
  	}

  	return 0;
}

int XWDVIPath::grow(int max_pe)
{
	if (max_pe < max_paths)
    	return 0;

  	max_paths = qMax(max_paths + 8, max_pe);
  	path = (PathElement*)realloc(path, max_paths * sizeof(PathElement));

  	return 0;
}

PathElement * XWDVIPath::nextPE(const PDFCoord *cp)
{
	grow(num_paths + 2);
	PathElement * pe = 0;
  	if (num_paths == 0) 
  	{
    	pe = &path[num_paths++];
    	pe->type   = PE_TYPE__MOVETO;
    	pe->p[0].x = cp->x;
    	pe->p[0].y = cp->y;

    	return &path[num_paths++];
  	}
    
  	pe = &path[num_paths-1];
  	switch (pe->type) 
  	{
  		case PE_TYPE__MOVETO:
    		pe->p[0].x = cp->x;
    		pe->p[0].y = cp->y;
    		break;
    		
  		case PE_TYPE__LINETO:
    		if (!COORD_EQUAL(&pe->p[0], cp)) 
    		{
      			pe = &path[num_paths++];
      			pe->type   = PE_TYPE__MOVETO;
      			pe->p[0].x = cp->x;
      			pe->p[0].y = cp->y;
    		}
    		break;
    		
  		case PE_TYPE__CURVETO:
    		if (!COORD_EQUAL(&pe->p[2], cp)) 
    		{
      			pe = &path[num_paths++];
      			pe->type   = PE_TYPE__MOVETO;
      			pe->p[0].x = cp->x;
      			pe->p[0].y = cp->y;
    		}
    		break;
    		
  		case PE_TYPE__CURVETO_Y:
  		case PE_TYPE__CURVETO_V:
    		if (!COORD_EQUAL(&pe->p[1], cp)) 
    		{
      			pe = &path[num_paths++];
      			pe->type   = PE_TYPE__MOVETO;
      			pe->p[0].x = cp->x;
      			pe->p[0].y = cp->y;
    		}
    		break;
    		
  		case PE_TYPE__CLOSEPATH:
    		pe = &path[num_paths++];
    		pe->type   = PE_TYPE__MOVETO;
    		pe->p[0].x = cp->x;
    		pe->p[0].y = cp->y;
    		break;
  	}

  	return &path[num_paths++];
}

XWDVIGState::XWDVIGState()
{
	init();
}

XWDVIGState::XWDVIGState(XWDVIGState * gs2)
{
	copy(gs2);
}

int XWDVIGState::arc(double c_x , double c_y, double r,
                     double a_0 , double a_1)
{
	PDFCoord   c;
  	c.x = c_x; c.y = c_y;
  	return  path.ellipTArc(&cp, &c, r, r, 0.0, a_0, a_1, +1);
}

int XWDVIGState::arcn(double c_x , double c_y, double r,
                      double a_0 , double a_1)
{
  	PDFCoord   c;
  	c.x = c_x; c.y = c_y;
  	return  path.ellipTArc(&cp, &c, r, r, 0.0, a_0, a_1, -1);
}

int XWDVIGState::arcx(double c_x , double c_y,
                      double r_x , double r_y,
                      double a_0 , double a_1,
                      int    a_d ,
                      double xar)
{
  	PDFCoord   c;
  	c.x = c_x; c.y = c_y;
  	return  path.ellipTArc(&cp, &c, r_x, r_y, xar, a_0, a_1, a_d);
}

int  XWDVIGState::bspline(double x0, double y0,
                          double x1, double y1, 
                          double x2, double y2)
{
	PDFCoord  p1, p2, p3;  	
  	p1.x = x0 + 2.0 * (x1 - x0) / 3.0;
  	p1.y = y0 + 2.0 * (y1 - y0) / 3.0;
  	p2.x = x1 + (x2 - x1) / 3.0;
  	p2.y = y1 + (y2 - y1) / 3.0;
  	p3.x = x2;
  	p3.y = y2;
  	return  path.curveTo(&cp, &p1, &p2, &p3);
}

void XWDVIGState::clear()
{
	path.clear();
	init();
}

int XWDVIGState::closePath()
{
	return path.close(&cp);
}

void XWDVIGState::copy(XWDVIGState * gs2)
{
	cp.x = gs2->cp.x;
  	cp.y = gs2->cp.y;
  	pdf_copymatrix(&matrix, &(gs2->matrix));
  	path.copy(&(gs2->path));
  	
  	linedash.num_dash = gs2->linedash.num_dash;
  	for (int i = 0; i < gs2->linedash.num_dash; i++) 
    	linedash.pattern[i] = gs2->linedash.pattern[i];
    	
  	linedash.offset = gs2->linedash.offset;

  	linecap    = gs2->linecap;
  	linejoin   = gs2->linejoin;
  	linewidth  = gs2->linewidth;
  	miterlimit = gs2->miterlimit;
  	flatness   = gs2->flatness;

  	fillcolor.copy(&(gs2->fillcolor));
  	strokecolor.copy(&(gs2->strokecolor));
}

int XWDVIGState::curveTo(double x0, double y0,
                         double x1, double y1,
                         double x2, double y2)
{
  	PDFCoord   p0, p1, p2;  	
  	p0.x = x0; p0.y = y0;
  	p1.x = x1; p1.y = y1;
  	p2.x = x2; p2.y = y2;
  	return path.curveTo(&cp, &p0, &p1, &p2);
}

void XWDVIGState::dtransform(PDFCoord *p, const PDFTMatrix *M)
{
	pdf_coord__dtransform(p, (M ? M : &matrix));
}

void XWDVIGState::getMatrix(PDFTMatrix *M)
{
	if (M)
		pdf_copymatrix(M, &matrix);
}

void XWDVIGState::idtransform(PDFCoord *p, const PDFTMatrix *M)
{
	pdf_coord__idtransform(p, (M ? M : &matrix));
}

int XWDVIGState::lineTo(double x, double y)
{
  	PDFCoord   p0;  	
  	p0.x = x; p0.y = y;
  	return path.lineTo(&cp, &p0);
}

int XWDVIGState::moveTo(double x, double y)
{
  	PDFCoord   p;
  	p.x = x; p.y = y;
  	return path.moveTo(&cp, &p);
}

int XWDVIGState::rcurveTo(double x0, double y0,
                          double x1, double y1,
                          double x2, double y2)
{
	PDFCoord   p0, p1, p2;  	
  	p0.x = x0 + cp.x; p0.y = y0 + cp.y;
  	p1.x = x1 + cp.x; p1.y = y1 + cp.y;
  	p2.x = x2 + cp.x; p2.y = y2 + cp.y;

  	return path.curveTo(&cp, &p0, &p1, &p2);
}

int XWDVIGState::rlineTo(double x, double y)
{
	PDFCoord   p0;  	
  	p0.x = x + cp.x; p0.y = y + cp.y;
  	return path.lineTo(&cp, &p0);
}

int XWDVIGState::rmoveTo(double x, double y)
{
	PDFCoord   p;  	
  	p.x = cp.x + x;
  	p.y = cp.y + y;
  	return path.moveTo(&cp, &p);
}

void XWDVIGState::setLineDash(int count, double *pattern, double offset)
{
	linedash.num_dash = count;
  	linedash.offset   = offset;
  	for (int i = 0; i < count; i++)
  		linedash.pattern[i] = pattern[i];
}

void XWDVIGState::transform(PDFCoord *p, const PDFTMatrix *M)
{
	pdf_coord__transform(p, (M ? M : &matrix));
}

void XWDVIGState::init()
{
	cp.x = 0.0;
	cp.y = 0.0;
	pdf_setmatrix(&matrix, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
	strokecolor.black();
	fillcolor.black();
	linedash.num_dash = 0;
  	linedash.offset   = 0;
  	linecap    = 0;
  	linejoin   = 0;
  	linewidth  = 1.0;
  	miterlimit = 10.0;

  	flatness   = 1;
  	flags = 0;
}

XWDVIGStateStack::XWDVIGStateStack()
	:size(0),
	 top(0),
	 bottom(0)
{
	XWDVIGState * gs = new XWDVIGState;
	push(gs);
}

XWDVIGStateStack::~XWDVIGStateStack()
{
	XWDVIGState * gs = 0;
	while ((gs = pop()) != 0) 
		delete gs;
}

XWDVIGState * XWDVIGStateStack::getTop()
{
	if (size == 0)
    	return 0;

  	XWDVIGState * gs = top->data;

  	return gs;
}

XWDVIGState * XWDVIGStateStack::pop()
{
	if (size == 0)
    	return 0;
    	
    XWDVIGState * data = top->data;
  	StackElement * elem = top;
  	top = elem->prev;
  	if (size == 1)
    	bottom = 0;
  	delete elem;

  	size--;

  	return data;
}

void XWDVIGStateStack::push(XWDVIGState * data)
{
	StackElement * elem = new StackElement;
	elem->prev = top;
  	elem->data = data;

  	top = elem;
  	if (size == 0)
    	bottom = elem;

  	size++;
}

int XWDVIGStateStack::restore()
{
	XWDVIGState * gs = pop();
	if (gs)
		delete gs;
		
	return 0;
}

void XWDVIGStateStack::save()
{
	XWDVIGState * gs0 = getTop();
	if (!gs0)
		return ;
		
  	XWDVIGState * gs1 = new XWDVIGState(gs0);
  	push(gs1);
}

static unsigned long ten_pow[10] = {
  1ul, 10ul, 100ul, 1000ul, 10000ul, 100000ul, 1000000ul, 10000000ul, 100000000ul, 1000000000ul
};

static double ten_pow_inv[10] = {
  1.0, 0.1,  0.01,  0.001,  0.0001,  0.00001,  0.000001,  0.0000001,  0.00000001,  0.000000001
};

#define DEV_PRECISION_MAX  8

#define dround_at(v,p) (round( (v), ten_pow_inv[(p)] ))

#define GRAPHICS_MODE  1
#define TEXT_MODE      2
#define STRING_MODE    3

#define FORMAT_BUF_SIZE 4096

#define TEXT_WMODE_HH 0
#define TEXT_WMODE_HV 1
#define TEXT_WMODE_VH 2
#define TEXT_WMODE_VV 3

#define ANGLE_CHANGES(m1,m2) ((abs((m1)-(m2)) % 3) == 0 ? 0 : 1)
#define ROTATE_TEXT(m)       ((m) != TEXT_WMODE_HH && (m) != TEXT_WMODE_VV)


#define PDF_FONTTYPE_SIMPLE    1
#define PDF_FONTTYPE_BITMAP    2
#define PDF_FONTTYPE_COMPOSITE 3


static unsigned char sbuf0[FORMAT_BUF_SIZE];
static unsigned char sbuf1[FORMAT_BUF_SIZE];

#define PDF_LINE_THICKNESS_MAX 5.0

#define WORD_SPACE_MAX(f) (long) (3.0 * (f)->extend * (f)->sptsize)

class DevFont
{
public:
	DevFont();
	~DevFont();
	
	void close(XWDVIRef * xref);
	
	int handleMultiByteString(unsigned char **str_ptr, 
	                          int *str_len, 
	                          int ctype);
	
public:
	char     short_name[7];
	int      used_on_this_page;
	char   * tex_name;
	long     sptsize;
	int      font_id;
	int      enc_id;
	XWObject resource;
	char   * used_chars;
	int      format;
	int      wmode;
	double   extend;
  	double   slant;
  	double   bold;
  	int      mapc;
  	int      ucs_group;
  	int      ucs_plane;

  	int      is_unicode;
  	ushort * ft_to_gid;
};

DevFont::DevFont()
{
	memset(short_name, 0, 7);
	used_on_this_page = 0;
	tex_name = 0;
	sptsize = 0;
	font_id = -1;
	enc_id = -1;
	resource.initNull();
	used_chars = 0;
	format = 0;
	wmode = 0;
	extend = 1.0;
	slant = 0.0;
	bold = 0.0;
	mapc = -1;
	ucs_group = 0;
	ucs_plane = 0;
	is_unicode = 0;
	ft_to_gid = 0;
}

DevFont::~DevFont()
{
	if (tex_name)
		delete [] tex_name;
}

void DevFont::close(XWDVIRef * xref)
{
	if (!resource.isNull())
		xref->releaseObj(&resource);
}

int DevFont::handleMultiByteString(unsigned char **str_ptr, 
	                               int *str_len, 
	                               int ctype)
{
	uchar * p      = *str_ptr;
  	int length = *str_len;
  	
  	if (is_unicode)
  	{
  		if (ctype == 1) 
  		{
      		if (length * 4 >= FORMAT_BUF_SIZE) 
        		return -1;
      		
      		for (int i = 0; i < length; i++) 
      		{
        		sbuf1[i*4  ] = ucs_group;
        		sbuf1[i*4+1] = ucs_plane;
        		sbuf1[i*4+2] = '\0';
        		sbuf1[i*4+3] = p[i];
      		}
      		length *= 4;
    	} 
    	else if (ctype == 2) 
    	{
      		if (length * 2 >= FORMAT_BUF_SIZE) 
        		return -1;
      		
      		for (int i = 0; i < length; i += 2) 
      		{
        		sbuf1[i*2  ] = ucs_group;
        		sbuf1[i*2+1] = ucs_plane;
        		sbuf1[i*2+2] = p[i];
        		sbuf1[i*2+3] = p[i+1];
      		}
      		length *= 2;
    	}
    	p = sbuf1;
  	}
  	else if (ctype == 1 && mapc >= 0)
  	{
  		if (length * 2 >= FORMAT_BUF_SIZE) 
      		return -1;
    	
    	for (int i = 0; i < length; i++) 
    	{
      		sbuf1[i*2  ] = (mapc & 0xff);
      		sbuf1[i*2+1] = p[i];
    	}
    	length *= 2;
    	p       = sbuf1;
  	}
  	else if (ft_to_gid && ctype == -1)
  	{
  			uchar *inbuf = p;
      	uchar *outbuf = sbuf0;
      	for (int i = 0; i < length; i += 2)
      	{
      		uint gid = *inbuf++ << 8;
        	gid += *inbuf++;
        	gid = ft_to_gid[gid];
        	*outbuf++ = gid >> 8;
        	*outbuf++ = gid & 0xff;
      	}
      	
      	 p = sbuf0;
      	length = outbuf - sbuf0;
  	}
  	
  	if (enc_id >= 0) 
  	{
    	XWFontCache fcache(true);
    	XWCMap * cmap = fcache.getCMap(enc_id);
    	uchar * inbuf        = p;
    	uchar * outbuf       = sbuf0;
    	long inbytesleft  = length;
    	long outbytesleft = FORMAT_BUF_SIZE;

    	cmap->decode((const uchar **) &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    	if (inbytesleft != 0) 
      		return -1;
    	
    	length  = FORMAT_BUF_SIZE - outbytesleft;
    	p       = sbuf0;
  	}

  	*str_ptr = p;
  	*str_len = length;
  	return 0;
}

#define CURRENTFONT() ((text_state.font_id < 0) ? NULL : (dev_fonts[text_state.font_id]))
#define GET_FONT(n)   ((dev_fonts[(n)]))
	
XWDVIDev::XWDVIDev(XWDVICore * coreA, XWDVIRef * xref)
{
	core = coreA;
	dev_unit.dvi2pts = 1.52018;
	dev_unit.min_bp_val = 658;
	dev_unit.precision = 2;
	dev_param.autorotate = 1;
	dev_param.colormode = 1;
	
	motion_state = GRAPHICS_MODE;
	
	format_buffer = (char*)malloc(FORMAT_BUF_SIZE * sizeof(char));
	
  dev_fonts = 0;
  num_dev_fonts   = 0;
  max_dev_fonts   = 0;
  num_phys_fonts  = 0;
  	
  dev_coords = 0;
  num_dev_coords = 0;
  max_dev_coords = 0;
  	
  gs_stack = 0;
  color_stack = 0;
  	
  dev_unit.min_bp_val   = (long) round(1.0/(ten_pow[dev_unit.precision]*dev_unit.dvi2pts), 1);
  if (dev_unit.min_bp_val < 0)
   	dev_unit.min_bp_val = -dev_unit.min_bp_val;

  dev_param.colormode = 1;

  clear(xref);
}

XWDVIDev::~XWDVIDev()
{
	if (format_buffer)
		free(format_buffer);
		
	if (gs_stack)
		delete gs_stack;
		
	if (color_stack)
		delete color_stack;
		
	if (dev_fonts)
	{
		for (int i = 0; i < num_dev_fonts; i++)
		{
			if (dev_fonts[i])
				delete dev_fonts[i];
		}
		
		free(dev_fonts);
	}
	
	if (dev_coords)
		free(dev_coords);
}

int XWDVIDev::arc(double c_x , double c_y, double r,
                  double a_0 , double a_1)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
  	return gs->arc(c_x, c_y, r, a_0, a_1);
}

int XWDVIDev::arcn(double c_x, double c_y, double r,
                   double a_0, double a_1)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
  	return gs->arcn(c_x, c_y, r, a_0, a_1);
}

int XWDVIDev::arcx(double c_x , double c_y,
                   double r_x , double r_y,
                   double a_0 , double a_1,
                   int    a_d ,
                   double xar)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
  	return gs->arcx(c_x, c_y, r_x, r_y, a_0, a_1, a_d, xar);
}

void XWDVIDev::bop(XWDVIRef * xref, const PDFTMatrix *M)
{
	graphicsMode(xref);
  	text_state.force_reset  = 0;

  	gsave(xref);
  	concat(xref, M);

  	resetFonts();
  	resetColor(xref, 0);
}

int XWDVIDev::bspline(double x0, double y0,
                      double x1, double y1, 
                      double x2, double y2)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
  	return gs->bspline(x0, y0, x1, y1, x2, y2);
}

void XWDVIDev::clear(XWDVIRef * xref)
{
	motion_state = GRAPHICS_MODE;
	
	text_state.font_id = -1;
	text_state.offset = 0;
	text_state.ref_x = 0;
  text_state.ref_y = 0;
  text_state.raise = 0;
  text_state.leading = 0;

  text_state.matrix.slant = 0.0;
  text_state.matrix.extend = 1.0;
  text_state.matrix.rotate = 0.0;
  		
  text_state.bold_param = 0.0;
  text_state.dir_mode = 0;
  text_state.force_reset = 0;
  text_state.is_mb = 0;
	
	if (dev_coords)
		free(dev_coords);
  	
  dev_coords = 0;
  num_dev_coords = 0;
  max_dev_coords = 0;
  
  if (gs_stack)
		delete gs_stack;
		
	if (color_stack)
		delete color_stack;
		
	gs_stack = new XWDVIGStateStack;
  color_stack = new XWDVIColorStack(this);
  
  graphicsMode(xref);
}

void XWDVIDev::clearColorStack()
{
	color_stack->clear();
}

int XWDVIDev::clip(XWDVIRef * xref)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
  	XWDVIPath    *cpa = gs->getPath();
  	return flushPath(xref, cpa, 'W', PDF_FILL_RULE_NONZERO, 0);
}

int XWDVIDev::closePath()
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
  	XWDVIPath    *cpa = gs->getPath();
  	PDFCoord  *cpt = gs->getCurrentPoint();
  	return cpa->close(cpt);
}

int XWDVIDev::concat(XWDVIRef * xref, const PDFTMatrix *M)
{
	XWDVIGState  *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
  	XWDVIPath    *cpa = gs->getPath();
  	PDFCoord   *cpt = gs->getCurrentPoint();
  	PDFTMatrix *CTM = gs->getCTM();
  	if (fabs(detP(M)) < 1.0e-8) 
    	return -1;
    	
    int len = 0;
    format_buffer[len++] = ' ';
  	len += sprintMatrix(format_buffer + len, M);
  	format_buffer[len++] = ' ';
  	format_buffer[len++] = 'c';
  	format_buffer[len++] = 'm';
  	xref->addPageContent(format_buffer, len);  /* op: cm */

  	pdf_concatmatrix(CTM, M);

	PDFTMatrix W;
  	inversematrix(&W, M);

  	cpa->transform(&W);
  	pdf_coord__transform(cpt, &W);

  	return 0;
}

int XWDVIDev::currentMatrix(PDFTMatrix *M)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
	gs->getMatrix(M);
		
	return 0;
}

int XWDVIDev::currentPoint(PDFCoord *p)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
  	PDFCoord  *cpt = gs->getCurrentPoint();

  	p->x = cpt->x; p->y = cpt->y;

  	return 0;
}

int XWDVIDev::curveTo(double x0, double y0,
                      double x1, double y1,
                      double x2, double y2)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
	return gs->curveTo(x0, y0, x1, y1, x2, y2);
}

void XWDVIDev::dtransform(PDFCoord *p, const PDFTMatrix *M)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return ;
		
	gs->dtransform(p, M);
}

int XWDVIDev::eoclip(XWDVIRef * xref)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
  	XWDVIPath    *cpa = gs->getPath();
  	return flushPath(xref, cpa, 'W', PDF_FILL_RULE_EVENODD, 0);
}

void XWDVIDev::eop(XWDVIRef * xref)
{
	graphicsMode(xref);
  	int depth = currentDepth();
  	if (depth != 1) 
    	grestoreTo(xref, 0);
  	else 
    	grestore(xref);
}

int XWDVIDev::flushPath(XWDVIRef * xref, char p_op, int fill_rule)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
  	XWDVIPath    *cpa = gs->getPath();
  	int error = flushPath(xref, cpa, p_op, fill_rule, 1);
  	cpa->clear();
	int f = gs->getFlags();
  	f &= ~GS_FLAG_CURRENTPOINT_SET;
  	gs->setFlags(f);
  	return error;
}

void XWDVIDev::getCoord(double *xpos, double *ypos)
{
	if (num_dev_coords > 0) 
	{
    	*xpos = dev_coords[num_dev_coords-1].x;
    	*ypos = dev_coords[num_dev_coords-1].y;
  	} 
  	else 
  	{
    	*xpos = *ypos = 0.0;
  	}
}

void XWDVIDev::getCurrentColor(XWDVIColor **sc, XWDVIColor **fc)
{
	color_stack->getCurrent(sc, fc);
}

int XWDVIDev::getFontWMode(int font_id)
{
	DevFont * font = GET_FONT(font_id);
  	if (font) 
    	return font->wmode;

  	return 0;
}

int XWDVIDev::getParam(int param_type)
{
	int value = 0;
	switch (param_type) 
	{
  		case PDF_DEV_PARAM_AUTOROTATE:
    		value = dev_param.autorotate;
    		break;
    		
  		case PDF_DEV_PARAM_COLORMODE:
    		value = dev_param.colormode;
    		break;
    		
  		default:
  			break;
  	}

  	return value;
}

void XWDVIDev::graphicsMode(XWDVIRef * xref)
{
	switch (motion_state) 
	{
  		case GRAPHICS_MODE:
    		break;
    		
  		case STRING_MODE:
  			xref->addPageContent(text_state.is_mb ? ">]TJ" : ")]TJ", 4);  /* op: TJ */
  		case TEXT_MODE:
    		xref->addPageContent(" ET", 3);  /* op: ET */
    		text_state.force_reset =  0;
    		text_state.font_id     = -1;
    		break;
  	}
  	motion_state = GRAPHICS_MODE;
}

int XWDVIDev::grestore(XWDVIRef * xref)
{
	gs_stack->restore();
	xref->addPageContent(" Q", 2);  /* op: Q */
  	resetFonts();

  	return  0;
}

void XWDVIDev::grestoreTo(XWDVIRef * xref, int depth)
{
	while (gs_stack->depth() > depth + 1) 
	{
		xref->addPageContent(" Q", 2);  /* op: Q */
    	XWDVIGState  * gs = gs_stack->pop();
    	if (gs)
    		delete gs;
  	}
  	
  	resetFonts();
}

int XWDVIDev::gsave(XWDVIRef * xref)
{
	gs_stack->save();
	xref->addPageContent(" q", 2);

  	return 0;
}

void XWDVIDev::idtransform(PDFCoord *p, const PDFTMatrix *M)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return ;
		
	gs->idtransform(p, M);
}

int XWDVIDev::lineTo(double x, double y)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;

  	return gs->lineTo(x, y);
}

int XWDVIDev::locateFont(const char *font_name, long ptsize)
{
	if (!font_name || ptsize == 0)
    	return  -1;
    	
    DevFont * font = 0;
    for (int i = 0; i < num_dev_fonts; i++) 
    {
    	font = dev_fonts[i];
    	if (!strcmp(font_name, font->tex_name) && ptsize == font->sptsize) 
      		return  i;
  	}
  	
  	if (num_dev_fonts >= max_dev_fonts) 
  	{
    	max_dev_fonts += 16;
    	dev_fonts      = (DevFont**)realloc(dev_fonts, max_dev_fonts * sizeof(DevFont*));
  	}
  
  	XWFontCache fcache(true);
  	XWTexFontMap * mrec = fcache.lookupFontMapRecord(font_name);  	
  	font = new DevFont;
  	font->font_id = core->findFontResource(font_name, ptsize * dev_unit.dvi2pts, mrec);
  	if (font->font_id < 0)
  	{
  		delete font;
  		QString msg = QString("fail to find font file for tex font '%1'.\n").arg(font_name);
  		xwApp->error(msg);
    	return  -1;
    }
    dev_fonts[num_dev_fonts] = font;
    font->short_name[0] = 'F';
    intToAscii(num_phys_fonts + 1, &(font->short_name[1]));
    num_phys_fonts++;
    
    font->tex_name = qstrdup(font_name);
    font->sptsize  = ptsize;

  	switch (core->getFontSubtype(font->font_id)) 
  	{
  		case PDF_FONT_FONTTYPE_TYPE3:
    		font->format = PDF_FONTTYPE_BITMAP;
    		break;
    		
  		case PDF_FONT_FONTTYPE_TYPE0:
    		font->format = PDF_FONTTYPE_COMPOSITE;
    		break;
    		
  		default:
    		font->format = PDF_FONTTYPE_SIMPLE;
    		break;
  	}
  	
  	font->wmode  = core->getFontWMode(font->font_id);
  	font->enc_id = core->getFontEncoding(font->font_id);
  	
  	XWDVIFont * t0 = core->getDviFont(font->font_id);
  	font->ft_to_gid  = t0->getFtToGid(core);
  	
  	if (mrec) 
  	{
    	font->extend = mrec->opt->extend;
    	font->slant  = mrec->opt->slant;
    	font->bold   = mrec->opt->bold;
    	if (mrec->opt->mapc >= 0)
      		font->mapc = (mrec->opt->mapc >> 8) & 0xff;
    	
    	if (mrec->enc_name && !strcmp(mrec->enc_name, "unicode")) 
    	{
      	font->is_unicode   = 1;
      	if (mrec->opt->mapc >= 0) 
      	{
        	font->ucs_group  = (mrec->opt->mapc >> 24) & 0xff;
        	font->ucs_plane  = (mrec->opt->mapc >> 16) & 0xff;
      	}
    	} 
  	}

  	return  num_dev_fonts++;
}

int XWDVIDev::moveTo(double x, double y)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
	return gs->moveTo(x, y);
}

int XWDVIDev::newPath(XWDVIRef * xref)
{
	XWDVIGState  * gs = gs_stack->getTop();
	if (!gs)
		return -1;
		
	XWDVIPath    *p = gs->getPath();
	if (p->length() > 0) 
    	p->clear();
    	
  	xref->addPageContent(" n", 2);  /* op: n */

  	return 0;
}

void XWDVIDev::popColor(XWDVIRef * xref)
{
	color_stack->pop(xref);
}

void XWDVIDev::popCoord()
{
	if (num_dev_coords > 0) 
		num_dev_coords--;
}

int XWDVIDev::popGState()
{
	XWDVIGState * gs = gs_stack->pop();
	if (gs)
  		delete gs;

  	return  0;
}

void XWDVIDev::pushColor(XWDVIRef * xref, XWDVIColor *sc, XWDVIColor *fc)
{
	color_stack->push(xref, sc, fc);
}

void XWDVIDev::pushCoord(double xpos, double ypos)
{
	if (num_dev_coords >= max_dev_coords) 
	{
    	max_dev_coords += 4;
    	dev_coords = (PDFCoord*)realloc(dev_coords, max_dev_coords * sizeof(PDFCoord));
  	}
  	
  	dev_coords[num_dev_coords].x = xpos;
  	dev_coords[num_dev_coords].y = ypos;
  	num_dev_coords++;
}

int XWDVIDev::pushGState()
{
	XWDVIGState * gs0 = new XWDVIGState;
	gs_stack->push(gs0);
	return 0;
}

int  XWDVIDev::putImage(XWDVIRef * xref,
	                   int             id,
                       TransformInfo *p,
                       double          ref_x,
                       double          ref_y)
{
	if (num_dev_coords > 0) 
	{
    	ref_x -= dev_coords[num_dev_coords-1].x;
    	ref_y -= dev_coords[num_dev_coords-1].y;
  	}
  	
  	PDFTMatrix  M, M1;
  	pdf_copymatrix(&M, &(p->matrix));
  	M.e += ref_x; M.f += ref_y;
  	if (dev_param.autorotate && text_state.dir_mode) 
  	{
    	double tmp = -M.a; M.a = M.b; M.b = tmp;
    	tmp = -M.c; M.c = M.d; M.d = tmp;
  	}

  	graphicsMode(xref);
  	gsave(xref);
  	
  	PDFRect r;
  	core->scaleImage(id, &M1, &r, p);
  	pdf_concatmatrix(&M, &M1);
  	concat(xref, &M);
  	if (p->flags & INFO_DO_CLIP)
  		rectClip(xref, r.llx, r.lly, r.urx - r.llx, r.ury - r.lly);
  		
  	char * res_name = core->getImageResName(id);
  	int len = sprintf(format_buffer, " /%s Do", res_name);
  	xref->addPageContent(format_buffer, len);  /* op: Do */

  	grestore(xref);
  	if (xref->isScanning())
  	{
  		xref->addObjects(1);
		XWObject obj;
		core->getImageReference(id, &obj);
  		xref->addPageResource("XObject", res_name, &obj);
		obj.free();
	}
	
  	return 0;
}

int XWDVIDev::rcurveTo(double x0, double y0,
                       double x1, double y1,
                       double x2, double y2)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
	return gs->rcurveTo(x0, y0, x1, y1, x2, y2);
}

int XWDVIDev::rectClip(XWDVIRef * xref, 
	                   double x, double y,
                       double w, double h)
{
	PDFRect r;
  	r.llx = x;
  	r.lly = y;
  	r.urx = x + w;
  	r.ury = y + h;  
  	return  rectShape(xref, &r, NULL, 'W');
}

int XWDVIDev::rectFill(XWDVIRef * xref, 
	                   double x, double y,
                       double w, double h)
{
	PDFRect r;
  	r.llx = x;
  	r.lly = y;
  	r.urx = x + w;
  	r.ury = y + h;
  	return  rectShape(xref, &r, NULL, 'f');
}

void XWDVIDev::resetColor(XWDVIRef * xref, int force)
{
	XWDVIColor *sc, *fc;

  	color_stack->getCurrent(&sc, &fc);
  	setColor(xref, sc,    0, force);
  	setColor(xref, fc, 0x20, force);
}

void XWDVIDev::resetFonts()
{
	for (int i = 0; i < num_dev_fonts; i++) 
    	dev_fonts[i]->used_on_this_page = 0;

  	text_state.font_id       = -1;

  	text_state.matrix.slant  = 0.0;
  	text_state.matrix.extend = 1.0;
  	text_state.matrix.rotate = TEXT_WMODE_HH;

  	text_state.bold_param    = 0.0;

  	text_state.is_mb         = 0;
}

int XWDVIDev::rlineTo(double x, double y)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
	return gs->rlineTo(x, y);
}

int XWDVIDev::rmoveTo(double x, double y)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
	return gs->rmoveTo(x, y);
}

void XWDVIDev::setColor(XWDVIRef * xref, XWDVIColor *sc, XWDVIColor *fc)
{
	color_stack->set(xref, sc, fc);
}

void XWDVIDev::setColor(XWDVIRef * xref, 
	                    XWDVIColor *color, 
	                    char mask, 
	                    int force)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return ;
		
  	XWDVIColor *current = mask ? gs->getFillColor() : gs->getStrokeColor();
  	if (!(getParam(PDF_DEV_PARAM_COLORMODE) &&
		(force || color->compare(current))))
		return ;
		
	graphicsMode(xref);
  	int len = color->toString(format_buffer);
  	format_buffer[len++] = ' ';
  	switch (color->getType()) 
  	{
  		case  PDF_COLORSPACE_TYPE_RGB:
    		format_buffer[len++] = 'R' | mask;
    		format_buffer[len++] = 'G' | mask;
    		break;
    		
  		case  PDF_COLORSPACE_TYPE_CMYK:
    		format_buffer[len++] = 'K' | mask;
    		break;
    		
  		case  PDF_COLORSPACE_TYPE_GRAY:
    		format_buffer[len++] = 'G' | mask;
    		break;
    		
  		default: /* already verified the given color */
    		break;
  	}
  	xref->addPageContent(format_buffer, len);  /* op: RG K G rg k g */

  	current->copy(color);
}

void XWDVIDev::setDecimalDigits(int precision)
{
	if (precision < 0) 
   	dev_unit.precision  = 0;
  else if (precision > DEV_PRECISION_MAX) 
   	dev_unit.precision  = DEV_PRECISION_MAX;
  else 
   	dev_unit.precision  = precision;
   	
  dev_unit.min_bp_val   = (long) round(1.0/(ten_pow[dev_unit.precision]*dev_unit.dvi2pts), 1);
  if (dev_unit.min_bp_val < 0)
   	dev_unit.min_bp_val = -dev_unit.min_bp_val;
}

void XWDVIDev::setDirMode(int text_dir)
{
	DevFont * font = CURRENTFONT();
  	int vert_font = (font && font->wmode) ? 1 : 0;
  	int vert_dir = vert_font;
  	if (dev_param.autorotate) 
    	vert_dir = text_dir ? 1 : 0;
    		
  	int text_rotate = (vert_font << 1)|vert_dir;

  	if (font && ANGLE_CHANGES(text_rotate, text_state.matrix.rotate)) 
    	text_state.force_reset = 1;

  	text_state.matrix.rotate = text_rotate;
  	text_state.dir_mode      = text_dir;
}

void XWDVIDev::setDvi2Pts(double dvi2pts)
{
	dev_unit.dvi2pts = dvi2pts;
	dev_unit.min_bp_val   = (long) round(1.0/(ten_pow[dev_unit.precision]*dev_unit.dvi2pts), 1);
  if (dev_unit.min_bp_val < 0)
   	dev_unit.min_bp_val = -dev_unit.min_bp_val;
}

void XWDVIDev::setIgnoreColors(int is_bw)
{
	dev_param.colormode = (is_bw ? 0 : 1);
}

int XWDVIDev::setLineCap(XWDVIRef * xref, int capstyle)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
  	if (gs->getLineCap() != capstyle) 
  	{
  		int len = sprintf(format_buffer, " %d J", capstyle);
    	xref->addPageContent(format_buffer, len);  /* op: J */
    	gs->setLineCap(capstyle);
  	}

  	return 0;
}

int XWDVIDev::setLineDash(XWDVIRef * xref, 
	                      int count, 
	                      double *pattern, 
	                      double offset)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
	gs->setLineDash(count, pattern, offset);
	xref->addPageContent(" [", 2);  /* op: */
	int len = 0;
  	for (int i = 0; i < count; i++) 
  	{
  		format_buffer[0] = ' ';
    	len = sprintLength(format_buffer + 1, pattern[i]);
    	xref->addPageContent(format_buffer, len + 1);
  	}
  	
  	xref->addPageContent("] ", 2);  /* op: */
  	len = sprintLength(format_buffer, offset);
  	xref->addPageContent(format_buffer, len);  /* op: */
  	xref->addPageContent(" d", 2);  /* op: d */

  	return 0;
}

int XWDVIDev::setLineJoin(XWDVIRef * xref, int joinstyle)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
  	if (gs->getLineJoin() != joinstyle) 
  	{
  		int len = sprintf(format_buffer, " %d j", joinstyle);
    	xref->addPageContent(format_buffer, len);  /* op: j */
    	gs->setLineJoin(joinstyle);
  	}
	return 0;
}

int XWDVIDev::setLineWidth(XWDVIRef * xref, double width)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
  	if (gs->getLineWidth() != width) 
  	{
  		int len = 0;
    	format_buffer[len++] = ' ';
    	len += sprintLength(format_buffer + len, width);
    	format_buffer[len++] = ' ';
    	format_buffer[len++] = 'w';
    	xref->addPageContent(format_buffer, len);  /* op: w */
    	gs->setLineWidth(width);
  	}

  	return 0;
}

int XWDVIDev::setMiterLimit(XWDVIRef * xref, double mlimit)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return -1;
		
  	if (gs->getMiterLimit() != mlimit) 
  	{
  		int len = 0;
    	format_buffer[len++] = ' ';
    	len += sprintLength(format_buffer + len, mlimit);
    	format_buffer[len++] = ' ';
    	format_buffer[len++] = 'M';
    	xref->addPageContent(format_buffer, len);  /* op: M */
    	gs->setMiterLimit(mlimit);
  	}

  	return 0;
}

void XWDVIDev::setParam(int param_type, int value)
{
	switch (param_type) 
	{
  		case PDF_DEV_PARAM_AUTOROTATE:
    		setParamAutoRotate(value);
    		break;
    		
  		case PDF_DEV_PARAM_COLORMODE:
    		dev_param.colormode = value; /* 0 for B&W */
    		break;
    		
  		default:
  			break;
  	}
}

void XWDVIDev::setRect(PDFRect *rect,
                       long x_user, 
                       long y_user,
                       long width,  
                       long height, 
                       long depth)
{
	double dev_x = x_user * dev_unit.dvi2pts;
  	double dev_y = y_user * dev_unit.dvi2pts;
  	
  	PDFCoord   p0, p1, p2, p3;
  	if (text_state.dir_mode) 
  	{
    	p0.x = dev_x - dev_unit.dvi2pts * depth;
    	p0.y = dev_y - dev_unit.dvi2pts * width;
    	p1.x = dev_x + dev_unit.dvi2pts * height;
    	p1.y = p0.y;
    	p2.x = p1.x;
    	p2.y = dev_y;
    	p3.x = p0.x;
    	p3.y = p2.y;
  	} 
  	else 
  	{
    	p0.x = dev_x;
    	p0.y = dev_y - dev_unit.dvi2pts * depth;
    	p1.x = dev_x + dev_unit.dvi2pts * width;
    	p1.y = p0.y;
    	p2.x = p1.x;
    	p2.y = dev_y + dev_unit.dvi2pts * height;
    	p3.x = p0.x;
    	p3.y = p2.y;
  	}

  	transform(&p0, NULL); /* currentmatrix */
  	transform(&p1, NULL);
  	transform(&p2, NULL);
  	transform(&p3, NULL);

  	double min_x = qMin(p0.x , p1.x);
  	min_x = qMin(min_x, p2.x);
  	min_x = qMin(min_x, p3.x);

  	double max_x = qMax(p0.x , p1.x);
  	max_x = qMax(max_x, p2.x);
  	max_x = qMax(max_x, p3.x);

  	double min_y = qMin(p0.y , p1.y);
  	min_y = qMin(min_y, p2.y);
  	min_y = qMin(min_y, p3.y);

  	double max_y = qMax(p0.y , p1.y);
  	max_y = qMax(max_y, p2.y);
  	max_y = qMax(max_y, p3.y);

  	rect->llx = min_x;
  	rect->lly = min_y;
  	rect->urx = max_x;
  	rect->ury = max_y;
}

void XWDVIDev::setRule(XWDVIRef * xref,
	                   long xpos, 
	                   long ypos, 
	                   long width, 
	                   long height)
{
	if (num_dev_coords > 0) 
	{
    	xpos -= bpt2spt(dev_coords[num_dev_coords-1].x);
    	ypos -= bpt2spt(dev_coords[num_dev_coords-1].y);
  	}
  	
  	graphicsMode(xref);

	int len = 0;
	format_buffer[len++] = ' ';
  	format_buffer[len++] = 'q';
  	format_buffer[len++] = ' ';
  	
  	double width_in_bp = ((width < height) ? width : height) * dev_unit.dvi2pts;
  	if (width_in_bp < 0.0 || width_in_bp > PDF_LINE_THICKNESS_MAX) 
  	{
  		PDFRect rect;
    	rect.llx =  dev_unit.dvi2pts * xpos;
    	rect.lly =  dev_unit.dvi2pts * ypos;
    	rect.urx =  dev_unit.dvi2pts * width;
    	rect.ury =  dev_unit.dvi2pts * height;
    	len += sprintRect(format_buffer+len, &rect);
    	format_buffer[len++] = ' ';
    	format_buffer[len++] = 'r';
    	format_buffer[len++] = 'e';
    	format_buffer[len++] = ' ';
    	format_buffer[len++] = 'f';
  	}
  	else
  	{
  		if (width > height)
  			len += sprintLine(format_buffer+len, height, xpos, ypos + height/2, xpos + width, ypos + height/2);
  		else
  			len += sprintLine(format_buffer+len, width, xpos + width/2, ypos, xpos + width/2, ypos + height);
  	}
  	
  	format_buffer[len++] = ' ';
  	format_buffer[len++] = 'Q';
  	xref->addPageContent(format_buffer, len);
}

void XWDVIDev::setString(XWDVIRef * xref,
	                     long xpos, 
	                     long ypos,
                         const void *instr_ptr, 
                         int instr_len,
                         long width,
                         int   font_id, 
                         int ctype)
{
	if (font_id < 0 || font_id >= num_dev_fonts)
		return;
		
	if (font_id != text_state.font_id) 
    	setFont(xref, font_id);
    	
    DevFont * font = CURRENTFONT();
    if (!font)
    	return ;
    	
    long text_xorigin = text_state.ref_x;
  	long text_yorigin = text_state.ref_y;

  	uchar * str_ptr = (uchar *) instr_ptr;
  	int length  = instr_len;
  	if (font->format == PDF_FONTTYPE_COMPOSITE) 
  	{
    	if (font->handleMultiByteString(&str_ptr, &length, ctype) < 0) 
      		return;
    	
    	if (font->used_chars != NULL) 
    	{
      		for (int i = 0; i < length; i += 2)
        		add_to_used_chars2(font->used_chars, (ushort)(str_ptr[i] << 8)|str_ptr[i+1]);
    	}
  	} 
  	else 
  	{
    	if (font->used_chars != NULL) 
    	{
      		for (int i = 0; i < length; i++)
        		font->used_chars[str_ptr[i]] = 1;
    	}
  	}

  	if (num_dev_coords > 0) 
  	{
    	xpos -= bpt2spt(dev_coords[num_dev_coords-1].x);
    	ypos -= bpt2spt(dev_coords[num_dev_coords-1].y);
  	}
  	
  	long delh, delv;
  	if (text_state.dir_mode) 
  	{
    	delh = ypos - text_yorigin + text_state.offset;
    	delv = xpos - text_xorigin;
  	} 
  	else 
  	{
    	delh = text_xorigin + text_state.offset - xpos;
    	delv = ypos - text_yorigin;
  	}
  	
	long kern = 0;
	if (text_state.force_reset ||
      	labs(delv) > dev_unit.min_bp_val ||
      	labs(delh) > WORD_SPACE_MAX(font)) 
    {
    	textMode(xref);
  	} 
  	else 
    	kern = (long) (1000.0 / font->extend * delh / font->sptsize);
  	
  	int len = 0;
  	if (motion_state != STRING_MODE)
    	stringMode(xref, xpos, ypos, font->slant, font->extend, text_state.matrix.rotate);
  	else if (kern != 0)
  	{
  		text_state.offset -= (long) (kern * font->extend * (font->sptsize / 1000.0));
  		format_buffer[len++] = text_state.is_mb ? '>' : ')';
    	if (font->wmode)
      		len += intToAscii(-kern, format_buffer + len);
    	else 
      		len += intToAscii( kern, format_buffer + len);
      		
    	format_buffer[len++] = text_state.is_mb ? '<' : '(';
    	xref->addPageContent(format_buffer, len);  /* op: */
    	len = 0;
  	}
  	
  	if (text_state.is_mb) 
  	{
    	if (FORMAT_BUF_SIZE - len < 2 * length)
      		return ;
      		
      	for (int i = 0; i < length; i++) 
    	{
      		int first  = (str_ptr[i] >> 4) & 0x0f;
      		int second = str_ptr[i] & 0x0f;
      		format_buffer[len++] = ((first >= 10)  ? first  + 'W' : first  + '0');
      		format_buffer[len++] = ((second >= 10) ? second + 'W' : second + '0');
    	}
  	} 
  	else 
    	len += XWDVIRef::escapeStr(format_buffer + len, FORMAT_BUF_SIZE - len, str_ptr, length);
  	
  	xref->addPageContent(format_buffer, len);  /* op: */

  	text_state.offset += width;
}

int XWDVIDev::sprintCoord(char *buf, const PDFCoord *p)
{
	int len  = doubleToAscii(p->x, dev_unit.precision, buf);
  	buf[len++] = ' ';
  	len += doubleToAscii(p->y, dev_unit.precision, buf+len);
  	buf[len]   = '\0';
  	return  len;
}

int XWDVIDev::sprintLength (char *buf, double value)
{
	int len = doubleToAscii(value, dev_unit.precision, buf);
  	buf[len] = '\0';

  	return  len;
}

int XWDVIDev::sprintMatrix(char *buf, const PDFTMatrix *M)
{
	int  prec2 = qMin(dev_unit.precision + 2, DEV_PRECISION_MAX);
  	int  prec0 = qMax(dev_unit.precision, 2);
  	
  	int len  = doubleToAscii(M->a, prec2, buf);
  	buf[len++] = ' ';
  	len += doubleToAscii(M->b, prec2, buf+len);
  	buf[len++] = ' ';
  	len += doubleToAscii(M->c, prec2, buf+len);
  	buf[len++] = ' ';
  	len += doubleToAscii(M->d, prec2, buf+len);
  	buf[len++] = ' ';
  	len += doubleToAscii(M->e, prec0, buf+len);
  	buf[len++] = ' ';
  	len += doubleToAscii(M->f, prec0, buf+len);
  	buf[len]   = '\0';

  	return  len;
}

int XWDVIDev::sprintNumber(char *buf, double value)
{
	int len = doubleToAscii(value, DEV_PRECISION_MAX, buf);
  	buf[len] = '\0';

  	return  len;
}

int XWDVIDev::sprintRect(char *buf, const PDFRect *rect)
{
	int len  = doubleToAscii(rect->llx, dev_unit.precision, buf);
  	buf[len++] = ' ';
  	len += doubleToAscii(rect->lly, dev_unit.precision, buf+len);
  	buf[len++] = ' ';
  	len += doubleToAscii(rect->urx, dev_unit.precision, buf+len);
  	buf[len++] = ' ';
  	len += doubleToAscii(rect->ury, dev_unit.precision, buf+len);
  	buf[len]   = '\0';

  	return  len;
}

void XWDVIDev::transform(PDFCoord *p, const PDFTMatrix *M)
{
	XWDVIGState *gs  = gs_stack->getTop();
	if (!gs)
		return ;
		
	gs->transform(p, M);
}

int XWDVIDev::doubleToAscii(double value, int prec, char *buf)
{
	const long p[10] = { 1, 10, 100, 1000, 10000,
		       100000, 1000000, 10000000, 100000000, 1000000000 };
		       
	char *c = buf;
  	int n = 0;
  	if (value < 0) 
  	{
    	value = -value;
    	*c++ = '-';
    	n = 1;
  	}
  	
  	long i = (long) value;
  	long f = (long) ((value-i)*p[prec] + 0.5);
  	
  	if (f == p[prec]) 
  	{
    	f = 0;
    	i++;
  	}
  	
  	if (i) 
  	{
    	int m = intToAscii(i, c);
    	c += m;
    	n += m;
  	} 
  	else if (!f) 
  	{
    	*(c = buf) = '0';
    	n = 1;
  	}

  	if (f) 
  	{
    	int j = prec;
    	*c++ = '.';

    	while (j--) 
    	{
      		c[j] = (f % 10) + '0';
      		f /= 10;
    	}
    	c += prec-1;
    	n += 1+prec;

    	while (*c == '0') 
    	{
      		c--;
      		n--;
    	}
  	}

  	*(++c) = 0;

  	return n;
}

int XWDVIDev::flushPath(XWDVIRef * xref,
	                    XWDVIPath  *pa,
                        char       opchr,
                        int        rule,
                        int        ignore_rule)
{
	if (!pa || pa->length() <= 0)
    	return 0;
    	
    graphicsMode(xref);
    return pa->flush(xref, this, format_buffer, opchr, rule, ignore_rule);
}

int XWDVIDev::intToAscii(long value, char *buf)
{
	int   sign  = 0;
  	char *p = buf;
  	if (value < 0) 
  	{
    	*p++  = '-';
    	value = -value;
    	sign  = 1;
  	}
  	
  	int ndigits = 0;
  	do 
  	{
    	p[ndigits++] = (value % 10) + '0';
    	value /= 10;
  	} while (value != 0);
  	
  	for (int i = 0; i < ndigits / 2 ; i++) 
  	{
      	char tmp = p[i];
      	p[i] = p[ndigits-i-1];
      	p[ndigits-i-1] = tmp;
    }
    
    p[ndigits] = '\0';

  	return  (sign ? ndigits + 1 : ndigits);
}

int XWDVIDev::rectShape(XWDVIRef * xref,
                        const PDFRect    *r,
                        const PDFTMatrix *M,
                        char               opchr)
{
	int isclip = (opchr == 'W') ? 1 : 0;
	if (M && (isclip || !INVERTIBLE_MATRIX(M)))
    	return -1;
    	
    graphicsMode(xref);    	
    char  *buf = format_buffer;
    int len = 0;
    buf[len++] = ' ';
  	if (!isclip) 
  	{
    	buf[len++] = 'q';
    	if (M) 
    	{
      		buf[len++] = ' ';
      		len += sprintMatrix(buf + len, M);
      		buf[len++] = ' ';
      		buf[len++] = 'c'; buf[len++] = 'm';
    	}
    	buf[len++] = ' ';
  	}
  	buf[len++] = 'n';
  	
  	PDFCoord p;
  	p.x = r->llx; p.y = r->lly;
  	double wd  = r->urx - r->llx;
  	double ht  = r->ury - r->lly;
  	buf[len++] = ' ';
  	len += sprintCoord(buf + len, &p);
  	buf[len++] = ' ';
  	len += sprintLength(buf + len, wd);
  	buf[len++] = ' ';
  	len += sprintLength(buf + len, ht);
  	buf[len++] = ' ';
  	buf[len++] = 'r'; buf[len++] = 'e';

  	buf[len++] = ' ';
  	buf[len++] = opchr;

  	buf[len++] = ' ';
  	buf[len++] = isclip ? 'n' : 'Q';

  	xref->addPageContent(buf, len);

  	return 0;
}

void XWDVIDev::resetTextState(XWDVIRef * xref)
{
	xref->addPageContent(" BT", 3);
	if (text_state.force_reset ||
      	text_state.matrix.slant  != 0.0 ||
      	text_state.matrix.extend != 1.0 ||
      	ROTATE_TEXT(text_state.matrix.rotate)) 
    {
    	setTextMatrix(xref, 0, 0,
                      text_state.matrix.slant,
                      text_state.matrix.extend,
                      text_state.matrix.rotate);
  	}
  	
  	text_state.ref_x = 0;
  	text_state.ref_y = 0;
  	text_state.offset   = 0;
  	text_state.force_reset = 0;
}

int XWDVIDev::setFont(XWDVIRef * xref, int font_id)
{
	textMode(xref);
	
	DevFont * font = GET_FONT(font_id);
	text_state.is_mb = (font->format == PDF_FONTTYPE_COMPOSITE) ? 1 : 0;
	int vert_font  = font->wmode ? 1 : 0;
	int vert_dir = vert_font;
	if (dev_param.autorotate)
		vert_dir = text_state.dir_mode ? 1 : 0;
			
	int text_rotate = (vert_font << 1)|vert_dir;
	if (font->slant  != text_state.matrix.slant  ||
      	font->extend != text_state.matrix.extend ||
      	ANGLE_CHANGES(text_rotate, text_state.matrix.rotate)) 
    {
    	text_state.force_reset = 1;
  	}
  	
  	text_state.matrix.slant  = font->slant;
  	text_state.matrix.extend = font->extend;
  	text_state.matrix.rotate = text_rotate;

  	if (font->resource.isNull() || xref->isScanning())
  	{
  		core->getFontReference(font->font_id, &(font->resource));
    	font->used_chars = core->getFontUsedChars(font->font_id);
  	}
  	
  	if (!font->used_on_this_page && xref->isScanning())
  	{
  		XWObject obj;
  		font->resource.copy(&obj);  	 
    	xref->addPageResource("Font", font->short_name, &obj);
    	font->used_on_this_page = 1;
    	xref->addObjects(1);
  	}
  	
  	double font_scale = (double) font->sptsize * dev_unit.dvi2pts;
  	int len  = sprintf(format_buffer, " /%s", font->short_name);
  	format_buffer[len++] = ' ';
  	len += doubleToAscii(font_scale, qMin(dev_unit.precision+1, DEV_PRECISION_MAX), format_buffer+len);
  	format_buffer[len++] = ' ';
  	format_buffer[len++] = 'T';
  	format_buffer[len++] = 'f';
  	xref->addPageContent(format_buffer, len);
  	if (font->bold > 0.0 || font->bold != text_state.bold_param) 
  	{
    	if (font->bold <= 0.0)
    		len = sprintf(format_buffer, " 0 Tr");
    	else
      		len = sprintf(format_buffer, " 2 Tr %.2f w", font->bold); /* _FIXME_ */
    	xref->addPageContent(format_buffer, len);
  	}
  	text_state.bold_param = font->bold;

  	text_state.font_id    = font_id;

  	return  0;
}

void XWDVIDev::setParamAutoRotate(int auto_rotate)
{
	DevFont * font = CURRENTFONT();
  	int vert_font = (font && font->wmode) ? 1 : 0;
  	int vert_dir = vert_font;
  	if (auto_rotate) 
    	vert_dir = text_state.dir_mode ? 1 : 0;
    		
  	int text_rotate = (vert_font << 1)|vert_dir;

  	if (ANGLE_CHANGES(text_rotate, text_state.matrix.rotate)) 
    	text_state.force_reset = 1;
    	
  	text_state.matrix.rotate = text_rotate;
  	dev_param.autorotate     = auto_rotate;
}

void XWDVIDev::setTextMatrix(XWDVIRef * xref, 
	                         long xpos, 
	                         long ypos, 
	                         double slant, 
	                         double extend, 
	                         int rotate)
{
	PDFTMatrix tm;
	switch (rotate)
	{
		case TEXT_WMODE_VH:
			tm.a =  slant;   
			tm.b =  1.0;
    		tm.c = -extend;   
    		tm.d =  0.0;
			break;
			
		case TEXT_WMODE_HV:
    		tm.a =  0.0;    
    		tm.b = -extend;
    		tm.c =  1.0;    
    		tm.d = -slant;
    		break;
    		
    	case TEXT_WMODE_HH:
    		tm.a =  extend; 
    		tm.b =  0.0;
    		tm.c =  slant ; 
    		tm.d =  1.0;
    		break;
    		
  		case TEXT_WMODE_VV:
    		tm.a =  1.0; 
    		tm.b =  -slant;
    		tm.c =  0.0; 
    		tm.d =   extend;
    		break;
	}
	
	tm.e = xpos * dev_unit.dvi2pts;
  	tm.f = ypos * dev_unit.dvi2pts;

	int len = 0;
  	format_buffer[len++] = ' ';
  	len += sprintMatrix(format_buffer+len, &tm);
  	format_buffer[len++] = ' ';
  	format_buffer[len++] = 'T';
  	format_buffer[len++] = 'm';
  	xref->addPageContent(format_buffer, len);

  	text_state.ref_x = xpos;
  	text_state.ref_y = ypos;
  	text_state.matrix.slant  = slant;
  	text_state.matrix.extend = extend;
  	text_state.matrix.rotate = rotate;
}

int XWDVIDev::sprintBP(char *buf, long value, long *error)
{
	int prec = dev_unit.precision;
	double value_in_bp = spt2bpt(value);
	if (error) 
	{
    	double error_in_bp = value_in_bp - dround_at(value_in_bp, prec);
    	*error = bpt2spt(error_in_bp);
  	}

  	return  doubleToAscii(value_in_bp, prec, buf);
}

int XWDVIDev::sprintLine(char *buf, 
	                     long width,
                 	     long p0_x, 
                 	     long p0_y, 
                 	     long p1_x, 
                 	     long p1_y)
{
	double w = width * dev_unit.dvi2pts;
	int len = 0;
	len += doubleToAscii(w, qMin(dev_unit.precision+1, DEV_PRECISION_MAX), buf+len);
  	buf[len++] = ' ';
  	buf[len++] = 'w';
  	buf[len++] = ' ';
  	len += sprintBP(buf+len, p0_x, NULL);
  	buf[len++] = ' ';
  	len += sprintBP(buf+len, p0_y, NULL);
  	buf[len++] = ' ';
  	buf[len++] = 'm';
  	buf[len++] = ' ';
  	len += sprintBP(buf+len, p1_x, NULL);
  	buf[len++] = ' ';
  	len += sprintBP(buf+len, p1_y, NULL);
  	buf[len++] = ' ';
  	buf[len++] = 'l';
  	buf[len++] = ' ';
  	buf[len++] = 'S';

  	return len;
}

void  XWDVIDev::startString(XWDVIRef * xref, 
	                        long xpos, 
	                        long ypos, 
	                        double slant, 
	                        double extend, 
	                        int rotate)
{
	long delx = xpos - text_state.ref_x;
  	long dely = ypos - text_state.ref_y;
  	
  	long desired_delx, desired_dely;
  	long error_delx, error_dely;
  	int len = 0;
  	switch (rotate)
  	{
  		case TEXT_WMODE_VH:
  			desired_delx = dely;
    		desired_dely = (long) (-(delx - dely*slant)/extend);
    		format_buffer[len++] = ' ';
    		len += sprintBP(format_buffer+len, desired_delx, &error_dely);
    		format_buffer[len++] = ' ';
    		len += sprintBP(format_buffer+len, desired_dely, &error_delx);
    		error_delx = -error_delx;
    		break;
    		
    	case TEXT_WMODE_HV:
    		desired_delx = (long)(-(dely + delx*slant)/extend);
    		desired_dely = delx;
    		format_buffer[len++] = ' ';
    		len += sprintBP(format_buffer+len, desired_delx, &error_dely);
    		format_buffer[len++] = ' ';
    		len += sprintBP(format_buffer+len, desired_dely, &error_delx);
    		error_dely = -error_dely;
    		break;
    		
    	case TEXT_WMODE_HH:
    		desired_delx = (long)((delx - dely*slant)/extend);
    		desired_dely = dely;

    		format_buffer[len++] = ' ';
    		len += sprintBP(format_buffer+len, desired_delx, &error_delx);
    		format_buffer[len++] = ' ';
    		len += sprintBP(format_buffer+len, desired_dely, &error_dely);
    		break;
    		
    	case TEXT_WMODE_VV:
    		desired_delx = delx;
    		desired_dely = (long)((dely + delx*slant)/extend);
    		format_buffer[len++] = ' ';
    		len += sprintBP(format_buffer+len, desired_delx, &error_delx);
    		format_buffer[len++] = ' ';
    		len += sprintBP(format_buffer+len, desired_dely, &error_dely);
    		break;
  	}
  	
  	xref->addPageContent(format_buffer, len);
  	xref->addPageContent(text_state.is_mb ? " Td[<" : " Td[(", 5);
  	text_state.ref_x = xpos - error_delx;
  	text_state.ref_y = ypos - error_dely;

  	text_state.offset   = 0;
}

void XWDVIDev::stringMode(XWDVIRef * xref, 
	                      long xpos, 
	                      long ypos, 
	                      double slant, 
	                      double extend, 
	                      int rotate)
{
	switch (motion_state) 
	{
  		case STRING_MODE:
    		break;
    		
  		case GRAPHICS_MODE:
    		resetTextState(xref);
  		case TEXT_MODE:
    		if (text_state.force_reset) 
    		{
      			setTextMatrix(xref, xpos, ypos, slant, extend, rotate);
      			xref->addPageContent(text_state.is_mb ? "[<" : "[(", 2);  /* op: */
      			text_state.force_reset = 0;
    		} 
    		else
      			startString(xref, xpos, ypos, slant, extend, rotate);
    		break;
  	}
  	motion_state = STRING_MODE;
}

void XWDVIDev::textMode(XWDVIRef * xref)
{
	switch (motion_state) 
	{
  		case TEXT_MODE:
    		break;
    		
  		case STRING_MODE:
  			xref->addPageContent(text_state.is_mb ? ">]TJ" : ")]TJ", 4);  /* op: TJ */
    		break;
    		
  		case GRAPHICS_MODE:
    		resetTextState(xref);
    		break;
  	}
  	motion_state      = TEXT_MODE;
  	text_state.offset = 0;
}

