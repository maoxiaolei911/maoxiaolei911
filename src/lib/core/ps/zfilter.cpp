/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSParam.h"
#include "XWPSStream.h"
#include "XWPSContextState.h"

int XWPSContextState::filterEnsureBuf(XWPSStream ** ps, uint min_buf_size, bool writing)
{
	XWPSStream *s = *ps;
  uint min_size = min_buf_size + PS_MAX_MIN_LEFT;
  XWPSStream *bs;
  XWPSRef bsop;
  int code;

  if (s->modes == 0 || s->bsize >= min_size)
		return 0;
    
  if (s->cbuf == 0) 
  {
  	XWPSBytes * b = new XWPSBytes;
  	s->cbuf_bytes = b;
		uint len = qMax(min_size, (uint)128);
		uchar *buf = (uchar*)malloc((len+1)*sizeof(uchar)); 
		s->cbuf_bytes->arr = buf;
		s->cbuf_bytes->self = true;
		s->cbuf = buf;
		s->cursor.r.ptr = s->cursor.r.limit = buf - 1;
		s->cursor.w.limit = buf - 1 + len;
		s->bsize = s->cbsize = len;
		return 0;
  } 
  else 
  {
  	PSStreamState * st = new PSStreamState;
		if (writing)
	    code = filterOpen("w", min_size, &bsop, &s_filter_write_procs, &s_NullE_template, st);
		else
	    code = filterOpen("r", min_size, &bsop, &s_filter_read_procs, &s_Null1D_template, st);
		if (code < 0)
	    return code;
		bs = bsop.getStream();
		bs->incRef();
		bs->strm = s;
		bs->is_temp = 2;
		*ps = bs;
		return code;
  }
}

void XWPSContextState::filterMarkStrmTemp(XWPSRef * fop, int is_temp)
{
	XWPSStream *s = fop->getStream();
  XWPSStream *strm = s->strm;

  strm->is_temp = is_temp;
  s->close_strm = strm->close_strm;
}

int XWPSContextState::filterRead(int npop, 
	               const PSStreamTemplate * templat,
	                PSStreamState * st, 
	                uint space)
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint min_size = templat->min_out_size + PS_MAX_MIN_LEFT;
  uint save_space = idmemory()->iallocSpace();
  XWPSRef * sop = op - npop;
  XWPSStream *s;
  XWPSStream *sstrm;
  bool close = false;
  int code;

  if (sop->hasType(XWPSRef::Dictionary)) 
  {
		code = sop->checkDictRead();
		if (code < 0)
		{
			if (st)
				delete st;
			return code;
		}
			
		if ((code = sop->dictBoolParam(this, "CloseSource", false, &close)) < 0)
		{
			if (st)
				delete st;
	    return code;
	  }
		--sop;
  }
  
  switch (sop->type()) 
  {
		case XWPSRef::String:
			idmemory()->setSpace(qMax(space, (uint)(sop->space())));
	    code = sop->checkRead();
	    if (code < 0)
	    {
	    	if (st)
					delete st;
				return code;
			}
	    sstrm = new XWPSStream;
	    sstrm->cbuf_bytes = sop->value.bytes->arr;
	    if (sstrm->cbuf_bytes)
	    	sstrm->cbuf_bytes->incRef();
	    sstrm->readString(sop->getBytes(), sop->size(), false);
	    sstrm->is_temp = 1;
	    break;
	    
		case XWPSRef::File:
			sstrm = sop->getStream();
			if (sstrm->read_id != sop->size())
			{
				if (sstrm->read_id == 0 && sstrm->write_id == sop->size())
				{
					int fcode = fileSwitchToRead(sop);
					if (fcode < 0)
					{
						if (st)
							delete st;
						return fcode;
					}
				}
				else
					sstrm = &invalid_file_stream;
			}
			idmemory()->setSpace(qMax(space, (uint)(sop->space())));
	    goto ens;
	    
		default:
	    code = sop->checkProc();
	    idmemory()->setSpace(qMax(space, (uint)(sop->space())));
	    sstrm = new XWPSStream;
	    code = sstrm->readProc(sop);
	    if (code < 0)
	    {
	    	if (st)
					delete st;
				goto out;
			}
	    sstrm->is_temp = 2;
	  
ens:
	    code = filterEnsureBuf(&sstrm, templat->min_in_size +  sstrm->state->templat->min_out_size, false);
	    if (code < 0)
	    {
	    	if (st)
					delete st;
				goto out;
			}
	    break;
  }
  
  if (min_size < 128)
		min_size = 512;
  code = filterOpen("r", min_size, sop, &s_filter_read_procs, templat, st);
  if (code < 0)
		goto out;
  s = sop->getStream();
  s->strm = sstrm;
  s->close_strm = close;
  pop(op - sop);
  
out:
	idmemory()->setSpace(save_space);
  return code;
}

int XWPSContextState::filterReadPredictor(int npop, const PSStreamTemplate * templat, PSStreamState * st)
{
	XWPSRef * op = op_stack.getCurrentTop();
  int predictor, code;
  PSStreamPDiffState * pds = new PSStreamPDiffState;
  PSStreamPNGPState * pps = new PSStreamPNGPState;

  if (op->hasType(XWPSRef::Dictionary)) 
  {
		if ((code = op->dictIntParam(this, "Predictor", 0, 15, 1, &predictor)) < 0)
	    return code;
		switch (predictor) 
		{
	    case 0:	
				predictor = 1;
				
	    case 1:	
				break;
				
	    case 2:	
				code = zpdsetup(op, pds);
				break;
				
	    case 10:
	    case 11:
	    case 12:
	    case 13:
	    case 14:
	    case 15:
				code = zppsetup(op, pps);
				break;
				
	    default:
	    	delete pds;
	    	delete pps;
	    	if (st)
	    		delete st;
				return (int)(XWPSError::RangeCheck);
		}
		if (code < 0)
		{
			if (st)
				delete st;
	    return code;
	  }
  } 
  else
		predictor = 1;
  if (predictor == 1)
  {
  	delete pds;
	  delete pps;
		return filterRead(npop, templat, st, 0);
	}
	
  {
		XWPSRef rsource, rdict;
		int code;
		rsource.assign(op - 1);
		rdict.assign(op);
		code = filterRead(1, templat, st, 0);
		if (code < 0)
		{
			delete pds;
	  	delete pps;
	    return code;
	  }
	    
		op = op_stack.getCurrentTop();
		if (predictor == 2)
		{
			delete pps;
			code = filterRead(0, &s_PDiffD_template, (PSStreamState *)pds, 0);
		}
		else
		{
			delete pds;
			code = filterRead(0, &s_PNGPD_template, (PSStreamState *)pps, 0);
		}
		if (code < 0) 
		{
			++op;
			op_stack.setCurrentTop(op);
	    op[-1].assign(&rsource);
	    op->assign(&rdict);
	    return code;
		}
		filterMarkStrmTemp(op, 2);
		return code;
  }
}

int XWPSContextState::filterReadSimple(const PSStreamTemplate * templat, PSStreamState * st)
{
	 return filterRead(0, templat, st, 0);
}

int XWPSContextState::filterWrite(int npop, 
	                const PSStreamTemplate * templat,
	                PSStreamState * st, 
	                uint space)
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint min_size = templat->min_in_size + PS_MAX_MIN_LEFT;
  uint save_space = idmemory()->iallocSpace();
  XWPSRef * sop = op - npop;
  XWPSStream *s;
  XWPSStream *sstrm;
  bool close = false;
  int code;

  if (sop->hasType(XWPSRef::Dictionary)) 
  {
		code = sop->checkDictRead();
		if (code < 0)
		{
			if (st)
				delete st;
			return code;
		}
			
		if ((code = sop->dictBoolParam(this, "CloseTarget", false, &close)) < 0)
		{
			if (st)
				delete st;
	    return code;
	  }
		--sop;
  }
  
  switch (sop->type()) 
  {
		case XWPSRef::String:
			idmemory()->setSpace(qMax(space, (uint)(sop->space())));
	    code = sop->checkWrite();
	    if (code < 0)
	    {
	    	if (st)
	    		delete st;
				return code;
			}
	    sstrm = new XWPSStream;
	    sstrm->cbuf_bytes = sop->value.bytes->arr;
	    if (sstrm->cbuf_bytes)
	    	sstrm->cbuf_bytes->incRef();
	    sstrm->writeString(sop->getBytes(), sop->size(), false);
	    sstrm->is_temp = 1;
	    break;
	    
		case XWPSRef::File:
			sstrm = sop->getStream();
			if (sstrm->write_id != sop->size())
			{
				int fcode = fileSwitchToWrite(sop);
				if (fcode < 0)
				{
					if (st)
	    			delete st;
					return fcode;
				}
			}
			idmemory()->setSpace(qMax(space, (uint)(sop->space())));
	    goto ens;
	    
	default:
	    sop->checkProc();
	    idmemory()->setSpace(qMax(space, (uint)(sop->space())));
	    sstrm = new XWPSStream;
	    code = sstrm->writeProc(sop);
	    if (code < 0)
	    {
				if (st)
	    		delete st;
				goto out;
			}
	    sstrm->is_temp = 2;

ens:
	    code = filterEnsureBuf(&sstrm, templat->min_out_size + sstrm->state->templat->min_in_size,true);
	    if (code < 0)
	    {
	    	if (st)
	    		delete st;
				goto out;
			}
	    break;
  }
  if (min_size < 128)
		min_size = 512;
    
  code = filterOpen("w", min_size, sop, &s_filter_write_procs, templat, st);
  if (code < 0)
		goto out;
  s = sop->getStream();
  s->strm = sstrm;
  s->close_strm = close;
  pop(op - sop);
  
out:
  idmemory()->setSpace(save_space);
  return code;
}

int XWPSContextState::filterWritePredictor(int npop,
		       								 const PSStreamTemplate * templat, 
		       								 PSStreamState * st)
{
	XWPSRef * op = op_stack.getCurrentTop();
  int predictor, code;
  PSStreamPDiffState * pds = new PSStreamPDiffState;
  PSStreamPNGPState * pps = new PSStreamPNGPState;

  if (op->hasType(XWPSRef::Dictionary)) 
  {
		if ((code = op->dictIntParam(this, "Predictor", 0, 15, 1, &predictor)) < 0)
		{
			if (st)
				delete st;
	    return code;
	  }
		switch (predictor) 
		{
	    case 0:	
				predictor = 1;
	    case 1:	
				break;
				
	    case 2:	
				code = zpdsetup(op, pds);
				break;
				
	    case 10:
	    case 11:
	    case 12:
	    case 13:
	    case 14:
	    case 15:
				code = zppsetup(op, pps);
				break;
				
	    default:
	    	delete pds;
	    	delete pps;
	    	if (st)
	    		delete st;
				return (int)(XWPSError::RangeCheck);
		}
		if (code < 0)
		{
			if (st)
				delete st;
	    return code;
	  }
  } 
  else
		predictor = 1;
  if (predictor == 1)
  {
  	delete pds;
	  delete pps;
		return filterWrite(npop, templat, st, 0);
	}
  {
		XWPSRef rtarget, rdict;
		int code;
		
		rtarget.assign(op - 1);
		rdict.assign(op);
		code = filterWrite(npop, templat, st, 0);
		if (code < 0)
		{
			delete pds;
	    delete pps;
	    return code;
	  }
		op = op_stack.getCurrentTop();
		if (predictor == 2)
		{
			delete pps;
			code = filterWrite(0, &s_PDiffE_template, (PSStreamState *)pds, 0);
		}
		else
		{
			delete pds;
			code = filterWrite(0, &s_PNGPE_template, (PSStreamState *)pps, 0);
		}
		if (code < 0) 
		{
			++op;
			op_stack.setCurrentTop(op);
	    op[-1].assign(&rtarget);
	    op->assign(&rdict);
	    return code;
		}
		filterMarkStrmTemp(op, 2);
		return code;
  }
}

int XWPSContextState::filterWriteSimple(const PSStreamTemplate * templat, PSStreamState * st)
{
	return filterWrite(0, templat, st, 0);
}

int XWPSContextState::filterZLib(PSStreamZLibState *pzls)
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = 0;

  (*s_zlibE_template.set_defaults)((PSStreamState *)pzls);
  if (op->hasType(XWPSRef::Dictionary))
		code = op->dictIntParam(this, "Effort", -1, 9, -1, &pzls->level);
  return code;
}

int XWPSContextState::bhcSetup(XWPSRef * op, PSStreamBHCState * pbhcs)
{
	int code;
  int num_counts;
  int data[max_hc_length + 1 + 256 + max_zero_run + 1];
  uint dsize;
  int i;
  uint num_values, accum;
  ushort *counts;
  ushort *values;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
	  return code;
    
  code = op->checkDictRead();
  if (code < 0)
	  return code;
	  
  if ((code = op->dictBoolParam(this, "FirstBitLowOrder", false,	&pbhcs->FirstBitLowOrder)) < 0 ||
			(code = op->dictIntParam(this, "MaxCodeLength", 1, max_hc_length, max_hc_length, &num_counts)) < 0 ||
			(code = op->dictBoolParam(this, "EndOfData", true,	&pbhcs->EndOfData)) < 0 ||
			(code = op->dictUintParam(this, "EncodeZeroRuns", 2, 256,	256, &pbhcs->EncodeZeroRuns)) < 0 ||
			(code = op->dictIntArrayParam(this, "Tables", sizeof(data) / sizeof(data[0]), data)) <= 0)
		return (code < 0 ? code : (int)(XWPSError::RangeCheck));
    
  dsize = code;
  if (dsize <= num_counts + 2)
  {
	  return (int)(XWPSError::RangeCheck);
	}
  
  for (i = 0, num_values = 0, accum = 0; i <= num_counts; i++, accum <<= 1) 
  {
		int count = data[i];

		if (count < 0)
		{
	    return (int)(XWPSError::RangeCheck);
	  }
		num_values += count;
		accum += count;
  }
  if (dsize != num_counts + 1 + num_values ||
			accum != 1 << (num_counts + 1) ||
			pbhcs->EncodeZeroRuns >	(pbhcs->EndOfData ? num_values - 1 : num_values))
	{
	  return (int)(XWPSError::RangeCheck);
	}
    
  for (; i < num_counts + 1 + num_values; i++) 
  {
		int value = data[i];

		if (value < 0 || value >= num_values)
	    return (int)(XWPSError::RangeCheck);
  }
  pbhcs->definition.counts = counts = new ushort[num_counts + 1];
  pbhcs->definition.values = values = new ushort[num_values];
  for (i = 0; i <= num_counts; i++)
		counts[i] = data[i];
  pbhcs->definition.counts = counts;
  pbhcs->definition.num_counts = num_counts;
  for (i = 0; i < num_values; i++)
		values[i] = data[i + num_counts + 1];
  pbhcs->definition.values = values;
  pbhcs->definition.num_values = num_values;
  return 0;
}

int XWPSContextState::btSetup(XWPSRef * op, PSStreamBTState * pbts)
{
	int code = op->checkReadType(XWPSRef::String);
	if (code < 0)
		return code;
		
  if (op->size() != 256)
	  return (int)(XWPSError::RangeCheck);
  memcpy(pbts->table, op->getBytes(), 256);
  return 0;
}

int XWPSContextState::bwbsSetup(XWPSRef * op, PSStreamBWBSState * pbwbss)
{
	int code = op->dictIntParam(this, "BlockSize", 1, max_int / sizeof(int) - 10, 16384, &pbwbss->BlockSize);

  if (code < 0)
		return code;
  return 0;
}
		       								 
int XWPSContextState::rlsetup(XWPSRef * dop, bool * eod)
{
	if (dop->hasType(XWPSRef::Dictionary)) 
	{
		int code;

		code = dop->checkDictRead();
		if (code < 0)
  		return code;
		if ((code = dop->dictBoolParam(this, "EndOfData", true, eod)) < 0)
	    return code;
		return 1;
  } 
  else 
  {
		*eod = true;
		return 0;
  }
}


int XWPSContextState::zAXD()
{
	PSStreamAXDState * st = new PSStreamAXDState;
	return filterReadSimple(&s_AXD_template, (PSStreamState*)st);
}

int XWPSContextState::zAXE()
{
	PSStreamAXEState * st = new PSStreamAXEState;
	return filterWriteSimple(&s_AXE_template, (PSStreamState*)st);
}

int XWPSContextState::zA85D()
{
	PSStreamA85DState * st = new PSStreamA85DState;
	return filterReadSimple(&s_A85D_template, (PSStreamState*)st);
}

int XWPSContextState::zA85E()
{
	PSStreamA85EState * st = new PSStreamA85EState;
	return filterWriteSimple(&s_A85E_template, (PSStreamState*)st);
}

static
int nobcpRequestStatus(PSStreamState * ) 
{
	return 0;
}

static
int nobcpSignalInterrupt(PSStreamState * ) 
{
	return 0;
}

int XWPSContextState::zBCPD()
{
	PSStreamBCPDState * state = new PSStreamBCPDState;

  state->signal_interrupt = &nobcpSignalInterrupt;
  state->request_status = &nobcpRequestStatus;
  return filterRead(0, &s_BCPD_template, (PSStreamState *)state, 0);
}

int XWPSContextState::zBCPE()
{
	PSStreamState * st = new PSStreamState;
	return filterWriteSimple(&s_BCPE_template, (PSStreamState*)st);
}

int XWPSContextState::zBHCD()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamBHCDState * bhcs = new PSStreamBHCDState;
  int code = bhcSetup(op, (PSStreamBHCState *)bhcs);

  if (code < 0)
  {
  	delete bhcs;
		return code;
	}
  return filterRead(0, &s_BHCD_template, (PSStreamState *)bhcs, 0);
}

int XWPSContextState::zBHCE()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamBHCEState * bhcs = new PSStreamBHCEState;
  int code = bhcSetup(op, (PSStreamBHCState *)bhcs);

  if (code < 0)
  {
  	delete bhcs;
		return code;
	}
  return filterWrite(0, &s_BHCE_template, (PSStreamState *)bhcs, 0);
}

int XWPSContextState::zBTD()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamBTState * bts = new PSStreamBTState;
  int code = btSetup(op, bts);

  if (code < 0)
  {
  	if (bts)
  		delete bts;
		return code;
	}
  return filterRead(0, &s_BTD_template, (PSStreamState *)bts, 0);
}

int XWPSContextState::zBTE()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamBTState * bts = new PSStreamBTState;
  int code = btSetup(op, bts);

  if (code < 0)
  {
  	if (bts)
  		delete bts;
		return code;
	}
  return filterWrite(0, &s_BTE_template, (PSStreamState *)bts, 0);
}

int XWPSContextState::zBWBSD()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamBWBSDState * bwbss = new PSStreamBWBSDState;
  int code = bwbsSetup(op, (PSStreamBWBSState *)bwbss);

  if (code < 0)
  {
  	if (bwbss)
  		delete bwbss;
		return code;
	}
  return filterRead(0, &s_BWBSD_template, (PSStreamState *)bwbss, 0);
}

int XWPSContextState::zBWBSE()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamBWBSEState * bwbss = new PSStreamBWBSEState;
  int code;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  {
  	if (bwbss)
  		delete bwbss;
		return code;
	}
		
  code = op->checkDictRead();
  if (code < 0)
  {
  	if (bwbss)
  		delete bwbss;
		return code;
	}
		
  code = bwbsSetup(op, (PSStreamBWBSState *)bwbss);
  if (code < 0)
  {
  	if (bwbss)
  		delete bwbss;
		return code;
	}
    
  return filterWrite(0, &s_BWBSE_template, (PSStreamState *)bwbss, 0);
}

int XWPSContextState::zCFD()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * dop;
  PSStreamCFDState * cfs = new PSStreamCFDState;
  int code;

  if (op->hasType(XWPSRef::Dictionary)) 
  {
		code = op->checkDictRead();
		dop = op;
  } 
  else
		dop = 0;
		
  code = zcfsetup(dop, (PSStreamCFState *)cfs);
  if (code < 0)
  {
  	delete cfs;
		return code;
	}
  return filterRead(0, &s_CFD_template, (PSStreamState *)cfs, 0);
}

int XWPSContextState::zCFE()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamCFEState * cfs = new PSStreamCFEState;
  int code;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  {
  	if (cfs)
  		delete cfs;
  	return code;
  }
  	
  code = op->checkDictRead();
  if (code < 0)
  {
  	if (cfs)
  		delete cfs;
  	return code;
  }
  	
  code = zcfsetup(op, (PSStreamCFState *)cfs);
  if (code < 0)
  {
  	if (cfs)
  		delete cfs;
		return code;
	}
    
  return filterWrite(0, &s_CFE_template, (PSStreamState *)cfs, 0);
}

int XWPSContextState::zcfsetup(XWPSRef * op, PSStreamCFState *pcfs)
{
	XWPSDictParamList list(false, op, NULL, false);
	
  s_CF_set_defaults_inline(pcfs);
  int code = list.putParams(this, pcfs);
  return code;
}

int XWPSContextState::zcomputeCodes()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  uint asize;
  PSHCDefinition def;
  ushort *data;
  long *freqs;
  int code = 0;

  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  code = op1->checkWriteType(XWPSRef::Array);
  if (code < 0)
		return code;
		
  asize = op1->size();
  if (op->value.intval < 1 || op->value.intval > max_hc_length)
	  return (int)(XWPSError::RangeCheck);
	  	
  def.num_counts = op->value.intval;
  if (asize < def.num_counts + 2)
	  return (int)(XWPSError::RangeCheck);
	  	
  def.num_values = asize - (def.num_counts + 1);
  data = new ushort[asize];
  freqs = new long[def.num_values];
  if (data == 0 || freqs == 0)
		code = (int)(XWPSError::VMError);
  else 
  {
		uint i;

		def.counts = data;
		def.values = data + (def.num_counts + 1);
		for (i = 0; i < def.num_values; i++) 
		{
	    XWPSRef *pf = op1->getArray() + i + def.num_counts + 1;

	    if (!pf->hasType(XWPSRef::Integer)) 
	    {
				code = (int)(XWPSError::TypeCheck);
				break;
	    }
	    freqs[i] = pf->value.intval;
		}
		if (!code) 
		{
	    code = hc_compute(&def, freqs);
	    if (code >= 0) 
	    {
	    	XWPSRef * pp = op1->getArray();
				for (i = 0; i < asize; i++)
				{
					XWPSRef * p = pp + i;
		    	p->makeInt(data[i]);
		    }
	    }
		}
  }
  if (freqs)
  	delete [] freqs;
  	
  if (data)
  	delete [] data;
  	
  if (code < 0)
		return code;
  pop(1);
  return code;
}

int XWPSContextState::zDCTD()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamDCTState * state = new PSStreamDCTState;
  PSJPEGDecompressData *jddp;
  int code;
  XWPSRef *dop;
  uint dspace;

  jddp = (PSJPEGDecompressData *)malloc(sizeof(*jddp));
  if (s_DCTD_template.set_defaults)
		(*s_DCTD_template.set_defaults) ((PSStreamState *)state);
  state->data.decompress = jddp;
 
  jddp->scanline_buffer = NULL;	
  jpeg_create_decompress(&state->data.decompress->dinfo);
  state->data.decompress->Picky = 0, state->data.decompress->Relax = 0;
		
  if (op->hasType(XWPSRef::Dictionary))
		dop = op, dspace = op->space();
  else
		dop = 0, dspace = 0;
	
	XWPSDictParamList list(false, dop, NULL, false);    
  if ((code = list.sDCTDPutParams(this, state)) < 0)
		goto rel;
		
  jddp->templat = s_DCTD_template;
  code = filterRead(0, &jddp->templat, (PSStreamState *)state, dspace);
  if (code >= 0)
		return code;
		
rel:
	
  ps_jpeg_destroy(state);
  delete state;
  if (jddp)
  	free(jddp);
  return code;
}

int XWPSContextState::zDCTE()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamDCTState * state = new PSStreamDCTState;
  PSJPEGCompressData *jcdp;
  int code;
  XWPSRef *dop;
  uint dspace;

  jcdp = (PSJPEGCompressData *)malloc(sizeof(*jcdp));
  if (s_DCTE_template.set_defaults)
		(*s_DCTE_template.set_defaults) ((PSStreamState *)state);
  state->data.compress = jcdp;
  if ((code = ps_jpeg_create_compress(state)) < 0)
  {
		ps_jpeg_destroy(state);
		if (state)
			delete state;
  	if (jcdp)
  		free(jcdp);
  	return code;
	}
		
  if (op->hasType(XWPSRef::Dictionary))
		dop = op, dspace = op->space();
  else
		dop = 0, dspace = 0;
    
  XWPSDictParamList list(false, dop, NULL, false);
  if ((code = list.sDCTEPutParams(this, state)) < 0)
  {
		ps_jpeg_destroy(state);
		if (state)
			delete state;
  	if (jcdp)
  		free(jcdp);
  	return code;
	}
		
  jcdp->templat = s_DCTE_template;
  
  state->scan_line_size = jcdp->cinfo.input_components *	jcdp->cinfo.image_width;
  jcdp->templat.min_in_size = qMax(s_DCTE_template.min_in_size, state->scan_line_size);
  jcdp->templat.min_out_size = qMax(s_DCTE_template.min_out_size, state->Markers.size);
  code = filterWrite(0, &jcdp->templat,	(PSStreamState *)state, dspace);
  
  ps_jpeg_destroy(state);
  if (jcdp)
  	free(jcdp);
  return code;
}

int XWPSContextState::zFlateD()
{
	PSStreamZLibState * zls = new PSStreamZLibState;

  (*s_zlibD_template.set_defaults)((PSStreamState *)zls);
  return filterReadPredictor(0, &s_zlibD_template, (PSStreamState *)zls);
}

int XWPSContextState::zFlateE()
{
	PSStreamZLibState * zls = new PSStreamZLibState;
  int code = filterZLib(zls);

  if (code < 0)
  {
  	if (zls)
  		delete zls;
		return code;
	}
  return filterWritePredictor(0, &s_zlibE_template, (PSStreamState *)zls);
}

int XWPSContextState::zLZWD()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamLZWState * lzs = new PSStreamLZWState;
  int code = zlzsetup(op, lzs);

  if (code < 0)
  {
  	if (lzs)
  		delete lzs;
		return code;
	}
    
  if ((language_level >= 3) && op->hasType(XWPSRef::Dictionary)) 
  {
		int unit_size;

		if ((code = op->dictBoolParam(this, "LowBitFirst", lzs->FirstBitLowOrder, &lzs->FirstBitLowOrder)) < 0 ||
	    	(code = op->dictIntParam(this, "UnitSize", 3, 8, 8,   &unit_size)) < 0)
	  {
	  	if (lzs)
  			delete lzs;
	    return code;
	  }
		if (code == 0 )
	    lzs->InitialCodeLength = unit_size + 1;
  }
  return filterReadPredictor(0, &s_LZWD_template, (PSStreamState *)lzs);
}

int XWPSContextState::zLZWE()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamLZWState * lzs = new PSStreamLZWState;
  int code = zlzsetup(op, lzs);

  if (code < 0)
  {
  	if (lzs)
  		delete lzs;
		return code;
	}
    
  return filterWritePredictor(0, &s_LZWE_template, (PSStreamState *)lzs);
}

int XWPSContextState::zMD5E()
{
	PSStreamMD5EState * st = new PSStreamMD5EState;
	return filterWriteSimple(&s_MD5E_template, (PSStreamState*)st);
}

int XWPSContextState::zMTFD()
{
//	XWPSRef * op = op_stack.getCurrentTop();
	PSStreamMTFState * st = new PSStreamMTFState;
  return filterReadSimple(&s_MTFD_template, (PSStreamState*)st);
}

int XWPSContextState::zMTFE()
{
//	XWPSRef * op = op_stack.getCurrentTop();
	PSStreamMTFState * st = new PSStreamMTFState;
  return filterWriteSimple(&s_MTFE_template, (PSStreamState*)st);
}

int XWPSContextState::zNullE()
{
	PSStreamState * st = new PSStreamState;
	return filterWriteSimple(&s_NullE_template, (PSStreamState*)st);
}

int XWPSContextState::zPDiffD()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamPDiffState * pds = new PSStreamPDiffState;
  int code = zpdsetup(op, pds);

  if (code < 0)
  {
  	if (pds)
  		delete pds;
		return code;
	}
  return filterRead(0, &s_PDiffD_template, (PSStreamState *)pds, 0);
}

int XWPSContextState::zPDiffE()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamPDiffState * pds = new PSStreamPDiffState;
  int code = zpdsetup(op, pds);

  if (code < 0)
  {
  	if (pds)
  		delete pds;
		return code;
	}
  return filterWrite(0, &s_PDiffE_template, (PSStreamState *) pds, 0);
}

int XWPSContextState::zpdsetup(XWPSRef * op, PSStreamPDiffState * ppds)
{
	int code, bpc;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = op->checkDictRead();
  if (code < 0)
  	return code;
  	
  if ((code = op->dictIntParam(this, "Colors", 1, s_PDiff_max_Colors, 1, &ppds->Colors)) < 0 ||
			(code = op->dictIntParam(this, "BitsPerComponent", 1, 8, 8, &bpc)) < 0 ||
			(bpc & (bpc - 1)) != 0 ||
			(code = op->dictIntParam(this, "Columns", 1, max_int, 1, &ppds->Columns)) < 0)
		return (code < 0 ? code : (int)(XWPSError::RangeCheck));
  ppds->BitsPerComponent = bpc;
  return 0;
}

int XWPSContextState::zPFBD()
{
	XWPSRef * sop = op_stack.getCurrentTop();
  PSStreamPFBDState * state = new PSStreamPFBDState;

  int code = sop->checkType(XWPSRef::Boolean);
  if (code < 0)
  {
  	if (state)
  		delete state;
  	return code;
  }
    
  state->binary_to_hex = sop->value.boolval;
  return filterRead(1, &s_PFBD_template, (PSStreamState *)state, 0);
}

int XWPSContextState::zPNGPD()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamPNGPState * pps = new PSStreamPNGPState;
  int code = zppsetup(op, pps);

  if (code < 0)
  {
  	if (pps)
  		delete pps;
		return code;
	}
  return filterRead(0, &s_PNGPD_template, (PSStreamState *)pps, 0);
}

int XWPSContextState::zPNGPE()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamPNGPState * pps = new PSStreamPNGPState;
  int code = zppsetup(op, pps);

  if (code < 0)
  {
  	if (pps)
  		delete pps;
		return code;
	}
  return filterWrite(0, &s_PNGPE_template, (PSStreamState *) pps, 0);
}

int XWPSContextState::zppsetup(XWPSRef * op, PSStreamPNGPState * ppps)
{
	int code, bpc;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = op->checkDictRead();
  if (code < 0)
  	return code;
  	
  if ((code = op->dictIntParam(this, "Colors", 1, 16, 1, &ppps->Colors)) < 0 ||
			(code = op->dictIntParam(this, "BitsPerComponent", 1, 16, 8,  &bpc)) < 0 ||
			(bpc & (bpc - 1)) != 0 ||
			(code = op->dictUintParam(this, "Columns", 1, max_uint, 1,&ppps->Columns)) < 0 ||
			(code = op->dictIntParam(this, "Predictor", 10, 15, 15, &ppps->Predictor)) < 0)
		return (code < 0 ? code : (int)(XWPSError::RangeCheck));
  ppps->BitsPerComponent = bpc;
  return 0;
}

int XWPSContextState::zPSSE()
{
	PSStreamState * st = new PSStreamState;
	return filterWriteSimple(&s_PSSE_template, (PSStreamState*)st);
}

int XWPSContextState::zRLD()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamRLDState * state = new PSStreamRLDState;
  int code = rlsetup(op, &state->EndOfData);

  if (code < 0)
  {
  	if (state)
  		delete state;
		return code;
	}
  return filterRead(0, &s_RLD_template, (PSStreamState *)state, 0);
}

int XWPSContextState::zRLE()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamRLEState * state = new PSStreamRLEState;
  int code;

  code = checkOp(op,2);
  if (code < 0)
  {
  	if (state)
  		delete state;
  	return code;
  }
  	
  code = rlsetup(op - 1, &state->EndOfData);
  if (code < 0)
		return code;
    
  code = op->checkIntLEU(max_uint);
  state->record_size = op->value.intval;
  return filterWrite(1, &s_RLE_template, (PSStreamState *) state, 0);
}

int XWPSContextState::zSFD()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamSFDState * state = new PSStreamSFDState;
  XWPSRef *sop = op;
  int npop;
  int code;

  if (s_SFD_template.set_defaults)
		s_SFD_template.set_defaults((PSStreamState *)state);
  if ((language_level >= 3) && op->hasType(XWPSRef::Dictionary)) 
  {
		int count;
		code = op->checkDictRead();
		if (code < 0)
		{
			if (state)
				delete state;
			return code;
		}
		
		if ((code = op->dictIntParam(this, "EODCount", 0, max_int, -1, &count)) < 0)
		{
			if (state)
				delete state;
	    return code;
	  }
		if (op->dictFindString(this, "EODString", &sop) <= 0)
		{
			if (state)
				delete state;
	    return (int)(XWPSError::RangeCheck);
	  }
		state->count = count;
		npop = 0;
  } 
  else 
  {
		sop[-1].checkType(XWPSRef::Integer);
		if (sop[-1].value.intval < 0)
		{
			if (state)
				delete state;
	    return (int)(XWPSError::RangeCheck);
	  }
		state->count = sop[-1].value.intval;
		npop = 2;
  }
  
  code = sop->checkReadType(XWPSRef::String);
  if (code < 0)
  {
  	if (state)
				delete state;
		return code;
	}
  state->eod.data = sop->getBytes();
  state->eod.size = sop->size();
  return filterRead(npop, &s_SFD_template,  (PSStreamState *)state, sop->space());
}

int XWPSContextState::zTBCPD()
{
	PSStreamBCPDState * state = new PSStreamBCPDState;

  state->signal_interrupt = &nobcpSignalInterrupt;
  state->request_status = &nobcpRequestStatus;
  return filterRead(0, &s_TBCPD_template, (PSStreamState *)state, 0);
}

int XWPSContextState::zTBCPE()
{
	PSStreamState * st = new PSStreamState;
	return filterWriteSimple(&s_TBCPE_template, (PSStreamState*)st);
}

int XWPSContextState::zzlibD()
{
	PSStreamZLibState * zls = new PSStreamZLibState;

  (*s_zlibD_template.set_defaults)((PSStreamState *)zls);
  return filterRead(0, &s_zlibD_template, (PSStreamState *)zls, 0);
}

int XWPSContextState::zzlibE()
{
	PSStreamZLibState * zls = new PSStreamZLibState;
  int code = filterZLib(zls);

  if (code < 0)
  {
  	if (zls)
  		delete zls;
		return code;
	}
  return filterWrite(0, &s_zlibE_template, (PSStreamState *)zls, 0);
}

int XWPSContextState::zlzsetup(XWPSRef * op, PSStreamLZWState * plzs)
{
	int code;
  XWPSRef *dop;

  if (op->hasType(XWPSRef::Dictionary)) 
  {
		op->checkDictRead();
		dop = op;
  } 
  else
		return 0;
    
  if ((code = dop->dictIntParam(this, "EarlyChange", 0, 1, 1, &plzs->EarlyChange)) < 0 ||
	   	(code = dop->dictIntParam(this, "InitialCodeLength", 2, 11, 8, &plzs->InitialCodeLength)) < 0 ||
	   	(code = dop->dictBoolParam(this, "FirstBitLowOrder", false,  &plzs->FirstBitLowOrder)) < 0 ||
	   	(code = dop->dictBoolParam(this, "BlockData", false, &plzs->BlockData)) < 0)
		return code;
  return 0;
}
