/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <malloc.h>
#include <string.h>
#include <QString>
#include "XWApplication.h"
#include "XWFontCache.h"
#include "XWFontFileSFNT.h"
#include "XWFontFileCFF.h"
#include "XWSFNTType.h"
#include "XWCFFType.h"
#include "XWAdobeGlyphList.h"
#include "XWType0Font.h"
#include "XWObject.h"
#include "XWDVIRef.h"
#include "XWDVICore.h"
#include "XWDVIEncoding.h"
#include "XWOTLConf.h"
#include "XWDVIResources.h"

#define PDF_RESOURCE_DEBUG_STR "PDF"
#define PDF_RESOURCE_DEBUG     3

#define PDF_RESOURCE_FONT       0
#define PDF_RESOURCE_CIDFONT    1
#define PDF_RESOURCE_ENCODING   2
#define PDF_RESOURCE_CMAP       3
#define PDF_RESOURCE_XOBJECT    4
#define PDF_RESOURCE_COLORSPACE 5
#define PDF_RESOURCE_SHADING    6
#define PDF_RESOURCE_PATTERN    7
#define PDF_RESOURCE_GSTATE     8


static unsigned char srange_min[2] = {0x00, 0x00};
static unsigned char srange_max[2] = {0xff, 0xff};
static unsigned char lrange_min[4] = {0x00, 0x00, 0x00, 0x00};
static unsigned char lrange_max[4] = {0x7f, 0xff, 0xff, 0xff};

#define MAX_UNICODES 16

struct Gent
{
  ushort gid;
  long   ucv; /* assigned PUA unicode */

  int    num_unicodes;
  long   unicodes[MAX_UNICODES];
};

class XWGentEntry
{
public:
	XWGentEntry(char * keyA, int keylenA, Gent * glyphA);
	~XWGentEntry();
	
public:
	char  *key;
  	int    keylen;
  	
  	Gent * glyph;
  	
  	XWGentEntry * next;
};

XWGentEntry::XWGentEntry(char * keyA, int keylenA, Gent * glyphA)
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

XWGentEntry::~XWGentEntry()
{
	if (glyph)
		delete glyph;
}

#define GENT_TABLE_SIZE 503

class XWGents
{
public:
	struct Iterator 
	{
  		int    index;
  		XWGentEntry  * curr;
	};
	
	XWGents();
	~XWGents();
	
	void addGlyph(ushort gid, 
	              long ucv, 
	              int num_unicodes, 
	              long *unicodes);
	
	void createCMaps(XWCMap *cmap, 
	                 XWCMap *tounicode,
	                 unsigned char *GIDToCIDMap);
	
	
	Gent * findGlyph(long ucv);
	
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
	void appendTable(const void *key, int keylen, Gent *value);
	
	void clearIterator();
	
	uint getHash(const void *key, int keylen);
	char * getKey(int *keylen);
	Gent * getVal();
	
	void insertTable(const void *key, int keylen, Gent *value);
	
	XWGentEntry * lookup(const void *key, int keylen);
	Gent * lookupTable(const void *key, int keylen);
	
	bool next();
	
	bool setIterator();
	
private:
	long   count;
	XWGentEntry ** table;
	Iterator * itor;
};

XWGents::XWGents()
{
	table = (XWGentEntry**)malloc(GENT_TABLE_SIZE * sizeof(XWGentEntry*));
	for (int i = 0; i < GENT_TABLE_SIZE; i++)
		table[i] = 0;
		
	itor = 0;
}

XWGents::~XWGents()
{
	for (int i = 0; i < GENT_TABLE_SIZE; i++)
	{
		XWGentEntry * cur = table[i];
		while (cur)
		{
			XWGentEntry * tmp = cur->next;
			delete cur;
			cur = tmp;
		}
	}
	
	free(table);
	
	if (itor)
		delete itor;
}

void XWGents::addGlyph(ushort gid, 
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

  	Gent * glyph = new Gent;
  	glyph->gid = gid;
  	glyph->num_unicodes = num_unicodes;
  	for (int i = 0; i < num_unicodes && i < MAX_UNICODES; i++) 
    	glyph->unicodes[i] = unicodes[i];

  	appendTable(wbuf, 4, glyph);
}

void XWGents::createCMaps(XWCMap *cmap, 
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
    	Gent  * glyph = getVal();
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

Gent * XWGents::findGlyph(long ucv)
{
	uchar wbuf[10];
	wbuf[0] = (ucv >> 24) & 0xff;
  	wbuf[1] = (ucv >> 16) & 0xff;
  	wbuf[2] = (ucv >>  8) & 0xff;
  	wbuf[3] =  ucv & 0xff;

  	return lookupTable(wbuf, 4);
}

int XWGents::handleAssign(XWObject *dst, 
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

int XWGents::handleGsub(XWObject *conf,
	     			     XWSFNTCmap *ttcmap, 
	     			     XWOTLGsub *gsub_list)
{
	if (!conf)
    	return 0;
    	
    XWObject rule;
    XWOTLConf::getRule(conf, &rule);
	if (rule.isNull())
		return 0;
		
	if (!rule.isArray()) 
	{
    	rule.free();
    	return 0;
  	}
  	
  	char * script   = XWOTLConf::getScript(conf);
  	char * language = XWOTLConf::getLanguage(conf);
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

int XWGents::handleSubst(XWObject *dst_obj, 
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
      		Gent * glyph = findGlyph(src);
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

void XWGents::appendTable(const void *key, int keylen, Gent *value)
{
	uint hkey = getHash(key, keylen);
  	XWGentEntry * hent = table[hkey];
  	if (!hent) 
  	{
    	hent = new XWGentEntry((char*)key, keylen, value);
    	table[hkey] = hent;
  	} 
  	else 
  	{
  		XWGentEntry * last = 0;
    	while (hent) 
    	{
      		last = hent;
      		hent = hent->next;
    	}
    	
    	hent = new XWGentEntry((char*)key, keylen, value);
    	last->next = hent;
  	}

  	count++;
}

void XWGents::clearIterator()
{
	if (!itor)
		itor = new Iterator;
		
	itor->index = GENT_TABLE_SIZE;
    itor->curr  = 0;
}

uint XWGents::getHash(const void *key, int keylen)
{
	unsigned int hkey = 0;

  	for (int i = 0; i < keylen; i++) 
    	hkey = (hkey << 5) + hkey + ((char *)key)[i];

  	return (hkey % GENT_TABLE_SIZE);
}

char * XWGents::getKey(int *keylen)
{
	if (!itor)
		setIterator();
		
	XWGentEntry * hent = itor->curr;
  	if (itor && hent) 
  	{
    	*keylen = hent->keylen;
    	return hent->key;
  	} 
  	
  	*keylen = 0;
    return 0;
}

Gent * XWGents::getVal()
{
	if (!itor)
		setIterator();
		
	XWGentEntry * hent = itor->curr;
  	if (itor && hent) 
    	return hent->glyph;
  	
    return 0;
}

void XWGents::insertTable(const void *key, int keylen, Gent *value)
{
	if (!key || keylen < 1)
		return ;
		
	uint hkey = getHash(key, keylen);
  	XWGentEntry * hent = table[hkey];
  	XWGentEntry * prev = 0;
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
  		hent = new XWGentEntry((char*)key, keylen, value);
  		if (prev) 
      		prev->next = hent;
    	else 
      		table[hkey] = hent;
    	count++;
  	}
}

XWGentEntry * XWGents::lookup(const void *key, int keylen)
{
	uint hkey = getHash(key, keylen);
	XWGentEntry * hent = table[hkey];
	while (hent) 
	{
    	if (hent->keylen == keylen && !memcmp(hent->key, key, keylen)) 
      		return hent;
      		
    	hent = hent->next;
  	}

  	return 0;
}

Gent * XWGents::lookupTable(const void *key, int keylen)
{
	XWGentEntry * hent = lookup(key, keylen);
	if (hent)
		return hent->glyph;

  	return 0;
}

bool XWGents::next()
{
	XWGentEntry * hent = itor->curr;
	if (hent)
  		hent = hent->next;
  	while (!hent && (++(itor->index)) < GENT_TABLE_SIZE) 
    	hent = table[itor->index];
  	
  	itor->curr = hent;

  	return (hent ? true : false);
}

bool XWGents::setIterator()
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

class DVIResource
{
public:
	DVIResource();
	~DVIResource();
	
	void flush(XWDVIRef * xrefA, bool finished = true);
	
public:
	char    *ident;
	int      flags;
  	
	int      category;
  	void    *cdata;

  	XWObject object;
  	XWObject reference;
};

DVIResource::DVIResource()
	:ident(0),
	 flags(0),
	 category(-1),
	 cdata(0)
{
	object.initNull();
	reference.initNull();
}

DVIResource::~DVIResource()
{
	if (ident)
		delete [] ident;
		
	object.free();
	reference.free();
}

void DVIResource::flush(XWDVIRef * xrefA, bool finished)
{
	if (!object.isNull())
	{
		if (finished)
			xrefA->releaseObj(&object);
		else
		{
			XWObject o;
			object.copy(&o);
			xrefA->releaseObj(&o);
		}
	}
	reference.free();
	reference.initNull();
}

static struct {
  const char *name;
  int         cat_id;
} pdf_resource_categories[] = {
  {"Font",       PDF_RESOURCE_FONT},
  {"CIDFont",    PDF_RESOURCE_CIDFONT},
  {"Encoding",   PDF_RESOURCE_ENCODING},
  {"CMap",       PDF_RESOURCE_CMAP},
  {"XObject",    PDF_RESOURCE_XOBJECT},
  {"ColorSpace", PDF_RESOURCE_COLORSPACE},
  {"Shading",    PDF_RESOURCE_SHADING},
  {"Pattern",    PDF_RESOURCE_PATTERN},
  {"ExtGState",  PDF_RESOURCE_GSTATE},
};

#define PDF_NUM_RESOURCE_CATEGORIES (sizeof(pdf_resource_categories)/sizeof(pdf_resource_categories[0]))

#define CACHE_ALLOC_SIZE 16u

static int get_category (const char *category)
{
	for (int i = 0; i < PDF_NUM_RESOURCE_CATEGORIES; i++) 
	{
    	if (!strcmp(category, pdf_resource_categories[i].name)) 
      		return pdf_resource_categories[i].cat_id;
  	}

  	return -1;
}

XWDVIResources::XWDVIResources(QObject * parent)
	:QObject(parent)
{
	resources = (ResourceCache*)malloc(PDF_NUM_RESOURCE_CATEGORIES * sizeof(ResourceCache));
	for (int i = 0; i < PDF_NUM_RESOURCE_CATEGORIES; i++)
	{
		resources[i].count     = 0;
    	resources[i].capacity  = 0;
    	resources[i].resources = 0;
	}
}

XWDVIResources::~XWDVIResources()
{
	if (resources)
		free(resources);
}

void XWDVIResources::close(XWDVIRef * xref, bool finished)
{
	for (int i = 0; i < PDF_NUM_RESOURCE_CATEGORIES; i++) 
	{
    	ResourceCache * rc = &resources[i];
    	for (int j = 0; j < rc->count; j++) 
    	{
    		DVIResource * res = rc->resources[j];
    		res->flush(xref, finished);
    		if (finished)
    			delete res;
    	}
    	
    	if (finished)
    	{
    		free(rc->resources);
    		rc->count     = 0;
    		rc->capacity  = 0;
    		rc->resources = 0;
    	}
  	}
}

XWObject * XWDVIResources::createCMapStream(XWDVIRef * xref,
	                                        XWCMap *cmap, 
	                                        int ,
	                                        XWObject * obj)
{
	if (!cmap || !cmap->isValid()) 
	{
    	xwApp->warning("Invalid CMap.\n");
    	return 0;
  	}
  	
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
    		char * n = useCMap->getName();
    		int res_id = findResource("CMap", n);
    		XWObject ucmap_ref;
    		if (res_id >= 0)
    			getResourceReference(xref, res_id, &ucmap_ref);
    		else
    		{
    			XWObject ucmap_obj;
    			if (!createCMapStream(xref, useCMap, 0, &ucmap_obj))
    				return 0;
    				
    			res_id = defineResource(xref, "CMap", n, &ucmap_obj, PDF_RES_FLUSH_IMMEDIATE);
    			getResourceReference(xref, res_id, &ucmap_ref);
    		}
    		
    		obj->streamGetDict()->add(qstrdup("UseCMap"), &ucmap_ref);
    	}
    }
    
    cmap->writeStream(obj->streamGetIO());
    return obj;
}

long XWDVIResources::defineResource(XWDVIRef * xref,
	                                const char *category, 
		    			            const char *resname, 
		    			            XWObject *object, 
		    			            int flags)
{
	int cat_id = get_category(category);
  	if (cat_id < 0) 
  	{
  		QString msg = QString(tr("unknown resource category: %1\n")).arg(category);
    	xwApp->error(msg);
    	return -1;
  	}
  	
  	ResourceCache * rc = &resources[cat_id];
  	int  res_id = 0;
  	if (resname)
  	{
  		for (; res_id < rc->count; res_id++)
  		{
  			DVIResource * res = rc->resources[res_id];
  			if (!strcmp(resname, res->ident))
  			{
  				QString msg = QString(tr("resource %1 (category: %2) already defined...\n"))
  				                        .arg(resname).arg(category);
				xwApp->warning(msg);
  				res->flush(xref);
  				res->flags    = flags;
				if (flags & PDF_RES_FLUSH_IMMEDIATE)
				{
					xref->refObj(object, &(res->reference));
	  				xref->releaseObj(object);
				}
				else
					res->object = *object;
				
				return (long) ((cat_id << 16)|(res_id));
			}
  		}
  	}
  	else
  		res_id = rc->count;
  		
  	if (res_id == rc->count)
  	{
  		if (rc->count >= rc->capacity) 
  		{
      		rc->capacity += CACHE_ALLOC_SIZE;
      		rc->resources = (DVIResource **)realloc(rc->resources, rc->capacity * sizeof(DVIResource *));
    	}
    	
    	DVIResource * res = new DVIResource;
    	rc->resources[res_id] = res;
    	if (resname && resname[0] != '\0')
      		res->ident = qstrdup(resname);
      	res->category = cat_id;
    	res->flags    = flags;
    	if (flags & PDF_RES_FLUSH_IMMEDIATE) 
    	{
      		xref->refObj(object, &(res->reference));
	  		xref->releaseObj(object);
    	} 
    	else 
      		res->object = *object;
    	rc->count++;
  	}
  	
  	return (long) ((cat_id << 16)|(res_id));
}

long XWDVIResources::findResource(const char *category, const char *resname)
{
	int cat_id = get_category(category);
  	if (cat_id < 0) 
  	{
    	QString msg = QString(tr("unknown resource category: %1\n")).arg(category);
    	xwApp->error(msg);
    	return -1;
  	}

  	ResourceCache * rc = &resources[cat_id];
  	for (int res_id = 0; res_id < rc->count; res_id++) 
  	{
    	DVIResource * res = rc->resources[res_id];
    	if (!strcmp(resname, res->ident)) 
      		return (long) (cat_id << 16|res_id);
  	}

  	return -1;
}

XWObject * XWDVIResources::getResourceReference(XWDVIRef * xref, long rc_id, XWObject * ref)
{
	int cat_id = (rc_id >> 16) & 0xffff;
  	int res_id = rc_id & 0xffff;

  	if (cat_id < 0 || cat_id >= PDF_NUM_RESOURCE_CATEGORIES) 
  	{
  		QString msg = QString(tr("invalid category ID: %1\n")).arg(cat_id);
    	xwApp->error(msg);
    	return 0;
  	}
  	
  	ResourceCache * rc  = &resources[cat_id];
  	if (res_id < 0 || res_id >= rc->count) 
  	{
  		QString msg = QString(tr("invalid resource ID: %1\n")).arg(res_id);
    	xwApp->error(msg);
    	return 0;
  	}
  	
  	DVIResource * res = rc->resources[res_id];
  	if (res->reference.isNull()) 
  	{
    	if (res->object.isNull()) 
    	{
    		QString msg = QString(tr("undefined object...\n"));
    		xwApp->error(msg);
    		return 0;
    	} 
    	else 
    		xref->refObj(&(res->object), &(res->reference));
  	}

	res->reference.copy(ref);
  	return ref;
}

XWObject * XWDVIResources::loadToUnicodeStream(XWDVIRef * xref,
	                                           const char *ident,
	                                           XWObject * obj)
{
	XWFontCache fcache(true);
	int id = fcache.findCMap(ident);
	if (id < 0)
		return 0;
		
	XWCMap * cmap = fcache.getCMap(id);
	return createCMapStream(xref, cmap, 0, obj);
}

XWObject * XWDVIResources::readToUnicodeFile(XWDVIRef * xref, 
	                                         const char *cmap_name, 
	                                         XWObject * obj)
{
	XWObject stream;
	stream.initNull();
	int res_id = findResource("CMap", cmap_name);
  	if (res_id < 0) 
  	{
    	if (!strcmp(cmap_name, "Adobe-Identity-UCS2"))
      		createDummyCMap(xref, &stream);
    	else
      		loadToUnicodeStream(xref, cmap_name, &stream);
      		
    	if (!stream.isNull()) 
      		res_id = defineResource(xref, "CMap", cmap_name, &stream, PDF_RES_FLUSH_IMMEDIATE);
  	}

  	return  (res_id < 0 ? NULL : getResourceReference(xref, res_id, obj));
}

XWObject * XWDVIResources::otfCreateToUnicodeStream(XWDVIRef * xref,
	                                                const char *font_name,
			     					                int ttc_index,
			                                        const char *used_glyphs,
			                                        XWObject * obj)
{
	char * cmap_name = new char[strlen(font_name)+strlen("-UTF16")+5];
  	sprintf(cmap_name, "%s,%03d-UTF16", font_name, ttc_index);

  	int res_id = findResource("CMap", cmap_name);
  	if (res_id >= 0) 
  	{
    	delete [] cmap_name;
    	return getResourceReference(xref, res_id, obj);
  	}
  	
  	XWFontFileSFNT * sfont = XWFontFileSFNT::load((char*)font_name, ttc_index, false);
  	if (!sfont)
  	{
  		delete [] cmap_name;
  		QString msg = QString(tr("could not open TrueType font file \"%1\"\n", font_name));
  		xwApp->error(msg);
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
  		createToUnicodeCMap12(xref, (SFNTCmap12*)(ttcmap->map), cmap_name, cmap_add, used_glyphs, &cmap_obj);
  	else
  	{
  		if (ttcmap)
  			delete ttcmap;
  			
  		ttcmap = sfont->readCmap(3, 1);
  		if (ttcmap && ttcmap->format == 4)
  			createToUnicodeCMap4(xref, (SFNTCmap4*)(ttcmap->map), cmap_name, cmap_add, used_glyphs, &cmap_obj);
  		else
  		{
  			if (ttcmap)
  				delete ttcmap;
  				
  			ttcmap = sfont->readCmap(0, 3);
  			if (ttcmap && ttcmap->format == 4)
  				createToUnicodeCMap4(xref, (SFNTCmap4*)(ttcmap->map), cmap_name, cmap_add, used_glyphs, &cmap_obj);
  			else
  			{
  				delete [] cmap_name;
  				if (ttcmap)
  					delete ttcmap;
  					
  				delete sfont;
  				xwApp->error(tr("unable to read OpenType/TrueType Unicode cmap table.\n"));
  				return 0;
  			}
  		}
  	}
  	
  	if (ttcmap)
  		delete ttcmap;
  		
  	if (!cmap_obj.isNull()) 
  	{
    	int res_id = defineResource(xref, "CMap", cmap_name, &cmap_obj, PDF_RES_FLUSH_IMMEDIATE);
    	getResourceReference(xref, res_id, obj);
  	}
  	
  	delete [] cmap_name;
  	delete sfont;
  	
  	return obj;
}

int XWDVIResources::otfLoadUnicodeCMap(XWDVICore * core,
	                                   XWDVIRef * xref,
	                                   const char *map_name, 
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
  	{
  		QString msg = QString(tr("could not open OpenType/TrueType/dfont font file \"%1\"\n")).arg(map_name);
  		xwApp->error(msg);
  		return -1;
  	}
  	
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
      	core->otlFindConf(conf_name, &conf);
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
      		if (!XWOTLConf::findOpt(&conf, opt_tag, &opt_conf))
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
      	
      	XWGents unencoded;
      	unencoded.handleGsub(&conf, ttcmap, &gsub_list);
      	if (opt_tag)
      	{
      		opt_conf.free();
      		if (!XWOTLConf::findOpt(&conf, opt_tag, &opt_conf))
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

XWObject * XWDVIResources::createDummyCMap(XWDVIRef * xref, XWObject * obj)
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

XWObject * XWDVIResources::createToUnicodeCMap4(XWDVIRef * xref,
	                                            SFNTCmap4 *map,
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
  		createCMapStream(xref, &cmap, 0, obj);
  		
  	return obj;
}

XWObject * XWDVIResources::createToUnicodeCMap12(XWDVIRef * xref,
	                                             SFNTCmap12 * map,
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
  		createCMapStream(xref, &cmap, 0, obj);
  		
  	return obj;
}

int XWDVIResources::handleCIDFont(XWFontFileSFNT *sfont,
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
  	{
  		xwApp->error(tr("no glyph contained in this font...\n"));
  		return -1;
  	}
  	
  	QIODevice * fp = sfont->getFile();
  	XWFontFileCFF * cffont = XWFontFileCFF::make(fp, 0, offset, (ulong)(fp->size()) - offset, 0);
  	if (!cffont)
  	{
    	xwApp->error(tr("could not open CFF font...\n"));
    	return -1;
    }
    
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
    	xwApp->error(tr("no CIDSystemInfo???\n"));
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
    	xwApp->error(tr("no CFF charset data???\n"));
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
    		xwApp->error(tr("unknown CFF charset format...\n"));
    		return -1;
    		break;
  	}
  	
  	delete cffont;

  	*GIDToCIDMap = map;
  	return 1;
}

ushort XWDVIResources::handleSubstGlyphs(XWCMap *cmap, 
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

int XWDVIResources::loadBaseCMap(const char *cmap_name, 
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

int XWDVIResources::loadGsub(XWObject *conf, 
	                         XWOTLGsub *gsub_list, 
	                         XWFontFileSFNT *sfont)
{
	XWObject rule;
	XWOTLConf::getRule(conf, &rule);
	if (rule.isNull())
		return 0;
    
	char * script   = XWOTLConf::getScript(conf);
  	char * language = XWOTLConf::getLanguage(conf);
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

