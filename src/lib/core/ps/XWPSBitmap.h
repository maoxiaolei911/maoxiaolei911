/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSBITMAP_H
#define XWPSBITMAP_H

#include "XWPSType.h"

#if arch_align_long_mod <= 4
#  define log2_align_bitmap_mod 2
#else
#if arch_align_long_mod == 8
#  define log2_align_bitmap_mod 3
#endif
#endif
#define align_bitmap_mod (1 << log2_align_bitmap_mod)
#define bitmap_raster(width_bits)\
  ((uint)((((width_bits) + (align_bitmap_mod * 8 - 1))\
    >> (log2_align_bitmap_mod + 3)) << log2_align_bitmap_mod))
    
#define align_cached_bits_mod\
  (qMax(align_bitmap_mod, qMax(arch_align_ptr_mod, arch_align_long_mod)))
  
#define sample_end_\
  default: return (int)(XWPSError::RangeCheck);\
	}} while(0) 
  
#define sample_load_declare(sptr, sbit)\
  const uchar *sptr;\
  int sbit
  
#define sample_load_declare_setup(sptr, sbit, ptr, bitno, sbpv)\
  const uchar *sptr = (ptr);\
  int sample_load_setup(sbit, bitno, sbpv)
  
#define sample_load_setup(sbit, bitno, sbpv)\
  sbit = (bitno)
  
#define sample_load8_(value, sptr, sbit, sbpv)\
  do{ switch ( (sbpv) >> 2 ) {\
  case 0: value = (*(sptr) >> (8 - (sbit) - (sbpv))) & ((sbpv) | 1); break;\
  case 1: value = (*(sptr) >> (4 - (sbit))) & 0xf; break;\
  case 2: value = *(sptr); break;
  	
#define sample_load8(value, sptr, sbit, sbpv)\
  sample_load8_(value, sptr, sbit, sbpv)\
  sample_end_
  
#define sample_load_next8(value, sptr, sbit, sbpv)\
  sample_load8(value, sptr, sbit, sbpv);\
  sample_next(sptr, sbit, sbpv)
  
#define sample_load12_(value, sptr, sbit, sbpv)\
  sample_load8_(value, sptr, sbit, sbpv)\
  case 3:\
    value = ((sbit) ? ((*(sptr) & 0xf) << 8) | (sptr)[1] :\
	      (*(sptr) << 4) | ((sptr)[1] >> 4));\
    break;
    
#define sample_load12(value, sptr, sbit, sbpv)\
  sample_load12_(value, sptr, sbit, sbpv)\
  sample_end_
  
#define sample_load_next12(value, sptr, sbit, sbpv)\
  sample_load12(value, sptr, sbit, sbpv);\
  sample_next(sptr, sbit, sbpv)
  
#define sample_load16_(value, sptr, sbit, sbpv)\
  sample_load12_(value, sptr, sbit, sbpv)\
  case 4: value = (*(sptr) << 8) | (sptr)[1]; break;
#define sample_load16(value, sptr, sbit, sbpv)\
  sample_load16_(value, sptr, sbit, sbpv)\
  sample_end_
#define sample_load_next16(value, sptr, sbit, sbpv)\
  sample_load16(value, sptr, sbit, sbpv);\
  sample_next(sptr, sbit, sbpv)
#define sample_load32(value, sptr, sbit, sbpv)\
  sample_load16_(value, sptr, sbit, sbpv)\
  case 6: value = (*(sptr) << 16) | ((sptr)[1] << 8) | (sptr)[2]; break;\
  case 8:\
    value = (*(sptr) << 24) | ((sptr)[1] << 16) | ((sptr)[2] << 8) | sptr[3];\
    break;\
  sample_end_
#define sample_load_next32(value, sptr, sbit, sbpv)\
  sample_load32(value, sptr, sbit, sbpv);\
  sample_next(sptr, sbit, sbpv)

#define sample_store_declare(dptr, dbit, dbbyte)\
  uchar *dptr;\
  int dbit;\
  uchar dbbyte	
#define sample_store_declare_setup(dptr, dbit, dbbyte, ptr, bitno, dbpv)\
  uchar *dptr = (ptr);\
  int sample_store_setup(dbit, bitno, dbpv);\
  uchar \
    sample_store_preload(dbbyte, dptr, dbit, dbpv)

#define sample_store_setup(dbit, bitno, dbpv)\
  dbit = (bitno)

#define sample_store_preload(dbbyte, dptr, dbit, dbpv)\
  dbbyte = ((dbit) ? (uchar)(*(dptr) & (0xff00 >> (dbit))) : 0)
  	
#define sample_store_next8_(value, dptr, dbit, dbpv, dbbyte)\
  do{switch ( (dbpv) >> 2 ) {\
  case 0:\
    if ( (dbit += (dbpv)) == 8 )\
       *(dptr)++ = dbbyte | (value), dbbyte = 0, dbit = 0;\
    else dbbyte |= (value) << (8 - dbit);\
    break;\
  case 1:\
    if ( dbit ^= 4 ) dbbyte = (uchar)((value) << 4);\
    else *(dptr)++ = dbbyte | (value);\
    break;\
  /* case 2 is deliberately omitted */
  
#define sample_store_next8(value, dptr, dbit, dbpv, dbbyte)\
  sample_store_next8_(value, dptr, dbit, dbpv, dbbyte)\
  case 2: *(dptr)++ = (uchar)(value); break;\
  sample_end_
#define sample_store_next_12_(value, dptr, dbit, dbbyte)\
    if ( dbit ^= 4 ) *(dptr)++ = (value) >> 4, dbbyte = (uchar)((value) << 4);\
    else\
      *(dptr) = dbbyte | ((value) >> 8), (dptr)[1] = (uchar)(value), dptr += 2;
#define sample_store_next_12(value, dptr, dbit, dbbyte)\
  do{sample_store_next_12_(value, dptr, dbit, dbbyte)}while(0)
#define sample_store_next12_(value, dptr, dbit, dbpv, dbbyte)\
  sample_store_next8_(value, dptr, dbit, dbpv, dbbyte)\
  /* case 2 is deliberately omitted */\
  case 3: sample_store_next_12_(value, dptr, dbit, dbbyte) break;
#define sample_store_next12(value, dptr, dbit, dbpv, dbbyte)\
  sample_store_next12_(value, dptr, dbit, dbpv, dbbyte)\
  case 2: *(dptr)++ = (uchar)(value); break;\
  sample_end_
#define sample_store_next16(value, dptr, dbit, dbpv, dbbyte)\
  sample_store_next12_(value, dptr, dbit, dbpv, dbbyte)\
  case 4: *(dptr)++ = (uchar)((value) >> 8);\
  case 2: *(dptr)++ = (uchar)(value); break;\
  sample_end_
#define sample_store_next32(value, dptr, dbit, dbpv, dbbyte)\
  sample_store_next12_(value, dptr, dbit, dbpv, dbbyte)\
  case 8: *(dptr)++ = (uchar)((value) >> 24);\
  case 6: *(dptr)++ = (uchar)((value) >> 16);\
  case 4: *(dptr)++ = (uchar)((value) >> 8);\
  case 2: *(dptr)++ = (uchar)(value); break;\
  sample_end_
  
#define sample_store_skip_next(dptr, dbit, dbpv, dbbyte)\
  if ( (dbpv) < 8 ) {\
    sample_store_flush(dptr, dbit, dbpv, dbbyte);\
    sample_next(dptr, dbit, dbpv);\
  } else dptr += ((dbpv) >> 3)
  	
#define sample_store_flush(dptr, dbit, dbpv, dbbyte)\
  if ( (dbit) != 0 )\
    *(dptr) = dbbyte | (*(dptr) & (0xff >> (dbit)));
    
#define sample_next(ptr, bit, bpv)\
   do{bit += (bpv); ptr += bit >> 3; bit &= 7; }while(0)


#define DECLARE_LINE_ACCUM(line, bpp, xo)\
	sample_store_declare_setup(l_dptr, l_dbit, l_dbyte, line, 0, bpp)
#define LINE_ACCUM(color, bpp)\
	sample_store_next32(color, l_dptr, l_dbit, bpp, l_dbyte)
#define LINE_ACCUM_SKIP(bpp)\
	sample_store_skip_next(l_dptr, l_dbit, bpp, l_dbyte)
#define LINE_ACCUM_STORE(bpp)\
	sample_store_flush(l_dptr, l_dbit, bpp, l_dbyte)

#define DECLARE_LINE_ACCUM_COPY(line, bpp, xo)\
	DECLARE_LINE_ACCUM(line, bpp, xo);\
	int l_xprev = (xo)
	
void bits_bounding_box(const uchar * data, 
                       uint height, 
                       uint raster,
		                   XWPSIntRect * pbox);
void bits_compress_scaled(const uchar * src, 
                          int srcx, 
                          uint width, 
                          uint height,
		                      uint sraster, 
		                      uchar * dest, uint draster,
		 											const XWPSLog2ScalePoint * plog2_scale, 
		 											int log2_out_bits);
	
class XWPSBitmap
{
public:
	XWPSBitmap();
	
public:
	uchar * data;
	int     raster;
	XWPSIntPoint size;
	ulong id;
};
	  
class XWPSTileBitmap : public XWPSBitmap
{
public:
	XWPSTileBitmap();
	
	uint tileSize(int depth) {return (rep_width * depth + 7) / 8 * rep_height;}
	
public:
	ushort rep_width, rep_height;
};

class XWPSStripBitmap : public XWPSTileBitmap
{
public:
	XWPSStripBitmap();
	
	void init(uchar * d);
	
	uint tileSize(int depth) {return (rep_width * depth + 7) / 8 * rep_height;}
	
	XWPSStripBitmap & operator=(const XWPSStripBitmap & other);
	
public:
	ushort rep_shift;
	ushort shift;
};

class XWPSColorTile
{
public:
	XWPSColorTile();
	
public:
	ulong id;
	int depth;
	XWPSUid uid;
	int tiling_type;
	XWPSMatrix step_matrix;
	XWPSRect bbox;
	XWPSStripBitmap tbits;
  XWPSStripBitmap tmask;
  bool is_simple;
  uint index;
};

class XWPSCachedBitsHead
{
public:
	XWPSCachedBitsHead() : size(0),depth(0){}
	
	bool isFree() {return depth == 0;}
	
	void setFree() {depth=0;}
	
public:
	uint size;
	uint depth;	
};

class XWPSCachedBits : public XWPSCachedBitsHead
{
public:
	XWPSCachedBits();
	virtual ~XWPSCachedBits() {}
	
public:
	ushort width, height, shift;
	ushort raster;
	ulong id;
};

class XWPSBitsCacheChunk
{
public:
	XWPSBitsCacheChunk();
	XWPSBitsCacheChunk(uchar * dataA, uint sizeA);
	~XWPSBitsCacheChunk();
	
	void init(uchar * dataA, uint sizeA);
	
public:
	XWPSBitsCacheChunk *next;
  uchar *data;
  uint size;
  uint allocated;
};

class XWPSBitsCache
{
public:
	XWPSBitsCache();
	virtual ~XWPSBitsCache();
	
	int alloc(ulong lsize, XWPSCachedBitsHead ** pcbh);
	
	void bitsCacheFree(XWPSCachedBitsHead * cbh, XWPSBitsCacheChunk * bck);
	void bitsCacheShorten(XWPSCachedBitsHead * cbh, 
	                      uint diff, 
	                      XWPSBitsCacheChunk * bck);
	                      
	void init(XWPSBitsCacheChunk * bck);
	
public:
	XWPSBitsCacheChunk *chunks;
	uint cnext;
	uint bsize;
	uint csize;
};

class XWPSBitsPlane
{
public:
	XWPSBitsPlane();
	
	int bitsExpandPlane(const XWPSBitsPlane *source, 
	                          int shift, 
	                          int width, 
	                          int height);
	int bitsExtractPlane(const XWPSBitsPlane *source, 
	                           int shift, 
	                           int width, 
	                           int height);
	
public:
	union bpd_ 
	{
		uchar *write;
		const uchar *read;
  } data;
  int raster;
  int depth;
  int x;
};

#endif //XWPSBITMAP_H
