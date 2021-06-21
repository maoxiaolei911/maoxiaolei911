/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPSError.h"
#include "XWPSColorSpace.h"
#include "XWPSDeviceCompositeAlpha.h"
#include "XWPSComposite.h"

PSPixelRow::PSPixelRow()
{
	data = 0;
	bits_per_value = 0;
	initial_x = 0;
}

PSPixelRow::~PSPixelRow()
{
}

XWPSComposite::XWPSComposite()
	:XWPSStruct()
{
	type.procs.create_default_compositor_.create_default_compositor = 0;
	type.procs.equal_.equal = 0;
	type.procs.write_.write = 0;
	id = 0;
}

int XWPSComposite::createDefaultCompositor(XWPSDevice** pcdev,
	                            XWPSDevice * dev,
	                            const XWPSImagerState *pis)
{
	return (this->*(type.procs.create_default_compositor_.create_default_compositor))(pcdev, dev, pis);
}

bool XWPSComposite::equal(XWPSComposite * pc2)
{
	return (this->*(type.procs.equal_.equal))(pc2);
}

int XWPSComposite::getLength()
{
	return sizeof(XWPSComposite);
}

const char * XWPSComposite::getTypeName()
{
	return "composite";
}

int XWPSComposite::write(uchar * b, uint *s)
{
	return (this->*(type.procs.write_.write))(b, s);
}

XWPSAlphaCompositeState::XWPSAlphaCompositeState()
{
	pcte = 0;
	cdev = 0;
	orig_dev = 0;
}

XWPSAlphaCompositeState::~XWPSAlphaCompositeState()
{
	if (pcte)
	{
		if (!pcte->decRef())
			delete pcte;
		pcte = 0;
	}
}

XWPSCompositeAlpha::XWPSCompositeAlpha(const PSCompositeAlphaParams * pparams)
	:XWPSComposite()
{
	type.procs.create_default_compositor_.create_default_compositoralpha = &XWPSCompositeAlpha::createDefaultCompositorDCA;
  type.procs.equal_.equalalpha = &XWPSCompositeAlpha::equalDCA;
  type.procs.write_.writealpha = &XWPSCompositeAlpha::writeDCA;
  id = 0;
  params = *pparams;
}

int XWPSCompositeAlpha::createDefaultCompositorDCA(XWPSDevice ** pcdev, 
	                                XWPSDevice * dev, 
	                                const XWPSImagerState * )
{
	XWPSDeviceCompositeAlpha *cdevA;

  if (params.op == composite_Copy) 
  {
		*pcdev = dev;
		return 0;
  }
  
  cdevA = new XWPSDeviceCompositeAlpha;
  *pcdev = cdevA;
  cdevA->copyParams(dev);
  cdevA->color_info.depth = (dev->color_info.num_components == 4 ? 32 : (dev->color_info.num_components + 1) * 8);
  cdevA->color_info.max_gray = cdevA->color_info.max_color = 255;
  cdevA->color_info.dither_grays = cdevA->color_info.dither_colors = 256;
  cdevA->setTarget(dev);
  cdevA->params = params;
  return 0;
}

bool XWPSCompositeAlpha::equalDCA(XWPSComposite * pcte2)
{
	if (pcte2->type.procs.create_default_compositor_.create_default_compositoralpha != &XWPSCompositeAlpha::createDefaultCompositorDCA)
		return false;
		
	
	XWPSCompositeAlpha * pc2 = (XWPSCompositeAlpha*)(pcte2);
	return (pc2->params.op == params.op &&  (params.op != composite_Dissolve ||  pc2->params.delta == params.delta));
}

int XWPSCompositeAlpha::getLength()
{
	return sizeof(XWPSCompositeAlpha);
}

const char * XWPSCompositeAlpha::getTypeName()
{
	return "compositealpha";
}

int XWPSCompositeAlpha::writeDCA(uchar * data, uint * psize)
{
	uint size = *psize;
  uint used;

  if (params.op == composite_Dissolve) 
  {
		used = 1 + sizeof(params.delta);
		if (size < used) 
		{
	    *psize = used;
	    return (int)(XWPSError::RangeCheck);
		}
		memcpy(data + 1, &params.delta, sizeof(params.delta));
  } 
  else 
  {
		used = 1;
		if (size < used) 
		{
	    *psize = used;
	    return (int)(XWPSError::RangeCheck);
		}
  }
  *data = (uchar)params.op;
  *psize = used;
  return 0;
}

int XWPSCompositeAlpha::readDCA(XWPSComposite ** ppcte, const uchar * data, uint size)
{
	PSCompositeAlphaParams params;

  if (size < 1 || *data > composite_op_last)
	  return (int)(XWPSError::RangeCheck);
	
  params.op = (PSCompositeOp)(*data);
  if (params.op == composite_Dissolve) 
  {
		if (size != 1 + sizeof(params.delta))
	    return (int)(XWPSError::RangeCheck);
	    	
		memcpy(&params.delta, data + 1, sizeof(params.delta));
  } 
  else 
  {
		if (size != 1)
	    return (int)(XWPSError::RangeCheck);
  }
  *ppcte = new XWPSCompositeAlpha(&params);
  return 0;
}

int composite_values(const PSPixelRow * pdest, 
                     const PSPixelRow * psource,
                     int values_per_pixel, 
                     uint num_pixels, 
                     const PSCompositeParams * pcp)
{
	int dest_bpv = pdest->bits_per_value;
  int source_bpv = psource->bits_per_value;
  
  int source_alpha_j = (psource->alpha == ps_image_alpha_last ? values_per_pixel :
     										psource->alpha == ps_image_alpha_first ? 0 : -1);

  int dest_alpha_j = (pdest->alpha == ps_image_alpha_last ? values_per_pixel :
     									pdest->alpha == ps_image_alpha_first ? 0 : -1);

  int dest_vpp = values_per_pixel + (dest_alpha_j >= 0);
  int source_vpp = values_per_pixel + (source_alpha_j >= 0);

  bool constant_colors = psource->data == 0;
  uint highlight_value = (1 << dest_bpv) - 1;

  sample_load_declare(sptr, sbit);
  sample_store_declare(dptr, dbit, dbyte);

  {
		uint xbit = pdest->initial_x * dest_bpv * dest_vpp;

		sample_store_setup(dbit, xbit & 7, dest_bpv);
		dptr = pdest->data + (xbit >> 3);
  }
  {
		uint xbit = psource->initial_x * source_bpv * source_vpp;

		sbit = xbit & 7;
		sptr = psource->data + (xbit >> 3);
  }
  {
		uint source_max = (1 << source_bpv) - 1;
		uint dest_max = (1 << dest_bpv) - 1;
		
		float source_delta = pcp->delta * dest_max / source_max;
		float dest_delta = 1.0 - pcp->delta;
		uint source_alpha = pcp->source_alpha;
		uint dest_alpha = dest_max;

#ifdef PREMULTIPLY_TOWARDS_WHITE
		uint source_bias = source_max - source_alpha;
		uint dest_bias = 0;
		uint result_bias = 0;

#endif
		uint x;

		if (!pdest->alpha) 
		{
	    uint mask = (psource->alpha || source_alpha != source_max ? alpha_out_S_notD : alpha_out_notS_notD);

	    if ((mask >> pcp->cop) & 1) 
	    	return (int)(XWPSError::RangeCheck);
		}
		
		sample_store_preload(dbyte, dptr, dbit, dest_bpv);

		for (x = 0; x < num_pixels; ++x) 
		{
	    int j;
	    uint result_alpha = dest_alpha;

#define get_value(v, ptr, bit, bpv, vmax)\
  sample_load16(v, ptr, bit, bpv)

#define put_value(v, ptr, bit, bpv, bbyte)\
  sample_store_next16(v, ptr, bit, bpv, bbyte)

#define advance(ptr, bit, bpv)\
  sample_next(ptr, bit, bpv)

	    if (dest_alpha_j >= 0) 
	    {
				int dabit = dbit + dest_bpv * dest_alpha_j;
				const uchar *daptr = dptr + (dabit >> 3);

				get_value(dest_alpha, daptr, dabit & 7, dest_bpv, dest_max);
#ifdef PREMULTIPLY_TOWARDS_WHITE
				dest_bias = dest_max - dest_alpha;
#endif
	    }
	    
	    if (source_alpha_j >= 0) 
	    {
				int sabit = sbit;
				const uchar *saptr = sptr;

				if (source_alpha_j == 0)
		    	advance(sptr, sbit, source_bpv);
				else
		    	advance(saptr, sabit, source_bpv * source_alpha_j);
				get_value(source_alpha, saptr, sabit, source_bpv, source_max);
#ifdef PREMULTIPLY_TOWARDS_WHITE
				source_bias = source_max - source_alpha;
#endif
	    }
	    
#define fr(v, a) ((v) * (a) / source_max)
#define nfr(v, a, maxv) ((v) * (maxv - (a)) / source_max)

	    for (j = 0; j <= values_per_pixel; ++j) 
	    {
				uint dest_v, source_v, result;

#define set_clamped(r, v)\
   if ( (r = (v)) > dest_max ) r = dest_max; 

				if (j == 0) 
				{
		    	source_v = source_alpha;
		    	dest_v = dest_alpha;
				} 
				else 
				{
		    	if (constant_colors)
						source_v = pcp->source_values[j - 1];
		    	else 
		    	{
						get_value(source_v, sptr, sbit, source_bpv, source_max);
						advance(sptr, sbit, source_bpv);
		    	}
		    	get_value(dest_v, dptr, dbit, dest_bpv, dest_max);
#ifdef PREMULTIPLY_TOWARDS_WHITE
		    	source_v -= source_bias;
		    	dest_v -= dest_bias;
#endif
				}

				switch (pcp->cop) 
				{
		    	case composite_Clear:
						result = 0;
						break;
						
		    	case composite_Copy:
						result = source_v;
						break;
						
		    	case composite_PlusD:
						result = source_v + dest_v;
						result = (result < dest_max ? 0 : result - dest_max);
						break;
						
		    	case composite_PlusL:
						set_clamped(result, source_v + dest_v);
						break;
						
		    	case composite_Sover:
						set_clamped(result, source_v + nfr(dest_v, source_alpha, source_max));
						break;
						
		    	case composite_Dover:
						set_clamped(result, nfr(source_v, dest_alpha, dest_max) + dest_v);
						break;
						
		    	case composite_Sin:
						result = fr(source_v, dest_alpha);
						break;
						
		    	case composite_Din:
						result = fr(dest_v, source_alpha);
						break;
						
		    	case composite_Sout:
						result = nfr(source_v, dest_alpha, dest_max);
						break;
						
		    	case composite_Dout:
						result = nfr(dest_v, source_alpha, source_max);
						break;
						
		    	case composite_Satop:
						set_clamped(result, fr(source_v, dest_alpha) + nfr(dest_v, source_alpha, source_max));
						break;
						
		    	case composite_Datop:
						set_clamped(result, nfr(source_v, dest_alpha, dest_max) +  fr(dest_v, source_alpha));
						break;
						
		    	case composite_Xor:
						set_clamped(result, nfr(source_v, dest_alpha, dest_max) +  nfr(dest_v, source_alpha, source_max));
						break;
						
		    	case composite_Highlight:
						if (j != 0 && !((source_v ^ highlight_value) & ~1))
			    		result = source_v ^ 1;
						else
			    		result = source_v;
						break;
						
		    	case composite_Dissolve:
						result = (uint) (source_v * source_delta + dest_v * dest_delta);
						break;
						
		    	default:
						return (int)(XWPSError::RangeCheck);
				}
#ifdef PREMULTIPLY_TOWARDS_WHITE
				if (j == 0) 
				{
		    	result_alpha = result;
		    	result_bias = dest_max - result_alpha;
		    	if (dest_alpha_j != 0)
						continue;
				} 
				else 
				{
		    	result += result_bias;
				}
#else
				if (j == 0 && dest_alpha_j != 0) 
				{
		    	result_alpha = result;
		    	continue;
				}
#endif
				put_value(result, dptr, dbit, dest_bpv, dbyte);
	    }
	    
	    if (source_alpha_j > 0)
				advance(sptr, sbit, source_bpv);
	    if (dest_alpha_j > 0)
				put_value(result_alpha, dptr, dbit, dest_bpv, dbyte);
#undef get_value
#undef put_value
#undef advance
		}
		sample_store_flush(dptr, dbit, dest_bpv, dbyte);
  }
  return 0;
}
