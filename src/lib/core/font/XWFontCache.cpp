/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QMutex>
#include "XWEncoding.h"
#include "XWAdobeGlyphList.h"
#include "XWTexFont.h"
#include "XWTexFontMap.h"
#include "XWCIDFont.h"
#include "XWType0Font.h"
#include "XWFontCache.h"

static QMutex fontCacheMutex;

class XWFontCachePrivate
{
public:
	XWFontCachePrivate();
	~XWFontCachePrivate();
	
	void lock();
	
public:
	XWTexFontCache * texFontCache;
	
	XWTexFontMaps * texFontMaps;
	
	XWAdobeGlyphList * agl;
	XWCMapCache  * cmapCache;
	XWEncodingCache * encodings;
	
	XWCIDFontCache * cidFontCache;
	XWType0FontCache * t0Cache;
};

XWFontCachePrivate::XWFontCachePrivate()
{
	texFontCache = new XWTexFontCache;
	
	texFontMaps = new XWTexFontMaps;
	
	agl = new XWAdobeGlyphList;	
	cmapCache = new XWCMapCache(1);
	encodings = new XWEncodingCache;
	
	cidFontCache = new XWCIDFontCache;
	t0Cache = new XWType0FontCache;
}

XWFontCachePrivate::~XWFontCachePrivate()
{
	delete texFontCache;
	delete texFontMaps;
	delete agl;
	delete cmapCache;
	delete encodings;
	delete cidFontCache;
	delete t0Cache;
}

static XWFontCachePrivate * fontCaches = 0;

XWFontCache::XWFontCache(bool pdfA, QObject * parent)
	:QObject(parent),
	 pdf(pdfA)
{
	if (!fontCaches)
		fontCaches = new XWFontCachePrivate;
}

int XWFontCache::addCMap(XWCMap * cmap)
{
	int ret = fontCaches->cmapCache->add(cmap);
	return ret;
}

int XWFontCache::addTexFont(XWTexFont * font)
{
	int ret = fontCaches->texFontCache->add(font);
	return ret;
}

int XWFontCache::aglGetUnicodes(const char *glyphstr,
			       		        long *unicodes, 
			       		        int max_uncodes)
{
	int ret = fontCaches->agl->getUnicodes(glyphstr, unicodes, max_uncodes);
	return ret;
}

XWAdobeGlyphName * XWFontCache::aglLookupList(const char *glyphname)
{
	XWAdobeGlyphName * ret = fontCaches->agl->lookupList(glyphname);
	return ret;
}

long  XWFontCache::aglSPutUTF16BE(const char *name,
			       	              uchar **dstpp,
			       		          uchar *limptr, 
			       		          int *num_fails)
{
	long ret = fontCaches->agl->sputUTF16BE(name, dstpp, limptr, num_fails);
	return ret;
}

int XWFontCache::appendFontMapRecord(const char *kp, const XWTexFontMap *vp)
{
	int ret = fontCaches->texFontMaps->appendRecord(kp, vp);
	return ret;
}

void XWFontCache::close()
{
	fontCacheMutex.lock();
	if (fontCaches)
		delete fontCaches;
		
	fontCaches = 0;
	fontCacheMutex.unlock();
}

void XWFontCache::encodingAddUsedChars(int encoding_id, const char *is_used)
{
	fontCaches->encodings->addUsedChars(encoding_id, is_used);
}

XWEncoding * XWFontCache::encodingGet(int enc_id)
{
	XWEncoding * ret = fontCaches->encodings->get(enc_id);
	return ret;
}

void XWFontCache::encodingUsedByType3(int enc_id)
{
	fontCaches->encodings->usedByType3(enc_id);
}

int XWFontCache::findCIDFont(const char *map_name, 
	                         CIDSysInfo *cmap_csi, 
	                         XWTexFontMapOpt *fmap_opt,
	                         int pdf_ver)
{
	int ret = fontCaches->cidFontCache->find(map_name, cmap_csi, fmap_opt, pdf_ver);
	return ret;
}

int XWFontCache::findCMap(const char *cmap_name)
{
	int ret = fontCaches->cmapCache->find(cmap_name);
	return ret;
}

int XWFontCache::findEncoding(char *enc_name)
{
	int ret = fontCaches->encodings->findResource(enc_name);
	return ret;
}

int XWFontCache::findTexFont(const char *texname,
	                         const char *fontname,
		       	             double font_scale, 
		       	             int    encoding_id,
		       	             XWTexFontMap *mrec,
		       	             int base_dpi)
{
	int ret = fontCaches->texFontCache->find(texname, fontname, font_scale, encoding_id, mrec, base_dpi);
	return ret;
}

int XWFontCache::findType0Font(const char *map_name, 
	                           int cmap_id, 
	                           XWTexFontMapOpt *fmap_opt,
	                           int pdf_ver)
{
	int ret = fontCaches->t0Cache->find(map_name, cmap_id, fmap_opt, pdf_ver);
	return ret;
}

XWCIDFont * XWFontCache::getCIDFont(int fnt_id)
{
	XWCIDFont * ret = fontCaches->cidFontCache->get(fnt_id);
	return ret;
}

XWCMap * XWFontCache::getCMap(int id)
{
	XWCMap * ret = fontCaches->cmapCache->get(id);
	return ret;
}

char ** XWFontCache::getEncoding(int enc_id)
{
	char ** ret = fontCaches->encodings->getEncoding(enc_id);
	return ret;
}

char * XWFontCache::getEncodingName(int enc_id)
{
	char * ret = fontCaches->encodings->getName(enc_id);
	return ret;
}

XWTexFont * XWFontCache::getTexFont(int id)
{
	XWTexFont * ret = fontCaches->texFontCache->get(id);
	return ret;
}

int XWFontCache::getTexFontEncoding(int font_id)
{
	int ret = fontCaches->texFontCache->getFontEncoding(font_id);
	return ret;
}

int  XWFontCache::getTexFontSubtype(int font_id)
{
	int ret = fontCaches->texFontCache->getFontSubtype(font_id);
	return ret;
}

char * XWFontCache::getTexFontUsedChars(int font_id)
{
	char * ret = fontCaches->texFontCache->getFontUsedChars(font_id);
	return ret;
}

int XWFontCache::getTexFontWMode(int font_id)
{
	int ret = fontCaches->texFontCache->getFontWMode(font_id);
	return ret;
}

XWType0Font * XWFontCache::getType0Font(int id)
{
	XWType0Font * ret = fontCaches->t0Cache->get(id);
	return ret;
}

void XWFontCache::init()
{
	if (!fontCaches)
		fontCaches = new XWFontCachePrivate;
}

int XWFontCache::insertFontMapRecord(const char  *kp, const XWTexFontMap *mrec)
{
	int ret = fontCaches->texFontMaps->insertRecord(kp, mrec);
	return ret;
}

int XWFontCache::isPredefinedEncoding(int enc_id)
{
	int ret = fontCaches->encodings->isPredefined(enc_id);
	return ret;
}

int XWFontCache::loadFontMapFile(const char  *filename, int mode)
{
	int ret = fontCaches->texFontMaps->loadFile(filename, mode, false);
	return ret;
}

int XWFontCache::loadNativeFont(const char *ps_name,
                                const char *fam_name, 
                                const char *sty_name,
                                int layout_dir)
{
	int ret = fontCaches->texFontMaps->loadNativeFont(ps_name, fam_name, sty_name, layout_dir);
	return ret;
}

XWTexFontMap * XWFontCache::lookupFontMapRecord (const char  *kp)
{
	XWTexFontMap * ret = fontCaches->texFontMaps->lookupRecord(kp);
	return ret;
}

int XWFontCache::removeFontmapRecord(const char  *kp)
{
	int ret = fontCaches->texFontMaps->removeRecord(kp);
	return ret;
}
