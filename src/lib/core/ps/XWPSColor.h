/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSCOLOR_H
#define XWPSCOLOR_H

#include "XWPSBitmap.h"

class XWPSHTTile;
class XWPSDeviceHalftone;
class XWPSPattern;
class XWPSPatternInstance;
class XWPSImagerState;
class XWPSState;
class XWPSDevice;
class XWPSDeviceColor;
class XWPSColorSpaceCommon;
class XWPSColorSpace;
class XWPSDeviceMaskClip;
class XWPSTileFillState;
class XWPSHTCache;

#define MAX_DCC PS_DEVICE_COLOR_MAX_COMPONENTS

#define tile_longs_LARGE 256
#define tile_longs_SMALL 64
#if arch_small_memory
#  define tile_longs_allocated tile_longs_SMALL
#  define tile_longs tile_longs_SMALL
#else
#  define tile_longs_allocated tile_longs_LARGE
#  ifdef DEBUG
#    define tile_longs\
       (gs_debug_c('.') ? tile_longs_SMALL : tile_longs_LARGE)
#  else
#    define tile_longs tile_longs_LARGE
#  endif
#endif

struct PSColorValuesPair 
{
    ushort values[MAX_DCC][2];
} ;

struct PSTileCursor 
{
    int tile_shift;	
    int xoffset;
    int xshift;
    uint xbytes;
    int xbits;
    const uchar *row;
    const uchar *tdata;
    uint raster;
    const uchar *data;
    int bit_shift;
} ;

typedef void (*set_color_ht_)(uchar *, uint , int px, int py,
		  				int , int , int , int , int ,
		  ulong , XWPSDevice *,
		  const PSColorValuesPair *ignore_pvp,
		  ulong * ,
		  XWPSStripBitmap **);

struct PSDeviceColorType
{
	int (XWPSDeviceColor::*load)(XWPSImagerState *,
	      											 XWPSDevice *, 
	      											 PSColorSelect);
	int (XWPSDeviceColor::*fill_rectangle)(int , 
		                                     int ,
			                                   int , 
			                                   int , 
			                                   XWPSDevice *,
																				 uint ,
																				 const PSRopSource *);
	int (XWPSDeviceColor::*fill_masked)(const uchar *,
		                                  int , int , 
		                                  ulong ,
		                                  int , 
		                                  int , 
		                                  int , int , 
		                                  XWPSDevice *,
		                                  uint , 
		                                  bool);
	bool (XWPSDeviceColor::*equal)(XWPSDeviceColor *);
};

class XWPSClientColor : public XWPSStruct
{
public:
	XWPSClientColor();
	XWPSClientColor(const XWPSClientColor * other);
	~XWPSClientColor();
	
	XWPSPattern * getPattern();
	
	int makePattern(XWPSPattern * pcp,	
	                 XWPSMatrix * pmat, 
	                XWPSState * pgs);
	int zPaintProc(XWPSState * pgs);
	
	XWPSClientColor & operator=(XWPSClientColor & other);
	
public:
	PSPaintColor paint;
  XWPSPatternInstance *pattern;
};

class XWPSTileFillState
{
public:
	XWPSTileFillState();
	~XWPSTileFillState();
	
	int init(XWPSDeviceColor * pdevcA, XWPSDevice * dev, bool set_mask_phase);
	
	int tileBySteps(int x0A, int y0A, int w0A, int h0A,
	      					XWPSColorTile * ptile,
	      					XWPSStripBitmap * tbits_or_tmask,
	      					int (XWPSTileFillState::*fill_proc)(int x, int y, int w, int h));
	      						
	int tileColoredFill(int x, int y, int w, int h);
	int tileMaskedFill(int x, int y, int w, int h);
	
public:
	XWPSDeviceColor *pdevc;
	int x0, y0, w0, h0;
  ulong lop;
  const PSRopSource *source;
  XWPSDeviceMaskClip * cdev;
  XWPSDevice *pcdev;
  XWPSStripBitmap *tmask;
  XWPSIntPoint phase;
  int (XWPSDeviceColor::*fill_rectangle)(int , 
		                                     int ,
			                                   int , 
			                                   int , 
			                                   XWPSDevice *,
																				 uint ,
																				 const PSRopSource *);
	const PSRopSource *rop_source;
  XWPSDevice *orig_dev;
  int xoff, yoff;
};

class XWPSDeviceColor
{
public:
	XWPSDeviceColor(ulong p = 0);
	~XWPSDeviceColor();
	
	ulong binaryColor(int i) {return (colors.binary.color[i]);}
	XWPSStripBitmap * binaryTile();
	
	void completeCMYKHalftone(XWPSDeviceHalftone *pdht);
	void completeRGBHalftone(XWPSDeviceHalftone *pdht);
	
	int  defaultFillMaskedDC(const uchar * data,
		                     int data_x, int raster, 
		                     ulong id,
		                     int x, 
		                     int y, 
		                     int w, 
		                     int h, 
		                     XWPSDevice *dev,
		                     uint lop, 
		                     bool invert);
		
	bool equal(XWPSDeviceColor *pdevc2);
	
	int fillMasked(const uchar *data, 
	              int data_x,
                 int raster, 
                 ulong id, 
                 int x, 
                 int y, 
                 int w, 
                 int h,
    							XWPSDevice *dev, 
    							ulong lop, 
    							bool invert);
	int fillRectangle(int x, 
		                int y,
			              int w, 
			              int h, 
			              XWPSDevice *dev,
										uint lop,
										const PSRopSource *source);
										
	XWPSIntPoint * getPhase() {return &phase;}
	bool isBinaryHalftone();
	bool isNull();
	bool isPure();
	bool isSet();
	
	int load(XWPSImagerState * pis, XWPSDevice * dev);
	int loadPattern(XWPSImagerState * pis,
	      			   XWPSDevice * dev, 
	      			   PSColorSelect select);
	int loadSelect(XWPSImagerState * pis,
	      			   XWPSDevice * dev, 
	      			   PSColorSelect select);
	
	bool  patternCacheLookup(XWPSImagerState * pis,
												   XWPSDevice * dev, 
												   PSColorSelect select);
	ulong pureColor() {return colors.pure;}
	
	int reduceColoredHalftone(XWPSDevice *dev,  bool cmyk);
	int renderCMYK(short c, 
	                short m, 
	                short y, 
	                short k, 
	                XWPSImagerState * pis, 
	                XWPSDevice * dev, 
	                PSColorSelect select);
	int renderColor(short r, 
	                short g, 
	                short b, 
	                short w, 
	                bool  cmyk, 
	                XWPSImagerState * pis, 
	                XWPSDevice * dev, 
	                PSColorSelect select);
	int renderColorAlpha(short r, 
	                     short g, 
	                     short b, 
	                     short w, 
	                     ushort a, 
	                     bool cmyk, 
	                     XWPSImagerState * pis, 
	                     XWPSDevice * dev, 
	                     PSColorSelect select);
	int renderDeviceColor(short red, 
	                      short green, 
	                      short blue, 
	                      short white,
			                  bool cmyk, 
			                  ushort alpha,
			       					  XWPSDevice * dev,
			                  XWPSDeviceHalftone * pdht,
			                  XWPSIntPoint * ht_phase);
	int renderDeviceGray(short gray, 
	                     ushort alpha,
			     						 XWPSDevice * dev,
			                 XWPSDeviceHalftone * dev_ht,
			                XWPSIntPoint * ht_phase);
	int renderGray(short gray, 
	               XWPSImagerState * pis, 
	               XWPSDevice * dev, 
	               PSColorSelect select);
	int renderGrayAlpha(short gray, 
	                    ushort alpha, 
	                    XWPSImagerState * pis, 
	                    XWPSDevice * dev, 
	                    PSColorSelect select);
	int renderRGB(short r, 
	              short g,
	              short b, 
	              XWPSImagerState * pis, 
	              XWPSDevice * dev, 
	              PSColorSelect select);
	int renderRGBAlpha(short r, 
	                   short g, 
	                   short b, 
	                   ushort a, 
	                   XWPSImagerState * pis, 
	                   XWPSDevice * dev, 
	                   PSColorSelect select);
	
	
	void setBinaryHalftone(XWPSDeviceHalftone * ht,
	                                ulong color0, 
	                                ulong color1,
	                                uint level);
	void setBinaryHalftoneComponent(XWPSDeviceHalftone * ht, 
	                                int index, 
	                                ulong color0, 
	                                ulong color1,	
                                  uint level);
	void setCMYKHalftone(XWPSDeviceHalftone * ht, 
	                     uchar bc, 
	                     uint lc, 
	                     uchar bm, 
	                     uint lm, 
	                     uchar by, 
	                     uint ly, 
	                     uchar bk, 
	                     uint lk);
	int  setCMYK1BitColors(PSColorValuesPair *ignore_pvp,
		     ulong * colorsA,
		     XWPSStripBitmap ** sbits,
		     XWPSDevice * dev,
		     XWPSHTCache ** caches,
		     int nplanes);
	int  setHTColorsGT4(PSColorValuesPair *pvp,
		   								ulong * colorsA,
		   								XWPSStripBitmap ** sbits,
		   								XWPSDevice * dev,
		                  XWPSHTCache ** caches, 
		                  int nplanes);
	int  setHTColorsLE4(PSColorValuesPair *pvp,
		   								ulong * colorsA,
		   								XWPSStripBitmap ** sbits,
		   								XWPSDevice * dev,
		                  XWPSHTCache ** caches, 
		                  int nplanes);
	void setNull();
	void setNullPattern();
	void setRGBHalftone(XWPSDeviceHalftone * ht, 
	                    uchar br, 
	                    uint lr, 
	                    uchar bg, 
	                    uint lg, 
	                    uchar bb, 
	                    uint lb, 
	                    ushort a);
	void setPure(ulong color);
	
	void unset();
	
	bool writePure(ulong lop);

	bool binaryEqualHT(XWPSDeviceColor *pdevc2);
	int  binaryFillRectangleHT(int x, 
		                    int y,
			                  int w, 
			                  int h, 
			                  XWPSDevice *dev,
												uint lop,
												const PSRopSource *source);
	int binaryLoadHT(XWPSImagerState *pis,
	      			     XWPSDevice *dev, 
	      			     PSColorSelect select);
	bool binaryMaskedEqual(XWPSDeviceColor * pdevc2);
	int binaryMaskedFillRect(int x, 
	                         int y, 
	                         int w, 
	                         int h, 
	                         XWPSDevice * dev,
			                     uint lop,
			                     const PSRopSource * source);
	
	bool coloredEqualHT(XWPSDeviceColor * pdevc2);
	int  coloredLoadHT(XWPSImagerState *pis,
	      			     XWPSDevice * ignore_dev, 
	      			     PSColorSelect select);
	int  coloredFillRectangleHT(int x, 
	                           int y, 
	                           int w, 
	                           int h, 
	                           XWPSDevice * dev,
			                       uint lop,
			                       const PSRopSource * source);	
	bool coloredMaskedEqual(XWPSDeviceColor * pdevc2);
	int  coloredMaskedFillRect(int x, 
	                           int y, 
	                           int w, 
	                           int h, 
	                           XWPSDevice * dev,
			                       uint lop,
			                       const PSRopSource * source);
	int  coloredMaskedLoad(XWPSImagerState * pis,
									XWPSDevice * dev, 
									PSColorSelect select);
		                     
	int maskedLoadBinaryDC(XWPSImagerState * pis,
									XWPSDevice * dev, 
									PSColorSelect select);
	bool noEqual(XWPSDeviceColor * ) {return false;}
	int  noFillMasked(const uchar *,
		                 int , int , 
		                 ulong ,
		                 int , 
		                 int , 
		                 int w, 
		                 int h, 
		                 XWPSDevice *,
		                 uint , 
		                 bool);
	int noFillRectangle(int x, 
		                  int y,
			                int w, 
			                int h, 
			                XWPSDevice *dev,
											uint lop,
											const PSRopSource *source) ;
	int noLoad(XWPSImagerState *,
	      			 XWPSDevice *, 
	      			 PSColorSelect) {return 0;}
	      			 
	bool nullEqual(XWPSDeviceColor * pdevc2);
	int  nullFillMasked(const uchar *,
		                 int , int , 
		                 ulong ,
		                 int , 
		                 int , 
		                 int , 
		                 int , 
		                 XWPSDevice *,
		                 uint , 
		                 bool) {return 0;}
	int nullFillRectangle(int , 
		                    int ,
			                  int , 
			                  int , 
			                  XWPSDevice *,
												uint ,
												const PSRopSource *) {return 0;}
	int nullLoad(XWPSImagerState *,
	      			 XWPSDevice *, 
	      			 PSColorSelect) {return 0;}
	      			 
	bool patternEqualDC(XWPSDeviceColor * pdevc2);
	int  patternFillRectangleDC(int x, 
	                            int y,
			                        int w, 
			                        int h, 
			                        XWPSDevice * dev,
			     										uint lop,
			     										const PSRopSource * source);
	int patternLoad(XWPSImagerState * pis,
									XWPSDevice * dev, 
									PSColorSelect select);
	int patternLoadDC(XWPSImagerState *pis,
	      			 XWPSDevice *dev, 
	      			 PSColorSelect select);
	bool pattern2EqualDC(XWPSDeviceColor * pdevc2);
	int pattern2FillRectangleDC(int x, 
		                        int y,
			                      int w, 
			                      int h, 
			                      XWPSDevice *dev,
												    uint lop,
												    const PSRopSource *source);
	int pattern2LoadDC(XWPSImagerState *,
	      			 XWPSDevice *, 
	      			 PSColorSelect) {return 0;}
	      			 
	bool pureEqualDC(XWPSDeviceColor * pdevc2);
	int  pureFillMasked(const uchar * data,
		                 int data_x, int raster, 
		                 ulong id,
		                 int x, 
		                 int y, 
		                 int w, 
		                 int h, 
		                 XWPSDevice *dev,
		                 uint lop, 
		                 bool invert);
	int pureFillRectangle(int x, 
		                    int y,
			                  int w, 
			                  int h, 
			                  XWPSDevice *dev,
												uint lop,
												const PSRopSource *source);
	int pureLoadDC(XWPSImagerState *,
	      			 XWPSDevice *, 
	      			 PSColorSelect) {return 0;}
	bool pureMaskedEqualDC(XWPSDeviceColor * pdevc2);
	int pureMaskedFillRectDC(int x, 
	                         int y, 
	                         int w, 
	                         int h, 
	                         XWPSDevice * dev,
			                     uint lop,
			                    const PSRopSource * source);
	      			 
	int  pureMaskedLoadDC(XWPSImagerState * pis,
		       							XWPSDevice * dev, 
		       							PSColorSelect select);
	void ropSourceSetColor(PSRopSource * prs, ulong pixel);
	
	void setC(int i, uchar b, uint l);
	void setPhase(int px, int py);
	void setPhaseMod(int px, int py, int tw, int th);
	void setRopNoSource(const PSRopSource **psource,
			                PSRopSource *pno_source, 
			                XWPSDevice *dev);
	
public:
	PSDeviceColorType * type;
	
	union _c 
	{
		ulong pure;
		struct _bin 
		{
	    XWPSDeviceHalftone * b_ht;
	    ulong color[2];
	    uint b_level;
	    int b_index;
	    XWPSHTTile *b_tile;
		} binary;
		struct _col 
		{
	    XWPSDeviceHalftone *c_ht; /* non-const for setting cache ptr */
	    uchar c_base[PS_DEVICE_COLOR_MAX_COMPONENTS];
	    uint c_level[PS_DEVICE_COLOR_MAX_COMPONENTS];
	    ushort /*gx_color_value */ alpha;
#if PS_DEVICE_COLOR_MAX_COMPONENTS <= ARCH_SIZEOF_SHORT * 8
	    ushort plane_mask;
#else
#if PS_DEVICE_COLOR_MAX_COMPONENTS <= ARCH_SIZEOF_INT * 8
	    uint plane_mask;
#else
	    ulong plane_mask;
#endif
#endif
		} colored;
		struct _pat 
		{
	    XWPSColorTile *p_tile;
		} /*(colored) */ pattern;
  } colors;
  
  XWPSIntPoint phase;
  XWPSClientColor ccolor;	
  struct _mask 
  {
		struct mp_ 
		{
	    short x, y;
		} m_phase;
		ulong id;
		XWPSColorTile *m_tile;
  } mask;
  
  uchar bytes[sizeof(ulong) * 8];
  XWPSStripBitmap ht_no_bitmap;
};

extern PSDeviceColorType ps_dc_type_data_null;
extern PSDeviceColorType ps_dc_type_data_none;
extern PSDeviceColorType ps_dc_type_data_pure;
extern PSDeviceColorType ps_dc_type_ht_binary;
extern PSDeviceColorType ps_dc_type_ht_colored;
extern PSDeviceColorType ps_dc_type_pattern;
extern PSDeviceColorType ps_dc_binary_masked;
extern PSDeviceColorType ps_dc_colored_masked;
extern PSDeviceColorType ps_dc_pattern2;
extern PSDeviceColorType ps_dc_pure_masked;

#endif //XWPSCOLOR_H
