/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QCoreApplication>
#include "XWFontCache.h"
#include "XWCMap.h"
#include "XWFontFileFT.h"
#include "XWFontFilePK.h"
#include "XWTexFont.h"
#include "XWCIDFont.h"
#include "XWType0Font.h"
#include "XWDVIRef.h"
#include "XWDVICore.h"
#include "XWDVIEncoding.h"
#include "XWDVIFonts.h"

#define CACHE_ALLOC_SIZE  16u

XWDVICIDFont::XWDVICIDFont(XWDVIRef * xref, int font_idA)
{
	font_id = font_idA;
	indirect.initNull();
	
	XWObject obj;
	fontdict.initDict(xref);
	xref->refObj(&fontdict, &indirect);
	XWFontCache fcache(true);
	XWCIDFont * font = fcache.getCIDFont(font_id);
	obj.initName(font->getDictType());
	fontdict.dictAdd(qstrdup("Type"), &obj);	
	obj.initName(font->getDictSubtype());
	fontdict.dictAdd(qstrdup("Subtype"), &obj);	
	obj.initName(font->getDictBaseFont());
	fontdict.dictAdd(qstrdup("BaseFont"), &obj);	
	if (font->getSubtype() != CIDFONT_TYPE2)
	{
		CIDSysInfo * csi = font->getCIDSysInfo();
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
			fontdict.dictAdd(qstrdup("CIDSystemInfo"), &csi_dict);
		}
	}
	
	descriptor.initDict(xref);
	obj.initName("FontDescriptor");
	descriptor.dictAdd(qstrdup("Type"), &obj);	
	obj.initName(font->getDescFontName());
	descriptor.dictAdd(qstrdup("FontName"), &obj);
	
	if (!font->getFlag(CIDFONT_FLAG_TYPE1))
	{
		obj.initReal(font->getDescAscent());
		descriptor.dictAdd(qstrdup("Ascent"), &obj);
		obj.initReal(font->getDescDescent());
		descriptor.dictAdd(qstrdup("Descent"), &obj);
		obj.initReal(font->getStemV());
		descriptor.dictAdd(qstrdup("StemV"), &obj);
		obj.initReal(font->getCapHeight());
		descriptor.dictAdd(qstrdup("CapHeight"), &obj);
		if (font->getXHeight() != 0.0)
		{
			obj.initReal(font->getXHeight());
			descriptor.dictAdd(qstrdup("XHeight"), &obj);
		}
	
		if (font->getAvgWidth() != 0.0)
		{
			obj.initReal(font->getAvgWidth());
			descriptor.dictAdd(qstrdup("AvgWidth"), &obj);
		}
	
		obj.initArray(xref);
		double * bbox = font->getFontBBox();
		XWObject tmp;
		for (int i = 0; i < 4; i++)
		{
			tmp.initReal(bbox[i]);
			obj.arrayAdd(&tmp);
		}
		descriptor.dictAdd(qstrdup("FontBBox"), &obj);
	
		obj.initReal(font->getItalicAngle());
		descriptor.dictAdd(qstrdup("ItalicAngle"), &obj);
	
		obj.initInt(font->getDescFlags());
		descriptor.dictAdd(qstrdup("Flags"), &obj);
	
		XWObject styledict;
		styledict.initDict(xref);
		uchar * panose = font->getPanose();
		obj.initString(new XWString((char*)panose, 12));
		styledict.dictAdd(qstrdup("Panose"), &obj);
		descriptor.dictAdd(qstrdup("Style"), &styledict);
	}
	
	xref->refObj(&descriptor, &obj);
	fontdict.dictAdd(qstrdup("FontDescriptor"), &obj);
}

XWDVICIDFont::~XWDVICIDFont()
{
	indirect.free();
	fontdict.free();
	descriptor.free();
}

void XWDVICIDFont::doFont(XWDVICore * core, XWDVIRef * xref)
{
	if (indirect.isNull())
		return ;
		
	XWFontCache fcache(true);
	XWCIDFont * font = fcache.getCIDFont(font_id);
	if (!font)
		return ;
		
	while (font->isLocking())
	{
		QCoreApplication::sendPostedEvents();
	}
	
	XWCMap * tounicodecmap = 0;
	uchar * streamdata= 0;
	long   streamlen = 0;
	char * usedchars = 0;
	int    lastcid = 0;
	uchar* cidtogidmapA = 0;
	if (font->load(&tounicodecmap, 
			           &streamdata, 
			           &streamlen, 
			           &usedchars, 
			           &lastcid, 
			           &cidtogidmapA) < 0)
	{
		return ;
	}
	
	XWObject obj;
	if (font->getSubtype() == CIDFONT_TYPE2)
	{
		CIDSysInfo * csi = font->getCIDSysInfo();
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
			fontdict.dictAdd(qstrdup("CIDSystemInfo"), &csi_dict);
		}
	}
	else
	{
		if (font->getFlag(CIDFONT_FLAG_TYPE1))
		{
			obj.initReal(font->getCapHeight());
			descriptor.dictAdd(qstrdup("CapHeight"), &obj);
			obj.initReal(font->getDescAscent());
			descriptor.dictAdd(qstrdup("Ascent"), &obj);
			obj.initReal(font->getDescDescent());
			descriptor.dictAdd(qstrdup("Descent"), &obj);
			obj.initReal(font->getItalicAngle());
			descriptor.dictAdd(qstrdup("ItalicAngle"), &obj);
			obj.initReal(font->getStemV());
			descriptor.dictAdd(qstrdup("StemV"), &obj);
			obj.initInt(font->getDescFlags());
			descriptor.dictAdd(qstrdup("Flags"), &obj);
			if (tounicodecmap)
			{
				XWObject tounicode;
				if (core->createCMapStream(tounicodecmap, 0, &tounicode))
				{
					int hparent_id = font->getParentID(0);
    				int vparent_id = font->getParentID(1);
    				int id = hparent_id < 0 ? vparent_id : hparent_id;
    			
					xref->refObj(&tounicode, &obj);
					core->type0T1SetToUnicode(id, &obj);
					xref->releaseObj(&tounicode);
				}
		
				delete tounicodecmap;
			}
		}
	}
	
	obj.initInt(font->getDW());
	fontdict.dictAdd(qstrdup("DW"), &obj);
	
	XWCIDWidths * W = font->getW();
	if (W)
	{
		getCIDWidths(xref, W, &obj);
		XWObject tmp;
		xref->refObj(&obj, &tmp);
		fontdict.dictAdd(qstrdup("W"), &tmp);
		xref->releaseObj(&obj);
	}
	
	W = font->getDW2();
	if (W)
	{
		getCIDWidths(xref, W, &obj);
		XWObject tmp;
		xref->refObj(&obj, &tmp);
		fontdict.dictAdd(qstrdup("DW2"), &tmp);
		xref->releaseObj(&obj);
	}
	
	W = font->getW2();
	if (W)
	{
		getCIDWidths(xref, W, &obj);
		XWObject tmp;
		xref->refObj(&obj, &tmp);
		fontdict.dictAdd(qstrdup("W2"), &tmp);
		xref->releaseObj(&obj);
	}
	
	if (streamdata)
	{
		XWObject fontfile;
		fontfile.initStream(STREAM_COMPRESS, xref);
		xref->refObj(&fontfile, &obj);
		if (font->getSubtype() == CIDFONT_TYPE0)
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
		xref->releaseObj(&fontfile);
	}
	
	if (usedchars)
	{
		XWObject cidset;
		cidset.initStream(STREAM_COMPRESS, xref);
		cidset.streamAdd(usedchars, (lastcid/8)+1);
		xref->refObj(&cidset, &obj);
		xref->releaseObj(&cidset);
		descriptor.dictAdd(qstrdup("CIDSet"), &obj);
	}
		
	if (cidtogidmapA)
	{
		XWObject c2gmstream;
		c2gmstream.initStream(STREAM_COMPRESS, xref);
		c2gmstream.streamAdd((const char*)cidtogidmapA, (lastcid + 1) * 2);
		xref->refObj(&c2gmstream, &obj);
		descriptor.dictAdd(qstrdup("CIDToGIDMap"), &obj);
		xref->releaseObj(&c2gmstream);
		free(cidtogidmapA);
	}
}

void XWDVICIDFont::flush(XWDVIRef * xref, bool finished)
{
	XWObject obj;		
	if (!fontdict.isNull())
	{
		if (finished)
			xref->releaseObj(&fontdict);
		else
		{
			fontdict.copy(&obj);
			xref->releaseObj(&obj);
			fontdict.dictRemove("FontDescriptor");
			if (getSubtype() == CIDFONT_TYPE2)
				fontdict.dictRemove("CIDSystemInfo");
			fontdict.dictRemove("DW");
			fontdict.dictRemove("W");
			fontdict.dictRemove("DW2");
			fontdict.dictRemove("W2");
		}
	}
		
	if (!descriptor.isNull())
	{
		if (finished)
			xref->releaseObj(&descriptor);
		else
		{
			descriptor.copy(&obj);
			xref->releaseObj(&obj);
			if (getFlag(CIDFONT_FLAG_TYPE1))
			{
				descriptor.dictRemove("CapHeight");
				descriptor.dictRemove("Ascent");
				descriptor.dictRemove("Descent");
				descriptor.dictRemove("ItalicAngle");
				descriptor.dictRemove("StemV");
				descriptor.dictRemove("Flags");
			}
			descriptor.dictRemove("FontFile3");
			descriptor.dictRemove("FontFile2");
			descriptor.dictRemove("CIDSet");
			descriptor.dictRemove("CIDToGIDMap");
		}
	}
}

CIDSysInfo  * XWDVICIDFont::getCIDSysInfo()
{
	XWFontCache fcache(true);
	XWCIDFont * font = fcache.getCIDFont(font_id);
	return font->getCIDSysInfo();
}

int  XWDVICIDFont::getEmbedding()
{
	XWFontCache fcache(true);
	XWCIDFont * font = fcache.getCIDFont(font_id);
	return font->getEmbedding();
}

int XWDVICIDFont::getFlag(int mask)
{
	XWFontCache fcache(true);
	XWCIDFont * font = fcache.getCIDFont(font_id);
	return font->getFlag(mask);
}

const char  * XWDVICIDFont::getFontName()
{
	XWFontCache fcache(true);
	XWCIDFont * font = fcache.getCIDFont(font_id);
	return font->getFontName();
}

char * XWDVICIDFont::getIdent()
{
	XWFontCache fcache(true);
	XWCIDFont * font = fcache.getCIDFont(font_id);
	return font->getIdent();
}

int XWDVICIDFont::getOptIndex()
{
	XWFontCache fcache(true);
	XWCIDFont * font = fcache.getCIDFont(font_id);
	return font->getOptIndex();
}

int  XWDVICIDFont::getParentID(int wmode)
{
	XWFontCache fcache(true);
	XWCIDFont * font = fcache.getCIDFont(font_id);
	return font->getParentID(wmode);
}

XWObject *  XWDVICIDFont::getResource(XWDVIRef * xref, XWObject * obj)
{
	if (indirect.isNull())
		xref->refObj(&fontdict, &indirect);
		
	indirect.copy(obj);
	return obj;
}

int  XWDVICIDFont::getSubtype()
{
	XWFontCache fcache(true);
	XWCIDFont * font = fcache.getCIDFont(font_id);
	return font->getSubtype();
}

bool XWDVICIDFont::isACCFont()
{
	XWFontCache fcache(true);
	XWCIDFont * font = fcache.getCIDFont(font_id);
	return font->isACCFont();
}

bool XWDVICIDFont::isBaseFont()
{
	XWFontCache fcache(true);
	XWCIDFont * font = fcache.getCIDFont(font_id);
	return font->isBaseFont();
}

bool XWDVICIDFont::isUCSFont()
{
	XWFontCache fcache(true);
	XWCIDFont * font = fcache.getCIDFont(font_id);
	return font->isUCSFont();
}

XWObject * XWDVICIDFont::getCIDWidths(XWDVIRef * xref, 
	                                  XWCIDWidths * w, 
	                                  XWObject * w_array)
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
			getCIDWidths(xref, tmp, &obj);
			w_array->arrayAdd(&obj);
			obj.initNull();
		}
	}
	
	return w_array;
}

XWDVICIDFontCache::XWDVICIDFontCache()
{
	max  = CACHE_ALLOC_SIZE;
	fonts = (XWDVICIDFont**)malloc(max * sizeof(XWDVICIDFont*));
	num  = 0;
}

XWDVICIDFontCache::~XWDVICIDFontCache()
{
	for (int i = 0; i < num; i++)
	{
		if (fonts[i])
			delete fonts[i];
	}
	
	free(fonts);
}

void XWDVICIDFontCache::close(XWDVICore * core, XWDVIRef * xref, bool finished)
{
	for (int i = 0; i < num; i++)
	{
		XWDVICIDFont * font = fonts[i];
		font->doFont(core, xref);
		font->flush(xref, finished);
	}
}

int XWDVICIDFontCache::find(XWDVIRef * xref,
	                        const char *map_name, 
	                        CIDSysInfo *cmap_csi, 
	                        XWTexFontMapOpt *fmap_opt)
{
	XWFontCache fcache(true);
	int font_id = fcache.findCIDFont(map_name, cmap_csi, fmap_opt, xref->getVersion());
	if (font_id == -1)
		return font_id;
		
	XWDVICIDFont * font = 0;	
	for (int i = 0; i < num; i++)
	{
		font = fonts[i];
		if (font && font->font_id == font_id)
			return i;
	}
		
	font = new XWDVICIDFont(xref, font_id);
	if (num >= max) 
	{
		max  += CACHE_ALLOC_SIZE;
		fonts = (XWDVICIDFont**)realloc(fonts, max * sizeof(XWDVICIDFont*));
    }
    
    fonts[num] = font;
    num++;
    return (num - 1);
}

XWDVICIDFont * XWDVICIDFontCache::get(int font_id)
{
	if (font_id < 0 || font_id >= num)
		return 0;
		
	return fonts[font_id];
}

XWDVIType0Font::XWDVIType0Font(XWDVIRef * xref, 
	                           int font_idA,
	                            XWDVICIDFont * descendantA)
{
	font_id = font_idA;
	
	descendant = descendantA;
	
	indirect.initNull();
	descriptor.initNull();	
	fontdict.initDict(xref);
	XWObject obj;
	obj.initName("Font");
	fontdict.dictAdd(qstrdup("Type"), &obj);
	obj.initName("Type0");
	fontdict.dictAdd(qstrdup("Subtype"), &obj);
	
	XWFontCache fcache(true);
	XWType0Font * font = fcache.getType0Font(font_id);
	
	const char * tmp = font->getBaseFont();
	if (tmp)
	{
		obj.initName(tmp);
		fontdict.dictAdd(qstrdup("BaseFont"), &obj);
	}
	
	tmp = font->getEncoding();
	if (tmp)
	{
		obj.initName(tmp);
		fontdict.dictAdd(qstrdup("Encoding"), &obj);
	}
	
	XWObject array;
	descendant->getResource(xref, &obj);
	array.initArray(xref);
	array.arrayAdd(&obj);
	fontdict.dictAdd(qstrdup("DescendantFonts"), &array);
	xref->refObj(&fontdict, &indirect);
}

XWDVIType0Font::~XWDVIType0Font()
{
	indirect.free();
	fontdict.free();
	descriptor.free();
}

void XWDVIType0Font::doFont(XWDVICore * core)
{
	if (indirect.isNull())
		return ;
		
	XWObject obj;
	fontdict.dictLookupNF("ToUnicode", &obj);
	if (obj.isNull())
	{
		XWFontCache fcache(true);
		XWType0Font * font = fcache.getType0Font(font_id);
		if (!font)
			return ;
		
		addToUnicode(core, font);
	}
	else
		obj.free();
}

void XWDVIType0Font::flush(XWDVIRef * xref, bool finished)
{
	XWObject obj;
	if (!fontdict.isNull())
	{
		if (finished)
			xref->releaseObj(&fontdict);
		else
		{
			fontdict.copy(&obj);
			xref->releaseObj(&obj);
			fontdict.dictRemove("ToUnicode");
		}
	}
		
/*	if (!descriptor.isNull())
	{
		if (finished)
			xref->releaseObj(&descriptor);
		else
		{
			descriptor.copy(&obj);
			xref->releaseObj(&obj);
		}
	}*/
}

FT_Face XWDVIType0Font::getFtFace()
{
	XWFontCache fcache(true);
	XWType0Font * font = fcache.getType0Font(font_id);
	return font->getFtFace();
}

ushort  * XWDVIType0Font::getFtToGid()
{
	XWFontCache fcache(true);
	XWType0Font * font = fcache.getType0Font(font_id);
	return font->getFtToGid();
}

char * XWDVIType0Font::getUsedChars()
{
	XWFontCache fcache(true);
	XWType0Font * font = fcache.getType0Font(font_id);
	return font->getUsedChars();
}

XWObject * XWDVIType0Font::getResource(XWDVIRef * xref, XWObject * obj)
{
	indirect.copy(obj);
	
	return obj;
}

int XWDVIType0Font::getWMode()
{
	XWFontCache fcache(true);
	XWType0Font * font = fcache.getType0Font(font_id);
	return font->getWMode();
}

void XWDVIType0Font::setToUnicode(XWObject *cmap_ref)
{
	fontdict.dictAdd(qstrdup("ToUnicode"), cmap_ref);
}

void XWDVIType0Font::addToUnicode(XWDVICore * core, XWType0Font * font)
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
		core->readToUnicodeFile("Adobe-Identity-UCS2", &tounicode);
		if (tounicode.isNull())
			tounicode.initName("Identity-H");
			
		fontdict.dictAdd(qstrdup("ToUnicode"), &tounicode);
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
      			core->otfCreateToUnicodeStream((const char * )(cidfont->getIdent()),
					                            cidfont->getOptIndex(),
					                            (const char * )(font->getUsedChars()),
					                            &tounicode);
      			break;
      			
      		default:
      			if (cidfont->getFlag(CIDFONT_FLAG_TYPE1C)) 
      			{ /* FIXME */
					core->otfCreateToUnicodeStream((const char * )(cidfont->getIdent()),
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
					core->readToUnicodeFile(cmap_name, &tounicode);
					if (tounicode.isNull()) 
					{
	  					sprintf(cmap_name, "%s-UCS2", fontname);
	  					core->readToUnicodeFile(cmap_name, &tounicode);
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
    	core->readToUnicodeFile(cmap_name, &tounicode);
    	if (tounicode.isNull()) 
    	{
      		sprintf(cmap_name, "%s-%s-UCS2", csi->registry, csi->ordering);
      		core->readToUnicodeFile(cmap_name, &tounicode);
    	}
    	delete [] cmap_name;
    }
    
    if (!tounicode.isNull())
    	fontdict.dictAdd(qstrdup("ToUnicode"), &tounicode);
}

XWDVIType0FontCache::XWDVIType0FontCache()
	:count(0),
	 capacity(0),
	 fonts(0),
	 cidCache(0)
{
}

XWDVIType0FontCache::~XWDVIType0FontCache()
{
	for (int i = 0; i < count; i++)
	{
		if (fonts[i])
			delete fonts[i];
	}
	
	if (fonts)
		free(fonts);
		
	QHashIterator<long, PageFonts*> it(pageFonts);
	while (it.hasNext())
	{
		it.next();
		PageFonts * tmp = it.value();
		if (tmp)
			delete tmp;
	}
	
	pageFonts.clear();
		
	if (cidCache)
		delete cidCache;
}

void XWDVIType0FontCache::addReference(XWDVIRef * xref, int id)
{
	if (id < 0 || id >= count)
		return ;
		
	long page_no = xref->currentPageNumber();
	PageFonts * pfonts = 0;
	if (!pageFonts.contains(page_no))
	{
		pfonts = new PageFonts;
		pageFonts[page_no] = pfonts;
	}
	else
		pfonts = pageFonts[page_no];
			
	if (!(pfonts->ids.contains(id)))
		pfonts->ids << id;
}

void XWDVIType0FontCache::close(XWDVICore * core, XWDVIRef * xref, bool finished)
{
	if (fonts)
	{
		for (int i = 0; i < count; i++)
		{
			XWDVIType0Font * font = fonts[i];
			font->doFont(core);
		}
	}
	
	if (cidCache)
		cidCache->close(core, xref, finished);
		
	if (fonts)
	{
		for (int i = 0; i < count; i++)
		{
			XWDVIType0Font * font = fonts[i];
			font->flush(xref, finished);
		}
	}
}

int XWDVIType0FontCache::find(XWDVIRef * xref, 
	                          const char *map_name, 
	                          int cmap_id, 
	                          XWTexFontMapOpt *fmap_opt)
{
	XWFontCache fcache(true);
	int font_id = fcache.findType0Font(map_name, cmap_id, fmap_opt, xref->getVersion());
	if (font_id == -1)
		return -1;
		
	XWDVIType0Font * font = 0;
    for (int i = 0; i < count; i++)
	{
		font = fonts[i];
		if (font && font->font_id == font_id)
			return i;
	}
	
	if (!cidCache)
		cidCache = new XWDVICIDFontCache;		
	
	XWCMap * cmap = fcache.getCMap(cmap_id);
    CIDSysInfo *csi = cmap->isIdentity() ? 0 : cmap->getCIDSysInfo();
    int cid_id = cidCache->find(xref, map_name, csi, fmap_opt);
    if (cid_id < 0) 
    	return -1;
    	
    XWDVICIDFont * cidfont = cidCache->get(cid_id);	
	font = new XWDVIType0Font(xref, font_id, cidfont);
	if (count >= capacity) 
    {
    	capacity += CACHE_ALLOC_SIZE;
    	fonts     = (XWDVIType0Font**)realloc(fonts, capacity * sizeof(XWDVIType0Font*));
  	}
    
    fonts[count] = font;
    count++;
    return (count - 1);
}

XWDVIType0Font * XWDVIType0FontCache::get(int id)
{
	if (id < 0 || id >= count)
		return 0;
		
	return fonts[id];
}

void XWDVIType0FontCache::load(XWDVICore * core, XWDVIRef * xref, long page_no)
{
	if (!pageFonts.contains(page_no))
		return ;
		
	PageFonts * pfonts = pageFonts[page_no];
	for (int i = 0; i < pfonts->ids.size(); i++)
	{
		XWDVIType0Font * font = fonts[pfonts->ids.at(i)];
		font->doFont(core);
	}
	
	for (int i = 0; i < pfonts->ids.size(); i++)
	{
		XWDVIType0Font * font = fonts[pfonts->ids.at(i)];
		if (font->descendant)
		{
			font->descendant->doFont(core, xref);
			font->descendant->flush(xref, false);
		}
	}
	
	for (int i = 0; i < pfonts->ids.size(); i++)
	{
		XWDVIType0Font * font = fonts[pfonts->ids.at(i)];
		font->flush(xref, false);
//		char * usedchars = font->getUsedChars();
//		if (usedchars)
//			memset(usedchars, 0, 8192);
	}
}

void XWDVIType0FontCache::setToUnicode(int t0t1id, XWObject *cmap_ref)
{
	for (int i = 0; i < count; i++)
	{
		XWDVIType0Font * font = fonts[i];
		if (font->font_id == t0t1id)
		{
			font->setToUnicode(cmap_ref);
			return ;
		}
	}
}

XWDVIFont::XWDVIFont(XWDVIRef * xref, int font_idA, int type0_idA)
{
	font_id = font_idA;
	type0_id = type0_idA;
	 
	reference.initNull();
	resource.initNull();
	descriptor.initNull();
	if (font_id >= 0)
	{
		XWFontCache cache(true);	
		XWTexFont * font = cache.getTexFont(font_id);	
		getResource(xref, font);
		if (font->getSubtype() == PDF_FONT_FONTTYPE_TYPE1C || 
			font->getSubtype() == PDF_FONT_FONTTYPE_TRUETYPE)
		{
			getDescriptor(xref, font);
		}
	}
}

XWDVIFont::~XWDVIFont()
{
	reference.free();
	resource.free();
	descriptor.free();
}

void XWDVIFont::doFont(XWDVICore * core, 
	                   XWDVIRef * xref, 
	                   int hDPI, 
	                   int vDPI)
{
	if (reference.isNull() || font_id < 0)
		return ;
		
	XWFontCache cache(true);	
	XWTexFont * font = cache.getTexFont(font_id);
	if (!font)
		return ;
		
	while (font->isLocking())
	{
		QCoreApplication::sendPostedEvents();
	}
	
	tryLoadToUnicodeCMap(core, xref, font);
	
	switch (font->getSubtype())
	{
		case PDF_FONT_FONTTYPE_TYPE1:
		case PDF_FONT_FONTTYPE_TYPE1C:
			doType1(core, xref, font);
			break;
			
		case PDF_FONT_FONTTYPE_TRUETYPE:
			doTrueType(xref, font);
			break;
			
		case PDF_FONT_FONTTYPE_TYPE3:
			doType3(xref, font, hDPI, vDPI);
			break;
	}
	
	int encoding_idA = font->getEncoding();
	char * used_chars = font->getUsedChars();
	if (encoding_idA >= 0 && font_id >= 0)
      	cache.encodingAddUsedChars(encoding_idA, used_chars);
}

void XWDVIFont::flush(XWDVICore * core, XWDVIRef * xref, bool finished)
{
	if (font_id < 0)
		return ;
		
	XWFontCache cache(true);	
	XWTexFont * font = cache.getTexFont(font_id);	
	XWObject obj;
	if (encoding_id >= 0 && font_id >= 0)
	{
		XWObject *enc_obj = core->getEncodingObj(encoding_id);
		if (enc_obj && !enc_obj->isNull())
		{
			if (enc_obj->isName())
				enc_obj->copy(&obj);
			else
				xref->refObj(enc_obj, &obj);
				
			resource.dictAdd(qstrdup("Encoding"), &obj);
		}
		
		obj.initNull();
		resource.dictLookupNF("ToUnicode", &obj);
		if (!obj.isNull())
		{
			obj.free();
			obj.initNull();
			XWObject * tounicode = core->getEncodingTounicode(encoding_id);
			if (tounicode && !tounicode->isNull())
			{
				xref->refObj(tounicode, &obj);
				resource.dictAdd(qstrdup("ToUnicode"), &obj);
			}
		}
	}
	else if (font->getSubtype() == PDF_FONT_FONTTYPE_TRUETYPE)
	{
		obj.initName("MacRomanEncoding");
		resource.dictAdd(qstrdup("Encoding"), &obj);
	}	
	
	char * fontname = font->getFontName();
	if (!resource.isNull() && !reference.isNull())
	{
		if (font->getSubtype() != PDF_FONT_FONTTYPE_TYPE3)
		{
			if (font->getFlag(PDF_FONT_FLAG_NOEMBED))
			{
				obj.initName(fontname);
				resource.dictAdd(qstrdup("BaseFont"), &obj);
				
				if (!descriptor.isNull())
				{
					obj.initName(fontname);
					descriptor.dictAdd(qstrdup("FontName"), &obj);
				}
			}
			else
			{
				if (!fontname)
					return ;
					
				char * fontnameA = new char[7+strlen(fontname)+1];
				char * uniqueTag = font->getUniqueTag();
				sprintf(fontnameA, "%6s+%s", uniqueTag, fontname);
				obj.initName(fontnameA);
				resource.dictAdd(qstrdup("BaseFont"), &obj);				
				if (!descriptor.isNull())
				{
					obj.initName(fontnameA);
					descriptor.dictAdd(qstrdup("FontName"), &obj);
				}
				
				delete [] fontnameA;
			}
			
			if (!descriptor.isNull())
			{
				xref->refObj(&descriptor, &obj);
				resource.dictAdd(qstrdup("FontDescriptor"), &obj);
			}
		}
	}
	
	if (!resource.isNull())
	{
		if (finished)
			xref->releaseObj(&resource);
		else
		{
			resource.copy(&obj);
			xref->releaseObj(&obj);
			resource.dictRemove("Widths");
			resource.dictRemove("FirstChar");
			resource.dictRemove("LastChar");
			resource.dictRemove("CharProcs");
			resource.dictRemove("Resources");
			resource.dictRemove("FontBBox");
			resource.dictRemove("FontMatrix");
			
			resource.dictRemove("Encoding");
			resource.dictRemove("ToUnicode");
			resource.dictRemove("BaseFont");
			resource.dictRemove("FontDescriptor");
		}
	}
		
	if (!descriptor.isNull())
	{
		if (finished)
    		xref->releaseObj(&descriptor);
    	else
    	{
    		descriptor.copy(&obj);
    		xref->releaseObj(&obj);
    		if (font->getSubtype() != PDF_FONT_FONTTYPE_TYPE1C && 
			    font->getSubtype() != PDF_FONT_FONTTYPE_TRUETYPE)
			{
				descriptor.dictRemove("CapHeight");
				descriptor.dictRemove("XHeight");
				descriptor.dictRemove("Ascent");
				descriptor.dictRemove("Descent");
				descriptor.dictRemove("ItalicAngle");
				descriptor.dictRemove("StemV");
				descriptor.dictRemove("AvgWidth");
				descriptor.dictRemove("Flags");
				descriptor.dictRemove("FontBBox");
			}
			
    		descriptor.dictRemove("FontFile2");
    		descriptor.dictRemove("FontFile3");
    		    		
    		descriptor.dictRemove("FontName");
    	}
    }
}

int XWDVIFont::getEncoding()
{
	if (font_id < 0)
		return encoding_id;
		
	return -1;
}

char * XWDVIFont::getFontName()
{
	if (font_id < 0)
		return 0;
		
	XWFontCache cache(true);	
	XWTexFont * font = cache.getTexFont(font_id);
	return font->getFontName();
}

FT_Face XWDVIFont::getFtFace(XWDVICore * core)
{
	if (type0_id < 0)
		return 0;
		
	XWDVIType0Font * t0font = core->getType0Font(type0_id);
	return t0font->getFtFace();
}

ushort * XWDVIFont::getFtToGid(XWDVICore * core)
{
	if (type0_id < 0)
		return 0;
		
	XWDVIType0Font * t0font = core->getType0Font(type0_id);
	return t0font->getFtToGid();
}

char * XWDVIFont::getIdent()
{
	if (font_id < 0)
		return 0;
		
	XWFontCache cache(true);	
	XWTexFont * font = cache.getTexFont(font_id);
	return font->getIdent();
}

XWObject * XWDVIFont::getReference(XWDVICore * core, XWDVIRef * xref, XWObject * obj)
{
	if (type0_id != -1)
	{
		XWDVIType0Font * t0font = core->getType0Font(type0_id);
		return t0font->getResource(xref, obj);
	}
	
	if (reference.isNull())
	{
		XWFontCache cache(true);	
		XWTexFont * font = cache.getTexFont(font_id);
		getResource(xref, font);
	}
	
	reference.copy(obj);
	
	return obj;
}

int XWDVIFont::getSubtype()
{
	if (font_id < 0)
		return PDF_FONT_FONTTYPE_TYPE0;
		
	XWFontCache cache(true);	
	XWTexFont * font = cache.getTexFont(font_id);
	return font->getSubtype();
}

char * XWDVIFont::getUsedChars(XWDVICore * core)
{
	if (type0_id != -1)
	{
		XWDVIType0Font * t0font = core->getType0Font(type0_id);
		return t0font->getUsedChars();
	}
	
	XWFontCache cache(true);	
	XWTexFont * font = cache.getTexFont(font_id);
	return font->getUsedChars();
}

int XWDVIFont::getWMode(XWDVICore * core)
{
	if (type0_id != -1)
	{
		XWDVIType0Font * t0font = core->getType0Font(type0_id);
		return t0font->getWMode();
	}
	
	return 0;
}

void XWDVIFont::doTrueType(XWDVIRef * xref, XWTexFont * font)
{
	uchar * streamdata = 0;
	long streamlen = 0;
	if (!font->loadTrueType(&streamdata, &streamlen))
		return ;
		
	XWObject obj, obj1;
	obj.initArray(xref);
	int firstchar = font->getFirstChar();
	int lastchar = font->getLastChar();
	double *  widths = font->getDictWidths();
	for (int code = firstchar; code <= lastchar; code++)
	{
		obj1.initReal(widths[code]);
		obj.arrayAdd(&obj1);
	}
	
	xref->refObj(&obj, &obj1);
	resource.dictAdd(qstrdup("Widths"), &obj1);
	xref->releaseObj(&obj);
	obj.initInt(firstchar);
	resource.dictAdd(qstrdup("FirstChar"), &obj);
	obj.initInt(lastchar);
	resource.dictAdd(qstrdup("LastChar"), &obj);
	
	if (streamdata)
	{
		XWObject fontfile;
		fontfile.initStream(STREAM_COMPRESS, xref);
		xref->refObj(&fontfile, &obj);
		descriptor.dictAdd(qstrdup("FontFile2"), &obj);
		obj.initInt(streamlen);
		fontfile.streamGetDict()->add(qstrdup("Length1"), &obj);
		fontfile.streamAdd((const char*)streamdata, streamlen);
		free(streamdata);
		xref->releaseObj(&fontfile);
	}
}

void XWDVIFont::doType1(XWDVICore * core, XWDVIRef * xref, XWTexFont * font)
{
	char ** enc_vec = 0;
	if (font->getEncoding() < 0)
	{
		enc_vec = (char**)malloc(256 * sizeof(char *));
    	for (ushort code = 0; code <= 0xff; code++) 
      		enc_vec[code] = 0;
	}
	else
	{
		XWFontCache cache(true);
		enc_vec = cache.getEncoding(encoding_id);
	}
	
	char fullname[200];
	uchar * streamdata = 0;
	long streamlen = 0;
	
	if (font->getSubtype() == PDF_FONT_FONTTYPE_TYPE1)
	{
		if (!font->loadType1(enc_vec, fullname, &streamdata, &streamlen))
		{
			if (font->getEncoding() < 0 && enc_vec) 
  		{
    		for (ushort code = 0; code < 256; code++) 
    		{
      		if (enc_vec[code])
						delete [] enc_vec[code];
      		enc_vec[code] = 0;
    		}
    		free(enc_vec);
  		}
			return ;
		}
			
		getDescriptor(xref, font);
	}
	else
	{
		if (!font->loadType1C(enc_vec, fullname, &streamdata, &streamlen))
		{
			if (font->getEncoding() < 0 && enc_vec) 
  		{
    		for (ushort code = 0; code < 256; code++) 
    		{
      		if (enc_vec[code])
						delete [] enc_vec[code];
      		enc_vec[code] = 0;
    		}
    		free(enc_vec);
  		}
			return ;
		}
	}
	
	XWObject obj, obj1;
	getResource(xref, font);
	obj.initNull();
	resource.dictLookupNF("ToUnicode", &obj);
	if (obj.isNull())
	{
		char * usedchars = font->getUsedChars();
		if (XWDVIEncoding::createToUnicodeCMap(core, xref, fullname, enc_vec, usedchars, &obj))
		{
			xref->refObj(&obj, &obj1);
			resource.dictAdd(qstrdup("ToUnicode"), &obj1);
			xref->releaseObj(&obj);
		}
	}
	else
		obj.free();
	
	if (font->getEncoding() < 0 && enc_vec) 
  {
   	for (ushort code = 0; code < 256; code++) 
   	{
     		if (enc_vec[code])
			delete [] enc_vec[code];
     		enc_vec[code] = 0;
   	}
   	free(enc_vec);
  }
	
	
	obj.initArray(xref);
	int firstchar = font->getFirstChar();
	int lastchar = font->getLastChar();
	double *  widths = font->getDictWidths();
	for (int code = firstchar; code <= lastchar; code++)
	{
		obj1.initReal(widths[code]);
		obj.arrayAdd(&obj1);
	}
	
	xref->refObj(&obj, &obj1);
	resource.dictAdd(qstrdup("Widths"), &obj1);
	xref->releaseObj(&obj);
	obj.initInt(firstchar);
	resource.dictAdd(qstrdup("FirstChar"), &obj);
	obj.initInt(lastchar);
	resource.dictAdd(qstrdup("LastChar"), &obj);
	
	if (streamdata)
	{
		XWObject fontfile, dict;
		fontfile.initStream(STREAM_COMPRESS, xref);
		xref->refObj(&fontfile, &obj);
		descriptor.dictAdd(qstrdup("FontFile3"), &obj);
		obj.initName("Type1C");
		fontfile.streamGetDict()->add(qstrdup("Subtype"), &obj);
		fontfile.streamAdd((const char*)streamdata, streamlen);
		free(streamdata);
		xref->releaseObj(&fontfile);
	}
}

void XWDVIFont::doType3(XWDVIRef * xref, 
	                    XWTexFont * font, 
	                    int hDPI, 
	                    int vDPI)
{
	XWFontFileFT * ft = 0;
	XWFontFilePK * pk = 0;
	if (font->getT3Type() == PDF_FONT_TYPE3_FT)
	{
		ft = font->loadFT(hDPI, vDPI);
		if (!ft)
			return ;
	}
	else if (font->getT3Type() == PDF_FONT_TYPE3_PK)
	{
		pk = font->loadPK(hDPI);
		if (!pk)
			return ;
	}
	else
		return ;
		
	char * used_chars = font->getUsedChars();
	
	XWObject charprocs, charproc, dict, obj;
	charprocs.initDict(xref);
	
	int firstchar = font->getFirstChar();
	int lastchar = font->getLastChar();
	const uchar * streamdata = 0;
	long streamlen = 0;	
	int encoding_idA = font->getEncoding();
	char    **enc_vec = 0;
	if (encoding_idA >= 0)
	{
		XWFontCache fcache(true);
		enc_vec = fcache.getEncoding(encoding_idA);
	}
	
	char * charname = 0;
	char buf[50];
	for (int code = firstchar; code <= lastchar; code++)
	{
		if (used_chars[code])
		{
			charproc.initStream(STREAM_COMPRESS, xref);
			if (ft)
				streamdata = ft->getCharProcStream(code, hDPI, &streamlen);
			else
				streamdata = pk->getCharProcStream(code, &streamlen);
				
			if (streamlen > 0)
				charproc.streamAdd((const char *)streamdata, streamlen);
				
			if (enc_vec)
				charname = enc_vec[code];
			else
			{
				charname = buf;
				sprintf(charname, "x%02X", (uchar)code);
			}
			
			xref->refObj(&charproc, &obj);
			charprocs.dictAdd(qstrdup(charname), &obj);
			xref->releaseObj(&charproc);
		}
	}
	
	xref->refObj(&charprocs, &obj);
	resource.dictAdd(qstrdup("CharProcs"), &obj);
	xref->releaseObj(&charprocs);
	
	XWObject procset, tmp_array;
	procset.initDict(xref);
	tmp_array.initArray(xref);
	obj.initName("PDF");
	tmp_array.arrayAdd(&obj);
	obj.initName("ImageB");
	tmp_array.arrayAdd(&obj);
	procset.dictAdd(qstrdup("ProcSet"), &tmp_array);
	resource.dictAdd(qstrdup("Resources"), &procset);
	
	tmp_array.initArray(xref);
	for (int code = firstchar; code <= lastchar; code++)
	{
		if (used_chars[code])
		{
			if (enc_vec)
				charname = enc_vec[code];
			else
			{
				charname = buf;
				sprintf(charname, "x%02X", (uchar)code);
			}
			
			obj.initName(charname);
			tmp_array.arrayAdd(&obj);
		}
	}
	
	XWObject encoding;
	encoding.initDict(xref);
	obj.initName("Encoding");
	encoding.dictAdd(qstrdup("Type"), &obj);
	encoding.dictAdd(qstrdup("Differences"), &tmp_array);
	resource.dictAdd(qstrdup("Encoding"), &encoding);
	
	double * fontbbox = font->getDictFontBBox();
	tmp_array.initArray(xref);
	for (int i = 0; i < 4; i++)
	{
		obj.initReal(fontbbox[i]);
		tmp_array.arrayAdd(&obj);
	}
	
	resource.dictAdd(qstrdup("FontBBox"), &tmp_array);
	tmp_array.initArray(xref);
	double *  widths = font->getDictWidths();
	for (int code = firstchar; code <= lastchar; code++)
	{
		if (used_chars[code])
		{
			obj.initReal(widths[code]);
			tmp_array.arrayAdd(&obj);
		}
	}
	
	resource.dictAdd(qstrdup("Widths"), &tmp_array);
	
	tmp_array.initArray(xref);
	double * fontmatrix = font->getFontMatrix();
	for (int i = 0; i < 6; i++)
	{
		obj.initReal(fontmatrix[i]);
		tmp_array.arrayAdd(&obj);
	}
	
	resource.dictAdd(qstrdup("FontMatrix"), &tmp_array);
	obj.initInt(firstchar);
	resource.dictAdd(qstrdup("FirstChar"), &obj);
	obj.initInt(lastchar);
	resource.dictAdd(qstrdup("LastChar"), &obj);
}

void XWDVIFont::getDescriptor(XWDVIRef * xref, XWTexFont * font)
{
	if (descriptor.isNull())
	{
		descriptor.initDict(xref);
		XWObject obj;
		obj.initName("FontDescriptor");
		descriptor.dictAdd(qstrdup("Type"), &obj);
		xref->refObj(&descriptor, &obj);
	}
	
	XWObject obj, obj1;
	obj.initReal(font->getCapHeight());
	descriptor.dictAdd(qstrdup("CapHeight"), &obj);
	if (font->getXHeight() > 0.0)
	{
		obj.initReal(font->getXHeight());
		descriptor.dictAdd(qstrdup("XHeight"), &obj);
	}
	
	obj.initReal(font->getDescAscent());
	descriptor.dictAdd(qstrdup("Ascent"), &obj);
	obj.initReal(font->getDescDescent());
	descriptor.dictAdd(qstrdup("Descent"), &obj);
	obj.initReal(font->getItalicAngle());
	descriptor.dictAdd(qstrdup("ItalicAngle"), &obj);
	obj.initReal(font->getStemV());
	descriptor.dictAdd(qstrdup("StemV"), &obj);
	
	if (font->getAvgWidth() > 0.0)
	{
		obj.initReal(font->getAvgWidth());
		descriptor.dictAdd(qstrdup("AvgWidth"), &obj);
	}
	
	obj.initInt(font->getDescFlags());
	descriptor.dictAdd(qstrdup("Flags"), &obj);
	
	double * fontbbox = font->getDescFontBBox();
	
	obj.initArray(xref);
	for (int i = 0; i < 4; i++)
	{
		obj1.initReal(fontbbox[i]);
		obj.arrayAdd(&obj1);
	}
	
	descriptor.dictAdd(qstrdup("FontBBox"), &obj);
}

void XWDVIFont::getResource(XWDVIRef * xref, XWTexFont * font)
{
	if (resource.isNull())
	{
		resource.initDict(xref);		
		XWObject obj;
		obj.initName("Font");
		resource.dictAdd(qstrdup("Type"), &obj);
		switch (font->getSubtype())
		{
			case PDF_FONT_FONTTYPE_TYPE1:
    		case PDF_FONT_FONTTYPE_TYPE1C:
    			obj.initName("Type1");
    			resource.dictAdd(qstrdup("Subtype"), &obj);
    			break;
    			
    		case PDF_FONT_FONTTYPE_TRUETYPE:
    			obj.initName("TrueType");
    			resource.dictAdd(qstrdup("Subtype"), &obj);
    			break;
    			
    		default:
    			obj.initName("Type3");
    			resource.dictAdd(qstrdup("Subtype"), &obj);
    			break;
		}
		
		xref->refObj(&resource, &reference);
	}
}

int XWDVIFont::tryLoadToUnicodeCMap(XWDVICore * core, XWDVIRef * xref, XWTexFont * font)
{
	if (font->getSubtype() == PDF_FONT_FONTTYPE_TYPE0)
    	return  0;
    	
	char * cmap_name = font->getMapName();
	if (!cmap_name)
		return 0;
		
	XWFontCache fcache(true);
	XWTexFontMap * mrec = fcache.lookupFontMapRecord(cmap_name);
	if (mrec && mrec->opt && mrec->opt->tounicode)
		cmap_name = mrec->opt->tounicode;
		
	if (!cmap_name)
		return 0;
		
	XWObject tounicode, obj;
	tounicode.initNull();
	core->loadToUnicodeStream(cmap_name, &tounicode);
	if (!tounicode.isNull())
	{
		xref->refObj(&tounicode, &obj);
		resource.dictAdd(qstrdup("ToUnicode"), &obj);
		xref->releaseObj(&tounicode);
	}
	
	return 0;
}

XWDVIFontCache::XWDVIFontCache()
	:count(0),
	 capacity(0),
	 fonts(0)
{
	capacity = CACHE_ALLOC_SIZE;
  	fonts    = (XWDVIFont**)malloc(capacity * sizeof(XWDVIFont*));
}

XWDVIFontCache::~XWDVIFontCache()
{
	if (fonts)
	{
		for (int i = 0; i < count; i++)
		{
			XWDVIFont * font = fonts[i];
			if (font)
				delete font;
		}
		
		free(fonts);
	}
	
	QHashIterator<long, PageFonts*> it(pageFonts);
	while (it.hasNext())
	{
		it.next();
		PageFonts * tmp = it.value();
		if (tmp)
			delete tmp;
	}
}

void XWDVIFontCache::close(XWDVICore * core, XWDVIRef * xref, int hDPI, int vDPI, bool finished)
{
	for (int i = 0; i < count; i++)
	{
		XWDVIFont * font = fonts[i];
		if (font)
			font->doFont(core, xref, hDPI, vDPI);
	}
	
	core->encodingComplete(finished);
	
	for (int i = 0; i < count; i++)
	{
		XWDVIFont * font = fonts[i];
		if (font)
			font->flush(core, xref, finished);
	}
}

int XWDVIFontCache::find(XWDVICore * core, 
	                     XWDVIRef * xref, 
				         int hDPI,
	                     const char *tex_name,
		       	         double font_scale, 
		       	         XWTexFontMap *mrec)
{
	int encoding_id = -1, cmap_id = -1;
	char * fontname = mrec ? mrec->font_name : (char*)tex_name;
	XWFontCache fcache(true);
	if (mrec && mrec->enc_name)
	{
#define MAYBE_CMAP(s) (!strstr((s), ".enc") || strstr((s), ".cmap"))
		if (MAYBE_CMAP(mrec->enc_name))
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
				cmap_id = core->otfLoadUnicodeCMap(mrec->font_name,
				                                   mrec->opt->index, mrec->opt->otl_tags,
					                               ((mrec->opt->flags & FONTMAP_OPT_VERT) ? 1 : 0));
				if (cmap_id < 0)
					cmap_id = XWCIDFontCache::t1LoadUnicodeCMap(mrec->font_name, 
						                                        mrec->opt->otl_tags,
												               ((mrec->opt->flags & FONTMAP_OPT_VERT) ? 1 : 0));
				if (cmap_id < 0)
					return -1;
			}
		}
		if (cmap_id < 0)
		{
			encoding_id = core->encodingFindResource(mrec->enc_name);		
			if (encoding_id < 0)
				return -1;
		}
	}
	
	int font_id = -1;
	int type0_id = -1;
	int ret = -1;
	if (mrec && cmap_id >= 0)
	{
		type0_id = core->findType0Font(mrec->font_name, cmap_id, mrec->opt);
		if (type0_id < 0) 
      		return -1;
      		
      	int found = 0;
      	for (int i = 0; i < count; i++)
      	{
      		XWDVIFont * font = fonts[i];
      		if (font->getSubtype() == PDF_FONT_FONTTYPE_TYPE0 && 
      			font->type0_id == type0_id && 
      			font->encoding_id == cmap_id)
      		{
      			found = 1;
      			ret = i;
      			break;
      		}
      	}
      	
      	if (!found)
      	{
      		ret = count;
      		if (count >= capacity) 
      		{
				capacity += CACHE_ALLOC_SIZE;
				fonts     = (XWDVIFont**)realloc(fonts, capacity * sizeof(XWDVIFont*));
      		}
      		
      		XWDVIFont * font = new XWDVIFont(xref, font_id, type0_id);
      		font->encoding_id = cmap_id;
      		fonts[ret] = font;
      		count++;
      	}
	}
	else
	{
		int  found = 0;
		int encoding_idA = -1;
		if (encoding_id >= 0)
		{
			XWDVIEncoding * enc = core->encodingGet(encoding_id);
			encoding_idA = enc->enc_id;
		}
		font_id = fcache.findTexFont(tex_name, fontname, font_scale, encoding_idA, mrec, hDPI);
		if (font_id < 0)
			return -1;
		
		for (int i = 0; i < count; i++)
		{
			XWDVIFont * font = fonts[i];
			if (font->font_id == font_id)
			{
				found = 1;
				ret = i;
				break;
			}
		}
		
		if (!found)
      	{
      		ret = count;
      		if (count >= capacity) 
      		{
				capacity += CACHE_ALLOC_SIZE;
				fonts     = (XWDVIFont**)realloc(fonts, capacity * sizeof(XWDVIFont*));
      		}
      		
      		XWDVIFont * font = new XWDVIFont(xref, font_id, type0_id);
      		font->encoding_id = encoding_id;
      		fonts[ret] = font;
      		count++;
      	}
	}
	
	return ret;
}

XWDVIFont * XWDVIFontCache::getFont(int font_id)
{
	if (font_id < 0 || font_id >= count)
		return 0;
		
	return fonts[font_id];
}

int  XWDVIFontCache::getFontEncoding(int font_id)
{
	if (font_id < 0 || font_id >= count)
		return -1;
		
	return fonts[font_id]->getEncoding();
}

XWObject * XWDVIFontCache::getFontReference(XWDVICore * core, 
	                                        XWDVIRef * xref, 
	                                        int font_id, 
	                                        XWObject * obj)
{
	if (font_id < 0 || font_id >= count)
		return 0;
	
	long page_no = xref->currentPageNumber();
	PageFonts * pfonts = 0;
	if (!pageFonts.contains(page_no))
	{
		pfonts = new PageFonts;
		pageFonts[page_no] = pfonts;
	}
	else
		pfonts = pageFonts[page_no];
			
	if (!(pfonts->ids.contains(font_id)))
		pfonts->ids << font_id;
		
	if (fonts[font_id]->getType0ID() > -1)
		core->addType0FontReference(fonts[font_id]->getType0ID());
				
	return fonts[font_id]->getReference(core, xref, obj);
}

int XWDVIFontCache::getFontSubtype(int font_id)
{
	if (font_id < 0 || font_id >= count)
		return 0;
		
	return fonts[font_id]->getSubtype();
}

char * XWDVIFontCache::getFontUsedChars(XWDVICore * core, int font_id)
{
	if (font_id < 0 || font_id >= count)
		return 0;
	
	return fonts[font_id]->getUsedChars(core);
}

int  XWDVIFontCache::getFontWMode(XWDVICore * core, int font_id)
{
	if (font_id < 0 || font_id >= count)
		return 0;
		
	return fonts[font_id]->getWMode(core);
}

void XWDVIFontCache::load(XWDVICore * core, 
	                      XWDVIRef * xref, 
	                      long page_no, 
	                      int hDPI, 
	                      int vDPI)
{
	if (!pageFonts.contains(page_no))
		return ;
		
	PageFonts * pfonts = pageFonts[page_no];
	for (int i = 0; i < pfonts->ids.size(); i++)
	{
		XWDVIFont * font = fonts[pfonts->ids.at(i)];
		if (font)
			font->doFont(core, xref, hDPI, vDPI);
	}
	
	core->encodingCompletePage();
	
	for (int i = 0; i < pfonts->ids.size(); i++)
	{
		XWDVIFont * font = fonts[pfonts->ids.at(i)];
		if (font && font->getSubtype() != PDF_FONT_FONTTYPE_TYPE0)
		{
			font->flush(core, xref, false);
//			char * usedchars = font->getUsedChars(core);
//			if (usedchars)
//				memset(usedchars, 0, 256);
		}
	}
}

