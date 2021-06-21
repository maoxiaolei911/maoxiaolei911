/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPSTransferMap.h"

XWPSTransferMap::XWPSTransferMap()
	:XWPSStruct()
{
	id = ps_no_id;
	values[0] = frac_0;
	setType(ps_tm_null);
}

XWPSTransferMap::XWPSTransferMap(XWPSTransferMap * other)
{
	id = other->id;
	memcpy(values, other->values, transfer_map_size * sizeof(short));
	proc = other->proc;
	type = other->type;
}

int XWPSTransferMap::getLength()
{
	return sizeof(XWPSTransferMap);
}

const char * XWPSTransferMap::getTypeName()
{
	return "transfermap";
}

bool  XWPSTransferMap::isIdentity()
{
	return type == ps_tm_identity;
}

void XWPSTransferMap::loadTransferMap(float min_value)
{
	short fmin = float2frac(min_value);
	for (int i = 0; i < transfer_map_size; i++)
	{
		float fval = (this->*proc)((float)i / (transfer_map_size - 1));
		values[i] = (fval < min_value ? fmin : fval >= 1.0 ? frac_1 : float2frac(fval));
	}
}

short XWPSTransferMap::mapColorFrac(short cf)
{
	if (proc == &XWPSTransferMap::identityTransfer)
		return cf;
		
	return colorFracMap(cf, values);
}

float XWPSTransferMap::mappedTransfer(float value)
{
	return (values[(int)(value * transfer_map_size + 0.5)] / frac_1_float);
}

short XWPSTransferMap::mapping(short cf)
{
	switch (type)
	{
		case ps_tm_null:
		case ps_tm_identity:
		case ps_tm_mapped:
			return (this->*proc)(cf);
			break;
			
		case ps_tm_frac:
			return mapColorFrac(cf);
			break;
			
		default:
			break;
	}
	
	return cf;
}

void XWPSTransferMap::setIdentityTransfer()
{
	setType(ps_tm_identity);
	for (int i = 0; i < transfer_map_size; ++i)
		values[i] = bits2frac(i, log2_transfer_map_size);
}

void XWPSTransferMap::setType(PSTransferMapType t)
{
	type = t;
	switch (type)
	{
		case ps_tm_null:
			proc = &XWPSTransferMap::nullTransfer;			
			break;
			
		case ps_tm_identity:
			proc = &XWPSTransferMap::identityTransfer;			
			break;
			
		case ps_tm_mapped:
			proc = &XWPSTransferMap::mappedTransfer;
			break;
			
		default:
			break;
	}
}

XWPSTransferMap & XWPSTransferMap::operator=(const XWPSTransferMap & other)
{
	id = other.id;
	memcpy(values, other.values, transfer_map_size * sizeof(short));
	proc = other.proc;
	type = other.type;
	return *this;
}

short XWPSTransferMap::colorFracMap(short cv, const short * valuesA)
{
#define cp_frac_bits (frac_bits - log2_transfer_map_size)

  int cmi = frac2bits_floor(cv, log2_transfer_map_size);
  short mv = valuesA[cmi];
  int rem, mdv;

  rem = cv - bits2frac(cmi, log2_transfer_map_size);
  if (rem == 0)
		return mv;
  mdv = valuesA[cmi + 1] - mv;
  
#if arch_ints_are_short
    if (mdv < -1 << (16 - cp_frac_bits) ||
	mdv > 1 << (16 - cp_frac_bits)
	)
	return mv + (uint) (((ulong) rem * mdv) >> cp_frac_bits);
#endif
  return mv + ((rem * mdv) >> cp_frac_bits);
#undef cp_frac_bits
}
