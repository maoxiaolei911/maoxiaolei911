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
#include <QByteArray>
#include "XWString.h"
#include "XWApplication.h"
#include "XWPSTokenizer.h"
#include "XWFontSetting.h"
#include "XWCharCodeToUnicode.h"


#define maxUnicodeString 8

struct CharCodeToUnicodeString 
{
    unsigned int c;
    unsigned int u[maxUnicodeString];
    int len;
};


static int getCharFromString(void *data) 
{
    int c = EOF;
    char * p = *(char **)data;
    if (*p) 
    {
        c = *p++;
        *(char **)data = p;
    } 
    
    return c;
}


static int getCharFromFile(void *data) 
{
	QIODevice * f = (QIODevice*)data;
	char c;
	if (f->getChar(&c))
    	return (int)c;
    	
    return EOF;
}

static int hexCharVals[256] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 1x
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 2x
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1, // 3x
  -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 4x
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 5x
  -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 6x
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 7x
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 8x
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 9x
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // Ax
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // Bx
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // Cx
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // Dx
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // Ex
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  // Fx
};

// Parse a <len>-byte hex string <s> into *<val>.  Returns false on
// error.
static bool parseHex(char *s, int len, uint *val) {
  int i, x;

  *val = 0;
  for (i = 0; i < len; ++i) 
  {
    x = hexCharVals[s[i] & 0xff];
    if (x < 0) {
      return false;
    }
    *val = (*val << 4) + x;
  }
  return true;
}

XWCharCodeToUnicode::XWCharCodeToUnicode()
{
	tag = NULL;
  map = NULL;
  mapLen = 0;
  sMap = NULL;
  sMapLen = sMapSize = 0;
  refCnt = 1;
}

XWCharCodeToUnicode::XWCharCodeToUnicode(XWString *tagA)
{
    tag = tagA;
    mapLen = 256;
    map = (unsigned int *)malloc(mapLen * sizeof(unsigned int));
    for (unsigned int i = 0; i < mapLen; ++i) 
        map[i] = 0;
        
    sMap = NULL;
    sMapLen = sMapSize = 0;
    refCnt = 1;
}

XWCharCodeToUnicode::XWCharCodeToUnicode(XWString *tagA, unsigned int *mapA,
		                                 unsigned int mapLenA, bool copyMap,
		                                 CharCodeToUnicodeString *sMapA,
		                                 int sMapLenA, int sMapSizeA)
{
    tag = tagA;
    mapLen = mapLenA;
    if (copyMap) 
    {
        map = (unsigned int *)malloc(mapLen * sizeof(unsigned int));
        memcpy(map, mapA, mapLen * sizeof(unsigned int));
    } 
    else 
        map = mapA;
        
    sMap = sMapA;
    sMapLen = sMapLenA;
    sMapSize = sMapSizeA;
    refCnt = 1;
}

XWCharCodeToUnicode::~XWCharCodeToUnicode()
{
    if (tag) 
        delete tag;
    
    if (map)
    	free(map);
    
    if (sMap) 
        free(sMap);
}

void XWCharCodeToUnicode::decRefCnt()
{
	mutex.lock();
  bool done = --refCnt == 0;
  mutex.unlock();
  if (done) 
     delete this;
}

void XWCharCodeToUnicode::incRefCnt()
{
	mutex.lock();
  ++refCnt;
  mutex.unlock();
}

XWCharCodeToUnicode * XWCharCodeToUnicode::makeIdentityMapping()
{
	return new XWCharCodeToUnicode();
}

XWCharCodeToUnicode *XWCharCodeToUnicode::make8BitToUnicode(unsigned int *toUnicode)
{
    return new XWCharCodeToUnicode(0, toUnicode, 256, true, NULL, 0, 0);
}

int XWCharCodeToUnicode::mapToUnicode(unsigned int c, unsigned int *u, int size)
{
	if (!map) 
	{
    u[0] = (int)c;
    return 1;
  }
  
    if (c >= mapLen) 
        return 0;
    
    if (map[c]) 
    {
        u[0] = map[c];
        return 1;
    }
    
    for (int i = 0; i < sMapLen; ++i) 
    {
        if (sMap[i].c == c) 
        {
        	int j = 0;
            for (; j < sMap[i].len && j < size; ++j) 
	            u[j] = sMap[i].u[j];
	            
            return j;
        }
    }
    return 0;
}

bool XWCharCodeToUnicode::match(XWString *tagA)
{
    return tag && !tag->cmp(tagA);
}

void  XWCharCodeToUnicode::mergeCMap(XWString *buf, int nBits)
{
    char * p = buf->getCString();
    parseCMap1(&getCharFromString, &p, nBits);
}

XWCharCodeToUnicode * XWCharCodeToUnicode::parseCIDToUnicode(XWString *fileName,
					                                         XWString *collection)
{
    QString fn = QFile::decodeName(fileName->getCString());
    QFile f(fn);
    if (!f.open(QIODevice::ReadOnly))
    	return 0;
        
    unsigned int size = 32768;
    unsigned int * mapA = (unsigned int *)malloc(size * sizeof(unsigned int));
    unsigned int mapLenA = 0;
    char buf[64];
    unsigned int u = 0;
    while (f.readLine(buf, sizeof(buf)) > 0) 
    {
    	if (buf[0] == '\n')
    		continue;
    		
        if (mapLenA == size) 
        {
            size *= 2;
            mapA = (unsigned int *)realloc(mapA, size * sizeof(unsigned int));
        }
        
        if (sscanf(buf, "%x", &u) == 1)
            mapA[mapLenA] = u;
        else 
            mapA[mapLenA] = 0;
        
        ++mapLenA;
    }
    
    f.close();

    XWCharCodeToUnicode * ctu = new XWCharCodeToUnicode(collection->copy(), mapA, mapLenA, true,
			      NULL, 0, 0);
    free(mapA);
    return ctu;
}

XWCharCodeToUnicode *XWCharCodeToUnicode::parseCMap(XWString *buf, int nBits)
{
    XWCharCodeToUnicode * ctu = new XWCharCodeToUnicode(0);
    char * p = buf->getCString();
    ctu->parseCMap1(&getCharFromString, &p, nBits);
    return ctu;
}

XWCharCodeToUnicode *XWCharCodeToUnicode::parseUnicodeToUnicode(XWString *fileName)
{
    QString fn = QFile::decodeName(fileName->getCString());
    QFile f(fn);
    if (!f.open(QIODevice::ReadOnly))
    	return 0;
        
    unsigned int size = 4096;
    unsigned int * mapA = (unsigned int *)malloc(size * sizeof(unsigned int));
    memset(mapA, 0, size * sizeof(unsigned int));
    unsigned int len = 0;
    CharCodeToUnicodeString * sMapA = NULL;
    unsigned int sMapSizeA = 0, sMapLenA = 0;
    int line = 0;
    
    char buf[256];
    char *tok = 0;
    unsigned int u0 = 0;
    int uBuf[maxUnicodeString];
    while (f.readLine(buf, sizeof(buf)) > 0)
    {
        ++line;
        if (buf[0] == '\n')
        	continue;
        	
        if (!(tok = strtok(buf, " \t\r\n")) || sscanf(tok, "%x", &u0) != 1)
            continue;
      
        int n = 0;
        while (n < maxUnicodeString) 
        {
            if (!(tok = strtok(NULL, " \t\r\n"))) 
	            break;
	            
	        if (sscanf(tok, "%x", &uBuf[n]) != 1)
	        	break;
	        	    
            ++n;
        }
        
        if (n < 1) 
            continue;
            
        if (u0 >= size) 
        {
            unsigned int oldSize = size;
            while (u0 >= size) 
	            size *= 2;
	            
            mapA = (unsigned int *)realloc(mapA, size * sizeof(unsigned int));
            memset(mapA + oldSize, 0, (size - oldSize) * sizeof(unsigned int));
        }
        
        if (n == 1) 
            mapA[u0] = uBuf[0];
        else 
        {
            mapA[u0] = 0;
            if (sMapLenA == sMapSizeA) 
            {
	            sMapSizeA += 16;
	            sMapA = (CharCodeToUnicodeString *)realloc(sMapA, sMapSizeA * sizeof(CharCodeToUnicodeString));
            }
            
            sMapA[sMapLenA].c = u0;
            for (int i = 0; i < n; ++i) 
	            sMapA[sMapLenA].u[i] = uBuf[i];
            
            sMapA[sMapLenA].len = n;
            ++sMapLenA;
        }
        
        if (u0 >= len) 
            len = u0 + 1;
    }
    
    f.close();
    
    XWCharCodeToUnicode * ctu = new XWCharCodeToUnicode(fileName->copy(), mapA, len, true,
			                                            sMapA, sMapLenA, sMapSizeA);
    free(mapA);
    return ctu;
}

void XWCharCodeToUnicode::setMapping(unsigned int c, unsigned int *u, int len)
{
	if (!map) 
	{
    return;
  }
  
    if (len == 1) 
        map[c] = u[0];
    else 
    {
        int i = 0;
        for (; i < sMapLen; ++i) 
        {
            if (sMap[i].c == c) 
	            break;
        }
        
        if (i == sMapLen) 
        {
            if (sMapLen == sMapSize) 
            {
	            sMapSize += 8;
	            sMap = (CharCodeToUnicodeString *)realloc(sMap, sMapSize * sizeof(CharCodeToUnicodeString));
            }
            ++sMapLen;
        }
        
        map[c] = 0;
        sMap[i].c = c;
        sMap[i].len = len;
        for (int j = 0; j < len && j < maxUnicodeString; ++j) 
            sMap[i].u[j] = u[j];
    }
}

void XWCharCodeToUnicode::addMapping(unsigned int code, char *uStr, int n, int offset)
{
	if (code > 0xffffff)
		return ;
		
	uint oldLen, i;
  uint u;
  uint j;
	if (code >= mapLen) 
	{
    oldLen = mapLen;
    mapLen = mapLen ? 2 * mapLen : 256;
    if (code >= mapLen) 
    {
      mapLen = (code + 256) & ~255;
    }
    map = (uint *)realloc(map, mapLen * sizeof(uint));
    for (i = oldLen; i < mapLen; ++i) 
   	{
      map[i] = 0;
    }
  }
  if (n <= 4) 
  {
    if (!parseHex(uStr, n, &u)) 
    {
      xwApp->error("Illegal entry in ToUnicode CMap");
      return;
    }
    map[code] = u + offset;
  } 
  else 
  {
    if (sMapLen >= sMapSize) 
    {
      sMapSize = sMapSize + 16;
      sMap = (CharCodeToUnicodeString *)realloc(sMap, sMapSize * sizeof(CharCodeToUnicodeString));
    }
    map[code] = 0;
    sMap[sMapLen].c = code;
    if ((sMap[sMapLen].len = n / 4) > maxUnicodeString) 
    {
      sMap[sMapLen].len = maxUnicodeString;
    }
    for (j = 0; j < sMap[sMapLen].len; ++j) 
    {
      if (!parseHex(uStr + j*4, 4, &sMap[sMapLen].u[j])) 
      {
				xwApp->error("Illegal entry in ToUnicode CMap");
				return;
      }
    }
    sMap[sMapLen].u[sMap[sMapLen].len - 1] += offset;
    ++sMapLen;
  }
}

void XWCharCodeToUnicode::parseCMap1(int (*getCharFunc)(void *), void *data, int nBits)
{
	XWPSTokenizer *pst;
  char tok1[256], tok2[256], tok3[256];
  int n1, n2, n3;
  int i;
  uint maxCode, code1, code2;
  XWString *name;
  
  maxCode = (nBits == 8) ? 0xff : (nBits == 16) ? 0xffff : 0xffffffff;
  pst = new XWPSTokenizer(getCharFunc, data);
  pst->getToken(tok1, sizeof(tok1), &n1);
  XWFontSetting fontsetting;
  while (pst->getToken(tok2, sizeof(tok2), &n2)) 
  {
    if (!strcmp(tok2, "usecmap")) 
    {
      if (tok1[0] == '/') 
      {
				name = new XWString(tok1 + 1);
				QFile * f = fontsetting.findToUnicodeFile(name);
				if (f) 
				{
	  			parseCMap1(&getCharFromFile, f, nBits);
	  			f->close();
	  			delete f;
				} 
				delete name;
      }
      pst->getToken(tok1, sizeof(tok1), &n1);
    } 
    else if (!strcmp(tok2, "beginbfchar")) 
    {
      while (pst->getToken(tok1, sizeof(tok1), &n1)) 
      {
				if (!strcmp(tok1, "endbfchar")) 
				{
	  			break;
				}
				if (!pst->getToken(tok2, sizeof(tok2), &n2) || !strcmp(tok2, "endbfchar")) 
				{
	  			xwApp->error("Illegal entry in bfchar block in ToUnicode CMap");
	  			break;
				}
				if (!(tok1[0] == '<' && tok1[n1 - 1] == '>' && tok2[0] == '<' && tok2[n2 - 1] == '>')) 
				{
	  			xwApp->error("Illegal entry in bfchar block in ToUnicode CMap");
	  			continue;
				}
				tok1[n1 - 1] = tok2[n2 - 1] = '\0';
				if (!parseHex(tok1 + 1, n1 - 2, &code1)) 
				{
	  			xwApp->error("Illegal entry in bfchar block in ToUnicode CMap");
	  			continue;
				}
				if (code1 > maxCode) 
				{
	  			xwApp->error("Invalid entry in bfchar block in ToUnicode CMap");
				}
				addMapping(code1, tok2 + 1, n2 - 2, 0);
      }
      pst->getToken(tok1, sizeof(tok1), &n1);
    } 
    else if (!strcmp(tok2, "beginbfrange")) 
    {
      while (pst->getToken(tok1, sizeof(tok1), &n1)) 
     	{
				if (!strcmp(tok1, "endbfrange")) 
				{
	  			break;
				}
				if (!pst->getToken(tok2, sizeof(tok2), &n2) ||
	    			!strcmp(tok2, "endbfrange") ||
	    			!pst->getToken(tok3, sizeof(tok3), &n3) ||
	    			!strcmp(tok3, "endbfrange")) 
	    	{
	  			xwApp->error("Illegal entry in bfrange block in ToUnicode CMap");
	  			break;
				}
				if (!(tok1[0] == '<' && tok1[n1 - 1] == '>' &&
	      		tok2[0] == '<' && tok2[n2 - 1] == '>')) 
	      {
	  			xwApp->error("Illegal entry in bfrange block in ToUnicode CMap");
	  			continue;
				}
				tok1[n1 - 1] = tok2[n2 - 1] = '\0';
				if (!parseHex(tok1 + 1, n1 - 2, &code1) || !parseHex(tok2 + 1, n2 - 2, &code2)) 
				{
	  			xwApp->error("Illegal entry in bfrange block in ToUnicode CMap");
	  			continue;
				}
				if (code1 > maxCode || code2 > maxCode) 
				{
	  			xwApp->error("Invalid entry in bfrange block in ToUnicode CMap");
	  			if (code1 > maxCode) 
	  			{
	    			code1 = maxCode;
	  			}
	  			if (code2 > maxCode) 
	  			{
	    			code2 = maxCode;
	  			}
				}
				if (!strcmp(tok3, "[")) 
				{
	  			i = 0;
	  			while (pst->getToken(tok1, sizeof(tok1), &n1) && code1 + i <= code2) 
	  			{
	    			if (!strcmp(tok1, "]")) 
	    			{
	      			break;
	    			}
	    			if (tok1[0] == '<' && tok1[n1 - 1] == '>') 
	    			{
	      			tok1[n1 - 1] = '\0';
	      			addMapping(code1 + i, tok1 + 1, n1 - 2, 0);
	    			} 
	    			else 
	    			{
	      			xwApp->error("Illegal entry in bfrange block in ToUnicode CMap");
	    			}
	    			++i;
	  			}
				} 
				else if (tok3[0] == '<' && tok3[n3 - 1] == '>') 
				{
	  			tok3[n3 - 1] = '\0';
	  			for (i = 0; code1 <= code2; ++code1, ++i) 
	  			{
	    			addMapping(code1, tok3 + 1, n3 - 2, i);
	  			}

				} 
				else 
				{
	  			xwApp->error("Illegal entry in bfrange block in ToUnicode CMap");
				}
      }
      pst->getToken(tok1, sizeof(tok1), &n1);
    } 
    else 
    {
      strcpy(tok1, tok2);
    }
  }
  delete pst;
}

XWCharCodeToUnicodeCache::XWCharCodeToUnicodeCache(int sizeA)
{
    size = sizeA;
    cache = (XWCharCodeToUnicode **)malloc(size * sizeof(XWCharCodeToUnicode *));
    for (int i = 0; i < size; ++i) 
        cache[i] = 0;
}

XWCharCodeToUnicodeCache::~XWCharCodeToUnicodeCache()
{
    for (int i = 0; i < size; ++i) 
    {
        if (cache[i]) 
            cache[i]->decRefCnt();
    }
    
    if (cache)
    	free(cache);
}

void XWCharCodeToUnicodeCache::add(XWCharCodeToUnicode *ctu)
{
    if (cache[size - 1]) 
        cache[size - 1]->decRefCnt();
        
    for (int i = size - 1; i >= 1; --i) 
        cache[i] = cache[i - 1];
        
    cache[0] = ctu;
    ctu->incRefCnt();
}

XWCharCodeToUnicode * XWCharCodeToUnicodeCache::getCharCodeToUnicode(XWString *tag)
{
    if (cache[0] && cache[0]->match(tag)) 
    {
        cache[0]->incRefCnt();
        return cache[0];
    }
    
    for (int i = 1; i < size; ++i) 
    {
        if (cache[i] && cache[i]->match(tag)) 
        {
            XWCharCodeToUnicode * ctu = cache[i];
            for (int j = i; j >= 1; --j) 
	            cache[j] = cache[j - 1];
	            
            cache[0] = ctu;
            ctu->incRefCnt();
            return ctu;
        }
    }
    return 0;
}


