/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <math.h>
#include <QtDebug>
#include "XWApplication.h"
#include "XWFontSetting.h"
#include "XWDocSetting.h"
#include "XWObject.h"
#include "XWGraphix.h"
#include "XWCoreFont.h"
#include "XWLink.h"
#include "BuiltinFont.h"
#include "BuiltinFontTables.h"
#include "XWCharCodeToUnicode.h"
#include "FontEncodingTables.h"
#include "XWFontFileTrueType.h"
#include "XWBitmap.h"
#include "XWRasterPattern.h"
#include "XWScreen.h"
#include "XWRasterPath.h"
#include "XWRasterState.h"
#include "XWRasterFontEngine.h"
#include "XWRasterFont.h"
#include "XWRasterFontFile.h"
#include "XWFontFileID.h"
#include "XWRaster.h"
#include "XWObject.h"
#include "XWRasterOutputDev.h"

#define type3FontCacheAssoc   8
#define type3FontCacheMaxSets 8
#define type3FontCacheSize    (128*1024)

static void rasterOutBlendMultiply(uchar * src, 
                                   uchar *  dest,
				                   uchar *  blend, 
				                   int cm) 
{
    for (int i = 0; i < colorModeNComps[cm]; ++i) 
        blend[i] = (dest[i] * src[i]) / 255;
}


static void rasterOutBlendScreen(uchar *  src, 
                                 uchar *  dest,
				                 uchar *  blend, 
				                 int cm) 
{
    for (int i = 0; i < colorModeNComps[cm]; ++i) 
        blend[i] = dest[i] + src[i] - (dest[i] * src[i]) / 255;
}


static void rasterOutBlendOverlay(uchar * src, 
                                  uchar * dest,
				                  uchar * blend, 
				                  int cm) 
{
    for (int i = 0; i < colorModeNComps[cm]; ++i) 
    {
        blend[i] = dest[i] < 0x80
                 ? (src[i] * 2 * dest[i]) / 255
                 : 255 - 2 * ((255 - src[i]) * (255 - dest[i])) / 255;
    }
}


static void rasterOutBlendDarken(uchar * src, 
                                 uchar * dest,
				                 uchar * blend, 
				                 int cm) 
{
    for (int i = 0; i < colorModeNComps[cm]; ++i) 
        blend[i] = dest[i] < src[i] ? dest[i] : src[i];
}

static void rasterOutBlendLighten(uchar * src, 
                                  uchar * dest,
				                  uchar * blend, 
				                  int cm) 
{
    for (int i = 0; i < colorModeNComps[cm]; ++i) 
        blend[i] = dest[i] > src[i] ? dest[i] : src[i];
}

static void rasterOutBlendColorDodge(uchar * src, 
                                      uchar * dest,
				                      uchar * blend,
				                      int cm) 
{
    for (int i = 0; i < colorModeNComps[cm]; ++i) 
    {
        if (src[i] == 255) 
            blend[i] = 255;
        else 
        {
            int x = (dest[i] * 255) / (255 - src[i]);
            blend[i] = x <= 255 ? x : 255;
        }
    }
}

static void rasterOutBlendColorBurn(uchar * src, 
                                    uchar * dest,
				                    uchar * blend, 
				                    int cm) 
{
    for (int i = 0; i < colorModeNComps[cm]; ++i) 
    {
        if (src[i] == 0) 
            blend[i] = 0;
        else 
        {
            int x = ((255 - dest[i]) * 255) / src[i];
            blend[i] = x <= 255 ? 255 - x : 0;
        }
    }
}

static void rasterOutBlendHardLight(uchar * src, 
                                    uchar * dest,
				                    uchar * blend, 
				                    int cm) 
{
    for (int i = 0; i < colorModeNComps[cm]; ++i) 
    {
        blend[i] = src[i] < 0x80
                 ? (dest[i] * 2 * src[i]) / 255
                 : 255 - 2 * ((255 - dest[i]) * (255 - src[i])) / 255;
    }
}

static void rasterOutBlendSoftLight(uchar * src, 
                                    uchar * dest,
				                    uchar * blend, 
				                    int cm) 
{
    int x = 0;
    for (int i = 0; i < colorModeNComps[cm]; ++i) 
    {
      if (src[i] < 0x80) {
      blend[i] = dest[i] - (255 - 2 * src[i]) * dest[i] * (255 - dest[i]) /
	         (255 * 255);
    } else {
      if (dest[i] < 0x40) {
	x = (((((16 * dest[i] - 12 * 255) * dest[i]) / 255)
	      + 4 * 255) * dest[i]) / 255;
      } else {
	x = (int)sqrt(255.0 * dest[i]);
      }
      blend[i] = dest[i] + (2 * src[i] - 255) * (x - dest[i]) / 255;
    }
    }
}

static void rasterOutBlendDifference(uchar *  src, 
                                     uchar *  dest,
				                     uchar *  blend,
				                     int cm) 
{
    for (int i = 0; i < colorModeNComps[cm]; ++i) 
        blend[i] = dest[i] < src[i] ? src[i] - dest[i] : dest[i] - src[i];
}

static void rasterOutBlendExclusion(uchar * src, 
                                    uchar * dest,
				                    uchar * blend, 
				                    int cm) 
{
    for (int i = 0; i < colorModeNComps[cm]; ++i) 
        blend[i] = dest[i] + src[i] - (2 * dest[i] * src[i]) / 255;
}

static int getLum(int r, int g, int b) {
  return (int)(0.3 * r + 0.59 * g + 0.11 * b);
}

static int getSat(int r, int g, int b) {
  int rgbMin, rgbMax;

  rgbMin = rgbMax = r;
  if (g < rgbMin) {
    rgbMin = g;
  } else if (g > rgbMax) {
    rgbMax = g;
  }
  if (b < rgbMin) {
    rgbMin = b;
  } else if (b > rgbMax) {
    rgbMax = b;
  }
  return rgbMax - rgbMin;
}

static void clipColor(int rIn, int gIn, int bIn,
		      uchar *rOut, uchar *gOut, uchar *bOut) {
  int lum, rgbMin, rgbMax;

  lum = getLum(rIn, gIn, bIn);
  rgbMin = rgbMax = rIn;
  if (gIn < rgbMin) {
    rgbMin = gIn;
  } else if (gIn > rgbMax) {
    rgbMax = gIn;
  }
  if (bIn < rgbMin) {
    rgbMin = bIn;
  } else if (bIn > rgbMax) {
    rgbMax = bIn;
  }
  if (rgbMin < 0) {
    *rOut = (uchar)(lum + ((rIn - lum) * lum) / (lum - rgbMin));
    *gOut = (uchar)(lum + ((gIn - lum) * lum) / (lum - rgbMin));
    *bOut = (uchar)(lum + ((bIn - lum) * lum) / (lum - rgbMin));
  } else if (rgbMax > 255) {
    *rOut = (uchar)(lum + ((rIn - lum) * (255 - lum)) / (rgbMax - lum));
    *gOut = (uchar)(lum + ((gIn - lum) * (255 - lum)) / (rgbMax - lum));
    *bOut = (uchar)(lum + ((bIn - lum) * (255 - lum)) / (rgbMax - lum));
  } else {
    *rOut = rIn;
    *gOut = gIn;
    *bOut = bIn;
  }
}

static void setLum(uchar rIn, uchar gIn, uchar bIn, int lum,
		   uchar *rOut, uchar *gOut, uchar *bOut) {
  int d;

  d = lum - getLum(rIn, gIn, bIn);
  clipColor(rIn + d, gIn + d, bIn + d, rOut, gOut, bOut);
}

static void setSat(uchar rIn, uchar gIn, uchar bIn, int sat,
		   uchar *rOut, uchar *gOut, uchar *bOut) {
  int rgbMin, rgbMid, rgbMax;
  uchar *minOut, *midOut, *maxOut;

  if (rIn < gIn) {
    rgbMin = rIn;  minOut = rOut;
    rgbMid = gIn;  midOut = gOut;
  } else {
    rgbMin = gIn;  minOut = gOut;
    rgbMid = rIn;  midOut = rOut;
  }
  if (bIn > rgbMid) {
    rgbMax = bIn;  maxOut = bOut;
  } else if (bIn > rgbMin) {
    rgbMax = rgbMid;  maxOut = midOut;
    rgbMid = bIn;     midOut = bOut;
  } else {
    rgbMax = rgbMid;  maxOut = midOut;
    rgbMid = rgbMin;  midOut = minOut;
    rgbMin = bIn;     minOut = bOut;
  }
  if (rgbMax > rgbMin) {
    *midOut = (uchar)((rgbMid - rgbMin) * sat) / (rgbMax - rgbMin);
    *maxOut = (uchar)sat;
  } else {
    *midOut = *maxOut = 0;
  }
  *minOut = 0;
}

static void rasterOutBlendHue(uchar * src, 
                              uchar * dest,
			                  uchar * blend, 
			                  int cm) 
{
    uchar r0, g0, b0, r1, g1, b1;
    switch (cm) 
    {
        case COLOR_MODE_MONO1:
        case COLOR_MODE_MONO8:
            blend[0] = dest[0];
            break;
            
        case COLOR_MODE_RGB8:
        case COLOR_MODE_BGR8:
            setSat(src[0], src[1], src[2], getSat(dest[0], dest[1], dest[2]), &r0, &g0, &b0);
    				setLum(r0, g0, b0, getLum(dest[0], dest[1], dest[2]),  &blend[0], &blend[1], &blend[2]);
            break;
            
        case COLOR_MODE_CMYK8:
            setSat(src[0], src[1], src[2], getSat(dest[0], dest[1], dest[2]), &r0, &g0, &b0);
    				setLum(r0, g0, b0, getLum(dest[0], dest[1], dest[2]),  &r1, &g1, &b1);
    				blend[0] = r1;
    				blend[1] = g1;
    				blend[2] = b1;
    				blend[3] = dest[3];
            break;
    }
}

static void rasterOutBlendSaturation(uchar * src, 
                                     uchar * dest,
				                     uchar * blend,
				                     int cm) 
{
    uchar r0, g0, b0, r1, g1, b1;
    switch (cm) 
    {
        case COLOR_MODE_MONO1:
        case COLOR_MODE_MONO8:
            blend[0] = dest[0];
            break;
            
        case COLOR_MODE_RGB8:
        case COLOR_MODE_BGR8:
            setSat(dest[0], dest[1], dest[2], getSat(src[0], src[1], src[2]), &r0, &g0, &b0);
    				setLum(r0, g0, b0, getLum(dest[0], dest[1], dest[2]), &blend[0], &blend[1], &blend[2]);
            break;
            
        case COLOR_MODE_CMYK8:
            setSat(dest[0], dest[1], dest[2], getSat(src[0], src[1], src[2]), &r0, &g0, &b0);
    				setLum(r0, g0, b0, getLum(dest[0], dest[1], dest[2]), &r1, &g1, &b1);
    				blend[0] = r1;
    				blend[1] = g1;
    				blend[2] = b1;
    				blend[3] = dest[3];
            break;
    }
}

static void rasterOutBlendColor(uchar * src, 
                                uchar * dest,
				                uchar * blend, 
				                int cm) 
{
    uchar r, g, b;

    switch (cm) 
    {
        case COLOR_MODE_MONO1:
        case COLOR_MODE_MONO8:
            blend[0] = dest[0];
            break;
            
        case COLOR_MODE_RGB8:
        case COLOR_MODE_BGR8:
            setLum(src[0], src[1], src[2], getLum(dest[0], dest[1], dest[2]), &blend[0], &blend[1], &blend[2]);
            break;
            
        case COLOR_MODE_CMYK8:
            setLum(src[0], src[1], src[2], getLum(dest[0], dest[1], dest[2]), &r, &g, &b);
    				blend[0] = r;
    				blend[1] = g;
    				blend[2] = b;
    				blend[3] = dest[3];
            break;
    }
}

static void rasterOutBlendLuminosity(uchar * src, 
                                     uchar * dest,
				                     uchar * blend,
				                     int cm) 
{
    uchar r, g, b;

    switch (cm) 
    {
        case COLOR_MODE_MONO1:
        case COLOR_MODE_MONO8:
            blend[0] = dest[0];
            break;
            
        case COLOR_MODE_RGB8:
        case COLOR_MODE_BGR8:
            setLum(dest[0], dest[1], dest[2], getLum(src[0], src[1], src[2]), &blend[0], &blend[1], &blend[2]);
            break;
            
        case COLOR_MODE_CMYK8:
            setLum(dest[0], dest[1], dest[2], getLum(src[0], src[1], src[2]), &r, &g, &b);
    				blend[0] = r;
    				blend[1] = g;
    				blend[2] = b;
    				blend[3] = src[3];
            break;
    }
}

XWRasterBlendFunc rasterOutBlendFuncs[] = {
    NULL,
    &rasterOutBlendMultiply,
    &rasterOutBlendScreen,
    &rasterOutBlendOverlay,
    &rasterOutBlendDarken,
    &rasterOutBlendLighten,
    &rasterOutBlendColorDodge,
    &rasterOutBlendColorBurn,
    &rasterOutBlendHardLight,
    &rasterOutBlendSoftLight,
    &rasterOutBlendDifference,
    &rasterOutBlendExclusion,
    &rasterOutBlendHue,
    &rasterOutBlendSaturation,
    &rasterOutBlendColor,
    &rasterOutBlendLuminosity
};


class RasterOutFontFileID: public XWFontFileID 
{
public:
    RasterOutFontFileID(ObjRef *rA) { r = *rA; substIdx = -1; }

    ~RasterOutFontFileID() {}

    int  getSubstIdx() { return substIdx; }
    
    bool matches(XWFontFileID *id) 
    {
        return ((RasterOutFontFileID *)id)->r.num == r.num && ((RasterOutFontFileID *)id)->r.gen == r.gen;
    }

    void setSubstIdx(int substIdxA) { substIdx = substIdxA; }
    
private:
    ObjRef r;
};


struct T3FontCacheTag 
{
    ushort code;
    ushort mru;
};

class T3FontCache
{
public:
    T3FontCache(ObjRef *fontIDA, 
                double m11A, 
                double m12A,
	            double m21A, 
	            double m22A,
	            int glyphXA, 
	            int glyphYA, 
	            int glyphWA, 
	            int glyphHA,
	            bool aa, 
	            bool validBBoxA);
    ~T3FontCache();
    
    bool matches(ObjRef *idA, 
                 double m11A, 
                 double m12A,
		         double m21A, 
		         double m22A)
        { 
            return fontID.num == idA->num && fontID.gen == idA->gen &&  
	                m11 == m11A && m12 == m12A && m21 == m21A && m22 == m22A; 
	    }
	    
public:
    ObjRef fontID;
    double m11, m12, m21, m22;
    int glyphX, glyphY;
    int glyphW, glyphH;
    bool validBBox;
    int glyphSize;
    int cacheSets;
    int cacheAssoc;
    uchar *cacheData;
    T3FontCacheTag *cacheTags;
};

T3FontCache::T3FontCache(ObjRef *fontIDA, 
                         double m11A, 
                         double m12A,
	                     double m21A, 
	                     double m22A,
	                     int glyphXA, 
	                     int glyphYA, 
	                     int glyphWA, 
	                     int glyphHA,
	                     bool aa, 
	                     bool validBBoxA)
{
  int i;

  fontID = *fontIDA;
  m11 = m11A;
  m12 = m12A;
  m21 = m21A;
  m22 = m22A;
  glyphX = glyphXA;
  glyphY = glyphYA;
  glyphW = glyphWA;
  glyphH = glyphHA;
  validBBox = validBBoxA;
  // sanity check for excessively large glyphs (which most likely
  // indicate an incorrect BBox)
  i = glyphW * glyphH;
  if (i > 100000 || glyphW > INT_MAX / glyphH || glyphW <= 0 || glyphH <= 0) {
    glyphW = glyphH = 100;
    validBBox = false;
  }
  if (aa) {
    glyphSize = glyphW * glyphH;
  } else {
    glyphSize = ((glyphW + 7) >> 3) * glyphH;
  }
  cacheAssoc = type3FontCacheAssoc;
  for (cacheSets = type3FontCacheMaxSets;
       cacheSets > 1 &&
	 cacheSets * cacheAssoc * glyphSize > type3FontCacheSize;
       cacheSets >>= 1) ;
  cacheData = (uchar *)malloc(cacheSets * cacheAssoc * glyphSize * sizeof(uchar));
  cacheTags = (T3FontCacheTag *)malloc(cacheSets * cacheAssoc * sizeof(T3FontCacheTag));
  for (i = 0; i < cacheSets * cacheAssoc; ++i) {
    cacheTags[i].mru = i & (cacheAssoc - 1);
  }
}

T3FontCache::~T3FontCache()
{
    if (cacheData)
        free(cacheData);
        
    if (cacheTags)
        free(cacheTags);
}

struct T3GlyphStack
{
    ushort code;
    T3FontCache *cache;
    T3FontCacheTag *cacheTag;
    uchar *cacheData;
    XWBitmap *origBitmap;
    XWRaster *origSplash;
    double origCTM4, origCTM5;
    T3GlyphStack *next;
};

struct RasterTransparencyGroup
{
    int tx, ty;
    XWBitmap *tBitmap;
    
    XWColorSpace *blendingColorSpace;
    bool isolated;
    
    XWBitmap *origBitmap;
    XWRaster *origSplash;

    RasterTransparencyGroup *next;
};


struct RasterOutImageMaskData 
{
    XWImageStream *imgStr;
    bool invert;
    int width, height, y;
};


struct RasterOutImageData 
{
    XWImageStream *imgStr;
    XWImageColorMap *colorMap;
    uchar * lookup;
    int *maskColors;
    int colorMode;
  int width, height, y;
};


struct RasterOutMaskedImageData 
{
    XWImageStream *imgStr;
    XWImageColorMap *colorMap;
    XWBitmap *mask;
    uchar * lookup;
    int colorMode;
    int width, height, y;
};

XWRasterOutputDev::XWRasterOutputDev()
{
	colorMode = COLOR_MODE_RGB8;
  bitmapRowPad = 4;
  reverseVideo = false;
  bitmapTopDown = true;
  bitmapUpsideDown = false;
  allowAntialias = true;
  setupScreenParams(72.0, 72.0);
  paperColor[0] = 0xff;
  paperColor[1] = 0xff;
  paperColor[2] = 0xff;
  paperColor[3] = 0xff;
  xref = NULL;
  skipHorizText = false;
  skipRotatedText = false;

  bitmap = new XWBitmap(1, 1, bitmapRowPad, colorMode, colorMode != COLOR_MODE_MONO1, bitmapTopDown);
  raster = new XWRaster(bitmap, vectorAntialias, &screenParams);
  raster->clear(paperColor, 0);
  fontEngine = 0;

  nT3Fonts = 0;
  t3GlyphStack = 0;

  font = NULL;
  needFontUpdate = false;
  textClipPath = 0;

  transpGroupStack = 0;
  nestCount = 0;
}

XWRasterOutputDev::XWRasterOutputDev(int colorModeA, 
                                     int bitmapRowPadA,
		                             bool reverseVideoA, 
		                             uchar * paperColorA,
		                             bool bitmapTopDownA,
		                             bool allowAntialiasA)
{
    colorMode = colorModeA;
    bitmapRowPad = bitmapRowPadA;
    bitmapTopDown = bitmapTopDownA;
    bitmapUpsideDown = false;
    allowAntialias = allowAntialiasA;
    
    XWDocSetting docsetting;
    
    vectorAntialias = allowAntialias && 
                      docsetting.getVectorAntialias() && 
                      colorMode != COLOR_MODE_MONO1;
    setupScreenParams(72.0, 72.0);
    reverseVideo = reverseVideoA;
    colorCopy(paperColor, paperColorA);
    skipHorizText = false;
  	skipRotatedText = false;
    
    xref = NULL;

    bitmap = new XWBitmap(1, 1, bitmapRowPad, colorMode, colorMode != COLOR_MODE_MONO1, bitmapTopDown);
    raster = new XWRaster(bitmap, vectorAntialias, &screenParams);
    raster->clear(paperColor, 0);

    fontEngine = 0;

    nT3Fonts = 0;
    t3GlyphStack = 0;

    font = NULL;
    needFontUpdate = false;
    textClipPath = 0;

    transpGroupStack = 0;
    nestCount = 0;
}

XWRasterOutputDev::~XWRasterOutputDev()
{
    for (int i = 0; i < nT3Fonts; ++i) 
        delete t3FontCache[i];
    
    if (fontEngine) 
        delete fontEngine;
    
    if (raster) 
    {
        delete raster;
      }
    
    if (bitmap) 
    {
        delete bitmap;
      }
}

void XWRasterOutputDev::beginTransparencyGroup(XWGraphixState *state, 
                                               double *bbox,
				                               XWColorSpace *blendingColorSpace,
				                               bool isolated, 
				                               bool ,
				                               bool forSoftMask)
{
  RasterTransparencyGroup *transpGroup;
  uchar color[4];
  double xMin, yMin, xMax, yMax, x, y;
  int tx, ty, w, h, i;

  // transform the bbox
  state->transform(bbox[0], bbox[1], &x, &y);
  xMin = xMax = x;
  yMin = yMax = y;
  state->transform(bbox[0], bbox[3], &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  state->transform(bbox[2], bbox[1], &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  state->transform(bbox[2], bbox[3], &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  tx = (int)floor(xMin);
  if (tx < 0) {
    tx = 0;
  } else if (tx >= bitmap->getWidth()) {
    tx = bitmap->getWidth() - 1;
  }
  ty = (int)floor(yMin);
  if (ty < 0) {
    ty = 0;
  } else if (ty >= bitmap->getHeight()) {
    ty = bitmap->getHeight() - 1;
  }
  w = (int)ceil(xMax) - tx + 1;
  if (tx + w > bitmap->getWidth()) {
    w = bitmap->getWidth() - tx;
  }
  if (w < 1) {
    w = 1;
  }
  h = (int)ceil(yMax) - ty + 1;
  if (ty + h > bitmap->getHeight()) {
    h = bitmap->getHeight() - ty;
  }
  if (h < 1) {
    h = 1;
  }

  // push a new stack entry
  transpGroup = new RasterTransparencyGroup();
  transpGroup->tx = tx;
  transpGroup->ty = ty;
  transpGroup->blendingColorSpace = blendingColorSpace;
  transpGroup->isolated = isolated;
  transpGroup->next = transpGroupStack;
  transpGroupStack = transpGroup;

  // save state
  transpGroup->origBitmap = bitmap;
  transpGroup->origSplash = raster;

  //~ this handles the blendingColorSpace arg for soft masks, but
  //~   not yet for transparency groups

  // switch to the blending color space
  if (forSoftMask && isolated && blendingColorSpace) {
    if (blendingColorSpace->getMode() == COLOR_SPACE_DEVICEGRAY ||
	blendingColorSpace->getMode() == COLOR_SPACE_CALGRAY ||
	(blendingColorSpace->getMode() == COLOR_SPACE_ICCBASED &&
	 blendingColorSpace->getNComps() == 1)) {
      colorMode = COLOR_MODE_MONO8;
    } else if (blendingColorSpace->getMode() == COLOR_SPACE_DEVICERGB ||
	       blendingColorSpace->getMode() == COLOR_SPACE_CALRGB ||
	       (blendingColorSpace->getMode() == COLOR_SPACE_ICCBASED &&
		blendingColorSpace->getNComps() == 3)) {
      //~ does this need to use BGR8?
      colorMode = COLOR_MODE_RGB8;
    } else if (blendingColorSpace->getMode() == COLOR_SPACE_DEVICECMYK ||
	       (blendingColorSpace->getMode() == COLOR_SPACE_ICCBASED &&
		blendingColorSpace->getNComps() == 4)) {
      colorMode = COLOR_MODE_CMYK8;
    }
  }

  // create the temporary bitmap
  bitmap = new XWBitmap(w, h, bitmapRowPad, colorMode, true,
			    bitmapTopDown); 
  raster = new XWRaster(bitmap, vectorAntialias,
		      transpGroup->origSplash->getScreen());
  raster->setMinLineWidth(0.0);
  //~ Acrobat apparently copies at least the fill and stroke colors, and
  //~ maybe other state(?) -- but not the clipping path (and not sure
  //~ what else)
  //~ [this is likely the same situation as in type3D1()]
  raster->setFillPattern(transpGroup->origSplash->getFillPattern()->copy());
  raster->setStrokePattern(
		         transpGroup->origSplash->getStrokePattern()->copy());
  if (isolated) {
    for (i = 0; i < MAX_COLOR_COMPS; ++i) {
      color[i] = 0;
    }
    raster->clear(color, 0);
  } else {
    raster->blitTransparent(transpGroup->origBitmap, tx, ty, 0, 0, w, h);
    raster->setInNonIsolatedGroup(transpGroup->origBitmap, tx, ty);
  }
  transpGroup->tBitmap = bitmap;
  state->shiftCTM(-tx, -ty);
  updateCTM(state, 0, 0, 0, 0, 0, 0);
  ++nestCount;
}

bool XWRasterOutputDev::beginType3Char(XWGraphixState *state, 
                                       double , 
                                       double ,
			                           double , 
			                           double ,
			                           uint code, 
			                           uint *, 
			                           int )
{
  XWCoreFont *gfxFont;
  ObjRef *fontID;
  double *ctm, *bbox;
  T3FontCache *t3Font;
  T3GlyphStack *t3gs;
  bool validBBox;
  double m[4];
  bool horiz;
  double x1, y1, xMin, yMin, xMax, yMax, xt, yt;
  int i, j;

  if (skipHorizText || skipRotatedText) {
    state->getFontTransMat(&m[0], &m[1], &m[2], &m[3]);
    horiz = m[0] > 0 && fabs(m[1]) < 0.001 &&
            fabs(m[2]) < 0.001 && m[3] < 0;
    if ((skipHorizText && horiz) || (skipRotatedText && !horiz)) {
      return true;
    }
  }

  if (!(gfxFont = state->getFont())) {
    return false;
  }
  fontID = gfxFont->getID();
  ctm = state->getCTM();
  state->transform(0, 0, &xt, &yt);

  // is it the first (MRU) font in the cache?
  if (!(nT3Fonts > 0 &&
	t3FontCache[0]->matches(fontID, ctm[0], ctm[1], ctm[2], ctm[3]))) {

    // is the font elsewhere in the cache?
    for (i = 1; i < nT3Fonts; ++i) {
      if (t3FontCache[i]->matches(fontID, ctm[0], ctm[1], ctm[2], ctm[3])) {
	t3Font = t3FontCache[i];
	for (j = i; j > 0; --j) {
	  t3FontCache[j] = t3FontCache[j - 1];
	}
	t3FontCache[0] = t3Font;
	break;
      }
    }
    if (i >= nT3Fonts) {

      // create new entry in the font cache
      if (nT3Fonts == rasterOutT3FontCacheSize) {
	delete t3FontCache[nT3Fonts - 1];
	--nT3Fonts;
      }
      for (j = nT3Fonts; j > 0; --j) {
	t3FontCache[j] = t3FontCache[j - 1];
      }
      ++nT3Fonts;
      bbox = gfxFont->getFontBBox();
      if (bbox[0] == 0 && bbox[1] == 0 && bbox[2] == 0 && bbox[3] == 0) {
	// unspecified bounding box -- just take a guess
	xMin = xt - 5;
	xMax = xMin + 30;
	yMax = yt + 15;
	yMin = yMax - 45;
	validBBox = false;
      } else {
	state->transform(bbox[0], bbox[1], &x1, &y1);
	xMin = xMax = x1;
	yMin = yMax = y1;
	state->transform(bbox[0], bbox[3], &x1, &y1);
	if (x1 < xMin) {
	  xMin = x1;
	} else if (x1 > xMax) {
	  xMax = x1;
	}
	if (y1 < yMin) {
	  yMin = y1;
	} else if (y1 > yMax) {
	  yMax = y1;
	}
	state->transform(bbox[2], bbox[1], &x1, &y1);
	if (x1 < xMin) {
	  xMin = x1;
	} else if (x1 > xMax) {
	  xMax = x1;
	}
	if (y1 < yMin) {
	  yMin = y1;
	} else if (y1 > yMax) {
	  yMax = y1;
	}
	state->transform(bbox[2], bbox[3], &x1, &y1);
	if (x1 < xMin) {
	  xMin = x1;
	} else if (x1 > xMax) {
	  xMax = x1;
	}
	if (y1 < yMin) {
	  yMin = y1;
	} else if (y1 > yMax) {
	  yMax = y1;
	}
	validBBox = true;
      }
      t3FontCache[0] = new T3FontCache(fontID, ctm[0], ctm[1], ctm[2], ctm[3],
	                               (int)floor(xMin - xt) - 2,
				       (int)floor(yMin - yt) - 2,
				       (int)ceil(xMax) - (int)floor(xMin) + 4,
				       (int)ceil(yMax) - (int)floor(yMin) + 4,
				       validBBox,
				       colorMode != COLOR_MODE_MONO1);
    }
  }
  t3Font = t3FontCache[0];

  // is the glyph in the cache?
  i = (code & (t3Font->cacheSets - 1)) * t3Font->cacheAssoc;
  for (j = 0; j < t3Font->cacheAssoc; ++j) {
    if ((t3Font->cacheTags[i+j].mru & 0x8000) &&
	t3Font->cacheTags[i+j].code == code) {
      drawType3Glyph(state, t3Font, &t3Font->cacheTags[i+j],
		     t3Font->cacheData + (i+j) * t3Font->glyphSize);
      return true;
    }
  }

  // push a new Type 3 glyph record
  t3gs = new T3GlyphStack();
  t3gs->next = t3GlyphStack;
  t3GlyphStack = t3gs;
  t3GlyphStack->code = code;
  t3GlyphStack->cache = t3Font;
  t3GlyphStack->cacheTag = NULL;
  t3GlyphStack->cacheData = NULL;

  haveT3Dx = false;

  return false;
}

void XWRasterOutputDev::clearModRegion()
{
    raster->clearModRegion();
}

void XWRasterOutputDev::clearSoftMask(XWGraphixState *)
{
    raster->setSoftMask(0);
}

void XWRasterOutputDev::clip(XWGraphixState *state)
{
   XWRasterPath *path;

  path = convertPath(state, state->getPath(), true);
  raster->clipToPath(path, false);
  delete path;
}

void XWRasterOutputDev::clipToStrokePath(XWGraphixState *state)
{
  XWRasterPath *path, *path2;

  path = convertPath(state, state->getPath(), false);
  path2 = raster->makeStrokePath(path, state->getLineWidth());
  delete path;
  raster->clipToPath(path2, false);
  delete path2;
}

void XWRasterOutputDev::drawChar(XWGraphixState *state, 
                                 double x, 
                                 double y,
			                     double , 
			                     double ,
			                     double originX, 
			                     double originY,
			                     uint code, 
			                     int , 
			                     uint *, 
			                     int )
{
  XWRasterPath *path;
  int render;
  bool doFill, doStroke, doClip, strokeAdjust;
  double m[4];
  bool horiz;

  if (skipHorizText || skipRotatedText) {
    state->getFontTransMat(&m[0], &m[1], &m[2], &m[3]);
    horiz = m[0] > 0 && fabs(m[1]) < 0.001 &&
            fabs(m[2]) < 0.001 && m[3] < 0;
    if ((skipHorizText && horiz) || (skipRotatedText && !horiz)) {
      return;
    }
  }

  // check for invisible text -- this is used by Acrobat Capture
  render = state->getRender();
  if (render == 3) {
    return;
  }

  if (needFontUpdate) {
    doUpdateFont(state);
  }
  if (!font) {
    return;
  }

  x -= originX;
  y -= originY;

  doFill = !(render & 1) && !state->getFillColorSpace()->isNonMarking();
  doStroke = ((render & 3) == 1 || (render & 3) == 2) &&
             !state->getStrokeColorSpace()->isNonMarking();
  doClip = render & 4;

  path = NULL;
  if (doStroke || doClip) {
    if ((path = font->getGlyphPath(code))) {
      path->offset((double)x, (double)y);
    }
  }

  // don't use stroke adjustment when stroking text -- the results
  // tend to be ugly (because characters with horizontal upper or
  // lower edges get misaligned relative to the other characters)
  strokeAdjust = false; // make gcc happy
  if (doStroke) {
    strokeAdjust = raster->getStrokeAdjust();
    raster->setStrokeAdjust(false);
  }

  // fill and stroke
  if (doFill && doStroke) {
    if (path) {
      setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		       state->getOverprintMode(), state->getFillColor());
      raster->fill(path, false);
      setOverprintMask(state->getStrokeColorSpace(),
		       state->getStrokeOverprint(),
		       state->getOverprintMode(),
		       state->getStrokeColor());
      raster->stroke(path);
    }

  // fill
  } else if (doFill) {
    setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		     state->getOverprintMode(), state->getFillColor());
    raster->fillChar((double)x, (double)y, code, font);

  // stroke
  } else if (doStroke) {
    if (path) {
      setOverprintMask(state->getStrokeColorSpace(),
		       state->getStrokeOverprint(),
		       state->getOverprintMode(),
		       state->getStrokeColor());
      raster->stroke(path);
    }
  }

  // clip
  if (doClip) {
    if (path) {
      if (textClipPath) {
	textClipPath->append(path);
      } else {
	textClipPath = path;
	path = NULL;
      }
    }
  }

  if (doStroke) {
    raster->setStrokeAdjust(strokeAdjust);
  }

  if (path) {
    delete path;
  }
}

void XWRasterOutputDev::drawImage(XWGraphixState *state, 
	                              XWObject *, 
	                              XWStream *str,
			                      int width, 
			                      int height, 
			                      XWImageColorMap *colorMap,
			                      int *maskColors, 
			                      bool inlineImg)
{
  double *ctm;
  double mat[6];
  RasterOutImageData imgData;
  int srcMode;
  XWRasterImageSource src;
  CoreGray gray;
  CoreRGB rgb;
  CoreCMYK cmyk;
  uchar pix;
  int n, i;

  setOverprintMask(colorMap->getColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), NULL);

  ctm = state->getCTM();
  mat[0] = ctm[0];
  mat[1] = ctm[1];
  mat[2] = -ctm[2];
  mat[3] = -ctm[3];
  mat[4] = ctm[2] + ctm[4];
  mat[5] = ctm[3] + ctm[5];

  imgData.imgStr = new XWImageStream(str, width,
				   colorMap->getNumPixelComps(),
				   colorMap->getBits());
  imgData.imgStr->reset();
  imgData.colorMap = colorMap;
  imgData.maskColors = maskColors;
  imgData.colorMode = colorMode;
  imgData.width = width;
  imgData.height = height;
  imgData.y = 0;

  // special case for one-channel (monochrome/gray/separation) images:
  // build a lookup table here
  imgData.lookup = NULL;
  if (colorMap->getNumPixelComps() == 1) {
    n = 1 << colorMap->getBits();
    switch (colorMode) {
    case COLOR_MODE_MONO1:
    case COLOR_MODE_MONO8:
      imgData.lookup = (uchar*)malloc(n * sizeof(uchar));
      for (i = 0; i < n; ++i) {
	pix = (uchar)i;
	colorMap->getGray(&pix, &gray);
	imgData.lookup[i] = colToByte(gray);
      }
      break;
      
    case COLOR_MODE_RGB8:
    case COLOR_MODE_BGR8:
      imgData.lookup = (uchar*)malloc(n * 3 * sizeof(uchar));
      for (i = 0; i < n; ++i) {
	pix = (uchar)i;
	colorMap->getRGB(&pix, &rgb);
	imgData.lookup[3*i] = colToByte(rgb.r);
	imgData.lookup[3*i+1] = colToByte(rgb.g);
	imgData.lookup[3*i+2] = colToByte(rgb.b);
      }
      break;
      
    case COLOR_MODE_CMYK8:
      imgData.lookup = (uchar*)malloc(n * 4 * sizeof(uchar));
      for (i = 0; i < n; ++i) {
	pix = (uchar)i;
	colorMap->getCMYK(&pix, &cmyk);
	imgData.lookup[4*i] = colToByte(cmyk.c);
	imgData.lookup[4*i+1] = colToByte(cmyk.m);
	imgData.lookup[4*i+2] = colToByte(cmyk.y);
	imgData.lookup[4*i+3] = colToByte(cmyk.k);
      }
      break;
    }
  }

  if (colorMode == COLOR_MODE_MONO1) {
    srcMode = COLOR_MODE_MONO8;
  } else {
    srcMode = colorMode;
  }
  src = maskColors ? &alphaImageSrc : &imageSrc;
  raster->drawImage(src, &imgData, srcMode, maskColors ? true : false,
		    width, height, mat);
  if (inlineImg) {
    while (imgData.y < height) {
      imgData.imgStr->getLine();
      ++imgData.y;
    }
  }

	if (imgData.lookup)
  free(imgData.lookup);
  delete imgData.imgStr;
  str->close();
}

void XWRasterOutputDev::drawImageMask(XWGraphixState *state, 
	                                  XWObject *, 
	                                  XWStream *str,
			                          int width, 
			                          int height, 
			                          bool invert,
			                          bool inlineImg)
{
  double *ctm;
  double mat[6];
  RasterOutImageMaskData imgMaskData;

  if (state->getFillColorSpace()->isNonMarking()) {
    return;
  }
  setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), state->getFillColor());

  ctm = state->getCTM();
  mat[0] = ctm[0];
  mat[1] = ctm[1];
  mat[2] = -ctm[2];
  mat[3] = -ctm[3];
  mat[4] = ctm[2] + ctm[4];
  mat[5] = ctm[3] + ctm[5];

  imgMaskData.imgStr = new XWImageStream(str, width, 1, 1);
  imgMaskData.imgStr->reset();
  imgMaskData.invert = invert ? 0 : 1;
  imgMaskData.width = width;
  imgMaskData.height = height;
  imgMaskData.y = 0;

  raster->fillImageMask(&imageMaskSrc, &imgMaskData, width, height, mat,
			t3GlyphStack != NULL);
  if (inlineImg) {
    while (imgMaskData.y < height) {
      imgMaskData.imgStr->getLine();
      ++imgMaskData.y;
    }
  }

  delete imgMaskData.imgStr;
  str->close();
}

void XWRasterOutputDev::drawMaskedImage(XWGraphixState *state, 
                                        XWObject *ref, 
                                        XWStream *str,
			                            int width, 
			                            int height,
			                            XWImageColorMap *colorMap,
			                            XWStream *maskStr, 
			                            int maskWidth, 
			                            int maskHeight,
			                            bool maskInvert)
{   
  XWImageColorMap *maskColorMap;
  XWObject maskDecode, decodeLow, decodeHigh;
  double *ctm;
  double mat[6];
  RasterOutMaskedImageData imgData;
  RasterOutImageMaskData imgMaskData;
  int srcMode;
  XWBitmap *maskBitmap;
  XWRaster *maskSplash;
  uchar maskColor[4];
  CoreGray gray;
  CoreRGB rgb;
  CoreCMYK cmyk;
  uchar pix;
  int n, i;

  setOverprintMask(colorMap->getColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), NULL);

  // If the mask is higher resolution than the image, use
  // drawSoftMaskedImage() instead.
  if (maskWidth > width || maskHeight > height) {
    decodeLow.initInt(maskInvert ? 0 : 1);
    decodeHigh.initInt(maskInvert ? 1 : 0);
    maskDecode.initArray(xref);
    maskDecode.arrayAdd(&decodeLow);
    maskDecode.arrayAdd(&decodeHigh);
    maskColorMap = new XWImageColorMap(1, &maskDecode, new XWDeviceGrayColorSpace());
    maskDecode.free();
    drawSoftMaskedImage(state, ref, str, width, height, colorMap,
			maskStr, maskWidth, maskHeight, maskColorMap);
    delete maskColorMap;

  } else {

    //----- scale the mask image to the same size as the source image

    mat[0] = (double)width;
    mat[1] = 0;
    mat[2] = 0;
    mat[3] = (double)height;
    mat[4] = 0;
    mat[5] = 0;
    imgMaskData.imgStr = new XWImageStream(maskStr, maskWidth, 1, 1);
    imgMaskData.imgStr->reset();
    imgMaskData.invert = maskInvert ? 0 : 1;
    imgMaskData.width = maskWidth;
    imgMaskData.height = maskHeight;
    imgMaskData.y = 0;
    maskBitmap = new XWBitmap(width, height, 1, COLOR_MODE_MONO1, false);
    maskSplash = new XWRaster(maskBitmap, false);
    maskColor[0] = 0;
    maskSplash->clear(maskColor);
    maskColor[0] = 0xff;
    maskSplash->setFillPattern(new XWSolidColor(maskColor));
    maskSplash->fillImageMask(&imageMaskSrc, &imgMaskData,
			      maskWidth, maskHeight, mat, false);
    delete imgMaskData.imgStr;
    maskStr->close();
    delete maskSplash;

    //----- draw the source image

    ctm = state->getCTM();
    mat[0] = ctm[0];
    mat[1] = ctm[1];
    mat[2] = -ctm[2];
    mat[3] = -ctm[3];
    mat[4] = ctm[2] + ctm[4];
    mat[5] = ctm[3] + ctm[5];

    imgData.imgStr = new XWImageStream(str, width,
				     colorMap->getNumPixelComps(),
				     colorMap->getBits());
    imgData.imgStr->reset();
    imgData.colorMap = colorMap;
    imgData.mask = maskBitmap;
    imgData.colorMode = colorMode;
    imgData.width = width;
    imgData.height = height;
    imgData.y = 0;

    // special case for one-channel (monochrome/gray/separation) images:
    // build a lookup table here
    imgData.lookup = NULL;
    if (colorMap->getNumPixelComps() == 1) {
      n = 1 << colorMap->getBits();
      switch (colorMode) {
      case COLOR_MODE_MONO1:
      case COLOR_MODE_MONO8:
	imgData.lookup = (uchar*)malloc(n * sizeof(uchar));
	for (i = 0; i < n; ++i) {
	  pix = (uchar)i;
	  colorMap->getGray(&pix, &gray);
	  imgData.lookup[i] = colToByte(gray);
	}
	break;
	
      case COLOR_MODE_RGB8:
      case COLOR_MODE_BGR8:
	imgData.lookup = (uchar*)malloc(n * 3 * sizeof(uchar));
	for (i = 0; i < n; ++i) {
	  pix = (uchar)i;
	  colorMap->getRGB(&pix, &rgb);
	  imgData.lookup[3*i] = colToByte(rgb.r);
	  imgData.lookup[3*i+1] = colToByte(rgb.g);
	  imgData.lookup[3*i+2] = colToByte(rgb.b);
	}
	break;
	
      case COLOR_MODE_CMYK8:
	imgData.lookup = (uchar*)malloc(n * 4 * sizeof(uchar));
	for (i = 0; i < n; ++i) {
	  pix = (uchar)i;
	  colorMap->getCMYK(&pix, &cmyk);
	  imgData.lookup[4*i] = colToByte(cmyk.c);
	  imgData.lookup[4*i+1] = colToByte(cmyk.m);
	  imgData.lookup[4*i+2] = colToByte(cmyk.y);
	  imgData.lookup[4*i+3] = colToByte(cmyk.k);
	}
	break;
      }
    }

    if (colorMode == COLOR_MODE_MONO1) {
      srcMode = COLOR_MODE_MONO8;
    } else {
      srcMode = colorMode;
    }
    raster->drawImage(&maskedImageSrc, &imgData, srcMode, true,
		      width, height, mat);

    delete maskBitmap;
    if (imgData.lookup)
    free(imgData.lookup);
    delete imgData.imgStr;
    str->close();
  }
}

void XWRasterOutputDev::drawSoftMaskedImage(XWGraphixState *state, 
	                                        XWObject *, 
	                                        XWStream *str,
				                            int width, 
				                            int height,
				                            XWImageColorMap *colorMap,
				                            XWStream *maskStr,
				                            int maskWidth, 
				                            int maskHeight,
				                            XWImageColorMap *maskColorMap)
{
  double *ctm;
  double mat[6];
  RasterOutImageData imgData;
  RasterOutImageData imgMaskData;
  int srcMode;
  XWBitmap *maskBitmap;
  XWRaster *maskSplash;
  uchar maskColor[4];
  CoreGray gray;
  CoreRGB rgb;
  CoreCMYK cmyk;
  uchar pix;
  int n, i;

  setOverprintMask(colorMap->getColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), NULL);

  ctm = state->getCTM();
  mat[0] = ctm[0];
  mat[1] = ctm[1];
  mat[2] = -ctm[2];
  mat[3] = -ctm[3];
  mat[4] = ctm[2] + ctm[4];
  mat[5] = ctm[3] + ctm[5];

  //----- set up the soft mask

  imgMaskData.imgStr = new XWImageStream(maskStr, maskWidth,
				       maskColorMap->getNumPixelComps(),
				       maskColorMap->getBits());
  imgMaskData.imgStr->reset();
  imgMaskData.colorMap = maskColorMap;
  imgMaskData.maskColors = NULL;
  imgMaskData.colorMode = COLOR_MODE_MONO8;
  imgMaskData.width = maskWidth;
  imgMaskData.height = maskHeight;
  imgMaskData.y = 0;
  n = 1 << maskColorMap->getBits();
  imgMaskData.lookup = (uchar*)malloc(n * sizeof(uchar));
  for (i = 0; i < n; ++i) {
    pix = (uchar)i;
    maskColorMap->getGray(&pix, &gray);
    imgMaskData.lookup[i] = colToByte(gray);
  }
  maskBitmap = new XWBitmap(bitmap->getWidth(), bitmap->getHeight(),
				1, COLOR_MODE_MONO8, false);
  maskSplash = new XWRaster(maskBitmap, vectorAntialias);
  maskColor[0] = 0;
  maskSplash->clear(maskColor);
  maskSplash->drawImage(&imageSrc, &imgMaskData, COLOR_MODE_MONO8, false,
			maskWidth, maskHeight, mat);
  delete imgMaskData.imgStr;
  maskStr->close();
  if (imgMaskData.lookup)
  free(imgMaskData.lookup);
  delete maskSplash;
  raster->setSoftMask(maskBitmap);

  //----- draw the source image

  imgData.imgStr = new XWImageStream(str, width,
				   colorMap->getNumPixelComps(),
				   colorMap->getBits());
  imgData.imgStr->reset();
  imgData.colorMap = colorMap;
  imgData.maskColors = NULL;
  imgData.colorMode = colorMode;
  imgData.width = width;
  imgData.height = height;
  imgData.y = 0;

  // special case for one-channel (monochrome/gray/separation) images:
  // build a lookup table here
  imgData.lookup = NULL;
  if (colorMap->getNumPixelComps() == 1) {
    n = 1 << colorMap->getBits();
    switch (colorMode) {
    case COLOR_MODE_MONO1:
    case COLOR_MODE_MONO8:
      imgData.lookup = (uchar*)malloc(n * sizeof(uchar));
      for (i = 0; i < n; ++i) {
	pix = (uchar)i;
	colorMap->getGray(&pix, &gray);
	imgData.lookup[i] = colToByte(gray);
      }
      break;
      
    case COLOR_MODE_RGB8:
    case COLOR_MODE_BGR8:
      imgData.lookup = (uchar*)malloc(n * 3 * sizeof(uchar));
      for (i = 0; i < n; ++i) {
	pix = (uchar)i;
	colorMap->getRGB(&pix, &rgb);
	imgData.lookup[3*i] = colToByte(rgb.r);
	imgData.lookup[3*i+1] = colToByte(rgb.g);
	imgData.lookup[3*i+2] = colToByte(rgb.b);
      }
      break;
      
    case COLOR_MODE_CMYK8:
      imgData.lookup = (uchar*)malloc(n * 4 * sizeof(uchar));
      for (i = 0; i < n; ++i) {
	pix = (uchar)i;
	colorMap->getCMYK(&pix, &cmyk);
	imgData.lookup[4*i] = colToByte(cmyk.c);
	imgData.lookup[4*i+1] = colToByte(cmyk.m);
	imgData.lookup[4*i+2] = colToByte(cmyk.y);
	imgData.lookup[4*i+3] = colToByte(cmyk.k);
      }
      break;
    }
  }

  if (colorMode == COLOR_MODE_MONO1) {
    srcMode = COLOR_MODE_MONO8;
  } else {
    srcMode = colorMode;
  }
  raster->drawImage(&imageSrc, &imgData, srcMode, false, width, height, mat);

  raster->setSoftMask(NULL);
  if (imgData.lookup)
  free(imgData.lookup);
  delete imgData.imgStr;
  str->close();
}

void XWRasterOutputDev::endPage()
{
    if (colorMode != COLOR_MODE_MONO1) 
        raster->compositeBackground(paperColor);
}

void XWRasterOutputDev::endTextObject(XWGraphixState *)
{
  if (textClipPath) {
    raster->clipToPath(textClipPath, false);
    delete textClipPath;
    textClipPath = NULL;
  }
}

void XWRasterOutputDev::endTransparencyGroup(XWGraphixState *state)
{
  --nestCount;
  delete raster;
  bitmap = transpGroupStack->origBitmap;
  colorMode = bitmap->getMode();
  raster = transpGroupStack->origSplash;
  state->shiftCTM(transpGroupStack->tx, transpGroupStack->ty);
  updateCTM(state, 0, 0, 0, 0, 0, 0);
}

void XWRasterOutputDev::endType3Char(XWGraphixState *state)
{
  T3GlyphStack *t3gs;
  double *ctm;

  if (t3GlyphStack->cacheTag) {
    --nestCount;
    memcpy(t3GlyphStack->cacheData, bitmap->getDataPtr(),
	   t3GlyphStack->cache->glyphSize);
    delete bitmap;
    delete raster;
    bitmap = t3GlyphStack->origBitmap;
    raster = t3GlyphStack->origSplash;
    ctm = state->getCTM();
    state->setCTM(ctm[0], ctm[1], ctm[2], ctm[3],
		  t3GlyphStack->origCTM4, t3GlyphStack->origCTM5);
    updateCTM(state, 0, 0, 0, 0, 0, 0);
    drawType3Glyph(state, t3GlyphStack->cache,
		   t3GlyphStack->cacheTag, t3GlyphStack->cacheData);
  }
  t3gs = t3GlyphStack;
  t3GlyphStack = t3gs->next;
  delete t3gs;
}

void XWRasterOutputDev::eoClip(XWGraphixState *state)
{
  XWRasterPath *path;

  path = convertPath(state, state->getPath(), true);
  raster->clipToPath(path, true);
  delete path;
}

void XWRasterOutputDev::eoFill(XWGraphixState *state)
{
  XWRasterPath *path;

  if (state->getFillColorSpace()->isNonMarking()) {
    return;
  }
  setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), state->getFillColor());
  path = convertPath(state, state->getPath(), true);
  raster->fill(path, true);
  delete path;
}

void XWRasterOutputDev::fill(XWGraphixState *state)
{
  XWRasterPath *path;

  if (state->getFillColorSpace()->isNonMarking()) {
    return;
  }
  setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), state->getFillColor());
  path = convertPath(state, state->getPath(), true);
  raster->fill(path, false);
  delete path;
}

int XWRasterOutputDev::getBitmapHeight()
{
    return bitmap->getHeight();
}

int XWRasterOutputDev::getBitmapWidth()
{
    return bitmap->getWidth();
}

XWRasterFont * XWRasterOutputDev::getFont(XWString *name, double *textMatA)
{
  ObjRef ref;
  RasterOutFontFileID *id;
  XWFontLoc *fontLoc;
  XWRasterFontFile *fontFile;
  XWRasterFont *fontObj;
  XWFontFileTrueType *ff;
  int *codeToGID;
  int u;
  double textMat[4];
  int cmap, i;

  for (i = 0; i < nBuiltinFonts; ++i) {
    if (!name->cmp(builtinFonts[i].name)) {
      break;
    }
  }
  if (i == nBuiltinFonts) {
    return NULL;
  }
  ref.num = i;
  ref.gen = -1;
  id = new RasterOutFontFileID(&ref);
  
  XWFontSetting fontsetting;

  // check the font file cache
  if ((fontFile = fontEngine->getFontFile(id))) {
    delete id;

  // load the font file
  } else {
    if (!(fontLoc = XWCoreFont::locateBase14Font(name))) {
      return NULL;
    }
    if (fontLoc->fontType == fontType1) {
      fontFile = fontEngine->loadType1Font(id, fontLoc->path->getCString(),
					   false, winAnsiEncoding);
    } else if (fontLoc->fontType == fontTrueType) {
      if (!(ff = XWFontFileTrueType::load(fontLoc->path->getCString()))) {
	delete fontLoc;
	delete id;
	return NULL;
      }
      for (cmap = 0; cmap < ff->getNumCmaps(); ++cmap) {
	if ((ff->getCmapPlatform(cmap) == 3 &&
	     ff->getCmapEncoding(cmap) == 1) ||
	    ff->getCmapPlatform(cmap) == 0) {
	  break;
	}
      }
      if (cmap == ff->getNumCmaps()) {
	delete ff;
	delete fontLoc;
	delete id;
	return NULL;
      }
      codeToGID = (int *)malloc(256 * sizeof(int));
      for (i = 0; i < 256; ++i) {
	codeToGID[i] = 0;
	if (winAnsiEncoding[i] &&
	    (u = fontsetting.mapNameToUnicode(winAnsiEncoding[i]))) {
	  codeToGID[i] = ff->mapCodeToGID(cmap, u);
	}
      }
      delete ff;
      fontFile = fontEngine->loadTrueTypeFont(id,
					      fontLoc->path->getCString(),
					      fontLoc->fontNum,
					      false, codeToGID, 256, NULL);
    } else {
      delete fontLoc;
      delete id;
      return NULL;
    }
    delete fontLoc;
  }
  if (!fontFile) {
    return NULL;
  }

  // create the scaled font
  textMat[0] = (double)textMatA[0];
  textMat[1] = (double)textMatA[1];
  textMat[2] = (double)textMatA[2];
  textMat[3] = (double)textMatA[3];
  fontObj = fontEngine->getFont(fontFile, textMat, raster->getMatrix());

  return fontObj;
}

void XWRasterOutputDev::getModRegion(int *xMin, int *yMin, int *xMax, int *yMax)
{
    raster->getModRegion(xMin, yMin, xMax, yMax);
}

bool XWRasterOutputDev::getVectorAntialias()
{
    return raster->getVectorAntialias();
}

void XWRasterOutputDev::paintTransparencyGroup(XWGraphixState *, double *)
{
  XWBitmap *tBitmap;
  RasterTransparencyGroup *transpGroup;
  bool isolated;
  int tx, ty;

  tx = transpGroupStack->tx;
  ty = transpGroupStack->ty;
  tBitmap = transpGroupStack->tBitmap;
  isolated = transpGroupStack->isolated;

  // paint the transparency group onto the parent bitmap
  // - the clip path was set in the parent's state)
  if (tx < bitmap->getWidth() && ty < bitmap->getHeight()) {
    raster->setOverprintMask(0xffffffff);
    raster->composite(tBitmap, 0, 0, tx, ty,
		      tBitmap->getWidth(), tBitmap->getHeight(),
		      false, !isolated);
  }

  // pop the stack
  transpGroup = transpGroupStack;
  transpGroupStack = transpGroup->next;
  delete transpGroup;

  delete tBitmap;
}

void XWRasterOutputDev::restoreState(XWGraphixState *)
{
    raster->restoreState();
    needFontUpdate = true;
}

void XWRasterOutputDev::saveState(XWGraphixState *)
{
    raster->saveState();
}

void XWRasterOutputDev::setFillColor(int r, int g, int b)
{
  CoreRGB rgb;
  CoreGray gray;
  CoreCMYK cmyk;

  rgb.r = byteToCol(r);
  rgb.g = byteToCol(g);
  rgb.b = byteToCol(b);
  switch (colorMode) {
  case COLOR_MODE_MONO1:
  case COLOR_MODE_MONO8:
    gray = (int)(0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.g + 0.5);
    if (gray > COLOR_COMP1) {
      gray = COLOR_COMP1;
    }
    raster->setFillPattern(getColor(gray));
    break;
    
  case COLOR_MODE_RGB8:
  case COLOR_MODE_BGR8:
    raster->setFillPattern(getColor(&rgb));
    break;
    
  case COLOR_MODE_CMYK8:
    cmyk.c = COLOR_COMP1 - rgb.r;
    cmyk.m = COLOR_COMP1 - rgb.g;
    cmyk.y = COLOR_COMP1 - rgb.b;
    cmyk.k = 0;
    raster->setFillPattern(getColor(&cmyk));
    break;
  }
}

void XWRasterOutputDev::setInShading(bool sh)
{
	raster->setInShading(sh);
}

void XWRasterOutputDev::setPaperColor(uchar * paperColorA)
{
    colorCopy(paperColor, paperColorA);
}

void XWRasterOutputDev::setScreenParam(int t, 
                                       int s, 
                                       int dr, 
                                       double gamma, 
                                       double bt, 
                                       double wt)
{
    screenParams.size = s;
    screenParams.dotRadius = dr;
    screenParams.gamma = gamma;
    screenParams.blackThreshold = bt;
    screenParams.whiteThreshold = wt;
    switch (t)
    {
        case SCREEN_CLUSTERED:
            screenParams.type = SCREEN_TYPE_CLUSTERED;
            if (screenParams.size < 0) 
                screenParams.size = 10;
            break;
            
        case SCREEN_STOCHASTICCLUSTERED:
            screenParams.type = SCREEN_TYPE_STOCHASTICCLUSTERED;
            if (screenParams.size < 0) 
                screenParams.size = 100;
            if (screenParams.dotRadius < 0) 
                screenParams.dotRadius = 2;
            break;
            
        case SCREEN_DISPERSED:
        default:
            screenParams.type = SCREEN_TYPE_DISPERSED;
            if (screenParams.size < 0)
                screenParams.size = 4;
            break;
    }
}

void XWRasterOutputDev::setSoftMask(XWGraphixState *, 
                                    double *, 
                                    bool alpha,
			                        XWFunction *transferFunc, 
			                        CoreColor *backdropColor)
{
  XWBitmap *softMask, *tBitmap;
  XWRaster *tSplash;
  RasterTransparencyGroup *transpGroup;
  uchar color[4];
  uchar* p;
  CoreGray gray;
  CoreRGB rgb;
  CoreCMYK cmyk;
  double lum, lum2;
  int tx, ty, x, y;

  tx = transpGroupStack->tx;
  ty = transpGroupStack->ty;
  tBitmap = transpGroupStack->tBitmap;

  // composite with backdrop color
  if (!alpha && tBitmap->getMode() != COLOR_MODE_MONO1) {
    //~ need to correctly handle the case where no blending color
    //~ space is given
    tSplash = new XWRaster(tBitmap, vectorAntialias,
			 transpGroupStack->origSplash->getScreen());
    if (transpGroupStack->blendingColorSpace) {
      switch (tBitmap->getMode()) {
      case COLOR_MODE_MONO1:
	// transparency is not supported in mono1 mode
	break;
	
      case COLOR_MODE_MONO8:
	transpGroupStack->blendingColorSpace->getGray(backdropColor, &gray);
	color[0] = colToByte(gray);
	tSplash->compositeBackground(color);
	break;
	
      case COLOR_MODE_RGB8:
      case COLOR_MODE_BGR8:
	transpGroupStack->blendingColorSpace->getRGB(backdropColor, &rgb);
	color[0] = colToByte(rgb.r);
	color[1] = colToByte(rgb.g);
	color[2] = colToByte(rgb.b);
	tSplash->compositeBackground(color);
	break;
	
      case COLOR_MODE_CMYK8:
	transpGroupStack->blendingColorSpace->getCMYK(backdropColor, &cmyk);
	color[0] = colToByte(cmyk.c);
	color[1] = colToByte(cmyk.m);
	color[2] = colToByte(cmyk.y);
	color[3] = colToByte(cmyk.k);
	tSplash->compositeBackground(color);
	break;
      }
      delete tSplash;
    }
  }

  softMask = new XWBitmap(bitmap->getWidth(), bitmap->getHeight(),
			      1, COLOR_MODE_MONO8, false);
  memset(softMask->getDataPtr(), 0,
	 softMask->getRowSize() * softMask->getHeight());
  if (tx < softMask->getWidth() && ty < softMask->getHeight()) {
    p = softMask->getDataPtr() + ty * softMask->getRowSize() + tx;
    for (y = 0; y < tBitmap->getHeight(); ++y) {
      for (x = 0; x < tBitmap->getWidth(); ++x) {
	if (alpha) {
	  lum = tBitmap->getAlpha(x, y) / 255.0;
	} else {
	  tBitmap->getPixel(x, y, color);
	  // convert to luminosity
	  switch (tBitmap->getMode()) {
	  case COLOR_MODE_MONO1:
	  case COLOR_MODE_MONO8:
	    lum = color[0] / 255.0;
	    break;
	    
	  case COLOR_MODE_RGB8:
	  case COLOR_MODE_BGR8:
	    lum = (0.3 / 255.0) * color[0] +
	          (0.59 / 255.0) * color[1] +
	          (0.11 / 255.0) * color[2];
	    break;
	    
	  case COLOR_MODE_CMYK8:
	    lum = (1 - color[3] / 255.0)
	          - (0.3 / 255.0) * color[0]
	          - (0.59 / 255.0) * color[1]
	          - (0.11 / 255.0) * color[2];
	    if (lum < 0) {
	      lum = 0;
	    }
	    break;
	  }
	}
	if (transferFunc) {
	  transferFunc->transform(&lum, &lum2);
	} else {
	  lum2 = lum;
	}
	p[x] = (int)(lum2 * 255.0 + 0.5);
      }
      p += softMask->getRowSize();
    }
  }
  raster->setSoftMask(softMask);

  // pop the stack
  transpGroup = transpGroupStack;
  transpGroupStack = transpGroup->next;
  delete transpGroup;

  delete tBitmap;
}

void XWRasterOutputDev::setSoftMaskFromImageMask(XWGraphixState *state,
																					XWObject *, 
																					XWStream *str,
																					int width, 
																					int height, 
																					bool invert,
																					bool )
{
	double *ctm;
  double mat[6];
  RasterOutImageMaskData imgMaskData;
  XWBitmap *maskBitmap;
  XWRaster *maskSplash;
  uchar maskColor[4];

  ctm = state->getCTM();
  mat[0] = ctm[0];
  mat[1] = ctm[1];
  mat[2] = -ctm[2];
  mat[3] = -ctm[3];
  mat[4] = ctm[2] + ctm[4];
  mat[5] = ctm[3] + ctm[5];
  imgMaskData.imgStr = new XWImageStream(str, width, 1, 1);
  imgMaskData.imgStr->reset();
  imgMaskData.invert = invert ? 0 : 1;
  imgMaskData.width = width;
  imgMaskData.height = height;
  imgMaskData.y = 0;
  maskBitmap = new XWBitmap(bitmap->getWidth(), bitmap->getHeight(),1, COLOR_MODE_MONO8, false);
  maskSplash = new XWRaster(maskBitmap, true);
  maskColor[0] = 0;
  maskSplash->clear(maskColor);
  maskColor[0] = 0xff;
  maskSplash->setFillPattern(new XWSolidColor(maskColor));
  maskSplash->fillImageMask(&imageMaskSrc, &imgMaskData,
			    width, height, mat, false);
  delete imgMaskData.imgStr;
  str->close();
  delete maskSplash;
  raster->setSoftMask(maskBitmap);
}

void XWRasterOutputDev::setVectorAntialias(bool vaa)
{
    raster->setVectorAntialias(vaa);
}

void XWRasterOutputDev::startDoc(XWRef *xrefA)
{
    xref = xrefA;
    if (fontEngine) 
        delete fontEngine;
        
    fontEngine = new XWRasterFontEngine(allowAntialias && colorMode != COLOR_MODE_MONO1);
    for (int i = 0; i < nT3Fonts; ++i) 
        delete t3FontCache[i];
    nT3Fonts = 0;
}

void XWRasterOutputDev::startPage(int , XWGraphixState *state)
{
  int w, h;
  double *ctm;
  double mat[6];
  uchar color[4];
  XWDocSetting docsetting;

  if (state) {
    setupScreenParams(state->getHDPI(), state->getVDPI());
    w = (int)(state->getPageWidth() + 0.5);
    if (w <= 0) {
      w = 1;
    }
    h = (int)(state->getPageHeight() + 0.5);
    if (h <= 0) {
      h = 1;
    }
  } else {
    w = h = 1;
  }
  
  if (raster) {
    delete raster;
    raster = NULL;
  }
  if (!bitmap || w != bitmap->getWidth() || h != bitmap->getHeight()) {
    if (bitmap) {
      delete bitmap;
      bitmap = NULL;
    }
    bitmap = new XWBitmap(w, h, bitmapRowPad, colorMode, colorMode != COLOR_MODE_MONO1, bitmapTopDown);
  }
  
  raster = new XWRaster(bitmap, vectorAntialias, &screenParams);
  raster->setMinLineWidth(0.0);
  if (state) {
    ctm = state->getCTM();
    mat[0] = (double)ctm[0];
    mat[1] = (double)ctm[1];
    mat[2] = (double)ctm[2];
    mat[3] = (double)ctm[3];
    mat[4] = (double)ctm[4];
    mat[5] = (double)ctm[5];
    raster->setMatrix(mat);
  }
  switch (colorMode) {
  case COLOR_MODE_MONO1:
  case COLOR_MODE_MONO8:
    color[0] = 0;
    break;
  case COLOR_MODE_RGB8:
  case COLOR_MODE_BGR8:
    color[0] = color[1] = color[2] = 0;
    break;
  case COLOR_MODE_CMYK8:
    color[0] = color[1] = color[2] = color[3] = 0;
    break;
  }
  raster->setStrokePattern(new XWSolidColor(color));
  raster->setFillPattern(new XWSolidColor(color));
  raster->setLineCap(LINE_CAP_BUTT);
  raster->setLineJoin(LINE_JOIN_MITER);
  raster->setLineDash(NULL, 0, 0);
  raster->setMiterLimit(10);
  raster->setFlatness(1);
  // the SA parameter supposedly defaults to false, but Acrobat
  // apparently hardwires it to true  
  raster->setStrokeAdjust(docsetting.getStrokeAdjust());
  raster->clear(paperColor, 0);
}

void XWRasterOutputDev::stroke(XWGraphixState *state)
{
   XWRasterPath *path;

  if (state->getStrokeColorSpace()->isNonMarking()) {
    return;
  }
  setOverprintMask(state->getStrokeColorSpace(), state->getStrokeOverprint(),
		   state->getOverprintMode(), state->getStrokeColor());
  path = convertPath(state, state->getPath(), false);
  raster->stroke(path);
  delete path;
}

XWBitmap * XWRasterOutputDev::takeBitmap()
{
    XWBitmap * ret = bitmap;
    bitmap = new XWBitmap(1, 1, bitmapRowPad, colorMode, colorMode != COLOR_MODE_MONO1, bitmapTopDown);
    return ret;
}

void XWRasterOutputDev::tilingPatternFill(XWGraphixState *state, 
		                                      XWGraphix *gfx, 
		                                      XWObject *str,
				                                  int , 
				                                  XWDict *resDict,
				                                  double *mat, 
				                                  double *bbox,
				                                  int x0, 
				                                  int y0, 
				                                  int x1, 
				                                  int y1,
				                                  double xStep, 
				                                  double yStep)
{
	double tileXMin, tileYMin, tileXMax, tileYMax, tx, ty;
  int tileX0, tileY0, tileW, tileH, tileSize;
  XWBitmap *origBitmap, *tileBitmap;
  XWRaster *origSplash;
  uchar color[4];
  double mat1[6];
  double xa, ya, xb, yb, xc, yc;
  int x, y, xx, yy, i;

  // transform the four corners of the bbox from pattern space to
  // device space and compute the device space bbox
  state->transform(bbox[0] * mat[0] + bbox[1] * mat[2] + mat[4],
		   bbox[0] * mat[1] + bbox[1] * mat[3] + mat[5],
		   &tx, &ty);
  tileXMin = tileXMax = tx;
  tileYMin = tileYMax = ty;
  state->transform(bbox[2] * mat[0] + bbox[1] * mat[2] + mat[4],
		   bbox[2] * mat[1] + bbox[1] * mat[3] + mat[5],
		   &tx, &ty);
  if (tx < tileXMin) {
    tileXMin = tx;
  } else if (tx > tileXMax) {
    tileXMax = tx;
  }
  if (ty < tileYMin) {
    tileYMin = ty;
  } else if (ty > tileYMax) {
    tileYMax = ty;
  }
  state->transform(bbox[2] * mat[0] + bbox[3] * mat[2] + mat[4],
		   bbox[2] * mat[1] + bbox[3] * mat[3] + mat[5],
		   &tx, &ty);
  if (tx < tileXMin) {
    tileXMin = tx;
  } else if (tx > tileXMax) {
    tileXMax = tx;
  }
  if (ty < tileYMin) {
    tileYMin = ty;
  } else if (ty > tileYMax) {
    tileYMax = ty;
  }
  state->transform(bbox[0] * mat[0] + bbox[3] * mat[2] + mat[4],
		   bbox[0] * mat[1] + bbox[3] * mat[3] + mat[5],
		   &tx, &ty);
  if (tx < tileXMin) {
    tileXMin = tx;
  } else if (tx > tileXMax) {
    tileXMax = tx;
  }
  if (ty < tileYMin) {
    tileYMin = ty;
  } else if (ty > tileYMax) {
    tileYMax = ty;
  }
  if (tileXMin == tileXMax || tileYMin == tileYMax) {
    return;
  }

  tileX0 = (int)floor(tileXMin);
  tileY0 = (int)floor(tileYMin);
  tileW = (int)ceil(tileXMax) - tileX0;
  tileH = (int)ceil(tileYMax) - tileY0;

  // check for an excessively large tile size
  tileSize = tileW * tileH;
  if (tileSize > 1000000 || tileSize < 0) {
    mat1[0] = mat[0];
    mat1[1] = mat[1];
    mat1[2] = mat[2];
    mat1[3] = mat[3];
    for (y = y0; y < y1; ++y) {
      for (x = x0; x < x1; ++x) {
	xa = x * xStep;
	ya = y * yStep;
	mat1[4] = xa * mat[0] + ya * mat[2] + mat[4];
	mat1[5] = xa * mat[1] + ya * mat[3] + mat[5];
	gfx->drawForm(str, resDict, mat1, bbox);
      }
    }
    return;
  }

  // create a temporary bitmap
  origBitmap = bitmap;
  origSplash = raster;
  bitmap = tileBitmap = new XWBitmap(tileW, tileH, bitmapRowPad,
					 colorMode, true, bitmapTopDown);
  raster = new XWRaster(bitmap, vectorAntialias, origSplash->getScreen());
  raster->setMinLineWidth(0.0);
  for (i = 0; i < MAX_COLOR_COMPS; ++i) {
    color[i] = 0;
  }
  raster->clear(color);
  ++nestCount;

  // copy the fill color (for uncolored tiling patterns)
  // (and stroke color, to handle buggy PDF files)
  raster->setFillPattern(origSplash->getFillPattern()->copy());
  raster->setStrokePattern(origSplash->getStrokePattern()->copy());

  // render the tile
  state->shiftCTM(-tileX0, -tileY0);
  updateCTM(state, 0, 0, 0, 0, 0, 0);
  gfx->drawForm(str, resDict, mat, bbox);
  state->shiftCTM(tileX0, tileY0);
  updateCTM(state, 0, 0, 0, 0, 0, 0);

  // restore the original bitmap
  --nestCount;
  delete raster;
  bitmap = origBitmap;
  raster = origSplash;
  raster->setOverprintMask(0xffffffff);

  // draw the tiles
  for (y = y0; y < y1; ++y) {
    for (x = x0; x < x1; ++x) {
      xa = x * xStep;
      ya = y * yStep;
      xb = xa * mat[0] + ya * mat[2];
      yb = xa * mat[1] + ya * mat[3];
      state->transformDelta(xb, yb, &xc, &yc);
      xx = (int)(xc + tileX0 + 0.5);
      yy = (int)(yc + tileY0 + 0.5);
      raster->composite(tileBitmap, 0, 0, xx, yy, tileW, tileH,
			false, false);
    }
  }

  delete tileBitmap;
}

void XWRasterOutputDev::type3D0(XWGraphixState *, double, double)
{
	haveT3Dx = true;
}

void XWRasterOutputDev::type3D1(XWGraphixState *state, double , double ,
		                        double llx, double lly, double urx, double ury)
{
   double *ctm;
  T3FontCache *t3Font;
  uchar color[4];
  double xt, yt, xMin, xMax, yMin, yMax, x1, y1;
  int i, j;

  // ignore multiple d0/d1 operators
  if (haveT3Dx) {
    return;
  }
  haveT3Dx = true;

  t3Font = t3GlyphStack->cache;

  // check for a valid bbox
  state->transform(0, 0, &xt, &yt);
  state->transform(llx, lly, &x1, &y1);
  xMin = xMax = x1;
  yMin = yMax = y1;
  state->transform(llx, ury, &x1, &y1);
  if (x1 < xMin) {
    xMin = x1;
  } else if (x1 > xMax) {
    xMax = x1;
  }
  if (y1 < yMin) {
    yMin = y1;
  } else if (y1 > yMax) {
    yMax = y1;
  }
  state->transform(urx, lly, &x1, &y1);
  if (x1 < xMin) {
    xMin = x1;
  } else if (x1 > xMax) {
    xMax = x1;
  }
  if (y1 < yMin) {
    yMin = y1;
  } else if (y1 > yMax) {
    yMax = y1;
  }
  state->transform(urx, ury, &x1, &y1);
  if (x1 < xMin) {
    xMin = x1;
  } else if (x1 > xMax) {
    xMax = x1;
  }
  if (y1 < yMin) {
    yMin = y1;
  } else if (y1 > yMax) {
    yMax = y1;
  }
  if (xMin - xt < t3Font->glyphX ||
      yMin - yt < t3Font->glyphY ||
      xMax - xt > t3Font->glyphX + t3Font->glyphW ||
      yMax - yt > t3Font->glyphY + t3Font->glyphH) {
    if (t3Font->validBBox) {
      xwApp->error("Bad bounding box in Type 3 glyph");
    }
    return;
  }

  // allocate a cache entry
  i = (t3GlyphStack->code & (t3Font->cacheSets - 1)) * t3Font->cacheAssoc;
  for (j = 0; j < t3Font->cacheAssoc; ++j) {
    if ((t3Font->cacheTags[i+j].mru & 0x7fff) == t3Font->cacheAssoc - 1) {
      t3Font->cacheTags[i+j].mru = 0x8000;
      t3Font->cacheTags[i+j].code = t3GlyphStack->code;
      t3GlyphStack->cacheTag = &t3Font->cacheTags[i+j];
      t3GlyphStack->cacheData = t3Font->cacheData + (i+j) * t3Font->glyphSize;
    } else {
      ++t3Font->cacheTags[i+j].mru;
    }
  }

  // save state
  t3GlyphStack->origBitmap = bitmap;
  t3GlyphStack->origSplash = raster;
  ctm = state->getCTM();
  t3GlyphStack->origCTM4 = ctm[4];
  t3GlyphStack->origCTM5 = ctm[5];

  // create the temporary bitmap
  if (colorMode == COLOR_MODE_MONO1) {
    bitmap = new XWBitmap(t3Font->glyphW, t3Font->glyphH, 1, COLOR_MODE_MONO1, false);
    raster = new XWRaster(bitmap, false, t3GlyphStack->origSplash->getScreen());
    color[0] = 0;
    raster->clear(color);
    color[0] = 0xff;
  } else {
    bitmap = new XWBitmap(t3Font->glyphW, t3Font->glyphH, 1, COLOR_MODE_MONO8, false);
    raster = new XWRaster(bitmap, vectorAntialias,	t3GlyphStack->origSplash->getScreen());
    color[0] = 0x00;
    raster->clear(color);
    color[0] = 0xff;
  }
  raster->setMinLineWidth(0.0);
  raster->setFillPattern(new XWSolidColor(color));
  raster->setStrokePattern(new XWSolidColor(color));
  //~ this should copy other state from t3GlyphStack->origSplash?
  //~ [this is likely the same situation as in beginTransparencyGroup()]
  state->setCTM(ctm[0], ctm[1], ctm[2], ctm[3],
		-t3Font->glyphX, -t3Font->glyphY);
  updateCTM(state, 0, 0, 0, 0, 0, 0);
  ++nestCount;
}

void XWRasterOutputDev::updateAll(XWGraphixState *state)
{
  updateLineDash(state);
  updateLineJoin(state);
  updateLineCap(state);
  updateLineWidth(state);
  updateFlatness(state);
  updateMiterLimit(state);
  updateStrokeAdjust(state);
  updateFillColor(state);
  updateStrokeColor(state);
  needFontUpdate = true;
}

void XWRasterOutputDev::updateBlendMode(XWGraphixState *state)
{
	raster->setBlendFunc(rasterOutBlendFuncs[state->getBlendMode()]);
}

void XWRasterOutputDev::updateCTM(XWGraphixState *state, double , double,
			                      double , double , double , double )
{
  double *ctm;
  double mat[6];

  ctm = state->getCTM();
  mat[0] = (double)ctm[0];
  mat[1] = (double)ctm[1];
  mat[2] = (double)ctm[2];
  mat[3] = (double)ctm[3];
  mat[4] = (double)ctm[4];
  mat[5] = (double)ctm[5];
  raster->setMatrix(mat);
}

void XWRasterOutputDev::updateFillColor(XWGraphixState *state)
{
  CoreGray gray;
  CoreRGB rgb;
  CoreCMYK cmyk;

  switch (colorMode) {
  case COLOR_MODE_MONO1:
  case COLOR_MODE_MONO8:
    state->getFillGray(&gray);
    raster->setFillPattern(getColor(gray));
    break;
    
  case COLOR_MODE_RGB8:
  case COLOR_MODE_BGR8:
    state->getFillRGB(&rgb);
    raster->setFillPattern(getColor(&rgb));
    break;
    
  case COLOR_MODE_CMYK8:
    state->getFillCMYK(&cmyk);
    raster->setFillPattern(getColor(&cmyk));
    break;
  }
}

void XWRasterOutputDev::updateFillOpacity(XWGraphixState *state)
{
    raster->setFillAlpha(state->getFillOpacity());
}

void XWRasterOutputDev::updateFlatness(XWGraphixState *)
{
//    raster->setFlatness(state->getFlatness());
}

void XWRasterOutputDev::updateFont(XWGraphixState *)
{
    needFontUpdate = true;
}

void XWRasterOutputDev::updateLineCap(XWGraphixState *state)
{
    raster->setLineCap(state->getLineCap());
}

void XWRasterOutputDev::updateLineDash(XWGraphixState *state)
{
  double *dashPattern;
  int dashLength;
  double dashStart;
  double dash[20];
  int i;

  state->getLineDash(&dashPattern, &dashLength, &dashStart);
  if (dashLength > 20) {
    dashLength = 20;
  }
  for (i = 0; i < dashLength; ++i) {
    dash[i] = (double)dashPattern[i];
    if (dash[i] < 0) {
      dash[i] = 0;
    }
  }
  raster->setLineDash(dash, dashLength, (double)dashStart);
}

void XWRasterOutputDev::updateLineJoin(XWGraphixState *state)
{
    raster->setLineJoin(state->getLineJoin());
}

void XWRasterOutputDev::updateLineWidth(XWGraphixState *state)
{
    raster->setLineWidth(state->getLineWidth());
}

void XWRasterOutputDev::updateMiterLimit(XWGraphixState *state)
{
    raster->setMiterLimit(state->getMiterLimit());
}

void XWRasterOutputDev::updateStrokeAdjust(XWGraphixState *)
{
}

void XWRasterOutputDev::updateStrokeColor(XWGraphixState *state)
{
  CoreGray gray;
  CoreRGB rgb;
  CoreCMYK cmyk;

  switch (colorMode) {
  case COLOR_MODE_MONO1:
  case COLOR_MODE_MONO8:
    state->getStrokeGray(&gray);
    raster->setStrokePattern(getColor(gray));
    break;
    
  case COLOR_MODE_RGB8:
  case COLOR_MODE_BGR8:
    state->getStrokeRGB(&rgb);
    raster->setStrokePattern(getColor(&rgb));
    break;
    
  case COLOR_MODE_CMYK8:
    state->getStrokeCMYK(&cmyk);
    raster->setStrokePattern(getColor(&cmyk));
    break;
  }
}

void XWRasterOutputDev::updateStrokeOpacity(XWGraphixState *state)
{
    raster->setStrokeAlpha(state->getStrokeOpacity());
}

void XWRasterOutputDev::updateTransfer(XWGraphixState *state)
{
	XWFunction **transfer;
  uchar red[256], green[256], blue[256], gray[256];
  double x, y;
  int i;

  transfer = state->getTransfer();
  if (transfer[0] &&
      transfer[0]->getInputSize() == 1 &&
      transfer[0]->getOutputSize() == 1) {
    if (transfer[1] &&
	transfer[1]->getInputSize() == 1 &&
	transfer[1]->getOutputSize() == 1 &&
	transfer[2] &&
	transfer[2]->getInputSize() == 1 &&
	transfer[2]->getOutputSize() == 1 &&
	transfer[3] &&
	transfer[3]->getInputSize() == 1 &&
	transfer[3]->getOutputSize() == 1) {
      for (i = 0; i < 256; ++i) {
	x = i / 255.0;
	transfer[0]->transform(&x, &y);
	red[i] = (uchar)(y * 255.0 + 0.5);
	transfer[1]->transform(&x, &y);
	green[i] = (uchar)(y * 255.0 + 0.5);
	transfer[2]->transform(&x, &y);
	blue[i] = (uchar)(y * 255.0 + 0.5);
	transfer[3]->transform(&x, &y);
	gray[i] = (uchar)(y * 255.0 + 0.5);
      }
    } else {
      for (i = 0; i < 256; ++i) {
	x = i / 255.0;
	transfer[0]->transform(&x, &y);
	red[i] = green[i] = blue[i] = gray[i] = (uchar)(y * 255.0 + 0.5);
      }
    }
  } else {
    for (i = 0; i < 256; ++i) {
      red[i] = green[i] = blue[i] = gray[i] = (uchar)i;
    }
  }
  raster->setTransfer(red, green, blue, gray);
}

bool XWRasterOutputDev::alphaImageSrc(void *data, uchar * colorLine, uchar *alphaLine)
{
  RasterOutImageData *imgData = (RasterOutImageData *)data;
  uchar *p, *aq;
  uchar * q, * col;
  CoreRGB rgb;
  CoreGray gray;
  CoreCMYK cmyk;
  uchar alpha;
  int nComps, x, i;

  if (imgData->y == imgData->height) {
    return false;
  }
  if (!(p = imgData->imgStr->getLine())) {
    return false;
  }

  nComps = imgData->colorMap->getNumPixelComps();

  for (x = 0, q = colorLine, aq = alphaLine;
       x < imgData->width;
       ++x, p += nComps) {
    alpha = 0;
    for (i = 0; i < nComps; ++i) {
      if (p[i] < imgData->maskColors[2*i] ||
	  p[i] > imgData->maskColors[2*i+1]) {
	alpha = 0xff;
	break;
      }
    }
    if (imgData->lookup) {
      switch (imgData->colorMode) {
      case COLOR_MODE_MONO1:
      case COLOR_MODE_MONO8:
	*q++ = imgData->lookup[*p];
	break;
	
      case COLOR_MODE_RGB8:
      case COLOR_MODE_BGR8:
	col = &imgData->lookup[3 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	break;
	
      case COLOR_MODE_CMYK8:
	col = &imgData->lookup[4 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	*q++ = col[3];
	break;
      }
      *aq++ = alpha;
    } else {
      switch (imgData->colorMode) {
      case COLOR_MODE_MONO1:
      case COLOR_MODE_MONO8:
	imgData->colorMap->getGray(p, &gray);
	*q++ = colToByte(gray);
	break;
	
      case COLOR_MODE_RGB8:
      case COLOR_MODE_BGR8:
	imgData->colorMap->getRGB(p, &rgb);
	*q++ = colToByte(rgb.r);
	*q++ = colToByte(rgb.g);
	*q++ = colToByte(rgb.b);
	break;
	
      case COLOR_MODE_CMYK8:
	imgData->colorMap->getCMYK(p, &cmyk);
	*q++ = colToByte(cmyk.c);
	*q++ = colToByte(cmyk.m);
	*q++ = colToByte(cmyk.y);
	*q++ = colToByte(cmyk.k);
	break;
      }
      *aq++ = alpha;
    }
  }

  ++imgData->y;
  return true;
}

XWRasterPath * XWRasterOutputDev::convertPath(XWGraphixState *, XWPath *path, bool dropEmptySubpaths)
{
  XWRasterPath *sPath;
  XWSubPath *subpath;
  int n, i, j;

  n = dropEmptySubpaths ? 1 : 0;
  sPath = new XWRasterPath();
  for (i = 0; i < path->getNumSubpaths(); ++i) {
    subpath = path->getSubpath(i);
    if (subpath->getNumPoints() > n) {
      sPath->moveTo((double)subpath->getX(0),
		    (double)subpath->getY(0));
      j = 1;
      while (j < subpath->getNumPoints()) {
	if (subpath->getCurve(j)) {
	  sPath->curveTo((double)subpath->getX(j),
			 (double)subpath->getY(j),
			 (double)subpath->getX(j+1),
			 (double)subpath->getY(j+1),
			 (double)subpath->getX(j+2),
			 (double)subpath->getY(j+2));
	  j += 3;
	} else {
	  sPath->lineTo((double)subpath->getX(j),
			(double)subpath->getY(j));
	  ++j;
	}
      }
      if (subpath->isClosed()) {
	sPath->close();
      }
    }
  }
  return sPath;
}

void XWRasterOutputDev::doUpdateFont(XWGraphixState *state)
{
  XWCoreFont *gfxFont;
  XWFontLoc *fontLoc;
  CoreFontType fontType;
  RasterOutFontFileID *id;
  XWRasterFontFile *fontFile;
  int fontNum;
  XWFontFileTrueType *ff;
  ObjRef embRef;
  XWObject refObj, strObj;
  XWString *tmpFileName, *fileName;
  QFile *tmpFile;
  int *codeToGID;
  XWCharCodeToUnicode *ctu;
  double *textMat;
  double m11, m12, m21, m22, fontSize;
  double w, fontScaleMin, fontScaleAvg, fontScale;
  ushort ww;
  double mat[4];
  char *name;
  uint uBuf[8];
  int c, substIdx, n, code, cmap, i;

  needFontUpdate = false;
  font = NULL;
  tmpFileName = NULL;
  substIdx = -1;

  if (!(gfxFont = state->getFont())) {
    goto err1;
  }
  
  fontType = gfxFont->getType();
  if (fontType == fontType3) {
    goto err1;
  }

  // sanity-check the font size - skip anything larger than 10 inches
  // (this avoids problems allocating memory for the font cache)
  if (state->getTransformedFontSize()
        > 10 * (state->getHDPI() + state->getVDPI())) {
    goto err1;
  }

  // check the font file cache
  id = new RasterOutFontFileID(gfxFont->getID());
  if ((fontFile = fontEngine->getFontFile(id))) {
    delete id;

  } else {

    fileName = NULL;
    fontNum = 0;

    if (!(fontLoc = gfxFont->locateFont(xref, false))) {
      goto err2;
    }

    // embedded font
    if (fontLoc->locType == fontLocEmbedded) {
      gfxFont->getEmbeddedFontID(&embRef);
      tmpFile = xwApp->openTmpFile();      
      if (!tmpFile) {
	xwApp->error("Couldn't create temporary font file");
	delete fontLoc;
	goto err2;
      }
      QByteArray ba = QFile::encodeName(tmpFile->fileName());
      tmpFileName = new XWString(ba.data());
      refObj.initRef(embRef.num, embRef.gen);
      refObj.fetch(xref, &strObj);
      refObj.free();
      if (!strObj.isStream()) {
	xwApp->error("Embedded font object is wrong type");
	strObj.free();
	tmpFile->close();
	delete tmpFile;
	delete fontLoc;
	goto err2;
      }
      strObj.streamReset();
      while ((c = strObj.streamGetChar()) != EOF) {
	tmpFile->putChar(c);
      }
      strObj.streamClose();
      strObj.free();
      tmpFile->close();      
      fileName = tmpFileName;
      delete tmpFile;

    // external font
    } else { // gfxFontLocExternal
      fileName = fontLoc->path;
      fontNum = fontLoc->fontNum;
      if (fontLoc->substIdx >= 0) {
	id->setSubstIdx(fontLoc->substIdx);
      }
    }

    // load the font file
    switch (fontLoc->fontType) {
    case fontType1:
      if (!(fontFile = fontEngine->loadType1Font(
		   id,
		   fileName->getCString(),
		   fileName == tmpFileName,
		   (char **)((XWCore8BitFont *)gfxFont)->getEncoding()))) {
	delete fontLoc;
	goto err2;
      }
      break;
    case fontType1C:
      if (!(fontFile = fontEngine->loadType1CFont(
		   id,
		   fileName->getCString(),
		   fileName == tmpFileName,
		   (char **)((XWCore8BitFont *)gfxFont)->getEncoding()))) {
	delete fontLoc;
	goto err2;
      }
      break;
    case fontType1COT:
      if (!(fontFile = fontEngine->loadOpenTypeT1CFont(
		   id,
		   fileName->getCString(),
		   fileName == tmpFileName,
		   (char **)((XWCore8BitFont *)gfxFont)->getEncoding()))) {
	delete fontLoc;
	goto err2;
      }
      break;
    case fontTrueType:
    case fontTrueTypeOT:
      if ((ff = XWFontFileTrueType::load(fileName->getCString()))) {
	codeToGID = ((XWCore8BitFont *)gfxFont)->getCodeToGIDMap(ff);
	n = 256;
	delete ff;
	// if we're substituting for a non-TrueType font, we need to mark
	// all notdef codes as "do not draw" (rather than drawing TrueType
	// notdef glyphs)
	if (gfxFont->getType() != fontTrueType &&
	    gfxFont->getType() != fontTrueTypeOT) {
	  for (i = 0; i < 256; ++i) {
	    if (codeToGID[i] == 0) {
	      codeToGID[i] = -1;
	    }
	  }
	}
      } else {
	codeToGID = NULL;
	n = 0;
      }
      if (!(fontFile = fontEngine->loadTrueTypeFont(
			   id,
			   fileName->getCString(), fontNum,
			   fileName == tmpFileName,
			   codeToGID, n,
			   gfxFont->getEmbeddedFontName()
			     ? gfxFont->getEmbeddedFontName()->getCString()
			     : (char *)NULL))) {
	delete fontLoc;
	goto err2;
      }
      break;
    case fontCIDType0:
    case fontCIDType0C:
      if (!(fontFile = fontEngine->loadCIDFont(
			   id,
			   fileName->getCString(),
			   fileName == tmpFileName))) {
	delete fontLoc;
	goto err2;
      }
      break;
    case fontCIDType0COT:
      if (((XWCoreCIDFont *)gfxFont)->getCIDToGID()) {
	n = ((XWCoreCIDFont *)gfxFont)->getCIDToGIDLen();
	codeToGID = (int *)malloc(n * sizeof(int));
	memcpy(codeToGID, ((XWCoreCIDFont *)gfxFont)->getCIDToGID(),
	       n * sizeof(int));
      } else {
	codeToGID = NULL;
	n = 0;
      }
      if (!(fontFile = fontEngine->loadOpenTypeCFFFont(
			   id,
			   fileName->getCString(),
			   fileName == tmpFileName,
			   codeToGID, n))) {
	delete fontLoc;
	goto err2;
      }
      break;
    case fontCIDType2:
    case fontCIDType2OT:
      codeToGID = NULL;
      n = 0;
      if (fontLoc->locType == fontLocEmbedded) {
	if (((XWCoreCIDFont *)gfxFont)->getCIDToGID()) {
	  n = ((XWCoreCIDFont *)gfxFont)->getCIDToGIDLen();
	  codeToGID = (int *)malloc(n * sizeof(int));
	  memcpy(codeToGID, ((XWCoreCIDFont *)gfxFont)->getCIDToGID(),
		 n * sizeof(int));
	}
      } else {
	// create a CID-to-GID mapping, via Unicode
	if ((ctu = ((XWCoreCIDFont *)gfxFont)->getToUnicode())) {
	  //~ this should use fontNum to load the correct font
	  if ((ff = XWFontFileTrueType::load(fileName->getCString()))) {
	    // look for a Unicode cmap
	    for (cmap = 0; cmap < ff->getNumCmaps(); ++cmap) {
	      if ((ff->getCmapPlatform(cmap) == 3 &&
		   ff->getCmapEncoding(cmap) == 1) ||
		  ff->getCmapPlatform(cmap) == 0) {
		break;
	      }
	    }
	    if (cmap < ff->getNumCmaps()) {
	      // map CID -> Unicode -> GID
	      n = ctu->getLength();
	      codeToGID = (int *)malloc(n * sizeof(int));
	      for (code = 0; code < n; ++code) {
		if (ctu->mapToUnicode(code, uBuf, 8) > 0) {
		  codeToGID[code] = ff->mapCodeToGID(cmap, uBuf[0]);
		} else {
		  codeToGID[code] = -1;
		}
	      }
	    }
	    delete ff;
	  }
	  ctu->decRefCnt();
	} 
      }
      if (!(fontFile = fontEngine->loadTrueTypeFont(
			   id,
			   fileName->getCString(), fontNum,
			   fileName == tmpFileName,
			   codeToGID, n,
			   gfxFont->getEmbeddedFontName()
			     ? gfxFont->getEmbeddedFontName()->getCString()
			     : (char *)NULL))) {
	delete fontLoc;
	goto err2;
      }
      break;
    default:
      // this shouldn't happen
      goto err2;
    }

    delete fontLoc;
  }

  // get the font matrix
  textMat = state->getTextMat();
  fontSize = state->getFontSize();
  m11 = textMat[0] * fontSize * state->getHorizScaling();
  m12 = textMat[1] * fontSize * state->getHorizScaling();
  m21 = textMat[2] * fontSize;
  m22 = textMat[3] * fontSize;

  // for substituted fonts: adjust the font matrix -- compare the
  // widths of letters and digits (A-Z, a-z, 0-9) in the original font
  // and the substituted font
  substIdx = ((RasterOutFontFileID *)fontFile->getID())->getSubstIdx();
  if (substIdx >= 0 && substIdx < 12) {
    fontScaleMin = 1;
    fontScaleAvg = 0;
    n = 0;
    for (code = 0; code < 256; ++code) {
      if ((name = ((XWCore8BitFont *)gfxFont)->getCharName(code)) &&
	  name[0] && !name[1] &&
	  ((name[0] >= 'A' && name[0] <= 'Z') ||
	   (name[0] >= 'a' && name[0] <= 'z') ||
	   (name[0] >= '0' && name[0] <= '9'))) {
	w = ((XWCore8BitFont *)gfxFont)->getWidth(code);
	builtinFontSubst[substIdx]->widths->getWidth(name, &ww);
	if (w > 0.01 && ww > 10) {
	  w /= ww * 0.001;
	  if (w < fontScaleMin) {
	    fontScaleMin = w;
	  }
	  fontScaleAvg += w;
	  ++n;
	}
      }
    }
    // if real font is narrower than substituted font, reduce the font
    // size accordingly -- this currently uses a scale factor halfway
    // between the minimum and average computed scale factors, which
    // is a bit of a kludge, but seems to produce mostly decent
    // results
    if (n) {
      fontScaleAvg /= n;
      if (fontScaleAvg < 1) {
	fontScale = 0.5 * (fontScaleMin + fontScaleAvg);
	m11 *= fontScale;
	m12 *= fontScale;
      }
    }
  }

  // create the scaled font
  mat[0] = m11;  mat[1] = m12;
  mat[2] = m21;  mat[3] = m22;
  font = fontEngine->getFont(fontFile, mat, raster->getMatrix());

  if (tmpFileName) {
    delete tmpFileName;
  }
  return;

 err2:
  delete id;
 err1:
  if (tmpFileName) {
  QString fn = tmpFileName->toQString();
    QFile::remove(fn);
    delete tmpFileName;
  }
  return;
}

void XWRasterOutputDev::drawType3Glyph(XWGraphixState *state, 
                        T3FontCache *t3Font,
				                T3FontCacheTag *, 
				                uchar *data)
{
  GlyphBitmap glyph;

  setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), state->getFillColor());
  glyph.x = -t3Font->glyphX;
  glyph.y = -t3Font->glyphY;
  glyph.w = t3Font->glyphW;
  glyph.h = t3Font->glyphH;
  glyph.aa = colorMode != COLOR_MODE_MONO1;
  glyph.data = data;
  glyph.freeData = false;
  raster->fillGlyph(0, 0, &glyph);
}

XWRasterPattern * XWRasterOutputDev::getColor(CoreGray gray)
{
  uchar color[4];

  if (reverseVideo) {
    gray = COLOR_COMP1 - gray;
  }
  color[0] = colToByte(gray);
  return new XWSolidColor(color);
}

XWRasterPattern * XWRasterOutputDev::getColor(CoreRGB *rgb)
{
	int r, g, b;
  uchar color[4];

  if (reverseVideo) {
    r = COLOR_COMP1 - rgb->r;
    g = COLOR_COMP1 - rgb->g;
    b = COLOR_COMP1 - rgb->b;
  } else {
    r = rgb->r;
    g = rgb->g;
    b = rgb->b;
  }
  color[0] = colToByte(r);
  color[1] = colToByte(g);
  color[2] = colToByte(b);
  return new XWSolidColor(color);
}

XWRasterPattern * XWRasterOutputDev::getColor(CoreCMYK *cmyk)
{
	uchar color[4];

  color[0] = colToByte(cmyk->c);
  color[1] = colToByte(cmyk->m);
  color[2] = colToByte(cmyk->y);
  color[3] = colToByte(cmyk->k);
  return new XWSolidColor(color);
}

bool XWRasterOutputDev::imageMaskSrc(void *data, uchar * line)
{
	RasterOutImageMaskData *imgMaskData = (RasterOutImageMaskData *)data;
  uchar *p;
  uchar* q;
  int x;

  if (imgMaskData->y == imgMaskData->height) {
    return false;
  }
  if (!(p = imgMaskData->imgStr->getLine())) {
    return false;
  }
  for (x = 0, q = line; x < imgMaskData->width; ++x) {
    *q++ = *p++ ^ imgMaskData->invert;
  }
  ++imgMaskData->y;
  return true;
}

bool XWRasterOutputDev::imageSrc(void *data, uchar * colorLine, uchar *)
{
  RasterOutImageData *imgData = (RasterOutImageData *)data;
  uchar *p;
  uchar * q, *col;
  CoreRGB rgb;
  CoreGray gray;
  CoreCMYK cmyk;
  int nComps, x;

  if (imgData->y == imgData->height) {
    return false;
  }
  if (!(p = imgData->imgStr->getLine())) {
    return false;
  }

  nComps = imgData->colorMap->getNumPixelComps();

  if (imgData->lookup) {
    switch (imgData->colorMode) {
    case COLOR_MODE_MONO1:
    case COLOR_MODE_MONO8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, ++p) {
	*q++ = imgData->lookup[*p];
      }
      break;
      
    case COLOR_MODE_RGB8:
    case COLOR_MODE_BGR8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, ++p) {
	col = &imgData->lookup[3 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
      }
      break;
      
    case COLOR_MODE_CMYK8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, ++p) {
	col = &imgData->lookup[4 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	*q++ = col[3];
      }
      break;
    }
  } else {
    switch (imgData->colorMode) {
    case COLOR_MODE_MONO1:
    case COLOR_MODE_MONO8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, p += nComps) {
	imgData->colorMap->getGray(p, &gray);
	*q++ = colToByte(gray);
      }
      break;
      
    case COLOR_MODE_RGB8:
    case COLOR_MODE_BGR8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, p += nComps) {
	imgData->colorMap->getRGB(p, &rgb);
	*q++ = colToByte(rgb.r);
	*q++ = colToByte(rgb.g);
	*q++ = colToByte(rgb.b);
      }
      break;
      
    case COLOR_MODE_CMYK8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, p += nComps) {
	imgData->colorMap->getCMYK(p, &cmyk);
	*q++ = colToByte(cmyk.c);
	*q++ = colToByte(cmyk.m);
	*q++ = colToByte(cmyk.y);
	*q++ = colToByte(cmyk.k);
      }
      break;
    }
  }

  ++imgData->y;
  return true;
}

bool XWRasterOutputDev::maskedImageSrc(void *data, uchar * colorLine, uchar *alphaLine)
{
  RasterOutMaskedImageData *imgData = (RasterOutMaskedImageData *)data;
  uchar *p, *aq;
  uchar * q, *col;
  CoreRGB rgb;
  CoreGray gray;
  CoreCMYK cmyk;
  uchar alpha;
  uchar *maskPtr;
  int maskBit;
  int nComps, x;

  if (imgData->y == imgData->height) {
    return false;
  }
  if (!(p = imgData->imgStr->getLine())) {
    return false;
  }

  nComps = imgData->colorMap->getNumPixelComps();

  maskPtr = imgData->mask->getDataPtr() +
              imgData->y * imgData->mask->getRowSize();
  maskBit = 0x80;
  for (x = 0, q = colorLine, aq = alphaLine;
       x < imgData->width;
       ++x, p += nComps) {
    alpha = (*maskPtr & maskBit) ? 0xff : 0x00;
    if (!(maskBit >>= 1)) {
      ++maskPtr;
      maskBit = 0x80;
    }
    if (imgData->lookup) {
      switch (imgData->colorMode) {
      case COLOR_MODE_MONO1:
      case COLOR_MODE_MONO8:
	*q++ = imgData->lookup[*p];
	break;
	
      case COLOR_MODE_RGB8:
      case COLOR_MODE_BGR8:
	col = &imgData->lookup[3 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	break;
	
      case COLOR_MODE_CMYK8:
	col = &imgData->lookup[4 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	*q++ = col[3];
	break;
      }
      *aq++ = alpha;
    } else {
      switch (imgData->colorMode) {
      case COLOR_MODE_MONO1:
      case COLOR_MODE_MONO8:
	imgData->colorMap->getGray(p, &gray);
	*q++ = colToByte(gray);
	break;
	
      case COLOR_MODE_RGB8:
      case COLOR_MODE_BGR8:
	imgData->colorMap->getRGB(p, &rgb);
	*q++ = colToByte(rgb.r);
	*q++ = colToByte(rgb.g);
	*q++ = colToByte(rgb.b);
	break;
	
      case COLOR_MODE_CMYK8:
	imgData->colorMap->getCMYK(p, &cmyk);
	*q++ = colToByte(cmyk.c);
	*q++ = colToByte(cmyk.m);
	*q++ = colToByte(cmyk.y);
	*q++ = colToByte(cmyk.k);
	break;
      }
      *aq++ = alpha;
    }
  }

  ++imgData->y;
  return true;
}

void XWRasterOutputDev::setOverprintMask(XWColorSpace *colorSpace, 
		                      bool overprintFlag,
			                    int overprintMode, 
			                    CoreColor *singleColor)
{
	uint mask;
  CoreCMYK cmyk;

	XWDocSetting docsetting;
  if (overprintFlag && docsetting.getOverprintPreview()) {
    mask = colorSpace->getOverprintMask();
    if (singleColor && overprintMode &&
	(colorSpace->getMode() == COLOR_SPACE_DEVICECMYK ||
	 (colorSpace->getMode() == COLOR_SPACE_ICCBASED &&
	  colorSpace->getNComps() == 4))) {
      colorSpace->getCMYK(singleColor, &cmyk);
      if (cmyk.c == 0) {
	mask &= ~1;
      }
      if (cmyk.m == 0) {
	mask &= ~2;
      }
      if (cmyk.y == 0) {
	mask &= ~4;
      }
      if (cmyk.k == 0) {
	mask &= ~8;
      }
    }
  } else {
    mask = 0xffffffff;
  }
  raster->setOverprintMask(mask);
}

void XWRasterOutputDev::setupScreenParams(double hDPI, double vDPI)
{
	XWDocSetting docsetting;
	
	screenParams.size = docsetting.getScreenSize();
  	screenParams.dotRadius = docsetting.getScreenDotRadius();
  	screenParams.gamma = docsetting.getScreenGamma();
  	screenParams.blackThreshold = docsetting.getScreenBlackThreshold();
  	screenParams.whiteThreshold = docsetting.getScreenWhiteThreshold();      
    switch (docsetting.getScreenType())
    {
        case SCREEN_DISPERSED:
            screenParams.type = SCREEN_TYPE_DISPERSED;
            if (screenParams.size < 0)
                screenParams.size = 4;
            break;
            
        case SCREEN_CLUSTERED:
            screenParams.type = SCREEN_TYPE_CLUSTERED;
            if (screenParams.size < 0) 
                screenParams.size = 10;
            break;
            
        case SCREEN_STOCHASTICCLUSTERED:
            screenParams.type = SCREEN_TYPE_STOCHASTICCLUSTERED;
            if (screenParams.size < 0) 
                screenParams.size = 100;
            if (screenParams.dotRadius < 0) 
                screenParams.dotRadius = 2;
            break;
            
        case SCREEN_UNSET:
        default:
            if (hDPI > 299.9 && vDPI > 299.9) 
            {
                screenParams.type = SCREEN_TYPE_STOCHASTICCLUSTERED;
                if (screenParams.size < 0) 
	                screenParams.size = 100;
                if (screenParams.dotRadius < 0) 
	                screenParams.dotRadius = 2;
            } 
            else 
            {
                screenParams.type = SCREEN_TYPE_DISPERSED;
                if (screenParams.size < 0) 
	                screenParams.size = 4;
            }
            break;
    }
}

