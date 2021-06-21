/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <math.h>
#include "XWPSError.h"
#include "XWPSTransferMap.h"
#include "XWPSStream.h"
#include "XWPSDevice.h"
#include "XWPSState.h"
#include "XWPSHalfTone.h"

#define max_cached_tiles_HUGE 5000	/* not used */
#define max_ht_bits_HUGE 1000000	/* not used */
#define max_cached_tiles_LARGE 577
#define max_ht_bits_LARGE 100000
#define max_cached_tiles_SMALL 25
#define max_ht_bits_SMALL 1000

static bool FORCE_STRIP_HALFTONES = false;

void bits_replicate_horizontally(uchar * data, 
                                 uint width, uint height,
		                             uint raster, 
		                             uint replicated_width, 
		                             uint replicated_raster)
{
	const uchar *orig_row = data + (height - 1) * raster;
  uchar *tile_row = data + (height - 1) * replicated_raster;
  uint y;

  if (!(width & 7)) 
  {
		uint src_bytes = width >> 3;
		uint dest_bytes = replicated_width >> 3;

		for (y = height; y-- > 0; orig_row -= raster, tile_row -= replicated_raster) 
		{
	    uint move = src_bytes;
	    const uchar *from = orig_row;
	    uchar *to = tile_row + dest_bytes - src_bytes;

	    memmove(to, from, move);
	    while (to - tile_row >= move) 
	    {
				from = to;
				to -= move;
				memmove(to, from, move);
				move <<= 1;
	    }
	    if (to != tile_row)
				memmove(tile_row, to, to - tile_row);
		}
  } 
  else 
  {
		uint bit_count = width & -width;
		uint left_mask = (0xff00 >> bit_count) & 0xff;

		for (y = height; y-- > 0;  orig_row -= raster, tile_row -= replicated_raster) 
		{
	    uint sx;

	    for (sx = width; sx > 0;) 
	    {
				uint bits, dx;

				sx -= bit_count;
				bits = (orig_row[sx >> 3] << (sx & 7)) & left_mask;
				for (dx = sx + replicated_width; dx >= width;) 
				{
		    	uchar *dp;
		    	int dbit;

		    	dx -= width;
		    	dbit = dx & 7;
		    	dp = tile_row + (dx >> 3);
		    	*dp = (*dp & ~(left_mask >> dbit)) | (bits >> dbit);
				}
	   	}
		}
  }
}

void bits_replicate_vertically(uchar * data, 
                               uint height, 
                               uint raster,
			                         uint replicated_height)
{
	uchar *dest = data;
  uint h = replicated_height;
  uint size = raster * height;

  while (h > height) 
  {
		memcpy(dest + size, dest, size);
		dest += size;
		h -= height;
  }
}

XWPSScreenHalftone::XWPSScreenHalftone(const XWPSScreenHalftone * other)
{
	frequency = other->frequency;
	angle = other->angle;
	proc = other->proc;
	actual_frequency = other->actual_frequency;
	actual_angle = other->actual_angle;
}

int XWPSScreenHalftone::pickCellSize(const XWPSMatrix * pmat, 
	                    ulong max_size,
                      uint min_levels, 
                      bool accurate, 
                      XWPSHTCellParams * phcp)
{
	const bool landscape = (pmat->xy != 0.0 || pmat->yx != 0.0);
	const bool reflected = pmat->xy * pmat->yx > pmat->xx * pmat->yy;
  const int reflection = (reflected ? -1 : 1);
  const int rotation =  (landscape ? (pmat->yx < 0 ? 90 : -90) : pmat->xx < 0 ? 180 : 0);
  const double f0 = frequency, a0 = angle;
  const double T = fabs((landscape ? pmat->yx / pmat->xy : pmat->xx / pmat->yy));
  XWPSPoint uv0;
  
  int rt = 1;
  double f = 0, a = 0;
  double e_best = 1000;
  bool better;
  {
    XWPSMatrix rmat;

    rmat.makeRotation(a0 * reflection + rotation);
    uv0.distanceTransform(72.0 / f0, 0.0, &rmat);
    uv0.distanceTransform(uv0.x, uv0.y, (XWPSMatrix*)pmat);
  }
  
  if (uv0.x == 0 && uv0.y == 0)
    return (int)(XWPSError::RangeCheck);
   
  while ((fabs(uv0.x) + fabs(uv0.y)) * rt < 4)
    ++rt;

try_size:
	better = false;
	{
		int m0 = (int)floor(uv0.x * rt + 0.0001);
    int n0 = (int)floor(uv0.y * rt + 0.0001);
    XWPSHTCellParams p;
    p.R = p.R1 = rt;
    for (p.M = m0 + 1; p.M >= m0; p.M--)
    	for (p.N = n0 + 1; p.N >= n0; p.N--)
    	{
    		long raster, wt, wt_size;
        double fr, ar, ft, at, f_diff, a_diff, f_err, a_err;

        p.M1 = (int)floor(p.M / T + 0.5);
        p.N1 = (int)floor(p.N * T + 0.5);
        p.computeCellValues();
        wt = p.W;
        if (wt >= max_short)
          continue;
          
        raster = bitmap_raster(wt);
        if (raster > max_size / p.D || raster > max_long / wt)
           continue;
        wt_size = raster * wt;
        if (landscape)
          ar = atan2(p.M * pmat->xy, p.N * pmat->yx), fr = 72.0 * (p.M == 0 ? pmat->xy / p.N * cos(ar) : pmat->yx / p.M * sin(ar));
        else
          ar = atan2(p.N * pmat->xx, p.M * pmat->yy), fr = 72.0 * (p.M == 0 ? pmat->yy / p.N * sin(ar) :  pmat->xx / p.M * cos(ar));
        ft = fabs(fr) * rt;
        at = (ar * radians_to_degrees - rotation) * reflection;
        at -= floor(at / 180.0) * 180.0;
        at += floor(a0 / 180.0) * 180.0;
        f_diff = fabs(ft - f0);
        a_diff = fabs(at - a0);
        f_err = f_diff / fabs(f0);
        a_err = a_diff;
        {
          double err = f_err * a_err;

          if (err > e_best)
            continue;
          e_best = err;
        }
        *phcp = p;
        f = ft, a = at;
        better = true;
        if (f_err <= 0.01 && a_err <= 0.9 )
        	goto done;
    	}
	}
	
	if (phcp->C < min_levels)
	{
		++rt;
    goto try_size;
	}
	if (better)
	{
		if (accurate) 
		{
    	++rt;
      goto try_size;
    }
	}
	else
	{
		if (rt == 1)
    	return (int)(XWPSError::RangeCheck);
	}
	
done:
	actual_frequency = f;
  actual_angle = a;
  return 0;
}

float XWPSScreenHalftone::spotFunction(float x, float y)
{
	return (this->*(proc.spot_function_.spot_function))(x,y);
}

XWPSColorScreenHalftone::XWPSColorScreenHalftone()
{
	screens.colored.red = new XWPSScreenHalftone;
	screens.colored.green = new XWPSScreenHalftone;
	screens.colored.blue = new XWPSScreenHalftone;
	screens.colored.gray = new XWPSScreenHalftone;
	
	screens.indexed[0] = screens.colored.red;
	screens.indexed[1] = screens.colored.green;
	screens.indexed[2] = screens.colored.blue;
	screens.indexed[3] = screens.colored.gray;
}

XWPSColorScreenHalftone::XWPSColorScreenHalftone(const XWPSColorScreenHalftone * other)
{
	screens.colored.red = new XWPSScreenHalftone(other->screens.colored.red);
	screens.colored.green = new XWPSScreenHalftone(other->screens.colored.green);
	screens.colored.blue = new XWPSScreenHalftone(*other->screens.colored.blue);
	screens.colored.gray = new XWPSScreenHalftone(other->screens.colored.gray);
	screens.indexed[0] = screens.colored.red;
	screens.indexed[1] = screens.colored.green;
	screens.indexed[2] = screens.colored.blue;
	screens.indexed[3] = screens.colored.gray;
}

XWPSColorScreenHalftone::~XWPSColorScreenHalftone()
{
	if (screens.colored.red)
	{
		delete screens.colored.red;
		screens.colored.red = 0;
	}
	if (screens.colored.green)
	{
		delete screens.colored.green;
		screens.colored.green = 0;
	}
	if (screens.colored.blue)
	{
		delete screens.colored.blue;
		screens.colored.blue = 0;
	}
	if (screens.colored.gray)
	{
		delete screens.colored.gray;
		screens.colored.gray = 0;
	}
}

XWPSSpotHalftone::XWPSSpotHalftone()
{
	accurate_screens = false;
}

XWPSSpotHalftone::~XWPSSpotHalftone()
{
}

XWPSThresholdHalftoneCommon::~XWPSThresholdHalftoneCommon()
{
}

XWPSThresholdHalftone::XWPSThresholdHalftone()
	:XWPSThresholdHalftoneCommon()
{
}

XWPSThresholdHalftone::~XWPSThresholdHalftone()
{
}

XWPSThresholdHalftone2::XWPSThresholdHalftone2()
	:XWPSThresholdHalftoneCommon(),
	 width2(0),
	 height2(0),
	 bytes_per_sample(0)
{
}

XWPSThresholdHalftone2::~XWPSThresholdHalftone2()
{
}

XWPSClientOrderHalftone::~XWPSClientOrderHalftone()
{
}

int XWPSClientOrderHalftone::createOrder(XWPSHTOrder * porder, XWPSState * pgs)
{
	return (this->*(procs->create_order))(porder, pgs);
}

XWPSHalftoneComponent::XWPSHalftoneComponent()
	:cname(ps_ht_separation_Default),
	 type(ht_type_none)
{
	params.spot = 0;
}

XWPSHalftoneComponent::~XWPSHalftoneComponent()
{
	switch (type)
	{
  	case ht_type_spot:
  		if (params.spot)
  		{
  			delete params.spot;
  			params.spot = 0;
  		}
  		break;
  		
  	case ht_type_threshold:
  		if (params.threshold)
  		{
  			delete params.threshold;
  			params.threshold = 0;
  		}
  		break;
  		
  	case ht_type_threshold2:
  		if (params.threshold2)
  		{
  			delete params.threshold2;
  			params.threshold2 = 0;
  		}
  		break;
  		
  	case ht_type_client_order:
  		if (params.client_order)
  		{
  			delete params.client_order;
  			params.client_order = 0;
  		}
  		break;
  		
  	default:
  		break;
	}
}

void XWPSHalftoneComponent::setType(PSHalftoneType t)
{
	type = t;
	switch (type)
	{
  	case ht_type_spot:
  		params.spot = new XWPSSpotHalftone;
  		break;
  		
  	case ht_type_threshold:
  		params.threshold = new XWPSThresholdHalftone;
  		break;
  		
  	case ht_type_threshold2:
  		params.threshold2 = new XWPSThresholdHalftone2;
  		break;
  		
  	case ht_type_client_order:
  		params.client_order = new XWPSClientOrderHalftone;
  		break;
  		
  	default:
  		break;
	}
}

XWPSMultipleHalftone::~XWPSMultipleHalftone()
{
	if (components)
	{
		delete [] components;
		components = 0;
	}
}

XWPSHalftone::XWPSHalftone()
	:XWPSStruct(),
	 type(ht_type_none)
{
	params.screen = 0;
}

XWPSHalftone::~XWPSHalftone()
{
	switch (type)
	{
  	case ht_type_screen:
  		if (params.screen)
  		{
  			delete params.screen;
  			params.screen = 0;
  		}
  		break;
  		
  	case ht_type_colorscreen:
  		if (params.colorscreen)
  		{
  			delete params.colorscreen;
  			params.colorscreen = 0;
  		}
  		break;
  		
  	case ht_type_spot:
  		if (params.spot)
  		{
  			delete params.spot;
  			params.spot = 0;
  		}
  		break;
  		
  	case ht_type_threshold:
  		if (params.threshold)
  		{
  			delete params.threshold;
  			params.threshold = 0;
  		}
  		break;
  		
  	case ht_type_threshold2:
  		if (params.threshold2)
  		{
  			delete params.threshold2;
  			params.threshold2 = 0;
  		}
  		break;
  		
  	case ht_type_multiple:
  	case ht_type_multiple_colorscreen:
  		if (params.multiple)
  		{
  			delete params.multiple;
  			params.multiple = 0;
  		}
  		break;
  		
  	case ht_type_client_order:
  		if (params.client_order)
  		{
  			delete params.client_order;
  			params.client_order = 0;
  		}
  		break;
  		
  	default:
  		break;
	}
}

int XWPSHalftone::getLength()
{
	return sizeof(XWPSHalftone);
}

const char * XWPSHalftone::getTypeName()
{
	return "halftone";
}

void XWPSHalftone::setType(PSHalftoneType t)
{
	type = t;
	switch (type)
	{
  	case ht_type_screen:
  		params.screen = new XWPSScreenHalftone;
  		break;
  		
  	case ht_type_colorscreen:
  		params.colorscreen = new XWPSColorScreenHalftone;
  		break;
  		
  	case ht_type_spot:
  		params.spot = new XWPSSpotHalftone;
  		break;
  		
  	case ht_type_threshold:
  		params.threshold = new XWPSThresholdHalftone;
  		break;
  		
  	case ht_type_threshold2:
  		params.threshold2 = new XWPSThresholdHalftone2;
  		break;
  		
  	case ht_type_multiple:
  	case ht_type_multiple_colorscreen:
  		params.multiple = new XWPSMultipleHalftone;
  		break;
  		
  	case ht_type_client_order:
  		params.client_order = new XWPSClientOrderHalftone;
  		break;
  		
  	default:
  		break;
	}
}

void XWPSHTCellParams::computeCellValues()
{
	const int MA = M, NA = N, M1A = M1, N1A = N1;
  const uint m = qAbs(MA), n = qAbs(NA);
  const uint m1 = qAbs(M1A), n1 = qAbs(N1A);
  const ulong CA = C = (ulong)m * m1 + (ulong)n * n1;
  const int DA = D = igcd(m1, n);
  const int D1A = D1 = igcd(m, n1);

  W = CA / DA, W1 = CA / D1A;
  if (M1A && NA)
  {
  	int h = 0, k = 0, dy = 0;
    int shift;
    while (dy != DA)
      if (dy > DA) 
      {
        if (M1A > 0)
          ++k;
        else
          --k;
        dy -= m1;
      } 
      else 
      {
        if (NA > 0)
          ++h;
        else
          --h;
        dy += n;
      }
   	shift = h * MA + k * N1A;
   	S = imod(-shift, W);
  }
  else
    S = 0;
}

XWPSHTOrderScreenParams::XWPSHTOrderScreenParams()
	:max_size(0)
{
}

void ps_ht_construct_bit(PSHTBit * bit, int width, int bit_num)
{
	uint padding = bitmap_raster(width) * 8 - width;
  int pix = bit_num;
  uint mask;
  uchar *pb;

  pix += pix / width * padding;
  bit->offset = (pix >> 3) & -sizeof(mask);
  mask = (uint) 1 << (~pix & (ht_mask_bits - 1));
  pix = ht_mask_bits - width;
  while ((pix -= width) >= 0)
		mask |= mask >> width;
		
  bit->mask = 0;
  for (pb = (uchar *) & bit->mask + (sizeof(mask) - 1); mask != 0;mask >>= 8, pb--)
		*pb = (uchar) mask;
}

static int compare_samples(const void *p1, const void *p2)
{
    uint m1 = ((const PSHTBit *)p1)->mask;
    uint m2 = ((const PSHTBit *)p2)->mask;

    return (m1 < m2 ? -1 : m1 > m2 ? 1 : 0);
}

void ps_sort_ht_order(PSHTBit * recs, uint N)
{
	for (uint i = 0; i < N; i++)
		recs[i].offset = i;
  qsort((void *)recs, N, sizeof(*recs), compare_samples);
}

const PSHTOrderProcs ht_order_procs_table[2] = {
    { sizeof(PSHTBit), &XWPSHTOrder::constructDefault, &XWPSHTOrder::bitIndexDefault,
      &XWPSHTOrder::renderDefault },
    { sizeof(ushort), &XWPSHTOrder::constructShort, &XWPSHTOrder::bitIndexShort,
      &XWPSHTOrder::renderShort }
};

#define ht_order_procs_default ht_order_procs_table[0]
#define ht_order_procs_short ht_order_procs_table[1]

ps_dht_proc ps_device_halftone_list[] = {
    0
};

XWPSHTOrder::XWPSHTOrder()
{
	width = 0;
	height = 0;
	raster = 0;
	shift = 0;
	orig_height = 0;
	orig_shift = 0;
	full_height = 0;
	num_levels = 0;
	num_bits = 0;
	procs = 0;
	levels = 0;
	bit_data = 0;
	cache = 0;
	transfer = 0;
}

XWPSHTOrder::~XWPSHTOrder()
{
	if (levels)
	{
		if (!levels->decRef())
			delete levels;
		levels = 0;
	}
	
	if (bit_data)
	{
		if (!bit_data->decRef())
			delete bit_data;
		bit_data = 0;
	}
	
	if (cache)
	{
		if (!cache->decRef())
		{
			cache->order.cache = 0;
			delete cache;
		}
		cache = 0;
	}
	
	if (transfer)
	{
		if (!transfer->decRef())
			delete transfer;
		transfer = 0;
	}
}

int XWPSHTOrder::alloc(uint widthA, 
	          uint heightA,
		        uint num_levelsA, 
		        uint num_bitsA, 
		        uint strip_shiftA,
		        const PSHTOrderProcs *procsA)
{
	if (levels)
	{
		if (!levels->decRef())
			delete levels;
		levels = 0;
	}
	
	if (bit_data)
	{
		if (!bit_data->decRef())
			delete bit_data;
		bit_data = 0;
	}
	
	width = widthA;
  height = heightA;
  raster = bitmap_raster(widthA);
  shift = strip_shiftA;
  orig_height = height;
  orig_shift = shift;
  full_height = fullHeight();
  num_levels = num_levelsA;
  num_bits = num_bitsA;
  procs = procsA;
  levels = new XWPSUints(num_levels);
  bit_data = new XWPSBytes(num_bits * procs->bit_data_elt_size);
  return 0;
}

int XWPSHTOrder::allocClientOrder(uint widthA, 
	                     uint heightA,
			                 uint num_levelsA, 
			                 uint num_bitsA)
{
	 XWPSHTOrder order;
  int code;

  order = *this;
  order.params.R = 1;
  order.params.R1 = 1;
  order.params.computeCellValues();
  code = order.alloc(widthA, heightA, num_levelsA, num_bitsA, 0, &ht_order_procs_default);
  *this = order;
  if (code < 0)
		return code;
  return 0;
}

int XWPSHTOrder::allocOrder(uint widthA, 
	               uint heightA,
		  					 uint strip_shiftA, 
		  					 uint num_levelsA)
{
	params.computeCellValues();
	int code = alloc(widthA, heightA, num_levelsA,widthA * heightA, strip_shiftA,	&ht_order_procs_default);
	return code;
}

int XWPSHTOrder::allocThresholdOrder(uint widthA, uint heightA, uint num_levelsA)
{
	XWPSHTOrder order;
	order = *this;
	uint num_bits = widthA * heightA;
  const PSHTOrderProcs *procs =	(num_bits > 2000 && num_bits <= max_ushort ?
	 								&ht_order_procs_short : &ht_order_procs_default);
  int code;

  order.params.computeCellValues();
  code = order.alloc(widthA, heightA, num_levelsA,	widthA * heightA, 0, procs);
  *this = order;
  if (code < 0)
		return code;
  return 0;
}

int XWPSHTOrder::bitIndex(uint index, XWPSIntPoint *ppt)
{
	return (this->*(procs->bit_index))(index, ppt);
}

int XWPSHTOrder::bitIndexDefault(uint index, XWPSIntPoint *ppt)
{
	const PSHTBit *phtb = &((const PSHTBit *)(bit_data->arr))[index];
  int bit = 0;

  while (!(((const uchar *)&phtb->mask)[bit >> 3] & (0x80 >> (bit & 7))))
		++bit;
  ppt->x = (phtb->offset % raster * 8) + bit;
  ppt->y = phtb->offset / raster;
  return 0;
}

int XWPSHTOrder::bitIndexShort(uint index, XWPSIntPoint *ppt)
{
	uint bit_index = ((const ushort *)(bit_data->arr))[index];
  uint bit_raster = raster * 8;

  ppt->x = bit_index % bit_raster;
  ppt->y = bit_index / bit_raster;
  return 0;
}

void XWPSHTOrder::completeThresholdOrder()
{
	uint size = num_bits;
  PSHTBit *bits = (PSHTBit*)(bit_data->arr);
  ps_sort_ht_order(bits, size);
  uint i, j;
  for (i = 0, j = 0; i < size; i++) 
  {
		if (bits[i].mask != j) 
		{
	    while (j < bits[i].mask)
				((levels->ptr)[j++]) = i;
		}
  }
  while (j < num_levels)
		((levels->ptr)[j++]) = size;
  constructBits();
}

void XWPSHTOrder::constructBits()
{
	uint i;
  PSHTBit *phb;

  for (i = 0, phb = (PSHTBit *)(bit_data->arr); i < num_bits; i++, phb++)
		ps_ht_construct_bit(phb, width, phb->offset);
}

int XWPSHTOrder::constructOrder(const uchar *thresholds)
{
	return (this->*(procs->construct_order))(thresholds);
}

int XWPSHTOrder::constructDefault(const uchar *thresholds)
{
	PSHTBit *bits = (PSHTBit *)(bit_data->arr);
  uint i;

  for (i = 0; i < num_bits; i++)
		bits[i].mask = qMax(1, (int)(thresholds[i]));
  completeThresholdOrder();
  return 0;
}

int XWPSHTOrder::constructShort(const uchar *thresholds)
{
	uint size = num_bits;
	ushort *bits = (ushort *)(bit_data->arr);
	memset(levels->ptr, 0, num_levels * sizeof(uint));
	for (uint i = 0; i < size; i++) 
	{
		uint value = qMax(1, (int)(thresholds[i]));

		if (value + 1 < num_levels)
	    ((levels->ptr)[value + 1])++;
  }
  
  for (uint i = 2; i < num_levels; ++i)
		((levels->ptr)[i]) += ((levels->ptr)[i - 1]);
		
	{
		uint padding = bitmap_raster(width) * 8 - width;

		for (uint i = 0; i < size; i++) 
		{
	    uint value = qMax(1, (int)(thresholds[i]));
	    bits[((levels->ptr)[value])++] = i + (i / width * padding);
		}
  }
  
  {
  	ps_dht_proc *phtrp = ps_device_halftone_list;

		for (; *phtrp; ++phtrp)
		{
			PSDeviceHalftoneResource *pphtr = (*phtrp)();
	    PSDeviceHalftoneResource *phtr;
	    while ((phtr = pphtr++) != 0)
	    {
	    	if (phtr->Width == width &&
		    		phtr->Height == height &&
		    		phtr->elt_size == sizeof(ushort) &&
		    		!memcmp(phtr->levels, levels->ptr, num_levels * sizeof(uint)) &&
		    		!memcmp(phtr->bit_data, bit_data->arr, size * phtr->elt_size))
				{
					if (levels)
					{
						if (!levels->decRef())
							delete levels;
						levels = 0;
					}
	
					if (bit_data)
					{
						if (!bit_data->decRef())
							delete bit_data;
						bit_data = 0;
					}
					
					levels = new XWPSUints((uint*)(phtr->levels));
		    	bit_data = new XWPSBytes((uchar*)(phtr->bit_data));
		    	goto out;
				}
	    }
		}
  }
out:
    return 0;
}

void XWPSHTOrder::constructSpotOrder()
{
	uint strip = num_levels / width;
	PSHTBit *bits = (PSHTBit *)(bit_data->arr);
	uint shift = orig_shift;
	uint copies = num_bits / (width * strip);
  PSHTBit *bp = bits + num_bits - 1;
  uint i;

  ps_sort_ht_order(bits, num_levels);
  for (i = num_levels; i > 0;) 
  {
		uint offset = bits[--i].offset;
		uint x = offset % width;
		uint hy = offset - x;
		uint k;

		((levels->ptr)[i]) = i * copies;
		for (k = 0; k < copies; k++, bp--, hy += num_levels, x = (x + width - shift) % width)
	    bp->offset = hy + x;
  }
  if (num_bits == width * full_height) 
  {
		height = full_height;
		shift = 0;
  }
  constructBits();
}

int XWPSHTOrder::constructThresholdOrder(const uchar * thresholds)
{
	return (this->*(procs->construct_order))(thresholds);
}

uint XWPSHTOrder::fullHeight()
{
	return shift == 0 ? height : width / igcd(width, shift) * height;
}

int XWPSHTOrder::processClientOrder(XWPSState * pgs, XWPSClientOrderHalftone * phcop)
{
	int code = phcop->createOrder(this, pgs);

  if (code < 0)
		return code;
  return processTransfer(pgs, phcop->transfer_closure);
}

int XWPSHTOrder::processSpot(XWPSState * pgs, XWPSSpotHalftone * phsp)
{
	XWPSScreenEnum senum;
  int code = senum.processScreenMemory(pgs, &phsp->screen, phsp->accurate_screens);

  if (code < 0)
		return code;
    
  *this = senum.order;  
  return processTransfer(pgs, &phsp->transfer);
}

int XWPSHTOrder::processTransfer(XWPSState * pgs, XWPSTransferMap * proc)
{
	if (!transfer)
		transfer = new XWPSTransferMap(proc);
	else
		*transfer = *proc;
	pgs->loadTransferMap(transfer, 0.0);
	return 0;
}

int XWPSHTOrder::processThreshold(XWPSState * pgs, XWPSThresholdHalftone * phtp)
{
	int code;

  params.M = phtp->width, params.N = 0;
  params.R = 1;
  params.M1 = phtp->height, params.N1 = 0;
  params.R1 = 1;
  code = allocThresholdOrder(phtp->width, phtp->height, 256);
  if (code < 0)
		return code;
    
  constructThresholdOrder(phtp->thresholds.data);
  return processTransfer(pgs, &phtp->transfer);
}

int XWPSHTOrder::processThreshold2(XWPSState * pgs, XWPSThresholdHalftone2 * phtp)
{
	int code;
	
#define LOG2_MAX_HT_LEVELS 14
#define MAX_HT_LEVELS (1 << LOG2_MAX_HT_LEVELS)
    
  int bps = phtp->bytes_per_sample;
  uchar *data = phtp->thresholds.data;
  const int w1 = phtp->width, h1 = phtp->height, size1 = w1 * h1;
  const int w2 = phtp->width2, h2 = phtp->height2, size2 = w2 * h2;
  const uint size = size1 + size2;
  const int d = (h2 == 0 ? h1 : igcd(h1, h2));
  const int sod = size / d;
  uint num_levels;
  uint i;
  int rshift = 0;
  int shift;

  {
		uint mask = 0, max_thr = 0;

		for (i = 0; i < size; ++i) 
		{
	    uint thr =(bps == 1 ? data[i] : (data[i * 2] << 8) + data[i * 2 + 1]);

	    mask |= thr;
	    max_thr = qMax(max_thr, thr);
		}
		if (mask == 0)
	    mask = 1, max_thr = 1;
		while (!(mask & 1) || max_thr > MAX_HT_LEVELS)
	    mask >>= 1, max_thr >>= 1, rshift++;
		num_levels = max_thr + 1;
  }
  
  params.M = sod, params.N = d;
  params.R = 1;
  params.M1 = d, params.N1 = sod;
  params.R1 = 1;
  shift = 0;
  {
		int x = 0, y = 0;

		do
		{
	    if (y < h1)
				x += w1, y += h2;
	    else
				x += w2, y -= h1;
		} while (y > d);
		if (y)
	    shift = x;
  }
  code = alloc(sod, d, num_levels, size, shift,	&ht_order_procs_default);
  if (code < 0)
		return code;
    
  {
		PSHTBit *bits = (PSHTBit *)bit_data;
		int row, di;
		for (row = 0, di = 0; row < d; ++row) 
		{
	    int dx, sy = row;	
	    int w;

	    for (dx = 0; dx < sod; dx += w) 
	    {
				int si, j;

				if (sy < h1) 
				{
		    	si = sy * w1;
		    	w = w1;
		    	sy += h2;
				} 
				else 
				{
		    	si = size1 + (sy - h1) * w2;
		    	w = w2;
		    	sy -= h1;
				}
				for (j = 0; j < w; ++j, ++si, ++di) 
				{
		    	uint thr =(bps == 1 ? data[si] :	 (data[si * 2] << 8) + data[si * 2 + 1])  >> rshift;
		    	bits[di].mask = qMax(thr, (uint)1);
				}
	    }
		}
  }
  completeThresholdOrder();
  return processTransfer(pgs, &phtp->transfer_closure);
#undef LOG2_MAX_HT_LEVELS
#undef MAX_HT_LEVELS
}

int XWPSHTOrder::render(XWPSHTTile *tile, int new_bit_level)
{
	return (this->*(procs->render))(tile, new_bit_level);
}

int XWPSHTOrder::renderDefault(XWPSHTTile *pbt, int level)
{
	int old_level = pbt->level;
  const PSHTBit *p =(const PSHTBit *)(bit_data->arr) + old_level;
  uchar *data = pbt->tiles.data;
  
#define INVERT_DATA(i)\
       do{uint *dp = (uint *)&data[p[i].offset];\
     *dp ^= p[i].mask;}while(0)
       
#define INVERT(i) INVERT_DATA(i)

sw:
	switch (level - old_level)
	{
		default:
	    if (level > old_level) 
	    {
				INVERT(0); INVERT(1); INVERT(2); INVERT(3);
				p += 4; old_level += 4;
	    } 
	    else 
	    {
				INVERT(-1); INVERT(-2); INVERT(-3); INVERT(-4);
				p -= 4; old_level -= 4;
	    }
	    goto sw;
	    
		case 7: INVERT(6);
		case 6: INVERT(5);
		case 5: INVERT(4);
		case 4: INVERT(3);
		case 3: INVERT(2);
		case 2: INVERT(1);
		case 1: INVERT(0);
		case 0: break;		/* Shouldn't happen! */
		case -7: INVERT(-7);
		case -6: INVERT(-6);
		case -5: INVERT(-5);
		case -4: INVERT(-4);
		case -3: INVERT(-3);
		case -2: INVERT(-2);
		case -1: INVERT(-1);
	}
#undef INVERT_DATA
#undef INVERT
  return 0;
}

int XWPSHTOrder::renderShort(XWPSHTTile *pbt, int level)
{
	int old_level = pbt->level;
  const ushort *p = (const ushort *)(bit_data->arr) + old_level;
  uchar *data = pbt->tiles.data;
  
#define INVERT_DATA(i)\
       do{uint bit_index = p[i];\
       uchar *dp = &data[bit_index >> 3];\
     *dp ^= 0x80 >> (bit_index & 7);}while(0)
       
#define INVERT(i) INVERT_DATA(i)

sw:
	switch (level - old_level)
	{
		default:
	    if (level > old_level) 
	    {
				INVERT(0); INVERT(1); INVERT(2); INVERT(3);
				p += 4; old_level += 4;
	    } 
	    else 
	    {
				INVERT(-1); INVERT(-2); INVERT(-3); INVERT(-4);
				p -= 4; old_level -= 4;
	    }
	    goto sw;
	    
		case 7: INVERT(6);
		case 6: INVERT(5);
		case 5: INVERT(4);
		case 4: INVERT(3);
		case 3: INVERT(2);
		case 2: INVERT(1);
		case 1: INVERT(0);
		case 0: break;		/* Shouldn't happen! */
		case -7: INVERT(-7);
		case -6: INVERT(-6);
		case -5: INVERT(-5);
		case -4: INVERT(-4);
		case -3: INVERT(-3);
		case -2: INVERT(-2);
		case -1: INVERT(-1);
	}
#undef INVERT_DATA
#undef INVERT
  return 0;
}

int XWPSHTOrder::screenOrderAlloc()
{
	uint num_levelsA = params.W * params.D;
  int code;
  if (!FORCE_STRIP_HALFTONES &&
        ((ulong)params.W1 * bitmap_raster(params.W) + num_levelsA * sizeof(uint) +
           params.W * params.W1 * sizeof(PSHTBit)) <= screen_params.max_size)
	{
		code = allocOrder(params.W, params.W1, 0, num_levelsA);
    height = orig_height = params.D;
    shift = orig_shift = params.S;
	}
	else
		code = allocOrder(params.W,params.D, params.S,num_levelsA);
	return code;
}

int XWPSHTOrder::screenOrderInitMemory(XWPSState * pgs,
                            XWPSScreenHalftone * phsp, 
                            bool accurate)
{
	XWPSMatrix imat;
  ulong max_size = pgs->ht_cache->bits_size;
  int code;

  if (phsp->frequency < 0.1)
    return (int)(XWPSError::RangeCheck);
  pgs->currentDevice()->getInitialMatrix(&imat);
  code = phsp->pickCellSize(&imat, max_size,  pgs->currentMinScreenLevels(), accurate, &params);
  if (code < 0)
     return code;
  params.computeCellValues();
  screen_params.matrix = imat;
  screen_params.max_size = max_size;
  return screenOrderAlloc();
}

XWPSHTOrder & XWPSHTOrder::operator=(const XWPSHTOrder & other)
{
	params = other.params;
	width = other.width;
  height = other.height;
  raster = other.raster;
  shift = other.shift;
  orig_height = other.orig_height;
  orig_shift = other.orig_shift;
  full_height = other.full_height;
  num_levels = other.num_levels;
  num_bits = other.num_bits;
  procs = other.procs;
  
  if (levels != other.levels)
	{
		if (levels)
			if (!levels->decRef())
				delete levels;
		levels = other.levels;
		if (levels)
			levels->incRef();
	}
	
	if (bit_data != other.bit_data)
	{
		if (bit_data)
			if (!bit_data->decRef())
				delete bit_data;
		bit_data = other.bit_data;
		if (bit_data)
			bit_data->incRef();
	}
	
	if (cache != other.cache)
	{
		if (!cache->decRef())
		{
			cache->order.cache = 0;
			delete cache;
		}
		
		cache = other.cache;
	  if (other.cache)
		  cache->incRef();
	}
		
	if (transfer != other.transfer)
	{
		if (!transfer->decRef())
			delete transfer;
		transfer = other.transfer;
	  if (other.transfer)
		  other.transfer->incRef();
	}
	
	return *this;
}

XWPSScreenEnum::XWPSScreenEnum()
	:XWPSStruct()
{
	halftone = new XWPSHalftone;
	x = y = 0;
	strip = shift = 0;
	pgs = 0;
}

XWPSScreenEnum::~XWPSScreenEnum()
{
	if (halftone)
	{
		if (!halftone->decRef())
			delete halftone;
		halftone = 0;
	}
}

int XWPSScreenEnum::currentPoint(XWPSPoint * ppt)
{
	XWPSPoint pt;
  int code;
  double sx, sy;
  XWPSPoint spot_center;
  if (y >= strip) 
  {
     order.constructSpotOrder();
     return 1;
  }
  
  if ((code = pt.transform(x + 0.501, y + 0.498, &mat)) < 0)
    return code;
    
  sx = ceil( pt.x / 2 ) * 2;
  sy = ceil( pt.y / 2 ) * 2;
  if ((code = spot_center.transform(sx, sy, &mat_inv)) < 0)
    return code;
    
  spot_center.x = floor(spot_center.x) + 0.5;
  spot_center.y = floor(spot_center.y) + 0.5;
  if ((code = pt.distanceTransform(x - spot_center.x + 0.501, y - spot_center.y + 0.498, &mat)) < 0)
    return code;
  pt.x += 1;
  pt.y += 1;
  
  if (pt.x < -1.0)
    pt.x += ((int)(-ceil(pt.x)) + 1) & ~1;
  else if (pt.x >= 1.0)
    pt.x -= ((int)pt.x + 1) & ~1;
  if (pt.y < -1.0)
    pt.y += ((int)(-ceil(pt.y)) + 1) & ~1;
  else if (pt.y >= 1.0)
    pt.y -= ((int)pt.y + 1) & ~1;
  *ppt = pt;
  return 0;
}

int XWPSScreenEnum::getLength()
{
	return sizeof(XWPSScreenEnum);
}

const char * XWPSScreenEnum::getTypeName()
{
	return "screenenum";
}

int XWPSScreenEnum::init(XWPSState * pgsA, XWPSScreenHalftone * phsp)
{
	return initMemory(pgsA, phsp, pgs->currentAccurateScreens());
}

int XWPSScreenEnum::initMemory(XWPSState * pgsA,
                 XWPSScreenHalftone * phsp, 
                 bool accurate)
{
	int code = order.screenOrderInitMemory(pgsA, phsp, accurate);

  if (code < 0)
     return code;
  return initMemory(&order, pgsA, phsp);
}

int XWPSScreenEnum::initMemory(XWPSHTOrder * porder,
                               XWPSState * pgsA, 
                               XWPSScreenHalftone * phsp)
{
	pgs = pgsA;
	if (&order != porder)
  	order = *porder;
  halftone->type = ht_type_screen;
  halftone->params.screen = new XWPSScreenHalftone(phsp);
  strip = porder->num_levels / porder->width;
  shift = porder->shift;
  {
    const int M = porder->params.M, N = porder->params.N, R = porder->params.R;
    const int M1 = porder->params.M1, N1 = porder->params.N1, R1 = porder->params.R1;
    double Q = 2.0 / ((long)M * M1 + (long)N * N1);

    mat.xx = Q * (R * M1);
    mat.xy = Q * (-R1 * N);
    mat.yx = Q * (R * N1);
    mat.yy = Q * (R1 * M);
    mat.tx = -1.0;
    mat.ty = -1.0;
    mat_inv.matrixInvert(&mat);
  }
  return 0;
}

int XWPSScreenEnum::install()
{
	XWPSDeviceHalftone * dev_ht = new XWPSDeviceHalftone;
	dev_ht->order = order;
	
  int code = pgs->install(halftone, dev_ht);
  return code;
}

int XWPSScreenEnum::next(float value)
{
	uint sample;
  int width = order.width;
  PSHTBit *bits = (PSHTBit *)(order.bit_data->arr);

  if (value < -1.0 || value > 1.0)
     return (int)(XWPSError::RangeCheck);
  sample = (uint) ((value + 1) * max_ht_sample);
  bits[y * width + x].mask = sample;
  if (++(x) >= width)
    x = 0, ++(y);
  return 0;
}

int XWPSScreenEnum::processScreenMemory(XWPSState * pgsA,
													XWPSScreenHalftone * phsp, 
													bool accurate)
{
	XWPSPoint pt;
  int code = initMemory(pgsA, phsp, accurate);

  if (code < 0)
		return code;
  
  while ((code = currentPoint(&pt)) == 0)
		if ((code = next(phsp->spotFunction(pt.x, pt.y))) < 0)
	    return code;
  return 0;
}

XWPSHTTile::XWPSHTTile()
{
	level = 0;
	index = 0;
}

XWPSHTTile::XWPSHTTile(const XWPSHTTile * other)
{
	tiles = other->tiles;
	level = other->level;
	index = other->index;
}

XWPSHTCache::XWPSHTCache()
	:XWPSStruct()
{
	bits = new uchar[max_ht_bits_LARGE];
	bits_size = max_ht_bits_LARGE;
	ht_tiles = new XWPSHTTile[max_cached_tiles_LARGE];
	num_tiles = max_cached_tiles_LARGE;
	order.cache = this;
  order.transfer = 0;
  clear();
}

XWPSHTCache::XWPSHTCache(uint max_tiles, uint max_bits)
	:XWPSStruct()
{
	bits = new uchar[max_bits];
  bits_size = max_bits;
  ht_tiles = new XWPSHTTile[max_tiles];
  num_tiles = max_tiles;
  order.cache = this;
  order.transfer = 0;
  clear();
}

XWPSHTCache::XWPSHTCache(const XWPSHTCache * other)
{
	bits = other->bits;
	bits_size = other->bits_size;
	if (other->bits)
	{
		bits = new uchar[other->bits_size];
		memcpy(bits, other->bits, other->bits_size);
	}
	ht_tiles = other->ht_tiles;
	num_tiles = other->num_tiles;
	if (other->ht_tiles)
	{
		ht_tiles = new XWPSHTTile[other->num_tiles];
		memcpy(ht_tiles, other->ht_tiles, other->num_tiles * sizeof(XWPSHTTile));
	}
	
	num_cached = other->num_cached;
	levels_per_tile = other->levels_per_tile;
	order = other->order;
	tiles_fit = other->tiles_fit;
	base_id = other->base_id;
}

XWPSHTCache::~XWPSHTCache()
{
	order.cache = 0;
	if (bits)
	{
		delete [] bits;
		bits = 0;
	}
	
	if (ht_tiles)
	{
		delete [] ht_tiles;
		ht_tiles = 0;
	}
}

XWPSHTCache & XWPSHTCache::operator=(const XWPSHTCache & other)
{
	if (bits)
	{
		delete [] bits;
		bits = 0;
	}
	
	if (other.bits)
	{
		bits = new uchar[other.bits_size];
		memcpy(bits, other.bits, other.bits_size);
	}
	
	if (ht_tiles)
	{
		delete [] ht_tiles;
		ht_tiles = 0;
	}
	
	if (other.ht_tiles)
	{
		ht_tiles = new XWPSHTTile[other.num_tiles];
		memcpy(ht_tiles, other.ht_tiles, other.num_tiles * sizeof(XWPSHTTile));
	}
	
	bits_size = other.bits_size;
	num_tiles = other.num_tiles;
	num_cached = other.num_cached;
	levels_per_tile = other.levels_per_tile;
	order = other.order;
	tiles_fit = other.tiles_fit;
	base_id = other.base_id;
	
	return *this;
}

void XWPSHTCache::clear()
{
	if (order.levels)
			if (!order.levels->decRef())
				delete order.levels;
	order.levels = 0;
	
	if (order.bit_data)
			if (!order.bit_data->decRef())
				delete order.bit_data;
	order.bit_data = 0;
	if (ht_tiles)
		ht_tiles[0].tiles.data = 0;
}

int XWPSHTCache::getLength()
{
	return sizeof(XWPSHTCache);
}

const char * XWPSHTCache::getTypeName()
{
	return "htcache";
}

void XWPSHTCache::init(XWPSHTOrder * porder)
{
	uint width = porder->width;
  uint height = porder->height;
  uint size = width * height + 1;
  int width_unit = (width <= ht_mask_bits / 2 ? ht_mask_bits / width * width : width);
  int height_unit = height;
  uint raster = porder->raster;
  uint tile_bytes = raster * height;
  uint shift = porder->shift;
  int i;
  uchar *tbits = bits;

  if (porder->num_bits >= size)
		size = porder->num_bits + 1;
  num_cached = bits_size / tile_bytes;
  if (num_cached > size)
		num_cached = size;
  if (num_cached > num_tiles)
		num_cached = num_tiles;
  if (num_cached == size &&	tile_bytes * num_cached <= bits_size / 2) 
  {
		uint rep_raster =	((bits_size / num_cached) / height) &	~(align_bitmap_mod - 1);
		uint rep_count = rep_raster * 8 / width;
		if (rep_count > sizeof(ulong) * 8)
	    rep_count = sizeof(ulong) * 8;
		width_unit = width * rep_count;
		raster = bitmap_raster(width_unit);
		tile_bytes = raster * height;
  }
  order = *porder;
  order.transfer = 0;
  levels_per_tile = (size + num_cached - 1) / num_cached;
  tiles_fit = -1;
  memset(tbits, 0, bits_size);
  for (i = 0; i < num_cached; i++, tbits += tile_bytes) 
  {
		XWPSHTTile *bt = &ht_tiles[i];
		bt->index = i;
		bt->tiles.data = tbits;
		bt->tiles.raster = raster;
		bt->tiles.size.x = width_unit;
		bt->tiles.size.y = height_unit;
		bt->tiles.rep_width = width;
		bt->tiles.rep_height = height;
		bt->tiles.shift = bt->tiles.rep_shift = shift;
  }
  render_ht =	(num_tiles == 1 ? &XWPSHTCache::renderHt1Tile :
	 	levels_per_tile == 1 ? &XWPSHTCache::renderHt1Level :	 &XWPSHTCache::renderHtDefault);
}

XWPSHTTile * XWPSHTCache::render(int t)
{
	return (this->*render_ht)(t);
}

int XWPSHTCache::renderHt(XWPSHTTile * pbt, int level,
	              XWPSHTOrder * porder, 
	              ulong new_id)
{
	uchar *data = pbt->tiles.data;
  int code;
  
  code = porder->render(pbt, level);
  if (code < 0)
		return code;
    
  pbt->level = level;
  pbt->tiles.id = new_id;
  
  if (pbt->tiles.raster > porder->raster)
		bits_replicate_horizontally(data, pbt->tiles.rep_width,
				    pbt->tiles.rep_height, porder->raster,   pbt->tiles.size.x, pbt->tiles.raster);
  if (pbt->tiles.size.y > pbt->tiles.rep_height &&	pbt->tiles.shift == 0)
		bits_replicate_vertically(data, pbt->tiles.rep_height,  pbt->tiles.raster, pbt->tiles.size.y);

  return 0;
}

XWPSHTTile * XWPSHTCache::renderHtDefault(int b_level)
{
	XWPSHTOrder *porder = &order;
  int level = ((porder->levels->ptr)[b_level]);
  XWPSHTTile *bt = &ht_tiles[level / levels_per_tile];

  if (bt->level != level) 
  {
		int code = renderHt(bt, level, porder, base_id + b_level);

		if (code < 0)
	    return 0;
  }
  return bt;
}

XWPSHTTile * XWPSHTCache::renderHt1Tile(int b_level)
{
	XWPSHTOrder *porder = &order;
  int level = ((porder->levels->ptr)[b_level]);
  XWPSHTTile *bt = &ht_tiles[0];

  if (bt->level != level) 
  {
		int code = renderHt(bt, level, porder, base_id + b_level);

		if (code < 0)
	    return 0;
  }
  return bt;
}

XWPSHTTile * XWPSHTCache::renderHt1Level(int b_level)
{
	XWPSHTOrder *porder = &order;
  int level = ((porder->levels->ptr)[b_level]);
  XWPSHTTile *bt = &ht_tiles[level];

  if (bt->level != level) 
  {
		int code = renderHt(bt, level, porder, base_id + b_level);

		if (code < 0)
	    return 0;
  }
  return bt;
}

XWPSDeviceHalftone::XWPSDeviceHalftone()
	:XWPSStruct()
{
	id = 0;
	type = ht_type_none;
	components = 0;
	num_comp = 0;
	color_indices[0] = 0;
	color_indices[1] = 0;
	color_indices[2] = 0;
	color_indices[3] = 0;
	lcm_width = 0;
	lcm_height = 0;
}

XWPSDeviceHalftone::~XWPSDeviceHalftone()
{
	if (components)
	{
		delete [] components;
		components = 0;
	}
}

int XWPSDeviceHalftone::getLength()
{
	return sizeof(XWPSDeviceHalftone);
}

const char * XWPSDeviceHalftone::getTypeName()
{
	return "devicehalftone";
}
