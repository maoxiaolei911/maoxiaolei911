/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <QPointF>
#include <QImage>
#include <QVector>
#include "XWFTFontFile.h"
#include "XWFTFontEngine.h"
#include "XWFTFont.h"


static int pathMoveTo(const FT_Vector *pt, void *path);
static int pathLineTo(const FT_Vector *pt, void *path);
static int pathConicTo(const FT_Vector *ctrl, const FT_Vector *pt, void *path);
static int pathCubicTo(const FT_Vector *ctrl1, const FT_Vector *ctrl2, const FT_Vector *pt, void *path);


struct FTFontPath 
{
    QPainterPath path;
    double textScale;
    bool needClose;
};

static int pathMoveTo(const FT_Vector *pt, void *path)
{
    FTFontPath *p = (FTFontPath *)path;

    if (p->needClose) 
    {
        p->path.closeSubpath();
        p->needClose = false;
    }
    
    p->path.moveTo((double)pt->x * p->textScale / 64.0, (double)pt->y * p->textScale / 64.0);
    return 0;
}

static int pathLineTo(const FT_Vector *pt, void *path)
{
    FTFontPath *p = (FTFontPath *)path;
    
    p->path.lineTo((double)pt->x * p->textScale / 64.0, (double)pt->y * p->textScale / 64.0);
    p->needClose = true;
    return 0;
}

static int pathConicTo(const FT_Vector *ctrl, const FT_Vector *pt, void *path)
{
    FTFontPath *p = (FTFontPath *)path;
    QPointF curp = p->path.currentPosition();
    double x0 = curp.x();
    double y0 = curp.y();
    
    double xc = (double)ctrl->x * p->textScale / 64.0;
    double yc = (double)ctrl->y * p->textScale / 64.0;
    double x3 = (double)pt->x * p->textScale / 64.0;
    double y3 = (double)pt->y * p->textScale / 64.0;
    double x1 = (double)(1.0 / 3.0) * (x0 + (double)2 * xc);
    double y1 = (double)(1.0 / 3.0) * (y0 + (double)2 * yc);
    double x2 = (double)(1.0 / 3.0) * ((double)2 * xc + x3);
    double y2 = (double)(1.0 / 3.0) * ((double)2 * yc + y3);

    p->path.cubicTo(x1, y1, x2, y2, x3, y3);
    p->needClose = true;
    return 0;
}

static int pathCubicTo(const FT_Vector *ctrl1, const FT_Vector *ctrl2, const FT_Vector *pt, void *path)
{
    FTFontPath *p = (FTFontPath *)path;
    p->path.cubicTo((double)ctrl1->x * p->textScale / 64.0,
		             (double)ctrl1->y * p->textScale / 64.0,
		             (double)ctrl2->x * p->textScale / 64.0,
		             (double)ctrl2->y * p->textScale / 64.0,
		             (double)pt->x * p->textScale / 64.0,
		             (double)pt->y * p->textScale / 64.0);
    p->needClose = true;
    return 0;
}

XWFTFont::XWFTFont(XWFTFontFile *fontFileA, 
	               int size,
	               double efactorA,
	               double slantA,
	               double boldfA,
	               bool boldA,
	               bool rotateA)
{
	fontFile = fontFileA;
    fontFile->incRefCnt();
    mat[0] = 1;
    mat[1] = 0;
    mat[2] = 0;
    mat[3] = size;
    textMat[0] = 1;
    textMat[1] = 0;
    textMat[2] = 0;
    textMat[3] = 1;
    aa = fontFileA->engine->aa;
    xMin = yMin = xMax = yMax = 0;
    
    FT_Face face = fontFileA->face;
    if (FT_New_Size(face, &sizeObj)) 
        return;
    
    face->size = sizeObj;
    if (FT_Set_Pixel_Sizes(face, 0, (int)size)) 
        return;
        
    double div = face->bbox.xMax > 20000 ? 65536 : 1;
        
    int x = (int)((mat[0] * face->bbox.xMin + mat[2] * face->bbox.yMin) / (div * face->units_per_EM));
    xMin = xMax = x;
    int y = (int)((mat[1] * face->bbox.xMin + mat[3] * face->bbox.yMin) / (div * face->units_per_EM));
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
        xMax = (int)size;
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
    setMatrix(efactorA, slantA, boldfA, boldA, rotateA);
}

XWFTFont::XWFTFont(XWFTFontFile *fontFileA, double *matA, double *textMatA)
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
    aa = fontFileA->engine->aa;
    xMin = yMin = xMax = yMax = 0;
    
    FT_Face face = fontFileA->face;
    if (FT_New_Size(face, &sizeObj)) 
        return;
    
    face->size = sizeObj;
    double size = sqrt(mat[2]*mat[2] + mat[3]*mat[3]);
    if (FT_Set_Pixel_Sizes(face, 0, (int)size)) 
        return;
        
    textScale = sqrt(textMat[2]*textMat[2] + textMat[3]*textMat[3]) / size;

    double div = face->bbox.xMax > 20000 ? 65536 : 1;
        
    int x = (int)((mat[0] * face->bbox.xMin + mat[2] * face->bbox.yMin) / (div * face->units_per_EM));
    xMin = xMax = x;
    int y = (int)((mat[1] * face->bbox.xMin + mat[3] * face->bbox.yMin) / (div * face->units_per_EM));
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
        xMax = (int)size;
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
    textMatrix.xx = (FT_Fixed)((textMat[0] / (size * textScale)) * 65536);
    textMatrix.yx = (FT_Fixed)((textMat[1] / (size * textScale)) * 65536);
    textMatrix.xy = (FT_Fixed)((textMat[2] / (size * textScale)) * 65536);
    textMatrix.yy = (FT_Fixed)((textMat[3] / (size * textScale)) * 65536);
}


XWFTFont::~XWFTFont()
{
    fontFile->decRefCnt();
}

void XWFTFont::drawChar(QPainter * painter,
                        int c, 
                        int * x,
                        int * y, 
                        int squareSize)
{
	int tmpx = *x;
	int tmpy = *y;
	FT_Face face = fontFile->face;
	FT_UInt glyph_index = 0;
    if (fontFile->codeToGID && c < fontFile->codeToGIDLen) 
        glyph_index = fontFile->codeToGID[c];
    else 
        glyph_index = FT_Get_Char_Index(face, c);
        
    if (glyph_index == 0)
    	return ;
    	
    FT_Glyph aglyph;    
    FT_GlyphSlot slot = 0;
    if (FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT))
    	return ;
	if (FT_IS_SCALABLE(face))
	{
        slot = face->glyph;
        if (efactor != 1.0 || slant != 0.0 )
            FT_Outline_Transform(&(slot->outline), &matrix1);

        if (rotate)
            FT_Outline_Transform(&(slot->outline), &matrix2);

        if (bold)
            FT_Outline_Embolden(&(slot->outline), (int)(boldf * 64));
            
        FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL); 
	}
	else
	{
        slot = face->glyph;        
        FT_GlyphSlot_Own_Bitmap(slot);
        if (bold)
            FT_Bitmap_Embolden(fontFile->engine->lib, &(slot->bitmap), (int)(boldf * 64), (int)(boldf * 64));
        
        FT_Get_Glyph(slot, &aglyph);
        FT_Done_Glyph(aglyph);
	}
	
	if (slot->bitmap.width == 0 || slot->bitmap.rows == 0)
		return ;
		  
	tmpx += ((squareSize - slot->bitmap.rows) / 2);
  tmpy += ((squareSize - slot->bitmap.width) / 2);
  if (slot->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
  {
  	QImage img(slot->bitmap.buffer, 
               slot->bitmap.width, 
               slot->bitmap.rows, 
               slot->bitmap.pitch, 
               QImage::Format_Mono);
		painter->drawImage(tmpx, tmpy, img);			
  }
  else
  {
  	QImage img(slot->bitmap.buffer, 
               slot->bitmap.width, 
               slot->bitmap.rows, 
               slot->bitmap.pitch, 
               QImage::Format_Indexed8);			
	 QVector<QRgb> colorTable;
	 for (int i = 0; i < 256; ++i)
		 colorTable << qRgba(0, 0, 0, i);
	 img.setColorTable(colorTable);
	 painter->drawImage(tmpx, tmpy, img);
	}
}

void XWFTFont::drawChar(QPainter * painter,
                        int c, 
                        int x,
                        int y)
{
	FT_Face face = fontFile->face;
	FT_UInt glyph_index = 0;
    if (fontFile->codeToGID && c < fontFile->codeToGIDLen) 
        glyph_index = fontFile->codeToGID[c];
    else 
        glyph_index = FT_Get_Char_Index(face, c);
        
    if (glyph_index == 0)
    	return ;
    	
    FT_Glyph aglyph;    
    FT_GlyphSlot slot = 0;
    if (FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT))
    	return ;
	if (FT_IS_SCALABLE(face))
	{
        slot = face->glyph;
        if (efactor != 1.0 || slant != 0.0 )
            FT_Outline_Transform(&(slot->outline), &matrix1);

        if (rotate)
            FT_Outline_Transform(&(slot->outline), &matrix2);

        if (bold)
            FT_Outline_Embolden(&(slot->outline), (int)(boldf * 64));
            
        FT_Render_Glyph(slot, ft_render_mode_normal ); 
	}
	else
	{
        slot = face->glyph;        
        FT_GlyphSlot_Own_Bitmap(slot);
        if (bold)
            FT_Bitmap_Embolden(fontFile->engine->lib, &(slot->bitmap), (int)(boldf * 64), (int)(boldf * 64));
        
        FT_Get_Glyph(slot, &aglyph);
        FT_Done_Glyph(aglyph);
	}
	
	if (slot->bitmap.width == 0 || slot->bitmap.rows == 0)
		return ;
	
	if (slot->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
	{
		QImage img(slot->bitmap.buffer, 
               slot->bitmap.width, 
               slot->bitmap.rows, 
               slot->bitmap.pitch, 
               QImage::Format_Mono);
		painter->drawImage(x, y, img);	
	}
	else
	{
		QImage img(slot->bitmap.buffer, 
             	slot->bitmap.width, 
             	slot->bitmap.rows, 
             	slot->bitmap.pitch, 
             	QImage::Format_Indexed8);		
		QVector<QRgb> colorTable;
		for (int i = 0; i < 256; ++i)
			colorTable << qRgba(0, 0, 0, i);
		img.setColorTable(colorTable);
		painter->drawImage(x, y, img);
	}
}

int XWFTFont::getCharMapIndex(int pidA, int eidA)
{
	for (int i = 0; i < fontFile->face->num_charmaps; i++)
    {
        FT_CharMap charmap = fontFile->face->charmaps[i];
        if ((charmap->platform_id == pidA) && (charmap->encoding_id == eidA))
        	return i;
    }
    
    return 0;
}

const char * XWFTFont::getCodingScheme()
{
	FT_CharMap cm = fontFile->face->charmap;
	switch (cm->encoding)
    {
        case FT_ENCODING_UNICODE:
            return "Unicode";
            break;

        case FT_ENCODING_MS_SYMBOL:
            return "MS Symbol";
            break;

        case FT_ENCODING_SJIS:
            return "SJIS";
            break;

        case FT_ENCODING_GB2312:
            return "GB2312";
            break;

        case FT_ENCODING_BIG5:
            return "Big5";
            break;

        case FT_ENCODING_WANSUNG:
            return "WANSUNG";
            break;

        case FT_ENCODING_JOHAB:
            return "JOHAB";
            break;

        case FT_ENCODING_ADOBE_LATIN_1:
            return "Adobe Latin1";
            break;

        case FT_ENCODING_ADOBE_STANDARD:
            return "Adobe Standard";
            break;

        case FT_ENCODING_ADOBE_EXPERT:
            return "Adobe Expert";
            break;

        case FT_ENCODING_ADOBE_CUSTOM:
            return "Adobe Custom";
            break;
    
        case FT_ENCODING_APPLE_ROMAN:
            return "Apple Roman";
            break;

        default:
            break;
    }
    
    return "Unspecified";
}

int XWFTFont::getEncodingID()
{
	FT_CharMap cm = fontFile->face->charmap;
	return cm->encoding_id;
}

char * XWFTFont::getFamilyName()
{
	return (char*)(fontFile->face->family_name);
}

QPainterPath XWFTFont::getGlyphPath(int c)
{
	static FT_Outline_Funcs outlineFuncs = {
        &pathMoveTo,
        &pathLineTo,
        &pathConicTo,
        &pathCubicTo,
        0, 0
    };
    
    fontFile->face->size = sizeObj;
    FT_Set_Transform(fontFile->face, &textMatrix, NULL);
    FT_GlyphSlot slot = fontFile->face->glyph;
    FT_UInt gid;
    if (fontFile->codeToGID && c < fontFile->codeToGIDLen) 
        gid = fontFile->codeToGID[c];
    else 
        gid = (FT_UInt)c;
        
    if (fontFile->trueType && gid == 0) 
        return QPainterPath();
        
    if (FT_Load_Glyph(fontFile->face, gid, FT_LOAD_NO_BITMAP)) 
        return QPainterPath();
        
    FT_Glyph glyph;
    if (FT_Get_Glyph(slot, &glyph)) 
        return QPainterPath();
        
    FTFontPath path;
    path.textScale = textScale;
    path.needClose = false;
    FT_Outline_Decompose(&((FT_OutlineGlyph)glyph)->outline, &outlineFuncs, &path);
    if (path.needClose) 
        path.path.closeSubpath();
        
    FT_Done_Glyph(glyph);
    return path.path;
}

int XWFTFont::getNumCmaps()
{
	return fontFile->face->num_charmaps;
}

int XWFTFont::getNumFaces()
{
	return fontFile->face->num_faces;
}

int XWFTFont::getNumGlyphs()
{
	return fontFile->face->num_glyphs;
}

int XWFTFont::getPlatformID()
{
	FT_CharMap cm = fontFile->face->charmap;
	return cm->platform_id;
}

char * XWFTFont::getStyleName()
{
	return (char*)(fontFile->face->style_name);
}

bool XWFTFont::hasGlyphNames()
{
	return FT_HAS_GLYPH_NAMES(fontFile->face);
}

bool XWFTFont::hasHorizontal()
{
	return FT_HAS_HORIZONTAL(fontFile->face);
}

bool XWFTFont::hasKerning()
{
	return FT_HAS_KERNING(fontFile->face);
}

bool XWFTFont::hasMultipleMasters()
{
	return FT_HAS_MULTIPLE_MASTERS(fontFile->face);
}

bool XWFTFont::hasPSGlyphNames()
{
	return FT_Has_PS_Glyph_Names(fontFile->face);
}

bool XWFTFont::hasVertical()
{
	return FT_HAS_VERTICAL(fontFile->face);
}

bool XWFTFont::isCIDKeyed()
{
	return FT_IS_CID_KEYED(fontFile->face);
}

bool XWFTFont::isFixedWidth()
{
	return FT_IS_FIXED_WIDTH(fontFile->face);
}

bool XWFTFont::isScalable()
{
	return FT_IS_SCALABLE(fontFile->face);
}

bool XWFTFont::isSFNT()
{
	return FT_IS_SFNT(fontFile->face);
}

bool XWFTFont::isTricky()
{
	return FT_IS_TRICKY(fontFile->face);
}

void XWFTFont::setCharMap(int i)
{
	if (i < 0 || i >= fontFile->face->num_charmaps)
		return ;
		
	FT_CharMap charmap = fontFile->face->charmaps[i];
	FT_Set_Charmap(fontFile->face, charmap);
}

void XWFTFont::setCharMap(int pidA, int eidA)
{
	for (int i = 0; i < fontFile->face->num_charmaps; i++)
    {
        FT_CharMap charmap = fontFile->face->charmaps[i];
        if ((charmap->platform_id == pidA) && (charmap->encoding_id == eidA))
        {
            FT_CharMap cm = charmap;
            FT_Set_Charmap(fontFile->face, cm);
        }
    }
}

void XWFTFont::setMatrix(double efactorA,
	                     double slantA,
	                     double boldfA,
	                     bool boldA,
	                     bool rotateA)
{
	efactor = efactorA;
  	slant = slantA;
  	boldf = boldfA;
  	bold = boldA;
  	rotate = rotateA;
  	
  	matrix1.xx = (FT_Fixed)(floor(efactor * 1024) * (1L<<16)/1024);
    matrix1.xy = (FT_Fixed)(floor(slant * 1024) * (1L<<16)/1024);
    matrix1.yx = (FT_Fixed)0;
    matrix1.yy = (FT_Fixed)(1L<<16);
    if (rotate)
    {
        matrix2.xx = 0;
        matrix2.yx = 1L << 16;
        matrix2.xy = -matrix2.yx;
        matrix2.yy = matrix2.xx;
    }
}

