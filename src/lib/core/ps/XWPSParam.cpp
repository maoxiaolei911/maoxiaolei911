/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <stdio.h>
#include "XWPSError.h"
#include "XWPSStream.h"
#include "XWPSCosObject.h"
#include "XWPSDevicePDF.h"
#include "XWPSContextState.h"
#include "XWObject.h"
#include "XWPSParam.h"

PSParamCollection::~PSParamCollection()
{
	if (list)
	{
		if (!persistent)
		  delete list;
		list = 0;
	}
}

PSParamTypedValue::PSParamTypedValue()
{
	type = ps_param_type_null;
}

PSParamTypedValue::PSParamTypedValue(PSParamType t)
{
	type = t;
	switch (type) 
	{
		case ps_param_type_bool:
			value.s = false;
			break;
			
		case ps_param_type_int:
			value.i = 0;
			break;
			
  	case ps_param_type_long:
  		value.l = 0;
			break;
			
  	case ps_param_type_float:
  		value.f = 0.0;
			break;
			
		case ps_param_type_string:
			value.s = new PSParamString;
			break;
			
  	case ps_param_type_name:
  		value.n = new PSParamString;
			break;
			
  	case ps_param_type_int_array:
  		value.ia = new PSParamIntArray;
			break;
			
  	case ps_param_type_float_array:
  		value.fa = new PSParamFloatArray;
			break;
			
  	case ps_param_type_string_array:
  		value.sa = new PSParamStringArray;
			break;
			
  	case ps_param_type_name_array:
  		value.na = new PSParamStringArray;
			break;
			
		case ps_param_type_dict:
  	case ps_param_type_dict_int_keys:
  	case ps_param_type_array:
  		value.d = new PSParamCollection;
  		break;
			
		default:
			break;
	}
}

PSParamTypedValue::PSParamTypedValue(const bool b)
{
	value.b = b;
	type = ps_param_type_bool;
}

PSParamTypedValue::PSParamTypedValue(const int i)
{
	value.i = i;
	type = ps_param_type_int;
}

PSParamTypedValue::PSParamTypedValue(const long l)
{
	value.l = l;
	type = ps_param_type_long;
}

PSParamTypedValue::PSParamTypedValue(const float f)
{
	value.f = f;
	type = ps_param_type_long;
}

PSParamTypedValue::PSParamTypedValue(const int * ia, uint size)
{
	value.ia = new PSParamIntArray(ia, size);
	type = ps_param_type_int_array;
}

PSParamTypedValue::PSParamTypedValue(const PSParamIntArray * ia)
{
	value.ia = new PSParamIntArray(ia->data, ia->size);
	type = ps_param_type_int_array;
}

PSParamTypedValue::PSParamTypedValue(const float * fa, uint size)
{
	value.fa = new PSParamFloatArray(fa, size);
	type = ps_param_type_float_array;
}

PSParamTypedValue::PSParamTypedValue(const PSParamFloatArray * fa)
{
	value.fa = new PSParamFloatArray(fa->data, fa->size);
	type = ps_param_type_float_array;
}

PSParamTypedValue::PSParamTypedValue(const PSParamString * s, bool n)
{
	if (n)
	{
		value.n = new PSParamString((const uchar *)(s->data), s->size);
		type = ps_param_type_name;
	}
	else
	{
		value.s = new PSParamString((const uchar*)(s->data), s->size);
		type = ps_param_type_string;
	}
}

PSParamTypedValue::PSParamTypedValue(const PSParamStringArray * sa, bool n)
{
	if (n)
	{
		value.na = new PSParamStringArray(*sa);
		type = ps_param_type_name_array;
	}
	else
	{
		value.sa = new PSParamStringArray(*sa);
		type = ps_param_type_string_array;
	}
}

PSParamTypedValue::~PSParamTypedValue()
{
	clear();
}

void PSParamTypedValue::clear()
{
	switch (type) 
	{
		case ps_param_type_string:
			if (value.s)
				delete value.s;
			value.s = 0;
			break;
				
  	case ps_param_type_name:
  		if (value.n)
				delete value.n;
			value.n = 0;
			break;
				
  	case ps_param_type_int_array:
  		if (value.ia)
				delete value.ia;
			value.ia = 0;
			break;
				
  	case ps_param_type_float_array:
  		if (value.fa)
				delete value.fa;
			value.fa = 0;
			break;
				
  	case ps_param_type_string_array:
  		if (value.sa)
				delete value.sa;
			value.sa = 0;
			break;
				
  	case ps_param_type_name_array:
  		if (value.na)
				delete value.na;
			value.na = 0;
			break;
			
		case ps_param_type_dict:
  	case ps_param_type_dict_int_keys:
  	case ps_param_type_array:
  		if (value.d)
  			delete value.d;
  		value.d = 0;
  		break;
				
		default:
  		break;
	}
	
	type = ps_param_type_null;
}

void PSParamTypedValue::reset(PSParamType t)
{
	clear();
	type = t;
	switch (type) 
	{
		case ps_param_type_bool:
			value.s = false;
			break;
			
		case ps_param_type_int:
			value.i = 0;
			break;
			
  	case ps_param_type_long:
  		value.l = 0;
			break;
			
  	case ps_param_type_float:
  		value.f = 0.0;
			break;
			
		case ps_param_type_string:
			value.s = new PSParamString;
			break;
			
  	case ps_param_type_name:
  		value.n = new PSParamString;
			break;
			
  	case ps_param_type_int_array:
  		value.ia = new PSParamIntArray;
			break;
			
  	case ps_param_type_float_array:
  		value.fa = new PSParamFloatArray;
			break;
			
  	case ps_param_type_string_array:
  		value.sa = new PSParamStringArray;
			break;
			
  	case ps_param_type_name_array:
  		value.na = new PSParamStringArray;
			break;
			
		case ps_param_type_dict:
  	case ps_param_type_dict_int_keys:
  	case ps_param_type_array:
  		value.d = new PSParamCollection;
  		break;
						
		default:
			break;
	}
}

PSCParam::PSCParam()
{
	next = 0;
	alternate_typed_data = 0;
	type = ps_param_type_null;
}

PSCParam::~PSCParam()
{		
	clear();
}

void PSCParam::clear()
{
	switch (type) 
	{
		case ps_param_type_string:
			if (value.s)
			{
				delete value.s;
				value.s = 0;
			}
			break;
				
  	case ps_param_type_name:
  		if (value.n)
  		{
				delete value.n;
				value.n = 0;
			}
			break;
				
  	case ps_param_type_int_array:
  		if (value.ia)
  		{
				delete value.ia;
				value.ia = 0;
			}
			break;
				
  	case ps_param_type_float_array:
  		if (value.fa)
  		{
				delete value.fa;
				value.fa = 0;
			}
			break;
				
  	case ps_param_type_string_array:
  		if (value.sa)
  		{
				delete value.sa;
				value.sa = 0;
			}
			break;
				
  	case ps_param_type_name_array:
  		if (value.na)
  		{
				delete value.na;
				value.na = 0;
			}
			break;
			
		case ps_param_type_dict:
  	case ps_param_type_dict_int_keys:
  	case ps_param_type_array:
  		if (value.d)
  		{
  			delete value.d;
  			value.d = 0;
  		}
  		break;
				
		default:
  		break;
	}
	
	if (alternate_typed_data)
	{
		delete [] alternate_typed_data;
		alternate_typed_data = 0;
	}
	
	type = ps_param_type_null;
}

void PSCParam::reset(PSParamType t)
{
	clear();
	type = t;
	switch (type) 
	{
		case ps_param_type_bool:
			value.s = false;
			break;
			
		case ps_param_type_int:
			value.i = 0;
			break;
			
  	case ps_param_type_long:
  		value.l = 0;
			break;
			
  	case ps_param_type_float:
  		value.f = 0.0;
			break;
			
		case ps_param_type_string:
			value.s = new PSParamString;
			break;
			
  	case ps_param_type_name:
  		value.n = new PSParamString;
			break;
			
  	case ps_param_type_int_array:
  		value.ia = new PSParamIntArray;
			break;
			
  	case ps_param_type_float_array:
  		value.fa = new PSParamFloatArray;
			break;
			
  	case ps_param_type_string_array:
  		value.sa = new PSParamStringArray;
			break;
			
  	case ps_param_type_name_array:
  		value.na = new PSParamStringArray;
			break;
			
		case ps_param_type_dict:
  	case ps_param_type_dict_int_keys:
  	case ps_param_type_array:
  		value.d = new XWPSCParamList;
  		break;
			
		default:
			break;
	}
}

static const PSParamItem s_CF_param_items[] =
{
#define cfp(key, type, memb) { key, type, (int)((char *)&((PSStreamCFState *)0)->memb - (char *)((PSStreamCFState *)0))}
    cfp("Uncompressed", ps_param_type_bool, Uncompressed),
    cfp("K", ps_param_type_int, K),
    cfp("EndOfLine", ps_param_type_bool, EndOfLine),
    cfp("EncodedByteAlign", ps_param_type_bool, EncodedByteAlign),
    cfp("Columns", ps_param_type_int, Columns),
    cfp("Rows", ps_param_type_int, Rows),
    cfp("EndOfBlock", ps_param_type_bool, EndOfBlock),
    cfp("BlackIs1", ps_param_type_bool, BlackIs1),
    cfp("DamagedRowsBeforeError", ps_param_type_int, DamagedRowsBeforeError),
    cfp("FirstBitLowOrder", ps_param_type_bool, FirstBitLowOrder),
    cfp("DecodedByteAlign", ps_param_type_int, DecodedByteAlign),
#undef cfp
    {0,0,0}
};

#define dctp(key, type, stype, memb) { key, type, (int)((char *)&((stype *)0)->memb - (char *)((PSStreamCFState *)0))}
static const PSParamItem s_DCT_param_items[] =
{
dctp("ColorTransform", ps_param_type_int, PSStreamDCTState, ColorTransform),
    dctp("QFactor", ps_param_type_float, PSStreamDCTState, QFactor),
    {0,0,0}
};
static const PSParamItem jsd_param_items[] =
{
    dctp("Picky", ps_param_type_int, PSJPEGStreamData, Picky),
    dctp("Relax", ps_param_type_int, PSJPEGStreamData, Relax),
    {0,0,0}
};

#undef dctp

class XWPSDCTEScalars
{
public:
	XWPSDCTEScalars();
	
public:
	int Columns;
  int Rows;
  int Colors;
  PSParamString Markers;
  bool NoMarker;
  int Resync;
  int Blend;
};

XWPSDCTEScalars::XWPSDCTEScalars()
{
	Columns = 0;
	Rows = 0;
	Colors = -1;
	NoMarker = false;
	Resync = 0;
	Blend = 0;
}

static const PSParamItem s_DCTE_param_items[] =
{
#define dctp(key, type, memb) { key, type, _OFFSET_OF_(XWPSDCTEScalars, memb)}
    dctp("Columns", ps_param_type_int, Columns),
    dctp("Rows", ps_param_type_int, Rows),
    dctp("Colors", ps_param_type_int, Colors),
    dctp("Marker", ps_param_type_string, Markers),
    dctp("NoMarker", ps_param_type_bool, NoMarker),
    dctp("Resync", ps_param_type_int, Resync),
    dctp("Blend", ps_param_type_int, Blend),
#undef dctp
    {0,0,0}
};

XWPSParamList::XWPSParamList(QObject * parent)
	:QObject(parent),
	 persistent_keys(true)
{
}

int XWPSParamList::antiAliasBits(XWPSContextState * ctx, 
	                          const char * param_name, 
	                          int *pa)
{
	int code = readInt(ctx, param_name, pa);

  switch (code) 
  {
    case 0:
			switch (*pa) 
			{
				case 1: case 2: case 4:
	    		return 0;
				default:
	    		code = XWPSError::RangeCheck;
			}
			
    default:
			signalError(ctx, param_name, code);
    
    case 1:
			;
  }
  return code;
}

int XWPSParamList::checkBool(XWPSContextState * ctx, 
	                      const char * pname, 
	                      bool value,
		                    bool defined)
{
	int code;
  bool new_value;

  switch (code = readBool(ctx, pname, &new_value)) 
  {
		case 0:
	    if (defined && new_value == value)
				break;
	    code = XWPSError::RangeCheck;
	    goto e;
	    
		default:
	    if (readNull(ctx, pname) == 0)
				return 1;
				
e:
			signalError(ctx, pname, code);
		case 1:
	    ;
  }
  return code;
}

int XWPSParamList::checkBytes(XWPSContextState * ctx, 
	                       const char * pname, 
	                       const uchar * str,
		  								   uint size, 
		  								   bool defined)
{
	int code;
  PSParamString new_value;

  switch (code = readString(ctx, pname, &new_value)) 
  {
		case 0:
	    if (defined && new_value.size == size &&
					!memcmp((const char *)str, (const char *)new_value.data,size))
				break;
	    code = XWPSError::RangeCheck;
	    goto e;
	    
		default:
	    if (readNull(ctx, pname) == 0)
				return 1;
e:
			signalError(ctx, pname, code);
		case 1:
	    ;
  }
  return code;
}

int XWPSParamList::checkLong(XWPSContextState * ctx, 
	                      const char * pname, 
	                      long value,
		                    bool defined)
{
	int code;
  long new_value;
  switch (code = readLong(ctx, pname, &new_value)) 
  {
		case 0:
	    if (defined && new_value == value)
				break;
	    code = XWPSError::RangeCheck;
	    goto e;
		default:
	    if (readNull(ctx, pname) == 0)
				return 1;
e:
			signalError(ctx, pname, code);
			
		case 1:
	    ;
   }
   return code;
}

int XWPSParamList::checkPassword(XWPSContextState * ctx, const PSPassword * ppass)
{
	if (ppass->size != 0) 
	{
		PSPassword pass;
		int code = readPassword(ctx, "Password", &pass);
		if (code)
	    return code;
	    
		if (pass.size != ppass->size ||
	    memcmp(&pass.data[0], &ppass->data[0],  ppass->size) != 0)
	    return 1;
  }
  return 0;
}

int XWPSParamList::copy(XWPSContextState * ctx, XWPSParamList * plfrom)
{
	XWPSParamEnumerator key_enum;
	PSParamString key;
	PSParamCollectionType coll_type;
	int code;
	char string_key[256];
	while ((code = plfrom->nextKey(ctx, &key_enum, &key)) == 0)
	{
		if (key.size > sizeof(string_key) - 1) 
		{
	    code = (int)(XWPSError::RangeCheck);
	    break;
		}
		memcpy(string_key, key.data, key.size);
		string_key[key.size] = 0;
		PSParamTypedValue value;
		value.type = ps_param_type_any;
		code = plfrom->readRequestedTyped(ctx, string_key, &value);
		if (code != 0) 
		{
	    code = (code > 0 ? (int)(XWPSError::Unknown) : code);
	    break;
		}
		setPersistentKeys(key.persistent);
		PSParamTypedValue copy;
		switch (value.type)
		{
			case ps_param_type_dict:
	    	coll_type = ps_param_collection_dict_any;
	    	goto cc;
	    	
			case ps_param_type_dict_int_keys:
	    	coll_type = ps_param_collection_dict_int_keys;
	    	goto cc;
	    	
			case ps_param_type_array:
	    	coll_type = ps_param_collection_array;
	    	
cc:
				copy.reset(ps_param_type_dict_int_keys);
	    	copy.value.d->size = value.value.d->size;
	    	if ((code = beginTransmit(ctx, string_key,  copy.value.d, coll_type)) < 0 ||
						(code = copy.value.d->list->copy(ctx, value.value.d->list)) < 0 ||
						(code = endTransmit(ctx, string_key, copy.value.d)) < 0)
						break;
	    	code = plfrom->endTransmit(ctx, string_key, value.value.d);
	    	break;
	    	
			default:
	    	code = transmit(ctx, string_key, &value);
		}
		if (code < 0)
	    break;
	}
	return code;
}

int XWPSParamList::getParams(XWPSContextState * ctx, const PSStreamCFState * ss, bool all)
{
	PSStreamCFState cfs_defaults;
  PSStreamCFState *defaults;

  if (all)
		defaults = 0;
  else 
  {
		s_CF_set_defaults_inline(&cfs_defaults);
		defaults = &cfs_defaults;
  }
  return writeItems(ctx, (void*)ss, (void*)defaults, (PSParamItem*)(&s_CF_param_items));
}

int XWPSParamList::sDCTDGetParams(XWPSContextState * ctx, const PSStreamDCTState * ss, bool all)
{
	PSStreamDCTState dcts_defaults;
  const PSStreamDCTState *defaults;

  if (all)
		defaults = 0;
  else 
  {
		(*s_DCTE_template.set_defaults)((PSStreamState *) &dcts_defaults);
		defaults = &dcts_defaults;
  }
  
  return sDCTGetParams(ctx, ss, defaults);
}

int XWPSParamList::sDCTEGetParams(XWPSContextState * ctx, const PSStreamDCTState * ss, bool all)
{
	PSStreamDCTState dcts_defaults;
  const PSStreamDCTState *defaults = 0;
  XWPSDCTEScalars params;
  const PSJPEGCompressData *jcdp = ss->data.compress;
  int code;
  XWPSDCTEScalars dcte_scalars_default;
  
  if (!all) 
  {
		PSJPEGCompressData *jcdp_default = (PSJPEGCompressData *)malloc(sizeof(PSJPEGCompressData));
		defaults = &dcts_defaults;
		(*s_DCTE_template.set_defaults) ((PSStreamState *) & dcts_defaults);
		dcts_defaults.data.compress = jcdp_default;
		jpeg_create_compress(&dcts_defaults.data.compress->cinfo);
		jpeg_stream_data_common_init(dcts_defaults.data.compress);
	    
		dcts_defaults.data.common->Picky = 0;
		dcts_defaults.data.common->Relax = 0;
  }
  params.Columns = jcdp->cinfo.image_width;
  params.Rows = jcdp->cinfo.image_height;
  params.Colors = jcdp->cinfo.input_components;
  params.Markers.data = ss->Markers.data;
  params.Markers.size = ss->Markers.size;
  params.NoMarker = ss->NoMarker;
  params.Resync = jcdp->cinfo.restart_interval;
  
  if ((code = sDCTGetParams(ctx, ss, defaults)) < 0 ||
			(code = writeItems(ctx, &params,  &dcte_scalars_default, (PSParamItem*)(&s_DCTE_param_items[0]))) < 0 ||
			(code = dcteGetSamples(ctx, "HSamples", params.Colors,jcdp, false, all)) < 0 ||
			(code = dcteGetSamples(ctx, "VSamples", params.Colors, jcdp, true, all)) < 0 ||
    	(code = sDCTGetQuantizationTables(ctx, ss, defaults, true)) < 0 ||
			(code = sDCTGetHuffmanTables(ctx, ss, defaults, true)) < 0)
			;
			

	if (defaults) 
	{
		ps_jpeg_destroy(&dcts_defaults);
		if (dcts_defaults.data.compress)
		{
			free(dcts_defaults.data.compress);
			dcts_defaults.data.compress = 0;
		}
  }
  return code;
}

int XWPSParamList::sDCTGetQuantizationTables(XWPSContextState * ctx, 
	                                           const PSStreamDCTState * pdct, 
	                                           const PSStreamDCTState * defaults,
			                                       bool is_encode)
{
	jpeg_component_info d_comp_info[4];
  int num_in_tables;
  const jpeg_component_info *comp_info;
  const jpeg_component_info *default_comp_info;
  JQUANT_TBL **table_ptrs;
  JQUANT_TBL **default_table_ptrs;
  PSParamCollection quant_tables;
  float QFactor = pdct->QFactor;
  int i;
  int code;
  
  if (is_encode) 
  {
		num_in_tables = pdct->data.compress->cinfo.num_components;
		comp_info = pdct->data.compress->cinfo.comp_info;
		table_ptrs = pdct->data.compress->cinfo.quant_tbl_ptrs;
		if (defaults) 
		{
	    default_comp_info = defaults->data.compress->cinfo.comp_info;
	    default_table_ptrs = defaults->data.compress->cinfo.quant_tbl_ptrs;
		}
  }
  else
  {
  	num_in_tables = quant_tables.size;
		for (i = 0; i < num_in_tables; ++i)
	    d_comp_info[i].quant_tbl_no = i;
		comp_info = d_comp_info;
		table_ptrs = pdct->data.decompress->dinfo.quant_tbl_ptrs;
		if (defaults) 
		{
	    default_comp_info = d_comp_info;
	    default_table_ptrs =
			defaults->data.decompress->dinfo.quant_tbl_ptrs;
		}
  }
  
  if (defaults) 
  {
		bool match = true;

		for (i = 0; i < num_in_tables; ++i) 
		{
	    JQUANT_TBL *tbl = table_ptrs[comp_info[i].quant_tbl_no];
	    JQUANT_TBL *default_tbl = (default_comp_info == 0 || default_table_ptrs == 0 ? 0 :
	     			default_table_ptrs[default_comp_info[i].quant_tbl_no]);

	    if (tbl == default_tbl)
				continue;
	    if (tbl == 0 || default_tbl == 0 ||
					memcmp(tbl->quantval, default_tbl->quantval, DCTSIZE2 * sizeof(quint16))) 
			{
				match = false;
				break;
	    }
		}
		if (match)
	    return 0;
  }
  
  quant_tables.size = num_in_tables;
  code = beginTransmit(ctx, "QuantTables",	&quant_tables, ps_param_collection_array);
  if (code < 0)
		return code;
		
	for (i = 0; i < num_in_tables; ++i)
	{
		char key[3];
		PSParamString str;
		PSParamFloatArray fa;

		sprintf(key, "%d", i);
		if (QFactor == 1.0) 
		{
	    code = str.quantParamString(DCTSIZE2, table_ptrs[comp_info[i].quant_tbl_no]->quantval, QFactor);
	    switch (code)
	    {
	    	case 0:
		    	code = quant_tables.list->writeString(ctx, key, &str);
		    	if (code < 0)
						return code;
		    	continue;
		    
				default:
		    	return code;
		    
				case 1:
		    	break;
	    }
		}
		
		code = fa.quantParamArray(DCTSIZE2, table_ptrs[comp_info[i].quant_tbl_no]->quantval, QFactor);
		if (code < 0)
	    return code;
	    
	  code = quant_tables.list->writeFloatArray(ctx, key, &fa);
		if (code < 0)
	    return code;
	}
	
	return endTransmit(ctx, "QuantTables", &quant_tables);
}

int XWPSParamList::sDCTGetHuffmanTables(XWPSContextState * ctx,
	   															 const PSStreamDCTState * pdct, 
	   															 const PSStreamDCTState * ,
			                             bool is_encode)
{
	PSParamString *huff_data;
  PSParamStringArray hta;
  int num_in_tables;
  jpeg_component_info *comp_info;
  JHUFF_TBL **dc_table_ptrs;
  JHUFF_TBL **ac_table_ptrs;
  int i;
  int code = 0;
  
  if (is_encode) 
  {
		dc_table_ptrs = pdct->data.compress->cinfo.dc_huff_tbl_ptrs;
		ac_table_ptrs = pdct->data.compress->cinfo.ac_huff_tbl_ptrs;
		num_in_tables = pdct->data.compress->cinfo.input_components * 2;
		comp_info = pdct->data.compress->cinfo.comp_info;
  } 
  else 
  {
		dc_table_ptrs = pdct->data.decompress->dinfo.dc_huff_tbl_ptrs;
		ac_table_ptrs = pdct->data.decompress->dinfo.ac_huff_tbl_ptrs;
		for (i = 2; i > 0; --i)
	    if (dc_table_ptrs[i - 1] || ac_table_ptrs[i - 1])
				break;
		num_in_tables = i * 2;
		comp_info = NULL;
  }
  
  huff_data = new PSParamString[num_in_tables];
  for (i = 0; i < num_in_tables; i += 2) 
  {
  	PSParamString * p0 = huff_data + i;
  	PSParamString * p1 = huff_data + i + 1;
		if ((code = p0->packHuffTable(ac_table_ptrs[i >> 1])) < 0 ||
	    	(code = p1->packHuffTable(dc_table_ptrs[i >> 1])))
	    break;
  }
  if (code < 0)
		return code;
  hta.data = huff_data;
  hta.size = num_in_tables;  
  hta.persistent = false;
  return writeStringArray(ctx, "HuffTables", &hta);
}

int XWPSParamList::sDCTGetParams(XWPSContextState * ctx, 
					                  const PSStreamDCTState * ss,
		                        const PSStreamDCTState * defaults)
{
	int code = writeItems(ctx, (void*)ss, (void*)defaults, (PSParamItem*)(&s_DCT_param_items[0]));

  if (code >= 0)
		code = writeItems(ctx, (void*)(ss->data.common), (void*)(defaults ? defaults->data.common : NULL), (PSParamItem*)(&jsd_param_items[0]));
  return code;
}

int XWPSParamList::sDCTByteParams(XWPSContextState * ctx, 
					                   const char * key, 
					                   int start,
		                         int countA, 
		                         quint8 * pvals)
{
	int i;
  PSParamString bytes;
  PSParamFloatArray floats;
  int code = readString(ctx, key, &bytes);

  switch (code) 
  {
		case 0:
	    if (bytes.size < start + countA) 
	    {
				code = XWPSError::RangeCheck;
				break;
	    }
	    for (i = 0; i < countA; ++i)
				pvals[i] = (quint8) bytes.data[start + i];
	    return 0;
	    
		default:
	    code = readFloatArray(ctx, key, &floats);
	    if (!code) 
	    {
				if (floats.size < start + countA) 
				{
		    	code = XWPSError::RangeCheck;
		    	break;
				}
				for (i = 0; i < countA; ++i) 
				{
		    	float v = floats.data[start + i];

		    	if (v < 0 || v > 255) 
		    	{
						code = XWPSError::RangeCheck;
						break;
		    	}
		    	pvals[i] = (quint8) (v + 0.5);
				}
	    }
  }
    
  return code;
}

int XWPSParamList::quantParams(XWPSContextState * ctx, 
					                const char * key, 
					                int countA,
	                        quint16 * pvals, 
	                        float QFactor)
{
	int i;
  PSParamString bytes;
  PSParamFloatArray floats;
  int code = readString(ctx, key, &bytes);

  switch (code) 
  {
		case 0:
	    if (bytes.size != countA) 
	    {
				code = XWPSError::RangeCheck;
				break;
	    }
	    for (i = 0; i < countA; ++i) 
	    {
				double v = bytes.data[i] * QFactor;

				pvals[jpeg_order(i)] = (quint16) (v < 1 ? 1 : v > 255 ? 255 : v + 0.5);
	    }
	    return 0;
	    
		default:
	    code = readFloatArray(ctx, key, &floats);
	    if (!code) 
	    {
				if (floats.size != countA) 
				{
		    	code = XWPSError::RangeCheck;
		    	break;
				}
				for (i = 0; i < countA; ++i) 
				{
		    	double v = floats.data[i] * QFactor;

		    	pvals[jpeg_order(i)] =(quint16) (v < 1 ? 1 : v > 255 ? 255 : v + 0.5);
				}
	    }
    }
    
  return code;
#undef jpeg_order
}

int XWPSParamList::sDCTPutParams(XWPSContextState * ctx, PSStreamDCTState * pdct)
{
	int code = readItems(ctx, pdct, (PSParamItem*)(&s_DCT_param_items[0]));

  if (code < 0)
		return code;
  code = readItems(ctx, pdct->data.common, (PSParamItem*)(&jsd_param_items[0]));
  if (code < 0)
		return code;
  if (pdct->data.common->Picky < 0 || pdct->data.common->Picky > 1 ||
			pdct->data.common->Relax < 0 || pdct->data.common->Relax > 1 ||
			pdct->ColorTransform < -1 || pdct->ColorTransform > 2 ||
			pdct->QFactor < 0.0 || pdct->QFactor > 1000000.0)
			return (int)(XWPSError::RangeCheck);
  return 0;
}

int XWPSParamList::sDCTPutQuantizationTables(XWPSContextState * ctx, 
					                              PSStreamDCTState * pdct,
			                                  bool is_encode)
{
	int code;
  int i, j;
  PSParamCollection quant_tables;
  int num_in_tables;
  int num_out_tables;
  jpeg_component_info *comp_info;
  JQUANT_TBL **table_ptrs;
  JQUANT_TBL *this_table;

  switch ((code = beginTransmit(ctx, "QuantTables",  &quant_tables, ps_param_collection_dict_int_keys))) 
  {
		case 1:
	    return 0;
		default:
	    return code;
		case 0:
	    ;
  }
  if (is_encode) 
  {
		num_in_tables = pdct->data.compress->cinfo.num_components;
		if (quant_tables.size < num_in_tables)
	    return (int)(XWPSError::RangeCheck);
		comp_info = pdct->data.compress->cinfo.comp_info;
		table_ptrs = pdct->data.compress->cinfo.quant_tbl_ptrs;
  } 
  else 
  {
		num_in_tables = quant_tables.size;
		comp_info = NULL;
		table_ptrs = pdct->data.decompress->dinfo.quant_tbl_ptrs;
  }
  num_out_tables = 0;
  for (i = 0; i < num_in_tables; ++i) 
  {
		char istr[5];	
		quint16 values[DCTSIZE2];

		sprintf(istr, "%d", i);
		code = quant_tables.list->quantParams(ctx, istr, DCTSIZE2, values, pdct->QFactor);
		if (code < 0)
	    return code;
	
		for (j = 0; j < num_out_tables; j++) 
		{
	    if (!memcmp(table_ptrs[j]->quantval, values, sizeof(values)))
				break;
		}
		if (comp_info != NULL)
	    comp_info[i].quant_tbl_no = j;
		if (j < num_out_tables)
	    continue;
		if (++num_out_tables > NUM_QUANT_TBLS)
	    return (int)(XWPSError::RangeCheck);
		this_table = table_ptrs[j];
		if (this_table == NULL) 
		{
	    this_table = jpeg_alloc_quant_table((j_common_ptr)&(pdct->data.compress->cinfo));
	    table_ptrs[j] = this_table;
		}
		memcpy(this_table->quantval, values, sizeof(values));
  }
  return 0;
}

int XWPSParamList::sDCTPutHuffmanTables(XWPSContextState * ctx, 
					                         PSStreamDCTState * pdct,
			                             bool is_encode)
{
	int code;
  int i, j;
  PSParamCollection huff_tables;
  int num_in_tables;
  int ndc, nac;
  int codes_size;
  jpeg_component_info *comp_info;
  JHUFF_TBL **dc_table_ptrs;
  JHUFF_TBL **ac_table_ptrs;
  JHUFF_TBL **this_table_ptr;
  JHUFF_TBL *this_table;
  int max_tables = 2;

  switch ((code = beginTransmit(ctx, "HuffTables", &huff_tables, ps_param_collection_dict_int_keys))) 
  {
		case 1:
	    return 0;
		default:
	    return code;
		case 0:
	    ;
  }
  if (is_encode) 
  {
		num_in_tables = pdct->data.compress->cinfo.input_components * 2;
		if (huff_tables.size < num_in_tables)
	    return (int)(XWPSError::RangeCheck);
		comp_info = pdct->data.compress->cinfo.comp_info;
		dc_table_ptrs = pdct->data.compress->cinfo.dc_huff_tbl_ptrs;
		ac_table_ptrs = pdct->data.compress->cinfo.ac_huff_tbl_ptrs;
		if (pdct->data.common->Relax)
	    max_tables = qMax(pdct->data.compress->cinfo.input_components, 2);
  } 
  else 
  {
		num_in_tables = huff_tables.size;
		comp_info = NULL;
		dc_table_ptrs = pdct->data.decompress->dinfo.dc_huff_tbl_ptrs;
		ac_table_ptrs = pdct->data.decompress->dinfo.ac_huff_tbl_ptrs;
		if (pdct->data.common->Relax)
	    max_tables = NUM_HUFF_TBLS;
  }
  ndc = nac = 0;
  for (i = 0; i < num_in_tables; ++i) 
  {
		char istr[5];
		quint8 counts[16], values[256];

	
		sprintf(istr, "%d", i);
		code = huff_tables.list->sDCTByteParams(ctx, istr, 0, 16, counts);
		if (code < 0)
	    return code;
		for (codes_size = 0, j = 0; j < 16; j++)
	    codes_size += counts[j];
		if (codes_size > 256)
	    return (int)(XWPSError::RangeCheck);
		code = huff_tables.list->sDCTByteParams(ctx, istr, 16, codes_size,values);
		if (code < 0)
	    return code;
		if (i & 1) 
		{
	    j = findHuffValues(ac_table_ptrs, nac, counts, values,codes_size);
	    if (comp_info != NULL)
				comp_info[i >> 1].ac_tbl_no = j;
	    if (j < nac)
				continue;
	    if (++nac > NUM_HUFF_TBLS)
				return (int)(XWPSError::RangeCheck);
	    this_table_ptr = ac_table_ptrs + j;
		} 
		else 
		{
	    j = findHuffValues(dc_table_ptrs, ndc, counts, values,codes_size);
	    if (comp_info != NULL)
				comp_info[i >> 1].dc_tbl_no = j;
	    if (j < ndc)
				continue;
	    if (++ndc > NUM_HUFF_TBLS)
				return (int)(XWPSError::RangeCheck);
	    this_table_ptr = dc_table_ptrs + j;
		}
		this_table = *this_table_ptr;
		if (this_table == NULL) 
		{
	    this_table = jpeg_alloc_huff_table((j_common_ptr) &(pdct->data.compress->cinfo));
	    *this_table_ptr = this_table;
		}
		memcpy(this_table->bits, counts, sizeof(counts));
		memcpy(this_table->huffval, values, codes_size * sizeof(values[0]));
  }
  if (nac > max_tables || ndc > max_tables)
		return (int)(XWPSError::RangeCheck);
  return 0;
}

int XWPSParamList::sDCTDPutParams(XWPSContextState * ctx, PSStreamDCTState * pdct)
{
	int code;

  if ((code = sDCTPutParams(ctx, pdct)) < 0 ||
			(code = sDCTPutHuffmanTables(ctx, pdct, false)) < 0 ||
			(code = sDCTPutQuantizationTables(ctx, pdct, false)) < 0)
		;
  return code;
}

int XWPSParamList::sDCTEPutParams(XWPSContextState * ctx, PSStreamDCTState * pdct)
{
	PSJPEGCompressData *jcdp = pdct->data.compress;
  XWPSDCTEScalars params;
  int i;
  int code;

  code = readItems(ctx, (void*)&params, (PSParamItem*)(&s_DCTE_param_items[0]));
  if (code < 0)
		return code;
  if (params.Columns <= 0 || params.Columns > 0xffff ||
			params.Rows <= 0 || params.Rows > 0xffff ||
			params.Colors <= 0 || params.Colors == 2 || params.Colors > 4 ||
			params.Resync < 0 || params.Resync > 0xffff ||
			params.Blend < 0 || params.Blend > 1)
		return (int)(XWPSError::RangeCheck);
			
	jcdp->Picky = 0;
  jcdp->Relax = 0;
  if ((code = sDCTPutParams(ctx, pdct)) < 0 ||
			(code = sDCTPutHuffmanTables(ctx, pdct, false)) < 0)
		return code;
		
	switch ((code = sDCTPutQuantizationTables(ctx, pdct, false)))
	{
		case 0:
	    break;
		default:
	    return code;
		case 1:
	    if (pdct->QFactor != 1.0) 
	    {
				code = ps_jpeg_set_linear_quality(pdct,(int)(qMin(pdct->QFactor, (float)100.0) * 100.0 + 0.5), true);
				if (code < 0)
		    	return code;
	    }
	}
	
	jcdp->cinfo.image_width = params.Columns;
  jcdp->cinfo.image_height = params.Rows;
  jcdp->cinfo.input_components = params.Colors;
  switch (params.Colors) 
  {
		case 1:
	    jcdp->cinfo.in_color_space = JCS_GRAYSCALE;
	    break;
		case 3:
	    jcdp->cinfo.in_color_space = JCS_RGB;
	    break;
		case 4:
	    jcdp->cinfo.in_color_space = JCS_CMYK;
	    break;
		default:
	    jcdp->cinfo.in_color_space = JCS_UNKNOWN;
  }
  if ((code = ps_jpeg_set_defaults(pdct)) < 0)
		return code;
		
	switch (params.Colors)
	{
		case 3:
	    if (pdct->ColorTransform < 0)
				pdct->ColorTransform = 1;
	    if (pdct->ColorTransform == 0) 
	    {
				if ((code = ps_jpeg_set_colorspace(pdct, JCS_RGB)) < 0)
		    	return code;
	    } 
	    else
				pdct->ColorTransform = 1;
	    break;
		case 4:
	    if (pdct->ColorTransform < 0)
				pdct->ColorTransform = 0;
	    if (pdct->ColorTransform != 0) 
	    {
				if ((code = ps_jpeg_set_colorspace(pdct, JCS_YCCK)) < 0)
		    	return code;
				pdct->ColorTransform = 2;
	    } 
	    else 
	    {
				if ((code = ps_jpeg_set_colorspace(pdct, JCS_CMYK)) < 0)
		    	return code;
	    }
	    break;
		default:
	    pdct->ColorTransform = 0;
	    break;
	}
	
	pdct->Markers.data = params.Markers.data;
  pdct->Markers.size = params.Markers.size;
  pdct->NoMarker = params.NoMarker;
  if ((code = dctePutSamples(ctx, "HSamples", params.Colors,jcdp, false)) < 0 ||
			(code = dctePutSamples(ctx, "VSamples", params.Colors, jcdp, true)) < 0)
		return code;
  jcdp->cinfo.write_JFIF_header = false;
  jcdp->cinfo.write_Adobe_marker = false;
  jcdp->cinfo.restart_interval = params.Resync;
  if (pdct->data.common->Relax == 0)
  {
  	jpeg_component_info *comp_info = jcdp->cinfo.comp_info;
		int num_samples;

		for (i = 0, num_samples = 0; i < params.Colors; i++)
	    num_samples += comp_info[i].h_samp_factor *	comp_info[i].v_samp_factor;
		if (num_samples > 10)
	    return (int)(XWPSError::RangeCheck);
  }
  
  return 0;
}

int XWPSParamList::mediaSize(XWPSContextState *ctx, 
	                           const char * pname,
				                     const float *res, 
				                     PSParamFloatArray * pa)
{
  int ecode = 0;
  int code;

	switch (code = readFloatArray(ctx, pname,pa))
	{
		case 0:
			if (pa->size != 2)
			{
				ecode = XWPSError::RangeCheck;
				pa->data = 0;
			}
			else
			{
				float width_new = pa->data[0] * res[0] / 72;
				float height_new = pa->data[1] * res[1] / 72;
				if (width_new < 0 || height_new < 0)
	    		ecode = XWPSError::RangeCheck;
#define max_coord (max_fixed / fixed_1)
#if max_coord < max_int
				else if (width_new > max_coord || height_new > max_coord)
	    		ecode = XWPSError::LimitCheck;
#endif
#undef max_coord
				else
	    		break;
			}
			goto e;
			
		default:
			ecode = code;
			
e:	
	    signalError(ctx, pname, ecode);
	    
	  case 1:
	  	break;
	}
  return ecode;
}

int XWPSParamList::putBool(XWPSContextState * ctx, 
	                         const char * param_name,
	                         bool * pval, 
	                         int ecode)
{
	int code = readBool(ctx, param_name, pval);
  switch (code) 
  {
		default:
	    ecode = code;
	    signalError(ctx, param_name, ecode);
		case 0:
		case 1:
	    break;
  }
  return ecode;
}

int XWPSParamList::putEnum(XWPSContextState * ctx, 
	                         const char * param_name,
	                         int *pvalue, 
	                         const char *const pnames[], 
	                         int ecode)
{
	PSParamString ens;
  int code = readName(ctx, param_name, &ens);
  switch (code) 
  {
		case 1:
	    return ecode;
	    
		case 0:
			for (int i = 0; pnames[i] != 0; ++i)
			{
		    if (paramStringEq(&ens, pnames[i])) 
		    {
					*pvalue = i;
					return 0;
		    }
		  }
	    code = (int)(XWPSError::RangeCheck);
	    	
		default:
	    ecode = code;
	    signalError(ctx, param_name, code);
  }
  return code;
}

int XWPSParamList::putInt(XWPSContextState * ctx, 
	                        const char * param_name,
	                        int *pval, 
	                        int ecode)
{
	int code = readInt(ctx, param_name, pval);
  switch (code) 
  {
		default:
	    ecode = code;
	    signalError(ctx, param_name, ecode);
		case 0:
		case 1:
	    break;
  }
  return ecode;
}

int XWPSParamList::putLong(XWPSContextState * ctx, 
	                         const char * param_name,
	                         long *pval, 
	                         int ecode)
{
	int code = readLong(ctx, param_name, pval);
  switch (code) 
  {
		default:
	    ecode = code;
	    signalError(ctx, param_name, ecode);
		case 0:
		case 1:
	    break;
  }
  return ecode;
}

#define cf_max_height 32000

int XWPSParamList::putParams(XWPSContextState * ctx, PSStreamCFState * ss)
{
	PSStreamCFState state;
  int code;

  state = *ss;
  code = readItems(ctx, (void *)&state, (PSParamItem*)(&s_CF_param_items[0]));
  if (code >= 0 &&	(state.K < -cf_max_height || state.K > cf_max_height ||
	 		state.Columns < 0 || state.Columns > cfe_max_width ||
	 		state.Rows < 0 || state.Rows > cf_max_height ||
	 		state.DamagedRowsBeforeError < 0 ||
	 		state.DamagedRowsBeforeError > cf_max_height ||
	 		state.DecodedByteAlign < 1 || state.DecodedByteAlign > 16 ||
	 		(state.DecodedByteAlign & (state.DecodedByteAlign - 1)) != 0))
		code = XWPSError::RangeCheck;
  if (code >= 0)
		*ss = state;
  return code;
}

int XWPSParamList::readBool(XWPSContextState * ctx, const char * pkey, bool * pvalue)
{
	PSParamTypedValue typed(ps_param_type_bool);
  int code = readRequestedTyped(ctx, pkey, &typed);
  if (code == 0)
  	*pvalue = typed.value.b;
  	
  return code;
}

int XWPSParamList::readFloat(XWPSContextState * ctx, const char * pkey, float *pvalue)
{
	PSParamTypedValue typed(ps_param_type_float);
  int code = readRequestedTyped(ctx, pkey, &typed);
  if (code == 0)
  	*pvalue = typed.value.f;
  	
  return code;
}

int XWPSParamList::readFloatArray(XWPSContextState * ctx, const char * pkey, PSParamFloatArray * pvalue)
{
	PSParamTypedValue typed(ps_param_type_float_array);
  int code = readRequestedTyped(ctx, pkey, &typed);
  if (code == 0)
  	*pvalue = *(typed.value.fa);
  	
  return code;
}

int XWPSParamList::readInt(XWPSContextState * ctx, const char * pkey, int *pvalue)
{
	PSParamTypedValue typed(ps_param_type_int);
  int code = readRequestedTyped(ctx, pkey, &typed);
  if (code == 0)
  	*pvalue = typed.value.i;
  	
  return code;
}

int XWPSParamList::readIntArray(XWPSContextState * ctx, const char * pkey, PSParamIntArray * pvalue)
{
	PSParamTypedValue typed(ps_param_type_int_array);
  int code = readRequestedTyped(ctx, pkey, &typed);
  if (code == 0)
  	*pvalue = *(typed.value.ia);
  	
  return code;
}

int XWPSParamList::readItems(XWPSContextState * ctx, void * obj, PSParamItem * items)
{
	PSParamItem * pi = items;
	PSParamTypedValue * typed = 0;
	int ecode = 0;
	for (; pi->key != 0; ++pi)
	{
		char *key = (char*)(pi->key);
		void *pvalue = (void *)((char *)obj + pi->offset);
		typed = new PSParamTypedValue((PSParamType)(pi->type));
		int code = readRequestedTyped(ctx, (const char*)key, typed);
		switch (code) 
		{
	    default:
				ecode = code;
	    case 1:
				break;
				
	    case 0:
				if (typed->type != pi->type)	
		    	ecode = (int)(XWPSError::TypeCheck);
			else
			{
				switch (pi->type)
				{
					case ps_param_type_bool:
						*((bool*)pvalue) = typed->value.b;
						break;
						
					case ps_param_type_int:
						*((int*)pvalue) = typed->value.i;
						break;
						
  				case ps_param_type_long:
  					*((long*)pvalue) = typed->value.l;
						break;
						
  				case ps_param_type_float:
  					*((float*)pvalue) = (typed->value.f);
						break;
						
  				case ps_param_type_string:
  					*((PSParamString*)pvalue) = *(typed->value.s);
  					break;
  					
  				case ps_param_type_name:
  					*((PSParamString*)pvalue) = *(typed->value.n);
  					break;
  					
  				case ps_param_type_int_array:
  					*((PSParamIntArray*)pvalue) = *(typed->value.ia);
  					break;
  					
  				case ps_param_type_float_array:
  					*((PSParamFloatArray*)pvalue) = *(typed->value.fa);
  					break;
  					
  				case ps_param_type_string_array:
  					*((PSParamStringArray*)pvalue) = *(typed->value.sa);
  					break;
  					
  				case ps_param_type_name_array:
  					*((PSParamStringArray*)pvalue) = *(typed->value.na);
  					break;
  					
  				default:
  					break;
				}
		  }
		}
		
		delete typed;
		typed = 0;
	}
	
	return ecode;
}

int XWPSParamList::readLong(XWPSContextState * ctx, const char * pkey, long *pvalue)
{
	PSParamTypedValue typed(ps_param_type_long);
  int code = readRequestedTyped(ctx, pkey, &typed);
  if (code == 0)
  	*pvalue = typed.value.l;
  	
  return code;
}

int XWPSParamList::readName(XWPSContextState * ctx, const char * pkey,	PSParamString * pvalue)
{
	PSParamTypedValue typed(ps_param_type_name);
  int code = readRequestedTyped(ctx, pkey, &typed);
  if (code == 0)
  	*pvalue = *(typed.value.n);
  	
  return code;
}

int XWPSParamList::readNameArray(XWPSContextState * ctx, const char * pkey, PSParamStringArray * pvalue)
{
	PSParamTypedValue typed(ps_param_type_name_array);
  int code = readRequestedTyped(ctx, pkey, &typed);
  if (code == 0)
  	*pvalue = *(typed.value.na);
  	
  return code;
}

int XWPSParamList::readNull(XWPSContextState * ctx, const char * pkey)
{
	PSParamTypedValue typed(ps_param_type_null);
  return readRequestedTyped(ctx, pkey, &typed);
}

int XWPSParamList::readPassword(XWPSContextState * ctx, const char *kstr, PSPassword * ppass)
{
	PSParamString ps(ppass->data, ppass->size);
	int code = readString(ctx, kstr, &ps);
  switch (code)
  {
  	case 0:
	    ppass->size = ps.size;
	    return 0;
	    
		case 1:	
	    return 1;
  }
  
  if (code != XWPSError::TypeCheck)
		return code;
    
  long ipass=0;
  code = readLong(ctx, kstr, &ipass);
  if (code != 0)
		return code;
    
  sprintf((char *)ppass->data, "%ld", ipass);
  ppass->size = strlen((char *)ppass->data);
  return 0;
}

int XWPSParamList::readRequestedTyped(XWPSContextState * ctx, const char* pkey, PSParamTypedValue * pvalue)
{
	PSParamType req_type = pvalue->type;
	int code = transmit(ctx, pkey, pvalue);
  if (code != 0)
		return code;
  
  return coerceTyped(pvalue, req_type);
}

int XWPSParamList::readString(XWPSContextState * ctx, const char* pkey, PSParamString * pvalue)
{
	PSParamTypedValue typed(ps_param_type_string);
  int code = readRequestedTyped(ctx, pkey, &typed);
  if (code == 0)
  	*pvalue = *(typed.value.s);
  	
  return code;
}

int XWPSParamList::readStringArray(XWPSContextState * ctx, const char* pkey, PSParamStringArray * pvalue)
{
	PSParamTypedValue typed(ps_param_type_string_array);
  int code = readRequestedTyped(ctx, pkey, &typed);
  if (code == 0)
  	*pvalue = *(typed.value.sa);
  	
  return code;
}

int XWPSParamList::writeBool(XWPSContextState * ctx, const char * pkey, const bool * pvalue)
{
	PSParamTypedValue typed(*pvalue);
	return transmit(ctx, pkey, &typed);
}

int XWPSParamList::writeFloat(XWPSContextState * ctx, const char * pkey, const float *pvalue)
{
	PSParamTypedValue typed(*pvalue);
	return transmit(ctx, pkey, &typed);
}

int XWPSParamList::writeFloatArray(XWPSContextState * ctx, 
	                                 const char * pkey, 
	                                 const PSParamFloatArray * pvalue)
{
	PSParamTypedValue typed(pvalue);
	return transmit(ctx, pkey, &typed);
}

int XWPSParamList::writeFloatArray(XWPSContextState * ctx, const char *key, const float *values, int countA)
{
	if (countA != 0) 
	{
		PSParamFloatArray fa(values, countA);
		return writeFloatArray(ctx, key, &fa);
  }
  return 0;
}

int XWPSParamList::writeFloatValues(XWPSContextState * ctx, 
	                                  const char * pkey, 
	                                  const float *values, 
	                                  uint size, 
	                                  bool)
{
	PSParamFloatArray fa(values, size);
  return writeFloatArray(ctx, pkey, &fa);
}

int XWPSParamList::writeInt(XWPSContextState * ctx, const char * pkey, const int *pvalue)
{
	PSParamTypedValue typed(*pvalue);
	return transmit(ctx, pkey, &typed);
}

int XWPSParamList::writeIntArray(XWPSContextState * ctx, 
	                               const char * pkey, 
	                               const PSParamIntArray * pvalue)
{
	PSParamTypedValue typed(pvalue);
	return transmit(ctx, pkey, &typed);
}

int XWPSParamList::writeIntValues(XWPSContextState * ctx, 
	                                const char * pkey, 
	                                const int *values, 
	                                uint size, 
	                                bool )
{
	PSParamIntArray ia(values, size);
  return writeIntArray(ctx, pkey, &ia);
}

int XWPSParamList::writeItems(XWPSContextState * ctx, 
	                            void *obj, 
	                            void *default_obj, 
	                            PSParamItem * items)
{
	PSParamItem * pi = items;
	PSParamTypedValue * typed = 0;
	int ecode = 0;
	for (; pi->key != 0; ++pi)
	{
		char *key = (char*)(pi->key);
	  void *pvalue = (void *)((char *)obj + pi->offset);
	  void *dpvalue = 0;
	  if (default_obj != 0)
	  	dpvalue = (void *)((char *)default_obj + pi->offset);
	  bool samevalue = false;
	 	typed = new PSParamTypedValue((PSParamType)(pi->type));
	 	int code = 0;
	 	switch (pi->type)
	 	{
	 		case ps_param_type_bool:
				typed->value.b = *((bool*)pvalue);
				if (dpvalue != 0)
				  samevalue = !memcmp(dpvalue, pvalue, sizeof(bool));
				break;
						
			case ps_param_type_int:
				typed->value.i =	*((int*)pvalue);
				if (dpvalue != 0)
				  samevalue = !memcmp(dpvalue, pvalue, sizeof(int));
				break;
						
  		case ps_param_type_long:
  			typed->value.l =	*((long*)pvalue);
  			if (dpvalue != 0)
				  samevalue = !memcmp(dpvalue, pvalue, sizeof(long));
				break;
						
  		case ps_param_type_float:
  			typed->value.f = *((float*)pvalue);
  			if (dpvalue != 0)
				  samevalue = !memcmp(dpvalue, pvalue, sizeof(float));
				break;
						
  		case ps_param_type_string:
  			*(typed->value.s) =	*((PSParamString*)pvalue);
  			if (dpvalue != 0)
				  samevalue = !memcmp((void*)(((PSParamString*)dpvalue)->data), (void*)(((PSParamString*)pvalue)->data), typed->value.s->size);
  			break;
  					
  		case ps_param_type_name:
  			*(typed->value.n) = *((PSParamString*)pvalue);
  			if (dpvalue != 0)
				  samevalue = !memcmp((void*)(((PSParamString*)dpvalue)->data), (void*)(((PSParamString*)pvalue)->data), typed->value.n->size);
  			break;
  					
  		case ps_param_type_int_array:
  			*(typed->value.ia) = *((PSParamIntArray*)pvalue);
  			if (dpvalue != 0)
				  samevalue = !memcmp((void*)(((PSParamIntArray*)dpvalue)->data), (void*)(((PSParamIntArray*)pvalue)->data), typed->value.ia->size * sizeof(int));
  			break;
  					
  		case ps_param_type_float_array:
  			*(typed->value.fa) = *((PSParamFloatArray*)pvalue);
  			if (dpvalue != 0)
				  samevalue = !memcmp((void*)(((PSParamFloatArray*)dpvalue)->data), (void*)(((PSParamFloatArray*)pvalue)->data), typed->value.ia->size * sizeof(float));
  			break;
  					
  		case ps_param_type_string_array:
  			*(typed->value.sa) = *((PSParamStringArray*)pvalue);
  			break;
  					
  		case ps_param_type_name_array:
  			*(typed->value.na) = *((PSParamStringArray*)pvalue);
  			break;
  					
  		default:
  			break;
	 	}
	 	if (!samevalue)
		   code = transmit(ctx, (const char*)key, typed);
		delete typed;
		typed = 0;
		if (code < 0)
	    ecode = code;
	}
	
	return ecode;
}

int XWPSParamList::writeLong(XWPSContextState * ctx, const char * pkey, const long *pvalue)
{
	PSParamTypedValue typed(*pvalue);
	return transmit(ctx, pkey, &typed);
}

int XWPSParamList::writeName(XWPSContextState * ctx, const char * pkey, const PSParamString * pvalue)
{
	PSParamTypedValue typed(pvalue, true);
	return transmit(ctx, pkey, &typed);
}

int XWPSParamList::writeNameArray(XWPSContextState * ctx, const char * pkey, const PSParamStringArray * pvalue)
{
	PSParamTypedValue typed(pvalue, true);
	return transmit(ctx, pkey, &typed);
}

int XWPSParamList::writeNull(XWPSContextState * ctx, const char * pkey)
{
	PSParamTypedValue typed;
  return transmit(ctx, pkey, &typed);
}

int XWPSParamList::writePassword(XWPSContextState * ctx, const char *kstr, const PSPassword * ppass)
{
	PSParamString ps(ppass->data, ppass->size);
	if (ps.size > MAX_PASSWORD)
		return (int)(XWPSError::LimitCheck);
		
  return writeString(ctx, kstr, &ps);
}

int XWPSParamList::writeString(XWPSContextState * ctx, const char * pkey, const PSParamString * pvalue)
{
	PSParamTypedValue typed(pvalue, false);
	return transmit(ctx, pkey, &typed);
}

int XWPSParamList::writeStringArray(XWPSContextState * ctx, const char * pkey, const PSParamStringArray * pvalue)
{
	PSParamTypedValue typed(pvalue, false);
	return transmit(ctx, pkey, &typed);
}

int XWPSParamList::coerceTyped(PSParamTypedValue * pvalue, PSParamType req_type)
{
	if (req_type == -1 || pvalue->type == req_type)
		return 0;
		
	switch (pvalue->type)
	{
		case ps_param_type_int:
			switch (req_type) 
			{
				case ps_param_type_long:
		    	pvalue->value.l = pvalue->value.i;
		    	goto ok;
		    	
				case ps_param_type_float:
		    	pvalue->value.f = (float)pvalue->value.l;
		    	goto ok;
		    	
				default:
		    	break;
	    }
	    break;
	    
	  case ps_param_type_long:
	    switch (req_type) 
	    {
				case ps_param_type_int:
#if arch_sizeof_int < arch_sizeof_long
		    	if (pvalue->value.l != (int)pvalue->value.l)
						return (int)(XWPSError::RangeCheck);
#endif
		    	pvalue->value.i = (int)pvalue->value.l;
		    	goto ok;
		    
				case ps_param_type_float:
		    	pvalue->value.f = (float)pvalue->value.l;
		    	goto ok;
		    	
				default:
		    	break;
	    }
	    break;
	    
	  case ps_param_type_string:
	    if (req_type == ps_param_type_name)
				goto ok;
	    break;
	    
		case ps_param_type_name:
	    if (req_type == ps_param_type_string)
				goto ok;
	    break;
	    
	  case ps_param_type_int_array:
	    switch (req_type) 
	    {
				case ps_param_type_float_array:
					{
						uint size = pvalue->value.ia->size;
						float *fv = new float[size];
						if (fv == 0)
			    		return (int)(XWPSError::VMError);
						for (uint i = 0; i < size; ++i)
			    		fv[i] = pvalue->value.ia->data[i];
			    	
			    	pvalue->reset(ps_param_type_float_array);			    	
						pvalue->value.fa->data = fv;
						pvalue->value.fa->size = size;
						pvalue->value.fa->persistent = false;
						goto ok;
		    	}
		    	
				default:
		    	break;
	    }
	    break;
	    
	  case ps_param_type_string_array:
	    if (req_type == ps_param_type_name_array)
				goto ok;
	    break;
	    
		case ps_param_type_name_array:
	    if (req_type == ps_param_type_string_array)
				goto ok;
	    break;
	    
		case ps_param_type_array:
	    if (pvalue->value.d->size == 0 &&
					(req_type == ps_param_type_int_array ||
		 			req_type == ps_param_type_float_array ||
		 			req_type == ps_param_type_string_array ||
		 			req_type == ps_param_type_name_array))
				goto ok;
	    break;
	    
		default:
	    break;
	}
	
	 return (int)(XWPSError::TypeCheck);
ok:
	pvalue->type = req_type;
  return 0;
}

int XWPSParamList::dcteGetSamples(XWPSContextState * ctx, 
	                   const char *  key, 
	                   int num_colors,
                     const PSJPEGCompressData * jcdp, 
                     bool is_vert, 
                     bool all)
{
	const jpeg_component_info *comp_info = jcdp->cinfo.comp_info;
  int samples[4];
  bool write = all;
  int i;

  for (i = 0; i < num_colors; ++i)
		write |= (samples[i] = (is_vert ? comp_info[i].v_samp_factor :	comp_info[i].h_samp_factor)) != 1;
  if (write) 
  {
		int *data = new int[num_colors * sizeof(int)];
		PSParamIntArray sa;
		sa.data = data;
		sa.size = num_colors;		
		memcpy(data, samples, num_colors * sizeof(samples[0]));
		sa.persistent = false;
		return writeIntArray(ctx, key, &sa);
   }
   return 0;
}

int XWPSParamList::dctePutSamples(XWPSContextState * ctx, 
	                   const char * key, 
	                   int num_colors,
		                 PSJPEGCompressData * jcdp, 
		                 bool is_vert)
{
	int code;
  int i;
  jpeg_component_info *comp_info = jcdp->cinfo.comp_info;
  quint8 samples[4];
  
  switch ((code = sDCTByteParams(ctx, key, 0, num_colors, samples))) 
  {
		default:
	    return code;
		case 0:
	    break;
		case 1:
	    samples[0] = samples[1] = samples[2] = samples[3] = 1;
  }
  for (i = 0; i < num_colors; i++) 
  {
		if (samples[i] < 1 || samples[i] > 4)
	    return (int)(XWPSError::RangeCheck);
		if (is_vert)
	    comp_info[i].v_samp_factor = samples[i];
		else
	    comp_info[i].h_samp_factor = samples[i];
  }
  return 0;
}

int XWPSParamList::findHuffValues(JHUFF_TBL ** table_ptrs, 
	                     int num_tables,
	                     const quint8 counts[16], 
	                     const quint8 * values, 
	                     int codes_size)
{
	int j;

  for (j = 0; j < num_tables; ++j)
		if (!memcmp(table_ptrs[j]->bits, counts, sizeof(counts)) &&
	    !memcmp(table_ptrs[j]->huffval, values, codes_size * sizeof(values[0])))
	    break;
  return j;
}

XWPSCParamList::XWPSCParamList(QObject * parent)
	:XWPSParamList(parent),
	 writing(false)
{
	head = 0;
	target = 0;
	count = 0;
	any_requested = false;	
	coll_type = ps_param_collection_dict_any;
}

XWPSCParamList::~XWPSCParamList()
{
	release();
}

int XWPSCParamList::beginTransmit(XWPSContextState * ctx, 
	                                const char * pkey, 
	                                PSParamCollection * pvalue, 
	                                PSParamCollectionType coll_typeA)
{
	if (writing)
		return beginWrite(ctx, pkey, pvalue, coll_typeA);
	
	return beginRead(ctx, pkey, pvalue, coll_typeA);
}

int XWPSCParamList::endTransmit(XWPSContextState * ctx, const char * pkey, PSParamCollection * pvalue)
{
	if (writing)
		return endWrite(ctx, pkey, pvalue);
	
	return endRead(ctx, pkey, pvalue);
}

int XWPSCParamList::getPolicy(XWPSContextState *,const char *)
{
	if (writing)
		return -1;
		
	return ps_param_policy_ignore;
}

int XWPSCParamList::nextKey(XWPSContextState *, XWPSParamEnumerator * penum, PSParamString *key)
{
	if (writing)
		return -1;
		
	return readNextKey(penum, key);
}

int XWPSCParamList::request(XWPSContextState * ctx, const char *pkey)
{
	if (writing)
		return writeRequest(ctx, pkey);
		
	return 0;
}

int XWPSCParamList::requested(XWPSContextState * ctx, const char *pkey)
{
	if (writing)
		return writeRequested(ctx, pkey);
		
	return -1;
}

int XWPSCParamList::signalError(XWPSContextState * ctx, const char * pkey, int code)
{
	if (writing)
		return -1;
		
	return readSignalError(ctx, pkey, code);
}

int XWPSCParamList::transmit(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue)
{
	if (writing)
		return write(ctx, pkey, pvalue);
		
	return read(ctx, pkey, pvalue);
}

PSCParam * XWPSCParamList::add(const char * pkey)
{
	PSCParam *pparam = new PSCParam;
	if (pparam == 0)
		return 0;
		
	uint len = strlen(pkey);
	if (len <= 0)
		return pparam;
		
	pparam->next = head;
  uchar *str = new uchar[len];
  if (!str)
  {
  	delete pparam;
  	return 0;
  }
  	
  memcpy(str, pkey, len);
	pparam->key.data = str;
	pparam->key.persistent = false;
  
  pparam->key.size = len;
  pparam->next = head;
  return pparam;
}

int XWPSCParamList::beginWrite(XWPSContextState * , 
	                             const char * , 
	                             PSParamCollection * pvalue, 
	                             PSParamCollectionType coll_typeA)
{
	XWPSCParamList *dlist = new XWPSCParamList;
  if (dlist == 0)
		return (int)(XWPSError::VMError);
			
  dlist->coll_type = coll_typeA;
  pvalue->list = dlist;
  pvalue->persistent = false;
  dlist->write();
  return 0;
}

int XWPSCParamList::beginRead(XWPSContextState * ctx, 
	                            const char * pkey, 
	                            PSParamCollection * pvalue, 
	                            PSParamCollectionType coll_typeA)
{
	PSCParam *pparam = find(pkey, false);
  if (pparam == 0)
		return (target ? target->beginTransmit(ctx, pkey, pvalue, coll_typeA) : 1);
    
  switch (pparam->type) 
  {
		case ps_param_type_dict:
	    if (coll_typeA != ps_param_collection_dict_any)
				return (int)(XWPSError::TypeCheck);
	    break;
	    
		case ps_param_type_dict_int_keys:
	    if (coll_typeA == ps_param_collection_array)
				return (int)(XWPSError::TypeCheck);
	    break;
	    
		case ps_param_type_array:
	    break;
	    
		default:
	    return (int)(XWPSError::TypeCheck);
  }
  
  if (!pparam->value.d)
		pparam->value.d = new XWPSCParamList;
	pparam->value.d->read(); 
	if (pvalue->list)
	{
		if (!pvalue->persistent)
			delete pvalue->list;
	}
  pvalue->list = pparam->value.d;
  pvalue->persistent = true;
  pvalue->size = pparam->value.d->count;
  return 0;
}

int XWPSCParamList::endRead(XWPSContextState * , const char * , PSParamCollection * )
{
	return 0;
}

int XWPSCParamList::endWrite(XWPSContextState * , const char * pkey, PSParamCollection * pvalue)
{
	XWPSCParamList *dlist = (XWPSCParamList *) pvalue->list;
  return write(pkey, pvalue->list, 
		           (dlist->coll_type == ps_param_collection_dict_int_keys ?
		            ps_param_type_dict_int_keys :
		            dlist->coll_type == ps_param_collection_array ?
		            ps_param_type_array : ps_param_type_dict));
}

PSCParam * XWPSCParamList::find(const char * pkey, bool any)
{
	PSCParam *pparam = head;
  uint len = strlen(pkey);
  for (; pparam != 0; pparam = pparam->next)
  {
		if (pparam->key.size == len && !memcmp(pparam->key.data, pkey, len))
	    return (pparam->type != -1 || any ? pparam : 0);
	}
  return 0;
}

int  XWPSCParamList::read(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue)
{
	PSParamType req_type = pvalue->type;
  PSCParam *pparam = find(pkey, false);
  if (pparam == 0)
		return (target ? target->transmit(ctx, pkey, pvalue) : 1);
  pvalue->reset(pparam->type);
  switch (pvalue->type) 
  {
		case ps_param_type_dict:
		case ps_param_type_dict_int_keys:
		case ps_param_type_array:
			pparam->reset(pparam->type);
			if (!pparam->value.d)
				pparam->value.d = new XWPSCParamList;
			pparam->value.d->read();
	    pvalue->value.d->list = pparam->value.d;
	    pvalue->value.d->persistent = true;
	    pvalue->value.d->size = pparam->value.d->count;
	    return 0;
	    
	  case ps_param_type_bool:
				pvalue->value.b = pparam->value.b;
				break;
						
			case ps_param_type_int:
				pvalue->value.i =	pparam->value.i;
				break;
						
  		case ps_param_type_long:
  			pvalue->value.l =	pparam->value.l;
				break;
						
  		case ps_param_type_float:
  			pvalue->value.f = pparam->value.f;
				break;
						
  		case ps_param_type_string:
  			*(pvalue->value.s) =	*(pparam->value.s);
  			break;
  					
  		case ps_param_type_name:
  			*(pvalue->value.n) = *(pparam->value.n);
  			break;
  					
  		case ps_param_type_int_array:
  			*(pvalue->value.ia) = *(pparam->value.ia);
  			break;
  					
  		case ps_param_type_float_array:
  			*(pvalue->value.fa) = *(pparam->value.fa);
  			break;
  					
  		case ps_param_type_string_array:
  			*(pvalue->value.sa) = *(pparam->value.sa);
  			break;
  					
  		case ps_param_type_name_array:
  			*(pvalue->value.na) = *(pparam->value.na);
  			break;
	    
		default:
	    break;
  }
  
  int code = coerceTyped(pvalue, req_type);
  if (code == XWPSError::TypeCheck &&	
  	  req_type == ps_param_type_float_array &&
	    pvalue->type == ps_param_type_int_array)
	{
		pvalue->reset(ps_param_type_float_array);
		pvalue->value.fa->size = pparam->value.ia->size;
		if (pparam->alternate_typed_data == 0) 
		{
			pparam->alternate_typed_data = new float[pvalue->value.fa->size];
	    if (pparam->alternate_typed_data == 0)
		      return (int)(XWPSError::VMError);

	    for (int element = 0; element < pvalue->value.fa->size; ++element)	    
				((float *)(pparam->alternate_typed_data))[element] = (float)pparam->value.ia->data[element];
		}
		pvalue->value.fa->data = pparam->alternate_typed_data;
		return 0;
	}
	
	return code;
}

int  XWPSCParamList::readGetPolicy(XWPSContextState *, const char *)
{
	return ps_param_policy_ignore;
}

int  XWPSCParamList::readNextKey(XWPSParamEnumerator * penum, PSParamString *key)
{
	PSCParam *pparam = (penum->pvoid ? ((PSCParam *) (penum->pvoid))->next :head);
  if (pparam == 0)
		return 1;
    
  penum->pvoid = pparam;
  *key = pparam->key;
  return 0;
}

int  XWPSCParamList::readSignalError(XWPSContextState *, const char *, int code)
{
	return code;
}

void XWPSCParamList::release()
{
	PSCParam *pparam = head;
	while (pparam != 0)
	{
		PSCParam *next = pparam->next;				
		delete pparam;		
		pparam = next;
		count--;
	}
	
	head = 0;
}

int XWPSCParamList::write(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue)
{
	PSParamCollectionType coll_typeA;
  switch (pvalue->type) 
  {
		case ps_param_type_dict:
	    coll_typeA = ps_param_collection_dict_any;
	    break;
	    
		case ps_param_type_dict_int_keys:
	    coll_typeA = ps_param_collection_dict_int_keys;
	    break;
	    
		case ps_param_type_array:
	    coll_typeA = ps_param_collection_array;
	    break;
	    
		default:
	    return write(pkey, &pvalue->value, pvalue->type);
  }
  
  return beginWrite(ctx, pkey, pvalue->value.d, coll_typeA);
}

int XWPSCParamList::write(const char * pkey, void *pvalue, PSParamType type)
{
//	unsigned top_level_sizeof = 0;
//  unsigned second_level_sizeof = 0;
  PSCParam *pparam = add(pkey);
  if (pparam == 0)
		return (int)(XWPSError::VMError);
  pparam->reset(type);
  switch (type)
  {
  	case ps_param_type_bool:
			pparam->value.b = *((bool*)pvalue);
			break;
						
		case ps_param_type_int:
			pparam->value.i =	*((int*)pvalue);
			break;
						
  	case ps_param_type_long:
  		pparam->value.l =	*((long*)pvalue);
			break;
						
  	case ps_param_type_float:
  		pparam->value.f = *((float*)pvalue);
			break;
						
  	case ps_param_type_string:
  		*(pparam->value.s) =	*((PSParamString*)pvalue);
  		break;
  					
  	case ps_param_type_name:
  		*(pparam->value.n) = *((PSParamString*)pvalue);
  		break;
  					
  	case ps_param_type_int_array:
  		*(pparam->value.ia) = *((PSParamIntArray*)pvalue);
  		break;
  					
  	case ps_param_type_float_array:
  		*(pparam->value.fa) = *((PSParamFloatArray*)pvalue);
  		break;
  					
  	case ps_param_type_string_array:
  		*(pparam->value.sa) = *((PSParamStringArray*)pvalue);
  		break;
  					
  	case ps_param_type_name_array:
  		*(pparam->value.na) = *((PSParamStringArray*)pvalue);
  		break;
  					
  	default:
  		break;
  }
  
  head = pparam;
  count++;
  return 0;
}

int XWPSCParamList::writeRequest(XWPSContextState * , const char *pkey)
{
	any_requested = true;
  if (find(pkey, true))
		return 0;
		
  PSCParam * pparam = add(pkey);
  if (pparam == 0)
		return (int)(XWPSError::VMError);
  pparam->type = (PSParamType)ps_param_type_any;
  head = pparam;
  return 0;
}

int XWPSCParamList::writeRequested(XWPSContextState * ctx, const char *pkey)
{
	if (!any_requested)
		return (target ? target->requested(ctx, pkey) : -1);
			
  if (find(pkey, true) != 0)
		return 1;
  
  if (!target)
		return 0;
    
  int code = target->requested(ctx, pkey);
  return (code < 0 ? 0 : 1);
}

XWPSIParamList::XWPSIParamList(QObject * parent)
	:XWPSParamList(parent)
{
	writing = false;
	policies.makeNull();
	wanted.makeNull();
	require_all = false;
	results = 0;
	count = 0;
	int_keys = false;
}

XWPSIParamList::~XWPSIParamList()
{
	if (results)
	{
		delete [] results;
		results = 0;
	}
}

int XWPSIParamList::beginTransmit(XWPSContextState * ctx, 
	                                const char * pkey, 
	                                PSParamCollection * pvalue, 
	                                PSParamCollectionType coll_typeA)
{
	if (writing)
		return beginWrite(ctx, pkey, pvalue, coll_typeA);
		
	return beginRead(ctx, pkey, pvalue, coll_typeA);
}

int XWPSIParamList::commit()
{
	if (writing)
		return -1;

  if (!require_all)
		return 0;
		
	int ecode = 0;
  for (uint i = 0; i < count; ++i)
		if (results[i] == 0)
	    results[i] = ecode = (int)(XWPSError::Undefined);
  return ecode;
}

int XWPSIParamList::endTransmit(XWPSContextState * ctx, const char * pkey, PSParamCollection * pvalue)
{
	if (writing)
		return endWrite(ctx, pkey, pvalue);
	
	return endRead(ctx, pkey, pvalue);
}

int XWPSIParamList::getPolicy(XWPSContextState * ctx, const char * pkey)
{
	if (writing)
		return -1;
		
	XWPSRef *pvalue;
  if (!(policies.hasType(XWPSRef::Dictionary) &&
	      policies.dictFindString(ctx, pkey, &pvalue) > 0 &&
	      pvalue->hasType(XWPSRef::Integer)))
	{
		return ps_param_policy_ignore;
	}
  return (int)pvalue->getInt();
}

int XWPSIParamList::nextKey(XWPSContextState *ctx, XWPSParamEnumerator * penum, PSParamString *key)
{
	XWPSRef::RefType keytype;
  return enumerate(ctx, penum, key, &keytype);
}

int XWPSIParamList::request(XWPSContextState * , const char *)
{
//	if (writing)
		return -1;
}

int XWPSIParamList::requested(XWPSContextState * ctx, const char *pkey)
{
	if (writing)
		return refParamRequested(ctx, pkey);
		
	return -1;
}

int XWPSIParamList::signalError(XWPSContextState * ctx, const char * pkey, int code)
{
	if (writing)
		return -1;
		
	PSIParamLoc loc;
  refParamRead(ctx, pkey, &loc, -1);
  *loc.presult = code;
  switch (getPolicy(ctx, pkey)) 
  {
		case ps_param_policy_ignore:
	    return 0;
	    
		case ps_param_policy_consult_user:
	     return (int)(XWPSError::ConfigurationError);
	     	
		default:
	    return code;
  }
}

int XWPSIParamList::transmit(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue)
{
	if (writing)
		return write(ctx, pkey, pvalue);
		
	return read(ctx, pkey, pvalue);
}

int XWPSIParamList::beginRead(XWPSContextState * ctx, 
	                            const char * pkey, 
	                            PSParamCollection * pvalue, 
	                            PSParamCollectionType coll_typeA)
{
	PSIParamLoc loc;
  bool int_keys = coll_typeA != 0;
  int code = refParamRead(ctx, pkey, &loc, -1);
  if (code != 0)
		return code;
		
	XWPSDictParamList *dlist = 0;
	if (loc.pvalue->hasType(XWPSRef::Dictionary)) 
	{
		dlist = new XWPSDictParamList(false, loc.pvalue, NULL, false);
		dlist->int_keys = int_keys;
		pvalue->size = loc.pvalue->dictLength();
		dlist->writing = false;
  } 
  else if (int_keys && loc.pvalue->isArray()) 
  {
		dlist = new XWPSDictParamList(true, loc.pvalue, NULL, false);
		dlist->int_keys = int_keys;
		pvalue->size = loc.pvalue->size();
		dlist->writing = false;
  } 
  else
		code = (int)(XWPSError::TypeCheck);
			
  pvalue->list = dlist;
  pvalue->persistent = false;
  return 0;
}

int XWPSIParamList::beginWrite(XWPSContextState * ctx, 
	                             const char * , 
	                             PSParamCollection * pvalue, 
	                             PSParamCollectionType coll_typeA)
{
	XWPSDictParamList * dlist = 0;			
	int code = 0;
	if (coll_typeA != ps_param_collection_array)
	{
		XWPSRef dref;
		dref.dictAlloc(ctx->currentSpace(), pvalue->size);
		dlist = new XWPSDictParamList(false, &dref, (XWPSRef*)0, (QObject*)0);
	  dlist->int_keys = coll_typeA == ps_param_collection_dict_int_keys;
	}
	else
	{
		XWPSRef aref;
		aref.makeArray(PS_A_ALL, pvalue->size);
		dlist = new XWPSDictParamList(true, &aref, (XWPSRef*)0, (QObject*)0);
	}
	
	dlist->writing = true;
	pvalue->list = dlist;
	pvalue->persistent = false;
  return code;
}

int XWPSIParamList::endRead(XWPSContextState * , const char * , PSParamCollection * )
{
	return 0;
}

int XWPSIParamList::endWrite(XWPSContextState * ctx, const char * pkey, PSParamCollection * pvalue)
{
	return refParamWrite(ctx, pkey, ((XWPSDictParamList *) pvalue->list)->getDict());
}

int XWPSIParamList::read(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue)
{
	PSIParamLoc loc;  
  int code = refParamRead(ctx, pkey, &loc, -1);
  if (code != 0)
		return code;
    
  XWPSRef elt;
  switch (loc.pvalue->type()) 
  {
		case XWPSRef::Array:
		case XWPSRef::MixedArray:
		case XWPSRef::ShortArray:
			if (!loc.pvalue->hasAttr(PS_A_READ))
				return (int)(XWPSError::TypeCheck);
	    if (loc.pvalue->size() <= 0) 
				return 0;
	    loc.pvalue->arrayGet(ctx, 0, &elt);
	    switch (elt.type()) 
	    {
				case XWPSRef::Integer:
		    	pvalue->reset(ps_param_type_int_array);
		    	code = refParamReadIntArray(ctx, pkey, pvalue->value.ia);
		    	if (code != XWPSError::TypeCheck)
						return code;
		    	*loc.presult = 0;
		    	
				case XWPSRef::Real:
		    	pvalue->reset(ps_param_type_float_array);
		    	return refParamReadFloatArray(ctx, pkey, pvalue->value.fa);
		    	
				case XWPSRef::String:
		    	pvalue->reset(ps_param_type_string_array);
		    	return refParamReadStringArray(ctx, pkey, pvalue->value.sa);
		    	
				case XWPSRef::Name:
		    	pvalue->reset(ps_param_type_name_array);
		    	return refParamReadStringArray(ctx, pkey, pvalue->value.na);
		    	
				default:
		    	break;
	    }
	    return (int)(XWPSError::TypeCheck);
	    	
		case XWPSRef::Boolean:
	    pvalue->reset(ps_param_type_bool);
	    pvalue->value.b = loc.pvalue->getBool();
	    return 0;
	    
		case XWPSRef::Dictionary:
			pvalue->reset(ps_param_type_dict);
	    code = beginRead(ctx, pkey, pvalue->value.d, ps_param_collection_dict_any);
	    if (code < 0)
				return code;
	    {
				XWPSParamEnumerator enumr;
				PSParamString key;
				XWPSRef::RefType keytype;
				XWPSIParamList * ilist = (XWPSIParamList*)(pvalue->value.d->list);
				if (!ilist->enumerate(ctx, &enumr, &key, &keytype) && keytype == XWPSRef::Integer) 
				{
		    	((XWPSDictParamList *) pvalue->value.d->list)->int_keys = 1;
		    	pvalue->type = ps_param_type_dict_int_keys;
				}
	    }
	    return 0;
	    
		case XWPSRef::Integer:
	    pvalue->reset(ps_param_type_long);
	    pvalue->value.l = loc.pvalue->getInt();
	    return 0;
	    
		case XWPSRef::Name:
	    pvalue->reset(ps_param_type_name);
	    return refParamReadStringValue(ctx, &loc, pvalue->value.n);
	    
		case XWPSRef::Null:
	    pvalue->reset(ps_param_type_null);
	    return 0;
	    
		case XWPSRef::Real:
			pvalue->reset(ps_param_type_float);
	    pvalue->value.f = loc.pvalue->getReal();
	    return 0;
	    
		case XWPSRef::String:
	    pvalue->reset(ps_param_type_string);
	    return refParamReadStringValue(ctx, &loc, pvalue->value.s);
	    
		default:
	    break;
  }
  return (int)(XWPSError::TypeCheck);
}

int XWPSIParamList::readInit(uint countA, XWPSRef * ppolicies, bool require_allA)
{
	if (ppolicies != 0)
		policies.assign(ppolicies);		
	require_all = require_allA;
	count = countA;
	if (count > 0)
	{
		results = new int[count];
		memset(results, 0, count * sizeof(int));
	}
	
	return 0;
}

int XWPSIParamList::refArrayParamRequested(XWPSContextState * ctx, 
	                                         const char * pkey,
			                                     XWPSRef *pvalue, 
			                                     uint size)
{
	if (!refParamRequested(ctx, pkey))
		return 0;
		
	pvalue->makeArray(PS_A_ALL, size);
	return 1;
}

int XWPSIParamList::refParamKey(XWPSContextState * ctx, const char * pkey, XWPSRef * pkref)
{
	if (int_keys) 
	{
		long key = 0;
		if (sscanf(pkey, "%ld", &key) != 1)
	    return (int)(XWPSError::RangeCheck);
		pkref->makeInt(key);
		return 0;
  } 
  else
		return ctx->nameRef((const uchar *)pkey, strlen(pkey), pkref, 0);
}

int XWPSIParamList::refParamRead(XWPSContextState * ctx, 
	                               const char * pkey, 
	                               PSIParamLoc * ploc,
	                               int type)
{
	XWPSRef kref;
  int code = refParamKey(ctx, pkey, &kref);
  if (code < 0)
		return code;
    
  code = read(ctx, &kref, ploc);
  if (code != 0)
		return code;
  if (type >= 0)
  {
  	if (!ploc->pvalue->hasType(type))
  		return (int)(XWPSError::TypeCheck);
  }
  
  return 0;
}

int XWPSIParamList::refParamReadArray(XWPSContextState * ctx, const char * pkey, PSIParamLoc * ploc)
{
	int code = refParamRead(ctx, pkey, ploc, -1);
  if (code != 0)
		return code;
		
  if (!ploc->pvalue->isArray())
		return (int)(XWPSError::TypeCheck);
  
  if (!ploc->pvalue->hasAttr(PS_A_READ))
		return (int)(XWPSError::TypeCheck);
  return 0;
}

int XWPSIParamList::refParamReadFloatArray(XWPSContextState * ctx, const char * pkey, PSParamFloatArray * pvalue)
{
	PSIParamLoc loc;
	int code = refParamReadArray(ctx, pkey, &loc);
	if (code != 0)
		return code;
		
	uint size = loc.pvalue->size();
  float * pfv = 0;
  if (size > 0)
    pfv = new float[size];
  if (pfv == 0)
		return (int)(XWPSError::VMError);
			
	XWPSRef aref, elt;
	aref.assign(loc.pvalue);
	loc.pvalue = &elt;
	for (uint i = 0; code >= 0 && i < size; i++) 
	{
		aref.arrayGet(ctx, i, &elt);
		code = elt.floatParam(pfv + i);
  }
  
  if (code < 0) 
  {
		delete [] pfv;
		return (*loc.presult = code);
  }
  pvalue->data = pfv;
  pvalue->size = size;
  pvalue->persistent = false;
  return 0;
}

int XWPSIParamList::refParamReadIntArray(XWPSContextState * ctx, const char * pkey, PSParamIntArray * pvalue)
{
	PSIParamLoc loc;
	int code = refParamReadArray(ctx, pkey, &loc);
	if (code != 0)
		return code;
		
  uint size = loc.pvalue->size();
  int * piv = 0;
  if (size > 0)
    piv = new int[size];
  if (piv == 0)
		return (int)(XWPSError::VMError);
    
  for (uint i = 0; i < size; i++) 
  {
		XWPSRef elt;
		loc.pvalue->arrayGet(ctx, i, &elt);
		if (!elt.hasType(XWPSRef::Integer)) 
		{
	    code = (int)(XWPSError::TypeCheck);
	    break;
		}
		
		piv[i] = (int)elt.getInt();
  }
  if (code < 0) 
  {
		delete [] piv;
		return (*loc.presult = code);
  }
  pvalue->data = piv;
  pvalue->size = size;
  pvalue->persistent = false;
  return 0;
}

int XWPSIParamList::refParamReadStringArray(XWPSContextState * ctx, const char * pkey, PSParamStringArray * pvalue)
{
	PSIParamLoc loc;
	int code = refParamReadArray(ctx, pkey, &loc);
	if (code != 0)
		return code;
		
	uint size = loc.pvalue->size();
  PSParamString * psv = 0;
  if (size > 0)
    psv = new PSParamString[size];
  if (psv == 0)
		return (int)(XWPSError::VMError);
			
	XWPSRef aref;
	aref.assign(loc.pvalue);
	if (aref.hasType(XWPSRef::Array)) 
	{
		for (uint i = 0; code >= 0 && i < size; i++) 
		{
	    loc.pvalue = aref.getArray() + i;
	    code = refParamReadStringValue(ctx, &loc, psv + i);
		}
  }
  else
  {
  	XWPSRef elt;
		loc.pvalue = &elt;
		for (uint i = 0; code >= 0 && i < size; i++) 
		{
	    aref.arrayGet(ctx, i, &elt);
	    code = refParamReadStringValue(ctx, &loc, psv + i);	    
		}
  }
  
  if (code < 0)
	{
	 	delete psv;
	 	return (*loc.presult = code);
	}
	
	pvalue->data = psv;
  pvalue->size = size;
  pvalue->persistent = false;
  return 0;
}

int XWPSIParamList::refParamReadStringValue(XWPSContextState * ctx, PSIParamLoc * ploc, PSParamString * pvalue)
{
	XWPSRef *pref = ploc->pvalue;
  switch (pref->type()) 
  {
		case XWPSRef::Name: 
			{
	    	XWPSRef nref;
	      ctx->nameStringRef(pref, &nref);
	      pvalue->size = nref.size();
	    	pvalue->persistent = true;
	    	pvalue->data = nref.getBytes();
			}
	    break;
	    
		case XWPSRef::String:
	    if (!ploc->pvalue->hasAttr(PS_A_READ))
				return (int)(XWPSError::TypeCheck);
	    
	    pvalue->size = pref->size();
	    if (pref->size() > 0)
	    {
	    	pvalue->persistent = false;
	    	pvalue->data = new uchar[pvalue->size];
	    	memcpy(pvalue->data, pref->getBytes(), pvalue->size);
	    }
	    break;
	    
		default:
	    return (int)(XWPSError::TypeCheck);
  }
  return 0;
}

int XWPSIParamList::refParamRequested(XWPSContextState * ctx, const char *pkey)
{
	return writeRequested(ctx, pkey);
}

int XWPSIParamList::refParamWrite(XWPSContextState * ctx, const char * pkey, XWPSRef * pvalue)
{	
	if (!refParamRequested(ctx, pkey))
		return 0;
		
	XWPSRef kref;
  int code = refParamKey(ctx, pkey, &kref);
  if (code < 0)
		return code;
    
  return write(ctx, &kref, pvalue);
}

int XWPSIParamList::refParamWriteNameValue(XWPSContextState * ctx, XWPSRef * pref, PSParamString * pvalue)
{
	return ctx->nameRef(pvalue->data, pvalue->size, pref, (pvalue->persistent ? 0 : 1));
}

int XWPSIParamList::refParamWriteStringValue(XWPSContextState * ctx, XWPSRef * pref, PSParamString * pvalue)
{
	pref->makeString(ctx->currentSpace() | PS_A_READONLY, pvalue->size, pvalue->data);
	return 0;
}

int XWPSIParamList::refParamWriteTypedArray(XWPSContextState * ctx, 
	                                          const char * pkey, 
	                                          void *pvalue, 
	                                          uint countA,
	                                          PSParamType t)
{
	XWPSRef value;
	int code = refArrayParamRequested(ctx, pkey, &value, countA);
	if (code <= 0)
		return code;
		
	XWPSRef *pe = value.getArray();
	PSParamIntArray * ia;
	PSParamFloatArray * fa;
	PSParamStringArray * sa;
	for (uint i = 0; i < countA; ++i, ++pe)
	{
		switch (t)
		{
			case ps_param_type_int_array:
				ia = (PSParamIntArray*)pvalue;
				pe->makeInt(ia->data[i]);
				break;
				
			case ps_param_type_float_array:
				fa = (PSParamFloatArray*)pvalue;
				pe->makeReal(fa->data[i]);
				break;
				
			case ps_param_type_string_array:
				sa = (PSParamStringArray*)pvalue;
				refParamWriteStringValue(ctx, pe, &sa->data[i]);
				break;
				
			case ps_param_type_name_array:
				sa = (PSParamStringArray*)pvalue;
				refParamWriteNameValue(ctx, pe, &sa->data[i]);
				break;
				
			default:
				break;
		}
	}
	
	return refParamWrite(ctx, pkey, &value);
}

int XWPSIParamList::refToKey(XWPSContextState * ctx, XWPSRef * pref, PSParamString * key)
{
	if (!key->persistent && key->data)
		delete [] key->data;
	key->data = 0;
	key->size = 0;
	key->persistent = false;
	if (pref->hasType(XWPSRef::Name)) 
	{
		XWPSRef nref;
		ctx->nameStringRef(pref, &nref);
		key->persistent = true;
		key->size = nref.size();
		key->data = nref.getBytes();
  } 
  else if (pref->hasType(XWPSRef::Integer)) 
  {
		char istr[sizeof(long) * 8 / 3 + 2];
		int len = sprintf(istr, "%ld", pref->getInt());
		uchar * buf = new uchar[len];
		key->data = buf;
		key->size = len;
		memcpy((void*)buf, (void*)istr, len * sizeof(uchar));
    
  } 
  else
  {
		return (int)(XWPSError::TypeCheck);
	}
 	
 	return 0;
}

int XWPSIParamList::write(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue)
{
	XWPSRef value;
  int code = 0;
  switch (pvalue->type) 
  {
		case ps_param_type_null:
	    value.makeNull();
	    break;
	    
		case ps_param_type_bool:
	    value.makeBool(pvalue->value.b);
	    break;
	    
		case ps_param_type_int:
	    value.makeInt(pvalue->value.i);
	    break;
	    
		case ps_param_type_long:
	    value.makeInt(pvalue->value.l);
	    break;
	    
		case ps_param_type_float:
	    value.makeReal(pvalue->value.f);
	    break;
	    
		case ps_param_type_string:
	    if (!requested(ctx, pkey))
				return 0;
	    code = refParamWriteStringValue(ctx, &value, pvalue->value.s);
	    break;
	    
		case ps_param_type_name:
	    if (!requested(ctx, pkey))
				return 0;
	    code = refParamWriteNameValue(ctx, &value, pvalue->value.n);
	    break;
	    
		case ps_param_type_int_array:
	    return refParamWriteTypedArray(ctx, pkey, 
	                                   pvalue->value.ia, 
	                                   pvalue->value.ia->size, 
	                                   ps_param_type_int_array);
					       
		case ps_param_type_float_array:
	    return refParamWriteTypedArray(ctx, pkey, 
	                                   pvalue->value.fa, 
	                                   pvalue->value.fa->size, 
	                                   ps_param_type_float_array);
	    
		case ps_param_type_string_array:
	    return refParamWriteTypedArray(ctx, pkey, 
	                                   pvalue->value.sa,
					                           pvalue->value.sa->size,
					                           ps_param_type_string_array);
					                           
		case ps_param_type_name_array:
	    return refParamWriteTypedArray(ctx, pkey, 
	                                   pvalue->value.na,
					                           pvalue->value.na->size,
					                           ps_param_type_name_array);
					                           
		case ps_param_type_dict:
		case ps_param_type_dict_int_keys:
		case ps_param_type_array:
	    return beginWrite(ctx, pkey, pvalue->value.d, (PSParamCollectionType)(pvalue->type - ps_param_type_dict));
	    
		default:
	    return (int)(XWPSError::TypeCheck);
  }
  if (code < 0)
		return code;
  return refParamWrite(ctx, pkey, &value);
}

int XWPSIParamList::writeInit(XWPSRef * pwanted)
{
	if (pwanted == 0)
		wanted.makeNull();
	else
		wanted.assign(pwanted);
		
	return 0;
}

int XWPSIParamList::writeRequested(XWPSContextState * ctx, const char *pkey)
{
  if (!wanted.hasType(XWPSRef::Dictionary))
		return -1;
		
	XWPSRef kref;
  if (refParamKey(ctx, pkey, &kref) < 0)
		return -1;
		
	XWPSRef *ignore_value;
  return (wanted.dictFind(ctx, &kref, &ignore_value) > 0);
}

XWPSDictParamList::XWPSDictParamList(bool a,
	                                   XWPSRef * pdict, 
	                                   XWPSRef * pwanted, 
	                                   QObject * parent)
	:XWPSIParamList(parent),
	 array(a)
{
	dict.assign(pdict);
	if (a)
	{
		dict.checkArray();
		dict.checkWrite();
		int_keys = true;
	}
	else			
		dict.checkDictWrite();		
	
	writeInit(pwanted);
}

XWPSDictParamList::XWPSDictParamList(bool a,
	                                   XWPSRef * pdict,
	                                   XWPSRef * ppolicies,
	                                   bool require_allA,
	                                   QObject * parent)
	:XWPSIParamList(parent),
	 array(a)
{
	if (!a)
	{
		uint c = 0;
		if (pdict == 0)
			dict.makeNull();
		else
		{
			dict.assign(pdict);
			dict.checkDictRead();
			c = dict.dictMaxIndex() + 1;
		}
		readInit(c, ppolicies, require_allA);
	}
	else
	{
		if (pdict == 0)
			dict.makeNull();
		else
		{
			dict.assign(pdict);
			dict.checkReadType(XWPSRef::Array);
		}
		
		readInit(dict.size(), ppolicies, require_allA);
    int_keys = true;
	}
}

int XWPSDictParamList::enumerate(XWPSContextState * ctx, 
	                               XWPSParamEnumerator * penum,
		                             PSParamString * key, 
		                             XWPSRef::RefType * type)
{
	if (!array)
	{
		XWPSRef elt[2];
  	int index = (penum->intval != 0 ? penum->intval : dict.dictFirst());
  	index = dict.dictNext(ctx, index, elt);
  	if (index < 0)
			return 1;
    
  	*type = elt[1].type();
  	int code = refToKey(ctx, &elt[0], key);
  	penum->intval = index;
  	return code;
  }
  
  return -1;
}

int XWPSDictParamList::read(XWPSContextState * ctx, XWPSRef * pkey, PSIParamLoc * ploc)
{
	if (!array)
	{
		if (dict.type() == XWPSRef::Null)
			return 1;
		
		int code = dict.dictFind(ctx, pkey, &ploc->pvalue);
  	if (code != 1)
			return 1;
		
		ploc->presult =	&results[dict.dictValueIndex(ploc->pvalue)];
  	*ploc->presult = 1;
  	return 0;
  }
  else
  {
  	pkey->checkType(XWPSRef::Integer);
    if (pkey->getInt() < 0 || pkey->getInt() >= dict.size())
			return 1;
			
    ploc->pvalue = dict.getArray() + pkey->getInt();
    ploc->presult = &results[pkey->getInt()];
    *ploc->presult = 1;
    return 0;
  }
}

int XWPSDictParamList::write(XWPSContextState * ctx, XWPSRef * pkey, XWPSRef * pvalue)
{
	if (!array)
	{
		int code = dict.dictPut(ctx, pkey, pvalue, NULL);
  	return qMin(code, 0);
  }
  else
  {
  	if (!pkey->hasType(XWPSRef::Integer))
			return (int)(XWPSError::TypeCheck);
    pkey->checkIntLTU(dict.size());
    dict.storeCheckDest(pvalue);
    XWPSRef * eltp = dict.getArray() + pkey->getInt();
    eltp->assign(pvalue);
    return 0;
  }
}

XWPSArrayParamList::XWPSArrayParamList(XWPSRef * botA, 
	                                     uint countA,
		                                   XWPSRef * ppolicies, 
		                                   bool require_allA, 
		                                   QObject * parent)
	:XWPSIParamList(parent),
	 bot(botA)	 
{
	top = bot + countA;
	 readInit(countA, ppolicies, require_allA);
}

int XWPSArrayParamList::enumerate(XWPSContextState * ctx, 
	                                XWPSParamEnumerator * penum,
		                              PSParamString * key, 
		                              XWPSRef::RefType * type)
{
	int index = penum->intval;
  XWPSRef *ptr = bot + index;
  for (; ptr < top; ptr += 2) 
  {
		index += 2;
		if (ptr->hasType(XWPSRef::Name)) 
		{
	    int code = refToKey(ctx, ptr, key);
	    *type = ptr->type();
	    penum->intval = index;
	    return code;
		}
  }
  return 1;
}

int XWPSArrayParamList::read(XWPSContextState * , XWPSRef * pkey, PSIParamLoc * ploc)
{
	XWPSRef *ptr = bot;
  for (; ptr < top; ptr += 2) 
  {
		if (ptr->hasType(XWPSRef::Name) && XWPSContextState::nameEq(ptr, pkey)) 
		{
	    ploc->pvalue = ptr + 1;
	    ploc->presult = &results[ptr - bot];
	    *ploc->presult = 1;
	    return 0;
		}
  }
  return 1;
}

XWPSStackParamList::XWPSStackParamList(XWPSRefStack * stack, 
	                                     XWPSRef * pwanted,
	                                     QObject * parent)
	:XWPSIParamList(parent),
	 pstack(stack),
	 skip(0)
{
	writeInit(pwanted);
}

XWPSStackParamList::XWPSStackParamList(XWPSRefStack * stack,
	                    uint skipA, 
	                    XWPSRef * ppolicies, 
	                    bool require_allA,	                   
	                   QObject * parent)
	:XWPSIParamList(parent),
	 pstack(stack),
	 skip(skipA)
{
	uint c = pstack->countToMark();
	c -= skip + 1;
	readInit(c >> 1, ppolicies, require_allA);
}

int XWPSStackParamList::enumerate(XWPSContextState * ctx, 
	                                XWPSParamEnumerator * penum,
		                              PSParamString * key, 
		                              XWPSRef::RefType * type)
{
	long index = penum->intval;
  XWPSRef *stack_element;

  do 
  {
		stack_element = pstack->index(index + 1 + skip);
		if (!stack_element)
	    return 1;
  } while (index += 2, !stack_element->hasType(XWPSRef::Name));
  *type = stack_element->type();
  int code = refToKey(ctx, stack_element, key);
  penum->intval = index;
  return code;
}

int XWPSStackParamList::read(XWPSContextState * , XWPSRef * pkey, PSIParamLoc * ploc)
{
	uint index = skip + 1;
  uint c = count;
  for (; c; c--, index += 2) 
  {
		XWPSRef *p = pstack->index(index);
		if (p && p->hasType(XWPSRef::Name) && XWPSContextState::nameEq(p, pkey)) 
		{
	    ploc->pvalue = pstack->index(index - 1);
	    ploc->presult = &results[c - 1];
	    *ploc->presult = 1;
	    return 0;
		}
  }
  return 1;
}

int XWPSStackParamList::write(XWPSContextState * , XWPSRef * pkey, XWPSRef * pvalue)
{
	XWPSRef * p = pstack->getCurrentTop();
  if (pstack->getTop() - p < 2) 
  {
		int code = pstack->push(2);
		if (code < 0)
	    return code;
	    
		pstack->index(1)->assign(pkey);
		p = pstack->getCurrentTop();
  } 
  else 
  {
  	p = pstack->incCurrentTop(2);
		p[-1].assign(pkey);
  }
  p->assign(pvalue);
  count++;
  return 0;
}

XWPSPrinterParamList::XWPSPrinterParamList(XWPSStream * s,
	                                         PSPrinterParams * ppp,
	                                         QObject * parent)
	:XWPSParamList(parent)
{
	strm = s;
  params = *ppp;
  any = false;
}

XWPSPrinterParamList::~XWPSPrinterParamList()
{
}

void XWPSPrinterParamList::release()
{
	if (any && params.suffix)
	    strm->pputs(params.suffix);
}

int XWPSPrinterParamList::transmit(XWPSContextState * , const char * pkey, PSParamTypedValue * pvalue)
{
	if (!any) 
	{
		if (params.prefix)
	    strm->pputs(params.prefix);
		any = true;
 	}
 	
 	if (params.item_prefix)
		strm->pputs(params.item_prefix);
		
	strm->print("/%s", pkey);
  switch (pvalue->type) 
  {
		case ps_param_type_null:
	    strm->pputs(" null");
	    break;
		case ps_param_type_bool:
	    strm->pputs((pvalue->value.b ? " true" : " false"));
	    break;
		case ps_param_type_int:
	    strm->print( " %d", pvalue->value.i);
	    break;
		case ps_param_type_long:
	    strm->print(" %l", pvalue->value.l);
	    break;
		case ps_param_type_float:
	    strm->print(" %g", pvalue->value.f);
	    break;
		case ps_param_type_string:
	    strm->writePSString(pvalue->value.s->data, pvalue->value.s->size, params.print_ok);
	    break;
		case ps_param_type_name:
	    strm->pputc('/');
	    strm->write(pvalue->value.n->data, pvalue->value.n->size);
	    break;
		case ps_param_type_int_array:
	    {
				char sepr = (pvalue->value.ia->size <= 10 ? ' ' : '\n');

				strm->pputc('[');
				for (uint i = 0; i < pvalue->value.ia->size; ++i) 
				{
		    	strm->print("%d", pvalue->value.ia->data[i]);
		    	strm->pputc(sepr);
				}
				strm->pputc(']');
	    }
	    break;
		case ps_param_type_float_array:
	    {
				char sepr = (pvalue->value.fa->size <= 10 ? ' ' : '\n');
				strm->pputc('[');
				for (uint i = 0; i < pvalue->value.fa->size; ++i) 
				{
		    	strm->print("%g", pvalue->value.fa->data[i]);
		    	strm->pputc(sepr);
				}
				strm->pputc(']');
	    }
	    break;
	    
		default:
	    return (int)(XWPSError::TypeCheck);
  }
  if (params.item_suffix)
		strm->pputs(params.item_suffix);
  return 0;
}

XWPSCosParamListWriter::XWPSCosParamListWriter(XWPSCosObject *pcdA,
				                                       int print_okA,
				                                       QObject * parent)
	:XWPSParamList(parent),
	 pcd(pcdA),
	 print_ok(print_okA)
{
}

XWPSCosParamListWriter::~XWPSCosParamListWriter()
{
}

int XWPSCosParamListWriter::transmit(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue)
{
	int key_len = strlen(pkey);	
	if (key_len > 99)
		return (int)(XWPSError::LimitCheck);
			
	XWPSCosValue value;
	switch (pvalue->type)
	{
		default:
			{
				PSPrinterParams ppp;
				ppp.prefix = 0;
				ppp.suffix = 0;
				ppp.item_prefix = 0;
				ppp.item_suffix = 0;
				ppp.print_ok = print_ok;
				
				XWPSStream s;
				
				XWPSPrinterParamList pplist(&s, &ppp);
				s.writePositionOnly();
				pplist.transmit(ctx, "", pvalue);
				long len = s.tell();
				uchar * str = (uchar*)malloc((len+1) * sizeof(uchar));
				if (str == 0)
	    		return (int)(XWPSError::VMError);
	    			
	    	s.writeString(str, len, false);
	    	XWPSPrinterParamList ppplist(&s, &ppp);
	    	ppplist.transmit(ctx, "", pvalue);
	    	long skip = (str[1] == ' ' ? 2 : 1);
				memmove(str, str + skip, len - skip);
				str = (uchar*)realloc(str, (len - skip)+1);
				XWPSCosValue tempv(str, len - skip);
				value.copy(&tempv);				
				free(str);
			}
			break;
			
		case ps_param_type_int_array:
			{
				XWPSCosObject * pca = new XWPSCosObject(pcd->pdev, (PSCosObjectProcs*)&cos_array_procs);
	    	for (int i = 0; i < pvalue->value.ia->size; ++i)
	    		pca->arrayAdd(pvalue->value.ia->data[i]);
	    		
	    	XWPSCosValue tempv(pca);
				value.copy(&tempv);
			}
			break;
			
		case ps_param_type_float_array:
			{
				XWPSCosObject * pca = new XWPSCosObject(pcd->pdev, (PSCosObjectProcs*)&cos_array_procs);
	    	for (int i = 0; i < pvalue->value.fa->size; ++i)
	    		pca->arrayAdd(pvalue->value.fa->data[i]);
	    		
	    	XWPSCosValue tempv(pca);
				value.copy(&tempv);
			}
			break;
			
		case ps_param_type_string_array:
    case ps_param_type_name_array:
    	return (int)(XWPSError::TypeCheck);
	}
	uchar key_chars[100];
	memcpy(key_chars + 1, pkey, key_len);
  key_chars[0] = '/';
  return pcd->dictPut(key_chars, key_len + 1, &value);
}

XWPSObjectParamListWriter::XWPSObjectParamListWriter(XWObject *pcdA,
				                 int print_okA,
				                 QObject * parent)
	:XWPSParamList(parent),
	 pcd(pcdA),
	 print_ok(print_okA)
{
}

XWPSObjectParamListWriter::~XWPSObjectParamListWriter()
{
}

int XWPSObjectParamListWriter::transmit(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue)
{
	int key_len = strlen(pkey);	
	if (key_len > 99)
		return (int)(XWPSError::LimitCheck);
			
	XWObject obj;
	switch (pvalue->type)
	{
		default:
			{
				PSPrinterParams ppp;
				ppp.prefix = 0;
				ppp.suffix = 0;
				ppp.item_prefix = 0;
				ppp.item_suffix = 0;
				ppp.print_ok = print_ok;
				
				XWPSStream s;
				
				XWPSPrinterParamList pplist(&s, &ppp);
				s.writePositionOnly();
				pplist.transmit(ctx, "", pvalue);
				long len = s.tell();
				uchar * str = (uchar*)malloc((len+1) * sizeof(uchar));
				if (str == 0)
	    		return (int)(XWPSError::VMError);
	    			
	    	s.writeString(str, len, false);
	    	XWPSPrinterParamList ppplist(&s, &ppp);
	    	ppplist.transmit(ctx, "", pvalue);
	    	long skip = (str[1] == ' ' ? 2 : 1);
				memmove(str, str + skip, len - skip);
				str = (uchar*)realloc(str, (len - skip)+1);
				obj.initString(new XWString((char *)str, len - skip));
				free(str);
			}
			break;
			
		case ps_param_type_int_array:
			{	    			
	    	obj.initArray(0);	    	
	    	for (int i = 0; i < pvalue->value.ia->size; ++i)
	    	{
	    		XWObject v;
	    		v.initInt(pvalue->value.ia->data[i]);
	    		obj.arrayAdd(&v);
	    	}
			}
			break;
			
		case ps_param_type_float_array:
			{
				obj.initArray(0);	
	    	for (int i = 0; i < pvalue->value.fa->size; ++i)
	    	{
	    		XWObject v;
	    		v.initReal(pvalue->value.fa->data[i]);
	    		obj.arrayAdd(&v);
	    	}
			}
			break;
			
		case ps_param_type_string_array:
    case ps_param_type_name_array:
    	return (int)(XWPSError::TypeCheck);
	}
	
  pcd->dictAdd(qstrdup(pkey), &obj);
  return 0;
}
