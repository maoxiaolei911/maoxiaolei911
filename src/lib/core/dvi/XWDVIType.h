/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVITYPE_H
#define XWDVITYPE_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include "XWGlobal.h"

#define XDV_FLAG_FONTTYPE_ATSUI 0x0001
#define XDV_FLAG_FONTTYPE_ICU   0x0002
#define XDV_FLAG_VERTICAL       0x0100
#define XDV_FLAG_COLORED        0x0200
#define XDV_FLAG_FEATURES       0x0400
#define XDV_FLAG_VARIATIONS     0x0800

#define PDF_COLORSPACE_TYPE_DEVICECMYK -4
#define PDF_COLORSPACE_TYPE_DEVICERGB  -3
#define PDF_COLORSPACE_TYPE_DEVICEGRAY -1
#define PDF_COLORSPACE_TYPE_INVALID     0
#define PDF_COLORSPACE_TYPE_CALGRAY     1
#define PDF_COLORSPACE_TYPE_CIELAB      2
#define PDF_COLORSPACE_TYPE_CALRGB      3
#define PDF_COLORSPACE_TYPE_ICCBASED    4

#define PDF_COLORSPACE_TYPE_CMYK  PDF_COLORSPACE_TYPE_DEVICECMYK
#define PDF_COLORSPACE_TYPE_RGB   PDF_COLORSPACE_TYPE_DEVICERGB
#define PDF_COLORSPACE_TYPE_GRAY  PDF_COLORSPACE_TYPE_DEVICEGRAY

#define PDF_COLOR_COMPONENT_MAX 4


struct XW_DVI_EXPORT DVIHeader
{
    ulong unitNum;
    ulong unitDen;
    ulong mag;
    ulong mediaWidth, mediaHeight;
    ulong stackDepth;
    char  comment[257];  
};

struct XW_DVI_EXPORT DVILoadedFont
{
    int  type;
    int  fontId;
    int  subfontId;
    int  tfmId;
    long size;
    int  source;
    FT_Face ft_face;
    ulong rgbaColor;
    ushort * glyphWidths;
    int layout_dir;
};

struct XW_DVI_EXPORT DVIFontDef
{
    long   texId;
    long   pointSize;
    long   designSize;
    char * fontName;
    int    fontId;
    int    used;
    char * familyName;
    char * styleName;
    ulong  rgbaColor;
    int    layoutDir;
};

struct XW_DVI_EXPORT DVIState
{
    long h, v, w, x, y, z, d;
};


typedef struct PDFTMatrix
{
  	double a, b, c, d, e, f;
} PDFTMatrix;

typedef struct PDFRect
{
  double llx, lly, urx, ury;
} PDFRect;

typedef struct PDFCoord
{
  double x, y;
} PDFCoord;

typedef struct
{
  	double      width;
  	double      height;
  	double      depth;

  	PDFTMatrix matrix;
  	PDFRect    bbox;

  	int         flags;
} TransformInfo;


extern void transform_info_clear(TransformInfo *info);

#define INFO_HAS_USER_BBOX (1 << 0)
#define INFO_HAS_WIDTH     (1 << 1)
#define INFO_HAS_HEIGHT    (1 << 2)
#define INFO_DO_CLIP       (1 << 3)
#define INFO_DO_HIDE       (1 << 4)

#define pdf_copymatrix(m,n) do {\
  (m)->a = (n)->a; (m)->b = (n)->b;\
  (m)->c = (n)->c; (m)->d = (n)->d;\
  (m)->e = (n)->e; (m)->f = (n)->f;\
} while (0)

#define pdf_setmatrix(m,p,q,r,s,t,u) do {\
  (m)->a = (p); (m)->b = (q);\
  (m)->c = (r); (m)->d = (s);\
  (m)->e = (t); (m)->f = (u);\
} while (0)

/* m -> n x m */
#define pdf_concatmatrix(m,n) do {\
  double _tmp_a, _tmp_b, _tmp_c, _tmp_d; \
  _tmp_a = (m)->a; _tmp_b = (m)->b; \
  _tmp_c = (m)->c; _tmp_d = (m)->d; \
  (m)->a  = ((n)->a) * _tmp_a + ((n)->b) * _tmp_c; \
  (m)->b  = ((n)->a) * _tmp_b + ((n)->b) * _tmp_d; \
  (m)->c  = ((n)->c) * _tmp_a + ((n)->d) * _tmp_c; \
  (m)->d  = ((n)->c) * _tmp_b + ((n)->d) * _tmp_d; \
  (m)->e += ((n)->e) * _tmp_a + ((n)->f) * _tmp_c; \
  (m)->f += ((n)->e) * _tmp_b + ((n)->f) * _tmp_d; \
} while (0)


#define PDF_FILL_RULE_NONZERO 0
#define PDF_FILL_RULE_EVENODD 1


#define  PDF_DASH_SIZE_MAX  16
#define  PDF_GSAVE_MAX      256


typedef struct
{
	char * key;
	int    type;
} SpecialHandler;


class XWDVIColor
{
public:
	XWDVIColor();
	XWDVIColor(const char * name);
	XWDVIColor(double g);
	XWDVIColor(double r, double g, double b);
	XWDVIColor(double c, double m, double y, double k);
	XWDVIColor(const XWDVIColor * other);
	XWDVIColor(const XWDVIColor * src, double f);
	~XWDVIColor() {}
	
	void black() {num_components = 1; values[0] = 0.0;}
	void brighten(const XWDVIColor * src, double f);
	
	int  compare(const XWDVIColor *color2);
	void copy(const XWDVIColor * other);
	
	int getType() {return -num_components;}
	
	bool isValid();
	bool isWhite();
	
	void setCMYK(double c, double m, double y, double k);
	void setGray(double g);
	int  setName(const char * name);
	void setRGB(double r, double g, double b);
	void setRGBFromHSV(double h, double s, double v);
	
	int toString(char *buffer);
	
	void white() {num_components = 1; values[0] = 1.0;}
	
public:
	int    num_components;
  	double values[PDF_COLOR_COMPONENT_MAX];
};

#endif //XWDVITYPE_H