/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <math.h>
#include "XWPSDataSource.h"
#include "XWPSPattern.h"
#include "XWPSError.h"
#include "XWPSColor.h"
#include "XWPSPath.h"
#include "XWPSFunction.h"
#include "XWPSColorSpace.h"
#include "XWPSStream.h"
#include "XWPSHalfTone.h"
#include "XWPSDeviceClip.h"
#include "XWPSState.h"
#include "XWPSContextState.h"
#include "XWPSShading.h"

XWPSShadingParams::XWPSShadingParams()
{
	ColorSpace = 0;
	Background = 0;
	have_BBox = false;
	AntiAlias = false;
}

XWPSShadingParams::XWPSShadingParams(XWPSShadingParams * src)
{
	copyCommon(src);
}

XWPSShadingParams::~XWPSShadingParams()
{
	if (ColorSpace)
	{
		delete ColorSpace;
		ColorSpace = 0;
	}
	
	if (Background)
	{
		delete Background;
		Background = 0;
	}
}

int XWPSShadingParams::checkCBFD(XWPSFunction * function, const float *, int m)
{
	int ncomp = ColorSpace->numComponents();

  if (ncomp < 0 ||	(have_BBox &&	(BBox.p.x > BBox.q.x ||	BBox.p.y > BBox.q.y)))
		return (int)(XWPSError::RangeCheck);
			
	if (function != 0)
	{
		if (function->params->m != m || function->params->n != ncomp)
	    return (int)(XWPSError::RangeCheck);
	}
	return 0;
}

void XWPSShadingParams::copyCommon(XWPSShadingParams * src)
{
	ColorSpace = src->ColorSpace;
	src->ColorSpace = 0;
	Background = src->Background;
	src->Background = 0;
	have_BBox = src->have_BBox;
	BBox = src->BBox;
	AntiAlias = src->AntiAlias;
}

XWPSShadingFbParams::XWPSShadingFbParams()
	:XWPSShadingParams()
{
	Domain[0] = Domain[2] = 0;
  Domain[1] = Domain[3] = 1;
  Function = 0;
}

XWPSShadingFbParams::XWPSShadingFbParams(XWPSShadingFbParams * src)
	:XWPSShadingParams()
{
	copyFb(src);
}

XWPSShadingFbParams::~XWPSShadingFbParams()
{
	if (Function)
	{
		if (!Function->decRef())
			delete Function;
		Function = 0;
	}
}

int XWPSShadingFbParams::check()
{
	int code = checkCBFD(Function, Domain, 2);

	XWPSMatrix imat;
  if (code < 0 || (code = imat.matrixInvert(&Matrix)) < 0	)
		return code;
	return 0;
}

void XWPSShadingFbParams::copyFb(XWPSShadingFbParams * src)
{
	copyCommon(src);
	memcpy(Domain, src->Domain, 4 * sizeof(float));
	Matrix = src->Matrix;
	Function = src->Function;
	if (Function)
		Function->incRef();
}

XWPSShadingAParams::XWPSShadingAParams()
	:XWPSShadingParams()
{
	Domain[0] = 0;
  Domain[1] = 1;
  Function = 0;
  Extend[0] = Extend[1] = false;
}

XWPSShadingAParams::XWPSShadingAParams(XWPSShadingAParams * src)
	:XWPSShadingParams()
{
	copyA(src);
}

XWPSShadingAParams::~XWPSShadingAParams()
{
	if (Function)
	{
		if (!Function->decRef())
			delete Function;
		Function = 0;
	}
}

int XWPSShadingAParams::check()
{
	int code = checkCBFD(Function, Domain, 1);
	return code;
}

void XWPSShadingAParams::copyA(XWPSShadingAParams * src)
{
	copyCommon(src);
	memcpy(Coords, src->Coords, 4 * sizeof(float));
	memcpy(Domain, src->Domain, 2 * sizeof(float));
	Function = src->Function;
	if (Function)
			Function->incRef();
	memcpy(Extend, src->Extend, 2 * sizeof(bool));
}

XWPSShadingRParams::XWPSShadingRParams()
	:XWPSShadingParams()
{
	Domain[0] = 0;
  Domain[1] = 1;
  Function = 0;	
  Extend[0] = Extend[1] = false;
}

XWPSShadingRParams::XWPSShadingRParams(XWPSShadingRParams * src)
	:XWPSShadingParams()
{
	copyR(src);
}

XWPSShadingRParams::~XWPSShadingRParams()
{
	if (Function)
	{
		if (!Function->decRef())
			delete Function;
		Function = 0;
	}
}

int XWPSShadingRParams::check()
{
	int code = checkCBFD(Function, Domain, 1);

  if (code < 0)
		return code;
    
  if ((Domain != 0 && Domain[0] == Domain[1]) || Coords[2] < 0 || Coords[5] < 0)
  {
	  return (int)(XWPSError::RangeCheck);
	}
	
	return 0;
}

void XWPSShadingRParams::copyR(XWPSShadingRParams * src)
{
	copyCommon(src);
	memcpy(Coords, src->Coords, 6 * sizeof(float));
	memcpy(Domain, src->Domain, 2 * sizeof(float));
	Function = src->Function;
	if (Function)
			Function->incRef();
	memcpy(Extend, src->Extend, 2 * sizeof(bool));
}

XWPSShadingMeshParams::XWPSShadingMeshParams()
	:XWPSShadingParams()
{
	BitsPerCoordinate = 0;
	BitsPerComponent = 0;
	Decode = 0;
	Function = 0;
	DataSource.initFloats(NULL, 0);
	ds_bytes = 0;
}

XWPSShadingMeshParams::XWPSShadingMeshParams(XWPSShadingMeshParams * src)
	:XWPSShadingParams()
{
	copyMesh(src);
}

XWPSShadingMeshParams::~XWPSShadingMeshParams()
{
	if (DataSource.type == data_source_type_floats)
	{
		if (DataSource.data.str.data)
		{
			float * tmp = (float*)(DataSource.data.str.data);
			delete [] tmp;
		}
	}
	else if (DataSource.type == data_source_type_stream)
	{
		if (DataSource.data.strm)
		{
			if (DataSource.data.strm->decRef() == 0)
				delete DataSource.data.strm;
		}
	}
	
	DataSource.data.str.data = 0;
	DataSource.data.str.size = 0;
	if (Decode)
	{
		delete [] Decode;
		Decode = 0;
	}
	
	if (Function)
	{
		if (!Function->decRef())
			delete Function;
		Function = 0;
	}
	
	if (ds_bytes)
	{
		if (ds_bytes->decRef() == 0)
			delete ds_bytes;
		ds_bytes = 0;
	}
}

int XWPSShadingMeshParams::checkMesh()
{
	if (!DataSource.isArray()) 
	{
		int code = checkCBFD(Function, Decode, 1);

		if (code < 0)
	    return code;
		switch (BitsPerCoordinate) 
		{
	    case  1: case  2: case  4: case  8:
	    case 12: case 16: case 24: case 32:
				break;
				
	    default:
				return (int)(XWPSError::RangeCheck);
		}
		switch (BitsPerComponent) 
		{
	    case  1: case  2: case  4: case  8:
	    case 12: case 16:
				break;
				
	    default:
				return (int)(XWPSError::RangeCheck);
		}
  }
  return 0;
}

void XWPSShadingMeshParams::copyMesh(XWPSShadingMeshParams * src)
{
	copyCommon(src);
	DataSource = src->DataSource;
	src->DataSource.data.str.data = 0;
	src->DataSource.data.str.size = 0;
	src->DataSource.data.strm = 0;
	BitsPerCoordinate = src->BitsPerCoordinate;
	BitsPerComponent = src->BitsPerComponent;
	Decode = src->Decode;
	src->Decode = 0;
	Function = src->Function;
	if (Function)
			Function->incRef();
	ds_bytes = src->ds_bytes;
	if (ds_bytes)
		ds_bytes->incRef();
}

XWPSShadingFfGtParams::XWPSShadingFfGtParams()
	:XWPSShadingMeshParams()
{
	BitsPerFlag = 0;
}

XWPSShadingFfGtParams::XWPSShadingFfGtParams(XWPSShadingFfGtParams * src)
	:XWPSShadingMeshParams()
{
	copyFfGt(src);
}

int XWPSShadingFfGtParams::check()
{
	int code = checkMesh();
  int bpf = DataSource.checkBPF(BitsPerFlag);

  if (code < 0)
		return code;
  if (bpf < 0)
		return bpf;
    
  if (Decode != 0 && Decode[0] == Decode[1])
	  return (int)(XWPSError::RangeCheck);
	
	return 0;
}

void XWPSShadingFfGtParams::copyFfGt(XWPSShadingFfGtParams * src)
{
	copyMesh(src);
	BitsPerFlag = src->BitsPerFlag;
}

XWPSShadingLfGtParams::XWPSShadingLfGtParams()
	:XWPSShadingMeshParams()
{
	VerticesPerRow = 0;
}

XWPSShadingLfGtParams::XWPSShadingLfGtParams(XWPSShadingLfGtParams * src)
	:XWPSShadingMeshParams()
{
	copyLfGt(src);
}

int XWPSShadingLfGtParams::check()
{
	int code = checkMesh();

  if (code < 0)
		return code;
  if (VerticesPerRow < 2)
	  return (int)(XWPSError::RangeCheck);
	
	return 0;
}

void XWPSShadingLfGtParams::copyLfGt(XWPSShadingLfGtParams * src)
{
	copyMesh(src);
	VerticesPerRow = src->VerticesPerRow;
}

XWPSShadingCpParams::XWPSShadingCpParams()
	:XWPSShadingMeshParams()
{
	BitsPerFlag = 0;
}

XWPSShadingCpParams::XWPSShadingCpParams(XWPSShadingCpParams * src)
	:XWPSShadingMeshParams()
{
	copyCp(src);
}

int XWPSShadingCpParams::check()
{
	int code = checkMesh();
  int bpf = DataSource.checkBPF(BitsPerFlag);

  if (code < 0)
		return code;
  if (bpf < 0)
		return bpf;
		
	return 0;
}

void XWPSShadingCpParams::copyCp(XWPSShadingCpParams * src)
{
	copyMesh(src);
	BitsPerFlag = src->BitsPerFlag;
}

XWPSShadingTppParams::XWPSShadingTppParams()
	:XWPSShadingMeshParams()
{
	BitsPerFlag = 0;
}

XWPSShadingTppParams::XWPSShadingTppParams(XWPSShadingTppParams * src)
	:XWPSShadingMeshParams()
{
	copyTpp(src);
}

int XWPSShadingTppParams::check()
{
	int code = checkMesh();
  int bpf = DataSource.checkBPF(BitsPerFlag);

  if (code < 0)
		return code;
  if (bpf < 0)
		return bpf;
		
	return 0;
}

void XWPSShadingTppParams::copyTpp(XWPSShadingTppParams * src)
{
	copyMesh(src);
	BitsPerFlag = src->BitsPerFlag;
}

XWPSShadeCoordStream::XWPSShadeCoordStream()
{
	s = 0;
	bits = 0;
	left = 0;
	params = 0;
}

XWPSShadeCoordStream::~XWPSShadeCoordStream()
{
}

int XWPSShadeCoordStream::getDecoded(int num_bits, const float decode[2], float *pvalue)
{
	return (this->*get_decoded)(num_bits, decode, pvalue);
}

int XWPSShadeCoordStream::getValue(int num_bits, uint *pvalue)
{
	return (this->*get_value)(num_bits, pvalue);
}

int  XWPSShadeCoordStream::nextArrayDecoded(int ,
		                    const float decode[2], 
		                    float *pvalue)
{
	float value;
  uint read;

  if (s->gets((uchar *)&value, sizeof(float), &read) < 0 ||
			read != sizeof(float))
		return (int)(XWPSError::RangeCheck);
  *pvalue = value;
  return 0;
}

int  XWPSShadeCoordStream::nextArrayValue(int num_bits, uint * pvalue)
{
	float value;
  uint read;

  if (s->gets((uchar *)&value, sizeof(float), &read) < 0 ||
			read != sizeof(float) || value < 0 || value >= (1 << num_bits) ||
			value != (int)value)
		return (int)(XWPSError::RangeCheck);
  *pvalue = (uint) value;
  return 0;
}

int  XWPSShadeCoordStream::nextColor(float *pc)
{
	const float *decode = params->Decode + 4;
  XWPSColorSpace *pcs = params->ColorSpace;
  PSColorSpaceIndex index = pcs->getIndex();
  int num_bits = params->BitsPerComponent;

  if (index == ps_color_space_index_Indexed) 
  {
		uint i;
		int code = getValue(num_bits, &i);

		if (code < 0)
	    return code;
  } 
  else 
  {
		int i, code;
		int ncomp = pcs->numComponents();

		for (i = 0; i < ncomp; ++i)
	    if ((code = getDecoded(num_bits, decode + i * 2, &pc[i])) < 0)
				return code;
  }
  return 0;
}

int  XWPSShadeCoordStream::nextColors(XWPSPatchCurve * curves, int num_vertices)
{
	int i, code = 0;

  for (i = 0; i < num_vertices && code >= 0; ++i)
		code = nextColor(curves[i].vertex.cc);
  return code;
}

int  XWPSShadeCoordStream::nextCoords(XWPSFixedPoint * ppt, int num_points)
{
	int num_bits = params->BitsPerCoordinate;
  const float *decode = params->Decode;
  int code = 0;
  int i;

  for (i = 0; i < num_points; ++i) 
  {
		float x, y;

		if ((code = getDecoded(num_bits, decode, &x)) < 0 ||
	    (code = getDecoded(num_bits, decode + 2, &y)) < 0 ||
	    (code = ppt[i].transform2fixed(pctm, x, y)) < 0)
	    break;
  }
  return code;
}

int  XWPSShadeCoordStream::nextCurve(XWPSPatchCurve * curve)
{
	int code = nextCoords(&curve->vertex.p, 1);

  if (code >= 0)
		code = nextCoords(curve->control, 2);
  return code;
}

int  XWPSShadeCoordStream::nextFlag(int BitsPerFlag)
{
	uint flag;
  int code;

  left = 0;
  code = getValue(BitsPerFlag, &flag);
  return (code < 0 ? code : flag);
}

void XWPSShadeCoordStream::nextInit(XWPSShadingMeshParams * paramsA,	XWPSImagerState * pis)
{
	params = paramsA;
  pctm = &pis->ctm;
  if (params->DataSource.isStream())
  {
  	s = params->DataSource.data.strm;
  	if ((s->file != 0 && s->file_limit != max_long) || (s->file == 0 && s->strm == 0))
	    s->reset();
  }
  else
  {
  	ds.readString(params->DataSource.data.str.data, params->DataSource.data.str.size, false);
		s = &ds;
  }
  
  if (params->DataSource.isArray()) 
  {
		get_value = &XWPSShadeCoordStream::nextArrayValue;
		get_decoded = &XWPSShadeCoordStream::nextArrayDecoded;
  } 
  else 
  {
		get_value = &XWPSShadeCoordStream::nextPackedValue;
		get_decoded = &XWPSShadeCoordStream::nextPackedDecoded;
  }
  left = 0;
}

int  XWPSShadeCoordStream::nextPackedDecoded(int num_bits,
		       							 const float decode[2], 
		       							 float *pvalue)
{
	uint value;
  int code = getValue(num_bits, &value);
#if ARCH_CAN_SHIFT_FULL_LONG
  double max_value = (double)(uint) ((1 << num_bits) - 1);
#else
  double max_value = (double)(uint)(num_bits == sizeof(uint) * 8 ? ~0 : ((1 << num_bits) - 1));
#endif

  if (code < 0)
		return code;
  *pvalue =	(decode == 0 ? value / max_value : decode[0] + value * (decode[1] - decode[0]) / max_value);
  return 0;
}

int  XWPSShadeCoordStream::nextPackedValue(int num_bits, uint * pvalue)
{
	if (left >= num_bits)
	{
		left -= num_bits;
		*pvalue = (bits >> left) & ((1 << num_bits) - 1);
	}
	else
	{
		int needed = num_bits - left;
		uint value = bits & ((1 << left) - 1);

		for (; needed >= 8; needed -= 8) 
		{
	    int b = s->getc();

	    if (b < 0)
				return (int)(XWPSError::RangeCheck);
	    value = (value << 8) + b;
		}
		if (needed == 0) 
		{
	    left = 0;
	    *pvalue = value;
		} 
		else 
		{
	    int b = s->getc();

	    if (b < 0)
				return (int)(XWPSError::RangeCheck);
	    bits = b;
	    left = 8 - needed;
	    *pvalue = (value << needed) + (b >> left);
		}
	}
	return 0;
}

int  XWPSShadeCoordStream::nextPatch(int BitsPerFlag,
								 XWPSPatchCurve curve[4], 
								 XWPSFixedPoint interior[4])
{
	int flag = nextFlag(BitsPerFlag);
  int num_colors, code;

  if (flag < 0)
		return 1;
		
  switch (flag & 3) 
  {
		default:
	    return (int)(XWPSError::RangeCheck);
	    
		case 0:
	    if ((code = nextCurve(&curve[0])) < 0 ||
					(code = nextCoords(&curve[1].vertex.p, 1)) < 0)
				return code;
	    num_colors = 4;
	    goto vx;
	    
		case 1:
	    curve[0] = curve[1], curve[1].vertex = curve[2].vertex;
	    goto v3;
	    
		case 2:
	    curve[0] = curve[2], curve[1].vertex = curve[3].vertex;
	    goto v3;
	    
		case 3:
	    curve[1].vertex = curve[0].vertex, curve[0] = curve[3];
	    
v3:	    
			num_colors = 2;
			
vx:	    
			if ((code = nextCoords(curve[1].control, 2)) < 0 ||
					(code = nextCurve(&curve[2])) < 0 ||
					(code = nextCurve(&curve[3])) < 0 ||
					(interior != 0 && (code = nextCoords(interior, 4)) < 0) ||
					(code = nextColors(&curve[4 - num_colors], num_colors)) < 0	)
				return code;
  }
  return 0;
}

int  XWPSShadeCoordStream::nextVertex(XWPSMeshVertex * vertex)
{
	int code = nextCoords(&vertex->p, 1);

  if (code >= 0)
		code = nextColor(vertex->cc);
  return code;
}

#define D1START 0
#define C2START 1
#define D2START 3
#define C1START 0

#define D1CTRL 0
#define C2CTRL 1
#define D2XCTRL 2
#define C1XCTRL 3

#define D1END 1
#define C2END 2
#define D2END 2
#define C1END 3

static void
curve_eval(XWPSFixedPoint * pt, const XWPSFixedPoint * p0,
	   const XWPSFixedPoint * p1, const XWPSFixedPoint * p2,
	   const XWPSFixedPoint * p3, float t)
{
  long a, b, c, d;
  long t01, t12;

  d = p0->x;
  curve_points_to_coefficients(d, p1->x, p2->x, p3->x, a, b, c, t01, t12);
  pt->x = (long) (((a * t + b) * t + c) * t + d);
  d = p0->y;
  curve_points_to_coefficients(d, p1->y, p2->y, p3->y, a, b, c, t01, t12);
  pt->y = (long) (((a * t + b) * t + c) * t + d);
}

static int
merge_splits(double *out, const double *a1, int n1, const double *a2, int n2)
{
	double *p = out;
  int i1 = 0, i2 = 0;
  while (i1 < n1 || i2 < n2)
		if (i1 == n1)
	    *p++ = a2[i2++];
		else if (i2 == n2 || a1[i1] < a2[i2])
	    *p++ = a1[i1++];
		else if (a1[i1] > a2[i2])
	    *p++ = a2[i2++];
		else
	    i1++, *p++ = a2[i2++];
  return p - out;
}

static int
split_xy(double out[4], const XWPSFixedPoint *p0, const XWPSFixedPoint *p1,
	 const XWPSFixedPoint *p2, const XWPSFixedPoint *p3)
{
  double tx[2], ty[2];

  return merge_splits(out, tx, ps_curve_monotonic_points(p0->x, p1->x, p2->x, p3->x, tx),	ty,
											ps_curve_monotonic_points(p0->y, p1->y, p2->y, p3->y, ty));
}

static int
split2_xy(double out[8], const XWPSFixedPoint *p10, const XWPSFixedPoint *p11,
	  const XWPSFixedPoint *p12, const XWPSFixedPoint *p13,
	  const XWPSFixedPoint *p20, const XWPSFixedPoint *p21,
	  const XWPSFixedPoint *p22, const XWPSFixedPoint *p23)
{
    double t1[4], t2[4];

    return merge_splits(out, t1, split_xy(t1, p10, p11, p12, p13), t2, split_xy(t2, p20, p21, p22, p23));
}

static void
Cp_transform(XWPSFixedPoint * pt, const XWPSPatchCurve curve[4],
	     const XWPSFixedPoint ignore_interior[4], float u, float v)
{
	double co_u = 1.0 - u, co_v = 1.0 - v;
    XWPSFixedPoint c1u, d1v, c2u, d2v;

    curve_eval(&c1u, &curve[C1START].vertex.p,
	       &curve[C1XCTRL].control[1], &curve[C1XCTRL].control[0],
	       &curve[C1END].vertex.p, u);
    curve_eval(&d1v, &curve[D1START].vertex.p,
	       &curve[D1CTRL].control[0], &curve[D1CTRL].control[1],
	       &curve[D1END].vertex.p, v);
    curve_eval(&c2u, &curve[C2START].vertex.p,
	       &curve[C2CTRL].control[0], &curve[C2CTRL].control[1],
	       &curve[C2END].vertex.p, u);
    curve_eval(&d2v, &curve[D2START].vertex.p,
	       &curve[D2XCTRL].control[1], &curve[D2XCTRL].control[0],
	       &curve[D2END].vertex.p, v);
#define COMPUTE_COORD(xy)\
    pt->xy = (long)\
	((co_v * c1u.xy + v * c2u.xy) + (co_u * d1v.xy + u * d2v.xy) -\
	 (co_v * (co_u * curve[C1START].vertex.p.xy +\
		  u * curve[C1END].vertex.p.xy) +\
	  v * (co_u * curve[C2START].vertex.p.xy +\
	       u * curve[C2END].vertex.p.xy)))
    COMPUTE_COORD(x);
    COMPUTE_COORD(y);
#undef COMPUTE_COORD
}

static void
Tpp_transform(XWPSFixedPoint * pt, const XWPSPatchCurve curve[4],
	      const XWPSFixedPoint interior[4], float u, float v)
{
	double Bu[4], Bv[4];
  XWPSFixedPoint pts[4][4];
  int i, j;
  double x = 0, y = 0;

  {
		double u2 = u * u, co_u = 1.0 - u, co_u2 = co_u * co_u;
		double v2 = v * v, co_v = 1.0 - v, co_v2 = co_v * co_v;

		Bu[0] = co_u * co_u2, Bu[1] = 3 * u * co_u2,  Bu[2] = 3 * u2 * co_u, Bu[3] = u * u2;
		Bv[0] = co_v * co_v2, Bv[1] = 3 * v * co_v2,  Bv[2] = 3 * v2 * co_v, Bv[3] = v * v2;
  }

  pts[0][0] = curve[0].vertex.p;
  pts[0][1] = curve[0].control[0];
  pts[0][2] = curve[0].control[1];
  pts[0][3] = curve[1].vertex.p;
  pts[1][3] = curve[1].control[0];
  pts[2][3] = curve[1].control[1];
  pts[3][3] = curve[2].vertex.p;
  pts[3][2] = curve[2].control[0];
  pts[3][1] = curve[2].control[1];
  pts[3][0] = curve[3].vertex.p;
  pts[2][0] = curve[3].control[0];
  pts[1][0] = curve[3].control[1];
  pts[1][1] = interior[0];
  pts[2][1] = interior[1];
  pts[2][2] = interior[2];
  pts[1][2] = interior[3];


  for (i = 0; i < 4; ++i)
		for (j = 0; j < 4; ++j) 
		{
	    double coeff = Bu[i] * Bv[j];

	    x += pts[i][j].x * coeff, y += pts[i][j].y * coeff;
		}
  pt->x = (long)x, pt->y = (long)y;
}

XWPSShading::XWPSShading()
	:XWPSStruct()
{
	head.procs.fill_rectangle = 0;
	params = 0;
}

XWPSShading::XWPSShading(XWPSShadingFbParams * pFb)
	:XWPSStruct()
{
	head.type = shading_type_Function_based;
	head.procs.fill_rectangle = &XWPSShading::fillRectangleFb;
	params = new 	XWPSShadingFbParams(pFb);
}

XWPSShading::XWPSShading(XWPSShadingAParams * pA)
	:XWPSStruct()
{
	head.type = shading_type_Axial;
	head.procs.fill_rectangle = &XWPSShading::fillRectangleA;
	params = new 	XWPSShadingAParams(pA);
}

XWPSShading::XWPSShading(XWPSShadingRParams * pR)
	:XWPSStruct()
{
	head.type = shading_type_Radial;
	head.procs.fill_rectangle = &XWPSShading::fillRectangleR;
	params = new 	XWPSShadingRParams(pR);
}

XWPSShading::XWPSShading(XWPSShadingFfGtParams * pFfGt)
	:XWPSStruct()
{
	head.type = shading_type_Free_form_Gouraud_triangle;
	head.procs.fill_rectangle = &XWPSShading::fillRectangleFfGt;
	params = new 	XWPSShadingFfGtParams(pFfGt);
}

XWPSShading::XWPSShading(XWPSShadingLfGtParams * pLfGt)
	:XWPSStruct()
{
	head.type = shading_type_Lattice_form_Gouraud_triangle;
	head.procs.fill_rectangle = &XWPSShading::fillRectangleLfGt;
	params = new 	XWPSShadingLfGtParams(pLfGt);
}

XWPSShading::XWPSShading(XWPSShadingCpParams * pCp)
	:XWPSStruct()
{
	head.type = shading_type_Coons_patch;
	head.procs.fill_rectangle = &XWPSShading::fillRectangleCp;
	params = new 	XWPSShadingCpParams(pCp);
}

XWPSShading::XWPSShading(XWPSShadingTppParams * pTpp)
	:XWPSStruct()
{
	head.type = shading_type_Tensor_product_patch;
	head.procs.fill_rectangle = &XWPSShading::fillRectangleTpp;
	params = new 	XWPSShadingTppParams(pTpp);
}

XWPSShading::~XWPSShading()
{
	if (params)
	{
		switch (head.type)
		{
			case shading_type_Free_form_Gouraud_triangle: 
			case shading_type_Lattice_form_Gouraud_triangle: 
			case shading_type_Coons_patch: 
			case shading_type_Tensor_product_patch: 
				{
					XWPSShadingMeshParams * mparam = (XWPSShadingMeshParams*)params;
					if (mparam->DataSource.isStream())
					{
						if (mparam->DataSource.data.strm)
							if (!(mparam->DataSource.data.strm->decRef()))
								delete mparam->DataSource.data.strm;
								
						mparam->DataSource.data.strm = 0;
					}
					else if (mparam->DataSource.isArray())
					{
						if (mparam->DataSource.data.str.data)
						{
							float * d = (float*)(mparam->DataSource.data.str.data);
							delete [] d;
							mparam->DataSource.data.str.data = 0;
						}
					}
				}
				break;
				
			default:
				break;
		}
		
		delete params;
		params = 0;
	}
}

int XWPSShading::fillPath(XWPSPath *ppath,
		                      const XWPSFixedRect *prect, 
		                     XWPSDevice *orig_dev,
		                     XWPSImagerState *pis, 
		                     bool fill_background)
{
	XWPSMatrix mat;
	pis->ctmOnly(&mat);
  XWPSDevice *dev = orig_dev;
  XWPSFixedRect path_box;
  XWPSRect rect;
  XWPSClipPath *path_clip = 0;
  bool path_clip_set = false;
  XWPSDeviceClip path_dev;
  path_dev.incRef();
  path_dev.context_state = dev->context_state;
  int code = 0;
  
  path_clip = new XWPSClipPath(0);
  path_clip->id = dev->context_state->nextIDS(1);
  dev->getClipingBox(&path_box);
  if (prect)
		path_box.intersect(*(XWPSFixedRect*)prect);
  if (params->have_BBox)
  {
  	XWPSFixedRect bbox_fixed;

		if ((mat.isXXYY() || mat.isXYYX()) &&
	    (code = bbox_fixed.shadeBboxTransform2fixed(&params->BBox, pis)) >= 0)
	  {
	  	path_box.intersect(bbox_fixed);
	  }
	  else
	  {
	  	XWPSPath *box_path;
	    if (path_box.p.x >= path_box.q.x || path_box.p.y >= path_box.q.y)
				goto out;
				
			box_path = new XWPSPath;
			if ((code = path_clip->fromRectangle(&path_box)) < 0 ||
					(code = pathAddBox(box_path, &params->BBox, &pis->ctm)) < 0 ||
					(code = path_clip->intersect(box_path, ps_rule_winding_number, pis)) < 0)
				;
	    delete box_path;
	    if (code < 0)
				goto out;
	    path_clip_set = true;
	  }
  }
  
  if (!path_clip_set) 
  {
		if (path_box.p.x >= path_box.q.x || path_box.p.y >= path_box.q.y)
	    goto out;
		if ((code = path_clip->fromRectangle(&path_box)) < 0)
	    goto out;
  }
  
  if (ppath &&
		(code = path_clip->intersect(ppath, ps_rule_winding_number, pis)) < 0)
		goto out;
		
	path_dev.makeClipTranslateDevice(path_clip->rect_list->list, 0, 0);
  path_dev.target = dev;
  dev->incRef();
  dev = &path_dev;
  dev->open();
  dev->getClipingBox(&path_box);
  if (params->Background && fill_background)
  {
  	int x0 = fixed2int(path_box.p.x);
		int y0 = fixed2int(path_box.p.y);
		int x1 = fixed2int(path_box.q.x);
		int y1 = fixed2int(path_box.q.y);
		XWPSColorSpace *pcs = params->ColorSpace;
		XWPSClientColor cc;
		cc.incRef();
		XWPSDeviceColor dev_color;
	
		cc = *params->Background;
		pcs->restrictColor(&cc);
		pcs->remapColor(pis, &cc, pcs, &dev_color, dev, ps_color_select_texture);
		code = dev_color.fillRectangle(x0, y0, x1 - x0, y1 - y0, dev, pis->log_op, NULL);
		if (code < 0)
	    goto out;
  }
  
  {
		XWPSRect path_rect;

		path_rect.p.x = fixed2float(path_box.p.x);
		path_rect.p.y = fixed2float(path_box.p.y);
		path_rect.q.x = fixed2float(path_box.q.x);
		path_rect.q.y = fixed2float(path_box.q.y);
		rect.bboxTransformInverse(&path_rect, &mat);
  }
  code = fillRectangle(&rect, dev, pis);
out:
  if (path_clip)
		delete path_clip;
  return code;
}

int XWPSShading::fillRectangle(const XWPSRect *prect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis)
{
	return (this->*(head.procs.fill_rectangle))(prect, dev, pis);
}

int XWPSShading::fillRectangleA(const XWPSRect *rect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis)
{
	XWPSShadingAParams * pA = (XWPSShadingAParams*)params;
	XWPSMatrix cmat;
  XWPSRect t_rect;
  XWPSAFillState state;
  XWPSClientColor rcc[2];
  float d0 = pA->Domain[0], d1 = pA->Domain[1];
  float dd = d1 - d0;
  float t0, t1;
  float t[2];
  XWPSPoint dist;
  int i;
  int code;

  state.initFillState(this, dev, pis);
  state.psh = this;
  state.orthogonal = pis->ctm.isXXYY() || pis->ctm.isXYYX();
  state.rect = *rect;
  
  cmat.tx = pA->Coords[0];
  cmat.ty = pA->Coords[1];
  state.delta.x = pA->Coords[2] - pA->Coords[0];
  state.delta.y = pA->Coords[3] - pA->Coords[1];
  cmat.yx = state.delta.x;
  cmat.yy = state.delta.y;
  cmat.xx = cmat.yy;
  cmat.xy = -cmat.yx;
  t_rect.bboxTransformInverse((XWPSRect*)rect, &cmat);
  state.frames[0].t0 = t0 = qMax(t_rect.p.y, (double)0);
  t[0] = t0 * dd + d0;
  state.frames[0].t1 = t1 = qMin(t_rect.q.y, (double)1);
  t[1] = t1 * dd + d0;
  for (i = 0; i < 2; ++i) 
  {
		pA->Function->evaluate(&t[i], rcc[i].paint.values);
  }
  memcpy(state.frames[0].cc, rcc, sizeof(rcc[0]) * 2);
  for (i = 0; i < 2; ++i)
  {
  	if (rcc[i].pattern)
  		rcc[i].pattern->incRef();
  }
  XWPSMatrix ctm;
  pis->currentMatrix(&ctm);
  dist.distanceTransform(state.delta.x, state.delta.y, &ctm);
  state.length = hypot(dist.x, dist.y);
  state.dd = dd;
  state.depth = 1;
  code = state.fillRegion();
  if (pA->Extend[0] && t0 > t_rect.p.y) 
  {
		if (code < 0)
	    return code;
		code = state.fillStripe(&rcc[0], t_rect.p.y, t0);
  }
  if (pA->Extend[1] && t1 < t_rect.q.y) 
  {
		if (code < 0)
	    return code;
		code = state.fillStripe(&rcc[1], t1, t_rect.q.y);
  }
  return code;
}

int XWPSShading::fillRectangleCp(const XWPSRect * prect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis)
{
	XWPSShadingCpParams * pCp = (XWPSShadingCpParams*)params;
	XWPSPatchFillState state;
  XWPSShadeCoordStream cs;
  XWPSPatchCurve curve[4];
  int code;

  state.initFillState(this, prect, dev, pis);
  state.Function = pCp->Function;
  cs.nextInit(pCp, pis);
  while ((code = cs.nextPatch(pCp->BitsPerFlag, curve, NULL)) == 0 &&
	   (code = state.fill(curve, NULL, Cp_transform)) >= 0)
		;
  return qMin(code, 0);
}

int XWPSShading::fillRectangleFb(const XWPSRect *rect, 
	                               XWPSDevice *dev,  
	                               XWPSImagerState *pis)
{
	XWPSShadingFbParams * pFb = (XWPSShadingFbParams*)params;
	XWPSMatrix save_ctm;
  int xi, yi;
  float x[2], y[2];
  XWPSFbFillState state;

  state.initFillState(this, dev, pis);
  state.psh = this;
  
  pis->currentMatrix(&save_ctm);
  ((XWPSState*)(pis))->concat(&pFb->Matrix);
  state.ptm = pis->ctm;
  pis->setMatrix(&save_ctm);
  state.orthogonal = state.ptm.isXXYY() || state.ptm.isXYYX();
  
  {
		XWPSRect pbox;

		pbox.bboxTransformInverse((XWPSRect*)rect, &pFb->Matrix);
		x[0] = qMax(pbox.p.x, (double)(pFb->Domain[0]));
		x[1] = qMin(pbox.q.x, (double)(pFb->Domain[1]));
		y[0] = qMax(pbox.p.y, (double)(pFb->Domain[2]));
		y[1] = qMin(pbox.q.y, (double)(pFb->Domain[3]));
  }
  for (xi = 0; xi < 2; ++xi)
		for (yi = 0; yi < 2; ++yi) 
		{
	    float v[2];

	    v[0] = x[xi], v[1] = y[yi];
	    pFb->Function->evaluate(v, state.frames[0].cc[yi * 2 + xi].paint.values);
		}
  state.frames[0].region.p.x = x[0];
  state.frames[0].region.p.y = y[0];
  state.frames[0].region.q.x = x[1];
  state.frames[0].region.q.y = y[1];
  state.depth = 1;
  return state.fillRegion();
}

int XWPSShading::fillRectangleFfGt(const XWPSRect *prect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis)
{
	XWPSShadingFfGtParams* pFfGt = (XWPSShadingFfGtParams*)params;
	XWPSMeshFillState state;
  XWPSShadeCoordStream cs;
  int num_bits = pFfGt->BitsPerFlag;
  int flag;
  XWPSMeshVertex va, vb, vc;

  state.initFillState(this, prect, dev, pis);
  cs.nextInit(pFfGt, pis);
  while ((flag = cs.nextFlag(num_bits)) >= 0) 
  {
		int code;

		switch (flag) 
		{
	    default:
				return (int)(XWPSError::RangeCheck);
					
	    case 0:
				if ((code = nextVertexGt(&cs, &va)) < 0 ||
		    		(code = cs.nextFlag(num_bits)) < 0 ||
		    		(code = nextVertexGt(&cs, &vb)) < 0 ||
		    		(code = cs.nextFlag(num_bits)) < 0)
		    	return code;
				goto v2;
				
	    case 1:
				va = vb;
	    case 2:
				vb = vc;
				
v2:		
				if ((code = nextVertexGt(&cs, &vc)) < 0 ||
		    		(code = state.fillTriangleGt(&va, &vb, &vc)) < 0)
		    	return code;
		}
  }
  return 0;
}

int XWPSShading::fillRectangleLfGt(const XWPSRect *prect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis)
{
	XWPSShadingLfGtParams * pLfGt = (XWPSShadingLfGtParams*)params;
	XWPSMeshFillState state;
  XWPSShadeCoordStream cs;
  XWPSMeshVertex *vertex;
  XWPSMeshVertex next;
  int per_row = pLfGt->VerticesPerRow;
  int i, code = 0;

  state.initFillState(this, prect, dev, pis);
  cs.nextInit(pLfGt, pis);
  vertex = new XWPSMeshVertex[per_row];
  for (i = 0; i < per_row; ++i)
		if ((code = nextVertexGt(&cs, &vertex[i])) < 0)
	    goto out;
  while (!cs.s->eofp()) 
  {
		code = nextVertexGt(&cs, &next);
		if (code < 0)
	    goto out;
		for (i = 1; i < per_row; ++i) 
		{
	    code = state.fillTriangleGt(&vertex[i - 1], &vertex[i], &next);
	    if (code < 0)
				goto out;
	    vertex[i - 1] = next;
	    code = nextVertexGt(&cs, &next);
	    if (code < 0)
				goto out;
	    code = state.fillTriangleGt(&vertex[i], &vertex[i - 1], &next);
	    if (code < 0)
				goto out;
		}
		vertex[per_row - 1] = next;
  }
  
out:
  delete [] vertex;
  return code;
}

int XWPSShading::fillRectangleR(const XWPSRect *rect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis)
{
	XWPSShadingRParams * pR = (XWPSShadingRParams*)params;
	XWPSRFillState state;
  XWPSClientColor rcc[2];
  float d0 = pR->Domain[0], d1 = pR->Domain[1];
  float dd = d1 - d0;
  float x0 = pR->Coords[0], y0 = pR->Coords[1];
  float r0 = pR->Coords[2];
  float x1 = pR->Coords[3], y1 = pR->Coords[4];
  float r1 = pR->Coords[5];
  float t[2];
  int i;
  int code;

  state.initFillState(this, dev, pis);
  state.psh = this;
  state.rect = *rect;
  t[0] = d0;
  t[1] = d1;
  for (i = 0; i < 2; ++i)
		pR->Function->evaluate(&t[i], rcc[i].paint.values);
  memcpy(state.frames[0].cc, rcc, sizeof(rcc[0]) * 2);
  for (i = 0; i < 2; ++i)
  {
  	if (rcc[i].pattern)
  		rcc[i].pattern->incRef();
  }
  state.delta.x = x1 - x0;
  state.delta.y = y1 - y0;
  state.dr = r1 - r0;
  
  state.width =	(fabs(pis->ctm.xx) + fabs(pis->ctm.xy) + fabs(pis->ctm.yx) + fabs(pis->ctm.yy)) * fabs(state.dr);
  state.dd = dd;
  if (pR->Extend[0]) 
  {
		if (r0 < r1)
	    code = state.fillAnnulus(&rcc[0], 0.0, 0.0, 0.0, r0);
		else
	    code = state.fillAnnulus(&rcc[0], 0.0, 0.0, r0, state.computeRadius(x0, y0, rect));
		if (code < 0)
	    return code;
  }
  state.depth = 1;
  state.frames[0].t0 = (t[0] - d0) / dd;
  state.frames[0].t1 = (t[1] - d0) / dd;
  code = state.fillRegion();
  if (pR->Extend[1]) 
  {
		if (code < 0)
	    return code;
		if (r0 < r1)
	    code = state.fillAnnulus(&rcc[1], 1.0, 1.0, r1, state.computeRadius(x1, y1, rect));
		else
	    code = state.fillAnnulus(&rcc[1], 1.0, 1.0, 0.0, r1);
  }
  return code;
}

int XWPSShading::fillRectangleTpp(const XWPSRect *prect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis)
{
	XWPSShadingTppParams * pTpp = (XWPSShadingTppParams*)params;
	XWPSPatchFillState state;
  XWPSShadeCoordStream cs;
  XWPSPatchCurve curve[4];
  XWPSFixedPoint interior[4];
  int code;

  state.initFillState(this, prect, dev, pis);
  state.Function = pTpp->Function;
  cs.nextInit(pTpp, pis);
  while ((code = cs.nextPatch(pTpp->BitsPerFlag, curve, interior)) == 0) 
  {
		XWPSFixedPoint swapped_interior[4];

		swapped_interior[0] = interior[0];
		swapped_interior[1] = interior[3];
		swapped_interior[2] = interior[2];
		swapped_interior[3] = interior[1];
		code = state.fill(curve, swapped_interior, Tpp_transform);
		if (code < 0)
	    break;
  }
  return qMin(code, 0);
}

int XWPSShading::getLength()
{
	return sizeof(XWPSShading);
}

const char * XWPSShading::getTypeName()
{
	return "shading";
}

int XWPSShading::nextVertexGt(XWPSShadeCoordStream * cs, XWPSMeshVertex * vertex)
{
	int code = cs->nextVertex(vertex);

	XWPSShadingMeshParams * pm = (XWPSShadingMeshParams*)params;
  if (code >= 0 && pm->Function) 
		code = pm->Function->evaluate(vertex->cc, vertex->cc);
  return code;
}

int XWPSShading::pathAddBox(XWPSPath *ppath, 
	               const XWPSRect *pbox, 
	               const XWPSMatrixFixed *pmat)
{
	XWPSFixedPoint pt;
  XWPSFixedPoint pts[3];
  int code;

  if ((code = pt.transform2fixed((XWPSMatrixFixed*)pmat, pbox->p.x, pbox->p.y)) < 0 ||
			(code = ppath->addPoint(pt.x, pt.y)) < 0 ||
			(code = pts[0].transform2fixed((XWPSMatrixFixed*)pmat, pbox->q.x, pbox->p.y)) < 0 ||
			(code = pts[1].transform2fixed((XWPSMatrixFixed*)pmat, pbox->q.x, pbox->q.y)) < 0 ||
			(code = pts[2].transform2fixed((XWPSMatrixFixed*)pmat, pbox->p.x, pbox->q.y)) < 0 ||
			(code = ppath->addLinesNotes(pts, 3, sn_none)) < 0	)
		;
  return code;
}

XWPSShadingFillState::XWPSShadingFillState()
{
	dev = 0;
	pis = 0;
	num_components = 0;
}

int  XWPSShadingFillState::fillPath(XWPSPath * ppath,	XWPSDeviceColor * pdevc)
{
	XWPSFillParams params;

  params.rule = -1;
  params.adjust = pis->fill_adjust;
  params.flatness = 0;	/* irrelevant */
  params.fill_zero_width = false;
  return dev->fillPath(pis, ppath, &params, pdevc, NULL);
}

void XWPSShadingFillState::initFillState(XWPSShading * pshA,
		                            XWPSDevice * devA, 
		                            XWPSImagerState * pisA)
{
	XWPSColorSpace *pcs = pshA->params->ColorSpace;
  float max_error = qMin(pisA->smoothness, (float)0.2);
  
  long num_colors =	qMax(devA->color_info.max_gray, devA->color_info.max_color) + 1;
  const PSRange *ranges = 0;
  int ci;

  dev = devA;
  pis = pisA;
  num_components = pcs->numComponents();
  
top:
	switch (pcs->getIndex())
	{
		case ps_color_space_index_Indexed:
	    pcs = pcs->baseSpace();
	    goto top;
	    
		case ps_color_space_index_CIEDEFG:
	    ranges = pcs->params.defg->RangeDEFG.ranges;
	    break;
	    
		case ps_color_space_index_CIEDEF:
	    ranges = pcs->params.def->RangeDEF.ranges;
	    break;
	    
		case ps_color_space_index_CIEABC:
	    ranges = pcs->params.abc->RangeABC.ranges;
	    break;
	    
		case ps_color_space_index_CIEA:
	    ranges = &pcs->params.a->RangeA;
	    break;
	    
		default:
	    break;
	}
	
	if (num_colors <= 32)
		num_colors *= pisA->dev_ht->order.num_levels;
	
	if (max_error < 1.0 / num_colors)
		max_error = 1.0 / num_colors;
  for (ci = 0; ci < num_components; ++ci)
		cc_max_error[ci] = (ranges == 0 ? max_error : max_error * (ranges[ci].rmax - ranges[ci].rmin));
}

bool XWPSShadingFillState::shadeColors2Converge(XWPSClientColor cc[2])
{
	int ci;

  for (ci = num_components - 1; ci >= 0; --ci)
		if (fabs(cc[1].paint.values[ci] - cc[0].paint.values[ci]) > cc_max_error[ci])
	    return false;
  return true;
}

bool XWPSShadingFillState::shadeColors4Converge(XWPSClientColor cc[4])
{
	int ci;

  for (ci = 0; ci < num_components; ++ci) 
  {
		float  c0 = cc[0].paint.values[ci], c1 = cc[1].paint.values[ci],
	      	c2 = cc[2].paint.values[ci], c3 = cc[3].paint.values[ci];
		float min01, max01, min23, max23;

		if (c0 < c1)
	    min01 = c0, max01 = c1;
		else
	    min01 = c1, max01 = c0;
		if (c2 < c3)
	    min23 = c2, max23 = c3;
		else
	    min23 = c3, max23 = c2;
		if (qMax(max01, max23) - qMin(min01, min23) > cc_max_error[ci])
	    return false;
  }
  return true;
}

int  XWPSShadingFillState::shadeFillDeviceRectangle(const XWPSFixedPoint * p0,
			    											const XWPSFixedPoint * p1,
			   								 				XWPSDeviceColor * pdevc)
{
	long xmin, ymin, xmax, ymax;
  int x, y;

  if (p0->x < p1->x)
		xmin = p0->x, xmax = p1->x;
  else
		xmin = p1->x, xmax = p0->x;
  if (p0->y < p1->y)
		ymin = p0->y, ymax = p1->y;
  else
		ymin = p1->y, ymax = p0->y;

  xmin -= pis->fill_adjust.x;
  if (pis->fill_adjust.x == fixed_half)
		xmin += fixed_epsilon;
  xmax += pis->fill_adjust.x;
  ymin -= pis->fill_adjust.y;
  if (pis->fill_adjust.y == fixed_half)
		ymin += fixed_epsilon;
  ymax += pis->fill_adjust.y;
  x = fixed2int_var_pixround(xmin);
  y = fixed2int_var_pixround(ymin);
  return	pdevc->fillRectangle(x, y, fixed2int_var_pixround(xmax) - x,
				     									fixed2int_var_pixround(ymax) - y, dev, pis->log_op, NULL);
}

XWPSFbFillState::XWPSFbFillState()
	:XWPSShadingFillState()
{
	psh = 0;
	orthogonal = false;
	depth = -1;
}

int XWPSFbFillState::fillRegion()
{
	XWPSShadingFbParams * pFb = (XWPSShadingFbParams*)(psh->params);
	XWPSFbFrame *fp = &frames[depth - 1];
	for (;;)
	{
		const double  x0 = fp->region.p.x, y0 = fp->region.p.y,
	                x1 = fp->region.q.x, y1 = fp->region.q.y;

		if (!shadeColors4Converge(fp->cc) &&  fp < &frames[Fb_max_depth - 1])
		{
			XWPSRect region;
			XWPSMatrix p;
			p = ptm;
	    fp->region.bboxTransform(&p, &region);
	    if (region.q.x - region.p.x > 1 || region.q.y - region.p.y > 1)
				goto recur;
				
			{
				long ax = pis->fill_adjust.x;
				int nx =  fixed2int_pixround(float2fixed(region.q.x) + ax) - fixed2int_pixround(float2fixed(region.p.x) - ax);
				long ay = pis->fill_adjust.y;
				int ny =  fixed2int_pixround(float2fixed(region.q.y) + ay) - fixed2int_pixround(float2fixed(region.p.y) - ay);

				if ((nx > 1 && ny != 0) || (ny > 1 && nx != 0))
		    	goto recur;
			}
		}
		
		{
			XWPSDeviceColor dev_color;
	    XWPSColorSpace *pcs = pFb->ColorSpace;
	    XWPSClientColor cc;
	    XWPSFixedPoint pts[4];
	    int code;

	    cc = fp->cc[0];
	    pcs->restrictColor(&cc);
	    pcs->remapColor(pis, &cc, pcs, &dev_color, dev, ps_color_select_texture);
	    pts[0].transform2fixed(&ptm, x0, y0);
	    pts[2].transform2fixed(&ptm, x1, y1);
	    if (orthogonal) 
				code =  shadeFillDeviceRectangle(&pts[0], &pts[2], &dev_color);
	    else 
	    {
				XWPSPath *ppath = new XWPSPath;

				pts[1].transform2fixed(&ptm, x1, y0);
				pts[3].transform2fixed(&ptm, x0, y1);
				ppath->addPoint(pts[0].x, pts[0].y);
				ppath->addLinesNotes(pts + 1, 3, sn_none);
				code = fillPath(ppath, &dev_color);
				delete ppath;
	    }
	    if (code < 0 || fp == &frames[0])
				return code;
	    --fp;
	    continue;
		}
		
recur:
		{
			XWPSFunction *pfn = pFb->Function;
	    float v[2];
	    int code;

	    if (fabs(y1 - y0) > fabs(x1 - x0)) 
	    {
				double ym = (y0 + y1) * 0.5;
				v[1] = ym;
				v[0] = x0;
				code = pfn->evaluate(v, fp[1].cc[2].paint.values);
				if (code < 0)
		    	return code;
				v[0] = x1;
				code = pfn->evaluate(v, fp[1].cc[3].paint.values);
				fp[1].region.q.x = x1;
				fp[1].region.q.y = fp->region.p.y = ym;
				fp[1].cc[0].paint = fp->cc[0].paint;
				fp[1].cc[1].paint = fp->cc[1].paint;
				fp->cc[0].paint = fp[1].cc[2].paint;
				fp->cc[1].paint = fp[1].cc[3].paint;
	    } 
	    else 
	    {
				double xm = (x0 + x1) * 0.5;
				v[0] = xm;
				v[1] = y0;
				code = pfn->evaluate(v, fp[1].cc[1].paint.values);
				if (code < 0)
		    	return code;
				v[1] = y1;
				code = pfn->evaluate(v, fp[1].cc[3].paint.values);
				fp[1].region.q.x = fp->region.p.x = xm;
				fp[1].region.q.y = y1;
				fp[1].cc[0].paint = fp->cc[0].paint;
				fp[1].cc[2].paint = fp->cc[2].paint;
				fp->cc[0].paint = fp[1].cc[1].paint;
				fp->cc[2].paint = fp[1].cc[3].paint;
	    }
	    if (code < 0)
				return code;
	    fp[1].region.p.x = x0, fp[1].region.p.y = y0;
	    ++fp;
		}
	}
}

XWPSARFrame::XWPSARFrame()
{
	t0 = t1 = 0;
}

XWPSAFillState::XWPSAFillState()
	:XWPSShadingFillState()
{
	psh = 0;
	orthogonal = false;
	length = dd = 0;
	depth = -1;
}

int XWPSAFillState::fillRegion()
{
	XWPSShadingAParams * pA = (XWPSShadingAParams*)(psh->params);
	XWPSFunction * pfn = pA->Function;
  XWPSARFrame *fp = &frames[depth - 1];
  for (;;)
  {
  	double t0 = fp->t0, t1 = fp->t1;
		float ft0, ft1;
		if ((!(pfn->head.is_monotonic > 0 ||  
				(ft0 = (float)t0, ft1 = (float)t1, pfn->isMonotonic(&ft0, &ft1, EFFORT_MODERATE) > 0)) ||
	     	!shadeColors2Converge(fp->cc) && length * (t1 - t0) > 1 && fp < &frames[A_max_depth - 1]))
	  {
	  	double tm = (t0 + t1) * 0.5;
	    float dm = tm * dd + pA->Domain[0];

	    pfn->evaluate(&dm, fp[1].cc[1].paint.values);
	    fp[1].cc[0].paint = fp->cc[0].paint;
	    fp[1].t0 = t0;
	    fp[1].t1 = fp->t0 = tm;
	    fp->cc[0].paint = fp[1].cc[1].paint;
	    ++fp;
	  }
	  else
	  {
	  	int code = fillStripe(&fp->cc[0], t0, t1);

	    if (code < 0 || fp == &frames[0])
				return code;
	    --fp;
	  }
  }
}

int XWPSAFillState::fillStripe(XWPSClientColor *pcc, float t0, float t1)
{
	XWPSShadingAParams * pA = (XWPSShadingAParams*)(psh->params);
	XWPSDeviceColor dev_color;
  XWPSColorSpace *pcs = pA->ColorSpace;
  double	x0 = pA->Coords[0] + delta.x * t0,
					y0 = pA->Coords[1] + delta.y * t0;
  double	x1 = pA->Coords[0] + delta.x * t1,
					y1 = pA->Coords[1] + delta.y * t1;
  XWPSFixedPoint pts[4];
  int code;

  pcs->restrictColor(pcc);
  pcs->remapColor(pis, pcc, pcs, &dev_color, dev, ps_color_select_texture);
  if (x0 == x1 && orthogonal)
  {
  	x0 = rect.p.x;
		x1 = rect.q.x;
  }
  else if (y0 == y1 && orthogonal)
  {
  	y0 = rect.p.y;
		y1 = rect.q.y;
  }
  else
  {
  	XWPSPath *ppath = new XWPSPath;
  	if (fabs(delta.x) < fabs(delta.y))
  	{
  		double slope = delta.x / delta.y;
	    double yi = y0 - slope * (rect.p.x - x0);

	    pts[0].transform2fixed(&pis->ctm, rect.p.x, yi);
	    yi = y1 - slope * (rect.p.x - x1);
	    pts[1].transform2fixed(&pis->ctm, rect.p.x, yi);
	    yi = y1 - slope * (rect.q.x - x1);
	    pts[2].transform2fixed(&pis->ctm, rect.q.x, yi);
	    yi = y0 - slope * (rect.q.x - x0);
	    pts[3].transform2fixed(&pis->ctm, rect.q.x, yi);
  	}
  	else
  	{
  		double slope = delta.y / delta.x;
	    double xi = x0 - slope * (rect.p.y - y0);

	    pts[0].transform2fixed(&pis->ctm, xi, rect.p.y);
	    xi = x1 - slope * (rect.p.y - y1);
	    pts[1].transform2fixed(&pis->ctm, xi, rect.p.y);
	    xi = x1 - slope * (rect.q.y - y1);
	    pts[2].transform2fixed(&pis->ctm, xi, rect.q.y);
	    xi = x0 - slope * (rect.q.y - y0);
	    pts[3].transform2fixed(&pis->ctm, xi, rect.q.y);
  	}
  	
  	ppath->addPoint(pts[0].x, pts[0].y);
		ppath->addLinesNotes(pts + 1, 3, sn_none);
		code = fillPath(ppath, &dev_color);
		delete ppath;
		return code;
  }
  
  pts[0].transform2fixed(&pis->ctm, x0, y0);
  pts[1].transform2fixed(&pis->ctm, x1, y1);
  return	shadeFillDeviceRectangle(&pts[0], &pts[1], &dev_color);
}

XWPSRFillState::XWPSRFillState()
	:XWPSShadingFillState()
{
	psh = 0;
	dr = width = dd = 0;
	depth = -1;
}

double XWPSRFillState::computeRadius(float x, float y, const XWPSRect *rect)
{
	double x0 = rect->p.x - x, y0 = rect->p.y - y, x1 = rect->q.x - x, y1 = rect->q.y - y;
  double r00 = hypot(x0, y0), r01 = hypot(x0, y1),r10 = hypot(x1, y0), r11 = hypot(x1, y1);
  double rm0 = qMax(r00, r01), rm1 = qMax(r10, r11);

  return qMax(rm0, rm1);
}

int XWPSRFillState::fillAnnulus(XWPSClientColor *pcc, float t0, float t1, float r0, float r1)
{
	XWPSDeviceColor dev_color;
	XWPSShadingRParams * pR = (XWPSShadingRParams*)(psh->params);
  XWPSColorSpace *pcs = pR->ColorSpace;
  double x0 = pR->Coords[0] + delta.x * t0,
					y0 = pR->Coords[1] + delta.y * t0;
  double	x1 = pR->Coords[0] + delta.x * t1,
					y1 = pR->Coords[1] + delta.y * t1;
  XWPSPath *ppath = new XWPSPath;
  int code;
  pcs->restrictColor(pcc);
  pcs->remapColor(pis, pcc,pcs, &dev_color, dev, ps_color_select_texture);
  if ((code = ppath->imagerArcAdd(pis, false, x0, y0, r0, 0.0, 360.0, false)) >= 0 &&
			(code = ppath->imagerArcAdd(pis, true, x1, y1, r1,  360.0, 0.0, false)) >= 0) 
	{
		code = fillPath(ppath, &dev_color);
  }
  delete ppath;
  return code;
}

int XWPSRFillState::fillRegion()
{
	XWPSShadingRParams * pR = (XWPSShadingRParams*)(psh->params);
	XWPSFunction *pfn = pR->Function;
  XWPSARFrame *fp = &frames[depth - 1];

  for (;;) 
  {
		double t0 = fp->t0, t1 = fp->t1;
		float ft0, ft1;

		if ((!(pfn->head.is_monotonic > 0 ||  
				(ft0 = (float)t0, ft1 = (float)t1, pfn->isMonotonic(&ft0, &ft1, EFFORT_MODERATE) > 0)) ||
	     	!shadeColors2Converge(fp->cc)) && width * (t1 - t0) > 1 && fp < &frames[R_max_depth - 1]) 
	  {
	    float tm = (t0 + t1) * 0.5;
	    float dm = tm * dd + pR->Domain[0];

	    pfn->evaluate(&dm, fp[1].cc[1].paint.values);
	    fp[1].cc[0].paint = fp->cc[0].paint;
	    fp[1].t0 = t0;
	    fp[1].t1 = fp->t0 = tm;
	    fp->cc[0].paint = fp[1].cc[1].paint;
	    ++fp;
		} 
		else 
		{
	    int code = fillAnnulus(&fp->cc[0], t0, t1, pR->Coords[2] + dr * t0, pR->Coords[2] + dr * t1);

	    if (code < 0 || fp == &frames[0])
				return code;
	    --fp;
		}
  }
}

XWPSMeshFrame::XWPSMeshFrame()
{
	check_clipping = false;
}

XWPSMeshFillState::XWPSMeshFillState()
	:XWPSShadingFillState()
{
	pshm = 0;
	depth = 1;
}

#define SET_MIN_MAX_3(vmin, vmax, a, b, c)\
  if ( a < b ) vmin = a, vmax = b; else vmin = b, vmax = a;\
  if ( c < vmin ) vmin = c; else if ( c > vmax ) vmax = c
  	
  	
int XWPSMeshFillState::fillTriangle()
{
	const XWPSShading *psh = pshm;
  XWPSMeshFrame *fp = &frames[depth - 1];
  int ci;

  for (;;)
  {
  	bool check = fp->check_clipping;
  	if (check) 
  	{
	    long xmin, ymin, xmax, ymax;

	    SET_MIN_MAX_3(xmin, xmax, fp->va.p.x, fp->vb.p.x, fp->vc.p.x);
	    SET_MIN_MAX_3(ymin, ymax, fp->va.p.y, fp->vb.p.y, fp->vc.p.y);
	    if (xmin >= rect.p.x && xmax <= rect.q.x &&
					ymin >= rect.p.y && ymax <= rect.q.y) 
			{
				check = false;
	    } 
	    else if (xmin >= rect.q.x || xmax <= rect.p.x ||
		       ymin >= rect.q.y || ymax <= rect.p.y) 
		  {
				goto next;
	    }
		}
		
		if (fp < &frames[mesh_max_depth - 3]) 
		{
	    for (ci = 0; ci < num_components; ++ci) 
	    {
				float c0 = fp->va.cc[ci], c1 = fp->vb.cc[ci], c2 = fp->vc.cc[ci];
				float cmin, cmax;

				SET_MIN_MAX_3(cmin, cmax, c0, c1, c2);
				if (cmax - cmin > cc_max_error[ci])
		    	goto nofill;
	    }
		}
    
fill:
		{
			XWPSDeviceColor dev_color;
	    XWPSColorSpace *pcs = psh->params->ColorSpace;
	    XWPSClientColor fcc;
	    int code;
	    
	    {
				int ci;

				for (ci = 0; ci < num_components; ++ci)
		    	fcc.paint.values[ci] =(fp->va.cc[ci] + fp->vb.cc[ci] + fp->vc.cc[ci]) / 3.0;
	    }
	    
	   	pcs->restrictColor(&fcc);
	    pcs->remapColor(pis, &fcc,pcs, &dev_color, dev, ps_color_select_texture);
	    if (pis->fill_adjust.x != 0 || pis->fill_adjust.y != 0) 
	    {
				XWPSPath *ppath = new XWPSPath;
	
				ppath->addPoint(fp->va.p.x, fp->va.p.y);
				ppath->addLineNotes(fp->vb.p.x, fp->vb.p.y, sn_none);
				ppath->addLineNotes(fp->vc.p.x, fp->vc.p.y, sn_none);
				code = fillPath(ppath, &dev_color);
				delete ppath;
	    }
	    else
	    {
	    	code = dev->fillTriangle(fp->va.p.x, fp->va.p.y,
		     							fp->vb.p.x - fp->va.p.x, fp->vb.p.y - fp->va.p.y,
		     							fp->vc.p.x - fp->va.p.x, fp->vc.p.y - fp->va.p.y,
		     							&dev_color, pis->log_op);
				if (code < 0)
		    	return code;
	    }
		}
		
next:
		if (fp == &frames[0])
	    return 0;
		--fp;
		continue;
    
nofill:
		{
			XWPSFixedRect region;

	    SET_MIN_MAX_3(region.p.x, region.q.x, fp->va.p.x, fp->vb.p.x, fp->vc.p.x);
	    SET_MIN_MAX_3(region.p.y, region.q.y, fp->va.p.y, fp->vb.p.y, fp->vc.p.y);
	    if (region.q.x - region.p.x <= fixed_1 &&
					region.q.y - region.p.y <= fixed_1)
			{
				long ax = pis->fill_adjust.x;
				int nx = fixed2int_pixround(region.q.x + ax) - fixed2int_pixround(region.p.x - ax);
				long ay = pis->fill_adjust.y;
				int ny = fixed2int_pixround(region.q.y + ay) - fixed2int_pixround(region.p.y - ay);

				if (!(nx > 1 && ny != 0) || (ny > 1 && nx != 0))
		    	goto fill;
			}
		}
		
		{
#define VAB fp[3].vb
#define VAC fp[2].vb
#define VBC fp[1].vb
	    int i;

#define MIDPOINT_FAST(a,b) arith_rshift_1((a) + (b) + 1)
	    VAB.p.x = MIDPOINT_FAST(fp->va.p.x, fp->vb.p.x);
	    VAB.p.y = MIDPOINT_FAST(fp->va.p.y, fp->vb.p.y);
	    VAC.p.x = MIDPOINT_FAST(fp->va.p.x, fp->vc.p.x);
	    VAC.p.y = MIDPOINT_FAST(fp->va.p.y, fp->vc.p.y);
	    VBC.p.x = MIDPOINT_FAST(fp->vb.p.x, fp->vc.p.x);
	    VBC.p.y = MIDPOINT_FAST(fp->vb.p.y, fp->vc.p.y);
#undef MIDPOINT_FAST
	    for (i = 0; i < num_components; ++i) 
	    {
				float ta = fp->va.cc[i], tb = fp->vb.cc[i], tc = fp->vc.cc[i];

				VAB.cc[i] = (ta + tb) * 0.5;
				VAC.cc[i] = (ta + tc) * 0.5;
				VBC.cc[i] = (tb + tc) * 0.5;
	   	}
	   
	   	fp[3].va = fp->va;
	   	fp[3].vc = VAC;
	   	fp[2].va = VAB;
	   	fp[2].vc = VBC;
	   	fp[1].va = VAC;
	   	fp[1].vc = fp->vc;
	   	fp->va = VAB;
	   	fp->vc = VBC;
	   	fp[3].check_clipping = fp[2].check_clipping =fp[1].check_clipping = fp->check_clipping = check;
#undef VAB
#undef VAC
#undef VBC
	   	fp += 3;
		}
  }
}

int XWPSMeshFillState::fillTriangleGt(const XWPSMeshVertex * va,
		                 const XWPSMeshVertex * vb, 
		                 const XWPSMeshVertex * vc)
{
	initFillTriangle(va, vb, vc, true);
  return fillTriangle();
}

void XWPSMeshFillState::initFillState(const XWPSShading * psh,
	                  const XWPSRect * prect, 
		                XWPSDevice * devA, 
		                XWPSImagerState * pisA)
{
	XWPSShadingFillState::initFillState((XWPSShading*)psh, devA, pisA);
	pshm = (XWPSShading*)psh;
  rect.shadeBboxTransform2fixed((XWPSRect*)prect, pis);
}

void XWPSMeshFillState::initFillTriangle(const XWPSMeshVertex *va, 
	                      const XWPSMeshVertex *vb, 
	                      const XWPSMeshVertex *vc,
                        bool check_clipping)
{
	depth = 1;
  frames[0].va = *va;
  frames[0].vb = *vb;
  frames[0].vc = *vc;
  frames[0].check_clipping = check_clipping;
}

XWPSPatchFillState::XWPSPatchFillState()
	:XWPSMeshFillState()
{
	Function = 0;
}

int  XWPSPatchFillState::fill(const XWPSPatchCurve curve[4],
	   				const XWPSFixedPoint interior[4],
	   				void (*transform)(XWPSFixedPoint *, 
	   				                   const XWPSPatchCurve[4],	
	   				                   const XWPSFixedPoint[4], float, 
	   				                   float))
{
	double su[9], sv[9];
  int nu = split2_xy(su, &curve[C1START].vertex.p,&curve[C1XCTRL].control[1],
		       &curve[C1XCTRL].control[0], &curve[C1END].vertex.p,
		       &curve[C2START].vertex.p, &curve[C2CTRL].control[0],
		       &curve[C2CTRL].control[1], &curve[C2END].vertex.p);
  int nv = split2_xy(sv, &curve[D1START].vertex.p, &curve[D1CTRL].control[0],
		       &curve[D1CTRL].control[1], &curve[D1END].vertex.p,
		       &curve[D2START].vertex.p, &curve[D2XCTRL].control[1],
		       &curve[D2XCTRL].control[0], &curve[D2END].vertex.p);
  int iu, iv, ju, jv, ku, kv;
  double du, dv;
  double v0, v1, vn, u0, u1, un;
  XWPSPatchColor c00, c01, c10, c11;
  bool check = true;
  su[nu] = 1;
  sv[nv] = 1;
  {
  	long flatness = float2fixed(pis->flatness);
		int i;
		int log2_k[4];

		for (i = 0; i < 4; ++i) 
		{
	    XWPSCurveSegment cseg;

	    cseg.p1 = curve[i].control[0];
	    cseg.p2 = curve[i].control[1];
	    cseg.pt = curve[(i + 1) & 3].vertex.p;
	    log2_k[i] =	cseg.log2Samples(curve[i].vertex.p.x, curve[i].vertex.p.y, flatness);
		}
		ku = 1 << qMax(log2_k[1], log2_k[3]);
		kv = 1 << qMax(log2_k[0], log2_k[2]);
  }
  
#define PATCH_SET_COLOR(c, v)\
  if ( Function ) c.t = v.cc[0];\
  else memcpy(c.cc.paint.values, v.cc, sizeof(c.cc.paint.values))

    PATCH_SET_COLOR(c00, curve[D1START].vertex);
    PATCH_SET_COLOR(c01, curve[D1END].vertex);
    PATCH_SET_COLOR(c11, curve[C2END].vertex);
    PATCH_SET_COLOR(c10, curve[C1END].vertex);

#undef PATCH_SET_COLOR

	du = 1.0 / ku;
  dv = 1.0 / kv;
  for (iv = 0, jv = 0, v0 = 0, v1 = vn = dv; jv < kv; v0 = v1, v1 = vn)
  {
  	XWPSPatchColor c0v0, c0v1, c1v0, c1v1;
  	
#define CHECK_SPLIT(ix, jx, x1, xn, dx, ax)\
  	if (x1 > ax[ix])\
      x1 = ax[ix++];\
  	else {\
      xn += dx;\
      jx++;\
      if (x1 == ax[ix])\
	  ix++;\
  	}
  	
  	CHECK_SPLIT(iv, jv, v1, vn, dv, sv);

		interpolateColor(&c0v0, &c00, &c01, v0);
		interpolateColor(&c0v1, &c00, &c01, v1);
		interpolateColor(&c1v0, &c10, &c11, v0);
		interpolateColor(&c1v1, &c10, &c11, v1);
		
		for (iu = 0, ju = 0, u0 = 0, u1 = un = du; ju < ku; u0 = u1, u1 = un)
		{
			XWPSPatchColor cu0v0, cu1v0, cu0v1, cu1v1;
	    int code;

	    CHECK_SPLIT(iu, ju, u1, un, du, su);

#undef CHECK_SPLIT

			interpolateColor(&cu0v0, &c0v0, &c1v0, u0);
	    resolveColor(&cu0v0);
	    interpolateColor(&cu1v0, &c0v0, &c1v0, u1);
	    resolveColor(&cu1v0);
	    interpolateColor(&cu0v1, &c0v1, &c1v1, u0);
	    resolveColor(&cu0v1);
	    interpolateColor(&cu1v1, &c0v1, &c1v1, u1);
	    resolveColor(&cu1v1);
	    
	    {
	    	XWPSMeshVertex mu0v0, mu1v0, mu1v1, mu0v1;

				(*transform)(&mu0v0.p, curve, interior, u0, v0);
				(*transform)(&mu1v0.p, curve, interior, u1, v0);
				(*transform)(&mu1v1.p, curve, interior, u1, v1);
				(*transform)(&mu0v1.p, curve, interior, u0, v1);
				
				memcpy(mu0v0.cc, cu0v0.cc.paint.values, sizeof(mu0v0.cc));
				memcpy(mu1v0.cc, cu1v0.cc.paint.values, sizeof(mu1v0.cc));
				memcpy(mu1v1.cc, cu1v1.cc.paint.values, sizeof(mu1v1.cc));
				memcpy(mu0v1.cc, cu0v1.cc.paint.values, sizeof(mu0v1.cc));
				
#define FILL_TRI(pva, pvb, pvc)\
    initFillTriangle(pva, pvb, pvc, check);\
    code = fillTriangle();\
    if (code < 0)\
	return code;
	
				{
					XWPSMeshVertex mmid;
		    	int ci;

		    	(*transform)(&mmid.p, curve, interior, (u0 + u1) * 0.5, (v0 + v1) * 0.5);
		    	for (ci = 0; ci < num_components; ++ci)
						mmid.cc[ci] = (mu0v0.cc[ci] + mu1v0.cc[ci] + mu1v1.cc[ci] + mu0v1.cc[ci]) * 0.25;
		    	FILL_TRI(&mu0v0, &mu1v0, &mmid);
		    	FILL_TRI(&mu1v0, &mu1v1, &mmid);
		    	FILL_TRI(&mu1v1, &mu0v1, &mmid);
		    	FILL_TRI(&mu0v1, &mu0v0, &mmid);
				}
	    }
		}
  }
  
  return 0;
}

void XWPSPatchFillState::interpolateColor(XWPSPatchColor * ppcr, 
	                      const XWPSPatchColor * ppc0,
                        const XWPSPatchColor * ppc1, 
                        float t)
{
	if (Function)
		ppcr->t = ppc0->t + t * (ppc1->t - ppc0->t);
  else 
  {
		int ci;

		for (ci = num_components - 1; ci >= 0; --ci)
	    ppcr->cc.paint.values[ci] =	ppc0->cc.paint.values[ci] +	t * (ppc1->cc.paint.values[ci] - ppc0->cc.paint.values[ci]);
  }
}

void XWPSPatchFillState::resolveColor(XWPSPatchColor * ppcr)
{
	if (Function)
		Function->evaluate(&ppcr->t, ppcr->cc.paint.values);
}
