/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <math.h>
#include <QBuffer>
#include "XWApplication.h"
#include "XWObject.h"
#include "XWDict.h"
#include "XWLexer.h"
#include "XWDVIParser.h"
#include "XWDVIType.h"
#include "XWDVIRef.h"
#include "XWDVIDev.h"
#include "XWDVICore.h"
#include "XWPSError.h"
#include "psbittable.h"
#include "XWPSParam.h"
#include "XWPSStream.h"
#include "XWPSFunction.h"
#include "XWPSCie.h"
#include "XWPSColorSpace.h"
#include "XWPSShading.h"
#include "XWPSFont.h"
#include "XWPSPath.h"
#include "XWPSText.h"
#include "XWPSHalfTone.h"
#include "XWPSPattern.h"
#include "XWPSState.h"
#include "XWPSPDFResource.h"
#include "XWPSDeviceDVI.h"
#include "XWPSContextState.h"

const int CoreDistVersion = 4000;
static bool MAKE_FONT_NAMES_UNIQUE = true;

XWPSDVIResource::XWPSDVIResource()
{
	next = 0;
	prev = 0;
	rid = 0;
	named = false;
	used_on_page = false;
	indirect.initNull();
	object.initNull();
}

XWPSDVIResource::~XWPSDVIResource()
{
	object.free();
}

void XWPSDVIResource::reset()
{
	object.free();
}

long XWPSDVIResource::resourceId()
{
	return indirect.label;
}

XWPSDVIFontFile::XWPSDVIFontFile()
{
	strm = 0;
	save_strm = 0;
}

XWPSDVIFontFile::~XWPSDVIFontFile()
{
	reset();
}

void XWPSDVIFontFile::reset()
{
	if (strm)
	{
		strm->close_file = false;
		if (strm->decRef() == 0)
			delete strm;
		strm = 0;
	}
	save_strm = 0;
}

XWPSDVIFontDescriptor::XWPSDVIFontDescriptor()
	:XWPSDVIResource()
{
	memset(FontName.chars, 0, MAX_PDF_FONT_NAME);
	FontName.size = 0;
	subset_ok = false;
	FontFile = 0;
	base_font = 0;
	notified = false;
	written = false;
}

XWPSDVIFontDescriptor::~XWPSDVIFontDescriptor()
{
	reset();
	
	if (FontFile)
	{
		delete FontFile;
		FontFile = 0;
	}
	
	if (base_font)
	{
		if (base_font->decRef() == 0)
			delete base_font;
		base_font = 0;
	}
}

long XWPSDVIFontDescriptor::fontDescriptorId()
{
	return resourceId();
}

void XWPSDVIFontDescriptor::reset()
{
	if (chars_used.data)
	{
		delete [] chars_used.data;
		chars_used.data = 0;
	}
	
	if (glyphs_used.data)
	{
		delete [] glyphs_used.data;
		glyphs_used.data = 0;
	}
	
	if (FontFile)
		FontFile->reset();
		
	XWPSDVIResource::reset();
}

XWPSDVICharProc::XWPSDVICharProc()
	:XWPSDVIResource()
{
	font = 0;
	char_next = 0;
	width = 0; 
	height = 0;
	x_width = 0;
	y_offset = 0;
	char_code = 0;
	strm = 0;
	save_strm = 0;
}

XWPSDVICharProc::~XWPSDVICharProc()
{
	reset();
}

long XWPSDVICharProc::charProcId()
{
	return resourceId();
}

void XWPSDVICharProc::reset()
{
	if (strm)
	{
		strm->close_file = false;
		if (strm->decRef() == 0)
			delete strm;
		strm = 0;
	}
	XWPSDVIResource::reset();
}

XWPSDVIFont::XWPSDVIFont()
	:XWPSDVIResource()
{
	memset(fname.chars, 0, MAX_PDF_FONT_NAME);
	fname.size = 0;
	FontType = ft_composite;
	font = 0;
	index = 0;
	is_MM_instance = false;
	memset(frname, 0, sizeof(frname));
	BaseEncoding = ENCODING_INDEX_UNKNOWN;
	Differences = 0;
	FontDescriptor = 0;
	FirstChar = 0; 
	LastChar = 0;
	write_Widths = false;
	memset(Widths, 0, 256 * sizeof(int));
	memset(widths_known, 0, 32);
	skip = false;
	num_chars = 0;
	char_procs = 0;
	max_y_offset = 0;
	memset(spaces, 0, X_SPACE_MAX - X_SPACE_MIN + 1);
}

XWPSDVIFont::~XWPSDVIFont()
{
	reset();
}

int XWPSDVIFont::charWidth(int ch, XWPSFont *fontA, int *pwidth)
{
	if (ch < 0 || ch > 255)
		return (int)(XWPSError::RangeCheck);
    
  if (!(widths_known[ch >> 3] & (1 << (ch & 7)))) 
  {
		XWPSFontBase *bfont = (XWPSFontBase *)fontA;
		ulong glyph = bfont->encodeChar(ch, GLYPH_SPACE_INDEX);
		int wmode = fontA->WMode;
		XWPSGlyphInfo info;
		double w, v;
		int code;

		if (glyph != ps_no_glyph &&  (code = fontA->glyphInfo(glyph, NULL, GLYPH_INFO_WIDTH0 << wmode, &info)) >= 0) 
		{
	    XWPSString gnstr;

	    if (wmode && (w = info.width[wmode].y) != 0)
				v = info.width[wmode].x;
	    else
				w = info.width[wmode].x, v = info.width[wmode].y;
	    if (v != 0)
				return (int)(XWPSError::RangeCheck);
	    if (fontA->FontType == ft_TrueType) 
	    {
				w *= 1000;
	    }
	    Widths[ch] = (int)w;
	    gnstr.data = (uchar *)(bfont->glyphName(glyph, &gnstr.size));
	    if (gnstr.size != 7 || memcmp(gnstr.data, ".notdef", 7))
				widths_known[ch >> 3] |= 1 << (ch & 7);
		} 
		else 
		{
	    XWPSPoint tt_scale;
	    tt_scale.x = 1000;
	    tt_scale.y = 1000;
	    const XWPSPoint *pscale = 0;
	    XWPSFontInfo finfo;

	    if (fontA->FontType == ft_TrueType) 
				pscale = &tt_scale;
				
	    code = fontA->fontInfo(pscale, FONT_INFO_MISSING_WIDTH, &finfo);
	    if (code < 0)
				return code;
	    Widths[ch] = finfo.MissingWidth;
		}
  }
  if (pwidth)
		*pwidth = Widths[ch];
  return 0;
}

long XWPSDVIFont::fontId()
{
	return resourceId();
}

void XWPSDVIFont::reset()
{
	if (Differences)
	{
		delete [] Differences;
		Differences = 0;
	}
	
	if (font)
	{
		if (font->decRef() == 0)
			delete font;
		font = 0;
	}
	XWPSDVIResource::reset();
}

XWPSDVIXObjects::XWPSDVIXObjects()
	:XWPSDVIResource()
{
	width = height = 0;
	data_height = 0;
}

XWPSDVIStdFont::XWPSDVIStdFont()
{
	font = 0;
	pfd = 0;
}

XWPSDVIStdFont::~XWPSDVIStdFont()
{
}

XWPSDVITextState::XWPSDVITextState()
{
	reset();
}

XWPSDVITextState::~XWPSDVITextState()
{
}

void XWPSDVITextState::reset()
{
	character_spacing = 0;
	font = 0;
	size = 0;
	word_spacing = 0;
	leading = 0;
	use_leading = false;
	buffer[0] = 0;
	buffer_count = 0;
}

XWPSDVIResourceList::XWPSDVIResourceList()
{
	for (int i = 0; i < NUM_RESOURCE_CHAINS; i++)
		chains[i] = 0;
}

XWPSDVIResourceList::~XWPSDVIResourceList()
{
}

void XWPSDVIResourceList::reset()
{
	for (int i = 0; i < NUM_RESOURCE_CHAINS; i++)
		chains[i] = 0;
}

XWPSDVIGraphicsSave::XWPSDVIGraphicsSave()
{
	prev = 0;
	position = 0;
	save_context = PDF_IN_NONE;
	save_contents_id = 0;
	strm = 0;
	save_strm = 0;
}

XWPSDVIGraphicsSave::~XWPSDVIGraphicsSave()
{
	if (strm)
	{
		strm->close_file = false;
		if (strm->decRef() == 0)
			delete strm;
		strm = 0;
	}
	object.free();
}

XWPSDVIDataWriter::XWPSDVIDataWriter()
{
	start = 0;
	length_id = 0;
	object = 0;
	strm = 0;
	save_strm = 0;
}

XWPSDVIDataWriter::~XWPSDVIDataWriter()
{	
	if (strm)
	{
		strm->close_file = false;
		if (strm->decRef() == 0)
			delete strm;
		strm = 0;
	}
}

XWPSDVIImageWriter::XWPSDVIImageWriter()
{
	pin = 0;
	pres = 0;
	height = 0;
	end_string = 0;
	strm = 0;
	save_strm = 0;
	dict = 0;
}

XWPSDVIImageWriter::~XWPSDVIImageWriter()
{
	if (strm)
	{
		strm->close_file = false;
		if (strm->decRef() == 0)
			delete strm;
		strm = 0;
	}
	data.free();
}

XWPSDVIOutlineNode::XWPSDVIOutlineNode()
{
	id = 0; 
	parent_id = 0; 
	prev_id = 0; 
	first_id = 0; 
	last_id = 0;
	count = 0;
}

XWPSDVIOutlineNode::~XWPSDVIOutlineNode()
{
}

XWPSDVIOutlineLevel::XWPSDVIOutlineLevel()
{
	left = max_int;
}


XWPSDVIArticle::XWPSDVIArticle()
{
	next = 0;

	id = 0;
}

XWPSDVIArticle::~XWPSDVIArticle()
{
}

context_proc_dvi XWPSDeviceDVI::context_procs_dvi[4][4] =
{
	{0, &XWPSDeviceDVI::noneToStream, &XWPSDeviceDVI::noneToStream, &XWPSDeviceDVI::noneToStream},
  {&XWPSDeviceDVI::streamToNone, 0, &XWPSDeviceDVI::streamToText, &XWPSDeviceDVI::streamToText},
  {&XWPSDeviceDVI::textToStream, &XWPSDeviceDVI::textToStream, 0, 0},
  {&XWPSDeviceDVI::stringToText, &XWPSDeviceDVI::stringToText, &XWPSDeviceDVI::stringToText, 0}
};

PSDVIMarkName XWPSDeviceDVI::mark_names[] = 
{
	{"ANN",          &XWPSDeviceDVI::pdfmarkANN,         PDFMARK_NAMEABLE},
    {"LNK",        &XWPSDeviceDVI::pdfmarkLNK,         PDFMARK_NAMEABLE},
    {"OUT",        &XWPSDeviceDVI::pdfmarkOUT,         0},
    {"ARTICLE",    &XWPSDeviceDVI::pdfmarkARTICLE,     0},
    {"DEST",       &XWPSDeviceDVI::pdfmarkDEST,        PDFMARK_NAMEABLE},
    {"PS",         &XWPSDeviceDVI::pdfmarkPS,          PDFMARK_NAMEABLE},
    {"PAGES",      &XWPSDeviceDVI::pdfmarkPAGES,       0},
    {"PAGE",       &XWPSDeviceDVI::pdfmarkPAGE,        0},
    {"DOCINFO",    &XWPSDeviceDVI::pdfmarkDOCINFO,     0},
    {"DOCVIEW",     &XWPSDeviceDVI::pdfmarkDOCVIEW,     0},
	/* Named objects. */
    {"BP",          &XWPSDeviceDVI::pdfmarkBP,          PDFMARK_NAMEABLE},
    {"EP",           &XWPSDeviceDVI::pdfmarkEP,          0},
    {"SP",          &XWPSDeviceDVI::pdfmarkSP,          PDFMARK_ODD_OK | PDFMARK_KEEP_NAME},
    {"OBJ",          &XWPSDeviceDVI::pdfmarkOBJ,         PDFMARK_NAMEABLE},
    {"PUT",          &XWPSDeviceDVI::pdfmarkPUT,         PDFMARK_ODD_OK | PDFMARK_KEEP_NAME},
    {".PUTDICT",     &XWPSDeviceDVI::pdfmarkPUTDICT,     PDFMARK_ODD_OK | PDFMARK_KEEP_NAME},
    {".PUTINTERVAL", &XWPSDeviceDVI::pdfmarkPUTINTERVAL, PDFMARK_ODD_OK | PDFMARK_KEEP_NAME},
    {".PUTSTREAM",   &XWPSDeviceDVI::pdfmarkPUTSTREAM,   PDFMARK_ODD_OK | PDFMARK_KEEP_NAME |
                                          PDFMARK_NO_REFS},
    {"CLOSE",        &XWPSDeviceDVI::pdfmarkCLOSE,       PDFMARK_ODD_OK | PDFMARK_KEEP_NAME},
    {"NamespacePush", &XWPSDeviceDVI::pdfmarkNamespacePush, 0},
    {"NamespacePop", &XWPSDeviceDVI::pdfmarkNamespacePop, 0},
    {"NI",           &XWPSDeviceDVI::pdfmarkNI,          PDFMARK_NAMEABLE},
	/* Document structure. */
    {"StRoleMap",    &XWPSDeviceDVI::pdfmarkStRoleMap,   0},
    {"StClassMap",   &XWPSDeviceDVI::pdfmarkStClassMap,  0},
    {"StPNE",        &XWPSDeviceDVI::pdfmarkStPNE,       PDFMARK_NAMEABLE},
    {"StBookmarkRoot", &XWPSDeviceDVI::pdfmarkStBookmarkRoot, 0},
    {"StPush",       &XWPSDeviceDVI::pdfmarkStPush,       0},
    {"StPop",        &XWPSDeviceDVI::pdfmarkStPop,        0},
    {"StPopAll",     &XWPSDeviceDVI::pdfmarkStPopAll,     0},
    {"StBMC",        &XWPSDeviceDVI::pdfmarkStBMC,        0},
    {"StBDC",        &XWPSDeviceDVI::pdfmarkStBDC,        0},
    {"EMC",          &XWPSDeviceDVI::pdfmarkEMC,          0},
    {"StOBJ",        &XWPSDeviceDVI::pdfmarkStOBJ,        0},
    {"StAttr",       &XWPSDeviceDVI::pdfmarkStAttr,       0},
    {"StStore",      &XWPSDeviceDVI::pdfmarkStStore,      0},
    {"StRetrieve",   &XWPSDeviceDVI::pdfmarkStRetrieve,   0},
	/* End of list. */
    {0, 0}
};

static double round_byte_color(int cv)
{
	return (int)(cv * (1000.0 / 255.0) + 0.5) / 1000.0;
}

 
static int
dvi_make_mxd(XWPSDevice **pmxdev, XWPSDevice *tdev)
{
  XWPSDeviceNull *fdev = new XWPSDeviceNull;
  fdev->context_state = tdev->context_state;
  fdev->setTarget(tdev);
  *pmxdev = fdev;
  return 0;
}

static int
dvi_image3_make_mid(XWPSDevice **pmidev, XWPSDevice *dev, int , int )
{
  int code = dvi_make_mxd(pmidev, dev);

  if (code < 0)
		return code;
  (*pmidev)->procs.begin_typed_image_.begin_typed_imagenull = &XWPSDeviceNull::beginTypedImageDVIMid;
  return 0;
}

static int
dvi_image3_make_mcde(XWPSDevice *dev, 
                     XWPSImagerState *pis,
		                 XWPSMatrix *pmat, 
		                 XWPSImageCommon *pic,
		     						XWPSIntRect *prect, 
		     						XWPSDeviceColor *pdcolor,
		                 XWPSClipPath *pcpath,
		                 XWPSImageEnumCommon **pinfo,
		                XWPSDevice **pmcdev, 
		                XWPSDevice *midev,
		                XWPSImageEnumCommon *pminfo,
		                XWPSIntPoint *)
{
  int code = dvi_make_mxd(pmcdev, midev);
  XWPSDVIImageEnum *pmie;
  XWPSDVIImageEnum *pmce;
  XWDict *pmcs;

  if (code < 0)
		return code;
	
	XWPSDeviceDVI * pdev = (XWPSDeviceDVI*)dev;
  code = pdev->beginTypedImage(pis, pmat, pic, prect, pdcolor, pcpath,  pinfo, PDF_IMAGE_TYPE3_DATA);
  if (code < 0)
		return code;
		
  if ((*pinfo)->procs.plane_data_.plane_datadvi != &XWPSDVIImageEnum::dviImagePlaneData) 
  {
		(*pinfo)->end(false);
		if (!((*pmcdev)->decRef()))
			delete *pmcdev;
		*pmcdev = 0;
		
		return (int)(XWPSError::RangeCheck);
  }
  pmie = (XWPSDVIImageEnum *)pminfo;
  pmce = (XWPSDVIImageEnum *)(*pinfo);
  if (pmce->writer->pres->object.isStream())
  	pmcs = pmce->writer->pres->object.streamGetDict();
  else
  	pmcs = pmce->writer->pres->object.getDict();
  pmcs->add(qstrdup("Mask"),  &(pmie->writer->pres->indirect));
  return 0;
}

static int
dvi_image3x_make_mid(XWPSDevice **pmidev, XWPSDevice *dev, int , int , int )
{
  int code = dvi_make_mxd(pmidev, dev);

  if (code < 0)
		return code;
  (*pmidev)->procs.begin_typed_image_.begin_typed_imagenull = &XWPSDeviceNull::beginTypedImageDVIMid;
  return 0;
}

static int
dvi_image3x_make_mcde(XWPSDevice *dev, 
                      XWPSImagerState *pis,
		                  XWPSMatrix *pmat, 
		                  XWPSImageCommon *pic,
		                  XWPSIntRect *prect,
		                  XWPSDeviceColor *pdcolor,
		                  XWPSClipPath *pcpath, 
		                  XWPSImageEnumCommon **pinfo,
		                  XWPSDevice **pmcdev, 
		                  XWPSDevice **midev,
		                  XWPSImageEnumCommon **pminfo,
		                  XWPSIntPoint * ,
		                  XWPSImage3X *pim)
{
  int code;
  XWPSDVIImageEnum *pmie;
  XWPSDVIImageEnum *pmce;
  XWDict *pmcs;
  int i;
  XWPSImage3xMask *pixm;

  if (midev[0]) 
  {
		if (midev[1])
	    return (int)(XWPSError::RangeCheck);
		i = 0, pixm = &pim->Opacity;
  } 
  else if (midev[1])
		i = 1, pixm = &pim->Shape;
  else
		return (int)(XWPSError::RangeCheck);
			
  code = dvi_make_mxd(pmcdev, midev[i]);
  if (code < 0)
		return code;
		
	XWPSDeviceDVI * pdev = (XWPSDeviceDVI*)dev;
  code = pdev->beginTypedImage(pis, pmat, pic, prect, pdcolor, pcpath, pinfo, PDF_IMAGE_TYPE3_DATA);
  if (code < 0)
		return code;
    
  if ((*pinfo)->procs.plane_data_.plane_datadvi != &XWPSDVIImageEnum::dviImagePlaneData)
  {
		(*pinfo)->end(false);
		delete *pinfo;
		*pinfo = 0;
		return (int)(XWPSError::RangeCheck);
  }
  pmie = (XWPSDVIImageEnum *)pminfo[i];
  pmce = (XWPSDVIImageEnum *)(*pinfo);
  if (pmce->writer->pres->object.isStream())
  	pmcs = pmce->writer->pres->object.streamGetDict();
  else
  	pmcs = pmce->writer->pres->object.getDict();
  
  if (pixm->has_Matte) 
  {
  	XWObject a, obj;
		int num_components = pim->ColorSpace->numComponents();
		XWDict * pcd = 0;
		if (pmie->writer->pres->object.isStream())
  		pcd = pmie->writer->pres->object.streamGetDict();
  	else
  		pcd = pmie->writer->pres->object.getDict();

		a.initArray(pdev->xref);
		for (int i = 0; i < num_components; i++)
		{
			obj.initReal(pixm->Matte[i]);
			a.arrayAdd(&obj);
		}
		pcd->add(qstrdup("Matte"), &a);
  }
  
  pmcs->add(qstrdup("SMask"), &(pmie->writer->pres->indirect));  
  return 0;
}

const PSParamItem dvi_param_items[] = {
	{"PDFEndPage", ps_param_type_int, _OFFSET_OF_(XWPSDeviceDVI,EndPage)},
{"PDFStartPage", ps_param_type_int, _OFFSET_OF_(XWPSDeviceDVI,StartPage)},
{"Optimize", ps_param_type_bool, _OFFSET_OF_(XWPSDeviceDVI,Optimize)},
{"ParseDSCCommentsForDocInfo", ps_param_type_bool,  _OFFSET_OF_(XWPSDeviceDVI,ParseDSCCommentsForDocInfo)},
{"ParseDSCComments", ps_param_type_bool, _OFFSET_OF_(XWPSDeviceDVI,ParseDSCComments)},
{"EmitDSCWarnings", ps_param_type_bool, _OFFSET_OF_(XWPSDeviceDVI,EmitDSCWarnings)},
{"CreateJobTicket", ps_param_type_bool, _OFFSET_OF_(XWPSDeviceDVI,CreateJobTicket)},
{"PreserveEPSInfo", ps_param_type_bool, _OFFSET_OF_(XWPSDeviceDVI,PreserveEPSInfo)},
{"AutoPositionEPSFiles", ps_param_type_bool, _OFFSET_OF_(XWPSDeviceDVI,AutoPositionEPSFiles)},
{"PreserveCopyPage", ps_param_type_bool, _OFFSET_OF_(XWPSDeviceDVI,PreserveCopyPage)},
{"UsePrologue", ps_param_type_bool, _OFFSET_OF_(XWPSDeviceDVI,UsePrologue)},

 {"ReAssignCharacters", ps_param_type_bool, _OFFSET_OF_(XWPSDeviceDVI,ReAssignCharacters)},
 {"ReEncodeCharacters", ps_param_type_bool, _OFFSET_OF_(XWPSDeviceDVI,ReEncodeCharacters)},
 {"FirstObjectNumber", ps_param_type_long, _OFFSET_OF_(XWPSDeviceDVI,FirstObjectNumber)},
 {0,0,0}
};


static void put_clamped(uchar *p, float v, int num_bytes)
{
  int limit = 1 << (num_bytes * 8);
  int i, shift;

  if (v <= -limit)
		i = -limit + 1;
  else if (v >= limit)
		i = limit - 1;
  else
		i = (int)v;
  for (shift = (num_bytes - 1) * 8; shift >= 0; shift -= 8)
		*p++ = (uchar)(i >> shift);
}

static void put_clamped_coord(uchar *p, float v, int num_bytes)
{
    put_clamped(p, (v + 32768) * 0xffffff / 65535, num_bytes);
}

static int put_float_mesh_data(XWObject *pscs, XWPSShadeCoordStream *cs,
		    int flag, int num_pts, int num_components,
		    bool is_indexed)
{
  uchar b[1 + (3 + 3) * 16];	/* flag + x + y or c */
  XWPSFixedPoint pts[16];
  int i;
  int code;

  b[0] = (uchar)flag;		/* may be -1 */
  if ((code = cs->nextCoords(pts, num_pts)) < 0)
		return code;
  for (i = 0; i < num_pts; ++i) 
  {
		put_clamped_coord(b + 1 + i * 6, fixed2float(pts[i].x), 3);
		put_clamped_coord(b + 4 + i * 6, fixed2float(pts[i].y), 3);
  }
  pscs->streamAdd((const char*)(b + (flag < 0)), (flag >= 0) + num_pts * 6);
  for (i = 0; i < num_components; ++i) 
  {
		float c;

		cs->getDecoded(0, NULL, &c);
		put_clamped(b, (is_indexed ? c + 32768 : (c + 256) * 65535 / 511), 2);
		pscs->streamAdd((const char*)&b[0], 2);
  }
  return 0;
}

  
XWPSDeviceDVI::XWPSDeviceDVI(int w, int h, QObject * parent)
	:XWPSDevicePsdf("pdfwrite", 
		                w * X_DPI / 10,
		                h * Y_DPI / 10,
		                X_DPI, 
		                Y_DPI,
		                3, 
		                24, 
		                255, 
		                255, 
		                256, 
		                256,
		                psdf_version_ll3,
		              	false,
		                parent)
{
	procs.open__.opendvi = &XWPSDeviceDVI::openDVI;
	procs.get_initial_matrix_.get_initial_matrix = &XWPSDevice::getInitialMatrixUpright;
	procs.output_page_.output_pagedvi = &XWPSDeviceDVI::outputPageDVI;
	procs.close__.closedvi = &XWPSDeviceDVI::closeDVI;
	procs.map_rgb_color_.map_rgb_color = &XWPSDevice::mapRGBColorRGBDefault;
	procs.map_color_rgb_.map_color_rgb = &XWPSDevice::mapColorRGBRGBDefault;
	procs.fill_rectangle_.fill_rectangledvi = &XWPSDeviceDVI::fillRectangleDVI;
	procs.copy_mono_.copy_monodvi = &XWPSDeviceDVI::copyMonoDVI;
	procs.copy_color_.copy_colordvi = &XWPSDeviceDVI::copyColorDVI;
	procs.get_params_.get_paramsdvi = &XWPSDeviceDVI::getParamsDVI;
	procs.put_params_.put_paramsdvi = &XWPSDeviceDVI::putParamsDVI;
	procs.get_page_device_.get_page_device = &XWPSDevice::getPageDevicePageDevice;
	procs.fill_path_.fill_pathdvi = &XWPSDeviceDVI::fillPathDVI;
	procs.stroke_path_.stroke_pathdvi = &XWPSDeviceDVI::strokePathDVI;
	procs.fill_mask_.fill_maskdvi = &XWPSDeviceDVI::fillMaskDVI;
	procs.strip_tile_rectangle_.strip_tile_rectangledvi = &XWPSDeviceDVI::stripTileRectangleDVI;
	procs.begin_typed_image_.begin_typed_imagedvi = &XWPSDeviceDVI::beginTypedImageDVI;
	procs.text_begin_.text_begindvi = &XWPSDeviceDVI::textBeginDVI;
	
	vec_procs.setlinewidth_.setlinewidthdvi = &XWPSDeviceDVI::setLineWidthDVI;
	vec_procs.setlinecap_.setlinecapdvi = &XWPSDeviceDVI::setLineCapDVI;
	vec_procs.setlinejoin_.setlinejoindvi = &XWPSDeviceDVI::setLineJoinDVI;
	vec_procs.setmiterlimit_.setmiterlimitdvi = &XWPSDeviceDVI::setMiterLimitDVI;
	vec_procs.setdash_.setdashdvi = &XWPSDeviceDVI::setDashDVI;
	vec_procs.setflat_.setflatdvi = &XWPSDeviceDVI::setFlatDVI;
	vec_procs.setfillcolor_.setfillcolordvi = &XWPSDeviceDVI::setFillColorDVI;
	vec_procs.setstrokecolor_.setstrokecolordvi = &XWPSDeviceDVI::setStrokeColorDVI;
	vec_procs.dorect_.dorectdvi = &XWPSDeviceDVI::doRectDVI;
	vec_procs.moveto_.movetodvi = &XWPSDeviceDVI::moveToDVI;
	vec_procs.lineto_.linetodvi = &XWPSDeviceDVI::lineToDVI;
	vec_procs.curveto_.curvetodvi = &XWPSDeviceDVI::curveToDVI;
	vec_procs.closepath_.closepathdvi = &XWPSDeviceDVI::closePathDVI;
	vec_procs.endpath_.endpathdvi = &XWPSDeviceDVI::endPathDVI;
		
	core = 0;
	xref = 0;
	dev = 0;
	
	EndPage = -1;
	StartPage = 1;
	Optimize = true;
	ParseDSCCommentsForDocInfo = false;
	ParseDSCComments = true;
	EmitDSCWarnings = false;
	CreateJobTicket = false;
	PreserveEPSInfo = false;
	AutoPositionEPSFiles = true;
	PreserveCopyPage = true;
	UsePrologue = false;
	ReAssignCharacters = true;
	ReEncodeCharacters = true;
	FirstObjectNumber = 1;
	init();
}

XWPSDeviceDVI::~XWPSDeviceDVI()
{
	XWPSDVIGraphicsSave * cg = open_graphics;
	while (cg)
	{
		XWPSDVIGraphicsSave * t = cg->prev;
		delete cg;
		cg = t;
	}
	
	if (articles != 0) 
	{
		XWPSDVIArticle *part;

		while ((part = articles) != 0) 
		{
	    articles = part->next;
	    delete part;
		}
  }
  
  if (last_resource != 0)
  {
  	XWPSDVIResource *pres;
		XWPSDVIResource *prev;

		for (prev = last_resource; (pres = prev) != 0;) 
		{
	  	prev = pres->prev;
	  	delete pres;
		}
		last_resource = 0;
	}
	
	if (pages)
	{
		for (int i = 0; i < num_pages; i++)
		{
			if (pages[i].xobjLabels)
			{
				free(pages[i].xobjLabels);
				pages[i].xobjLabels = 0;
			}
			if (pages[i].csLabels)
			{
				free(pages[i].csLabels);
				pages[i].csLabels = 0;
			}
			if (pages[i].gsLabels)
			{
				free(pages[i].gsLabels);
				pages[i].gsLabels = 0;
			}
			if (pages[i].patternLabels)
			{
				free(pages[i].patternLabels);
				pages[i].patternLabels = 0;
			}
			if (pages[i].shadingLabels)
			{
				free(pages[i].shadingLabels);
				pages[i].shadingLabels = 0;
			}
			if (pages[i].functionLabels)
			{
				free(pages[i].functionLabels);
				pages[i].functionLabels = 0;
			}
		}
		
		free(pages);
		pages = 0;
	}
	
	QHash<QString, XWPSDVILabel*>::iterator ifnt = fontLabels.begin();
	while (ifnt != fontLabels.end())
	{
		XWPSDVILabel * fl = ifnt.value();
		ifnt = fontLabels.erase(ifnt);
		if (fl)
			delete fl;
	}
	
	QHash<QString, XWPSDVILabel*>::iterator ifntd = fontDesLabels.begin();
	while (ifntd != fontDesLabels.end())
	{
		XWPSDVILabel * fdl = ifntd.value();
		ifntd = fontDesLabels.erase(ifntd);
		if (fdl)
			delete fdl;
	}
}

int XWPSDeviceDVI::appendChars(const uchar * str, uint size)
{
	const uchar *p = str;
  uint left = size;

  while (left)
		if (text.buffer_count == max_text_buffer) 
		{
	    int code = openPage(PDF_IN_TEXT);

	    if (code < 0)
				return code;
		} 
		else 
		{
	    int code = openPage(PDF_IN_STRING);
	    uint copy;

	    if (code < 0)
				return code;
	    copy = qMin((uint)(max_text_buffer - text.buffer_count), left);
	    memcpy(text.buffer + text.buffer_count, p, copy);
	    text.buffer_count += copy;
	    p += copy;
	    left -= copy;
		}
  return 0;
}

int  XWPSDeviceDVI::beginTypedImage(XWPSImagerState * pis,
		                   XWPSMatrix *pmat, 
		                   XWPSImageCommon *pic,
		                   XWPSIntRect * prect,
		                   XWPSDeviceColor * pdcolor,
		                   XWPSClipPath * pcpath, 
		                   XWPSImageEnumCommon ** pinfo,
		                   PSPDFTypedImageContext contextA)
{
	XWPSPixelImage *pim;
  int code;
  XWPSDVIImageEnum *pie;
  PSImageFormat format;
  XWPSColorSpace *pcs;
  XWPSColorSpace cs_gray_temp;
  cs_gray_temp.i_ctx_p = context_state;
  XWObject cs_value;
  int num_components;
  bool is_mask = false, in_line = false;
  XWPSIntRect rect;
  XWPSMatrix ctm;
  
  union iu_ 
  {
		XWPSPixelImage * pixel;
		XWPSImage1 * type1;
		XWPSImage3 * type3;
		XWPSImage3X * type3x;
		XWPSImage4 * type4;
  } image;
  ulong nbytes;
  int widthA, heightA;
  
  switch (pic->type.index)
  {
  	case 1: 
  		{
				XWPSImage1 *pim1 = (XWPSImage1 *)pic;

				if (pim1->Alpha != ps_image_alpha_none)
	    		goto nyi;
				is_mask = pim1->ImageMask;
				in_line = contextA == PDF_IMAGE_DEFAULT && canWriteImageInLine(pim1);
				image.type1 = pim1;
				break;
    	}
    	
    case 3: 
    	{
				XWPSImage3 *pim3 = (XWPSImage3 *)pic;

				if (CompatibilityLevel < 1.3)
	    		goto nyi;
				if (prect && !(prect->p.x == 0 && prect->p.y == 0 &&
		       	prect->q.x == pim3->Width && prect->q.y == pim3->Height))
	    		goto nyi;
	    		
				return pim3->beginImage3Generic(this, pis, pmat, prect, pdcolor, pcpath, dvi_image3_make_mid, dvi_image3_make_mcde, pinfo);
    	}
    	
    case IMAGE3X_IMAGETYPE: 
    	{
				XWPSImage3X *pim3x = (XWPSImage3X *)pic;

				if (CompatibilityLevel < 1.4)
	    		goto nyi;
				if (prect && !(prect->p.x == 0 && prect->p.y == 0 &&
		       	prect->q.x == pim3x->Width && prect->q.y == pim3x->Height))
	    		goto nyi;
				return pim3x->beginImage3xGeneric(this, pis, pmat, prect, pdcolor, pcpath,dvi_image3x_make_mid,	dvi_image3x_make_mcde, pinfo);
    	}
    	
    case 4:
			if (CompatibilityLevel < 1.3)
	    	goto nyi;
			image.type4 = (XWPSImage4 *)pic;
			break;
			
    default:
			goto nyi;
  }
  
  pim = (XWPSPixelImage *)pic;
  format = pim->format;
  switch (format)
  {
  	case ps_image_format_chunky:
    case ps_image_format_component_planar:
			break;
			
    default:
			goto nyi;
  }
  
  pcs = pim->ColorSpace;
  num_components = (is_mask ? 1 : pcs->numComponents());
  	
	code = openPage(PDF_IN_STREAM);
  if (code < 0)
		return code;
  if (contextA == PDF_IMAGE_TYPE3_MASK)
  {
  	cs_gray_temp.initDeviceGray();
		pcs = &cs_gray_temp;
  }
  else if (is_mask)
		code = prepareImageMask(pis, pdcolor);
  else
		code = prepareImage(pis);
  if (code < 0)
		goto nyi;
    
  if (prect)
		rect = *prect;
  else 
  {
		rect.p.x = rect.p.y = 0;
		rect.q.x = pim->Width, rect.q.y = pim->Height;
  }
  
  pie = new XWPSDVIImageEnum;
  *pinfo = pie;
  pie->commonInit(pim, this, num_components, format);
  if (contextA == PDF_IMAGE_TYPE3_MASK)
  	pie->procs.end_image_.end_imagedvi = &XWPSDVIImageEnum::dviImageEndImageObject;
  else
  	pie->procs.end_image_.end_imagedvi = &XWPSDVIImageEnum::dviImageEndImage;
  widthA = rect.q.x - rect.p.x;
  pie->width = widthA;
  heightA = rect.q.y - rect.p.y;
  pie->bits_per_pixel =	pim->BitsPerComponent * num_components / pie->num_planes;
  pie->rows_left = heightA;
  nbytes = (((ulong) pie->width * pie->bits_per_pixel + 7) >> 3) * pie->num_planes * pie->rows_left;
  in_line &= nbytes <= MAX_INLINE_IMAGE_BYTES;
  if (rect.p.x != 0 || rect.p.y != 0 ||
			rect.q.x != pim->Width || rect.q.y != pim->Height ||
			(is_mask ? pim->CombineWithColor : 
				colorSpace(&cs_value, pcs, (in_line ? &pdf_color_space_names_short :  &pdf_color_space_names), in_line) < 0)) 
	{
		delete pie;
		*pinfo = 0;
		goto nyi;
  }
  putClipPath(pcpath);
  pis->ctmOnly(&ctm);
  if (pmat == 0)
		pmat = &ctm;
  {
		XWPSMatrix mat;
		XWPSMatrix bmat;
		int code;

		bmat.makeBitmapMatrix(rect.p.x, -rect.p.y, pim->Width, pim->Height, heightA);
		if ((code = mat.matrixInvert(&pim->ImageMatrix)) < 0 ||
	    	(code = mat.matrixMultiply(&bmat, &mat)) < 0 ||
	    	(code = pie->mat.matrixMultiply(&mat, pmat)) < 0) 
	  {
	  	cs_value.free();
	    delete pie;
	    *pinfo = 0;
	    return code;
		}
  }
  
  if ((code = beginWriteImage(pie->writer, ps_no_id, widthA, heightA, NULL, in_line)) < 0 ||
			(code = pie->writer->binary.setupImageFilters(this, image.pixel, pmat, pis)) < 0 ||
			(code = beginImageData(pie->writer, image.pixel, &cs_value)) < 0)
	{
		cs_value.free();
		return code;
	}
  return 0;
 
nyi:
	cs_value.free();
  return beginTypedImageDefault(pis, pmat, pic, prect, pdcolor, pcpath, pinfo);
}

int XWPSDeviceDVI::beginTypedImageDVI(XWPSImagerState * pis,
			                        XWPSMatrix *pmat, 
			                        XWPSImageCommon *pic,
			   											XWPSIntRect * prect,
			                        XWPSDeviceColor * pdcolor,
			                        XWPSClipPath * pcpath, 
			                        XWPSImageEnumCommon ** pinfo)
{
	return beginTypedImage(pis, pmat, pic, prect, pdcolor, pcpath, pinfo, PDF_IMAGE_DEFAULT);
}

int XWPSDeviceDVI::closeDVI()
{
	if (!core || !xref || !dev)
		return 0;
		
	if (core->lastClose())
	{
		if (!xref->isScanning())
		{
			closePage();
			while (!registeredFonts.isEmpty())
	   	{
		   	XWPSDVIFontDescriptor * pfd = registeredFonts.takeFirst();
		   	if (pfd)
			   	finalizeFontDescriptor(pfd);
	   	}
			writeFontResources();
			writeResourceObjectsLast(resourceFont);
	   	writeResourceObjectsLast(resourceCharProc);
	   	writeResourceObjectsLast(resourceFontDescriptor);
	   	while (!registeredStdFonts.isEmpty())
	   	{
		   	int i = registeredStdFonts.takeFirst();
		   	std_fonts[i].font = 0;
	   	}
	  }
	  
	   if (articles != 0) 
      {
  	    XWPSDVIArticle *part;
	    while ((part = articles) != 0) 
		  {
	      articles = part->next;
	      delete part;
		  }
		
		  articles = 0;
     }
  
  	  if (last_resource != 0)
  	 {
  		   XWPSDVIResource *pres;
			XWPSDVIResource *prev;

			for (prev = last_resource; (pres = prev) != 0;) 
			{
	  		prev = pres->prev;
	  		delete pres;
			}
			last_resource = 0;
		}
		
		init();
		
		for (int i = 0; i < NUM_RESOURCE_TYPES; i++)
		    resources[i].reset();
	}
  
	if (strm)
	{
		if (!(strm->decRef()))
			delete strm;
		strm = 0;
	}
	
	if (strmbuf)
	{
		delete [] strmbuf;
		strmbuf = 0;
	}
	
	context = PDF_IN_NONE;
	file = 0;
	pageIDs();
  return 0;
}

int XWPSDeviceDVI::closePathDVI(double, 
	                      double,
		                    double, 
		                    double, 
		                    PSPathType)
{
	if (canWriteStream())
	  stream()->pputs("h\n");
  return 0;
}

int XWPSDeviceDVI::curveToDVI(double x0, 
	                    double y0,
			                double x1, 
			                double y1, 
			                double x2, 
			                double y2,
			                double x3, 
			                double y3, 
			                PSPathType)
{
	if (!canWriteStream())
		return 0;
		
	if (x1 == x0 && y1 == y0)
		stream()->print("%g %g %g %g v\n", x2, y2, x3, y3);
  else if (x3 == x2 && y3 == y2)
		stream()->print("%g %g %g %g y\n", x1, y1, x2, y2);
  else
		stream()->print("%g %g %g %g %g %g c\n", x1, y1, x2, y2, x3, y3);
  return 0;
}

int XWPSDeviceDVI::copyColorDVI(const uchar * base, 
	                 int sourcex,
		               int raster, 
		               ulong id, 
		               int x, 
		               int y, 
		               int w, 
		               int h)
{
	XWPSImage1 image;
  XWPSDVIImageWriter writer;
  int code;
    
  if (w <= 0 || h <= 0)
		return 0;
		
	code = openPage(PDF_IN_STREAM);
  if (code < 0)
		return code;
  
  putClipPath(NULL);
  code = copyColorData(base, sourcex, raster, id, x, y, w, h, &image, &writer, 0);
  switch (code) 
  {
		default:
	    return code;
	    
		case 1:
	    return 0;
	    
		case 0:
	    return doImage(writer.pres, NULL);
  }
}

int  XWPSDeviceDVI::copyDevice(XWPSDevice **pnew)
{
	XWPSDeviceDVI * ret = new XWPSDeviceDVI(width, height);
	ret->copyDeviceParamDVI(this);
	*pnew = ret;
	return 0;
}

void XWPSDeviceDVI::copyDeviceParamDVI(XWPSDeviceDVI * proto)
{
	copyDeviceParamPSDF(proto);
	core = proto->core;
	xref = proto->xref;
	dev = proto->dev;
	
	CompatibilityLevel = proto->CompatibilityLevel;
	EndPage = proto->EndPage;
  StartPage = proto->StartPage;
  Optimize = proto->Optimize;
  ParseDSCCommentsForDocInfo = proto->ParseDSCCommentsForDocInfo;
  ParseDSCComments = proto->ParseDSCComments;
  EmitDSCWarnings = proto->EmitDSCWarnings;
  CreateJobTicket = proto->CreateJobTicket;
  PreserveEPSInfo = proto->PreserveEPSInfo;
  AutoPositionEPSFiles = proto->AutoPositionEPSFiles;
  PreserveCopyPage = proto->PreserveCopyPage;
  UsePrologue = proto->UsePrologue;
  ReAssignCharacters = proto->ReAssignCharacters;
  ReEncodeCharacters = proto->ReEncodeCharacters;
  FirstObjectNumber = proto->FirstObjectNumber;
  fill_overprint = proto->fill_overprint;
  stroke_overprint = proto->stroke_overprint;
  overprint_mode = proto->overprint_mode;
  halftone_id = proto->halftone_id;
  memcpy(transfer_ids, proto->transfer_ids, 4 * sizeof(ulong));
  black_generation_id = proto->black_generation_id;
  undercolor_removal_id = proto->undercolor_removal_id;
  
	compression = proto->compression;
	  
  use_open_font = proto->use_open_font;	
  embedded_encoding_id = proto->embedded_encoding_id;
  text_rotation = proto->text_rotation;
  outline_depth = proto->outline_depth;
  closed_outline_depth = proto->closed_outline_depth;
  outlines_open = proto->outlines_open;
}

int XWPSDeviceDVI::copyMonoDVI(const uchar * base, 
	                int sourcex, 
	                int raster, 
	                ulong id,
		   						int x, 
		   						int y, 
		   						int w, 
		   						int h, 
		   						ulong zero,
		   						ulong one)
{
	if (w <= 0 || h <= 0)
		return 0;
  return copyMono(base, sourcex, raster, id, x, y, w, h, zero, one, NULL);
}

int XWPSDeviceDVI::createPDFFont(XWPSFont *font, 
	                  XWPSMatrix *pomat,
		                XWPSDVIFont **pppf)
{
	int index = -1;
  XWPSDVIFont ftemp;
  int BaseEncoding = ENCODING_INDEX_UNKNOWN;
  int same = 0, base_same = 0;
  PSPDFFontEmbed embed =fontEmbedStatus(font, &index, &same);
  bool have_widths = false;
  bool is_standard = false;
  long ffid = 0;
  XWPSDVIFontDescriptor *pfd = 0;
  XWPSFont *base_font = font;
  XWPSFont *below = 0;
  XWPSDVIFontDescriptor fdesc;
  XWPSDVIFont *ppf = 0;
  int code;
#define BASE_UID(fnt) (((XWPSFontBase *)(fnt))->UID)

	while ((below = base_font->base) != base_font && base_font->sameFont(below, FONT_SAME_OUTLINES))
		base_font = below;

set_base:
	if (base_font == font)
		base_same = same;
  else
		embed = fontEmbedStatus(base_font, &index, &base_same);
  if (embed == FONT_EMBED_STANDARD && std_fonts[index].font != 0)
  {
  	base_font = std_fonts[index].font;
		is_standard = true;
  }
  else if (embed == FONT_EMBED_YES &&
	       base_font->FontType != ft_composite &&
	       BASE_UID(base_font).isValid() &&
	       !base_font->is_resource)
	{
		XWPSFont *orig = base_font->dir->orig_fonts;

		for (; orig; orig = orig->next)
	    if (orig != base_font && orig->FontType == base_font->FontType &&
					orig->is_resource && BASE_UID(base_font).equal(&BASE_UID(orig))) 
			{
				base_font = orig;
				goto set_base;
	    }
	}
	
	pfd = (XWPSDVIFontDescriptor *)findResourceByPsId(resourceFontDescriptor, base_font->id);
  if (pfd != 0 && pfd->base_font != base_font)
		pfd = 0;
		
	switch (embed)
	{
		case FONT_EMBED_YES:
			if (font->FontType == ft_TrueType && CompatibilityLevel <= 1.2) 
			{
	    	int i;

	    	for (i = 0; i <= 0xff; ++i) 
	    	{
					ulong glyph = font->encodeChar(i,  GLYPH_SPACE_INDEX);

					if (glyph == ps_no_glyph ||
		    			(glyph >= ps_min_cid_glyph && glyph <= ps_min_cid_glyph + 0xff))
		    		continue;
		
					return (int)(XWPSError::RangeCheck);
	    	}
			}
			code = computeFontDescriptor(&fdesc, font, NULL);
			if (code < 0)
	    	return code;
	    if (!pfd)
	    	ffid = 1;
			goto wf;
			
    case FONT_EMBED_NO:
    	BaseEncoding = ((XWPSFontBase *)base_font)->nearest_encoding_index;
			switch (BaseEncoding) 
			{
				default:
	    		BaseEncoding = ENCODING_INDEX_WINANSI;
				case ENCODING_INDEX_WINANSI:
				case ENCODING_INDEX_MACROMAN:
				case ENCODING_INDEX_MACEXPERT:
	    		break;
			}
			code = computeFontDescriptor(&fdesc, font, NULL);
			if (code < 0)
	    	return code;
	    	
wf:
		  same &= ~FONT_SAME_METRICS;
		  
		case FONT_EMBED_STANDARD:
			if (~same & (FONT_SAME_METRICS | FONT_SAME_ENCODING))
			{
				int i;

	    	for (i = 0; i <= 255; ++i) 
	    	{
					code = ftemp.charWidth(i, font, NULL);
					if (code < 0 && code != XWPSError::Undefined)
		    		return code;
	    	}
	    	have_widths = true;
			}
			if (pfd) 
			{
	    	code = allocFont(font->id, &ppf, NULL);
	    	if (code < 0)
					return code;
	    	ppf->FontDescriptor = pfd;
			}
			else
			{
				int name_index = index;

	    	fdesc.rid = base_font->id;
	    	switch (base_font->FontType) 
	    	{
	    		case ft_CID_encrypted:
						fdesc.chars_used.size = (((XWPSFontCid0 *)base_font)->CIDCount + 7) >> 3;
						break;
						
	    		case ft_CID_TrueType:
						fdesc.chars_used.size = (((XWPSFontCid2 *)base_font)->CIDCount + 7) >> 3;
						break;
						
	    		default:
						fdesc.chars_used.size = 256/8;
	    	}
	    	code = allocFont(font->id, &ppf, &fdesc);
	    	if (code < 0)
					return code;
	    	pfd = ppf->FontDescriptor;
	    	if (index < 0)
	    	{
	    		int ignore_same;

					memcpy(pfd->FontName.chars, base_font->font_name.chars, base_font->font_name.size);
					pfd->FontName.size = base_font->font_name.size;
					if (ffid)
					{
						pfd->FontFile = new XWPSDVIFontFile;
						pfd->FontFile->object.initStream(0, xref);
						xref->refObj(&pfd->FontFile->object, &pfd->FontFile->indirect);
						int l = 1024;
						uchar * buf = (uchar*)malloc(l * sizeof(uchar));
						pfd->FontFile->strm = new XWPSStream;
						pfd->FontFile->strm->close_file = false;
						pfd->FontFile->strm->writeFile(pfd->FontFile->object.streamGetIO(), buf, l, true, false);
					}
					pfd->base_font = base_font;
					if (pfd->base_font)
						pfd->base_font->incRef();
					pfd->orig_matrix = *pomat;
					fontEmbedStatus(base_font, &name_index, &ignore_same);
	    	}
	    	else
	    	{
	    		const PSPDFStandardFont *ppsf = &pdf_standard_fonts[index];
					const char *fnchars = ppsf->fname;
					uint fnsize = strlen(fnchars);

					memcpy(pfd->FontName.chars, fnchars, fnsize);
					pfd->FontName.size = fnsize;
					memset(&pfd->values, 0, sizeof(&pfd->values));
	    	}
	    	if (!is_standard) 
	    	{
					code = adjustFontName(pfd, name_index >= 0);
					if (code < 0)
		    		return code;
	    	}
			}
			
			ppf->FontType = font->FontType;
			ppf->index = index;
			switch (font->FontType)
			{
				case ft_encrypted:
				case ft_encrypted2:
	    		ppf->is_MM_instance =	((XWPSFontType1 *)font)->WeightVector.count > 0;
				default:
	    		;
			}
			
			ppf->BaseEncoding = (PSEncodingIndex)BaseEncoding;
			ppf->fname = pfd->FontName;
			ppf->font = font;
			if (ppf->font)
				ppf->font->incRef();
			if (~same & FONT_SAME_METRICS)
			{
				ppf->write_Widths = true;
	    	font->findCharRange(&ppf->FirstChar, &ppf->LastChar);
			}
			
			if (have_widths)
			{
				 memcpy(ppf->Widths, ftemp.Widths, sizeof(ppf->Widths));
	    	memcpy(ppf->widths_known, ftemp.widths_known, sizeof(ppf->widths_known));
			}
			
			registeredFonts << ppf->FontDescriptor;
	}
	
	checkFont(ppf);
	checkFontDescriptor(pfd);
	*pppf = ppf;
  return code;
}

int XWPSDeviceDVI::doImage(XWPSDVIResource * pres, XWPSMatrix * pimat)
{
	int code = openContents(PDF_IN_STREAM);
	if (code < 0)
	   return code;
	    
	if (pimat) 
  {
		XWPSDVIXObjects * pxo = (XWPSDVIXObjects *)pres;
		double scale = (double)pxo->data_height / pxo->height;

		putImageMatrix(pimat, scale);
  }
  if (canWriteStream())
    strm->print(" /R%ld Do\nQ\n", pres->resourceId());
  return 0;
}

int XWPSDeviceDVI::doRectDVI(long x0, long y0, long x1, long y1, PSPathType type)
{
	long xmax = int2fixed(width), ymax = int2fixed(height);
  long xmin = 0, ymin = 0;
  if (type & ps_path_type_stroke) 
  {
		double w = state->line_params.half_width;
		double xw = w * (fabs(state->ctm.xx) + fabs(state->ctm.yx));
		double yw = w * (fabs(state->ctm.xy) + fabs(state->ctm.yy));

		xmin = -(float2fixed(xw) + fixed_1);
		xmax -= xmin;
		ymin = -(float2fixed(yw) + fixed_1);
		ymax -= ymin;
  }
  
  if (!(type & ps_path_type_clip) &&
			(x0 > xmax || x1 < xmin || y0 > ymax || y1 < ymin ||
	 		x0 > x1 || y0 > y1))
		return 0;
		
	if (x0 < xmin)
		x0 = xmin;
  if (x1 > xmax)
		x1 = xmax;
  if (y0 < ymin)
		y0 = ymin;
  if (y1 > ymax)
		y1 = ymax;
		
	if (!canWriteStream())
		return 0;
  return doRectPSDF(x0, y0, x1, y1, type);
}

int XWPSDeviceDVI::encodeChar(int chr, XWPSFontBase *bfont,	XWPSDVIFont *ppf)
{
	XWPSDVIFontDescriptor * pfd = ppf->FontDescriptor;
	PSEncodingIndex ei = bfont->encoding_index;
	XWPSFont *base_font = pfd->base_font;
  bool have_font = base_font != 0 && base_font->FontType != ft_composite;
  bool is_standard = ppf->index != ENCODING_INDEX_UNKNOWN;
  PSEncodingIndex bei =	(ppf->BaseEncoding != ENCODING_INDEX_UNKNOWN ? ppf->BaseEncoding :
	 											is_standard ? pdf_standard_fonts[ppf->index].base_encoding :
	 											ENCODING_INDEX_UNKNOWN);
  XWPSPDFEncodingElement *pdiff = ppf->Differences;
  ulong font_glyph, glyph;
#define IS_USED(c)\
  (((pfd)->chars_used.data[(c) >> 3] & (1 << ((c) & 7))) != 0)

  if (ei == bei && ei != ENCODING_INDEX_UNKNOWN && pdiff == 0)
  {
  	pfd->recordUsed(chr);
		return chr;
  }
  
  if (!is_standard && !have_font)
		return (int)(XWPSError::Undefined);
			
#define ENCODE_NO_DIFF(ch)\
   (bei != ENCODING_INDEX_UNKNOWN ?\
    bfont->knownEncode((ch), bei) :\
    base_font->encodeChar(chr, GLYPH_SPACE_NAME))
#define HAS_DIFF(ch) (pdiff != 0 && pdiff[ch].data != 0)
#define ENCODE_DIFF(ch) (pdiff[ch].glyph)
#define ENCODE(ch)\
  (HAS_DIFF(ch) ? ENCODE_DIFF(ch) : ENCODE_NO_DIFF(ch))
  	
  font_glyph = ENCODE(chr);
  glyph =	(ei == ENCODING_INDEX_UNKNOWN ?	 bfont->encodeChar(chr, GLYPH_SPACE_NAME) :	bfont->knownEncode(chr, ei));
  if (glyph == font_glyph) 
  {
		pfd->recordUsed(chr);
		return chr;
  }
	
	if (bfont->FontType == ft_TrueType) 
	{
		if (ReEncodeCharacters) 
		{
	    int c;

	    for (c = 0; c < 256; ++c)
				if (ENCODE_NO_DIFF(c) == glyph) 
				{
		    	pfd->recordUsed(c);
		    	return c;
				}
		}
		return (int)(XWPSError::Undefined);
  }
    
  if (ReAssignCharacters)
  {
  	if (!HAS_DIFF(chr) && !IS_USED(chr))
  	{
  		int code =	addEncodingDifference(ppf, chr, bfont, glyph);
  		if (code >= 0)
  		{
  			int c;

				for (c = 0; c < 256; ++c)
		    	if (ENCODE_NO_DIFF(c) == glyph)
						break;
				if (c < 256)
		    	pfd->recordUsed(c);
				else
		    	pfd->subset_ok = false;
				return chr;
  		}
  	}
  }
  
  if (ReEncodeCharacters)
  {
  	int c, code;

		for (c = 0; c < 256; ++c) 
		{
	    if (HAS_DIFF(c)) 
	    {
				if (ENCODE_DIFF(c) == glyph)
		    	return c;
	    } 
	    else if (ENCODE_NO_DIFF(c) == glyph) 
	    {
				pfd->recordUsed(c);
				return c;
	    }
		}
		
		for (c = 0; c < 256; ++c)
		{
			XWPSString gnstr;

	    if (HAS_DIFF(c) || IS_USED(c))
				continue;
	    font_glyph = ENCODE_NO_DIFF(c);
	    if (font_glyph == ps_no_glyph)
				break;
	    gnstr.data = (uchar *)bfont->glyphName(font_glyph, &gnstr.size);
	    if (gnstr.size == 7 &&	!memcmp(gnstr.data, ".notdef", 7))
				break;
		}
		
		if (c == 256)
			return (int)(XWPSError::Undefined);
				
		code = addEncodingDifference(ppf, c, bfont, glyph);
		if (code < 0)
	    return code;
	    
	  pfd->subset_ok = false;
		return c;
  }
  
  return (int)(XWPSError::Undefined);

#undef IS_USED
#undef ENCODE_NO_DIFF
#undef HAS_DIFF
#undef ENCODE_DIFF
#undef ENCODE
}

int XWPSDeviceDVI::endImageBinary(XWPSDVIImageWriter *piw, int data_h)
{
	int code = 0;
	piw->binary.endBinary();
	if (data_h != piw->height)
	{
		XWObject obj;
		obj.initInt(data_h);
		piw->dict->add(qstrdup(piw->pin->Height + 1), &obj);
	}
  return code;
}

int XWPSDeviceDVI::endWriteImage(XWPSDVIImageWriter * piw)
{
	XWPSDVIResource *pres = piw->pres;
	if (pres) 
  {
		strm = piw->save_strm;
		iswritestrm = false;
		return 0;
  } 
  else 
  {
  	strm = piw->save_strm;
  	if (canWriteStream())
			writeImage(piw, strm);
		iswritestrm = false;
		piw->data.free();
		piw->data.initNull();
			
		return 1;
  }
}

int XWPSDeviceDVI::fillMaskDVI(const uchar * data, 
	                int data_x, 
	                int raster, 
	                ulong id,
		   						int x, 
		   						int y, 
		   						int widthA, 
		   						int heightA,
		   						XWPSDeviceColor * pdcolor, 
		   						int depth,
		   						ulong lop, 
		   						XWPSClipPath * pcpath)
{
	if (widthA <= 0 || heightA <= 0)
		return 0;
  if (depth > 1 || !pdcolor->isPure() != 0)
		return fillMaskDefault(data, data_x, raster, id, x, y, widthA, heightA, pdcolor, depth, lop,  pcpath);
  return copyMono(data, data_x, raster, id, x, y, widthA, heightA,ps_no_color_index, pdcolor->pureColor(), pcpath);
}

int XWPSDeviceDVI::fillPathDVI(XWPSImagerState * pis, 
	                XWPSPath * ppath,
		              XWPSFillParams * params,
	                XWPSDeviceColor * pdcolor, 
	                XWPSClipPath * pcpath)
{
	int code;
	if (pcpath) 
	{
		XWPSFixedRect box;

		pcpath->outerBox(&box);
		if (box.p.x >= box.q.x || box.p.y >= box.q.y)
	    return 0;
  }
  
  code = prepareFill(pis);
  if (code < 0)
		return code;
  if (pdcolor->isPure())
  {
  	if (pdcolor->pureColor() == white && !file)
	    return 0;
  }
  
  bool have_path = !ppath->isVoid();
  if (have_path || context == PDF_IN_NONE ||
			mustPutClipPath(pcpath)) 
	{
		code = openPage(PDF_IN_STREAM);
		if (code < 0)
	    return code;
  }
  putClipPath(pcpath);
  if (setFillColor(pdcolor) < 0)
		return fillPathDefault(pis, ppath, params, pdcolor, pcpath);
  if (have_path) 
  {
		XWPSStream *s = strm;
		double scaleA;
		XWPSMatrix smat;
		XWPSMatrix *psmat = NULL;

		if (params->flatness != state->flatness) 
		{
			if (canWriteStream())
	      s->print(" %g i\n", params->flatness);
	    state->flatness = params->flatness;
		}
		if (makePathScaling(ppath, &scaleA)) 
		{
	    smat.makeScaling(scale.x / scaleA, scale.y / scaleA);
	    psmat = &smat;
	    if (canWriteStream())
	      s->pputs(" q\n");
		}
		doPath(ppath, (PSPathType)(ps_path_type_fill | ps_path_type_optimize), psmat);
		if (canWriteStream())
		{
		  s->pputs((params->rule < 0 ? " f\n" : " f*\n"));
			if (psmat)
	    	s->pputs("Q\n");
	  }
  }
  return 0;
}

int XWPSDeviceDVI::fillRectangleDVI(int x, int y, int  w, int  h, ulong color)
{
	if (color == white && !file)
		return 0;
  		
  int code = openPage(PDF_IN_STREAM);
  if (code < 0)
		return code;
		
  putClipPath(NULL);
  setPureColor(color, fill_color, &psdf_set_fill_color_commands);
  if (canWriteStream())
    strm->print(" %d %d %d %d re f\n", x, y, w, h);
  return 0;
}

int XWPSDeviceDVI::findOrigFont(XWPSFont *font, XWPSMatrix *pfmat)
{
	bool scan = true;
  int i;

  if (font->FontType == ft_composite)
		return -1;
  for (;; font = font->base) 
  {
		XWPSFontBase *bfont = (XWPSFontBase *)font;
		int same;

		i = findStdAppearance(bfont, 0, &same);
		if (i >= 0)
	    break;
		if (scan) 
		{
	    bool found = scanForStandardFonts(font->dir);

	    scan = false;
	    if (found) 
	    {
				i = findStdAppearance(bfont, 0, &same);
				if (i >= 0)
		    	break;
	    }
		}
		if (font->base == font)
	    return -1;
  }
  *pfmat = std_fonts[i].orig_matrix;
  return i;
}

XWPSDVIResource * XWPSDeviceDVI::findResourceByPsId(PSPDFResourceType rtype, ulong rid)
{
	XWPSDVIResource **pchain = resourceChain(rtype, rid);
  XWPSDVIResource **pprev = pchain;
  XWPSDVIResource *pres;

  for (; (pres = *pprev) != 0; pprev = &pres->next)
		if (pres->rid == rid) 
		{
	    if (pprev != pchain) 
	    {
				*pprev = pres->next;
				pres->next = *pchain;
				*pchain = pres;
	    }
	    return pres;
		}
  return 0;
}

int  XWPSDeviceDVI::getParamsDVI(XWPSContextState * ctx, XWPSParamList *plist)
{
	float cl = (float)CompatibilityLevel;
  int code = getParamsPSDF(ctx, plist);
  int cdv = CoreDistVersion;

  if (code < 0 ||
			(code = plist->writeInt(ctx, "CoreDistVersion", &cdv)) < 0 ||
			(code = plist->writeFloat(ctx, "CompatibilityLevel", &cl)) < 0 ||
			(plist->requested(ctx, "pdfmark") > 0 &&
	 		(code = plist->writeNull(ctx, "pdfmark")) < 0) ||
			(plist->requested(ctx, "DSC") > 0 &&
	 		(code = plist->writeNull(ctx, "DSC")) < 0) ||
			(code = plist->writeItems(ctx, this, NULL, (PSParamItem*)&dvi_param_items[0])) < 0);
  return code;
}

void XWPSDeviceDVI::initDVI(XWDVICore * coreA,
	             XWDVIRef * xrefA,
	             XWDVIDev * devA)
{
	core = coreA;
	xref = xrefA;
	dev = devA;
	
	CompatibilityLevel = 1 + 0.1 * xref->getVersion();	
	if (CompatibilityLevel < 1.5)
		params.CompressPages = false;
	else
		params.CompressPages = true;
}

int XWPSDeviceDVI::lineToDVI(double, double, double x, double y, PSPathType)
{
	if (canWriteStream())
	  stream()->print("%g %g l\n", x, y);
  return 0;
}
	
int XWPSDeviceDVI::moveToDVI(double, double, double x, double y, PSPathType)
{
	if (canWriteStream())
	  stream()->print("%g %g m\n", x, y);
  return 0;
}

int XWPSDeviceDVI::openDVI()
{
	reset();
	scale.x = scale.y = 1.0;
	loadCache();
  fill_options = stroke_options = ps_path_type_optimize;
  in_page = true;
  num_pages = xref->currentPageNumber() + 10;
  pages = (XWPSDVIPage*)malloc(num_pages * sizeof(XWPSDVIPage));
  for (int i = 0; i < num_pages; i++)
  	pageReset(&pages[i]);
  return 0;
}

int  XWPSDeviceDVI::outputPageDVI(int , int )
{
	context = PDF_IN_NONE;
	return 0;
}

void XWPSDeviceDVI::putImageMatrix(XWPSMatrix * pmat, float y_scale)
{
	XWPSMatrix imat;

  imat.matrixTranslate(pmat, 0.0, 1.0 - y_scale);
  imat.matrixScale(&imat, 1.0, y_scale);
  if (canWriteStream())
    putMatrix(" q ", &imat, "cm\n");
}

int XWPSDeviceDVI::putParamsDVI(XWPSContextState * ctx, XWPSParamList *plist)
{
	int ecode, code;
  XWPSDeviceDVI save_dev(width, height);
  float cl = (float)CompatibilityLevel;
  bool locked = params.LockDistillerParams;
  const char * param_name;
  
  {
		PSParamStringArray ppa;

		code = plist->readStringArray(ctx, (param_name = "pdfmark"), &ppa);
		switch (code) 
		{
	    case 0:
				code = pdfmarkProcess(&ppa);
				if (code >= 0)
		    	return code;
		
	    default:
				plist->signalError(ctx, param_name, code);
				return code;
				
	    case 1:
				break;
		}

		code = plist->readStringArray(ctx, (param_name = "DSC"), &ppa);
		switch (code) 
		{
	    case 0:
		    return 0;
		    
	    default:
				plist->signalError(ctx, param_name, code);
				return code;
				
	    case 1:
				break;
		}
  }
  
  ecode = code = plist->readBool(ctx, "LockDistillerParams", &locked);
  if (locked && params.LockDistillerParams)
		return ecode;
		
	{
		int cdv = CoreDistVersion;

		ecode = plist->putInt(ctx, (param_name = "CoreDistVersion"), &cdv, ecode);
		if (cdv != CoreDistVersion)
		  plist->signalError(ctx, param_name, ecode = XWPSError::RangeCheck);
  }

  save_dev.copyDeviceParamDVI(this);
  switch (code = plist->readFloat(ctx, (param_name = "CompatibilityLevel"), &cl)) 
  {
		default:
	    ecode = code;
	    plist->signalError(ctx, param_name, ecode);
		case 0:
		case 1:
	    break;
  }
  
  code = plist->readItems(ctx, this, (PSParamItem*)&dvi_param_items[0]);
  if (code < 0)
		ecode = code;
	
	{
		static const char *const pcm_names[] = {
	    "DeviceGray", "DeviceRGB", "DeviceCMYK", 0};
		int pcm = -1;

		ecode = plist->putEnum(ctx, "ProcessColorModel", &pcm, pcm_names, ecode);
		if (pcm >= 0) 
		{
			switch (pcm)
			{
				case 0:
					color_info.num_components = 1;
					color_info.depth = 8;
					color_info.max_gray = 255;
					color_info.max_color = 0;
					color_info.dither_grays = 256;
					color_info.dither_colors = 0;
					break;
					
				case 1:
					color_info.num_components = 3;
					color_info.depth = 24;
					color_info.max_gray = 255;
					color_info.max_color = 255;
					color_info.dither_grays = 256;
					color_info.dither_colors = 256;
					break;
					
				case 2:
					color_info.num_components = 4;
					color_info.depth = 32;
					color_info.max_gray = 255;
					color_info.max_color = 255;
					color_info.dither_grays = 256;
					color_info.dither_colors = 256;
					break;
					
				default:
					break;
			}
			
	    setProcessColorModel();
		}
	}
	
	if (ecode < 0)
		goto fail;
		
	version = (cl < 1.2 ? psdf_version_level2 : psdf_version_ll3);
  ecode = putParamsPSDF(ctx, plist);
  if (ecode < 0)
		goto fail;
		
#define MAX_EXTENT 28000
  
  if (height > MAX_EXTENT || width > MAX_EXTENT) 
  {
		double factor =  qMax(height / (double)MAX_EXTENT,	width / (double)MAX_EXTENT);

		setResolution(HWResolution[0] / factor, HWResolution[1] / factor);
  }
#undef MAX_EXTENT
  
  CompatibilityLevel = (int)(cl * 10 + 0.5) / 10.0;
  return 0;
 
fail:
  version = save_dev.version;
  color_info = save_dev.color_info;
  setProcessColorModel();
  
  EndPage = save_dev.EndPage;
  StartPage = save_dev.StartPage;
  Optimize = save_dev.Optimize;
  ParseDSCCommentsForDocInfo = save_dev.ParseDSCCommentsForDocInfo;
  ParseDSCComments = save_dev.ParseDSCComments;
  EmitDSCWarnings = save_dev.EmitDSCWarnings;
  CreateJobTicket = save_dev.CreateJobTicket;
  PreserveEPSInfo = save_dev.PreserveEPSInfo;
  AutoPositionEPSFiles = save_dev.AutoPositionEPSFiles;
  PreserveCopyPage = save_dev.PreserveCopyPage;
  UsePrologue = save_dev.UsePrologue;
  ReAssignCharacters = save_dev.ReAssignCharacters;
  ReEncodeCharacters = save_dev.ReEncodeCharacters;
  FirstObjectNumber = save_dev.FirstObjectNumber;
  return ecode;
}

int XWPSDeviceDVI::setDashDVI(const float *pattern, uint count, double offset)
{
	if (!canWriteStream())
		return 0;
		
	XWPSStream *s = stream();
  s->pputs("[ ");
  for (uint i = 0; i < count; ++i)
		s->print("%g ", pattern[i]);
  s->print("] %g d\n", offset);
  return 0;
}

int XWPSDeviceDVI::setFillColorDVI(XWPSDeviceColor * pdc)
{
	return setDrawingColor(pdc, fill_color, &psdf_set_fill_color_commands);
}

int XWPSDeviceDVI::setFlatDVI(double flatness)
{
	if (canWriteStream())
	  stream()->print("%g i\n", flatness);
  return 0;
}

int XWPSDeviceDVI::setLineCapDVI(PSLineCap cap)
{
	if (canWriteStream())
	  stream()->print("%d J\n", cap);
  return 0;
}

int XWPSDeviceDVI::setLineJoinDVI(PSLineJoin join)
{
	if (canWriteStream())
	  stream()->print("%d j\n", join);
  return 0;
}

int XWPSDeviceDVI::setLineWidthDVI(double widthA)
{
	if (canWriteStream())
	  setLineWidthPSDF(fabs(widthA));
	return 0;
}

int XWPSDeviceDVI::setMiterLimitDVI(double limit)
{
	if (canWriteStream())
	  stream()->print("%g M\n", limit);
  return 0;
}

int XWPSDeviceDVI::setStrokeColorDVI(XWPSDeviceColor * pdc)
{
	if (!canWriteStream())
		return 0;
	return setDrawingColor(pdc, stroke_color, &psdf_set_stroke_color_commands);
}

int XWPSDeviceDVI::setTextMatrix(XWPSMatrix * pmat)
{
	XWPSStream *s = strm;
  double sx = 72.0 / HWResolution[0],	sy = 72.0 / HWResolution[1];
  int code;

  if (pmat->xx == text.matrix.xx &&
			pmat->xy == text.matrix.xy &&
			pmat->yx == text.matrix.yx &&
			pmat->yy == text.matrix.yy &&
			(context == PDF_IN_TEXT || context == PDF_IN_STRING)) 
	{
		XWPSPoint dist;
	
		dist.setTextDistance(&text.current, pmat);
		if (dist.y == 0 && dist.x >= X_SPACE_MIN &&
	    	dist.x <= X_SPACE_MAX &&
	    	text.font != 0 &&
	    	text.font->isSynthesized()) 
	  {
	    int dx = (int)dist.x;
	    int dx_i = dx - X_SPACE_MIN;
	    uchar space_char = text.font->spaces[dx_i];

	    if (space_char == 0) 
	    {
				if (text.font != open_font)
		    	goto not_spaces;
				code = assignCharCode();
				if (code <= 0)
		    	goto not_spaces;
				space_char = open_font->spaces[dx_i] = (uchar)code;
				if (space_char_ids[dx_i] == 0) 
				{
					XWObject ss, obj;
					ss.initStream(STREAM_COMPRESS, xref);
					xref->refObj(&ss, &obj);
					
		    	char spstr[3 + 14 + 1];

		    	sprintf(spstr, " %d 0 0 0 0 0 d1\n", dx);
		    	
		    	space_char_ids[dx_i] = obj.label;
		    	ss.streamAdd(spstr, strlen(spstr));
		    	if (!xref->isScanning())
						xref->releaseObj(&ss);
					else
						ss.free();		    	
				}
	    }
	    appendChars(&space_char, 1);
	    text.current.x += dx * pmat->xx;
	    text.use_leading = false;
	    return 0;
		}
    
not_spaces:
		code = openPage(PDF_IN_TEXT);
		if (code < 0)
	    return code;
		if (dist.x == 0 && dist.y < 0) 
		{
	    float dist_y = (float)-dist.y;

	    if (fabs(text.leading - dist_y) > 0.0005) 
	    {
	    	if (canWriteStream())
				  s->print(" %g TL\n", dist_y);
				text.leading = dist_y;
	    }
	    text.use_leading = true;
		} 
		else 
		{
	    dist.setTextDistance(&text.line_start, pmat);
	    if (canWriteStream())
	      s->print(" %g %g Td\n", dist.x, dist.y);
	    text.use_leading = false;
		}
  } 
  else 
  {
		code = openPage(PDF_IN_TEXT);
		if (code < 0)
	    return code;
	    
	  if (canWriteStream())
		  strm->print(" %g %g %g %g %g %g Tm\n", pmat->xx * sx, pmat->xy * sy,
		 						pmat->yx * sx, pmat->yy * sy, pmat->tx * sx, pmat->ty * sy);
		text.matrix = *pmat;
		text.use_leading = false;
  }
  text.line_start.x = pmat->tx;
  text.line_start.y = pmat->ty;
  text.current.x = pmat->tx;
  text.current.y = pmat->ty;
  return 0;
}

int XWPSDeviceDVI::stripTileRectangleDVI(XWPSStripBitmap * tiles,
			      								int x, 
			      								int y, 
			      								int w, 
			      								int h,
			                      ulong color0, 
			                      ulong color1,
			      								int px, 
			      								int py)
{
	int tw = tiles->rep_width, th = tiles->rep_height;
  double xscale = HWResolution[0] / 72.0, yscale = HWResolution[1] / 72.0;
  bool mask;
  int depth;
  int (XWPSDeviceDVI::*copy_data)(const uchar *, int, int, ulong, int, int, int, int, XWPSImage1 *, XWPSDVIImageWriter *, int);
  XWPSDVIResource *pres;
  XWObject cs_value;
  int code;

  if (tiles->id == ps_no_bitmap_id || tiles->shift != 0 ||
			(w < tw && h < th) ||	color0 != ps_no_color_index ||
			CompatibilityLevel < 1.2)
		goto use_default;
  
  if (color1 != ps_no_color_index) 
  {
		mask = true;
		depth = 1;
		copy_data = &XWPSDeviceDVI::copyMaskData;
		code = csPatternUnColored(&cs_value);
  } 
  else 
  {
		mask = false;
		depth = color_info.depth;
		copy_data = &XWPSDeviceDVI::copyColorData;
		code = csPatternColored(&cs_value);
  }
  
  if (code < 0)
		goto use_default;
  pres = findResourceByPsId(resourcePattern, tiles->id);
  if (!pres) 
  {
		int code;
		long image_id;		
		XWPSImage1 image;
		XWPSDVIImageWriter writer;
		
		long image_bytes = ((long)tw * depth + 7) / 8 * th;
		bool in_line = image_bytes <= MAX_INLINE_IMAGE_BYTES;
		ulong tile_id =  (tw == tiles->size.x && th == tiles->size.y ? tiles->id : ps_no_bitmap_id);

		if (in_line)
	    image_id = 0;
		else if (image_bytes > 65500) 
	    goto use_default;
		else 
		{
	    code = (this->*copy_data)(tiles->data, 0, tiles->raster, tile_id, 0, 0, tw, th, &image, &writer, 1);
	    if (code < 0)
				goto use_default;
	    image_id = writer.pres->resourceId();
		}
		code = beginResource(resourcePattern, tiles->id, &pres);
		if (code < 0)
	    goto use_default;
	    
	  XWObject obj, obj1, obj2, a, a1, a2;
	  XWDict * dict = pres->object.streamGetDict();
	  obj.initInt(1);
	  dict->add(qstrdup("PatternType"), &obj);
	  if (mask)
	  	obj.initInt(2);
	  dict->add(qstrdup("PaintType"), &obj);
	  obj.initInt(1);
	  dict->add(qstrdup("TilingType"), &obj);
	  obj1.initDict(xref);
	  if (image_id)
	  {
	  	obj2.initDict(xref);
	  	char buf[20];
	  	sprintf(buf, "R%ld", image_id);
	  	obj2.dictAdd(qstrdup(buf), &(writer.pres->indirect));
	  	obj1.dictAdd(qstrdup("XObject"), &obj2);
	  }
	  
	  a.initArray(xref);
	  obj.initName("PDF");
	  a.arrayAdd(&obj);
	  if (mask)
	  	obj.initName("ImageB");
	  else
	  	obj.initName("ImageC");
	  a.arrayAdd(&obj);
	  obj1.dictAdd(qstrdup("ProcSet"), &a);
	  dict->add(qstrdup("Resources"), &obj1);
	  
	  a1.initArray(xref);
	  obj.initReal(tw / xscale);
	  a1.arrayAdd(&obj);
	  obj.initReal(0);
	  a1.arrayAdd(&obj);
	  obj.initReal(0);
	  a1.arrayAdd(&obj);
	  obj.initReal(th / yscale);
	  a1.arrayAdd(&obj);
	  obj.initReal(0);
	  a1.arrayAdd(&obj);
	  obj.initReal(0);
	  a1.arrayAdd(&obj);
	  dict->add(qstrdup("Matrix"), &a1);
	  
	  a2.initArray(xref);	  
	  obj.initReal(0);
	  a2.arrayAdd(&obj);
	  obj.initReal(0);
	  a2.arrayAdd(&obj);	  
	  a2.initArray(xref);	  
	  obj.initReal(1);
	  a2.arrayAdd(&obj);
	  obj.initReal(1);
	  a2.arrayAdd(&obj);
	  dict->add(qstrdup("BBox"), &a2);
	  
	  obj.initReal(1);
	  dict->add(qstrdup("XStep"), &obj);
	  dict->add(qstrdup("YStep"), &obj);
		if (image_id) 
		{
	    char buf[MAX_REF_CHARS + 6 + 1]; 

	    sprintf(buf, " /R%ld Do\n", image_id);
	    pres->object.streamAdd(buf, strlen(buf));
		} 
		else 
		{
	    code = (this->*copy_data)(tiles->data, 0, tiles->raster,  tile_id, 0, 0, tw, th, &image, &writer, -1);
	    switch (code) 
	    {
	    	default:
					return code;
					
	    	case 1:
					break;
					
	    	case 0:
					return (int)(XWPSError::Fatal);
	    }
		}
  }
  
  {
  	int code = openPage(PDF_IN_STREAM);
		if (code < 0)
	    goto use_default;
	    
		putClipPath(NULL);
		XWPSStream * s = strm;
		
		if (canWriteStream())
		{
			s->print(" q %g 0 0 %g 0 0 cm\n", xscale, yscale);
			writeObj(&cs_value, s, true);
			cs_value.free();
			s->pputs(" cs");
			if (mask)
	    	s->print(" %d %d %d", (int)(color1 >> 16), (int)((color1 >> 8) & 0xff), (int)(color1 & 0xff));
			s->print(" /R%ld scn", pres->resourceId());
			s->print(" %g %g %g %g re f Q\n",	 x / xscale, y / yscale, w / xscale, h / xscale);
		}
  }
  return 0;
  
use_default:
  return stripTileRectangleDefault(tiles, x, y, w, h, color0, color1, px, py);
}

int XWPSDeviceDVI::strokePathDVI(XWPSImagerState * pis,
		                XWPSPath * ppath, 
		                XWPSStrokeParams * params,
	                  XWPSDeviceColor * pdcolor, 
	                  XWPSClipPath * pcpath)
{
	XWPSStream *s;
  int code;
  double scale, path_scale;
  bool set_ctm;
  XWPSMatrix mat;
  
  if (ppath->isVoid())
		return 0;
		
	code = prepareStroke(pis);
  if (code < 0)
		return code;
		
	code = openPage(PDF_IN_STREAM);
  if (code < 0)
		return code;
				
	set_ctm = (bool)strokeScaling(pis, &scale, &mat);
  if (makePathScaling(ppath, &path_scale)) 
  {
		scale *= path_scale;
		if (set_ctm)
	    mat.matrixScale(&mat, path_scale, path_scale);
		else 
		{
	    mat.makeScaling(path_scale, path_scale);
	    set_ctm = true;
		}
  }
  putClipPath(pcpath);
  code = prepareStrokeVector(pis, params, pdcolor, scale);
  if (code < 0)
		return strokePathDefault(pis, ppath, params, pdcolor, pcpath);

  if (canWriteStream())
  {
  	if (set_ctm)
			putMatrix(" q ", &mat, "cm\n");
    	code = doPath(ppath, (PSPathType)(ps_path_type_stroke | ps_path_type_optimize),  (set_ctm ? &mat : 0));
  	if (code < 0)
			return code;
    s = strm;
    s->pputs((code ? " s" : " S"));
    s->pputs((set_ctm ? " Q\n" : "\n"));
  }
  return 0;
}

int XWPSDeviceDVI::textBeginDVI(XWPSImagerState * pis,
		               XWPSTextParams *text, 
		               XWPSFont * font,
		               XWPSPath * path, 
		               XWPSDeviceColor * pdcolor,
		               XWPSClipPath * pcpath,
		               XWPSTextEnum ** ppte)
{
	XWPSDVITextEnum *penum;
  XWPSFixedPoint cpt;
  int code;
  
  {
		XWPSMatrix tmat;
		int i;

		XWPSMatrix ctm;
		pis->ctmOnly(&ctm);
		tmat.matrixMultiply(&font->FontMatrix, &ctm);
		if (tmat.isXXYY())
	    i = (tmat.xx >= 0 ? 0 : 2);
		else if (tmat.isXYYX())
	    i = (tmat.xy >= 0 ? 1 : 3);
		else
	    i = 4;
		text_rotation.counts[i] += text->size;
  }
  
  code = prepareFill(pis);
  if (code < 0)
		return code;

  if ((text->operation & ~(TEXT_FROM_STRING | TEXT_FROM_BYTES |
	   				TEXT_ADD_TO_ALL_WIDTHS | TEXT_ADD_TO_SPACE_WIDTH |
	   				TEXT_REPLACE_WIDTHS | TEXT_DO_DRAW | TEXT_RETURN_WIDTH)) != 0 ||
						path->currentPoint(&cpt) < 0 ||
						font->PaintType != 0)
		return textBeginDefault(pis, text, font, path, pdcolor, pcpath, ppte);

  if (mustPutClipPath(pcpath)) 
  {
  	int code = openPage(PDF_IN_STREAM);

		if (code < 0)
	    return code;
		putClipPath(pcpath);
  }
  if (setDrawingColor(pdcolor, fill_color,  &psdf_set_fill_color_commands) < 0)
		return textBeginDefault(pis, text, font, path, pdcolor,  pcpath, ppte);

  penum = new XWPSDVITextEnum;
  code = penum->init(this, pis, text, font, path, pdcolor, pcpath);
  if (code < 0) 
  {
		delete penum;
		return code;
  }

  if (text->operation & TEXT_RETURN_WIDTH)
		path->currentPoint(&penum->origin);
  *ppte = penum;

  return 0;
}

int XWPSDeviceDVI::writeTextProcessState(XWPSDVITextProcessState *ppts, XWPSString *pstr)
{
	int code;
  XWPSStream *s;

  setFontAndSize(ppts->pdfont, ppts->size);
  code = setTextMatrix(&ppts->text_matrix);
  if (code < 0)
		return code;

  if (text.character_spacing != ppts->chars &&
			pstr->size + text.buffer_count > 1) 
	{
		code = openPage(PDF_IN_TEXT);
		if (code < 0)
	    return code;
		s = strm;
		if (canWriteStream())
		  s->print(" %g Tc\n", ppts->chars);
		text.character_spacing = ppts->chars;
  }

  if (text.word_spacing != ppts->words &&
			(memchr(pstr->data, 32, pstr->size) ||
	 		memchr(text.buffer, 32, text.buffer_count))) 
	{
		code = openPage(PDF_IN_TEXT);
		if (code < 0)
	    return code;
		s = strm;
		if (canWriteStream())
		  s->print(" %g Tw\n", ppts->words);
		text.word_spacing = ppts->words;
  }

  return 0;
}

int  XWPSDeviceDVI::addEncodingDifference(XWPSDVIFont *ppf, 
	                           int chr,
			                       XWPSFontBase *bfont, 
			                       ulong glyph)
{
	XWPSPDFEncodingElement *pdiff = ppf->Differences;

  if (pdiff == 0) 
  {
		pdiff = new XWPSPDFEncodingElement[256];
		ppf->Differences = pdiff;
  }
  pdiff[chr].glyph = glyph;
  pdiff[chr].data = (uchar *)bfont->glyphName(glyph, &pdiff[chr].size);
  return 0;
}

void  XWPSDeviceDVI::addVector3(XWObject *pca, const PSVector3 *pvec)
{
	XWObject obj;
	obj.initReal(pvec->u);
	pca->arrayAdd(&obj);
	obj.initReal(pvec->v);
	pca->arrayAdd(&obj);
	obj.initReal(pvec->w);	
	pca->arrayAdd(&obj);
}

ulong XWPSDeviceDVI::adjustColorIndexDVI(ulong color)
{
	return (color == (ps_no_color_index ^ 1) ? ps_no_color_index : color);
}

int  XWPSDeviceDVI::adjustFontName(XWPSDVIFontDescriptor *pfd, bool is_standard)
{
	int code = 0;
	if (MAKE_FONT_NAMES_UNIQUE)
	{
		int j = 0;
		XWPSDVIFontDescriptor *old;
		uchar *chars = pfd->FontName.chars;
		uint size = pfd->FontName.size;

#define SUFFIX_CHAR '~'

		{
	    int i;

	    for (i = size;	i > 0 && isxdigit(chars[i - 1]);--i)
				;
	    if (i < size && i > 0 && chars[i - 1] == SUFFIX_CHAR) 
	    {
				do 
				{
		    	--i;
				} while (i > 0 && chars[i - 1] == SUFFIX_CHAR);
				size = i + 1;
	    }
	    code = size != pfd->FontName.size;
		}
		
		if (!is_standard)
	    for (; j < NUM_RESOURCE_CHAINS; ++j)
				for (old = (XWPSDVIFontDescriptor *)resources[resourceFontDescriptor].chains[j]; old != 0; old = (XWPSDVIFontDescriptor *)(old->next))
				{
		    	const uchar *old_chars = old->FontName.chars;
		    	uint old_size = old->FontName.size;

		    	if (old == pfd)
						continue;
		    	if (hasSubsetPrefix(old_chars, old_size))
						old_chars += SUBSET_PREFIX_SIZE,  old_size -= SUBSET_PREFIX_SIZE;
		    	if (!bytes_compare(old_chars, old_size, chars, size))
						goto found;
				}
				
found:
		
		if (j < NUM_RESOURCE_CHAINS)
		{
			char suffix[sizeof(long) * 2 + 2];
	    uint suffix_size;

	    sprintf(suffix, " %c%lx", SUFFIX_CHAR, pfd->resourceId());
	    suffix_size = strlen(suffix);
	    if (size + suffix_size > sizeof(pfd->FontName.chars))
				return (int)(XWPSError::RangeCheck);
	    memcpy(chars + size, (const uchar *)suffix, suffix_size);
	    size += suffix_size;
	    code = 1;
		}
		pfd->FontName.size = size;
#undef SUFFIX_CHAR
	}
	
	return code;
}

int  XWPSDeviceDVI::allocAside(XWPSDVIResource ** plist,
		              XWPSDVIResource **ppres,
		              int rtype)
{
	XWPSDVIResource *pres = *ppres;
	char buf[50];	
	long pageno = xref->currentPageNumber();
	XWPSDVIPage *page = &(pages[pageno]);
	XWObject obj;
	XWDict * dict = 0;
	switch (rtype)
	{
		case resourceFont:
			pres = new XWPSDVIFont;
			pres->object.initDict(xref);
			dict = pres->object.getDict();
		  obj.initName(resource_names[rtype]);
  	  dict->add(qstrdup("Type"), &obj);
			break;
			
		case resourceXObject:
			pres = new XWPSDVIXObjects;
			pres->object.initStream(0, xref);
			if (xref->isScanning())
			{
				xref->refObj(&(pres->object), &(pres->indirect));
				if (!innaming)
				{
					page->xobjLabels = (long*)realloc(page->xobjLabels, (page->xobjCounter + 1) * sizeof(long));
					page->xobjLabels[page->xobjCounter] = pres->indirect.label;
				}
				sprintf(buf, "Im%d%d", pageno, pres->indirect.label);
				xref->addPageResource(resource_names[rtype], buf, &(pres->indirect));
			}
			else
			{
				pres->object.label = page->xobjLabels[page->xobjCounter];
				pres->indirect.initIndirect(0,page->xobjLabels[page->xobjCounter],0);
			}
			dict = pres->object.streamGetDict();
		  obj.initName(resource_names[rtype]);
  	  dict->add(qstrdup("Type"), &obj);
  	  if (!innaming)
	      page->xobjCounter++;
			break;
			
		case resourceColorSpace:
			pres = new XWPSDVIResource;
			pres->object.initArray(xref);
			if (xref->isScanning())
			{
				xref->refObj(&(pres->object), &(pres->indirect));
				if (!innaming)
				{
					page->csLabels = (long*)realloc(page->csLabels, (page->csCounter + 1) * sizeof(long));
					page->csLabels[page->csCounter] = pres->indirect.label;
				}
			}
			else
			{
				pres->object.label = page->csLabels[page->csCounter];
				pres->indirect.initIndirect(0,page->csLabels[page->csCounter],0);
			}
			if (!innaming)
	      page->csCounter++;
			break;
			
		case resourceExtGState:
			pres = new XWPSDVIResource;
			pres->object.initDict(xref);
			if (xref->isScanning())
			{
				xref->refObj(&(pres->object), &(pres->indirect));
				if (!innaming)
				{
					page->gsLabels = (long*)realloc(page->gsLabels, (page->gsCounter + 1) * sizeof(long));
					page->gsLabels[page->gsCounter] = pres->indirect.label;
				}
				sprintf(buf, "GS%d%d", pageno, pres->indirect.label);
				xref->addPageResource(resource_names[rtype], buf, &(pres->indirect));
			}
			else
			{
				pres->object.label = page->gsLabels[page->gsCounter];
				pres->indirect.initIndirect(0,page->gsLabels[page->gsCounter],0);
			}
			dict = pres->object.getDict();
		  obj.initName(resource_names[rtype]);
  	  dict->add(qstrdup("Type"), &obj);
  	  sprintf(buf, "GS%d%d", pageno, pres->indirect.label);
  	  obj.initName(buf);
  	  dict->add(qstrdup("Name"), &obj);
  	  if (!innaming)
	      page->gsCounter++;
			break;
			
		case resourcePattern:
			pres = new XWPSDVIResource;
			pres->object.initStream(0, xref);
			if (xref->isScanning())
			{
				xref->refObj(&(pres->object), &(pres->indirect));
				if (!innaming)
				{
					page->patternLabels = (long*)realloc(page->patternLabels, (page->patternCounter + 1) * sizeof(long));
					page->patternLabels[page->patternCounter] = pres->indirect.label;
				}
				sprintf(buf, "P%d%d", pageno, pres->indirect.label);
				xref->addPageResource(resource_names[rtype], buf, &(pres->indirect));
			}
			else
			{
				pres->object.label = page->patternLabels[page->patternCounter];
				pres->indirect.initIndirect(0,page->patternLabels[page->patternCounter],0);
			}
			dict = pres->object.streamGetDict();
		  obj.initName(resource_names[rtype]);
  	  dict->add(qstrdup("Type"), &obj);
  	  sprintf(buf, "P%d%d", pageno, pres->indirect.label);
  	  obj.initName(buf);
  	  dict->add(qstrdup("Name"), &obj);
  	  if (!innaming)
	      page->patternCounter++;
			break;
			
		case resourceShading:
			pres = new XWPSDVIResource;
			pres->object.initStream(0, xref);
			if (xref->isScanning())
			{
				xref->refObj(&(pres->object), &(pres->indirect));
				if (!innaming)
				{
					page->shadingLabels = (long*)realloc(page->shadingLabels, (page->shadingCounter + 1) * sizeof(long));
					page->shadingLabels[page->shadingCounter] = pres->indirect.label;
				}
			}
			else
			{
				pres->object.label = page->shadingLabels[page->shadingCounter];
				pres->indirect.initIndirect(0,page->shadingLabels[page->shadingCounter],0);
			}
  	  if (!innaming)
	      page->shadingCounter++;
			break;
			
		case resourceFontDescriptor:
			pres = new XWPSDVIFontDescriptor;	
			pres->object.initDict(xref);
			dict = pres->object.getDict();
		  obj.initName(resource_names[rtype]);
  	  dict->add(qstrdup("Type"), &obj);
			break;
			
		case resourceCharProc:
			pres = new XWPSDVICharProc;
			pres->object.initStream(0, xref);
			xref->refObj(&(pres->object), &(pres->indirect));
			dict = pres->object.streamGetDict();
		  obj.initName(resource_names[rtype]);
  	  dict->add(qstrdup("Type"), &obj);
			break;
						
		default:
			pres = new XWPSDVIResource;
			pres->object.initStream(0, xref);
			if (xref->isScanning())
			{
				xref->refObj(&(pres->object), &(pres->indirect));
				if (!innaming)
				{
					page->functionLabels = (long*)realloc(page->functionLabels, (page->functionCounter + 1) * sizeof(long));
					page->functionLabels[page->functionCounter] = pres->indirect.label;
				}
			}
			else
			{
				pres->object.label = page->functionLabels[page->functionCounter];
				pres->indirect.initIndirect(0,page->functionLabels[page->functionCounter],0);
			}
  	  if (!innaming)
	      page->functionCounter++;
			break;
	}
	
	pres->next = *plist;
  *plist = pres;
  pres->prev = last_resource;
  last_resource = pres;
  pres->used_on_page = true;
  *ppres = pres;
  return 0;
}

int  XWPSDeviceDVI::allocFont(ulong rid, 
	               XWPSDVIFont **ppfres,
	       				XWPSDVIFontDescriptor *pfd_in)
{
	XWPSDVIFontDescriptor *pfd = 0;
  XWPSString chars_used, glyphs_used;
  int code;
  XWPSDVIFont *pfres;

	if (*ppfres)
		pfd = (*ppfres)->FontDescriptor;
  if (pfd_in != 0)
  {
  	code = allocResource(resourceFontDescriptor, pfd_in->rid, (XWPSDVIResource **)&pfd);
		if (code < 0)
	    return code;
		chars_used.size = pfd_in->chars_used.size;
		chars_used.data = new uchar[chars_used.size];
		
		memset(chars_used.data, 0, chars_used.size);
		pfd->values = pfd_in->values;
		pfd->chars_used = chars_used;
		pfd->subset_ok = true;
  }
  
  code = allocResource(resourceFont, rid, (XWPSDVIResource **)ppfres);
  pfres = *ppfres;
  sprintf(pfres->frname, "F%ld", pfres->indirect.label);
  pfres->index = -1;
  pfres->BaseEncoding = ENCODING_INDEX_UNKNOWN;
  pfres->FontDescriptor = pfd;
  return 0;
}

int  XWPSDeviceDVI::allocResource(int rtype, 
	                   ulong rid,
		                 XWPSDVIResource ** ppres)
{
	int code = allocAside(resourceChain((PSPDFResourceType)rtype, rid), ppres, rtype);
  if (code >= 0)
		(*ppres)->rid = rid;
  return code;
}

int  XWPSDeviceDVI::assignCharCode()
{
	XWPSDVIFont *font = open_font;
  if (embedded_encoding_id == 0)
  {
  	XWObject ss, obj, a;
  	ss.initDict(xref);
  	a.initArray(xref);
  	xref->refObj(&ss, &obj);
  	long id = obj.label;
  	
  	obj.initName("Encoding");
  	ss.dictAdd(qstrdup("Type"), &obj);
  	
		char buf[20];
		for (int i = 0; i < 256; ++i) 
		{
			sprintf(buf, "a%d", i);
			obj.initName(buf);
			a.arrayAdd(&obj);
		}
		
		ss.dictAdd(qstrdup("Differences"), &a);
		if (!xref->isScanning())
			xref->releaseObj(&ss);
		else
			ss.free();	
			
		embedded_encoding_id = id;
 	}
 	
 	if (font == 0 || font->num_chars == 256 || !use_open_font) 
 	{
		int code = allocFont(ps_no_id, &font, NULL);
		char *pc;

		if (code < 0)
	    return code;
		if (open_font == 0)
	    memset(font->frname, 0, sizeof(font->frname));
		else
	    strcpy(font->frname, open_font->frname);
		for (pc = font->frname; *pc == 'Z'; ++pc)
	    *pc = '@';
		if ((*pc)++ == 0)
	    *pc = 'A', pc[1] = 0;
		open_font = font;
		use_open_font = true;
		checkFont(font);
  }
  return font->num_chars++;
}

int  XWPSDeviceDVI::beginCharProc(int w, 
	                   int h, 
	                   int x_width,
                     int y_offset, 
                     ulong id, 
                     XWPSDVICharProc ** ppcp, 
                     PSPDFStreamPosition * )
{
	XWPSDVIResource *pres;
  XWPSDVICharProc *pcp;
  int char_code = assignCharCode();
  XWPSDVIFont *font = open_font;
  int code;

  if (char_code < 0)
		return char_code;
    
  code = beginResource(resourceCharProc, id, &pres);
  if (code < 0)
		return code;
  pcp = (XWPSDVICharProc *) pres;
  pcp->font = font;
  pcp->char_next = font->char_procs;
  font->char_procs = pcp;
  pcp->char_code = char_code;
  pcp->width = w;
  pcp->height = h;
  pcp->x_width = x_width;
  pcp->y_offset = y_offset;
  font->max_y_offset = qMax(font->max_y_offset, h + (h >> 2));
  int l = 1024;
	uchar * buf = (uchar*)malloc(l * sizeof(uchar*));
	pcp->strm = new XWPSStream;
	pcp->strm->close_file = false;
	pcp->strm->writeFile(pcp->object.streamGetIO(), buf, l, true, false);
  *ppcp = pcp;
  pcp->save_strm = strm;
  strm = pcp->strm;
  iswritestrm = true;
  return 0;
}

int  XWPSDeviceDVI::beginData(XWPSDVIDataWriter *pdw)
{
	int l = 1024;
	uchar * buf = (uchar*)malloc(l * sizeof(uchar*));
	pdw->strm = new XWPSStream;
	pdw->strm->close_file = false;
	pdw->strm->writeFile(pdw->object->streamGetIO(), buf, l, true, false);
	pdw->save_strm = strm;
	strm = pdw->strm;
	iswritestrm = true;
	int filters = 0;
  int code;
  
#define USE_ASCII85 1
#define USE_FLATE 2

  if (!binary_ok)
		filters |= USE_ASCII85;
  if (CompatibilityLevel >= 1.2)
		filters |= USE_FLATE;
		
	XWDict * dict = pdw->object->streamGetDict();
	if (filters == 1)
	{
		XWObject obj;
		obj.initName("ASCII85Decode");
		dict->add(qstrdup("Filter"), &obj);
	}
	else if (filters == 2)
	{
		XWObject obj;
		obj.initName("FlateDecode");
		dict->add(qstrdup("Filter"), &obj);
	}
	else if (filters == 3)
	{
		XWObject obj;
		obj.initArray(xref);
		XWObject v;
		v.initName("ASCII85Decode");
		obj.arrayAdd(&v);
		v.initName("FlateDecode");
		obj.arrayAdd(&v);
		dict->add(qstrdup("Filter"), &obj);
	}		
	code = pdw->binary.beginBinary(this);
  if (code < 0)
		return code;
  pdw->start = strm->tell();
  if (filters & USE_FLATE)
		code = flateBinary(&pdw->binary);
  return code;
#undef USE_ASCII85
#undef USE_FLATE
}

int  XWPSDeviceDVI::beginFontFile(XWPSDVIFontFile * fontfile,
		                  long len, 
		                  XWPSPSDFBinaryWriter *pbw)
{
	XWDict * dict = fontfile->object.streamGetDict();
	XWObject obj;
	if (!binary_ok)
	{
		obj.initName("ASCII85Decode");
		dict->add(qstrdup("Filter"), &obj);
	}
	
	obj.initInt(len);
	dict->add(qstrdup("Length1"), &obj);
	fontfile->save_strm = strm;
	strm = fontfile->strm;
	iswritestrm = true;
	return pbw->beginBinary(this);
}

int  XWPSDeviceDVI::beginImageData(XWPSDVIImageWriter * piw,
		     							XWPSPixelImage * pim, 
		     							XWObject *pcsvalue)
{
	XWDict *pcd = piw->dict;
  int code = putImageValues(pcd, pim, piw->pin, pcsvalue);

  if (code >= 0)
		code = putImageFilters(pcd, piw->strm, piw->pin);
  
  return code;
}

int  XWPSDeviceDVI::beginResource(PSPDFResourceType rtype, 
	                   ulong rid,
		                 XWPSDVIResource ** ppres)
{
	return beginResourceBody(rtype, rid, ppres);
}

int XWPSDeviceDVI::beginResourceBody(PSPDFResourceType rtype,	
	                       ulong rid, 
	                       XWPSDVIResource ** ppres)
{
	int code = allocAside(resourceChain(rtype, rid), ppres, rtype);
	if (code >= 0)
		(*ppres)->rid = rid;
		
	return code;
}

int  XWPSDeviceDVI::beginWriteImage(XWPSDVIImageWriter * piw,
		      						 ulong id, 
		      						 int w, 
		      						 int h, 
		      						 XWPSDVIResource *pres,
		                   bool in_line)
{
	if (in_line) 
  {
		piw->pres = 0;
		piw->pin = &pdf_image_names_short;
		piw->data.initStream(0, xref);
		piw->end_string = " Q";
		int l = 1024;
		uchar * buf = (uchar*)malloc(l * sizeof(uchar*));
		piw->strm = new XWPSStream;
		piw->strm->close_file = false;
		piw->strm->writeFile(piw->data.streamGetIO(), buf, l, true, false);
		piw->dict = piw->data.streamGetDict();
  } 
  else
  {
  	if (pres == 0) 
		{
	    int code = allocResource(resourceXObject, id, &piw->pres);
	    if (code < 0)
				return code;
		} 
		else 
	    piw->pres = pres;
	    
		piw->pres->rid = id;
		piw->pin = &pdf_image_names_full;
		XWPSDVIXObjects * pxo = (XWPSDVIXObjects*)piw->pres;
		pxo->width = w;
		pxo->height = h;
		pxo->data_height = h;
		
		XWDict * dict = piw->pres->object.streamGetDict();
		XWObject obj;
		obj.initName("Image");
		dict->add(qstrdup("Subtype"), &obj);
		int l = 1024;
		uchar * buf = (uchar*)malloc(l * sizeof(uchar*));
		piw->strm = new XWPSStream;
		piw->strm->close_file = false;
		piw->strm->writeFile(piw->pres->object.streamGetIO(), buf, l, true, false);
		piw->dict = piw->pres->object.streamGetDict();
  }
  piw->height = h;
  piw->save_strm = strm;
	strm = piw->strm;
	iswritestrm = true;
	return piw->binary.beginBinary(this);
}

bool XWPSDeviceDVI::canWriteImageInLine(XWPSImage1 *pim)
{
	XWPSColorSpace *pcs;

  if (pim->ImageMask)
		return true;
    
  if (CompatibilityLevel >= 1.2)
		return true;
  
  pcs = pim->ColorSpace;
  
cs:
  switch (pcs->getIndex()) 
  {
    case ps_color_space_index_DeviceGray:
    case ps_color_space_index_DeviceRGB:
    case ps_color_space_index_DeviceCMYK:
			return true;
			
    case ps_color_space_index_Indexed:
			if (pcs->params.indexed->use_proc)
	    	return false;
			pcs = pcs->params.indexed->base_space;
			goto cs;
			
    default:
			return false;
  }
}

bool XWPSDeviceDVI::canWriteStream()
{
	if (!xref->isScanning())
		return true;
		
	return iswritestrm;
}

void XWPSDeviceDVI::checkFont(XWPSDVIFont * ppf)
{
	if (!ppf)
		return ;
	QString fontname;
	if (ppf->font)
	  fontname = QString::fromAscii((const char*)(ppf->font->font_name.chars),ppf->font->font_name.size);
	else
		fontname = QString::fromAscii((const char*)(ppf->frname), strlen((const char*)(ppf->frname)));
	XWPSDVILabel * flabel = 0;
	if (fontLabels.contains(fontname))
	{
		flabel = fontLabels[fontname];
		if (flabel)
		{
			if (flabel->used)
				ppf->used_on_page = false;
			else
				flabel->used = true;
			
			ppf->object.label = flabel->label;
			ppf->indirect.initIndirect(0,flabel->label,0);
		}
	}
	else
	{
		xref->refObj(&(ppf->object), &(ppf->indirect));
		flabel = new XWPSDVILabel;
		flabel->label = ppf->indirect.label;
		fontLabels[fontname] = flabel;
	}
		
	sprintf(ppf->frname, "F%ld", ppf->indirect.label);
	if (xref->isScanning() && ppf->used_on_page)
		xref->addPageResource("Font", ppf->frname, &(ppf->indirect));
}

void XWPSDeviceDVI::checkFontDescriptor(XWPSDVIFontDescriptor * pfd)
{
	if (!pfd)
		return ;
		
	QString fontname;
	if (pfd->base_font)
		fontname = QString::fromAscii((const char*)(pfd->base_font->font_name.chars), pfd->base_font->font_name.size);
	else
		fontname = QString::fromAscii((const char*)(pfd->FontName.chars), pfd->FontName.size);

	XWPSDVILabel * flabel = 0;
	if (fontDesLabels.contains(fontname))
	{
		flabel = fontDesLabels[fontname];
		if (flabel)
		{
			if (flabel->used)
				pfd->used_on_page = false;
			else
				flabel->used = true;
			
			pfd->object.label = flabel->label;
			pfd->indirect.initIndirect(0,flabel->label,0);
		}
	}
	else
	{
		xref->refObj(&(pfd->object), &(pfd->indirect));
		XWPSDVILabel * flabel = new XWPSDVILabel;
		flabel->label = pfd->indirect.label;
		fontDesLabels[fontname] = flabel;
	}
}

int XWPSDeviceDVI::closeContents(bool last)
{
	if (context == PDF_IN_NONE)
		return 0;
  
  if (last) 
  {
//  	openContents(PDF_IN_STREAM);
//		strm->pputs("Q\n");
		text.font = 0;
  }
  return openContents(PDF_IN_NONE);
}

int XWPSDeviceDVI::closePage()
{
	closeContents(true);
	for (int i = 0; i < resourceFont; i++)
  {
  	bool any = false;
	  for (int j = 0; j < NUM_RESOURCE_CHAINS; ++j)
	  {
	  	XWPSDVIResource *pres = resources[i].chains[j];
	  	for (; pres != 0; pres = pres->next)
	  	{
	  		if (pres->used_on_page)
	  		{
					if (!any) 
			    	any = true;
	  		}
	  	}
	  }
	  
	  if (any) 
			writeResourceObjects((PSPDFResourceType)i);
  }
  
  writeResourceObjects(resourceFunction);
  {
  	bool any = false;
		for (int j = 0; j < NUM_RESOURCE_CHAINS; ++j) 
		{
	    XWPSDVIResource **prev = (XWPSDVIResource **)&resources[resourceFont].chains[j];
	    XWPSDVIFont *font;
	    while ((font = (XWPSDVIFont*)(*prev)) != 0) 
	    {
				if (font->used_on_page) 
				{
		    	if (!any) 
						any = true;
				}
				if (font->skip) 
				{
		    	*prev = font->next;
				} 
				else
		    	prev = &font->next;
	    }
		}
  }
  
  resetGraphics();
  procsets = NoMarks;
  
  text.reset();
  context = PDF_IN_NONE;
  return 0;
}

int XWPSDeviceDVI::colorSpace(XWObject *pvalue,
								 XWPSColorSpace *pcs,
									const PSPDFColorSpaceNames *pcsn,
									bool by_name)
{
	PSColorSpaceIndex csi = pcs->getIndex();
  XWObject pca;
  XWObject pcd;
  XWObject obj;
  XWPSCie *pciec;
  XWPSFunction *pfn;
  int code;
  
  switch (csi) 
  {
  	case ps_color_space_index_DeviceGray:
			pvalue->initName((pcsn->DeviceGray) + 1);
			return 0;
			
    case ps_color_space_index_DeviceRGB:
			pvalue->initName((pcsn->DeviceRGB) + 1);
			return 0;
			
    case ps_color_space_index_DeviceCMYK:
			pvalue->initName((pcsn->DeviceCMYK) + 1);
			return 0;
			
    case ps_color_space_index_Pattern:
			if (!pcs->params.pattern && pcs->params.pattern->has_base_space) 
			{
	    	pvalue->initName("Pattern");
	    	return 0;
			}
			break;
			
    default:
			break;
  }
  
  pca.initArray(xref);
  switch (csi)
  {
  	case ps_color_space_index_CIEA:
  		{
  			XWPSCieA * pcie = pcs->params.a;
				PSVector3 expts;
				if (!(pcie->MatrixA.u == 1 && pcie->MatrixA.v == 1 &&
	      		pcie->MatrixA.w == 1 &&
	      		pcie->common.MatrixLMN.is_identity))
	      {
	      	pca.free();
	    		return (int)(XWPSError::RangeCheck);
	    	}
	    			
	    	if (pcie->cacheIsIdentity() &&
	    			pcie->scalar3CacheIsExponential(&expts) &&
	    			expts.v == expts.u && expts.w == expts.u) 
	    	{
	    		;
				} 
				else if (pcie->cache3IsIdentity() &&
		   					pcie->vectorCacheIsExponential(&(pcie->caches.DecodeA), &expts.u)) 
		   	{
	    		;
				} 
				else
				{
					pca.free();
	    		return (int)(XWPSError::RangeCheck);
	    	}
	    	
	    	obj.initName("CalGray");
	    	pca.arrayAdd(&obj);
	    		
	    	pcd.initDict(xref);
	    	if (expts.u != 1) 
	    	{
	    		obj.initReal(expts.u);
	    		pcd.dictAdd(qstrdup("Gamma"), &obj);
				}
				pciec = pcie;
  		}
  		
cal:
    	putVector3(pcd.getDict(), "WhitePoint", &pciec->common.points.WhitePoint);
			if (pciec->common.points.BlackPoint.u != 0 ||
					pciec->common.points.BlackPoint.v != 0 ||
					pciec->common.points.BlackPoint.w != 0) 
			{
				putVector3(pcd.getDict(), "BlackPoint", &pciec->common.points.BlackPoint);
    	}
    	pca.arrayAdd(&pcd);    	
  		break;
  		
  	case ps_color_space_index_CIEABC:
  		{
  			XWPSCieABC *pcie = pcs->params.abc;
				PSVector3 expts;
				const PSMatrix3 *pmat;
				
				if (pcie->common.MatrixLMN.is_identity &&
	    			pcie->cache3IsIdentity() &&
	    			pcie->vector3CacheIsExponential(&expts))
	    			pmat = &pcie->MatrixABC;
				else if (pcie->MatrixABC.is_identity &&
		 						 pcie->cache3IsIdentityABC() &&
		 						 pcie->scalar3CacheIsExponential(&expts))
	    		pmat = &pcie->common.MatrixLMN;
				else
				{
					pca.free();
	    		return (int)(XWPSError::RangeCheck);
	    	}
	    	
	    	{
	    		obj.initName("CalRGB");
					pca.arrayAdd(&obj);
				}
	    	pcd.initDict(xref);
	    	if (expts.u != 1 || expts.v != 1 || expts.w != 1) 
	    	{
	    		putVector3(pcd.getDict(), "Gamma", &expts);
  			}
  			
  			if (!pmat->is_identity) 
  			{
	    		XWObject pcma;
	    		pcma.initArray(xref);
	    		addVector3(&pcma, &pmat->cu);
					addVector3(&pcma, &pmat->cv);
					addVector3(&pcma, &pmat->cw);
					pcd.dictAdd(qstrdup("Matrix"), &pcma);
				}
  		}
  		goto cal;
  		
    case ps_color_space_index_Indexed:
    	{
    		XWPSIndexedParams *pip = pcs->params.indexed;
				XWPSColorSpace *base_space = pip->base_space;
				int num_entries = pip->hival + 1;
				int num_components = base_space->numComponents();
				uint table_size = num_entries * num_components;
				uint string_size = 1 + table_size * 2 + table_size / 30 + 2;
				uint string_used;
				uchar buf[100];
				PSStreamAXEState st;
				XWPSStream s, es;
				uchar *table = (uchar *)malloc(string_size * sizeof(uchar));
				uchar *palette = (uchar *)malloc((table_size+1) * sizeof(uchar));
				XWPSColorSpace cs_gray;
        cs_gray.i_ctx_p = context_state;
				s.writeString(table, string_size, false);
				es.init();
				st.templat = &s_AXE_template;
				es.incRef();
				s.incRef();
				XWPSStream::initFilter(&es, (PSStreamState *)&st, buf, sizeof(buf), &s, false, false);
				s.putc('<');
				if (pcs->params.indexed->use_proc)
				{
					XWPSClientColor cmin, cmax;
	    		uchar *pnext = palette;
	    		for (int j = 0; j < num_components; ++j)
						cmin.paint.values[j] = min_long, cmax.paint.values[j] = max_long;
	    		base_space->restrictColor(&cmin);
	    		base_space->restrictColor(&cmax);
	    		for (int i = 0; i < num_entries; ++i) 
	    		{
						XWPSClientColor cc;
						pcs->params.indexed->indexedLookup(i, &cc);
						for (int j = 0; j < num_components; ++j) 
						{
		    			float v = (cc.paint.values[j] - cmin.paint.values[j])	* 255 / (cmax.paint.values[j] - cmin.paint.values[j]);
		    			*pnext++ = (v <= 0 ? 0 : v >= 255 ? 255 : (uchar)v);
						}
	    		}
				}
				else
					memcpy(palette, pip->lookup.table->data, table_size);
				if (base_space->getIndex() == ps_color_space_index_DeviceRGB)
				{
					int i;

	    		for (i = table_size; (i -= 3) >= 0; )
						if (palette[i] != palette[i + 1] || palette[i] != palette[i + 2])
		    			break;
	    		if (i < 0) 
	    		{
						for (i = 0; i < num_entries; ++i)
		    			palette[i] = palette[i * 3];
						table_size = num_entries;
						cs_gray.initDeviceGray();
						base_space = &cs_gray;
	    		}
				}
				es.write(palette, table_size);
				free(palette);
				es.close();
				s.flush();
				string_used = (uint)(s.tell());
				table = (uchar*)realloc(table, (string_used+1));
				obj.initName(pdf_color_space_names.Indexed + 1);
				XWObject obj1;
				obj1.initString(new XWString((char*)table, string_used));
				free(table);
				colorSpace(pvalue, base_space,  &pdf_color_space_names, false);	 
				pca.arrayAdd(&obj);  
				pca.arrayAdd(pvalue);
				obj.initInt(pip->hival);
	    	pca.arrayAdd(&obj); 	
	    	pca.arrayAdd(&obj1);
    	}
    	break;
    	
    case ps_color_space_index_DeviceN:
			pfn = pcs->getDevNFunction();
			if (pfn == 0)
			{
				pca.free();
	    	return (int)(XWPSError::RangeCheck);
	    }
	    {
	    	XWObject psna, obj1;
	    	psna.initArray(xref);
	    	for (int i = 0; i < pcs->params.device_n->num_components; ++i) 
	    	{
					code = separationName(&obj, pcs->params.device_n->names[i]);
					obj1.initName(obj.getString()->getCString());
					psna.arrayAdd(&obj1);
					obj.free();
	    	}
	    	
	    	if ((code = separationColorSpace(&pca, "DeviceN", &psna, pcs->params.device_n->alt_space,	pfn, &pdf_color_space_names)) < 0)
	    	{
	    		pca.free();
	    		psna.free();
					return code;
				}
	    }
	    break;
	    
	  case ps_color_space_index_Separation:
			pfn = pcs->getSeprFunction();
			if (pfn == 0)
			{
				pca.free();
	    	return (int)(XWPSError::RangeCheck);
	    }
	    {
	    	XWObject obj1;
	    	separationName(&obj, pcs->params.separation->sname);
	    	obj1.initName(obj.getString()->getCString());
	    	obj.free();
				separationColorSpace(&pca, "Separation", &obj1, pcs->params.separation->alt_space,	pfn, &pdf_color_space_names);
	    }
			break;
			
		case ps_color_space_index_Pattern:
			{
				obj.initName("Pattern");
				colorSpace(pvalue, pcs->params.pattern->base_space, &pdf_color_space_names, false);
	    	pca.arrayAdd(&obj);
	    	pca.arrayAdd(pvalue);
	    }
			break;
			
    default:
    	pca.free();
			return (int)(XWPSError::RangeCheck);
  }
  
  XWPSDVIResource *pres;
	allocResource(resourceColorSpace, ps_no_id, &pres);
	pres->object.free();
	pca.copy(&(pres->object));
	pres->object.label = pres->indirect.label;
		pca.free();
	
	if (by_name) 
	{
		char buf[50];				
		sprintf(buf, "R%ld", pres->indirect.label);
		pvalue->initName(buf);
	}
	else
		pres->indirect.copy(pvalue);
		
  return 0;
}

void XWPSDeviceDVI::colorSpaceProcsets(XWPSColorSpace *pcs)
{
	XWPSColorSpace *pbcs = pcs;

csw:
  switch (pbcs->getIndex()) 
  {
    case ps_color_space_index_DeviceGray:
    case ps_color_space_index_CIEA:
			procsets = (PSPDFProcset)((int)procsets | (int)ImageB);
			break;
			
    case ps_color_space_index_Indexed:
			procsets = (PSPDFProcset)((int)procsets | (int)ImageI);
			pbcs = pcs->params.indexed->base_space;
			goto csw;
			
    default:
			procsets = (PSPDFProcset)((int)procsets | (int)ImageC);
			break;
  }
}

int XWPSDeviceDVI::computeFontDescriptor(XWPSDVIFontDescriptor *pfd,
			                      XWPSFont *font, 
			                      const uchar *)
{
	XWPSFontBase *bfont = (XWPSFontBase *)font;
	int wmode = font->WMode;
  int members = (GLYPH_INFO_WIDTH0 << wmode) | GLYPH_INFO_BBOX | GLYPH_INFO_NUM_PIECES;
  XWPSPDFFontDescriptorValues desc;
  desc.FontType = font->FontType;
  desc.FontBBox.p.x = desc.FontBBox.p.y = max_int;
  desc.FontBBox.q.x = desc.FontBBox.q.y = min_int;
  XWPSMatrix smat;
  XWPSMatrix *pmat = NULL;
  if (font->FontType == ft_TrueType) 
  {
		smat.makeScaling(1000.0, 1000.0);
		pmat = &smat;
  }
  
  int code = 0;
  ulong letters[52];
  int num_letters = 0;
  int fixed_width = 0;
  int small_descent = 0, small_height = 0;
  if (bfont->considerFontSymbolic())
		desc.Flags |= FONT_IS_SYMBOLIC;
  else
  {
  	bool small_present = false;
		int ch;
		int x_height = min_int;
		int cap_height = 0;
		XWPSRect bbox, bbox2;
		desc.Flags |= FONT_IS_ADOBE_ROMAN;
		for (ch = 'a'; ch <= 'z'; ++ch)
		{
			int y0, y1;
	    code = font->charBbox(&bbox, &letters[num_letters], ch, pmat);
	    if (code < 0)
				continue;
	    ++num_letters;
	    desc.FontBBox.merge(bbox);
	    small_present = true;
	    y0 = (int)bbox.p.y;
	    y1 = (int)bbox.q.y;
	    switch (ch)
	    {
	    	case 'b': case 'd': case 'f': case 'h':
	    	case 'k': case 'l': case 't':
	    		small_height = qMax(small_height, y1);
	    	case 'i':
	    		break;
	    		
	    	case 'j':
	    		small_descent = qMin(small_descent, y0);
					break;
					
				case 'g': case 'p': case 'q': case 'y':
					small_descent = qMin(small_descent, y0);
					
				default:
					x_height = qMax(x_height, y1);	
	    }
		}
		
		desc.XHeight = (int)x_height;
		if (!small_present)
	    desc.Flags |= FONT_IS_ALL_CAPS;
	    
	  for (ch = 'A'; ch <= 'Z'; ++ch) 
	  {
	    code = font->charBbox(&bbox, &letters[num_letters], ch, pmat);
	    if (code < 0)
				continue;
	    ++num_letters;
	    desc.FontBBox.merge(bbox);
	    cap_height = qMax(cap_height, (int)bbox.q.y);
		}
		desc.CapHeight = cap_height;
		if ((code = font->charBbox(&bbox, NULL, ':', pmat)) >= 0 &&
	     (code = font->charBbox(&bbox2, NULL, '.', pmat)) >= 0)
	  {
	  	int angle = (int)(atan2((bbox.q.y - bbox.p.y) - (bbox2.q.y - bbox2.p.y),
			                 (bbox.q.x - bbox.p.x) - (bbox2.q.x - bbox2.p.x)) * radians_to_degrees) - 90;
			while (angle > 90)
				angle -= 180;
	    while (angle < -90)
				angle += 180;
	    if (angle < -30)
				angle = -30;
	    else if (angle > 30)
				angle = 30;
			if (angle <= 2 && angle >= -2)
				angle = 0;
	    desc.ItalicAngle = angle;
	  }
	  
	  if (desc.ItalicAngle)
	    desc.Flags |= FONT_IS_ITALIC;
		if (code >= 0)
		{
			double wdot = bbox2.q.x - bbox2.p.x;

	    if ((code = font->charBbox(&bbox2, NULL, 'I', pmat)) >= 0) 
	    {
				double wcolon = bbox.q.x - bbox.p.x;
				double wI = bbox2.q.x - bbox2.p.x;

				desc.StemV = (int)wdot;
				if (wI > wcolon * 2.5 || wI > (bbox2.q.y - bbox2.p.y) * 0.25)
		    	desc.Flags |= FONT_IS_SERIF;
	    }
		}
  }
  
  num_letters = psf_sort_glyphs(letters, num_letters);
  desc.Ascent = desc.FontBBox.q.y;
  ulong notdef = ps_no_glyph;
  int index;
  ulong glyph;
  for (index = 0;	 (code = font->enumerateGlyph( &index, GLYPH_SPACE_INDEX, &glyph)) >= 0 && index != 0;)
  {
  	XWPSGlyphInfo info;
		XWPSString gnstr;
		if (psf_sorted_glyphs_include(letters, num_letters, glyph)) 
		{
	    code = font->glyphInfo(glyph, pmat, members - GLYPH_INFO_BBOX, &info);
	    if (code < 0)
				return code;
		}
		else
		{
	    code = font->glyphInfo(glyph, pmat, members, &info);
	    if (code < 0)
				return code;
	    desc.FontBBox.merge(info.bbox);
	    if (!info.num_pieces)
				desc.Ascent = qMax(long(desc.Ascent), (long)(info.bbox.q.y));
		}
		
		if (notdef == ps_no_glyph) 
		{
	    gnstr.data = (uchar *)(bfont->glyphName(glyph, &gnstr.size));
	    if (gnstr.size == 7 && !memcmp(gnstr.data, ".notdef", 7)) 
	    {
				notdef = glyph;
				desc.MissingWidth = info.width[wmode].x;
	    }
		}
		
		if (info.width[wmode].y != 0)
	    fixed_width = min_int;
		else if (fixed_width == 0)
	    fixed_width = info.width[wmode].x;
		else if (info.width[wmode].x != fixed_width)
	    fixed_width = min_int;
  }
  
  if (code < 0)
		return code;
  if (desc.Ascent == 0)
		desc.Ascent = desc.FontBBox.q.y;
  desc.Descent = desc.FontBBox.p.y;
  
  if (!(desc.Flags & (FONT_IS_SYMBOLIC | FONT_IS_ALL_CAPS)) &&
			(small_descent > desc.Descent / 3 || desc.XHeight > small_height * 0.9))
		desc.Flags |= FONT_IS_SMALL_CAPS;
  if (fixed_width > 0) 
  {
		desc.Flags |= FONT_IS_FIXED_WIDTH;
		desc.AvgWidth = desc.MaxWidth = desc.MissingWidth = fixed_width;
  }
  if (desc.CapHeight == 0)
		desc.CapHeight = desc.Ascent;
  if (desc.StemV == 0)
		desc.StemV = (int)(desc.FontBBox.q.x * 0.15);
  pfd->values = desc;
  return 0;
}

int XWPSDeviceDVI::copyColorBits(XWPSStream *s, 
	                 const uchar *base, 
	                 int sourcex, 
	                 int raster,
		               int w, 
		               int h, 
		               int bytes_per_pixel)
{
  for (int yi = 0; yi < h; ++yi) 
  {
		uint ignore;

		s->puts(base + sourcex * bytes_per_pixel + yi * raster,  w * bytes_per_pixel, &ignore);
  }
  return 0;
}

int XWPSDeviceDVI::copyColorData(const uchar * base, 
	                  int sourcex,
		                int raster, 
		                ulong id, 
		                int x, 
		                int y, 
		                int w, 
		                int h,
		                XWPSImage1 *pim, 
		                XWPSDVIImageWriter *piw,
		                int for_pattern)
{
	int depth = color_info.depth;
  int bytes_per_pixel = depth >> 3;
  XWPSColorSpace cs;
  cs.i_ctx_p = context_state;
  XWObject cs_value;
  ulong nbytes;
  int code = cs.initDevice(bytes_per_pixel);
  const uchar *row_base;
  int row_step;  
  bool in_line;

  if (code < 0)
		return code;
  pim->initAdjust(&cs, true);
  pim->makeBitmapImage(x, y, w, h);
  pim->BitsPerComponent = 8;
  nbytes = (ulong)w * bytes_per_pixel * h;

  if (for_pattern) 
  {
		row_base = base + (h - 1) * raster;
		row_step = -raster;
		in_line = for_pattern < 0;
  } 
  else 
  {
		row_base = base;
		row_step = raster;
		in_line = nbytes <= MAX_INLINE_IMAGE_BYTES;
		putImageMatrix(&pim->ImageMatrix, 1.0);
		if (id != ps_no_bitmap_id) 
		{
	    piw->pres = findResourceByPsId(resourceXObject, id);
	    if (piw->pres)
				return 0;
		}
  }
  
  if (canWriteStream())
   if (for_pattern < 0)
		 strm->pputs(" q ");
  if ((code = beginWriteImage(piw, id, w, h, NULL, in_line)) < 0 ||
			(code = colorSpace(&cs_value, &cs, &piw->pin->color_spaces, in_line)) < 0 ||
			(code = piw->binary.setupLosslessFilters(this, pim)) < 0 ||
			(code = beginImageData(piw, pim, &cs_value)) < 0)
	{
		cs_value.free();
		return code;
	}
  copyColorBits(piw->binary.strm, row_base, sourcex, row_step, w, h,bytes_per_pixel);
  endImageBinary(piw, piw->height);
  return endWriteImage(piw);
}

int XWPSDeviceDVI::copyMaskBits(XWPSStream *s, 
	                 const uchar *base, 
	                 int sourcex, 
	                 int raster,
		   							int w, 
		   							int h, 
		   							uchar invert)
{
	int yi;

  for (yi = 0; yi < h; ++yi) 
  {
		const uchar *data = base + yi * raster + (sourcex >> 3);
		int sbit = sourcex & 7;

		if (sbit == 0) 
		{
	    int nbytes = (w + 7) >> 3;
	    int i;

	    for (i = 0; i < nbytes; ++data, ++i)
				s->putc(*data ^ invert);
		} 
		else 
		{
	    int wleft = w;
	    int rbit = 8 - sbit;

	    for (; wleft + sbit > 8; ++data, wleft -= 8)
				s->putc(((*data << sbit) + (data[1] >> rbit)) ^ invert);
	    if (wleft > 0)
				s->putc(((*data << sbit) ^ invert) & (uchar) (0xff00 >> wleft));
		}
  }
  return 0;
}

int XWPSDeviceDVI::copyMaskData(const uchar * base, 
	                 int sourcex,
		               int raster, 
		               ulong id, 
		               int x, 
		               int y, 
		               int w,
		               int h,
		   						 XWPSImage1 *pim, 
		   						 XWPSDVIImageWriter *piw,
		   						 int for_pattern)
{
	ulong nbytes;
  int code;
 	const uchar *row_base;
  int row_step;  
  bool in_line;

  pim->initMask(true);
  pim->makeBitmapImage(x, y, w, h);
  nbytes = ((ulong)w * h + 7) / 8;

  if (for_pattern)
  {
  	row_base = base + (h - 1) * raster;
		row_step = -raster;
		in_line = for_pattern < 0;
  }
  else
  {
  	row_base = base;
		row_step = raster;
		in_line = nbytes <= MAX_INLINE_IMAGE_BYTES;
		putImageMatrix(&pim->ImageMatrix, 1.0);
		if (id != ps_no_bitmap_id) 
		{
	    piw->pres = findResourceByPsId(resourceXObject, id);
	    if (piw->pres)
				return 0;
		}
  }
  
  if (canWriteStream())
    if (for_pattern < 0)
		  strm->pputs(" q ");
  if ((code = beginWriteImage(piw, id, w, h, NULL, in_line)) < 0 ||
			(code = piw->binary.setupLosslessFilters(this, pim)) < 0 ||
			(code = beginImageData(piw, pim, NULL)) < 0)
	{
		return code;
	}
  copyMaskBits(piw->binary.strm, row_base, sourcex, row_step, w, h, 0);
  endImageBinary(piw, piw->height);
  return endWriteImage(piw);
}

int XWPSDeviceDVI::copyMono(const uchar *base, 
	             int sourcex, 
	             int raster, 
	             ulong id,
	      			 int x, 
	      			 int y, 
	      			 int w, 
	      			 int h, 
	      			 ulong zero,
	      			 ulong one, 
	      			 XWPSClipPath *pcpath)
{
	int code;
  XWPSColorSpace cs;
  cs.i_ctx_p = context_state;
  XWObject cs_value;
  XWObject *pcsvalue;
  uchar palette[sizeof(ulong) * 2];
  XWPSImage1 image;
  XWPSDVIImageWriter writer;
  PSPDFStreamPosition ipos;
  XWPSDVIResource *pres = 0;
  XWPSDVICharProc *pcp = 0;
  uchar invert = 0;
  bool in_line = false;
  
  if (mustPutClipPath(pcpath)) 
  {
  	code = openPage(PDF_IN_STREAM);
		if (code < 0)
	    return code;
		putClipPath(pcpath);
  }
  
  if (zero == ps_no_color_index) 
  {
		if (one == ps_no_color_index)
	    return 0;
		if (id != ps_no_bitmap_id && sourcex == 0) 
		{
	    setPureColor(one, fill_color,  &psdf_set_fill_color_commands);
	    pres = findResourceByPsId(resourceCharProc, id);
	    if (pres == 0) 
	    {
				int y_offset;
				int max_y_offset =(open_font == 0 ? 0 :	open_font->max_y_offset);

				image.initMask(false);
				invert = 0xff;
				image.makeBitmapImage(x, y, w, h);
				y_offset = image.ImageMatrix.ty - (int)(text.current.y + 0.5);
				if (x < text.current.x || y_offset < -max_y_offset || y_offset > max_y_offset)
		    	y_offset = 0;
		    	
				code = beginCharProc(w, h, 0, y_offset, id, &pcp, &ipos);
				if (code < 0)
		    	return code;
				y_offset = -y_offset;
				strm->print(" 0 0 0 %d %d %d d1\n", y_offset, w, h + y_offset);
				strm->print("%d 0 0 %d 0 %d cm\n", w, h, y_offset);
				code = beginWriteImage(&writer, ps_no_id, w, h, NULL, true);
				if (code < 0)
		    	return code;
				pcp->rid = id;
				pres = pcp;
				goto wr;
	    }
	    image.ImageMatrix.makeBitmapMatrix(x, y, w, h, h);
	    goto rx;
		}
		setPureColor(one, fill_color,  &psdf_set_fill_color_commands);
		image.initMask(false);
		invert = 0xff;
  } 
  else if (one == ps_no_color_index) 
  {
		image.initMask(false);
		setPureColor(zero, fill_color, &psdf_set_fill_color_commands);
  } 
  else if (zero == black && one == white) 
  {
		cs.initDeviceGray();
		image.initAdjust(&cs, true);
  } 
  else if (zero == white && one == black) 
  {
		cs.initDeviceGray();
		image.initAdjust(&cs, true);
		invert = 0xff;
  } 
  else 
  {
		XWPSColorSpace cs_base;
		cs_base.i_ctx_p = context_state;
		ulong c[2];
		int i, j;
		int ncomp = color_info.num_components;
		uchar *p;
	
		code = cs_base.initDevice(ncomp);
		if (code < 0)
	    return code;
		c[0] = adjustColorIndexPSDF(zero);
		c[1] = adjustColorIndexPSDF(one);
		cs.init(&ps_color_space_type_Indexed);
		cs.params.indexed->base_space->copy(&cs_base);
		cs.params.indexed->hival = 1;
		p = palette;
		for (i = 0; i < 2; ++i)
	    for (j = ncomp - 1; j >= 0; --j)
				*p++ = (uchar)(c[i] >> (j * 8));
		cs.params.indexed->lookup.table->data = palette;
		cs.params.indexed->lookup.table->size = p - palette;
		cs.params.indexed->use_proc = false;
		image.initAdjust(&cs, true);
		image.BitsPerComponent = 1;
  }
  
  image.makeBitmapImage(x, y, w, h);
  {
		ulong nbytes = (ulong) ((w + 7) >> 3) * h;

		in_line = nbytes <= MAX_INLINE_IMAGE_BYTES;
		if (in_line)
	    putImageMatrix(&image.ImageMatrix, 1.0);
	  code = openPage(PDF_IN_STREAM);
		if (code < 0)
	    return code;
		code = beginWriteImage(&writer, ps_no_id, w, h, NULL, in_line);
		if (code < 0)
	    return code;
  }
  
wr:
  if (image.ImageMask)
		pcsvalue = NULL;
  else 
  {
		code = colorSpace(&cs_value, &cs, &writer.pin->color_spaces, in_line);
		if (code < 0)
		{
			cs_value.free();
	    return code;
	  }
		pcsvalue = &cs_value;
  }
  
  if (pres) 
		writer.binary.CFEBinary(image.Width, image.Height, false);
  else 
		writer.binary.setupImageFilters(this, &image, NULL, NULL);
		
  beginImageData(&writer, &image, pcsvalue);
  code = copyMaskBits(writer.binary.strm, base, sourcex, raster, w, h, invert);
  if (code < 0)
		return code;
  endImageBinary(&writer, writer.height);
  if (!pres) 
  {
		switch ((code = endWriteImage(&writer))) 
		{
	    default:		/* error */
				return code;
				
	    case 1:
				return 0;
				
	    case 0:
				return doImage(writer.pres, &image.ImageMatrix);
		}
  }
  writer.end_string = "";
  switch ((code = endWriteImage(&writer))) 
  {
    default:
			return code;
			
    case 0:
			return (int)(XWPSError::Fatal);
				
    case 1:
			break;
  }
  
  if (pcp)
  	code = endCharProc(pcp);
  	
  if (code < 0)
		return code;
		
rx:
	{
		XWPSMatrix imat;

		imat = image.ImageMatrix;
		imat.xx /= w;
		imat.xy /= h;
		imat.yx /= w;
		imat.yy /= h;
		return doCharImage((XWPSDVICharProc *)pres, &imat);
  }
}

int XWPSDeviceDVI::createNamedArray(PSParamString *pname, XWObject * pvalue)
{
	pvalue->initArray(xref);
	if (pname->size > 0)
	{
		char * buf = getName(pname);
		if (buf)
		{
			core->pushObject((const char *)buf, pvalue);
			delete [] buf;
		}
	}
	return 0;
}

int XWPSDeviceDVI::createNamedDict(PSParamString *pname, XWObject * pvalue)
{
	pvalue->initDict(xref);
	if (pname->size > 0)
	{
		char *buf = getName(pname);
		if (buf)
		{
			core->pushObject((const char *)buf, pvalue);
			delete [] buf;
		}
	}
	
	return (int)(XWPSError::Undefined);
}

int XWPSDeviceDVI::createNamedStream(PSParamString *pname, XWObject * pvalue)
{
	pvalue->initStream(0, xref);
	if (pname->size > 0)
	{
		char *buf = getName(pname);
		if (buf)
		{
			core->pushObject((const char *)buf, pvalue);
			delete [] buf;
		}
	}
	
	return (int)(XWPSError::Undefined);
}

int XWPSDeviceDVI::csPatternColored(XWObject *pvalue)
{
	return patternSpace(pvalue, &cs_Patterns[0], "Pattern");
}

int XWPSDeviceDVI::csPatternUnColored(XWObject *pvalue)
{
	int ncomp = color_info.num_components;
	if (!cs_Patterns[ncomp]) 
	{
		XWObject obj;
		beginResourceBody(resourceColorSpace, ps_no_id, &cs_Patterns[ncomp]);
		obj.initName("Pattern");
		cs_Patterns[ncomp]->object.arrayAdd(&obj);
		switch (color_info.num_components)
		{
			case 1:
				obj.initName("DeviceGray");
				break;
				
			case 3:
				obj.initName("DeviceRGB");
				break;
				
			case 4:
				obj.initName("DeviceCMYK");
				break;
				
			default:
				break;
			
		}
		
		cs_Patterns[ncomp]->object.arrayAdd(&obj);
  }
  cs_Patterns[ncomp]->indirect.copy(pvalue);
  return 0;
}

int XWPSDeviceDVI::doCharImage(XWPSDVICharProc * pcp, XWPSMatrix * pimat)
{
	setFontAndSize(pcp->font, 1.0);
  {
		XWPSMatrix tmat;

		tmat = *pimat;
		tmat.ty -= pcp->y_offset;
		setTextMatrix(&tmat);
  }
  appendChars(&pcp->char_code, 1);
  text.current.x += pcp->x_width * text.matrix.xx;
  return 0;
}

int XWPSDeviceDVI::dominantRotation(PSPDFTextRotation * ptr)
{
	int i, imax = 0;
  long max_count = ptr->counts[0];
  static const int angles[] = {0, 90, 180, 270, -1};

  for (i = 1; i < (sizeof(ptr->counts) / sizeof(ptr->counts[0])); ++i) 
  {
		long count = ptr->counts[i];
		if (count > max_count)
		{
	    imax = i; 
	    max_count = count;
	  }
  }
  return angles[imax];
}

int XWPSDeviceDVI::embedFontAsType1(XWPSFontType1 *font,	
	                    XWPSDVIFontFile * FontFile, 
	                    ulong * subset_glyphs,
			                uint subset_size, 
			                XWPSString *pfname)
{
	XWPSStream poss;
	int lengths[3];
  int code;
  XWPSPSDFBinaryWriter writer;
  
  poss.writePositionOnly();
  
#define TYPE1_OPTIONS (WRITE_TYPE1_EEXEC | WRITE_TYPE1_EEXEC_MARK |\
		       WRITE_TYPE1_WITH_LENIV)
		       
  code = font->writeType1Font(&poss,TYPE1_OPTIONS, subset_glyphs, subset_size, pfname, lengths);
  if (code < 0)
		return code;
		
	XWObject obj;
	XWDict * dict = FontFile->object.streamGetDict();
	obj.initInt(lengths[1]);
	dict->add(qstrdup("Length2"), &obj);
	obj.initInt(0);
	dict->add(qstrdup("Length3"), &obj);
	code = beginFontFile(FontFile, lengths[0], &writer);
  if (code < 0)
		return code;
	font->writeType1Font(writer.strm, TYPE1_OPTIONS, subset_glyphs, subset_size, pfname, lengths);
#undef TYPE1_OPTIONS
  endFontFile(FontFile, &writer);
  return 0;
}

int XWPSDeviceDVI::embedFontAsType2(XWPSFontType1 *font,
											 XWPSDVIFontFile * FontFile, 
											 ulong * subset_glyphs,
											 uint subset_size, 
											 XWPSString *pfname)
{
	XWPSStream poss;
	int code;
  XWPSPSDFBinaryWriter writer;
  int options = TYPE2_OPTIONS |	(CompatibilityLevel < 1.3 ? WRITE_TYPE2_AR3 : 0);

	poss.writePositionOnly();
  code = font->writeType2Font(&poss, options,	subset_glyphs, subset_size, pfname);
  if (code < 0)
		return code;
	XWObject obj;
	XWDict * dict = FontFile->object.streamGetDict();
	obj.initName("Type1C");
	dict->add(qstrdup("Subtype"), &obj);
	code = beginFontFile(FontFile, poss.tell(), &writer);
  if (code < 0)
		return code;
  code = font->writeType2Font(writer.strm, options, subset_glyphs, subset_size, pfname);
  endFontFile(FontFile, &writer);
  return 0;
}

int XWPSDeviceDVI::embedFontCid0(XWPSFontCid0 *font,
		    						XWPSDVIFontFile * FontFile, 
		    						const uchar *subset_cids,
		    						uint subset_size, 
		    						XWPSString *pfname)
{
	XWPSStream poss;
	int code;
  XWPSPSDFBinaryWriter writer;
  
  if (CompatibilityLevel < 1.2)
		return (int)(XWPSError::RangeCheck);
			
	poss.writePositionOnly();
	code = font->writeCid0Font(&poss, TYPE2_OPTIONS,  subset_cids, subset_size, pfname);
  if (code < 0)
		return code;
		
	XWObject obj;
	XWDict * dict = FontFile->object.streamGetDict();
	obj.initName("CIDFontType0C");
	dict->add(qstrdup("Subtype"), &obj);
	code = beginFontFile(FontFile, poss.tell(), &writer);
  if (code < 0)
		return code;
		
  code = font->writeCid0Font(writer.strm, TYPE2_OPTIONS, subset_cids, subset_size, pfname);
  endFontFile(FontFile, &writer);
  return 0;
}

int XWPSDeviceDVI::embedFontCid2(XWPSFontCid2 *font,
		                XWPSDVIFontFile * FontFile, 
		                const uchar *subset_bits,
		    						uint subset_size, 
		    						XWPSString *pfname)
{
#define OPTIONS 0
  XWPSStream poss;
  int code;
  XWPSPSDFBinaryWriter writer;
	poss.writePositionOnly();
  code = font->writeCid2Font(&poss, OPTIONS, subset_bits, subset_size, pfname);
  if (code < 0)
		return code;
  code = beginFontFile(FontFile, poss.tell(), &writer);
  if (code < 0)
		return code;
  font->writeCid2Font(writer.strm, OPTIONS, subset_bits, subset_size, pfname);
#undef OPTIONS
  endFontFile(FontFile, &writer);
  return 0;
}

int XWPSDeviceDVI::embedFontType1(XWPSFontType1 *font,
		     						 XWPSDVIFontFile * FontFile, 
		     						 ulong * subset_glyphs,
		                 uint subset_size, 
		                 XWPSString *pfname)
{
	switch (font->FontType) 
	{
    case ft_encrypted:
			if (CompatibilityLevel < 1.2)
	  		return embedFontAsType1(font, FontFile, subset_glyphs, subset_size, pfname);
	    			
    case ft_encrypted2:
			return embedFontAsType2(font, FontFile, subset_glyphs, subset_size, pfname);
				
    default:    		
			return (int)(XWPSError::RangeCheck);
  }
}

int XWPSDeviceDVI::embedFontType42(XWPSFontType42 *font,
		      						XWPSDVIFontFile * FontFile, 
		      						ulong * subset_glyphs,
		      						uint subset_size, 
		      						XWPSString *pfname)
{
	int options = WRITE_TRUETYPE_CMAP | WRITE_TRUETYPE_NAME | (CompatibilityLevel <= 1.2 ? WRITE_TRUETYPE_NO_TRIMMED_TABLE : 0);
  XWPSStream poss;
  int code;
  XWPSPSDFBinaryWriter writer;

	poss.writePositionOnly();
  code = font->writeTrueTypeFont(&poss, options, subset_glyphs, subset_size, pfname);
  if (code < 0)
		return code;
  code = beginFontFile(FontFile, poss.tell(), &writer);
  if (code < 0)
		return code;
  font->writeTrueTypeFont(writer.strm, options, subset_glyphs, subset_size, pfname);
  endFontFile(FontFile, &writer);
  return 0;
}

int XWPSDeviceDVI::endCharProc(XWPSDVICharProc * pcp)
{
	strm = pcp->save_strm;
	iswritestrm = false;
	return 0;
}

int XWPSDeviceDVI::endData(XWPSDVIDataWriter *pdw)
{
	int code = pdw->binary.endBinary();
	strm = pdw->save_strm;
	if (!xref->isScanning())
		xref->releaseObj(pdw->object);
	else
		pdw->object->free();
	iswritestrm = false;
	return code;
}

int XWPSDeviceDVI::endFontFile(XWPSDVIFontFile * fontfile, XWPSPSDFBinaryWriter *pbw)
{
	pbw->endBinary();
	strm = fontfile->save_strm;
	iswritestrm = false;
	return 0;
}

int XWPSDeviceDVI::finalizeFontDescriptor(XWPSDVIFontDescriptor *pfd)
{
	XWPSFont *font = pfd->base_font;
  int code = (font ? computeFontDescriptor(pfd, font, NULL) : 0);

  if (code >= 0) 
  {
		if (pfd->FontFile)
	    code = writeEmbeddedFont(pfd);
		else
	    code = writeFontDescriptor(pfd);
		pfd->written = true;
  }
  if (pfd->base_font)
  {
  	if (pfd->base_font->decRef() == 0)
  		delete pfd->base_font;
  	pfd->base_font = 0;
  }
  return code;
}

int XWPSDeviceDVI::findNamed(PSParamString * pname, XWObject *pco)
{
	char * buf = getName(pname);
	if (buf)
	{
		char * p = buf;
		int len = strlen(buf);
		char * ls = new char[len + 1];
		for (int i = 0; i < len; i++)
		{
			if (isupper(p[i]))
				ls[i] = tolower(p[i]);
			else
				ls[i] = p[i];
		}
		ls[len] = '\0';
		if (core->lookupObject(ls, pco) != 0)
		{
			delete [] ls;
			delete [] buf;
			return 0;
		}
		delete [] ls;
		if (core->lookupObject(buf, pco) != 0)
		{
			delete [] buf;
			return 0;
		}
	
		delete [] buf;
	}
	return (int)(XWPSError::Undefined);
}

int XWPSDeviceDVI::findStdAppearance(XWPSFontBase *bfont, int mask, int *psame)
{
	bool has_uid = bfont->UID.isUniqueID() && bfont->UID.id != 0;
  XWPSDVIStdFont *psf = std_fonts;
  int i;

  mask |= FONT_SAME_OUTLINES;
  for (i = 0; i < PDF_NUM_STD_FONTS; ++psf, ++i) 
  {
		if (has_uid) 
		{
	    if (!bfont->UID.equal(&psf->uid))
				continue;
	    if (!psf->font) 
	    {
				*psame = FONT_SAME_OUTLINES | FONT_SAME_METRICS;
				return i;
	    }
		}
		if (psf->font) 
		{
	    int same = *psame =	bfont->sameFont(psf->font, mask);

	    if (same & FONT_SAME_OUTLINES)
				return i;
		}
  }
  *psame = 0;
  return -1;
}

int XWPSDeviceDVI::flateBinary(XWPSPSDFBinaryWriter *pbw)
{
	const PSStreamTemplate *templat = &s_zlibE_template;
  PSStreamZLibState *st = new PSStreamZLibState;
  if (templat->set_defaults)
		(*(templat->set_defaults))((PSStreamState*)st);
  return pbw->encodeBinary((PSStreamTemplate*)templat, (PSStreamState*)st, true);
}

PSPDFFontEmbed XWPSDeviceDVI::fontEmbedStatus(XWPSFont *font, int *pindex, int *psame)
{
	const uchar *chars = font->font_name.chars;
  uint size = font->font_name.size;
  int index = pdf_find_standard_font(chars, size);

	if (index >= 0) 
	{
	  *pindex = index;
	  if (font->is_resource) 
	  {
			*psame = ~0;
			return FONT_EMBED_STANDARD;
	  } 
	  else if (font->FontType != ft_composite && findStdAppearance((XWPSFontBase *)font, -1, psame) == index)
		return FONT_EMBED_STANDARD;
	}
	
	*pindex = -1;
  *psame = 0;
  if (params.NeverEmbed.embedListIncludes(chars, size))
		return FONT_EMBED_NO;
  if (params.EmbedAllFonts || font->FontType == ft_composite || ((XWPSFontBase*)(font))->isSymbolic()	 ||	
  	 params.AlwaysEmbed.embedListIncludes(chars, size))
		return FONT_EMBED_YES;
  return FONT_EMBED_NO;
}

int XWPSDeviceDVI::function(XWPSFunction *pfn, XWObject *obj)
{
	XWPSFunctionInfo info;
  XWPSDVIResource *pres;
  XWDict * pcd;
  XWObject dict;
  int code = allocResource(resourceFunction, ps_no_id, &pres);
  *obj = pres->indirect;
  if (code < 0)
		return code;

  pfn->getInfo(&info);
  if (info.DataSource != 0)
  {
  	pcd = pres->object.streamGetDict();
  	XWPSPSDFBinaryWriter writer;
		code = writer.beginBinary(this);
		if (code >= 0 && info.data_size > 30 &&	CompatibilityLevel >= 1.2)
	    code = flateBinary(&writer);
	  if (code >= 0) 
	  {
	  	static const PSPDFFilterNames fnames = {
				"/ASCII85Decode", "/ASCIIHexDecode", "/CCITTFaxDecode",
  			"/DCTDecode",  "/DecodeParms", "/Filter", "/FlateDecode",
  			"/LZWDecode", "/RunLengthDecode" };

	    code = putFilters(pcd, writer.strm, &fnames);
	    if (code >= 0) 
	    {
	    	uchar buf[100];
	    	ulong pos;
	    	uint count;
	    	uchar *ptr;

	    	for (pos = 0; pos < info.data_size; pos += count) 
	    	{
					count = qMin((uint)(sizeof(buf)), (uint)(info.data_size - pos));
					info.DataSource->accessOnly(pos, count, buf,	&ptr);
					writer.strm->write(ptr, count);
	    	}
	    	code = writer.endBinary();
			}
			
			if (code < 0)
	    	return code;
	  }
  }
  else
  {
  	pres->object.free();
  	pres->object.initDict(xref);
  	pres->object.label = pres->indirect.label;
  	pcd = pres->object.getDict();
  }
  
  if (info.Functions != 0)
  {
		XWObject functions;
		functions.initArray(xref);
		for (int i = 0; i < info.num_Functions; ++i)
		{
			XWObject v;
			if ((code = function(info.Functions[i], &v)) < 0)
			{
				functions.free();
				return code;
	    }
	    functions.arrayAdd(&v);
		}
		
		pcd->add(qstrdup("Functions"), &functions);
  }
  
  dict.initDict(pcd); 
  XWPSObjectParamListWriter rlist(&dict, PRINT_BINARY_OK);  
  code = pfn->getParams(context_state, &rlist);  
  return 0;
}

char * XWPSDeviceDVI::getName(PSParamString * pname)
{
	char * ret = 0;
	if (pname)
	{
		if (pname->size > 2 && 
			  pname->data[0] == '{' && 
			  (uchar *)memchr(pname->data, '}', pname->size) == pname->data + pname->size - 1)
		{
			ret = new char[pname->size - 1];
			memcpy(ret, pname->data + 1, pname->size - 2);
			ret[pname->size - 2] = '\0';
		}
	}
	
	return ret;
}

int XWPSDeviceDVI::getNamed(PSParamString * pname, XWObject *pco)
{
	char * buf = getName(pname);
	if (buf)
	{
		char * p = buf;
		int len = strlen(buf);
		char * ls = new char[len + 1];
		for (int i = 0; i < len; i++)
		{
			if (isupper(p[i]))
				ls[i] = tolower(p[i]);
			else
				ls[i] = p[i];
		}
		ls[len] = '\0';
		if (core->lookupObject(ls, pco) != 0)
		{
			delete [] ls;
			delete [] buf;
			return 0;
		}
		delete [] ls;
		if (core->lookupObject(buf, pco) != 0)
		{
			delete [] buf;
			return 0;
		}
	
		delete [] buf;
	}
	
	return (int)(XWPSError::Undefined);
}

int XWPSDeviceDVI::getObject(PSParamString * pvalue, XWObject *obj)
{
	uint size = pvalue->size;
	uchar * vstr = pvalue->data;
	if (size > 0 && vstr[0] == '/')
	{
		char buf[100];
		int len = 0;
		for (uint i = 1; i < size; ++i)
		{
			uint c = vstr[i];
			switch (c)
			{
				default:
					if (c >= 0x21 && c <= 0x7e)
					  buf[len++] = (char)c;
					break;
					
				case '#':
					len += sprintf(buf + len, "#%02x", c);
					break;
					
				case '%':
	      case '(': case ')':
	      case '<': case '>':
	      case '[': case ']':
	      case '{': case '}':
	      case '/':
	      	len += sprintf(buf + len, "#%02x", c);
	      	break;
	      	
	      case 0:
	      	buf[len++] = '?';
	      	break;
			}
		}
		buf[len] = '\0';
		obj->initName(buf);
		return 0;
	}
	else
	{
		XWLexer lexer((const char*)(vstr), size);
		const char * p = lexer.skipWhite();
		const char * endptr = lexer.getEndPtr();
		XWDVIParser parser(xref, &lexer);
		switch (p[0])
		{
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
  			{
  				int id = 0;
  				int gen = 0;
  				while (p < endptr && isdigit(p[0]))
  				{
  					id = id * 10 + (p[0] - '0');
  					p++;
  				}
  				
  				if (p < endptr)
  				{
  					while (p < endptr && XWLexer::isSpace(p[0]))
  						p++;
  						
  					if (p < endptr && isdigit(p[0]))
  					{
  						while (p < endptr && isdigit(p[0]))
  				    {
  					    gen = gen * 10 + (p[0] - '0');
  					    p++;
  				    }
  				    if (p < endptr)
  				    {
  				    	while (p < endptr && XWLexer::isSpace(p[0]))
  						    p++;
  						    
  						  if (p < endptr && p[0] == 'R')
  						  	obj->initIndirect(0, id, gen);
  						  else
  						  {
  						  	if (parser.parsePDFObject(obj))
			              return 0;
  						  }
  				    }
  				    else
  				    {
  				    	if (parser.parsePDFObject(obj))
			            return 0;
  				    }
  					}
  					else
  					{
  						if (parser.parsePDFObject(obj))
			          return 0;
  					}
  				}
  				else
  				{
  					if (parser.parsePDFObject(obj))
			        return 0;
  				}
  			}
  			break;
  			
  		default:
		    if (parser.parsePDFObject(obj))
			    return 0;
  		  break;
		}
		
		return (int)(XWPSError::Undefined);
	}
}

int XWPSDeviceDVI::getRefence(PSParamString * pname, XWObject *pco)
{
	char * n = getName(pname);
	if (!n)
		return (int)(XWPSError::Undefined);
			
	if (core->lookupReference(n, pco))
	{
		delete [] n;
		return 0;
	}
	int len = strlen(n);
	for (int i = 0; i < len; i++)
	{
		if (isupper(n[i]))
			n[i] = tolower(n[i]);
	}
	if (core->lookupReference(n, pco))
	{
		delete [] n;
		return 0;
	}
	delete [] n;
	return (int)(XWPSError::Undefined);
}

void XWPSDeviceDVI::init()
{
	fill_overprint = false;
	stroke_overprint = false;
	overprint_mode = 0;
	halftone_id = 0;
	transfer_ids[0] = ps_no_id;
	transfer_ids[1] = ps_no_id;
	transfer_ids[2] = ps_no_id;
	transfer_ids[3] = ps_no_id;
	black_generation_id = ps_no_id;
	undercolor_removal_id = ps_no_id;
	compression = pdf_compress_none;
	open_font = 0;
	use_open_font = false;
	embedded_encoding_id = 0;
	context = PDF_IN_NONE;
	memset(space_char_ids, 0, sizeof(space_char_ids));
	procsets = NoMarks;
	for (int i = 0; i < 5; i++)
		text_rotation.counts[i] = 0;
	text_rotation.Rotate = 0;
	pages = 0;
	num_pages = 0;
	outlines_id = 0;
	next_id = 0;
	cs_Patterns[0] = 0;
	cs_Patterns[1] = 0;
	cs_Patterns[2] = 0;
	cs_Patterns[3] = 0;
	cs_Patterns[4] = 0;
	outline_depth = 0;
	closed_outline_depth = 0;
	outlines_open = 0;
	last_resource = 0;
	
	articles = 0;
	open_graphics = 0;
	iswritestrm = false;
	innaming = false;
}

int XWPSDeviceDVI::makeNamedArray(PSParamString * pname,
		    						XWObject *pcd, 
		    						XWObject * ref)
{
	int code = 0;
	if (pname) 
	{
		code = referNamed(pname, pcd);
		if (code == XWPSError::Undefined)
		{
			code = createNamedArray(pname, pcd);
			XWObject cpy;
			pcd->copy(&cpy);
		}
		XWObject obj;
		char * n = getName(pname);
		if (n)
		{
			core->lookupReference(n, &obj);
			delete [] n;
		}
		if (ref)
			obj.copy(ref);
	}
	else
	{
		pcd->initArray(xref);
		if (ref)
			xref->refObj(pcd, ref);
	}
	
	return code;
}

int XWPSDeviceDVI::makeNamedDict(PSParamString * pname,
		    						XWObject *pcd, 
		    						XWObject * ref)
{
	int code = 0;
	if (pname) 
	{
		code = referNamed(pname, pcd);
		if (code == XWPSError::Undefined)
		{
			code = createNamedDict(pname, pcd);
			XWObject cpy;
			pcd->copy(&cpy);
		}
		XWObject obj;
		char * n = getName(pname);
		if (n)
		{
			core->lookupReference(n, &obj);
			delete [] n;
		}
		if (ref)
			obj.copy(ref);
	}
	else
	{
		pcd->initDict(xref);
		if (ref)
			xref->refObj(pcd, ref);
	}
	
	return code;
}

int XWPSDeviceDVI::makeNamedStream(PSParamString * pname,
		    						XWObject *pcd, 
		    						XWObject * ref)
{
	int code = 0;
	if (pname) 
	{
		code = referNamed(pname, pcd);
		if (code == XWPSError::Undefined)
		{
			code = createNamedStream(pname, pcd);
			XWObject cpy;
			pcd->copy(&cpy);
		}
		
		XWObject obj;
		char * n = getName(pname);
		if (n)
		{
			core->lookupReference(n, &obj);
			delete [] n;
		}
		if (ref)
			obj.copy(ref);
	}
	else
  {
		pcd->initStream(0, xref);
		if (ref)
			xref->refObj(pcd, ref);
	}
	
	return code;
}

int XWPSDeviceDVI::noneToStream()
{
	if (!file)
	{
		file = xref->getCurrentPageFile();
	
		openFile(1024);	
		strm->close_file = false;
	}
	return PDF_IN_STREAM;
}

int  XWPSDeviceDVI::openContents(PSPDFContext contextA)
{
	context_proc_dvi proc;
   while ((proc = context_procs_dvi[context][contextA]) != 0) 
   {
			int code = (this->*proc)();
			if (code < 0)
	    	return code;
			context = (PSPDFContext)code;
   }
   context = contextA;
   return 0;
}

int  XWPSDeviceDVI::openGState(XWPSDVIResource **ppres)
{
	if (*ppres)
		return 0;
		
  return beginResource(resourceExtGState, ps_no_id, ppres);
}

int  XWPSDeviceDVI::openPage(PSPDFContext contextA)
{
  return openContents(contextA);
}

void XWPSDeviceDVI::pageIDs()
{
	int page_no = xref->currentPageNumber();
	page_no++;
	if (page_no >= num_pages)
	{
		uint new_num_pages = num_pages + 10;
  	pages = (XWPSDVIPage*)realloc(pages, new_num_pages * sizeof(XWPSDVIPage));
  	for (int i = num_pages; i < new_num_pages; i++)
  		pageReset(&pages[i]);
  	num_pages = new_num_pages;
	}
	else
	{
		pages[page_no].xobjCounter = 0;
		pages[page_no].csCounter = 0;
		pages[page_no].gsCounter = 0;
		pages[page_no].patternCounter = 0;
		pages[page_no].shadingCounter = 0;
		pages[page_no].functionCounter = 0;
	}
}

void XWPSDeviceDVI::pageReset(XWPSDVIPage * p)
{
  p->xobjCounter = 0;
  p->csCounter = 0;
  p->gsCounter = 0;
  p->patternCounter = 0;
  p->shadingCounter = 0;
  p->functionCounter = 0;
  p->xobjLabels = 0;
  p->csLabels = 0;
  p->gsLabels = 0;
  p->patternLabels = 0;
  p->shadingLabels = 0;
  p->functionLabels = 0;
}

int XWPSDeviceDVI::pattern(XWPSDeviceColor *pdc,
	    						XWPSColorTile *p_tile, 
	    						XWPSColorTile *m_tile,
	    						XWObject *pcs_image, 
	    						XWPSDVIResource **ppres)
{
	XWPSColorTile *tile = (p_tile ? p_tile : m_tile);
  XWPSStripBitmap *btile = (p_tile ? &p_tile->tbits : &m_tile->tmask);
  uint p_size =	(p_tile == 0 ? 0 : p_tile->tbits.tileSize(color_info.depth));
  uint m_size =	(m_tile == 0 ? 0 : m_tile->tmask.tileSize(1));
  bool mask = p_tile == 0;
  XWPSPoint step;
  XWPSMatrix smat;
  
  if (qMax(p_size, m_size) > 65500)
		return (int)(XWPSError::LimitCheck);
			
	if (tile->step_matrix.isXXYY())
		step.x = tile->step_matrix.xx, step.y = tile->step_matrix.yy;
  else if (tile->step_matrix.isXYYX())
		step.x = tile->step_matrix.yx, step.y = tile->step_matrix.xy;
  else
		return (int)(XWPSError::RangeCheck);
			
	smat.xx = btile->rep_width / (HWResolution[0] / 72.0);
  smat.yy = btile->rep_height / (HWResolution[1] / 72.0);
  
	XWPSDVIResource *pres;
  int code = allocResource(resourcePattern, pdc->mask.id, ppres);
  if (code < 0)
		return code;
	pres = *ppres;
  XWObject pcd_Resources;
  pcd_Resources.initDict(xref);
  {
		XWObject pcd_XObject;
		pcd_XObject.initDict(xref);
		char key[MAX_REF_CHARS + 3];

		sprintf(key, "/R%ld", pcs_image->label);
		pcd_XObject.dictAdd(qstrdup(key), pcs_image);
	  pcd_Resources.dictAdd(qstrdup("XObject"), &pcd_XObject);
  }
  
  XWObject procset, obj, a;
  procset.initArray(xref);
  obj.initName("PDF");
  procset.arrayAdd(&obj);
  if (mask)
  	obj.initName("ImageB");
  else
  	obj.initName("ImageC");
  procset.arrayAdd(&obj);
  pcd_Resources.dictAdd(qstrdup("ProcSet"), &procset);
  XWDict * pcd = pres->object.streamGetDict();
  obj.initInt(1);
  pcd->add(qstrdup("PatternType"), &obj);
  if (mask)
  	obj.initInt(2);
  else
  	obj.initInt(1);
  pcd->add(qstrdup("PaintType"), &obj);
  obj.initInt(tile->tiling_type);
  pcd->add(qstrdup("TilingType"), &obj);  
  
  xref->refObj(&pcd_Resources, &obj);
  if (!xref->isScanning())
		xref->releaseObj(&pcd_Resources);
	else
		pcd_Resources.free();
			
  pcd->add(qstrdup("Resources"), &obj);
  
  a.initArray(xref);
  obj.initInt(0);
  a.arrayAdd(&obj);
  obj.initInt(0);
  a.arrayAdd(&obj);
  obj.initInt(1);
  a.arrayAdd(&obj);
  obj.initInt(1);
  a.arrayAdd(&obj);
  
  pcd->add(qstrdup("BBox"), &a);
  putMatrix(pcd, "Matrix", &smat);
  
  obj.initReal(step.x / btile->rep_width);
  pcd->add(qstrdup("XStep"), &obj);
  obj.initReal(step.y / btile->rep_height);
  pcd->add(qstrdup("YStep"), &obj);
  
  {
		char buf[MAX_REF_CHARS + 6 + 1];

		sprintf(buf, " /R%ld Do\n", pcs_image->label);
		pres->object.streamAdd((const char *)buf, strlen(buf));
  }

  return 0;
}

int  XWPSDeviceDVI::patternSpace(XWObject *pvalue,
		  							XWPSDVIResource **ppres, 
		  							const char *cs_name)
{
	if (!*ppres) 
	{
		beginResourceBody(resourceColorSpace, ps_no_id, ppres);
		XWObject obj;
		obj.initName(cs_name);
		(*ppres)->object.arrayAdd(&obj);
  }
  (*ppres)->indirect.copy(pvalue);
  return 0;
}

void XWPSDeviceDVI::pdfmarkAdjustParentCount(XWPSDVIOutlineLevel * plevel)
{
	XWPSDVIOutlineLevel *parent = plevel - 1;
  int count = plevel->last.count;

  if (count > 0) 
  {
		if (parent->last.count < 0)
	    parent->last.count -= count;
		else
	    parent->last.count += count;
  }
}

int  XWPSDeviceDVI::pdfmarkANN(PSParamString *pairs, 
	                uint count, 
	                XWPSMatrix *pctm, 
	                PSParamString *objname)
{
	if (!(xref->isScanning()))
		return 0;
		
	return pdfmarkAnnot(pairs, count, pctm, objname, "Text");
}

int  XWPSDeviceDVI::pdfmarkCLOSE(PSParamString * pairs, 
	                  uint ,
	                  XWPSMatrix * , 
	                  PSParamString * )
{
	if (!(xref->isScanning()))
		return 0;
		
	char * buf = getName(pairs);
	if (buf)
	{
		XWObject obj;
		if (0 != core->lookupReference(buf, &obj))
			core->flushObject(buf);
		else
		{
			int len = strlen(buf);
			for (int i = 0; i < len; i++)
			{
			  if (isupper(buf[i]))
			  	buf[i] = tolower(buf[i]);
			}
			if (0 != core->lookupReference(buf, &obj))
			  core->flushObject(buf);
		}
		
		delete [] buf;
	}
	return 0;
}

int  XWPSDeviceDVI::pdfmarkCloseOutline()
{
	int depth = outline_depth;
  XWPSDVIOutlineLevel *plevel = &outline_levels[depth];
  

  if (depth > 0) 
  {
		plevel[-1].last.last_id = plevel->last.id;
		pdfmarkAdjustParentCount(plevel);
		--plevel;
		if (plevel->last.count < 0)
	    closed_outline_depth--;
		outline_depth--;
  }
  return 0;
}

int  XWPSDeviceDVI::pdfmarkAnnot(PSParamString * pairs, 
	                  uint count,
	                  XWPSMatrix * pctm, 
	                  PSParamString *objname,
	                  const char *subtype)
{
	PSDVIAOParams params;
 
  XWObject annots, ref, obj;
  int code;
  
  params.pdev = this;
  params.subtype = subtype;
  params.src_pg = xref->currentPageNumber();
  code = makeNamedDict(objname, &annots, &ref);
  if (code != XWPSError::Undefined)
  	return code;
  	
  obj.initName("Annot");
  annots.dictAdd(qstrdup("Type"), &obj);
  XWPSRect rect;
  code = pdfmarkPutAoPairs(annots.getDict(), pairs, count, pctm, &params, false, &rect);
  if (code < 0)
		return code;
		
	PDFRect r;
	r.llx = rect.p.x;
	r.lly = rect.p.y;
	r.urx = rect.q.x;
	r.ury = rect.q.y;
	
	xref->addAnnot(dev,params.src_pg, &r, &annots, 1);
	xref->releaseObj(&annots);
	return 0;
}

int XWPSDeviceDVI::pdfmarkARTICLE(PSParamString * pairs, 
	                   uint count,
		                 XWPSMatrix * pctm, 
		                 PSParamString * )
{
	if (!(xref->isScanning()))
		return 0;
		
	PSParamString title;
  PSParamString rectstr;
  XWPSRect rect;
  long bead_id;
  XWPSDVIArticle *part;
  int code;

  if (!pdfmarkFindKey("/Title", pairs, count, &title) ||
			!pdfmarkFindKey("/Rect", pairs, count, &rectstr))
		return (int)(XWPSError::RangeCheck);
  if ((code = pdfmarkScanRect(&rect, &rectstr, pctm)) < 0)
		return code;
		
	bead_id = ++next_id;
	for (part = articles; part != 0; part = part->next) 
  {
		XWObject *a_title = part->contents.dictLookupNFOrg("Title");
		if (a_title && a_title->isString())
		{
			XWString * s = a_title->getString();
			char * ss = s->getCString();
			if (!bytes_compare((const uchar*)ss, s->getLength(), title.data, title.size))
				break;
		}
  }
  
  if (part == 0) 
  {
		part = new XWPSDVIArticle;
		part->contents.initDict(xref);
		part->id = ++next_id;
		part->next = articles;
		articles = part;
		XWObject obj;
		obj.initString(new XWString((char*)(title.data + 1), title.size - 2));
		part->contents.dictAdd(qstrdup("Title"), &obj);
  }
  
  if (part->last.id == 0) 
  {
		part->first.next_id = bead_id;
		part->last.id = part->first.id;
  } 
  else 
  {
		part->last.next_id = bead_id;
		pdfmarkWriteBead(&part->last);
  }
  part->last.prev_id = part->last.id;
  part->last.id = bead_id;
  part->last.article_id = part->id;
  part->last.next_id = 0;
  part->last.rect = rect;
  
  {
		PSParamString page_string;
		int page = 0;
		if (pdfmarkFindKey("/Page", pairs, count, &page_string))
	    page = pdfmarkPageNumber(&page_string);
		
		for (uint i = 0; i < count; i += 2) 
		{
	    if (pairs[i].pdfKeyEq("/Rect") || pairs[i].pdfKeyEq("/Page"))
				continue;
	    pdfmarkPutPair(part->contents.getDict(), &pairs[i]);
		}
  }
  if (part->first.id == 0) 
  {
		part->first = part->last;
		part->last.id = 0;
  }
  
  pdfmarkWriteArticle(part);
  
  return 0;
}

int XWPSDeviceDVI::pdfmarkBP(PSParamString * pairs, 
	               uint count,
	               XWPSMatrix * , 
	               PSParamString * objname)
{
	if (!(xref->isScanning()))
		return 0;
		
	if (objname == 0 || count != 2 || !pairs[0].pdfKeyEq("/BBox"))
		return (int)(XWPSError::RangeCheck);
			
	XWPSRect bbox;
	if (sscanf((const char *)pairs[1].data, "[%lg %lg %lg %lg]",
	       &bbox.p.x, &bbox.p.y, &bbox.q.x, &bbox.q.y) != 4)
		return (int)(XWPSError::RangeCheck);
	
  XWObject obj;
  XWPSDVIGraphicsSave *pdgs = new XWPSDVIGraphicsSave;
  if (context != PDF_IN_NONE) 
  {
		int code = openPage(PDF_IN_STREAM);
		if (code < 0) 
		{
	    delete pdgs;
	    return code;
		}
  }
  pdgs->prev = open_graphics;
  open_graphics = pdgs;
  context = PDF_IN_STREAM;
  
  makeNamedStream(objname, &(pdgs->object), &(pdgs->indirect));
  
  XWDict * pcs = pdgs->object.streamGetDict();
  int l = 1024;
  uchar * buf = (uchar*)malloc(l * sizeof(uchar*));
  pdgs->strm = new XWPSStream;
  pdgs->strm->close_file = false;
  pdgs->strm->writeFile(pdgs->object.streamGetIO(), buf, l, true, false);
  pdgs->save_strm = strm;
  iswritestrm = true;
  innaming = true;
  strm = pdgs->strm;
  obj.initName("XObject");
  pcs->add(qstrdup("Type"), &obj);
  obj.initName("Form");
  pcs->add(qstrdup("Subtype"), &obj);
  obj.initInt(1);
  pcs->add(qstrdup("FormType"), &obj);
  
  XWObject a, a1;
  a.initArray(xref);
  obj.initInt(1);
  a.arrayAdd(&obj);
  obj.initInt(0);
  a.arrayAdd(&obj);
  obj.initInt(0);
  a.arrayAdd(&obj);
  obj.initInt(1);
  a.arrayAdd(&obj);
  obj.initInt(0);
  a.arrayAdd(&obj);
  obj.initInt(0);
  a.arrayAdd(&obj);
  pcs->add(qstrdup("Matrix"), &a);
  
  double xscale = HWResolution[0] / 72.0,	yscale = HWResolution[1] / 72.0;
  a1.initArray(xref);
  obj.initReal(bbox.p.x * xscale);
  a1.arrayAdd(&obj);
  obj.initReal(bbox.p.y * yscale);
  a1.arrayAdd(&obj);
  obj.initReal(bbox.q.x * xscale);
  a1.arrayAdd(&obj);
  obj.initReal(bbox.q.y * yscale);
  a1.arrayAdd(&obj);
  pcs->add(qstrdup("BBox"), &a1);
  return 0;
}

int  XWPSDeviceDVI::pdfmarkDEST(PSParamString * pairs, 
	                 uint count,
	                 XWPSMatrix * , 
	                 PSParamString * )
{
	if (!(xref->isScanning()))
		return 0;
		
	int present;
  XWObject dest;
  PSParamString key;
  dest.initArray(xref);
  if (!pdfmarkFindKey("/Dest", pairs, count, &key) ||
			(present = pdfmarkMakeDest(&dest, "/Page", "/View", pairs, count)) < 0)
	{
		dest.free();
		return (int)(XWPSError::RangeCheck);
	}
			
	xref->addNames("Dests", key.data + 1, key.size - 1, &dest);
	return 0;
}

int XWPSDeviceDVI::pdfmarkDOCINFO(PSParamString * pairs, 
	                   uint count,
		                 XWPSMatrix * , 
		                 PSParamString * )
{
	if (!(xref->isScanning()))
		return 0;
		
	XWObject * pcd = xref->getDocInfo();
	int code = 0;
  if (count & 1)
		return (int)(XWPSError::RangeCheck);
  for (int i = 0; code >= 0 && i < count; i += 2)
  {
  	PSParamString *pair = pairs + i;
		PSParamString alt_pair[2];
#define VDATA alt_pair[1].data
#define VSIZE alt_pair[1].size
		uchar *str = 0;
		PSParamString * tmpa = pairs + i;
		if (tmpa->pdfKeyEq("/Producer"))
		{
			XWPSStringMatchParams params;
			alt_pair[0] = pairs[i];
			alt_pair[1] = pairs[i + 1];
			if (params.stringMatch(VDATA, VSIZE, (const uchar *)"*Distiller*", 11) ||
					params.stringMatch(VDATA, VSIZE, (const uchar *)"*\000D\000i\000s\000t\000i\000l\000l\000e\000r*", 20))
			{
				char buf[PDF_MAX_PRODUCER];
				int len;
				uint j = VSIZE;
				for (; j > 0 && VDATA[--j] != '+'; )
		    	;
				if (VSIZE - j > 2 && VDATA[j] == '+') 
				{
		    	++j;
		    	while (j < VSIZE && VDATA[j] == ' ')
					++j;
				}
				
				storeDefaultProducer(buf);
				len = strlen(buf) - 1;
				str = new uchar[j + len];
				memcpy(str, VDATA, j);
				memcpy(str + j, buf + 1, len);
				if (!(alt_pair[1].persistent) && alt_pair[1].data)
					delete [] alt_pair[1].data;
				VDATA = str;
				VSIZE = j + len;
				alt_pair[1].persistent = false;
				pair = alt_pair;
			}			
		}
		code = pdfmarkPutPair(pcd->getDict(), pair);
#undef VDATA
#undef VSIZE
  }
  return code;
}

int XWPSDeviceDVI::pdfmarkDOCVIEW(PSParamString * pairs, 
	                   uint count,
		                 XWPSMatrix * , 
		                 PSParamString * )
{
	if (!(xref->isScanning()))
		return 0;
		
	XWObject * pcd = xref->getCatalog();
	XWObject dest;
	
  if (count & 1)
		return (int)(XWPSError::RangeCheck);
	dest.initArray(xref);
  if (pdfmarkMakeDest(&dest, "/Page", "/View", pairs, count)) 
  {
		pcd->dictAdd(qstrdup("OpenAction"),&dest);
		for (uint i = 0; i < count; i += 2)
	    if (!(pairs[i].pdfKeyEq("/Page") || pairs[i].pdfKeyEq("/View")))
				pdfmarkPutPair(pcd->getDict(), pairs + i);
		return 0;
  } 
  else
  {
  	dest.free();
		return pdfmarkPutPairs(pcd->getDict(), pairs, count);
	}
}

int  XWPSDeviceDVI::pdfmarkEMC(PSParamString *, 
	               uint , 
	               XWPSMatrix *, 
	               PSParamString *)
{
	return 0;
}

int XWPSDeviceDVI::pdfmarkEP(PSParamString * , 
	              uint count,
	              XWPSMatrix * , 
	              PSParamString * )
{
	if (!(xref->isScanning()))
		return 0;
		
	XWPSDVIGraphicsSave *pdgs = open_graphics;
	XWPSDVIResource *pres;
	if (count != 0 || pdgs == 0)
		return (int)(XWPSError::RangeCheck);
	int code = openContents(PDF_IN_STREAM);
  if (code < 0)
		return code;
	strm->flush();
	strm = pdgs->save_strm;
	iswritestrm = false;
	innaming = false;
  pdgs->object.free();
	open_graphics = pdgs->prev;
	delete pdgs;
	return 0;
}

bool XWPSDeviceDVI::pdfmarkFindKey(const char *key, 
	                    PSParamString * pairs, 
	                    uint count,
		                  PSParamString * pstr)
{
	for (int i = 0; i < count; i += 2)
		if (pairs[i].pdfKeyEq(key)) 
		{
	    *pstr = pairs[i + 1];
	    return true;
		}
  return false;
}

int XWPSDeviceDVI::pdfmarkLNK(PSParamString *pairs, 
	                uint count, 
	                XWPSMatrix *pctm, 
	                PSParamString *objname)
{
	if (!(xref->isScanning()))
		return 0;
		
	return pdfmarkAnnot(pairs, count, pctm, objname, "Link");
}

int  XWPSDeviceDVI::pdfmarkMakeDest(XWObject * dest,
		  								 const char *Page_key, 
		  								 const char *View_key,
		                   PSParamString * pairs, 
		                   uint count)
{
	PSParamString page_string, view_string, action;
  int present =	pdfmarkFindKey(Page_key, pairs, count, &page_string) + pdfmarkFindKey(View_key, pairs, count, &view_string);
  XWObject obj;
  int page = pdfmarkPageNumber(&page_string);
  
  if (page == 0)
  	obj.initNull();
  else if (pdfmarkFindKey("/Action", pairs, count, &action) && action.pdfKeyEq("/GoToR"))
  	obj.initInt(page - 1);
  else
  	xref->refPage(page, &obj);
  dest->arrayAdd(&obj);
  if (view_string.size == 0)
  {
  	obj.initName("XYZ");
  	dest->arrayAdd(&obj);
  	obj.initInt(0);
  	dest->arrayAdd(&obj);
  	dest->arrayAdd(&obj);
  	obj.initInt(1);
  	dest->arrayAdd(&obj);
  }
  else
  {
  	uchar * vstr = view_string.data;
  	vstr++;
  	XWLexer lexer((const char*)(vstr), view_string.size - 1);
		const char * p = lexer.skipWhite();
		const char * endptr = lexer.getEndPtr();
		while (endptr > p && endptr[0] != ']')
		    endptr--;
		XWDVIParser parser(xref, &lexer);
		while (p < endptr)
		{
			parser.parsePDFObject(&obj);
			dest->arrayAdd(&obj);
			p = lexer.skipWhite();
		}
  }
  return present;
}

int XWPSDeviceDVI::pdfmarkNamespacePop(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int XWPSDeviceDVI::pdfmarkNamespacePush(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

const uchar * XWPSDeviceDVI::pdfmarkNextObject(const uchar * scan, 
	                                const uchar * end, 
	                                const uchar **pname,
		                              XWObject *ppco)
{
	int code;
	ppco->initNull();
  while ((code = scanToken(&scan, end, pname)) != 0) 
  {
  	{
			PSParamString sname;

			if (code < 0) 
			{
	    	++scan;
	    	continue;
			}
			if (**pname != '{')
	    	continue;
	    
	  	scan = *pname;
			code = scanTokenComposite(&scan, end, pname);
			if (code < 0) 
			{
	    	++scan;
	    	continue;
			}
			sname.data = (uchar*)(*pname);
			sname.size = scan - sname.data;
			code = getRefence(&sname, ppco);
			if (code < 0)
	    	continue;
			return scan;
		}
	}
  return end;
}

int  XWPSDeviceDVI::pdfmarkNI(PSParamString *, 
	               uint , 
	               XWPSMatrix *, 
	               PSParamString *)
{
	return 0;
}

int  XWPSDeviceDVI::pdfmarkOBJ(PSParamString * pairs, 
	                uint count,
	                XWPSMatrix * , 
	                PSParamString * objname)
{
	if (!(xref->isScanning()))
		return 0;
		
	XWObject pco;

  if (objname == 0 || count != 2 || !pairs[0].pdfKeyEq("/type"))
		return (int)(XWPSError::RangeCheck);
			
  if (pairs[1].pdfKeyEq("/array"))
		makeNamedArray(objname, &pco, 0);
  else if (pairs[1].pdfKeyEq("/dict"))
		makeNamedDict(objname, &pco, 0);
  else if (pairs[1].pdfKeyEq("/stream"))
		makeNamedStream(objname, &pco, 0);
	pco.free();
  return 0;
}

int  XWPSDeviceDVI::pdfmarkOUT(PSParamString * pairs, 
	                uint count,
	                XWPSMatrix * pctm, 
	                PSParamString * )
{
	if (!(xref->isScanning()))
		return 0;
		
	int depth = outline_depth;
  XWPSDVIOutlineLevel *plevel = &outline_levels[depth];
  int sub_count = 0;
  uint i;
  XWPSDVIOutlineNode node;
  PSDVIAOParams ao;
  int code;
  
  for (i = 0; i < count; i += 2) 
  {
		PSParamString *pair = &pairs[i];

		if (pair->pdfKeyEq("/Count"))
	    (pair + 1)->pdfmarkScanInt(&sub_count);
  }
  if (sub_count != 0 && depth == MAX_OUTLINE_DEPTH - 1)
		return (int)(XWPSError::LimitCheck);
			
  ao.pdev = this;
  ao.subtype = 0;
  ao.src_pg = -1;
  
  XWObject action;
  action.initDict(xref);
  XWPSRect rect;
  code = pdfmarkPutAoPairs(action.getDict(), pairs, count, pctm, &ao, true, &rect);
  if (code < 0)
  {
  	action.free();
		return code;
	}
	
	if (outlines_id == 0)
		outlines_id = ++next_id;
	
	node.id = ++next_id;
  node.parent_id = (depth == 0 ? outlines_id : plevel[-1].last.id);
  node.prev_id = plevel->last.id;
  node.count = sub_count;
  if (plevel->first.id == 0) 
  {
		if (depth > 0)
	    plevel[-1].last.first_id = node.id;
			node.prev_id = 0;
			plevel->first = node;
			
  } 
  else 
  {
		if (depth > 0)
	    pdfmarkAdjustParentCount(plevel);
  }
  plevel->last = node;
  plevel->left--;
  
	if (!closed_outline_depth)
		outlines_open++;
	if (sub_count != 0) 
	{
		outline_depth++;
		++plevel;
		plevel->left = (sub_count > 0 ? sub_count : -sub_count);
		plevel->first.id = 0;
		if (sub_count < 0)
	    closed_outline_depth++;
	  xref->bookmarksDown();
	}
	else
	{
		while ((depth = outline_depth) > 0 && outline_levels[depth].left == 0 )
		{
	    pdfmarkCloseOutline();		
	    xref->bookmarksUp();
	  }
	}
	
	xref->bookmarksAdd(&action, 0);
	return 0;
}

int  XWPSDeviceDVI::pdfmarkPAGE(PSParamString * pairs, 
	                 uint count,
	                 XWPSMatrix * , 
	                 PSParamString * )
{
	if (!(xref->isScanning()))
		return 0;
		
	XWObject * pa = xref->getThisPage();
	return pdfmarkPutPairs(pa->getDict(), pairs, count);
}

int XWPSDeviceDVI::pdfmarkPageNumber(PSParamString * pnstr)
{
	int page = xref->currentPageNumber();
	if (pnstr->pdfKeyEq("/Next"))
		page++;
	else if (pnstr->pdfKeyEq("/Prev"))
		page--;
	else if (pnstr->pdfmarkScanInt(&page) < 0)
		page = 0;
	
	return page;
}

int  XWPSDeviceDVI::pdfmarkPAGES(PSParamString * pairs, 
	                  uint count,
	                  XWPSMatrix * , 
	                  PSParamString * )
{
	if (!(xref->isScanning()))
		return 0;
		
	XWObject * pas = xref->getPageTree();
	return pdfmarkPutPairs(pas->getDict(), pairs, count);
}	

int  XWPSDeviceDVI::pdfmarkProcess(PSParamStringArray * pma)
{
	PSParamString *data = pma->data;
  uint size = pma->size;
  PSParamString *pts = &data[size - 1];
  PSParamString *objname = 0;
  XWPSMatrix ctm;
  PSDVIMarkName *pmn;
  int code = 0;

  if (size < 2 || sscanf((const char *)pts[-1].data, "[%g %g %g %g %g %g]",
	       &ctm.xx, &ctm.xy, &ctm.yx, &ctm.yy, &ctm.tx, &ctm.ty) != 6)
		return (int)(XWPSError::RangeCheck);
		
  {
		double xscale = 72.0 / HWResolution[0], yscale = 72.0 / HWResolution[1];

		ctm.xx *= xscale, ctm.xy *= yscale;
		ctm.yx *= xscale, ctm.yy *= yscale;
		ctm.tx *= xscale, ctm.ty *= yscale;
  }
  size -= 2;
  for (pmn = mark_names; pmn->mname != 0; ++pmn)
		if (pts->pdfKeyEq(pmn->mname)) 
		{
	    int odd_ok = (pmn->options & PDFMARK_ODD_OK) != 0;
	    PSParamString *pairs;
	    int j;

	    if (size & !odd_ok)
				return (int)(XWPSError::RangeCheck);
	    if (pmn->options & PDFMARK_NAMEABLE) 
	    {
				for (j = 0; j < size; j += 2) 
				{
		    	if (data[j].pdfKeyEq("/_objdef")) 
		    	{
						objname = &data[j + 1];
						if (!objnameIsValid(objname->data, objname->size))
			    		return (int)(XWPSError::RangeCheck);
						size -= 2;
						pairs = new PSParamString[size];
						for (int i = 0; i < j; i++)
						   pairs[i] = data[i];
						   
						for (int i = 0; i < (size - j); i++)
						   pairs[i + j] = data[i + j + 2];
						   
						goto copied;
		    	}
				}
	    }
	    
	    pairs = new PSParamString[size];
	    for (int i = 0; i < size; i++)
	      pairs[i] = data[i];
	
copied:	
	    if (!(pmn->options & PDFMARK_NO_REFS)) 
	    {
				for (j = (pmn->options & PDFMARK_KEEP_NAME ? 1 : 1 - odd_ok); j < size; j += 2 - odd_ok) 
				{
		    	code = replaceNames(&pairs[j], &pairs[j]);
		    	if (code < 0) 
		    	{
		    		delete [] pairs;
						return code;
		    	}
				}
	    }
	    code = (this->*(pmn->pdfmark_proc))(pairs, size, &ctm, objname);
	    delete [] pairs;
	    break;
		}
  return code;
}

int  XWPSDeviceDVI::pdfmarkPS(PSParamString * pairs, 
	               uint count,
	               XWPSMatrix * , 
	               PSParamString * objname)
{
	PSParamString source;
  PSParamString level1;

  if (!pdfmarkFindKey("/DataSource", pairs, count, &source) ||
			!psSourceOk(&source) ||
			(pdfmarkFindKey("/Level1", pairs, count, &level1) &&
	 		!psSourceOk(&level1)))
		return (int)(XWPSError::RangeCheck);
  if (level1.data == 0 && source.size <= MAX_PS_INLINE &&
			CompatibilityLevel >= 1.2 && objname == 0) 
	{
		int code = openContents(PDF_IN_STREAM);
		if (code < 0)
	    return code;
	  if (canWriteStream())
	  {
			XWPSStream *s = strm;
			s->write(source.data, source.size);
			s->pputs(" PS\n");
		}
  }
  else
  {
  	XWPSDVIResource *pres;
  	uint size;
		int code;
		code = allocResource(resourceXObject, ps_no_id, &pres);
		if (code < 0)
	    return code;
	    
	  int l = 1024;
	  uchar * buf = new uchar[l];
	  XWObject obj;	  
	  XWDict * pcd = pres->object.streamGetDict();	  	
	  obj.initName("PS");
	  pcd->add(qstrdup("Subtype"), &obj);
	 	if (level1.data != 0) 
	  {
	  	XWObject ll;
	  	ll.initStream(STREAM_COMPRESS, xref);
	  	
	  	XWPSStream ss;
	  	ss.close_file = false;
	  	ss.writeFile(ll.streamGetIO(), buf, l, false, false);
	  	size = pdfmarkWritePs(&ss, &level1);
	  	xref->refObj(&ll, &obj);
	  	pcd->add(qstrdup("Level1"), &obj);
	  	if (!xref->isScanning())
	  	  xref->releaseObj(&ll);
	  	else
	  		ll.free();
		}
		
		XWPSStream s;
		s.close_file = false;
		s.writeFile(pres->object.streamGetIO(), buf, l, false, false);
		pdfmarkWritePs(&s, &source);
		delete [] buf;
		code = openContents(PDF_IN_STREAM);
		if (code < 0)
	    return code;
	  if (canWriteStream())
		  strm->print(" /R%ld Do\n", pres->indirect.label);
  }
  return 0;
}

int  XWPSDeviceDVI::pdfmarkPUT(PSParamString * pairs, 
	                uint count,
	                XWPSMatrix * , 
	                PSParamString * )
{
	if (!(xref->isScanning()))
		return 0;
		
	if (count != 3)
		return (int)(XWPSError::RangeCheck);
	
	XWObject pco, obj;
	int code = 0;
	if ((code = getNamed(&pairs[0], &pco)) < 0)
	{
		pco.free();
		return code;
	}
	
	if (!pco.isArray())
	{
		pco.free();
		return 0;
	}
		
	int i = 0;
	if ((code = pairs[1].pdfmarkScanInt(&i)) < 0)
		return code;
		
  getObject(&pairs[2], &obj);
	
	if (i >= pco.arrayGetLength())	
		pco.arrayAdd(&obj);
	else
	{
		XWObject lst;
		pco.arrayGet(pco.arrayGetLength() - 1, &lst);
		
		XWObject * n = 0;
		XWObject * p = 0;
		for (int k = pco.arrayGetLength() - 1; k >= i; k--)
		{
			n = pco.arrayGetOrg(k);
			p = pco.arrayGetOrg(k - 1);
			n->free();
			p->copy(n);
			p->free();
		}
		if (p)
			obj.copy(p);
		obj.free();
		pco.arrayAdd(&lst);
	}
	pco.free();
	return 0;
}

int  XWPSDeviceDVI::pdfmarkPutAoPairs(XWDict *pcd,
		                     PSParamString * pairs, 
		                     uint count,
		                     XWPSMatrix * pctm, 
		                     PSDVIAOParams * params,
		                     bool for_outline,
		                     XWPSRect * prect)
{
	PSParamString *Action = 0;
  PSParamString *File = 0;
  int code;
  XWObject Dest, Subtype, obj;
  Dest.initNull();
  bool coerce_dest = false;
  
  if (!for_outline) 
  {
  	Dest.initArray(xref);
  	code = pdfmarkMakeDest(&Dest,"/Page", "/View", pairs, count);
  	if (code < 0)
  	{
  		Dest.free();
	    return code;
	  }
	  else if (code == 0)
	  {
	  	Dest.free();
	  	Dest.initNull();
	  }
  }
  if (params->subtype)
		Subtype.initName(params->subtype);
		
	for (uint i = 0; i < count; i += 2)
	{
		PSParamString *pair = &pairs[i];
		long src_pg;
		
		if (pair->pdfKeyEq("/SrcPg") &&	sscanf((const char *)pair[1].data, "%ld", &src_pg) == 1)
	    params->src_pg = src_pg - 1;
	  else if (!for_outline && pair->pdfKeyEq("/Color"))
	  	pdfmarkPutCPair(pcd, "C", pair + 1);
	  else if (!for_outline && pair->pdfKeyEq("/Title"))
	  	pdfmarkPutCPair(pcd, "T", pair + 1);
	  else if (pair->pdfKeyEq("/Action") || pair->pdfKeyEq("/A"))
	    Action = pair;
		else if (pair->pdfKeyEq("/File") || pair->pdfKeyEq("/F"))
	    File = pair;
		else if (pair->pdfKeyEq("/Dest")) 
		{
			coerce_dest = true;
			Dest.free();
      uchar * p = pair[1].data;
      if (p[0] == '/')
      	Dest.initString(new XWString((char*)(pair[1].data + 1), pair[1].size - 1));
      else
      	getObject(&pair[1], &Dest);
		}
		else if (pair->pdfKeyEq("/Page") || pair->pdfKeyEq("/View"))
		{
			if (Dest.isNull())
			{
				Dest.initArray(xref);
				code = pdfmarkMakeDest(&Dest, "/Page", "/View", pairs, count);
				if (code < 0)
				{
					Dest.free();
		    	return code;
		    }
		    coerce_dest = false;
			}
		}
		else if (pair->pdfKeyEq("/Subtype"))
		{
			Subtype.free();
			getObject(&pair[1], &Subtype);
		}
		else if (pair->pdfKeyEq("/Contents"))
			pdfmarkPutPair(pcd, pair);
		else if (pair->pdfKeyEq("/Rect"))
	  {
	  	XWPSRect rect;
	    
	    int code = pdfmarkScanRect(&rect, pair + 1, pctm);
	    if (code < 0)
				return code;
	    *prect = rect;
	    if (for_outline) 
	    {
	    	XWObject a;
	    	a.initArray(xref);
	    	obj.initReal(rect.p.x);
	   	 	a.arrayAdd(&obj);
	   	 	obj.initReal(rect.p.y);
	   	 	a.arrayAdd(&obj);
	   	 	obj.initReal(rect.q.x);
	   	 	a.arrayAdd(&obj);
	   	 	obj.initReal(rect.q.y);
	   	 	a.arrayAdd(&obj);
	    	pcd->add(qstrdup("Rect"), &a);
	    }
	  }
	  else if (pair->pdfKeyEq("/Border"))
	  {
	  	XWObject a;
	  	a.initArray(xref);
	  	code = pdfmarkWriteBorder(&a, pair + 1, pctm);
	    if (code < 0)
	    	return code;
	    	
	    pcd->add(qstrdup("Border"), &a);
	  }
	  else if (for_outline && pair->pdfKeyEq("/Count"))
	  		;
	  else
	  	pdfmarkPutPair(pcd, pair);
	}
	
	if (Action)
	{
		uchar *astr = Action[1].data;
		uint asize = Action[1].size;
		PSParamString * tmpa = Action + 1;
		if ((File != 0 || !Dest.isNull()) &&
	     (tmpa->pdfKeyEq("/Launch") ||
	     (tmpa->pdfKeyEq("/GoToR") && File) ||
	      tmpa->pdfKeyEq("/Article")))
		{
			XWObject adict;
			adict.initDict(xref);
			if (!for_outline)
			{
				obj.initName("Action");
				adict.dictAdd(qstrdup("Type"), &obj);
			}
			
			if (tmpa->pdfKeyEq("/Article")) 
	  	{
	  		obj.initName("Thread");
				adict.dictAdd(qstrdup("S"), &obj);
				coerce_dest = false;
	    }
	    else
	    	pdfmarkPutCPair(adict.getDict(), "S", tmpa);
	    	
	    if (!Dest.isNull())
	    {
	    	adict.dictAdd(qstrdup("D"), &Dest);
	    	Dest.initNull();
	    }
	    if (File) 
	    {
				pdfmarkPutCPair(adict.getDict(), "F", File + 1);
				File = 0;
	    }	    
	    pcd->add(qstrdup("A"), &adict);
		}
		else if (asize >= 4 && !memcmp(astr, "<<", 2))
		{
			uchar *scan = astr + 2;
	    uchar *end = astr + asize;
			PSParamString key, value;
	    XWObject adict;
	    adict.initDict(xref);
	    
	    while ((code = scanToken((const uchar**)&scan, end, (const uchar**)&key.data)) > 0)
	    {
	    	key.size = scan - key.data;
				if (key.data[0] != '/' || (code = scanTokenComposite((const uchar**)&scan, end, (const uchar**)&value.data)) != 1)
		    	break;
				value.size = scan - value.data;
				if (key.pdfKeyEq("/Dest") || key.pdfKeyEq("/D")) 
				{
		    	key.fromString("D");
		    	if (value.data[0] == '(') 
		    	{
		    		char dest[MAX_DEST_STRING];
						memcpy(dest, value.data + 1, value.size - 1);
  					dest[value.size - 1] = 0;
						obj.initName(dest);
		    	}
		    	else
		    		obj.initString(new XWString((char*)value.data, value.size));
		    	adict.dictAdd(qstrdup("D"), &obj);
				} 
				else if (key.pdfKeyEq("/File"))
					pdfmarkPutCPair(adict.getDict(), "F", &value);
				else if (key.pdfKeyEq("/Subtype"))
					pdfmarkPutCPair(adict.getDict(), "Subtype", &value);
	    }
	    
	    if (code <= 0 || !key.pdfKeyEq(">>"))
	    {
	    	adict.free();
				return (int)(XWPSError::RangeCheck);
			}
			
			pcd->add(qstrdup("A"), &adict);
		}
		else if (Action[1].pdfKeyEq("/GoTo"))
	    pdfmarkPutPair(pcd, Action);
	}
	
	if (!Dest.isNull())
		pcd->add(qstrdup("Dest"), &Dest);
	
	if (File)
		pdfmarkPutPair(pcd, File);
		
	if (Subtype.isName())
		pcd->add(qstrdup("Subtype"), &Subtype);
	else
		Subtype.free();
  return 0;
}

int  XWPSDeviceDVI::pdfmarkPutCPair(XWDict *pcd, 
	                     const char *key,
		                   PSParamString * pvalue)
{
	XWObject obj;
	getObject(pvalue, &obj);
	pcd->add(qstrdup(key), &obj);
	return 0;
}

int  XWPSDeviceDVI::pdfmarkPUTDICT(PSParamString * pairs, 
	                    uint count,
	                    XWPSMatrix * , 
	                    PSParamString * )
{
	if (!(xref->isScanning()))
		return 0;
		
	XWObject pco, obj;
	int code = 0;
	if ((code = getNamed(&pairs[0], &pco)) < 0)
	{
		pco.free();
		return code;
	}
		
	if (pco.isDict())
		code = pdfmarkPutPairs(pco.getDict(), pairs + 1, count - 1);
	else if (pco.isStream())
		code = pdfmarkPutPairs(pco.streamGetDict(), pairs + 1, count - 1);
		
	pco.free();
	return code;
}

int  XWPSDeviceDVI::pdfmarkPUTINTERVAL(PSParamString * pairs, 
	                        uint count,
	                        XWPSMatrix * , 
	                        PSParamString * )
{
	if (!(xref->isScanning()))
		return 0;
		
	if (count < 2)
		return (int)(XWPSError::RangeCheck);
	
	XWObject pco, obj, index;
	int code = 0;
	if ((code = getNamed(&pairs[0], &pco)) < 0)
	{
		pco.free();
		return code;
	}
	
	if (!pco.isArray())
	{
		pco.free();
		return 0;
	}
			
	int i = 0;
	if ((code = pairs[1].pdfmarkScanInt(&i)) < 0)
		return code;
	if (i < 0)
	{
		pco.free();
		return (int)(XWPSError::RangeCheck);
	}
			
	XWObject narr;
	narr.initArray(xref);
	
	for (int k = 2; k < count; k++)
	{
		getObject(&pairs[k], &obj);		
		narr.arrayAdd(&obj);
	}
	
	int len = pco.arrayGetLength();
	if (i >= len)
	{
		for (int k = 0; k < narr.arrayGetLength();k++)
		{
			narr.arrayGet(k, &obj);
			pco.arrayAdd(&obj);
		}
	}
	else
	{
		XWObject oarr;
		oarr.initArray(xref);
	
		for (int k = count - 2; k > 0 ; k--)
  	{
  		pco.arrayGet(len - k, &obj);
  		oarr.arrayAdd(&obj);
		}
		
		XWObject * n = 0;
		XWObject * p = 0;
		if ((count - 2) > 0)
		{
			for (int k = len - 1; k >= i ; k--)
  		{
  			n = pco.arrayGetOrg(k);
  			p = pco.arrayGetOrg(k - (count - 2));
  			n->free();
  			p->copy(n);
  			p->free();
			}
		}
		
		if (p)
		{
			for (int k = 0; k < (count - 2); k++)
			{
				narr.arrayGet(k, &obj);
				n = pco.arrayGetOrg(k + i);
				n->free();
				obj.copy(n);
				obj.free();
			}
		}
		
		for (int k = 0; k < oarr.arrayGetLength(); k++)
		{
			oarr.arrayGet(k, &obj);
			pco.arrayAdd(&obj);
		}
		
		oarr.free();
	}
	
	pco.free();
	narr.free();	
	return code;
}

int  XWPSDeviceDVI::pdfmarkPutPair(XWDict *pcd, PSParamString * pair)
{
	uint size = pair[0].size;
	uchar * vstr = pair[0].data;
	char buf[100];
	if (size > 0 && vstr[0] == '/')
	{
		int len = 0;
		for (uint i = 1; i < size; ++i)
		{
			uint c = vstr[i];
			switch (c)
			{
				default:
					if (c >= 0x21 && c <= 0x7e)
					  buf[len++] = (char)c;
					break;
					
				case '#':
					len += sprintf(buf + len, "#%02x", c);
					break;
					
				case '%':
	      case '(': case ')':
	      case '<': case '>':
	      case '[': case ']':
	      case '{': case '}':
	      case '/':
	      	len += sprintf(buf + len, "#%02x", c);
	      	break;
	      	
	      case 0:
	      	buf[len++] = '?';
	      	break;
			}
		}
		buf[len] = '\0';
	}
	else
	{
		//应该不会走到这儿
	  memcpy(buf, vstr, size);
		buf[size] = '\0';
	}
	XWObject obj;
	getObject(&pair[1], &obj);			
	pcd->add(qstrdup(buf), &obj);
	return 0;
}

int  XWPSDeviceDVI::pdfmarkPutPairs(XWDict *pcd, PSParamString * pairs, uint count)
{
	int code = 0;

  if (count & 1)
		return (int)(XWPSError::RangeCheck);
  for (int i = 0; code >= 0 && i < count; i += 2)
		code = pdfmarkPutPair(pcd, pairs + i);
  return code;
}

int  XWPSDeviceDVI::pdfmarkPUTSTREAM(PSParamString * pairs, 
	                      uint count,
	                      XWPSMatrix * , 
	                      PSParamString * )
{
	if (!(xref->isScanning()))
		return 0;
		
	if (count < 2)
	   return (int)(XWPSError::RangeCheck);
	
	XWObject pco;
	int code = 0;
	if ((code = getNamed(&pairs[0], &pco)) < 0)
	{
		pco.free();
		return code;
	}
	
	if (!pco.isStream())
	{
		pco.free();
		return code;
	}
		
	for (int i = 1; i < count; ++i)
		pco.streamAdd((const char*)pairs[i].data, pairs[i].size);
	
	pco.free();
	return code;
}

int  XWPSDeviceDVI::pdfmarkStAttr(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDeviceDVI::pdfmarkStBDC(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDeviceDVI::pdfmarkStBMC(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDeviceDVI::pdfmarkStBookmarkRoot(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDeviceDVI::pdfmarkStClassMap(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDeviceDVI::pdfmarkStOBJ(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDeviceDVI::pdfmarkStPNE(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDeviceDVI::pdfmarkStPop(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDeviceDVI::pdfmarkStPopAll(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDeviceDVI::pdfmarkStPush(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int XWPSDeviceDVI::pdfmarkStRetrieve(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDeviceDVI::pdfmarkStRoleMap(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDeviceDVI::pdfmarkScanRect(XWPSRect * prect, 
	                     PSParamString * str,
		                   XWPSMatrix * pctm)
{
	uint size = str->size;
  double v[4];
#define MAX_RECT_STRING 100
  char chars[MAX_RECT_STRING + 3];
  int end_check;

  if (str->size > MAX_RECT_STRING)
		return (int)(XWPSError::LimitCheck);
  memcpy(chars, str->data, size);
  strcpy(chars + size, " 0");
  if (sscanf(chars, "[%lg %lg %lg %lg]%d",
	       &v[0], &v[1], &v[2], &v[3], &end_check) != 5)
		return (int)(XWPSError::RangeCheck);
  prect->p.transform(v[0], v[1], pctm);
  prect->q.transform(v[2], v[3], pctm);
  return 0;
}

int  XWPSDeviceDVI::pdfmarkSP(PSParamString * pairs, 
	               uint count,
	               XWPSMatrix * pctm, 
	               PSParamString * )
{
	if (!canWriteStream())
		return 0;
		
	XWObject pco;
  XWPSMatrix ctm;
  int code;

  if (count != 1)
		return (int)(XWPSError::RangeCheck);
			
	code = getRefence(&pairs[0], &pco);
	if (code < 0)
		return code;
		
	code = openContents(PDF_IN_STREAM);
  if (code < 0)
  {
  	pco.free();
		return code;
	}
	ctm = *pctm;
  ctm.tx *= HWResolution[0] / 72.0;
  ctm.ty *= HWResolution[1] / 72.0;
  putMatrix(" q ", &ctm, "cm\n");
  strm->print(" /R%ld Do Q\n", pco.label);
  pco.free();
  return 0;
}

int  XWPSDeviceDVI::pdfmarkStStore(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDeviceDVI::pdfmarkWriteArticle(XWPSDVIArticle * part)
{
	if (part->last.id == 0)
	{
		part->first.prev_id = part->first.next_id = part->first.id;
	}
	else
	{
		part->first.prev_id = part->last.id;
		part->last.next_id = part->first.id;
		pdfmarkWriteBead(&part->last);
	}
	
	pdfmarkWriteBead(&part->first);
	char aid[20];
	sprintf(aid, "art%ld", part->id);
	xref->beginArticle(aid, &part->contents);
	return 0;
}

int  XWPSDeviceDVI::pdfmarkWriteBead(XWPSPDFBead * pbead)
{
	char bid[20];
	char aid[20];
	sprintf(aid, "art%ld", pbead->article_id);
	sprintf(aid, "bead%ld", pbead->id);
	
	PDFRect r;
	r.llx = pbead->rect.p.x;
	r.lly = pbead->rect.p.y;
	r.urx = pbead->rect.q.x;
	r.ury = pbead->rect.q.y;
	xref->addBead(aid, bid, xref->currentPageNumber(), &r);
	return 0;
}

int XWPSDeviceDVI::pdfmarkWriteBorder(XWObject *a, 
	                        PSParamString *str,
		     									XWPSMatrix *pctm)
{
	uint size = str->size;
#define MAX_BORDER_STRING 100
  char chars[MAX_BORDER_STRING + 1];
  double bx, by, c;
  XWPSPoint bpt, cpt;
  const char *next;

  if (str->size > MAX_BORDER_STRING)
		return (int)(XWPSError::LimitCheck);
  memcpy(chars, str->data, size);
  chars[size] = 0;
  if (sscanf(chars, "[%lg %lg %lg", &bx, &by, &c) != 3)
		return (int)(XWPSError::RangeCheck);
  bpt.distanceTransform(bx, by, pctm);
  cpt.distanceTransform(0.0, c, pctm);
  XWObject obj;
  obj.initReal(bx);
  a->arrayAdd(&obj);
  obj.initReal(by);
  a->arrayAdd(&obj);
  obj.initReal(c);
  a->arrayAdd(&obj);
  next = strchr(chars + 1, ']');
  if (next == 0)
		return (int)(XWPSError::RangeCheck);
  if (next[1] != 0)
  {
  	double v;

		while (next != 0 && sscanf(++next, "%lg", &v) == 1) 
		{
	  	XWPSPoint vpt;

	  	vpt.distanceTransform(0.0, v, pctm);
	  	obj.initReal(fabs(vpt.x + vpt.y));
	  	a->arrayAdd(&obj);
	  	next = strchr(next, ' ');
		}
  }
  return 0;
}

uint XWPSDeviceDVI::pdfmarkWritePs(XWPSStream *s, PSParamString * psource)
{
	uint size = psource->size - 2;
  s->write(psource->data + 1, size);
  return size + 1;
}

int  XWPSDeviceDVI::prepareDrawing(XWPSImagerState *pis,
		    							const char *ca_format, 
		    							XWPSDVIResource **ppres)
{
	int code;

  if (CompatibilityLevel >= 1.4) 
  {
		if (state->blend_mode != pis->blend_mode) 
		{
	    static const char *const bm_names[] = { 
	    	"Compatible", "Normal", "Multiply", "Screen", "Difference",
  			"Darken", "Lighten", "ColorDodge", "ColorBurn", "Exclusion",
  			"HardLight", "Overlay", "SoftLight", "Luminosity", "Hue",
  			"Saturation", "Color" };

	    code = openGState(ppres);
	    if (code < 0)
				return code;
			XWObject obj;
			obj.initName(bm_names[pis->blend_mode]);
			(*ppres)->object.dictAdd(qstrdup("BM"), &obj);
	    state->blend_mode = pis->blend_mode;
		}
		code = updateAlpha(pis, ca_format, ppres);
		if (code < 0)
	    return code;
  } 
  else 
  {
		if (pis->opacity.alpha != 1 || 
	    	pis->shape.alpha != 1 || 
	    	pis->transparency_stack != 0)
	    return (int)(XWPSError::RangeCheck);
  }
  return 0;
}

int  XWPSDeviceDVI::prepareFill(XWPSImagerState *pis)
{
	XWPSDVIResource *pres = 0;
  int code = prepareVector(pis, "ca", &pres);
  if (code < 0)
		return code;
		
  if (CompatibilityLevel >= 1.2) 
  {
  	if (!(pres->object.isDict()))
			code = openGState(&pres);
		if (code < 0)
			return code;
		
		XWDict * dict = pres->object.getDict();
		if (params.PreserveOverprintSettings &&
	    	fill_overprint != pis->overprint) 
	  {
	  	XWObject obj;
	    if (CompatibilityLevel < 1.3) 
	    {
	    	obj.initBool(pis->overprint);
	    	dict->add(qstrdup("OP"), &obj);
				stroke_overprint = pis->overprint;
	    } 
	    else 
	    {
	    	obj.initBool(pis->overprint);
	    	dict->add(qstrdup("op"), &obj);
	    }
	    fill_overprint = pis->overprint;
		}
  }
  return 0;
}

int  XWPSDeviceDVI::prepareImage(XWPSImagerState *pis)
{
	XWPSDVIResource *pres = 0;
  int code = prepareDrawing(pis, "ca", &pres);

  if (code < 0)
		return code;
  return 0;
}

int  XWPSDeviceDVI::prepareImageMask(XWPSImagerState *pis,XWPSDeviceColor *pdcolor)
{
	int code = prepareImage(pis);

  if (code < 0)
		return code;
  return setDrawingColor(pdcolor, fill_color, &psdf_set_fill_color_commands);
}

int  XWPSDeviceDVI::prepareStroke(XWPSImagerState *pis)
{
	XWPSDVIResource *pres = 0;
  int code = prepareVector(pis, "CA", &pres);

  if (code < 0)
		return code;
		
  if (CompatibilityLevel >= 1.2) 
  {
  	if (!(pres->object.isDict()))
			code = openGState(&pres);
		if (code < 0)
			return code;
		
		XWDict * dict = pres->object.getDict();
		XWObject obj;
		if (params.PreserveOverprintSettings &&
	    	stroke_overprint != pis->overprint) 
	  {
	    obj.initBool(pis->overprint);
	    dict->add(qstrdup("OP"), &obj);
	    stroke_overprint = pis->overprint;
	    
	    if (CompatibilityLevel < 1.3)
				fill_overprint = pis->overprint;
		}
		if (state->stroke_adjust != pis->stroke_adjust) 
		{
	    obj.initBool(pis->stroke_adjust);
	    dict->add(qstrdup("SA"), &obj);
	    state->stroke_adjust = pis->stroke_adjust;
		}
  }
  return 0;
}

int  XWPSDeviceDVI::prepareVector(XWPSImagerState *pis,
		   								const char *ca_format, 
		   								XWPSDVIResource **ppres)
{
	int code = prepareDrawing(pis, ca_format, ppres);	
  if (code < 0)
		return code;
		
	if (!((*ppres)->object.isDict()))
		code = openGState(ppres);
	if (code < 0)
		return code;
		
	XWDict * dict = (*ppres)->object.getDict();
	if (CompatibilityLevel >= 1.2)
	{
		XWPSIntPoint phase, dev_phase;
		if (params.PreserveHalftoneInfo &&
	    	halftone_id != pis->dev_ht->id )
	  {
	  	code = updateHalftone(pis, dict);
	    if (code < 0)
				return code;
	  }
	  
	  if (params.TransferFunctionInfo == tfi_Preserve) 
	  {
	    code = updateTransfer(pis, dict);
	    if (code < 0)
				return code;
		}
		
		if (params.UCRandBGInfo == ucrbg_Preserve)
		{
			if (black_generation_id != pis->black_generation->id) 
			{
				code = writeTransferMap(pis->black_generation, 0, false, "BG", dict);
				if (code < 0)
		    	return code;
				black_generation_id = pis->black_generation->id;
	    }
	    if (undercolor_removal_id != pis->undercolor_removal->id) 
	    {
				code = writeTransferMap(pis->undercolor_removal, -1, false, "UCR", dict);
				if (code < 0)
		    	return code;
				undercolor_removal_id = pis->undercolor_removal->id;
	    }
		}
		
		pis->currentHalftonePhase(&phase);
		state->currentHalftonePhase(&dev_phase);
		if (dev_phase.x != phase.x || dev_phase.y != phase.y)
		{
			XWObject a, obj;
			a.initArray(xref);
			obj.initInt(phase.x);
			a.arrayAdd(&obj);
			obj.initInt(phase.y);
			a.arrayAdd(&obj);
			dict->add(qstrdup("HTP"), &a);
	    state->setScreenPhase(phase.x, phase.y, ps_color_select_all);
		}
	}
	
	if (CompatibilityLevel >= 1.3) 
  {
		if (overprint_mode != params.OPM) 
		{
	    XWObject obj;
	    obj.initInt(params.OPM);
	    dict->add(qstrdup("OPM"), &obj);
	    overprint_mode = params.OPM;
		}
		if (state->smoothness != pis->smoothness) 
		{
	    XWObject obj;
	    obj.initReal(pis->smoothness);
	    dict->add(qstrdup("SM"), &obj);
	    state->smoothness = pis->smoothness;
		}
		if (CompatibilityLevel >= 1.4) 
		{
	    if (state->text_knockout != pis->text_knockout) 
	    {
				XWObject obj;
				obj.initBool(pis->text_knockout);
				dict->add(qstrdup("TK"), &obj);
				state->text_knockout = pis->text_knockout;
	    }
		}
  }
  return code;
}

int XWPSDeviceDVI::putClipPath(XWPSClipPath * pcpath)
{
	XWPSStream *s = strm;
  if (pcpath == NULL) 
  {
		if (clip_path_id == no_clip_path_id)
	    return 0;
	  if (canWriteStream())
		  s->pputs(" Q\nq\n");
		clip_path_id = no_clip_path_id;
  } 
  else 
  {
		if (clip_path_id == pcpath->id)
	    return 0;
		if (pcpath->includesRectangle(fixed_0, fixed_0,int2fixed(width),int2fixed(height))) 
		{
	    if (clip_path_id == no_clip_path_id)
				return 0;
			if (canWriteStream())
	      s->pputs(" Q\nq\n");
	    clip_path_id = no_clip_path_id;
		} 
		else 
		{
	    XWPSDevVectorDoPathState state;
	    XWPSCPathEnum cenum;
	    XWPSFixedPoint vs[3];
	    int pe_op;
      if (canWriteStream())
	      s->print(" Q\nq\n%s\n", (pcpath->rule <= 0 ? "W" : "W*"));
	    state.init(this, ps_path_type_fill, NULL);
	    
	    cenum.init(pcpath);
	    while ((pe_op = cenum.next(vs)) > 0)
				state.segment(pe_op, vs);
			if (canWriteStream())
	      s->pputs(" n\n");
	    if (pe_op < 0)
				return pe_op;
	    clip_path_id = pcpath->id;
		}
  }
  text.font = 0;
  if (context == PDF_IN_TEXT)
		context = PDF_IN_STREAM;
  resetGraphics();
  return 0;
}

int  XWPSDeviceDVI::putColoredPattern(XWPSDeviceColor *pdc,
													const PSPSDFSetColorCommands *ppscc,
													XWPSDVIResource **ppres)
{
	XWPSColorTile *m_tile = pdc->mask.m_tile;
  XWPSColorTile *p_tile = pdc->colors.pattern.p_tile;
  int w = p_tile->tbits.rep_width, h = p_tile->tbits.rep_height;
  XWPSColorSpace cs_Device;
  cs_Device.i_ctx_p = context_state;
  XWPSDVIImageWriter writer;
  XWPSImage1 image;
  XWDict * pcs_image = 0;
  XWObject pcs_mask;
  XWObject v;
  XWObject cs_value;
  
  int code = csPatternColored(&v);
  if (code < 0)
		return code;
  cs_Device.initDevice(color_info.num_components);
  code = colorSpace(&cs_value, &cs_Device, &pdf_color_space_names, true);
  if (code < 0)
  {
  	v.free();
  	cs_value.free();
		return code;
	}
		
	image.initAdjust(&cs_Device, false);
  image.BitsPerComponent = 8;
  setPatternImage(&image, &(p_tile->tbits));
  if (m_tile) 
  {
		if ((code = putPatternMask(m_tile, &pcs_mask)) < 0)
		{
			v.free();
  		cs_value.free();
  		pcs_mask.free();
	    return code;
	  }
  }
  if ((code = beginWriteImage(&writer, ps_no_id, w, h, NULL, false)) < 0 ||
			(code = writer.binary.setupLosslessFilters(this, &image)) < 0 ||
			(code = beginImageData(&writer, &image, &cs_value)) < 0)
	{
		v.free();
  	cs_value.free();
  	pcs_mask.free();
		return code;
	}
		
	pcs_image = writer.pres->object.streamGetDict();
	if ((code = copyColorBits(writer.binary.strm, p_tile->tbits.data + (h - 1) * p_tile->tbits.raster, 0, -p_tile->tbits.raster, w, h, color_info.depth >> 3)) < 0 ||
			(code = endImageBinary(&writer, h)) < 0)
	{
		v.free();
		pcs_mask.free();
		return code;
	}
	if (pcs_mask.isIndirect())
	{
		pcs_image->add(qstrdup("Mask"),&pcs_mask);
		if ((code = endWriteImage(&writer)) < 0)
			return code;
	}
	else
		pcs_mask.free();
  code = pattern(pdc, p_tile, m_tile, &writer.pres->indirect, ppres);
  if (code < 0)
		return code;
		
	if (canWriteStream())
	{
		writeObj(&v, strm, false);
  	strm->print(" %s ", ppscc->setcolorspace);
  }
  v.free();
  return 0;
}

int  XWPSDeviceDVI::putDrawingColor(XWPSDeviceColor *pdc,
		      						 const PSPSDFSetColorCommands *ppscc)
{
	if (pdc->isPure())
		return setColor(pdc, ppscc);
	else
	{
		int code;
		XWPSDVIResource *pres;		

		if (pdc->type == &ps_dc_type_pattern)
	    code = putColoredPattern(pdc, ppscc, &pres);
		else if (pdc->type == &ps_dc_pure_masked)
	    code = putUncoloredPattern(pdc, ppscc, &pres);
		else if (pdc->type == &ps_dc_pattern2)
	    code = putPattern2(pdc, ppscc, &pres);
		else
	    return (int)(XWPSError::RangeCheck);
		if (code < 0)
	    return code;
	    
	  if (canWriteStream())
	  {
	  	writeObj(&pres->indirect, strm, false);
			strm->print(" %s\n", ppscc->setcolorn);
		}
		return 0;
	}
}

int XWPSDeviceDVI::putFilters(XWDict *pcd, 
	                            XWPSStream *s,
		              const PSPDFFilterNames *pfn)
{
	const char *filter_name = 0;
  bool binary_okA = true;
  XWPSStream *fs = s;
  XWObject decode_parms;
  
  for (; fs != 0; fs = fs->strm)
  {
  	const PSStreamState *st = fs->state;
  	const PSStreamTemplate *templat = st->templat;
#define TEMPLATE_IS(atemp)\
  	(templat->process == (atemp).process)
  	
  	if (TEMPLATE_IS(s_A85E_template))
  		binary_okA = false;
		else if (TEMPLATE_IS(s_CFE_template))
		{
			decode_parms.initDict(xref);
			XWPSObjectParamListWriter writer(&decode_parms, 0);
	    PSStreamCFState cfs;
	    cfs = *(const PSStreamCFState *)st;
	    if (cfs.EndOfBlock)
				cfs.Rows = 0;
				
			writer.getParams(context_state, &cfs, false),
			filter_name = pfn->CCITTFaxDecode;
		}
		else if (TEMPLATE_IS(s_DCTE_template))
	   	filter_name = pfn->DCTDecode;
		else if (TEMPLATE_IS(s_zlibE_template))
	   	filter_name = pfn->FlateDecode;
		else if (TEMPLATE_IS(s_LZWE_template))
	   	filter_name = pfn->LZWDecode;
		else if (TEMPLATE_IS(s_PNGPE_template))
		{
			const PSStreamPNGPState * ss =	(const PSStreamPNGPState *)st;
			
			decode_parms.initDict(xref);
			XWObject obj;
			obj.initInt(ss->Predictor);
			decode_parms.dictAdd(qstrdup("Predictor"), &obj);
			obj.initInt(ss->Columns);
			decode_parms.dictAdd(qstrdup("Columns"), &obj);
			if (ss->Colors != 1)
			{
				obj.initInt(ss->Colors);
				decode_parms.dictAdd(qstrdup("Colors"), &obj);
			}
			
			if (ss->BitsPerComponent != 8)
			{
				obj.initInt(ss->BitsPerComponent);
	    	decode_parms.dictAdd(qstrdup("BitsPerComponent"), &obj);
	    }
		}
		else if (TEMPLATE_IS(s_RLE_template))
			filter_name = pfn->RunLengthDecode;
#undef TEMPLATE_IS
  }
  if (filter_name)
  {
  	if (binary_okA)
  	{
  		XWObject obj;
  		obj.initName(filter_name + 1);
  		pcd->add(qstrdup((pfn->Filter) + 1), &obj);
  		if (!decode_parms.isNone())
  			pcd->add(qstrdup((pfn->DecodeParms) + 1), &decode_parms);
  	}
  	else
  	{
  		XWObject obj;
  		obj.initArray(xref);
  		
  		XWObject v;
  		
  		v.initName((pfn->ASCII85Decode) + 1);  		
  		obj.arrayAdd(&v);
  		
  		v.initName(filter_name + 1);  		
  		obj.arrayAdd(&v);
  		
  		pcd->add(qstrdup((pfn->Filter)+1), &obj);
  		if (!decode_parms.isNone())
  		{
  			XWObject obj1;
  			
  			v.initString(new XWString("null"));  			
  			obj1.arrayAdd(&v); 
  			
  			obj1.initArray(xref);
  			obj1.arrayAdd(&decode_parms);  			
  			 			
  			pcd->add(qstrdup((pfn->DecodeParms) + 1), &obj1);
  		}
  	}
  }
  else if (!binary_okA)
  {
  	XWObject obj;
  	obj.initName(((pfn->ASCII85Decode) + 1));  	
  	pcd->add(qstrdup(pfn->Filter + 1), &obj);
  }
  	
  return 0;
}

int  XWPSDeviceDVI::putImageFilters(XWDict *pcd,
	                         XWPSStream *s,
		      							const PSPDFImageNames *pin)
{
	return putFilters(pcd, s, &pin->filter_names);
}

int  XWPSDeviceDVI::putImageValues(XWDict *pcd, 
		     							XWPSPixelImage *pic,
		     							const PSPDFImageNames *pin,
		     							XWObject *pcsvalue)
{
	XWObject obj;
	XWPSColorSpace *pcs = pic->ColorSpace;

  switch (pic->type.index) 
  {
    case 1: 
    	{
				XWPSImage1 *pim = (XWPSImage1 *)pic;

				if (pim->ImageMask) 
				{
					obj.initBool(true);
	    		pcd->add(qstrdup(pin->ImageMask + 1), &obj);
	    		procsets = (PSPDFProcset)((int)procsets | (int)ImageB);
	    		pcs = NULL;
				}
    	}
			break;
			
    case 3:
			break;
			
    case 4: 
    	{
				XWPSImage4 *pim = (XWPSImage4 *)pic;
				int num_components = pcs->numComponents();
				XWObject pca;
				pca.initArray(xref);
				int i;
    
				for (i = 0; i < num_components; ++i) 
				{
	    		int lo, hi;

	    		if (pim->MaskColor_is_range)
						lo = pim->MaskColor[i * 2], hi = pim->MaskColor[i * 2 + 1];
	    		else
						lo = hi = pim->MaskColor[i];
					
					obj.initInt(lo);
					pca.arrayAdd(&obj);
					
					obj.initInt(hi);
					pca.arrayAdd(&obj);
				}
				pcd->add(qstrdup("Mask"), &pca);
    	}
			break;
			
    default:
			return (int)(XWPSError::RangeCheck);
  }
  return putPixelImageValues(pcd, pic, pcs, pin, pcsvalue);
}

int  XWPSDeviceDVI::putLinearShading(XWDict *pscd, 
	                      const float *Coords,
		                    int num_coords, 
		                    const float *Domain,
		                    XWPSFunction *Function,
		       							const bool *Extend)
{
	XWObject a,b, c, obj, fn_value;
	a.initArray(xref);
	for (int i = 0; i < num_coords; i++)
	{
		obj.initReal(Coords[i]);
		a.arrayAdd(&obj);
	}
	
	pscd->add(qstrdup("Coords"), &a);
	b.initArray(xref);
	for (int i = 0; i < 2; i++)
	{
		obj.initReal(Domain[i]);
		b.arrayAdd(&obj);
	}
	
	if (Domain[0] != 0 || Domain[1] != 1)
		pscd->add(qstrdup("Domain"), &b);
	else
		b.free();
		
	if (Function) 
  {
		function(Function, &fn_value);
	  pscd->add(qstrdup("Function"), &fn_value);
  }
  
  if (Extend[0] | Extend[1])
  {
  	c.initArray(xref);
  	if (Extend[0])
  		obj.initBool(true);
  	else
  		obj.initBool(false);
  		
  	c.arrayAdd(&obj);
  	if (Extend[1])
  		obj.initBool(true);
  	else
  		obj.initBool(false);
  		
  	c.arrayAdd(&obj);
  	pscd->add(qstrdup("Extend"), &c);
  }
  
  return 0;
}

void XWPSDeviceDVI::putMatrix(XWDict * dict, const char *key, XWPSMatrix *pmat)
{
	XWObject a, v;
	a.initArray(xref);
	
	v.initReal(pmat->xx);
	a.arrayAdd(&v);	
	v.initReal(pmat->xy);
	a.arrayAdd(&v);
	v.initReal(pmat->yx);
	a.arrayAdd(&v);
	v.initReal(pmat->yy);
	a.arrayAdd(&v);
	v.initReal(pmat->tx);
	a.arrayAdd(&v);
	v.initReal(pmat->ty);
	a.arrayAdd(&v);	
	dict->add(qstrdup(key), &a);
}

void XWPSDeviceDVI::putMatrix(const char *before,
	       				              XWPSMatrix * pmat, 
	       				              const char *after)
{
	XWPSStream *s = strm;

  if (before)
		s->pputs(before);
  s->print("%g %g %g %g %g %g ", pmat->xx, pmat->xy, pmat->yx, pmat->yy, pmat->tx, pmat->ty);
  if (after)
		s->pputs(after);
}

int  XWPSDeviceDVI::putMeshShading(XWObject *pscs, XWPSShading *psh)
{
	XWDict * pscd = pscs->streamGetDict();
  XWPSColorSpace *pcs = psh->params->ColorSpace;
  XWPSShadingMeshParams * pmp = (XWPSShadingMeshParams *)psh->params;
  int code = putShadingCommon(pscd, psh);
  int bits_per_coordinate, bits_per_component, bits_per_flag;
  int num_comp = (pmp->Function ? 1 : pcs->numComponents());
  bool from_array = pmp->DataSource.isArray();
  bool is_indexed;
  XWPSShadeCoordStream cs;
  XWPSMatrixFixed ctm_ident;
  int flag;
		
	cs.nextInit(pmp, NULL);
	XWObject obj;
	if (from_array)
  {
  	XWObject pca;
  	pca.initArray(xref);
		int i;

		for (i = 0; i < 2; ++i)
		{
			obj.initInt(-32768);
			pca.arrayAdd(&obj);
			
			obj.initInt(32767);
			pca.arrayAdd(&obj);
		}
		
		if (pcs->getIndex() == ps_color_space_index_Indexed) 
		{
	    is_indexed = true;
	    
			obj.initInt(-32768);
			pca.arrayAdd(&obj); 
			
			obj.initInt(32767);
			pca.arrayAdd(&obj);	   
		} 
		else 
		{
	    is_indexed = false;
	    for (i = 0; i < num_comp; ++i)
	    {				
	    	obj.initInt(-256);
				pca.arrayAdd(&obj);
				
	    	obj.initInt(256);
				pca.arrayAdd(&obj);
	    }
		}
		
		pscd->add(qstrdup("Decode"), &pca);
		bits_per_coordinate = 24;
		bits_per_component = 16;
		bits_per_flag = 8;
		ctm_ident.tx_fixed = ctm_ident.ty_fixed = 0;
		cs.pctm = &ctm_ident;
  }
  else
  {
  	uchar buf[100];
		uint num_read;
		
		int c = 4 + pcs->numComponents() * 2;
		XWObject pca;
  	pca.initArray(xref);
		for (int i = 0; i < c; i++)
		{
			obj.initReal(pmp->Decode[i]);
			pca.arrayAdd(&obj);
		}

		pscd->add(qstrdup("Decode"), &pca);
		while (cs.s->gets(buf, sizeof(buf), &num_read), num_read > 0)
	    pscs->streamAdd((const char*)buf, num_read);
	    
		bits_per_coordinate = pmp->BitsPerCoordinate;
		bits_per_component = pmp->BitsPerComponent;
		bits_per_flag = -1;
  }
  
  obj.initInt(bits_per_coordinate);
  pscd->add(qstrdup("BitsPerCoordinate"), &obj);
  obj.initInt(bits_per_component);
  pscd->add(qstrdup("BitsPerComponent"), &obj);
  
  switch (psh->getType())
  {
  	case shading_type_Free_form_Gouraud_triangle: 
  		{
				XWPSShadingFfGtParams * params =   (XWPSShadingFfGtParams *)pmp;

				if (from_array)
	    		while ((flag = cs.nextFlag(0)) >= 0)
						if ((code = put_float_mesh_data(pscs, &cs, flag, 1, num_comp,	is_indexed)) < 0)
		    			return code;
				if (bits_per_flag < 0)
	    		bits_per_flag = params->BitsPerFlag;
				break;
    	}
    	
    case shading_type_Lattice_form_Gouraud_triangle: 
    	{
				XWPSShadingLfGtParams * params = (XWPSShadingLfGtParams *)pmp;

				if (from_array)
	    		while (!cs.s->eofp())
						if ((code = put_float_mesh_data(pscs, &cs, -1, 1, num_comp,is_indexed)) < 0)
		    			return code;
		    obj.initInt(params->VerticesPerRow);
				pscd->add(qstrdup("VerticesPerRow"), &obj);
				return 0;
    	}
    	
    case shading_type_Coons_patch: 
    	{
				XWPSShadingCpParams *params = (XWPSShadingCpParams *)pmp;

				if (from_array)
	    		while ((flag = cs.nextFlag(0)) >= 0) 
	    		{
						int num_c = (flag == 0 ? 4 : 2);

						if ((code = put_float_mesh_data(pscs, &cs, flag, 4 + num_c * 2,	num_comp * num_c,	is_indexed)) < 0)
		    			return code;
	    		}
				if (bits_per_flag < 0)
	    		bits_per_flag = params->BitsPerFlag;
				break;
    	}
    	
    case shading_type_Tensor_product_patch: 
    	{
				XWPSShadingTppParams * params = (XWPSShadingTppParams *)pmp;

				if (from_array)
	    		while ((flag = cs.nextFlag(0)) >= 0) 
	    		{
						int num_c = (flag == 0 ? 4 : 2);

						if ((code = put_float_mesh_data(pscs, &cs, flag, 8 + num_c * 2,	num_comp * num_c,	is_indexed)) < 0)
		    			return code;
	    		}
				if (bits_per_flag < 0)
	    		bits_per_flag = params->BitsPerFlag;
				break;
    	}
    	
    default:
			return (int)(XWPSError::RangeCheck);
  }
  
  obj.initInt(bits_per_flag);
  pscd->add(qstrdup("BitsPerFlag"), &obj);
  return 0;
}

int  XWPSDeviceDVI::putPatternMask(XWPSColorTile *m_tile,
		     							XWObject *ppcs_mask)
{
	int w = m_tile->tmask.rep_width, h = m_tile->tmask.rep_height;
  XWPSImage1 image;
  XWPSDVIImageWriter writer;
  int code;
  if ((code = beginWriteImage(&writer, ps_no_id, w, h, NULL, false)) < 0 ||
			(params.MonoImage.Encode &&
	 		(code = writer.binary.CFEBinary(w, h, true)) < 0) ||
			(code = beginImageData(&writer, &image, NULL)) < 0)
	{
		return code;
	}
  *ppcs_mask = writer.pres->indirect;
  if ((code = copyMaskBits(writer.binary.strm, m_tile->tmask.data + (h - 1) * m_tile->tmask.raster, 0, -m_tile->tmask.raster, w, h, 0)) < 0 ||
			(code = endImageBinary(&writer, h)) < 0 ||
			(code = endWriteImage(&writer)) < 0)
	{
		return code;
	}
  return 0;
}

int  XWPSDeviceDVI::putPattern2(XWPSDeviceColor *pdc,
		 							 const PSPSDFSetColorCommands *ppscc,
		 							 XWPSDVIResource **ppres)
{
	XWPSPattern2Instance *pinst = (XWPSPattern2Instance *)pdc->ccolor.pattern;
  XWPSShading *psh = (XWPSShading*)(pinst->templat.Shading);
  XWObject v;
  XWPSDVIResource *pres;
  XWPSDVIResource *psres;
  XWDict * pcd;
  int code = csPatternColored(&v);
  XWPSMatrix smat;
  
  if (code < 0)
		return code;
  code = allocResource(resourcePattern, ps_no_id, ppres);
  if (code < 0)
		return code;
    
  pres = *ppres;
  pres->object.free();
  pres->object.initDict(xref);
  pres->object.label = pres->indirect.label;
  pcd = pres->object.getDict();
  code = allocResource(resourceShading, ps_no_id, &psres);
  if (code < 0)
		return code;
  if (psh->getType() >= 4) 
		putMeshShading(&psres->object, psh);
  else 
  {
  	psres->object.free();
  	psres->object.initDict(xref);
  	psres->object.label = psres->indirect.label;
		code = putScalarShading(psres->object.getDict(), psh);
  }
  
  pinst->saved->currentMatrix(&smat);
  {
		double xscale = 72.0 / HWResolution[0], yscale = 72.0 / HWResolution[1];

		smat.xx *= xscale, smat.yx *= xscale, smat.tx *= xscale;
		smat.xy *= yscale, smat.yy *= yscale, smat.ty *= yscale;
  }
  
  XWObject a, obj;
  obj.initInt(2);
  pcd->add(qstrdup("PatternType"), &obj);
  pcd->add(qstrdup("Shading"), &(psres->indirect));
  
  if (canWriteStream())
  {
    putMatrix(pcd, "Matrix", &smat);
  	writeObj(&v, strm, false);
  	strm->print(" %s ", ppscc->setcolorspace);
  }
  v.free();
  return 0;
}

int  XWPSDeviceDVI::putPixelImageValues(XWDict *pcd, 
			   									 XWPSPixelImage *pim,
			   										XWPSColorSpace *pcs,
			   										const PSPDFImageNames *pin,
			   										XWObject *pcsvalue)
{
	int num_components;
  float indexed_decode[2];
  const float *default_decode = NULL;
  XWObject obj;

  if (pcs) 
  {
		pcd->add(qstrdup((pin->ColorSpace) + 1), pcsvalue);			
		colorSpaceProcsets(pcs);
		num_components = pcs->numComponents();
		if (pcs->getIndex() == ps_color_space_index_Indexed) 
		{
	    indexed_decode[0] = 0;
	    indexed_decode[1] = (1 << pim->BitsPerComponent) - 1;
	    default_decode = indexed_decode;
		}
  } 
  else
		num_components = 1;
    
  obj.initInt(pim->Width);
  pcd->add(qstrdup((pin->Width) + 1), &obj);  
	obj.initInt(pim->Height);
  pcd->add(qstrdup((pin->Height) + 1), &obj); 
  obj.initInt(pim->BitsPerComponent);
  pcd->add(qstrdup((pin->BitsPerComponent) + 1), &obj);
	
  {
		int i;

		for (i = 0; i < num_components * 2; ++i)
	    if (pim->Decode[i] !=	(default_decode ? default_decode[i] : i & 1))
				break;
		if (i < num_components * 2) 
		{
	    XWObject pca;
	    pca.initArray(xref);
	    for (i = 0; i < num_components * 2; ++i)
	    {
	    	obj.initReal(pim->Decode[i]);
				pca.arrayAdd(&obj);
			}
	    pcd->add(qstrdup(pin->Decode + 1), &pca);
		}
  }
  if (pim->Interpolate)
  {
  	obj.initBool(true);
		pcd->add(qstrdup(pin->Interpolate + 1), &obj);
	}
  return 0;
}

int XWPSDeviceDVI::putScalarShading(XWDict *pscd, XWPSShading *psh)
{
	int code = putShadingCommon(pscd, psh);

  if (code < 0)
		return code;
  switch (psh->getType()) 
  {
    case shading_type_Function_based: 
    	{
				XWPSShadingFbParams * params = (XWPSShadingFbParams *)psh->params;
				XWObject a,obj, fn_value;
				a.initArray(xref);
				for (int i = 0; i < 4; i++)
				{
					obj.initReal(params->Domain[i]);
					a.arrayAdd(&obj);
				}
				pscd->add(qstrdup("Domain"), &a);

				function(params->Function, &fn_value);
	    	pscd->add(qstrdup("Function"), &fn_value);
	    	
	    	putMatrix(pscd, "Matrix", &params->Matrix);
				return 0;
    	}
    	
    case shading_type_Axial: 
    	{
				XWPSShadingAParams * params = (XWPSShadingAParams *)&psh->params;

				return putLinearShading(pscd, params->Coords, 4, params->Domain, params->Function, params->Extend);
    	}
    	
    case shading_type_Radial: 
    	{
				XWPSShadingRParams * params = (XWPSShadingRParams *)&psh->params;

				return putLinearShading(pscd, params->Coords, 6, params->Domain, params->Function, params->Extend);
    	}
    	
    default:
		return (int)(XWPSError::RangeCheck);
  }
}

int  XWPSDeviceDVI::putShadingCommon(XWDict *pscd, XWPSShading *psh)
{
	PSShadingType type = psh->getType();
  XWPSColorSpace *pcs = psh->params->ColorSpace;
  
  XWObject cs_value, obj;
  obj.initInt(type);
  pscd->add(qstrdup("ShadingType"), &obj);
  if (psh->params->AntiAlias)
  {
  	obj.initBool(true);
  	pscd->add(qstrdup("AntiAlias"), &obj);
  }
  
  int code = colorSpace(&cs_value, pcs,	&pdf_color_space_names, false);
  if (code < 0)
  {
  	cs_value.free();
  	return code;
  }
  pscd->add(qstrdup("AntiAlias"), &cs_value);
  if (psh->params->Background) 
  {
  	XWObject a;
  	a.initArray(xref);
  	for (int i = 0; i < pcs->numComponents(); i++)
  	{
  		obj.initReal(psh->params->Background->paint.values[i]);
  		a.arrayAdd(&obj);
  	}
  	pscd->add(qstrdup("Background"), &a);
  }
  if (psh->params->have_BBox) 
  {		
		XWObject a;
  	a.initArray(xref);
  	
  	obj.initReal(psh->params->BBox.p.x);
  	a.arrayAdd(&obj);
  	obj.initReal(psh->params->BBox.p.y);
  	a.arrayAdd(&obj);
  	obj.initReal(psh->params->BBox.q.x);
  	a.arrayAdd(&obj);
  	obj.initReal(psh->params->BBox.q.y);
  	a.arrayAdd(&obj);
		pscd->add(qstrdup("BBox"), &a);
  }
  return 0;
}

int XWPSDeviceDVI::putString(const uchar * str, uint size)
{
	if (canWriteStream())
	  strm->writePSString(str, size, (binary_ok ? PRINT_BINARY_OK : 0));
	return 0;
}

int  XWPSDeviceDVI::putUncoloredPattern(XWPSDeviceColor *pdc,
			  									 const PSPSDFSetColorCommands *ppscc,
			                     XWPSDVIResource **ppres)
{
	XWPSColorTile *m_tile = pdc->mask.m_tile;
  XWObject v;
  int code = csPatternUnColored(&v);
  XWObject pcs_image;
  XWPSDeviceColor dc_pure;
  static const PSPSDFSetColorCommands no_scc = {0, 0, 0};

  if (code < 0 ||	(code = putPatternMask(m_tile, &pcs_image)) < 0 ||
			(code = pattern(pdc, NULL, m_tile, &pcs_image, ppres)) < 0)
	{
		v.free();
		return code;
	}
    
  writeObj(&v, strm, false);
  if (canWriteStream())
    strm->print(" %s ", ppscc->setcolorspace);
  dc_pure.setPure(pdc->pureColor());
  setColor(&dc_pure, &no_scc);
  v.free();
  return 0;
}

void XWPSDeviceDVI::putVector3(XWDict *pcd, 
                  const char *key,
			            const PSVector3 *pvec)
{
	XWObject a;
	a.initArray(xref);
	
	addVector3(&a, pvec);
	pcd->add(qstrdup(key), &a);
}

int  XWPSDeviceDVI::referNamed(PSParamString * pname_orig,	XWObject *pco)
{
	PSParamString *pname = pname_orig;  
  int page_number = 1;		
  int code = findNamed(pname, pco);
  if (code != XWPSError::Undefined)
		return code;
		
	if (pname->size >= 7 &&	sscanf((const char *)pname->data, "{Page%d}", &page_number) == 1)
		goto cpage;
	else if (pname->pdfKeyEq("{ThisPage}"))
		page_number = xref->currentPageNumber();
	else if (pname->pdfKeyEq("{NextPage}"))
		page_number = xref->currentPageNumber() + 1;
	else if (pname->pdfKeyEq("{PrevPage}"))
		page_number = xref->currentPageNumber() - 1;
	else
		return code;
		
cpage:
	if (page_number <= 0)
		return code;
		
	xref->getPageDict(page_number)->copy(pco);
		
	return 0;
}

int  XWPSDeviceDVI::replaceNames(PSParamString * from, PSParamString * to)
{
	uchar *start = from->data;
  uchar *end = start + from->size;
  uchar *scan;
  uint size = 0;
  XWObject pco;
  bool any = false;
  uchar *sto;
  char ref[1 + 10 + 5 + 1];
  for (scan = start; scan < end;)
  {
  	uchar *sname;
		uchar *next =  (uchar*)pdfmarkNextObject(scan, end, (const uchar**)&sname, &pco);

		size += sname - scan;
		if (pco.isIndirect())
		{
	  	sprintf(ref, " %ld 0 R ", pco.label);
	  	size += strlen(ref);
	  }
		scan = next;
		any |= next != sname;
  }
  
  if (!any) 
  {
		to->size = size;
		return 0;
  }
  
  uchar * oldsto = to->data;
  bool oldpersistent = to->persistent;
  sto = new uchar[size];
  to->data = sto;
  to->size = size;
  to->persistent = false;
  for (scan = start; scan < end;)
  {
  	uchar *sname;
		uchar *next = (uchar*)pdfmarkNextObject(scan, end, (const uchar**)&sname, &pco);
		uint copy = sname - scan;
		int rlen;

		memcpy(sto, scan, copy);
		sto += copy;
		if (pco.isIndirect())
		{
	  	sprintf(ref, " %ld 0 R ", pco.label);
	  	rlen = strlen(ref);
	  	memcpy(sto, ref, rlen);
	  	sto += rlen;
	  }
		scan = next;
  }
  if (!oldpersistent && oldsto)
  	delete [] oldsto;
  return 0;
}

int  XWPSDeviceDVI::resetColor(XWPSDeviceColor *pdc,
		              XWPSDeviceColor * pdcolor,
		              const PSPSDFSetColorCommands *ppscc)
{
	int code = openPage(PDF_IN_STREAM);
  if (code < 0)
		return code;
		
	code = putDrawingColor(pdc, ppscc);
  if (code >= 0)
		*pdcolor = *pdc;
  return code;
}

void XWPSDeviceDVI::resetGraphics()
{
	if (!fill_color)
		fill_color = new XWPSDeviceColor;		
	
	if (!stroke_color)
		stroke_color = new XWPSDeviceColor;		
		
	fill_color->setPure(0);
	stroke_color->setPure(0);
	
	if (state)
		delete state;
	state = new XWPSImagerState;
	state->i_ctx_p = context_state;
	fill_overprint = false;
  stroke_overprint = false;
  resetText();
}

void XWPSDeviceDVI::resetText()
{
	text.reset();
}

bool XWPSDeviceDVI::scanForStandardFonts(XWPSFontDir *dir)
{
	bool found = false;
  XWPSFont *orig = dir->orig_fonts;
  for (; orig; orig = orig->next)
  {
  	XWPSFontBase *obfont;

		if (orig->FontType == ft_composite || !orig->is_resource)
	    continue;
		obfont = (XWPSFontBase *)orig;
		if (obfont->UID.isUniqueID())
		{
			int i = pdf_find_standard_font(orig->key_name.chars, orig->key_name.size);
			if (i >= 0 && std_fonts[i].font == 0)
			{
				std_fonts[i].uid = obfont->UID;
				std_fonts[i].orig_matrix = obfont->FontMatrix;
				std_fonts[i].font = orig;
				found = true;
				registeredStdFonts << i;
			}
		}
  }
  
  return found;
}

int  XWPSDeviceDVI::separationColorSpace(XWObject *pca, 
	                          const char *csname,
			                      XWObject *snames,
			                      XWPSColorSpace *alt_space,
			                      XWPSFunction *pfn,
			                      const PSPDFColorSpaceNames *pcsn)
{
	XWObject obj;
	obj.initName(csname);
	pca->arrayAdd(&obj);
	pca->arrayAdd(snames);
	colorSpace(&obj, alt_space, pcsn, false);
	pca->arrayAdd(&obj);
	function(pfn, &obj);
	pca->arrayAdd(&obj);	
	return 0;
}

int XWPSDeviceDVI::separationName(XWObject *pvalue, ulong sname)
{
	static const char *const snames[] = {
		"Default", "Gray", "Red", "Green", "Blue", 
    "Cyan", "Magenta", "Yellow", "Black" };
  if ((ulong)sname < 9) 
  	pvalue->initString(new XWString(snames[(int)sname]));
  else 
  {
  	char buf[sizeof(ulong) * 8 / 3 + 2];
		sprintf(buf, "S%ld", (ulong)sname);
		pvalue->initString(new XWString(buf));
  }
  return 0;
}

int  XWPSDeviceDVI::setDrawingColor(XWPSDeviceColor *pdc,
		      						 XWPSDeviceColor * pdcolor,
		      						 const PSPSDFSetColorCommands *ppscc)
{
	if (pdcolor->equal(pdc))
		return 0;
  return resetColor(pdc, pdcolor, ppscc);
}

int  XWPSDeviceDVI::setFontAndSize(XWPSDVIFont * font, float size)
{
	if (font != text.font || size != text.size) 
	{
		int code = openPage(PDF_IN_TEXT);
		XWPSStream *s = strm;

		if (code < 0)
	    return code;
	  if (canWriteStream())
	  {
			s->print(" /%s ", font->frname);
			s->print("%g Tf\n", size);
		}
		text.font = font;
		text.size = size;
  }

  return 0;
}

int  XWPSDeviceDVI::setPureColor(ulong color,  
	                  XWPSDeviceColor * pdcolor,
		                const PSPSDFSetColorCommands *ppscc)
{
	XWPSDeviceColor dcolor;

  if (pdcolor->isPure() && pdcolor->pureColor() == color)
		return 0;
  dcolor.setPure(color);
  return resetColor(&dcolor, pdcolor, ppscc);
}

int XWPSDeviceDVI::streamToNone()
{
	return PDF_IN_NONE;
}

int XWPSDeviceDVI::streamToText()
{
	if (canWriteStream())
	  strm->print(" q %g 0 0 %g 0 0 cm BT\n", HWResolution[0] / 72.0, HWResolution[1] / 72.0);
  procsets = (PSPDFProcset)((int)procsets | (int)Text);
  text.matrix.reset();
  text.line_start.x = text.line_start.y = 0;
  text.buffer_count = 0;
  return PDF_IN_TEXT;
}

int XWPSDeviceDVI::stringToText()
{  
  putString(text.buffer, text.buffer_count);
  if (canWriteStream())
    strm->pputs((text.use_leading ? "'\n" : "Tj\n"));
  text.use_leading = false;
  text.buffer_count = 0;
  return PDF_IN_TEXT;
}

int XWPSDeviceDVI::textToStream()
{
	if (canWriteStream())
	  strm->pputs(" ET Q\n");
  resetText();
  return PDF_IN_STREAM;
}

int XWPSDeviceDVI::updateAlpha(XWPSImagerState *pis,
		 							const char *ca_format, 
		 							XWPSDVIResource **ppres)
{
	bool ais;
  float alpha;
  int code;

  if (state->opacity.alpha != pis->opacity.alpha) 
  {
		if (state->shape.alpha != pis->shape.alpha)
	    return (int)(XWPSError::RangeCheck);
		ais = false;
		alpha = state->opacity.alpha = pis->opacity.alpha;
  } 
  else if (state->shape.alpha != pis->shape.alpha) 
  {
		ais = true;
		alpha = state->shape.alpha = pis->shape.alpha;
  } 
  else
		return 0;
  code = openGState(ppres);
  if (code < 0)
		return code;
		
	XWObject obj;
	obj.initBool(ais);
	(*ppres)->object.dictAdd(qstrdup("AIS"), &obj);
	obj.initReal(alpha);
	(*ppres)->object.dictAdd(qstrdup(ca_format), &obj);
	return 0;
}

int XWPSDeviceDVI::updateHalftone(XWPSImagerState *pis, XWDict *hts)
{
	XWPSHalftone *pht = pis->halftone;
  XWPSDeviceHalftone *pdht = pis->dev_ht;
  int code;
  XWObject id; 
  switch (pht->type) 
  {
    case ht_type_screen:
			code = writeScreenHalftone(pht->params.screen, &pdht->order, &id);
			break;
			
    case ht_type_colorscreen:
			code = writeColorScreenHalftone(pht->params.colorscreen, pdht, &id);
			break;
			
    case ht_type_spot:  
			code = writeSpotHalftone(pht->params.spot, &pdht->order, &id);
			break;
			
    case ht_type_threshold:
			code = writeThresholdHalftone(pht->params.threshold, &pdht->order, &id);
			break;
			
    case ht_type_threshold2:
			code = writeThreshold2Halftone(pht->params.threshold2, &pdht->order, &id);
			break;
			
    case ht_type_multiple:
    case ht_type_multiple_colorscreen:
			code = writeMultipleHalftone(pht->params.multiple, pdht, &id);
			break;
			
    default:
			return (int)(XWPSError::RangeCheck);
  }
  if (code < 0)
		return code;
	
	hts->add(qstrdup("HT"), &id);
  halftone_id = pis->dev_ht->id;
  return code;
}

int XWPSDeviceDVI::updateTransfer(XWPSImagerState *pis, XWDict *trs)
{
	int i;
  bool multiple = false, update = false;
  ulong transfer_idsA[4];
  int code = 0;

  for (i = 0; i < 4; ++i) 
  {
		transfer_idsA[i] = pis->set_transfer.u.indexed[i]->id;
		if (transfer_ids[i] != transfer_idsA[i])
	    update = true;
		if (transfer_idsA[i] != transfer_idsA[0])
	    multiple = true;
  }
  
  if (update) 
  {
  	if (!multiple) 
		{
	    code = writeTransfer(pis->set_transfer.u.indexed[0], "TR", trs);
	    if (code < 0)
				return code;
		} 
		else 
		{
			XWObject a, obj;
			a.initArray(xref);
  
			for (i = 0; i < 4; ++i) 
	    {
	    	{
	    		XWPSFunctionSdParams params;
	    	
					params.m = 1;
  				params.Domain = new float[2];
  				params.Domain[0] = 0;
  				params.Domain[1] = 1;
  				params.n = 1;
  				params.Range = new float[2];
  				params.Range[0] = 0;
  				params.Range[1] = 1;
  				params.Order = 1;
  				params.DataSource.access =	&XWPSDataSource::transferMapAccess;
  				params.DataSource.data.str.data = (const uchar *)(pis->set_transfer.u.indexed[i]);
  
  				params.BitsPerSample = 8;
  				params.Size = new int[1];
  				params.Size[0] = transfer_map_size;
  				
  				code = params.check();
  				if (code < 0)
  				{
  					a.free();
  					return code;
  				}
  			
  				XWPSFunctionSd * pfn = new XWPSFunctionSd(&params);
  				if (!pfn || !pfn->params)
  				{
  					a.free();
						return -1;
					}
  				code = writeFunction(pfn, &obj);
  				delete pfn;
  			}
  			a.arrayAdd(&obj);
	    }
	    
	    trs->add(qstrdup("TR"), &a);
		}
		
		memcpy(transfer_ids, transfer_idsA, sizeof(transfer_ids));
  }
  
  return code;
}

void XWPSDeviceDVI::writeArray(XWObject * obj, XWPSStream * s, bool in_line)
{
	XWArray * array = obj->getArray();
	int length = array->getLength();
	if (array && length > 0)
	{
		s->pputs(" [");
		XWObject obj1;
		for (int i = 0; i < length; i++)
		{
			array->getNF(i, &obj1);
			writeObj(&obj1, s, in_line);
			obj1.free();
			obj1.initNull();
		}
		
		s->pputs("]");
	}
}

void XWPSDeviceDVI::writeBoolean(XWObject * obj, XWPSStream * s)
{
	if (obj->getBool())
		s->pputs(" true");
	else
		s->pputs(" false");
}

void XWPSDeviceDVI::writeCIDSystemInfo(XWDict * dict, XWPSCidSystemInfo *pcidsi)
{
	XWObject d, obj;
	d.initDict(xref);
	obj.initString(new XWString((char*)(pcidsi->Registry.data), pcidsi->Registry.size));
	d.dictAdd(qstrdup("Registry"), &obj);
	obj.initString(new XWString((char*)(pcidsi->Ordering.data), pcidsi->Ordering.size));
	d.dictAdd(qstrdup("Ordering"), &obj);
	obj.initInt(pcidsi->Supplement);
	d.dictAdd(qstrdup("Supplement"), &obj);
	dict->add(qstrdup("CISystemInfo"), &d);
}

int XWPSDeviceDVI::writeColorScreenHalftone(XWPSColorScreenHalftone *pcsht,
			                         XWPSDeviceHalftone *pdht, 
			                         XWObject *r)
{
	int i;
	XWObject ht_ids[4];
	for (i = 0; i < 4; ++i) 
  {
		int code = writeScreenHalftone(pcsht->screens.indexed[i],  &pdht->components[i].corder, &ht_ids[i]);
		if (code < 0)
	    return code;
  }
  
  XWObject dict, obj;
  dict.initDict(xref);
  xref->refObj(&dict, r);
  
  obj.initName("Halftone");
  dict.dictAdd(qstrdup("Type"), &obj);
  obj.initInt(5);
  dict.dictAdd(qstrdup("HalftoneType"), &obj);
  dict.dictAdd(qstrdup("Default"), &ht_ids[3]);
  
  dict.dictAdd(qstrdup("Red"), &ht_ids[0]);
  dict.dictAdd(qstrdup("Cyan"), &ht_ids[0]);
  
  dict.dictAdd(qstrdup("Green"), &ht_ids[1]);
  dict.dictAdd(qstrdup("Magenta"), &ht_ids[1]);
  
  dict.dictAdd(qstrdup("Blue"), &ht_ids[2]);
  dict.dictAdd(qstrdup("Yellow"), &ht_ids[2]);
  
  dict.dictAdd(qstrdup("Gray"), &ht_ids[3]);
  dict.dictAdd(qstrdup("Black"), &ht_ids[3]);
  
  if (!xref->isScanning())
	  xref->releaseObj(&dict);
	else
	 	dict.free();
  return 0;
}

void XWPSDeviceDVI::writeDict(XWObject * obj, XWPSStream * s, bool in_line)
{
	XWDict * dict = obj->getDict();
	int length = dict->getLength();
	if (dict && length > 0)
	{
		if (!in_line)
			s->pputs(" <<");
		XWObject key, value;
		for (int i = 0; i < length; i++)
		{
			char * sn = dict->getKey(i);
			key.initName(sn);
			writeName(&key, s);			
			key.free();
			dict->getValNF(i, &value);
			writeObj(&value, s, in_line);
			value.free();
			s->pputs("\n");
		}
		
		if (!in_line)
			s->pputs(">>");
	}
}

int XWPSDeviceDVI::writeEmbeddedFont(XWPSDVIFontDescriptor *pfd)
{
	XWPSFont *font = pfd->base_font;
  XWPSString font_name;
  uchar *fnchars = pfd->FontName.chars;
  uint fnsize = pfd->FontName.size;
  bool do_subset = pfd->subset_ok && params.SubsetFonts &&	params.MaxSubsetPct > 0;
  ulong subset_glyphs[256];
  ulong *subset_list = 0;
  uchar *subset_bits = 0;
  uint subset_size = 0;
  XWPSMatrix save_mat;
  int code;
  
  if (do_subset) 
  {
		int used, i, total, index;
		ulong ignore_glyph;

		for (i = 0, used = 0; i < pfd->chars_used.size; ++i)
	    used += byte_count_bits[pfd->chars_used.data[i]];
		for (index = 0, total = 0; (font->enumerateGlyph(&index, GLYPH_SPACE_INDEX, &ignore_glyph), index != 0);)
	    ++total;
		if ((double)used / total > params.MaxSubsetPct / 100.0)
	    do_subset = false;
  }
  
  if (hasSubsetPrefix(fnchars, fnsize)) 
  {
		fnsize -= SUBSET_PREFIX_SIZE;
		memmove(fnchars, fnchars + SUBSET_PREFIX_SIZE, fnsize);
  }
  if (do_subset) 
  {
		memmove(fnchars + SUBSET_PREFIX_SIZE, fnchars, fnsize);
		makeSubsetPrefix(fnchars, pfd->indirect.label);
		fnsize += SUBSET_PREFIX_SIZE;
  }
  font_name.data = fnchars;
  font_name.size = pfd->FontName.size = fnsize;
  code = writeFontDescriptor(pfd);
  if (code >= 0) 
  {
		pfd->written = true;
		save_mat = font->FontMatrix;
		font->FontMatrix = pfd->orig_matrix;
		switch (font->FontType) 
		{
			case ft_composite:
	    	break;
	    	
			case ft_encrypted:
			case ft_encrypted2:
	    	if (do_subset) 
	    	{
					subset_size = font->subsetGlyphs(subset_glyphs, pfd->chars_used.data);
					subset_list = subset_glyphs;
	    	}
	    	code = embedFontType1((XWPSFontType1 *)font,pfd->FontFile, subset_list,subset_size, &font_name);
	    	break;
	    
			case ft_TrueType:
	    	if (do_subset) 
	    	{
					subset_size = font->subsetGlyphs(subset_glyphs,pfd->chars_used.data);
					subset_list = subset_glyphs;
	    	}
	    	code = embedFontType42((XWPSFontType42 *)font, pfd->FontFile, subset_list, subset_size, &font_name);
	    	break;
	    	
			case ft_CID_encrypted:
	    	if (do_subset) 
	    	{
					subset_size = pfd->chars_used.size << 3;
					subset_bits = pfd->chars_used.data;
	    	}
	    	code = embedFontCid0((XWPSFontCid0 *)font, pfd->FontFile, subset_bits, subset_size, &font_name);
	    	break;
	    	
			case ft_CID_TrueType:
		    if (do_subset) 
		    {
					subset_size = pfd->chars_used.size << 3;
					subset_bits = pfd->chars_used.data;
	    	}
	    	code = embedFontCid2((XWPSFontCid2 *)font, pfd->FontFile, subset_bits, subset_size, &font_name);
	    	break;
			default:
	    	code = (int)(XWPSError::RangeCheck);
		}
		font->FontMatrix = save_mat;
  }
  return code;
}

int XWPSDeviceDVI::writeFontDescriptor(XWPSDVIFontDescriptor *pfd)
{
	XWPSFont *font = pfd->getBaseFont();
  bool is_subset =	hasSubsetPrefix(pfd->FontName.chars, pfd->FontName.size);
  XWObject cidset, cidsetref, obj;  
 
  int code = 0;
  if (font && is_subset)
  {
  	switch (pfd->values.FontType)
  	{
  		case ft_CID_encrypted:
			case ft_CID_TrueType:
				{
					cidset.initStream(0, xref);
					xref->refObj(&cidset, &cidsetref);	
					for (int i = 0; i < pfd->chars_used.size; ++i)				
						cidset.streamAdd((char*)&(byte_reverse_bits[pfd->chars_used.data[i]]), 1);
						
					if (!xref->isScanning())
						xref->releaseObj(&cidset);
					else
						cidset.free();
				}
				
			default:
				break;
  	}
  }
  
  XWDict * dict = pfd->object.getDict();
  char buf[100];
  xref->escapeStr(buf, 100, (const uchar*)(pfd->FontName.chars), pfd->FontName.size);
  obj.initName(buf);
  dict->add(qstrdup("FontName"), &obj);
  if (font)
  {
  	XWObject a;
  	a.initArray(xref);
  	obj.initInt(pfd->values.FontBBox.p.x);
  	a.arrayAdd(&obj);
  	obj.initInt(pfd->values.FontBBox.p.y);
  	a.arrayAdd(&obj);
  	obj.initInt(pfd->values.FontBBox.q.x);
  	a.arrayAdd(&obj);
  	obj.initInt(pfd->values.FontBBox.q.x);
  	a.arrayAdd(&obj);
  	dict->add(qstrdup("FontBBox"), &a);
  	
  	{
  		obj.initInt(pfd->values.Ascent);
  		dict->add(qstrdup("Ascent"), &obj);
  		obj.initInt(pfd->values.CapHeight);
  		dict->add(qstrdup("CapHeight"), &obj);
  		obj.initInt(pfd->values.Descent);
  		dict->add(qstrdup("Descent"), &obj);
  		obj.initInt(pfd->values.ItalicAngle);
  		dict->add(qstrdup("ItalicAngle"), &obj);
  		obj.initInt(pfd->values.StemV);
  		dict->add(qstrdup("StemV"), &obj);
  	
  		obj.initInt(pfd->values.AvgWidth);
  		dict->add(qstrdup("AvgWidth"), &obj);
  		obj.initInt(pfd->values.Leading);
  		dict->add(qstrdup("Leading"), &obj);
  		obj.initInt(pfd->values.MaxWidth);
  		dict->add(qstrdup("MaxWidth"), &obj);
  		obj.initInt(pfd->values.MissingWidth);
  		dict->add(qstrdup("MissingWidth"), &obj);
  		obj.initInt(pfd->values.StemH);
  		dict->add(qstrdup("StemH"), &obj);
  		obj.initInt(pfd->values.XHeight);
  		dict->add(qstrdup("XHeight"), &obj);
		}
		
		if (is_subset)
		{
			switch (pfd->values.FontType)
			{
				case ft_CID_encrypted:
	    	case ft_CID_TrueType:
	    		dict->add(qstrdup("CIDSet"), &cidsetref);
					break;
					
	    	case ft_composite:
					return (int)(XWPSError::RangeCheck);
						
				default:
					{
						ulong subset_glyphs[256];
						uint subset_size = font->subsetGlyphs(subset_glyphs, pfd->chars_used.data);
						QByteArray ba;
						ba.append("(");
						for (int i = 0; i < subset_size; ++i)
						{
							uint len;
		    			const char *str = font->glyphName(subset_glyphs[i], &len);
		    			if (bytes_compare((const uchar *)str, len, (const uchar *)".notdef", 7))
		    			{
		    				ba.append("/");
		    				ba.append((const char *)str, len);
							}
						}
						ba.append(")");
						obj.initString(new XWString(ba.data(), ba.size()));
						dict->add(qstrdup("CharSet"), &obj);
					}
					break;
			}
		}
		
		if (pfd->FontFile)
		{
			switch (pfd->values.FontType)
			{
				case ft_TrueType:
	    	case ft_CID_TrueType:
	    		dict->add(qstrdup("FontFile2"), &pfd->FontFile->indirect);
					break;
					
	    	default:
					code = (int)(XWPSError::RangeCheck);
						
				case ft_encrypted:
					if (CompatibilityLevel < 1.2) 
					{
						dict->add(qstrdup("FontFile"), &pfd->FontFile->indirect);
		    		break;
					}
					
				case ft_encrypted2:
	    	case ft_CID_encrypted:
	    		dict->add(qstrdup("FontFile3"), &pfd->FontFile->indirect);
					break;
			}
		}
  }
  
  return code;
}

int XWPSDeviceDVI::writeFontResource(XWPSDVIFont *pef,
												XWPSString *,
												XWPSString *pbfname)
{
	XWPSDVIFontDescriptor *pfd = pef->FontDescriptor;
  static const char *const encoding_names[] = {
		"StandardEncoding", "ISOLatin1Encoding", "SymbolEncoding",
  	"DingbatsEncoding", "WinAnsiEncoding", "MacRomanEncoding",
  	"MacExpertEncoding"
    };
    
  XWDict * dict = pef->object.getDict();
  XWObject obj;
  char buf[100];
  switch (pef->FontType)
  {
  	case ft_composite:
  		obj.initName("Type0");
  		dict->add(qstrdup("Subtype"), &obj);
  		memcpy(buf, pbfname->data, pbfname->size);
  		buf[pbfname->size] = '\0';
  		obj.initName(buf);
  		dict->add(qstrdup("BaseFont"), &obj);
  		break;
  		
  	case ft_encrypted:
    case ft_encrypted2:
    	obj.initName("Type1");
  		dict->add(qstrdup("Subtype"), &obj);
  		memcpy(buf, pbfname->data, pbfname->size);
  		buf[pbfname->size] = '\0';
  		obj.initName(buf);
  		dict->add(qstrdup("BaseFont"), &obj);
  		break;
  		
  	case ft_CID_encrypted:
  		obj.initName("CIDFontType0");
  		dict->add(qstrdup("Subtype"), &obj);
			{
				XWPSFontCid0 * cidf = (XWPSFontCid0*)(pfd->getBaseFont());
				writeCIDSystemInfo(dict, (XWPSCidSystemInfo*)(cidf->getCIDSysInfo()));
			}
			memcpy(buf, pbfname->data, pbfname->size);
  		buf[pbfname->size] = '\0';
  		obj.initName(buf);
  		dict->add(qstrdup("BaseFont"), &obj);
  		break;
  		
  	case ft_CID_TrueType:
  		obj.initName("CIDFontType2");
  		dict->add(qstrdup("Subtype"), &obj);
			{
				XWPSFontCid2 * cidf = (XWPSFontCid2*)(pfd->getBaseFont());
				writeCIDSystemInfo(dict, (XWPSCidSystemInfo*)(cidf->getCIDSysInfo()));
			}
			memcpy(buf, pbfname->data, pbfname->size);
  		buf[pbfname->size] = '\0';
  		obj.initName(buf);
  		dict->add(qstrdup("BaseFont"), &obj);
  		break;
  		
  	case ft_TrueType:
  		obj.initName("TrueType");
  		dict->add(qstrdup("Subtype"), &obj);
			memcpy(buf, pbfname->data, pbfname->size);
  		buf[pbfname->size] = '\0';
  		obj.initName(buf);
  		dict->add(qstrdup("BaseFont"), &obj);
  		break;
			
		default:
			return (int)(XWPSError::RangeCheck);
  }
  
  if (pef->index < 0 || pfd->base_font || pfd->FontFile)
  	dict->add(qstrdup("FontDescriptor"), &pfd->indirect);
  	
  if (pef->write_Widths)
		writeWidths(dict, pef->FirstChar, pef->LastChar, pef->Widths);
		
	if (pef->Differences) 
	{
		int prev = 256;
		XWObject enc, encref, dif;
		enc.initDict(xref);
		xref->refObj(&enc, &encref);
		dict->add(qstrdup("Encoding"), &encref);
		
		obj.initName("Encoding");
		enc.dictAdd(qstrdup("Type"), &obj);
		if (pef->BaseEncoding != ENCODING_INDEX_UNKNOWN)
		{
			obj.initName(encoding_names[pef->BaseEncoding]);
			enc.dictAdd(qstrdup("BaseEncoding"), &obj);
		}
		
		dif.initArray(xref);
		for (int i = 0; i < 256; ++i)
	    if (pef->Differences[i].data != 0) 
	    {
				if (i != prev + 1)
				{
					obj.initInt(i);
					dif.arrayAdd(&obj);
		    	
		    }		    
		    memcpy(buf, pef->Differences[i].data, pef->Differences[i].size);
		    buf[pef->Differences[i].size] = '\0';
		    obj.initName(buf);
				dif.arrayAdd(&obj);
				prev = i;
	    }
	    
	  enc.dictAdd(qstrdup("Differences"), &dif);
	  if (!xref->isScanning())
	    xref->releaseObj(&enc);
	  else
	 	  enc.free();
	}
	else if (pef->BaseEncoding != ENCODING_INDEX_UNKNOWN) 
  {
  	obj.initName(encoding_names[pef->BaseEncoding]);
  	dict->add(qstrdup("Encoding"), &obj);
  }
  
  return 0;
}

int XWPSDeviceDVI::writeFontResources()
{	
	for (int j = 0; j < NUM_RESOURCE_CHAINS; ++j)
	{
		XWPSDVIFont * ppf = (XWPSDVIFont *)resources[resourceFont].chains[j];
		for (; ppf != 0; ppf = (XWPSDVIFont *)(ppf->next))
		{
			if (ppf->isSynthesized())
				writeSynthesizedType3(ppf);
	    else if (!ppf->skip)
	    {
	    	XWPSString font_name;
	    	XWPSDVIFontDescriptor * pfd = ppf->FontDescriptor;
				font_name.data = pfd->FontName.chars;
				font_name.size = pfd->FontName.size;
				if (hasSubsetPrefix(font_name.data, font_name.size))
		    	ppf->write_Widths = true;
				writeFontResource(ppf, &font_name, &font_name);
	    }
		}
		
		XWPSDVIFontDescriptor * pfd = (XWPSDVIFontDescriptor *)resources[resourceFontDescriptor].chains[j];
		for (; pfd != 0; pfd = (XWPSDVIFontDescriptor *)(pfd->next))
		{
	    if (!pfd->written) 
				finalizeFontDescriptor(pfd);
		}
	}
	return 0;
}

int XWPSDeviceDVI::writeFunction(XWPSFunction *pfn, XWObject * pid)
{
  return function(pfn, pid);
}

void XWPSDeviceDVI::writeImage(XWPSDVIImageWriter * piw, XWPSStream * s)
{
	XWObject dic;
	dic.initDict(piw->dict);
	s->pputs("\nBI\n");
	writeDict(&dic, s, true);
	dic.free();
	s->pputs((binary_ok ? "ID " : "ID\n"));		
	QByteArray ss = piw->data.streamReadAll();
	s->putBytes((const uchar *)ss.data(), ss.size());
	s->print("\nEI%s\n", piw->end_string);
}

int XWPSDeviceDVI::writeIndirect(XWObject * obj, XWPSStream * s)
{
	char buf[1024];
	int l = sprintf(buf, " %lu %hu R", obj->label, obj->generation);
	buf[l] = '\0';
  s->pputs(buf);
  return 0;
}

int XWPSDeviceDVI::writeMultipleHalftone(XWPSMultipleHalftone *pmht,
			    										XWPSDeviceHalftone *pdht, 
			    										XWObject *r)
{
	XWObject obj, dict;
	dict.initDict(xref);
	xref->refObj(&dict, r);
	
	obj.initName("Halftone");
	dict.dictAdd(qstrdup("Type"), &obj);
	obj.initInt(5);
	dict.dictAdd(qstrdup("HalftoneType"), &obj);
	
	int i, code;
  XWObject * ids = new XWObject[pmht->num_comp];
  for (i = 0; i < pmht->num_comp; ++i) 
  {
		XWPSHalftoneComponent * phtc = &pmht->components[i];
		XWPSHTOrder *porder =  (pdht->components == 0 ? &pdht->order :  &pdht->components[i].corder);
		
		switch (phtc->type) 
		{
			case ht_type_spot:
	    	code = writeSpotHalftone(phtc->params.spot, porder, &ids[i]);
	    	break;
	    	
			case ht_type_threshold:
	    	code = writeThresholdHalftone(phtc->params.threshold,porder,  &ids[i]);
	    	break;
	    	
			case ht_type_threshold2:
	    	code = writeThreshold2Halftone(phtc->params.threshold2,porder,  &ids[i]);
	    	break;
	    	
			default:
	    	code = XWPSError::RangeCheck;
		}
		if (code < 0) 
		{
			delete [] ids;
			dict.free();
	    return code;
		}
  }
  
  for (i = 0; i < pmht->num_comp; ++i) 
  {
		XWPSHalftoneComponent * phtc = &pmht->components[i];
		code = separationName(&obj, phtc->cname);
		if (code < 0)
		{
			obj.free();
			delete [] ids;
			dict.free();
	    return code;
	  }
	  
	  char * key = obj.getString()->getCString();
	  dict.dictAdd(qstrdup(key), &ids[i]);
	  obj.free();
  }
  delete [] ids;
  if (!xref->isScanning())
	  xref->releaseObj(&dict);
	else
	 	dict.free();
  return 0;
}

void XWPSDeviceDVI::writeName(XWObject * obj, XWPSStream * s)
{
	char * str  = obj->getName();
	char buf[100];
	int l = sprintf(buf, " /%s", str);
	buf[l] = '\0';
	s->pputs(buf);
}

void XWPSDeviceDVI::writeNull(XWPSStream * s)
{
	s->pputs(" null");
}

void XWPSDeviceDVI::writeNumber(XWObject * obj, XWPSStream * s)
{
	if (obj->isInt())
	{
		int num = obj->getInt();
		s->print(" %ld", num);
	}
	else
	{
		double num = obj->getReal();
		s->print(" %ld", num);
	}
}

void XWPSDeviceDVI::writeObj(XWObject * obj, XWPSStream * s, bool in_line)
{
	if (!obj || obj->isNull())
	{
		writeNull(s);
		return ;
	}
	
	switch (obj->getType())
	{
		case XWObject::Bool:
			writeBoolean(obj, s);
			break;
			
		case XWObject::Int:
		case XWObject::Real:
			writeNumber(obj, s);
			break;
			
		case XWObject::String:
			writeString(s, (const uchar*)(obj->getString()->getCString()), (uint)(obj->getString()->getLength()));
			break;
			
		case XWObject::Name:
			writeName(obj, s);
			break;
						
		case XWObject::Array:
			writeArray(obj, s, in_line);
			break;
			
		case XWObject::Dict:
			writeDict(obj, s, in_line);
			break;
						
		case XWObject::Ref:
		case XWObject::Indirect:
			writeIndirect(obj, s);
			break;
			
		default:
			break;
	}
}

int XWPSDeviceDVI::writeResourceObjects(PSPDFResourceType rtype)
{
	for (int j = 0; j < NUM_RESOURCE_CHAINS; ++j) 
	{
		XWPSDVIResource *pres = resources[rtype].chains[j];

		for (; pres != 0; pres = pres->next)
		{
	    if (!xref->isScanning() && pres->used_on_page)
	    	xref->releaseObj(&pres->object);
	    else
	    	pres->object.free();
	    	
	    pres->used_on_page = false;
		}
  }

  return 0;
}

int XWPSDeviceDVI::writeResourceObjectsLast(PSPDFResourceType rtype)
{
	for (int j = 0; j < NUM_RESOURCE_CHAINS; ++j) 
	{
		XWPSDVIResource *pres = resources[rtype].chains[j];

		for (; pres != 0; pres = pres->next)
		{
	    if (pres->used_on_page)
	    	xref->releaseObj(&pres->object);
	    else
	    	pres->object.free();
	    	
	    pres->used_on_page = false;
		}
  }

  return 0;
}

int  XWPSDeviceDVI::writeScreenHalftone(XWPSScreenHalftone *psht,
			  									 XWPSHTOrder *porder, 
			  									 XWObject *r)
{
	XWPSSpotHalftone spot;
  spot.screen = *psht;
  spot.accurate_screens = false;
  return writeSpotHalftone(&spot, porder, r);
}

int XWPSDeviceDVI::writeSpotFunction(XWPSHTOrder *porder,	XWObject *pid)
{
	int w = porder->width, h = porder->height;
  uint num_bits = porder->num_bits;
  XWPSFunctionSdParams params;
  
	uchar *values;
  XWPSFunction *pfn;
  uint i;
  int code = 0;

  params.m = 2;
  params.Domain = new float[4];
  params.Domain[0] = -1;
  params.Domain[1] = 1;
  params.Domain[2] = -1;
  params.Domain[3] = 1;
  params.n = 1;
  params.Range = new float[4];
  params.Range[0] = -1;
  params.Range[1] = 1;
  params.Range[2] = 0;
  params.Range[3] = 0;
  
  if (num_bits > 0x10000)
		return (int)(XWPSError::RangeCheck);
  params.BitsPerSample = 16;
  
  params.Size = new int[2];
  params.Size[0] = w;
  params.Size[1] = h;
  code = params.check();
  if (code < 0)
  	return code;
  
  values = new uchar[(num_bits * 2) * sizeof(uchar)];
  for (i = 0; i < num_bits; ++i)
  {
  	XWPSIntPoint pt;
		int value;

		if ((code = porder->bitIndex(i, &pt)) < 0)
	    break;
	    
	  value = pt.y * w + pt.x;
		values[i * 2] = (uchar)(value >> 8);
		values[i * 2 + 1] = (uchar)value;
  }
  
  params.DataSource.initBytes((const uchar *)values, sizeof(*values) * num_bits);
  pfn = new XWPSFunctionSd(&params);
  if (code >= 0)
  	code = writeFunction(pfn, pid);
  if (values)
  	delete [] values;
  delete pfn;
  return code;
}

int XWPSDeviceDVI::writeSpotHalftone(XWPSSpotHalftone *psht,
			                   XWPSHTOrder *porder,
			                   XWObject *r)
{		
	XWObject obj, spot_id, dict;
	dict.initDict(xref);
	xref->refObj(&dict, r);
	
	int code = 0;
		
	obj.initName("Halftone");
	dict.dictAdd(qstrdup("Type"), &obj);
	obj.initInt(1);
	dict.dictAdd(qstrdup("HalftoneType"), &obj);
	obj.initReal(psht->screen.actual_frequency);
  dict.dictAdd(qstrdup("Frequency"), &obj);
  obj.initReal(psht->screen.actual_angle);
  dict.dictAdd(qstrdup("Angle"), &obj);
  
  if (psht->accurate_screens)
  {
  	obj.initBool(true);
  	dict.dictAdd(qstrdup("AccurateScreens"), &obj);
  }
  
	int i = _COUNT_OF_HT_FUNCS_;
	
	{
		XWPSScreenEnum senum;
		XWPSHTOrder order;
		int code;

		order = *porder;
		code = order.screenOrderAlloc();
		if (code < 0)
	    goto notrec;
		for (i = 0; i < _COUNT_OF_HT_FUNCS_; ++i) 
		{
	    float (*spot_proc)(float, float) = ht_functions[i].proc;
	    XWPSPoint pt;

	    senum.initMemory(&order, NULL, &psht->screen);
	    while ((code = senum.currentPoint(&pt)) == 0 && senum.next(spot_proc(pt.x, pt.y)) >= 0)
				;
	    if (code < 0)
				continue;
				
	    if (memcmp(order.levels->ptr, porder->levels->ptr, order.num_levels * sizeof(*order.levels)))
				continue;
	    if (memcmp(order.bit_data->arr, porder->bit_data->arr, order.num_bits * porder->procs->bit_data_elt_size))
				continue;
	    break;
		}
  }
  
notrec:
  if (i == _COUNT_OF_HT_FUNCS_) 
		writeSpotFunction(porder, &spot_id);
  
  if (i < _COUNT_OF_HT_FUNCS_)
  {
  	obj.initName(ht_functions[i].fname);
  	dict.dictAdd(qstrdup("SpotFunction"), &obj);
  }
  else
  	dict.dictAdd(qstrdup("SpotFunction"), &spot_id);
  
  code = writeTransfer(porder->transfer, "TransferFunction", dict.getDict());
  
  if (!xref->isScanning())
	  xref->releaseObj(&dict);
	else
	 	dict.free();
  return 0;
}

int XWPSDeviceDVI::writeSynthesizedType3(XWPSDVIFont *pef)
{
	XWObject obj, proc, box, mat;
	XWDict * dict = pef->object.getDict();
	XWPSIntRect bbox;
  int widths[256];
  memset(widths, 0, sizeof(widths));
  obj.initName(pef->frname);
  dict->add(qstrdup("Name"), &obj);
  obj.initName("Type3");
  dict->add(qstrdup("Subtype"), &obj);
  obj.initRef(embedded_encoding_id, 0);
  dict->add(qstrdup("Encoding"), &obj);
  XWPSDVICharProc * pcp = pef->char_procs;
  proc.initDict(xref);
  char buf[20];
  for (; pcp; pcp = pcp->char_next)
  {
  	bbox.p.y = qMin(bbox.p.y, pcp->y_offset);
	  bbox.q.x = qMax(bbox.q.x, pcp->width);
	  bbox.q.y = qMax(bbox.q.y, pcp->height + pcp->y_offset);
	  widths[pcp->char_code] = pcp->x_width;
	  sprintf(buf, "a%ld", pcp->char_code);
	  proc.dictAdd(qstrdup(buf), &pcp->indirect);
  }
  
  for (int w = 0; w < (sizeof(pef->spaces) / sizeof(pef->spaces[0])); ++w)
  {
  	uchar ch = pef->spaces[w];
	  if (ch) 
	  {
	  	sprintf(buf, "a%ld", ch);
	  	obj.initRef(space_char_ids[w], 0);
	  	proc.dictAdd(qstrdup(buf), &obj);
			widths[ch] = w + X_SPACE_MIN;
	  }
  }
  dict->add(qstrdup("CharProcs"), &proc);
  box.initArray(xref);
  obj.initInt(bbox.p.x);
  box.arrayAdd(&obj);
  obj.initInt(bbox.p.y);
  box.arrayAdd(&obj);
  obj.initInt(bbox.q.x);
  box.arrayAdd(&obj);
  obj.initInt(bbox.q.y);
  box.arrayAdd(&obj);
  dict->add(qstrdup("FontBBox"), &box);
  
  mat.initArray(xref);
  obj.initInt(1);
  mat.arrayAdd(&obj);
  obj.initInt(0);
  mat.arrayAdd(&obj);
  obj.initInt(0);
  mat.arrayAdd(&obj);
  obj.initInt(1);
  mat.arrayAdd(&obj);
  obj.initInt(0);
  mat.arrayAdd(&obj);
  obj.initInt(0);
  mat.arrayAdd(&obj);
  dict->add(qstrdup("FontMatrix"), &mat);
  writeWidths(dict, 0, pef->num_chars - 1, widths);
  return 0;
}

int XWPSDeviceDVI::writeThresholdHalftone(XWPSThresholdHalftone *ptht,
			     									 XWPSHTOrder *porder,
			     									 XWObject *r)
{
	XWObject obj, s;
	s.initStream(0, xref);
	xref->refObj(&s, r);
	
	XWDict * dict = s.streamGetDict();
	obj.initName("Halftone");
	dict->add(qstrdup("Type"), &obj);
	obj.initInt(6);
	dict->add(qstrdup("HalftoneType"), &obj);
	obj.initInt(ptht->width);
	dict->add(qstrdup("Width"), &obj);
	obj.initInt(ptht->height);
	dict->add(qstrdup("Height"), &obj);
	int code = writeTransfer(porder->transfer, "TransferFunction", s.getDict());
	if (code < 0)
	{
		s.free();
		return code;
	}
	XWPSDVIDataWriter writer;
	writer.object= &s;
	code = beginData(&writer);
  if (code < 0)
  {
  	s.free();
		return code;
	}
  writer.binary.strm->write(ptht->thresholds.data, ptht->thresholds.size);
  code = endData(&writer);
  return code;
}

int XWPSDeviceDVI::writeThreshold2Halftone(XWPSThresholdHalftone2 *ptht,
			                        XWPSHTOrder *porder, 
			                        XWObject *r)
{
	XWObject obj, s;
	s.initStream(0, xref);
	xref->refObj(&s, r);
	
	XWDict * dict = s.streamGetDict();	
	obj.initName("Halftone");
	dict->add(qstrdup("Type"), &obj);
	obj.initInt(16);
	dict->add(qstrdup("HalftoneType"), &obj);
	obj.initInt(ptht->width);
	dict->add(qstrdup("Width"), &obj);
	obj.initInt(ptht->height);
	dict->add(qstrdup("Height"), &obj);
	if (ptht->width2 && ptht->height2)
	{
		obj.initInt(ptht->width2);
		dict->add(qstrdup("Width2"), &obj);
		obj.initInt(ptht->height2);
		dict->add(qstrdup("Height2"), &obj);
	}
	int code = writeTransfer(porder->transfer, "TransferFunction", dict);
	if (code < 0)
	{
		s.free();
		return code;
	}
	XWPSDVIDataWriter writer;
	writer.object = &s;
	code = beginData(&writer);
  if (code < 0)
  {
  	s.free();
		return code;
	}
	if (ptht->bytes_per_sample == 2)
		s.streamAdd((const char*)(ptht->thresholds.data), ptht->thresholds.size);
  else 
  {
		int i;

		for (i = 0; i < ptht->thresholds.size; ++i) 
		{
	    uchar b = ptht->thresholds.data[i];

	    s.streamAdd((const char*)&b, 1);
	    s.streamAdd((const char*)&b, 1);
		}
  }
  code = endData(&writer);
  return code;
}

int XWPSDeviceDVI::writeTransfer(XWPSTransferMap *map,
		   							const char *key, 
		   							XWDict *ids)
{
	return writeTransferMap(map, 0, true, key, ids);
}

int XWPSDeviceDVI::writeTransferMap(XWPSTransferMap *map,
		       						 int range0, 
		       						 bool check_identity,
		                   const char *key, 
		                   XWDict *ids)
{
	XWPSFunctionSdParams params;
  XWPSFunction *pfn;
  XWObject id;
  int code;
	
  if (map == 0) 
		return 0;
    
  if (check_identity) 
  {
		int i;

		if (map->isIdentity())
	    i = transfer_map_size;
		else
	    for (i = 0; i < transfer_map_size; ++i)
				if (map->values[i] != bits2frac(i, log2_transfer_map_size))
		    	break;
		if (i == transfer_map_size) 
		{
			id.initName("Identity");
			ids->add(qstrdup(key), &id);
	    return 1;
		}
  }
  params.m = 1;
  params.Domain = new float[2];
  params.Domain[0] = 0;
  params.Domain[1] = 1;
  params.n = 1;
  params.Range = new float[2];
  params.Range[0] = range0;
  params.Range[1] = 1;
  params.Order = 1;
  params.DataSource.access =	(range0 < 0 ? &XWPSDataSource::transferMapAccessSigned : &XWPSDataSource::transferMapAccess);
  params.DataSource.data.str.data = (const uchar *)map;
  
  params.BitsPerSample = 8;
  params.Size = new int[1];
  params.Size[0] = transfer_map_size;
  code = params.check();
  if (code < 0)
  	return code;
  	
  pfn = new XWPSFunctionSd(&params);
  if (!pfn || !pfn->params)
		return -1;
  code = writeFunction(pfn, &id);
  delete pfn;
  if (code < 0)
		return code;
	
  ids->add(qstrdup(key), &id);
  return 0;
}

int XWPSDeviceDVI::writeWidths(XWDict * dict, int first, int last, const int widths[256])
{
	XWObject a, obj;
	obj.initInt(first);
	dict->add(qstrdup("FirstChar"), &obj);
	obj.initInt(last);
	dict->add(qstrdup("LastChar"), &obj);
	a.initArray(xref);
	for (int i = first; i <= last; ++i)
	{
		obj.initInt(widths[i]);
		a.arrayAdd(&obj);
	}
	dict->add(qstrdup("Widths"), &a);
  return 0;
}
