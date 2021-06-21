/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTFILECFF_H
#define XWFONTFILECFF_H

#include "XWFontFile.h"

#define CS_NUM_SUBR_MAX    65536
#define CS_STR_LEN_MAX     65536
#define CS_STEM_ZONE_MAX   96
#define CS_ARG_STACK_MAX   48
#define CS_TRANS_ARRAY_MAX 32
#define CS_SUBR_NEST_MAX   10


#define FONTTYPE_CIDFONT  (1 << 0)
#define FONTTYPE_FONT     (1 << 1)
#define FONTTYPE_MMASTER  (1 << 2)

#define ENCODING_STANDARD (1 << 3)
#define ENCODING_EXPERT   (1 << 4)

#define CHARSETS_ISOADOBE (1 << 5)
#define CHARSETS_EXPERT   (1 << 6)
#define CHARSETS_EXPSUB   (1 << 7)

#define HAVE_STANDARD_ENCODING (ENCODING_STANDARD|ENCODING_EXPERT)
#define HAVE_STANDARD_CHARSETS \
  (CHARSETS_ISOADOBE|CHARSETS_EXPERT|CHARSETS_EXPSUB)

#define CFF_STRING_NOTDEF 65535

class XWFontFileType1;
class XWPFBParser;
class XWCFFIndex;
class XWCFFHeader;
class XWCFFDict;
class XWCFFEncoding;
class XWCFFCharsets;
class XWCFFFDSelect;
class XWCSGInfo;

class XW_FONT_EXPORT XWFontFileCFF : public XWFontFile
{
	Q_OBJECT
	
public:	
	friend class XWFontFileSFNT;
	
	virtual ~XWFontFileCFF();
	
	ushort addString(const char *str, int unique);
	
	void createFDArray(int c);
	
	long   cstringSize();
	
	void fdarrayAdd(int i, const char *key, int countA);
	long fdarrayPack(int i, uchar *dest, long destlen);
	void fdarrayRemove(int i, const char * key);
	void fdarraySet(int i, const char *key, int idx, double value);
	
	XWCFFCharsets * getCharsets() {return charsets;}
	XWCFFIndex * getCStrings() {return cstrings;}
	XWCFFEncoding * getEncoding() {return encoding;}
	XWCFFFDSelect * getFDSelect() {return fdselect;}
	ushort     * getFtToGid() {return ft_to_gid;}
	XWCFFIndex * getGsubr() {return gsubr;}
	XWCFFIndex * getIndexHeader();
	char * getName();
	uchar  getNumFDs() {return num_fds;}
	char * getOrdering();
	char * getRegistry();
	long   getSID(char *str);
	char * getString(ushort id);
	XWCFFIndex * getStrings() {return string;}
	XWCFFIndex * getSubrs(int i) {return subrs[i];}
	int    getSupplement();
	long   gsubrSize();
	
	bool isCIDFont() {return flag & FONTTYPE_CIDFONT;}
	
	static XWFontFileCFF * loadType1(char *fileName, 
	                                 char **enc_vec, 
	                                 int mode);
	
	ushort lookupChartsets(ushort cid);
	ushort lookupChartsetsInverse(ushort gid);
	ushort lookupEncoding(ushort code);
	uchar  lookupFDSelect(ushort gid);
	ushort lookupGlyph(const char *glyph);
	
	static XWFontFileCFF * make(QIODevice * fileA, 
	                            int fileFinalA,
	                            ulong offsetA,
		 	                    ulong lenA,
		 	                    int  indexA);	
	int matchString(const char *str, ushort sid);
		
	long nameSize();
	void newString();
	
	long packCharsets(uchar *dest, long destlen);
	long packCString(uchar *dest, long destlen);
	long packEncoding(uchar *dest, long destlen);
	long packFDSelect(uchar *dest, long destlen);
	long packGsubr(uchar *dest, long destlen);
	long packName(uchar *dest, long destlen);
	long packString(uchar *dest, long destlen);
	void   privDictAdd(int i, const char *key, int countA);
	double privDictGet(int i, const char * key, int idx);
	bool privDictKnow(int i, const char * key);
	long privDictPack(int i, uchar *dest, long destlen);
	void privDictRemove(int i, const char * key);
	void privDictSet(int i, const char *key, int idx, double value);
	void privDictUpdate(int i);
	long putHeader(uchar *dest, long destlen);
	
	long readCharsets();
	long readEncoding();
	long readFDArray();
	long readFDSelect();
	long readPrivate();
	long readSubrs();
	
	void setCharsets(XWCFFCharsets * cs);
	void setCString(XWCFFIndex * idx);
	void setEncoding(XWCFFEncoding * enc);
	void setFDSelect(XWCFFFDSelect * fd);
	void setFlag(int t) {flag = t;}
	void setGsub(XWCFFIndex * g);
	long setName(char *nameA);
	void setNumGlyphs(int num_glyphs) {nGlyphs = num_glyphs;}
	void setSubrs(int i, XWCFFIndex * idx);
	long stringSize();
	
	void   topDictAdd(const char *key, int countA);
	double topDictGet(const char *key, int idx);
	bool   topDictKnow(const char *key);
	long   topDictPack(uchar *dest, long destlen);
	void   topDictRemove(const char *key);
	void   topDictSet(const char *key, int idx, double value);
	void   topDictUpdate();
	
	void updateString();
	
protected:
	XWFontFileCFF();
	XWFontFileCFF(QIODevice * fileA,
		 	      int    fileFinalA,
		 	      ulong offsetA,
		 	      ulong lenA,
		 	      int  indexA = 0);
	
	static int parseCharStrings(XWFontFileType1 * filet1,
	                            XWFontFileCFF * font, 
	                            XWPFBParser * parser, 
	                            int lenIV, 
	                            int mode);
	static int parsePart1(XWFontFileCFF * font,
	                      uchar * startA,
	                      uchar * endA,
	                      char **enc_vec);
	static int parsePart2(XWFontFileType1 * filet1,
	                      XWFontFileCFF * font,
	                      uchar * startA,
	                      uchar * endA,
	                      int mode);
	static int parseSubrs(XWFontFileType1 * filet1,
	                      XWFontFileCFF * font, 
	                      XWPFBParser * parser, 
	                      int lenIV, 
	                      int mode);
		 
protected:
	char         *fontname;
	
	XWCFFHeader   *  header;
	XWCFFIndex    *  name;
	XWCFFDict     *  topDict;
	XWCFFIndex    *  string;
	XWCFFIndex    *  gsubr;
	XWCFFEncoding *  encoding;
	XWCFFCharsets *  charsets;
	XWCFFFDSelect *  fdselect;
	XWCFFIndex    *  cstrings;
	XWCFFDict     ** fdarray;
	XWCFFDict     ** privateDict;
	XWCFFIndex    ** subrs;
	
	ulong  gsubr_offset;
	uchar  num_fds;
	
	XWCFFIndex  *_string;
	
	int filter;
	int index;
	int flag;
	
	ushort * ft_to_gid;
};

#endif //XWFONTFILECFF_H