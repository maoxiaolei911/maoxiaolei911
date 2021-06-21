/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "XWApplication.h"
#include "XWPSError.h"
#include "psscantable.h"
#include "XWPSContextState.h"

#define MAX_FLOAT\
     ((0x1000000 - 1.0) * 0x1000000 * 0x1000000 * 0x10000000 * 0x10000000)

typedef enum {
    BT_SEQ = 128,	
    BT_SEQ_IEEE_MSB = 128,
    BT_SEQ_IEEE_LSB = 129,
    BT_SEQ_NATIVE_MSB = 130,
    BT_SEQ_NATIVE_LSB = 131,
    BT_INT32_MSB = 132,
    BT_INT32_LSB = 133,
    BT_INT16_MSB = 134,
    BT_INT16_LSB = 135,
    BT_INT8 = 136,
    BT_FIXED = 137,
    BT_FLOAT_IEEE_MSB = 138,
    BT_FLOAT_IEEE_LSB = 139,
    BT_FLOAT_NATIVE = 140,
    BT_BOOLEAN = 141,
    BT_STRING_256 = 142,
    BT_STRING_64K_MSB = 143,
    BT_STRING_64K_LSB = 144,
    BT_LITNAME_SYSTEM = 145,
    BT_EXECNAME_SYSTEM = 146,
    BT_LITNAME_USER = 147,
    BT_EXECNAME_USER = 148,
    BT_NUM_ARRAY = 149
} bin_token_type_t;


#define MIN_BIN_TOKEN_TYPE 128
#define MAX_BIN_TOKEN_TYPE 159
#define NUM_BIN_TOKEN_TYPES (MAX_BIN_TOKEN_TYPE - MIN_BIN_TOKEN_TYPE + 1)

const uchar bin_token_bytes[NUM_BIN_TOKEN_TYPES] =
{
    4, 4, 4, 4, 5, 5, 3, 3, 2, 2, 5, 5, 5,
    2, 2, 3, 3, 2, 2, 2, 2, 4,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

const uchar bin_token_num_formats[NUM_BIN_TOKEN_TYPES] =
{
    num_msb + num_float_IEEE,	/* BT_SEQ_IEEE_MSB */
    num_lsb + num_float_IEEE,	/* BT_SEQ_IEEE_LSB */
    num_msb + num_float_native,	/* BT_SEQ_NATIVE_MSB */
    num_lsb + num_float_native,	/* BT_SEQ_NATIVE_LSB */
    num_msb + num_int32,	/* BT_INT32_MSB */
    num_lsb + num_int32,	/* BT_INT32_LSB */
    num_msb + num_int16,	/* BT_INT16_MSB */
    num_lsb + num_int16,	/* BT_INT16_LSB */
    0,				/* BT_INT8, not used */
    0,				/* BT_FIXED, not used */
    num_msb + num_float_IEEE,	/* BT_FLOAT_IEEE_MSB */
    num_lsb + num_float_IEEE,	/* BT_FLOAT_IEEE_LSB */
    num_float_native,		/* BT_FLOAT_NATIVE */
    0,				/* BT_BOOLEAN, not used */
    0,				/* BT_STRING_256, not used */
    num_msb,			/* BT_STRING_64K_MSB */
    num_lsb			/* BT_STRING_64K_LSB */
    /* rest not used */
};

static const double binary_scale[32] = {
#define EXPN2(n) (0.5 / (1L << (n-1)))
    1.0, EXPN2(1), EXPN2(2), EXPN2(3),
    EXPN2(4), EXPN2(5), EXPN2(6), EXPN2(7),
    EXPN2(8), EXPN2(9), EXPN2(10), EXPN2(11),
    EXPN2(12), EXPN2(13), EXPN2(14), EXPN2(15),
    EXPN2(16), EXPN2(17), EXPN2(18), EXPN2(19),
    EXPN2(20), EXPN2(21), EXPN2(22), EXPN2(23),
    EXPN2(24), EXPN2(25), EXPN2(26), EXPN2(27),
    EXPN2(28), EXPN2(29), EXPN2(30), EXPN2(31)
#undef EXPN2
};

int (*scan_dsc_proc)(uchar *, uint) = NULL;
int (*scan_comment_proc)(uchar *, uint) = NULL;

XWPSOpArrayTable::XWPSOpArrayTable()
{
	nx_table = 0;
	count = 0;
	base_index = 0;
	attrs = 0;
	root_p = &table;
}

XWPSOpArrayTable::~XWPSOpArrayTable()
{
	if (nx_table)
		delete [] nx_table;
}

void XWPSOpArrayTable::init(XWPSContextState * ctx, uint size, uint space, uint bidx)
{
	if (nx_table)
		delete [] nx_table;
		
	uint save_space = ctx->idmemory()->iallocSpace();
	ctx->idmemory()->setSpace(space);
	table.makeArray(PS_A_READONLY, size);	
	ctx->idmemory()->setSpace(save_space);
	nx_table = 0;
	if (size > 0)
	  nx_table = new ushort[size];
	base_index = bidx;
	attrs = space | PS_A_EXECUTABLE;
}

XWPSDynamicArea::XWPSDynamicArea()
{
	is_dynamic = false;
	base = buf;
	next = buf;
	limit = buf + da_buf_size;
	dynamic_buf = 0;
}

XWPSDynamicArea::~XWPSDynamicArea()
{
	release();
}

void XWPSDynamicArea::copy(const XWPSDynamicArea & other)
{
	is_dynamic = other.is_dynamic;
	if (is_dynamic)
	{
		base = other.base;
		next = other.next;
		limit = other.limit;
	}
	else
	{
		base = buf;
		next = base + (other.next - other.base);
		limit = buf + da_buf_size;
		memcpy(buf, other.buf, da_buf_size);
	}
	if (dynamic_buf != other.dynamic_buf)
	{
		if (dynamic_buf)
		{
			if (dynamic_buf->decRef() == 0)
				delete dynamic_buf;
		}
				
		dynamic_buf = other.dynamic_buf;
		if (dynamic_buf)
			dynamic_buf->incRef();
	}
}

int XWPSDynamicArea::grow(uchar * nextA, uint max_size)
{
	uint old_size = limit - base;
  uint new_size = (old_size < 10 ? 20 :  old_size >= (max_size >> 1) ? max_size : old_size << 1);
  	
  next = nextA;
  if (old_size == max_size)
		return (int)(XWPSError::LimitCheck);
    
  int code = 0;
  while ((code = resize(new_size)) < 0 && new_size > old_size) 
  {	
		new_size -= (new_size - old_size + 1) >> 1;
  }
  return code;
}

void XWPSDynamicArea::init()
{
	is_dynamic = false;
	limit = buf + da_buf_size;
  next = base = buf;
}

void XWPSDynamicArea::release()
{
	if (dynamic_buf)
	  if (dynamic_buf->decRef() == 0)
	  	delete dynamic_buf;
	base = 0;
	is_dynamic = false;
	dynamic_buf = 0;
}

int XWPSDynamicArea::resize(uint new_size)
{
	uint pos = next - base;
	uint old_size = limit - base;
	if (!dynamic_buf)
	{
		dynamic_buf = new XWPSBytes;
		dynamic_buf->self = true;
	}
	if (is_dynamic)
	{
		base = (uchar*)realloc((void*)base, (new_size+1) * sizeof(uchar));
		if (base == NULL)
			return (int)(XWPSError::VMError);
	}
	else
	{
		uchar * b = (uchar*)malloc((new_size+1) * sizeof(uchar));
		if (b == NULL)
			return (int)(XWPSError::VMError);
		memcpy(b, base, qMin(old_size, new_size));
		base = b;
		is_dynamic = true;
	}	  
	dynamic_buf->arr = base;
	dynamic_buf->length = new_size;
	next = base + pos;
	limit = base + new_size;
	return 0;
}

void XWPSDynamicArea::save()
{
	if (!is_dynamic && base != buf) 
	{
		memcpy((void*)buf, (void*)base, (limit - base) * sizeof(uchar));
		next = buf + (uint)(limit - base);
		base = buf;
  }
}

XWPSScannerState::XWPSScannerState()
	:XWPSStruct()
{
	s_scan_type = ScanningNone;
	s_pstack = 0;
	s_pdepth = 0;
	s_options = 0;
	s_ss.binary.num_format = 0;
	s_ss.binary.cont = 0;
	s_ss.binary.index = 0;
	s_ss.binary.max_array_index = 0;
	s_ss.binary.min_string_index = 0;
	s_ss.binary.top_size = 0;
	s_ss.binary.size = 0;
	s_ss.s_name.s_name_type = 0;
	s_ss.s_name.s_try_number = false;
}

XWPSScannerState::~XWPSScannerState()
{
}

int XWPSScannerState::getLength()
{
	return sizeof(XWPSScannerState);
}

const char * XWPSScannerState::getTypeName()
{
	return "scannerstate";
}

void XWPSScannerState::init(int options)
{
	s_scan_type = ScanningNone;
	s_pstack = 0;
	s_options = options;
}

void XWPSScannerState::init(bool from_string)
{
	initCheck(from_string, false);
}

void XWPSScannerState::initCheck(bool from_string, bool check_only)
{
	init((from_string ? SCAN_FROM_STRING : 0) | (check_only ? SCAN_CHECK_ONLY : 0));
}

XWPSScannerState & XWPSScannerState::operator=(XWPSScannerState & other)
{
	s_pstack = other.s_pstack;
	s_pdepth = other.s_pdepth;
	s_options = other.s_options;
	s_scan_type = other.s_scan_type;
	s_da.copy(other.s_da);
	bin_array.assign(&other.bin_array);
	s_ss = other.s_ss;
	return *this;
}

int XWPSContextState::scanBinaryToken(XWPSStream *s, 
	                                    XWPSRef *pref,
		  								                XWPSScannerState *pstate)
{
	PSScanBinaryState * pbs = &pstate->s_ss.binary;
	XWPSStreamCursor * cursor = s->getCursor();
  uchar * p = cursor->r.ptr;
  uchar * rlimit = cursor->r.limit;  
	int code;
  uint arg;   
  uint wanted = bin_token_bytes[*p - MIN_BIN_TOKEN_TYPE] - 1;
  uint rcnt = rlimit - p;
  
  if (rcnt < wanted) 
  {
  	cursor->r.ptr = p - 1;
		pstate->s_scan_type = ScanningNone;
		return scan_Refill;
  }
  
  int num_format = bin_token_num_formats[*p - MIN_BIN_TOKEN_TYPE];
  switch (*p)
  {
  	case BT_SEQ_IEEE_MSB:
		case BT_SEQ_IEEE_LSB:
		case BT_SEQ_NATIVE_MSB:
		case BT_SEQ_NATIVE_LSB:
			{
				uint top_size = p[1];
				uint hsize, size;
				pbs->num_format = num_format;
				if (top_size == 0)
				{
					if (rcnt < 7) 
					{
						cursor->r.ptr = p - 1;
						pstate->s_scan_type = ScanningNone;
						return scan_Refill;
		    	}
		    	
		    	if (p[1] != 0)
		    		return (int)(XWPSError::SyntaxError);
		    			
		    	top_size = sdecodeushort(p + 2, num_format);
		    	ulong lsize = sdecodelong(p + 4, num_format);
		    	if ((size = lsize) != lsize)
						return (int)(XWPSError::LimitCheck);
		    	hsize = 8;
				}
				else
				{
					size = sdecodeushort(p + 2, num_format);
		    	hsize = 4;
				}
				
				if (size < hsize)
		    	return (int)(XWPSError::SyntaxError);
		    		
		    pstate->bin_array.makeArray(currentSpace() | (PS_A_ALL + PS_A_EXECUTABLE), size / sizeof(XWPSRef));
		    p += hsize - 1;
				size -= hsize;
				cursor->r.ptr = p;
				pbs->max_array_index = pbs->top_size = top_size;
				pbs->min_string_index = pbs->size = size;
				pbs->index = 0;
				pstate->s_da.is_dynamic = false;
				pstate->s_da.base = pstate->s_da.next = pstate->s_da.limit = pstate->s_da.buf;
				code = scanBosContinue(s, pref, pstate);
				if (code == scan_Refill || code < 0)
				{
					uint index = pbs->index;
					XWPSRef * to = pstate->bin_array.getArray() + index;
					uint c = pstate->bin_array.size()  - index;
					for (; c--; ++to)
						to->makeNull();
				}
				return code;
			}
			
		case BT_INT8:
	    pref->makeInt((p[1] ^ 128) - 128);
	    cursor->r.ptr = p + 1;
	    return 0;
	    
	  case BT_FIXED:
	    num_format = p[1];
	    if (!num_is_valid(num_format))
				return (int)(XWPSError::SyntaxError);
	    wanted = 1 + encoded_number_bytes(num_format);
	    if (rcnt < wanted) 
	    {
	    	cursor->r.ptr = p - 1;
				pstate->s_scan_type = ScanningNone;
				return scan_Refill;
	    }
	    code = sdecode_number(p + 2, num_format, pref);
	    goto rnum;
	    
	  case BT_INT32_MSB:
		case BT_INT32_LSB:
		case BT_INT16_MSB:
		case BT_INT16_LSB:
		case BT_FLOAT_IEEE_MSB:
		case BT_FLOAT_IEEE_LSB:
		case BT_FLOAT_NATIVE:
	    code = sdecode_number(p + 1, num_format, pref);
	    
rnum:
			switch (code)
			{
				case XWPSRef::Integer:
				case XWPSRef::Real:
		    	pref->setType(code);
		    	break;
		    	
				case XWPSRef::Null:
		    	return (int)(XWPSError::SyntaxError);
		    	
				default:
		    	return code;
			}
			cursor->r.ptr = p + wanted;
	    return 0;
	    
	  case BT_BOOLEAN:
	    arg = p[1];
	    if (arg & ~1)
				return (int)(XWPSError::SyntaxError);
	    pref->makeBool(arg);
	    cursor->r.ptr = p + 1;
	    return 0;
	    
		case BT_STRING_256:
	    arg = *++p;
	    goto str;
	    
	  case BT_STRING_64K_MSB:
		case BT_STRING_64K_LSB:
	    arg = sdecodeushort(p + 1, num_format);
	    p += 2;
	    
str:
	    if (s->foreign && rlimit - p >= arg)
	    {
	    	if (s->cbuf_bytes)
					s->cbuf_bytes->incRef();
	    	cursor->r.ptr = p;
				pref->makeString(PS_A_ALL | PS_AVM_FOREIGN, arg, (uchar *)(s->bufPtr()));
				pref->value.bytes->arr = s->cbuf_bytes;
				s->bufSkip(arg);
				return 0;
	    }
	    else
	    {
	    	uchar * str = 0;
	    	pstate->s_da.dynamic_buf = new XWPSBytes(arg);
	    	pstate->s_da.dynamic_buf->length = arg;
	    	pstate->s_da.dynamic_buf->self = true;
	    	str = pstate->s_da.dynamic_buf->arr;
				if (str == 0)
		    	return (int)(XWPSError::VMError);
				cursor->r.ptr = p;
				pstate->s_da.base = pstate->s_da.next = str;
				pstate->s_da.limit = str + arg;
				code = scanBinStringContinue(s, pref, pstate);
				if (code == scan_Refill || code < 0) 
				{
		    	pstate->s_da.is_dynamic = true;
		    	pstate->bin_array.makeNull();		/* clean up for GC */
		    	pbs->cont = &XWPSContextState::scanBinStringContinue;
				}
				return code;
	    }
	    
	  case BT_LITNAME_SYSTEM:
	    code = scanBinGetName(system_names_p, p[1], pref);
	    goto lname;
	    
		case BT_EXECNAME_SYSTEM:
	    code = scanBinGetName(system_names_p, p[1], pref);
	    goto xname;
	    
		case BT_LITNAME_USER:
	    code = scanBinGetName(user_names_p, p[1], pref);
	    
lname:
			if (code < 0)
				return code;
	    if (!pref->hasType(XWPSRef::Name))
				return (int)(XWPSError::Undefined);
	    cursor->r.ptr = p + 1;
	    return 0;
	    
	  case BT_EXECNAME_USER:
	    code = scanBinGetName(user_names_p, p[1], pref);
	    
xname:
	    if (code < 0)
				return code;
				
	    if (!pref->hasType(XWPSRef::Name))
				return (int)(XWPSError::Undefined);
	    pref->setAttrs(PS_A_EXECUTABLE);
	    cursor->r.ptr = p + 1;
	    return 0;
	    
	  case BT_NUM_ARRAY:
	    num_format = p[1];
	    if (!num_is_valid(num_format))
				return (int)(XWPSError::SyntaxError);
	    arg = sdecodeushort(p + 2, num_format);
	    pstate->bin_array.makeArray(currentSpace() | PS_A_ALL, arg);
	    pbs->num_format = num_format;
	    pbs->index = 0;
	    p += 3;
	    cursor->r.ptr = p;
	    code = scanBinNumArrayContinue(s, pref, pstate);
	    if (code == scan_Refill || code < 0) 
	    {
	    	XWPSRef * to = pstate->bin_array.getArray() + pbs->index;
	    	uint c = arg - pbs->index;
	    	for (; c--; ++to)
						to->makeNull();
				pbs->cont = &XWPSContextState::scanBinNumArrayContinue;
			}
	    return code;
  }
  
  return (int)(XWPSError::SyntaxError);
}

int XWPSContextState::scanBinGetName(XWPSRef *pnames, int index, XWPSRef *pref)
{
	if (pnames == 0)
		return (int)(XWPSError::RangeCheck);
			
  return pnames->arrayGet(this, (long)index, pref);
}

int XWPSContextState::scanBinNumArrayContinue(XWPSStream * s, 
	                                            XWPSRef * pref,
			                                        XWPSScannerState * pstate)
{
	PSScanBinaryState *const pbs = &pstate->s_ss.binary;
  uint index = pbs->index;
  XWPSRef *np = pstate->bin_array.getArray() + index;
  uint wanted = encoded_number_bytes(pbs->num_format);
  for (; index < pstate->bin_array.size(); index++, np++) 
  {
		if (s->bufAvailable() < wanted) 
		{
	    pbs->index = index;
	    pstate->s_scan_type = ScanningBinary;
	    return scan_Refill;
		}
		
		int code = sdecode_number(s->bufPtr(), pbs->num_format, np);
		switch (code) 
		{
	    case XWPSRef::Integer:
	    case XWPSRef::Real:
				np->setType(code);
				s->bufSkip(wanted);
				break;
				
	    case XWPSRef::Null:
				return (int)(XWPSError::SyntaxError);
				
	    default:
				return code;
		}
  }
  pref->assign(&(pstate->bin_array));
  return 0;
}

int XWPSContextState::scanBinStringContinue(XWPSStream * s, 
	                                          XWPSRef * pref,
			 											                XWPSScannerState * pstate)
{
	uchar *q = pstate->s_da.next;
  uint wanted = pstate->s_da.limit - q;
  uint rcnt;

  s->gets(q, wanted, &rcnt);
  if (rcnt == wanted) 
  {
  	if (pstate->s_da.dynamic_buf)
  		pstate->s_da.dynamic_buf->incRef();
  	pref->makeString(idmemory()->iallocSpace() | PS_A_ALL);
  	pref->value.bytes->arr = pstate->s_da.dynamic_buf;
  	pref->value.bytes->ptr = pstate->s_da.base;
  	pref->setSize(pstate->s_da.limit - pstate->s_da.base);
		return 0;
  }
  pstate->s_da.next = q + rcnt;
  pstate->s_scan_type = ScanningBinary;
  return scan_Refill;
}

int XWPSContextState::scanBosContinue(XWPSStream * s, 
	                                    XWPSRef * pref,
		                                  XWPSScannerState * pstate)
{
	PSScanBinaryState * pbs = &pstate->s_ss.binary;
	XWPSStreamCursor * cursor = s->getCursor();
	uchar * p = cursor->r.ptr;
	uchar * rlimit = cursor->r.limit;
  uint max_array_index = pbs->max_array_index;
  uint min_string_index = pbs->min_string_index;
  int num_format = pbs->num_format;
  uint index = pbs->index;
  uint size = pbs->size;
  XWPSRef *abase = pstate->bin_array.getArray();
  int code;
  
  pbs->cont = &XWPSContextState::scanBosContinue;
  for (; index < max_array_index; p += SIZEOF_BIN_SEQ_OBJ, index++)
  {
  	XWPSRef *op = abase + index;
  	cursor->r.ptr = p;
  	if (rlimit - p < SIZEOF_BIN_SEQ_OBJ) 
  	{
	    pbs->index = index;
	    pbs->max_array_index = max_array_index;
	    pbs->min_string_index = min_string_index;
	    pstate->s_scan_type = ScanningBinary;
	    return scan_Refill;
		}
		if (p[2] != 0) 
	    return (int)(XWPSError::SyntaxError);
	    	
	  uint attrs = (p[1] & 128 ? PS_A_EXECUTABLE : 0);
	  uint osize;
		long value;
		uint atype;
		switch (p[1] & 0x7f)
		{
			case BS_TYPE_NULL:
				op->makeNull();
				break;
				
	    case BS_TYPE_INTEGER:
				op->makeInt(sdecodelong(p + 5, num_format));
				break;
	    case BS_TYPE_REAL:
	    	{
	    		osize = sdecodeushort(p + 3, num_format);
	    		float vreal;
		    	if (osize != 0) 
		    	{
						value = sdecodelong(p + 5, num_format);
						vreal = (float)ldexp((double)value, -osize);
		    	} 
		    	else 
		    	{
						vreal = sdecodefloat(p + 5, num_format);
		    	}
		    	op->makeReal(vreal);
		    	break;
	    	}
	    	
	   	case BS_TYPE_BOOLEAN:
				op->makeBool(sdecodelong(p + 5, num_format) != 0);
				break;
				
	    case BS_TYPE_STRING:
	    	osize = sdecodeushort(p + 3, num_format);
				attrs |= PS_A_ALL;
	      
str:
				if (osize == 0) 
				{
		    	op->makeString(attrs);
		    	break;
				}
				value = sdecodelong(p + 5, num_format);
				if (value < max_array_index * SIZEOF_BIN_SEQ_OBJ || value + osize > size)
		    	return (int)(XWPSError::SyntaxError);
		    if (value < min_string_index)
		    {
		    	uint str_size = size - value;
		    	uchar *sbase = 0;

		    	if (pstate->s_da.is_dynamic)
						sbase = scanBosResize(pstate, str_size,	index);
		    	else
		    	{
		    		pstate->s_da.dynamic_buf = new XWPSBytes;
          	pstate->s_da.dynamic_buf->self = true;
						sbase = (uchar*)malloc((str_size + 1) * sizeof(uchar));
					}
						
		    	if (sbase == 0)
						return (int)(XWPSError::VMError);
					
					pstate->s_da.dynamic_buf->arr = sbase;
					pstate->s_da.dynamic_buf->length = str_size;
		    	pstate->s_da.is_dynamic = true;
		    	pstate->s_da.base = pstate->s_da.next = sbase;
		    	pstate->s_da.limit = sbase + str_size;
		    	min_string_index = value;
		    }
		    if (pstate->s_da.dynamic_buf)
		    	pstate->s_da.dynamic_buf->incRef();
		    op->makeString(attrs | idmemory()->iallocSpace());
		    op->setSize(osize);
		    op->value.bytes->arr = pstate->s_da.dynamic_buf;
		    op->setBytesPtr(pstate->s_da.base + (value - min_string_index));
				break;
				
			case BS_TYPE_EVAL_NAME:
				attrs |= PS_A_READONLY;
			case BS_TYPE_NAME:
				osize = sdecodeushort(p + 3, num_format);
				value = sdecodelong(p + 5, num_format);
				switch (osize) 
				{
		    	case 0:
						code = user_names_p->arrayGet(this, value, op);
						goto usn;
						
		    	case 0xffff:
						code = system_names_p->arrayGet(this, value, op);

usn:
						if (code < 0)
			    		return code;
						if (!op->hasType(XWPSRef::Name))
			    		return (int)(XWPSError::Undefined);
						op->setAttrs(attrs);
						break;
						
		    	default:
						goto str;
				}
				break;
				
			case BS_TYPE_ARRAY:
				osize = sdecodeushort(p + 3, num_format);
				atype = XWPSRef::Array;
	      
arr:
				value = sdecodeushort(p + 5, num_format);
				if (value + osize > min_string_index || value & (SIZEOF_BIN_SEQ_OBJ - 1))
		    	return (int)(XWPSError::SyntaxError);
				{
		    	uint aindex = value / SIZEOF_BIN_SEQ_OBJ;

		    	max_array_index =	qMax(max_array_index, aindex + osize);
		    	op->clear();
		    	op->setTypeAttrs(atype, attrs | PS_A_ALL | currentSpace());
		    	op->setSize(osize); 
		    	switch (atype)
		    	{
		    		case XWPSRef::Array:	
							op->value.refs = new XWPSArrayRef(abase + aindex);
							op->value.refs->arr = pstate->bin_array.value.refs->arr;
							if (op->value.refs->arr)
								op->value.refs->arr->incRef();
		    			break;
		    			
		    		case XWPSRef::MixedArray:
		    		case XWPSRef::ShortArray:
		    			op->value.packed = new XWPSPackedArrayRef((ushort*)(abase + aindex));
		    			break;
		    			
		    		default:
		    			break;
		    	}
				}
				break;
				
			case BS_TYPE_DICTIONARY:
				osize = sdecodeushort(p + 3, num_format);
				if ((osize & 1) != 0 && osize != 1)
		    	return (int)(XWPSError::SyntaxError);
				atype = XWPSRef::MixedArray;
				goto arr;
				
	    case BS_TYPE_MARK:
				op->makeMark();
				break;
				
	    default:
				return (int)(XWPSError::SyntaxError);
		}
  }
  
  cursor->r.ptr = p;
  XWPSRef to;
  to.tas.type_attrs = pstate->bin_array.tas.type_attrs;
	to.tas.rsize = pstate->bin_array.tas.rsize;
	to.value.refs = new XWPSArrayRef(max_array_index);
	to.setSize(max_array_index);
  uint count = qMin(max_array_index, (uint)(pstate->bin_array.size()));
  for (uint i = 0; i < count; i++)
  {
  	XWPSRef * tt = to.getArray() + i;
  	XWPSRef * pp = pstate->bin_array.getArray() + i;
  	tt->assign(pp);
  }
  pstate->bin_array.assign(&to);
  code = scanBosStringContinue(s, pref, pstate);
  if (code == scan_Refill)
		pbs->cont = &XWPSContextState::scanBosStringContinue;
  return code;
}

uchar * XWPSContextState::scanBosResize(XWPSScannerState * pstate, 
	                                      uint new_size,
												                uint index)
{
  uint old_size = pstate->s_da.limit-pstate->s_da.base;
  uchar *old_base = pstate->s_da.base;
  uchar *new_base = (uchar *)realloc(old_base, new_size+1);
  uchar *relocated_base = new_base + (new_size - old_size);
  XWPSRef *aptr = pstate->bin_array.getArray();

  if (new_base == 0)
		return 0;
		
  if (relocated_base != old_base)
		for (uint i = index; i != 0; i--, aptr++)
	    if (aptr->hasType(XWPSRef::String) && aptr->size() != 0)
				aptr->setBytesPtr(aptr->getBytes() - old_base + relocated_base);
  return new_base;
}

int XWPSContextState::scanBosStringContinue(XWPSStream * s, 
	                                          XWPSRef * pref,
			 										                  XWPSScannerState * pstate)
{
	PSScanBinaryState * pbs = &pstate->s_ss.binary;
  XWPSRef rstr;
  XWPSRef *op = pstate->bin_array.getArray();
  int code = scanBinStringContinue(s, &rstr, pstate);
  uint space = idmemory()->iallocSpace();
  bool rescan = false;

  if (code != 0)
		return code;
		
  for (uint i = pstate->bin_array.size(); i != 0; i--, op++)
		switch (op->type()) 
		{
	    case XWPSRef::String:
				if (op->hasAttr(PS_A_WRITE))
		    	break;
				{
		    	uint attrs = (op->hasAttr(PS_A_EXECUTABLE) ? PS_A_EXECUTABLE : 0);
		    	code = nameRef((const uchar*)op->getBytes(), op->size(), op, 1);
		    	if (code < 0)
						return code;
		    	op->setAttrs(attrs);
				}
				
	    case XWPSRef::Name:
				if (op->hasAttr(PS_A_READ)) 
				{
		    	XWPSRef *defp = dictFindName(op);

		    	if (defp == 0)
						return (int)(XWPSError::Undefined);
					code = defp->storeCheckSpace(space);
					if (code < 0)
						return code;
		    	op->assign(defp);
				}
				break;
				
	    case XWPSRef::MixedArray:
				{
		    	uint count = op->size();
		    	XWPSRef rdict;
		    	if (count == 1) 
		    	{
						if (op->getArray() < op)
			    		rdict.assign(op->getArray());
						else 
						{
			    		rescan = true;
			    		continue;
						}
		    	} 
		    	else 
		    	{
						code = rdict.dictAlloc(iimemory()->space, count >> 1);
						if (code < 0)
							return code;
						while (count) 
						{
			    		count -= 2;
			    		XWPSRef * p = op->getArray();
			    		code = dictPut(&rdict,  &p[count], &p[count + 1]);
			    		if (code < 0)
								return code;
						}
		    	}
		    	rdict.setAttrs(PS_A_ALL);
		    	rdict.copyAttrs(PS_A_EXECUTABLE, op);
		    	op->assign(&rdict);
				}
				break;
			}
			
  if (rescan)
  {
  	XWPSRef * op = pstate->bin_array.getArray();
  	uint i = pstate->bin_array.size();
		for (; i != 0; i--, op++)
		{
	  	if (op->hasType(XWPSRef::MixedArray)) 
	  	{
				XWPSRef *piref = op->getArray();
				XWPSRef rdict;
				if (piref->hasType(XWPSRef::MixedArray))
		    	return (int)(XWPSError::SyntaxError);
		    		
				rdict.assign(piref);
				rdict.copyAttrs(PS_A_EXECUTABLE, op);
				op->assign(&rdict);
	  	}
	  }
	}
  pref->assign(&pstate->bin_array);
  pref->setSize(pbs->top_size);
  return scan_BOS;
}

int XWPSContextState::scanComment(XWPSRef *pref, 
	                                XWPSScannerState *pstate,
	                                uchar * base, 
	                                uchar * end, 
	                                bool )
{
	uint len = (uint) (end - base);
  int code = 0;
  if (len > 1 && (base[1] == '%' || base[1] == '!'))
  {
  	if (scan_dsc_proc != NULL) 
  	{
	    code = scan_dsc_proc(base, len);
	    return (code < 0 ? code : 0);
		}
		if (pstate->s_options & SCAN_PROCESS_DSC_COMMENTS) 
		{
	    code = scan_DSC_Comment;
	    goto comment;
		}
  }
  
  if (scan_comment_proc != NULL) 
  {
		code = scan_comment_proc(base, len);
		return (code < 0 ? code : 0);
  }
  
  if (pstate->s_options & SCAN_PROCESS_COMMENTS) 
  {
		code = scan_Comment;
		goto comment;
  }
  return 0;

comment:
  {
		pref->makeString(currentSpace() | PS_A_ALL, len, base);
  }
  return code;
}

int XWPSContextState::scanHandleRefill(XWPSRef * fop, 
	                                     XWPSScannerState * sstate,
		                                   bool save, 
		                                   bool push_file, 
		                                   op_proc_t cont)
{
	XWPSStream *s = fop->getStream();
  uint avail = s->bufAvailable();
  
  if (s->getEndStatus() == EOFC) 
		return (int)(XWPSError::SyntaxError);
			
	int status = s->processReadBuf();
  if (s->bufAvailable() > avail)
		return 0;
		
  if (status == 0)
		status = s->getEndStatus();
		
	switch (status)
	{
		case EOFC:
	    return 0;
	    
		case ERRC:
	    return (int)(XWPSError::IOError);
	    	
		case INTC:
		case CALLC:
	    {
				XWPSRef rstate[2];				
				XWPSScannerState *pstate;
				int nstate = (push_file ? 2 : 1);
				if (save)
				{
					pstate = new XWPSScannerState;
					*pstate = *sstate;
				}
				else
				{
					pstate = sstate;
					pstate->incRef();
				}
				
				rstate[0].makeStruct(currentSpace(), pstate);
				rstate[1].assign(fop);
				rstate[1].clearAttrs(PS_A_EXECUTABLE);
				return handleReadException(status, fop, rstate, nstate, cont);
	    }
	    break;
	}
	
	xwApp->error("Can't refill scanner input buffer!\n");
  return (int)(XWPSError::Fatal);
}

int XWPSContextState::scanNumber(uchar * str, 
	                               uchar * end, 
	                               int sign,
	                               XWPSRef * pref, 
	                               uchar ** psp)
{
	uchar *sp = str;
	
#define NUM_POWERS_10 6
  static const float powers_10[NUM_POWERS_10 + 1] = {
			1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6
  };
  static const double neg_powers_10[NUM_POWERS_10 + 1] = {
			1e0, 1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6
  };
  
#define IS_DIGIT(d, c)\
  ((d = decoder[c]) < 10)
#define WOULD_OVERFLOW(val, d, maxv)\
  (val >= maxv / 10 && (val > maxv / 10 || d > (int)(maxv % 10)))
  
  int ival;
  long lval;
  double dval;
  int exp10;
  int code = 0;
  int c, d;
  const uchar *const decoder = scan_char_decoder;
  
  if (sp >= end)
  	return (int)(XWPSError::SyntaxError);
  else
  	c = *sp++;
  	
  if (!IS_DIGIT(d, c)) 
  {
		if (c != '.')
			return (int)(XWPSError::SyntaxError);
	    	
		if (sp >= end)
			return (int)(XWPSError::SyntaxError);
  	else
  		c = *sp++;
		if (!IS_DIGIT(d, c))
			return (int)(XWPSError::SyntaxError);
		ival = 0;
		goto i2r;
  }
  
  ival = d;
  if (end - sp >= 3) 
  {
		if (!IS_DIGIT(d, (c = *sp))) 
		{
	    sp++;
	    goto ind;
		}
		ival = ival * 10 + d;
		if (!IS_DIGIT(d, (c = sp[1]))) 
		{
	    sp += 2;
	    goto ind;
		}
		ival = ival * 10 + d;
		sp += 3;
		if (!IS_DIGIT(d, (c = sp[-1])))
	    goto ind;
		ival = ival * 10 + d;
  }
  
  for (;; ival = ival * 10 + d)
  {
  	if (sp >= end)
  		goto iret;
  	else
  		c = *sp++;
  		
  	if (!IS_DIGIT(d, c))
	    break;
		if (WOULD_OVERFLOW(ival, d, max_int))
	    goto i2l;
  }
  
ind:
	switch (c)
	{
		case '.':
			if (sp >= end)
  			c = EOFC;
  		else
  			c = *sp++;	    	
	    goto i2r;
	    
	  default:
	    *psp = sp;
	    code = 1;
	    break;
	    
		case 'e':
		case 'E':
	    if (sign < 0)
				ival = -ival;
	    dval = ival;
	    exp10 = 0;
	    goto fe;
	    
	  case '#':
	  	{
	  		const uint radix = (uint)ival;				
	  		ulong uval = 0, lmax;

				if (sign || radix < min_radix || radix > max_radix)
					return (int)(XWPSError::SyntaxError);
		    		
		    if (!(radix & (radix - 1)))
		    {
		    	int shift = 0;
		    	switch (radix)
		    	{
		    		case 2:
			    		shift = 1; 
			    		lmax = max_ulong >> 1;
			    		break;
			    		
						case 4:
			    		shift = 2; 
			    		lmax = max_ulong >> 2;
			    		break;
			    		
						case 8:
			    		shift = 3; 
			    		lmax = max_ulong >> 3;
			    		break;
			    		
						case 16:
			    		shift = 4; 
			    		lmax = max_ulong >> 4;
			    		break;
			    		
						case 32:
			    		shift = 5; 
			    		lmax = max_ulong >> 5;
			    		break;
			    		
						default:
			    		return (int)(XWPSError::RangeCheck);
		    	}
		    	
		    	for (;; uval = (uval << shift) + d)
		    	{
		    		if (sp >= end)
		    			break;
  					else
  						c = *sp++;
  						
  					d = decoder[c];
						if (d >= radix) 
						{
			    		*psp = sp;
			    		code = 1;
			    		break;
						}
						if (uval > lmax)
							return (int)(XWPSError::SyntaxError);
		    	}
		    }
		    else
		    {
		    	int lrem = max_ulong % radix;
		    	lmax = max_ulong / radix;
		    	for (;; uval = uval * radix + d) 
		    	{
						if (sp >= end)
  						break;
  					else
  						c = *sp++;	   
						d = decoder[c];
						if (d >= radix) 
						{
			    		*psp = sp;
			    		code = 1;
			    		break;
						}
						if (uval >= lmax && (uval > lmax || d > lrem))
							return (int)(XWPSError::SyntaxError);
		    	}
		    }
		    pref->makeInt(uval);
				return code;
	  	}
	}
	
iret:
  pref->makeInt((sign < 0 ? -ival : ival));
  return code;
  
i2l:
  for (lval = ival;;)
  {
  	if (WOULD_OVERFLOW(lval, d, max_long))
  	{
  		if (lval == max_long / 10 && d == (int)(max_long % 10) + 1 && sign < 0) 
			{
				if (sp >= end)
  				c = EOFC;
  			else
  				c = *sp++;	   
				dval = -(double)min_long;
				if (c == 'e' || c == 'E' || c == '.') 
				{
		    	exp10 = 0;
		    	goto fs;
				} 
				else if (!IS_DIGIT(d, c)) 
				{
		    	lval = min_long;
		    	break;
				}
	    } 
	    else
				dval = lval;
	    goto l2d;
  	}
  	
  	lval = lval * 10 + d;
  	if (sp >= end)
  		goto lret;
  	else
  		c = *sp++;	  
		if (!IS_DIGIT(d, c))
	    break;
  }
  
	switch (c) 
	{
		case '.':
			if (sp >= end)
  			c = EOFC;
  		else
	    	c = *sp++;	  
	    exp10 = 0;
	    goto l2r;
	    
		default:
	    *psp = sp;
	    code = 1;
	    break;
	    
		case 'e':
		case 'E':
	    exp10 = 0;
	    goto le;
	    
		case '#':
	    return (int)(XWPSError::SyntaxError);
  }
    
lret:
  pref->makeInt((sign < 0 ? -lval : lval));
  return code;
  
l2d:
  exp10 = 0;
  for (;;)
  {
  	dval = dval * 10 + d;
		if (sp >= end)
  		c = EOFC;
  	else
	   	c = *sp++;	  
		if (!IS_DIGIT(d, c))
	    break;
  }
  
  switch (c)
  {
  	case '.':
	    if (sp >= end)
  			c = EOFC;
  		else
	   		c = *sp++;
	    exp10 = 0;
	    goto fd;
	    
		default:
	    *psp = sp;
	    code = 1;
		case EOFC:
	    if (sign < 0)
				dval = -dval;
	    goto rret;
	    
		case 'e':
		case 'E':
	    exp10 = 0;
	    goto fs;
		case '#':
	    return (int)(XWPSError::SyntaxError);
  }
  
i2r:
  exp10 = 0;
  while (IS_DIGIT(d, c))
  {
  	if (WOULD_OVERFLOW(ival, d, max_int)) 
  	{
	    lval = ival;
	    goto l2r;
		}
		
		ival = ival * 10 + d;
		exp10--;
		if (sp >= end)
  		c = EOFC;
  	else
	  	c = *sp++;
  }
  
  if (sign < 0)
		ival = -ival;
		
	if (!(c == 'e' || c == 'E' || exp10 < -NUM_POWERS_10))
	{
		if (c != EOFC)
		{
	    *psp = sp; 
	    code = 1;
	  }
		pref->makeReal(ival * neg_powers_10[-exp10]);
		return code;
	}
	
	dval = ival;
  goto fe;
  
l2r:
  while (IS_DIGIT(d, c)) 
  {
		if (WOULD_OVERFLOW(lval, d, max_long)) 
		{
	    dval = lval;
	    goto fd;
		}
		lval = lval * 10 + d;
		exp10--;
		if (sp >= end)
  		c = EOFC;
  	else
	  	c = *sp++;
  }
  
le:
  if (sign < 0)
		lval = -lval;
  dval = lval;
  goto fe;
  
fd:
  while (IS_DIGIT(d, c)) 
  {
		dval = dval * 10 + d;
		exp10--;
		if (sp >= end)
  		c = EOFC;
  	else
	  	c = *sp++;
  }
  
fs:
  if (sign < 0)
		dval = -dval;
		
fe:
	switch (c) 
	{
		case 'e':
		case 'E':
	    {
				int esign = 0;
				int iexp;
				if (sp >= end)
					return (int)(XWPSError::SyntaxError);
  			else
	  			c = *sp++;
				switch (c) 
				{
		    	case '-':
						esign = 1;
		    	case '+':
						if (sp >= end)
							return (int)(XWPSError::SyntaxError);
  					else
	  					c = *sp++;
				}
				if (!IS_DIGIT(d, c))
					return (int)(XWPSError::SyntaxError);
		    		
				iexp = d;
				for (;; iexp = iexp * 10 + d) 
				{
		    	if (sp >= end)
  					break;
  				else
	  				c = *sp++;
		    	if (!IS_DIGIT(d, c)) 
		    	{
						*psp = sp;
						code = 1;
						break;
		    	}
		    	if (iexp > 99)
		    		return (int)(XWPSError::SyntaxError);
				}
				if (esign)
		    	exp10 -= iexp;
				else
		    	exp10 += iexp;
				break;
	    }
	    
		default:
	    *psp = sp;
	    code = 1;
		case EOFC:
	    ;
  }
  
  if (exp10 > 0) 
  {
		while (exp10 > NUM_POWERS_10)
		{
	    dval *= powers_10[NUM_POWERS_10];
			exp10 -= NUM_POWERS_10;
		}
		if (exp10 > 0)
	    dval *= powers_10[exp10];
  } 
  else if (exp10 < 0) 
  {
		while (exp10 < -NUM_POWERS_10)
		{
	    dval /= powers_10[NUM_POWERS_10];
			exp10 += NUM_POWERS_10;
		}
		if (exp10 < 0)
	    dval /= powers_10[-exp10];
  }
  
  if (dval >= 0) 
  {
		if (dval > MAX_FLOAT)
			return (int)(XWPSError::SyntaxError);
  } 
  else 
  {
		if (dval < -MAX_FLOAT)
			return (int)(XWPSError::SyntaxError);
  }
  
rret:
  pref->makeReal(dval);
  return code;
}

int XWPSContextState::scanStringToken(XWPSRef * pstr, XWPSRef * pref)
{
	return scanStringTokenOptions(pstr, pref, 0);
}

int XWPSContextState::scanStringTokenOptions(XWPSRef * pstr, XWPSRef * pref, int options)
{
	if (!pstr->hasAttr(PS_A_READ))
		return (int)(XWPSError::InvalidAccess);
	
	XWPSStream st;
  XWPSStream *s = &st;
	s->readString(pstr->getBytes(), pstr->size(), false);
	s->cbuf_bytes = pstr->value.bytes->arr;
	if (s->cbuf_bytes)
		s->cbuf_bytes->incRef();
	XWPSScannerState state;
	state.init(options | SCAN_FROM_STRING);
	int code = scanToken(s, pref, &state);
	switch (code)
	{
		default:
	    if (code < 0)
				break;
				
		case 0:
		case scan_BOS:
	    {
				uint pos = s->tell();
				uchar * p = pstr->getBytes();
				p += pos;
				pstr->setBytesPtr(p);
				pstr->decSize(pos);
	    }
	    break;
	    
		case scan_Refill:	
	    code = (int)(XWPSError::SyntaxError);
		case scan_EOF:
	    break;
	}
	return code;
}

int XWPSContextState::scanToken(XWPSStream * s, XWPSRef * pref, XWPSScannerState * pstate)
{
	XWPSRef *myref = pref;	
	XWPSStreamCursor * cursor = s->getCursor();
	int retcode = 0;
  int c = 0;
	uchar *sptr = 0;
  uchar *endptr = 0;
  uchar *newptr = 0;
  uchar *daptr = 0;
  int max_name_ctype = (recognizeBtokens()? ctype_name : ctype_btoken);
  	
  uchar s1[2];
  const uchar *const decoder = scan_char_decoder;
  int status = 0;
  int sign = 0;
  bool check_only = (pstate->s_options & SCAN_CHECK_ONLY) != 0;
  XWPSScannerState  sstate;
  
  if (pstate->s_pstack != 0) 
  {
		if ( op_stack.getCurrentTop() < op_stack.getTop() ) 
		{
			op_stack.incCurrentTop(1);
		}
		else if ( (retcode = op_stack.push(1)) >= 0 )
			;
		else
	    return retcode;
		myref = op_stack.getCurrentTop();
  }
  
  if (pstate->s_scan_type != ScanningNone)
  {
  	sstate = *pstate;
  	if (!sstate.s_da.is_dynamic && sstate.s_da.base != sstate.s_da.buf)
  	{
  		uint next = sstate.s_da.next - sstate.s_da.base;
	    uint limit = sstate.s_da.limit - sstate.s_da.base;

	    sstate.s_da.base = sstate.s_da.buf;
	    sstate.s_da.next = sstate.s_da.buf + next;
	    sstate.s_da.limit = sstate.s_da.buf + limit;
  	}
  	daptr = sstate.s_da.next;
  	switch (sstate.s_scan_type)
  	{
  		case ScanningBinary:
				retcode = (this->*(sstate.s_ss.binary.cont))(s, myref, &sstate);
				sptr = cursor->r.ptr;
				endptr = cursor->r.limit;
				if (retcode == scan_Refill)
		    	goto pause;
				goto sret;
				
			case ScanningComment:
				sptr = cursor->r.ptr;
				endptr = cursor->r.limit;
				goto cont_comment;
				
	    case ScanningName:
				goto cont_name;
				
	    case ScanningString:
				goto cont_string;
				
	    default:
				return (int)(XWPSError::Fatal);
  	}
  }
  
  sstate.s_pstack = pstate->s_pstack;
  sstate.s_pdepth = pstate->s_pdepth;
  sstate.s_options = pstate->s_options;
  sptr = cursor->r.ptr;
	endptr = cursor->r.limit;
  
top:	
	c = s->scangetc(&sptr, &endptr);
	switch (c)
	{
		case ' ':
		case '\f':
		case '\t':
		case char_CR:
		case char_EOL:
		case char_NULL:
	    goto top;
	    
	  case '[':
		case ']':
	    s1[0] = (uchar) c;
	    retcode = nameRef(s1, 1, myref, 1);
	    myref->setAttrs(PS_A_EXECUTABLE);
	    break;
	    
	  case '<':
	  	if (level2Enabled())
	  	{
	  		if (sptr >= endptr)
	  		{
	  			sptr -= 1;
	  			sstate.s_scan_type = ScanningNone;
	  			goto pause;
	  		}
	  		c = s->scangetc(&sptr, &endptr);
	  		switch (c)
	  		{
	  			case '<':
						--sptr;
						sstate.s_ss.s_name.s_name_type = 0;
						sstate.s_ss.s_name.s_try_number = false;
						goto try_funny_name;
						
					case '~':
						sstate.s_ss.a85d.min_left = 1;
						sstate.s_ss.a85d.word = 0;
						sstate.s_ss.a85d.odd = 0;
						sstate.s_ss.st.templat = &s_A85D_template;
						goto str;
						
					default:
						break;
	  		}
	  		--sptr;
	  	}
	  	sstate.s_ss.axd.min_left = 1;
	  	sstate.s_ss.axd.odd = -1;
	  	sstate.s_ss.st.templat = &s_AXD_template;

str:
			cursor->r.ptr = sptr;
			sstate.s_da.init();
			
cont_string:
			for (;;)
			{
				PSStreamCursorWrite w;
				w.ptr = sstate.s_da.next - 1;
				w.limit = sstate.s_da.limit - 1;
				status = (*(sstate.s_ss.st.templat->process))(&sstate.s_ss.st, &(s->cursor.r), &w,s->end_status == EOFC);
				if (!check_only)
		    	sstate.s_da.next = w.ptr + 1;
		    	
		    switch (status)
		    {
		    	case 0:
						status = s->getEndStatus();
						if (status < 0) 
						{
			    		if (status == EOFC) 
			    		{
								if (check_only) 
								{
				    			retcode = scan_Refill;
				    			sstate.s_scan_type = ScanningString;
				    			goto suspend;
								} 
								else
								{
				    			retcode = (int)(XWPSError::SyntaxError);
				    			goto sret;
				    		}
			    		}
			    		break;
						}
						s->processReadBuf();
						continue;
						
					case 1:
						if (!check_only) 
						{
			    		retcode = sstate.s_da.grow(sstate.s_da.next, max_short);
			    		if (retcode == XWPSError::VMError) 
			    		{
								sstate.s_scan_type = ScanningString;
								goto suspend;
			    		} 
			    		else if (retcode < 0)
								goto sret;
						}
						continue;
						
					default:
						break;
		    }
		    break;
			}
			sptr = cursor->r.ptr;
			endptr = cursor->r.limit;
			switch (status) 
			{
				default:
		    	retcode = (int)(XWPSError::SyntaxError);
		    	goto sret;
		    	
				case INTC:
				case CALLC:
		   		sstate.s_scan_type = ScanningString;
		    	goto pause;
		    	
				case EOFC:
		    	;
	    }
	    retcode = dynamicMakeString(myref, &(sstate.s_da), sstate.s_da.next);
	    if (retcode < 0) 
	    {
				s->putBack();
				sstate.s_scan_type = ScanningString;
				goto suspend;
	    }
	  	break;
	  	
	  case '(':
	    sstate.s_ss.pssd.from_string = ((pstate->s_options & SCAN_FROM_STRING) != 0) &&	!level2Enabled();
	    sstate.s_ss.pssd.depth = 0;
	    sstate.s_ss.st.templat = &s_PSSD_template;
	    goto str;
	  	
	  case '{':
	  	if (sstate.s_pstack == 0) 
	  	{	
				if ( op_stack.getCurrentTop() < op_stack.getTop() ) 
				{
					op_stack.incCurrentTop(1);
				}
				else if ( (retcode = op_stack.push(1)) >= 0 )
					;
				else
				{
		    	--sptr;
		    	sstate.s_scan_type = ScanningNone;
		    	goto pause_ret;
				}
				sstate.s_pdepth = op_stack.count();
	    }
	    op_stack.getCurrentTop()->makeInt(sstate.s_pstack);
	    sstate.s_pstack = op_stack.count();
	    goto snext;
	  	break;
	  	
	  case '>':
	    if (level2Enabled()) 
	    {
	    	if (sptr >= endptr)
	    	{
	    		sptr -= 1;
	    		sstate.s_scan_type = ScanningNone;
	    		goto pause;
	    	}
				sstate.s_ss.s_name.s_name_type = 0;
				sstate.s_ss.s_name.s_try_number = false;
				goto try_funny_name;
	    }
	    
	  case ')':
	    retcode = (int)(XWPSError::SyntaxError);
		  goto sret;
		  break;
		  
		case '}':
			if (sstate.s_pstack == 0)
			{
				retcode = (int)(XWPSError::SyntaxError);
		  	goto sret;
			}
			op_stack.incCurrentTop(-1);
			{
				uint size = op_stack.count() - sstate.s_pstack;
				XWPSRef arr;
				if (sstate.s_pstack == sstate.s_pdepth)
					myref = pref;
				else
					myref = &arr;
					
				if (check_only) 
				{
		    	myref->makeArray(0);
		    	op_stack.pop(size);
				} 
				else if (array_packing.getBool()) 
				{
		    	retcode = makePackedArray(myref, op_stack.getStack(), size);
		    	if (retcode < 0) 
		    	{
						op_stack.incCurrentTop(1);
						--sptr;
						sstate.s_scan_type = ScanningNone;
						goto pause_ret;
		    	}
		    	myref->setAttrs(PS_A_EXECUTABLE);
				} 
				else 
				{
					myref->makeArray(currentSpace() | (PS_A_EXECUTABLE + PS_A_ALL), size);
		    	retcode = op_stack.store(myref, size, 0, 1, false);
		    	if (retcode < 0) 
						goto sret;
		    	op_stack.pop(size);
				}
				if (sstate.s_pstack == sstate.s_pdepth) 
				{	
					if (op_stack.getCurrentTop() >= op_stack.getBottom())
						op_stack.incCurrentTop(-1);
		    	else
		    		op_stack.pop(1);
		    	sstate.s_pstack = 0;
				} 
				else 
				{
		    	if (op_stack.getCurrentTop() < op_stack.getBottom())
						op_stack.popBlock();
		    	sstate.s_pstack = op_stack.getCurrentTop()->getInt();
		    	op_stack.getCurrentTop()->assign(&arr);
		    	goto snext;
				}
			}
			break;
			
		case '/':
	    if (sptr >= endptr)
	  	{
	  		sptr -= 1;
	  		sstate.s_scan_type = ScanningNone;
	  		goto pause;
	  	}
	    c = s->scangetc(&sptr, &endptr);
	    if (c == '/') 
	    {
				sstate.s_ss.s_name.s_name_type = 2;
				c = s->scangetc(&sptr, &endptr);
	    } 
	    else
				sstate.s_ss.s_name.s_name_type = 1;
	    sstate.s_ss.s_name.s_try_number = false;
	    switch (decoder[c]) 
	    {
				case ctype_name:
				default:
		    	goto do_name;
		    	
				case ctype_btoken:
		    	if (!recognizeBtokens())
						goto do_name;
				case ctype_exception:
				case ctype_space:
				case ctype_other:
		    	sstate.s_da.base = sstate.s_da.limit = daptr = 0;
		    	sstate.s_da.is_dynamic = false;
		    	goto nx;
	    }
	    
	  case '%':
	    {	
				uchar *base = sptr;
				uchar *end;
				while (++sptr < endptr)
				{
		    	switch (*sptr) 
		    	{
						case char_CR:
			    		end = sptr;
			    		if (sptr[1] == char_EOL)
								sptr++;
								
cend:
			    		retcode = scanComment(myref, &sstate,  base, end, false);
			    		if (retcode != 0)
								goto comment;
			    		goto top;
						case char_EOL:
						case '\f':
			    		end = sptr;
			    		goto cend;
		    	}
		    }
#define comment_line sstate.s_da.buf
				--sptr;
				comment_line[1] = 0;
				if (scan_comment_proc != NULL ||
		    		((sptr == base || base[1] == '%') &&
		     		scan_dsc_proc != NULL)) 
		    {
		    	uint len = sptr + 1 - base;

		    	memcpy(comment_line, base, len);
		    	daptr = comment_line + len;
				} 
				else 
				{	/* Not a DSC comment. */
		    	daptr = comment_line + (max_comment_line + 1);
				}
				sstate.s_da.base = comment_line;
				sstate.s_da.is_dynamic = false;
	    }
	    
cont_comment:
			for (;;)
			{
				switch ((c = s->scangetc(&sptr, &endptr))) 
				{
		    	default:
						if (c < 0)
			    		switch (c) 
			    		{
								case INTC:
								case CALLC:
				    			sstate.s_da.next = daptr;
				    			sstate.s_scan_type = ScanningComment;
				    			goto pause;
				    			
								case EOFC:
				    			goto end_comment;
				    			
								default:
				    			retcode = (int)(XWPSError::SyntaxError);
				    			goto sret;
			    		}
							if (daptr < comment_line + max_comment_line)
			    			*daptr++ = c;
						continue;
						
		    	case char_CR:
		    	case char_EOL:
		    	case '\f':
		    		
end_comment:
						retcode = scanComment(pref, &sstate, comment_line, daptr, true);
						if (retcode != 0)
			    		goto comment;
						goto top;
				}
			}
			
#undef comment_line
		
		case EOFC:
	    if (sstate.s_pstack != 0) 
	    {
				if (check_only)
		    	goto pause;
				retcode = (int)(XWPSError::SyntaxError);
				goto sret;
	    }
	    retcode = scan_EOF;
	    break;
	    
	  case ERRC:
	    retcode = (int)(XWPSError::IOError);
	    goto sret;
	    
try_funny_name:
			{
				int c1 = s->scangetc(&sptr, &endptr);
				if (c1 == c) 
				{
		    	s1[0] = s1[1] = c;
		    	nameRef(s1, 2, myref, 1);
		    	goto have_name;
				}
				--sptr;
	  	}
	  	retcode = (int)(XWPSError::SyntaxError);
			goto sret;
			
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
	    sign = 0;
	    
nr:
			retcode = scanNumber(sptr + (sign & 1), endptr, sign, myref, &newptr);
	    if (retcode == 1 && decoder[newptr[-1]] == ctype_space) 
	    {
				sptr = newptr - 1;
				if (*sptr == char_CR && sptr[1] == char_EOL)
		    	sptr++;
				retcode = 0;
				break;
	    }
	    sstate.s_ss.s_name.s_name_type = 0;
	    sstate.s_ss.s_name.s_try_number = true;
	    goto do_name;
	    
	  case '+':
	    sign = 1;
	    goto nr;
		case '-':
	    sign = -1;
	    goto nr;

	    /* Check for a binary object */
#define case4(c) case c: case c+1: case c+2: case c+3
	  	case4(128): case4(132): case4(136): case4(140):
	  	case4(144): case4(148): case4(152): case4(156):
#undef case4
		
				if (recognizeBtokens()) 
				{
					cursor->r.ptr = sptr;;
					retcode = scanBinaryToken(s, myref, &sstate);
					sptr = cursor->r.ptr;
					endptr = cursor->r.limit;
					if (retcode == scan_Refill)
		    		goto pause;
					break;
	  		}
	  	default:
	    	if (c < 0) 
	    	{
					sstate.s_da.init();	
					sstate.s_scan_type = ScanningNone;
					goto pause;
	    	}
	    	
	  case '!':
		case '"':
		case '#':
		case '$':
		case '&':
		case '\'':
		case '*':
		case ',':
		case '=':
		case ':':
		case ';':
		case '?':
		case '@':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '\\':
		case '^':
		case '_':
		case '`':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
		case '|':
		case '~':
			sstate.s_ss.s_name.s_name_type = 0;
	    sstate.s_ss.s_name.s_try_number = false;
	    
do_name:
			sstate.s_da.base = (uchar *) sptr;
	    sstate.s_da.is_dynamic = false;
	    {
				uchar *endp1 = endptr - 1;
				do 
				{
		    	if (sptr >= endp1)
						goto dyn_name;
				}
				while (decoder[*++sptr] <= max_name_ctype);
	    }
	    daptr = (uchar *) sptr;
	    c = *sptr;
	    goto nx;
	    
dyn_name:
			cursor->r.ptr = sptr;
			sstate.s_da.limit = (uchar *)++ sptr;
	    retcode = sstate.s_da.grow(sstate.s_da.limit, name_max_string);
	    if (retcode < 0) 
	    {
				sstate.s_da.save();
				if (retcode != XWPSError::VMError)
					goto sret;
				sstate.s_scan_type = ScanningName;
				goto pause_ret;
	    }
	    daptr = sstate.s_da.next;
	    
cont_name:
			sptr = cursor->r.ptr;
			endptr = cursor->r.limit;
			while (decoder[c = s->scangetc(&sptr, &endptr)] <= max_name_ctype) 
			{
				if (daptr == sstate.s_da.limit) 
				{
		    	retcode = sstate.s_da.grow(daptr, name_max_string);
		    	if (retcode < 0) 
		    	{
						sstate.s_da.save();
						if (retcode != XWPSError::VMError)
			    		goto sret;
						--sptr;
						sstate.s_scan_type = ScanningName;
						goto pause_ret;
		    	}
		    	daptr = sstate.s_da.next;
				}
				*daptr++ = c;
	    }
	    
nx:
			switch (decoder[c])
			{
				case ctype_btoken:
				case ctype_other:
		    	--sptr;
		    	break;
		    	
		    case ctype_space:
		    	if (c == char_CR) 
		    	{
						if (sptr >= endptr) 
						{
			    		if (s->getEndStatus() != EOFC) 
			    		{
								sptr--;
								goto pause_name;
			    		}
						} 
						else if (sptr[1] == char_EOL)
			    		sptr++;
		    	}
		    	break;
		    	
		    case ctype_exception:
		    	switch (c) 
		    	{
						case INTC:
						case CALLC:
			    		goto pause_name;
			    		
						case ERRC:
							retcode = (int)(XWPSError::IOError);
							goto sret;
			    		
						case EOFC:
			    		break;
		    	}
	    	}
	    	
	    	if (sstate.s_ss.s_name.s_try_number)
		    {
		    	uchar *base = sstate.s_da.base;
		    	switch (*base)
		    	{
		    		case '-':
		    			sign = -1;
		    			base++;
		    			break;
		    			
		    		case '+':
		    			sign = 1;
		    			base++;
		    			break;
		    			
		    		default:
		    			sign = 0;
		    	}

					retcode = scanNumber(base, daptr, sign, myref, &newptr);
					if (retcode == 1) 
					{
		    		retcode = 0;
					} 
					else if (retcode != XWPSError::SyntaxError) 
					{
		    		sstate.s_da.release();
		    		if (sstate.s_ss.s_name.s_name_type == 2)
		    		{
		    			retcode = (int)(XWPSError::SyntaxError);
							goto sret;
						}
		    		break;
					}
		    }
		    if (sstate.s_da.is_dynamic) 
		    {
					uint size = daptr - sstate.s_da.base;
					retcode = nameRef(sstate.s_da.base, size, myref, -1);
					if (retcode >= 0) 
					{
		    		sstate.s_da.release();
					} 
					else 
					{
		    		retcode = sstate.s_da.resize(size);
		    		if (retcode < 0) 
		    		{
							if (c != EOFC)
			    			--sptr;
							sstate.s_scan_type = ScanningName;
							goto pause_ret;
		    		}
		    		retcode = nameRef(sstate.s_da.base, size, myref, 2);
					}
	    	}
	    	else 
	    	{
					retcode = nameRef(sstate.s_da.base, (uint) (daptr - sstate.s_da.base), myref, !s->getForeign());
	    	}
	    	
	    	if (retcode < 0) 
	    	{
					if (retcode != XWPSError::VMError)
		    		goto sret;
					if (!sstate.s_da.is_dynamic) 
					{
		    		sstate.s_da.next = daptr;
		    		sstate.s_da.save();
					}
					if (c != EOFC)
		    		--sptr;
					sstate.s_scan_type = ScanningName;
					goto pause_ret;
	    	}
	    	
have_name:
				switch (sstate.s_ss.s_name.s_name_type)
				{
					case 0:
		    		if (myref->hasType(XWPSRef::Name))
							myref->setAttrs(PS_A_EXECUTABLE);
					case 1:
		    		break;
		    		
					case 2:
		    		{
							XWPSRef *pvalue;
							if (!myref->hasType(XWPSRef::Name))
							{
								retcode = (int)(XWPSError::Undefined);
								goto sret;
			    		}
							if ((pvalue = dictFindName(myref)) == 0)
							{
			    			retcode = (int)(XWPSError::Undefined);
								goto sret;
			    		}
			    		if (sstate.s_pstack != 0 && pvalue->space() > idmemory()->iallocSpace())
			    		{
			    			retcode = (int)(XWPSError::InvalidAccess);
								goto sret;
			    		}
							myref->assign(pvalue);
		    		}
				}
	}
	
sret:
	if (retcode < 0)
	{
		cursor->r.ptr = sptr;
		if (sstate.s_pstack != 0)
	    op_stack.pop(op_stack.count() - (sstate.s_pdepth - 1));
	  
		return retcode;
	}
	
	if (sstate.s_pstack == 0) 
	{
		cursor->r.ptr = sptr;
		return retcode;
  }
  
snext:
	if (op_stack.getCurrentTop() < op_stack.getTop())
		op_stack.incCurrentTop(1);
	else if ((retcode = op_stack.push(1)) >= 0)
		;
	else
	{
		cursor->r.ptr = sptr;
		sstate.s_scan_type = ScanningNone;
		goto save;
	}
	
	myref = op_stack.getCurrentTop();
  goto top;
  
pause_name:
	sstate.s_da.next = daptr;
  sstate.s_da.save();
  sstate.s_scan_type = ScanningName;
  
pause:
  retcode = scan_Refill;
    
pause_ret:
  cursor->r.ptr = sptr;
  
suspend:
  if (sstate.s_pstack != 0)
		op_stack.incCurrentTop(-1);
		
save:
	*pstate = sstate;
  return retcode;
  
comment:
  if (retcode < 0)
		goto sret;
		
  cursor->r.ptr = sptr;
  sstate.s_scan_type = ScanningNone;
  goto save;
}
