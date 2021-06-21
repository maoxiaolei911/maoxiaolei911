/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "FontEncodingTables.h"
#include "TexEncoding.h"
#include "XWEncoding.h"


#define FLAG_IS_PREDEFINED  (1 << 0)
#define FLAG_USED_BY_TYPE3  (1 << 1)

#define WBUF_SIZE 1024
static uchar wbuf[WBUF_SIZE];
static uchar range_min[1] = {0x00u};
static uchar range_max[1] = {0xFFu};

static int
is_similar_charset (char **enc_vec, char **enc_vec2)
{
  	int same = 0;
  	for (int code = 0; code < 256; code++)
  	{
  		if (enc_vec[code] == NULL && enc_vec2[code] == NULL)
  			same++;
  		else if (enc_vec[code] && enc_vec2[code])
  		{
  			if (!strcmp(enc_vec[code], enc_vec2[code]))
      			same++;
      	}
      	
      	if (same >= 64)
      		return 1;
    }

  	return 0; 
}

XWDifference::XWDifference(long codeA)
	:code(codeA),
	 count(0),
	 encs(0)
{
}

XWDifference::~XWDifference()
{
	if (encs)
		free(encs);
}

void XWDifference::add(char * encA)
{
	count++;
	encs = (char**)realloc(encs, count * sizeof(char*));
	encs[count - 1] = encA;
}

XWEncoding::XWEncoding()
	:ident(0),
	 enc_name(0),
	 flags(0),
	 baseenc(0),
	 resource(0),
	 dcount(0),
	 differences(0)
{
	memset(glyphs,  0, 256*sizeof(char *));
  	memset(is_used, 0, 256);
}

XWEncoding::~XWEncoding()
{
	if (ident)
		delete [] ident;
		
	if (enc_name)
		delete [] enc_name;
		
	for (int code = 0; code < 256; code++) 
	{
    	if (glyphs[code])
      		delete [] glyphs[code];
      		
    	glyphs[code] = 0;
  	}
  	
  	if (resource)
  		free(resource);
  		
  	if (differences)
  	{
  		for (int i = 0; i < dcount; i++)
  		{
  			if (differences[i])
  				delete differences[i];
  		}
  		
  		free(differences);
  	}
}

void XWEncoding::addUsedChars(const char *is_usedA)
{
	if (!is_usedA || isPredefined())
    	return;
    	
    for (int code = 0; code <= 0xff; code++)
    	is_used[code] |= is_usedA[code];
}

XWDifference * XWEncoding::getDifference(int i)
{
	if (i < 0 || i >= dcount)
		return 0;
		
	return differences[i];
}

bool XWEncoding::isPredefined()
{
	return (flags & FLAG_IS_PREDEFINED) ? 1 : 0;
}

void XWEncoding::makeDifferences(int pdf_ver)
{
	if (differences)
  	{
  		for (int i = 0; i < dcount; i++)
  		{
  			if (differences[i])
  				delete differences[i];
  		}
  		
  		free(differences);
  	}
  	
  	differences = 0;
  	dcount = 0;
		
	int with_base = !(flags & FLAG_USED_BY_TYPE3) || pdf_ver >= 4;
	char ** base_enc = 0;
	if (with_base &&  baseenc)
		base_enc = baseenc->glyphs;
		
	int skipping = 1;
	XWDifference * diff = 0;
	for (int code = 0; code < 256; code++)
	{
		if ((is_used && !is_used[code]) || !glyphs[code])
      		skipping = 1;
      	else if (!base_enc || 
      		     !base_enc[code] || 
      		     strcmp(base_enc[code], glyphs[code]) != 0)
      	{
      		if (skipping)
      		{
      			dcount++;
      			differences = (XWDifference**)realloc(differences, dcount * sizeof(XWDifference*));
      			diff = new XWDifference(code);
      			differences[dcount - 1] = diff;
      		}
      		
      		if (diff)
      			diff->add(glyphs[code]);
      		skipping = 0;
      	}
      	else
      		skipping = 1;
	}
}

void XWEncoding::usedByType3()
{
	flags |= FLAG_USED_BY_TYPE3;
}

XWEncodingCache::XWEncodingCache(QObject * parent)
	:QObject(parent),
	 count(0),
	 capacity(0),
	 encodings(0)
{
	newEncoding("WinAnsiEncoding", 0, FLAG_IS_PREDEFINED);
	newEncoding("MacRomanEncoding", 0, FLAG_IS_PREDEFINED);
	newEncoding("MacExpertEncoding", 0, FLAG_IS_PREDEFINED);
	newEncoding("Symbol", 0, FLAG_IS_PREDEFINED);
	newEncoding("Standard", 0, FLAG_IS_PREDEFINED);
	newEncoding("Expert", 0, FLAG_IS_PREDEFINED);
	newEncoding("ZapfDingbats", 0, FLAG_IS_PREDEFINED);
}

XWEncodingCache::~XWEncodingCache()
{
	if (encodings) 
	{
    	for (int enc_id = 0; enc_id < count; enc_id++) 
    	{
      		XWEncoding * encoding = encodings[enc_id];
      		if (encoding) 
        		delete encoding;
    	}
    	free(encodings);
  	}
}

void XWEncodingCache::addUsedChars(int encoding_id, const char *is_used)
{
	if (encoding_id < 0 || encoding_id >= count)
		return ;
		
	encodings[encoding_id]->addUsedChars(is_used);
}

int XWEncodingCache::findResource(char *enc_name)
{
	for (int enc_id = 0; enc_id < count; enc_id++) 
	{
    	XWEncoding * encoding = encodings[enc_id];
    	if (encoding->ident && !strcmp(enc_name, encoding->ident))
      		return enc_id;
    	else if (encoding->enc_name && !strcmp(enc_name, encoding->enc_name))
      		return enc_id;
  	}

  	return newEncoding(enc_name, 0, 0);
}

XWEncoding * XWEncodingCache::get(int enc_id)
{
	if (enc_id < 0 || enc_id >= count)
		return 0;
		
	return encodings[enc_id];
}

XWDifference ** XWEncodingCache::getDifferences(int enc_id,
	                                            int pdf_ver, 
	                                            int * dcountA)
{
	if (enc_id < 0 || enc_id >= count)
		return 0;
		
	XWEncoding * encoding = encodings[enc_id];
	encoding->makeDifferences(pdf_ver);
		
	if (dcountA)
		*dcountA = encoding->dcount;
		
	return encoding->differences;
}

char ** XWEncodingCache::getEncoding(int enc_id)
{
	if (enc_id < 0 || enc_id >= count)
		return 0;
		
	return encodings[enc_id]->getEncoding();
}

char * XWEncodingCache::getName(int enc_id)
{
	if (enc_id < 0 || enc_id >= count)
		return 0;
		
	return encodings[enc_id]->getName();
}

char *  XWEncodingCache::getResource(int enc_id)
{
	if (enc_id < 0 || enc_id >= count)
		return 0;
		
	return encodings[enc_id]->getResource();
}

int XWEncodingCache::isPredefined(int enc_id)
{
	if (enc_id < 0 || enc_id >= count)
		return 0;
		
	return (encodings[enc_id]->isPredefined());
}

void XWEncodingCache::usedByType3(int enc_id)
{
	if (enc_id < 0 || enc_id >= count)
		return ;
		
	encodings[enc_id]->usedByType3();
}

int XWEncodingCache::newEncoding(const char *ident,
			                     char *baseenc_name, 
			                     int flags)
{
	int enc_id = count;
	if (count++ >= capacity) 
	{
    	capacity += 16;
    	encodings = (XWEncoding**)realloc(encodings, capacity * sizeof(XWEncoding*));
  	}
  	
  	XWEncoding* enc = new XWEncoding;
  	encodings[enc_id] = enc;
  	enc->ident = qstrdup(ident);
  	QString fn(ident);
  	char ** vec = enc->glyphs;
  	readTexEnc(fn, &(enc->enc_name), vec);
  	enc->flags = flags;
  	
  	for (int code = 0; code < 256; code++)
  	{
  		if (vec[code] && !strcmp(vec[code], ".notdef"))
  		{
  			delete [] vec[code];
  			vec[code] = 0;
  		}
  	}
  	
  	if (!baseenc_name && !(flags & FLAG_IS_PREDEFINED)) 
    {
    	if (is_similar_charset(vec, winAnsiEncoding))
    		baseenc_name = "WinAnsiEncoding";
  	}
  	
  	if (baseenc_name)
  	{
  		int baseenc_id = findResource(baseenc_name);
    	if (baseenc_id >= 0)
    		enc->baseenc = encodings[baseenc_id];
  	}
  	
  	if (flags & FLAG_IS_PREDEFINED)
    	enc->resource = qstrdup(enc->enc_name);

  	return enc_id;
}

