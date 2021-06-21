/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWCHARCODETOUNICODE_H
#define XWCHARCODETOUNICODE_H

#include <QMutex>
#include <QIODevice>
#include "XWGlobal.h"

class XWString;

struct CharCodeToUnicodeString;

class XW_FONT_EXPORT XWCharCodeToUnicode
{
public:
    ~XWCharCodeToUnicode();
    
    void decRefCnt();
    
    unsigned int getLength() { return mapLen; }
    
    void incRefCnt();
    
    int    mapToUnicode(unsigned int c, unsigned int *u, int size);
    bool   match(XWString *tagA);
    static XWCharCodeToUnicode *makeIdentityMapping();
    static XWCharCodeToUnicode *make8BitToUnicode(unsigned int *toUnicode);
    void   mergeCMap(XWString *buf, int nBits);
    
    static XWCharCodeToUnicode *parseCIDToUnicode(XWString *fileName,
					                              XWString *collection);
    static XWCharCodeToUnicode *parseCMap(XWString *buf, int nBits);
    static XWCharCodeToUnicode *parseUnicodeToUnicode(XWString *fileName);
    
    void setMapping(unsigned int c, unsigned int *u, int len);
    
private:
	  XWCharCodeToUnicode();
    XWCharCodeToUnicode(XWString *tagA);
    XWCharCodeToUnicode(XWString *tagA, unsigned int *mapA,
		                unsigned int mapLenA, bool copyMap,
		                CharCodeToUnicodeString *sMapA,
		                int sMapLenA, int sMapSizeA);
		                
    void addMapping(unsigned int code, char *uStr, int n, int offset);
    
    void parseCMap1(int (*getCharFunc)(void *), void *data, int nBits);
    
private:
    XWString *tag;
    unsigned int *map;
    unsigned int mapLen;
    CharCodeToUnicodeString *sMap;
    int sMapLen, sMapSize;
    int refCnt;
    
    QMutex mutex;
};


#define cMapCacheSize 4

class XW_FONT_EXPORT XWCharCodeToUnicodeCache
{
public:
    XWCharCodeToUnicodeCache(int sizeA);
    ~XWCharCodeToUnicodeCache();
    
    void add(XWCharCodeToUnicode *ctu);
    
    XWCharCodeToUnicode *getCharCodeToUnicode(XWString *tag);
    
private:
    XWCharCodeToUnicode **cache;
    int size;
};


#endif // XWCHARCODETOUNICODE_H

