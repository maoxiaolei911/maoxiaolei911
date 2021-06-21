/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSState.h"
#include "XWPSFont.h"
#include "XWPSPath.h"
#include "XWPSDeviceMem.h"
#include "XWPSDeviceClip.h"
#include "XWPSDevicePDF.h"
#include "XWPSDeviceDVI.h"
#include "XWPSContextState.h"
#include "XWPSText.h"


XWPSTextParams::XWPSTextParams()
{
	operation = 0;
	data.bytes = 0;
	size = 0;
	
	space.s_char = 0;
	x_widths = 0;
	y_widths = 0;
	widths_size = 0;
	p_bytes = 0;
}

int XWPSTextParams::replacedWidth(uint index, XWPSPoint *pwidth)
{
	if (index > size)
		return (int)(XWPSError::RangeCheck);
    
  if (x_widths == y_widths) 
  {
		if (x_widths) 
		{
	    index *= 2;
	    pwidth->x = x_widths[index];
	    pwidth->y = x_widths[index + 1];
		}
		else
	    pwidth->x = pwidth->y = 0;
  } 
  else 
  {
		pwidth->x = (x_widths ? x_widths[index] : 0.0);
		pwidth->y = (y_widths ? y_widths[index] : 0.0);
  }
  return 0;
}

XWPSTextReturned::XWPSTextReturned()
{
	current_char = 0;
	current_glyph = 0;
}

XWPSFontStackItem::XWPSFontStackItem()
{
	font = 0;
	index = 0;
}

XWPSFontStack::XWPSFontStack()
{
	depth = -1;
}

#define select_descendant(pfont, pdata, fidx, fdepth)\
  if (fidx >= pdata->encoding_size)\
    return (int)(XWPSError::RangeCheck);\
  if (fdepth == MAX_FONT_STACK)\
    return (int)(XWPSError::InvalidFont);\
  pfont = pdata->FDepVector[pdata->Encoding[fidx]];\
  if (++fdepth > orig_depth || pfont != fstack.items[fdepth].font)\
    fstack.items[fdepth].font = pfont, changed = 1;\
  fstack.items[fdepth].index = fidx
  
XWPSTextEnum::XWPSTextEnum()
	:XWPSStruct()
{
	dev = 0;
	pis = 0;
	orig_font = 0;
	path = 0;
	pdcolor = 0;
	pcpath = 0;
	current_font = 0;
	index = 0;
	xy_index = 0;
	cmap_code = 0;
}

XWPSTextEnum::~XWPSTextEnum()
{
	if (text.p_bytes)
	{
		if (text.p_bytes->decRef() == 0)
			delete text.p_bytes;
		text.p_bytes = 0;
	}
}

void XWPSTextEnum::copy(XWPSTextEnum * pfrom)
{
	if (text.p_bytes != pfrom->text.p_bytes)
	{
		if (text.p_bytes)
			if (text.p_bytes->decRef() == 0)
			  delete text.p_bytes;
			  
		if (pfrom->text.p_bytes)
			pfrom->text.p_bytes->incRef();
	}
	text = pfrom->text;	
	dev = pfrom->dev;
	pis = pfrom->pis;
	orig_font = pfrom->orig_font;
	path = pfrom->path;
	pdcolor = pfrom->pdcolor;
	pcpath = pfrom->pcpath;
	current_font = pfrom->current_font;
	log2_scale = pfrom->log2_scale;
	index = pfrom->index;
	xy_index = pfrom->xy_index;
	fstack = pfrom->fstack;
	cmap_code = pfrom->cmap_code;
	FontBBox_as_Metrics2 = pfrom->FontBBox_as_Metrics2;
	returned = pfrom->returned;
}

void XWPSTextEnum::copyDynamic(XWPSTextEnum *pfrom, bool for_return)
{
	int depth = pfrom->fstack.depth;

  current_font = pfrom->current_font;
  index = pfrom->index;
  xy_index = pfrom->xy_index;
  fstack.depth = depth;
  FontBBox_as_Metrics2 = pfrom->FontBBox_as_Metrics2;
  if (depth >= 0)
		memcpy(fstack.items, pfrom->fstack.items, (depth + 1) * sizeof(fstack.items[0]));
  if (for_return) 
  {
		cmap_code = pfrom->cmap_code;
		returned = pfrom->returned;
  }
}

int XWPSTextEnum::getLength()
{
	return sizeof(XWPSTextEnum);
}

const char * XWPSTextEnum::getTypeName()
{
	return "textenum";
}

int  XWPSTextEnum::init(XWPSDevice *devA, 
	                      XWPSImagerState *pisA,
		  							    XWPSTextParams *textA, 
		  							    XWPSFont *fontA, 
		  							    XWPSPath *pathA,
		  							    XWPSDeviceColor *pdcolorA, 
		  							    XWPSClipPath *pcpathA)
{
	int code;
	
	if (text.p_bytes != textA->p_bytes)
	{
		if (text.p_bytes)
			if (text.p_bytes->decRef() == 0)
				delete text.p_bytes;
		if (textA->p_bytes)
			textA->p_bytes->incRef();
	}
	text = *textA;  	
  dev = devA;
  pis = pisA;
  orig_font = fontA;
  path = pathA;
  pdcolor = pdcolorA;
  pcpath = pcpathA;
  log2_scale.x = log2_scale.y = 0;
  code = initDynamic(fontA);
  return code;
}

int XWPSTextEnum::initDynamic(XWPSFont *fontA)
{
	current_font = fontA;
  index = 0;
  xy_index = 0;
  FontBBox_as_Metrics2.x = FontBBox_as_Metrics2.y = 0;
  return fontA->initFStack(this);
}

ulong XWPSTextEnum::nextChar()
{
	const uint operation = text.operation;

  if (index >= text.size)
		return ps_no_char;
  
  if (operation & (TEXT_FROM_STRING | TEXT_FROM_BYTES))
		return text.data.bytes[index];
  
  if (operation & TEXT_FROM_CHARS)
		return text.data.chars[index];
  return ps_no_char;
}

int XWPSTextEnum::nextCharGlyphDefault(ulong *pchr, ulong *pglyph)
{
	if (index >= text.size)
		return 2;
    
  if (text.operation & (TEXT_FROM_STRING | TEXT_FROM_BYTES)) 
  {
		*pchr = text.data.bytes[index];
		*pglyph = ps_no_glyph;
  } 
  else if (text.operation & TEXT_FROM_SINGLE_GLYPH) 
  {
		*pchr = ps_no_char;
		*pglyph = text.data.d_glyph;
  } 
  else if (text.operation & TEXT_FROM_GLYPHS) 
  {
		*pchr = ps_no_char;
		*pglyph = text.data.glyphs[index];
  } 
  else if (text.operation & TEXT_FROM_SINGLE_CHAR) 
  {
		*pchr = text.data.d_char;
		*pglyph = ps_no_glyph;
  } 
  else if (text.operation & TEXT_FROM_CHARS) 
  {
		*pchr = text.data.chars[index];
		*pglyph = ps_no_glyph;
  } 
  else
		return (int)(XWPSError::RangeCheck); 
  index++;
  return 0;
}

int XWPSTextEnum::nextCharGlyphType0(ulong *pchr, ulong *pglyph)
{
	uchar *str = (uchar*)text.data.bytes;
  uchar *p = str + index;
  uchar *end = str + text.size;
  int fdepth = fstack.depth;
  int orig_depth = fdepth;
  XWPSFont *pfont;

#define pfont0 ((XWPSFontType0 *)pfont)
    
  XWPSFontType0 *pdata;
  uint fidx;
  ulong chr;
  ulong glyph = ps_no_glyph;
  int changed = 0;

  FontBBox_as_Metrics2.x = FontBBox_as_Metrics2.y = 0;

#define need_left(n)\
  if ( end - p < n ) return (int)(XWPSError::RangeCheck)


  if (index == 0) 
  {
		int idepth = 0;

		pfont = fstack.items[0].font;
		for (; pfont->FontType == ft_composite;) 
		{
	    PSFMapType fmt = (pdata = pfont0)->FMapType;

	    if (p == end)
				return 2;
	    chr = *p;
	    switch (fmt) 
	    {
				case fmap_escape:
		    	if (chr != rootEscChar())
						break;
		    	need_left(2);
		    	fidx = p[1];
		    	p += 2;
		    	
rdown:
					select_descendant(pfont, pdata, fidx, idepth);
		    	continue;
		    	
				case fmap_double_escape:
		    	if (chr != rootEscChar())
						break;
						
		    	need_left(2);
		    	fidx = p[1];
		    	p += 2;
		    	if (fidx == chr) 
		    	{
						need_left(1);
						fidx = *p++ + 256;
		    	}
		    	
		    	goto rdown;
		    	
				case fmap_shift:
		    	if (chr == pdata->ShiftIn)
						fidx = 0;
		    	else if (chr == pdata->ShiftOut)
						fidx = 1;
		    	else
						break;
		    	p++;
		    	goto rdown;
		    	
				default:
		    	break;
	    }
	    break;
		}
		
		if (idepth != 0) 
		{
	    int code;

	    fstack.depth = idepth;
	    code = stackModalFonts();
	    if (code < 0)
				return code;
	    if (fstack.depth > idepth)
				changed = 1;
	    orig_depth = fdepth = fstack.depth;
		}
  }
  
up:
	if (p == end)
		return 2;
    
  chr = *p;
  while (fdepth > 0) 
  {
		pfont = fstack.items[fdepth - 1].font;
		pdata = pfont0;
		switch (pdata->FMapType) 
		{
	    default:
				fdepth--;
				continue;

	    case fmap_escape:
				if (chr != rootEscChar())
		    	break;
				need_left(2);
				fidx = *++p;
				if (fidx == chr && fdepth > 1) 
				{
		    	fdepth--;
		    	goto up;
				}
	      
down:
				if (++p == end)
		    	return 2;
				chr = *p;
				fdepth--;
				do 
				{
		    	select_descendant(pfont, pdata, fidx, fdepth);
		    	if (pfont->FontType != ft_composite)
						break;
		    	pdata = pfont0;
		    	fidx = 0;
				}	while (pdata->FMapType == fmap_escape);
				continue;

	    case fmap_double_escape:
				if (chr != rootEscChar())
		    	break;
				need_left(2);
				fidx = *++p;
				if (fidx == chr) 
				{
		    	need_left(2);
		    	fidx = *++p + 256;
				}
				goto down;

	    case fmap_shift:
				if (chr == pdata->ShiftIn)
		    	fidx = 0;
				else if (chr == pdata->ShiftOut)
		    	fidx = 1;
				else
		    	break;
				goto down;
		}
		break;
  }
  p++;

  while ((pfont = fstack.items[fdepth].font)->FontType == ft_composite) 
  {
		pdata = pfont0;
		switch (pdata->FMapType) 
		{
	    default:
				return (int)(XWPSError::InvalidFont);

	    case fmap_8_8:
				need_left(1);
				fidx = chr;
				chr = *p++;
				break;

	    case fmap_1_7:
				fidx = chr >> 7;
				chr &= 0x7f;
				break;

	    case fmap_9_7:
				need_left(1);
				fidx = ((uint) chr << 1) + (*p >> 7);
				chr = *p & 0x7f;
				p++;
				break;

	    case fmap_SubsVector:
				{
		    	int width = pdata->subs_width;
		    	uint subs_count = pdata->subs_size;
			    uchar *psv = pdata->SubsVector.data;

#define subs_loop(subs_elt, width)\
  while ( subs_count != 0 && tchr >= (schr = subs_elt) )\
    subs_count--, tchr -= schr, psv += width;\
  chr = tchr; p += width - 1; break

		    	switch (width) 
		    	{
						default:
			    		return (int)(XWPSError::InvalidFont);
			    		
						case 1:
			    		{
								uchar tchr = (uchar) chr, schr;

								subs_loop(*psv, 1);
			    		}
			    		
						case 2:
			    		need_left(1);
#define w2(p) (((ushort)*p << 8) + p[1])
			    		{
								ushort tchr = ((ushort) chr << 8) + *p, schr;

								subs_loop(w2(psv), 2);
			    		}
			    		
						case 3:
			    		need_left(2);
#define w3(p) (((ulong)*p << 16) + ((uint)p[1] << 8) + p[2])
			    		{
								ulong tchr = ((ulong) chr << 16) + w2(p), schr;

								subs_loop(w3(psv), 3);
			    		}
			    		
						case 4:
			    		need_left(3);
#define w4(p) (((ulong)*p << 24) + ((ulong)p[1] << 16) + ((uint)p[2] << 8) + p[3])
			    		{
								ulong tchr = ((ulong) chr << 24) + w3(p),	schr;

								subs_loop(w4(psv), 4);
			    		}
#undef w2
#undef w3
#undef w4
#undef subs_loop
		    	}
		    	fidx = pdata->subs_size - subs_count;
		    	break;
				}

	    case fmap_CMap:
				{
		    	XWPSString cstr;
		    	uint mindex = p - str - 1;
		    	int code;

		    	cstr.data = str;
		    	cstr.size = end - str;
		    	code = pdata->CMap->decodeNext((const XWPSString *)&cstr, &mindex,  &fidx, &chr, &glyph);
		    if (code < 0)
					return code;
		    cmap_code = code;
		    p = str + mindex;
		    if (code == 0) 
		    {
					if (glyph == ps_no_glyph) 
					{
			    	glyph = ps_min_cid_glyph;
			    	goto done;
					}
		    } 
		    else
					chr = (ulong) glyph, glyph = ps_no_glyph;
		    break;
			}
		}

		select_descendant(pfont, pdata, fidx, fdepth);
		if (pfont->FontType == ft_CID_encrypted ||
	    	pfont->FontType == ft_CID_TrueType) 
	  {
	    XWPSFontBase *pfb = (XWPSFontBase *)pfont;

	    FontBBox_as_Metrics2 = pfb->FontBBox.q;
		}
  }
  
done:
  *pchr = chr;
  *pglyph = glyph;
  if (str == text.data.bytes)
		index = p - str;
  fstack.depth = fdepth;
  return changed;
#undef pfont0
}

int XWPSTextEnum::restart(XWPSTextParams *textA)
{
	XWPSTextEnum tenum;

  tenum = *this;
  tenum.text = *textA;
  if (tenum.text.p_bytes)
  	tenum.text.p_bytes->incRef();
  tenum.initDynamic(orig_font);
  return resync(&tenum);
}

uint XWPSTextEnum::rootEscChar()
{
	return ((XWPSFontType0 *) (fstack.items[0].font))->EscChar;
}

int XWPSTextEnum::setCacheDevice(const double wbox[6])
{
	return setCache(wbox, TEXT_SET_CACHE_DEVICE);
}

int XWPSTextEnum::setCacheDevice2(const double wbox2[10])
{
	return setCache(wbox2, TEXT_SET_CACHE_DEVICE2);
}

int XWPSTextEnum::setCharWidth(const double wxyA[2])
{
	return setCache(wxyA, TEXT_SET_CHAR_WIDTH);
}

bool XWPSTextEnum::showIsAddToAll()
{
	return showIs(TEXT_ADD_TO_ALL_WIDTHS);
}

bool XWPSTextEnum::showIsAddToSpace()
{
	return showIs(TEXT_ADD_TO_SPACE_WIDTH);
}

bool XWPSTextEnum::showIsDoKern()
{
	return showIs(TEXT_INTERVENE);
}

bool XWPSTextEnum::showIsDrawing()
{
	return showIs(TEXT_DO_NONE);
}

bool XWPSTextEnum::showIsSlow()
{
	return showIs(TEXT_REPLACE_WIDTHS | TEXT_ADD_TO_ALL_WIDTHS | TEXT_ADD_TO_SPACE_WIDTH | TEXT_INTERVENE);
}

bool XWPSTextEnum::showIsStringWidth()
{
	return showIsAllOf(TEXT_DO_NONE | TEXT_RETURN_WIDTH);
}

int XWPSTextEnum::stackModalFonts()
{
	int fdepth = fstack.depth;
  XWPSFont *cfont = fstack.items[fdepth].font;

  while (cfont->FontType == ft_composite) 
  {
		XWPSFontType0 * cmfont = (XWPSFontType0 *) cfont;

		if (!fmap_type_is_modal(cmfont->FMapType))
	    break;
		if (fdepth == MAX_FONT_STACK)
		{
	  	return (int)(XWPSError::InvalidFont);
	  }
		fdepth++;
		cfont = cmfont->FDepVector[cmfont->Encoding[0]];
		fstack.items[fdepth].font = cfont;
		fstack.items[fdepth].index = 0;
  }
  fstack.depth = fdepth;
  return 0;
}

int XWPSTextEnum::totalWidth(XWPSPoint *pwidth)
{
	*pwidth = returned.total_width;
  return 0;
}

bool XWPSTextEnum::zcharShowWidthOnly()
{
	if (!isWidthOnly())
		return false;
		
  switch (orig_font->FontType) 
  {
    case ft_encrypted:
    case ft_encrypted2:
    case ft_CID_encrypted:
    case ft_CID_TrueType:
    case ft_CID_bitmap:
    case ft_TrueType:
			return true;
			
    default:
			return false;
  }
}

static bool CACHE_ROTATED_CHARS = true;
static bool OVERSAMPLE = true;
static uint MAX_TEMP_BITMAP_BITS = 80000;

#define CURRENT_CHAR (returned.current_char)
#define SET_CURRENT_CHAR(chr)\
  (returned.current_char = (chr))
#define CURRENT_GLYPH (returned.current_glyph)
#define SET_CURRENT_GLYPH(glyph)\
  (returned.current_glyph = (glyph))

#define SHOW_USES_OUTLINE\
  !showIs(TEXT_DO_NONE | TEXT_DO_CHARWIDTH)

XWPSShowEnum::XWPSShowEnum(XWPSState *pgsA)
	:XWPSTextEnum()
{
	auto_release = true;
	pgs = pgsA;
	level = 0;
	charpath_flag = cpm_show;
	show_gstate = 0;
	can_cache = 0;
	ftx = fty = 0;
	dev_cache = 0;
	dev_cache2 = 0;
	dev_null = 0;
	cc = 0;
	width_status = sws_none;
	proc.continue_proc_.continue_proc = &XWPSShowEnum::continueShow;
}

XWPSShowEnum::~XWPSShowEnum()
{
	if (dev_cache)
	{
		if (!dev_cache->decRef())
			delete dev_cache;
			
		dev_cache = 0;
	}
	
	if (dev_cache2)
	{
		if (!dev_cache2->decRef())
			delete dev_cache2;
			
		dev_cache2 = 0;
	}
	
	if (dev_null)
	{
		if (!dev_null->decRef())
			delete dev_null;
		dev_null = 0;
	}
}

int  XWPSShowEnum::continueKShow()
{
	int code = showStateSetup();

  if (code < 0)
		return code;
  return showProceed();
}

int XWPSShowEnum:: continueShow()
{
	return showProceed();
}

int  XWPSShowEnum::continueShowUpdate()
{
	int code = showUpdate();
  if (code < 0)
		return code;
    
  code = showMove();
  if (code != 0)
		return code;
  return showProceed();
}

int  XWPSShowEnum::currentWidth(XWPSPoint *pwidth)
{
	return pgs->idtransform(fixed2float(wxy.x),fixed2float(wxy.y), pwidth);
}

int XWPSShowEnum::getLength()
{
	return sizeof(XWPSShowEnum);
}

const char * XWPSShowEnum::getTypeName()
{
	return "showenum";
}

int  XWPSShowEnum::imageCachedChar(XWPSCachedChar * ccA)
{
	XWPSDeviceColor *pdevc = pgs->dev_color;
  int x, y, w, h, depth;
  int code;
  XWPSFixedPoint pt;
  XWPSDevice *dev = pgs->currentDevice();
  XWPSDevice *orig_dev = dev;
  XWPSDeviceClip cdev;
  cdev.incRef();
  cdev.context_state = pgs->i_ctx_p;
  ulong xg = ccA->xglyph;
  XWPSXFont *xf;
  uchar *bits;
  XWPSLog2ScalePoint scale_log2_1;
  
top:
	code = pgs->path->currentPoint(&pt);
  if (code < 0)
		return code;
		
	pt.x -= ccA->offset.x;
  x = fixed2int_var_rounded(pt.x) + ftx;
  pt.y -= ccA->offset.y;
  y = fixed2int_var_rounded(pt.y) + fty;
  w = ccA->width;
  h = ccA->height;
  if ((x < ibox.p.x || x + w > ibox.q.x ||
	 		y < ibox.p.y || y + h > ibox.q.y) &&
			dev != &cdev)
	{
		XWPSClipPath *pcpathA;

		if (x >= obox.q.x || x + w <= obox.p.x ||
	    	y >= obox.q.y || y + h <= obox.p.y )
	    return 0;
	    
	  code = pgs->effectiveClipPath(&pcpathA);
		if (code < 0)
	    return code;
	    
	  cdev.translate(pcpathA->getList(),0, 0);
	  cdev.target = dev;
	  dev->incRef();
		dev = &cdev;
		dev->open();
	}
	
	if (xg != ps_no_xglyph && (xf = ccA->pair->xfont) != 0)
	{
		int cx = x + fixed2int(ccA->offset.x);
		int cy = y + fixed2int(ccA->offset.y);
		if (pgs->colorWritesPure())
		{
			code = xf->renderChar(xg, dev, cx, cy, pdevc->colors.pure, 0);
			if (code == 0)
				return -1;
		}
		
		if (!ccA->hasBits())
		{
			XWPSDeviceMem mdev;
			mdev.incRef();
			mdev.context_state = pgs->i_ctx_p;
			mdev.makeMemMonoDevice(dev);
			mdev.openCacheDevice(ccA);
	  	code = xf->renderChar(xg, &mdev, cx - x, cy - y, 1, 1);
	  	if (code != 0)
				return -1;
			
			ccA->pair->font->dir->addCharBits(ccA, &scale_log2_1);
			if (pgs->i_ctx_p)
				ccA->id = pgs->i_ctx_p->nextIDS(1);
			xg = ps_no_xglyph;
	    	goto top;
		}
	}
	
	bits = cc_bits(ccA);
  depth = ccA->depth;
  if (!lop_no_S_is_T(pgs->log_op))
  {
  	XWPSClipPath *pcpathA;

		code = pgs->effectiveClipPath(&pcpathA);
		if (code >= 0) 
		{
	    code = orig_dev->fillMask(bits, 0, ccA->raster, ccA->id, x, y, w, h, pdevc, depth, pgs->log_op, pcpathA);
	    if (code >= 0)
				goto done;
		}
  }
  else if (pgs->colorWritesPure())
  {
  	ulong color = pdevc->colors.pure;

		if (depth > 1) 
		{
	    code = dev->copyAlpha(bits, 0, ccA->raster, ccA->id, x, y, w, h, color, depth);
	    if (code >= 0)
				return -1;
	    bits = ccA->compressAlphaBits();
	    if (bits == 0)
				return 1;	
		}
		code = dev->copyMono(bits, 0, ccA->raster, ccA->id, x, y, w, h, ps_no_color_index, pdevc->colors.pure);
		goto done;
  }
  
  if (depth > 1)
  {
  	bits = ccA->compressAlphaBits();
		if (bits == 0)
	    return 1;
  }
  
  {
  	uint used;
  	XWPSImage1 image;
  	image.initMask(true);
  	XWPSImageEnums *pie = new XWPSImageEnums();
  	
  	image.ImageMatrix.makeTranslation(-x, -y);
  	XWPSMatrix ctm;
  	pgs->ctmOnly(&ctm);
		image.ImageMatrix.matrixMultiply(&ctm, &image.ImageMatrix);
		image.Width = w;
		image.Height = h;
		image.adjust = false;
		code = pie->imageInit(&image, false, pgs);
		switch (code)
		{
			case 1:	
				code = 0;
				
	    default:
				break;
				
	    case 0:
				for (int iy = 0; iy < h && code >= 0; iy++)
		    	code = pie->imageNext(bits + iy * ccA->raster, (w + 7) >> 3, &used);
		}
		
		delete pie;
  }
  
 done:
 	if (bits != cc_bits(ccA))
		free(bits);
  if (code > 0)
		code = 0;
  
  return code;
}

bool XWPSShowEnum::isWidthOnly()
{
	return ((!SHOW_USES_OUTLINE || cc != 0) && pgs->level == level + 1);
}

int XWPSShowEnum::process()
{
	return (this->*(proc.continue_proc_.continue_proc))();
}

int XWPSShowEnum::resync(XWPSTextEnum *pfrom)
{
	int old_index = index;

  if ((text.operation ^ pfrom->text.operation) & ~TEXT_FROM_ANY)
		return (int)(XWPSError::RangeCheck);
	if (text.p_bytes != pfrom->text.p_bytes)
	{
		if (text.p_bytes)
			if (text.p_bytes->decRef() == 0)
				delete text.p_bytes;
		if (pfrom->text.p_bytes)
			pfrom->text.p_bytes->incRef();
	}
  text = pfrom->text;
  if (index == old_index) 
		return 0;
  else
		return showStateSetup();
}

int XWPSShowEnum::retry()
{
	if (cc) 
	{
		XWPSFont *pfont = current_font;

		pfont->dir->freeCachedChar(cc);
		cc = 0;
  }
  pgs->restore();
  width_status = sws_none;
  log2_scale.x = log2_scale.y = 0;
  return 0;
}

int XWPSShowEnum::setCache(const double *pw,PSTextCacheControl control)
{
	switch (control) 
	{
    case TEXT_SET_CHAR_WIDTH:
			return setCharWidth(pgs, pw[0], pw[1]);
			
    case TEXT_SET_CACHE_DEVICE: 
    	{
				int code = setCharWidth(pgs, pw[0], pw[1]);

			if (code < 0)
	    	return code;
			return setCacheDevice(pgs, pw[2], pw[3], pw[4], pw[5]);
    	}
    	
    case TEXT_SET_CACHE_DEVICE2: 
    	{
				int code;

				if (pgs->rootFont()->WMode) 
				{
	    		float vx = pw[8], vy = pw[9];
	    		XWPSFixedPoint pvxy, dvxy;
	    		if ((code = pvxy.transform2fixed(&pgs->ctm, -vx, -vy)) < 0 ||
							(code = dvxy.distanceTransform2fixed(&pgs->ctm, vx, vy)) < 0)
						return 0;
	    		if ((code = setCharWidth(pgs, pw[6], pw[7])) < 0)
						return code;
						
	    		pgs->translateToFixed(pvxy.x, pvxy.y);
	    		code = setCacheDevice(pgs, pw[2], pw[3], pw[4], pw[5]);
	    		if (code != 1)
						return code;
	    		cc->offset.x += dvxy.x;
	    		cc->offset.y += dvxy.y;
				} 
				else 
				{
	    		code = setCharWidth(pgs, pw[0], pw[1]);
	    		if (code < 0)
						return code;
	    		code = setCacheDevice(pgs, pw[2], pw[3], pw[4], pw[5]);
				}
				return code;
    	}
    default:
			return (int)(XWPSError::RangeCheck);
  }
}

int XWPSShowEnum::setCacheDevice(XWPSState * pgsA, 
	                   double llx, 
	                   double lly,
		                 double urx, 
		                 double ury)
{
	ulong glyph;
	if (pgsA->in_cachedevice)
		return 0;
		
	pgsA->in_cachedevice = CACHE_DEVICE_NOT_CACHING;
	glyph = CURRENT_GLYPH;
  if (glyph == ps_no_glyph)
		return 0;
		
	if (can_cache <= 0 || !pgsA->char_tm_valid)
		return 0;
		
	{
		const XWPSFont *pfont = pgsA->font;
		XWPSFontDir *dir = pfont->dir;
		XWPSDevice *devA = pgsA->currentDevice();
		int alpha_bits = devA->getAlphaBits(go_text);
		static const long max_cdim[3] =
		{
#define max_cd(n)\
	    (fixed_1 << (arch_sizeof_short * 8 - n)) - (fixed_1 >> n) * 3
	    max_cd(0), max_cd(1), max_cd(2)
#undef max_cd
		};
		ushort iwidth, iheight;
		XWPSFixedRect clip_box;
		int code;
		
		XWPSFixedPoint cll, clr, cul, cur, cdim;

		if ((code = cll.distanceTransform2fixed(&pgsA->ctm, llx, lly)) < 0 ||
	    	(code = clr.distanceTransform2fixed(&pgsA->ctm, llx, ury)) < 0 ||
	    	(code = cul.distanceTransform2fixed(&pgsA->ctm, urx, lly)) < 0 ||
	 			(code = cur.distanceTransform2fixed(&pgsA->ctm, urx, ury)) < 0)
	    return 0;
	    
	  {
	    long ctemp;

#define swap(a, b) ctemp = a, a = b, b = ctemp
#define make_min(a, b) if ( (a) > (b) ) swap(a, b)

	    make_min(cll.x, cur.x);
	    make_min(cll.y, cur.y);
	    make_min(clr.x, cul.x);
	    make_min(clr.y, cul.y);
#undef make_min
#undef swap
		}
		
		if (clr.x < cll.x)
	    cll.x = clr.x, cur.x = cul.x;
		if (clr.y < cll.y)
	    cll.y = clr.y, cur.y = cul.y;
	    
	  cdim.x = cur.x - cll.x;
		cdim.y = cur.y - cll.y;
		showSetScale();
		log2_scale.x = log2_suggested_scale.x;
		log2_scale.y = log2_suggested_scale.y;
		
		if (alpha_bits > 1) 
		{
	    int more_bits =  alpha_bits - (log2_scale.x + log2_scale.y);

	    if (more_bits > 0) 
	    {
				if (log2_scale.x <= log2_scale.y) 
				{
		    	log2_scale.x += (more_bits + 1) >> 1;
		    	log2_scale.y += more_bits >> 1;
				} 
				else 
				{
		    	log2_scale.x += more_bits >> 1;
		    	log2_scale.y += (more_bits + 1) >> 1;
				}
	    }
		} 
		else if (!OVERSAMPLE || pfont->PaintType != 0) 
		{
	    log2_scale.x = log2_scale.y = 0;
		}
		
		if (cdim.x > max_cdim[log2_scale.x] ||
	    cdim.y > max_cdim[log2_scale.y])
	    return 0;
	    
	  iwidth = ((ushort) fixed2int_var(cdim.x) + 2) << log2_scale.x;
		iheight = ((ushort) fixed2int_var(cdim.y) + 2) << log2_scale.y;
		
		if (dev_cache == 0) 
		{
	    code = showCacheSetup();
	    if (code < 0)
				return code;
		}
		
		cc = XWPSCachedChar::alloc(dir, dev_cache,
														(iwidth > MAX_TEMP_BITMAP_BITS / iheight &&
				 										log2_scale.x + log2_scale.y > alpha_bits ?
				 										dev_cache2 : NULL),
														iwidth, iheight, &log2_scale, alpha_bits);
		if (cc == 0)
			return 0;
			
		cc->offset.x = fixed_ceiling(-cll.x);
		cc->offset.y = fixed_ceiling(-cll.y);
		pgsA->save();
		pgsA->setDeviceOnly(dev_cache);
		cc->code = glyph;
		cc->wmode = pgsA->rootFont()->WMode;
		cc->wxy = wxy;
		pgsA->ctm_default_set = false;
		pgsA->translateToFixed(cc->offset.x << log2_scale.x, cc->offset.y << log2_scale.y);
		if ((log2_scale.x | log2_scale.y) != 0)
	    pgsA->scaleCharMatrix(1 << log2_scale.x, 1 << log2_scale.y);
	    
	  dev_cache->initial_matrix = pgsA->ctm;
	  clip_box.p.x = clip_box.p.y = 0;
		clip_box.q.x = int2fixed(iwidth);
		clip_box.q.y = int2fixed(iheight);
		if ((code = pgs->clipToRectangle(&clip_box)) < 0)
	    return code;
	  pgsA->setDeviceColor1();
	  pgsA->in_cachedevice = CACHE_DEVICE_CACHING;
	}
	width_status = sws_cache;
  return 1;
}

int XWPSShowEnum::setCharWidth(XWPSState *pgs, double wx, double wy)
{
	int code = 0;
	
	if (width_status != sws_none)
		return (int)(XWPSError::Undefined);

  if ((code = wxy.distanceTransform2fixed(&pgs->ctm, wx, wy)) < 0)
		return code;
		
	if (cc != 0) 
	{
		cc->wxy = wxy;
		width_status = sws_cache_width_only;
  } 
  else 
  {
		width_status = sws_no_cache;
  }
  return !showIsDrawing();
}

int  XWPSShowEnum::showCacheSetup()
{
	XWPSDeviceMem * devA = new XWPSDeviceMem;
	devA->context_state = pgs->i_ctx_p;
	devA->makeMemMonoDevice(pgs->currentDevice());
	XWPSDeviceMem * dev2A = new XWPSDeviceMem;
	dev2A->context_state = pgs->i_ctx_p;
	dev2A->makeMemMonoDevice(pgs->currentDevice());
	dev_cache = devA;
	dev_cache2 = dev2A;
	return 0;
}

XWPSFont * XWPSShowEnum::showCurrentFont()
{
	return (fstack.depth < 0 ? pgs->font : fstack.items[fstack.depth].font);
}

int XWPSShowEnum::showFastMove(XWPSState * pgs, XWPSFixedPoint * pwxy)
{
	int code = pgs->path->addRelPoint(pwxy->x, pwxy->y);

  if ((code == XWPSError::LimitCheck) && pgs->clamp_coordinates)
		code = 0;
  return code;
}

int  XWPSShowEnum::showFinish()
{
  if (!showIsStringWidth())
		return 0;
		
  int code = pgs->currentPoint(&returned.total_width);
  int rcode = pgs->restore();
  return (code < 0 ? code : rcode);
}

int  XWPSShowEnum::showMove()
{
	if (showIs(TEXT_REPLACE_WIDTHS))
	{
		XWPSPoint dpt;

		text.replacedWidth(xy_index - 1, &dpt);
		wxy.distanceTransform2fixed(&pgs->ctm, dpt.x, dpt.y);
	}
	else
	{
		double dx = 0, dy = 0;

		if (showIsAddToSpace()) 
		{
	    ulong chr = CURRENT_CHAR & 0xff;
	    int fdepth = fstack.depth;

	    if (fdepth > 0) 
	    {
				uint fidx = fstack.items[fdepth].index;

				switch (((XWPSFontType0 *) (fstack.items[fdepth - 1].font))->FMapType) 
				{
					case fmap_1_7:
					case fmap_9_7:
		    		chr += fidx << 7;
		    		break;
		    		
					case fmap_CMap:
		    		chr = CURRENT_CHAR;
		    		if (!cmap_code)
							break;
							
					default:
		    		chr += fidx << 8;
				}
	    }
	    if (chr == text.space.s_char) 
	    {
				dx = text.delta_space.x;
				dy = text.delta_space.y;
	    }
	  }
	  
	  if (showIsAddToAll()) 
	  {
	    dx += text.delta_all.x;
	    dy += text.delta_all.y;
		}
		
		if (!is_fzero2(dx, dy)) 
		{
	    XWPSFixedPoint dxy;

	    dxy.distanceTransform2fixed(&pgs->ctm, dx, dy);
	    wxy.x += dxy.x;
	    wxy.y += dxy.y;
		}
	}
	
	if (showIsAllOf(TEXT_DO_NONE | TEXT_INTERVENE)) 
	{
		proc.continue_proc_.continue_proc = &XWPSShowEnum::continueKShow;
		return TEXT_PROCESS_INTERVENE;
  }
  
  {
		int code = showFastMove(pgs, &wxy);

		if (code < 0)
	    return code;
  }
  
  if (showIsDoKern() && index < text.size) 
  {
		proc.continue_proc_.continue_proc = &XWPSShowEnum::continueKShow;
		return TEXT_PROCESS_INTERVENE;
  }
  return 0;
}

int XWPSShowEnum::showOriginSetup(XWPSState * pgs, 
	                    long cpt_x, 
	                    long cpt_y,
		                  PSCharPathMode charpath_flag)
{
	if (charpath_flag == cpm_show)
	{
		cpt_x = fixed_rounded(cpt_x);
		cpt_y = fixed_rounded(cpt_y);
	}
	
	return pgs->translateToFixed(cpt_x, cpt_y);
}

int  XWPSShowEnum::showProceed()
{
	XWPSFont *pfont;
  XWPSCachedFMPair *pair = 0;
  XWPSFont *rfont =	(fstack.depth < 0 ? pgs->font : fstack.items[0].font);
  int wmode = rfont->WMode;
      
  ulong chr;
  ulong glyph;
  int code;
  XWPSDevice *dev = pgs->currentDevice();
  int alpha_bits = dev->getAlphaBits(go_text);
  if (charpath_flag == cpm_show && SHOW_USES_OUTLINE) 
  {
		code = pgs->colorLoad();
		if (code < 0)
	    return code;
  }
  
more:
	pfont = (fstack.depth < 0 ? pgs->font : fstack.items[fstack.depth].font);
  current_font = pfont;
  if (can_cache >= 0)
  {
  	for (;;)
  	{
  		++xy_index;
  		switch ((code = rfont->nextCharGlyph(this, &chr, &glyph)))
  		{
  			default:
  				return code;
  				
  			case 2:
		    	return showFinish();
		    	
				case 1:
		    	pfont = fstack.items[fstack.depth].font;
		    	current_font = pfont;
		    	pgs->char_tm_valid = false;
		    	showStateSetup();
		    	pair = 0;
		    	
		    case 0:
		    	SET_CURRENT_CHAR(chr);
		    	if (glyph == ps_no_glyph) 
		    	{
		    		if (showIs(TEXT_FROM_GLYPHS | TEXT_FROM_SINGLE_GLYPH))
		    			glyph = pfont->encodeCharNo(chr, GLYPH_SPACE_NAME);
		    		else
		    			glyph = pfont->encodeChar(chr, GLYPH_SPACE_NAME);
						if (glyph == ps_no_glyph) 
						{
			    		cc = 0;
			    		goto no_cache;
						}
		    	}
		    	if (pair == 0)
						pair = pgs->lookupFMPair(pfont);
		    	cc = pfont->lookupCachedChar(pair, glyph, wmode, alpha_bits);
		    	if (cc == 0)
		    	{
		    		if (pfont->ExactSize == fbit_use_outlines || pfont->PaintType == 2)
			    		goto no_cache;
			    		
			    	if (pfont->BitmapWidths)
			    	{
			    		//cc = pgs->lookupXFontChar(pair, chr, glyph, &pfont->procs.callbacks, wmode);
			    		//if (cc == 0)
								goto no_cache;
			    	}
			    	else
			    	{
			    		if (!SHOW_USES_OUTLINE ||
									(charpath_flag != cpm_show &&
				 						charpath_flag != cpm_charwidth))
								goto no_cache;
								
							goto no_cache;
			    	}
		    	}
		    	if (charpath_flag == cpm_charwidth)
		    	{
		    		;
		    	}
		    	else if (charpath_flag != cpm_show)
		    	{
		    		XWPSPath box_path;
						XWPSFixedPoint pt;
						long llx, lly, urx, ury;

						code = pgs->path->currentPoint(&pt);
						if (code < 0)
			    		return code;
						llx = fixed_rounded(pt.x - cc->offset.x) + int2fixed(ftx);
						lly = fixed_rounded(pt.y - cc->offset.y) + int2fixed(fty);
						urx = llx + int2fixed(cc->width), ury = lly + int2fixed(cc->height);
						code = box_path.addRectangle(llx, lly, urx, ury);
						if (code >= 0)
			    		code = pgs->show_gstate->path->addCharPath(&box_path, charpath_flag);
			    	if (code >= 0)
			    		code = pgs->path->addPoint(pt.x, pt.y);
			    		
			    	if (code < 0)
			    		return code;
		    	}
		    	else if (showIsDrawing())
		    	{
		    		code = imageCachedChar(cc);
						if (code < 0)
			    		return code;
						else if (code > 0) 
						{
			    		cc = 0;
			    		goto no_cache;
						}
		    	}
		    	
		    	if (showIsSlow())
		    	{
		    		wxy.x = cc->wxy.x;
						wxy.y = cc->wxy.y;
						code = showMove();
		    	}
		    	else
		    		code = showFastMove(pgs, &cc->wxy);
		    		
		    	if (code)
		    	{
		    		SET_CURRENT_GLYPH(glyph);
						return code;
		    	}
  		}
  	}
  }
  else
  {
  	++xy_index;
  	switch ((code = rfont->nextCharGlyph(this, &chr, &glyph)))
  	{
  		default:
				return code;
				
	    case 2:
				return showFinish();
				
	    case 1:
				pfont = fstack.items[fstack.depth].font;
				current_font = pfont;
				showStateSetup();
	    case 0:
				;
  	}
  	
  	SET_CURRENT_CHAR(chr);
		if (glyph == ps_no_glyph) 
		{
			if (showIs(TEXT_FROM_GLYPHS | TEXT_FROM_SINGLE_GLYPH))
		    glyph = pfont->encodeCharNo(chr, GLYPH_SPACE_NAME);
		  else
		  	glyph = pfont->encodeChar(chr, GLYPH_SPACE_NAME);
		}
		
		cc = 0;
  }
  
no_cache:
	SET_CURRENT_GLYPH(glyph);
  if ((code = pgs->save()) < 0)
		return code;
		
	pgs->font = pfont;
	pgs->in_cachedevice = CACHE_DEVICE_NONE;
	log2_scale.x = log2_scale.y = 0;
	pgs->in_charpath = charpath_flag;
  pgs->show_gstate = (show_gstate == pgs ? pgs->saved : show_gstate);
  pgs->stroke_adjust = false;
  
  {
  	XWPSFixedPoint cpt;
		XWPSPath *ppath = pgs->path;
		if ((code = ppath->currentPoint(&cpt)) < 0)
		{
			if (!showIs(TEXT_DO_NONE))
				goto rret;
	    cpt.x = cpt.y = 0;
		}
		
		origin.x = cpt.x;
		origin.y = cpt.y;
		
		pgs->currentCharMatrix(NULL, true);
#if 1				/*USE_FPU <= 0 */
		if (pgs->ctm.txy_fixed_valid && pgs->char_tm.txy_fixed_valid) 
		{
	    long tx = pgs->ctm.tx_fixed;
	    long ty = pgs->ctm.ty_fixed;

	    pgs->setToCharMatrix();
	    cpt.x += pgs->ctm.tx_fixed - tx;
	    cpt.y += pgs->ctm.ty_fixed - ty;
		} 
		else
#endif
		{
	    double tx = pgs->ctm.tx;
	    double ty = pgs->ctm.ty;
	    double fpx, fpy;

	    pgs->setToCharMatrix();
	    fpx = fixed2float(cpt.x) + (pgs->ctm.tx - tx);
	    fpy = fixed2float(cpt.y) + (pgs->ctm.ty - ty);
#define f_fits_in_fixed(f) f_fits_in_bits(f, fixed_int_bits)
	    if (!(f_fits_in_fixed(fpx) && f_fits_in_fixed(fpy))) 
	    {
				code = XWPSError::LimitCheck;
				goto rret;
	    }
	    cpt.x = float2fixed(fpx);
	    cpt.y = float2fixed(fpy);
  	}
  	
  	pgs->newPath();
		code = showOriginSetup(pgs, cpt.x, cpt.y, charpath_flag);
		if (code < 0)
	    goto rret;
  }
  
  width_status = sws_none;
  proc.continue_proc_.continue_proc = &XWPSShowEnum::continueShowUpdate;
  code = pfont->buildChar(this, pgs, chr, glyph);
  if (code < 0) 
		goto rret;
		
	if (code == 0) 
	{
		code = showUpdate();
		if (code < 0)
	    goto rret;
	    
		code = showMove();
		if (code)
	    return code;
		goto more;
  }
  
  if (dev_cache == 0) 
  {
		code = showCacheSetup();
		if (code < 0)
	    goto rret;
  }
  
  return TEXT_PROCESS_RENDER;
  
rret:
	pgs->restore();
  return code;
}

void XWPSShowEnum::showSetScale()
{
	if ((charpath_flag == cpm_show || charpath_flag == cpm_charwidth) &&
			SHOW_USES_OUTLINE &&
			pgs->getPath()->isVoid() &&
			(is_fzero2(pgs->char_tm.xy, pgs->char_tm.yx) ||
	 		is_fzero2(pgs->char_tm.xx, pgs->char_tm.yy)))
	{
		XWPSFontBase *pfont = (XWPSFontBase*)(pgs->font);
		XWPSFixedPoint extent;
		int code = extent.distanceTransform2fixed(&pgs->char_tm,
				  				pfont->FontBBox.q.x - pfont->FontBBox.p.x,
				  				pfont->FontBBox.q.y - pfont->FontBBox.p.y);
		if (code >= 0)
		{
			int sx = (extent.x == 0 ? 0 :  qAbs(extent.x) < int2fixed(25) ? 2 :
	     											qAbs(extent.x) < int2fixed(60) ? 1 :  0);
	    int sy = (extent.y == 0 ? 0 :  qAbs(extent.y) < int2fixed(25) ? 2 :
	     											qAbs(extent.y) < int2fixed(60) ? 1 :  0);

	    if (sx == 0 && sy != 0)
				sx = 1;
	    else if (sy == 0 && sx != 0)
				sy = 1;
	    log2_suggested_scale.x = sx;
	    log2_suggested_scale.y = sy;
	    return;
		}
	}
	
	log2_suggested_scale.x =log2_suggested_scale.y = 0;
}

int  XWPSShowEnum::showStateSetup()
{
	XWPSClipPath *pcpathA;
  XWPSFont *pfont;
  if (fstack.depth <= 0)
  {
  	pfont = pgs->font;
		pgs->currentCharMatrix(NULL, 1);
  }
  else
  {
  	XWPSMatrix mat;
		const XWPSFontStackItem *pfsi =  &fstack.items[fstack.depth];
		pfont = pfsi->font;
		mat.matrixMultiply(&pfont->FontMatrix, &pfsi[-1].font->FontMatrix);
		pgs->setCharMatrix(&mat);
  }
  
  current_font = pfont;
  if (!CACHE_ROTATED_CHARS &&
			(is_fzero2(pgs->char_tm.xy, pgs->char_tm.yx) ||
			is_fzero2(pgs->char_tm.xx, pgs->char_tm.yy)))
		can_cache = 0;
	if (can_cache >= 0 &&	pgs->effectiveClipPath(&pcpathA) >= 0)
	{
		XWPSFixedRect cbox;
		pcpathA->innerBox(&cbox);
		
		ibox.p.x = fixed2int_var(cbox.p.x);
		ibox.p.y = fixed2int_var(cbox.p.y);
		ibox.q.x = fixed2int_var_ceiling(cbox.q.x);
		ibox.q.y = fixed2int_var_ceiling(cbox.q.y);
		pcpathA->outerBox(&cbox);
		obox.p.x = fixed2int_var(cbox.p.x);
		obox.p.y = fixed2int_var(cbox.p.y);
		obox.q.x = fixed2int_var_ceiling(cbox.q.x);
		obox.q.y = fixed2int_var_ceiling(cbox.q.y);
#if 1				/*USE_FPU <= 0 */
		if (pgs->ctm.txy_fixed_valid && pgs->char_tm.txy_fixed_valid) 
		{
	  	ftx = (int)fixed2long(pgs->char_tm.tx_fixed - pgs->ctm.tx_fixed);
	  	fty = (int)fixed2long(pgs->char_tm.ty_fixed - pgs->ctm.ty_fixed);
		} 
		else 
		{
#endif
	    double fdx = pgs->char_tm.tx - pgs->ctm.tx;
	    double fdy = pgs->char_tm.ty - pgs->ctm.ty;

#define int_bits (arch_sizeof_int * 8 - 1)
	    if (!(f_fits_in_bits(fdx, int_bits) &&  f_fits_in_bits(fdy, int_bits)))
				return (int)(XWPSError::LimitCheck);
#undef int_bits
	    ftx = (int)fdx;
	    fty = (int)fdy;
		}
	}
	
  return 0;
}

int  XWPSShowEnum::showUpdate()
{
	int code;
	switch (width_status)
	{
		case sws_none:
			wxy.x = wxy.y = 0;
	    break;
	    
	   case sws_cache:
	   	switch (pgs->level - level)
	   	{
	   		default:
		    	return (int)(XWPSError::InvalidFont);
		    		
		    case 2:
		    	code = pgs->restore();
		    	if (code < 0)
						return code;
				case 1:
		    	;
	   	}	   	
	   	pgs->font->dir->addCachedChar(dev_cache, cc, pgs->font->lookupFMPair(pgs), &log2_scale);
	   	if (pgs->i_ctx_p)
	   		cc->id = pgs->i_ctx_p->nextIDS(1);
	    if (!SHOW_USES_OUTLINE ||	charpath_flag != cpm_show)
				break;
				
		case sws_cache_width_only:
			code = pgs->restore();
	    if (code < 0)
				return code;
	    code = pgs->colorLoad();
	    if (code < 0)
				return code;
	    return imageCachedChar(cc);
	    
		case sws_no_cache:
	    ;
	}
	
	if (charpath_flag != cpm_show)
	{
		code = pgs->show_gstate->path->addPoint(origin.x, origin.y);
		if (code < 0)
	    return code;
	}
	
	return pgs->restore();
}

XWPSBboxTextEnum::XWPSBboxTextEnum()
	:XWPSTextEnum()
{
	target_info = 0;
}

XWPSBboxTextEnum::~XWPSBboxTextEnum()
{
}

int XWPSBboxTextEnum::currentWidth(XWPSPoint *pwidth)
{
	return target_info->currentWidth(pwidth);
}

int XWPSBboxTextEnum::getLength()
{
	return sizeof(XWPSBboxTextEnum);
}

const char * XWPSBboxTextEnum::getTypeName()
{
	return "bboxtextenum";
}

bool XWPSBboxTextEnum::isWidthOnly()
{
	return target_info->isWidthOnly();
}

int XWPSBboxTextEnum::process()
{
	int code = target_info->process();

  copyDynamic(target_info, true);
  return code;
}

int XWPSBboxTextEnum::resync(XWPSTextEnum *pfrom)
{
	int code = target_info->resync(pfrom);

  if (code >= 0)
		copy(target_info);
  return code;
}

int  XWPSBboxTextEnum::retry()
{
	return target_info->retry();
}

int XWPSBboxTextEnum::setCache(const double *values,PSTextCacheControl control)
{
	int code = target_info->setCache(values, control);

  if (code < 0)
		return code;
  index = target_info->index;
  return code;
}


XWPSPDFTextProcessState::XWPSPDFTextProcessState()
{
	chars = 0;
	words = 0;
	size = 0;
	pdfont = 0;
}

XWPSPDFTextEnum::XWPSPDFTextEnum()
	:XWPSTextEnum()
{
	pte_default = 0;
}

XWPSPDFTextEnum::~XWPSPDFTextEnum()
{
	if (pte_default)
	{
		if (!pte_default->decRef())
			delete pte_default;
		pte_default = 0;
	}
}

int  XWPSPDFTextEnum::currentWidth(XWPSPoint *pwidth)
{
	if (pte_default)
		return pte_default->currentWidth(pwidth);
  return (int)(XWPSError::RangeCheck);
}

int XWPSPDFTextEnum::getLength()
{
	return sizeof(XWPSPDFTextEnum);
}

const char * XWPSPDFTextEnum::getTypeName()
{
	return "pdftextenum";
}

bool XWPSPDFTextEnum::isWidthOnly()
{
	if (pte_default)
		return pte_default->isWidthOnly();
  return false;
}

int XWPSPDFTextEnum::process()
{
	XWPSDevicePDF * pdev = (XWPSDevicePDF *)dev;
	XWPSTextParams *textA = &text;
  XWPSFont *font;
  XWPSMatrix fmat;
  XWPSTextParams alt_text;
  XWPSPDFTextProcessState text_state;
  XWPSString str;
  int i;
  uchar strbuf[200];
  int code;
  XWPSMatrix ctm;

top:
  if (pte_default)
  {
  	code = pte_default->process();
		copyDynamic(pte_default, true);
		if (code)
	    return code;
	    
	   delete pte_default;
	   pte_default = 0;
		return 0;
  }
  
  str.data = (uchar*)textA->data.bytes;
  str.size = textA->size;
  font = current_font;
  fmat = font->FontMatrix;

fnt:
  switch (font->FontType)
  {
  	case ft_TrueType:
    case ft_encrypted:
    case ft_encrypted2:
			break;
    case ft_composite: 
    	{
				XWPSFontType0 * font0 = (XWPSFontType0 *)font;
				switch (font0->FMapType) 
				{
					case fmap_8_8:
	    			if (str.size > sizeof(strbuf) * 2)
							goto dflt;
	    			for (i = 0; i < str.size; i += 2) 
	    			{
							if (str.data[i] != 0)
		    				goto dflt;
							strbuf[i >> 1] = str.data[i + 1];
	    			}
	    			str.data = strbuf;
	    			str.size >>= 1;
	    			font = current_font = font0->FDepVector[0];
	    			
	    			if (font->FontType != ft_composite)
							fmat.matrixMultiply(&fmat, &font->FontMatrix);
	    			goto fnt;
	    			
					default:
	    			goto dflt;
				}
				break;
    	}
    	
    default:
			goto dflt;
  }
  
  code = updateTextState(&text_state, &fmat);
  if (code < 0)
		goto dflt;
		
	for (i = 0; i < str.size; ++i) 
	{
		int chr = str.data[i];
		int code = pdev->encodeChar(chr, (XWPSFontBase *)font, text_state.pdfont);

		if (code < 0)
	    goto dflt;
		if (code != chr) 
		{
	    if (str.data != strbuf) 
	    {
				if (str.size > sizeof(strbuf))
		    	goto dflt;
				memcpy(strbuf, str.data, str.size);
				str.data = strbuf;
	    }
	    strbuf[i] = (uchar)code;
		}
  }
  
  if (index < str.size) 
  {
		code = pdev->writeTextProcessState(&text_state, &str);
		if (code < 0)
	    goto dflt;
  }

	pis->ctmOnly(&ctm);
  if (textA->operation & TEXT_REPLACE_WIDTHS) 
  {
		XWPSPoint w;
		XWPSMatrix tmat;

		tmat = text_state.text_matrix;
		for (; index < str.size; index++, xy_index++) 
		{
	    XWPSPoint d, dpt;

	    code = pdev->appendChars(str.data + index, 1);
	    if (code < 0)
				return code;
	    textA->replacedWidth(xy_index, &d);
	    w.x += d.x, w.y += d.y;
	    dpt.distanceTransform(d.x, d.y, &ctm);
	    tmat.tx += dpt.x;
	    tmat.ty += dpt.y;
	    if (index + 1 < str.size) 
	    {
				code = pdev->setTextMatrix(&tmat);
				if (code < 0)
		    	return code;
	    }
		}
		returned.total_width = w;
		return  (textA->operation & TEXT_RETURN_WIDTH ?  path->addPoint(float2fixed(tmat.tx), float2fixed(tmat.ty)) : 0);
  }

  code = pdev->appendChars(str.data + index, str.size - index);
  index = 0;
  if (code < 0)
		goto dflt;
		
  if (!(textA->operation & TEXT_RETURN_WIDTH))
		return 0;
  {
		int i, w;
		double scale = (font->FontType == ft_TrueType ? 0.001 : 1.0);
		XWPSPoint dpt;
		int num_spaces = 0;

		for (i = index, w = 0; i < str.size; ++i) 
		{
	    int cw;
	    int code =
			text_state.pdfont->charWidth(str.data[i], font, &cw);

	    if (code < 0)
				goto dflt_w;
	    w += cw;
	    if (str.data[i] == ' ')
				++num_spaces;
		}
		dpt.distanceTransform(w * scale, 0.0, &font->FontMatrix);
		if (textA->operation & TEXT_ADD_TO_ALL_WIDTHS) 
		{
	    int num_chars = str.size - index;

	    dpt.x += textA->delta_all.x * num_chars;
	    dpt.y += textA->delta_all.y * num_chars;
		}
		if (text.operation & TEXT_ADD_TO_SPACE_WIDTH) 
		{
	    dpt.x += textA->delta_space.x * num_spaces;
	    dpt.y += textA->delta_space.y * num_spaces;
		}
		returned.total_width = dpt;
		dpt.distanceTransform(dpt.x, dpt.y, &ctm);
		return path->addPoint(origin.x + float2fixed(dpt.x), origin.y + float2fixed(dpt.y));
  }

dflt_w:
  alt_text = *textA;
  alt_text.operation ^= TEXT_DO_DRAW | TEXT_DO_CHARWIDTH;
  textA = &alt_text;
  
dflt:
  code = dev->textBeginDefault(pis, textA, current_font, path, pdcolor, pcpath, &pte_default);
  if (code < 0)
		return code;
		
  pte_default->copyDynamic(this, false);
  goto top;
}

int XWPSPDFTextEnum::resync(XWPSTextEnum *pfrom)
{
	if ((text.operation ^ pfrom->text.operation) & ~TEXT_FROM_ANY)
		return (int)(XWPSError::RangeCheck);
  if (pte_default) 
  {
		int code = pte_default->resync(pfrom);

		if (code < 0)
	    return code;
  }
  if (text.p_bytes != pfrom->text.p_bytes)
  {
  	if (text.p_bytes)
  		if (text.p_bytes->decRef() == 0)
  			delete text.p_bytes;
  			
  	if (pfrom->text.p_bytes)
  		pfrom->text.p_bytes->incRef();
  }
  text = pfrom->text;
  copyDynamic(pfrom, false);
  return 0;
}

int  XWPSPDFTextEnum::retry()
{
	if (pte_default)
		return pte_default->retry();
  return (int)(XWPSError::RangeCheck);
}

int XWPSPDFTextEnum::setCache(const double *pw,PSTextCacheControl control)
{
	if (pte_default)
		return pte_default->setCache(pw, control);
  return (int)(XWPSError::RangeCheck);
}

int XWPSPDFTextEnum::updateTextState(XWPSPDFTextProcessState *ppts, XWPSMatrix *pfmat)
{
	XWPSDevicePDF *pdev = (XWPSDevicePDF *)dev;
  XWPSFont *font = current_font;
  XWPSPDFFont *ppf;
  XWPSFixedPoint cpt;
  XWPSMatrix orig_matrix, smat, tmat;
  double sx = pdev->HWResolution[0] / 72.0, sy = pdev->HWResolution[1] / 72.0;
  float chars, words, size;
  int code = path->currentPoint(&cpt);

  if (code < 0)
		return code;
		
	switch (font->FontType)
	{
		case ft_TrueType:
    case ft_CID_TrueType:
    	if (((XWPSFontType42 *)font)->glyf == 0)
	    	return (int)(XWPSError::RangeCheck);
			break;
			
    case ft_encrypted:
    case ft_encrypted2:
    case ft_CID_encrypted:
			orig_matrix.makeScaling(0.001, 0.001);
			break;
			
    default:
			return (int)(XWPSError::RangeCheck);
	}
	
	pdev->findOrigFont(font, &orig_matrix);
	smat.matrixInvert(&orig_matrix);
  smat.matrixMultiply(&smat, pfmat);
  XWPSMatrix ctm;
  pis->ctmOnly(&tmat);
  tmat.tx = tmat.ty = 0;
  tmat.matrixMultiply(&smat, &tmat);
  
  size = fabs(tmat.yy) / sy;
  if (size < 0.01)
		size = fabs(tmat.xx) / sx;
  if (size < 0.01)
		size = 1;
		
	if (text.operation & TEXT_ADD_TO_ALL_WIDTHS)
	{
		XWPSPoint pt;

		pt.distanceTransformInverse(text.delta_all.x, text.delta_all.y, &smat);
		if (pt.y != 0)
	    return (int)(XWPSError::RangeCheck);
		chars = pt.x * size;
	}
	else
		chars = 0.0;

  if (text.operation & TEXT_ADD_TO_SPACE_WIDTH)
  {
  	XWPSPoint pt;

		pt.distanceTransformInverse(text.delta_space.x, text.delta_space.y, &smat);
		if (pt.y != 0 || text.space.s_char != 32)
	    return (int)(XWPSError::RangeCheck);
		words = pt.x * size;
  }
  else
		words = 0.0;
		
	ppf = (XWPSPDFFont *)pdev->findResourceByPsId(resourceFont, font->id);
	if (ppf && ppf->FontDescriptor->written)
		ppf = 0;
  if (ppf == 0 || ppf->font == 0) 
  {
		code = pdev->createPDFFont(font, &orig_matrix, &ppf);
		if (code < 0)
	    return code;
  }
  
  tmat.xx /= size;
  tmat.xy /= size;
  tmat.yx /= size;
  tmat.yy /= size;
  tmat.tx += fixed2float(cpt.x);
  tmat.ty += fixed2float(cpt.y);

  ppts->chars = chars;
  ppts->words = words;
  ppts->size = size;
  ppts->text_matrix = tmat;
  ppts->pdfont = ppf;

  return 0;
}

XWPSDVITextProcessState::XWPSDVITextProcessState()
{
	chars = 0;
	words = 0;
	size = 0;
	pdfont = 0;
}

XWPSDVITextEnum::XWPSDVITextEnum()
{
	pte_default = 0;
}

XWPSDVITextEnum::~XWPSDVITextEnum()
{
	if (pte_default)
	{
		if (!pte_default->decRef())
			delete pte_default;
		pte_default = 0;
	}
}

int  XWPSDVITextEnum::currentWidth(XWPSPoint *pwidth)
{
	if (pte_default)
		return pte_default->currentWidth(pwidth);
  return (int)(XWPSError::RangeCheck);
}

int XWPSDVITextEnum::getLength()
{
	return sizeof(XWPSDVITextEnum);
}

const char * XWPSDVITextEnum::getTypeName()
{
	return "dvitextenum";
}

bool XWPSDVITextEnum::isWidthOnly()
{
	if (pte_default)
		return pte_default->isWidthOnly();
  return false;
}

int XWPSDVITextEnum::process()
{
	XWPSDeviceDVI *pdev = (XWPSDeviceDVI *)dev;
	XWPSTextParams * textA = &text;
  XWPSFont *font;
  XWPSMatrix fmat;
  XWPSTextParams alt_text;
  XWPSDVITextProcessState text_state;
  XWPSString str;
  int i;
  uchar strbuf[200];
  int code;
  XWPSMatrix ctm;

top:
  if (pte_default)
  {
  	code = pte_default->process();
		copyDynamic(pte_default, true);
		if (code)
	    return code;
	    
	   delete pte_default;
	   pte_default = 0;
		return 0;
  }
  
  str.data = (uchar*)textA->data.bytes;
  str.size = textA->size;
  font = current_font;
  fmat = font->FontMatrix;

fnt:
  switch (font->FontType)
  {
  	case ft_TrueType:
    case ft_encrypted:
    case ft_encrypted2:
			break;
    case ft_composite: 
    	{
				XWPSFontType0 * font0 = (XWPSFontType0 *)font;
				switch (font0->FMapType) 
				{
					case fmap_8_8:
	    			if (str.size > sizeof(strbuf) * 2)
							goto dflt;
	    			for (i = 0; i < str.size; i += 2) 
	    			{
							if (str.data[i] != 0)
		    				goto dflt;
							strbuf[i >> 1] = str.data[i + 1];
	    			}
	    			str.data = strbuf;
	    			str.size >>= 1;
	    			font = current_font = font0->FDepVector[0];
	    			
	    			if (font->FontType != ft_composite)
							fmat.matrixMultiply(&fmat, &font->FontMatrix);
	    			goto fnt;
	    			
					default:
	    			goto dflt;
				}
				break;
    	}
    	
    default:
			goto dflt;
  }
  
  code = updateTextState(&text_state, &fmat);
  if (code < 0)
		goto dflt;
		
	for (i = 0; i < str.size; ++i) 
	{
		int chr = str.data[i];
		int code = pdev->encodeChar(chr, (XWPSFontBase *)font, text_state.pdfont);

		if (code < 0)
	    goto dflt;
		if (code != chr) 
		{
	    if (str.data != strbuf) 
	    {
				if (str.size > sizeof(strbuf))
		    	goto dflt;
				memcpy(strbuf, str.data, str.size);
				str.data = strbuf;
	    }
	    strbuf[i] = (uchar)code;
		}
  }
  
  if (index < str.size) 
  {
		code = pdev->writeTextProcessState(&text_state, &str);
		if (code < 0)
	    goto dflt;
  }

	pis->ctmOnly(&ctm);
  if (textA->operation & TEXT_REPLACE_WIDTHS) 
  {
		XWPSPoint w;
		XWPSMatrix tmat;

		tmat = text_state.text_matrix;
		for (; index < str.size; index++, xy_index++) 
		{
	    XWPSPoint d, dpt;

	    code = pdev->appendChars(str.data + index, 1);
	    if (code < 0)
				return code;
	    textA->replacedWidth(xy_index, &d);
	    w.x += d.x, w.y += d.y;
	    dpt.distanceTransform(d.x, d.y, &ctm);
	    tmat.tx += dpt.x;
	    tmat.ty += dpt.y;
	    if (index + 1 < str.size) 
	    {
				code = pdev->setTextMatrix(&tmat);
				if (code < 0)
		    	return code;
	    }
		}
		returned.total_width = w;
		return  (textA->operation & TEXT_RETURN_WIDTH ?  path->addPoint(float2fixed(tmat.tx), float2fixed(tmat.ty)) : 0);
  }

  code = pdev->appendChars(str.data + index, str.size - index);
  index = 0;
  if (code < 0)
		goto dflt;
		
  if (!(textA->operation & TEXT_RETURN_WIDTH))
		return 0;
  {
		int i, w;
		double scale = (font->FontType == ft_TrueType ? 0.001 : 1.0);
		XWPSPoint dpt;
		int num_spaces = 0;

		for (i = index, w = 0; i < str.size; ++i) 
		{
	    int cw;
	    int code =
			text_state.pdfont->charWidth(str.data[i], font, &cw);

	    if (code < 0)
				goto dflt_w;
	    w += cw;
	    if (str.data[i] == ' ')
				++num_spaces;
		}
		dpt.distanceTransform(w * scale, 0.0, &font->FontMatrix);
		if (textA->operation & TEXT_ADD_TO_ALL_WIDTHS) 
		{
	    int num_chars = str.size - index;

	    dpt.x += textA->delta_all.x * num_chars;
	    dpt.y += textA->delta_all.y * num_chars;
		}
		if (text.operation & TEXT_ADD_TO_SPACE_WIDTH) 
		{
	    dpt.x += textA->delta_space.x * num_spaces;
	    dpt.y += textA->delta_space.y * num_spaces;
		}
		returned.total_width = dpt;
		dpt.distanceTransform(dpt.x, dpt.y, &ctm);
		return path->addPoint(origin.x + float2fixed(dpt.x), origin.y + float2fixed(dpt.y));
  }

dflt_w:
  alt_text = *textA;
  alt_text.operation ^= TEXT_DO_DRAW | TEXT_DO_CHARWIDTH;
  textA = &alt_text;
  
dflt:
  code = dev->textBeginDefault(pis, textA, current_font, path, pdcolor, pcpath, &pte_default);
  if (code < 0)
		return code;
		
  pte_default->copyDynamic(this, false);
  goto top;
}

int XWPSDVITextEnum::resync(XWPSTextEnum *pfrom)
{
	if ((text.operation ^ pfrom->text.operation) & ~TEXT_FROM_ANY)
		return (int)(XWPSError::RangeCheck);
  if (pte_default) 
  {
		int code = pte_default->resync(pfrom);

		if (code < 0)
	    return code;
  }
  if (text.p_bytes != pfrom->text.p_bytes)
  {
  	if (text.p_bytes)
  		if (text.p_bytes->decRef() == 0)
  			delete text.p_bytes;
  			
  	if (pfrom->text.p_bytes)
  		pfrom->text.p_bytes->incRef();
  }
  text = pfrom->text;
  copyDynamic(pfrom, false);
  return 0;
}

int  XWPSDVITextEnum::retry()
{
	if (pte_default)
		return pte_default->retry();
  return (int)(XWPSError::RangeCheck);
}

int XWPSDVITextEnum::setCache(const double *pw,PSTextCacheControl control)
{
	if (pte_default)
		return pte_default->setCache(pw, control);
  return (int)(XWPSError::RangeCheck);
}

int XWPSDVITextEnum::updateTextState(XWPSDVITextProcessState *ppts, XWPSMatrix *pfmat)
{
	XWPSDeviceDVI * pdev = (XWPSDeviceDVI *)dev;
  XWPSFont *font = current_font;
  XWPSDVIFont *ppf;
  XWPSFixedPoint cpt;
  XWPSMatrix orig_matrix, smat, tmat;
  double sx = pdev->HWResolution[0] / 72.0, sy = pdev->HWResolution[1] / 72.0;
  float chars, words, size;
  int code = path->currentPoint(&cpt);

  if (code < 0)
		return code;
		
	switch (font->FontType)
	{
		case ft_TrueType:
    case ft_CID_TrueType:
    	if (((XWPSFontType42 *)font)->glyf == 0)
	    	return (int)(XWPSError::RangeCheck);
			break;
			
    case ft_encrypted:
    case ft_encrypted2:
    case ft_CID_encrypted:
			orig_matrix.makeScaling(0.001, 0.001);
			break;
			
    default:
			return (int)(XWPSError::RangeCheck);
	}
	
	pdev->findOrigFont(font, &orig_matrix);
	smat.matrixInvert(&orig_matrix);
  smat.matrixMultiply(&smat, pfmat);
  XWPSMatrix ctm;
  pis->ctmOnly(&tmat);
  tmat.tx = tmat.ty = 0;
  tmat.matrixMultiply(&smat, &tmat);
  
  size = fabs(tmat.yy) / sy;
  if (size < 0.01)
		size = fabs(tmat.xx) / sx;
  if (size < 0.01)
		size = 1;
		
	if (text.operation & TEXT_ADD_TO_ALL_WIDTHS)
	{
		XWPSPoint pt;

		pt.distanceTransformInverse(text.delta_all.x, text.delta_all.y, &smat);
		if (pt.y != 0)
	    return (int)(XWPSError::RangeCheck);
		chars = pt.x * size;
	}
	else
		chars = 0.0;

  if (text.operation & TEXT_ADD_TO_SPACE_WIDTH)
  {
  	XWPSPoint pt;

		pt.distanceTransformInverse(text.delta_space.x, text.delta_space.y, &smat);
		if (pt.y != 0 || text.space.s_char != 32)
	    return (int)(XWPSError::RangeCheck);
		words = pt.x * size;
  }
  else
		words = 0.0;
		
	ppf = (XWPSDVIFont *)pdev->findResourceByPsId(resourceFont, font->id);
	if (ppf && ppf->FontDescriptor->written)
		ppf = 0;
  if (ppf == 0 || ppf->font == 0) 
  {
		code = pdev->createPDFFont(font, &orig_matrix, &ppf);
		if (code < 0)
	    return code;
  }
  
  tmat.xx /= size;
  tmat.xy /= size;
  tmat.yx /= size;
  tmat.yy /= size;
  tmat.tx += fixed2float(cpt.x);
  tmat.ty += fixed2float(cpt.y);

  ppts->chars = chars;
  ppts->words = words;
  ppts->size = size;
  ppts->text_matrix = tmat;
  ppts->pdfont = ppf;

  return 0;
}
