/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "XWRasterType.h"
#include "XWRasterPath.h"
#include "XWRasterXPath.h"


struct XPathPoint 
{
    double x, y;
};

struct XPathAdjust 
{
    int firstPt, lastPt;		          // range of points
    bool vert;			                  // vertical or horizontal hint
    double x0a, x0b, xma, xmb, x1a, x1b;  // hint boundaries
    double x0, x1, xm;	                  // adjusted coordinates
};

static int cmpXPathSegs(const void *arg0, const void *arg1)
{
    XPathSeg *seg0 = (XPathSeg *)arg0;
    XPathSeg *seg1 = (XPathSeg *)arg1;
    double x0, y0, x1, y1;

    if (seg0->flags & XPATH_FLIP) 
    {
        x0 = seg0->x1;
        y0 = seg0->y1;
    } 
    else 
    {
        x0 = seg0->x0;
        y0 = seg0->y0;
    }
    
    if (seg1->flags & XPATH_FLIP) 
    {
        x1 = seg1->x1;
        y1 = seg1->y1;
    } 
    else 
    {
        x1 = seg1->x0;
        y1 = seg1->y0;
    }
    
    if (y0 != y1) 
        return (y0 > y1) ? 1 : -1;
            
    if (x0 != x1) 
        return (x0 > x1) ? 1 : -1;
            
    return 0;
}

XWRasterXPath::XWRasterXPath(XWRasterPath *path, 
                             double * matrix, 
                             double flatness, 
                             bool closeSubpaths)
	:length(0),
	 size(0),
	 segs(0)
{
  PathHint *hint;
  XPathPoint *pts;
  XPathAdjust *adjusts, *adjust;
  double x0, y0, x1, y1, x2, y2, x3, y3, xsp, ysp;
  double adj0, adj1;
  int curSubpath, curSubpathX, i, j;

  // transform the points
  pts = (XPathPoint *)malloc(path->length * sizeof(XPathPoint));
  for (i = 0; i < path->length; ++i) {
    transform(matrix, path->pts[i].x, path->pts[i].y, &pts[i].x, &pts[i].y);
  }

  // set up the stroke adjustment hints
  if (path->hints) {
    adjusts = (XPathAdjust *)malloc(path->hintsLength * sizeof(XPathAdjust));
    for (i = 0; i < path->hintsLength; ++i) {
      hint = &path->hints[i];
      x0 = pts[hint->ctrl0    ].x;    y0 = pts[hint->ctrl0    ].y;
      x1 = pts[hint->ctrl0 + 1].x;    y1 = pts[hint->ctrl0 + 1].y;
      x2 = pts[hint->ctrl1    ].x;    y2 = pts[hint->ctrl1    ].y;
      x3 = pts[hint->ctrl1 + 1].x;    y3 = pts[hint->ctrl1 + 1].y;
      if (x0 == x1 && x2 == x3) {
	adjusts[i].vert = true;
	adj0 = x0;
	adj1 = x2;
      } else if (y0 == y1 && y2 == y3) {
	adjusts[i].vert = false;
	adj0 = y0;
	adj1 = y2;
      } else {
	free(adjusts);
	adjusts = NULL;
	break;
      }
      if (adj0 > adj1) {
	x0 = adj0;
	adj0 = adj1;
	adj1 = x0;
      }
      adjusts[i].x0a = adj0 - 0.01;
      adjusts[i].x0b = adj0 + 0.01;
      adjusts[i].xma = (double)0.5 * (adj0 + adj1) - 0.01;
      adjusts[i].xmb = (double)0.5 * (adj0 + adj1) + 0.01;
      adjusts[i].x1a = adj1 - 0.01;
      adjusts[i].x1b = adj1 + 0.01;
      // rounding both edge coordinates can result in lines of
      // different widths (e.g., adj=10.1, adj1=11.3 --> x0=10, x1=11;
      // adj0=10.4, adj1=11.6 --> x0=10, x1=12), but it has the
      // benefit of making adjacent strokes/fills line up without any
      // gaps between them
      x0 = rasterRound(adj0);
      x1 = rasterRound(adj1);
      if (x1 == x0) {
	x1 = x1 + 1;
      }
      adjusts[i].x0 = (double)x0;
      adjusts[i].x1 = (double)x1 - 0.01;
      adjusts[i].xm = (double)0.5 * (adjusts[i].x0 + adjusts[i].x1);
      adjusts[i].firstPt = hint->firstPt;
      adjusts[i].lastPt = hint->lastPt;
    }

  } else {
    adjusts = NULL;
  }

  // perform stroke adjustment
  if (adjusts) {
    for (i = 0, adjust = adjusts; i < path->hintsLength; ++i, ++adjust) {
      for (j = adjust->firstPt; j <= adjust->lastPt; ++j) {
	strokeAdjust(adjust, &pts[j].x, &pts[j].y);
      }
    }
    if (adjusts)
    free(adjusts);
  }

  segs = NULL;
  length = size = 0;

  x0 = y0 = xsp = ysp = 0; // make gcc happy
  adj0 = adj1 = 0; // make gcc happy
  curSubpath = 0;
  curSubpathX = 0;
  i = 0;
  while (i < path->length) {

    // first point in subpath - skip it
    if (path->flags[i] & PATH_FIRST) {
      x0 = pts[i].x;
      y0 = pts[i].y;
      xsp = x0;
      ysp = y0;
      curSubpath = i;
      curSubpathX = length;
      ++i;

    } else {

      // curve segment
      if (path->flags[i] & PATH_CURVE) {
	x1 = pts[i].x;
	y1 = pts[i].y;
	x2 = pts[i+1].x;
	y2 = pts[i+1].y;
	x3 = pts[i+2].x;
	y3 = pts[i+2].y;
	addCurve(x0, y0, x1, y1, x2, y2, x3, y3,
		 flatness,
		 (path->flags[i-1] & PATH_FIRST),
		 (path->flags[i+2] & PATH_LAST),
		 !closeSubpaths &&
		   (path->flags[i-1] & PATH_FIRST) &&
		   !(path->flags[i-1] & PATH_CLOSED),
		 !closeSubpaths &&
		   (path->flags[i+2] & PATH_LAST) &&
		   !(path->flags[i+2] & PATH_CLOSED));
	x0 = x3;
	y0 = y3;
	i += 3;

      // line segment
      } else {
	x1 = pts[i].x;
	y1 = pts[i].y;
	addSegment(x0, y0, x1, y1);
	x0 = x1;
	y0 = y1;
	++i;
      }

      // close a subpath
      if (closeSubpaths &&
	  (path->flags[i-1] & PATH_LAST) &&
	  (pts[i-1].x != pts[curSubpath].x ||
	   pts[i-1].y != pts[curSubpath].y)) {
	addSegment(x0, y0, xsp, ysp);
      }
    }
  }

	if (pts)
  free(pts);
}

XWRasterXPath::XWRasterXPath(XWRasterXPath * xPath)
	:length(0),
	 size(0),
	 segs(0)
{
  length = xPath->length;
  size = xPath->size;
  segs = (XPathSeg *)malloc(size * sizeof(XPathSeg));
  memcpy(segs, xPath->segs, length * sizeof(XPathSeg));
}

XWRasterXPath::~XWRasterXPath()
{
    if (segs)
		{
       free(segs);
     }
}

void XWRasterXPath::aaScale()
{
	XPathSeg *seg = segs;
    for (int i = 0; i < length; ++i, ++seg)
    {
        seg->x0 *= AA_SIZE;
        seg->y0 *= AA_SIZE;
        seg->x1 *= AA_SIZE;
        seg->y1 *= AA_SIZE;
    }
}

void XWRasterXPath::sort()
{
    qsort(segs, length, sizeof(XPathSeg), &cmpXPathSegs);
}

void XWRasterXPath::addCurve(double x0, 
                       double y0,
		               double x1, 
		               double y1,
		               double x2, 
		               double y2,
		               double x3, 
		               double y3,
		               double flatness,
		               bool , 
		               bool , 
		               bool , 
		               bool )
{
  double cx[MAX_CURVE_SPLITS + 1][3];
  double cy[MAX_CURVE_SPLITS + 1][3];
  int cNext[MAX_CURVE_SPLITS + 1];
  double xl0, xl1, xl2, xr0, xr1, xr2, xr3, xx1, xx2, xh;
  double yl0, yl1, yl2, yr0, yr1, yr2, yr3, yy1, yy2, yh;
  double dx, dy, mx, my, d1, d2, flatness2;
  int p1, p2, p3;

  flatness2 = flatness * flatness;
  
  // initial segment
  p1 = 0;
  p2 = MAX_CURVE_SPLITS;
  cx[p1][0] = x0;  cy[p1][0] = y0;
  cx[p1][1] = x1;  cy[p1][1] = y1;
  cx[p1][2] = x2;  cy[p1][2] = y2;
  cx[p2][0] = x3;  cy[p2][0] = y3;
  cNext[p1] = p2;

  while (p1 < MAX_CURVE_SPLITS) {

    // get the next segment
    xl0 = cx[p1][0];  yl0 = cy[p1][0];
    xx1 = cx[p1][1];  yy1 = cy[p1][1];
    xx2 = cx[p1][2];  yy2 = cy[p1][2];
    p2 = cNext[p1];
    xr3 = cx[p2][0];  yr3 = cy[p2][0];

    // compute the distances from the control points to the
    // midpoint of the straight line (this is a bit of a hack, but
    // it's much faster than computing the actual distances to the
    // line)
    mx = (xl0 + xr3) * 0.5;
    my = (yl0 + yr3) * 0.5;
    
    dx = xx1 - mx;
    dy = yy1 - my;
    d1 = dx*dx + dy*dy;
    dx = xx2 - mx;
    dy = yy2 - my;
    d2 = dx*dx + dy*dy;

    // if the curve is flat enough, or no more subdivisions are
    // allowed, add the straight line segment
    if (p2 - p1 == 1 || (d1 <= flatness2 && d2 <= flatness2)) {
      addSegment(xl0, yl0, xr3, yr3);
      p1 = p2;

    // otherwise, subdivide the curve
    } else {
      xl1 = (xl0 + xx1) * 0.5;
      yl1 = (yl0 + yy1) * 0.5;
      xh = (xx1 + xx2) * 0.5;
      yh = (yy1 + yy2) * 0.5;
      xl2 = (xl1 + xh) * 0.5;
      yl2 = (yl1 + yh) * 0.5;
      xr2 = (xx2 + xr3) * 0.5;
      yr2 = (yy2 + yr3) * 0.5;
      xr1 = (xh + xr2) * 0.5;
      yr1 = (yh + yr2) * 0.5;
      xr0 = (xl2 + xr1) * 0.5;
      yr0 = (yl2 + yr1) * 0.5;
      // add the new subdivision points
      p3 = (p1 + p2) / 2;
      cx[p1][1] = xl1;  cy[p1][1] = yl1;
      cx[p1][2] = xl2;  cy[p1][2] = yl2;
      cNext[p1] = p3;
      cx[p3][0] = xr0;  cy[p3][0] = yr0;
      cx[p3][1] = xr1;  cy[p3][1] = yr1;
      cx[p3][2] = xr2;  cy[p3][2] = yr2;
      cNext[p3] = p2;
    }
  }
}

void XWRasterXPath::addSegment(double x0, 
                         double y0,
			             double x1, 
			             double y1)
{
    grow(1);
    segs[length].x0 = x0;
    segs[length].y0 = y0;
    segs[length].x1 = x1;
    segs[length].y1 = y1;
    segs[length].flags = 0;
    if (y1 == y0) 
    {
        segs[length].dxdy = segs[length].dydx = 0;
        segs[length].flags |= XPATH_HORIZ;
        if (x1 == x0) 
        {
            segs[length].flags |= XPATH_VERT;
        }
    } 
    else if (x1 == x0) 
    {
        segs[length].dxdy = segs[length].dydx = 0;
        segs[length].flags |= XPATH_VERT;
    } 
    else 
    {
        segs[length].dxdy = (x1 - x0) / (y1 - y0);
        segs[length].dydx = (double)1 / segs[length].dxdy;
    }
    
    if (y0 > y1) 
        segs[length].flags |= XPATH_FLIP;
        
    ++length;
}

bool XWRasterXPath::grow(int nSegs)
{
    if (length + nSegs > size) 
    {
        if (size == 0) 
            size = 32;
        
        while (size < length + nSegs) 
            size *= 2;
        segs = (XPathSeg *)realloc(segs, size * sizeof(XPathSeg));
#if defined(XW_DEBUG)
				loSegs = segs;
				hiSegs = segs + size;
#endif
    }
    
    return (segs != 0);
}

void XWRasterXPath::strokeAdjust(XPathAdjust *adjust, double *xp, double *yp)
{
  double x, y;

  if (adjust->vert) {
    x = *xp;
    if (x > adjust->x0a && x < adjust->x0b) {
      *xp = adjust->x0;
    } else if (x > adjust->xma && x < adjust->xmb) {
      *xp = adjust->xm;
    } else if (x > adjust->x1a && x < adjust->x1b) {
      *xp = adjust->x1;
    }
  } else {
    y = *yp;
    if (y > adjust->x0a && y < adjust->x0b) {
      *yp = adjust->x0;
    } else if (y > adjust->xma && y < adjust->xmb) {
      *yp = adjust->xm;
    } else if (y > adjust->x1a && y < adjust->x1b) {
      *yp = adjust->x1;
    }
  }
}
