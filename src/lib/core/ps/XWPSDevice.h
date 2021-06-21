/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDEVICE_H
#define XWPSDEVICE_H

#include <QObject>
#include <QList>
#include "XWPSType.h"

#define DEFAULT_WIDTH_10THS_US_LETTER 85
#define DEFAULT_HEIGHT_10THS_US_LETTER 110

#define DEFAULT_WIDTH_10THS_A4 83
#define DEFAULT_HEIGHT_10THS_A4 117

#ifdef A4
#  define DEFAULT_WIDTH_10THS DEFAULT_WIDTH_10THS_A4
#  define DEFAULT_HEIGHT_10THS DEFAULT_HEIGHT_10THS_A4
#else
#  define DEFAULT_WIDTH_10THS DEFAULT_WIDTH_10THS_US_LETTER
#  define DEFAULT_HEIGHT_10THS DEFAULT_HEIGHT_10THS_US_LETTER
#endif

#ifndef   X_DPI
#define   X_DPI   720
#endif /* X_DPI */
#ifndef   Y_DPI 
#define   Y_DPI   720
#endif /* Y_DPI */

class QIODevice;

class XWPSStream;
class XWPSState;
class XWPSDevice;
class XWPSDeviceHit;
class XWPSDeviceForward;
class XWPSDeviceNull;
class XWPSDeviceVector;
class XWPSDeviceBbox;
class XWPSDeviceClip;
class XWPSDeviceCompositeAlpha;
class XWPSDeviceCPathAccum;
class XWPSDeviceMaskClip;
class XWPSDeviceMem;
class XWPSDevicePatternAccum;
class XWPSDeviceRopTexture;
class XWPSDevicePsdf;
class XWPSDevicePDF;
class XWPSDeviceDVI;
class XWPSContextState;
class XWPSMatrix;
class XWPSCParamList;
class XWPSTileBitmap;
class XWPSStripBitmap;
class XWPSParamList;
class PSParamString;
class XWPSImagerState;
class XWPSPath;
class XWPSFillParams;
class XWPSDeviceColor;
class XWPSClipPath;
class XWPSDataImage;
class XWPSPixelImage;
class XWPSImage1;
class XWPSImageEnumCommon;
class XWPSImageCommon;
class XWPSCidSystemInfo;
class XWPSFont;
class XWPSPDFFont;
class XWPSPDFFontDescriptor;
class XWPSCosObject;
class XWPSCosArray;
class XWPSCosDict;
class XWPSStreamState;
class XWPSDevicePDF;
class XWPSDeviceBbox;
class XWPSTextParams;
class XWPSTextEnum;
class XWPSComposite;
struct PSStreamTemplate;
struct PSStreamState;
class XWPSActiveLine;
class XWPSLineList;
class XWPSEndPoint;
class XWPSPartialLine;
class XWPSTrapLine;
class XWPSStrokeParams;
class XWPSPDFResource;

#define X_SPACE_MIN 24
#define X_SPACE_MAX 150

#define PS_MAX_DASH 11

#define FONT_USES_STANDARD_ENCODING (1<<5)
#define FONT_IS_ADOBE_ROMAN (1<<5)
#define FONT_IS_ITALIC (1<<6)
#define FONT_IS_ALL_CAPS (1<<16)
#define FONT_IS_SMALL_CAPS (1<<17)
#define FONT_IS_FORCE_BOLD (1<<18)

#define GB_COLORS_NATIVE (1L<<0)
#define GB_COLORS_GRAY   (1L<<1)
#define GB_COLORS_RGB    (1L<<2)
#define GB_COLORS_CMYK   (1L<<3)

#define GB_COLORS_STANDARD_ALL\
  (GB_COLORS_GRAY | GB_COLORS_RGB | GB_COLORS_CMYK)
#define GB_COLORS_ALL\
  (GB_COLORS_NATIVE | GB_COLORS_STANDARD_ALL)
  
#define GB_ALPHA_NONE  (1L<<4)
#define GB_ALPHA_FIRST (1L<<5)
#define GB_ALPHA_LAST  (1L<<6)
#define GB_ALPHA_ALL\
  (GB_ALPHA_NONE | GB_ALPHA_FIRST | GB_ALPHA_LAST)

#define GB_DEPTH_1  (1L<<8)
#define GB_DEPTH_2  (1L<<9)
#define GB_DEPTH_4  (1L<<10)
#define GB_DEPTH_8  (1L<<11)
#define GB_DEPTH_12 (1L<<12)
#define GB_DEPTH_16 (1L<<13)
#define GB_DEPTH_ALL\
  (GB_DEPTH_1 | GB_DEPTH_2 | GB_DEPTH_4 | GB_DEPTH_8 |\
   GB_DEPTH_12 | GB_DEPTH_16)
   
#define GB_PACKING_CHUNKY     (1L<<16)
#define GB_PACKING_PLANAR     (1L<<17)
#define GB_PACKING_BIT_PLANAR (1L<<18)

#define GB_PACKING_ALL\
  (GB_PACKING_CHUNKY | GB_PACKING_PLANAR | GB_PACKING_BIT_PLANAR)
  
#define GB_SELECT_PLANES  (1L<<19)
  
#define GB_RETURN_COPY    (1L<<20)
#define GB_RETURN_POINTER (1L<<21)
#define GB_RETURN_ALL\
  (GB_RETURN_COPY | GB_RETURN_POINTER)
  
#define GB_ALIGN_STANDARD (1L<<22)
#define GB_ALIGN_ANY      (1L<<23)
#define GB_ALIGN_ALL\
  (GB_ALIGN_ANY | GB_ALIGN_STANDARD)

#define GB_OFFSET_0         (1L<<24)
#define GB_OFFSET_SPECIFIED (1L<<25)
#define GB_OFFSET_ANY       (1L<<26)
#define GB_OFFSET_ALL\
  (GB_OFFSET_0 | GB_OFFSET_SPECIFIED | GB_OFFSET_ANY)
  
#define GB_RASTER_STANDARD  (1L<<28)
#define GB_RASTER_SPECIFIED (1L<<29)
#define GB_RASTER_ANY       (1L<<30)
#define GB_RASTER_ALL\
  (GB_RASTER_STANDARD | GB_RASTER_SPECIFIED | GB_RASTER_ANY)
  
#define GX_BITMAP_FORMAT_NAMES\
  GB_COLORS_NAMES, GB_ALPHA_NAMES, GB_DEPTH_NAMES, GB_PACKING_NAMES,\
  GB_SELECT_NAMES, GB_RETURN_NAMES, GB_ALIGN_NAMES, GB_OFFSET_NAMES,\
  GB_RASTER_NAMES

#define GB_OPTIONS_MAX_DEPTH(opt)\
"\
\000\001\002\002\004\004\004\004\010\010\010\010\010\010\010\010\
\014\014\014\014\014\014\014\014\014\014\014\014\014\014\014\014\
\020\020\020\020\020\020\020\020\020\020\020\020\020\020\020\020\
\020\020\020\020\020\020\020\020\020\020\020\020\020\020\020\020\
"[((opt) >> 8) & 0x3f]
	  
#define GB_OPTIONS_DEPTH(opt)\
  ((((opt) >> 8) & 0xf) |\
   "\000\000\014\020"[((opt) >> 12) & 3])
   
#define fit_fill_h(y, h)\
	if ( h > height - y )\
	  h = height - y;
	  
#define fit_fill_w(x, w)\
	if ( w > width - x )\
	  w = width - x;
	  
#define fit_fill_xywh(x, y, w, h)\
	fit_fill_xy(x, y, w, h);\
	fit_fill_w(x, w);\
	fit_fill_h(y, h);
  
#define fit_fill(x, y, w, h)\
	fit_fill_xywh(x, y, w, h);\
	if ( w <= 0 || h <= 0 )\
	  return 0;

#define fit_copy_xyw(data, data_x, raster, id, x, y, w, h)\
	if ( (x | y) < 0 ) {\
	  if ( x < 0 )\
	    w += x, data_x -= x, x = 0;\
	  if ( y < 0 )\
	    h += y, data -= y * raster, id = ps_no_bitmap_id, y = 0;\
	}\
	if ( w > width - x )\
	  w = width - x;
	  
#define fit_copy(data, data_x, raster, id, x, y, w, h)\
	fit_copy_xyw(data, data_x, raster, id, x, y, w, h);\
	if ( h > height - y )\
	  h = height - y;\
	if ( w <= 0 || h <= 0 )\
	  return 0;
	  
#define PARALLELOGRAM_IS_RECT(ax, ay, bx, by)\
  ( ((ax) | (by)) == 0 || ((bx) | (ay)) == 0 )
  
#define INT_RECT_FROM_PARALLELOGRAM(prect, px, py, ax, ay, bx, by)\
    int px_ = fixed2int_pixround(px);\
    int py_ = fixed2int_pixround(py);\
    int qx_ = fixed2int_pixround((px) + (ax) + (bx));  /* only one is non-zero */\
    int qy_ = fixed2int_pixround((py) + (ay) + (by));  /* ditto */\
\
    if (qx_ < px_)\
      (prect)->p.x = qx_, (prect)->q.x = px_;\
    else\
      (prect)->p.x = px_, (prect)->q.x = qx_;\
    if (qy_ < py_)\
      (prect)->p.y = qy_, (prect)->q.y = py_;\
    else\
      (prect)->p.y = py_, (prect)->q.y = qy_;


struct PSDeviceProcs
{
	union
	{
		int (XWPSDevice::*open_)();
		int (XWPSDeviceHit::*openhit)();
		int (XWPSDeviceForward::*openfw)();
		int (XWPSDeviceNull::*opennull)();
		int (XWPSDeviceVector::*openvec)();
		int (XWPSDeviceBbox::*openbbox)();
		int (XWPSDeviceClip::*openclip)();
		int (XWPSDeviceCompositeAlpha::*opencra)();
		int (XWPSDeviceCPathAccum::*openpathac)();
		int (XWPSDeviceMaskClip::*openmac)();
		int (XWPSDeviceMem::*openmem)();
		int (XWPSDevicePatternAccum::*openpatta)();
		int (XWPSDeviceRopTexture::*openropt)();
		int (XWPSDevicePsdf::*openpsdf)();
		int (XWPSDevicePDF::*openpdf)();
		int (XWPSDeviceDVI::*opendvi)();
	} open__;
		
	union
	{
		void (XWPSDevice::*get_initial_matrix)(XWPSMatrix *);
		void (XWPSDeviceHit::*get_initial_matrixhit)(XWPSMatrix *);
		void (XWPSDeviceForward::*get_initial_matrixfw)(XWPSMatrix *);
		void (XWPSDeviceNull::*get_initial_matrixnull)(XWPSMatrix *);
		void (XWPSDeviceVector::*get_initial_matrixvec)(XWPSMatrix *);
		void (XWPSDeviceBbox::*get_initial_matrixbbox)(XWPSMatrix *);
		void (XWPSDeviceClip::*get_initial_matrixclip)(XWPSMatrix *);
		void (XWPSDeviceCompositeAlpha::*get_initial_matrixcra)(XWPSMatrix *);
		void (XWPSDeviceCPathAccum::*get_initial_matrixpathac)(XWPSMatrix *);
		void (XWPSDeviceMaskClip::*get_initial_matrixmac)(XWPSMatrix *);
		void (XWPSDeviceMem::*get_initial_matrixmem)(XWPSMatrix *);
		void (XWPSDevicePatternAccum::*get_initial_matrixpatta)(XWPSMatrix *);
		void (XWPSDeviceRopTexture::*get_initial_matrixropt)(XWPSMatrix *);
		void (XWPSDevicePsdf::*get_initial_matrixpsdf)(XWPSMatrix *);
		void (XWPSDevicePDF::*get_initial_matrixpdf)(XWPSMatrix *);
		void (XWPSDeviceDVI::*get_initial_matrixdvi)(XWPSMatrix *);
	}get_initial_matrix_;
	
	union
	{
		int (XWPSDevice::*sync_output)();
		int (XWPSDeviceHit::*sync_outputhit)();
		int (XWPSDeviceForward::*sync_outputfw)();
		int (XWPSDeviceNull::*sync_outputnull)();
		int (XWPSDeviceVector::*sync_outputvec)();
		int (XWPSDeviceBbox::*sync_outputbbox)();
		int (XWPSDeviceClip::*sync_outputclip)();
		int (XWPSDeviceCompositeAlpha::*sync_outputcra)();
		int (XWPSDeviceCPathAccum::*sync_outputpathac)();
		int (XWPSDeviceMaskClip::*sync_outputmac)();
		int (XWPSDeviceMem::*sync_outputmem)();
		int (XWPSDevicePatternAccum::*sync_outputpatta)();
		int (XWPSDeviceRopTexture::*sync_outputropt)();
		int (XWPSDevicePsdf::*sync_outputpsdf)();
		int (XWPSDevicePDF::*sync_outputpdf)();
		int (XWPSDeviceDVI::*sync_outputdvi)();
	} sync_output_;
	
	union
	{
		int (XWPSDevice::*output_page)(int, int);
		int (XWPSDeviceHit::*output_pagehit)(int, int);
		int (XWPSDeviceForward::*output_pagefw)(int, int);
		int (XWPSDeviceNull::*output_pagenull)(int, int);
		int (XWPSDeviceVector::*output_pagevec)(int, int);
		int (XWPSDeviceBbox::*output_pagebbox)(int, int);
		int (XWPSDeviceClip::*output_pageclip)(int, int);
		int (XWPSDeviceCompositeAlpha::*output_pagecra)(int, int);
		int (XWPSDeviceCPathAccum::*output_pagepathac)(int, int);
		int (XWPSDeviceMaskClip::*output_pagemac)(int, int);
		int (XWPSDeviceMem::*output_pagemem)(int, int);
		int (XWPSDevicePatternAccum::*output_pagepatta)(int, int);
		int (XWPSDeviceRopTexture::*output_pageropt)(int, int);
		int (XWPSDevicePsdf::*output_pagepsdf)(int, int);
		int (XWPSDevicePDF::*output_pagepdf)(int, int);
		int (XWPSDeviceDVI::*output_pagedvi)(int, int);
	}output_page_;
	
	union
	{
		int (XWPSDevice::*close_)();
		int (XWPSDeviceHit::*closehit)();
		int (XWPSDeviceForward::*closefw)();
		int (XWPSDeviceNull::*closenull)();
		int (XWPSDeviceVector::*closevec)();
		int (XWPSDeviceBbox::*closebbox)();
		int (XWPSDeviceClip::*closeclip)();
		int (XWPSDeviceCompositeAlpha::*closecra)();
		int (XWPSDeviceCPathAccum::*closepathac)();
		int (XWPSDeviceMaskClip::*closemac)();
		int (XWPSDeviceMem::*closemem)();
		int (XWPSDevicePatternAccum::*closepatta)();
		int (XWPSDeviceRopTexture::*closeropt)();
		int (XWPSDevicePsdf::*closepsdf)();
		int (XWPSDevicePDF::*closepdf)();
		int (XWPSDeviceDVI::*closedvi)();
	}close__;
	
	union
	{
		ulong (XWPSDevice::*map_rgb_color)(ushort, ushort, ushort);
		ulong (XWPSDeviceHit::*map_rgb_colorhit)(ushort, ushort, ushort);
		ulong (XWPSDeviceForward::*map_rgb_colorfw)(ushort, ushort, ushort);
		ulong (XWPSDeviceNull::*map_rgb_colornull)(ushort, ushort, ushort);
		ulong (XWPSDeviceVector::*map_rgb_colorvec)(ushort, ushort, ushort);
		ulong (XWPSDeviceBbox::*map_rgb_colorbbox)(ushort, ushort, ushort);
		ulong (XWPSDeviceClip::*map_rgb_colorclip)(ushort, ushort, ushort);
		ulong (XWPSDeviceCompositeAlpha::*map_rgb_colorcra)(ushort, ushort, ushort);
		ulong (XWPSDeviceCPathAccum::*map_rgb_colorpathac)(ushort, ushort, ushort);
		ulong (XWPSDeviceMaskClip::*map_rgb_colormac)(ushort, ushort, ushort);
		ulong (XWPSDeviceMem::*map_rgb_colormem)(ushort, ushort, ushort);
		ulong (XWPSDevicePatternAccum::*map_rgb_colorpatta)(ushort, ushort, ushort);
		ulong (XWPSDeviceRopTexture::*map_rgb_colorropt)(ushort, ushort, ushort);
		ulong (XWPSDevicePsdf::*map_rgb_colorpsdf)(ushort, ushort, ushort);
		ulong (XWPSDevicePDF::*map_rgb_colorpdf)(ushort, ushort, ushort);
		ulong (XWPSDeviceDVI::*map_rgb_colordvi)(ushort, ushort, ushort);
	}map_rgb_color_;
	
	union
	{
		int (XWPSDevice::*map_color_rgb)(ulong color, ushort *);
		int (XWPSDeviceHit::*map_color_rgbhit)(ulong color, ushort *);
		int (XWPSDeviceForward::*map_color_rgbfw)(ulong color, ushort *);
		int (XWPSDeviceNull::*map_color_rgbnull)(ulong color, ushort *);
		int (XWPSDeviceVector::*map_color_rgbvec)(ulong color, ushort *);
		int (XWPSDeviceBbox::*map_color_rgbbbox)(ulong color, ushort *);
		int (XWPSDeviceClip::*map_color_rgbclip)(ulong color, ushort *);
		int (XWPSDeviceCompositeAlpha::*map_color_rgbcra)(ulong color, ushort *);
		int (XWPSDeviceCPathAccum::*map_color_rgbpathac)(ulong color, ushort *);
		int (XWPSDeviceMaskClip::*map_color_rgbmac)(ulong color, ushort *);
		int (XWPSDeviceMem::*map_color_rgbmem)(ulong color, ushort *);
		int (XWPSDevicePatternAccum::*map_color_rgbpatta)(ulong color, ushort *);
		int (XWPSDeviceRopTexture::*map_color_rgbropt)(ulong color, ushort *);
		int (XWPSDevicePsdf::*map_color_rgbpsdf)(ulong color, ushort *);
		int (XWPSDevicePDF::*map_color_rgbpdf)(ulong color, ushort *);
		int (XWPSDeviceDVI::*map_color_rgbdvi)(ulong color, ushort *);
	}map_color_rgb_;
	
	union
	{
		int (XWPSDevice::*fill_rectangle)(int, int, int, int, ulong);
		int (XWPSDeviceHit::*fill_rectanglehit)(int, int, int, int, ulong);
		int (XWPSDeviceForward::*fill_rectanglefw)(int, int, int, int, ulong);
		int (XWPSDeviceNull::*fill_rectanglenull)(int, int, int, int, ulong);
		int (XWPSDeviceVector::*fill_rectanglevec)(int, int, int, int, ulong);
		int (XWPSDeviceBbox::*fill_rectanglebbox)(int, int, int, int, ulong);
		int (XWPSDeviceClip::*fill_rectangleclip)(int, int, int, int, ulong);
		int (XWPSDeviceCompositeAlpha::*fill_rectanglecra)(int, int, int, int, ulong);
		int (XWPSDeviceCPathAccum::*fill_rectanglepathac)(int, int, int, int, ulong);
		int (XWPSDeviceMaskClip::*fill_rectanglemac)(int, int, int, int, ulong);
		int (XWPSDeviceMem::*fill_rectanglemem)(int, int, int, int, ulong);
		int (XWPSDevicePatternAccum::*fill_rectanglepatta)(int, int, int, int, ulong);
		int (XWPSDeviceRopTexture::*fill_rectangleropt)(int, int, int, int, ulong);
		int (XWPSDevicePsdf::*fill_rectanglepsdf)(int, int, int, int, ulong);
		int (XWPSDevicePDF::*fill_rectanglepdf)(int, int, int, int, ulong);
		int (XWPSDeviceDVI::*fill_rectangledvi)(int, int, int, int, ulong);
	}fill_rectangle_;
	
	union
	{
		int (XWPSDevice::*tile_rectangle)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
		int (XWPSDeviceHit::*tile_rectanglehit)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
		int (XWPSDeviceForward::*tile_rectanglefw)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
		int (XWPSDeviceNull::*tile_rectanglenull)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
		int (XWPSDeviceVector::*tile_rectanglevec)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
		int (XWPSDeviceBbox::*tile_rectanglebbox)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
		int (XWPSDeviceClip::*tile_rectangleclip)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
		int (XWPSDeviceCompositeAlpha::*tile_rectanglecra)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
		int (XWPSDeviceCPathAccum::*tile_rectanglepathac)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
		int (XWPSDeviceMaskClip::*tile_rectanglemac)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
		int (XWPSDeviceMem::*tile_rectanglemem)();
		int (XWPSDevicePatternAccum::*tile_rectanglepatta)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
		int (XWPSDeviceRopTexture::*tile_rectangleropt)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
		int (XWPSDevicePsdf::*tile_rectanglepsdf)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
		int (XWPSDevicePDF::*tile_rectanglepdf)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
		int (XWPSDeviceDVI::*tile_rectangledvi)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int);
	}tile_rectangle_;
	
	union
	{
		int (XWPSDevice::*copy_mono)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong );
		int (XWPSDeviceHit::*copy_monohit)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
		int (XWPSDeviceForward::*copy_monofw)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
		int (XWPSDeviceNull::*copy_mononull)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
		int (XWPSDeviceVector::*copy_monovec)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
		int (XWPSDeviceBbox::*copy_monobbox)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
		int (XWPSDeviceClip::*copy_monoclip)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
		int (XWPSDeviceCompositeAlpha::*copy_monocra)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
		int (XWPSDeviceCPathAccum::*copy_monopathac)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
		int (XWPSDeviceMaskClip::*copy_monomac)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
		int (XWPSDeviceMem::*copy_monomem)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
		int (XWPSDevicePatternAccum::*copy_monopatta)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
		int (XWPSDeviceRopTexture::*copy_monoropt)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
		int (XWPSDevicePsdf::*copy_monopsdf)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
		int (XWPSDevicePDF::*copy_monopdf)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
		int (XWPSDeviceDVI::*copy_monodvi)(const uchar *, 
	                             int , 
	                             int , 
	                             ulong ,
	                             int , 
	                             int , 
	                             int , 
	                             int ,
	                             ulong , 
	                            ulong);
	}copy_mono_;
	
	union
	{
		int (XWPSDevice::*copy_color)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int );
		int (XWPSDeviceHit::*copy_colorhit)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
		int (XWPSDeviceForward::*copy_colorfw)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
		int (XWPSDeviceNull::*copy_colornull)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
		int (XWPSDeviceVector::*copy_colorvec)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
		int (XWPSDeviceBbox::*copy_colorbbox)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
		int (XWPSDeviceClip::*copy_colorclip)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
		int (XWPSDeviceCompositeAlpha::*copy_colorcra)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
		int (XWPSDeviceCPathAccum::*copy_colorpathac)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
		int (XWPSDeviceMaskClip::*copy_colormac)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
		int (XWPSDeviceMem::*copy_colormem)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
		int (XWPSDevicePatternAccum::*copy_colorpatta)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
		int (XWPSDeviceRopTexture::*copy_colorropt)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
		int (XWPSDevicePsdf::*copy_colorpsdf)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
		int (XWPSDevicePDF::*copy_colorpdf)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
		int (XWPSDeviceDVI::*copy_colordvi)(const uchar *, 
	                              int , 
	                              int , 
	                              ulong ,
	                              int , 
	                              int y, 
	                              int w, 
	                              int);
	}copy_color_;
	
	union
	{
		int (XWPSDevice::*draw_line)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDeviceHit::*draw_linehit)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDeviceForward::*draw_linefw)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDeviceNull::*draw_linenull)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDeviceVector::*draw_linevec)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDeviceBbox::*draw_linebbox)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDeviceClip::*draw_lineclip)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDeviceCompositeAlpha::*draw_linecra)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDeviceCPathAccum::*draw_linepathac)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDeviceMaskClip::*draw_linemac)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDeviceMem::*draw_linemem)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDevicePatternAccum::*draw_linepatta)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDeviceRopTexture::*draw_lineropt)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDevicePsdf::*draw_linepsdf)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDevicePDF::*draw_linepdf)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
		int (XWPSDeviceDVI::*draw_linedvi)(int , 
	                             int , 
	                             int , 
	                             int , 
	                             ulong);
	}draw_line_;
	
	union
	{
		int (XWPSDevice::*get_bits)(int, uchar *, uchar **);
		int (XWPSDeviceHit::*get_bitshit)(int, uchar *, uchar **);
		int (XWPSDeviceForward::*get_bitsfw)(int, uchar *, uchar **);
		int (XWPSDeviceNull::*get_bitsnull)(int, uchar *, uchar **);
		int (XWPSDeviceVector::*get_bitsvec)(int, uchar *, uchar **);
		int (XWPSDeviceBbox::*get_bitsbbox)(int, uchar *, uchar **);
		int (XWPSDeviceClip::*get_bitsclip)(int, uchar *, uchar **);
		int (XWPSDeviceCompositeAlpha::*get_bitscra)(int, uchar *, uchar **);
		int (XWPSDeviceCPathAccum::*get_bitspathac)(int, uchar *, uchar **);
		int (XWPSDeviceMaskClip::*get_bitsmac)(int, uchar *, uchar **);
		int (XWPSDeviceMem::*get_bitsmem)(int, uchar *, uchar **);
		int (XWPSDevicePatternAccum::*get_bitspatta)(int, uchar *, uchar **);
		int (XWPSDeviceRopTexture::*get_bitsropt)(int, uchar *, uchar **);
		int (XWPSDevicePsdf::*get_bitspsdf)(int, uchar *, uchar **);
		int (XWPSDevicePDF::*get_bitspdf)(int, uchar *, uchar **);
		int (XWPSDeviceDVI::*get_bitsdvi)(int, uchar *, uchar **);
	}get_bits_;
	
	union
	{
		int (XWPSDevice::*get_params)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceHit::*get_paramshit)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceForward::*get_paramsfw)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceNull::*get_paramsnull)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceVector::*get_paramsvec)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceBbox::*get_paramsbbox)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceClip::*get_paramsclip)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceCompositeAlpha::*get_paramscra)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceCPathAccum::*get_paramspathac)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceMaskClip::*get_paramsmac)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceMem::*get_paramsmem)(XWPSContextState *, XWPSParamList *);
		int (XWPSDevicePatternAccum::*get_paramspatta)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceRopTexture::*get_paramsropt)(XWPSContextState *, XWPSParamList *);
		int (XWPSDevicePsdf::*get_paramspsdf)(XWPSContextState *, XWPSParamList *);
		int (XWPSDevicePDF::*get_paramspdf)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceDVI::*get_paramsdvi)(XWPSContextState *, XWPSParamList *);
	}get_params_;
	
	union
	{
		int (XWPSDevice::*put_params)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceHit::*put_paramshit)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceForward::*put_paramsfw)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceNull::*put_paramsnull)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceVector::*put_paramsvec)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceBbox::*put_paramsbbox)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceClip::*put_paramsclip)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceCompositeAlpha::*put_paramscra)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceCPathAccum::*put_paramspathac)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceMaskClip::*put_paramsmac)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceMem::*put_paramsmem)(XWPSContextState *, XWPSParamList *);
		int (XWPSDevicePatternAccum::*put_paramspatta)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceRopTexture::*put_paramsropt)(XWPSContextState *, XWPSParamList *);
		int (XWPSDevicePsdf::*put_paramspsdf)(XWPSContextState *, XWPSParamList *);
		int (XWPSDevicePDF::*put_paramspdf)(XWPSContextState *, XWPSParamList *);
		int (XWPSDeviceDVI::*put_paramsdvi)(XWPSContextState *, XWPSParamList *);
	}put_params_;
	
	union
	{
		ulong (XWPSDevice::*map_cmyk_color)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceHit::*map_cmyk_colorhit)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceForward::*map_cmyk_colorfw)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceNull::*map_cmyk_colornull)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceVector::*map_cmyk_colorvec)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceBbox::*map_cmyk_colorbbox)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceClip::*map_cmyk_colorclip)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceCompositeAlpha::*map_cmyk_colorcra)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceCPathAccum::*map_cmyk_colorpathac)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceMaskClip::*map_cmyk_colormac)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceMem::*map_cmyk_colormem)(ushort, ushort, ushort, ushort);
		ulong (XWPSDevicePatternAccum::*map_cmyk_colorpatta)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceRopTexture::*map_cmyk_colorropt)(ushort, ushort, ushort, ushort);
		ulong (XWPSDevicePsdf::*map_cmyk_colorpsdf)(ushort, ushort, ushort, ushort);
		ulong (XWPSDevicePDF::*map_cmyk_colorpdf)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceDVI::*map_cmyk_colordvi)(ushort, ushort, ushort, ushort);
	}map_cmyk_color_;
	
	union
	{
		XWPSDevice * (XWPSDevice::*get_xfont_device)();
		XWPSDevice * (XWPSDeviceHit::*get_xfont_devicehit)();
		XWPSDevice * (XWPSDeviceForward::*get_xfont_devicefw)();
		XWPSDevice * (XWPSDeviceNull::*get_xfont_devicenull)();
		XWPSDevice * (XWPSDeviceVector::*get_xfont_devicevec)();
		XWPSDevice * (XWPSDeviceBbox::*get_xfont_devicebbox)();
		XWPSDevice * (XWPSDeviceClip::*get_xfont_deviceclip)();
		XWPSDevice * (XWPSDeviceCompositeAlpha::*get_xfont_devicecra)();
		XWPSDevice * (XWPSDeviceCPathAccum::*get_xfont_devicepathac)();
		XWPSDevice * (XWPSDeviceMaskClip::*get_xfont_devicemac)();
		XWPSDevice * (XWPSDeviceMem::*get_xfont_devicemem)();
		XWPSDevice * (XWPSDevicePatternAccum::*get_xfont_devicepatta)();
		XWPSDevice * (XWPSDeviceRopTexture::*get_xfont_deviceropt)();
		XWPSDevice * (XWPSDevicePsdf::*get_xfont_devicepsdf)();
		XWPSDevice * (XWPSDevicePDF::*get_xfont_devicepdf)();
		XWPSDevice * (XWPSDeviceDVI::*get_xfont_devicedvi)();
	}get_xfont_device_;
	
	union
	{
		ulong (XWPSDevice::*map_rgb_alpha_color)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceHit::*map_rgb_alpha_colorhit)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceForward::*map_rgb_alpha_colorfw)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceNull::*map_rgb_alpha_colornull)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceVector::*map_rgb_alpha_colorvec)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceBbox::*map_rgb_alpha_colorbbox)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceClip::*map_rgb_alpha_colorclip)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceCompositeAlpha::*map_rgb_alpha_colorcra)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceCPathAccum::*map_rgb_alpha_colorpathac)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceMaskClip::*map_rgb_alpha_colormac)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceMem::*map_rgb_alpha_colormem)(ushort, ushort, ushort, ushort);
		ulong (XWPSDevicePatternAccum::*map_rgb_alpha_colorpatta)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceRopTexture::*map_rgb_alpha_colorropt)(ushort, ushort, ushort, ushort);
		ulong (XWPSDevicePsdf::*map_rgb_alpha_colorpsdf)(ushort, ushort, ushort, ushort);
		ulong (XWPSDevicePDF::*map_rgb_alpha_colorpdf)(ushort, ushort, ushort, ushort);
		ulong (XWPSDeviceDVI::*map_rgb_alpha_colordvi)(ushort, ushort, ushort, ushort);
	}map_rgb_alpha_color_;
	
	union
	{
		XWPSDevice * (XWPSDevice::*get_page_device)();
		XWPSDevice * (XWPSDeviceHit::*get_page_devicehit)();
		XWPSDevice * (XWPSDeviceForward::*get_page_devicefw)();
		XWPSDevice * (XWPSDeviceNull::*get_page_devicenull)();
		XWPSDevice * (XWPSDeviceVector::*get_page_devicevec)();
		XWPSDevice * (XWPSDeviceBbox::*get_page_devicebbox)();
		XWPSDevice * (XWPSDeviceClip::*get_page_deviceclip)();
		XWPSDevice * (XWPSDeviceCompositeAlpha::*get_page_devicecra)();
		XWPSDevice * (XWPSDeviceCPathAccum::*get_page_devicepathac)();
		XWPSDevice * (XWPSDeviceMaskClip::*get_page_devicemac)();
		XWPSDevice * (XWPSDeviceMem::*get_page_devicemem)();
		XWPSDevice * (XWPSDevicePatternAccum::*get_page_devicepatta)();
		XWPSDevice * (XWPSDeviceRopTexture::*get_page_deviceropt)();
		XWPSDevice * (XWPSDevicePsdf::*get_page_devicepsdf)();
		XWPSDevice * (XWPSDevicePDF::*get_page_devicepdf)();
		XWPSDevice * (XWPSDeviceDVI::*get_page_devicedvi)();
	}get_page_device_;
	
	union
	{
		int (XWPSDevice::*get_alpha_bits)(PSGraphicsObjectType);
		int (XWPSDeviceHit::*get_alpha_bitshit)();
		int (XWPSDeviceForward::*get_alpha_bitsfw)();
		int (XWPSDeviceNull::*get_alpha_bitsnull)();
		int (XWPSDeviceVector::*get_alpha_bitsvec)();
		int (XWPSDeviceBbox::*get_alpha_bitsbbox)();
		int (XWPSDeviceClip::*get_alpha_bitsclip)();
		int (XWPSDeviceCompositeAlpha::*get_alpha_bitscra)();
		int (XWPSDeviceCPathAccum::*get_alpha_bitspathac)();
		int (XWPSDeviceMaskClip::*get_alpha_bitsmac)();
		int (XWPSDeviceMem::*get_alpha_bitsmem)();
		int (XWPSDevicePatternAccum::*get_alpha_bitspatta)();
		int (XWPSDeviceRopTexture::*get_alpha_bitsropt)();
		int (XWPSDevicePsdf::*get_alpha_bitspsdf)();
		int (XWPSDevicePDF::*get_alpha_bitspdf)();
		int (XWPSDeviceDVI::*get_alpha_bitsdvi)();
	}get_alpha_bits_;
	
	union
	{
		int (XWPSDevice::*copy_alpha)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDeviceHit::*copy_alphahit)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDeviceForward::*copy_alphafw)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDeviceNull::*copy_alphanull)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDeviceVector::*copy_alphavec)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDeviceBbox::*copy_alphabbox)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDeviceClip::*copy_alphaclip)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDeviceCompositeAlpha::*copy_alphacra)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDeviceCPathAccum::*copy_alphapathac)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDeviceMaskClip::*copy_alphamac)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDeviceMem::*copy_alphamem)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDevicePatternAccum::*copy_alphapatta)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDeviceRopTexture::*copy_alpharopt)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDevicePsdf::*copy_alphapsdf)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDevicePDF::*copy_alphapdf)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
		int (XWPSDeviceDVI::*copy_alphadvi)(const uchar *, 
	                              int,
	                              int, 
	                              ulong, 
	                              int, 
	                              int, 
	                              int, 
	                              int,
	                              ulong, 
	                              int);
	}copy_alpha_;
	
	union
	{
		int (XWPSDevice::*get_band)(int , int *);
		int (XWPSDeviceHit::*get_bandhit)(int , int *);
		int (XWPSDeviceForward::*get_bandfw)(int , int *);
		int (XWPSDeviceNull::*get_bandnull)(int , int *);
		int (XWPSDeviceVector::*get_bandvec)(int , int *);
		int (XWPSDeviceBbox::*get_bandbbox)(int , int *);
		int (XWPSDeviceClip::*get_bandclip)(int , int *);
		int (XWPSDeviceCompositeAlpha::*get_bandcra)(int , int *);
		int (XWPSDeviceCPathAccum::*get_bandpathac)(int , int *);
		int (XWPSDeviceMaskClip::*get_bandmac)(int , int *);
		int (XWPSDeviceMem::*get_bandmem)(int , int *);
		int (XWPSDevicePatternAccum::*get_bandpatta)(int , int *);
		int (XWPSDeviceRopTexture::*get_bandropt)(int , int *);
		int (XWPSDevicePsdf::*get_bandpsdf)(int , int *);
		int (XWPSDevicePDF::*get_bandpdf)(int , int *);
		int (XWPSDeviceDVI::*get_banddvi)(int , int *);
	}get_band_;
	
	union
	{
		int (XWPSDevice::*copy_rop)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong );
		int (XWPSDeviceHit::*copy_rophit)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong);
		int (XWPSDeviceForward::*copy_ropfw)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong);
		int (XWPSDeviceNull::*copy_ropnull)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong);
		int (XWPSDeviceVector::*copy_ropvec)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong);
		int (XWPSDeviceBbox::*copy_ropbbox)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong);
		int (XWPSDeviceClip::*copy_ropclip)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong);
		int (XWPSDeviceCompositeAlpha::*copy_ropcra)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong);
		int (XWPSDeviceCPathAccum::*copy_roppathac)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong);
		int (XWPSDeviceMaskClip::*copy_ropmac)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong);
		int (XWPSDeviceMem::*copy_ropmem)();
		int (XWPSDevicePatternAccum::*copy_roppatta)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong);
		int (XWPSDeviceRopTexture::*copy_ropropt)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong);
		int (XWPSDevicePsdf::*copy_roppsdf)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong);
		int (XWPSDevicePDF::*copy_roppdf)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong);
		int (XWPSDeviceDVI::*copy_ropdvi)(const uchar * , 
	                            int , 
	                            uint , 
	                            ulong ,
		                          const ulong * ,
	                            XWPSTileBitmap * , 
	                            const ulong * ,
		                          int , 
		                          int , 
		                          int , 
		                          int ,
		                          int , 
		                          int , 
		                          ulong);
	}copy_rop_;
	
	union
	{
		int (XWPSDevice::*fill_path)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDeviceHit::*fill_pathhit)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDeviceForward::*fill_pathfw)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDeviceNull::*fill_pathnull)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDeviceVector::*fill_pathvec)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDeviceBbox::*fill_pathbbox)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDeviceClip::*fill_pathclip)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDeviceCompositeAlpha::*fill_pathcra)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDeviceCPathAccum::*fill_pathpathac)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDeviceMaskClip::*fill_pathmac)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDeviceMem::*fill_pathmem)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDevicePatternAccum::*fill_pathpatta)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDeviceRopTexture::*fill_pathropt)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDevicePsdf::*fill_pathpsdf)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDevicePDF::*fill_pathpdf)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
		int (XWPSDeviceDVI::*fill_pathdvi)(XWPSImagerState * , 
	                             XWPSPath * ,
		                           XWPSFillParams * ,
	                             XWPSDeviceColor * , 
	                             XWPSClipPath * );
	}fill_path_;
	
	union
	{
		int (XWPSDevice::*stroke_path)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDeviceHit::*stroke_pathhit)(const XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDeviceForward::*stroke_pathfw)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDeviceNull::*stroke_pathnull)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDeviceVector::*stroke_pathvec)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDeviceBbox::*stroke_pathbbox)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDeviceClip::*stroke_pathclip)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDeviceCompositeAlpha::*stroke_pathcra)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDeviceCPathAccum::*stroke_pathpathac)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDeviceMaskClip::*stroke_pathmac)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDeviceMem::*stroke_pathmem)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDevicePatternAccum::*stroke_pathpatta)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDeviceRopTexture::*stroke_pathropt)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDevicePsdf::*stroke_pathpsdf)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDevicePDF::*stroke_pathpdf)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
		int (XWPSDeviceDVI::*stroke_pathdvi)(XWPSImagerState * , 
	                               XWPSPath * ,
		                             XWPSStrokeParams * ,
	                               XWPSDeviceColor * , 
	                               XWPSClipPath *);
	}stroke_path_;
	
	union
	{
		int (XWPSDevice::*fill_mask)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDeviceHit::*fill_maskhit)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDeviceForward::*fill_maskfw)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDeviceNull::*fill_masknull)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDeviceVector::*fill_maskvec)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDeviceBbox::*fill_maskbbox)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDeviceClip::*fill_maskclip)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDeviceCompositeAlpha::*fill_maskcra)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDeviceCPathAccum::*fill_maskpathac)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDeviceMaskClip::*fill_maskmac)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDeviceMem::*fill_maskmem)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDevicePatternAccum::*fill_maskpatta)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDeviceRopTexture::*fill_maskropt)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDevicePsdf::*fill_maskpsdf)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDevicePDF::*fill_maskpdf)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
		int (XWPSDeviceDVI::*fill_maskdvi)(const uchar * , 
	                             int , 
	                             int, 
	                             ulong,
		   									       int, 
		   									       int, 
		   									       int, 
		   									       int,
		   									       XWPSDeviceColor *, 
		   									       int,
		   									       ulong, 
		   									       XWPSClipPath *);
	}fill_mask_;
	
	
	union
	{
		int (XWPSDevice::*fill_trapezoid)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDeviceHit::*fill_trapezoidhit)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDeviceForward::*fill_trapezoidfw)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDeviceNull::*fill_trapezoidnull)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDeviceVector::*fill_trapezoidvec)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDeviceBbox::*fill_trapezoidbbox)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDeviceClip::*fill_trapezoidclip)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDeviceCompositeAlpha::*fill_trapezoidcra)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDeviceCPathAccum::*fill_trapezoidpathac)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDeviceMaskClip::*fill_trapezoidmac)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDeviceMem::*fill_trapezoidmem)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDevicePatternAccum::*fill_trapezoidpatta)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDeviceRopTexture::*fill_trapezoidropt)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDevicePsdf::*fill_trapezoidpsdf)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDevicePDF::*fill_trapezoidpdf)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
		int (XWPSDeviceDVI::*fill_trapezoiddvi)(XWPSFixedEdge *, 
	                                  XWPSFixedEdge *,
	                                  long , 
	                                  long, 
	                                  bool,
	                                  XWPSDeviceColor *, 
	                                  ulong);
	}fill_trapezoid_;
	
	union
	{
		int (XWPSDevice::*fill_parallelogram)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDeviceHit::*fill_parallelogramhit)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDeviceForward::*fill_parallelogramfw)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDeviceNull::*fill_parallelogramnull)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDeviceVector::*fill_parallelogramvec)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDeviceBbox::*fill_parallelogrambbox)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDeviceClip::*fill_parallelogramclip)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDeviceCompositeAlpha::*fill_parallelogramcra)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDeviceCPathAccum::*fill_parallelogrampathac)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDeviceMaskClip::*fill_parallelogrammac)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDeviceMem::*fill_parallelogrammem)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDevicePatternAccum::*fill_parallelogrampatta)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDeviceRopTexture::*fill_parallelogramropt)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDevicePsdf::*fill_parallelogrampsdf)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDevicePDF::*fill_parallelogrampdf)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
		int (XWPSDeviceDVI::*fill_parallelogramdvi)(long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long, 
	                                      long,
	                                      XWPSDeviceColor *, 
	                                      ulong);
	}fill_parallelogram_;
	
	union
	{
		int (XWPSDevice::*fill_triangle)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDeviceHit::*fill_trianglehit)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDeviceForward::*fill_trianglefw)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDeviceNull::*fill_trianglenull)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDeviceVector::*fill_trianglevec)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDeviceBbox::*fill_trianglebbox)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDeviceClip::*fill_triangleclip)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDeviceCompositeAlpha::*fill_trianglecra)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDeviceCPathAccum::*fill_trianglepathac)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDeviceMaskClip::*fill_trianglemac)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDeviceMem::*fill_trianglemem)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDevicePatternAccum::*fill_trianglepatta)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDeviceRopTexture::*fill_triangleropt)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDevicePsdf::*fill_trianglepsdf)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDevicePDF::*fill_trianglepdf)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
		int (XWPSDeviceDVI::*fill_triangledvi)(long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long , 
	                                 long ,
	                                 XWPSDeviceColor *, 
	                                 ulong);
	}fill_triangle_;
	
	union
	{
		int (XWPSDevice::*draw_thin_line)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDeviceHit::*draw_thin_linehit)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDeviceForward::*draw_thin_linefw)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDeviceNull::*draw_thin_linenull)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDeviceVector::*draw_thin_linevec)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDeviceBbox::*draw_thin_linebbox)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDeviceClip::*draw_thin_lineclip)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDeviceCompositeAlpha::*draw_thin_linecra)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDeviceCPathAccum::*draw_thin_linepathac)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDeviceMaskClip::*draw_thin_linemac)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDeviceMem::*draw_thin_linemem)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDevicePatternAccum::*draw_thin_linepatta)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDeviceRopTexture::*draw_thin_lineropt)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDevicePsdf::*draw_thin_linepsdf)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDevicePDF::*draw_thin_linepdf)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
		int (XWPSDeviceDVI::*draw_thin_linedvi)(long , 
                                    long , 
                                    long , 
                                    long ,
                                    XWPSDeviceColor *, 
                                    ulong );
	}draw_thin_line_;
	
	union
	{
		int (XWPSDevice::*begin_image)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDeviceHit::*begin_imagehit)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDeviceForward::*begin_imagefw)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDeviceNull::*begin_imagenull)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDeviceVector::*begin_imagevec)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDeviceBbox::*begin_imagebbox)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDeviceClip::*begin_imageclip)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDeviceCompositeAlpha::*begin_imagecra)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDeviceCPathAccum::*begin_imagepathac)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDeviceMaskClip::*begin_imagemac)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDeviceMem::*begin_imagemem)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDevicePatternAccum::*begin_imagepatta)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDeviceRopTexture::*begin_imageropt)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDevicePsdf::*begin_imagepsdf)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDevicePDF::*begin_imagepdf)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
		int (XWPSDeviceDVI::*begin_imagedvi)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **);
	}begin_image_;
	
	union
	{
		int (XWPSDevice::*image_data)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDeviceHit::*image_datahit)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDeviceForward::*image_datafw)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDeviceNull::*image_datanull)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDeviceVector::*image_datavec)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDeviceBbox::*image_databbox)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDeviceClip::*image_dataclip)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDeviceCompositeAlpha::*image_datacra)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDeviceCPathAccum::*image_datapathac)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDeviceMaskClip::*image_datamac)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDeviceMem::*image_datamem)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDevicePatternAccum::*image_datapatta)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDeviceRopTexture::*image_dataropt)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDevicePsdf::*image_datapsdf)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDevicePDF::*image_datapdf)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
		int (XWPSDeviceDVI::*image_datadvi)(XWPSImageEnumCommon * ,
		      							        const uchar ** ,
		      							        int , 
		      							        uint , 
		      							        int );
	}image_data_;
	
	union
	{
		int (XWPSDevice::*end_image)(XWPSImageEnumCommon *, bool);
		int (XWPSDeviceHit::*end_imagehit)(XWPSImageEnumCommon *, bool);
		int (XWPSDeviceForward::*end_imagefw)(XWPSImageEnumCommon *, bool);
		int (XWPSDeviceNull::*end_imagenull)(XWPSImageEnumCommon *, bool);
		int (XWPSDeviceVector::*end_imagevec)(XWPSImageEnumCommon *, bool);
		int (XWPSDeviceBbox::*end_imagebbox)(XWPSImageEnumCommon *, bool);
		int (XWPSDeviceClip::*end_imageclip)(XWPSImageEnumCommon *, bool);
		int (XWPSDeviceCompositeAlpha::*end_imagecra)(XWPSImageEnumCommon *, bool);
		int (XWPSDeviceCPathAccum::*end_imagepathac)(XWPSImageEnumCommon *, bool);
		int (XWPSDeviceMaskClip::*end_imagemac)(XWPSImageEnumCommon *, bool);
		int (XWPSDeviceMem::*end_imagemem)(XWPSImageEnumCommon *, bool);
		int (XWPSDevicePatternAccum::*end_imagepatta)(XWPSImageEnumCommon *, bool);
		int (XWPSDeviceRopTexture::*end_imageropt)(XWPSImageEnumCommon *, bool);
		int (XWPSDevicePsdf::*end_imagepsdf)(XWPSImageEnumCommon *, bool);
		int (XWPSDevicePDF::*end_imagepdf)(XWPSImageEnumCommon *, bool);
		int (XWPSDeviceDVI::*end_imagedvi)(XWPSImageEnumCommon *, bool);
	}end_image_;
	
	union
	{
		int (XWPSDevice::*strip_tile_rectangle)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDeviceHit::*strip_tile_rectanglehit)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDeviceForward::*strip_tile_rectanglefw)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDeviceNull::*strip_tile_rectanglenull)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDeviceVector::*strip_tile_rectanglevec)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDeviceBbox::*strip_tile_rectanglebbox)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDeviceClip::*strip_tile_rectangleclip)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDeviceCompositeAlpha::*strip_tile_rectanglecra)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDeviceCPathAccum::*strip_tile_rectanglepathac)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDeviceMaskClip::*strip_tile_rectanglemac)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDeviceMem::*strip_tile_rectanglemem)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDevicePatternAccum::*strip_tile_rectanglepatta)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDeviceRopTexture::*strip_tile_rectangleropt)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDevicePsdf::*strip_tile_rectanglepsdf)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDevicePDF::*strip_tile_rectanglepdf)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
		int (XWPSDeviceDVI::*strip_tile_rectangledvi)(XWPSStripBitmap * ,
			      										          int x, 
			      										          int y, 
			      										          int w, 
			      										          int h,
			      										          ulong, 
			      										          ulong,
			      										          int, 
			      										          int);
	}strip_tile_rectangle_;
	
	union
	{
		int (XWPSDevice::*strip_copy_rop)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDeviceHit::*strip_copy_rophit)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDeviceForward::*strip_copy_ropfw)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDeviceNull::*strip_copy_ropnull)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDeviceVector::*strip_copy_ropvec)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDeviceBbox::*strip_copy_ropbbox)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDeviceClip::*strip_copy_ropclip)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDeviceCompositeAlpha::*strip_copy_ropcra)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDeviceCPathAccum::*strip_copy_roppathac)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDeviceMaskClip::*strip_copy_ropmac)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDeviceMem::*strip_copy_ropmem)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDevicePatternAccum::*strip_copy_roppatta)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDeviceRopTexture::*strip_copy_ropropt)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDevicePsdf::*strip_copy_roppsdf)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDevicePDF::*strip_copy_roppdf)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
		int (XWPSDeviceDVI::*strip_copy_ropdvi)(const uchar *, 
	                                  int , 
	                                  uint , 
	                                  ulong ,
	                                  const ulong *,
	                                  XWPSStripBitmap *, 
	                                  const ulong *,
	                                  int , 
	                                  int , 
	                                  int , 
	                                  int ,
	                                  int , 
	                                  int , 
	                                  ulong);
	}strip_copy_rop_;
	
	union
	{
		void (XWPSDevice::*get_clipping_box)(XWPSFixedRect*);
		void (XWPSDeviceHit::*get_clipping_boxhit)(XWPSFixedRect*);
		void (XWPSDeviceForward::*get_clipping_boxfw)(XWPSFixedRect*);
		void (XWPSDeviceNull::*get_clipping_boxnull)(XWPSFixedRect*);
		void (XWPSDeviceVector::*get_clipping_boxvec)(XWPSFixedRect*);
		void (XWPSDeviceBbox::*get_clipping_boxbbox)(XWPSFixedRect*);
		void (XWPSDeviceClip::*get_clipping_boxclip)(XWPSFixedRect*);
		void (XWPSDeviceCompositeAlpha::*get_clipping_boxcra)(XWPSFixedRect*);
		void (XWPSDeviceCPathAccum::*get_clipping_boxpathac)(XWPSFixedRect*);
		void (XWPSDeviceMaskClip::*get_clipping_boxmac)(XWPSFixedRect*);
		void (XWPSDeviceMem::*get_clipping_boxmem)(XWPSFixedRect*);
		void (XWPSDevicePatternAccum::*get_clipping_boxpatta)(XWPSFixedRect*);
		void (XWPSDeviceRopTexture::*get_clipping_boxropt)(XWPSFixedRect*);
		void (XWPSDevicePsdf::*get_clipping_boxpsdf)(XWPSFixedRect*);
		void (XWPSDevicePDF::*get_clipping_boxpdf)(XWPSFixedRect*);
		void (XWPSDeviceDVI::*get_clipping_boxdvi)(XWPSFixedRect*);
	}get_clipping_box_;
	
	union
	{
		int  (XWPSDevice::*begin_typed_image)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDeviceHit::*begin_typed_imagehit)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDeviceForward::*begin_typed_imagefw)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDeviceNull::*begin_typed_imagenull)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDeviceVector::*begin_typed_imagevec)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDeviceBbox::*begin_typed_imagebbox)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDeviceClip::*begin_typed_imageclip)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDeviceCompositeAlpha::*begin_typed_imagecra)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDeviceCPathAccum::*begin_typed_imagepathac)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDeviceMaskClip::*begin_typed_imagemac)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDeviceMem::*begin_typed_imagemem)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDevicePatternAccum::*begin_typed_imagepatta)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDeviceRopTexture::*begin_typed_imageropt)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDevicePsdf::*begin_typed_imagepsdf)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDevicePDF::*begin_typed_imagepdf)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
		int (XWPSDeviceDVI::*begin_typed_imagedvi)(XWPSImagerState * ,
			                                  XWPSMatrix *, 
			                                  XWPSImageCommon *,
			   											          XWPSIntRect * ,
			                                  XWPSDeviceColor * ,
			                                  XWPSClipPath * , 
			                                  XWPSImageEnumCommon ** );
	}begin_typed_image_;
	
	union
	{
		int  (XWPSDevice::*get_bits_rectangle)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDeviceHit::*get_bits_rectanglehit)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDeviceForward::*get_bits_rectanglefw)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDeviceNull::*get_bits_rectanglenull)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDeviceVector::*get_bits_rectanglevec)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDeviceBbox::*get_bits_rectanglebbox)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDeviceClip::*get_bits_rectangleclip)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDeviceCompositeAlpha::*get_bits_rectanglecra)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDeviceCPathAccum::*get_bits_rectanglepathac)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDeviceMaskClip::*get_bits_rectanglemac)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDeviceMem::*get_bits_rectanglemem)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDevicePatternAccum::*get_bits_rectanglepatta)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDeviceRopTexture::*get_bits_rectangleropt)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDevicePsdf::*get_bits_rectanglepsdf)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDevicePDF::*get_bits_rectanglepdf)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
		int (XWPSDeviceDVI::*get_bits_rectangledvi)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **);
	}get_bits_rectangle_;
	
	union
	{
		int  (XWPSDevice::*map_color_rgb_alpha)(ulong, ushort *);
		int (XWPSDeviceHit::*map_color_rgb_alphahit)(ulong, ushort *);
		int (XWPSDeviceForward::*map_color_rgb_alphafw)(ulong, ushort *);
		int (XWPSDeviceNull::*map_color_rgb_alphanull)(ulong, ushort *);
		int (XWPSDeviceVector::*map_color_rgb_alphavec)(ulong, ushort *);
		int (XWPSDeviceBbox::*map_color_rgb_alphabbox)(ulong, ushort *);
		int (XWPSDeviceClip::*map_color_rgb_alphaclip)(ulong, ushort *);
		int (XWPSDeviceCompositeAlpha::*map_color_rgb_alphacra)(ulong, ushort *);
		int (XWPSDeviceCPathAccum::*map_color_rgb_alphapathac)(ulong, ushort *);
		int (XWPSDeviceMaskClip::*map_color_rgb_alphamac)(ulong, ushort *);
		int (XWPSDeviceMem::*map_color_rgb_alphamem)(ulong, ushort *);
		int (XWPSDevicePatternAccum::*map_color_rgb_alphapatta)(ulong, ushort *);
		int (XWPSDeviceRopTexture::*map_color_rgb_alpharopt)(ulong, ushort *);
		int (XWPSDevicePsdf::*map_color_rgb_alphapsdf)(ulong, ushort *);
		int (XWPSDevicePDF::*map_color_rgb_alphapdf)(ulong, ushort *);
		int (XWPSDeviceDVI::*map_color_rgb_alphadvi)(ulong, ushort *);
	}map_color_rgb_alpha_;
	
	union
	{
		int  (XWPSDevice::*create_compositor)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDeviceHit::*create_compositorhit)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDeviceForward::*create_compositorfw)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDeviceNull::*create_compositornull)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDeviceVector::*create_compositorvec)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDeviceBbox::*create_compositorbbox)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDeviceClip::*create_compositorclip)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDeviceCompositeAlpha::*create_compositorcra)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDeviceCPathAccum::*create_compositorpathac)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDeviceMaskClip::*create_compositormac)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDeviceMem::*create_compositormem)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDevicePatternAccum::*create_compositorpatta)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDeviceRopTexture::*create_compositorropt)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDevicePsdf::*create_compositorpsdf)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDevicePDF::*create_compositorpdf)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
		int (XWPSDeviceDVI::*create_compositordvi)(XWPSDevice **, 
		                                    XWPSComposite *,
		                                    XWPSImagerState *);
	}create_compositor_;
	
	union
	{
		int  (XWPSDevice::*get_hardware_params)(XWPSContextState * , XWPSParamList *);
		int (XWPSDeviceHit::*get_hardware_paramshit)(XWPSContextState * , XWPSParamList *);
		int (XWPSDeviceForward::*get_hardware_paramsfw)(XWPSContextState * , XWPSParamList *);
		int (XWPSDeviceNull::*get_hardware_paramsnull)(XWPSContextState * , XWPSParamList *);
		int (XWPSDeviceVector::*get_hardware_paramsvec)(XWPSContextState * , XWPSParamList *);
		int (XWPSDeviceBbox::*get_hardware_paramsbbox)(XWPSContextState * , XWPSParamList *);
		int (XWPSDeviceClip::*get_hardware_paramsclip)(XWPSContextState * , XWPSParamList *);
		int (XWPSDeviceCompositeAlpha::*get_hardware_paramscra)(XWPSContextState * , XWPSParamList *);
		int (XWPSDeviceCPathAccum::*get_hardware_paramspathac)(XWPSContextState * , XWPSParamList *);
		int (XWPSDeviceMaskClip::*get_hardware_paramsmac)(XWPSContextState * , XWPSParamList *);
		int (XWPSDeviceMem::*get_hardware_paramsmem)(XWPSContextState * , XWPSParamList *);
		int (XWPSDevicePatternAccum::*get_hardware_paramspatta)(XWPSContextState * , XWPSParamList *);
		int (XWPSDeviceRopTexture::*get_hardware_paramsropt)(XWPSContextState * , XWPSParamList *);
		int (XWPSDevicePsdf::*get_hardware_paramspsdf)(XWPSContextState * , XWPSParamList *);
		int (XWPSDevicePDF::*get_hardware_paramspdf)(XWPSContextState * , XWPSParamList *);
		int (XWPSDeviceDVI::*get_hardware_paramsdvi)(XWPSContextState * , XWPSParamList *);
	}get_hardware_params_;
	
	union
	{
		int  (XWPSDevice::*text_begin)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDeviceHit::*text_beginhit)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDeviceForward::*text_beginfw)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDeviceNull::*text_beginnull)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDeviceVector::*text_beginvec)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDeviceBbox::*text_beginbbox)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDeviceClip::*text_beginclip)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDeviceCompositeAlpha::*text_begincra)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDeviceCPathAccum::*text_beginpathac)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDeviceMaskClip::*text_beginmac)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDeviceMem::*text_beginmem)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDevicePatternAccum::*text_beginpatta)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDeviceRopTexture::*text_beginropt)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDevicePsdf::*text_beginpsdf)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDevicePDF::*text_beginpdf)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
		int (XWPSDeviceDVI::*text_begindvi)(XWPSImagerState * ,
		    								         XWPSTextParams *, 
		    								         XWPSFont *,
		    								         XWPSPath *, 
		    								         XWPSDeviceColor * ,
		    								         XWPSClipPath *,
		    								         XWPSTextEnum **);
	}text_begin_;
};

struct PSDevicePageProcs
{
	int (XWPSDevice::*install)(XWPSState *);
	int (XWPSDevice::*begin_page)(XWPSState *);
	int (XWPSDevice::*end_page)(int, XWPSState *);
};

class XWPSDevice : public QObject
{
	Q_OBJECT 
	
public:
	XWPSDevice(QObject * parent = 0);
	XWPSDevice(const char * devname,
	           int w,
	           int h,
	           float xdpi,
	           float ydpi,
	           int nc,
	           int depth,
	           ushort mg, 
	           ushort mc,
	           ushort dg,
	           ushort dc,
	           QObject * parent = 0);
	virtual ~XWPSDevice();
	
	int beginImage(XWPSImagerState *pis, 
	                       XWPSImage1 *pim,
	                       PSImageFormat format, 
	                       XWPSIntRect *prect,
	                       XWPSDeviceColor *pdcolor, 
	                       XWPSClipPath *pcpath,
	                       XWPSImageEnumCommon **pinfo);
	int beginImageDefault(XWPSImagerState *pis, 
	                       XWPSImage1 *pim,
	                       PSImageFormat format, 
	                       XWPSIntRect *prect,
	                       XWPSDeviceColor *pdcolor, 
	                       XWPSClipPath *pcpath,
	                       XWPSImageEnumCommon **pinfo);
	int beginImageNo(XWPSImagerState *, 
	                       XWPSImage1 *,
	                       PSImageFormat , 
	                       XWPSIntRect *,
	                       XWPSDeviceColor *, 
	                       XWPSClipPath *,
	                       XWPSImageEnumCommon **) {return -1;}
	int beginPage(XWPSState *pgs);
	int beginPageDefault(XWPSState * ) {return 0;}
	int beginPageNo(XWPSState *) {return -1;}
	int beginTypedImage(XWPSImagerState * pis,
			                        XWPSMatrix *pmat, 
			                        XWPSImageCommon *pic,
			   											XWPSIntRect * prect,
			                        XWPSDeviceColor * pdcolor,
			                        XWPSClipPath * pcpath, 
			                        XWPSImageEnumCommon ** pinfo);
	int beginTypedImageDefault(XWPSImagerState * pis,
			                        XWPSMatrix *pmat, 
			                        XWPSImageCommon *pic,
			   											XWPSIntRect * prect,
			                        XWPSDeviceColor * pdcolor,
			                        XWPSClipPath * pcpath, 
			                        XWPSImageEnumCommon ** pinfo);
	int beginTypedImageNo(XWPSImagerState * ,
			                        XWPSMatrix *, 
			                        XWPSImageCommon *,
			   											XWPSIntRect * ,
			                        XWPSDeviceColor * ,
			                        XWPSClipPath * , 
			                        XWPSImageEnumCommon ** ) {return -1;}
	ulong getBlack();
	
	int close();
	int closeDefault() {return 0;}
	int gbcolorForDevice() 
	  {return (color_info.num_components == 4 ? GB_COLORS_CMYK : 
	  	color_info.num_components == 3 ? GB_COLORS_RGB : GB_COLORS_GRAY);}
	int copyAlpha(const uchar *data, 
	                      int data_x,
	                      int raster, 
	                      ulong id, 
	                      int x, 
	                      int y, 
	                      int widthA, 
	                      int heightA,
	                      ulong color, 
	                      int depthA);
	int copyAlphaDefault(const uchar *data, 
	                      int data_x,
	                      int raster, 
	                      ulong id, 
	                      int x, 
	                      int y, 
	                      int widthA, 
	                      int heightA,
	                      ulong color, 
	                      int depth);
	int copyAlphaNo(const uchar *, 
	                      int ,
	                      int , 
	                      ulong , 
	                      int , 
	                      int , 
	                      int , 
	                      int ,
	                      ulong , 
	                      int ) {return -1;}
	int copyColor(const uchar *data, 
	                      int dx, 
	                      int raster, 
	                      ulong id,
	                      int x, 
	                      int y, 
	                      int w, 
	                      int h);
	int copyColorDefault(const uchar *data, 
	                      int dx, 
	                      int raster, 
	                      ulong id,
	                      int x, 
	                      int y, 
	                      int w, 
	                      int h);
	int copyColorNo(const uchar *, 
	                      int , 
	                      int , 
	                      ulong ,
	                      int , 
	                      int , 
	                      int , 
	                      int )  {return -1;}
	void copyColorParams(XWPSDevice * proto);
	void copyColorProcs(XWPSDevice *targetA);
	virtual int  copyDevice(XWPSDevice **pnew);
					int  copyDevice2(XWPSDevice ** pnew_dev, 
					                 bool keep_open);
	        void copyDeviceParam(XWPSDevice * proto);
	        void copyDeviceProc(XWPSDevice * proto);
	int copyMono(const uchar *data, 
	                     int dx, 
	                     int raster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one);
	int copyMonoUnaligned(const uchar * data,
	    								  int dx, 
	    								  int raster, 
	    								  ulong id, 
	    								  int x, 
	    								  int y, 
	    								  int w, 
	    								  int h,
		       							ulong zero, 
		       							ulong one);
	int copyMonoDefault(const uchar *data, 
	                     int dx, 
	                     int raster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one);
	int copyMonoNo(const uchar *, 
	                     int , 
	                     int , 
	                     ulong ,
	                     int , 
	                     int , 
	                     int , 
	                     int ,
	                     ulong , 
	                     ulong ) {return -1;}
	void copyParams(XWPSDevice * proto);
	int copyRop(const uchar * sdata, 
	                    int sourcex, 
	                    uint sraster, 
	                    ulong id,
		                  const ulong * scolors,
	                    XWPSTileBitmap * texture, 
	                    const ulong * tcolors,
		                  int x, 
		                  int y, 
		                  int widthA, 
		                  int heightA,
		                  int phase_x, 
		                  int phase_y, 
		                  ulong lop);
	int copyRopDefault(const uchar * sdata, 
	                    int sourcex, 
	                    uint sraster, 
	                    ulong id,
		                  const ulong * scolors,
	                    XWPSTileBitmap * texture, 
	                    const ulong * tcolors,
		                  int x, 
		                  int y, 
		                  int widthA, 
		                  int heightA,
		                  int phase_x, 
		                  int phase_y, 
		                  ulong lop);
	int copyRopNo(const uchar * , 
	                    int , 
	                    uint , 
	                    ulong ,
		                  const ulong * ,
	                    XWPSTileBitmap * , 
	                    const ulong * ,
		                  int , 
		                  int , 
		                  int , 
		                  int ,
		                  int , 
		                  int , 
		                  ulong );
	int createCompositor(XWPSDevice **pcdev, 
		                   XWPSComposite *pcte,
		                   XWPSImagerState *pis);
	int createCompositorDefault(XWPSDevice **pcdev, 
		                   XWPSComposite *pcte,
		                   XWPSImagerState *pis);
	int createCompositorNo(XWPSDevice **, 
		                   XWPSComposite *,
		                   XWPSImagerState *);
	int createCompositorNull(XWPSDevice **pcdev, 
		                   XWPSComposite *,
		                   XWPSImagerState *);
	
	void decacheColors();
	int decRef() { return --refCount; }
	int drawLine(int x0, 
	                     int y0, 
	                     int x1, 
	                     int y1, 
	                     ulong color);
	int drawLineDefault(int , 
	                     int , 
	                     int , 
	                     int , 
	                     ulong ){return -1;}
  int drawThinLine(long fx0, 
                           long fy0, 
                           long fx1, 
                           long fy1,
                           XWPSDeviceColor *pdevc, 
                           ulong lop);
  int drawThinLineDefault(long fx0, 
                           long fy0, 
                           long fx1, 
                           long fy1,
                           XWPSDeviceColor *pdevc, 
                           ulong lop);
	int drawThinLineNo(long , 
                           long , 
                           long , 
                           long ,
                           XWPSDeviceColor *, 
                           ulong ) {return -1;}
                           
	int endImage(XWPSImageEnumCommon * info, bool draw_last);
	int endImageDefault(XWPSImageEnumCommon * info, bool draw_last);
	int endImageNo(XWPSImageEnumCommon * , bool ) {return -1;}
	int endPage(int reason, XWPSState * pgs);
	int endPageDefault(int reason, XWPSState * );
		
	int  fillMask(const uchar * data, 
	                      int sourcex, 
	                      int raster, 
	                      ulong id,
		   									int x, 
		   									int y, 
		   									int w, 
		   									int h,
		   									XWPSDeviceColor * pdcolor, 
		   									int depth,
		   									ulong lop, 
		   									XWPSClipPath *pcpath);
	int  fillMaskDefault(const uchar * data, 
	                      int dx, 
	                      int raster, 
	                      ulong id,
		   									int x, 
		   									int y, 
		   									int w, 
		   									int h,
		   									XWPSDeviceColor * pdcolor, 
		   									int depth,
		   									ulong lop, 
		   									XWPSClipPath *pcpath);
	int  fillMaskNo(const uchar * , 
	                      int , 
	                      int , 
	                      ulong ,
		   									int , 
		   									int , 
		   									int , 
		   									int ,
		   									XWPSDeviceColor * , 
		   									int ,
		   									ulong , 
		   									XWPSClipPath *) {return -1;}
	int  fillParallelogram(long px, 
	                               long py, 
	                               long ax, 
	                               long ay, 
	                               long bx, 
	                               long by,
	                               XWPSDeviceColor *pdevc, 
	                               ulong lop);
	int  fillParallelogramDefault(long px, 
	                               long py, 
	                               long ax, 
	                               long ay, 
	                               long bx, 
	                               long by,
	                               XWPSDeviceColor *pdevc, 
	                               ulong lop);
	int  fillParallelogramNo(long , 
	                               long , 
	                               long , 
	                               long , 
	                               long , 
	                               long ,
	                               XWPSDeviceColor *, 
	                               ulong ) {return -1;}
	int  fillPath(XWPSImagerState * pis, 
	                      XWPSPath * ppath,
		                    XWPSFillParams * params,
	                      XWPSDeviceColor * pdevc, 
	                      XWPSClipPath * pcpath);
	int  fillPathDefault(XWPSImagerState * pis, 
	                      XWPSPath * ppath,
		                    XWPSFillParams * params,
	                      XWPSDeviceColor * pdevc, 
	                      XWPSClipPath * pcpath);
	int  fillPathNo(XWPSImagerState * , 
	                      XWPSPath * ,
		                    XWPSFillParams * ,
	                      XWPSDeviceColor * , 
	                      XWPSClipPath * ) {return -1;}
	int fillRectangle(int x, int y, int w, int h, ulong color);
	int fillRectangleNo(int , int , int  , int  , ulong ) {return 0;}
	int fillTrapezoid( XWPSFixedEdge *left, 
	                          XWPSFixedEdge *right,
	                          long ybot, 
	                          long ytop, 
	                          bool swap_axes,
	                          XWPSDeviceColor *pdevc, 
	                          ulong lop);
	int fillTrapezoidDefault(XWPSFixedEdge *left, 
	                          XWPSFixedEdge *right,
	                          long ybot, 
	                          long ytop, 
	                          bool swap_axes,
	                          XWPSDeviceColor *pdevc, 
	                          ulong lop);
	int fillTrapezoidNo(XWPSFixedEdge *, 
	                          XWPSFixedEdge *,
	                          long , 
	                          long , 
	                          bool ,
	                          XWPSDeviceColor *, 
	                          ulong ) {return -1;}
	int fillTriangle(long px, 
	                         long py, 
	                         long ax, 
	                         long ay, 
	                         long bx, 
	                         long by,
	                         XWPSDeviceColor *pdevc, 
	                         ulong lop);
	int fillTriangleDefault(long px, 
	                         long py, 
	                         long ax, 
	                         long ay, 
	                         long bx, 
	                         long by,
	                         XWPSDeviceColor *pdevc, 
	                         ulong lop);
	int fillTriangleNo(long , 
	                         long , 
	                         long , 
	                         long , 
	                         long , 
	                         long ,
	                         XWPSDeviceColor *, 
	                         ulong ) {return -1;}
	
	int getAlphaBits(PSGraphicsObjectType type);
	int getAlphaBitsDefault(PSGraphicsObjectType type);
	int getBand(int y, int *band_start);
	int getBandDefault(int , int *) {return 0;}
	int getBits(int y, uchar * data, uchar ** actual_data);
	int getBitsDefault(int y, uchar * data, uchar ** actual_data);
	int getBitsNo(int , uchar * , uchar ** ) {return -1;}
	int getBitsCopy(int x, 
	                int w, 
	                int h,
	                XWPSGetBitsParams * params,
	                XWPSGetBitsParams *stored,
	                const uchar * src_base, 
	                uint dev_raster);
	int getBitsRectangle(XWPSIntRect *prect,
	                             XWPSGetBitsParams *params, 
	                             XWPSIntRect **unread);
	int getBitsRectangleDefault(XWPSIntRect *prect,
	                             XWPSGetBitsParams *params, 
	                             XWPSIntRect **unread);
	int getBitsRectangleNo(XWPSIntRect *,
	                             XWPSGetBitsParams *, 
	                             XWPSIntRect **) {return -1;}
  int  getBitsReturnPointer(int x, 
	                          int h,
	                          XWPSGetBitsParams *params,
	                          XWPSGetBitsParams *stored,
	                          uchar * stored_base);
	void getClipingBox(XWPSFixedRect*pbox);
	void getClipingBoxDefault(XWPSFixedRect*pbox);
	void getClipingBoxLarge(XWPSFixedRect*pbox);
	int  getDeviceOrHWParams(XWPSParamList * plist, bool is_hardware);
	XWPSContextState * getPSContextState() {return context_state;}
	int  getHardwareParams(XWPSContextState * ctx, XWPSParamList *plist);
	int  getHardwareParamsDefault(XWPSContextState * , XWPSParamList *) {return 0;}	
	void getInitialMatrix(XWPSMatrix *pmat);
	void getInitialMatrixDefault(XWPSMatrix *pmat);
	void getInitialMatrixUpright(XWPSMatrix *pmat);
	XWPSDevice * getPageDevice();
	XWPSDevice * getPageDeviceDefault() {return 0;}
	XWPSDevice * getPageDevicePageDevice() {return this;}
	int  getParams(XWPSContextState * ctx, XWPSParamList *plist);
	int  getParamsDefault(XWPSContextState * ctx, XWPSParamList *plist);
	virtual uint getRaster(bool pad);
	
	XWPSDevice * getXFontDevice();
	XWPSDevice * getXFontDeviceDefault() {return this;}
	
	bool hasColor() {return (color_info.num_components > 1);}
	
	int  imageData(XWPSImageEnumCommon * info,
		      							 const uchar ** plane_data,
		      							 int data_x, 
		      							 uint raster, 
		      							 int heightA);
	int  imageDataDefault(XWPSImageEnumCommon * info,
		      							 const uchar ** plane_data,
		      							 int data_x, 
		      							 uint raster, 
		      							 int heightA);
	int  incRef() { return ++refCount; }	
	int install(XWPSState * pgs);
	int installDefault(XWPSState * ) {return 0;}
	virtual bool isABuf() {return false;}
	virtual bool isMemory() {return false;}
	virtual bool isNull() {return false;}
	bool isTrueColor();
	
	ulong mapCMYKColor(ushort c, ushort m, ushort y, ushort k);
	ulong mapCMYKColorCMYK1Bit(ushort c, ushort m, ushort y, ushort k);
	ulong mapCMYKColorCMYK8Bit(ushort c, ushort m, ushort y, ushort k);
	ulong mapCMYKColorDefault(ushort c, ushort m, ushort y, ushort k);
	int   mapColorRGB(ulong color, ushort* prgb);
	int   mapColorRGBDefault(ulong color, ushort* prgb);
	int   mapColorRGBAlpha(ulong color, ushort * prgba);
	int   mapColorRGBGrayDefault(ulong color, ushort* prgb);
	int   mapColorRGBRGBDefault(ulong color, ushort* prgb);
	int   mapColorRGBAlphaDefault(ulong color, ushort * prgba);
	int   mapColorRGBCMYK1Bit(ulong color, ushort* prgb);
	int   mapColorRGBCMYK8Bit(ulong color, ushort* prgb);
	ulong mapRGBAlphaColor(ushort r, ushort g, ushort b, ushort a);
	ulong mapRGBAlphaColorDefault(ushort r, ushort g, ushort b, ushort );
	ulong mapRGBColor(ushort r, ushort g, ushort b);
	ulong mapRGBColorDefault(ushort r, ushort g, ushort b);
	ulong mapRGBColorGrayDefault(ushort r, ushort g, ushort b);
	ulong mapRGBColorRGBDefault(ushort r, ushort g, ushort b);
	
	int open();
	int openDefault() {return 0;}
	int outputPage(int num_copies, int flush);
	int outputPageDefault(int num_copies, int flush);
	
	int putDeviceParams(XWPSContextState * ctx, XWPSParamList * plist);
	int putParams(XWPSContextState * ctx, XWPSParamList *plist);
	int putParamsDefault(XWPSContextState * ctx, XWPSParamList *plist);
	
	void setMediaSize(float media_width, float media_height);
	void setPaperSize(int w, int h);
	void setPSContextState(XWPSContextState * ctx) {context_state=ctx;}	
	void setResolution(float x_dpi, float y_dpi);
	void setWidthHeight(int widthA, int heightA);
	int stripCopyRop(const uchar *sdata, 
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
	int stripCopyRopDefault(const uchar *, 
	                         int , 
	                         uint , 
	                         ulong ,
	                         const ulong *,
	                         XWPSStripBitmap *, 
	                         const ulong *,
	                         int , 
	                         int , 
	                         int , 
	                         int ,
	                         int , 
	                         int , 
	                         ulong ) {return 0;}
	int stripCopyRopNo(const uchar *, 
	                         int , 
	                         uint , 
	                         ulong ,
	                         const ulong *,
	                         XWPSStripBitmap *, 
	                         const ulong *,
	                         int , 
	                         int , 
	                         int , 
	                         int ,
	                         int , 
	                         int , 
	                         ulong ) {return -1;}	
	int stripTileRectangle(XWPSStripBitmap * tiles,
			      										 int x, 
			      										 int y, 
			      										 int w, 
			      										 int h,
			      										 ulong color0, 
			      										 ulong color1,
			      										 int px, 
			      										 int py);
	int stripTileRectangleDefault(XWPSStripBitmap * tiles,
			      										 int x, 
			      										 int y, 
			      										 int w, 
			      										 int h,
			      										 ulong color0, 
			      										 ulong color1,
			      										 int px, 
			      										 int py);
	int strokePath(XWPSImagerState * pis, 
	                       XWPSPath * ppath,
		                     XWPSStrokeParams * params,
	                       XWPSDeviceColor * pdcolor, 
	                       XWPSClipPath *pcpath);
	int strokePathDefault(XWPSImagerState * pis, 
	                       XWPSPath * ppath,
		                     XWPSStrokeParams * params,
	                       XWPSDeviceColor * pdcolor, 
	                       XWPSClipPath *pcpath);
	int strokePathOnly(XWPSPath * ppath,
	                   XWPSPath * to_path, 
	                   XWPSImagerState * pis, 
	                   XWPSStrokeParams * params,
	                   XWPSDeviceColor * pdevc, 
	                   XWPSClipPath *pcpath);
	int syncOutput();
	int syncOutputDefault() {return 0;}
	                      
	int textBegin(XWPSImagerState * pis,
		    								XWPSTextParams *text, 
		    								XWPSFont * font,
		    								XWPSPath * path, 
		    								XWPSDeviceColor * pdcolor,
		    								XWPSClipPath * pcpath,
		    								XWPSTextEnum ** ppte);
	int textBeginDefault(XWPSImagerState * pis,
		    								XWPSTextParams *text, 
		    								XWPSFont * font,
		    								XWPSPath * path, 
		    								XWPSDeviceColor * pdcolor,
		    								XWPSClipPath * pcpath,
		    								XWPSTextEnum ** ppte);
	int textBeginGX(XWPSImagerState * pis,
		    								XWPSTextParams *text, 
		    								XWPSFont * font,
		    								XWPSPath * path, 
		    								XWPSDeviceColor * pdcolor,
		    								XWPSClipPath * pcpath,
		    								XWPSTextEnum ** ppte);
		    								
	int tileRectangle(XWPSTileBitmap *tile, 
	                          int x, 
	                          int y, 
	                          int widthA, 
	                          int heightA,
                            ulong color0, 
                            ulong color1,
                            int phase_x, 
                            int phase_y);
	int tileRectangleDefault(XWPSTileBitmap *tile, 
	                          int x, 
	                          int y, 
	                          int widthA, 
	                          int heightA,
                            ulong color0, 
                            ulong color1,
                            int phase_x, 
                            int phase_y);
		    								
	ulong getWhite();
	
public:
	int refCount;
	int params_size;
	PSDeviceProcs procs;
	const char *dname;
	bool retained;
	bool is_open;
	int max_fill_band;
	XWPSDevColorInfo color_info;
	XWPSDevCachedColors cached_colors;
	int width;
	int height;
	float MediaSize[2];
	float ImagingBBox[4];
	bool ImagingBBox_set;
	float HWResolution[2];
	float MarginsHWResolution[2];
	float Margins[2];
	float HWMargins[4];
	long PageCount;
	long ShowpageCount;
	int NumCopies;
	bool NumCopies_set;
	bool IgnoreNumCopies;
	bool UseCIEColor;	
	
	XWPSContextState * context_state;
	
	PSDevicePageProcs page_procs;
	
protected:	
	int fillLoopByScanLines(XWPSLineList* ll, 
	       									XWPSFillParams * params, 
	       									XWPSDeviceColor * pdevc,
		     									ulong lop, 
		     									XWPSFixedRect * pbox,
													long adjust_left, 
													long adjust_right,
		                      long adjust_below, 
		                      long adjust_above, 
		                      long band_mask);
	int fillLoopByTrapezoids(XWPSLineList * ll,
	       									 XWPSFillParams * params, 
	       									 XWPSDeviceColor * pdevc,
		     									 ulong lop, 
		     									 XWPSFixedRect * pbox,
			                     long adjust_left, 
			                     long adjust_right,
		                       long adjust_below, 
		                       long adjust_above, 
		                       long band_mask);
	int fillSlantAdjust(long xlbot, 
	                    long xbot, 
	                    long y,
		                  long xltop, 
		                  long xtop, 
		                  long heightA, 
		                  long adjust_below,
		                  long adjust_above, 
		                  XWPSFixedRect * pbox,
		  							  XWPSDeviceColor * pdevc,
		  							  ulong lop);
	int finishOutputPage(int num_copies, int flush);
	
	void getBitsCopyCMYK1Bit(uchar *dest_line, 
	                         uint dest_raster,
			                     const uchar *src_line, 
			                     uint src_raster,
			                     int src_bit, 
			                     int w, 
			                     int h);
	int  getBitsNativeToStd(int x, 
	                        int w, 
	                        int h,
			                    XWPSGetBitsParams * params,
			                    XWPSGetBitsParams *stored,
			                    const uchar * src_base, 
			                    uint dev_raster,
			                    int x_offset, 
			                    uint raster, 
			                    uint std_raster);
	int  getBitsStdToNative(int x, 
	                        int w, 
	                        int h,
			                    XWPSGetBitsParams * params,
			                    XWPSGetBitsParams *stored,
			                    const uchar * src_base, 
			                    uint dev_raster,
			                    int x_offset, 
			                    uint raster);
	
	int loopFillTrap(long fx0, 
	                 long fw0, 
	                 long fy0,
	                 long fx1, 
	                 long fw1, 
	                 long fh, 
	                 XWPSFixedRect * pbox,
	                 XWPSDeviceColor * pdevc, 
	                 ulong lop);
	
	bool paramHWColorMap(uchar * palette);
	
	bool requestedIncludesStored(XWPSGetBitsParams *requested,
			                         XWPSGetBitsParams *stored);
	
	int strokeAdd(XWPSPath * ppath, 
	              int first, 
	              XWPSPartialLine * plp, 
	              XWPSPartialLine * nplp,
	   					  XWPSDeviceColor * pdevc,
	   						XWPSImagerState * pis, 
	   						XWPSStrokeParams * params,
	   					  XWPSFixedRect * ignore_pbbox, 
	   					  int uniform, 
	   					  PSLineJoin join);
	int strokeFill(XWPSPath * ppath, 
	               int first, 
	               XWPSPartialLine * plp, 
	               XWPSPartialLine * nplp,
	    					 XWPSDeviceColor * pdevc, 
	    				   XWPSImagerState * pis, 
	    				   XWPSStrokeParams * params,
	    					 XWPSFixedRect * pbbox, 
	    					 int uniform, 
	    					 PSLineJoin join);
};

class XWPSDeviceHit : public XWPSDevice
{
	Q_OBJECT 
	
public:
	XWPSDeviceHit(QObject * parent = 0);
	
	int  copyDevice(XWPSDevice **pnew);
	
	int fillRectangleHit(int x, int y, int w, int h, ulong color);
};

class XWPSDeviceForward : public XWPSDevice
{
	Q_OBJECT 
	
public:
	XWPSDeviceForward(QObject * parent = 0);
	XWPSDeviceForward(const char * devname,
	                  int w,
	                  int h,
	                  float xdpi,
	                  float ydpi,
	                  int nc,
	                  int depth,
	                  ushort mg, 
	                  ushort mc,
	                  ushort dg,
	                  ushort dc,
	                  QObject * parent = 0);
	virtual ~XWPSDeviceForward();
	
	int beginImageForward(XWPSImagerState *pis, 
	                       XWPSImage1 *pim,
	                       PSImageFormat format, 
	                       XWPSIntRect *prect,
	                       XWPSDeviceColor *pdcolor, 
	                       XWPSClipPath *pcpath,
	                       XWPSImageEnumCommon **pinfo);
	int beginTypedImageForward(XWPSImagerState * pis,
			                        XWPSMatrix *pmat, 
			                        XWPSImageCommon *pic,
			   											XWPSIntRect * prect,
			                        XWPSDeviceColor * pdcolor,
			                        XWPSClipPath * pcpath, 
			                        XWPSImageEnumCommon ** pinfo);
	                       
	virtual int  copyDevice(XWPSDevice **pnew);
	int  copyRopForward(const uchar * sdata, 
	                    int sourcex, 
	                    uint sraster, 
	                    ulong id,
		                  const ulong * scolors,
	                    XWPSTileBitmap * texture, 
	                    const ulong * tcolors,
		                  int x, 
		                  int y, 
		                  int widthA, 
		                  int heightA,
		                  int phase_x, 
		                  int phase_y, 
		                  ulong lop);
		                  
	int drawThinLineForward(long fx0, 
                           long fy0, 
                           long fx1, 
                           long fy1,
                           XWPSDeviceColor *pdevc, 
                           ulong lop);
                           
	int  fillMaskForward(const uchar * data, 
	                      int sourcex, 
	                      int raster, 
	                      ulong id,
		   									int x, 
		   									int y, 
		   									int w, 
		   									int h,
		   									XWPSDeviceColor * pdcolor, 
		   									int depth,
		   									ulong lop, 
		   									XWPSClipPath *pcpath);
	int  fillParallelogramForward(long px, 
	                               long py, 
	                               long ax, 
	                               long ay, 
	                               long bx, 
	                               long by,
	                               XWPSDeviceColor *pdevc, 
	                               ulong lop);
	int fillPathForward(XWPSImagerState * pis, 
	                      XWPSPath * ppath,
		                    XWPSFillParams * params,
	                      XWPSDeviceColor * pdevc, 
	                      XWPSClipPath * pcpath);
	int fillTrapezoidForward(XWPSFixedEdge *left, 
	                          XWPSFixedEdge *right,
	                          long ybot, 
	                          long ytop, 
	                          bool swap_axes,
	                          XWPSDeviceColor *pdevc, 
	                          ulong lop);
	int fillTriangleForward(long px, 
	                         long py, 
	                         long ax, 
	                         long ay, 
	                         long bx, 
	                         long by,
	                         XWPSDeviceColor *pdevc, 
	                         ulong lop);
	void forwardColorProcs();
	                         
	int  getBandForward(int y, int *band_start);
	int  getBitsForward(int y, uchar * data, uchar ** actual_data);
	int  getBitsRectangleForward(XWPSIntRect *prect,
	                             XWPSGetBitsParams *params, 
	                             XWPSIntRect **unread);
	void getClipingBoxForward(XWPSFixedRect*pbox);
	int  getHardwareParamsForward(XWPSContextState * ctx, XWPSParamList *plist);
	void getInitialMatrixForward(XWPSMatrix *pmat);
	XWPSDevice * getPageDeviceForward();
	int  getParamsForward(XWPSContextState * ctx,XWPSParamList *plist);
	XWPSDevice * getXFontDeviceForward();
	
	ulong mapCMYKColorForward(ushort c, ushort m, ushort y, ushort k);
	int   mapColorRGBForward(ulong color, ushort* prgb);
	int   mapColorRGBAlphaForward(ulong color, ushort * prgba);
	ulong mapRGBAlphaColorForward(ushort r, ushort g, ushort b, ushort alpha);
	ulong mapRGBColorForward(ushort r, ushort g, ushort b);
	
	int outputPageForward(int num_copies, int flush);
	int putParamsForward(XWPSContextState * ctx,XWPSParamList *plist);
	
	void setTarget(XWPSDevice * t);
	
	int stripCopyRopForward(const uchar *sdata, 
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
	int strokePathForward(XWPSImagerState * pis, 
	                       XWPSPath * ppath,
		                     XWPSStrokeParams * params,
	                       XWPSDeviceColor * pdcolor, 
	                       XWPSClipPath *pcpath);
	int syncOutputForward();
	
	int textBeginForward(XWPSImagerState * pis,
		    								XWPSTextParams *text, 
		    								XWPSFont * font,
		    								XWPSPath * path, 
		    								XWPSDeviceColor * pdcolor,
		    								XWPSClipPath * pcpath,
		    								XWPSTextEnum ** ppte);
	
public:
	XWPSDevice *target;
};

class XWPSClipCallbackData
{
public:
	XWPSClipCallbackData(XWPSDevice * dev=0);
	
	int copyAlpha(int xc, int yc, int xec, int yec);
	int copyColor(int xc, int yc, int xec, int yec);
	int copyMono(int xc, int yc, int xec, int yec);
	
	int fillMask(int xc, int yc, int xec, int yec);
	int fillRectangle(int xc, int yc, int xec, int yec);
		
	int stripCopyRop(int xc, int yc, int xec, int yec);
	int stripTileRectangle(int xc, int yc, int xec, int yec);
	
public:
	XWPSDevice *tdev;
	int x, y, w, h;
	ulong color[2];
	const uchar *data;
	int sourcex;
	uint raster;
	int depth;
	XWPSDeviceColor *pdcolor;
	ulong lop;
	XWPSClipPath *pcpath;
	XWPSStripBitmap *tiles;
	XWPSIntPoint phase;
	const ulong *scolors;
	XWPSStripBitmap *textures;
	const ulong *tcolors;
};

class XWPSDeviceNull : public XWPSDeviceForward
{
	Q_OBJECT 
	
public:
	XWPSDeviceNull(QObject * parent = 0);
	~XWPSDeviceNull();
	
	int beginTypedImageDVIMid(XWPSImagerState * pis,
			                     XWPSMatrix *pmat, 
			                     XWPSImageCommon *pic,
			                     XWPSIntRect * prect,
			                     XWPSDeviceColor * pdcolor,
			                     XWPSClipPath * pcpath,
			                     XWPSImageEnumCommon ** pinfo);
	int beginTypedImagePDFMid(XWPSImagerState * pis,
			                     XWPSMatrix *pmat, 
			                     XWPSImageCommon *pic,
			                     XWPSIntRect * prect,
			                     XWPSDeviceColor * pdcolor,
			                     XWPSClipPath * pcpath,
			                     XWPSImageEnumCommon ** pinfo);
	
	int copyAlphaNull(const uchar *, 
	                      int ,
	                      int , 
	                      ulong , 
	                      int , 
	                      int , 
	                      int , 
	                      int ,
	                      ulong , 
	                      int ) {return 0;}
	int copyColorNull(const uchar *, 
	                      int , 
	                      int , 
	                      ulong ,
	                      int , 
	                      int , 
	                      int , 
	                      int )  {return 0;}
	int  copyDevice(XWPSDevice **pnew);
	int copyMonoNull(const uchar *, 
	                     int , 
	                     int , 
	                     ulong ,
	                     int , 
	                     int , 
	                     int , 
	                     int ,
	                     ulong , 
	                     ulong ) {return 0;}
	int copyRopNull(const uchar * , 
	                    int , 
	                    uint , 
	                    ulong ,
		                  const ulong * ,
	                    XWPSTileBitmap * , 
	                    const ulong * ,
		                  int , 
		                  int , 
		                  int , 
		                  int ,
		                  int , 
		                  int , 
		                  ulong ) {return 0;}
		                  
	int drawThinLineNull(long , 
                   long , 
                   long , 
                   long ,
                   XWPSDeviceColor *, 
                   ulong ) {return 0;}
		                  
	int  fillParallelogramNull(long , 
	                       long , 
	                       long , 
	                       long , 
	                       long , 
	                       long ,
	                       XWPSDeviceColor *, 
	                       ulong ) {return 0;}
	int  fillPathNull(XWPSImagerState * , 
	                      XWPSPath * ,
		                    XWPSFillParams * ,
	                      XWPSDeviceColor * , 
	                      XWPSClipPath * ) {return 0;}
	int  fillRectangleNull(int, int, int, int, ulong) {return 0;}
	int fillTrapezoidNull(XWPSFixedEdge *, 
	                  XWPSFixedEdge *,
	                        long , 
	                        long , 
	                        bool ,
	                  XWPSDeviceColor *, 
	                        ulong ) {return 0;}
	int fillTriangleNull(long , 
	                 long , 
	                 long , 
	                 long , 
	                 long , 
	                 long ,
	                 XWPSDeviceColor *, 
	                 ulong ) {return 0;}
	                 
	bool isNull() {return true;}
	
	void makeNullDevice(XWPSDevice *dev);
	
	int putParamsNull(XWPSContextState * ctx,XWPSParamList *plist);
	
	int stripCopyRopNull(const uchar *, 
	                         int , 
	                         uint , 
	                         ulong ,
	                         const ulong *,
	                         XWPSStripBitmap *, 
	                         const ulong *,
	                         int , 
	                         int , 
	                         int , 
	                         int ,
	                         int , 
	                         int , 
	                         ulong ) {return 0;}
	int strokePathNull(XWPSImagerState * , 
	                     XWPSPath * ,
		             XWPSStrokeParams * ,
	               XWPSDeviceColor * , 
	               XWPSClipPath *) {return 0;}
};

struct PSDeviceVectorProcs
{
	union
	{
		int (XWPSDeviceVector::*beginpage)();
		int (XWPSDevicePsdf::*beginpagepsdf)();
		int (XWPSDevicePDF::*beginpagepdf)();
		int (XWPSDeviceDVI::*beginpagedvi)();
	}beginpage_;
	
	union
	{
		int (XWPSDeviceVector::*setlinewidth)(double);
		int (XWPSDevicePsdf::*setlinewidthpsdf)(double);
		int (XWPSDevicePDF::*setlinewidthpdf)(double);
		int (XWPSDeviceDVI::*setlinewidthdvi)(double);
	}setlinewidth_;
	
	union
	{
  	int (XWPSDeviceVector::*setlinecap)(PSLineCap);
  	int (XWPSDevicePsdf::*setlinecappsdf)(PSLineCap);
		int (XWPSDevicePDF::*setlinecappdf)(PSLineCap);
		int (XWPSDeviceDVI::*setlinecapdvi)(PSLineCap);
  }setlinecap_;
  
  union
  {
  	int (XWPSDeviceVector::*setlinejoin)(PSLineJoin);
  	int (XWPSDevicePsdf::*setlinejoinpsdf)(PSLineJoin);
		int (XWPSDevicePDF::*setlinejoinpdf)(PSLineJoin);
		int (XWPSDeviceDVI::*setlinejoindvi)(PSLineJoin);
  }setlinejoin_;
  
  union
  {
  	int (XWPSDeviceVector::*setmiterlimit)(double);
  	int (XWPSDevicePsdf::*setmiterlimitpsdf)(double);
		int (XWPSDevicePDF::*setmiterlimitpdf)(double);
		int (XWPSDeviceDVI::*setmiterlimitdvi)(double);
  }setmiterlimit_;
  
  union
  {
  	int (XWPSDeviceVector::*setdash)(const float *, uint , double);
  	int (XWPSDevicePsdf::*setdashpsdf)(const float *, uint , double);
		int (XWPSDevicePDF::*setdashpdf)(const float *, uint , double);
		int (XWPSDeviceDVI::*setdashdvi)(const float *, uint , double);
  }setdash_;
  
  union
  {
  	int (XWPSDeviceVector::*setflat)(double);
  	int (XWPSDevicePsdf::*setflatpsdf)(double);
		int (XWPSDevicePDF::*setflatpdf)(double);
		int (XWPSDeviceDVI::*setflatdvi)(double);
  }setflat_;
  
  union
  {
  	int (XWPSDeviceVector::*setlogop)(uint, uint);
  	int (XWPSDevicePsdf::*setlogoppsdf)(uint, uint);
		int (XWPSDevicePDF::*setlogoppdf)(uint, uint);
		int (XWPSDeviceDVI::*setlogopdvi)(uint, uint);
  }setlogop_;
  
  union
  {
  	int (XWPSDeviceVector::*setfillcolor)(XWPSDeviceColor *);
  	int (XWPSDevicePsdf::*setfillcolorpsdf)(XWPSDeviceColor *);
		int (XWPSDevicePDF::*setfillcolorpdf)(XWPSDeviceColor *);
		int (XWPSDeviceDVI::*setfillcolordvi)(XWPSDeviceColor *);
  }setfillcolor_;
  
  union
  {
  	int (XWPSDeviceVector::*setstrokecolor)(XWPSDeviceColor *);
  	int (XWPSDevicePsdf::*setstrokecolorpsdf)(XWPSDeviceColor *);
		int (XWPSDevicePDF::*setstrokecolorpdf)(XWPSDeviceColor *);
		int (XWPSDeviceDVI::*setstrokecolordvi)(XWPSDeviceColor *);
	}setstrokecolor_;
	
	union
	{
  	int (XWPSDeviceVector::*dopath)(XWPSPath *,  PSPathType, XWPSMatrix *);
  	int (XWPSDevicePsdf::*dopathpsdf)(XWPSPath *,  PSPathType, XWPSMatrix *);
		int (XWPSDevicePDF::*dopathpdf)(XWPSPath *,  PSPathType, XWPSMatrix *);
		int (XWPSDeviceDVI::*dopathdvi)(XWPSPath *,  PSPathType, XWPSMatrix *);
  }dopath_;
  
  union
  {
  	int (XWPSDeviceVector::*dorect)(long, long, long, long, PSPathType);
  	int (XWPSDevicePsdf::*dorectpsdf)(long, long, long, long, PSPathType);
		int (XWPSDevicePDF::*dorectpdf)(long, long, long, long, PSPathType);
		int (XWPSDeviceDVI::*dorectdvi)(long, long, long, long, PSPathType);
  }dorect_;
  
  union
  {
  	int (XWPSDeviceVector::*beginpath)(PSPathType);
  	int (XWPSDevicePsdf::*beginpathpsdf)(PSPathType);
		int (XWPSDevicePDF::*beginpathpdf)(PSPathType);
		int (XWPSDeviceDVI::*beginpathdvi)(PSPathType);
  }beginpath_;
  
  union
  {
  	int (XWPSDeviceVector::*moveto)(double, double,double, double, PSPathType);
  	int (XWPSDevicePsdf::*movetopsdf)(double, double,double, double, PSPathType);
		int (XWPSDevicePDF::*movetopdf)(double, double,double, double, PSPathType);
		int (XWPSDeviceDVI::*movetodvi)(double, double,double, double, PSPathType);
	}moveto_;
	
	union
	{
  	int (XWPSDeviceVector::*lineto)(double, double, double, double, PSPathType);
  	int (XWPSDevicePsdf::*linetopsdf)(double, double, double, double, PSPathType);
		int (XWPSDevicePDF::*linetopdf)(double, double, double, double, PSPathType);
		int (XWPSDeviceDVI::*linetodvi)(double, double, double, double, PSPathType);
  }lineto_;
  
  union
  {
  	int (XWPSDeviceVector::*curveto)(double, double,double, double, double, double,	double, double, PSPathType);
  	int (XWPSDevicePsdf::*curvetopsdf)(double, double,double, double, double, double,	double, double, PSPathType);
		int (XWPSDevicePDF::*curvetopdf)(double, double,double, double, double, double,	double, double, PSPathType);
		int (XWPSDeviceDVI::*curvetodvi)(double, double,double, double, double, double,	double, double, PSPathType);
  }curveto_;
  
  union
  {
  	int (XWPSDeviceVector::*closepath)(double, double, double, double, PSPathType);
  	int (XWPSDevicePsdf::*closepathpsdf)(double, double, double, double, PSPathType);
		int (XWPSDevicePDF::*closepathpdf)(double, double, double, double, PSPathType);
		int (XWPSDeviceDVI::*closepathdvi)(double, double, double, double, PSPathType);
  }closepath_;
  
  union
  {
  	int (XWPSDeviceVector::*endpath)(PSPathType);
  	int (XWPSDevicePsdf::*endpathpsdf)(PSPathType);
		int (XWPSDevicePDF::*endpathpdf)(PSPathType);
		int (XWPSDeviceDVI::*endpathdvi)(PSPathType);
  }endpath_;
};

enum PSDFVersion
{
    psdf_version_level1 = 1000,	/* Red Book Level 1 */
    psdf_version_level1_color = 1100,	/* Level 1 + colorimage + CMYK color */
    psdf_version_level2 = 2000,	/* Red Book Level 2 */
    psdf_version_level2_plus = 2017,	/* Adobe release 2017 */
    psdf_version_ll3 = 3010	/* LanguageLevel 3, release 3010 */
};

struct PSPSDFSetColorCommands 
{
    const char *setgray;
    const char *setrgbcolor;
    const char *setcmykcolor;
    const char *setcolorspace;
    const char *setcolor;
    const char *setcolorn;
};

enum psdf_auto_rotate_pages 
{
	arp_None,
	arp_All,
	arp_PageByPage
};
  
enum psdf_binding 
{
	binding_Left,
	binding_Right
};
  
enum psdf_default_rendering_intent 
{
	ri_Default,
	ri_Perceptual,
	ri_Saturation,
	ri_RelativeColorimetric,
	ri_AbsoluteColorimetric
};
  
enum psdf_color_conversion_strategy 
{
	ccs_LeaveColorUnchanged,
	ccs_UseDeviceDependentColor,
	ccs_UseDeviceIndependentColor,
	ccs_UseDeviceIndependentColorForImages,
	ccs_sRGB
};
  
enum psdf_transfer_function_info 
{
	tfi_Preserve,
	tfi_Apply,
	tfi_Remove
};
  
enum psdf_ucr_and_bg_info 
{
	ucrbg_Preserve,
	ucrbg_Remove
};
  
enum psdf_cannot_embed_font_policy 
{
	cefp_OK,
	cefp_Warning,
	cefp_Error
};

enum psdf_downsample_type 
{
	ds_Average,
	ds_Bicubic,
	ds_Subsample
} ;
  
struct PSPSDFImageFilterName
{
	const char *pname;
  const PSStreamTemplate *templat;
  PSDFVersion min_version;
};

extern const PSPSDFImageFilterName Poly_filters[];
extern const PSPSDFImageFilterName Mono_filters[];

struct PSPSDFImageParamNames
{
	const char *ACSDict;
  const char *Dict;
  const char *DownsampleType;
  float DownsampleThreshold_default;
  const PSPSDFImageFilterName *filter_names;
  const char *Filter;
  PSParamItem items[8];
};

extern const PSPSDFImageParamNames Color_names;
extern const PSPSDFImageParamNames Gray_names;
extern const PSPSDFImageParamNames Mono_names;

extern const char *const AutoRotatePages_names[];
extern const char *const ColorConversionStrategy_names[];
extern const char *const DownsampleType_names[];
extern const char *const Binding_names[];
extern const char *const DefaultRenderingIntent_names[];
extern const char *const TransferFunctionInfo_names[];
extern const char *const UCRandBGInfo_names[];
extern const char *const CannotEmbedFontPolicy_names[];

struct PSPDFFilterNames
{
	const char *ASCII85Decode;
  const char *ASCIIHexDecode;
  const char *CCITTFaxDecode;
  const char *DCTDecode;
  const char *DecodeParms;
  const char *Filter;
  const char *FlateDecode;
  const char *LZWDecode;
  const char *RunLengthDecode;
};

struct PSPDFColorSpaceNames
{
	const char *DeviceCMYK;
  const char *DeviceGray;
  const char *DeviceRGB;
  const char *Indexed;
};

struct PSPDFImageNames
{
	PSPDFColorSpaceNames color_spaces;
  PSPDFFilterNames filter_names;
  const char *BitsPerComponent;
  const char *ColorSpace;
  const char *Decode;
  const char *Height;
  const char *ImageMask;
  const char *Interpolate;
  const char *Width;
};

extern const PSPDFColorSpaceNames pdf_color_space_names_short;
extern const PSPDFColorSpaceNames pdf_color_space_names;
extern const char *const resource_names[];
extern const PSPDFImageNames pdf_image_names_full;
extern const PSPDFImageNames pdf_image_names_short;

class XWPSPDFTempFile
{
public:
	XWPSPDFTempFile();
	~XWPSPDFTempFile();
		
	int openFile();
	int openStream();
	
public:
	QString file_name;
  QIODevice *file;
  XWPSStream *strm;
  uchar *strm_buf;
  XWPSStream *save_strm;	
};

class XWPSPDFBead
{
public:
	XWPSPDFBead();
	
public:
	long id, article_id, prev_id, next_id, page_id;
  XWPSRect rect;
};

class XWPSPDFStdFont
{
public:
	XWPSPDFStdFont();
	~XWPSPDFStdFont();
	
public:
	XWPSFont *font;
	XWPSPDFFontDescriptor *pfd;
	XWPSMatrix orig_matrix;
	XWPSUid uid;
};

class XWPSPDFTextState
{
public:
	XWPSPDFTextState();
	~XWPSPDFTextState();
	
	void reset();
	
public:
	float character_spacing;
  XWPSPDFFont *font;
  double size;
  float word_spacing;
  float leading;
  bool use_leading;
  XWPSMatrix matrix;
  XWPSPoint line_start;
  XWPSPoint current;
  
#define max_text_buffer 200
	uchar buffer[max_text_buffer];
  int buffer_count;
};

struct PSPDFStreamPosition
{
	long length_id;
    long start_pos;
};

struct PSPDFTextRotation
{
	long counts[5];
	int Rotate;
};

enum PSPDFTypedImageContext
{
    PDF_IMAGE_DEFAULT,
    PDF_IMAGE_TYPE3_MASK,
    PDF_IMAGE_TYPE3_DATA
} ;

#define NUM_RESOURCE_CHAINS 16
#define ps_id_hash(rid) ((rid) + ((rid) / NUM_RESOURCE_CHAINS))

#define MAX_OUTLINE_DEPTH 8
#define MAX_DEST_STRING 80

#define PDFMARK_NAMEABLE 1
#define PDFMARK_ODD_OK 2	
#define PDFMARK_KEEP_NAME 4
#define PDFMARK_NO_REFS 8	

class XWPSPDFResourceList
{
public:
	XWPSPDFResourceList();
	~XWPSPDFResourceList();
	
public:
	XWPSPDFResource*chains[NUM_RESOURCE_CHAINS];
};

typedef struct ht_function_s {
    const char *fname;
    float (*proc)(float, float);
} ht_function_t;

extern const ht_function_t ht_functions[];
extern const PSParamItem psdf_param_items[];

#define _COUNT_OF_HT_FUNCS_ 18

class XWPSDeviceVector : public XWPSDevice
{
	Q_OBJECT 
	
public:
	XWPSDeviceVector(QObject * parent = 0);
	XWPSDeviceVector(const char * devname,
	                 int w,
	                 int h,
	                 float xdpi,
	                 float ydpi,
	                 int nc,
	                 int depth,
	                 ushort mg, 
	                 ushort mc,
	                 ushort dg,
	                 ushort dc,
	                 QObject * parent = 0);
	virtual ~XWPSDeviceVector();
	
	int beginPage();
	int beginPath(PSPathType type);
	
	int closePath(double x0, double y0, double x_start, double y_start, PSPathType type);
	virtual int  copyDevice(XWPSDevice **pnew);
	        void copyDeviceParamVector(XWPSDeviceVector * proto);
	int curveTo(double x0, 
	            double y0,
	            double x1, 
	            double y1, 
	            double x2, 
	            double y2,	
	            double x3, 
	            double y3, 
	            PSPathType type);
	
	int doPath(XWPSPath *ppath,  PSPathType type, XWPSMatrix *pmat);
	int doPathVector(XWPSPath * ppath, PSPathType type, XWPSMatrix *pmat);
	int doRect(long x0, long y0, long x1, long y1, PSPathType type);
	
	int endPath(PSPathType type);
	
	int getParamsVector(XWPSContextState * ctx, XWPSParamList *plist);
	
	int lineTo(double x0, double y0, double x, double y, PSPathType type);
	
	int moveTo(double x0, double y0,double x, double y, PSPathType type);
	
	int prepareStrokeVector(XWPSImagerState * pis,
	                  XWPSStrokeParams * params, 
	                  XWPSDeviceColor * pdcolor,
			              float scale);
			              
	int putParamsVector(XWPSContextState * ctx, XWPSParamList *plist);
	
	virtual int reset();
	
	int setDash(const float *pattern, uint count, double offset);
	int setFillColor(XWPSDeviceColor * pdc);
	int setFlat(double f);
	int setLineCap(PSLineCap c);
	int setLineJoin(PSLineJoin j);
	int setLineWidth(double w);
	int setLogOp(uint lop, uint diff);
	int setMiterLimit(double l);
	int setStrokeColor(XWPSDeviceColor * pdc);
	
	void setOutputFile(QIODevice * f) {file = f;}
	XWPSStream * stream();
	int strokeScaling(XWPSImagerState *pis,
			              double *pscale, 
			              XWPSMatrix *pmat);
			              
	int updateLogOp(uint lop);
	
public:
	PSDeviceVectorProcs vec_procs;
	char fname[255 + 1];
	QIODevice *file;
	XWPSStream *strm;
	uchar *strmbuf;
	uint strmbuf_size;
	XWPSImagerState * state;
	float dash_pattern[11];
	XWPSDeviceColor * fill_color;
	XWPSDeviceColor * stroke_color;
	ulong no_clip_path_id;
	ulong clip_path_id;
	PSPathType fill_options, stroke_options;
	XWPSPoint scale;
	bool in_page;
	XWPSDeviceBbox * bbox_device;
	ulong black, white;
	
protected:	
	int  closeFileVector();
	bool drawingColorEq(XWPSDeviceColor * pdc1, XWPSDeviceColor * pdc2);
	bool hasSubsetPrefix(const uchar *str, uint size);
	void loadCache();
	bool makePathScaling(XWPSPath *ppath, double *pscale);
	void makeSubsetPrefix(uchar *str, ulong id);
	bool mustPutClipPath( XWPSClipPath * pcpath);
	int  openFile(uint strmbuf_sizeA);
	int  openFileBbox(uint strmbuf_sizeA,  bool bbox);
	
	int scanToken(const uchar **pscan, const uchar * end, const uchar **ptoken);
	int scanTokenComposite(const uchar **pscan, const uchar * end, const uchar **ptoken_orig);
	void setPatternImage(XWPSDataImage *pic, XWPSStripBitmap *tile);
	void setProcessColorModel();
	void storeDefaultProducer(char * buf);
};

class XWPSDevVectorDoPathState
{
public:
	XWPSDevVectorDoPathState();
	~XWPSDevVectorDoPathState();
	
	void init(XWPSDeviceVector * dev, PSPathType t, XWPSMatrix *pmat);
	
	int  segment(int pe_op, XWPSFixedPoint * vs);
	
public:
	XWPSDeviceVector *vdev;
  PSPathType type;
  bool first;
  XWPSMatrix scale_mat;
  XWPSPoint start;
  XWPSPoint prev;
};

extern const PSPSDFSetColorCommands psdf_set_fill_color_commands;
extern const PSPSDFSetColorCommands psdf_set_stroke_color_commands;

class XWPSPSDFImageParams
{
public:
	XWPSPSDFImageParams();
	~XWPSPSDFImageParams();
	
	void copy(XWPSPSDFImageParams * from);
	
	bool doDownSample(XWPSPixelImage *pim, float resolution);
	
	void init(bool af, 
	          int res, 
	          float dst, 
	          const char * f, 
	          PSStreamTemplate * ft);
	
public:
	XWPSCParamList *ACSDict;
	bool AntiAlias;
  bool AutoFilter;
  int Depth;
  XWPSCParamList *Dict;
  bool Downsample;
  float DownsampleThreshold;  
  psdf_downsample_type DownsampleType;  
  bool Encode;
  const char *Filter;
  int Resolution;
  PSStreamTemplate * filter_template;
};

class XWPSPSDFDistillerParams
{
public:    
	XWPSPSDFDistillerParams();
	~XWPSPSDFDistillerParams();
	
	void copy(XWPSPSDFDistillerParams * from);
	
public:
	bool ASCII85EncodePages;
  psdf_auto_rotate_pages AutoRotatePages;
  psdf_binding Binding;
  bool CompressPages;
  psdf_default_rendering_intent DefaultRenderingIntent;
  bool DetectBlends;
  bool DoThumbnails;
  long ImageMemory;
  bool LockDistillerParams;
  bool LZWEncodePages;
  int OPM;
  bool PreserveOPIComments;
  bool UseFlateCompression;
  XWPSString CalCMYKProfile;
  XWPSString CalGrayProfile;
  XWPSString CalRGBProfile;
  XWPSString sRGBProfile;
  psdf_color_conversion_strategy ColorConversionStrategy;
  bool PreserveHalftoneInfo;
  bool PreserveOverprintSettings;
  psdf_transfer_function_info TransferFunctionInfo;
  psdf_ucr_and_bg_info UCRandBGInfo;
  XWPSPSDFImageParams ColorImage;
  bool ConvertCMYKImagesToRGB;
  bool ConvertImagesToIndexed;
  XWPSPSDFImageParams GrayImage;
  XWPSPSDFImageParams MonoImage;
  PSParamStringArray AlwaysEmbed;
  PSParamStringArray NeverEmbed;
  psdf_cannot_embed_font_policy CannotEmbedFontPolicy;
  bool EmbedAllFonts;
  int MaxSubsetPct;
  bool SubsetFonts;
};

#define PDF_MAX_PRODUCER 200
#define MAX_PS_INLINE 100

#endif //XWPSDEVICE_H
