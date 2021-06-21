/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPSError.h"
#include "psbittable.h"
#include "XWPSBitmap.h"

static const uchar half_byte_1s[16] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
};

static const uchar bits5_trailing_1s[32] = {
    0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 3,
    0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 4
};

static const uchar bits5_leading_1s[32] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 4
};

static const uchar compress_1_1[3] = {
    0, 1, 1
};
static const uchar compress_2_1[5] = {
    0, 0, 1, 1, 1
};
static const uchar compress_2_2[5] = {
    0, 1, 2, 2, 3
};
static const uchar compress_3_1[9] = {
    0, 0, 0, 0, 1, 1, 1, 1, 1
};
static const uchar compress_3_2[9] = {
    0, 0, 1, 1, 2, 2, 2, 3, 3
};
static const uchar compress_4_1[17] = {
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};
static const uchar compress_4_2[17] = {
    0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3
};
static const uchar compress_4_4[17] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 9, 10, 11, 12, 13, 14, 15
};

static const uchar *const compress_tables[4][4] = {
    {compress_1_1, compress_2_1, compress_3_1, compress_4_1},
    {0, compress_2_2, compress_3_2, compress_4_2},
    {0, 0, 0, compress_4_4}
};

void bits_bounding_box(const uchar * data, 
                       uint height, 
                       uint raster,
		                   XWPSIntRect * pbox)
{
	const ulong *lp;
  static const uchar first_1[16] = {
		4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};
  static const uchar last_1[16] = {
		0, 4, 3, 4, 2, 4, 3, 4, 1, 4, 3, 4, 2, 4, 3, 4};
		
	lp = (const ulong *)(data + raster * height);
  while ((const uchar *)lp > data && !lp[-1])
		--lp;
  if ((const uchar *)lp == data) 
  {
		pbox->p.x = pbox->q.x = pbox->p.y = pbox->q.y = 0;
		return;
  }
  pbox->q.y = height = ((const uchar *)lp - data + raster - 1) / raster;
  
  lp = (const ulong *)data;
  while (!*lp)
		++lp;
    
  {
		uint n = ((const uchar *)lp - data) / raster;

		pbox->p.y = n;
		if (n)
	    height -= n, data += n * raster;
  }
  
  {
  	uint raster_longs = raster >> arch_log2_sizeof_long;
		uint left = raster_longs - 1, right = 0;
		ulong llong = 0, rlong = 0;
		const uchar *q;
		uint h, n;

		for (q = data, h = height; h-- > 0; q += raster)
		{
			for (lp = (const ulong *)q, n = 0; n < left && !*lp; lp++, n++);
			if (n < left)
				left = n, llong = *lp;
	    else
				llong |= *lp;
				
			for (lp = (const ulong *)(q + raster - sizeof(long)), n = raster_longs - 1;
		 				n > right && !*lp; lp--, n--);
	    if (n > right)
				right = n, rlong = *lp;
	    else
				rlong |= *lp;
		}
		
#if arch_is_big_endian
#  define last_bits(n) ((1L << (n)) - 1)
#  define shift_out_last(x,n) ((x) >>= (n))
#  define right_justify_last(x,n) DO_NOTHING
#else
#  define last_bits(n) (-1L << ((arch_sizeof_long * 8) - (n)))
#  define shift_out_last(x,n) ((x) <<= (n))
#  define right_justify_last(x,n) (x) >>= ((arch_sizeof_long * 8) - (n))
#endif

	left <<= arch_log2_sizeof_long + 3;
#if arch_sizeof_long == 8
		if (llong & ~last_bits(32))
	    shift_out_last(llong, 32);
		else
	    left += 32;
#endif
		if (llong & ~last_bits(16))
	    shift_out_last(llong, 16);
		else
	    left += 16;
		if (llong & ~last_bits(8))
	    shift_out_last(llong, 8);
		else
	    left += 8;
		right_justify_last(llong, 8);
		if (llong & 0xf0)
	    left += first_1[(uchar) llong >> 4];
		else
	    left += first_1[(uchar) llong] + 4;

		right <<= arch_log2_sizeof_long + 3;
#if arch_sizeof_long == 8
		if (!(rlong & last_bits(32)))
	    shift_out_last(rlong, 32);
		else
	    right += 32;
#endif
		if (!(rlong & last_bits(16)))
	    shift_out_last(rlong, 16);
		else
	    right += 16;
		if (!(rlong & last_bits(8)))
	    shift_out_last(rlong, 8);
		else
	    right += 8;
		right_justify_last(rlong, 8);
		if (!(rlong & 0xf))
	    right += last_1[(uchar) rlong >> 4];
		else
	    right += last_1[(uint) rlong & 0xf] + 4;

		pbox->p.x = left;
		pbox->q.x = right;
  }
}

void bits_compress_scaled(const uchar * src, 
                          int srcx, 
                          uint width, 
                          uint height,
		                      uint sraster, 
		                      uchar * dest, uint draster,
		 											const XWPSLog2ScalePoint * plog2_scale, 
		 											int log2_out_bits)
{
	int log2_x = plog2_scale->x, log2_y = plog2_scale->y;
  int xscale = 1 << log2_x;
  int yscale = 1 << log2_y;
  int out_bits = 1 << log2_out_bits;
  
  int input_byte_out_bits = out_bits << (3 - log2_x);
  uchar input_byte_out_mask = (1 << input_byte_out_bits) - 1;
  const uchar *table =
	compress_tables[log2_out_bits][log2_x + log2_y - 1];
  uint sskip = sraster << log2_y;
  uint dwidth = (width >> log2_x) << log2_out_bits;
  uint dskip = draster - ((dwidth + 7) >> 3);
  uint mask = (1 << xscale) - 1;
  uint count_max = 1 << (log2_x + log2_y);
  
  const uchar *srow = src + (srcx >> 3);
  int in_shift_initial = 8 - xscale - (srcx & 7);
  int in_shift_check = (out_bits <= xscale ? 8 - xscale : -1);
  uchar *d = dest;
  uint h;

  for (h = height; h; srow += sskip, h -= yscale)
  {
  	const uchar *s = srow;

#if ALPHA_LSB_FIRST
#  define out_shift_initial 0
#  define out_shift_update(out_shift, nbits) ((out_shift += (nbits)) >= 8)
#else
#  define out_shift_initial (8 - out_bits)
#  define out_shift_update(out_shift, nbits) ((out_shift -= (nbits)) < 0)
#endif
		int out_shift = out_shift_initial;
		uchar out = 0;
		int in_shift = in_shift_initial;
		int dw = 8 - (srcx & 7);
		int w;
		
		for (w = width; w > 0; w -= dw, dw = 8)
		{
			int index;
	    int in_shift_final =  (w >= dw ? 0 : dw - w);
	    	
	    if (in_shift == in_shift_check && in_shift_final == 0)
				switch (*s)
				{
					case 0:
						for (index = sraster; index != sskip; index += sraster)
			    		if (s[index] != 0)
								goto p;
						if (out_shift_update(out_shift, input_byte_out_bits))
			    		*d++ = out, out_shift &= 7, out = 0;
						s++;
						continue;
#if !ALPHA_LSB_FIRST		/* too messy to make it work */
		    	case 0xff:
						for (index = sraster; index != sskip; index += sraster)
			    		if (s[index] != 0xff)
								goto p;
						{
			    		int shift = (out_shift -= input_byte_out_bits) + out_bits;

			    		if (shift > 0)
								out |= input_byte_out_mask << shift;
			    		else 
			    		{
								out |= input_byte_out_mask >> -shift;
								*d++ = out;
								out_shift += 8;
								out = input_byte_out_mask << (8 + shift);
			    		}
						}
						s++;
						continue;
#endif
		    	default:
						;
				}
				
p:
				do 
				{
					uint count;

					for (index = 0, count = 0; index != sskip; index += sraster)
		    		count += half_byte_1s[(s[index] >> in_shift) & mask];
					if (count != 0 && table[count] == 0) 
					{
		    		uint orig_count = count;
		    		uint shifted_mask = mask << in_shift;
		    		uchar in;
		    		if (yscale > 1) 
		    		{
							if (h < height && (in = s[0] & shifted_mask) != 0) 
							{
			    			uint lower;

			    			for (index = 0, lower = 0; -(index -= sraster) <= sskip && (in &= s[index]) != 0;)
									lower += half_byte_1s[in >> in_shift];
			    			if (lower <= orig_count)
									count += lower;
							}
							
							if (h > yscale && (in = s[sskip - sraster] & shifted_mask) != 0) 
							{
			    			uint upper;

			    			for (index = sskip, upper = 0; index < sskip << 1 && (in &= s[index]) != 0; index += sraster)
									upper += half_byte_1s[in >> in_shift];
			    			if (upper < orig_count)
									count += upper;
							}
		    		}
		    		if (xscale > 1) 
		    		{
							uint mask1 = (mask << 1) + 1;
							if (w < width) 
							{
			    			int lshift = in_shift + xscale - 1;
			    			uint left;

			    			for (index = 0, left = 0; index < sskip; index += sraster) 
			    			{
									uint bits = ((s[index - 1] << 8) + s[index]) >> lshift;

									left += bits5_trailing_1s[bits & mask1];
			    			}
			    			if (left < orig_count)
									count += left;
							}
			
							if (w > xscale) 
							{
			    			int rshift = in_shift - xscale + 8;
			    			uint right;

			    			for (index = 0, right = 0; index < sskip; index += sraster) 
			    			{
									uint bits =	((s[index] << 8) + s[index + 1]) >> rshift;

									right += bits5_leading_1s[(bits & mask1) << (4 - xscale)];
			    			}
			    			if (right <= orig_count)
									count += right;
							}
		    		}
		    		if (count > count_max)
							count = count_max;
					}
					out += table[count] << out_shift;
					if (out_shift_update(out_shift, out_bits))
		    		*d++ = out, out_shift &= 7, out = 0;
	    	}	while ((in_shift -= xscale) >= in_shift_final);
	    	
	    s++, in_shift += 8;
		}
		
		if (out_shift != out_shift_initial)
	    *d++ = out;
		for (w = dskip; w != 0; w--)
	    *d++ = 0;
#undef out_shift_initial
#undef out_shift_update
  }
}

XWPSBitmap::XWPSBitmap()
{
	data = 0;
	raster = 0;
	id = 0;
}

XWPSTileBitmap::XWPSTileBitmap()
	:XWPSBitmap()
{
	rep_width = 0;
	rep_height = 0;
}

XWPSStripBitmap::XWPSStripBitmap()
	:XWPSTileBitmap()
{
	rep_shift = 0;
	shift = 0;
}

void XWPSStripBitmap::init(uchar * d)
{
	data = d;
	raster = sizeof(ulong);
	size.x = sizeof(ulong) * 8;
	size.y = 8;
	id = ps_no_bitmap_id;
	rep_width = 1;
	rep_height = 1;
}

XWPSStripBitmap & XWPSStripBitmap::operator=(const XWPSStripBitmap & other)
{
	data = other.data;
	raster = other.raster;
	size.x = other.size.x;
	size.y = other.size.y;
	id = other.id;
	rep_width = other.rep_width;
	rep_height = other.rep_height;
	rep_shift = other.rep_shift;
	shift = other.rep_shift;
	return *this;
}

XWPSColorTile::XWPSColorTile()
{
	id = ps_no_bitmap_id;
	depth = 0;
	
	tiling_type = 0;
	is_simple = false;
	index = 0;
}

XWPSCachedBits::XWPSCachedBits()
{
	width = height = shift = 0;
	raster = 0;
	id = 0;
}

XWPSBitsCacheChunk::XWPSBitsCacheChunk()
	:next(0),
	 data(0),
	 size(0),
	 allocated(0)
{
}

XWPSBitsCacheChunk::XWPSBitsCacheChunk(uchar * dataA, uint sizeA)
	:next(0),
	 data(0),
	 size(0),
	 allocated(0)
{
	init(dataA, sizeA);
}

XWPSBitsCacheChunk::~XWPSBitsCacheChunk()
{
	if (data)
	{
		free(data);
		data = 0;
	}
}

void XWPSBitsCacheChunk::init(uchar * dataA, uint sizeA)
{
	next = 0;
  data = dataA;
  size = sizeA;
  allocated = 0;
  if (data != 0) 
  {
		XWPSCachedBitsHead *cbh = (XWPSCachedBitsHead *) data;

		cbh->size = size;
		cbh->setFree();
  }
}

XWPSBitsCache::XWPSBitsCache()
	:chunks(0),
	 cnext(0),
	 bsize(0),
	 csize(0)
{
}

XWPSBitsCache::~XWPSBitsCache()
{
}

int XWPSBitsCache::alloc(ulong lsize, XWPSCachedBitsHead ** pcbh)
{
#define ssize ((uint)lsize)
  ulong lsize1 = lsize + sizeof(XWPSCachedBitsHead);

#define ssize1 ((uint)lsize1)
  uint cnextA = cnext;
  XWPSBitsCacheChunk *bck = chunks;
  uint left = bck->size - cnextA;
  XWPSCachedBitsHead *cbh;
  XWPSCachedBitsHead *cbh_next;
  uint fsize = 0;

  if (lsize1 > bck->size - cnextA && lsize != left) 
  {
		*pcbh = 0;
		return -1;
  }
  
  cbh = cbh_next = (XWPSCachedBitsHead *)(bck->data + cnextA);
  while (fsize < ssize1 && fsize != ssize) 
  {
		if (!cbh_next->isFree()) 
		{
	    if (fsize)
				cbh->size = fsize;
	    *pcbh = cbh_next;
	    return -1;
		}
		fsize += cbh_next->size;
		cbh_next = (XWPSCachedBitsHead *) ((uchar *) cbh + fsize);
  }
  if (fsize > ssize) 
  {	
		cbh_next = (XWPSCachedBitsHead *) ((uchar *) cbh + ssize);
		cbh_next->size = fsize - ssize;
		cbh_next->setFree();
  }
  cbh->size = ssize;
  bsize += ssize;
  csize++;
  cnext += ssize;
  bck->allocated += ssize;
  *pcbh = cbh;
  return 0;
#undef ssize
#undef ssize1
}

void XWPSBitsCache::bitsCacheFree(XWPSCachedBitsHead * cbh, XWPSBitsCacheChunk * bck)
{
	uint size = cbh->size;
	csize--;
	bsize -= size;
	bck->allocated -= size;
	cbh->size = size;
	cbh->setFree();
}

void XWPSBitsCache::bitsCacheShorten(XWPSCachedBitsHead * cbh, 
	                      uint diff, 
	                      XWPSBitsCacheChunk * bck)
{
	XWPSCachedBitsHead *next;

  if ((uchar *) cbh + cbh->size == bck->data + cnext &&	bck == chunks)
		cnext -= diff;
  bsize -= diff;
  bck->allocated -= diff;
  cbh->size -= diff;
  next = (XWPSCachedBitsHead *) ((uchar *) cbh + cbh->size);
  next->setFree();
  next->size = diff;
}

void XWPSBitsCache::init(XWPSBitsCacheChunk * bck)
{
	bck->next = bck;
  chunks = bck;
  cnext = 0;
  bsize = 0;
  csize = 0;
}

XWPSBitsPlane::XWPSBitsPlane()
{
	data.write = 0;
	raster = 0;
	depth = 0;
	x = 0;
}

int XWPSBitsPlane::bitsExpandPlane(const XWPSBitsPlane *source, 
	                          int shift, 
	                          int width, 
	                          int height)
{
	int source_depth = source->depth;
  int source_bit = source->x * source_depth;
  const uchar *source_row = source->data.read + (source_bit >> 3);
  int dest_depth = depth;
  int dest_bit = x * dest_depth;
  uchar *dest_row = data.write + (dest_bit >> 3);
  
  enum 
  {
		EXPAND_SLOW = 0,
		EXPAND_1_TO_4,
		EXPAND_8_TO_32
  } loop_case = EXPAND_SLOW;
  int y;

  source_bit &= 7;
  
  if (!(source_bit || (dest_bit & 31) || (raster & 3)))
  {
  	switch (dest_depth) 
  	{
			case 4:
	    	if (source_depth == 1)
					loop_case = EXPAND_1_TO_4;
	    	break;
	    	
			case 32:
	    	if (source_depth == 8 && !(shift & 7))
					loop_case = EXPAND_8_TO_32;
	    	break;
		}
  }
  
  dest_bit &= 7;
  switch (loop_case)
  {
  	case EXPAND_8_TO_32: {
#if arch_is_big_endian
#  define word_shift (shift)
#else
	int word_shift = 24 - shift;
#endif
	for (y = 0; y < height;
	     ++y, source_row += source->raster, dest_row += raster
	     ) {
	    int x;
	    const uchar *source = source_row;
	    quint32 *dest = (quint32 *)dest_row;

	    for (x = width; x > 0; --x)
		*dest++ = (quint32)(*source++) << word_shift;
	}
#undef word_shift
    }
	break;

    case EXPAND_1_TO_4:
    default:
			for (y = 0; y < height; ++y, source_row += source->raster, dest_row += raster) 
			{
	    	int xA;
	    	sample_load_declare_setup(sptr, sbit, source_row, source_bit, source_depth);
	    	sample_store_declare_setup(dptr, dbit, dbbyte, dest_row, dest_bit, dest_depth);

	    	sample_store_preload(dbbyte, dptr, dbit, dest_depth);
	    	for (xA = width; xA > 0; --xA) 
	    	{
					uint color;
					quint32 pixel;

					sample_load_next8(color, sptr, sbit, source_depth);
					pixel = color << shift;
					sample_store_next32(pixel, dptr, dbit, dest_depth, dbbyte);
	    	}
	    	sample_store_flush(dptr, dbit, dest_depth, dbbyte);
			}
			break;
  }
  
  return 0;
}

int XWPSBitsPlane::bitsExtractPlane(const XWPSBitsPlane *source, 
	                           int shift, 
	                           int width, 
	                           int height)
{
	int source_depth = source->depth;
  int source_bit = source->x * source_depth;
  const uchar *source_row = source->data.read + (source_bit >> 3);
  int dest_depth = depth;
  uint plane_mask = (1 << dest_depth) - 1;
  int dest_bit = x * dest_depth;
  uchar *dest_row = data.write + (dest_bit >> 3);
  
  enum 
  {
		EXTRACT_SLOW = 0,
		EXTRACT_4_TO_1,
		EXTRACT_32_TO_8
  } loop_case = EXTRACT_SLOW;
  
  int y;
  source_bit &= 7;
  dest_bit &= 7;
  
  if (!(source_bit | dest_bit))
  {
  	switch (source_depth) 
  	{
			case 4:
	    	loop_case =	(dest_depth == 1 && !(source->raster & 3) && !(source->x & 1) ? EXTRACT_4_TO_1 : EXTRACT_SLOW);
	    	break;
	    	
			case 32:
	    	if (dest_depth == 8 && !(shift & 7)) 
	    	{
					loop_case = EXTRACT_32_TO_8;
					source_row += 3 - (shift >> 3);
	    	}
	    	break;
		}
  }
  
  for (y = 0; y < height; ++y, source_row += source->raster, dest_row += raster)
  {
  	int xA;
  	switch (loop_case)
  	{
  		case EXTRACT_4_TO_1: 
  			{
	    		const uchar *source = source_row;
	    		uchar *dest = dest_row;
	    
	    		for (xA = width; xA >= 8; source += 4, xA -= 8) 
	    		{
						quint32 sword =  (*(const quint32 *)source >> shift) & 0x11111111;
						*dest++ =   byte_acegbdfh_to_abcdefgh[(
#if arch_is_big_endian
		    		(sword >> 21) | (sword >> 14) | (sword >> 7) | sword
#else
		    		(sword << 3) | (sword >> 6) | (sword >> 15) | (sword >> 24)
#endif
						) & 0xff];
	    		}
	    
	    		if (xA) 
	    		{
						uint test = 0x10 << shift, store = 0x80;

						do 
						{
		    			*dest = (*source & test ? *dest | store : *dest & ~store);
		    			if (test >= 0x10)
								test >>= 4;
		    			else
								test <<= 4, ++source;
		    			store >>= 1;
						} while (--xA > 0);
	    		}
	    		break;
				}
				
			case EXTRACT_32_TO_8: 
				{
	    		const uchar *source = source_row;
	    		uchar *dest = dest_row;

	    		for (xA = width; xA > 0; source += 4, --xA)
						*dest++ = *source;
	    		break;
				}
				
			default: 
				{
	    		sample_load_declare_setup(sptr, sbit, source_row, source_bit, source_depth);
	    		sample_store_declare_setup(dptr, dbit, dbbyte, dest_row, dest_bit, dest_depth);

	    		sample_store_preload(dbbyte, dptr, dbit, dest_depth);  
	    		for (xA = width; xA > 0; --xA) 
	    		{
						quint32 color;
						uint pixel;

						sample_load_next32(color, sptr, sbit, source_depth);
						pixel = (color >> shift) & plane_mask;
						sample_store_next8(pixel, dptr, dbit, dest_depth, dbbyte);
	    		}
	    		sample_store_flush(dptr, dbit, dest_depth, dbbyte);
			}
  	}
  }
  
  return 0;
}
