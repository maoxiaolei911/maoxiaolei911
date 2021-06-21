/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPATH_H
#define XWPATH_H

#include "XWGlobal.h"

class XW_CORE_EXPORT XWSubPath
{
public:
	XWSubPath(double x1, double y1);
	~XWSubPath();
    
    void close();
    XWSubPath *copy() { return new XWSubPath(this); }
    void curveTo(double x1, double y1, double x2, double y2,
	             double x3, double y3);
    
    bool getCurve(int i) { return curve[i]; }
    double getLastX() { return x[n-1]; }
  	double getLastY() { return y[n-1]; }
    int getNumPoints() { return n; }
    double getX(int i) { return x[i]; }
  	double getY(int i) { return y[i]; }
  	
  	bool isClosed() { return closed; }
  	
  	void lineTo(double x1, double y1);
  	
  	void offset(double dx, double dy);
    
private:
	XWSubPath(XWSubPath *subpath);
	
private:
    double *x, *y;
    bool *curve;
    int n;
    int size;
    bool closed;
};

class XW_CORE_EXPORT XWPath
{
public:
    XWPath();
    ~XWPath();
    
    void append(XWPath *path);
    
    void close();
    XWPath *copy()
    { return new XWPath(justMoved, firstX, firstY, subpaths, n, size); }
    
    void curveTo(double x1, double y1, double x2, double y2,
	             double x3, double y3);
	       
    double getLastX() { return subpaths[n-1]->getLastX(); }
  	double getLastY() { return subpaths[n-1]->getLastY(); }
    int getNumSubpaths() { return n; }
    XWSubPath * getSubpath(int i) { return subpaths[i]; }
    
    bool isCurPt() { return n > 0 || justMoved; }
    bool isPath() { return n > 0; }
    
    void lineTo(double x, double y);
    
    void moveTo(double x, double y);
    
    void offset(double dx, double dy);
    
private:
	XWPath(bool justMoved1, 
	       double firstX1, 
	       double firstY1,
	       XWSubPath **subpaths1, 
	       int n1, 
	       int size1);
	
private:
    bool justMoved;
    double firstX, firstY;
    XWSubPath **subpaths;
    int n;
    int size;
};

#endif //XWPATH_H

