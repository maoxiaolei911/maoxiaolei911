/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSDeviceCPathAccum.h"
#include "XWPSColorSpace.h"
#include "XWPSState.h"
#include "XWPSPath.h"

int ps_curve_monotonic_points(long v0, long v1, long v2, long v3, double pst[2])
{
	long v01, v12, a, b, c, b2, a3;
  long dv_end, b2abs, a3abs;

  curve_points_to_coefficients(v0, v1, v2, v3, a, b, c, v01, v12);
  b2 = b << 1;
  a3 = (a << 1) + a;
  if (a == 0) 
  {
		if ((b ^ c) < 0 && qAbs(c) < qAbs(b2) && c != 0) 
		{
	    *pst = (double)(-c) / b2;
	    return 1;
		} 
		else
	    return 0;
  }
  
  if (c == 0) 
  {
		if ((a ^ b) < 0 && qAbs(b2) < qAbs(a3) && b != 0) 
		{
	    *pst = (double)(-b2) / a3;
	    return 1;
		} 
		else
	    return 0;
  }
  else if ((dv_end = a3 + b2 + c) == 0) 
  {
		if ((a ^ b) < 0 && (b2abs = qAbs(b2)) > (a3abs = qAbs(a3)) &&  b2abs < a3abs << 1) 
		{
	    *pst = (double)(-b2 - a3) / a3;
	    return 1;
		} 
		else
	    return 0;
  }
  else if ((dv_end = a3 + b2 + c) == 0) 
  {
		if ((a ^ b) < 0 && (b2abs = qAbs(b2)) > (a3abs = qAbs(a3)) && b2abs < a3abs << 1) 
		{
	    *pst = (double)(-b2 - a3) / a3;
	    return 1;
		} 
		else
	    return 0;
  }
  else if ((dv_end ^ c) < 0);
  else if ((a ^ b) >= 0)
		return 0;
	else if (qAbs(b) >= qAbs(a3))
		return 0;
		
	{
		double nbf = (double)(-b);
		double a3f = (double)a3;
		double radicand = nbf * nbf - a3f * c;

		if (radicand < 0) 
	    return 0;
	    
	  {
	  	double root = sqrt(radicand);
	    int nzeros = 0;
	    double z = (nbf - root) / a3f;
	    if (z > 0 && z < 1)
				*pst = z, nzeros = 1;
	    if (root != 0)
	    {
	    	z = (nbf + root) / a3f;
				if (z > 0 && z < 1) 
				{
		    	if (nzeros && a3f < 0)
						pst[1] = *pst, *pst = z;
		    	else
						pst[nzeros] = z;
		    	nzeros++;
				}
	    }
	    return nzeros;
	  }
	}
}

XWPSFillParams::XWPSFillParams()
{
	rule = 0;
	flatness = 0.0;
	fill_zero_width = false;
}

XWPSArcCurveParams::XWPSArcCurveParams()
{
	ppath = 0;
	pis = 0;
	radius = 0.0;
	angle = 0;
	fast_quadrant = 0;
	scaled_radius = 0;
	quadrant_delta = 0;
}

XWPSArcCurveParams::~XWPSArcCurveParams()
{
}

int XWPSArcCurveParams::arcAdd(bool is_quadrant)
{
	double x0 = p0.x, y0 = p0.y;
  double xt = pt.x, yt = pt.y;
  float fraction;
  XWPSFixedPoint p0A, p2A, p3A, ptA;
  int code;
  
  if ((action != arc_nothing &&
	 		(code = p0A.transform2fixed(&pis->ctm, x0, y0)) < 0) ||
			(code = ptA.transform2fixed(&pis->ctm, xt, yt)) < 0 ||
			(code = p3A.transform2fixed(&pis->ctm, p3.x, p3.y)) < 0 ||
			(code = (action == arc_nothing ?
	  	(p0A.x = ppath->position.x, p0A.y = ppath->position.y, 0) : 
	  		action == arc_lineto && ppath->positionValid() ?
	  		ppath->addLineNotes(p0A.x, p0A.y, sn_none) : ppath->addPoint(p0A.x, p0A.y))) < 0)
		return code;
		
	if (is_quadrant)
	{
		fraction = PS_QUARTER_ARC_FRACTION;
		if (fast_quadrant > 0) 
		{
	    long delta = quadrant_delta;

	    if (ptA.x != p0A.x)
				p0A.x = (ptA.x > p0A.x ? p0A.x + delta : p0A.x - delta);
	    if (ptA.y != p0A.y)
				p0A.y = (ptA.y > p0A.y ? p0A.y + delta : p0A.y - delta);
	    p2A.x = (ptA.x == p3A.x ? p3A.x : ptA.x > p3A.x ? p3A.x + delta : p3A.x - delta);
	    p2A.y = (ptA.y == p3A.y ? p3A.y : ptA.y > p3A.y ? p3A.y + delta : p3A.y - delta);
	    goto add;
		}		
	}
	else
	{
		double r = radius;
		float dx = xt - x0, dy = yt - y0;
		double dist = dx * dx + dy * dy;
		double r2 = r * r;

		if (dist >= r2 * 1.0e8)
	   	fraction = 0.0;
		else
	   	fraction = (4.0 / 3.0) / (1 + sqrt(1 + dist / r2));
	}
	
	p0A.x += (long)((ptA.x - p0A.x) * fraction);
  p0A.y += (long)((ptA.y - p0A.y) * fraction);
  p2A.x = p3A.x + (long)((ptA.x - p3A.x) * fraction);
  p2A.y = p3A.y + (long)((ptA.y - p3A.y) * fraction);
add:
  return ppath->addCurveNotes(p0A.x, p0A.y, p2A.x, p2A.y, p3A.x, p3A.y, (PSSegmentNotes)(notes | sn_from_arc));
}

int XWPSArcCurveParams::nextArcCurve(long anext)
{
	double x0 = p0.x = p3.x;
  double y0 = p0.y = p3.y;
  double trad = radius *	tan(fixed2float(anext - angle) * (degrees_to_radians / 2));

  pt.x = x0 - trad * sincos._sin;
  pt.y = y0 + trad * sincos._cos;
  sincos.sincosDegrees(fixed2float(anext));
  p3.x = center.x + radius * sincos._cos;
  p3.y = center.y + radius * sincos._sin;
  angle = anext;
  return arcAdd(false);
}

int XWPSArcCurveParams::nextArcQuadrant(long anext)
{
	double x0 = p0.x = p3.x;
  double y0 = p0.y = p3.y;

  if (!fast_quadrant)
  {
  	double scale;

		if (is_fzero2(pis->ctm.xy, pis->ctm.yx) ? (scale = fabs(pis->ctm.xx)) == fabs(pis->ctm.yy) :
	    is_fzero2(pis->ctm.xx, pis->ctm.yy) ?  (scale = fabs(pis->ctm.xy)) == fabs(pis->ctm.yx) : 0) 
	  {
	    double scaled_radiusA = radius * scale;

	    scaled_radius = float2fixed(scaled_radiusA);
	    quadrant_delta =	float2fixed(scaled_radiusA * PS_QUARTER_ARC_FRACTION);
	    fast_quadrant = 1;
		} 
		else 
	    fast_quadrant = -1;
  }
  
  switch ((fixed2int(anext) >> 1) & 3) 
  {
    case 0:
			sincos._sin = 0, sincos._cos = 1;
			p3.x = x0 = center.x + radius;
			p3.y = center.y;
			break;
			
    case 1:
			sincos._sin = 1, sincos._cos = 0;
			p3.x = center.x;
			p3.y = y0 = center.y + radius;
			break;
			
    case 2:
			sincos._sin = 0, sincos._cos = -1;
			p3.x = x0 = center.x - radius;
			p3.y = center.y;
			break;
			
    case 3:
			sincos._sin = -1, sincos._cos = 0;
			p3.x = center.x;
			p3.y = y0 = center.y - radius;
			break;
			
  }
  pt.x = x0, pt.y = y0;
  angle = anext;
  return arcAdd(true);
}

XWPSSegment::XWPSSegment()
	:prev(0),
	 next(0),
	 type(0),
	 notes(sn_none)
{
}

XWPSSegment::XWPSSegment(ushort t)
	:prev(0),
	 next(0),
	 type(t),
	 notes(sn_none)
{
}

XWPSSegment::XWPSSegment(ushort t, ushort n)
	:prev(0),
	 next(0),
	 type(t),
	 notes(n)
{
}

XWPSSegment::~XWPSSegment()
{
}

void XWPSSegment::adjustPointToTangent(const XWPSSegment * nextA,	const XWPSFixedPoint * p1A)
{
	const long x0 = pt.x, y0 = pt.y;
  const long fC = p1A->x - x0, fD = p1A->y - y0;
  
  if (fC == 0) 
  {
		const long DT = arith_rshift(nextA->pt.y - y0, 2);

		if (fD == 0)
	    return;
	    
		if ((DT ^ fD) > 0)
	    pt.y = DT + y0;
  } 
  else if (fD == 0) 
  {
		const long CT = arith_rshift(nextA->pt.x - x0, 2);
		if ((CT ^ fC) > 0)
	    pt.x = CT + x0;
  } 
  else 
  {
  	
		const double C = fC, D = fD;
		const double T = (C * (nextA->pt.x - x0) + D * (nextA->pt.y - y0)) / (C * C + D * D);
		if (T > 0) 
		{
	    pt.x = arith_rshift((long) (C * T), 2) + x0;
	    pt.y = arith_rshift((long) (D * T), 2) + y0;
		}
  }
}

void XWPSSegment::applyFinalHint(XWPSSegment * pseg_last, const XWPSFixedPoint * pdiff)
{
	XWPSSegment *pseg;

  for (pseg = pseg_last;; pseg = pseg->prev) 
  {
		XWPSSegment *prevA = pseg->prev;

		switch (pseg->type) 
		{
	    case s_curve:
				((XWPSCurveSegment *) pseg)->adjustCurveEnd(pdiff);
				return;
				
	    case s_line:
	    case s_line_close:
				if (!lineIsNull(prevA->pt, pseg->pt))
		    	return;
				addHintDiff(&prevA->pt, *pdiff);
				break;
				
	    default:
				return;
		}
  }
}

long XWPSSegment::scaleDelta(long diff, long dv, long lv, bool nearer)
{
	if (dv == 0)
		return 0;
		
  if (lv == 0)
		return (nearer ? diff : fixed_0);
    
  if (lv < 0)
		lv = -lv, dv = -dv;
    
  if (dv < 0)
		dv = -dv, diff = -diff;
		
  if (dv >= lv)
		return (nearer ? diff : fixed_0);
  else
		return fixed_mult_quo(diff, dv, lv);
}

XWPSLineSegment::XWPSLineSegment()
	:XWPSSegment(s_line)
{
}

XWPSLineSegment::XWPSLineSegment(ushort n)
	:XWPSSegment(s_line, n)
{
}

XWPSLineCloseSegment::XWPSLineCloseSegment()
	:XWPSSegment(s_line_close),
	 sub(0)
{
}

XWPSLineCloseSegment::XWPSLineCloseSegment(ushort n)
	:XWPSSegment(s_line_close,n),
	 sub(0)
{
}

XWPSLineCloseSegment::~XWPSLineCloseSegment()
{
}

#define k_sample_max qMin((int)((sizeof(int) * 8 - 1) / 3), (int)10)

XWPSCurveSegment::XWPSCurveSegment()
	:XWPSSegment(s_curve)
{
}

XWPSCurveSegment::XWPSCurveSegment(ushort n)
	:XWPSSegment(s_curve, n)
{
}

void XWPSCurveSegment::adjustCurveEnd(const XWPSFixedPoint * pdiff)
{
	long dx = pdiff->x, dy = pdiff->y;
  long start_x = prev->pt.x, start_y = prev->pt.y;
  long lx = pt.x - dx - start_x, ly = pt.y - dy - start_y;
  XWPSFixedPoint delta;

  delta.x = scaleDelta(dx, p1.x - start_x, lx, false);
  delta.y = scaleDelta(dy, p1.y - start_y, ly, false);
  addHintDiff(&p1, delta);
  delta.x = scaleDelta(dx, p2.x - start_x, lx, true);
  delta.y = scaleDelta(dy, p2.y - start_y, ly, true);
  addHintDiff(&p2, delta);
}

void XWPSCurveSegment::adjustCurveStart(const XWPSFixedPoint * pdiff)
{
	long dx = pdiff->x, dy = pdiff->y;
  long end_x = pt.x, end_y = pt.y;
  long lx = end_x - (prev->pt.x - dx), ly = end_y - (prev->pt.y - dy);
  XWPSFixedPoint delta;

  delta.x = scaleDelta(dx, end_x - p1.x, lx, true);
  delta.y = scaleDelta(dy, end_y - p1.y, ly, true);
  addHintDiff(&p1, delta);
  delta.x = scaleDelta(dx, end_x - p2.x, lx, false);
  delta.y = scaleDelta(dy, end_y - p2.y, ly, false);
  addHintDiff(&p2, delta);
}

int XWPSCurveSegment::log2Samples(long x0, long y0, long fixed_flat)
{
	long	x03 = pt.x - x0,	y03 = pt.y - y0;
  int k;
  if (x03 < 0)
		x03 = -x03;
  if (y03 < 0)
		y03 = -y03;
  if ((x03 | y03) < int2fixed(16))
		fixed_flat >>= 1;
  if (fixed_flat == 0) 
  {
		long m = qMax(x03, y03);
		for (k = 1; m > fixed_1;)
	    k++, m >>= 1;
  }
  else
  {
  	const long  x12 = p1.x - p2.x, y12 = p1.y - p2.y, dx0 = x0 - p1.x - x12, dy0 = y0 - p1.y - y12,
	      				dx1 = x12 - p2.x + pt.x, dy1 = y12 - p2.y + pt.y, adx0 = qAbs(dx0),
	      				ady0 = qAbs(dy0), adx1 = qAbs(dx1), ady1 = qAbs(dy1);

		long d = qMax(adx0, adx1) + qMax(ady0, ady1);
		uint qtmp = d - (d >> 2) /* 3/4 * D */ +fixed_flat - 1;
		uint q = qtmp / fixed_flat;
		for (k = 0; q > 1;)
	    k++, q = (q + 3) >> 2;
  }
  return k;
}

int XWPSCurveSegment::monotonicPoints(long v0, 
	                           long v1, 
	                           long v2, 
	                           long v3, 
	                           double * pst)
{
	long v01, v12, a, b, c, b2, a3;
  long dv_end, b2abs, a3abs;

  curve_points_to_coefficients(v0, v1, v2, v3, a, b, c, v01, v12);
  b2 = b << 1;
  a3 = (a << 1) + a;
  if (a == 0) 
  {
		if ((b ^ c) < 0 && qAbs(c) < qAbs(b2) && c != 0) 
		{
	    *pst = (double)(-c) / b2;
	    return 1;
		} 
		else
	    return 0;
  }
  
  if (c == 0) 
  {
		if ((a ^ b) < 0 && qAbs(b2) < qAbs(a3) && b != 0) 
		{
	    *pst = (double)(-b2) / a3;
	    return 1;
		} 
		else
	    return 0;
  }
  else if ((dv_end = a3 + b2 + c) == 0) 
  {
		if ((a ^ b) < 0 && (b2abs = qAbs(b2)) > (a3abs = qAbs(a3)) && b2abs < a3abs << 1) 
		{
	    *pst = (double)(-b2 - a3) / a3;
	    return 1;
		} 
		else
	    return 0;
  }
  else if ((dv_end ^ c) < 0);
  else if ((a ^ b) >= 0)
		return 0;
	else if (qAbs(b) >= qAbs(a3))
		return 0;
		
	double nbf = (double)(-b);
	double a3f = (double)a3;
	double radicand = nbf * nbf - a3f * c;
	if (radicand < 0)
		return 0;
		
	double root = sqrt(radicand);
	int nzeros = 0;
	double z = (nbf - root) / a3f;
	if (z > 0 && z < 1)
		*pst = z, nzeros = 1;
	if (root != 0) 
	{
		z = (nbf + root) / a3f;
		if (z > 0 && z < 1) 
		{
		    if (nzeros && a3f < 0)
					pst[1] = *pst, *pst = z;
		    else
					pst[nzeros] = z;
		    nzeros++;
		}
	}
	return nzeros;
}

void XWPSCurveSegment::split(long x0, 
	                           long y0, 
	                           double t,
	                           XWPSCurveSegment * pc1, 
	                           XWPSCurveSegment * pc2)
{
	double t2 = t * t, t3 = t2 * t;
  double omt = 1 - t, omt2 = omt * omt, omt3 = omt2 * omt;
  long v01, v12, a, b, c, na, nb, nc;
  
#define compute_seg(v0, v)\
	curve_points_to_coefficients(v0, p1.v, p2.v, pt.v,\
				     a, b, c, v01, v12);\
	na = (long)(a * t3), nb = (long)(b * t2), nc = (long)(c * t);\
	curve_coefficients_to_points(na, nb, nc, v0,\
				     pc1->p1.v, pc1->p2.v, pc1->pt.v);\
	na = (long)(a * omt3);\
	nb = (long)((a * t * 3 + b) * omt2);\
	nc = (long)((a * t2 * 3 + b * 2 * t + c) * omt);\
	curve_coefficients_to_points(na, nb, nc, pc1->pt.v,\
				     pc2->p1.v, pc2->p2.v, pc2->pt.v)
  compute_seg(x0, x);
  compute_seg(y0, y);
#undef compute_seg
}

void XWPSCurveSegment::splitCurveMidpoint(long x0, 
	                        long y0, 
	                        XWPSCurveSegment * pc1, 
	                        XWPSCurveSegment * pc2)
{
#define midpoint(a,b)\
  (arith_rshift_1(a) + arith_rshift_1(b) + ((a) & (b) & 1) + 1)
  
  long x12 = midpoint(p1.x, p2.x);
  long y12 = midpoint(p1.y, p2.y);
  
  pc1->p1.x = midpoint(x0, p1.x);
  pc1->p1.y = midpoint(y0, p1.y);
  pc2->p2.x = midpoint(p2.x, pt.x);
  pc2->p2.y = midpoint(p2.y, pt.y);
  pc1->p2.x = midpoint(pc1->p1.x, x12);
  pc1->p2.y = midpoint(pc1->p1.y, y12);
  pc2->p1.x = midpoint(x12, pc2->p2.x);
  pc2->p1.y = midpoint(y12, pc2->p2.y);
  if (pc2 != this)
		pc2->pt.x = pt.x,
	pc2->pt.y = pt.y;
  pc1->pt.x = midpoint(pc1->p2.x, pc2->p1.x);
  pc1->pt.y = midpoint(pc1->p2.y, pc2->p1.y);
#undef midpoint
}

XWPSSubpath::XWPSSubpath()
	:XWPSSegment(s_start),
	 last(0),
	 curve_count(0),
	 is_closed(false)
{
}

XWPSSubpath::XWPSSubpath(ushort n)
	:XWPSSegment(s_start, n),
	 last(0),
	 curve_count(0),
	 is_closed(false)
{
}

PSPathRectangularType  XWPSSubpath::isRectangular(XWPSFixedRect * pbox, 
	                                                const XWPSSubpath ** ppnext)
{
	const XWPSSegment *pseg1, *pseg2, *pseg3, *pseg4;
  PSPathRectangularType typeA;

  if (curve_count == 0 &&
			(pseg1 = next) != 0 &&
			(pseg2 = pseg1->next) != 0 &&
			(pseg3 = pseg2->next) != 0) 
	{
		if ((pseg4 = pseg3->next) == 0 || pseg4->type == s_start)
	    typeA = prt_open;
		else if (pseg4->type != s_line)
	    typeA = prt_closed;
		else if (pseg4->pt.x != pt.x || pseg4->pt.y != pt.y)
	    return prt_none;
		else if (pseg4->next == 0 || pseg4->next->type == s_start)
	    typeA = prt_fake_closed;
		else if (pseg4->next->type != s_line)	/* must be s_line_close */
	    typeA = prt_closed;	/* Mo, L, L, L, Lo, C */
		else
	    return prt_none;
		{
	    long x0 = pt.x, y0 = pt.y;
	    long x2 = pseg2->pt.x, y2 = pseg2->pt.y;

	    if ((x0 == pseg1->pt.x && pseg1->pt.y == y2 &&
		 			x2 == pseg3->pt.x && pseg3->pt.y == y0) ||
					(x0 == pseg3->pt.x && pseg3->pt.y == y2 &&
		 			x2 == pseg1->pt.x && pseg1->pt.y == y0)	) 
		 	{
				if (x0 < x2)
		    	pbox->p.x = x0, pbox->q.x = x2;
				else
		    	pbox->p.x = x2, pbox->q.x = x0;
				if (y0 < y2)
		    	pbox->p.y = y0, pbox->q.y = y2;
				else
		    	pbox->p.y = y2, pbox->q.y = y0;
				while (pseg4 != 0 && pseg4->type != s_start)
		    	pseg4 = pseg4->next;
				*ppnext = (const XWPSSubpath *)pseg4;
				return typeA;
	    }
		}
  }
  return prt_none;
}

XWPSSubpath::~XWPSSubpath()
{
}

XWPSCurveCursor::XWPSCurveCursor()
{
	k = 0;
	pc = 0;
	a = b = c = 0;
	da = db = dc = 0;
	double_set = false;
	
	cache.ky0 = 0;
	cache.ky3 = 0;
	cache.xl = 0;
	cache.xd = 0;
}

#define SUBDIVIDE_X USE_FPU_FIXED

long XWPSCurveCursor::curveXAtY(long y)
{
	long xl, xd;
  long yd, yrel;
  
  if (y >= cache.ky0 && y <= cache.ky3) 
  {
		yd = cache.ky3 - cache.ky0;
		yrel = y - cache.ky0;
		xl = cache.xl;
		xd = cache.xd;
		goto done;
  }
  
  long cy0 = p0.y, cy1, cy2, cy3 = pc->pt.y;
	long cx0;

#if SUBDIVIDE_X
	long cx1, cx2, cx3;

#else
	int t = 0;

#endif
	int i;
	
	if (cy0 > cy3)
	    cx0 = pc->pt.x,
#if SUBDIVIDE_X
		cx1 = pc->p2.x, cx2 = pc->p1.x, cx3 = p0.x,
#endif
		cy0 = pc->pt.y, cy1 = pc->p2.y, cy2 = pc->p1.y, cy3 = p0.y;
	else
	    cx0 = p0.x,
#if SUBDIVIDE_X
		cx1 = pc->p1.x, cx2 = pc->p2.x, cx3 = pc->pt.x,
#endif
		cy1 = pc->p1.y, cy2 = pc->p2.y;
		
#define midpoint_fast(a,b)\
  arith_rshift_1((a) + (b) + 1)
	
	for (i = k ; i > 0; --i)
	{
		long ym = midpoint_fast(cy1, cy2);
	  long yn = ym + arith_rshift(cy0 - cy1 - cy2 + cy3 + 4, 3);

#if SUBDIVIDE_X
	  long xm = midpoint_fast(cx1, cx2);
	  long xn = xm + arith_rshift(cx0 - cx1 - cx2 + cx3 + 4, 3);

#else
	  t <<= 1;
#endif

	  if (y < yn)
#if SUBDIVIDE_X
		cx1 = midpoint_fast(cx0, cx1),
		    cx2 = midpoint_fast(cx1, xm),
		    cx3 = xn,
#endif
		    cy1 = midpoint_fast(cy0, cy1),
		    cy2 = midpoint_fast(cy1, ym),
		    cy3 = yn;
	    else
#if SUBDIVIDE_X
		cx2 = midpoint_fast(cx2, cx3),
		    cx1 = midpoint_fast(xm, cx2),
		    cx0 = xn,
#else
		t++,
#endif
		    cy2 = midpoint_fast(cy2, cy3),
		    cy1 = midpoint_fast(ym, cy2),
		    cy0 = yn;
	}
	
#if SUBDIVIDE_X
	xl = cx0;
	xd = cx3 - cx0;
#else
  {
#define compute_fixed(a, b, c)\
  arith_rshift(arith_rshift(arith_rshift(a * t3, k) + b * t2, k)\
	       + c * t + ((1 << k) >> 1), k)
#define compute_diff_fixed(a, b, c)\
  arith_rshift(arith_rshift(arith_rshift(a * t3d, k) + b * t2d, k)\
	       + c, k)
	       
#define np2(n) (1.0 / (1L << (n)))
	    static const double k_denom[11] =
	    {np2(0), np2(1), np2(2), np2(3), np2(4),
	     np2(5), np2(6), np2(7), np2(8), np2(9), np2(10)
	    };
	    static const double k2_denom[11] =
	    {np2(0), np2(2), np2(4), np2(6), np2(8),
	     np2(10), np2(12), np2(14), np2(16), np2(18), np2(20)
	    };
	    static const double k3_denom[11] =
	    {np2(0), np2(3), np2(6), np2(9), np2(12),
	     np2(15), np2(18), np2(21), np2(24), np2(27), np2(30)
	    };
	    double den1, den2;

#undef np2

#define setup_floating(da, db, dc, a, b, c)\
  (k >= ((sizeof(k_denom) / sizeof(k_denom[0]))) ?\
   (den1 = ldexp(1.0, -k),\
    den2 = den1 * den1,\
    da = (den2 * den1) * a,\
    db = den2 * b,\
    dc = den1 * c) :\
   (da = k3_denom[k] * a,\
    db = k2_denom[k] * b,\
    dc = k_denom[k] * c))
#define compute_floating(da, db, dc)\
  ((long)(da * t3 + db * t2 + dc * t + 0.5))
#define compute_diff_floating(da, db, dc)\
  ((long)(da * t3d + db * t2d + dc))
  
  	if (t <= fixed_limit)
  	{
  		int t2 = t * t, t3 = t2 * t;
			int t3d = (t2 + t) * 3 + 1, t2d = t + t + 1;

			xl = compute_fixed(a, b, c) + cx0;
			xd = compute_diff_fixed(a, b, c);
  	}
  	else
  	{
#define fa da
#define fb db
#define fc dc
		if (!double_set) {
		    setup_floating(fa, fb, fc, a, b, c);
		    double_set = true;
		}
		if (t < 1L << ((sizeof(long) * 8 - 1) / 3)) 
		{
		  long t2 = (long)t * t, t3 = t2 * t;
		  long t3d = (t2 + t) * 3 + 1, t2d = t + t + 1;

		  xl = compute_floating(fa, fb, fc) + cx0;
		  xd = compute_diff_floating(fa, fb, fc);
		} 
		else 
		{
		  double t2 = (double)t * t, t3 = t2 * t;
		  double t3d = (t2 + t) * 3 + 1, t2d = t + t + 1;

		  xl = compute_floating(fa, fb, fc) + cx0;
		  xd = compute_diff_floating(fa, fb, fc);
		}
#undef fa
#undef fb
#undef fc
  	}
  }
#endif /* (!)SUBDIVIDE_X */

  cache.ky0 = cy0;
	cache.ky3 = cy3;
	cache.xl = pc->p1.x;
	cache.xd = xd;
	yd = cy3 - cy0;
	yrel = y - cy0;
	
done:
	if (yrel == 0)
		return pc->p1.x;
		
#define HALF_FIXED_BITS ((long)1 << (sizeof(long) * 4))
  if (yrel < HALF_FIXED_BITS) 
  {
		if (xd >= 0) 
		{
	    if (xd < HALF_FIXED_BITS)
				return (ulong)xd * (ulong)yrel / (ulong)yd + pc->p1.x;
		} 
		else 
		{
	    if (xd > -HALF_FIXED_BITS) 
	    {
				ulong num = (ulong)(-xd) * (ulong)yrel;
				ulong quo = num / (ulong)yd;

				if (quo * (ulong)yd != num)
		    	quo += fixed_epsilon;
				return pc->p1.x - (long)quo;
	    }
		}
  }
#undef HALF_FIXED_BITS
  return fixed_mult_quo(xd, yrel, yd) + pc->p1.x;
}

void XWPSCurveCursor::init(long x0, long y0, const XWPSCurveSegment * pcA, int kA)
{
	long v01, v12;
  int k2 = kA + kA, k3 = k2 + kA;

#define bits_fit(v, n)\
  (qAbs(v) <= max_fixed >> (n))
  
#define coeffs_fit(a, b, c)\
  (k3 <= sizeof(long) * 8 - 3 &&\
   bits_fit(a, k3 + 2) && bits_fit(b, k2 + 2) && bits_fit(c, kA + 1))

  k = kA;
  p0.x = x0, p0.y = y0;
  pc = (XWPSCurveSegment*)pcA;
  
  {
		long w0, w1, w2, w3;

		if (y0 < pc->pt.y)
	    w0 = x0, w1 = pc->p1.x, w2 = pc->p2.x, w3 = pc->pt.x;
		else
	    w0 = pc->pt.x, w1 = pc->p2.x, w2 = pc->p1.x, w3 = x0;
		curve_points_to_coefficients(w0, w1, w2, w3, a, b, c, v01, v12);
  }
  
  double_set = false;
  fixed_limit =(coeffs_fit(a, b, c) ? (1 << kA) - 1 : -1);
  cache.ky0 = cache.ky3 = y0;
  cache.xl = x0;
  cache.xd = 0;
}

#define al_dx(alp) ((alp)->diff.x)
#define al_dy(alp) ((alp)->diff.y)

#define set_al_points(alp, startp, endp)\
    (alp)->diff.y = (endp).y - (startp).y;\
    (alp)->diff.x = (endp).x - (startp).x;\
    SET_NUM_ADJUST(alp);\
    (alp)->y_fast_max = MAX_MINUS_NUM_ADJUST(alp) /\
      (((alp)->diff.x >= 0 ? (alp)->diff.x : -(alp)->diff.x) | 1) +\
      (startp).y;\
    (alp)->start = startp, (alp)->end = endp;
    

XWPSActiveLine::XWPSActiveLine()
{
	y_fast_max = 0;
	num_adjust = 0;
	x_current = 0;
	x_next = 0;
	pseg = 0;
	direction = 0;
	curve_k = 0;
	prev = 0;
	next = 0;
	alloc_next = 0;
}

bool XWPSActiveLine::end_x_line()
{
	XWPSSegment *psegA = pseg;
	XWPSSegment *nextA = (direction == dir_up ?	 (psegA->type == s_line_close ?
	 														((XWPSLineCloseSegment *)psegA)->sub->next : psegA->next) :
     													(psegA->type == s_start ? ((XWPSSubpath *)psegA)->last->prev : psegA->prev));
     														
	XWPSFixedPoint npt;
  npt.y = nextA->pt.y;
  if (npt.y <= psegA->pt.y) 
  {
		XWPSActiveLine *nlp = next;
		prev->next = nlp;
		if (nlp)
	    nlp->prev = prev;
		return true;
  }
  pseg = nextA;
  npt.x = nextA->pt.x;
  set_al_points(this, end, npt);
  return false;
}

void XWPSActiveLine::resortXLine()
{
	XWPSActiveLine *prevA = prev;
  XWPSActiveLine *nextA = next;
	prevA->next = nextA;
	if (nextA)
		nextA->prev = prevA;
		
	while (xorder(prevA, this) > 0)
	{
		nextA = prevA, prevA = prevA->prev;
	}
	next = nextA;
  prev = prevA;
  if (nextA)
		nextA->prev = this;
  prevA->next = this;
}

void XWPSActiveLine::setScanLinePoints(long fixed_flat)
{
	const XWPSSegment *psegA = pseg;
  const XWPSFixedPoint *pp0;

    if (direction < 0) 
    {
			psegA =  (psegA->type == s_line_close ? ((XWPSLineCloseSegment *)psegA)->sub->next :  psegA->next);
			if (psegA->type != s_curve) 
			{
	    	curve_k = -1;
	    	return;
			}
			pp0 = &end;
    } 
    else 
    {
			if (psegA->type != s_curve) 
			{
	    	curve_k = -1;
	    	return;
			}
			pp0 = &start;
    }
#define pcseg ((XWPSCurveSegment *)psegA)
    curve_k =	pcseg->log2Samples(pp0->x, pp0->y, fixed_flat);
    cursor.init(pp0->x, pp0->y, pcseg, curve_k);
#undef pcseg
}

int XWPSActiveLine::xorder(const XWPSActiveLine *lp1, const XWPSActiveLine *lp2)
{
	if (lp1->x_current < lp2->x_current)
		return -1;
  else if (lp1->x_current > lp2->x_current)
		return 1;
		
	bool s1;
	if ((s1 = lp1->start.x < lp1->end.x) != (lp2->start.x < lp2->end.x))
		return (s1 ? 1 : -1);
			
	{
		double diff = ((double)(lp1->end.x - lp1->start.x) * (lp2->end.y - lp2->start.y)) -
	    						((double)(lp2->end.x - lp2->start.x) * (lp1->end.y - lp1->start.y));

		return (diff < 0 ? -1 : diff > 0 ? 1 : 0);
  }
}

XWPSLineList::XWPSLineList()
{
	active_area = 0;
	next_active = local_active;
	limit = next_active + max_local_active;
	close_count = 0;
	y_list = 0;
	y_line = 0;
}

XWPSLineList::~XWPSLineList()
{
	XWPSActiveLine *alp;
	while ((alp = active_area) != 0)
	{
		XWPSActiveLine *next = alp->alloc_next;
		delete alp;
		active_area = next;
	}
}

int XWPSLineList::add_y_line(const XWPSSegment * prev_lp, const XWPSSegment * lp, int dir)
{
	XWPSFixedPoint thi, prev;
  XWPSActiveLine *alp = next_active;
  long y_start;
  if (alp == limit)
  {
  	alp = new XWPSActiveLine;
  	alp->alloc_next = active_area;
		active_area = alp;
  }
  else
		next_active++;
		
	thi.x = lp->pt.x;
  thi.y = lp->pt.y;
  prev.x = prev_lp->pt.x;
  prev.y = prev_lp->pt.y;
  switch ((alp->direction = dir))
  {
  	case dir_up:
	    y_start = prev.y;
	    set_al_points(alp, prev, thi);
	    alp->pseg = (XWPSSegment*)lp;
	    break;
	    
		case dir_down:
	    y_start = thi.y;
	    set_al_points(alp, thi, prev);
	    alp->pseg = (XWPSSegment*)prev_lp;
	    break;
	    
		case dir_horizontal:
	    y_start = thi.y;
	    alp->start = prev;
	    alp->end = thi;
	    alp->pseg = (XWPSSegment*)prev_lp;
	    break;
	    
		default:
	    return (int)(XWPSError::Unregistered);
  }
  
  {
  	XWPSActiveLine *yp = y_line;
		XWPSActiveLine *nyp;
		if (yp == 0)
		{
			alp->next = alp->prev = 0;
	    y_list = alp;
		}
		else if (y_start >= yp->start.y)
		{
			while (((nyp = yp->next) != NULL && y_start > nyp->start.y))
				yp = nyp;
				
			alp->next = nyp;
	    alp->prev = yp;
	    yp->next = alp;
	    if (nyp)
				nyp->prev = alp;
		}
		else
		{
			while (((nyp = yp->prev) != NULL && y_start < nyp->start.y))
				yp = nyp;
	    alp->prev = nyp;
	    alp->next = yp;
	    yp->prev = alp;
	    if (nyp)
				nyp->next = alp;
	    else
				y_list = alp;
		}
  }
  
  y_line = alp;
  return 0;
}

int XWPSLineList::add_y_list(XWPSPath * ppath, 
	               long adjust_below, 
	               long adjust_above,
	               const XWPSFixedRect * pbox)
{
	XWPSSegment *pseg = ppath->firstSubpath();
  int close_count = 0;
  long ymin = pbox->p.y;
  long ymax = pbox->q.y;
  int code;
  
  while (pseg)
  {
  	XWPSSubpath *psub = (XWPSSubpath *) pseg;
		XWPSSegment *plast = psub->last;
		int dir = 2;
		int first_dir, prev_dir;
		XWPSSegment *prev;
		if (plast->type != s_line_close)
		{
			XWPSLineCloseSegment *lp = &psub->closer;
	    XWPSSegment *next = plast->next;

	    lp->next = next;
	    lp->prev = plast;
	    plast->next = lp;
	    if (next)
				next->prev = lp;
	    lp->type = s_line_close;
	    lp->pt = psub->pt;
	    lp->sub = psub;
	    psub->last = plast = lp;
	    close_count++;
		}
		
		while ((prev_dir = dir, prev = pseg, (pseg = pseg->next) != 0 && pseg->type != s_start))
		{
			long iy = pseg->pt.y;
	    long py = prev->pt.y;
	    
#define compute_dir(xo, xe, yo, ye)\
  (ye > yo ? (ye <= ymin || yo >= ymax ? 0 : dir_up) :\
   ye < yo ? (yo <= ymin || ye >= ymax ? 0 : dir_down) :\
   2)
#define add_dir_lines(prev2, prev, this, pdir, dir)\
  if ( pdir )\
   { if ( (code = add_y_line(prev2, prev, pdir)) < 0 ) return code; }\
  if ( dir )\
   { if ( (code = add_y_line(prev, this, dir)) < 0 ) return code; }
	    dir = compute_dir(prev->pt.x, pseg->pt.x, py, iy);
	    
	    if (dir == 2)
	    {
	    	if (fixed2int_pixround(iy - adjust_below) < fixed2int_pixround(iy + adjust_above)) 
	    	{
		    	if ((code = add_y_line(prev, pseg, dir_horizontal)) < 0)
						return code;
				}
				dir = 0;
	    }
	    if (dir > prev_dir) 
	    {
				add_dir_lines(prev->prev, prev, pseg, prev_dir, dir);
	    } 
	    else if (prev_dir == 2)
				first_dir = dir;
			if (pseg == plast)
			{
				if (first_dir > dir) 
				{
		    	add_dir_lines(prev, pseg, psub->next, dir, first_dir);
				}
			}
		}
#undef compute_dir
#undef add_dir_lines
  }
  return close_count;
}

void XWPSLineList::insertXNew(XWPSActiveLine * alp)
{
	XWPSActiveLine *next;
  XWPSActiveLine *prev = &x_head;

  alp->x_current = alp->start.x;
  while ((next = prev->next) != 0 && XWPSActiveLine::xorder(next, alp) < 0)
		prev = next;
  alp->next = next;
  alp->prev = prev;
  if (next != 0)
		next->prev = alp;
  prev->next = alp;
}

int XWPSEndPoint::add_round_cap(XWPSPath * ppath)
{
	int code;
	if ((code = ppath->addPartialArcNotes(p.x + cdelta.x, p.y + cdelta.y,co.x + cdelta.x, co.y + cdelta.y,PS_QUARTER_ARC_FRACTION,sn_none)) < 0 ||
			(code = ppath->addPartialArcNotes(ce.x, ce.y, ce.x + cdelta.x, ce.y + cdelta.y,PS_QUARTER_ARC_FRACTION,sn_none)) < 0 ||
			(code = ppath->addPartialArcNotes(p.x - cdelta.x, p.y - cdelta.y,ce.x - cdelta.x, ce.y - cdelta.y,PS_QUARTER_ARC_FRACTION,sn_none)) < 0 ||
			(code = ppath->addPartialArcNotes(co.x, co.y, co.x - cdelta.x, co.y - cdelta.y,	PS_QUARTER_ARC_FRACTION,sn_none)) < 0 ||
			(code = ppath->addLineNotes(ce.x, ce.y, sn_none)) < 0)
		return code;
  return 0;
}

int XWPSEndPoint::cap_points(PSLineCap type, XWPSFixedPoint *pts)
{
#define PUT_POINT(i, px, py)\
  pts[i].x = (px), pts[i].y = (py)
    switch (type) {
	case ps_cap_butt:
	    PUT_POINT(0, co.x, co.y);
	    PUT_POINT(1, ce.x, ce.y);
	    return 2;
	case ps_cap_square:
	    PUT_POINT(0, co.x + cdelta.x, co.y + cdelta.y);
	    PUT_POINT(1, ce.x + cdelta.x, ce.y + cdelta.y);
	    return 2;
	case ps_cap_triangle:
	    PUT_POINT(0, co.x, co.y);
	    PUT_POINT(1, p.x + cdelta.x, p.y + cdelta.y);
	    PUT_POINT(2, ce.x, ce.y);
	    return 3;
	default:
	    return (int)(XWPSError::Unregistered);
    }
#undef PUT_POINT
}

XWPSPartialLine::XWPSPartialLine()
{
	thin = false;
}

void XWPSPartialLine::adjust_stroke(const XWPSImagerState * pis, bool thin)
{
	long *pw;
  long *pov;
  long *pev;
  long w, w2;
  long adj2;
  
  if (!pis->stroke_adjust && width.x != 0 && width.y != 0)
		return;
		
	if (qAbs(width.x) < qAbs(width.y))
	{
		pw = &width.y, pov = &o.p.y, pev = &e.p.y;
		adj2 = STROKE_ADJUSTMENT(thin, pis, y) << 1;
	}
	else
	{
		pw = &width.x, pov = &o.p.x, pev = &e.p.x;
		adj2 = STROKE_ADJUSTMENT(thin, pis, x) << 1;
	}
	
	w = *pw;
  w2 = fixed_rounded(w << 1);
  if (w2 == 0 && *pw != 0)
  {
  	w2 = (*pw < 0 ? -fixed_1 + adj2 : fixed_1 - adj2);
		*pw = arith_rshift_1(w2);
  }
  
  if (*pov == *pev)
  {
  	if (w >= 0)
	    w2 += adj2;
		else
	    w2 = adj2 - w2;
		if (w2 & fixed_1)
	    *pov = *pev = fixed_floor(*pov) + fixed_half;
		else
	    *pov = *pev = fixed_rounded(*pov);
  }
}

void XWPSPartialLine::compute_caps()
{
	long wx2 = width.x;
  long wy2 = width.y;

  o.co.x = o.p.x + wx2, o.co.y = o.p.y + wy2;
  o.cdelta.x = -e.cdelta.x, o.cdelta.y = -e.cdelta.y;
  o.ce.x = o.p.x - wx2, o.ce.y = o.p.y - wy2;
  e.co.x = e.p.x - wx2, e.co.y = e.p.y - wy2;
  e.ce.x = e.p.x + wx2, e.ce.y = e.p.y + wy2;
}

int XWPSPartialLine::line_join_points(const XWPSLineParams * pgs_lp, 
	                                    XWPSPartialLine * plp,
		                                  XWPSFixedPoint * join_points, 
		                                  const XWPSMatrix * pmat,
		 									                PSLineJoin join)
{
	bool ccw =(double)(plp->width.x) * (width.y) >	(double)(width.x) * (plp->width.y);
  XWPSFixedPoint * outp, *np;
  
  join_points[0].x = plp->e.co.x;
  join_points[0].y = plp->e.co.y;
  join_points[3].x = plp->e.ce.x;
  join_points[3].y = plp->e.ce.y;
  
  if (!ccw) 
  {
		outp = &join_points[3];
		join_points[2].x = o.co.x;
		join_points[2].y = o.co.y;
		join_points[1].x = o.p.x;
		join_points[1].y = o.p.y;
		np = &join_points[2];
  } 
  else 
  {
		outp = &join_points[0];
		join_points[1].x = o.ce.x;
		join_points[1].y = o.ce.y;
		join_points[2].x = o.p.x;
		join_points[2].y = o.p.y;
		np = &join_points[1];
  }
  
  if (join == ps_join_triangle) 
  {
		long tpx = outp->x - o.p.x + np->x;
		long tpy = outp->y - o.p.y + np->y;
		
		join_points[4].x = join_points[3].x;
		join_points[4].y = join_points[3].y;
		if (!ccw) 
		{
	    join_points[3].x = tpx, join_points[3].y = tpy;
		} 
		else 
		{
			join_points[3].x = join_points[2].x;
			join_points[3].y = join_points[2].y;
			
			join_points[2].x = join_points[1].x;
			join_points[2].y = join_points[1].y;
	    join_points[1].x = tpx, join_points[1].y = tpy;
		}
		return 5;
  }
  
  if (join == ps_join_miter &&
			!(fixed2long(outp->x << 1) == fixed2long(np->x << 1) &&
	  	fixed2long(outp->y << 1) == fixed2long(np->y << 1)))
	{
		float check = pgs_lp->miter_check;
		double u1 = plp->e.cdelta.y, v1 = plp->e.cdelta.x;
		double u2 = o.cdelta.y, v2 = o.cdelta.x;
		double num, denom;
		
		if (pmat)
		{
			XWPSPoint pt;

	    pt.distanceTransformInverse(v1, u1, (XWPSMatrix*)pmat);
	    v1 = pt.x, u1 = pt.y;
	    pt.distanceTransformInverse(v2, u2, (XWPSMatrix*)pmat);
	    v2 = pt.x, u2 = pt.y;
	    ccw = v1 * u2 < v2 * u1;
		}
		
		num = u1 * v2 - u2 * v1;
		denom = u1 * u2 + v1 * v2;
		if (!ccw)
			num = -num;
			
		if (denom < 0)
	    num = -num, denom = -denom;
	  if (check > 0 ?
	    (num < 0 || num >= denom * check) :
	    (num < 0 && num >= denom * check))
	  {
	  	XWPSFixedPoint mpt;
	  	if (line_intersect(outp, &plp->e.cdelta, np, &o.cdelta, &mpt) == 0)
	  	{
	  		outp->x = mpt.x;
	  		outp->y = mpt.y;
	  	}
	  }
	}
	return 4;
}

int XWPSPartialLine::line_intersect(XWPSFixedPoint * pp1,
		  							 XWPSFixedPoint * pd1,
		  							 XWPSFixedPoint * pp2,
		  							 XWPSFixedPoint * pd2,
		  							 XWPSFixedPoint * pi)
{
	float u1 = pd1->x, v1 = pd1->y;
  float u2 = pd2->x, v2 = pd2->y;
  double denom = u1 * v2 - u2 * v1;
  float xdiff = pp2->x - pp1->x;
  float ydiff = pp2->y - pp1->y;
  double f1;
  double max_result = qAbs(denom) * (double)max_fixed;
  
  if (qAbs(xdiff) >= max_result || qAbs(ydiff) >= max_result) 
		return -1;
		
  f1 = (v2 * xdiff - u2 * ydiff) / denom;
  pi->x = pp1->x + (long) (f1 * u1);
  pi->y = pp1->y + (long) (f1 * v1);
  return (f1 >= 0 && (v1 * xdiff >= u1 * ydiff ? denom >= 0 : denom < 0) ? 0 : 1);
}

void XWPSPartialLine::set_thin_widths()
{
	long dx = e.p.x - o.p.x, dy = e.p.y - o.p.y;

#define TRSIGN(v, c) ((v) >= 0 ? (c) : -(c))
  if (qAbs(dx) > qAbs(dy)) 
  {
		width.x = e.cdelta.y = 0;
		width.y = e.cdelta.x = TRSIGN(dx, fixed_half);
  } 
  else 
  {
		width.y = e.cdelta.x = 0;
		width.x = -(e.cdelta.y = TRSIGN(dy, fixed_half));
  }
#undef TRSIGN
}

bool XWPSPartialLine::width_is_thin()
{
	long dx, dy, wx = width.x, wy = width.y;
	
	if ((dy = e.p.y - o.p.y) == 0)
		return qAbs(wy) < fixed_half;
  if ((dx = e.p.x - o.p.x) == 0)
		return qAbs(wx) < fixed_half;
		
	if (qAbs(wx) < fixed_half && qAbs(wy) < fixed_half)
		return true;
		
	{
		double C = dx, D = dy;
		double num = C * wy - D * wx;
		double denom = hypot(C, D);
		return fabs(num) < denom * 0.5;
  }
}

#define YMULT_LIMIT (max_fixed / fixed_1)

XWPSTrapLine::XWPSTrapLine()
{
	h = 0;
	di = 0;
	df = 0;
	x = xf = 0;
	ldi = ldf = 0;
}

void XWPSTrapLine::compute_dx(long xd, long ys)
{
	long hA = h;
  int diA;
  if (xd >= 0) 
  {
		if (xd < hA)
	    di = 0, df = xd;
		else 
		{
	    di = diA = (int)(xd / hA);
	    df = xd - diA * hA;
	    x += ys * diA;
		}
  } 
  else 
  {
		if ((df = xd + hA) >= 0)
	    di = -1, x -= ys;
		else 
		{
	    di = diA = (int)-((hA - 1 - xd) / hA);
	    df = xd - diA * hA;
	    x += ys * diA;
		}
  }
}

void XWPSTrapLine::compute_ldx(long ys)
{
	int diA = di;
  long dfA = df;
  long hA = h;

  if (dfA < YMULT_LIMIT ) 
  {
	 	if ( dfA == 0 )	
	    ldi = int2fixed(diA), ldf = 0, xf = -hA;
	 	else 
	 	{
	     ldi = int2fixed(diA) + int2fixed(dfA) / hA;
	     ldf = int2fixed(dfA) % hA;
	     xf = (ys < fixed_1 ? ys * dfA % hA : fixed_mult_rem(ys, dfA, hA)) - hA;
	 	}
  }
  else 
  {
		ldi = int2fixed(diA) + fixed_mult_quo(fixed_1, dfA, hA);
		ldf = fixed_mult_rem(fixed_1, dfA, hA);
		xf = fixed_mult_rem(ys, dfA, hA) - hA;
  }
}

XWPSPathSegments::XWPSPathSegments()
	:XWPSStruct()
{
	contents.subpath_first = 0;
	contents.subpath_current = 0;
}

XWPSPathSegments::~XWPSPathSegments()
{
	clear();
}

void XWPSPathSegments::clear()
{
	if (contents.subpath_first != 0)
	{
		XWPSSegment * pseg = (XWPSSegment *)(contents.subpath_current->last);
		while (pseg)
		{
			XWPSSegment *prev = pseg->prev;
			delete pseg;
			pseg = prev;
		}
	}
	
	contents.subpath_first = 0;
	contents.subpath_current = 0;
}

int XWPSPathSegments::getLength()
{
	return sizeof(XWPSPathSegments);
}

const char * XWPSPathSegments::getTypeName()
{
	return "pathsegments";
}

XWPSPath::XWPSPath()
{
	segments = &local_segments;
	initContents();
}

XWPSPath::XWPSPath(const XWPSPath * shared)
{
	if (shared)
	{
		if (shared->segments == &(shared->local_segments))
		{
			segments = new XWPSPathSegments;
			segments->contents.subpath_first = shared->segments->contents.subpath_first;
			segments->contents.subpath_current = shared->segments->contents.subpath_current;
			shared->segments->contents.subpath_first = 0;
			shared->segments->contents.subpath_current = 0;
		}
		else
		{
			segments = shared->segments;
			segments->incRef();
		}
		copyContents(shared);
	}
	else
	{
		segments = new XWPSPathSegments;
		initContents();
	}
}

XWPSPath::~XWPSPath()
{
	clear();
}

int XWPSPath::addCharPath(XWPSPath * from_path, PSCharPathMode mode)
{
	int code;
  XWPSFixedRect bboxA;
  switch (mode)
  {
  	default:
	    from_path->newPath();
	    return 0;
	    
	  case cpm_charwidth: 
	  	{
	    	XWPSFixedPoint cpt;
	    	code = from_path->currentPoint(&cpt);
	    	if (code < 0)
					break;
	    	return addPoint(cpt.x, cpt.y);
			}
			
		case cpm_true_charpath:
		case cpm_false_charpath:
	    return addPath(from_path);
		    
		case cpm_true_charboxpath:
	    from_path->getBbox(&bboxA);
	    code = addRectangle(bboxA.p.x, bboxA.p.y, bboxA.q.x, bboxA.q.y);
	    break;
	    
		case cpm_false_charboxpath:
	    from_path->getBbox(&bboxA);
	    code = addPoint(bboxA.p.x, bboxA.p.y);
	    if (code >= 0)
				code = addLineNotes(bboxA.q.x, bboxA.q.y, sn_none);
	    break;
  }
  
  if (code < 0)
		return code;
  from_path->newPath();
  return 0;
}

int XWPSPath::addCurveNotes(long x1, 
	                  long y1, 
	                  long x2, 
	                  long y2, 
	                  long x3, 
	                  long y3,
			              PSSegmentNotes notesA)
{
  if (bbox_set) 
  {
		int code_ = checkInBbox(x1, y1);
		if (code_ < 0)
  		return code_;
		code_ = checkInBbox(x2, y2);
		if (code_ < 0)
  		return code_;
		code_ = checkInBbox(x3, y3);
		if (code_ < 0)
  		return code_;
  }
  
  if (!isDrawing())
  {
  	if ( !positionValid() )
  		return (int)(XWPSError::NoCurrentPoint);
  			
    int code_ = newSubpath();
  	if (code_ < 0)
  		return code_;
  }
  
  int code = unshare();
	if (code < 0)
		return code;
  XWPSSubpath *psub = segments->contents.subpath_current;
  XWPSCurveSegment *lp = new XWPSCurveSegment(notesA);
  XWPSSegment *prevA = psub->last;
  prevA->next = lp;
  lp->prev = prevA;
  psub->last = lp;
  lp->p1.x = x1;
  lp->p1.y = y1;
  lp->p2.x = x2;
  lp->p2.y = y2;
  lp->pt.x = position.x = x3;
	lp->pt.y = position.y = y3;
	psub->curve_count++;
  curve_count++;
  updateDraw();
  return 0;
}

int XWPSPath::addDashExpansion(XWPSPath * ppath_old, XWPSImagerState * pis)
{
	XWPSSubpath *psub;
  XWPSDashParams *dash = &(pis->currentLineParams()->dash);
  int code = 0;

  if (dash->pattern_size == 0)
		return copy(ppath_old);
  for (psub = ppath_old->firstSubpath(); psub != 0 && code >= 0; psub = (XWPSSubpath *)psub->last->next)
		code = subpathExpandDashes(psub, pis, dash);
  return code;
}

int XWPSPath::addFlattenedAccurate(XWPSPath * old, float flatness, bool accurate)
{
	return copyReducing(old, float2fixed(flatness), NULL,accurate ? pco_accurate : pco_none);
}

int XWPSPath::addLineNotes(long x, long y, PSSegmentNotes notesA)
{
	if (bbox_set)
	{
		int code_ = checkInBbox(x, y);
		if (code_ < 0)
  		return code_;
	}
		
	if (!isDrawing())
  {
  	if ( !positionValid() )
  		return (int)(XWPSError::NoCurrentPoint);
  			
    int code_ = newSubpath();
  	if (code_ < 0)
  		return code_;
  }
  
  int code = unshare();
	if (code < 0)
		return code;
  XWPSSubpath *psub = segments->contents.subpath_current;
  XWPSLineSegment *lp = new XWPSLineSegment(notesA);
  XWPSSegment *prevA = psub->last;
  prevA->next = lp;
  lp->prev = prevA;
  psub->last = lp;
  lp->pt.x = position.x = x;
	lp->pt.y = position.y = y;
	updateDraw();
  return 0;
}

int XWPSPath::addLinesNotes(const XWPSFixedPoint *ppts, 
	                          int count,
			                      PSSegmentNotes notesA)
{
  int code = 0;
  if (count <= 0)
		return 0;
		
	code = unshare();
	if (code < 0)
		return code;
		
	if (!isDrawing())
  {  	
  	if ( !positionValid() )
  		return (int)(XWPSError::NoCurrentPoint);
  			
    int code_ = newSubpath();
  	if (code_ < 0)
  		return code_;
  }
  
  XWPSSubpath *psub = segments->contents.subpath_current;
  XWPSLineSegment *lp = 0;
  XWPSSegment * prev = psub->last;
  for (int i = 0; i < count; i++)
  {
  	long x = ppts[i].x;
		long y = ppts[i].y;
		if (bbox_set && outsideBbox(x, y)) 
		{
	    code = XWPSError::RangeCheck;
	    break;
		}
		
		XWPSLineSegment * next = new XWPSLineSegment(notesA);
		lp = next;
		prev->next = lp;
		lp->prev = prev;
		lp->pt.x = x;
		lp->pt.y = y;
		prev = lp;
  }
  
  if (lp != 0)
		position.x = lp->pt.x,position.y = lp->pt.y,psub->last = lp, lp->next = 0,
	updateDraw();
  return code;
}

int XWPSPath::addMonotonized(XWPSPath * old)
{
	return copyReducing(old, max_fixed, NULL, pco_monotonize);
}

int XWPSPath::addPartialArcNotes(long x3, 
	                       long y3, 
	                       long xt, 
	                       long yt, 
	                       float fraction, 
	                       PSSegmentNotes notesA)
{
	long x0 = position.x, y0 = position.y;

  return addCurveNotes(x0 + (long) ((xt - x0) * fraction),
				   						 y0 + (long) ((yt - y0) * fraction),
				   						 x3 + (long) ((xt - x3) * fraction),
				   						 y3 + (long) ((yt - y3) * fraction),
				   						 x3, y3, (PSSegmentNotes)(notesA | sn_from_arc));
}

int XWPSPath::addPath(XWPSPath * ppfrom)
{
	int code = ppfrom->unshare();
	if (code < 0)
		return code;
		
	code = unshare();
	if (code < 0)
		return code;
		
	if (ppfrom->firstSubpath())
	{
		if (firstSubpath())
		{
			XWPSSubpath *psub = currentSubpath();
	    XWPSSegment *pseg = psub->last;
	    XWPSSubpath *pfsub = ppfrom->firstSubpath();

	    pseg->next = pfsub;
	    pfsub->prev = pseg;
		}
		else
			segments->contents.subpath_first = ppfrom->firstSubpath();
			
		segments->contents.subpath_current = ppfrom->currentSubpath();
		subpath_count += ppfrom->subpath_count;
		curve_count += ppfrom->curve_count;
	}
	
	position = ppfrom->position;
  outside_position = ppfrom->outside_position;
  state_flags = ppfrom->state_flags;
  ppfrom->initContents();
  return 0;
}

int XWPSPath::addPoint(long x, long y)
{
	if (bbox_set)
	{
		int code = checkInBbox(x, y);
		if (code < 0)
			return code;
	}
	position.x = x;
  position.y = y;
  updateMoveTo();
  return 0;
}

int XWPSPath::addPoints(const XWPSFixedPoint * points, int npoints, bool moveto_first)
{
	if (moveto_first) 
	{
		int code = addPoint(points[0].x, points[0].y);
		if (code < 0)
	    return code;
	    
		return addLinesNotes(points + 1, npoints - 1, sn_none);
  } 
  else
		return addLinesNotes(points, npoints, sn_none);
}

int XWPSPath::addRectangle(long x0, long y0, long x1, long y1)
{
	XWPSFixedPoint pts[3];
  int code;

  pts[0].x = x0;
  pts[1].x = pts[2].x = x1;
  pts[2].y = y0;
  pts[0].y = pts[1].y = y1;
  if ((code = addPoint(x0, y0)) < 0 ||
			(code = addLinesNotes(pts, 3, sn_none)) < 0 ||
			(code = closeSubpathNotes(sn_none)) < 0)
		return code;
  return 0;
}

int XWPSPath::addRelativePoint(long dx, long dy)
{
	if (!positionInRange())
		return (int)(XWPSError::LimitCheck);
			
	{
		long nx = position.x + dx, ny = position.y + dy;

		if (((nx ^ dx) < 0 && (position.x ^ dx) >= 0) ||
	    ((ny ^ dy) < 0 && (position.y ^ dy) >= 0))
	    return (int)(XWPSError::LimitCheck);
		if (bbox_set)
	    checkInBbox(nx, ny);
		position.x = nx;
		position.y = ny;
 }
 updateMoveTo();
 return 0;
}

int XWPSPath::addRelPoint(long dx, long dy)
{
	if (!positionInRange() || bbox_set)
		addRelativePoint(dx, dy);
	else
	{
		position.x += dx; 
		position.y += dy;
		updateMoveTo();
	}
	
	return 0;
}

int XWPSPath::assign(XWPSPath * shared, bool preserve)
{
	if (shared->segments == segments)
		return 0;
		
	clear();	
	copyContents(shared);
	if (shared->segments == &(shared->local_segments))
	{
		segments = new XWPSPathSegments;
		segments->contents.subpath_first = shared->segments->contents.subpath_first;
		segments->contents.subpath_current = shared->segments->contents.subpath_current;
		shared->segments->contents.subpath_first = 0;
		shared->segments->contents.subpath_current = 0;
	}
	else
	{
		segments = shared->segments;
		shared->segments->incRef();
	}
	if (!preserve)
		shared->clear();
	
	return 0;
}

int XWPSPath::checkInBbox(long px, long py)
{
	if (outsideBbox(px, py)) 
		return (int)(XWPSError::RangeCheck);
	
	return 0;
}

int XWPSPath::closeSubpathNotes(PSSegmentNotes notesA)
{
	if (!subpathOpen())
		return 0;
		
	int code = 0;
	if (lastIsMoveTo())
	{
		code = newSubpath();
		if (code < 0)
	    return code;
	}
	
	code = unshare();
	if (code < 0)
		return code;
		
	XWPSSubpath *psub = segments->contents.subpath_current;
  XWPSLineCloseSegment *lp = new XWPSLineCloseSegment(notesA);
  XWPSSegment *prevA = psub->last;
  prevA->next = lp;
  lp->prev = prevA;
  psub->last = lp;
  lp->pt.x = position.x = psub->pt.x;
	lp->pt.y = position.y = psub->pt.y;
	lp->sub = psub;
  psub->is_closed = 1;
	updateClosePath();
  return 0;
}

int XWPSPath::copy(XWPSPath *ppath_old)
{
	return copyReducing(ppath_old, max_fixed, NULL, pco_none);
}

int XWPSPath::copyReversed(XWPSPath * ppath_old)
{
	XWPSSubpath *psub = ppath_old->segments->contents.subpath_first;
	
nsp:
  if (psub)
  {
  	XWPSSegment *prevA = psub->last;
		XWPSSegment *pseg;
		PSSegmentNotes notesA =  (PSSegmentNotes)(prevA == psub ? sn_none :  psub->next->notes);
		PSSegmentNotes prev_notes;
		int code;

		if (!psub->is_closed) 
		{
	    code = addPoint(prevA->pt.x, prevA->pt.y);
	    if (code < 0)
				return code;
		}
		
		do 
		{
	    pseg = prevA;
	    prev_notes = notesA;
	    prevA = pseg->prev;
	    notesA = (PSSegmentNotes)pseg->notes;
	    prev_notes = (PSSegmentNotes)((prev_notes & sn_not_first) |	(notesA & ~sn_not_first));
	    switch (pseg->type) 
	    {
				case s_start:
		    	if (psub->is_closed) 
		    	{
						code =  closeSubpathNotes(prev_notes);
						if (code < 0)
			    		return code;
		    	}
		    	psub = (XWPSSubpath *)psub->last->next;
		    	goto nsp;
		    	
				case s_curve:
		    	{
						XWPSCurveSegment *pc =(XWPSCurveSegment *)pseg;

						code = addCurveNotes(pc->p2.x, pc->p2.y, pc->p1.x, pc->p1.y,prevA->pt.x, prevA->pt.y, prev_notes);
						break;
		    	}
		    	
				case s_line:
		    	code = addLineNotes(prevA->pt.x, prevA->pt.y, prev_notes);
		    	break;
		    	
				case s_line_close:
		    	code = addPoint(prevA->pt.x, prevA->pt.y);
		    	break;
		    	
				default:
		    	return (int)(XWPSError::Fatal);
	    }
		} while (code >= 0);
		return code;
  }
  
  if (ppath_old->segments->contents.subpath_first == 0 &&	ppath_old->lastIsMoveTo()) 
  {
		int code = addPoint(ppath_old->position.x, ppath_old->position.y);

		if (code < 0)
	    return code;
  }
  
  return 0;
}

int XWPSPath::copyReducing(XWPSPath *ppath_old, 
		      				 long fixed_flatness, 
		      				 XWPSImagerState *pis,
		               PSPathCopyOptions options)
{
	XWPSSegment *pseg;
  long flat = fixed_flatness;
  XWPSFixedPoint expansion;
  int code = unshare();
  if (code < 0)
  	return code;
  	
  if (options & pco_for_stroke)
  {
  	double width = pis->line_params.half_width;
		expansion.x = float2fixed((fabs(pis->ctm.xx) + fabs(pis->ctm.yx)) * width) * 2;
		expansion.y = float2fixed((fabs(pis->ctm.xy) + fabs(pis->ctm.yy)) * width) * 2;
  }
  pseg = ppath_old->segments->contents.subpath_first;
  while (pseg)
  {
  	switch (pseg->type)
  	{
  		case s_start:
				code = addPoint(pseg->pt.x, pseg->pt.y);
				break;
				
			case s_curve:
				{
					XWPSCurveSegment *pc = (XWPSCurveSegment *)pseg;
					if (fixed_flatness == max_fixed)
					{
						if (options & pco_monotonize)
			    		code = monotonizeInternal(pc);
						else
			    		code = addCurveNotes(pc->p1.x, pc->p1.y, pc->p2.x, pc->p2.y, pc->pt.x, pc->pt.y, (PSSegmentNotes)(pseg->notes));
					}
					else
					{
						long x0 = position.x;
						long y0 = position.y;
						PSSegmentNotes notesA = (PSSegmentNotes)pseg->notes;
						XWPSCurveSegment cseg;
						int k;
						if (options & pco_for_stroke)
						{
							long min01, max01, min23, max23;
			    	  long ex, ey, flat_x, flat_y;
#define SET_EXTENT(r, c0, c1, c2, c3)\
	if (c0 < c1) min01 = c0, max01 = c1;\
	else         min01 = c1, max01 = c0;\
	if (c2 < c3) min23 = c2, max23 = c3;\
	else         min23 = c3, max23 = c2;\
	r = qMax(max01, max23) - qMin(min01, min23);
	
			    SET_EXTENT(ex, x0, pc->p1.x, pc->p2.x, pc->pt.x);
			    SET_EXTENT(ey, y0, pc->p1.y, pc->p2.y, pc->pt.y);
#undef SET_EXTENT

							if (ex == 0 || ey == 0)
								flat = 0;
			    		else 
			    		{
								flat_x =  fixed_mult_quo(fixed_flatness, ex, ex + expansion.x);
								flat_y =  fixed_mult_quo(fixed_flatness, ey, ey + expansion.y);
								flat = qMin(flat_x, flat_y);
			    		}
						}
						
						k = pc->log2Samples(x0, y0, flat);
						if (options & pco_accurate)
						{
							XWPSSegment *start;
			    		XWPSSegment *end;
			    		code = addLineNotes(x0, y0, notesA);
			    		if (code < 0)
								break;
			    		start = segments->contents.subpath_current->last;
			    		notesA = (PSSegmentNotes)(notesA |sn_not_first);
			    		cseg = *pc;
			    		code = flattenSample(k, &cseg, notesA);
			    		if (code < 0)
								break;
								
							end = segments->contents.subpath_current->last;
			    		if ((code = addLineNotes(position.x, position.y, (PSSegmentNotes)(pseg->notes | sn_not_first))) < 0)
								break;
			    		start->adjustPointToTangent(start->next, &pc->p1);
			    		end->adjustPointToTangent(end->prev, &pc->p2);
						}
						else
						{
							cseg = *pc;
			    		code = flattenSample(k, &cseg, notesA);
						}
					}
					break;
				}
				
			case s_line:
				code = addLineNotes(pseg->pt.x, pseg->pt.y, (PSSegmentNotes)(pseg->notes));
				break;
				
	    case s_line_close:
				code = closeSubpathNotes(sn_none);
				break;
				
	    default:		/* can't happen */
				code = XWPSError::Unregistered;
  	}
  	
  	if (code < 0) 
  	{
	    clear();
	    initContents();
	    return code;
		}
		pseg = pseg->next;
  }
  
  if (ppath_old->lastIsMoveTo())
		addPoint(ppath_old->position.x, ppath_old->position.y);
  if (ppath_old->bbox_set) 
  {
		if (bbox_set) 
		{
	    bbox.p.x = qMin(ppath_old->bbox.p.x, bbox.p.x);
	    bbox.p.y = qMin(ppath_old->bbox.p.y, bbox.p.y);
	    bbox.q.x = qMax(ppath_old->bbox.q.x, bbox.q.x);
	    bbox.q.y = qMax(ppath_old->bbox.q.y, bbox.q.y);
		} 
		else 
		{
	    bbox_set = true;
	    bbox = ppath_old->bbox;
		}
  }
  
  return 0;
}

int XWPSPath::currentPoint(XWPSFixedPoint * ppt)
{
	if (!positionValid())
		return (int)(XWPSError::NoCurrentPoint);
  ppt->x = position.x; 
  ppt->y = position.y;
  return 0;
}

int XWPSPath::flattenSample(int k, XWPSCurveSegment * pc, PSSegmentNotes notesA)
{
	long x0, y0;
	long cx, bx, ax, cy, by, ay;
  long ptx, pty;
  long x, y;
  
  uint i;
  uint rmask;
  long idx, idy, id2x, id2y, id3x, id3y;
  uint rx, ry, rdx, rdy, rd2x, rd2y, rd3x, rd3y;
  XWPSFixedPoint *ppt;
  
#define max_points 50		/* arbitrary */
  XWPSFixedPoint points[max_points + 1];

top:
	x0 = position.x;
  y0 = position.y;
  {
		long x01, x12, y01, y12;
		curve_points_to_coefficients(x0, pc->p1.x, pc->p2.x, pc->pt.x, ax, bx, cx, x01, x12);
		curve_points_to_coefficients(y0, pc->p1.y, pc->p2.y, pc->pt.y, ay, by, cy, y01, y12);
  }
  
#define max_fast (max_fixed / 6)
#define min_fast (-max_fast)
#define in_range(v) (v < max_fast && v > min_fast)
  if (k == 0) 
		return addLineNotes(pc->pt.x, pc->pt.y, notesA);
  
  if (k <= k_sample_max &&
			in_range(ax) && in_range(ay) &&
			in_range(bx) && in_range(by) &&
			in_range(cx) && in_range(cy))
	{
		x = x0, y = y0;
		rx = ry = 0;
		ppt = points;
		if (k == 1)
		{
#define poly2(a,b,c)\
  arith_rshift_1(arith_rshift_1(arith_rshift_1(a) + b) + c)
	    x += poly2(ax, bx, cx);
	    y += poly2(ay, by, cy);
#undef poly2
			
			if (((x ^ x0) | (y ^ y0)) & float2fixed(-0.5))
				ppt->x = ptx = x,
		  ppt->y = pty = y,
		  ppt++;
	    goto last;
		}
		else
		{
			long bx2 = bx << 1, by2 = by << 1;
	    long ax6 = ((ax << 1) + ax) << 1, ay6 = ((ay << 1) + ay) << 1;

#define adjust_rem(r, q)\
  if ( r > rmask ) q ++, r &= rmask
	    const int k2 = k << 1;
	    const int k3 = k2 + k;

	    rmask = (1 << k3) - 1;
	    
	    idx = arith_rshift(cx, k),
			idy = arith_rshift(cy, k);
	    rdx = ((uint) cx << k2) & rmask,
			rdy = ((uint) cy << k2) & rmask;
			
	    id2x = arith_rshift(bx2, k2),
			id2y = arith_rshift(by2, k2);
	    rd2x = ((uint) bx2 << k) & rmask,
			rd2y = ((uint) by2 << k) & rmask;
	    idx += arith_rshift_1(id2x),
			idy += arith_rshift_1(id2y);
	    rdx += ((uint) bx << k) & rmask,
			rdy += ((uint) by << k) & rmask;
	    adjust_rem(rdx, idx);
	    adjust_rem(rdy, idy);
	    
	    idx += arith_rshift(ax, k3),
			idy += arith_rshift(ay, k3);
	    rdx += (uint) ax & rmask,
			rdy += (uint) ay & rmask;
	    adjust_rem(rdx, idx);
	    adjust_rem(rdy, idy);
	    id2x += id3x = arith_rshift(ax6, k3),
			id2y += id3y = arith_rshift(ay6, k3);
	    rd2x += rd3x = (uint) ax6 & rmask,
			rd2y += rd3y = (uint) ay6 & rmask;
	    adjust_rem(rd2x, id2x);
	    adjust_rem(rd2y, id2y);
#undef adjust_rem
		}
	}
	else
	{
		XWPSCurveSegment cseg;
		int code;

		k--;
		pc->splitCurveMidpoint(x0, y0, &cseg, pc);
		code = flattenSample(k, &cseg, notesA);
		if (code < 0)
	    return code;
		notesA = (PSSegmentNotes)( notesA | sn_not_first);
		goto top;
	}
	
	ptx = x0, pty = y0;
  for (i = (1 << k) - 1;;)
  {
  	int code;
  	
#define accum(i, r, di, dr)\
  if ( (r += dr) > rmask ) r &= rmask, i += di + 1;\
  else i += di
		accum(x, rx, idx, rdx);
		accum(y, ry, idy, rdy);
		
#define coord_near(v, ptv)\
  (!( ((v) ^ (ptv)) & float2fixed(-0.5) ))
#define coords_in_order(v0, v1, v2)\
  ( (((v1) - (v0)) ^ ((v2) - (v1))) >= 0 )
		if (coord_near(x, ptx)) 
		{
	  	if (coord_near(y, pty))
				goto skip;
			
			if (ppt > points + 1 && coord_near(x, ppt[-2].x) &&
					coords_in_order(ppt[-2].x, ptx, x) &&
					coords_in_order(ppt[-2].y, pty, y))
				--ppt;
  	}
  	else if (coord_near(y, pty))
  	{
  		if (ppt > points + 1 && coord_near(y, ppt[-2].y) &&
					coords_in_order(ppt[-2].x, ptx, x) &&
					coords_in_order(ppt[-2].y, pty, y))
					--ppt;
  	}
#undef coord_near
#undef coords_in_order

		if (ppt == &points[max_points]) 
		{
	    if (notesA & sn_not_first)
				code = addLinesNotes(points, max_points,	notesA);
	    else 
	    {
				code = addLineNotes(points[0].x,   points[0].y, notesA);
				if (code < 0)
		    	return code;
				code = addLinesNotes(points,  max_points - 1, (PSSegmentNotes)(notesA | sn_not_first));
	    }
	    if (code < 0)
				return code;
	    ppt = points;
	    notesA = (PSSegmentNotes)(notesA | sn_not_first);
		}
		ppt->x = ptx = x;
		ppt->y = pty = y;
		ppt++;
		
skip:
		if (--i == 0)
	    break;
	    
	  accum(idx, rdx, id2x, rd2x);
		accum(id2x, rd2x, id3x, rd3x);
		accum(idy, rdy, id2y, rd2y);
		accum(id2y, rd2y, id3y, rd3y);
#undef accum
	}
	
last:
	if (ppt > points) 
	{
		int count = ppt + 1 - points;
		XWPSFixedPoint *pts = points;

		if (!(notesA & sn_not_first)) 
		{
	    int code = addLineNotes(points[0].x, points[0].y, notesA);
	    if (code < 0)
				return code;
	    ++pts, --count;
	    notesA = (PSSegmentNotes)(notesA | sn_not_first);
		}
		ppt->x = pc->pt.x, ppt->y = pc->pt.y;
		return addLinesNotes(pts, count, notesA);
  }
  return addLineNotes(pc->pt.x, pc->pt.y, notesA);
}

int XWPSPath::getBbox(XWPSFixedRect * pbox)
{
	if (bbox_set) 
	{
		*pbox = bbox;
		return 0;
  }
  
  if (segments->contents.subpath_first == 0)
  {
  	int code = currentPoint(&pbox->p);
  	if (code < 0)
  	{
  		pbox->p.x = pbox->p.y = 0;
  	}
  	pbox->q = pbox->p;
		return code;
  }
  
  if (box_last == segments->contents.subpath_current->last)
  	*pbox = bbox;
  else
  {
  	long px, py, qx, qy;
		XWPSSegment *pseg = box_last;
		if (pseg == 0)
		{
			pseg = segments->contents.subpath_first;
	    px = qx = pseg->pt.x;
	    py = qy = pseg->pt.y;
		}
		else
		{
			px = bbox.p.x; 
			py = bbox.p.y;
	    qx = bbox.q.x; 
	    qy = bbox.q.y;
		}
		
#define ADJUST_BBOX(pt)\
  if ((pt).x < px) px = (pt).x;\
  else if ((pt).x > qx) qx = (pt).x;\
  if ((pt).y < py) py = (pt).y;\
  else if ((pt).y > qy) qy = (pt).y
  	
  	while ((pseg = pseg->next) != 0) 
  	{
	    switch (pseg->type) 
	    {
				case s_curve:
		    	ADJUST_BBOX(((XWPSCurveSegment *)pseg)->p1);
		    	ADJUST_BBOX(((XWPSCurveSegment *)pseg)->p2);
		    	
				default:
		    	ADJUST_BBOX(pseg->pt);
	    }
		}
#undef ADJUST_BBOX

#define STORE_BBOX(b)\
  (b).p.x = px, (b).p.y = py, (b).q.x = qx, (b).q.y = qy;
		STORE_BBOX(*pbox);
		STORE_BBOX(bbox);
#undef STORE_BBOX
		box_last = segments->contents.subpath_current->last;
  }
  
  return 0;
}

int  XWPSPath::imagerArcAdd(XWPSImagerState * pis, 
	                  bool clockwise,
	                  float axc, 
	                  float ayc, 
	                  float arad, 
	                  float aang1, 
	                  float aang2,
		                bool add_line)
{
	double ar = arad;
  long ang1 = float2fixed(aang1), ang2 = float2fixed(aang2), anext;
  double ang1r;
  XWPSArcCurveParams arc;
  int code;

  arc.ppath = this;
  arc.pis = pis;
  arc.center.x = axc;
  arc.center.y = ayc;
#define fixed_90 int2fixed(90)
#define fixed_180 int2fixed(180)
#define fixed_360 int2fixed(360)
  if (ar < 0) 
  {
		ang1 += fixed_180;
		ang2 += fixed_180;
		ar = -ar;
  }
  
  arc.radius = ar;
  arc.action = (add_line ? arc_lineto : arc_moveto);
  arc.notes = sn_none;
  arc.fast_quadrant = 0;
  ang1r = fixed2float(ang1 % fixed_360);
  arc.sincos.sincosDegrees(ang1r);
  arc.p3.x = axc + ar * arc.sincos._cos;
  arc.p3.y = ayc + ar * arc.sincos._sin;
  if (clockwise)
  {
  	while (ang1 < ang2)
	    ang2 -= fixed_360;
		if (ang2 < 0) 
		{
	    long adjust = ROUND_UP(-ang2, fixed_360);
	    ang1 += adjust, ang2 += adjust;
		}
		
		arc.angle = ang1;
		if (ang1 == ang2)
	    goto last;
	    
		if (!arc.sincos.orthogonal) 
		{
	    anext = ROUND_DOWN(arc.angle - fixed_epsilon, fixed_90);
	    if (anext < ang2)
				goto last;
	    code = arc.nextArcCurve(anext);
	    if (code < 0)
				return code;
	    arc.action = arc_nothing;
	    arc.notes = sn_not_first;
		}	
		
		while ((anext = arc.angle - fixed_90) >= ang2) 
		{
	    code = arc.nextArcQuadrant(anext);
	    if (code < 0)
				return code;
	    arc.action = arc_nothing;
	    arc.notes = sn_not_first;
		}
  }
  else
  {
  	while (ang2 < ang1)
	    ang2 += fixed_360;
		if (ang1 < 0) 
		{
	    long adjust = ROUND_UP(-ang1, fixed_360);
	    ang1 += adjust, ang2 += adjust;
		}
		arc.angle = ang1;
		if (ang1 == ang2)
	    return arc.nextArcCurve(ang2);
		if (!arc.sincos.orthogonal) 
		{
	    anext = ROUND_UP(arc.angle + fixed_epsilon, fixed_90);
	    if (anext > ang2)
				goto last;
	    code = arc.nextArcCurve(anext);
	    if (code < 0)
				return code;
	    arc.action = arc_nothing;
	    arc.notes = sn_not_first;
		}	
		
		while ((anext = arc.angle + fixed_90) <= ang2) 
		{
	    code = arc.nextArcQuadrant(anext);
	    if (code < 0)
				return code;
	    arc.action = arc_nothing;
	    arc.notes = sn_not_first;
		}
  }
  
	if (arc.angle == ang2)
		return 0;
last:
  return arc.nextArcCurve(ang2);
}

bool XWPSPath::isMonotonic()
{
	const XWPSSegment *pseg = (const XWPSSegment *)(segments->contents.subpath_first);
  XWPSFixedPoint pt0;
  while (pseg)
  {
  	switch (pseg->type)
  	{
  		case s_start:
  			{
  				const XWPSSubpath *psub = (const XWPSSubpath *)pseg;
  				if (!psub->curve_count)
						pseg = psub->last;
  			}
  			break;
  			
  		case s_curve:
				{
		    	const XWPSCurveSegment *pc = (const XWPSCurveSegment *)pseg;
		    	double t[2];
		    	int nz = XWPSCurveSegment::monotonicPoints(pt0.y, pc->p1.y, pc->p2.y, pc->pt.y, t);

		    	if (nz != 0)
						return false;
		    	nz = XWPSCurveSegment::monotonicPoints(pt0.x, pc->p1.x, pc->p2.x, pc->pt.x, t);
		    	if (nz != 0)
						return false;
				}
				break;
				
	    default:
				;
  	}
  	
  	pt0 = pseg->pt;
		pseg = pseg->next;
  }
  
  return true;
}

bool XWPSPath::isNull()
{
	return (isVoid() && !positionValid());
}

bool XWPSPath::isRectangle(XWPSFixedRect * pbox)
{
	return (isRectangular(pbox) != prt_none);
}

PSPathRectangularType XWPSPath::isRectangular(XWPSFixedRect * pbox)
{
	const XWPSSubpath *pnext;
	if (subpath_count == 1)
		return segments->contents.subpath_first->isRectangular(pbox, &pnext);
		
	return prt_none;
}

int XWPSPath::monotonizeInternal(XWPSCurveSegment * pc)
{
	long x0 = position.x, y0 = position.y;
  PSSegmentNotes notes = (PSSegmentNotes)(pc->notes);
  double t[2];

#define max_segs 9
  XWPSCurveSegment cs[max_segs];
  XWPSCurveSegment *pcs;
  XWPSCurveSegment *pcd;
  int i, j, nseg;
  int nz;
  
  nz = XWPSCurveSegment::monotonicPoints(y0, pc->p1.y, pc->p2.y, pc->pt.y, t);
  nseg = max_segs - 1 - nz;
  pcd = cs + nseg;
  if (nz == 0)
		*pcd = *pc;
  else 
  {
		pc->split(x0, y0, t[0], pcd, pcd + 1);
		if (nz == 2)
	    (pcd + 1)->split(pcd->pt.x, pcd->pt.y, (t[1] - t[0]) / (1 - t[0]), pcd + 1, pcd + 2);
  }
  
  for (pcs = pcd, pcd = cs, j = nseg; j < max_segs; ++pcs, ++j) 
  {
		nz = XWPSCurveSegment::monotonicPoints(x0, pcs->p1.x, pcs->p2.x, pcs->pt.x, t);

		if (nz == 0)
	    *pcd = *pcs;
		else 
		{
	    pcs->split(x0, y0, t[0], pcd, pcd + 1);
	    if (nz == 2)
				(pcd + 1)->split(pcd->pt.x, pcd->pt.y, (t[1] - t[0]) / (1 - t[0]), pcd + 1, pcd + 2);
		}
		pcd += nz + 1;
		x0 = pcd[-1].pt.x;
		y0 = pcd[-1].pt.y;
  }
  nseg = pcd - cs;
  for (pcs = cs, i = 0; i < nseg; ++pcs, ++i) 
  {
		int code = addCurveNotes(pcs->p1.x, pcs->p1.y,
					   								 pcs->p2.x, pcs->p2.y,
					                   pcs->pt.x, pcs->pt.y,
					                   (PSSegmentNotes)(notes | (i > 0 ? sn_not_first : sn_none)));

		if (code < 0)
	    return code;
  }

  return 0;
}

int XWPSPath::newPath()
{
	clear();
	if (!segments)
		segments = new XWPSPathSegments;
	initContents();
	return 0;
}

int XWPSPath::newSubpath()
{
	int code = unshare();
	if (code < 0)
		return code;
		
	XWPSSubpath *psub = segments->contents.subpath_current;
  XWPSSubpath *spp = new XWPSSubpath;
  spp->last = spp;
  spp->pt = position;
  if (!psub)
  	segments->contents.subpath_first = spp;
  else
  {
  	XWPSSegment *prev = psub->last;
		prev->next = (XWPSSegment *) spp;
		spp->prev = prev;
  }
  
  segments->contents.subpath_current = spp;
  subpath_count++;
  return 0;
}

bool XWPSPath::outsideBbox(long px, long py)
{
	return (px < bbox.p.x || px > bbox.q.x ||py < bbox.p.y || py > bbox.q.y);
}

int  XWPSPath::popCloseNotes(PSSegmentNotes notesA)
{
	XWPSSubpath *psub = currentSubpath();
  XWPSSegment *pseg;
  XWPSSegment *prev;

  if (psub == 0 || (pseg = psub->last) == 0 || pseg->type != s_line)
		return (int)(XWPSError::Unknown);
	
  prev = pseg->prev;
  prev->next = 0;
  psub->last = prev;
  delete pseg;
  return closeSubpathNotes(notesA);
}

bool XWPSPath::positionValid()
{
	return ((state_flags & psf_position_valid) != 0);
}

bool XWPSPath::positionInRange()
{
	return ((state_flags & (psf_position_valid + psf_outside_range)) == psf_position_valid);
}

int  XWPSPath::scaleExp2Shared(int log2_scale_x, int log2_scale_y,  bool segments_shared)
{
	XWPSSegment *pseg;

  bbox.scaleExp2(log2_scale_x, log2_scale_y);
#define SCALE_XY(pt) pt.scaleExp2(log2_scale_x, log2_scale_y)

  SCALE_XY(position);
  if (!segments_shared) 
  {
		for (pseg = (firstSubpath()); pseg != 0;  pseg = pseg->next)
	    switch (pseg->type) 
	    {
	    	case s_curve:
					SCALE_XY(((XWPSCurveSegment *)pseg)->p1);
					SCALE_XY(((XWPSCurveSegment *)pseg)->p2);
					
	    	default:
					SCALE_XY(pseg->pt);
	    }
  }
#undef SCALE_XY
  return 0;
}

int XWPSPath::translate(long dx, long dy)
{
	XWPSSegment *pseg;

#define update_xy(pt)\
  pt.x += dx, pt.y += dy
  
  if (box_last != 0) 
  {
		update_xy(bbox.p);
		update_xy(bbox.q);
  }
  if (positionValid())
		update_xy(position);
		
 for (pseg = firstSubpath(); pseg != 0; pseg = pseg->next)
		switch (pseg->type) 
		{
	    case s_curve:
#define pcseg ((XWPSCurveSegment *)pseg)
		update_xy(pcseg->p1);
		update_xy(pcseg->p2);
#undef pcseg
	    default:
				update_xy(pseg->pt);
		}
#undef update_xy
  return 0;
}

void XWPSPath::unclose(int count)
{
	XWPSSubpath *psub;

  for (psub = segments->contents.subpath_first; count != 0; psub = (XWPSSubpath *) psub->last->next)
		if (psub->last == (XWPSSegment *) & psub->closer) 
		{
	   	XWPSSegment *prev = psub->closer.prev, *next = psub->closer.next;
	   	prev->next = next;
	   	if (next)
				next->prev = prev;
	   	psub->last = prev;
	   	count--;
		}
}

int XWPSPath::unshare()
{
	int code = 0;

  if (isShared())
		code = allocCopy();
  return code;
}

void XWPSPath::updateMoveTo()
{
	state_flags = start_flags = psf_last_moveto;
}

int XWPSPath::allocCopy()
{
	XWPSPath path_new;
  int code;
  
  code = path_new.copy(this);
  if (code < 0)
  	return code;
  	
  assign(&path_new, false);
  return code;
}

void XWPSPath::clear()
{
	if (segments == &local_segments)
	{
		if (local_segments.decRef() == 0)
			local_segments.clear();
		else
		{
			local_segments.contents.subpath_first = 0;
			local_segments.contents.subpath_current = 0;
		}
	}
	else
	{
		if (segments)
			if (!segments->decRef())
				delete segments;
				
		segments = 0;
	}
}

void XWPSPath::copyContents(const XWPSPath * shared)
{
	bbox = shared->bbox;
	box_last = shared->box_last;
	start_flags = shared->start_flags;
	state_flags = shared->state_flags;
	bbox_set = shared->bbox_set;
	_pad = shared->_pad;
	subpath_count = shared->subpath_count;
	curve_count = shared->curve_count;
	position = shared->position;
	outside_position = shared->outside_position;
	outside_start = shared->outside_start;
}

void XWPSPath::initContents()
{
	box_last = 0;
	segments->contents.subpath_first = 0;
	segments->contents.subpath_current = 0;
  subpath_count = 0;
  curve_count = 0;
  start_flags = 0;
  state_flags = psf_last_newpath;
  bbox_set = 0;
  _pad = 0;
  subpath_count = 0;
  curve_count = 0;
}

int XWPSPath::subpathExpandDashes(XWPSSubpath * psub, 
	                                XWPSImagerState * pis, 
	                                XWPSDashParams * dash)
{
	float *pattern = dash->pattern;
  int count, index;
  bool ink_on;
  double elt_length;
  long x0 = psub->pt.x, y0 = psub->pt.y;
  long x, y;
  const XWPSSegment *pseg;
  int wrap = (dash->init_ink_on && psub->is_closed ? -1 : 0);
  int drawing = wrap;
  PSSegmentNotes notes = (PSSegmentNotes)(~sn_not_first);
  int code;

  if ((code = addPoint(x0, y0)) < 0)
		return code;
		
top:
	count = dash->pattern_size;
  ink_on = dash->init_ink_on;
  index = dash->init_index;
  elt_length = dash->init_dist_left;
  x = x0, y = y0;
  pseg = psub;
  while ((pseg = pseg->next) != 0 && pseg->type != s_start) 
  {
		long sx = pseg->pt.x, sy = pseg->pt.y;
		long udx = sx - x, udy = sy - y;
		double length, dx, dy;
		double scale = 1;
		double left;

		if (!(udx | udy))
	    dx = 0, dy = 0, length = 0;
		else 
		{
	    XWPSPoint d;

	    dx = udx, dy = udy;
	    pis->idtransform(dx, dy, &d);
	    length = hypot(d.x, d.y) * (1.0 / fixed_1);
	    if (pis->currentDashAdapt()) 
	    {
				double reps = length / dash->pattern_length;

				scale = reps / ceil(reps);
				count = dash->pattern_size;
				ink_on = dash->init_ink_on;
				index = dash->init_index;
				elt_length = dash->init_dist_left * scale;
	    }
		}
		left = length;
		while (left > elt_length) 
		{
	    double fraction = elt_length / length;
	    long nx = x + (long) (dx * fraction);
	    long ny = y + (long) (dy * fraction);

	    if (ink_on) 
	    {
				if (drawing >= 0)
		    	code = addLineNotes(nx, ny, (PSSegmentNotes)(notes & pseg->notes));
				notes = (PSSegmentNotes)(notes | sn_not_first);
	    } 
	    else 
	    {
				if (drawing > 0)
		    	return 0;
				code = addPoint(nx, ny);
				notes = (PSSegmentNotes)(notes & ~sn_not_first);
				drawing = 0;
	    }
	    
	    if (code < 0)
				return code;
	    left -= elt_length;
	    ink_on = !ink_on;
	    if (++index == count)
				index = 0;
	    elt_length = pattern[index] * scale;
	    x = nx, y = ny;
		}
		elt_length -= left;
		
on:
		if (ink_on) 
		{
	    if (drawing >= 0) 
	    {
				code = (pseg->type == s_line_close && drawing > 0 ?  closeSubpathNotes((PSSegmentNotes)(notes & pseg->notes)) :
		     			                 addLineNotes(sx, sy, (PSSegmentNotes)(notes & pseg->notes)));
				notes = (PSSegmentNotes)(notes | sn_not_first);
	    }
		} 
		else 
		{
	    code = addPoint(sx, sy);
	    notes = (PSSegmentNotes)(notes & ~sn_not_first);
	    if (elt_length < fixed2float(fixed_epsilon) &&
			(pseg->next == 0 || pseg->next->type == s_start)) 
			{
				if (code < 0)
		    	return code;
				elt_length = 0;
				ink_on = true;
				if (++index == count)
		    	index = 0;
				elt_length = pattern[index] * scale;
				goto on;
	  	}
	  	if (drawing > 0)
				return code;
	  	drawing = 0;
		}
		if (code < 0)
	    return code;
		x = sx, y = sy;
  }
  
  if (wrap && drawing <= 0) 
  {
		drawing = 1;
		goto top;
  }
  return 0;
}

XWPSPathEnum::XWPSPathEnum()
	:XWPSStruct()
{
	pseg = 0;
	path = 0;
	copied_path = 0;
	moveto_done = false;
	notes = sn_none;
}


XWPSPathEnum::~XWPSPathEnum()
{
	if (copied_path)
	{
		delete copied_path;
		copied_path = 0;
	}
}

int XWPSPathEnum::copyInit(XWPSState * pgs, bool copy)
{
	if (copy) 
	{
		copied_path = new XWPSPath(0);	
		init(copied_path);
		copied_path->copy(pgs->path);
  } 
  else 
		init(pgs->path);
  pgs->currentMatrix(&mat);
  return 0;
}

int XWPSPathEnum::getLength()
{
	return sizeof(XWPSPathEnum);
}

const char * XWPSPathEnum::getTypeName()
{
	return "pathenum";
}

int XWPSPathEnum::init(XWPSPath * ppath)
{
	path = ppath;
  copied_path = 0;	/* not copied */
  pseg = ppath->segments->contents.subpath_first;
  moveto_done = false;
  notes = sn_none;
  return 0;
}

int XWPSPathEnum::next(XWPSFixedPoint * ppts)
{
	XWPSSegment * psegA = pseg;
	if (psegA == 0) 
	{
		XWPSPath *ppath = path;

		if (ppath->lastIsMoveTo() && !moveto_done) 
		{
	    moveto_done = true;
	    notes = sn_none;
	    ppts[0] = ppath->position;
	    return ps_pe_moveto;
		}
		return 0;
  }
  pseg = psegA->next;
  notes = (PSSegmentNotes)(psegA->notes);
  switch (psegA->type) 
  {
		case s_start:
	    ppts[0] = psegA->pt;
	    return ps_pe_moveto;
	    
		case s_line:
	    ppts[0] = psegA->pt;
	    return ps_pe_lineto;
	    
		case s_line_close:
	    ppts[0] = psegA->pt;
	    return ps_pe_closepath;
	    
		case s_curve:
#define pcseg ((const XWPSCurveSegment *)psegA)
	    ppts[0] = pcseg->p1;
	    ppts[1] = pcseg->p2;
	    ppts[2] = psegA->pt;
	    return ps_pe_curveto;
#undef pcseg
		default:
	    return (int)(XWPSError::Fatal);
  }
}

int XWPSPathEnum::next(XWPSPoint ppts[3])
{
	XWPSFixedPoint fpts[3];
  int pe_op = next(fpts);
  int code;

  switch (pe_op) 
  {
		case 0:		/* all done */
		case ps_pe_closepath:
	    break;
	    
		case ps_pe_curveto:
	    if ((code = ppts[1].transformInverse(fixed2float(fpts[1].x), fixed2float(fpts[1].y), &mat)) < 0 ||
					(code = ppts[2].transformInverse(fixed2float(fpts[2].x), fixed2float(fpts[2].y),	&mat)) < 0)
			return code;
	    /* falls through */
		case ps_pe_moveto:
		case ps_pe_lineto:
	    if ((code = ppts[0].transformInverse(fixed2float(fpts[0].x), fixed2float(fpts[0].y),	&mat)) < 0)
				return code;
		default:		/* error */
	    break;
   }
   return pe_op;
}

XWPSClipRect::XWPSClipRect()
{
	init();
	to_visit =  0;
}

XWPSClipRect::~XWPSClipRect()
{
}

void XWPSClipRect::init()
{
	next = 0;
	prev = 0;
	ymin = min_int;
	ymax = max_int;
	xmin = 0;
	xmax = 0;
}

XWPSClipList::XWPSClipList()
{
	head = 0;
	tail = 0;
	xmin = 0;
	xmax = 0;
	count = 0;
}

XWPSClipList::~XWPSClipList()
{
	free();
}

void XWPSClipList::free()
{
	XWPSClipRect * rp = tail;
	while (rp != 0)
	{
		XWPSClipRect *prev = rp->prev;
		delete rp;
		rp = prev;
	}
	init();
}

void XWPSClipList::fromRectangle(XWPSFixedRect * rp)
{
	init();
	if (rp->p.x > rp->q.x) 
	{
		long t = rp->p.x;
		rp->p.x = rp->q.x;
		rp->q.x = t;
  }
  if (rp->p.y > rp->q.y) 
  {
		long t = rp->p.y;
		rp->p.y = rp->q.y;
		rp->q.y = t;
  }
  
  single.xmin = xmin = fixed2int_var(rp->p.x);
  single.ymin = fixed2int_var(rp->p.y);
  single.xmax = xmax = (rp->q.x == rp->p.x ? single.xmin : fixed2int_var_ceiling(rp->q.x));
  single.ymax =	(rp->q.y == rp->p.y ? single.ymin :	 fixed2int_var_ceiling(rp->q.y));
  count = 1;
}

void XWPSClipList::init()
{
	single.init();
	head = 0;
	tail = 0;
	xmin = 0;
	xmax = 0;
	count = 0;
}

XWPSClipRectList::XWPSClipRectList()
	:XWPSStruct()
{
	list = new XWPSClipList;
}

XWPSClipRectList::~XWPSClipRectList()
{
	if (list)
	{
		delete list;
		list = 0;
	}
}

void XWPSClipRectList::fromRectangle(XWPSFixedRect * rp)
{
	list->fromRectangle(rp);
}

int XWPSClipRectList::getLength()
{
	return sizeof(XWPSClipRectList);
}

const char * XWPSClipRectList::getTypeName()
{
	return "cliprectlist";
}

XWPSClipPath::XWPSClipPath()
{
	rule = 0;
	rect_list = &local_list;
	path_valid = false;
	id = 0;
	initContents();
}

XWPSClipPath::XWPSClipPath(XWPSClipPath * shared)
{
	if (shared)
	{
		path.assign(&(shared->path));
		if (shared->rect_list == &(shared->local_list))
		{
			rect_list = new XWPSClipRectList;
			*(rect_list->list) = *(shared->rect_list->list);
			shared->local_list.incRef();
		}
		else
		{
			rect_list = shared->rect_list;
			shared->rect_list->incRef();
		}
		
		shareContents(shared);
	}
	else
	{
		rule = 0;
		path_valid = false;
		id = 0;
		rect_list = new XWPSClipRectList;
		initContents();
	}
}

XWPSClipPath::~XWPSClipPath()
{
	clear();
}

int XWPSClipPath::assign(XWPSClipPath * shared, bool preserve)
{
	if (shared->rect_list == rect_list)
		return 0;
		
	clear();
	path.assign(&(shared->path));
	shareContents(shared);
	if (shared->rect_list == &(shared->local_list))
	{
		rect_list = new XWPSClipRectList;
		*(rect_list->list) = *(shared->rect_list->list);
		shared->local_list.incRef();
	}
	else
	{
		rect_list = shared->rect_list;	
		shared->rect_list->incRef();
	}
	if (!preserve)
		shared->clear();
		
	return 0;
}

int XWPSClipPath::fromRectangle(XWPSFixedRect * pbox)
{
	path.newPath();
	setRectangle(pbox); 
	return 0;
}

bool XWPSClipPath::innerBox(XWPSFixedRect * pbox)
{
	*pbox = inner_box;
	return rect_list->isRectangle();
}

bool XWPSClipPath::includesRectangle(ulong x0, ulong y0, ulong x1, ulong y1)
{
	return	(x0 <= x1 ? (inner_box.p.x <= x0 && x1 <= inner_box.q.x) :
	         (inner_box.p.x <= x1 && x0 <= inner_box.q.x)) &&
	         (y0 <= y1 ? (inner_box.p.y <= y0 && y1 <= inner_box.q.y) :
	         (inner_box.p.y <= y1 && y0 <= inner_box.q.y));
}

void XWPSClipPath::initOwnContents()
{
	XWPSFixedRect null_rect;

  null_rect.p.x = null_rect.p.y = null_rect.q.x = null_rect.q.y = 0;
  initRectangle(&null_rect);
}

int  XWPSClipPath::intersect(XWPSPath *ppath_orig,  int rule, XWPSImagerState *pis)
{
	XWPSPath fpath;
  XWPSPath *ppath = ppath_orig;
  XWPSFixedRect old_box, new_box;
  int code;
  
  if (ppath->hasCurves()) 
  {
		code = fpath.addFlattenedAccurate(ppath, pis->currentFlat(),  pis->accurate_curves);
		if (code < 0)
	    return code;
		ppath = &fpath;
  }
  
  if (innerBox(&old_box) &&
			((code = ppath->isRectangle(&new_box)) || ppath->isVoid()))
	{
		int changed = 0;

		if (!code)
		{
			if (ppath->currentPoint(&new_box.p) < 0) 
			{
				new_box.p.x = float2fixed(pis->ctm.tx);
				new_box.p.y = float2fixed(pis->ctm.ty);
	    }
	    new_box.q = new_box.p;
	    changed = 1;
		}
		else
		{
			if (old_box.p.x > new_box.p.x)
				new_box.p.x = old_box.p.x, ++changed;
	    if (old_box.p.y > new_box.p.y)
				new_box.p.y = old_box.p.y, ++changed;
	    if (old_box.q.x < new_box.q.x)
				new_box.q.x = old_box.q.x, ++changed;
	    if (old_box.q.y < new_box.q.y)
				new_box.q.y = old_box.q.y, ++changed;
	    if (new_box.q.x < new_box.p.x || new_box.q.y < new_box.p.y)
				new_box.p = new_box.q, changed = 1;
		}
		
		if (changed == 4) 
	    return 0;
		
		path.newPath();
		ppath->bbox = new_box;
		setRectangle(&new_box);
		if (changed == 0)
		{
			path.assign(ppath);
	    path_valid = true;
		}
	}
	else
	{
		bool path_validA =  innerBox(&old_box) &&
	    									ppath->getBbox(&new_box) >= 0 &&
	    									includesRectangle(new_box.p.x, new_box.p.y,	new_box.q.x, new_box.q.y);

		code = intersectPathSlow(ppath, rule, pis);
		if (code >= 0 && path_validA) 
		{
	    path.assign(ppath_orig);
	    path_valid = true;
		}
	}
	
  return code;
}

int  XWPSClipPath::intersectPathSlow(XWPSPath * ppath,
			     							 int rule, 
			     							 XWPSImagerState *pis)
{
	uint save_lop = pis->currentLogicalOp();
	XWPSDeviceCPathAccum adev;
  XWPSDeviceColor devc;
  XWPSFillParams params;
  int code;
  
  adev.context_state = pis->i_ctx_p;
  adev.begin();
  devc.setPure(0);
  pis->setLogicalOp(lop_default);
  params.rule = rule;
  params.adjust.x = params.adjust.y = fixed_half;
  params.flatness = pis->currentFlat();
  params.fill_zero_width = true;
  code = adev.fillPath(pis, ppath,  &params, &devc, this);
  if (code < 0 || (code = adev.end(this)) < 0)
		;
  pis->setLogicalOp(save_lop);
  return code;
}

bool XWPSClipPath::outerBox(XWPSFixedRect * pbox)
{
	*pbox = outer_box;
  return rect_list->isRectangle();
}

int XWPSClipPath::reset()
{
	XWPSFixedRect null_rect;

  return fromRectangle(&null_rect);
}

int XWPSClipPath::scaleExp2Shared(int log2_scale_x, int log2_scale_y, bool list_shared, bool segments_shared)
{
	int code =	(path_valid ? path.scaleExp2Shared(log2_scale_x, log2_scale_y, segments_shared) : 0);
  XWPSClipList *list = rect_list->list;
  XWPSClipRect *pr;

  if (code < 0)
		return code;
		
  inner_box.scaleExp2(log2_scale_x, log2_scale_y);
  outer_box.scaleExp2(log2_scale_x, log2_scale_y);
  if (!list_shared) 
  {
		pr = list->head;
		if (pr == 0)
	    pr = &list->single;
		for (; pr != 0; pr = pr->next)
	    if (pr != list->head && pr != list->tail) 
	    {

#define SCALE_V(v, s)\
  			if ( pr->v != min_int && pr->v != max_int )\
    			pr->v = (s >= 0 ? pr->v << s : pr->v >> -s)

				SCALE_V(xmin, log2_scale_x);
				SCALE_V(xmax, log2_scale_x);
				SCALE_V(ymin, log2_scale_y);
				SCALE_V(ymax, log2_scale_y);
#undef SCALE_V
	    }
  }
  return 0;
}

void XWPSClipPath::setOuterBox()
{
	outer_box.p.x = fixed_floor(path.bbox.p.x);
  outer_box.p.y = fixed_floor(path.bbox.p.y);
  outer_box.q.x = fixed_ceiling(path.bbox.q.x);
  outer_box.q.y = fixed_ceiling(path.bbox.q.y);
}

int XWPSClipPath::toPath(XWPSPath * ppath)
{
	if (!path_valid)
	{
		XWPSCPathEnum cenum;
		XWPSFixedPoint pts[3];
		XWPSPath rpath;
		int code;
		
		cenum.init(this);
		while ((code = cenum.next(pts)) != 0)
		{
			switch (code)
			{
				case ps_pe_moveto:
		    	code = rpath.addPoint(pts[0].x, pts[0].y);
		    	break;
		    	
				case ps_pe_lineto:
		    	code = rpath.addLineNotes(pts[0].x, pts[0].y, sn_none);
		    	break;
		    	
				case ps_pe_curveto:
		    	code = rpath.addCurveNotes(pts[0].x, pts[0].y, pts[1].x, pts[1].y, pts[2].x, pts[2].y, sn_none);
		    	break;
		    	
				case ps_pe_closepath:
		    	code = rpath.closeSubpathNotes(sn_none);
		    	break;
		    	
				default:
		    	if (code >= 0)
						code = XWPSError::Unregistered;
			}
			
			if (code < 0)
				break;
		}
		
		if (code >= 0)
	    code = path.assign(&rpath, false);
	    
	  path_valid = true;
	}
	
	return ppath->assign(&path);
}

int XWPSClipPath::unshare()
{
	int code = path.unshare();
	if (code < 0)
		return code;
		
	clear();
	return 0;
}

void XWPSClipPath::clear()
{
	if (rect_list != &local_list)
	{
		if (rect_list)
			if (!rect_list->decRef())
				delete rect_list;
			
		rect_list = 0;
	}
	else
	{
		if (local_list.decRef() > 0)
			local_list.list->init();
	}
}

void XWPSClipPath::initContents()
{
	XWPSFixedRect null_rect;
	initRectangle(&null_rect);
}

void XWPSClipPath::initRectangle(XWPSFixedRect * pbox)
{
	rect_list->fromRectangle(pbox);
  inner_box = *pbox;
  path_valid = false;
  path.bbox = *pbox;
  setOuterBox();
}

void XWPSClipPath::shareContents(const XWPSClipPath * shared)
{
	rect_list = shared->rect_list;
	rule = shared->rule;
	inner_box = shared->inner_box;
  path_valid = shared->path_valid;
  outer_box = shared->outer_box;
  id = shared->id;
}

int XWPSClipPath::setRectangle(XWPSFixedRect * pbox)
{
	clear();
	if (!rect_list)
		rect_list = new XWPSClipRectList;
	initRectangle(pbox);
	return 0;
}

XWPSClipStack::XWPSClipStack()
	:XWPSStruct()
{
	clip_path = 0;
	next = 0;
}

XWPSClipStack::~XWPSClipStack()
{
	if (clip_path)
	{
		delete clip_path;
		clip_path = 0;
	}
}


int XWPSClipStack::getLength()
{
	return sizeof(XWPSClipStack);
}

const char * XWPSClipStack::getTypeName()
{
	return "clipstack";
}


XWPSCPathEnum::XWPSCPathEnum()
{
	using_path = false;
	visit = 0;
	rp = 0;
	first_visit = visit_left;
	state = cpe_scan;
	have_line = false;
	any_rectangles = false;
}

XWPSCPathEnum::~XWPSCPathEnum()
{
}

int XWPSCPathEnum::init(XWPSClipPath * pcpath)
{
	if ((using_path = pcpath->path_valid)) 
	{
		path_enum.init(&pcpath->path);
		rp = visit = 0;
  } 
  else 
  {
		XWPSPath empty_path;
		XWPSClipList *clp = pcpath->listPrivate();
		XWPSClipRect *head = (clp->count <= 1 ? &clp->single : clp->head);
		XWPSClipRect *rpA;

		path_enum.init(&empty_path);
		visit = head;
		for (rpA = head; rpA != 0; rpA = rpA->next)
	    rpA->to_visit = (rpA->xmin < rpA->xmax && rpA->ymin < rpA->ymax ? visit_left | visit_right : 0);
		rp = 0;
		any_rectangles = false;
		state = cpe_scan;
		have_line = false;
  }
  return 0;
}

int XWPSCPathEnum::next(XWPSFixedPoint * pts)
{
	if (using_path)
		return path_enum.next(pts);
		
#define set_pt(xi, yi)\
  (pts[0].x = int2fixed(xi), pts[0].y = int2fixed(yi))
#define set_line(xi, yi)\
  (line_end.x = (xi), line_end.y = (yi), have_line = true)
  
  if (have_line) 
  {
		set_pt(line_end.x, line_end.y);
		have_line = false;
		return ps_pe_lineto;
  }
  
  XWPSClipRect *visitA = visit;
	XWPSClipRect *rpA = rp;
	PSCPEVisit first_visitA = first_visit;
	PSCPEState stateA = state;
	XWPSClipRect *look;
	int code;
	
	switch (stateA)
	{
		case cpe_scan:
			for (; visitA != 0; visitA = visitA->next) 
			{
		    if (visitA->to_visit & visit_left) 
		    {
					set_pt(visitA->xmin, visitA->ymin);
					first_visitA = visit_left;
					stateA = cpe_left;
		    } 
		    else if (visitA->to_visit & visit_right) 
		    {
					set_pt(visitA->xmax, visitA->ymax);
					first_visitA = visit_right;
					stateA = cpe_right;
		    } 
		    else
					continue;
		    rpA = visitA;
		    code = ps_pe_moveto;
		    any_rectangles = true;
		    goto out;
			}
			stateA = cpe_done;
			if (!any_rectangles) 
			{
		    set_pt(fixed_0, fixed_0);
		    code = ps_pe_moveto;
		    break;
			}
			
		case cpe_done:
			code = 0;
			break;
			
#define return_line(px, py)\
  set_pt(px, py); code = ps_pe_lineto; break
  
  	case cpe_left:
  		
left:
			rpA->to_visit &= ~visit_left;
			for (look = rpA; 
			     (look = look->next) != 0 && 
			      (look->ymin == rpA->ymin || 
			       (look->ymin == rpA->ymax && 
			        look->xmax <= rpA->xmin));)
			  ;
			  
			if (look == 0 || look->ymin > rpA->ymax || look->xmin >= rpA->xmax)
			{
				stateA = (rpA == visitA && first_visitA == visit_right ? 
				               cpe_close : (set_line(rpA->xmax, rpA->ymax), cpe_right));
		    return_line(rpA->xmin, rpA->ymax);
			}
			
			{
		    XWPSClipRect *prev = rpA->prev;
		    XWPSClipRect *cur = rpA;

		    if (prev != 0 && prev->ymax == rpA->ymax && look->xmin < prev->xmax) 
		    {
					rpA = prev;
					stateA = (rpA == visitA && first_visitA == visit_right ? 
								cpe_close : (set_line(prev->xmax, prev->ymax), cpe_right));
					return_line(cur->xmin, cur->ymax);
		    }
		    rpA = look;
		    if (rpA == visitA && first_visitA == visit_left)
					stateA = cpe_close;
		    else if (rpA->xmin == cur->xmin)
					goto left;
		    else
					set_line(rpA->xmin, rpA->ymin);
		    return_line(cur->xmin, cur->ymax);
			}
			
		case cpe_right:
			
right:
			rpA->to_visit &= ~visit_right;
			for (look = rpA; 
			   (look = look->prev) != 0 && 
			    (look->ymax == rpA->ymax || 
			     (look->ymax == rpA->ymin && 
			      look->xmin >= rpA->xmax));)
			  ;
			  
			if (look == 0 || look->ymax < rpA->ymin || look->xmax <= rpA->xmin)
			{
				stateA = (rpA == visitA && first_visitA == visit_left ? 
						cpe_close : (set_line(rpA->xmin, rpA->ymin), cpe_left));
		    return_line(rpA->xmax, rpA->ymin);
			}
			
			{
		    XWPSClipRect *next = rpA->next;
		    XWPSClipRect *cur = rpA;

		    if (next != 0 && next->ymin == rpA->ymin &&	look->xmax > next->xmin) 
		    {
					rpA = next;
					stateA = (rpA == visitA && first_visitA == visit_left ? 
					       cpe_close : (set_line(next->xmin, next->ymin), cpe_left));
					return_line(cur->xmax, cur->ymin);
		    }
		    rpA = look;
		    if (rpA == visitA && first_visitA == visit_right)
					stateA = cpe_close;
		    else if (rpA->xmax == cur->xmax)
					goto right;
		    else
					set_line(rpA->xmax, rpA->ymax);
		    return_line(cur->xmax, cur->ymin);
			}

#undef return_line
		
		case cpe_close:
			code = ps_pe_closepath;
			stateA = cpe_scan;
			break;

	  default:
			return (int)(XWPSError::Unknown);
	}
	
out:
	visit = visitA;
	rp = rpA;
	first_visit = first_visitA;
	state = stateA;
	return code;
	
#undef set_pt
#undef set_line
}
