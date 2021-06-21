/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWRASTEROUTPUTDEV_H
#define XWRASTEROUTPUTDEV_H

#include "XWRasterType.h"
#include "XWOutputDev.h"
#include "XWGraphixState.h"

class XWCore8BitFont;
class XWBitmap;
class XWRaster;
class XWRasterPath;
class XWRasterPattern;
class XWRasterFontEngine;
class XWRasterFont;
class T3FontCache;
struct T3FontCacheTag;
struct T3GlyphStack;
struct RasterTransparencyGroup;

#define rasterOutT3FontCacheSize 8

class XW_CORE_EXPORT XWRasterOutputDev : public XWOutputDev
{
public:   
	  XWRasterOutputDev();
    XWRasterOutputDev(int colorModeA, 
                      int bitmapRowPadA,
		              bool reverseVideoA, 
		              uchar * paperColorA,
		              bool bitmapTopDownA = true,
		              bool allowAntialiasA = true);
    virtual ~XWRasterOutputDev();
    
    virtual void beginTransparencyGroup(XWGraphixState *state, 
                                        double *bbox,
				                        XWColorSpace *blendingColorSpace,
				                        bool isolated, 
				                        bool knockout,
				                        bool forSoftMask);
    virtual bool beginType3Char(XWGraphixState *state, 
                                 double , 
                                 double ,
			                     double , 
			                     double ,
			                     uint code, 
			                     uint *u, 
			                     int uLen);
			       
            void clearModRegion();
    virtual void clearSoftMask(XWGraphixState *);
    virtual void clip(XWGraphixState *state);
    virtual void clipToStrokePath(XWGraphixState *state);
    
    virtual void drawChar(XWGraphixState *state, 
                          double x, 
                          double y,
			              double , 
			              double ,
			              double originX, 
			              double originY,
			              uint code, 
			              int , 
			              uint *, 
			              int );
	virtual void drawImage(XWGraphixState *state, 
	                       XWObject *, 
	                       XWStream *str,
			               int width, 
			               int height, 
			               XWImageColorMap *colorMap,
			               int *maskColors, 
			               bool inlineImg);
	virtual void drawImageMask(XWGraphixState *state, 
	                           XWObject *, 
	                           XWStream *str,
			                   int width, 
			                   int height, 
			                   bool invert,
			                   bool inlineImg);
    virtual void drawMaskedImage(XWGraphixState *state, 
                                 XWObject *ref, 
                                 XWStream *str,
			                     int width, 
			                     int height,
			                     XWImageColorMap *colorMap,
			                     XWStream *maskStr, 
			                     int maskWidth, 
			                     int maskHeight,
			                     bool maskInvert);
	virtual void drawSoftMaskedImage(XWGraphixState *state, 
	                                 XWObject *, 
	                                 XWStream *str,
				                     int width, 
				                     int height,
				                     XWImageColorMap *colorMap,
				                     XWStream *maskStr,
				                     int maskWidth, 
				                     int maskHeight,
				                     XWImageColorMap *maskColorMap);
				   		       
    virtual void endPage();    
    virtual void endTextObject(XWGraphixState *);
    virtual void endTransparencyGroup(XWGraphixState *state);
    virtual void endType3Char(XWGraphixState *state);
    virtual void eoClip(XWGraphixState *state);
    virtual void eoFill(XWGraphixState *state);
    
    virtual void fill(XWGraphixState *state);
    
            XWBitmap * getBitmap() { return bitmap; }
            int        getBitmapHeight();
            int        getBitmapWidth();     
            XWRasterFont *getCurrentFont() { return font; } 
            XWRasterFont *getFont(XWString *name, double *textMatA);
            void       getModRegion(int *xMin, int *yMin, int *xMax, int *yMax); 
            int getNestCount() { return nestCount; }
            XWRaster * getRaster() { return raster; }   
    virtual bool getVectorAntialias();  
    
    virtual bool interpretType3Chars() { return true; }
            bool isReverseVideo() { return reverseVideo; }
    
    virtual void paintTransparencyGroup(XWGraphixState *, double *);
    
    virtual void restoreState(XWGraphixState *);
    
    virtual void saveState(XWGraphixState *);
    void setBitmapUpsideDown(bool f) { bitmapUpsideDown = f; }
            void setFillColor(int r, int g, int b);
    virtual void setInShading(bool sh);
    virtual void setPaperColor(uchar * paperColorA);
    virtual void setReverseVideo(bool reverseVideoA) { reverseVideo = reverseVideoA; }
            void setScreenParam(int t, 
                                int s, 
                                int dr, 
                                double gamma, 
                                double bt, 
                                double wt);
    void setSkipText(bool skipHorizTextA, bool skipRotatedTextA)
    { skipHorizText = skipHorizTextA; skipRotatedText = skipRotatedTextA; }
    virtual void setSoftMask(XWGraphixState *, 
                             double *, 
                             bool alpha,
			                 XWFunction *transferFunc, 
			                 CoreColor *backdropColor);
    virtual void setSoftMaskFromImageMask(XWGraphixState *state,
																					XWObject *ref, 
																					XWStream *str,
																					int width, 
																					int height, 
																					bool invert,
																					bool inlineImg);
    virtual void setVectorAntialias(bool vaa);
            void startDoc(XWRef *xrefA);
    virtual void startPage(int , XWGraphixState *state);
    virtual void stroke(XWGraphixState *state);
    
            XWBitmap *takeBitmap();
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
    virtual void type3D0(XWGraphixState *, double, double);
    virtual void type3D1(XWGraphixState *state, double wx, double wy,
		                 double llx, double lly, double urx, double ury);
    
    virtual void updateAll(XWGraphixState *state);
    virtual void updateBlendMode(XWGraphixState *state);
    virtual void updateCTM(XWGraphixState *state, double , double ,
			               double , double , double , double );
    virtual void updateFillColor(XWGraphixState *state);
    virtual void updateFillOpacity(XWGraphixState *state);
    virtual void updateFlatness(XWGraphixState *state);
    virtual void updateFont(XWGraphixState *);
    virtual void updateLineCap(XWGraphixState *state);
    virtual void updateLineDash(XWGraphixState *state);
    virtual void updateLineJoin(XWGraphixState *state);
    virtual void updateLineWidth(XWGraphixState *state);
    virtual void updateMiterLimit(XWGraphixState *state);
    virtual void updateStrokeAdjust(XWGraphixState *);
    virtual void updateStrokeColor(XWGraphixState *state);
    virtual void updateStrokeOpacity(XWGraphixState *state);
    virtual void updateTransfer(XWGraphixState *state);
    virtual bool upsideDown() { return bitmapTopDown ^ bitmapUpsideDown; }
    virtual bool useDrawChar() { return true; }
    virtual bool useTilingPatternFill() { return true; }
    
private:
    static bool alphaImageSrc(void *data, uchar * colorLine, uchar *alphaLine);
			     
	XWRasterPath *convertPath(XWGraphixState *, XWPath *path, bool dropEmptySubpaths);
	
    void doUpdateFont(XWGraphixState *state);
    void drawType3Glyph(XWGraphixState *state, 
                        T3FontCache *t3Font,
				                T3FontCacheTag *tag, 
				                uchar *data);
    
    XWRasterPattern *getColor(CoreGray gray);
    XWRasterPattern *getColor(CoreRGB *rgb);
    XWRasterPattern *getColor(CoreCMYK *cmyk);
    
    static bool imageMaskSrc(void *data, uchar * line);
    static bool imageSrc(void *data, uchar * colorLine, uchar *);
    
    static bool maskedImageSrc(void *data, uchar * colorLine, uchar *alphaLine);
			      
		void setOverprintMask(XWColorSpace *colorSpace, 
		                      bool overprintFlag,
			                    int overprintMode, 
			                    CoreColor *singleColor);
    void setupScreenParams(double hDPI, double vDPI);
    
protected:
    int colorMode;
    int bitmapRowPad;
    bool bitmapTopDown;
    bool bitmapUpsideDown;
    bool allowAntialias;
    bool vectorAntialias;
    bool reverseVideo;
    uchar paperColor[4];
    ScreenParams screenParams;
    bool skipHorizText;
  	bool skipRotatedText;
    
    XWRef *xref;
    
    XWBitmap *bitmap;
    XWRaster *raster;
    XWRasterFontEngine *fontEngine;
    
    T3FontCache * t3FontCache[rasterOutT3FontCacheSize];
    
    int nT3Fonts;
    T3GlyphStack *t3GlyphStack;
    XWRasterFont *font;
    bool haveT3Dx;
    
    bool needFontUpdate;
    
    XWRasterPath *textClipPath;
    RasterTransparencyGroup * transpGroupStack;
    int nestCount;
};

#endif // XWRASTEROUTPUTDEV_H

