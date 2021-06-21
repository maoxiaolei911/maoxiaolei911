/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWGRAPHICSSTATE_H
#define XWGRAPHICSSTATE_H

#include "XWPattern.h"
#include "XWPath.h"

#define BLEND_NORMAL    0
#define BLEND_MULTIPLY  1
#define BLEND_SCREEN    2
#define BLEND_OVERLAY   3
#define BLEND_DARKEN    4
#define BLEND_LIGHTEN   5
#define BLEND_COLORDODGE 6
#define BLEND_COLORBURN  7
#define BLEND_HARDLIGHT  8
#define BLEND_SOFTLIGHT  9
#define BLEND_DIFFERENCE 10
#define BLEND_EXCLUSION  11
#define BLEND_HUE        12
#define BLEND_SATURATION 13
#define BLEND_COLOR      14
#define BLEND_LUMINOSITY 15

class XW_CORE_EXPORT XWGraphixState
{
public:
    XWGraphixState(double hDPIA, 
                   double vDPIA, 
                   XWPDFRectangle *pageBox,
	               int rotateA, 
	               bool upsideDown);
    ~XWGraphixState();
        
    void  clearPath();
    void  clip();
    void clipToRect(double xMin, double yMin, double xMax, double yMax);
    void  clipToStrokePath();
    void  closePath()
    { path->close(); curX = path->getLastX(); curY = path->getLastY(); }
    void  concatCTM(double a, double b, double c, double d, double e, double f);
    XWGraphixState * copy(bool copypath = false) { return new XWGraphixState(this, copypath); }
    void  curveTo(double x1, double y1, 
                  double x2, double y2,
	              double x3, double y3)
	{ path->curveTo(x1, y1, x2, y2, curX = x3, curY = y3); }
                      
        
    int     getBlendMode() { return blendMode; }
    double  getCharSpace() { return charSpace; }
    void    getClipBBox(double *xMin, double *yMin, double *xMax, double *yMax)
                { *xMin = clipXMin; *yMin = clipYMin; *xMax = clipXMax; *yMax = clipYMax; }
    double   getCurX() { return curX; }
    double   getCurY() { return curY; }
    double * getCTM() { return ctm; }
    void    getFillCMYK(CoreCMYK *cmyk) { fillColorSpace->getCMYK(&fillColor, cmyk); }
    CoreColor  * getFillColor() { return &fillColor; }
    XWColorSpace * getFillColorSpace() { return fillColorSpace; }
    void    getFillGray(CoreGray *gray) { fillColorSpace->getGray(&fillColor, gray); }
    double  getFillOpacity() { return fillOpacity; }
    bool    getFillOverprint() { return fillOverprint; }
    XWPattern * getFillPattern() { return fillPattern; }
    void    getFillRGB(CoreRGB *rgb) { fillColorSpace->getRGB(&fillColor, rgb); }
    int     getFlatness() { return flatness; }
    XWCoreFont * getFont() { return font; }
    double  getFontSize() { return fontSize; }
    void    getFontTransMat(double *m11, double *m12, double *m21, double *m22);
    double  getHDPI() { return hDPI; }
    double  getHorizScaling() { return horizScaling; }
    double  getLeading() { return leading; }
    int     getLineCap() { return lineCap; }
    void    getLineDash(double **dash, int *length, double *start)
               { *dash = lineDash; *length = lineDashLength; *start = lineDashStart; }
    int     getLineJoin() { return lineJoin; }
    double  getLineWidth() { return lineWidth; }
    double  getLineX() { return lineX; }
    double  getLineY() { return lineY; }
    double  getMiterLimit() { return miterLimit; }
    int     getOverprintMode() { return overprintMode; }
    double  getPageWidth() { return pageWidth; }
    double  getPageHeight() { return pageHeight; }
    XWPath * getPath() { return path; }
    int     getRender() { return render; }
    double  getRise() { return rise; }
    int     getRotate() { return rotate; }
    bool    getStrokeAdjust() { return strokeAdjust; }
    void    getStrokeCMYK(CoreCMYK *cmyk) { strokeColorSpace->getCMYK(&strokeColor, cmyk); }
    CoreColor  * getStrokeColor() { return &strokeColor; }
    XWColorSpace * getStrokeColorSpace() { return strokeColorSpace; }
    void    getStrokeGray(CoreGray *gray) { strokeColorSpace->getGray(&strokeColor, gray); }
    double  getStrokeOpacity() { return strokeOpacity; }
    bool    getStrokeOverprint() { return strokeOverprint; }
    XWPattern * getStrokePattern() { return strokePattern; }
    void    getStrokeRGB(CoreRGB *rgb) { strokeColorSpace->getRGB(&strokeColor, rgb); }
    double *getTextMat() { return textMat; }
    XWFunction ** getTransfer() { return transfer; }
    double  getTransformedFontSize();
    double  getTransformedLineWidth() { return transformWidth(lineWidth); }
    void    getUserClipBBox(double *xMin, double *yMin, double *xMax, double *yMax);
    double  getVDPI() { return vDPI; }
    double  getWordSpace() { return wordSpace; }
    double  getX1() { return px1; }
    double  getX2() { return px2; }
    double  getY1() { return py1; }    
    double  getY2() { return py2; }
    
    bool hasSaves() { return saved != 0; }
    
    bool isCurPt() { return path->isCurPt(); }
    bool isPath() { return path->isPath(); }
    
    void lineTo(double x, double y)
    { path->lineTo(curX = x, curY = y); }
    
    void moveTo(double x, double y)
    { path->moveTo(curX = x, curY = y); }
        
    bool parseBlendMode(XWObject *obj, int *mode);
    
    XWGraphixState * restore();
    
    XWGraphixState *save();
    void  setBlendMode(int mode) { blendMode = mode; }
    void  setCharSpace(double space) { charSpace = space; }
    void  setCTM(double a, double b, double c, double d, double e, double f);
    void  setFillColor(CoreColor *color) { fillColor = *color; }
    void  setFillColorSpace(XWColorSpace *colorSpace);
    void  setFillOpacity(double opac) { fillOpacity = opac; }
    void  setFillOverprint(bool op) { fillOverprint = op; }
    void  setFillPattern(XWPattern *pattern);
    void  setFlatness(int flatness1) { flatness = flatness1; }
    void  setFont(XWCoreFont *fontA, double fontSizeA) { font = fontA; fontSize = fontSizeA; }
    void  setHorizScaling(double scale) { horizScaling = 0.01 * scale; }
    void  setLeading(double leadingA) { leading = leadingA; }
    void  setLineCap(int capstyle) {lineCap = capstyle;}
    void  setLineDash(double *dash, int length, double start);
    void  setLineJoin(int joinstyle) {lineJoin = joinstyle;}
    void  setLineWidth(double width) {lineWidth = width;}
    void  setMiterLimit(double mlimit) {miterLimit = mlimit;}
    void  setOverprintMode(int opm) { overprintMode = opm; }
    void  setPath(XWPath *pathA);
    void  setRender(int renderA) { render = renderA; }
    void  setRise(double riseA) { rise = riseA; }
    void  setStrokeAdjust(bool sa) { strokeAdjust = sa; }
    void  setStrokeColor(CoreColor *color) { strokeColor = *color; }
    void  setStrokeColorSpace(XWColorSpace *colorSpace);
    void  setStrokeOpacity(double opac) { strokeOpacity = opac; }
    void  setStrokeOverprint(bool op) { strokeOverprint = op; }
    void  setStrokePattern(XWPattern *pattern);
    void  setTextMat(double a, double b, double c,
		            double d, double e, double f)
                    { textMat[0] = a; textMat[1] = b; textMat[2] = c;
                      textMat[3] = d; textMat[4] = e; textMat[5] = f; }
    void  setTransfer(XWFunction **funcs);
    void  setWordSpace(double space) { wordSpace = space; }
    void  shift(double dx, double dy);
    void  shiftCTM(double tx, double ty);
    
    void textMoveTo(double tx, double ty)
        { lineX = tx; lineY = ty; textTransform(tx, ty, &curX, &curY); }
    void textSetPos(double tx, double ty) { lineX = tx; lineY = ty; }
    void textShift(double tx, double ty);
    void textTransform(double x1, double y1, double *x2, double *y2)
            { *x2 = textMat[0] * x1 + textMat[2] * y1 + textMat[4];
      *y2 = textMat[1] * x1 + textMat[3] * y1 + textMat[5]; }
    void textTransformDelta(double x1, double y1, double *x2, double *y2)
            { *x2 = textMat[0] * x1 + textMat[2] * y1;
      *y2 = textMat[1] * x1 + textMat[3] * y1; }
    void transform(double x1, double y1, double *x2, double *y2)
    				{ *x2 = ctm[0] * x1 + ctm[2] * y1 + ctm[4];
     				 *y2 = ctm[1] * x1 + ctm[3] * y1 + ctm[5]; }
    void transformDelta(double x1, double y1, double *x2, double *y2)
            { *x2 = ctm[0] * x1 + ctm[2] * y1;
      *y2 = ctm[1] * x1 + ctm[3] * y1; }
    double transformWidth(double w);
    
public:
    double hDPI, vDPI;
    double ctm[6];
    double px1, py1, px2, py2;
    double pageWidth, pageHeight;
    int rotate;
    
    XWColorSpace * fillColorSpace;
    XWColorSpace * strokeColorSpace;
    CoreColor    fillColor;
    CoreColor    strokeColor;
    XWPattern * fillPattern;
    XWPattern * strokePattern;    
    int blendMode;
    
    double fillOpacity;
    double strokeOpacity;
    bool fillOverprint;
    bool strokeOverprint;
    int overprintMode;
    
    XWFunction * transfer[4];
    
    double   lineWidth;
    double * lineDash;	
    int lineDashLength;
    double lineDashStart;
    int flatness;
    int lineJoin;
    int lineCap;
    double miterLimit;
    bool strokeAdjust;
    
    XWCoreFont * font;
    double fontSize;
    
    double textMat[6];
    double charSpace;
    double wordSpace;
    double horizScaling;
    double leading;
    double rise;
    int    render;
    XWPath * path;
    double curX, curY;
    double lineX, lineY;
    double clipXMin, clipYMin, clipXMax, clipYMax;
    
    XWGraphixState *saved;
    
private:
    XWGraphixState(XWGraphixState *state, bool copypath);
};


#endif // XWGRAPHICSSTATE_H

