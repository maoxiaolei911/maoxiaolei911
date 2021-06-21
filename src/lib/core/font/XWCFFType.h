/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWCFFTYPE_H
#define XWCFFTYPE_H

#include "XWFontFileCFF.h"


#define CFF_INT_MAX 0x7fffffff
#define CFF_INT_MIN (-0x7fffffff - 1)

#define CFF_SID_MAX    64999
#define CFF_STDSTR_MAX 391

#define CFF_GLYPH_MAX  CFF_SID_MAX

#define CFF_NOMINALWIDTHX_DEFAULT 0.0
#define CFF_DEFAULTWIDTHX_DEFAULT 0.0

#define CFF_TYPE_UNKNOWN 0
#define CFF_TYPE_INTEGER (1 << 0)
#define CFF_TYPE_REAL    (1 << 1)
#define CFF_TYPE_NUMBER  (CFF_TYPE_INTEGER|CFF_TYPE_REAL)
#define CFF_TYPE_BOOLEAN (1 << 2)
#define CFF_TYPE_SID     (1 << 3)
#define CFF_TYPE_ARRAY   (1 << 4)
#define CFF_TYPE_DELTA   (1 << 5)

#define CFF_TYPE_ROS     (1 << 6)
/* offset(0) */
#define CFF_TYPE_OFFSET  (1 << 7)
/* size offset(0) */
#define CFF_TYPE_SZOFF   (1 << 8)

class XW_FONT_EXPORT XWCFFIndex
{
public:
	XWCFFIndex();
	XWCFFIndex(ushort countA);
	XWCFFIndex(XWFontFileCFF * fileA, bool onlyheader = false);
	~XWCFFIndex();
	
	ushort addString(const char *str, int unique);
	
	char * getName(int indexA);
	long   getSID(char *str);
	char * getString(ushort id);
	
	int matchString(const char *str, ushort sid);
	
	long pack(uchar *dest, long destlen);
	
	long setName(char * nameA);
	
	long size();
	
public:
	ushort  count; //len
  	uchar   offsize;
  	ulong  *offset;
  	uchar  *data;
};

class XW_FONT_EXPORT XWCFFHeader
{
public:
	XWCFFHeader();
	XWCFFHeader(XWFontFileCFF * fileA);
	~XWCFFHeader() {}
	
	long put(uchar *dest);
	
public:
	uchar  major;
  	uchar  minor;
  	uchar  hdr_size;
  	uchar  offsize;
};

class XW_FONT_EXPORT XWCFFDict
{
public:
	XWCFFDict();
	XWCFFDict(uchar *dataA, uchar *endptr);
	~XWCFFDict();
	
	void add(const char *key, int countA);
	
	double get(const char *key, int idx);
	
	bool isOK() {return ok;}
	
	int known(const char *key);
	
	long pack(uchar *dest, long destlen);
	
	void remove(const char *key);
	
	void set(const char *key, int idx, double value);
	
	void update(XWFontFileCFF * fileA);
	
public:
	int    max;
  	int    count;
  	
	struct CFFDictEntry
	{
  		int     id;
  		char   *key;
  		int     count;
  		double *values;
	};
	
	CFFDictEntry * entries;
	
	bool ok;
	
private:
	void add(uchar **data, 
	         uchar *endptr, 
	         int *status);
		      
	double getInteger(uchar **dataA, 
	                  uchar *endptr, 
	                  int *status);
	double getReal(uchar **dataA, 
	               uchar *endptr, 
	               int *status);
	               
	long packInteger(uchar *dest, 
	                 long destlen, 
	                 long value);	                 
	long packReal(uchar *dest, 
	              long destlen, 
	              double value);
	long putEntry(CFFDictEntry *de,
				  uchar *dest, 
				  long destlen);
	long putNumber(double value,
				   uchar *dest, 
				   long destlen,
				   int type);
};

struct XW_FONT_EXPORT CFFRange1
{
  	ushort first;
  	uchar  n_left;
};

struct XW_FONT_EXPORT CFFRange2
{
  	ushort first;
  	ushort n_left;
};

struct XW_FONT_EXPORT CFFRange3
{
	ushort first;
 	uchar  fd;
};
	
struct XW_FONT_EXPORT CFFMap
{
	ushort code;
  	ushort glyph;
};
	
class XW_FONT_EXPORT XWCFFEncoding
{
public:
	XWCFFEncoding();
	~XWCFFEncoding();
	
	ushort lookup(XWFontFileCFF * fileA, uchar code);
	
	long pack(uchar *dest, long destlen);
	
	long read(XWFontFileCFF * fileA);
	
public:
	uchar format;
  	uchar num_entries;
  	union 
  	{
    	uchar *codes;
    	CFFRange1 *range1;
  	} data;
  	uchar num_supps;
  	CFFMap *supp;
};

class XW_FONT_EXPORT XWCFFCharsets
{
public:
	XWCFFCharsets();
	~XWCFFCharsets();
	
	ushort lookup(ushort cid);
	ushort lookup(XWFontFileCFF * fileA, const char *glyph);
	ushort lookupInverse(ushort gid);
	
	long pack(uchar *dest, long destlen);
	
	long read(XWFontFileCFF * fileA);
	
public:
	uchar format;
  	ushort num_entries;
  	union 
  	{
    	ushort *glyphs;
    	CFFRange1 *range1;
    	CFFRange2 *range2;
  	} data;
};

class XW_FONT_EXPORT XWCFFFDSelect
{
public:
	XWCFFFDSelect();
	~XWCFFFDSelect();
	
	uchar lookup(ushort gid);
	
	long pack(XWFontFileCFF * fileA, uchar *dest, long destlen);
	
	long read(XWFontFileCFF * fileA);
	
public:
	uchar format;
  	ushort num_entries;
  	union 
  	{
    	uchar      *fds;
    	CFFRange3 *ranges;
  	} data;
};

class XW_FONT_EXPORT XWCSGInfo
{
public:
	XWCSGInfo();
	
	long copyCharString (uchar *dst, 
	                     long dstlen,
		    			 uchar *src, 
		    			 long srclen,
		                 XWCFFIndex *gsubr, 
		                 XWCFFIndex *subr,
		    			 double default_width, 
		    			 double nominal_width);
	
public:
	int flags;
	double wx, wy;
	struct 
	{
    	double llx, lly, urx, ury;
  	} bbox;
  	
  	struct 
  	{
    	double asb, adx, ady;
    	uchar  bchar, achar;
  	} seac;
  	
private:
	void clearStack(uchar **dest, uchar *limit);
	
	void doCharString(uchar **dest, 
	                  uchar *limit,
	                  uchar **data, 
	                  uchar *endptr,
	                  XWCFFIndex *gsubr_idx, 
	                  XWCFFIndex *subr_idx);
	void doOperator1(uchar **dest, 
	                 uchar *limit, 
	                 uchar **data, 
	                 uchar *endptr);
	void doOperator2(uchar **dest, 
	                 uchar *limit, 
	                 uchar **data, 
	                 uchar *endptr);
	                 
	void getFixed(uchar **data, uchar *endptr);
	void getInteger(uchar **data, uchar *endptr);
	void getSubr(uchar **subr, 
	             long *len, 
	             XWCFFIndex *subr_idx, 
	             long id);

	void init();
};

#endif //XWCFFTYPE_H