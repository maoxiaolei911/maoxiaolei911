/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include "XWPath.h"

XWSubPath::XWSubPath(double x1, double y1)
{
	size = 16;
  	x = (double *)malloc(size * sizeof(double));
  	y = (double *)malloc(size * sizeof(double));
  	curve = (bool *)malloc(size * sizeof(bool));
  	n = 1;
  	x[0] = x1;
  	y[0] = y1;
  	curve[0] = false;
  	closed = false;
}

XWSubPath::XWSubPath(XWSubPath *subpath)
{
	size = subpath->size;
  	n = subpath->n;
  	x = (double *)malloc(size * sizeof(double));
  	y = (double *)malloc(size * sizeof(double));
  	curve = (bool *)malloc(size * sizeof(bool));
  	memcpy(x, subpath->x, n * sizeof(double));
  	memcpy(y, subpath->y, n * sizeof(double));
  	memcpy(curve, subpath->curve, n * sizeof(bool));
  	closed = subpath->closed;
}

XWSubPath::~XWSubPath()
{
	if (x)
		free(x);
		
	if (y)
  		free(y);
  		
  	if (curve)
  		free(curve);
}

void XWSubPath::close()
{
	if (x[n-1] != x[0] || y[n-1] != y[0]) {
    lineTo(x[0], y[0]);
  }
  closed = true;
}

void XWSubPath::curveTo(double x1, 
	                    double y1, 
	                    double x2, 
	                    double y2,
	                    double x3, 
	                    double y3)
{
	if (n+3 > size) 
	{
    	size *= 2;
    	x = (double *)realloc(x, size * sizeof(double));
    	y = (double *)realloc(y, size * sizeof(double));
    	curve = (bool *)realloc(curve, size * sizeof(bool));
  	}
  	
  	x[n] = x1;
  y[n] = y1;
  x[n+1] = x2;
  y[n+1] = y2;
  x[n+2] = x3;
  y[n+2] = y3;
  curve[n] = curve[n+1] = true;
  curve[n+2] = false;
  n += 3;
}

void XWSubPath::lineTo(double x1, double y1)
{
	if (n >= size) 
	{
    	size *= 2;
    	x = (double *)realloc(x, size * sizeof(double));
    	y = (double *)realloc(y, size * sizeof(double));
    	curve = (bool *)realloc(curve, size * sizeof(bool));
  	}
  	
  	x[n] = x1;
  	y[n] = y1;
  	curve[n] = false;
  	++n;
}

void XWSubPath::offset(double dx, double dy)
{
	for (int i = 0; i < n; ++i) 
	{
    	x[i] += dx;
    	y[i] += dy;
  	}
}


XWPath::XWPath()
{
	justMoved = false;
  	size = 16;
  	n = 0;
  	firstX = firstY = 0;
  	subpaths = (XWSubPath **)malloc(size * sizeof(XWSubPath *));
}

XWPath::XWPath(bool justMoved1, 
	           double firstX1, 
	           double firstY1,
	           XWSubPath **subpaths1, 
	           int n1, 
	           int size1)
{
	justMoved = justMoved1;
  	firstX = firstX1;
  	firstY = firstY1;
  	size = size1;
  	n = n1;
  	subpaths = (XWSubPath **)malloc(size * sizeof(XWSubPath *));
  	for (int i = 0; i < n; ++i)
    	subpaths[i] = subpaths1[i]->copy();
}

XWPath::~XWPath()
{
	if (subpaths)
	{
		for (int i = 0; i < n; ++i)
    		delete subpaths[i];
  		free(subpaths);
  	}
}

void XWPath::append(XWPath *path)
{
	int i;

  if (n + path->n > size) {
    size = n + path->n;
    subpaths = (XWSubPath **)realloc(subpaths, size * sizeof(XWSubPath *));
  }
  for (i = 0; i < path->n; ++i) {
    subpaths[n++] = path->subpaths[i]->copy();
  }
  justMoved = false;
}

void XWPath::close()
{
	// this is necessary to handle the pathological case of
  // moveto/closepath/clip, which defines an empty clipping region
  if (justMoved) {
    if (n >= size) {
      size *= 2;
      subpaths = (XWSubPath **)realloc(subpaths, size * sizeof(XWSubPath *));
    }
    subpaths[n] = new XWSubPath(firstX, firstY);
    ++n;
    justMoved = false;
  }
  subpaths[n-1]->close();
}

void XWPath::curveTo(double x1, 
	                 double y1, 
	                 double x2, 
	                 double y2,
	                 double x3, 
	                 double y3)
{
	if (justMoved || (n > 0 && subpaths[n-1]->isClosed())) {
    if (n >= size) {
      size *= 2;
      subpaths = (XWSubPath **)realloc(subpaths, size * sizeof(XWSubPath *));
    }
    if (justMoved) {
      subpaths[n] = new XWSubPath(firstX, firstY);
    } else {
      subpaths[n] = new XWSubPath(subpaths[n-1]->getLastX(), subpaths[n-1]->getLastY());
    }
    ++n;
    justMoved = false;
  }
  subpaths[n-1]->curveTo(x1, y1, x2, y2, x3, y3);
}

void XWPath::lineTo(double x, double y)
{
	if (justMoved || (n > 0 && subpaths[n-1]->isClosed())) {
    if (n >= size) {
      size *= 2;
      subpaths = (XWSubPath **)realloc(subpaths, size * sizeof(XWSubPath *));
    }
    if (justMoved) {
      subpaths[n] = new XWSubPath(firstX, firstY);
    } else {
      subpaths[n] = new XWSubPath(subpaths[n-1]->getLastX(),
				   subpaths[n-1]->getLastY());
    }
    ++n;
    justMoved = false;
  }
  subpaths[n-1]->lineTo(x, y);
}

void XWPath::moveTo(double x, double y)
{
	justMoved = true;
  	firstX = x;
  	firstY = y;
}

void XWPath::offset(double dx, double dy)
{
	for (int i = 0; i < n; ++i) 
    	subpaths[i]->offset(dx, dy);
}

