/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSParam.h"
#include "XWPSStream.h"
#include "XWPSColor.h"
#include "XWPSColorSpace.h"
#include "XWPSPath.h"
#include "XWPSDevicePsdf.h"

typedef int (*ss_put_params_t)(XWPSContextState*, XWPSParamList *, PSStreamState *, bool);

static double round_byte_color(int cv)
{
	return (int)(cv * (1000.0 / 255.0) + 0.5) / 1000.0;
}

static bool coord_between(long start, long mid, long end)
{
    return (start <= end ? start <= mid && mid <= end : start >= mid && mid >= end);
}

static int
psdf_write_name(XWPSContextState * ctx, XWPSParamList *plist, const char *key, const char *str)
{
    PSParamString pstr;

    pstr.fromString(str);
    return plist->writeName(ctx, key, &pstr);
}

static int
psdf_write_string_param(XWPSContextState * ctx, XWPSParamList *plist, const char *key,
			const XWPSString *pstr)
{
    PSParamString ps;

    ps.data = pstr->data;
    ps.size = pstr->size;
    return plist->writeString(ctx, key, &ps);
}

static int
psdf_get_image_dict_param(XWPSContextState * ctx, XWPSParamList * plist, const char* pname,
			  XWPSCParamList *plvalue)
{
    PSParamCollection dict;
    int code;

    if (pname == 0)
			return 0;
    dict.size = 12;	
    
    if ((code = plist->beginTransmit(ctx, pname, &dict, ps_param_collection_dict_any)) < 0)
			return code;
    if (plvalue != 0) 
    {
			plvalue->read();
			code = dict.list->copy(ctx, plvalue);
    }
    plist->endTransmit(ctx, pname, &dict);
    return code;
}

static int
psdf_get_image_params(XWPSContextState * ctx, XWPSParamList * plist,
	  const PSPSDFImageParamNames * pnames, XWPSPSDFImageParams * params)
{
    PSParamItem *items =	(PSParamItem*)((pnames->items[0].key == 0 ? pnames->items + 1 : pnames->items));
    int code;
    if ( (code = plist->writeItems(ctx, params, NULL, items)) < 0 ||
	   		(code = psdf_get_image_dict_param(ctx, plist, pnames->ACSDict, params->ACSDict)) < 0 ||
	   		(code = psdf_get_image_dict_param(ctx, plist, pnames->Dict, params->Dict)) < 0 ||
	   		(code = psdf_write_name(ctx, plist, pnames->DownsampleType,	DownsampleType_names[params->DownsampleType])) < 0 ||
	   		(code = psdf_write_name(ctx, plist, pnames->Filter,  (params->Filter == 0 ?    pnames->filter_names[0].pname :
				    params->Filter))) < 0)
			return code;
    return code;
}

static int
psdf_get_embed_param(XWPSContextState * ctx, XWPSParamList *plist, const char* allpname,
		     const PSParamStringArray *psa)
{
    int code = plist->writeNameArray(ctx, allpname, psa);

    if (code >= 0)
			code = plist->writeNameArray(ctx, allpname + 1, psa);
    return code;
}

static int
psdf_read_string_param(XWPSContextState * ctx, XWPSParamList *plist, const char *key,
		       XWPSString *pstr, int ecode)
{
    PSParamString ps;
    int code;

    switch (code = plist->readString(ctx, key, &ps)) 
    {
    	case 0: 
    		{
					uint size = ps.size;
					uchar *data = new uchar[size];
					memcpy(data, ps.data, size);
					pstr->data = data;
					pstr->size = size;
					break;
    		}
    		
    	default:
				ecode = code;
    	case 1:
				break;
    }
    return ecode;
}

static int
psdf_put_enum(XWPSContextState * ctx, XWPSParamList *plist, const char *key, int value,
	      const char *const pnames[], int *pecode)
{
    *pecode = plist->putEnum(ctx, key, &value, pnames, *pecode);
    return value;
}

static int
psdf_CF_put_params(XWPSContextState * ctx, XWPSParamList * plist, PSStreamState * st, bool)
{
    PSStreamCFEState *const ss = (PSStreamCFEState *) st;

    (*(s_CFE_template.set_defaults)) (st);
    ss->K = -1;
    ss->BlackIs1 = true;
    return plist->putParams(ctx, (PSStreamCFState *) ss);
}

static int
psdf_DCT_put_params(XWPSContextState * ctx, XWPSParamList * plist, PSStreamState * st, bool dss)
{
    return XWPSPSDFBinaryWriter::DCTFilter(ctx, plist, st, 8 , 8, 3 ,  NULL, dss);
}

static int
param_read_embed_array(XWPSContextState * ctx, XWPSParamList * plist, const char * pname,
		       PSParamStringArray * psa, int ecode)
{
    int code;

    switch (code = plist->readNameArray(ctx, pname, psa)) 
    {
			default:
	    	ecode = code;
	    	plist->signalError(ctx, pname, ecode);
			case 0:
			case 1:
	    	break;
    }
    return ecode;
}

static bool
param_string_eq(const PSParamString *ps1, const PSParamString *ps2)
{
    return !bytes_compare(ps1->data, ps1->size, ps2->data, ps2->size);
}

static int
add_embed(PSParamStringArray *prsa, const PSParamStringArray *psa)
{
    uint i;
    PSParamString *const rdata =(PSParamString *)prsa->data;
    uint count = prsa->size;

    for (i = 0; i < psa->size; ++i) 
    {
			uint j;

			for (j = 0; j < count; ++j)
	    	if (param_string_eq(&psa->data[i], &rdata[j]))
		    break;
			if (j == count) 
			{
	    	uint size = psa->data[i].size;
	    	uchar *data = new uchar[size];
	    	memcpy(data, psa->data[i].data, size);
	    	if (!rdata[count].persistent && rdata[count].data)
	    	{
	    		delete [] rdata[count].data;
	    	}
	    	rdata[count].data = data;
	    	rdata[count].size = size;
	    	rdata[count].persistent = false;
	    	count++;
			}
   }
   prsa->size = count;
   return 0;
}

static void
delete_embed(PSParamStringArray *prsa, const PSParamStringArray *pnsa)
{
    uint i;
    PSParamString *const rdata = (PSParamString *)prsa->data;
    uint count = prsa->size;

    for (i = pnsa->size; i-- > 0;) 
    {
			uint j;

			for (j = count; j-- > 0;)
	    	if (param_string_eq(&pnsa->data[i], &rdata[j]))
					break;
			if (j + 1 != 0) 
			{
				if (!rdata[j].persistent && rdata[j].data)
					delete [] rdata[j].data;
				rdata[j].data = 0;
				rdata[j].size = 0;
	    	rdata[j] = rdata[--count];
			}
    }
    prsa->size = count;
}

static int
psdf_put_embed_param(XWPSContextState * ctx, XWPSParamList * plist, const char * notpname,
		     const char * allpname, PSParamStringArray * psa,
		     int ecode)
{
    const char *  pname = notpname + 1;
    PSParamStringArray sa, nsa, asa;
    bool replace;
    PSParamString *rdata;
    PSParamStringArray rsa;
    int code = 0;

    ecode = param_read_embed_array(ctx, plist, pname, &sa, ecode);
    ecode = param_read_embed_array(ctx, plist, notpname, &nsa, ecode);
    ecode = param_read_embed_array(ctx, plist, allpname, &asa, ecode);
    if (ecode < 0)
			return ecode;
			
    if (asa.data == 0 || nsa.data != 0)
			replace = false;
    else if (sa.data == 0)
			replace = true;
    else if (sa.size != asa.size)
			replace = false;
    else 
    {
			uint i;

			replace = true;
			for (i = 0; i < sa.size; ++i)
	    	if (!param_string_eq(&sa.data[i], &asa.data[i])) 
	    	{
					replace = false;
					break;
	    	}
			if (replace)
	    	return 0;	
    }
    
    if (replace) 
    {
			rdata = new PSParamString[asa.size];
			rsa.data = rdata;
			rsa.persistent = false;
			if ((code = add_embed(&rsa, &asa)) < 0) 
	    	ecode = code;
			else
	    	delete_embed(psa, psa);
    } 
    else if (sa.data || nsa.data) 
    {
    	rsa = *psa;
			if ((code = add_embed(&rsa, &sa)) < 0) 
	    	ecode = code;
			else 
	    	delete_embed(&rsa, &nsa);
    } 
    else
			return 0;
    if (code >= 0) 
			*psa = rsa;
    return ecode;
}

static int
psdf_put_image_dict_param(XWPSContextState * ctx, XWPSParamList * plist, const char * pname,
			  XWPSCParamList **pplvalue,
			  const PSStreamTemplate * templat,
			  ss_put_params_t put_params)
{
    PSParamCollection dict;
    XWPSCParamList *plvalue = *pplvalue;
    int code;

    switch (code = plist->beginTransmit(ctx, pname, &dict, ps_param_collection_dict_any)) 
    {
			default:
	    	plist->signalError(ctx, pname, code);
	    	return code;
	    	
			case 1:
	    	return 0;
	    	
			case 0: 
				{
					PSStreamState* ss = (*(templat->create_state))();
	    		ss->templat = templat;
	    		if (templat->set_defaults)
						(*(templat->set_defaults))(ss);
	    		code = put_params(ctx, dict.list, ss, true);
	    		if (templat->release)
						(*(templat->release))(ss);
					delete ss;
					
	    		if (code < 0) 
						plist->signalError(ctx, pname, code);
	    		else 
	    		{
						plvalue = new XWPSCParamList;
						plvalue->write();
						code = plvalue->copy(ctx, dict.list);
						if (code < 0) 
						{
		    			delete plvalue;
		    			plvalue = *pplvalue;
						}
	    		}
				}
				plist->endTransmit(ctx, pname, &dict);
				break;
    }
    
    if (plvalue != *pplvalue) 
    {
			if (*pplvalue)
	    	delete (*pplvalue);
			*pplvalue = plvalue;
    }
    return code;
}

XWPSDevicePsdf::XWPSDevicePsdf(QObject * parent)
	:XWPSDeviceVector(parent),
	 version(psdf_version_level1),
	 binary_ok(false)
{
	vec_procs.beginpage_.beginpagepsdf = &XWPSDevicePsdf::beginPagePSDF;
	vec_procs.setlinewidth_.setlinewidthpsdf = &XWPSDevicePsdf::setLineWidthPSDF;
	vec_procs.setlinecap_.setlinecappsdf = &XWPSDevicePsdf::setLineCapPSDF;
	vec_procs.setlinejoin_.setlinejoinpsdf = &XWPSDevicePsdf::setLineJoinPSDF;
	vec_procs.setmiterlimit_.setmiterlimitpsdf = &XWPSDevicePsdf::setMiterLimitPSDF;
	vec_procs.setdash_.setdashpsdf = &XWPSDevicePsdf::setDashPSDF;
	vec_procs.setflat_.setflatpsdf = &XWPSDevicePsdf::setFlatPSDF;
	vec_procs.setlogop_.setlogoppsdf = &XWPSDevicePsdf::setLogOpPSDF;
	vec_procs.setfillcolor_.setfillcolorpsdf = &XWPSDevicePsdf::setFillColorPSDF;
	vec_procs.setstrokecolor_.setstrokecolorpsdf = &XWPSDevicePsdf::setStrokeColorPSDF;
	vec_procs.dorect_.dorectpsdf = &XWPSDevicePsdf::doRectPSDF;
	vec_procs.beginpath_.beginpathpsdf = &XWPSDevicePsdf::beginPathPSDF;
	vec_procs.moveto_.movetopsdf = &XWPSDevicePsdf::moveToPSDF;
	vec_procs.lineto_.linetopsdf = &XWPSDevicePsdf::lineToPSDF;
	vec_procs.curveto_.curvetopsdf = &XWPSDevicePsdf::curveToPSDF;
	vec_procs.closepath_.closepathpsdf = &XWPSDevicePsdf::closePathPSDF;
	vec_procs.endpath_.endpathpsdf = &XWPSDevicePsdf::endPathPSDF;
		
	params.ColorImage.init(true, 72, 1.5, 0, 0);
	params.GrayImage.init(true, 72, 1.5, 0, 0);
	params.MonoImage.init(false, 300, 2.0, "CCITTFaxEncode", (PSStreamTemplate*)&s_CFE_template);
}

XWPSDevicePsdf::XWPSDevicePsdf(const char * devname,
	                             int w,
	                             int h,
	                             float xdpi,
	                             float ydpi,
	                             int nc,
	                             int depth,
	                             ushort mg, 
	                             ushort mc,
	                             ushort dg,
	                             ushort dc,
	                             PSDFVersion v,
	                             bool ascii,
	                             QObject * parent)
	:XWPSDeviceVector(devname, w, h, xdpi, ydpi, nc, depth, mg, mc, dg, dc, parent),
	 version(v),
	 binary_ok(!ascii)
{
	vec_procs.beginpage_.beginpagepsdf = &XWPSDevicePsdf::beginPagePSDF;
	vec_procs.setlinewidth_.setlinewidthpsdf = &XWPSDevicePsdf::setLineWidthPSDF;
	vec_procs.setlinecap_.setlinecappsdf = &XWPSDevicePsdf::setLineCapPSDF;
	vec_procs.setlinejoin_.setlinejoinpsdf = &XWPSDevicePsdf::setLineJoinPSDF;
	vec_procs.setmiterlimit_.setmiterlimitpsdf = &XWPSDevicePsdf::setMiterLimitPSDF;
	vec_procs.setdash_.setdashpsdf = &XWPSDevicePsdf::setDashPSDF;
	vec_procs.setflat_.setflatpsdf = &XWPSDevicePsdf::setFlatPSDF;
	vec_procs.setlogop_.setlogoppsdf = &XWPSDevicePsdf::setLogOpPSDF;
	vec_procs.setfillcolor_.setfillcolorpsdf = &XWPSDevicePsdf::setFillColorPSDF;
	vec_procs.setstrokecolor_.setstrokecolorpsdf = &XWPSDevicePsdf::setStrokeColorPSDF;
	vec_procs.dorect_.dorectpsdf = &XWPSDevicePsdf::doRectPSDF;
	vec_procs.beginpath_.beginpathpsdf = &XWPSDevicePsdf::beginPathPSDF;
	vec_procs.moveto_.movetopsdf = &XWPSDevicePsdf::moveToPSDF;
	vec_procs.lineto_.linetopsdf = &XWPSDevicePsdf::lineToPSDF;
	vec_procs.curveto_.curvetopsdf = &XWPSDevicePsdf::curveToPSDF;
	vec_procs.closepath_.closepathpsdf = &XWPSDevicePsdf::closePathPSDF;
	vec_procs.endpath_.endpathpsdf = &XWPSDevicePsdf::endPathPSDF;
	
	params.ASCII85EncodePages = ascii;
	params.ColorImage.init(true, 72, 1.5, 0, 0);
	params.GrayImage.init(true, 72, 1.5, 0, 0);
	params.MonoImage.init(false, 300, 2.0, "CCITTFaxEncode", (PSStreamTemplate*)&s_CFE_template);
}


int XWPSDevicePsdf::closePathPSDF(double, 
	                      double,
		                    double, 
		                    double, 
		                    PSPathType)
{
	stream()->pputs("h\n");
  return 0;
}

int  XWPSDevicePsdf::copyDevice(XWPSDevice **pnew)
{
	XWPSDevicePsdf * ret = new XWPSDevicePsdf(dname, width, height,HWResolution[0],HWResolution[1],color_info.num_components,color_info.depth,color_info.max_gray,color_info.max_color,color_info.dither_grays,color_info.dither_colors, version, true);
	ret->copyDeviceParamPSDF(this);
	*pnew = ret;
	return 0;
}

void XWPSDevicePsdf::copyDeviceParamPSDF(XWPSDevicePsdf * proto)
{
	copyDeviceParamVector(proto);
	version = proto->version;
	binary_ok = proto->binary_ok;
	params.copy(&(proto->params));
}

int XWPSDevicePsdf::curveToPSDF(double x0, 
	                    double y0,
			                double x1, 
			                double y1, 
			                double x2, 
			                double y2,
			                double x3, 
			                double y3, 
			                PSPathType)
{
	if (x1 == x0 && y1 == y0)
		stream()->print("%g %g %g %g v\n", x2, y2, x3, y3);
  else if (x3 == x2 && y3 == y2)
		stream()->print("%g %g %g %g y\n", x1, y1, x2, y2);
  else
		stream()->print("%g %g %g %g %g %g c\n", x1, y1, x2, y2, x3, y3);
  return 0;
}

int XWPSDevicePsdf::doRectPSDF(long x0, long y0, long x1, long y1, PSPathType type)
{
	int code = beginPath(type);
  if (code < 0)
		return code;
    
  stream()->print("%g %g %g %g re\n", fixed2float(x0), fixed2float(y0), fixed2float(x1 - x0), fixed2float(y1 - y0));
  return endPath(type);
}

int XWPSDevicePsdf::getParamsPSDF(XWPSContextState * ctx, XWPSParamList *plist)
{
	int code = getParamsVector(ctx, plist);
	if (code < 0 ||	
			(code = plist->writeItems(ctx, &params, NULL, (PSParamItem*)&psdf_param_items[0])) < 0 ||
			(code = psdf_write_name(ctx, plist, "AutoRotatePages",	AutoRotatePages_names[(int)params.AutoRotatePages])) < 0 ||
			(code = psdf_write_name(ctx, plist, "Binding",	Binding_names[(int)params.Binding])) < 0 ||
			(code = psdf_write_name(ctx, plist, "DefaultRenderingIntent",	DefaultRenderingIntent_names[(int)params.DefaultRenderingIntent])) < 0 ||
			(code = psdf_write_name(ctx, plist, "TransferFunctionInfo",	TransferFunctionInfo_names[(int)params.TransferFunctionInfo])) < 0 ||
			(code = psdf_write_name(ctx, plist, "UCRandBGInfo",	UCRandBGInfo_names[(int)params.UCRandBGInfo])) < 0 ||
			(code = psdf_get_image_params(ctx, plist, &Color_names, &params.ColorImage)) < 0 ||
			(code = psdf_write_name(ctx, plist, "ColorConversionStrategy",	ColorConversionStrategy_names[(int)params.ColorConversionStrategy])) < 0 ||
			(code = psdf_write_string_param(ctx, plist, "CalCMYKProfile", &params.CalCMYKProfile)) < 0 ||
			(code = psdf_write_string_param(ctx, plist, "CalGrayProfile",	&params.CalGrayProfile)) < 0 ||
			(code = psdf_write_string_param(ctx, plist, "CalRGBProfile",	&params.CalRGBProfile)) < 0 ||
			(code = psdf_write_string_param(ctx, plist, "sRGBProfile",	&params.sRGBProfile)) < 0 ||
			(code = psdf_get_image_params(ctx, plist, &Gray_names, &params.GrayImage)) < 0 ||
			(code = psdf_get_image_params(ctx, plist, &Mono_names, &params.MonoImage)) < 0 ||
			(code = psdf_get_embed_param(ctx, plist, ".AlwaysEmbed", &params.AlwaysEmbed)) < 0 ||
			(code = psdf_get_embed_param(ctx, plist, ".NeverEmbed", &params.NeverEmbed)) < 0 ||
			(code = psdf_write_name(ctx, plist, "CannotEmbedFontPolicy", CannotEmbedFontPolicy_names[(int)params.CannotEmbedFontPolicy])) < 0)
			return code;
  return code;
}

int XWPSDevicePsdf::lineToPSDF(double, double, double x, double y, PSPathType)
{
	stream()->print("%g %g l\n", x, y);
  return 0;
}

int XWPSDevicePsdf::moveToPSDF(double, double, double x, double y, PSPathType)
{
	stream()->print("%g %g m\n", x, y);
  return 0;
}

bool XWPSDevicePsdf::psSourceOk(PSParamString * psource)
{
	if (psource->size >= 2 && psource->data[0] == '(' &&
			psource->data[psource->size - 1] == ')')
		return true;
  
  return false;
}

int XWPSDevicePsdf::putParamsPSDF(XWPSContextState * ctx, XWPSParamList *plist)
{
	int ecode, code;
  XWPSPSDFDistillerParams paramsA;

  paramsA = params;
  paramsA.CalCMYKProfile.data = 0;
 	paramsA.CalGrayProfile.data = 0;
  paramsA.CalRGBProfile.data = 0;
  paramsA.sRGBProfile.data = 0;
  ecode = code = plist->readBool(ctx, "LockDistillerParams",  &paramsA.LockDistillerParams);
  if (params.LockDistillerParams && paramsA.LockDistillerParams)
		return ecode;

  code = plist->readItems(ctx, (PSParamItem*)&paramsA, (PSParamItem*)&psdf_param_items[0]);
  if (code < 0)
		ecode = code;
  paramsA.AutoRotatePages = (enum psdf_auto_rotate_pages)psdf_put_enum(ctx, plist, 
                     "AutoRotatePages", (int)paramsA.AutoRotatePages, AutoRotatePages_names, &ecode);
  paramsA.Binding = (enum psdf_binding)psdf_put_enum(ctx, plist, 
                     "Binding", (int)paramsA.Binding, Binding_names, &ecode);
  paramsA.DefaultRenderingIntent = (enum psdf_default_rendering_intent)psdf_put_enum(ctx, plist, 
                     "DefaultRenderingIntent", (int)paramsA.DefaultRenderingIntent, DefaultRenderingIntent_names, &ecode);
  paramsA.TransferFunctionInfo = (enum psdf_transfer_function_info)psdf_put_enum(ctx, plist, 
                     "TransferFunctionInfo", (int)paramsA.TransferFunctionInfo, TransferFunctionInfo_names, &ecode);
  paramsA.UCRandBGInfo = (enum psdf_ucr_and_bg_info)psdf_put_enum(ctx, plist, 
                     "UCRandBGInfo", (int)paramsA.UCRandBGInfo, UCRandBGInfo_names, &ecode);
  ecode = plist->putBool(ctx, "UseFlateCompression", &paramsA.UseFlateCompression, ecode);
  ecode = putImageParams(ctx, plist, &Color_names,  &paramsA.ColorImage, ecode);
  paramsA.ColorConversionStrategy = (enum psdf_color_conversion_strategy)psdf_put_enum(ctx, plist, 
                     "ColorConversionStrategy",  (int)paramsA.ColorConversionStrategy, ColorConversionStrategy_names, &ecode);
  ecode = psdf_read_string_param(ctx, plist, "CalCMYKProfile", &paramsA.CalCMYKProfile, ecode);
  ecode = psdf_read_string_param(ctx, plist, "CalGrayProfile", &paramsA.CalGrayProfile, ecode);
  ecode = psdf_read_string_param(ctx, plist, "CalRGBProfile", &paramsA.CalRGBProfile, ecode);
  ecode = psdf_read_string_param(ctx, plist, "sRGBProfile",  &paramsA.sRGBProfile, ecode);
  ecode = putImageParams(ctx, plist, &Gray_names,  &paramsA.GrayImage, ecode);
  ecode = putImageParams(ctx, plist, &Mono_names,  &paramsA.MonoImage, ecode);

  ecode = psdf_put_embed_param(ctx, plist, "~AlwaysEmbed", ".AlwaysEmbed", &paramsA.AlwaysEmbed, ecode);
  ecode = psdf_put_embed_param(ctx, plist, "~NeverEmbed", ".NeverEmbed", &paramsA.NeverEmbed, ecode);
  paramsA.CannotEmbedFontPolicy = (enum psdf_cannot_embed_font_policy)psdf_put_enum(ctx, plist,
  								 "CannotEmbedFontPolicy", (int)paramsA.CannotEmbedFontPolicy,  CannotEmbedFontPolicy_names, &ecode);

  if (ecode < 0)
		return ecode;
  code = putParamsVector(ctx, plist);
  if (code < 0)
		return code;

  params = paramsA;
  paramsA.CalCMYKProfile.data = 0;
 	paramsA.CalGrayProfile.data = 0;
  paramsA.CalRGBProfile.data = 0;
  paramsA.sRGBProfile.data = 0;
  return 0;
}

int XWPSDevicePsdf::setDashPSDF(const float *pattern, uint count, double offset)
{
	XWPSStream *s = stream();
  s->pputs("[ ");
  for (uint i = 0; i < count; ++i)
		s->print("%g ", pattern[i]);
  s->print("] %g d\n", offset);
  return 0;
}

int XWPSDevicePsdf::setFillColorPSDF(XWPSDeviceColor * pdc)
{
	return setColor(pdc, &psdf_set_fill_color_commands);
}

int XWPSDevicePsdf::setFlatPSDF(double flatness)
{
	stream()->print("%g i\n", flatness);
  return 0;
}

int XWPSDevicePsdf::setLineCapPSDF(PSLineCap cap)
{
	stream()->print("%d J\n", cap);
  return 0;
}

int XWPSDevicePsdf::setLineJoinPSDF(PSLineJoin join)
{
	stream()->print("%d j\n", join);
  return 0;
}

int XWPSDevicePsdf::setLineWidthPSDF(double widthA)
{
	stream()->print("%g w\n", widthA);
  return 0;
}

int XWPSDevicePsdf::setMiterLimitPSDF(double limit)
{
	stream()->print("%g M\n", limit);
  return 0;
}

int XWPSDevicePsdf::setStrokeColorPSDF(XWPSDeviceColor * pdc)
{
	return setColor(pdc, &psdf_set_stroke_color_commands);
}

ulong XWPSDevicePsdf::adjustColorIndexPSDF(ulong color)
{
	return (color == (ps_no_color_index ^ 1) ? ps_no_color_index : color);
}

bool XWPSDevicePsdf::objnameIsValid(const uchar *data, uint size)
{
	return (size >= 2 && data[0] == '{' &&  (const uchar *)memchr(data, '}', size) == data + size - 1);
}

int XWPSDevicePsdf::putImageParams(XWPSContextState * ctx,
	                   XWPSParamList * plist,
		                 const PSPSDFImageParamNames * pnames,
		                 XWPSPSDFImageParams * params, 
		                 int ecode)
{
	PSParamString fs;
  const char * pname;
  
  PSParamItem *items = (PSParamItem*)((pnames->items[0].key == 0 ? pnames->items + 1 : pnames->items));
  int code = plist->readItems(ctx, params, items);

  if ((pname = pnames->ACSDict) != 0) 
  {
		code = psdf_put_image_dict_param(ctx, plist, pname, &params->ACSDict, &s_DCTE_template, psdf_DCT_put_params);
		if (code < 0)
	    ecode = code;
  }
  
  if ((pname = pnames->Dict) != 0) 
  {
		const PSStreamTemplate *templat;
		ss_put_params_t put_params;
		if (pnames->Dict[0] == 'M')
		{
	    templat = &s_CFE_template; 
	    put_params = psdf_CF_put_params;
	  }
		else
		{
	    templat = &s_DCTE_template;
	    put_params = psdf_DCT_put_params;
	  }
		code = psdf_put_image_dict_param(ctx, plist, pname, &params->Dict, templat, put_params);
		if (code < 0)
	    ecode = code;
  }
   
  params->DownsampleType = (enum psdf_downsample_type)psdf_put_enum(ctx, plist, 
                 pnames->DownsampleType, (int)params->DownsampleType, DownsampleType_names, &ecode);
                 
  switch (code = plist->readString(ctx, pnames->Filter, &fs)) 
  {
		case 0:
	    {
				const PSPSDFImageFilterName *pn = pnames->filter_names;

				while (pn->pname != 0 && !paramStringEq(&fs, pn->pname))
		    	pn++;
				if (pn->pname == 0 || pn->min_version > version) 
				{
		    	ecode = XWPSError::RangeCheck;
		    	goto ipe;
				}
				params->Filter = pn->pname;
				params->filter_template = (PSStreamTemplate*)(pn->templat);
				break;
	    }
	    
		default:
	    ecode = code;

ipe:
			plist->signalError(ctx, pnames->Filter, ecode);
		case 1:
	    break;
  }
  
  if (ecode >= 0) 
  {
		if (params->Resolution < 1)
	    params->Resolution = 1;
		if (params->DownsampleThreshold < 1 ||
	    params->DownsampleThreshold > 10)
	    params->DownsampleThreshold = pnames->DownsampleThreshold_default;
		switch (params->Depth) 
		{
	    default:
				params->Depth = -1;
	    case 1:
	    case 2:
	    case 4:
	    case 8:
	    case -1:
				break;
		}
  }
  return ecode;
}

int XWPSDevicePsdf::setColor(XWPSDeviceColor * pdc, const PSPSDFSetColorCommands *ppscc)
{
	const char *setcolor;

  if (!pdc->isPure())
		return (int)(XWPSError::RangeCheck);
  {
		XWPSStream *s = stream();
		ulong color = adjustColorIndexPSDF(pdc->pureColor());
		double v3 = round_byte_color(color & 0xff);
		switch (color_info.num_components) 
		{
			case 4:
	    	if ((color & (0xffffff << 8)) == 0 && ppscc->setgray != 0) 
	    	{
					v3 = 1.0 - v3;
					goto g;
	    	}
	    	s->print("%g %g %g %g", round_byte_color(color >> 24),	
	    						round_byte_color((color >> 16) & 0xff),
		    					round_byte_color((color >> 8) & 0xff), v3);  	
		    setcolor = ppscc->setcmykcolor;
	    	break;
	    	
			case 3:
	    	if (!((color ^ (color >> 8)) & 0xffff) && ppscc->setgray != 0)
					goto g;
	    	s->print("%g %g %g", round_byte_color((color >> 16) & 0xff), round_byte_color((color >> 8) & 0xff), v3);
	    	setcolor = ppscc->setrgbcolor;
	    	break;
	    	
			case 1:
g:
	    	s->print("%g", v3);
	    	setcolor = ppscc->setgray;
	    	break;
	    	
			default:
	    	return (int)(XWPSError::RangeCheck);
		}
		if (setcolor)
	    s->print(" %s\n", setcolor);
  }
  return 0;
}

int XWPSDevicePsdf::writeString(XWPSStream * s, const uchar * str, uint size)
{
	s->writePSString(str, size, binary_ok ? PRINT_BINARY_OK : 0); return 0;
}

XWPSPSDFBinaryWriter::XWPSPSDFBinaryWriter()
{
	A85E = 0;
	target = 0;
	strm = 0;
	dev = 0;
}

XWPSPSDFBinaryWriter::~XWPSPSDFBinaryWriter()
{
}

int XWPSPSDFBinaryWriter::beginBinary(XWPSDevicePsdf * pdev)
{
	target = pdev->strm;
  dev = pdev;
  
  if (!pdev->binary_ok)
  {
  	int l = 100;
  	uchar *buf = (uchar*)malloc(l * sizeof(uchar*));
  	PSStreamA85EState * ss = new PSStreamA85EState;
  	ss->templat = &s_A85E_template;
  	A85E = new XWPSStream;  	
  	strm = A85E;
  	XWPSStream::initFilter(strm, (PSStreamState*)ss, buf, l, pdev->strm, true);
  }
  else
  {
  	strm = pdev->strm;
  }
  
  return 0;
}

int XWPSPSDFBinaryWriter::CFEBinary(int w, int, bool invert)
{
	const PSStreamTemplate *templat = &s_CFE_template;
  PSStreamCFEState *st = (PSStreamCFEState*)((*(templat->create_state))());
  int code;

  (*(templat->set_defaults))((PSStreamState *) st);
  st->K = -1;
  st->Columns = w;
  st->Rows = 0;
  st->BlackIs1 = !invert;
  st->EndOfBlock = strm->state->templat != &s_A85E_template;
  code = encodeBinary((PSStreamTemplate*)templat, (PSStreamState *) st, true);
  return code;
}

int XWPSPSDFBinaryWriter::DCTFilter(XWPSContextState * ctx,
	                              XWPSParamList *plist,
								       PSStreamState *st,
								       int Columns, 
								       int Rows, 
								       int Colors,
								       XWPSPSDFBinaryWriter *pbw,
								       bool dss)
{
	PSStreamDCTState *const ss = (PSStreamDCTState *) st;
	PSJPEGCompressData *jcdp;
	XWPSCParamList rcc_list;
	int code;
	
	rcc_list.write();
	if ((code = rcc_list.writeInt(ctx, "Rows", &Rows)) < 0 ||
	    (code = rcc_list.writeInt(ctx, "Columns", &Columns)) < 0 ||
	    (code = rcc_list.writeInt(ctx, "Colors",  &Colors)) < 0) 
	{
	    goto rcc_fail;
	}
	
	rcc_list.read();
	if (plist)
	  rcc_list.setTarget(plist);
	jcdp = (PSJPEGCompressData *)malloc(sizeof(PSJPEGCompressData));
	ss->data.compress = jcdp;
	if ((code = ps_jpeg_create_compress(ss)) < 0)
	   goto dcte_fail;
	   
	rcc_list.sDCTEPutParams(ctx, ss);
	jcdp->templat = s_DCTE_template;
	ss->scan_line_size = jcdp->cinfo.input_components * jcdp->cinfo.image_width;
	jcdp->templat.min_in_size = qMax(s_DCTE_template.min_in_size, ss->scan_line_size);
	jcdp->templat.min_out_size = qMax(s_DCTE_template.min_out_size, ss->Markers.size);
	if (pbw)
	   code = pbw->encodeBinary(&jcdp->templat, st, dss);
	if (code >= 0) 
	{
	    return 0;
	}
   
dcte_fail:
	ps_jpeg_destroy(ss);
	free(jcdp);
	
rcc_fail:
	return code;
}

int XWPSPSDFBinaryWriter::encodeBinary(PSStreamTemplate * templat, PSStreamState * ss, bool dss)
{
	XWPSStream::addFilter(&strm, templat, ss, dss); 
	return 0;
}

int XWPSPSDFBinaryWriter::endBinary()
{
	return XWPSStream::closeFilters(&strm, target);
}

int XWPSPSDFBinaryWriter::pixelResize(int width, int num_components, int bpc_in, int bpc_out)
{
	const PSStreamTemplate *templat;
  PSStreamState *st;

  if (bpc_out == bpc_in)
		return 0;
  if (bpc_in != 8) 
  {
		static const PSStreamTemplate *const exts[13] = {
	    0, &s_1_8_template, &s_2_8_template, 0, &s_4_8_template,
	    0, 0, 0, 0, 0, 0, 0, &s_12_8_template};

		templat = exts[bpc_in];
  } 
  else 
  {
		static const PSStreamTemplate *const rets[5] = {
	    0, &s_8_1_template, &s_8_2_template, 0, &s_8_4_template	};

		templat = rets[bpc_out];
  }
  
  st = (*(templat->create_state))();
  encodeBinary((PSStreamTemplate*)templat, st, true);
  s_1248_init((PSStream1248State*)st, width, num_components);
  return 0;
}

int XWPSPSDFBinaryWriter::setupDownSampling(XWPSPSDFImageParams * pdip,
		   									 XWPSPixelImage * pim, 
		   									 float resolution)
{
	const PSStreamTemplate *templat =(pdip->DownsampleType == ds_Subsample ?
	 						&s_Subsample_template : &s_Average_template);
  int factor = (int)(resolution / pdip->Resolution);
  int orig_bpc = pim->BitsPerComponent;
  int orig_width = pim->Width;
  int orig_height = pim->Height;
  PSStreamState *st;
  int code;
  
  st = (*(templat->create_state))();
  if (templat->set_defaults)
		(*(templat->set_defaults))(st);
	{
  	PSStreamDownsampleState * ss = (PSStreamDownsampleState*)st;
		ss->Colors =   (pim->ColorSpace == 0 ? 1 : pim->ColorSpace->numComponents());
		ss->WidthIn = pim->Width;
		ss->HeightIn = pim->Height;
		ss->XFactor = ss->YFactor = factor;
		ss->AntiAlias = pdip->AntiAlias;
		ss->padX = ss->padY = false;
		if (templat->init)
	    (*(templat->init))(st);
	  pim->Width = s_Downsample_size_out(pim->Width, factor, ss->padX);
		pim->Height = s_Downsample_size_out(pim->Height, factor, ss->padY);
		pim->BitsPerComponent = pdip->Depth;
		pim->ImageMatrix.matrixScale(&pim->ImageMatrix, (double)pim->Width / orig_width,(double)pim->Height / orig_height);
		if ((code = setupImageCompression(pdip, pim)) < 0 ||
	    (code = pixelResize(pim->Width, ss->Colors, 8, pdip->Depth)) < 0 ||
	    (code = encodeBinary((PSStreamTemplate*)templat, st, true)) < 0 ||
	    (code = pixelResize(orig_width, ss->Colors, orig_bpc, 8)) < 0 ) 
	  {
	  	if (st)
	  		delete st;
	  	strm->state = 0;
	    return code;
		}
	}
	return 0;
}

int XWPSPSDFBinaryWriter::setupImageCompression(XWPSPSDFImageParams *pdip, XWPSPixelImage * pim)
{
	XWPSDevicePsdf *pdev = dev;
	const PSStreamTemplate *templat = pdip->filter_template;
  const PSStreamTemplate *orig_template = templat;
  const PSStreamTemplate *lossless_template =	(pdev->params.UseFlateCompression &&
	 							pdev->version >= psdf_version_ll3 ?	 &s_zlibE_template : &s_LZWE_template);
  XWPSColorSpace *pcs = pim->ColorSpace;
  int Colors = (pcs ? pcs->numComponents() : 1);
  bool Indexed = (pcs != 0 &&  pcs->getIndex() == ps_color_space_index_Indexed);
  XWPSCParamList *dict = pdip->Dict;
  PSStreamState *st;
  int code;
  
  if (!pdip->Encode)
		return 0;
  if (pdip->AutoFilter)
  {
  	orig_template = templat = &s_DCTE_template;
		dict = pdip->ACSDict;
  }
  
  if (templat == 0)
		return 0;
  if (pim->Width * pim->Height * Colors * pim->BitsPerComponent <= 160)	
		return 0;
		
	if (templat == &s_DCTE_template) 
	{
		if (Indexed ||
	    !(pdip->Downsample ?
	      pdip->Depth == 8 ||
	      (pdip->Depth == -1 && pim->BitsPerComponent == 8) :
	      pim->BitsPerComponent == 8)) 
	  {
	    templat = lossless_template;
		}
  }
  
  st = (*(templat->create_state))();
  	
  if (templat->set_defaults)
		(*(templat->set_defaults))(st);
  if (templat == &s_CFE_template)
  {
  	PSStreamCFEState * ss = (PSStreamCFEState*)st;
		if (pdip->Dict != 0 && pdip->filter_template == templat) 
		{
	    pdip->Dict->putParams(pdev->context_state,  (PSStreamCFState *)ss); 
		} 
		else 
		{
	    ss->K = -1;
	    ss->BlackIs1 = true;
		}
		ss->Columns = pim->Width;
		ss->Rows = (ss->EndOfBlock ? 0 : pim->Height);
  }
  else if ((templat == &s_LZWE_template ||
						templat == &s_zlibE_template) &&
	       		pdev->version >= psdf_version_ll3)
	{
		if (!Indexed) 
		{
	    code = encodeBinary((PSStreamTemplate*)templat, st, true);
	    if (code < 0)
				goto fail;
	    templat = &s_PNGPE_template;
	    st = (*(templat->create_state))();
	    if (templat->set_defaults)
				(*(templat->set_defaults)) (st);
	    {
				PSStreamPNGPState * ss = (PSStreamPNGPState *) st;

				ss->Colors = Colors;
				ss->Columns = pim->Width;
	    }
		}
	}
	else if (templat == &s_DCTE_template)
	{
		code = DCTFilter(pdev->context_state, (dict != 0 && orig_template == templat ? dict : NULL),	st, pim->Width, pim->Height, Colors, this, true);
		if (code < 0)
	    goto fail;
		return 0;
	}
	
	code = encodeBinary((PSStreamTemplate*)templat, st, true);
  if (code >= 0)
		return 0;
		
fail:
  return code;
}

int XWPSPSDFBinaryWriter::setupImageFilters(XWPSDevicePsdf * pdev, 
			 								 XWPSPixelImage * pim, 
			 								 XWPSMatrix * pctm,
			 							   XWPSImagerState * pis)
{
	int code = 0;
  XWPSPSDFImageParams params;
  int bpc = pim->BitsPerComponent;
  int bpc_out = pim->BitsPerComponent = qMin(bpc, 8);
  int ncomp;
  double resolution;
  if (pim->ColorSpace == NULL)
  {
  	params = pdev->params.MonoImage;
		params.Depth = 1;
		ncomp = 1;
  }
  else
  {
  	ncomp = pim->ColorSpace->numComponents();
		if (ncomp == 1) 
		{
	    if (bpc == 1)
				params = pdev->params.MonoImage;
	    else
				params = pdev->params.GrayImage;
	    if (params.Depth == -1)
				params.Depth = bpc;
		} 
		else 
		{
	    params = pdev->params.ColorImage;	  
		}
  }
  
  if (pctm == 0)
		resolution = -1;
  else 
  {
		XWPSPoint pt;
		pt.distanceTransformInverse(1.0, 0.0, &pim->ImageMatrix);
		pt.distanceTransform(pt.x, pt.y, pctm);
		resolution = 1.0 / hypot(pt.x / pdev->HWResolution[0], pt.y / pdev->HWResolution[1]);
  }
  
  if (ncomp == 1)
  {
  	if (params.doDownSample(pim, resolution))
  	{
  		if (params.Depth == 1) 
  		{
				params.Filter = pdev->params.MonoImage.Filter;
				params.filter_template = pdev->params.MonoImage.filter_template;
				params.Dict = pdev->params.MonoImage.Dict;
	   	} 
	   	else 
	   	{
				params.Filter = pdev->params.GrayImage.Filter;
				params.filter_template = pdev->params.GrayImage.filter_template;
				params.Dict = pdev->params.GrayImage.Dict;
	    }
	    code = setupDownSampling(&params, pim, resolution);
  	}
  	else 
  	{
	    code = setupImageCompression(&params, pim);
		}
		if (code < 0)
	    return code;
		code = pixelResize(pim->Width, ncomp, bpc, bpc_out);
  }
  else
  {
  	bool cmyk_to_rgb = pdev->params.ConvertCMYKImagesToRGB && pis != 0 &&
	    									pim->ColorSpace->getIndex() == ps_color_space_index_DeviceCMYK;

		if (cmyk_to_rgb)
	    pim->ColorSpace = pis->deviceRGB();
		if (params.Depth == -1)
	    params.Depth = (cmyk_to_rgb ? 8 : bpc_out);
		if (params.doDownSample(pim, resolution)) 
		{
	    code = setupDownSampling(&params, pim, resolution);
		} 
		else 
		{
	    code = setupImageCompression(&params, pim);
		}
		if (cmyk_to_rgb) 
		{
	    PSStreamC2RState *ss = new PSStreamC2RState;
	    int code = pixelResize(pim->Width, 3, 8, bpc_out);
		    
	    if (code < 0 ||	(code = encodeBinary((PSStreamTemplate*)&s_C2R_template,  (PSStreamState *) ss, true)) < 0 ||
					(code = pixelResize(pim->Width, 4, bpc, 8)) < 0)
				return code;
	    s_C2R_init(ss, pis);
		} 
		else 
		{
	    code = pixelResize(pim->Width, ncomp, bpc, bpc_out);
	    if (code < 0)
				return code;
		}
  }
  return code;
}

int XWPSPSDFBinaryWriter::setupLosslessFilters(XWPSDevicePsdf *pdev, XWPSPixelImage *pim)
{
	XWPSDevicePsdf ipdev;
	ipdev.copyDeviceParamPSDF(pdev);
	ipdev.copyDeviceProc(pdev);
	
	ipdev.params.ColorImage.AutoFilter = false;
  ipdev.params.ColorImage.Downsample = false;
  ipdev.params.ColorImage.Filter = "FlateEncode";
  ipdev.params.ColorImage.filter_template = (PSStreamTemplate*)&s_zlibE_template;
  ipdev.params.ConvertCMYKImagesToRGB = false;
  ipdev.params.GrayImage.AutoFilter = false;
  ipdev.params.GrayImage.Downsample = false;
  ipdev.params.GrayImage.Filter = "FlateEncode";
  ipdev.params.GrayImage.filter_template = (PSStreamTemplate*)&s_zlibE_template;
  return setupImageFilters(&ipdev, pim, NULL, NULL);
}
