/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWCMap.h"
#include "XWCIDFont.h"
#include "XWFontCache.h"
#include "XWType0Font.h"


static char *
new_used_chars2(void)
{
  	char * used_chars = new char[8192];
  	memset(used_chars, 0, 8192);

  	return used_chars;
}

#define FLAG_NONE              0
#define FLAG_USED_CHARS_SHARED (1 << 0)


XWType0Font::XWType0Font(QObject * parent)
	:QObject(parent)
{
	fontname = 0;
	encoding = 0;
	used_chars = 0;
	descendant = 0;
	flags = 0;
	wmode = 0;
	
	BaseFont = 0;
	locking = false;
}

XWType0Font::~XWType0Font()
{
	if (fontname)
		delete [] fontname;
		
	if (encoding)
		delete [] encoding;
		
	if (!(flags & FLAG_USED_CHARS_SHARED) && used_chars)
		delete [] used_chars;
		
	if (BaseFont)
		delete [] BaseFont;
}

FT_Face  XWType0Font::getFtFace()
{
	if (!descendant)
		return 0;
		
	return descendant->getFtFace();
}

ushort * XWType0Font::getFtToGid()
{
	if (!descendant)
		return 0;
		
	return descendant->getFtToGid();
}

char * XWType0Font::getUsedChars()
{
	locking = true;
	if (!used_chars)
	{
		used_chars = new char[256];
    memset(used_chars, 0, 256 * sizeof(char));
	}
	locking = false;
	return used_chars;
}


#define CACHE_ALLOC_SIZE 16u

XWType0FontCache::XWType0FontCache()
	:count(0),
	 capacity(0),
	 fonts(0)
{
}

XWType0FontCache::~XWType0FontCache()
{
	for (int i = 0; i < count; i++)
	{
		if (fonts[i])
			delete fonts[i];
	}
	
	if (fonts)
		free(fonts);
}

int XWType0FontCache::find(const char *map_name, 
	                       int cmap_id, 
	                       XWTexFontMapOpt *fmap_opt,
	                       int pdf_ver)
{
	if (!map_name || cmap_id < 0 || pdf_ver < 2)
    	return -1;
    	
    XWFontCache cache(true);
    
    XWCMap * cmap = cache.getCMap(cmap_id);
    CIDSysInfo *csi = cmap->isIdentity() ? 0 : cmap->getCIDSysInfo();
    int cid_id = cache.findCIDFont(map_name, csi, fmap_opt, pdf_ver);
    if (cid_id < 0) 
    	return -1;
    	
    XWCIDFont * cidfont = cache.getCIDFont(cid_id);
  	int wmode   = cmap->getWMode();
  	int parent_id = cidfont->getParentID(wmode);
  	if (parent_id >= 0)
    	return parent_id;
    	
    if (count >= capacity) 
    {
    	capacity += CACHE_ALLOC_SIZE;
    	fonts     = (XWType0Font**)realloc(fonts, capacity * sizeof(XWType0Font*));
  	}
  	
  	int font_id =  count;
  	XWType0Font * font = new XWType0Font;
  	fonts[font_id] = font;
  	if (wmode) 
    	font->encoding = qstrdup("Identity-V");
  	else 
    	font->encoding = qstrdup("Identity-H");
  	font->wmode = wmode;
  	font->descendant = cidfont;
  	cidfont->attachParent(font_id, wmode);
  	const char * fontname = (cidfont->isBaseFont()) ? cidfont->getFontName() : cidfont->getDictBaseFont();
  	font->flags = FLAG_NONE;
  	switch (cidfont->getSubtype())
  	{
  		case CIDFONT_TYPE0:
    		font->fontname = new char[strlen(fontname)+strlen(font->encoding)+2];
    		sprintf(font->fontname, "%s-%s", fontname, font->encoding);
    		if ((parent_id = cidfont->getParentID(wmode ? 0 : 1)) < 0)
      			font->used_chars = new_used_chars2();
    		else 
    		{
      			/* Don't allocate new one. */
      			font->used_chars = get(parent_id)->getUsedChars();
      			font->flags     |= FLAG_USED_CHARS_SHARED;
    		}
    		
    		font->BaseFont = qstrdup(font->fontname);
    		break;
    		
    	case CIDFONT_TYPE2:
    		font->used_chars = new_used_chars2();
    		font->BaseFont = qstrdup(fontname);
    		break;
    		
    	default:
    		break;
  	}
  	
  	count++;

  	return font_id;
}

XWType0Font * XWType0FontCache::get(int id)
{
	if (id < 0 || id >= count)
		return 0;
		
	return fonts[id];
}

ushort * XWType0FontCache::getFtToGid(int id)
{
	if (id < 0 || id >= count)
		return 0;
		
	if (!fonts[id] || !(fonts[id]->descendant))
		return 0;
		
	return fonts[id]->getFtToGid();
}

