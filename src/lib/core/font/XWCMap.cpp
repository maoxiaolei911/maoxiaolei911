/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <ctype.h>
#include <math.h>
#include <QString>
#include <QFile>
#include <QTextCodec>
#include <QByteArray>
#include "XWApplication.h"
#include "XWString.h"
#include "XWObject.h"
#include "XWFontSea.h"
#include "XWFontSetting.h"
#include "XWPFBParser.h"
#include "XWPSTokenizer.h"
#include "XWStream.h"
#include "XWCMap.h"

CIDSysInfo CSI_IDENTITY = {
  	(char *) "Adobe",
  	(char *) "Identity",
  	0
};

CIDSysInfo CSI_UNICODE = {
  	(char *) "Adobe",
  	(char *) "UCS",
  	0
};

#define CMAP_BEGIN "\
/CIDInit /ProcSet findresource begin\n\
12 dict begin\n\
begincmap\n\
"

#define CMAP_END "\
endcmap\n\
CMapName currentdict /CMap defineresource pop\n\
end\n\
end\n\
"

#define INPUT_BUF_SIZE 4096
#define CMAP_SIG_MAX   64
#define TOKEN_LEN_MAX 127

struct sbuf {
  char *buf;
  char *curptr;
  char *limptr;
};

struct CMapVectorEntry 
{
    bool isVector;
    union 
    {
        CMapVectorEntry *vector;
        uint cid;
    };
};

static int getCharFromFile(void *data) 
{
	char c = 0;
	QIODevice * f = (QIODevice*)data;
	if (!f || f->atEnd())
		return EOF;
		
	f->getChar(&c);
  return (int)c;
}

static int getCharFromStream(void *data) 
{
  return ((XWStream *)data)->getChar();
}

class XWCMapFileReader
{
public:
	XWCMapFileReader(QIODevice * fpA, long size, long bufsize);
	~XWCMapFileReader();
	
	int checkNextToken(const char *key);
	
	int getCodeRange(uchar *codeLo, 
	                 uchar *codeHi, 
	                 int *dim, 
	                 int maxlen);
	int  getIV()
		{return parser->getIV();}
	long getLen()
		{return parser->getLen();}
	char * getSV()
		{return parser->getSV();}
	int getToken()
		{ return parser->getToken();}
		
	bool matchName(const char * name)
		{return parser->matchName(name);}
	
	bool matchOp(const char *op)
		{return parser->matchOp(op);}
	
	long read(long size);
	
public:
	uchar *cursor;
	uchar *endptr;
  	uchar *buf;
  	long   max;
  	QIODevice   *fp;
  	long    unread;
  	XWPFBParser * parser;
};

XWCMapFileReader::XWCMapFileReader(QIODevice * fpA, long size, long bufsize)
{
	buf = (uchar*)malloc((bufsize+1) * sizeof(uchar));
	max = bufsize;
	fp  = fpA;
	unread = size;
	cursor = endptr = buf;
	*endptr = 0;
	parser = 0;
}

XWCMapFileReader::~XWCMapFileReader()
{
	if (buf)
		free(buf);
		
	if (parser)
		delete parser;
}

int XWCMapFileReader::checkNextToken(const char *key)
{
	if (read(strlen(key)) < 0)
    	return -1;
    	
    getToken();
    char * str = getSV();
    if (!str)
    	return -1;
    	
    int cmp = strcmp(str, key) ? -1 : 0;
    return cmp;
}

int XWCMapFileReader::getCodeRange(uchar *codeLo, 
	                               uchar *codeHi, 
	                               int *dim, 
	                               int maxlen)
{ 
	int t = getToken();
	if (t != PFB_TYPE_STRING)
		return -1;
		
	char * str = getSV();
	long dim1 = getLen();
	if (dim1 > maxlen)
		return -1;		
	memcpy(codeLo, str, dim1);
	
	t = getToken();
	if (t != PFB_TYPE_STRING)
		return -1;
		
	str = getSV();
	long dim2 = getLen();
	if (dim1 != dim2)
		return -1;
	memcpy(codeHi, str, dim2);

  	*dim = dim1;
  	return 0;
}

long XWCMapFileReader::read(long size)
{
	long bytesrem = (long)endptr - (long)cursor;
	if (size > max)
	{
		buf = (uchar*)realloc(buf, (size + 1) * sizeof(uchar));
		max = size;
	}
	
	long bytesread = 0;
	if (unread > 0 && bytesrem < size)
	{
		bytesread = qMin(max - bytesrem, unread);
		memmove(buf, cursor, bytesrem);
    	cursor = buf;
    	endptr = buf + bytesrem;
    	if (fp->read((char*)endptr, bytesread) != bytesread)
    		return -1;
    		
    	endptr += bytesread;
    	unread -= bytesread;
	}
	
	*endptr = 0;
	if (parser)
		delete parser;
	parser = new XWPFBParser((const char * )cursor, (const char * )endptr);
  	return bytesread + bytesrem;
}

XWCMap::XWCMap(QObject * parent)
	:QObject(parent)
{
	refCnt = 1;
	cMapName = 0;
	wMode = 0;
	cmapType = 1;
	Cmap1 * map1 = new Cmap1;
	map = map1;
	map1->type = CMAP_TYPE_CODE_TO_CID;
	map1->CSI = 0;
	map1->useCMap  = 0;
	map1->flags = 0;
	map1->mapTbl = 0;
	
	map1->mapData = (MapData*)malloc(sizeof(MapData));
	map1->mapData->prev = 0;
	map1->mapData->pos  = 0;
	map1->mapData->data = (uchar*)malloc(4096 * sizeof(uchar));
	
	map1->codeSpace.num    = 0;
	map1->codeSpace.max    = 10;
	map1->codeSpace.ranges = (RangeDef*)malloc(10 * sizeof(RangeDef));
	
	map1->profile.minBytesIn  = 2;
	map1->profile.maxBytesIn  = 2;
	map1->profile.minBytesOut = 2;
	map1->profile.maxBytesOut = 2;
}

XWCMap::XWCMap(XWString *collectionA, 
	           XWString *cMapNameA, 
	           QObject * parent)
	:QObject(parent)
{
	refCnt = 1;
	cMapName = cMapNameA;
	isIdent = false;
	wMode = 0;
	cmapType = 0;
	
	Cmap0 * map0 = new Cmap0;
	map = map0;
	
	map0->collection = collectionA;
    map0->vector = (CMapVectorEntry *)malloc(256 * sizeof(CMapVectorEntry));
    for (int i = 0; i < 256; ++i) 
    {
        map0->vector[i].isVector = false;
        map0->vector[i].cid = 0;
    }
    
}

XWCMap::XWCMap(XWString *collectionA, 
	           XWString *cMapNameA, 
	           int wModeA, 
	           QObject * parent)
	:QObject(parent)
{
	refCnt = 1;
	cMapName = cMapNameA;
	isIdent = true;
	wMode = wModeA;
	cmapType = 0;
	
	Cmap0 * map0 = new Cmap0;
	map = map0;
    map0->collection = collectionA;
    map0->vector = 0;
}

XWCMap::~XWCMap()
{
	if (cMapName)
		delete cMapName;
		
	if (map)
	{				
		switch (cmapType)
		{
			case 0:
				{
					Cmap0 * map0 = (Cmap0*)map;
					if (map0->collection)
    					delete map0->collection;
    	
    				if (map0->vector) 
        				freeCMapVector(map0->vector);
        				
        			delete map0;
				}
				break;
				
			case 1:
				{
					Cmap1 * map1 = (Cmap1*)map;
					if (map1->CSI)
					{
						if (map1->CSI->registry)
							delete [] map1->CSI->registry;
			
						if (map1->CSI->ordering)
							delete [] map1->CSI->ordering;
			
						free(map1->CSI);
					}
					
					if (map1->codeSpace.ranges)
						free(map1->codeSpace.ranges);
					
					if (map1->mapTbl)
						mapDefRelease(map1->mapTbl);
						
					MapData *data = map1->mapData;
    				while (data != 0) 
    				{
      					MapData *prev = data->prev;
      					if (data->data != 0)
							free(data->data);
      					free(data);
      					data = prev;
    				}
					
					delete map1;
				}
				break;
		}
    }
}

int XWCMap::addBFChar(const uchar *src, 
	                  int srcdim,
		              const uchar *dst, 
		              int dstdim)
{
	return addBFRange(src, src, srcdim, dst, dstdim);
}

int XWCMap::addBFRange(const uchar *srclo, 
	                   const uchar *srchi, 
	                   int srcdim,
		  		       const uchar *base, 
		  		       int dstdim)
{
	if (checkRange(srclo, srchi, srcdim, base, dstdim) < 0)
	{
    	return -1;
    }

	Cmap1 * map1 = (Cmap1*)map;
  	if (map1->mapTbl == 0)
    	map1->mapTbl = mapDefNew();

  	MapDef * cur = map1->mapTbl;
  	if (locateTbl(&cur, srclo, srcdim) < 0)
  	{
    	return -1;
    }
    	
    for (int c = srclo[srcdim-1]; c <= srchi[srcdim-1]; c++)
    {
    	if (!MAP_DEFINED(cur[c].flag) || cur[c].len < dstdim) 
    	{
      		cur[c].flag = (MAP_LOOKUP_END|MAP_IS_CODE);
      		cur[c].code = getMem(dstdim);
    	}
    	
    	cur[c].len = dstdim;
    	memcpy(cur[c].code, base, dstdim);

    	int last_byte = c - srclo[srcdim-1] + base[dstdim-1];
    	cur[c].code[dstdim-1] = (last_byte & 0xFF);
    	for (int i = dstdim - 2; i >= 0 && last_byte > 255; i--) 
    	{
      		last_byte = cur[c].code[i] + 1;
      		cur[c].code[i] = (last_byte & 0xFF);
    	}
    }
    return 0;
}

int XWCMap::addCIDChar(const uchar *src, 
	                   int srcdim, 
	                   CID dst)
{
	return addCIDRange(src, src, srcdim, dst);
}

int XWCMap::addCIDRange(const uchar *srclo, 
	                    const uchar *srchi, 
	                    int srcdim, 
	                    CID base)
{
	if (checkRange(srclo, srchi, srcdim, (const unsigned char *)&base, 2) < 0)
	{
    	return -1;
    }

	Cmap1 * map1 = (Cmap1*)map;
  	if (map1->mapTbl == 0)
    	map1->mapTbl = mapDefNew();

  	MapDef * cur = map1->mapTbl;
  	if (locateTbl(&cur, srclo, srcdim) < 0)
  	{
    	return -1;
    }

  	for (int c = srclo[srcdim-1]; c <= srchi[srcdim-1]; c++) 
  	{
    	if (cur[c].flag == 0) 
    	{
      		cur[c].flag = (MAP_LOOKUP_END|MAP_IS_CID);
      		cur[c].len  = 2;
      		cur[c].code = getMem(2);
      		cur[c].code[0] = base >> 8;
      		cur[c].code[1] = base & 0xff;
    	}
    	
    	if (base >= CID_MAX)
      		xwApp->warning(tr("CID number too large.\n"));
    	base++;
  	}
  	return 0;
}

int XWCMap::addCodeSpaceRange(const uchar *codelo, 
	                          const uchar *codehi, 
	                          int dim)
{
	Cmap1 * map1 = (Cmap1*)map;
	RangeDef * csr = 0;	
	for (int i = 0; i < map1->codeSpace.num; i++) 
	{
    	csr = map1->codeSpace.ranges + i;
    	int overlap = 1;
    	for (int j = 0; j < qMin(csr->dim, dim) && overlap; j++) 
    	{
      		if ((codelo[j] >= csr->codeLo[j] && codelo[j] <= csr->codeHi[j]) || 
	  			(codehi[j] >= csr->codeLo[j] && codehi[j] <= csr->codeHi[j]))
				overlap = 1;
      		else
				overlap = 0;
    	}
    	
    	if (overlap) 
    	{
      		xwApp->warning(tr("overlapping codespace found. (ingored)\n"));
      		return -1;
    	}
  	}
  	
  	if (dim < map1->profile.minBytesIn)
    	map1->profile.minBytesIn = dim;
    	
  	if (dim > map1->profile.maxBytesIn)
    	map1->profile.maxBytesIn = dim;

  	if (map1->codeSpace.num + 1 > map1->codeSpace.max) 
  	{
    	map1->codeSpace.max += 10;
    	map1->codeSpace.ranges = (RangeDef*)realloc(map1->codeSpace.ranges, map1->codeSpace.max * sizeof(RangeDef));
  	}
  	
  	csr = map1->codeSpace.ranges + map1->codeSpace.num;
  	csr->dim    = dim;
  	csr->codeHi = getMem(dim);
  	csr->codeLo = getMem(dim);
  	memcpy(csr->codeHi, codehi, dim);
  	memcpy(csr->codeLo, codelo, dim);

  	(map1->codeSpace.num)++;
  	return 0;
}

int XWCMap::addNotDefChar(const uchar *src, 
	                      int srcdim, 
	                      CID dst)
{
	return addNotDefRange(src, src, srcdim, dst);
}

int XWCMap::addNotDefRange(const uchar *srclo, 
	                       const uchar *srchi, 
	                       int srcdim, 
	                       CID dst)
{
	if (checkRange(srclo, srchi, srcdim, (const uchar *)&dst, 2) < 0)
	{
    	return -1;
    }
    	
    Cmap1 * map1 = (Cmap1*)map;
    if (map1->mapTbl == 0)
    	map1->mapTbl = mapDefNew();
    	
    MapDef * cur = map1->mapTbl;
  	if (locateTbl(&cur, srclo, srcdim) < 0)
  	{
    	return -1;
    }
    	
    for (int c = srclo[srcdim-1]; c <= srchi[srcdim-1]; c++) 
    {
    	if (!MAP_DEFINED(cur[c].flag)) 
    	{
      		cur[c].flag = (MAP_LOOKUP_END|MAP_IS_NOTDEF);
      		cur[c].code = getMem(2);
      		cur[c].len  = 2;
      		cur[c].code[0] = dst >> 8;
      		cur[c].code[1] = dst & 0xff;
    	}
  	}
  	return 0;
}

long XWCMap::decode(const uchar **inbuf,  
	                long *inbytesleft,
	                uchar **outbuf, 
	                long *outbytesleft)
{
	int count = 0;	
	for (;*inbytesleft > 0 && *outbytesleft > 0; count++)
    	decodeChar(inbuf, inbytesleft, outbuf, outbytesleft);
  	return count;
}

void XWCMap::decodeChar(const uchar **inbuf, 
	                    long *inbytesleft,
		                uchar **outbuf, 
		                long *outbytesleft)
{
	uchar *p, c = 0, *save;
	p = save = (uchar *) *inbuf;

	Cmap1 * map1 = (Cmap1*)map;	
	if (map1->type == CMAP_TYPE_IDENTITY)
	{
		if ((*inbytesleft) % 2)
		{
      		xwApp->error(tr("invalid/truncated input string.\n"));
      		return ;
      	}
      	
    	if (*outbytesleft < 2)
    		return ;
    		
    	memcpy(*outbuf, *inbuf, 2);
    	*inbuf  += 2;
    	*outbuf += 2;
    	*outbytesleft -= 2;
    	*inbytesleft  -= 2;
    	return;
	}
	else if (!(map1->mapTbl))
	{
		if (map1->useCMap) 
		{
      		map1->useCMap->decodeChar(inbuf, inbytesleft, outbuf, outbytesleft);
      		return;
    	} 
    	else 
    	{
      		/* no mapping available in this CMap */
      		xwApp->warning(tr("no mapping available for this character.\n"));
      		handleUndefined(inbuf, inbytesleft, outbuf, outbytesleft);
      		return;
    	}
	}
	
	MapDef * t = map1->mapTbl;
	long count = 0;
  	while (count < *inbytesleft) 
  	{
    	c = *p++;
    	count++;
    	if (LOOKUP_END(t[c].flag))
      		break;
    	t = t[c].next;
  	}
  	
  	if (LOOKUP_CONTINUE(t[c].flag))
  	{
  		xwApp->error(tr("premature end of input string.\n"));
  		return ;
  	}
  	else if (!MAP_DEFINED(t[c].flag))
  	{
  		if (map1->useCMap) 
  		{
      		map1->useCMap->decodeChar(inbuf, inbytesleft, outbuf, outbytesleft);
      		return;
    	}
    	else
    	{
    		handleUndefined(inbuf, inbytesleft, outbuf, outbytesleft);
      		return;
    	}
  	}
  	else
  	{
  		switch (MAP_TYPE(t[c].flag)) 
  		{
    		case MAP_IS_NOTDEF:
      			xwApp->warning(tr("character mapped to .notdef found.\n"));
      			/* continue */
    		case MAP_IS_CID: 
    		case MAP_IS_CODE:
      			if (*outbytesleft >= t[c].len)
					memcpy(*outbuf, t[c].code, t[c].len);
      			else
					return;
      			*outbuf       += t[c].len;
      			*outbytesleft -= t[c].len;
      			break;
      			
    		case MAP_IS_NAME:
      			xwApp->error(tr("CharName mapping not supported.\n"));
      			return ;
      			break;
      			
    		default:
    			return ;
    			break;
    	}
    	
    	if (inbytesleft)
      		*inbytesleft -= count;
    	*inbuf = p;
  	}
}

void XWCMap::decRefCnt()
{
	mutex.lock();
    bool done = --refCnt == 0;
    mutex.unlock();
    if (done) 
        delete this;
}

uint XWCMap::getCID(char *s, int len, int *c, int *nUsed)
{
	Cmap0 * map0 = (Cmap0*)map;
	CMapVectorEntry * vec = map0->vector;
  int cc = 0;
  int n = 0;
  int i = 0;
  while (vec && n < len) 
  {
    i = s[n++] & 0xff;
    cc = (cc << 8) | i;
    if (!vec[i].isVector) 
    {
      *c = cc;
      *nUsed = n;
      return vec[i].cid;
    }
    vec = vec[i].vector;
  }
  if (isIdent && len >= 2) 
  {
    // identity CMap
    *nUsed = 2;
    *c = cc = ((s[0] & 0xff) << 8) + (s[1] & 0xff);
    return cc;
  }
  *nUsed = 1;
  *c = s[0] & 0xff;
  return 0;
}

CIDSysInfo * XWCMap::getCIDSysInfo()
{
	Cmap1 * map1 = (Cmap1*)map;	
	return map1->CSI;
}

XWString * XWCMap::getCollection()
{
	Cmap0 * map0 = (Cmap0*)map;
	return map0->collection;
}

char * XWCMap::getName()
{
	if (cMapName)
		return cMapName->getCString();
		
	return 0;
}

int XWCMap::getProfile(int typeA)
{
	Cmap1 * map1 = (Cmap1*)map;
	int value = 0;
	switch (typeA) 
	{
  		case CMAP_PROF_TYPE_INBYTES_MIN:
    		value = map1->profile.minBytesIn;
    		break;
    		
  		case CMAP_PROF_TYPE_INBYTES_MAX:
    		value = map1->profile.maxBytesIn;
    		break;
    		
  		case CMAP_PROF_TYPE_OUTBYTES_MIN:
    		value = map1->profile.maxBytesOut;
    		break;
    		
  		case CMAP_PROF_TYPE_OUTBYTES_MAX:
    		value = map1->profile.maxBytesOut;
    		break;
    		
  		default:
  			break;
  	}

  	return value;
}

XWCMap * XWCMap::getUseCMap()
{
	Cmap1 * map1 = (Cmap1*)map;
	return map1->useCMap;
}

int XWCMap::getType()
{
	Cmap1 * map1 = (Cmap1*)map;
	return map1->type;
}

void XWCMap::incRefCnt()
{
	mutex.lock();
	++refCnt;
	mutex.unlock();
}

bool XWCMap::isIdentity()
{
	if (!cMapName->cmp("Identity-H") || !cMapName->cmp("Identity-V"))
    	return true;
    
    return 0;
}

bool XWCMap::isValid()
{
	Cmap1 * map1 = (Cmap1*)map;
	if (map1->type < CMAP_TYPE_IDENTITY || 
      	map1->type > CMAP_TYPE_CID_TO_CODE || 
      	map1->codeSpace.num < 1 ||
      	(map1->type != CMAP_TYPE_IDENTITY && !(map1->mapTbl)))
    {
    	return false;
    }
    
    if (map1->useCMap) 
    {
    	CIDSysInfo * csi1 = getCIDSysInfo();
    	CIDSysInfo * csi2 = map1->useCMap->getCIDSysInfo();
    	if (strcmp(csi1->registry, csi2->registry) ||
			strcmp(csi1->ordering, csi2->ordering)) 
		{
			char * name1 = getName();
			char * name2 = map1->useCMap->getName();
			QString msg = QString(tr("CIDSystemInfo mismatched %1 <--> %2\n")).arg(name1).arg(name2);
      		xwApp->warning(msg);
      		return false;
    	}
  	}

  	return true;
}

bool XWCMap::match(XWString *collectionA, XWString *cMapNameA)
{
	Cmap0 * map0 = (Cmap0*)map;
    return !(((map0->collection)->cmp(collectionA))) && !cMapName->cmp(cMapNameA);
}

int  XWCMap::matchCodeSpace(const uchar *c, int dim)
{
	Cmap1 * map1 = (Cmap1*)map;
	for (int i = 0; i < map1->codeSpace.num; i++) 
	{
    	RangeDef *csr = map1->codeSpace.ranges + i;
    	if (csr->dim != dim)
      		continue;
      		
    	int pos = 0;
    	for (; pos < dim; pos++) 
    	{
      		if (c[pos] > csr->codeHi[pos] || c[pos] < csr->codeLo[pos])
				break;
    	}
    	
    	if (pos == dim)
      		return 0; /* Valid */
  	}

  	return -1;
}

bool XWCMap::parse(QIODevice *fp, XWCMapCache * cache)
{
	XWCMapFileReader input(fp, fp->size(), INPUT_BUF_SIZE-1);
	
	int status = 0, tmpint = -1;
	
	while (status >= 0)
	{
		input.read(INPUT_BUF_SIZE/2);
		int typeA = input.getToken();
		char * tok1 = input.getSV();
		if (typeA == PFB_TYPE_END)
			break;
		else if (input.matchName("CMapName"))
		{
			typeA = input.getToken();
			char * tok2 = input.getSV();
			if (!(typeA == PFB_TYPE_NAME || typeA == PFB_TYPE_STRING) || input.checkNextToken("def") < 0)
				status = -1;
      		else
				setName(tok2);
		}
		else if (input.matchName("CMapType"))
		{
			typeA = input.getToken();
			if (typeA != PFB_TYPE_INTEGER || input.checkNextToken("def") < 0)
				status = -1;
      		else
				setType(input.getIV());
		}
		else if (input.matchName("WMode"))
		{
			typeA = input.getToken();
			if (typeA != PFB_TYPE_INTEGER || input.checkNextToken("def") < 0)
				status = -1;
      		else
				setWMode(input.getIV());
		}
		else if (input.matchName("CIDSystemInfo"))
			status = doCIDSystemInfo(&input);
		else if (typeA == PFB_TYPE_NAME)
		{
			tok1 = qstrdup(tok1);
			typeA = input.getToken();
			if (input.matchName("usecmap")) 
	  		{
				int id = cache->find(tok1);
				if (id < 0)
	  				status = -1;
				else 
				{
	  				XWCMap * ucmap = cache->get(id);
	  				setUseCMap(ucmap);
				}
      		}
      		
      		delete [] tok1;
		}
		else if (input.matchOp("begincodespacerange")) 
      		status = doCodeSpaceRange(&input, tmpint);
    	else if (input.matchOp("beginnotdefrange")) 
      		status = doNotDefRange(&input, tmpint);
    	else if (input.matchOp("beginnotdefchar")) 
      		status = doNotDefChar(&input, tmpint);
    	else if (input.matchOp("beginbfrange")) 
      		status = doBFRange(&input, tmpint);
    	else if (input.matchOp("beginbfchar")) 
      		status =  doBFChar(&input, tmpint);
    	else if (input.matchOp("begincidrange")) 
      		status = doCIDRange(&input, tmpint);
    	else if (input.matchOp("begincidchar")) 
      		status =  doCIDChar(&input, tmpint);
    	else if (typeA == PFB_TYPE_INTEGER) 
      		tmpint = input.getIV();
	}
	
	return (status < 0) ? false : isValid();
}

XWCMap * XWCMap::parse(XWCMapCache *, XWString *collectionA, XWObject *obj)
{
	XWCMap *cMap;
  XWString *cMapNameA;

	XWFontSetting fontsetting;
  if (obj->isName()) 
  {
    cMapNameA = new XWString(obj->getName());
    if (!(cMap = fontsetting.getCMap(collectionA, cMapNameA))) 
    {
    	QString msg = QString("Unknown CMap '{0:%1}' for character collection '{1:%2}'")
    	              .arg(cMapNameA->getCString()).arg(collectionA->getCString());
      xwApp->error(msg);
    }
    delete cMapNameA;
  } 
  else if (obj->isStream()) 
  {
    if (!(cMap = XWCMap::parse(NULL, collectionA, obj->getStream()))) 
    {
      xwApp->error("Invalid CMap in Type 0 font");
    }
  } 
  else 
  {
    xwApp->error("Invalid Encoding in Type 0 font");
    return NULL;
  }
  return cMap;
}

XWCMap * XWCMap::parse(XWCMapCache *cache, XWString *collectionA, XWStream *str)
{
	XWObject obj1;
  XWCMap *cMap;

  cMap = new XWCMap(collectionA->copy(), NULL);

  if (!str->getDict()->lookup("UseCMap", &obj1)->isNull()) 
  {
    cMap->useCMap(cache, &obj1);
  }
  obj1.free();

  str->reset();
  cMap->parse2(cache, &getCharFromStream, str);
  str->close();
  return cMap;
}

XWCMap * XWCMap::parse(XWCMapCache *cache, 
                       XWString *collectionA,
		               XWString *cMapNameA)
{
	XWFontSetting fontsetting;
	QFile * f = fontsetting.findCMapFile(collectionA, cMapNameA);
    if (!f)
    {
        if (!cMapNameA->cmp("Identity") || !cMapNameA->cmp("Identity-H")) 
            return new XWCMap(collectionA->copy(), cMapNameA->copy(), 0);
        
        if (!cMapNameA->cmp("Identity-V")) 
            return new XWCMap(collectionA->copy(), cMapNameA->copy(), 1);
            
		QString msg = QString(tr("couldn't find '%1' CMap file for '%2' collection.\n"))
		                      .arg(cMapNameA->getCString()).arg(collectionA->getCString());
		xwApp->error(msg);
            
        return 0;
    }
    
    XWCMap * cMap = new XWCMap(collectionA->copy(), cMapNameA->copy());
    cMap->parse2(cache, &getCharFromFile, f);
    f->close();
    delete f;
    return cMap;
}

int XWCMap::parseCheckSig(QIODevice *fp)
{
	char sig[CMAP_SIG_MAX+1];
	
	fp->seek(0);
	
	int result = -1;
	if (fp->read(sig, CMAP_SIG_MAX) != CMAP_SIG_MAX)
		result = -1;
	else
	{
		sig[CMAP_SIG_MAX] = 0;
    	if (strncmp(sig, "%!PS", 4))
      		result = -1;
    	else if (strstr(sig + 4, "Resource-CMap"))
      		result = 0;
	}
	
	fp->seek(0);
	return result;
}

void XWCMap::setCIDSysInfo(const CIDSysInfo *csi)
{
	Cmap1 * map1 = (Cmap1*)map;
	if (map1->CSI) 
	{
    	if (map1->CSI->registry)
      		delete [] map1->CSI->registry;
      		
    	if (map1->CSI->ordering)
      		delete [] (map1->CSI->ordering);
    	free(map1->CSI);
  	}
  	
  	if (csi && csi->registry && csi->ordering) 
  	{
    	map1->CSI = (CIDSysInfo*)malloc(sizeof(CIDSysInfo));
    	map1->CSI->registry = qstrdup(csi->registry);
    	map1->CSI->ordering = qstrdup(csi->ordering);
    	map1->CSI->supplement = csi->supplement;
  	} 
  	else 
  	{
    	xwApp->warning(tr("invalid CIDSystemInfo.\n"));
    	map1->CSI = 0;
  	}
}

void XWCMap::setName(const char *nameA)
{
	if (cMapName)
		delete cMapName;
		
	cMapName = new XWString(nameA);
}

void XWCMap::setType(int typeA)
{
	Cmap1 * map1 = (Cmap1*)map;
	map1->type = typeA;
}

void XWCMap::setUseCMap(XWCMap *ucmap)
{
	Cmap1 * map1 = (Cmap1*)map;
	if (this == ucmap || !ucmap->isValid())
		return ;
		
	if (cMapName && cMapName->cmp(ucmap->cMapName) == 0)
		return ;
		
	Cmap1 * map11 = (Cmap1*)(ucmap->map);
		
	if (map1->CSI && map1->CSI->registry && map1->CSI->ordering)
	{
		if (strcmp(map1->CSI->registry, map11->CSI->registry) ||
			strcmp(map1->CSI->ordering, map11->CSI->ordering))
			return ;
	}
	
	for (int i = 0; i < map11->codeSpace.num; i++) 
	{
    	RangeDef *csr = map11->codeSpace.ranges + i;
    	addCodeSpaceRange(csr->codeLo, csr->codeHi, csr->dim);
  	}

  	map1->useCMap = ucmap;
}

void XWCMap::writeStream(QIODevice *fp)
{
	Cmap1 * map1 = (Cmap1*)map;
	sbuf wbuf;
	
	CIDSysInfo *csi = getCIDSysInfo();
	if (!csi) 
    	csi = (map1->type != CMAP_TYPE_TO_UNICODE) ? &CSI_IDENTITY : &CSI_UNICODE;
	
#define WBUF_SIZE 4096
  	wbuf.buf = (char*)malloc(WBUF_SIZE * sizeof(char));
  	uchar * codestr  = (uchar*)malloc((map1->profile.maxBytesIn) * sizeof(uchar));
  	memset(codestr, 0, map1->profile.maxBytesIn);
  	
  	wbuf.curptr = wbuf.buf;
  	wbuf.limptr = wbuf.buf + WBUF_SIZE - 2 * (map1->profile.maxBytesIn + map1->profile.maxBytesOut) + 16;
  	
  	fp->write((char *) CMAP_BEGIN, strlen(CMAP_BEGIN));
  	
  	wbuf.curptr += sprintf(wbuf.curptr, "/CMapName /%s def\n", cMapName->getCString());
  	wbuf.curptr += sprintf(wbuf.curptr, "/CMapType %d def\n" , map1->type);
  	if (wMode != 0 && map1->type != CMAP_TYPE_TO_UNICODE)
    	wbuf.curptr += sprintf(wbuf.curptr, "/WMode %d def\n", wMode);

#define CMAP_CSI_FMT "/CIDSystemInfo <<\n\
  /Registry (%s)\n\
  /Ordering (%s)\n\
  /Supplement %d\n\
>> def\n"
  	wbuf.curptr += sprintf(wbuf.curptr, CMAP_CSI_FMT, csi->registry, csi->ordering, csi->supplement);
  	fp->write(wbuf.buf, (long)(wbuf.curptr - wbuf.buf));
  	wbuf.curptr = wbuf.buf;
  	
  	RangeDef * ranges = map1->codeSpace.ranges;
  	wbuf.curptr += sprintf(wbuf.curptr, "%d begincodespacerange\n", map1->codeSpace.num);
  	for (int i = 0; i < map1->codeSpace.num; i++) 
  	{
    	*(wbuf.curptr)++ = '<';
    	for (int j = 0; j < ranges[i].dim; j++) 
      		sputx(ranges[i].codeLo[j], &(wbuf.curptr), wbuf.limptr);
      		
    	*(wbuf.curptr)++ = '>';
    	*(wbuf.curptr)++ = ' ';
    	*(wbuf.curptr)++ = '<';
    	for (int j = 0; j < ranges[i].dim; j++) 
      		sputx(ranges[i].codeHi[j], &(wbuf.curptr), wbuf.limptr);
    	
    	*(wbuf.curptr)++ = '>';
    	*(wbuf.curptr)++ = '\n';
  	}
  	
  	fp->write(wbuf.buf, (long)(wbuf.curptr - wbuf.buf));
  	wbuf.curptr = wbuf.buf;
  	fp->write("endcodespacerange\n", strlen("endcodespacerange\n"));
  	
  	if (map1->mapTbl) 
  	{
    	int count = writeMap(map1->mapTbl, 0, codestr, 0, &wbuf, fp); /* Top node */
    	if (count > 0) 
    	{ /* Flush */
      		char fmt_buf[32];
      		if (count > 100)
      		{
				free(codestr);
				free(wbuf.buf);
				return ;
			}
      		sprintf(fmt_buf, "%d beginbfchar\n", count);
      		fp->write(fmt_buf,  strlen(fmt_buf));
      		fp->write(wbuf.buf, (long) (wbuf.curptr - wbuf.buf));
      		fp->write("endbfchar\n", strlen("endbfchar\n"));
      		count = 0;
      		wbuf.curptr = wbuf.buf;
    	}
  	}
  	/* End CMap */
  	fp->write(CMAP_END, strlen(CMAP_END));
  	free(codestr);
  	free(wbuf.buf);
}

void XWCMap::addCIDs(uint start, uint end, uint nBytes, uint firstCID)
{
	CMapVectorEntry *vec;
  uint cid;
  int byte;
  uint i, j;

	Cmap0 * map0 = (Cmap0*)map;
  vec = map0->vector;
  for (i = nBytes - 1; i >= 1; --i) {
    byte = (start >> (8 * i)) & 0xff;
    if (!vec[byte].isVector) {
      vec[byte].isVector = true;
      vec[byte].vector = (CMapVectorEntry *)malloc(256 * sizeof(CMapVectorEntry));
      for (j = 0; j < 256; ++j) {
	vec[byte].vector[j].isVector = false;
	vec[byte].vector[j].cid = 0;
      }
    }
    vec = vec[byte].vector;
  }
  cid = firstCID;
  for (byte = (int)(start & 0xff); byte <= (int)(end & 0xff); ++byte) {  	
    if (vec[byte].isVector) {
    	QString msg = QString(tr("Invalid CID ({0:%1} - {1:%2} [{2:%3} bytes]) in CMap")).arg(start).arg(end).arg(nBytes);
      xwApp->error(msg);
    } else {
      vec[byte].cid = cid;
    }
    ++cid;
  }
}

void XWCMap::addCodeSpace(CMapVectorEntry *vec, 
                          uint start, 
                          uint end,
		                  uint nBytes)
{
    if (nBytes > 1) 
    {
        int startByte = (start >> (8 * (nBytes - 1))) & 0xff;
        int endByte = (end >> (8 * (nBytes - 1))) & 0xff;
        uint start2 = start & ((1 << (8 * (nBytes - 1))) - 1);
        uint end2 = end & ((1 << (8 * (nBytes - 1))) - 1);
        for (int i = startByte; i <= endByte; ++i) 
        {
            if (!vec[i].isVector) 
            {
	            vec[i].isVector = true;
	            vec[i].vector = (CMapVectorEntry *)malloc(256 * sizeof(CMapVectorEntry));
	            for (int j = 0; j < 256; ++j) 
	            {
	                vec[i].vector[j].isVector = false;
	                vec[i].vector[j].cid = 0;
	            }
            }
            addCodeSpace(vec[i].vector, start2, end2, nBytes - 1);
        }
    }
}

int XWCMap::blockCount(MapDef *mtab, int c)
{
	int count = 0;
	int n  = mtab[c].len - 1;
  	c += 1;
  	for (; c < 256; c++) 
  	{
    	if (LOOKUP_CONTINUE(mtab[c].flag) || 
			!MAP_DEFINED(mtab[c].flag)     ||
			(MAP_TYPE(mtab[c].flag) != MAP_IS_CID && 
	 		MAP_TYPE(mtab[c].flag) != MAP_IS_CODE) || 
			mtab[c-1].len != mtab[c].len)
      		break;
    	else if (!memcmp(mtab[c-1].code, mtab[c].code, n) && 
	     	mtab[c-1].code[n] < 255 &&
	     	mtab[c-1].code[n] + 1 == mtab[c].code[n])
      		count++;
    	else 
      		break;
  	}

  	return count;
}

int XWCMap::bytesConsumed(const uchar *instr, long inbytes)
{
	Cmap1 * map1 = (Cmap1*)map;
	int i = 0;
	int longest = 0;
	int bytesconsumed = 0;
	for (; i < map1->codeSpace.num; i++) 
	{
    	RangeDef *csr = map1->codeSpace.ranges + i;
    	int pos = 0;
    	for (; pos < qMin(csr->dim, (int)inbytes); pos++) 
    	{
      		if (instr[pos] > csr->codeHi[pos] || instr[pos] < csr->codeLo[pos])
				break;
    	}
    	
    	if (pos == csr->dim)
      		return csr->dim;
      		
    	if (pos > longest)
      		longest = pos;
  	}

  	if (i == map1->codeSpace.num)
    	bytesconsumed = map1->profile.minBytesIn;
  	else 
  	{
    	bytesconsumed = map1->profile.maxBytesIn;
    	for (i = 0; i < map1->codeSpace.num; i++) 
    	{
      		RangeDef *csr = map1->codeSpace.ranges + i;
      		if (csr->dim > longest && csr->dim < bytesconsumed)
				bytesconsumed = csr->dim;
    	}
  	}

  	return bytesconsumed;
}

int XWCMap::checkRange(const uchar *srclo, 
	                   const uchar *srchi, 
	                   int srcdim,
	     		       const uchar *dst, 
	     		       int dstdim)
{
	if ((srcdim < 1 || dstdim < 1) ||
      	(!srclo || !srchi || !dst) ||
      	memcmp(srclo, srchi, srcdim - 1) || 
      	srclo[srcdim-1] > srchi[srcdim-1]) 
    {
    	xwApp->warning(tr("invalid CMap mapping entry. (ignored)\n"));
    	return -1;
  	}

  	if (matchCodeSpace(srclo, srcdim) < 0 || matchCodeSpace(srchi, srcdim) < 0) 
  	{
    	xwApp->warning(tr("invalid CMap mapping entry. (ignored)\n"));
    	return -1;
  	}

	Cmap1 * map1 = (Cmap1*)map;
  	if (srcdim < map1->profile.minBytesIn)
    	map1->profile.minBytesIn  = srcdim;
    	
  	if (srcdim > map1->profile.maxBytesIn)
    	map1->profile.maxBytesIn  = srcdim;
    	
  	if (dstdim < map1->profile.minBytesOut)
    	map1->profile.minBytesOut = dstdim;
    	
  	if (dstdim > map1->profile.maxBytesOut)
    	map1->profile.maxBytesOut = dstdim;

  	return 0;
}

void XWCMap::copyVector(CMapVectorEntry *dest, CMapVectorEntry *src)
{
    for (int i = 0; i < 256; ++i) 
    {
        if (src[i].isVector) 
        {
            if (!dest[i].isVector) 
            {
	            dest[i].isVector = true;
	            dest[i].vector = (CMapVectorEntry *)malloc(256 * sizeof(CMapVectorEntry));
	            for (int j = 0; j < 256; ++j) 
	            {
	                dest[i].vector[j].isVector = false;
	                dest[i].vector[j].cid = 0;
	            }
            }
            copyVector(dest[i].vector, src[i].vector);
        } 
        else 
        {
            if (!dest[i].isVector) 
	            dest[i].cid = src[i].cid;
        }
    }
}

int XWCMap::doBFChar(XWCMapFileReader *input, int count)
{
  	while (count-- > 0)
  	{
  		if (input->read(TOKEN_LEN_MAX*2) < 0)
      		return -1;
      		
      	int type1 = input->getToken();
      	char * tok1 = input->getSV();
    	if (tok1 == 0)
      		return -1;
      		
      	tok1 = qstrdup(tok1);
      	int len1 = input->getLen();
      		
      	int type2 = input->getToken();
      	char * tok2 = input->getSV(); 	
    	if (tok2 == 0) 
    	{
      		delete [] tok1;
      		return -1;
    	}
    	
    	int len2 = input->getLen();
    	    	
    	if (type1 == PFB_TYPE_STRING && type2 == PFB_TYPE_STRING) 
      		addBFChar((uchar *)tok1, len1, (uchar *)tok2, len2);
    	else if (type2 == PFB_TYPE_NAME)
    	{
    		delete [] tok1;
      		xwApp->error(tr("mapping to charName not supported.\n"));
      		return -1;
      	}
    	else
      		xwApp->warning(tr("invalid CMap mapping record. (ignored)\n"));
      		
    	delete [] tok1;
  	}
  	
  	return input->checkNextToken("endbfchar");
}

int XWCMap::doBFRange(XWCMapFileReader *input, int count)
{
  	unsigned char   codeLo[TOKEN_LEN_MAX], codeHi[TOKEN_LEN_MAX];
  	int      srcdim;
  	
  	while (count-- > 0)
  	{
  		if (input->read(TOKEN_LEN_MAX*3) < 0)
      		return -1;
      		
    	if (input->getCodeRange(codeLo, codeHi, &srcdim, TOKEN_LEN_MAX) < 0)
      		return -1;
      		
      	int typeA = input->getToken();
      	char * tok = input->getSV();
      	if (tok == 0)
      		return -1;
      		
      	int lenA = input->getLen();
      	if (typeA == PFB_TYPE_STRING)
      		addBFRange(codeLo, codeHi, srcdim, (uchar *)tok, lenA);
      	else if (typeA == PFB_TYPE_MARK)
      	{
      		if (handleCodeArray(input, codeLo, srcdim, codeHi[srcdim-1] - codeLo[srcdim-1] + 1) < 0) 
				return -1;
      	}
      	else
      		xwApp->warning(tr("invalid CMap mapping record. (ignored)\n"));
  	}
  	
  	return input->checkNextToken("endbfrange");
}

int XWCMap::doCIDChar(XWCMapFileReader *input, int count)
{
  	while (count-- > 0)
  	{
  		if (input->read(TOKEN_LEN_MAX*2) < 0)
      		return -1;
      		   		
    	int type1 = input->getToken();
      	char * tok1 = input->getSV();
    	if (tok1 == 0)
      		return -1;
      		
      	tok1 = qstrdup(tok1);
      	int len1 = input->getLen();
      		
      	int type2 = input->getToken();
    	if (type1 == PFB_TYPE_STRING && type2 == PFB_TYPE_INTEGER) 
    	{
      		long dstCID = input->getIV();
      		if (dstCID >= 0 && dstCID <= CID_MAX)
				addCIDChar((uchar*)tok1, len1, (CID)dstCID);
    	} 
    	else
      		xwApp->warning(tr("invalid CMap mapping record. (ignored)\n"));
    
    	delete [] tok1;
  	}
  	
  	return input->checkNextToken("endcidchar");
}

int XWCMap::doCIDRange(XWCMapFileReader *input, int count)
{
  	unsigned char   codeLo[TOKEN_LEN_MAX], codeHi[TOKEN_LEN_MAX];
  	int      dim;
  	
  	while (count-- > 0)
  	{
  		if (input->read(TOKEN_LEN_MAX*3) < 0)
    		return -1;
    		
    	if (input->getCodeRange(codeLo, codeHi, &dim, TOKEN_LEN_MAX) < 0)
      		return -1;
      		
      	int typeA = input->getToken();
    	if (typeA == PFB_TYPE_INTEGER) 
    	{
      		long dstCID = input->getIV();
      		if (dstCID >= 0 && dstCID <= CID_MAX)
				addCIDRange(codeLo, codeHi, dim, (CID) dstCID);
    	} 
    	else
      		xwApp->warning(tr("invalid CMap mapping record. (ignored)\n"));
  	}
  	
  	return input->checkNextToken("endcidrange");
}

int XWCMap::doCIDSystemInfo(XWCMapFileReader *input)
{
	CIDSysInfo csi = {0, 0, -1};
  	int        simpledict = 0;
  	int        error = 0;
  	input->read(TOKEN_LEN_MAX*2);
  	int type1 = PFB_TYPE_END;
  	char * tok1 = 0;
  	while ((type1 = input->getToken()) != PFB_TYPE_END)
  	{
  		tok1 = input->getSV();
  		if (type1 == PFB_TYPE_MARK)
  		{
  			simpledict = 1;
      		break;
  		}
  		else if (input->matchOp("begin")) 
  		{
      		simpledict = 0;
      		break;
    	}
  	}
  	
  	int type2 = PFB_TYPE_END;
  	char * tok2 = 0;
  	while (!error && ((type1 = input->getToken()) != PFB_TYPE_END))
  	{
  		tok1 = input->getSV();
  		if (input->matchOp(">>") && simpledict) 
      		break;
    	else if (input->matchOp("end") && !simpledict) 
      		break;
    	else if (input->matchName("Registry") && ((type2 = input->getToken()) != PFB_TYPE_END))
    	{
    		tok2 = input->getSV();
    		if (type2 != PFB_TYPE_STRING)
        		error = -1;
      		else if (!simpledict && input->checkNextToken("def"))
        		error = -1;
      		if (!error)
        		csi.registry = qstrdup(tok2);
    	}
    	else if (input->matchName("Ordering") && ((type2 = input->getToken()) != PFB_TYPE_END))
        {
    		tok2 = input->getSV();
        	if (type2 != PFB_TYPE_STRING)
        		error = -1;
      		else if (!simpledict && input->checkNextToken("def"))
        		error = -1;
      		if (!error)
        		csi.ordering = qstrdup(tok2);
        }
        else if (input->matchName("Supplement") && ((type2 = input->getToken()) != PFB_TYPE_END))
		{
			if (type2 != PFB_TYPE_INTEGER)
        		error = -1;
      		else if (!simpledict && input->checkNextToken("def"))
        		error = -1;
      		if (!error)
        		csi.supplement = input->getIV();
		}
  	}
  	
  	if (!error && input->checkNextToken("def"))
    	error = -1;

  	if (!error && csi.registry && csi.ordering && csi.supplement >= 0) 
    	setCIDSysInfo(&csi);

  	if (csi.registry)
    	delete [] csi.registry;
    	
  	if (csi.ordering)
    	delete [] csi.ordering;

  	return  error;
}

int XWCMap::doCodeSpaceRange(XWCMapFileReader *input, int count)
{
	unsigned char codeLo[TOKEN_LEN_MAX], codeHi[TOKEN_LEN_MAX];
  	int dim = 0;

  	while (count-- > 0) 
  	{ 
    	if (input->getCodeRange(codeLo, codeHi, &dim, TOKEN_LEN_MAX) < 0)
      		return -1;
    	addCodeSpaceRange(codeLo, codeHi, dim);
  	}

  	return input->checkNextToken("endcodespacerange");
}

int XWCMap::doNotDefChar(XWCMapFileReader *input, int count)
{
  	while (count-- > 0)
  	{
  		if (input->read(TOKEN_LEN_MAX*2) < 0)
      		return -1;
      		     		
    	int type1 = input->getToken();
      	char * tok1 = input->getSV();
    	if (tok1 == 0)
      		return -1;
      		
      	tok1 = qstrdup(tok1);
      	int len1 = input->getLen();
      		
      	int type2 = input->getToken();
    	if (type1 == PFB_TYPE_STRING && type2 == PFB_TYPE_INTEGER) 
    	{
      		long dstCID = input->getIV();
      		if (dstCID >= 0 && dstCID <= CID_MAX)
				addNotDefChar((uchar*)tok1, len1, (CID) dstCID);
    	} 
    	else
      		xwApp->warning(tr("invalid CMap mapping record. (ignored)\n"));
		
		delete [] tok1;
  	}
  	
  	return input->checkNextToken("endnotdefchar");
}

int XWCMap::doNotDefRange(XWCMapFileReader *input, int count)
{
  	unsigned char   codeLo[TOKEN_LEN_MAX], codeHi[TOKEN_LEN_MAX];
  	int      dim = 0;  	
  	while (count-- > 0)
  	{
  		if (input->read(TOKEN_LEN_MAX*3) < 0)
    		return -1;
    		
    	if (input->getCodeRange(codeLo, codeHi, &dim, TOKEN_LEN_MAX) < 0)
  			return -1;
  			
      	int typeA = input->getToken();
  		if (typeA == PFB_TYPE_INTEGER)
  		{
  			long dstCID = input->getIV();
  			if (dstCID >= 0 && dstCID <= CID_MAX)
				addNotDefRange(codeLo, codeHi, dim, (CID) dstCID);
  		}
  		else
  			xwApp->warning(tr("invalid CMap mapping record. (ignored)\n"));
  	}
  	
  	return input->checkNextToken("endnotdefrange");
}

void XWCMap::freeCMapVector(CMapVectorEntry *vec)
{
    for (int i = 0; i < 256; ++i) 
    {
        if (vec[i].isVector) 
            freeCMapVector(vec[i].vector);
    }
    free(vec);
}

uchar * XWCMap::getMem(int size)
{
	Cmap1 * map1 = (Cmap1*)map;
	MapData * data = map1->mapData;
  	if (data->pos + size >= MEM_ALLOC_SIZE) 
  	{
    	MapData *prev = data;
    	data = (MapData*)malloc(sizeof(MapData)); 
    	data->data = (uchar*)malloc(4096 * sizeof(uchar));
    	data->prev = prev;
    	data->pos  = 0;
    	map1->mapData = data;
  	}
  	uchar * p = data->data + data->pos;
  	data->pos += size;

  	return p;
}

int  XWCMap::handleCodeArray(XWCMapFileReader *input, 
	                         uchar *codeLo, 
	                         int dim, 
	                         int count)
{
	if (dim < 1)
	{
    	xwApp->error(tr("invalid code range.\n"));
    	return -1;
    }
    
    while (count-- > 0)
    {
      	int typeA = input->getToken();
      	char * tok = input->getSV();
    	int lenA = input->getLen();
    	if (typeA == PFB_TYPE_STRING)
    		addBFChar(codeLo, dim, (uchar *)tok, lenA);
    	else if (typeA == PFB_TYPE_MARK || typeA != PFB_TYPE_NAME)
    	{
    		xwApp->error(tr("invalid CMap mapping record.\n"));
    		return -1;
    	}
    	else
    	{
    		xwApp->error(tr("mapping to charName not supported.\n"));
    		return -1;
    	}
    	codeLo[dim-1] += 1;
    }
    
    return input->checkNextToken("]");
}

void XWCMap::handleUndefined(const uchar **inbuf,  
	                         long *inbytesleft,
		  				     uchar **outbuf, 
		  				     long *outbytesleft)
{
	if (*outbytesleft < 2)
		return ;
		
	Cmap1 * map1 = (Cmap1*)map;
	switch (map1->type) 
	{
  		case CMAP_TYPE_CODE_TO_CID:
    		memcpy(*outbuf, CID_NOTDEF_CHAR, 2);
    		break;
    		
  		case CMAP_TYPE_TO_UNICODE:
    		memcpy(*outbuf, UCS_NOTDEF_CHAR, 2);
    		break;
    		
  		default:
  			{
  				QString msg = QString(tr("cannot handle undefined mapping for this type of CMap mapping: %1. "
  				                        "<0000> is used for .notdef char.")).arg(map1->type);
				xwApp->warning(msg);
    		}
    		memset(*outbuf, 0, 2);
  	}
  	
  	*outbuf += 2;
  	*outbytesleft -= 2;

  	long len = bytesConsumed(*inbuf, *inbytesleft);

  	*inbuf  += len;
  	*inbytesleft  -= len;
}

int XWCMap::locateTbl(MapDef **cur, 
	                  const uchar *code, 
	                  int dim)
{
	for (int i = 0; i < dim-1; i++) 
	{
    	int c = code[i];
    	if (MAP_DEFINED((*cur)[c].flag)) 
    	{
      		xwApp->warning(tr("ambiguous CMap entry.\n"));
      		return -1;
    	}
    	
    	if ((*cur)[c].next == NULL)
      		(*cur)[c].next = mapDefNew();
      		
    	(*cur)[c].flag  |= MAP_LOOKUP_CONTINUE;
    	*cur = (*cur)[c].next;
  	}

  	return 0;
}

MapDef * XWCMap::mapDefNew()
{
	MapDef * t = (MapDef*)malloc(256 * sizeof(MapDef));
  	for (int c = 0; c < 256; c++) 
  	{
    	t[c].flag = (MAP_LOOKUP_END|MAP_IS_UNDEF);
    	t[c].code = 0;
    	t[c].next = 0;
  	}

  	return t;
}

void XWCMap::mapDefRelease(MapDef *t)
{
	for (int c = 0; c < 256; c++) 
	{
    	if (LOOKUP_CONTINUE(t[c].flag))
      		mapDefRelease(t[c].next);
  	}
  	
  	free(t);
}

void XWCMap::parse2(XWCMapCache *cache, int (*getCharFunc)(void *), void *data)
{
  XWPSTokenizer * pst = new XWPSTokenizer(getCharFunc, data);
  char tok1[256], tok2[256], tok3[256];
  int n1, n2, n3;
  uint start, end, code;
  pst->getToken(tok1, sizeof(tok1), &n1);
  while (pst->getToken(tok2, sizeof(tok2), &n2)) 
  {
    if (!strcmp(tok2, "usecmap")) 
    {
      if (tok1[0] == '/') 
      {
				useCMap(cache, tok1 + 1);
      }
      pst->getToken(tok1, sizeof(tok1), &n1);
    } 
    else if (!strcmp(tok1, "/WMode")) 
    {
      wMode = atoi(tok2);
      pst->getToken(tok1, sizeof(tok1), &n1);
    } 
    else if (!strcmp(tok2, "begincidchar")) 
    {
      while (pst->getToken(tok1, sizeof(tok1), &n1)) 
     	{
				if (!strcmp(tok1, "endcidchar")) 
				{
	  			break;
				}
				if (!pst->getToken(tok2, sizeof(tok2), &n2) || !strcmp(tok2, "endcidchar")) 
				{
	  			xwApp->error("Illegal entry in cidchar block in CMap");
	  			break;
				}
				if (!(tok1[0] == '<' && tok1[n1 - 1] == '>' && n1 >= 4 && (n1 & 1) == 0)) 
				{
	  			xwApp->error("Illegal entry in cidchar block in CMap");
	  			continue;
				}
				tok1[n1 - 1] = '\0';
				if (sscanf(tok1 + 1, "%x", &code) != 1) 
				{
	  			xwApp->error("Illegal entry in cidchar block in CMap");
	  			continue;
				}
				n1 = (n1 - 2) / 2;
				addCIDs(code, code, n1, (uint)atoi(tok2));
      }
      pst->getToken(tok1, sizeof(tok1), &n1);
    } 
    else if (!strcmp(tok2, "begincidrange")) 
    {
      while (pst->getToken(tok1, sizeof(tok1), &n1)) 
     	{
				if (!strcmp(tok1, "endcidrange")) 
				{
	  			break;
				}
				if (!pst->getToken(tok2, sizeof(tok2), &n2) ||
	   			  !strcmp(tok2, "endcidrange") ||
	    			!pst->getToken(tok3, sizeof(tok3), &n3) ||
	    			!strcmp(tok3, "endcidrange")) 
	    	{
	  			xwApp->error("Illegal entry in cidrange block in CMap");
	  			break;
				}
				if (tok1[0] == '<' && tok2[0] == '<' && n1 == n2 && n1 >= 4 && (n1 & 1) == 0) 
				{
	  			tok1[n1 - 1] = tok2[n1 - 1] = '\0';
	  			sscanf(tok1 + 1, "%x", &start);
	  			sscanf(tok2 + 1, "%x", &end);
	  			n1 = (n1 - 2) / 2;
	  			addCIDs(start, end, n1, (uint)atoi(tok3));
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

int XWCMap::sputx(uchar c, char **s, char *end)
{
	char hi = (c >> 4), lo = c & 0x0f;

  	if (*s + 2 > end)
    	return 0;
    	
  	**s = (hi < 10) ? hi + '0' : hi + '7';
  	*(*s+1) = (lo < 10) ? lo + '0' : lo + '7';
  	*s += 2;

  	return 2;
}

void XWCMap::useCMap(XWCMapCache *cache, XWObject *obj)
{
	XWCMap *subCMap;
	Cmap0 * map0 = (Cmap0*)(map); 
	subCMap = XWCMap::parse(cache, map0->collection, obj);
	Cmap0 * smap0 = (Cmap0*)(subCMap->map);	 
  if (!subCMap) {
    return;
  }
  isIdent = subCMap->isIdent;
  
  if (smap0->vector) {
    copyVector(map0->vector, smap0->vector);
  }
  subCMap->decRefCnt();
}

void XWCMap::useCMap(XWCMapCache *cache, char *useName)
{	
	XWCMap * subCMap = 0;
	XWString * useNameStr = new XWString(useName);
	Cmap0 * smap0 = (Cmap0*)(subCMap->map);
	Cmap0 * map0 = (Cmap0*)(map);
  if (cache) 
  {
    subCMap = cache->getCMap(map0->collection, useNameStr);
  } 
  else 
  {
  	XWFontSetting fontsetting;
    subCMap = fontsetting.getCMap(map0->collection, useNameStr);
  }
  delete useNameStr;
  if (!subCMap) 
  {
    return;
  }
  isIdent = subCMap->isIdent;
  if (smap0->vector) 
  {
    copyVector(map0->vector, smap0->vector);
  }
  subCMap->decRefCnt();
}

int XWCMap::writeMap(MapDef *mtab, 
	                 int count,
	                 uchar *codestr, 
	                 int depth,
	                 sbuf *wbuf, 
	                 QIODevice *fp)
{
#define BLOCK_LEN_MIN 2
  	struct 
  	{
    	int start, count;
  	} blocks[256/BLOCK_LEN_MIN+1];
  	
  	int num_blocks = 0;
  	int block_length = 0;
  	for (int c = 0; c < 256; c++)
  	{
  		codestr[depth] = (uchar) (c & 0xff);
  		if (LOOKUP_CONTINUE(mtab[c].flag)) 
  		{
      		MapDef *mtab1 = mtab[c].next;
      		count = writeMap(mtab1, count, codestr, depth + 1, wbuf, fp);
    	}
    	else
    	{
    		if (MAP_DEFINED(mtab[c].flag))
    		{    			
    			switch (MAP_TYPE(mtab[c].flag))
    			{
    				case MAP_IS_CID: 
    				case MAP_IS_CODE:
	  					block_length = blockCount(mtab, c);
	  					if (block_length >= BLOCK_LEN_MIN) 
	  					{
	    					blocks[num_blocks].start = c;
	    					blocks[num_blocks].count = block_length;
	    					num_blocks++;
	    					c += block_length;
	  					} 
	  					else 
	  					{
	    					*(wbuf->curptr)++ = '<';
	    					for (int i = 0; i <= depth; i++)
	      						sputx(codestr[i], &(wbuf->curptr), wbuf->limptr);
	    					*(wbuf->curptr)++ = '>';
	    					*(wbuf->curptr)++ = ' ';
	    					*(wbuf->curptr)++ = '<';
	    					for (int i = 0; i < mtab[c].len; i++)
	      						sputx(mtab[c].code[i], &(wbuf->curptr), wbuf->limptr);
	    					*(wbuf->curptr)++ = '>';
	    					*(wbuf->curptr)++ = '\n';
	    					count++;
	  					}
	  					break;
	  					
					case MAP_IS_NAME:
						return -1;
	  					break;
	  					
					case MAP_IS_NOTDEF:
	  					break;
	  					
					default:
	  					return -1;
	  					break;
    			}
    		}
    	}
    	
    	if (count >= 100 || wbuf->curptr >= wbuf->limptr )
    	{
    		char fmt_buf[32];
      		if (count > 100)
				return -1;
				
      		sprintf(fmt_buf, "%d beginbfchar\n", count);
      		fp->write(fmt_buf,  strlen(fmt_buf));
      		fp->write(wbuf->buf, (long) (wbuf->curptr - wbuf->buf));
      		wbuf->curptr = wbuf->buf;
      		fp->write("endbfchar\n", strlen("endbfchar\n"));
      		count = 0;
    	}
  	}
  	
  	if (num_blocks > 0)
  	{
  		char fmt_buf[32];
  		if (count > 0) 
  		{
      		sprintf(fmt_buf, "%d beginbfchar\n", count);
      		fp->write(fmt_buf,  strlen(fmt_buf));
      		fp->write(wbuf->buf, (long) (wbuf->curptr - wbuf->buf));
      		wbuf->curptr = wbuf->buf;
      		fp->write("endbfchar\n", strlen("endbfchar\n"));
      		count = 0;
    	}
    	
    	sprintf(fmt_buf, "%d beginbfrange\n", num_blocks);
    	fp->write(fmt_buf, strlen(fmt_buf));
    	
    	for (int i = 0; i < num_blocks; i++) 
    	{
      		int c = blocks[i].start;
      		*(wbuf->curptr)++ = '<';
      		for (int j = 0; j < depth; j++)
				sputx(codestr[j], &(wbuf->curptr), wbuf->limptr);
      		sputx(c, &(wbuf->curptr), wbuf->limptr);
      		*(wbuf->curptr)++ = '>';
      		*(wbuf->curptr)++ = ' ';
      		*(wbuf->curptr)++ = '<';
      		for (int j = 0; j < depth; j++)
				sputx(codestr[j], &(wbuf->curptr), wbuf->limptr);
      		sputx(c + blocks[i].count, &(wbuf->curptr), wbuf->limptr);
      		*(wbuf->curptr)++ = '>';
      		*(wbuf->curptr)++ = ' ';
      		*(wbuf->curptr)++ = '<';
      		for (int j = 0; j < mtab[c].len; j++)
				sputx(mtab[c].code[j], &(wbuf->curptr), wbuf->limptr);
      		*(wbuf->curptr)++ = '>';
      		*(wbuf->curptr)++ = '\n';
    	}
    	fp->write(wbuf->buf, (long) (wbuf->curptr - wbuf->buf));
    	wbuf->curptr = wbuf->buf;
    	fp->write("endbfrange\n", strlen("endbfrange\n"));
  	}
  	
  	return count;
}

#define CMAP_CACHE_ALLOC_SIZE 16u

XWCMapCache::XWCMapCache(int typeA)
	:type(typeA),
	 num(0),
	 max(0),
	 cache(0)
{
	static unsigned char range_min[2] = {0x00, 0x00};
  	static unsigned char range_max[2] = {0xff, 0xff};
  	
	if (type == 0)
	{
		num = 0;
		max = cMapCacheSize;
		cache = (XWCMap**)malloc(max * sizeof(XWCMap*));
    	for (int i = 0; i < max; ++i) 
        	cache[i] = 0;
    }
    else
    {
    	max   = CMAP_CACHE_ALLOC_SIZE;
  		cache = (XWCMap**)malloc(CMAP_CACHE_ALLOC_SIZE * sizeof(XWCMap*));
  		cache[0] = new XWCMap;
  	
  		cache[0]->setName("Identity-H");
  		cache[0]->setType(CMAP_TYPE_IDENTITY);
  		cache[0]->setWMode(0);
  		cache[0]->setCIDSysInfo(&CSI_IDENTITY);
  		cache[0]->addCodeSpaceRange(range_min, range_max, 2);

  		cache[1] = new XWCMap;
  		cache[1]->setName("Identity-V");
  		cache[1]->setType(CMAP_TYPE_IDENTITY);
  		cache[1]->setWMode(1);
  		cache[1]->setCIDSysInfo(&CSI_IDENTITY);
  		cache[1]->addCodeSpaceRange(range_min, range_max, 2);

  		num += 2;
    }
}

XWCMapCache::~XWCMapCache()
{
	int size = num;
	if (type == 0)
		size = max;
		
    for (int i = 0; i < size; ++i) 
    {
        if (cache[i]) 
            cache[i]->decRefCnt();
    }
    
    if (cache)
    	free(cache);
}

int XWCMapCache::add(XWCMap *cmap)
{
	if (!cmap || !cmap->isValid())
		return -1;
		
	int id = 0;
	for (; id < num; id++) 
	{
    	char * cmap_name0 = cmap->getName();
    	char * cmap_name1 = cache[id]->getName();
    	if (!strcmp(cmap_name0, cmap_name1)) 
      		return -1;
  	}

  	if (num >= max) 
  	{
    	max   += CMAP_CACHE_ALLOC_SIZE;
    	cache = (XWCMap**)realloc(cache, max * sizeof(XWCMap*));
  	}
  	
  	id = num;
  	num++;
  	cache[id] = cmap;

  	return id;
}

XWCMap * XWCMapCache::get(int id)
{
	if (id < 0 || id >= num)
		return 0;
		
	return cache[id];
}

XWCMap *XWCMapCache::getCMap(XWString *collection, XWString *cMapName)
{
    if (cache[0] && cache[0]->match(collection, cMapName)) 
    {
        cache[0]->incRefCnt();
        return cache[0];
    }
    
    XWCMap * cmap = 0;
    for (int i = 1; i < max; ++i) 
    {
        if (cache[i] && cache[i]->match(collection, cMapName)) 
        {
            cmap = cache[i];
            for (int j = i; j >= 1; --j) 
	            cache[j] = cache[j - 1];
            cache[0] = cmap;
            cmap->incRefCnt();
            return cmap;
        }
    }
    
    if ((cmap = XWCMap::parse(this, collection, cMapName))) 
    {
        if (cache[max - 1]) 
            cache[max - 1]->decRefCnt();
        
        for (int j = max - 1; j >= 1; --j) 
            cache[j] = cache[j - 1];
            
        cache[0] = cmap;
        cmap->incRefCnt();
        return cmap;
    }
    return 0;
}

int XWCMapCache::find(const char *cmap_name)
{
	int id = 0;
	for (; id < num; id++)
	{
		char * name = cache[id]->getName();
    	if (name && strcmp(cmap_name, name) == 0) 
      		return id;
	}
	
	XWFontSea sea;
	QString filename(cmap_name);
	QFile * fp = sea.openCMap(filename);
	if (!fp)
		return -1;
		
	if (XWCMap::parseCheckSig(fp) < 0)
	{
		fp->close();
		delete fp;
		return -1;
	}
	
	if (num >= max) 
	{
    	max   += CMAP_CACHE_ALLOC_SIZE;
    	cache = (XWCMap**)realloc(cache, max * sizeof(XWCMap*));
  	}
  	
  	id = num;  	
  	cache[id] = new XWCMap;
  	if (!(cache[id]->parse(fp, this)))
  	{
  		fp->close();
		delete fp;
		delete cache[id];
		cache[id] = 0;
		return -1;
  	}
  	
  	fp->close();
	delete fp;
	num++;
	return id;
}

