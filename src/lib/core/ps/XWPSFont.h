/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSFONT_H
#define XWPSFONT_H

#include <QList>
#include "XWPSBitmap.h"

class XWPSContextState;
class XWPSCharstringFontRefs;
class XWPSDevice;
class XWPSFontDir;
class XWPSFontBase;
class XWPSFont;
class XWPSFontType0;
class XWPSFontType1;
class XWPSFontCid0;
class XWPSFontCid1;
class XWPSFontType42;
class XWPSFontCid2;
class XWPSPath;
class XWPSSegment;
class XWPSSubpath;
class XWPSStream;
class XWPSFontType1;
class XWPSImagerState;
class XWPSTextEnum;
class XWPSShowEnum;
class XWPSBboxTextEnum;
class XWPSPDFTextEnum;
class XWPSDVITextEnum;
class XWPSState;
class XWPSCMap;
class XWPSDeviceMem;
class XWPSOp1State;
class XWPSRef;
class XWPSType1State;
struct PSPrinterParams;

#define MAX_GDBytes 4

#define FONT_SAME_OUTLINES 1
#define FONT_SAME_METRICS 2
#define FONT_SAME_ENCODING 4

#define GLYPH_INFO_WIDTH0 1
#define GLYPH_INFO_WIDTH GLYPH_INFO_WIDTH0
#define GLYPH_INFO_WIDTH1 2	
#define GLYPH_INFO_WIDTHS (GLYPH_INFO_WIDTH0 | GLYPH_INFO_WIDTH1)

#define GLYPH_INFO_BBOX 4
#define GLYPH_INFO_NUM_PIECES 8
#define GLYPH_INFO_PIECES 16

#define FONT_IS_FIXED_WIDTH (1<<0)

#define STEM_TOLERANCE float2fixed(0.05)

#define ps_no_char ((ulong)~0L)
#define ps_no_glyph ((ulong)0x7fffffff)
#define ps_no_xglyph ((ulong)~0L)

#define ps_max_glyph max_ulong

#if arch_sizeof_long > 4
#  define ps_min_cid_glyph ((ulong)0x80000000L)
#else
/* Avoid compiler warnings about signed/unsigned constants. */
#  define ps_min_cid_glyph ((ulong)~0x7fffffff)
#endif

#define c_value_num1(ch) ((int)(uchar)(ch) - 139)

#if arch_sizeof_long > 4
#  define sign_extend_num4(lw)\
     lw = (lw ^ 0x80000000L) - 0x80000000L
#else
#  define sign_extend_num4(lw) 
#endif

#define X_SPACE_MIN 24
#define X_SPACE_MAX 150

int psf_sort_glyphs(ulong *glyphs, int count);
int psf_sorted_glyphs_index_of(const ulong *glyphs, int count, ulong glyph);
bool psf_sorted_glyphs_include(const ulong *glyphs, int count, ulong glyph);


#define MAX_CMAP_CODE_SIZE 4

class XWPSCharIndex
{
public:
	XWPSCharIndex() : encx(0),charx(0){}
	
public:
	uchar encx, charx;
};

class XWPSCFontDictKeys
{
public:
	XWPSCFontDictKeys() 
		: enc_keys(0),
			num_enc_keys(0),
			num_str_keys(0),
			extra_slots(0),
			dict_attrs(0),
			value_attrs(0) {}
	
public:
	const XWPSCharIndex *enc_keys;
  uint num_enc_keys;
  uint num_str_keys;
  uint extra_slots;	
  uint dict_attrs;
  uint value_attrs;
};

class XWPSCidSystemInfo
{
public:
	XWPSCidSystemInfo();
	~XWPSCidSystemInfo();
	
	bool isNull() {return Registry.size == 0 && Ordering.size == 0 && Supplement == 0;}
	
	void setNull();
	
public:
	XWPSString Registry;
  XWPSString Ordering;
  int Supplement;
};


class XWPSCodeSpaceRange
{
public:
	XWPSCodeSpaceRange();
	
public:
	uchar first[MAX_CMAP_CODE_SIZE];
  uchar last[MAX_CMAP_CODE_SIZE];
  int size;
};

class XWPSCodeSpace
{
public:
	XWPSCodeSpace();
	~XWPSCodeSpace();
	
public:
	XWPSCodeSpaceRange *ranges;
  int num_ranges;
};

class XWPSCodeLookupRange
{
public:
	XWPSCodeLookupRange();
	~XWPSCodeLookupRange();
	
public:
	XWPSCMap *cmap;
  uchar key_prefix[MAX_CMAP_CODE_SIZE];
  int key_prefix_size;
  int key_size;	
  int num_keys;
  bool key_is_range;
  XWPSString keys;
  PSCodeValueType value_type;
  int value_size;
  XWPSString values;
  int font_index;
};

class XWPSCodeMap
{
public:
	XWPSCodeMap();
	~XWPSCodeMap();
	
	int acquireCodeMap(XWPSContextState *ctx, 
	                   XWPSRef * pref, 
	                   XWPSCMap * root);
	                   
	uint bytes2int(const uchar *p, int n);
	
	int decodeNext(const XWPSString * pstr,
                 uint * pindex, 
                 uint * pfidx,
                 ulong * pchr, 
                 ulong * pglyph);
	
public:
	XWPSCodeLookupRange *lookup;
  int num_lookup;
};

typedef bool(XWPSContextState::*ps_glyph_mark_proc_t)(ulong glyph, void *proc_data);
typedef int(XWPSContextState::*ps_glyph_name_proc_t)(ulong glyph, XWPSString *pstr, void *proc_data);

class XWPSCMap : public XWPSStruct
{
public:
	XWPSCMap();
	~XWPSCMap();
	
	int acquireCodeRanges(XWPSContextState *ctx, XWPSRef * pref);
	
	int decodeNext(const XWPSString * pstr,
                 uint * pindex, 
                 uint * pfidx,
                 ulong * pchr, 
                 ulong * pglyph);
                 
	int getLength();
	const char * getTypeName();
	
public:
	int CMapType;
  XWPSString CMapName;
  XWPSCidSystemInfo *CIDSystemInfo;
  int num_fonts;
  float CMapVersion;
  XWPSUid uid;
  long UIDOffset;
  int WMode;
  XWPSCodeSpace code_space;
  XWPSCodeMap def;
  XWPSCodeMap notdef;
  ps_glyph_mark_proc_t mark_glyph;
  void *mark_glyph_data;
  ps_glyph_name_proc_t glyph_name;
  void *glyph_name_data;
};

class XWPSPSFGlyphEnum
{
public:
	XWPSPSFGlyphEnum();
	
	int enumerateGlyphsNext(ulong *pglyph);
	
	int bitsNext(ulong *pglyph);
	int fontNext(ulong *pglyph);
	int glyphsNext(ulong *pglyph);
	int rangeNext(ulong *pglyph);
	
	void bitsBegin(XWPSFont *fontA,
			 					 const uchar *subset_bits, 
			 					 uint subset_size,
			           PSGlyphSpace glyph_spaceA);
	
	void listBegin(XWPSFont *fontA,
			           const ulong *subset_list, 
			           uint subset_size,
			           PSGlyphSpace glyph_spaceA);
			           
	void reset() {index=0;}
	
public:
	XWPSFont *font;
	struct su_ 
	{
		union sus_ 
		{
	    const ulong *list;
	    const uchar *bits;
		} selected;
		uint size;
  } subset;
  PSGlyphSpace glyph_space;
  ulong index;
  
  int (XWPSPSFGlyphEnum::*enumerate_next)(ulong *);
};

class XWPSPSFOutlineGlyphs
{
public:
	XWPSPSFOutlineGlyphs();
	~XWPSPSFOutlineGlyphs();
	
public:
	ulong notdef;
	ulong subset_data[256 * 3 + 1];
	ulong *subset_glyphs;
	uint subset_size;
};

#define FONT_INFO_ASCENT 0x0001
#define FONT_INFO_AVG_WIDTH 0x0002
#define FONT_INFO_BBOX 0x0004
#define FONT_INFO_CAP_HEIGHT 0x0008
#define FONT_INFO_DESCENT 0x0010
#define FONT_INFO_FLAGS 0x0020
#define FONT_INFO_ITALIC_ANGLE 0x0100
#define FONT_INFO_LEADING 0x0200
#define FONT_INFO_MAX_WIDTH 0x0400
#define FONT_INFO_MISSING_WIDTH 0x0800
#define FONT_INFO_STEM_H 0x00010000
#define FONT_INFO_STEM_V 0x00020000
#define FONT_INFO_UNDERLINE_POSITION 0x00040000
#define FONT_INFO_UNDERLINE_THICKNESS 0x00080000
#define FONT_INFO_X_HEIGHT 0x00100000
#define FONT_INFO_COPYRIGHT 0x0040
#define FONT_INFO_NOTICE 0x0080
#define FONT_INFO_FAMILY_NAME 0x1000
#define FONT_INFO_FULL_NAME 0x2000

class XWPSFontInfo
{
public:
	XWPSFontInfo();
	
public:
	int members;
	int Ascent;
	int AvgWidth;
	XWPSIntRect BBox;
	int CapHeight;
	int Descent;
	uint Flags;	
	uint Flags_requested;
	uint Flags_returned;
	float ItalicAngle;
	int Leading;
	int MaxWidth;
	int MissingWidth;
	int StemH;
	int StemV;
	int UnderlinePosition;
	int UnderlineThickness;
	int XHeight;
	XWPSString Copyright;
	XWPSString Notice;
	XWPSString FamilyName;
	XWPSString FullName;
};

class XWPSGlyphInfo
{
public:
	XWPSGlyphInfo();
	~XWPSGlyphInfo();
	
public:
	int members;
	XWPSPoint width[2];
	XWPSRect bbox;
	int num_pieces;
	ulong *pieces;
};

class XWPSXFont
{
public:
	XWPSXFont();
	virtual ~XWPSXFont();
	
	virtual int renderChar(ulong,
	                       XWPSDevice *,
	                       int , 
	                       int , 
	                       ulong , 
	                       int ) {return 0;}
};

class XWPSCachedFMPair
{
public:
	XWPSCachedFMPair();
	~XWPSCachedFMPair();
	
	uint charsHeadIndex(ulong glyph) {return ((uint)glyph * 59 + hash * 73);}
	
	bool isFree() {return (font==0 && !UID.isValid());}
	
	void setFree();
	
public:
	XWPSFont * font;
	XWPSUid UID;
	PSFontType FontType;
	uint hash;
	float mxx, mxy, myx, myy;
	int num_chars;
	bool xfont_tried;
	XWPSXFont *xfont;
	uint index;
};

class XWPSFMPairCache
{
public:
	XWPSFMPairCache();
	~XWPSFMPairCache();
	
public:
	uint msize, mmax;
	XWPSCachedFMPair *mdata;
	uint mnext;
};

#define align_cached_char_mod align_cached_bits_mod
#define sizeof_cached_char\
  ROUND_UP(sizeof(XWPSCachedChar), align_cached_char_mod)
#define cc_bits(cc) ((uchar *)(cc) + sizeof_cached_char)
#define cc_const_bits(cc) ((const uchar *)(cc) + sizeof_cached_char)

struct PSFontCidRange
{
    ulong cid_min, cid_max;
    XWPSFont *font;
} ;

class XWPSCachedChar : public XWPSCachedBits
{
public:
	XWPSCachedChar();
	~XWPSCachedChar();
	
	static XWPSCachedChar * alloc(XWPSFontDir * dir, 
	               XWPSDeviceMem * dev,
		   					 XWPSDeviceMem * dev2, 
		   					 ushort iwidth, 
		   					 ushort iheight,
		             const XWPSLog2ScalePoint * pscale, 
		             int depth);
	
	uint charsHeadIndex();
	uchar * compressAlphaBits();
	
	bool hasBits() {return (id != ps_no_bitmap_id);}
	
	bool purgeFMPairChar(void *vpair);
	bool purgeFMPairCharXFont(void *vpair);
	
	bool selectCidRange(void *range_ptr);
	
	void setPair(XWPSCachedFMPair *p);
	void setRaster(uint r) {raster=r;}
	
public:
	XWPSCachedFMPair *pair;
	ulong code;
	uchar wmode;
	XWPSBitsCacheChunk *chunk;
	uint loc;
	uint pair_index;
	ulong xglyph;
	XWPSFixedPoint wxy;
	XWPSFixedPoint offset;
};

class XWPSCharCache : public XWPSBitsCache
{
public:
	XWPSCharCache();
	~XWPSCharCache();
	
public:
	XWPSCachedChar **table;
	uint table_mask;
	uint bmax;
  uint cmax;
  uint bspace;
  uint lower;
  uint upper;
  ps_glyph_mark_proc_t mark_glyph;
  void *mark_glyph_data;
};

class XWPSFontDir
{
public:
	XWPSFontDir();
	XWPSFontDir(uint smaxA, uint bmax, uint mmax, uint cmax, uint upper);
	~XWPSFontDir();
	
	void addCachedChar(XWPSDeviceMem * dev,
	                   XWPSCachedChar * cc, 
	                   XWPSCachedFMPair * pair, 
	                   const XWPSLog2ScalePoint * pscale);
	void addCharBits(XWPSCachedChar * cc,
		 							const XWPSLog2ScalePoint * plog2_scale);
	XWPSCachedFMPair * addFMPair(XWPSFont * font, 
	                             XWPSUid * puid,
	                             XWPSState * pgs);
	       
	XWPSCachedChar * allocChar(ulong icdsize);
	XWPSCachedChar * allocCharInChunk(ulong icdsize);
	void cacheStatus(uint pstat[7]);
	int  charCacheAlloc(uint bmax, uint mmax, uint cmax, uint upper);
	uint currentCacheLower() {return ccache.lower;}
	uint currentCacheSize() { return ccache.bmax;}
	uint currentCacheUpper() {return ccache.upper;}
	
	void freeCachedChar(XWPSCachedChar * cc);
	
	void hashRemoveCachedChar(uint chi);
	
	void init();
	
	void purgeFMPair(XWPSCachedFMPair * pair, int xfont_only);
	void purgeFontFromCharCaches(const XWPSFont * font);
	void purgeSelectedCachedChars(bool(XWPSCachedChar::*proc)(void *), void *proc_data);
		
	int setCacheLower(uint size);
	int setCacheSize(uint size);
	int setCacheUpper(uint size);
	void shortenCachedChar(XWPSCachedChar * cc, uint diff);
	
public:
	XWPSFont *orig_fonts;
	XWPSFont *scaled_fonts;
	uint ssize, smax;
	XWPSFMPairCache fmcache;
	XWPSCharCache ccache;
	uint enum_index;
	uint enum_offset;
	QList<XWPSBitsCacheChunk *> chunks;
	XWPSContextState * i_ctx_p;
};

#define WRITE_TRUETYPE_CMAP 1
#define WRITE_TRUETYPE_NAME 2	
#define WRITE_TRUETYPE_POST 4	
#define WRITE_TRUETYPE_NO_TRIMMED_TABLE 8

#define OS_2_LENGTH sizeof(ttf_OS_2_t)

struct PSXFontCallbacks 
{
	union
	{
  	const char * (XWPSFont::*glyph_name)(ulong, uint *);
  	const char * (XWPSFontBase::*glyph_nameb)(ulong, uint *);
  	const char * (XWPSFontType0::*glyph_namet0)(ulong, uint *);
  	const char * (XWPSFontType1::*glyph_namet1)(ulong, uint *);
  	const char * (XWPSFontType42::*glyph_namet42)(ulong, uint *);
  	const char * (XWPSFontCid1::*glyph_namec1)(ulong, uint *);
  	const char * (XWPSFontCid2::*glyph_namec2)(ulong, uint *);
  	const char * (XWPSFontCid0::*glyph_namec0)(ulong, uint *);
  }glyph_name_;
  
  union
  {
  	ulong (XWPSFont::*known_encode)(ulong, int);
  	ulong (XWPSFontBase::*known_encodeb)(ulong, int);
  	ulong (XWPSFontType0::*known_encodet0)(ulong, int);
  	ulong (XWPSFontType1::*known_encodet1)(ulong, int);
  	ulong (XWPSFontType42::*known_encodet42)(ulong, int);
  	ulong (XWPSFontCid1::*known_encodec1)(ulong, int);
  	ulong (XWPSFontCid2::*known_encodec2)(ulong, int);
  	ulong (XWPSFontCid0::*known_encodec0)(ulong, int);
  }known_encode_;
} ;

struct PSFontProcs
{
	union
	{
		int (XWPSFont::*define_font)(XWPSFontDir *);
		int (XWPSFontBase::*define_fontb)(XWPSFontDir *);
		int (XWPSFontType0::*define_fontt0)(XWPSFontDir *);
		int (XWPSFontType1::*define_fontt1)(XWPSFontDir *);
		int (XWPSFontType42::*define_fontt42)(XWPSFontDir *);
		int (XWPSFontCid1::*define_fontc1)(XWPSFontDir *);
		int (XWPSFontCid2::*define_fontc2)(XWPSFontDir *);
		int (XWPSFontCid0::*define_fontc0)(XWPSFontDir *);
	}define_font_;
	
	union
	{
		int (XWPSFont::*make_font)(XWPSFontDir *, const XWPSMatrix *, XWPSFont **);
		int (XWPSFontBase::*make_fontb)(XWPSFontDir *, const XWPSMatrix *, XWPSFont **);
		int (XWPSFontType0::*make_fontt0)(XWPSFontDir *, const XWPSMatrix *, XWPSFont **);
		int (XWPSFontType1::*make_fontt1)(XWPSFontDir *, const XWPSMatrix *, XWPSFont **);
		int (XWPSFontType42::*make_fontt42)(XWPSFontDir *, const XWPSMatrix *, XWPSFont **);
		int (XWPSFontCid1::*make_fontc1)(XWPSFontDir *, const XWPSMatrix *, XWPSFont **);
		int (XWPSFontCid2::*make_fontc2)(XWPSFontDir *, const XWPSMatrix *, XWPSFont **);
		int (XWPSFontCid0::*make_fontc0)(XWPSFontDir *, const XWPSMatrix *, XWPSFont **);
	}make_font_;
	
	union
	{
		int (XWPSFont::*font_info)(const XWPSPoint *, int , XWPSFontInfo*);
		int (XWPSFontBase::*font_infob)(const XWPSPoint *, int , XWPSFontInfo*);
		int (XWPSFontType0::*font_infot0)(const XWPSPoint *, int , XWPSFontInfo*);
		int (XWPSFontType1::*font_infot1)(const XWPSPoint *, int , XWPSFontInfo*);
		int (XWPSFontType42::*font_infot42)(const XWPSPoint *, int , XWPSFontInfo*);
		int (XWPSFontCid1::*font_infoc1)(const XWPSPoint *, int , XWPSFontInfo*);
		int (XWPSFontCid2::*font_infoc2)(const XWPSPoint *, int , XWPSFontInfo*);
		int (XWPSFontCid0::*font_infoc0)(const XWPSPoint *, int , XWPSFontInfo*);
	}font_info_;
	
	union
	{
		int (XWPSFont::*same_font)(const XWPSFont *, int );
		int (XWPSFontBase::*same_fontb)(const XWPSFont *, int );
		int (XWPSFontType0::*same_fontt0)(const XWPSFont *, int );
		int (XWPSFontType1::*same_fontt1)(const XWPSFont *, int );
		int (XWPSFontType42::*same_fontt42)(const XWPSFont *, int );
		int (XWPSFontCid1::*same_fontc1)(const XWPSFont *, int );
		int (XWPSFontCid2::*same_fontc2)(const XWPSFont *, int );
		int (XWPSFontCid0::*same_fontc0)(const XWPSFont *, int );
	}same_font_;
	
	union
	{
		ulong (XWPSFont::*encode_char)(ulong, PSGlyphSpace);
		ulong (XWPSFontBase::*encode_charb)(ulong, PSGlyphSpace);
		ulong (XWPSFontType0::*encode_chart0)(ulong, PSGlyphSpace);
		ulong (XWPSFontType1::*encode_chart1)(ulong, PSGlyphSpace);
		ulong (XWPSFontType42::*encode_chart42)(ulong, PSGlyphSpace);
		ulong (XWPSFontCid1::*encode_charc1)(ulong, PSGlyphSpace);
		ulong (XWPSFontCid2::*encode_charc2)(ulong, PSGlyphSpace);
		ulong (XWPSFontCid0::*encode_charc0)(ulong, PSGlyphSpace);
	}encode_char_;
	
	union
	{
		int (XWPSFont::*enumerate_glyph)(int *pindex, PSGlyphSpace ,ulong *);
		int (XWPSFontBase::*enumerate_glyphb)(int *pindex, PSGlyphSpace ,ulong *);
		int (XWPSFontType0::*enumerate_glypht0)(int *pindex, PSGlyphSpace ,ulong *);
		int (XWPSFontType1::*enumerate_glypht1)(int *pindex, PSGlyphSpace ,ulong *);
		int (XWPSFontType42::*enumerate_glypht42)(int *pindex, PSGlyphSpace ,ulong *);
		int (XWPSFontCid1::*enumerate_glyphc1)(int *pindex, PSGlyphSpace ,ulong *);
		int (XWPSFontCid2::*enumerate_glyphc2)(int *pindex, PSGlyphSpace ,ulong *);
		int (XWPSFontCid0::*enumerate_glyphc0)(int *pindex, PSGlyphSpace ,ulong *);
	}enumerate_glyph_;
	
	union
	{
		int (XWPSFont::*glyph_info)(ulong , const XWPSMatrix *,int , XWPSGlyphInfo *);
		int (XWPSFontBase::*glyph_infob)(ulong , const XWPSMatrix *,int , XWPSGlyphInfo *);
		int (XWPSFontType0::*glyph_infot0)(ulong , const XWPSMatrix *,int , XWPSGlyphInfo *);
		int (XWPSFontType1::*glyph_infot1)(ulong , const XWPSMatrix *,int , XWPSGlyphInfo *);
		int (XWPSFontType42::*glyph_infot42)(ulong , const XWPSMatrix *,int , XWPSGlyphInfo *);
		int (XWPSFontCid1::*glyph_infoc1)(ulong , const XWPSMatrix *,int , XWPSGlyphInfo *);
		int (XWPSFontCid2::*glyph_infoc2)(ulong , const XWPSMatrix *,int , XWPSGlyphInfo *);
		int (XWPSFontCid0::*glyph_infoc0)(ulong , const XWPSMatrix *,int , XWPSGlyphInfo *);
	}glyph_info_;
	
	union
	{
		int (XWPSFont::*glyph_outline)(ulong , const XWPSMatrix *,XWPSPath *ppath);
		int (XWPSFontBase::*glyph_outlineb)(ulong , const XWPSMatrix *,XWPSPath *ppath);
		int (XWPSFontType0::*glyph_outlinet0)(ulong , const XWPSMatrix *,XWPSPath *ppath);
		int (XWPSFontType1::*glyph_outlinet1)(ulong , const XWPSMatrix *,XWPSPath *ppath);
		int (XWPSFontType42::*glyph_outlinet42)(ulong , const XWPSMatrix *,XWPSPath *ppath);
		int (XWPSFontCid1::*glyph_outlinec1)(ulong , const XWPSMatrix *,XWPSPath *ppath);
		int (XWPSFontCid2::*glyph_outlinec2)(ulong , const XWPSMatrix *,XWPSPath *ppath);
		int (XWPSFontCid0::*glyph_outlinec0)(ulong , const XWPSMatrix *,XWPSPath *ppath);
	}glyph_outline_;
	
	union
	{
		int (XWPSFont::*init_fstack)(XWPSTextEnum *);
		int (XWPSFontBase::*init_fstackb)(XWPSTextEnum *);
		int (XWPSFontType0::*init_fstackt0)(XWPSTextEnum *);
		int (XWPSFontType1::*init_fstackt1)(XWPSTextEnum *);
		int (XWPSFontType42::*init_fstackt42)(XWPSTextEnum *);
		int (XWPSFontCid1::*init_fstackc1)(XWPSTextEnum *);
		int (XWPSFontCid2::*init_fstackc2)(XWPSTextEnum *);
		int (XWPSFontCid0::*init_fstackc0)(XWPSTextEnum *);
	}init_fstack_;
	
	union
	{
		int (XWPSTextEnum::*next_char_glyph)(ulong *, ulong *);
		int (XWPSShowEnum::*next_char_glyphshw)(ulong *, ulong *);
		int (XWPSBboxTextEnum::*next_char_glyphbbox)(ulong *, ulong *);
		int (XWPSPDFTextEnum::*next_char_glyphpdf)(ulong *, ulong *);
		int (XWPSDVITextEnum::*next_char_glyphdvi)(ulong *, ulong *);
	}next_char_glyph_;
	
	union
	{
		int (XWPSFont::*build_char)(XWPSTextEnum *, XWPSState *, ulong, ulong);
		int (XWPSFontBase::*build_charb)(XWPSTextEnum *, XWPSState *, ulong, ulong);
		int (XWPSFontType0::*build_chart0)(XWPSTextEnum *, XWPSState *, ulong, ulong);
		int (XWPSFontType1::*build_chart1)(XWPSTextEnum *, XWPSState *, ulong, ulong);
		int (XWPSFontType42::*build_chart42)(XWPSTextEnum *, XWPSState *, ulong, ulong);
		int (XWPSFontCid1::*build_charc1)(XWPSTextEnum *, XWPSState *, ulong, ulong);
		int (XWPSFontCid2::*build_charc2)(XWPSTextEnum *, XWPSState *, ulong, ulong);
		int (XWPSFontCid0::*build_charc0)(XWPSTextEnum *, XWPSState *, ulong, ulong);
	}build_char_;
		
	PSXFontCallbacks callbacks;
};

class XWPSFont : public XWPSStruct
{
public:
	XWPSFont();
	virtual ~XWPSFont();
	
	int addSubsetPieces(ulong *glyphs, 
	                    uint *pcount, 
	                    uint max_count,
		                  uint max_pieces);
	
	int buildChar(XWPSTextEnum * pte, 
	                      XWPSState *pgs, 
		                    ulong chr, 
		                    ulong glyph);
	int buildCharNo(XWPSTextEnum * , 
	                      XWPSState *, 
		                    ulong , 
		                    ulong ) {return 1;}
	int charBbox(XWPSRect *pbox, ulong *pglyph, int ch, const XWPSMatrix *pmat);
	void computePost(PSPost *post);
	
	virtual XWPSFont * copyFont();
	        void copyFontFont(XWPSFont * newfont);
	
	int defineFont(XWPSFontDir * pdir);
	int defineFontNo(XWPSFontDir * ) {return 0;}
	
	ulong encodeChar(ulong chr,  PSGlyphSpace glyph_space);
	ulong encodeCharNo(ulong ,  PSGlyphSpace ) {return ps_no_glyph;}
	int   enumerateGlyph(int *pindex, 
	                     PSGlyphSpace glyph_space, 
	                     ulong *pglyph);
	int  enumerateGlyphNo(int *, 
	                      PSGlyphSpace , 
	                      ulong *);
	void findCharRange(int *pfirst, int *plast);
	int   fontInfo(const XWPSPoint *pscale, 
	                       int members,
	                       XWPSFontInfo *info);
	int   fontInfoDefault(const XWPSPoint *pscale, 
	                       int members,
	                       XWPSFontInfo *info);
	
	virtual int getLength();
	virtual const char * getTypeName();
	int   getIndex(XWPSString *pgstr, int count, ulong *pval);
	int   glyphInfo(ulong glyph, 
	                        const XWPSMatrix * pmat, 
	                        int members, 
	                        XWPSGlyphInfo *info);
	int   glyphInfoDefault(ulong glyph, 
	                        const XWPSMatrix * pmat, 
	                        int members, 
	                        XWPSGlyphInfo *info);
	const char * glyphName(ulong index, uint *plen);
	int   glyphOutline(ulong glyph, const XWPSMatrix * pmat, XWPSPath *ppath);
	int   glyphOutlineNo(ulong, const XWPSMatrix *, XWPSPath *);
	ulong glyphToIndex(ulong glyph);
	
	int initFStack(XWPSTextEnum *pte);
	int initFStackDefault(XWPSTextEnum *pte);
	       
	ulong knownEncode(ulong chr, int encoding_index);
	
	static void fontLinkFirst(XWPSFont **pfirst, XWPSFont *elt);
	
	XWPSCachedFMPair * lookupFMPair(XWPSState * pgs);
	XWPSCachedChar * lookupCachedChar(XWPSCachedFMPair * pair,
		                                ulong glyph, 
		                                int wmode, 
		                                int alt_depth);
	int macGlyphIndex(int ch, XWPSString *pstr);	
	int makeFont(XWPSFontDir * pdir,
	             const XWPSMatrix * pmat, 
	             XWPSFont ** ppfont);
	int makeFontBase(XWPSFontDir * pdir,
	             const XWPSMatrix * pmat, 
	             XWPSFont ** ppfont);
	int makeFontNo(XWPSFontDir * ,
	             const XWPSMatrix * , 
	             XWPSFont ** ) {return 0;}
	             
	int nextCharGlyph(XWPSTextEnum *pte, ulong *pchr, ulong *pglyph);
	
	void purgeFont();
	
	int sameFont(const XWPSFont *ofont, int mask);
	int sameFontBase(const XWPSFont *ofont, int mask);
	int sameFontDefault(const XWPSFont *ofont, int mask);
	uint sizeCmap(uint first_code, 
	              int num_glyphs, 
	              ulong max_glyph,
	  						int options);
	uint sizeName(const XWPSString *font_nameA);
	virtual const XWPSCidSystemInfo * getCIDSysInfo() {return 0;}
	int subsetGlyphs(ulong * glyphs, const uchar * used);
	
	void updateOS2(ttf_OS_2_t *pos2, uint first_glyph, int num_glyphs);
	bool usesCharStrings() 
	 {return (FontType == ft_encrypted || FontType == ft_encrypted2 || FontType == ft_disk_based);}
	
	void writeCmap(XWPSStream *s, 
	                uint first_code, 
	                int num_glyphs,
	                ulong max_glyph, 
	                int options, 
	                uint cmap_length);
	void writeName(XWPSStream *s, const XWPSString *font_nameA);
	void writeOS2(XWPSStream *s, uint first_glyph, int num_glyphs);
	void writePost(XWPSStream *s, PSPost *post);
	
	int zbaseMakeFont(XWPSFontDir * pdir, 
		               const XWPSMatrix * pmat, 
		               XWPSFont ** ppfont);
	int zdefaultMakeFont(XWPSFontDir * pdir, 
		                   const XWPSMatrix * pmat, 
		                   XWPSFont ** ppfont);
	ulong zfontEncodeChar(ulong chr, PSGlyphSpace ignored);
	const char * zfontGlyphName(ulong index, uint * plen);
	ulong zfontKnownEncode(ulong chr, int encoding_index);
	
public:
	XWPSFont *next, *prev;
	XWPSFontDir *dir;
	bool is_resource;
	ulong id;
	XWPSFont *base;
	void *client_data;
	XWPSMatrix FontMatrix;
	PSFontType FontType;
	bool BitmapWidths;
	FBitType ExactSize, InBetweenSize, TransformedChar;
	int WMode;
	int PaintType;
	float StrokeWidth;
	PSFontProcs procs;
	PSFontName key_name, font_name;
	XWPSContextState * i_ctx_p;
	uchar * pfont_data;
	uchar * not_def;
};

typedef int (*glyph_data_proc_t)(XWPSFontBase *, ulong, XWPSString *, XWPSFontType1 **);

int psf_type1_glyph_data(XWPSFontBase *pbfont, ulong glyph, XWPSString *pstr, XWPSFontType1 **ppfont);

class XWPSFontBase : public XWPSFont
{
public:
	XWPSFontBase();
	virtual ~XWPSFontBase();
	
	int checkOutlineGlyphs(XWPSPSFGlyphEnum *ppge, glyph_data_proc_t glyph_data);
	
	bool considerFontSymbolic();
	
	virtual XWPSFont * copyFont();
	        void copyFontBase(XWPSFontBase * newfont);
	
	bool encodingHasGlyph(ulong font_glyph, PSEncodingIndex index);
	
	virtual int getLength();
	virtual const char * getTypeName();
	virtual int glyphData(ulong, XWPSString *, int *) {return 0;}
	        int getOutlineGlyphs(XWPSPSFOutlineGlyphs *pglyphs, 
		       										 ulong *orig_subset_glyphs, 
		       										 uint orig_subset_size,
		                           glyph_data_proc_t glyph_data);
		                           
	void initSimpleFont(const double bbox[4], const XWPSUid *puid);
	bool isSymbolic();
	
	ulong zfontNoEncodeChar(ulong , PSGlyphSpace ) {return ps_no_glyph;}
	
public:
	XWPSRect FontBBox;
	XWPSUid UID;
	PSEncodingIndex encoding_index;
	PSEncodingIndex nearest_encoding_index;
};

#define max_BlueValues 7
#define max_FamilyBlues 7
#define max_FamilyOtherBlues 5
#define max_OtherBlues 5
#define max_StemSnap 12
#define max_WeightVector 16
#define max_snaps (1 + max_StemSnap)
#define max_a_zones (max_BlueValues + max_OtherBlues)
#define max_total_stem_hints 96
#define max_stems 96

#define CD_LONGINT 29
#define CD_REAL 30
#define NUM_STD_STRINGS 391
#define SKIP_EMPTY_SUBRS
#define CE_OFFSET 32

#define WRITE_TYPE2_NO_LENIV 1	
#define WRITE_TYPE2_CHARSTRINGS 2 
#define WRITE_TYPE2_AR3 4

class XWPSPixelScale
{
public:
	XWPSPixelScale() : unit(0), half(0), log2_unit(0) {}
	
	long scaledRounded(long v) {return ((v + half) & -unit);}
	
	void set(int log2);
		
public:
	long unit;
	long half;
	int log2_unit;
};

class XWPSPointScale
{
public:
	XWPSPointScale() {}
	
	void copy(const XWPSPointScale * other);
	
	void set(const XWPSLog2ScalePoint * pscale);
	
public:
	XWPSPixelScale x, y;
};

class XWPSStemSnapTable
{
public:
	XWPSStemSnapTable();
	
	void copy(const XWPSStemSnapTable * other);
	
public:
	int count;
  long data[max_snaps];
};

class XWPSAlignmentZone
{
public:
	XWPSAlignmentZone() : is_top_zone(0), v0(0), v1(0), flat(0) {}
	
public:
	int is_top_zone;
  long v0, v1;
  long flat;
};

class XWPSFontHints
{
public:
	XWPSFontHints();
	
	void computeFontHints(const XWPSMatrixFixed * pmat,
                        const XWPSLog2ScalePoint * plog2_scale, 
                        const XWPSFontType1 * pdata);
	void computeSnaps(const XWPSMatrixFixed * pmat, 
	                  const PSAStemTable * pst,
	                  XWPSStemSnapTable * psst, 
	                  int from_y, 
	                  int to_y, 
	                  const char *tname);
	XWPSAlignmentZone * computeZones(const XWPSMatrixFixed * pmat, 
	                                 const PSAZoneTable * blues, 
	                                 const PSAZoneTable * family_blues,
	                                 XWPSAlignmentZone * zp, 
	                                 int bottom_count);
	      
	void copy(const XWPSFontHints * other);
	
	void reset(const XWPSLog2ScalePoint * plog2_scale);
	
	int transformZone(const XWPSMatrixFixed * pmat, 
	       						const float *vp, 
	       						XWPSAlignmentZone * zp);
	
public:
	bool axes_swapped;
	bool x_inverted, y_inverted;
	bool use_x_hints;
	bool use_y_hints;
	XWPSPointScale scale;
	XWPSStemSnapTable snap_h;
	XWPSStemSnapTable snap_v;
	long blue_fuzz, blue_shift;
	bool suppress_overshoot;
	int a_zone_count;
	XWPSAlignmentZone a_zones[max_a_zones];
};

class XWPSStemHint
{
public:
	XWPSStemHint() : v0(0),v1(0),dv0(0),dv1(0),index(0),active(0) {}
	
public:
	long v0, v1;
	long dv0, dv1;
	ushort index;
	ushort active;
};

class XWPSStemHintTable
{
public:
	XWPSStemHintTable();
	
	void copy(const XWPSStemHintTable * other);
	
	void enableHints(const uchar * mask);
	
	void saveReplacedHints();
	void storeStemDeltas(XWPSStemHint * psh,
		                   XWPSPixelScale * psp, 
		                   long v, 
		                   long dv, 
		                   long adj_dv);
	
public:
	int count;
  int current;
  int replaced_count;
  XWPSStemHint data[max_stems];
};

#define charstring_this(ch, state, encrypted)\
  (encrypted ? decrypt_this(ch, state) : ch)
#define charstring_next(ch, state, chvar, encrypted)\
  (encrypted ? (chvar = decrypt_this(ch, state),\
		decrypt_skip_next(ch, state)) :\
   (chvar = ch))
#define charstring_skip_next(ch, state, encrypted)\
  (encrypted ? decrypt_skip_next(ch, state) : 0)
  	
class XWPSIPState
{
public:
	XWPSIPState();
	
public:
	uchar *ip;
  ushort dstate;
  XWPSString char_string;
};

#define ostack_size 48
#define MAX_STACK ostack_size
#define ipstack_size 10	
#define flex_max 8
#define dotsection_in 0
#define dotsection_out (-1)

#define HINT_VERT_LOWER 1
#define HINT_VERT_UPPER 2	/* must be > lower */
#define HINT_VERT (HINT_VERT_LOWER | HINT_VERT_UPPER)
#define HINT_HORZ_LOWER 4
#define HINT_HORZ_UPPER 8	/* must be > lower */
#define HINT_HORZ (HINT_HORZ_LOWER | HINT_HORZ_UPPER)

#define FORCE_HINTS_TO_BIG_PIXELS 1

#define c_value_num1(ch) ((int)(uchar)(ch) - 139)
#define c_value_pos2(c1,c2)\
  (((int)(uchar)((c1) - (int)c_pos2_0) << 8) + (int)(uchar)(c2) + 108)
  
#define c_value_neg2(c1,c2)\
  -(((int)(uchar)((c1) - (int)c_neg2_0) << 8) + (int)(uchar)(c2) + 108)

#define CLEAR_CSTACK(cstack, csp)\
  (csp = (long*)(cstack) - 1)
  
#define INIT_CSTACK(cstack, csp)\
    if ( os_count == 0 )\
      CLEAR_CSTACK(cstack, csp);\
    else {\
      memcpy(cstack, ostack, os_count * sizeof(long));\
      csp = &cstack[os_count - 1];\
    }
    
#define decode_num1(var, c)\
  (var = c_value_num1(c))
#define decode_push_num1(csp, c)\
  (*++csp = int2fixed(c_value_num1(c)))
  
#define decode_num2(var, c, cip, state, encrypted)\
    do{uint c2 = *cip++;\
    int cn = charstring_this(c2, state, encrypted);\
\
    var = (c < c_neg2_0 ? c_value_pos2(c, 0) + cn :\
	   c_value_neg2(c, 0) - cn);\
    charstring_skip_next(c2, state, encrypted);}while(0)
    
#define decode_push_num2(csp, c, cip, state, encrypted)\
    do{uint c2 = *cip++;\
    int cn;\
\
    cn = charstring_this(c2, state, encrypted);\
    if ( c < c_neg2_0 )\
      { \
        *++csp = int2fixed(c_value_pos2(c, 0) + (int)cn);\
      }\
    else\
      { \
        *++csp = int2fixed(c_value_neg2(c, 0) - (int)cn);\
      }\
    charstring_skip_next(c2, state, encrypted);}while(0)
    
#define decode_num4(lw, cip, state, encrypted)\
    do{int i;\
    uint c4;\
\
    lw = 0;\
    for ( i = 4; --i >= 0; )\
      { charstring_next(*cip, state, c4, encrypted);\
        lw = (lw << 8) + c4;\
	cip++;\
      }\
    sign_extend_num4(lw);}while(0)
    
#define check_first_operator(explicit_width)\
  do{\
    if ( init_done < 0 )\
      { ipsp->ip = cip, ipsp->dstate = state;\
	return type2Sbw(csp, cstack, ipsp, explicit_width);\
      }\
  }while(0)

typedef int (XWPSType1State::*charstring_interpret_proc_t)(const XWPSString *, int *);

#define type1_result_sbw 1
#define type1_result_callothersubr 2

class XWPSType1State
{
public:
	XWPSType1State();
	~XWPSType1State();
	
	void addHintDiff(XWPSFixedPoint * ppt, XWPSFixedPoint delta)
	 {ppt->x += delta.x; ppt->y += delta.y;}
	void applyHintsAt(int hints, XWPSFixedPoint * ppt, XWPSFixedPoint * pdiff);
	void applyHStemHints(int dx, XWPSFixedPoint * ppt);
	void applyPathHints(bool closing);
	void applyVStemHints(int dy, XWPSFixedPoint * ppt);
	void applyWrappedHints(XWPSSubpath * psub, 
	                       XWPSSegment * pseg,
		                     int hints, 
		                     XWPSFixedPoint * pdiff);
	
	void centerVStem(long x0, long dx);
	void copy(XWPSType1State * other);
	
	void detectEdgeHint(long *xy, long *dxy);
	
	long findSnap(long dv, 
	              XWPSStemSnapTable * psst, 
	              XWPSPixelScale * pps);
	XWPSAlignmentZone * findZone(long vbot, long vtop);
	
	int lineHints(const XWPSFixedPoint * p0, const XWPSFixedPoint * p1);
	bool lineIsNull(XWPSFixedPoint p0, XWPSFixedPoint p1)
	 {return (qAbs(p1.x - p0.x) + qAbs(p1.y - p0.y) < fixed_epsilon * 4);}
	
	bool nearlyAxial(long dmajor, long dminor) {return (dminor <= dmajor >> 4);}
	
	void replaceStemHints();
	void resetStemHints();
	
	XWPSStemHint * searchHints(XWPSStemHintTable * psht, long v);
		
	void setCallbackData(void *d) {callback_data=d;}
	void setLSB(const XWPSPoint * psbpt);
	void setWidth(const XWPSPoint * pwpt);
	
	void skipiv();
	
	void type1ApplyPathHints(bool closing, XWPSPath * ppath);
	int  type1CallSubr(int index);
	void type1CisGetMetrics(double psbw[4]);
	void type1Clear() {os_count = 0;}
	int  type1Blend(long *csp, int num_results);
	void type1DoCenterVStem(long x0, long dx, const XWPSMatrixFixed * pmat);
	void type1DoHStem(long y, long dy, const XWPSMatrixFixed * pmat);
	void type1DoVStem(long x, long dx, const XWPSMatrixFixed * pmat);
	int  type1EndChar();
	int  type1ExecInit(XWPSTextEnum *penum,
		                 XWPSState *pgs, 
		                 XWPSFontType1 *pfont1);
	void type1FinishInit(XWPSOp1State * ps);
	void type1HStem(long y, long dy);
	int  type1InterpInit(XWPSImagerState * pisA,
                       XWPSPath * ppath, 
                       const XWPSLog2ScalePoint * pscale,
                       bool charpath_flagA,
		     							 int paint_typeA, XWPSFontType1 * pfontA);
	int  type1Interpret(const XWPSString * str, int *pindex);
	int  type1Next();
	void type1NextInit(const XWPSString *pstr, XWPSFontType1 *pfontA);
	void type1ReplaceStemHints();
	int  type1Sbw(long lsbx, long lsby, long wx, long wy);
	int  type1Seac(const long * cstack, long asb, XWPSIPState * ipsp);
	XWPSStemHint * type1Stem(XWPSStemHintTable * psht,  long v0, long d);
	int  type1Stem1(XWPSStemHintTable *psht, const long *pv, uchar *active_hints);
	void type1Stem3(XWPSStemHintTable *psht, const long *pv3, uchar *active_hints);
	void type1VStem(long x, long dx);
	
	int type2Interpret(const XWPSString * str, int *ignore_pindex);
	int type2Sbw(long * csp, long * cstack, XWPSIPState * ipsp,  bool explicit_width);
	int type2VStem(long * csp, long * cstack);
	
	void updateStemHints();
	
public:
	XWPSFontType1 *pfont;
	XWPSImagerState *pis;
	XWPSPath *path;
	bool charpath_flag;
	int paint_type;
	void *callback_data;
	XWPSFixedCoeff fc;
	float flatness;
	XWPSPointScale scale;
	XWPSFontHints fh;
	XWPSFixedPoint origin;
	long ostack[ostack_size];
	int os_count;
	XWPSIPState ipstack[ipstack_size + 1];
	int ips_count;
	int init_done;
	bool sb_set;
	bool width_set;
	bool have_hintmask;
	int num_hints;
	XWPSFixedPoint lsb;
	XWPSFixedPoint width;
	int seac_accent;
	long save_asb;
	XWPSFixedPoint save_lsb;
	XWPSFixedPoint save_adxy;
	long asb_diff;	
	XWPSFixedPoint adxy;
	XWPSFixedPoint position;
	int flex_path_state_flags;
	XWPSFixedPoint flex_points[flex_max];
	int flex_count;
  int ignore_pops;
  int dotsection_flag;
  bool vstem3_set;
  XWPSFixedPoint vs_offset;	
  int hints_initial;
  XWPSFixedPoint unmoved_start;
  XWPSSegment *hint_next;
  int hints_pending;
  XWPSFixedPoint unmoved_end;
  XWPSStemHintTable hstem_hints;
  XWPSStemHintTable vstem_hints;
  long transient_array[32];
};

#define max_coeff_bits 11

class XWPSOp1State
{
public:
	XWPSOp1State();
	~XWPSOp1State();
	
	void accumXYProc(long dx, long dy);
	
	int closePath();
	
	int rrcurveto(long dx1, long dy1,
		            long dx2, long dy2, 
		            long dx3, long dy3);
	
public:
	XWPSPath *ppath;
  XWPSType1State *pcis;
  XWPSFixedCoeff fc;
  XWPSFixedPoint co;
  XWPSFixedPoint p;
};

class XWPSCffStringItem
{
public:
	XWPSCffStringItem();
	
public:
	XWPSString key;
	int index1;
};

class XWPSCffStringTable
{
public:
	XWPSCffStringTable();
	~XWPSCffStringTable();
	
	int  add(const uchar *data, uint sizeA);
	
	int  index(const uchar *data, uint sizeA,  bool enter, int *pindex);
	void init(XWPSCffStringItem *itemsA, int sizeA);
	
public:
	XWPSCffStringItem *items;
  int count;
  int size;
  uint total;
  int reprobe;
};

class XWPSCffGlyphSubset
{
public:
	XWPSCffGlyphSubset();
	~XWPSCffGlyphSubset();
	
public:
	XWPSPSFOutlineGlyphs glyphs;
	int num_encoded;
	int num_encoded_chars;
};

class XWPSCffWriter
{
public:
	XWPSCffWriter();
	~XWPSCffWriter();
	
	bool convertCharStrings(const XWPSFontBase *pfont);
	
	int extraLenIV(const XWPSFontType1 *pfont);
	
	uint FDSelectSize();
	
	int glyphSid(ulong glyph);
	
	static int offsetSize(uint offset);
	
	void putBool(bool b);
	void putBoolValue(bool b, int op);
	void putCard16(uint c16);
	int  putCharString(const uchar *data, uint size, XWPSFontType1 *pfont);
	void putIndex(const XWPSCffStringTable *pcst);
	void putIndexHeader(uint count, uint total);
	void putInt(int i);
	void putIntIfNe(int i, int i_default, int op);
	void putIntValue(int i, int op);
	void putOffset(int offset);
	void putOp(int op);
	void putReal(double f);
	void putRealDeltArray(const float *pf, int count, int op);
	void putRealIfNe(double f, double f_default, int op);
	void putRealValue(double f, int op);
	int  putString(const uchar *data, uint size);
	int  putStringValue(const uchar *data, uint size, int op);
	
	int stringSid(const uchar *data, uint size);
	
	int  writeCharset(XWPSCffGlyphSubset *pgsub);
	void writeCharStrings(XWPSPSFGlyphEnum *penum,
		      		          uint charstrings_count, 
		      		          uint charstrings_size);
	int  writeCharStringsOffsets(XWPSPSFGlyphEnum *penum,  uint *pcount);
	int  writeCidSet(XWPSPSFGlyphEnum *penum);
	int  writeEncoding(XWPSCffGlyphSubset *pgsub);
	void writeFDArrayOffsets(uint *FDArray_offsets, int num_fonts);
	int  writeFDSelect(uint size);
	void writePrivate(uint Subrs_offset, const XWPSFontType1 *pfont);
	void writeROS(const XWPSCidSystemInfo *pcidsi);
	void writeSubrs(uint subrs_count, 
	                uint subrs_size,
		              XWPSFontType1 *pfont);
	uint writeSubrsOffsets(uint *pcount, XWPSFontType1 *pfont);
	void writeTopCidFont(uint charset_offset,
		      						 uint CharStrings_offset, 
		      						 uint FDSelect_offset,
		                   uint Font_offset);
	void writeTopCommon(XWPSFontBase *pbfont,  bool full_info);
	void writeTopFDArray(XWPSFontBase *pbfont,
		      						 uint Private_offset, 
		      						 uint Private_size);
	void writeTopFont(uint Encoding_offset,
		   						  uint charset_offset, 
		   						  uint CharStrings_offset,
		                uint Private_offset, 
		                uint Private_size);
	
public:
	int options;
  XWPSStream *strm;
  XWPSFontBase *pfont;
  glyph_data_proc_t glyph_data;
  int offset_size;
  long start_pos;
  XWPSCffStringTable std_strings;
  XWPSCffStringTable strings;
};

#define WRITE_TYPE1_EEXEC 1
#define WRITE_TYPE1_ASCIIHEX 2  
#define WRITE_TYPE1_EEXEC_PAD 4  
#define WRITE_TYPE1_EEXEC_MARK 8  
#define WRITE_TYPE1_POSTSCRIPT 16  
#define WRITE_TYPE1_WITH_LENIV 32

#define WRITE_TYPE2_NO_LENIV 1	
#define WRITE_TYPE2_CHARSTRINGS 2 
#define WRITE_TYPE2_AR3 4

#define TYPE2_OPTIONS WRITE_TYPE2_CHARSTRINGS

struct PSType1DataProcs
{
	int (XWPSFontType1::*glyph_data)(ulong , XWPSString * );
	int (XWPSFontType1::*subr_data) (int , bool , XWPSString *);
	int (XWPSFontType1::*seac_data)(int , ulong * , XWPSString * );
	int (*push_values)(void *, const long *,  int );
	int (*pop_value)(void *, long *value);
};

class XWPSFontType1 : public XWPSFontBase
{
public:
	XWPSFontType1();
	~XWPSFontType1();
	
	int callInterpret(XWPSType1State * st, const XWPSString * str, int *pindex);
	static uint cffEncodingSize(int num_encoded, int num_encoded_chars);
	static uint cffIndexSize(uint count, uint total);	
	int  charstringFontInit(XWPSCharstringFontRefs *pfr);
	int convertType1ToType2(XWPSStream *s, const XWPSString *pstr);
	
	virtual XWPSFont * copyFont();
	        void copyFontType1(XWPSFontType1 * newfont);
	
	int getLength();
	const char * getTypeName();
	int   getType1Glyphs(XWPSPSFOutlineGlyphs *pglyphs, 
	                     ulong *subset_glyphs, 
	                     uint subset_size);
	int   glyphData(ulong glyph, XWPSString * pgdata);
	        
	int  seacData(int ccode, ulong * pglyph, XWPSString *pstr);
	int subrData(int subr_num, bool global,	XWPSString * psdata);
	
	int  type1GlyphInfo(ulong glyph, 
	                    const XWPSMatrix *pmat,
		                  int members, 
		                  XWPSGlyphInfo *info);
	void type2PutFixed(XWPSStream *s,long v);
	void type2PutHintMask(XWPSStream *s, const uchar *mask, uint size);
	void type2PutInt(XWPSStream *s,int i);
	void type2PutOp(XWPSStream *s,int op);
	void type2PutStems(XWPSStream *s,const XWPSStemHintTable *psht, int op);
	
	int writeType1Font(XWPSStream *s, 
	          int options,
		        ulong * orig_subset_glyphs, 
		        uint orig_subset_size,
		        const XWPSString *alt_font_name, 
		        int * lengths);
	int  writeEncoding(XWPSStream *s, 
	                   int options,
	                   ulong *subset_glyphs, 
	                   uint subset_size, 
	                   ulong notdef);
	void writeFontName(XWPSStream *s, const XWPSString *alt_font_name);
	int  writePrivate(XWPSStream *s, 
	      						ulong *subset_glyphs, 
	      						uint subset_size,
	                  ulong notdef, 
	                  int lenIVA,
	      						const PSPrinterParams *ppp,
	      						bool e = false);
	      						
	int writeType2Font(XWPSStream *s, 
	                   int options,
		                 ulong *subset_glyphs, 
		                 uint subset_size,
		      					 const XWPSString *alt_font_name);
		      					 
	int zchar1GlyphOutline(ulong glyph, const XWPSMatrix *pmat, XWPSPath *ppath);
	int z1enumerateGlyph(int *pindex, PSGlyphSpace ignored, ulong * pglyph);
	int z1fontInfo(const XWPSPoint *pscale, int members, XWPSFontInfo *info);
	int z1glyphData(ulong glyph, XWPSString * pstr);
	int z1glyphInfo(ulong glyph, 
	                const XWPSMatrix *pmat,
	                int members, 
	                XWPSGlyphInfo *info);
	int z1sameFont(const XWPSFont *ofont, int mask);
	int z1seacData(int ccode, ulong *pglyph, XWPSString *pstr);
	int z1subrData(int index, bool global, XWPSString * pstr);
	int z9FDArrayGlyphData(ulong glyph, XWPSString * pgdata);
	int z9FDArraySeacData(int ccode, ulong *pglyph, XWPSString *pstr);
	
public:
	PSType1DataProcs type1Procs;
  charstring_interpret_proc_t interpret;
  void *proc_data;
	int lenIV;
	uint subroutineNumberBias;
	uint gsubrNumberBias;
	long initialRandomSeed;
	long defaultWidthX;
  long nominalWidthX;
  int BlueFuzz;
  float BlueScale;
  float BlueShift;

	struct 
	{
		int count;
		float values[max_BlueValues*2];
	} BlueValues;
	
  float ExpansionFactor;
  bool ForceBold;

	struct 
	{
		int count;
		float values[max_FamilyBlues*2];
	} FamilyBlues;
  
  struct 
	{
		int count;
		float values[max_FamilyOtherBlues*2];
	} FamilyOtherBlues;
	
  int LanguageGroup;

	struct 
	{
		int count;
		float values[max_OtherBlues*2];
	} OtherBlues;
	
  bool RndStemUp;
  struct 
  {
		int count;
		float values[1];
	} StdHW;
	
	struct 
  {
		int count;
		float values[1];
	} StdVW;
	
  struct 
  {
		int count;
		float values[max_StemSnap];
	} StemSnapH;
	
	struct 
  {
		int count;
		float values[max_StemSnap];
	} StemSnapV;
  
  struct 
  {
		int count;
		float values[max_WeightVector];
	} WeightVector;
};

#define MAX_FDBytes 4

class XWPSFontCid0 : public XWPSFontBase
{
public:
	XWPSFontCid0();
	~XWPSFontCid0();
	
	int cid0EnumerateGlyph(int *pindex,
			                   PSGlyphSpace ignore_glyph_space,
			                   ulong *pglyph);
	int cid0ReadBytes(ulong base, uint count, uchar *buf,	XWPSString *pstr);
	virtual XWPSFont * copyFont();
	        void copyFontCid0(XWPSFontCid0 * newfont);
	
	int getLength();
	const char * getTypeName();
	
	const XWPSCidSystemInfo * getCIDSysInfo() {return &CIDSystemInfo;}
	
	int glyphData(ulong glyph, XWPSString *pgstr, int *pfidx);
	
	int writeCid0Font(XWPSStream *s, int options,
		    					  const uchar *subset_cids, 
		    						uint subset_size,
		    						const XWPSString *alt_font_name);
		    						
	int z9glyphData(ulong glyph, XWPSString *pgstr, int *pfidx);
	int z9glyphOutline(ulong glyph, const XWPSMatrix *pmat, XWPSPath *ppath);
	
public:
	XWPSCidSystemInfo CIDSystemInfo;
  int CIDCount;
  int GDBytes;
  ulong CIDMapOffset;
  XWPSFontType1 **FDArray;
  uint FDArray_size;
  int FDBytes;
  
  union
  {
  	int (XWPSFontBase::*glyph_data)(ulong, XWPSString *, int *);
  	int (XWPSFontCid0::*glyph_datac0)(ulong, XWPSString *, int *);
  };
  void *proc_data;
};

class XWPSFontCid1 : public XWPSFontBase
{
public:
	XWPSFontCid1();
	~XWPSFontCid1();
	
	virtual XWPSFont * copyFont();
	        void copyFontCid1(XWPSFontCid1 * newfont);
	
	int getLength();
	const char * getTypeName();
	const XWPSCidSystemInfo * getCIDSysInfo() {return &CIDSystemInfo;}
	
public:
	XWPSCidSystemInfo CIDSystemInfo;
};

#define cg_argsAreWords 1
#define cg_argsAreXYValues 2
#define cg_roundXYToGrid 4
#define cg_haveScale 8
#define cg_moreComponents 32
#define cg_haveXYScale 64
#define cg_have2x2 128
#define cg_useMyMetrics 512

#define gf_OnCurve 1
#define gf_xShort 2
#define gf_yShort 4
#define gf_Repeat 8
#define gf_xPos 16	
#define gf_xSame 16	
#define gf_yPos 32	
#define gf_ySame 32	

class XWPSFontType42 : public XWPSFontBase
{
public:
	XWPSFontType42();
	virtual ~XWPSFontType42();
	
	int appendComponent(uint glyph_index, 
	                     const XWPSMatrixFixed * pmat,
		                   XWPSPath * ppath, 
		                   XWPSFixedPoint *ppts, 
		                   int point_index);
	int appendOutline(uint glyph_index, 
	                  const XWPSMatrixFixed * pmat,
	                  XWPSPath * ppath);
	int appendSimple(const uchar *gdata, 
	                  float sbw[4], 
	                  const XWPSMatrixFixed *pmat,
	                  XWPSPath *ppath, 
	                  XWPSFixedPoint *ppts);
	
	int checkComponent(uint glyph_index, 
	                   const XWPSMatrixFixed *pmat,
		                 XWPSPath *ppath, 
		                 XWPSFixedPoint *ppts,
										 const uchar **pgdata);
	virtual XWPSFont * copyFont();
	        void copyFontType42(XWPSFontType42 * newfont);
		
	int defaultGetMetrics(uint glyph_index, int wmode, float sbw[4]);
	int defaultGetOutline(uint glyph_index, XWPSString * pglyph);
	
	virtual int getLength();
	virtual const char * getTypeName();
	virtual int getMetrics(uint glyph_index, int wmode, float sbw[4]);
	virtual int getOutline(uint glyph_index, XWPSString *pgstr);
	
	void parseComponent(const uchar **pdata, 
	                  uint *pflags, 
	                  XWPSMatrixFixed *psmat,
										int *pmp , 
										const XWPSMatrixFixed *pmat);
	int parsePieces(ulong glyph, ulong *pieces, int *pnum_pieces);
	
	int simpleGlyphMetrics(uint glyph_index, int wmode, float sbw[4]);
	int stringProc(ulong offset, uint length,	const uchar ** pdata);
	
	int totalPoints(uint glyph_index);
	int type42Append(uint glyph_index, 
	                 XWPSImagerState * pis,
		 							 XWPSPath * ppath, 
		 							 const XWPSLog2ScalePoint * pscale,
		 							 bool charpath_flag, 
		 							 int paint_type);
	int type42EnumerateGlyph(int *pindex, 
	                         PSGlyphSpace glyph_space, 
	                         ulong *pglyph);
	int type42FontInit();
	int type42GetMetrics(uint glyph_index, float sbw[4]);
	int type42GlyphInfo(ulong glyph, 
	                    const XWPSMatrix *pmat,
		                  int members, 
		                  XWPSGlyphInfo *info);
	int type42GlyphOutline(ulong glyph, const XWPSMatrix *pmat,	XWPSPath *ppath);
	int type42WModeMetrics(uint glyph_index, int wmode,	float sbw[4]);
	
	int writeRange(XWPSStream *s, ulong start, uint length);
	int writeTrueTypeData(XWPSStream *s, 
	                      int options,
												XWPSPSFGlyphEnum *penum, 
												bool is_subset,
			                  const XWPSString *alt_font_name);
	int writeTrueTypeFont(XWPSStream *s, 
	                     int options,
			                 ulong *orig_subset_glyphs, 
			                 uint orig_subset_size,
											 const XWPSString *alt_font_name);
	ulong z42encodeChar(ulong chr, PSGlyphSpace glyph_space);
	int z42enumerateGlyph(int *pindex, 
	                      PSGlyphSpace glyph_space,
		                    ulong *pglyph);
	int z42gdirEnumerateGlyph(int *pindex,
			                      PSGlyphSpace glyph_space, 
			                      ulong *pglyph);
	int z42gdirGetOutline(uint glyph_index, XWPSString * pgstr);
	int z42glyphInfo(ulong glyph, 
	                 const XWPSMatrix *pmat,
	                 int members, 
	                 XWPSGlyphInfo *info);
	int z42glyphOutline(ulong glyph, const XWPSMatrix *pmat, XWPSPath *ppath);
	int z42stringProc(ulong offset, uint length,const uchar ** pdata);
	
public:
	union
	{
		int (XWPSFontType42::*string_proc)(ulong, uint, const uchar **);
		int (XWPSFontCid2::*string_procc2)(ulong, uint, const uchar **);
	};
	
	void *proc_data;
	
	union
	{
		int (XWPSFontType42::*get_outline)(uint ,  XWPSString *);
		int (XWPSFontCid2::*get_outlinec2)(uint ,  XWPSString *);
	};
	
	union
	{
  	int (XWPSFontType42::*get_metrics)(uint , int , float sbw[4]);
  	int (XWPSFontCid2::*get_metricsc2)(uint , int , float sbw[4]);
  };
	ulong glyf;
	uint unitsPerEm;
	uint indexToLocFormat;
	PSType42Mtx metrics[2];
	ulong loca;
	uint numGlyphs;
};

#define MAX_COMPOSITE_PIECES 3

class XWPSFontCid2 : public XWPSFontType42
{
public:
	XWPSFontCid2();
	~XWPSFontCid2();
	
	virtual XWPSFont * copyFont();
	        void copyFontCid2(XWPSFontCid2 * newfont);
	        
	int getLength();
	const char * getTypeName();
	const XWPSCidSystemInfo * getCIDSysInfo() {return &CIDSystemInfo;}
	int getMetrics(uint glyph_index, int wmode, float sbw[4]);
	int getOutline(uint glyph_index, XWPSString * pgstr);
	
	int writeCid2Font(XWPSStream *s, 
	                  int options,
		                const uchar *subset_bits, 
		                uint subset_size,
		    						const XWPSString *alt_font_name);
		    						
	int z11CIDMapProc(ulong glyph);
	int z11getMetrics(uint glyph_index, int wmode,	float sbw[4]);
	int z11getOutline(uint glyph_index,	XWPSString * pgstr);
	
public:
	XWPSCidSystemInfo CIDSystemInfo;
  int CIDCount;
  int GDBytes;
  int MetricsCount;
  
  int (XWPSFontCid2::*CIDMap_proc)(ulong);
  struct o_ 
  {
  	union
  	{
			int (XWPSFontType42::*get_outline)(uint, XWPSString *);
			int (XWPSFontCid2::*get_outlinec2)(uint, XWPSString *);
		};
		
		union
		{
			int (XWPSFontType42::*get_metrics)(uint, int, float [4]);
			int (XWPSFontCid2::*get_metricsc2)(uint, int, float [4]);
		};
  } orig_procs;
};

#define fmap_type_min 2
#define fmap_type_max 9
#define fmap_type_is_modal(fmt)\
  ((fmt) == fmap_escape || (fmt) == fmap_double_escape || (fmt) == fmap_shift)

class XWPSFontType0 : public XWPSFont
{
public:
	XWPSFontType0();
	~XWPSFontType0();
	
	virtual XWPSFont * copyFont();
	        void copyFontType0(XWPSFontType0 * newfont);
	
	int defineFontType0(XWPSFontDir * pdir);
	
	int getLength();
	const char * getTypeName();
	
	int makeFontType0(XWPSFontDir * pdir, 
		   							const XWPSMatrix * pmat, 
		   							XWPSFont ** ppfont);
	
	int type0AdjustMatrix(XWPSFontDir * pdir, const XWPSMatrix * pmat);
	int type0InitFStack(XWPSTextEnum *pte);
	
	int ztype0AdjustFDepVector();
	int ztype0DefineFont(XWPSFontDir * pdir);
	int ztype0MakeFont(XWPSFontDir * pdir, 
		 								 const XWPSMatrix * pmat, 
		 								 XWPSFont ** ppfont);
	
public:
	PSFMapType FMapType;
  uchar EscChar, ShiftIn, ShiftOut;
  XWPSString SubsVector;
  uint subs_size;
  uint subs_width;
  uint *Encoding;
  uint encoding_size;
  XWPSFont **FDepVector;
  uint fdep_size;
  XWPSCMap *CMap;
};

#endif //XWPSFONT_H
