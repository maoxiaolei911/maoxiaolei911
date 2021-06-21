/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXFONT_H
#define XWTEXFONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H
#include <QMutex>
#include <QObject>
#include <QByteArray>

#include "XWGlobal.h"

#define PDF_FONT_FONTTYPE_TYPE1    0
#define PDF_FONT_FONTTYPE_TYPE1C   1
#define PDF_FONT_FONTTYPE_TYPE3    2
#define PDF_FONT_FONTTYPE_TRUETYPE 3

#define PDF_FONT_FONTTYPE_TYPE0    4


#define PDF_FONT_FLAG_NOEMBED   (1 << 0)
#define PDF_FONT_FLAG_COMPOSITE (1 << 1)
#define PDF_FONT_FLAG_BASEFONT  (1 << 2)

#define PDF_FONT_PARAM_DESIGN_SIZE 1
#define PDF_FONT_PARAM_POINT_SIZE  2

#define PDF_FONT_TYPE3_PK 0
#define PDF_FONT_TYPE3_FT 1
#define PDF_FONT_TYPE3_GF 2


#define FONT_FLAG_FIXEDPITCH (1 << 0)  /* Fixed-width font */
#define FONT_FLAG_SERIF      (1 << 1)  /* Serif font */
#define FONT_FLAG_SYMBOLIC   (1 << 2)  /* Symbolic font */
#define FONT_FLAG_SCRIPT     (1 << 3)  /* Script font */
#define FONT_FLAG_STANDARD   (1 << 5)  /* Adobe Standard Character Set */
#define FONT_FLAG_ITALIC     (1 << 6)  /* Italic */
#define FONT_FLAG_ALLCAP     (1 << 16) /* All-cap font */
#define FONT_FLAG_SMALLCAP   (1 << 17) /* Small-cap font */
#define FONT_FLAG_FORCEBOLD  (1 << 18) /* Force bold at small text sizes */


class XWString;
class XWFontFileFT;
class XWFontFilePK;
class XWFontFileSFNT;
class XWSFNTCmap;
class XWOTLGsub;
class XWTTPostTable;
class XWFontFileCFF;
class XWTexFontMap;

struct GlyphMapper
{
  	XWSFNTCmap     * codetogid;
  	XWOTLGsub      * gsub;
  	XWFontFileSFNT * sfont;
  	XWTTPostTable  * nametogid;
};


class XW_FONT_EXPORT XWTexFont
{
public:
	friend class XWTexFontCache;
	
	XWTexFont();
	XWTexFont(const char * identA,
	          const char * map_nameA,
	          int    encoding_idA,
	          int    indexA,
	          double point_sizeA);
	~XWTexFont();
	
	double       getAvgWidth() {return desc.AvgWidth;}
	double       getCapHeight() {return desc.CapHeight;}
	double       getDescAscent() {return desc.Ascent;}
	double       getDescDescent() {return desc.Descent;}
	int          getDescFlags() {return desc.Flags;}
	double     * getDescFontBBox() {return desc.FontBBox;}
	const char * getDictBaseFont() {return dict.BaseFont;}
	double     * getDictFontBBox() {return dict.FontBBox;}
	const char * getDescFontName() {return desc.FontName;}
	const char * getDictSubtype() {return dict.Subtype;}
	const char * getDictType() {return dict.Type;}
	double *     getDictWidths() {return dict.Widths;}
	int    getEncoding() {return encoding_id;}
	int    getFlag(int mask) {return ((flags & mask) ? 1 : 0);}	
	int    getFirstChar() {return dict.FirstChar;}
	double * getFontMatrix() {return dict.FontMatrix;}
	char * getFontName() {return fontname;}
	char * getIdent() {return ident;}
	int    getIndex() {return index;}
	double getItalicAngle() {return desc.ItalicAngle;}
	int    getLastChar() {return dict.LastChar;}
	char * getMapName() {return map_name;}
	int    getParam(int param_type);
	double getStemV() {return desc.StemV;}
	int    getSubtype() {return subtype;}
	int    getT3Type() {return t3Type;}
	char * getUniqueTag();
	char * getUsedChars();
	double getXHeight() {return desc.XHeight;}
		
	bool isLoaded() {return loaded;}
	bool isLocking() {return locking;}
	
	XWFontFileFT   * loadFT(int hDPI, int vDPI);
	XWFontFilePK   * loadPK(int base_dpi);
	bool loadTrueType(uchar ** streamdata, 
	                  long * streamlen);
	bool  loadType1(char ** enc_vec, 
	                char * fullname, 
	                uchar ** streamdata, 
	                long * streamlen);
	bool  loadType1C(char ** enc_vec, 
	                char * fullname, 
	                uchar ** streamdata, 
	                long * streamlen);
	
	static void makeUniqueTag(char *tag);
	
	int openFT();
	int openPK(int base_dpi);
	int openTrueType();
	int openType1();
	int openType1C();
	
	void setEncoding(int id) {encoding_id = id;}
	void setFlags(int flagsA) {flags |= flagsA;}
	void setFontName(const char *fontnameA);
	void setIndex(int i) {index = i;}
	void setMapName(const char * n);
	void setSubtype(int subtypeA) {subtype = subtypeA;}
	
private:	
	void addMetrics(XWFontFileCFF *cffont, 
	                char **enc_vec, 
	                long num_glyphs);
	void addSimpleMetrics(XWFontFileCFF *cffont, ushort num_glyphs);
	int  aglDecomposeGlyphName(char *glyphname, 
	                           char **nptrs, 
	                           int size, 
	                           char **suffix);
	                           
	void cleanGlyphMapper(GlyphMapper *gm);
	int  composeGlyph(ushort *glyphs, 
	                  int n_glyphs,
                      const char *feat, 
                      GlyphMapper *gm, 
                      ushort *gid);
	int  composeUChar(long *unicodes, 
	                  int n_unicodes,
                      const char *feat, 
                      GlyphMapper *gm, 
                      ushort *gid);
	
	int  doBuiltinEncoding(XWFontFileSFNT *sfont);
	int  doCustomEncoding(char **encoding, 
	                      XWFontFileSFNT *sfont);
	void doWidths();
	
	int  findComposite(const char *glyphname, 
	                   ushort *gid, 
	                   GlyphMapper *gm);
	int  findParanoiac(const char *glyphname, 
	                   ushort *gid, 
	                   GlyphMapper *gm);
	int  findPostTable(const char *glyph_name, 
	                   ushort *gid, 
	                   GlyphMapper *gm);
	
	void getFontAttr(XWFontFileCFF *cffont);
	
	int  resolveGlyph(const char *glyphname, 
	                  ushort *gid, 
	                  GlyphMapper *gm);
	
	int  selectGlyph(ushort in, 
	                 const char *suffix, 
	                 GlyphMapper *gm, 
	                 ushort *out);
	int  selectGsub(const char *feat, GlyphMapper *gm);
	int  setupGlyphMapper(GlyphMapper *gm, XWFontFileSFNT *sfont);
	
	long writeFontFile(XWFontFileCFF *cffont, 
	                   uchar ** streamdata, 
	                   long * streamlen);
	
private:
	char    *ident;
  	int      subtype;
  	char    *map_name;
  	int      encoding_id;
  	int      index;
  	char    *fontname;
  	char    *usedchars;
  	int      flags;
  	double   point_size;
  	double   design_size;
  	int      t3Type;
  	bool     loaded;
  	
  	FT_Face ft_face;
  	ushort * ft_to_gid;
  	
  	char     uniqueID[7];
  	
  	struct Dictionary
  	{
  		char * Type;
  		char * Subtype;
  		char * Name;  		
  		char * BaseFont;
  		double FontBBox[4];
  		double FontMatrix[6];
  		int    FirstChar;
  		int    LastChar;
  		double * Widths;
  	};
  	
  	struct Descriptor
  	{
  		char * Type;
  		char * FontName;
  		int    Flags;
  		double FontBBox[4];
  		double ItalicAngle;
  		double Ascent;
  		double Descent;
  		double CapHeight;
  		double XHeight;
  		double StemV;
  		double AvgWidth;
  		uchar  panose[12];
  	};
  	
  	Dictionary dict;
  	Descriptor desc;
  	bool locking;
};

class XW_FONT_EXPORT XWTexFontCache
{
public:
	XWTexFontCache();
	~XWTexFontCache();
	
	int    add(XWTexFont * font);
	
	int    find(const char *texname,
	            const char *fontname,
		       	double font_scale, 
		       	int    encoding_id,
		       	XWTexFontMap *mrec,
		       	int base_dpi);
	
	XWTexFont * get(int font_id);
	
	int    getFontEncoding(int font_id);
	int    getFontSubtype(int font_id);
	char * getFontUsedChars(int font_id);
	int    getFontWMode(int font_id);
	
private:
	int       count;
  	int       capacity;
  	XWTexFont **fonts;
};

#endif // XWTEXFONT_H
