/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "XWNumberUtil.h"
#include "XWFontFileSFNT.h"
#include "XWFontFileCFF.h"
#include "XWSFNTType.h"
#include "XWCFFType.h"
#include "XWLexer.h"
#include "cidbasefont.h"
#include "XWTexFont.h"
#include "XWT1Type.h"
#include "XWType0Font.h"
#include "XWAdobeGlyphList.h"
#include "XWFontCache.h"
#include "XWCIDFont.h"


#define PDF_CID_SUPPORT_MIN 2
#define PDF_CID_SUPPORT_MAX 6

#ifndef PDFUNIT
#define PDFUNIT(v) (round((1000.0*(v))/(head->unitsPerEm),1))
#endif

static struct {
  const char *registry;
  const char *ordering;
  /* Heighest Supplement values supported by PDF-1.0, 1.1, ...; see
   * also http://partners.adobe.com/public/developer/font/index.html#ckf
   */
  int   supplement[16];
} CIDFont_stdcc_def[] = {
  {"Adobe", "UCS",      {-1, -1, 0, 0, 0, 0, 0, 0}}, 
  {"Adobe", "GB1",      {-1, -1, 0, 2, 4, 4, 4, 4}}, 
  {"Adobe", "CNS1",     {-1, -1, 0, 0, 3, 4, 4, 4}},
  {"Adobe", "Japan1",   {-1, -1, 2, 2, 4, 5, 6, 6}},
  {"Adobe", "Korea1",   {-1, -1, 1, 1, 2, 2, 2, 2}},
  {"Adobe", "Identity", {-1, -1, 0, 0, 0, 0, 0, 0}},
  {NULL, NULL, {0,0,0,0,0,0,0,0}}
};
#define UCS_CC    0
#define ACC_START 1
#define ACC_END   4

static struct {
  const char *name;
  int   index;
} CIDFont_stdcc_alias[] = {
  {"AU",     0}, {"AG1",    1}, {"AC1",    2}, {"AJ1",    3}, {"AK1",    4}, {"AI", 5},
  {"UCS",    0}, {"GB1",    1}, {"CNS1",   2}, {"Japan1", 3}, {"Korea1", 4}, {"Identity", 5},
  {"U",      0}, {"G",      1}, {"C",      2}, {"J",      3}, {"K",      4}, {"I", 5},
  {NULL,     0}
};


void
pdf_font_make_uniqueTag (char *tag)
{
  	static char first = 1;
  	if (first) 
  	{
    	srand(time(NULL));
    	first = 0;
  	}

  	for (char i = 0; i < 6; i++) 
  	{
    	char ch = rand() % 26;
    	tag[i] = ch + 'A';
  	}
  	
  	tag[6] = '\0';
}


static void
validate_name (char *fontname, int len)
{
  	static const char *badstrlist[] = {
    	"-WIN-RKSJ-H",
    	"-WINP-RKSJ-H",
    	"-WING-RKSJ-H",
    	"-90pv-RKSJ-H",
    	NULL
  	};

	int count = 0;
  	for (int  i = 0; i < len; i++) 
  	{
    	if (fontname[i] == 0) 
    	{
      		memmove(fontname + i, fontname + i + 1, len - i);
      		count++;
      		len--;
    	}
  	}
  	
  	fontname[len] = '\0';

  	/* For some fonts that have bad PS name. ad hoc. remove me. */
  	for (int i = 0; badstrlist[i] != NULL; i++) 
  	{
    	char * p = strstr(fontname, badstrlist[i]);
    	if (p && p > fontname) 
    	{
      		p[0] = '\0';
      		len  = (int) (p - fontname);
      		break;
    	}
  	}
}

static long opt_flags = 0;

static struct
{
  const char *name;
  int         must_exist;
} required_table[] = {
  {"OS/2", 0}, {"head", 1}, {"hhea", 1}, {"loca", 1}, {"maxp", 1},
  {"name", 1}, {"glyf", 1}, {"hmtx", 1}, {"fpgm", 0}, {"cvt ", 0},
  {"prep", 0}, {NULL, 0}
};


#define WIN_UCS_INDEX_MAX   1
#define KNOWN_ENCODINGS_MAX 9
static struct
{
  unsigned short  platform;
  unsigned short  encoding;
  const char     *pdfnames[5];
} known_encodings[] = {
  {TT_WIN, TT_WIN_UCS4,     {"UCSms-UCS4", "UCSms-UCS2", "UCS4", "UCS2", NULL}},
  {TT_WIN, TT_WIN_UNICODE,  {"UCSms-UCS4", "UCSms-UCS2", "UCS4", "UCS2", NULL}},
  {TT_WIN, TT_WIN_SJIS,     {"90ms-RKSJ", NULL}},
  {TT_WIN, TT_WIN_RPC,      {"GBK-EUC",   NULL}},
  {TT_WIN, TT_WIN_BIG5,     {"ETen-B5",   NULL}},
  {TT_WIN, TT_WIN_WANSUNG,  {"KSCms-UHC", NULL}},
  {TT_MAC, TT_MAC_JAPANESE, {"90pv-RKSJ", NULL}},
  {TT_MAC, TT_MAC_TRADITIONAL_CHINESE, {"B5pc",     NULL}},
  {TT_MAC, TT_MAC_SIMPLIFIED_CHINESE,  {"GBpc-EUC", NULL}},
  {TT_MAC, TT_MAC_KOREAN,   {"KSCpc-EUC", NULL}},
  {0, 0, {NULL}}
};

XWCIDOpt::XWCIDOpt()
	:name(0),
	 csi(0),
	 index(0),
	 style(0),
	 embed(0),
	 stemv(0)
{
}

XWCIDOpt::~XWCIDOpt()
{
	if (csi)
	{
		if (csi->registry)
			delete [] csi->registry;
			
		if (csi->ordering)
			delete [] csi->ordering;
			
		delete csi;
	}
}

XWCIDWidths::XWCIDWidths()
{
	count = 0;
	max = 0;
	widths = 0;
	types = 0;
}

XWCIDWidths::~XWCIDWidths()
{
	if (widths)
	{
		for (int i = 0; i <= count; i++)
		{
			if (widths[i])
			{
				if (types[i] == 1)
				{
					XWCIDWidths * tmp = (XWCIDWidths*)(widths[i]);
					delete tmp;
				}
				else
				{
					Widths * tmp = (Widths*)(widths[i]);
					if (tmp->values)
						free(tmp->values);
					delete tmp;
				}
			}
		}
		
		free(widths);
		free(types);
	}
}

void XWCIDWidths::add(double num)
{
	if (types && types[count] == 0)
	{
		Widths * W = (Widths*)(widths[count]);
		W->values = (double*)realloc(W->values, (W->len + 1) * sizeof(double));
		W->values[W->len] = num;
		W->len += 1;
	}
	else
	{
		if (types)
			count++;
			
		if (count >= max)
		{
			max += 2;
			widths = (void**)realloc(widths, max * sizeof(void*));
			types = (char*)realloc(types, max * sizeof(char));
		}
		
		Widths * W = new Widths;
		W->len = 1;
		W->values = (double*)malloc(sizeof(double));
		W->values[0] = num;
		widths[count] = W;
		types[count] = 0;
	}
}

void XWCIDWidths::add(XWCIDWidths * WS)
{
	if (types)
		count++;
		
	if (count >= max)
	{
		max += 2;
		widths = (void**)realloc(widths, max * sizeof(void*));
		types = (char*)realloc(types, max * sizeof(char));
	}
	
	types[count] = 1;
	widths[count] = WS;
}

XWCIDFont::XWCIDFont(QObject * parentA)
	:QObject(parentA),
	 ident(0),
	 name(0),
	 fontname(0),
	 subtype(-1),
	 flags(FONT_FLAG_NONE),
	 csi(0),
	 options(0),
	 loaded(false),
	 ft_face(0),
	 ft_to_gid(0)
{
	parent[0] = -1;
	parent[1] = -1;
	
	dict.Type = 0;
	dict.Subtype = 0;
	dict.BaseFont = 0;
	dict.DW = 0.0;
	dict.W = 0;
	dict.DW2 = 0;
	dict.W2 = 0;
	
	desc.Type = 0;
	desc.FontName = 0;
	desc.Flags = 0;
	for (int i = 0; i < 4; i++)
		desc.FontBBox[i] = 0.0;
		
	desc.ItalicAngle = 0.0;
	desc.Ascent = 0.0;
	desc.Descent = 0.0;
	desc.CapHeight = 0.0;
	desc.XHeight = 0.0;
	desc.StemV = 0.0;
	desc.AvgWidth = 0.0;
	locking = false;
}

XWCIDFont::~XWCIDFont()
{
	if (ident)
		delete [] ident;
		
	if (name)
		delete [] name;
		
	if (fontname)
		delete [] fontname;
		
	if (csi)
	{
		if (csi->registry)
			delete [] csi->registry;
			
		if (csi->ordering)
			delete [] csi->ordering;
			
		delete csi;
	}
	
	if (options)
		delete options;
		
	if (dict.Type)
		delete [] dict.Type;
		
	if (dict.Subtype)
		delete [] dict.Subtype;
		
	if (dict.BaseFont)
		delete [] dict.BaseFont;
		
	if (dict.W)
		delete dict.W;
		
	if (dict.DW2)
		delete dict.DW2;
		
	if (dict.W2)
		delete dict.W2;
		
	if (desc.Type)
		delete [] desc.Type;
		
	if (desc.FontName)
		delete [] desc.FontName;
}

void XWCIDFont::attachParent(int parent_id, int wmode)
{
	if (wmode < 0 || wmode > 1 || parent[wmode] >= 0)
		return ;
		
	parent[wmode] = parent_id;
}

int XWCIDFont::getEmbedding()
{
	if (!options)
		return 0;
		
	return options->embed;
}

int XWCIDFont::getOptIndex()
{
	if (!options)
		return 0;
		
	return options->index;
}

int XWCIDFont::getParentID(int wmode)
{
	if (wmode < 0 || wmode > 1)
		return -1;
		
	return parent[wmode];
}

bool XWCIDFont::isACCFont()
{
	if (!csi)
		return false;
		
	for (int i = ACC_START; i <= ACC_END ; i++) 
	{
    	if (!strcmp(csi->registry, CIDFont_stdcc_def[i].registry) &&
			!strcmp(csi->ordering, CIDFont_stdcc_def[i].ordering))
      		return true;
  	}

  	return 0;
}

bool XWCIDFont::isFixedPitch()
{
	return (opt_flags & CIDFONT_FORCE_FIXEDPITCH);
}

bool XWCIDFont::isUCSFont()
{
	if (!strcmp(csi->ordering, "UCS") ||
      	!strcmp(csi->ordering, "UCS2"))
    	return true;
    	
  	return false;
}

int XWCIDFont::load(XWCMap ** tounicodecmap,
	                uchar ** streamdata, 
	                long * streamlen,
	                char ** usedchars,
	                int  * lastcid,
	                uchar** cidtogidmapA)
{
	locking = true;
	bool ret = false;
	switch (subtype)
	{
		case CIDFONT_TYPE0:
			if (getFlag(CIDFONT_FLAG_TYPE1))
      			ret = loadType0T1(tounicodecmap, streamdata, streamlen, usedchars, lastcid);
    		else if (getFlag(CIDFONT_FLAG_TYPE1C))
      			ret = loadType0T1C(streamdata, streamlen, usedchars, lastcid);
    		else
      			ret = loadType0(streamdata, streamlen, usedchars, lastcid);
			break;
			
		case CIDFONT_TYPE2:
			ret = loadType2(streamdata, streamlen, usedchars, lastcid, cidtogidmapA);
			break;
			
		default:
			break;
	}
	
	if (ret)
		loaded = true;
		
	locking = false;
		
	return ret ? 0 : -1;
}

int XWCIDFont::open(const char *nameA,
		            CIDSysInfo *cmap_csi, 
		            XWCIDOpt *opt)
{
	locking = true;
	if (openType0(nameA, cmap_csi, opt) < 0 &&
		openType2(nameA, cmap_csi, opt) < 0 &&
		openType0T1(nameA, cmap_csi, opt)  < 0 &&
		openType0T1C(nameA, cmap_csi, opt) < 0 &&
		openBase(nameA, cmap_csi, opt) < 0)
	{
		locking = false;
		return -1;
	}
	locking = false;
	return 0;
}

void XWCIDFont::addCIDHMetrics(uchar *CIDToGIDMap, 
		 				       ushort last_cid,
		 				       XWTTMaxpTable *maxp,
		 				       XWTTHeadTable *head, 
		 				       TTLongMetrics *hmtx)
{
	double defaultAdvanceWidth = PDFUNIT(hmtx[0].advance);
	if (dict.W)
		delete dict.W;
		
	dict.W = new XWCIDWidths;
	long start = 0, prev = 0;
	int  empty = 1;
	XWCIDWidths *W = 0;
	for (ushort cid = 0; cid <= last_cid; cid++)
	{
		ushort gid = CIDToGIDMap ? ((CIDToGIDMap[2*cid] << 8)|CIDToGIDMap[2*cid+1]) : cid;
    	if (gid >= maxp->numGlyphs || (cid != 0 && gid == 0))
      		continue;
      		
      	if (gid >= maxp->numGlyphs || (cid != 0 && gid == 0))
      		continue;
      		
    	double advanceWidth = PDFUNIT(hmtx[gid].advance);
    	if (advanceWidth == defaultAdvanceWidth)
    	{
    		if (W)
    		{
    			dict.W->add(start);
    			dict.W->add(W);
    			W = 0;
				empty = 0;
    		}
    	}
    	else
    	{
    		if (cid != prev + 1 &&  W)
    		{
    			dict.W->add(start);
    			dict.W->add(W);
    			W = 0;
				empty = 0;
    		}
    		
    		if (!W)
    		{
    			W = new XWCIDWidths;
    			start = cid;
    		}
    		
    		W->add(advanceWidth);
    		prev = cid;
    	}
	}
	
	if (W)
	{
		dict.W->add(start);
		dict.W->add(W);
		empty = 0;
	}
	
	if (empty && dict.W)
	{
		delete dict.W;
		dict.W = 0;
	}
	
	dict.DW = defaultAdvanceWidth;
}

void XWCIDFont::addCIDMetrics(XWFontFileSFNT *sfont,
					   		  uchar *CIDToGIDMap, 
					          ushort last_cid, 
					          int need_vmetrics)
{
	XWTTHeadTable * head = sfont->readHeadTable();
  	XWTTMaxpTable * maxp = sfont->readMaxpTable();
  	XWTTHHeaTable * hhea = sfont->readHHeaTable();
  	
  	sfont->locateTable("hmtx");
  	TTLongMetrics * hmtx = sfont->readLongMetrics(maxp->numGlyphs, hhea->numOfLongHorMetrics, hhea->numOfExSideBearings);

  	addCIDHMetrics(CIDToGIDMap, last_cid, maxp, head, hmtx);
  	if (need_vmetrics)
    	addCIDVMetrics(sfont, CIDToGIDMap, last_cid, maxp, head, hmtx);

  	free(hmtx);
  	delete hhea;
  	delete maxp;
  	delete head;
}

void XWCIDFont::addCIDVMetrics(XWFontFileSFNT *sfont,
		                       uchar *CIDToGIDMap, 
		                       ushort last_cid,
		 				       XWTTMaxpTable *maxp,
		 				       XWTTHeadTable *head, 
		 				       TTLongMetrics *hmtx)
{
	XWTTVORGTable * vorg = sfont->readVORGTable();
	if (!vorg)
		return ;
		
	double defaultVertOriginY = PDFUNIT(vorg->defaultVertOriginY);
	XWTTVHeaTable * vhea = 0;
	TTLongMetrics * vmtx = 0;
	if (sfont->findTablePos("vhea") > 0)
    	vhea = sfont->readVHeaTable();
    if (vhea && sfont->findTablePos("vmtx") > 0)
    {
    	sfont->locateTable("vmtx");
    	vmtx = sfont->readLongMetrics(maxp->numGlyphs, vhea->numOfLongVerMetrics, vhea->numOfExSideBearings);
    }
    
    double defaultAdvanceHeight = 1000;
    if (sfont->findTablePos("OS/2") <= 0)
    {
    	XWTTOs2Table * os2 = sfont->readOs2Table();
    	defaultVertOriginY   = PDFUNIT(os2->sTypoAscender);
    	defaultAdvanceHeight = PDFUNIT(os2->sTypoAscender - os2->sTypoDescender);
    	delete os2;
    }
    
    if (dict.W2)
    	delete dict.W2;
    	
    dict.W2 = new XWCIDWidths;
    int  empty = 1;
    for (ushort cid = 0; cid <= last_cid; cid++)
    {
    	ushort gid = CIDToGIDMap ? ((CIDToGIDMap[2*cid] << 8)|CIDToGIDMap[2*cid+1]) : cid;
    	if (gid >= maxp->numGlyphs || (cid != 0 && gid == 0))
      		continue;
      		
      	double advanceHeight = vmtx ? PDFUNIT(vmtx[gid].advance) : defaultAdvanceHeight;
    	double vertOriginX   = PDFUNIT(hmtx[gid].advance*0.5);
    	double vertOriginY   = defaultVertOriginY;
    	for (ushort i = 0;  i < vorg->numVertOriginYMetrics && gid > vorg->vertOriginYMetrics[i].glyphIndex; i++)
    	{
    		if (gid == vorg->vertOriginYMetrics[i].glyphIndex)
				vertOriginY = PDFUNIT(vorg->vertOriginYMetrics[i].vertOriginY);
    	}
    	
    	if (vertOriginY != defaultVertOriginY || 
			advanceHeight != defaultAdvanceHeight) 
		{
      		dict.W2->add(cid);
      		dict.W2->add(cid);
      		dict.W2->add(-advanceHeight);
      		dict.W2->add(vertOriginX);
      		dict.W2->add(vertOriginY);
      		empty = 0;
    	}
    }
    
    if (empty && dict.W2)
    {
    	delete dict.W2;
    	dict.W2 = 0;
    }
    
    if (dict.DW2)
    {
    	delete dict.DW2;    	
    	dict.DW2 = 0;
    }
    
    if (defaultVertOriginY != 880 || defaultAdvanceHeight != 1000)
    {
    	dict.DW2 = new XWCIDWidths;
    	dict.DW2->add(defaultVertOriginY);
    	dict.DW2->add(-defaultAdvanceHeight);
    }
    
    
  	delete vorg;

  	if (vmtx)
    	free(vmtx);
  	if (vhea)
    	delete vhea;
}

void XWCIDFont::addMetrics(XWFontFileCFF *cffont,
	     			       uchar *CIDToGIDMap,
	     			       double *widths, 
	     			       double default_width, 
	     			       ushort last_cid)
{
	if (!cffont->topDictKnow("FontBBox")) 
		return ;
		
	for (int i = 0; i < 4; i++) 
	{
    	double val = cffont->topDictGet("FontBBox", i);
    	desc.FontBBox[i] = round(val, 1.0);
  	}
  	
  	int parent_id = -1;
  	if ((parent_id = getParentID(0)) < 0 &&
      	(parent_id = getParentID(1)) < 0)
    {
    	return ;
    }
    
    XWFontCache cache(true);
	char * used_chars = cache.getType0Font(parent_id)->getUsedChars();
	if (!used_chars)
		return ;
		
	if (dict.W)
    	delete dict.W;
    	
    dict.W = new XWCIDWidths;
    for (ushort cid = 0; cid <= last_cid; cid++)
    {
    	if (is_used_char2(used_chars, cid))
    	{
    		ushort gid = (CIDToGIDMap[2*cid] << 8)|CIDToGIDMap[2*cid+1];
    		if (widths[gid] != default_width) 
    		{
				dict.W->add(cid);
				dict.W->add(cid);
				dict.W->add(round(widths[gid], 1.0));
      		}
    	}
    }
    
    dict.DW = default_width;
}

void XWCIDFont::addTTCIDHMetrics(XWTTGlyphs *g,
		                         char *used_chars, 
		                         uchar *cidtogidmap, 
		                         ushort last_cid)
{
	if (g->dw != 0 && g->dw <= g->emsize) 
    	dict.DW = round(1000.0 * g->dw / g->emsize, 1);
  	else 
    	dict.DW = round(1000.0 * g->gd[0].advw / g->emsize, 1);
    	
    long start = 0, prev = 0;
    int  empty = 1;
    if (dict.W)
    	delete dict.W;
    	
    dict.W = new XWCIDWidths;
    XWCIDWidths * W = 0;
    for (ushort cid = 0; cid <= last_cid; cid++)
    {
    	if (!is_used_char2(used_chars, cid))
      		continue;
      		
      	ushort gid = (cidtogidmap) ? ((cidtogidmap[2*cid] << 8)|cidtogidmap[2*cid+1]) : cid;
    	ushort idx = g->getIndex(gid);
    	if (cid != 0 && idx == 0)
      		continue;
      		
      	double width = round(1000.0 * (g->gd)[idx].advw / g->emsize, 1);
      	if (width == dict.DW)
      	{
      		if (W)
      		{
      			dict.W->add(start);
      			dict.W->add(W);
      			W = 0;
				empty = 0;
      		}
      	}
      	else
      	{
      		if (cid != prev + 1)
      		{
      			if (W)
      			{
      				dict.W->add(start);
	  				dict.W->add(W);
	  				W = 0;
	  				empty = 0;
      			}
      		}
      		
      		if (!W) 
      		{
				W = new XWCIDWidths;
				start = cid;
      		}
      		W->add(width);
      		prev = cid;
      	}
    }
    
    if (W) 
    {
    	dict.W->add(start);
    	dict.W->add(W);
    	empty = 0;
  	}
  	
  	if (empty && dict.W)
	{
		delete dict.W;
		dict.W = 0;
	}
}

void XWCIDFont::addTTCIDVMetrics(XWTTGlyphs *g,
		   				         char *used_chars, 
		   				         /*uchar *cidtogidmap,*/ 
		   				         ushort last_cid)
{
	double defaultVertOriginY   = round(1000.0 * (g->default_advh - g->default_tsb) / g->emsize, 1);
  	double defaultAdvanceHeight = round(1000.0 * g->default_advh / g->emsize, 1);
  	
  	if (dict.W2)
    	delete dict.W2;
    	
    dict.W2 = new XWCIDWidths;
    
    int  empty = 1;
    for (ushort cid = 0; cid <= last_cid; cid++)
    {
    	if (!is_used_char2(used_chars, cid))
      		continue;
      		
      	//ushort gid = (cidtogidmap) ? ((cidtogidmap[2*cid] << 8)|cidtogidmap[2*cid+1]) : cid;
    	ushort idx = g->getIndex(cid);
    	if (cid != 0 && idx == 0)
      		continue;
      		
      	double advanceHeight = round(1000.0 * g->gd[idx].advh / g->emsize, 1);
    	double vertOriginX   = round(1000.0 * 0.5 * (g->gd[idx].advw) / g->emsize, 1);
    	double vertOriginY   = round(1000.0 * (g->gd[idx].tsb + g->gd[idx].ury) / g->emsize, 1);
    	if (vertOriginY != defaultVertOriginY ||
			advanceHeight != defaultAdvanceHeight) 
		{
      		dict.W2->add(cid);
      		dict.W2->add(cid);
      		dict.W2->add(-advanceHeight);
      		dict.W2->add(vertOriginX);
      		dict.W2->add(vertOriginY);
      		empty = 0;
    	}
    }
    
    if (dict.DW2)
    {
    	delete dict.DW2;    	
    	dict.DW2 = 0;
    }
    
    if (defaultVertOriginY != 880 || defaultAdvanceHeight != 1000) 
    {
    	dict.DW2 = new XWCIDWidths;
    	dict.DW2->add(defaultVertOriginY);
    	dict.DW2->add(-defaultAdvanceHeight);
  	}
  	
  	if (empty && dict.W2)
    {
    	delete dict.W2;
    	dict.W2 = 0;
    }
}

long XWCIDFont::cidToCode(XWCMap *cmap, ushort cid)
{
	if (!cmap)
    	return cid;
    	
    uchar inbuf[2], outbuf[32];
  	long  inbytesleft = 2, outbytesleft = 32;
  	inbuf[0] = (cid >> 8) & 0xff;
  	inbuf[1] = cid & 0xff;
  	uchar * p = inbuf; 
  	uchar * q = outbuf;
  	
  	cmap->decodeChar((const uchar **) &p, &inbytesleft, &q, &outbytesleft);

  	if (inbytesleft != 0)
    	return 0;
  	else if (outbytesleft == 31)
    	return (long) outbuf[0];
  	else if (outbytesleft == 30)
    	return (long) (outbuf[0] << 8|outbuf[1]);
  	else if (outbytesleft == 28)
    {
    	ushort hi = outbuf[0] << 8|outbuf[1];
    	ushort lo = outbuf[2] << 8|outbuf[3];
    	if (hi >= 0xd800 && hi <= 0xdbff && lo >= 0xdc00 && lo <= 0xdfff)
      	return (long) ((hi - 0xd800) * 0x400 + 0x10000 + lo - 0xdc00);
    	else
      	return (long) (hi << 16|lo);
    }

  	return 0;
}

XWCMap * XWCIDFont::createToUnicodeCMap(XWFontFileCFF *cffont, 
	                                    const char *used_glyphs)
{
	static unsigned char range_min[2] = {0x00, 0x00};
  	static unsigned char range_max[2] = {0xff, 0xff};
  	
  	XWCMap * cmap = new XWCMap;

  	char * cmap_name = new char[strlen(fontname)+strlen("-UTF16")+1];
  	strcpy(cmap_name, fontname);
  	strcat(cmap_name, "-UTF16");
  	cmap->setName(cmap_name);
  	delete [] cmap_name;

  	cmap->setWMode(0);
  	cmap->setType(CMAP_TYPE_TO_UNICODE);
  	cmap->setCIDSysInfo(&CSI_UNICODE);
  	cmap->addCodeSpaceRange(range_min, range_max, 2);
  	
  	long glyph_count = 0;
  	long total_fail_count = 0;
  	uchar  wbuf[1024];
  	uchar * p      = wbuf;
  	uchar * endptr = wbuf + 1024;
  	XWFontCache cache(true);
  	for (ushort cid = 1; cid < (ushort)(cffont->getNumGlyphs()); cid++)
  	{
  		if (is_used_char2(used_glyphs, cid))
  		{
  			wbuf[0] = (cid >> 8) & 0xff;
      		wbuf[1] = (cid & 0xff);

      		p = wbuf + 2;
      		ushort gid = cffont->lookupChartsetsInverse(cid);
      		if (gid == 0)
				continue;
				
			char * glyph = cffont->getString(gid);
      		if (glyph)
      		{
      			int   fail_count = 0;
      			long len = cache.aglSPutUTF16BE(glyph, &p, endptr, &fail_count);
      			if (len < 1 || fail_count) 
	  				total_fail_count += fail_count;
				else 
	  				cmap->addBFChar(wbuf, 2, wbuf+2, len);
				delete [] glyph;
      		}
      		
      		glyph_count++;
  		}
  	}
  	
  	return cmap;
}

XWCMap * XWCIDFont::findToCodeCMap(const char *reg, 
	                               const char *ord, 
	                               int select)
{
	if (!reg || !ord || select < 0 || select > KNOWN_ENCODINGS_MAX)
		return 0;
		
	if (!strcmp(ord, "UCS") && select <= WIN_UCS_INDEX_MAX)
    	return 0;
    	
    int   cmap_id = -1;
    XWFontCache cache(true);
    for (int i = 0; cmap_id < 0 && i < 5; i++)
    {
    	char * append = (char *) known_encodings[select].pdfnames[i];
    	if (!append)
      		break;
      		
      	char * cmap_name = new char[strlen(reg) + strlen(ord) + strlen(append) + 3];
    	sprintf(cmap_name, "%s-%s-%s", reg, ord, append);
    	cmap_id = cache.findCMap(cmap_name);
    	delete [] cmap_name;
    }
    
    if (cmap_id < 0)
    	return 0;
    	
    return cache.getCMap(cmap_id);
}

ushort XWCIDFont::fixCJKSymbols(ushort code)
{
  	static struct
  	{
    	ushort alt1;
    	ushort alt2;
  	} CJK_Uni_symbols[] = {
    	{0x2014, 0x2015},
    	{0x2016, 0x2225},
    	{0x203E, 0xFFE3},
    	{0x2026, 0x22EF},
    	{0x2212, 0xFF0D},
    	{0x301C, 0xFF5E},
    	{0xFFE0, 0x00A2},
    	{0xFFE1, 0x00A3},
    	{0xFFE2, 0x00AC},
    	{0xFFFF, 0xFFFF},
  	};
#define NUM_CJK_SYMBOLS (sizeof(CJK_Uni_symbols)/sizeof(CJK_Uni_symbols[0]))

  	ushort alt_code = code;
  	for (int i = 0; i < NUM_CJK_SYMBOLS; i++) 
  	{
    	if (CJK_Uni_symbols[i].alt1 == code) 
    	{
      		alt_code = CJK_Uni_symbols[i].alt2;
      		break;
    	} 
    	else if (CJK_Uni_symbols[i].alt2 == code) 
    	{
      		alt_code = CJK_Uni_symbols[i].alt1;
      		break;
    	}
  	}

  	return alt_code;
}

void XWCIDFont::getFontAttr(XWFontFileCFF *cffont)
{
	static const char *L_c[] = 
	{
    	"H", "P", "Pi", "Rho", NULL
  	};
  	
  	static const char *L_d[] = 
  	{
    	"p", "q", "mu", "eta", NULL
  	};
  	
  	static const char *L_a[] = 
  	{
    	"b", "h", "lambda", NULL
  	};
  	
  	double defaultwidth = 500.0;
  	double nominalwidth = 0.0;
  	
  	desc.CapHeight = 680.0;
  	desc.Ascent = 690.0;
  	desc.Descent = -190.0;
  	
  	if (cffont->topDictKnow("FontBBox")) 
  	{
    	desc.CapHeight = desc.Ascent = cffont->topDictGet("FontBBox", 3);
    	desc.Descent = cffont->topDictGet("FontBBox", 1);
  	}
  	
  	desc.StemV = 88.0;
  	if (cffont->privDictKnow(0, "StdVW"))
  		desc.StemV = cffont->privDictGet(0, "StdVW", 0);
  		
  	desc.ItalicAngle = 0.0;
  	desc.Flags = 0;
  	if (cffont->topDictKnow("ItalicAngle")) 
  	{
    	desc.ItalicAngle = cffont->topDictGet("ItalicAngle", 0);
    	if (desc.ItalicAngle != 0.0)
      		desc.Flags |= FONT_FLAG_ITALIC;
  	}
  	
  	XWT1GInfo gm;
  	ushort gid = cffont->lookupGlyph("space");
  	XWCFFIndex * cstrings = cffont->getCStrings();
  	XWCFFIndex * subrs = cffont->getSubrs(0);
  	if (gid >= 0 && gid < cstrings->count) 
  	{
    	gm.getMetrics(cstrings->data + cstrings->offset[gid] - 1,
		       		  cstrings->offset[gid+1] - cstrings->offset[gid],
		       		  subrs);
    	defaultwidth = gm.wx;
  	}
  	
  	for (int i = 0; L_c[i] != NULL; i++) 
  	{
    	gid = cffont->lookupGlyph(L_c[i]);
    	if (gid >= 0 && gid < cstrings->count) 
    	{
      		gm.getMetrics(cstrings->data + cstrings->offset[gid] - 1,
			 			  cstrings->offset[gid+1] - cstrings->offset[gid],
			 			  subrs);
      		desc.CapHeight = gm.bbox.ury;
      		break;
    	}
  	}
  	
  	for (int i = 0; L_d[i] != NULL; i++) 
  	{
    	gid = cffont->lookupGlyph(L_d[i]);
    	if (gid >= 0 && gid < cstrings->count) 
    	{
      		gm.getMetrics(cstrings->data + cstrings->offset[gid] - 1,
			 			  cstrings->offset[gid+1] - cstrings->offset[gid],
			 			  subrs);
      		desc.Descent = gm.bbox.lly;
      		break;
    	}
  	}

  	for (int i = 0; L_a[i] != NULL; i++) 
  	{
    	gid = cffont->lookupGlyph(L_a[i]);
    	if (gid >= 0 && gid < cstrings->count) 
    	{
      		gm.getMetrics(cstrings->data + cstrings->offset[gid] - 1,
			 			  cstrings->offset[gid+1] - cstrings->offset[gid],
			 			  subrs);
      		desc.Ascent = gm.bbox.ury;
      		break;
    	}
  	}
  	
  	if (defaultwidth != 0.0) 
  	{
    	cffont->privDictAdd(0, "defaultWidthX", 1);
    	cffont->privDictSet(0, "defaultWidthX", 0, defaultwidth);
  	}
  	
  	if (nominalwidth != 0.0) 
  	{
    	cffont->privDictAdd(0, "nominalWidthX", 1);
    	cffont->privDictSet(0, "nominalWidthX", 0, nominalwidth);
  	}
  	
  	if (cffont->privDictKnow(0, "ForceBold") && 
  		cffont->privDictGet(0, "ForceBold", 0)) 
  	{
    	desc.Flags |= FONT_FLAG_FORCEBOLD;
  	}
  	
  	if (cffont->privDictKnow(0, "IsFixedPitch") && 
  		cffont->privDictGet(0, "IsFixedPitch", 0)) 
  	{
    	desc.Flags |= FONT_FLAG_FIXEDPITCH;
  	}
  	
  	if (fontname && !strstr(fontname, "Sans")) 
    	desc.Flags |= FONT_FLAG_SERIF;
  	
  	desc.Flags |= FONT_FLAG_SYMBOLIC;
}

bool  XWCIDFont::loadType0(uchar ** streamdata, 
	                       long * streamlen,
	                       char ** usedchars,
	                	   int  * lastcid)
{
	if (isBaseFont())
		return true;
		
	if (!getEmbedding() && (opt_flags & CIDFONT_FORCE_FIXEDPITCH))
	{
		dict.DW = 1000;
		return true;
	}
	
	int parent_id = -1;
	if ((parent_id = getParentID(0)) < 0 && 
		(parent_id = getParentID(1)) < 0)
	{
		return false;
	}
	
	XWFontCache cache(true);
	char * used_chars = cache.getType0Font(parent_id)->getUsedChars();
	if (!used_chars)
		return false;
		
	XWFontFileSFNT * sfont = 0;
	if (ft_face)
		sfont = XWFontFileSFNT::load(ft_face, SFNT_TYPE_POSTSCRIPT, false);
	else
	 	sfont = XWFontFileSFNT::loadCIDType0(ident, false);
	if (!sfont)
		return false;
		
	XWFontFileCFF * cffont = sfont->makeCIDType0(options->index);
	if (!cffont)
	{
		delete sfont;
		return false;
	}
	
	long cid_count = CID_MAX + 1;
	if (cffont->topDictKnow("CIDCount"))
		cid_count = (long) (cffont->topDictGet("CIDCount", 0));
		
	cffont->readCharsets();
  	uchar * CIDToGIDMap = (uchar*)malloc((2*cid_count) * sizeof(uchar));
  	memset(CIDToGIDMap, 0, 2*cid_count);
  	add_to_used_chars2(used_chars, 0);
  	long cid = 0; 
  	long last_cid = 0; 
  	ushort num_glyphs = 0;
  	for (cid = 0; cid <= CID_MAX; cid++) 
  	{
    	if (is_used_char2(used_chars, cid)) 
    	{
      		ushort gid = cffont->lookupChartsets(cid);
      		if (cid != 0 && gid == 0) 
      		{
				used_chars[cid/8] &= ~(1 << (7 - (cid % 8)));
				continue;
      		}
      		
      		CIDToGIDMap[2*cid]   = (gid >> 8) & 0xff;
      		CIDToGIDMap[2*cid+1] = gid & 0xff;
      		last_cid = cid;
      		num_glyphs++;
    	}
  	}
  	
  	if (opt_flags & CIDFONT_FORCE_FIXEDPITCH) 
  		dict.DW = 1000;
  	else 
    	addCIDMetrics(sfont, CIDToGIDMap, last_cid, ((getParentID(1) < 0) ? 0 : 1));
    		
    if (!getEmbedding()) 
    {
    	free(CIDToGIDMap);
    	delete cffont;
    	delete sfont;
    	return true;
  	}
  	
  	cffont->readFDSelect();
  	cffont->readFDArray();
  	cffont->readPrivate();

  	cffont->readSubrs();
  	
  	long offset = (long)(cffont->topDictGet("CharStrings", 0));
  	cffont->seek(offset);
  	XWCFFIndex * idx = cffont->getIndexHeader();
  	offset = cffont->tellPosition();
  	ushort cs_count = 0;
  	if ((cs_count = idx->count) < 2) 
  	{
    	free(CIDToGIDMap);
    	delete cffont;
    	delete sfont;
    	return false;
  	}
  	
  	XWCFFCharsets * charset = new XWCFFCharsets;
  	charset->format = 0;
  	charset->num_entries = 0;
  	charset->data.glyphs = (ushort*)malloc(num_glyphs * sizeof(ushort));
  	
  	XWCFFFDSelect * fdselect = new XWCFFFDSelect;
  	fdselect->format = 3;
  	fdselect->num_entries = 0;
  	fdselect->data.ranges = (CFFRange3*)malloc(num_glyphs * sizeof(CFFRange3));
  	
  	XWCFFIndex * charstrings = new XWCFFIndex(num_glyphs+1);
  	long max_len = 2 * CS_STR_LEN_MAX;
  	charstrings->data = (uchar*)malloc(max_len * sizeof(uchar));
  	long charstring_len = 0;
  	
  	int prev_fd = -1; 
  	ushort gid = 0;
  	uchar * data = (uchar*)malloc(CS_STR_LEN_MAX * sizeof(uchar));
  	long size = 0;
  	XWCSGInfo gm;
  	XWCFFIndex * gsubr = cffont->getGsubr();
  	for (cid = 0; cid <= last_cid; cid++)
  	{
  		if (!is_used_char2(used_chars, cid))
      		continue;
      		
      	ushort gid_org = (CIDToGIDMap[2*cid] << 8)|(CIDToGIDMap[2*cid+1]);
      	if ((size = (idx->offset)[gid_org+1] - (idx->offset)[gid_org]) > CS_STR_LEN_MAX)
      	{
      		free(data);
      		delete charstrings;
      		delete fdselect;
      		delete charset;
      		free(CIDToGIDMap);
    		delete cffont;
    		delete sfont;
    		return false;
      	}
      	
      	if (charstring_len + CS_STR_LEN_MAX >= max_len) 
      	{
      		max_len = charstring_len + 2 * CS_STR_LEN_MAX;
      		charstrings->data = (uchar*)realloc(charstrings->data, max_len * sizeof(uchar));
    	}
    	
    	(charstrings->offset)[gid] = charstring_len + 1;
    	cffont->seekAbsolute(offset + (idx->offset)[gid_org] - 1);
    	cffont->read((char*)data, size);
    	uchar fd = cffont->lookupFDSelect(gid_org);
    	charstring_len += gm.copyCharString(charstrings->data + charstring_len,
					 						max_len - charstring_len,
					 						data, size,
					 						gsubr, 
					 						cffont->getSubrs(fd), 0, 0);
    	if (cid > 0 && gid_org > 0) 
    	{
      		charset->data.glyphs[charset->num_entries] = cid;
      		charset->num_entries += 1;
    	}
    	
    	if (fd != prev_fd) 
    	{
      		fdselect->data.ranges[fdselect->num_entries].first = gid;
      		fdselect->data.ranges[fdselect->num_entries].fd    = fd;
      		fdselect->num_entries += 1;
      		prev_fd = fd;
    	}
    	gid++;
  	}
  	
  	if (gid != num_glyphs)
  	{
  		free(data);
      	delete charstrings;
      	delete fdselect;
      	delete charset;
      	free(CIDToGIDMap);
    	delete cffont;
    	delete sfont;
    	return false;
  	}
  	
  	free(data);
  	delete idx;
  	free(CIDToGIDMap);
  	
  	(charstrings->offset)[num_glyphs] = charstring_len + 1;
  	charstrings->count = num_glyphs;
  	cffont->setNumGlyphs(num_glyphs);
  	cffont->setCString(charstrings);
  	cffont->setCharsets(charset);
  	cffont->setFDSelect(fdselect);
  	
  	cffont->setGsub(new XWCFFIndex((ushort)0));
  	for (uchar fd = 0; fd < cffont->getNumFDs(); fd++) 
  	{
    	cffont->setSubrs(fd, 0);
    	cffont->privDictRemove(fd, "Subrs");
  	}

  	writeFontFile(cffont, streamdata, streamlen);

  	delete cffont;
  	delete sfont;
  	
  	if (usedchars)
  		*usedchars = used_chars;
  		
  	if (lastcid)
  		*lastcid = last_cid;
  	
  	return true;
}

bool XWCIDFont::loadType0T1(XWCMap ** tounicodecmap,
	                        uchar ** streamdata, 
	                        long * streamlen,
	                        char ** usedchars,
	                        int  * lastcid)
{
	XWFontFileCFF * cffont = XWFontFileCFF::loadType1(ident, 0, 0);
	if (!cffont)
		return false;
		
	if (!fontname)
	{
		delete cffont;
		return false;
	}
	
	int hparent_id = getParentID(0);
    int vparent_id = getParentID(1);
    if (hparent_id < 0 && vparent_id < 0)
    {
    	delete cffont;
		return false;
    }
    
    XWFontCache cache(true);
    XWType0Font * hparent = 0;
    XWType0Font * vparent = 0;
	
	char * used_chars = 0;
	if (hparent_id >= 0)
    {
      	hparent    = cache.getType0Font(hparent_id);
      	used_chars = hparent->getUsedChars();
    }
    
    if (vparent_id >= 0)
    {
      	vparent    = cache.getType0Font(vparent_id);
      	used_chars = vparent->getUsedChars();
    }
    
    if (!used_chars)
    {
    	delete cffont;
		return false;
    }
    
    if (tounicodecmap)
    	*tounicodecmap = createToUnicodeCMap(cffont, used_chars);
    
	cffont->setName(fontname);
	getFontAttr(cffont);
	
	double defaultwidth = 0.0;
	if (cffont->privDictKnow(0, "defaultWidthX"))
		defaultwidth = cffont->privDictGet(0, "defaultWidthX", 0);
		
	double nominalwidth = 0.0;
	if (cffont->privDictKnow(0, "nominalWidthX"))
		nominalwidth = cffont->privDictGet(0, "nominalWidthX", 0);
		
	long num_glyphs = 0; 
	ushort last_cid = 0;
  	add_to_used_chars2(used_chars, 0);
  	
  	for (long i = 0; i < (cffont->getNumGlyphs() + 7)/8; i++) 
  	{
    	int c = used_chars[i];
    	for (int j = 7; j >= 0; j--) 
    	{
      		if (c & (1 << j)) 
      		{
				num_glyphs++;
				last_cid = (i + 1) * 8 - j - 1;
      		}
    	}
  	}
  	
  	XWCFFFDSelect * fdselect = new XWCFFFDSelect;
  	fdselect->format = 3;
    fdselect->num_entries = 1;
    fdselect->data.ranges = (CFFRange3*)malloc(sizeof(CFFRange3));
    fdselect->data.ranges[0].first = 0;
    fdselect->data.ranges[0].fd    = 0;
    cffont->setFDSelect(fdselect);
    
    uchar* CIDToGIDMap = (uchar*)malloc(2*(last_cid+1) * sizeof(uchar));
  	memset(CIDToGIDMap, 0, 2*(last_cid+1));
  	
  	XWCFFCharsets * charset = new XWCFFCharsets;
    charset->format = 0;
    charset->num_entries = num_glyphs-1;
    charset->data.glyphs = (ushort*)malloc((num_glyphs-1) * sizeof(ushort));

	ushort gid = 0;
    for (ushort cid = 0; cid <= last_cid; cid++) 
    {
      	if (is_used_char2(used_chars, cid)) 
      	{
			if (gid > 0)
	  			charset->data.glyphs[gid-1] = cid;
			CIDToGIDMap[2*cid  ] = (gid >> 8) & 0xff;
			CIDToGIDMap[2*cid+1] = gid & 0xff;
			gid++;
      	}
    }

    cffont->setCharsets(charset);
    
    cffont->topDictAdd("CIDCount", 1);
  	cffont->topDictSet("CIDCount", 0, last_cid + 1);

  	cffont->createFDArray(1);
  	cffont->fdarrayAdd(0, "FontName", 1);
  	cffont->fdarraySet(0, "FontName", 0, (double)(cffont->addString(fontname + 7, 1)));
  	cffont->fdarrayAdd(0, "Private", 2);
  	cffont->fdarraySet(0, "Private", 0, 0.0);
  	cffont->fdarraySet(0, "Private", 0, 0.0);
  	
  	cffont->topDictAdd("FDArray", 1);
  	cffont->topDictSet("FDArray", 0, 0.0);
  	
  	cffont->topDictAdd("FDSelect", 1);
  	cffont->topDictSet("FDSelect", 0, 0.0);

  	cffont->topDictAdd("charset", 1);
  	cffont->topDictSet("charset", 0, 0.0);

  	cffont->topDictAdd("CharStrings", 1);
  	cffont->topDictSet("CharStrings", 0, 0.0);
  	
  	double * widths = (double*)malloc(num_glyphs * sizeof(double));
  	int w_stat[1001];
    memset(w_stat, 0, sizeof(int)*1001);
    long offset  = 0L;
    XWCFFIndex * cstring = new XWCFFIndex((ushort)num_glyphs);
    cstring->data = 0;
    cstring->offset[0] = 1;
    gid = 0;
    long max = 0;
    XWT1GInfo gm;
    XWCFFIndex * cstrings = cffont->getCStrings();
    XWCFFIndex * subrs = cffont->getSubrs(0);
    for (ushort cid = 0; cid <= last_cid; cid++)
    {
    	if (!is_used_char2(used_chars, cid))
	  		continue;
	  		
	  	if (offset + CS_STR_LEN_MAX >= max) 
	  	{
			max += CS_STR_LEN_MAX*2;
			cstring->data = (uchar*)realloc(cstring->data, max * sizeof(uchar));
      	}
      	
      	offset += gm.convertCharString(cstring->data + cstring->offset[gid] - 1, CS_STR_LEN_MAX,
					  				   cstrings->data + cstrings->offset[cid] - 1,
					  				   cstrings->offset[cid+1] - cstrings->offset[cid],
					  				   subrs, defaultwidth, nominalwidth);
      	cstring->offset[gid+1] = offset + 1;
      	if (gm.use_seac)
      	{
      		free(widths);
      		free(CIDToGIDMap);
      		delete cstring;
      		delete cffont;
			return false;
      	}
      	
      	widths[gid] = gm.wx;
      	if (gm.wx >= 0.0 && gm.wx <= 1000.0) 
			w_stat[((int) gm.wx)] += 1;
      	gid++;
    }
    
    cffont->setCString(cstring);
    
    int max_count = 0; 
    int dw = -1;
    for (int i = 0; i <= 1000; i++) 
    {
      	if (w_stat[i] > max_count) 
      	{
			dw        = i;
			max_count = w_stat[i];
      	}
    }
    
    if (dw >= 0)
    	addMetrics(cffont, CIDToGIDMap, widths, dw, last_cid);
    else
    	addMetrics(cffont, CIDToGIDMap, widths, defaultwidth, last_cid);
    	
    free(widths);
    cffont->setSubrs(0, 0);
    free(CIDToGIDMap);
    
    cffont->addString("Adobe", 1);
  	cffont->addString("Identity", 1);

  	cffont->topDictUpdate();
  	cffont->privDictUpdate(0);

  	cffont->updateString();
  	
  	cffont->topDictAdd("ROS", 3);
  	cffont->topDictSet("ROS", 0,(double)(cffont->getSID((char *)"Adobe")));
  	cffont->topDictSet("ROS", 1, (double)(cffont->getSID((char *)"Identity")));
  	cffont->topDictSet("ROS", 2, 0.0);

  	cffont->setNumGlyphs(num_glyphs);
  	offset = writeFontFile(cffont, streamdata, streamlen);

  	delete cffont;
  	
  	if (usedchars)
  		*usedchars = used_chars;
  		
  	if (lastcid)
  		*lastcid = last_cid;
  	
  	return true;
}

bool  XWCIDFont::loadType0T1C(uchar ** streamdata, 
	                          long * streamlen,
	                          char ** usedchars,
	                          int  * lastcid)
{
	int parent_id = 0;
	if ((parent_id = getParentID(0)) < 0 && (parent_id = getParentID(1)) < 0)
    	return false;
    	
    XWFontCache cache(true);
	char * used_chars = cache.getType0Font(parent_id)->getUsedChars();
	if (!used_chars)
		return false;
		
	XWFontFileSFNT * sfont = 0;
	if (ft_face)
		sfont = XWFontFileSFNT::load(ft_face, SFNT_TYPE_POSTSCRIPT, false);
	else
	 	sfont = XWFontFileSFNT::loadType1C(ident, false);
	if (!sfont)
		return false;
		
	XWFontFileCFF * cffont = sfont->makeType1C(options->index);
	if (!cffont)
	{
		delete sfont;
		return false;
	}
	
	cffont->readPrivate();
  	cffont->readSubrs();
  	
  	if (cffont->privDictKnow(0, "StdVW")) 
  		desc.StemV = cffont->privDictGet(0, "StdVW", 0);
  		
  	double default_width = CFF_DEFAULTWIDTHX_DEFAULT;
  	if (cffont->privDictKnow(0, "defaultWidthX"))
  		default_width = (cffont->privDictGet(0, "defaultWidthX", 0));
  		
  	double nominal_width = CFF_NOMINALWIDTHX_DEFAULT;
  	if (cffont->privDictKnow(0, "nominalWidthX"))
  		nominal_width = (cffont->privDictGet(0, "nominalWidthX", 0));
  		
  	ushort num_glyphs = 0; 
  	long last_cid = 0;
  	add_to_used_chars2(used_chars, 0);
  	for (int i = 0; i < (cffont->getNumGlyphs() + 7)/8; i++)
  	{
  		int c = used_chars[i];
    	for (int j = 7; j >= 0; j--)
    	{
    		if (c & (1 << j)) 
    		{
				num_glyphs++;
				last_cid = (i + 1) * 8 - j - 1;
      		}
    	}
  	}
  	
  	XWCFFFDSelect * fdselect = new XWCFFFDSelect;
  	fdselect->format = 3;
    fdselect->num_entries = 1;
    fdselect->data.ranges = (CFFRange3*)malloc(sizeof(CFFRange3));
    fdselect->data.ranges[0].first = 0;
    fdselect->data.ranges[0].fd    = 0;
    cffont->setFDSelect(fdselect);
    
    XWCFFCharsets * charset = new XWCFFCharsets;
    charset->format = 0;
    charset->num_entries = num_glyphs-1;
    charset->data.glyphs = (ushort*)malloc((num_glyphs-1) * sizeof(ushort));
    ushort gid = 0;
    for (long cid = 0; cid <= last_cid; cid++) 
    {
      	if (is_used_char2(used_chars, cid)) 
      	{
			if (gid > 0)
	  			charset->data.glyphs[gid-1] = cid;
			gid++;
      	}
    }
    
    cffont->setCharsets(charset);
    cffont->topDictAdd("CIDCount", 1);
  	cffont->topDictSet("CIDCount", 0, last_cid + 1);
  	
  	cffont->createFDArray(1);
  	cffont->fdarrayAdd(0, "FontName", 1);
  	cffont->fdarraySet(0, "FontName", 0, (double)(cffont->addString(fontname + 7, 1)));
  	
  	cffont->fdarrayAdd(0, "Private", 2);
  	cffont->fdarraySet(0, "Private", 0, 0.0);
  	cffont->fdarraySet(0, "Private", 0, 0.0);
  	
  	cffont->topDictAdd("FDArray", 1);
  	cffont->topDictSet("FDArray", 0, 0.0);
  	
  	cffont->topDictAdd("FDSelect", 1);
  	cffont->topDictSet("FDSelect", 0, 0.0);

  	cffont->topDictRemove("UniqueID");
  	cffont->topDictRemove("XUID");
  	cffont->topDictRemove("Private");
  	cffont->topDictRemove("Encoding");
  	
  	long offset = (long)(cffont->topDictGet("CharStrings", 0));
  	cffont->seek(offset);
  	XWCFFIndex * idx = cffont->getIndexHeader();
  	offset = cffont->tellPosition();
  	
  	if (idx->count < 2)
  	{
  		delete idx;
  		delete cffont;
  		delete sfont;
		return false;
  	}
  	
  	XWCFFIndex * charstrings = new XWCFFIndex(num_glyphs+1);
  	long max_len = 2 * CS_STR_LEN_MAX;
  	charstrings->data = (uchar*)malloc(max_len * sizeof(uchar));
  	long charstring_len = 0;
  	
  	gid  = 0;
  	uchar * data = (uchar*)malloc(CS_STR_LEN_MAX * sizeof(uchar));
  	long size = 0;
  	XWCSGInfo gm;
  	for (long cid = 0; cid <= last_cid; cid++)
  	{
  		if (!is_used_char2(used_chars, cid))
      		continue;
      		
      	if ((size = (idx->offset)[cid+1] - (idx->offset)[cid]) > CS_STR_LEN_MAX)
      	{
      		free(data);
      		delete charstrings;
      		delete idx;
      		delete cffont;
  			delete sfont;
			return false;
      	}
      	
      	if (charstring_len + CS_STR_LEN_MAX >= max_len) 
      	{
      		max_len = charstring_len + 2 * CS_STR_LEN_MAX;
      		charstrings->data = (uchar*)realloc(charstrings->data, max_len * sizeof(uchar));
    	}
    	
    	(charstrings->offset)[gid] = charstring_len + 1;
    	cffont->seekAbsolute(offset + (idx->offset)[cid] - 1);
    	cffont->read((char*)data, size);
    	charstring_len += gm.copyCharString(charstrings->data + charstring_len,
					                        max_len - charstring_len,
					                        data, size,
					                        cffont->getGsubr(), cffont->getSubrs(0),
					                        default_width, nominal_width);
        gid++;
  	}
  
  	free(data);	
  	delete idx;
  	if (gid != num_glyphs)
  	{
  		delete charstrings;
    	delete cffont;
  		delete sfont;
		return false;
    }

  	(charstrings->offset)[num_glyphs] = charstring_len + 1;
  	charstrings->count = num_glyphs;
  	cffont->setNumGlyphs(num_glyphs);
  	cffont->setCString(charstrings);
  	
  	cffont->setGsub(new XWCFFIndex((ushort)0));
  	cffont->setSubrs(0, 0);
  	cffont->privDictRemove(0, "Subrs");
  	
  	cffont->addString("Adobe", 1);
  	cffont->addString("Identity", 1);

  	cffont->topDictUpdate();
  	cffont->privDictUpdate(0);
  	cffont->updateString();
  	
  	cffont->topDictAdd("ROS", 3);
  	cffont->topDictSet("ROS", 0, (double)(cffont->getSID((char *)"Adobe")));
  	cffont->topDictSet("ROS", 1, (double)(cffont->getSID((char *)"Identity")));
  	cffont->topDictSet("ROS", 2, 0.0);

  	writeFontFile(cffont, streamdata, streamlen);

  	delete cffont;
  	
  	{
    	uchar * CIDToGIDMap = (uchar*)malloc(2 * (last_cid+1) * sizeof(uchar));
    	memset(CIDToGIDMap, 0, 2 * (last_cid + 1));
    	for (long cid = 0; cid <= last_cid; cid++) 
    	{
      		if (is_used_char2(used_chars, cid)) 
      		{
				CIDToGIDMap[2*cid  ] = (cid >> 8) & 0xff;
				CIDToGIDMap[2*cid+1] = cid & 0xff;
      		}
    	}
    	addCIDMetrics(sfont, CIDToGIDMap, last_cid, ((getParentID(1) < 0) ? 0 : 1));
    	free(CIDToGIDMap);
  	}
  	
  	delete sfont;
  	
  	if (usedchars)
  		*usedchars = used_chars;
  		
  	if (lastcid)
  		*lastcid = last_cid;
  	
  	return true;
}

bool XWCIDFont::loadType2(uchar ** streamdata, 
	                      long * streamlen,
	                      char ** usedchars,
	                      int  * lastcid,
	                      uchar** cidtogidmapA)
{
	if (isBaseFont())
		return true;
		
	if (!getEmbedding() && (opt_flags & CIDFONT_FORCE_FIXEDPITCH)) 
	{
		dict.DW = 1000;
    	return true;
  	}
  	
  	XWFontFileSFNT * sfont = 0;
  	if (ft_face)
  		sfont = XWFontFileSFNT::load(ft_face, SFNT_TYPE_POSTSCRIPT, false);
  	else
  		sfont = XWFontFileSFNT::loadCIDType2(ident, options->index, false);
	if (!sfont)
		return false;
		
	int glyph_ordering = 0;
	if (!strcmp(csi->registry, "Adobe") && !strcmp(csi->ordering, "Identity")) 
    	glyph_ordering = 1;
    	
    XWCMap     *cmap = 0;
  	XWSFNTCmap *ttcmap = 0;
  	int unicode_cmap = 0;
  	if (!glyph_ordering)
  	{
  		int i = 0;
  		for (; i <= KNOWN_ENCODINGS_MAX; i++)
  		{
  			ttcmap = sfont->readCmap(known_encodings[i].platform, known_encodings[i].encoding);
      		if (ttcmap)
				break;
  		}
  		
  		if (!ttcmap)
		{
			delete sfont;
			return false;
		}		
		else if (i <= WIN_UCS_INDEX_MAX)
			unicode_cmap = 1;
		else
			unicode_cmap = 0;
			
		cmap = findToCodeCMap(csi->registry, csi->ordering, i);
  	}
  	
  	XWTTGlyphs glyphs;
  	ushort last_cid   = 0;
  	ushort num_glyphs = 1;
  	char * used_chars = 0;
  	char * h_used_chars = 0;
  	char * v_used_chars = 0;
  	
  	XWFontCache cache(true);
  	{
  		int parent_id = -1;
  		XWType0Font * parent = 0;
  		if ((parent_id = getParentID(0)) >= 0) 
  		{
      		parent = cache.getType0Font(parent_id);
      		h_used_chars = parent->getUsedChars();
    	}
    	
    	if ((parent_id = getParentID(1)) >= 0) 
    	{
      		parent = cache.getType0Font(parent_id);
      		v_used_chars = parent->getUsedChars();
    	}
    	
    	if (!h_used_chars && !v_used_chars)
    	{
    		if (ttcmap)
    			delete ttcmap;
      		delete sfont;
			return false;
      	}
      	
      	int c = 0;
    	for (int i = 8191; i >= 0; i--) 
    	{
      		if (h_used_chars && h_used_chars[i] != 0) 
      		{
				last_cid = i * 8 + 7;
				c = h_used_chars[i];
				break;
      		}
      		
      		if (v_used_chars && v_used_chars[i] != 0) 
      		{
				last_cid = i * 8 + 7;
				c = v_used_chars[i];
				break;
      		}
    	}
    	
    	if (last_cid > 0) 
    	{
      		for (int i = 0; i < 8; i++) 
      		{
				if ((c >> i) & 1)
	  			break;
				last_cid--;
      		}
    	}
    	
    	if (last_cid >= 0xFFFFu) 
    	{
    		if (ttcmap)
    			delete ttcmap;
      		delete sfont;
			return false;
    	}
  	}
  	
  	uchar * cidtogidmap = 0;//(uchar*)malloc((last_cid + 1) * 2 * sizeof(uchar));
  	//memset(cidtogidmap, 0, (last_cid + 1) * 2);
  	if (h_used_chars)
  	{
  		used_chars = h_used_chars;
  		long code = 0;
      	ushort gid = 0;
    	for (ushort cid = 1; cid <= last_cid; cid++)
    	{
    		if (!is_used_char2(h_used_chars, cid))
				continue;
				
			if (glyph_ordering) 
			{
				gid  = cid;
				code = cid;
      		}
      		else
      		{
    			code = cidToCode(cmap, cid);
				gid  = ttcmap->lookup(code);
		
				if (gid == 0 && unicode_cmap) 
				{
	  				long alt_code = fixCJKSymbols(code);
	  				if (alt_code != code) 
	    				gid = ttcmap->lookup(alt_code);
				}
			}
			
			gid = glyphs.addGlyph(gid, cid);
			//gid = glyphs.addGlyph(gid, num_glyphs);
      //cidtogidmap[2*cid  ] = gid >> 8;
      //cidtogidmap[2*cid+1] = gid & 0xff;
		}
		
		num_glyphs++;
  	}
  	
  	if (v_used_chars)
  	{
  		XWOTLGsub * gsub_list = 0;
  		if (!glyph_ordering)
  		{
  			gsub_list = new XWOTLGsub;
      		if (gsub_list->addFeat("*", "*", "vrt2", sfont) < 0)
      		{
      			if (gsub_list->addFeat("*", "*", "vert", sfont) < 0)
      			{
      				delete gsub_list;
	  				gsub_list = 0;
      			}
      			else
      				gsub_list->select("*", "*", "vert");
      		}
  			else
  				gsub_list->select("*", "*", "vrt2");
  		}
  			
  		long   code = 0;
      	ushort gid = 0;
  		for (ushort cid = 1; cid <= last_cid; cid++)
  		{
  			if (!is_used_char2(v_used_chars, cid))
				continue;
				
			if (h_used_chars && is_used_char2(h_used_chars, cid)) 
				continue;
				
			if (glyph_ordering) 
			{
				gid  = cid;
				code = cid;
      		}
      		else
      		{
      			code = cidToCode(cmap, cid);
				gid  = ttcmap->lookup(code);
				if (gid == 0 && unicode_cmap) 
				{
	  				long alt_code = fixCJKSymbols(code);
	  				if (alt_code != code) 
	    				gid = ttcmap->lookup(alt_code);
				}
      		}
      		
      		if (gsub_list)
      			gsub_list->apply(&gid);
      			
      		gid = glyphs.addGlyph(gid, cid);
      		//gid = glyphs.addGlyph(gid, num_glyphs);
      		//cidtogidmap[2*cid  ] = gid >> 8;
      		//cidtogidmap[2*cid+1] = gid & 0xff;
      		
      		if (used_chars)
      			add_to_used_chars2(used_chars, cid);
      			
      		num_glyphs++;
  		}
  		
  		if (gsub_list)
      		delete gsub_list;

    	if (!used_chars)
      		used_chars = v_used_chars;
  	}
  	
  	if (!used_chars)
  	{
  		//free(cidtogidmap);
  		if (ttcmap)
  			delete ttcmap;
  		delete sfont;
		return false;
  	}
  	
  	if (ttcmap)
  		delete ttcmap;
  	
  	if (getEmbedding())
  	{
  		if (glyphs.buildTables(sfont) < 0)
  		{
  			//free(cidtogidmap);
  			delete sfont;
			return false;
  		}
  	}
  	else
  	{
  		if (glyphs.getMetrics(sfont) < 0)
  		{
  			//free(cidtogidmap);
  			delete sfont;
			return false;
  		}
  	}
  	
  	if (opt_flags & CIDFONT_FORCE_FIXEDPITCH)
  		dict.DW = 1000;
  	else
  	{
  		addTTCIDHMetrics(&glyphs, used_chars, cidtogidmap, last_cid);
    	if (v_used_chars)
      		addTTCIDVMetrics(&glyphs, used_chars, /*cidtogidmap,*/ last_cid);
  	}
  	
  	if (!getEmbedding()) 
  	{
      	//free(cidtogidmap);
    	delete sfont;
		return true;
  	}
  	
  	for (int i = 0; required_table[i].name; i++) 
  	{
    	if (sfont->requireTable(required_table[i].name, required_table[i].must_exist) < 0) 
    	{
      		//free(cidtogidmap);
    		delete sfont;
			return false;
    	}
  	}
  	
  	if (!sfont->createFontFileStream(streamdata, streamlen))
  	{
  		//free(cidtogidmap);
    	delete sfont;
		return true;
  	}
  	
  	delete sfont;
  	
  	if (usedchars)
  		*usedchars = used_chars;
  		
  	if (lastcid)
  		*lastcid = last_cid;
  		
  	if (cidtogidmapA)
  		*cidtogidmapA = cidtogidmap;
  	//else
  		//free(cidtogidmap);
  		
  	return true;
}

int XWCIDFont::openBase(const char *nameA,
		                CIDSysInfo *cmap_csi, 
		                XWCIDOpt *opt)
{
	int idx = 0;
	for (; cid_basefont[idx].fontname != NULL; idx++) 
	{
    	if (!strcmp(nameA, cid_basefont[idx].fontname) ||
	        (strlen(nameA) == strlen(cid_basefont[idx].fontname) - strlen("-Acro") &&
         	!strncmp(nameA, cid_basefont[idx].fontname, strlen(cid_basefont[idx].fontname)-strlen("-Acro"))))
      		break;
  	}
  	
  	if (cid_basefont[idx].fontname == NULL)
    	return -1;

  	fontname = new char[strlen(nameA)+12];
  	memset(fontname, 0, strlen(nameA)+12);
  	strcpy(fontname, nameA);
  	flags   |= FONT_FLAG_BASEFONT;
  	
  	switch (opt->style) 
  	{
  		case FONT_STYLE_BOLD:
    		strcat(fontname, ",Bold");
    		break;
    		
  		case FONT_STYLE_ITALIC:
    		strcat(fontname, ",Italic");
    		break;
    		
  		case FONT_STYLE_BOLDITALIC:
    		strcat(fontname, ",BoldItalic");
    		break;
    		
  	}
  	
  	csi = new CIDSysInfo;
  	csi->registry = 0;
  	csi->ordering = 0;
  	csi->supplement = 0;
  	
  	const char *start = cid_basefont[idx].fontdict;
    const char *end = start + strlen(start);    
  	if (parseDict(start, end, cmap_csi) < 0)
  		return -1;
  		
  	start = cid_basefont[idx].descriptor;
  	end = start + strlen(start);  
  	if (parseDescriptor(start, end) < 0)
  		return -1;
  	
  	opt->embed = 0;
  	
  	return 0;
}

int XWCIDFont::openType0(const char *nameA,
		                 CIDSysInfo *cmap_csi, 
		                 XWCIDOpt *opt)
{
	XWFontFileSFNT * sfont = 0;
	if (ft_face)
		sfont = XWFontFileSFNT::load(ft_face, SFNT_TYPE_POSTSCRIPT, false);
	else
	 	sfont = XWFontFileSFNT::loadCIDType0((char*)nameA, false);
	if (!sfont)
		return -1;
		
	XWFontFileCFF * cffont = sfont->makeCIDType0(opt->index);
	if (!cffont)
	{
		delete sfont;
		return -1;
	}
		
	csi = new CIDSysInfo;
	csi->registry = cffont->getRegistry();
	csi->ordering = cffont->getOrdering();
	csi->supplement = cffont->getSupplement();
	if (cmap_csi)
	{
		if (strcmp(csi->registry, cmap_csi->registry) != 0 ||
			strcmp(csi->ordering, cmap_csi->ordering) != 0)
			return -1;
	}
	
	{
		char * shortname = cffont->getName();
		if (!shortname)
			return -1;
			
		fontname = new char[strlen(shortname) + 19];
    	memset(fontname, 0, strlen(shortname)+19);
    	strcpy(fontname, shortname);
    	delete [] shortname;
	}
	
	delete cffont;
	
	if (opt->embed && opt->style != FONT_STYLE_NONE) 
    	opt->embed = 0;
    	
    switch (opt->style) 
    {
  		case FONT_STYLE_BOLD:
    		strcat(fontname, ",Bold");
    		break;
    		
  		case FONT_STYLE_ITALIC:
    		strcat(fontname, ",Italic");
    		break;
    		
  		case FONT_STYLE_BOLDITALIC:
    		strcat(fontname, ",BoldItalic");
    		break;
  	}
  	
  	if (!dict.Type)
		dict.Type = qstrdup("Font");
		
	if (!dict.Subtype)
		dict.Subtype = qstrdup("CIDFontType0");
		
  	subtype  = CIDFONT_TYPE0;
  	
  	if (!sfont->getTTFontDesc(&(opt->embed), opt->stemv, 0, 
		                      &(desc.Ascent), &(desc.Descent),
		                      &(desc.StemV), &(desc.CapHeight),
		                      &(desc.XHeight), &(desc.AvgWidth),
		                      desc.FontBBox, &(desc.ItalicAngle),
		                      &(desc.Flags), desc.panose))
	{
		delete sfont;
		return -1;
	}
	
  	char * fontnameA = new char[strlen(fontname) + 100];
	if (opt->embed) 
	{
    	pdf_font_make_uniqueTag(fontnameA);
    	fontnameA[6] = '+';
    	strcpy(fontnameA + 7, fontname);
  	}
  	else
  		strcpy(fontnameA, fontname);
  	
  	if (!dict.BaseFont)
  		dict.BaseFont = qstrdup(fontnameA);
  		
  	if (!desc.FontName)
  		desc.FontName = qstrdup(fontnameA);
  		
  	delete [] fontnameA;
  	
  	dict.DW = 1000;
  	delete sfont;
  	return 0;
}

int XWCIDFont::openType0T1(const char *nameA,
		                   CIDSysInfo *cmap_csi, 
		                   XWCIDOpt *opt)
{
	if (cmap_csi &&
      	(strcmp(cmap_csi->registry, "Adobe")    != 0 ||
       	strcmp(cmap_csi->ordering, "Identity") != 0)) 
    {
    	return -1;
  	}
  
	XWFontFileCFF * cffont = XWFontFileCFF::loadType1((char*)nameA, 0, 1);
	if (!cffont)
		return -1;
		
	char * shortname = cffont->getName();
	fontname = new char[strlen(shortname) + 8];
  	memset(fontname, 0, strlen(shortname) + 8);
  	strcpy(fontname, shortname);
  	delete [] shortname;
	ft_to_gid = cffont->getFtToGid();
  	delete cffont;
  	
  	if (opt->style != FONT_STYLE_NONE) 
    	opt->style = FONT_STYLE_NONE;
    	
    subtype  = CIDFONT_TYPE0;
  	csi      = new CIDSysInfo;
  	csi->registry = qstrdup("Adobe");
  	csi->ordering = qstrdup("Identity");
  	csi->supplement = 0;
  	flags   |= CIDFONT_FLAG_TYPE1;
  	
  	if (!dict.Type)
		dict.Type = qstrdup("Font");
		
	if (!dict.Subtype)
		dict.Subtype = qstrdup("CIDFontType0");
		
	char * fontnameA = new char[strlen(fontname) + 100];
    pdf_font_make_uniqueTag(fontnameA);
    fontnameA[6] = '+';
    strcpy(fontnameA + 7, fontname);
  	
  	if (!dict.BaseFont)
  		dict.BaseFont = qstrdup(fontnameA);
  		
  	if (!desc.FontName)
  		desc.FontName = qstrdup(fontnameA);
  		
  	delete [] fontnameA;
  	
  	return 0;
}

int XWCIDFont::openType0T1C(const char *nameA,
		                    CIDSysInfo *cmap_csi, 
		                    XWCIDOpt *opt)
{
	XWFontFileSFNT * sfont = 0;
	if (ft_face)
		sfont = XWFontFileSFNT::load((char*)ft_face, SFNT_TYPE_POSTSCRIPT, false);
	else
	  sfont = XWFontFileSFNT::loadType1C((char*)nameA, false);
	if (!sfont)
		return -1;
		
	XWFontFileCFF * cffont = sfont->makeType1C(opt->index);
	if (!cffont)
	{
		delete sfont;
		return -1;
	}
		
	csi = new CIDSysInfo;
  	csi->registry   = qstrdup("Adobe");
  	csi->ordering   = qstrdup("Identity");
  	csi->supplement = 0;
  	if (cmap_csi)
  	{
  		if (strcmp(csi->registry, cmap_csi->registry) != 0 ||
			strcmp(csi->ordering, cmap_csi->ordering) != 0)
			return -1;
  	}
  	
  	{
  		char * shortname = cffont->getName();
    	if (!shortname)
      		return -1;

    	fontname = new char[strlen(shortname) + 8];
    	memset(fontname, 0, strlen(shortname) + 8);
    	strcpy(fontname, shortname);
    	delete [] shortname;
  	}
  	
  	delete cffont;
  	opt->embed = 1;
  	subtype  = CIDFONT_TYPE0;
  	flags   |= CIDFONT_FLAG_TYPE1C;
  	
  	if (!dict.Type)
		dict.Type = qstrdup("Font");
		
	if (!dict.Subtype)
		dict.Subtype = qstrdup("CIDFontType0");
  	
  	char * fontnameA = new char[strlen(fontname) + 100];
  	if (opt->embed) 
	{
    	pdf_font_make_uniqueTag(fontnameA);
    	fontnameA[6] = '+';
    	strcpy(fontnameA + 7, fontname);
  	}
  	else
  		strcpy(fontnameA, fontname);
  	
  	if (!dict.BaseFont)
  		dict.BaseFont = qstrdup(fontnameA);
  		
  	if (!desc.FontName)
  		desc.FontName = qstrdup(fontnameA);
  		
  	delete [] fontnameA;
  	
  	return 0;
}

int XWCIDFont::openType2(const char *nameA,
		                 CIDSysInfo *cmap_csi, 
		                 XWCIDOpt *opt)
{
	XWFontFileSFNT * sfont = 0;
	if (ft_face)
		sfont = XWFontFileSFNT::load(ft_face, SFNT_TYPE_POSTSCRIPT, false);
	else
		sfont = XWFontFileSFNT::loadCIDType2((char*)nameA, opt->index, false);
	if (!sfont)
		return -1;
		
	{
		char * shortname = new char[127];
    	long namelen   = sfont->getTTPSFontName(shortname, 127);
    	if (namelen == 0) 
    	{
      		memset(shortname, 0, 127);
      		strncpy(shortname, nameA, 127);
      		namelen = strlen(shortname);
    	}
    	validate_name(shortname, namelen);
    	
    	fontname = new char[strlen(shortname)+19];
    	strcpy(fontname, shortname);
    	delete [] shortname;
	}
	
	if (opt->embed && opt->style != FONT_STYLE_NONE) 
    	opt->embed = 0;
    	
  	switch (opt->style) 
  	{
  		case FONT_STYLE_BOLD:
    		strcat(fontname, ",Bold");
    		break;
    		
  		case FONT_STYLE_ITALIC:
    		strcat(fontname, ",Italic");
    		break;
    		
  		case FONT_STYLE_BOLDITALIC:
    		strcat(fontname, ",BoldItalic");
    		break;
  	}
  	
  	subtype  = CIDFONT_TYPE2;  	
  	csi = new CIDSysInfo;
  	if (opt->csi)
  	{
  		if (cmap_csi)
  		{
  			if (strcmp(opt->csi->registry, cmap_csi->registry) ||
	  			strcmp(opt->csi->ordering, cmap_csi->ordering))
	  			return -1;
	  			
	  		if (opt->csi->supplement < cmap_csi->supplement)
	  			opt->csi->supplement = cmap_csi->supplement;
  		}
  		
  		csi->registry = qstrdup(opt->csi->registry);
  		csi->ordering = qstrdup(opt->csi->ordering);
  		csi->supplement = opt->csi->supplement;
  	}
  	else if (cmap_csi)
  	{
  		csi->registry = qstrdup(cmap_csi->registry);
  		csi->ordering = qstrdup(cmap_csi->ordering);
  		csi->supplement = cmap_csi->supplement;
  	}
  	else
  	{
  		csi->registry = qstrdup("Adobe");
  		csi->ordering = qstrdup("Identity");
  		csi->supplement = 0;
  	}
  	
  	if (!dict.Type)
		dict.Type = qstrdup("Font");
		
	if (!dict.Subtype)
		dict.Subtype = qstrdup("CIDFontType2");
		
	if (!sfont->getTTFontDesc(&(opt->embed), opt->stemv, 0, 
		                      &(desc.Ascent), &(desc.Descent),
		                      &(desc.StemV), &(desc.CapHeight),
		                      &(desc.XHeight), &(desc.AvgWidth),
		                      desc.FontBBox, &(desc.ItalicAngle),
		                      &(desc.Flags), desc.panose))
	{
		delete sfont;
		return -1;
	}
	
	char * fontnameA = new char[strlen(fontname) + 100];
	if (opt->embed) 
	{
    	pdf_font_make_uniqueTag(fontnameA);
    	fontnameA[6] = '+';
    	strcpy(fontnameA + 7, fontname);
  	}
  	else
  		strcpy(fontnameA, fontname);
  	
  	if (!dict.BaseFont)
  		dict.BaseFont = qstrdup(fontnameA);
  		
  	if (!desc.FontName)
  		desc.FontName = qstrdup(fontnameA);
  		
  	delete [] fontnameA;
  	delete sfont;
  	
  	return 0;
}

int XWCIDFont::parseDescriptor(const char *start, const char *end)
{
	XWLexer lexer(start, end);
  const char * p = lexer.skipWhite();
  p = lexer.skip(2);
  p = lexer.skipWhite();
  char  buf[256];
  int   lenA = 0;
  bool ok = true;
  while (ok && p < end && p[0] != '>')
  {
  	p = lexer.getName(buf, 256, &ok);
  	p = lexer.skipWhite();
  	if (!strcmp(buf, "FontName"))
  	{
  		p = lexer.getName(buf, 256, &ok);
  		if (ok)
  			desc.FontName = qstrdup(buf);
  	}
  	else if (!strcmp(buf, "Flags"))
  	{
  		double v = 0.0;
  		p = lexer.getNumber(&v, &ok);
  		desc.Flags = (int)v;
  	}
  	else if (!strcmp(buf, "FontBBox"))
  	{
  		p = lexer.skipWhite();
  		p = lexer.skip(1);
  		double v = 0.0;
  		int i = 0;
  		while (ok && p < end && p[0] != ']' && i < 4)
  		{
  			p = lexer.skipWhite();
  			p = lexer.getNumber(&v, &ok);
  			desc.FontBBox[i++] = v;
  		}
  		p = lexer.skip(1);
  	}
  	else if (!strcmp(buf, "ItalicAngle"))
  		p = lexer.getNumber(&(desc.ItalicAngle), &ok);
  	else if (!strcmp(buf, "Ascent"))
  		p = lexer.getNumber(&(desc.Ascent), &ok);
  	else if (!strcmp(buf, "Descent"))
  		p = lexer.getNumber(&(desc.Descent), &ok);
  	else if (!strcmp(buf, "CapHeight"))
  		p = lexer.getNumber(&(desc.CapHeight), &ok);
  	else if (!strcmp(buf, "XHeight"))
  		p = lexer.getNumber(&(desc.XHeight), &ok);
  	else if (!strcmp(buf, "StemV"))
  		p = lexer.getNumber(&(desc.StemV), &ok);
  	else if (!strcmp(buf, "AvgWidth"))
  		p = lexer.getNumber(&(desc.AvgWidth), &ok);
  	else if (!strcmp(buf, "Style"))
  	{
  		p = lexer.skipWhite();
  		p = lexer.skip(2);
  		p = lexer.skipWhite();
  		p = lexer.getName(buf, 256, &ok);
  		p = lexer.skipWhite();
  		p = lexer.skip(1);
  		p = lexer.skipWhite();
  		int i = 0;
  		while (p < end && p[0] != '>' && i < 12)
  		{
  			char c = *p;
  			c = c- '0';
  			p = lexer.skipWhite();
  			char t = c + (*p - '0');
  			i++;  			
  			p = lexer.skipWhite();
  			desc.panose[i] = t;
  		}
  		p = lexer.skipWhite();
  		p = lexer.skip(1);
  		p = lexer.skipWhite();
  		p = lexer.skip(2);
  	}
  	else
  		p = lexer.skip(1);
  	
  	p = lexer.skipWhite();
  }
  
  return 0;
}

int XWCIDFont::parseDict(const char *start, const char *end, CIDSysInfo *cmap_csi)
{
	XWLexer lexer(start, end);
  const char * p = lexer.skipWhite();
  p = lexer.skip(2);
  p = lexer.skipWhite();
  char  buf[256];
  int   lenA = 0;
  bool ok = true;
  while (ok && p < end && p[0] != '>')
  {
  	p = lexer.getName(buf, 256, &ok);
  	p = lexer.skipWhite();
  	if (!strcmp(buf, "Subtype"))
  	{
  		p = lexer.getName(buf, 256, &ok);
  		if (!strcmp(buf, "CIDFontType0"))
  			subtype = CIDFONT_TYPE0;
  		else if (!strcmp(buf, "CIDFontType2"))
  			subtype = CIDFONT_TYPE2;
  	}
  	else if (!strcmp(buf, "CIDSystemInfo"))
  	{
  		p = lexer.skip(2);
  		while (ok && p < end && p[0] != '>')
  		{
  			p = lexer.getName(buf, 256, &ok);
  			p = lexer.skipWhite();
  			if (!strcmp(buf, "Registry"))
  			{
  				lenA = 0;
  				p = lexer.getString(buf, 256, &lenA, &ok);
  				buf[lenA] = '\0';
  				if (lenA > 0)
  					csi->registry = qstrdup(buf);
  			}
  			else if (!strcmp(buf, "Ordering"))
  			{
  				lenA = 0;
  				p = lexer.getString(buf, 256, &lenA, &ok);
  				buf[lenA] = '\0';
  				if (lenA > 0)
  					csi->ordering = qstrdup(buf);
  			}
  			else if (!strcmp(buf, "Supplement"))
  			{
  				double v = 0.0;
  				p = lexer.getNumber(&v, &ok);
  				csi->supplement = (int)(v);
  			}
  				
  			p = lexer.skipWhite();
   		}
  			
  		p = lexer.skip(2);
  		if (cmap_csi)
  		{
  			if (strcmp(csi->registry, cmap_csi->registry) || 
  					strcmp(csi->ordering, cmap_csi->ordering))
  			{
  				return -1;
  			}
  		}
  	}
  	else if (!strcmp(buf, "DW"))
  	{
  		double v = 0.0;
  		p = lexer.getNumber(&v, &ok);
  		dict.DW = (int)(v);
  	}
  	else if (!strcmp(buf, "W"))
  	{
  		if (!dict.W)
	  		dict.W = new XWCIDWidths;
  				
  		p = lexer.skipWhite();
  		p = lexer.skip(1);
  		double v = 0.0;
  		while (ok && p < end && p[0] != ']')
  		{
  			p = lexer.skipWhite();
  			if (p[0] == '[')
  			{
  				XWCIDWidths * W = new XWCIDWidths;
  				p = lexer.skipWhite();
  				dict.W->add(W);
  				while (ok && p < end && p[0] != ']')
  				{
  					p = lexer.getNumber(&v, &ok);
  					W->add(v);
  					p = lexer.skipWhite();
  				}
  				p = lexer.skip(1);
  				p = lexer.skipWhite();
  			}
  			else
  			{
  				p = lexer.getNumber(&v, &ok);
  				dict.W->add(v);
  				p = lexer.skipWhite();
  			}
  		}
  			
  		p = lexer.skip(1);
  	}
  		
  	p = lexer.skipWhite();
  }
  	
  if (opt_flags & CIDFONT_FORCE_FIXEDPITCH)
  {
  	if (dict.W)
  		delete dict.W;
  	dict.W = 0;
  		
  	if (dict.W2)
  		delete dict.W2;
  			
  	dict.W2 = 0;
  }
  	
  return 0;
}

long XWCIDFont::writeFontFile(XWFontFileCFF *cffont,
	                          uchar ** streamdata, 
	                          long * streamlen)
{
	XWCFFIndex * topdict = new XWCFFIndex((ushort)1);
  	XWCFFIndex * fdarray = new XWCFFIndex((ushort)(cffont->getNumFDs()));
  	XWCFFIndex * priv    = new XWCFFIndex((ushort)(cffont->getNumFDs()));
  	cffont->topDictRemove("UniqueID");
  	cffont->topDictRemove("XUID");
  	cffont->topDictRemove("Private");
  	cffont->topDictRemove("Encoding");
  	
  	char buf[1024];
  	topdict->offset[1] = cffont->topDictPack((uchar*)buf, 1024) + 1;
  	for (long i = 0; i < cffont->getNumFDs(); i++)
  	{
  		long size = cffont->privDictPack(i, (uchar*)buf, 1024);
  		if (size < 1)
  			cffont->fdarrayRemove(i, "Private");
  		(priv->offset)[i+1] = (priv->offset)[i] + size;
    	(fdarray->offset)[i+1] = (fdarray->offset)[i] +cffont->fdarrayPack(i, (uchar*)buf, 1024);
  	}
  	
  	long destlen = 4; /* header size */
  	destlen += cffont->setName(fontname);
  	destlen += topdict->size();
  	destlen += cffont->cstringSize();
  	destlen += cffont->gsubrSize();
  	XWCFFCharsets * charsets = cffont->getCharsets();
  	destlen += (charsets->num_entries) * 2 + 1;
  	XWCFFFDSelect * fdselect = cffont->getFDSelect();
  	destlen += (fdselect->num_entries) * 3 + 5;
  	destlen += cffont->cstringSize();
  	destlen += fdarray->size();
  	destlen += priv->offset[priv->count] - 1;
  	
  	uchar * dest = (uchar*)malloc(destlen * sizeof(uchar));
  	long offset = 0;
  	
  	offset += cffont->putHeader(dest + offset, destlen - offset);
  	offset += cffont->packName(dest + offset, destlen - offset);
  	long topdict_offset = offset;
  	offset += topdict->size();
  	offset += cffont->packString(dest + offset, destlen - offset);
  	offset += cffont->packGsubr(dest + offset, destlen - offset);
  	cffont->topDictSet("charset", 0, offset);
  	offset += cffont->packCharsets(dest + offset, destlen - offset);
  	cffont->topDictSet("FDSelect", 0, offset);
  	offset += cffont->packFDSelect(dest + offset, destlen - offset);
  	cffont->topDictSet("CharStrings", 0, offset);
  	offset += cffont->packCString(dest + offset, cffont->getCStrings()->size());
  	cffont->setCString(0);
  	
  	cffont->topDictSet("FDArray", 0, offset);
  	long fdarray_offset = offset;
  	offset += fdarray->size();
  	
  	fdarray->data = (uchar*)malloc((fdarray->offset[fdarray->count] - 1) * sizeof(uchar));
  	
  	for (long i = 0; i < cffont->getNumFDs(); i++) 
  	{
    	long size = priv->offset[i+1] - priv->offset[i];
    	cffont->privDictPack(i, dest + offset, size);
    	cffont->fdarraySet(i, "Private", 0, size);
    	cffont->fdarraySet(i, "Private", 1, offset);
    	cffont->fdarrayPack(i, fdarray->data + (fdarray->offset)[i] - 1, fdarray->offset[fdarray->count] - 1);
    	offset += size;
  	}

  	fdarray->pack(dest + fdarray_offset, fdarray->size());
  	delete fdarray;
  	delete priv;
  	
  	topdict->data = (uchar*)malloc((topdict->offset[topdict->count] - 1) * sizeof(uchar));
  	cffont->topDictPack(topdict->data, topdict->offset[topdict->count] - 1);
  	topdict->pack(dest + topdict_offset, topdict->size());
  	delete topdict;
  	
  	if (streamdata)
  		*streamdata = dest;
  	else
  		free(dest);
  		
  	if (streamlen)
  		*streamlen = offset;
  		
  	return destlen;
}

#define CACHE_ALLOC_SIZE  16u

int XWCIDFontCache::flags = 0;

XWCIDFontCache::XWCIDFontCache()
{
	max  = CACHE_ALLOC_SIZE;
	fonts = (XWCIDFont**)malloc(max * sizeof(XWCIDFont*));
	num  = 0;
}

XWCIDFontCache::~XWCIDFontCache()
{
	for (int i = 0; i < num; i++)
	{
		if (fonts[i])
			delete fonts[i];
	}
	
	free(fonts);
}

int XWCIDFontCache::find(const char *map_name, 
	                     CIDSysInfo *cmap_csi, 
	                     XWTexFontMapOpt *fmap_opt,
	                     int pdf_ver)
{
	XWCIDOpt * opt  = new XWCIDOpt;
  	opt->style = fmap_opt->style;
  	opt->index = fmap_opt->index;
  	opt->embed = (fmap_opt->flags & FONTMAP_OPT_NOEMBED) ? 0 : 1;
  	opt->name  = NULL;
  	opt->csi   = getCIDSysInfo(fmap_opt, pdf_ver);
  	opt->stemv = fmap_opt->stemv;
  	
  	if (!opt->csi && cmap_csi)
  	{
  		opt->csi = new CIDSysInfo;
    	opt->csi->registry = qstrdup(cmap_csi->registry);
    	opt->csi->ordering   = qstrdup(cmap_csi->ordering);
    	opt->csi->supplement = cmap_csi->supplement;
  	}
  	
  	XWCIDFont * font = 0;
  	int font_id = 0;
  	for (; font_id < num; font_id++)
  	{
  		font = fonts[font_id];
  		if (!strcmp(font->name, map_name) &&
			 font->options->style == opt->style &&
			 font->options->index == opt->index)
		{
			if (font->options->embed == opt->embed)
			{
				if (!opt->csi) 
				{
	  				if (font->subtype == CIDFONT_TYPE2)
	    				continue;
	  				else
	    				break;
				} 
				else if (!strcmp(font->csi->registry, opt->csi->registry) && 
		   				 !strcmp(font->csi->ordering, opt->csi->ordering)) 
		   		{
	  				if (font->subtype == CIDFONT_TYPE2)
	    				font->csi->supplement = qMax(opt->csi->supplement, font->csi->supplement);
	  				break;
				}
			}
			else if (font->isBaseFont())
			{
				opt->embed = 0;
				break;
			}
		}
  	}
  	
  	if (font_id < num && cmap_csi)
  	{
  		if (strcmp(font->csi->registry, cmap_csi->registry) ||
			strcmp(font->csi->ordering, cmap_csi->ordering))
		{
			delete opt;
			return -1;
		}
  	}
  	
  	if (font_id == num)
  	{
  		font = new XWCIDFont;
  		font->ft_face = fmap_opt->ftface;
  		if (font->open(map_name, cmap_csi, opt) < 0)
			{
				delete opt;
				delete font;
				return -1;
			}
			else
			{
				if (num >= max) 
				{
					max  += CACHE_ALLOC_SIZE;
					fonts = (XWCIDFont**)realloc(fonts, max * sizeof(XWCIDFont*));
      	}
      
      	font->name    = qstrdup(map_name);
      	font->ident   = qstrdup(map_name);
      	font->options = opt;
      	fonts[font_id] = font;
      	num++;
			}
  	}
  	else
  		delete opt;
  		
  	return font_id;
}

XWCIDFont * XWCIDFontCache::get(int font_id)
{
	if (font_id < 0 || font_id >= num)
		return 0;
		
	return fonts[font_id];
}

int XWCIDFontCache::t1LoadUnicodeCMap(const char *font_name,
		     			              const char *otl_tags,
		     					      int wmode)
{
	if (otl_tags || !font_name)
		return -1;
		
	XWFontFileCFF * cffont = XWFontFileCFF::loadType1((char*)font_name, 0, 1);
	if (!cffont)
		return -1;
		
	int cmap_id = loadBaseCMap(font_name, wmode, cffont);
	delete cffont;
	return cmap_id;
}

void XWCIDFontCache::setFlags(int flagsA)
{
	opt_flags |= flagsA;
}

CIDSysInfo * XWCIDFontCache::getCIDSysInfo(XWTexFontMapOpt *fmap_opt,
	                                       int pdf_ver)
{
	if (!fmap_opt || !fmap_opt->charcoll)
    	return 0;
    	
    CIDSysInfo *csi = 0;
    for (int i = 0; CIDFont_stdcc_alias[i].name != NULL; i++)
    {
    	int n = strlen(CIDFont_stdcc_alias[i].name);
    	if (!strncmp(fmap_opt->charcoll, CIDFont_stdcc_alias[i].name, n))
    	{
    		csi = new CIDSysInfo;
    		int csi_idx  = CIDFont_stdcc_alias[i].index;
      		csi->registry = qstrdup(CIDFont_stdcc_def[csi_idx].registry);
      		csi->ordering = qstrdup(CIDFont_stdcc_def[csi_idx].ordering);
      		if (strlen(fmap_opt->charcoll) > (ulong)n) 
				csi->supplement = (int) strtoul(&(fmap_opt->charcoll[n]), NULL, 10);
      		else 
				csi->supplement = CIDFont_stdcc_def[csi_idx].supplement[pdf_ver];
      		break;
    	}
    }
    
    int csi_idx = -1;
    if (csi == 0)
    {
    	char * p = (char *) fmap_opt->charcoll;
    	csi = new CIDSysInfo;
    	csi->registry = 0;
    	csi->ordering = 0;
    	p = strchr(fmap_opt->charcoll, '-');
    	if (!p || p[1] == '\0')
    	{
    		delete csi;
    		return 0;
    	}
    	
    	p++;
    	char * q = strchr(p, '-');
    	if (!q || q[1] == '\0')
    	{
    		delete csi;
    		return 0;
    	}
    	
    	q++;
    	if (!isdigit(q[0]))
    	{
    		delete csi;
    		return 0;
    	}
    	
    	int n = strlen(fmap_opt->charcoll) - strlen(p) - 1;
    	csi->registry = new char[n+1];
    	memcpy(csi->registry, fmap_opt->charcoll, n);
    	csi->registry[n] = '\0';

    	int m = strlen(p) - strlen(q) - 1;
    	csi->ordering = new char[m+1];
    	memcpy(csi->ordering, p, m);
    	csi->ordering[m] = '\0';

    	csi->supplement = (int) strtoul(q, NULL, 10);
    	
    	for (int i = 0; CIDFont_stdcc_def[i].ordering != NULL; i++) 
    	{
      		if ((CIDFont_stdcc_def[i].registry &&
	   			!strcmp(csi->registry, CIDFont_stdcc_def[i].registry)) &&
	  			!strcmp(csi->ordering, CIDFont_stdcc_def[i].ordering)) 
	  		{
				csi_idx = i;
				break;
      		}
    	}
    }
    
    return csi;
}

int XWCIDFontCache::loadBaseCMap(const char *font_name, 
	                             int wmode, 
	                             XWFontFileCFF *cffont)
{
	unsigned char range_min[4] = {0x00, 0x00, 0x00, 0x00};
  	unsigned char range_max[4] = {0x7f, 0xff, 0xff, 0xff};
  	
  	char * cmap_name = new char[strlen(font_name)+strlen("-UCS4-H")+1];
  	if (wmode) 
    	sprintf(cmap_name, "%s-UCS4-V", font_name);
  	else 
    	sprintf(cmap_name, "%s-UCS4-H", font_name);
    	
    XWFontCache cache(true);
    int cmap_id = cache.findCMap(cmap_name);
  	if (cmap_id >= 0) 
  	{
    	delete [] cmap_name;
    	return cmap_id;
  	}
  	
  	XWCMap * cmap = new XWCMap;
  	cmap->setName(cmap_name);
  	cmap->setType(CMAP_TYPE_CODE_TO_CID);
  	cmap->setWMode(wmode);
  	cmap->addCodeSpaceRange(range_min, range_max, 4);
  	cmap->setCIDSysInfo(&CSI_IDENTITY);
  	delete [] cmap_name;
  	
  	uchar  srcCode[4];
  	for (ushort gid = 1; gid < cffont->getNumGlyphs(); gid++)
  	{
  		ushort sid   = cffont->lookupChartsetsInverse(gid);
    	char * glyph = cffont->getString(sid);
    	
    	char * suffix = 0;
    	char * name  = XWAdobeGlyphName::chopSuffix(glyph, &suffix);
    	if (!name) 
    	{
      		if (suffix)
				delete [] suffix;
      		delete [] glyph;
      		continue;
    	}

    	if (suffix) 
    	{
      		delete [] name;
      		delete [] suffix;
      		delete [] glyph;
      		continue;
    	}
    	
    	if (XWAdobeGlyphList::nameIsUnicode(name)) 
    	{
      		long ucv = XWAdobeGlyphList::nameConvertUnicode(name);
      		srcCode[0] = (ucv >> 24) & 0xff;
      		srcCode[1] = (ucv >> 16) & 0xff;
      		srcCode[2] = (ucv >>  8) & 0xff;
      		srcCode[3] = ucv & 0xff;
      		cmap->addCIDChar(srcCode, 4, gid);
    	}
    	else
    	{
    		XWAdobeGlyphName * agln = cache.aglLookupList(name);
    		while (agln)
    		{
    			if (agln->n_components == 1)
    			{
    				long ucv = agln->unicodes[0];
	  				srcCode[0] = (ucv >> 24) & 0xff;
	  				srcCode[1] = (ucv >> 16) & 0xff;
	  				srcCode[2] = (ucv >>  8) & 0xff;
	  				srcCode[3] = ucv & 0xff;
	  				cmap->addCIDChar(srcCode, 4, gid);
    			}
    			
    			agln = agln->alternate;
    		}
    	}
    	
    	delete [] name;
    	if (suffix)
      		delete [] suffix;
    	delete [] glyph;
  	}
  	
  	cmap_id = cache.addCMap(cmap);

  	return cmap_id;
}

