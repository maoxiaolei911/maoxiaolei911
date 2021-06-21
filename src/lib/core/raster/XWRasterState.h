/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWRASTERSTATE_H
#define XWRASTERSTATE_H

#include "XWGlobal.h"
#include "XWRasterType.h"

class XWRasterPattern;
class XWScreen;
class XWRasterClip;
class XWBitmap;
struct ScreenParams;

class XW_RASTER_EXPORT XWRasterState
{
public:
    XWRasterState(int width, 
                  int height, 
                  bool vectorAntialias, 
                  ScreenParams *screenParams);
    XWRasterState(int width, 
                  int height, 
                  bool vectorAntialias, 
                  XWScreen *screenA);
                  
    XWRasterState(XWRasterState * state);
    ~XWRasterState();
    
    XWRasterState *copy() { return new XWRasterState(this); }
    
    void setFillPattern(XWRasterPattern *fillPatternA);
    void setLineDash(double *lineDashA, int lineDashLengthA, double lineDashPhaseA);
    void setScreen(XWScreen *screenA);
    void setSoftMask(XWBitmap *softMaskA);
    void setStrokePattern(XWRasterPattern *strokePatternA);
    void setTransfer(uchar *red, uchar *green, uchar *blue, uchar *gray);
    
private:
    double matrix[6];
    XWRasterPattern * strokePattern;
    XWRasterPattern * fillPattern;
    XWScreen  * screen;
    XWRasterBlendFunc blendFunc;
    double strokeAlpha;
    double fillAlpha;
    double lineWidth;
    int lineCap;
    int lineJoin;
    double miterLimit;
    double flatness;
    double *lineDash;
    int lineDashLength;
    double lineDashPhase;
    bool strokeAdjust;
    XWRasterClip *clip;
    XWBitmap *softMask;
    bool deleteSoftMask;
    bool inNonIsolatedGroup;
    
    uchar rgbTransferR[256],
         	rgbTransferG[256],
         	rgbTransferB[256];
  	uchar grayTransfer[256];
  	uchar cmykTransferC[256],
         	cmykTransferM[256],
         	cmykTransferY[256],
         	cmykTransferK[256];
  	uint overprintMask;
    
    XWRasterState * next;
    
    friend class XWRaster;
};


#endif // XWRASTERSTATE_H

