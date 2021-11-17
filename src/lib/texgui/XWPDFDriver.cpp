/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <QFile>
#include <QFont>
#include "XWNumberUtil.h"
#include "XWAdobeGlyphList.h"
#include "XWCMap.h"
#include "XWFontFileSFNT.h"
#include "XWFontFileCFF.h"
#include "XWFontFileFT.h"
#include "XWFontFilePK.h"
#include "XWSFNTType.h"
#include "XWCFFType.h"
#include "XWCIDFont.h"
#include "XWType0Font.h"
#include "XWTexFont.h"
#include "XWTexFontMap.h"
#include "XWFontCache.h"
#include "XWObject.h"
#include "XWPaintOutputDev.h"
#include "XWPDFOTLConf.h"
#include "XWPDFDriver.h"

#define MAX_UNICODES 16

#define PDF_REF_CATALOG_GEN   0
#define PDF_REF_PAGES_GEN     1
#define PDF_REF_PAGE_GEN      2
#define PDF_REF_CONTENTS_GEN  3
#define PDF_REF_RESOURCES_GEN 4

static unsigned char srange_min[2] = {0x00, 0x00};
static unsigned char srange_max[2] = {0xff, 0xff};
static unsigned char lrange_min[4] = {0x00, 0x00, 0x00, 0x00};
static unsigned char lrange_max[4] = {0x7f, 0xff, 0xff, 0xff};


struct PDFGent
{
  ushort gid;
  long   ucv; /* assigned PUA unicode */

  int    num_unicodes;
  long   unicodes[MAX_UNICODES];
};

class XWPDFGentEntry
{
public:
	XWPDFGentEntry(char * keyA, int keylenA, PDFGent * glyphA);
	~XWPDFGentEntry();
	
public:
	char  *key;
  	int    keylen;
  	
  	PDFGent * glyph;
  	
  	XWPDFGentEntry * next;
};

XWPDFGentEntry::XWPDFGentEntry(char * keyA, int keylenA, PDFGent * glyphA)
{
	if (keyA && keylenA > 0)
	{
		key = new char[keylenA + 1];
		memcpy(key, keyA, keylenA);
		keylen = keylenA;
	}
	else
	{
		key = 0;
		keylen = 0;
	}
	
	glyph = glyphA;
		
	next = 0;
}

XWPDFGentEntry::~XWPDFGentEntry()
{
	if (glyph)
		delete glyph;
}

#define GENT_TABLE_SIZE 503

class XWPDFGents
{
public:
	struct Iterator 
	{
  		int    index;
  		XWPDFGentEntry  * curr;
	};
	
	XWPDFGents();
	~XWPDFGents();
	
	void addGlyph(ushort gid, 
	              long ucv, 
	              int num_unicodes, 
	              long *unicodes);
	
	void createCMaps(XWCMap *cmap, 
	                 XWCMap *tounicode,
	                 unsigned char *GIDToCIDMap);
	
	
	PDFGent * findGlyph(long ucv);
	
	int handleAssign(XWObject *dst, 
	                  XWObject *src, 
	                  int flag,
	                  XWOTLGsub *gsub_list, 
	                  XWSFNTCmap *ttcmap);
	int handleGsub(XWObject *conf,
	     			XWSFNTCmap *ttcmap, 
	     			XWOTLGsub *gsub_list);
	int handleSubst(XWObject *dst_obj, 
	                 XWObject *src_obj, 
	                 int flag,
	                 XWOTLGsub *gsub_list, 
	                 XWSFNTCmap *ttcmap);
	
private:
	void appendTable(const void *key, int keylen, PDFGent *value);
	
	void clearIterator();
	
	uint getHash(const void *key, int keylen);
	char * getKey(int *keylen);
	PDFGent * getVal();
	
	void insertTable(const void *key, int keylen, PDFGent *value);
	
	XWPDFGentEntry * lookup(const void *key, int keylen);
	PDFGent * lookupTable(const void *key, int keylen);
	
	bool next();
	
	bool setIterator();
	
private:
	long   count;
	XWPDFGentEntry ** table;
	Iterator * itor;
};

XWPDFGents::XWPDFGents()
{
	table = (XWPDFGentEntry**)malloc(GENT_TABLE_SIZE * sizeof(XWPDFGentEntry*));
	for (int i = 0; i < GENT_TABLE_SIZE; i++)
		table[i] = 0;
		
	itor = 0;
}

XWPDFGents::~XWPDFGents()
{
	for (int i = 0; i < GENT_TABLE_SIZE; i++)
	{
		XWPDFGentEntry * cur = table[i];
		while (cur)
		{
			XWPDFGentEntry * tmp = cur->next;
			delete cur;
			cur = tmp;
		}
	}
	
	free(table);
	
	if (itor)
		delete itor;
}

void XWPDFGents::addGlyph(ushort gid, 
	                   long ucv, 
	                   int num_unicodes, 
	                   long *unicodes)
{
	if (gid == 0 || num_unicodes < 1) 
    	return;

	uchar wbuf[10];
  	wbuf[0] = (ucv >> 24) & 0xff;
  	wbuf[1] = (ucv >> 16) & 0xff;
  	wbuf[2] = (ucv >>  8) & 0xff;
  	wbuf[3] =  ucv & 0xff;

  	PDFGent * glyph = new PDFGent;
  	glyph->gid = gid;
  	glyph->num_unicodes = num_unicodes;
  	for (int i = 0; i < num_unicodes && i < MAX_UNICODES; i++) 
    	glyph->unicodes[i] = unicodes[i];

  	appendTable(wbuf, 4, glyph);
}

void XWPDFGents::createCMaps(XWCMap *cmap, 
	                      XWCMap *tounicode,
	                      unsigned char *GIDToCIDMap)
{
	if (!setIterator())
    	return;
    	
    int len = 0;
    ushort cid = 0;
    uchar wbuf[1024];
    do
    {
    	PDFGent  * glyph = getVal();
    	uchar * ucv   = (uchar *) getKey(&len);
    	if (GIDToCIDMap) 
      		cid = ((GIDToCIDMap[2 * glyph->gid] << 8)|GIDToCIDMap[2 * glyph->gid + 1]);
      	else
      		cid = glyph->gid;
      		
      	cmap->addCIDChar(ucv, 4, cid);
      	if (tounicode) 
      	{
      		wbuf[0] = (cid >> 8) & 0xff;
      		wbuf[1] = cid & 0xff;
      		uchar * p       = wbuf + 2;
      		uchar * endptr  = wbuf + 1024;
      		len     = 0;
      		for (int i = 0; i < glyph->num_unicodes; i++) 
				len += XWAdobeGlyphList::UC_sput_UTF16BE(glyph->unicodes[i], &p, endptr);
					
      		tounicode->addBFChar(wbuf, 2, wbuf + 2, len);
    	}
    } while(next());
    
    clearIterator();
}

PDFGent * XWPDFGents::findGlyph(long ucv)
{
	uchar wbuf[10];
	wbuf[0] = (ucv >> 24) & 0xff;
  	wbuf[1] = (ucv >> 16) & 0xff;
  	wbuf[2] = (ucv >>  8) & 0xff;
  	wbuf[3] =  ucv & 0xff;

  	return lookupTable(wbuf, 4);
}

int XWPDFGents::handleAssign(XWObject *dst, 
	                       XWObject *src, 
	                       int flag,
	                       XWOTLGsub *gsub_list, 
	                       XWSFNTCmap *ttcmap)
{
	long n_unicodes = src->arrayGetLength();
	XWObject obj;
	dst->arrayGetNF(0, &obj);
	long ucv = (long) (obj.getInt());
	if (!XWAdobeGlyphList::UC_is_valid(ucv))
	{
		if (flag == 'r')
			return -1;
			
		return 0;
	}
	
	long   unicodes[MAX_UNICODES];
	ushort gid_in[MAX_UNICODES], lig;
	for (long i = 0; i < n_unicodes; i++)
	{
		src->arrayGetNF(i, &obj);
		unicodes[i] = (long) (obj.getInt());
    	gid_in[i] = ttcmap->lookup(unicodes[i]);
    	if (gid_in[i] == 0)
    	{
    		if (flag == 'r')
				return -1;
    	}
	}
	
	int rv = gsub_list->applyLig(gid_in, n_unicodes, &lig);
	if (rv < 0) 
	{
    	if (flag == 'r')
      		return -1;
    	return 0;
  	}
  	
  	addGlyph(lig, ucv, n_unicodes, unicodes);
  	return 0;
}

int XWPDFGents::handleGsub(XWObject *conf,
	     			     XWSFNTCmap *ttcmap, 
	     			     XWOTLGsub *gsub_list)
{
	if (!conf)
    	return 0;
    	
    XWObject rule;
    XWPDFOTLConf::getRule(conf, &rule);
	if (rule.isNull())
		return 0;
		
	if (!rule.isArray()) 
	{
    	rule.free();
    	return 0;
  	}
  	
  	char * script   = XWPDFOTLConf::getScript(conf);
  	char * language = XWPDFOTLConf::getLanguage(conf);
	char * feature = 0;
  	long size = rule.arrayGetLength();
  	XWObject tmp, commands, src, dst, feat, op;
  	for (long i = 0; i < size; i += 2)
  	{
  		rule.arrayGetNF(i, &tmp);
    	int flag = tmp.getInt();
    	
    	rule.arrayGetNF(i+1, &commands);
    	long num_comms = commands.arrayGetLength();
    	for (long j = 0; j < num_comms; j += 4)
    	{
    		commands.arrayGetNF(j, &op);
      		commands.arrayGetNF(j+1, &feat);
      		if (feat.isString())
				feature = feat.getString()->getCString();
      		else
				feature = 0;
				
			commands.arrayGetNF(j+2, &dst);
      		commands.arrayGetNF(j+3, &src);
      		int rv = gsub_list->select(script, language, feature);
      		if (rv < 0)
      		{
      			if (flag == 'r')
      			{
      				op.free();
      				feat.free();
      				dst.free();
      				src.free();
      				commands.free();
      				rule.free();
      				return -1;
      			}
      		}
      		else
      		{
      			if (!strcmp(op.getName(), "assign"))
      				handleAssign(&dst, &src, flag, gsub_list, ttcmap);
      			else if (!strcmp(op.getName(), "substitute"))
      				handleSubst(&dst, &src, flag, gsub_list, ttcmap);
      		}
      		
      		op.free();
      		feat.free();
      		dst.free();
      		src.free();
    	}
    	
    	commands.free();
  	}
  	
  	rule.free();
  	return 0;
}

int XWPDFGents::handleSubst(XWObject *dst_obj, 
	                      XWObject *src_obj, 
	                      int flag,
	                      XWOTLGsub *gsub_list, 
	                      XWSFNTCmap *ttcmap)
{
	long dst_start = -1;
	long dst_end = -1; 
	long dst = 0;
  	long src_start = -1;
  	long src_end = -1; 
  	long src = 0;
  	
  	long src_size = src_obj->arrayGetLength();
  	long dst_size = dst_obj->arrayGetLength();
  	long i = 0;
  	long j = 0;
  	XWObject tmp, obj;
  	ushort gid = 0;
  	for (;i < src_size && j < dst_size; i++)
  	{
  		src_obj->arrayGetNF(i, &tmp);
    	if (tmp.isArray()) 
    	{
    		tmp.arrayGetNF(0, &obj);
      		src_start = (long)(obj.getInt());
      		tmp.arrayGetNF(1, &obj);
      		src_end   = (long)(obj.getInt());
    	} 
    	else 
      		src_start = src_end = (long)(tmp.getInt());
      		
      	tmp.free();
      	for (src = src_start; src <= src_end; src++)
      	{
      		PDFGent * glyph = findGlyph(src);
      		if (glyph)
				gid = glyph->gid;
      		else 
 				gid = ttcmap->lookup(src);
      		dst++;
      		if (dst > dst_end)
      		{
      			dst_obj->arrayGetNF(j++, &tmp);
				if (tmp.isArray()) 
				{
					tmp.arrayGetNF(0, &obj);
	  				dst_start = (long)(obj.getInt());
	  				tmp.arrayGetNF(1, &obj);
	  				dst_end   = (long)(obj.getInt());
				} 
				else 
				{
	  				dst_start = dst_end = (long)(tmp.getInt());
				}
				tmp.free();
				dst = dst_start;				
      		}
      		
      		if (gid == 0)
      			continue;
      			
      		int rv = gsub_list->apply(&gid);
      		if (rv < 0)
      		{
      			if (flag == 'r')
      				return -1;
      				
      			continue;
      		}
      			
      		if (glyph) 
				glyph->gid = gid;
      		else 
				addGlyph(gid, dst, 1, &src);
      	}
  	}
  	
  	return 0;
}

void XWPDFGents::appendTable(const void *key, int keylen, PDFGent *value)
{
	uint hkey = getHash(key, keylen);
  	XWPDFGentEntry * hent = table[hkey];
  	if (!hent) 
  	{
    	hent = new XWPDFGentEntry((char*)key, keylen, value);
    	table[hkey] = hent;
  	} 
  	else 
  	{
  		XWPDFGentEntry * last = 0;
    	while (hent) 
    	{
      		last = hent;
      		hent = hent->next;
    	}
    	
    	hent = new XWPDFGentEntry((char*)key, keylen, value);
    	last->next = hent;
  	}

  	count++;
}

void XWPDFGents::clearIterator()
{
	if (!itor)
		itor = new Iterator;
		
	itor->index = GENT_TABLE_SIZE;
    itor->curr  = 0;
}

uint XWPDFGents::getHash(const void *key, int keylen)
{
	unsigned int hkey = 0;

  	for (int i = 0; i < keylen; i++) 
    	hkey = (hkey << 5) + hkey + ((char *)key)[i];

  	return (hkey % GENT_TABLE_SIZE);
}

char * XWPDFGents::getKey(int *keylen)
{
	if (!itor)
		setIterator();
		
	XWPDFGentEntry * hent = itor->curr;
  	if (itor && hent) 
  	{
    	*keylen = hent->keylen;
    	return hent->key;
  	} 
  	
  	*keylen = 0;
    return 0;
}

PDFGent * XWPDFGents::getVal()
{
	if (!itor)
		setIterator();
		
	XWPDFGentEntry * hent = itor->curr;
  	if (itor && hent) 
    	return hent->glyph;
  	
    return 0;
}

void XWPDFGents::insertTable(const void *key, int keylen, PDFGent *value)
{
	if (!key || keylen < 1)
		return ;
		
	uint hkey = getHash(key, keylen);
  	XWPDFGentEntry * hent = table[hkey];
  	XWPDFGentEntry * prev = 0;
  	while (hent) 
  	{
    	if (hent->keylen == keylen && !memcmp(hent->key, key, keylen)) 
      		break;
    	prev = hent;
    	hent = hent->next;
  	}
  	
  	if (hent)
  		hent->glyph = value;
  	else
  	{
  		hent = new XWPDFGentEntry((char*)key, keylen, value);
  		if (prev) 
      		prev->next = hent;
    	else 
      		table[hkey] = hent;
    	count++;
  	}
}

XWPDFGentEntry * XWPDFGents::lookup(const void *key, int keylen)
{
	uint hkey = getHash(key, keylen);
	XWPDFGentEntry * hent = table[hkey];
	while (hent) 
	{
    	if (hent->keylen == keylen && !memcmp(hent->key, key, keylen)) 
      		return hent;
      		
    	hent = hent->next;
  	}

  	return 0;
}

PDFGent * XWPDFGents::lookupTable(const void *key, int keylen)
{
	XWPDFGentEntry * hent = lookup(key, keylen);
	if (hent)
		return hent->glyph;

  	return 0;
}

bool XWPDFGents::next()
{
	XWPDFGentEntry * hent = itor->curr;
	if (hent)
  		hent = hent->next;
  	while (!hent && (++(itor->index)) < GENT_TABLE_SIZE) 
    	hent = table[itor->index];
  	
  	itor->curr = hent;

  	return (hent ? true : false);
}

bool XWPDFGents::setIterator()
{
	if (!itor)
		itor = new Iterator;
		
	for (int i = 0; i < GENT_TABLE_SIZE; i++) 
	{
    	if (table[i]) 
    	{
      		itor->index = i;
      		itor->curr  = table[i];
      		return true;
    	}
  	}
  	
  	return false;
}

XWPDFDriverRef::XWPDFDriverRef(double w, double h)
:nextLabel(0),
 numObjects(5),
 curStream(0),
 objs(0),
 fontDict(0),
 xobjDict(0),
 extGSDict(0),
 colorSpaceDict(0),
 shadingDict(0),
 patternDict(0)
{
  objs = new XWObject[2000];

  XWObject obj;
  obj.initName("Catalog");
  objs[PDF_REF_CATALOG_GEN].initDict(this);
  objs[PDF_REF_CATALOG_GEN].dictAdd(qstrdup("Type"), &obj);

  obj.initName("Pages");
  objs[PDF_REF_PAGES_GEN].initDict(this);
  objs[PDF_REF_PAGES_GEN].dictAdd(qstrdup("Type"), &obj);
  obj.initRef(PDF_REF_PAGES_GEN,0);
  objs[PDF_REF_CATALOG_GEN].dictAdd(qstrdup("Pages"), &obj);

  XWObject kids;
  kids.initArray(this);
  obj.initRef(PDF_REF_PAGE_GEN,0);
  kids.arrayAdd(&obj);
  objs[PDF_REF_PAGES_GEN].dictAdd(qstrdup("Kids"), &kids);
  
  obj.initName("Page");
  objs[PDF_REF_PAGE_GEN].initDict(this);
  objs[PDF_REF_PAGE_GEN].dictAdd(qstrdup("Type"), &obj);
  obj.initRef(PDF_REF_PAGES_GEN,0);
  objs[PDF_REF_PAGE_GEN].dictAdd(qstrdup("Parent"), &obj);

  XWObject mediabox;
  mediabox.initArray(this);
  obj.initInt(round(0, 0.01));
  mediabox.arrayAdd(&obj);
  obj.initInt(round(0, 0.01));
  mediabox.arrayAdd(&obj);
  obj.initInt(round(w, 0.01));
  mediabox.arrayAdd(&obj);
  obj.initInt(round(h, 0.01));
  mediabox.arrayAdd(&obj);
  objs[PDF_REF_PAGE_GEN].dictAdd(qstrdup("MediaBox"), &mediabox);

  obj.initRef(PDF_REF_CONTENTS_GEN,0);
  objs[PDF_REF_CONTENTS_GEN].initStream(STREAM_COMPRESS, this);
  objs[PDF_REF_PAGE_GEN].dictAdd(qstrdup("Contents"), &obj);

	streams << &(objs[PDF_REF_CONTENTS_GEN]);

  objs[PDF_REF_RESOURCES_GEN].initDict(this);
  obj.initRef(PDF_REF_RESOURCES_GEN,0);
  objs[PDF_REF_PAGE_GEN].dictAdd(qstrdup("Resources"), &obj);

  obj.initDict(this);
  fontDict = obj.getDict();
  objs[PDF_REF_RESOURCES_GEN].dictAdd(qstrdup("Font"), &obj);

  obj.initDict(this);
  xobjDict = obj.getDict();
  objs[PDF_REF_RESOURCES_GEN].dictAdd(qstrdup("XObject"), &obj);

	obj.initDict(this);
  extGSDict = obj.getDict();
  objs[PDF_REF_RESOURCES_GEN].dictAdd(qstrdup("ExtGState"), &obj);

	obj.initDict(this);
  colorSpaceDict = obj.getDict();
  objs[PDF_REF_RESOURCES_GEN].dictAdd(qstrdup("ColorSpace"), &obj);

	obj.initDict(this);
  shadingDict = obj.getDict();
  objs[PDF_REF_RESOURCES_GEN].dictAdd(qstrdup("Shading"), &obj);

	obj.initDict(this);
  patternDict = obj.getDict();
  objs[PDF_REF_RESOURCES_GEN].dictAdd(qstrdup("Pattern"), &obj);

  nextLabel = PDF_REF_RESOURCES_GEN;
}

XWPDFDriverRef::~XWPDFDriverRef()
{
  if (objs)
  {
    for (int i = 0; i < numObjects;i ++)
      objs[i].free();

    delete [] objs;
    objs = 0;
  }
}

void XWPDFDriverRef::addColorSpace(XWObject * csobj, const QString &shortname, XWObject * ref)
{
	numObjects++;
	nextLabel++;
	ref->initRef(nextLabel,0);
  objs[nextLabel] = *csobj;
  QByteArray ba = shortname.toAscii();
  colorSpaceDict->add(qstrdup(ba.constData()),ref);
}

void XWPDFDriverRef::addExtGState(XWObject * egsobj, const QString &shortname, XWObject * ref)
{
	numObjects++;
	nextLabel++;
  ref->initRef(nextLabel,0);
  objs[nextLabel] = *egsobj;
  QByteArray ba = shortname.toAscii();
  extGSDict->add(qstrdup(ba.constData()),ref);
}

void XWPDFDriverRef::addFont(XWObject * font, const QString &shortname, XWObject * ref)
{
	numObjects++;
  nextLabel++;
  ref->initRef(nextLabel,0);
  objs[nextLabel] = *font;
  QByteArray ba = shortname.toAscii();
  fontDict->add(qstrdup(ba.constData()),ref);
}

void XWPDFDriverRef::addObject(XWObject * obj, XWObject * ref)
{
	numObjects++;
  nextLabel++;
  objs[nextLabel] = *obj;
  ref->initRef(nextLabel,0);
}

void XWPDFDriverRef::addPageContent(const char *buffer, unsigned length)
{
  streams[curStream]->streamAdd(buffer, length);
}

void XWPDFDriverRef::addPattern(XWObject * paobj, const QString &shortname, XWObject * ref)
{
  pushStream(paobj,ref);
	QByteArray ba = shortname.toAscii();
  patternDict->add(qstrdup(ba.constData()),ref);
}

void XWPDFDriverRef::addXObject(XWObject * xobj, const QString & shortname, XWObject * ref)
{
	pushStream(xobj,ref);
  QByteArray ba = shortname.toAscii();
  xobjDict->add(qstrdup(ba.constData()),ref);
}

XWObject * XWPDFDriverRef::fetch(int num, int , XWObject *obj, int)
{
  if (num < 0 || num >= numObjects ) 
    return obj->initNull();

  return objs[num].copy(obj);
}

XWObject * XWPDFDriverRef::getCatalog(XWObject *obj)
{
  return objs[PDF_REF_CATALOG_GEN].copy(obj);
}

bool XWPDFDriverRef::getStreamEnd(uint streamStart, uint *streamEnd)
{
  if (streamStart > 10000)
    return false;

  *streamEnd = 10000;
  return true;
}

void XWPDFDriverRef::popStream()
{
	if (curStream < 0)
	  return ;

	streams[curStream]->streamSeek(0);

	if (curStream == 0)
	  return ;

	curStream--;
}

void XWPDFDriverRef::pushStream(XWObject * obj, XWObject * ref)
{
	numObjects++;
	curStream++;
	nextLabel++;
  objs[nextLabel] = *obj;
  ref->initRef(nextLabel,0);
	streams << &(objs[nextLabel]);
}

#define PDF_FONT_TYPE1    0
#define PDF_FONT_TRUETYPE 1

#define PDF_GRAPHICS_MODE  1
#define PDF_TEXT_MODE      2
#define PDF_STRING_MODE    3

#define PDF_PRECISION_MAX  8

XWPDFDriver::XWPDFDriver(double w, double h,QObject * parent)
:XWDoc(parent),
 motion_state(PDF_GRAPHICS_MODE),
 precision(2),
 nextFont(0),
 nextForm(0),
 nextExtGS(0),
 nextShading(0),
 nextColorSpace(0),
 nextPattern(0)
{
  xref = new XWPDFDriverRef(w,h);
  catalog = new XWCatalog(this);

	otlConf = new XWPDFOTLConf((XWPDFDriverRef*)xref);
  is_mb = false;
	ref_x = 0;
	ref_y = 0;
}

XWPDFDriver::~XWPDFDriver()
{
	if (otlConf)
	{
		delete otlConf;
		otlConf = 0;
	}
}

void XWPDFDriver::addFuns(XWObject * obj, XWObject* funs,int len)
{
	XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
	XWObject a,r;
	a.initArray(xref);
	for (int i = 0; i < len; i++)
	{
		myref->addObject(&(funs[i]),&r);
		a.arrayAdd(&r);
	}

	obj->dictAdd(qstrdup("Functions"), &a);
}

void XWPDFDriver::addPageContent(const char *buffer, unsigned length)
{
	XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
	myref->addPageContent(buffer,length);
}

void XWPDFDriver::beginFading(const QString & fadname,
                         XWObject * bbox,
                         XWObject * matrix,
                         XWObject * res,
												 XWObject * ref)
{
	XWObject form;
	initXForm(fadname,&form,bbox,matrix,res,ref);
	XWObject group, obj;
  group.initDict(xref);
  obj.initName("Transparency");
  group.dictAdd(qstrdup("S"), &obj);
	obj.initName("DeviceGray");
	group.dictAdd(qstrdup("CS"), &obj);
	XWDict * xform_dict = form.streamGetDict();
	xform_dict->add(qstrdup("Group"), &group);
}

void XWPDFDriver::beginPattern(const QString & patname,
                         int ptype,
                         double llx, double lly,
                         double urx,double ury,
                         double xstep,double ystep,
                         XWObject * ref)
{
	XWObject pattern;
	initStream(&pattern);
	XWDict * dict = pattern.streamGetDict();
	XWObject tmp;
	tmp.initName("Pattern");
	dict->add(qstrdup("Type"), &tmp);
	tmp.initInt(1);
	dict->add(qstrdup("PatternType"), &tmp);
	if (ptype == 0)
	  tmp.initInt(2);
	else
	  tmp.initInt(1);

	dict->add(qstrdup("PaintType"), &tmp);
	tmp.initInt(1);
	dict->add(qstrdup("TilingType"), &tmp);
	initBBox(llx,lly,urx,ury,&tmp);
	dict->add(qstrdup("BBox"), &tmp);
	tmp.initReal(xstep);
	dict->add(qstrdup("XStep"), &tmp);
	tmp.initReal(ystep);
	dict->add(qstrdup("YStep"), &tmp);
	initDict(&tmp);
	dict->add(qstrdup("Resources"), &tmp);

	nextPattern++;
	QString shortname = QString("/P%1").arg(nextPattern);
	XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
	myref->addPattern(&pattern,shortname,ref);
	patternShortNames[patname] = shortname;
	patternRefs[patname] = ref->getRefGen();
}

void XWPDFDriver::beginTransparency(const QString & transname,
                         XWObject * bbox,
                         XWObject * matrix,
                         XWObject * res,
												 XWObject * ref)
{
	XWObject form;
	initXForm(transname,&form,bbox,matrix,res,ref);
	XWObject group, obj;
  group.initDict(xref);
  obj.initName("Transparency");
  group.dictAdd(qstrdup("S"), &obj);
	XWDict * xform_dict = form.streamGetDict();
	xform_dict->add(qstrdup("Group"), &group);
}

void XWPDFDriver::closePath()
{
  graphicsMode();
  addPageContent("h\n", 2);
}

void XWPDFDriver::curveTo(const QPointF & c1,const QPointF & c2,const QPointF & e)
{
  graphicsMode();
  
  char buf[100];
  int len = sprintCoord(buf,c1);
  buf[len++] = ' ';
  len += sprintCoord(buf,c2);
  buf[len++] = ' ';
  len = sprintCoord(buf,e);
  buf[len++] = ' ';
  buf[len++] = 'c';
  buf[len] = '\n';
  addPageContent(buf, len);
  currentPos = e;
}

void XWPDFDriver::display(QPainter * painter)
{
  XWPaintOutputDev outdev(painter);
  outdev.startDoc(getXRef());
	outdev.startPage(1, NULL);
	displayPage(&outdev, 1, 72, 72, 0, true, true, true);
}

void XWPDFDriver::display(QPainter * painter,double scale)
{
	XWPaintOutputDev outdev(painter);
  outdev.startDoc(getXRef());
	outdev.startPage(1, NULL);
	displayPage(&outdev, 1, 72 * scale, 72 * scale, 0, true, true, true);
}

void XWPDFDriver::endPath()
{
	graphicsMode();
  addPageContent("n\n", 2);
}

void XWPDFDriver::endPattern()
{
	XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
	myref->popStream();
}

void XWPDFDriver::endXForm()
{
	XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
	myref->popStream();
}

void XWPDFDriver::evenoddClip()
{
  graphicsMode();
  addPageContent("W*\n", 3);
}

void XWPDFDriver::evenoddCloseFillStroke()
{
  graphicsMode();
  addPageContent("b*\n", 3);
}

void XWPDFDriver::evenoddFill()
{
  graphicsMode();
  addPageContent("f*\n", 3);
}

void XWPDFDriver::evenoddFillStroke()
{
  graphicsMode();
  addPageContent("B*\n", 3);
}

void XWPDFDriver::fillText()
{
	stringMode();
	addPageContent("0 Tr\n", 5);
}

void XWPDFDriver::fillStrokeText()
{
	stringMode();
	addPageContent("2 Tr\n", 5);
}

void XWPDFDriver::grestore()
{
  graphicsMode();
  addPageContent("Q\n", 2);
}

void XWPDFDriver::gsave()
{
  graphicsMode();
  addPageContent("q\n", 2);
}

bool XWPDFDriver::hasColorSpace(const QString & csname)
{
	return colorSpaceShortNames.contains(csname);
}

bool XWPDFDriver::hasPattern(const QString & patname,XWObject * ref)
{
	if (patternShortNames.contains(patname))
	{
		int gen = patternRefs[patname];
		ref->initRef(gen,0);
		return true;
	}
	return false;
}

bool XWPDFDriver::hasXObject(const QString & xobjname,XWObject * ref)
{
	if (xobjShortNames.contains(xobjname))
	{
		int gen = xobjRefs[xobjname];
		ref->initRef(gen,0);
		return true;
	}
	return false;
}

XWObject * XWPDFDriver::initArray(XWObject * obj)
{
	obj->initArray(xref);
	return obj;
}

XWObject * XWPDFDriver::initBBox(double llx, double lly,
                      double urx,double ury,
                      XWObject * obj)
{
	obj->initArray(xref);
	XWObject tmp;
	tmp.initInt(round(llx, .001));
	obj->arrayAdd(&tmp);
	tmp.initInt(round(lly, .001));
	obj->arrayAdd(&tmp);
	tmp.initInt(round(urx, .001));
	obj->arrayAdd(&tmp);
	tmp.initInt(round(ury, .001));
	obj->arrayAdd(&tmp);
	return obj;
}

XWObject * XWPDFDriver::initDict(XWObject * obj)
{
	obj->initDict(xref);
	return obj;
}

XWObject * XWPDFDriver::initExtGState(double so,double fo,XWObject * obj)
{
	obj->initDict(xref);
	XWObject tmp;
	tmp.initName("ExtGState");
	obj->dictAdd(qstrdup("Type"), &tmp);
	tmp.initReal(so);
	obj->dictAdd(qstrdup("CA"), &tmp);
	tmp.initReal(fo);
	obj->dictAdd(qstrdup("ca"), &tmp);
	return obj;
}

XWObject * XWPDFDriver::initFunc2(double doma, double domb, 
	                          double * c0, int c0len, 
                            double * c1, int c1len,
														double n,
                            XWObject * obj)
{
  obj->initDict(xref);
  XWObject tmp, tmp1;
	tmp.initInt(2);
	obj->dictAdd(qstrdup("FunctionType"), &tmp);

	tmp.initArray(xref);
	tmp1.initReal(doma);
	tmp.arrayAdd(&tmp1);
	tmp1.initReal(domb);
	tmp.arrayAdd(&tmp1);
	obj->dictAdd(qstrdup("Domain"), &tmp);

	tmp.initArray(xref);
	for (int i = 0; i < c0len; i++)
	{
		tmp1.initReal(c0[i]);
		tmp.arrayAdd(&tmp1);
	}

	obj->dictAdd(qstrdup("C0"), &tmp);

	tmp.initArray(xref);
	for (int i = 0; i < c1len; i++)
	{
		tmp1.initReal(c1[i]);
		tmp.arrayAdd(&tmp1);
	}

	obj->dictAdd(qstrdup("C1"), &tmp);

	tmp.initReal(n);
	obj->dictAdd(qstrdup("N"), &tmp);

	return obj;
}

XWObject * XWPDFDriver::initFunc3(double doma, double domb, 
                       double * bounds, int blen,
                       double * enc ,int elen,
                       XWObject * obj)
{
  obj->initDict(xref);
	XWObject tmp, tmp1;
	tmp.initInt(3);
	obj->dictAdd(qstrdup("FunctionType"), &tmp);

	tmp.initArray(xref);
	tmp1.initReal(doma);
	tmp.arrayAdd(&tmp1);
	tmp1.initReal(domb);
	tmp.arrayAdd(&tmp1);
	obj->dictAdd(qstrdup("Domain"), &tmp);

	tmp.initArray(xref);
	for (int i = 0; i < blen; i++)
	{
		tmp1.initReal(bounds[i]);
		tmp.arrayAdd(&tmp1);
	}

	obj->dictAdd(qstrdup("Bounds"), &tmp);

	tmp.initArray(xref);
	for (int i = 0; i < elen; i++)
	{
		tmp1.initReal(enc[i]);
		tmp.arrayAdd(&tmp1);
	}

	obj->dictAdd(qstrdup("Encode"), &tmp);

	return obj;
}

XWObject * XWPDFDriver::initFunc4(double * dom, int dlen, 
                          double * range, int rlen,
													const char * code,int len,
                          XWObject * obj)
{
  obj->initStream(STREAM_COMPRESS,xref);
	XWDict * dict = obj->streamGetDict();
	XWObject tmp;
	tmp.initInt(4);
	dict->add(qstrdup("FunctionType"), &tmp);

	XWObject tmp1;
	tmp.initArray(xref);
	for (int i = 0; i < dlen; i++)
	{
		tmp1.initReal(dom[i]);
		tmp.arrayAdd(&tmp1);
	}

	dict->add(qstrdup("Domain"), &tmp);

	tmp.initArray(xref);
	for (int i = 0; i < rlen; i++)
	{
		tmp1.initReal(range[i]);
		tmp.arrayAdd(&tmp1);
	}

	dict->add(qstrdup("Range"), &tmp);
	obj->streamAdd(code, len);

	return obj;
}

XWObject * XWPDFDriver::initMatrix(double a,double b,double c, double d,
                        double e, double f,XWObject * obj)
{
	obj->initArray(xref);
	XWObject tmp;
	tmp.initInt(round(a, .00001));
  obj->arrayAdd(&tmp);
  tmp.initInt(round(b, .00001));
  obj->arrayAdd(&tmp);
  tmp.initInt(round(c, .00001));    	
  obj->arrayAdd(&tmp);
  tmp.initInt(round(d, .00001));
  obj->arrayAdd(&tmp);
  tmp.initInt(round(e, .001));
  obj->arrayAdd(&tmp);
  tmp.initInt(round(f, .001));
  obj->arrayAdd(&tmp);
	return obj;
}

XWObject * XWPDFDriver::initProcSet(XWObject * obj)
{
	XWObject tmp;
  obj->initArray(xref);
  tmp.initName("PDF");
  obj->arrayAdd(&tmp);
  tmp.initName("Text");
  obj->arrayAdd(&tmp);
  tmp.initName("ImageC");
  obj->arrayAdd(&tmp);
  tmp.initName("ImageB");
  obj->arrayAdd(&tmp);
  tmp.initName("ImageI");
  obj->arrayAdd(&tmp);  

	return obj;
}

XWObject * XWPDFDriver::initShading1(double * dom, int dlen, 
                          double * m, int mlen,
                          XWObject * func,
                          XWObject * obj)
{
	obj->initDict(xref);
	XWObject tmp;
	tmp.initInt(1);
	obj->dictAdd(qstrdup("ShadingType"), &tmp);
	tmp.initName("DeviceRGB");
	obj->dictAdd(qstrdup("ColorSpace"), &tmp);

	XWObject tmp1;
	tmp.initArray(xref);
	for (int i = 0; i < dlen; i++)
	{
		tmp1.initReal(dom[i]);
		tmp.arrayAdd(&tmp1);
	}

	obj->dictAdd(qstrdup("Domain"), &tmp);

	tmp.initArray(xref);
	for (int i = 0; i < mlen; i++)
	{
		tmp1.initReal(m[i]);
		tmp.arrayAdd(&tmp1);
	}

	obj->dictAdd(qstrdup("Matrix"), &tmp);

	XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
	myref->addObject(func,&tmp);
	obj->dictAdd(qstrdup("Function"), &tmp);

	return obj;
}

XWObject * XWPDFDriver::initShading23(int stype,
	                        double doma, double domb, 
                          double * coords, int clen,
                          XWObject * func,
                          XWObject * obj)
{
	obj->initDict(xref);
	XWObject tmp;
	tmp.initInt(stype);
	obj->dictAdd(qstrdup("ShadingType"), &tmp);
	tmp.initName("DeviceRGB");
	obj->dictAdd(qstrdup("ColorSpace"), &tmp);

	XWObject tmp1;
	tmp.initArray(xref);
	tmp1.initReal(doma);
	tmp.arrayAdd(&tmp1);
	tmp1.initReal(domb);
	tmp.arrayAdd(&tmp1);

	obj->dictAdd(qstrdup("Domain"), &tmp);

	tmp.initArray(xref);
	for (int i = 0; i < clen; i++)
	{
		tmp1.initReal(coords[i]);
		tmp.arrayAdd(&tmp1);
	}

	obj->dictAdd(qstrdup("Coords"), &tmp);

	XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
	myref->addObject(func,&tmp);
	obj->dictAdd(qstrdup("Function"), &tmp);

	tmp.initArray(xref);
	if (stype == 2)
	  tmp1.initString(new XWString("false", 5));
	else
	  tmp1.initString(new XWString("true", 4));
	tmp.arrayAdd(&tmp1);
	tmp1.initString(new XWString("false", 5));
	tmp.arrayAdd(&tmp1);

	obj->dictAdd(qstrdup("Extend"), &tmp);

	return obj;
}

XWObject * XWPDFDriver::initStream(XWObject * obj)
{
	obj->initStream(STREAM_COMPRESS, xref);
	return obj;
}

void XWPDFDriver::invisibleText()
{
	stringMode();
	addPageContent("3 Tr\n", 5);
}

void XWPDFDriver::lineTo(const QPointF & p)
{
  graphicsMode();
  
  char buf[100];
  int len = sprintCoord(buf,p);
  buf[len++] = ' ';
  buf[len++] = 'l';
  buf[len] = '\n';
  addPageContent(buf, len);
  currentPos = p;
}

void XWPDFDriver::lineTo(double xA,double yA)
{
	QPointF p(xA,yA);
	lineTo(p);
}

void XWPDFDriver::moveTo(const QPointF & p)
{
  graphicsMode();
  
  char buf[100];
  int len = sprintCoord(buf,p);
  buf[len++] = ' ';
  buf[len++] = 'm';
  buf[len] = '\n';
  addPageContent(buf, len);
  currentPos = p;
}

void XWPDFDriver::moveTo(double xA,double yA)
{
	QPointF p(xA,yA);
	moveTo(p);
}

void XWPDFDriver::newPath()
{
  graphicsMode();
  addPageContent("n\n", 2);
}

void XWPDFDriver::nonzeroClip()
{
  graphicsMode();
  addPageContent("W\n", 2);
}

void XWPDFDriver::nonzeroCloseFillStroke()
{
  graphicsMode();
  addPageContent("b\n", 2);
}

void XWPDFDriver::nonzeroFill()
{
  graphicsMode();
  addPageContent("f\n", 2);
}

void XWPDFDriver::nonzeroFillStroke()
{
  graphicsMode();
  addPageContent("B\n", 2);
}

void XWPDFDriver::setChar(const QChar & c)
{
	stringMode();
	char buf[1000];
	int len = 0;
	buf[len++] = '[';
	if (is_mb)
  {
		buf[len++] = '<';
		buf[len++] = '0';
		buf[len++] = '0';
    int ch = c.unicode();
    int first  = (((ch >> 8) & 0x00ff) >> 4) & 0x000f;
    int second = (ch & 0x00ff) & 0x000f;
    buf[len++] = ((first >= 10)  ? first  + 'W' : first  + '0');
    buf[len++] = ((second >= 10)  ? second  + 'W' : second  + '0');
    buf[len++] = '>';
  }
	else
	{
		buf[len++] = '(';
	  int ch = (char)(c.unicode() & 0x00ff);
	
	  switch (ch)
	  {
		  case '(':
			  buf[len++] = '\\';
			  buf[len++] = '(';
			  break;
					
		  case ')':
		    buf[len++] = '\\';
		    buf[len++] = ')';
		    break;
					
 		  case '\\':
		    buf[len++] = '\\';
		    buf[len++] = '\\';
  	    break;
					
   	  default:
	      buf[len++] = ch;
		    break;
	  }
	  buf[len++] = (char)(c.unicode() & 0x00ff);
	  buf[len++] = ')';
	}
	
	buf[len++] = ']';
	buf[len++] = 'T';
	buf[len++] = 'J';
	buf[len++] = '\n';
	addPageContent(buf, len);
}

void XWPDFDriver::setChar(double xpos,double ypos, const QChar & c)
{
	double deltaX = xpos - ref_x;
	double deltaY = ypos - ref_y;
	ref_x = xpos;
	ref_y = ypos;
	stringMode();
  char buf[1000];
  int len = sprintLength(buf,deltaX);
  buf[len++] = ' ';
  len += sprintLength(buf,deltaY);
  buf[len++] = ' ';
  buf[len++] = 'T';
  buf[len++] = 'd';
  buf[len++] = '\n';
	buf[len++] = '[';
	buf[len++] = '(';
	int ch = (char)(c.unicode() & 0x00ff);
	switch (ch)
	{
		case '(':
			buf[len++] = '\\';
			buf[len++] = '(';
			break;
					
		case ')':
		  buf[len++] = '\\';
		  buf[len++] = ')';
		  break;
					
 		case '\\':
		  buf[len++] = '\\';
		  buf[len++] = '\\';
  	  break;
					
   	default:
	    buf[len++] = ch;
		  break;
	}
	buf[len++] = (char)(c.unicode() & 0x00ff);
	buf[len++] = ')';
	buf[len++] = ']';
	buf[len++] = 'T';
	buf[len++] = 'J';
	buf[len++] = '\n';
	addPageContent(buf, len);
}

void XWPDFDriver::setColorSpace(const QString & csname, XWObject * obj)
{
	nextColorSpace++;
	QString shortname = QString("/CS%1").arg(nextColorSpace);
	colorSpaceShortNames[csname] = shortname;
	XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
	XWObject ref;
	myref->addColorSpace(obj,shortname,&ref);
}

void XWPDFDriver::setExtGState(const QString & gsname, XWObject * obj)
{
	nextExtGS++;
	QString shortname = QString("/GS%1").arg(nextExtGS);
	extGSShortNames[gsname] = shortname;
	XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
	XWObject ref;
	myref->addExtGState(obj,shortname,&ref);
}

void XWPDFDriver::setFillColor(const QColor & rgb)
{
  graphicsMode();

  char buf[100];
  int len = sprintf(buf, "%g %g %g rg\n", round(rgb.redF(), 0.001),
                    round(rgb.greenF(), 0.001),round(rgb.blueF(), 0.001));
  addPageContent(buf, len);
}

void XWPDFDriver::setFillColorSpace(const QString & csname)
{
	QString shortname = colorSpaceShortNames[csname];
	QByteArray ba = shortname.toAscii();

  char buf[20];
  int len = sprintf(buf, "%s cs\n", ba.constData());
	addPageContent(buf, len);
}

void XWPDFDriver::setFillPattern(const QString & patname)
{
	QString shortname = patternShortNames[patname];
	QByteArray ba = shortname.toAscii();

  char buf[20];
  int len = sprintf(buf, "%s scn\n", ba.constData());
	addPageContent(buf, len);
}

void XWPDFDriver::setFont(const QString & fontname,double size, bool sys)
{
	stringMode();
  if (fontname != curFont)
  {
    is_mb = sys;
    curFont = fontname;
    QString shortname;
    if (fontType.contains(fontname))
      shortname = fontShortNames[fontname];
    else
    {
      fontSize[fontname] = size;
      if (sys)
        fontType[fontname] = PDF_FONT_TRUETYPE;
      else
        fontType[fontname] = PDF_FONT_TYPE1;
      
      nextFont++;
      shortname = QString("/F%1").arg(nextFont);
      fontShortNames[fontname] = shortname;
    }

    QByteArray ba = shortname.toAscii();

    char buf[100];
    int len = sprintf(buf, "%s", ba.constData());
    buf[len++] = ' ';
    len += doubleToAscii(size, qMin(precision+1, PDF_PRECISION_MAX), buf+len);
    buf[len++] = ' ';
  	buf[len++] = 'T';
  	buf[len++] = 'f';
    buf[len++] = '\n';
    addPageContent(buf, len);
  }
}

void XWPDFDriver::setLineCap(int capstyle)
{
  graphicsMode();
  char buf[20];
  int len = sprintf(buf,"%d J\n",capstyle);
  addPageContent(buf, len);
}

void XWPDFDriver::setLineDash(const QVector<qreal> & pattern,double offset)
{
  graphicsMode();
  int len = 0;
  char buf[200];
  buf[len++] = '[';
  for (int i = 0; i < pattern.size(); i++) 
  {
    len += sprintLength(buf + len, pattern[i]);
    buf[len++] = ' ';
  }

  buf[len - 1] = ']';
  buf[len++] = ' ';
	len += sprintLength(buf + len, offset);
  buf[len++] = 'd';
  buf[len++] = '\n';
  addPageContent(buf, len);
}

void XWPDFDriver::setLineJoin(int joinstyle)
{
  graphicsMode();
  char buf[20];
  int len = sprintf(buf,"%d j\n",joinstyle);
  addPageContent(buf, len);
}

void XWPDFDriver::setLineWidth(double width)
{
  graphicsMode();
  char buf[20];
  int len = sprintLength(buf,width);
  buf[len++] = ' ';
  buf[len++] = 'w';
  buf[len++] = '\n';
  addPageContent(buf, len);
}

void XWPDFDriver::setMatrix(double a,double b,double c,double d,double e,double f)
{
	graphicsMode();

	char buf[100];
	int  prec2 = qMin(precision + 2, PDF_PRECISION_MAX);
  int  prec0 = qMax(precision, 2);
  	
  int len  = doubleToAscii(a, prec2, buf);
  buf[len++] = ' ';
  len += doubleToAscii(b, prec2, buf+len);
  buf[len++] = ' ';
  len += doubleToAscii(c, prec2, buf+len);
  buf[len++] = ' ';
  len += doubleToAscii(d, prec2, buf+len);
  buf[len++] = ' ';
  len += doubleToAscii(e, prec0, buf+len);
  buf[len++] = ' ';
  len += doubleToAscii(f, prec0, buf+len);
	buf[len++] = ' ';
	buf[len++] = 'c';
	buf[len++] = 'm';
	buf[len++] = '\n';
	addPageContent(buf, len);
}

void XWPDFDriver::setMiterLimit(double mlimit)
{
  graphicsMode();
  char buf[20];
  int len = sprintLength(buf,mlimit);
  buf[len++] = ' ';
  buf[len++] = 'M';
  buf[len++] = '\n';
  addPageContent(buf, len);
}

void XWPDFDriver::setString(double xpos,double ypos, const QString & str)
{
	double deltaX = xpos - ref_x;
	double deltaY = ypos - ref_y;
	ref_x = xpos;
	ref_y = ypos;

	stringMode();
  char buf[1000];
  int len = sprintLength(buf,deltaX);
  buf[len++] = ' ';
  len += sprintLength(buf,deltaY);
  buf[len++] = ' ';
  buf[len++] = 'T';
  buf[len++] = 'd';
  buf[len++] = '\n';
	buf[len++] = '[';

  if (is_mb)
  {
		buf[len++] = '<';
    for (int i = 0; i < str.length(); i++)
    {
			buf[len + i*4] = '0';
			buf[len + i*4+1] = '0';
      int ch = str[i].unicode();
      int first  = (((ch >> 8) & 0x00ff) >> 4) & 0x000f;
      int second = (ch & 0x00ff) & 0x000f;
      buf[len + i*4+2] = ((first >= 10)  ? first  + 'W' : first  + '0');
      buf[len + i*4+3] = ((second >= 10)  ? second  + 'W' : second  + '0');
    }

		len += str.length() * 4;

    buf[len++] = '>';
  }
  else
  {
		buf[len++] = '(';
    for (int i = 0; i < str.length(); i++)
    {
      int ch = str[i].unicode() & 0x00ff;
      if (ch < 32 || ch > 126)
      {
        buf[len++] = '\\';
			  len += sprintf(buf+len, "%03o", ch);
      }
      else
      {
        switch (ch)
        {
          case '(':
					  buf[len++] = '\\';
					  buf[len++] = '(';
					break;
					
      		case ')':
					  buf[len++] = '\\';
					  buf[len++] = ')';
					  break;
					
      		case '\\':
					  buf[len++] = '\\';
					  buf[len++] = '\\';
					  break;
					
      		default:
					  buf[len++] = ch;
					  break;
        }
      }
    }

		buf[len++] = ')';
  }

	buf[len++] = ']';
	buf[len++] = 'T';
	buf[len++] = 'J';
	buf[len++] = '\n';

  addPageContent(buf, len);
}

void XWPDFDriver::setStrokeColor(const QColor & rgb)
{
  graphicsMode();

  char buf[100];
  int len = sprintf(buf, "%g %g %g RG\n", round(rgb.redF(), 0.001),
                    round(rgb.greenF(), 0.001),round(rgb.blueF(), 0.001));
  addPageContent(buf, len);
}

void XWPDFDriver::setStrokColorSpace(const QString & csname)
{
	QString shortname = colorSpaceShortNames[csname];
	QByteArray ba = shortname.toAscii();

  char buf[20];
  int len = sprintf(buf, "%s CS\n", ba.constData());
	addPageContent(buf, len);
}

void XWPDFDriver::setStrokePattern(const QString & patname)
{
	QString shortname = patternShortNames[patname];
	QByteArray ba = shortname.toAscii();

  char buf[20];
  int len = sprintf(buf, "%s SCN\n", ba.constData());
	addPageContent(buf, len);
}

void XWPDFDriver::setTextMatrix(double a,double b,double c,double d,double e,double f)
{
	stringMode();

	char buf[100];
	int  prec2 = qMin(precision + 2, PDF_PRECISION_MAX);
  int  prec0 = qMax(precision, 2);
  	
  int len  = doubleToAscii(a, prec2, buf);
  buf[len++] = ' ';
  len += doubleToAscii(b, prec2, buf+len);
  buf[len++] = ' ';
  len += doubleToAscii(c, prec2, buf+len);
  buf[len++] = ' ';
  len += doubleToAscii(d, prec2, buf+len);
  buf[len++] = ' ';
  len += doubleToAscii(e, prec0, buf+len);
  buf[len++] = ' ';
  len += doubleToAscii(f, prec0, buf+len);
	buf[len++] = ' ';
	buf[len++] = 'T';
	buf[len++] = 'm';
	buf[len++] = '\n';
	addPageContent(buf, len);
}

void XWPDFDriver::shadingFunc(const QString & shadingname,
                   const QPointF & ll,
                   const QPointF & ur,
                   const char * code,int len,
									 XWObject * ref)
{
  double w = ur.x() - ll.x();
  double h = ur.y() - ll.y();
  XWObject bbox;
  initBBox(0,0,w,h,&bbox);

  double dom[4];
  dom[0] = ll.x();
  dom[1] = ur.x();
  dom[2] = ll.y();
  dom[3] = ur.y();

  double range[6];
  range[0] = 0;
  range[1] = 1;
  range[2] = 0;
  range[3] = 1;
  range[4] = 0;
  range[5] = 1;
  XWObject func;
  initFunc4(dom,4,range,6,code,len,&func);

  double m[6];
  m[0] = 1;
  m[1] = 0;
  m[2] = 0;
  m[3] = 1;
  m[4] = -ll.x();
  m[5] = -ll.y();

  XWObject sh;
  initShading1(dom,4,m,6,&func,&sh);

  XWObject shading;
  initDict(&shading);
  shading.dictAdd(qstrdup("Sh"), &sh);

  XWObject procset;
  initProcSet(&procset);

  XWObject res;
  initDict(&res);
  res.dictAdd(qstrdup("ProcSet"), &procset);
  res.dictAdd(qstrdup("Shading"), &shading);

  XWObject form;
	initXForm(shadingname,&form,&bbox,0,&res,ref);
  addPageContent("/Sh sh\n",7);
  endXForm();
}

void XWPDFDriver::shadingHoriVert(const QString & shadingname,
                   bool hori,
                   double h,
                   double funs[][4],int nfuns,
									 XWObject * ref)
{
  double doma = funs[0][0];
  double domb = funs[nfuns - 1][0];
  double w = domb;
  XWObject bbox, func;
  if (hori)
    initBBox(0,0,w,h,&bbox);
  else
    initBBox(0,0,h,w,&bbox);
  
  if (nfuns == 2)
  {
    double *c0 = &(funs[0][1]);
    double *c1 = &(funs[1][1]);
    initFunc2(doma,domb,c0,3,c1,3,1,&func);
  }
  else
  {
    double bounds[100];
    double enc[100];
    enc[0] = 0;
    enc[1] = 1;
    int n = 2;
    int k = 0;
    for (int i = 1; i < (nfuns - 1); i++)
    {
      bounds[k++] = funs[i][1];
      enc[n++] = 1;
    }

    XWObject objs[100];
    for (int i = 0; i < (nfuns - 1); i++)
    {
      double a = funs[i][0];
      double b = funs[i+1][0];
      double * c0 = &(funs[i][1]);
      double * c1 = &(funs[i+1][1]);
      initFunc2(a,b,c0,3,c1,3,1,&(objs[i]));
    }
    
    XWObject func3;
    initFunc3(doma,domb,bounds,k,enc,n,&func);
    addFuns(&func,objs,nfuns - 1);
  }

  double coords[4];
  coords[0] = doma;
  coords[1] = 0;
  coords[2] = domb;
  coords[3] = 0;

  XWObject sh;
  initShading23(2,doma,domb,coords,4,&func,&sh);

  XWObject shading;
  initDict(&shading);
  shading.dictAdd(qstrdup("Sh"), &sh);

  XWObject procset;
  initProcSet(&procset);

  XWObject res;
  initDict(&res);
  res.dictAdd(qstrdup("ProcSet"), &procset);
  res.dictAdd(qstrdup("Shading"), &shading);

  XWObject form;
	initXForm(shadingname,&form,&bbox,0,&res,ref);
  addPageContent("/Sh sh\n",7);
  endXForm();
}

void XWPDFDriver::shadingRadial(const QString & shadingname,
                   const QPointF & p,
                   double funs[][4],int nfuns,
									 XWObject * ref)
{
  double doma = funs[0][0];
  double domb = funs[nfuns - 1][0];
  double tmpdim = 2 * domb;
  XWObject bbox, func;
  initBBox(0,0,tmpdim,tmpdim,&bbox);

  if (nfuns == 2)
  {
    double *c0 = &(funs[0][1]);
    double *c1 = &(funs[1][1]);
    initFunc2(doma,domb,c0,3,c1,3,1,&func);
  }
  else
  {
    double bounds[100];
    double enc[100];
    enc[0] = 0;
    enc[1] = 1;
    int n = 2;
    int k = 0;
    for (int i = 1; i < (nfuns - 1); i++)
    {
      bounds[k++] = funs[i][1];
      enc[n++] = 1;
    }

    XWObject objs[100];
    for (int i = 0; i < (nfuns - 1); i++)
    {
      double a = funs[i][0];
      double b = funs[i+1][0];
      double * c0 = &(funs[i][1]);
      double * c1 = &(funs[i+1][1]);
      initFunc2(a,b,c0,3,c1,3,1,&(objs[i]));
    }
    
    XWObject func3;
    initFunc3(doma,domb,bounds,k,enc,n,&func);
    addFuns(&func,objs,nfuns - 1);
  }

  double coords[4];
  coords[0] = p.x() + domb;
  coords[1] = p.y() + domb;
  coords[2] = doma;
  coords[3] = domb;

  XWObject sh;
  initShading23(3,doma,domb,coords,4,&func,&sh);

  XWObject shading;
  initDict(&shading);
  shading.dictAdd(qstrdup("Sh"), &sh);

  XWObject procset;
  initProcSet(&procset);

  XWObject res;
  initDict(&res);
  res.dictAdd(qstrdup("ProcSet"), &procset);
  res.dictAdd(qstrdup("Shading"), &shading);

  XWObject form;
	initXForm(shadingname,&form,&bbox,0,&res,ref);
  addPageContent("/Sh sh\n",7);
  endXForm();
}

void XWPDFDriver::stroke()
{
  graphicsMode();
  addPageContent("S\n", 2);
}

void XWPDFDriver::strokeText()
{
	stringMode();
	addPageContent("1 Tr\n", 5);
}

void XWPDFDriver::useExtGState(const QString & name)
{
	QString shortname = extGSShortNames[name];
	QByteArray ba = shortname.toAscii();

  char buf[20];
  int len = sprintf(buf, "%s gs\n", ba.constData());
	addPageContent(buf, len);
}

void XWPDFDriver::useFading(const QString & name,XWObject * stm)
{
	QString shortname;
	if (extGSShortNames.contains(name))
	  shortname = extGSShortNames[name];
	else
	{
		nextExtGS++;
	  QString shortname = QString("/GS%1").arg(nextExtGS);
	  extGSShortNames[name] = shortname;
		
		XWObject extgs, tmp;
	  extgs.initDict(xref);
	  tmp.initName("ExtGState");
	  extgs.dictAdd(qstrdup("Type"), &tmp);

	  XWObject mask;
	  mask.initDict(xref);
	  tmp.initName("Luminosity");
	  mask.dictAdd(qstrdup("S"), &tmp);
	  mask.dictAdd(qstrdup("G"), stm);
	  extgs.dictAdd(qstrdup("SMask"), &mask);

	  XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
	  XWObject ref;
	  myref->addExtGState(&extgs,shortname,&ref);
	}

	QByteArray ba = shortname.toAscii();

  char buf[20];
  int len = sprintf(buf, "%s gs\n", ba.constData());
	addPageContent(buf, len);
}

void XWPDFDriver::usePattern(const QString & name)
{
	QString shortname = patternShortNames[name];
	QByteArray ba = shortname.toAscii();

  char buf[100];
  int len = sprintf(buf, "/Pattern cs %s scn\n", ba.constData());
	addPageContent(buf, len);
}

void XWPDFDriver::usePattern(const QString & csname,
                  const QColor & rgb,
                  const QString & patname)
{
	QString shortname = colorSpaceShortNames[csname];
	QByteArray ba = shortname.toAscii();
	char buf[100];
  int len = sprintf(buf, "%s cs ", ba.constData());
	len += sprintf(buf + len, "%g %g %g ", round(rgb.redF(), 0.001),
                    round(rgb.greenF(), 0.001),round(rgb.blueF(), 0.001));
	shortname = patternShortNames[patname];
	ba = shortname.toAscii();
	len += sprintf(buf + len, "%s scn\n", ba.constData());
	addPageContent(buf, len);
}

void XWPDFDriver::useXObject(const QString & name)
{
	QString shortname = xobjShortNames[name];
	QByteArray ba = shortname.toAscii();

  char buf[20];
  int len = sprintf(buf, "%s Do\n", ba.constData());
	addPageContent(buf, len);
}

void XWPDFDriver::addToUnicode(XWType0Font * font, XWObject * fontdict)
{
	XWCIDFont * cidfont = font->getDescendant();
	if (!cidfont)
		return ;
		
	if (cidfont->isACCFont())
		return;
		
	XWObject tounicode;
	tounicode.initNull();
	if (cidfont->isUCSFont())
	{
		readToUnicodeFile("Adobe-Identity-UCS2", &tounicode);
		if (tounicode.isNull())
			tounicode.initName("Identity-H");
			
		fontdict->dictAdd(qstrdup("ToUnicode"), &tounicode);
		return;
	}
	
	CIDSysInfo * csi = cidfont->getCIDSysInfo();
  const char * fontname  = cidfont->getFontName();
  if (cidfont->getEmbedding()) 
   	fontname += 7;
    	
  if (!strcmp(csi->registry, "Adobe") && !strcmp(csi->ordering, "Identity"))
  {
  	switch (cidfont->getSubtype())
   	{
   		case CIDFONT_TYPE2:
     		otfCreateToUnicodeStream((const char * )(cidfont->getIdent()),
					                            cidfont->getOptIndex(),
					                            (const char * )(font->getUsedChars()),
					                            &tounicode);
      	break;
      			
      default:
      	if (cidfont->getFlag(CIDFONT_FLAG_TYPE1C)) 
      	{ /* FIXME */
					otfCreateToUnicodeStream((const char * )(cidfont->getIdent()),
					                               cidfont->getOptIndex(),
					                               (const char * )(font->getUsedChars()),
					                               &tounicode);
      	} 
      	else if (cidfont->getFlag(CIDFONT_FLAG_TYPE1)) 
      	{ /* FIXME */
					/* Font loader will create ToUnicode and set. */
					return;
      	} 
      	else 
      	{
					char * cmap_name = new char[strlen(fontname) + 7];
					sprintf(cmap_name, "%s-UTF16", fontname);
					readToUnicodeFile(cmap_name, &tounicode);
					if (tounicode.isNull()) 
					{
	  				sprintf(cmap_name, "%s-UCS2", fontname);
	  				readToUnicodeFile(cmap_name, &tounicode);
					}
					delete [] cmap_name;
      	}
      	break;
    	}
  }
  else
  {
   	char * cmap_name = new char[strlen(csi->registry)+strlen(csi->ordering)+8];
   	sprintf(cmap_name, "%s-%s-UTF16", csi->registry, csi->ordering);
   	readToUnicodeFile(cmap_name, &tounicode);
   	if (tounicode.isNull()) 
   	{
     	sprintf(cmap_name, "%s-%s-UCS2", csi->registry, csi->ordering);
     	readToUnicodeFile(cmap_name, &tounicode);
   	}
   	delete [] cmap_name;
  }
    
  if (!tounicode.isNull())
   	fontdict->dictAdd(qstrdup("ToUnicode"), &tounicode);
}

XWObject * XWPDFDriver::createCMapStream(XWCMap *cmap,XWObject * obj)
{
  if (cmap->getType() == CMAP_TYPE_IDENTITY)
    return 0;

  obj->initStream(STREAM_COMPRESS, xref);
  CIDSysInfo * csi = cmap->getCIDSysInfo();
  if (!csi) 
   	csi = (cmap->getType() != CMAP_TYPE_TO_UNICODE) ? &CSI_IDENTITY : &CSI_UNICODE;

  if (cmap->getType() != CMAP_TYPE_TO_UNICODE)
  {
   	XWObject csi_dict, obj1;
   	csi_dict.initDict(xref);
   	obj1.initString(new XWString(csi->registry, strlen(csi->registry)));
   	csi_dict.dictAdd(qstrdup("Registry"), &obj1);
    	
   	obj1.initString(new XWString(csi->ordering, strlen(csi->ordering)));
   	csi_dict.dictAdd(qstrdup("Ordering"), &obj1);
    	
   	obj1.initInt(csi->supplement);
   	csi_dict.dictAdd(qstrdup("Supplement"), &obj1);
    	
   	obj1.initName("CMap");
   	obj->streamGetDict()->add(qstrdup("Type"), &obj1);
    	
   	const char * tmp = (const char * )(cmap->getName());
   	obj1.initName(tmp);
   	obj->streamGetDict()->add(qstrdup("CMapName"), &obj1);
   	obj->streamGetDict()->add(qstrdup("CIDSystemInfo"), &csi_dict);
   	if (cmap->getWMode() != 0)
   	{
   		obj1.initInt(cmap->getWMode());
   		obj->streamGetDict()->add(qstrdup("WMode"), &obj1);
   	}
  }
    
  XWCMap * useCMap = cmap->getUseCMap();
  if (useCMap)
  {
   	XWObject obj1;
   	if (useCMap->isIdentity())
   	{
   		if (cmap->getWMode() == 1)
   		{
   			obj1.initName("Identity-V");
   			obj->streamGetDict()->add(qstrdup("UseCMap"), &obj1);
   		}
   		else
   		{
   			obj1.initName("Identity-H");
   			obj->streamGetDict()->add(qstrdup("UseCMap"), &obj1);
   		}
   	}
   	else
   	{
   		XWObject ucmap_obj,ucmap_ref;
   		createCMapStream(useCMap, &ucmap_obj);
			XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
      myref->addObject(&ucmap_obj,&ucmap_ref);
   		obj->streamGetDict()->add(qstrdup("UseCMap"), &ucmap_ref);
   	}
  }
    
  cmap->writeStream(obj->streamGetIO());
	return obj;
}

XWObject * XWPDFDriver::createDummyCMap(XWObject * obj)
{
#define CMAP_PART0 "\
%!PS-Adobe-3.0 Resource-CMap\n\
%%DocumentNeededResources: ProcSet (CIDInit)\n\
%%IncludeResource: ProcSet (CIDInit)\n\
%%BeginResource: CMap (Adobe-Identity-UCS2)\n\
%%Title: (Adobe-Identity-UCS2 Adobe UCS2 0)\n\
%%Version: 1.0\n\
%%Copyright:\n\
%% ---\n\
%%EndComments\n\n\
"
#define CMAP_PART1 "\
/CIDInit /ProcSet findresource begin\n\
\n\
12 dict begin\n\nbegincmap\n\n\
/CIDSystemInfo 3 dict dup begin\n\
  /Registry (Adobe) def\n\
  /Ordering (UCS2) def\n\
  /Supplement 0 def\n\
end def\n\n\
/CMapName /Adobe-Identity-UCS2 def\n\
/CMapVersion 1.0 def\n\
/CMapType 2 def\n\n\
2 begincodespacerange\n\
<0000> <FFFF>\n\
endcodespacerange\n\
"
#define CMAP_PART3 "\
endcmap\n\n\
CMapName currentdict /CMap defineresource pop\n\n\
end\nend\n\n\
%%EndResource\n\
%%EOF\n\
"

	obj->initStream(STREAM_COMPRESS, xref);
	
	obj->streamAdd(CMAP_PART0, strlen(CMAP_PART0));
  obj->streamAdd(CMAP_PART1, strlen(CMAP_PART1));
  obj->streamAdd("\n100 beginbfrange\n", strlen("\n100 beginbfrange\n"));
  char buf[32];
  for (int i = 0; i < 0x64; i++) 
  {
   	int n = sprintf(buf, "<%02X00> <%02XFF> <%02X00>\n", i, i, i);
   	obj->streamAdd(buf, n);
  }
  	
  obj->streamAdd("endbfrange\n\n", strlen("endbfrange\n\n"));

  obj->streamAdd("\n100 beginbfrange\n", strlen("\n100 beginbfrange\n"));
  for (int i = 0x64; i < 0xc8; i++) 
  {
   	int n = sprintf(buf, "<%02X00> <%02XFF> <%02X00>\n", i, i, i);
   	obj->streamAdd(buf, n);
  }
  	
  obj->streamAdd("endbfrange\n\n", strlen("endbfrange\n\n"));

  obj->streamAdd("\n48 beginbfrange\n", strlen("\n48 beginbfrange\n"));
  for (int i = 0xc8; i <= 0xd7; i++) 
  {
   	int n = sprintf(buf, "<%02X00> <%02XFF> <%02X00>\n", i, i, i);
   	obj->streamAdd(buf, n);
  }
  	
  for (int i = 0xe0; i <= 0xff; i++) 
  {
   	int n = sprintf(buf, "<%02X00> <%02XFF> <%02X00>\n", i, i, i);
   	obj->streamAdd(buf, n);
  }
  	
  obj->streamAdd("endbfrange\n\n", strlen("endbfrange\n\n"));

  obj->streamAdd(CMAP_PART3, strlen(CMAP_PART3));

  return  obj;
}

XWObject * XWPDFDriver::createToUnicodeCMap4(SFNTCmap4 *map,
									            const char *cmap_name, 
									            XWCMap *cmap_add,
									            const char *used_glyphs,
									            XWObject * obj)
{
	char used_glyphs_copy[8192];

  XWCMap cmap;
  cmap.setName(cmap_name);
  cmap.setWMode(0);
  cmap.setType(CMAP_TYPE_TO_UNICODE);
  cmap.setCIDSysInfo(&CSI_UNICODE);
  cmap.addCodeSpaceRange(srange_min, srange_max, 2);
  	
  memcpy(used_glyphs_copy, used_glyphs, 8192);
  ushort segCount = map->segCountX2 / 2;
  ushort countA = 0;
  uchar wbuf[1024];
  ushort gid = 0;
  for (ushort i = 0; i < segCount; i++)
  {
  	ushort c0 = map->startCount[i];
   	ushort c1 = map->endCount[i];
   	ushort d  = map->idRangeOffset[i] / 2 - (segCount - i);
   	for (ushort j = 0; j <= c1 - c0; j++)
   	{
   		ushort ch = c0 + j;
   		if (map->idRangeOffset[i] == 0)
   			gid = (ch + map->idDelta[i]) & 0xffff;
   		else
   			gid = (map->glyphIndexArray[j+d] + map->idDelta[i]) & 0xffff;
    			
   		if (is_used_char2(used_glyphs_copy, gid))
   		{
   			countA++;
				wbuf[0] = (gid >> 8) & 0xff;
				wbuf[1] = (gid & 0xff);

				wbuf[2] = (ch >> 8) & 0xff;
				wbuf[3] =  ch & 0xff;

				cmap.addBFChar(wbuf, 2, wbuf+2, 2);

				used_glyphs_copy[gid/8] &= ~(1 << (7 - (gid % 8)));
				countA++;
   		}
   	}
  }
  	
  countA += handleSubstGlyphs(&cmap, cmap_add, used_glyphs_copy);
  if (countA < 1)
  	obj->initNull();
  else
  	createCMapStream(&cmap, obj);
  		
  return obj;
}

XWObject * XWPDFDriver::createToUnicodeCMap12(SFNTCmap12 * map,
			 						             const char *cmap_name, 
			 						             XWCMap *cmap_add,
			 						             const char *used_glyphs,
			 						             XWObject * obj)
{
	char  used_glyphs_copy[8192];
  XWCMap cmap;
 	cmap.setName(cmap_name);
 	cmap.setWMode(0);
 	cmap.setType(CMAP_TYPE_TO_UNICODE);
 	cmap.setCIDSysInfo(&CSI_UNICODE);
 	cmap.addCodeSpaceRange(srange_min, srange_max, 2);

 	memcpy(used_glyphs_copy, used_glyphs, 8192);
  	
 	ushort countA = 0;
 	ushort gid = 0;
 	ulong ch = 0;
 	uchar wbuf[1024];
 	for (ulong i = 0; i < map->nGroups; i++)
 	{
 		for (ch  = map->groups[i].startCharCode; ch <= map->groups[i].endCharCode; ch++)
 		{
 			uchar * p   = wbuf + 2;
   		long d   = ch - map->groups[i].startCharCode;
   		gid = (ushort) ((map->groups[i].startGlyphID + d) & 0xffff);
   		if (is_used_char2(used_glyphs_copy, gid)) 
   		{
     		countA++;
     		wbuf[0] = (gid >> 8) & 0xff;
     		wbuf[1] = (gid & 0xff);
     		int len = XWAdobeGlyphList::UC_sput_UTF16BE((long)ch, &p, wbuf+1024);

     		used_glyphs_copy[gid/8] &= ~(1 << (7 - (gid % 8)));
     		cmap.addBFChar(wbuf, 2, wbuf+2, len);
   		}
 		}
 	}
  	
 	countA += handleSubstGlyphs(&cmap, cmap_add, used_glyphs_copy);

 	if (countA < 1)
 		obj->initNull();
 	else
 		createCMapStream(&cmap, obj);
  		
 	return obj;
}

void XWPDFDriver::doType0(const QString & family,
                 double size,
                 const QString & shortname)
{
  QFont font(family);
  font.setPointSizeF(size);
  QString rawname = font.rawName();
  QByteArray ps_name = QFile::encodeName(rawname);
  QByteArray fam = QFile::encodeName(family);
  QString sn = font.styleName();
  QByteArray style_name = QFile::encodeName(sn);

  XWFontCache fcache(true);
  XWTexFontMap * mrec = fcache.lookupFontMapRecord(ps_name.constData());
  if (!mrec)
  {
    fcache.loadNativeFont(ps_name.constData(),fam.constData(),style_name.constData(),0);
    mrec = fcache.lookupFontMapRecord(ps_name.constData());
  }
  
	int cmap_id = -1;
  if (mrec && mrec->enc_name)
  {
    cmap_id = fcache.findCMap(mrec->enc_name);
    if (cmap_id >= 0)
		{
			XWCMap * cmap = fcache.getCMap(cmap_id);
			int minbytes = cmap->getProfile(CMAP_PROF_TYPE_INBYTES_MIN);
			if (minbytes == 2 && mrec->opt->mapc < 0)
				mrec->opt->mapc = 0;
		}
    else if (!strcmp(mrec->enc_name, "unicode"))
    {
      cmap_id = otfLoadUnicodeCMap(mrec->font_name,
				                                   mrec->opt->index, mrec->opt->otl_tags,
					                               ((mrec->opt->flags & FONTMAP_OPT_VERT) ? 1 : 0));
			if (cmap_id < 0)
				cmap_id = XWCIDFontCache::t1LoadUnicodeCMap(mrec->font_name, 
						                                        mrec->opt->otl_tags,
												               ((mrec->opt->flags & FONTMAP_OPT_VERT) ? 1 : 0));
    }
  }

	XWObject fontdict, obj,descendant,descriptor;
  fontdict.initDict(xref);
  obj.initName("Font");
	fontdict.dictAdd(qstrdup("Type"), &obj);
	obj.initName("Type0");
	fontdict.dictAdd(qstrdup("Subtype"), &obj);

	descendant.initDict(xref);
	descriptor.initDict(xref);
	obj.initName("FontDescriptor");
	descriptor.dictAdd(qstrdup("Type"), &obj);	

  int font_id = -1;
	XWType0Font * type0font = 0;
	XWCIDFont * cidfont = 0;
	XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
  if (mrec && cmap_id >= 0)
	{
		font_id = fcache.findType0Font(mrec->font_name, cmap_id, mrec->opt, 4);
		type0font = fcache.getType0Font(font_id);
		if (type0font)
		{
			const char * tmp = type0font->getBaseFont();
	    if (tmp)
	    {
		    obj.initName(tmp);
		    fontdict.dictAdd(qstrdup("BaseFont"), &obj);
	    }
	
	    tmp = type0font->getEncoding();
	    if (tmp)
	    {
		    obj.initName(tmp);
		    fontdict.dictAdd(qstrdup("Encoding"), &obj);
	    }
			cidfont = type0font->getDescendant();
			if (cidfont)
			{
				obj.initName(cidfont->getDescFontName());
	      descriptor.dictAdd(qstrdup("FontName"), &obj);

				if (!cidfont->getFlag(CIDFONT_FLAG_TYPE1))
	      {
		      obj.initReal(cidfont->getDescAscent());
		      descriptor.dictAdd(qstrdup("Ascent"), &obj);
		      obj.initReal(cidfont->getDescDescent());
		      descriptor.dictAdd(qstrdup("Descent"), &obj);
		      obj.initReal(cidfont->getStemV());
		      descriptor.dictAdd(qstrdup("StemV"), &obj);
		      obj.initReal(cidfont->getCapHeight());
		      descriptor.dictAdd(qstrdup("CapHeight"), &obj);
		      if (cidfont->getXHeight() != 0.0)
		      {
			      obj.initReal(cidfont->getXHeight());
			      descriptor.dictAdd(qstrdup("XHeight"), &obj);
		      }
	
		      if (cidfont->getAvgWidth() != 0.0)
		      {
			      obj.initReal(cidfont->getAvgWidth());
			      descriptor.dictAdd(qstrdup("AvgWidth"), &obj);
		      }
	
		      obj.initArray(xref);
		      double * bbox = cidfont->getFontBBox();
		      XWObject tmp;
		      for (int i = 0; i < 4; i++)
		      {
			      tmp.initReal(bbox[i]);
			      obj.arrayAdd(&tmp);
		      }
		      descriptor.dictAdd(qstrdup("FontBBox"), &obj);
	
		      obj.initReal(cidfont->getItalicAngle());
		      descriptor.dictAdd(qstrdup("ItalicAngle"), &obj);
	
		      obj.initInt(cidfont->getDescFlags());
		      descriptor.dictAdd(qstrdup("Flags"), &obj);
	
		      XWObject styledict;
		      styledict.initDict(xref);
		      uchar * panose = cidfont->getPanose();
		      obj.initString(new XWString((char*)panose, 12));
		      styledict.dictAdd(qstrdup("Panose"), &obj);
		      descriptor.dictAdd(qstrdup("Style"), &styledict);
	      }

				XWCMap * tounicodecmap = 0;
	      uchar * streamdata= 0;
	      long   streamlen = 0;
	      char * usedchars = 0;
	      int    lastcid = 0;
	      uchar* cidtogidmapA = 0;
	      cidfont->load(&tounicodecmap, &streamdata, &streamlen, &usedchars, &lastcid, &cidtogidmapA);
	      if (cidfont->getSubtype() == CIDFONT_TYPE2)
	      {
		      CIDSysInfo * csi = cidfont->getCIDSysInfo();
		      if (csi)
		      {
			      XWObject csi_dict;
			      csi_dict.initDict(xref);
			      obj.initString(new XWString(csi->registry, strlen(csi->registry)));
			      csi_dict.dictAdd(qstrdup("Registry"), &obj);
			      obj.initString(new XWString(csi->ordering, strlen(csi->ordering)));
			      csi_dict.dictAdd(qstrdup("Ordering"), &obj);
			      obj.initInt(csi->supplement);
			      csi_dict.dictAdd(qstrdup("Supplement"), &obj);
			      descendant.dictAdd(qstrdup("CIDSystemInfo"), &csi_dict);
		      }
	      }
	      else
	      {
		      if (cidfont->getFlag(CIDFONT_FLAG_TYPE1))
		      {
			      obj.initReal(cidfont->getCapHeight());
			      descriptor.dictAdd(qstrdup("CapHeight"), &obj);
			      obj.initReal(cidfont->getDescAscent());
			      descriptor.dictAdd(qstrdup("Ascent"), &obj);
			      obj.initReal(cidfont->getDescDescent());
			      descriptor.dictAdd(qstrdup("Descent"), &obj);
			      obj.initReal(cidfont->getItalicAngle());
			      descriptor.dictAdd(qstrdup("ItalicAngle"), &obj);
			      obj.initReal(cidfont->getStemV());
			      descriptor.dictAdd(qstrdup("StemV"), &obj);
			      obj.initInt(cidfont->getDescFlags());
			      descriptor.dictAdd(qstrdup("Flags"), &obj);
			      if (tounicodecmap)
			      {
				      XWObject tounicode;
				      if (createCMapStream(tounicodecmap, &tounicode))
				      {
					      myref->addObject(&tounicode, &obj);
					      fontdict.dictAdd(qstrdup("ToUnicode"), &obj);
				      }
		
				      delete tounicodecmap;
			      }
		      }
	      }
	
	      obj.initInt(cidfont->getDW());
	      descendant.dictAdd(qstrdup("DW"), &obj);
	
	      XWCIDWidths * W = cidfont->getW();
	      if (W)
	      {
		      getCIDWidths(W, &obj);
		      XWObject tmp;
					myref->addObject(&obj, &tmp);
		      descendant.dictAdd(qstrdup("W"), &tmp);
	      }
	
	      W = cidfont->getDW2();
	      if (W)
	      {
		      getCIDWidths(W, &obj);
		      XWObject tmp;
		      myref->addObject(&obj, &tmp);
		      descendant.dictAdd(qstrdup("DW2"), &tmp);
	      }
	
	      W = cidfont->getW2();
	      if (W)
	      {
		      getCIDWidths(W, &obj);
		      XWObject tmp;
		      myref->addObject(&obj, &tmp);
		      descendant.dictAdd(qstrdup("W2"), &tmp);
	      }
	
	      if (streamdata)
	      {
		      XWObject fontfile;
		      fontfile.initStream(STREAM_COMPRESS, xref);
		      myref->addObject(&fontfile, &obj);
		      if (cidfont->getSubtype() == CIDFONT_TYPE0)
		      {
			      descriptor.dictAdd(qstrdup("FontFile3"), &obj);
			      obj.initName("CIDFontType0C");
			      fontfile.streamGetDict()->add(qstrdup("Subtype"), &obj);
		      }
		      else
		      {
			      descriptor.dictAdd(qstrdup("FontFile2"), &obj);
			      obj.initInt(streamlen);
			      fontfile.streamGetDict()->add(qstrdup("Length1"), &obj);
		      }
		      fontfile.streamAdd((const char*)streamdata, streamlen);
		      free(streamdata);
	      }
	
	      if (usedchars)
	      {
		      XWObject cidset;
		      cidset.initStream(STREAM_COMPRESS, xref);
		      cidset.streamAdd(usedchars, (lastcid/8)+1);
		      myref->addObject(&cidset, &obj);
		      descriptor.dictAdd(qstrdup("CIDSet"), &obj);
	      }
		
	      if (cidtogidmapA)
	      {
		      XWObject c2gmstream;
		      c2gmstream.initStream(STREAM_COMPRESS, xref);
		      c2gmstream.streamAdd((const char*)cidtogidmapA, (lastcid + 1) * 2);
		      myref->addObject(&c2gmstream, &obj);
		      descriptor.dictAdd(qstrdup("CIDToGIDMap"), &obj);
		      free(cidtogidmapA);
	      }
			}

      obj.initNull();
			fontdict.dictLookupNF("ToUnicode", &obj);
	    if (obj.isNull())
			  addToUnicode(type0font,&fontdict);
		}
	}

	myref->addObject(&descriptor,&obj);
	descendant.dictAdd(qstrdup("FontDescriptor"), &obj);
	myref->addObject(&descendant,&obj);
	XWObject array;
	array.initArray(xref);
	array.arrayAdd(&obj);
	fontdict.dictAdd(qstrdup("DescendantFonts"), &array);
	XWObject ref;
	myref->addFont(&fontdict,shortname,&ref);
}

void XWPDFDriver::doType1(const char * fontname,
                 const char * map_name,
                 double size,
                 const QString & shortname)
{
  XWTexFont font(fontname,map_name,-1,0,size);
  if (!font.openType1())
    return ;

  char ** enc_vec = 0;
	enc_vec = (char**)malloc(256 * sizeof(char *));
  for (ushort code = 0; code <= 0xff; code++) 
   	enc_vec[code] = 0;

  char fullname[200];
	uchar * streamdata = 0;
	long streamlen = 0;
  bool ok = font.loadType1(enc_vec, fullname, &streamdata, &streamlen);
  for (ushort code = 0; code < 256; code++) 
 	{
 		if (enc_vec[code])
  		delete [] enc_vec[code];
  	enc_vec[code] = 0;
  }
  free(enc_vec);

  if (!ok)
    return ;

  XWObject descriptor;
  descriptor.initDict(xref);
	XWObject obj,obj1;
  obj.initName("FontDescriptor");
	descriptor.dictAdd(qstrdup("Type"), &obj);
  obj.initReal(font.getCapHeight());
	descriptor.dictAdd(qstrdup("CapHeight"), &obj);
	if (font.getXHeight() > 0.0)
	{
		obj.initReal(font.getXHeight());
		descriptor.dictAdd(qstrdup("XHeight"), &obj);
	}
	obj.initReal(font.getDescAscent());
	descriptor.dictAdd(qstrdup("Ascent"), &obj);
	obj.initReal(font.getDescDescent());
	descriptor.dictAdd(qstrdup("Descent"), &obj);
	obj.initReal(font.getItalicAngle());
	descriptor.dictAdd(qstrdup("ItalicAngle"), &obj);
	obj.initReal(font.getStemV());
	descriptor.dictAdd(qstrdup("StemV"), &obj);

  if (font.getAvgWidth() > 0.0)
	{
		obj.initReal(font.getAvgWidth());
		descriptor.dictAdd(qstrdup("AvgWidth"), &obj);
	}
	
	obj.initInt(font.getDescFlags());
	descriptor.dictAdd(qstrdup("Flags"), &obj);
	
	double * fontbbox = font.getDescFontBBox();
	
	obj.initArray(xref);
	for (int i = 0; i < 4; i++)
	{
		obj1.initReal(fontbbox[i]);
		obj.arrayAdd(&obj1);
	}
	
	descriptor.dictAdd(qstrdup("FontBBox"), &obj);

  XWObject resource;
  resource.initDict(xref);
  obj.initName("Font");
	resource.dictAdd(qstrdup("Type"), &obj);
  obj.initName("Type1");
  resource.dictAdd(qstrdup("Subtype"), &obj);

  obj.initArray(xref);
	int firstchar = font.getFirstChar();
	int lastchar = font.getLastChar();
	double *  widths = font.getDictWidths();

	for (int code = firstchar; code <= lastchar; code++)
	{
		obj1.initReal(widths[code]);
		obj.arrayAdd(&obj1);
	}

	XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
	myref->addObject(&obj,&obj1);
	resource.dictAdd(qstrdup("Widths"), &obj1);
	obj.initInt(firstchar);
	resource.dictAdd(qstrdup("FirstChar"), &obj);
	obj.initInt(lastchar);
	resource.dictAdd(qstrdup("LastChar"), &obj);
	
	if (streamdata)
	{
		XWObject fontfile;
		fontfile.initStream(STREAM_COMPRESS, xref);
		obj.initName("Type1C");
		fontfile.streamGetDict()->add(qstrdup("Subtype"), &obj);
		fontfile.streamAdd((const char*)streamdata, streamlen);
		free(streamdata);
    myref->addObject(&fontfile,&obj);
    descriptor.dictAdd(qstrdup("FontFile3"), &obj);
	}

  if (font.getFlag(PDF_FONT_FLAG_NOEMBED))
  {
    char * name = font.getFontName();
    obj.initName(name);
    resource.dictAdd(qstrdup("BaseFont"), &obj);
    obj.initName(name);
	  descriptor.dictAdd(qstrdup("FontName"), &obj);
  }
  else
  {
    char fontnameA[100];
		char * uniqueTag = font.getUniqueTag();
		sprintf(fontnameA, "%6s+%s", uniqueTag, fontname);
		obj.initName(fontnameA);
		resource.dictAdd(qstrdup("BaseFont"), &obj);
		obj.initName(fontnameA);
		descriptor.dictAdd(qstrdup("FontName"), &obj);
  }
  
  myref->addObject(&descriptor,&obj);
  resource.dictAdd(qstrdup("FontDescriptor"), &obj);
	XWObject ref;
  myref->addFont(&resource,shortname,&ref);
}

int XWPDFDriver::doubleToAscii(double value, int prec, char *buf)
{
  const long p[10] = { 1, 10, 100, 1000, 10000,
		       100000, 1000000, 10000000, 100000000, 1000000000 };
		       
	char *c = buf;
  	int n = 0;
  	if (value < 0) 
  	{
    	value = -value;
    	*c++ = '-';
    	n = 1;
  	}
  	
  	long i = (long) value;
  	long f = (long) ((value-i)*p[prec] + 0.5);
  	
  	if (f == p[prec]) 
  	{
    	f = 0;
    	i++;
  	}
  	
  	if (i) 
  	{
    	int m = intToAscii(i, c);
    	c += m;
    	n += m;
  	} 
  	else if (!f) 
  	{
    	*(c = buf) = '0';
    	n = 1;
  	}

  	if (f) 
  	{
    	int j = prec;
    	*c++ = '.';

    	while (j--) 
    	{
      		c[j] = (f % 10) + '0';
      		f /= 10;
    	}
    	c += prec-1;
    	n += 1+prec;

    	while (*c == '0') 
    	{
      		c--;
      		n--;
    	}
  	}

  	*(++c) = 0;

  	return n;
}

void XWPDFDriver::flushFonts()
{
  QHashIterator<QString, int> i(fontType);
  while (i.hasNext())
  {
    i.next();
    QString fontname = i.key();
    int fonttype = i.value();
    QString shortname = fontShortNames[fontname];
    double size = fontSize[fontname];
    flushFont(fonttype,fontname,shortname,size);
  }
}

void XWPDFDriver::flushFont(int fonttype,
                            const QString & fontname,
                            const QString & shortname,
                            double size)
{
  switch (fonttype)
  {
    case PDF_FONT_TYPE1:
      {
        QByteArray ident = QFile::encodeName(fontname);
        QByteArray map_name = ident;
        int i = fontname.length() - 1;
        while (fontname[i].isNumber())
          i--;

        if (i > 0)
        {
          i++;
          QString tmp = fontname.left(i);
          map_name = QFile::encodeName(tmp);
        }

        doType1(ident.constData(),map_name.constData(),size,shortname);
      }
      break;

    case PDF_FONT_TRUETYPE:
      doType0(fontname,size,shortname);
      break;

    default:
      break;
  }
}

void XWPDFDriver::getCIDWidths(XWCIDWidths * w, XWObject * w_array)
{
  XWObject obj;
	w_array->initArray(xref);
	for (int i = 0; i <= w->count; i++)
	{
		if (w->types[i] == 0)
		{
			XWCIDWidths::Widths * tmp = (XWCIDWidths::Widths*)(w->widths[i]);
			for (int k = 0; k < tmp->len; k++)
			{
				obj.initReal(tmp->values[k]);
				w_array->arrayAdd(&obj);
			}
		}
		else
		{
			XWCIDWidths * tmp = (XWCIDWidths*)(w->widths[i]);
			getCIDWidths(tmp, &obj);
			w_array->arrayAdd(&obj);
			obj.initNull();
		}
	}
}

void XWPDFDriver::graphicsMode()
{
  switch (motion_state) 
	{
  	case PDF_GRAPHICS_MODE:
    	break;
    
		default:
    	addPageContent("ET\n",3);  /* op: ET */
    	break;
  }
  motion_state = PDF_GRAPHICS_MODE;
}

int XWPDFDriver::handleCIDFont(XWFontFileSFNT *sfont,
					              uchar **GIDToCIDMap, 
					              CIDSysInfo *csi)
{
  ulong offset = sfont->findTablePos("CFF ");
  if (offset == 0) 
  {
   	csi->registry = 0;
   	csi->ordering = 0;
   	*GIDToCIDMap  = 0;
   	return 0;
  }
  	
  XWTTMaxpTable * maxp = sfont->readMaxpTable();
  ushort num_glyphs = (ushort)maxp->numGlyphs;
  delete maxp;
  if (num_glyphs < 1)
  	return -1;
  	
  QIODevice * fp = sfont->getFile();
  XWFontFileCFF * cffont = XWFontFileCFF::make(fp, 0, offset, (ulong)(fp->size()) - offset, 0);
  if (!cffont)
   	return -1;
    
  if (!cffont->isCIDFont())
  {
   	delete cffont;
   	csi->registry = 0;
   	csi->ordering = 0;
   	*GIDToCIDMap  = 0;
   	return 0;
  }
  if (!cffont->topDictKnow("ROS"))
  {
   	delete cffont;
   	return -1;
  }
  else
  {
   	ushort reg = (ushort)(cffont->topDictGet("ROS", 0));
   	ushort ord = (ushort)(cffont->topDictGet("ROS", 1));

   	csi->registry = cffont->getString(reg);
   	csi->ordering = cffont->getString(ord);
   	csi->supplement = (int)(cffont->topDictGet("ROS", 2));
  }
    
  cffont->readCharsets();
  XWCFFCharsets * charset = cffont->getCharsets();
  if (!charset) 
  {
  	delete cffont;
   	return -1;
  }
  	
  uchar * map = (uchar*)malloc(num_glyphs * 2 * sizeof(uchar));
  memset(map, 0, num_glyphs * 2);
  ushort gid = 0;
  switch (charset->format)
  {
  	case 0:
   		{
   			ushort * cids = charset->data.glyphs;
   			gid = 1;
   			for (ushort i = 0; i < charset->num_entries; i++) 
   			{
					map[2*gid  ] = (cids[i] >> 8) & 0xff;
					map[2*gid+1] = cids[i] & 0xff;
					gid++;
   			}
   		}
   		break;
    		
   	case 1:
   		{
   			CFFRange1 * ranges = charset->data.range1;
   			gid = 1;
   			for (ushort i = 0; i < charset->num_entries; i++) 
   			{
	  			ushort cid   = ranges[i].first;
					ushort countA = ranges[i].n_left + 1; /* card8 */
					while (countA-- > 0 && gid <= num_glyphs) 
					{
	  					map[2*gid    ] = (cid >> 8) & 0xff;
	  					map[2*gid + 1] = cid & 0xff;
	  					gid++;
					}
    		}
    	}
    	break;
    		
    case 2:
    	{
    		CFFRange2 * ranges = charset->data.range2;
    		if (charset->num_entries == 1 && ranges[0].first == 1) 
    		{
					/* "Complete" CIDFont */
					free(map); 
					map = 0;
    		} 
    		else 
    		{
    			gid = 1;
					for (ushort i = 0; i < charset->num_entries; i++) 
					{
	  				ushort cid   = ranges[i].first;
	  				ushort countA = ranges[i].n_left + 1;
	  				while (countA-- > 0 && gid <= num_glyphs) 
	  				{
	   					map[gid] = (cid >> 8) & 0xff;
	   					map[gid] = cid & 0xff;
	   					gid++;
	  				}
					}
    		}
    	}
    	break;
    		
    default:
    	free(map); 
    	map = 0;
    	delete cffont;
    	return -1;
    	break;
  }
  	
  delete cffont;

  *GIDToCIDMap = map;
  return 1;
}

ushort XWPDFDriver::handleSubstGlyphs(XWCMap *cmap, 
	                         XWCMap *cmap_add, 
	                         const char *used_glyphs)
{
	ushort countA = 0;
	uchar wbuf[1024];
	ushort gid = 0;
	for (ushort i = 0; i < 8192; i++)
	{
		if (used_glyphs[i] == 0)
      continue;
      		
  	for (int j = 0; j < 8; j++) 
   	{
   		gid = 8 * i + j;
   		if (!is_used_char2(used_glyphs, gid))
				continue;
				
  		if (cmap_add)
			{
				wbuf[0] = (gid >> 8) & 0xff;
				wbuf[1] =  gid & 0xff;
				uchar * inbuf     = wbuf;
				long inbytesleft  = 2;
				uchar * outbuf       = inbuf + 2;
				long outbytesleft = 1024 - 2;
				cmap_add->decode((const unsigned char **) &inbuf , &inbytesleft, &outbuf, &outbytesleft);
				if (inbytesleft == 0)
				{
					long len = 1024 - 2 - outbytesleft;
	 				cmap->addBFChar(wbuf, 2, wbuf + 2, len);
	 				countA++;
				}
	  	}
    }
	}
	
	return countA;
}

XWObject * XWPDFDriver::initXForm(const QString & fname,
	                      XWObject * form,
                       XWObject * bbox,
                       XWObject * matrix,
                       XWObject * res,
											 XWObject * ref)
{
	form->initStream(STREAM_COMPRESS, xref);
	XWDict * xform_dict = form->streamGetDict();
	XWObject obj;
	obj.initName("XObject");
	xform_dict->add(qstrdup("Type"), &obj);
	obj.initName("Form");
	xform_dict->add(qstrdup("Subtype"), &obj);
	obj.initReal(1.0);
	xform_dict->add(qstrdup("FormType"), &obj);

	if (bbox && (bbox->isArray() || bbox->isIndirect()))
	  xform_dict->add(qstrdup("BBox"), bbox);

	if (matrix && (matrix->isArray() || matrix->isIndirect()))
	  xform_dict->add(qstrdup("Matrix"), matrix);

  XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
	if (res)
	{
		myref->addObject(res,ref);
		xform_dict->add(qstrdup("Resources"), ref);
	}
	
	nextForm++;
	QString shortname = QString("/Fm%1").arg(nextForm);
	myref->addXObject(form,shortname,ref);
	xobjShortNames[fname] = shortname;
	xobjRefs[fname] = ref->getRefGen();
	return form;
}

int XWPDFDriver::intToAscii(long value, char *buf)
{
	int   sign  = 0;
  	char *p = buf;
  	if (value < 0) 
  	{
    	*p++  = '-';
    	value = -value;
    	sign  = 1;
  	}
  	
  	int ndigits = 0;
  	do 
  	{
    	p[ndigits++] = (value % 10) + '0';
    	value /= 10;
  	} while (value != 0);
  	
  	for (int i = 0; i < ndigits / 2 ; i++) 
  	{
      	char tmp = p[i];
      	p[i] = p[ndigits-i-1];
      	p[ndigits-i-1] = tmp;
    }
    
    p[ndigits] = '\0';

  	return  (sign ? ndigits + 1 : ndigits);
}

int XWPDFDriver::loadBaseCMap(const char *cmap_name, 
	                             int wmode,
		                         CIDSysInfo *csi, 
		                         uchar *GIDToCIDMap,
					             XWSFNTCmap *ttcmap)
{
  XWFontCache fcache(true);
	int cmap_id = fcache.findCMap(cmap_name);
  if (cmap_id < 0) 
  {
   	XWCMap  * cmap = new XWCMap;
   	cmap->setName(cmap_name);
   	cmap->setType(CMAP_TYPE_CODE_TO_CID);
   	cmap->setWMode(wmode);
   	cmap->addCodeSpaceRange(lrange_min, lrange_max, 4);

   	if (csi) 
   		cmap->setCIDSysInfo(csi);
   	else 
   		cmap->setCIDSysInfo(&CSI_IDENTITY);

   	if (ttcmap->format == 12) 
   		ttcmap->loadCMap12(GIDToCIDMap, cmap);
   	else if (ttcmap->format == 4) 
   		ttcmap->loadCMap4(GIDToCIDMap, cmap);

   	cmap_id = fcache.addCMap(cmap);
  }

  return cmap_id;
}

int XWPDFDriver::loadGsub(XWObject *conf, 
	                         XWOTLGsub *gsub_list, 
	                         XWFontFileSFNT *sfont)
{
  XWObject rule;
	XWPDFOTLConf::getRule(conf, &rule);
	if (rule.isNull())
		return 0;
    
	char * script   = XWPDFOTLConf::getScript(conf);
  char * language = XWPDFOTLConf::getLanguage(conf);
  long size = rule.arrayGetLength();
  XWObject tmp, commands;
  for (long i = 0; i < size; i += 2)
  {
  	rule.arrayGetNF(i, &tmp);
   	int flag = tmp.getInt();
    	
   	rule.arrayGetNF(i+1, &commands);
   	long num_comms = commands.arrayGetLength();
   	for (long j = 0 ; j < num_comms; j += 4)
   	{
   		commands.arrayGetNF(1, &tmp);
   		if (tmp.isString())
   		{
   			char * feature = tmp.getString()->getCString();
   			if (gsub_list->addFeat(script, language, feature, sfont) < 0)
   			{
   				if (flag == 'r')
   				{
   					tmp.free();
   					commands.free();
   					rule.free();
   					return -1;
   				}
   			}
   		}
   		tmp.free();
   	}
   	commands.free();
  }
  	
  rule.free();
  return 0;
}

XWObject * XWPDFDriver::loadToUnicodeStream(const char *ident, XWObject * obj)
{
	XWFontCache fcache(true);
	int id = fcache.findCMap(ident);
	if (id < 0)
		return 0;
		
	XWCMap * cmap = fcache.getCMap(id);
	return createCMapStream(cmap, obj);
}

XWObject * XWPDFDriver::otfCreateToUnicodeStream(const char *font_name,
			     					                int ttc_index,
			                              const char *used_glyphs,
			                              XWObject * obj)
{
	char * cmap_name = new char[strlen(font_name)+strlen("-UTF16")+5];
  sprintf(cmap_name, "%s,%03d-UTF16", font_name, ttc_index);
  	
  XWFontFileSFNT * sfont = XWFontFileSFNT::load((char*)font_name, ttc_index, false);
  if (!sfont)
  {
  	delete [] cmap_name;
  	return 0;
  }
  	
  XWFontCache fcache(true);
  int cmap_add_id = fcache.findCMap(cmap_name);
  XWCMap * cmap_add = 0;
  if (cmap_add_id >= 0) 
   	cmap_add = fcache.getCMap(cmap_add_id);
  
  XWSFNTCmap * ttcmap = sfont->readCmap(3, 10);
  XWObject cmap_obj;
  cmap_obj.initNull();
  if (ttcmap && ttcmap->format == 12)
  	createToUnicodeCMap12((SFNTCmap12*)(ttcmap->map), cmap_name, cmap_add, used_glyphs, &cmap_obj);
  else
  {
  	if (ttcmap)
  		delete ttcmap;
  			
  	ttcmap = sfont->readCmap(3, 1);
  	if (ttcmap && ttcmap->format == 4)
  		createToUnicodeCMap4((SFNTCmap4*)(ttcmap->map), cmap_name, cmap_add, used_glyphs, &cmap_obj);
  	else
  	{
  		if (ttcmap)
  			delete ttcmap;
  				
  		ttcmap = sfont->readCmap(0, 3);
  		if (ttcmap && ttcmap->format == 4)
  			createToUnicodeCMap4((SFNTCmap4*)(ttcmap->map), cmap_name, cmap_add, used_glyphs, &cmap_obj);
  		else
  		{
  			delete [] cmap_name;
  			if (ttcmap)
  				delete ttcmap;
  					
  			delete sfont;
  			return 0;
  		}
  	}
  }
  	
  if (ttcmap)
  	delete ttcmap;
  		
  if (!cmap_obj.isNull()) 
  {
		XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
	  myref->addObject(&cmap_obj,obj);
  }
  	
  delete [] cmap_name;
  delete sfont;
  	
  return obj;
}

int XWPDFDriver::otfLoadUnicodeCMap(const char *map_name, 
	                       int ttc_index,
		                     const char *otl_tags, 
		                    int wmode)
{
  if (!map_name)
   	return -1;

  if (ttc_index > 999 || ttc_index < 0) 
   	return -1;
    	
   XWFontFileSFNT * sfont = XWFontFileSFNT::load((char*)map_name, ttc_index, false);
  if (!sfont)
  	return -1;
  	
  char * base_name = new char[strlen(map_name)+strlen("-UCS4-H")+5];
  if (wmode)
   	sprintf(base_name, "%s,%03d-UCS4-V", map_name, ttc_index);
  else 
   	sprintf(base_name, "%s,%03d-UCS4-H", map_name, ttc_index);
    	
  char * cmap_name = 0;
  if (otl_tags) 
  {
   	cmap_name = new char[strlen(map_name)+strlen(otl_tags)+strlen("-UCS4-H")+6];
   	if (wmode)
   		sprintf(cmap_name, "%s,%03d,%s-UCS4-V", map_name, ttc_index, otl_tags);
   	else
   		sprintf(cmap_name, "%s,%03d,%s-UCS4-H", map_name, ttc_index, otl_tags);
  } 
  else 
  {
   	cmap_name = new char[strlen(base_name)+1];
   	strcpy(cmap_name, base_name);
  }
  	
  int is_cidfont = 0;
  CIDSysInfo     csi = {0, 0, 0};
  unsigned char *GIDToCIDMap = 0;
  if (sfont->getType() == SFNT_TYPE_POSTSCRIPT) 
   	is_cidfont = handleCIDFont(sfont, &GIDToCIDMap, &csi);
    	
  char * tounicode_name = 0;
  if (!is_cidfont)
  {
   	tounicode_name = new char[strlen(map_name)+strlen("-UTF16")+5];
   	sprintf(tounicode_name, "%s,%03d-UTF16", map_name, ttc_index);
  }
    
  XWFontCache fcache(true);
  int cmap_id = fcache.findCMap(cmap_name);
  if (cmap_id >= 0)
  {
  	delete [] cmap_name;
   	delete [] base_name;
   	if (GIDToCIDMap)
   		free(GIDToCIDMap);
   	if (tounicode_name)
   		delete [] tounicode_name;

   	delete sfont;

   	return cmap_id;
  }
  	
  XWSFNTCmap * ttcmap = sfont->readCmap(3, 10);
  if (!ttcmap)
  {
  	ttcmap = sfont->readCmap(3, 1);
  	if (!ttcmap)
  		sfont->readCmap(0, 3);
  }
  	
  if (!ttcmap)
  {
  	delete [] cmap_name;
   	delete [] base_name;
   	if (GIDToCIDMap)
   		free(GIDToCIDMap);
   	if (tounicode_name)
   		delete [] tounicode_name;

   	delete sfont;
   	return -1;
  }
  	
  cmap_id = loadBaseCMap(base_name, wmode, (is_cidfont ? &csi : NULL), GIDToCIDMap, ttcmap);
  if (cmap_id < 0)
  {
  	delete [] cmap_name;
   	delete [] base_name;
   	if (GIDToCIDMap)
   		free(GIDToCIDMap);
   	if (tounicode_name)
   		delete [] tounicode_name;

	  delete ttcmap;
    delete sfont;
    return -1;
  }
  	
  if (!otl_tags) 
  {
   	delete [] cmap_name;
   	delete [] base_name;
   	if (GIDToCIDMap)
   		free(GIDToCIDMap);
   	if (tounicode_name)
   		delete [] tounicode_name;
      		
   	if (is_cidfont) 
   	{
   		if (csi.registry)
	  		delete [] csi.registry;
    	if (csi.ordering)
		  	delete [] csi.ordering;
    }
    delete ttcmap;
    delete sfont;

    return cmap_id;
  }
  	
  XWCMap * base = fcache.getCMap(cmap_id);
  XWCMap * cmap = new XWCMap;
  cmap->setName(cmap_name);
  cmap->setType(CMAP_TYPE_CODE_TO_CID);
  cmap->setWMode(wmode);
  cmap->setUseCMap(base);
  cmap->addCIDChar(lrange_max, 4, 0);
  	
  if (is_cidfont) 
  {
   	cmap->setCIDSysInfo(&csi);
   	if (csi.registry)
   		delete [] csi.registry;
   	if (csi.ordering)
   		delete [] csi.ordering;
  } 
  else 
   	cmap->setCIDSysInfo(&CSI_IDENTITY);
    	
  XWOTLGsub gsub_list;
  int tounicode_id = -1;
  XWCMap * tounicode = 0;
  {
  	char * conf_name = new char[strlen(otl_tags)+1];
   	memset(conf_name, 0, strlen(otl_tags)+1);
   	char * opt_tag  = (char*)strchr(otl_tags, ':');
    	
   	if (opt_tag) 
   	{
   		opt_tag++;
   		strncpy(conf_name, otl_tags, strlen(otl_tags) - strlen(opt_tag) - 1);
   	} 
   	else 
   		strcpy(conf_name, otl_tags);
      		
   	XWObject conf, opt_conf;   
   	otlConf->findConf((XWPDFDriverRef*)(xref),conf_name, &conf);
   	if (conf.isNull())
   	{
   		delete [] conf_name;
   		delete [] cmap_name;
  		delete [] base_name;
   		if (GIDToCIDMap)
   			free(GIDToCIDMap);
   		if (tounicode_name)
   			delete [] tounicode_name;
      	
   		delete ttcmap;
   		delete sfont;
   		return -1;
   	}
     	
   	loadGsub(&conf, &gsub_list, sfont);
   	if (opt_tag)
   	{
   		if (!XWPDFOTLConf::findOpt(&conf, opt_tag, &opt_conf))
   		{
   			delete [] conf_name;
   			conf.free();
   			delete [] cmap_name;
   			delete [] base_name;
   			if (GIDToCIDMap)
   				free(GIDToCIDMap);
   			if (tounicode_name)
   				delete [] tounicode_name;
      	
   			delete ttcmap;
   			delete sfont;
   			return -1;
   		}
      		
   		loadGsub(&opt_conf, &gsub_list, sfont);
   	}
      	
   	XWPDFGents unencoded;
   	unencoded.handleGsub(&conf, ttcmap, &gsub_list);
   	if (opt_tag)
   	{
   		opt_conf.free();
   		if (!XWPDFOTLConf::findOpt(&conf, opt_tag, &opt_conf))
   		{
   			delete [] conf_name;
   			conf.free();
   			delete [] cmap_name;
  			delete [] base_name;
  			if (GIDToCIDMap)
   				free(GIDToCIDMap);
   			if (tounicode_name)
   				delete [] tounicode_name;
      	
   			delete ttcmap;
   			delete sfont;
   			return -1;
   		}
      		
   		unencoded.handleGsub(&opt_conf, ttcmap, &gsub_list);
   	}
      	
   	opt_conf.free();
   	conf.free();
   	if (is_cidfont) 
   	{
   		tounicode_id = -1;
   		tounicode    = 0;
  	}
   	else
   	{
  		tounicode_id = fcache.findCMap(tounicode_name);
   		if (tounicode_id >= 0)
				tounicode  = fcache.getCMap(tounicode_id);
   		else 
   		{
				tounicode = new XWCMap;
				tounicode->setName(tounicode_name);
				tounicode->setType(CMAP_TYPE_TO_UNICODE);
				tounicode->setWMode(0);
				tounicode->addCodeSpaceRange(srange_min, srange_max, 2);
				tounicode->setCIDSysInfo(&CSI_UNICODE);
				tounicode->addBFChar(srange_min, 2, srange_max, 2);
   		}
   	}
    	
   	unencoded.createCMaps(cmap, tounicode, GIDToCIDMap);
   	delete [] conf_name;
  }
    
  cmap_id = fcache.addCMap(cmap);
  if (!is_cidfont && tounicode_id < 0) /* New */
   	fcache.addCMap(tounicode);

  delete ttcmap;
  	
  if (GIDToCIDMap)
   	free(GIDToCIDMap);
  if (base_name)
   	delete [] base_name;
  if (cmap_name)
   	delete [] cmap_name;
  if (tounicode_name)
   	delete [] tounicode_name;

  delete sfont;

  return cmap_id;
}

XWObject * XWPDFDriver::readToUnicodeFile(const char *cmap_name, XWObject * obj)
{
	XWObject stream;
	stream.initNull();
	if (!strcmp(cmap_name, "Adobe-Identity-UCS2"))
    createDummyCMap(&stream);
  else
  	loadToUnicodeStream(cmap_name, &stream);
	XWPDFDriverRef * myref = (XWPDFDriverRef*)(xref);
	myref->addObject(&stream,obj);
	return obj;
}

int XWPDFDriver::sprintCoord(char *buf, const QPointF & p)
{
  int len  = doubleToAscii(p.x(), precision, buf);
  buf[len++] = ' ';
  len += doubleToAscii(p.y(), precision, buf+len);
  return  len;
}

int XWPDFDriver::sprintLength (char *buf, double value)
{
  return doubleToAscii(value, precision, buf);
}

void XWPDFDriver::stringMode()
{
  switch (motion_state) 
	{
  	default:
    	break;
    		
  	case PDF_GRAPHICS_MODE:
			addPageContent("BT\n", 3);
			break;
  }
  motion_state = PDF_STRING_MODE;
}
