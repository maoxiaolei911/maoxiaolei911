/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWRASTERFONT_H
#define XWRASTERFONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SIZES_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include "XWGlobal.h"

struct GlyphBitmap;
struct RasterFontCacheTag;
class  XWRasterFontFile;
class  XWRasterPath;

#define RasterFontCacheAssoc   8
#define RasterFontCacheMaxSets 8
#define RasterFontCacheSize    (128*1024)

#define RasterFontFractionBits 2
#define RasterFontFraction     (1 << RasterFontFractionBits)
#define RasterFontFractionMul  ((double)1 / (double)RasterFontFraction)


class XW_RASTER_EXPORT XWRasterFont
{
public:    
    XWRasterFont(XWRasterFontFile *fontFileA, 
                 double *matA,
	             double *textMatA, 
	             bool aaA);
    virtual ~XWRasterFont();
    
    void    getBBox(int *xMinA, int *yMinA, int *xMaxA, int *yMaxA)
                { *xMinA = xMin; *yMinA = yMin; *xMaxA = xMax; *yMaxA = yMax; }
    XWRasterFontFile * getFontFile() { return fontFile; }
    virtual bool       getGlyph(int c, int xFrac, int, GlyphBitmap *bitmap);
    virtual XWRasterPath   * getGlyphPath(int c) = 0;
            double   * getMatrix() { return mat; }
    
    void initCache();
    
    virtual bool makeGlyph(int c, int xFrac, int, GlyphBitmap *bitmap) = 0;
    bool matches(XWRasterFontFile *fontFileA, double *matA, double *textMatA) 
        {
            return fontFileA == fontFile &&
                    matA[0] == mat[0] && matA[1] == mat[1] &&
                    matA[2] == mat[2] && matA[3] == mat[3] &&
                    textMatA[0] == textMat[0] && textMatA[1] == textMat[1] &&
                    textMatA[2] == textMat[2] && textMatA[3] == textMat[3];
        }
        
protected:
    XWRasterFontFile *fontFile;
    
    double mat[4];
    double textMat[4];
    
    bool   aa;
    
    int xMin, yMin, xMax, yMax;
    
    uchar *cache;
    
    RasterFontCacheTag * cacheTags;
    
    int glyphW, glyphH;
    int glyphSize;
    int cacheSets;
    int cacheAssoc;
};

class XWRasterFTFontFile;


class XW_RASTER_EXPORT XWRasterFTFont: public XWRasterFont 
{
public:
    XWRasterFTFont(XWRasterFTFontFile *fontFileA, double *matA, double *textMatA);
    virtual ~XWRasterFTFont();

    virtual bool     getGlyph(int c, int xFrac, int yFrac, GlyphBitmap *bitmap);
    virtual XWRasterPath * getGlyphPath(int c);

    virtual bool makeGlyph(int c, int xFrac, int yFrac, GlyphBitmap *bitmap);

private:
    FT_Size sizeObj;
    FT_Matrix matrix;
    FT_Matrix textMatrix;
    double textScale;
};

#endif // XWRASTERFONT_H

