/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDEVICEMEM_H
#define XWPSDEVICEMEM_H

#include "XWPSDevice.h"

#define cbytes(ct) sizeof(ct)
#  define chunk_bytes cbytes(chunk)
#define clog2_bytes(ct) (sizeof(ct) == 8 ? 3 : sizeof(ct)>>1)
#  define chunk_log2_bytes clog2_bytes(chunk)
#define cbits(ct) (sizeof(ct)*8)
#  define chunk_bits cbits(chunk)
#define clog2_bits(ct) (clog2_bytes(ct)+3)
#  define chunk_log2_bits clog2_bits(chunk)
#define cbit_mask(ct) (cbits(ct)-1)
#  define chunk_bit_mask cbit_mask(chunk)
#define calign_bytes(ct)\
  (sizeof(ct) == 1 ? 1:\
   sizeof(ct) == sizeof(short) ? arch_align_short_mod :\
   sizeof(ct) == sizeof(int) ? arch_align_int_mod : arch_align_long_mod)
#  define chunk_align_bytes calign_bytes(chunk)
#define calign_bit_mask(ct) (calign_bytes(ct)*8-1)
#  define chunk_align_bit_mask calign_bit_mask(chunk)

#define cmask(ct) ((ct) (((((ct)1 << (sizeof(ct)*8-2)) - 1) << 2) + 3))
#  define chunk_all_bits cmask(chunk)

#define chi_bits(ct,n) (ct)(~(ct)1 << (cbits(ct)-1 - (n)))
#  define chunk_hi_bits(n) chi_bits(chunk,n)

#define arch_cant_shift_full_chunk\
  (arch_is_big_endian && !arch_ints_are_short && !arch_can_shift_full_long)
  
#if arch_is_big_endian
#  define mono_copy_chunk uint
#  define set_mono_right_mask(var, w)\
	(var = ((w) == chunk_bits ? chunk_all_bits : chunk_hi_bits(w)))
		
#  define set_mono_thin_mask(var, w, bit)\
	set_mono_right_mask(var, w), var >>= (bit)
	
#  define set_mono_left_mask(var, bit)\
	(var = chunk_all_bits, var >>= (bit))
#else
#  define mono_copy_chunk quint16
extern const quint16 mono_copy_masks[17];

#  if mono_fill_chunk_bytes == 2
#    define mono_fill_masks mono_copy_masks
#  else
extern const quint32 mono_fill_masks[33];

#  endif

#  define set_mono_left_mask(var, bit)\
	(var = mono_masks[bit])
#  define set_mono_thin_mask(var, w, bit)\
	(var = ~mono_masks[(w) + (bit)] & mono_masks[bit])
#  define set_mono_right_mask(var, ebit)\
	(var = ~mono_masks[ebit])
#endif

#if mono_fill_chunk_bytes == 2
#  define mono_fill_make_pattern(byt) (uint)((uint)(byt) * 0x0101)
#else
#  define mono_fill_make_pattern(byt) (uint)((uint)(byt) * 0x01010101)
#endif

struct PSYTransfer
{
	int y_next;
  int height_left;
  int transfer_y;
  int transfer_height;
};

struct PSDevString
{
	const uchar *data;
	uint   size;
};

class XWPSCachedChar;

class XWPSRenderPlane
{
public:
	XWPSRenderPlane() : depth(0),shift(0),index(0){}
	
public:
	int depth;
	int shift;
	int index;
};

#define max_value_gray(rgb_depth, gray_depth)\
  (gray_depth ? (1 << gray_depth) - 1 : max_value_rgb(rgb_depth, 0))
#define max_value_rgb(rgb_depth, gray_depth)\
  (rgb_depth >= 8 ? 255 : rgb_depth == 4 ? 15 : rgb_depth == 2 ? 3 :\
   rgb_depth == 1 ? 1 : (1 << gray_depth) - 1)

class XWPSDeviceMem : public XWPSDeviceForward
{
	Q_OBJECT 
	
public:
	XWPSDeviceMem(QObject * parent = 0);
	virtual ~XWPSDeviceMem();
	
	ulong bitmapSize();
	void  bitsFillRectangle(uchar * dest, 
	                       int dest_bit, 
	                       uint draster,
		                     uint pattern, 
		                     int width_bits, 
		                     int heightA);
	ulong bitsSize(int widthA, int heightA);
	
	int closeABuf();
	int closeMem();
	int copyAlphaAlpha(const uchar *data, 
	                           int data_x,
	                           int rasterA, 
	                           ulong id, 
	                           int x, 
	                           int y, 
	                           int widthA, 
	                           int heightA,
	                           ulong color, 
	                           int depth);
	int copyAlphaTrue24(const uchar * baseA, 
	                    int sourcex,
		                  int sraster, 
		                  ulong id, 
		                  int x, 
		                  int y, 
		                  int w, 
		                  int h,
		      						ulong color, 
		      						int depth);
	int copyColorMapped2(const uchar * baseA, 
	                     int sourcex, 
	                     int sraster,
		                   ulong id, 
		                   int x, 
		                   int y, 
		                   int w, 
		                   int h);
	int copyColorMapped4(const uchar * base, 
	                     int sourcex, 
	                     int sraster,
		                   ulong id, 
		                   int x, 
		                   int y, 
		                   int w, 
		                   int h);
	int copyColorMapped8(const uchar * base, 
	                     int sourcex, 
	                     int sraster,
		                   ulong id, 
		                   int x, 
		                   int y, 
		                   int w, 
		                   int h);
	int copyColorTrue16(const uchar * baseA, 
	                     int sourcex, 
	                     int sraster,
		                   ulong id, 
		                   int x, 
		                   int y, 
		                   int w, 
		                   int h);
	int copyColorTrue24(const uchar * baseA, 
	                     int sourcex, 
	                     int sraster,
		                   ulong id, 
		                   int x, 
		                   int y, 
		                   int w, 
		                   int h);
	int copyColorTrue32(const uchar * baseA, 
	                     int sourcex, 
	                     int sraster,
		                   ulong id, 
		                   int x, 
		                   int y, 
		                   int w, 
		                   int h);
	int  copyDevice(XWPSDevice **pnew);
	void copyDeviceParamMem(XWPSDeviceMem * proto);
	int copyMonoABuf(const uchar *source_data, 
	                     int source_x, 
	                     int source_raster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong color0, 
	                     ulong color1);
	int copyMonoMapped2(const uchar *baseA, 
	                     int sourcex, 
	                     int sraster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one);
	int copyMonoMapped4(const uchar *baseA, 
	                     int sourcex, 
	                     int sraster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one);
	int copyMonoMapped8(const uchar *baseA, 
	                     int sourcex, 
	                     int sraster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one);
	int copyMonoTrue16(const uchar *baseA, 
	                     int sourcex, 
	                     int sraster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one);
	int copyMonoTrue24(const uchar *baseA, 
	                     int sourcex, 
	                     int sraster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one);
	int copyMonoTrue32(const uchar *baseA, 
	                     int sourcex, 
	                     int sraster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one);
	int copyMonoMono(const uchar *source_data, 
	                     int source_x, 
	                     int source_raster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong color0, 
	                     ulong color1);
	
	ulong dataSize(int widthA, int heightA);
	
	int fillRectangleABuf(int x, int y, int w, int h, ulong color);
	int fillRectangleMapped2(int x, int y, int w, int h, ulong color);
	int fillRectangleMapped4(int x, int y, int w, int h, ulong color);
	int fillRectangleMapped8(int x, int y, int w, int h, ulong color);
	int fillRectangleTrue16(int x, int y, int w, int h, ulong color);
	int fillRectangleTrue24(int x, int y, int w, int h, ulong color);
	int fillRectangleTrue32(int x, int y, int w, int h, ulong color);
	int fillRectangleMono(int x, int y, int w, int h, ulong color);
	
	int getBitsRectangleMem(XWPSIntRect *prect,
	                             XWPSGetBitsParams *params, 
	                             XWPSIntRect **unread);
	void  getClippingBoxABuf(XWPSFixedRect * pbox);                             
	void  getInitialMatrixMem(XWPSMatrix * pmat);
	uint  getRasterMem();
	
	int initializeWordImageDevice(XWPSMatrix * pmat,
	                             uint widthA, 
	                             uint heightA, 
	                             const uchar * colors, 
	                             int colors_size,
		                           bool word_oriented, 
		                           bool page_device);
	bool isABuf();
	virtual bool isMemory() {return true;}
	
	ulong linePtrsSize(int widthA, int heightA);
	void  makeMemABufDevice(XWPSDevice * targetA, 
	                        XWPSLog2ScalePoint * pscale,
													int alpha_bits, 
													int mapped_xA);
	void  makeMemAlphaDevice(XWPSDevice * targetA, int alpha_bits);
	void  makeMemDevice(int bits_per_pixel,
		   								int page_device, 
		   								XWPSDevice * targetA);
	void  makeMemMapped2Device();
	void  makeMemMapped4Device();
	void  makeMemMapped8Device();
	void  makeMemMonoDevice(XWPSDevice * targetA);
	void  makeTrue16Device();
	void  makeTrue24Device();
	void  makeTrue32Device();
	int   mapColorRGBAlpha(ulong color, ushort* prgb);
	int   mapColorRGBMapped(ulong color, ushort* prgb);
	int   mapColorRGBMono(ulong color, ushort* prgb);
	int   mapColorRGBTrue16(ulong color, ushort* prgb);
	ulong mapRGBAlphaColorAlpha(ushort r, ushort g, ushort b, ushort );
	ulong mapRGBColorAlpha(ushort r, ushort g, ushort b);
	ulong mapRGBColorMapped(ushort r, ushort g, ushort b);
	ulong mapRGBColorMono(ushort r, ushort g, ushort b);
	ulong mapRGBColorTrue16(ushort r, ushort g, ushort b);
		
	int openMem();
	void openCacheDevice(XWPSCachedChar * cc);
	int openScanLines(int setup_height);
	      
	uchar* scanLineBase(int y) {return line_ptrs[y];}
	void setInverted(bool black_is_1);
	int setLinePtrs(uchar * baseA, 
	                int rasterA, 
	                uchar **line_ptrsA, 
	                int setup_height);
	                
	int stripCopyRopMono(const uchar *sdata, 
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
	                         ulong lop);
	int stripCopyRopGray8RGB24(const uchar *sdata, 
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
	                         ulong lop);
	                         
	int stripTileRectangleMono(XWPSStripBitmap * tiles,
			      										 int tx, 
			      										 int y, 
			      										 int tw, 
			      										 int th,
			      										 ulong color0, 
			      										 ulong color1,
			      										 int px, 
			      										 int py);
	
public:
	uint raster;
	uchar *base;
	bool foreign_bits;
	bool foreign_line_pointers;
	int num_planes;
	XWPSRenderPlane planes[PS_DEVICE_COLOR_MAX_COMPONENTS];
	XWPSMatrix initial_matrix;
	uchar **line_ptrs;
	PSDevString palette;
	struct _c24 
	{
		ulong rgb;
		quint32 rgbr, gbrg, brgb;
  } color24;
  
  XWPSLog2ScalePoint log2_scale;
  int log2_alpha_bits;
  int mapped_x;
  int mapped_y;
  int mapped_height;
  int mapped_start;
  ulong save_color;
  int plane_depth;
  
  bool own_palette;
  
private:
	int  abuf_flush_block(int y);
	int  flushABuf();
	void makeFullAlphaDevice(const char * devname, int rgb_depth, int gray_depth);
	void makeFullDevice(const char * devname, int rgb_depth, int gray_depth);
	void memDevice(const char * devname, int rgb_depth, int gray_depth);
	
	void y_transfer_init(PSYTransfer * pyt, int ty, int th);
	void y_transfer_next(PSYTransfer * pyt);
};

#endif //
