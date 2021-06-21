/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "XWPSError.h"
#include "XWPSPath.h"
#include "XWPSRef.h"
#include "XWPSParam.h"
#include "XWPSStream.h"
#include "XWPSColorSpace.h"
#include "XWPSText.h"
#include "XWPSDeviceMem.h"
#include "XWPSState.h"
#include "XWPSContextState.h"
#include "XWPSFont.h"

#define smax_LARGE 50		/* smax - # of scaled fonts */
#define bmax_LARGE 500000	/* bmax - space for cached chars */
#define mmax_LARGE 200		/* mmax - # of cached font/matrix pairs */
#define cmax_LARGE 5000		/* cmax - # of cached chars */
#define blimit_LARGE 2500	/* blimit/upper - max size of a single cached char */
/*** Small memory machines ***/
#define smax_SMALL 20		/* smax - # of scaled fonts */
#define bmax_SMALL 25000	/* bmax - space for cached chars */
#define mmax_SMALL 40		/* mmax - # of cached font/matrix pairs */
#define cmax_SMALL 500		/* cmax - # of cached chars */
#define blimit_SMALL 100

static int compare_glyphs(const void *pg1, const void *pg2)
{
	ulong g1 = *(const ulong *)pg1;
	ulong g2 = *(const ulong *)pg2;
  return (g1 < g2 ? -1 : g1 > g2 ? 1 : 0);
}

int psf_sort_glyphs(ulong *glyphs, int count)
{
	int i, n;
	qsort(glyphs, count, sizeof(*glyphs), compare_glyphs);
  for (i = n = 0; i < count; ++i)
		if (i == 0 || glyphs[i] != glyphs[i - 1])
	  	glyphs[n++] = glyphs[i];
  return n;
}

int psf_sorted_glyphs_index_of(const ulong *glyphs, int count, ulong glyph)
{
	int lo = 0, hi = count - 1;
  if (hi < 0)
		return -1;
		
  if (glyph < glyphs[0] || glyph > glyphs[hi])
		return -1;
		
	while (hi - lo > 1) 
	{
		int mid = (lo + hi) >> 1;

		if (glyph >= glyphs[mid])
	    lo = mid;
		else
	    hi = mid;
  }
  return (glyph == glyphs[lo] ? lo : glyph == glyphs[hi] ? hi : -1);
}

bool psf_sorted_glyphs_include(const ulong *glyphs, int count, ulong glyph)
{
	return psf_sorted_glyphs_index_of(glyphs, count, glyph) >= 0;
}

XWPSCidSystemInfo::XWPSCidSystemInfo()
{
	Supplement = 0;
}

XWPSCidSystemInfo::~XWPSCidSystemInfo()
{
}

void XWPSCidSystemInfo::setNull()
{
	Registry.data = 0;
	Registry.size = 0;
	Ordering.data = 0;
	Ordering.size = 0;
	Supplement = 0;
}

XWPSCodeSpaceRange::XWPSCodeSpaceRange()
{
	memset(first, 0, MAX_CMAP_CODE_SIZE * sizeof(uchar));
	memset(last, 0, MAX_CMAP_CODE_SIZE * sizeof(uchar));
	size = 0;
}

XWPSCodeSpace::XWPSCodeSpace()
	:ranges(0),
	 num_ranges(0)
{
}

XWPSCodeSpace::~XWPSCodeSpace()
{
	if (ranges)
	{
		delete [] ranges;
		ranges = 0;
	}
}

XWPSCodeLookupRange::XWPSCodeLookupRange()
{
	cmap = 0;
	key_prefix_size = 0;
  key_size = 0;	
  num_keys = 0;
  key_is_range = false;
  value_type = CODE_VALUE_CID;
  value_size = 0;
  font_index = 0;
}

XWPSCodeLookupRange::~XWPSCodeLookupRange()
{
}

XWPSCodeMap::XWPSCodeMap()
	:lookup(0),
	 num_lookup(0)
{
}

XWPSCodeMap::~XWPSCodeMap()
{
	if (lookup)
	{
		delete [] lookup;
		lookup = 0;
	}
}

int XWPSCodeMap::acquireCodeMap(XWPSContextState *ctx, 
	                   XWPSRef * pref, 
	                   XWPSCMap * root)
{
	uint num_lookupA;
  XWPSCodeLookupRange *pclr;
  long i;

  if (!pref->isArray() || (num_lookupA = pref->size()) % 5 != 0)
		return (int)(XWPSError::RangeCheck);
  
  num_lookupA /= 5;
  pclr = new XWPSCodeLookupRange[num_lookupA];
  if (lookup)
  {
  	delete [] lookup;
    lookup = 0;
  }
  lookup = pclr;
  num_lookup = num_lookupA;
  for (i = 0; i < num_lookup * 5; i += 5, ++pclr) 
  {
		XWPSRef rprefix, rmisc, rkeys, rvalues, rfxs;

		pref->arrayGet(ctx, i, &rprefix);
		pref->arrayGet(ctx, i + 1, &rmisc);
		pref->arrayGet(ctx, i + 2, &rkeys);
		pref->arrayGet(ctx, i + 3, &rvalues);
		pref->arrayGet(ctx, i + 4, &rfxs);

		if (!rprefix.hasType(XWPSRef::String) ||
	    	!rmisc.hasType(XWPSRef::String) ||
	    	!rkeys.hasType(XWPSRef::String) ||
	    	!(rvalues.hasType(XWPSRef::String) || rvalues.isArray()) ||
	    	!rfxs.hasType(XWPSRef::Integer))
	  {
	    return (int)(XWPSError::TypeCheck);
	  }
	  
	  uchar * p = rmisc.getBytes();
		if (rmisc.size() != 4 ||
	    p[0] > MAX_CMAP_CODE_SIZE - rprefix.size() ||
	    p[1] > 1 ||
	    p[2] > CODE_VALUE_MAX ||
	    p[3] == 0)
	  {
			return (int)(XWPSError::RangeCheck);
	  }
		pclr->cmap = root;
		pclr->key_size = p[0];
		pclr->key_prefix_size = rprefix.size();
		memcpy(pclr->key_prefix, rprefix.getBytes(), pclr->key_prefix_size);
		pclr->key_is_range = p[1];
		if (pclr->key_size == 0) 
		{
	    if (rkeys.size() != 0)
				return (int)(XWPSError::RangeCheck);
	    pclr->num_keys = 1;
		} 
		else 
		{
	    int step = pclr->key_size * (pclr->key_is_range ? 2 : 1);

	    if (rkeys.size() % step != 0)
				return (int)(XWPSError::RangeCheck);
	    pclr->num_keys = rkeys.size() / step;
		}
		pclr->keys.data = rkeys.getBytes();
		pclr->keys.size = rkeys.size();
		pclr->keys.bytes = rkeys.value.bytes->arr;
		if (rkeys.value.bytes->arr)
			rkeys.value.bytes->arr->incRef();
		pclr->value_type = (PSCodeValueType)(p[2]);
		pclr->value_size = p[3];
		if (rvalues.hasType(XWPSRef::String)) 
		{
	    if (pclr->value_type == CODE_VALUE_GLYPH)
				return (int)(XWPSError::RangeCheck);
	    if (rvalues.size() % pclr->num_keys != 0 ||	rvalues.size() / pclr->num_keys != pclr->value_size)
				return (int)(XWPSError::RangeCheck);
			pclr->values.size = rvalues.size();
			pclr->values.data = rvalues.getBytes();
			pclr->values.bytes = rvalues.value.bytes->arr;
			if (rvalues.value.bytes->arr)
			  rvalues.value.bytes->arr->incRef();
		} 
		else 
		{
	    uint values_size = pclr->num_keys * pclr->value_size;
	    long k;
	    uchar *pvalue;

	    if (pclr->value_type != CODE_VALUE_GLYPH ||
					rvalues.size() != pclr->num_keys ||
					pclr->value_size > sizeof(ulong))
			{
				return (int)(XWPSError::RangeCheck);
			}
			pclr->values.bytes = new XWPSBytes(values_size);
	    pclr->values.data = pclr->values.bytes->arr;
	    pclr->values.size = values_size;
	    pvalue = pclr->values.data;
	    for (k = 0; k < pclr->num_keys; ++k) 
	    {
				XWPSRef rvalue;
				ulong value;
				int i;

				rvalues.arrayGet(ctx, k, &rvalue);
				if (!rvalue.hasType(XWPSRef::Name))
					return (int)(XWPSError::RangeCheck);
				value = ctx->nameIndex(&rvalue);
				
				if (pclr->value_size < sizeof(value) && (value >> (pclr->value_size * 8)) != 0)
					return (int)(XWPSError::RangeCheck);
				for (i = pclr->value_size; --i >= 0; )
		    	*pvalue++ = (uchar)(value >> (i * 8));
	    }
		}
		rfxs.checkIntLEUOnly(0xff);
		pclr->font_index = (int)rfxs.value.intval;
  }
  return 0;
}

uint XWPSCodeMap::bytes2int(const uchar *p, int n)
{
	uint v = 0;
  int i;

  for (i = 0; i < n; ++i)
    v = (v << 8) + p[i];
  return v;
}

int XWPSCodeMap::decodeNext(const XWPSString * pstr,
                 uint * pindex, 
                 uint * pfidx,
                 ulong * pchr, 
                 ulong * pglyph)
{
	uchar *str = pstr->data + *pindex;
  uint ssize = pstr->size - *pindex;
  int i;

  for (i = num_lookup - 1; i >= 0; --i) 
  {
    XWPSCodeLookupRange *pclr = &lookup[i];
    int pre_size = pclr->key_prefix_size, key_size = pclr->key_size, chr_size = pre_size + key_size;

    if (ssize < chr_size)
       continue;
    if (memcmp(str, pclr->key_prefix, pre_size))
       continue;
       
    {
      uchar *key = pclr->keys.data;
      int step = key_size;
      int k;
      uchar *pvalue;

      if (pclr->key_is_range) 
      {
        step <<= 1;
        for (k = 0; k < pclr->num_keys; ++k, key += step)
      	  if (memcmp(str + pre_size, key, key_size) >= 0 &&
              memcmp(str + pre_size, key + key_size, key_size) <= 0)
               break;
      } 
      else 
      {
        for (k = 0; k < pclr->num_keys; ++k, key += step)
          if (!memcmp(str + pre_size, key, key_size))
              break;
      }
      if (k == pclr->num_keys)
         continue;
         
      *pchr = (*pchr << (chr_size * 8)) + bytes2int(str, chr_size);
      *pindex += chr_size;
      *pfidx = pclr->font_index;
      pvalue = pclr->values.data + k * pclr->value_size;
      switch (pclr->value_type) 
      {
        case CODE_VALUE_CID:
          *pglyph = ps_min_cid_glyph + bytes2int(pvalue, pclr->value_size) +
                    bytes2int(str + pre_size, key_size) - bytes2int(key, key_size);
           return 0;
           
        case CODE_VALUE_GLYPH:
          *pglyph = bytes2int(pvalue, pclr->value_size);
          return 0;
            
        case CODE_VALUE_CHARS:
          *pglyph = bytes2int(pvalue, pclr->value_size) +
                    bytes2int(str + pre_size, key_size) - bytes2int(key, key_size);
          return pclr->value_size;
            
        default: 
          return (int)(XWPSError::RangeCheck);
      }
    }
  }
  *pglyph = ps_no_glyph;
  return 0;
}

XWPSCMap::XWPSCMap()
	:XWPSStruct()
{
	CMapType = 0;
	CIDSystemInfo = 0;
	num_fonts = 0;
	CMapVersion = 0;
	UIDOffset = 0;
	WMode = 0;
	mark_glyph_data = 0;
	glyph_name_data = 0;
}

XWPSCMap::~XWPSCMap()
{
	if (CIDSystemInfo)
	{
		delete [] CIDSystemInfo;
		CIDSystemInfo = 0;
	}
}

int XWPSCMap::acquireCodeRanges(XWPSContextState *ctx, XWPSRef * pref)
{
	uint num_ranges;
  XWPSCodeSpaceRange *ranges;
  uint i;

  if (!pref->isArray() || (num_ranges = pref->size()) == 0 ||	num_ranges & 1)
		return (int)(XWPSError::RangeCheck);
  
  num_ranges >>= 1;
  ranges = new XWPSCodeSpaceRange[num_ranges];
  if (code_space.ranges)
  {
  	delete [] code_space.ranges;
  	code_space.ranges = 0;
  }
  code_space.ranges = ranges;
  code_space.num_ranges = num_ranges;
  for (i = 0; i < num_ranges; ++i, ++ranges) 
  {
		XWPSRef rfirst, rlast;
		int size;

		pref->arrayGet(ctx, i * 2L, &rfirst);
		pref->arrayGet(ctx, i * 2L + 1, &rlast);
		if (!rfirst.hasType(XWPSRef::String) ||
	    	!rlast.hasType(XWPSRef::String) ||
	    	(size = rfirst.size()) == 0 || size > MAX_CMAP_CODE_SIZE ||
	    	rlast.size() != size ||
	    	memcmp(rfirst.getBytes(), rlast.getBytes(), size) > 0)
	  {
			return (int)(XWPSError::RangeCheck);
	  }
		memcpy(ranges->first, rfirst.getBytes(), size);
		memcpy(ranges->last, rlast.getBytes(), size);
		ranges->size = size;
  }
  return 0;
}

int XWPSCMap::decodeNext(const XWPSString * pstr,
                 uint * pindex, 
                 uint * pfidx,
                 ulong * pchr, 
                 ulong * pglyph)
{
	uint save_index = *pindex;
  int code;

  *pchr = 0;
  code = def.decodeNext(pstr, pindex, pfidx, pchr, pglyph);
  if (code != 0 || *pglyph != ps_no_glyph)
    return code;    *pindex = save_index;
  *pchr = 0;
  return notdef.decodeNext(pstr, pindex, pfidx, pchr, pglyph);
}

int XWPSCMap::getLength()
{
	return sizeof(XWPSCMap);
}

const char * XWPSCMap::getTypeName()
{
	return "cmap";
}

XWPSPSFGlyphEnum::XWPSPSFGlyphEnum()
{
	font = 0;
	subset.selected.list = 0;
	subset.size = 0;
	glyph_space = GLYPH_SPACE_NAME;
	index = 0;
}

int XWPSPSFGlyphEnum::enumerateGlyphsNext(ulong *pglyph)
{
	return (this->*enumerate_next)(pglyph);
}

int XWPSPSFGlyphEnum::bitsNext(ulong *pglyph)
{
	for (; index < subset.size; index++)
		if (subset.selected.bits[index >> 3] & (0x80 >> (index & 7))) 
		{
	    *pglyph = (ulong)(index++ + ps_min_cid_glyph);
	    return 0;
		}
  return 1;
}

int XWPSPSFGlyphEnum::fontNext(ulong *pglyph)
{
	int i = (int)(index);
	int code = font->enumerateGlyph(&i, glyph_space, pglyph);
	index = i;
  return (i == 0 ? 1 : code < 0 ? code : 0);
}

int XWPSPSFGlyphEnum::glyphsNext(ulong *pglyph)
{
	if (index >= subset.size)
		return 1;
    
  *pglyph = subset.selected.list[index++];
  return 0;
}

int XWPSPSFGlyphEnum::rangeNext(ulong *pglyph)
{
	if (index >= subset.size)
		return 1;
  *pglyph = (ulong)(index++ + ps_min_cid_glyph);
  return 0;
}

void XWPSPSFGlyphEnum::bitsBegin(XWPSFont *fontA,
			 					                 const uchar *subset_bits, 
			 					                 uint subset_size,
			                           PSGlyphSpace glyph_spaceA)
{
	font = fontA;
  subset.selected.bits = subset_bits;
  subset.size = subset_size;
  glyph_space = glyph_spaceA;
  enumerate_next =	(subset_bits ? (&XWPSPSFGlyphEnum::bitsNext) :
	                 subset_size ? (&XWPSPSFGlyphEnum::rangeNext) : (&XWPSPSFGlyphEnum::fontNext));
  index = 0;
}

void XWPSPSFGlyphEnum::listBegin(XWPSFont *fontA,
			                           const ulong *subset_list, 
			                           uint subset_size,
			                           PSGlyphSpace glyph_spaceA)
{
	font = fontA;
  subset.selected.list = subset_list;
  subset.size = subset_size;
  glyph_space = glyph_spaceA;
  enumerate_next = (subset_list ? (&XWPSPSFGlyphEnum::glyphsNext) :
	 									subset_size ? (&XWPSPSFGlyphEnum::rangeNext) : (&XWPSPSFGlyphEnum::fontNext));
  index = 0;
}

XWPSPSFOutlineGlyphs::XWPSPSFOutlineGlyphs()
{
	notdef = ps_no_glyph;
	memset(subset_data, 0, (256 * 3 + 1) * sizeof(ulong));
	subset_glyphs = 0;
	subset_size = 0;
}

XWPSPSFOutlineGlyphs::~XWPSPSFOutlineGlyphs()
{
}

XWPSFontInfo::XWPSFontInfo()
{
	members = 0;
	Ascent = 0;
	AvgWidth = 0;
	CapHeight = 0;
	Descent = 0;
	Flags = 0;	
	Flags_requested = 0;
	Flags_returned = 0;
	ItalicAngle = 0.0;
	Leading = 0;
	MaxWidth;
	MissingWidth = 0;
	StemH = 0;
	StemV = 0;
	UnderlinePosition = 0;
	UnderlineThickness = 0;
	XHeight = 0;
}

XWPSGlyphInfo::XWPSGlyphInfo()
{
	members = 0;
	num_pieces = 0;
	pieces = 0;
}

XWPSGlyphInfo::~XWPSGlyphInfo()
{
}

XWPSXFont::XWPSXFont()
{
}

XWPSXFont::~XWPSXFont()
{
}

XWPSCachedFMPair::XWPSCachedFMPair()
	:font(0),
	 FontType(ft_composite),
	 hash(0),
	 mxx(0),
	 mxy(0),
	 myx(0),
	 myy(0),
	 num_chars(0),
	 xfont_tried(false),
	 xfont(0),
	 index(0)
{
}

XWPSCachedFMPair::~XWPSCachedFMPair()
{
}

void XWPSCachedFMPair::setFree()
{
	font = 0;
	UID.invalid();
}

XWPSFMPairCache::XWPSFMPairCache()
	:msize(0), 
	 mmax(0),
	 mdata(0),
	 mnext(0)
{
}

XWPSFMPairCache::~XWPSFMPairCache()
{
	if (mdata)
	{
		delete [] mdata;
		mdata = 0;
	}
}

XWPSCachedChar::XWPSCachedChar()
	:XWPSCachedBits()
{
	code = 0;
	wmode = 0;
	chunk = 0;
	loc = 0;
	pair_index = 0;
	xglyph = 0;
}

XWPSCachedChar::~XWPSCachedChar()
{
}

XWPSCachedChar * XWPSCachedChar::alloc(XWPSFontDir * dir, 
	               XWPSDeviceMem * dev,
		   					 XWPSDeviceMem * dev2, 
		   					 ushort iwidth, 
		   					 ushort iheight,
		             const XWPSLog2ScalePoint * pscale, 
		             int depth)
{
	XWPSCachedChar * cc = 0;
	int log2_xscale = pscale->x;
  int log2_yscale = pscale->y;
  int log2_depth = ilog2(depth);
  uint nwidth_bits = (iwidth >> log2_xscale) << log2_depth;
  ulong isize, icdsize;
  uint iraster;
  XWPSDeviceMem mdev;
  mdev.incRef();
  XWPSDeviceMem *pdev = dev;
  XWPSDeviceMem *pdev2;
  if (dev == NULL) 
  	pdev = &mdev;
  
  pdev2 = (dev2 == 0 ? pdev : dev2);
  iraster = bitmap_raster(nwidth_bits);
  if (iraster != 0 && iheight >> log2_yscale > dir->ccache.upper / iraster)
  	return cc;
  	
  if (dev2 == 0)
  {
  	XWPSDevice *target = pdev->target;

		pdev->makeMemMonoDevice(target);
		pdev->retained = pdev->retained;
		pdev->width = iwidth;
		pdev->height = iheight;
		isize = pdev->bitmapSize();
  }
  else
  {
  	dev2->makeMemAlphaDevice(NULL, depth);
		dev2->width = iwidth >> log2_xscale;
		dev2->height = iheight >> log2_yscale;
		dev->makeMemABufDevice(dev2,	(XWPSLog2ScalePoint*)pscale, depth, 0);
		dev->width = iwidth;
		dev->height = 2 << log2_yscale;
		isize = dev->bitmapSize() + dev2->bitmapSize();
  }
  icdsize = isize + sizeof_cached_char;
  cc = dir->allocChar(icdsize);
  if (cc == 0)
  	return cc;
  	
  cc->code = 0;
	cc->wmode = 0;
	cc->chunk = 0;
	cc->loc = 0;
	cc->pair_index = 0;
  cc->depth = depth;
  cc->xglyph = ps_no_xglyph;
  cc->width = pdev2->width;
  cc->height = pdev2->height;
  cc->shift = 0;
  cc->raster = pdev2->getRasterMem();
  cc->pair = 0;
  cc->id = ps_no_bitmap_id;
  if (dev2)
  {
		uchar *bits = cc_bits(cc);
		uint bsize = (uint) (dev2->bitmapSize());

		memset(bits, 0, bsize);
		dev2->base = bits;
		dev2->open();
		dev->base = bits + bsize;
		dev->open();
  } 
  else if (dev)
		dev->openCacheDevice(cc);
		
	return cc;
}

uint XWPSCachedChar::charsHeadIndex()
{
	return pair->charsHeadIndex(code);
}

uchar * XWPSCachedChar::compressAlphaBits()
{
	const uchar *data = cc_const_bits(this);
	int log2_scale = depth;
  int scale = 1 << log2_scale;
  uint sraster = raster;
  uint sskip = sraster - ((width * scale + 7) >> 3);
  uint draster = bitmap_raster(width);
  uint dskip = draster - ((width + 7) >> 3);
  
  uchar *mask = (uchar*)malloc((draster * height+1) * sizeof(uchar));
  const uchar *sptr = data;
  uchar *dptr = mask;
  uint h;

  if (mask == 0)
		return 0;
    
  for (h = height; h; --h) 
  {
		uchar sbit = 0x80;
		uchar d = 0;
		uchar dbit = 0x80;
		uint w;

		for (w = width; w; --w) 
		{
	    if (*sptr & sbit)
				d += dbit;
	    if (!(sbit >>= log2_scale))
				sbit = 0x80, sptr++;
	    if (!(dbit >>= 1))
				dbit = 0x80, dptr++, d = 0;
		}
		if (dbit != 0x80)
	    *dptr++ = d;
		for (w = dskip; w != 0; --w)
	    *dptr++ = 0;
		sptr += sskip;
  }
  return mask;
}

bool XWPSCachedChar::purgeFMPairChar(void *vpair)
{
	return pair == (XWPSCachedFMPair*)vpair;
}

bool XWPSCachedChar::purgeFMPairCharXFont(void *vpair)
{
	return ((pair == ((XWPSCachedFMPair*)vpair)) && ((XWPSCachedFMPair*)vpair)->xfont == 0 && !hasBits());
}

void XWPSCachedChar::setPair(XWPSCachedFMPair *p)
{
	pair = p;
	pair_index = p->index;
}

bool XWPSCachedChar::selectCidRange(void *range_ptr)
{
	PSFontCidRange *range = (PSFontCidRange*)range_ptr;

  return (code >= range->cid_min && code <= range->cid_max && pair->font == range->font);
}

XWPSCharCache::XWPSCharCache()
	:XWPSBitsCache()
{
	table = 0;
	table_mask = 0;
	bmax = 0;
	cmax = 0;
	bspace = 0;
	lower = 0;
	upper = 0;
	mark_glyph = 0;
	mark_glyph_data = 0;
}

XWPSCharCache::~XWPSCharCache()
{
	if (table)
	{
		free(table);
		table = 0;
	}
}

XWPSFontDir::XWPSFontDir()
{
	orig_fonts = 0;
	scaled_fonts = 0;
	ssize = 0;
	smax = smax_LARGE;
	enum_index = 0;
	enum_offset = 0;
	ccache.mark_glyph = &XWPSContextState::ccNoMarkGlyph;
	charCacheAlloc(bmax_LARGE, mmax_LARGE, cmax_LARGE, blimit_LARGE);
	i_ctx_p = 0;
}

XWPSFontDir::XWPSFontDir(uint smaxA, uint bmax, uint mmax, uint cmax, uint upper)
{
	orig_fonts = 0;
	scaled_fonts = 0;
	ssize = 0;
	smax = smaxA;
	enum_index = 0;
	enum_offset = 0;
	ccache.mark_glyph = &XWPSContextState::ccNoMarkGlyph;
	charCacheAlloc(bmax, mmax, cmax, upper);
	i_ctx_p = 0;
}

XWPSFontDir::~XWPSFontDir()
{
	while (!chunks.isEmpty())
	{
		XWPSBitsCacheChunk * tmp = chunks.takeFirst();
		if (tmp)
			delete tmp;
	}
}

void XWPSFontDir::addCachedChar(XWPSDeviceMem * dev,
	                   XWPSCachedChar * cc, 
	                   XWPSCachedFMPair * pair, 
	                   const XWPSLog2ScalePoint * pscale)
{
	if (dev != NULL) 
	{
		static const XWPSLog2ScalePoint no_scale;

		dev->close();
		addCharBits(cc, (dev->isABuf() ? &no_scale : pscale));
  }
  
  {
		uint chi = pair->charsHeadIndex(cc->code);

		while (ccache.table[chi &= ccache.table_mask] != 0)
	    chi++;
		ccache.table[chi] = cc;
		cc->setPair(pair);
		pair->num_chars++;
  }
}

void XWPSFontDir::addCharBits(XWPSCachedChar * cc,
		 							const XWPSLog2ScalePoint * plog2_scale)
{
	int log2_x = plog2_scale->x, log2_y = plog2_scale->y;
  uint raster = cc->raster;
  uchar *bits = cc_bits(cc);
  int depth = cc->depth;
  int log2_depth = ilog2(depth);
  uint nwidth_bits, nraster;
  XWPSIntRect bbox;
  
  bits_bounding_box(bits, cc->height, raster, &bbox);
  if ((log2_x | log2_y) != 0)
  {
  	{
	    int scale_x = 1 << log2_x;

	    bbox.p.x &= -scale_x;
	    bbox.q.x = (bbox.q.x + scale_x - 1) & -scale_x;
		}
		{
	    int scale_y = 1 << log2_y;

	    bbox.p.y &= -scale_y;
	    bbox.q.y = (bbox.q.y + scale_y - 1) & -scale_y;
		}
		cc->width = (bbox.q.x - bbox.p.x) >> log2_x;
		cc->height = (bbox.q.y - bbox.p.y) >> log2_y;
		nwidth_bits = cc->width << log2_depth;
		nraster = bitmap_raster(nwidth_bits);
		bits_compress_scaled(bits + raster * bbox.p.y, bbox.p.x,
			     cc->width << log2_x,
			     cc->height << log2_y,
			     raster,
			     bits, nraster, plog2_scale, log2_depth);
		bbox.p.x >>= log2_x;
		bbox.p.y >>= log2_y;
  }
  else
  {
  	const uchar *from = bits + raster * bbox.p.y + (bbox.p.x >> 3);

		cc->height = bbox.q.y - bbox.p.y;
		bbox.p.x &= ~7;
		bbox.p.x >>= log2_depth;
		bbox.q.x = (bbox.q.x + depth - 1) >> log2_depth;
		cc->width = bbox.q.x - bbox.p.x;
		nwidth_bits = cc->width << log2_depth;
		nraster = bitmap_raster(nwidth_bits);
		if (bbox.p.x != 0 || nraster != raster) 
		{
	    uchar *to = bits;
	    uint n = cc->height;
	    for (; n--; from += raster, to += nraster)
				memmove(to, from, nraster);
		} 
		else if (bbox.p.y != 0) 
	    memmove(bits, from, raster * cc->height);
  }
  
  cc->offset.x -= int2fixed(bbox.p.x);
  cc->offset.y -= int2fixed(bbox.p.y);
  cc->setRaster(nraster);
  {
		uint diff = ROUND_DOWN(cc->size - sizeof_cached_char - nraster * cc->height, align_cached_char_mod);

		if (diff >= sizeof(XWPSCachedBitsHead)) 
	    shortenCachedChar(cc, diff);
  }
  
  if (i_ctx_p)
  	cc->id = i_ctx_p->nextIDS(1);
}

XWPSCachedFMPair * XWPSFontDir::addFMPair(XWPSFont * font, 
	                             XWPSUid * puid,
	                             XWPSState * pgs)
{
	XWPSCachedFMPair *pair = fmcache.mdata + fmcache.mnext;
  XWPSCachedFMPair *mend = fmcache.mdata + fmcache.mmax;
  if (fmcache.msize == fmcache.mmax)
  {
  	int count;
		for (count = fmcache.mmax; --count >= 0 && pair->num_chars != 0;)
	  	if (++pair == mend)
				pair = fmcache.mdata;
				
		purgeFMPair(pair, 0);
  }
  else
  {
  	while (!pair->isFree())
	    if (++pair == mend)
				pair = fmcache.mdata;
  }
  
  fmcache.msize++;
  fmcache.mnext = pair + 1 - fmcache.mdata;
  if (fmcache.mnext == fmcache.mmax)
		fmcache.mnext = 0;
  pair->font = font;
  pair->UID = *puid;
  pair->FontType = font->FontType;
  pair->hash = (uint) (ulong) pair % 549;	/* scramble bits */
  pair->mxx = pgs->char_tm.xx, pair->mxy = pgs->char_tm.xy;
  pair->myx = pgs->char_tm.yx, pair->myy = pgs->char_tm.yy;
  pair->num_chars = 0;
  pair->xfont_tried = false;
  pair->xfont = 0;
  return pair;
}

XWPSCachedChar * XWPSFontDir::allocChar(ulong icdsize)
{
	XWPSCachedChar * cc = allocCharInChunk(icdsize);
	if (cc == 0)
	{
		if (ccache.bspace < ccache.bmax)
		{
			XWPSBitsCacheChunk * cck_prev = ccache.chunks;
			XWPSBitsCacheChunk *cck = 0;
			uint cksize = ccache.bmax / 5 + 1;
	    uint tsize = ccache.bmax - ccache.bspace;
	    uchar *cdata;

	    if (cksize > tsize)
		    cksize = tsize;
		  if (icdsize + sizeof(XWPSCachedBitsHead) > cksize)
		  	return 0;
		  	
		  cck = new XWPSBitsCacheChunk;
		  cdata = (uchar*)malloc(cksize);
		  cck->init(cdata, cksize);
		  cck->next = cck_prev->next;
	    cck_prev->next = cck;
	    ccache.bspace += cksize;
	    ccache.chunks = cck;
		}
		else
		{
			XWPSBitsCacheChunk *cck_init = ccache.chunks;
	    XWPSBitsCacheChunk *cck = cck_init;
	    while ((ccache.chunks = cck = cck->next) != cck_init) 
	    {
		    ccache.cnext = 0;
		    cc = allocCharInChunk(icdsize);
		    if (cc != 0)
		      return cc;
	    }
		}
		
		ccache.cnext = 0;
	  cc = allocCharInChunk(icdsize);
	}
	
	return cc;
}

XWPSCachedChar * XWPSFontDir::allocCharInChunk(ulong icdsize)
{
	XWPSBitsCacheChunk * cck = ccache.chunks;
	XWPSCachedChar * cch = 0;
	while (ccache.alloc(icdsize, (XWPSCachedBitsHead**)&cch) < 0)
	{
		if (cch == 0)
			return 0;
	}
	
	if (cch->pair)
	{
		uint chi = cch->charsHeadIndex();
		while (ccache.table[chi & ccache.table_mask] != cch)
		    chi++;
		hashRemoveCachedChar(chi);
	}
	
	freeCachedChar(cch);
	cch->chunk = cck;
	cch->loc = (uchar *) cch - cck->data;
  return cch;
}

void XWPSFontDir::cacheStatus(uint pstat[7])
{
	pstat[0] = ccache.bsize;
  pstat[1] = ccache.bmax;
  pstat[2] = fmcache.msize;
  pstat[3] = fmcache.mmax;
  pstat[4] = ccache.csize;
  pstat[5] = ccache.cmax;
  pstat[6] = ccache.upper;
}

int XWPSFontDir::charCacheAlloc(uint bmax, uint mmax, uint cmax, uint upper)
{
	uint chsize = (cmax + (cmax >> 1)) | 31;
	
  while (chsize & (chsize + 1))
		chsize |= chsize >> 1;
    
  chsize++;
  
  XWPSCachedFMPair * mdata = new XWPSCachedFMPair[mmax];
  XWPSCachedChar ** chars = (XWPSCachedChar**)malloc((chsize+1) * sizeof(XWPSCachedChar*));
  memset(chars, 0, chsize * sizeof(XWPSCachedChar*));
  fmcache.mmax = mmax;
  fmcache.mdata = mdata;
  ccache.bmax = bmax;
  ccache.cmax = cmax;
  ccache.lower = upper / 10;
  ccache.upper = upper;
  ccache.table = chars;
  ccache.table_mask = chsize - 1;
  init();
  return 0;
}

void XWPSFontDir::freeCachedChar(XWPSCachedChar * cc)
{
	XWPSBitsCacheChunk *cck = cc->chunk;
	ccache.chunks = cck;
	ccache.cnext = (uchar *) cc - cck->data;
	if (cc->pair != 0)
		cc->pair->num_chars--;
	
	ccache.bitsCacheFree(cc, cck);
}

void XWPSFontDir::hashRemoveCachedChar(uint chi)
{
	uint mask = ccache.table_mask;
  uint from = ((chi &= mask) + 1) & mask;
  XWPSCachedChar *cc;

  ccache.table[chi] = 0;
  while ((cc = ccache.table[from]) != 0)
  {
  	uint fchi = cc->pair->charsHeadIndex(cc->code);
  	if ((chi < from ? chi <= fchi && fchi < from :
	     chi <= fchi || fchi < from))
	  {
	  	ccache.table[chi] = cc;
	    ccache.table[from] = 0;
	    chi = from;
	  }
	  from = (from + 1) & mask;
  }
}

void XWPSFontDir::init()
{
	int i;
  XWPSCachedFMPair *pair;
  XWPSBitsCacheChunk *cck = new XWPSBitsCacheChunk;

  fmcache.msize = 0;
  fmcache.mnext = 0;
  
  ccache.init(cck);
  ccache.bspace = 0;
  memset((char *)ccache.table, 0, (ccache.table_mask + 1) * sizeof(XWPSCachedChar *));
  for (i = 0, pair = fmcache.mdata;	 i < fmcache.mmax; i++, pair++) 
		pair->index = i;
}

void XWPSFontDir::purgeFMPair(XWPSCachedFMPair * pair, int xfont_only)
{
	if (pair->xfont != 0) 
	{
		delete pair->xfont;
		pair->xfont_tried = false;
		pair->xfont = 0;
  }
  
  purgeSelectedCachedChars((xfont_only ? &XWPSCachedChar::purgeFMPairCharXFont :
				    								&XWPSCachedChar::purgeFMPairChar), pair);
	if (!xfont_only)
	{
		pair->setFree();
		fmcache.msize--;
	}
}

void XWPSFontDir::purgeFontFromCharCaches(const XWPSFont * font)
{
	XWPSCachedFMPair *pair = fmcache.mdata;
  int count = fmcache.mmax;

  while (count--) 
  {
		if (pair->font == font) 
		{
	    if (pair->UID.isValid()) 
				pair->font = 0;
	    else
				purgeFMPair(pair, 0);
		}
		pair++;
  }
}

void XWPSFontDir::purgeSelectedCachedChars(bool(XWPSCachedChar::*proc)(void *), void *proc_data)
{
	int chi;
  int cmax = ccache.table_mask;

  for (chi = 0; chi <= cmax;) 
 	{
		XWPSCachedChar *cc = ccache.table[chi];

		if (cc != 0 && (cc->*proc)(proc_data)) 
		{
	    hashRemoveCachedChar(chi);
	    freeCachedChar(cc);
		} 
		else
	    chi++;
  }
}

int XWPSFontDir::setCacheLower(uint size)
{
	ccache.lower = size;
  return 0;
}

int XWPSFontDir::setCacheSize(uint size)
{
	ccache.bmax = size;
  return 0;
}

int XWPSFontDir::setCacheUpper(uint size)
{
	ccache.upper = size;
  return 0;
}

void XWPSFontDir::shortenCachedChar(XWPSCachedChar * cc, uint diff)
{
	ccache.bitsCacheShorten(cc, diff, cc->chunk);
}

#define U8(p) ((uint)((p)[0]))
#define S8(p) (int)((U8(p) ^ 0x80) - 0x80)
#define U16(p) (((uint)((p)[0]) << 8) + (p)[1])
#define S16(p) (int)((U16(p) ^ 0x8000) - 0x8000)
#define u32(p) get_u32_msb(p)

static void put_u16(uchar *p, uint v)
{
    p[0] = (uchar)(v >> 8);
    p[1] = (uchar)v;
}

static void put_u32(uchar *p, ulong v)
{
    p[0] = (uchar)(v >> 24);
    p[1] = (uchar)(v >> 16);
    p[2] = (uchar)(v >> 8);
    p[3] = (uchar)v;
}

static ulong put_table(uchar tab[16], 
                       const char *tname, 
                       ulong checksum, 
                       ulong offset,
	                     uint length)
{
	memcpy(tab, (const uchar *)tname, 4);
  put_u32(tab + 4, checksum);
  put_u32(tab + 8, offset + 0x40000000);
  put_u32(tab + 12, (ulong)length);
  return offset + ROUND_UP(length, 4);
}

static const uchar cmap_initial_0[] = {
    0, 0,		/* table version # = 0 */
    0, 2,		/* # of encoding tables = 2 */

	/* First table, Macintosh */
    0, 1,		/* platform ID = Macintosh */
    0, 0,		/* platform encoding ID = ??? */
    0, 0, 0, 4+8+8,	/* offset to table start */
	/* Second table, Windows */
    0, 3,		/* platform ID = Microsoft */
    0, 0,		/* platform encoding ID = unknown */
    0, 0, 1, 4+8+8+6,	/* offset to table start */

	/* Start of Macintosh format 0 table */
    0, 0,		/* format = 0, byte encoding table */
    1, 6,		/* length */
    0, 0		/* version number */
};
static const uchar cmap_initial_6[] = {
    0, 0,		/* table version # = 0 */
    0, 2,		/* # of encoding tables = 2 */

	/* First table, Macintosh */
    0, 1,		/* platform ID = Macintosh */
    0, 0,		/* platform encoding ID = ??? */
    0, 0, 0, 4+8+8,	/* offset to table start */
	/* Second table, Windows */
    0, 3,		/* platform ID = Microsoft */
    0, 0,		/* platform encoding ID = unknown */
    0, 0, 0, 4+8+8+10,	/* offset to table start */
			/****** VARIABLE, add 2 x # of entries ******/

	/* Start of Macintosh format 6 table */
    0, 6,		/* format = 6, trimmed table mapping */
    0, 10,		/* length ****** VARIABLE, add 2 x # of entries ******/
    0, 0,		/* version number */
    0, 0,		/* first character code */
    0, 0		/* # of entries ****** VARIABLE ****** */
};
static const uchar cmap_initial_4[] = {
    0, 0,		/* table version # = 0 */
    0, 1,		/* # of encoding tables = 2 */

	/* Single table, Windows */
    0, 3,		/* platform ID = Microsoft */
    0, 0,		/* platform encoding ID = unknown */
    0, 0, 0, 4+8	/* offset to table start */
};
static const uchar cmap_sub_initial[] = {
    0, 4,		/* format = 4, segment mapping */
    0, 32,		/* length ** VARIABLE, add 2 x # of glyphs ** */
    0, 0,		/* version # */
    0, 4,		/* 2 x segCount */
    0, 4,		/* searchRange = 2 x 2 ^ floor(log2(segCount)) */
    0, 1,		/* floor(log2(segCount)) */
    0, 0,		/* 2 x segCount - searchRange */

    0, 0,		/* endCount[0] **VARIABLE** */
    255, 255,		/* endCount[1] */
    0, 0,		/* reservedPad */
    0, 0,		/* startCount[0] **VARIABLE** */
    255, 255,		/* startCount[1] */
    0, 0,		/* idDelta[0] */
    0, 1,		/* idDelta[1] */
    0, 4,		/* idRangeOffset[0] */
    0, 0		/* idRangeOffset[1] */
};

static const uchar name_initial[] = {
    0, 0,			/* format */
    0, 1,			/* # of records = 1 */
    0, 18,			/* start of string storage */

    0, 2,			/* platform ID = ISO */
    0, 2,			/* encoding ID = ISO 8859-1 */
    0, 0,			/* language ID (none) */
    0, 6,			/* name ID = PostScript name */
    0, 0,			/* length ****** VARIABLE ****** */
    0, 0			/* start of string within string storage */
};
static uint
size_name(const XWPSString *font_name)
{
    return sizeof(name_initial) + font_name->size;
}
static int compare_post_glyphs(const void *pg1, const void *pg2)
{
	ulong g1 = ((const PSPostGlyph *)pg1)->glyph_index, g2 = ((const PSPostGlyph *)pg2)->glyph_index;
  return (g1 < g2 ? -1 : g1 > g2 ? 1 : 0);
}

static int compare_table_tags(const void *pt1, const void *pt2)
{
	ulong t1 = u32((const uchar*)pt1), t2 = u32((const uchar*)pt2);
  return (t1 < t2 ? -1 : t1 > t2 ? 1 : 0);
}

XWPSFont::XWPSFont()
	:XWPSStruct()
{
	procs.define_font_.define_font = &XWPSFont::defineFontNo;
	procs.make_font_.make_font = &XWPSFont::makeFontNo;
	procs.font_info_.font_info = &XWPSFont::fontInfoDefault;
	procs.same_font_.same_font = &XWPSFont::sameFontDefault;
	procs.encode_char_.encode_char = &XWPSFont::encodeCharNo;
	procs.enumerate_glyph_.enumerate_glyph = &XWPSFont::enumerateGlyphNo;
	procs.glyph_info_.glyph_info = &XWPSFont::glyphInfoDefault;
	procs.glyph_outline_.glyph_outline = &XWPSFont::glyphOutlineNo;
	procs.init_fstack_.init_fstack = &XWPSFont::initFStackDefault;
	procs.next_char_glyph_.next_char_glyph = &XWPSTextEnum::nextCharGlyphDefault;
	procs.build_char_.build_char = &XWPSFont::buildCharNo;
		
	next = 0;
	prev = 0;
	dir = 0;
	is_resource = false;
	id = 0;
	base = this;
	client_data = 0;
	FontType = ft_composite;
	BitmapWidths = false;
	ExactSize = fbit_use_outlines;
	InBetweenSize = fbit_use_outlines;
	TransformedChar = fbit_use_outlines;
	WMode = 0;
	PaintType = 0;
	StrokeWidth = 0.0;
	key_name.size = 0;
	font_name.size = 0;
	i_ctx_p = 0;
	pfont_data = 0;
	not_def = 0;
}

XWPSFont::~XWPSFont()
{
	if (client_data)
	{
		XWPSFontData * fd = (XWPSFontData*)(client_data);
		if (!fd->incRef())
			delete fd;
		client_data = 0;
	}
	
	if (pfont_data)
	{
		delete [] pfont_data;
		pfont_data = 0;
	}
	
	if (not_def)
	{
		free(not_def);
		not_def = 0;
	}
}

int XWPSFont::addSubsetPieces(ulong *glyphs, 
	                            uint *pcount, 
	                            uint max_count,
		                          uint max_pieces)
{
	uint count = *pcount;
	for (uint i = 0; i < count; ++i)
	{
		XWPSGlyphInfo info;
		int code;
		if (count + max_pieces > max_count)
		{
			code = glyphInfo(glyphs[i], NULL, GLYPH_INFO_NUM_PIECES, &info);
	    if (code < 0)
				continue;
	    if (count + info.num_pieces > max_count)
				return (int)(XWPSError::RangeCheck);
		}
		info.pieces = &glyphs[count];
		code = glyphInfo(glyphs[i], NULL, GLYPH_INFO_NUM_PIECES | GLYPH_INFO_PIECES, &info);
		if (code >= 0)
	    count += info.num_pieces;
	}
	
	*pcount = count;
  return 0;
}

int XWPSFont::buildChar(XWPSTextEnum * pte, 
	                      XWPSState *pgs, 
		                    ulong chr, 
		                    ulong glyph)
{
	return (this->*(procs.build_char_.build_char))(pte, pgs, chr, glyph);
}

int XWPSFont::charBbox(XWPSRect *pbox, ulong *pglyph, int ch, const XWPSMatrix *pmat)
{
	ulong glyph = encodeChar((ulong)ch, GLYPH_SPACE_INDEX);
  if (glyph == ps_no_glyph)
		return (int)(XWPSError::Undefined);
	
	XWPSGlyphInfo info;
	int code = glyphInfo(glyph, pmat, GLYPH_INFO_BBOX, &info);
  if (code < 0)
		return code;
  
  *pbox = info.bbox;
  if (pglyph)
		*pglyph = glyph;
  return 0;
}

void XWPSFont::computePost(PSPost *post)
{
	int i;
	for (i = 0, post->length = 32 + 2; i <= 255; ++i)
	{
		XWPSString str;
		ulong glyph = encodeChar((ulong)i, GLYPH_SPACE_INDEX);
		int mac_index = macGlyphIndex(i, &str);
		if (mac_index != 0) 
		{
	    post->glyphs[post->count].char_index = i;
	    post->glyphs[post->count].size = (mac_index < 0 ? str.size + 1 : 0);
	    post->glyphs[post->count].glyph_index = glyph - ps_min_cid_glyph;
	    post->count++;
		}
  }
  if (post->count) 
  {
		qsort(post->glyphs, post->count, sizeof(post->glyphs[0]), compare_post_glyphs);
		int i = 0;
		int j = 0;
		for (; i < post->count; ++i) 
		{
	    if (i == 0 ||	post->glyphs[i].glyph_index != post->glyphs[i - 1].glyph_index) 
	    {
				post->length += post->glyphs[i].size;
				post->glyphs[j++] = post->glyphs[i];
	    }
		}
		post->count = j;
		post->glyph_count = post->glyphs[post->count - 1].glyph_index + 1;
  }
  post->length += post->glyph_count * 2;
}

XWPSFont * XWPSFont::copyFont()
{
	XWPSFont * newfont = new XWPSFont;
	copyFontFont(newfont);
	return newfont;
}

void XWPSFont::copyFontFont(XWPSFont * newfont)
{
	newfont->next = next;
	newfont->prev = prev;
	newfont->dir = dir;
	newfont->is_resource = is_resource;
	newfont->id = id;
	newfont->base = base;
	newfont->FontType = FontType;
	newfont->BitmapWidths = BitmapWidths;
	newfont->ExactSize = ExactSize;
	newfont->InBetweenSize = InBetweenSize;
	newfont->TransformedChar = TransformedChar;
	newfont->WMode = WMode;
	newfont->PaintType = PaintType;
	newfont->StrokeWidth = StrokeWidth;
	newfont->procs = procs;
	newfont->key_name.size = key_name.size;
	if (key_name.size > 0)
		memcpy(newfont->key_name.chars, key_name.chars, key_name.size * sizeof(uchar));
	newfont->font_name.size = font_name.size;
	if (font_name.size > 0)
		memcpy(newfont->font_name.chars, font_name.chars, font_name.size * sizeof(uchar));
	newfont->i_ctx_p = i_ctx_p;
}

int XWPSFont::defineFont(XWPSFontDir * pdir)
{
	dir = pdir;
  base = this;
	int code = (this->*(procs.define_font_.define_font))(pdir);
	if (code < 0)
	{
		base = 0;
		return code;
	}
	fontLinkFirst(&pdir->orig_fonts, this);
	return 0;
}

ulong XWPSFont::encodeChar(ulong chr,  PSGlyphSpace glyph_space)
{
	return (this->*(procs.encode_char_.encode_char))(chr, glyph_space);
}

int  XWPSFont::enumerateGlyph(int *pindex, 
	                     PSGlyphSpace glyph_space, 
	                     ulong *pglyph)
{
	return (this->*(procs.enumerate_glyph_.enumerate_glyph))(pindex, glyph_space, pglyph);
}

int XWPSFont::enumerateGlyphNo(int *, 
	                           PSGlyphSpace , 
	                           ulong *)
{
	return (int)(XWPSError::Undefined);
}

void XWPSFont::findCharRange(int *pfirst, int *plast)
{
	ulong notdef = ps_no_glyph;
  int first = 0, last = 255;
  ulong glyph;

  switch (FontType) 
  {
    case ft_encrypted:
    case ft_encrypted2: 
    	{
				int ch;

				for (ch = 0; ch <= 255; ++ch) 
				{
	    		ulong glyph =
					encodeChar(ch,GLYPH_SPACE_INDEX);
	    		XWPSString gnstr;

	    		if (glyph == ps_no_glyph)
						continue;
	    		gnstr.data = (uchar *)glyphName(glyph, &gnstr.size);
	    		if (gnstr.size == 7 && !memcmp(gnstr.data, ".notdef", 7)) 
	    		{
						notdef = glyph;
						break;
	    		}
				}
				break;
    	}
    	
    default:
			;
  }
  while (last >= first && ((glyph = encodeChar(last, GLYPH_SPACE_INDEX))== ps_no_glyph ||
	    		glyph == notdef || glyph == ps_min_cid_glyph))
		--last;
  while (first <= last && ((glyph = encodeChar(first, GLYPH_SPACE_INDEX))== ps_no_glyph ||
	    		glyph == notdef || glyph == ps_min_cid_glyph))
		++first;
  if (first > last)
		last = first;
  *pfirst = first;
  *plast = last;
}

int XWPSFont::fontInfo(const XWPSPoint *pscale, 
	                       int members,
	                       XWPSFontInfo *info)
{
	return (this->*(procs.font_info_.font_info))(pscale, members, info);
}

int XWPSFont::fontInfoDefault(const XWPSPoint *pscale, 
	                       int members,
	                       XWPSFontInfo *info)
{
	int wmode = WMode;
	XWPSPoint scale;
  XWPSMatrix smat;
  XWPSMatrix *pmat=0;
  if (pscale != 0)
  {
  	scale = *pscale;
		smat.makeScaling(scale.x, scale.y);
		pmat = &smat;
  }
  info->members = 0;
  if (members & FONT_INFO_FLAGS)
		info->Flags_returned = 0;
  if (FontType == ft_composite)
		return 0;
		
	if ((members & FONT_INFO_FLAGS) &&	(info->Flags_requested & FONT_IS_FIXED_WIDTH))
	{
		ulong notdef = ps_no_glyph;
		ulong glyph;
		int fixed_width = 0;
		int index, code;
		
		for (index = 0; fixed_width >= 0 && (code = enumerateGlyph(&index, GLYPH_SPACE_NAME, &glyph)) >= 0 && index != 0;)
		{
			XWPSGlyphInfo glyph_info;	    
			XWPSString gnstr;
	    code = glyphInfo(glyph, pmat, (GLYPH_INFO_WIDTH0 << wmode), &glyph_info);
	    if (code < 0)
				return code;
				
			if (notdef == ps_no_glyph)
			{
				gnstr.data = (uchar *)glyphName(glyph, &gnstr.size);
				if (gnstr.size == 7 && !memcmp(gnstr.data, ".notdef", 7))
				{
					notdef = glyph;
		    	info->MissingWidth = glyph_info.width[wmode].x;
		    	info->members |= FONT_INFO_MISSING_WIDTH;
		    }
			}
			
			if (glyph_info.width[wmode].y != 0)
				fixed_width = min_int;
	    else if (fixed_width == 0)
				fixed_width = glyph_info.width[wmode].x;
	    else if (glyph_info.width[wmode].x != fixed_width)
				fixed_width = min_int;
		}
		
		if (code < 0)
	    return code;
	    
	  if (fixed_width > 0) 
	  {
	    info->Flags |= FONT_IS_FIXED_WIDTH;
	    info->members |= FONT_INFO_AVG_WIDTH | FONT_INFO_MAX_WIDTH |	FONT_INFO_MISSING_WIDTH;
	    info->AvgWidth = info->MaxWidth = info->MissingWidth = fixed_width;
		}
		info->Flags_returned |= FONT_IS_FIXED_WIDTH;
	}
	else if (members & FONT_INFO_MISSING_WIDTH)
	{
		ulong glyph;
		int index;
		
		for (index = 0; enumerateGlyph(&index, GLYPH_SPACE_NAME, &glyph) >= 0 && index != 0;)
		{
			if (glyph >= ps_min_cid_glyph) 
			{
				if (glyph != ps_min_cid_glyph)
		    	continue;
	    } 
	    else 
	    {
	    	XWPSString gnstr;
	    	gnstr.data = (uchar *)glyphName(glyph, &gnstr.size);
				if (gnstr.size != 7 || memcmp(gnstr.data, ".notdef", 7))
		    	continue;
	    }
	    
	    {
				XWPSGlyphInfo glyph_info;
				int code = glyphInfo(glyph, pmat, (GLYPH_INFO_WIDTH0 << wmode), &glyph_info);

				if (code < 0)
		    	return code;
				info->MissingWidth = glyph_info.width[wmode].x;
				info->members |= FONT_INFO_MISSING_WIDTH;
				break;
	    }
		}
	}
	
	return 0;
}

int XWPSFont::getLength()
{
	return sizeof(XWPSFont);
}

const char * XWPSFont::getTypeName()
{
	return "font";
}

int XWPSFont::getIndex(XWPSString *pgstr, int count, ulong *pval)
{
	int i;

  if (pgstr->size < count)
		return (int)(XWPSError::RangeCheck);
	
  *pval = 0;
  for (i = 0; i < count; ++i)
		*pval = (*pval << 8) + pgstr->data[i];
  pgstr->data += count;
  pgstr->size -= count;
  return 0;
}

int XWPSFont::glyphInfo(ulong glyph, 
	                        const XWPSMatrix * pmat, 
	                        int members, 
	                        XWPSGlyphInfo *info)
{
	return (this->*(procs.glyph_info_.glyph_info))(glyph, pmat, members, info);
}

int XWPSFont::glyphInfoDefault(ulong glyph, 
	                      const XWPSMatrix * pmat, 
	                      int members, 
	                      XWPSGlyphInfo *info)
{
	XWPSPath  * ppath = new XWPSPath(0);
  int returned = 0;
  int code;
  
  if (ppath == 0)
		return (int)(XWPSError::VMError);
		
  code = ppath->addPoint(0, 0);
  if (code < 0)
		goto out;
		
	code = glyphOutline(glyph, pmat, ppath);
  if (code < 0)
		goto out;
		
	if (members & GLYPH_INFO_WIDTHS)
	{
		int wmode = WMode;
		int wmask = GLYPH_INFO_WIDTH0 << wmode;

		if (members & wmask) 
		{
	    XWPSFixedPoint pt;

	    code = ppath->currentPoint(&pt);
	    if (code < 0)
				goto out;
	    info->width[wmode].x = fixed2float(pt.x);
	    info->width[wmode].y = fixed2float(pt.y);
	    returned |= wmask;
		}
	}
	
	if (members & GLYPH_INFO_BBOX)
	{
		XWPSFixedRect bbox;
		code = ppath->getBbox(&bbox);
		if (code < 0)
	    goto out;
		info->bbox.p.x = fixed2float(bbox.p.x);
		info->bbox.p.y = fixed2float(bbox.p.y);
		info->bbox.q.x = fixed2float(bbox.q.x);
		info->bbox.q.y = fixed2float(bbox.q.y);
		returned |= GLYPH_INFO_BBOX;
	}
	
	if (members & GLYPH_INFO_NUM_PIECES) 
	{
		info->num_pieces = 0;
		returned |= GLYPH_INFO_NUM_PIECES;
  }
  returned |= members & GLYPH_INFO_PIECES;
  
out:
    delete ppath;
    info->members = returned;
    return code;
}

const char * XWPSFont::glyphName(ulong index, uint *plen)
{
	return (this->*(procs.callbacks.glyph_name_.glyph_name))(index, plen);
}

int XWPSFont::glyphOutline(ulong glyph, const XWPSMatrix * pmat, XWPSPath *ppath)
{
	return (this->*(procs.glyph_outline_.glyph_outline))(glyph, pmat, ppath);
}

int XWPSFont::glyphOutlineNo(ulong, const XWPSMatrix *, XWPSPath*)
{
	return (int)(XWPSError::Undefined);
}

ulong XWPSFont::glyphToIndex(ulong glyph)
{
	XWPSRef gref;
  XWPSRef *pcstr;

  if (glyph >= ps_min_cid_glyph)
		return glyph;
		
  i_ctx_p->nameIndexRef(glyph, &gref);
  XWPSFontData *pdata = (XWPSFontData*)(client_data);
  if (pdata->CharStrings.dictFind(i_ctx_p, &gref, &pcstr) > 0 &&
			pcstr->hasType(XWPSRef::Integer)) 
	{
		ulong index_glyph = pcstr->value.intval + ps_min_cid_glyph;

		if (index_glyph >= ps_min_cid_glyph && index_glyph <= ps_max_glyph)
	    return index_glyph;
  }
  return ps_min_cid_glyph;
}

int XWPSFont::initFStack(XWPSTextEnum *pte)
{
	return (this->*(procs.init_fstack_.init_fstack))(pte);
}

int XWPSFont::initFStackDefault(XWPSTextEnum *pte)
{
	pte->fstack.depth = -1;
  return 0;
}

ulong XWPSFont::knownEncode(ulong chr, int encoding_index)
{
	return (this->*(procs.callbacks.known_encode_.known_encode))(chr, encoding_index);
}

void XWPSFont::fontLinkFirst(XWPSFont **pfirst, XWPSFont *elt)
{
	XWPSFont *first = elt->next = *pfirst;

  if (first)
		first->prev = elt;
  elt->prev = 0;
  *pfirst = elt;
}

XWPSCachedFMPair * XWPSFont::lookupFMPair(XWPSState * pgs)
{
	float mxx = pgs->char_tm.xx, mxy = pgs->char_tm.xy, myx = pgs->char_tm.yx,  myy = pgs->char_tm.yy;
	XWPSFont * font = this;
  XWPSCachedFMPair *pair =  dir->fmcache.mdata + dir->fmcache.mnext;
  int count = dir->fmcache.mmax;
  XWPSUid uid;

  if (FontType == ft_composite || PaintType != 0) 
		uid.invalid();
  else 
  {
		uid = ((XWPSFontBase *) this)->UID;
		if (uid.isValid())
	    font = 0;
  }
  
  while (count--) 
  {
		if (pair == dir->fmcache.mdata)
	    pair += dir->fmcache.mmax;
		pair--;
		
		if (font != 0) 
		{
	    if (pair->font != font)
				continue;
		} 
		else 
		{
	    if (!pair->UID.equal(&uid) ||	pair->FontType != FontType)
				continue;
		}
		if (pair->mxx == mxx && pair->mxy == mxy &&
	    	pair->myx == myx && pair->myy == myy) 
	  {
	    if (pair->font == 0) 
				pair->font = this;
	    return pair;
		}
  }
  return dir->addFMPair(this, &uid, pgs);
}

XWPSCachedChar * XWPSFont::lookupCachedChar(XWPSCachedFMPair * pair,
		                                ulong glyph, 
		                                int wmode, 
		                                int alt_depth)
{
	uint chi = pair->charsHeadIndex(glyph);
  XWPSCachedChar *cc;
  while ((cc = dir->ccache.table[chi & dir->ccache.table_mask]) != 0)
  {
  	if (cc->code == glyph && cc->pair == pair &&
	    cc->wmode == wmode && (cc->depth == 1 || cc->depth == alt_depth))
	  {
	  	return cc;
	  }
	  chi++;
  }
  return 0;
}

int XWPSFont::macGlyphIndex(int ch, XWPSString *pstr)
{
	ulong glyph = encodeChar((ulong)ch, GLYPH_SPACE_NAME);
  if (glyph == ps_no_glyph)
		return 0;
		
	pstr->data = (uchar *)glyphName(glyph, &pstr->size);
  if (glyph < ps_min_cid_glyph)
  {
  	ulong mac_char;
		ulong mac_glyph;
		XWPSString mstr;
		if (ch >= 32 && ch <= 126)
	    mac_char = ch - 29;
		else if (ch >= 128 && ch <= 255)
	    mac_char = ch - 30;
		else
	    return -1;
		mac_glyph = knownEncode(mac_char, ENCODING_INDEX_MACGLYPH);
		if (mac_glyph == ps_no_glyph)
	    return -1;
		mstr.data =(uchar *)glyphName(mac_glyph, &mstr.size);
		if (!bytes_compare(pstr->data, pstr->size, mstr.data, mstr.size))
	    return (int)mac_char;
  }
  
  return -1;
}

int XWPSFont::makeFont(XWPSFontDir * pdir,
	             const XWPSMatrix * pmat, 
	             XWPSFont ** ppfont)
{
	int code;
  XWPSFont *prevA = 0;
  XWPSFont *pf_out = pdir->scaled_fonts;
  XWPSMatrix newmat;
  bool can_cache;
  
  if ((code = newmat.matrixMultiply(&FontMatrix, (XWPSMatrix*)pmat)) < 0)
		return code;
		
	if (FontType != ft_composite)
  {
  	for (; pf_out != 0; prevA = pf_out, pf_out = pf_out->next)
  		if (pf_out->FontType == FontType &&
					pf_out->base == base &&
					pf_out->FontMatrix.xx == newmat.xx &&
					pf_out->FontMatrix.xy == newmat.xy &&
					pf_out->FontMatrix.yx == newmat.yx &&
					pf_out->FontMatrix.yy == newmat.yy &&
					pf_out->FontMatrix.tx == newmat.tx &&
					pf_out->FontMatrix.ty == newmat.ty)
			{
				*ppfont = pf_out;
				return 0;
			}
			
		can_cache = true;
  }
  else
  	can_cache = false;
  	
  pf_out = copyFont();
  pf_out->FontMatrix = newmat;
  pf_out->dir = pdir;
  *ppfont = pf_out;
  
	code = (this->*(procs.make_font_.make_font))(pdir, pmat, ppfont);
	if (code < 0)
		return code;
	if (can_cache)
	{
		if (pdir->ssize >= pdir->smax && prev != 0)
		{
			if (prevA->prev != 0)
				prevA->prev->next = 0;
	    else
				pdir->scaled_fonts = 0;
			pdir->ssize--;
	    prevA->prev = 0;	    
		}
		
		pdir->ssize++;
		fontLinkFirst(&pdir->scaled_fonts, pf_out);
	}
	else
		pf_out->next = pf_out->prev = 0;
		
	return 1;
}

int XWPSFont::makeFontBase(XWPSFontDir * ,
	             const XWPSMatrix * , 
	             XWPSFont ** ppfont)
{
	XWPSFontBase * pbfont = (XWPSFontBase *)*ppfont;

  if (pbfont->UID.isXUID()) 
  {
		uint xsize = pbfont->UID.XUIDSize();
		long *xvalues = new long[xsize];
		memcpy(xvalues, pbfont->UID.XUIDValues(), xsize * sizeof(long));
    if (pbfont->UID.xvalues)
    	delete [] pbfont->UID.xvalues;
		pbfont->UID.xvalues = xvalues;
  }
  return 0;
}

int XWPSFont::nextCharGlyph(XWPSTextEnum *pte, ulong *pchr, ulong *pglyph)
{
	return (pte->*(procs.next_char_glyph_.next_char_glyph))(pchr, pglyph);
}

void XWPSFont::purgeFont()
{
	XWPSFontDir *pdir = dir;
  XWPSFont *pf;
  
  XWPSFont *prevA = prev;
  XWPSFont *nextA = next;

  if (nextA != 0)
		nextA->prev = prevA, next = 0;
  if (prevA != 0)
		prevA->next = nextA, prev = 0;
  else if (pdir->orig_fonts == this)
		pdir->orig_fonts = nextA;
  else if (pdir->scaled_fonts == this)
		pdir->scaled_fonts = nextA;
		
  for (pf = pdir->scaled_fonts; pf != 0;) 
  {
		if (pf->base == this) 
		{
	    pf->purgeFont();
	    pf = pdir->scaled_fonts;
		} 
		else
	    pf = pf->next;
  }
  pdir->purgeFontFromCharCaches(this);
}

int XWPSFont::sameFont(const XWPSFont *ofont, int mask)
{
	return (this->*(procs.same_font_.same_font))(ofont, mask);
}

int XWPSFont::sameFontBase(const XWPSFont *ofont, int mask)
{
	XWPSFont *  font = this;
	int same = sameFontDefault(ofont, mask);

  if (!same) 
  {
		const XWPSFontBase *const bfont = (const XWPSFontBase *)font;
		const XWPSFontBase *const obfont = (const XWPSFontBase *)ofont;

		if (mask & FONT_SAME_ENCODING) 
		{
	    if (bfont->encoding_index != ENCODING_INDEX_UNKNOWN ||
	    	 obfont->encoding_index != ENCODING_INDEX_UNKNOWN) 
	    {
				if (bfont->encoding_index == obfont->encoding_index)
		    same |= FONT_SAME_ENCODING;
	    }
		}
  }
  return same;
}

int XWPSFont::sameFontDefault(const XWPSFont *ofont, int mask)
{
	XWPSFont * font = this;
	while (font->base != font)
		font = font->base;
  while (ofont->base != ofont)
		ofont = ofont->base;
  if (ofont == font)
		return mask;
  
  return 0;
}

uint XWPSFont::sizeCmap(uint first_code, 
	                      int num_glyphs, 
	                      ulong max_glyph,
	  						        int options)
{
	XWPSStream poss;
	poss.writePositionOnly();
	writeCmap(&poss, first_code, num_glyphs, max_glyph, options, 0);
  return poss.tell();
}

uint XWPSFont::sizeName(const XWPSString *font_nameA)
{
	return sizeof(name_initial) + font_nameA->size;
}

int XWPSFont::subsetGlyphs(ulong * glyphs, const uchar * used)
{
	int n = 0;
  for (int i = 0; i < 256; ++i)
		if (used[i >> 3] & (1 << (i & 7))) 
		{
	    ulong glyph = encodeChar((ulong)i, GLYPH_SPACE_INDEX);
	   	if (glyph != ps_no_glyph)
				glyphs[n++] = glyph;
		}
  return n;
}

void XWPSFont::updateOS2(ttf_OS_2_t *pos2, uint first_glyph, int num_glyphs)
{
	put_u16(pos2->usFirstCharIndex, first_glyph);
  put_u16(pos2->usLastCharIndex, first_glyph + num_glyphs - 1);
}

void XWPSFont::writeCmap(XWPSStream *s, 
	                       uint first_code, 
	                       int num_glyphs,
	                       ulong max_glyph, 
	                       int options, 
	                       uint cmap_length)
{
	uchar cmap_sub[sizeof(cmap_sub_initial)];
  uchar entries[256 * 2];
  int first_entry = 0, end_entry = num_glyphs;
  bool can_use_trimmed = !(options & WRITE_TRUETYPE_NO_TRIMMED_TABLE);
  uint merge = 0;
  uint num_entries;
  
  for (int i = 0; i < num_glyphs; ++i)
  {
  	ulong glyph = encodeChar((ulong)i, GLYPH_SPACE_INDEX);
		uint glyph_index;

		if (glyph == ps_no_glyph || glyph < ps_min_cid_glyph || glyph > max_glyph )
	    glyph = ps_min_cid_glyph;
		glyph_index = (uint)(glyph - ps_min_cid_glyph);
		merge |= glyph_index;
		put_u16(entries + 2 * i, glyph_index);
  }
  
  while (end_entry > first_entry && !U16(entries + 2 * end_entry - 2))
		--end_entry;
  while (first_entry < end_entry && !U16(entries + 2 * first_entry))
		++first_entry;
  num_entries = end_entry - first_entry;
  
  if (merge == (uchar)merge && (num_entries <= 127 || !can_use_trimmed))
  {
  	memset(entries + 2 * num_glyphs, 0, sizeof(entries) - 2 * num_glyphs);
		s->write(cmap_initial_0, sizeof(cmap_initial_0));
		for (int i = 0; i <= 0xff; ++i)
	    s->putc((uchar)entries[2 * i + 1]);
  }
  else if (can_use_trimmed)
  {
  	uchar cmap_data[sizeof(cmap_initial_6)];

		memcpy(cmap_data, cmap_initial_6, sizeof(cmap_initial_6));
		put_u16(cmap_data + 18,	U16(cmap_data + 18) + num_entries * 2);  /* offset */
		put_u16(cmap_data + 22,	U16(cmap_data + 22) + num_entries * 2);  /* length */
		put_u16(cmap_data + 26, first_code + first_entry);
		put_u16(cmap_data + 28, num_entries);
		s->write(cmap_data, sizeof(cmap_data));
		s->write(entries + first_entry * 2, num_entries * 2);
  }
  else
  	s->write(cmap_initial_4, sizeof(cmap_initial_4));
  	
  memcpy(cmap_sub, cmap_sub_initial, sizeof(cmap_sub_initial));
  put_u16(cmap_sub + 2, U16(cmap_sub + 2) + num_entries * 2); /* length */
  put_u16(cmap_sub + 14, first_code + end_entry - 1); /* endCount[0] */
  put_u16(cmap_sub + 20, first_code + first_entry); /* startCount[0] */
  s->write(cmap_sub, sizeof(cmap_sub));
  s->write(entries + first_entry * 2, num_entries * 2);
  s->putPad(cmap_length);
}

void XWPSFont::writeName(XWPSStream *s, const XWPSString *font_nameA)
{
	uchar name_bytes[sizeof(name_initial)];

  memcpy(name_bytes, name_initial, sizeof(name_initial));
  put_u16(name_bytes + 14, font_nameA->size);
  s->write(name_bytes, sizeof(name_bytes));
  s->write(font_nameA->data, font_nameA->size);
  s->putPad(size_name(font_nameA));
}

void XWPSFont::writeOS2(XWPSStream *s, uint first_glyph, int num_glyphs)
{
	ttf_OS_2_t os2;
	memset(&os2, 0, sizeof(os2));
  put_u16(os2.version, 1);
  put_u16(os2.usWeightClass, 400); /* Normal */
  put_u16(os2.usWidthClass, 5); /* Normal */
  updateOS2(&os2, first_glyph, num_glyphs);
  if (first_glyph >= 0xf000)
		os2.ulCodePageRanges[3] = 1; /* bit 31, symbolic */
  s->write(&os2, sizeof(os2));
  s->putPad(sizeof(os2));
}

void XWPSFont::writePost(XWPSStream *s, PSPost *post)
{
	uchar post_initial[32 + 2];
  uint name_index = 258;
  uint glyph_index = 0;

  memset(post_initial, 0, 32);
  put_u32(post_initial, 0x00020000);
  put_u16(post_initial + 32, post->glyph_count);
  s->write(post_initial, sizeof(post_initial));

  for (int i = 0; i < post->count; ++i) 
  {
		XWPSString str;
		int ch = post->glyphs[i].char_index;
		int mac_index = macGlyphIndex(ch, &str);

		for (; glyph_index < post->glyphs[i].glyph_index; ++glyph_index) 
			s->putUShort(0);glyph_index++;	
		if (mac_index >= 0)
	    s->putUShort(mac_index);
		else 
		{
	    s->putUShort(name_index);
	    name_index++;
		}
  }

  for (int i = 0; i < post->count; ++i) 
  {
		XWPSString str;
		int ch = post->glyphs[i].char_index;
		int mac_index = macGlyphIndex(ch, &str);

		if (mac_index < 0) 
		{
	    s->pputc(str.size);
	    s->write(str.data, str.size);
		}
  }
  s->putPad(post->length);
}

int XWPSFont::zbaseMakeFont(XWPSFontDir * pdir, 
		               const XWPSMatrix * pmat, 
		               XWPSFont ** ppfont)
{
	int code = makeFontBase(pdir, pmat, ppfont);

  if (code < 0)
		return code;
  return zdefaultMakeFont(pdir, pmat, ppfont);
}

int XWPSFont::zdefaultMakeFont(XWPSFontDir * , 
		                   const XWPSMatrix * pmat, 
		                   XWPSFont ** ppfont)
{
	XWPSFont *newfont = *ppfont;
	XWPSFontData * pdata = (XWPSFontData*)(client_data);
  XWPSRef *fp = &pdata->dict;
  XWPSRef newdict, newmat, scalemat;
  uint dlen = fp->dictMaxLength();
  uint mlen = fp->dictLength() + 3;
  int code;

  if (dlen < mlen)
		dlen = mlen;
		
	if (newfont->client_data)
	{
		XWPSFontData * fd = (XWPSFontData*)(newfont->client_data);
		if (!fd->incRef())
			delete fd;
		newfont->client_data = 0;
	}
	pdata = new XWPSFontData;
	newfont->client_data = pdata;
	newdict.dictAlloc(i_ctx_p->imemory()->space, dlen);
	code = i_ctx_p->dictCopy(fp, &newdict);
	newmat.makeArray(PS_A_ALL | i_ctx_p->currentSpace(), 12);
	
  if (code < 0)
		return code;
		
	newmat.value.refs->arr->incRef();
  scalemat.makeArray(PS_A_ALL | i_ctx_p->currentSpace(),6, newmat.getArray());
  scalemat.value.refs->arr = newmat.value.refs->arr;
  
  {
		XWPSMatrix scale, prev_scale;
		XWPSRef *ppsm;

		if (!(fp->dictFindString(i_ctx_p, "ScaleMatrix", &ppsm) > 0 &&
	      ppsm->readMatrix(i_ctx_p, &prev_scale) >= 0 &&
	      scale.matrixMultiply((XWPSMatrix*)pmat, &prev_scale) >= 0))
	    scale = *((XWPSMatrix*)pmat);
	  scalemat.writeMatrixIn(&scale);
  }
  
  scalemat.clearAttrs(PS_A_WRITE);
  newmat.setSize(6);
  newmat.writeMatrixIn(&newfont->FontMatrix);
  newmat.clearAttrs(PS_A_WRITE);
  XWPSFontData * odict = (XWPSFontData*)(base->client_data);
  if ((code = newdict.dictPutString(i_ctx_p, "FontMatrix", &newmat, NULL)) < 0 ||
			(code = newdict.dictPutString(i_ctx_p, "OrigFont", &odict->dict, NULL)) < 0 ||
			(code = newdict.dictPutString(i_ctx_p, "ScaleMatrix", &scalemat, NULL)) < 0 ||
			(code = i_ctx_p->addFID(&newdict, newfont, false)) < 0)
		return code;
  
  pdata->copy((XWPSFontData*)client_data);
  pdata->dict.assign(&newdict);
  newdict.dictAccessRef()->clearAttrs(PS_A_WRITE);
  return 0;
}

ulong XWPSFont::zfontEncodeChar(ulong chr, PSGlyphSpace)
{
	XWPSFontData * fd = (XWPSFontData*)(client_data);
	XWPSRef *pencoding = &fd->Encoding;
  ulong index = chr;
  XWPSRef cname;
  int code = pencoding->arrayGet(i_ctx_p, (long)index, &cname);

  if (code < 0 || !cname.hasType(XWPSRef::Name))
		return ps_no_glyph;
  return i_ctx_p->nameIndex(&cname);
}

const char * XWPSFont::zfontGlyphName(ulong index, uint * plen)
{
	XWPSRef nref, sref;

  if (index >= ps_min_cid_glyph) 
  {	
		char cid_name[sizeof(ulong) * 3 + 1];
		int code;

		sprintf(cid_name, "%lu", (ulong) index);
		code = i_ctx_p->nameRef((const uchar *)cid_name, strlen(cid_name),	&nref, 1);
		if (code < 0)
	    return 0;	
  } 
  else
		i_ctx_p->nameIndexRef(index, &nref);
  i_ctx_p->nameStringRef(&nref, &sref);
  *plen = sref.size();
  uchar * p = sref.getBytes();
  return (const char *)p;
}

ulong XWPSFont::zfontKnownEncode(ulong chr, int encoding_index)
{
	ulong index = chr;
  XWPSRef cname;
  int code;

  if (encoding_index < 0)
		return ps_no_glyph;
  code = (i_ctx_p->registeredEncoding(encoding_index))->arrayGet(i_ctx_p,(long)index, &cname);
  if (code < 0 || !cname.hasType(XWPSRef::Name))
		return ps_no_glyph;
  return i_ctx_p->nameIndex(&cname);
}

int psf_type1_glyph_data(XWPSFontBase *pbfont, ulong glyph, XWPSString *pstr, XWPSFontType1 **ppfont)
{
	XWPSFontType1 * pfont = (XWPSFontType1 *)pbfont;
  *ppfont = pfont;
  return pfont->glyphData(glyph, pstr);
}

XWPSFontBase::XWPSFontBase()
	:XWPSFont()
{
	encoding_index = ENCODING_INDEX_UNKNOWN;
	nearest_encoding_index = ENCODING_INDEX_UNKNOWN;
}

XWPSFontBase::~XWPSFontBase()
{
}

int XWPSFontBase::checkOutlineGlyphs(XWPSPSFGlyphEnum *ppge, glyph_data_proc_t glyph_data)
{
	uint members = GLYPH_INFO_WIDTH0 << WMode;
  ulong glyph;
  int code;
  while ((code = ppge->enumerateGlyphsNext(&glyph)) != 1)
  {
  	if (code < 0)
	    return code;
	    
	  XWPSString gdata;
		XWPSFontType1 *ignore_font;
		code = glyph_data(this, glyph, &gdata, &ignore_font);
		if (code < 0) 
		{
	    if (code == XWPSError::Undefined)
				continue;
	    return code;
		}
		
		XWPSGlyphInfo info;
		code = glyphInfo(glyph, NULL, members, &info);
		if (code < 0)
	    return code;
  }
  
  return 0;
}

bool XWPSFontBase::considerFontSymbolic()
{
	return (FontType == ft_composite || encoding_index != ENCODING_INDEX_STANDARD);
}

XWPSFont * XWPSFontBase::copyFont()
{
	XWPSFontBase * newfont = new XWPSFontBase;
	copyFontBase(newfont);
	return newfont;
}

void XWPSFontBase::copyFontBase(XWPSFontBase * newfont)
{
	copyFontFont(newfont);
	newfont->FontBBox = FontBBox;
	newfont->UID = UID;
	newfont->encoding_index = encoding_index;
	newfont->nearest_encoding_index = nearest_encoding_index;
}

bool XWPSFontBase::encodingHasGlyph(ulong font_glyph, PSEncodingIndex index)
{
	int ch;
  ulong glyph;

  for (ch = 0; (glyph = knownEncode(ch, index)) != ps_no_glyph; ++ch)
		if (glyph == font_glyph)
	    return true;
  return false;
}

int XWPSFontBase::getLength()
{
	return sizeof(XWPSFontBase);
}

const char * XWPSFontBase::getTypeName()
{
	return "fontbase";
}

int XWPSFontBase::getOutlineGlyphs(XWPSPSFOutlineGlyphs *pglyphs, 
		       										     ulong *orig_subset_glyphs, 
		       										     uint orig_subset_size,
		                               glyph_data_proc_t glyph_data)
{
	ulong notdef = ps_no_glyph;
  ulong *subset_glyphs = orig_subset_glyphs;
  uint subset_size = orig_subset_size;
  if (subset_glyphs) 
  {
		if (subset_size > (256 * 3 + 1))
	    return (int)(XWPSError::LimitCheck);
		memcpy(pglyphs->subset_data, orig_subset_glyphs,  sizeof(ulong) * subset_size);
		subset_glyphs = pglyphs->subset_data;
  }
  
  {
  	XWPSPSFGlyphEnum genum;
  	genum.listBegin(this, subset_glyphs,  (subset_glyphs ? subset_size : 0),  GLYPH_SPACE_NAME);
		int code = checkOutlineGlyphs(&genum, glyph_data);
		if (code < 0)
	    return code;
  }
  
  {
  	XWPSPSFGlyphEnum genum;
  	genum.listBegin(this, NULL, 0,  GLYPH_SPACE_NAME);
  	ulong glyph;
		int code;
		while ((code = genum.enumerateGlyphsNext(&glyph)) != 1) 
		{
	    uint namelen;
	    const char *namestr = glyphName(glyph, &namelen);
	    if (namestr && namelen == 7 && !memcmp(namestr, ".notdef", 7)) 
	    {
				notdef = glyph;
				break;
	    }
		}
  }
  
  if (subset_glyphs)
  {
  	int code = addSubsetPieces(subset_glyphs, &subset_size, (256 * 3 + 1) - 1, 2);
		if (code < 0)
	    return code;
	    
	  if (notdef == ps_no_glyph)
	    return (int)(XWPSError::RangeCheck);
	    	
	  subset_glyphs[subset_size++] = notdef;
		subset_size = psf_sort_glyphs(subset_glyphs, subset_size);
  }
  
  pglyphs->notdef = notdef;
  pglyphs->subset_glyphs = subset_glyphs;
  pglyphs->subset_size = subset_size;
  return 0;
}

void XWPSFontBase::initSimpleFont(const double bbox[4], const XWPSUid *puid)
{
	FontBBox.p.x = bbox[0];
  FontBBox.p.y = bbox[1];
  FontBBox.q.x = bbox[2];
  FontBBox.q.y = bbox[3];
  UID = *puid;
}

bool XWPSFontBase::isSymbolic()
{
	if (FontType == ft_composite)
		return true;
  
  switch (nearest_encoding_index) 
  {
    case ENCODING_INDEX_STANDARD:
    case ENCODING_INDEX_ISOLATIN1:
    case ENCODING_INDEX_WINANSI:
    case ENCODING_INDEX_MACROMAN:
			return false;
			
    default:
			return true;
  }
}

void XWPSPixelScale::set(int log2)
{
	unit = (half = fixed_half << (log2_unit = log2)) << 1;
}

void XWPSPointScale::copy(const XWPSPointScale * other)
{
	x = other->x;
	y = other->y;
}

void XWPSPointScale::set(const XWPSLog2ScalePoint * pscale)
{
	x.set(pscale->x);
	y.set(pscale->y);
}

XWPSStemSnapTable::XWPSStemSnapTable()
{
	memset(data, 0, max_snaps * sizeof(long));
}

void XWPSStemSnapTable::copy(const XWPSStemSnapTable * other)
{
	count = other->count;
	memcpy(data, other->data, max_snaps * sizeof(long));
}

static bool USE_HINTS = true;

XWPSFontHints::XWPSFontHints()
{
	axes_swapped = false;
	x_inverted = false;
	y_inverted = false;
	use_x_hints = false;
	use_y_hints = false;
	blue_fuzz = 0; 
	blue_shift = 0;
	suppress_overshoot = false;
	a_zone_count = 0;
}

void XWPSFontHints::computeFontHints(const XWPSMatrixFixed * pmat,
                        const XWPSLog2ScalePoint * plog2_scale, 
                        const XWPSFontType1 * pdata)
{
	XWPSAlignmentZone *zp = &a_zones[0];

  reset(plog2_scale);
  if (is_fzero(pmat->xy))
		y_inverted = is_fneg(pmat->yy), use_y_hints = USE_HINTS;
  else if (is_fzero(pmat->xx))
		y_inverted = is_fneg(pmat->xy), axes_swapped = true, use_y_hints = USE_HINTS;
  if (is_fzero(pmat->yx))
		x_inverted = is_fneg(pmat->xx), use_x_hints = USE_HINTS;
  else if (is_fzero(pmat->yy))
		x_inverted = is_fneg(pmat->yx), axes_swapped = true, use_x_hints = USE_HINTS;

  if (use_x_hints) 
  {
		computeSnaps(pmat, (const PSAStemTable *)&pdata->StdHW, &snap_h, 0, axes_swapped, "h");
		computeSnaps(pmat, (const PSAStemTable *)&pdata->StemSnapH, &snap_h, 0, axes_swapped, "h");
  }
  if (use_y_hints) 
  {
		XWPSFixedPoint vw;
		long *vp = (axes_swapped ? &vw.x : &vw.y);
		XWPSPixelScale *psp =	(axes_swapped ? &scale.x : &scale.y);
		if (vw.distanceTransform2fixed((XWPSMatrixFixed*)pmat, 0.0,	(float)pdata->BlueFuzz) < 0)
	    vw.x = vw.y = fixed_0;
		blue_fuzz = qAbs(*vp);
		if (vw.distanceTransform2fixed((XWPSMatrixFixed*)pmat, 0.0, 1.0) < 0)
	    vw.x = vw.y = fixed_0;
		suppress_overshoot = fixed2float(qAbs(*vp) >> psp->log2_unit) < pdata->BlueScale;
		if (vw.distanceTransform2fixed((XWPSMatrixFixed*)pmat, 0.0, pdata->BlueShift) < 0)
	    vw.x = vw.y = fixed_0;
		blue_shift = qAbs(*vp);
		if (blue_shift > psp->half)
	    blue_shift = psp->half;
		zp = computeZones(pmat, (const PSAZoneTable *)&pdata->BlueValues,
			   (const PSAZoneTable *)&pdata->FamilyBlues, zp, 1);
		zp = computeZones(pmat,  (const PSAZoneTable *)&pdata->OtherBlues,
			   (const PSAZoneTable *)&pdata->FamilyOtherBlues,  zp, max_OtherBlues);
		computeSnaps(pmat, (const PSAStemTable *)&pdata->StdVW, &snap_v, 1, !axes_swapped, "v");
		computeSnaps(pmat, (const PSAStemTable *)&pdata->StemSnapV, &snap_v, 1, !axes_swapped, "v");
  }
  a_zone_count = zp - &a_zones[0];
}

void XWPSFontHints::computeSnaps(const XWPSMatrixFixed * pmat, 
	                  const PSAStemTable * pst,
	                  XWPSStemSnapTable * psst, 
	                  int from_y, 
	                  int to_y, 
	                  const char *)
{
	XWPSFixedPoint wxy;
  long *wp = (to_y ? &wxy.y : &wxy.x);
  int i;
  int j = psst->count;

  for (i = 0; i < pst->count; i++) 
  {
		float w = pst->values[i];
		int code =(from_y ?	 wxy.distanceTransform2fixed((XWPSMatrixFixed*)pmat, 0.0, w) :wxy.distanceTransform2fixed((XWPSMatrixFixed*)pmat, w, 0.0));

		if (code < 0)
	    continue;
		psst->data[j] = qAbs(*wp);
		j++;
  }
  psst->count = j;
}

XWPSAlignmentZone * XWPSFontHints::computeZones(const XWPSMatrixFixed * pmat, 
	                                 const PSAZoneTable * blues, 
	                                 const PSAZoneTable * family_blues,
	                                 XWPSAlignmentZone * zp, 
	                                 int bottom_count)
{
	int i;
  long fuzz = blue_fuzz;
  int inverted = (axes_swapped ? x_inverted : y_inverted);

  for (i = 0; i < blues->count; i += 2) 
  {
		const float *vp = &blues->values[i];

		zp->is_top_zone = i >> 1 >= bottom_count;
		if (transformZone(pmat, vp, zp) < 0)
	    continue;
	
		if (i < family_blues->count) 
		{
	    XWPSAlignmentZone fz;
	    const float *fvp = &family_blues->values[i];
	    long unit = (axes_swapped ?  scale.x.unit : scale.y.unit);
	    long diff;

	    if (transformZone(pmat, fvp, &fz) < 0)
				continue;
	    diff = (zp->v1 - zp->v0) - (fz.v1 - fz.v0);
	    if (diff > -unit && diff < unit)
				zp->v0 = fz.v0, zp->v1 = fz.v1;
		}
		
		if ((inverted ? zp->is_top_zone : !zp->is_top_zone))
	    zp->flat = zp->v1, zp->v0 -= fuzz;
		else
	    zp->flat = zp->v0, zp->v1 += fuzz;
		zp++;
  }
  return zp;
}

void XWPSFontHints::copy(const XWPSFontHints * other)
{
	axes_swapped = other->axes_swapped;
	x_inverted = other->x_inverted;
	y_inverted = other->y_inverted;
	use_x_hints = other->use_x_hints;
	use_y_hints = other->use_y_hints;
	scale.copy(&other->scale);
	snap_h.copy(&other->snap_h);
	snap_v.copy(&other->snap_v);
	blue_fuzz = other->blue_fuzz;
	blue_shift = other->blue_shift;
	suppress_overshoot = other->suppress_overshoot;
	a_zone_count = other->a_zone_count;
	a_zone_count = other->a_zone_count;
	for (int i = 0; i < a_zone_count; i++)
		a_zones[i] = other->a_zones[i];
}

void XWPSFontHints::reset(const XWPSLog2ScalePoint * plog2_scale)
{
	scale.x.set(plog2_scale->x);
  scale.y.set(plog2_scale->y);
  axes_swapped = x_inverted = y_inverted = false;
  use_x_hints = use_y_hints = false;
  snap_h.count = snap_v.count = 0;
  a_zone_count = 0;
}

int XWPSFontHints::transformZone(const XWPSMatrixFixed * pmat, 
	       						const float *vp, 
	       						XWPSAlignmentZone * zp)
{
	XWPSFixedPoint p0, p1;
  long v0, v1;
  int code;

  if ((code = p0.transform2fixed((XWPSMatrixFixed*)pmat, 0.0, vp[0])) < 0 ||
			(code = p1.transform2fixed((XWPSMatrixFixed*)pmat, 0.0, vp[1])) < 0)
		return code;
    
  if (axes_swapped)
		v0 = p0.x, v1 = p1.x;
  else
		v0 = p0.y, v1 = p1.y;
  if (v0 <= v1)
		zp->v0 = v0, zp->v1 = v1;
  else
		zp->v0 = v1, zp->v1 = v0;
  return 0;
}

XWPSStemHintTable::XWPSStemHintTable()
	:count(0),
	 current(0),
	 replaced_count(0)
{
}

void XWPSStemHintTable::copy(const XWPSStemHintTable * other)
{
	count = other->count;
	current = other->current;
	replaced_count = other->replaced_count;
	for (int i = 0; i < max_stems; i++)
		data[i] = other->data[i];
}

void XWPSStemHintTable::enableHints(const uchar * mask)
{
	XWPSStemHint *table = &data[0];
  XWPSStemHint *ph = table + current;

  for (ph = &table[count]; --ph >= table;) 
		ph->active = (mask[ph->index >> 3] & (0x80 >> (ph->index & 7))) != 0;
}

void XWPSStemHintTable::saveReplacedHints()
{
	int rep_count = qMin(replaced_count + count, max_stems);

  memmove(&data[max_stems - rep_count], &data[0], count * sizeof(data[0]));
  replaced_count = rep_count;
  count = current = 0;
}

void XWPSStemHintTable::storeStemDeltas(XWPSStemHint * psh,
		                   XWPSPixelScale * psp, 
		                   long v, 
		                   long dv, 
		                   long adj_dv)
{
	XWPSStemHint *psh0 = 0;
  XWPSStemHint *psh1 = 0;
  int i;
  
  for (i = max_stems - replaced_count; i < max_stems; ++i) 
  {
		XWPSStemHint *ph = &data[i];

		if (ph == psh)
	    continue;
		if (ph->v0 == psh->v0)
	    psh0 = ph;
		if (ph->v1 == psh->v1)
	    psh1 = ph;
  }
  
  for (i = 0; i < count; ++i) 
  {
		XWPSStemHint *ph = &data[i];

		if (ph == psh)
	    continue;
		if (ph->v0 == psh->v0)
	    psh0 = ph;
		if (ph->v1 == psh->v1)
	    psh1 = ph;
  }
  
  if (psh0 != 0) 
  {
		psh->dv0 = psh0->dv0;
		if (psh1 != 0) 
	    psh->dv1 = psh1->dv1;
		else 
	    psh->dv1 = psh->dv0 + adj_dv - dv;
  } 
  else if (psh1 != 0) 
  {
		psh->dv1 = psh1->dv1;
		psh->dv0 = psh->dv1 + adj_dv - dv;
  } 
  else 
  {	
		long diff2_dv = arith_rshift_1(adj_dv - dv);
		long edge = v - diff2_dv;
		long diff_v = psp->scaledRounded(edge) - edge;

		psh->dv0 = diff_v - diff2_dv;
		psh->dv1 = diff_v + diff2_dv;
  }
}

XWPSIPState::XWPSIPState()
	:ip(0),
	 dstate(0)	 
{
}

XWPSType1State::XWPSType1State()
{
	pfont = 0;
	pis = 0;
	path = 0;
	charpath_flag = false;
	paint_type = 0;
	callback_data = 0;
	flatness = 0;
	memset(ostack, 0, ostack_size*sizeof(long));
	os_count = 0;
	ips_count = 0;
	init_done = 0;
	sb_set = false;
	width_set = false;
	have_hintmask = false;
	num_hints = 0;
	seac_accent = 0;
	save_asb = 0;
	asb_diff = 0;
	flex_path_state_flags = 0;
	flex_count = 0;
	ignore_pops = 0;
	dotsection_flag = 0;
	vstem3_set = false;
	hints_initial = 0;
	hint_next = 0;
	hints_pending = 0;
	memset(transient_array, 0, 32 * sizeof(long));
}

XWPSType1State::~XWPSType1State()
{
}

void XWPSType1State::applyHintsAt(int hints, XWPSFixedPoint * ppt, XWPSFixedPoint * pdiff)
{
	long px = ppt->x, py = ppt->y;

  if ((hints & HINT_VERT) != 0 &&	(vstem_hints.count & dotsection_flag) != 0)
		applyVStemHints((hints & HINT_VERT_UPPER) - (hints & HINT_VERT_LOWER), ppt);
  if ((hints & HINT_HORZ) != 0 && (hstem_hints.count & dotsection_flag) != 0)
		applyHStemHints((hints & HINT_HORZ_UPPER) - (hints & HINT_HORZ_LOWER), ppt);
  if (pdiff != NULL)
		pdiff->x = ppt->x - px,  pdiff->y = ppt->y - py;
}

void XWPSType1State::applyHStemHints(int dx, XWPSFixedPoint * ppt)
{
	long *pv = (fh.axes_swapped ? &ppt->x : &ppt->y);
  XWPSStemHint *ph = searchHints(&hstem_hints, *pv);

  if (ph != 0) 
		*pv += (dx > 0 ? ph->dv1 : ph->dv0);
}

void XWPSType1State::applyPathHints(bool closing)
{
	type1ApplyPathHints(closing, path);
}

void XWPSType1State::applyVStemHints(int dy, XWPSFixedPoint * ppt)
{
	long *pv = (fh.axes_swapped ? &ppt->y : &ppt->x);
  XWPSStemHint *ph = searchHints(&vstem_hints, *pv);

  if (ph != 0) 
		*pv += (dy > 0 ? ph->dv1 : ph->dv0);
}

void XWPSType1State::applyWrappedHints(XWPSSubpath * psub, 
	                       XWPSSegment * pseg,
		                     int hints, 
		                     XWPSFixedPoint * pdiff)
{
	long ctemp;
  bool closed =	(pseg->type == s_line_close || 
                 ((ctemp = pseg->pt.x - psub->pt.x,   qAbs(ctemp) < float2fixed(0.1)) && 
                 (ctemp = pseg->pt.y - psub->pt.y, qAbs(ctemp) < float2fixed(0.1))));
  XWPSSegment * pfirst = psub->next;
  int hints_first = hints_initial;

  if (closed) 
  {
		int do_x, do_y;
		XWPSFixedPoint diff2;

		if (fh.axes_swapped)
	    do_x = HINT_HORZ, do_y = HINT_VERT;
		else
	    do_x = HINT_VERT, do_y = HINT_HORZ;
		{
	    int hints_end = hints_first & ~hints;

	    diff2.x =	(hints_end & do_x ?	 psub->pt.x - unmoved_start.x : 0);
	    diff2.y =	(hints_end & do_y ?	 psub->pt.y - unmoved_start.y : 0);
		}
	
		{
	  	int hints_start = hints & ~hints_first;

	  	pdiff->x = (hints_start & do_x ? pseg->pt.x - unmoved_end.x : 0);
	  	pdiff->y = (hints_start & do_y ? pseg->pt.y - unmoved_end.y : 0);
		}
	
		addHintDiff(&pseg->pt, diff2);
		XWPSSegment::applyFinalHint(pseg, &diff2);
		addHintDiff(&psub->pt, *pdiff);
	
		diff2.x = psub->pt.x - pseg->pt.x;
		diff2.y = psub->pt.y - pseg->pt.y;
		if (diff2.x || diff2.y) 
		{
	  	pseg->pt = psub->pt;
	  	XWPSSegment::applyFinalHint(pseg, &diff2);
		}
  } 
  else 
  {
		int hints_close =	lineHints(&unmoved_end, &unmoved_start);

		hints_close &= ~(hints | hints_first);
		if (hints_close) 
		{
	    applyHintsAt(hints_close, &pseg->pt, pdiff);
	    XWPSSegment::applyFinalHint(pseg, pdiff);
	    applyHintsAt(hints_close, &psub->pt, pdiff);
		} 
		else
	    pdiff->x = pdiff->y = 0;
  }
  if (pfirst->type == s_curve)
		((XWPSCurveSegment *) pfirst)->adjustCurveStart(pdiff);
}

void XWPSType1State::centerVStem(long x0, long dx)
{
	type1DoCenterVStem(x0, dx, &pis->ctm);
}

void XWPSType1State::copy(XWPSType1State * other)
{
	pfont = other->pfont;
	pis = other->pis;
	path = other->path;
	charpath_flag = other->charpath_flag;
	paint_type = other->paint_type;
	callback_data = other->callback_data;
	fc = other->fc;
	flatness = other->flatness;
	scale.copy(&other->scale);
	fh.copy(&other->fh);
	origin = other->origin;
	memcpy(ostack, other->ostack, ostack_size*sizeof(long));
	os_count = other->os_count;
	for (int i = 0; i < (ipstack_size + 1); i++)
		ipstack[i] = other->ipstack[i];
	ips_count = other->ips_count;
	init_done = other->init_done;
	sb_set = other->sb_set;
	width_set = other->width_set;
	have_hintmask = other->have_hintmask;
	num_hints = other->num_hints;
	lsb = other->lsb;
	width = other->width;
	seac_accent = other->seac_accent;
	save_asb = other->save_asb;
	save_lsb = other->save_lsb;
	save_adxy = other->save_adxy;
	asb_diff = other->asb_diff;	
	adxy = other->adxy;
	position = other->position;
	flex_path_state_flags = other->flex_path_state_flags;
	for (int i = 0; i < flex_max; i++)
		flex_points[i] = other->flex_points[i];
	flex_count = other->flex_count;
  ignore_pops = other->ignore_pops;
  dotsection_flag = other->dotsection_flag;
  vstem3_set = other->vstem3_set;
  vs_offset = other->vs_offset;	
  hints_initial = other->hints_initial;
  unmoved_start = other->unmoved_start;
  hint_next = other->hint_next;
  hints_pending = other->hints_pending;
  unmoved_end = other->unmoved_end;
  hstem_hints.copy(&other->hstem_hints);
  vstem_hints.copy(&other->vstem_hints);
  memcpy(transient_array, other->transient_array, 32 * sizeof(long));
}

void XWPSType1State::detectEdgeHint(long *xy, long *dxy)
{
	if (*dxy == -21) 
	{
		*xy -= 21, *dxy = 0;
  } 
  else if (*dxy == -20) 
		*dxy = 0;
}

long XWPSType1State::findSnap(long dv, 
	              XWPSStemSnapTable * psst, 
	              XWPSPixelScale * pps)
{
#define max_snap_distance (pps->half)
    
  long best = max_snap_distance;
  long adj_dv;
  int i;

  for (i = 0; i < psst->count; i++)
  {
		long diff = psst->data[i] - dv;

		if (qAbs(diff) < qAbs(best)) 
	    best = diff;
  }
  adj_dv = pps->scaledRounded((qAbs(best) < max_snap_distance ?  dv + best : dv));
  if (adj_dv == 0)
		adj_dv = pps->unit;

  return adj_dv;
#undef max_snap_distance
}

XWPSAlignmentZone * XWPSType1State::findZone(long vbot, long vtop)
{
	XWPSAlignmentZone *pz;

  for (pz = &fh.a_zones[fh.a_zone_count]; --pz >= &fh.a_zones[0];) 
  {
		long v = (pz->is_top_zone ? vtop : vbot);

		if (v >= pz->v0 && v <= pz->v1) 
	    return pz;
  }
  return 0;
}

int XWPSType1State::lineHints(const XWPSFixedPoint * p0, const XWPSFixedPoint * p1)
{
	long dx = p1->x - p0->x;
  long dy = p1->y - p0->y;
  long adx, ady;
  bool xi = fh.x_inverted, yi = fh.y_inverted;
  int hints;
  
  if (xi)
		dx = -dx;
  if (yi)
		dy = -dy;
  if (fh.axes_swapped) 
  {
		long t = dx;
		int ti = xi;

		dx = dy, xi = yi;
		dy = t, yi = ti;
  }
  adx = qAbs(dx);
  ady = qAbs(dy);
  
  if (dy != 0 && nearlyAxial(ady, adx)) 
  {
		hints = (dy > 0 ? HINT_VERT_UPPER : HINT_VERT_LOWER);
		if (xi)
	    hints ^= (HINT_VERT_LOWER | HINT_VERT_UPPER);
  } 
  else if (dx != 0 && nearlyAxial(adx, ady)) 
  {
		hints = (dx < 0 ? HINT_HORZ_UPPER : HINT_HORZ_LOWER);
		if (yi)
	    hints ^= (HINT_HORZ_LOWER | HINT_HORZ_UPPER);
  } 
  else
		hints = 0;
  return hints;
}

void XWPSType1State::replaceStemHints()
{
	applyPathHints(false);
	type1ReplaceStemHints();
}

void XWPSType1State::resetStemHints()
{
	hstem_hints.count = hstem_hints.replaced_count = 0;
  vstem_hints.count = vstem_hints.replaced_count = 0;
  updateStemHints();
}

XWPSStemHint * XWPSType1State::searchHints(XWPSStemHintTable * psht, long v)
{
	XWPSStemHint *table = &psht->data[0];
  XWPSStemHint *ph = table + psht->current;

  if (v >= ph->v0 && v <= ph->v1 && ph->active)
		return ph;
		
  for (ph = &table[psht->count]; --ph >= table;)
		if (v >= ph->v0 && v <= ph->v1 && ph->active) 
		{
	    psht->current = ph - table;
	    return ph;
		}
  return 0;
}

void XWPSType1State::setLSB(const XWPSPoint * psbpt)
{
	lsb.x = float2fixed(psbpt->x);
  lsb.y = float2fixed(psbpt->y);
  sb_set = true;
}

void XWPSType1State::setWidth(const XWPSPoint * pwpt)
{
	width.x = float2fixed(pwpt->x);
  width.y = float2fixed(pwpt->y);
  width_set = true;
}

void XWPSType1State::skipiv()
{
	int skip = pfont->lenIV;
  XWPSIPState *ipsp = &ipstack[ips_count - 1];
  uchar *cip = ipsp->char_string.data;
  ushort state = crypt_charstring_seed;
  for (; skip > 0; ++cip, --skip)
		decrypt_skip_next(*cip, state);
  ipsp->ip = cip;
  ipsp->dstate = state;
}

void XWPSType1State::type1ApplyPathHints(bool closing, XWPSPath * ppath)
{
	XWPSSegment *pseg = hint_next;
  XWPSSegment *pnext;
  XWPSSubpath *const psub = ppath->currentSubpath();
  
  int hints = hints_pending;
  XWPSFixedPoint diff;
  
  if (init_done < 0)
		return;
    
  if (pseg == 0) 
  {
		if (psub == 0)
	    return;
		if (psub->is_closed && !closing)
	    return;
		pseg = psub;
		if (pseg->next == 0)
	    return;
		hints = 0;
		unmoved_start = psub->pt;
		unmoved_end = psub->pt;
  } 
  else
		hints = hints_pending;
		
  for (; (pnext = pseg->next) != 0; pseg = pnext) 
  {
		int hints_next;
		XWPSFixedPoint dseg;

		switch (pnext->type) 
		{
	    case s_curve:
	    	{
		    	XWPSCurveSegment * pnext_curve = (XWPSCurveSegment *) pnext;
		    	int hints_first = lineHints(&unmoved_end,    &pnext_curve->p1) & ~hints;
		    	XWPSFixedPoint diff2;

		    	if (pseg == psub)
						hints_initial = hints_first;
		    	if (hints_first)
						applyHintsAt(hints_first, &pseg->pt, &dseg);
		    	else
						dseg.x = dseg.y = 0;
		    	diff2.x = pseg->pt.x - unmoved_end.x;
		    	diff2.y = pseg->pt.y - unmoved_end.y;
		    	hints_next = lineHints( &pnext_curve->p2, &pnext->pt);
		    	pnext_curve->adjustCurveStart(&diff2);
		    	if (hints_next) 
		    	{
						applyHintsAt(hints_next, &pnext_curve->p2, &diff);
						unmoved_end = pnext->pt;
						addHintDiff(&pnext->pt, diff);
		    	} 
		    	else
						unmoved_end = pnext->pt;
		    	break;
				}
				
	    case s_line_close:
				pnext->pt = unmoved_start;
				
	    default:
				if (lineIsNull(pnext->pt, unmoved_end)) 
				{
		    	hints_next = hints;
		    	dseg.x = dseg.y = 0;
				} 
				else 
				{
		    	hints_next = lineHints(&unmoved_end, &pnext->pt);
		    	if (hints_next & ~hints)
						applyHintsAt(hints_next & ~hints,  &pseg->pt, &dseg);
		    	else
						dseg.x = dseg.y = 0;
				}
				if (pseg == psub)
		    	hints_initial = hints_next;
				unmoved_end = pnext->pt;
				if (hints_next)
		    	applyHintsAt(hints_next, &pnext->pt, NULL);
		}
		if (pseg->type == s_curve)
	   	((XWPSCurveSegment *) pseg)->adjustCurveEnd(&dseg);
		hints = hints_next;
  }
  if (closing) 
  {
		applyWrappedHints(psub, pseg, hints, &diff);
		hint_next = 0;
		hints_pending = 0;
  } 
  else 
  {
		hint_next = pseg;
		hints_pending = hints;
  }
}

int XWPSType1State::type1Blend(long *csp, int num_results)
{
	int num_values = fixed2int_var(csp[-1]);
  int k1 = num_values / num_results - 1;
  if (num_values < num_results ||	num_values % num_results != 0)
  	return (int)(XWPSError::InvalidFont);
	
	long * base = csp - 1 - num_values;
  long * deltas = base + num_results - 1;
  for (int j = 0; j < num_results; j++, base++, deltas += k1)
		for (int i = 1; i <= k1; i++)
	    *base += deltas[i] * pfont->WeightVector.values[i];
  ignore_pops = num_results;
  return num_values - num_results + 2;
}

int XWPSType1State::type1CallSubr(int index)
{
	int code = pfont->subrData(index, false,	&ipstack[ips_count].char_string);
  if (code < 0)
		return code;
  ips_count++;
  skipiv();
  return 0;
}

void XWPSType1State::type1CisGetMetrics(double psbw[4])
{
	psbw[0] = fixed2float(lsb.x);
  psbw[1] = fixed2float(lsb.y);
  psbw[2] = fixed2float(width.x);
  psbw[3] = fixed2float(width.y);
}

void XWPSType1State::type1DoCenterVStem(long x0, long dx, const XWPSMatrixFixed * pmat)
{
	long x1 = x0 + dx;
  XWPSFixedPoint pt0, pt1, widthA;
  long center, int_width;
  long *psxy;

  if (pt0.transform2fixed((XWPSMatrixFixed*)pmat, fixed2float(x0), 0.0) < 0 ||
			pt1.transform2fixed((XWPSMatrixFixed*)pmat, fixed2float(x1), 0.0) < 0) 
	{	
		return;
  }
    
  widthA.x = pt0.x - pt1.x;
  if (widthA.x < 0)
		widthA.x = -widthA.x;
  widthA.y = pt0.y - pt1.y;
  if (widthA.y < 0)
		widthA.y = -widthA.y;
  if (width.y < float2fixed(0.05)) 
  {
		center = arith_rshift_1(pt0.x + pt1.x);
		int_width = fixed_rounded(widthA.x);
		psxy = &vs_offset.x;
  } 
  else 
  {	
		center = arith_rshift_1(pt0.y + pt1.y);
		int_width = fixed_rounded(widthA.y);
		psxy = &vs_offset.y;
  }
  if (int_width == fixed_0 || (int_width & fixed_1) == 0) 
		*psxy = fixed_floor(center) + fixed_half - center;
  else 
		*psxy = fixed_rounded(center) - center;
}

void  XWPSType1State::type1DoHStem(long y, long dy, const XWPSMatrixFixed * pmat)
{
	XWPSStemHint *psh;
  XWPSAlignmentZone *pz;
  XWPSPixelScale *psp;
  long v, dv, adj_dv;
  long vtop, vbot;

  if (!fh.use_y_hints || !pmat->txy_fixed_valid)
		return;
    
  detectEdgeHint(&y, &dy);
  y += lsb.y + adxy.y;
  if (fh.axes_swapped) 
  {
		psp = &scale.x;
		v = vs_offset.x + fc.mfixedYX(y, max_coeff_bits) + pmat->tx_fixed;
		dv = fc.mfixedYX(dy, max_coeff_bits);
  } 
  else 
  {
		psp = &scale.y;
		v = vs_offset.y + fc.mfixedYY(y, max_coeff_bits) + pmat->ty_fixed;
		dv = fc.mfixedYY(dy, max_coeff_bits);
  }
  
  if (dy < 0)
		vbot = v + dv, vtop = v;
  else
		vbot = v, vtop = v + dv;
  
  if (dv < 0)
		v += dv, dv = -dv;
  psh = type1Stem(&hstem_hints, v, dv);
  if (psh == 0)
		return;
    
  adj_dv = findSnap(dv, &fh.snap_h, psp);
  pz = findZone(vbot, vtop);
  if (pz != 0) 
  {
		int inverted = (fh.axes_swapped ? fh.x_inverted : fh.y_inverted);
		int adjust_v1 =	(inverted ? !pz->is_top_zone : pz->is_top_zone);
		long flat_v = pz->flat;
		long overshoot = (pz->is_top_zone ? vtop - flat_v : flat_v - vbot);
		long pos_over =	(inverted ? -overshoot : overshoot);
		long ddv = adj_dv - dv;
		long shift = psp->scaledRounded(flat_v) - flat_v;

		if (pos_over > 0) 
		{
	    if (pos_over < fh.blue_shift || fh.suppress_overshoot) 
	    {	
				if (pz->is_top_zone)
		    	shift -= overshoot;
				else
		    	shift += overshoot;
	    } 
	    else if (pos_over < psp->unit) 
	    {
				if (overshoot < 0)
		    	overshoot = -psp->unit - overshoot;
				else
		    	overshoot = psp->unit - overshoot;
				if (pz->is_top_zone)
		    	shift += overshoot;
				else
		    	shift -= overshoot;
	    }
		}
		if (adjust_v1)
	    psh->dv1 = shift, psh->dv0 = shift - ddv;
		else
	    psh->dv0 = shift, psh->dv1 = shift + ddv;
  } 
  else 
		hstem_hints.storeStemDeltas(psh, psp, v, dv, adj_dv);
}

void XWPSType1State::type1DoVStem(long x, long dx, const XWPSMatrixFixed * pmat)
{
	XWPSStemHint *psh;
  XWPSPixelScale *psp;
  long v, dv, adj_dv;

  if (!fh.use_x_hints)
		return;
		
  detectEdgeHint(&x, &dx);
  x += lsb.x + adxy.x;
  
  if (fh.axes_swapped) 
  {
		psp = &scale.y;
		v = vs_offset.y + fc.mfixedXY(x, max_coeff_bits) + pmat->ty_fixed;
		dv = fc.mfixedXY(dx, max_coeff_bits);
  } 
  else 
  {
		psp = &scale.x;
		v = vs_offset.x + fc.mfixedXX(x, max_coeff_bits) + pmat->tx_fixed;
		dv = fc.mfixedXX(dx, max_coeff_bits);
  }
  
  if (dv < 0)
		v += dv, dv = -dv;
  psh = type1Stem(&vstem_hints, v, dv);
  if (psh == 0)
		return;
    
  adj_dv = findSnap(dv, &fh.snap_v, psp);
  if (pfont->ForceBold && adj_dv < psp->unit)
		adj_dv = psp->unit;
  
  vstem_hints.storeStemDeltas(psh, psp, v, dv, adj_dv);
}

int XWPSType1State::type1EndChar()
{
  if (seac_accent >= 0) 
  {
		XWPSOp1State s;
		XWPSString astr;
		int achar = seac_accent;
		int code;

		seac_accent = -1;
		s.p.x = origin.x, s.p.y = origin.y;
		asb_diff = save_asb - save_lsb.x;
		adxy = save_adxy;
		
		s.accumXYProc(adxy.x, adxy.y);
		path->position.x = position.x = s.p.x;
		path->position.y = position.y = s.p.y;
		os_count = 0;
		ips_count = 1;
		resetStemHints();
		code = pfont->seacData(achar, NULL, &astr);
		if (code < 0)
	    return code;
		ips_count = 1;
		if (astr.bytes)
			astr.bytes->incRef();
		ipstack[0].char_string = astr;
		return 1;
  }
  
  if (hint_next != 0 || path->isDrawing())
		applyPathHints(true);
		
  {
		XWPSFixedPoint pt;

		pt.transform2fixed(&pis->ctm,	 fixed2float(width.x), fixed2float(width.y));
		path->addPoint(pt.x, pt.y);
  }
  
  if (scale.x.log2_unit + scale.y.log2_unit == 0) 
  {	
		XWPSFixedRect bbox;
		int dx, dy, dmax;

		path->getBbox(&bbox);
		dx = fixed2int_ceiling(bbox.q.x - bbox.p.x);
		dy = fixed2int_ceiling(bbox.q.y - bbox.p.y);
		dmax = qMax(dx, dy);
		if (fh.snap_h.count || fh.snap_v.count || fh.a_zone_count) 
		{	
	    pis->fill_adjust.x = pis->fill_adjust.y =	(dmax < 15 ? float2fixed(0.15) :
		 					dmax < 25 ? float2fixed(0.1) : fixed_0);
		} 
		else 
		{	
	    pis->fill_adjust.x = pis->fill_adjust.y =		(dmax < 10 ? float2fixed(0.2) :
		 			dmax < 25 ? float2fixed(0.1) : float2fixed(0.05));
		}
  } 
  else 
  {	
		pis->fill_adjust.x = pis->fill_adjust.y = fixed_0;
  }
  
  if (!charpath_flag)
		pis->setFlat(flatness);
  return 0;
}

int  XWPSType1State::type1ExecInit(XWPSTextEnum *penum,
		                 XWPSState *pgs, 
		                 XWPSFontType1 *pfont1)
{
	return type1InterpInit(pgs, pgs->path,	&penum->log2_scale,
	            	(penum->text.operation & TEXT_DO_ANY_CHARPATH) != 0,	pfont1->PaintType, pfont1);
}

void XWPSType1State::type1FinishInit(XWPSOp1State * ps)
{
	XWPSMatrix ctm;
	pis->ctmOnly(&ctm);
	ctm.matrixToFixedCoeff(&fc, max_coeff_bits);
	ps->fc = fc;
	ps->p.x = origin.x = path->position.x;
	ps->p.y = origin.y = path->position.y;
	
	asb_diff = adxy.x = adxy.y = 0;
  flex_count = flex_max;
  dotsection_flag = dotsection_out;
  vstem3_set = false;
  vs_offset.x = vs_offset.y = 0;
  hints_initial = 0;
  hint_next = 0;
  hints_pending = 0;
  
  {
		XWPSLog2ScalePoint log2_scale;

		log2_scale.x = scale.x.log2_unit;
		log2_scale.y = scale.y.log2_unit;
		if (charpath_flag)
	    fh.reset(&log2_scale);
		else
	    fh.computeFontHints(&pis->ctm, &log2_scale, pfont);
  }
  
  resetStemHints();
  flatness = pis->charFlatness(0.001);
  ps->accumXYProc(lsb.x, lsb.y);
  position.x = ps->p.x;
  position.y = ps->p.y;

  init_done = 1;
}

void XWPSType1State::type1HStem(long y, long dy)
{
	type1DoHStem(y, dy, &pis->ctm);
}

int  XWPSType1State::type1InterpInit(XWPSImagerState * pisA,
                                     XWPSPath * ppath, 
                                     const XWPSLog2ScalePoint * pscale,
                                     bool ,
		     							               int paint_typeA, XWPSFontType1 * pfontA)
{
	XWPSLog2ScalePoint no_scale;
	XWPSLog2ScalePoint *plog2_scale =	(FORCE_HINTS_TO_BIG_PIXELS ? (XWPSLog2ScalePoint*)pscale : &no_scale);
	pfont = pfontA;
  pis = pisA;
  path = ppath;
  callback_data = pfontA;
  charpath_flag = false;
  paint_type = paint_typeA;
  os_count = 0;
  ips_count = 1;
  ipstack[0].ip = 0;
  ipstack[0].char_string.data = 0;
  ipstack[0].char_string.size = 0;
  ignore_pops = 0;
  init_done = -1;
  sb_set = false;
  width_set = false;
  have_hintmask = false;
  num_hints = 0;
  seac_accent = -1;
  scale.set(plog2_scale);
  return 0;
}

int  XWPSType1State::type1Interpret(const XWPSString * str, int *pindex)
{
	bool encrypted = pfont->lenIV >= 0;
  XWPSOp1State s;
  long cstack[ostack_size];

#define cs0 cstack[0]
#define ics0 fixed2int_var(cs0)
#define cs1 cstack[1]
#define ics1 fixed2int_var(cs1)
#define cs2 cstack[2]
#define ics2 fixed2int_var(cs2)
#define cs3 cstack[3]
#define ics3 fixed2int_var(cs3)
#define cs4 cstack[4]
#define ics4 fixed2int_var(cs4)
#define cs5 cstack[5]
#define ics5 fixed2int_var(cs5)
    
  long * csp;
  
#define clear CLEAR_CSTACK(cstack, csp)
    
  XWPSIPState *ipsp = &ipstack[ips_count - 1];
  uchar *cip;
  ushort state;
  int c;
  int code = 0;
  long ftx = origin.x, fty = origin.y;

  switch (init_done) 
  {
		case -1:
	    break;
	    
		case 0:
	    type1FinishInit(&s);
	    ftx = origin.x, fty = origin.y;
	    break;
	    
		default:
	    s.p.x = position.x;
	    s.p.y = position.y;
	    s.fc = fc;
  }
  s.pcis = this;
  s.ppath = path;
  INIT_CSTACK(cstack, csp);

  if (str == 0)
		goto cont;
		
	if (str->bytes)
		str->bytes->incRef();
  ipsp->char_string = *str;
  cip = str->data;
  
call:
	state = crypt_charstring_seed;
  if (encrypted) 
  {
		int skip = pfont->lenIV;
		for (; skip > 0; ++cip, --skip)
	    decrypt_skip_next(*cip, state);
  }
  goto top;
  
cont:
	cip = ipsp->ip;
  state = ipsp->dstate;
  
top:
	for (;;) 
	{
		uint c0 = *cip++;

		charstring_next(c0, state, c, encrypted);
		if (c >= c_num1) 
		{
	  	if (c < c_pos2_0) 
	  	{	
				decode_push_num1(csp, c);
	  	} 
	  	else if (c < cx_num4) 
	  	{
				decode_push_num2(csp, c, cip, state, encrypted);
	  	} 
	  	else if (c == cx_num4) 
	  	{
				long lw;

				decode_num4(lw, cip, state, encrypted);
				*++csp = int2fixed(lw);
				if (lw != fixed2long(*csp)) 
				{
		    	long denom;

		    	c0 = *cip++;
		    	charstring_next(c0, state, c, encrypted);
		    	if (c < c_num1)
						return (int)(XWPSError::RangeCheck);
		    	if (c < c_pos2_0)
						decode_num1(denom, c);
		    	else if (c < cx_num4)
						decode_num2(denom, c, cip, state, encrypted);
		    	else if (c == cx_num4)
						decode_num4(denom, cip, state, encrypted);
		    	else
						return (int)(XWPSError::InvalidFont);
		    	c0 = *cip++;
		    	charstring_next(c0, state, c, encrypted);
		    	if (c != cx_escape)
						return (int)(XWPSError::RangeCheck);
		    	c0 = *cip++;
		    	charstring_next(c0, state, c, encrypted);
		    	if (c != ce1_div)
						return (int)(XWPSError::RangeCheck);
		    	*csp = float2fixed((double)lw / denom);
				}
	  	} 
	  	else
				return (int)(XWPSError::InvalidFont);

pushed:
	  	continue;
		}
	
		switch ((PSCharCommand) c) 
		{
#define cnext clear; goto top
#define inext goto top

	    case c_undef0:
	    case c_undef2:
	    case c_undef17:
				return (int)(XWPSError::InvalidFont);
					
	    case c_callsubr:
				c = fixed2int_var(*csp) + pfont->subroutineNumberBias;
				code = pfont->subrData(c, false, &ipsp[1].char_string);
				if (code < 0)
		    	return code;
				--csp;
				ipsp->ip = cip, ipsp->dstate = state;
				++ipsp;
				cip = ipsp->char_string.data;
				goto call;
				
	    case c_return:
				--ipsp;
				goto cont;
				
	    case c_undoc15:
				cnext;
				
	    case cx_hstem:
				applyPathHints(false);
				type1HStem(cs0, cs1);
				cnext;
				
	    case cx_vstem:
				applyPathHints(false);
				type1VStem(cs0, cs1);
				cnext;
				
	    case cx_vmoveto:
				cs1 = cs0;
				cs0 = 0;
				s.p.y += s.fc.mfixedYY(cs1, max_coeff_bits);
				if ( s.fc.skewed ) 
					s.p.x += s.fc.mfixedYX(cs1, max_coeff_bits);
				
move:	
				if ((hint_next != 0 || s.ppath->isDrawing()) && flex_count == flex_max)
		    	applyPathHints(true);
				code = s.ppath->addPoint(s.p.x, s.p.y);
				goto cc;
				
	    case cx_rlineto:
				s.accumXYProc(cs0, cs1);
	      
line:
				code = s.ppath->addLineNotes(s.p.x, s.p.y, sn_none);
	      
cc:
				if (code < 0)
		    	return code;

pp:	
				cnext;
				
	    case cx_hlineto:
	    	s.p.x += s.fc.mfixedXX(cs0, max_coeff_bits);
	    	if ( s.fc.skewed ) 
	    		s.p.y += s.fc.mfixedXY(cs0, max_coeff_bits);
				cs1 = 0;
				goto line;
				
	    case cx_vlineto:
				cs1 = cs0;
				cs0 = 0;
				s.p.y += s.fc.mfixedYY(cs1, max_coeff_bits);
				if ( s.fc.skewed ) 
					s.p.x += s.fc.mfixedYX(cs1, max_coeff_bits);
				goto line;
				
	    case cx_rrcurveto:
				code = s.rrcurveto(cs0, cs1, cs2, cs3, cs4, cs5);
				goto cc;
				
	    case cx_endchar:
				code = type1EndChar();
				if (code == 1) 
				{
		    	s.p = position;
		    	ipsp = &ipstack[ips_count - 1];
		    	cip = ipsp->char_string.data;
		    	goto call;
				}
				return code;
				
	    case cx_rmoveto:
				s.accumXYProc(cs0, cs1);
				goto move;
				
	    case cx_hmoveto:
				s.p.x += s.fc.mfixedXX(cs0, max_coeff_bits);
	    	if ( s.fc.skewed ) 
	    		s.p.y += s.fc.mfixedXY(cs0, max_coeff_bits);
				cs1 = 0;
				goto move;
				
	    case cx_vhcurveto:
				{
		    	XWPSFixedPoint pt1, pt2;
		    	long ax0 = s.ppath->position.x - s.p.x;
		    	long ay0 = s.ppath->position.y - s.p.y;

		    	s.p.y += s.fc.mfixedYY(cs0, max_coeff_bits);
					if ( s.fc.skewed ) 
						s.p.x += s.fc.mfixedYX(cs0, max_coeff_bits);
		    	pt1.x = s.p.x + ax0, pt1.y = s.p.y + ay0;
		    	s.accumXYProc(cs1, cs2);
		    	pt2.x = s.p.x, pt2.y = s.p.y;
		    	s.p.x += s.fc.mfixedXX(cs3, max_coeff_bits);
	    		if ( s.fc.skewed ) 
	    			s.p.y += s.fc.mfixedXY(cs3, max_coeff_bits);
		    	code = s.ppath->addCurveNotes(pt1.x, pt1.y, pt2.x, pt2.y, s.p.x, s.p.y, sn_none);
				}
				goto cc;
				
	    case cx_hvcurveto:
				{
		    	XWPSFixedPoint pt1, pt2;
		    	long ax0 = s.ppath->position.x - s.p.x;
		    	long ay0 = s.ppath->position.y - s.p.y;

		    	s.p.x += s.fc.mfixedXX(cs0, max_coeff_bits);
	    		if ( s.fc.skewed ) 
	    			s.p.y += s.fc.mfixedXY(cs0, max_coeff_bits);
		    	pt1.x = s.p.x + ax0, pt1.y = s.p.y + ay0;
		    	s.accumXYProc(cs1, cs2);
		    	pt2.x = s.p.x, pt2.y = s.p.y;
		    	s.p.y += s.fc.mfixedYY(cs3, max_coeff_bits);
					if ( s.fc.skewed ) 
						s.p.x += s.fc.mfixedYX(cs3, max_coeff_bits);
		    	code = s.ppath->addCurveNotes(pt1.x, pt1.y, pt2.x, pt2.y, s.p.x, s.p.y, sn_none);
				}
				goto cc;

	    case c1_closepath:
				code = s.closePath();
				applyPathHints(true);
				goto cc;
				
	    case c1_hsbw:
				type1Sbw(cs0, fixed_0, cs1, fixed_0);
				cs1 = fixed_0;
				
rsbw:
				os_count = 0;	
				ipsp->ip = cip, ipsp->dstate = state;
				ips_count = ipsp - &ipstack[0] + 1;
				if (init_done < 0) 
				{
		    	init_done = 0;
				} 
				else 
				{
		    	if (seac_accent >= 0) 
		    	{
						long dsbx = cs0 - save_lsb.x;
						long dsby = cs1 - save_lsb.y;

						if (dsbx | dsby) 
						{
			    		s.accumXYProc(dsbx, dsby);
			    		lsb.x += dsbx;
			    		lsb.y += dsby;
			    		save_adxy.x -= dsbx;
			    		save_adxy.y -= dsby;
						}
		    	} 
		    	else
						s.accumXYProc(lsb.x, lsb.y);
		    	position.x = s.p.x;
		    	position.y = s.p.y;
				}
				return type1_result_sbw;
				
	    case cx_escape:
				charstring_next(*cip, state, c, encrypted);
				++cip;
		
				switch ((PSChar1ExtendedCommand) c) 
				{
		    	case ce1_dotsection:
						dotsection_flag ^= (dotsection_in ^ dotsection_out);
						cnext;
						
		    	case ce1_vstem3:
						applyPathHints(false);
						if (!vstem3_set && fh.use_x_hints) 
						{
			    		centerVStem(lsb.x + cs2, cs3);
			    		s.p.x += vs_offset.x;
			    		s.p.y += vs_offset.y;
			    		vstem3_set = true;
						}
						type1VStem(cs0, cs1);
						type1VStem(cs2, cs3);
						type1VStem(cs4, cs5);
						cnext;
						
		    	case ce1_hstem3:
						applyPathHints(false);
						type1HStem(cs0, cs1);
						type1HStem(cs2, cs3);
						type1HStem(cs4, cs5);
						cnext;
						
		    	case ce1_seac:
						code = type1Seac(cstack + 1, cstack[0], ipsp);
						if (code != 0) 
						{
			    		*pindex = ics3;
			    		return code;
						}
						clear;
						
						cip = ipsp->char_string.data;
						goto call;
					
		    	case ce1_sbw:
						type1Sbw(cs0, cs1, cs2, cs3);
						goto rsbw;
						
		    	case ce1_div:
						csp[-1] = float2fixed((double)csp[-1] / (double)*csp);
						--csp;
						goto pushed;
						
		    	case ce1_undoc15:
						cnext;
						
		    	case ce1_callothersubr:
						{
			    		int num_results;

#define fpts flex_points

			    		switch (*pindex = fixed2int_var(*csp)) 
			    		{
								case 0:
				    			{	
										XWPSFixedPoint ept;
#if defined(DEBUG) || !ALWAYS_DO_FLEX_AS_CURVE
					long fheight = csp[-4];
					XWPSFixedPoint hpt;

#endif

										if (flex_count != 8)
					    				return (int)(XWPSError::InvalidFont);
										ignore_pops = 2;
										csp[-4] = csp[-3] - asb_diff;
										csp[-3] = csp[-2];
										csp -= 3;
										s.ppath->currentPoint(&ept);
										s.ppath->addPoint(fpts[0].x, fpts[0].y);
										s.ppath->state_flags =	flex_path_state_flags;

#if !ALWAYS_DO_FLEX_AS_CURVE	
										if (qAbs(hpt.x) + qAbs(hpt.y) < fheight / 100) 
										{	
					    				code = s.ppath->addLineNotes(ept.x, ept.y, sn_none);
										} 
										else
#endif
										{	
					    				code = s.ppath->addCurveNotes(fpts[2].x, fpts[2].y,
						       					fpts[3].x, fpts[3].y, fpts[4].x, fpts[4].y, sn_none);
					    				if (code < 0)
												return code;
					    				code = s.ppath->addCurveNotes(fpts[5].x, fpts[5].y,
						       							fpts[6].x, fpts[6].y, fpts[7].x, fpts[7].y, sn_none);
										}
				    			}
				    			if (code < 0)
										return code;
				    			flex_count = flex_max;
				    			inext;
				    		
								case 1:
				    			s.ppath->currentPoint(&fpts[0]);
				    			flex_path_state_flags =	s.ppath->state_flags;
				    			flex_count = 1;
				    			csp -= 2;	
				    			inext;
				    		
								case 2:
				    			if (flex_count >= flex_max)
										return (int)(XWPSError::InvalidFont);
				    			s.ppath->currentPoint(&fpts[flex_count++]);
				    			csp -= 2;
				    			inext;
				    		
								case 3:
				    			ignore_pops = 1;
				    			replaceStemHints();
				    			csp -= 2;
				    			inext;
				    		
								case 12:
								case 13:
				    			cnext;
				    		
								case 14:
				    			num_results = 1;
				  
blend:
				    			code = type1Blend(csp, num_results);
				    			if (code < 0)
										return code;
				    			csp -= code;
				    			inext;
				    		
								case 15:
				    			num_results = 2;
				    			goto blend;
				    		
								case 16:
				    			num_results = 3;
				    			goto blend;
				    		
								case 17:
				    			num_results = 4;
				    			goto blend;
				    		
								case 18:
				    			num_results = 6;
				    			goto blend;
				    		
			    		}
						}
#undef fpts
						{
			    		int scount = csp - cstack;
			    		int n;

			    		if (scount < 1 || csp[-1] < 0 || csp[-1] > int2fixed(scount - 1))
								return (int)(XWPSError::InvalidFont);
			    		n = fixed2int_var(csp[-1]);
			    		code = (*pfont->type1Procs.push_values)(callback_data, csp - (n + 1), n);
			    		if (code < 0)
								return code;
			    		scount -= n + 1;
			    		position.x = s.p.x;
			    		position.y = s.p.y;
			    		applyPathHints(false);
			    		ipsp->ip = cip, ipsp->dstate = state;
			    		os_count = scount;
			    		ips_count = ipsp - &ipstack[0] + 1;
			    		if (scount)
								memcpy(ostack, cstack, scount * sizeof(long));
			    		return type1_result_callothersubr;
						}
						
		    	case ce1_pop:
						if (ignore_pops != 0) 
						{
			    		ignore_pops--;
			    		inext;
						}
						++csp;
						
						code = (*pfont->type1Procs.pop_value)(callback_data, csp);
						if (code < 0)
			    		return code;
						goto pushed;
						
		    	case ce1_setcurrentpoint:
						s.p.x = ftx, s.p.y = fty;
						cs0 += adxy.x;
						cs1 += adxy.y;
						s.accumXYProc(cs0, cs1);
						goto pp;
						
		    	default:
						return (int)(XWPSError::InvalidFont);
				}

	    case 16: 
	    case 18: 
	    case 19:
			case 20: 
			case 23: 
			case 24:
			case 25: 
			case 26: 
			case 27: 
			case 28: 
			case 29:
	    default:	
				return (int)(XWPSError::InvalidFont);
					
#undef cnext
#undef inext
		}
  }
  
#undef cs0 
#undef ics0 
#undef cs1 
#undef ics1 
#undef cs2 
#undef ics2 
#undef cs3 
#undef ics3 
#undef cs4 
#undef ics4 
#undef cs5 
#undef ics5 
#undef clear
}

int XWPSType1State::type1Next()
{
	XWPSIPState *ipsp = &ipstack[ips_count - 1];
  uchar *cip;
  ushort state;
  long *csp = &ostack[os_count - 1];
  const bool encrypted = pfont->lenIV >= 0;
  int c, code, num_results;
  
load:
  cip = ipsp->ip;
  state = ipsp->dstate;
  for (;;)
  {
  	uint c0 = *cip++;
  	charstring_next(c0, state, c, encrypted);
  		
  	if (c >= c_num1)
  	{
  		if (c < c_pos2_0) 
  		{	
				decode_push_num1(csp, c);
	    } 
	    else if (c < cx_num4) 
	    {	
				decode_push_num2(csp, c, cip, state, encrypted);
	    } 
	    else if (c == cx_num4) 
	    {
				long lw;
				decode_num4(lw, cip, state, encrypted);
				*++csp = int2fixed(lw);
	    }
  		else
  			return (int)(XWPSError::InvalidFont);
  		continue;
  	}
  	
  	switch ((PSCharCommand) c)
  	{
  		default:
	    	break;
			case c_undef0:
			case c_undef2:
			case c_undef17:
	    	return (int)(XWPSError::InvalidFont);
	    		
	    case c_callsubr:
	    	code = type1CallSubr(fixed2int_var(*csp));
	    	if (code < 0)
					return code;
	    	ipsp->ip = cip; 
	    	ipsp->dstate = state;
	    	--csp;
	    	++ipsp;
	    	goto load;
	    	
			case c_return:
	    	ips_count--;
	    	--ipsp;
	    	goto load;
	    	
	    case cx_escape:
	    	if (encrypted)
      	{
      		c = decrypt_this(*cip, state);
      		decrypt_skip_next(*cip, state);
      	}
      	else
      		c = *cip;
	    	++cip;
	    	switch ((PSChar1ExtendedCommand) c)
	    	{
	    		default:
						c += CE_OFFSET;
						break;
						
	    		case ce1_div:
						csp[-1] = float2fixed((double)csp[-1] / (double)*csp);
						--csp;
						continue;
						
					case ce1_undoc15:
						csp = ostack - 1;
						continue;
						
					case ce1_callothersubr:
						switch (fixed2int_var(*csp))
						{
							case 0:
		    				ignore_pops = 2;
		    				break;
		    				
							case 3:
		    				ignore_pops = 1;
		    				break;
		    				
							case 14:
		    				num_results = 1; goto blend;
							case 15:
		    				num_results = 2; goto blend;
							case 16:
		    				num_results = 3; goto blend;
							case 17:
		    				num_results = 4; goto blend;
							case 18:
		    				num_results = 6;
		    				
blend:
		    				code = type1Blend(csp, num_results);
		    				if (code < 0)
									return code;
		    				csp -= code;
		    				continue;
		    				
							default:
		    				break;
						}
						break;
						
					case ce1_pop:
						if (ignore_pops != 0) 
						{
		    			ignore_pops--;
		    			continue;
						}
						return (int)(XWPSError::RangeCheck);
	    	}
	    	break;
	   }
	   break;
  }
  	
  ipsp->ip = cip; 
  ipsp->dstate = state;
  ips_count = ipsp + 1 - &ipstack[0];
  os_count = csp + 1 - &ostack[0];
  return c;
}

void XWPSType1State::type1NextInit(const XWPSString *pstr, XWPSFontType1 *pfontA)
{
	XWPSLog2ScalePoint no_scale;
  type1InterpInit(NULL, NULL, &no_scale, false, 0, pfontA);
  flex_count = flex_max;
  dotsection_flag = dotsection_out;
  if (pstr->bytes)
  	pstr->bytes->incRef();
  ipstack[0].char_string = *pstr;
  skipiv();
}

void XWPSType1State::type1ReplaceStemHints()
{
	hstem_hints.saveReplacedHints();
  vstem_hints.saveReplacedHints();
}

int XWPSType1State::type1Sbw(long lsbx, long lsby, long wx, long wy)
{
	if (!sb_set)
	{
		lsb.x = lsbx; 
		lsb.y = lsby,
	  sb_set = true;
	}
  if (!width_set)
  {
		width.x = wx; 
		width.y = wy,
	  width_set = true;
	}
  return 0;
}

int XWPSType1State::type1Seac(const long * cstack, long asb, XWPSIPState * ipsp)
{
	XWPSString bcstr;
  int code;

  seac_accent = fixed2int_var(cstack[3]);
  save_asb = asb;
  save_lsb = lsb;
  save_adxy.x = cstack[0];
  save_adxy.y = cstack[1];
  os_count = 0;	
  code = pfont->seacData(fixed2int_var(cstack[2]), NULL, &bcstr);
  if (code != 0)
		return code;
	if (bcstr.bytes)
		bcstr.bytes->incRef();
  ipsp->char_string = bcstr;
  return 0;
}

XWPSStemHint * XWPSType1State::type1Stem(XWPSStemHintTable * psht,  long v0, long d)
{
	XWPSStemHint *bot = &psht->data[0];
  XWPSStemHint *top = bot + psht->count;

  if (psht->count >= max_stems)
		return 0;
    
  while (top > bot && v0 < top[-1].v0) 
  {
		*top = top[-1];
		top--;
  }
  
  top->v0 = v0 - STEM_TOLERANCE;
  top->v1 = v0 + d + STEM_TOLERANCE;
  top->index = hstem_hints.count + vstem_hints.count;
  top->active = true;
  psht->count++;
  return top;
}

int XWPSType1State::type1Stem1(XWPSStemHintTable *psht, const long *pv, uchar *active_hints)
{
	long v0 = pv[0], v1 = v0 + pv[1];
  XWPSStemHint *bot = &psht->data[0];
  XWPSStemHint *orig_top = bot + psht->count;
  XWPSStemHint *top = orig_top;

  if (psht->count >= max_stems)
		return (int)(XWPSError::LimitCheck);
			
  while (top > bot && (v0 < top[-1].v0 || (v0 == top[-1].v0 && v1 < top[-1].v1))) 
  {
		*top = top[-1];
		top--;
  }
  
  if (top > bot && v0 == top[-1].v0 && v1 == top[-1].v1) 
  {
		memmove(top, top + 1, (char *)orig_top - (char *)top);
		if (active_hints) 
		{
	    uint index = top[-1].index;
	    active_hints[index >> 3] |= 0x80 >> (index & 7);
		}
		return 0;
  }
  top->v0 = v0;
  top->v1 = v1;
  psht->count++;
  return 0;
}

void XWPSType1State::type1Stem3(XWPSStemHintTable *psht, const long *pv3, uchar *active_hints)
{
	type1Stem1(psht, pv3, active_hints);
  type1Stem1(psht, pv3 + 2, active_hints);
  type1Stem1(psht, pv3 + 4, active_hints);
}

void XWPSType1State::type1VStem(long x, long dx)
{
	type1DoVStem(x, dx, &pis->ctm);
}

int XWPSType1State::type2Interpret(const XWPSString * str, int *)
{
  bool encrypted = pfont->lenIV >= 0;
  XWPSOp1State s;
  long cstack[ostack_size];
  long * csp;
  
#define clear CLEAR_CSTACK(cstack, csp)
  
  XWPSIPState *ipsp = &ipstack[ips_count - 1];
  uchar *cip;
  ushort state;
  int c;
  long * ap;
  bool vertical;
  int code = 0;

  struct 
  {
		float *values;
		uint size;
  } Registry[1];

  Registry[0].values = pfont->WeightVector.values;

  switch (init_done) 
  {
		case -1:
	    break;
	    
		case 0:
	    type1FinishInit(&s);	
	    break;
	    
		default:
	    s.p.x = position.x;
	    s.p.y = position.y;
	    s.fc = fc;
  }
  s.ppath = path;
  s.pcis = this;
  INIT_CSTACK(cstack, csp);

  if (str == 0)
		goto cont;
    
  if (str->bytes)
  	str->bytes->incRef();
  ipsp->char_string = *str;
  cip = str->data;
  
call:
	state = crypt_charstring_seed;
  if (encrypted) 
  {
		int skip = pfont->lenIV;
		for (; skip > 0; ++cip, --skip)
	    decrypt_skip_next(*cip, state);
  }
  goto top;
  
cont:
	cip = ipsp->ip;
  state = ipsp->dstate;
  
top:
	for (;;) 
	{
		uint c0 = *cip++;

		charstring_next(c0, state, c, encrypted);
		if (c >= c_num1) 
		{
	    if (c < c_pos2_0) 
	    {
				decode_push_num1(csp, c);
	    } 
	    else if (c < cx_num4) 
	   	{
				decode_push_num2(csp, c, cip, state, encrypted);
	    } 
	    else if (c == cx_num4) 
	    {
				long lw;

				decode_num4(lw, cip, state, encrypted);
				*++csp = arith_rshift(lw, 16 - _fixed_shift);
	    } 
	    else
				return (int)(XWPSError::InvalidFont);
	  
pushed:
	    continue;
		}
		
		switch ((PSCharCommand) c) 
		{
#define cnext clear; goto top
	    case c_undef0:
	    case c_undef2:
	    case c_undef17:
				return (int)(XWPSError::InvalidFont);
					
	    case c_callsubr:
				c = fixed2int_var(*csp) + pfont->subroutineNumberBias;
				code = pfont->subrData(c, false, &ipsp[1].char_string);
	      
subr:
				if (code < 0)
		    	return code;
				--csp;
				ipsp->ip = cip, ipsp->dstate = state;
				++ipsp;
				cip = ipsp->char_string.data;
				goto call;
				
	    case c_return:
				--ipsp;
				goto cont;
				
	    case c_undoc15:
				cnext;

	    case cx_hstem:
				goto hstem;
				
	    case cx_vstem:
				goto vstem;
				
	    case cx_vmoveto:
				if (pfont->PaintType != 1 && s.ppath != 0) 
				{
		    		code = s.ppath->closeSubpathNotes(sn_none);
		    		if (code < 0)
							return code;
				}
				check_first_operator(csp > cstack);
				s.p.y += s.fc.mfixedYY(*csp, max_coeff_bits);
				if ( s.fc.skewed ) 
					s.p.x += s.fc.mfixedYX(*csp, max_coeff_bits);
				
move:
				if ((hint_next != 0 || s.ppath->isDrawing()))
		    	applyPathHints(true);
				code = s.ppath->addPoint(s.p.x, s.p.y);
	      
cc:
				if (code < 0)
		    	return code;
				goto pp;
				
	    case cx_rlineto:
				for (ap = cstack; ap + 1 <= csp; ap += 2) 
				{
		    	s.accumXYProc(ap[0], ap[1]);
		    	code = s.ppath->addLineNotes(s.p.x, s.p.y, sn_none);
		    	if (code < 0)
						return code;
				}
	      
pp:
				cnext;
				
	    case cx_hlineto:
				vertical = false;
				goto hvl;
				
	    case cx_vlineto:
				vertical = true;
	      
hvl:
				for (ap = cstack; ap <= csp; vertical = !vertical, ++ap) 
				{
		    	if (vertical)
		    	{
		    		s.p.y += s.fc.mfixedYY(*ap, max_coeff_bits);
						if ( s.fc.skewed ) 
							s.p.x += s.fc.mfixedYX(*ap, max_coeff_bits);
					}
		    	else
		    	{
		    		s.p.x += s.fc.mfixedXX(*ap, max_coeff_bits);
		    		if ( s.fc.skewed ) 
		    			s.p.y += s.fc.mfixedXY(*ap, max_coeff_bits);
					}
		    	code = s.ppath->addLineNotes(s.p.x, s.p.y, sn_none);
		  	  if (code < 0)
						return code;
				}
				goto pp;
				
	    case cx_rrcurveto:
				for (ap = cstack; ap + 5 <= csp; ap += 6) 
				{
		    	code = s.rrcurveto(ap[0], ap[1], ap[2],  ap[3], ap[4], ap[5]);
		    	if (code < 0)
						return code;
				}
				goto pp;
				
	    case cx_endchar:
				if (pfont->PaintType != 1 && s.ppath != 0) 
				{
		    	code = s.ppath->closeSubpathNotes(sn_none);
		    	if (code < 0)
						return code;
				}
				if (csp >= cstack + 3) 
				{
		    	check_first_operator(csp > cstack + 3);
		    	code = type1Seac(cstack, lsb.x, ipsp);
		    	if (code < 0)
						return code;
		    	clear;
		    	cip = ipsp->char_string.data;
		    	goto call;
				}
				check_first_operator(csp >= cstack);
				code = type1EndChar();
				if (code == 1) 
				{
		    	num_hints = 0;
		    	s.p = position;
		    	ipsp = &ipstack[ips_count - 1];
		    	cip = ipsp->char_string.data;
		    	goto call;
				}
				return code;
				
	    case cx_rmoveto:
				if (pfont->PaintType != 1 && s.ppath != 0) 
				{
		    	code = s.ppath->closeSubpathNotes(sn_none);
		    	if (code < 0)
						return code;
				}
				check_first_operator(csp > cstack + 1);
				s.accumXYProc(csp[-1], *csp);
				goto move;
				
	    case cx_hmoveto:
				if (pfont->PaintType != 1 && s.ppath != 0) 
				{
		    	code = s.ppath->closeSubpathNotes(sn_none);
		    	if (code < 0)
						return code;
				}
				check_first_operator(csp > cstack);
				s.p.x += s.fc.mfixedXX(*csp, max_coeff_bits);
		    if ( s.fc.skewed ) 
		    	s.p.y += s.fc.mfixedXY(*csp, max_coeff_bits);
				goto move;
				
	    case cx_vhcurveto:
				vertical = true;
				goto hvc;
				
	    case cx_hvcurveto:
				vertical = false;
	      
hvc:
				for (ap = cstack; ap + 3 <= csp; vertical = !vertical, ap += 4) 
				{
		    	XWPSFixedPoint pt1, pt2;
		    	long ax0 = s.ppath->position.x - s.p.x;
		    	long ay0 = s.ppath->position.y - s.p.y;

		    	if (vertical)
		    	{
		    		s.p.y += s.fc.mfixedYY(ap[0], max_coeff_bits);
						if ( s.fc.skewed ) 
							s.p.x += s.fc.mfixedYX(ap[0], max_coeff_bits);
					}
		    	else
		    	{
		    		s.p.x += s.fc.mfixedXX(ap[0], max_coeff_bits);
		    		if ( s.fc.skewed ) 
		    			s.p.y += s.fc.mfixedXY(ap[0], max_coeff_bits);
					}
		    	pt1.x = s.p.x + ax0, pt1.y = s.p.y + ay0;
		    	s.accumXYProc(ap[1], ap[2]);
		    	pt2.x = s.p.x, pt2.y = s.p.y;
		    	if (vertical) 
		    	{
						if (ap + 4 == csp)
			    		s.accumXYProc(ap[3], ap[4]);
						else
						{
			    		s.p.x += s.fc.mfixedXX(ap[3], max_coeff_bits);
		    			if ( s.fc.skewed ) 
		    				s.p.y += s.fc.mfixedXY(ap[3], max_coeff_bits);
			    	}
		    	} 
		    	else 
		    	{
						if (ap + 4 == csp)
			    		s.accumXYProc(ap[4], ap[3]);
						else
						{
			    		s.p.y += s.fc.mfixedYY(ap[3], max_coeff_bits);
							if ( s.fc.skewed ) 
								s.p.x += s.fc.mfixedYX(ap[3], max_coeff_bits);
			    	}
		    	}
		    	code = s.ppath->addCurveNotes(pt1.x, pt1.y, pt2.x, pt2.y, s.p.x, s.p.y, sn_none);
		    	if (code < 0)
						return code;
				}
				goto pp;

	    case c2_blend:
				{
		    	int n = fixed2int_var(*csp);
		    	int num_values = csp - cstack;
		    	int k = pfont->WeightVector.count;
		    	int i, j;
		    	long* base, *deltas;

		    	base = csp - 1 - num_values;
		    	deltas = base + n - 1;
		    	for (j = 0; j < n; j++, base++, deltas += k - 1)
						for (i = 1; i < k; i++)
			    		*base += deltas[i] * pfont->WeightVector.values[i];
				}
				cnext;
				
	    case c2_hstemhm:
				have_hintmask = true;
	      
hstem:
				check_first_operator(!((csp - cstack) & 1));
				applyPathHints(false);
				{
		    	long x = 0;

		    	for (ap = cstack; ap + 1 <= csp; x += ap[1], ap += 2)
						type1HStem(x += ap[0], ap[1]);
				}
				num_hints += (csp + 1 - cstack) >> 1;
				cnext;
				
	    case c2_hintmask:
	    case c2_cntrmask:
				have_hintmask = true;
				check_first_operator(!((csp - cstack) & 1));
				type2VStem(csp, cstack);
				clear;
				{
		    	uchar mask[max_total_stem_hints / 8];
		    	int i;

		    	for (i = 0; i < num_hints; ++cip, i += 8) 
		    	{
						charstring_next(*cip, state, mask[i >> 3], encrypted);
		    	}
		    	ipsp->ip = cip;
		    	ipsp->dstate = state;
		    	if (c == c2_cntrmask) 
		    	{
		    		;
		    	} 
		    	else 
		    	{
						hstem_hints.enableHints(mask);
						vstem_hints.enableHints(mask);
		    	}
				}
				break;
				
	    case c2_vstemhm:
				have_hintmask = true;
	      
vstem:
				check_first_operator(!((csp - cstack) & 1));
				type2VStem(csp, cstack);
				cnext;
				
	    case c2_rcurveline:
				for (ap = cstack; ap + 5 <= csp; ap += 6) 
				{
		    	code = s.rrcurveto(ap[0], ap[1], ap[2], ap[3], ap[4], ap[5]);
		    	if (code < 0)
						return code;
				}
				s.accumXYProc(ap[0], ap[1]);
				code = s.ppath->addLineNotes(s.p.x, s.p.y, sn_none);
				goto cc;
				
	    case c2_rlinecurve:
				for (ap = cstack; ap + 7 <= csp; ap += 2) 
				{
		    	s.accumXYProc(ap[0], ap[1]);
		    	code = s.ppath->addLineNotes(s.p.x, s.p.y, sn_none);
		    	if (code < 0)
						return code;
				}
				code = s.rrcurveto(ap[0], ap[1], ap[2], ap[3],ap[4], ap[5]);
				goto cc;
				
	    case c2_vvcurveto:
				ap = cstack;
				{
		    	int n = csp + 1 - cstack;
		    	long dxa = (n & 1 ? *ap++ : 0);

		    	for (; ap + 3 <= csp; ap += 4) 
		    	{
						code = s.rrcurveto(dxa, ap[0], ap[1], ap[2], fixed_0, ap[3]);
						if (code < 0)
			    		return code;
						dxa = 0;
		    	}
				}
				goto pp;
				
	    case c2_hhcurveto:
				ap = cstack;
				{
		    	int n = csp + 1 - cstack;
		    	long dya = (n & 1 ? *ap++ : 0);

		    	for (; ap + 3 <= csp; ap += 4) 
		    	{
						code = s.rrcurveto(ap[0], dya, ap[1], ap[2],ap[3], fixed_0);
						if (code < 0)
			    		return code;
						dya = 0;
		    	}
				}
				goto pp;
				
	    case c2_shortint:
				{
		    	int c1, c2;

		    	charstring_next(*cip, state, c1, encrypted);
		    	++cip;
		    	charstring_next(*cip, state, c2, encrypted);
		    	++cip;
		    	*++csp = int2fixed((((c1 ^ 0x80) - 0x80) << 8) + c2);
				}
				goto pushed;
				
	    case c2_callgsubr:
				c = fixed2int_var(*csp) + pfont->gsubrNumberBias;
				code = pfont->subrData(c, true, &ipsp[1].char_string);
				goto subr;
				
	    case cx_escape:
				charstring_next(*cip, state, c, encrypted);
				++cip;
				switch ((PSChar2ExtendedCommand) c) 
				{
		    	case ce2_and:
						csp[-1] = ((csp[-1] != 0) & (*csp != 0) ? fixed_1 : 0);
						--csp;
						break;
						
		    	case ce2_or:
						csp[-1] = (csp[-1] | *csp ? fixed_1 : 0);
						--csp;
						break;
						
		    	case ce2_not:
						*csp = (*csp ? 0 : fixed_1);
						break;
						
		    	case ce2_store:
						{
			    		int i, n = fixed2int_var(*csp);
			    		float *to = Registry[fixed2int_var(csp[-3])].values + fixed2int_var(csp[-2]);
			    		long *from = transient_array + fixed2int_var(csp[-1]);

			    		for (i = 0; i < n; ++i)
								to[i] = fixed2float(from[i]);
						}
						csp -= 4;
						break;
						
		    	case ce2_abs:
						if (*csp < 0)
			    		*csp = -*csp;
						break;
						
		    	case ce2_add:
						csp[-1] += *csp;
						--csp;
						break;
						
		    	case ce2_sub:
						csp[-1] -= *csp;
						--csp;
						break;
						
		    	case ce2_div:
						csp[-1] = float2fixed((double)csp[-1] / *csp);
						--csp;
						break;
						
		    	case ce2_load:
						{
			    		int i, n = fixed2int_var(*csp);
			    		float *from = Registry[fixed2int_var(csp[-2])].values;
			    		long *to = transient_array + fixed2int_var(csp[-1]);

			    		for (i = 0; i < n; ++i)
								to[i] = float2fixed(from[i]);
						}
						csp -= 3;
						break;
						
		    	case ce2_neg:
						*csp = -*csp;
						break;
						
		    	case ce2_eq:
						csp[-1] = (csp[-1] == *csp ? fixed_1 : 0);
						--csp;
						break;
						
		    	case ce2_drop:
						--csp;
						break;
						
		    	case ce2_put:
						transient_array[fixed2int_var(*csp)] = csp[-1];
						csp -= 2;
						break;
						
		    	case ce2_get:
						*csp = transient_array[fixed2int_var(*csp)];
						break;
						
		    	case ce2_ifelse:
						if (csp[-1] > *csp)
			    		csp[-3] = csp[-2];
						csp -= 3;
						break;
						
		    	case ce2_random:
						++csp;
						break;
						
		    	case ce2_mul:
						{
			    		double prod = fixed2float(csp[-1]) * *csp;

			    		csp[-1] =	(prod > max_fixed ? max_fixed : prod < min_fixed ? min_fixed : prod);
						}
						--csp;
						break;
						
		    	case ce2_sqrt:
						if (*csp >= 0)
			    		*csp = float2fixed(sqrt(fixed2float(*csp)));
						break;
						
		    	case ce2_dup:
						csp[1] = *csp;
						++csp;
						break;
						
		    	case ce2_exch:
						{
			    		long top = *csp;

			    		*csp = csp[-1], csp[-1] = top;
						}
						break;
						
		    	case ce2_index:
						*csp = (*csp < 0 ? csp[-1] : csp[-1 - fixed2int_var(csp[-1])]);
						break;
						
		    	case ce2_roll:
						{
			    		int distance = fixed2int_var(*csp);
			    		int count = fixed2int_var(csp[-1]);
			    		long* bot;

			    		csp -= 2;
			    		if (count < 0 || count > csp + 1 - cstack)
								return (int)(XWPSError::InvalidFont);
			    		if (count == 0)
							break;
			    		if (distance < 0)
								distance = count - (-distance % count);
			    		bot = csp + 1 - count;
			    		while (--distance >= 0) 
			    		{
								long top = *csp;

								memmove(bot + 1, bot,	(count - 1) * sizeof(long));
								*bot = top;
			    		}
						}
						break;
						
		    	case ce2_hflex:
						csp[6] = fixed_half;
						csp[4] = *csp, csp[5] = 0;
						csp[2] = csp[-1], csp[3] = -csp[-4];
						*csp = csp[-2], csp[1] = 0;	
						csp[-2] = csp[-3], csp[-1] = 0;	
						csp[-3] = csp[-4], csp[-4] = csp[-5];	
						csp[-5] = 0;
						csp += 6;
						goto flex;
						
		    	case ce2_flex:
						*csp /= 100;
flex:			
						{
			    		long x_join = csp[-12] + csp[-10] + csp[-8];
			    		long y_join = csp[-11] + csp[-9] + csp[-7];
			    		long x_end = x_join + csp[-6] + csp[-4] + csp[-2];
			    		long y_end = y_join + csp[-5] + csp[-3] + csp[-1];
			    		XWPSPoint join, end;
			    		double flex_depth;
							XWPSMatrix ctm;
							pis->ctmOnly(&ctm);
			    		if ((code =	join.distanceTransform(fixed2float(x_join),fixed2float(y_join), &ctm)) < 0 ||
									(code = end.distanceTransform(fixed2float(x_end), fixed2float(y_end),  &ctm)) < 0)
								return code;
			    		if (qAbs(end.y) > qAbs(end.x))
								flex_depth = join.x;
			    		else
								flex_depth = join.y;
			    		if (fabs(flex_depth) < fixed2float(*csp)) 
			    		{
								s.accumXYProc(x_end, y_end);
								code = s.ppath->addLineNotes(s.p.x, s.p.y, sn_none);
			    		} 
			    		else 
			    		{
								code = s.rrcurveto(csp[-12], csp[-11], csp[-10],csp[-9], csp[-8], csp[-7]);
								if (code < 0)
				    			return code;
								code = s.rrcurveto(csp[-6], csp[-5], csp[-4],csp[-3], csp[-2], csp[-1]);
			    		}
			    		if (code < 0)
								return code;
			    		csp -= 13;
						}
						cnext;
						
		    	case ce2_hflex1:
						csp[4] = fixed_half;
						csp[2] = *csp, csp[3] = 0;
						*csp = csp[-2], csp[1] = csp[-1];	
						csp[-2] = csp[-3], csp[-1] = 0;	
						csp[-3] = 0;
						csp += 4;
						goto flex;
						
		    	case ce2_flex1:
						{
			    		long dx = csp[-10] + csp[-8] + csp[-6] + csp[-4] + csp[-2];
			    		long dy = csp[-9] + csp[-7] + csp[-5] + csp[-3] + csp[-1];

			    		if (qAbs(dx) > qAbs(dy))
								csp[1] = -dy;
			    		else
								csp[1] = *csp, *csp = -dx;
						}
						csp[2] = fixed_half;
						csp += 2;
						goto flex;
				}
				break;

	    case 9: 
	    case 13:
	    default:
				return (int)(XWPSError::InvalidFont);
				
#undef cnext
		}
  }
#undef clear 
}

int XWPSType1State::type2Sbw(long * csp, long * cstack, XWPSIPState * ipsp,  bool explicit_width)
{
	long wx;

  if (explicit_width) 
  {
		wx = cstack[0] + pfont->nominalWidthX;
		memmove(cstack, cstack + 1, (csp - cstack) * sizeof(*cstack));
		--csp;
  } 
  else
		wx = pfont->defaultWidthX;
    
  type1Sbw(fixed_0, fixed_0, wx, fixed_0);
  
  {
		XWPSIPState *ipsp = &ipstack[ips_count - 1];

		ipsp->ip--;
		decrypt_skip_previous(*ipsp->ip, ipsp->dstate);
  }
  
  os_count = csp + 1 - cstack;
  ips_count = ipsp - &ipstack[0] + 1;
  memcpy(ostack, cstack, os_count * sizeof(cstack[0]));
  if (init_done < 0) 
		init_done = 0;
  return type1_result_sbw;
}

int XWPSType1State::type2VStem(long * csp, long * cstack)
{
	long x = 0;
  long * ap;

  applyPathHints(false);
  for (ap = cstack; ap + 1 <= csp; x += ap[1], ap += 2)
		type1VStem(x += ap[0], ap[1]);
  num_hints += (csp + 1 - cstack) >> 1;
  return 0;
}

void XWPSType1State::updateStemHints()
{
	hstem_hints.current = 0;
  vstem_hints.current = 0;
}

XWPSOp1State::XWPSOp1State()
{
	ppath = 0;
	pcis = 0;
}

XWPSOp1State::~XWPSOp1State()
{
}

void XWPSOp1State::accumXYProc(long dx, long dy)
{
	p.x += fc.mfixedXX(dx, max_coeff_bits),
	p.y += fc.mfixedYY(dy, max_coeff_bits);
  if (fc.skewed)
		p.x += fc.mfixedYX(dy, max_coeff_bits),  p.y += fc.mfixedXY(dx, max_coeff_bits);
}

int XWPSOp1State::closePath()
{
	XWPSSubpath *psub;
  XWPSSegment *pseg;
  long dx, dy;
  int code;

  if ((psub = ppath->currentSubpath()) != 0 &&	(pseg = psub->last) != 0 &&
			(dx = pseg->pt.x - psub->pt.x, qAbs(dx) < float2fixed(0.1)) &&
			(dy = pseg->pt.y - psub->pt.y, qAbs(dy) < float2fixed(0.1)))
		switch (pseg->type) 
		{
	    case s_line:
				code = ppath->popCloseNotes(sn_none);
				break;
				
	    case s_curve:
	    	
				pseg->pt = psub->pt;
#define pcseg ((XWPSCurveSegment *)pseg)
				pcseg->p2.x -= dx;
				pcseg->p2.y -= dy;
#undef pcseg

	    default:
				code = ppath->closeSubpathNotes(sn_none);
    } 
  else
		code = ppath->closeSubpathNotes(sn_none);
  if (code < 0)
		return code;
  return ppath->addPoint(p.x, p.y);
}

int XWPSOp1State::rrcurveto(long dx1, long dy1,
		            long dx2, long dy2, 
		            long dx3, long dy3)
{
	XWPSFixedPoint pt1, pt2;
  long ax0 = ppath->position.x - p.x;
  long ay0 = ppath->position.y - p.y;

  accumXYProc(dx1, dy1);
  pt1.x = p.x + ax0, pt1.y = p.y + ay0;
  accumXYProc(dx2, dy2);
  pt2.x = p.x, pt2.y = p.y;
  accumXYProc(dx3, dy3);
  return ppath->addCurveNotes(pt1.x, pt1.y, pt2.x, pt2.y, p.x, p.y, sn_none);
}

XWPSCffStringItem::XWPSCffStringItem()
{
	index1 = 0;
}

XWPSCffStringTable::XWPSCffStringTable()
{
	items = 0;
	count = 0;
	size = 0;
	total = 0;
	reprobe = 0;
}

XWPSCffStringTable::~XWPSCffStringTable()
{
}

int  XWPSCffStringTable::add(const uchar *data, uint sizeA)
{
	if (count >= size)
		return (int)(XWPSError::LimitCheck);
	
  int  index = count++;
  items[index].key.data = (uchar*)data;
  items[index].key.size = sizeA;
  total += sizeA;
  return index;
}

int  XWPSCffStringTable::index(const uchar *data, uint sizeA,  bool enter, int *pindex)
{
	int j = (sizeA == 0 ? 0 : data[0] * 23 + data[sizeA - 1] * 59 + sizeA);
  int index;

  while ((index = items[j %= size].index1) != 0) 
  {
		--index;
		if (!bytes_compare(items[index].key.data, items[index].key.size, data, sizeA)) 
		{
	    *pindex = index;
	    return 0;
		}
		j += reprobe;
  }
  if (!enter)
		return (int)(XWPSError::Undefined);
			
  index = add(data, sizeA);
  if (index < 0)
		return index;
  items[j].index1 = index + 1;
  *pindex = index;
  return 1;
}

void XWPSCffStringTable::init(XWPSCffStringItem *itemsA, int sizeA)
{
	int reprobeA = 17;

  items = itemsA;
  count = 0;
  size = sizeA;
  while (sizeA % reprobeA == 0 && reprobeA != 1)
		reprobeA = (reprobeA * 2 + 1) % sizeA;
		
	total = 0;
  reprobe = reprobeA;
}

XWPSCffGlyphSubset::XWPSCffGlyphSubset()
{
	num_encoded = 0;
	num_encoded_chars = 0;
}

XWPSCffGlyphSubset::~XWPSCffGlyphSubset()
{
}

enum PSTopOp
{
  TOP_version = 0,
  TOP_Notice = 1,
  TOP_FullName = 2,
  TOP_FamilyName = 3,
  TOP_Weight = 4,
  TOP_FontBBox = 5,
  TOP_UniqueID = 13,
  TOP_XUID = 14,
  TOP_charset = 15,
#define charset_ISOAdobe 0
#define charset_Expert 1
#define charset_ExpertSubset 2
#define charset_DEFAULT 0
  TOP_Encoding = 16,
#define Encoding_Standard 0
#define Encoding_Expert 1
#define Encoding_DEFAULT 0
  TOP_CharStrings = 17,
  TOP_Private = 18,	
  TOP_Copyright = 32,
  TOP_isFixedPitch = 33,
#define isFixedPitch_DEFAULT false
  TOP_ItalicAngle = 34,
#define ItalicAngle_DEFAULT 0
  TOP_UnderlinePosition = 35,
#define UnderlinePosition_DEFAULT (-100)
  TOP_UnderlineThickness = 36,
#define UnderlineThickness_DEFAULT 50
  TOP_PaintType = 37,
#define PaintType_DEFAULT 0
  TOP_CharstringType = 38,
#define CharstringType_DEFAULT 2
  TOP_FontMatrix = 39,
  TOP_StrokeWidth = 40,
#define StrokeWidth_DEFAULT 0
  TOP_ROS = 62,
  TOP_CIDFontVersion = 63,
#define CIDFontVersion_DEFAULT 0
  TOP_CIDFontRevision = 64,
#define CIDFontRevision_DEFAULT 0
  TOP_CIDFontType = 65,
#define CIDFontType_DEFAULT 0
  TOP_CIDCount = 66,
#define CIDCount_DEFAULT 8720
  TOP_UIDBase = 67,
  TOP_FDArray = 68,
  TOP_FDSelect = 69,
  TOP_FontName = 70	
};

enum PSPrivateOp
{
    PRIVATE_BlueValues = 6,
    PRIVATE_OtherBlues = 7,
    PRIVATE_FamilyBlues = 8,
    PRIVATE_FamilyOtherBlues = 9, 
    PRIVATE_StdHW = 10,
    PRIVATE_StdVW = 11,
    PRIVATE_Subrs = 19,	
    PRIVATE_defaultWidthX = 20,
#define defaultWidthX_DEFAULT fixed_0
    PRIVATE_nominalWidthX = 21,
#define nominalWidthX_DEFAULT fixed_0
    PRIVATE_BlueScale = 41,
#define BlueScale_DEFAULT 0.039625
    PRIVATE_BlueShift = 42,
#define BlueShift_DEFAULT 7
    PRIVATE_BlueFuzz = 43,
#define BlueFuzz_DEFAULT 1
    PRIVATE_StemSnapH = 44,
    PRIVATE_StemSnapV = 45,
    PRIVATE_ForceBold = 46,
#define ForceBold_DEFAULT false
    PRIVATE_ForceBoldThreshold = 47,
#define ForceBoldThreshold_DEFAULT 0
    PRIVATE_lenIV = 48,
#define lenIV_DEFAULT (-1)
    PRIVATE_LanguageGroup = 49,
#define LanguageGroup_DEFAULT 0
    PRIVATE_ExpansionFactor = 50,
#define ExpansionFactor_DEFAULT 0.06
    PRIVATE_initialRandomSeed = 51
#define initialRandomSeed_DEFAULT 0
};

XWPSCffWriter::XWPSCffWriter()
{
	options = 0;
	strm = 0;
	pfont = 0;
	glyph_data = 0;
	offset_size = 0;
	start_pos = 0;
}

XWPSCffWriter::~XWPSCffWriter()
{
}

bool XWPSCffWriter::convertCharStrings(const XWPSFontBase *pfont)
{
	return (pfont->FontType != ft_encrypted2 &&  (options & WRITE_TYPE2_CHARSTRINGS) != 0);
}

int XWPSCffWriter::extraLenIV(const XWPSFontType1 *pfont)
{
	return (options & WRITE_TYPE2_NO_LENIV ?  qMax(pfont->lenIV, 0) : 0);
}

uint XWPSCffWriter::FDSelectSize()
{
	XWPSFontCid0 *pfontcid0 = (XWPSFontCid0 *)pfont;
  int cid_count = pfontcid0->CIDCount;
  int i = 0; 
  int prev = -1; 
  int num_ranges = 0;
  for (; i < cid_count; ++i) 
  {
		int font_index;
		int code = pfontcid0->glyphData((ulong)(i + ps_min_cid_glyph), NULL, &font_index);

		if (code >= 0) 
		{
	    if (font_index != prev)
				++num_ranges, prev = font_index;
		}
  }
  return qMin(1 + cid_count, 5 + num_ranges * 3);
}

int XWPSCffWriter::glyphSid(ulong glyph)
{
	uint len;
  const uchar *chars = (const uchar *)(pfont->glyphName(glyph, &len));
  if (chars == 0)
		return (int)(XWPSError::RangeCheck);
  return stringSid(chars, len);
}

int XWPSCffWriter::offsetSize(uint offset)
{
	int size = 1;
  while (offset > 255)
  {
		offset >>= 8; 
		++size;
	}
  return size;
}

void XWPSCffWriter::putBool(bool b)
{
	putInt((b ? 1 : 0));
}

void XWPSCffWriter::putBoolValue(bool b, int op)
{
	putBool(b);
  putOp(op);
}

void XWPSCffWriter::putCard16(uint c16)
{
	strm->putc((uchar)(c16 >> 8));
  strm->putc((uchar)c16);
}

int  XWPSCffWriter::putCharString(const uchar *data, uint size, XWPSFontType1 *pfont)
{
	int lenIV = pfont->lenIV;
  XWPSStream *s = strm;
  if (convertCharStrings(pfont)) 
  {
		XWPSString str;
		str.data = (uchar*)data;
		str.size = size;
		int code = pfont->convertType1ToType2(s, &str);
		if (code < 0)
	    return code;
  } 
  else if (lenIV < 0 || !(options & WRITE_TYPE2_NO_LENIV))
		s->putBytes(data, size);
  else if (size >= lenIV) 
  {
		ushort state = crypt_charstring_seed;
		uchar buf[50];
		uint left, n;

		for (left = lenIV; left > 0; left -= n) 
		{
	    n = qMin(left, sizeof(buf));
	    ps_type1_decrypt(buf, data + lenIV - left, n, &state);
		}
		for (left = size - lenIV; left > 0; left -= n) 
		{
	    n = qMin(left, sizeof(buf));
	    ps_type1_decrypt(buf, data + size - left, n, &state);
	    s->putBytes(buf, n);
		}
  }
  return 0;
}

void XWPSCffWriter::putIndex(const XWPSCffStringTable *pcst)
{
	if (pcst->count == 0) 
	{
		putCard16(0);
		return;
  }
  putIndexHeader(pcst->count, pcst->total);
  uint offset = 1;
  for (uint j = 0; j < pcst->count; ++j) 
  {
		offset += pcst->items[j].key.size;
		putOffset(offset);
  }
  for (uint j = 0; j < pcst->count; ++j)
		strm->putBytes(pcst->items[j].key.data, pcst->items[j].key.size);
}

void XWPSCffWriter::putIndexHeader(uint count, uint total)
{
	putCard16(count);
  if (count > 0) 
  {
		offset_size = offsetSize(total + 1);
		strm->putc(offset_size);
		putOffset(1);
  }
}

void XWPSCffWriter::putInt(int i)
{
	XWPSStream *s = strm;

  if (i >= -107 && i <= 107)
		s->putc((uchar)(i + 139));
  else if (i <= 1131 && i >= 0)
		putCard16((c_pos2_0 << 8) + i - 108);
  else if (i >= -1131 && i < 0)
		putCard16((c_neg2_0 << 8) - i - 108);
  else if (i >= -32768 && i <= 32767) 
  {
		s->putc(c2_shortint);
		putCard16(i & 0xffff);
  } 
  else 
  {
		s->putc(CD_LONGINT);
		putCard16(i >> 16);
		putCard16(i & 0xffff);
  }
}

void XWPSCffWriter::putIntIfNe(int i, int i_default, int op)
{
	if (i != i_default)
		putIntValue(i, op);
}

void XWPSCffWriter::putIntValue(int i, int op)
{
	putInt(i);
  putOp(op);
}

void XWPSCffWriter::putOffset(int offset)
{
	for (int i = offset_size - 1; i >= 0; --i)
		strm->putc((uchar)(offset >> (i * 8)));
}

void XWPSCffWriter::putOp(int op)
{
	if (op >= CE_OFFSET) 
	{
		strm->putc(cx_escape);
		strm->putc(op - CE_OFFSET);
  } 
  else
		strm->putc(op);
}

void XWPSCffWriter::putReal(double f)
{
	if (f == (int)f)
		putInt((int)f);
  else
  {
  	char str[50];
		uchar b = 0xff;
		const char *p;

		sprintf(str, "%g", f);
		strm->putc(CD_REAL);
		for (p = str; ; ++p)
		{
			int digit;
			switch (*p)
			{
				case 0:
					goto done;
	    	case '.':
					digit = 0xa; break;
	    	case '-':
					digit = 0xe; break;
	    	case 'e': case 'E':
					if (p[1] == '-')
		    		digit = 0xc, ++p;
					else
		    		digit = 0xb;
					break;
	    	case '0': case '1': case '2': case '3': case '4':
	    	case '5': case '6': case '7': case '8': case '9':
					digit = *p - '0';
					break;
	    	default:	
					digit = 0xd;
					break;
			}
			
			if (b == 0xff)
				b = (digit << 4) + 0xf;
	    else 
	    {
				strm->putc((b & 0xf0) + digit);
				b = 0xff;
			}
		}
		
done:
		strm->putc(b);
  }
}

void XWPSCffWriter::putRealDeltArray(const float *pf, int count, int op)
{
	float prev = 0;
  if (count <= 0)
		return;
  
  for (int i = 0; i < count; ++i) 
  {
		float f = pf[i];

		putReal(f - prev);
		prev = f;
  }
  putOp(op);
}

void XWPSCffWriter::putRealIfNe(double f, double f_default, int op)
{
	if ((float)f != (float)f_default)
		putRealValue(f, op);
}

void XWPSCffWriter::putRealValue(double f, int op)
{
	putReal(f);
  putOp(op);
}

int XWPSCffWriter::putString(const uchar *data, uint size)
{
	int sid = stringSid(data, size);
  if (sid < 0)
		return sid;
  putInt(sid);
  return 0;
}

int XWPSCffWriter::putStringValue(const uchar *data, uint size, int op)
{
	int code = putString(data, size);
  if (code >= 0)
		putOp(op);
  return code;
}

int XWPSCffWriter::stringSid(const uchar *data, uint size)
{
	int index;
  int code = std_strings.index(data, size, false, &index);
  if (code < 0) 
  {
		code = strings.index(data, size, true, &index);
		if (code < 0)
	    return code;
		index += NUM_STD_STRINGS;
  }
  return index;
}

int  XWPSCffWriter::writeCharset(XWPSCffGlyphSubset *pgsub)
{
	strm->putc(0);
  for (int j = 1; j < pgsub->glyphs.subset_size; ++j)
		putCard16(glyphSid(pgsub->glyphs.subset_data[j]));
  return 0;
}

void XWPSCffWriter::writeCharStrings(XWPSPSFGlyphEnum *penum,
		      		                       uint charstrings_count, 
		      		                       uint charstrings_size)
{
	uint ignore_count;
  ulong glyph = ps_no_glyph;
  putIndexHeader(charstrings_count, charstrings_size);
  writeCharStringsOffsets(penum, &ignore_count);
  penum->reset();
  int code = 0;
  for (;(code = penum->enumerateGlyphsNext(&glyph)) != 1; ) 
  {
		XWPSString str;
		XWPSFontType1 *pfd;
		if (code == 0)
		{
			code = glyph_data(pfont, glyph, &str, &pfd);
			if (code >= 0)
	    	putCharString(str.data, str.size, pfd);
	  }
  }
}

int  XWPSCffWriter::writeCharStringsOffsets(XWPSPSFGlyphEnum *penum,  uint *pcount)
{
	penum->reset();
	ulong glyph = ps_no_glyph;
	uint count = 0;
	XWPSString str;
	int offset = 1;
	int code = 0;
	for (;(code = penum->enumerateGlyphsNext(&glyph)) != 1; ++count)
	{
		XWPSFontType1 *pfd = 0;
		int extra_lenIV = 0;
		if (code == 0)
		{
			code = glyph_data(pfont, glyph, &str, &pfd);
			if (code >= 0 && str.size >= (extra_lenIV = extraLenIV(pfd)))
			{
				if (convertCharStrings(pfd)) 
				{
					XWPSStream poss;
					poss.writePositionOnly();
					int ccode = pfd->convertType1ToType2(&poss, &str);
					if (ccode < 0)
		    		return ccode;
					offset += poss.tell();
	    	} 
	    	else
					offset += str.size - extra_lenIV;
			}
		}
		
		putOffset(offset);
	}
	
	*pcount = count;
  return offset - 1;
}

int  XWPSCffWriter::writeCidSet(XWPSPSFGlyphEnum *penum)
{
	ulong glyph;
  int code;

  strm->putc(0);
  penum->reset();
  while ((code = penum->enumerateGlyphsNext(&glyph)) == 0)
		putCard16((uint)(glyph - ps_min_cid_glyph));
  return qMin(code, 0);
}

int  XWPSCffWriter::writeEncoding(XWPSCffGlyphSubset *pgsub)
{
	XWPSStream *s = strm;
	int num_enc = pgsub->num_encoded, num_enc_chars = pgsub->num_encoded_chars;
  uchar used[256], index[256], supplement[256];
  int nsupp = 0;
  s->putc((num_enc_chars > num_enc ? 0x80 : 0));
  memset(used, 0, num_enc);
  s->putc(num_enc);
  for (int j = 0; j < 256; ++j)
  {
  	ulong glyph = pfont->encodeChar((ulong)j, GLYPH_SPACE_NAME);
  	if (glyph == ps_no_glyph || glyph == pgsub->glyphs.notdef)
	    continue;
		int i = psf_sorted_glyphs_index_of(pgsub->glyphs.subset_data + 1, num_enc, glyph);
		if (i < 0)
	    continue;
	    
	  if (used[i])
	    supplement[nsupp++] = j;
		else
	    index[i] = j, used[i] = 1;
  }
  
  s->putBytes(index, num_enc);
  if (nsupp)
  {
  	s->putc(nsupp);
		for (int j = 0; j < nsupp; ++j) 
		{
	    uchar chr = supplement[j];
	    s->putc(chr);
	    putCard16(glyphSid(pfont->encodeChar((ulong)chr, GLYPH_SPACE_NAME)));
		}
  }
  return 0;
}

void XWPSCffWriter::writeFDArrayOffsets(uint *FDArray_offsets, int num_fonts)
{
	putIndexHeader(num_fonts, FDArray_offsets[num_fonts] - FDArray_offsets[0]);
  for (int j = 1; j <= num_fonts; ++j)
		putOffset(FDArray_offsets[j] - FDArray_offsets[0] + 1);
}

int  XWPSCffWriter::writeFDSelect(uint size)
{
	XWPSStream *s = strm;
  XWPSFontCid0 * pfontcid0 = (XWPSFontCid0 *)pfont;
  int cid_count = pfontcid0->CIDCount;
  if (size < 1 + cid_count)
  {
  	s->pputc(3);
		putCard16((size - 5) / 3);
		for (int i = 0, prev = -1; i < cid_count; ++i) 
		{
	    int font_index;
	    int code = pfontcid0->glyphData((ulong)(i + ps_min_cid_glyph), NULL, &font_index);
	    if (code >= 0) 
	    {
				if (font_index != prev) 
				{
		    	putCard16(i);
		    	s->putc((uchar)font_index);
		    prev = font_index;
				}
	    }
		}
		putCard16(cid_count);
  }
  else
  {
  	s->pputc(0);
		for (int i = 0; i < cid_count; ++i) 
		{
	    int font_index = 0;
	    pfontcid0->glyphData((ulong)(i + ps_min_cid_glyph), NULL, &font_index);
	    s->putc((uchar)font_index);
		}
  }
  
  return 0;
}

void XWPSCffWriter::writePrivate(uint Subrs_offset, const XWPSFontType1 *pfont)
{
	putRealDeltArray(pfont->BlueValues.values, pfont->BlueValues.count, PRIVATE_BlueValues);
  putRealDeltArray(pfont->OtherBlues.values, pfont->OtherBlues.count, PRIVATE_OtherBlues);
  putRealDeltArray(pfont->FamilyBlues.values, pfont->FamilyBlues.count, PRIVATE_FamilyBlues);
  putRealDeltArray(pfont->FamilyOtherBlues.values, pfont->FamilyOtherBlues.count, PRIVATE_FamilyOtherBlues);
  if (pfont->StdHW.count > 0)
		putRealValue(pfont->StdHW.values[0], PRIVATE_StdHW);
  if (pfont->StdVW.count > 0)
		putRealValue(pfont->StdVW.values[0], PRIVATE_StdVW);
  if (Subrs_offset)
		putIntValue(Subrs_offset, PRIVATE_Subrs);
  if (pfont->FontType != ft_encrypted) 
  {
		if (pfont->defaultWidthX != defaultWidthX_DEFAULT)
	    putRealValue(fixed2float(pfont->defaultWidthX), PRIVATE_defaultWidthX);
		if (pfont->nominalWidthX != nominalWidthX_DEFAULT)
	    putRealValue(fixed2float(pfont->nominalWidthX), PRIVATE_nominalWidthX);
		putIntIfNe(pfont->initialRandomSeed, initialRandomSeed_DEFAULT, PRIVATE_initialRandomSeed);
  }
  putRealIfNe(pfont->BlueScale, BlueScale_DEFAULT,  PRIVATE_BlueScale);
  putRealIfNe(pfont->BlueShift, BlueShift_DEFAULT,  PRIVATE_BlueShift);
  putIntIfNe(pfont->BlueFuzz, BlueFuzz_DEFAULT,  PRIVATE_BlueFuzz);
  putRealDeltArray(pfont->StemSnapH.values, pfont->StemSnapH.count, PRIVATE_StemSnapH);
  putRealDeltArray(pfont->StemSnapV.values, pfont->StemSnapV.count, PRIVATE_StemSnapV);
  if (pfont->ForceBold != ForceBold_DEFAULT)
		putBoolValue(pfont->ForceBold, PRIVATE_ForceBold);
  if (!(options & WRITE_TYPE2_NO_LENIV))
		putIntIfNe(pfont->lenIV, lenIV_DEFAULT, PRIVATE_lenIV);
  putIntIfNe(pfont->LanguageGroup, LanguageGroup_DEFAULT,  PRIVATE_LanguageGroup);
  putRealIfNe(pfont->ExpansionFactor, ExpansionFactor_DEFAULT, PRIVATE_ExpansionFactor);
}

void XWPSCffWriter::writeROS(const XWPSCidSystemInfo *pcidsi)
{
	putString(pcidsi->Registry.data, pcidsi->Registry.size);
  putString(pcidsi->Ordering.data, pcidsi->Ordering.size);
  putIntValue(pcidsi->Supplement, TOP_ROS);
}

void XWPSCffWriter::writeSubrs(uint subrs_count, 
	                             uint subrs_size,
		                           XWPSFontType1 *pfont)
{
	int j = 0;
  uint ignore_count;
  XWPSString str;
  int code = 0;

  putIndexHeader(subrs_count, subrs_size);
  writeSubrsOffsets(&ignore_count, pfont);
  for (;(code = pfont->subrData(j, false, &str)) !=  XWPSError::RangeCheck; ++j) 
  {
		if (code >= 0)
	    putCharString(str.data, str.size, pfont);
  }
}

uint XWPSCffWriter::writeSubrsOffsets(uint *pcount, XWPSFontType1 *pfont)
{
	int extra_lenIV = extraLenIV(pfont);
	int offset = 1;
	int j = 0; int code;
	XWPSString str;
	for (;(code = pfont->subrData(j, false, &str)) != XWPSError::RangeCheck; ++j) 
	{
		if (code >= 0 && str.size >= extra_lenIV)
	    offset += str.size - extra_lenIV;
		putOffset(offset);
  }
  *pcount = j;
  return offset - 1;
}

void XWPSCffWriter::writeTopCidFont(uint charset_offset,
		      						              uint CharStrings_offset, 
		      						              uint FDSelect_offset,
		                                uint Font_offset)
{
	XWPSFontBase *pbfont = (XWPSFontBase *)pfont;
  XWPSFontCid0 *pfont = (XWPSFontCid0 *)pbfont;

  writeROS(&(pfont->CIDSystemInfo));
  writeTopCommon(pbfont, true);
  putIntIfNe(charset_offset, charset_DEFAULT, TOP_charset);
  putIntValue(CharStrings_offset, TOP_CharStrings);
  putIntIfNe(pfont->CIDCount, CIDCount_DEFAULT, TOP_CIDCount);
  putIntValue(Font_offset, TOP_FDArray);
  putIntValue(FDSelect_offset, TOP_FDSelect);
}

void XWPSCffWriter::writeTopCommon(XWPSFontBase *pbfont,  bool full_info)
{
	XWPSFontInfo info;
  info.Flags_requested = FONT_IS_FIXED_WIDTH;
  info.ItalicAngle = ItalicAngle_DEFAULT;
  info.UnderlinePosition = UnderlinePosition_DEFAULT;
  info.UnderlineThickness = UnderlineThickness_DEFAULT;
  pbfont->fontInfo(NULL,
								(full_info ?
			 					FONT_INFO_FLAGS | FONT_INFO_ITALIC_ANGLE |
			 					FONT_INFO_UNDERLINE_POSITION |
			 					FONT_INFO_UNDERLINE_THICKNESS : 0) |
								(FONT_INFO_COPYRIGHT | FONT_INFO_NOTICE |
			 					FONT_INFO_FAMILY_NAME | FONT_INFO_FULL_NAME),
				  			&info);
	if (info.members & FONT_INFO_NOTICE)
		putStringValue(info.Notice.data, info.Notice.size, TOP_Notice);
  if (info.members & FONT_INFO_FULL_NAME)
		putStringValue(info.FullName.data, info.FullName.size, TOP_FullName);
  if (info.members & FONT_INFO_FAMILY_NAME)
		putStringValue(info.FamilyName.data, info.FamilyName.size, TOP_FamilyName);
		
	{
		putReal(pbfont->FontBBox.p.x);
		putReal(pbfont->FontBBox.p.y);
		putReal(pbfont->FontBBox.q.x);
		putReal(pbfont->FontBBox.q.y);
		putOp(TOP_FontBBox);
  }
  
  if (pbfont->UID.isUniqueID())
		putIntValue(pbfont->UID.id, TOP_UniqueID);
  else if (pbfont->UID.isXUID())
  {
  	long * v = pbfont->UID.XUIDValues();
  	for (int j = 0; j < pbfont->UID.XUIDSize(); ++j)
	    putInt(v[j]);
		putOp(TOP_XUID);
  }
  
  if (!(options & WRITE_TYPE2_AR3)) 
  {
		if (info.members & FONT_INFO_COPYRIGHT)
	    putStringValue(info.Copyright.data, info.Copyright.size, TOP_Copyright);
		if (info.Flags & info.Flags_returned & FONT_IS_FIXED_WIDTH)
	    putBoolValue(true, TOP_isFixedPitch);
		putRealIfNe(info.ItalicAngle, ItalicAngle_DEFAULT, TOP_ItalicAngle);
		putIntIfNe(info.UnderlinePosition, UnderlinePosition_DEFAULT, TOP_UnderlinePosition);
		putIntIfNe(info.UnderlineThickness, UnderlineThickness_DEFAULT, TOP_UnderlineThickness);
		putIntIfNe(pbfont->PaintType, PaintType_DEFAULT, TOP_PaintType);
  }
  
  {
  	XWPSMatrix fm_default(0.001, 0, 0, 0.001, 0, 0);
  	if (pbfont->FontMatrix.xx != fm_default.xx ||
	      pbfont->FontMatrix.xy != 0 || pbfont->FontMatrix.yx != 0 ||
	      pbfont->FontMatrix.yy != fm_default.yy ||
	      pbfont->FontMatrix.tx != 0 || pbfont->FontMatrix.ty != 0) 
	  {
	    putReal(pbfont->FontMatrix.xx);
	    putReal(pbfont->FontMatrix.xy);
	    putReal(pbfont->FontMatrix.yx);
	    putReal(pbfont->FontMatrix.yy);
	    putReal(pbfont->FontMatrix.tx);
	    putReal(pbfont->FontMatrix.ty);
	    putOp(TOP_FontMatrix);
		}
  }
  putRealIfNe(pbfont->StrokeWidth, StrokeWidth_DEFAULT, TOP_StrokeWidth);
}

void XWPSCffWriter::writeTopFDArray(XWPSFontBase *pbfont,
		      						              uint Private_offset, 
		      						              uint Private_size)
{
	PSFontName *pfname = &(pbfont->font_name);
  writeTopCommon(pbfont, false);
  putInt(Private_size);
  putIntValue(Private_offset, TOP_Private);
  if (pfname->size == 0)
		pfname = &(pbfont->key_name);
  if (pfname->size) 
  {
		putString(pfname->chars, pfname->size);
		putOp(TOP_FontName);
  }
}

void XWPSCffWriter::writeTopFont(uint Encoding_offset,
		   						               uint charset_offset, 
		   						               uint CharStrings_offset,
		                             uint Private_offset, 
		                             uint Private_size)
{
	writeTopCommon(pfont, true);
  putInt(Private_size);
  putIntValue(Private_offset, TOP_Private);
  putIntValue(CharStrings_offset, TOP_CharStrings);
  putIntIfNe(charset_offset, charset_DEFAULT, TOP_charset);
  putIntIfNe(Encoding_offset, Encoding_DEFAULT, TOP_Encoding);
  {
		int type = (options & WRITE_TYPE2_CHARSTRINGS ? 2 : pfont->FontType == ft_encrypted2 ? 2 : 1);
		putIntIfNe(type, CharstringType_DEFAULT, TOP_CharstringType);
  }
}

const PSType1DataProcs z1_data_procs = {
    &XWPSFontType1::z1glyphData, 
    &XWPSFontType1::z1subrData, 
    &XWPSFontType1::z1seacData, 
    &XWPSContextState::z1push, 
    &XWPSContextState::z1pop
};

XWPSFontType1::XWPSFontType1()
	:XWPSFontBase()
{
	proc_data = 0;
	lenIV = 0;
	subroutineNumberBias = 0;
	gsubrNumberBias = 0;
	initialRandomSeed = 0;
	defaultWidthX = 0;
  nominalWidthX = 0;
  BlueFuzz = 0;
  BlueScale = 0.0;
  BlueShift = 0.0;
  
  BlueValues.count = 0;
  for (int i = 0; i < max_BlueValues*2; i++)
  	BlueValues.values[i] = 0.0;
  	
  ExpansionFactor = 0.0;
  ForceBold = false;
  
  FamilyBlues.count = 0;
  for (int i = 0; i < max_FamilyBlues*2; i++)
  	FamilyBlues.values[i] = 0.0;
  	
  FamilyOtherBlues.count = 0;
  for (int i = 0; i < max_FamilyOtherBlues*2; i++)
  	FamilyOtherBlues.values[i] = 0.0;
  	
  LanguageGroup = 0;
  
  OtherBlues.count = 0;
  for (int i = 0; i < max_OtherBlues*2; i++)
  	OtherBlues.values[i] = 0.0;
  	
  RndStemUp = false;
  
  StdHW.count = 0;
  StdHW.values[0] = 0.0;
  
  StdVW.count = 0;
  StdVW.values[0] = 0.0;
  
  StemSnapH.count = 0;
  for (int i = 0; i < max_StemSnap; i++)
  	StemSnapH.values[i] = 0.0;
  	
  StemSnapV.count = 0;
  for (int i = 0; i < max_StemSnap; i++)
  	StemSnapV.values[i] = 0.0;
  	
  WeightVector.count = 0;
  for (int i = 0; i < max_WeightVector; i++)
  	WeightVector.values[i] = 0.0;
}

XWPSFontType1::~XWPSFontType1()
{
}

int XWPSFontType1::callInterpret(XWPSType1State * st, const XWPSString * str, int *pindex)
{
	return (st->*interpret)(str, pindex);
}

uint XWPSFontType1::cffEncodingSize(int num_encoded, int num_encoded_chars)
{
	return 2 + num_encoded +(num_encoded_chars > num_encoded ? 1 + (num_encoded_chars - num_encoded) * 3 : 0);
}

uint XWPSFontType1::cffIndexSize(uint count, uint total)
{
	return (count == 0 ? 2 :  3 + XWPSCffWriter::offsetSize(total + 1) * (count + 1) + total);
}

int  XWPSFontType1::charstringFontInit(XWPSCharstringFontRefs *pfr)
{
  XWPSFontData * pdata = (XWPSFontData*)(client_data);
  
  pdata->u.type1.OtherSubrs->assign(pfr->OtherSubrs);
  pdata->u.type1.Subrs->assign(pfr->Subrs);
  pdata->u.type1.GlobalSubrs->assign(pfr->GlobalSubrs);
  type1Procs = z1_data_procs;
  procs.font_info_.font_infot1 = &XWPSFontType1::z1fontInfo;
  procs.same_font_.same_fontt1 = &XWPSFontType1::z1sameFont;
  procs.glyph_info_.glyph_infot1 = &XWPSFontType1::z1glyphInfo;
  procs.enumerate_glyph_.enumerate_glypht1 = &XWPSFontType1::z1enumerateGlyph;
  procs.glyph_outline_.glyph_outlinet1 = &XWPSFontType1::zchar1GlyphOutline;
  return 0;
}

int XWPSFontType1::convertType1ToType2(XWPSStream *s, const XWPSString *pstr)
{
	XWPSType1State cis;
  bool first = true;
  bool replace_hints = false;
  bool hints_changed = false;
  uchar active_hints[(max_total_stem_hints + 7) / 8];
  uchar dot_save_hints[(max_total_stem_hints + 7) / 8];
  uint hintmask_size;
  int depth;
  int prev_op;
  
  cis.type1NextInit(pstr, this);
  for (;;)
  {
  	int c = cis.type1Next();
		long *csp = &cis.ostack[cis.os_count - 1];
		switch (c)
		{
			default:
	    	if (c < 0)
					return c;
	    	cis.type1Clear();
	    	continue;
	    	
			case cx_hstem:
	    	cis.type1Stem1(&cis.hstem_hints, csp - 1, NULL);
	    	goto clear;
	    	
			case cx_vstem:
	    	cis.type1Stem1(&cis.vstem_hints, csp - 1, NULL);
	    	goto clear;
	    	
			case CE_OFFSET + ce1_vstem3:
	    	cis.type1Stem3(&cis.vstem_hints, csp - 5, NULL);
	    	goto clear;
	    	
			case CE_OFFSET + ce1_hstem3:
	    	cis.type1Stem3(&cis.hstem_hints, csp - 5, NULL);
	    	
clear:
	    	cis.type1Clear();
	    	continue;
	    	
			case ce1_callothersubr:
	    	if (*csp == int2fixed(3))
					replace_hints = true;
	    	cis.os_count -= 2;
	    	continue;
	    	
			case CE_OFFSET + ce1_dotsection:
	    	replace_hints = true;
	    	continue;
	    	
			case CE_OFFSET + ce1_seac:
			case cx_endchar:
	    	break;
		}
		break;
  }
  
  for (int i = 0; i < cis.hstem_hints.count; ++i)
	   cis.hstem_hints.data[i].index = i;
	for (int i = 0; i < cis.vstem_hints.count; ++i)
	   cis.vstem_hints.data[i].index = i + cis.hstem_hints.count;
	   
	if (replace_hints) 
	{
		hintmask_size =  (cis.hstem_hints.count + cis.vstem_hints.count + 7) / 8;
		memset(active_hints, 0, hintmask_size);
  } 
  else 
		hintmask_size = 0;
		
	cis.type1NextInit(pstr, this);
	depth = 0; 
	prev_op = -1;
	
	for (;;)
	{
		int c = cis.type1Next();
		long *csp = &cis.ostack[cis.os_count - 1];
		long mx, my;
		switch (c)
		{
			default:
	    	if (c < 0)
					return c;
	    	if (c >= CE_OFFSET)
					return (int)(XWPSError::RangeCheck);
						
copy:
				if (prev_op >= 0) 
				{
					type2PutOp(s, prev_op);
					depth = 0; 
					prev_op = -1;
    		}
    		
    		if (hints_changed) 
    		{
					type2PutHintMask(s, active_hints, hintmask_size);
					hints_changed = false;
    		}
    		
put:
	    	for (int i = 0; i < cis.os_count; ++i)
					type2PutFixed(s, cis.ostack[i]);
	    	depth += cis.os_count;
	    	prev_op = c;
	    	cis.type1Clear();
	    	continue;
	    	
	    case cx_hstem:
	    	cis.type1Stem1(&cis.hstem_hints, csp - 1, active_hints);
	    	
hint:
				hints_changed = replace_hints;
				if (hints_changed)
				{
					depth = 0; 
					prev_op = -1;
				}
				cis.type1Clear();
	    	continue;
	    
			case cx_vstem:
	    	cis.type1Stem1(&cis.vstem_hints, csp - 1, active_hints);
	    	goto hint;
	    
			case CE_OFFSET + ce1_vstem3:
	    	cis.type1Stem3(&cis.vstem_hints, csp - 5, active_hints);
	    	goto hint;
	    	
			case CE_OFFSET + ce1_hstem3:
	    	cis.type1Stem3(&cis.hstem_hints, csp - 5, active_hints);
	    	goto hint;
	    	
			case CE_OFFSET + ce1_dotsection:
	    	if (cis.dotsection_flag == dotsection_out) 
	    	{
					memcpy(dot_save_hints, active_hints, hintmask_size);
					memset(active_hints, 0, hintmask_size);
					cis.dotsection_flag = dotsection_in;
	    	} 
	    	else 
	    	{
					memcpy(active_hints, dot_save_hints, hintmask_size);
					cis.dotsection_flag = dotsection_out;
	    	}
	    	hints_changed = replace_hints;
	    	if (hints_changed)
	    	{
	    		depth = 0; 
					prev_op = -1;
	    	}
	    	continue;
	    	
	    case c1_closepath:
				case CE_OFFSET + ce1_setcurrentpoint:
	    	continue;
	    	
			case cx_vmoveto:
	    	mx = 0; 
	    	my = *csp;
	    	csp -= 1; 
	    	cis.os_count -= 1;
	    	goto move;
	    	
			case cx_hmoveto:
	    	mx = *csp; 
	    	my = 0;
	    	csp -= 1; 
	    	cis.os_count -= 1;
	    	goto move;
			case cx_rmoveto:
	    	mx = csp[-1], my = *csp;
	    	csp -= 2; 
	    	cis.os_count -= 2;
	    	
move:
				if (prev_op >= 0)
				{
					type2PutOp(s, prev_op);
					depth = 0; 
					prev_op = -1;
				}
				if (first) 
				{
					if (cis.os_count)
		    		type2PutFixed(s, *csp);
					mx += cis.lsb.x; 
					my += cis.lsb.y;
					first = false;
	    	}
	    	if (cis.flex_count != flex_max) 
	    	{
					if (cis.type1Next() != ce1_callothersubr)
		    		return (int)(XWPSError::RangeCheck);
					csp = &cis.ostack[cis.os_count - 1];
					if (*csp != int2fixed(2) || csp[-1] != fixed_0)
		    		return (int)(XWPSError::RangeCheck);
					cis.flex_count++;
					csp[-1] = mx, *csp = my;
					continue;
	    	}
	    	if (hints_changed) 
    		{
					type2PutHintMask(s, active_hints, hintmask_size);
					hints_changed = false;
    		}
	    	if (mx == 0) 
	    	{
					type2PutFixed(s, my);
					depth = 1;
					prev_op = cx_vmoveto;
	    	} 
	    	else if (my == 0) 
	    	{
					type2PutFixed(s, mx);
					depth = 1; 
					prev_op = cx_hmoveto;
	    	} 
	    	else 
	    	{
					type2PutFixed(s, mx);
					type2PutFixed(s, my);
					depth = 2; 
					prev_op = cx_rmoveto;
	    	}
	    	cis.type1Clear();
	    	continue;
	    	
			case c1_hsbw:
	    	cis.type1Sbw(cis.ostack[0], fixed_0, cis.ostack[1], fixed_0);
	    	cis.ostack[0] = cis.ostack[1];
sbw:
				if (cis.ostack[0] == defaultWidthX)
					cis.os_count = 0;
	    	else 
	    	{
					cis.ostack[0] -= nominalWidthX;
					cis.os_count = 1;
	    	}
	    	if (cis.hstem_hints.count) 
	    	{
					if (cis.os_count)
		    		type2PutFixed(s, cis.ostack[0]);
					cis.os_count = 0;
					type2PutStems(s, &cis.hstem_hints, (replace_hints ? c2_hstemhm : cx_hstem));
	    	}
	    	if (cis.vstem_hints.count) 
	    	{
					if (cis.os_count)
		    		type2PutFixed(s, cis.ostack[0]);
					cis.os_count = 0;
					type2PutStems(s, &cis.vstem_hints,	(replace_hints ? c2_vstemhm : cx_vstem));
	    	}
	    	continue;
			case CE_OFFSET + ce1_seac:
				csp[-3] += cis.lsb.x - csp[-4];
	    	memmove(csp - 4, csp - 3, sizeof(*csp) * 4);
	    	csp -= 1; 
	    	cis.os_count -= 1;
	    case cx_endchar:
	    	if (prev_op >= 0)
				{
					type2PutOp(s, prev_op);
					depth = 0; 
					prev_op = -1;
				}
	    	for (int i = 0; i < cis.os_count; ++i)
					type2PutFixed(s, cis.ostack[i]);
	    	type2PutOp(s, cx_endchar);
	    	return 0;
	    	
			case CE_OFFSET + ce1_sbw:
	    	cis.type1Sbw(cis.ostack[0], cis.ostack[1],cis.ostack[2], cis.ostack[3]);
	    	cis.ostack[0] = cis.ostack[2];
	    	goto sbw;
	    	
			case ce1_callothersubr:
	    	if (prev_op >= 0)
				{
					type2PutOp(s, prev_op);
					depth = 0; 
					prev_op = -1;
				}
				switch (fixed2int_var(*csp))
				{
					default:
						return (int)(XWPSError::RangeCheck);
						
	    		case 0:
	    			csp[-18] += csp[-16], csp[-17] += csp[-15];
						memmove(csp - 16, csp - 14, sizeof(*csp) * 11);
						cis.os_count -= 6; 
						csp -= 6;
						c = CE_OFFSET + ce2_flex;
						cis.flex_count = flex_max;
						cis.ignore_pops = 2;
						goto copy;
						
	    		case 1:
						cis.flex_count = 0;
						cis.os_count -= 2;
						continue;
						
					case 3:
						memset(active_hints, 0, hintmask_size);
						hints_changed = replace_hints;
						if (hints_changed)
						{
							depth = 0; 
							prev_op = -1;
						}
						cis.ignore_pops = 1;
						cis.os_count -= 2;
						continue;
						
	    		case 12:
	    		case 13:
	    			cis.os_count -= 2 + fixed2int(csp[-1]);
						continue;
				}
				
			case cx_rlineto:
	    	if (depth > MAX_STACK - 2)
					goto copy;
				switch (prev_op)
				{
					case cx_rlineto:
						goto put;
					case cx_rrcurveto:
						c = c2_rcurveline;
						goto put;
	    		default:
						goto copy;
				}
				
			case cx_hlineto:
				if (depth > MAX_STACK - 1 || prev_op != (depth & 1 ? cx_vlineto : cx_hlineto))
					goto copy;
	    	c = prev_op;
	    	goto put;
	    	
	    case cx_vlineto:
	    	if (depth > MAX_STACK - 1 ||	prev_op != (depth & 1 ? cx_hlineto : cx_vlineto))
					goto copy;
	    	c = prev_op;
	    	goto put;
	    	
	    case cx_hvcurveto:
	    	if ((depth & 1) || depth > MAX_STACK - 4 ||	prev_op != (depth & 4 ? cx_vhcurveto : cx_hvcurveto))
					goto copy;
	    	c = prev_op;
	    	goto put;
	    	
	    case cx_vhcurveto:
	    	if ((depth & 1) || depth > MAX_STACK - 4 ||	prev_op != (depth & 4 ? cx_hvcurveto : cx_vhcurveto))
					goto copy;
	    	c = prev_op;
	    	goto put;
	    	
	    case cx_rrcurveto:
	    	if (depth == 0)
	    	{
	    		if (csp[-1] == 0)
	    		{
	    			c = c2_vvcurveto;
		    		csp[-1] = csp[0];
		    		if (csp[-5] == 0) 
		    		{
							memcpy(csp - 5, csp - 4, sizeof(*csp) * 4);
							csp -= 2; 
	    				cis.os_count -= 2;
		    		} 
		    		else
		    		{
							csp -= 1; 
	    				cis.os_count -= 1;
						}
	    		}
	    		else if (*csp == 0)
	    		{
	    			c = c2_hhcurveto;
		    		if (csp[-4] == 0) 
		    		{
							memcpy(csp - 4, csp - 3, sizeof(*csp) * 3);
							csp -= 2; 
	    				cis.os_count -= 2;
		    		} 
		    		else 
		    		{
							*csp = csp[-5]; 
							csp[-5] = csp[-4]; 
							csp[-4] = *csp;
							csp -= 1; 
	    				cis.os_count -= 1;
		    		}
	    		}
	    		goto copy;
	    	}
	    	if (depth > MAX_STACK - 6)
					goto copy;
				switch (prev_op)
				{
					case c2_hhcurveto:
						if (csp[-4] == 0 && *csp == 0) 
						{
		    			memcpy(csp - 4, csp - 3, sizeof(*csp) * 3);
		    			c = prev_op;
		    			csp -= 2; 
	    				cis.os_count -= 2;
		    			goto put;
						}
						goto copy;
						
					case c2_vvcurveto:
						if (csp[-5] == 0 && csp[-1] == 0) 
						{
		    			memcpy(csp - 5, csp - 4, sizeof(*csp) * 3);
		    			csp[-2] = *csp;
		    			c = prev_op;
		    			csp -= 2; 
	    				cis.os_count -= 2;
		    			goto put;
						}
						goto copy;
						
					case cx_hvcurveto:
						if (depth & 1)
		    			goto copy;
						if (!(depth & 4))
		    			goto hrc;
		    			
vrc:
						if (csp[-5] != 0)
		    			goto copy;
						memcpy(csp - 5, csp - 4, sizeof(*csp) * 5);
						c = prev_op;
						csp -= 1; 
	    			cis.os_count -= 1;
						goto put;
						
	    		case cx_vhcurveto:
						if (depth & 1)
		    			goto copy;
						if (depth & 4)
		    			goto vrc;
		    			
hrc:
						if (csp[-4] != 0)
		    			goto copy;		    			
		    		memcpy(csp - 4, csp - 3, sizeof(*csp) * 2);
						csp[-2] = *csp;
						c = prev_op;
						csp -= 1; 
	    			cis.os_count -= 1;
						goto put;
						
					case cx_rlineto:
						c = c2_rlinecurve;
						goto put;
						
					case cx_rrcurveto:
						goto put;
						
	    		default:
						goto copy;
				}
		}
	}
}

XWPSFont * XWPSFontType1::copyFont()
{
	XWPSFontType1 * newfont = new XWPSFontType1;
	copyFontType1(newfont);
	return newfont;
}

void XWPSFontType1::copyFontType1(XWPSFontType1 * newfont)
{
	copyFontBase(newfont);
	newfont->type1Procs = type1Procs;
	newfont->interpret = interpret;
	newfont->proc_data = proc_data;
	newfont->lenIV = lenIV;
	newfont->subroutineNumberBias = subroutineNumberBias;
	newfont->gsubrNumberBias = gsubrNumberBias;
	newfont->initialRandomSeed = initialRandomSeed;
	newfont->defaultWidthX = defaultWidthX;
	newfont->nominalWidthX = nominalWidthX;
	newfont->BlueFuzz = BlueFuzz;
	newfont->BlueScale = BlueScale;
	newfont->BlueShift = BlueShift;
	newfont->BlueValues = BlueValues;
	newfont->ExpansionFactor = ExpansionFactor;
	newfont->ForceBold = ForceBold;
	newfont->FamilyBlues = FamilyBlues;
	newfont->FamilyOtherBlues = FamilyOtherBlues;
	newfont->LanguageGroup = LanguageGroup;
	newfont->OtherBlues = OtherBlues;
	newfont->RndStemUp = RndStemUp;
	newfont->StdHW = StdHW;
	newfont->StdVW = StdVW;
	newfont->StemSnapH = StemSnapH;
	newfont->StemSnapV = StemSnapV;
	newfont->WeightVector = WeightVector;
}

int XWPSFontType1::getLength()
{
	return sizeof(XWPSFontType1);
}

const char * XWPSFontType1::getTypeName()
{
	return "fonttype1";
}

int  XWPSFontType1::getType1Glyphs(XWPSPSFOutlineGlyphs *pglyphs, 
	                                 ulong *subset_glyphs, 
	                                 uint subset_size)
{
	return getOutlineGlyphs(pglyphs,  subset_glyphs, subset_size, psf_type1_glyph_data);
}

int XWPSFontType1::glyphData(ulong glyph, XWPSString * pgdata)
{
	return (this->*(type1Procs.glyph_data))(glyph, pgdata);
}

int XWPSFontType1::seacData(int ccode, ulong * pglyph, XWPSString *pstr)
{
	return (this->*(type1Procs.seac_data))(ccode, pglyph, pstr);
}

int XWPSFontType1::subrData(int subr_num, bool global,	XWPSString * psdata)
{
	return (this->*(type1Procs.subr_data))(subr_num, global, psdata);
}

int  XWPSFontType1::type1GlyphInfo(ulong glyph, 
	                    const XWPSMatrix *pmat,
		                  int members, 
		                  XWPSGlyphInfo *info)
{
	int piece_members = members & (GLYPH_INFO_NUM_PIECES | GLYPH_INFO_PIECES);
  int width_members = members & (GLYPH_INFO_WIDTH0 << WMode);
  int default_members = members - (piece_members + width_members);
  int code = 0;
  XWPSString str;

  if (default_members) 
  {
		code = glyphInfoDefault(glyph, pmat, default_members, info);

		if (code < 0)
	    return code;
  } 
  else
		info->members = 0;

	int gcode = 0;
  if (default_members != members) 
  {
		if ((code = glyphData(glyph, &str)) < 0)
	    return code;
	    
	  gcode = code;
  }
	
  if (piece_members) 
  {
		ulong *pieces =  (members & GLYPH_INFO_PIECES ? info->pieces : 0);
		bool encrypted = lenIV >= 0;
		long cstack[ostack_size];
		long *csp;
		XWPSIPState ipstack[ipstack_size + 1];
		XWPSIPState *ipsp = &ipstack[0];
		uchar *cip;
		ushort state;
		int c;
    
		CLEAR_CSTACK(cstack, csp);
		info->num_pieces = 0;	
		cip = str.data;
    
call:
		state = crypt_charstring_seed;
		if (encrypted) 
		{
	    int skip = lenIV;
	    for (; skip > 0; ++cip, --skip)
				decrypt_skip_next(*cip, state);
		}
    
top:
		for (;;) 
		{
	    uint c0 = *cip++;

	    charstring_next(c0, state, c, encrypted);
	    if (c >= c_num1) 
	    {
				if (c < c_pos2_0) 
				{
		    	decode_push_num1(csp, c);
				} 
				else if (c < cx_num4) 
				{
		    	decode_push_num2(csp, c, cip, state, encrypted);
				} 
				else if (c == cx_num4) 
				{
		    	long lw;

		    	decode_num4(lw, cip, state, encrypted);
		    	*++csp = int2fixed(lw);
				} 
				else
				{
		    	return (int)(XWPSError::InvalidFont);
		    }
				continue;
	    }
#define cnext CLEAR_CSTACK(cstack, csp); goto top
	   	
	   	switch ((PSCharCommand) c) 
	   	{
	    	default:
					goto out;
					
	    	case c_callsubr:
					c = fixed2int_var(*csp);
					code = subrData(c, false, &ipsp[1].char_string);
					if (code < 0)
		    		return code;
					--csp;
					ipsp->ip = cip, ipsp->dstate = state;
					++ipsp;
					cip = ipsp->char_string.data;
					goto call;
					
	    	case c_return:
					--ipsp;
					cip = ipsp->ip, state = ipsp->dstate;
					goto top;
					
	    	case cx_hstem:
	    	case cx_vstem:
	    	case c1_hsbw:
					cnext;
					
	    	case cx_endchar:
					if (csp < cstack + 3)
		    		goto out;

do_seac:
					if (pieces) 
					{
		    		ulong bchar = fixed2int(csp[-1]);
		    		ulong achar = fixed2int(csp[0]);
		    		int bcode = seacData(bchar, &pieces[0], NULL);
		    		int acode =	seacData(achar,  &pieces[1], NULL);

		    		code = (bcode < 0 ? bcode : acode);
					}
					info->num_pieces = 2;
					goto out;
					
	    	case cx_escape:
					charstring_next(*cip, state, c, encrypted);
					++cip;
					switch ((PSChar1ExtendedCommand) c) 
					{
						default:
		    			goto out;
		    			
						case ce1_vstem3:
						case ce1_hstem3:
						case ce1_sbw:
		    			cnext;
		    			
						case ce1_pop:
		    			goto top;
		    			
						case ce1_seac:
		    			goto do_seac;
		    			
						case ce1_callothersubr:
		    			switch (fixed2int_var(*csp)) 
		    			{
		    				default:
									goto out;
									
		    				case 3:
									csp -= 2;
									goto top;
									
		    				case 12:
		    				case 13:
		    				case 14:
		    				case 15:
		    				case 16:
		    				case 17:
		    				case 18:
									cnext;
		    			}
					}
	    	}
#undef cnext
		}
out:	
		info->members |= piece_members;
  }

  if (width_members) 
  {
		XWPSImagerState gis;
		gis.i_ctx_p = i_ctx_p;
		XWPSType1State cis;
		int value;

		if (pmat)
	    gis.ctm.fromMatrix((XWPSMatrix*)pmat);
		else 
		{
	    XWPSMatrix imat;

	    gis.ctm.fromMatrix(&imat);
		}
		gis.flatness = 0;
		XWPSLog2ScalePoint no_scale;
		code = cis.type1InterpInit(&gis, NULL , &no_scale, true, 0, this);
		if (code < 0)
	    return code;
		cis.charpath_flag = true;
		code = callInterpret(&cis, &str, &value);
		switch (code) 
		{
			case 0:
	    	code = (int)(XWPSError::InvalidFont);
	    		
			default:
	    	return code;
			case type1_result_callothersubr:
	    	return (int)(XWPSError::RangeCheck);
	    	
			case type1_result_sbw:
	    	info->width[WMode].x = fixed2float(cis.width.x);
	    	info->width[WMode].y = fixed2float(cis.width.y);
	    	break;
		}
		info->members |= width_members;
  }

  return code;
}

void XWPSFontType1::type2PutFixed(XWPSStream *s,long v)
{
	if (fixed_is_int(v))
		type2PutInt(s, fixed2int_var(v));
  else if (v >= int2fixed(-32768) && v < int2fixed(32768)) 
  {
		s->pputc(cx_num4);
		s->putc4(v << (16 - _fixed_shift));
  } 
  else 
  {
		type2PutInt(s, fixed2int_var(v));
		type2PutFixed(s, fixed_fraction(v));
		type2PutOp(s, CE_OFFSET + ce2_add);
  }
}

void XWPSFontType1::type2PutHintMask(XWPSStream *s, const uchar *mask, uint size)
{
	uint ignore;
  type2PutOp(s, c2_hintmask);
  s->puts(mask, size, &ignore);
}

void XWPSFontType1::type2PutInt(XWPSStream *s, int i)
{
	if (i >= -107 && i <= 107)
		s->putc((uchar)(i + 139));
  else if (i <= 1131 && i >= 0)
		s->putc2((c_pos2_0 << 8) + i - 108);
  else if (i >= -1131 && i < 0)
		s->putc2((c_neg2_0 << 8) - i - 108);
  else if (i >= -32768 && i <= 32767) 
  {
		s->pputc(c2_shortint);
		s->putc2(i);
  } 
  else 
  {
		type2PutInt(s, i >> 10);
		type2PutInt(s, 1024);
		type2PutOp(s, CE_OFFSET + ce2_mul);
		type2PutInt(s, i & 1023);
		type2PutOp(s, CE_OFFSET + ce2_add);
  }
}

void XWPSFontType1::type2PutOp(XWPSStream *s, int op)
{
	if (op >= CE_OFFSET) 
	{
		s->pputc(cx_escape);
		s->pputc(op - CE_OFFSET);
  } 
  else
		s->putc(op);
}

void XWPSFontType1::type2PutStems(XWPSStream *s, const XWPSStemHintTable *psht, int op)
{
	long prev = 0;
  int pushed = 0;
  for (int i = 0; i < psht->count; ++i, pushed += 2) 
  {
		long v0 = psht->data[i].v0;
		long v1 = psht->data[i].v1;

		if (pushed > ostack_size - 2) 
		{
	    type2PutOp(s, op);
	    pushed = 0;
		}
		type2PutFixed(s, v0 - prev);
		type2PutFixed(s, v1 - v0);
		prev = v1;
  }
  type2PutOp(s, op);
}

int XWPSFontType1::writeType1Font(XWPSStream *s, 
	                                int options,
		                              ulong * orig_subset_glyphs, 
		                              uint orig_subset_size,
		                              const XWPSString *alt_font_name, 
		                              int * lengths)
{
	XWPSStream *es = s;
  long start = s->tell();
  bool e = false;
  XWPSPSFOutlineGlyphs glyphs;
  int lenIVA = lenIV;
  int code = getType1Glyphs(&glyphs, orig_subset_glyphs, orig_subset_size);
  if (code < 0)
		return code;
  
  PSPrinterParams ppp = {0, 0, 0, " def\n", 0};
  ppp.print_ok = (options & WRITE_TYPE1_ASCIIHEX ? 0 : PRINT_BINARY_OK) |	PRINT_HEX_NOT_OK;
  XWPSPrinterParamList rlist(s, &ppp);
  s->pputs("%!FontType1-1.0: ");
  writeFontName(s, alt_font_name);
  s->pputs("\n11 dict begin\n");
  s->pputs("/FontInfo 5 dict dup begin");
  {
  	XWPSFontInfo info;
		code = fontInfo(NULL,	(FONT_INFO_COPYRIGHT | FONT_INFO_NOTICE |	 FONT_INFO_FAMILY_NAME | FONT_INFO_FULL_NAME), &info);
		if (code >= 0) 
		{
	    s->writeFontInfo("Copyright", &info.Copyright, info.members & FONT_INFO_COPYRIGHT);
	    s->writeFontInfo("Notice", &info.Notice, info.members & FONT_INFO_NOTICE);
	    s->writeFontInfo("FamilyName", &info.FamilyName, info.members & FONT_INFO_FAMILY_NAME);
	    s->writeFontInfo("FullName", &info.FullName, info.members & FONT_INFO_FULL_NAME);
		}
  }  
  s->pputs("\nend readonly def\n");
  s->pputs("/FontName /");
  writeFontName(s, alt_font_name);
  s->pputs(" def\n");
  code = writeEncoding(s, options, glyphs.subset_glyphs, glyphs.subset_size, glyphs.notdef);
  if (code < 0)
		return code;
  s->print("/FontMatrix [%g %g %g %g %g %g] readonly def\n",
	     				FontMatrix.xx, FontMatrix.xy,
	     				FontMatrix.yx, FontMatrix.yy,
	     				FontMatrix.tx, FontMatrix.ty);
  s->writeUid(&UID);
  s->print("/FontBBox {%g %g %g %g} readonly def\n",
	     			FontBBox.p.x, FontBBox.p.y,
	     			FontBBox.q.x, FontBBox.q.y);
	{
		static const PSParamItem font_items[] = {
	    {"FontType", ps_param_type_int,_OFFSET_OF_(XWPSFontType1, FontType)},
	    {"PaintType", ps_param_type_int, _OFFSET_OF_(XWPSFontType1, PaintType)},
	    {"StrokeWidth", ps_param_type_float, _OFFSET_OF_(XWPSFontType1, StrokeWidth)},
	    {0,0,0}}; 
	    	
	  code = rlist.writeItems(i_ctx_p, this, NULL, (PSParamItem*)(&font_items[0]));
		if (code < 0)
	    return code;
	}
	
	rlist.writeFloatArray(i_ctx_p, "WeightVector", (const float*)(&WeightVector.values[0]), WeightVector.count);
	s->pputs("currentdict end\n");
	if (lenIVA < 0 && (options & WRITE_TYPE1_WITH_LENIV))
	{
		lenIVA = 0;
		e = true;
	}
	
	uchar AXE_buf[200];		
	uchar exE_buf[200];
	XWPSStream AXE_stream;	
	AXE_stream.incRef();
	PSStreamAXEState AXE_state;
	XWPSStream exE_stream;
	exE_stream.incRef();
	PSStreamexEState exE_state;
	if (options & WRITE_TYPE1_EEXEC)
	{
		s->pputs("currentfile eexec\n");
		lengths[0] = s->tell() - start;
		start = s->tell();
		if (options & WRITE_TYPE1_ASCIIHEX)
		{
			AXE_stream.init();
			AXE_stream.incRef();
			AXE_state.templat = &s_AXE_template;
	    AXE_state.EndOfData = false;
	    XWPSStream::initFilter(&AXE_stream, (PSStreamState *)&AXE_state, AXE_buf, sizeof(AXE_buf), es, false, false);
	    es = &AXE_stream;
	    es->incRef();
		}
		exE_stream.init();
		exE_stream.incRef();
		exE_state.templat = &s_exE_template;
		exE_state.cstate = 55665;
  	XWPSStream::initFilter(&exE_stream, (PSStreamState *)&exE_state, exE_buf, sizeof(exE_buf), es, false, false);
  	es = &exE_stream;
  	es->incRef();
  	es->pputs("****");
	}
	
	code = writePrivate(es, glyphs.subset_glyphs, glyphs.subset_size, glyphs.notdef, lenIV, &ppp, e);
  if (code < 0)
		return code;
	
	es->pputs("dup/FontName get exch definefont pop\n");
	if (options & WRITE_TYPE1_EEXEC)
	{
		if (options & (WRITE_TYPE1_EEXEC_PAD | WRITE_TYPE1_EEXEC_MARK))
			es->pputs("mark ");
		es->pputs("currentfile closefile\n");
		XWPSStream::closeFilters(&es, s);
		lengths[1] = s->tell() - start;
		start = s->tell();
		if (options & WRITE_TYPE1_EEXEC_PAD)
		{
			for (int i = 0; i < 8; ++i)
				s->pputs("\n0000000000000000000000000000000000000000000000000000000000000000");
	    s->pputs("\ncleartomark\n");
		}
		lengths[2] = s->tell() - start;
	}
	else
	{
		lengths[0] = s->tell() - start;
		lengths[1] = lengths[2] = 0;
	}
  	
  rlist.release();
  return 0;
}

int XWPSFontType1::writeType2Font(XWPSStream *s, 
	                                int options,
		                              ulong *subset_glyphs, 
		                              uint subset_size,
		      					              const XWPSString *alt_font_name)
{
	XWPSCffWriter writer;
  XWPSCffGlyphSubset subset;
  XWPSCffStringItem std_string_items[500];
  XWPSCffStringItem string_items[500  +  40];
  XWPSString font_nameA;
  XWPSStream poss;
  uint charstrings_count, charstrings_size;
  uint subrs_count, subrs_size;
  uint encoding_size, charset_size;
  
  uint	Top_size = 0xffff,
				Encoding_offset = 0xffff,
				charset_offset = 0xffff,
				CharStrings_offset = 0xffff,
				Private_offset = 0xffff,
				Private_size = 0xffff,
				Subrs_offset = 0xffff,
				End_offset = 0xffff;
				
	XWPSPSFGlyphEnum genum;
  ulong glyph;
  long start_pos;
  uint offset;
  
  int code = getType1Glyphs(&subset.glyphs, subset_glyphs, subset_size);
  if (code < 0)
		return code;
  if (subset.glyphs.notdef == ps_no_glyph)
		return (int)(XWPSError::RangeCheck);
			
	if (options & WRITE_TYPE2_CHARSTRINGS) 
	{
		options |= WRITE_TYPE2_NO_LENIV;
		if (FontType != ft_encrypted2)
		{
	    defaultWidthX = nominalWidthX = 0;
	  }
  }
  writer.options = options;
  poss.writePositionOnly();
  writer.strm = &poss;
  writer.pfont = this;
  writer.glyph_data = psf_type1_glyph_data;
  writer.offset_size = 1;
  writer.start_pos = s->tell();
  genum.listBegin(this,	subset.glyphs.subset_glyphs, (subset.glyphs.subset_glyphs ? subset.glyphs.subset_size : 0),GLYPH_SPACE_NAME);
  {
  	ulong encoded[256];
		int num_enc = 0;
		int num_enc_chars = 0;
		for (int j = 0; j < 256; ++j) 
		{
	    glyph = encodeChar((ulong)j, GLYPH_SPACE_NAME);
	    if (glyph != ps_no_glyph && glyph != subset.glyphs.notdef &&
					(subset.glyphs.subset_glyphs == 0 ||
		 			psf_sorted_glyphs_include(subset.glyphs.subset_data, subset.glyphs.subset_size, glyph)))
			encoded[num_enc_chars++] = glyph;
		}
		
		subset.num_encoded_chars = num_enc_chars;
		subset.num_encoded = num_enc = psf_sort_glyphs(encoded, num_enc_chars);
		if (!subset.glyphs.subset_glyphs)
		{
			int num_glyphs = 0;

	    genum.reset();
	    while ((code = genum.enumerateGlyphsNext(&glyph)) != 1)
				if (code == 0) 
				{
		    	if (num_glyphs == (sizeof(subset.glyphs.subset_data) / sizeof(subset.glyphs.subset_data[0])))
						return (int)(XWPSError::LimitCheck);
		    	subset.glyphs.subset_data[num_glyphs++] = glyph;
				}
	    subset.glyphs.subset_size =	psf_sort_glyphs(subset.glyphs.subset_data, num_glyphs);
	    subset.glyphs.subset_glyphs = subset.glyphs.subset_data;
		}
		
		{
			int from = subset.glyphs.subset_size;
	    int to = from;

	    while (from > 0) 
	    {
				glyph = subset.glyphs.subset_data[--from];
				if (glyph != subset.glyphs.notdef &&  !psf_sorted_glyphs_include(encoded, num_enc, glyph))
		    	subset.glyphs.subset_data[--to] = glyph;
	    }
		}
		
		subset.glyphs.subset_data[0] = subset.glyphs.notdef;
		memcpy(subset.glyphs.subset_data + 1, encoded,  sizeof(encoded[0]) * num_enc);
  }
  
  if (alt_font_name)
  {
  	if (alt_font_name->bytes)
  		alt_font_name->bytes->incRef();
		font_nameA = *alt_font_name;
	}
  else
  {
		font_nameA.data = font_name.chars;
	  font_nameA.size = font_name.size;
	}
	
	writer.std_strings.init(std_string_items, sizeof(std_string_items) / sizeof(std_string_items[0]));
  for (int j = 0; (glyph = knownEncode((ulong)j, ENCODING_INDEX_CFFSTRINGS)) != ps_no_glyph; ++j) 
  {
		uint size;
		const uchar *str = (const uchar *)glyphName(glyph, &size);
		int ignore;

		writer.std_strings.index(str, size, true, &ignore);
  }
  writer.strings.init(string_items, sizeof(string_items) / sizeof(string_items[0]));
  writer.writeTopFont(0, 0, 0, 0, 0);
  genum.listBegin(this, subset.glyphs.subset_data, subset.glyphs.subset_size, GLYPH_SPACE_NAME);
  while ((code = genum.enumerateGlyphsNext(&glyph)) != 1)
		if (code == 0) 
		{
	    code = writer.glyphSid(glyph);
	    if (code < 0)
				return code;
		}
		
	encoding_size = cffEncodingSize(subset.num_encoded,  subset.num_encoded_chars);
	charset_size = 1 + (subset.glyphs.subset_size - 1) * 2;
	charstrings_size =	writer.writeCharStringsOffsets(&genum, &charstrings_count);
	
#ifdef SKIP_EMPTY_SUBRS
  subrs_size = (writer.convertCharStrings(this) ? 0 : writer.writeSubrsOffsets(&subrs_count, this));
#else
  if (writer.convertCharStrings(this))
		subrs_count = 0;
  subrs_size = writer.writeSubrsOffsets(&subrs_count, this);
#endif

iter:
	poss.writePositionOnly();
	writer.strm = &poss;
	
	Encoding_offset = 4 + cffIndexSize(1, font_nameA.size) +	cffIndexSize(1, Top_size) +
										cffIndexSize(writer.strings.count, writer.strings.total) + cffIndexSize(0, 0);
  charset_offset = Encoding_offset + encoding_size;
  CharStrings_offset = charset_offset + charset_size;
  Private_offset = CharStrings_offset +	cffIndexSize(charstrings_count, charstrings_size);
  Subrs_offset = Private_size;
  
write:
  start_pos = writer.strm->tell();
  writer.strm->putBytes((const uchar *)"\001\000\004\002", 4);
  writer.putIndexHeader(1, font_nameA.size);
  writer.putOffset(font_nameA.size + 1);
  writer.strm->putBytes(font_nameA.data, font_nameA.size);
  
  writer.putIndexHeader(1, Top_size);
  writer.putOffset(Top_size + 1);
  offset = writer.strm->tell() - start_pos;
  writer.writeTopFont(Encoding_offset, charset_offset, CharStrings_offset, Private_offset, Private_size);
  Top_size = writer.strm->tell() - start_pos - offset;
  writer.putIndex(&writer.strings);
  writer.putIndexHeader(0, 0);
  writer.writeEncoding(&subset);
  writer.writeCharset(&subset);
  
  offset = writer.strm->tell() - start_pos;
  if (offset > CharStrings_offset)
		return (int)(XWPSError::RangeCheck);
  CharStrings_offset = offset;
  writer.writeCharStrings(&genum, charstrings_count, charstrings_size);
  
  offset = writer.strm->tell() - start_pos;
  if (offset > Private_offset)
		return (int)(XWPSError::RangeCheck);
  Private_offset = offset;
  writer.writePrivate((subrs_size == 0 ? 0 : Subrs_offset), this);
  Private_size = writer.strm->tell() - start_pos - offset;
  
  offset = writer.strm->tell() - (start_pos + Private_offset);
  if (offset > Subrs_offset)
		return (int)(XWPSError::RangeCheck);
  Subrs_offset = offset;
  if (writer.convertCharStrings(this))
		writer.putIndexHeader(0, 0);
  else if (subrs_size != 0)
		writer.writeSubrs(subrs_count, subrs_size, this);
		
	offset = writer.strm->tell() - start_pos;
  if (offset > End_offset)
		return (int)(XWPSError::RangeCheck);
			
	if (offset == End_offset)
	{
		if (writer.strm == &poss) 
		{
	    writer.strm = s;
	    goto write;
		}
	}
	else
	{
		End_offset = offset;
		goto iter;
	}
	return 0;
}

int XWPSFontType1::writeEncoding(XWPSStream *s, 
	                               int options,
	                               ulong *subset_glyphs, 
	                               uint subset_size, 
	                               ulong notdef)
{
	s->pputs("/Encoding ");
  switch (encoding_index)
  {
  	case ENCODING_INDEX_STANDARD:
	    s->pputs("StandardEncoding");
	    break;
	    
	  case ENCODING_INDEX_ISOLATIN1:
	    if (options & WRITE_TYPE1_POSTSCRIPT) 
	    {
				s->pputs("ISOLatin1Encoding");
				break;
	    }
	    
	  default:
	  	{
	  		s->pputs("256 array\n");
				s->pputs("0 1 255 {1 index exch /.notdef put} for\n");
				for (ulong i = 0; i < 256; ++i)
				{
					ulong glyph = encodeChar(i, GLYPH_SPACE_NAME);
					const char *namestr;
		    	uint namelen;
		    	if (subset_glyphs && subset_size)
		    	{
		    		if (!psf_sorted_glyphs_include(subset_glyphs,subset_size, glyph))
			    		continue;
		    	}
		    	
		    	if (glyph != ps_no_glyph && glyph != notdef &&	
		    		 (namestr = glyphName(glyph, &namelen)) != 0) 
		    	{
						s->print("dup %d /", (int)i);
						s->write(namestr, namelen);
						s->pputs(" put\n");
		    	}
				}
				
				s->pputs("readonly");
	  	}
  }
  
  s->pputs(" def\n");
  return 0;
}

void XWPSFontType1::writeFontName(XWPSStream *s, const XWPSString *alt_font_name)
{
	if (alt_font_name)
		s->write(alt_font_name->data, alt_font_name->size);
  else
		s->write(font_name.chars, font_name.size);
}

int  XWPSFontType1::writePrivate(XWPSStream *s, 
	      						             ulong *subset_glyphs, 
	      						             uint subset_size,
	                               ulong , 
	                               int lenIVA,
	      						             const PSPrinterParams *ppp,
	      						             bool e)
{
	XWPSPrinterParamList rlist(s, (PSPrinterParams*)ppp);
	s->pputs("dup /Private 17 dict dup begin\n");
  s->pputs("/-|{string currentfile exch readstring pop}executeonly def\n");
  s->pputs("/|-{noaccess def}executeonly def\n");
  s->pputs("/|{noaccess put}executeonly def\n");
  
  {
  	static const PSParamItem private_items[] = {
  		{"BlueFuzz", ps_param_type_int, _OFFSET_OF_(XWPSFontType1, BlueFuzz)},
  		{"BlueScale", ps_param_type_float, _OFFSET_OF_(XWPSFontType1, BlueScale)},
  		{"BlueShift", ps_param_type_float, _OFFSET_OF_(XWPSFontType1, BlueShift)},
  		{"ExpansionFactor", ps_param_type_float, _OFFSET_OF_(XWPSFontType1, ExpansionFactor)},
  		{"ForceBold", ps_param_type_bool, _OFFSET_OF_(XWPSFontType1, ForceBold)},
  		{"LanguageGroup", ps_param_type_int, _OFFSET_OF_(XWPSFontType1, LanguageGroup)},
  		{"RndStemUp", ps_param_type_bool, _OFFSET_OF_(XWPSFontType1, RndStemUp)},
  	{0, 0, 0}};
  	
  	XWPSFontType1 defaults;
  	defaults.BlueFuzz = 1;
		defaults.BlueScale = 0.039625;
		defaults.BlueShift = 7.0;
		defaults.ExpansionFactor = 0.06;
		defaults.ForceBold = false;
		defaults.LanguageGroup = 0;
		defaults.RndStemUp = true;
		int code = rlist.writeItems(i_ctx_p, this, &defaults, (PSParamItem*)&private_items[0]);
		if (code < 0)
	    return code;
	    
		if (lenIVA != 4) 
		{
	    code = rlist.writeInt(i_ctx_p, "lenIV", &lenIVA);
	    if (code < 0)
				return code;
		}
		rlist.writeFloatArray(i_ctx_p, "BlueValues", (const float*)(&BlueValues.values[0]), BlueValues.count);
		rlist.writeFloatArray(i_ctx_p, "OtherBlues", (const float*)(&OtherBlues.values[0]), OtherBlues.count);
		rlist.writeFloatArray(i_ctx_p, "FamilyBlues", (const float*)(&FamilyBlues.values[0]), FamilyBlues.count);
		rlist.writeFloatArray(i_ctx_p, "FamilyOtherBlues", (const float*)(&FamilyOtherBlues.values[0]), FamilyOtherBlues.count);
		rlist.writeFloatArray(i_ctx_p, "StdHW", (const float*)(&StdHW.values[0]), StdHW.count);
		rlist.writeFloatArray(i_ctx_p, "StemSnapH", (const float*)(&StemSnapH.values[0]), StemSnapH.count);
		rlist.writeFloatArray(i_ctx_p, "StemSnapV", (const float*)(&StemSnapV.values[0]), StemSnapV.count);
	}
	
	s->writeUid(&UID);
  s->pputs("/MinFeature{16 16} def\n");
  s->pputs("/password 5839 def\n");
  {
  	XWPSString str;
  	int n = 0;
  	for (; subrData(n, false, &str) != (int)(XWPSError::RangeCheck);)
	    ++n;
		s->print("/Subrs %d array\n", n);
		
		for (int i = 0; i < n; ++i)
		{
			if (subrData(i, false, &str) >= 0)
			{
				char buf[50];
				sprintf(buf, "dup %d %u -| ", i, str.size);
				s->pputs(buf);
				if (e)
					s->writeEncrypted(str.data, str.size);
				else
					s->write(str.data, str.size);
				s->pputs(" |\n");
			}
		}
		s->pputs("|-\n");
  }
  
  {
  	int num_chars = 0;
		ulong glyph;
		XWPSPSFGlyphEnum genum;
		XWPSString gdata;
		int code;
		genum.listBegin(this, subset_glyphs, (subset_glyphs ? subset_size : 0), GLYPH_SPACE_NAME);
		for (glyph = ps_no_glyph; (code = genum.enumerateGlyphsNext(&glyph)) != 1;)
		{
			if (code == 0)
			{
				code = glyphData(glyph, &gdata);
				if (code >= 0)
					++num_chars;
			}
		}
		
		s->print("2 index /CharStrings %d dict dup begin\n", num_chars);
		genum.reset();
		for (glyph = ps_no_glyph; (code = genum.enumerateGlyphsNext( &glyph)) != 1;)
		{
			if (code == 0) 
			{
				code = glyphData(glyph, &gdata);
				if (code >= 0)
				{
					uint gssize;
					const char *gstr = glyphName(glyph, &gssize);
					s->pputs("/");
					s->write(gstr, gssize);
					s->print(" %d -| ", (int)(gdata.size));
					if (e)
						s->writeEncrypted(gdata.data, gdata.size);
					else
						s->write(gdata.data, gdata.size);
						
					s->pputs(" |-\n");
				}
	    }
		}
  }
  
  s->pputs("end\nend\nreadonly put\nnoaccess put\n");
  rlist.release();
  return 0;
}

int XWPSFontType1::zchar1GlyphOutline(ulong glyph, const XWPSMatrix *pmat, XWPSPath *ppath)
{
	XWPSRef gref;
  XWPSString charstring;
  int code;

  i_ctx_p->glyphRef(glyph, &gref);
  code = i_ctx_p->zcharCharStringData(this, &gref, &charstring);
  if (code < 0)
		return code;
  return i_ctx_p->zcharStringOutline(this, &gref, &charstring, pmat, ppath);
}

int XWPSFontType1::z1enumerateGlyph(int *pindex, PSGlyphSpace , ulong * pglyph)
{
	XWPSFontData * pdata = (XWPSFontData*)client_data;
	XWPSRef *pcsdict = &pdata->CharStrings;

  return i_ctx_p->zcharEnumerateGlyph(pcsdict, pindex, pglyph);
}

int XWPSFontType1::z1fontInfo(const XWPSPoint *pscale, int members, XWPSFontInfo *info)
{
  int code = fontInfoDefault(pscale, members &  ~(FONT_INFO_COPYRIGHT | FONT_INFO_NOTICE |
		      										FONT_INFO_FAMILY_NAME | FONT_INFO_FULL_NAME),  info);
  XWPSRef *pfdict;
  XWPSRef *pfontinfo;

  if (code < 0)
		return code;
    
  XWPSFontData * pdata = (XWPSFontData*)client_data;
  pfdict = &pdata->dict;
  if (pfdict->dictFindString(i_ctx_p, "FontInfo", &pfontinfo) <= 0 ||
				!pfontinfo->hasType(XWPSRef::Dictionary))
		return 0;
		
  if ((members & FONT_INFO_COPYRIGHT) && i_ctx_p->z1fontInfoHas(pfontinfo, "Copyright", &info->Copyright))
		info->members |= FONT_INFO_COPYRIGHT;
  if ((members & FONT_INFO_NOTICE) &&	i_ctx_p->z1fontInfoHas(pfontinfo, "Notice", &info->Notice))
		info->members |= FONT_INFO_NOTICE;
  if ((members & FONT_INFO_FAMILY_NAME) &&	i_ctx_p->z1fontInfoHas(pfontinfo, "FamilyName", &info->FamilyName))
		info->members |= FONT_INFO_FAMILY_NAME;
  if ((members & FONT_INFO_FULL_NAME) &&	i_ctx_p->z1fontInfoHas(pfontinfo, "FullName", &info->FullName))
		info->members |= FONT_INFO_FULL_NAME;
  return code;
}

int XWPSFontType1::z1glyphData(ulong glyph, XWPSString * pstr)
{
	XWPSRef gref;

  i_ctx_p->glyphRef(glyph, &gref);
  return i_ctx_p->zcharCharStringData(this, &gref, pstr);
}

int XWPSFontType1::z1glyphInfo(ulong glyph, 
	                const XWPSMatrix *pmat,
	                int members, 
	                XWPSGlyphInfo *info)
{
	XWPSRef gref;
  XWPSRef *pcdevproc;
  int wmode = WMode;
  XWPSFontData * pfdata = (XWPSFontData*)(client_data);
  XWPSRef *pfdict = &pfdata->dict;
  double sbw[4];
  int width_members = members & (GLYPH_INFO_WIDTH0 << wmode);
  int default_members = members - width_members;
  int done_members = 0;
  int code;

  if (!width_members)
		return type1GlyphInfo(glyph, pmat, members, info);
  if (pfdict->dictFindString(i_ctx_p, "CDevProc", &pcdevproc) > 0)
		return (int)(XWPSError::RangeCheck);
  i_ctx_p->glyphRef(glyph, &gref);
  if (width_members == GLYPH_INFO_WIDTH1) 
  {
		code = i_ctx_p->zcharGetMetrics2(this, &gref, sbw);
		if (code > 0) 
		{
	    info->width[1].x = sbw[2];
	    info->width[1].y = sbw[3];
	    done_members = width_members;
	    width_members = 0;
		}
  }
  if (width_members) 
  {
		code = i_ctx_p->zcharGetMetrics(this, &gref, sbw);
		if (code > 0) 
		{
	    info->width[wmode].x = sbw[2];
	    info->width[wmode].y = sbw[3];
	    done_members = width_members;
	    width_members = 0;
		}
  }
  default_members |= width_members;
  if (default_members) 
  {
		code = type1GlyphInfo(glyph, pmat, default_members, info);

		if (code < 0)
	    return code;
  } 
  else
		info->members = 0;
  info->members |= done_members;
  return 0;
}

int XWPSFontType1::z1sameFont(const XWPSFont *ofont, int mask)
{
	if (ofont->FontType != FontType)
		return 0;
		
	XWPSFont * font = this;
  while (font->base != font)
		font = font->base;
  
  while (ofont->base != ofont)
		ofont = ofont->base;
    
  if (ofont == font)
		return mask;
    
  {
		int same = sameFontBase(ofont, mask);
		int check = mask & ~same;
		const XWPSFontType1 *const pfont1 = (const XWPSFontType1 *)font;
		XWPSFontData * pdata = (XWPSFontData*)(pfont1->client_data);
		const XWPSFontType1 *pofont1 = (const XWPSFontType1 *)ofont;
		XWPSFontData * podata = (XWPSFontData*)(pofont1->client_data);

		if ((check & (FONT_SAME_OUTLINES | FONT_SAME_METRICS)) &&
	    	!memcmp(&pofont1->procs, &z1_data_procs, sizeof(z1_data_procs)) &&
	    	pdata->CharStrings.objEq(i_ctx_p, &podata->CharStrings) &&
	    	i_ctx_p->sameFontDict(pdata, podata, "Private"))
	    same |= FONT_SAME_OUTLINES;

		if ((check & FONT_SAME_METRICS) && (same & FONT_SAME_OUTLINES) &&
	    	!memcmp(&pofont1->procs, &z1_data_procs, sizeof(z1_data_procs)) &&
	    	i_ctx_p->sameFontDict(pdata, podata, "Metrics") &&
	    	i_ctx_p->sameFontDict(pdata, podata, "Metrics2") &&
	    	i_ctx_p->sameFontDict(pdata, podata, "CDevProc"))
	    same |= FONT_SAME_METRICS;

		if ((check & FONT_SAME_ENCODING) && pofont1->procs.same_font_.same_fontt1 == &XWPSFontType1::z1sameFont &&
	    		pdata->Encoding.objEq(i_ctx_p, &podata->Encoding))
	    same |= FONT_SAME_ENCODING;

		return same & mask;
  }
}

int XWPSFontType1::z1seacData(int ccode, ulong *pglyph, XWPSString *pstr)
{
	XWPSRef std_glyph;
  int code = (i_ctx_p->StandardEncoding())->arrayGet(i_ctx_p, (long)ccode, &std_glyph);

  if (code < 0)
		return code;
  if (pglyph) 
  {
		switch (std_glyph.type()) 
		{
			case XWPSRef::Name:
	    	*pglyph = i_ctx_p->nameIndex(&std_glyph);
	    	break;
	    	
			case XWPSRef::Integer:
	    	*pglyph = ps_min_cid_glyph + std_glyph.value.intval;
	    	if (*pglyph < ps_min_cid_glyph || *pglyph > ps_max_glyph)
					*pglyph = ps_no_glyph;
	    	break;
	    	
			default:
	    	return (int)(XWPSError::TypeCheck);
		}
  }
  if (pstr)
		code = i_ctx_p->zcharCharStringData(this, &std_glyph, pstr);
  return code;
}

int XWPSFontType1::z1subrData(int index, bool global, XWPSString * pstr)
{
	XWPSFontData *pfdata = (XWPSFontData*)(client_data);
  XWPSRef subr;
  int code;

	XWPSRef * p;
	if (global)
		p = pfdata->u.type1.GlobalSubrs;
	else
		p = pfdata->u.type1.Subrs;
  code = p->arrayGet(i_ctx_p, index, &subr);
  if (code < 0)
		return code;
    
  code = subr.checkTypeOnly(XWPSRef::String);
  if (code < 0)
		return code;
		
	if (pstr->bytes)
	{
		if (pstr->bytes->decRef() == 0)
			delete pstr->bytes;
		pstr->bytes = 0;
	}
  pstr->data = subr.getBytes();
  pstr->size = subr.size();
  pstr->bytes = subr.value.bytes->arr;
  if (pstr->bytes)
  	pstr->bytes->incRef();
  return 0;
}

int XWPSFontType1::z9FDArrayGlyphData(ulong , XWPSString * )
{
	return (int)(XWPSError::InvalidFont);
}

int XWPSFontType1::z9FDArraySeacData(int , ulong *, XWPSString *)
{
	return (int)(XWPSError::InvalidFont);
}

static int cid0_glyph_data(XWPSFontBase *pbfont, 
                           ulong glyph, 
                           XWPSString *pstr,
		                       XWPSFontType1 **ppfont)
{
	XWPSFontCid0 *  pfont = (XWPSFontCid0 *)pbfont;
  int font_index;
  int code = pfont->glyphData(glyph, pstr, &font_index);
  if (code >= 0)
		*ppfont = pfont->FDArray[font_index];
  return code;
}

XWPSFontCid0::XWPSFontCid0()
	:XWPSFontBase()
{
	CIDCount = 0;
	GDBytes = 0;
	CIDMapOffset = 0;
	FDArray = 0;
	FDArray_size = 0;
	FDBytes = 0;
	proc_data = 0;
}

XWPSFontCid0::~XWPSFontCid0()
{	
	if (FDArray)
	{
		for (uint i = 0; i < FDArray_size; i++)
		{
			if (FDArray[i])
			{
				if (FDArray[i]->decRef() == 0)
					delete FDArray[i];
				FDArray[i] = 0;
			}
		}
		
		free(FDArray);
		FDArray = 0;
	}
}

int XWPSFontCid0::cid0EnumerateGlyph(int *pindex,
			                   PSGlyphSpace ,
			                   ulong *pglyph)
{
	while (*pindex < CIDCount) 
	{
		XWPSString gstr;
		int fidx;
		ulong glyph = (ulong)(ps_min_cid_glyph + (*pindex)++);
		int code = glyphData(glyph,  &gstr, &fidx);

		if (code < 0 || gstr.size == 0)
	    continue;
	    
		*pglyph = glyph;
		return 0;
  }
  *pindex = 0;
  return 0;
}

int XWPSFontCid0::cid0ReadBytes(ulong base, uint count, uchar *buf,	XWPSString *pstr)
{
	XWPSFontData *pfdata = (XWPSFontData*)(client_data);
  uchar *data = buf;
  int code = 0;
  XWPSBytes * b = 0;

  if (base != (long)base || base > base + count)
	  return (int)(XWPSError::RangeCheck);
  
  if (pfdata->u.cid0.DataSource->hasType(XWPSRef::Null)) 
  {
		XWPSRef *pgdata = pfdata->u.cid0.GlyphData;

		if (pgdata->hasType(XWPSRef::String)) 
		{
	    uint size = pgdata->size();

	    if (base >= size || count > size - base)
	  		return (int)(XWPSError::RangeCheck);
	    data = pgdata->getBytes() + base;
	    b = pgdata->value.bytes->arr;
	    if (b)
	    	b->incRef();
		} 
		else 
		{
	    ulong skip = base;
	    uint copied = 0;
	    uint index = 0;
	    XWPSRef rstr;
	    uint size;

	    for (;; skip -= size, ++index) 
	    {
				int ccode = pgdata->arrayGet(i_ctx_p, index, &rstr);

				if (ccode < 0)
		    	return ccode;
				if (!rstr.hasType(XWPSRef::String))
		    	return rstr.checkTypeFailed();
				size = rstr.size();
				if (skip < size)
		    	break;
	    }
	    size -= skip;
	    if (count <= size) 
	    {
				data = rstr.getBytes() + skip;
				b = rstr.value.bytes->arr;
				if (b)
					b->incRef();
			}
	    else 
	    {
				if (data == 0) 
				{
					b = new XWPSBytes(count);
		    	data = b->arr;
		    	code = 1;
				}
				memcpy(data, rstr.getBytes() + skip, size);
				copied = size;
				while (copied < count) 
				{
		    	int ccode = pgdata->arrayGet(i_ctx_p, ++index, &rstr);
					
		    	if (ccode < 0)
		    	{
		    		code = ccode;
						goto err;
					}
		    	if (!rstr.hasType(XWPSRef::String)) 
		    	{
						code = rstr.checkTypeFailed();
						goto err;
		    	}
		    	size = rstr.size();
		    	if (size > count - copied)
						size = count - copied;
		    	memcpy(data + copied, rstr.getBytes(), size);
		    	copied += size;
				}
	    }
		}
  } 
  else 
  {
		XWPSStream *s;
		uint nread;

		s = pfdata->u.cid0.DataSource->getStream();
		if (s->read_id != pfdata->u.cid0.DataSource->size())
		{
			if (s->read_id == 0 && s->write_id == pfdata->u.cid0.DataSource->size())
			{
				int fcode = i_ctx_p->fileSwitchToRead(pfdata->u.cid0.DataSource);
				if (fcode < 0)
				{
					code = fcode;
					goto err;
				}
			}
			else
				s = i_ctx_p->getInvalidFileEntry();
		}
			
		if (s->seek(base) < 0)
	    return (int)(XWPSError::IOError);
	    	
		if (data == 0) 
		{	
			b = new XWPSBytes(count);
	    data = b->arr;
	    code = 1;
		}
		if (s->gets(data, count, &nread) < 0 || nread != count) 
		{
	    code = (int)(XWPSError::IOError);
		}
  }
  
err:
  pstr->data = data;
  pstr->size = count;
  pstr->bytes = b;
  return code;
}

XWPSFont * XWPSFontCid0::copyFont()
{
	XWPSFontCid0 * newfont = new XWPSFontCid0;
	copyFontCid0(newfont);
	return newfont;
}

void XWPSFontCid0::copyFontCid0(XWPSFontCid0 * newfont)
{
	copyFontBase(newfont);	
	newfont->CIDCount = CIDCount;
	newfont->GDBytes = GDBytes;
	newfont->CIDMapOffset = CIDMapOffset;
	newfont->FDArray_size = FDArray_size;
	newfont->FDBytes = FDBytes;
	newfont->glyph_data = glyph_data;
	newfont->glyph_datac0 = glyph_datac0;
	newfont->proc_data = proc_data;
}

int XWPSFontCid0::getLength()
{
	return sizeof(XWPSFontCid0);
}

const char * XWPSFontCid0::getTypeName()
{
	return "fontcid0";
}

int XWPSFontCid0::glyphData(ulong glyph, XWPSString *pgstr, int *pfidx)
{
	return (this->*glyph_data)(glyph, pgstr, pfidx);
}

int XWPSFontCid0::writeCid0Font(XWPSStream *s, int options,
		    					              const uchar *subset_cids, 
		    						            uint subset_size,
		    						            const XWPSString *alt_font_name)
{
	XWPSCffWriter writer;
  XWPSCffStringItem std_string_items[500];
  XWPSCffStringItem string_items[500 + 40];
  XWPSString font_nameA;
  XWPSStream poss;
  uint charstrings_count, charstrings_size;
  uint charset_size, fdselect_size;
  uint subrs_count[256], subrs_size[256];
  uint	Top_size = 0x7fffff,
				charset_offset = 0x7fffff,
				FDSelect_offset = 0x7fffff,
				CharStrings_offset = 0x7fffff,
				Font_offset = 0x7fffff,
				FDArray_offsets[257],
				Private_offsets[257],
				Subrs_offsets[257],
				End_offset = 0x7fffff;
				
	XWPSPSFGlyphEnum genum;
  long start_pos;
  uint offset;
  int num_fonts = FDArray_size;
  int code;
  
  genum.bitsBegin(this, subset_cids, subset_size, GLYPH_SPACE_NAME);
  code = checkOutlineGlyphs(&genum, cid0_glyph_data);
  if (code < 0)
		return code;
		
	writer.options = options;
	poss.writePositionOnly();
  writer.strm = &poss;
  writer.pfont = this;
  writer.glyph_data = cid0_glyph_data;
  writer.offset_size = 1;
  writer.start_pos = s->tell();
  if (alt_font_name)
  {
  	if (alt_font_name->bytes)
  		alt_font_name->bytes->incRef();
		font_nameA = *alt_font_name;
	}
  else if (font_name.size)
  {
		font_nameA.data = font_name.chars;
	  font_nameA.size = font_name.size;
	}
  else
  {
		font_nameA.data = key_name.chars;
	  font_nameA.size = key_name.size;
	}
	
	writer.std_strings.init(std_string_items, sizeof(std_string_items) / sizeof(std_string_items[0]));
  writer.strings.init(string_items, sizeof(string_items)/ sizeof(string_items[0]));
  
  writer.writeROS(&CIDSystemInfo);
  for (int j = 0; j < num_fonts; ++j) 
  {
		XWPSFontType1 *pfd = FDArray[j];
		writer.writeTopFDArray(pfd, 0, 0);
  }
  
  for (int j = 0; j <= num_fonts; ++j)
  {
		FDArray_offsets[j] = Private_offsets[j] = Subrs_offsets[j] = 0x7effffff / num_fonts * j + 0x1000000;
	}
	
	poss.writePositionOnly();
	writer.writeCidSet(&genum);
  charset_size = poss.tell();
  
  fdselect_size = writer.FDSelectSize();
  charstrings_size = writer.writeCharStringsOffsets(&genum, &charstrings_count);
  for (int j = 0; j < num_fonts; ++j) 
  {
		XWPSFontType1 *pfd = FDArray[j];

#ifdef SKIP_EMPTY_SUBRS
		subrs_size[j] = (writer.convertCharStrings(pfd) ? 0 : writer.writeSubrsOffsets(&subrs_count[j], pfd));
#else
		if (writer.convertCharstrings(pfd))
	    subrs_count[j] = 0;
		subrs_size[j] = writer.writeSubrsOffsets(&subrs_count[j], pfd);
#endif
  }
  
iter:
  poss.writePositionOnly();
  writer.strm = &poss;
  charset_offset = 4 + XWPSFontType1::cffIndexSize(1, font_name.size) +
										XWPSFontType1::cffIndexSize(1, Top_size) +
										XWPSFontType1::cffIndexSize(writer.strings.count, writer.strings.total) +
										XWPSFontType1::cffIndexSize(0, 0);
  FDSelect_offset = charset_offset + charset_size;
  CharStrings_offset = FDSelect_offset + fdselect_size;
  
write:
  start_pos = writer.strm->tell();
  writer.offset_size = (End_offset > 0x7fff ? 3 : 2);
  writer.strm->putBytes((const uchar *)"\001\000\004", 3);
  writer.strm->putc(writer.offset_size);
  
  writer.putIndexHeader(1, font_nameA.size);
  writer.putOffset(font_nameA.size + 1);
  writer.strm->putBytes(font_nameA.data, font_nameA.size);
  
  writer.putIndexHeader(1, Top_size);
  writer.putOffset(Top_size + 1);
  offset = writer.strm->tell() - start_pos;
  writer.writeTopCidFont(charset_offset, CharStrings_offset, FDSelect_offset, Font_offset);
  Top_size = writer.strm->tell() - start_pos - offset;
  
  writer.putIndex(&writer.strings);
  writer.putIndexHeader(0, 0);
  
  writer.writeCidSet(&genum);
  offset = writer.strm->tell() - start_pos;
  if (offset > FDSelect_offset)
		return (int)(XWPSError::RangeCheck);
  FDSelect_offset = offset;
  writer.writeFDSelect(fdselect_size);
  
  offset = writer.strm->tell() - start_pos;
  if (offset > CharStrings_offset)
		return (int)(XWPSError::RangeCheck);
  CharStrings_offset = offset;
  writer.writeCharStrings(&genum, charstrings_count, charstrings_size);
  
  offset = writer.strm->tell() - start_pos;
  if (offset > Font_offset)
		return (int)(XWPSError::RangeCheck);
  Font_offset = offset;
  writer.writeFDArrayOffsets(FDArray_offsets, num_fonts);
  offset = writer.strm->tell() - start_pos;
  if (offset > FDArray_offsets[0])
		return (int)(XWPSError::RangeCheck);
  FDArray_offsets[0] = offset;
  for (int j = 0; j < num_fonts; ++j) 
  {
		XWPSFontType1 *pfd = FDArray[j];
		if (options & WRITE_TYPE2_CHARSTRINGS) 
		{
	    options |= WRITE_TYPE2_NO_LENIV;
	    if (pfd->FontType != ft_encrypted2)
				pfd->defaultWidthX = pfd->nominalWidthX = 0;
		}
		writer.writeTopFDArray(pfd, Private_offsets[j], Private_offsets[j + 1] - Private_offsets[j]);
		offset = writer.strm->tell() - start_pos;
		if (offset > FDArray_offsets[j + 1])
	    return (int)(XWPSError::RangeCheck);
		FDArray_offsets[j + 1] = offset;
  }
  
  for (int j = 0; ; ++j)
  {
		offset = writer.strm->tell() - start_pos;
		if (offset > Private_offsets[j])
	    return (int)(XWPSError::RangeCheck);
		Private_offsets[j] = offset;
		if (j == num_fonts)
	    break;
		XWPSFontType1 * pfd = FDArray[j];
		writer.writePrivate((subrs_size[j] == 0 ? 0 : Subrs_offsets[j]), pfd);
  }
  
  for (int j = 0; ; ++j)
  {
  	XWPSFontType1 *pfd;

		offset = writer.strm->tell() - (start_pos + Private_offsets[j]);
		if (offset > Subrs_offsets[j])
	    return (int)(XWPSError::RangeCheck);
		Subrs_offsets[j] = offset;
		if (j == num_fonts)
	    break;
		pfd = FDArray[j];
		if (writer.convertCharStrings(pfd))
	    writer.putIndexHeader(0, 0);
		else if (subrs_size[j] != 0)
	    writer.writeSubrs(subrs_count[j], subrs_size[j], pfd);
  }
  offset = writer.strm->tell() - start_pos;
  if (offset > End_offset)
		return (int)(XWPSError::RangeCheck);
			
	if (offset == End_offset)
	{
		if (writer.strm == &poss) 
		{
	    writer.strm = s;
	    goto write;
		}
	}
	else
	{
		End_offset = offset;
		goto iter;
	}
	return 0;
}

int XWPSFontCid0::z9glyphData(ulong glyph, XWPSString *pgstr, int *pfidx)
{
	XWPSFontData *pfdata = (XWPSFontData*)(client_data);
  long glyph_index = (long)(glyph - ps_min_cid_glyph);
  XWPSString gstr;
  ulong fidx;
  int code;

  if (!pfdata->u.cid0.GlyphDirectory->hasType(XWPSRef::Null)) 
  {
		code = i_ctx_p->fontGDirGetOutline(pfdata->u.cid0.GlyphDirectory, glyph_index, &gstr);
		if (code < 0)
	    return code;
	    
		if (gstr.data) 
		{
	    code = getIndex(&gstr, FDBytes, &fidx);
	    if (code < 0)
				return code;
	    if (fidx >= FDArray_size)
				return (int)(XWPSError::RangeCheck);
					
	    if (pgstr)
	    {
	    	if (gstr.bytes)
	    		gstr.bytes->incRef();
				*pgstr = gstr;
				gstr.bytes = 0;
			}
	    *pfidx = (int)fidx;
	    return code;
		}
  }
  
  if (glyph_index < 0 || glyph_index >= CIDCount) 
  {
		*pfidx = 0;
		if (pgstr)
	    pgstr->data = 0, pgstr->size = 0;
		return (int)(XWPSError::Undefined);
  }
  
  {
		uchar fd_gd[(MAX_FDBytes + MAX_GDBytes) * 2];
		uint num_bytes = FDBytes + GDBytes;
		ulong base = CIDMapOffset + glyph_index * num_bytes;
		ulong gidx, gidx_next;

		code = cid0ReadBytes(base, (ulong)(num_bytes * 2), fd_gd, &gstr);
		if (code < 0)
	    return code;
		getIndex(&gstr, FDBytes, &fidx);
		getIndex(&gstr, GDBytes, &gidx);
		gstr.data += FDBytes;
		gstr.size -= FDBytes;
		getIndex(&gstr, GDBytes, &gidx_next);
		if (gidx_next <= gidx) 
		{
	    *pfidx = 0;
	    if (pgstr)
				pgstr->data = 0, pgstr->size = 0;
	    return (int)(XWPSError::Undefined);
		}
		if (fidx >= FDArray_size)
			return (int)(XWPSError::RangeCheck);
	  
		*pfidx = (int)fidx;
		if (pgstr == 0)
	    return 0;
		return cid0ReadBytes(gidx, gidx_next - gidx, NULL, pgstr);
  }
}

int XWPSFontCid0::z9glyphOutline(ulong glyph, const XWPSMatrix *pmat, XWPSPath *ppath)
{
	XWPSRef gref;
  XWPSString gstr;
  int code, fidx, ocode;

  code = glyphData(glyph, &gstr, &fidx);
  if (code < 0)
		return code;
    
  i_ctx_p->glyphRef(glyph, &gref);
  ocode = i_ctx_p->zcharStringOutline(FDArray[fidx], &gref, &gstr,	pmat, ppath);
  return ocode;
}

XWPSFontCid1::XWPSFontCid1()
	:XWPSFontBase()
{
}

XWPSFontCid1::~XWPSFontCid1()
{
}

XWPSFont * XWPSFontCid1::copyFont()
{
	XWPSFontCid1 * newfont = new XWPSFontCid1;
	copyFontCid1(newfont);
	return newfont;
}

void XWPSFontCid1::copyFontCid1(XWPSFontCid1 * newfont)
{
	copyFontBase(newfont);
}

int XWPSFontCid1::getLength()
{
	return sizeof(XWPSFontCid1);
}

const char * XWPSFontCid1::getTypeName()
{
	return "fontcid1";
}

#define U8(p) ((uint)((p)[0]))
#define S8(p) (int)((U8(p) ^ 0x80) - 0x80)
#define U16(p) (((uint)((p)[0]) << 8) + (p)[1])
#define S16(p) (int)((U16(p) ^ 0x8000) - 0x8000)
#define u32(p) get_u32_msb(p)

XWPSFontType42::XWPSFontType42()
	:XWPSFontBase()
{
	glyf = 0;
	unitsPerEm = 0;
	indexToLocFormat = 0;
	memset(metrics, 0, 2 * sizeof(PSType42Mtx));
	loca = 0;
	numGlyphs = 0;
}

XWPSFontType42::~XWPSFontType42()
{
}

int XWPSFontType42::appendComponent(uint glyph_index, 
	                     const XWPSMatrixFixed * pmat,
		                   XWPSPath * ppath, 
		                   XWPSFixedPoint *ppts, 
		                   int point_index)
{
	const uchar *gdata;
  int code;

  code = checkComponent(glyph_index, pmat, ppath, ppts + point_index,  &gdata);
  if (code != 1)
		return code;
		
  {
		uint flags;

		gdata += 10;
		do 
		{
	    uint comp_index = U16(gdata + 2);
	    XWPSMatrixFixed mat;
	    int mp[2];

	    parseComponent(&gdata, &flags, &mat, mp, pmat);
	    if (mp[0] >= 0) 
	    {
				const XWPSFixedPoint *const pfrom = ppts + mp[0];
				const XWPSFixedPoint *const pto = ppts + point_index + mp[1];
				XWPSFixedPoint diff;

				code = appendComponent(comp_index, &mat, NULL, ppts, point_index);
				if (code < 0)
		    	return code;
				diff.x = pfrom->x - pto->x;
				diff.y = pfrom->y - pto->y;
				mat.tx = fixed2float(mat.tx_fixed += diff.x);
				mat.ty = fixed2float(mat.ty_fixed += diff.y);
	    }
	    code = appendComponent(comp_index, &mat, ppath, ppts, point_index);
	    if (code < 0)
				return code;
	    point_index += totalPoints(comp_index);
		}	while (flags & cg_moreComponents);
  }
  return 0;
}

int XWPSFontType42::appendOutline(uint glyph_index, 
	                  const XWPSMatrixFixed * pmat,
	                  XWPSPath * ppath)
{
	{
		const uchar *gdata;
		int code = checkComponent(glyph_index, pmat, ppath, NULL, &gdata);

		if (code != 1)
	    return code;
  }
  
  {
#define MAX_STACK_PTS 150	/* usually enough */
	
		int num_points = totalPoints(glyph_index);

		if (num_points <= MAX_STACK_PTS) 
		{
	    XWPSFixedPoint pts[MAX_STACK_PTS];

	    return appendComponent(glyph_index, pmat, ppath, pts, 0);
		} 
		else 
		{
	    XWPSFixedPoint *ppts = new XWPSFixedPoint[num_points];
	    int code = appendComponent(glyph_index, pmat, ppath, ppts, 0);
	    delete [] ppts;
	    return code;
		}
#undef MAX_STACK_PTS
  }
}

int XWPSFontType42::appendSimple(const uchar *gdata, 
	                  float sbw[4], 
	                  const XWPSMatrixFixed *pmat,
	                  XWPSPath *ppath, 
	                  XWPSFixedPoint *ppts)
{
	int numContours = S16(gdata);
  const uchar *pends = gdata + 10;
  const uchar *pinstr = pends + numContours * 2;
  const uchar *pflags;
  uint npoints;
  const uchar *pxc, *pyc;
  int code;

  if (numContours == 0)
		return 0;
		
  {
		const uchar *pf = pflags = pinstr + 2 + U16(pinstr);
		uint xbytes = npoints = U16(pinstr - 2) + 1;
		uint np = npoints;

		while (np > 0) 
		{
	    uchar flags = *pf++;
	    uint reps = (flags & gf_Repeat ? *pf++ + 1 : 1);

	    if (!(flags & gf_xShort)) 
	    {
				if (flags & gf_xSame)
		    	xbytes -= reps;
				else
		    	xbytes += reps;
	    }
	    np -= reps;
		}
		pxc = pf;
		pyc = pxc + xbytes;
  }

  {
		uint i, np;
		XWPSFixedPoint pt;
		double factor = 1.0 / unitsPerEm;
		uchar flags = *pflags++;
		uint reps = (flags & gf_Repeat ? *pflags++ + 1 : 1);
			
		pt.transform2fixed((XWPSMatrixFixed*)pmat, sbw[0] - S16(gdata + 2) * factor, 0.0);
		for (i = 0, np = 0; i < numContours; ++i) 
		{
	    bool move = true;
	    uint last_point = U16(pends + i * 2);
	    float dx, dy;
	    int off_curve = 0;
	    XWPSFixedPoint start;
	    XWPSFixedPoint cpoints[3];

	    for (; np <= last_point; --reps, ++np) 
	    {
				XWPSFixedPoint dpt;

				if (reps == 0) 
				{
		    	flags = *pflags++;
		    	reps = (flags & gf_Repeat ? *pflags++ + 1 : 1);
				}
				if (flags & gf_xShort) 
				{
		    	if (flags & gf_xPos)
						dx = *pxc++ * factor;
		    	else
						dx = -(int)*pxc++ * factor;
				} 
				else if (!(flags & gf_xSame))
		    	dx = S16(pxc) * factor, pxc += 2;
				else
		    	dx = 0;
				if (flags & gf_yShort) 
				{
		    	if (flags & gf_yPos)
						dy = *pyc++ * factor;
		    	else
						dy = -(int)*pyc++ * factor;
				} 
				else if (!(flags & gf_ySame))
		    	dy = S16(pyc) * factor, pyc += 2;
				else
		    	dy = 0;
				code = dpt.distanceTransform2fixed((XWPSMatrixFixed*)pmat, dx, dy);
				if (code < 0)
		    	return code;
				pt.x += dpt.x, pt.y += dpt.y;
				if (ppts)	
		    	ppts[np] = pt;
				if (ppath) 
				{
#define control1(xy) cpoints[1].xy
#define control2(xy) cpoints[2].xy
#define control3off(xy) ((cpoints[1].xy + pt.xy) / 2)
#define control4off(xy) ((cpoints[0].xy + 2 * cpoints[1].xy) / 3)
#define control5off(xy) ((2 * cpoints[1].xy + cpoints[2].xy) / 3)
#define control6off(xy) ((2 * cpoints[1].xy + pt.xy) / 3)
#define control7off(xy) ((2 * cpoints[1].xy + start.xy) / 3)
		    	if (move) 
		    	{
						start = pt;
						code = ppath->addPoint(pt.x, pt.y);
						cpoints[0] = pt;
						move = false;
		    	} 
		    	else if (flags & gf_OnCurve) 
		    	{
						if (off_curve)
			    		code = ppath->addCurveNotes(control4off(x),control4off(y), control6off(x),
										control6off(y), pt.x, pt.y, sn_none);
						else
			    		code = ppath->addLineNotes(pt.x, pt.y, sn_none);
						cpoints[0] = pt;
						off_curve = 0;
		    	} 
		    	else 
		    	{
						switch (off_curve++) 
						{
							default:
			    			control2(x) = control3off(x);
			    			control2(y) = control3off(y);
			    			code = ppath->addCurveNotes(control4off(x), control4off(y),
												control5off(x), control5off(y),	control2(x), control2(y), sn_none);
			    			cpoints[0] = cpoints[2];
			    			off_curve = 1;
			    			
							case 0:
			    			cpoints[1] = pt;
						}
		    	}
		    	if (code < 0)
						return code;
				}
	    }
	    if (ppath) 
	    {
				if (off_curve)
		    	code = ppath->addCurveNotes(control4off(x), control4off(y),	control7off(x), control7off(y),
																start.x, start.y, sn_none);
				code = ppath->closeSubpathNotes(sn_none);
				if (code < 0)
		    	return code;
	    }
		}
  }
  return 0;
}

int XWPSFontType42::checkComponent(uint glyph_index, 
	                   const XWPSMatrixFixed *pmat,
		                 XWPSPath *ppath, 
		                 XWPSFixedPoint *ppts,
										 const uchar **pgdata)
{
	XWPSString glyph_string;
  const uchar *gdata;
  float sbw[4];
  int numContours;
  int code;

  code = getOutline(glyph_index, &glyph_string);
  if (code < 0)
		return code;
    
  gdata = glyph_string.data;
  if (gdata == 0 || glyph_string.size == 0)	
		return 0;
  
  numContours = S16(gdata);
  if (numContours >= 0) 
  {
		simpleGlyphMetrics(glyph_index, WMode, sbw);
		code = appendSimple(gdata, sbw, pmat, ppath, ppts);
		return (code < 0 ? code : 0); /* simple */
  }
  if (numContours != -1)
		return (int)(XWPSError::RangeCheck);
  *pgdata = gdata;
  return 1;	
}

XWPSFont * XWPSFontType42::copyFont()
{
	XWPSFontType42 * newfont = new XWPSFontType42;
	copyFontType42(newfont);
	return newfont;
}

void XWPSFontType42::copyFontType42(XWPSFontType42 * newfont)
{
	copyFontBase(newfont);
	newfont->string_proc = string_proc;
	newfont->string_procc2 = string_procc2;
	newfont->proc_data = proc_data;
	newfont->get_outline = get_outline;
	newfont->get_outlinec2 = get_outlinec2;
	newfont->get_metrics = get_metrics;
	newfont->get_metricsc2 = get_metricsc2;
	newfont->glyf = glyf;
	newfont->unitsPerEm = unitsPerEm;
	newfont->indexToLocFormat = indexToLocFormat;
	newfont->loca = loca;
	newfont->numGlyphs = numGlyphs;
	newfont->metrics[0] = metrics[0];
	newfont->metrics[1] = metrics[1];
}

int XWPSFontType42::defaultGetMetrics(uint glyph_index, int wmode, float sbw[4])
{
	XWPSString glyph_string;
  int code = getOutline(glyph_index, &glyph_string);

  if (code < 0)
		return code;
    
  if (glyph_string.size != 0 && S16(glyph_string.data) == -1) 
  {
		uint flags;
		uchar *gdata = glyph_string.data + 10;
		XWPSMatrixFixed mat;

		memset(&mat, 0, sizeof(mat));
		do 
		{
	    uint comp_index = U16(gdata + 2);

	    parseComponent((const uchar**)&gdata, &flags, &mat, NULL, &mat);
	    if (flags & cg_useMyMetrics) 
				return type42WModeMetrics(comp_index, wmode, sbw);
		}	while (flags & cg_moreComponents);
  }
  return simpleGlyphMetrics(glyph_index, wmode, sbw);
}

int XWPSFontType42::defaultGetOutline(uint glyph_index, XWPSString * pglyph)
{
  uchar *ploca;
  ulong glyph_start;
  uint glyph_length;
  int code;
  
  if (indexToLocFormat) 
  {
  	code = stringProc(loca + glyph_index * 4, 4, (const uchar**)&ploca);
  	if ( code < 0 ) 
  		return code;
  	
		glyph_start = u32(ploca);
		
		code = stringProc(loca + glyph_index * 4 + 4, 4, (const uchar**)&ploca);
  	if ( code < 0 ) 
  		return code;
  	
		glyph_length = u32(ploca) - glyph_start;
  } 
  else 
  {
  	code = stringProc(loca + glyph_index * 2, 2, (const uchar**)&ploca);
  	if ( code < 0 ) 
  		return code;
  		
		glyph_start = (ulong) U16(ploca) << 1;
		
		code = stringProc(loca + glyph_index * 2 + 2, 4, (const uchar**)&ploca);
  	if ( code < 0 ) 
  		return code;
  		
		glyph_length = ((ulong) U16(ploca) << 1) - glyph_start;
  }
  pglyph->size = glyph_length;
  if (glyph_length == 0)
		pglyph->data = 0;
  else
  {
  	code = stringProc(glyf + glyph_start, glyph_length, (const uchar**)(&pglyph->data));
  	if ( code < 0 ) 
  		return code;
	}
  return 0;
}

int XWPSFontType42::getLength()
{
	return sizeof(XWPSFontType42);
}

const char * XWPSFontType42::getTypeName()
{
	return "fontcid42";
}

int XWPSFontType42::getMetrics(uint glyph_index, int wmode, float sbw[4])
{
	return (this->*get_metrics)(glyph_index, wmode, sbw);
}

int XWPSFontType42::getOutline(uint glyph_index, XWPSString *pgstr)
{
	return (this->*get_outline)(glyph_index, pgstr);
}

void XWPSFontType42::parseComponent(const uchar **pdata, 
	                  uint *pflags, 
	                  XWPSMatrixFixed *psmat,
										int *pmp , 
										const XWPSMatrixFixed *pmat)
{
	const uchar *gdata = *pdata;
  uint flags;
  double factor = 1.0 / unitsPerEm;
  XWPSMatrixFixed mat;
  XWPSMatrix scale_mat;

  flags = U16(gdata);
  gdata += 4;
  mat = *((XWPSMatrixFixed*)pmat);
  if (flags & cg_argsAreXYValues) 
  {
		int arg1, arg2;
		XWPSFixedPoint pt;

		if (flags & cg_argsAreWords)
	    arg1 = S16(gdata), arg2 = S16(gdata + 2), gdata += 4;
		else
	    arg1 = S8(gdata), arg2 = S8(gdata + 1), gdata += 2;
		if (flags & cg_roundXYToGrid) 
		{
	  	;
		}
		pt.transform2fixed((XWPSMatrixFixed*)pmat, arg1 * factor, arg2 * factor);
		mat.tx = fixed2float(mat.tx_fixed = pt.x);
		mat.ty = fixed2float(mat.ty_fixed = pt.y);
		if (pmp)
	    pmp[0] = pmp[1] = -1;
  } 
  else 
  {
		if (flags & cg_argsAreWords) 
		{
	    if (pmp)
				pmp[0] = U16(gdata), pmp[1] = S16(gdata + 2);
	    gdata += 4;
		} 
		else 
		{
	    if (pmp)
				pmp[0] = U8(gdata), pmp[1] = U8(gdata + 1);
	    gdata += 2;
		}
  }
#define S2_14(p) (S16(p) / 16384.0)
  
  if (flags & cg_haveScale) 
  {
		scale_mat.xx = scale_mat.yy = S2_14(gdata);
		scale_mat.xy = scale_mat.yx = 0;
		gdata += 2;
  } 
  else if (flags & cg_haveXYScale) 
  {
		scale_mat.xx = S2_14(gdata);
		scale_mat.yy = S2_14(gdata + 2);
		scale_mat.xy = scale_mat.yx = 0;
		gdata += 4;
  } 
  else if (flags & cg_have2x2) 
  {
		scale_mat.xx = S2_14(gdata);
		scale_mat.xy = S2_14(gdata + 2);
		scale_mat.yx = S2_14(gdata + 4);
		scale_mat.yy = S2_14(gdata + 6);
		gdata += 8;
  } 
  else
		goto no_scale;
#undef S2_14
    
  scale_mat.tx = 0;
  scale_mat.ty = 0;
  mat.matrixMultiply(&scale_mat, &mat);
  
no_scale:
  *pdata = gdata;
  *pflags = flags;
  *psmat = mat;
}

int XWPSFontType42::parsePieces(ulong glyph, ulong *pieces, int *pnum_pieces)
{
	uint glyph_index = glyph - ps_min_cid_glyph;
  XWPSString glyph_string;
  int code = getOutline(glyph_index, &glyph_string);

  if (code < 0)
		return code;
  if (glyph_string.size != 0 && S16(glyph_string.data) == -1) 
  {
		int i = 0;
		uint flags = cg_moreComponents;
		uchar *gdata = glyph_string.data + 10;
		XWPSMatrixFixed mat;

		memset(&mat, 0, sizeof(mat));
		for (i = 0; flags & cg_moreComponents; ++i) 
		{
	    if (pieces)
				pieces[i] = U16(gdata + 2) + ps_min_cid_glyph;
	    parseComponent((const uchar**)&gdata, &flags, &mat, NULL, &mat);
		}
		*pnum_pieces = i;
  } 
  else
		*pnum_pieces = 0;
  return 0;
}

int XWPSFontType42::simpleGlyphMetrics(uint glyph_index, int wmode, float sbw[4])
{
	double factor = 1.0 / unitsPerEm;
  uint width;
  int lsb;
  int code;

  {
		const PSType42Mtx *pmtx = &metrics[wmode];
		uint num_metrics = pmtx->numMetrics;
		uchar *pmetrics;

		if (glyph_index < num_metrics) 
		{
			code = stringProc(pmtx->offset + glyph_index * 4, 4, (const uchar**)&pmetrics);
  		if ( code < 0 ) 
  			return code;
  		
	    width = U16(pmetrics);
	    lsb = S16(pmetrics + 2);
		} 
		else 
		{
	    uint offset = pmtx->offset + num_metrics * 4;
	    uint glyph_offset = (glyph_index - num_metrics) * 2;
	    const uchar *plsb;

			code = stringProc(offset - 4, 4, (const uchar**)&pmetrics);
  		if ( code < 0 ) 
  			return code;
  			
	    width = U16(pmetrics);
	    if (glyph_offset >= pmtx->length)
				glyph_offset = pmtx->length - 2;
				
			code = stringProc(offset + glyph_offset, 2, &plsb);
  		if ( code < 0 ) 
  			return code;
  			
	    lsb = S16(plsb);
		}
  }
  if (wmode) 
  {
		factor = -factor;	
		sbw[0] = 0, sbw[1] = lsb * factor;
		sbw[2] = 0, sbw[3] = width * factor;
  } 
  else 
  {
		sbw[0] = lsb * factor, sbw[1] = 0;
		sbw[2] = width * factor, sbw[3] = 0;
  }
  return 0;
}

int XWPSFontType42::stringProc(ulong offset, uint length,	const uchar ** pdata)
{
	return (this->*string_proc)(offset, length, pdata);
}

int XWPSFontType42::totalPoints(uint glyph_index)
{
	XWPSString glyph_string;
  int code = getOutline(glyph_index, &glyph_string);
  uchar *gdata = glyph_string.data;

  if (code < 0)
		return code;
    
  if (glyph_string.size == 0)
		return 0;
    
  if (S16(gdata) != -1) 
  {
		int numContours = S16(gdata);
		uchar *pends = gdata + 10;
		uchar *pinstr = pends + numContours * 2;

		return (numContours == 0 ? 0 : U16(pinstr - 2) + 1);
  }
  
  gdata += 10;
  {
		uint flags;
		XWPSMatrixFixed mat;
		int total = 0;

		memset(&mat, 0, sizeof(mat));
		do 
		{
	    code = totalPoints(U16(gdata + 2));
	    if (code < 0)
				return code;
	    total += code;
	    parseComponent((const uchar**)&gdata, &flags, &mat, NULL, &mat);
		}	while (flags & cg_moreComponents);
		return total;
  }
}

int XWPSFontType42::type42Append(uint glyph_index, 
	                 XWPSImagerState * pis,
		 							 XWPSPath * ppath, 
		 							 const XWPSLog2ScalePoint * ,
		 							 bool , 
		 							 int )
{
	int code = appendOutline(glyph_index, &pis->ctm, ppath);

  if (code < 0)
		return code;
		
  pis->setFlat(pis->charFlatness(1.0)); return 0;
}

int XWPSFontType42::type42EnumerateGlyph(int *pindex, 
	                         PSGlyphSpace , 
	                         ulong *pglyph)
{
	while (++*pindex <= numGlyphs) 
	{
		XWPSString outline;
		uint glyph_index = *pindex - 1;
		int code = getOutline(glyph_index, &outline);

		if (code < 0)
	    return code;
		if (outline.data == 0)
	    continue;	
		*pglyph = glyph_index + ps_min_cid_glyph;
		return 0;
  }
  *pindex = 0;
  return 0;
}

int XWPSFontType42::type42FontInit()
{
  uchar *OffsetTable;
  uint numTables;
  uchar *TableDirectory;
  uint i;
  int code;
  uchar head_box[8];
  ulong loca_size = 0;
  
  code = stringProc(0, 12, (const uchar**)&OffsetTable);
  if ( code < 0 ) 
  	return code;

  {
		static const uchar version1_0[4] = {0, 1, 0, 0};
		static const uchar version_true[4] = {'t', 'r', 'u', 'e'};

		if (memcmp(OffsetTable, version1_0, 4) && memcmp(OffsetTable, version_true, 4))
		{
	    return (int)(XWPSError::InvalidFont);
	  }
  }
  
  numTables = (((uint)((OffsetTable + 4)[0]) << 8) + (OffsetTable + 4)[1]);
  code = stringProc(12, numTables * 16, (const uchar**)&TableDirectory);
  if ( code < 0 ) 
  	return code;
  	
  memset(metrics, 0, sizeof(metrics));
  for (i = 0; i < numTables; ++i) 
  {
		const uchar *tab = TableDirectory + i * 16;
		ulong offset = get_u32_msb(tab + 8);

		if (!memcmp(tab, "glyf", 4))
	    glyf = offset;
		else if (!memcmp(tab, "head", 4)) 
		{
	    const uchar *head;

			code = stringProc(offset, 54, &head);
  		if ( code < 0 ) 
  			return code;
  	
	    unitsPerEm = (((uint)((head + 18)[0]) << 8) + (head + 18)[1]);
	    memcpy(head_box, head + 36, 8);
	    indexToLocFormat = (((uint)((head + 50)[0]) << 8) + (head + 50)[1]); 
		} 
		else if (!memcmp(tab, "hhea", 4)) 
		{
	    const uchar *hhea;

			code = stringProc(offset, 36, &hhea);
  		if ( code < 0 ) 
  			return code;
  			
	    metrics[0].numMetrics = (((uint)((hhea + 34)[0]) << 8) + (hhea + 34)[1]); 
		} 
		else if (!memcmp(tab, "hmtx", 4)) 
		{
	    metrics[0].offset = offset;
	    metrics[0].length = (uint)get_u32_msb(tab + 12);
		} 
		else if (!memcmp(tab, "loca", 4)) 
		{
	    loca = offset;
	    loca_size = get_u32_msb(tab + 12);
		} 
		else if (!memcmp(tab, "vhea", 4)) 
		{
	    const uchar *vhea;

	    code = stringProc(offset, 36, &vhea);
  		if ( code < 0 ) 
  			return code;
	    metrics[1].numMetrics = (((uint)((vhea + 34)[0]) << 8) + (vhea + 34)[1]);
		} 
		else if (!memcmp(tab, "vmtx", 4)) 
		{
	    metrics[1].offset = offset;
	    metrics[1].length = (uint)get_u32_msb(tab + 12);
		}
  }
  loca_size >>= indexToLocFormat + 1;
  numGlyphs = (loca_size == 0 ? 0 : loca_size - 1);
  	
  if (FontBBox.p.x >= FontBBox.q.x ||
			FontBBox.p.y >= FontBBox.q.y ||
			FontBBox.p.x < -0.5 || FontBBox.p.x > 0.5 ||
			FontBBox.p.y < -0.5 || FontBBox.p.y > 0.5) 
	{
		float upem = unitsPerEm;
	
		FontBBox.p.x = S16(head_box) / upem;
		FontBBox.p.y = S16(head_box + 2) / upem;
		FontBBox.q.x = S16(head_box + 4) / upem;
		FontBBox.q.y = S16(head_box + 6) / upem;
  }
    
  get_outline = &XWPSFontType42::defaultGetOutline;
  get_metrics = &XWPSFontType42::defaultGetMetrics;
  procs.glyph_outline_.glyph_outlinet42 = &XWPSFontType42::type42GlyphOutline;
  procs.glyph_info_.glyph_infot42 = &XWPSFontType42::type42GlyphInfo;
  procs.enumerate_glyph_.enumerate_glypht42 = &XWPSFontType42::type42EnumerateGlyph;
  return 0;
}

int XWPSFontType42::type42GlyphInfo(ulong glyph, 
	                    const XWPSMatrix *pmat,
		                  int members, 
		                  XWPSGlyphInfo *info)
{
	uint glyph_index = glyph - ps_min_cid_glyph;
  int default_members =	members & ~(GLYPH_INFO_WIDTHS | GLYPH_INFO_NUM_PIECES | GLYPH_INFO_PIECES);
  XWPSString outline;
  int code = 0;

  if (default_members) 
  {
		code = glyphInfoDefault(glyph, pmat, default_members, info);

		if (code < 0)
	    return code;
  } 
  else if ((code = getOutline(glyph_index, &outline)) < 0)
		return code;
  else
		info->members = 0;
    
  if (members & GLYPH_INFO_WIDTH) 
  {
		int i;

		for (i = 0; i < 2; ++i)
	    if (members & (GLYPH_INFO_WIDTH0 << i)) 
	    {
				float sbw[4];

				code = type42WModeMetrics(glyph_index, i, sbw);
				if (code < 0)
		    	return code;
				if (pmat)
		    	code = info->width[i].transform(sbw[2], sbw[3], (XWPSMatrix*)pmat);
				else
		    	info->width[i].x = sbw[2], info->width[i].y = sbw[3];
	    }
		info->members |= members & GLYPH_INFO_WIDTH;
  }
  
  if (members & (GLYPH_INFO_NUM_PIECES | GLYPH_INFO_PIECES)) 
  {
		ulong *pieces = (members & GLYPH_INFO_PIECES ? info->pieces : 0);
		int code = parsePieces(glyph, pieces, &info->num_pieces);

		if (code < 0)
	    return code;
		info->members |= members & (GLYPH_INFO_NUM_PIECES | GLYPH_INFO_PIECES);
  }
  return code;
}

int XWPSFontType42::type42GetMetrics(uint glyph_index, float sbw[4])
{
	return type42WModeMetrics(glyph_index, WMode, sbw);
}

int XWPSFontType42::type42GlyphOutline(ulong glyph, const XWPSMatrix *pmat,	XWPSPath *ppath)
{
	uint glyph_index = glyph - ps_min_cid_glyph;
  XWPSFixedPoint origin;
  int code;
  XWPSGlyphInfo info;
  XWPSMatrixFixed fmat;
  XWPSMatrix imat;

  if (pmat == 0)
		pmat = &imat;
    
  if ((code = fmat.fromMatrix((XWPSMatrix*)pmat)) < 0 ||
			(code = ppath->currentPoint(&origin)) < 0 ||
			(code = appendOutline(glyph_index, &fmat, ppath)) < 0 ||
			(code = glyphInfo(glyph, pmat, GLYPH_INFO_WIDTH, &info)) < 0)
		return code;
    
  return ppath->addPoint(origin.x + float2fixed(info.width[0].x), origin.y + float2fixed(info.width[0].y));
}

int XWPSFontType42::type42WModeMetrics(uint glyph_index, int wmode,	float sbw[4])
{
	return getMetrics(glyph_index, wmode, sbw);
}

int XWPSFontType42::writeRange(XWPSStream *s, ulong start, uint length)
{
	ulong base = start;
  ulong limit = base + length;
  while (base < limit) 
  {
		uint size = limit - base;
		const uchar *ptr;
		int code;
		while ((code = stringProc(base, size, &ptr)) < 0) 
		{
	    if (size <= 1)
				return code;
	    size >>= 1;
		}
		s->write(ptr, size);
		base += size;
  }
  return 0;
}

int XWPSFontType42::writeTrueTypeData(XWPSStream *s, 
	                                    int options,
												              XWPSPSFGlyphEnum *penum, 
												              bool is_subset,
			                                const XWPSString *alt_font_name)
{
	XWPSString font_nameA;
	const uchar *OffsetTable;
  uint numTables_stored, numTables, numTables_out;
#define MAX_NUM_TABLES 40
  uchar tables[MAX_NUM_TABLES * 16];
  ulong offset;
  ulong glyph, glyph_prev;
  ulong max_glyph;
  uint glyf_length, glyf_checksum = 0;
  uint loca_length, loca_checksum[2];
  uint numGlyphs;
  uchar head[56];
  PSPost post;
  ulong head_checksum, file_checksum = 0;
  int indexToLocFormat;
  bool have_cmap = false,
				have_name = !(options & WRITE_TRUETYPE_NAME),
				have_OS_2 = false,
				have_post = false;
  uint cmap_length;
  ulong OS_2_start;
  uint OS_2_length = OS_2_LENGTH;
  int code, i;
  
  if (alt_font_name)
		font_nameA = *alt_font_name;
  else
  {
		font_nameA.data = font_name.chars;
	  font_nameA.size = font_name.size;
	}
	
	code = stringProc((ulong)(0), 12, &OffsetTable);
  if (code < 0) 
  	return code;
  	
  numTables_stored = U16(OffsetTable + 4);
  for (uint i = numTables = 0; i < numTables_stored; ++i)
  {
  	uchar *tab;
	  uchar *data;
	  ulong start;
	  uint length;
	  
	  code = stringProc((ulong)(12 + i * 16), 16, (const uchar**)&tab);
  	if (code < 0) 
  		return code;
  		
  	start = u32(tab + 8);
		length = u32(tab + 12);
		if (!memcmp(tab, "head", 4))
		{
			if (length != 54)
				return (int)(XWPSError::InvalidFont);
	    code = stringProc((ulong)(start), length, (const uchar**)&data);
  		if (code < 0) 
  			return code;
	    memcpy(head, data, length);
		}
		else if (
	    !memcmp(tab, "gly", 3) /*glyf=synthesized, glyx=Adobe bogus*/ ||
	    !memcmp(tab, "loc", 3) /*loca=synthesized, locx=Adobe bogus*/ ||
	    !memcmp(tab, "gdir", 4) /*Adobe marker*/ ||
	    ((options & WRITE_TRUETYPE_CMAP) && !memcmp(tab, "cmap", 4)))
	  {
	  	;
	  }
	  else
	  {
	  	if (numTables == MAX_NUM_TABLES)
				return (int)(XWPSError::LimitCheck);
	    if (!memcmp(tab, "cmap", 4))
				have_cmap = true;
	    else if (!memcmp(tab, "maxp", 4)) 
	    {
	    	code = stringProc((ulong)(start), length, (const uchar**)&data);
  			if (code < 0) 
  				return code;
				numGlyphs = U16(data + 4);
	    } 
	    else if (!memcmp(tab, "name", 4))
				have_name = true;
	    else if (!memcmp(tab, "OS/2", 4)) 
	    {
				have_OS_2 = true;
				if (length > OS_2_LENGTH)
		    	return (int)(XWPSError::InvalidFont);
				OS_2_start = start;
				OS_2_length = length;
				continue;
	    } 
	    else if (!memcmp(tab, "post", 4))
				have_post = true;
	    memcpy(&tables[numTables++ * 16], tab, 16);
	  }
  }
  
  for (max_glyph = 0, glyf_length = 0; (code = penum->enumerateGlyphsNext(&glyph)) != 1;)
  {
  	uint glyph_index;
		XWPSString glyph_string;
		if (glyph < ps_min_cid_glyph)
	    return (int)(XWPSError::InvalidFont);
	  glyph_index = glyph - ps_min_cid_glyph;
	  if (getOutline(glyph_index, &glyph_string) >= 0)
	  {
	  	max_glyph = qMax((uint)max_glyph, glyph_index);
	    glyf_length += glyph_string.size;
	  }
  }
  
  loca_length = (numGlyphs + 1) << 2;
  indexToLocFormat = (glyf_length > 0x1fffc);
  if (!indexToLocFormat)
		loca_length >>= 1;
		
	if (!have_post) 
	{
		memset(&post, 0, sizeof(post));
		if (options & WRITE_TRUETYPE_POST)
	    computePost(&post);
		else
	    post.length = 32;
  }
  
  memset(head + 8, 0, 4);
  head[51] = (uchar)indexToLocFormat;
  memset(head + 54, 0, 2);
  for (head_checksum = 0, i = 0; i < 56; i += 4)
		head_checksum += u32(&head[i]);
		
	numTables_out = numTables + 4 +	!have_cmap + !have_name + !have_post;
  offset = 12 + numTables_out * 16;
  for (i = 0; i < numTables; ++i) 
  {
		uchar *tab = &tables[i * 16];
		ulong length = u32(tab + 12);
		offset += ROUND_UP(length, 4);
  }
  
  {
  	uchar *tab = &tables[numTables * 16];
		offset = put_table(tab, "glyf", glyf_checksum, offset, glyf_length);
		tab += 16;
		offset = put_table(tab, "loca", loca_checksum[indexToLocFormat], offset, loca_length);
		tab += 16;
		if (!have_cmap) 
		{
	    cmap_length = sizeCmap(0xf000, 256, ps_min_cid_glyph + max_glyph, options);
	    offset = put_table(tab, "cmap", 0L , offset, cmap_length);
	    tab += 16;
		}
		if (!have_name) 
		{
	    offset = put_table(tab, "name", 0L, offset, sizeName(&font_nameA));
	    tab += 16;
		}
		offset = put_table(tab, "OS/2", 0L, offset, OS_2_length);
		tab += 16;
		if (!have_post) 
		{
	    offset = put_table(tab, "post", 0L, offset, post.length);
	    tab += 16;
		}
		offset = put_table(tab, "head", head_checksum, offset, 54);
		tab += 16;
  }
  numTables = numTables_out;
  {
		static const uchar version[4] = {0, 1, 0, 0};
		s->write(version, 4);
  }
  s->putUShort(numTables);
  {
  	int j = 0;
    for (j = 0; 1 << j <= numTables; ++j)
		;
    --j;
    s->putUShort(16 << j);
    s->putUShort(j);
    s->putUShort(numTables * 16 - (16 << j));
  }
  
  qsort(tables, numTables, 16, compare_table_tags);
  offset = 12 + numTables * 16;
  for (i = 0; i < numTables; ++i)
  {
  	uchar *tab = &tables[i * 16];
		uchar entry[16];
		memcpy(entry, tab, 16);
		if (entry[8] < 0x40) 
		{
	    uint length = u32(tab + 12);

	    put_u32(entry + 8, offset);
	    offset += ROUND_UP(length, 4);
		} 
		else 
		{
	    entry[8] -= 0x40;
		}
		s->write(entry, 16);
  }
  
  for (i = 0; i < numTables; ++i) 
  {
		uchar *tab = &tables[i * 16];
		if (tab[8] < 0x40) 
		{
	    ulong start = u32(tab + 8);
	    uint length = u32(tab + 12);

	    writeRange(s, start, length);
	    s->putPad(length);
		}
  }
  
  if (is_subset)
		penum->reset();
  else
		penum->listBegin(this, NULL, max_glyph + 1, GLYPH_SPACE_INDEX);
  for (offset = 0; penum->enumerateGlyphsNext(&glyph) != 1; ) 
  {
		XWPSString glyph_string;

		if (getOutline(glyph - ps_min_cid_glyph, &glyph_string) >= 0) 
		{
	    s->write(glyph_string.data, glyph_string.size);
	    offset += glyph_string.size;
		}
  }
 	s->putPad((uint)offset);
 	
 	penum->reset();
  glyph_prev = ps_min_cid_glyph;
  for (offset = 0; penum->enumerateGlyphsNext(&glyph) != 1; ) 
  {
		XWPSString glyph_string;
		for (; glyph_prev <= glyph; ++glyph_prev)
	    s->putLoca(offset, indexToLocFormat);
		if (getOutline(glyph - ps_min_cid_glyph, &glyph_string) >= 0)
	    offset += glyph_string.size;
  }
  
  for (; glyph_prev <= ps_min_cid_glyph + numGlyphs; ++glyph_prev)
		s->putLoca(offset, indexToLocFormat);
  s->putPad(loca_length);
  if (!have_cmap)
		writeCmap(s,  0xf000, 256, ps_min_cid_glyph + max_glyph, options, cmap_length);
  if (!have_name)
		writeName(s, &font_nameA);
  if (!have_OS_2)
		writeOS2(s, 0xf000, 256);
  else if (!have_cmap)
  {
  	uchar *pos2;
		ttf_OS_2_t os2;
		code = stringProc((ulong)(OS_2_start), OS_2_length, (const uchar**)&pos2);
  	if (code < 0) 
  		return code;
  	
		memcpy(&os2, pos2, qMin(OS_2_length, (uint)(sizeof(os2))));
		updateOS2(&os2, 0xf000, 256);
		s->write(&os2, OS_2_length);
		s->putPad(OS_2_length);
  }
  else
  {
  	writeRange(s, OS_2_start, OS_2_length);
		s->putPad(OS_2_length);
  }
  
  if (!have_post) 
  {
		if (options & WRITE_TRUETYPE_POST)
	    writePost(s, &post);
		else 
		{
	    uchar post_initial[32 + 2];

	    memset(post_initial, 0, 32);
	    put_u32(post_initial, 0x00030000);
	    s->write(post_initial, 32);
		}
  }
  
#if ARCH_SIZEOF_LONG > ARCH_SIZEOF_INT
#  define HEAD_MAGIC 0xb1b0afbaL
#else
#  define HEAD_MAGIC ((ulong)~0x4e4f5045)
#endif
  put_u32(head + 8, HEAD_MAGIC - file_checksum);
#undef HEAD_MAGIC
  s->write(head, 56);

  return 0;
}

int XWPSFontType42::writeTrueTypeFont(XWPSStream *s, 
	                                    int options,
			                                ulong *orig_subset_glyphs, 
			                                uint orig_subset_size,
											                const XWPSString *alt_font_name)
{
	XWPSPSFGlyphEnum genum;
  ulong subset_data[256 * MAX_COMPOSITE_PIECES];
  ulong *subset_glyphs = orig_subset_glyphs;
  uint subset_size = orig_subset_size;
  if (subset_glyphs)
  {
  	memcpy(subset_data, orig_subset_glyphs, sizeof(ulong) * subset_size);
		subset_glyphs = subset_data;
		int code = addSubsetPieces(subset_glyphs, &subset_size,
				                   sizeof(subset_data) / sizeof(subset_data[0]),
				                   sizeof(subset_data)/ sizeof(subset_data[0]));
		if (code < 0)
	    return code;
		subset_size = psf_sort_glyphs(subset_glyphs, subset_size);
  }
  
  genum.listBegin(this, subset_glyphs, (subset_glyphs ? subset_size : 0), GLYPH_SPACE_INDEX);
  return writeTrueTypeData(s, options, &genum,  subset_glyphs != 0, alt_font_name);
}

#undef U8
#undef S8
#undef U16
#undef S16
#undef u32

ulong XWPSFontType42::z42encodeChar(ulong chr, PSGlyphSpace glyph_space)
{
	ulong glyph = zfontEncodeChar(chr, glyph_space);

  return (glyph_space == GLYPH_SPACE_INDEX && glyph != ps_no_glyph ? glyphToIndex(glyph) : glyph);
}

int XWPSFontType42::z42enumerateGlyph(int *pindex, 
	                      PSGlyphSpace glyph_space,
		                    ulong *pglyph)
{
	if (glyph_space == GLYPH_SPACE_INDEX)
		return type42EnumerateGlyph(pindex, glyph_space, pglyph);
  else 
  {
  	XWPSFontData *pdata = (XWPSFontData*)client_data;
		XWPSRef *pcsdict = &pdata->CharStrings;

		return i_ctx_p->zcharEnumerateGlyph(pcsdict, pindex, pglyph);
  }
}

int XWPSFontType42::z42gdirEnumerateGlyph(int *pindex,
			                      PSGlyphSpace glyph_space, 
			                      ulong *pglyph)
{
	XWPSRef *pgdict;
	XWPSFontData *pdata = (XWPSFontData*)client_data;
  if (glyph_space == GLYPH_SPACE_INDEX) 
  {
		pgdict = pdata->u.type42.GlyphDirectory;
		if (!pgdict->hasType(XWPSRef::Dictionary)) 
		{
	    XWPSRef gdef;

	    for (;; (*pindex)++) 
	    {
				if (pgdict->arrayGet(i_ctx_p, (long)*pindex, &gdef) < 0) 
				{
		    	*pindex = 0;
		    	return 0;
				}
				if (!gdef.hasType(XWPSRef::Null)) 
				{
		    	*pglyph = ps_min_cid_glyph + (*pindex)++;
		    	return 0;
				}
	    }
		}
  } 
  else
		pgdict = &pdata->CharStrings;
  return i_ctx_p->zcharEnumerateGlyph(pgdict, pindex, pglyph);
}

int XWPSFontType42::z42gdirGetOutline(uint glyph_index, XWPSString * pgstr)
{
	const XWPSFontData *pfdata = (XWPSFontData*)(client_data);
  XWPSRef *pgdir = pfdata->u.type42.GlyphDirectory;

  return i_ctx_p->fontGDirGetOutline(pgdir, (long)glyph_index, pgstr);
}

int XWPSFontType42::z42glyphInfo(ulong glyph, 
	                 const XWPSMatrix *pmat,
	                 int members, 
	                 XWPSGlyphInfo *info)
{
	return type42GlyphInfo(glyphToIndex(glyph),	pmat, members, info);
}

int XWPSFontType42::z42glyphOutline(ulong glyph, const XWPSMatrix *pmat, XWPSPath *ppath)
{
	return type42GlyphOutline(glyphToIndex(glyph), pmat, ppath);
}

int XWPSFontType42::z42stringProc(ulong offset, uint length,const uchar ** pdata)
{
	const XWPSFontData *pfdata = (XWPSFontData*)(client_data);
	return i_ctx_p->stringArrayAccessProc(pfdata->u.type42.sfnts, 2, offset, length, pdata);
}

XWPSFontCid2::XWPSFontCid2()
	:XWPSFontType42()
{
	CIDCount = 0;
	GDBytes = 0;
	MetricsCount = 0;
}

XWPSFontCid2::~XWPSFontCid2()
{
}

XWPSFont * XWPSFontCid2::copyFont()
{
	XWPSFontCid2 * newfont = new XWPSFontCid2;
	copyFontCid2(newfont);
	return newfont;
}

void XWPSFontCid2::copyFontCid2(XWPSFontCid2 * newfont)
{
	copyFontType42(newfont);
	CIDCount = CIDCount;
	GDBytes = GDBytes;
	MetricsCount = MetricsCount;
	orig_procs = orig_procs;
}
	        
int XWPSFontCid2::getLength()
{
	return sizeof(XWPSFontCid2);
}

const char * XWPSFontCid2::getTypeName()
{
	return "fontcid2";
}

int XWPSFontCid2::getMetrics(uint glyph_index, int wmode, float sbw[4])
{
	return (this->*(orig_procs.get_metrics))(glyph_index, wmode, sbw);
}

int XWPSFontCid2::getOutline(uint glyph_index, XWPSString * pgstr)
{
	return (this->*(orig_procs.get_outline))(glyph_index, pgstr);
}

int XWPSFontCid2::writeCid2Font(XWPSStream *s, 
	                              int options,
		                            const uchar *subset_bits, 
		                            uint subset_size,
		    						            const XWPSString *alt_font_name)
{
	XWPSPSFGlyphEnum genum;

  genum.bitsBegin(this, subset_bits, (subset_bits ? subset_size : 0), GLYPH_SPACE_INDEX);
  return writeTrueTypeData(s, options, &genum,  subset_bits != 0, alt_font_name);
}

int XWPSFontCid2::z11CIDMapProc(ulong glyph)
{
	XWPSFontData *pdata = (XWPSFontData*)(client_data);
	XWPSRef *pcidmap = pdata->u.type42.CIDMap;
  ulong cid = glyph - ps_min_cid_glyph;
  int gdbytes = GDBytes;
  int gnum = 0;
  uchar *data;
  int i, code;
  XWPSRef rcid;
  XWPSRef *prgnum;

  switch (pcidmap->type()) 
  {
    case XWPSRef::String:
			if (cid >= pcidmap->size() / gdbytes)
	    	return (int)(XWPSError::RangeCheck);
			data = pcidmap->getBytes() + cid * gdbytes;
			break;
			
    case XWPSRef::Integer:
			return cid + pcidmap->value.intval;
			
    case XWPSRef::Dictionary:
			rcid.makeInt(cid);
			code = pcidmap->dictFind(i_ctx_p, &rcid, &prgnum);
			if (code <= 0)
	    	return (code < 0 ? code : (int)(XWPSError::Undefined));
			if (!prgnum->hasType(XWPSRef::Integer))
	    	return prgnum->checkTypeFailed();	
			return prgnum->value.intval;
			
    default:			/* array type */
			code = i_ctx_p->stringArrayAccessProc(pcidmap, 1, cid * gdbytes, gdbytes, (const uchar**)&data);

			if (code < 0)
	    	return code;
  }
  for (i = 0; i < gdbytes; ++i)
		gnum = (gnum << 8) + data[i];
  return gnum;
}

int XWPSFontCid2::z11getMetrics(uint glyph_index, int wmode,	float sbw[4])
{
	int skip = MetricsCount << 1;
  XWPSString gstr;
  uchar *pmetrics;
  int lsb, width;

  if (wmode > skip >> 2 || (this->*(orig_procs.get_outline))(glyph_index, &gstr) < 0 || gstr.size < skip)
		return (this->*(orig_procs.get_metrics))(glyph_index, wmode, sbw);
  pmetrics = gstr.data + skip - (wmode << 2);
  lsb = (pmetrics[2] << 8) + pmetrics[3];
  width = (pmetrics[0] << 8) + pmetrics[1];
  
  {
		double factor = 1.0 / unitsPerEm;

		if (wmode) 
		{
	    sbw[0] = 0, sbw[1] = -lsb * factor;
	    sbw[2] = 0, sbw[3] = -width * factor;
		} 
		else 
		{
	    sbw[0] = lsb * factor, sbw[1] = 0;
	    sbw[2] = width * factor, sbw[3] = 0;
		}
  }
  return 0;
}

int XWPSFontCid2::z11getOutline(uint glyph_index,	XWPSString * pgstr)
{
	int skip = MetricsCount << 1;
  int code = getOutline(glyph_index, pgstr);

  if (code >= 0) 
  {
		uchar *data = (uchar *)pgstr->data; 
		uint size = pgstr->size;

		if (size <= skip) 
		{
	    if (code > 0 && size != 0)
				free(data);
	    pgstr->data = 0, pgstr->size = 0;
		} 
		else 
		{
	    if (code > 0) 
				memmove(data, data + skip, size - skip);
	    else 
				pgstr->data += skip;
	    pgstr->size = size - skip;
		}
  }
  return code;
}

XWPSFontType0::XWPSFontType0()
	:XWPSFont()
{
	FMapType = fmap_8_8;
	EscChar = ShiftIn = ShiftOut = 0;
	subs_size = 0;
	subs_width = 0;
	Encoding = 0;
	encoding_size = 0;
	FDepVector = 0;
	fdep_size = 0;
	CMap = 0;
}

XWPSFontType0::~XWPSFontType0()
{
	if (Encoding)
	{
		delete [] Encoding;
		Encoding = 0;
	}
	
	if (FDepVector)
	{
		for (uint i = 0; i < fdep_size; i++)
		{
			if (FDepVector[i])
			{
				if (FDepVector[i]->decRef() == 0)
					delete FDepVector[i];
				FDepVector[i] = 0;
			}
		}
		free(FDepVector);
		FDepVector = 0;
	}
	
	if (CMap)
	{
		if (CMap->decRef() == 0)
			delete CMap;
		CMap = 0;
	}
}

XWPSFont * XWPSFontType0::copyFont()
{
	XWPSFontType0 * newfont = new XWPSFontType0;
	copyFontType0(newfont);
	return newfont;
}

void XWPSFontType0::copyFontType0(XWPSFontType0 * newfont)
{
	copyFontFont(newfont);
	newfont->FMapType = FMapType;
	newfont->ShiftIn = ShiftIn;
	newfont->ShiftOut = ShiftOut;
	
	newfont->subs_size = subs_size;
	newfont->subs_width = subs_width;
	newfont->encoding_size = encoding_size;
	newfont->fdep_size = fdep_size;
	CMap = newfont->CMap;
}

int XWPSFontType0::defineFontType0(XWPSFontDir * pdir)
{
	XWPSMatrix *pmat = &FontMatrix;
  if (pmat->xx == 1.0 && pmat->yy == 1.0 &&
		pmat->xy == 0.0 && pmat->yx == 0.0 &&
		pmat->tx == 0.0 && pmat->ty == 0.0)
		return 0;
  return type0AdjustMatrix(pdir, pmat);
}

int XWPSFontType0::getLength()
{
	return sizeof(XWPSFontType0);
}

const char * XWPSFontType0::getTypeName()
{
	return "fontt0";
}

int XWPSFontType0::makeFontType0(XWPSFontDir * pdir, 
		   							const XWPSMatrix * pmat, 
		   							XWPSFont ** ppfont)
{
	XWPSFontType0 * pfont= (XWPSFontType0*)(* ppfont);
	if (!pfont)
		return 0;
	return pfont->type0AdjustMatrix(pdir, pmat);
}

int XWPSFontType0::type0AdjustMatrix(XWPSFontDir * pdir, const XWPSMatrix * pmat)
{
	uint i;
	for (i = 0; i < fdep_size; i++)
		if (FDepVector[i]->FontType == ft_composite)
	    break;
  if (i == fdep_size)
		return 0;
		
	XWPSFont ** ptdep = (XWPSFont**)malloc((fdep_size+1)*sizeof(XWPSFont*));
	memcpy(ptdep, FDepVector, sizeof(XWPSFont *) * fdep_size);
	for (; i < fdep_size; i++)
		if (FDepVector[i]->FontType == ft_composite) 
		{
	    int code = FDepVector[i]->makeFont(pdir, pmat, &ptdep[i]);
	    if (code < 0)
	    {
	    	free(ptdep);
				return code;
			}
		}
	if (FDepVector)
		free(FDepVector);
  FDepVector = ptdep;
  return 0;
}

int XWPSFontType0::type0InitFStack(XWPSTextEnum *pte)
{
	if (!(pte->text.operation & (TEXT_FROM_STRING | TEXT_FROM_BYTES)))
	{
	  return (int)(XWPSError::InvalidFont);
	}
  
  pte->fstack.depth = 0;
  pte->fstack.items[0].font = this;
  pte->fstack.items[0].index = 0;
  return pte->stackModalFonts();
}

int XWPSFontType0::ztype0AdjustFDepVector()
{
  XWPSRef newdep;
  XWPSRef *prdep;
  uint i;
  XWPSFontData *pdata;
  
  newdep.makeArray(PS_A_READONLY | i_ctx_p->currentSpace(), fdep_size);
  
  prdep = newdep.getArray();
  for (i = 0; i < fdep_size; i++, prdep++) 
  {
  	pdata = (XWPSFontData*)(FDepVector[i]->client_data);
		XWPSRef *pdict = &pdata->dict;

		prdep->assign(pdict);
  }
  
  pdata = (XWPSFontData*)(client_data);
  return pdata->dict.dictPutString(i_ctx_p, "FDepVector", &newdep, NULL);
}

int XWPSFontType0::ztype0DefineFont(XWPSFontDir * pdir)
{
	XWPSFont **pdep = FDepVector;
	int code = defineFontType0(pdir);

  if (code < 0 || FDepVector == pdep)
		return code;
  return ztype0AdjustFDepVector();
}

int XWPSFontType0::ztype0MakeFont(XWPSFontDir * pdir, 
		 								 const XWPSMatrix * pmat, 
		 								 XWPSFont ** ppfont)
{
	XWPSFontType0 **ppfont0 = (XWPSFontType0 **)ppfont;
  XWPSFont **pdep = (*ppfont0)->FDepVector;
	int code = zdefaultMakeFont(pdir, pmat, ppfont);
  if (code < 0)
		return code;
    
  code = makeFontType0(pdir, pmat, ppfont);
  if (code < 0)
		return code;
		
	if ((*ppfont0)->FDepVector == pdep)
		return 0;
   
  return ztype0AdjustFDepVector();
}
