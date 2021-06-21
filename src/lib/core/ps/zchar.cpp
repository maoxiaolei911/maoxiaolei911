/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSFont.h"
#include "XWPSDevice.h"
#include "XWPSText.h"
#include "XWPSState.h"
#include "XWPSContextState.h"

class XWPSType1execState : public XWPSStruct
{
public:
	XWPSType1execState();
	~XWPSType1execState();
	
	void copy(XWPSType1execState * other);
	
	int getLength();
	const char * getTypeName();
	
public:
	XWPSType1State cis;
	XWPSContextState *i_ctx_p;	
	double sbw[4];
  int present;
  XWPSRect char_bbox;
  bool use_FontBBox_as_Metrics2;
  XWPSRef save_args[6];
  int num_args;
};

XWPSType1execState::XWPSType1execState()
	:XWPSStruct()
{
	i_ctx_p = 0;
	present = 0;
	use_FontBBox_as_Metrics2 = false;
	num_args = 0;
}

XWPSType1execState::~XWPSType1execState()
{
}

void XWPSType1execState::copy(XWPSType1execState * other)
{
	cis.copy(&other->cis);	
	i_ctx_p = other->i_ctx_p;
	memcpy(sbw, other->sbw, 4 * sizeof(double));
	present = other->present;
	char_bbox = other->char_bbox;
	use_FontBBox_as_Metrics2 = other->use_FontBBox_as_Metrics2;
	num_args = other->num_args;
	for (int i = 0; i < num_args; i++)
	{
		save_args[i].assign(&other->save_args[i]);
	}
}

int XWPSType1execState::getLength()
{
	return sizeof(XWPSType1execState);
}

const char * XWPSType1execState::getTypeName()
{
	return "type1execstate";
}
	

int XWPSContextState::bboxContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int npop = (op->hasType(XWPSRef::String) ? 4 : 6);
  int code = type1CalloutDispatch(&XWPSContextState::bboxContinue, npop);

  if (code == 0) 
  {
		op = op_stack.getCurrentTop();
		npop -= 4;
		pop(npop);
		op -= npop;
		opType1Free();
  }
  return code;
}

int XWPSContextState::bboxDraw(int (XWPSState::*draw)())
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRect bbox;
  XWPSFont *pfont;
  XWPSTextEnum *penum;
  XWPSFontBase * pbfont;
  XWPSFontType1 * pfont1;
  XWPSType1execState cxs;
  int code;

  if (pgs->in_cachedevice < 2)
		return nobboxDraw(draw);
  if ((code = fontParam(op - 3, &pfont)) < 0)
		return code;
  penum = opShowFind();
  if (penum == 0 || !pfont->usesCharStrings())
		return (int)(XWPSError::Undefined);
    
  if ((code = pgs->upathBbox(&bbox, false)) < 0) 
  {
		if (code == XWPSError::UndefinedResult) 
		{
	    pop(4);
	    pgs->newPath();
	    return 0;
		}
		return code;
  }
  
  if (draw == &XWPSState::stroke) 
  {
		float width = pgs->currentLineWidth() * 1.41422;

		bbox.p.x -= width, bbox.p.y -= width;
		bbox.q.x += width, bbox.q.y += width;
  }
  
  pbfont = (XWPSFontBase *)pfont;
  if (bbox.within(pbfont->FontBBox))
		return nobboxDraw(draw);
		
  pbfont->FontBBox.merge(bbox);
  penum->retry();
  pfont1 = (XWPSFontType1 *) pfont;
  code = zcharGetMetrics(pbfont, op - 1, cxs.sbw);
  if (code < 0)
		return code;
  cxs.present = code;
  code = cxs.cis.type1ExecInit(penum, pgs, pfont1);
  if (code < 0)
		return code;
  cxs.char_bbox = pfont1->FontBBox;
  return type1ExecBbox(&cxs, pfont);
}

int XWPSContextState::bboxFill()
{
	return bboxDraw(&XWPSState::eofill);
}

int XWPSContextState::bboxFinish(int (XWPSContextState::*cont)())
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSFont *pfont;
  int code;
  XWPSTextEnum *penum = opShowFind();
  XWPSType1execState cxs;	
  XWPSType1State *const pcis = &cxs.cis;
  double sbxy[2];
  XWPSPoint sbpt;
  XWPSPoint *psbpt = 0;
  XWPSRef * opc = op;
  XWPSRef *opstr;
  XWPSRef other_subr;

  if (!opc->hasType(XWPSRef::String)) 
  {
		code = checkOp(op,3);
		if (code < 0)
	    return code;
		code = numParams(op, 2, sbxy);
		if (code < 0)
	    return code;
		sbpt.x = sbxy[0];
		sbpt.y = sbxy[1];
		psbpt = &sbpt;
		opc -= 2;
		code = opc->checkType(XWPSRef::String);
  }
  code = fontParam(op - 3, &pfont);
  if (code < 0)
		return code;
    
  if (penum == 0 || !pfont->usesCharStrings())
		return (int)(XWPSError::Undefined);
  
  {
		XWPSFontType1 *const pfont1 = (XWPSFontType1 *) pfont;
		int lenIV = pfont1->lenIV;

		if (lenIV > 0 && opc->size() <= lenIV)
			return (int)(XWPSError::InvalidFont);
		code = checkEStack(5);
		if (code < 0)
			return code;
		code = pcis->type1ExecInit(penum, pgs, pfont1);
		if (code < 0)
	    return code;
		if (psbpt)
	    pcis->setLSB(psbpt);
  }
  
  opstr = opc;
  
icont:
  code = type1ContinueDispatch(&cxs, opstr, &other_subr, (psbpt ? 6 : 4));
  op = op_stack.getCurrentTop();
  
  XWPSRef * esp = exec_stack.getCurrentTop();
  switch (code) 
  {
		case 0:	
	    if (psbpt)
				pop(2);
	    return (this->*cont)();
	    
		case type1_result_callothersubr:
			esp = exec_stack.incCurrentTop(1);
			esp->makeOper(0, cont);
	    return type1CallOtherSubr(&cxs, &XWPSContextState::bboxContinue,&other_subr);
	    	
		case type1_result_sbw:
	    opstr = 0;
	    goto icont;
	    
		default:
	    return code;
  }
}

int XWPSContextState::bboxFinishFill()
{
	return bboxFinish(&XWPSContextState::bboxFill);
}

int XWPSContextState::bboxFinishStroke()
{
	return bboxFinish(&XWPSContextState::bboxStroke);
}

int XWPSContextState::bboxGetSbwContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef other_subr;
  XWPSRef * esp = exec_stack.getCurrentTop();
  XWPSType1execState *pcxs = (XWPSType1execState*)(esp->getStruct());
  XWPSType1State *const pcis = &pcxs->cis;
  int code;

  code = type1ContinueDispatch(pcxs, NULL, &other_subr, 4);
  op = op_stack.getCurrentTop();
  switch (code) 
  {
		default:
	    opType1Free();
	    return ((code < 0 ? code : (int)(XWPSError::InvalidFont)));
	    	
		case type1_result_callothersubr:
	    return type1PushOtherSubr(pcxs, &XWPSContextState::bboxGetSbwContinue,	&other_subr);
	    	
		case type1_result_sbw: 
			{
	    	double sbw[4];
	    	const XWPSFontBase *const pbfont = (const XWPSFontBase *)pcis->pfont;
	    	XWPSRect bbox;
	    	pcis->type1CisGetMetrics(sbw);
	    	bbox = pcxs->char_bbox;
	    	opType1Free();
	    	return zcharSetCache(pbfont, op, sbw, sbw + 2, &bbox,
				   &XWPSContextState::bboxFinishFill, &XWPSContextState::bboxFinishStroke, NULL);
		}
  }
}

int XWPSContextState::bboxStroke()
{
	return bboxDraw(&XWPSState::stroke);
}

int XWPSContextState::charStringExecChar(int font_type_mask)
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSFont *pfont;
  int code = fontParam(op - 3, &pfont);
  XWPSFontBase *const pbfont = (XWPSFontBase *) pfont;
  XWPSFontType1 *const pfont1 = (XWPSFontType1 *) pfont;
  XWPSTextEnum *penum = opShowFind();
  XWPSType1execState cxs;
  XWPSType1State *const pcis = &cxs.cis;

  if (code < 0)
		return code;
    
  if (penum == 0 ||	pfont->FontType >= sizeof(font_type_mask) * 8 ||
			!(font_type_mask & (1 << (int)pfont->FontType)))
	{
		return (int)(XWPSError::Undefined);
	}
   
  if (pfont->PaintType)
		pgs->setLineWidth(pfont->StrokeWidth);
  
  code = checkEStack(3);
  if (code < 0)
		return code;
		
  if (op->isProc())
		return zcharExecCharProc();
		
  code = op->checkType(XWPSRef::String);
  if (code < 0)
		return code;
		
  if (op->size() <= qMax(pfont1->lenIV, 0))
		return (int)(XWPSError::InvalidFont);
	
  code = zcharGetMetrics(pbfont, op - 1, cxs.sbw);
  if (code < 0)
		return code;
    
  cxs.present = code;
  code = pgs->moveTo(0.0, 0.0);
  if (code < 0)
		return code;
    
  code = pcis->type1ExecInit(penum, pgs, pfont1);
  if (code < 0)
		return code;
    
  pcis->setCallbackData(&cxs);
  if (pfont1->FontBBox.q.x > pfont1->FontBBox.p.x && pfont1->FontBBox.q.y > pfont1->FontBBox.p.y) 
  {
		cxs.char_bbox = pfont1->FontBBox;
		return type1ExecBbox(&cxs, pfont);
  } 
  else 
  {
		XWPSRef *opstr = op;
		XWPSRef other_subr;

		if (cxs.present == metricsSideBearingAndWidth) 
		{
	    XWPSPoint sbpt;

	    sbpt.x = cxs.sbw[0], sbpt.y = cxs.sbw[1];
	    pcis->setLSB(&sbpt);
		}
		
icont:
		code = type1ContinueDispatch(&cxs, opstr, &other_subr, 4);
		op = op_stack.getCurrentTop();
		switch (code) 
		{
	    case 0:		/* all done */
			return nobboxFinish(&cxs);
			
	    default:
				return code;
				
	    case type1_result_callothersubr:
				return type1CallOtherSubr(&cxs, &XWPSContextState::nobboxContinue,  &other_subr);
					
	    case type1_result_sbw:
				if (cxs.present != metricsSideBearingAndWidth) 
				{
		    	if ((penum->FontBBox_as_Metrics2.x == 0 &&
			 				penum->FontBBox_as_Metrics2.y == 0) ||
		        	pgs->rootFont()->WMode == 0) 
		      {
 		        pcis->type1CisGetMetrics(cxs.sbw);
		        cxs.use_FontBBox_as_Metrics2 = false;
	        } 
	        else 
	        {
		        cxs.sbw[0] = penum->FontBBox_as_Metrics2.x / 2;
		        cxs.sbw[1] = penum->FontBBox_as_Metrics2.y;
		        cxs.sbw[2] = 0;
		        cxs.sbw[3] = -penum->FontBBox_as_Metrics2.x; /* Sic! */
		        cxs.use_FontBBox_as_Metrics2 = true;
						cxs.present = metricsSideBearingAndWidth;
		    	}
				}
				opstr = 0;
				goto icont;
				
		}
  }
}

int  XWPSContextState::charStringMakeNotdef(XWPSString *pstr, XWPSFont *font)
{
	const XWPSFontType1 *const pfont = (const XWPSFontType1 *)font;
  static const uchar char_data[4] = { 139,	139,	c1_hsbw, cx_endchar  };
  uint len = qMax(pfont->lenIV, 0) + sizeof(char_data);
  uchar*chars = (uchar*)malloc((len+1) * sizeof(uchar)); 
  pstr->data = chars;
  pstr->size = len;
  if (pfont->lenIV < 0)
		memcpy(chars, char_data, sizeof(char_data));
  else 
  {
		ushort state = crypt_charstring_seed;

		memcpy(chars + pfont->lenIV, char_data, sizeof(char_data));
		ps_type1_encrypt(chars, chars, len, &state);
  }
  if (font->not_def)
  	free(font->not_def);
  font->not_def = chars;
  return 0;
}

bool XWPSContextState::charStringIsNotdefProc(XWPSRef *pcstr)
{
	if (pcstr->isArray() && pcstr->size() == 4) 
	{
		XWPSRef elts[4];
		long i;

		for (i = 0; i < 4; ++i)
	    pcstr->arrayGet(this, i, &elts[i]);
			if (elts[0].hasType(XWPSRef::Name) &&
	    		elts[1].hasType(XWPSRef::Integer) && elts[1].value.intval == 0 &&
	    		elts[2].hasType(XWPSRef::Integer) && elts[2].value.intval == 0 &&
	    		elts[3].hasType(XWPSRef::Name)) 
	   	{
	    	XWPSRef nref;

	    	nameEnterString("pop", &nref);
	    	if (nameEq(&elts[0], &nref)) 
	    	{
					nameEnterString("setcharwidth", &nref);
					if (nameEq(&elts[3], &nref))
		    		return true;
	    	}
			}
  }
  return false;
}

int XWPSContextState::finishStringWidth()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSPoint width;

	XWPSRef * esp = exec_stack.getCurrentTop();
	XWPSTextEnum *penum = (XWPSTextEnum*)(esp->getStruct());
  penum->totalWidth(&width);
  int code = push(&op, 2);
  if (code < 0)
		return code;
  op[-1].makeReal(width.x);
  op->makeReal(width.y);
  return 0;
}

int XWPSContextState::fontBboxParam(XWPSRef * pfdict, double bbox[4])
{
	XWPSRef *pbbox;
	bbox[0] = bbox[1] = bbox[2] = bbox[3] = 0.0;
  if (pfdict->dictFindString(this, "FontBBox", &pbbox) > 0) 
  {
		if (!pbbox->isArray())
	    return (int)(XWPSError::TypeCheck);
		
		if (pbbox->size() == 4) 
		{
	    ushort *pbe = pbbox->getPacked();
	    XWPSRef rbe[4];
	    int i;
	    int code;
	    float dx, dy, ratio;

	    for (i = 0; i < 4; i++) 
	    {
				packedGet(pbe, rbe + i);
				pbe = packed_next(pbe);
	    }
	    if ((code = numParams(rbe + 3, 4, bbox)) < 0)
				return code;
	    dx = bbox[2] - bbox[0];
	    dy = bbox[3] - bbox[1];
	    if (dx <= 0 || dy <= 0 ||
					(ratio = dy / dx) < 0.125 || ratio > 8.0)
				bbox[0] = bbox[1] = bbox[2] = bbox[3] = 0.0;
		}
  }
  return 0;
}

void XWPSContextState::glyphRef(ulong glyph, XWPSRef * gref)
{
	if (glyph < ps_min_cid_glyph)
		nameIndexRef(glyph, gref);
  else
		gref->makeInt(glyph - ps_min_cid_glyph);
}

int XWPSContextState::glyphShowSetup(ulong *pglyph)
{
	XWPSRef * op = op_stack.getCurrentTop();

  switch (pgs->currentFont()->FontType) 
  {
		case ft_CID_encrypted:
		case ft_CID_user_defined:
		case ft_CID_TrueType:
		case ft_CID_bitmap:
	    op->checkIntLEU(max_ulong - ps_min_cid_glyph);
	    *pglyph = (ulong) op->value.intval + ps_min_cid_glyph;
	    break;
	    
		default:
	    op->checkType(XWPSRef::Name);
	    *pglyph = nameIndex(op);
  }
  return opShowEnumSetup();
}

bool XWPSContextState::mapGlyphToChar(XWPSRef * pgref, XWPSRef * pencoding, XWPSRef * pch)
{
	uint esize = pencoding->size();
  uint ch;
  XWPSRef eref;

  for (ch = 0; ch < esize; ch++) 
  {
		pencoding->arrayGet(this, (long)ch, &eref);
		if (pgref->objEq(this, &eref)) 
		{
	    pch->makeInt(ch);
	    return true;
		}
  }
  return false;
}

int  XWPSContextState::moveShow(bool have_x, bool have_y)
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSTextEnum *penum = 0;
  int code = opShowSetup(op - 1);
  int format;
  uint i, size;
  float *values = 0;

  if (code != 0)
		return code;
    
  format = op->numArrayFormat();
  if (format < 0)
		return format;
    
  size = op->numArraySize(format);
  if (size > 0)
    values = new float[size];
  for (i = 0; i < size; ++i) 
  {
		XWPSRef value;

		switch (code = op->numArrayGet(this, format, i, &value)) 
		{
			case XWPSRef::Integer:
	    	values[i] = value.value.intval; 
	    	break;
	    	
			case XWPSRef::Real:
	    	values[i] = value.value.realval; 
	    	break;
	    	
			case XWPSRef::Null:
	    	code = (int)(XWPSError::RangeCheck);
			default:
				if (values)
	    	  delete [] values;
	    	return code;
		}
  }
  
  code = pgs->xyshowBegin(op[-1].getBytes(), op[-1].size(), (have_x ? values : (float *)0), 
  		(have_y ? values : (float *)0),	size, &penum);
  if (code < 0)
  {
  	if (values)
  	  delete [] values;
		return code;
  }
  penum->text.p_bytes = op[-1].value.bytes->arr;
  if (op[-1].value.bytes->arr)
  	op[-1].value.bytes->arr->incRef();
  if ((code = opShowFinishSetup(penum, 2, NULL)) < 0)
 	{
 		if (values)
		  delete [] values;
		return code;
  }
  
  pop(2);
  return opShowContinue();
}

int  XWPSContextState::nobboxContinue()
{
	int code = type1CalloutDispatch(&XWPSContextState::nobboxContinue, 4);

  if (code)
		return code;
  {
		XWPSRef * esp = exec_stack.getCurrentTop();
		XWPSType1execState *pcxs = (XWPSType1execState*)(esp->getStruct());
		XWPSType1execState cxs;

		cxs.copy(pcxs);
		cxs.cis.setCallbackData(&cxs);
		opType1Free();
		return nobboxFinish(&cxs);
  }
}

int  XWPSContextState::nobboxDraw(int (XWPSState::*draw)())
{
	int code = (pgs->*draw)();

  if (code >= 0)
		pop(4);
  return code;
}

int  XWPSContextState::nobboxFill()
{
	return nobboxDraw(&XWPSState::eofill);
}

int  XWPSContextState::nobboxFinish(XWPSType1execState * pcxs)
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  XWPSTextEnum *penum = opShowFind();
  XWPSFont *pfont;

  if ((code = pgs->upathBbox(&pcxs->char_bbox, false)) < 0 ||
			(code = fontParam(op - 3, &pfont)) < 0)
		return code;
    
  if (penum == 0 || !pfont->usesCharStrings())
		return (int)(XWPSError::Undefined);
  
  {
		XWPSFontBase *const pbfont = (XWPSFontBase *) pfont;
		XWPSFontType1 *const pfont1 = (XWPSFontType1 *) pfont;

		if (pcxs->present == metricsNone) 
		{
	    XWPSPoint endpt;

	    if ((code = pgs->currentPoint(&endpt)) < 0)
				return code;
	    pcxs->sbw[2] = endpt.x, pcxs->sbw[3] = endpt.y;
	    pcxs->present = metricsSideBearingAndWidth;
		}
		
		if (pgs->device->getAlphaBits(go_text) > 1) 
		{
	    pgs->newPath();
	    pgs->moveTo(0.0, 0.0);
	    code = pcxs->cis.type1ExecInit(penum, pgs, pfont1);
	    if (code < 0)
				return code;
	    return type1ExecBbox(pcxs, pfont);
		}
		return zcharSetCache(pbfont, op, NULL, pcxs->sbw + 2, &pcxs->char_bbox,
			       		&XWPSContextState::nobboxFill, &XWPSContextState::nobboxStroke,   
			       		(pcxs->use_FontBBox_as_Metrics2 ? pcxs->sbw : NULL));
	}
}

int  XWPSContextState::nobboxStroke()
{
	return nobboxDraw(&XWPSState::stroke);
}

int XWPSContextState::opShowCleanup()
{
	return opShowRestore(true);
}

int XWPSContextState::opShowContinue()
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	XWPSTextEnum *penum = (XWPSTextEnum*)(esp->getStruct());
	return opShowContinueDispatch(0, penum->process());
}

int XWPSContextState::opShowContinueDispatch(int npop, int code)
{
	XWPSRef * op = op_stack.getCurrentTop() - npop;
	XWPSRef * esp = exec_stack.getCurrentTop();
	XWPSTextEnum *penum = (XWPSTextEnum*)(esp->getStruct());

  switch (code) 
  {
		case 0: 
			{		/* all done */
	    	XWPSRef * save_osp = op_stack.getCurrentTop();

	    	op_stack.setCurrentTop(op);
	    	esp = exec_stack.getCurrentTop();
	    	code = esp[-7].callProc(this);
	    	opShowFree(code);
	    	if (code < 0) 
	    	{
					op_stack.setCurrentTop(save_osp);
					return code;
	    	}
	    	return PS_O_POP_ESTACK;
			}
			
		case TEXT_PROCESS_INTERVENE: 
			{
				esp = exec_stack.getCurrentTop();
	    	XWPSRef *pslot = esp - 1;

	    	code = push(&op, 2);
	    	if (code < 0)
					return code;
	    	op[-1].makeInt(penum->currentChar());
	    	op->makeInt(penum->nextChar());
	    	esp = exec_stack.incCurrentTop(1);
	    	esp->makeOper(0,&XWPSContextState::opShowContinue);
	    	esp = exec_stack.incCurrentTop(1);
	    	esp->assign(pslot);	
	    	return PS_O_PUSH_ESTACK;
			}
			
		case TEXT_PROCESS_RENDER: 
			{
	    	XWPSFont *pfont = pgs->currentFont();
	    	XWPSFontData *pfdata = (XWPSFontData*)(pfont->client_data);
	    	ulong chr  = penum->currentChar();
	    	ulong glyph = penum->currentGlyph();

	    	code = push(&op, 2);
	    	if (code < 0)
					return code;
	    	op[-1].assign(&pfdata->dict);	    	
	    	if (pfont->FontType == ft_user_defined) 
	    	{
					if (level2Enabled() &&  !pfdata->BuildGlyph.hasType(XWPSRef::Null) && glyph != ps_no_glyph) 
					{
		    		glyphRef(glyph, op);
		    		esp = exec_stack.getCurrentTop();
		    		esp[2].assign(&pfdata->BuildGlyph);
					} 
					else if (pfdata->BuildChar.hasType(XWPSRef::Null))
		    		goto err;
					else if (chr == ps_no_char) 
					{
		    		XWPSRef gref;
		    		XWPSRef *pencoding = &pfdata->Encoding;

		    		glyphRef(glyph, &gref);
		    		if (!mapGlyphToChar(&gref, pencoding, op)	) 
		    		{	
							nameEnterString(".notdef", &gref);
							if (!mapGlyphToChar(&gref, pencoding, op))
			    			goto err;
		    		}
		    		esp = exec_stack.getCurrentTop();
		    		esp[2].assign(&pfdata->BuildChar);
					} 
					else 
					{
		    		op->makeInt(chr & 0xff);
		    		esp = exec_stack.getCurrentTop();
		    		esp[2].assign(&pfdata->BuildChar);
					}
	    	} 
	    	else 
	    	{
					XWPSRef eref, gref;

					if (chr != ps_no_char &&  !pfdata->BuildChar.hasType(XWPSRef::Null) &&
		    		(glyph == ps_no_glyph || (pfdata->Encoding.arrayGet(this, (long)(chr & 0xff), &eref) >= 0 &&
		      	(glyphRef(glyph, &gref), gref.objEq(this, &eref))))) 
		    	{
		    		op->makeInt(chr & 0xff);
		    		esp = exec_stack.getCurrentTop();
		    		esp[2].assign(&pfdata->BuildChar);
					} 
					else 
					{
		    		if (glyph == ps_no_glyph)
							op->makeInt(0);
		    		else
							glyphRef(glyph, op);
						esp = exec_stack.getCurrentTop();
		    		esp[2].assign(&pfdata->BuildGlyph);
					}
	    	}
	    	
	    	esp = exec_stack.getCurrentTop();
	    	esp[-2].value.intval = op_stack.count() - 2;
	    	esp[-3].value.intval = dict_stack.count();
	    	esp = exec_stack.incCurrentTop(1);
	    	esp->makeOper(0,&XWPSContextState::opShowContinue);
	    	esp = exec_stack.incCurrentTop(1);
	    	return PS_O_PUSH_ESTACK;
			}
		
		default:
err:
	    if (code >= 0)
				code = (int)(XWPSError::InvalidFont);
	    return opShowFree(code);
  }
}

int XWPSContextState::opShowContinuePop(int npop)
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	XWPSTextEnum *penum = (XWPSTextEnum*)(esp->getStruct());	
	return opShowContinueDispatch(npop, penum->process());
}

int XWPSContextState::opShowEnumSetup()
{
	checkEStack(9 + 2);
  return 0;
}

XWPSTextEnum * XWPSContextState::opShowFind()
{
	uint index = opShowFindIndex();

  if (index == 0)
		return 0;	
  
  XWPSRef * ep = exec_stack.index(index - (9 - 1));
  return (XWPSTextEnum*)(ep->getStruct());
}

uint XWPSContextState::opShowFindIndex()
{
	XWPSRefStackEnum rsenum;
  uint count = 0;

  rsenum.begin(exec_stack.getStack());
  do 
 	{
		XWPSRef * ep = rsenum.ptr;
		uint size = rsenum.size;

		for (ep += size - 1; size != 0; size--, ep--, count++)
	    if (ep->isEStackMark() && ep->size() == PS_ES_SHOW)
			return count;
  } while (rsenum.next());
  return 0;	
}

int XWPSContextState::opShowFinishSetup(XWPSTextEnum * penum, int , op_proc_t endproc)
{
	 XWPSTextEnum *osenum = opShowFind();
	 XWPSRef * esp = exec_stack.getCurrentTop();
   XWPSRef * ep = esp + 9;
   ulong glyph;
   
   if (osenum && osenum->showIsAllOf(TEXT_FROM_STRING | TEXT_DO_NONE | TEXT_INTERVENE) &&
				penum->showIsAllOf(TEXT_FROM_STRING | TEXT_RETURN_WIDTH) &&
				(glyph = osenum->currentGlyph()) != ps_no_glyph &&
				glyph >= ps_min_cid_glyph) 
	{
		XWPSTextParams text;

		if (!(penum->text.size == 1 &&  penum->text.data.bytes[0] == (osenum->currentChar() & 0xff)))
		{
			if (penum->decRef() == 0)
			{
				if (penum->text.operation & TEXT_REPLACE_WIDTHS) 
        {
  	      if (penum->text.x_widths)
		      {
			      if (penum->text.x_widths == penum->text.y_widths)
				       penum->text.y_widths = 0;
				
			      delete [] penum->text.x_widths;
			      penum->text.x_widths = 0;
		      }
  	      if (penum->text.y_widths)
  	      {
  		      delete [] penum->text.y_widths;
  		      penum->text.y_widths = 0;
  	      }
        }
				delete penum;
			}
			return (int)(XWPSError::RangeCheck);
		}
		text = penum->text;
		text.operation = (text.operation &   ~(TEXT_FROM_STRING | TEXT_FROM_BYTES | TEXT_FROM_CHARS |
	       TEXT_FROM_GLYPHS | TEXT_FROM_SINGLE_CHAR)) |   TEXT_FROM_SINGLE_GLYPH;
		text.data.d_glyph = glyph;
		text.size = 1;
		penum->restart(&text);
  }
  ep[-8].makeMarkEStack(PS_ES_SHOW, &XWPSContextState::opShowCleanup);
  if (endproc == NULL)
		endproc = &XWPSContextState::finishShow;
  
  ep[-1].makeNull();
  ep[-2].makeInt(0);
  ep[-3].makeInt(0);
  ep[-4].makeInt(pgs->level);
  ep[-5].makeNull();
  ep[-6].makeNull();
  ep[-7].makeOper(0, endproc);
  ep->makeStruct(currentSpace() | 0, penum);
  exec_stack.setCurrentTop(ep);
  return 0;
}

int XWPSContextState::opShowFree(int code)
{
	int rcode;

  exec_stack.incCurrentTop(-9);
  rcode = opShowRestore(code < 0);
  return (rcode < 0 ? rcode : code);
}

int XWPSContextState::opShowRestore(bool for_error)
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	XWPSRef * ep = esp + 9;
  XWPSTextEnum *penum = (XWPSTextEnum*)(ep->getStruct());
  int saved_level = ep[-4].value.intval;
  int code = 0;

  if (for_error) 
  {
		uint saved_count = ep[-2].value.intval;
		uint count = op_stack.count();

		if (count > saved_count)
	    op_stack.pop(count - saved_count);
  }
  if (penum->showIsStringWidth()) 
		--saved_level;
  if (penum->text.operation & TEXT_REPLACE_WIDTHS) 
  {
  	if (penum->text.x_widths)
		{
			if (penum->text.x_widths == penum->text.y_widths)
				penum->text.y_widths = 0;
				
			delete [] penum->text.x_widths;
			penum->text.x_widths = 0;
		}
  	if (penum->text.y_widths)
  	{
  		delete [] penum->text.y_widths;
  		penum->text.y_widths = 0;
  	}
  }
    
  pgs->setCurrentFont(penum->orig_font);
  while (pgs->level > saved_level && code >= 0) 
  {
		if (pgs->saved == 0 || pgs->saved->saved == 0) 
	    code = (int)(XWPSError::Fatal);
		else
	    code = pgs->restore();
  }
  
  return code;
}

int XWPSContextState::opShowReturnWidth(uint npop, double *pwidth)
{
	uint index = opShowFindIndex();
  XWPSRef * ep = exec_stack.index(index - (9 - 1));
  XWPSTextEnum * penum = (XWPSTextEnum*)(ep->getStruct());
  int code = penum->setCharWidth(pwidth);
  uint ocount, dsaved, dcount;

  if (code < 0)
		return code;
		
  ocount = op_stack.count() - (uint)(ep[-2].value.intval);
  if (ocount < npop)
		return (int)(XWPSError::StackUnderflow);
  
  dsaved = (uint)(ep[-3].value.intval);
  dcount = dict_stack.count();
  if (dcount < dsaved)
		return (int)(XWPSError::DictStackUnderflow);
  
  while (dcount > dsaved) 
 	{
		code = zend();
		if (code < 0)
	    return code;
		dcount--;
  }
  op_stack.pop(ocount);
  
  popEStack(index - 9);
  return PS_O_POP_ESTACK;
}

int XWPSContextState::opShowSetup(XWPSRef * op)
{
	int code = op->checkReadType(XWPSRef::String);
	if (code < 0)
  	return code;
  	
  return opShowEnumSetup();
}

int XWPSContextState::opType1Cleanup()
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	esp[2].makeNull();
	return 0;
}

void XWPSContextState::opType1Free()
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	esp[-1].makeArray(PS_A_READONLY + PS_A_EXECUTABLE);
  esp->makeArray(PS_A_READONLY + PS_A_EXECUTABLE);
}

int XWPSContextState::type1CalloutDispatch(int (XWPSContextState::*cont)(), int num_args)
{
	XWPSRef other_subr;
	XWPSRef * esp = exec_stack.getCurrentTop();
  XWPSType1execState *pcxs = (XWPSType1execState*)(esp->getStruct());
  int code;

icont:
  code = type1ContinueDispatch(pcxs, NULL, &other_subr, num_args);
  switch (code) 
  {
		case 0:	
	    return 0;
	    
		default:
	    opType1Free();
	    return ((code < 0 ? code : (int)(XWPSError::InvalidFont)));
	    	
		case type1_result_callothersubr:
	    return type1PushOtherSubr(pcxs, cont, &other_subr);
	    
		case type1_result_sbw:
	    goto icont;
  }
}

int XWPSContextState::type1CallOtherSubr(XWPSType1execState * pcxs,
		     								 int (XWPSContextState::*cont) (),
		                     XWPSRef * pos)
{
	XWPSType1execState *hpcxs = new XWPSType1execState;
  hpcxs->copy(pcxs);
  hpcxs->cis.setCallbackData(hpcxs);
  XWPSRef * esp = exec_stack.incCurrentTop(1);
  esp->makeMarkEStack(PS_ES_SHOW, &XWPSContextState::opType1Cleanup);
  esp = exec_stack.incCurrentTop(1);
  esp->makeStruct(currentSpace() | 0, hpcxs);
  return type1PushOtherSubr(pcxs, cont, pos);
}

int XWPSContextState::type1ContinueDispatch(XWPSType1execState *pcxs,
			                     XWPSRef * pcref, 
			                     XWPSRef *pos, 
			                     int num_args)
{
	int value;
  int code;
  XWPSString charstring;
  XWPSString *pchars;

  if (pcref == 0) 
		pchars = 0;
  else 
  {
		charstring.data = pcref->getBytes();
		charstring.size = pcref->size();
		pchars = &charstring;
  }
  
  pcxs->i_ctx_p = this;
  pcxs->num_args = num_args;
  XWPSRef * d = pcxs->save_args;
  XWPSRef * s = op_stack.getCurrentTop() - (num_args - 1);
  memcpyRef(d, s, num_args);
  
  op_stack.incCurrentTop(-num_args);
  pcxs->cis.setCallbackData(pcxs);
  code = (pcxs->cis.pfont)->callInterpret(&(pcxs->cis), pchars, &value);
  switch (code) 
  {
		case type1_result_callothersubr: 
			{
	    	XWPSFontData *pfdata = (XWPSFontData*)(pgs->currentFont()->client_data);

	    	code = pfdata->u.type1.OtherSubrs->arrayGet(this, (long)value, pos);
	    	if (code >= 0)
					return type1_result_callothersubr;
			}
  }
  
  d = op_stack.getCurrentTop() + 1;
  s = pcxs->save_args;
  memcpyRef(d, s, num_args);
  op_stack.incCurrentTop(num_args);
  return code;
}

int XWPSContextState::type1ExecBbox(XWPSType1execState * pcxs, XWPSFont * pfont)
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSType1State *const pcis = &pcxs->cis;
  XWPSFontBase *const pbfont = (XWPSFontBase *) pfont;
  
  if (pcxs->present == metricsNone) 
  {
		XWPSRef cnref;
		XWPSRef other_subr;
		int code;
		
		cnref.assign(op - 1);
		code = type1ContinueDispatch(pcxs, op, &other_subr, 4);
		op = op_stack.getCurrentTop();
		switch (code) 
		{
	    default:	
				return ((code < 0 ? code : (int)(XWPSError::InvalidFont)));
					
	    case type1_result_callothersubr:
				return type1CallOtherSubr(pcxs,  &XWPSContextState::bboxGetSbwContinue, &other_subr);
					
	    case type1_result_sbw:
				break;
		}
		pcis->type1CisGetMetrics(pcxs->sbw);
		return zcharSetCache(pbfont, &cnref, NULL, pcxs->sbw + 2,  &pcxs->char_bbox,
			       				&XWPSContextState::bboxFinishFill, &XWPSContextState::bboxFinishStroke, NULL);
  } 
  else 
  {
		return zcharSetCache(pbfont, op - 1, (pcxs->present == metricsSideBearingAndWidth ?
						pcxs->sbw : NULL), pcxs->sbw + 2, &pcxs->char_bbox,
			       &XWPSContextState::bboxFinishFill, &XWPSContextState::bboxFinishStroke, NULL);
  }
}

int XWPSContextState::type1PushOtherSubr(XWPSType1execState *pcxs,
		                     int (XWPSContextState::*cont)(), 
		                     XWPSRef *pos)
{
	int i, n = pcxs->num_args;
	XWPSRef * esp = exec_stack.incCurrentTop(1);
	esp->makeOper(0,cont);
  for (i = n; --i >= 0; ) 
  {
  	esp = exec_stack.incCurrentTop(1);
		esp->assign(&pcxs->save_args[i]);
		esp->clearAttrs(PS_A_EXECUTABLE);
  }
  esp = exec_stack.incCurrentTop(1);
  esp->assign(pos);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::type42Fill()
{
	return type42Finish(&XWPSState::fill);
}

int XWPSContextState::type42Finish(int (XWPSState::*cont)())
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSFont *pfont;
  int code;
  XWPSTextEnum *penum = opShowFind();
  double sbxy[2];
  XWPSPoint sbpt;
  XWPSPoint *psbpt = 0;
  XWPSRef * opc = op;

  if (!op[-3].hasType(XWPSRef::Dictionary)) 
  {
		code = checkOp(op,6);
		if (code < 0)
	    return code;
		code = numParams(op, 2, sbxy);
		if (code < 0)
	    return code;
		sbpt.x = sbxy[0];
		sbpt.y = sbxy[1];
		psbpt = &sbpt;
		opc -= 2;
  }
  
  code = opc->checkType(XWPSRef::Integer);
  if (code < 0)
	  return code;
  code = fontParam(opc - 3, &pfont);
  if (code < 0)
		return code;
    
  if (penum == 0 || (pfont->FontType != ft_TrueType && pfont->FontType != ft_CID_TrueType))
		return (int)(XWPSError::Undefined);
	
  code = ((XWPSFontType42 *)pfont)->type42Append((uint)opc->value.intval, pgs, pgs->path, 
  				&(penum->log2_scale), (penum->text.operation & TEXT_DO_ANY_CHARPATH) != 0,  pfont->PaintType);
  if (code < 0)
		return code;
    
  pop((psbpt == 0 ? 4 : 6));
  return (pgs->*cont)();
}

int XWPSContextState::type42Stroke()
{
	return type42Finish(&XWPSState::stroke);
}

int XWPSContextState::zashow()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSTextEnum *penum = 0;
  double axy[2];
  int code = numParams(op - 1, 2, axy);

  if (code < 0 ||
			(code = opShowSetup(op)) != 0 ||
			(code = pgs->ashowBegin(axy[0], axy[1], op->getBytes(), op->size(), &penum)) < 0)
		return code;
    
  penum->text.p_bytes = op->value.bytes->arr;
  if (op->value.bytes->arr)
  	op->value.bytes->arr->incRef();
  if ((code = opShowFinishSetup(penum, 3, &XWPSContextState::finishShow)) < 0) 
  {
		return code;	
  }
  return opShowContinuePop(3);
}

int XWPSContextState::zawidthShow()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSTextEnum *penum = 0;
  double cxy[2], axy[2];
  int code;

  code = op[-3].checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if ((ulong) (op[-3].value.intval) != op[-3].value.intval)
		return (int)(XWPSError::RangeCheck);
  
  if ((code = numParams(op - 4, 2, cxy)) < 0 ||
			(code = numParams(op - 1, 2, axy)) < 0 ||
			(code = opShowSetup(op)) != 0 ||
			(code = pgs->awidthShowBegin(cxy[0], cxy[1],
				    (ulong) op[-3].value.intval, axy[0], axy[1],  op->getBytes(), op->size(), &penum)) < 0)
		return code;
    
  penum->text.p_bytes = op->value.bytes->arr;
  if (op->value.bytes->arr)
  	op->value.bytes->arr->incRef();
  if ((code = opShowFinishSetup(penum, 6, &XWPSContextState::finishShow)) < 0) 
  {
		return code;	
  }
  return opShowContinuePop(6);
}

int XWPSContextState::zcharBoxPath()
{
	return zcharPath(&XWPSState::charBoxPathBegin);
}

int XWPSContextState::zcharCharStringData(XWPSFont *font, XWPSRef *pgref, XWPSString *pstr)
{
	XWPSRef *pcstr;
	XWPSFontData * pfdata = (XWPSFontData*)(font->client_data);

  if (pfdata->CharStrings.dictFind(this, pgref, &pcstr) <= 0)
		return (int)(XWPSError::Undefined);
	
  if (!pcstr->hasType(XWPSRef::String)) 
  {
		if (font->FontType == ft_encrypted &&
	    	charStringIsNotdefProc(pcstr))
	  {
	    return charStringMakeNotdef(pstr, font);
	  }
		else
	    return (int)(XWPSError::TypeCheck);
  }
  pstr->data = pcstr->getBytes();
  pstr->size = pcstr->size();
  pstr->bytes = pcstr->value.bytes->arr;
  if (pstr->bytes)
  	pstr->bytes->incRef();
  return 0;
}

int XWPSContextState::zcharExecCharProc()
{
	XWPSRef * op = op_stack.getCurrentTop();
	
  XWPSRef * ep;

  int code = checkEStack(5);
  if (code < 0)
  	return code;
  	
  XWPSRef * esp = exec_stack.incCurrentTop(5);
  ep = esp;
  ep[-4].makeOper(0, &XWPSContextState::zend);
  ep[-3].makeOper(0, &XWPSContextState::zend);
  ep[-2].assign(op);
  ep[-1].makeOper(0, &XWPSContextState::zbegin);
  ep->makeOper(0, &XWPSContextState::zbegin);
  op[-1].assign(dict_stack.getSystemDict());
  {
		XWPSRef rfont;

		rfont.assign(op - 3);
		op[-3].assign(op - 2);
		op[-2].assign(&rfont);
  }
  pop(1);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zcharGetMetrics(const XWPSFontBase * pbfont, 
	                    XWPSRef * pcnref,
		                  double psbw[4])
{
	XWPSFontData * pfdata = (XWPSFontData*)(pbfont->client_data);
	XWPSRef *pfdict = &pfdata->dict;
  XWPSRef *pmdict;

  if (pfdict->dictFindString(this, "Metrics", &pmdict) > 0) 
  {
		XWPSRef *pmvalue;

		int code = pmdict->checkTypeOnly(XWPSRef::Dictionary);
		if (code < 0)
			return code;
			
		code = pmdict->checkDictRead();
		if (code < 0)
			return code;
			
		if (pmdict->dictFind(this, pcnref, &pmvalue) > 0) 
		{
	    if (numParams(pmvalue, 1, psbw + 2) >= 0) 
	    {
				psbw[3] = 0;
				return metricsWidthOnly;
	    } 
	    else 
	    {
				code = pmvalue->checkReadTypeOnly(XWPSRef::Array);
				if (code < 0)
					return code;
					
				XWPSRef * a = pmvalue->getArray();
				switch (pmvalue->size()) 
				{
		    	case 2:	/* [<sbx> <wx>] */
						code = numParams(a + 1, 2, psbw);
						psbw[2] = psbw[1];
						psbw[1] = psbw[3] = 0;
						break;
						
		    	case 4:	
						code = numParams(a + 3, 4, psbw);
						break;
						
		    	default:
						return (XWPSError::RangeCheck);
				}
				if (code < 0)
		    	return code;
		    	
				return metricsSideBearingAndWidth;
	    }
		}
  }
  return metricsNone;
}

int XWPSContextState::zcharGetMetrics2(const XWPSFontBase * pbfont, 
	                     XWPSRef * pcnref,
		                   double pwv[4])
{
	XWPSFontData * pfdata = (XWPSFontData*)(pbfont->client_data);
	XWPSRef *pfdict = &pfdata->dict;
  XWPSRef *pmdict;

  if (pfdict->dictFindString(this, "Metrics2", &pmdict) > 0) 
  {
		XWPSRef *pmvalue;

		int code = pmdict->checkTypeOnly(XWPSRef::Dictionary);
		if (code < 0)
		   return code;
		code = pmdict->checkDictRead();
		if (code < 0)
		   return code;
		if (pmdict->dictFind(this, pcnref, &pmvalue) > 0) 
		{
	    code = pmvalue->checkReadTypeOnly(XWPSRef::Array);
	    if (code < 0)
		   	return code;
	    if (pmvalue->size() == 4) 
	    {
	    	XWPSRef * a = pmvalue->getArray();
				code = numParams(a + 3, 4, pwv);

				return (code < 0 ? code : metricsSideBearingAndWidth);
	    }
		}
  }
  return metricsNone;
}

int XWPSContextState::zcharPath(int (XWPSState::*begin)(const uchar *, uint, bool, XWPSTextEnum **))
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSTextEnum *penum = 0;
  int code;

  code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  code = opShowSetup(op - 1);
  if (code != 0 || (code = (pgs->*begin)(op[-1].getBytes(), op[-1].size(), op->value.boolval, &penum)) < 0)
		return code;
	penum->text.p_bytes = op[-1].value.bytes->arr;
	if (op[-1].value.bytes->arr)
		op[-1].value.bytes->arr->incRef();
  if ((code = opShowFinishSetup(penum, 2, &XWPSContextState::finishShow)) < 0) 
  {
		return code;
  }
  return opShowContinuePop(2);
}

int XWPSContextState::zcharPath()
{
	return zcharPath(&XWPSState::charPathBegin);
}

int XWPSContextState::zcharSetCache(const XWPSFontBase * pbfont,
		                XWPSRef * pcnref, 
		                const double psb[2],
		                const double pwidth[2], 
		                const XWPSRect * pbbox,
		                op_proc_t cont_fill, 
		                op_proc_t cont_stroke,
		                const double Metrics2_sbw_default[4])
{
	XWPSRef * op = op_stack.getCurrentTop();
	XWPSFontData * pfdata = (XWPSFontData*)(pbfont->client_data);
	XWPSRef *pfdict = &pfdata->dict;
	XWPSRef * esp = exec_stack.getCurrentTop();
  XWPSRef *pcdevproc;
  int have_cdevproc;
  XWPSRef rpop;
  bool metrics2;
  op_proc_t cont;
  double w2[10];
  XWPSTextEnum *penum = opShowFind();

  w2[0] = pwidth[0], w2[1] = pwidth[1];
  w2[2] = pbbox->p.x, w2[3] = pbbox->p.y;
  w2[4] = pbbox->q.x, w2[5] = pbbox->q.y;
  if (pbfont->PaintType == 0)
		cont = cont_fill;
  else 
  {
		double expand = qMax((float)1.415, pgs->currentMiterLimit()) *	pgs->currentLineWidth() / 2;

		w2[2] -= expand, w2[3] -= expand;
		w2[4] += expand, w2[5] += expand;
		cont = cont_stroke;
  }

  {
		int code = zcharGetMetrics2(pbfont, pcnref, w2 + 6);

		if (code < 0)
	    return code;
		metrics2 = code > 0;
  }

  if (!metrics2 && Metrics2_sbw_default != NULL) 
  {
    w2[6] = Metrics2_sbw_default[2];
    w2[7] = Metrics2_sbw_default[3];
    w2[8] = Metrics2_sbw_default[0];
    w2[9] = Metrics2_sbw_default[1];
		metrics2 = true;
  }

  have_cdevproc = pfdict->dictFindString(this, "CDevProc", &pcdevproc) > 0;
  if (have_cdevproc || penum->zcharShowWidthOnly()) 
  {
		int i;
		op_proc_t zsetc;
		int nparams;

		if (have_cdevproc) 
		{
	    int code = pcdevproc->checkProcOnly();
	    if (code < 0)
	    	return code;
	    	
	    zsetc = &XWPSContextState::zsetCacheDevice2;
	    if (!metrics2) 
	    {
				w2[6] = w2[0], w2[7] = w2[1];
				w2[8] = w2[9] = 0;
	    }
	    nparams = 10;
		} 
		else 
		{
	    rpop.makeOper(0, &XWPSContextState::zpop);
	    pcdevproc = &rpop;
	    if (metrics2)
				zsetc = &XWPSContextState::zsetCacheDevice2, nparams = 10;
	    else
				zsetc = &XWPSContextState::zsetCacheDevice, nparams = 6;
		}
		checkEStack(3);
		if (psb != 0) 
		{
	    int code = push(&op, nparams + 3);
	    if (code < 0)
				return code;
	    op[-(nparams + 2)].makeReal(psb[0]);
	    op[-(nparams + 1)].makeReal(psb[1]);
		} 
		else 
		{
	    int code = push(&op, nparams + 1);
	    if (code < 0)
				return code;
		}
		for (i = 0; i < nparams; ++i)
	    op[- (nparams - i)].makeReal(w2[i]);
		op->assign(pcnref);
		esp = exec_stack.incCurrentTop(1);
		esp->makeOper(0, cont);
		esp = exec_stack.incCurrentTop(1);
		esp->makeOper(0, zsetc);
		esp = exec_stack.incCurrentTop(1);
		esp->assign(pcdevproc);
		return PS_O_PUSH_ESTACK;
  } 
  
  {
		int code = (metrics2 ? penum->setCacheDevice2(w2) : penum->setCacheDevice(w2));

		if (code < 0)
	    return code;
  }
  
  if (psb != 0) 
  {
		int code = push(&op, 2);
		if (code < 0)
			return code;
		op[-1].makeReal(psb[0]);
		op->makeReal(psb[1]);
  }
  return (this->*cont)();
}

int XWPSContextState::zcharStringOutline(XWPSFontType1 *pfont1, 
	                       XWPSRef *pgref,
		                     const XWPSString *pgstr,
		                     const XWPSMatrix *pmat, 
		                     XWPSPath *ppath)
{
	XWPSString *pchars = (XWPSString*)pgstr;
  int code;
  XWPSType1execState cxs;
  XWPSType1State *const pcis = &cxs.cis;
  XWPSLog2ScalePoint no_scale;
  XWPSRef *pfdict;
  XWPSRef *pcdevproc;
  int value;
  XWPSImagerState gis;
  gis.i_ctx_p = this;
  double sbw[4];
  XWPSPoint mpt;

  if (pgstr->size <= qMax(pfont1->lenIV, 0))
		return (int)(XWPSError::InvalidFont);
	
	XWPSFontData * pfdata = (XWPSFontData*)(pfont1->client_data);
  pfdict = &pfdata->dict;
  if (pfdict->dictFindString(this, "CDevProc", &pcdevproc) > 0)
		return (int)(XWPSError::RangeCheck);
  
  switch (pfont1->WMode) 
  {
    default:
			code = zcharGetMetrics2(pfont1, pgref, sbw);
			if (code)
	    	break;
	    	
    case 0:
			code = zcharGetMetrics(pfont1, pgref, sbw);
  }
  if (code < 0)
		return code;
    
  cxs.present = code;
  if (pmat)
		gis.ctm.fromMatrix((XWPSMatrix*)pmat);
  else 
  {
		XWPSMatrix imat;

		gis.ctm.fromMatrix(&imat);
  }
  gis.flatness = 0;
  code = cxs.cis.type1InterpInit(&gis, ppath, &no_scale, true, 0,	pfont1);
  if (code < 0)
		return code;
    
  cxs.cis.charpath_flag = true;	
  pcis->setCallbackData(&cxs);
  switch (cxs.present) 
  {
    case metricsSideBearingAndWidth:
			mpt.x = sbw[0], mpt.y = sbw[1];
			pcis->setLSB(&mpt);
			
    case metricsWidthOnly:
			mpt.x = sbw[2], mpt.y = sbw[3];
			pcis->setWidth(&mpt);
    case metricsNone:
			;
  }
  
icont:
  code = pfont1->callInterpret(pcis, pchars, &value);
  switch (code) 
  {
    case 0:
    default:
			return code;
			
    case type1_result_callothersubr:
			return (int)(XWPSError::RangeCheck);
				
    case type1_result_sbw:
			pcis->type1CisGetMetrics(cxs.sbw);
			pchars = 0;
			goto icont;
  }
}

int XWPSContextState::zfontBbox()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double bbox[4];
  int code;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = op->checkDictRead();
  if (code < 0)
  	return code;
  	
  code = fontBboxParam(op, bbox);
  if (code < 0)
		return code;
    
  if (bbox[0] < bbox[2] && bbox[1] < bbox[3]) 
  {
		code = push(&op, 4);
		if (code < 0)
			return code;
		XWPSRef * p = op-4;
		double *pval = &bbox[0];
		int count = 4;
		for (; count--; p++, pval++)
			p->makeReal(*pval);
    return 0;
    
		op->makeTrue();
  } 
  else 
		op->makeFalse();
  return 0;
}

int XWPSContextState::zgetMetrics32()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uchar *data;
  uint size;
  int i, n = 6;
  XWPSRef * wop;

  int code = op->checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  data = op->getBytes();
  size = op->size();
  if (size < 5)
		return (int)(XWPSError::RangeCheck);
  
  if (data[0]) 
  {
		int llx = (int)data[3] - 128, lly = (int)data[4] - 128;

		n = 6;
		size = 5;
		code = push(&op, 8);
		if (code < 0)
			return code;
		op[-6].makeInt(data[2]);
		op[-5].makeInt(0);
		op[-4].makeInt(llx);
		op[-3].makeInt(lly);
		op[-2].makeInt(llx + data[0]); 
		op[-1].makeInt(lly + data[1]);
  } 
  else 
  {
		if (data[1]) 
		{
	    if (size < 22)
				return (int)(XWPSError::RangeCheck);
	    n = 10;
	    size = 22;
		} 
		else 
		{
	    if (size < 14)
				return (int)(XWPSError::RangeCheck);
					
	    n = 6;
	    size = 14;
		}
		code = push(&op, 2 + n);
		if (code < 0)
			return code;
		for (i = 0; i < n; ++i)
	    op[-n + i].makeInt(((int)((data[2 * i + 2] << 8) + data[2 * i + 3]) ^ 0x8000) - 0x8000);
  }
  wop = op - n;
  wop[-2].makeInt(wop[4].value.intval - wop[2].value.intval);
  wop[-1].makeInt(wop[5].value.intval - wop[3].value.intval);
  op->makeInt(size);
  return 0;
}

int XWPSContextState::zglyphShow()
{
	ulong glyph;
  XWPSTextEnum *penum = 0;
  int code;

  if ((code = glyphShowSetup(&glyph)) != 0 ||	(code = pgs->glyphShowBegin(glyph, &penum)) < 0)
		return code;
		
  if ((code = opShowFinishSetup(penum, 1, NULL)) < 0) 
		return code;
  return opShowContinuePop(1);
}

int XWPSContextState::zglyphWidth()
{
	ulong glyph;
  XWPSTextEnum *penum = 0;
  int code;

  if ((code = glyphShowSetup(&glyph)) != 0 ||	(code = pgs->glyphWidthBegin(glyph, &penum)) < 0)
		return code;
    
  if ((code = opShowFinishSetup(penum, 1, &XWPSContextState::finishStringWidth)) < 0) 
  {
		return code;
  }
  return opShowContinuePop(1);
}

int XWPSContextState::zkshow()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSTextEnum *penum = 0;
  int code;

  code = op[-1].checkProc();
  if (code < 0)
  	return code;
  	
  if ((code = opShowSetup(op)) != 0 || (code = pgs->kshowBegin(op->getBytes(), op->size(), &penum)) < 0)
		return code;
	penum->text.p_bytes = op->value.bytes->arr;
	if (op->value.bytes->arr)
		op->value.bytes->arr->incRef();
  if ((code = opShowFinishSetup(penum, 2, &XWPSContextState::finishShow)) < 0) 
  {
		return code;	
  }
  XWPSRef * esp = exec_stack.getCurrentTop();
  esp[-1].assign(&op[-1]);
  return opShowContinuePop(2);
}

int XWPSContextState::zmakeGlyph32()
{
	XWPSRef * op = op_stack.getCurrentTop();
  bool long_form;
  uint msize;
  double metrics[10];
  int wx, llx, lly, urx, ury;
  int width, height, raster;
  XWPSFont *pfont;
  int code;
  uchar *str;

  code = op[-4].checkArray();
 	if (code < 0)
		return code;
		
  msize = op[-4].size();
  switch (msize) 
  {
		case 10:
	    long_form = true;
	    break;
	    
		case 6:
	    long_form = false;
	    break;
	    
		default:
	    return (int)(XWPSError::RangeCheck);
  }
  
  XWPSRef * a = op[-4].getArray() + msize - 1;
  code = numParams(a, msize, metrics);
  if (code < 0)
		return code;
    
  if (~code & 0x3c)	
		return (int)(XWPSError::TypeCheck);
  
  code = op[-3].checkReadType(XWPSRef::String);
  if (code < 0)
		return code;
		
  llx = (int)metrics[2];
  lly = (int)metrics[3];
  urx = (int)metrics[4];
  ury = (int)metrics[5];
  width = urx - llx;
  height = ury - lly;
  raster = (width + 7) >> 3;
  if (width < 0 || height < 0 || op[-3].size() != raster * height)
		return (int)(XWPSError::RangeCheck);
    
  code = op[-2].checkIntLEU(65535);
  if (code < 0)
		return code;
		
  code = fontParam(op - 1, &pfont);
  if (code < 0)
		return code;
    
  if (pfont->FontType != ft_CID_bitmap)
		return (int)(XWPSError::InvalidFont);
 	
 	code = op->checkWriteType(XWPSRef::String);
  if (op->size() < 22)
		return (int)(XWPSError::RangeCheck);
  
  str = op->getBytes();
  if (long_form || metrics[0] != (wx = (int)metrics[0]) ||
			metrics[1] != 0 || height == 0 ||
			((wx | width | height | (llx + 128) | (lly + 128)) & ~255) != 0) 
	{
		int i, n = (long_form ? 10 : 6);

		str[0] = 0;
		str[1] = long_form;
		for (i = 0; i < n; ++i) 
		{
	    int v = (int)metrics[i];

	    str[2 + 2 * i] = (uchar)(v >> 8);
	    str[2 + 2 * i + 1] = (uchar)v;
		}
		op->setSize(2 + n * 2);
  } 
  else 
  {
		str[0] = (uchar)width;
		str[1] = (uchar)height;
		str[2] = (uchar)wx;
		str[3] = (uchar)(llx + 128);
		str[4] = (uchar)(lly + 128);
		op->setSize(5);
  }
  return code;
}

int XWPSContextState::zremoveGlyphs()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  PSFontCidRange range;

  code = op[-2].checkIntLEU(65535);
  if (code < 0)
		return code;
		
  code = op[-1].checkIntLEU(65535);
  if (code < 0)
		return code;
  code = fontParam(op, &range.font);
  if (code < 0)
		return code;
    
  if (range.font->FontType != ft_CID_bitmap)
		return (int)(XWPSError::InvalidFont);
  
  range.cid_min = ps_min_cid_glyph + op[-2].value.intval;
  range.cid_max = ps_min_cid_glyph + op[-1].value.intval;
  range.font->dir->purgeSelectedCachedChars(&XWPSCachedChar::selectCidRange, &range);
  pop(3);
  return 0;
}

int XWPSContextState::zsetCacheDevice()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double wbox[6];
  XWPSTextEnum *penum = opShowFind();
  int code = numParams(op, 6, wbox);

  if (penum == 0)
		return (int)(XWPSError::Undefined);
  
  if (code < 0)
		return code;
    
  if (penum->zcharShowWidthOnly())
		return opShowReturnWidth(6, &wbox[0]);
  
  code = penum->setCacheDevice(wbox);
  if (code < 0)
		return code;
    
  pop(6);
  if (code == 1)
  {
  	XWPSIntGState * iigs = (XWPSIntGState*)(pgs->client_data);
		iigs->pagedevice.makeNull();
	}
  return 0;
}

int XWPSContextState::zsetCacheDevice2()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double wbox[10];
  XWPSTextEnum *penum = opShowFind();
  int code = numParams(op, 10, wbox);

  if (penum == 0)
		return (int)(XWPSError::Undefined);
	
  if (code < 0)
		return code;
		
  if (penum->zcharShowWidthOnly())
		return opShowReturnWidth(10, (pgs->rootFont()->WMode ?  &wbox[6] : &wbox[0]));
  code = penum->setCacheDevice2(wbox);
  if (code < 0)
		return code;
    
  pop(10);
  if (code == 1)
  {
		XWPSIntGState * iigs = (XWPSIntGState*)(pgs->client_data);
		iigs->pagedevice.makeNull();
	}
  return 0;
}

int XWPSContextState::zsetCharWidth()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double width[2];
  XWPSTextEnum *penum = opShowFind();
  int code = numParams(op, 2, width);

  if (penum == 0)
		return (int)(XWPSError::Undefined);
	
  if (code < 0)
		return code;
    
  if (penum->zcharShowWidthOnly())
		return opShowReturnWidth(2, &width[0]);
  code = penum->setCharWidth(width);
  if (code < 0)
		return code;
  pop(2);
  return 0;
}

int XWPSContextState::zshow()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSTextEnum *penum = 0;
  int code = opShowSetup(op);

  if (code != 0 ||	(code = pgs->showBegin(op->getBytes(), op->size(), &penum)) < 0)
		return code;
	penum->text.p_bytes = op->value.bytes->arr;
	if (op->value.bytes->arr)
		op->value.bytes->arr->incRef();
  if ((code = opShowFinishSetup(penum, 1, &XWPSContextState::finishShow)) < 0) 
  {
		return code;	
  }
  return opShowContinuePop(1);
}

int XWPSContextState::zstringWidth()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSTextEnum *penum = 0;
  int code = opShowSetup(op);

  if (code != 0 || (code = pgs->stringWidthBegin(op->getBytes(), op->size(), &penum)) < 0)
		return code;
	penum->text.p_bytes = op->value.bytes->arr;
	if (op->value.bytes->arr)
		op->value.bytes->arr->incRef();
  if ((code = opShowFinishSetup(penum, 1, &XWPSContextState::finishStringWidth)) < 0) 
  {
		return code;
  }
  return opShowContinuePop(1);
}

int XWPSContextState::ztype1ExecChar()
{
	return charStringExecChar((1 << (int)ft_encrypted) |  (1 << (int)ft_disk_based));
}

int XWPSContextState::ztype2ExecChar()
{
	return charStringExecChar((1 << (int)ft_encrypted2));
}

int XWPSContextState::ztype42ExecChar()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSFont *pfont=0;
  int code = fontParam(op - 3, &pfont);
  XWPSFontBase *const pbfont = (XWPSFontBase *) pfont;
  XWPSTextEnum *penum = opShowFind();
  int present;
  double sbw[4];

  if (code < 0)
		return code;
    
  if (penum == 0 ||	(pfont->FontType != ft_TrueType &&	pfont->FontType != ft_CID_TrueType))
		return (int)(XWPSError::Undefined);
	
  if (pfont->PaintType)
		pgs->setLineWidth(pfont->StrokeWidth);
  code = checkEStack(3);
  if (code < 0)
		return code;
		
  if (op->isProc())
		return zcharExecCharProc();
		
  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  code = checkOStack(3);
  if (code < 0)
		return code;
		
  present = zcharGetMetrics(pbfont, op - 1, sbw);
  if (present < 0)
		return present;
		
  code = pgs->moveTo(0.0, 0.0);
  if (code < 0)
		return code;
		
  if (present == metricsNone) 
  {
		float sbw42[4];
		int i;

		code = ((XWPSFontType42 *) pfont)->type42GetMetrics((uint) op->value.intval, sbw42);
		if (code < 0)
	    return code;
		for (i = 0; i < 4; ++i)
	    sbw[i] = sbw42[i];
  }
  return zcharSetCache(pbfont, op - 1,
			   (present == metricsSideBearingAndWidth ? sbw : NULL),
			   sbw + 2, &pbfont->FontBBox,
			   &XWPSContextState::type42Fill, &XWPSContextState::type42Stroke, NULL);
}

int XWPSContextState::zwidthShow()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSTextEnum *penum = 0;
  double cxy[2];
  int code;

  code = op[-1].checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if ((ulong) (op[-1].value.intval) != op[-1].value.intval)
		return (int)(XWPSError::RangeCheck);
  
  if ((code = numParams(op - 2,2, cxy)) < 0 ||
			(code = opShowSetup(op)) != 0 ||
			(code = pgs->widthShowBegin(cxy[0], cxy[1],  (ulong) op[-1].value.intval, op->getBytes(), op->size(), &penum)) < 0)
		return code;
	penum->text.p_bytes = op->value.bytes->arr;
	if (op->value.bytes->arr)
		op->value.bytes->arr->incRef();
  if ((code = opShowFinishSetup(penum, 4, &XWPSContextState::finishShow)) < 0) 
  {
		return code;	
  }
  return opShowContinuePop(4);
}

int XWPSContextState::z1pop(void * callback_data, long * pf)
{
	XWPSType1execState *pcxs = (XWPSType1execState*)callback_data;
  XWPSContextState *i_ctx_p = pcxs->i_ctx_p;
  double val;
  int code = i_ctx_p->op_stack.getCurrentTop()->realParam(&val);

  if (code < 0)
		return code;
  *pf = float2fixed(val);
  i_ctx_p->op_stack.incCurrentTop(-1);
  return 0;
}

int XWPSContextState::z1push(void * callback_data, const long * pf, int count)
{
	XWPSType1execState *pcxs = (XWPSType1execState*)callback_data;
  XWPSContextState *i_ctx_p = pcxs->i_ctx_p;
  const long *p = pf + count - 1;

  int code = i_ctx_p->checkOStack(count);
  if (code < 0)
  	return code;
  for (int i = 0; i < count; i++, p--) 
  {
  	XWPSRef * osp = i_ctx_p->op_stack.incCurrentTop(1);
		osp->makeReal(fixed2float(*p));
  }
  return 0;
}

int XWPSContextState::zxshow()
{
	return moveShow(true, false);
}

int XWPSContextState::zxyshow()
{
	return moveShow(true, true);
}

int XWPSContextState::zyshow()
{
	return moveShow(false, true);
}
