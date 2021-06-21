/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWUNICODEMAP_H
#define XWUNICODEMAP_H

#include <QMutex>
#include "XWGlobal.h"

class XWString;

enum UnicodeMapKind 
{
    unicodeMapUser,		// read from a file
    unicodeMapResident,		// static list of ranges
    unicodeMapFunc		// function pointer
};

typedef int (*UnicodeMapFunc)(uint u, char *buf, int bufSize);

struct UnicodeMapRange 
{
    uint start, end;		// range of Unicode chars
    uint code, nBytes;		// first output code
};

struct UnicodeMapExt;


class XW_FONT_EXPORT XWUnicodeMap 
{
public:
    XWUnicodeMap(char *encodingNameA, 
                 bool unicodeOutA,
	             UnicodeMapRange *rangesA, 
	             int lenA);
	XWUnicodeMap(char *encodingNameA, 
               bool unicodeOutA,
	           UnicodeMapFunc funcA);
	
	~XWUnicodeMap();
	
	void decRefCnt();
	
	XWString *getEncodingName() { return encodingName; }
	
	void incRefCnt();
	bool isUnicode() { return unicodeOut; }
	
	int  mapUnicode(uint u, char *buf, int bufSize);
	bool match(XWString *encodingNameA);
	
    static XWUnicodeMap *parse(XWString *encodingNameA);

private:
    XWUnicodeMap(XWString *encodingNameA);

private:
    XWString *encodingName;
    UnicodeMapKind kind;
    bool unicodeOut;
    union 
    {
        UnicodeMapRange *ranges;	// (user, resident)
        UnicodeMapFunc func;	// (func)
    };
    int len;			// (user, resident)
    UnicodeMapExt *eMaps;		// (user)
    int eMapsLen;			// (user)
    int refCnt;
    QMutex mutex;
};

#define unicodeMapCacheSize 4

class XWUnicodeMapCache 
{
public:
    XWUnicodeMapCache();
    ~XWUnicodeMapCache();

    XWUnicodeMap *getUnicodeMap(XWString *encodingName);

private:
    XWUnicodeMap *cache[unicodeMapCacheSize];
};

#endif //XWUNICODEMAP_H

