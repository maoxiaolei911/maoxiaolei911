/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSHalfTone.h"
#include "XWPSPath.h"
#include "XWPSState.h"
#include "XWPSContextState.h"

int XWPSContextState::dictRealResult(XWPSRef * pdict, const char *kstr, float val)
{
	int code = 0;
  XWPSRef *ignore;

  if (pdict->dictFindString(this, kstr, &ignore) > 0) 
  {
		XWPSRef rval;

		code = pdict->checkDictWrite();
		rval.makeReal(val);
		code = dictPutString(pdict, kstr, &rval);
  }
  return code;
}

int XWPSContextState::dictSpotParams(XWPSRef * pdict, 
	                   XWPSSpotHalftone * psp,
		                 XWPSRef * psproc, 
		                 XWPSRef * ptproc)
{
	int code;

  code = pdict->checkDictRead();
  if (code < 0)
  	return code;
  	
  if ((code = pdict->dictFloatParam(this, "Frequency", 0.0, &psp->screen.frequency)) != 0 ||
			(code = pdict->dictFloatParam(this, "Angle", 0.0, &psp->screen.angle)) != 0 ||
      (code = pdict->dictProcParam(this, "SpotFunction", psproc, false)) != 0 ||
			(code = pdict->dictBoolParam(this, "AccurateScreens", pgs->currentAccurateScreens(),
						&psp->accurate_screens)) < 0 ||
      (code = pdict->dictProcParam(this, "TransferFunction", ptproc, false)) < 0)
		return (code < 0 ? code : XWPSError::Undefined);
  psp->transfer.setType((code > 0 ? ps_tm_null : ps_tm_mapped));
  return 0;
}

int XWPSContextState::dictSpotResults(XWPSRef * pdict, const XWPSSpotHalftone * psp)
{
	int code;

  code = dictRealResult(pdict, "ActualFrequency",  psp->screen.actual_frequency);
  if (code < 0)
		return code;
  return dictRealResult(pdict, "ActualAngle", psp->screen.actual_angle);
}

int XWPSContextState::dictThresholdCommonParams(XWPSRef * pdict,
			                          XWPSThresholdHalftoneCommon * ptp,
			                          XWPSRef **pptstring, 
			                          XWPSRef *ptproc)
{
	int code;

  code = pdict->checkDictRead();
  if ((code = pdict->dictIntParam(this, "Width", 1, 0x7fff, -1, &ptp->width)) < 0 ||
			(code = pdict->dictIntParam(this, "Height", 1, 0x7fff, -1,&ptp->height)) < 0 ||
			(code = pdict->dictFindString(this, "Thresholds", pptstring)) <= 0 ||
      (code = pdict->dictProcParam(this, "TransferFunction", ptproc, false)) < 0)
		return (code < 0 ? code : XWPSError::Undefined);
  return code;
}

int XWPSContextState::dictThresholdParams(XWPSRef * pdict, 
	                        XWPSThresholdHalftone * ptp,
		                      XWPSRef * ptproc)
{
	XWPSRef *tstring;
  int code =	dictThresholdCommonParams(pdict, ptp, &tstring, ptproc);

  if (code < 0)
		return code;
    
  code = tstring->checkReadTypeOnly(XWPSRef::String);
  if (tstring->size() != (long)ptp->width * ptp->height)
	 	return (int)(XWPSError::RangeCheck);
  
  ptp->thresholds.size = tstring->size();
  ptp->thresholds.data = tstring->getBytes();
  ptp->thresholds.bytes = tstring->value.bytes->arr;
  if (ptp->thresholds.bytes)
  	ptp->thresholds.bytes->incRef();
  ptp->transfer.setType((code > 0 ? ps_tm_null : ps_tm_mapped));
  return 0;
}

int XWPSContextState::dictThreshold2Params(XWPSRef * pdict, 
	                         XWPSThresholdHalftone2 * ptp,
		                       XWPSRef * ptproc)
{
	XWPSRef *tstring;
  int code =	dictThresholdCommonParams(pdict, ptp, &tstring, ptproc);
  int bps;
  uint size;
  int cw2, ch2;

  if (code < 0 ||
			(code = cw2 = pdict->dictIntParam(this, "Width2", 0, 0x7fff, 0, &ptp->width2)) < 0 ||
			(code = ch2 = pdict->dictIntParam(this, "Height2", 0, 0x7fff, 0,&ptp->height2)) < 0 ||
			(code = pdict->dictIntParam(this, "BitsPerSample", 8, 16, -1, &bps)) < 0)
		return code;
  if ((bps != 8 && bps != 16) || cw2 != ch2 ||(!cw2 && (ptp->width2 == 0 || ptp->height2 == 0)))
  {
	 	return (int)(XWPSError::RangeCheck);
	}
	
  ptp->bytes_per_sample = bps / 8;
  switch (tstring->type()) 
  {
    case XWPSRef::String:
			size = tstring->size();
			ptp->thresholds.size = size;
			ptp->thresholds.data = tstring->getBytes();
			ptp->thresholds.bytes = tstring->value.bytes->arr;
			if (ptp->thresholds.bytes)
				ptp->thresholds.bytes->incRef();
			break;
			
		case XWPSRef::AStruct:
			{
				const char * tname = tstring->getTypeName();
				if (strcmp(tname, "bytes"))
					return (int)(XWPSError::TypeCheck);
						
				XWPSBytes * b = (XWPSBytes*)(tstring->value.pstruct);
				ptp->thresholds.size = b->length;
				ptp->thresholds.data = b->arr;
				ptp->thresholds.bytes = b;
				b->incRef();
			}
			
    default:
			return (int)(XWPSError::RangeCheck);
  }
  code = tstring->checkRead();
  if (size != (ptp->width * ptp->height + ptp->width2 * ptp->height2) *	ptp->bytes_per_sample)
  {
	 	return (int)(XWPSError::RangeCheck);
	}
  return 0;
}

int XWPSContextState::screenCleanup()
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	esp[4].makeNull();
	return 0;
}

int XWPSContextState::screenSample()
{
	XWPSRef * op = op_stack.getCurrentTop();
	XWPSRef * esp = exec_stack.getCurrentTop();
  XWPSScreenEnum *penum = (XWPSScreenEnum*)(esp->getStruct());
  XWPSPoint pt;
  int code = penum->currentPoint(&pt);
  XWPSRef proc;

  switch (code) 
  {
		default:
	    return code;
	    
		case 1:
			esp = exec_stack.getCurrentTop();
	    if (esp[-2].getOper() != 0)
				code = esp[-2].callProc(this);
	    esp = exec_stack.incCurrentTop(-4);
	    screenCleanup();
	    return (code < 0 ? code : PS_O_POP_ESTACK);
	    	
		case 0:
	    ;
  }
  code = push(&op, 2);
  if (code < 0)
		return code;
  op[-1].makeReal(pt.x);
  op->makeReal(pt.y);
  esp = exec_stack.getCurrentTop();
  proc.assign(esp-1);
  esp = exec_stack.incCurrentTop(1);
  esp->makeOper(0, &XWPSContextState::setScreenContinue);
  esp = exec_stack.incCurrentTop(1);
  esp->assign(&proc);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::setColorScreenCleanup()
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	esp[6].makeNull();
	esp[7].makeNull();
	return 0;
}

int XWPSContextState::setColorScreenFinish()
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	XWPSDeviceHalftone *pdht = (XWPSDeviceHalftone*)(esp->getStruct());
  int code;

  pdht->order = pdht->components[0].corder;
  esp = exec_stack.getCurrentTop();
  code = pgs->install((XWPSHalftone*)(esp[-1].getStruct()), pdht);
  if (code < 0)
		return code;
    
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  XWPSRef * d = &(iigs->screen_procs.indexed[0]);
  esp = exec_stack.getCurrentTop();
  memcpyRef(d, esp - 5, 4);
  iigs->halftone.makeNull();
  esp = exec_stack.incCurrentTop(-7);
  setColorScreenCleanup();
  return PS_O_POP_ESTACK;
}

int XWPSContextState::setHalftoneCleanup()
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	esp[4].makeNull();
	esp[3].makeNull();
  return 0;
}

int XWPSContextState::setHalftoneFinish()
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	XWPSDeviceHalftone *pdht = (XWPSDeviceHalftone*)(esp->getStruct());
  int code;

  if (pdht->components)
		pdht->order = pdht->components[0].corder;
  code = pgs->install((XWPSHalftone*)(esp[-1].getStruct()), pdht);
  if (code < 0)
		return code;
    
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->halftone.assign(esp-2);
  esp = exec_stack.incCurrentTop(-4);
  setHalftoneCleanup();
  return PS_O_POP_ESTACK;
}

int XWPSContextState::setScreenContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double value;
  int code = op->realParam(&value);

  if (code < 0)
		return code;
    
  XWPSRef * esp = exec_stack.getCurrentTop();
  XWPSScreenEnum *penum = (XWPSScreenEnum*)(esp->getStruct());
  code = penum->next(value);
  if (code < 0)
		return code;
  pop(1);
  return screenSample();
}

int XWPSContextState::setScreenFinish()
{
	XWPSRef * esp = exec_stack.getCurrentTop();
  XWPSScreenEnum *penum = (XWPSScreenEnum*)(esp->getStruct());
	penum->install();
	
	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->screen_procs.colored.red.assign(esp-1);
  iigs->screen_procs.colored.green.assign(esp-1);
  iigs->screen_procs.colored.blue.assign(esp-1);
  iigs->screen_procs.colored.gray.assign(esp-1);
  iigs->halftone.makeNull();
  return 0;
}

int XWPSContextState::zcurrentHalftone()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSHalftone *ht;
  int code;

  pgs->currentHalftone(&ht);
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  switch (ht->type) 
  {
		case ht_type_screen:
	    code = push(&op, 4);
	    if (code < 0)
				return code;
	    op[-3].makeReal(ht->params.screen->frequency);
	    op[-2].makeReal(ht->params.screen->angle);
	    op[-1].assign(&iigs->screen_procs.colored.gray);
	    op->makeInt(1);
	    break;
	    
		case ht_type_colorscreen:
	    code = push(&op, 13);
	    if (code < 0)
				return code;
	    {
				int i;

				for (i = 0; i < 4; i++) 
				{
		    	XWPSRef * opc = op - 12 + i * 3;
		    	XWPSScreenHalftone *pht = ht->params.colorscreen->screens.indexed[i];

		    	opc->makeReal(pht->frequency);
		    	opc[1].makeReal(pht->angle);
		    	opc[2].assign(&iigs->screen_procs.indexed[i]);
				}
	   	}
	    op->makeInt(2);
	    break;
	    
		default:	
	    code = push(&op, 2);
	    if (code < 0)
				return code;
	    op[-1].assign(&iigs->halftone);
	    op->makeInt(0);
	    break;
  }
  return 0;
}

int XWPSContextState::zcurrentScreenLevels()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt(pgs->currentScreenLevels());
  return 0;
}

int XWPSContextState::zcurrentScreenPhase()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSIntPoint phase;
  int code;

  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if (op->value.intval < -1 || op->value.intval >= ps_color_select_count)
	 	return (int)(XWPSError::RangeCheck);
  
  code = pgs->currentScreenPhase(&phase, (PSColorSelect)op->value.intval);
  if (code < 0)
		return code;
    
  code = push(&op, 1);
  if (code < 0)
		return code;
  op[-1].makeInt(phase.x);
  op->makeInt(phase.y);
  return 0;
}

int XWPSContextState::zimage2()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
		return code;
		
  code = op->checkDictRead();
  if (code < 0)
		return code;
		
  {
		XWPSImage2 image;
		XWPSRef *pDataSource;

		if ((code = op->dictMatrixParam(this, "ImageMatrix", &image.ImageMatrix)) < 0 ||
	    	(code = op->dictFindString(this, "DataSource", &pDataSource)) < 0 ||
	    	(code = op->dictFloatParam(this, "XOrigin", 0.0, &image.XOrigin)) != 0 ||
	    	(code = op->dictFloatParam(this, "YOrigin", 0.0, &image.YOrigin)) != 0 ||
	    	(code = op->dictFloatParam(this, "Width", 0.0,  &image.Width)) != 0 || image.Width <= 0 ||
	    	(code = op->dictFloatParam(this, "Height", 0.0, &image.Height)) != 0 || image.Height <= 0 ||
	    	(code = op->dictBoolParam(this, "PixelCopy", false, &image.PixelCopy)) < 0 )
	    return (code < 0 ? code : (int)(XWPSError::RangeCheck));
		
		code = pDataSource->checkType("igstateobj");
		if (code < 0)
			return code;
			
		XWPSIGStateObj * igs = (XWPSIGStateObj*)(pDataSource->getStruct());			
		image.DataSource = (XWPSState*)(igs->gstate.getStruct());
		{
	    XWPSRef *ignoref;

	    if (op->dictFindString(this, "UnpaintedPath", &ignoref) > 0) 
	    {
				code = op->checkDictWrite();
				if (code < 0)
					return code;
				image.UnpaintedPath = new XWPSPath;
	    }
		}
		code = processNonSourceImage(&image);
		if (image.UnpaintedPath) 
		{
	    XWPSRef rupath;

	    if (code < 0)
				return code;
	    if (image.UnpaintedPath->isNull())
				rupath.makeNull();
	    else
				code = makeUPath(&rupath, pgs, image.UnpaintedPath,  false);
	    if (code < 0)
				return code;
	    code = dictPutString(op, "UnpaintedPath", &rupath);
		}
  }
  if (code >= 0)
		pop(1);
  return code;
}

int XWPSContextState::zscreenEnumInit(XWPSHTOrder * porder,
		                   XWPSScreenHalftone * psp, 
		                   XWPSRef * pproc, 
		                   int npop,
		                   op_proc_t finish_proc)
{
	XWPSScreenEnum *penum;
  int code;

  code = checkEStack(5);
  if (code < 0)
  	return code;
  penum = new XWPSScreenEnum;
  XWPSRef * esp = exec_stack.getCurrentTop();
  esp[4].makeStruct(currentSpace() | 0, penum);
  code = penum->initMemory(porder, pgs, psp);
  if (code < 0) 
  {
		screenCleanup();
		return code;
  }
  
  esp[1].makeMarkEStack(PS_ES_OTHER, &XWPSContextState::screenCleanup);
  esp = exec_stack.incCurrentTop(4);
  esp[-2].makeOper(0, finish_proc);
  esp[-1].assign(pproc);
  esp = exec_stack.incCurrentTop(1);
  esp->makeOper(0, &XWPSContextState::screenSample);
  pop(npop);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zscreenParams(XWPSRef * op, XWPSScreenHalftone * phs)
{
	double fa[2];
  int code = numParams(op - 1,2, fa);

  if (code < 0)
		return code;
  
  code = op->checkProc();
  if (code < 0)
		return code;
		
  phs->frequency = fa[0];
  phs->angle = fa[1];
  return 0;
}

int XWPSContextState::zsetColorScreen()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSColorScreenHalftone cscreen;
  XWPSRef sprocs[4];
  XWPSHalftone *pht;
  XWPSDeviceHalftone *pdht;
  int i;
  int code = 0;
//  int space = 0;

  for (i = 0; i < 4; i++) 
  {
		XWPSRef * op1 = op - 9 + i * 3;
		int code = zscreenParams(op1, cscreen.screens.indexed[i]);

		if (code < 0)
	    return code;
		cscreen.screens.indexed[i]->proc.spot_function_.spot_function = &XWPSScreenHalftone::spotDummy;
		sprocs[i].assign(op1);
  }
  
  code = checkEStack(8);	
  if (code < 0)
	  return code;
	  
	pht = new XWPSHalftone;
	pdht = new XWPSDeviceHalftone;
	pht->type = ht_type_colorscreen;
	pht->params.colorscreen = new XWPSColorScreenHalftone(&cscreen);
	code = pgs->setHalftonePrepare(pht, pdht);
		
  if (code >= 0) 
  {	
  	XWPSRef * esp = exec_stack.getCurrentTop();
		XWPSRef * esp0 = esp;	

		esp = exec_stack.incCurrentTop(8);
		esp[-7].makeMarkEStack(PS_ES_OTHER, &XWPSContextState::setColorScreenCleanup);
		memcpyRef(esp - 6, sprocs, 4);
		
		esp[-2].makeStruct(currentSpace() | 0, pht);
		esp[-1].makeStruct(currentSpace() | 0, pdht);
		esp->makeOper(0, &XWPSContextState::setColorScreenFinish);
		for (i = 0; i < 4; i++) 
		{
	    code = zscreenEnumInit(&pdht->components[(i + 1) & 3].corder, pht->params.colorscreen->screens.indexed[i], &sprocs[i], 0, 0);
	    if (code < 0) 
	    {
				exec_stack.setCurrentTop(esp0);
				break;
	    }
		}
		
		if (code < 0) 
			return code;
  }
  else
  	return code;
  pop(12);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zsetHalftone5()
{
	XWPSRef * op = op_stack.getCurrentTop();
	XWPSRef * esp = exec_stack.getCurrentTop();
  uint count;
  XWPSHalftoneComponent *phtc;
  XWPSHalftoneComponent *pc;
  int code = 0;
  int i, j;
  XWPSHalftone *pht;
  XWPSDeviceHalftone *pdht;
  static const char *const color_names[] = {"Default", "Gray", "Red", "Green", "Blue",
    "Cyan", "Magenta", "Yellow", "Black"};
  XWPSRef sprocs[sizeof(color_names) /sizeof(color_names[0])];
  XWPSRef tprocs[sizeof(color_names) /sizeof(color_names[0])];
  uint edepth = exec_stack.count();
  int npop = 2;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0) 
		return code;
    
  code = op->checkDictRead();
  if (code < 0) 
		return code;
		
  code = op[-1].checkType(XWPSRef::Dictionary);
  if (code < 0) 
		return code;
		
  code = op[-1].checkDictRead();
  if (code < 0) 
		return code;
		
  count = 0;
  for (i = 0; i < sizeof(color_names) /sizeof(color_names[0]); i++) 
  {
		XWPSRef *pvalue;

		if (op->dictFindString(this, color_names[i], &pvalue) > 0)
	    count++;
		else if (i == ps_ht_separation_Default)
	  	return (int)(XWPSError::RangeCheck);
  }
  
  code = checkEStack(5);
  if (code < 0) 
		return code;
		
	pht = new XWPSHalftone;
  phtc = 0;
  if (count > 0)
    phtc = new XWPSHalftoneComponent[count];
  pdht = new XWPSDeviceHalftone;
	for (i = 0, j = 0, pc = phtc; i < sizeof(color_names) /sizeof(color_names[0]); i++) 
	{
	    int type;
	    XWPSRef *pvalue;

	    if (op->dictFindString(this, color_names[i], &pvalue) > 0) 
	    {
				pvalue->checkTypeOnly(XWPSRef::Dictionary);
				pvalue->checkDictRead();
				if (pvalue->dictIntParam(this, "HalftoneType", 1, 7, 0, &type) < 0) 
				{
		    	code = (int)(XWPSError::RangeCheck);
		    	break;
				}
				
				switch (type) 
				{
		    	default:
						code = (int)(XWPSError::RangeCheck);
						break;
						
		    	case 1:
		    		pc->setType(ht_type_spot);
						code = dictSpotParams(pvalue, pc->params.spot, sprocs + j,	tprocs + j);
						pc->params.spot->screen.proc.spot_function_.spot_function = &XWPSScreenHalftone::spotDummy;
						break;
						
		    	case 3:
		    		pc->setType(ht_type_threshold);
						code = dictThresholdParams(pvalue, pc->params.threshold, tprocs + j);
						break;
					
		    	case 7:
		    		pc->setType(ht_type_threshold2);
						code = dictThreshold2Params(pvalue,	pc->params.threshold2, tprocs + j);
						break;
				}
				if (code < 0)
		    	break;
				pc->cname = (PSHTSeparationName) i;
				pc++, j++;
	   }
	}
  if (code >= 0) 
  {
		int type = 0;

		op[-1].dictIntParam(this, "HalftoneType", 1, 5, 0, &type);
		pht->setType((type == 2 || type == 4 ? ht_type_multiple_colorscreen :  ht_type_multiple));
		pht->params.multiple->components = phtc;
		pht->params.multiple->num_comp = count;
		code = pgs->setHalftonePrepare(pht, pdht);
  }  
  else
  {
  	if (phtc)
  	  delete [] phtc;
  }
  
  if (code >= 0)
		for (j = 0, pc = phtc; j < count; j++, pc++) 
		{
	   	if (pc->type == ht_type_spot) 
	   	{
				XWPSRef *pvalue;
				op->dictFindString(this, color_names[pc->cname], &pvalue);
				code = dictSpotResults(pvalue, pc->params.spot);
				if (code < 0)
	    		break;
	   	}
		}
  if (code >= 0) 
  {
		uint odepth = op_stack.count();
		XWPSRef odict, odict5;

		odict.assign(&op[-1]);
		odict5.assign(op);
		pop(2);
		op = op_stack.getCurrentTop();
		esp = exec_stack.incCurrentTop(5);
		esp[-4].makeMarkEStack(PS_ES_OTHER, &XWPSContextState::setHalftoneCleanup);
		esp[-3].assign(&odict);
		esp[-2].makeStruct(currentSpace() | 0, pht);
		esp[-1].makeStruct(currentSpace() | 0, pdht);
		esp->makeOper(0, &XWPSContextState::setHalftoneFinish);
		for (j = 0; j < count; j++) 
		{
	   	XWPSHTOrder *porder = (pdht->components == 0 ? &pdht->order :  &pdht->components[j].corder);

	   	switch (phtc[j].type) 
	   	{
				case ht_type_spot:
	    		code = zscreenEnumInit(porder, &phtc[j].params.spot->screen,  &sprocs[j], 0, 0);
	    		if (code < 0)
						break;
							
				case ht_type_threshold:
	    		if (!tprocs[j].hasType(XWPSRef::Invalid)) 
	    		{
						checkOStack(zcolor_remap_one_ostack);
						checkEStack(zcolor_remap_one_estack);
						code = zcolorRemapOne(tprocs + j, porder->transfer, pgs, &XWPSContextState::zcolorRemapOneFinish);
						op = op_stack.getCurrentTop();
	    		}
		    	break;
		    		
				default:	
		    		;
	    }
	    if (code < 0) 
	    {
				op_stack.pop(op_stack.count() - odepth);
				exec_stack.pop(exec_stack.count() - edepth);
				op = op_stack.getCurrentTop();
				op[-1].assign(&odict);
				op->assign(&odict5);
				break;
	    }
	    npop = 0;
		}
  }
  
  if (code < 0) 
		return code;
		
  pop(npop);
  return (exec_stack.count() > edepth ? PS_O_PUSH_ESTACK : 0);
}

int XWPSContextState::zsetScreen()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSScreenHalftone screen;
  XWPSHTOrder order;
  int code = zscreenParams(op, &screen);

  if (code < 0)
		return code;
		
  code = order.screenOrderInitMemory(pgs, &screen, pgs->currentAccurateScreens());
  if (code < 0)
		return code;
    
  return zscreenEnumInit(&order, &screen, op, 3, &XWPSContextState::setScreenFinish);
}

int XWPSContextState::zsetScreenPhase()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  long x, y;

  code = op[-2].checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  code = op[-1].checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  x = op[-1].value.intval;
  y = op->value.intval;
  if (x != (int)x || y != (int)y || op[-2].value.intval < -1 || op[-2].value.intval >= ps_color_select_count)
  {
	 	return (int)(XWPSError::RangeCheck);
	}
	
  code = pgs->setScreenPhase((int)x, (int)y, (PSColorSelect) op[-2].value.intval);
  if (code >= 0)
		pop(3);
  return code;
}
