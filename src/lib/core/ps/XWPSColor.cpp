/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <math.h>
#include "XWPSError.h"
#include "psbittable.h"
#include "XWPSBitmap.h"
#include "XWPSHalfTone.h"
#include "XWPSShading.h"
#include "XWPSPattern.h"
#include "XWPSDevicePatternAccum.h"
#include "XWPSDeviceMaskClip.h"
#include "XWPSState.h"
#include "XWPSRef.h"
#include "XWPSContextState.h"
#include "XWPSColor.h"

#define SET_PLANE_COLOR_CONSTANT(i)\
    do{pvp->values[1][i] = pvp->values[0][i] = colors.colored.c_base[i];\
  sbits[i] = &ht_no_bitmap;}while(0)

#define SET_PLANE_COLOR(i)\
    do{uint q = colors.colored.c_base[i];\
    uint r = colors.colored.c_level[i];\
\
    pvp->values[0][i] = fractional_color(q, max_color);\
    if (r == 0)\
	pvp->values[1][i] = pvp->values[0][i], sbits[i] = &ht_no_bitmap;\
    else if (!invert) {\
	pvp->values[1][i] = fractional_color(q + 1, max_color);\
	sbits[i] =  &(caches[i]->render(r)->tiles);\
    } else {                                                        \
	 XWPSDeviceHalftone *pdht = colors.colored.c_ht;  \
	int nlevels =\
	    (pdht->components ?\
	     pdht->components[pdht->color_indices[i]].corder.num_levels :\
	     pdht->order.num_levels);\
\
	pvp->values[1][i] = pvp->values[0][i];                   \
	pvp->values[0][i] = fractional_color(q + 1, max_color);   \
	sbits[i] = &(caches[i]->render(nlevels - r)->tiles);    \
  }}while(0)
    
static void
init_tile_cursor(int , PSTileCursor *ptc, XWPSStripBitmap *btile, int endx, int lasty)
{
    int tw = btile->size.x;
    int bx = ((ptc->tile_shift = btile->shift) == 0 ? endx :
	      endx + lasty / btile->size.y * ptc->tile_shift) % tw;
    int by = lasty % btile->size.y;

    ptc->xoffset = bx >> 3;
    ptc->xshift = 8 - (bx & 7);
    ptc->xbytes = (tw - 1) >> 3;
    ptc->xbits = ((tw - 1) & 7) + 1;
    ptc->tdata = btile->data;
    ptc->raster = btile->raster;
    ptc->row = ptc->tdata + by * ptc->raster;
    ptc->data = ptc->row + ptc->xoffset;
    ptc->bit_shift = ptc->xshift;
}

static void
wrap_shifted_cursor(PSTileCursor *ptc, XWPSStripBitmap *psbit)
{
    ptc->row += ptc->raster * (psbit->size.y - 1);
    if (ptc->tile_shift) 
    {
			if ((ptc->xshift += ptc->tile_shift) >= 8) 
			{
	    	if ((ptc->xoffset -= ptc->xshift >> 3) < 0) 
	    	{
					int bx = (ptc->xoffset << 3) + 8 - (ptc->xshift & 7) +   psbit->size.x;

					ptc->xoffset = bx >> 3;
					ptc->xshift = 8 - (bx & 7);
	    	} 
	    	else
					ptc->xshift &= 7;
			}
    }
}

#define STEP_ROW(c, i)\
    if (c.row > c.tdata)\
      c.row -= c.raster;\
    else {	/* wrap around to end of tile */\
	wrap_shifted_cursor(&c, sbits[i]);\
    }\
    c.data = c.row + c.xoffset;\
    c.bit_shift = c.xshift;

/* Define a table for expanding 8x1 bits to 8x4. */
static const quint32 expand_8x1_to_8x4[256] = {
#define X16(c)\
  c+0, c+1, c+0x10, c+0x11, c+0x100, c+0x101, c+0x110, c+0x111,\
  c+0x1000, c+0x1001, c+0x1010, c+0x1011, c+0x1100, c+0x1101, c+0x1110, c+0x1111
    X16(0x00000000), X16(0x00010000), X16(0x00100000), X16(0x00110000),
    X16(0x01000000), X16(0x01010000), X16(0x01100000), X16(0x01110000),
    X16(0x10000000), X16(0x10010000), X16(0x10100000), X16(0x10110000),
    X16(0x11000000), X16(0x11010000), X16(0x11100000), X16(0x11110000)
#undef X16
};

static void
set_color_ht_le_4(uchar *dest_data, uint dest_raster, int px, int py,
		  int w, int h, int depth, int special, int ,
		  ulong plane_mask, XWPSDevice *,
		  const PSColorValuesPair *,
		  ulong * colors,
		  XWPSStripBitmap ** sbits)
{
    int x, y;
    PSTileCursor cursor[MAX_DCC];
    int dbytes = depth >> 3;
    uchar *dest_row =	dest_data + dest_raster * (h - 1) + (w * depth) / 8;

    if (special > 0) 
    {
			plane_mask =
	    "\000\010\004\014\002\012\006\016\001\011\005\015\003\013\007\017"[plane_mask];
    }

    {
			int endx = w + px;
			int lasty = h - 1 + py;

			if (plane_mask & 1)
	    	init_tile_cursor(0, &cursor[0], sbits[0], endx, lasty);
			if (plane_mask & 2)
	    	init_tile_cursor(1, &cursor[1], sbits[1], endx, lasty);
			if (plane_mask & 4)
	    	init_tile_cursor(2, &cursor[2], sbits[2], endx, lasty);
			if (plane_mask & 8)
	    	init_tile_cursor(3, &cursor[3], sbits[3], endx, lasty);
    }

    for (y = h; ; dest_row -= dest_raster) 
    {
			uchar *dest = dest_row;

			--y;
			for (x = w; x > 0;) 
			{
	    	quint32 indices;
	    	int nx, i;
	    	uint bits;

#define NEXT_BITS(c)\
    if (c.data > c.row) {\
      bits = ((c.data[-1] << 8) | *c.data) >> c.bit_shift;\
      c.data--;\
    } else {\
      bits = *c.data >> c.bit_shift;\
      c.data += c.xbytes;\
      if ((c.bit_shift -= c.xbits) < 0) {\
	bits |= *c.data << -c.bit_shift;\
	c.bit_shift += 8;\
      } else {\
	bits |= ((c.data[-1] << 8) | *c.data) >> c.bit_shift;\
	c.data--;\
      }\
    }
    
	    if (plane_mask & 1) 
	    {
				NEXT_BITS(cursor[0]);
				indices = expand_8x1_to_8x4[bits & 0xff];
	    } 
	    else
				indices = 0;
	    if (plane_mask & 2) 
	    {
				NEXT_BITS(cursor[1]);
				indices |= expand_8x1_to_8x4[bits & 0xff] << 1;
	    }
	    if (plane_mask & 4) 
	    {
				NEXT_BITS(cursor[2]);
				indices |= expand_8x1_to_8x4[bits & 0xff] << 2;
	    }
	    if (plane_mask & 8) 
	    {
				NEXT_BITS(cursor[3]);
				indices |= expand_8x1_to_8x4[bits & 0xff] << 3;
	    }
#undef NEXT_BITS
	    nx = qMin(x, 8);	
	    x -= nx;
	    switch (dbytes) 
	    {
				case 0:	/* 4 */
		    	if (special > 0) 
		    	{
						indices = (indices & colors[1]) | (~indices & colors[0]);
						i = nx;
						if ((x + nx) & 1) 
						{
			    		*dest = (*dest & 0xf) +	((indices & 0xf) << 4);
			    		indices >>= 4;
			    		--i;
						}
						for (; (i -= 2) >= 0; indices >>= 8)
			    		*--dest = (uchar)indices;
			    		
						if (i & 1)
			    		*--dest = indices & 0xf;
		    	} 
		    	else 
		    	{
						i = nx;
						if ((x + nx) & 1) 
						{
			    		*dest = (*dest & 0xf) +	((uchar)colors[indices & 0xf] << 4);
			    		indices >>= 4;
			    		--i;
						}
						
						for (; (i -= 2) >= 0; indices >>= 8)
			    		*--dest =	(uchar)colors[indices & 0xf] +	((uchar)colors[(indices >> 4) & 0xf] << 4);
						if (i & 1)
			    		*--dest = (uchar)colors[indices & 0xf];
		    	}
		    	break;
		    	
				case 4:	/* 32 */
		    	for (i = nx; --i >= 0; indices >>= 4) 
		    	{
						quint32 tcolor = (quint32)colors[indices & 0xf];

						dest -= 4;
						dest[3] = (uchar)tcolor;
						dest[2] = (uchar)(tcolor >> 8);
						tcolor >>= 16;
						dest[1] = (uchar)tcolor;
						dest[0] = (uchar)(tcolor >> 8);
		    	}
		    	break;
		    	
				case 3:	/* 24 */
		    	for (i = nx; --i >= 0; indices >>= 4) 
		    	{
						quint32 tcolor = (quint32)colors[indices & 0xf];

						dest -= 3;
						dest[2] = (uchar) tcolor;
						dest[1] = (uchar)(tcolor >> 8);
						dest[0] = (uchar)(tcolor >> 16);
		    	}
		    	break;
		    	
				case 2:	/* 16 */
		    	for (i = nx; --i >= 0; indices >>= 4) 
		    	{
						uint tcolor = (uint)colors[indices & 0xf];

						dest -= 2;
						dest[1] = (uchar)tcolor;
						dest[0] = (uchar)(tcolor >> 8);
		    	}
		    	break;
		    	
				case 1:	/* 8 */
		    	for (i = nx; --i >= 0; indices >>= 4)
						*--dest = (uchar)colors[indices & 0xf];
		    	break;
	    }
		}
		if (y == 0)
	    break;

		if (plane_mask & 1)
		{
	    STEP_ROW(cursor[0], 0);
	  }
		if (plane_mask & 2)
		{
	    STEP_ROW(cursor[1], 1);
	  }
		if (plane_mask & 4)
		{
	    STEP_ROW(cursor[2], 2);
	  }
		if (plane_mask & 8)
		{
	    STEP_ROW(cursor[3], 3);
	  }
  }
}

static void
set_color_ht_gt_4(uchar *dest_data, uint dest_raster, int px, int py,
		  int w, int h, int depth, int , int num_planes,
		  ulong plane_mask, XWPSDevice *dev,
		  const PSColorValuesPair *pvp,
		  ulong * colors,
		  XWPSStripBitmap ** sbits)
{
    int x, y;
    PSTileCursor cursor[MAX_DCC];
    int dbytes = depth >> 3;
    uchar *dest_row =	dest_data + dest_raster * (h - 1) + (w * depth) / 8;
    int pmin, pmax;
    ushort cv[MAX_DCC];

    if (plane_mask == 0)
			pmin = 0, pmax = -1;
    else 
    {
			for (pmin = 0; !((plane_mask >> pmin) & 1); )
	    	++pmin;
			for (pmax = 0; (plane_mask >> pmax) > 1; )
	    	++pmax;
    }

    {
			int endx = w + px;
			int lasty = h - 1 + py;
			int i;

			for (i = pmin; i <= pmax; ++i)
	    	if ((plane_mask >> i) & 1)
					init_tile_cursor(i, &cursor[i], sbits[i], endx, lasty);
    }

    {
			int i;

			for (i = 0; i < pmin; ++i)
	    	cv[i] = pvp->values[0][i];
			for (i = pmax + 1; i < num_planes; ++i)
	    	cv[i] = pvp->values[0][i];
    }

    for (y = h; ; dest_row -= dest_raster) 
    {
			uchar *dest = dest_row;
			int i;

			--y;
			for (x = w; x > 0;) 
			{
	    	ulong index = 0;
	    	ulong tcolor;

	    	for (i = pmin; i <= pmax; ++i)
					if ((plane_mask >> i) & 1) 
					{
		    		PSTileCursor *ptc = &cursor[i];
		    		uchar tile_bit;

b:		    
						if (ptc->bit_shift < 8)
							tile_bit = *ptc->data >> ptc->bit_shift++;
		    		else if (ptc->data > ptc->row) 
		    		{
							tile_bit = *--(ptc->data);
							ptc->bit_shift = 1;
		    		} 
		    		else 
		    		{
							ptc->data += ptc->xbytes;
							ptc->bit_shift = 8 - ptc->xbits;
							goto b;
		    		}
		    		index |= (ulong)(tile_bit & 1) << i;
					}
	    		tcolor = colors[index];
	    		if (tcolor == ps_no_color_index) 
	    		{
						int i;

						for (i = pmin; i <= pmax; ++i)
		    			cv[i] = pvp->values[(index >> i) & 1][i];
						tcolor = colors[index] = dev->mapCMYKColor(cv[0], cv[1], cv[2], cv[3]);
	    		}
	    		--x;
	    		switch (dbytes) 
	    		{
						case 0:	
		    			if (x & 1) 
		    			{ 
								*--dest = (uchar)tcolor;
		    			} 
		    			else 
		    			{
								*dest = (*dest & 0xf) + ((uchar)tcolor << 4);
		    			}
		    			break;
		    			
						case 4:	/* 32 */
		    			dest[-4] = (uchar)(tcolor >> 24);
						case 3:	/* 24 */
		    			dest[-3] = (uchar)(tcolor >> 16);
						case 2:	/* 16 */
		    			dest[-2] = (uchar)(tcolor >> 8);
						case 1:	/* 8 */
		    			dest[-1] = (uchar)tcolor;
		    			dest -= dbytes;
		    			break;
	    		}
				}
				if (y == 0)
	    		break;
				for (i = pmin; i <= pmax; ++i)
	    		if ((plane_mask >> i) & 1)
	    		{
						STEP_ROW(cursor[i], i);
					}
    }
}

static PSRopSource ps_rop_no_source_0 = {NULL, 0, 0, 0, {0, 0}, true};
static PSRopSource ps_rop_no_source_1 = {NULL, 0, 0, 0, {1, 1}, true};

static const ushort q0[] = {
    0
};
static const ushort q1[] = {
    0, frac_color_(1, 1)
};
static const ushort q2[] = {
    0, frac_color_(1, 2), frac_color_(2, 2)
};
static const ushort q3[] = {
    0, frac_color_(1, 3), frac_color_(2, 3), frac_color_(3, 3)
};
static const ushort q4[] = {
    0, frac_color_(1, 4), frac_color_(2, 4), frac_color_(3, 4),
    frac_color_(4, 4)
};
static const ushort q5[] = {
    0, frac_color_(1, 5), frac_color_(2, 5), frac_color_(3, 5),
    frac_color_(4, 5), frac_color_(5, 5)
};
static const ushort q6[] = {
    0, frac_color_(1, 6), frac_color_(2, 6), frac_color_(3, 6),
    frac_color_(4, 6), frac_color_(5, 6), frac_color_(6, 6)
};
static const ushort q7[] = {
    0, frac_color_(1, 7), frac_color_(2, 7), frac_color_(3, 7),
    frac_color_(4, 7), frac_color_(5, 7), frac_color_(6, 7), frac_color_(7, 7)
};

/* We export fc_color_quo for the fractional_color macro in gzht.h. */

#define WNUM 128000
#define	WEIGHT1		(ulong)(WNUM/1000)	/* 1.0                    */
#define	WEIGHT2		(ulong)(WNUM/1414)	/* 1/sqrt(2.0)            */
#define	WEIGHT3		(ulong)(WNUM/1732)	/* 1/sqrt(3.0)            */
#define WEIGHT4		(ulong)(WNUM/2000)	/* 1/sqrt(4.0)            */

#define	DIAG_R		(0x1)
#define	DIAG_G		(0x2)
#define	DIAG_B		(0x4)
#define DIAG_W		(0x8)
#define	DIAG_RG		(0x3)
#define	DIAG_GB		(0x6)
#define	DIAG_BR		(0x5)
#define	DIAG_RGB	(0x7)
#define DIAG_RGBW	(0xf)

#define lum_white_weight 0
static const unsigned short lum_w[16] =
{
    (0 * lum_blue_weight + 0 * lum_green_weight + 0 * lum_red_weight + 0 * lum_white_weight),
    (0 * lum_blue_weight + 0 * lum_green_weight + 1 * lum_red_weight + 0 * lum_white_weight),
    (0 * lum_blue_weight + 1 * lum_green_weight + 0 * lum_red_weight + 0 * lum_white_weight),
    (0 * lum_blue_weight + 1 * lum_green_weight + 1 * lum_red_weight + 0 * lum_white_weight),
    (1 * lum_blue_weight + 0 * lum_green_weight + 0 * lum_red_weight + 0 * lum_white_weight),
    (1 * lum_blue_weight + 0 * lum_green_weight + 1 * lum_red_weight + 0 * lum_white_weight),
    (1 * lum_blue_weight + 1 * lum_green_weight + 0 * lum_red_weight + 0 * lum_white_weight),
    (1 * lum_blue_weight + 1 * lum_green_weight + 1 * lum_red_weight + 0 * lum_white_weight),
    (0 * lum_blue_weight + 0 * lum_green_weight + 0 * lum_red_weight + 1 * lum_white_weight),
    (0 * lum_blue_weight + 0 * lum_green_weight + 1 * lum_red_weight + 1 * lum_white_weight),
    (0 * lum_blue_weight + 1 * lum_green_weight + 0 * lum_red_weight + 1 * lum_white_weight),
    (0 * lum_blue_weight + 1 * lum_green_weight + 1 * lum_red_weight + 1 * lum_white_weight),
    (1 * lum_blue_weight + 0 * lum_green_weight + 0 * lum_red_weight + 1 * lum_white_weight),
    (1 * lum_blue_weight + 0 * lum_green_weight + 1 * lum_red_weight + 1 * lum_white_weight),
    (1 * lum_blue_weight + 1 * lum_green_weight + 0 * lum_red_weight + 1 * lum_white_weight),
    (1 * lum_blue_weight + 1 * lum_green_weight + 1 * lum_red_weight + 1 * lum_white_weight)
};

XWPSClientColor::XWPSClientColor()
	:XWPSStruct()
{
	for (int i = 0; i < PS_CLIENT_COLOR_MAX_COMPONENTS; i++)
		paint.values[i] = 0.0;
		
	pattern = 0;
}

XWPSClientColor::XWPSClientColor(const XWPSClientColor * other)
	:XWPSStruct()
{
	memcpy(paint.values, other->paint.values, PS_CLIENT_COLOR_MAX_COMPONENTS * sizeof(float));
	if (other->pattern)
	{
		pattern = other->pattern;
		pattern->incRef();
	}
	else
		pattern = 0;
}

XWPSClientColor::~XWPSClientColor()
{
	if (pattern)
	{
		pattern->decRef();
		pattern = 0;
	}
}

XWPSPattern * XWPSClientColor::getPattern()
{
	return pattern->getPattern();
}

int XWPSClientColor::makePattern(XWPSPattern * pcp,	
	                 XWPSMatrix * pmat, 
	                XWPSState * pgs)
{
	return pcp->makePattern(this, pmat, pgs);
}

int XWPSClientColor::zPaintProc(XWPSState * pgs)
{
	XWPSIntGState * iigs = (XWPSIntGState*)(pgs->client_data);
	XWPSIntRemapColorInfo * info = (XWPSIntRemapColorInfo*)(iigs->remap_color_info.getStruct());
	info->proc = &XWPSContextState::patternPaintPrepare;
  return (int)(XWPSError::RemapColor);
}

XWPSClientColor & XWPSClientColor::operator=(XWPSClientColor & other)
{
	memcpy(paint.values, other.paint.values, PS_CLIENT_COLOR_MAX_COMPONENTS * sizeof(float));
	
	if (pattern != other.pattern)
	{
		if (pattern)
			pattern->decRef();
	
		pattern = other.pattern;
		if (pattern)
			pattern->incRef();
	}
	
	return *this;
}

XWPSTileFillState::XWPSTileFillState()
{
	pdevc = 0;
	x0 = y0 = w0 = h0 = 0;
	lop = 0;
	source = 0;
	cdev = new XWPSDeviceMaskClip;
	pcdev = 0;
	tmask = 0;
	rop_source = 0;
	orig_dev = 0;
	xoff = yoff = 0;
}

XWPSTileFillState::~XWPSTileFillState()
{
}

int XWPSTileFillState::init(XWPSDeviceColor * pdevcA, XWPSDevice * dev, bool set_mask_phase)
{
	XWPSColorTile *m_tile = pdevcA->mask.m_tile;
  int px, py;

  pdevc = pdevcA;
  if (m_tile == 0) 
  {
		pcdev = dev;
		phase = pdevc->phase;
		return 0;
  }
  pcdev = cdev;
  tmask = &m_tile->tmask;
  phase.x = pdevc->mask.m_phase.x;
  phase.y = pdevc->mask.m_phase.y;
  
  if (set_mask_phase && m_tile->is_simple) 
  {
		px = imod(-(int)(m_tile->step_matrix.tx - phase.x + 0.5),  m_tile->tmask.rep_width);
		py = imod(-(int)(m_tile->step_matrix.ty - phase.y + 0.5),  m_tile->tmask.rep_height);
  } 
  else
		px = py = 0;
  return cdev->tileClipInitialize(tmask, dev, px, py);
}

int XWPSTileFillState::tileBySteps(int x0A, int y0A, int w0A, int h0A,
	      					XWPSColorTile * ptile,
	      					XWPSStripBitmap * tbits_or_tmask,
	      					int (XWPSTileFillState::*fill_proc)(int x, int y, int w, int h))
{
	int x1 = x0A + w0A, y1 = y0A + h0A;
  int i0, i1, j0, j1, i, j;
  XWPSMatrix step_matrix;
  int code;

  x0 = x0A, w0 = w0A;
  y0 = y0A, h0 = h0A;
  step_matrix = ptile->step_matrix;
  step_matrix.tx -= phase.x;
  step_matrix.ty -= phase.y;
  
  {
  	XWPSRect bbox;
		XWPSRect ibbox;
		double bbw = ptile->bbox.q.x - ptile->bbox.p.x;
		double bbh = ptile->bbox.q.y - ptile->bbox.p.y;
		double u0, v0, u1, v1;

		bbox.p.x = x0A, bbox.p.y = y0A;
		bbox.q.x = x1, bbox.q.y = y1;
		ibbox.bboxTransformInverse(&bbox, &step_matrix);
		
		u0 = ibbox.p.x - qMax(ptile->bbox.p.x, (double)0) - 0.000001;
		v0 = ibbox.p.y - qMax(ptile->bbox.p.y, (double)0) - 0.000001;
		u1 = ibbox.q.x - qMin(ptile->bbox.q.x, (double)0) + 0.000001;
		v1 = ibbox.q.y - qMin(ptile->bbox.q.y, (double)0) + 0.000001;
		if (!ptile->is_simple)
	    u0 -= bbw, v0 -= bbh, u1 += bbw, v1 += bbh;
		i0 = (int)floor(u0);
		j0 = (int)floor(v0);
		i1 = (int)ceil(u1);
		j1 = (int)ceil(v1);
  }
  
  for (i = i0; i < i1; i++)
		for (j = j0; j < j1; j++)
		{
			int x = (int)(step_matrix.xx * i + step_matrix.yx * j + step_matrix.tx);
	    int y = (int)(step_matrix.xy * i + step_matrix.yy * j + step_matrix.ty);
	    int w = tbits_or_tmask->size.x;
	    int h = tbits_or_tmask->size.y;
	    int xoffA, yoffA;

	    if (x < x0)
				xoffA = x0 - x, x = x0, w -= xoffA;
	    else
				xoffA = 0;
	    if (y < y0)
				yoffA = y0 - y, y = y0, h -= yoffA;
	    else
				yoffA = 0;
	    if (x + w > x1)
				w = x1 - x;
	    if (y + h > y1)
				h = y1 - y;
	    
	    if (w > 0 && h > 0) 
	    {
				if (pcdev == cdev)
		    	cdev->tileClipSetPhase(imod(xoffA - x, tmask->rep_width),	imod(yoffA - y, tmask->rep_height));
				xoff = xoffA;
				yoff = yoffA;
				code = (this->*fill_proc) (x, y, w, h);
				if (code < 0)
		    	return code;
	    }
		}
		
	return 0;
}

int XWPSTileFillState::tileColoredFill(int x, int y, int w, int h)
{
	XWPSColorTile *ptile = pdevc->colors.pattern.p_tile;

  XWPSDevice *dev = orig_dev;
  XWPSStripBitmap *bits = &ptile->tbits;
  uchar *data = bits->data;
  bool full_transfer = (w == w0 && h == h0);
  ulong source_id = (full_transfer ? rop_source->id : ps_no_bitmap_id);
  int code;

  if (source == NULL && lop_no_S_is_T(lop))
		code = pcdev->copyColor(data + bits->raster * yoff, xoff,
	     						bits->raster,  (full_transfer ? bits->id : ps_no_bitmap_id), x, y, w, h);
  else 
  {
		XWPSStripBitmap data_tile;

		data_tile.data = (uchar *) data;
		data_tile.raster = bits->raster;
		data_tile.size.x = data_tile.rep_width = ptile->tbits.size.x;
		data_tile.size.y = data_tile.rep_height = ptile->tbits.size.y;
		data_tile.id = bits->id;
		data_tile.shift = data_tile.rep_shift = 0;
		code = dev->stripCopyRop(rop_source->sdata + (y - y0) * rop_source->sraster,
	     												rop_source->sourcex + (x - x0), rop_source->sraster, source_id,
	     											(rop_source->use_scolors ? rop_source->scolors : NULL), &data_tile, NULL,
	     												x, y, w, h, imod(xoff - x, data_tile.rep_width),  imod(yoff - y, data_tile.rep_height), lop);
  }
  return code;
}

int XWPSTileFillState::tileMaskedFill(int x, int y, int w, int h)
{
	if (source == NULL)
		return (pdevc->*fill_rectangle)(x, y, w, h, pcdev, lop, NULL);
  else 
  {
		PSRopSource step_source;

		step_source.sdata = source->sdata + (y - y0) * source->sraster;
		step_source.sourcex = source->sourcex + (x - x0);
		step_source.sraster = source->sraster;
		step_source.id = (w == w0 && h == h0 ? source->id : ps_no_bitmap_id);
		step_source.scolors[0] = source->scolors[0];
		step_source.scolors[1] = source->scolors[1];
		step_source.use_scolors = source->use_scolors;
		return (pdevc->*fill_rectangle)(x, y, w, h, pcdev, lop, &step_source);
  }
}

PSDeviceColorType ps_dc_type_data_null =
{
	&XWPSDeviceColor::nullLoad,
	&XWPSDeviceColor::nullFillRectangle,
	&XWPSDeviceColor::nullFillMasked,
	&XWPSDeviceColor::nullEqual
};

PSDeviceColorType ps_dc_type_data_none={
	&XWPSDeviceColor::noLoad,
	&XWPSDeviceColor::noFillRectangle,
	&XWPSDeviceColor::noFillMasked,
	&XWPSDeviceColor::noEqual
};

PSDeviceColorType ps_dc_type_data_pure={
	&XWPSDeviceColor::pureLoadDC,
	&XWPSDeviceColor::pureFillRectangle,
	&XWPSDeviceColor::pureFillMasked,
	&XWPSDeviceColor::pureEqualDC
};

PSDeviceColorType ps_dc_type_ht_binary={
	&XWPSDeviceColor::binaryLoadHT,
	&XWPSDeviceColor::binaryFillRectangleHT,
	&XWPSDeviceColor::defaultFillMaskedDC,
	&XWPSDeviceColor::binaryEqualHT
};

PSDeviceColorType ps_dc_type_ht_colored={
	&XWPSDeviceColor::coloredLoadHT,
	&XWPSDeviceColor::coloredFillRectangleHT,
	&XWPSDeviceColor::defaultFillMaskedDC,
	&XWPSDeviceColor::coloredEqualHT
};

PSDeviceColorType ps_dc_type_pattern={
	&XWPSDeviceColor::patternLoadDC,
	&XWPSDeviceColor::patternFillRectangleDC,
	&XWPSDeviceColor::defaultFillMaskedDC,
	&XWPSDeviceColor::patternEqualDC
};

PSDeviceColorType ps_dc_pure_masked = {
	&XWPSDeviceColor::pureMaskedLoadDC, 
	&XWPSDeviceColor::pureMaskedFillRectDC,
  &XWPSDeviceColor::defaultFillMaskedDC,
  &XWPSDeviceColor::pureMaskedEqualDC
};

PSDeviceColorType ps_dc_binary_masked = {
    &XWPSDeviceColor::maskedLoadBinaryDC, 
    &XWPSDeviceColor::binaryMaskedFillRect,
    &XWPSDeviceColor::defaultFillMaskedDC,
    &XWPSDeviceColor::binaryMaskedEqual
};

PSDeviceColorType ps_dc_colored_masked = {
    &XWPSDeviceColor::coloredMaskedLoad, 
    &XWPSDeviceColor::coloredMaskedFillRect,
    &XWPSDeviceColor::defaultFillMaskedDC,
    &XWPSDeviceColor::coloredMaskedEqual
};

PSDeviceColorType ps_dc_pattern2 = {
  &XWPSDeviceColor::pattern2LoadDC, 
  &XWPSDeviceColor::pattern2FillRectangleDC,
  &XWPSDeviceColor::defaultFillMaskedDC,
  &XWPSDeviceColor::pattern2EqualDC
};

XWPSDeviceColor::XWPSDeviceColor(ulong p)
{
	type = 0;
	colors.pure = p;
	ht_no_bitmap.init(bytes);
}

XWPSDeviceColor::~XWPSDeviceColor()
{
}

XWPSStripBitmap * XWPSDeviceColor::binaryTile()
{
	return (&(colors.binary.b_tile->tiles));
}

void XWPSDeviceColor::completeCMYKHalftone(XWPSDeviceHalftone *pdht)
{
	type = &ps_dc_type_ht_colored;
  colors.colored.c_ht = pdht;
  colors.colored.alpha = max_ushort;
  colors.colored.plane_mask =	(colors.colored.c_level[0] != 0) |
															((colors.colored.c_level[1] != 0) << 1) |
															((colors.colored.c_level[2] != 0) << 2) |
															((colors.colored.c_level[3] != 0) << 3);
}

void XWPSDeviceColor::completeRGBHalftone(XWPSDeviceHalftone *pdht)
{
	type = &ps_dc_type_ht_colored;
  colors.colored.c_ht = pdht;
  colors.colored.plane_mask =	(colors.colored.c_level[0] != 0) |
															((colors.colored.c_level[1] != 0) << 1) |
															((colors.colored.c_level[2] != 0) << 2);
  colors.colored.c_base[3] = 0;
  colors.colored.c_level[3] = 0;
}

int  XWPSDeviceColor::defaultFillMaskedDC(const uchar * data,
		                                   int data_x, int raster, 
		                                   ulong ,
		                                  int x, 
		                                  int y, 
		                                  int w, 
		                                  int h, 
		                                  XWPSDevice *dev,
		                                  uint lop, 
		                                  bool invert)
{
	int lbit = data_x & 7;
  const uchar *row = data + (data_x >> 3);
  uint one = (invert ? 0 : 0xff);
  uint zero = one ^ 0xff;
  int iy;

  for (iy = 0; iy < h; ++iy, row += raster) 
  {
		const uchar *p = row;
		int bit = lbit;
		int left = w;
		int l0;

		while (left) 
		{
	    int run, code;
	    run = byte_bit_run_length[bit][*p ^ one];
	    if (run) 
	    {
				if (run < 8) 
				{
		    	if (run >= left)
						break;
		    	bit += run, left -= run;
				} 
				else if ((run -= 8) >= left)
		    	break;
				else 
				{
		    	left -= run;
		    	++p;
		    	while (left > 8 && *p == zero)
						left -= 8, ++p;
		    	run = byte_bit_run_length_0[*p ^ one];
		    	if (run >= left)
						break;
		    	else
						bit = run & 7, left -= run;
				}
	    }
	    l0 = left;
	    run = byte_bit_run_length[bit][*p ^ zero];
	    if (run < 8) 
	    {
				if (run >= left)
		    	left = 0;
				else
		    	bit += run, left -= run;
	    } 
	    else if ((run -= 8) >= left)
				left = 0;
	    else 
	    {
				left -= run;
				++p;
				while (left > 8 && *p == one)
		    	left -= 8, ++p;
				run = byte_bit_run_length_0[*p ^ zero];
				if (run >= left)
		    	left = 0;
				else
		    	bit = run & 7, left -= run;
	    }
	    code = fillRectangle( x + w - l0, y + iy, l0 - left, 1, dev, lop, NULL);
	    if (code < 0)
				return code;
		}
  }
  return 0;
}

bool XWPSDeviceColor::equal(XWPSDeviceColor *pdevc2)
{
	return (this->*(type->equal))(pdevc2);
}

int XWPSDeviceColor::fillMasked(const uchar *data, 
	              int data_x,
                 int raster, 
                 ulong id, 
                 int x, 
                 int y, 
                 int w, 
                 int h,
    							XWPSDevice *dev, 
    							ulong lop, 
    							bool invert)
{
	return (this->*(type->fill_masked))(data, data_x, raster,id,x,y,w,h,dev,lop,invert);
}

int XWPSDeviceColor::fillRectangle(int x, 
		                               int y,
			                             int w, 
			                             int h, 
			                             XWPSDevice *dev,
										               uint lop,
										               const PSRopSource *source)
{
	return (this->*(type->fill_rectangle))(x, y, w, h, dev, lop, source);
}

bool XWPSDeviceColor::isBinaryHalftone()
{
	return type == &ps_dc_type_ht_binary;
}

bool XWPSDeviceColor::isNull()
{
	return type == &ps_dc_type_data_null;
}

bool XWPSDeviceColor::isPure()
{
	return type == &ps_dc_type_data_pure;
}

bool XWPSDeviceColor::isSet()
{
	return type != &ps_dc_type_data_none;
}

int XWPSDeviceColor::load(XWPSImagerState * pis, XWPSDevice * dev)
{
	return loadSelect(pis, dev, ps_color_select_texture);
}

int XWPSDeviceColor::loadPattern(XWPSImagerState * pis,
	      			   XWPSDevice * dev, 
	      			   PSColorSelect select)
{
	XWPSDevicePatternAccum *adev;
  XWPSPattern1Instance *pinst =	(XWPSPattern1Instance *)ccolor.pattern;
  XWPSState *saved;
  XWPSColorTile *ctile;
  int code;

  if (patternCacheLookup(pis, dev, select))
		return 0;
		
  code = pis->ensurePatternCache();
  if (code < 0)
		return code;
		
  adev = new XWPSDevicePatternAccum;
  adev->context_state = dev->context_state;
  adev->setTarget(dev);
  adev->instance = pinst;
  code = adev->open();
  if (code < 0)
		goto fail;
    
  saved = new XWPSState(pinst->saved);
  saved->i_ctx_p = dev->context_state;
  
  if (saved->pattern_cache == 0)
		saved->pattern_cache = pis->pattern_cache;
  saved->setDeviceNoInit(adev);
  code = (ccolor.*pinst->templat.PaintProc)(saved);
  if (code < 0) 
  {
		adev->close();
		delete saved;
		if (!adev->decRef())
   		delete adev;
		return code;
  }
  
  code = pis->patternCacheAddEntry(adev, &ctile);
  if (code >= 0) 
  {
		if (!patternCacheLookup(pis, dev, select)) 
	    code = (int)(XWPSError::Fatal);
  }
  
  adev->close();
  delete saved;
  if (!adev->decRef())
   	delete adev;
  return code;
  
fail:
	if (adev)
	{
		if (!adev->decRef())
   		delete adev;
 	}
   return code;
}

int XWPSDeviceColor::loadSelect(XWPSImagerState * pis,
	      			                  XWPSDevice * dev, 
	      			                  PSColorSelect select)
{
	return (this->*(type->load))(pis, dev, select);
}

bool  XWPSDeviceColor::patternCacheLookup(XWPSImagerState * pis,
												   XWPSDevice * dev, 
												   PSColorSelect select)
{
	XWPSPatternCache *pcache = pis->pattern_cache;
  ulong id = mask.id;

  if (id == ps_no_bitmap_id) 
  {
		setNullPattern();
		return true;
  }
  if (pcache != 0) 
  {
		XWPSColorTile *ctile = &pcache->tiles[id % pcache->num_tiles];

		if (ctile->id == id &&
	    	(type != &ps_dc_type_pattern ||
	     	ctile->depth == dev->color_info.depth)) 
	  {
	    int px = pis->screen_phase[select].x;
	    int py = pis->screen_phase[select].y;

	    if (type == &ps_dc_type_pattern) 
	    {
				colors.pattern.p_tile = ctile;
				setPhaseMod(px, py, ctile->tbits.rep_width, ctile->tbits.rep_height);
	    }
	    mask.m_tile =	(ctile->tmask.data == 0 ? 0 : ctile);
	    mask.m_phase.x = -px;
	    mask.m_phase.y = -py;
	    return true;
		}
  }
  return false;
}

int XWPSDeviceColor::reduceColoredHalftone(XWPSDevice *dev,  bool cmyk)
{
	int planes = colors.colored.plane_mask;
  ushort max_color = dev->color_info.dither_colors - 1;
  uint b[4];
  ushort v[4];
  ulong c0, c1;

  b[0] = colors.colored.c_base[0];
  v[0] = fractional_color(b[0], max_color);
  b[1] = colors.colored.c_base[1];
  v[1] = fractional_color(b[1], max_color);
  b[2] = colors.colored.c_base[2];
  v[2] = fractional_color(b[2], max_color);
  if (cmyk) 
  {
		b[3] = colors.colored.c_base[3];
		v[3] = fractional_color(b[3], max_color);
		c0 = dev->mapCMYKColor(v[0], v[1], v[2], v[3]);
  } 
  else
		c0 = dev->mapRGBColor(v[0], v[1], v[2]);
  if (planes == 0) 
  {
		setPure(c0);
		return 0;
  } 
  else 
  {
		int i = (planes >> 1) - (planes >> 3);
		uint bi = b[i] + 1;
		XWPSDeviceHalftone *pdht = colors.colored.c_ht;
		int index = (pdht->components == 0 ? -1 : pdht->color_indices[i]);
		bool invert = dev->color_info.num_components == 4;
		uint level = colors.colored.c_level[i];

		v[i] = fractional_color(bi, max_color);
		c1 = (cmyk ? dev->mapCMYKColor(v[0], v[1], v[2], v[3]) :  dev->mapRGBColor(v[0], v[1], v[2]));
		if (invert) 
		{
	    level =	(index < 0 ? &pdht->order : &pdht->components[index].corder)->num_levels - level;
	    setBinaryHalftoneComponent(pdht, index, c1, c0, level);
		} 
		else
	    setBinaryHalftoneComponent(pdht, index, c0, c1, level);
					    
		return 1;
  }
}

int XWPSDeviceColor::renderCMYK(short c, 
	                short m, 
	                short y, 
	                short k, 
	                XWPSImagerState * pis, 
	                XWPSDevice * dev, 
	                PSColorSelect select)
{
	return renderColor(c, m, y, k, true, pis, dev, select);
}

int XWPSDeviceColor::renderColor(short r, 
	                short g, 
	                short b, 
	                short w, 
	                bool  cmyk, 
	                XWPSImagerState * pis, 
	                XWPSDevice * dev, 
	                PSColorSelect select)
{
	return renderColorAlpha(r, g, b, w, pis->alpha, cmyk, pis, dev, select);
}

int XWPSDeviceColor::renderColorAlpha(short r, 
	                     short g, 
	                     short b, 
	                     short w, 
	                     ushort a, 
	                     bool cmyk, 
	                     XWPSImagerState * pis, 
	                     XWPSDevice * dev, 
	                     PSColorSelect select)
{
	return renderDeviceColor(r, g, b, w, cmyk, a, dev,pis->dev_ht, &pis->screen_phase[select]);
}

int XWPSDeviceColor::renderDeviceColor(short red, 
	                                     short green, 
	                                     short blue, 
	                                     short white,
			                                 bool cmyk, 
			                                 ushort alpha,
			       					                 XWPSDevice * dev,
			                                 XWPSDeviceHalftone * pdht,
			                                 XWPSIntPoint * ht_phase)
{
	uint max_value = dev->color_info.dither_colors - 1;
  uint num_levels = pdht->order.num_levels;
  short rem_r, rem_g, rem_b, rem_w;
  uint r, g, b, w;
  ushort vr, vg, vb, vw;
  
#define MAP_COLOR_RGB()\
  (alpha == ps_max_color_value ?\
   dev->mapRGBColor(vr, vg, vb) :\
   dev->mapRGBAlphaColor(vr, vg, vb, alpha))
#define MAP_COLOR_CMYK()\
  dev->mapCMYKColor(vr, vg, vb, vw)
#define MAP_COLOR()\
  (cmyk ? MAP_COLOR_CMYK() : MAP_COLOR_RGB())
  	
  switch (max_value)
  {
  	case 1:
	    if (red == frac_1)
				rem_r = 0, r = 1;
	    else
				rem_r = red, r = 0;
	    if (green == frac_1)
				rem_g = 0, g = 1;
	    else
				rem_g = green, g = 0;
	    if (blue == frac_1)
				rem_b = 0, b = 1;
	    else
				rem_b = blue, b = 0;
	    if (white == frac_1)
				rem_w = 0, w = 1;
	    else
				rem_w = white, w = 0;
	    break;
	    
		default:
	    {
				ulong want_r, want_g, want_b, want_w;

				want_r = (ulong) max_value * red;
				r = frac_1_quo(want_r);
				rem_r = frac_1_rem(want_r, r);

				want_g = (ulong) max_value * green;
				g = frac_1_quo(want_g);
				rem_g = frac_1_rem(want_g, g);

				want_b = (ulong) max_value * blue;
				b = frac_1_quo(want_b);
				rem_b = frac_1_rem(want_b, b);

				want_w = (ulong) max_value * white;
				w = frac_1_quo(want_w);
				rem_w = frac_1_rem(want_w, w);
	    }
  }
  
  if (!(rem_r | rem_g | rem_b | rem_w)) 
  {
		vr = fractional_color(r, max_value);
		vg = fractional_color(g, max_value);
		vb = fractional_color(b, max_value);
		vw = fractional_color(w, max_value);
		setPure(MAP_COLOR());
		return 0;
  }
  
  if (dev->color_info.num_components >= 4)
  {
#define RGB_REM(rem_v, i)\
  (rem_v * (ulong)(pdht->components ? pdht->components[pdht->color_indices[i]].corder.num_levels : num_levels) / frac_1)
  	
		uint lr = RGB_REM(rem_r, 0), lg = RGB_REM(rem_g, 1),   lb = RGB_REM(rem_b, 2);	
		if (cmyk)
	    setCMYKHalftone(pdht, r, lr, g, lg, b, lb,    w, RGB_REM(rem_w, 3));
		else
	    setRGBHalftone(pdht, r, lr, g, lg, b, lb, alpha);
#undef RGB_REM

		setPhaseMod(ht_phase->x, ht_phase->y,  pdht->lcm_width, pdht->lcm_height);
		if (!(colors.colored.plane_mask &  (colors.colored.plane_mask - 1)))
	    return reduceColoredHalftone(dev, cmyk);
		return 1;
	}
	
	ulong hsize = num_levels;
	int adjust_r, adjust_b, adjust_g, adjust_w;
	ulong color1;
	short amax, amin;
	ulong fmax, cmax;
	int axisc, facec, cubec, diagc;
	ushort lum_invert;
	ulong dot1, dot2, dot3, dot4;
	int level;
	int code;
	
	lum_invert = 0;
#define half (frac_1/2)
	if (rem_r > half)
	    rem_r = frac_1 - rem_r,	adjust_r = -1, r++, lum_invert += lum_red_weight * 2;
	else
	    adjust_r = 1;
	if (rem_g > half)
	    rem_g = frac_1 - rem_g,	adjust_g = -1, g++, lum_invert += lum_green_weight * 2;
	else
	    adjust_g = 1;
	if (rem_b > half)
	    rem_b = frac_1 - rem_b,	adjust_b = -1, b++, lum_invert += lum_blue_weight * 2;
	else
	    adjust_b = 1;
	vr = fractional_color(r, max_value);
	vg = fractional_color(g, max_value);
	vb = fractional_color(b, max_value);
	if (cmyk) 
	{
	  if (rem_w > half)
			rem_w = frac_1 - rem_w, adjust_w = -1, w++, lum_invert += lum_white_weight * 2;
	  else
			adjust_w = 1;
	  vw = fractional_color(w, max_value);
	  color1 = MAP_COLOR_CMYK();
	} 
	else
	  color1 = MAP_COLOR_RGB();
	  
	cmax = (ulong) rem_r + rem_g + rem_b;
	dot4 = cmax + rem_w;
	if (rem_g > rem_r) 
	{
	  if (rem_b > rem_g)
			amax = rem_b, axisc = DIAG_B;
	  else
			amax = rem_g, axisc = DIAG_G;
	  if (rem_b > rem_r)
			amin = rem_r, fmax = (ulong) rem_g + rem_b, facec = DIAG_GB;
	  else
			amin = rem_b, fmax = (ulong) rem_r + rem_g, facec = DIAG_RG;
	} 
	else 
	{
	  if (rem_b > rem_r)
			amax = rem_b, axisc = DIAG_B;
	  else
			amax = rem_r, axisc = DIAG_R;
	  if (rem_b > rem_g)
			amin = rem_g, fmax = (ulong) rem_b + rem_r, facec = DIAG_BR;
	  else
			amin = rem_b, fmax = (ulong) rem_r + rem_g, facec = DIAG_RG;
	}
	if (rem_w > amin) 
	{
	  cmax = fmax + rem_w, cubec = facec + DIAG_W;
	  if (rem_w > amax)
			fmax = (ulong) amax + rem_w, facec = axisc + DIAG_W, amax = rem_w, axisc = DIAG_W;
	  else if (rem_w > fmax - amax)
			fmax = (ulong) amax + rem_w, facec = axisc + DIAG_W;
	} 
	else
	  cubec = DIAG_RGB;

	dot1 = amax * WEIGHT1;
	dot2 = fmax * WEIGHT2;
	dot3 = cmax * WEIGHT3;
	
#define use_axis()\
  diagc = axisc, level = (hsize * amax + (frac_1_long / 2)) / frac_1_long
#define use_face()\
  diagc = facec, level = (hsize * fmax + frac_1_long) / (2 * frac_1_long)
#define use_cube()\
  diagc = cubec, level = (hsize * cmax + (3 * frac_1_long / 2)) / (3 * frac_1_long)
#define use_tesseract()\
  diagc = DIAG_RGBW, level = (hsize * dot4 + (2 * frac_1_long)) / (4 * frac_1_long)
  
	if (dot1 > dot2) 
	{
	  if (dot3 > dot1) 
	  {
			if (dot4 * WEIGHT4 > dot3)
		    use_tesseract();
			else
		    use_cube();
	  } 
	  else 
	  {
			if (dot4 * WEIGHT4 > dot1)
		  	use_tesseract();
			else
		  	use_axis();
	  }
	} 
	else 
	{
	  if (dot3 > dot2) 
	  {
			if (dot4 * WEIGHT4 > dot3)
		    use_tesseract();
			else
		    use_cube();
	  } 
	  else 
	  {
			if (dot4 * WEIGHT4 > dot2)
		    use_tesseract();
			else
		    use_face();
	  }
	}
	if (level == 0) 
	{
	  setPure(color1);
	  code = 0;
	} 
	else 
	{
		ulong color2;
		if (diagc & DIAG_R)
			r += adjust_r;
	  if (diagc & DIAG_G)
			g += adjust_g;
	  if (diagc & DIAG_B)
			b += adjust_b;
			
		vr = fractional_color(r, max_value);
	  vg = fractional_color(g, max_value);
	  vb = fractional_color(b, max_value);
	  if (cmyk) 
	  {
			if (diagc & DIAG_W)
		    w += adjust_w;
			vw = fractional_color(w, max_value);
			color2 = MAP_COLOR_CMYK();
	  } 
	  else
			color2 = MAP_COLOR_RGB();
	  if (level == num_levels) 
	  {
			setPure(color2);
			code = 0;
	  } 
	  else 
	  {
			if (lum_w[diagc] < lum_invert)
		    setBinaryHalftone(pdht, color2, color1, hsize - level);
			else
		    setBinaryHalftone(pdht, color1, color2, level);
			setPhaseMod(ht_phase->x, ht_phase->y,  pdht->order.width, pdht->order.full_height);
			code = 1;
	  }
	}
	return code;
}

int XWPSDeviceColor::renderDeviceGray(short gray, 
	                                    ushort alpha,
			     						                XWPSDevice * dev,
			                                XWPSDeviceHalftone * pdht,
			                                XWPSIntPoint * ht_phase)
{
	bool cmyk = dev->color_info.num_components == 4;
	if (alpha == ps_max_color_value)
	{
		ushort lum;
		switch (gray) 
		{
	    case frac_0:
				lum = 0;
				goto bw;
				
	    case frac_1:
				lum = ps_max_color_value;
			
bw:
				setPure((cmyk ?	dev->mapCMYKColor(0, 0, 0, ps_max_color_value - lum) : dev->mapRGBColor(lum, lum, lum)));
				return 0;
				
	    	default:
	    		break;
		;
		}
	}
	
	uint max_value = dev->color_info.dither_grays - 1;
	ulong hsize = (unsigned)pdht->order.num_levels;
	ulong nshades = hsize * max_value + 1;
	ulong lx = (nshades * gray) / (frac_1_long + 1);
	uint v = lx / hsize;
	ushort lum = fractional_color(v, max_value);
	ulong color1;
	int level = lx % hsize;
	if ( cmyk )
    color1 = dev->mapCMYKColor(0, 0, 0, ps_max_color_value - lum);
  else if ( alpha == ps_max_color_value )
    color1 = dev->mapRGBColor(lum, lum, lum);
  else
    color1 = dev->mapRGBAlphaColor(lum, lum, lum, alpha);
    
  if (level == 0)
  {
  	setPure(color1);
	  return 0;
  }
  else
  {
  	ulong color2;
	  v++;
	  lum = fractional_color(v, max_value);
	  if ( cmyk )
    	color2 = dev->mapCMYKColor(0, 0, 0, ps_max_color_value - lum);
  	else if ( alpha == ps_max_color_value )
    	color2 = dev->mapRGBColor(lum, lum, lum);
  	else
    	color2 = dev->mapRGBAlphaColor(lum, lum, lum, alpha);
	  setBinaryHalftone(pdht, color1, color2, level);
	  setPhaseMod(ht_phase->x, ht_phase->y,pdht->order.width,pdht->order.full_height);
	  return 1;
  }
}

int XWPSDeviceColor::renderGray(short gray, 
	                              XWPSImagerState * pis, 
	                              XWPSDevice * dev, 
	                              PSColorSelect select)
{
	return renderGrayAlpha(gray, pis->alpha, pis, dev, select);
}

int XWPSDeviceColor::renderGrayAlpha(short gray, 
	                                   ushort alpha, 
	                                   XWPSImagerState * pis, 
	                                   XWPSDevice * dev, 
	                                   PSColorSelect select)
{
	return renderDeviceGray(gray, alpha, dev, pis->dev_ht,	&pis->screen_phase[select]);
}

int XWPSDeviceColor::renderRGB(short r, 
	              short g,
	              short b, 
	              XWPSImagerState * pis, 
	              XWPSDevice * dev, 
	              PSColorSelect select)
{
	return renderColor(r, g, b, frac_0, false, pis, dev, select);
}

int XWPSDeviceColor::renderRGBAlpha(short r, 
	                   short g, 
	                   short b, 
	                   ushort a, 
	                   XWPSImagerState * pis, 
	                   XWPSDevice * dev, 
	                   PSColorSelect select)
{
	return renderColorAlpha(r, g, b, frac_0, a, false, pis, dev, select);
}

void XWPSDeviceColor::setBinaryHalftone(XWPSDeviceHalftone * ht,
	                                      ulong color0, 
	                                      ulong color1,
	                                      uint level)
{
	setBinaryHalftoneComponent(ht, -1, color0, color1, level);
}

void XWPSDeviceColor::setBinaryHalftoneComponent(XWPSDeviceHalftone * ht, 
	                                               int index, 
	                                               ulong color0, 
	                                               ulong color1,
	                                               uint level)
{
	colors.binary.b_ht = ht;
  colors.binary.b_index = index;
  colors.binary.color[0] = color0;
  colors.binary.color[1] = color1;
  colors.binary.b_level = level;
  type = &ps_dc_type_ht_binary;
}

void XWPSDeviceColor::setCMYKHalftone(XWPSDeviceHalftone * ht, 
	                     uchar bc, 
	                     uint lc, 
	                     uchar bm, 
	                     uint lm, 
	                     uchar by, 
	                     uint ly, 
	                     uchar bk, 
	                     uint lk)
{
	setC(0, bc, lc);
  setC(1, bm, lm);
  setC(2, by, ly);
  setC(3, bk, lk);
  completeCMYKHalftone(ht);
}

int  XWPSDeviceColor::setCMYK1BitColors(PSColorValuesPair *,
		     ulong * colorsA,
		     XWPSStripBitmap ** sbits,
		     XWPSDevice * ,
		     XWPSHTCache ** caches,
		     int )
{
	XWPSDeviceHalftone *pdht = colors.colored.c_ht;
	
	quint32 mask0 = 0, mask1 = 0;
#define SET_PLANE_COLOR_CMYK(i, mask)\
    do{uint r = colors.colored.c_level[i];\
\
    if (r == 0) {\
	if (colors.colored.c_base[i])\
	    mask0 |= mask, mask1 |= mask;\
	sbits[3 - i] = &ht_no_bitmap;\
    } else {\
	int nlevels =\
	    (pdht->components ?\
	     pdht->components[pdht->color_indices[i]].corder.num_levels :\
	     pdht->order.num_levels);\
\
	mask0 |= mask;\
	sbits[3 - i] =  &(caches[i]->render(nlevels - r)->tiles);\
  }}while(0)
    
	
  SET_PLANE_COLOR_CMYK(0, (quint32)~0x77777777);
  SET_PLANE_COLOR_CMYK(1, 0x44444444);
  SET_PLANE_COLOR_CMYK(2, 0x22222222);
  SET_PLANE_COLOR_CMYK(3, 0x11111111);
#undef SET_PLANE_COLOR_CMYK
  
  {
		XWPSHTCache *ctemp;

		ctemp = caches[0], caches[0] = caches[3], caches[3] = ctemp;
		ctemp = caches[1], caches[1] = caches[2], caches[2] = ctemp;
  }
  colorsA[0] = mask0;
  colorsA[1] = mask1;
  return 1;
}

int  XWPSDeviceColor::setHTColorsGT4(PSColorValuesPair *pvp,
		   								ulong * colorsA,
		   								XWPSStripBitmap ** sbits,
		   								XWPSDevice * dev,
		                  XWPSHTCache ** caches, 
		                  int nplanes)
{
	ushort max_color = dev->color_info.dither_colors - 1;
	bool invert = true;
  ulong plane_mask = colors.colored.plane_mask;
  int i;
  ulong ci;

  for (i = 0; i < nplanes; ++i)
		if ((plane_mask >> i) & 1)
		{
	    SET_PLANE_COLOR(i);
	  }
    else
    {
	    SET_PLANE_COLOR_CONSTANT(i);
	  }
  ci = 0;
  do 
  {
		colorsA[ci] = ps_no_color_index;
  } while ((ci = ((ci | ~plane_mask) + 1) & plane_mask) != 0);

  return 0;
}

int  XWPSDeviceColor::setHTColorsLE4(PSColorValuesPair *pvp,
		   								ulong * colorsA,
		   								XWPSStripBitmap ** sbits,
		   								XWPSDevice * dev,
		                  XWPSHTCache ** caches, 
		                  int nplanes)
{
	short max_color = dev->color_info.dither_colors - 1;
	bool invert = dev->color_info.num_components >= 4;
	
	SET_PLANE_COLOR(0);
  SET_PLANE_COLOR(1);
  SET_PLANE_COLOR(2);
  if (nplanes == 3) 
  {
		ushort alpha = colors.colored.alpha;

		if (alpha == ps_max_color_value) 
		{
#define M(i)\
  		colorsA[i] = dev->mapRGBColor(pvp->values[(i) & 1][0],\
			       pvp->values[((i) & 2) >> 1][1],\
			       pvp->values[(i) >> 2][2])
	    M(0); M(1); M(2); M(3); M(4); M(5); M(6); M(7);
#undef M
		} 
		else 
		{
#define M(i)\
  	colorsA[i] = dev->mapRGBAlphaColor(pvp->values[(i) & 1][0],\
				     pvp->values[((i) & 2) >> 1][1],\
				     pvp->values[(i) >> 2][2], alpha)
	    M(0); M(1); M(2); M(3); M(4); M(5); M(6); M(7);
#undef M
		}
  } 
  else 
  {
		SET_PLANE_COLOR(3);
		if (nplanes > 4) 
		{
	    int pi;

	    for (pi = 4; pi < nplanes; ++pi)
				SET_PLANE_COLOR_CONSTANT(pi);
		}

#define M(i)\
  colorsA[i] = dev->mapCMYKColor(pvp->values[(i) & 1][0],\
				pvp->values[((i) & 2) >> 1][1],\
				pvp->values[((i) & 4) >> 2][2],\
				pvp->values[(i) >> 3][3])

		switch ((int)colors.colored.plane_mask) 
		{
	    case 15:
				M(15); M(14); M(13); M(12);
				M(11); M(10); M(9); M(8);
				
	    case 7:
				M(7); M(6); M(5); M(4);
c3:	    
			case 3:
				M(3); M(2);
c1:	    
			case 1:
				M(1);
				break;
				
	    case 14:
				M(14); M(12); M(10); M(8);
				
	    case 6:
				M(6); M(4);
c2:	    
			case 2:
				M(2);
				break;
				
	    case 13:
				M(13); M(12); M(9); M(8);
				
	    case 5:
				M(5); M(4);
				goto c1;
				
	    case 12:
				M(12); M(8);
				
	    case 4:
				M(4);
				break;
				
	    case 11:
				M(11); M(10); M(9); M(8);
				goto c3;
				
	    case 10:
				M(10); M(8);
				goto c2;
				
	    case 9:
				M(9); M(8);
				goto c1;
				
	    case 8:
				M(8);
				break;
				
	    case 0:;
		}
		M(0);

#undef M

  }
  return 0;
}

void XWPSDeviceColor::setNull()
{
	type = &ps_dc_type_data_null;
}

void XWPSDeviceColor::setNullPattern()
{
	mask.id = ps_no_bitmap_id;
	mask.m_tile = 0;
	colors.pattern.p_tile = 0;
	type = &ps_dc_type_pattern;
}

void XWPSDeviceColor::setRGBHalftone(XWPSDeviceHalftone * ht, 
	                    uchar br, 
	                    uint lr, 
	                    uchar bg, 
	                    uint lg, 
	                    uchar bb, 
	                    uint lb, 
	                    ushort a)
{
	setC(0, br, lr);
  setC(1, bg, lg);
  setC(2, bb, lb);
  colors.colored.alpha = a;
  completeRGBHalftone(ht);
}

void XWPSDeviceColor::setPhase(int px, int py)
{
	phase.x = px;
	phase.y = py;
}

void XWPSDeviceColor::setPhaseMod(int px, int py, int tw, int th)
{
	setPhase(imod(-(px), tw), imod(-(py), th));
}

void XWPSDeviceColor::setPure(ulong color)
{
	colors.pure = color;
	type = &ps_dc_type_data_pure;
}

void XWPSDeviceColor::unset()
{
	type = &ps_dc_type_data_none;
}

bool XWPSDeviceColor::writePure(ulong lop)
{
	return (isPure() && lop_no_S_is_T(lop));
}

bool XWPSDeviceColor::binaryEqualHT(XWPSDeviceColor *pdevc2)
{
	return pdevc2->type == type &&
		phase.x == pdevc2->phase.x &&
		phase.y == pdevc2->phase.y &&
		colors.binary.color[0] == pdevc2->colors.binary.color[0] &&
	colors.binary.color[1] == pdevc2->colors.binary.color[1] &&
	colors.binary.b_level == pdevc2->colors.binary.b_level;
}

int XWPSDeviceColor::binaryFillRectangleHT(int x, 
		                    int y,
			                  int w, 
			                  int h, 
			                  XWPSDevice *dev,
												uint lop,
												const PSRopSource *source)
{
	PSRopSource no_source;
	if (dev->color_info.depth > 1)
		lop &= ~lop_T_transparent;
  if (source == NULL && lop_no_S_is_T(lop))
		return dev->stripTileRectangle(&colors.binary.b_tile->tiles,
				  					x, y, w, h, colors.binary.color[0],
					      colors.binary.color[1],  phase.x, phase.y);
					      
  if (colors.binary.color[0] == ps_no_color_index)
		lop = rop3_use_D_when_T_0(lop);
    
  if (colors.binary.color[1] == ps_no_color_index)
		lop = rop3_use_D_when_T_1(lop);
  if (source == NULL)
		setRopNoSource(&source, &no_source, dev);
  return dev->stripCopyRop(source->sdata,
			       source->sourcex, source->sraster, source->id,
			     (source->use_scolors ? source->scolors : NULL),
					&colors.binary.b_tile->tiles,
					     colors.binary.color,
				 x, y, w, h, phase.x, phase.y, lop);
}

int XWPSDeviceColor::binaryLoadHT(XWPSImagerState *pis,
	      			     XWPSDevice *, 
	      			     PSColorSelect )
{
	int component_index = colors.binary.b_index;
  XWPSHTOrder *porder =	(component_index < 0 ?	 &colors.binary.b_ht->order :
	 		&colors.binary.b_ht->components[component_index].corder);
  XWPSHTCache *pcache =	(porder->cache == 0 ? pis->ht_cache : porder->cache);

  if (pcache->order.bit_data->arr != porder->bit_data->arr)
		pcache->init(porder);
		
  {
		int b_level = colors.binary.b_level;
		int level = ((porder->levels->ptr)[b_level]);
		XWPSHTTile *bt = &pcache->ht_tiles[level / pcache->levels_per_tile];

		if (bt->level != level) 
		{
	    int code = pcache->renderHt(bt, level, porder, pcache->base_id + b_level);

	    if (code < 0)
				return (int)(XWPSError::Fatal);
		}
		colors.binary.b_tile = bt;
  }
  return 0;
}

bool XWPSDeviceColor::binaryMaskedEqual(XWPSDeviceColor * pdevc2)
{
	return (this->*(ps_dc_type_ht_binary.equal))(pdevc2) && mask.id == pdevc2->mask.id;
}

int XWPSDeviceColor::binaryMaskedFillRect(int x, 
	                         int y, 
	                         int w, 
	                         int h, 
	                         XWPSDevice * dev,
			                     uint lop,
			                     const PSRopSource * source)
{
	XWPSColorTile *ptile = mask.m_tile;
  XWPSTileFillState state;
  int code;

  code = state.init(this, dev, true);
  if (code < 0)
		return code;
  if (state.pcdev == dev || ptile->is_simple)
		return binaryFillRectangleHT(x, y, w, h, state.pcdev, lop, source);
  else 
  {
		state.lop = lop;
		state.source = source;
		state.fill_rectangle = &XWPSDeviceColor::binaryFillRectangleHT;
		return state.tileBySteps(x, y, w, h, ptile, &ptile->tmask, &XWPSTileFillState::tileMaskedFill);
  }
}

bool XWPSDeviceColor::coloredEqualHT(XWPSDeviceColor * pdevc2)
{
	uint num_comp;

  if (pdevc2->type != type ||
			colors.colored.c_ht != pdevc2->colors.colored.c_ht ||
			colors.colored.alpha != pdevc2->colors.colored.alpha ||
			phase.x != pdevc2->phase.x ||
			phase.y != pdevc2->phase.y)
		return false;
    
  num_comp = colors.colored.c_ht->num_comp;
  return	!memcmp(colors.colored.c_base,	pdevc2->colors.colored.c_base,
  						num_comp * sizeof(colors.colored.c_base[0])) &&
					!memcmp(colors.colored.c_level,		pdevc2->colors.colored.c_level,
							num_comp * sizeof(colors.colored.c_level[0]));
}

int  XWPSDeviceColor::coloredLoadHT(XWPSImagerState *pis,
	      			     XWPSDevice * , 
	      			     PSColorSelect )
{
	XWPSDeviceHalftone *pdht = colors.colored.c_ht;
  XWPSHTCache *pcache = pis->ht_cache;
  XWPSHTOrder *porder =	(pdht->components ? &pdht->components[0].corder : &pdht->order);

  if (pcache->order.bit_data->arr != porder->bit_data->arr)
		pcache->init(porder);
			
	if (porder->cache != pcache)
	{
		if (porder->cache)
			if (porder->cache->decRef() == 0)
				delete porder->cache;
	}
	
  pdht->order.cache = porder->cache = pcache;
  if (pcache)
  {
  	pcache->incRef();
  	pcache->incRef();
  }
  return 0;
}

int  XWPSDeviceColor::coloredFillRectangleHT(int x, 
	                           int y, 
	                           int w, 
	                           int h, 
	                           XWPSDevice * dev,
			                       uint lop,
			                       const PSRopSource * source)
{
	ulong tbits[tile_longs_allocated];
  const uint tile_bytes = tile_longs * sizeof(long);
  XWPSStripBitmap tiles;
  PSRopSource no_source;
  XWPSDeviceHalftone *pdht = colors.colored.c_ht;
  int depth = dev->color_info.depth;
  int nplanes = dev->color_info.num_components;
  int (XWPSDeviceColor::*set_ht_colors)(PSColorValuesPair *, ulong *,	XWPSStripBitmap ** , XWPSDevice * ,
		   				XWPSHTCache ** , int) =	 
		   				dev->procs.map_cmyk_color_.map_cmyk_color == &XWPSDevice::mapCMYKColorCMYK1Bit ? 
		   					&XWPSDeviceColor::setCMYK1BitColors :  nplanes < 4 ? 
	   						&XWPSDeviceColor::setHTColorsLE4 : &XWPSDeviceColor::setHTColorsGT4;
	 
   void (*set_color_ht)(uchar *, uint, int, int, int, int, int, int, int,	ulong , 
                        XWPSDevice *, const PSColorValuesPair *, ulong *,  XWPSStripBitmap ** ) = 
                     !(colors.colored.plane_mask & ~(ulong)15) && 
                      set_ht_colors != &XWPSDeviceColor::setHTColorsGT4 ?
	  					            &set_color_ht_le_4 :	 &set_color_ht_gt_4;
    
  PSColorValuesPair vp;
  ulong colorsA[1 << MAX_DCC];
  XWPSStripBitmap *sbits[MAX_DCC];
  XWPSHTCache *caches[MAX_DCC];
  int special;
  int code = 0;
  int raster;
  uint size_x;
  int dw, dh;
  int lw = pdht->lcm_width, lh = pdht->lcm_height;
  bool no_rop;
  int i;

  if (w <= 0 || h <= 0)
		return 0;
  if ((w | h) >= 16) 
  {
		XWPSFixedRect cbox;
		int t;

		dev->getClipingBox(&cbox);
		if ((t = fixed2int(cbox.p.x)) > x) 
		{
	    if ((w += x - t) <= 0)
				return 0;
	    x = t;
		}
		if ((t = fixed2int(cbox.p.y)) > y) 
		{
	    if ((h += y - t) <= 0)
				return 0;
	    y = t;
		}
		if ((t = fixed2int(cbox.q.x)) < x + w)
	    if ((w = t - x) <= 0)
				return 0;
		if ((t = fixed2int(cbox.q.y)) < y + h)
	    if ((h = t - y) <= 0)
				return 0;
  }
  
  lop &= ~lop_T_transparent;
  if (pdht->components == 0) 
  {
		caches[0] = caches[1] = caches[2] = caches[3] = pdht->order.cache;
		for (i = 4; i < nplanes; ++i)
	    caches[i] = pdht->order.cache;
  } 
  else 
  {
		XWPSHTOrderComponent *pocs = pdht->components;

		caches[0] = pocs[pdht->color_indices[0]].corder.cache;
		caches[1] = pocs[pdht->color_indices[1]].corder.cache;
		caches[2] = pocs[pdht->color_indices[2]].corder.cache;
		caches[3] = pocs[pdht->color_indices[3]].corder.cache;
		for (i = 4; i < nplanes; ++i)
	    caches[i] = pocs[pdht->color_indices[i]].corder.cache;
  }
  special = (this->*set_ht_colors)(&vp, &colorsA[0], &sbits[0], dev, &caches[0], nplanes);
  no_rop = source == NULL && lop_no_S_is_T(lop);
  
  if ((w > lw || h > lh) &&	(raster = bitmap_raster(lw * depth)) <= tile_bytes / lh) 
  {
  	fit_fill_xy(x, y, w, h);
  	if ( w > dev->width - x )
	  	w = dev->width - x;
	  	
	  if ( h > dev->height - y )
	  	h = dev->height - y;
	  	
		if (w > lw || h > lh) 
		{
	    tiles.data = (uchar *)tbits;
	    tiles.raster = raster;
	    tiles.rep_width = tiles.size.x = lw;
	    tiles.rep_height = tiles.size.y = lh;
	    tiles.id = dev->context_state->nextIDS(1);
	    tiles.rep_shift = tiles.shift = 0;
	    (*set_color_ht)((uchar *)tbits, raster, 0, 0, lw, lh, depth,
			 			special, nplanes, colors.colored.plane_mask,
			 			dev, &vp, &colorsA[0], &sbits[0]);
	    if (no_rop)
				return dev->stripTileRectangle(&tiles, x, y, w, h, ps_no_color_index, ps_no_color_index,
					    		phase.x, phase.y);
	    if (source == NULL)
				setRopNoSource(&source, &no_source, dev);
	    return dev->stripCopyRop(source->sdata,
			       source->sourcex, source->sraster, source->id,
			     (source->use_scolors ? source->scolors : NULL),
						     &tiles, NULL, x, y, w, h, phase.x, phase.y, lop);
		}
  }
  size_x = w * depth;
  raster = bitmap_raster(size_x);
  if (raster > tile_bytes) 
  {
		if (x < 0)
	    w += x, x = 0;
		if (x > dev->width - w)
	    w = dev->width - x;
		if (w <= 0)
	    return 0;
		size_x = w * depth;
		raster = bitmap_raster(size_x);
		if (raster > tile_bytes) 
		{
	    dw = tile_bytes * 8 / depth;
	    size_x = dw * depth;
	    raster = bitmap_raster(size_x);
	    dh = 1;
	    goto fit;
		}
  }
  
  dw = w;
  dh = tile_bytes / raster;
  if (dh > h)
		dh = h;
		
fit:	
  if (!no_rop) 
  {
		tiles.data = (uchar *)tbits;
		tiles.id = ps_no_bitmap_id;
		tiles.raster = raster;
		tiles.rep_width = tiles.size.x = size_x / depth;
		tiles.rep_shift = tiles.shift = 0;
  }
  
  while (w) 
 	{
		int cy = y, ch = dh, left = h;

		for (;;) 
		{
	    (*set_color_ht)((uchar *)tbits, raster, x + phase.x, cy + phase.y,
			 dw, ch, depth, special, nplanes,	colors.colored.plane_mask, dev, &vp, &colorsA[0], sbits);
	    if (no_rop) 
				code = dev->copyColor((uchar *)tbits, 0, raster, ps_no_bitmap_id, x, cy, dw, ch);
	    else 
	    {
				tiles.rep_height = tiles.size.y = ch;
        if (source == NULL)
          setRopNoSource(&source, &no_source, dev);
	      code = dev->stripCopyRop(source->sdata, source->sourcex, source->sraster,	source->id,  
	      	(source->use_scolors ? source->scolors : NULL), &tiles, NULL, x, cy, dw, ch, 0, 0, lop);
	    }
	    if (code < 0)
				return code;
	    if (!(left -= ch))
				break;
	    cy += ch;
	    if (ch > left)
				ch = left;
		}
		if (!(w -= dw))
	    break;
		x += dw;
		if (dw > w)
	    dw = w;
  }
  return code;
}

bool XWPSDeviceColor::coloredMaskedEqual(XWPSDeviceColor * pdevc2)
{
	return (this->*(ps_dc_type_ht_colored.equal))(pdevc2) && mask.id == pdevc2->mask.id;
}

int  XWPSDeviceColor::coloredMaskedFillRect(int x, 
	                           int y, 
	                           int w, 
	                           int h, 
	                           XWPSDevice * dev,
			                       uint lop,
			                       const PSRopSource * source)
{
	XWPSColorTile *ptile = mask.m_tile;
  XWPSTileFillState state;
  int code;

  code = state.init(this, dev, true);
  if (code < 0)
		return code;
  if (state.pcdev == dev || ptile->is_simple)
		return coloredFillRectangleHT(x, y, w, h, state.pcdev, lop, source);
  else 
  {
		state.lop = lop;
		state.source = source;
		state.fill_rectangle = &XWPSDeviceColor::coloredFillRectangleHT;
		return state.tileBySteps(x, y, w, h, ptile, &ptile->tmask, &XWPSTileFillState::tileMaskedFill);
  }
}

int XWPSDeviceColor::coloredMaskedLoad(XWPSImagerState * pis,
									XWPSDevice * dev, 
									PSColorSelect select)
{
	int code = (this->*(ps_dc_type_ht_colored.load))(pis, dev, select);

  if (code < 0)
		return code;
    
  while (!patternCacheLookup(pis, dev, select))
	{
		code = patternLoad(pis, dev, select);
		if ( code < 0 ) 
			break;
	}
	
	return code;
}

int XWPSDeviceColor::maskedLoadBinaryDC(XWPSImagerState * pis,
									XWPSDevice * dev, 
									PSColorSelect select)
{
	int code = (this->*(ps_dc_type_ht_binary.load))(pis, dev, select);

  if (code < 0)
		return code;
    
  while (!patternCacheLookup(pis, dev, select))
	{
		code = patternLoad(pis, dev, select);
		if ( code < 0 ) 
			break;
	}
	
	return code;
}

int  XWPSDeviceColor::noFillMasked(const uchar *,
		                               int , int , 
		                               ulong ,
		                               int , 
		                               int , 
		                               int w, 
		                               int h, 
		                               XWPSDevice *,
		                              uint , 
		                              bool)
{
	if (w <= 0 || h <= 0)
		return 0;
    
  return (int)(XWPSError::Fatal);
}

int XWPSDeviceColor::noFillRectangle(int x, 
		                                 int y,
			                               int w, 
			                               int h, 
			                               XWPSDevice *dev,
											               uint lop,
											               const PSRopSource *source)
{
	XWPSDeviceColor filler;
  if (w <= 0 || h <= 0)
		return 0;
  if (lop_uses_T(lop))
		return (int)(XWPSError::Fatal);
  
  filler.setPure(0);
  return filler.pureFillRectangle(x, y, w, h, dev, lop, source);
}

bool XWPSDeviceColor::nullEqual(XWPSDeviceColor * pdevc2)
{
	return pdevc2->type == type;
}

bool XWPSDeviceColor::patternEqualDC(XWPSDeviceColor * pdevc2)
{
	return pdevc2->type == type &&
					phase.x == pdevc2->phase.x &&
					phase.y == pdevc2->phase.y &&
					mask.id == pdevc2->mask.id;
}

int XWPSDeviceColor::patternFillRectangleDC(int x, 
	                            int y,
			                        int w, 
			                        int h, 
			                        XWPSDevice * dev,
			     										uint lop,
			     										const PSRopSource * source)
{
	XWPSColorTile *ptile = colors.pattern.p_tile;
  const PSRopSource *rop_source = source;
  PSRopSource no_source;
  XWPSStripBitmap *bits;
  XWPSTileFillState state;
  int code;

  if (ptile == 0)
		return 0;
  if (rop_source == NULL)
		setRopNoSource(&rop_source, &no_source, dev);
  bits = &ptile->tbits;
  code = state.init(this, dev, false);
  if (code < 0)
		return code;
  if (ptile->is_simple) 
  {
		int px = imod(-(int)(ptile->step_matrix.tx - state.phase.x + 0.5), bits->rep_width);
		int py = imod(-(int)(ptile->step_matrix.ty - state.phase.y + 0.5),bits->rep_height);

		if (state.pcdev != dev)
	    state.cdev->tileClipSetPhase(px, py);
		if (source == NULL && lop_no_S_is_T(lop))
	    code = state.pcdev->stripTileRectangle(bits, x, y, w, h, ps_no_color_index, ps_no_color_index, px, py);
		else
	    code = state.pcdev->stripCopyRop( rop_source->sdata, rop_source->sourcex, rop_source->sraster, rop_source->id,
		 						(rop_source->use_scolors ? rop_source->scolors : NULL),	 bits, NULL, x, y, w, h, px, py, lop);
  } 
  else 
  {
		state.lop = lop;
		state.source = source;
		state.rop_source = rop_source;
		state.orig_dev = dev;
		code = state.tileBySteps(x, y, w, h, ptile,  &ptile->tbits, &XWPSTileFillState::tileColoredFill);
  }
  return code;
}

int XWPSDeviceColor::patternLoad(XWPSImagerState * pis,
									XWPSDevice * dev, 
									PSColorSelect select)
{
	XWPSDevicePatternAccum *adev;
  XWPSPattern1Instance *pinst =	(XWPSPattern1Instance *)ccolor.pattern;
  XWPSState *saved;
  XWPSColorTile *ctile;
  int code;
  
  if (patternCacheLookup(pis, dev, select))
		return 0;
		
	code = pis->ensurePatternCache();
  if (code < 0)
		return code;
		
	adev = new XWPSDevicePatternAccum;
	adev->context_state = dev->context_state;
	adev->setTarget(dev);
  adev->instance = pinst;
 
  code = adev->open();
  if (code < 0)
		goto fail;
    
  saved = new XWPSState(0, pinst->saved);
  if (saved == 0) 
  {
		code = XWPSError::VMError;
		goto fail;
  }
  
  if (saved->pattern_cache == 0)
		saved->pattern_cache = pis->pattern_cache;
		
	saved->setDeviceNoInit(adev);
	code = (ccolor.*pinst->templat.PaintProc)(saved);
	if (code < 0)
	{
		adev->close();
		if (saved)
  		delete saved;
		if (!adev->decRef())
   		delete adev;
		return code;
  }
  
  code = pis->patternCacheAddEntry(adev, &ctile);
  if (code >= 0) 
  {
		if (!patternCacheLookup(pis, dev, select)) 
	    code = XWPSError::Fatal;
  }
  
  adev->close();
  if (saved)
  	delete saved;
  if (!adev->decRef())
   	delete adev;
  return code;
  
fail:
	if (adev)
	{
		if (!adev->decRef())
   		delete adev;
  	delete adev;
  }
  return code;
}

int XWPSDeviceColor::patternLoadDC(XWPSImagerState *pis,
	      			 XWPSDevice *dev, 
	      			 PSColorSelect select)
{
	int code = 0;
	while (!patternCacheLookup(pis, dev, select))
	{
		code = patternLoad(pis, dev, select);
		if ( code < 0 ) 
			break;
	}
	
	return code;
}

bool XWPSDeviceColor::pattern2EqualDC(XWPSDeviceColor * pdevc2)
{
	return pdevc2->type == type && ccolor.pattern == pdevc2->ccolor.pattern;
}

int XWPSDeviceColor::pattern2FillRectangleDC(int x, 
		                                       int y,
			                                     int w, 
			                                     int h, 
			                                     XWPSDevice *dev,
												                   uint ,
												                   const PSRopSource *)
{
	XWPSPattern2Instance *pinst = (XWPSPattern2Instance *)ccolor.pattern;
  XWPSState *pgs = pinst->saved;
  XWPSFixedRect rect;
  XWPSPoint save_adjust;
  int code;

  rect.p.x = int2fixed(x);
  rect.p.y = int2fixed(y);
  rect.q.x = int2fixed(x + w);
  rect.q.y = int2fixed(y + h);
  
  pgs->currentFillAdjust(&save_adjust);
  pgs->setFillAdjust(0.5, 0.5);
  code = ((XWPSShading*)(pinst->templat.Shading))->fillPath(NULL, &rect, dev, pgs, true);
  pgs->setFillAdjust(save_adjust.x, save_adjust.y);
  return code;
}

bool XWPSDeviceColor::pureEqualDC(XWPSDeviceColor * pdevc2)
{
	return pdevc2->type == type && pureColor() == pdevc2->pureColor();
}

int  XWPSDeviceColor::pureFillMasked(const uchar * data,
		                               int data_x, int raster, 
		                               ulong id,
		                               int x, 
		                               int y, 
		                               int w, 
		                               int h, 
		                               XWPSDevice *dev,
		                               uint lop, 
		                               bool invert)
{
	if (lop_no_S_is_T(lop)) 
	{
		ulong color0, color1;
		if (invert)
	    color0 = colors.pure, color1 = ps_no_color_index;
		else
	    color1 = colors.pure, color0 = ps_no_color_index;
		return dev->copyMono(data, data_x, raster, id, x, y, w, h, color0, color1);
  } 
 	{
		ulong scolors[2];
		ulong tcolors[2];

		scolors[0] = dev->getBlack();
		scolors[1] = dev->getWhite();
		tcolors[0] = tcolors[1] = colors.pure;
		return dev->stripCopyRop(data, data_x, raster, id, scolors, NULL, tcolors, x, y, w, h, 0, 0, (invert ? rop3_invert_S(lop) : lop) | lop_S_transparent);
  }
}

int XWPSDeviceColor::pureFillRectangle(int x, 
		                                   int y,
			                                 int w, 
			                                 int h, 
			                                 XWPSDevice *dev,
												               uint lop,
												               const PSRopSource *source)
{
	if (source == NULL && lop_no_S_is_T(lop))
		return dev->fillRectangle(x, y, w, h, colors.pure);
  
  {
		ulong colorsA[2];
		PSRopSource no_source;
		colorsA[0] = colorsA[1] = colors.pure;
		if (source == NULL)
	    setRopNoSource(&source, &no_source, dev);
		return dev->stripCopyRop(source->sdata, source->sourcex, source->sraster,
	     											 source->id, (source->use_scolors ? source->scolors : NULL),
	                           NULL /*arbitrary */ , (const ulong *)(&colorsA[0]), x, y, w, h, 0, 0, lop);
  }
}

bool XWPSDeviceColor::pureMaskedEqualDC(XWPSDeviceColor * pdevc2)
{
	return pureEqualDC(pdevc2) && mask.id == pdevc2->mask.id;
}

int XWPSDeviceColor::pureMaskedFillRectDC(int x, 
	                         int y, 
	                         int w, 
	                         int h, 
	                         XWPSDevice * dev,
			                     uint lop,
			                    const PSRopSource * source)
{
	XWPSColorTile *ptile = mask.m_tile;
  XWPSTileFillState state;
  int code;
    
  code = state.init(this, dev, true);
  if (code < 0)
		return code;
  if (state.pcdev == dev || ptile->is_simple)
		return pureFillRectangle(x, y, w, h, state.pcdev, lop, source);
  else 
  {
		state.lop = lop;
		state.source = source;
		state.fill_rectangle = &XWPSDeviceColor::pureFillRectangle;
		return state.tileBySteps(x, y, w, h, ptile, &ptile->tmask, &XWPSTileFillState::tileMaskedFill);
  }
}

int  XWPSDeviceColor::pureMaskedLoadDC(XWPSImagerState * pis,
		       							XWPSDevice * dev, 
		       							PSColorSelect select)
{
	int code = pureLoadDC(pis, dev, select);

  if (code < 0)
		return code;
  while (!patternCacheLookup(pis, dev, select))
	{
		code = patternLoad(pis, dev, select);
		if ( code < 0 ) 
			break;
	}
	
	return code;
}

void XWPSDeviceColor::ropSourceSetColor(PSRopSource * prs, ulong pixel)
{
	prs->scolors[0] = pixel;
	prs->scolors[1] = pixel;
}

void XWPSDeviceColor::setC(int i, uchar b, uint l)
{
	colors.colored.c_base[i] = b;
  colors.colored.c_level[i] = l;
}

void XWPSDeviceColor::setRopNoSource(const PSRopSource **psource,
			                               PSRopSource *pno_source, 
			                               XWPSDevice *dev)
{
	ulong black;

top:
  black = dev->cached_colors.black;
  if (black == 0)
		*psource = &ps_rop_no_source_0;
  else if (black == 1)
		*psource = &ps_rop_no_source_1;
  else if (black == ps_no_color_index) 
  {
		dev->getBlack();
		goto top;
  } 
  else 
  {
		*pno_source = ps_rop_no_source_0;
		ropSourceSetColor(pno_source, black);
		*psource = pno_source;
  }
}
