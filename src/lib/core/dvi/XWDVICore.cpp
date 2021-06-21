/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include "XWUtil.h"
#include "XWApplication.h"
#include "XWNumberUtil.h"
#include "XWLexer.h"
#include "XWCMap.h"
#include "XWCIDFont.h"
#include "XWSubfont.h"
#include "XWTFMFile.h"
#include "XWTexFontMap.h"
#include "XWFontCache.h"
#include "XWDVIRef.h"
#include "XWDVIDev.h"
#include "XWDVICore.h"
#include "XWDVIParser.h"
#include "XWFontFileVF.h"
#include "XWDVIResources.h"
#include "XWDVIEncoding.h"
#include "XWOTLConf.h"
#include "XWDVIFonts.h"
#include "XWPDFNames.h"
#include "XWDVIColorSpaces.h"
#include "XWDVIImage.h"
#include "XWPDFFile.h"
#include "XWMPost.h"
#ifdef XW_BUILD_PS_LIB
#include  "XWPSInterpreter.h"
#endif //XW_BUILD_PS_LIB
#include "XWDVISpecial.h"

#define OPT_TPIC_TRANSPARENT_FILL (1 << 1)
#define OPT_CIDFONT_FIXEDPITCH    (1 << 2)
#define OPT_FONTMAP_FIRST_MATCH   (1 << 3)
#define OPT_PDFDOC_NO_DEST_REMOVE (1 << 4)

#define TEX_FONTS_ALLOC_SIZE 16u
#define VF_NESTING_MAX       16u


#define PHYSICAL 1
#define VIRTUAL  2
#define SUBFONT  3
#define NATIVE   4
#define DVI      1
#define VF       2


/* reserved keys */
static const char *_rkeys[] = {
#define  K_OBJ__XPOS      0
#define  K_OBJ__YPOS      1
  "xpos", "ypos",
#define  K_OBJ__THISPAGE  2
#define  K_OBJ__PREVPAGE  3
#define  K_OBJ__NEXTPAGE  4
  "thispage", "prevpage", "nextpage",
#define  K_OBJ__RESOURCES 5
  "resources",
#define  K_OBJ__PAGES     6
#define  K_OBJ__NAMES     7
  "pages", "names",
#define  K_OBJ__CATALOG   8
#define  K_OBJ__DOCINFO   9
  "catalog", "docinfo",
#if  0
#define  K_OBJ__TRAILER  10
  "trailer",
#endif /* NYI */
  NULL
};

static int
ispageref (const char *key)
{
  	const char  *p;
  	if (strlen(key) <= strlen("page") ||
      	memcmp(key, "page", strlen("page")))
    	return  0;
  	else 
  	{
    	for (p = key + 4; *p && *p >= '0' && *p <= '9'; p++);
    	if (*p != '\0')
      		return  0;
  	}
  	return  1;
}


HTMLState::HTMLState()
{
	opts.extensions = 0;
	link_dict.initNull();
	baseurl = 0;
	pending_type = -1;
}

HTMLState::~HTMLState()
{
	if (baseurl)
		delete [] baseurl;
}

void HTMLState::clear(XWDVIRef * xrefA)
{
	if (!link_dict.isNull())
		xrefA->releaseObj(&link_dict);
	if (baseurl)
		delete [] baseurl;
  	baseurl      = 0;
  	pending_type = -1;
}

#define RES_MAP_SIZE 503


ResourceMap::ResourceMap(const char * keyA, int keylenA, int typeA, int res_idA)
{
	if (keyA)
	{
		key = qstrdup(keyA);
		keylen = keylenA;
	}
	else
	{
		key = 0;
		keylen = 0;
	}
	
	type = typeA;
	res_id = res_idA;
	next = 0;
}

ResourceMap::~ResourceMap()
{
	if (key)
		delete [] key;
}

ResourceMaps::ResourceMaps()
{
	table = (ResourceMap**)malloc(RES_MAP_SIZE * sizeof(ResourceMap*));
	for (int i = 0; i < RES_MAP_SIZE; i++)
		table[i] = 0;
}

ResourceMaps::~ResourceMaps()
{
	for (int i = 0; i < RES_MAP_SIZE; i++)
	{
		ResourceMap * cur = table[i];
		while (cur)
		{
			ResourceMap * tmp = cur->next;
			delete cur;
			cur = tmp;
		}
	}
	
	free(table);
}

void ResourceMaps::appendTable(const char * ident, int typeA, int res_idA)
{
	const char * key = ident;
	int keylen = strlen(key);
	uint hkey = getHash(key, keylen);
  	ResourceMap * hent = table[hkey];
  	if (!hent) 
  	{
    	hent = new ResourceMap(key, keylen, typeA, res_idA);
    	table[hkey] = hent;
  	} 
  	else 
  	{
  		ResourceMap * last = 0;
    	while (hent) 
    	{
      		last = hent;
      		hent = hent->next;
    	}
    	
    	hent = new ResourceMap(key, keylen, typeA, res_idA);
    	last->next = hent;
  	}

  	count++;
}

ResourceMap * ResourceMaps::lookup(const void *key, int keylen)
{
	uint hkey = getHash(key, keylen);
	ResourceMap * hent = table[hkey];
	while (hent) 
	{
    	if (hent->keylen == keylen && !memcmp(hent->key, key, keylen)) 
      		return hent;
      		
    	hent = hent->next;
  	}

  	return 0;
}

uint ResourceMaps::getHash(const void *key, int keylen)
{
	unsigned int hkey = 0;

  	for (int i = 0; i < keylen; i++) 
    	hkey = (hkey << 5) + hkey + ((char *)key)[i];

  	return (hkey % RES_MAP_SIZE);
}

static const char *default_taintkeys[] = {
    "Title",   "Author",   "Subject", "Keywords",
    "Creator", "Producer", "Contents", "Subj",
    "TU",      "T",        "TM",        NULL /* EOD */
};

PDFState::PDFState(XWDVIRef * xrefA)
{
	annot_dict.initNull();
	lowest_level = 255;
	cd.cmap_id = -1;
	cd.unescape_backslash = 0;
	resourcemap = new ResourceMaps;
	cd.taintkeys.initArray(xrefA);
	XWObject obj;
	for (int i = 0; default_taintkeys[i] != NULL; i++) 
	{
		obj.initName(default_taintkeys[i]);
    	cd.taintkeys.arrayAdd(&obj);
  	}
}

PDFState::~PDFState()
{
	annot_dict.free();
		
	if (resourcemap)
		delete resourcemap;
		
	cd.taintkeys.free();
}

int PDFState::addResource(XWDVICore * coreA, const char *ident, int res_id)
{
	if (!ident || res_id < 0)
    	return  -1;
    	
    resourcemap->appendTable(ident, 0, res_id);
    
    XWObject obj;
    if (!coreA->getImageReference(res_id, &obj))
    	return -1;
    	
    coreA->pushObject(ident, &obj);
    return 0;
}

void PDFState::clean(XWDVIRef * xrefA)
{
	xrefA->releaseObj(&annot_dict);
	if (resourcemap)
		delete resourcemap;
		
	resourcemap = 0;
	lowest_level = 255;		
	if (!cd.taintkeys.isNull())
		xrefA->releaseObj(&(cd.taintkeys));
}

int PDFState::findResource(const char *ident)
{
	ResourceMap * r = resourcemap->lookup(ident, strlen(ident));
	return (r ? r->res_id : -1);
}

int PDFState::modStrings(const char *kp, XWObject *vp)
{
	if (cd.cmap_id < 0 || cd.taintkeys.isNull())
    	return  -1;
    	
    XWFontCache fcache(true);
    XWCMap * cmap = 0;
    int r = 0;
    switch (vp->getType()) 
    {
  		case XWObject::String:
    		cmap = fcache.getCMap(cd.cmap_id);
    		if (needReencode(kp, vp)) 
      			r = reencodeString(cmap, vp);
    		break;
    		
  		case XWObject::Dict:
  			{
  				int len = vp->dictGetLength();
  				XWObject obj;
  				obj.initNull();
  				for (int i = 0; i < len; i++)
  				{
  					char * key = vp->dictGetKey(i);
  					XWObject * obj = vp->dictGetValOrg(i);
  					modStrings(key, obj);
  				}
  			}
    		break;
    		
  		case XWObject::Stream:
  			{
  				XWObject dict;
  				dict.initDict(vp->streamGetDict());
  				int len = dict.dictGetLength();
  				for (int i = 0; i < len; i++)
  				{
  					char * key = dict.dictGetKey(i);
  					XWObject * obj = dict.dictGetValOrg(i);
  					modStrings(key, obj);
  				}
  				
  				dict.free();
  			}
    		break;
  	}

  	return  r;
}

int PDFState::needReencode(const char *k, XWObject *vp)
{
	int r = 0;
	XWObject tk;
	int len = cd.taintkeys.arrayGetLength();
	for (int i = 0; i < len; i++) 
	{
		tk.initNull();
		cd.taintkeys.arrayGet(i, &tk);
    	if (!strcmp(k, tk.getName())) 
    	{
      		r = 1;
      		break;
    	}
  	}
  	
  	if (r) 
  	{
  		XWString * str = vp->getString();
    	if (str->getLength() >= 2 && !memcmp(str->getCString(), "\xfe\xff", 2))
      		r = 0;
  	}

  	return  r;
}

XWObject * PDFState::parsePDFDict(XWDVICore * coreA,
	                              XWDVIRef * xrefA,
	                              XWLexer * lexer, 
	                              XWObject * dict)
{
	XWDVIParser parser(coreA, xrefA, lexer, 0);
	if (cd.cmap_id < 0)
    	return  parser.parsePDFDict(dict);
    
    XWObject * ret = 0;
    if (cd.unescape_backslash) 
    	ret = parser.parsePDFTaintedDict(dict);
  	else 
    	ret = parser.parsePDFDict(dict);
    	
  	if (ret)
  	{
  		int len = dict->streamGetLength();
  		for (int i = 0; i < len; i++)
  		{
  			char * key = dict->dictGetKey(i);
  			XWObject * obj = dict->dictGetValOrg(i);
  			modStrings(key, obj);
  		}  		
    }

  	return  ret;
}

int PDFState::reencodeString(XWCMap *cmap, XWObject *instring)
{
	if (!cmap || !instring)
    	return 0;
    	
    XWString * str = instring->getString();
    long inbufleft = str->getLength();
    uchar * inbufcur  = (uchar*)(str->getCString());
    
    uchar  wbuf[4096];
    wbuf[0]  = 0xfe;
  	wbuf[1]  = 0xff;
  	uchar * obufcur  = wbuf + 2;
  	long obufleft = 4096 - 2;
  	cmap->decode((const uchar **)&inbufcur, &inbufleft, &obufcur, &obufleft);
  	
  	if (inbufleft > 0) 
    	return  -1;

	instring->free();
	instring->initString(new XWString((char*)wbuf, 4096 - obufleft));

  	return  0;
}


TPICState::TPICState()
{
	pen_size = 1.0;
	mode.fill = 0;
	num_points = 0;
  	max_points = 0;
  	fill_shape = 0;
  	fill_color = 0.0;
  	points = 0;
}

TPICState::~TPICState()
{
	if (points)
		free(points);
}

void TPICState::clear()
{
	if (points)
		free(points);
	points = 0;
	mode.fill = TPIC_MODE__FILL_SOLID;
	num_points = 0;
  	max_points = 0;
  	fill_shape = 0;
  	fill_color = 0.0;
}

XWDVICore::XWDVICore(XWDVIRef * xrefA, int mpmodeA, QObject * parent)
	:QObject(parent),
	 xref(xrefA),
	 mpMode(mpmodeA),
	 dev(0)
{
	optFlags = 0;
	mag = 1.0;
	hDPI = 600;
	vDPI = 600;
	keyBits = 40;
	permission  = 0x003C;
	
	paperWidth = 595.0;
	paperHeight = 842.0;
	pageWidth = paperWidth;
  pageHeight = paperHeight;
	x_offset = 72.0;
	y_offset = 72.0;
	landscapeMode = 0;	
	
	dviInfo.unitNum = 25400000;
  dviInfo.unitDen = 473628672;
  dviInfo.mag = 1000;
  dviInfo.mediaWidth = 0;
  dviInfo.mediaHeight = 0;
  dviInfo.stackDepth = 0;
  dviInfo.comment[0] = '\0';
  devOriginX = 72.0;
  devOriginY = 770.0;
  loadedFonts = 0;
  numLoadedFonts = 0;
  maxLoadedFonts = 0;
  defFonts = 0;
  numDefFonts = 0;
  maxDefFonts = 0;
  computeBoxes = 0;
  linkAnnot = 1;
  pageNo = -1;
  	
  dvi2pts = 1.52018;
  totalMag = 1.0;  	
  	
  markedDepth = 0;
  taggedDepth = -1;
  processingPage = 0;
  namedObjects = 0;
  	
  numSavedFonts = 0;
  	
  pageAttrs = 0;
  pageAttrCounter = 0;
  	
  resources = 0;
  colorSpaces = 0;
  otlconf = 0;
  encodings = 0;
  type0FontCache = 0;
  dviFontCache = 0;
  imageCache = 0;
  namedObjects = 0;
  	
  _html_state = 0;  	
  _pdf_stat = 0;
  _tpic_state = 0;
  
#ifdef XW_BUILD_PS_LIB
  usesspecial = false;
  lastclose = false;
  endscanning = false;
	interpreter = 0;
	bangspecialsFile = 0;
#endif //XW_BUILD_PS_LIB
  
  clearState();
  
  totalMag = (double) dviInfo.mag / 1000.0 * mag;
  dvi2pts  = (double) dviInfo.unitNum / (double) dviInfo.unitDen;
  dvi2pts  *= (72.0 / 254000.0);	
  	
  dev = new XWDVIDev(this, xref);
  dev->setDvi2Pts(dvi2pts);
  	
  if (!mpMode)
  {
   	PDFRect mediabox;
   	mediabox.llx = 0.0;
  	mediabox.lly = 0.0;
  	mediabox.urx = paperWidth;
  	mediabox.ury = paperHeight;
  	xref->setMediaBox(0, &mediabox);
  }
}

XWDVICore::~XWDVICore()
{	
	clear();
}

#ifdef XW_BUILD_PS_LIB

void XWDVICore::addBangSpecial(const char *buffer, long sizeA)
{
	if (!bangspecialsFile)
	{
		bangspecialsFile = new QBuffer;
		bangspecialsFile->open(QBuffer::ReadWrite | QIODevice::Truncate);
		bangspecialsFile->write("TeXDict begin @defspecial\n");
	}
	
	bangspecialsFile->write(buffer, sizeA);
	bangspecialsFile->write("\n");
}

#endif //XW_BUILD_PS_LIB

void XWDVICore::addType0FontReference(int id)
{
	if (type0FontCache)
		type0FontCache->addReference(xref, id);
}

void XWDVICore::beginDoc()
{
	clearState();
	initDev();
  xref->open();
}

void XWDVICore::clear()
{
	xref->clear();
	dev->clear(xref);
	
	if (_tpic_state)
		delete _tpic_state;
		
	if (_pdf_stat)
		delete _pdf_stat;
		
	if (_html_state)
		delete _html_state;
		
	if (namedObjects)
		delete namedObjects;
	
	if (imageCache)
		delete imageCache;
		
	if (dviFontCache)
		delete dviFontCache;
		
	if (type0FontCache)
		delete type0FontCache;
		
	if (encodings)
		delete encodings;
		
	if (otlconf)
		delete otlconf;
		
	if (colorSpaces)
		delete colorSpaces;
		
	if (resources)
		delete resources;
	
	if (pageAttrs)
		free(pageAttrs);
		
	if (defFonts)
	{
		for (int i = 0; i < numDefFonts; i++)
		{
			if (defFonts[i].fontName)
				delete [] defFonts[i].fontName;
				
			if (defFonts[i].familyName)
				delete [] defFonts[i].familyName;
				
			if (defFonts[i].styleName)
				delete [] defFonts[i].styleName;
		}		
		free(defFonts);
	}
	
	if (loadedFonts)
		free(loadedFonts);
		
	pageAttrHash.clear();
	pageAttrCounter = 0;
	pageAttrs = 0;
	loadedFonts = 0;
	numLoadedFonts = 0;
	maxLoadedFonts = 0;
	numDefFonts = 0;
	maxDefFonts = 0;
	numSavedFonts = 0;
	defFonts = 0;
	pageAttrs = 0;
	resources = 0;
	colorSpaces = 0;
	otlconf = 0;
	encodings = 0;
	type0FontCache = 0;
	dviFontCache = 0;
	imageCache = 0;
	namedObjects = 0;
	_html_state = 0;
	_pdf_stat = 0;
	_tpic_state = 0;
	
	
	
#ifdef XW_BUILD_PS_LIB
  usesspecial = false;
	if (interpreter)
	{
		delete interpreter;
		interpreter = 0;
	}
	if (bangspecialsFile)
	{
		delete bangspecialsFile;
		bangspecialsFile = 0;
	}
#endif //XW_BUILD_PS_LIB
}

void XWDVICore::clearObjects()
{
	if (namedObjects && xref->isScanning())
		namedObjects->close(xref);
}

void XWDVICore::clearState()
{
	dviState.h = 0;
  	dviState.v = 0; 
  	dviState.w = 0; 
  	dviState.x = 0; 
  	dviState.y = 0; 
  	dviState.z = 0; 
  	dviState.d = 0;
  	dviStackDepth = 0;
  	currentFont    = -1;
  	currentFontDef = -1;
}

XWObject * XWDVICore::createCMapStream(XWCMap *cmap, 
	                                   int flagsA,
	                                   XWObject * obj)
{
	if (!resources)
		resources = new XWDVIResources;
		
	return resources->createCMapStream(xref, cmap, flagsA, obj);
}

int XWDVICore::defineImageResource(const char *ident,
			                       int subtype, 
			                       void *info, 
			                       XWObject *resource)
{
	if (!imageCache)
		imageCache = new XWDVIImageCache;
		
	return imageCache->defineResource(xref, ident, subtype, info, resource);
}

long XWDVICore::defineResource(const char *category, 
		    			       const char *resname, 
		    			       XWObject *object, 
		    			       int flagsA)
{
	if (!resources)
		resources = new XWDVIResources;
		
	return resources->defineResource(xref, category, resname, object, flagsA);
}

void XWDVICore::doBop()
{
	if (_tpic_state)
	{
		delete _tpic_state;
		_tpic_state = 0;
	}
	
	if (_html_state && _html_state->pending_type >= 0)
		xwApp->warning("...html anchor continues from previous page processed...\n");
		
	if (processingPage) 
	{
    	xwApp->error(tr("got a bop in the middle of a page!\n"));
    	return ;
    }
    
    clearState();
  	processingPage = 1;
  	
  	if (pageAttrs)
  	{
  		int i = pageAttrHash[pageNo];
  		PageAttr * attr = &pageAttrs[i];
  		pageWidth = attr->width;
  		pageHeight = attr->height;
  		x_offset = attr->xoff;
  		y_offset = attr->yoff;
  	}
  	
  	devOriginX = x_offset;
  	devOriginY = pageHeight - y_offset;
  	dviStackDepth = 0;
  	
#ifdef XW_BUILD_PS_LIB
    if (interpreter)
    {
    	if (xref->isScanning())
    		interpreter->savePageNextID(pageNo);
    	else
    		interpreter->resetPageNextID(pageNo);
    }
#endif //XW_BUILD_PS_LIB
  	
  	xref->beginPage(dev, tellMag(), devOriginX, devOriginY);
}

void XWDVICore::doDir(uchar d)
{
	dviState.d = d ? 1 : 0;
  	dev->setDirMode(dviState.d);
}

void XWDVICore::doEop()
{
	XWMPost::eopCleanup(xref);
		
	if (_tpic_state && _tpic_state->num_points > 0)
	{
    	xwApp->warning(tr("Unflushed tpic path at end of the page.\n"));
    	delete _tpic_state;
    	_tpic_state = 0;
    }
    
	if (_html_state && _html_state->pending_type >= 0)
		xwApp->warning("unclosed html anchor at end-of-page!\n");
		
	processingPage = 0;

  	if (dviStackDepth != 0) 
    	xwApp->error(tr("DVI stack depth is not zero at end of page.\n"));
  	
#ifdef XW_BUILD_PS_LIB
    lastclose = false;
    if (!xref->isBatch() && !xref->isScanning())
    	lastclose = true;
    if (interpreter)
		  interpreter->close();
#endif //XW_BUILD_PS_LIB

  	xref->endPage(this, dev);
  	
#ifdef XW_BUILD_PS_LIB
    if (!xref->isBatch() && !xref->isScanning())
#else
  	if (!xref->isScanning())
#endif //XW_BUILD_PS_LIB
    {
  		if (imageCache)
  			imageCache->load(this, xref, pageNo + 1);
  			
  		if (dviFontCache)
  			dviFontCache->load(this, xref, pageNo + 1, hDPI, vDPI);
  		
  		if (type0FontCache)
  			type0FontCache->load(this, xref, pageNo + 1);
  		
  		if (encodings)	
  			encodings->load(xref, pageNo + 1);
  		
  		if (otlconf)	
  			otlconf->close(xref);
  			
  		if (colorSpaces)
			colorSpaces->close(xref);
		
  		if (resources)
			resources->close(xref);
  	}
}

void XWDVICore::doFnt(long tex_id)
{
	int i = 0;
	for (; i < numDefFonts; i++) 
	{
    	if (defFonts[i].texId == tex_id)
      		break;
  	}

  	if (i == numDefFonts) 
  	{
  		QString msg = QString(tr("tried to select a font that hasn't been defined: id=%1.\n")).arg(tex_id);
    	xwApp->error(msg);
  	}

  	if (!defFonts[i].used) 
  	{
  		int font_id = -1;
  		if (defFonts[i].familyName)
  		{
  			font_id = locateNativeFont(defFonts[i].fontName,
                                       defFonts[i].familyName,
                                       defFonts[i].styleName,
                                       defFonts[i].pointSize,
                                       defFonts[i].layoutDir);
  		}
  		else
    		font_id = locateFont(defFonts[i].fontName, defFonts[i].pointSize);
    	loadedFonts[font_id].source = DVI;
    	defFonts[i].used    = 1;
    	defFonts[i].fontId = font_id;
  	}
  	
  	currentFontDef = i;
  	currentFont = defFonts[i].fontId;
}

void XWDVICore::doGlyphArray(QIODevice * file, int yLocsPresent)
{
	if (currentFont < 0)
		return ;
		
	DVILoadedFont * font  = &loadedFonts[currentFont];
	long width = getSignedQuad(file);
	
	uint slen = (uint) getUnsignedPair(file);
  long * xloc = new long[slen];
  long * yloc = new long[slen];
  for (uint i = 0; i < slen; i++)
  {
  	xloc[i] = getSignedQuad(file);
    yloc[i] = yLocsPresent ? getSignedQuad(file) : 0;
  }
  
  if (font->rgbaColor != 0xffffffff)
  {
  	XWDVIColor color;  	
  	color.setRGB((double)((uchar)(font->rgbaColor >> 24) & 0xff) / 255,
      						(double)((uchar)(font->rgbaColor >> 16) & 0xff) / 255,
      						(double)((uchar)(font->rgbaColor >>  8) & 0xff) / 255);
		dev->pushColor(xref, &color, &color);
  }
  
  uchar wbuf[2];
  long glyph_width = 0;
  for (int i = 0; i < slen; i++)
  {
  	uint glyph_id = getUnsignedPair(file);
  	if (glyph_id < font->ft_face->num_glyphs)
  	{
  		if (font->glyphWidths[glyph_id] == 0xffff)
  		{
  			if (FT_IS_SFNT(font->ft_face))
  			{
  				TT_HoriHeader  * dir_hea = (TT_HoriHeader*)FT_Get_Sfnt_Table(font->ft_face, (font->layout_dir == 0)
                                                                       ? ft_sfnt_hhea
                                                                       : ft_sfnt_vhea);
					if (dir_hea)
					{
						FT_UShort *metrics = (FT_UShort*) dir_hea->long_metrics;
						if (glyph_id < dir_hea->number_Of_HMetrics)
							font->glyphWidths[glyph_id] = metrics[glyph_id * 2];
						else
							font->glyphWidths[glyph_id] = metrics[2 * (dir_hea->number_Of_HMetrics - 1)];
					}
					else
						font->glyphWidths[glyph_id] = 0;
  			}
  			else
  			{
  				FT_Load_Glyph(font->ft_face, glyph_id, FT_LOAD_NO_SCALE);
          font->glyphWidths[glyph_id] = (font->layout_dir == 0)
                                          ? font->ft_face->glyph->metrics.horiAdvance
                                          : font->ft_face->glyph->metrics.vertAdvance;
  			}
  		}
  		
  		glyph_width = (double)font->size * (double)font->glyphWidths[glyph_id] / (double)font->ft_face->units_per_EM;
  		if (computeBoxes && linkAnnot && markedDepth >= taggedDepth)
  		{
  			double height = (double)font->size * (double)font->ft_face->ascender / (double)font->ft_face->units_per_EM;
        double depth  = (double)font->size * -(double)font->ft_face->descender / (double)font->ft_face->units_per_EM;
        
        PDFRect rect;
        dev->setRect(&rect, dviState.h + xloc[i], -dviState.v - yloc[i], glyph_width, (long)height, (long)depth);
        xref->expandBox(&rect);
  		}
  	}
  	
  	wbuf[0] = glyph_id >> 8;
    wbuf[1] = glyph_id & 0xff;
    dev->setString(xref, dviState.h + xloc[i], -dviState.v - yloc[i], wbuf, 2, glyph_width, font->fontId, -1);
  }
  
  if (font->rgbaColor != 0xffffffff)
  	dev->popColor(xref);
  	
  delete [] xloc;
  delete [] yloc;
  
  if (!dviState.d) 
    dviState.h += width;
  else 
    dviState.v += width;
}

void XWDVICore::doMarkDepth()
{
	if (linkAnnot && markedDepth == taggedDepth &&
      	dviStackDepth == taggedDepth - 1) 
    {
    	xref->breakAnnot(dev);
  	}
  	
  	markedDepth = dviStackDepth;
}

void XWDVICore::doMPSPage(QIODevice * fp)
{
	if (!fp)
		return ;
		
	mpMode = 1;
		
	XWDVIImage::doMPSPage(this, xref, dev, fp);
}

void XWDVICore::doPicFile(QIODevice * file)
{
	TransformInfo ti;
	transform_info_clear(&ti);
	
	getUnsignedByte(file);
	
	ti.matrix.a = getSignedQuad(file) / 65536.0; /* convert 16.16 Fixed to floating-point */
  ti.matrix.b = getSignedQuad(file) / 65536.0;
  ti.matrix.c = getSignedQuad(file) / 65536.0;
  ti.matrix.d = getSignedQuad(file) / 65536.0;
  ti.matrix.e = getSignedQuad(file) / 65536.0;
  ti.matrix.f = getSignedQuad(file) / 65536.0;
  
  int page_no = getSignedPair(file);
  int len = getUnsignedPair(file);
  char * path = new char[len + 1];
  for (int i = 0; i < len; ++i)
    path[i] = getUnsignedByte(file);
  path[len] = 0;
  
  if (page_no > 0)
  	--page_no;
  
  int xobj_id = findImageResource(path, page_no, 0);
  if (xobj_id >= 0)
  	dev->putImage(xref, xobj_id, &ti, dviState.h * dvi2pts, -(dviState.v * dvi2pts));
}

void XWDVICore::doPop()
{
	if (dviStackDepth <= 0)
	{
    	xwApp->error(tr("tried to pop an empty stack.\n"));
    	return ;
    }

  	dviState = dviStack[--dviStackDepth];
  	doMoveTo(dviState.h, dviState.v);
  	dev->setDirMode(dviState.d);
}

void XWDVICore::doPush()
{
	if (dviStackDepth >= DVI_STACK_DEPTH_MAX)
	{
		xwApp->error(tr("DVI stack exceeded limit.\n"));
		return ;
	}

  	dviStack[dviStackDepth++] = dviState;
}

void XWDVICore::doPut(long ch)
{
	if (currentFont < 0)
	{
		xwApp->error(tr("No font selected!"));
		return ;
	}
		
	DVILoadedFont * font  = &loadedFonts[currentFont];	
	switch (font->type)
	{
		case  PHYSICAL:
			{
				if (font->tfmId < 0)
					return ;
					
				XWTFMFile tfm(font->tfmId);
				long width = tfm.getFWWidth(ch);
  				width = scaleFixWord(font->size, width);
  				unsigned char wbuf[5];  				
  				if (ch > 255)
  				{
  					wbuf[0] = (ch >> 8) & 0xff;
      				wbuf[1] =  ch & 0xff;
      				dev->setString(xref, dviState.h, -dviState.v, wbuf, 2, width, font->fontId, 2);
  				}
  				else if (font->subfontId >= 0)
  				{
  					XWSubfont subfont;
  					uint uch = subfont.lookupRecord(font->subfontId, (uchar)ch);
  					wbuf[0] = (uch >> 8) & 0xff;
      			wbuf[1] =  uch & 0xff;
      			dev->setString(xref, dviState.h, -dviState.v, wbuf, 2, width, font->fontId, 2);
  				}
  				else
  				{
  					wbuf[0] = (uchar) ch;
      				dev->setString(xref, dviState.h, -dviState.v, wbuf, 1, width, font->fontId, 1);
  				}
  				
  				if (computeBoxes && 
  					linkAnnot && 
  					markedDepth >= taggedDepth)
  				{
  					long height = tfm.getFWHeight(ch);
      				long depth  = tfm.getFWDepth(ch);
      				height = scaleFixWord(font->size, height);
      				depth  = scaleFixWord(font->size, depth);
      			
					PDFRect rect;
      				dev->setRect(&rect, dviState.h, -dviState.v, width, height, depth);
      				xref->expandBox(&rect);
  				}
			}
			break;
			
		case  VIRTUAL:
			XWFontFileVF::setChar(this, ch, font->fontId);
			break;
	}
}

void XWDVICore::doRule(long width, long height)
{
	doMoveTo(dviState.h, dviState.v);	
  	if (!dviState.d) 
    	dev->setRule(xref, dviState.h, -dviState.v,  width, height);
  	else 
    	dev->setRule(xref, dviState.h, -dviState.v - width, height, width);
}

void XWDVICore::doSet(long ch)
{
	if (currentFont < 0)
	{
		xwApp->error(tr("No font selected!"));
		return ;
	}
		
	DVILoadedFont * font  = &loadedFonts[currentFont];
	if (font->tfmId < 0)
		return ;
		
	XWTFMFile tfm(font->tfmId);
	long width = tfm.getFWWidth(ch);
  	width = scaleFixWord(font->size, width);
  	unsigned char wbuf[5];
  	switch (font->type)
  	{
  		case  PHYSICAL:  			
  			if (ch > 255)
  			{
  				wbuf[0] = (ch >> 8) & 0xff;
      		wbuf[1] =  ch & 0xff;
      		dev->setString(xref, dviState.h, -dviState.v, wbuf, 2, width, font->fontId, 2);
  			}
  			else if (font->subfontId >= 0)
  			{
  				XWSubfont subfont;
  				uint uch = subfont.lookupRecord(font->subfontId, (uchar)ch);
  				wbuf[0] = (uch >> 8) & 0xff;
      		wbuf[1] =  uch & 0xff;
      		dev->setString(xref, dviState.h, -dviState.v, wbuf, 2, width, font->fontId, 2);
  			}
  			else
  			{
  				wbuf[0] = (uchar) ch;
      		dev->setString(xref, dviState.h, -dviState.v, wbuf, 1, width, font->fontId, 1);
  			}
  			if (computeBoxes && 
  				linkAnnot && 
  				markedDepth >= taggedDepth)
  			{
  				long height = tfm.getFWHeight(ch);
      			long depth  = tfm.getFWDepth(ch);
      			height = scaleFixWord(font->size, height);
      			depth  = scaleFixWord(font->size, depth);
      			
				PDFRect rect;
      			dev->setRect(&rect, dviState.h, -dviState.v, width, height, depth);
      			xref->expandBox(&rect);
  			}
  			break;
  			
  		case  VIRTUAL:
  			XWFontFileVF::setChar(this, ch, font->fontId);
  			break;
  	}
  	
  	if (!dviState.d) 
    	dviState.h += width;
  	else 
    	dviState.v += width;
}

void XWDVICore::doSpecial(const char *buffer, long size)
{
	if (size <= 0)
		return ;
		
	if (size > 0x7fffffffUL) 
	{
		QString msg = QString(tr("special more than %1 bytes???\n")).arg(size);
    	xwApp->warning(msg);
    	return ;
  	}
  	
  	if (xref->isScanning())
  		scanPaperSize(buffer, size);
  		
  	dev->graphicsMode(xref);
  	
  	double x_user =  dviState.h * dvi2pts;
  	double y_user = -dviState.v * dvi2pts;
  	XWDVISpecial * special = XWDVISpecial::parse(this, xref, dev, buffer, size, x_user, y_user, totalMag);
  	if (!special)
  	{
//  		xwApp->warning(tr("ignore unknown special.\n"));
  		return ;
  	}
  	
  	special->exec(hDPI, vDPI);
  	delete special;
}

void XWDVICore::doString(const uchar *s, int len)
{
	if (currentFont < 0)
	{
		xwApp->error(tr("No font selected!"));
		return ;
	}
		
	DVILoadedFont * font  = &loadedFonts[currentFont];
	if (font->tfmId < 0)
		return ;
		
	XWTFMFile tfm(font->tfmId);
  	long width = tfm.getStringWidth(s, len);
  	width = scaleFixWord(font->size, width);
  	switch (font->type)
  	{
  		case PHYSICAL:
  			if (font->subfontId < 0)
  			{
  				dev->setString(xref, dviState.h, -dviState.v, s, len, width, font->fontId, 1);
      			if (computeBoxes && linkAnnot && markedDepth >= taggedDepth) 
      			{
        			long height = tfm.getStringHeight(s, len);
        			long depth  = tfm.getStringDepth(s, len);
        			height = scaleFixWord(font->size, height);
        			depth  = scaleFixWord(font->size, depth);
					PDFRect rect;
        			dev->setRect(&rect, dviState.h, -dviState.v, width, height, depth);
        			xref->expandBox(&rect);
      			}
  			}
  			else
  			{
  				doPush();
      			for (int i = 0; i < len; i++) 
        			doSet(s[i]);
      			doPop();
  			}
  			break;
  			
  		case VIRTUAL:
    		doPush();
    		for (int i = 0; i < len; i++) 
      			doSet(s[i]);
    		doPop();
    		break;
  	}
  	
  	if (!dviState.d) 
    	dviState.h += width;
  	else 
    	dviState.v += width;
}

void XWDVICore::doTagDepth()
{
	taggedDepth = markedDepth;
  	doComputeBoxes(1);
}

void XWDVICore::doUntagDepth()
{
	taggedDepth = -1;
  	doComputeBoxes(0);
}

void XWDVICore::doW(long ch)
{
	dviState.w = ch;
  	doRight(ch);
}

void XWDVICore::doW0()
{
	doRight(dviState.w);
}

void XWDVICore::doX(long ch)
{
	dviState.x = ch;
  	doRight(ch);
}

void XWDVICore::doX0()
{
	doRight(dviState.x);
}

void XWDVICore::doY(long ch)
{
	dviState.y = ch;
  	doDown(ch);
}

void XWDVICore::doY0()
{
	doDown(dviState.y);
}

void XWDVICore::doZ(long ch)
{
	dviState.z = ch;
  	doDown(ch);
}

void XWDVICore::doZ0()
{
	doDown(dviState.z);
}

#ifdef XW_BUILD_PS_LIB
int XWDVICore::doLiteralSpecial(const char *buffer, long sizeA)
{
	if (!interpreter)
	{
		interpreter = new XWPSInterpreter;
		interpreter->savePageNextID(pageNo);
	}
	
	QByteArray ba("@beginspecial @setspecial\n");
	ba.append(buffer, sizeA);
	ba.append("\n@endspecial\n");
	
	return interpreter->toPDF(this, xref, dev, (const uchar*)(ba.data()), ba.size());
}
#endif //XW_BUILD_PS_LIB

void XWDVICore::encodingAddUsedChars(int encoding_id, const char *is_used)
{
	if (encodings)
		encodings->addUsedChars(encoding_id, is_used, pageNo + 1);
}

void XWDVICore::encodingComplete(bool finished)
{
	if (encodings)		
		encodings->complete(this, xref, finished);
}

void XWDVICore::encodingCompletePage()
{
	if (encodings)
		encodings->completePage(this, xref, pageNo);
}

void XWDVICore::endDoc(bool finished)
{
	if (_html_state)
	{
		delete _html_state;
		_html_state = 0;
	}
		
	if (_tpic_state && _tpic_state->num_points > 0)
	{
		xwApp->warning(tr("unflushed tpic path at end of the document.\n"));
		delete _tpic_state;
		_tpic_state = 0;
	}
	
#ifdef XW_BUILD_PS_LIB
  lastclose = true;
  if (xref->isScanning())
  {
  	endscanning = true;
  	endBangSpecials();
  }
	if (interpreter)
		interpreter->close();
#endif //XW_BUILD_PS_LIB

  xref->close();
  
	if (namedObjects)
		namedObjects->close(xref);
		
	if (!finished)
	{	
		dev->clear(xref);
		if (_pdf_stat)
		{
			_pdf_stat->annot_dict.initNull();
			_pdf_stat->lowest_level = 255;
		}
		
		pageNo = -1;
		return ;
	}
		
	if (imageCache)
		imageCache->close(finished);
		
	if (dviFontCache)
		dviFontCache->close(this, xref, hDPI, vDPI, finished);
		
	if (type0FontCache)
		type0FontCache->close(this, xref, finished);
		
	if (encodings)
		encodings->close(xref, finished);
		
	if (otlconf)
		otlconf->close(xref, finished);
		
	if (colorSpaces)
		colorSpaces->close(xref, finished);
		
	if (resources)
		resources->close(xref, finished);
		
	xref->outFlush();
	pageNo = -1;
}

int  XWDVICore::encodingFindResource(char * enc_name)
{
	if (!encodings)
		encodings = new XWDVIEncodingCache;
		
	return encodings->findResource(enc_name);
}

XWDVIEncoding * XWDVICore::encodingGet(int enc_id)
{
	if (encodings)
		return encodings->get(enc_id);
		
	return 0;
}

void XWDVICore::encodingUsedByType3(int enc_id)
{
	if (encodings)
		encodings->usedByType3(enc_id);
}

#ifdef XW_BUILD_PS_LIB
void XWDVICore::endBangSpecials()
{
	if (bangspecialsFile)
	{
	  bangspecialsFile->write("@fedspecial end\n");
	  bangspecialsFile->seek(0);
	  QByteArray ba = bangspecialsFile->readAll();
	  if (!interpreter)
	  {
		  interpreter = new XWPSInterpreter;
		  interpreter->savePageNextID(pageNo);
		}
	  interpreter->toPDF(this, xref, dev, (const uchar*)(ba.data()), ba.size());
	  bangspecialsFile->close();
	  delete bangspecialsFile;
	  bangspecialsFile = 0;
	}
}
#endif //XW_BUILD_PS_LIB

int XWDVICore::findFontResource(const char *tex_name,
		       	                double font_scale, 
		       	                XWTexFontMap *mrec)
{
	if (!dviFontCache)
		dviFontCache = new XWDVIFontCache;
		
	return dviFontCache->find(this, xref, hDPI, tex_name, font_scale, mrec);
}

int  XWDVICore::findImageResource(const char *identA, 
	                              long page_noA, 
	                              XWObject *dictA)
{
	if (!imageCache)
		imageCache = new XWDVIImageCache;
		
	return imageCache->findResource(this, xref, dev, identA, page_noA, dictA);
}

long XWDVICore::findResource(const char *category, const char *resname)
{
	if (!resources)
		resources = new XWDVIResources;
		
	return resources->findResource(category, resname);
}

int  XWDVICore::findType0Font(const char *map_name, 
	                          int cmap_id, 
	                          XWTexFontMapOpt *fmap_opt)
{
	if (!type0FontCache)
		type0FontCache = new XWDVIType0FontCache;
		
	return type0FontCache->find(xref, map_name, cmap_id, fmap_opt);
}

void XWDVICore::finishVF()
{
	doPop();
  	if (numSavedFonts > 0) 
    	currentFont = savedDVIFont[--numSavedFonts];
  	else 
    	xwApp->error(tr("tried to pop an empty font stack.\n"));
}

void XWDVICore::flushObject(const char *key)
{
	if (namedObjects && (xref->isScanning()))
		namedObjects->closeObject(key, strlen(key));
}

XWObject * XWDVICore::getColorSpaceReference(int cspc_id, XWObject * ref)
{
	if (colorSpaces)
		return colorSpaces->getColorSpaceReference(xref, cspc_id, ref);
	
	return 0;
}

DVIFontDef * XWDVICore::getCurrentFontDef()
{
	if (currentFontDef < 0)
		return 0;
		
	return &defFonts[currentFontDef];
}

XWDVIFont *  XWDVICore::getDviFont(int font_id)
{
	if (dviFontCache)
		return dviFontCache->getFont(font_id);
		
	return 0;
}

char ** XWDVICore::getEncoding(int enc_id)
{
	if (encodings)
		return encodings->getEncoding(enc_id);
		
	return 0;
}

char * XWDVICore::getEncodingName(int enc_id)
{
	if (encodings)
		return encodings->getName(enc_id);
		
	return 0;
}

XWObject * XWDVICore::getEncodingObj(int enc_id)
{
	if (encodings)
		return encodings->getEncodingObj(enc_id);
		
	return 0;
}

XWObject  *  XWDVICore::getEncodingTounicode(int enc_id)
{
	if (encodings)
		return encodings->getTounicode(enc_id);
		
	return 0;
}

DVIFontDef * XWDVICore::getFontDef()
{
	DVIFontDef * ret = 0;
	if (numDefFonts >= maxDefFonts) 
	{
    	maxDefFonts += TEX_FONTS_ALLOC_SIZE;
    	defFonts = (DVIFontDef*)realloc(defFonts, maxDefFonts * sizeof(DVIFontDef));
    	for (int i = numDefFonts; i < maxDefFonts; i++)
    	{
    		ret = &defFonts[i];
    		ret->fontName = 0;
    		ret->familyName = 0;
    		ret->styleName = 0;
    	}    	
  	}
  	
  	ret = &defFonts[numDefFonts];
  	ret->texId = -1;
  	ret->pointSize = 0;
  	ret->designSize = 0;
  	if (ret->fontName)
  		delete [] ret->fontName;
  		
  	if (ret->familyName)
  		delete [] ret->familyName;
  		
  	if (ret->styleName)
  		delete [] ret->styleName;
  		
  	ret->fontName = 0;
  	ret->fontId = -1;
  	ret->used = 0;
  	ret->familyName = 0;
    ret->styleName = 0;
    ret->rgbaColor = 0xffffffff;
    ret->layoutDir = 0;
  	numDefFonts++;
  	return ret;
}

DVIFontDef * XWDVICore::getFontDef(int i)
{
	if (i < 0 || i >= numDefFonts)
		return 0;
		
	return &defFonts[i];
}

int  XWDVICore::getFontEncoding(int font_id)
{
	if (dviFontCache)
		return dviFontCache->getFontEncoding(font_id);
		
	return -1;
}

XWObject * XWDVICore::getFontReference(int font_id, XWObject * obj)
{
	if (dviFontCache)
		return dviFontCache->getFontReference(this, xref, font_id, obj);
		
	return 0;
}

int XWDVICore::getFontSubtype(int font_id)
{
	if (dviFontCache)
		return dviFontCache->getFontSubtype(font_id);
		
	return 0;
}

char * XWDVICore::getFontUsedChars(int font_id)
{
	if (dviFontCache)
		return dviFontCache->getFontUsedChars(this, font_id);
		
	return 0;
}

int XWDVICore::getFontWMode(int font_id)
{
	if (dviFontCache)
		return dviFontCache->getFontWMode(this, font_id);
		
	return 0;
}

HTMLState * XWDVICore::getHTMLState()
{
	if (!_html_state)
		_html_state = new HTMLState;
		
	return _html_state;
}

XWObject * XWDVICore::getImageReference(int id, XWObject * obj)
{
	if (imageCache)
		return imageCache->getReference(xref, id, pageNo + 1, obj);
		
	return 0;
}

char * XWDVICore::getImageResName(int id)
{
	if (imageCache)
		return imageCache->getResName(id);
		
	return 0;
}

int  XWDVICore::getImageSubtype(int id)
{
	if (imageCache)
		return imageCache->getSubtype(id);
		
	return 0;
}

DVILoadedFont * XWDVICore::getLoadedFont(int i)
{
	if (i < 0 || i >= numLoadedFonts)
		return 0;
		
	return &loadedFonts[i];
}

PDFState * XWDVICore::getPDFState()
{
	if (!_pdf_stat)
		_pdf_stat = new PDFState(xref);
		
	return _pdf_stat;
}

#ifdef XW_BUILD_PS_LIB
XWPSInterpreter * XWDVICore::getPSInterpreter()
{
	if (!interpreter)
	{
		interpreter = new XWPSInterpreter;
		interpreter->savePageNextID(pageNo);
	}
		
	return interpreter;
}
#endif //XW_BUILD_PS_LIB

XWObject * XWDVICore::getResourceReference(long rc_id, XWObject * ref)
{
	if (resources)
		return resources->getResourceReference(xref, rc_id, ref);
		
	return 0;
}

TPICState * XWDVICore::getTPICState()
{
	if (!_tpic_state)
	{
		_tpic_state = new TPICState;
		if (optFlags & OPT_TPIC_TRANSPARENT_FILL)
			_tpic_state->mode.fill = TPIC_MODE__FILL_OPACITY;
			
		if (_tpic_state->mode.fill != TPIC_MODE__FILL_SOLID && xref->getVersion() < 4) 
		{
			xwApp->warning(tr("tpic shading support requires PDF version 1.4.\n"));
    		_tpic_state->mode.fill = TPIC_MODE__FILL_SOLID;
    	}
	}
		
	return _tpic_state;
}

XWDVIType0Font * XWDVICore::getType0Font(int id)
{
	if (type0FontCache)
		return type0FontCache->get(id);
		
	return 0;
}

void XWDVICore::getPos(double * x, double * y)
{
	PDFCoord p;
	p.x = dviState.h * dvi2pts;
	p.y = -dviState.v * dvi2pts;
	dev->transform(&p, 0);
	*x = p.x;
	*y = p.y;
}

int XWDVICore::iccpLoadProfile(const char *ident,
		                       const void *profile, 
		                       long proflen)
{
	if (!colorSpaces)
		colorSpaces = new XWDVIColorSpaces;
		
	return colorSpaces->iccpLoadProfile(xref, dev, ident, profile, proflen);
}

void XWDVICore::initDev()
{
	totalMag = (double) dviInfo.mag / 1000.0 * mag;
  dvi2pts  = (double) dviInfo.unitNum / (double) dviInfo.unitDen;
  dvi2pts  *= (72.0 / 254000.0);	
  dev->setDvi2Pts(dvi2pts);
  xref->setCreator(dviInfo.comment);
}

void XWDVICore::initVF(int dev_font_id)
{
	doPush();

  	dviState.w = 0; 
  	dviState.x = 0;
  	dviState.y = 0; 
  	dviState.z = 0;

  	if (numSavedFonts < VF_NESTING_MAX) 
    	savedDVIFont[numSavedFonts++] = currentFont;
  	else
  	{
    	xwApp->error(tr("virtual fonts nested too deeply!\n"));
    	return ;
    }
  	currentFont = dev_font_id;
}

int XWDVICore::isPredefinedEncoding(int enc_id)
{
	if (encodings)
		return encodings->isPredefined(enc_id);
		
	return 0;
}

void XWDVICore::loadFontMapFile(const char  *filename)
{
	if (!filename)
		return ;
		
	XWFontCache fcache(true);
	if (optFlags & OPT_FONTMAP_FIRST_MATCH)
		fcache.loadFontMapFile(filename, FONTMAP_RMODE_APPEND);
	else
		fcache.loadFontMapFile(filename, FONTMAP_RMODE_REPLACE);
}

XWObject * XWDVICore::loadToUnicodeStream(const char *ident,
	                                      XWObject * obj)
{
	if (!resources)
		resources = new XWDVIResources;
		
	return resources->loadToUnicodeStream(xref, ident, obj);
}

int XWDVICore::locateFont(const char *tfm_name, long ptsize, bool vf)
{
	XWTFMFile tfm(tfm_name);
	if (vf)
	{
		for (int i = 0; i < numLoadedFonts; i++)
		{
			if (loadedFonts[i].tfmId == tfm.getID() && 
				loadedFonts[i].size == ptsize)
			{
				return i;
			}
		}
	}
	
	needMoreFonts(1);
	int cur_id = numLoadedFonts++;
	XWFontCache fcache(true);
	XWTexFontMap * mrec = fcache.lookupFontMapRecord(tfm_name);
	int subfont_id = -1;
	if (mrec && mrec->charmap.sfd_name && mrec->charmap.subfont_id) 
	{
		QString n(mrec->charmap.sfd_name);
		QString id(mrec->charmap.subfont_id);
		XWSubfont subfont(n);
    	subfont_id = subfont.loadRecord(id);
  	}
  		
  	loadedFonts[cur_id].tfmId     = tfm.getID();
  	loadedFonts[cur_id].subfontId = subfont_id;
  	loadedFonts[cur_id].size      = ptsize;
  	loadedFonts[cur_id].source    = VF;
  	
  	int font_id = -1;
  	if (!mrec || (mrec && mrec->useVF() && mrec->getVFont()))
  	{
  		if (!mrec)
  			font_id = XWFontFileVF::locateFont(this, tfm_name, ptsize);
  		else if (mrec->getVFont())
  			font_id = XWFontFileVF::locateFont(this, mrec->getVFont(), ptsize);
  			
  		if (font_id >= 0)
  		{
  			loadedFonts[cur_id].type   = VIRTUAL;
      		loadedFonts[cur_id].fontId = font_id;
      		return  cur_id;
  		}
  	}
  	
  	char * name = 0;
  	if (mrec && mrec->map_name) 
    	name = mrec->map_name;
  	else 
    	name = (char*)tfm_name;
    	    	
    font_id = dev->locateFont(name, ptsize);
    if (font_id < 0)
    {
    	numLoadedFonts--;
    	QString msg = QString(tr("failed to locate tex font '%1'.\n")).arg(tfm_name);
    	xwApp->error(msg);
    	return -1;
    }
    
    loadedFonts[cur_id].type   = PHYSICAL;
  	loadedFonts[cur_id].fontId = font_id;
  	return  cur_id;
}

int  XWDVICore::locateNativeFont(const char *ps_name, 
	                               const char *fam_name,
                                 const char *sty_name, 
                                 long ptsize, 
                                 int layout_dir)
{
	needMoreFonts(1);
	int cur_id = numLoadedFonts++;
	XWFontCache fcache(true);
	
	char * fontmap_key = new char[strlen(ps_name) + 3];
	sprintf(fontmap_key, "%s/%c", ps_name, layout_dir == 0 ? 'H' : 'V');
	
	XWTexFontMap * mrec = fcache.lookupFontMapRecord(fontmap_key);
	if (!mrec)
	{
		if (-1 == fcache.loadNativeFont(ps_name, fam_name, sty_name, layout_dir))
		{
			delete [] fontmap_key;
			return -1;
		}
			
		mrec = fcache.lookupFontMapRecord(fontmap_key);
	}
	
	XWTexFontMapOpt * opt = mrec->getOpt();
   	
	loadedFonts[cur_id].fontId = locateFont(fontmap_key, ptsize);
	loadedFonts[cur_id].size    = ptsize;
	loadedFonts[cur_id].type    = NATIVE;
	delete [] fontmap_key;
	loadedFonts[cur_id].ft_face = opt->ftface;
	if (opt->glyphwidths == NULL)
	{
		opt->glyphwidths = new ushort[opt->ftface->num_glyphs];
		for (int i = 0; i < opt->ftface->num_glyphs; ++i)
      opt->glyphwidths[i] = 0xffff;
	}
	
	loadedFonts[cur_id].glyphWidths = opt->glyphwidths;
  loadedFonts[cur_id].layout_dir = layout_dir;
  
  return cur_id;
}

XWObject * XWDVICore::lookupObject(const char *key, XWObject * obj)
{
	obj->initNull();
	if (!key)
    	return 0;

  	int k = 0;
    for (; _rkeys[k] && strcmp(key, _rkeys[k]); k++);
    
    PDFCoord cp;
    XWObject * tmp = 0;
    switch (k)
    {
    	case  K_OBJ__XPOS:
    		cp.x = dviState.h * dvi2pts;
    		cp.y = 0.0;
    		dev->transform(&cp, NULL);
    		obj->initInt(round(cp.x, .01));
    		break;
    		
  		case  K_OBJ__YPOS:
    		cp.x = 0.0; 
    		cp.y = -(dviState.v * dvi2pts);
    		dev->transform(&cp, NULL);
    		obj->initInt(round(cp.y, .01));
    		break;
    		
  		case  K_OBJ__THISPAGE:
    		xref->getThisPage()->copy(obj);
    		break;
    		
  		case  K_OBJ__PAGES:
    		xref->getPageTree()->copy(obj);
    		break;
    		
  		case  K_OBJ__NAMES:
    		xref->getNames()->copy(obj);
    		break;
    		
  		case  K_OBJ__RESOURCES:
    		xref->currentPageResources()->copy(obj);
    		break;
    		
  		case  K_OBJ__CATALOG:
    		xref->getCatalog()->copy(obj);
    		break;
    		
  		case  K_OBJ__DOCINFO:
    		xref->getDocInfo()->copy(obj);
    		break;
    		
  		default:
  			tmp = namedObjects->lookupObject(key, strlen(key));
  			if (tmp)
  				tmp->copy(obj);
  			else
  				return 0;
    		break;
    }
    
    return obj;
}

XWObject * XWDVICore::lookupReference(const char *key, XWObject * obj)
{
	obj->initNull();	
	if (!key)
    	return  0;
    	
    int k = 0;
    for (; _rkeys[k] && strcmp(key, _rkeys[k]); k++);
    
    PDFCoord cp;
    switch (k)
    {
    	case  K_OBJ__XPOS:
    		cp.x = dviState.h * dvi2pts;
    		cp.y = 0.0;
    		dev->transform(&cp, NULL);
    		obj->initInt(round(cp.x, .01));
    		break;
    		
    	case  K_OBJ__YPOS:
    		cp.x = 0.0; 
    		cp.y = -(dviState.v * dvi2pts);
    		dev->transform(&cp, NULL);
    		obj->initInt(round(cp.y, .01));
    		break;
    		
    	case  K_OBJ__THISPAGE:
    		xref->thisPageRef(obj);
    		break;
    		
    	case  K_OBJ__PREVPAGE:
    		xref->prevPageRef(obj);
    		break;
    		
    	case  K_OBJ__NEXTPAGE:
    		xref->nextPageRef(obj);
    		break;
    		
    	case  K_OBJ__PAGES:
    		xref->refObj(xref->getPageTree(), obj);
    		break;
    		
    	case  K_OBJ__NAMES:
    		xref->refObj(xref->getNames(), obj);
    		break;
    		
    	case  K_OBJ__RESOURCES:
    		xref->refObj(xref->currentPageResources(), obj);
    		break;
    		
    	case  K_OBJ__CATALOG:
    		xref->refObj(xref->getCatalog(), obj);
    		break;
    		
    	case  K_OBJ__DOCINFO:
    		xref->refObj(xref->getDocInfo(), obj);
    		break;
    		
    	default:
    		if (ispageref(key))
      			xref->refPage(atoi(key + 4), obj);
    		else 
      			return namedObjects->lookupReference(xref, key, strlen(key), obj);
    		break;
    }
    	
    return obj;
}

XWObject * XWDVICore::otfCreateToUnicodeStream(const char *font_name,
			     					           int ttc_index,
			                                   const char *used_glyphs,
			                                   XWObject * obj)
{
	if (!resources)
		resources = new XWDVIResources;
		
	return resources->otfCreateToUnicodeStream(xref, font_name, ttc_index, used_glyphs, obj);
}

int XWDVICore::otfLoadUnicodeCMap(const char *map_name, 
	                              int ttc_index,
		                          const char *otl_tags, 
		                          int wmode)
{
	if (!resources)
		resources = new XWDVIResources;
		
	return resources->otfLoadUnicodeCMap(this, xref, map_name, ttc_index, otl_tags, wmode);
}

XWObject * XWDVICore::otlFindConf(const char *conf_name, 
	                              XWObject * rule)
{
	if (!otlconf)
		otlconf = new XWOTLConf;
		
	return otlconf->findConf(xref, conf_name, rule);
}

void XWDVICore::otlInitConf()
{
	if (!otlconf)
		otlconf = new XWOTLConf;
		
	otlconf->init(xref);
}

void XWDVICore::pushObject(const char *key, XWObject *value)
{
	if (!key || !value)
    	return;
    	
    if (xref->isScanning())
    {
    	if (!namedObjects)
    		namedObjects = new XWPDFNames;
    	namedObjects->addObject(key, strlen(key), value);
    }
}

void XWDVICore::quit()
{
	XWMPost::quit();
	XWFontFileVF::close();
	XWPDFFile::closeAll();
	XWTFMFile::closeAll();
}

XWObject * XWDVICore::readToUnicodeFile(const char *cmap_name, 
	                                    XWObject * obj)
{
	if (!resources)
		resources = new XWDVIResources;
		
	return resources->readToUnicodeFile(xref, cmap_name, obj);
}

void XWDVICore::resetFlags()
{
	if (optFlags & OPT_CIDFONT_FIXEDPITCH)
    	XWCIDFontCache::setFlags(CIDFONT_FORCE_FIXEDPITCH);
    else
    	XWCIDFontCache::setFlags(0);
}

int XWDVICore::scaleImage(int            id,
                          PDFTMatrix    *M,
                          PDFRect       *r,
                          TransformInfo *p)
{
	if (imageCache)
		return imageCache->scaleImage(id, M, r, p);
		
	return -1;
}

void XWDVICore::scanPaperSize(const char *buf, long size)
{
	double w = pageWidth;
	double h = pageHeight;
	double xo = x_offset;
	double yo = y_offset;
	
	int ns_pdf = 0;
	const char * endptr = buf + size;
	XWLexer lexer(buf, size);
	const char * p = lexer.skipWhite();
	char * q = lexer.getCIdent();
  	if (q && !strcmp(q, "pdf")) 
  	{
    	p = lexer.skipWhite();
    	if (p < endptr && *p == ':') 
    	{
      		p = lexer.skip(1);
      		p = lexer.skipWhite();
      		delete [] q;
      		q = lexer.getCIdent(); 
      		ns_pdf = 1;
    	}
  	}
  	
  	p = lexer.skipWhite();
  	int error = 0;
  	if (q)
  	{
  		if (!strcmp(q, "landscape")) 
      		landscapeMode = 1;
      	else if (ns_pdf && !strcmp(q, "pagesize"))
      	{
      		while (!error && p < endptr)
      		{
      			char  *kp = lexer.getCIdent(); 
        		if (!kp)
          			break;
          		else
          		{
          			p = lexer.skipWhite();
          			if (!strcmp(kp, "width")) 
          			{
            			if (lexer.getLength(&w, totalMag))
              				w *= totalMag;
              			else
              				error = 1;
          			} 
          			else if (!strcmp(kp, "height")) 
          			{
            			if (lexer.getLength(&h, totalMag))
              				h *= totalMag;
              			else
              				error = 1;
          			} 
          			else if (!strcmp(kp, "xoffset")) 
          			{
            			if (lexer.getLength(&xo, totalMag))
              				xo *= totalMag;
              			else
              				error = 1;
          			} 
          			else if (!strcmp(kp, "yoffset")) 
          			{
            			if (lexer.getLength(&yo, totalMag))
              				yo *= totalMag;
              			else
              				error = 1;
          			}
          			delete [] kp;
          		}
          		p = lexer.skipWhite();
      		}
      	}
      	else if (!strcmp(q, "papersize"))
      	{
      		char  qchr = 0;
      		if (*p == '=') 
      			p = lexer.skip(1);
      		p = lexer.skipWhite();
      		if (p < endptr && (*p == '\'' || *p == '\"')) 
      		{
        		qchr = *p; 
        		p = lexer.skip(1);
        		p = lexer.skipWhite();
      		}
      		
      		if (lexer.getLength(&w, 1.0))
      		{
      			p = lexer.skipWhite();
        		if (p < endptr && *p == ',') 
        		{
          			p = lexer.skip(1);
        			p = lexer.skipWhite();
        		}
        		lexer.getLength(&h, 1.0);
        		p = lexer.skipWhite();
      		}
      		
      		if (qchr)
      		{
      			if (p >= endptr || *p != qchr)
          			error = -1;
      		}
      	}
      	
      	delete [] q;
  	}
  	
  	if (landscapeMode)
  	{
  		double tmp = w;
  		w = h;
  		h = tmp;
  	}
  	
  	bool attchg = false;
  	
  	if (pageWidth  != w || pageHeight != h) 
  	{
    	pageWidth  = w;
        pageHeight = h;
        attchg = true;
    }
    
    if (x_offset != xo || y_offset != yo) 
    {
    	x_offset = xo;
        y_offset = yo;
        attchg = true;
    }
    
    if (attchg && xref->isScanning())
    {
    	pageAttrs = (PageAttr*)realloc(pageAttrs, (pageAttrCounter + 1) * sizeof(PageAttr));
    	PageAttr * attr = &pageAttrs[pageAttrCounter];
    	attr->width = pageWidth;
    	attr->height = pageHeight;
    	attr->xoff = x_offset;
    	attr->yoff = y_offset;
    	pageAttrCounter++;
    }
    
    if (pageWidth != paperWidth || pageHeight != paperHeight) 
    {
    	PDFRect mediabox;
    	mediabox.llx = 0.0;
        mediabox.lly = 0.0;
        mediabox.urx = pageWidth;
        mediabox.ury = pageHeight;
        xref->setMediaBox(pageNo + 1, &mediabox);
   }
}

int XWDVICore::scanSpecial(double *wd, 
                           double *ht, 
                           double *xo, 
                           double *yo, 
                           char *lm,
	                         unsigned *minorversion,
	                         int *do_enc, 
	                         unsigned *key_bits, 
	                         unsigned *permission, 
	                         char *owner_pw, 
	                         char *user_pw,
	                         const char *buf, 
	                         long size)
{
	int ns_pdf = 0;
	const char * endptr = buf + size;
	XWLexer lexer(buf, size);
	const char * p = lexer.skipWhite();
	char * q = lexer.getCIdent();
	char opw[100];
	char upw[100];
	if (!owner_pw)
		owner_pw = opw;
		
	if (!user_pw)
		user_pw = upw;
	
	if (q && !strcmp(q, "pdf")) 
  {
   	p = lexer.skipWhite();
   	if (p < endptr && *p == ':') 
   	{
     		p = lexer.skip(1);
     		p = lexer.skipWhite();
     		delete [] q;
     		q = lexer.getCIdent(); 
     		ns_pdf = 1;
   	}
  }
  
  p = lexer.skipWhite();
  int error = 0;
  bool ok = true;
  int slen = 0;
  if (q)
  {
  	if (!strcmp(q, "landscape")) 
  		*lm = 1;
  	else if (ns_pdf && !strcmp(q, "pagesize"))
  	{
  		while (!error && p < endptr)
      {
      	char  *kp = lexer.getCIdent(); 
      	if (!kp)
      		break;
      	else
      	{
      		p = lexer.skipWhite();
      		if (!strcmp(kp, "width")) 
      		{
       			if (lexer.getLength(wd, totalMag))
       				*wd *= totalMag;
       			else
       				error = 1;
      		} 
      		else if (!strcmp(kp, "height")) 
      		{
       			if (lexer.getLength(ht, totalMag))
       				*ht *= totalMag;
       			else
       				error = 1;
      		} 
      		else if (!strcmp(kp, "xoffset")) 
      		{
       			if (lexer.getLength(xo, totalMag))
       				*xo *= totalMag;
       			else
       				error = 1;
      		} 
      		else if (!strcmp(kp, "yoffset")) 
      		{
       			if (lexer.getLength(yo, totalMag))
       				*yo *= totalMag;
       			else
      				error = 1;
      		}
      		delete [] kp;
      	}
      	p = lexer.skipWhite();
      }
  	}
  	else if (!strcmp(q, "papersize"))
    {
    	char  qchr = 0;
    	if (*p == '=') 
    		p = lexer.skip(1);
    	p = lexer.skipWhite();
    	if (p < endptr && (*p == '\'' || *p == '\"')) 
    	{
     		qchr = *p; 
     		p = lexer.skip(1);
     		p = lexer.skipWhite();
    	}
      		
    	if (lexer.getLength(wd, 1.0))
    	{
    		p = lexer.skipWhite();
     		if (p < endptr && *p == ',') 
     		{
     			p = lexer.skip(1);
     			p = lexer.skipWhite();
     		}
     		lexer.getLength(ht, 1.0);
     		p = lexer.skipWhite();
    	}
      		
    	if (qchr)
    	{
    		if (p >= endptr || *p != qchr)
     			error = -1;
    	}
    }
    else if (minorversion && ns_pdf && !strcmp(q, "minorversion"))
    {
      if (*p == '=') 
      	p = lexer.skip(1);
      p = lexer.skipWhite();
      char * kv = lexer.getFloatDecimal();
      if (kv) 
      {
        *minorversion = (unsigned)strtol(kv, NULL, 10);
        delete [] kv;
      }
    }
    else if (ns_pdf && !strcmp(q, "encrypt") && do_enc)
    {
    	*do_enc = 1;
      *owner_pw = *user_pw = 0;
      while (!error && p < endptr)
      {
      	char  *kp = lexer.getCIdent();
        if (!kp)
          break;
        else
        {
        	p = lexer.skipWhite();
        	if (!strcmp(kp, "ownerpw"))
        	{
        		p = lexer.getString(owner_pw, 100, &slen, &ok);
        		if (!ok)
        			error = -1;
        	}
        	else if (!strcmp(kp, "userpw"))
        	{
        		p = lexer.getString(user_pw, 100, &slen, &ok);
        		if (!ok)
        			error = -1;
        	}
        	else if (!strcmp(kp, "length"))
        	{
        		double kbs = 0.0;
        		p = lexer.getNumber(&kbs, &ok);
        		if (ok)
        		{
        			if (key_bits)
        				*key_bits = (unsigned)(kbs);
        		}
        		else
        			error = -1;
        	}
        	else if (!strcmp(kp, "perm"))
        	{
        		double perm = 0.0;
        		p = lexer.getNumber(&perm, &ok);
        		if (ok)
        		{
        			if (permission)
        				*permission = (unsigned)(perm);
        		}
        		else
        			error = -1;
        	}
        	else
        		error = -1;
        		
        	delete [] kp;
        }
        
        p = lexer.skipWhite();
      }
    }
    
    delete [] q;
  }
  
  return  error;
}

void XWDVICore::setCIDFontFixedPitch(bool e)
{
	if (e)
	{
		optFlags |= OPT_CIDFONT_FIXEDPITCH;
		XWCIDFontCache::setFlags(CIDFONT_FORCE_FIXEDPITCH);
	}
	else
	{
		optFlags &= ~(OPT_CIDFONT_FIXEDPITCH);
		XWCIDFontCache::setFlags(0);
	}
}

void XWDVICore::setDecimalDigits(int i)
{
	dev->setDecimalDigits(i);
}

void XWDVICore::setDPI(int hDPIA, int vDPIA)
{
	hDPI = hDPIA;
	vDPI = vDPIA;
}

void XWDVICore::setFontMapFirstMatch(bool e)
{
	if (e)
		optFlags |= OPT_FONTMAP_FIRST_MATCH;
	else
		optFlags &= ~(OPT_FONTMAP_FIRST_MATCH);
}

void XWDVICore::setIgnoreColors(int i)
{
	dev->setIgnoreColors(i);
}

void XWDVICore::setImageAttr(int id, 
	                         long width, 
	                         long height, 
	                         double xdensity, 
	                         double ydensity, 
	                         double llx, 
	                         double lly, 
	                         double urx, 
	                         double ury)
{
	if (imageCache)
		imageCache->setAttr(id, width, height, xdensity, ydensity, llx, lly, urx, ury);
}

void XWDVICore::setOffset(double hoff, double voff)
{
	x_offset = hoff;
	y_offset = voff;
}

void XWDVICore::setPageNo(long n)
{
	pageNo = n;
	if (!(xref->isScanning()))
		xref->setCurrentPageNumber(n);
}

void XWDVICore::setPaperSize(double w, double h)
{
	paperWidth = w;
	paperHeight = h; 
	pageWidth = paperWidth;
  pageHeight = paperHeight;
	if (!mpMode)
  {
   	PDFRect mediabox;
   	mediabox.llx = 0.0;
  	mediabox.lly = 0.0;
  	mediabox.urx = paperWidth;
  	mediabox.ury = paperHeight;
  	xref->setMediaBox(pageNo + 1, &mediabox);
  }
}

void XWDVICore::setTPICTransFill(bool e)
{
	if (e)
		optFlags |= OPT_TPIC_TRANSPARENT_FILL;
	else
		optFlags &= ~(OPT_TPIC_TRANSPARENT_FILL);
}

#ifdef XW_BUILD_PS_LIB

void XWDVICore::setUseSpecial(bool e)
{
	usesspecial = e;
}

#endif //XW_BUILD_PS_LIB

void XWDVICore::type0T1SetToUnicode(int t0t1id, XWObject *cmap_ref)
{
	if (type0FontCache)
		type0FontCache->setToUnicode(t0t1id, cmap_ref);
}

void XWDVICore::needMoreFonts(int n)
{
	if (numLoadedFonts + n > maxLoadedFonts) 
	{
    	maxLoadedFonts += TEX_FONTS_ALLOC_SIZE;
    	loadedFonts = (DVILoadedFont*)realloc(loadedFonts, maxLoadedFonts * sizeof(DVILoadedFont));
  	}
}

