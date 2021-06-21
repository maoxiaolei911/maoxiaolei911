/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWRASTER_H
#define XWRASTER_H

#include "XWGlobal.h"
#include "XWRasterType.h"

class XWBitmap;
class XWRasterState;
class XWRasterPattern;
struct ScreenParams;
class XWScreen;
class XWRasterPath;
class XWRasterXPath;
class XWRasterFont;
class XWRasterClip;
struct RasterPipe;

extern int colorModeNComps[];


class XW_RASTER_EXPORT XWRaster
{
public:
    XWRaster(XWBitmap *bitmapA, bool vectorAntialiasA, ScreenParams *screenParams = 0);
    XWRaster(XWBitmap *bitmapA, bool vectorAntialiasA, XWScreen *screenA);
    ~XWRaster();
    
    bool blitTransparent(XWBitmap *src, int xSrc, int ySrc, int xDest, int yDest, int w, int h);
			      
	void clear(uchar * color, uchar alpha = 0x00);
    void clearModRegion();
    void clipResetToRect(double x0, 
                         double y0,
		                 double x1, 
		                 double y1);
    bool clipToPath(XWRasterPath *path, bool eo);
    bool clipToRect(double x0, 
                    double y0,
		            double x1, 
		            double y1);
    bool composite(XWBitmap *src, 
                   int xSrc, 
                   int ySrc,
			       int xDest, 
			       int yDest, 
			       int w, 
			       int h,
			       bool noClip, 
			       bool nonIsolated);
    void compositeBackground(uchar * color);
    
    bool drawImage(XWRasterImageSource src, 
                   void *srcData,
			       int srcMode, 
			       bool srcAlpha,
			       int w, 
			       int h, 
			       double *mat);
			       
	bool fill(XWRasterPath *path, bool eo);
	bool fillChar(double x, 
	              double y,
			      int c, 
			      XWRasterFont *font);
	bool fillGlyph(double x, double y, GlyphBitmap *glyph);
	bool fillImageMask(XWRasterImageMaskSource src, 
	                    void *srcData,
			            int w, int h, 
			            double *mat,
			            bool glyphMode);
    
    XWBitmap  *  getBitmap() { return bitmap; }
    XWRasterBlendFunc getBlendFunc();
    XWRasterClip * getClip();
    int         getClipRes() { return opClipRes; }
    double      getFillAlpha();
    XWRasterPattern * getFillPattern();
    double      getFlatness();
    bool        getInNonIsolatedGroup();
    int         getLineCap();
    double    * getLineDash();
    int         getLineDashLength();
    double      getLineDashPhase();
    int         getLineJoin();
    double      getLineWidth();
    double *    getMatrix();
    double      getMiterLimit();
    void        getModRegion(int *xMin, int *yMin, int *xMax, int *yMax)
                  { *xMin = modXMin; *yMin = modYMin; *xMax = modXMax; *yMax = modYMax; }
    XWScreen  * getScreen();
    XWBitmap  * getSoftMask();
    bool        getStrokeAdjust();
    double      getStrokeAlpha();
    XWRasterPattern * getStrokePattern();
    bool getVectorAntialias() { return vectorAntialias; }
    
    XWRasterPath * makeStrokePath(XWRasterPath *path, 
                                  double w,
				                          bool flatten = true);
    
    void restoreState();
    
    void saveState();
    void setBlendFunc(XWRasterBlendFunc func);
    void setDebugMode(bool debugModeA) { debugMode = debugModeA; }
    void setFillAlpha(double alpha);
    void setFillPattern(XWRasterPattern *fillPattern);
    void setFlatness(double flatness);
    void setInNonIsolatedGroup(XWBitmap *alpha0BitmapA, int alpha0XA, int alpha0YA);
    void setInShading(bool sh) { inShading = sh; }
    void setLineCap(int lineCap);
    void setLineDash(double *lineDash, int lineDashLength, double lineDashPhase);
    void setLineJoin(int lineJoin);
    void setLineWidth(double lineWidth);
    void setMatrix(double * m);
    void setMinLineWidth(double w) { minLineWidth = w; }
    void setMiterLimit(double miterLimit);
    void setOverprintMask(uint overprintMask);
    void setScreen(XWScreen *screen);
    void setSoftMask(XWBitmap *softMask);
    void setStrokeAdjust(bool strokeAdjust);
    void setStrokeAlpha(double alpha);
    void setStrokePattern(XWRasterPattern *strokePattern);
    void setTransfer(uchar *red, uchar *green, uchar *blue, uchar *gray);
    void setVectorAntialias(bool vaa) { vectorAntialias = vaa; }
    bool stroke(XWRasterPath *path);
    
    bool xorFill(XWRasterPath *path, bool eo);
    
private:
	void arbitraryTransformMask(XWRasterImageMaskSource src, void *srcData,
			      									int srcWidth, int srcHeight,
			      									double *mat, bool glyphMode);
		void arbitraryTransformImage(XWRasterImageSource src, void *srcData,
			       										int srcMode, int nComps,
			       										bool srcAlpha,
			       										int srcWidth, int srcHeight,
			       										double *mat);
    void blitImage(XWBitmap *src, bool srcAlpha, int xDest, int yDest, int clipRes);
    void blitImageClipped(XWBitmap *src, bool srcAlpha,
													int xSrc, int ySrc, int xDest, int yDest,
													int w, int h);
    void blitMask(XWBitmap *src, int xDest, int yDest, int clipRes);
    void drawAALine(RasterPipe *pipe, int x0, int x1, int y);
    void drawAAPixel(RasterPipe *pipe, int x, int y);
    void drawAAPixelInit();
    void drawPixel(RasterPipe *pipe, int x, int y, bool noClip);
    void drawSpan(RasterPipe *pipe, int x0, int x1, int y, bool noClip);
    
    bool fillGlyph2(int x0, int y0, GlyphBitmap *glyph);
    bool fillWithPattern(XWRasterPath *path, bool eo, XWRasterPattern *pattern, double alpha);
    void flattenCurve(double x0, 
                      double y0,
			          double x1, 
			          double y1,
			          double x2, 
			          double y2,
			          double x3, 
			          double y3,
			          double *matrix, 
		              double flatness2,
		              XWRasterPath *fPath);
    XWRasterPath *flattenPath(XWRasterPath *path, double *matrix, double flatness);
    
    XWRasterPath * makeDashedPath(XWRasterPath *path);
    
    bool pathAllOutside(XWRasterPath *path);
    void pipeIncX(RasterPipe *pipe);
    void pipeInit(RasterPipe *pipe, int x, int y,
		          XWRasterPattern *pattern, uchar * cSrc,
		          double aInput, bool usesShape,
		          bool nonIsolatedGroup);
    void pipeRun(RasterPipe *pipe);
    void pipeRunSimpleMono1(RasterPipe *pipe);
    void pipeRunSimpleMono8(RasterPipe *pipe);
    void pipeRunSimpleRGB8(RasterPipe *pipe);
    void pipeRunSimpleBGR8(RasterPipe *pipe);
    void pipeRunSimpleCMYK8(RasterPipe *pipe);
    void pipeRunAAMono1(RasterPipe *pipe);
    void pipeRunAAMono8(RasterPipe *pipe);
    void pipeRunAARGB8(RasterPipe *pipe);
    void pipeRunAABGR8(RasterPipe *pipe);
    void pipeRunAACMYK8(RasterPipe *pipe);
    void pipeSetXY(RasterPipe *pipe, int x, int y);
    
    XWBitmap *scaleImage(XWRasterImageSource src, void *srcData,
			   								int srcMode, int nComps,
			   								bool srcAlpha, int srcWidth, int srcHeight,
			   								int scaledWidth, int scaledHeight);
    void scaleImageYdXd(XWRasterImageSource src, void *srcData,
		      							int srcMode, int nComps,
		      							bool srcAlpha, int srcWidth, int srcHeight,
		      							int scaledWidth, int scaledHeight,
		      							XWBitmap *dest);
    void scaleImageYdXu(XWRasterImageSource src, void *srcData,
		      							int srcMode, int nComps,
		      							bool srcAlpha, int srcWidth, int srcHeight,
		      							int scaledWidth, int scaledHeight,
		      							XWBitmap *dest);
    void scaleImageYuXd(XWRasterImageSource src, void *srcData,
		      							int srcMode, int nComps,
		      							bool srcAlpha, int srcWidth, int srcHeight,
		      							int scaledWidth, int scaledHeight,
		      							XWBitmap *dest);
    void scaleImageYuXu(XWRasterImageSource src, void *srcData,
		      							int srcMode, int nComps,
		      							bool srcAlpha, int srcWidth, int srcHeight,
		      							int scaledWidth, int scaledHeight,
		      							XWBitmap *dest);
    XWBitmap *scaleMask(XWRasterImageMaskSource src, void *srcData,
			  								int srcWidth, int srcHeight,
			  								int scaledWidth, int scaledHeight);
		void scaleMaskYdXd(XWRasterImageMaskSource src, void *srcData,
		     								int srcWidth, int srcHeight,
		     								int scaledWidth, int scaledHeight,
		     								XWBitmap *dest);
		void scaleMaskYdXu(XWRasterImageMaskSource src, void *srcData,
		     							int srcWidth, int srcHeight,
		     							int scaledWidth, int scaledHeight,
		     							XWBitmap *dest);
		void scaleMaskYuXd(XWRasterImageMaskSource src, void *srcData,
		     							int srcWidth, int srcHeight,
		     							int scaledWidth, int scaledHeight,
		     							XWBitmap *dest);
		void scaleMaskYuXu(XWRasterImageMaskSource src, void *srcData,
		     							int srcWidth, int srcHeight,
		     							int scaledWidth, int scaledHeight,
		     							XWBitmap *dest);
    void strokeNarrow(XWRasterPath *path);
    void strokeWide(XWRasterPath *path, double w);
    
    void transform(double *matrix, double xi, double yi, double * xo, double * yo)
         {
            *xo = xi * matrix[0] + yi * matrix[2] + matrix[4];
  			*yo = xi * matrix[1] + yi * matrix[3] + matrix[5];
         }
		 
    void updateModX(int x);
    void updateModY(int y);
    
    void vertFlipImage(XWBitmap *img, int width, int height, int nComps);
    
    void dumpPath(XWRasterPath *path);
    
private:
    static int pipeResultColorNoAlphaBlend[];
    static int pipeResultColorAlphaNoBlend[];
    static int pipeResultColorAlphaBlend[];
    static int pipeNonIsoGroupCorrection[];
  
    XWBitmap *bitmap;
    XWRasterState *state;
    XWBitmap *aaBuf;
    int aaBufY;
    XWBitmap *alpha0Bitmap;
    int alpha0X, alpha0Y;
    double aaGamma[17];
    double minLineWidth;
    int modXMin, modYMin, modXMax, modYMax;
    int opClipRes;
    bool vectorAntialias;
    bool inShading;
    bool debugMode;
};

#endif //XWRASTER_H

