/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "XWPSError.h"
#include "XWPSPath.h"
#include "XWPSDevice.h"
#include "XWPSState.h"
#include "XWPSContextState.h"

#define UPATH_MAX_OP 11
#define UPATH_REPEAT 32
static const uchar up_nargs[UPATH_MAX_OP + 1] = {
    4, 2, 2, 2, 2, 6, 6, 5, 5, 5, 0, 0
};

static const op_proc_t up_ops[UPATH_MAX_OP + 1] = {
    &XWPSContextState::zsetBbox, 
    &XWPSContextState::zmoveTo, 
    &XWPSContextState::zrmoveTo, 
    &XWPSContextState::zlineTo, 
    &XWPSContextState::zrlineTo,
    &XWPSContextState::zcurveTo, 
    &XWPSContextState::zrcurveTo, 
    &XWPSContextState::zarc, 
    &XWPSContextState::zarcn, 
    &XWPSContextState::zarct,
    &XWPSContextState::zclosePath, 
    &XWPSContextState::zucache
};

int XWPSContextState::commonArc(int (XWPSState::*aproc)(float, float, float, float, float))
{
	XWPSRef * op = op_stack.getCurrentTop();
  double xyra[5];
  int code = numParams(op, 5, xyra);

  if (code < 0)
		return code;
  code = (pgs->*aproc)(xyra[0], xyra[1], xyra[2], xyra[3], xyra[4]);
  if (code >= 0)
		pop(5);
  return code;
}

int XWPSContextState::commonArct(float *tanxy)
{
	XWPSRef * op = op_stack.getCurrentTop();
  double args[5];
  int code = numParams(op, 5, args);

  if (code < 0)
		return code;
  return pgs->arcTo(args[0], args[1], args[2], args[3], args[4], tanxy);
}

int XWPSContextState::commonCurve(int (XWPSState::*add_proc)(float, float, float, float, float, float))
{
	XWPSRef * op = op_stack.getCurrentTop();
  double opxy[6];
  int code;
  
  if ((code = numParams(op, 6, opxy)) < 0)
		return code;
  code = (pgs->*add_proc)(opxy[0], opxy[1], opxy[2], opxy[3], opxy[4], opxy[5]);
  if (code >= 0)
		pop(6);
  return code;
}

int XWPSContextState::commonTo(int (XWPSState::*add_proc)(float, float))
{
	XWPSRef * op = op_stack.getCurrentTop();
  double opxy[2];
  int code;

  if ((code = numParams(op, 2, opxy)) < 0 ||
			(code = (pgs->*add_proc)(opxy[0], opxy[1])) < 0)
		return code;
  pop(2);
  return 0;
}

int XWPSContextState::pathCleanup()
{
	XWPSRef * ep = exec_stack.getCurrentTop();
	ep[-6].makeInt(0);
  return 0;
}

void XWPSContextState::pfPush(XWPSPoint * ppts, int n)
{
	XWPSRef * op = op_stack.getCurrentTop();

  while (n--) 
 	{
		op += 2;
		op[-1].makeReal(ppts->x);
		op->makeReal(ppts->y);
		ppts++;
  }
  op_stack.setCurrentTop(op);
}

int XWPSContextState::zarc()
{
	return commonArc(&XWPSState::arc);
}

int XWPSContextState::zarcn()
{
	return commonArc(&XWPSState::arcn);
}

int XWPSContextState::zarct()
{
	int code = commonArct((float *)0);

  if (code < 0)
		return code;
  pop(5);
  return 0;
}

int XWPSContextState::zarcTo()
{
	XWPSRef * op = op_stack.getCurrentTop();
  float tanxy[4];
  int code = commonArct(tanxy);

  if (code < 0)
		return code;
    
  op[-4].makeReal(tanxy[0]);
  op[-3].makeReal(tanxy[1]);
  op[-2].makeReal(tanxy[2]);
  op[-1].makeReal(tanxy[3]);
  pop(1);
  return 0;
}

int XWPSContextState::zclip()
{
	return pgs->clip();
}

int XWPSContextState::zclosePath()
{
	return pgs->closePath();
}

int XWPSContextState::zcurrentPoint()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSPoint pt;
  int code = pgs->currentPoint(&pt);

  if (code < 0)
		return code;
  
  code = push(&op, 2);
  if (code < 0)
		return code;
  op[-1].makeReal(pt.x);
  op->makeReal(pt.y);
  return 0;
}

int XWPSContextState::zcurveTo()
{
	return commonCurve(&XWPSState::curveTo);
}

int XWPSContextState::zclipPath()
{
	return pgs->clipPath();
}

int XWPSContextState::zdashPath()
{
	return pgs->dashPath();
}

int XWPSContextState::zdefineUserName()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef uname;

  int code = op[-1].checkIntLTU(PS_MAX_ARRAY_SIZE);
  if (code < 0)
  	return code;
  	
  code = op->checkType(XWPSRef::Name);
  if (user_names_p == 0) 
  {
  	user_names_p = new XWPSRef;
  	user_names_p->makeArray(PS_A_READONLY);
  }
  
  if (user_names_p->arrayGet(this, op[-1].value.intval, &uname) >= 0) 
  {
		switch (uname.type()) 
		{
	    case XWPSRef::Null:
				break;
				
	    case XWPSRef::Name:
				if (nameEq(&uname, op))
		    	goto ret;
		    	
	    default:
				return (int)(XWPSError::InvalidAccess);
		}
  } 
  else 
  {	
		XWPSRef new_array;
		uint old_size = user_names_p->size();
		uint new_size = (uint) op[-1].value.intval + 1;

		if (new_size < 100)
	    new_size = 100;
		else if (new_size > PS_MAX_ARRAY_SIZE / 2)
	    new_size = PS_MAX_ARRAY_SIZE;
		else if (new_size >> 1 < old_size)
	    new_size = (old_size > PS_MAX_ARRAY_SIZE / 2 ? PS_MAX_ARRAY_SIZE :
			old_size << 1);
		else
	    new_size <<= 1;
	    
		{
	    new_array.makeArray(PS_A_READONLY, new_size);
	    XWPSRef * d = new_array.getArray();
	    XWPSRef * s = user_names_p->getArray();
	    for (int i = 0; i < old_size; i++)
	    {
	    	d->assign(s);
	    	d++;
	    	s++;
	    }
	    
	    user_names_p->assign(&new_array);
		}
  }
  
  XWPSRef * p = user_names_p->getArray() + op[-1].value.intval;
  p->assign(op);
  
ret:
  pop(2);
  return 0;
}

int XWPSContextState::zeoclip()
{
	return pgs->eoclip();
}

int XWPSContextState::zinitClip()
{
	return pgs->initClip();
}

int XWPSContextState::zinitViewClip()
{
	return pgs->initViewClip();
}

int XWPSContextState::zlineTo()
{
	return commonTo(&XWPSState::lineTo);
}

int XWPSContextState::zmoveTo()
{
	return commonTo(&XWPSState::moveTo);
}

int XWPSContextState::znewPath()
{
	return pgs->newPath();
}

int XWPSContextState::zpathBbox()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRect box;
  int code;

  code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  code = pgs->upathBbox(&box, op->value.boolval);
  if (code < 0)
		return code;
  code = push(&op, 3);
  if (code < 0)
		return code;
  op[-3].makeReal(box.p.x);
  op[-2].makeReal(box.p.y);
  op[-1].makeReal(box.q.x);
  op->makeReal(box.q.y);
  return 0;
}

int XWPSContextState::pathContinue()
{
	XWPSRef * ep = exec_stack.getCurrentTop();
	XWPSPathEnum *penum = (XWPSPathEnum*)(ep->getStruct());
  XWPSPoint ppts[3];
  int code;

  code = checkOStack(6);
  if (code < 0)
		return code;
		
  code = pgs->pathEnumNext(penum, ppts);
  switch (code) 
  {
		case 0:
	    ep = exec_stack.incCurrentTop(-6);
	    pathCleanup();
	    return PS_O_POP_ESTACK;
	    
		default:
	    return code;
	    
		case ps_pe_moveto:
	    ep[2].assign(ep - 4);
	    pfPush(ppts, 1);
	    break;
	    
		case ps_pe_lineto:
	    ep[2].assign(ep - 3);
	    pfPush(ppts, 1);
	    break;
	    
		case ps_pe_curveto:
	    ep[2].assign(ep - 2);
	    pfPush(ppts, 3);
	    break;
	    
		case ps_pe_closepath:
	    ep[2].assign(ep - 1);
	    break;
  }
  
  ep = exec_stack.incCurrentTop(1);
  ep->makeOper(0, &XWPSContextState::pathContinue);
  ep = exec_stack.incCurrentTop(1);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zpathForall()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSPathEnum *penum;
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
		
  code = checkEStack(8);
  if (code < 0)
		return code;
		
  penum = new XWPSPathEnum;
  code = pgs->pathEnumCopyInit(penum, true);
  if (code < 0) 
		return code;
  
  XWPSRef * ep = exec_stack.incCurrentTop(1);
  ep->makeMarkEStack(PS_ES_FOR, &XWPSContextState::pathCleanup);
  memcpyRef(ep + 1, op - 3, 4);
  ep = exec_stack.incCurrentTop(5);
  ep->makeStruct(currentSpace() | 0, (XWPSStruct*)penum);
  ep = exec_stack.incCurrentTop(1);
  ep->makeOper(0, &XWPSContextState::pathContinue);
  pop(4);
  op -= 4;
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zrcurveTo()
{
	return commonCurve(&XWPSState::rcurveTo);
}

int XWPSContextState::zrlineTo()
{
	return commonTo(&XWPSState::rlineTo);
}

int XWPSContextState::zrmoveTo()
{
	return commonTo(&XWPSState::rmoveTo);
}

int XWPSContextState::zflattenPath()
{
	return pgs->flattenPath();
}

int XWPSContextState::zreversePath()
{
	return pgs->reversePath();
}

int XWPSContextState::zstrokePath()
{
	return pgs->strokePath();
}

int XWPSContextState::zeofill()
{
	return pgs->eofill();
}

int XWPSContextState::zeoviewClip()
{
	return pgs->eoviewClip();
}

int XWPSContextState::zfill()
{
	return pgs->fill();
}

int XWPSContextState::zfillPage()
{
	return pgs->fillPage();
}

int XWPSContextState::zimagePath()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op[-2].checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
    
  code = op[-1].checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  code = op->checkReadType(XWPSRef::String);
  if (code < 0)
		return code;
		
  if (op->size() < ((op[-2].value.intval + 7) >> 3) * op[-1].value.intval)
		return (int)(XWPSError::RangeCheck);
	
  code = pgs->imagePath((int)op[-2].value.intval, (int)op[-1].value.intval,	op->getBytes());
  if (code >= 0)
		pop(3);
  return code;
}

int XWPSContextState::zstroke()
{
	return pgs->stroke();
}

int XWPSContextState::zviewClip()
{
	return pgs->viewClip();
}

int XWPSContextState::zviewClipPath()
{
	return pgs->viewClipPath();
}

int XWPSContextState::gstateUnshare()
{
	XWPSRef * op = op_stack.getCurrentTop();
	XWPSIGStateObj * g = (XWPSIGStateObj*)(op->getStruct());
  XWPSRef *pgsref = &g->gstate;
  XWPSState *pgsA = (XWPSState*)(pgsref->getStruct());
  XWPSState *pnew;
  XWPSIntGState *isp;

  pnew = new XWPSState(0, pgsA);
  isp = (XWPSIntGState*)(pnew->client_data);
  pgsref->makeStruct(currentSpace(), pnew);
  return 0;
}

int XWPSContextState::rectGet(XWPSLocalRects * plr, XWPSRef * op)
{
	int format, code;
  uint n, count;
  XWPSRect *pr;
  double rv[4];

  switch (op->type()) 
  {
		case XWPSRef::Array:
		case XWPSRef::MixedArray:
		case XWPSRef::ShortArray:
		case XWPSRef::String:
	    code = op->numArrayFormat();
	    if (code < 0)
				return code;
	    format = code;
	    count = op->numArraySize(format);
	    if (count % 4)
	    	return (int)(XWPSError::RangeCheck);
	    count /= 4;
	    break;
	    
		default:
	    code = numParams(op, 4, rv);
	    if (code < 0)
				return code;
			
	    plr->pr = plr->rl;
	    plr->count = 1;
	    plr->rl[0].q.x = (plr->rl[0].p.x = rv[0]) + rv[2];
	    plr->rl[0].q.y = (plr->rl[0].p.y = rv[1]) + rv[3];
	    return 4;
	    
  }
  plr->count = count;
  if (count <= MAX_LOCAL_RECTS)
		pr = plr->rl;
  else 
		pr = new XWPSRect[count];
  plr->pr = pr;
  for (n = 0; n < count; n++, pr++) 
  {
		XWPSRef rnum;
		int i;

		for (i = 0; i < 4; i++) 
		{
	    code = op->numArrayGet(this, format, (n << 2) + i, &rnum);
	    switch (code) 
	    {
				case XWPSRef::Integer:
		    	rv[i] = rnum.value.intval;
		    	break;
		    
				case XWPSRef::Real:
		    	rv[i] = rnum.value.realval;
		    	break;
		    	
				default:
		    	return code;
	    }
		}
		pr->q.x = (pr->p.x = rv[0]) + rv[2];
		pr->q.y = (pr->p.y = rv[1]) + rv[3];
  }
  return 1;
}

int XWPSContextState::zcopyGState()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  XWPSState *pgsA;
  XWPSState *pgs1;
  XWPSIntGState *pistate;
    
  int code = op->checkType("igstateobj");
	if (code < 0)
		return code;
		
	code = op1->checkType("igstateobj");
	if (code < 0)
		return code;

  code = gstateUnshare();
  if (code < 0)
		return code;
    
  XWPSIGStateObj * g = (XWPSIGStateObj*)(op->getStruct());
  XWPSRef * p = &g->gstate;
  pgsA = (XWPSState*)(p->getStruct());
  g = (XWPSIGStateObj*)(op1->getStruct());
  p = &g->gstate;
  pgs1 = (XWPSState*)(p->getStruct());
  
  pistate = (XWPSIntGState*)(pgsA->client_data);
  code = pgsA->copyGState(pgs1);
  op1->assign(op);
  pop(1);
  return 0;
}

int XWPSContextState::zcurrentGState()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSState *pgsA;
  XWPSIntGState *pistate;
  
  int code = op->checkType("igstateobj");
	if (code < 0)
		return code;
   
  code = op->checkWrite();
  if (code < 0)
		return code;
		
  code = gstateUnshare();
  if (code < 0)
		return code;
    
  XWPSIGStateObj * g = (XWPSIGStateObj*)(op->getStruct());
  XWPSRef * p = &g->gstate;
  pgsA = (XWPSState*)(p->getStruct());
  
  pistate = (XWPSIntGState*)(pgsA->client_data);
  
  pgsA->currentGState(pgs);
  
  return 0;
}

int XWPSContextState::zcurrentStrokeAdjust()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeBool(pgs->currentStrokeAdjust());
  return 0;
}

int XWPSContextState::zgstate()
{
	XWPSRef * op = op_stack.getCurrentTop();

//  int code;
  XWPSIGStateObj *pigo;
  XWPSState *pnew;
  XWPSIntGState *isp;

  pigo = new XWPSIGStateObj;
  pnew = new XWPSState(pgs, copy_for_gstate);
  isp = (XWPSIntGState*)(pnew->client_data);
  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeAStruct(currentSpace() | PS_A_ALL, pigo);
  pigo->gstate.makeStruct(currentSpace() | 0, pnew);
  return 0;
}

int XWPSContextState::zrectAppend()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSLocalRects lr;
  int npop = rectGet(&lr, op);
  int code;

  if (npop < 0)
		return npop;
    
  code = pgs->rectAppend(lr.pr, lr.count);
  if (code < 0)
		return code;
  pop(npop);
  return 0;
}

int XWPSContextState::zrectClip()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSLocalRects lr;
  int npop = rectGet(&lr, op);
  int code;

  if (npop < 0)
		return npop;
    
  code = pgs->rectClip(lr.pr, lr.count);
  if (code < 0)
		return code;
  pop(npop);
  return 0;
}

int XWPSContextState::zrectFill()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSLocalRects lr;
  int npop = rectGet(&lr, op);
  int code;

  if (npop < 0)
		return npop;
		
  code = pgs->rectFill(lr.pr, lr.count);
  if (code < 0)
		return code;
  pop(npop);
  return 0;
}

int XWPSContextState::zrectStroke()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSMatrix mat;
  XWPSLocalRects lr;
  int npop, code;

  if (op->readMatrix(this, &mat) >= 0) 
  {
		npop = rectGet(&lr, op - 1);
		if (npop < 0)
	    return npop;
		code = pgs->rectStroke(lr.pr, lr.count, &mat);
		npop++;
  } 
  else 
  {
		npop = rectGet(&lr, op);
		if (npop < 0)
	    return npop;
		code = pgs->rectStroke(lr.pr, lr.count, 0);
  }
  
  if (code < 0)
		return code;
  pop(npop);
  return 0;
}

int XWPSContextState::zsetBbox()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double box[4];

  int code = numParams(op, 4, box);

  if (code < 0)
		return code;
  if ((code = pgs->setBbox(box[0], box[1], box[2], box[3])) < 0)
		return code;
  pop(4);
  return 0;
}

int XWPSContextState::zsetGState()
{
	XWPSRef * op = op_stack.getCurrentTop();

	int code = op->checkType("igstateobj");
	if (code < 0)
		return code;
  code = op->checkRead();
  XWPSIGStateObj * g = (XWPSIGStateObj*)(op->getStruct());
  XWPSRef * p = &g->gstate;
  XWPSState * pgsA = (XWPSState*)(p->getStruct());
  
  code = pgs->setGState(pgsA);
  if (code < 0)
		return code;
  pop(1);
  return 0;
}

int XWPSContextState::zsetStrokeAdjust()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
		return code;
		
  pgs->setStrokeAdjust(op->value.boolval);
  pop(1);
  return 0;
}

int XWPSContextState::z1copy()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = zcopy();

  if (code >= 0)
		return code;
    
  if (!op->hasType(XWPSRef::AStruct))
		return code;
  return zcopyGState();
}

int XWPSContextState::inPath(XWPSRef * oppath, XWPSDevice * phdev)
{
	int code = pgs->save();
  int npop;
  double uxy[2];

  if (code < 0)
  {
  	if (phdev)
  		if (phdev->decRef() == 0)
  			delete phdev;
		return code;
	}
    
  code = numParams(oppath, 2, uxy);
  if (code >= 0) 
  {
		XWPSPoint dxy;
		XWPSFixedRect fr;

		pgs->transform(uxy[0], uxy[1], &dxy);
		fr.p.x = fixed_floor(float2fixed(dxy.x));
		fr.p.y = fixed_floor(float2fixed(dxy.y));
		fr.q.x = fr.p.x + fixed_1;
		fr.q.y = fr.p.y + fixed_1;
		code = pgs->clipToRectangle(&fr);
		npop = 2;
  } 
  else 
  {
		XWPSPath *ipath = pgs->path;
		XWPSPath save;
		
		save.assign(ipath);
		pgs->newPath();
		code = upathAppend(oppath);
		if (code >= 0)
	    code = pgs->clipToPath();
		pgs->path->assign(&save, false);
		npop = 1;
  }
  if (code < 0) 
  {
  	if (phdev)
  		if (phdev->decRef() == 0)
  			delete phdev;
		pgs->restore();
		return code;
  }
  
  pgs->setDeviceColor1();
  phdev->width = phdev->height = max_int;
  pgs->setDeviceOnly(phdev);
  return npop;
}

int XWPSContextState::inPathResult(int npop, int code)
{
	XWPSRef * op = op_stack.getCurrentTop();
  bool result;

  pgs->restore();
  if (code == XWPSError::HitDetected)
		result = true;
  else if (code == 0)	
		result = false;
  else
		return code;
  npop--;
  pop(npop);
  op -= npop;
  op->makeBool(result);
  return 0;
}

int XWPSContextState::inTest(int (XWPSState::*paintproc)())
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSDeviceHit * hdev = new XWPSDeviceHit;
  int npop = inPath(op, hdev);
  int code;
  if (npop < 0)
		return npop;
    
  code = (pgs->*paintproc)();
  return inPathResult(npop, code);
}

int XWPSContextState::inUTest(int (XWPSState::*paintproc)())
{
	XWPSDeviceHit * hdev = new XWPSDeviceHit;
  int npop = inUPath(hdev);
  int code;
  if (npop < 0)
		return npop;
  code = (pgs->*paintproc)();
  return inUPathResult(npop, code);
}

int XWPSContextState::inUPath(XWPSDevice * phdev)
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = pgs->save();
  int npop;

  if (code < 0)
		return code;
    
  if ((code = upathAppend(op)) < 0 || (npop = inPath(op - 1, phdev)) < 0) 
  {
		pgs->restore();
		return code;
  }
  return npop + 1;
}

int XWPSContextState::inUPathResult(int npop, int code)
{
	pgs->restore();
  return inPathResult(npop, code);
}

int XWPSContextState::makeUPath(XWPSRef *rupath, 
	              XWPSState *pgsA, 
	              XWPSPath *ppath,
	              bool with_ucache)
{
	int size = (with_ucache ? 6 : 5);
  XWPSPathEnum penum;
  int op;
  XWPSRef *next;
  int code;

  {
		XWPSFixedPoint pts[3];

		penum.init(ppath);
		while ((op = penum.next(pts)) != 0) 
		{
	    switch (op) 
	    {
				case ps_pe_moveto:
				case ps_pe_lineto:
		    	size += 3;
		    	continue;
		    	
				case ps_pe_curveto:
		    	size += 7;
		    	continue;
		    	
				case ps_pe_closepath:
		    	size += 1;
		    	continue;
		    	
				default:
		    	return (int)(XWPSError::Unregistered);
	    }
		}
  }
  rupath->makeArray(currentSpace() | PS_A_ALL | PS_A_EXECUTABLE, size);
  next = rupath->getArray();
  if (with_ucache) 
  {
		if ((code = nameEnterString("ucache", next)) < 0)
	    return code;
		next->setAttrs(PS_A_EXECUTABLE | PS_L_NEW);
		++next;
  } 
  
  {
		XWPSRect bbox;

		pgsA->upathBbox(&bbox, true);
		next->makeReal(bbox.p.x);
		next[1].makeReal(bbox.p.y);
		next[2].makeReal(bbox.q.x);
		next[3].makeReal(bbox.q.y);
		next += 4;
		if ((code = nameEnterString("setbbox", next)) < 0)
	    return code;
		next->setAttrs(PS_A_EXECUTABLE | PS_L_NEW);
		++next;
  }
  {
		XWPSPoint pts[3];

		XWPSPath *save_path = pgsA->path;

		pgsA->path = ppath;
		penum.copyInit(pgsA, false);
		pgsA->path = save_path;
		while ((op = penum.next(pts)) != 0) 
		{
	    const char *opstr;

	    switch (op) 
	    {
				case ps_pe_moveto:
		    	opstr = "moveto";
		    	goto ml;
		    
				case ps_pe_lineto:
		    	opstr = "lineto";
ml:
					next->makeReal(pts[0].x);
		    	next[1].makeReal(pts[0].y);
		    	next += 2;
		    	break;
		    	
				case ps_pe_curveto:
		    	opstr = "curveto";
		    	next->makeReal(pts[0].x);
		    	next[1].makeReal(pts[0].y);
		    	next[2].makeReal(pts[1].x);
		    	next[3].makeReal(pts[1].y);
		    	next[4].makeReal(pts[2].x);
		    	next[5].makeReal(pts[2].y);
		    	next += 6;
		    	break;
		    	
				case ps_pe_closepath:
		    	opstr = "closepath";
		    	break;
		    	
				default:
		    	return (int)(XWPSError::Unregistered);
	    }
	    if ((code = nameEnterString(opstr, next)) < 0)
				return code;
	    next->setAttrs(PS_A_EXECUTABLE);
	    ++next;
		}
  }
  return 0;
}

int XWPSContextState::upathAppend(XWPSRef * oppath)
{
	int code = oppath->checkRead();
	if (code < 0)
		return code;
		
  pgs->newPath();
  
  XWPSRef * p = oppath->getArray();
  if (oppath->hasType(XWPSRef::Array) && oppath->size() == 2 &&
		 p[1].hasType(XWPSRef::String)) 
	{	
		XWPSRef * operands = p;
		int format;
		int repcount = 1;
		uchar *opp;
		uint ocount, i = 0;

		code = operands->numArrayFormat();
		if (code < 0)
	    return code;
		format = code;
		opp = p[1].getBytes();
		ocount = p[1].size();
		while (ocount--) 
		{
	    uchar opx = *opp++;

	    if (opx > UPATH_REPEAT)
				repcount = opx - UPATH_REPEAT;
	    else if (opx > UPATH_MAX_OP)
	    	return (int)(XWPSError::RangeCheck);
	    else 
	    {
				do 
				{
		    	XWPSRef * op = op_stack.getCurrentTop();
		    	uchar opargs = up_nargs[opx];

		    	while (opargs--) 
		    	{
						code = push(&op, 1);
						if (code < 0)
							return code;
						code = operands->numArrayGet(this, format, i++, op);
						switch (code) 
						{
			    		case XWPSRef::Integer:
								op->setTypeAttrs(XWPSRef::Integer, 0);
								break;
								
			    		case XWPSRef::Real:
								op->setTypeAttrs(XWPSRef::Real, 0);
								break;
								
			    		default:
								return op->checkTypeFailed();
						}
		    	}
		    	code = (this->*up_ops[opx])();
		    	if (code < 0)
						return code;
				}	while (--repcount);
				repcount = 1;
	    }
		}
  } 
  else if (oppath->isArray()) 
  {	
		XWPSRef *arp = oppath;
		uint ocount = oppath->size();
		long index = 0;
		int argcount = 0;
		op_proc_t oproc;
		int opx, code;

		for (; index < ocount; index++) 
		{
	    XWPSRef rup;
	    XWPSRef *defp;
	    XWPSRef * op = op_stack.getCurrentTop();

	    arp->arrayGet(this, index, &rup);
	    switch (rup.type()) 
	    {
				case XWPSRef::Integer:
				case XWPSRef::Real:
		    	argcount++;
		    	code = push(&op,1);
		    	if (code < 0)
						return code;
		    	op->assign(&rup);
		    	break;
		    	
				case XWPSRef::Name:
		    	if (!rup.hasAttr(PS_A_EXECUTABLE))
						return rup.checkTypeFailed();
		    	if (dict_stack.getSystemDict()->dictFind(this, &rup, &defp) <= 0)
						return (int)(XWPSError::Undefined);
		    	if (defp->btype() != XWPSRef::Operator)
						return defp->checkTypeFailed();
		    	goto xop;
		    	
				case XWPSRef::Operator:
		    	defp = &rup;
		  
xop:
					if (!defp->hasAttr(PS_A_EXECUTABLE))
						return defp->checkTypeFailed();
		    	oproc = defp->getOper();
		    	for (opx = 0; opx <= UPATH_MAX_OP; opx++)
						if (oproc == up_ops[opx])
			    		break;
		    	if (opx > UPATH_MAX_OP || argcount != up_nargs[opx])
						return defp->checkTypeFailed();
		    	code = (this->*oproc)();
		    	if (code < 0)
						return code;
		    	argcount = 0;
		    	break;
		    	
				default:
		    	return rup.checkTypeFailed();
	    }
		}
		if (argcount)
	    return oppath->checkTypeFailed();
  } 
  else
		return oppath->checkTypeFailed();
  return 0;
}

int XWPSContextState::upathStroke(XWPSMatrix *pmat)
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code, npop;
  XWPSMatrix mat;

  if ((code = op->readMatrix(this, &mat)) >= 0) 
  {
		if ((code = upathAppend(op - 1)) >= 0) 
		{
	    if (pmat)
				*pmat = mat;
	    else
				code = pgs->concat(&mat);
		}
		npop = 2;
  } 
  else 
  {
		if ((code = upathAppend(op)) >= 0)
	    if (pmat)
				pmat->reset();
		npop = 1;
  }
  return (code < 0 ? code : npop);
}

int XWPSContextState::zineofill()
{
	return inTest(&XWPSState::eofill);
}

int XWPSContextState::zinfill()
{
	return inTest(&XWPSState::fill);
}

int XWPSContextState::zinstroke()
{
	return inTest(&XWPSState::stroke);
}

int XWPSContextState::zinueofill()
{
	return inUTest(&XWPSState::eofill);
}

int XWPSContextState::zinufill()
{
	return inUTest(&XWPSState::fill);
}

int XWPSContextState::zinustroke()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = pgs->save();
  int spop, npop;
  XWPSMatrix mat;
  XWPSDeviceHit * hdev;

  if (code < 0)
		return code;
    
  if ((spop = upathStroke(&mat)) < 0) 
  {
		pgs->restore();
		return spop;
  }
  
  hdev = new XWPSDeviceHit;
  if ((npop = inPath(op - spop, hdev)) < 0) 
  {
		pgs->restore();
		return npop;
  }
  if (npop > 1)
		code = pgs->concat(&mat);
  if (code >= 0)
		code = pgs->stroke();
  return inUPathResult(npop + spop, code);
}

int XWPSContextState::zuappend()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = pgs->save();

  if (code < 0)
		return code;
    
  if ((code = upathAppend(op)) >= 0)
		code = pgs->upmergePath();
  pgs->restore();
  if (code < 0)
		return code;
  pop(1);
  return 0;
}

int XWPSContextState::zueofill()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = pgs->save();

  if (code < 0)
		return code;
  if ((code = upathAppend(op)) >= 0)
		code = pgs->eofill();
  pgs->restore();
  if (code < 0)
		return code;
  pop(1);
  return 0;
}

int XWPSContextState::zufill()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = pgs->save();

  if (code < 0)
		return code;
  if ((code = upathAppend(op)) >= 0)
		code = pgs->fill();
  pgs->restore();
  if (code < 0)
		return code;
  pop(1);
  return 0;
}

int XWPSContextState::zupath()
{
	XWPSRef * op = op_stack.getCurrentTop();

  op->checkType(XWPSRef::Boolean);
  return makeUPath(op, pgs, pgs->path, op->value.boolval);
}

int XWPSContextState::zustroke()
{
	int code = pgs->save();
  int npop;

  if (code < 0)
		return code;
    
  if ((code = npop = upathStroke(NULL)) >= 0)
		code = pgs->stroke();
  pgs->restore();
  if (code < 0)
		return code;
  pop(npop);
  return 0;
}

int XWPSContextState::zustrokePath()
{
	XWPSPath save;
	save.assign(pgs->path);
  int code, npop;
  
  if ((code = npop = upathStroke(NULL)) < 0 || (code = pgs->strokePath()) < 0) 
  {
		pgs->path->assign(&save, false);
		return code;
  }
  pop(npop);
  return 0;
}
