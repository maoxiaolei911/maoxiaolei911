/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPATTERN_H
#define XWPATTERN_H

#include "XWColorSpace.h"

class XWShading;

class XW_CORE_EXPORT XWPattern
{
public:

  XWPattern(int typeA);
  virtual ~XWPattern();

  virtual XWPattern *copy() = 0;

  int getType() { return type; }
  
  static XWPattern *parse(XWObject *obj);

private:
  int type;
};


class XW_CORE_EXPORT XWTilingPattern: public XWPattern 
{
public:  
    virtual ~XWTilingPattern();

    virtual XWPattern *copy();

    double    * getBBox() { return bbox; }
    XWObject  * getContentStream() { return &contentStream; }
    double    * getMatrix() { return matrix; }
    int         getPaintType() { return paintType; }
    XWDict  * getResDict() { return resDict.isDict() ? resDict.getDict() : (XWDict *)NULL; }
    int         getTilingType() { return tilingType; }
    double      getXStep() { return xStep; }
    double      getYStep() { return yStep; }
    
  
    static XWTilingPattern *parse(XWObject *patObj);

private:
    XWTilingPattern(int paintTypeA, 
                       int tilingTypeA,
		               double *bboxA, 
		               double xStepA, 
		               double yStepA,
		               XWObject *resDictA, 
		               double *matrixA,
		               XWObject *contentStreamA);

private:
    int paintType;
    int tilingType;
    double bbox[4];
    double xStep, yStep;
    XWObject  resDict;
    double matrix[6];
    XWObject  contentStream;
};


class XW_CORE_EXPORT XWShadingPattern: public XWPattern 
{
public:
    virtual ~XWShadingPattern();

    virtual XWPattern *copy();

    double *getMatrix() { return matrix; }
    XWShading *getShading() { return shading; }
  
    static XWShadingPattern *parse(XWObject *patObj);

private:
    XWShadingPattern(XWShading *shadingA, double *matrixA);

private:
    XWShading *shading;
    double matrix[6];
};


class XW_CORE_EXPORT XWShading 
{
public:
    XWShading(int typeA);
    XWShading(XWShading *shading);
    virtual ~XWShading();

    virtual XWShading *copy() = 0;

    CoreColor  * getBackground() { return &background; }
    void           getBBox(double *xMinA, double *yMinA, double *xMaxA, double *yMaxA)
                      { *xMinA = xMin; *yMinA = yMin; *xMaxA = xMax; *yMaxA = yMax; }
    XWColorSpace * getColorSpace() { return colorSpace; }
    bool           getHasBackground() { return hasBackground; }
    bool           getHasBBox() { return hasBBox; }
    int            getType() { return type; }
    
    static XWShading *parse(XWObject *obj);

protected:
    bool init(XWDict *dict);

protected:
    int type;
    XWColorSpace *colorSpace;
    CoreColor background;
    bool hasBackground;
    double xMin, yMin, xMax, yMax;
    bool hasBBox;
};


class XW_CORE_EXPORT XWFunctionShading: public XWShading 
{
public:
    XWFunctionShading(double x0A, 
                       double y0A,
		               double x1A, 
		               double y1A,
		               double *matrixA,
		               XWFunction **funcsA, 
		               int nFuncsA);
    XWFunctionShading(XWFunctionShading *shading);
    virtual ~XWFunctionShading();

    virtual XWShading *copy();

    void     getColor(double x, double y, CoreColor *color);
    void     getDomain(double *x0A, double *y0A, double *x1A, double *y1A)
                { *x0A = x0; *y0A = y0; *x1A = x1; *y1A = y1; }
    double * getMatrix() { return matrix; }
    int      getNFuncs() { return nFuncs; }
    XWFunction * getFunc(int i) { return funcs[i]; }
    
    
    static XWFunctionShading *parse(XWDict *dict);

private:
    double x0, y0, x1, y1;
    double matrix[6];
    XWFunction *funcs[COLOR_MAX_COMPS];
    int nFuncs;
};


class XW_CORE_EXPORT XWAxialShading: public XWShading 
{
public:
    XWAxialShading(double x0A, 
                     double y0A,
		             double x1A, 
		             double y1A,
		             double t0A, 
		             double t1A,
		             XWFunction **funcsA, 
		             int nFuncsA,
		             bool extend0A, 
		             bool extend1A);
    XWAxialShading(XWAxialShading *shading);
    virtual ~XWAxialShading();

    virtual XWShading *copy();
    
    void getColor(double t, CoreColor *color);
    void getCoords(double *x0A, double *y0A, double *x1A, double *y1A)
                    { *x0A = x0; *y0A = y0; *x1A = x1; *y1A = y1; }
    double getDomain0() { return t0; }
    double getDomain1() { return t1; }
    bool   getExtend0() { return extend0; }
    bool   getExtend1() { return extend1; }
    XWFunction *getFunc(int i) { return funcs[i]; }
    int    getNFuncs() { return nFuncs; }
  
    static XWAxialShading *parse(XWDict *dict);

private:
    double x0, y0, x1, y1;
    double t0, t1;
    XWFunction *funcs[COLOR_MAX_COMPS];
    int nFuncs;
    bool extend0, extend1;
};


class XW_CORE_EXPORT XWRadialShading: public XWShading 
{
public:
    XWRadialShading(double x0A, 
                      double y0A, 
                      double r0A,
		              double x1A, 
		              double y1A, 
		              double r1A,
		              double t0A, 
		              double t1A,
		              XWFunction **funcsA, 
		              int nFuncsA,
		              bool extend0A, 
		              bool extend1A);
    XWRadialShading(XWRadialShading *shading);
    virtual ~XWRadialShading();

    virtual XWShading *copy();
    
    void getColor(double t, CoreColor *color);
    void getCoords(double *x0A, double *y0A, double *r0A,
		            double *x1A, double *y1A, double *r1A)
		 { *x0A = x0; *y0A = y0; *r0A = r0; *x1A = x1; *y1A = y1; *r1A = r1; }
    double getDomain0() { return t0; }
    double getDomain1() { return t1; }
    bool getExtend0() { return extend0; }
    bool getExtend1() { return extend1; }
    int getNFuncs() { return nFuncs; }
    XWFunction *getFunc(int i) { return funcs[i]; }
    
    static XWRadialShading *parse(XWDict *dict);
    
private:
    double x0, y0, r0, x1, y1, r1;
    double t0, t1;
    XWFunction *funcs[COLOR_MAX_COMPS];
    int nFuncs;
    bool extend0, extend1;
};


struct XW_CORE_EXPORT GouraudVertex 
{
    double x, y;
    CoreColor color;
};


class XW_CORE_EXPORT XWGouraudTriangleShading: public XWShading 
{
public:
    XWGouraudTriangleShading(int typeA,
			                 GouraudVertex *verticesA, 
			                 int nVerticesA,
			                 int (*trianglesA)[3], 
			                 int nTrianglesA,
			                 XWFunction **funcsA, 
			                 int nFuncsA);
    XWGouraudTriangleShading(XWGouraudTriangleShading *shading);
    virtual ~XWGouraudTriangleShading();

    virtual XWShading *copy();
    
    int  getNTriangles() { return nTriangles; }
    void getTriangle(int i, double *x0, double *y0, CoreColor *color0,
		             double *x1, double *y1, CoreColor *color1,
		             double *x2, double *y2, CoreColor *color2);
		   
    static XWGouraudTriangleShading *parse(int typeA, XWDict *dict, XWStream *str);    

private:
    GouraudVertex *vertices;
    int nVertices;
    int (*triangles)[3];
    int nTriangles;
    XWFunction *funcs[COLOR_MAX_COMPS];
    int nFuncs;
};


struct XW_CORE_EXPORT XWPatch 
{
    double x[4][4];
    double y[4][4];
    CoreColor color[2][2];
};


class XW_CORE_EXPORT XWPatchMeshShading: public XWShading 
{
public:
    XWPatchMeshShading(int typeA, 
                          XWPatch *patchesA, 
                          int nPatchesA,
		                  XWFunction **funcsA, 
		                  int nFuncsA);
    XWPatchMeshShading(XWPatchMeshShading *shading);
    virtual ~XWPatchMeshShading();

    virtual XWShading *copy();
    
    int getNPatches() { return nPatches; }
    XWPatch *getPatch(int i) { return &patches[i]; }
    
    static XWPatchMeshShading *parse(int typeA, XWDict *dict, XWStream *str);

private:
    XWPatch *patches;
    int nPatches;
    XWFunction *funcs[COLOR_MAX_COMPS];
    int nFuncs;
};


class XW_CORE_EXPORT XWImageColorMap 
{
public:
    XWImageColorMap(int bitsA, XWObject *decode, XWColorSpace *colorSpaceA);
    ~XWImageColorMap();
    
    XWImageColorMap *copy() { return new XWImageColorMap(this); }

    int    getBits() { return bits; }
    void   getCMYK(quint8 *x, CoreCMYK *cmyk);
    void   getColor(quint8 *x, CoreColor *color);
    XWColorSpace *getColorSpace() { return colorSpace; }
    double getDecodeLow(int i) { return decodeLow[i]; }
    double getDecodeHigh(int i) { return decodeLow[i] + decodeRange[i]; }
    void   getGray(quint8 *x, CoreGray *gray);
    int    getNumPixelComps() { return nComps; }
    void   getRGB(quint8 *x, CoreRGB *rgb);
    
    void getGrayByteLine(quint8 *in, quint8 *out, int n);
  	void getRGBByteLine(quint8 *in, quint8 *out, int n);
  	void getCMYKByteLine(quint8 *in, quint8 *out, int n);
    
    bool isOk() { return ok; }

private:
    XWImageColorMap(XWImageColorMap *colorMap);

    XWColorSpace *colorSpace;
    int bits;	
    int nComps;	
    XWColorSpace *colorSpace2;	
    int nComps2;			
    int *   lookup[COLOR_MAX_COMPS];
    int *   lookup2[COLOR_MAX_COMPS];
    double decodeLow[COLOR_MAX_COMPS];
    double decodeRange[COLOR_MAX_COMPS];
    bool ok;
};

#endif //XWPATTERN_H

