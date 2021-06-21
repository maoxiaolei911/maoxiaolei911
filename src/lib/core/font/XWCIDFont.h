/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWCIDFONT_H
#define XWCIDFONT_H

#include <QMutex>
#include <QObject>
#include <QString>
#include "XWCMap.h"
#include "XWTexFontMap.h"

#define CIDFONT_TYPE0 1
#define CIDFONT_TYPE2 2

#define CIDFONT_FORCE_FIXEDPITCH (1 << 1)

#define FONT_FLAG_NONE        0
#define FONT_FLAG_BASEFONT    (1 << 0)
#define FONT_FLAG_ACCFONT     (1 << 1)
#define FONT_FLAG_UCSFONT     (1 << 2)

#define FONT_STYLE_NONE       FONTMAP_STYLE_NONE
#define FONT_STYLE_BOLD       FONTMAP_STYLE_BOLD
#define FONT_STYLE_ITALIC     FONTMAP_STYLE_ITALIC
#define FONT_STYLE_BOLDITALIC FONTMAP_STYLE_BOLDITALIC

#define CIDFONT_FLAG_TYPE1      (1 << 8)
#define CIDFONT_FLAG_TYPE1C     (1 << 9)
#define CIDFONT_FLAG_TRUETYPE   (1 << 10)

class XWLexer;
class XWFontFileSFNT;
class XWFontFileCFF;
class XWTexFontMapOpt;
class XWTTMaxpTable;
class XWTTHeadTable;
struct TTLongMetrics;
class XWCMap;
class XWTTGlyphs;

class XW_FONT_EXPORT XWCIDOpt
{
public:
	XWCIDOpt();
	~XWCIDOpt();
	
public:
	char       *name;
	CIDSysInfo *csi;
	int         index;
	int         style;
  	int         embed;
 	int         stemv;
};

class XW_FONT_EXPORT XWCIDWidths
{
public:
	XWCIDWidths();
	~XWCIDWidths();
	
	void add(double num);
	void add(XWCIDWidths * WS);
	
public:
	long count;	
	int  max;
	struct Widths
	{
		long len;
		double * values;
	};
	
	char    * types;
	void   ** widths;
};
	
class XW_FONT_EXPORT XWCIDFont : public QObject
{
	Q_OBJECT
	
public:
	friend class XWCIDFontCache;
	
	XWCIDFont(QObject * parentA = 0);
	~XWCIDFont();
	
	void attachParent(int parent_id, int wmode);
	
	double       getAvgWidth() {return desc.AvgWidth;}
	double       getCapHeight() {return desc.CapHeight;}
	CIDSysInfo * getCIDSysInfo() {return csi;}	
	double       getDescAscent() {return desc.Ascent;}
	double       getDescDescent() {return desc.Descent;}
	int          getDescFlags() {return desc.Flags;}
	const char * getDictBaseFont() {return dict.BaseFont;}
	const char * getDescFontName() {return desc.FontName;}
	const char * getDictSubtype() {return dict.Subtype;}
	const char * getDictType() {return dict.Type;}
	double     getDW() {return dict.DW;}
	XWCIDWidths * getDW2() {return dict.DW2;}
	int        getEmbedding();
	int        getFlag(int mask) {return ((flags & mask) ? 1 : 0);}
	double   * getFontBBox() {return desc.FontBBox;}
	char     * getFontName() {return fontname;}
	FT_Face    getFtFace() {return ft_face;}
	ushort   * getFtToGid() {return ft_to_gid;}
	char     * getIdent() {return ident;}
	double     getItalicAngle() {return desc.ItalicAngle;}
	const char * getName() {return name;}
	int        getOptIndex();
	uchar   *  getPanose() {return desc.panose;}
	int        getParentID(int wmode);
	double     getStemV() {return desc.StemV;}
	int        getSubtype() {return subtype;}
	XWCIDWidths * getW() {return dict.W;}
	XWCIDWidths * getW2() {return dict.W2;}
	double     getXHeight() {return desc.XHeight;}
	
	bool isACCFont();
	bool isBaseFont() {return (flags & FONT_FLAG_BASEFONT) ? 1 : 0;}
	static bool isFixedPitch();
	bool isLoaded() {return loaded;}
	bool isLocking() {return locking;}
	bool isUCSFont();
	
	int load(XWCMap ** tounicodecmap,
	         uchar ** streamdata, 
	         long * streamlen,
	         char ** usedchars,
	         int  * lastcid,
	         uchar** cidtogidmapA);
	
	int open(const char *nameA,
		     CIDSysInfo *cmap_csi, 
		     XWCIDOpt *opt);
		          
private:
	void addCIDHMetrics(uchar *CIDToGIDMap, 
		 				ushort last_cid,
		 				XWTTMaxpTable *maxp,
		 				XWTTHeadTable *head, 
		 				TTLongMetrics *hmtx);
	void addCIDMetrics(XWFontFileSFNT *sfont,
					   uchar *CIDToGIDMap, 
					   ushort last_cid, 
					   int need_vmetrics);
	void addCIDVMetrics(XWFontFileSFNT *sfont,
		                uchar *CIDToGIDMap, 
		                ushort last_cid,
		 				XWTTMaxpTable *maxp,
		 				XWTTHeadTable *head, 
		 				TTLongMetrics *hmtx);
	void addMetrics(XWFontFileCFF *cffont,
	     			uchar *CIDToGIDMap,
	     			double *widths, 
	     			double default_width, 
	     			ushort last_cid);
	void addTTCIDHMetrics(XWTTGlyphs *g,
		                  char *used_chars, 
		                  uchar *cidtogidmap, 
		                  ushort last_cid);
	void addTTCIDVMetrics(XWTTGlyphs *g,
		   				  char *used_chars, 
		   				  /*uchar *cidtogidmap,*/ 
		   				  ushort last_cid);
		 				
	long cidToCode(XWCMap *cmap, ushort cid);
	XWCMap * createToUnicodeCMap(XWFontFileCFF *cffont, 
	                             const char *used_glyphs);
	
	XWCMap * findToCodeCMap(const char *reg, 
	                        const char *ord, 
	                        int select);
	ushort fixCJKSymbols(ushort code);

	void getFontAttr(XWFontFileCFF *cffont);
	
	bool  loadType0(uchar ** streamdata, 
	                long * streamlen,
	                char ** usedchars,
	                int  * lastcid);
	bool  loadType0T1(XWCMap ** tounicodecmap,
	                   uchar ** streamdata, 
	                   long * streamlen,
	                   char ** usedchars,
	                   int  * lastcid);
	bool  loadType0T1C(uchar ** streamdata, 
	                   long * streamlen,
	                   char ** usedchars,
	                   int  * lastcid);
	bool  loadType2(uchar ** streamdata, 
	                long * streamlen,
	                char ** usedchars,
	                int  * lastcid,
	                uchar** cidtogidmapA);
	                
	int openBase(const char *nameA,
		         CIDSysInfo *cmap_csi, 
		         XWCIDOpt *opt);
	int openType0(const char *nameA,
		          CIDSysInfo *cmap_csi, 
		          XWCIDOpt *opt);
	int openType0T1(const char *nameA,
		            CIDSysInfo *cmap_csi, 
		            XWCIDOpt *opt);
	int openType0T1C(const char *nameA,
		             CIDSysInfo *cmap_csi, 
		             XWCIDOpt *opt);
	int openType2(const char *nameA,
		          CIDSysInfo *cmap_csi, 
		          XWCIDOpt *opt);

	int parseDescriptor(const char *start, const char *end);
	int parseDict(const char *start, const char *end, CIDSysInfo *cmap_csi);
	
	long writeFontFile(XWFontFileCFF *cffont,
	                   uchar ** streamdata, 
	                   long * streamlen);
	
private:
	char       *ident;
	char       *name;
	char       *fontname;
	int         subtype;
	int         flags;
	CIDSysInfo *csi;
	
	XWCIDOpt *options;
	bool loaded;
	
	FT_Face   ft_face;
  ushort  * ft_to_gid;
  
	int     parent[2];
	
	struct Dictionary
  	{
  		char * Type;
  		char * Subtype;
  		char * BaseFont;
  		double DW;
  		XWCIDWidths * W;
  		XWCIDWidths * DW2;
  		XWCIDWidths * W2;
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

class XW_FONT_EXPORT XWCIDFontCache
{
public:
	XWCIDFontCache();
	~XWCIDFontCache();
	
	int find(const char *map_name, 
	         CIDSysInfo *cmap_csi, 
	         XWTexFontMapOpt *fmap_opt,
	         int pdf_ver = 4);
	
	XWCIDFont * get(int font_id);
	
	static int t1LoadUnicodeCMap(const char *font_name,
		     			         const char *otl_tags,
		     					 int wmode);
	
	static void setFlags(int flagsA);
	
private:
	CIDSysInfo * getCIDSysInfo(XWTexFontMapOpt *fmap_opt,
	                           int pdf_ver);
	                           
	static int loadBaseCMap(const char *font_name, 
	                        int wmode, 
	                        XWFontFileCFF *cffont);
	
private:
	int       num;
  	int       max;
  	XWCIDFont **fonts;
  	
  	static int flags;
};

#endif // XWCIDFONT_H
