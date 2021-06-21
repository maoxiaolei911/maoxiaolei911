/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWSFNTTYPE_H
#define XWSFNTTYPE_H

#include "XWFontFileSFNT.h"


#define TT_HEAD_TABLE_SIZE 54UL
#define TT_MAXP_TABLE_SIZE 32UL
#define TT_HHEA_TABLE_SIZE 36UL
#define TT_VHEA_TABLE_SIZE 36UL

#define SFNT_TABLE_REQUIRED (1 << 0)

class XWCMap;

class XW_FONT_EXPORT XWSFNTTableDirectory
{
public:
	XWSFNTTableDirectory();
	XWSFNTTableDirectory(XWFontFileSFNT * fileA, ulong offsetA);
	~XWSFNTTableDirectory();
	
	ulong calcCheckSum(void *data, ulong length);
	
	static void convertTag(char *tagA, ulong u_tag)
			{
  				for (int i = 3; i >= 0; i--) 
  				{
    				tagA[i] = (char) (u_tag % 256);
    				u_tag /= 256;
  				}

  				return;
			}
			
	int   findTableIndex(const char *tagA);
	ulong findTableLen(const char *tagA);	
	ulong findTablePos(const char *tagA);
			
	static ulong makeTag(char *tagA)
		{
			return ((tagA[0] & 0xff) << 24) |
         			((tagA[1] & 0xff) << 16) |
         			((tagA[2] & 0xff) << 8) |
          			(tagA[3] & 0xff);
		}
		
	int requireTable(const char *tagA, int must_exist);
		
	void setTable(const char *tagA, void *data, ulong length);
	
public:
	struct SFNTTable
	{
		char   tag[4];
		ulong  checksum;
		ulong  offset;
		ulong  length;
		void * data;
	};
	
	ulong version;
	ushort   num_tables;
	ushort   search_range;
	ushort   entry_selector;
	ushort   range_shift;
	ushort   num_kept_tables;
	char *flags;
	
	SFNTTable * tables;
};

struct XW_FONT_EXPORT SFNTSubHeader
{
	ushort firstCode;
  	ushort entryCount;
  	short  idDelta;
  	ushort idRangeOffset;
};

struct XW_FONT_EXPORT SFNTCmap0
{
	uchar glyphIndexArray[256];
};

struct XW_FONT_EXPORT SFNTCmap2
{
	ushort  subHeaderKeys[256];
  	SFNTSubHeader *subHeaders;
  	ushort *glyphIndexArray;
};

struct XW_FONT_EXPORT SFNTCmap4
{
	ushort  segCountX2;
  	ushort  searchRange;
  	ushort  entrySelector;
  	ushort  rangeShift;
  	ushort *endCount;
  	ushort  reservedPad;
  	ushort *startCount;
  	ushort *idDelta;
  	ushort *idRangeOffset;
  	ushort *glyphIndexArray;
};

struct XW_FONT_EXPORT SFNTCmap6
{
	ushort  firstCode;
  	ushort  entryCount;
  	ushort *glyphIndexArray;
};


struct XW_FONT_EXPORT SFNTCharGroup
{
	ulong startCharCode;
  	ulong endCharCode;
  	ulong startGlyphID;
};

struct XW_FONT_EXPORT SFNTCmap12
{
	ulong  nGroups;
  	SFNTCharGroup *groups;
};
	
class XW_FONT_EXPORT XWSFNTCmap 
{
public:
	XWSFNTCmap();
	XWSFNTCmap(XWFontFileSFNT * fileA, ushort platformA, ushort encodingA);
	~XWSFNTCmap();
	
	void loadCMap4(uchar *GIDToCIDMap, XWCMap *cmap);
	void loadCMap12(uchar *GIDToCIDMap, XWCMap *cmap);	     
	ushort lookup(long cc);
	
public:
  	int platform;
  	int encoding;
  	int format;
  	ulong language;
  	void  *map;
};

struct XW_FONT_EXPORT TTGlyphDesc
{
	ushort gid;
  	ushort ogid;
  	ushort advw, advh;
  	short  lsb, tsb;
  	short  llx, lly, urx, ury;
  	ulong  length;
	uchar  *data;
};
	
class XW_FONT_EXPORT XWTTGlyphs
{
public:
	XWTTGlyphs();
	~XWTTGlyphs();
	
	ushort addGlyph(ushort gid, ushort new_gid);
	
	int buildTables(XWFontFileSFNT * sfont);

	ushort findGlyph(ushort gid);
	
	ushort getIndex(ushort gid);
	int    getMetrics(XWFontFileSFNT *sfont);
	
public:
  	ushort num_glyphs;
  	ushort max_glyphs;
  	ushort last_gid;
  	ushort emsize;
  	ushort dw;
  	ushort default_advh;
  	short  default_tsb;
  	TTGlyphDesc *gd;
  	uchar * used_slot;
  	
private:
	ushort findEmptySlot();
};


class XW_FONT_EXPORT XWTTPostTable
{
public:
	XWTTPostTable();
	XWTTPostTable(XWFontFileSFNT * sfont);
	~XWTTPostTable();
	
	ushort lookup(const char *glyphname);
	
public:
  	ulong    Version;
  	ulong    italicAngle;
  	short    underlinePosition;
  	short    underlineThickness;
  	ulong    isFixedPitch;
  	ulong    minMemType42;
  	ulong    maxMemType42;
  	ulong    minMemType1;
  	ulong    maxMemType1; 

  	ushort   numberOfGlyphs;

  	char   **glyphNamePtr;
  	char   **names;

  	ushort   count;
  	
private:
	int readV2Names(XWFontFileSFNT *sfont);
};

#define TT_HEAD_TABLE_SIZE 54UL
#define TT_MAXP_TABLE_SIZE 32UL
#define TT_HHEA_TABLE_SIZE 36UL
#define TT_VHEA_TABLE_SIZE 36UL

class XW_FONT_EXPORT XWTTHeadTable
{
public:
	XWTTHeadTable();
	XWTTHeadTable(XWFontFileSFNT * sfont);
	~XWTTHeadTable() {}
	
	char * pack();
	
public:
  	ulong  version;
  	ulong  fontRevision;
  	ulong  checkSumAdjustment;
  	ulong  magicNumber;
  	ushort flags;
  	ushort unitsPerEm;
  	uchar  created[8];
  	uchar  modified[8];
  	short  xMin, yMin, xMax, yMax;
  	ushort macStyle;
  	ushort lowestRecPPEM;
  	short  fontDirectionHint;
  	short  indexToLocFormat;
  	short  glyphDataFormat;
};

class XW_FONT_EXPORT XWTTMaxpTable
{
public:
	XWTTMaxpTable();
	XWTTMaxpTable(XWFontFileSFNT * sfont);
	~XWTTMaxpTable() {}
	
	char * pack();
	
public:
  	ulong  version;
  	ushort numGlyphs;
  	ushort maxPoints;
  	ushort maxContours;
  	ushort maxComponentPoints;
  	ushort maxComponentContours;
  	ushort maxZones;
  	ushort maxTwilightPoints;
  	ushort maxStorage;
  	ushort maxFunctionDefs;
  	ushort maxInstructionDefs;
  	ushort maxStackElements;
  	ushort maxSizeOfInstructions;
  	ushort maxComponentElements;
  	ushort maxComponentDepth;
};

class XW_FONT_EXPORT XWTTHHeaTable
{
public:
	XWTTHHeaTable();
	XWTTHHeaTable(XWFontFileSFNT * sfont);
	~XWTTHHeaTable() {}
	
	char * pack();
	
public:
  	ulong  version;
  	short  ascent;
  	short  descent;
  	short  lineGap;
  	ushort advanceWidthMax;
  	short  minLeftSideBearing;
  	short  minRightSideBearing;
  	short  xMaxExtent;
  	short  caretSlopeRise;
  	short  caretSlopeRun;
  	short  caretOffset;
  	short  reserved[4]; /* set to 0 */
  	short  metricDataFormat;
  	ushort numOfLongHorMetrics;
  	ushort numOfExSideBearings; /* extra information */
};

class XW_FONT_EXPORT XWTTVHeaTable
{
public:
	XWTTVHeaTable();
	XWTTVHeaTable(XWFontFileSFNT * sfont);
	~XWTTVHeaTable() {}
	
public:
  	ulong  version;
  	short  vertTypoAscender;  /* v.1.1 name */
  	short  vertTypoDescender; /* v.1.1 name */
  	short  vertTypoLineGap;   /* v.1.1 name */ 
  	short  advanceHeightMax;
  	short  minTopSideBearing;
  	short  minBottomSideBearing;
  	short  yMaxExtent;
  	short  caretSlopeRise;
  	short  caretSlopeRun;
  	short  caretOffset;
  	short  reserved[4]; /* set to 0 */
  	short  metricDataFormat;
  	ushort numOfLongVerMetrics;
  	ushort numOfExSideBearings; /* extra information */
};


class XW_FONT_EXPORT XWTTVORGTable 
{
public:
	XWTTVORGTable();
	XWTTVORGTable(XWFontFileSFNT * sfont);
	~XWTTVORGTable();
	
public:
	struct TTVertOriginYMetrics 
	{
  		ushort glyphIndex;
  		short  vertOriginY;
	};

  	short  defaultVertOriginY;
  	ushort numVertOriginYMetrics;
  	TTVertOriginYMetrics *vertOriginYMetrics;
};

/* hmtx and vmtx */
struct XW_FONT_EXPORT TTLongMetrics
{
  	ushort advance;
  	short  sideBearing;
};

class XW_FONT_EXPORT XWTTOs2Table
{
public:
	XWTTOs2Table();
	XWTTOs2Table(XWFontFileSFNT * sfont);
	
public:
  	ushort  version; /* 0x0001 or 0x0002 */
  	short   xAvgCharWidth;  
  	ushort  usWeightClass;  
  	ushort  usWidthClass;   
  	short   fsType;  /* if (faType & 0x08) editable_embedding */
  	short   ySubscriptXSize;        
  	short   ySubscriptYSize;      
  	short   ySubscriptXOffset;
  	short   ySubscriptYOffset;      
  	short   ySuperscriptXSize;      
  	short   ySuperscriptYSize;      
  	short   ySuperscriptXOffset;    
  	short   ySuperscriptYOffset;    
  	short   yStrikeoutSize; 
  	short   yStrikeoutPosition;     
  	short   sFamilyClass;   
  	uchar   panose[10];
  	ulong   ulUnicodeRange1;
  	ulong   ulUnicodeRange2;
  	ulong   ulUnicodeRange3;
  	ulong   ulUnicodeRange4;
  	char    achVendID[4];   
  	ushort  fsSelection;    
  	ushort  usFirstCharIndex;
  	ushort  usLastCharIndex;
  	short   sTypoAscender;  /* TTF spec. from MS is wrong */
  	short   sTypoDescender; /* TTF spec. from MS is wrong */
  	short   sTypoLineGap;   /* TTF spec. from MS is wrong */
  	ushort  usWinAscent;   
  	ushort  usWinDescent;    
  	ulong   ulCodePageRange1;
  	ulong   ulCodePageRange2;
  	/* version 0x0002 */
  	short   sxHeight;
  	short   sCapHeight;
  	ushort  usDefaultChar;
  	ushort  usBreakChar;
  	ushort  usMaxContext;
};

class XW_FONT_EXPORT XWOTLOpt
{
public:
	XWOTLOpt();
	~XWOTLOpt();
	
	int match(const char *tag);

	int parse(const char *optstr);
	
public:
	struct BTNode 
	{
  		int    flag;
  		BTNode *left;
  		BTNode *right;
  		char data[4];
	};
	
	BTNode * rule;
	
private:
	int matchExpr(BTNode *expr, const char *key);
	
	BTNode * newTree();
	
	BTNode * parseExpr(char **pp, char *endptr);

	void releaseTree(BTNode *tree);
};

#define OTL_GSUB_TYPE_SINGLE    1
#define OTL_GSUB_TYPE_MULTIPLE  2
#define OTL_GSUB_TYPE_ALTERNATE 3
#define OTL_GSUB_TYPE_LIGATURE  4
#define OTL_GSUB_TYPE_CONTEXT   5
#define OTL_GSUB_TYPE_CCONTEXT  6
#define OTL_GSUB_TYPE_ESUBST    7

#define GSUB_LIST_MAX 32

struct XW_FONT_EXPORT CLTRecord
{
	char   tag[5];
	ushort offset;
};
	
struct XW_FONT_EXPORT CLTRange
{
	ushort Start;
	ushort End;
	ushort StartCoverageIndex;
};
	
struct XW_FONT_EXPORT CLTRecordList
{
	ushort count;
	CLTRecord *record;
};
	
struct XW_FONT_EXPORT CLTNumberList
{
	ushort  count;
  	ushort *value;
};
	
struct XW_FONT_EXPORT CLTCoverage
{
	ushort format;
	ushort count;
	ushort *list;
	CLTRange *range;
};
	
struct XW_FONT_EXPORT OTLGsubHeader
{
	ulong  version;
	ushort ScriptList;
	ushort FeatureList;
	ushort LookupList;
};

struct XW_FONT_EXPORT OTLGsubSingle1
{
	short DeltaGlyphID;
	CLTCoverage coverage;
};
	
struct XW_FONT_EXPORT OTLGsubSingle2
{
	ushort   GlyphCount;
	ushort * Substitute;
	CLTCoverage coverage;
};

struct XW_FONT_EXPORT OTLGsubAltSet
{
	ushort   GlyphCount;
	ushort * Alternate;
};
	
struct XW_FONT_EXPORT OTLGsubAlternate1
{
	ushort   AlternateSetCount;
	OTLGsubAltSet *AlternateSet;
	CLTCoverage coverage;
};

struct XW_FONT_EXPORT OTLGsubLigTab
{
	ushort  LigGlyph;
	ushort   CompCount;
	ushort *Component;
};
	
struct XW_FONT_EXPORT OTLGsubLigSet
{
	ushort LigatureCount;
	OTLGsubLigTab *Ligature;
};

struct XW_FONT_EXPORT OTLGsubLigature1
{
	ushort LigSetCount;
	OTLGsubLigSet *LigatureSet;
	CLTCoverage    coverage;
};
	
struct XW_FONT_EXPORT OTLGsubSubTab
{
	ushort LookupType;
	ushort SubstFormat;
	union 
	{
    	OTLGsubSingle1    *single1;
    	OTLGsubSingle2    *single2;
    	OTLGsubAlternate1 *alternate1;
    	OTLGsubLigature1  *ligature1;
  	}table;
};
	
struct XW_FONT_EXPORT CLTScriptTable
{
	ushort DefaultLangSys;
  	CLTRecordList LangSysRecord;
};
	
struct XW_FONT_EXPORT CLTLangSysTable
{
	ushort LookupOrder;
	ushort ReqFeatureIndex;
	CLTNumberList FeatureIndex;
};
	
struct XW_FONT_EXPORT CLTFeatureTable
{
	ushort FeatureParams;
	CLTNumberList LookupListIndex;
};

struct XW_FONT_EXPORT CLTLookupTable
{
	ushort LookupType;
	ushort LookupFlag;
  	CLTNumberList SubTableList;
};
	
struct XW_FONT_EXPORT OTLGsubTab
{
	char *script;
	char *language;
  	char *feature;
	int    num_subtables;
	OTLGsubSubTab *subtables;
};
	
class XW_FONT_EXPORT XWOTLGsub
{
public:
	XWOTLGsub();
	~XWOTLGsub();
	
	int addFeat(const char *script,
                const char *language,
                const char *feature,
                XWFontFileSFNT *sfont);
	int apply(ushort *gid);
	int applyAlt(ushort alt_idx, ushort *gid);
	int applyLig(ushort *gid_in, 
	             ushort num_gids, 
	             ushort *gid_out);
	             
	int select(const char *script,
               const char *language,
               const char *feature);
	
private:
	int applyOTLGsubAlternate(OTLGsubSubTab *subtab, ushort alt_idx, ushort *gid);
	int applyOTLGsubLigature(OTLGsubSubTab *subtab,
                         	 ushort *gid_in,  
                         	 ushort num_gids,
                         	 ushort *gid_out);
	int applyOTLGsubSingle(OTLGsubSubTab *subtab, ushort *gid);
	
	int glyphSeqCmp(ushort *glyph_seq0, 
	                ushort n_glyphs0,
                    ushort *glyph_seq1, 
                    ushort n_glyphs1);

	long lookupCLTCoverage(CLTCoverage *cov, ushort gid);

	long readCLTCoverage(CLTCoverage *cov, XWFontFileSFNT *sfont);
	long readCLTFeatureTable(CLTFeatureTable *tab, XWFontFileSFNT *sfont);
	long readCLTLangSysTable(CLTLangSysTable *tab, XWFontFileSFNT *sfont);
	long readCLTLookupTable(CLTLookupTable *tab, XWFontFileSFNT *sfont);
	long readCLTNumberList(CLTNumberList *list, XWFontFileSFNT *sfont);
	long readCLTRange(CLTRange *rec, XWFontFileSFNT *sfont);
	long readCLTRecord(CLTRecord *rec, XWFontFileSFNT *sfont);
	long readCLTRecordList(CLTRecordList *list, XWFontFileSFNT *sfont);	
	long readCLTScriptTable(CLTScriptTable *tab, XWFontFileSFNT *sfont);
	long readOTLGsubAlternate(OTLGsubSubTab *subtab, XWFontFileSFNT *sfont);
	int  readOTLGsubFeat(OTLGsubTab *gsub, XWFontFileSFNT *sfont);
	long readOTLGsubHeader(OTLGsubHeader *head, XWFontFileSFNT *sfont);
	long readOTLGsubLigature(OTLGsubSubTab *subtab, XWFontFileSFNT *sfont);
	long readOTLGsubSingle(OTLGsubSubTab *subtab, XWFontFileSFNT *sfont);
	void releaseCLTCoverage(CLTCoverage *cov);
	void releaseCLTFeatureTable(CLTFeatureTable *tab);
	void releaseCLTLangSysTable(CLTLangSysTable *tab);
	void releaseCLTLookupTable(CLTLookupTable *tab);
	void releaseCLTNumberList(CLTNumberList *list);
	void releaseCLTRecordList(CLTRecordList *list);
	void releaseCLTScriptTable(CLTScriptTable *tab);
	void releaseOTLGsubAlternate(OTLGsubSubTab *subtab);
	void releaseOTLGsubLigature(OTLGsubSubTab *subtab);
	void releaseOTLGsubSingle(OTLGsubSubTab *subtab);
	
private:
	int num_gsubs;
  	int selected;
  	OTLGsubTab gsubs[GSUB_LIST_MAX];
};

#endif //XWSFNTTYPE_H