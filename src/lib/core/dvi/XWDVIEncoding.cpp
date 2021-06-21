/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWCMap.h"
#include "XWAdobeGlyphList.h"
#include "XWEncoding.h"
#include "XWFontCache.h"
#include "XWDVIRef.h"
#include "XWDVICore.h"
#include "XWDVIEncoding.h"

#ifndef FLAG_USED_BY_TYPE3
#define FLAG_USED_BY_TYPE3  (1 << 1)
#endif

static unsigned char range_min[1] = {0x00u};
static unsigned char range_max[1] = {0xFFu};

XWDVIEncoding::XWDVIEncoding(int enc_idA)
	:enc_id(enc_idA)
{
	baseenc = 0;
	tounicode.initNull();
	resource.initNull();
}

XWDVIEncoding::~XWDVIEncoding()
{
	tounicode.free();
	resource.free();
}

void XWDVIEncoding::addUsedChars(const char *is_used)
{
	XWFontCache fcache(true);	
	fcache.encodingAddUsedChars(enc_id, is_used);
}

void XWDVIEncoding::createResource(XWDVIRef * xref)
{
	XWFontCache fcache(true);
	XWEncoding * encoding = fcache.encodingGet(enc_id);
	encoding->makeDifferences(xref->getVersion());
	int dcount = encoding->getDCount();	
	bool with_base = !(encoding->getFlags() & FLAG_USED_BY_TYPE3) || xref->getVersion() >= 4;	
	XWDVIEncoding * baseencA = with_base ? baseenc : 0;
	if (dcount > 0)
	{
		XWObject differences, obj;
		differences.initArray(xref);
		for (int i = 0; i < dcount; i++)
		{
			XWDifference * diff = encoding->getDifference(i);
			obj.initInt(diff->code);
			differences.arrayAdd(&obj);
			for (int j = 0; j < diff->count; j++)
			{
				obj.initName(diff->encs[j]);
				differences.arrayAdd(&obj);
			}
		}
		
		resource.initDict(xref);
		if (baseencA)
		{
			baseencA->resource.copy(&obj);
			resource.dictAdd(qstrdup("BaseEncoding"), &obj);
		}
		
		resource.dictAdd(qstrdup("Differences"), &differences);
		return ;
	}
	else
	{
		if (baseencA)
			baseencA->resource.copy(&resource);
	}
}

void XWDVIEncoding::createToUnicodeCMap(XWDVICore * core, XWDVIRef * xref)
{
	XWFontCache fcache(true);
	XWEncoding * encoding = fcache.encodingGet(enc_id);
	char * enc_name = encoding->getName();
	char ** enc_vec = encoding->getEncoding();
	char * is_used = encoding->getIsUsed();
	createToUnicodeCMap(core, xref, enc_name, enc_vec, is_used, &tounicode);
}

XWObject * XWDVIEncoding::createToUnicodeCMap(XWDVICore * core,
											  XWDVIRef * xref,
	                                          const char *enc_nameA,
                                              char **enc_vecA, 
                                              const char *is_usedA,
                                              XWObject * obj)
{
	char * cmap_name = new char[strlen(enc_nameA)+strlen("-UTF16")+1];
  	sprintf(cmap_name, "%s-UTF16", enc_nameA);
  	
  	XWCMap cmap;
  	cmap.setName(cmap_name);
  	cmap.setType(CMAP_TYPE_TO_UNICODE);
  	cmap.setWMode(0);
  	
  	cmap.setCIDSysInfo(&CSI_UNICODE);

  	cmap.addCodeSpaceRange(range_min, range_max, 1);

  	int all_predef = 1;
  	XWFontCache fcache(true);
  	uchar wbuf[1024];
  	for (int code = 0; code <= 0xff; code++)
  	{
  		if (is_usedA && !is_usedA[code])
      		continue;
      		
      	if (enc_vecA[code])
      	{
      		XWAdobeGlyphName *agln = fcache.aglLookupList(enc_vecA[code]);
      		if (xref->getVersion() < 5 || !agln || !agln->is_predef)
      		{
      			wbuf[0] = (code & 0xff);
        		uchar * p = wbuf + 1;
        		uchar * endptr = wbuf + 1024;
        		int    fail_count = 0;
        		int len = fcache.aglSPutUTF16BE(enc_vecA[code], &p, endptr, &fail_count);
        		if (len >= 1 && !fail_count)
        		{
        			cmap.addBFChar(wbuf, 1, wbuf + 1, len);
	  					all_predef &= agln && agln->is_predef;
	  				}
      		}
      	}
  	}
  	
  	if (all_predef)
  	{
  		obj->initNull();
  		return 0;
  	}
  	
  	XWObject * ret = core->createCMapStream(&cmap, 0, obj);
  	delete [] cmap_name;
  	return ret;
}

void XWDVIEncoding::flush(XWDVIRef * xref, bool finished)
{
	if (finished)
	{
		if (!resource.isNull())
			xref->releaseObj(&resource);
		
		if (!tounicode.isNull())
			xref->releaseObj(&tounicode);
	}
	else
	{
		XWObject obj;
		if (!resource.isNull())
		{
			resource.copy(&obj);
			xref->releaseObj(&obj);
		}
		
		if (!tounicode.isNull())
		{
			tounicode.copy(&obj);
			xref->releaseObj(&obj);
		}
	}
}

char ** XWDVIEncoding::getEncoding()
{
	XWFontCache fcache(true);	
	return fcache.getEncoding(enc_id);
}

char * XWDVIEncoding::getName()
{
	XWFontCache fcache(true);	
	return fcache.getEncodingName(enc_id);
}

bool XWDVIEncoding::isPredefined()
{
	XWFontCache fcache(true);	
	return fcache.isPredefinedEncoding(enc_id);
}

void XWDVIEncoding::usedByType3()
{
	XWFontCache fcache(true);	
	fcache.encodingUsedByType3(enc_id);
}

XWDVIEncodingCache::XWDVIEncodingCache()
	:count(0),
	 capacity(0),
	 encodings(0)
{
}

XWDVIEncodingCache::~XWDVIEncodingCache()
{
	if (encodings) 
	{
    	for (int enc_id = 0; enc_id < count; enc_id++) 
    	{
      		XWDVIEncoding * encoding = encodings[enc_id];
      		if (encoding) 
        		delete encoding;
    	}
    	free(encodings);
  	}
  	
  	QHashIterator<long, PageEncodings*> it(pageEncodings);
	while (it.hasNext())
	{
		it.next();
		PageEncodings * tmp = it.value();
		if (tmp)
			delete tmp;
	}
}

void XWDVIEncodingCache::addUsedChars(int enc_id, const char *is_used, int page_no)
{
	if (enc_id < 0 || enc_id >= count)
		return ;
		
	encodings[enc_id]->addUsedChars(is_used);
	
	PageEncodings * pencodings = 0;
	if (!pageEncodings.contains(page_no))
	{
		pencodings = new PageEncodings;
		pageEncodings[page_no] = pencodings;
	}
	else
		pencodings = pageEncodings[page_no];
		
	if (!pencodings->ids.contains(enc_id))
		pencodings->ids << enc_id;
}

void XWDVIEncodingCache::close(XWDVIRef * xref, bool finished)
{
	if (encodings) 
	{
    	for (int enc_id = 0; enc_id < count; enc_id++) 
    	{
      		XWDVIEncoding * encoding = encodings[enc_id];
        	encoding->flush(xref, finished);
        	if (finished)
        		delete encoding;
    	}
    	
    	if (finished)
    		free(encodings);
  	}
  	
  	if (finished)
  	{
  		encodings = 0;
  		count     = 0;
  		capacity  = 0;
  	}
}

void XWDVIEncodingCache::complete(XWDVICore * core, XWDVIRef * xref, bool)
{
	for (int enc_id = 0; enc_id < count; enc_id++)
	{
		XWDVIEncoding * encoding = encodings[enc_id];
		if (!encoding->isPredefined())
		{
			encoding->createResource(xref);
			encoding->createToUnicodeCMap(core, xref);
		}
	}
}

void XWDVIEncodingCache::completePage(XWDVICore * core, XWDVIRef * xref, long page_no)
{
	if (!pageEncodings.contains(page_no))
		return ;
		
	PageEncodings * pencodings = pageEncodings[page_no];
	for (int i = 0; i < pencodings->ids.size(); i++)
	{
		XWDVIEncoding * encoding = encodings[pencodings->ids.at(i)];
		if (!encoding->isPredefined())
		{
			encoding->createResource(xref);
			encoding->createToUnicodeCMap(core, xref);
		}
	}
}

int XWDVIEncodingCache::findResource(char *enc_name)
{
	XWFontCache fcache(true);
	int enc_idA = 0;
	XWEncoding * encoding = 0;
	for (; enc_idA < count; enc_idA++) 
	{
		encoding = fcache.encodingGet(encodings[enc_idA]->getID());
		char * ident = encoding->getIdent();
		char * enc_nameA = encoding->getName();
		if (ident && !strcmp(enc_name, ident))
      		return enc_idA;
    	else if (enc_nameA && !strcmp(enc_name, enc_nameA))
      		return enc_idA;
	}
	
	int enc_id = fcache.findEncoding(enc_name);
	if (enc_id < 0)
		return -1;
  	
  	XWDVIEncoding * enc = new XWDVIEncoding(enc_id);
  	encoding = fcache.encodingGet(enc_id);
  	XWEncoding * base = encoding->getBaseEnc();
  	if (base)
  	{
  		char * ident = base->getIdent();
  		if (!ident)
  			ident = base->getName();
  		
  		if (ident)
  		{
  			int i = findResource(ident);
  			if (i >= 0)
  				enc->baseenc = encodings[i];
  		}
  	}
  	
  if (fcache.isPredefinedEncoding(enc_id))
  		enc->resource.initName(enc_name);
  		
  enc_idA = count;
  if (count++ >= capacity) 
	{
   	capacity += 16;
   	encodings = (XWDVIEncoding**)realloc(encodings, capacity * sizeof(XWDVIEncoding*));
  }
  	
  encodings[enc_idA] = enc;  	
  	
  		
  return enc_idA;
}

XWDVIEncoding * XWDVIEncodingCache::get(int enc_id)
{
	if (enc_id < 0 || enc_id >= count)
		return 0;
		
	return encodings[enc_id];
}

char ** XWDVIEncodingCache::getEncoding(int enc_id)
{
	if (enc_id < 0 || enc_id >= count)
		return 0;
		
	return encodings[enc_id]->getEncoding();
}

XWObject * XWDVIEncodingCache::getEncodingObj(int enc_id)
{
	if (enc_id < 0 || enc_id >= count)
		return 0;
		
	return encodings[enc_id]->getObj();
}

char *  XWDVIEncodingCache::getName(int enc_id)
{
	if (enc_id < 0 || enc_id >= count)
		return 0;
	
	return encodings[enc_id]->getName();
}

XWObject * XWDVIEncodingCache::getTounicode(int enc_id)
{
	if (enc_id < 0 || enc_id >= count)
		return 0;
		
	return encodings[enc_id]->getTounicode();
}

int XWDVIEncodingCache::isPredefined(int enc_id)
{
	if (enc_id < 0 || enc_id >= count)
		return 0;
	
	return encodings[enc_id]->isPredefined();
}

void XWDVIEncodingCache::load(XWDVIRef * xref, long page_no)
{
	if (!pageEncodings.contains(page_no))
		return ;
		
	PageEncodings * pencodings = pageEncodings[page_no];
	for (int i = 0; i < pencodings->ids.size(); i++)
	{
		XWDVIEncoding * encoding = encodings[pencodings->ids.at(i)];
		encoding->flush(xref);
	}
}

void XWDVIEncodingCache::usedByType3(int enc_id)
{
	if (enc_id < 0 || enc_id >= count)
		return ;
		
	encodings[enc_id]->usedByType3();
}


