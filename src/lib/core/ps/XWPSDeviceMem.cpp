/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <math.h>
#include "XWPSError.h"
#include "psbittable.h"
#include "XWPSFont.h"
#include "XWPSColor.h"
#include "XWPSDeviceMem.h"

#ifndef ALPHA_LSB_FIRST
#  define ALPHA_LSB_FIRST 0
#endif

#if !arch_is_big_endian
const quint16 mono_copy_masks[17] = {
    0xffff, 0xff7f, 0xff3f, 0xff1f,
    0xff0f, 0xff07, 0xff03, 0xff01,
    0xff00, 0x7f00, 0x3f00, 0x1f00,
    0x0f00, 0x0700, 0x0300, 0x0100,
    0x0000
};
const quint32 mono_fill_masks[33] = {
#define mask(n)\
  ((~0xff | (0xff >> (n & 7))) << (n & -8))
    mask( 0),mask( 1),mask( 2),mask( 3),mask( 4),mask( 5),mask( 6),mask( 7),
    mask( 8),mask( 9),mask(10),mask(11),mask(12),mask(13),mask(14),mask(15),
    mask(16),mask(17),mask(18),mask(19),mask(20),mask(21),mask(22),mask(23),
    mask(24),mask(25),mask(26),mask(27),mask(28),mask(29),mask(30),mask(31),
    0
#undef mask
};
#endif

#define CFETCH_ALIGNED(cptr)\
  (*(const chunk *)(cptr))
  
#undef chunk
#if arch_is_big_endian
#  define chunk uint
#  define CFETCH_RIGHT(cptr, shift, cshift)\
	(CFETCH_ALIGNED(cptr) >> shift)
#  define CFETCH_LEFT(cptr, shift, cshift)\
	(CFETCH_ALIGNED(cptr) << shift)
#  define CFETCH_USES_CSKEW 0
/* Fetch a chunk that straddles a chunk boundary. */
#  define CFETCH2(cptr, cskew, skew)\
    (CFETCH_LEFT(cptr, cskew, skew) +\
     CFETCH_RIGHT((const chunk *)(cptr) + 1, skew, cskew))
#else /* little-endian */
#  define chunk quint16

static const quint16 right_masks2[9] =
{
    0xffff, 0x7f7f, 0x3f3f, 0x1f1f, 0x0f0f, 0x0707, 0x0303, 0x0101, 0x0000
};
static const quint16 left_masks2[9] =
{
    0xffff, 0xfefe, 0xfcfc, 0xf8f8, 0xf0f0, 0xe0e0, 0xc0c0, 0x8080, 0x0000
};

#define fpat(byt) mono_fill_make_pattern(byt)

static const uint tile_patterns2[4] = {
    fpat(0x00), fpat(0x55), fpat(0xaa), fpat(0xff)
};

static const uint tile_patterns4[16] =
{fpat(0x00), fpat(0x11), fpat(0x22), fpat(0x33),
 fpat(0x44), fpat(0x55), fpat(0x66), fpat(0x77),
 fpat(0x88), fpat(0x99), fpat(0xaa), fpat(0xbb),
 fpat(0xcc), fpat(0xdd), fpat(0xee), fpat(0xff)
};

#  define CCONT(cptr, off) (((const chunk *)(cptr))[off])
#  define CFETCH_RIGHT(cptr, shift, cshift)\
	((shift) < 8 ?\
	 ((CCONT(cptr, 0) >> (shift)) & right_masks2[shift]) +\
	  (CCONT(cptr, 0) << (cshift)) :\
	 ((chunk)*(const uchar *)(cptr) << (cshift)) & 0xff00)
#  define CFETCH_LEFT(cptr, shift, cshift)\
	((shift) < 8 ?\
	 ((CCONT(cptr, 0) << (shift)) & left_masks2[shift]) +\
	  (CCONT(cptr, 0) >> (cshift)) :\
	 ((CCONT(cptr, 0) & 0xff00) >> (cshift)) & 0xff)
#  define CFETCH_USES_CSKEW 1
/* Fetch a chunk that straddles a chunk boundary. */
/* We can avoid testing the shift amount twice */
/* by expanding the CFETCH_LEFT/right macros in-line. */
#  define CFETCH2(cptr, cskew, skew)\
	((cskew) < 8 ?\
	 ((CCONT(cptr, 0) << (cskew)) & left_masks2[cskew]) +\
	  (CCONT(cptr, 0) >> (skew)) +\
	  (((chunk)(((const uchar *)(cptr))[2]) << (cskew)) & 0xff00) :\
	 (((CCONT(cptr, 0) & 0xff00) >> (skew)) & 0xff) +\
	  ((CCONT(cptr, 1) >> (skew)) & right_masks2[skew]) +\
	   (CCONT(cptr, 1) << (cskew)))
#endif

typedef enum {
    COPY_OR = 0, COPY_STORE, COPY_AND, COPY_FUNNY
} copy_function;
typedef struct {
    int invert;
    copy_function op;
} copy_mode;

static const copy_mode copy_modes[16] = {
    {~0, COPY_FUNNY},
    {~0, COPY_AND},
    {0, COPY_OR},
    {0, COPY_OR},	
    {0, COPY_AND},
    {0, COPY_FUNNY},
    {0, COPY_STORE},
    {0, COPY_OR},	
    {~0, COPY_OR},	
    {~0, COPY_STORE},
    {0, COPY_FUNNY},
    {0, COPY_OR},	
    {0, COPY_OR},	
    {0, COPY_OR},
    {0, COPY_OR},
    {0, COPY_OR},
};

#define FUNNY_CASE()\
  (invert ?  (-1) :\
   fillRectangle(x, y, w, h, color0))

#define COPY_BEGIN\
	while ( h-- > 0 )\
	{	uchar *pptr = dest;\
		const uchar *sptr = line;\
		int sbyte = *sptr;\
		uint bit = first_bit;\
		int count = w;\
		do\
		{
#define COPY_END\
			if ( (bit >>= 1) == 0 )\
				bit = 0x80, sbyte = *++sptr;\
			pptr++;\
		}\
		while ( --count > 0 );\
		line += sraster;\
		dest = dest + draster;\
	}
	
static void
mapped8_copy01(uchar * dest, const uchar * line, int first_bit,
	       int sraster, uint draster, int w, int h, uchar b0, uchar b1)
{
    COPY_BEGIN
	* pptr = (sbyte & bit ? b1 : b0);
    COPY_END
}

static void
mapped8_copyN1(uchar * dest, const uchar * line, int first_bit,
	       int sraster, uint draster, int w, int h, uchar b1)
{
    COPY_BEGIN
	if (sbyte & bit)
	*pptr = b1;
    COPY_END
}

static void
mapped8_copy0N(uchar * dest, const uchar * line, int first_bit,
	       int sraster, uint draster, int w, int h, uchar b0)
{
    COPY_BEGIN
	if (!(sbyte & bit))
	*pptr = b0;
    COPY_END
}
#undef COPY_BEGIN
#undef COPY_END

#define declare_unpack_color(r, g, b, color)\
	uchar r = (uchar)(color >> 16);\
	uchar g = (uchar)((uint)color >> 8);\
	uchar b = (uchar)color
	
#define put3(ptr, r, g, b)\
	(ptr)[0] = r, (ptr)[1] = g, (ptr)[2] = b

#define putw(ptr, wxyz)\
	*(quint32 *)(ptr) = (wxyz)


#if arch_is_big_endian
#  define set_color24_cache(crgb, r, g, b)\
	color24.rgbr = rgbr = ((quint32)(crgb) << 8) | (r),\
	color24.gbrg = gbrg = (rgbr << 8) | (g),\
	color24.brgb = brgb = (gbrg << 8) | (b),\
	color24.rgb = (crgb)
#else
#  define set_color24_cache(crgb, r, g, b)\
	color24.rgbr = rgbr =\
		((quint32)(r) << 24) | ((quint32)(b) << 16) |\
		((quint16)(g) << 8) | (r),\
	color24.brgb = brgb = (rgbr << 8) | (b),\
	color24.gbrg = gbrg = (brgb << 8) | (g),\
	color24.rgb = (crgb)
#endif

#define color_swap_bytes(color)\
  (((color) >> 24) + (((color) >> 8) & 0xff00) +\
   (((color) & 0xff00) << 8) + ((color) << 24))
#if arch_is_big_endian
#  define arrange_bytes(color) (color)
#else
#  define arrange_bytes(color) color_swap_bytes(color)
#endif

static const uchar b_w_palette_string[6] = {
    0xff, 0xff, 0xff, 0, 0, 0
};
const PSDevString mem_mono_b_w_palette = {
    b_w_palette_string, 6
};
static const uchar w_b_palette_string[6] = {
    0, 0, 0, 0xff, 0xff, 0xff
};
const PSDevString mem_mono_w_b_palette = {
    w_b_palette_string, 6
};

static const char * memMonoName = "image1";
static const char * memMapped2Name = "image2";
static const char * memMapped4Name = "image4";
static const char * memMapped8Name = "image8";
static const char * memTrue16Name = "image16";
static const char * memTrue24Name = "image24";
static const char * memTrue32Name = "image32";
static const char * memAlphaBufferName = "image(alpha buffer)";

XWPSDeviceMem::XWPSDeviceMem(QObject * parent)
	:XWPSDeviceForward("mem", 0,0,72,72, 1,1,	1,0,2,0, parent)
{
	procs.open__.openmem = &XWPSDeviceMem::openMem;
	procs.get_initial_matrix_.get_initial_matrixmem = &XWPSDeviceMem::getInitialMatrixMem;
	procs.close__.closemem = &XWPSDeviceMem::closeMem;
	procs.get_bits_.get_bits = &XWPSDevice::getBitsDefault;
	procs.get_params_.get_params = &XWPSDevice::getParamsDefault;
	procs.put_params_.put_params = &XWPSDevice::putParamsDefault;
	procs.get_xfont_device_.get_xfont_device = &XWPSDevice::getXFontDeviceDefault;
	procs.map_rgb_alpha_color_.map_rgb_alpha_color = &XWPSDevice::mapRGBAlphaColorDefault;
	procs.get_band_.get_band = &XWPSDevice::getBandDefault;
	procs.copy_rop_.copy_rop = &XWPSDevice::copyRopDefault;
	procs.fill_path_.fill_path = &XWPSDevice::fillPathDefault;
	procs.stroke_path_.stroke_path = &XWPSDevice::strokePathDefault;
	procs.fill_mask_.fill_mask = &XWPSDevice::fillMaskDefault;
	procs.fill_trapezoid_.fill_trapezoid = &XWPSDevice::fillTrapezoidDefault;
	procs.fill_parallelogram_.fill_parallelogram = &XWPSDevice::fillParallelogramDefault;
	procs.fill_triangle_.fill_triangle = &XWPSDevice::fillTriangleDefault;
	procs.draw_thin_line_.draw_thin_line = &XWPSDevice::drawThinLineDefault;
	procs.begin_image_.begin_image = &XWPSDevice::beginImageDefault;
	procs.get_clipping_box_.get_clipping_box = &XWPSDevice::getClipingBoxDefault;
	procs.begin_typed_image_.begin_typed_image = &XWPSDevice::beginTypedImageDefault;
	procs.map_color_rgb_alpha_.map_color_rgb_alpha = &XWPSDevice::mapColorRGBAlphaDefault;
	procs.get_hardware_params_.get_hardware_params = &XWPSDevice::getHardwareParamsDefault;
	procs.text_begin_.text_begin = &XWPSDevice::textBeginDefault;
		
	raster = 0;
	base = 0;
	foreign_bits = true;
	foreign_line_pointers = true;
	num_planes = 0;
	line_ptrs = 0;
	color24.rgb = 0;
	log2_alpha_bits = 0;
	mapped_x = 0;
	mapped_y = 0;
	mapped_height = 0;
	mapped_start = 0;
	save_color = ps_no_color_index;
	plane_depth = 0;
	own_palette = false;
}

XWPSDeviceMem::~XWPSDeviceMem()
{
	if (base)
	{
		if (!foreign_bits)
			delete [] base;
		base = 0;
	}
	
	if (own_palette)
	{
		delete [] palette.data;
		palette.data = 0;
		own_palette = false;
	}
}

ulong XWPSDeviceMem::bitmapSize()
{
	return dataSize(width, height);
}

void  XWPSDeviceMem::bitsFillRectangle(uchar * dest, 
	                       int dest_bit, 
	                       uint draster,
		                     uint pattern, 
		                     int width_bits, 
		                     int heightA)
{
	uint bit;
  uint right_mask;
  int line_count = heightA;
  uint *ptr;
  int last_bit;
#define mono_masks mono_fill_masks
#define FOR_EACH_LINE(stat)\
	do { stat } while ( ptr = (ptr + draster), --line_count )
	
	dest += (dest_bit >> 3) & -chunk_align_bytes;
  ptr = (uint *) dest;
  bit = dest_bit & chunk_align_bit_mask;
  last_bit = width_bits + bit - (chunk_bits + 1);
  
  if (last_bit < 0)
  {
  	set_mono_thin_mask(right_mask, width_bits, bit);
  	switch ((uchar) pattern)
  	{
  		case 0:
				FOR_EACH_LINE(*ptr &= ~right_mask;);
				break;
				
	    case 0xff:
				FOR_EACH_LINE(*ptr |= right_mask;);
				break;
				
	    default:
				FOR_EACH_LINE(*ptr = (*ptr & ~right_mask) | (pattern & right_mask););
  	}
  }
  else
  {
  	uint mask;
		int last = last_bit >> chunk_log2_bits;

		set_mono_left_mask(mask, bit);
		set_mono_right_mask(right_mask, (last_bit & chunk_bit_mask) + 1);
		switch (last)
		{
			case 0:		/* 2 chunks */
				switch ((uchar) pattern) 
				{
		    	case 0:
						FOR_EACH_LINE(*ptr &= ~mask; ptr[1] &= ~right_mask;);
						break;
					
		    	case 0xff:
						FOR_EACH_LINE(*ptr |= mask; ptr[1] |= right_mask;);
						break;
					
		    	default:
						FOR_EACH_LINE(*ptr = (*ptr & ~mask) | (pattern & mask);	ptr[1] = (ptr[1] & ~right_mask) | (pattern & right_mask););
				}
				break;
				
	    case 1:		/* 3 chunks */
				switch ((uchar) pattern) 
				{
		    	case 0:
						FOR_EACH_LINE(*ptr &= ~mask; ptr[1] = 0; ptr[2] &= ~right_mask;);
						break;
						
		    	case 0xff:
						FOR_EACH_LINE(*ptr |= mask; ptr[1] = ~(uint) 0; ptr[2] |= right_mask;);
						break;
						
		    	default:
						FOR_EACH_LINE(*ptr = (*ptr & ~mask) | (pattern & mask); ptr[1] = pattern;
					 												ptr[2] = (ptr[2] & ~right_mask) | (pattern & right_mask););
				}
				break;
				
	    default:
	    	{		/* >3 chunks */
		    	uint byte_count = (last_bit >> 3) & -chunk_bytes;

		    	switch ((uchar) pattern) 
		    	{
						case 0:
			    		FOR_EACH_LINE(*ptr &= ~mask; memset(ptr + 1, 0, byte_count); ptr[last + 1] &= ~right_mask;);
			    		break;
			    		
						case 0xff:
			    		FOR_EACH_LINE(*ptr |= mask; memset(ptr + 1, 0xff, byte_count); ptr[last + 1] |= right_mask;);
			    		break;
			    		
						default:
			    		FOR_EACH_LINE(*ptr = (*ptr & ~mask) | (pattern & mask);	memset(ptr + 1, (uchar) pattern, byte_count);
					     							ptr[last + 1] = (ptr[last + 1] & ~right_mask) | (pattern & right_mask););
		    	}
			}
		}
  }
#undef FOR_EACH_LINE
#undef mono_masks
}

ulong XWPSDeviceMem::bitsSize(int widthA, int heightA)
{
	XWPSRenderPlane plane1;
  const XWPSRenderPlane *planesA;
  ulong size;
  int pi;

  if (num_planes)
		planesA = planes;
  else
		planesA = &plane1, plane1.depth = color_info.depth, num_planes = 1;
		
  for (size = 0, pi = 0; pi < num_planes; ++pi)
		size += bitmap_raster(widthA * planes[pi].depth);
  return ROUND_UP(size * heightA, ARCH_ALIGN_PTR_MOD);
}

int XWPSDeviceMem::closeABuf()
{
	int code = flushABuf();
  if (code < 0)
		return code;
  return closeMem();
}

int XWPSDeviceMem::closeMem()
{
	if (base)
	{
		if (!foreign_bits)
			delete [] base;
		base = 0;
	}
	
	return 0;
}

int XWPSDeviceMem::copyAlphaAlpha(const uchar *data, 
	                             int data_x,
	                             int rasterA, 
	                             ulong id, 
	                             int x, 
	                             int y, 
	                             int widthA, 
	                             int heightA,
	                             ulong color, 
	                             int )
{
	return (color == 0 ?  fillRectangle(x, y, widthA, heightA, color) :
	    		copyColor(data, data_x, rasterA, id, x, y, widthA, heightA));
}

int XWPSDeviceMem::copyAlphaTrue24(const uchar * baseA, 
	                    int sourcex,
		                  int sraster, 
		                  ulong id, 
		                  int x, 
		                  int y, 
		                  int w, 
		                  int h,
		      						ulong color, 
		      						int depth)
{
	uchar *line;
	uchar * dest;
	uint draster;
	
	declare_unpack_color(r, g, b, color);

  fit_copy(base, sourcex, sraster, id, x, y, w, h);
  draster = raster;
  dest = (uchar*)(scanLineBase(y) + ((x  * 3) & -chunk_align_bytes));
  line = (uchar*)baseA;
  while (h-- > 0)
  {
  	uchar *pptr = dest;
		int sx;

		for (sx = sourcex; sx < sourcex + w; ++sx, pptr += 3) 
		{
	    int alpha2, alpha;

	    if (depth == 2)	
				alpha = ((line[sx >> 2] >> ((3 - (sx & 3)) << 1)) & 3) * 5;
	    else
				alpha2 = line[sx >> 1], alpha = (sx & 1 ? alpha2 & 0xf : alpha2 >> 4);
	    if (alpha == 15) 
	    {
				put3(pptr, r, g, b);
	    } 
	    else if (alpha != 0) 
	    {
#define make_shade(old, clr, alpha, amax) \
  (old) + (((int)(clr) - (int)(old)) * (alpha) / (amax))
		
				pptr[0] = make_shade(pptr[0], r, alpha, 15);
				pptr[1] = make_shade(pptr[1], g, alpha, 15);
				pptr[2] = make_shade(pptr[2], b, alpha, 15);
#undef make_shade
	    }
		}
		line += sraster;
		dest = dest + draster;
  }
  
  return 0;
}

int XWPSDeviceMem::copyColorMapped2(const uchar * baseA, 
	                     int sourcex, 
	                     int sraster,
		                   ulong id, 
		                   int x, 
		                   int y, 
		                   int w, 
		                   int h)
{
	int code;

  fit_copy(baseA, sourcex, sraster, id, x, y, w, h);
  width <<= 1;
  code = copyMono(baseA, sourcex << 1, sraster, id, x << 1, y, w << 1, h, 0, 1);
  width >>= 1;
  return code;
}

int XWPSDeviceMem::copyColorMapped4(const uchar * baseA, 
	                     int sourcex, 
	                     int sraster,
		                   ulong id, 
		                   int x, 
		                   int y, 
		                   int w, 
		                   int h)
{
	int code;
  width <<= 2;
  code = copyMono(baseA, sourcex << 2, sraster, id, x << 2, y, w << 2, h, 0, 1);
  width >>= 2;
  return code;
}

int XWPSDeviceMem::copyColorMapped8(const uchar * baseA, 
	                     int sourcex, 
	                     int sraster,
		                   ulong id, 
		                   int x, 
		                   int y, 
		                   int w, 
		                   int h)
{
	fit_copy(baseA, sourcex, sraster, id, x, y, w, h);
  bytes_copy_rectangle(scanLineBase(y) + x, raster, baseA + sourcex, sraster, w, h);
  return 0;
}

int XWPSDeviceMem::copyColorTrue16(const uchar * baseA, 
	                     int sourcex, 
	                     int sraster,
		                   ulong id, 
		                   int x, 
		                   int y, 
		                   int w, 
		                   int h)
{
	fit_copy(baseA, sourcex, sraster, id, x, y, w, h);
	bytes_copy_rectangle(scanLineBase(y) + (x << 1), raster, baseA + (sourcex << 1), sraster, (w << 1), h);
  return 0;
}

int XWPSDeviceMem::copyColorTrue24(const uchar * baseA, 
	                     int sourcex, 
	                     int sraster,
		                   ulong id, 
		                   int x, 
		                   int y, 
		                   int w, 
		                   int h)
{
	fit_copy(baseA, sourcex, sraster, id, x, y, w, h);
  bytes_copy_rectangle(scanLineBase(y) + (x * 3), raster, baseA + (sourcex * 3), sraster, (w * 3), h);
  return 0;
}

int XWPSDeviceMem::copyColorTrue32(const uchar * baseA, 
	                     int sourcex, 
	                     int sraster,
		                   ulong id, 
		                   int x, 
		                   int y, 
		                   int w, 
		                   int h)
{
	fit_copy(baseA, sourcex, sraster, id, x, y, w, h);
  bytes_copy_rectangle(scanLineBase(y) + (x << 2), raster, baseA + (sourcex << 2), sraster, (w << 2), h);
  return 0;
}

int  XWPSDeviceMem::copyDevice(XWPSDevice **pnew)
{
	XWPSDeviceMem * ret = new XWPSDeviceMem;
	ret->copyDeviceParamMem(this);
	ret->copyDeviceProc(this);
	*pnew = ret;
	return 0;
}

void XWPSDeviceMem::copyDeviceParamMem(XWPSDeviceMem * proto)
{
	copyDeviceParam(proto);
	raster = proto->raster;
	foreign_bits = proto->foreign_bits;
	foreign_line_pointers = proto->foreign_line_pointers;
	num_planes = proto->num_planes;
	initial_matrix = proto->initial_matrix;
	color24.rgb = proto->color24.rgb;
	color24.rgbr = proto->color24.rgbr;
	color24.gbrg = proto->color24.gbrg;
	color24.brgb = proto->color24.brgb;
	log2_scale = proto->log2_scale;
	log2_alpha_bits = proto->log2_alpha_bits;
	mapped_x = proto->mapped_x;
	mapped_y = proto->mapped_y;
	mapped_height = proto->mapped_height;
	mapped_start = proto->mapped_start;
	save_color = proto->save_color;
	plane_depth = proto->plane_depth;
}

int XWPSDeviceMem::copyMonoABuf(const uchar *source_data, 
	                     int source_x, 
	                     int source_raster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one)
{
	PSYTransfer yt;

  if (zero != ps_no_color_index || one == ps_no_color_index)
		return -1;
  x -= mapped_x;
  fit_copy_xyw(source_data, source_x, source_raster, id, x, y, w, h);
  if (w <= 0 || h <= 0)
		return 0;
    
  save_color = one;
  y_transfer_init(&yt, y, h);
  while (yt.height_left > 0) 
  {
		y_transfer_next(&yt);
		copyMono(source_data + (yt.y_next - y) * source_raster, source_x, source_raster, ps_no_bitmap_id,
				     x, yt.transfer_y, w, yt.transfer_height, ps_no_color_index, 1);
  }
  return 0;
}

int XWPSDeviceMem::copyMonoMapped2(const uchar *baseA, 
	                     int sourcex, 
	                     int sraster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one)
{
	const uchar *line;
  int first_bit;
  uchar first_mask, b0, b1, bxor, left_mask, right_mask;
  static const uchar btab[4] = {0, 0x55, 0xaa, 0xff};
  static const uchar bmask[4] = {0xc0, 0x30, 0xc, 3};
  static const uchar lmask[4] = {0, 0xc0, 0xf0, 0xfc};
  
  uchar * dest;
  uint draster;
  
  fit_copy(baseA, sourcex, sraster, id, x, y, w, h);
  draster = raster;
  dest = scanLineBase(y) + ((x >> 2) & -chunk_align_bytes);
  
  line = baseA + (sourcex >> 3);
  first_bit = 0x80 >> (sourcex & 7);
  first_mask = bmask[x & 3];
  left_mask = lmask[x & 3];
  right_mask = ~lmask[(x + w) & 3];
  if ((x & 3) + w <= 3)
		left_mask = right_mask = left_mask | right_mask;
  b0 = btab[zero & 3];
  b1 = btab[one & 3];
  bxor = b0 ^ b1;
  
  while (h-- > 0) 
  {
		uchar *pptr = (uchar*)dest;
		uchar *sptr = (uchar*)line;
		int sbyte = *sptr++;
		int bit = first_bit;
		uchar mask = first_mask;
		int count = w;
		if (one != ps_no_color_index) 
		{
	    if (zero != ps_no_color_index) 
	    {
				uchar data = (*pptr & left_mask) | (b0 & ~left_mask);

				for ( ; ; ) 
				{
		    	if (sbyte & bit)
						data ^= bxor & mask;
		    	if (--count <= 0)
						break;
		    	if ((bit >>= 1) == 0)
						bit = 0x80, sbyte = *sptr++;
		    	if ((mask >>= 2) == 0)
						mask = 0xc0, *pptr++ = data, data = b0;
				}
				if (mask != 0xc0)
		    	*pptr =	(*pptr & right_mask) | (data & ~right_mask);
	    } 
	    else 
	    {
				for ( ; ; ) 
				{
		    	if (sbyte & bit)
						*pptr = (*pptr & ~mask) + (b1 & mask);
		    	if (--count <= 0)
						break;
		    	if ((bit >>= 1) == 0)
						bit = 0x80, sbyte = *sptr++;
		    	if ((mask >>= 2) == 0)
						mask = 0xc0, pptr++;
				}
	    }
		} 
		else 
		{
	    for ( ; ; ) 
	    {
				if (!(sbyte & bit)) 
				{
		    	if (zero != ps_no_color_index)
						*pptr = (*pptr & ~mask) + (b0 & mask);
				}
				if (--count <= 0)
		    	break;
				if ((bit >>= 1) == 0)
		    	bit = 0x80, sbyte = *sptr++;
				if ((mask >>= 2) == 0)
		    	mask = 0xc0, pptr++;
	    }
		}
		line += sraster;
		dest = dest + draster;
  }
  return 0;
}

int XWPSDeviceMem::copyMonoMapped4(const uchar *baseA, 
	                     int sourcex, 
	                     int sraster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one)
{
	uchar *line;
	uchar * dest;
	uint draster;
	
	uchar invert, bb;

  fit_copy(baseA, sourcex, sraster, id, x, y, w, h);
  draster = raster;
  dest = (uchar*)(scanLineBase(y) + ((x >> 1) & -chunk_align_bytes));
  line = (uchar*)(baseA + (sourcex >> 3));
  if (one == ps_no_color_index) 
  {
		if (zero == ps_no_color_index)
	    return 0;
		invert = 0xff;
		bb = ((uchar) zero << 4) | (uchar) zero;
  } 
  else if (zero == ps_no_color_index) 
  {
		invert = 0;
		bb = ((uchar) one << 4) | (uchar) one;
  }
  else
  {
  	int shift = ~(sourcex ^ x) & 1;
		uchar oz[4];

		oz[0] = (uchar)((zero << 4) | zero);
		oz[1] = (uchar)((zero << 4) | one);
		oz[2] = (uchar)((one << 4) | zero);
		oz[3] = (uchar)((one << 4) | one);
		do
		{
			uchar *dptr = (uchar *) dest;
	    uchar *sptr = line;
	    uint sbyte = *sptr++;
	    int sbit = ~sourcex & 7;
	    int count = w;
	    if (x & 1) 
	    {
				*dptr = (*dptr & 0xf0) |  ((sbyte >> sbit) & 1 ? one : zero);
				--count;	
				if (--sbit < 0)
		    	sbit = 7, sbyte = *sptr++;
				++dptr;
	    }
	    sbyte <<= shift;
	    sbit += shift - 1;
	    while (count >= 2 && sbit >= 0) 
	    {
				*dptr++ = oz[(sbyte >> sbit) & 3];
				sbit -= 2, count -= 2;
	    }
	    if (shift) 
	    {
				sbyte >>= 1;
				for (; count >= 8; dptr += 4, count -= 8) 
				{
		    	sbyte = *sptr++;
		    	dptr[0] = oz[sbyte >> 6];
		    	dptr[1] = oz[(sbyte >> 4) & 3];
		    	dptr[2] = oz[(sbyte >> 2) & 3];
		    	dptr[3] = oz[sbyte & 3];
				}
				sbyte <<= 1;
	    } 
	    else 
	    {
				for (; count >= 8; dptr += 4, count -= 8) 
				{
		    	sbyte = (sbyte << 8) | *sptr++;
		    	dptr[0] = oz[(sbyte >> 7) & 3];
		    	dptr[1] = oz[(sbyte >> 5) & 3];
		    	dptr[2] = oz[(sbyte >> 3) & 3];
		    	dptr[3] = oz[(sbyte >> 1) & 3];
				}
	    }
	    if (!count)
				continue;
			if (sbit < 0) 
			{
				sbyte = (sbyte << 8) | (*sptr << shift);
				sbit = 7;
	    }
	    while (count >= 2) 
	    {
				*dptr++ = oz[(sbyte >> sbit) & 3];
				sbit -= 2, count -= 2;
	    }
	    if (count) 
	    {
				*dptr = (*dptr & 0x0f) | (((sbyte >> sbit) & 2 ? one : zero) << 4);
	    }
		} while(((line += sraster, dest += draster), --h) > 0);
		
		return 0;
  }
  
  do 
  {
		uchar *dptr = (uchar *) dest;
		uchar *sptr = line;
		int sbyte = *sptr++ ^ invert;
		int sbit = 0x80 >> (sourcex & 7);
		uchar mask = (x & 1 ? 0x0f : 0xf0);
		int count = w;

		do 
		{
	    if (sbyte & sbit)
				*dptr = (*dptr & ~mask) | (bb & mask);
	    if ((sbit >>= 1) == 0)
				sbit = 0x80, sbyte = *sptr++ ^ invert;
	    dptr += (mask = ~mask) >> 7;
		} while (--count > 0);
		line += sraster;
		dest = dest + draster;
  } while (--h > 0);
  return 0;
}

int XWPSDeviceMem::copyMonoMapped8(const uchar *baseA, 
	                     int sourcex, 
	                     int sraster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one)
{
	uchar *line;
  int first_bit;

  uchar * dest;
	uint draster;
  fit_copy(baseA, sourcex, sraster, id, x, y, w, h);
  draster = raster;
  dest = (uchar*)(scanLineBase(y) + (x & -chunk_align_bytes));
  line = (uchar*)(baseA + (sourcex >> 3));
  first_bit = 0x80 >> (sourcex & 7);
#define is_color(c) ((int)(c) != (int)ps_no_color_index)
  if (is_color(one)) 
  {
		if (is_color(zero))
	    mapped8_copy01(dest, line, first_bit, sraster, draster,  w, h, (uchar) zero, (uchar) one);
		else
	    mapped8_copyN1(dest, line, first_bit, sraster, draster,  w, h, (uchar) one);
  } 
  else if (is_color(zero))
		mapped8_copy0N(dest, line, first_bit, sraster, draster,  w, h, (uchar) zero);
#undef is_color
  return 0;
}

int XWPSDeviceMem::copyMonoTrue16(const uchar *baseA, 
	                     int sourcex, 
	                     int sraster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one)
{
#if arch_is_big_endian
  const ushort zero16 = (ushort)zero;
  const ushort one16 = (ushort)one;
#else
  ushort zero16 = ((uint) (uchar) zero << 8) + ((ushort) zero >> 8);
  ushort one16 = ((uint) (uchar) one << 8) + ((ushort) one >> 8);
#endif
  uchar *line;
  int first_bit;
  
  uchar * dest;
	uint draster;
	fit_copy(baseA, sourcex, sraster, id, x, y, w, h);
	draster = raster;
  dest = (uchar*)(scanLineBase(y) + ((x << 1) & -chunk_align_bytes));
  
  line = (uchar*)(baseA + (sourcex >> 3));
  first_bit = 0x80 >> (sourcex & 7);
  while (h-- > 0) 
  {
		ushort *pptr = (ushort *) dest;
		uchar *sptr = line;
		int sbyte = *sptr++;
		int bit = first_bit;
		int count = w;

		do 
		{
	    if (sbyte & bit) 
	    {
				if (one != ps_no_color_index)
		    	*pptr = one16;
	    } 
	    else 
	    {
				if (zero != ps_no_color_index)
		    *pptr = zero16;
	    }
	    if ((bit >>= 1) == 0)
				bit = 0x80, sbyte = *sptr++;
	    pptr++;
		}	while (--count > 0);
		line += sraster;
		dest = dest + draster;
  }
  return 0;
}

int XWPSDeviceMem::copyMonoTrue24(const uchar *baseA, 
	                     int sourcex, 
	                     int sraster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one)
{
	const uchar *line;
  int sbit;
  int first_bit;
  
  uchar * dest;
	uint draster;
	
	fit_copy(baseA, sourcex, sraster, id, x, y, w, h);
	draster = raster;
  dest = (scanLineBase(y) + ((x  * 3) & -chunk_align_bytes));
  line = baseA + (sourcex >> 3);
  sbit = sourcex & 7;
  first_bit = 0x80 >> sbit;
  if (zero != ps_no_color_index)
  {
  	declare_unpack_color(r0, g0, b0, zero);
		declare_unpack_color(r1, g1, b1, one);
		while (h-- > 0) 
		{
	    uchar *pptr = (uchar*)dest;
	    uchar *sptr = (uchar*)line;
	    int sbyte = *sptr++;
	    int bit = first_bit;
	    int count = w;

	    do 
	    {
				if (sbyte & bit) 
				{
		    	if (one != ps_no_color_index)
						put3(pptr, r1, g1, b1);
				} 
				else
		    	put3(pptr, r0, g0, b0);
				pptr += 3;
				if ((bit >>= 1) == 0)
		    	bit = 0x80, sbyte = *sptr++;
	    } while (--count > 0);
	    line += sraster;
	    dest = dest + draster;
		}
  }
  else if (one != ps_no_color_index)
  {
  	declare_unpack_color(r1, g1, b1, one);
		int first_mask = first_bit << 1;
		int first_count, first_skip;

		if (sbit + w > 8)
	    first_mask -= 1, first_count = 8 - sbit;
		else
	    first_mask -= first_mask >> w, first_count = w;
		first_skip = first_count * 3;
		while (h-- > 0) 
		{
	    uchar *pptr = (uchar*)dest;
	    uchar *sptr = (uchar*)line;
	    int sbyte = *sptr++ & first_mask;
	    int count = w - first_count;

	    if (sbyte) 
	    {
				int bit = first_bit;

				do 
				{
		    	if (sbyte & bit)
						put3(pptr, r1, g1, b1);
		    	pptr += 3;
				}	while ((bit >>= 1) & first_mask);
	    } 
	    else
				pptr += first_skip;
	    while (count >= 8) 
	    {
				sbyte = *sptr++;
				if (sbyte & 0xf0) 
				{
		    	if (sbyte & 0x80)
						put3(pptr, r1, g1, b1);
		    	if (sbyte & 0x40)
						put3(pptr + 3, r1, g1, b1);
		    	if (sbyte & 0x20)
						put3(pptr + 6, r1, g1, b1);
		    	if (sbyte & 0x10)
						put3(pptr + 9, r1, g1, b1);
				}
				if (sbyte & 0xf) 
				{
		    	if (sbyte & 8)
						put3(pptr + 12, r1, g1, b1);
		    	if (sbyte & 4)
						put3(pptr + 15, r1, g1, b1);
		    	if (sbyte & 2)
						put3(pptr + 18, r1, g1, b1);
		    	if (sbyte & 1)
						put3(pptr + 21, r1, g1, b1);
				}
				pptr += 24;
				count -= 8;
	    }
	    if (count > 0) 
	    {
				int bit = 0x80;

				sbyte = *sptr++;
				do 
				{
		    	if (sbyte & bit)
						put3(pptr, r1, g1, b1);
		    	pptr += 3;
		    	bit >>= 1;
				}	while (--count > 0);
	    }
	    line += sraster;
	    dest = dest + draster;
		}
  }
  
  return 0;
}

int XWPSDeviceMem::copyMonoTrue32(const uchar *baseA, 
	                     int sourcex, 
	                     int sraster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one)
{
	quint32 a_zero = arrange_bytes(zero);
  quint32 a_one = arrange_bytes(one);
  uchar *line;
  
  uchar * dest;
	uint draster;
	fit_copy(baseA, sourcex, sraster, id, x, y, w, h);
	draster = raster;
	dest = (uchar*)(scanLineBase(y) + ((x << 2) & -chunk_align_bytes));
	line = (uchar*)(baseA + (sourcex >> 3));
  if (zero == ps_no_color_index)
  {
  	int first_bit = sourcex & 7;
		int w_first = qMin(w, 8 - first_bit);
		int w_rest = w - w_first;

		if (one == ps_no_color_index)
	    return 0;
	    
	  while (h-- > 0)
	  {
	  	quint32 *pptr = (quint32 *) dest;
	    uchar *sptr = line;
	    int sbyte = (*sptr++ << first_bit) & 0xff;
	    int count = w_first;

	    if (sbyte)
				do 
				{
		    	if (sbyte & 0x80)
						*pptr = a_one;
		    	sbyte <<= 1;
		    	pptr++;
				}	while (--count > 0);
	    else
				pptr += count;
	    for (count = w_rest; count >= 8; count -= 8, pptr += 8) 
	    {
				sbyte = *sptr++;
				if (sbyte) 
				{
		    	if (sbyte & 0x80) pptr[0] = a_one;
		    	if (sbyte & 0x40) pptr[1] = a_one;
		    	if (sbyte & 0x20) pptr[2] = a_one;
		    	if (sbyte & 0x10) pptr[3] = a_one;
		    	if (sbyte & 0x08) pptr[4] = a_one;
		    	if (sbyte & 0x04) pptr[5] = a_one;
		    	if (sbyte & 0x02) pptr[6] = a_one;
		    	if (sbyte & 0x01) pptr[7] = a_one;
				}
	    }
	    if (count) 
	    {
				sbyte = *sptr;
				do 
				{
		    	if (sbyte & 0x80)
						*pptr = a_one;
		    	sbyte <<= 1;
		    	pptr++;
				}	while (--count > 0);
	    }
	    line += sraster;
	    dest = dest + draster;
	  }
  }
  else
  {
  	int first_bit = 0x80 >> (sourcex & 7);

		while (h-- > 0) 
		{
	    quint32 *pptr = (quint32 *) dest;
	    uchar *sptr = line;
	    int sbyte = *sptr++;
	    int bit = first_bit;
	    int count = w;

	    do 
	    {
				if (sbyte & bit) 
				{
		    	if (one != ps_no_color_index)
						*pptr = a_one;
				} 
				else
		    	*pptr = a_zero;
				if ((bit >>= 1) == 0)
		    	bit = 0x80, sbyte = *sptr++;
				pptr++;
	    } while (--count > 0);
	    line += sraster;
	    dest = dest + draster;
		}
  }
  
  return 0;
}

int XWPSDeviceMem::copyMonoMono(const uchar *source_data, 
	                     int source_x, 
	                     int source_raster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong color0, 
	                     ulong color1)
{
	const uchar *bptr;
  int dbit, wleft;
  uint mask;
  copy_mode mode;
  
  uchar * dbptr;
  uint dest_raster;
#define mono_masks mono_copy_masks
#define optr ((chunk *)dbptr)
  int skew;
  uint invert;
  
  fit_copy(source_data, source_x, source_raster, id, x, y, w, h);
#if ps_no_color_index_value != -1	/* hokey! */
  if (color0 == ps_no_color_index)
		color0 = -1;
  if (color1 == ps_no_color_index)
		color1 = -1;
#endif
  mode = copy_modes[((int)color0 << 2) + (int)color1 + 5];
  invert = (uint)mode.invert;
  dest_raster = raster;
	dbptr = (uchar*)(scanLineBase(y) + (x >> 3 & -chunk_align_bytes));
	
  bptr = source_data + ((source_x & ~chunk_align_bit_mask) >> 3);
  dbit = x & chunk_align_bit_mask;
  skew = dbit - (source_x & chunk_align_bit_mask);
  
#define CINVERT(bits) ((bits) ^ invert)
#define WRITE_OR_MASKED(bits, mask, off)\
  optr[off] |= (CINVERT(bits) & mask)
#define WRITE_STORE_MASKED(bits, mask, off)\
  optr[off] = ((optr[off] & ~mask) | (CINVERT(bits) & mask))
#define WRITE_AND_MASKED(bits, mask, off)\
  optr[off] &= (CINVERT(bits) | ~mask)
  
#define WRITE_OR(bits)  *optr |= CINVERT(bits)
#define WRITE_STORE(bits) *optr = CINVERT(bits)
#define WRITE_AND(bits) *optr &= CINVERT(bits)

#define NEXT_X_CHUNK()\
  bptr += chunk_bytes; dbptr += chunk_bytes
  
#define END_Y_LOOP(sdelta, ddelta)\
  bptr += sdelta; dbptr += ddelta
  
  if ((wleft = w + dbit - chunk_bits) <= 0)
  {
		set_mono_thin_mask(mask, w, dbit);

#define WRITE_SINGLE(wr_op, src)\
  for ( ; ; )\
   { wr_op(src, mask, 0);\
     if ( --h == 0 ) break;\
     END_Y_LOOP(source_raster, dest_raster);\
   }

#define WRITE1_LOOP(src)\
  switch ( mode.op ) {\
    case COPY_OR: WRITE_SINGLE(WRITE_OR_MASKED, src); break;\
    case COPY_STORE: WRITE_SINGLE(WRITE_STORE_MASKED, src); break;\
    case COPY_AND: WRITE_SINGLE(WRITE_AND_MASKED, src); break;\
    default: return FUNNY_CASE();\
  }
  
  	if (skew >= 0)
  	{
  		if (skew == 0)
  		{
  			WRITE1_LOOP(CFETCH_ALIGNED(bptr));
  		}
  		else
  		{
#if CFETCH_USES_CSKEW
				int cskew = chunk_bits - skew;
#endif

				WRITE1_LOOP(CFETCH_RIGHT(bptr, skew, cskew));
  		}
  	}
  	else if (wleft <= skew)
  	{
#if CFETCH_USES_CSKEW
	    int cskew = chunk_bits + skew;
#endif

	    skew = -skew;
	    WRITE1_LOOP(CFETCH_LEFT(bptr, skew, cskew));
  	}
  	else
  	{
  		int cskew = -skew;

	    skew += chunk_bits;
	    WRITE1_LOOP(CFETCH2(bptr, cskew, skew));
  	}
#undef WRITE1_LOOP
#undef WRITE_SINGLE
  }
  else if (wleft <= skew)
  {
  	uint rmask;
		int cskew = chunk_bits - skew;

		set_mono_left_mask(mask, dbit);
		set_mono_right_mask(rmask, wleft);
#undef CINVERT
#define CINVERT(bits) (bits)

#if arch_is_big_endian	
#  define WRITE_1TO2(wr_op)\
  for ( ; ; )\
   { register uint bits = CFETCH_ALIGNED(bptr) ^ invert;\
     wr_op(bits >> skew, mask, 0);\
     wr_op(bits << cskew, rmask, 1);\
     if ( --h == 0 ) break;\
     END_Y_LOOP(source_raster, dest_raster);\
   }
#else /* byte swapping */
#  define WRITE_1TO2(wr_op)\
  for ( ; ; )\
   { wr_op(CFETCH_RIGHT(bptr, skew, cskew) ^ invert, mask, 0);\
     wr_op(CFETCH_LEFT(bptr, cskew, skew) ^ invert, rmask, 1);\
     if ( --h == 0 ) break;\
     END_Y_LOOP(source_raster, dest_raster);\
   }
#endif

    switch (mode.op) 
    {
	    case COPY_OR:
				WRITE_1TO2(WRITE_OR_MASKED);
				break;
				
	    case COPY_STORE:
				WRITE_1TO2(WRITE_STORE_MASKED);
				break;
				
	    case COPY_AND:
				WRITE_1TO2(WRITE_AND_MASKED);
				break;
				
	    default:
				return FUNNY_CASE();
		}
#undef CINVERT
#define CINVERT(bits) ((bits) ^ invert)
#undef WRITE_1TO2
  }
  else
  {
  	uint rmask;
		int words = (wleft & ~chunk_bit_mask) >> 3;
		uint sskip = source_raster - words;
		uint dskip = dest_raster - words;
		uint bits;

		set_mono_left_mask(mask, dbit);
		set_mono_right_mask(rmask, wleft & chunk_bit_mask);
		if (skew == 0) 
		{
#define WRITE_ALIGNED(wr_op, wr_op_masked)\
  for ( ; ; )\
   { int count = wleft;\
     /* Do first partial chunk. */\
     wr_op_masked(CFETCH_ALIGNED(bptr), mask, 0);\
     /* Do full chunks. */\
     while ( (count -= chunk_bits) >= 0 )\
      { NEXT_X_CHUNK(); wr_op(CFETCH_ALIGNED(bptr)); }\
     /* Do last chunk */\
     if ( count > -chunk_bits )\
      { wr_op_masked(CFETCH_ALIGNED(bptr + chunk_bytes), rmask, 1); }\
     if ( --h == 0 ) break;\
     END_Y_LOOP(sskip, dskip);\
   }
     
      switch (mode.op) 
      {
				case COPY_OR:
		    	WRITE_ALIGNED(WRITE_OR, WRITE_OR_MASKED);
		    	break;
		    	
				case COPY_STORE:
		    	WRITE_ALIGNED(WRITE_STORE, WRITE_STORE_MASKED);
		    	break;
		    	
				case COPY_AND:
		    	WRITE_ALIGNED(WRITE_AND, WRITE_AND_MASKED);
		    	break;
		    	
				default:
		    	return FUNNY_CASE();
	    }
#undef WRITE_ALIGNED
		}
		else
		{
			int cskew = -skew & chunk_bit_mask;
	    bool case_right = (skew >= 0 ? true : ((bptr += chunk_bytes), false));

	    skew &= chunk_bit_mask;

#define WRITE_UNALIGNED(wr_op, wr_op_masked)\
  /* Prefetch partial word. */\
  bits =\
    (case_right ? CFETCH_RIGHT(bptr, skew, cskew) :\
     CFETCH2(bptr - chunk_bytes, cskew, skew));\
  wr_op_masked(bits, mask, 0);\
  /* Do full chunks. */\
  while ( count >= chunk_bits )\
    { bits = CFETCH2(bptr, cskew, skew);\
      NEXT_X_CHUNK(); wr_op(bits); count -= chunk_bits;\
    }\
  /* Do last chunk */\
  if ( count > 0 )\
    { bits = CFETCH_LEFT(bptr, cskew, skew);\
      if ( count > skew ) bits += CFETCH_RIGHT(bptr + chunk_bytes, skew, cskew);\
      wr_op_masked(bits, rmask, 1);\
    }
    
      switch (mode.op) 
      {
				case COPY_OR:
		    	for (;;) 
		    	{
						int count = wleft;

						WRITE_UNALIGNED(WRITE_OR, WRITE_OR_MASKED);
						if (--h == 0)
			    		break;
						END_Y_LOOP(sskip, dskip);
		    	}
		    	break;
		    	
				case COPY_STORE:
		    	for (;;) 
		   		{
						int count = wleft;

						WRITE_UNALIGNED(WRITE_STORE, WRITE_STORE_MASKED);
						if (--h == 0)
			    		break;
						END_Y_LOOP(sskip, dskip);
		    	}
		    	break;
		    
				case COPY_AND:
		    	for (;;) 
		    	{
						int count = wleft;

						WRITE_UNALIGNED(WRITE_AND, WRITE_AND_MASKED);
						if (--h == 0)
			    		break;
						END_Y_LOOP(sskip, dskip);
		    	}
		    	break;
		    	
				default /*case COPY_FUNNY */ :
		    	return FUNNY_CASE();
	    }
#undef WRITE_UNALIGNED
		}
  }
  
#undef END_Y_LOOP
#undef NEXT_X_CHUNK
  return 0;
#undef optr
#undef mono_masks
}

ulong XWPSDeviceMem::dataSize(int widthA, int heightA)
{
	return bitsSize(widthA, heightA) +	linePtrsSize(widthA, heightA);
}

int XWPSDeviceMem::fillRectangleABuf(int x, int y, int w, int h, ulong color)
{
	PSYTransfer yt;

  x -= mapped_x;
  fit_fill_xy(x, y, w, h);
  
  if (w > width - x)
	  w = width - x;
	  
  fit_fill_w(x, w);
  save_color = color;
  y_transfer_init(&yt, y, h);
  while (yt.height_left > 0) 
  {
		y_transfer_next(&yt);
		fillRectangle(x, yt.transfer_y, w, yt.transfer_height, 1);
  }
  return 0;
}

int XWPSDeviceMem::fillRectangleMapped2(int x, int y, int w, int h, ulong color)
{
	fit_fill(x, y, w, h);
  bitsFillRectangle(scanLineBase(y), x << 1, raster, tile_patterns2[color], w << 1, h);
  return 0;
}

int XWPSDeviceMem::fillRectangleMapped4(int x, int y, int w, int h, ulong color)
{
	fit_fill(x, y, w, h);
  bitsFillRectangle(scanLineBase(y), x << 2, raster, tile_patterns4[color], w << 2, h);
  return 0;
}

int XWPSDeviceMem::fillRectangleMapped8(int x, int y, int w, int h, ulong color)
{
	fit_fill(x, y, w, h);
  bytes_fill_rectangle(scanLineBase(y) + x, raster, (uchar) color, w, h);
  return 0;
}

int XWPSDeviceMem::fillRectangleTrue16(int x, int y, int w, int h, ulong color)
{
#if arch_is_big_endian
  const ushort color16 = (ushort)color;
#else
  const ushort color16 = (ushort)((color << 8) | (color >> 8));
#endif

	uchar * dest;
	uint draster;
	fit_fill(x, y, w, h);
	
	draster = raster;
	dest = (uchar*)(scanLineBase(y) + ((x << 1) & -chunk_align_bytes));
	
	if (w == 1) 
	{
		while (h-- > 0) 
		{
	    *(ushort *)dest = color16;
	    dest = dest + draster;
		}
  } 
  else if ((color16 >> 8) == (color16 & 0xff)) 
  {
		bytes_fill_rectangle(scanLineBase(y) + (x << 1), draster, (uchar)color, w << 1, h);
  } 
  else 
  {
		while (h-- > 0) 
		{
	    ushort *pptr = (ushort *) dest;
	    int cnt = w;

	    for (; cnt >= 4; pptr += 4, cnt -= 4)
				pptr[3] = pptr[2] = pptr[1] = pptr[0] = color16;
				
	    switch (cnt) 
	    {
	    	case 3: pptr[2] = color16;
	    	case 2: pptr[1] = color16;
	    	case 1: pptr[0] = color16;
	    	case 0: ;
	    }
	    dest = dest + draster;
		}
  }
  return 0;
}

int XWPSDeviceMem::fillRectangleTrue24(int x, int y, int w, int h, ulong color)
{
	declare_unpack_color(r, g, b, color);
	uchar * dest;
	uint draster;
	fit_fill_xywh(x, y, w, h);
	
	if (w >= 5)
	{
		if (h <= 0)
	    return 0;
	    
	  draster = raster;
	  dest = (uchar*)(scanLineBase(y) + ((x * 3) & -chunk_align_bytes));
	  if (r == g && r == b)
	  {
	  	int bcnt = w * 3;

	    while (h-- > 0) 
	    {
				memset(dest, r, bcnt);
				dest = dest + draster;
	    }
	  }
	  else
	  {
	  	int x3 = -x & 3, ww = w - x3;
	    quint32 rgbr, gbrg, brgb;

	    if (color24.rgb == color) 
	    {
				rgbr = color24.rgbr;
				gbrg = color24.gbrg;
				brgb = color24.brgb;
	    } 
	    else 
	    {
				set_color24_cache(color, r, g, b);
	    }
	    while (h-- > 0)
	    {
	    	uchar *pptr = dest;
				int w1 = ww;

				switch (x3) 
				{
		    	case 1:
						put3(pptr, r, g, b);
						pptr += 3;
						break;
						
		    	case 2:
						pptr[0] = r;
						pptr[1] = g;
						putw(pptr + 2, brgb);
						pptr += 6;
						break;
						
		    	case 3:
						pptr[0] = r;
						putw(pptr + 1, gbrg);
						putw(pptr + 5, brgb);
						pptr += 9;
						break;
						
		    	case 0:
						;
				}
				
				while (w1 >= 4)
				{
					putw(pptr, rgbr);
		    	putw(pptr + 4, gbrg);
		    	putw(pptr + 8, brgb);
		    	pptr += 12;
		    	w1 -= 4;
				}
				
				switch (w1) 
				{
		    	case 1:
						put3(pptr, r, g, b);
						break;
						
		    	case 2:
						putw(pptr, rgbr);
						pptr[4] = g;
						pptr[5] = b;
						break;
						
		    	case 3:
						putw(pptr, rgbr);
						putw(pptr + 4, gbrg);
						pptr[8] = b;
						break;
						
		    	case 0:
						;
				}
				
				dest = dest + draster;
	    }
	  }
	}
	else if (h > 0)
	{
		draster = raster;
	  dest = (uchar*)(scanLineBase(y) + ((x * 3) & -chunk_align_bytes));
	  switch (w) 
	  {
	    case 4:
				do 
				{
		    	dest[9] = dest[6] = dest[3] = dest[0] = r;
		    	dest[10] = dest[7] = dest[4] = dest[1] = g;
		    	dest[11] = dest[8] = dest[5] = dest[2] = b;
		    	dest = dest + draster;
				}	while (--h);
				break;
				
	    case 3:
				do 
				{
		    	dest[6] = dest[3] = dest[0] = r;
		    	dest[7] = dest[4] = dest[1] = g;
		    	dest[8] = dest[5] = dest[2] = b;
		    	dest = dest + draster;
				}	while (--h);
				break;
				
	    case 2:
				do 
				{
		    	dest[3] = dest[0] = r;
		    	dest[4] = dest[1] = g;
		    	dest[5] = dest[2] = b;
		    	dest = dest + draster;
				}	while (--h);
				break;
				
	    case 1:
				do 
				{
		    	dest[0] = r, dest[1] = g, dest[2] = b;
		    	dest = dest + draster;
				}	while (--h);
				break;
				
	    case 0:
	    default:
				;
		}
	}
	
	return 0;
}

int XWPSDeviceMem::fillRectangleTrue32(int x, int y, int w, int h, ulong color)
{
	uchar * dest;
	uint draster;
	quint32 a_color;
	a_color = arrange_bytes(color);
	draster = raster;
	dest = (scanLineBase(y) + ((x << 2) & -chunk_align_bytes));
	if (w <= 4)
		switch (w)
		{
#define dest32 ((quint32 *)dest)
	    case 1:
				do 
				{
		    	dest32[0] = a_color;
		    	dest = dest + draster;
				}	while (--h > 0);
				break;
				
	    case 2:
				do 
				{
		    	dest32[1] = dest32[0] = a_color;
		    	dest = dest + draster;
				}	while (--h > 0);
				break;
				
	    case 3:
				do 
				{
		    	dest32[2] = dest32[1] = dest32[0] = a_color;
		    	dest = dest + draster;
				}	while (--h > 0);
				break;
				
	    case 4:
				do 
				{
		    	dest32[3] = dest32[2] = dest32[1] = dest32[0] = a_color;
		    	dest = dest + draster;
				}	while (--h > 0);
				break;
				
	    default:
				;
		}
	else if (a_color == 0)
		do 
		{
	    memset(dest, 0, w << 2);
	    dest = dest + draster;
		} while (--h > 0);
	else
		do 
		{
	    quint32 *pptr = dest32;
	    int cnt = w;

	    do 
	    {
				pptr[3] = pptr[2] = pptr[1] = pptr[0] = a_color;
				pptr += 4;
	    }   while ((cnt -= 4) > 4);
	    do 
	    {
				*pptr++ = a_color;
	    } while (--cnt > 0);
	    dest = dest + draster;
		}	while (--h > 0);
#undef dest32
  return 0;
}

int XWPSDeviceMem::fillRectangleMono(int x, int y, int w, int h, ulong color)
{
	fit_fill(x, y, w, h);
  bitsFillRectangle(scanLineBase(y), x, raster,	-(uint) color, w, h);
  return 0;
}

int XWPSDeviceMem::getBitsRectangleMem(XWPSIntRect *prect,
	                             XWPSGetBitsParams *params, 
	                             XWPSIntRect **)
{
	ulong options = params->options;
  int x = prect->p.x, w = prect->q.x - x, y = prect->p.y, h = prect->q.y - y;

  if (options == 0) 
  {
		params->options =
	  	  (GB_ALIGN_STANDARD | GB_ALIGN_ANY) |
	    	(GB_RETURN_COPY | GB_RETURN_POINTER) |
	    	(GB_OFFSET_0 | GB_OFFSET_SPECIFIED | GB_OFFSET_ANY) |
	    	(GB_RASTER_STANDARD | GB_RASTER_SPECIFIED | GB_RASTER_ANY) |
	    	GB_PACKING_CHUNKY | GB_COLORS_NATIVE | GB_ALPHA_NONE;
		return (int)(XWPSError::RangeCheck);
  }
  
  if ((w <= 0) | (h <= 0)) 
  {
		if ((w | h) < 0)
	    return (int)(XWPSError::RangeCheck);
		return 0;
  }
  
  if (x < 0 || w > width - x || y < 0 || h > height - y)
		return (int)(XWPSError::RangeCheck);
    
  {
		XWPSGetBitsParams copy_params;
		uchar *baseA = scanLineBase(y);
		int code;

		copy_params.options = GB_COLORS_NATIVE | GB_PACKING_CHUNKY | GB_ALPHA_NONE |
	    										(raster == bitmap_raster(width * color_info.depth) ? GB_RASTER_STANDARD : GB_RASTER_SPECIFIED);
		copy_params.raster = raster;
		code = getBitsReturnPointer(x, h, params, &copy_params, baseA);
		if (code >= 0)
	    return code;
		return getBitsCopy(x, w, h, params, &copy_params, baseA, getRasterMem());
  }
}

void  XWPSDeviceMem::getClippingBoxABuf(XWPSFixedRect * pbox)
{
	target->getClipingBox(pbox);
  pbox->p.x <<= log2_scale.x;
  pbox->p.y <<= log2_scale.y;
  pbox->q.x <<= log2_scale.x;
  pbox->q.y <<= log2_scale.y;
}

void  XWPSDeviceMem::getInitialMatrixMem(XWPSMatrix * pmat)
{
	pmat->xx = initial_matrix.xx;
  pmat->xy = initial_matrix.xy;
  pmat->yx = initial_matrix.yx;
  pmat->yy = initial_matrix.yy;
  pmat->tx = initial_matrix.tx;
  pmat->ty = initial_matrix.ty;
}

uint  XWPSDeviceMem::getRasterMem()
{
	return getRaster(true);
}

int XWPSDeviceMem::initializeWordImageDevice(XWPSMatrix * pmat,
	                             uint widthA, 
	                             uint heightA, 
	                             const uchar * colors, 
	                             int colors_size,
		                           bool word_oriented, 
		                           bool page_device)
{
	int palette_count = colors_size;
  int num_components = 1;
  int pcount;
  int bits_per_pixel;
  float x_pixels_per_unit, y_pixels_per_unit;
  uchar paletteA[256 * 3];
  bool has_color;

  switch (colors_size) 
  {
		case 3 * 2:
	    palette_count = 2;
	    num_components = 3;
		case 2:
	    bits_per_pixel = 1;
	    break;
	    
		case 3 * 4:
	    palette_count = 4;
	    num_components = 3;
		case 4:
	    bits_per_pixel = 2;
	    break;
	    
		case 3 * 16:
	    palette_count = 16;
	    num_components = 3;
		case 16:
	    bits_per_pixel = 4;
	    break;
	    
		case 3 * 256:
	    palette_count = 256;
	    num_components = 3;
		case 256:
	    bits_per_pixel = 8;
	    break;
	    
		case -16:
	    bits_per_pixel = 16;
	    palette_count = 0;
	    break;
	    
		case -24:
	    bits_per_pixel = 24;
	    palette_count = 0;
	    break;
	    
		case -32:
	    bits_per_pixel = 32;
	    palette_count = 0;
	    break;
	    
		default:
	    return (int)(XWPSError::RangeCheck);
  }
  
  if (word_oriented)
  {
  	return (int)(XWPSError::RangeCheck);
  }
  
  pcount = palette_count * 3;
  if (bits_per_pixel <= 8) 
  {
		uchar *p;
		uchar *q;
		int primary_mask = 0;
		int i;

		has_color = false;
		for (i = 0, p = (uchar*)colors, q = paletteA; i < palette_count; i++, q += 3) 
		{
	    int mask = 1;

	    switch (num_components) 
	    {
				case 1:	/* gray */
		    	q[0] = q[1] = q[2] = *p++;
		    	break;
		    	
				default /* case 3 */ :		/* RGB */
		    	q[0] = p[0], q[1] = p[1], q[2] = p[2];
		    	p += 3;
	    }
#define shift_mask(b,n)\
  switch ( b ) { case 0xff: mask <<= n; case 0: break; default: mask = 0; }
	    
	    shift_mask(q[0], 4);
	    shift_mask(q[1], 2);
	    shift_mask(q[2], 1);
	    
#undef shift_mask

	    primary_mask |= mask;
	    if (q[0] != q[1] || q[0] != q[2])
				has_color = true;
		}
		switch (primary_mask) 
		{
	    case 129:	
				if (has_color)
		    	return (int)(XWPSError::RangeCheck);
		    		
	    case 255:	
				break;
				
	    default:
				return (int)(XWPSError::RangeCheck);
		}
  } 
  else
		has_color = true;
		
  if (is_fzero2(pmat->xy, pmat->yx))
		x_pixels_per_unit = pmat->xx, y_pixels_per_unit = pmat->yy;
  else if (is_fzero2(pmat->xx, pmat->yy))
		x_pixels_per_unit = pmat->yx, y_pixels_per_unit = pmat->xy;
  else
		return (int)(XWPSError::UndefinedResult);
	
  if (bits_per_pixel == 1) 
  {
		makeMemDevice(bits_per_pixel,  (page_device ? 1 : -1), 0);
		setInverted((paletteA[0] | paletteA[1] | paletteA[2]) != 0);
  } 
  else 
  {
		uchar *dev_palette = new uchar[pcount]; 
		makeMemDevice(bits_per_pixel,  (page_device ? 1 : -1), 0);
		palette.size = pcount;
		palette.data = dev_palette;
		memcpy(dev_palette, paletteA, pcount);
		own_palette = true;
		if (!has_color) 
		{
	    color_info.num_components = 1;
	    color_info.max_color = 0;
	    color_info.dither_colors = 0;
		}
  }
  
  initial_matrix = *pmat;
  MarginsHWResolution[0] = HWResolution[0] =	fabs(x_pixels_per_unit) * 72;
  MarginsHWResolution[1] = HWResolution[1] = fabs(y_pixels_per_unit) * 72;
  setWidthHeight(widthA, heightA);
  
  {
		XWPSRect bbox;
		bbox.q.x = widthA;
		bbox.q.y = heightA;
		bbox.bboxTransformInverse(&bbox, pmat);
		ImagingBBox[0] = bbox.p.x;
		ImagingBBox[1] = bbox.p.y;
		ImagingBBox[2] = bbox.q.x;
		ImagingBBox[3] = bbox.q.y;
		ImagingBBox_set = true;
  }
  return 0;
}		                           

bool XWPSDeviceMem::isABuf()
{
	return dname == memAlphaBufferName;
}

ulong XWPSDeviceMem::linePtrsSize(int , int heightA)
{
	return (ulong)heightA * sizeof(uchar *) * qMax(num_planes, 1);
}

void XWPSDeviceMem::makeMemABufDevice(XWPSDevice * targetA, 
	                        XWPSLog2ScalePoint * pscale,
													int alpha_bits, 
													int mapped_xA)
{
	memDevice(memAlphaBufferName, 0, 1);
	procs.map_rgb_color_.map_rgb_colorfw = &XWPSDeviceForward::mapRGBColorForward;
	procs.map_color_rgb_.map_color_rgbfw = &XWPSDeviceForward::mapColorRGBForward;
	procs.copy_mono_.copy_monomem = &XWPSDeviceMem::copyMonoABuf;
	procs.copy_color_.copy_color = &XWPSDevice::copyColorDefault;
	procs.fill_rectangle_.fill_rectanglemem = &XWPSDeviceMem::fillRectangleABuf;
	procs.strip_copy_rop_.strip_copy_rop = &XWPSDevice::stripCopyRopNo;
		
	makeMemDevice(0, 0, targetA);
	max_fill_band = 1 << pscale->y;
  log2_scale = *pscale;
  log2_alpha_bits = alpha_bits >> 1;
  mapped_x = mapped_xA;
  procs.close__.closemem = &XWPSDeviceMem::closeABuf;
  procs.get_clipping_box_.get_clipping_boxmem = &XWPSDeviceMem::getClippingBoxABuf;
  color_info.anti_alias.graphics_bits =	alpha_bits;
}

void  XWPSDeviceMem::makeMemAlphaDevice(XWPSDevice * targetA, int alpha_bits)
{
	makeMemDevice(alpha_bits, 0, targetA);
	
	
	color_info.depth = alpha_bits;
	procs.map_rgb_color_.map_rgb_colormem = &XWPSDeviceMem::mapRGBColorAlpha;
	procs.map_color_rgb_.map_color_rgbmem = &XWPSDeviceMem::mapColorRGBAlpha;
	procs.map_rgb_alpha_color_.map_rgb_alpha_colormem = &XWPSDeviceMem::mapRGBAlphaColorAlpha;
	procs.copy_alpha_.copy_alphamem = &XWPSDeviceMem::copyAlphaAlpha;
}

void  XWPSDeviceMem::makeMemDevice(int bits_per_pixel,
		   								int page_device, 
		   								XWPSDevice * targetA)
{
	switch (bits_per_pixel)
	{
		case 1:
			makeMemMonoDevice(targetA);
			break;
			
		case 2:
			makeMemMapped2Device();
			break;
			
		case 4:
			makeMemMapped4Device();
			break;
			
		case 8:
			makeMemMapped8Device();
			break;
			
		case 16:
			makeTrue16Device();
			break;
			
		case 24:
			makeTrue24Device();
			break;
			
		case 32:
			makeTrue32Device();
			break;
			
		default:
			break;
	}
	
	switch (page_device)
	{
		case -1:
			procs.get_page_device_.get_page_device = &XWPSDevice::getPageDeviceDefault;
			break;
			
		case 1:
			procs.get_page_device_.get_page_device = &XWPSDevice::getPageDevicePageDevice;
			break;
			
		default:
			break;
	}
	
	if (targetA == 0)
	{
		if (color_info.depth == 1) 
		{
	    cached_colors.black = 1;
	    cached_colors.white = 0;
		} 
		else 
		{
	    cached_colors.black = 0;
	    cached_colors.white = (1 << color_info.depth) - 1;
		}
	}
	else
	{
		setTarget(targetA);
		copyColorProcs(targetA);
		cached_colors = targetA->cached_colors;
	}
	
	if (color_info.depth == 1)
		setInverted((targetA == 0 || targetA->mapRGBColor(0, 0, 0) != 0));
}

void  XWPSDeviceMem::makeMemMapped2Device()
{
	memDevice(memMapped2Name, 2, 0);
	
	procs.map_rgb_color_.map_rgb_colormem = &XWPSDeviceMem::mapRGBColorMapped;
	procs.map_color_rgb_.map_color_rgbmem = &XWPSDeviceMem::mapColorRGBMapped;
	procs.fill_rectangle_.fill_rectanglemem = &XWPSDeviceMem::fillRectangleMapped2;
	procs.copy_mono_.copy_monomem = &XWPSDeviceMem::copyMonoMapped2;
	procs.copy_color_.copy_colormem = &XWPSDeviceMem::copyColorMapped2;
	procs.strip_copy_rop_.strip_copy_rop = &XWPSDevice::stripCopyRopNo;
}

void  XWPSDeviceMem::makeMemMapped4Device()
{
	memDevice(memMapped2Name, 4, 0);
	
	procs.map_rgb_color_.map_rgb_colormem = &XWPSDeviceMem::mapRGBColorMapped;
	procs.map_color_rgb_.map_color_rgbmem = &XWPSDeviceMem::mapColorRGBMapped;
	procs.fill_rectangle_.fill_rectanglemem = &XWPSDeviceMem::fillRectangleMapped4;
	procs.copy_mono_.copy_monomem = &XWPSDeviceMem::copyMonoMapped4;
	procs.copy_color_.copy_colormem = &XWPSDeviceMem::copyColorMapped4;
	procs.strip_copy_rop_.strip_copy_rop = &XWPSDevice::stripCopyRopNo;
}

void  XWPSDeviceMem::makeMemMapped8Device()
{
	memDevice(memMapped8Name, 8, 0);
	procs.map_rgb_color_.map_rgb_colormem = &XWPSDeviceMem::mapRGBColorMapped;
	procs.map_color_rgb_.map_color_rgbmem = &XWPSDeviceMem::mapColorRGBMapped;
	procs.fill_rectangle_.fill_rectanglemem = &XWPSDeviceMem::fillRectangleMapped8;
	procs.copy_mono_.copy_monomem = &XWPSDeviceMem::copyMonoMapped8;
	procs.copy_color_.copy_colormem = &XWPSDeviceMem::copyColorMapped8;
	procs.strip_copy_rop_.strip_copy_ropmem = &XWPSDeviceMem::stripCopyRopGray8RGB24;
}

void  XWPSDeviceMem::makeMemMonoDevice(XWPSDevice * targetA)
{
	procs.map_rgb_color_.map_rgb_colormem = &XWPSDeviceMem::mapRGBColorMono;
	procs.map_color_rgb_.map_color_rgbmem = &XWPSDeviceMem::mapColorRGBMono;
	procs.fill_rectangle_.fill_rectanglemem = &XWPSDeviceMem::fillRectangleMono;
	procs.copy_mono_.copy_monomem = &XWPSDeviceMem::copyMonoMono;
	procs.copy_color_.copy_color = &XWPSDevice::copyColorDefault;
	procs.map_cmyk_color_.map_cmyk_color = &XWPSDevice::mapCMYKColorDefault;
	procs.copy_alpha_.copy_alpha = &XWPSDevice::copyAlphaNo;
	procs.strip_tile_rectangle_.strip_tile_rectanglemem = &XWPSDeviceMem::stripTileRectangleMono;
	procs.strip_copy_rop_.strip_copy_ropmem = &XWPSDeviceMem::stripCopyRopMono;
	procs.get_bits_rectangle_.get_bits_rectanglemem = &XWPSDeviceMem::getBitsRectangleMem;
		
	makeFullAlphaDevice(memMonoName, 0, 1);
	
	setTarget(targetA);
	setInverted(true);
}

void  XWPSDeviceMem::makeTrue16Device()
{
	memDevice(memTrue16Name, 16, 0);
	procs.map_rgb_color_.map_rgb_colormem = &XWPSDeviceMem::mapRGBColorTrue16;
	procs.map_color_rgb_.map_color_rgbmem = &XWPSDeviceMem::mapColorRGBTrue16;
	procs.fill_rectangle_.fill_rectanglemem = &XWPSDeviceMem::fillRectangleTrue16;
	procs.copy_mono_.copy_monomem = &XWPSDeviceMem::copyMonoTrue16;
	procs.copy_color_.copy_colormem = &XWPSDeviceMem::copyColorTrue16;
	procs.strip_copy_rop_.strip_copy_rop = &XWPSDevice::stripCopyRopNo;
}

void  XWPSDeviceMem::makeTrue24Device()
{
	makeFullAlphaDevice(memTrue24Name, 24, 0);
	
	procs.map_rgb_color_.map_rgb_color = &XWPSDevice::mapRGBColorRGBDefault;
	procs.map_color_rgb_.map_color_rgb = &XWPSDevice::mapColorRGBRGBDefault;
	procs.fill_rectangle_.fill_rectanglemem = &XWPSDeviceMem::fillRectangleTrue24;
	procs.map_cmyk_color_.map_cmyk_color = &XWPSDevice::mapCMYKColorDefault;
	procs.copy_mono_.copy_monomem = &XWPSDeviceMem::copyMonoTrue24;
	procs.copy_color_.copy_colormem = &XWPSDeviceMem::copyColorTrue24;
	procs.copy_alpha_.copy_alphamem = &XWPSDeviceMem::copyAlphaTrue24;
	procs.strip_tile_rectangle_.strip_tile_rectangle = &XWPSDevice::stripTileRectangleDefault;
	procs.strip_copy_rop_.strip_copy_ropmem = &XWPSDeviceMem::stripCopyRopGray8RGB24;
	procs.get_bits_rectangle_.get_bits_rectanglemem = &XWPSDeviceMem::getBitsRectangleMem;
}

void  XWPSDeviceMem::makeTrue32Device()
{
	makeFullDevice(memTrue32Name, 24, 8);
	
	procs.map_rgb_color_.map_rgb_color = &XWPSDevice::mapRGBColorRGBDefault;
	procs.map_color_rgb_.map_color_rgb = &XWPSDevice::mapColorRGBRGBDefault;
	procs.fill_rectangle_.fill_rectanglemem = &XWPSDeviceMem::fillRectangleTrue32;
	procs.copy_mono_.copy_monomem = &XWPSDeviceMem::copyMonoTrue32;
	procs.copy_color_.copy_colormem = &XWPSDeviceMem::copyColorTrue32;
	procs.map_cmyk_color_.map_cmyk_color = &XWPSDevice::mapCMYKColorDefault;
	procs.strip_tile_rectangle_.strip_tile_rectangle = &XWPSDevice::stripTileRectangleDefault;
	procs.strip_copy_rop_.strip_copy_rop = &XWPSDevice::stripCopyRopNo;
	procs.get_bits_rectangle_.get_bits_rectanglemem = &XWPSDeviceMem::getBitsRectangleMem;
}

int XWPSDeviceMem::mapColorRGBAlpha(ulong color, ushort* prgb)
{
	return	XWPSDeviceForward::mapColorRGB((color == 0 ? color : 1), prgb);
}

int XWPSDeviceMem::mapColorRGBMapped(ulong color, ushort* prgb)
{
	uchar *pptr = (uchar*)(palette.data + (int)color * 3);

  prgb[0] = ps_color_value_from_byte(pptr[0]);
  prgb[1] = ps_color_value_from_byte(pptr[1]);
  prgb[2] = ps_color_value_from_byte(pptr[2]);
  return 0;
}

int XWPSDeviceMem::mapColorRGBMono(ulong color, ushort* prgb)
{
	return mapColorRGBDefault((color ^ palette.data[0]) & 1,	prgb);
}

int XWPSDeviceMem::mapColorRGBTrue16(ulong color, ushort* prgb)
{
	ushort value = color >> 11;

  prgb[0] = ((value << 11) + (value << 6) + (value << 1) + (value >> 4)) >> (16 - ps_color_value_bits);
  value = (color >> 5) & 0x3f;
  prgb[1] = ((value << 10) + (value << 4) + (value >> 2)) >> (16 - ps_color_value_bits);
  value = color & 0x1f;
  prgb[2] = ((value << 11) + (value << 6) + (value << 1) + (value >> 4)) >> (16 - ps_color_value_bits);
  return 0;
}

ulong XWPSDeviceMem::mapRGBAlphaColorAlpha(ushort r, ushort g, ushort b, ushort alpha)
{
	ulong color = XWPSDeviceForward::mapRGBColor(r, g, b);

  return (color == 0 || color == ps_no_color_index ? color : (ulong) (alpha >> (ps_color_value_bits -	log2_alpha_bits)));
}

ulong XWPSDeviceMem::mapRGBColorAlpha(ushort r, ushort g, ushort b)
{
	ulong color = XWPSDeviceForward::mapRGBColor(r, g, b);

  return (color == 0 || color == ps_no_color_index ? color : (ulong) ((1 << log2_alpha_bits) - 1));
}

ulong XWPSDeviceMem::mapRGBColorMapped(ushort r, ushort g, ushort b)
{
	uchar br = ps_color_value_to_byte(r);
  uchar bg = ps_color_value_to_byte(g);
  uchar bb = ps_color_value_to_byte(b);
  uchar *pptr = (uchar*)(palette.data);
  int cnt = palette.size;
  uchar *which = 0;
  int best = 256 * 3;

  while ((cnt -= 3) >= 0) 
  {
		int diff = *pptr - br;

		if (diff < 0)
	    diff = -diff;
		if (diff < best) 
		{
	    int dg = pptr[1] - bg;

	    if (dg < 0)
				dg = -dg;
	    if ((diff += dg) < best) 
	    {
				int db = pptr[2] - bb;

				if (db < 0)
		    	db = -db;
				if ((diff += db) < best)
		    	which = pptr, best = diff;
	    }
		}
		pptr += 3;
  }
  return (ulong) ((which - palette.data) / 3);
}

ulong XWPSDeviceMem::mapRGBColorMono(ushort r, ushort g, ushort b)
{
	return (mapRGBColorDefault(r, g, b) ^ palette.data[0]) & 1;
}

ulong XWPSDeviceMem::mapRGBColorTrue16(ushort r, ushort g, ushort b)
{
	return ((r >> (ps_color_value_bits - 5)) << 11) +
					((g >> (ps_color_value_bits - 6)) << 5) +
					(b >> (ps_color_value_bits - 5));
}

int XWPSDeviceMem::openMem()
{
	if (num_planes)
		return (int)(XWPSError::RangeCheck);
  return openScanLines(height);
}

void XWPSDeviceMem::openCacheDevice(XWPSCachedChar * cc)
{
	 uchar *bits = cc_bits(cc);

   width = cc->width;
   height = cc->height;
   memset((char *)bits, 0, (uint)dataSize(width, height));
   base = bits;
   open();
}

int XWPSDeviceMem::openScanLines(int setup_height)
{
	if (setup_height < 0 || setup_height > height)
		return (int)(XWPSError::RangeCheck);
			
	ulong size = bitmapSize();
	if (!base)
	{
		base = new uchar[(uint)size];
		foreign_bits = false;
	}
	line_ptrs = (uchar **)(base + bitsSize(width, height));
  raster = getRasterMem();
  return setLinePtrs(NULL, 0, NULL, setup_height);
}

void XWPSDeviceMem::setInverted(bool black_is_1)
{
	if (black_is_1)
		palette = mem_mono_b_w_palette;
  else
		palette = mem_mono_w_b_palette;
	own_palette = false;
}

int XWPSDeviceMem::setLinePtrs(uchar * baseA, 
	                        int rasterA, 
	                        uchar **line_ptrsA, 
	                        int setup_height)
{
	int num_planesA = num_planes;
	XWPSRenderPlane plane1;
  const XWPSRenderPlane *planesA;
  uchar **pline =	(line_ptrsA ? (line_ptrs = line_ptrsA) : line_ptrs);
  uchar *data =	(baseA ? (raster = rasterA, base = baseA) : (rasterA = raster, base));
  int pi;

  if (num_planesA) 
  {
		if (baseA && !plane_depth)
	    return (int)(XWPSError::RangeCheck);
		planesA = planes;
  } 
  else 
  {
		planesA = &plane1;
		plane1.depth = color_info.depth;
		num_planesA = 1;
  }

  for (pi = 0; pi < num_planesA; ++pi) 
  {
		int rasterB = bitmap_raster(width * planesA[pi].depth);
		uchar **pptr = pline;
		uchar **pend = pptr + setup_height;
		uchar *scan_line = data;

		while (pptr < pend) 
		{
	    *pptr++ = scan_line;
	    scan_line += rasterB;
		}
		data += rasterB * height;
		pline += setup_height;
  }
  return 0;
}

#define x_offset(px, ty, textures)\
  ((textures)->shift == 0 ? (px) :\
   (px) + (ty) / (textures)->rep_height * (textures)->rep_shift)
   
int XWPSDeviceMem::stripCopyRopMono(const uchar *sdata, 
	                         int sourcex, 
	                         uint sraster, 
	                         ulong id,
	                         const ulong *scolors,
	                         XWPSStripBitmap *textures, 
	                         const ulong *tcolors,
	                         int x, 
	                         int y, 
	                         int widthA, 
	                         int heightA,
	                         int phase_x, 
	                         int phase_y, 
	                         ulong lop)
{
	int rop = ps_transparent_rop(lop);
	XWPSStripBitmap no_texture;
  bool invert;
  uint draster = raster;
  uint traster;
  int line_count;
  uchar *drow;
  const uchar *srow;
  int ty;
  
  if (palette.data == 0)
	  setInverted(mapRGBColor(0, 0, 0) != 0);
    
  invert = palette.data[0] != 0;
  if (invert)
		rop = byte_reverse_bits[rop] ^ 0xff;
		
	if (scolors != 0)
	{
		switch ((int)((scolors[1] << 1) + scolors[0])) 
		{
	    case 0:
				rop = rop3_know_S_0(rop);
				break;
				
	    case 1:
				rop = rop3_invert_S(rop);
				break;
				
	    case 2:
				break;
				
	    case 3:
				rop = rop3_know_S_1(rop);
				break;
				
		}
	}
	
	if (tcolors != 0) 
	{
		switch ((int)((tcolors[1] << 1) + tcolors[0])) 
		{
	    case 0:
				rop = rop3_know_T_0(rop);
				break;
				
	    case 1:
				rop = rop3_invert_T(rop);
				break;
				
	    case 2:
				break;
				
	    case 3:
				rop = rop3_know_T_1(rop);
				break;
		}
	}
	
	{
		ulong color0, color1;
		switch (rop_usage_table[rop])
		{
			case rop_usage_none:
				return fillRectangle(x, y, widthA, heightA, (ulong) (rop & 1));
				
			case rop_usage_D:
				if (rop == rop3_D)
		    	return 0;
		    	
		    fit_fill(x, y, widthA, heightA);
				sdata = scanLineBase(0);
				sourcex = x;
				sraster = 0;
				goto no_T;
				
			case rop_usage_S:
				if (rop == rop3_S)
		    	color0 = 0, color1 = 1;
				else
		    	color0 = 1, color1 = 0;
	      
do_copy:
				return copyMono(sdata, sourcex, sraster, id, x, y, widthA, heightA, color0, color1);
				
			case rop_usage_DS:
#define copy_case(c0, c1) color0 = c0, color1 = c1; goto do_copy;

				switch ((uint) rop)
				{
					case rop3_D & rop3_not(rop3_S):
						copy_case(ps_no_color_index, 0);
		    	case rop3_D | rop3_S:
						copy_case(ps_no_color_index, 1);
		    	case rop3_D & rop3_S:
						copy_case(0, ps_no_color_index);
		    	case rop3_D | rop3_not(rop3_S):
						copy_case(1, ps_no_color_index);
		    	default:;
				}
#undef copy_case

				fit_copy(sdata, sourcex, sraster, id, x, y, widthA, heightA);
				
no_T:
				no_texture.data = scanLineBase(0);
				no_texture.raster = 0;
				no_texture.size.x = widthA;
				no_texture.size.y = heightA;
				no_texture.rep_width = no_texture.rep_height = 1;
				no_texture.rep_shift = no_texture.shift = 0;
				textures = &no_texture;
				break;
				
			case rop_usage_T:
				if (rop == rop3_T)
		    	color0 = 0, color1 = 1;
				else
		    	color0 = 1, color1 = 0;
	      
do_tile:
				return stripTileRectangle(textures, x, y, widthA, heightA, color0, color1, phase_x, phase_y);
				
			case rop_usage_DT:
#define tile_case(c0, c1) color0 = c0, color1 = c1; goto do_tile;

        switch ((uint) rop)
        {
        	case rop3_D & rop3_not(rop3_T):
						tile_case(ps_no_color_index, 0);
		    	case rop3_D | rop3_T:
						tile_case(ps_no_color_index, 1);
		    	case rop3_D & rop3_T:
						tile_case(0, ps_no_color_index);
		    	case rop3_D | rop3_not(rop3_T):
						tile_case(1, ps_no_color_index);
		    	default:;
        }
        
#undef tile_case
				fit_fill(x, y, widthA, heightA);
				sdata = base;
				sourcex = x;
				sraster = 0;
				break;
				
	    default:
				fit_copy(sdata, sourcex, sraster, id, x, y, widthA, heightA);
		}
	}
	
	line_count = heightA;
  srow = sdata;
  drow = scanLineBase(y);
  traster = textures->raster;
  ty = y + phase_y;
  
  for (; line_count-- > 0; drow += draster, srow += sraster, ++ty)
  {
  	int sx = sourcex;
		int dx = x;
		int w = width;
		const uchar *trow =
		textures->data + (ty % textures->rep_height) * traster;
		int xoff = x_offset(phase_x, ty, textures);
		int nw;
		
		for (; w > 0; sx += nw, dx += nw, w -= nw)
		{
			int dbit = dx & 7;
	    int sbit = sx & 7;
	    int sskew = sbit - dbit;
	    int tx = (dx + xoff) % textures->rep_width;
	    int tbit = tx & 7;
	    int tskew = tbit - dbit;
	    int left = nw = qMin(w, (int)(textures->size.x - tx));
	    uchar lmask = 0xff >> dbit;
	    uchar rmask = 0xff << (~(dbit + nw - 1) & 7);
	    uchar mask = lmask;
	    int nx = 8 - dbit;
	    uchar *dptr = drow + (dx >> 3);
	    const uchar *sptr = srow + (sx >> 3);
	    const uchar *tptr = trow + (tx >> 3);
	    
	    if (sskew < 0)
				--sptr, sskew += 8;
	    if (tskew < 0)
				--tptr, tskew += 8;
				
			for (; left > 0; left -= nx, mask = 0xff, nx = 8, ++dptr, ++sptr, ++tptr)
			{
				uchar dbyte = *dptr;

#define fetch1(ptr, skew)\
  (skew ? (ptr[0] << skew) + (ptr[1] >> (8 - skew)) : *ptr)
		
				uchar sbyte = fetch1(sptr, sskew);
				uchar tbyte = fetch1(tptr, tskew);

#undef fetch1
				uchar result =(*rop_proc_table[rop]) (dbyte, sbyte, tbyte);

				if (left <= nx)
		    	mask &= rmask;
				*dptr = (mask == 0xff ? result : (result & mask) | (dbyte & ~mask));
			}
		}
  }
  
  return 0;
}

int XWPSDeviceMem::stripCopyRopGray8RGB24(const uchar *sdata, 
	                         int sourcex, 
	                         uint sraster, 
	                         ulong id,
	                         const ulong *scolors,
	                         XWPSStripBitmap *textures, 
	                         const ulong *tcolors,
	                         int x, 
	                         int y, 
	                         int w, 
	                         int h,
	                         int phase_x, 
	                         int phase_y, 
	                         ulong lop)
{
#define x_offset(px, ty, textures)\
  ((textures)->shift == 0 ? (px) :\
   (px) + (ty) / (textures)->rep_height * (textures)->rep_shift)
   
	int rop = lop_rop(lop);
  ulong const_source = ps_no_color_index;
  ulong const_texture = ps_no_color_index;
  uint draster = raster;
  int line_count;
  uchar *drow;
  int depth = color_info.depth;
  int bpp = depth >> 3;
  ulong all_ones = ((ulong) 1 << depth) - 1;
  ulong strans =	(lop & lop_S_transparent ? all_ones : ps_no_color_index);
  ulong ttrans =(lop & lop_T_transparent ? all_ones : ps_no_color_index);
  if (!rop3_uses_S(rop))
		const_source = 0;
	else if (scolors != 0 && scolors[0] == scolors[1])
	{
		const_source = scolors[0];
		if (const_source == getBlack())
	    rop = rop3_know_S_0(rop);
		else if (const_source == getWhite())
	    rop = rop3_know_S_1(rop);
	}
	
	if (!rop3_uses_T(rop))
		const_texture = 0;
	else if (tcolors != 0 && tcolors[0] == tcolors[1])
	{
		const_texture = tcolors[0];
		if (const_texture == getBlack())
	    rop = rop3_know_T_0(rop);
		else if (const_texture == getWhite())
	    rop = rop3_know_T_1(rop);
	}
	
	if (bpp == 1 &&	(hasColor() || getBlack() != 0 || getWhite() != all_ones))
	{
		ulong bw_pixel;

		switch (rop) 
		{
			case rop3_0:
	    	bw_pixel = getBlack();
	    	goto bw;
	    	
			case rop3_1:
	    	bw_pixel = getWhite();
	    	
bw:	    if (bw_pixel == 0x00)
					rop = rop3_0;
	    	else if (bw_pixel == 0xff)
					rop = rop3_1;
	    	else
					goto df;
	    	break;
	    	
			case rop3_D:
	    	break;
	    	
			case rop3_S:
	    	if (lop & lop_S_transparent)
					goto df;
	    	break;
	    	
			case rop3_T:
	    	if (lop & lop_T_transparent)
					goto df;
	    	break;
	    	
			default:
				
df:	    
				return stripCopyRopMono(sdata, sourcex, sraster, id, scolors, 
				                        textures, tcolors, x, y, w, h,
				                        phase_x, phase_y, lop);
		}
	}
	
	if (const_source == ps_no_color_index) 
	{
		fit_copy(sdata, sourcex, sraster, id,	 x, y, w, h);
  } 
  else 
  {
		fit_fill(x, y, w, h);
  }
  
  line_count = height;
  drow = scanLineBase(y) + x * bpp;
  
#define bits32 quint32
  
#define dbit(base, i) ((base)[(i) >> 3] & (0x80 >> ((i) & 7)))
/* 8-bit */
#define cbit8(base, i, colors)\
  (dbit(base, i) ? (uchar)colors[1] : (uchar)colors[0])
#define rop_body_8(s_pixel, t_pixel)\
  if ( (s_pixel) == strans ||	/* So = 0, s_tr = 1 */\
       (t_pixel) == ttrans	/* Po = 0, p_tr = 1 */\
     )\
    continue;\
  *dptr = (*rop_proc_table[rop])(*dptr, s_pixel, t_pixel)
/* 24-bit */
#define get24(ptr)\
  (((ulong)(ptr)[0] << 16) | ((ulong)(ptr)[1] << 8) | (ptr)[2])
#define put24(ptr, pixel)\
  (ptr)[0] = (uchar)((pixel) >> 16),\
  (ptr)[1] = (uchar)((uint)(pixel) >> 8),\
  (ptr)[2] = (uchar)(pixel)
#define cbit24(base, i, colors)\
  (dbit(base, i) ? colors[1] : colors[0])
#define rop_body_24(s_pixel, t_pixel)\
  if ( (s_pixel) == strans ||	/* So = 0, s_tr = 1 */\
       (t_pixel) == ttrans	/* Po = 0, p_tr = 1 */\
     )\
    continue;\
  { ulong d_pixel = get24(dptr);\
    d_pixel = (*rop_proc_table[rop])(d_pixel, s_pixel, t_pixel);\
    put24(dptr, d_pixel);\
  }
  
  if (const_texture != ps_no_color_index)
  {
  	if (const_source != ps_no_color_index)
  	{
  		for (; line_count-- > 0; drow += draster) 
  		{
				uchar *dptr = drow;
				int left = width;

				if (bpp == 1)
		    	for (; left > 0; ++dptr, --left) 
		    	{
						rop_body_8((uchar)const_source, (uchar)const_texture);
		    	}
				else
		    	for (; left > 0; dptr += 3, --left) 
		    	{
						rop_body_24(const_source, const_texture);
		    	}
	    }
  	}
  	else
  	{
  		const uchar *srow = sdata;

	    for (; line_count-- > 0; drow += draster, srow += sraster) 
	    {
				uchar *dptr = drow;
				int left = width;

				if (scolors) 
				{
		    	int sx = sourcex;

		    	if (bpp == 1)
						for (; left > 0; ++dptr, ++sx, --left) 
						{
			    		uchar s_pixel = cbit8(srow, sx, scolors);

			    		rop_body_8(s_pixel, (uchar)const_texture);
						}
		    	else
						for (; left > 0; dptr += 3, ++sx, --left) 
						{
			    		bits32 s_pixel = cbit24(srow, sx, scolors);

			    		rop_body_24(s_pixel, const_texture);
						}
				} 
				else if (bpp == 1) 
				{
		    	const uchar *sptr = srow + sourcex;

		    	for (; left > 0; ++dptr, ++sptr, --left) 
		    	{
						uchar s_pixel = *sptr;

						rop_body_8(s_pixel, (uchar)const_texture);
		    	}
				} 
				else 
				{
		    	const uchar *sptr = srow + sourcex * 3;

		    	for (; left > 0; dptr += 3, sptr += 3, --left) 
		    	{
						bits32 s_pixel = get24(sptr);

						rop_body_24(s_pixel, const_texture);
		    	}
				}
	    }
  	}
  }
  else if (const_source != ps_no_color_index)
  {
  	uint traster = textures->raster;
		int ty = y + phase_y;
		for (; line_count-- > 0; drow += draster, ++ty)
		{
			int dx = x, w = width, nw;
	    uchar *dptr = drow;
	    const uchar *trow =  textures->data + (ty % textures->size.y) * traster;
	    int xoff = x_offset(phase_x, ty, textures);

	    for (; w > 0; dx += nw, w -= nw)
	    {
	    	int tx = (dx + xoff) % textures->rep_width;
				int left = nw = qMin(w, (int)(textures->size.x - tx));
				const uchar *tptr = trow;
				if (tcolors)
				{
					if (bpp == 1)
						for (; left > 0; ++dptr, ++tx, --left) 
						{
			    		uchar t_pixel = cbit8(tptr, tx, tcolors);

			    		rop_body_8((uchar)const_source, t_pixel);
						}
		    	else
						for (; left > 0; dptr += 3, ++tx, --left) 
						{
			    		bits32 t_pixel = cbit24(tptr, tx, tcolors);

			    		rop_body_24(const_source, t_pixel);
						}
				}
				else if (bpp == 1)
				{
					tptr += tx;
		    	for (; left > 0; ++dptr, ++tptr, --left) 
		    	{
						uchar t_pixel = *tptr;

						rop_body_8((uchar)const_source, t_pixel);
		    	}
				}
				else
				{
					tptr += tx * 3;
		    	for (; left > 0; dptr += 3, tptr += 3, --left) 
		    	{
						bits32 t_pixel = get24(tptr);

						rop_body_24(const_source, t_pixel);
		    	}
				}
	    }
		}
  }
  else
  {
  	uint traster = textures->raster;
		int ty = y + phase_y;
		const uchar *srow = sdata;
		
		for (; line_count-- > 0; drow += draster, srow += sraster, ++ty)
		{
			int sx = sourcex;
	    int dx = x;
	    int w = width;
	    int nw;
	    uchar *dptr = drow;
	    const uchar *trow = textures->data + (ty % textures->size.y) * traster;
	    int xoff = x_offset(phase_x, ty, textures);

	    for (; w > 0; dx += nw, w -= nw)
	    {
	    	int tx = (dx + xoff) % textures->rep_width;
				int left = nw = qMin(w, textures->size.x - tx);
				const uchar *tptr = trow;
				if (bpp == 1)
				{
					const uchar *sptr = srow + sx;

		    	tptr += tx;
		    	for (; left > 0; ++dptr, ++sptr, ++tptr, ++sx, ++tx, --left) 
		    	{
						uchar s_pixel = (scolors ? cbit8(srow, sx, scolors) : *sptr);
						uchar t_pixel = (tcolors ? cbit8(tptr, tx, tcolors) : *tptr);

						rop_body_8(s_pixel, t_pixel);
		    	}
				}
				else
				{
					const uchar *sptr = srow + sx * 3;

		    	tptr += tx * 3;
		    	for (; left > 0; dptr += 3, sptr += 3, tptr += 3, ++sx, ++tx, --left) 
		    	{
						bits32 s_pixel =  (scolors ? cbit24(srow, sx, scolors) :  get24(sptr));
						bits32 t_pixel = (tcolors ? cbit24(tptr, tx, tcolors) :  get24(tptr));

						rop_body_24(s_pixel, t_pixel);
		    	}
				}
	    }
		}
  }
	
#undef rop_body_8
#undef rop_body_24
#undef dbit
#undef cbit8
#undef cbit24
#undef x_offset
#undef bits32
	return 0;
}

int XWPSDeviceMem::stripTileRectangleMono(XWPSStripBitmap * tiles,
			      										 int tx, 
			      										 int y, 
			      										 int tw, 
			      										 int th,
			      										 ulong color0, 
			      										 ulong color1,
			      										 int px, 
			      										 int py)
{
	uint invert;
  int source_raster;
  uint tile_bits_size;
  const uchar *source_data;
  const uchar *end;
  int x, rw, w, h;
  const uchar *bptr;
  int dbit, wleft;
  uint mask;
  uchar *dbase;
  
  uchar * dbptr;
  uint dest_raster;
#define mono_masks mono_copy_masks
#define optr ((uint *)dbptr)
  
  int skew;
  
	if (color0 != (color1 ^ 1) || tiles->shift != 0)
		return XWPSDevice::stripTileRectangle(tiles, tx, y, tw, th, color0, color1, px, py);
    
  fit_fill(tx, y, tw, th);
  invert = -(uint) color0;
  source_raster = tiles->raster;
  source_data = tiles->data + ((y + py) % tiles->rep_height) * source_raster;
  tile_bits_size = tiles->size.y * source_raster;
  end = tiles->data + tile_bits_size;
#undef END_Y_LOOP
#define END_Y_LOOP(sdelta, ddelta)\
  if ( end - bptr <= sdelta )\
    bptr -= tile_bits_size;\
  bptr += sdelta; dbptr += ddelta
  
  dest_raster = raster;
  dbase = scanLineBase(y);
  x = tx;
  rw = tw;
  
  {
		int source_x = (x + px) % tiles->rep_width;

		w = tiles->size.x - source_x;
		bptr = source_data + ((source_x & ~chunk_align_bit_mask) >> 3);
		dbit = x & chunk_align_bit_mask;
		skew = dbit - (source_x & chunk_align_bit_mask);
  }
  
outer:
	
	if (w > rw)
		w = rw;
  h = th;
  dbptr = dbase + ((x >> 3) & -chunk_align_bytes);
  if ((wleft = w + dbit - chunk_bits) <= 0)
  {
  	set_mono_thin_mask(mask, w, dbit);
#define WRITE1_LOOP(src)\
  for ( ; ; )\
   { WRITE_STORE_MASKED(src, mask, 0);\
     if ( --h == 0 ) break;\
     END_Y_LOOP(source_raster, dest_raster);\
   }
   
    if (skew >= 0)
    {
    	if (skew == 0)
    	{
    		WRITE1_LOOP(CFETCH_ALIGNED(bptr));
    	}
    	else
    	{
#if CFETCH_USES_CSKEW
				int cskew = chunk_bits - skew;
#endif

				WRITE1_LOOP(CFETCH_RIGHT(bptr, skew, cskew));
    	}
    }
    else if (wleft <= skew)
    {
#if CFETCH_USES_CSKEW
	    int cskew = chunk_bits + skew;
#endif

	    skew = -skew;
	    WRITE1_LOOP(CFETCH_LEFT(bptr, skew, cskew));
    }
    else
    {
    	int cskew = -skew;

	    skew += chunk_bits;
	    WRITE1_LOOP(CFETCH2(bptr, cskew, skew));
    }
  }
  else if (wleft <= skew)
  {
  	uint rmask;
		int cskew = chunk_bits - skew;

		set_mono_left_mask(mask, dbit);
		set_mono_right_mask(rmask, wleft);
#if arch_is_big_endian	
#undef CINVERT
#define CINVERT(bits) (bits)	

		for (;;) 
		{
	    uint bits = CFETCH_ALIGNED(bptr) ^ invert;

	    WRITE_STORE_MASKED(bits >> skew, mask, 0);
	    WRITE_STORE_MASKED(bits << cskew, rmask, 1);
	    if (--h == 0)
				break;
	    END_Y_LOOP(source_raster, dest_raster);
		}
#undef CINVERT
#define CINVERT(bits) ((bits) ^ invert)
#else 

		for (;;) 
		{
	    WRITE_STORE_MASKED(CFETCH_RIGHT(bptr, skew, cskew), mask, 0);
	    WRITE_STORE_MASKED(CFETCH_LEFT(bptr, cskew, skew), rmask, 1);
	    if (--h == 0)
				break;
	    END_Y_LOOP(source_raster, dest_raster);
		}
#endif
  }
  else
  {
  	uint rmask;
		int words = (wleft & ~chunk_bit_mask) >> 3;
		uint sskip = source_raster - words;
		uint dskip = dest_raster - words;
		register uint bits;

#define NEXT_X_CHUNK()\
  bptr += chunk_bytes; dbptr += chunk_bytes

		set_mono_right_mask(rmask, wleft & chunk_bit_mask);
		if (skew == 0) 
		{
	    if (dbit == 0)
				mask = 0;
	    else
				set_mono_left_mask(mask, dbit);
	    for (;;) 
	   	{
				int count = wleft;

				if (mask)
		    	WRITE_STORE_MASKED(CFETCH_ALIGNED(bptr), mask, 0);
				else
		    	WRITE_STORE(CFETCH_ALIGNED(bptr));
		    	
				while ((count -= chunk_bits) >= 0) 
				{
		    	NEXT_X_CHUNK();
		    	WRITE_STORE(CFETCH_ALIGNED(bptr));
				}
				
				if (count > -chunk_bits) 
				{
		    	WRITE_STORE_MASKED(CFETCH_ALIGNED(bptr + chunk_bytes), rmask, 1);
				}
				if (--h == 0)
		    	break;
				END_Y_LOOP(sskip, dskip);
	    }
		} 
		else 
		{
	    bool case_right = (skew >= 0 ? true : ((bptr += chunk_bytes), false));
	    int cskew = -skew & chunk_bit_mask;

	    skew &= chunk_bit_mask;
	    set_mono_left_mask(mask, dbit);
	    for (;;) 
	    {
				int count = wleft;

				if (case_right)
		    	bits = CFETCH_RIGHT(bptr, skew, cskew);
				else
		    	bits = CFETCH2(bptr - chunk_bytes, cskew, skew);
				WRITE_STORE_MASKED(bits, mask, 0);
				
				while (count >= chunk_bits) 
				{
		    	bits = CFETCH2(bptr, cskew, skew);
		    	NEXT_X_CHUNK();
		    	WRITE_STORE(bits);
		    	count -= chunk_bits;
				}
				
				if (count > 0) 
				{
		    	bits = CFETCH_LEFT(bptr, cskew, skew);
		    	if (count > skew)
						bits += CFETCH_RIGHT(bptr + chunk_bytes, skew, cskew);
		    	WRITE_STORE_MASKED(bits, rmask, 1);
				}
				if (--h == 0)
		    	break;
				END_Y_LOOP(sskip, dskip);
	    }
		}
  }
  
#undef END_Y_LOOP
#undef NEXT_X_CHUNK
#undef optr
#undef mono_masks
	if ((rw -= w) > 0) 
	{
		x += w;
		w = tiles->size.x;
		bptr = source_data;
		skew = dbit = x & chunk_align_bit_mask;
		goto outer;
  }
  return 0;
}

int  XWPSDeviceMem::abuf_flush_block(int y)
{
	int block_height = 1 << log2_scale.y;
  int alpha_bits = 1 << log2_alpha_bits;
  int ddepth =  (width >> log2_scale.x) << log2_alpha_bits;
  uint draster = bitmap_raster(ddepth);
  int buffer_y = y - mapped_y + mapped_start;
  uchar *bits;

  if (buffer_y >= height)
		buffer_y -= height;
  bits = scanLineBase(buffer_y);
  
  int alpha_mask = ~7;
	XWPSIntRect bbox;
	int widthA;

	bits_bounding_box(bits, block_height, raster, &bbox);
	bbox.p.x &= alpha_mask;
	bbox.q.x = (bbox.q.x + ~alpha_mask) & alpha_mask;
	widthA = bbox.q.x - bbox.p.x;
	bits_compress_scaled(bits, bbox.p.x, widthA, block_height, raster, bits, draster, &log2_scale, log2_alpha_bits);
	return target->copyAlpha(bits, 0, draster, ps_no_bitmap_id,(mapped_x + bbox.p.x) >> log2_scale.x,
														y >> log2_scale.y, widthA >> log2_scale.x, 1, save_color, alpha_bits);
}

int  XWPSDeviceMem::flushABuf()
{
	int y, code = 0;
  int block_height = 1 << log2_scale.y;

  for (y = 0; y < mapped_height; y += block_height)
		if ((code = abuf_flush_block(mapped_y + y)) < 0)
	    return code;
    mapped_height = mapped_start = 0;
  return 0;
}

void XWPSDeviceMem::makeFullAlphaDevice(const char * devname, int rgb_depth, int gray_depth)
{
	dname = devname;
	color_info.num_components = (rgb_depth ? 3 : 0) + (gray_depth ? 1 : 0);
	color_info.depth = rgb_depth + gray_depth;
	color_info.max_gray = max_value_gray(rgb_depth, gray_depth);
	color_info.max_color = max_value_rgb(rgb_depth, gray_depth);
	color_info.dither_grays = max_value_gray(rgb_depth, gray_depth) + 1;
	color_info.dither_colors = max_value_rgb(rgb_depth, gray_depth) + 1;
}

void XWPSDeviceMem::makeFullDevice(const char * devname, int rgb_depth, int gray_depth)
{
	makeFullAlphaDevice(devname, rgb_depth, gray_depth);
	procs.copy_alpha_.copy_alpha = &XWPSDevice::copyAlphaDefault;
}

void XWPSDeviceMem::memDevice(const char * devname, int rgb_depth, int gray_depth)
{
	makeFullDevice(devname, rgb_depth, gray_depth);
	procs.map_cmyk_color_.map_cmyk_color = &XWPSDevice::mapCMYKColorDefault;
	procs.strip_tile_rectangle_.strip_tile_rectangle = &XWPSDevice::stripTileRectangleDefault;
	procs.get_bits_rectangle_.get_bits_rectanglemem = &XWPSDeviceMem::getBitsRectangleMem;
}

void XWPSDeviceMem::y_transfer_init(PSYTransfer * pyt, int ty, int th)
{
	int bh = 1 << log2_scale.y;

  if (ty < mapped_y || ty > mapped_y + mapped_height) 
  {
		flushABuf();
		mapped_y = ty & -bh;
		mapped_height = bh;
		memset(scanLineBase(0), 0, bh * raster);
  }
  pyt->y_next = ty;
  pyt->height_left = th;
  pyt->transfer_height = 0;
}

void XWPSDeviceMem::y_transfer_next(PSYTransfer * pyt)
{
	int my = mapped_y, mh = mapped_height;
  int ms = mapped_start;
  int ty = pyt->y_next += pyt->transfer_height;
  int th = pyt->height_left;
  int bh = 1 << log2_scale.y;

  int tby, tbh;

  if (ty == my + mh) 
  {
		if (mh == height) 
		{
	    abuf_flush_block(my);
	    mapped_y = my += bh;
	    if ((mapped_start = ms += bh) == mh)
				mapped_start = ms = 0;
		} 
		else 
		{
	    mapped_height = mh += bh;
		}
		memset(scanLineBase((ms == 0 ? mh : ms) - bh), 0, bh * raster);
  }
  
  tby = ty - my + ms;
  if (tby < height) 
  {
		tbh = height - ms;
		if (tbh > mh)
	    tbh = mh;
		tbh -= tby - ms;
  } 
  else 
  {
		tby -= height;
		tbh = ms + mh - height - tby;
  }
  if (tbh > th)
		tbh = th;
  pyt->height_left = th - tbh;
  pyt->transfer_y = tby;
  pyt->transfer_height = tbh;
}
