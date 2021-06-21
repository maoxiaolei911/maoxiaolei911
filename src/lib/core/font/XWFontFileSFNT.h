/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTFILESFNT_H
#define XWFONTFILESFNT_H

#include "XWFontFile.h"

#define TT_MAC 1u
#define TT_WIN 3u

#define TT_WIN_SYMBOL  0u
#define TT_WIN_UNICODE 1u
#define TT_WIN_SJIS    2u
#define TT_WIN_RPC     3u
#define TT_WIN_BIG5    4u
#define TT_WIN_WANSUNG 5u
#define TT_WIN_JOHAB   6u
#define TT_WIN_UCS4    10u

#define TT_MAC_ROMAN               0u
#define TT_MAC_JAPANESE            1u
#define TT_MAC_TRADITIONAL_CHINESE 2u
#define TT_MAC_KOREAN              3u
#define TT_MAC_SIMPLIFIED_CHINESE  25u

#define SFNT_TRUETYPE   0x00010000UL
#define SFNT_MAC_TRUE	  0x74727565UL
#define SFNT_OPENTYPE   0x00010000UL
#define SFNT_POSTSCRIPT 0x4f54544fUL
#define SFNT_TTC        0x74746366UL

#define SFNT_TYPE_TRUETYPE   (1 << 0)
#define SFNT_TYPE_OPENTYPE   (1 << 1)
#define SFNT_TYPE_POSTSCRIPT (1 << 2)
#define SFNT_TYPE_TTC        (1 << 4)
#define SFNT_TYPE_DFONT      (1 << 8)

#define fixed(a) ((double)((a)%0x10000L)/(double)(0x10000L) + \
 (a)/0x10000L - (((a)/0x10000L > 0x7fffL) ? 0x10000L : 0))

class XWFontFileCFF;

class XWSFNTTableDirectory;
class XWSFNTCmap;
class XWTTPostTable;
class XWTTHeadTable;
class XWTTMaxpTable;
class XWTTHHeaTable;
class XWTTVHeaTable;
class XWTTVORGTable;
struct TTLongMetrics;
class XWTTOs2Table;

class XW_FONT_EXPORT XWFontFileSFNT : public XWFontFile
{
	Q_OBJECT
	
public:
	virtual ~XWFontFileSFNT();
	
	bool createFontFileStream(uchar ** streamdata, 
	                          long * streamlen);
	
	int   findTableIndex(const char *tagA);
	ulong findTableLen(const char *tagA);
	ulong findTablePos(const char *tagA);
		
	bool   getTTFontDesc(int *embed, 
	                     int stemv, 
	                     int typeA,
	                     double * Ascent,
	                     double * Descent,
	                     double * StemV,
	                     double * CapHeight,
	                     double * XHeight,
	                     double * AvgWidth,
	                     double * FontBBox,
	                     double * ItalicAngle,
	                     int    * Flags,
	                     uchar  * panose);
	                     
	ushort getTTName(char *dest, 
	                 ushort destlen,
	                 ushort plat_id, 
	                 ushort enco_id,
	     			 ushort lang_id, 
	     			 ushort name_id);
	ushort getTTPSFontName(char *dest, ushort destlen);
	int getType() {return type;}
	
	static bool isDFont(QIODevice * fp);
	
	static XWFontFileSFNT  * load(char *fileName, 
	                              int indexA, 
	                              bool deleteFileA = false);
	static XWFontFileSFNT  * load(FT_Face face,
	                              int accept_types, 
	                              bool deleteFileA = false);
	static XWFontFileSFNT  * loadCIDType0(char *fileName, 
	                                     bool deleteFileA = false);	
	static XWFontFileSFNT  * loadCIDType2(char *fileName, 
	                                     int indexA, 
	                                     bool deleteFileA = false);
	static XWFontFileSFNT * loadTrueType(char *fileName, 
	                                     int indexA, 
	                                     bool deleteFileA = false);
	static XWFontFileSFNT  * loadType1C(char *fileName, 
	                                    bool deleteFileA = false);
	
	ulong  locateTable(const char *tagA);
	
	XWFontFileCFF * makeCIDType0(int indexA);
	XWFontFileCFF * makeType1C(int indexA);
	
	XWSFNTCmap    * readCmap(ushort platform, ushort encoding);
	XWTTHeadTable * readHeadTable();
	XWTTHHeaTable * readHHeaTable();
	TTLongMetrics * readLongMetrics(ushort numGlyphs, 
	                                ushort numLongMetrics, 
	                                ushort numExSideBearings);
	XWTTMaxpTable * readMaxpTable();
	XWTTOs2Table  * readOs2Table();
	XWTTPostTable * readPostTable();
	ulong            readTTCOffset(int ttc_idx);
	XWTTVHeaTable * readVHeaTable();
	XWTTVORGTable * readVORGTable();
	int requireTable(const char *tagA, int must_exist);
	
	virtual bool seek(ulong offsetA);
	void setTable(const char *tagA, void *data, ulong length);
	
protected:
	XWFontFileSFNT(QIODevice * fileA,
		 	       int    fileFinalA,
		 	       ulong offsetA,
		 	       ulong lenA);
	XWFontFileSFNT(QIODevice * fileA,
		 	       int    fileFinalA,
		 	       ulong offsetA,
		 	       ulong lenA,
		 	       int  indexA);
	XWFontFileSFNT(int fileFinalA);
		 	       
	unsigned log2floor(unsigned n);
	
	unsigned max2floor(unsigned n);
			
	int   readTableDirectory(ulong offsetA);
		
protected:
	int    type;
	XWSFNTTableDirectory * directory;	
};

#endif //XWFONTFILESFNT_H