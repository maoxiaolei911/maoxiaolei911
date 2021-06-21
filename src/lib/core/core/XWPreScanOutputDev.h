/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPRESCANOUTPUTDEV_H
#define XWPRESCANOUTPUTDEV_H

#include "XWGraphixState.h"
#include "XWOutputDev.h"

class XW_CORE_EXPORT XWPreScanOutputDev : public XWOutputDev
{
public:
    XWPreScanOutputDev();
    virtual ~XWPreScanOutputDev() {}
    
    virtual bool axialShadedFill(XWGraphixState *state, XWAxialShading *shading);
    
    virtual void beginStringOp(XWGraphixState *state);
    virtual void beginTransparencyGroup(XWGraphixState *, 
                                        double *,
				                        XWColorSpace *,
				                        bool , 
				                        bool ,
				                        bool );
    virtual bool beginType3Char(XWGraphixState *, 
                                double , 
                                double ,
			                    double , 
			                    double ,
			                    uint , 
			                    uint *, int);
			                    
    void clearStats();
    
    virtual void clip(XWGraphixState *);
    
    virtual void drawImage(XWGraphixState *state, 
                           XWObject *, 
                           XWStream *str,
			               int width, 
			               int height, 
			               XWImageColorMap *colorMap,
			               int *, 
			               bool inlineImg);
    virtual void drawImageMask(XWGraphixState *state, 
                               XWObject *, 
                               XWStream *str,
			                   int width, 
			                   int height, 
			                   bool ,
			                   bool inlineImg);
    virtual void drawMaskedImage(XWGraphixState *state, 
                                 XWObject *, 
                                 XWStream *,
			                     int , 
			                     int ,
			                     XWImageColorMap *colorMap,
			                     XWStream *, 
			                     int , 
			                     int ,
			                     bool );
    virtual void drawSoftMaskedImage(XWGraphixState *, 
                                     XWObject *, 
                                     XWStream *,
				                     int , 
				                     int ,
				                     XWImageColorMap *colorMap,
				                     XWStream *,
				                     int , 
				                     int ,
				                     XWImageColorMap *);
    
    virtual void endPage();
    virtual void endStringOp(XWGraphixState *);
    virtual void endType3Char(XWGraphixState *);
    virtual void eoClip(XWGraphixState *);
    virtual void eoFill(XWGraphixState *state);
    
    virtual void fill(XWGraphixState *state);
    virtual bool functionShadedFill(XWGraphixState *state, XWFunctionShading *shading);
    
    virtual bool interpretType3Chars() { return true; }
    bool isAllGDI() { return gdi; }
    bool isGray() { return gray; }
    bool isMonochrome() { return mono; }
    
    virtual bool radialShadedFill(XWGraphixState *state, XWRadialShading *shading);
    
    virtual void startPage(int , XWGraphixState *) ;
    virtual void stroke(XWGraphixState *state);
    
    virtual void tilingPatternFill(XWGraphixState *state, 
                                   XWGraphix *gfx, 
                                   XWObject *str,
				                           int paintType, 
				                           XWDict *resDict,
				                           double *mat, 
				                           double *bbox,
				                           int x0, 
				                           int y0, 
				                           int x1, 
				                           int y1,
				                           double xStep, 
				                           double yStep);
        
    virtual bool upsideDown() { return true; }
    virtual bool useDrawChar() { return true; }
    
    bool usesPatternImageMask() { return patternImgMask; }
    virtual bool useShadedFills() { return true; }
    virtual bool useTilingPatternFill() { return true; }
    bool usesTransparency() { return transparency; }
    
private:
  	void check(XWColorSpace *colorSpace, CoreColor *color,
	           double opacity, int blendMode);

private:
  	bool mono;
  	bool gray;
  	bool transparency;
  	bool patternImgMask;
  	bool gdi;
};

#endif // XWPRESCANOUTPUTDEV_H

