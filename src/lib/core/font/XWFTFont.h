/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWFTFONT_H
#define XWFTFONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TYPE1_TABLES_H
#include FT_TRUETYPE_TABLES_H
#include FT_SIZES_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_IMAGE_H
#include FT_BITMAP_H
#include FT_BBOX_H

#include <QPainterPath>
#include <QPainter>
#include <QColor>

#include "XWGlobal.h"

class XWFTFontFile;

class XW_FONT_EXPORT XWFTFont
{
public:
	XWFTFont(XWFTFontFile *fontFileA, 
	         int size,
	         double efactorA,
	         double slantA,
	         double boldfA,
	         bool boldA,
	         bool rotateA);
    XWFTFont(XWFTFontFile *fontFileA, double *matA, double *textMatA);
    ~XWFTFont();
    
    void drawChar(QPainter * painter,
                  int c, 
                  int * x,
                  int * y, 
                  int squareSize);
                  
    void drawChar(QPainter * painter,
                  int c, 
                  int x,
                  int y);

	int   getCharMapIndex(int pidA, int eidA);
	const char * getCodingScheme();
	int    getEncodingID();
	char * getFamilyName();	
	XWFTFontFile * getFontFile() { return fontFile; }	
    QPainterPath   getGlyphPath(int c);    
    int   getNumCmaps();
    int   getNumFaces();
    int   getNumGlyphs();
    int   getPlatformID();
    char * getStyleName();
    
    bool hasGlyphNames();
    bool hasHorizontal();
    bool hasKerning();
    bool hasMultipleMasters();
    bool hasPSGlyphNames();
    bool hasVertical();
    
    bool isCIDKeyed();
    bool isFixedWidth();
    bool isScalable();
    bool isSFNT();
    bool isTricky();
    
    bool matches(XWFTFontFile *fontFileA, double *matA, double *textMatA) 
        {
            return fontFileA == fontFile &&
                    matA[0] == mat[0] && matA[1] == mat[1] &&
                    matA[2] == mat[2] && matA[3] == mat[3] &&
                    textMatA[0] == textMat[0] && textMatA[1] == textMat[1] &&
                    textMatA[2] == textMat[2] && textMatA[3] == textMat[3];
        }
        
    void setCharMap(int i);
    void setCharMap(int pidA, int eidA);
    void setMatrix(double efactorA,
	               double slantA,
	               double boldfA,
	               bool boldA,
	               bool rotateA);

private:
	XWFTFontFile *fontFile;
    
    double mat[4];
    double textMat[4];
    
    bool   aa;
    
    int xMin, yMin, xMax, yMax;
    
    FT_Size sizeObj;
    FT_Matrix matrix;
    FT_Matrix textMatrix;
    double textScale;
    
    double efactor;
  	double slant;
  	double boldf;
  	bool bold;
  	bool rotate;
  	
  	FT_Matrix matrix1;
    FT_Matrix matrix2;
};


#endif // XWFTFONT_H

