/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSState.h"
#include "XWPSContextState.h"

#define NUM_PUSH(nsource) ((nsource) * 2 + 5)
#define EBOT_NUM_SOURCES(ep) ((ep) + 2)
#define EBOT_SOURCE(ep, i)\
  ((ep) + 3 + (EBOT_NUM_SOURCES(ep)->value.intval - 1 - (i)) * 2)
#define ETOP_SOURCE(ep, i)\
  ((ep) - 4 - (i) * 2)
#define ETOP_PLANE_INDEX(ep) ((ep) - 2)
#define ETOP_NUM_SOURCES(ep) ((ep) - 1)

int XWPSContextState::dataImageParams(XWPSRef *op, 
	                    XWPSDataImage *pim,
		                  XWPSImageParams *pip, 
		                  bool require_DataSource,
		                  int num_components, 
		                  int max_bits_per_component)
{
	int code;
  int decode_size;
  XWPSRef *pds;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
    
  code = op->checkDictRead();
  if (code < 0)
  	return code;
  	
  if ((code = op->dictIntParam(this, "Width", 0, max_int_in_fixed / 2, -1, &pim->Width)) < 0 ||
			(code = op->dictIntParam(this, "Height", 0, max_int_in_fixed / 2, -1, &pim->Height)) < 0 ||
			(code = op->dictMatrixParam(this, "ImageMatrix", &pim->ImageMatrix)) < 0 ||
			(code = op->dictBoolParam(this, "MultipleDataSources", false,	&pip->MultipleDataSources)) < 0 ||
			(code = op->dictIntParam(this, "BitsPerComponent", 1, max_bits_per_component, -1,&pim->BitsPerComponent)) < 0 ||
			(code = decode_size = op->dictFloatsParam(this, "Decode",	num_components * 2,	&pim->Decode[0], NULL)) < 0 ||
			(code = op->dictBoolParam(this, "Interpolate", false,	&pim->Interpolate)) < 0)
		return code;
  
  pip->pDecode = &pim->Decode[0];
  
  if ((code = op->dictFindString(this, "DataSource", &pds)) <= 0) 
  {
		if (require_DataSource)
	    return (code < 0 ? code : (int)(XWPSError::RangeCheck));
		return 1;
  }
  
  if (pip->MultipleDataSources) 
  {
		code = pds->checkTypeOnly(XWPSRef::Array);
		if (code < 0)
  		return code;
		if (pds->size() != num_components)
	    return (int)(XWPSError::RangeCheck);
	  
	  memcpyRef(&pip->DataSource[0], pds->getArray(), num_components);
  } 
  else
		pip->DataSource[0].assign(pds);
  return 0;
}

int XWPSContextState::imageCleanup()
{
  XWPSRef * ep_top = exec_stack.getCurrentTop() + NUM_PUSH(EBOT_NUM_SOURCES(exec_stack.getCurrentTop())->value.intval);
  ep_top->makeNull();
  return 0;
}

int XWPSContextState::imageFileContinue()
{
  XWPSImageEnums *penum = (XWPSImageEnums*)(exec_stack.getCurrentTop()->getStruct());
  int num_sources = ETOP_NUM_SOURCES(exec_stack.getCurrentTop())->value.intval;

  for (;;) 
  {
		uint min_avail = max_int;
		XWPSString plane_data[ps_image_max_planes];
		int code;
		int px;
		XWPSRef *pp;
		bool at_eof = false;
		for (px = 0, pp = ETOP_SOURCE(exec_stack.getCurrentTop(), 0); px < num_sources;  ++px, pp -= 2) 
		{
	    int num_aliases = pp[1].value.intval;
	    XWPSStream *s = pp->getStream();
	    int min_left;
	    uint avail;
	    if (num_aliases <= 0)
				num_aliases = ETOP_SOURCE(exec_stack.getCurrentTop(), -num_aliases)[1].value.intval;
	    while ((avail = s->bufAvailable()) <= (min_left = s->bufMinLeft()) + num_aliases - 1) 
	    {
				int next = s->end_status;

				switch (next) 
				{
					case 0:
		    		s->processReadBuf();
		    		continue;
		    		
					case EOFC:
		    		at_eof = true;
		    		break;
		    		
					case INTC:
					case CALLC:
		    		return	0;
		
					default:
		    		return (int)(XWPSError::IOError);
				}
				break;
	    }
	    
	    if (avail >= min_left)
				avail = (avail - min_left) / num_aliases;
	    if (avail < min_avail)
				min_avail = avail;
	    plane_data[px].data = s->bufPtr();
	    plane_data[px].size = avail;
		}

		{
	    int pi;
	    uint used[ps_image_max_planes];

	    code = penum->imageNextPlanes(plane_data, used);
	    for (pi = 0, pp = ETOP_SOURCE(exec_stack.getCurrentTop(), 0); pi < num_sources; ++pi, pp -= 2)
				pp->value.pfile->bufSkip(used[pi]);
	    if (code == XWPSError::RemapColor)
				return code;
		}
		if (at_eof)
	    code = 1;
		if (code) 
		{
	    exec_stack.setCurrentTop(zimagePopEStack(exec_stack.getCurrentTop()));
	    imageCleanup();
	    return (code < 0 ? code : PS_O_POP_ESTACK);
		}
  }
}

int XWPSContextState::imageProcContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSImageEnums *penum = (XWPSImageEnums*)(exec_stack.getCurrentTop()->getStruct());
  int px = ETOP_PLANE_INDEX(exec_stack.getCurrentTop())->value.intval;
  int num_sources = ETOP_NUM_SOURCES(exec_stack.getCurrentTop())->value.intval;
  uint size, used[ps_image_max_planes];
  XWPSString plane_data[ps_image_max_planes];
  const uchar *wanted;
  int code;

  if (!op->hasTypeAttrs(XWPSRef::String, PS_A_READ)) 
  {
		code = checkOp(op, 1);
		if (code < 0)
  		return code;
  		
		exec_stack.setCurrentTop(zimagePopEStack(exec_stack.getCurrentTop()));
		imageCleanup();
		return (int)(!op->hasType(XWPSRef::String) ? XWPSError::TypeCheck : XWPSError::InvalidAccess);
  }
  size = op->size();
  if (size == 0 && ETOP_SOURCE(exec_stack.getCurrentTop(), 0)[1].value.intval == 0)
		code = 1;
  else 
  {
		plane_data[px].data = op->getBytes();
		plane_data[px].size = size;
		code = penum->imageNextPlanes(plane_data, used);
		if (code == XWPSError::RemapColor) 
		{
	    op->setBytesPtr(op->getBytes() + used[px]);
	    op->decSize(used[px]);
	    ETOP_SOURCE(exec_stack.getCurrentTop(), 0)[1].value.intval = 0;
	    return code;
		}
  }
  
  if (code) 
  {	
		exec_stack.setCurrentTop(zimagePopEStack(exec_stack.getCurrentTop()));
		pop(1);
		imageCleanup();
		return (code < 0 ? code : PS_O_POP_ESTACK);
  }
  pop(1);
  wanted = penum->imagePlanesWanted();
  do 
  {
		if (++px == num_sources)
	    px = 0;
  } while (!wanted[px]);
  ETOP_PLANE_INDEX(exec_stack.getCurrentTop())->value.intval = px;
  return imageProcProcess();
}

int XWPSContextState::imageProcProcess()
{
	int px = ETOP_PLANE_INDEX(exec_stack.getCurrentTop())->value.intval;
  XWPSImageEnums *penum = (XWPSImageEnums*)(exec_stack.getCurrentTop()->getStruct());
  const uchar *wanted = penum->imagePlanesWanted();
  int num_sources = ETOP_NUM_SOURCES(exec_stack.getCurrentTop())->value.intval;
  XWPSRef *pp;

  ETOP_SOURCE(exec_stack.getCurrentTop(), 0)[1].value.intval = 0;
  while (!wanted[px]) 
  {
		if (++px == num_sources)
	    px = 0;
		ETOP_PLANE_INDEX(exec_stack.getCurrentTop())->value.intval = px;
  }
  pp = ETOP_SOURCE(exec_stack.getCurrentTop(), px);
  XWPSRef * esp = exec_stack.incCurrentTop(1);
  esp->makeOper(0, &XWPSContextState::imageProcContinue);
  esp = exec_stack.incCurrentTop(1);
  esp->assign(pp);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::imageSetup(XWPSRef * op, 
	               XWPSImage1 * pim,
	               XWPSColorSpace * pcs, 
	               int npop)
{
	int code;

  code = op[-4].checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  op[-3].checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if (op[-4].value.intval < 0 || op[-3].value.intval < 0)
		return (int)(XWPSError::RangeCheck);
  
  if ((code = op[-1].readMatrix(this, &pim->ImageMatrix)) < 0)
		return code;
    
  pim->ColorSpace = pcs;
  pim->Width = (int)op[-4].value.intval;
  pim->Height = (int)op[-3].value.intval;
  return zimageSetup(pim, op,	pim->ImageMask | pim->CombineWithColor, npop);
}

int XWPSContextState::imageStringContinue()
{
  XWPSImageEnums *penum = (XWPSImageEnums*)(exec_stack.getCurrentTop()->getStruct());
  int num_sources = ETOP_NUM_SOURCES(exec_stack.getCurrentTop())->value.intval;
  XWPSString sources[ps_image_max_planes];
  uint used[ps_image_max_planes];

  memset(sources, 0, sizeof(sources[0]) * num_sources);
  for (;;) 
  {
		int px;
		int code = penum->imageNextPlanes(sources, used);

		if (code == XWPSError::RemapColor)
	    return code;

stop_now:
		if (code) 
		{	
	    exec_stack.incCurrentTop(-NUM_PUSH(num_sources));
	    imageCleanup();
	    return (code < 0 ? code : PS_O_POP_ESTACK);
		}
		for (px = 0; px < num_sources; ++px)
	    if (sources[px].size == 0) 
	    {
				XWPSRef *psrc = ETOP_SOURCE(exec_stack.getCurrentTop(), px);
				uint size = psrc->size();

				if (size == 0) 
				{
		    	code = 1;
		    	goto stop_now;
        }
				sources[px].data = psrc->getBytes();
				sources[px].size = size;
	   	}
  }
}

int XWPSContextState::pixelImageParams(XWPSRef *op, 
	                     XWPSPixelImage *pim,
		                   XWPSImageParams *pip, 
		                   int max_bits_per_component)
{
	int num_components =	pgs->currentColorSpace()->numComponents();
  int code;

  if (num_components < 1)
		return (int)(XWPSError::RangeCheck);
	
  pim->ColorSpace = pgs->currentColorSpace();
  code = dataImageParams(op, pim, pip, true,  num_components, max_bits_per_component);
  if (code < 0)
		return code;
    
  pim->format =(pip->MultipleDataSources ? ps_image_format_component_planar : ps_image_format_chunky);
  return op->dictBoolParam(this, "CombineWithColor", false,  &pim->CombineWithColor);
}

int XWPSContextState::processNonSourceImage(XWPSImageCommon * pic)
{
	XWPSImageEnumCommon *pie;
  int code = pic->beginTyped(pgs, false , &pie);
  return code;
}

int XWPSContextState::zimage()
{
	return zimageOpaqueSetup(op_stack.getCurrentTop(), false, ps_image_alpha_none, pgs->currentDeviceGraySpace(), 5);
}

int XWPSContextState::zimageDataSetup(const XWPSPixelImage * pim,
		                  XWPSImageEnumCommon * pie, 
		                  XWPSRef * sources, 
		                  int npop)
{
	int num_sources = pie->num_planes;
  int inumpush = NUM_PUSH(num_sources);
  int code;
  XWPSImageEnums *penum;
  int px;
  XWPSRef *pp;

  code = checkEStack(inumpush + 2);
  if (code < 0)
  	return code;

  EBOT_NUM_SOURCES(exec_stack.getCurrentTop())->makeInt(num_sources);
  
  for (px = 0, pp = sources; px < num_sources; px++, pp++) 
  {
		XWPSRef * ep = EBOT_SOURCE(exec_stack.getCurrentTop(), px);

		ep[1].makeInt(1);
		switch (pp->type()) 
		{
	    case XWPSRef::File:
				if (!level2Enabled())
		    	return pp->checkTypeFailed();
				{
		    	int pi;

		    	for (pi = 0; pi < px; ++pi)
						if (sources[pi].getStream() == pp->getStream()) 
						{
			    		ep[1].makeInt(-pi);
			    		EBOT_SOURCE(exec_stack.getCurrentTop(), pi)[1].value.intval++;
			    		break;
						}
				}
				
	    case XWPSRef::String:
				if (pp->type() != sources->type())
		    	return pp->checkTypeFailed();
				code = pp->checkRead();
				if (code < 0)
  				return code;
				break;
				
	    default:
				if (!sources->isProc())
		    	return pp->checkTypeFailed();
				code = pp->checkProc();
				if (code < 0)
  				return code;
		}
		ep->assign(pp);
  }
  penum = new XWPSImageEnums;
  code = penum->imageEnumInit(pie, pim, pgs);
  if (code != 0) 
  {
		if (code >= 0)
	    pop(npop);
		return code;
  }
  
  XWPSRef * esp = exec_stack.incCurrentTop(1);
  esp->makeMarkEStack(PS_ES_OTHER, &XWPSContextState::imageCleanup);
  esp = exec_stack.incCurrentTop(inumpush - 1);
  ETOP_PLANE_INDEX(esp)->makeInt(0);
  ETOP_NUM_SOURCES(esp)->makeInt(num_sources);
  exec_stack.getCurrentTop()->makeStruct(currentSpace() | 0, penum);
  switch (sources->type()) 
  {
		case XWPSRef::File:
			esp = exec_stack.incCurrentTop(1);
			esp->makeOper(0, &XWPSContextState::imageFileContinue);
	    break;
	    
		case XWPSRef::String:
			esp = exec_stack.incCurrentTop(1);
	    esp->makeOper(0, &XWPSContextState::imageStringContinue);
	    break;
	    
		default:
			esp = exec_stack.incCurrentTop(1);
	    esp->makeOper(0, &XWPSContextState::imageProcProcess);
	    break;
  }
  pop(npop);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zimageMask()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSImage1 image;

  int code = op[-2].checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  image.initMaskAdjust(op[-2].value.boolval, pgs->inCacheDevice() != CACHE_DEVICE_NONE);
  return imageSetup(op, &image, NULL, 5);
}

int XWPSContextState::zimageMask1()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSImage1 image;
  XWPSImageParams ip;
  int code;

  image.initMaskAdjust(false, pgs->inCacheDevice() != CACHE_DEVICE_NONE);
  code = dataImageParams(op, &image, &ip, true, 1, 1);
  if (code < 0)
		return code;
  if (ip.MultipleDataSources)
		return (int)(XWPSError::RangeCheck);
			
  return zimageSetup(&image, &ip.DataSource[0],true, 1);
}

int XWPSContextState::zimageMultiple(bool has_alpha)
{
	XWPSRef * op = op_stack.getCurrentTop();
  int spp;
  int npop = 7;
  XWPSRef * procp = op - 2;
  XWPSColorSpace *pcs;
  bool multi = false;

  int code = op->checkIntLEU(4);
  if (code < 0)
  	return code;
  	
  op[-1].checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  switch ((spp = (int)(op->value.intval))) 
  {
		case 1:
	    pcs = pgs->currentDeviceGraySpace();
	    break;
	    
		case 3:
	    pcs = pgs->currentDeviceRGBSpace();
	    goto color;
	    
		case 4:
	    pcs = pgs->currentDeviceCMYKSpace();
color:
	    if (op[-1].value.boolval) 
	    {
				if (has_alpha)
		    	++spp;
				npop += spp - 1;
				procp -= spp - 1;
				multi = true;
	    }
	    break;
	    
		default:
	    return (int)(XWPSError::RangeCheck);
  }
  return zimageOpaqueSetup(procp, multi, (has_alpha ? ps_image_alpha_last : ps_image_alpha_none), pcs, npop);
}

int XWPSContextState::zimageOpaqueSetup(XWPSRef * op, 
	                      bool multi,
		                    PSImageAlpha alpha, 
		                    XWPSColorSpace * pcs,
		                    int npop)
{
	XWPSImage1 image;

  int code = op[-2].checkIntLEU((level2Enabled() ? 12 : 8));
  if (code < 0)
  	return code;
  image.initAdjust(pcs, true);
  image.BitsPerComponent = (int)op[-2].value.intval;
  image.Alpha = alpha;
  image.format =(multi ? ps_image_format_component_planar : ps_image_format_chunky);
  return imageSetup(op, &image, pcs, npop);
}

XWPSRef * XWPSContextState::zimagePopEStack(XWPSRef * tep)
{
	return tep - NUM_PUSH(ETOP_NUM_SOURCES(tep)->value.intval);
}

int XWPSContextState::zimageSetup(XWPSPixelImage * pim,
	                XWPSRef * sources, 
	                bool uses_color, 
	                int npop)
{
	XWPSImageEnumCommon *pie;
  int code =	pim->beginTyped(pgs, uses_color, &pie);

  if (code < 0)
		return code;
  
  return zimageDataSetup(pim, pie, sources, npop);
}

int XWPSContextState::zimage1()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSImage1 image;
  XWPSImageParams ip;
  int code;

  image.initAdjust(pgs->currentColorSpace(), true);
  code = pixelImageParams(op, &image, &ip, 12);
  if (code < 0)
		return code;
  return zimageSetup(&image, &ip.DataSource[0],	image.CombineWithColor, 1);
}

int XWPSContextState::zimage3()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSImage3 image;
  int interleave_type;
  XWPSRef *pDataDict;
  XWPSRef *pMaskDict;
  XWPSImageParams ip_data, ip_mask;
  int ignored;
  int code, mcode;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
		return code;
		
  code = op->checkDictRead();
  if (code < 0)
		return code;
		
  if ((code = op->dictIntParam(this, "InterleaveType", 1, 3, -1,  &interleave_type)) < 0)
		return code;
  
  image.image3Init(NULL, (PSImage3InterleaveType)interleave_type);
  if (op->dictFindString(this, "DataDict", &pDataDict) <= 0 ||
			op->dictFindString(this, "MaskDict", &pMaskDict) <= 0)
	{
		return (int)(XWPSError::RangeCheck);
	}
    
  if ((code = pixelImageParams(pDataDict, &image, &ip_data, 12)) < 0 ||
			(mcode = code = dataImageParams(pMaskDict, &image.MaskDict, &ip_mask, false, 1, 12)) < 0 ||
			(code = pDataDict->dictIntParam(this, "ImageType", 1, 1, 0, &ignored)) < 0 ||
			(code = pMaskDict->dictIntParam(this, "ImageType", 1, 1, 0, &ignored)) < 0)
		return code;
		
  if ((ip_data.MultipleDataSources && interleave_type != 3) ||
			ip_mask.MultipleDataSources || mcode != (image.InterleaveType != 3))
	{
		return (int)(XWPSError::RangeCheck);
	}
  
  if (image.InterleaveType == 3) 
  {
		memmoveRef(&ip_data.DataSource[1], &ip_data.DataSource[0],6);
		ip_data.DataSource[0].assign(&ip_mask.DataSource[0]);
  }
  return zimageSetup(&image,&ip_data.DataSource[0],	image.CombineWithColor, 1);
}

int XWPSContextState::zimage4()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSImage4 image;
  XWPSImageParams ip;
  int num_components = pgs->currentColorSpace()->numComponents();
  int colors[PS_IMAGE_MAX_COMPONENTS * 2];
  int code;
  int i;

  image.image4Init(NULL);
  code = pixelImageParams(op, &image, &ip, 12);
  if (code < 0)
		return code;
  
  code = op->dictIntArrayCheckParam(this, "MaskColor", num_components * 2, colors, 0, XWPSError::RangeCheck);
  if (code == num_components) 
  {
		image.MaskColor_is_range = false;
		for (i = 0; i < code; ++i)
	    image.MaskColor[i] = (colors[i] < 0 ? ~(uint)0 : colors[i]);
  }
  else if (code == num_components * 2) 
  {
		image.MaskColor_is_range = true;
		for (i = 0; i < code; i += 2) 
		{
	    if (colors[i+1] < 0)
				image.MaskColor[i] = 1, image.MaskColor[i+1] = 0;
	    else 
	    {
				image.MaskColor[i+1] = colors[i+1];
				image.MaskColor[i] = qMax(colors[i], 0);
	    }
		}
  } 
  else
		return (code < 0 ? code : (int)(XWPSError::RangeCheck));
  return zimageSetup(&image, &ip.DataSource[0],	image.CombineWithColor, 1);
}
