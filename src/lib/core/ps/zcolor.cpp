/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSParam.h"
#include "XWPSDevice.h"
#include "XWPSCie.h"
#include "XWPSFunction.h"
#include "XWPSColor.h"
#include "XWPSComposite.h"
#include "XWPSPattern.h"
#include "XWPSTransferMap.h"
#include "XWPSDevicePatternAccum.h"
#include "XWPSState.h"
#include "XWPSContextState.h"

int XWPSContextState::beginComposite(XWPSAlphaCompositeState * pcp)
{
	XWPSDevice *dev = pgs->currentDevice();
	pcp->pcte = new  XWPSCompositeAlpha(&pcp->params);
	pcp->pcte->id = nextIDS(1);
  pcp->orig_dev = pcp->cdev = dev;
  int code = dev->createCompositor(&pcp->cdev, pcp->pcte, pgs);
  if (code < 0) 
  {
		endComposite(pcp);
		return code;
  }
  
  pgs->setDeviceNoInit(pcp->cdev);
  return 0;
}

int XWPSContextState::cacheAbcCommon(XWPSCieABC * pcie,
		                 XWPSRefCieProcs * pcprocs,
		                 void *)
{
	int code =	ciePrepareCaches4(&pcie->RangeABC.ranges[0],
			   				pcprocs->ABC.getArray(),
			   	      &pcie->caches.DecodeABC[0].floats,
			   	       &pcie->caches.DecodeABC[1].floats, 
			   	      &pcie->caches.DecodeABC[2].floats, NULL,pcie);

  return (code < 0 ? code : cacheCommon(pcie, pcprocs, pcie));
}

int XWPSContextState::cacheColorRendering1(XWPSCieRender * pcrd,
		      								XWPSRefCieRenderProcs * pcrprocs)
{
	XWPSRef * ep = exec_stack.getCurrentTop();
  int code = pcrd->init();
  int i;

  if (code < 0 ||
			(code = cieCachePushFinish(&XWPSContextState::cieCacheRenderFinish, pcrd)) < 0 ||
			(code = ciePrepareCaches4(&pcrd->DomainLMN.ranges[0], 
			                          pcrprocs->EncodeLMN.getArray(), 
			                          &pcrd->caches.EncodeLMN[0].floats,
			                          &pcrd->caches.EncodeLMN[1].floats,
			                          &pcrd->caches.EncodeLMN[2].floats,
			                          NULL, pcrd)) < 0 ||
			(code = ciePrepareCaches4(&pcrd->DomainABC.ranges[0], 
			                          pcrprocs->EncodeABC.getArray(), 
			                          &pcrd->caches.EncodeABC[0].floats,
			                          &pcrd->caches.EncodeABC[1].floats,
			                          &pcrd->caches.EncodeABC[2].floats,
			                          NULL, pcrd)) < 0) 
	{
		exec_stack.setCurrentTop(ep);
		return code;
  }
  if (pcrd->RenderTable.lookup.table != 0) 
  {
		bool is_identity = true;

		for (i = 0; i < pcrd->RenderTable.lookup.m; i++)
		{
			XWPSRef * p = pcrprocs->RenderTableT.getArray() + i;
	    if (p->size() != 0) 
	    {
				is_identity = false;
				break;
	    }
	  }
		pcrd->caches.RenderTableT_is_identity = is_identity;
		if (!is_identity)
	    for (i = 0; i < pcrd->RenderTable.lookup.m; i++)
				if ((code = ciePrepareCache((PSRange*)&Range4_default.ranges[0],
							pcrprocs->RenderTableT.getArray() + i,
				       &pcrd->caches.RenderTableT[i].floats, pcrd)) < 0) 
				{
		    	exec_stack.setCurrentTop(ep);
		    	return code;
				}
  }
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::cacheCommon(XWPSCie * pcie,
	                XWPSRefCieProcs * pcprocs,
	                void *container)
{
	return ciePrepareCaches4(&pcie->common.RangeLMN.ranges[0],
			      pcprocs->DecodeLMN.getArray(),
			      &pcie->common.caches.DecodeLMN[0].floats,
			      &pcie->common.caches.DecodeLMN[1].floats, 
			      &pcie->common.caches.DecodeLMN[2].floats, NULL,container);
}

int XWPSContextState::cieAbcParam(XWPSRef * pdref, 
	                 XWPSCieABC * pcie, 
	                 XWPSRefCieProcs * pcprocs)
{
	int code;

  if ((code = dictRange3Param(pdref, "RangeABC", &pcie->RangeABC)) < 0 ||
			(code = dictProc3Param(pdref, "DecodeABC", &pcprocs->ABC)) < 0 ||
			(code = dictMatrix3Param(pdref, "MatrixABC", &pcie->MatrixABC)) < 0 ||
			(code = cieLmnpParam(pdref, pcie, pcprocs)) < 0)
		return code;
  pcie->DecodeABC = DecodeABC_default;
  return 0;
}

int XWPSContextState::cieCacheJoint(XWPSRefCieRenderProcs * pcrprocs,
		                XWPSCie *pcie, 
		                XWPSState * pgsA)
{
	XWPSCieRender *pcrd = pgsA->currentColorRendering();
  XWPSCieJointCaches *pjc = pgsA->currentCieCaches();
  XWPSRef pqr_procs;
//  uint space;
  int code;
  int i;

  if (pcrd == 0)
		return 0;
  if (pjc == 0)	
		return (int)(XWPSError::VMError);
			
  if (pcrprocs->TransformPQR.hasType(XWPSRef::Null)) 
		return pgsA->cieCSComplete(true);
		
  pjc->computePointsSD(pcie, pcrd);
  pqr_procs.makeArray(currentSpace() | PS_A_READONLY, 3 * (1 + 4 + 4 * 6));
  code = checkEStack(3);
  if (code < 0)
		return code;
  cieCachePushFinish(&XWPSContextState::cieTpqrFinish, pgsA);
  XWPSRef * esp = exec_stack.incCurrentTop(1);
  esp->assign(&pqr_procs);
  for (i = 0; i < 3; i++) 
  {
		XWPSRef *p = pqr_procs.getArray() + 3 + (4 + 4 * 6) * i;
		const float *ppt = (float *)&pjc->points_sd;
		int j;

		XWPSRef * t = pqr_procs.getArray() + i;
		if (pqr_procs.value.refs->arr)
		{
			pqr_procs.value.refs->arr->incRef();
			pqr_procs.value.refs->arr->incRef();
		}
		t->makeArray(currentSpace() | PS_A_READONLY | PS_A_EXECUTABLE, 4, p);
		t->value.refs->arr = pqr_procs.value.refs->arr;
		p->makeArray(currentSpace() | PS_A_READONLY, 4 * 6, p + 4);
		p->value.refs->arr = pqr_procs.value.refs->arr;
		p[1].assign(pcrprocs->TransformPQR.getArray() + i );
		p[2].makeOper(0, &XWPSContextState::cieExecTpqr);
		p[3].makeOper(0, &XWPSContextState::ciePostExecTpqr);
		for (j = 0, p += 4; j < 4 * 6; j++, p++, ppt++)
	    p->makeReal(*ppt);
  }
  return ciePrepareCaches4(&pcrd->RangePQR.ranges[0],
			      pqr_procs.getArray(),
			      &pjc->TransformPQR[0].floats,
			      &pjc->TransformPQR[1].floats,
			      &pjc->TransformPQR[2].floats, NULL, pjc);
}

int XWPSContextState::cieAFinish()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSCieA *pcie = (XWPSCieA*)(op->getStruct());

  pcie->DecodeA = DecodeA_from_cache;
  pcie->common.DecodeLMN = DecodeLMN_from_cache;
  pcie->acomplete();
  pop(1);
  return 0;
}

int XWPSContextState::cieABCFinish()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSCieABC *pcie = (XWPSCieABC*)(op->getStruct());

  pcie->DecodeABC = DecodeABC_from_cache;
  pcie->common.DecodeLMN = DecodeLMN_from_cache;
  pcie->abccomplete();
  pop(1);
  return 0;
}

int XWPSContextState::cieCacheFinish()
{
	return cieCacheFinishStore(false);
}

int XWPSContextState::cieCacheFinish1()
{
	return cieCacheFinishStore(true);
}

int XWPSContextState::cieCacheFinishStore(bool replicate)
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSCieCacheFloats *pcache;
  int code;

	if (exec_stack.getCurrentTop() < (exec_stack.getBottom() + 1))
	{ 
		exec_stack.setRequested(2); 
		return (int)(XWPSError::ExecStackUnderflow); 
	}
	
	XWPSRef * esp = exec_stack.getCurrentTop();
  pcache = (PSCieCacheFloats *)(((char*)(esp[-1].getStruct())) + esp->value.intval);

  pcache->params.is_identity = false;	
  if (replicate ||	(code = floatParams(op, ps_cie_cache_size, &pcache->values[0])) < 0) 
  {
		uint i;

		for (i = 0; i < ps_cie_cache_size; i++) 
		{
	    code = op_stack.index(replicate ? 0 : ps_cie_cache_size - 1 - i)->floatParam(&pcache->values[i]);
	    if (code < 0)
				return code;
		}
  }
  
  op_stack.pop((replicate ? 1 : ps_cie_cache_size));
  exec_stack.incCurrentTop(-2);	
  return PS_O_POP_ESTACK;
}

int XWPSContextState::cieCachePushFinish(op_proc_t finish_proc, void *data)
{
	int code = checkEStack(2);
	if (code < 0)
		return code;
		
	XWPSRef*  esp = exec_stack.incCurrentTop(1);
	esp->makeOper(0, finish_proc);
	esp = exec_stack.incCurrentTop(1);
	((XWPSStruct*)data)->incRef();
  esp->makeStruct(iimemoryLocal()->space, (XWPSStruct*)data);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::cieCacheRenderFinish()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSCieRender *pcrd = (XWPSCieRender*)(op->getStruct());
  int code;

  if (pcrd->RenderTable.lookup.table != 0 &&	!pcrd->caches.RenderTableT_is_identity) 
  {
		int j;

		for (j = 0; j < pcrd->RenderTable.lookup.m; j++)
	    ps_cie_cache_to_fracs(&pcrd->caches.RenderTableT[j].floats, &pcrd->caches.RenderTableT[j].fracs);
  }
  pcrd->status = CIE_RENDER_STATUS_SAMPLED;
  pcrd->EncodeLMN = EncodeLMN_from_cache;
  pcrd->EncodeABC = EncodeABC_from_cache;
  pcrd->RenderTable.T = RenderTableT_from_cache;
  code = pcrd->complete();
  if (code < 0)
		return code;
  pop(1);
  return 0;
}

int XWPSContextState::cieDEFFinish()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSCieDef *pcie = (XWPSCieDef*)(op->getStruct());

  pcie->DecodeDEF = DecodeDEF_from_cache;
  pcie->common.DecodeLMN = DecodeLMN_from_cache;
  pcie->defcomplete();
  pop(1);
  return 0;
}

int XWPSContextState::cieDEFGFinish()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSCieDefg *pcie = (XWPSCieDefg*)(op->getStruct());

  pcie->DecodeDEFG = DecodeDEFG_from_cache;
  pcie->common.DecodeLMN = DecodeLMN_from_cache;
  pcie->defgcomplete();
  pop(1);
  return 0;
}

int XWPSContextState::cieExecTpqr()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef *ppt = op[-1].getArray();
  XWPSRef * arr = op - 1;
  uint space = op[-1].space();
  int i;

  int code = checkOp(op,3);
  if (code < 0)
		return code;
		
  code = push(&op, 4);
  if (code < 0)
		return code;
  op->assign(op - 4);		/* proc */
  op[-1].assign(op - 6);		/* v */
  for (i = 0; i < 4; i++)
  {
  	XWPSRef * p = op - 5 + i;
  	if (arr->value.refs->arr)
  		arr->value.refs->arr->incRef();
  	p->makeArray(space | PS_A_READONLY, 6, ppt + i * 6);
  	p->value.refs->arr = arr->value.refs->arr;
	}
  op[-6].makeMark();
  return zexec();
}

int XWPSContextState::cieLmnpParam(XWPSRef * pdref, 
	                 XWPSCie * pcie, 
	                 XWPSRefCieProcs * pcprocs)
{
	int code;

  if ((code = dictRange3Param(pdref, "RangeLMN", &pcie->common.RangeLMN)) < 0 ||
    	(code = dictProc3Param(pdref, "DecodeLMN", &pcprocs->DecodeLMN)) < 0 ||
			(code = dictMatrix3Param(pdref, "MatrixLMN", &pcie->common.MatrixLMN)) < 0 ||
			(code = ciePointsParam(pdref, &pcie->common.points)) < 0)
		return code;
  pcie->common.DecodeLMN = DecodeLMN_default;
  return 0;
}

int XWPSContextState::ciePointsParam(XWPSRef * pdref, PSCieWB * pwb)
{
	int code;

  if ((code = pdref->dictFloatsParam(this, "WhitePoint", 3, (float *)&pwb->WhitePoint, NULL)) < 0 ||
			(code = pdref->dictFloatsParam(this, "BlackPoint", 3, (float *)&pwb->BlackPoint, (const float *)&BlackPoint_default)) < 0)
		return code;
  if (pwb->WhitePoint.u <= 0 ||
			pwb->WhitePoint.v != 1 ||
			pwb->WhitePoint.w <= 0 ||
			pwb->BlackPoint.u < 0 ||
			pwb->BlackPoint.v < 0 ||
			pwb->BlackPoint.w < 0)
	{
	  return (int)(XWPSError::RangeCheck);
	}
  return 0;
}

int XWPSContextState::ciePostExecTpqr()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint count = op_stack.countToMark();
  XWPSRef vref;

  if (count < 2)
		return (int)(XWPSError::UnmatchedMark);
			
  vref.assign(op);
  op_stack.pop(count - 1);
  op_stack.getCurrentTop()->assign(&vref);
  return 0;
}

int XWPSContextState::ciePrepareCache(PSRange * domain, 
	                    XWPSRef * proc,
		                  PSCieCacheFloats * pcache, 
		                  void *container)
{
  PSForLoopParams flp;
 	XWPSRef * ep;

  ps_cie_cache_init(&pcache->params, &flp, domain);
  pcache->params.is_identity = proc->size() == 0;
  if (flp.step == 0) 
  {
		checkEStack(5);
		ep = exec_stack.getCurrentTop();
		ep[5].makeReal(flp.init);
		ep[4].assign(proc);
		ep[3].makeOper(0, &XWPSContextState::cieCacheFinish1);
		exec_stack.incCurrentTop(5);
  } 
  else 
  {
		checkEStack(9);
		ep = exec_stack.getCurrentTop();
		ep[9].makeReal(flp.init);
		ep[8].makeReal(flp.step);
		ep[7].makeReal(flp.limit);
		ep[6].assign(proc);
		ep[6].clearAttrs(PS_A_EXECUTABLE);
		ep[5].makeOper(0, &XWPSContextState::zcvx);
		ep[4].makeOper(0, &XWPSContextState::zfor);
		ep[3].makeOper(0, &XWPSContextState::cieCacheFinish);
		exec_stack.incCurrentTop(9);
  }
  
  ep[2].makeInt((char *)pcache - (char *)container);
  ((XWPSStruct*)container)->incRef();
  ep[1].makeStruct(iimemoryLocal()->space, (XWPSStruct*)container);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::ciePrepareCaches4(PSRange * domains,
		                    XWPSRef * procs,
		                    PSCieCacheFloats * pc0, 
		                    PSCieCacheFloats * pc1,
		                    PSCieCacheFloats * pc2, 
		                    PSCieCacheFloats * pc3,
		                    void *container)
{
	PSCieCacheFloats *pcn[4];
  int i, n, code = 0;

  pcn[0] = pc0, pcn[1] = pc1, pcn[2] = pc2;
  if (pc3 == 0)
		n = 3;
  else
		pcn[3] = pc3, n = 4;
  for (i = 0; i < n && code >= 0; ++i)
		code = ciePrepareCache(domains + i, procs + i, pcn[i], container);
  return code;
}

int XWPSContextState::cieTableParam(XWPSRef * ptref, XWPSColorLookupTable * pclt)
{
	int n = pclt->n, m = pclt->m;
  XWPSRef *pta = ptref->getArray();
  int i;
  uint nbytes;
  int code;
  XWPSString *table = 0;

  for (i = 0; i < n; ++i) 
  {
		code = pta[i].checkTypeOnly(XWPSRef::Integer);
		if (code < 0)
			return code;
		if (pta[i].value.intval <= 1 || pta[i].value.intval > max_ushort)
	  	return (int)(XWPSError::RangeCheck);
		pclt->dims[i] = (int)pta[i].value.intval;
  }
  nbytes = m * pclt->dims[n - 2] * pclt->dims[n - 1];
  if (n == 3) 
  {
  	pclt->table_size = pclt->dims[0];
  	if (pclt->dims[0] > 0)
		  table = new XWPSString[pclt->dims[0]];
		code = cie3dTableParam(pta + 3, pclt->dims[0], nbytes, table);
  } 
  else 
  {
		int d0 = pclt->dims[0], d1 = pclt->dims[1];
		uint ntables = d0 * d1;
		XWPSRef *psuba;

		pta[4].checkReadType(XWPSRef::Array);
		if (code < 0)
			return code;
		if (pta[4].size() != d0)
	  	return (int)(XWPSError::RangeCheck);
	  		
	  pclt->table_size = ntables;
	  if (ntables > 0)
		  table = new XWPSString[ntables];
		psuba = pta[4].getArray();
		for (code = 0, i = 0; i < d0; ++i) 
		{
	    code = cie3dTableParam(psuba + i, d1, nbytes, table + d1 * i);
	    if (code < 0)
				break;
		}
  }
  if (code < 0) 
  {
		delete [] table;
		return code;
  }
  pclt->table = table;
  return 0;
}

int XWPSContextState::cie3dTableParam(XWPSRef * ptable, 
	                     uint count, 
	                     uint nbytes,
		                   XWPSString * strings)
{
	XWPSRef *rstrings;
  uint i;

  int code = ptable->checkReadType(XWPSRef::Array);
  if (code < 0)
		return code;
		
  if (ptable->size() != count)
	 	return (int)(XWPSError::RangeCheck);
	 		
  rstrings = ptable->getArray();
  for (i = 0; i < count; ++i) 
  {
		XWPSRef * prt2 = rstrings + i;

		code = prt2->checkReadType(XWPSRef::String);
		if (code < 0)
			return code;
		if (prt2->size() != nbytes)
	 		return (int)(XWPSError::RangeCheck);
		strings[i].size = nbytes;
		strings[i].data = prt2->getBytes();
		strings[i].bytes = prt2->value.bytes->arr;
		if (strings[i].bytes)
			strings[i].bytes->incRef();
  }
  return 0;
}

int XWPSContextState::cieTpqrFinish()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSState *pgsA = (XWPSState*)(op->getStruct());
  XWPSCieRender *pcrd = pgsA->currentColorRendering();
  int code;

  op[-1].makeNull();
  pcrd->TransformPQR = TransformPQR_from_cache;
  code = pgsA->cieCSComplete(false);
  pop(2);
  return code;
}

int XWPSContextState::compositeImage(const PSCompositeAlphaParams * params)
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSAlphaCompositeState cstate;
  XWPSImage2 image;
  double src_rect[4];
  double dest_pt[2];
  XWPSMatrix save_ctm;
  int code = xywhParam(op - 4, src_rect);

  cstate.params = *params;
  if (code < 0 ||	(code = numParams(op - 1, 2, dest_pt)) < 0)
		return code;
  
  if (op[-3].hasType(XWPSRef::Null))
		image.DataSource = pgs;
  else 
  {
		op[-3].checkRead();
		code = op[-3].checkType("igstateobj");
		if (code < 0)
			return code;
		
		XWPSIGStateObj * o = (XWPSIGStateObj*)(op[-3].getStruct());
		image.DataSource = (XWPSState*)(o->gstate.getStruct());
  }
  image.XOrigin = src_rect[0];
  image.YOrigin = src_rect[1];
  image.Width = src_rect[2];
  image.Height = src_rect[3];
  image.PixelCopy = true;
  
  pgs->currentMatrix(&save_ctm);
  pgs->translate(dest_pt[0], dest_pt[1]);
  if (image.DataSource == pgs) 
  {
		image.XOrigin -= dest_pt[0];
		image.YOrigin -= dest_pt[1];
  }
  code = beginComposite(&cstate);
  if (code >= 0) 
  {
		code = processNonSourceImage(&image);
		endComposite(&cstate);
		if (code >= 0)
	    pop(8);
  }
  pgs->setMatrix(&save_ctm);
  return code;
}

int XWPSContextState::deviceNRemapCleanup()
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	int num_in = esp->value.intval;

	esp = exec_stack.incCurrentTop(-(num_in + 1));
  return PS_O_POP_ESTACK;
}

int XWPSContextState::deviceNRemapFinish()
{
	XWPSColorSpace *pcs = pgs->currentColorSpace();
  XWPSDeviceNParams *params;
  XWPSColorSpace *pacs;
  XWPSDeviceNMap *map;
  int num_in, num_out;
  float conc[PS_DEVICE_COLOR_MAX_COMPONENTS];
  int code;
  int i;

  while (pcs->getIndex() != ps_color_space_index_DeviceN) 
		pcs = pcs->baseSpace();
		
  params = pcs->params.device_n;
  num_in = params->num_components;
  pacs = params->alt_space;
  map = params->map;
  num_out = pacs->numComponents();
  code = floatParams(op_stack.getCurrentTop(), num_out, conc);
  if (code < 0)
		return code;
    
  XWPSRef * esp = exec_stack.incCurrentTop(-(num_in + 2));	
  for (i = 0; i < num_in; ++i)
		map->tint[i] = esp[i + 1].value.realval;
  for (i = 0; i < num_out; ++i)
		map->conc[i] = float2frac(conc[i]);
  map->cache_valid = true;
  op_stack.incCurrentTop(-num_out);
  return PS_O_POP_ESTACK;
}

int XWPSContextState::deviceNRemapPrepare()
{
	XWPSColorSpace *pcs = pgs->currentColorSpace();
  XWPSColorSpace *pbcs;
  XWPSClientColor cc;
  ushort ignore_conc[PS_DEVICE_COLOR_MAX_COMPONENTS];
  
  if (pcs->getIndex() == ps_color_space_index_Pattern) 
		pcs = pcs->baseSpace();
   
  pbcs = pcs;
  if (pbcs->getIndex() != ps_color_space_index_DeviceN) 
		pbcs = pbcs->baseSpace();
  
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  XWPSRef * p = &iigs->remap_color_info;
  XWPSIntRemapColorInfo * t = (XWPSIntRemapColorInfo*)(p->getStruct());
  memcpy(cc.paint.values, t->tint, pbcs->params.device_n->num_components * sizeof(float));
  pbcs->params.device_n->map->tint_transform = &XWPSDeviceNParams::deviceNRemapTransform;
  pbcs->params.device_n->map->tint_transform_data = this;
  return pcs->concretizeColor(&cc, ignore_conc, pgs);
}

int XWPSContextState::dictMatrix3Param(XWPSRef *pdref, 
	                     const char *kstr, 
	                     PSMatrix3 *pmat3)
{
	float values[9];
  int code;

  memcpy(&values[0], &Matrix3_default.cu, 3 * sizeof(float));
  memcpy(&values[3], &Matrix3_default.cv, 3 * sizeof(float));
  memcpy(&values[6], &Matrix3_default.cw, 3 * sizeof(float));
  code = pdref->dictFloatsParam(this, kstr, 9, values, values);
  if (code < 0)
		return code;
  memcpy(&pmat3->cu, &values[0], 3 * sizeof(float));
  memcpy(&pmat3->cv, &values[3], 3 * sizeof(float));
  memcpy(&pmat3->cw, &values[6], 3 * sizeof(float));
  return 0;
}

int XWPSContextState::dictProcArrayParam(XWPSRef * pdict, 
	                          const char *kstr,
		                        uint count, 
		                        XWPSRef * pparray)
{
	XWPSRef *pvalue;

  if (pdict->dictFindString(this, kstr, &pvalue) > 0) 
  {
		uint i;

		int code = pvalue->checkArrayOnly();
		if (code < 0)
			return code;
		if (pvalue->size() != count)
	    return (int)(XWPSError::RangeCheck);
	    	
		for (i = 0; i < count; i++) 
		{
	    XWPSRef proc;

	    pvalue->arrayGet(this, (long)i, &proc);
	    code = proc.checkProcOnly();
	    if (code < 0)
				return code;
		}
		pparray->assign(pvalue);
  } 
  else
  {
		pparray->makeArray(PS_A_READONLY | PS_AVM_FOREIGN, count);
		XWPSRef * p = pparray->getArray();
		for (int i = 0; i < count; i++)
		{
			p->makeArray(PS_A_READONLY | PS_A_EXECUTABLE);
			p++;
		}
	}
  return 0;
}

int XWPSContextState::dictProc3Param(XWPSRef *pdref, const char *kstr, XWPSRef proc3[3])
{
	return dictProcArrayParam(pdref, kstr, 3, proc3);
}

int XWPSContextState::dictRangesParam(XWPSRef * pdref, 
	                    const char *kstr, 
	                    int count,
		                  PSRange * prange)
{
	int code = pdref->dictFloatsParam(this, kstr, count * 2, (float *)prange, NULL);

  if (code < 0)
		return code;
  else if (code == 0)
		memcpy(prange, Range4_default.ranges, count * sizeof(PSRange));
  return 0;
}

int XWPSContextState::dictRange3Param(XWPSRef *pdref, 
	                    const char *kstr, 
	                    PSRange3 *prange3)
{
	return dictRangesParam(pdref, kstr, 3, prange3->ranges);
}

void XWPSContextState::endComposite(XWPSAlphaCompositeState * pcp)
{
	if (pcp->cdev != pcp->orig_dev) 
	{
		pcp->cdev->close();
		pgs->setDeviceNoInit(pcp->orig_dev);
  }
}

int XWPSContextState::indexedMap1()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * ep = exec_stack.getCurrentTop();
  int i = (int)ep[csme_index].value.intval;

  if (i >= 0) 
  {
		int m = (int)ep[csme_num_components].value.intval;
		XWPSIndexedMap * map = (XWPSIndexedMap*)(ep[csme_map].getStruct());
		int code = floatParams(op, m, &(map->values[i * m]));

		if (code < 0)
	    return code;
		pop(m);
		op -= m;
		if (i == (int)ep[csme_hival].value.intval) 
		{
	    exec_stack.incCurrentTop(-num_csme);
	    return PS_O_POP_ESTACK;
		}
  }
  int ccode = push(&op, 1);
  if (ccode < 0)
		return ccode;
  ep[csme_index].value.intval = ++i;
  op->makeInt(i);
  ep[1].makeOper(0, &XWPSContextState::indexedMap1);
  ep[2].assign(&ep[csme_proc]);
 	exec_stack.setCurrentTop(ep + 2);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::loadColorParams(XWPSRef * op, 
	                    PSPaintColor * pc, 
	                    XWPSColorSpace * pcs)
{
	int n = pcs->numComponents();
  int code = floatParams(op, n, pc->values);

  if (code < 0)
		return code;
  return n;
}

int XWPSContextState::patternPaintCleanup()
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	XWPSDevicePatternAccum * pdev = (XWPSDevicePatternAccum*)(esp[2].getDevice());
  pdev->close();
  return pgs->restore();
}

int XWPSContextState::patternPaintFinish()
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	XWPSDevicePatternAccum *pdev = (XWPSDevicePatternAccum*)(esp->getDevice());
  XWPSColorTile *ctile;
  int code = pgs->patternCacheAddEntry(pdev, &ctile);

  if (code < 0)
		return code;
  esp = exec_stack.incCurrentTop(-2);
  patternPaintCleanup();
  return PS_O_POP_ESTACK;
}

int XWPSContextState::patternPaintPrepare()
{
	XWPSPattern1Instance *pinst =	(XWPSPattern1Instance *)pgs->currentColor()->pattern;
  XWPSRef *pdict = &((XWPSIntPattern *) pinst->templat.client_data)->dict;
  XWPSDevicePatternAccum *pdev;
  int code;
  XWPSRef *ppp;

  code = checkEStack(5);
  if (code < 0)
  	return code;
  pdev = new XWPSDevicePatternAccum;
  pdev->instance = pinst;
  code = pdev->open();
  if (code < 0) 
		return code;
  code = pgs->save();
  if (code < 0)
		return code;
    
  code = pgs->setGState(pinst->saved);
  if (code < 0) 
  {
		pgs->restore();
		return code;
  }
  
  pgs->setDeviceOnly(pdev);
  XWPSRef * esp = exec_stack.incCurrentTop(1);
  esp->makeMarkEStack(PS_ES_OTHER, &XWPSContextState::patternPaintCleanup);
  esp = exec_stack.incCurrentTop(1);
  esp->makeDevice(currentSpace() | 0, pdev);
  esp = exec_stack.incCurrentTop(1);
  esp->makeOper(0, &XWPSContextState::patternPaintFinish);
  pdict->dictFindString(this, "PaintProc", &ppp);
  esp = exec_stack.incCurrentTop(1);
  esp->assign(ppp);
  esp = exec_stack.incCurrentTop(1);
  esp->assign(pdict);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::setCieFinish(XWPSColorSpace * pcs,
	       					 XWPSRefCieProcs * pcprocs, 
	       					 int edepth, 
	       					 int code)
{
	pcs->id = nextIDS(1);
	if (code >= 0)
		code = pgs->setColorSpace(pcs);
		  
  delete pcs;
  
  if (code < 0) 
  {
		exec_stack.pop(exec_stack.count() - edepth);
		return code;
  }
  
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->colorspace.procs.cie.copy(pcprocs);
  pop(1);
  return (exec_stack.count() == edepth ? 0 : PS_O_PUSH_ESTACK);
}

int XWPSContextState::separationMap1()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * ep = exec_stack.getCurrentTop();
  int i = (int)ep[csme_index].value.intval;

  if (i >= 0) 
  {	
		int m = (int)ep[csme_num_components].value.intval;
		XWPSIndexedMap * map = (XWPSIndexedMap*)(ep[csme_map].getStruct());
		int code = floatParams(op, m, &(map->values[i * m]));

		if (code < 0)
	    return code;
		pop(m);
		op -= m;
		if (i == (int)ep[csme_hival].value.intval) 
		{
	    XWPSFunction *pfn = refFunction(&ep[csme_proc]);

	    if (pfn)
	    {
	    	pfn->incRef();
				pgs->currentColorSpace()->setSeprFunction(pfn);
			}
				
			exec_stack.incCurrentTop(-num_csme);
	    return PS_O_POP_ESTACK;
		}
  }
  int ccode = push(&op, 1);
  if (ccode < 0)
		return ccode;
  ep[csme_index].value.intval = ++i;
  op->makeReal(i / (float)SEPARATION_CACHE_SIZE);
  ep[1].makeOper(0, &XWPSContextState::separationMap1);
  ep[2].assign(&ep[csme_proc]);
  exec_stack.setCurrentTop(ep + 2);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::storeColorParams(XWPSRef * op, 
	                     const PSPaintColor * pc,
		                   XWPSColorSpace * pcs)
{
	int n = pcs->numComponents();

  if (pcs->getIndex() == ps_color_space_index_Indexed)
		op[1].makeInt((int)pc->values[0]);
  else
  {
  	XWPSRef * p = op + 1;
  	float * par = (float*)&(pc->values[0]);
  	for (int i = n; i--; p++, par++)
    	p->makeReal(*par);
	}
  return n;
}

int XWPSContextState::zalphaImage()
{
	return zimageMultiple(true);
}

int XWPSContextState::zbuildColorRendering1()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  XWPSRef * ep = exec_stack.getCurrentTop();
  XWPSCieRender *pcrd;
  XWPSRefCieRenderProcs procs;

  code = op->checkReadType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = op->checkDictRead();
  if (code < 0)
  	return code;
  	
  pcrd = new XWPSCieRender;
  pcrd->id = nextIDS(1);
  code = zcrd1Params(op, pcrd, &procs);
  if (code < 0 || (code = cacheColorRendering1(pcrd, &procs)) < 0) 
  {
		exec_stack.setCurrentTop(ep);
		return code;
  }
  
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->colorrendering.dict.assign(op);
  op->makeStruct(PS_A_READONLY | currentSpace(), pcrd);
  return (exec_stack.getCurrentTop() == ep ? 0 : PS_O_PUSH_ESTACK);
}

int XWPSContextState::zbuildDeviceColorRendering1()
{
	XWPSRef * op = op_stack.getCurrentTop();  
  XWPSCieRender *pcrd = 0;
  int code;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  XWPSDictParamList list(false, op, NULL, false);
  list.writing = false;
  pcrd = new XWPSCieRender;
  pcrd->id = nextIDS(1);
  pcrd->device = dev;
	code = pcrd->paramGetCieRender1(this, &list, dev);	
  if (code < 0) 
		return code;
  
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->colorrendering.dict.assign(op);
  op->makeStruct(PS_A_READONLY | currentSpace(), pcrd);
  return 0;
}

int XWPSContextState::zbuildPattern1()
{
	XWPSRef * op = op_stack.getCurrentTop();  
  XWPSRef * op1 = op - 1;
  int code;
  XWPSMatrix mat;
  float BBox[4];
  XWPSPattern1 templat;
  XWPSIntPattern *pdata;
  XWPSClientColor cc_instance;
  XWPSRef *pPaintProc;

  code = op1->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = op1->checkDictRead();
 	if (code < 0)
  	return code;
  	
  if ((code = op->readMatrix(this, &mat)) < 0 ||
			(code = op1->dictUidParam(&templat.uid, 1, this)) != 1 ||
			(code = op1->dictIntParam(this, "PaintType", 1, 2, 0, &templat.PaintType)) < 0 ||
			(code = op1->dictIntParam(this, "TilingType", 1, 3, 0, &templat.TilingType)) < 0 ||
			(code = op1->dictFloatsParam(this, "BBox", 4, BBox, NULL)) < 0 ||
			(code = op1->dictFloatParam(this, "XStep", 0.0, &templat.XStep)) != 0 ||
			(code = op1->dictFloatParam(this, "YStep", 0.0, &templat.YStep)) != 0 ||
			(code = op1->dictFindString(this, "PaintProc", &pPaintProc)) <= 0)
		return ((code < 0 ? code : XWPSError::RangeCheck));
  code = pPaintProc->checkProc();
  if (code < 0)
  	return code;
  	
  templat.BBox.p.x = BBox[0];
  templat.BBox.p.y = BBox[1];
  templat.BBox.q.x = BBox[2];
  templat.BBox.q.y = BBox[3];
  templat.PaintProc = &XWPSClientColor::zPaintProc;
  pdata = new XWPSIntPattern(op1);
  
  templat.client_data = pdata;
  code = cc_instance.makePattern(&templat, &mat, pgs);
  if (code < 0) 
		return code;
		
	((XWPSPattern1Instance*)(cc_instance.pattern))->id = nextIDS(1);
  cc_instance.pattern->incRef();
  op->makeStruct(iimemoryLocal()->space | PS_A_READONLY, cc_instance.pattern);
  return code;
}

int XWPSContextState::zcolorImage()
{
	return zimageMultiple(false);
}

int XWPSContextState::zcolorRemapColor()
{
	pgs->unsetDevColor();
  return 0;
}

int XWPSContextState::zcolorRemapOne(XWPSRef * pproc,
		                 XWPSTransferMap * pmap, 
		                 const XWPSState * ,
		                 op_proc_t finish_proc)
{
	XWPSRef * op;
	if (pproc->size() == 0)
	{
		pmap->setIdentityTransfer();
		return PS_O_PUSH_ESTACK;
	}
	
	op = op_stack.incCurrentTop(4);
  op[-3].makeInt(0);
  op[-2].makeInt(1);
  op[-1].makeInt(transfer_map_size - 1);
  op->assign(pproc);
  XWPSRef * esp = exec_stack.incCurrentTop(1);  
  pmap->incRef();
  esp->makeStruct(0, pmap);
  esp = exec_stack.incCurrentTop(1);  
  esp->makeOper(0, finish_proc);
  esp = exec_stack.incCurrentTop(1);  
  esp->makeOper(iimemoryLocal()->space, &XWPSContextState::zforFraction);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zcolorRemapOneFinish()
{
	return zcolorRemapOneStore(0.0);
}

int XWPSContextState::zcolorRemapOneSignedFinish()
{
	return zcolorRemapOneStore(-1.0);
}

int XWPSContextState::zcolorRemapOneStore(float min_value)
{
	int i;
	XWPSRef * esp = exec_stack.getCurrentTop();  
  XWPSTransferMap *pmap = (XWPSTransferMap*)(esp->getStruct());

  if (op_stack.count() < transfer_map_size)
		return (int)(XWPSError::StackUnderflow);
    
  for (i = 0; i < transfer_map_size; i++) 
  {
		double v;
		int code =  op_stack.index(transfer_map_size - 1 - i)->realParam(&v);

		if (code < 0)
	    return code;
		pmap->values[i] =  (v < min_value ? float2frac(min_value) :   v >= 1.0 ? frac_1 :   float2frac(v));
  }
  op_stack.pop(transfer_map_size);
  esp = exec_stack.incCurrentTop(-1);
  return PS_O_POP_ESTACK;
}

int XWPSContextState::zcolorResetTransfer()
{
	pgs->setEffectiveTransfer();
  return zcolorRemapColor();
}

int XWPSContextState::zcomposite()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSCompositeAlphaParams params;

  op->checkIntLEU(composite_last);
  params.op = (PSCompositeOp) op->value.intval;
  return compositeImage(&params);
}

int XWPSContextState::zcompositeRect()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double dest_rect[4];
  XWPSAlphaCompositeState cstate;
  int code = xywhParam(op - 1, dest_rect);

  if (code < 0)
		return code;
    
  op->checkIntLEU(compositerect_last);
  cstate.params.op = (PSCompositeOp) op->value.intval;
  code = beginComposite(&cstate);
  if (code < 0)
		return code;
  {
		XWPSRect rect;

		rect.q.x = (rect.p.x = dest_rect[0]) + dest_rect[2];
		rect.q.y = (rect.p.y = dest_rect[1]) + dest_rect[3];
		code = pgs->rectFill(&rect, 1);
  }
  endComposite(&cstate);
  if (code >= 0)
		pop(5);
  return code;
}

int XWPSContextState::zcrd1Params(XWPSRef * op, 
	                 XWPSCieRender * pcrd,
	                 XWPSRefCieRenderProcs * pcprocs)
{
	int code;
  int ignore;
  XWPSColorLookupTable * prtl = &pcrd->RenderTable.lookup;
  XWPSRef *pRT;

  if ((code = op->dictIntParam(this,"ColorRenderingType", 1, 1, 0, &ignore)) < 0 ||
			(code = zcrd1ProcParams(op, pcprocs)) < 0 ||
			(code = dictMatrix3Param(op, "MatrixLMN", &pcrd->MatrixLMN)) < 0 ||
			(code = dictRange3Param(op, "RangeLMN", &pcrd->RangeLMN)) < 0 ||
			(code = dictMatrix3Param(op, "MatrixABC", &pcrd->MatrixABC)) < 0 ||
			(code = dictRange3Param(op, "RangeABC", &pcrd->RangeABC)) < 0 ||
			(code = ciePointsParam(op, &pcrd->points)) < 0 ||
			(code = dictMatrix3Param(op, "MatrixPQR", &pcrd->MatrixPQR)) < 0 ||
			(code = dictRange3Param(op, "RangePQR", &pcrd->RangePQR)) < 0)
		return code;
  if (op->dictFindString(this, "RenderTable", &pRT) > 0) 
  {
		XWPSRef *prte = pRT->getArray();

		prte[4].checkTypeOnly(XWPSRef::Integer);
		if (code < 0)
			return code;
			
		if (!(prte[4].value.intval == 3 || prte[4].value.intval == 4))
	    return (int)(XWPSError::RangeCheck);
	    	
		prtl->n = 3;
		prtl->m = prte[4].value.intval;
		if (pRT->size() != prtl->m + 5)
	    return (int)(XWPSError::RangeCheck);
	    	
		code = cieTableParam(pRT, prtl);
		if (code < 0)
	    return code;
  } 
  else 
		prtl->table = 0;
		
  pcrd->EncodeLMN = Encode_default;
  pcrd->EncodeABC = Encode_default;
  pcrd->TransformPQR = TransformPQR_default;
  pcrd->RenderTable.T = RenderTableT_default;
  return 0;
}

int XWPSContextState::zcrd1ProcParams(XWPSRef * op, XWPSRefCieRenderProcs * pcprocs)
{
	int code;
  XWPSRef *pRT;

  if ((code = dictProc3Param(op, "EncodeLMN", &pcprocs->EncodeLMN)) < 0 ||
      (code = dictProc3Param(op, "EncodeABC", &pcprocs->EncodeABC)) < 0 ||
    	(code = dictProc3Param(op, "TransformPQR", &pcprocs->TransformPQR)) < 0)
		return (code < 0 ? code : (int)(XWPSError::RangeCheck));
  
  if (op->dictFindString(this, "RenderTable", &pRT) > 0) 
  {
		XWPSRef *prte;
		int size;
		int i;

		code = pRT->checkReadType(XWPSRef::Array);
		if (code < 0)
			return code;
		size = pRT->size();
		if (size < 5)
	    return (int)(XWPSError::RangeCheck);
	    	
		prte = pRT->getArray();
		for (i = 5; i < size; i++)
	    prte[i].checkProcOnly();
	  
	  if (pRT->value.refs->arr)
	  	pRT->value.refs->arr->incRef();
	  pcprocs->RenderTableT.makeArray(PS_A_READONLY | pRT->space(), size - 5, prte + 5);
	  pcprocs->RenderTableT.value.refs->arr = pRT->value.refs->arr;
  } 
  else
  	pcprocs->RenderTableT.makeNull();
  
  return 0;
}

int XWPSContextState::zcsBeginMap(XWPSIndexedMap ** pmap, 
	                 XWPSRef * pproc,
	                 int num_entries,  
	                 XWPSColorSpace * base_space,
	      						op_proc_t map1)
{
	int num_components =	base_space->numComponents();
  int num_values = num_entries * num_components;
  XWPSIndexedMap *map = new XWPSIndexedMap(num_values);
  map->incRef();
  XWPSRef * ep;
  
  *pmap = map;
  int code = checkEStack(num_csme + 1);
  if (code < 0)
  {
  	*pmap = 0;
  	return code;
  }
  XWPSRef * esp = exec_stack.incCurrentTop(num_csme);
  ep = esp;
  ep[csme_num_components].makeInt(num_components);
  ep[csme_map].makeStruct(iimemoryLocal()->space, map);
  ep[csme_proc].assign(pproc);
  ep[csme_hival].makeInt(num_entries - 1);
  ep[csme_index].makeInt(-1);
  esp = exec_stack.incCurrentTop(1);
  esp->makeOper(0,map1);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zcurrentAlpha()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeReal(pgs->currentAlpha());
    return 0;
}

int XWPSContextState::zcurrentBlackGeneration()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  op->assign(&(iigs->black_generation));
  return 0;
}

int XWPSContextState::zcurrentCMYKColor()
{
	XWPSRef * op = op_stack.getCurrentTop();
  float par[4];
  int code = pgs->currentCMYKColor(par);

  if (code < 0)
		return code;
    
  code = push(&op, 4);
  if (code < 0)
		return code;
  XWPSRef * p = op - 3;
  float * pp = &par[0];
  for (int i = 4; i--; p++, pp++)
    p->makeReal(*pp);
  return 0;
}

int XWPSContextState::zcurrentColor()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSClientColor *pc = pgs->currentColor();
  XWPSColorSpace *pcs = pgs->currentColorSpace();
  int n;

  int code = checkOStack(5);	
  if (code < 0)
		return code;

  if (pcs->type->index == ps_color_space_index_Pattern) 
  {
		XWPSPatternInstance *pinst = pc->pattern;
		XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;

		n = 1;
		if (pinst != 0 && pinst->usesBaseSpace())
	    n += storeColorParams(op, &pc->paint, (pcs->params.pattern->base_space));
		op[n].assign(&iigs->pattern);
  } 
  else
		n = storeColorParams(op, &pc->paint, pcs);
  code = push(&op,n);
  if (code < 0)
		return code;
  return 0;
}

int XWPSContextState::zcurrentColorRendering()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  op->assign(&iigs->colorrendering.dict);
  return 0;
}

int XWPSContextState::zcurrentColorSpace()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  op->assign(&iigs->colorspace.array);
  if (op->hasType(XWPSRef::Null)) 
  {
		PSColorSpaceIndex csi = pgs->currentColorSpaceIndex();

		op->makeInt((int)(csi));
  }
  return 0;
}

int XWPSContextState::zcurrentColorTransfer()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op,4);
  if (code < 0)
		return code;
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  op[-3].assign(&iigs->transfer_procs.colored.red);
  op[-2].assign(&iigs->transfer_procs.colored.green);
  op[-1].assign(&iigs->transfer_procs.colored.blue);
  op->assign(&iigs->transfer_procs.colored.gray);
  return 0;
}

int XWPSContextState::zcurrentGray()
{
	XWPSRef * op = op_stack.getCurrentTop();
  float gray;
  int code = pgs->currentGray(&gray);

  if (code < 0)
		return code;
    
  code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeReal(gray);
  return 0;
}

int XWPSContextState::zcurrentHSBColor()
{
	XWPSRef * op = op_stack.getCurrentTop();
  float par[3];

  pgs->currentHSBColor(par);
  int code = push(&op, 3);
  if (code < 0)
		return code;
  XWPSRef * p = op - 2;
  int count = 3;
  float * pp = &par[0];
  for (; count--; p++, pp++)
		p->makeReal(*pp);
  return 0;
}

int XWPSContextState::zcurrentOverprint()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeBool(pgs->currentOverprint());
  return 0;
}

int XWPSContextState::zcurrentOverprintMode()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt(pgs->currentOverprintMode());
  return 0;
}

int XWPSContextState::zcurrentRGBColor()
{
	XWPSRef * op = op_stack.getCurrentTop();
  float par[3];
  int code = pgs->currentRGBColor(par);

  if (code < 0)
		return code;
		
  code = push(&op, 3);
  if (code < 0)
		return code;
  XWPSRef * p = op - 2;
  float * pp = &par[0];
  for (int i = 3; i--; p++, pp++)
    p->makeReal(*pp);
  return 0;
}

int XWPSContextState::zcurrentTransfer()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  op->assign(&(iigs->transfer_procs.colored.gray));
  return 0;
}

int XWPSContextState::zcurrentUnderColorRemoval()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op,1);
  if (code < 0)
		return code;
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  op->assign(&iigs->undercolor_removal);
  return 0;
}

int XWPSContextState::zdissolve()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSCompositeAlphaParams params;
  double delta;
  int code = op->realParam(&delta);

  if (code < 0)
		return code;
    
  if (delta < 0 || delta > 1)
		return (int)(XWPSError::RangeCheck);
			
  params.op = composite_Dissolve;
  params.delta = delta;
  return compositeImage(&params);
}

int XWPSContextState::zpcolorInit()
{
	XWPSPatternCache * pc = new XWPSPatternCache(max_cached_patterns_LARGE, max_pattern_bits_LARGE);
	pgs->setPatternCache(pc);
	return 0;
}

int XWPSContextState::zprocessColors()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt(pgs->currentDevice()->color_info.num_components);
  return 0;
}

int XWPSContextState::zsetAlpha()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double alpha;
  int code;

  if (op->realParam(&alpha) < 0)
		return op->checkTypeFailed();
  
  if ((code = pgs->setAlpha(alpha)) < 0)
		return code;
    
  pop(1);
  return 0;
}

int XWPSContextState::zsetBlackGeneration()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op->checkProc();
  if (code < 0)
		return code;
		
  code = checkOStack(zcolor_remap_one_ostack - 1);
  if (code < 0)
		return code;
		
  code = checkEStack(1 + zcolor_remap_one_estack);
  if (code < 0)
		return code;
		
  code = pgs->setBlackGenerationRemap(ps_tm_mapped, false);
  if (code < 0)
		return code;
    
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->black_generation.assign(op);
  pop(1);
  XWPSRef * esp = exec_stack.incCurrentTop(1);
  esp->makeOper(0, &XWPSContextState::zcolorRemapColor);
  return zcolorRemapOne(&iigs->black_generation,
			    pgs->black_generation, pgs,
			    &XWPSContextState::zcolorRemapOneFinish);
}

int XWPSContextState::zsetCieASpace()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int edepth = exec_stack.count();
  XWPSColorSpace *pcs;
  XWPSRefCieProcs procs;
  XWPSCieA *pcie;
  int code;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
		return code;
		
  code = op->checkDictRead();
  if (code < 0)
		return code;
		
	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  procs.copy(&iigs->colorspace.procs.cie);
  if ((code = op->dictProcParam(this, "DecodeA", &procs.ABC, true)) < 0)
		return code;
  
  pcs = new XWPSColorSpace(&ps_color_space_type_CIEA, 0);
  pcs->i_ctx_p = this;
  pcie = pcs->params.a;
  if ((code = op->dictFloatsParam(this, "RangeA", 2, (float *)&pcie->RangeA, (const float *)&RangeA_default)) < 0 ||
			(code = op->dictFloatsParam(this, "MatrixA", 3, (float *)&pcie->MatrixA, (const float *)&MatrixA_default)) < 0 ||
			(code = cieLmnpParam(op, pcie, &procs)) < 0 ||
			(code = cieCacheJoint(&iigs->colorrendering.procs, pcie, pgs)) < 0 ||
			(code = cieCachePushFinish(&XWPSContextState::cieAFinish, pcie)) < 0 ||
			(code = ciePrepareCache(&pcie->RangeA, &procs.ABC, &pcie->caches.DecodeA.floats, pcie)) < 0 ||
			(code = cacheCommon(pcie, &procs, pcie)) < 0)
		;
  pcie->DecodeA = DecodeA_default;
  return setCieFinish(pcs, &procs, edepth, code);
}

int XWPSContextState::zsetCieABCSpace()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int edepth = exec_stack.count();
  XWPSColorSpace *pcs;
  XWPSRefCieProcs procs;
  XWPSCieABC *pcie;
  int code;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
		return code;
		
  code = op->checkDictRead();
  if (code < 0)
		return code;
		
	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  procs.copy(&iigs->colorspace.procs.cie);
  pcs = new XWPSColorSpace(&ps_color_space_type_CIEABC, 0);
  pcs->i_ctx_p = this;
  pcie = pcs->params.abc;
  code = cieAbcParam(op, pcie, &procs);
  if (code < 0 ||
		(code = cieCacheJoint(&iigs->colorrendering.procs, pcie, pgs)) < 0 ||
		(code = cieCachePushFinish(&XWPSContextState::cieABCFinish, pcie)) < 0 ||
		(code = cacheAbcCommon(pcie, &procs, pcie)) < 0)
		;
  return setCieFinish(pcs, &procs, edepth, code);
}

int XWPSContextState::zsetCieDEFSpace()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int edepth = exec_stack.count();
  XWPSColorSpace *pcs;
  XWPSRefCieProcs procs;
  XWPSCieDef *pcie;
  int code;
  XWPSRef *ptref;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
		return code;
		
  code = op->checkDictRead();
  if (code < 0)
		return code;
		
  if ((code = op->dictFindString(this, "Table", &ptref)) <= 0)
		return (code < 0 ? code : (int)(XWPSError::RangeCheck));
			
  code = ptref->checkReadType(XWPSRef::Array);
  if (code < 0)
		return code;
		
  if (ptref->size() != 4)
		return (int)(XWPSError::RangeCheck);
	
	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  procs.copy(&iigs->colorspace.procs.cie);
  pcs = new XWPSColorSpace(&ps_color_space_type_CIEDEF, 0);
  pcs->i_ctx_p = this;
  pcie = pcs->params.def;
  pcie->Table.n = 3;
  pcie->Table.m = 3;
  if ((code = dictRange3Param(op, "RangeDEF", &pcie->RangeDEF)) < 0 ||
			(code = dictProc3Param(op, "DecodeDEF", &procs.DEFG)) < 0 ||
			(code = dictRange3Param(op, "RangeHIJ", &pcie->RangeHIJ)) < 0 ||
			(code = cieTableParam(ptref, &pcie->Table)) < 0 ||
			(code = cieAbcParam(op, pcie, &procs)) < 0 ||
			(code = cieCacheJoint(&iigs->colorrendering.procs, pcie, pgs)) < 0 ||
			(code = cieCachePushFinish(&XWPSContextState::cieDEFFinish, pcie)) < 0 ||
			(code = ciePrepareCaches4(&pcie->RangeDEF.ranges[0],
				   		procs.DEFG.getArray(),
				   		&pcie->caches_def.DecodeDEF[0].floats,
				   		&pcie->caches_def.DecodeDEF[1].floats,
				   		&pcie->caches_def.DecodeDEF[2].floats,NULL, pcie)) < 0 ||
			(code = cacheAbcCommon(pcie, &procs, pcie)) < 0)
		;
  return setCieFinish(pcs, &procs, edepth, code);
}

int XWPSContextState::zsetCieDEFGSpace()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int edepth = exec_stack.count();
  XWPSColorSpace *pcs;
  XWPSRefCieProcs procs;
  XWPSCieDefg *pcie;
  int code;
  XWPSRef *ptref;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
		return code;
		
  code = op->checkDictRead();
  if (code < 0)
		return code;
		
  if ((code = op->dictFindString(this, "Table", &ptref)) <= 0)
		return (code < 0 ? code : (int)(XWPSError::RangeCheck));
	
  code = ptref->checkReadType(XWPSRef::Array);
  if (code < 0)
		return code;
		
  if (ptref->size() != 5)
		return (int)(XWPSError::RangeCheck);
	
	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  procs.copy(&iigs->colorspace.procs.cie);
  
  pcs = new XWPSColorSpace(&ps_color_space_type_CIEDEFG, 0);
  pcs->i_ctx_p = this;
  pcie = pcs->params.defg;
  pcie->Table.n = 4;
  pcie->Table.m = 3;
  if ((code = dictRangesParam(op, "RangeDEFG", 4, pcie->RangeDEFG.ranges)) < 0 ||
			(code = dictProcArrayParam(op, "DecodeDEFG", 4, &procs.DEFG)) < 0 ||
			(code = dictRangesParam(op, "RangeHIJK", 4, pcie->RangeHIJK.ranges)) < 0 ||
			(code = cieTableParam(ptref, &pcie->Table)) < 0 ||
			(code = cieAbcParam(op, pcie, &procs)) < 0 ||
			(code = cieCacheJoint(&iigs->colorrendering.procs, pcie, pgs)) < 0 ||
			(code = cieCachePushFinish(&XWPSContextState::cieDEFGFinish, pcie)) < 0 ||
			(code = ciePrepareCaches4(&pcie->RangeDEFG.ranges[0], 
			                  procs.DEFG.getArray(),
				   &pcie->caches_defg.DecodeDEFG[0].floats, 
				   &pcie->caches_defg.DecodeDEFG[1].floats,
				   &pcie->caches_defg.DecodeDEFG[2].floats, 
				   &pcie->caches_defg.DecodeDEFG[3].floats, 
				   pcie)) < 0 ||
			(code = cacheAbcCommon(pcie, &procs, pcie)) < 0)
		;
  return setCieFinish(pcs, &procs, edepth, code);
}

int XWPSContextState::zsetCMYKColor()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double par[4];
  int code;

  if ((code = numParams(op, 4, par)) < 0 ||
		(code = pgs->setCMYKColor(par[0], par[1], par[2], par[3])) < 0)
		return code;
	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->colorspace.array.makeNull();
  pop(4);
  return 0;
}

int XWPSContextState::zsetColor()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSClientColor c;
  XWPSColorSpace *pcs = pgs->currentColorSpace();
  int n, code;
  XWPSPatternInstance *pinst = 0;

  if (pcs->getIndex() == ps_color_space_index_Pattern) 
  {
		XWPSRef *pImpl;

		if (op->hasType(XWPSRef::Null)) 
		{
	    c.pattern = 0;
	    n = 1;
		} 
		else 
		{
	    code = op->checkType(XWPSRef::Dictionary);
	    if (code < 0)
				return code;
	    code = op->checkDictRead();
	    if (code < 0)
				return code;
	    
	    if (op->dictFindString(this, "Implementation", &pImpl) <= 0 || !pImpl->isStruct())
				return (int)(XWPSError::RangeCheck);
					
	    pinst = (XWPSPatternInstance*)(pImpl->getStruct());
	    c.pattern = pinst;
	    pinst->incRef();
	    if (pinst->usesBaseSpace()) 
	    {
				if (!pcs->params.pattern->has_base_space)
					return (int)(XWPSError::RangeCheck);
					
				n = loadColorParams(op - 1, &c.paint, pcs->params.pattern->base_space);
				if (n < 0)
		    	return n;
				n++;
	    } 
	    else
				n = 1;
		}
  } 
  else 
  {
		n = loadColorParams(op, &c.paint, pcs);
		c.pattern = 0;
  }
  if (n < 0)
		return n;
  code = pgs->setColor(&c);
  if (code < 0)
		return code;
  if (pinst != 0)
  {
  	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
		iigs->pattern.assign(op);
	}
  pop(n);
  return code;
}

int XWPSContextState::zsetColorRendering1()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * ep = exec_stack.getCurrentTop();
  XWPSRefCieRenderProcs procs;
  int code;

  code = op[-1].checkType(XWPSRef::Dictionary);
  if (code < 0)
		return code;
		
	code = op->checkType("cierender");
	if (code < 0)
		return code;
	
  code = zcrd1ProcParams(op - 1, &procs);
  if (code < 0)
		return code;
    
  code = pgs->setColorRendering((XWPSCieRender*) (op->getStruct()));
  if (code < 0)
		return code;
  if (pgs->cieCSCommon() != 0 &&
		(code = cieCacheJoint(&procs, pgs->cieCSCommon(), pgs)) < 0)
		return code;
  
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->colorrendering.dict.assign(op - 1);
  iigs->colorrendering.procs.copy(&procs);
  pop(2);
  return (exec_stack.getCurrentTop() == ep ? 0 : PS_O_PUSH_ESTACK);
}

int XWPSContextState::zsetColorSpace()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Array);
  if (code < 0)
		return code;
		
	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->colorspace.array.assign(op);
  pop(1);
  return 0;
}

int XWPSContextState::zsetColorTransfer()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op[-3].checkProc();
  if (code < 0)
		return code;
		
  code = op[-2].checkProc();
  if (code < 0)
		return code;
		
  code = op[-1].checkProc();
  if (code < 0)
		return code;
		
  code = op->checkProc();
  if (code < 0)
		return code;
		
  code = checkOStack(zcolor_remap_one_ostack * 4 - 4);
  if (code < 0)
		return code;
		
  code = checkEStack(1 + zcolor_remap_one_estack * 4);
  if (code < 0)
		return code;
		
	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->transfer_procs.colored.red.assign(op - 3);
  iigs->transfer_procs.colored.green.assign(op - 2);
  iigs->transfer_procs.colored.blue.assign(op - 1);
  iigs->transfer_procs.colored.gray.assign(op);
  if ((code = pgs->setColorTransferRemap(ps_tm_mapped, ps_tm_mapped, ps_tm_mapped, ps_tm_mapped, false)) < 0)
		return code;
		
  pop(4);
  XWPSRef * esp = exec_stack.incCurrentTop(1);
  esp->makeOper(0, &XWPSContextState::zcolorResetTransfer);
  	
  if ((code = zcolorRemapOne(&iigs->transfer_procs.colored.red, 
  	                          pgs->set_transfer.u.colored.red, pgs,	 
  	                          &XWPSContextState::zcolorRemapOneFinish)) < 0 ||
			(code = zcolorRemapOne(&iigs->transfer_procs.colored.green,
				 											pgs->set_transfer.u.colored.green, pgs,
				 											&XWPSContextState::zcolorRemapOneFinish)) < 0 ||
			(code = zcolorRemapOne(&iigs->transfer_procs.colored.blue,
				 										pgs->set_transfer.u.colored.blue, pgs,
				 										&XWPSContextState::zcolorRemapOneFinish)) < 0 ||
			(code = zcolorRemapOne(&iigs->transfer_procs.colored.gray,
				 										pgs->set_transfer.u.colored.gray, pgs,
				 										&XWPSContextState::zcolorRemapOneFinish)) < 0	)
		return code;
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zsetDeviceColorRendering1()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  XWPSRefCieRenderProcs procs;

  code = op[-1].checkType(XWPSRef::Dictionary);
  if (code < 0)
		return code;
		
	code = op->checkType("cierender");
	if (code < 0)
		return code;
		
  code = pgs->setColorRendering((XWPSCieRender*) (op->getStruct()));
  if (code < 0)
		return code;
    
  if (pgs->cieCSCommon() != 0 &&	(code = cieCacheJoint(&procs, pgs->cieCSCommon(), pgs)) < 0)
		return code;
    
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->colorrendering.dict.assign(op - 1);
  pop(2);
  return 0;
}

int XWPSContextState::zsetDeviceNSpace()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef *pcsa;
  ulong *names;
  XWPSDeviceNMap *pmap;
  uint num_components;
  XWPSColorSpace cs;
  cs.i_ctx_p = this;
  XWPSRefColorSpace cspace_old;
  XWPSFunction *pfn;
  int code;

  code = op->checkReadType(XWPSRef::Array);
  if (code < 0)
		return code;
		
  if (op->size() != 4)
		return (int)(XWPSError::RangeCheck);
			
  pcsa = op->getArray() + 1;
  if (!pcsa->isArray())
		return pcsa->checkTypeFailed();
		
  num_components = pcsa->size();
  if (num_components == 0)
		return (int)(XWPSError::RangeCheck);
			
  if (num_components > PS_CLIENT_COLOR_MAX_COMPONENTS)
		return (int)(XWPSError::LimitCheck);
			
  code = pcsa[2].checkProc();
  if (code < 0)
		return code;
	
  if (!pgs->currentColorSpace()->type->can_be_alt_space)
		return (int)(XWPSError::RangeCheck);
			
  pmap = new XWPSDeviceNMap;
  names = new ulong[num_components];
  {
		uint i;
		XWPSRef sname;

		for (i = 0; i < num_components; ++i) 
		{
	   	pcsa->arrayGet(this, (long)i, &sname);
	   	switch (sname.type()) 
	   	{
				case XWPSRef::String:
		    	code = nameFromString(&sname, &sname);
	    		if (code < 0) 
	    		{
						delete [] names;
						delete pmap;
						return code;
	    		}
		    	
				case XWPSRef::Name:
	    		names[i] = nameIndex(&sname);
	    		break;
		    		
				default:
	    		delete [] names;
					delete pmap;
	    		return sname.checkTypeFailed();
	   	}
		}
  }
  cs.init(&ps_color_space_type_DeviceN);
  cs.params.device_n->alt_space = pgs->currentColorSpace();
  if (cs.params.device_n->alt_space)
  	cs.params.device_n->alt_space->incRef();
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  
  cspace_old.copy(&iigs->colorspace);
  iigs->colorspace.procs.device_n.layer_names.assign(&pcsa[0]);
  iigs->colorspace.procs.device_n.tint_transform.assign(&pcsa[2]);
  cs.params.device_n->names = names;
  cs.params.device_n->num_components = num_components;
  cs.params.device_n->map = pmap;
  pfn = refFunction(pcsa + 2);
  if (pfn)
  {
  	pfn->incRef();
		cs.setDevNFunction(pfn);
	}
  else
		pmap->tint_transform = &XWPSDeviceNParams::ztransformDeviceN;
  code = pgs->setColorSpace(&cs);
  if (code < 0) 
  {
		iigs->colorspace.copy(&cspace_old);
		return code;
  }
  pop(1);
  return 0;
}

int XWPSContextState::zsetDevicePixelSpace()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef depth;
  XWPSColorSpace cs;
  cs.i_ctx_p = this;
  int code;

  code = op->checkReadType(XWPSRef::Array);
  if (code < 0)
  	return code;
  	
  if (op->size() != 2)
		return (int)(XWPSError::RangeCheck);
			
  op->arrayGet(this, 1L, &depth);
  code = depth.checkTypeOnly(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  cs.initDevicePixel((int)depth.value.intval);
  code = pgs->setColorSpace(&cs);
  if (code >= 0)
		pop(1);
  return code;
}

int XWPSContextState::zsetGray()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double gray;
  int code;

  if (op->realParam(&gray) < 0)
		return op->checkTypeFailed();
    
  if ((code = pgs->setGray(gray)) < 0)
		return code;
    
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->colorspace.array.makeNull();
  pop(1);
  return 0;
}

int XWPSContextState::zsetHSBColor()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double par[3];
  int code;

  if ((code = numParams(op, 3, par)) < 0 ||
			(code = pgs->setHSBColor(par[0], par[1], par[2])) < 0)
		return code;
    
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->colorspace.array.makeNull();
  pop(3);
  return 0;
}

int XWPSContextState::zsetIndexedSpace()
{
	XWPSRef * op = op_stack.getCurrentTop();
	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  XWPSRef *pproc = &iigs->colorspace.procs.index_proc;
  XWPSRef *pcsa;
  XWPSColorSpace cs;
  cs.i_ctx_p = this;
  XWPSRefColorSpace cspace_old;
  uint edepth = exec_stack.count();
  int num_entries;
  int code;

  code = op->checkReadType(XWPSRef::Array);
  if (code < 0)
  	return code;
  	
  if (op->size() != 4)
		return (int)(XWPSError::RangeCheck);
  pcsa = op->getArray() + 1;
  code = pcsa[1].checkTypeOnly(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if (pcsa[1].value.intval < 0 || pcsa[1].value.intval > 4095)
		return (int)(XWPSError::RangeCheck);
    
  num_entries = (int)pcsa[1].value.intval + 1;
  if (!pgs->currentColorSpace()->type->can_be_base_space)
		return (int)(XWPSError::RangeCheck);
	
  cspace_old.copy(&iigs->colorspace);
  if (pcsa[2].hasType(XWPSRef::String)) 
  {
		int num_values = num_entries * pgs->currentColorSpace()->numComponents();

		code = pcsa[2].checkRead();
		if (code < 0)
  		return code;
		if (pcsa[2].size() != num_values)
			return (int)(XWPSError::RangeCheck);
				
	  cs.init(&ps_color_space_type_Indexed);
		cs.params.indexed->base_space = pgs->currentColorSpace();
		if (cs.params.indexed->base_space)
			cs.params.indexed->base_space->incRef();
		cs.params.indexed->lookup.table = new XWPSString;
		cs.params.indexed->lookup.table->data = pcsa[2].getBytes(); 
		cs.params.indexed->lookup.table->size = num_values;
		cs.params.indexed->lookup.table->bytes = pcsa[2].value.bytes->arr;
		if (pcsa[2].value.bytes->arr)
			pcsa[2].value.bytes->arr->incRef();
		cs.params.indexed->use_proc = 0;
		pproc->makeNull();
		code = 0;
  } 
  else 
  {
		XWPSIndexedMap *map;

		code = pcsa[2].checkProc();
		if (code < 0)
  		return code;
		code = zcsBeginMap(&map, &pcsa[2], num_entries, &cs,  &XWPSContextState::indexedMap1);
		if (code < 0)
	    return code;
		cs.init(&ps_color_space_type_Indexed);
		cs.params.indexed->base_space = pgs->currentColorSpace();
		if (cs.params.indexed->base_space)
			cs.params.indexed->base_space->incRef();
		cs.params.indexed->use_proc = 1;
		pproc->assign(&pcsa[2]);
		map->proc.lookup_index = &XWPSIndexedParams::lookupIndexedMap;
		cs.params.indexed->lookup.map = map;
  }
  cs.params.indexed->hival = num_entries - 1;
  code = pgs->setColorSpace(&cs);
  if (code < 0) 
  {
		iigs->colorspace.copy(&cspace_old);
		exec_stack.pop(exec_stack.count() - edepth);
		return code;
  }
  pop(1);
  return (exec_stack.count() == edepth ? 0 : PS_O_PUSH_ESTACK);
}

int XWPSContextState::zsetOverprint()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
    
  pgs->setOverprint(op->value.boolval);
  pop(1);
  return 0;
}

int XWPSContextState::zsetOverprintMode()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int param;
  int code = op->intParam(max_int, &param);

  if (code < 0 || (code = pgs->setOverprintMode(param)) < 0)
		return code;
  pop(1);
  return 0;
}

int XWPSContextState::zsetPatternSpace()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSColorSpace cs;
  cs.i_ctx_p = this;
  uint edepth = exec_stack.count();
  int code;

  code = op->checkReadType(XWPSRef::Array);
  if (code < 0)
  	return code;
  	
  cs.init(&ps_color_space_type_Pattern);
  switch (op->size()) 
  {
		case 1:	
	    cs.params.pattern->has_base_space = false;
	    break;
	    
		default:
	    return (int)(XWPSError::RangeCheck);
	    
		case 2:
	    if (pgs->currentColorSpace()->numComponents() < 0)
				return (int)(XWPSError::RangeCheck);
			
	    cs.params.pattern->has_base_space = true;
	    cs.params.pattern->base_space = pgs->currentColorSpace();
	    if (cs.params.pattern->base_space)
	    	cs.params.pattern->base_space->incRef();
  }

  code = pgs->setColorSpace(&cs);
  if (code < 0) 
  {
		exec_stack.pop(exec_stack.count() - edepth);
		return code;
  }
  pop(1);
  return (exec_stack.count() == edepth ? 0 : PS_O_PUSH_ESTACK);
}

int XWPSContextState::zsetRGBColor()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double par[3];
  int code;

  if ((code = numParams(op, 3, par)) < 0 ||
		(code = pgs->setRGBColor(par[0], par[1], par[2])) < 0)
		return code;
    
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->colorspace.array.makeNull();
  pop(3);
  return 0;
}

int XWPSContextState::zsetSeparationSpace()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef *pcsa;
  XWPSColorSpace cs;
  cs.i_ctx_p = this;
  XWPSRefColorSpace cspace_old;
  uint edepth = exec_stack.count();
  XWPSIndexedMap *map;
  XWPSRef sname;
  int code;

  code = op->checkReadType(XWPSRef::Array);
  if (code < 0)
	  return code;
    
  if (op->size() != 4)
		return (int)(XWPSError::RangeCheck);
			
  pcsa = op->getArray() + 1;
  sname.assign(pcsa);
  
  switch (sname.type()) 
  {
		default:
	    return sname.checkTypeFailed();
		    
		case XWPSRef::String:
	    code = nameFromString(&sname, &sname);
	    if (code < 0)
				return code;
				
			case XWPSRef::Name:
	    	break;
  }
  code = pcsa[2].checkProc();
  if (code < 0)
	  return code;

  if (!pgs->currentColorSpace()->type->can_be_alt_space)
		return (int)(XWPSError::RangeCheck);
    
  code = zcsBeginMap(&map, &pcsa[2], SEPARATION_CACHE_SIZE + 1, pgs->currentColorSpace(),	 &XWPSContextState::separationMap1);
  if (code < 0)
		return code;
    
  map->proc.tint_transform = &XWPSSeparationParams::lookupTint;
  cs.init(&ps_color_space_type_Separation);
  cs.params.separation->alt_space = pgs->currentColorSpace();
  if (cs.params.separation->alt_space)
  	cs.params.separation->alt_space->incRef();
  cs.params.separation->sname = nameIndex(&sname);
  cs.params.separation->map = map;
  XWPSIntGState * iigs = (XWPSIntGState*)(pgs->client_data);
  cspace_old.copy(&iigs->colorspace);
  iigs->colorspace.procs.separation.layer_name.assign(&pcsa[0]);
  iigs->colorspace.procs.separation.tint_transform.assign(&pcsa[2]);
  code = pgs->setColorSpace(&cs);
  if (code < 0) 
  {
		iigs->colorspace.copy(&cspace_old);
		exec_stack.pop(exec_stack.count() - edepth);
		return code;
  }
  pop(1);
  return (exec_stack.count() == edepth ? 0 : PS_O_PUSH_ESTACK);
}

int XWPSContextState::zsetTransfer()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op->checkProc();
  if (code < 0)
		return code;
		
  code = checkOStack(zcolor_remap_one_ostack - 1);
  if (code < 0)
		return code;
		
  code = checkEStack(1 + zcolor_remap_one_estack);
  if (code < 0)
		return code;
		
	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
		
  iigs->transfer_procs.colored.red.assign(op);
	iigs->transfer_procs.colored.green.assign(op);
	iigs->transfer_procs.colored.blue.assign(op);
	iigs->transfer_procs.colored.gray.assign(op);
  code = pgs->setTransferRemap(ps_tm_mapped, false);
  if (code < 0)
		return code;
		
	XWPSRef * esp = exec_stack.incCurrentTop(1);
	esp->makeOper(0, &XWPSContextState::zcolorResetTransfer);
  pop(1);
  return zcolorRemapOne(&iigs->transfer_procs.colored.gray,
			    						pgs->set_transfer.u.colored.gray, pgs,
			    						&XWPSContextState::zcolorRemapOneFinish);
}

int XWPSContextState::zsetUnderColorRemoval()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op->checkProc();
  if (code < 0)
		return code;
		
  code = checkOStack(zcolor_remap_one_ostack - 1);
  if (code < 0)
		return code;
		
  code = checkEStack(1 + zcolor_remap_one_estack);
  if (code < 0)
		return code;
		
  code = pgs->setUnderColorRemovalRemap(ps_tm_mapped, false);
  if (code < 0)
		return code;
	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->undercolor_removal.assign(op);
  pop(1);
  XWPSRef * esp = exec_stack.incCurrentTop(1);
	esp->makeOper(0, &XWPSContextState::zcolorRemapColor);
  return zcolorRemapOne(&iigs->undercolor_removal,
			    				pgs->undercolor_removal, pgs,
			    				&XWPSContextState::zcolorRemapOneSignedFinish);
}

static void
box_confine(int *pp, int *pq, int wh)
{
    if ( *pq <= 0 )
	*pp = *pq = 0;
    else if ( *pp >= wh )
	*pp = *pq = wh;
    else {
	if ( *pp < 0 )
	    *pp = 0;
	if ( *pq > wh )
	    *pq = wh;
    }
}


int XWPSContextState::zsizeImageBox()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSDevice *dev = pgs->currentDevice();
  XWPSRect srect, drect;
  XWPSMatrix mat;
  XWPSIntRect rect;
  int w, h;
  int code;

  code = op[-4].checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  op[-3].checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  op[-2].checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  op[-1].checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  srect.p.x = op[-4].value.intval;
  srect.p.y = op[-3].value.intval;
  srect.q.x = srect.p.x + op[-2].value.intval;
  srect.q.y = srect.p.y + op[-1].value.intval;
  pgs->currentMatrix(&mat);
  srect.bboxTransform(&mat, &drect);
  
  rect.p.x = (int)floor(drect.p.x);
  rect.p.y = (int)floor(drect.p.y);
  rect.q.x = (int)ceil(drect.q.x);
  rect.q.y = (int)ceil(drect.q.y);
  
  box_confine(&rect.p.x, &rect.q.x, dev->width);
  box_confine(&rect.p.y, &rect.q.y, dev->height);
  w = rect.q.x - rect.p.x;
  h = rect.q.y - rect.p.y;
  
  mat.tx -= rect.p.x;
  mat.ty -= rect.p.y;
  code = op->writeMatrixIn(&mat);
  if (code < 0)
		return code;
    
  op[-4].makeInt(rect.p.x);
  op[-3].makeInt(rect.p.y);
  op[-2].makeInt(w);
  op[-1].makeInt(h);
  return 0;
}

int XWPSContextState::zsizeImageParams()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSDevice *dev = pgs->currentDevice();
  int ncomp = dev->color_info.num_components;
  int bps;

  int code = push(&op, 3);
  if (code < 0)
		return code;
  if (dev->isTrueColor())
		bps = dev->color_info.depth / ncomp;
  else 
  {
		ushort max_value =  (dev->color_info.num_components == 1 ?   dev->color_info.max_gray :
	                 qMax(dev->color_info.max_gray, dev->color_info.max_color));
		static const ushort sizes[] = {
	    1, 2, 4, 8, 12, sizeof(ps_max_color_value) * 8};
		int i;

		for (i = 0;; ++i)
	    if (max_value <= ((ulong) 1 << sizes[i]) - 1)
				break;
		bps = sizes[i];
  }
  op[-2].makeInt(bps);
  op[-1].makeFalse();
  op->makeInt(ncomp);
  return 0;
}

int XWPSContextState::xywhParam(XWPSRef * op, double rect[4])
{
	int code = numParams(op, 4, rect);

  if (code < 0)
		return code;
    
  if (rect[2] < 0)
		rect[0] += rect[2], rect[2] = -rect[2];
    
  if (rect[3] < 0)
		rect[1] += rect[3], rect[3] = -rect[3];
  return code;
}
