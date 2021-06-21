/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <QString>
#include <QFile>
#include <QTextCodec>
#include <QByteArray>
#include "XWString.h"
#include "XWList.h"
#include "XWFontSetting.h"
#include "XWUnicodeMap.h"

#define maxExtCode 16

struct UnicodeMapExt 
{
    uint u;			// Unicode char
    char code[maxExtCode];
    uint nBytes;
};

XWUnicodeMap::XWUnicodeMap(XWString *encodingNameA) 
{
    encodingName = encodingNameA;
    unicodeOut = false;
    kind = unicodeMapUser;
    ranges = NULL;
    len = 0;
    eMaps = 0;
    eMapsLen = 0;
    refCnt = 1;
}

XWUnicodeMap::XWUnicodeMap(char *encodingNameA, 
                           bool unicodeOutA,
		                   UnicodeMapRange *rangesA, 
		                   int lenA) 
{
    encodingName = new XWString(encodingNameA);
    unicodeOut = unicodeOutA;
    kind = unicodeMapResident;
    ranges = rangesA;
    len = lenA;
    eMaps = 0;
    eMapsLen = 0;
    refCnt = 1;
}

XWUnicodeMap::XWUnicodeMap(char *encodingNameA, 
                           bool unicodeOutA,
		                   UnicodeMapFunc funcA) 
{
    encodingName = new XWString(encodingNameA);
    unicodeOut = unicodeOutA;
    kind = unicodeMapFunc;
    func = funcA;
    eMaps = 0;
    eMapsLen = 0;
    refCnt = 1;
}

XWUnicodeMap::~XWUnicodeMap() 
{
	if (encodingName)
    	delete encodingName;
    	
    if (kind == unicodeMapUser && ranges) 
        free(ranges);
        
    if (eMaps) 
        free(eMaps);
}

void XWUnicodeMap::decRefCnt() 
{
	mutex.lock();
    bool done = --refCnt == 0;
  mutex.unlock();
    if (done) 
        delete this;
}

void XWUnicodeMap::incRefCnt() 
{
	mutex.lock();
    ++refCnt;
 	mutex.unlock();
}

int XWUnicodeMap::mapUnicode(uint u, char *buf, int bufSize) 
{
    if (kind == unicodeMapFunc) 
        return (*func)(u, buf, bufSize);

    int a = 0;
    int b = len;
    if (u >= ranges[a].start) 
    {
        while (b - a > 1) 
        {
            int m = (a + b) / 2;
            if (u >= ranges[m].start) 
	            a = m;
            else if (u < ranges[m].start) 
	            b = m;
        }
        
        if (u <= ranges[a].end) 
        {
            int n = ranges[a].nBytes;
            if (n > bufSize) 
	            return 0;
	            
            uint code = ranges[a].code + (u - ranges[a].start);
            for (int i = n - 1; i >= 0; --i) 
            {
	            buf[i] = (char)(code & 0xff);
	            code >>= 8;
            }
            return n;
        }
    }

    for (int i = 0; i < eMapsLen; ++i) 
    {
        if (eMaps[i].u == u) 
        {
            int n = eMaps[i].nBytes;
            for (int j = 0; j < n; ++j) 
	            buf[j] = eMaps[i].code[j];
	            
            return n;
        }
    }

    return 0;
}

bool XWUnicodeMap::match(XWString *encodingNameA) 
{
    return !encodingName->cmp(encodingNameA);
}

XWUnicodeMap *XWUnicodeMap::parse(XWString *encodingNameA) 
{
	XWFontSetting fontsetting;
	QFile * f = fontsetting.getUnicodeMapFile(encodingNameA);
	if (!f)
        return 0;

    XWUnicodeMap * map = new XWUnicodeMap(encodingNameA->copy());

    int size = 8;
    map->ranges = (UnicodeMapRange *)malloc(size * sizeof(UnicodeMapRange));
    int eMapsSize = 0;
    int line = 1;
    char *tok1, *tok2, *tok3;
    char buf[256];
    while (f->readLine(buf, sizeof(buf)) > 0) 
    {
    	if (buf[0] == '\n')
    		continue;
    		
        if ((tok1 = strtok(buf, " \t\r\n")) && (tok2 = strtok(NULL, " \t\r\n"))) 
        {
            if (!(tok3 = strtok(NULL, " \t\r\n"))) 
            {
	            tok3 = tok2;
	            tok2 = tok1;
            }
            int nBytes = strlen(tok3) / 2;
            if (nBytes <= 4) 
            {
	            if (map->len == size) 
	            {
	                size *= 2;
	                map->ranges = (UnicodeMapRange *)realloc(map->ranges, size * sizeof(UnicodeMapRange));
	            }
	            UnicodeMapRange * range = &map->ranges[map->len];
	            sscanf(tok1, "%x", &range->start);
	            sscanf(tok2, "%x", &range->end);
	            sscanf(tok3, "%x", &range->code);
	            range->nBytes = nBytes;
	            ++map->len;
            } 
            else if (tok2 == tok1) 
            {
	            if (map->eMapsLen == eMapsSize) 
	            {
	                eMapsSize += 16;
	                map->eMaps = (UnicodeMapExt *)realloc(map->eMaps, eMapsSize * sizeof(UnicodeMapExt));
	            }
	            UnicodeMapExt * eMap = &map->eMaps[map->eMapsLen];
	            sscanf(tok1, "%x", &eMap->u);
	            int x = 0;
	            for (int i = 0; i < nBytes; ++i) 
	            {
	                sscanf(tok3 + i*2, "%2x", &x);
	                eMap->code[i] = (char)x;
	            }
	            eMap->nBytes = nBytes;
	            ++map->eMapsLen;
            }
        } 
        ++line;
    }
    f->close();
    delete f;

    return map;
}

XWUnicodeMapCache::XWUnicodeMapCache() 
{
    for (int i = 0; i < unicodeMapCacheSize; ++i) 
        cache[i] = 0;
}

XWUnicodeMapCache::~XWUnicodeMapCache() 
{
    for (int i = 0; i < unicodeMapCacheSize; ++i) 
    {
        if (cache[i]) 
            cache[i]->decRefCnt();
    }
}

XWUnicodeMap *XWUnicodeMapCache::getUnicodeMap(XWString *encodingName) 
{
    if (cache[0] && cache[0]->match(encodingName)) 
    {
        cache[0]->incRefCnt();
        return cache[0];
    }
    
    XWUnicodeMap * map = 0;
    for (int i = 1; i < unicodeMapCacheSize; ++i) 
    {
        if (cache[i] && cache[i]->match(encodingName)) 
        {
            map = cache[i];
            for (int j = i; j >= 1; --j) 
	            cache[j] = cache[j - 1];
            cache[0] = map;
            map->incRefCnt();
            return map;
        }
    }
    
    if ((map = XWUnicodeMap::parse(encodingName))) 
    {
        if (cache[unicodeMapCacheSize - 1]) 
            cache[unicodeMapCacheSize - 1]->decRefCnt();
            
        for (int j = unicodeMapCacheSize - 1; j >= 1; --j) 
            cache[j] = cache[j - 1];
            
        cache[0] = map;
        map->incRefCnt();
        return map;
    }
    
    return 0;
}
