/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSContextState.h"

class XWPSMatchRecord
{
public:
	XWPSMatchRecord();
	
	void resetMatch();
	
public:
	XWPSRef best_key, match_key;
  uint priority, no_match_priority;
};

XWPSMatchRecord::XWPSMatchRecord()
{
	priority = 0;
	no_match_priority = 0;
}

void XWPSMatchRecord::resetMatch()
{
	best_key.makeNull();
  match_key.makeNull();
  priority = no_match_priority;
}

class XWPSMKD_ 
{
public:
	XWPSMKD_() {}
	XWPSRef key, dict;
} ;

#define MIN_MEDIA_SIZE 9
void XWPSContextState::makeAdjustmentMatrix(const XWPSPoint * request, 
	                          const XWPSRect * medium,
		                        XWPSMatrix * pmat, 
		                        bool scale, 
		                        int rotate)
{
	double rx = request->x, ry = request->y;
  double mx = medium->q.x, my = medium->q.y;
  
  if (rotate & 1) 
  {
		double temp = rx;

		rx = ry, ry = temp;
  }
  
  if (medium->p.x < MIN_MEDIA_SIZE && mx > rx)
		mx = rx;
  if (medium->p.y < MIN_MEDIA_SIZE && my > ry)
		my = ry;

  pmat->makeTranslation(mx / 2, my / 2);
   
  if (rotate)
		pmat->matrixRotate(pmat, 90.0 * rotate);

  if (scale) 
  {
		double xfactor = mx / rx;
		double yfactor = my / ry;
		double factor = qMin(xfactor, yfactor);

		if (factor < 1)
	    pmat->matrixScale(pmat, factor, factor);
  }
  
  pmat->matrixTranslate(pmat, -request->x / 2, -request->y / 2);
}

int XWPSContextState::matchPageSize(const XWPSPoint * request, 
	                  const XWPSRect * medium, 
	                  int policy,
		                int orient, 
		                bool , 
		                float *best_mismatch, 
		                XWPSMatrix * pmat,
		                XWPSPoint * pmsize)
{
	double rx = request->x, ry = request->y;

  if (policy == 7) 
  {
		*best_mismatch = 0;
		pmat->reset();
		*pmsize = *request;
		return true;
  }
  
  if (rx - medium->p.x >= -5 && rx - medium->q.x <= 5 &&
			ry - medium->p.y >= -5 && ry - medium->q.y <= 5 &&
			(orient < 0 || !(orient & 1))) 
	{
		*best_mismatch = 0;
		makeAdjustmentMatrix(request, medium, pmat, false, (orient >= 0 ? orient : 0));
  } 
  else if (rx - medium->p.y >= -5 && rx - medium->q.y <= 5 &&
	       	ry - medium->p.x >= -5 && ry - medium->q.x <= 5 &&
	       (orient < 0 || (orient & 1))) 
	{
		*best_mismatch = 0;
		makeAdjustmentMatrix(request, medium, pmat, false, (orient >= 0 ? orient : rx < ry ? -1 : 1));
  } 
  else 
  {
		int rotate = (orient >= 0 ? orient :  rx < ry ?	 (medium->q.x > medium->q.y ? -1 : 0) :
	 								(medium->q.x < medium->q.y ? 1 : 0));
		bool larger =	(rotate ? medium->q.y >= rx && medium->q.x >= ry :
	 								medium->q.x >= rx && medium->q.y >= ry);
		bool adjust = false;
		float mismatch = medium->q.x * medium->q.y - rx * ry;

		switch (policy) 
		{
	    default:	
				return false;
	    case 3:	
				adjust = true;
	    case 5:	
				if (fabs(mismatch) >= fabs(*best_mismatch))
		    	return false;
				break;
	    case 4:
				adjust = true;
	    case 6:	
				if (!larger || mismatch >= *best_mismatch)
		    	return false;
				break;
		}
		if (adjust)
	    makeAdjustmentMatrix(request, medium, pmat, !larger, rotate);
		else 
		{
	    XWPSRect req_rect;

	    req_rect.p = *request;
	    req_rect.q = *request;
	    makeAdjustmentMatrix(request, &req_rect, pmat, false, rotate);
		}
		*best_mismatch = mismatch;
  }
  if (pmat->xx == 0) 
  {
		double temp = rx;

		rx = ry, ry = temp;
  }
#define ADJUST_INTO(req, mmin, mmax)\
  (req < mmin ? mmin : req > mmax ? mmax : req)
  
  pmsize->x = ADJUST_INTO(rx, medium->p.x, medium->q.x);
  pmsize->y = ADJUST_INTO(ry, medium->p.y, medium->q.y);
#undef ADJUST_INTO
  return true;
}

int XWPSContextState::zmatchMedia()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * preq = op - 3;
  XWPSRef * pattr = op - 2;
  XWPSRef * ppol = op - 1;
  XWPSRef * pkeys = op;	
  int policy_default;
  float best_mismatch = (float)max_long;
  float mbest = best_mismatch;
  XWPSMatchRecord match;
  XWPSRef no_priority;
  XWPSRef *ppriority;
  int mepos, orient;
  bool roll;
  int code;
  int ai;
  XWPSMKD_ aelt;
  
  if (pattr->hasType(XWPSRef::Null)) 
  {
		checkOp(op, 4);
		op[-3].makeNull();
		op[-2].makeTrue();
		pop(2);
		return 0;
  }
  
  code = preq->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = preq->checkDictRead();
  if (code < 0)
  	return code;
  	
  pattr->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = pattr->checkDictRead();
  if (code < 0)
  	return code;
  	
  ppol->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = ppol->checkDictRead();
  if (code < 0)
  	return code;
  	
  code = pkeys->checkArray();
  if (code < 0)
  	return code;
  	
  code = pkeys->checkRead();
  if (code < 0)
  	return code;
  	
  switch (code = preq->dictIntNullParam(this, "MediaPosition", 0, 0x7fff, 0, &mepos)) 
  {
		default:
	    return code;
	    
		case 2:
		case 1:
	    mepos = -1;
		case 0:;
  }
  
  switch (code = preq->dictIntNullParam(this, "Orientation", 0, 3, 0, &orient)) 
  {
		default:
	    return code;
	    
		case 2:
		case 1:
	    orient = -1;
		case 0:;
  }
  
  code = preq->dictBoolParam(this, "RollFedMedia", false, &roll);
  if (code < 0)
		return code;
    
  code = ppol->dictIntParam(this, "PolicyNotFound", 0, 7, 0,  &policy_default);
  if (code < 0)
		return code;
    
  if (pattr->dictFindString(this, "Priority", &ppriority) > 0) 
  {
		code = ppriority->checkArrayOnly();
		if (code < 0)
			return code;
		code = ppriority->checkRead();
		if (code < 0)
			return code;
  } 
  else 
  {
		no_priority.makeArray(PS_A_READONLY);
		ppriority = &no_priority;
  }
    
  match.no_match_priority = ppriority->size();
  match.resetMatch();
  for (ai = pattr->dictFirst(); (ai = pattr->dictNext(this, ai, (XWPSRef *)&aelt)) >= 0;) 
  {
		if (aelt.dict.hasType(XWPSRef::Dictionary) &&
	    	aelt.dict.dictAccessRef()->hasAttr(PS_A_READ) &&
	    	aelt.key.hasType(XWPSRef::Integer) &&
	    	(mepos < 0 || aelt.key.value.intval == mepos)) 
	  {
	    bool match_all;
	    uint ki, pi;

	    code = aelt.dict.dictBoolParam(this, "MatchAll", false,  &match_all);
	    if (code < 0)
				return code;
	    for (ki = 0; ki < pkeys->size(); ki++) 
	    {
				XWPSRef key;
				XWPSRef kstr;
				XWPSRef *prvalue;
				XWPSRef *pmvalue;
				XWPSRef *ppvalue;
				int policy;

				pkeys->arrayGet(this, ki, &key);
				if (aelt.dict.dictFind(this, &key, &pmvalue) <= 0)
		    	continue;
				if (preq->dictFind(this, &key, &prvalue) <= 0 || prvalue->hasType(XWPSRef::Null)) 
				{
		    	if (match_all)
						goto no;
		    	else
						continue;
				}
				
				if (ppol->dictFind(this, &key, &ppvalue) > 0) 
				{
		    	ppvalue->checkTypeOnly(XWPSRef::Integer);
		    	policy = ppvalue->value.intval;
				} 
				else
		    	policy = policy_default;
		    	
				if (key.hasType(XWPSRef::Name) &&	(nameStringRef(&key, &kstr),
		     		kstr.size() == 8 &&  !memcmp(kstr.getBytes(), "PageSize", 8))) 
		    {
		    	XWPSMatrix ignore_mat;
		    	XWPSPoint ignore_msize;

		    	if (zmatchPageSize(prvalue, pmvalue, policy, orient, roll,
					 			&best_mismatch, &ignore_mat,	 &ignore_msize) <= 0)
						goto no;
				} 
				else if (!prvalue->objEq(this, pmvalue))
		    	goto no;
	    }
	    
	    if (best_mismatch < mbest) 
	    {
				mbest = best_mismatch;
				match.resetMatch();
	    }
	    
	    for (pi = match.priority; pi > 0;) 
	    {
				XWPSRef pri;

				pi--;
				ppriority->arrayGet(this, pi, &pri);
				if (aelt.key.objEq(this, &pri)) 
				{	
		    	match.best_key.assign(&aelt.key);
		    	match.priority = pi;
		    	break;
				}
	    }
	    
	    match.match_key.assign(&aelt.key);
no:
			;
		}
  }
  
  if (match.match_key.hasType(XWPSRef::Null)) 
  {
		op[-3].makeFalse();
		pop(3);
  } 
  else 
  {
		if (match.best_key.hasType(XWPSRef::Null))
	    op[-3].assign(&match.match_key);
		else
	    op[-3].assign(&match.best_key);
		op[-2].makeTrue();
		pop(2);
  }
  return 0;
}

int XWPSContextState::zmatchPageSize()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSMatrix mat;
  float ignore_mismatch = (float)max_long;
  XWPSPoint media_size;
  int orient;
  bool roll;
  int code;

  code = op[-3].checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  if (op[-2].hasType(XWPSRef::Null))
		orient = -1;
  else 
  {
		op[-2].checkIntLEU(3);
		orient = (int)op[-2].value.intval;
  }
    
  op[-1].checkType(XWPSRef::Boolean);
  if (code < 0)
		return code;
		
  roll = op[-1].value.boolval;
  code = zmatchPageSize(op - 5, op - 4, (int)op[-3].value.intval,
			    orient, roll,  &ignore_mismatch, &mat, &media_size);
  
  switch (code) 
  {
		default:
	    return code;
		case 0:
	    op[-5].makeFalse();
	    pop(5);
	    break;
		case 1:
	    code = op->writeMatrixIn(&mat);
	    if (code < 0 && !op->hasType(XWPSRef::Null))
				return code;
	    op[-5].assign(op);
	    op[-4].makeReal(media_size.x);
	    op[-3].makeReal(media_size.y);
	    op[-2].makeTrue();
	    pop(2);
	    break;
  }
  return 0;
}

int XWPSContextState::zmatchPageSize(XWPSRef * pvreq, 
	                   XWPSRef * pvmed,
		                 int policy, 
		                 int orient, 
		                 bool roll,
		                 float *best_mismatch, 
		                 XWPSMatrix * pmat, 
		                 XWPSPoint * pmsize)
{
	uint nr, nm;

  int code = pvreq->checkArray();
  if (code < 0)
		return code;
		
  nr = pvreq->size();
  code = pvmed->checkArray();
  if (code < 0)
		return code;
		
  nm = pvmed->size();
  if (!((nm == 2 || nm == 4) && (nr == 2 || nr == nm)))
  {
		return (int)(XWPSError::RangeCheck);
	}
    
  {
		XWPSRef rv[6];
		uint i;
		double v[6];
		int code;

		pvreq->arrayGet(this, 0, &rv[0]);
		pvreq->arrayGet(this, 1, &rv[1]);
		for (i = 0; i < 4; ++i)
	    pvmed->arrayGet(this, i % nm, &rv[i + 2]);
		if ((code = numParams(rv + 5, 6, v)) < 0)
	    return code;
		{
	    XWPSPoint request;
	    XWPSRect medium;

	    request.x = v[0], request.y = v[1];
	    medium.p.x = v[2], medium.p.y = v[3],
			medium.q.x = v[4], medium.q.y = v[5];
	    return matchPageSize(&request, &medium, policy, orient,   roll, best_mismatch, pmat, pmsize);
		}
  }
}
#undef MIN_MEDIA_SIZE