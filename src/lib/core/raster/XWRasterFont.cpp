/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <QtDebug>
#include "XWRasterType.h"
#include "XWRasterFontFile.h"
#include "XWRasterFontEngine.h"
#include "XWRasterPath.h"
#include "XWRasterFont.h"


struct RasterFontCacheTag 
{
    int c;
    short xFrac, yFrac;
    int mru;
    int x, y, w, h;
};

XWRasterFont::XWRasterFont(XWRasterFontFile *fontFileA, 
                           double *matA,
	                       double *textMatA, 
	                       bool aaA)
{
    fontFile = fontFileA;
    fontFile->incRefCnt();
    mat[0] = matA[0];
    mat[1] = matA[1];
    mat[2] = matA[2];
    mat[3] = matA[3];
    textMat[0] = textMatA[0];
    textMat[1] = textMatA[1];
    textMat[2] = textMatA[2];
    textMat[3] = textMatA[3];
    aa = aaA;

    cache = NULL;
    cacheTags = NULL;

    xMin = yMin = xMax = yMax = 0;
}

XWRasterFont::~XWRasterFont()
{
    fontFile->decRefCnt();
    if (cache) 
        free(cache);
    
    if (cacheTags) 
        free(cacheTags);
}

bool XWRasterFont::getGlyph(int c, int xFrac, int yFrac, GlyphBitmap *bitmap)
{
    if (!aa || glyphH > 50) 
    {
        xFrac = yFrac = 0;
    }
    
    int i = (c & (cacheSets - 1)) * cacheAssoc;
    for (int j = 0; j < cacheAssoc; ++j) 
    {
        if ((cacheTags[i+j].mru & 0x80000000) && 
	         cacheTags[i+j].c == c &&
	         (int)cacheTags[i+j].xFrac == xFrac && 
	         (int)cacheTags[i+j].yFrac == yFrac) 
	    {
            bitmap->x = cacheTags[i+j].x;
            bitmap->y = cacheTags[i+j].y;
            bitmap->w = cacheTags[i+j].w;
            bitmap->h = cacheTags[i+j].h;
            for (int k = 0; k < cacheAssoc; ++k) 
            {
	            if (k != j && 
	                (cacheTags[i+k].mru & 0x7fffffff) < (cacheTags[i+j].mru & 0x7fffffff)) 
	            {
	                ++cacheTags[i+k].mru;
	            }
            }
            
            cacheTags[i+j].mru = 0x80000000;
            bitmap->aa = aa;
            bitmap->data = cache + (i+j) * glyphSize;
            bitmap->freeData = false;
            return true;
        }
    }
    
    GlyphBitmap bitmap2;
    bitmap2.data = 0;
    if (!makeGlyph(c, xFrac, yFrac, &bitmap2)) 
        return false;
        
    if (bitmap2.w > glyphW || bitmap2.h > glyphH) 
    {
        *bitmap = bitmap2;
        return true;
    }
    
    int size = 0;
    if (aa) 
        size = bitmap2.w * bitmap2.h;
    else 
        size = ((bitmap2.w + 7) >> 3) * bitmap2.h;
    
    uchar * p = NULL;
    for (int j = 0; j < cacheAssoc; ++j) 
    {
        if ((cacheTags[i+j].mru & 0x7fffffff) == cacheAssoc - 1) 
        {
            cacheTags[i+j].mru = 0x80000000;
            cacheTags[i+j].c = c;
            cacheTags[i+j].xFrac = (short)xFrac;
            cacheTags[i+j].yFrac = (short)yFrac;
            cacheTags[i+j].x = bitmap2.x;
            cacheTags[i+j].y = bitmap2.y;
            cacheTags[i+j].w = bitmap2.w;
            cacheTags[i+j].h = bitmap2.h;
            p = cache + (i+j) * glyphSize;
            memcpy(p, bitmap2.data, size);
        } 
        else 
            ++cacheTags[i+j].mru;
    }
    *bitmap = bitmap2;
    bitmap->data = p;
    bitmap->freeData = false;
    if (bitmap2.freeData && bitmap2.data) 
        free(bitmap2.data);
        
    return true;
}

void XWRasterFont::initCache()
{
    glyphW = xMax - xMin + 3;
    glyphH = yMax - yMin + 3;
    if (aa) 
        glyphSize = glyphW * glyphH;
    else 
        glyphSize = ((glyphW + 7) >> 3) * glyphH;
        
    cacheAssoc = RasterFontCacheAssoc;
  	for (cacheSets = RasterFontCacheMaxSets; 
  			cacheSets > 1 && cacheSets * cacheAssoc * glyphSize > RasterFontCacheSize;
       		cacheSets >>= 1) ;
  	cache = (uchar *)malloc(cacheSets * cacheAssoc * glyphSize * sizeof(uchar));
  	cacheTags = (RasterFontCacheTag *)malloc(cacheSets * cacheAssoc * sizeof(RasterFontCacheTag));
  	for (int i = 0; i < cacheSets * cacheAssoc; ++i) 
  	{
    	cacheTags[i].mru = i & (cacheAssoc - 1);
  	}
}


static int glyphPathMoveTo(const FT_Vector *pt, void *path);
static int glyphPathLineTo(const FT_Vector *pt, void *path);
static int glyphPathConicTo(const FT_Vector *ctrl, const FT_Vector *pt, void *path);
static int glyphPathCubicTo(const FT_Vector *ctrl1, const FT_Vector *ctrl2, const FT_Vector *pt, void *path);


struct RasterFTFontPath 
{
    XWRasterPath *path;
    double textScale;
    bool needClose;
};

static int glyphPathMoveTo(const FT_Vector *pt, void *path)
{
    RasterFTFontPath *p = (RasterFTFontPath *)path;

    if (p->needClose) 
    {
        p->path->close();
        p->needClose = false;
    }
    
    p->path->moveTo((double)pt->x * p->textScale / 64.0, (double)pt->y * p->textScale / 64.0);
    return 0;
}

static int glyphPathLineTo(const FT_Vector *pt, void *path)
{
    RasterFTFontPath *p = (RasterFTFontPath *)path;
    
    p->path->lineTo((double)pt->x * p->textScale / 64.0, (double)pt->y * p->textScale / 64.0);
    p->needClose = true;
    return 0;
}

static int glyphPathConicTo(const FT_Vector *ctrl, const FT_Vector *pt, void *path)
{
    RasterFTFontPath *p = (RasterFTFontPath *)path;
    double x0, y0;
    if (!p->path->getCurPt(&x0, &y0)) 
        return 0;
    
    double xc = (double)ctrl->x * p->textScale / 64.0;
    double yc = (double)ctrl->y * p->textScale / 64.0;
    double x3 = (double)pt->x * p->textScale / 64.0;
    double y3 = (double)pt->y * p->textScale / 64.0;
    double x1 = (double)(1.0 / 3.0) * (x0 + (double)2 * xc);
    double y1 = (double)(1.0 / 3.0) * (y0 + (double)2 * yc);
    double x2 = (double)(1.0 / 3.0) * ((double)2 * xc + x3);
    double y2 = (double)(1.0 / 3.0) * ((double)2 * yc + y3);

    p->path->curveTo(x1, y1, x2, y2, x3, y3);
    p->needClose = true;
    return 0;
}

static int glyphPathCubicTo(const FT_Vector *ctrl1, const FT_Vector *ctrl2, const FT_Vector *pt, void *path)
{
    RasterFTFontPath *p = (RasterFTFontPath *)path;
    p->path->curveTo((double)ctrl1->x * p->textScale / 64.0,
		             (double)ctrl1->y * p->textScale / 64.0,
		             (double)ctrl2->x * p->textScale / 64.0,
		             (double)ctrl2->y * p->textScale / 64.0,
		             (double)pt->x * p->textScale / 64.0,
		             (double)pt->y * p->textScale / 64.0);
    p->needClose = true;
    return 0;
}

XWRasterFTFont::XWRasterFTFont(XWRasterFTFontFile *fontFileA, double *matA, double *textMatA)
	:XWRasterFont(fontFileA, matA, textMatA, fontFileA->engine->aa)
{
		int size, div;
  	int x, y;
    FT_Face face = fontFileA->face;
    if (FT_New_Size(face, &sizeObj)) 
        return;
    
    face->size = sizeObj;
    size = rasterRound(rasterDist(0, 0, mat[2], mat[3]));
  	if (size < 1) 
  	{
    	size = 1;
  	}
  
  	if (FT_Set_Pixel_Sizes(face, 0, size)) 
  	{
    	return;
  	}
  	// if the textMat values are too small, FreeType's fixed point
  	// arithmetic doesn't work so well
  	textScale = rasterDist(0, 0, textMat[2], textMat[3]) / size;

  	div = face->bbox.xMax > 20000 ? 65536 : 1;
  		
  	x = (int)((mat[0] * face->bbox.xMin + mat[2] * face->bbox.yMin) / (div * face->units_per_EM));
  	xMin = xMax = x;
  	y = (int)((mat[1] * face->bbox.xMin + mat[3] * face->bbox.yMin) / (div * face->units_per_EM));
  	yMin = yMax = y;
  	x = (int)((mat[0] * face->bbox.xMin + mat[2] * face->bbox.yMax) / (div * face->units_per_EM));
  	if (x < xMin) 
    	xMin = x;
  	else if (x > xMax) 
    	xMax = x;
    	
  	y = (int)((mat[1] * face->bbox.xMin + mat[3] * face->bbox.yMax) / (div * face->units_per_EM));
  	if (y < yMin) 
    	yMin = y;
  	else if (y > yMax) 
    	yMax = y;
  	x = (int)((mat[0] * face->bbox.xMax + mat[2] * face->bbox.yMin) / (div * face->units_per_EM));
  	if (x < xMin) 
    	xMin = x;
  	else if (x > xMax) 
    	xMax = x;
    	
  	y = (int)((mat[1] * face->bbox.xMax + mat[3] * face->bbox.yMin) / (div * face->units_per_EM));
  	if (y < yMin) 
    	yMin = y;
  	else if (y > yMax) 
    	yMax = y;
    	
  	x = (int)((mat[0] * face->bbox.xMax + mat[2] * face->bbox.yMax) / (div * face->units_per_EM));
  	if (x < xMin) 
    	xMin = x;
  	else if (x > xMax) 
    	xMax = x;
  	y = (int)((mat[1] * face->bbox.xMax + mat[3] * face->bbox.yMax) / (div * face->units_per_EM));
  	if (y < yMin) 
    	yMin = y;
  	else if (y > yMax) 
    	yMax = y;
    	
    if (xMax == xMin) 
    {
    	xMin = 0;
    	xMax = size;
  	}
  	if (yMax == yMin) 
  	{
    	yMin = 0;
    	yMax = (int)((double)1.2 * size);
  	}
  	
  	matrix.xx = (FT_Fixed)((mat[0] / size) * 65536);
  	matrix.yx = (FT_Fixed)((mat[1] / size) * 65536);
  	matrix.xy = (FT_Fixed)((mat[2] / size) * 65536);
  	matrix.yy = (FT_Fixed)((mat[3] / size) * 65536);
  	textMatrix.xx = (FT_Fixed)((textMat[0] / (textScale * size)) * 65536);
 	 	textMatrix.yx = (FT_Fixed)((textMat[1] / (textScale * size)) * 65536);
  	textMatrix.xy = (FT_Fixed)((textMat[2] / (textScale * size)) * 65536);
  	textMatrix.yy = (FT_Fixed)((textMat[3] / (textScale * size)) * 65536);
}

XWRasterFTFont::~XWRasterFTFont()
{
}

bool  XWRasterFTFont::getGlyph(int c, int xFrac, int, GlyphBitmap *bitmap)
{
    return XWRasterFont::getGlyph(c, xFrac, 0, bitmap);
}

XWRasterPath * XWRasterFTFont::getGlyphPath(int c)
{
    static FT_Outline_Funcs outlineFuncs = {
        &glyphPathMoveTo,
        &glyphPathLineTo,
        &glyphPathConicTo,
        &glyphPathCubicTo,
        0, 0
    };
    
    XWRasterFTFontFile * ff = (XWRasterFTFontFile *)fontFile;
    ff->face->size = sizeObj;
    FT_Set_Transform(ff->face, &textMatrix, NULL);
    FT_GlyphSlot slot = ff->face->glyph;
    FT_UInt gid;
    if (ff->codeToGID && c < ff->codeToGIDLen) 
        gid = ff->codeToGID[c];
    else 
        gid = (FT_UInt)c;
        
    if (ff->trueType && gid < 0) 
        return 0;
        
    if (FT_Load_Glyph(ff->face, gid, FT_LOAD_NO_BITMAP)) 
        return 0;
        
    FT_Glyph glyph;
    if (FT_Get_Glyph(slot, &glyph)) 
        return 0;
        
    RasterFTFontPath path;
    path.path = new XWRasterPath();
    path.textScale = textScale;
    path.needClose = false;
    FT_Outline_Decompose(&((FT_OutlineGlyph)glyph)->outline, &outlineFuncs, &path);
    if (path.needClose) 
        path.path->close();
        
    FT_Done_Glyph(glyph);
    return path.path;
}

bool XWRasterFTFont::makeGlyph(int c, int xFrac, int, GlyphBitmap *bitmap)
{
    XWRasterFTFontFile * ff = (XWRasterFTFontFile *)fontFile;

    ff->face->size = sizeObj;
    FT_Vector offset;
    offset.x = (FT_Pos)(int)((double)xFrac * RasterFontFractionMul * 64);
    offset.y = 0;
    FT_Set_Transform(ff->face, &matrix, &offset);
    FT_GlyphSlot slot = ff->face->glyph;

    FT_UInt gid;
    if (ff->codeToGID && c < ff->codeToGIDLen) 
        gid = (FT_UInt)ff->codeToGID[c];
    else 
        gid = (FT_UInt)c;
        
    if (ff->trueType && gid < 0) 
        return false;
        
    FT_Int32 flags = 0;
  	if (aa) 
  	{
    	flags |= FT_LOAD_NO_BITMAP;
  	}
  	if (ff->engine->flags & RasterFTNoHinting) 
  	{
    	flags |= FT_LOAD_NO_HINTING;
  	} 
  	else if (ff->trueType) 
  	{
    	// FT2's autohinting doesn't always work very well (especially with
    	// font subsets), so turn it off if anti-aliasing is enabled; if
    	// anti-aliasing is disabled, this seems to be a tossup - some fonts
    	// look better with hinting, some without, so leave hinting on
    	if (aa) 
    	{
      	flags |= FT_LOAD_NO_AUTOHINT;
    	}
  	} 
  	else if (ff->type1) 
  	{
    	// Type 1 fonts seem to look better with 'light' hinting mode
    	flags |= FT_LOAD_TARGET_LIGHT;
  	}
        
    if (FT_Load_Glyph(ff->face, gid, flags)) 
        return false;
        
    if (FT_Render_Glyph(slot, aa ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO)) 
        return false;
        
    if (slot->bitmap.width == 0 || slot->bitmap.rows == 0) 
    {
    	// this can happen if (a) the glyph is really tiny or (b) the
    	// metrics in the TrueType file are broken
    	return false;
  	}

    bitmap->x = -slot->bitmap_left;
    bitmap->y = slot->bitmap_top;
    bitmap->w = slot->bitmap.width;
    bitmap->h = slot->bitmap.rows;
    bitmap->aa = aa;
    int rowSize = 0;
    if (aa) 
        rowSize = bitmap->w;
    else 
        rowSize = (bitmap->w + 7) >> 3;
    bitmap->data = (uchar *)malloc(rowSize * bitmap->h * sizeof(uchar));
    bitmap->freeData = true;
    int i = 0;
    uchar * p = bitmap->data;
    uchar * q = slot->bitmap.buffer;
    for (; i < bitmap->h; ++i, p += rowSize, q += slot->bitmap.pitch) 
    {
        memcpy(p, q, rowSize);
    }

    return true;
}

