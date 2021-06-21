/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSHALFTONE_H
#define XWPSHALFTONE_H

#include "XWPSBitmap.h"
#include "XWPSTransferMap.h"

#define ht_mask_bits (sizeof(uint) * 8)
#define max_ht_sample (uint)(((1 << (ht_mask_bits - 2)) - 1) * 2 + 1)

void bits_replicate_horizontally(uchar * data, 
                                 uint width, uint height,
		                             uint raster, 
		                             uint replicated_width, 
		                             uint replicated_raster);
void bits_replicate_vertically(uchar * data, 
                               uint height, 
                               uint raster,
			                         uint replicated_height);

class XWPSScreenHalftone;
class XWPSHTCache;
class XWPSHTTile;
class XWPSState;
class XWPSHTCellParams;
class XWPSClientOrderHalftone;
class XWPSHTOrder;

struct PSScreenHalftoneProc
{
	union
	{
  	float (XWPSScreenHalftone::*spot_function)(float, float);
  }spot_function_;
};

class XWPSScreenHalftone
{
public:
	XWPSScreenHalftone() : frequency(0.0), 
		                     angle(0.0), 
		                     actual_frequency(0.0), 
		                     actual_angle(0.0) {}
	XWPSScreenHalftone(const XWPSScreenHalftone * other);
	
	int pickCellSize(const XWPSMatrix * pmat, 
	                    ulong max_size,
                      uint min_levels, 
                      bool accurate, 
                      XWPSHTCellParams * phcp);
                      
  float spotDummy(float x, float y) {return (x + y) / 2;}
	float spotFunction(float x, float y);
		
public:
	float frequency;
  float angle;
  PSScreenHalftoneProc proc;
  float actual_frequency;
  float actual_angle;
};

class XWPSColorScreenHalftone
{
public:
	XWPSColorScreenHalftone();
	XWPSColorScreenHalftone(const XWPSColorScreenHalftone * other);
	~XWPSColorScreenHalftone();
	
public:	
	union _css 
	{
		XWPSScreenHalftone * indexed[4];
		struct _csc 
		{
	    XWPSScreenHalftone * red;
	    XWPSScreenHalftone * green;
	    XWPSScreenHalftone * blue;
	    XWPSScreenHalftone * gray;
		} colored;
  } screens;
};

class XWPSSpotHalftone
{
public:
	XWPSSpotHalftone();
	~XWPSSpotHalftone();
	
public:
	XWPSScreenHalftone screen;
	bool accurate_screens;
	XWPSTransferMap transfer;
	XWPSTransferMap transfer_closure;
};

class XWPSThresholdHalftoneCommon
{
public:
	XWPSThresholdHalftoneCommon() :width(0),height(0) {}
	virtual ~XWPSThresholdHalftoneCommon();
	
public:
	int width;
	int height;
	XWPSTransferMap transfer_closure;
};

class XWPSThresholdHalftone : public XWPSThresholdHalftoneCommon
{
public:
	XWPSThresholdHalftone();
	~XWPSThresholdHalftone();
	
public:	
	XWPSTransferMap transfer;
	XWPSString  thresholds;
};

class XWPSThresholdHalftone2 : public XWPSThresholdHalftoneCommon
{
public:
	XWPSThresholdHalftone2();
	~XWPSThresholdHalftone2();
	
public:
	int width2;
  int height2;
  int bytes_per_sample;
  XWPSString thresholds;
};

struct PSClientOrderHTProcs 
{
  int (XWPSClientOrderHalftone::*create_order) (XWPSHTOrder * , XWPSState * );

};

class XWPSClientOrderHalftone
{
public:
	XWPSClientOrderHalftone() : width(0),height(0),num_levels(0),transfer_closure(0) {}
	~XWPSClientOrderHalftone();
	
	int createOrder(XWPSHTOrder * porder, XWPSState * pgs);
	
public:
	int width;
  int height;
  int num_levels;
  const PSClientOrderHTProcs *procs;
  const void *client_data;
  XWPSTransferMap * transfer_closure;
};

class XWPSHalftoneComponent
{
public:
	XWPSHalftoneComponent();
	~XWPSHalftoneComponent();
	
	void setType(PSHalftoneType t);
	
public:
	PSHTSeparationName cname;
  PSHalftoneType type;
  union
  {
  	XWPSSpotHalftone * spot;
		XWPSThresholdHalftone * threshold;
		XWPSThresholdHalftone2 * threshold2;
		XWPSClientOrderHalftone * client_order;
  }params;
};

class XWPSMultipleHalftone
{
public:
	XWPSMultipleHalftone() : components(0),num_comp(0) {}
	~XWPSMultipleHalftone();
	
public:
	XWPSHalftoneComponent* components;
  uint num_comp;
};

class XWPSHalftone : public XWPSStruct
{
public:
	XWPSHalftone();
	~XWPSHalftone();
	
	int getLength();
	const char * getTypeName();
	
	void setType(PSHalftoneType t);
	
public:
	PSHalftoneType type;
	union 
	{
		XWPSScreenHalftone * screen;
		XWPSColorScreenHalftone * colorscreen;
		XWPSSpotHalftone * spot;
		XWPSThresholdHalftone * threshold;
		XWPSThresholdHalftone2 * threshold2;
		XWPSClientOrderHalftone * client_order;
		XWPSMultipleHalftone * multiple;
  } params;
};

class XWPSHTCellParams
{
public:
	XWPSHTCellParams() : M(0),N(0),R(0),
		                   M1(0),N1(0),R1(0),
		                   C(0),W(0),W1(0),
		                   S(0) {}
	
	void computeCellValues();
	
public:
	short M, N, R;
	short M1, N1, R1;
	ulong C;
	short D, D1;
	uint W, W1;
	int S;
};

class XWPSHTOrderScreenParams
{
public:
	XWPSHTOrderScreenParams();
	
public:
	ulong max_size;
	XWPSMatrix matrix;
};

struct PSHTBit
{
	uint offset;
  uint mask;
};

void ps_ht_construct_bit(PSHTBit * bit, int width, int bit_num);
void ps_sort_ht_order(PSHTBit * recs, uint N);

struct PSHTOrderProcs
{
	uint bit_data_elt_size;
	int (XWPSHTOrder::*construct_order)(const uchar *);
	int (XWPSHTOrder::*bit_index)(uint ,	XWPSIntPoint *);
	int (XWPSHTOrder::*render)(XWPSHTTile *, int );
};

class XWPSHTOrder
{
public:
	XWPSHTOrder();
	~XWPSHTOrder();
	
	int alloc(uint widthA, 
	          uint heightA,
		        uint num_levelsA, 
		        uint num_bitsA, 
		        uint strip_shiftA,
		        const PSHTOrderProcs *procsA);
	int allocClientOrder(uint widthA, 
	                     uint heightA,
			                 uint num_levelsA, 
			                 uint num_bitsA);
	int allocOrder(uint widthA, 
	               uint heightA,
		  					 uint strip_shiftA, 
		  					 uint num_levelsA);
	int allocThresholdOrder(uint widthA, uint heightA, uint num_levelsA);
	
	int bitIndex(uint index, XWPSIntPoint *ppt);
	int bitIndexDefault(uint index, XWPSIntPoint *ppt);
	int bitIndexShort(uint index, XWPSIntPoint *ppt);
	
	void completeThresholdOrder();
	void constructBits();
	int  constructOrder(const uchar *thresholds);
	int  constructDefault(const uchar *thresholds);
	int  constructShort(const uchar *thresholds);
	void constructSpotOrder();
	int  constructThresholdOrder(const uchar * thresholds);
	
	uint fullHeight();
	
	int processClientOrder(XWPSState * pgs, XWPSClientOrderHalftone * phcop);
	int processSpot(XWPSState * pgs, XWPSSpotHalftone * phsp);
	int processTransfer(XWPSState * pgs, XWPSTransferMap * proc);
	int processThreshold(XWPSState * pgs, XWPSThresholdHalftone * phtp);
	int processThreshold2(XWPSState * pgs, XWPSThresholdHalftone2 * phtp);
	
	int render(XWPSHTTile *tile, int new_bit_level);
	int renderDefault(XWPSHTTile *pbt, int level);
	int renderShort(XWPSHTTile *pbt, int level);
	
	int screenOrderAlloc();
	int screenOrderInitMemory(XWPSState * pgs,
                            XWPSScreenHalftone * phsp, 
                            bool accurate);
                            
	XWPSHTOrder & operator=(const XWPSHTOrder & other);
	
public:
	XWPSHTCellParams params;
  ushort width;
  ushort height;
  ushort raster;
  ushort shift;
  ushort orig_height;
  ushort orig_shift;
  uint full_height;
  uint num_levels;
  uint num_bits;
  const PSHTOrderProcs *procs;
  XWPSUints *levels;
  XWPSBytes *bit_data;
  XWPSHTCache *cache;
  XWPSTransferMap * transfer;
  XWPSHTOrderScreenParams screen_params;
};

class XWPSScreenEnum : public XWPSStruct
{
public:
	XWPSScreenEnum();
	~XWPSScreenEnum();
	
	int currentPoint(XWPSPoint * ppt);
	
	int getLength();
	const char * getTypeName();
	
	int init(XWPSState * pgsA, XWPSScreenHalftone * phsp);
	int initMemory(XWPSState * pgsA,
                 XWPSScreenHalftone * phsp, 
                 bool accurate);
	int initMemory(XWPSHTOrder * porder,
                 XWPSState * pgsA, 
                 XWPSScreenHalftone * phsp);
	int install();
	
	int next(float value);
	
	int processScreenMemory(XWPSState * pgsA,
													XWPSScreenHalftone * phsp, 
													bool accurate);
	
public:
	XWPSHalftone * halftone;
	XWPSHTOrder order;
	XWPSMatrix mat;
	XWPSMatrix mat_inv;
	int x, y;
  int strip, shift;
  XWPSState *pgs;
};

class XWPSHTTile
{
public:
	XWPSHTTile();
	XWPSHTTile(const XWPSHTTile * other);
	
public:
	XWPSStripBitmap tiles;
	int level;
	uint index;
};

class XWPSHTCache : public XWPSStruct
{
public:
	XWPSHTCache();
	XWPSHTCache(uint max_tiles, uint max_bits);
	XWPSHTCache(const XWPSHTCache * other);
	~XWPSHTCache();
	
	int getLength();
	const char * getTypeName();
	
	void init(XWPSHTOrder * porder);
	
	void clear();
	
	XWPSHTTile * render(int t);
	int renderHt(XWPSHTTile * pbt, int level,
	              XWPSHTOrder * porder, 
	              ulong new_id);
	XWPSHTTile * renderHtDefault(int b_level);
	XWPSHTTile * renderHt1Tile(int b_level);
	XWPSHTTile * renderHt1Level(int b_level);
	
	XWPSHTCache & operator=(const XWPSHTCache & other);
	
public:
	uchar *bits;
	uint bits_size;
	XWPSHTTile *ht_tiles;
	uint num_tiles;
	XWPSHTOrder order;
	int num_cached;
	int levels_per_tile;
	int tiles_fit;
	ulong base_id;
	
	XWPSHTTile *(XWPSHTCache::*render_ht)(int);
};

struct PSDeviceHalftoneResource
{
	const char *rname;
  int HalftoneType;
  int Width;
  int Height;
  int num_levels;
  const unsigned int *levels;
  const void *bit_data;
  int elt_size;
};

typedef PSDeviceHalftoneResource * (*ps_dht_proc)();
extern ps_dht_proc ps_device_halftone_list[];

class XWPSHTOrderComponent
{
public:
	XWPSHTOrderComponent() {}
	
public:
	XWPSHTOrder corder;
  PSHTSeparationName cname;
};

class XWPSDeviceHalftone : public XWPSStruct
{
public:
	XWPSDeviceHalftone();
	~XWPSDeviceHalftone();
	
	int getLength();
	const char * getTypeName();
	
public:
	XWPSHTOrder order;
	ulong id;
	PSHalftoneType type;
	XWPSHTOrderComponent *components;
	uint num_comp;
	uint color_indices[4];
	int lcm_width, lcm_height;
};

#endif //XWPSHALFTONE_H
