/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSStream.h"
#include "XWPSText.h"
#include "XWPSFont.h"
#include "XWPSState.h"
#include "XWPSContextState.h"

class XWPSStrEnum
{
public:
	XWPSStrEnum();
	XWPSStrEnum(XWPSContextState *ctx, const char *ksa);
	
	int cfontNextString();
	
	void init(XWPSContextState *ctx, const char *ksa);
	
public:
	XWPSContextState *i_ctx_p;
  const char *str_array;
  XWPSRef next;
};

XWPSStrEnum::XWPSStrEnum()
{
	i_ctx_p = 0;
	str_array = 0;
}

XWPSStrEnum::XWPSStrEnum(XWPSContextState *ctx, const char *ksa)
{
	init(ctx, ksa);
}

void XWPSStrEnum::init(XWPSContextState *ctx, const char *ksa)
{
	i_ctx_p = ctx;
  str_array = ksa;
}

int XWPSStrEnum::cfontNextString()
{
	uchar *str = (uchar *)str_array;
  uint len = (str[0] << 8) + str[1];

  if (len == 0xffff) 
  {
		next.makeNull();
		str_array += 2;
		return 0;
  } 
  else if (len >= 0xff00) 
  {
		int code;

		len = ((len & 0xff) << 8) + str[2];
		code = i_ctx_p->cfontRefFromString(&next, str_array + 3, len);
		if (code < 0)
	    return code;
		str_array += 3 + len;
		return 0;
  }
  next.makeString(PS_AVM_FOREIGN, len, str + 2);
  str_array += 2 + len;
  return 1;
}

class XWPSKeyEnum
{
public:
	XWPSKeyEnum() {}
	XWPSKeyEnum(XWPSContextState *ctx, const XWPSCFontDictKeys *pkeys, const char *ksa);
	
	int cfontPutNext(XWPSRef * pdict, XWPSRef * pvalue);
	
	void init(XWPSContextState *ctx, const XWPSCFontDictKeys *pkeys, const char *ksa);
	
	bool moreKeys();
	
public:
	XWPSCFontDictKeys keys;
  XWPSStrEnum strings;
};

XWPSKeyEnum::XWPSKeyEnum(XWPSContextState *ctx, const XWPSCFontDictKeys *pkeys, const char *ksa)
{
	init(ctx, pkeys, ksa);
}

int XWPSKeyEnum::cfontPutNext(XWPSRef * pdict, XWPSRef * pvalue)
{
	XWPSContextState *i_ctx_p = strings.i_ctx_p;
  XWPSCFontDictKeys * const kp = &keys;
  XWPSRef kname;
  int code;

  if (pdict->getDict() == 0) 
  	pdict->dictAlloc(i_ctx_p->iimemory()->space, kp->num_enc_keys + kp->num_str_keys + kp->extra_slots);
  
  if (kp->num_enc_keys) 
  {
		const XWPSCharIndex *skp = kp->enc_keys++;

		code = i_ctx_p->registeredEncoding(skp->encx)->arrayGet(i_ctx_p, (long)(skp->charx), &kname);
		kp->num_enc_keys--;
  } 
  else 
  {	
		code = strings.cfontNextString();
		if (code != 1)
	    return (code < 0 ? code : (int)(XWPSError::Fatal));
			code = i_ctx_p->nameRef(strings.next.getBytes(),	strings.next.size(), &kname, 0);
		kp->num_str_keys--;
  }
  if (code < 0)
		return code;
  return i_ctx_p->dictPut(pdict, &kname, pvalue);
}

void XWPSKeyEnum::init(XWPSContextState *ctx, const XWPSCFontDictKeys *pkeys, const char *ksa)
{
	keys = *pkeys;
  strings.init(ctx, ksa);
}

bool XWPSKeyEnum::moreKeys()
{
	return (keys.num_enc_keys | keys.num_str_keys);
}

struct PSCFontProcs
{
	int (XWPSContextState::*ref_dict_create)(XWPSRef *, const XWPSCFontDictKeys *, const char *, XWPSRef *);
  int (XWPSContextState::*string_dict_create)(XWPSRef *, const XWPSCFontDictKeys *, const char *, const char *);
  int (XWPSContextState::*num_dict_create)(XWPSRef *,
  	                                       const XWPSCFontDictKeys *, 
  	                                        const char *, 
  	                                        XWPSRef *, 
  	                                        const char *);
  int (XWPSContextState::*name_array_create)(XWPSRef *, const char *, int);
  int (XWPSContextState::*string_array_create)(XWPSRef *, const char *, int, uint);
  int (XWPSContextState::*scalar_array_create)(XWPSRef *, XWPSRef *, int , uint);
  int (XWPSContextState::*name_create)(XWPSRef *, const char *);
  int (XWPSContextState::*ref_from_string)(XWPSRef *, const char *, uint);
};

static const PSCFontProcs ccfont_procs = {
    &XWPSContextState::cfontRefDictCreate,
    &XWPSContextState::cfontStringDictCreate,
    &XWPSContextState::cfontNumDictCreate,
    &XWPSContextState::cfontNameArrayCreate,
    &XWPSContextState::cfontStringArrayCreate,
    &XWPSContextState::cfontScalarArrayCreate,
    &XWPSContextState::cfontNameCreate,
    &XWPSContextState::cfontRefFromString
};

int XWPSContextState::cfontNameArrayCreate(XWPSRef * parray, const char * ksa,int size)
{
	parray->makeArray(currentSpace() | PS_A_READONLY, size);
  XWPSRef *aptr = parray->getArray();
  int i;
  XWPSStrEnum senum(this, ksa);
		
  for (i = 0; i < size; i++, aptr++) 
  {
		XWPSRef nref;
		int code = senum.cfontNextString();

		if (code != 1)
	    return (code < 0 ? code : (int)(XWPSError::Fatal));
		code = nameRef(senum.next.getBytes(),	senum.next.size(), &nref, 0);
		if (code < 0)
	    return code;
		aptr->assign(&nref);
  }
  return 0;
}

int XWPSContextState::cfontNameCreate(XWPSRef * pnref, const char *str)
{
	return nameRef((const uchar *)str, strlen(str), pnref, 0);
}

int XWPSContextState::cfontNumDictCreate(XWPSRef * pdict,
		                     const XWPSCFontDictKeys * kp, 
		                     const char * ksa,
		                     XWPSRef * values, 
		                     const char *lengths)
{
	XWPSKeyEnum kenum(this, kp, ksa);
  XWPSRef *vp = values;
  const char *lp = lengths;
  XWPSRef vnum;

  pdict->makeNull();
  while (kenum.moreKeys()) 
  {
		int len = (lp == 0 ? 0 : *lp++);
		int code;

		if (len == 0)
		{
	    vnum.assign(vp);
	    vp++;
	  }
		else 
		{
	    --len;
	    vnum.makeArray(PS_AVM_FOREIGN | PS_A_READONLY, len, vp);
	    vp += len;
		}
		code = kenum.cfontPutNext(pdict, &vnum);
		if (code < 0)
	    return code;
  }
  return 0;
}

int XWPSContextState::cfontRefDictCreate(XWPSRef *pdict,
		                     const XWPSCFontDictKeys *kp, 
		                     const char * ksa,
		      								XWPSRef *values)
{
	XWPSKeyEnum kenum(this, kp, ksa);
  XWPSRef *vp = values;

  pdict->makeNull();
  while (kenum.moreKeys()) 
  {
		XWPSRef *pvalue = vp++;
		int code = kenum.cfontPutNext(pdict, pvalue);

		if (code < 0)
	    return code;
  }
  return 0;
}

int XWPSContextState::cfontRefFromString(XWPSRef * pref, const char *str, uint len)
{
	XWPSScannerState sstate;
  XWPSStream s;
  int code;

  sstate.init(false);
  s.readString((const uchar *)str, len, false);
  code = scanToken(&s, pref, &sstate);
  return (code <= 0 ? code : (int)(XWPSError::Fatal));
}

int XWPSContextState::cfontScalarArrayCreate(XWPSRef * parray,
			  										 XWPSRef *va, 
			  										 int size, 
			  										 uint attrs)
{
	parray->makeArray(currentSpace() | attrs, size);
  XWPSRef *aptr = parray->getArray();
  uint elt_attrs = attrs;
    
  memcpyRef(aptr, va, size);
  
  for (int i = 0; i < size; i++, aptr++)
		aptr->setAttrs(elt_attrs);
  return 0;
}

int XWPSContextState::cfontStringArrayCreate(XWPSRef * parray,
			                      const char * ksa, 
			                      int size, 
			                      uint attrs)
{
	parray->makeArray(currentSpace() | PS_A_READONLY, size);
  XWPSRef *aptr = parray->getArray();
  int i;
  XWPSStrEnum senum(this, ksa);
    
  for (i = 0; i < size; i++, aptr++) 
  {
		int code = senum.cfontNextString();

		switch (code) 
		{
	    default:
				return code;
				
	    case 1:
				senum.next.setAttrs(attrs);
	    case 0:	
				;
		}
		aptr->assign(&senum.next);
  }
  return 0;
}

int XWPSContextState::cfontStringDictCreate(XWPSRef *pdict,
			                      const XWPSCFontDictKeys *kp, 
			                      const char * ksa,
			                       const char * kva)
{
	XWPSKeyEnum kenum(this, kp, ksa);
  XWPSStrEnum senum(this, kva);
  uint attrs = kp->value_attrs;

  pdict->makeNull();
  while (kenum.moreKeys()) 
  {
		int code = senum.cfontNextString();

		switch (code) 
		{
	    default:
				return code;
				
	    case 1:	
				senum.next.setAttrs(attrs);
	    case 0:	
				;
		}
		code = kenum.cfontPutNext(pdict, &senum.next);
		if (code < 0)
	    return code;
  }
  return 0;
}
	
#define ccfont_version 19	

int XWPSContextState::ccfontFProcs(int *pnum_fprocs, const ccfont_fproc ** pfprocs)
{
	*pnum_fprocs = 1;
  *pfprocs = &fprocs[0];
  return ccfont_version;
}

int XWPSContextState::zgetCCFont()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  const ccfont_fproc *fprocsA;
  int nfonts;
  int index;

  code = ccfontFProcs(&nfonts, &fprocsA);
  if (code != ccfont_version)
		return (int)(XWPSError::InvalidFont);

  if (op->hasType(XWPSRef::Null)) 
  {
		op->makeInt(nfonts);
		return 0;
  }
  op->checkType(XWPSRef::Integer);
  index = op->value.intval;
  if (index < 0 || index >= nfonts)
		return (int)(XWPSError::RangeCheck);

  return (this->*(fprocsA[index]))(&ccfont_procs, op);
}

int XWPSContextState::buildFDArrayFont(XWPSRef *op,
		      						 XWPSFontBase **ppfont,
		      						PSFontType ftype, 
		      						XWPSBuildProcRefs * pbuild)
{
	XWPSFontBase *pfont = *ppfont;
  XWPSFontData *pdata;
  int code = buildOutlineFont(op, &pfont, ftype, pbuild, bf_options_none,
				     		&XWPSContextState::buildFDArraySubFont);
  static const double bbox[4] = { 0, 0, 0, 0 };
  XWPSUid uid;

  if (code < 0)
		return code;
    
  pdata = (XWPSFontData*)(pfont->client_data);
  pdata->CharStrings.makeNull();
  uid.invalid();
  pfont->initSimpleFont(bbox, &uid);
  pfont->encoding_index = ENCODING_INDEX_UNKNOWN;
  pfont->nearest_encoding_index = ENCODING_INDEX_UNKNOWN;
  pfont->key_name = pfont->font_name;
  *ppfont = pfont;
  return 0;
}

int XWPSContextState::buildFDArraySubFont(XWPSRef *op,
		       								XWPSFontBase **ppfont,
		       								PSFontType ftype,
		       								XWPSBuildProcRefs * pbuild,
		       								PSBuildFontOptions )
{
	XWPSFont *pfont = *ppfont;
  int code = buildSubFont(op, &pfont, ftype, pbuild, NULL, op);

  if (code >= 0)
		*ppfont = (XWPSFontBase *)pfont;
  return code;
}

int XWPSContextState::buildFont(XWPSRef * op, 
	             XWPSFont ** ppfont, 
	             PSFontType ftype,
	             XWPSBuildProcRefs * pbuild,
	             PSBuildFontOptions options)
{
	XWPSRef kname;			/* t_string */
  XWPSRef *pftype;
  XWPSRef *pencoding = 0;
  bool bitmapwidths;
  int exactsize, inbetweensize, transformedchar;
  int wmode;
  int code;
  XWPSFont *pfont = *ppfont;
  XWPSRef *pfid;
  XWPSRef *aop = op->dictAccessRef();

  getFontName(&kname, op - 1);
  if (op->dictFindString(this, "FontType", &pftype) <= 0 ||
			!pftype->hasType(XWPSRef::Integer) ||
			pftype->value.intval != (int)ftype)
	{
		if (pfont)
		{
			if (pfont->decRef() == 0)
				delete pfont;
		}
		*ppfont = 0;
	  return (int)(XWPSError::InvalidFont);
	}
	
  if (op->dictFindString(this, "Encoding", &pencoding) <= 0) 
  {
		if (!(options & bf_Encoding_optional))
		{
			if (pfont)
		  {
			  if (pfont->decRef() == 0)
				  delete pfont;
		  }
		  *ppfont = 0;
	  	return (int)(XWPSError::InvalidFont);
	  }
  } 
  else 
  {
		if (!pencoding->isArray())
		{
			if (pfont)
		  {
			  if (pfont->decRef() == 0)
				  delete pfont;
		  }
		  *ppfont = 0;
	  	return (int)(XWPSError::InvalidFont);
	  }
  }
  
  if ((code = op->dictIntParam(this, "WMode", 0, 1, 0, &wmode)) < 0 ||
			(code = op->dictBoolParam(this, "BitmapWidths", false, &bitmapwidths)) < 0 ||
			(code = op->dictIntParam(this, "ExactSize", 0, 2, fbit_use_bitmaps, &exactsize)) < 0 ||
			(code = op->dictIntParam(this, "InBetweenSize", 0, 2, fbit_use_outlines, &inbetweensize)) < 0 ||
			(code = op->dictIntParam(this, "TransformedChar", 0, 2, fbit_use_outlines, &transformedchar)) < 0)
	{
		if (pfont)
		{
			if (pfont->decRef() == 0)
				delete pfont;
		}
		*ppfont = 0;
		return code;
	}
  
  code = op->dictFindString(this, "FID", &pfid);
  if (code > 0) 
  {
		if (!pfid->hasType(XWPSRef::FontID))
		{
			if (pfont)
		  {
			  if (pfont->decRef() == 0)
				  delete pfont;
		  }
		  *ppfont = 0;
	  	return (int)(XWPSError::InvalidFont);
	  }
	  
	  if (pfont)
	  {
	  	if (pfont->decRef() == 0)
	  		delete pfont;
	  }
		pfont = (XWPSFont*)(pfid->getStruct());
		*ppfont = pfont;
		pfont->incRef();
		if (pfont->base == pfont) 
		{	
	    if (!level2Enabled())
	  		return (int)(XWPSError::InvalidFont);
	  			
			XWPSFontData * fd = (XWPSFontData*)(pfont->client_data);
	    if (fd->dict.objEq(this, op)) 
				return 1;
		} 
		else 
		{
	    XWPSMatrix mat;
	    XWPSRef fname;			/* t_string */

	    code = subFontParams(op, &mat, &fname);
	    if (code < 0)
				return code;
				
	    code = 1;
	    copyFontName(&pfont->font_name, &fname);
	    goto set_name;
		}
  }
  
  if (!aop->hasAttr(PS_A_WRITE))
  {
  	if (pfont)
		{
			if (pfont->decRef() == 0)
				delete pfont;
		}
		*ppfont = 0;
	  return (int)(XWPSError::InvalidFont);
	}
  
  {
		XWPSRef encoding;
		if (pencoding) 
		{
	    encoding.assign(pencoding);
	    pencoding->assign(&encoding);
		}
		code = buildSubFont(op, &pfont, ftype, pbuild, pencoding, op);
		if (code < 0)
	    return code;
  }
  pfont->BitmapWidths = bitmapwidths;
  pfont->ExactSize = (FBitType)exactsize;
  pfont->InBetweenSize = (FBitType)inbetweensize;
  pfont->TransformedChar = (FBitType)transformedchar;
  pfont->WMode = wmode;
  code = 0;
  
set_name:
  copyFontName(&pfont->key_name, &kname);
  *ppfont = pfont;
  return code;
}

int XWPSContextState::buildFontProcs(XWPSRef * op, XWPSBuildProcRefs * pbuild)
{
	int ccode, gcode;
  XWPSRef *pBuildChar;
  XWPSRef *pBuildGlyph;

  ccode = op->checkType(XWPSRef::Dictionary);  	
  ccode = op->dictFindString(this, "BuildChar", &pBuildChar);
  gcode = op->dictFindString(this, "BuildGlyph", &pBuildGlyph);
  if (ccode <= 0) 
  {
		if (gcode <= 0)
	    return (int)(XWPSError::InvalidFont);
		pbuild->BuildChar.makeNull();
  } 
  else 
  {
		pBuildChar->checkProc();
		pbuild->BuildChar.assign(pBuildChar);
  }
  if (gcode <= 0)
		pbuild->BuildGlyph.makeNull();
  else 
  {
		pBuildGlyph->checkProc();
		pbuild->BuildGlyph.assign(pBuildGlyph);
  }
  return 0;
}

int XWPSContextState::buildOutlineFont(XWPSRef * op, 
	                     XWPSFontBase ** ppfont,
		                   PSFontType ftype, 
		                   XWPSBuildProcRefs * pbuild,
		                   PSBuildFontOptions options,
		                   build_base_font_proc_t build_base_font)
{
	int painttype;
  float strokewidth;
  XWPSFontBase *pfont = *ppfont;
  int code = op->dictIntParam(this, "PaintType", 0, 3, 0, &painttype);

  if (code < 0)
  {
  	if (pfont)
  	{
  		if (pfont->decRef() == 0)
  			delete pfont;
  	}
  	*ppfont = 0;
		return code;
	}
    
  code = op->dictFloatParam(this, "StrokeWidth", 0.0, &strokewidth);
  if (code < 0)
  {
  	if (pfont)
  	{
  		if (pfont->decRef() == 0)
  			delete pfont;
  	}
  	*ppfont = 0;
		return code;
	}
    
  code = (this->*build_base_font)(op, ppfont, ftype, pbuild,  options);
  if (code != 0)
		return code;
    
  pfont = *ppfont;
  pfont->PaintType = painttype;
  pfont->StrokeWidth = strokewidth;
  return 0;
}

int XWPSContextState::buildPrimitiveFont(XWPSRef * op, 
	                       XWPSFontBase ** ppfont,
			                   PSFontType ftype, 
			                   XWPSBuildProcRefs * pbuild,
			                   PSBuildFontOptions options)
{
	XWPSRef *pcharstrings = 0;
  XWPSRef CharStrings;
  XWPSFontBase *pfont = *ppfont;
  XWPSFontData *pdata;
  int code;

  if (op->dictFindString(this, "CharStrings", &pcharstrings) <= 0) 
  {
		if (!(options & bf_CharStrings_optional))
		{
			if (pfont)
		  {
			  if (pfont->decRef() == 0)
				  delete pfont;
		  }
		  *ppfont = 0;
	    return (int)(XWPSError::InvalidFont);
	  }
  } 
  else 
  {
		XWPSRef *ignore;

		if (!pcharstrings->hasType(XWPSRef::Dictionary))
		{
			if (pfont)
		  {
			  if (pfont->decRef() == 0)
				  delete pfont;
		  }
		  *ppfont = 0;
	    return (int)(XWPSError::InvalidFont);
	  }
		if ((options & bf_notdef_required) != 0 && pcharstrings->dictFindString(this, ".notdef", &ignore) <= 0 )
	  {
	  	if (pfont)
		  {
			  if (pfont->decRef() == 0)
				  delete pfont;
		  }
		  *ppfont = 0;
	    return (int)(XWPSError::InvalidFont);
	  }
	  
		CharStrings.assign(pcharstrings);
  }
  code = buildOutlineFont(op, ppfont, ftype, pbuild,	options, &XWPSContextState::buildSimpleFont);
  if (code != 0)
		return code;
    
  pfont = *ppfont;
  pdata = (XWPSFontData*)(pfont->client_data);
  if (pcharstrings)
		pdata->CharStrings.assign(&CharStrings);
  else
		pdata->CharStrings.makeNull();
  if (pfont->UID.isValid() &&	!op->dictCheckUidParam(this, &pfont->UID))
		pfont->UID.invalid();
  return 0;
}

int XWPSContextState::buildProcNameRefs(XWPSBuildProcRefs * pbuild,
		                    const char *bcstr, 
		                    const char *bgstr)
{
	int code;

  if (!bcstr)
		pbuild->BuildChar.makeNull();
  else 
  {
		if ((code = nameRef((const uchar *)bcstr, strlen(bcstr), &pbuild->BuildChar, 0)) < 0)
	    return code;
		pbuild->BuildChar.setAttrs(PS_A_EXECUTABLE);
  }
  
  if (!bgstr)
		pbuild->BuildGlyph.makeNull();
  else 
  {
		if ((code = nameRef((const uchar *)bgstr, strlen(bgstr), &pbuild->BuildGlyph, 0)) < 0)
	    return code;
		pbuild->BuildGlyph.setAttrs(PS_A_EXECUTABLE);
  }
  return 0;
}

int XWPSContextState::buildSimpleFont(XWPSRef * op, 
	                    XWPSFontBase ** ppfont,
		                  PSFontType ftype, 
		                  XWPSBuildProcRefs * pbuild,
		                  PSBuildFontOptions options)
{
	double bbox[4];
  XWPSUid uid;
  int code;
  XWPSFontBase *pfont = *ppfont;

  code = fontBboxParam(op, bbox);
  if (code < 0)
  {
  	if (pfont)
		{
			if (pfont->decRef() == 0)
				delete pfont;
		}
		*ppfont = 0;
		return code;
	}
		
  if ((options & bf_FontBBox_required) &&
		bbox[0] == 0 && bbox[1] == 0 && bbox[2] == 0 && bbox[3] == 0)
	{
		if (pfont)
		{
			if (pfont->decRef() == 0)
				delete pfont;
		}
		*ppfont = 0;
	  return (int)(XWPSError::InvalidFont);
	}
  code = op->dictUidParam(&uid, 0, this);
  if (code < 0)
  {
  	if (pfont)
		{
			if (pfont->decRef() == 0)
				delete pfont;
		}
		*ppfont = 0;
		return code;
	}
    
  if ((options & bf_UniqueID_ignored) && uid.isUniqueID())
		uid.invalid();
  code = buildFont(op, (XWPSFont**)ppfont, ftype, pbuild, options);
  if (code != 0)
		return code;
	
	pfont = *ppfont;
  pfont->procs.init_fstack_.init_fstack = &XWPSFont::initFStackDefault;
  pfont->procs.define_font_.define_font = &XWPSFont::defineFontNo;
  pfont->procs.make_font_.make_font = &XWPSFont::zbaseMakeFont;
  pfont->procs.next_char_glyph_.next_char_glyph = &XWPSTextEnum::nextCharGlyphDefault;
  pfont->initSimpleFont(bbox, &uid);
  lookupSimpleFontEncoding(pfont);
  return 0;
}

int XWPSContextState::buildSubFont(XWPSRef *op, 
	                 XWPSFont **ppfont,
		               PSFontType ftype, 
		               XWPSBuildProcRefs * pbuild, 
		               XWPSRef *pencoding,
		               XWPSRef *fid_op)
{
	XWPSMatrix mat;
  XWPSRef fname;	
  XWPSFont *pfont = *ppfont;
  XWPSFontData *pdata;
  uint space = idmemory()->iallocSpace();
  
  int code = subFontParams(op, &mat, &fname);
  if (code < 0)
  {
  	if (pfont)
  	{
  		if (pfont->decRef() == 0)
  			delete pfont;
  	}
  	*ppfont = 0;
		return code;
	}

	idmemory()->setSpace(op->space());
  pdata = new XWPSFontData;
  if (fid_op)
		code = addFID(fid_op, pfont, true);
  
  pdata->dict.assign(op);
  pdata->BuildChar.assign(&pbuild->BuildChar);
  pdata->BuildGlyph.assign(&pbuild->BuildGlyph);
  if (pencoding)
		pdata->Encoding.assign(pencoding);
  pfont->client_data = pdata;
  pfont->FontType = ftype;
  pfont->FontMatrix = mat;
  pfont->BitmapWidths = false;
  pfont->ExactSize = fbit_use_bitmaps;
  pfont->InBetweenSize = fbit_use_outlines;
  pfont->TransformedChar = fbit_use_outlines;
  pfont->WMode = 0;
  pfont->procs.encode_char_.encode_char = &XWPSFont::zfontEncodeChar;
  pfont->procs.callbacks.glyph_name_.glyph_name = &XWPSFont::zfontGlyphName;
  pfont->procs.callbacks.known_encode_.known_encode = &XWPSFont::zfontKnownEncode;
  idmemory()->setSpace(space);
  copyFontName(&pfont->font_name, &fname);
  *ppfont = pfont;
  return 0;
}

int  XWPSContextState::addFID(XWPSRef * fp, XWPSFont * pfont, bool c)
{
	XWPSRef fid;

  fid.makeFontID(iimemory()->space | PS_A_READONLY, pfont);
  return c ? dictPutString(fp, "FID", &fid) : fp->dictPutString(this, "FID", &fid, NULL);
}

int  XWPSContextState::acquireCidSystemInfo(XWPSRef *psia, XWPSRef *op)
{
	XWPSRef *prcidsi;

  if (op->dictFindString(this, "CIDSystemInfo", &prcidsi) <= 0) 
  {
		psia->makeArray(PS_A_READONLY);
		return 1;
  }
  if (prcidsi->hasType(XWPSRef::Dictionary)) 
  {
		psia->makeArray(PS_A_READONLY, 1, prcidsi);
		return 0;
  }
  
  if (!prcidsi->isArray())
		return prcidsi->checkTypeFailed();
  psia->assign(prcidsi);
  return 0;
}

int XWPSContextState::buildCharstringFont(XWPSRef * op, 
	                         XWPSBuildProcRefs *pbuild,
		                       PSFontType ftype, 
		                       XWPSCharstringFontRefs *pfr,
		                       XWPSFontType1 *pdata1, 
		                       PSBuildFontOptions options)
{
	int code = charstringFontParams(op, pfr, pdata1);
  XWPSFontType1 *pfont = pdata1;

  if (code < 0)
		return code;
		
  code = buildPrimitiveFont(op, (XWPSFontBase**)&pfont, ftype, pbuild, options);
  if (code != 0)
		return code;
		
  pfont->charstringFontInit(pfr);
  return defineFont(pfont);
}

int XWPSContextState:: buildFont1Or4(XWPSRef * op, 
	                   XWPSBuildProcRefs * pbuild,
	                    PSFontType ftype, 
	                    PSBuildFontOptions options)
{
	XWPSCharstringFontRefs refs;
 	int code = charstringFontGetRefs(op, &refs);

  if (code < 0)
		return code;
		
	XWPSFontType1 * pfont = new XWPSFontType1;
	pfont->i_ctx_p = this;
	pfont->id = nextIDS(1);
  pfont->interpret = &XWPSType1State::type1Interpret;
  pfont->lenIV = DEFAULT_LENIV_1;
  return buildCharstringFont(op, pbuild, ftype, &refs, pfont, options);
}

int  XWPSContextState::buildTrueTypeFont(XWPSRef * op, 
	                       XWPSFontType42 **ppfont,
		       							 PSFontType ftype,
		                     const char *bcstr, 
		                     const char *bgstr,
		                     PSBuildFontOptions options)
{
	XWPSBuildProcRefs build;
  XWPSRef sfnts, GlyphDirectory;
  XWPSFontType42 *pfont = *ppfont;
  XWPSFontData *pdata;
  int code;

  code = buildProcNameRefs(&build, bcstr, bgstr);
  if (code < 0)
  {
  	if (pfont)
		{
			if (pfont->decRef() == 0)
				delete pfont;
		}
		*ppfont = 0;
		return code;
	}
   
  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  {
  	if (pfont)
		{
			if (pfont->decRef() == 0)
				delete pfont;
		}
		*ppfont = 0;
		return code;
	}
	
  if ((code = fontStringArrayParam(op, "sfnts", &sfnts)) < 0 ||
			(code = fontGlyphDirectoryParam(op, &GlyphDirectory)) < 0)
	{
		if (pfont)
		{
			if (pfont->decRef() == 0)
				delete pfont;
		}
		*ppfont = 0;
		return code;
	}
    
  code = buildPrimitiveFont(op, (XWPSFontBase**)ppfont,  ftype, &build, options);
  if (code != 0)
		return code;
    
  pfont = *ppfont;
  pdata = (XWPSFontData*)(pfont->client_data);
  pdata->u.type42.sfnts->assign(&sfnts);
  pdata->u.type42.CIDMap->makeNull();
  pdata->u.type42.GlyphDirectory->assign(&GlyphDirectory);
  pfont->string_proc = &XWPSFontType42::z42stringProc;
  pfont->proc_data = (char *)pdata;
  code = pfont->type42FontInit();
  if (code < 0)
		return code;
		
  if (!GlyphDirectory.hasType(XWPSRef::Null)) 
  {
		pfont->get_outline = &XWPSFontType42::z42gdirGetOutline;
		pfont->procs.enumerate_glyph_.enumerate_glypht42 = &XWPSFontType42::z42gdirEnumerateGlyph;
  } 
  else
		pfont->procs.enumerate_glyph_.enumerate_glypht42 = &XWPSFontType42::z42enumerateGlyph;
  pfont->procs.encode_char_.encode_chart42 = &XWPSFontType42::z42encodeChar;
  pfont->procs.glyph_info_.glyph_infot42 = &XWPSFontType42::z42glyphInfo;
  pfont->procs.glyph_outline_.glyph_outlinet42 = &XWPSFontType42::z42glyphOutline;
  return 0;
}

int  XWPSContextState::charstringFontGetRefs(XWPSRef * op, XWPSCharstringFontRefs *pfr)
{
	int code = op->checkType(XWPSRef::Dictionary);
	if (code < 0)
		return code;
		
  if (op->dictFindString(this, "Private", &pfr->Private) <= 0 ||
			!pfr->Private->hasType(XWPSRef::Dictionary))
	{
	  return (int)(XWPSError::InvalidFont);
	}
  
  pfr->no_subrs.makeArray(0);
  if (pfr->Private->dictFindString(this, "OtherSubrs", &pfr->OtherSubrs) > 0) 
  {
		if (!pfr->OtherSubrs->isArray())
	    return pfr->Private->checkTypeFailed();
  } 
  else
		pfr->OtherSubrs = &pfr->no_subrs;
    
  if (pfr->Private->dictFindString(this, "Subrs", &pfr->Subrs) > 0) 
  {
		if (!pfr->Subrs->isArray())
	    return pfr->Subrs->checkTypeFailed();
  } 
  else
		pfr->Subrs = &pfr->no_subrs;
  pfr->GlobalSubrs = &pfr->no_subrs;
  return 0;
}

int  XWPSContextState::charstringFontParams(XWPSRef * op, 
	                          XWPSCharstringFontRefs *pfr,
		                        XWPSFontType1 *pdata1)
{
	XWPSRef *pprivate = pfr->Private;
  int code;

  if ((code = pprivate->dictIntParam(this, "lenIV", -1, 255, pdata1->lenIV, &pdata1->lenIV)) < 0 ||
			(code = pprivate->dictUintParam(this, "subroutineNumberBias",	0, max_uint, 
			        pdata1->subroutineNumberBias,&pdata1->subroutineNumberBias)) < 0 ||
			(code = pprivate->dictIntParam(this, "BlueFuzz", 0, 1999, 1,  &pdata1->BlueFuzz)) < 0 ||
			(code = pprivate->dictFloatParam(this, "BlueScale", 0.039625, &pdata1->BlueScale)) < 0 ||
			(code = pprivate->dictFloatParam(this, "BlueShift", 7.0, &pdata1->BlueShift)) < 0 ||
			(code = pdata1->BlueValues.count = 	pprivate->dictFloatArrayParam(this, 
			        "BlueValues", max_BlueValues * 2,	&pdata1->BlueValues.values[0], NULL)) < 0 ||
			(code = pprivate->dictFloatParam(this, "ExpansionFactor", 0.06, &pdata1->ExpansionFactor)) < 0 ||
			(code = pdata1->FamilyBlues.count =	pprivate->dictFloatArrayParam(this, 
							"FamilyBlues", max_FamilyBlues * 2,	&pdata1->FamilyBlues.values[0], NULL)) < 0 ||
			(code = pdata1->FamilyOtherBlues.count = pprivate->dictFloatArrayParam(this,
							"FamilyOtherBlues", max_FamilyOtherBlues * 2,	&pdata1->FamilyOtherBlues.values[0], NULL)) < 0 ||
			(code = pprivate->dictBoolParam(this, "ForceBold", false,	&pdata1->ForceBold)) < 0 ||
			(code = pprivate->dictIntParam(this, 
							"LanguageGroup", min_int, max_int, 0, &pdata1->LanguageGroup)) < 0 ||
			(code = pdata1->OtherBlues.count =	pprivate->dictFloatArrayParam(this, 
							"OtherBlues", max_OtherBlues * 2,	&pdata1->OtherBlues.values[0], NULL)) < 0 ||
			(code = pprivate->dictBoolParam(this, "RndStemUp", true,	&pdata1->RndStemUp)) < 0 ||
			(code = pdata1->StdHW.count = pprivate->dictFloatArrayCheckParam(this, 
			        "StdHW", 1,  &pdata1->StdHW.values[0], NULL, 0, XWPSError::RangeCheck)) < 0 ||
			(code = pdata1->StdVW.count =	 pprivate->dictFloatArrayCheckParam(this, 
							"StdVW", 1,  &pdata1->StdVW.values[0], NULL,  0, XWPSError::RangeCheck)) < 0 ||
			(code = pdata1->StemSnapH.count = pprivate->dictFloatArrayParam(this, 
							"StemSnapH", max_StemSnap,	&pdata1->StemSnapH.values[0], NULL)) < 0 ||
			(code = pdata1->StemSnapV.count =	 pprivate->dictFloatArrayParam(this, 
			        "StemSnapV", max_StemSnap,	&pdata1->StemSnapV.values[0], NULL)) < 0 ||
			(code = pdata1->WeightVector.count = op->dictFloatArrayParam(this, 
			         "WeightVector", max_WeightVector,pdata1->WeightVector.values, NULL)) < 0)
			return code;
			
  {
		float max_zone_height = 1.0;

#define SCAN_ZONE(z)\
    findZoneHeight(&max_zone_height, pdata1->z.count, pdata1->z.values);

		SCAN_ZONE(BlueValues);
		SCAN_ZONE(OtherBlues);
		SCAN_ZONE(FamilyBlues);
		SCAN_ZONE(FamilyOtherBlues);

#undef SCAN_ZONE

		if (pdata1->BlueScale * max_zone_height > 1.0)
	    pdata1->BlueScale = 1.0 / max_zone_height;
  }
  
  if (pdata1->LanguageGroup > 1 || pdata1->LanguageGroup < 0)
		pdata1->LanguageGroup = 0;
  return 0;
}

int  XWPSContextState::cidFontDataParam(XWPSRef * op, XWPSFontCid0 *pdata, XWPSRef *pGlyphDirectory)
{
	int code;
  XWPSRef *pgdir;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
		return code;
		
  if ((code = cidFontSystemInfoParam(&pdata->CIDSystemInfo, op)) < 0 ||
			(code = op->dictIntParam(this, "CIDCount", 0, max_int, -1, &pdata->CIDCount)) < 0)
		return code;
		
  if (op->dictFindString(this, "GlyphDirectory", &pgdir) <= 0) 
  {
		pGlyphDirectory->makeNull();
		return op->dictIntParam(this, "GDBytes", 1, MAX_GDBytes, 0, &pdata->GDBytes);
  }
  
  if (pgdir->hasType(XWPSRef::Dictionary) || pgdir->isArray()) 
  {
		pGlyphDirectory->assign(pgdir);
		code = op->dictIntParam(this, "GDBytes", 1, MAX_GDBytes, 1,  &pdata->GDBytes);
		if (code == 1) 
		{
	    pdata->GDBytes = 0;
	    code = 0;
		}
		return code;
  } 
  else 
		return pgdir->checkTypeFailed();
}

int  XWPSContextState::cidFontDataParam(XWPSRef * op, XWPSFontCid2 *pdata, XWPSRef *pGlyphDirectory)
{
	int code;
  XWPSRef *pgdir;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
		return code;
		
  if ((code = cidFontSystemInfoParam(&pdata->CIDSystemInfo, op)) < 0 ||
			(code = op->dictIntParam(this, "CIDCount", 0, max_int, -1, &pdata->CIDCount)) < 0)
		return code;
		
  if (op->dictFindString(this, "GlyphDirectory", &pgdir) <= 0) 
  {
		pGlyphDirectory->makeNull();
		return op->dictIntParam(this, "GDBytes", 1, MAX_GDBytes, 0, &pdata->GDBytes);
  }
  
  if (pgdir->hasType(XWPSRef::Dictionary) || pgdir->isArray()) 
  {
		pGlyphDirectory->assign(pgdir);
		code = op->dictIntParam(this, "GDBytes", 1, MAX_GDBytes, 1,  &pdata->GDBytes);
		if (code == 1) 
		{
	    pdata->GDBytes = 0;
	    code = 0;
		}
		return code;
  } 
  else 
		return pgdir->checkTypeFailed();
}

int  XWPSContextState::cidFontSystemInfoParam(XWPSCidSystemInfo *pcidsi, XWPSRef *prfont)
{
	XWPSRef *prcidsi;

  if (prfont->dictFindString(this, "CIDSystemInfo", &prcidsi) <= 0)
		return (int)(XWPSError::RangeCheck);
	
  return cidSystemInfoParam(pcidsi, prcidsi);
}

int  XWPSContextState::cidSystemInfoParam(XWPSCidSystemInfo *pcidsi, XWPSRef *prcidsi)
{
	XWPSRef *pregistry;
  XWPSRef *pordering;
  int code;

  if (!prcidsi->hasType(XWPSRef::Dictionary))
		return prcidsi->checkTypeFailed();
    
  if (prcidsi->dictFindString(this, "Registry", &pregistry) <= 0 ||
			prcidsi->dictFindString(this, "Ordering", &pordering) <= 0)
	{
		return (int)(XWPSError::RangeCheck);
	}	
  
  code = pregistry->checkReadTypeOnly(XWPSRef::String);
  if (code < 0)
		return code;
    
  code = pordering->checkReadTypeOnly(XWPSRef::String);
  if (code < 0)
		return code;
		
	if (pcidsi->Registry.bytes)
	{
		if (pcidsi->Registry.bytes->decRef() == 0)
			delete pcidsi->Registry.bytes;
		pcidsi->Registry.bytes = 0;
	}
	if (pcidsi->Ordering.bytes)
	{
		if (pcidsi->Ordering.bytes->decRef() == 0)
			delete pcidsi->Ordering.bytes;
		pcidsi->Ordering.bytes = 0;
	}
	pcidsi->Registry.size = pregistry->size();
  pcidsi->Registry.data = pregistry->getBytes();
  pcidsi->Registry.bytes = pregistry->value.bytes->arr;
  if (pcidsi->Registry.bytes)
  	pcidsi->Registry.bytes->incRef();
  pcidsi->Ordering.size = pordering->size();
  pcidsi->Ordering.data = pordering->getBytes();
  pcidsi->Ordering.bytes = pordering->value.bytes->arr;
  if (pcidsi->Ordering.bytes)
  	pcidsi->Ordering.bytes->incRef();
  code = prcidsi->dictIntParam(this, "Supplement", 0, max_int, -1, &pcidsi->Supplement);
  return (code < 0 ? code : 0);
}

void XWPSContextState::copyFontName(PSFontName * pfstr, XWPSRef * pfname)
{
	uint size = pfname->size();

  if (size > ps_font_name_max)
		size = ps_font_name_max;
  memcpy(&pfstr->chars[0], pfname->getBytes(), size);
  pfstr->chars[size] = 0;
  pfstr->size = size;
}

int  XWPSContextState::cshowContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
	XWPSRef * esp = exec_stack.getCurrentTop();
  XWPSTextEnum *penum = (XWPSTextEnum*)(esp->getStruct());
  int code;

  code = checkEStack(4);
  code = penum->process();
  if (code != TEXT_PROCESS_INTERVENE) 
  {
		code = opShowContinueDispatch(0, code);
		if (code == PS_O_PUSH_ESTACK)
		{
			esp = exec_stack.getCurrentTop();
	    esp[-1].makeOper(0, &XWPSContextState::cshowContinue);
	  }
		return code;
  }
  
  {
  	esp = exec_stack.getCurrentTop();
		XWPSRef *pslot = esp - 1;
		XWPSPoint wpt;
		XWPSFont *font = penum->currentFont();
		XWPSFont *root_font = pgs->rootFont();
		XWPSFont *scaled_font;
		XWPSFontData * fdata = (XWPSFontData*)(font->client_data);
		XWPSRef * fdict = &(fdata->dict);
		XWPSFontData * rootfdata = (XWPSFontData*)(root_font->client_data);
		XWPSRef * rootfdict = &(rootfdata->dict);
		
		uint font_space = fdict->space();
		uint root_font_space = rootfdict->space();

		penum->currentWidth(&wpt);
		if (font == root_font)
	    scaled_font = font;
		else 
		{
			uint save_space = idmemory()->current_space;
			idmemory()->setSpace(font_space);
	    code = font->makeFont(font->dir, &root_font->FontMatrix, &scaled_font);
	    idmemory()->setSpace(save_space);
	    if (code < 0)
				return code;
		}
	
		code = push(&op, 3);
		if (code < 0)
			return code;
		op[-2].makeInt(penum->currentChar() & 0xff);
		op[-1].makeReal(wpt.x);
		op->makeReal(wpt.y);
		esp = exec_stack.getCurrentTop();
		font->incRef();
		esp[-5].makeStruct(font_space, font);
		esp = exec_stack.getCurrentTop();
		root_font->incRef();
		esp[-6].makeStruct(root_font_space, root_font);
		esp = exec_stack.incCurrentTop(1);
		esp->makeOper(0, &XWPSContextState::cshowRestoreFont);
		pgs->setCurrentFont(scaled_font);
		esp = exec_stack.incCurrentTop(1);
		esp->assign(pslot);	
  }
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::cshowRestoreFont()
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	pgs->setFont((XWPSFont*)(esp[-6].getStruct()));
  pgs->setCurrentFont((XWPSFont*)(esp[-5].getStruct()));
  return cshowContinue();
}

int  XWPSContextState::defineFont(XWPSFont * pfont)
{
	return (pfont->base == pfont && pfont->dir == 0 ?  pfont->defineFont(ifont_dir) : 0);
}

int  XWPSContextState::ensureCharEntry(XWPSRef * op, 
	                     const char *kstr,
		                   uchar * pvalue, 
		                   int default_value)
{
	XWPSRef *pentry;

  if (op->dictFindString(this, kstr, &pentry) <= 0) 
  {
		XWPSRef ent;

		ent.makeInt(default_value);
		*pvalue = (uchar) default_value;
		return dictPutString(op, kstr, &ent);
  } 
  else 
  {
		pentry->checkIntLEUOnly(255);
		*pvalue = (uchar) pentry->value.intval;
		return 0;
  }
}

int  XWPSContextState::fdArrayElement(XWPSFontType1 **ppfont, XWPSRef *prfd)
{
	XWPSCharstringFontRefs refs;
  XWPSBuildProcRefs build;
  XWPSFontType1 *pfont;
  
  int fonttype = 1;	
  int code = charstringFontGetRefs(prfd, &refs);

  if (code < 0 ||	(code = prfd->dictIntParam(this, "FontType", 1, 2, 1, &fonttype)) < 0)
		return code;
		
	pfont = new XWPSFontType1;
	pfont->i_ctx_p = this;
  switch (fonttype) 
  {
    case 1:
			pfont->interpret = &XWPSType1State::type1Interpret;
			pfont->subroutineNumberBias = 0;
			pfont->lenIV = DEFAULT_LENIV_1;
			code = charstringFontParams(prfd, &refs, pfont);
			if (code < 0)
	    	return code;
			code = buildProcNameRefs(&build, "%Type1BuildChar", "%Type1BuildGlyph");
			break;	
			
    case 2:
			code = type2FontParams(prfd, &refs, pfont);
			if (code < 0)
	    	return code;
			code = charstringFontParams(prfd, &refs, pfont);
	    	return code;
			code = buildProcNameRefs(&build, "%Type2BuildChar", "%Type2BuildGlyph");
			break;
			
    default:
			return (int)(XWPSError::Fatal);
  }
  if (code < 0)
		return code;
  
  code = buildFDArrayFont(prfd, (XWPSFontBase**)&pfont, (PSFontType)fonttype, &build);
  if (code < 0)
		return code;
		
  pfont->charstringFontInit(&refs);
  pfont->type1Procs.glyph_data = &XWPSFontType1::z9FDArrayGlyphData;
  pfont->type1Procs.seac_data = &XWPSFontType1::z9FDArraySeacData;
  *ppfont = pfont;
  return 0;
}

void XWPSContextState::findZoneHeight(float *pmax_height, int count, const float *values)
{
	int i;
  float zone_height;

  for (i = 0; i < count; i += 2)
		if ((zone_height = values[i + 1] - values[i]) > *pmax_height)
	    *pmax_height = zone_height;
}

int  XWPSContextState::fontGDirGetOutline(XWPSRef *pgdir, 
	                        long glyph_index,
		                      XWPSString * pgstr)
{
	XWPSRef iglyph;
  XWPSRef gdef;
  XWPSRef *pgdef;
  int code;

  if (pgdir->hasType(XWPSRef::Dictionary)) 
  {
		iglyph.makeInt(glyph_index);
		code = pgdir->dictFind(this, &iglyph, &pgdef) - 1;
  } 
  else 
  {
		code = pgdir->arrayGet(this, glyph_index, &gdef);
		pgdef = &gdef;
  }
  
  if (code < 0) 
  {
		pgstr->data = 0;
		pgstr->size = 0;
  } 
  else if (!pgdef->hasType(XWPSRef::String)) 
		return pgdef->checkTypeFailed();
  else 
  {
  	if (pgstr->bytes)
  	{
  		if (pgstr->bytes->decRef() == 0)
  			delete pgstr->bytes;
  		pgstr->bytes = 0;
  	}
		pgstr->data = pgdef->getBytes();
		pgstr->size = pgdef->size();
		pgstr->bytes = pgdef->value.bytes->arr;
		if (pgstr->bytes)
			pgstr->bytes->incRef();
  }
  return 0;
}

int XWPSContextState:: fontGlyphDirectoryParam(XWPSRef * op, XWPSRef *pGlyphDirectory)
{
	XWPSRef *pgdir;

  if (op->dictFindString(this, "GlyphDirectory", &pgdir) <= 0)
		pGlyphDirectory->makeNull();
  else if (!pgdir->hasType(XWPSRef::Dictionary) && !pgdir->isArray())
		return pgdir->checkTypeFailed();
  else
	pGlyphDirectory->assign(pgdir);
  return 0;
}

int  XWPSContextState::fontParam(XWPSRef * pfdict, XWPSFont ** ppfont)
{
	XWPSRef *pid;
  XWPSFont *pfont;
  XWPSFontData *pdata;

  int code = pfdict->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  if (pfdict->dictFindString(this, "FID", &pid) <= 0 || !pid->hasType(XWPSRef::FontID))
	  return (int)(XWPSError::InvalidFont);
    
  pfont = (XWPSFont*)(pid->getStruct());
  pdata = (XWPSFontData*)(pfont->client_data);
  if (!pdata->dict.objEq(this, pfdict))
	  return (int)(XWPSError::InvalidFont);
	
  *ppfont = pfont;
  if (pfont == 0)
	  return (int)(XWPSError::InvalidFont);
  return 0;
}

int XWPSContextState:: fontStringArrayParam(XWPSRef * op, const char *kstr, XWPSRef *psa)
{
	XWPSRef *pvsa;
  XWPSRef rstr0;
  int code;

  if (op->dictFindString(this, kstr, &pvsa) <= 0)
	  return (int)(XWPSError::InvalidFont);
	  	
  psa->assign(pvsa);
  
  if ((code = pvsa->arrayGet(this, 0L, &rstr0)) < 0)
		return code;
  
  if (!rstr0.hasType(XWPSRef::String))
		return rstr0.checkTypeFailed();
  return 0;
}

int  XWPSContextState::getCidSystemInfo(XWPSCidSystemInfo *pcidsi, 
	                      XWPSRef *psia, 
	                      uint index)
{
	XWPSRef rcidsi;
  int code = psia->arrayGet(this, (long)index, &rcidsi);

  if (code < 0 || rcidsi.hasType(XWPSRef::Null)) 
		return 1;
		
  return cidSystemInfoParam(pcidsi, &rcidsi);
}

void XWPSContextState::getFontName(XWPSRef * pfname, XWPSRef * op)
{
	switch (op->type()) 
	{
		case XWPSRef::String:
	    pfname->assign(op);
	    break;
	    
		case XWPSRef::Name:
	    nameStringRef(op, pfname);
	    break;
	    
		default:
	    pfname->makeString(PS_A_READONLY);
  }
}

void XWPSContextState::lookupSimpleFontEncoding(XWPSFontBase * pfont)
{
	XWPSFontData * fd = (XWPSFontData*)(pfont->client_data);
	XWPSRef *pfe = &fd->Encoding;
  int index;

  for (index = NUM_KNOWN_REAL_ENCODINGS; --index >= 0;)
		if (pfe->objEq(this, registeredEncoding(index)))
	    break;
  pfont->encoding_index = (PSEncodingIndex)index;
  if (index < 0) 
  {
		int near_index = -1;
		uint esize = pfe->size();
		uint best = esize / 3;

		for (index = NUM_KNOWN_REAL_ENCODINGS; --index >= 0;) 
		{
	    XWPSRef *pre = registeredEncoding(index);
	    bool r_packed = pre->hasType(XWPSRef::ShortArray);
	    bool f_packed = !pfe->hasType(XWPSRef::Array);
	    uint match = esize;
	    int i;
	    XWPSRef fchar, rchar;
	    XWPSRef *pfchar = &fchar;

	    if (pre->size() != esize)
				continue;
	    for (i = esize; --i >= 0;) 
	   	{
				uint rnidx;

				if (r_packed)
		    	rnidx = packed_name_index(pre->getPacked() + i);
				else 
				{
		    	pre->arrayGet(this, (long)i, &rchar);
		    	rnidx = nameIndex(&rchar);
				}
				if (f_packed)
		    	pfe->arrayGet(this, (long)i, &fchar);
				else
		    	pfchar = pfe->getArray() + i;
				if (!pfchar->hasType(XWPSRef::Name) ||
		    		nameIndex(pfchar) != rnidx)
		    	if (--match <= best)
						break;
	    }
	    if (match > best)
				best = match, near_index = index;
		}
		index = near_index;
  }
  pfont->nearest_encoding_index = (PSEncodingIndex)index;
}

int  XWPSContextState::makeFont(const XWPSMatrix * pmat)
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * fp = op - 1;
  XWPSFont *oldfont, *newfont;
  int code;
  XWPSRef *pencoding = 0;

  code = fontParam(fp, &oldfont);
  if (code < 0)
		return code;
		
	XWPSFontData *pdata = 0;  
  {
  	uint space = idmemory()->iallocSpace();
  	idmemory()->setSpace(fp->space());
		if (fp->dictFindString(this, "Encoding", &pencoding) > 0 && !pencoding->isArray())
	    code = (int)(XWPSError::InvalidFont);
		else 
		{
	    XWPSRef olddict;
			
			pdata = (XWPSFontData*)(oldfont->client_data);
	    olddict.assign(&pdata->dict);
	    pdata->dict.assign(fp);
	    code = oldfont->makeFont(ifont_dir, pmat, &newfont);
	    pdata->dict.assign(&olddict);
		}
		
		idmemory()->setSpace(space);
  }
  if (code < 0)
		return code;
		
	pdata = (XWPSFontData*)(newfont->client_data);
  if (pencoding != 0 &&	!pencoding->objEq(this, &pdata->Encoding)) 
  {
		if (newfont->FontType == ft_composite)
	    return (int)(XWPSError::RangeCheck);
	  
		pdata->Encoding.assign(pencoding);
		lookupSimpleFontEncoding((XWPSFontBase *) newfont);
  }
  fp->assign(&pdata->dict);
  pop(1);
  return 0;
}

void XWPSContextState::makeUintArray(XWPSRef * op, const uint * intp, int count)
{
	for (int i = 0; i < count; i++, op++, intp++)
		op->makeInt(*intp);
}

bool XWPSContextState::sameFontDict(XWPSFontData *pdata, 
	                  XWPSFontData *podata,
	                  const char *key)
{
	XWPSRef *pvalue;
  bool present = pdata->dict.dictFindString(this, key, &pvalue) > 0;
  XWPSRef *povalue;
  bool opresent = podata->dict.dictFindString(this, key, &povalue) > 0;

  return (present == opresent && (present <= 0 || pvalue->objEq(this, povalue)));
}

int XWPSContextState:: stringArrayAccessProc(XWPSRef *psa, 
	                           int modulus, 
	                           ulong offset,
			                       uint length, 
			                       const uchar **pdata)
{
	ulong left = offset;
  uint index = 0;

  for (;; ++index) 
  {
		XWPSRef rstr;
		int code = psa->arrayGet(this, index, &rstr);
		uint size;

		if (code < 0)
	    return code;
		if (!rstr.hasType(XWPSRef::String))
	    return rstr.checkTypeFailed();
	    
		size = rstr.size() & -modulus;
		if (left < size) 
		{
	    if (left + length > size)
	    	return (int)(XWPSError::RangeCheck);
	    		
	    *pdata = rstr.getBytes() + left;
	    return 0;
		}
		left -= size;
  }
}

int  XWPSContextState::subFontParams(XWPSRef *op, XWPSMatrix *pmat, XWPSRef *pfname)
{
	XWPSRef *pmatrix;
  XWPSRef *pfontname;

  if (op->dictFindString(this, "FontMatrix", &pmatrix) <= 0 ||
			pmatrix->readMatrix(this, pmat) < 0)
	{
	  return (int)(XWPSError::InvalidFont);
	}
  if (op->dictFindString(this, "FontName", &pfontname) > 0)
		getFontName(pfname, pfontname);
  else
		pfname->makeString(PS_A_READONLY);
  return 0;
}

int  XWPSContextState::type2FontParams(XWPSRef * , 
	                     XWPSCharstringFontRefs *pfr,
		                   XWPSFontType1 *pdata1)
{
	int code;
  float dwx, nwx;

  pdata1->interpret = &XWPSType1State::type2Interpret;
  pdata1->lenIV = DEFAULT_LENIV_2;
  pdata1->subroutineNumberBias = pfr->Subrs->subrBias();
  
  if (pfr->Private->dictFindString(this, "GlobalSubrs", &pfr->GlobalSubrs) > 0) 
  {
		if (!pfr->GlobalSubrs->isArray())
	    return pfr->GlobalSubrs->checkTypeFailed();
  }
  pdata1->gsubrNumberBias = pfr->GlobalSubrs->subrBias();
  if ((code = pfr->Private->dictUintParam(this, "gsubrNumberBias",	0, max_uint, pdata1->gsubrNumberBias,
														&pdata1->gsubrNumberBias)) < 0 ||
			(code = pfr->Private->dictFloatParam(this, "defaultWidthX", 0.0, &dwx)) < 0 ||
			(code = pfr->Private->dictFloatParam(this, "nominalWidthX", 0.0, &nwx)) < 0)
		return code;
    
  pdata1->defaultWidthX = float2fixed(dwx);
  pdata1->nominalWidthX = float2fixed(nwx);
  {
		XWPSRef *pirs;

		if (pfr->Private->dictFindString(this, "initialRandomSeed", &pirs) <= 0)
	    pdata1->initialRandomSeed = 0;
		else if (!pirs->hasType(XWPSRef::Integer))
	    return pirs->checkTypeFailed();
		else
	    pdata1->initialRandomSeed = pirs->value.intval;
  }
  return 0;
}

int XWPSContextState::zbfontInit()
{
	registered_Encodings.makeArray(currentSpace() | PS_A_ALL, NUM_KNOWN_ENCODINGS);
	for (int i = 0; i < NUM_KNOWN_ENCODINGS; i++)
		registeredEncoding(i)->makeArray(0);
		
  initialEnterName("registeredencodings", &registered_Encodings);
  return 0;
}

int XWPSContextState::zbuildCMap()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  XWPSRef *pcmapname;
  XWPSRef *puidoffset;
  XWPSRef *pcodemapdata;
  XWPSRef *pcodemap;
  XWPSRef rname, rcidsi, rcoderanges, rdefs, rnotdefs;
  XWPSCMap *pcmap = 0;
  XWPSCidSystemInfo *pcidsi = 0;
  XWPSRef rcmap;
  uint i;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
 	
  code = op->checkDictWrite();
  if (code < 0)
  	return code;
  	
  pcmap = new XWPSCMap;
  if ((code = op->dictIntParam(this, "CMapType", 0, 1, -1, &pcmap->CMapType)) < 0 ||
			(code = op->dictFloatParam(this, "CMapVersion", 0.0, &pcmap->CMapVersion)) < 0 ||
			(code = op->dictUidParam(&pcmap->uid, 0, this)) < 0 ||
			(code = op->dictIntParam(this, "WMode", 0, 1, 0, &pcmap->WMode)) < 0)
	{
		return code;
	}
    
  if ((code = op->dictFindString(this, "CMapName", &pcmapname)) <= 0) 
  {
		return (int)(XWPSError::RangeCheck);
  }
  
  if (!pcmapname->hasType(XWPSRef::Name)) 
  {
		return (int)(XWPSError::TypeCheck);
  }
    
  nameStringRef(pcmapname, &rname);
  pcmap->CMapName.size = rname.size();
  pcmap->CMapName.data = rname.getBytes();
  pcmap->CMapName.bytes = rname.value.bytes->arr;
  if (rname.value.bytes->arr)
  	rname.value.bytes->arr->incRef();
  if (op->dictFindString(this, "UIDOffset", &puidoffset) > 0) 
  {
		if (!puidoffset->hasType(XWPSRef::Integer)) 
		{
	    code = (int)(XWPSError::TypeCheck);
	    goto fail;
		}
		pcmap->UIDOffset = puidoffset->value.intval; /* long, not int */
  }
  
  if (op->dictFindString(this, ".CodeMapData", &pcodemapdata) <= 0 ||
			!pcodemapdata->hasType(XWPSRef::Array) ||
				pcodemapdata->size() != 3 ||
			op->dictFindString(this, "CodeMap", &pcodemap) <= 0 ||
			!pcodemap->hasType(XWPSRef::Null)) 
	{
		code = (int)(XWPSError::RangeCheck);
		goto fail;
  }
    
  if ((code = acquireCidSystemInfo(&rcidsi, op)) < 0)
		goto fail;
    
  pcidsi = 0;
  if (rcidsi.size() > 0)
    pcidsi = new XWPSCidSystemInfo[rcidsi.size()]; 
  pcmap->CIDSystemInfo = pcidsi;
  pcmap->num_fonts = rcidsi.size();
  for (i = 0; i < rcidsi.size(); ++i) 
  {
		code = getCidSystemInfo(pcidsi + i, &rcidsi, i);
		if (code < 0)
	    goto fail;
  }
  pcodemapdata->arrayGet(this, 0L, &rcoderanges);
  pcodemapdata->arrayGet(this, 1L, &rdefs);
  pcodemapdata->arrayGet(this, 2L, &rnotdefs);
  if ((code = pcmap->acquireCodeRanges(this, &rcoderanges)) < 0)
		goto fail;
    
  if ((code = pcmap->def.acquireCodeMap(this, &rdefs, pcmap)) < 0)
		goto fail;
    
  if ((code = pcmap->notdef.acquireCodeMap(this, &rnotdefs, pcmap)) < 0)
		goto fail;
  
  pcmap->mark_glyph = &XWPSContextState::zfontMarkGlyphName;
  pcmap->mark_glyph_data = 0;
  pcmap->glyph_name = &XWPSContextState::zfcmapGlyphName;
  pcmap->glyph_name_data = 0;
  rcmap.makeStruct(PS_A_READONLY | currentSpace(), pcmap);
  code = dictPutString(op, "CodeMap", &rcmap);
  if (code < 0)
		goto fail;
    
  return zreadOnly();
  
fail:
  pcmap->CIDSystemInfo = 0;
  return code;
}

int XWPSContextState::zbuildFont0()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef fdepvector;
  XWPSRef *pprefenc;
  XWPSFontType0 *pfont= new XWPSFontType0;
  pfont->i_ctx_p = this;
  pfont->id = nextIDS(1);
  XWPSFontData *pdata;
  XWPSRef save_FID;
  int i;
  int code = 0;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  {
		XWPSRef *pfmaptype;
		XWPSRef *pfdepvector;

		if (op->dictFindString(this, "FMapType", &pfmaptype) <= 0 ||
	    	!pfmaptype->hasType(XWPSRef::Integer) ||
	    	pfmaptype->value.intval < (int)fmap_type_min ||
	    	pfmaptype->value.intval > (int)fmap_type_max ||
	    	op->dictFindString(this, "FDepVector", &pfdepvector) <= 0 ||
	    	!pfdepvector->isArray())
	  {
	  	return (int)(XWPSError::InvalidFont);
	  }
		pfont->FMapType = (PSFMapType) pfmaptype->value.intval;
		fdepvector.assign(pfdepvector);
  }
  
  uint tmp_fdep_size = pfont->fdep_size;
  pfont->fdep_size = fdepvector.size();
  for (i = 0; i < pfont->fdep_size; i++) 
  {
		XWPSRef fdep;
		XWPSFont *psub;

		fdepvector.arrayGet(this, i, &fdep);
		if ((code = fontParam(&fdep, &psub)) < 0)
	    return code;
	  
		if (psub->FontType == ft_composite) 
		{
	    const XWPSFontType0 *const psub0 = (const XWPSFontType0 *)psub;
	    PSFMapType fmt = psub0->FMapType;

	    if (fmt == fmap_double_escape || fmt == fmap_shift ||
					(fmt == fmap_escape && !(pfont->FMapType == fmap_escape || pfont->FMapType == fmap_double_escape)))
			{
	  		return (int)(XWPSError::InvalidFont);
			}
		}
  }
  
  switch (pfont->FMapType) 
  {
		case fmap_escape:
		case fmap_double_escape:
	    code = ensureCharEntry(op, "EscChar", &pfont->EscChar, 255);
	    break;
	    
		case fmap_shift:
	    code = ensureCharEntry(op, "ShiftIn", &pfont->ShiftIn, 15);
	    if (code >= 0)
				code = ensureCharEntry(op, "ShiftOut", &pfont->ShiftOut, 14);
	    break;
	    
		case fmap_SubsVector:	
	    {
				XWPSRef *psubsvector;
				uint svsize;

				if (op->dictFindString(this, "SubsVector", &psubsvector) <= 0 ||
		    		!psubsvector->hasType(XWPSRef::String) ||
		    		(svsize = psubsvector->size()) == 0 ||
						(pfont->subs_width = (int)(*(psubsvector->getBytes() + 1))) > 4 ||
		    		(svsize - 1) % pfont->subs_width != 0)
				{
	  			return (int)(XWPSError::InvalidFont);
				}
				if (pfont->SubsVector.bytes)
				{
					if (pfont->SubsVector.bytes->decRef() == 0)
						delete pfont->SubsVector.bytes;
					pfont->SubsVector.bytes = 0;
				}
				pfont->subs_size = (svsize - 1) / pfont->subs_width;
				pfont->SubsVector.size = svsize - 1;
				pfont->SubsVector.data = psubsvector->getBytes() + 1;
				pfont->SubsVector.bytes = psubsvector->value.bytes->arr;
				if (pfont->SubsVector.bytes)
					pfont->SubsVector.bytes->incRef();
	    } 
	    break;
	    	
		case fmap_CMap:	
	    code = ztype0GetCMap(&pfont->CMap, &fdepvector, op);
	    if (pfont->CMap)
	    	pfont->CMap->incRef();
	    break;
	    
		default:
	    ;
  }
  if (code < 0)
		return code;
	
  {
		XWPSRef *pfid;

		if (op->dictFindString(this, "FID", &pfid) <= 0)
	    save_FID.makeNull();
		else
	    save_FID.assign(pfid);
  }
  
  {
		XWPSBuildProcRefs build;

		code = buildProcNameRefs(&build, "%Type0BuildChar", "%Type0BuildGlyph");
		if (code < 0)
	    return code;
		code = buildFont(op, (XWPSFont**)&pfont, ft_composite, &build, bf_options_none);
  }
  
  if (code != 0)
		return code;
		
  pfont->procs.init_fstack_.init_fstackt0 = &XWPSFontType0::type0InitFStack;
  pfont->procs.define_font_.define_fontt0 = &XWPSFontType0::ztype0DefineFont;
  pfont->procs.make_font_.make_fontt0 = &XWPSFontType0::ztype0MakeFont;
  pfont->procs.next_char_glyph_.next_char_glyph = &XWPSTextEnum::nextCharGlyphType0;
  if (op->dictFindString(this, "PrefEnc", &pprefenc) <= 0) 
  {
		XWPSRef nul;

		if ((code = dictPutString(op, "PrefEnc", &nul)) < 0)
	    goto fail;
  }
  
  pdata = (XWPSFontData*)(pfont->client_data);
  pfont->encoding_size = pdata->Encoding.size();
  if (pfont->Encoding)
  	delete [] pfont->Encoding;
  pfont->Encoding = 0;
  if (pfont->encoding_size > 0)
    pfont->Encoding = new uint[pfont->encoding_size];
  
  for (i = 0; i < pfont->encoding_size; i++) 
  {
		XWPSRef enc;

		pdata->Encoding.arrayGet(this, i, &enc);
		if (!enc.hasType(XWPSRef::Integer)) 
		{
	    code = (int)(XWPSError::TypeCheck);
	    goto fail;
		}
		
		if ((ulong) enc.value.intval >= pfont->fdep_size) 
		{
	    code = (int)(XWPSError::RangeCheck);
	    goto fail;
		}
		pfont->Encoding[i] = (uint) enc.value.intval;
  }
  
  if (pfont->FDepVector)
  {
  	for (uint i = 0; i < tmp_fdep_size; i++)
  	{
  		if (pfont->FDepVector[i])
  		{
  			if (pfont->FDepVector[i]->decRef() == 0)
  				delete pfont->FDepVector[i];
  			pfont->FDepVector[i] = 0;
  		}
  	}
  	free(pfont->FDepVector);
  }  
  pfont->FDepVector = (XWPSFont**)malloc((pfont->fdep_size+1) * sizeof(XWPSFont*));
  for (i = 0; i < pfont->fdep_size; i++) 
  {
		XWPSRef fdep;
		XWPSRef *pfid;

		fdepvector.arrayGet(this, i, &fdep);
		fdep.dictFindString(this, "FID", &pfid);
		pfont->FDepVector[i] = (XWPSFont*)(pfid->getStruct());
		if (pfont->FDepVector[i])
			pfont->FDepVector[i]->incRef();
  }
  
  code = defineFont(pfont);
  if (code >= 0)
		return code;
	
fail:
  if (save_FID.hasType(XWPSRef::Null)) 
  {
		XWPSRef rnfid;

		nameEnterString("FID", &rnfid);
		dictUndef(op, &rnfid);
  } 
  else
		dictPutString(op, "FID", &save_FID);
  return code;
}

int XWPSContextState::zbuildFont1()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSBuildProcRefs build;
  int code = buildProcNameRefs(&build, "%Type1BuildChar", "%Type1BuildGlyph");

  if (code < 0)
		return code;
  return buildFont1Or4(op, &build, ft_encrypted, bf_notdef_required);
}

int XWPSContextState::zbuildFont2()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSCharstringFontRefs refs;
  XWPSBuildProcRefs build;
  int code = buildProcNameRefs(&build, "%Type2BuildChar", "%Type2BuildGlyph");

  if (code < 0)
		return code;
    
  code = charstringFontGetRefs(op, &refs);
  if (code < 0)
		return code;
		
	XWPSFontType1 * pdata1 = new XWPSFontType1;
	pdata1->i_ctx_p = this;
	pdata1->id = nextIDS(1);
  code = type2FontParams(op, &refs, pdata1);
  if (code < 0)
		return code;
  return buildCharstringFont(op, &build, ft_encrypted2, &refs, pdata1, bf_notdef_required);
}

int XWPSContextState::zbuildFont3()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  XWPSBuildProcRefs build;
  XWPSFontBase *pfont = new XWPSFontBase;
  pfont->i_ctx_p = this;
  pfont->id = nextIDS(1);

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = buildFontProcs(op, &build);
  if (code < 0)
		return code;
    
  code = buildSimpleFont(op, &pfont, ft_user_defined, &build, bf_options_none);
  if (code < 0)
		return code;
  return defineFont(pfont);
}

int XWPSContextState::zbuildFont4()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSBuildProcRefs build;
  int code = buildFontProcs(op, &build);

  if (code < 0)
		return code;
  return buildFont1Or4(op, &build, ft_disk_based, bf_options_none);
}

int XWPSContextState::zbuildFont9()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSBuildProcRefs build;
  int code = buildProcNameRefs(&build, NULL, "%Type9BuildGlyph");
  XWPSRef GlyphDirectory, GlyphData, DataSource;
  XWPSRef *prfda;
  XWPSFontType1 **FDArray;
  uint FDArray_size;
  int FDBytes;
  uint CIDMapOffset;
  XWPSFontBase *pfont;
  XWPSFontCid0 *pfcid;
  uint i;
  
  pfcid = new XWPSFontCid0;
  pfcid->i_ctx_p = this;
  pfcid->id = nextIDS(1);
  pfont = pfcid;
  if (code < 0 || (code = cidFontDataParam(op, pfcid, &GlyphDirectory)) < 0 ||
			(code = op->dictFindString(this, "FDArray", &prfda)) < 0 ||
			(code = op->dictIntParam(this, "FDBytes", 0, MAX_FDBytes, -1, &FDBytes)) < 0)
	{
		return code;
	}
	
  if (GlyphDirectory.hasType(XWPSRef::Null)) 
  {
		XWPSRef *pGlyphData;

		if ((code = op->dictFindString(this, "GlyphData", &pGlyphData)) < 0 ||
	    	(code = op->dictUintParam(this, "CIDMapOffset", 0, max_uint - 1, max_uint, &CIDMapOffset)) < 0)
	  {
	    return code;
	  }
	  
		GlyphData.assign(pGlyphData);
		if (GlyphData.hasType(XWPSRef::Integer)) 
		{
	    XWPSRef *pds;
	    XWPSStream *ignore_s;

	    if ((code = op->dictFindString(this, "DataSource", &pds)) < 0)
	    {
				return code;
			}
			code = pds->checkReadType(XWPSRef::File);
			if (code < 0)
				return code;
			ignore_s = pds->getStream();
			if (ignore_s->read_id != pds->size())
			{
				if (ignore_s->read_id == 0 && ignore_s->write_id == pds->size())
				{
					int fcode = fileSwitchToRead(op);
					if (fcode < 0)
						return fcode;
				}
				else
					ignore_s = &invalid_file_stream;
			}
			
	    DataSource.assign(pds);
		} 
		else 
		{
	    if (!GlyphData.hasType(XWPSRef::String) && !GlyphData.isArray())
				return GlyphData.checkTypeFailed();
	    DataSource.makeNull();
		}
  } 
  else 
  {
		GlyphData.makeNull();
		DataSource.makeNull();
		CIDMapOffset = 0;
  }
    
  if (!prfda->isArray())
		return (int)(XWPSError::InvalidFont);
	
  FDArray_size = prfda->size();
  if (FDArray_size == 0)
		return (int)(XWPSError::InvalidFont);
	
  FDArray = (XWPSFontType1**)malloc((FDArray_size+1) * sizeof(XWPSFontType1*)); 
  memset(FDArray, 0, sizeof(XWPSFontType1 *) * FDArray_size);
  for (i = 0; i < FDArray_size; ++i) 
  {
		XWPSRef rfd;

		prfda->arrayGet(this, (long)i, &rfd);
		code = fdArrayElement(&FDArray[i], &rfd);
		if (code < 0)
		{
			free(FDArray);
	    return code;
	  }
  }
  
  code = buildSimpleFont(op, &pfont, ft_CID_encrypted, &build,(PSBuildFontOptions)(bf_Encoding_optional | bf_FontBBox_required |	bf_UniqueID_ignored));
  if (code < 0)
		free(FDArray);
    
  pfont->procs.enumerate_glyph_.enumerate_glyphc0 = &XWPSFontCid0::cid0EnumerateGlyph;
  pfont->procs.glyph_outline_.glyph_outlinec0 = &XWPSFontCid0::z9glyphOutline;
  pfcid = (XWPSFontCid0*)pfont;
  pfcid->CIDMapOffset = CIDMapOffset;
  if (pfcid->FDArray)
  {
  	for (uint i = 0; i < pfcid->FDArray_size; i++)
  	{
  		if (pfcid->FDArray[i])
  		{
  			if (pfcid->FDArray[i]->decRef() == 0)
  				delete pfcid->FDArray[i];
  			pfcid->FDArray[i] = 0;
  		}
  	}
  	free(pfcid->FDArray);
  }
  pfcid->FDArray = FDArray;
  pfcid->FDArray_size = FDArray_size;
  pfcid->FDBytes = FDBytes;
  pfcid->glyph_datac0 = &XWPSFontCid0::z9glyphData;
  	
  XWPSFontData * pdata = (XWPSFontData*)(pfont->client_data);
  pdata->u.cid0.GlyphDirectory->assign(&GlyphDirectory);
  pdata->u.cid0.GlyphData->assign(&GlyphData);
  pdata->u.cid0.DataSource->assign(&DataSource);
  code = defineFont(pfont);
  return code;
}

int XWPSContextState::zbuildFont10()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSBuildProcRefs build;
  int code = buildFontProcs(op, &build);
  XWPSCidSystemInfo cidsi;
  XWPSFontBase *pfont;

  if (code < 0)
		return code;
    
  code = cidFontSystemInfoParam(&cidsi, op);
  if (code < 0)
		return code;
    
  pfont = new XWPSFontCid1;
  pfont->i_ctx_p = this;
  pfont->id = nextIDS(1);
  code = buildSimpleFont(op, &pfont, ft_CID_user_defined,		&build,
				(PSBuildFontOptions)(bf_Encoding_optional | bf_FontBBox_required |	bf_UniqueID_ignored));
  if (code < 0)
		return code;
    
  ((XWPSFontCid1 *)pfont)->CIDSystemInfo = cidsi;
  return defineFont(pfont);
}

int XWPSContextState::zbuildFont11()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSFontType42 *pfont;
  XWPSFontCid2 *pfcid = new XWPSFontCid2;
  pfcid->i_ctx_p = this;
  pfcid->id = nextIDS(1);
  pfont = pfcid;
  int MetricsCount;
  XWPSRef rcidmap, ignore_gdir;
  int code = cidFontDataParam(op, pfcid, &ignore_gdir);

  if (code < 0 ||	(code = op->dictIntParam(this, "MetricsCount", 0, 4, 0, &MetricsCount)) < 0)
  {
		return code;
	}
    
  if (MetricsCount & 1)	
	  return (int)(XWPSError::RangeCheck);
	
  code = fontStringArrayParam(op, "CIDMap", &rcidmap);
  switch (code) 
  {
    case 0:	
    
gdb:
			if (pfcid->GDBytes == 0)
	  		return (int)(XWPSError::RangeCheck);
			break;
			
    default:
			return code;
			
    case XWPSError::TypeCheck:
			switch (rcidmap.type()) 
			{
					case XWPSRef::String:
	    			goto gdb;
	    			
					case XWPSRef::Dictionary:
					case XWPSRef::Integer:	
	    			break;
	    			
					default:
	    			return code;
			}
			break;
  }
  
  code = buildTrueTypeFont(op, &pfont, ft_CID_TrueType,
				  (const char *)0, "%Type11BuildGlyph",
				  (PSBuildFontOptions)(bf_Encoding_optional |
				  bf_FontBBox_required |
				  bf_UniqueID_ignored |
				  bf_CharStrings_optional));
  if (code < 0)
		return code;
   
  pfcid = (XWPSFontCid2*)pfont;
 	pfcid->MetricsCount = MetricsCount;
 	
 	XWPSFontData *pdata = (XWPSFontData*)(pfont->client_data);
  pdata->u.type42.CIDMap->assign(&rcidmap);
  pfcid->CIDMap_proc = &XWPSFontCid2::z11CIDMapProc;
  if (MetricsCount) 
  {
		pfcid->orig_procs.get_outline = pfont->get_outline;
		pfont->get_outlinec2 = &XWPSFontCid2::z11getOutline;
		pfcid->orig_procs.get_metrics = pfont->get_metrics;
		pfont->get_metricsc2 = &XWPSFontCid2::z11getMetrics;
  }
  return defineFont(pfont);
}

int XWPSContextState::zbuildFont32()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  XWPSBuildProcRefs build;
  XWPSFontBase *pfont;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
		return code;
		
  code = buildProcNameRefs(&build, NULL, "%Type32BuildGlyph");
  if (code < 0)
		return code;
    
  pfont = new XWPSFontBase;
  pfont->i_ctx_p = this;
  pfont->id = nextIDS(1);
  code = buildSimpleFont(op, &pfont, ft_CID_bitmap, &build,	bf_Encoding_optional);
  if (code < 0)
		return code;
		
  pfont->BitmapWidths = true;
  pfont->ExactSize = fbit_transform_bitmaps;
  pfont->InBetweenSize = fbit_transform_bitmaps;
  pfont->TransformedChar = fbit_transform_bitmaps;
  pfont->procs.encode_char_.encode_charb = &XWPSFontBase::zfontNoEncodeChar;
  return defineFont(pfont);
}

int XWPSContextState::zbuildFont42()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSFontType42 *pfont = new XWPSFontType42;
  pfont->i_ctx_p = this;
  int code = buildTrueTypeFont(op, &pfont, ft_TrueType, "%Type42BuildChar","%Type42BuildGlyph", bf_options_none);

  if (code < 0)
		return code;
  return defineFont(pfont);
}

int XWPSContextState::zcacheStatus()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint status[7];
 
  if (!ifont_dir)
  	return 0;

  ifont_dir->cacheStatus(status);
  int code = push(&op, 7);
  if (code < 0)
		return code;
  makeUintArray(op - 6, status, 7);
  return 0;
}

int XWPSContextState::zcharEnumerateGlyph(XWPSRef *prdict, int *pindex, ulong *pglyph)
{
	int index = *pindex - 1;
  XWPSRef elt[2];

  if (index < 0)
		index = prdict->dictFirst();
		
next:
  index = prdict->dictNext(this, index, elt);
  *pindex = index + 1;
  if (index >= 0) 
  {
		switch (elt->type()) 
		{
	    case XWPSRef::Integer:
				*pglyph = ps_min_cid_glyph + elt[0].value.intval;
				break;
				
	    case XWPSRef::Name:
				*pglyph = nameIndex(elt);
				break;
				
	    default:
				goto next;
		}
  }
  return 0;
}

int XWPSContextState::zcshow()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * proc_op = op - 1;
  XWPSRef * str_op = op;
  XWPSTextEnum *penum = 0;
  int code;
  
  if (proc_op->isProc())
		;
  else if (op->isProc()) 
  {	
		proc_op = op;
		str_op = op - 1;
  } 
  else 
  {
		checkOp(op, 2);
		return op->checkTypeFailed();
  }
   
  if ((code = opShowSetup(str_op)) != 0 ||
			(code = pgs->cshowBegin(str_op->getBytes(), str_op->size(), &penum)) < 0)
		return code;
    
  penum->text.p_bytes = str_op->value.bytes->arr;
  if (str_op->value.bytes->arr)
  	str_op->value.bytes->arr->incRef();
  if ((code = opShowFinishSetup(penum, 2, NULL)) < 0) 
		return code;
  
  XWPSRef * esp = exec_stack.getCurrentTop();
  esp[-1].assign(proc_op);
  pop(2);
  return cshowContinue();
}

int XWPSContextState::zcurrentCacheParams()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint params[3];

  params[0] = ifont_dir->currentCacheSize();
  params[1] = ifont_dir->currentCacheLower();
  params[2] = ifont_dir->currentCacheUpper();
  int code = push(&op,4);
  if (code < 0)
		return code;
  op[-3].makeMark();
  makeUintArray(op - 2, params, 3);
  return 0;
}

int XWPSContextState::zcurrentFont()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op,1);
  if (code < 0)
		return code;
  XWPSFontData *pdata = (XWPSFontData*)(pgs->currentFont()->client_data);
  op->assign(&pdata->dict);
  return 0;
}

bool XWPSContextState::z1fontInfoHas(XWPSRef *pfidict, const char *key, XWPSString *pmember)
{
	XWPSRef *pvalue;

  if (pfidict->dictFindString(this, key, &pvalue) > 0 && pvalue->hasType(XWPSRef::String)) 
  {
		pmember->data = pvalue->getBytes();
		pmember->size = pvalue->size();
		pmember->bytes = pvalue->value.bytes->arr;
		if (pmember->bytes)
			pmember->bytes->incRef();
		return true;
  }
  return false;
}

int XWPSContextState::zfcmapGlyphName(ulong glyph, XWPSString *pstr, void *)
{
	XWPSRef nref, nsref;

  nameIndexRef((uint)glyph, &nref);    
  nameStringRef(&nref, &nsref);
  pstr->data = nsref.getBytes();
  pstr->size = nsref.size();
  pstr->bytes = nsref.value.bytes->arr;
  if (pstr->bytes)
  	pstr->bytes->incRef();
  return 0;
}

int XWPSContextState::zfontInit()
{
	if (!ifont_dir)
	{
		ifont_dir = new XWPSFontDir;
		ifont_dir->i_ctx_p = this;
    ifont_dir->ccache.mark_glyph = &XWPSContextState::zfontMarkGlyphName;
  }
  return 0;
}

bool XWPSContextState::zfontMarkGlyphName(ulong glyph, void *)
{
	return (glyph >= ps_min_cid_glyph || glyph == ps_no_glyph ? false :  nameMarkIndex((uint) glyph));
}

int XWPSContextState::zmakeFont()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  XWPSMatrix mat;

  if ((code = op->readMatrix(this, &mat)) < 0)
		return code;
  return makeFont(&mat);
}

int XWPSContextState::zregisterFont()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSFont *pfont;
  int code = fontParam(op, &pfont);

  if (code < 0)
		return code;
  pfont->is_resource = true;
  pop(1);
  return 0;
}

int XWPSContextState::zrootFont()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  XWPSFontData * pdata = (XWPSFontData*)(pgs->rootFont()->client_data);
  op->assign(&pdata->dict);
  return 0;
}

int XWPSContextState::zscaleFont()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  double scale;
  XWPSMatrix mat;

  if ((code = op->realParam(&scale)) < 0)
		return code;
    
  if ((code = mat.makeScaling(scale, scale)) < 0)
		return code;
  return makeFont(&mat);
}

int XWPSContextState::zsetCacheLimit()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkIntLEU(max_uint);
  if (code < 0)
  	return code;
  	
  ifont_dir->setCacheUpper((uint)op->value.intval);
  pop(1);
  return 0;
}

int XWPSContextState::zsetCacheParams()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint params[3];
  int i, code;
  XWPSRef * opp = op;

  for (i = 0; i < 3 && !opp->hasType(XWPSRef::Mark); i++, opp--) 
  {
		opp->checkIntLEU(max_uint);
		params[i] = opp->value.intval;
  }
  
  switch (i) 
  {
		case 3:
	    if ((code = ifont_dir->setCacheSize(params[2])) < 0)
				return code;
				
		case 2:
	    if ((code = ifont_dir->setCacheLower(params[1])) < 0)
				return code;
				
		case 1:
	    if ((code = ifont_dir->setCacheUpper(params[0])) < 0)
				return code;
				
		case 0:;
  }
  return zclearToMark();
}

int XWPSContextState::zsetFont()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSFont *pfont;
  int code = fontParam(op, &pfont);

  if (code < 0 || (code = pgs->setFont(pfont)) < 0)
		return code;
	
  pop(1);
  return code;
}

int XWPSContextState::ztype0GetCMap(XWPSCMap **ppcmap, 
	                  XWPSRef *pfdepvector,
		                XWPSRef *op)
{
	XWPSRef *prcmap;
  XWPSRef *pcodemap;
  XWPSCMap *pcmap;
  int code;
  uint num_fonts;
  uint i;

  if (op->dictFindString(this, "CMap", &prcmap) <= 0 ||
			!prcmap->hasType(XWPSRef::Dictionary) ||
			 prcmap->dictFindString(this, "CodeMap", &pcodemap) <= 0)
	{
		return (int)(XWPSError::InvalidFont);
	}
	
  pcmap = (XWPSCMap*)(pcodemap->getStruct());
  num_fonts = pfdepvector->size();
  for (i = 0; i < num_fonts; ++i) 
  {
		XWPSRef rfdep, rfsi;

		pfdepvector->arrayGet(this, (long)i, &rfdep);
		code = acquireCidSystemInfo(&rfsi, &rfdep);
		if (code < 0)
	    return code;
		if (code == 0) 
		{
	    if (rfsi.size() != 1)
				return (int)(XWPSError::RangeCheck);
		}
  }
  *ppcmap = pcmap;
  return 0;
}
		                
int XWPSContextState::ztype9MapCid()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSFont *pfont;
  XWPSFontCid0 *pfcid;
  int code = fontParam(op - 1, &pfont);
  XWPSString gstr;
  int fidx;

  if (code < 0)
		return code;
    
  if (pfont->FontType != ft_CID_encrypted)
		return (int)(XWPSError::InvalidFont);
	
  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  pfcid = (XWPSFontCid0 *)pfont;
  code = pfcid->glyphData((ulong)(ps_min_cid_glyph + op->value.intval), &gstr, &fidx);
  if (code < 0)
		return code;
    
  if (gstr.bytes)
  {
  	gstr.bytes->incRef();
  	op[-1].clear();
  	op[-1].value.bytes = new XWPSBytesRef;
  	op[-1].value.bytes->arr = gstr.bytes;
  	op[-1].value.bytes->ptr = gstr.bytes->arr;
  	op[-1].setTypeAttrs((ushort)(XWPSRef::String), PS_A_READONLY); 
	  op[-1].setSize(gstr.size);
  }
  else
    op[-1].makeString(PS_A_READONLY, gstr.size, gstr.data);
  op->makeInt(fidx);
  return 0;
}

int XWPSContextState::ztype11MapCid()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSFont *pfont;
  int code = fontParam(op - 1, &pfont);

  if (code < 0)
		return code;
    
  if (pfont->FontType != ft_CID_TrueType)
		return (int)(XWPSError::InvalidFont);
	
  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  code = ((XWPSFontCid2 *)pfont)->z11CIDMapProc((ps_min_cid_glyph + op->value.intval));
  if (code < 0)
		return code;
    
  op[-1].makeInt(code);
  pop(1);
  return 0;
}

#if defined(DEBUG) || defined(PROFILE)

int XWPSContextState::zwriteFont9()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSFont *pfont;
  XWPSFontCid0 *pfcid;
  int code = fontParam(op, &pfont);
  XWPSStream *s;

  if (code < 0)
		return code;
    
  if (pfont->FontType != ft_CID_encrypted)
		return (int)(XWPSError::InvalidFont);
    
  code = op[-1].checkWriteType(XWPSRef::File);
  if (code < 0)
		return code;
		
	s = op[-1].getStream();
	if ( s->write_id != op[-1].size() )
	{
		int fcode = fileSwitchToWrite(op - 1);
		if (fcode < 0)
			return fcode;
	}
	
  pfcid = (XWPSFontCid0 *)pfont;
  code = ppfcid->writeCid0Font(s, WRITE_TYPE2_NO_LENIV | WRITE_TYPE2_CHARSTRINGS,  NULL, 0, NULL);
  if (code >= 0)
		pop(2);
  return code;
}
#endif