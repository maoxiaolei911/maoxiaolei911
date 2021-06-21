/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <math.h>
#include <QIODevice>
#include "XWPSError.h"
#include "psbittable.h"
#include "XWPSParam.h"
#include "XWPSCosObject.h"
#include "XWPSFunction.h"
#include "XWPSStream.h"
#include "XWPSCie.h"
#include "XWPSColorSpace.h"
#include "XWPSShading.h"
#include "XWPSFont.h"
#include "XWPSPath.h"
#include "XWPSText.h"
#include "XWPSHalfTone.h"
#include "XWPSPattern.h"
#include "XWPSState.h"
#include "XWPSDevicePDF.h"

const int CoreDistVersion = 4000;
static bool MAKE_FONT_NAMES_UNIQUE = true;


XWPSPDFPage::XWPSPDFPage()
{
	Page = 0;
	procsets = NoMarks;
	contents_id = 0;
	for (int i = 0; i < resourceFont; i++)
		resource_ids[i] = 0;
	fonts_id = 0;
	Annots = 0;
	for (int i = 0; i < 5; i++)
		text_rotation.counts[i] = 0;
	text_rotation.Rotate = 0;
}

XWPSPDFPage::~XWPSPDFPage()
{
	if (Page)
	{
		if (Page->decRef() == 0)
		  delete Page;
		Page = 0;
	}
	
	if (Annots)
	{
		delete Annots;
		Annots = 0;
	}
}

XWPSPDFPage & XWPSPDFPage::operator=(XWPSPDFPage & other)
{
	Page = other.Page;
	MediaBox = other.MediaBox;
	procsets = other.procsets;
	contents_id = other.contents_id;
	memcpy(resource_ids, other.resource_ids, resourceFont * sizeof(long));
	fonts_id = other.fonts_id;
	Annots = other.Annots;
	memcpy(text_rotation.counts, other.text_rotation.counts, 5 * sizeof(long));
	text_rotation.Rotate = other.text_rotation.Rotate;
	other.Page = 0;
	other.Annots = 0;
	return *this;
}

XWPSPDFOutlineNode::XWPSPDFOutlineNode()
{
	id = 0; 
	parent_id = 0; 
	prev_id = 0; 
	first_id = 0; 
	last_id = 0;
	count = 0;
	action = 0;
}

XWPSPDFOutlineNode::~XWPSPDFOutlineNode()
{
	if (action)
	{
		delete action;
		action = 0;
	}
}

XWPSPDFOutlineLevel::XWPSPDFOutlineLevel()
{
	left = max_int;
}

XWPSPDFArticle::XWPSPDFArticle()
{
	next = 0;
	contents = 0;
}

XWPSPDFArticle::~XWPSPDFArticle()
{
	if (contents)
	{
		delete contents;
		contents = 0;
	}
}

XWPSPDFGraphicsSave::XWPSPDFGraphicsSave()
{
	prev = 0;
	object = 0;
	position = 0;
	save_context = PDF_IN_NONE;
	save_contents_id = 0;
}

XWPSPDFGraphicsSave::~XWPSPDFGraphicsSave()
{
}

XWPSPDFDataWriter::XWPSPDFDataWriter()
{
	start = 0;
	length_id = 0;
}

int XWPSPDFDataWriter::endData()
{
	XWPSDevicePDF *pdev = (XWPSDevicePDF *)binary.dev;
  int code = binary.endBinary();
  long length = pdev->strm->tell() - start;

  if (code < 0)
		return code;
    
  pdev->strm->pputs("\nendstream\n");
  pdev->endSeparate();
  pdev->openSeparate(length_id);
  pdev->strm->print("%ld\n", length);
  return pdev->endSeparate();
}

XWPSPDFImageWriter::XWPSPDFImageWriter()
{
	pin = 0;
	pres = 0;
	height = 0;
	data = 0;
	end_string = 0;
}

XWPSPDFImageWriter::~XWPSPDFImageWriter()
{
	if (!pres)
	{
		if (data)
		{
			if (data->decRef() == 0)
				delete data;
			data = 0;
		}
	}
}

#define PSDF_VERSION_INITIAL psdf_version_ll3
#define PDF_COMPATIBILITY_LEVEL_INITIAL 1.4

#define compression_filter_name "FlateDecode"

context_proc XWPSDevicePDF::context_procs[4][4] =
{
	{0, &XWPSDevicePDF::noneToStream, &XWPSDevicePDF::noneToStream, &XWPSDevicePDF::noneToStream},
  {&XWPSDevicePDF::streamToNone, 0, &XWPSDevicePDF::streamToText, &XWPSDevicePDF::streamToText},
  {&XWPSDevicePDF::textToStream, &XWPSDevicePDF::textToStream, 0, 0},
  {&XWPSDevicePDF::stringToText, &XWPSDevicePDF::stringToText, &XWPSDevicePDF::stringToText, 0}
};

static const char *const resource_type_names[] = 
{
  "ColorSpace", 
  "ExtGState", 
  "Pattern", 
  "Shading", 
  "XObject", 
  "Font",
  0, 0, 0
};

PSPDFMarkName XWPSDevicePDF::mark_names[] = 
{
	{"ANN",          &XWPSDevicePDF::pdfmarkANN,         PDFMARK_NAMEABLE},
    {"LNK",        &XWPSDevicePDF::pdfmarkLNK,         PDFMARK_NAMEABLE},
    {"OUT",        &XWPSDevicePDF::pdfmarkOUT,         0},
    {"ARTICLE",    &XWPSDevicePDF::pdfmarkARTICLE,     0},
    {"DEST",       &XWPSDevicePDF::pdfmarkDEST,        PDFMARK_NAMEABLE},
    {"PS",         &XWPSDevicePDF::pdfmarkPS,          PDFMARK_NAMEABLE},
    {"PAGES",      &XWPSDevicePDF::pdfmarkPAGES,       0},
    {"PAGE",       &XWPSDevicePDF::pdfmarkPAGE,        0},
    {"DOCINFO",    &XWPSDevicePDF::pdfmarkDOCINFO,     0},
    {"DOCVIEW",     &XWPSDevicePDF::pdfmarkDOCVIEW,     0},
	/* Named objects. */
    {"BP",          &XWPSDevicePDF::pdfmarkBP,          PDFMARK_NAMEABLE},
    {"EP",           &XWPSDevicePDF::pdfmarkEP,          0},
    {"SP",          &XWPSDevicePDF::pdfmarkSP,          PDFMARK_ODD_OK | PDFMARK_KEEP_NAME},
    {"OBJ",          &XWPSDevicePDF::pdfmarkOBJ,         PDFMARK_NAMEABLE},
    {"PUT",          &XWPSDevicePDF::pdfmarkPUT,         PDFMARK_ODD_OK | PDFMARK_KEEP_NAME},
    {".PUTDICT",     &XWPSDevicePDF::pdfmarkPUTDICT,     PDFMARK_ODD_OK | PDFMARK_KEEP_NAME},
    {".PUTINTERVAL", &XWPSDevicePDF::pdfmarkPUTINTERVAL, PDFMARK_ODD_OK | PDFMARK_KEEP_NAME},
    {".PUTSTREAM",   &XWPSDevicePDF::pdfmarkPUTSTREAM,   PDFMARK_ODD_OK | PDFMARK_KEEP_NAME |
                                          PDFMARK_NO_REFS},
    {"CLOSE",        &XWPSDevicePDF::pdfmarkCLOSE,       PDFMARK_ODD_OK | PDFMARK_KEEP_NAME},
    {"NamespacePush", &XWPSDevicePDF::pdfmarkNamespacePush, 0},
    {"NamespacePop", &XWPSDevicePDF::pdfmarkNamespacePop, 0},
    {"NI",           &XWPSDevicePDF::pdfmarkNI,          PDFMARK_NAMEABLE},
	/* Document structure. */
    {"StRoleMap",    &XWPSDevicePDF::pdfmarkStRoleMap,   0},
    {"StClassMap",   &XWPSDevicePDF::pdfmarkStClassMap,  0},
    {"StPNE",        &XWPSDevicePDF::pdfmarkStPNE,       PDFMARK_NAMEABLE},
    {"StBookmarkRoot", &XWPSDevicePDF::pdfmarkStBookmarkRoot, 0},
    {"StPush",       &XWPSDevicePDF::pdfmarkStPush,       0},
    {"StPop",        &XWPSDevicePDF::pdfmarkStPop,        0},
    {"StPopAll",     &XWPSDevicePDF::pdfmarkStPopAll,     0},
    {"StBMC",        &XWPSDevicePDF::pdfmarkStBMC,        0},
    {"StBDC",        &XWPSDevicePDF::pdfmarkStBDC,        0},
    {"EMC",          &XWPSDevicePDF::pdfmarkEMC,          0},
    {"StOBJ",        &XWPSDevicePDF::pdfmarkStOBJ,        0},
    {"StAttr",       &XWPSDevicePDF::pdfmarkStAttr,       0},
    {"StStore",      &XWPSDevicePDF::pdfmarkStStore,      0},
    {"StRetrieve",   &XWPSDevicePDF::pdfmarkStRetrieve,   0},
	/* End of list. */
    {0, 0}
};

static int cos_array_add_vector3(XWPSCosObject *pca, const PSVector3 *pvec)
{
  int code = pca->arrayAdd(pvec->u);
  if (code >= 0)
		code = pca->arrayAdd(pvec->v);
  if (code >= 0)
		code = pca->arrayAdd(pvec->w);
  return code;
}

static int cos_dict_put_c_key_vector3(XWPSCosObject *pcd, 
                                      const char *key,
			                                const PSVector3 *pvec)
{
  XWPSCosObject *pca = new XWPSCosObject(pcd->pdev, &cos_array_procs);
  
  int code = cos_array_add_vector3(pca, pvec);
  if (code < 0) 
  {
		delete pca;
		return code;
  }
  
  XWPSCosValue v(pca);
  return pcd->dictPut(key, &v);
}

static int cos_dict_put_matrix(XWPSCosObject *pscd, const char *key, XWPSMatrix *pmat)
{
	float matrix[6];

  matrix[0] = pmat->xx;
  matrix[1] = pmat->xy;
  matrix[2] = pmat->yx;
  matrix[3] = pmat->yy;
  matrix[4] = pmat->tx;
  matrix[5] = pmat->ty;
  return pscd->dictPut(key, matrix, 6);
}

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

static int put_float_mesh_data(XWPSCosObject *pscs, XWPSShadeCoordStream *cs,
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
  if ((code = pscs->streamAddBytes(b + (flag < 0), (flag >= 0) + num_pts * 6)) < 0)
		return code;
  for (i = 0; i < num_components; ++i) 
  {
		float c;

		cs->getDecoded(0, NULL, &c);
		put_clamped(b, (is_indexed ? c + 32768 : (c + 256) * 65535 / 511), 2);
		if ((code = pscs->streamAddBytes(b, 2)) < 0)
	    return code;
  }
  return 0;
}

const PSParamItem pdf_param_items[] = {
	{"PDFEndPage", ps_param_type_int, _OFFSET_OF_(XWPSDevicePDF,EndPage)},
{"PDFStartPage", ps_param_type_int, _OFFSET_OF_(XWPSDevicePDF,StartPage)},
{"Optimize", ps_param_type_bool, _OFFSET_OF_(XWPSDevicePDF,Optimize)},
{"ParseDSCCommentsForDocInfo", ps_param_type_bool,  _OFFSET_OF_(XWPSDevicePDF,ParseDSCCommentsForDocInfo)},
{"ParseDSCComments", ps_param_type_bool, _OFFSET_OF_(XWPSDevicePDF,ParseDSCComments)},
{"EmitDSCWarnings", ps_param_type_bool, _OFFSET_OF_(XWPSDevicePDF,EmitDSCWarnings)},
{"CreateJobTicket", ps_param_type_bool, _OFFSET_OF_(XWPSDevicePDF,CreateJobTicket)},
{"PreserveEPSInfo", ps_param_type_bool, _OFFSET_OF_(XWPSDevicePDF,PreserveEPSInfo)},
{"AutoPositionEPSFiles", ps_param_type_bool, _OFFSET_OF_(XWPSDevicePDF,AutoPositionEPSFiles)},
{"PreserveCopyPage", ps_param_type_bool, _OFFSET_OF_(XWPSDevicePDF,PreserveCopyPage)},
{"UsePrologue", ps_param_type_bool, _OFFSET_OF_(XWPSDevicePDF,UsePrologue)},

 {"ReAssignCharacters", ps_param_type_bool, _OFFSET_OF_(XWPSDevicePDF,ReAssignCharacters)},
 {"ReEncodeCharacters", ps_param_type_bool, _OFFSET_OF_(XWPSDevicePDF,ReEncodeCharacters)},
 {"FirstObjectNumber", ps_param_type_long, _OFFSET_OF_(XWPSDevicePDF,FirstObjectNumber)},
 {0,0,0}
};
  
static int
pdf_make_mxd(XWPSDevice **pmxdev, XWPSDevice *tdev)
{
  XWPSDeviceNull *fdev = new XWPSDeviceNull;
  fdev->context_state = tdev->context_state;
  fdev->setTarget(tdev);
  *pmxdev = fdev;
  return 0;
}

static int
pdf_image3_make_mid(XWPSDevice **pmidev, XWPSDevice *dev, int , int )
{
  int code = pdf_make_mxd(pmidev, dev);

  if (code < 0)
		return code;
  (*pmidev)->procs.begin_typed_image_.begin_typed_imagenull = &XWPSDeviceNull::beginTypedImagePDFMid;
  return 0;
}

static int
pdf_image3_make_mcde(XWPSDevice *dev, 
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
  int code = pdf_make_mxd(pmcdev, midev);
  XWPSPDFImageEnum *pmie;
  XWPSPDFImageEnum *pmce;
  XWPSCosObject *pmcs;

  if (code < 0)
		return code;
	
	XWPSDevicePDF * pdev = (XWPSDevicePDF*)dev;
    code = pdev->beginTypedImage(pis, pmat, pic, prect, pdcolor, pcpath,  pinfo, PDF_IMAGE_TYPE3_DATA);
  if (code < 0)
		return code;
		
  if ((*pinfo)->procs.plane_data_.plane_datapdf != &XWPSPDFImageEnum::pdfImagePlaneData) 
  {
		(*pinfo)->end(false);
		if (!((*pmcdev)->decRef()))
			delete *pmcdev;
		*pmcdev = 0;
		
		return (int)(XWPSError::RangeCheck);
  }
  pmie = (XWPSPDFImageEnum *)pminfo;
  pmce = (XWPSPDFImageEnum *)(*pinfo);
  pmcs = pmce->writer->pres->object;
  if (pmie->writer->pres->object)
  	pmie->writer->pres->object->incRef();
  return pmcs->dictPut("/Mask",  pmie->writer->pres->object);
}

static int
pdf_image3x_make_mid(XWPSDevice **pmidev, XWPSDevice *dev, int , int , int )
{
  int code = pdf_make_mxd(pmidev, dev);

  if (code < 0)
		return code;
  (*pmidev)->procs.begin_typed_image_.begin_typed_imagenull = &XWPSDeviceNull::beginTypedImagePDFMid;
  return 0;
}

static int
pdf_image3x_make_mcde(XWPSDevice *dev, 
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
  XWPSPDFImageEnum *pmie;
  XWPSPDFImageEnum *pmce;
  XWPSCosObject *pmcs;
  int i;
  const XWPSImage3xMask *pixm;

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
			
  code = pdf_make_mxd(pmcdev, midev[i]);
  if (code < 0)
		return code;
		
	XWPSDevicePDF * pdev = (XWPSDevicePDF*)dev;
  code = pdev->beginTypedImage(pis, pmat, pic, prect, pdcolor, pcpath, pinfo, PDF_IMAGE_TYPE3_DATA);
  if (code < 0)
		return code;
    
  if ((*pinfo)->procs.plane_data_.plane_datapdf != &XWPSPDFImageEnum::pdfImagePlaneData) 
  {
		(*pinfo)->end(false);
		delete *pinfo;
		*pinfo = 0;
		return (int)(XWPSError::RangeCheck);
  }
  pmie = (XWPSPDFImageEnum *)pminfo[i];
  pmce = (XWPSPDFImageEnum *)(*pinfo);
  pmcs = pmce->writer->pres->object;
  
  if (pixm->has_Matte) 
  {
		int num_components = pim->ColorSpace->numComponents();

		code = pmie->writer->pres->object->dictPut("/Matte", pixm->Matte, num_components);
		if (code < 0)
	    return code;
  }
  if (pmie->writer->pres->object)
  	pmie->writer->pres->object->incRef();
  return pmcs->dictPut("/SMask", pmie->writer->pres->object);
}


XWPSDevicePDF::XWPSDevicePDF(QObject * parent)
	:XWPSDevicePsdf("pdfwrite", 
		              DEFAULT_WIDTH_10THS * X_DPI / 10,
		              DEFAULT_HEIGHT_10THS * Y_DPI / 10,
		              X_DPI, 
		              Y_DPI,
		              3, 
		              24, 
		              255, 
		              255, 
		              256, 
		              256,
		              PSDF_VERSION_INITIAL,
		              false,
		              parent)
{
	procs.open__.openpdf = &XWPSDevicePDF::openPDF;
	procs.get_initial_matrix_.get_initial_matrix = &XWPSDevice::getInitialMatrixUpright;
	procs.output_page_.output_pagepdf = &XWPSDevicePDF::outputPagePDF;
	procs.close__.closepdf = &XWPSDevicePDF::closePDF;
	procs.map_rgb_color_.map_rgb_color = &XWPSDevice::mapRGBColorRGBDefault;
	procs.map_color_rgb_.map_color_rgb = &XWPSDevice::mapColorRGBRGBDefault;
	procs.fill_rectangle_.fill_rectanglepdf = &XWPSDevicePDF::fillRectanglePDF;
	procs.copy_mono_.copy_monopdf = &XWPSDevicePDF::copyMonoPDF;
	procs.copy_color_.copy_colorpdf = &XWPSDevicePDF::copyColorPDF;
	procs.get_params_.get_paramspdf = &XWPSDevicePDF::getParamsPDF;
	procs.put_params_.put_paramspdf = &XWPSDevicePDF::putParamsPDF;
	procs.get_page_device_.get_page_device = &XWPSDevice::getPageDevicePageDevice;
	procs.fill_path_.fill_pathpdf = &XWPSDevicePDF::fillPathPDF;
	procs.stroke_path_.stroke_pathpdf = &XWPSDevicePDF::strokePathPDF;
	procs.fill_mask_.fill_maskpdf = &XWPSDevicePDF::fillMaskPDF;
	procs.strip_tile_rectangle_.strip_tile_rectanglepdf = &XWPSDevicePDF::stripTileRectanglePDF;
	procs.begin_typed_image_.begin_typed_imagepdf = &XWPSDevicePDF::beginTypedImagePDF;
	procs.text_begin_.text_beginpdf = &XWPSDevicePDF::textBeginPDF;
	
	vec_procs.setlinewidth_.setlinewidthpdf = &XWPSDevicePDF::setLineWidthPDF;
	vec_procs.setfillcolor_.setfillcolorpdf = &XWPSDevicePDF::setFillColorPDF;
	vec_procs.setstrokecolor_.setstrokecolorpdf = &XWPSDevicePDF::setStrokeColorPDF;
	vec_procs.dorect_.dorectpdf = &XWPSDevicePDF::doRectPDF;
	vec_procs.endpath_.endpathpdf = &XWPSDevicePDF::endPathPDF;
		
	CompatibilityLevel = PDF_COMPATIBILITY_LEVEL_INITIAL;
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
	fill_overprint = false;
	stroke_overprint = false;
	overprint_mode = 0;
	halftone_id = ps_no_id;
	transfer_ids[0] = ps_no_id;
	transfer_ids[1] = ps_no_id;
	transfer_ids[2] = ps_no_id;
	transfer_ids[3] = ps_no_id;
	black_generation_id = ps_no_id;
	undercolor_removal_id = ps_no_id;
	compression = pdf_compress_none;
	xref = new XWPSPDFTempFile;
	asides = new XWPSPDFTempFile;
	streams = new XWPSPDFTempFile;
	pictures = new XWPSPDFTempFile;
	open_font = 0;
	use_open_font = false;
	embedded_encoding_id = 0;
	next_id = 0;
	Catalog = 0;
	Info = 0;
	Pages = 0;
	outlines_id = 0;
	next_page = 0;
	contents_id = 0;
	context = PDF_IN_NONE;
	contents_length_id = 0;
	contents_pos = 0;
	memset(space_char_ids, 0, sizeof(space_char_ids));
	procsets = NoMarks;
	for (int i = 0; i < 5; i++)
		text_rotation.counts[i] = 0;
	text_rotation.Rotate = 0;
	pages = 0;
	num_pages = 0;
	cs_Patterns[0] = 0;
	cs_Patterns[1] = 0;
	cs_Patterns[2] = 0;
	cs_Patterns[3] = 0;
	cs_Patterns[4] = 0;
	last_resource = 0;
	outline_depth = 0;
	closed_outline_depth = 0;
	outlines_open = 0;
	articles = 0;
	Dests = 0;
	named_objects = 0;
	open_graphics = 0;
}

XWPSDevicePDF::~XWPSDevicePDF()
{
	closeFiles(0);
	
	if (named_objects)
	{
		delete named_objects;
		named_objects = 0;
	}
	
	XWPSPDFGraphicsSave * cg = open_graphics;
	while (cg)
	{
		XWPSPDFGraphicsSave * t = cg->prev;
		delete cg;
		cg = t;
	}
	
	if (articles != 0) 
	{
		XWPSPDFArticle *part;

		while ((part = articles) != 0) 
		{
	    articles = part->next;
	    delete part;
		}
  }
  
  if (Dests) 
  {
		delete Dests;
		Dests = 0;
  }
  
  if (last_resource != 0)
  {
  	XWPSPDFResource *pres;
		XWPSPDFResource *prev;

		for (prev = last_resource; (pres = prev) != 0;) 
		{
	  	prev = pres->prev;
	  	delete pres;
		}
		last_resource = 0;
	}
	
	if (Pages)
	{
		if (Pages->decRef() == 0)
			delete Pages;
		Pages = 0;
	}
}

int XWPSDevicePDF::appendChars(const uchar * str, uint size)
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

int  XWPSDevicePDF::beginTypedImage(XWPSImagerState * pis,
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
  XWPSPDFImageEnum *pie;
  PSImageFormat format;
  XWPSColorSpace *pcs;
  XWPSColorSpace cs_gray_temp;
  cs_gray_temp.i_ctx_p = context_state;
  XWPSCosValue cs_value;
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
	    		
				return pim3->beginImage3Generic(this, pis, pmat, prect, pdcolor, pcpath, pdf_image3_make_mid, pdf_image3_make_mcde, pinfo);
    	}
    	
    case IMAGE3X_IMAGETYPE: 
    	{
				XWPSImage3X *pim3x = (XWPSImage3X *)pic;

				if (CompatibilityLevel < 1.4)
	    		goto nyi;
				if (prect && !(prect->p.x == 0 && prect->p.y == 0 &&
		       	prect->q.x == pim3x->Width && prect->q.y == pim3x->Height))
	    		goto nyi;
				return pim3x->beginImage3xGeneric(this, pis, pmat, prect, pdcolor, pcpath,pdf_image3x_make_mid,	pdf_image3x_make_mcde, pinfo);
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
  
  pie = new XWPSPDFImageEnum;
  *pinfo = pie;
  pie->commonInit(pim, this, num_components, format);
  if (contextA == PDF_IMAGE_TYPE3_MASK)
  	pie->procs.end_image_.end_imagepdf = &XWPSPDFImageEnum::pdfImageEndImageObject;
  else
  	pie->procs.end_image_.end_imagepdf = &XWPSPDFImageEnum::pdfImageEndImage;
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
	    delete pie;
	    *pinfo = 0;
	    return code;
		}
  }
  
  if ((code = beginWriteImage(pie->writer, ps_no_id, widthA, heightA, NULL, in_line)) < 0 ||
			(code = pie->writer->binary.setupImageFilters(this, image.pixel, pmat, pis)) < 0 ||
			(code = beginImageData(pie->writer, image.pixel, &cs_value)) < 0)
		return code;
  return 0;
 
nyi:
  return beginTypedImageDefault(pis, pmat, pic, prect, pdcolor, pcpath, pinfo);
}

int XWPSDevicePDF::beginTypedImagePDF(XWPSImagerState * pis,
			                        XWPSMatrix *pmat, 
			                        XWPSImageCommon *pic,
			   											XWPSIntRect * prect,
			                        XWPSDeviceColor * pdcolor,
			                        XWPSClipPath * pcpath, 
			                        XWPSImageEnumCommon ** pinfo)
{
	return beginTypedImage(pis, pmat, pic, prect, pdcolor, pcpath, pinfo, PDF_IMAGE_DEFAULT);
}

int XWPSDevicePDF::closePDF()
{
	while (!registeredFonts.isEmpty())
	{
		XWPSPDFFontDescriptor * pfd = registeredFonts.takeFirst();
		if (pfd)
			finalizeFontDescriptor(pfd);
	}
	
	while (!registeredStdFonts.isEmpty())
	{
		int i = registeredStdFonts.takeFirst();
		std_fonts[i].font = 0;
	}
	
	if (!xref)
		return 0;
		
	QIODevice *tfile = xref->file;
	if (!tfile)
		return 0;
		
	long Catalog_id = Catalog->id, Info_id = Info->id, Pages_id = Pages->id;
	long Threads_id = 0;
  bool partial_page = (contents_id != 0 && next_page != 0);
  if (next_page == 0)
		openDocument();
  if (contents_id != 0)
		closePage();
	for (int i = 1; i <= next_page; ++i)
	   writePage(i);
	   
	writeFontResources();
	openObj(Pages_id);
  XWPSStream * s = strm;
  s->pputs("<< /Type /Pages /Kids [\n");
  if (partial_page)
		--next_page;
		
	for (int i = 0; i < next_page; ++i)
	  s->print("%ld 0 R\n", pages[i].Page->id);
	  
	s->print("] /Count %d\n", next_page);
  if (params.AutoRotatePages == arp_All)
		s->print("/Rotate %d\n", dominantRotation(&text_rotation));
  Pages->dictElementsWrite(this);
  s->pputs(">>\n");
  endObj();
  if (outlines_id != 0)
  {
  	while (outline_depth > 0)
	    pdfmarkCloseOutline();
		pdfmarkCloseOutline();
		openObj(outlines_id);
		s->print("<< /Count %d", outlines_open);
		s->print(" /First %ld 0 R /Last %ld 0 R >>\n", outline_levels[0].first.id, outline_levels[0].last.id);
		endObj();
  }
  
  if (articles != 0) 
  {
		XWPSPDFArticle *part;
		for (part = articles; part != 0; part = part->next)
	    pdfmarkWriteArticle(part);
  }
  if (Dests)
		Dests->writeObject(this);
		
	if (articles != 0) 
	{
		XWPSPDFArticle *part;

		Threads_id = beginObj();
		s = strm;
		s->pputs("[ ");
		while ((part = articles) != 0) 
		{
	    articles = part->next;
	    s->print("%ld 0 R\n", part->contents->id);
	    delete part;
		}
		s->pputs("]\n");
		endObj();
  }
  
  openObj(Catalog_id);
  s = strm;
  s->pputs("<<");
  s->print("/Type /Catalog /Pages %ld 0 R\n", Pages_id);
  if (outlines_id != 0)
		s->print("/Outlines %ld 0 R\n", outlines_id);
  if (Threads_id)
		s->print("/Threads %ld 0 R\n", Threads_id);
  if (Dests)
	s->print("/Dests %ld 0 R\n", Dests->id);
  Catalog->dictElementsWrite(this);
  s->pputs(">>\n");
  endObj();
  if (Dests) 
  {
		delete Dests;
		Dests = 0;
  }
    
  Catalog->id = 0;
  Pages->id = 0;
  for (int i = 0; i < num_pages; ++i)
	    if (pages[i].Page)
				pages[i].Page->id = 0;
		
	named_objects->dictObjectsWrite(this);
	s = strm;
  long resource_pos = s->tell();
  asides->strm->flush();
  {
		QIODevice *rfile = asides->file;
		long res_end = rfile->pos();
		rfile->seek(0);
		s->copyData(rfile, res_end);
  }
    
  long xrefA = tell();
  if (FirstObjectNumber == 1)
		s->print("xref\n0 %ld\n0000000000 65535 f \n", next_id);
  else
		s->print("xref\n0 1\n0000000000 65535 f \n%ld %ld\n", FirstObjectNumber, next_id - FirstObjectNumber);
  tfile->seek(0);
  {
		for (long i = FirstObjectNumber; i < next_id; ++i) 
		{
	    ulong pos;
	    char str[21];

	    tfile->read((char*)&pos, sizeof(pos));
	    if (pos & ASIDES_BASE_POSITION)
				pos += resource_pos - ASIDES_BASE_POSITION;
	    sprintf(str, "%010ld 00000 n \n", pos);
	    s->pputs(str);
		}
  }
    
  s->pputs("trailer\n");
  s->print("<< /Size %ld /Root %ld 0 R /Info %ld 0 R\n", next_id, Catalog_id, Info_id);
  s->pputs(">>\n");
  s->print("startxref\n%ld\n%%%%EOF\n", xrefA);
  {
		XWPSPDFResource *pres;
		XWPSPDFResource *prev;

		for (prev = last_resource; (pres = prev) != 0;) 
		{
	    prev = pres->prev;
	    delete pres;
		}
	  last_resource = 0;
  }
    
  delete named_objects;
  named_objects = 0;
  delete [] pages;
  pages = 0;
  num_pages = 0;

  {
		int code = closeFileVector();

		return closeFiles(code);
  }
}

int XWPSDevicePDF::copyColorPDF(const uchar * base, 
	                 int sourcex,
		               int raster, 
		               ulong id, 
		               int x, 
		               int y, 
		               int w, 
		               int h)
{
	XWPSImage1 image;
  XWPSPDFImageWriter writer;
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
	    return doImage(writer.pres, NULL, true);
  }
}

int  XWPSDevicePDF::copyDevice(XWPSDevice **pnew)
{
	XWPSDevicePDF * ret = new XWPSDevicePDF;
	ret->copyDeviceParamPDF(this);
	*pnew = ret;
	return 0;
}

void XWPSDevicePDF::copyDeviceParamPDF(XWPSDevicePDF * proto)
{
	copyDeviceParamPSDF(proto);
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
  next_id = proto->next_id;
  outlines_id = proto->outlines_id;
  next_page = proto->next_page;
  contents_id = proto->contents_id;
  context = proto->context;
  contents_length_id = proto->contents_length_id;
  contents_pos = proto->contents_pos;
  procsets = proto->procsets;
  text_rotation = proto->text_rotation;
  num_pages = proto->num_pages;
  outline_depth = proto->outline_depth;
  closed_outline_depth = proto->closed_outline_depth;
  outlines_open = proto->outlines_open;
}

int XWPSDevicePDF::copyMonoPDF(const uchar * base, 
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

int XWPSDevicePDF::createPDFFont(XWPSFont *font, 
	                  XWPSMatrix *pomat,
		                XWPSPDFFont **pppf)
{
	int index = -1;
  XWPSPDFFont ftemp;
  int BaseEncoding = ENCODING_INDEX_UNKNOWN;
  int same = 0, base_same = 0;
  PSPDFFontEmbed embed =fontEmbedStatus(font, &index, &same);
  bool have_widths = false;
  bool is_standard = false;
  long ffid = 0;
  XWPSPDFFontDescriptor *pfd;
  XWPSFont *base_font = font;
  XWPSFont *below;
  XWPSPDFFontDescriptor fdesc;
  XWPSPDFFont *ppf;
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
	
	pfd = (XWPSPDFFontDescriptor *)findResourceByPsId(resourceFontDescriptor, base_font->id);
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
	    	ffid = objRef();
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
					pfd->FontFile_id = ffid;
					if (pfd->base_font != base_font)
					{
						if (pfd->base_font)
						  if (pfd->base_font->decRef() == 0)
							  delete pfd->base_font;
							
						pfd->base_font = base_font; 
					  if (base_font)
						  base_font->incRef();
					}
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
			if (ppf->font != font)
			{
				if (ppf->font)
					if (ppf->font->decRef() == 0)
						delete ppf->font;
				ppf->font = font;
				if (ppf->font)
					ppf->font->incRef();
			}
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
	*pppf = ppf;
  return code;
}

int XWPSDevicePDF::doImage(XWPSPDFResource * pres,
	     				XWPSMatrix * pimat, 
	     				bool in_contents)
{
	if (in_contents) 
	{
		int code = openContents(PDF_IN_STREAM);

		if (code < 0)
	    return code;
  }
  if (pimat) 
  {
		XWPSPDFXObjects * pxo = (XWPSPDFXObjects *)pres;
		double scale = (double)pxo->data_height / pxo->height;

		putImageMatrix(pimat, scale);
  }
  strm->print("/R%ld Do\nQ\n", pres->resourceId());
  return 0;
}

int XWPSDevicePDF::doRectPDF(long x0, long y0, long x1, long y1, PSPathType type)
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
  return doRectPSDF(x0, y0, x1, y1, type);
}

int XWPSDevicePDF::encodeChar(int chr, XWPSFontBase *bfont,	XWPSPDFFont *ppf)
{
	XWPSPDFFontDescriptor * pfd = ppf->FontDescriptor;
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
  
  if (ppf->index == ENCODING_INDEX_UNKNOWN && 
  	  pfd->FontFile_id == 0 &&
			CompatibilityLevel <= 1.2)
	{
		return (int)(XWPSError::Undefined);
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
  
  if (pfd->FontFile_id == 0) 
  {
		switch (bei) 
		{
			case ENCODING_INDEX_STANDARD:
			case ENCODING_INDEX_ISOLATIN1:
			case ENCODING_INDEX_WINANSI:
			case ENCODING_INDEX_MACROMAN:
				
	    	if (!bfont->encodingHasGlyph(glyph, ENCODING_INDEX_ALOGLYPH) &&
						(CompatibilityLevel < 1.3 || !bfont->encodingHasGlyph(glyph, ENCODING_INDEX_ALXGLYPH)))
					return (int)(XWPSError::Undefined);
					
			default:
	    	break;
		}
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
	    gnstr.data = (uchar *)(bfont->glyphName(font_glyph, &gnstr.size));
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

int XWPSDevicePDF::endImageBinary(XWPSPDFImageWriter *piw, int data_h)
{
	long pos = streams->strm->tell();
  int code;

  piw->binary.endBinary();
  code = piw->data->streamAddSince(pos);
  if (code < 0)
		return code;
    
  if (data_h != piw->height)
		code = piw->data->dictPut(piw->pin->Height, data_h);
  return code;
}

int XWPSDevicePDF::endSeparate()
{
	int code = endObj();
  strm = asides->save_strm;
  asides->save_strm = 0;
  return code;
}

int XWPSDevicePDF::endWriteImage(XWPSPDFImageWriter * piw)
{
	XWPSPDFResource *pres = piw->pres;

  if (pres) 
  {
		if (!pres->named) 
	    pres->object->writeObject(this);
		return 0;
  } 
  else 
  {
		XWPSStream *s = strm;

		s->pputs("BI\n");
		piw->data->streamElementsWrite(this);
		s->pputs((binary_ok ? "ID " : "ID\n"));
		piw->data->streamContentsWrite(this);
		s->print("\nEI%s\n", piw->end_string);
		return 1;
  }
}

int XWPSDevicePDF::fillMaskPDF(const uchar * data, 
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

int XWPSDevicePDF::fillPathPDF(XWPSImagerState * pis, 
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
  	if (pdcolor->pureColor() == white && !isInPage())
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
	    s->print("%g i\n", params->flatness);
	    state->flatness = params->flatness;
		}
		if (makePathScaling(ppath, &scaleA)) 
		{
	    smat.makeScaling(scale.x / scaleA, scale.y / scaleA);
	    psmat = &smat;
	    s->pputs("q\n");
		}
		doPath(ppath, (PSPathType)(ps_path_type_fill | ps_path_type_optimize), psmat);
		s->pputs(params->rule < 0 ? "f\n" : "f*\n");
		if (psmat)
	    s->pputs("Q\n");
  }
  return 0;
}

int XWPSDevicePDF::fillRectanglePDF(int x, int y, int  w, int  h, ulong color)
{
	if (color == white && !isInPage())
		return 0;
  
  int code = openPage(PDF_IN_STREAM);
  if (code < 0)
		return code;
		
  putClipPath(NULL);
  setPureColor(color, fill_color, &psdf_set_fill_color_commands);
  strm->print("%d %d %d %d re f\n", x, y, w, h);
  return 0;
}

int XWPSDevicePDF::findOrigFont(XWPSFont *font, XWPSMatrix *pfmat)
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

XWPSPDFResource * XWPSDevicePDF::findResourceByPsId(PSPDFResourceType rtype, ulong rid)
{
	XWPSPDFResource **pchain = resourceChain(rtype, rid);
  XWPSPDFResource **pprev = pchain;
  XWPSPDFResource *pres;

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

int  XWPSDevicePDF::getParamsPDF(XWPSContextState * ctx, XWPSParamList *plist)
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
			(code = plist->writeItems(ctx, this, NULL, (PSParamItem*)&pdf_param_items[0])) < 0);
  return code;
}

void XWPSDevicePDF::initialize()
{
	next_id = FirstObjectNumber;
	PSParamString cstr((const uchar*)("{Catalog}"), 9);
	createNamedDict(&cstr, &Catalog, 0L);
	
	PSParamString dstr((const uchar*)("{DocInfo}"), 9);
	createNamedDict(&dstr, &Info, 0L);
	
	char buf[PDF_MAX_PRODUCER];
	storeDefaultProducer(buf);
	Info->dictPut("/Producer", (uchar *)buf, strlen(buf));
	
	createNamedDict(NULL, &Pages, 0L);
}

int XWPSDevicePDF::openPDF()
{
	int code = xref->openStream();
	if (code < 0)
		return code;
		
	code = asides->openStream();
	if (code < 0)
		return code;
		
	code = streams->openStream();
	if (code < 0)
		return code;
		
	code = pictures->openStream();
	if (code < 0)
		return code;
		
	code = openFile(512);
	if (code < 0)
		return code;
		
	reset();
	scale.x = scale.y = 1.0;
	loadCache();
  fill_options = stroke_options = ps_path_type_optimize;
  in_page = true;
  named_objects = new XWPSCosObject(this, &cos_dict_procs);
  initialize();
  outlines_id = 0;
  next_page = 0;
  pages = new XWPSPDFPage[50];
  num_pages = 50;
  resetPage();
  return 0;
}

long XWPSDevicePDF::openSeparate(long id)
{
	openDocument();
  asides->save_strm = strm;
  strm = asides->strm;
  return openObj(id);
}

int XWPSDevicePDF::outputPagePDF(int num_copies, int flush)
{
	int code = closePage();
  return (code < 0 ? code : finishOutputPage(num_copies, flush));
}

void XWPSDevicePDF::putImageMatrix(XWPSMatrix * pmat, float y_scale)
{
	XWPSMatrix imat;

  imat.matrixTranslate(pmat, 0.0, 1.0 - y_scale);
  imat.matrixScale(&imat, 1.0, y_scale);
  putMatrix("q ", &imat, "cm\n");
}

int XWPSDevicePDF::putParamsPDF(XWPSContextState * ctx, XWPSParamList *plist)
{
	int ecode, code;
  XWPSDevicePDF save_dev;
  float cl = (float)CompatibilityLevel;
  bool locked = params.LockDistillerParams;
  const char * param_name;
  
  {
		PSParamStringArray ppa;

		code = plist->readStringArray(ctx, (param_name = "pdfmark"), &ppa);
		switch (code) 
		{
	    case 0:
				openDocument();
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
				openDocument();
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

  save_dev.copyDeviceParamPDF(this);
  switch (code = plist->readFloat(ctx, (param_name = "CompatibilityLevel"), &cl)) 
  {
		default:
	    ecode = code;
	    plist->signalError(ctx, param_name, ecode);
		case 0:
		case 1:
	    break;
  }
  
  code = plist->readItems(ctx, this, (PSParamItem*)&pdf_param_items[0]);
  if (code < 0)
		ecode = code;
		
	{
		long fon = FirstObjectNumber;

		if (fon != save_dev.FirstObjectNumber) 
		{
	    if (fon <= 0 || fon > 0x7fff0000 ||
					(next_id != 0 && next_id !=	save_dev.FirstObjectNumber + pdf_num_initial_ids)) 
			{
				ecode = XWPSError::RangeCheck;
				plist->signalError(ctx, "FirstObjectNumber", ecode);
	    }
		}
	}
	
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

		if (is_open)
	    close();
		setResolution(HWResolution[0] / factor, HWResolution[1] / factor);
  }
#undef MAX_EXTENT
  
  if (FirstObjectNumber != save_dev.FirstObjectNumber) 
  {
		if (xref->file != 0) 
		{
	    xref->file->seek(0L);
	    initialize();
		}
  }
  
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

int XWPSDevicePDF::setFillColorPDF(XWPSDeviceColor * pdc)
{
	return setDrawingColor(pdc, fill_color, &psdf_set_fill_color_commands);
}

int XWPSDevicePDF::setLineWidthPDF(double widthA)
{
	setLineWidthPSDF(fabs(widthA));return 0;
}

int XWPSDevicePDF::setStrokeColorPDF(XWPSDeviceColor * pdc)
{
	return setDrawingColor(pdc, stroke_color, &psdf_set_stroke_color_commands);
}

int XWPSDevicePDF::setTextMatrix(XWPSMatrix * pmat)
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
		    	char spstr[3 + 14 + 1];
		    	XWPSStream *s;

		    	sprintf(spstr, "%d 0 0 0 0 0 d1\n", dx);
		    	space_char_ids[dx_i] = beginSeparate();
		    	s = strm;
		    	s->print("<</Length %d>>\nstream\n", (int)(strlen(spstr)));
		    	s->print("%sendstream\n", spstr);
		    	endSeparate();
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
				s->print("%g TL\n", dist_y);
				text.leading = dist_y;
	    }
	    text.use_leading = true;
		} 
		else 
		{
	    dist.setTextDistance(&text.line_start, pmat);
	    s->print("%g %g Td\n", dist.x, dist.y);
	    text.use_leading = false;
		}
  } 
  else 
  {
		code = openPage(PDF_IN_TEXT);
		if (code < 0)
	    return code;
	    
		strm->print("%g %g %g %g %g %g Tm\n", pmat->xx * sx, pmat->xy * sy,
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

int XWPSDevicePDF::stripTileRectanglePDF(XWPSStripBitmap * tiles,
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
  int (XWPSDevicePDF::*copy_data)(const uchar *, int, int, ulong, int, int, int, int, XWPSImage1 *, XWPSPDFImageWriter *, int);
  XWPSPDFResource *pres;
  XWPSCosValue cs_value;
  int code;

  if (tiles->id == ps_no_bitmap_id || tiles->shift != 0 ||
			(w < tw && h < th) ||	color0 != ps_no_color_index ||
			CompatibilityLevel < 1.2)
		goto use_default;
  
  if (color1 != ps_no_color_index) 
  {
		mask = true;
		depth = 1;
		copy_data = &XWPSDevicePDF::copyMaskData;
		code = csPatternUnColored(&cs_value);
  } 
  else 
  {
		mask = false;
		depth = color_info.depth;
		copy_data = &XWPSDevicePDF::copyColorData;
		code = csPatternColored(&cs_value);
  }
  if (code < 0)
		goto use_default;
  pres = findResourceByPsId(resourcePattern, tiles->id);
  if (!pres) 
  {
		int code;
		long image_id, length_id, start, end;
		XWPSStream *s;
		XWPSImage1 image;
		XWPSPDFImageWriter writer;
		long image_bytes = ((long)tw * depth + 7) / 8 * th;
		bool in_line = image_bytes <= MAX_INLINE_IMAGE_BYTES;
		ulong tile_id =  (tw == tiles->size.x && th == tiles->size.y ? tiles->id : ps_no_bitmap_id);

		if (in_line)
	    image_id = 0;
		else if (image_bytes > 65500) 
		{
	    goto use_default;
		} 
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
		s = strm;
		s->print("/Type/Pattern/PatternType 1/PaintType %d/TilingType 1/Resources<<\n", (mask ? 2 : 1));
		if (image_id)
	    s->print("/XObject<</R%ld %ld 0 R>>", image_id, image_id);
		s->print("/ProcSet[/PDF/Image%s]>>\n", (mask ? "B" : "C"));
		s->print("/Matrix[%g 0 0 %g 0 0]", tw / xscale, th / yscale);
		s->pputs("/BBox[0 0 1 1]/XStep 1/YStep 1/Length ");
		if (image_id) 
		{
	    char buf[MAX_REF_CHARS + 6 + 1]; 

	    sprintf(buf, "/R%ld Do\n", image_id);
	    s->print("%d>>stream\n", (int)(strlen(buf)));
	    s->print("%sendstream\n", buf);
	    endResource();
		} 
		else 
		{
	    length_id = objRef();
	    s->print("%ld 0 R>>stream\n", length_id);
	    start = tell();
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
	    end = tell();
	    s->pputs("\nendstream\n");
	    endResource();
	    openSeparate(length_id);
	    strm->print("%ld\n", end - start);
	    endSeparate();
		}
		pres->object->written = true; 
  }
  
  {
		int code = openPage(PDF_IN_STREAM);
		XWPSStream *s;

		if (code < 0)
	    goto use_default;
	    
		putClipPath(NULL);
		s = strm;
		
		s->print("q %g 0 0 %g 0 0 cm\n", xscale, yscale);
		cs_value.write(this);
		s->pputs(" cs");
		if (mask)
	    s->print(" %d %d %d", (int)(color1 >> 16), (int)((color1 >> 8) & 0xff), (int)(color1 & 0xff));
		s->print("/R%ld scn", pres->resourceId());
		s->print(" %g %g %g %g re f Q\n",	 x / xscale, y / yscale, w / xscale, h / xscale);
  }
  return 0;
  
use_default:
  return stripTileRectangleDefault(tiles, x, y, w, h, color0, color1, px, py);
}

int XWPSDevicePDF::strokePathPDF(XWPSImagerState * pis,
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

  if (set_ctm)
		putMatrix("q ", &mat, "cm\n");
    code = doPath(ppath, (PSPathType)(ps_path_type_stroke | ps_path_type_optimize),  (set_ctm ? &mat : 0));
  if (code < 0)
		return code;
  s = strm;
  s->pputs((code ? "s" : "S"));
  s->pputs((set_ctm ? " Q\n" : "\n"));
  return 0;
}

int XWPSDevicePDF::textBeginPDF(XWPSImagerState * pis,
		               XWPSTextParams *text, 
		               XWPSFont * font,
		               XWPSPath * path, 
		               XWPSDeviceColor * pdcolor,
		               XWPSClipPath * pcpath,
		               XWPSTextEnum ** ppte)
{
	XWPSPDFTextEnum *penum;
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
		currentPage()->text_rotation.counts[i] += text->size;
  }
  
  code = prepareFill(pis);
  if (code < 0)
		return code;
	
  if ((text->operation & ~(TEXT_FROM_STRING | TEXT_FROM_BYTES |
	   				TEXT_ADD_TO_ALL_WIDTHS | TEXT_ADD_TO_SPACE_WIDTH |
	   				TEXT_REPLACE_WIDTHS | TEXT_DO_DRAW | TEXT_RETURN_WIDTH)) != 0 ||
						path->currentPoint(&cpt) < 0 ||
						font->PaintType != 0)
	{
		return textBeginDefault(pis, text, font, path, pdcolor, pcpath, ppte);
	}

  if (mustPutClipPath(pcpath)) 
  {
		int code = openPage(PDF_IN_STREAM);

		if (code < 0)
	    return code;
		putClipPath(pcpath);
  }
  if (setDrawingColor(pdcolor, fill_color,  &psdf_set_fill_color_commands) < 0)
		return textBeginDefault(pis, text, font, path, pdcolor,  pcpath, ppte);

  penum = new XWPSPDFTextEnum;
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

int XWPSDevicePDF::writeTextProcessState(XWPSPDFTextProcessState *ppts, XWPSString *pstr)
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
		s->print("%g Tc\n", ppts->chars);
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
		s->print("%g Tw\n", ppts->words);
		text.word_spacing = ppts->words;
  }

  return 0;
}

void XWPSDevicePDF::writeValue(const uchar * vstr, uint size)
{
	if (size > 0 && vstr[0] == '/')
		putName(vstr + 1, size - 1);
  else
		strm->write(vstr, size);
}

int  XWPSDevicePDF::addEncodingDifference(XWPSPDFFont *ppf, 
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

int  XWPSDevicePDF::adjustFontName(XWPSPDFFontDescriptor *pfd, bool is_standard)
{
	int code = 0;
	if (MAKE_FONT_NAMES_UNIQUE)
	{
		int j = 0;
		XWPSPDFFontDescriptor *old;
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
				for (old = (XWPSPDFFontDescriptor *)(resources[resourceFontDescriptor].chains[j]); old != 0; old = (XWPSPDFFontDescriptor *)(old->next))
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

	    sprintf(suffix, "%c%lx", SUFFIX_CHAR, pfd->resourceId());
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

int  XWPSDevicePDF::allocAside(XWPSPDFResource ** plist,
		                           XWPSPDFResource **ppres,
		                           int rtype,
		                           long id)
{
	XWPSPDFResource *pres = 0;
  XWPSCosObject *object;

	switch (rtype)
	{
		case resourceFontDescriptor:
			pres = new XWPSPDFFontDescriptor;
			break;
			
		case resourceCharProc:
			pres = new XWPSPDFCharProc;
			break;
			
		case resourceFont:
			pres = new XWPSPDFFont;
			break;
			
		case resourceXObject:
			pres = new XWPSPDFXObjects;
			break;
			
		default:
			pres = new XWPSPDFResource;
			break;
	}
	
  object = new XWPSCosObject(this);
  object->id = (id < 0 ? -1L : id == 0 ? objRef() : id);
  pres->next = *plist;
  *plist = pres;
  pres->prev = last_resource;
  last_resource = pres;
  pres->named = false;
  pres->used_on_page = true;
  pres->object = object;
  *ppres = pres;
  return 0;
}

int  XWPSDevicePDF::allocFont(ulong rid, 
	               XWPSPDFFont **ppfres,
	       				XWPSPDFFontDescriptor *pfd_in)
{
	XWPSPDFFontDescriptor *pfd = 0;
  XWPSString chars_used, glyphs_used;
  int code;
  XWPSPDFFont *pfres;

  if (pfd_in != 0)
  {
  	code = allocResource(resourceFontDescriptor, pfd_in->rid, (XWPSPDFResource **)&pfd, 0L);
		if (code < 0)
	    return code;
		chars_used.size = pfd_in->chars_used.size;
		chars_used.data = new uchar[chars_used.size];
		
		memset(chars_used.data, 0, chars_used.size);
		pfd->values = pfd_in->values;
		pfd->chars_used = chars_used;
		pfd->subset_ok = true;
  }
  
  code = allocResource(resourceFont, rid, (XWPSPDFResource**)ppfres, 0L);
  pfres = *ppfres;
  sprintf(pfres->frname, "R%ld", pfres->object->id);
  pfres->index = -1;
  pfres->BaseEncoding = ENCODING_INDEX_UNKNOWN;
  pfres->FontDescriptor = pfd;
  return 0;
}

int  XWPSDevicePDF::allocResource(int rtype,
	                                ulong rid,
		                              XWPSPDFResource ** ppres, 
		                              long id)
{
	int code = allocAside(resourceChain((PSPDFResourceType)rtype, rid), ppres, rtype, id);
  if (code >= 0)
		(*ppres)->rid = rid;
  return code;
}

int  XWPSDevicePDF::arrayAddInt2(XWPSCosObject *pca, int lower, int upper)
{
	int code = pca->arrayAdd(lower);

  if (code >= 0)
		code = pca->arrayAdd(upper);
  return code;
}

int  XWPSDevicePDF::assignCharCode()
{
	XWPSPDFFont *font = open_font;
  if (embedded_encoding_id == 0)
  {
  	long id = beginSeparate();
		XWPSStream *s = strm;
		int i;
		
		s->pputs("<</Type/Encoding/Differences[0");
		for (i = 0; i < 256; ++i) 
		{
	    if (!(i & 15))
				s->pputs("\n");
	    s->print("/a%d", i);
		}
		s->pputs("\n] >>\n");
		endSeparate();
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
  }
  return font->num_chars++;
}

int  XWPSDevicePDF::beginAside(XWPSPDFResource ** plist, XWPSPDFResource ** ppres,int rtype)
{
	long id = beginSeparate();

  if (id < 0)
		return (int)id;
  
  return allocAside(plist, ppres, rtype, id);
}

int  XWPSDevicePDF::beginCharProc(int w, 
	                   int h, 
	                   int x_width,
                     int y_offset, 
                     ulong id, 
                     XWPSPDFCharProc ** ppcp, 
                     PSPDFStreamPosition * ppos)
{
	XWPSPDFResource *pres;
  XWPSPDFCharProc *pcp;
  int char_code = assignCharCode();
  XWPSPDFFont *font = open_font;
  int code;

  if (char_code < 0)
		return char_code;
    
  code = beginResource(resourceCharProc, id, &pres);
  if (code < 0)
		return code;
  pcp = (XWPSPDFCharProc *) pres;
  pcp->font = font;
  pcp->char_next = font->char_procs;
  font->char_procs = pcp;
  pcp->char_code = char_code;
  pcp->width = w;
  pcp->height = h;
  pcp->x_width = x_width;
  pcp->y_offset = y_offset;
  font->max_y_offset = qMax(font->max_y_offset, h + (h >> 2));
  *ppcp = pcp;
  {
		XWPSStream *s = strm;
		s->pputs("<</Length     >>\nstream\n");
		ppos->start_pos = s->tell();
  }
  return 0;
}

int  XWPSDevicePDF::beginData(XWPSPDFDataWriter *pdw)
{
	long length_id = objRef();
  XWPSStream *s = strm;
#define USE_ASCII85 1
#define USE_FLATE 2
  static const char *const fnames[4] = {
		"", "/Filter/ASCII85Decode", "/Filter/FlateDecode",
		"/Filter[/ASCII85Decode/FlateDecode]" };
  int filters = 0;
  int code;

  if (!binary_ok)
		filters |= USE_ASCII85;
  if (CompatibilityLevel >= 1.2)
		filters |= USE_FLATE;
		
	s->pputs(fnames[filters]);
  s->print("/Length %ld 0 R>>stream\n", length_id);
  code = pdw->binary.beginBinary(this);
  if (code < 0)
		return code;
  pdw->start = s->tell();
  pdw->length_id = length_id;
  if (filters & USE_FLATE)
		code = flateBinary(&pdw->binary);
  return code;
#undef USE_ASCII85
#undef USE_FLATE
}

int  XWPSDevicePDF::beginFontFile(long FontFile_id,
		   								            long *plength_id, 
		   								            const char *entries,
		                              long len, 
		                              long *pstart, 
		                              XWPSPSDFBinaryWriter *pbw)
{
  openSeparate( FontFile_id);
  *plength_id = objRef();
  XWPSStream *s = strm;
  s->print("<</Length %ld 0 R", *plength_id);
  if (!binary_ok)
		s->pputs("/Filter/ASCII85Decode");
  if (entries)
		strm->pputs(entries);
  strm->print("/Length1 %ld>>stream\n", len);
  *pstart = tell();
  return pbw->beginBinary(this);
}

int  XWPSDevicePDF::beginImageData(XWPSPDFImageWriter * piw,
		     							XWPSPixelImage * pim, 
		     							XWPSCosValue *pcsvalue)
{
	XWPSCosObject *pcd = piw->data;
  int code = putImageValues(pcd, pim, piw->pin, pcsvalue);

  if (code >= 0)
		code = putImageFilters(pcd, &piw->binary, piw->pin);
  if (code < 0) 
  {
		if (!piw->pres)
	    delete piw->data;
		piw->data = 0;
  }
  return code;
}

long XWPSDevicePDF::beginObj()
{
	return openObj(0L);
}

int  XWPSDevicePDF::beginResource(PSPDFResourceType rtype, 
	                   ulong rid,
		                 XWPSPDFResource ** ppres)
{
	int code = beginResourceBody(rtype, rid, ppres);

  if (code >= 0 && resource_names[rtype] != 0) 
  {
		XWPSStream *s = strm;

		s->print("<</Type/%s", resource_names[rtype]);
		s->print("/Name/R%ld", (*ppres)->object->id);
  }
  return code;
}

int  XWPSDevicePDF::beginResourceBody(PSPDFResourceType rtype,	
	                       ulong rid, 
	                       XWPSPDFResource ** ppres)
{
	int code = beginAside(resourceChain(rtype, rid), ppres, rtype);

  if (code >= 0)
		(*ppres)->rid = rid;
  return code;
}

long XWPSDevicePDF::beginSeparate()
{
	return openSeparate(0L);
}

int  XWPSDevicePDF::beginWriteImage(XWPSPDFImageWriter * piw,
		      						 ulong id, 
		      						 int w, 
		      						 int h, 
		      						 XWPSPDFResource *pres,
		                   bool in_line)
{
	XWPSStream *save_strm = strm;
  int code;

  if (in_line) 
  {
		piw->pres = 0;
		piw->pin = &pdf_image_names_short;
		piw->data = new XWPSCosObject(this, &cos_stream_procs);
		piw->end_string = " Q";
  } 
  else 
  {
		XWPSPDFXObjects *pxo;
		XWPSCosObject *pcos;

		if (pres == 0) 
		{
	    code = allocResource(resourceXObject, id, &piw->pres, 0L);
	    if (code < 0)
				return code;
	    piw->pres->object->become(&cos_stream_procs);
		} 
		else 
		{
	    piw->pres = pres;
		}
		piw->pres->rid = id;
		piw->pin = &pdf_image_names_full;
		pxo = (XWPSPDFXObjects*)piw->pres;
		pcos = pxo->object;
		code = pcos->dictPut("/Subtype", "/Image");
		pxo->width = w;
		pxo->height = h;
		pxo->data_height = h;
		piw->data = pcos;
  }
  piw->height = h;
  strm = streams->strm;
  code = piw->binary.beginBinary(this);
  strm = save_strm;
  return code;
}

bool XWPSDevicePDF::canWriteImageInLine(XWPSImage1 *pim)
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

int XWPSDevicePDF::closeContents(bool last)
{
	if (context == PDF_IN_NONE)
		return 0;
  
  if (last) 
  {
		openContents(PDF_IN_STREAM);
		strm->pputs("Q\n");
		text.font = 0;
  }
  return openContents(PDF_IN_NONE);
}

int XWPSDevicePDF::closeFiles(int code)
{
	if (xref)
	{
		delete xref;
		xref = 0;
	}
	
	if (asides)
	{
		delete asides;
		asides = 0;
	}
	
	if (streams)
	{
		delete streams;
		streams = 0;
	}
	
	if (pictures)
	{
		delete pictures;
		pictures = 0;
	}
	
	return code;
}

int XWPSDevicePDF::closePage()
{
	int page_num = ++(next_page);
	openDocument();
  closeContents(true);
  pageId(page_num);
  XWPSPDFPage * page = &pages[page_num - 1];
  page->MediaBox.x = (int)(MediaSize[0]);
  page->MediaBox.y = (int)(MediaSize[1]);
  page->procsets = procsets;
  page->contents_id = contents_id;
  for (int i = 0; i < resourceFont; i++)
  {
  	bool any = false;
	  XWPSStream *s = 0;
	  for (int j = 0; j < NUM_RESOURCE_CHAINS; ++j)
	  {
	  	XWPSPDFResource *pres = resources[i].chains[j];
	  	for (; pres != 0; pres = pres->next)
	  	{
	  		if (pres->used_on_page)
	  		{
	  			long id = pres->object->id;

					if (!any) 
					{
			    	page->resource_ids[i] = beginObj();
			    	s = strm;
			    	s->pputs("<<");
			    	any = true;
					}
					s->print("/R%ld\n%ld 0 R", id, id);
	  		}
	  	}
	  }
	  
	  if (any) 
	  {
			s->pputs(">>\n");
			endObj();
			writeResourceObjects((PSPDFResourceType)i);
	  }
  }
  
  writeResourceObjects(resourceFunction);
  {
  	bool any = false;
		XWPSStream *s = 0;
		for (int j = 0; j < NUM_RESOURCE_CHAINS; ++j) 
		{
	    XWPSPDFResource **prev = &resources[resourceFont].chains[j];
	    XWPSPDFFont *font;
	    while ((font = (XWPSPDFFont*)(*prev)) != 0) 
	    {
				if (font->used_on_page) 
				{
		    	if (!any) 
		    	{
						page->fonts_id = beginObj();
						s = strm;
						s->pputs("<<");
						any = true;
		    	}
		    	
		    	s->print("/%s", font->frname);
		    	s->print("\n%ld 0 R", font->object->id);
		    	font->used_on_page = false;
				}
				if (font->skip) 
				{
		    	*prev = font->next;
				} 
				else
		    	prev = &font->next;
	    }
		}
		if (any) 
		{
	    s->pputs(">>\n");
	    endObj();
		}
  }
  
  if (CompatibilityLevel <= 1.2)
		use_open_font = false;
		
	page->text_rotation.Rotate =	(params.AutoRotatePages == arp_PageByPage ? dominantRotation(&page->text_rotation) : -1);
	for (int i = 0; i < (sizeof(page->text_rotation.counts) / sizeof(page->text_rotation.counts[0])); ++i)
	    text_rotation.counts[i] += page->text_rotation.counts[i];
	    
	resetPage();
  return 0;
}

int XWPSDevicePDF::colorSpace(XWPSCosValue *pvalue,
								 XWPSColorSpace *pcs,
									const PSPDFColorSpaceNames *pcsn,
									bool by_name)
{
	PSColorSpaceIndex csi = pcs->getIndex();
  XWPSCosObject *pca;
  XWPSCosObject *pcd;
  XWPSCie *pciec;
  XWPSFunction *pfn;
  int code;
  
  switch (csi) 
  {
    case ps_color_space_index_DeviceGray:
			pvalue->stringValue(pcsn->DeviceGray);
			return 0;
			
    case ps_color_space_index_DeviceRGB:
			pvalue->stringValue(pcsn->DeviceRGB);
			return 0;
			
    case ps_color_space_index_DeviceCMYK:
			pvalue->stringValue(pcsn->DeviceCMYK);
			return 0;
			
    case ps_color_space_index_Pattern:
			if (!pcs->params.pattern && pcs->params.pattern->has_base_space) 
			{
	    	pvalue->stringValue("/Pattern");
	    	return 0;
			}
			break;
			
    default:
			break;
  }
  
  pca = new XWPSCosObject(this, &cos_array_procs);
  switch (csi)
  {
  	case ps_color_space_index_CIEA:
  		{
  			XWPSCieA * pcie = pcs->params.a;
				PSVector3 expts;
				if (!(pcie->MatrixA.u == 1 && pcie->MatrixA.v == 1 &&
	      		pcie->MatrixA.w == 1 &&
	      		pcie->common.MatrixLMN.is_identity))
	    		return (int)(XWPSError::RangeCheck);
	    			
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
	    		return (int)(XWPSError::RangeCheck);
	    	
	    	{
	    		XWPSCosValue tmpv("/CalGray");
	    		code = pca->arrayAdd(&tmpv);
	    	}
	    	if (code < 0)
	    		return code;
	    		
	    	pcd = new XWPSCosObject(this, &cos_dict_procs);
	    	if (expts.u != 1) 
	    	{
	    		code = pcd->dictPut("/Gamma", expts.u);
	    		if (code < 0)
						return code;
				}
				pciec = pcie;
  		}
  		
cal:
    	code = cos_dict_put_c_key_vector3(pcd, "/WhitePoint", &pciec->common.points.WhitePoint);
    	if (code < 0)
				return code;
			if (pciec->common.points.BlackPoint.u != 0 ||
					pciec->common.points.BlackPoint.v != 0 ||
					pciec->common.points.BlackPoint.w != 0) 
			{
				code = cos_dict_put_c_key_vector3(pcd, "/BlackPoint", &pciec->common.points.BlackPoint);
				if (code < 0)
	    		return code;
    	}
    	{
    		XWPSCosValue tmpv(pcd);
    		code = pca->arrayAdd(&tmpv);
    	}    	
  		break;
  		
  	case ps_color_space_index_CIEABC:
  		{
  			XWPSCieABC *pcie = pcs->params.abc;
				PSVector3 expts;
				PSMatrix3 *pmat;
				
				if (pcie->common.MatrixLMN.is_identity &&
	    			pcie->cache3IsIdentity() &&
	    			pcie->vector3CacheIsExponential(&expts))
	    			pmat = &pcie->MatrixABC;
				else if (pcie->MatrixABC.is_identity &&
		 						 pcie->cache3IsIdentityABC() &&
		 						 pcie->scalar3CacheIsExponential(&expts))
	    		pmat = &pcie->common.MatrixLMN;
				else
	    		return (int)(XWPSError::RangeCheck);
	    	
	    	{
	    		XWPSCosValue tmpv("/CalRGB");
					code = pca->arrayAdd(&tmpv);
				}
				if (code < 0)
	    		return code;
	    	pcd = new XWPSCosObject(this, &cos_dict_procs);
	    	if (expts.u != 1 || expts.v != 1 || expts.w != 1) 
	    	{
	    		code = cos_dict_put_c_key_vector3(pcd, "/Gamma", &expts);
	    		if (code < 0)
						return code;
  			}
  			
  			if (!pmat->is_identity) 
  			{
	    		XWPSCosObject *pcma = new XWPSCosObject(this, &cos_array_procs);
	    		XWPSCosValue tmpv(pcma);
	    		if ((code = cos_array_add_vector3(pcma, &pmat->cu)) < 0 ||
							(code = cos_array_add_vector3(pcma, &pmat->cv)) < 0 ||
							(code = cos_array_add_vector3(pcma, &pmat->cw)) < 0 ||
							(code = pcd->dictPut((const uchar *)"/Matrix", 7,	&tmpv)) < 0)
						return code;
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
				XWPSCosValue tmpv(pdf_color_space_names.Indexed);
				XWPSCosValue tmpvv(table, string_used);
				free(table);
				if ((code = colorSpace(pvalue, base_space,  &pdf_color_space_names, false)) < 0 ||
	    			(code = pca->arrayAdd(&tmpv)) < 0 ||
	    			(code = pca->arrayAdd(pvalue)) < 0 ||
	    			(code = pca->arrayAdd(pip->hival)) < 0 ||
	    			(code = pca->arrayAdd(&tmpvv)) < 0)
	    		return code;
    	}
    	break;
    	
    case ps_color_space_index_DeviceN:
			pfn = pcs->getDevNFunction();
			if (pfn == 0)
	    	return (int)(XWPSError::RangeCheck);
	    {
	    	XWPSCosObject *psna = new XWPSCosObject(this, &cos_array_procs);
	    	for (int i = 0; i < pcs->params.device_n->num_components; ++i) 
	    	{
	    		XWPSCosValue v;
					code = separationName(&v, pcs->params.device_n->names[i]);
					if (code < 0 || (code = psna->arrayAdd(&v)) < 0)
		    		return code;
	    	}
	    	
	    	XWPSCosValue tmpv(psna);
	    	if ((code = separationColorSpace(pca, "/DeviceN", &tmpv, pcs->params.device_n->alt_space,	pfn, &pdf_color_space_names)) < 0)
					return code;
	    }
	    break;
	    
	  case ps_color_space_index_Separation:
			pfn = pcs->getSeprFunction();
			if (pfn == 0)
	    	return (int)(XWPSError::RangeCheck);
	    {
	    	XWPSCosValue v;
				if ((code = separationName(&v, pcs->params.separation->sname)) < 0 ||
	    			(code = separationColorSpace(pca, "/Separation", &v, pcs->params.separation->alt_space,	pfn, &pdf_color_space_names)) < 0)
	    	return code;
	    }
			break;
			
		case ps_color_space_index_Pattern:
			{
				XWPSCosValue v("/Pattern");
				if ((code = colorSpace(pvalue, pcs->params.pattern->base_space, &pdf_color_space_names, false)) < 0 ||
	    			(code = pca->arrayAdd(&v)) < 0 ||
	    			(code = pca->arrayAdd(pvalue)) < 0)
	    	return code;
	    }
			break;
			
    default:
			return (int)(XWPSError::RangeCheck);
  }
  
  {
		XWPSPDFResource *pres;

		code = allocResource(resourceColorSpace, ps_no_id, &pres, 0L);
		if (code < 0) 
		{
			delete pca;
	    return code;
		}
		pca->id = pres->object->id;
		if (pres->object)
			if (pres->object->decRef() == 0)
			  delete pres->object;
		pres->object = pca;
		if (pca)
			pca->incRef();
		pca->writeObject(this);
  }
  if (by_name) 
  {
  	pvalue->resourceValue(pca);
  } 
  else
		pvalue->objectValue(pca);
  return 0;
}

void XWPSDevicePDF::colorSpaceProcsets(XWPSColorSpace *pcs)
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

int XWPSDevicePDF::computeFontDescriptor(XWPSPDFFontDescriptor *pfd,
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

int XWPSDevicePDF::copyColorBits(XWPSStream *s, 
	                 const uchar *base, 
	                 int sourcex, 
	                 int raster,
		               int w, 
		               int h, 
		               int bytes_per_pixel)
{
	int yi;

  for (yi = 0; yi < h; ++yi) 
  {
		uint ignore;

		s->puts(base + sourcex * bytes_per_pixel + yi * raster,  w * bytes_per_pixel, &ignore);
  }
  return 0;
}

int XWPSDevicePDF::copyMaskBits(XWPSStream *s, 
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

int XWPSDevicePDF::copyMaskData(const uchar * base, 
	                 int sourcex,
		               int raster, 
		               ulong id, 
		               int x, 
		               int y, 
		               int w,
		               int h,
		   						 XWPSImage1 *pim, 
		   						 XWPSPDFImageWriter *piw,
		   						 int for_pattern)
{
	ulong nbytes;
  int code;
 	const uchar *row_base;
  int row_step;
  long pos;
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
  
  if (for_pattern < 0)
		strm->pputs("q ");
  if ((code = beginWriteImage(piw, id, w, h, NULL, in_line)) < 0 ||
			(code = piw->binary.setupLosslessFilters(this, pim)) < 0 ||
			(code = beginImageData(piw, pim,   NULL)) < 0)
		return code;
  pos = streams->strm->tell();
  copyMaskBits(piw->binary.strm, row_base, sourcex, row_step, w, h, 0);
  piw->data->streamAddSince(pos);
  endImageBinary(piw, piw->height);
  return endWriteImage(piw);
}

int XWPSDevicePDF::copyColorData(const uchar * base, 
	                  int sourcex,
		                int raster, 
		                ulong id, 
		                int x, 
		                int y, 
		                int w, 
		                int h,
		                XWPSImage1 *pim, 
		                XWPSPDFImageWriter *piw,
		                int for_pattern)
{
	int depth = color_info.depth;
  int bytes_per_pixel = depth >> 3;
  XWPSColorSpace cs;
  cs.i_ctx_p = context_state;
  XWPSCosValue cs_value;
  ulong nbytes;
  int code = cs.initDevice(bytes_per_pixel);
  const uchar *row_base;
  int row_step;
  long pos;
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
  
  if (for_pattern < 0)
		strm->pputs("q ");
  if ((code = beginWriteImage(piw, id, w, h, NULL, in_line)) < 0 ||
			(code = colorSpace(&cs_value, &cs, &piw->pin->color_spaces, in_line)) < 0 ||
			(code = piw->binary.setupLosslessFilters(this, pim)) < 0 ||
			(code = beginImageData(piw, pim, &cs_value)) < 0)
		return code;
  pos = streams->strm->tell();
  copyColorBits(piw->binary.strm, row_base, sourcex, row_step, w, h,bytes_per_pixel);
  piw->data->streamAddSince(pos);
  endImageBinary(piw, piw->height);
  return endWriteImage(piw);
}

int XWPSDevicePDF::copyMono(const uchar *base, 
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
  XWPSCosValue cs_value;
  XWPSCosValue *pcsvalue;
  uchar palette[sizeof(ulong) * 2];
  XWPSImage1 image;
  XWPSPDFImageWriter writer;
  PSPDFStreamPosition ipos;
  XWPSPDFResource *pres = 0;
  uchar invert = 0;
  bool in_line = false;
  long pos;
  
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
				XWPSPDFCharProc *pcp;
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
				strm->print("0 0 0 %d %d %d d1\n", y_offset, w, h + y_offset);
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
	    return code;
		pcsvalue = &cs_value;
  }
  
  if (pres) 
		writer.binary.CFEBinary(image.Width, image.Height, false);
  else 
		writer.binary.setupImageFilters(this, &image, NULL, NULL);
  beginImageData(&writer, &image, pcsvalue);
  pos = streams->strm->tell();
  code = copyMaskBits(writer.binary.strm, base, sourcex, raster, w, h, invert);
  if (code < 0)
		return code;
  code = writer.data->streamAddSince(pos);
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
				return doImage(writer.pres, &image.ImageMatrix, true);
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
  code = endCharProc(&ipos);
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
		return doCharImage((XWPSPDFCharProc *)pres, &imat);
  }
}

int XWPSDevicePDF::cosStreamPutCStrings(XWPSCosObject *pcs, const char *key, const char *value)
{
	return pcs->dictPut(key, value);
}

int XWPSDevicePDF::createNamed(PSParamString *pname,
		 							             PSCosObjectProcs * cotype, 
		 							             XWPSCosObject **ppco, 
		 							             long id)
{
	XWPSCosObject *pco = new XWPSCosObject(this, cotype);  
  if (pco)
  {
  	pco->id = (id == -1 ? 0L : id == 0 ? objRef() : id);
  	if (pname) 
  	{
  		XWPSCosValue value(pco);
			int code = named_objects->dictPut(pname->data, pname->size, &value);
			if (code < 0)
	    	return code;
    }
  }
  
  *ppco = pco;
  return 0;
}

int XWPSDevicePDF::createNamedDict(PSParamString *pname, XWPSCosObject **ppco, long id)
{
	XWPSCosObject *pco;
  int code = createNamed(pname, &cos_dict_procs, &pco, id);
  *ppco = pco;
  return code;
}

int XWPSDevicePDF::csPatternColored(XWPSCosValue *pvalue)
{
	return patternSpace(pvalue, &cs_Patterns[0], "[/Pattern]");
}

int XWPSDevicePDF::csPatternUnColored(XWPSCosValue *pvalue)
{
	int ncomp = color_info.num_components;
    static const char *const pcs_names[5] = {
			0, "[/Pattern /DeviceGray]", 0, "[/Pattern /DeviceRGB]", "[/Pattern /DeviceCMYK]"    };

  return patternSpace(pvalue, &cs_Patterns[ncomp], pcs_names[ncomp]);
}

XWPSPDFPage * XWPSDevicePDF::currentPage()
{
	return &pages[next_page];
}

XWPSCosObject * XWPSDevicePDF::currentPageDict()
{
	if (pageId(next_page + 1) <= 0)
		return 0;
  return pages[next_page].Page;
}

int XWPSDevicePDF::doCharImage(XWPSPDFCharProc * pcp, XWPSMatrix * pimat)
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

int XWPSDevicePDF::dominantRotation(PSPDFTextRotation * ptr)
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

int XWPSDevicePDF::embedFontAsType1(XWPSFontType1 *font,	
	                                  long FontFile_id, 
	                                  ulong * subset_glyphs,
			                              uint subset_size, 
			                              XWPSString *pfname)
{
	XWPSStream poss;
	int lengths[3];
  int code;
  long length_id;
  long start;
  XWPSPSDFBinaryWriter writer;
  
  poss.writePositionOnly();
  
#define MAX_INT_CHARS ((sizeof(int) * 8 + 2) / 3)
    char lengths_str[9 + MAX_INT_CHARS + 11];
#undef MAX_INT_CHARS

#define TYPE1_OPTIONS (WRITE_TYPE1_EEXEC | WRITE_TYPE1_EEXEC_MARK |\
		       WRITE_TYPE1_WITH_LENIV)
		       
  code = font->writeType1Font(&poss,TYPE1_OPTIONS, subset_glyphs, subset_size, pfname, lengths);
  if (code < 0)
		return code;
		
	sprintf(lengths_str, "/Length2 %d/Length3 0", lengths[1]);
  code = beginFontFile(FontFile_id, &length_id, lengths_str, lengths[0], &start, &writer);
  if (code < 0)
		return code;
  font->writeType1Font(writer.strm, TYPE1_OPTIONS, subset_glyphs, subset_size, pfname, lengths);
#undef TYPE1_OPTIONS
  endFontFile(start, length_id, &writer);
  return 0;
}

int XWPSDevicePDF::embedFontAsType2(XWPSFontType1 *font,
											              long FontFile_id, 
											              ulong * subset_glyphs,
											              uint subset_size, 
											              XWPSString *pfname)
{
	XWPSStream poss;
	int code;
  long length_id;
  long start;
  XWPSPSDFBinaryWriter writer;
  int options = TYPE2_OPTIONS |	(CompatibilityLevel < 1.3 ? WRITE_TYPE2_AR3 : 0);

	poss.writePositionOnly();
  code = font->writeType2Font(&poss, options,	subset_glyphs, subset_size, pfname);
  if (code < 0)
		return code;
  code = beginFontFile(FontFile_id, &length_id, "/Subtype/Type1C", poss.tell(), &start, &writer);
  if (code < 0)
		return code;
  code = font->writeType2Font(writer.strm, options, subset_glyphs, subset_size, pfname);
  endFontFile(start, length_id, &writer);
  return 0;
}

int XWPSDevicePDF::embedFontCid0(XWPSFontCid0 *font,
		    						             long FontFile_id, 
		    						             const uchar *subset_cids,
		    						             uint subset_size, 
		    						             XWPSString *pfname)
{
	XWPSStream poss;
	int code;
  long length_id;
  long start;
  XWPSPSDFBinaryWriter writer;
  
  if (CompatibilityLevel < 1.2)
		return (int)(XWPSError::RangeCheck);
			
	poss.writePositionOnly();
	code = font->writeCid0Font(&poss, TYPE2_OPTIONS,  subset_cids, subset_size, pfname);
  if (code < 0)
		return code;
  code = beginFontFile(FontFile_id, &length_id, "/Subtype/CIDFontType0C", poss.tell(), &start, &writer);
  if (code < 0)
		return code;
		
  code = font->writeCid0Font(writer.strm, TYPE2_OPTIONS, subset_cids, subset_size, pfname);
  endFontFile(start, length_id, &writer);
  return 0;
}

int XWPSDevicePDF::embedFontCid2(XWPSFontCid2 *font,
		                             long FontFile_id, 
		                             const uchar *subset_bits,
		    						             uint subset_size, 
		    						             XWPSString *pfname)
{
#define OPTIONS 0
  XWPSStream poss;
  int code;
  long length_id;
  long start;
  XWPSPSDFBinaryWriter writer;
	poss.writePositionOnly();
  code = font->writeCid2Font(&poss, OPTIONS, subset_bits, subset_size, pfname);
  if (code < 0)
		return code;
  code = beginFontFile(FontFile_id, &length_id, NULL, poss.tell(), &start, &writer);
  if (code < 0)
		return code;
  font->writeCid2Font(writer.strm, OPTIONS, subset_bits, subset_size, pfname);
#undef OPTIONS
  endFontFile(start, length_id, &writer);
  return 0;
}

int XWPSDevicePDF::embedFontType1(XWPSFontType1 *font,
		     						              long FontFile_id, 
		     						              ulong * subset_glyphs,
		                              uint subset_size, 
		                              XWPSString *pfname)
{
	switch (font->FontType) 
	{
    case ft_encrypted:
			if (CompatibilityLevel < 1.2)
	  		return embedFontAsType1(font, FontFile_id, subset_glyphs, subset_size, pfname);
	    			
    case ft_encrypted2:
			return embedFontAsType2(font, FontFile_id, subset_glyphs, subset_size, pfname);
				
    default:    		
			return (int)(XWPSError::RangeCheck);
  }
}

int XWPSDevicePDF::embedFontType42(XWPSFontType42 *font,
		      						             long FontFile_id, 
		      						             ulong * subset_glyphs,
		      						             uint subset_size, 
		      						             XWPSString *pfname)
{
	int options = WRITE_TRUETYPE_CMAP | WRITE_TRUETYPE_NAME | (CompatibilityLevel <= 1.2 ? WRITE_TRUETYPE_NO_TRIMMED_TABLE : 0);
  XWPSStream poss;
  int code;
  long length_id;
  long start;
  XWPSPSDFBinaryWriter writer;

	poss.writePositionOnly();
  code = font->writeTrueTypeFont(&poss, options, subset_glyphs, subset_size, pfname);
  if (code < 0)
		return code;
  code = beginFontFile(FontFile_id, &length_id, NULL, poss.tell(), &start, &writer);
  if (code < 0)
		return code;
  font->writeTrueTypeFont(writer.strm, options, subset_glyphs, subset_size, pfname);
  endFontFile(start, length_id, &writer);
  return 0;
}

int XWPSDevicePDF::endAside()
{
	return endSeparate();
}

int XWPSDevicePDF::endCharProc(PSPDFStreamPosition * ppos)
{
	XWPSStream *s = strm;
  long start_pos = ppos->start_pos;
  long end_pos = s->tell();
  long length = end_pos - start_pos;

  if (length > 9999)
		return (int)(XWPSError::LimitCheck);
  s->seek(start_pos - 14);
  s->print("%d", length);
  s->seek(end_pos);
  s->pputs("endstream\n");
  endSeparate();
  return 0;
}

int XWPSDevicePDF::endFontFile(long start, long length_id, XWPSPSDFBinaryWriter *pbw)
{
	XWPSStream *s = strm;
  pbw->endBinary();
  s->pputs("\n");
  long length = tell() - start;
  s->pputs("endstream\n");
  endSeparate();
  openSeparate(length_id);
  strm->print("%ld\n", length);
  endSeparate();
  return 0;
}

int XWPSDevicePDF::endGState(XWPSPDFResource *pres)
{
	if (pres) 
	{
		int code;

		strm->pputs(">>\n");
		code = endResource();
		pres->object->written = true;
		if (code < 0)
	    return code;
		code = openPage(PDF_IN_STREAM);
		if (code < 0)
	    return code;
		strm->print("/R%ld gs\n", pres->resourceId());
  }
  return 0;
}

int XWPSDevicePDF::endObj()
{
	strm->pputs("endobj\n");
  return 0;
}

int XWPSDevicePDF::endResource()
{
	return endAside();
}

int XWPSDevicePDF::finalizeFontDescriptor(XWPSPDFFontDescriptor *pfd)
{
	XWPSFont *font = pfd->base_font;
  int code = (font ? computeFontDescriptor(pfd, font, NULL) : 0);

  if (code >= 0) 
  {
		if (pfd->FontFile_id)
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

int XWPSDevicePDF::findNamed(PSParamString * pname, XWPSCosObject **ppco)
{
	const XWPSCosValue *pvalue;

  if (!objnameIsValid(pname->data, pname->size))
		return (int)(XWPSError::RangeCheck);
  if ((pvalue = named_objects->dictFind(pname->data, pname->size)) != 0) 
  {
		*ppco = pvalue->contents.object;
		return 0;
  }
  return (int)(XWPSError::Undefined);
}

int XWPSDevicePDF::findStdAppearance(XWPSFontBase *bfont, int mask, int *psame)
{
	bool has_uid = bfont->UID.isUniqueID() && bfont->UID.id != 0;
  const XWPSPDFStdFont *psf = std_fonts;
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

int XWPSDevicePDF::flateBinary(XWPSPSDFBinaryWriter *pbw)
{
	const PSStreamTemplate *templat = &s_zlibE_template;
  PSStreamState *st = (*(templat->create_state))();
  if (templat->set_defaults)
		(*(templat->set_defaults))(st);
  return pbw->encodeBinary((PSStreamTemplate*)templat, st, true);
}

PSPDFFontEmbed XWPSDevicePDF::fontEmbedStatus(XWPSFont *font, int *pindex, int *psame)
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

int XWPSDevicePDF::getNamed(PSParamString * pname,
	                          PSCosObjectProcs * , 
	                          XWPSCosObject **ppco)
{
	return referNamed(pname, ppco);
}

int XWPSDevicePDF::makeNamed(PSParamString * pname,
	                           PSCosObjectProcs * cotype, 
	                           XWPSCosObject **ppco, 
	                           bool assign_id)
{
	if (pname) 
	{
		int code = referNamed(pname, ppco);
		XWPSCosObject *pco = *ppco;
		if (code < 0)
	    return code;
	    
		if (assign_id && pco->id == 0)
	    pco->id = objRef();
		return code;
  } 
  else 
  {
		int code = createNamed(pname, cotype, ppco, (assign_id ? 0L : -1L));
		return (code < 0 ? code : 1);
  }
}

int XWPSDevicePDF::makeNamedDict(PSParamString * pname,
		    						             XWPSCosObject **ppcd, 
		    						             bool assign_id)
{
	XWPSCosObject *pco;
  int code = makeNamed(pname, &cos_dict_procs, &pco, assign_id);
  if (pco)
  	pco->incRef();
  *ppcd = pco;
  return code;
}

int  XWPSDevicePDF::noneToStream()
{
	if (contents_id != 0)
		return (int)(XWPSError::Fatal);
			
  contents_id = beginObj();
  contents_length_id = objRef();
  XWPSStream * s = strm;
  s->print("<</Length %ld 0 R", contents_length_id);
  if (compression == pdf_compress_Flate)
		s->print("/Filter /%s", compression_filter_name);
  s->pputs(">>\nstream\n");
  contents_pos = tell();
  if (compression == pdf_compress_Flate)
  {
  	const PSStreamTemplate *templat = &s_zlibE_template;
  	uint l = 512;
  	uchar *buf = (uchar*)malloc(l * sizeof(uchar*));
  	PSStreamState * st = (*(templat->create_state))();
  	XWPSStream *es = new XWPSStream;
  	es->stdInit(buf, l, &s_filter_write_procs, PS_STREAM_MODE_WRITE, true);
  	st->templat = templat;
  	es->state = st;
  	es->own_ss = true;
		es->procs.process = templat->process;
		es->strm = s;
		(*(templat->set_defaults))(st);
		(*(templat->init))(st);
  	strm = s = es;
  }
  s->print("%g 0 0 %g 0 0 cm\n", 72.0 / HWResolution[0], 72.0 / HWResolution[1]);
  if (CompatibilityLevel >= 1.3) 
  {
		if (params.DefaultRenderingIntent != ri_Default) 
		{
	    static const char *const ri_names[] = {"Default", "Perceptual", "Saturation", 
	    	                                     "RelativeColorimetric","AbsoluteColorimetric" };

	    s->print("/%s ri\n", ri_names[(int)params.DefaultRenderingIntent]);
		}
  }
  
  s->pputs("q\n");
  return PDF_IN_STREAM;
}

long XWPSDevicePDF::objRef()
{
	long id = nextId();
  long pos = tell();
  xref->file->write((char*)(&pos), sizeof(pos));
  return id;
}

int XWPSDevicePDF::openContents(PSPDFContext contextA)
{
	context_proc proc;
   while ((proc = context_procs[context][contextA]) != 0) 
   {
			int code = (this->*proc)();
			if (code < 0)
	    	return code;
			context = (PSPDFContext)code;
   }
   context = contextA;
   return 0;
}

void XWPSDevicePDF::openDocument()
{
	if (!isInPage() && tell() == 0) 
	{
		XWPSStream *s = strm;
		int level = (int)(CompatibilityLevel * 10 + 0.5);

		s->print("%%PDF-%d.%d\n", level / 10, level % 10);
		binary_ok = !params.ASCII85EncodePages;
		if (binary_ok)
	    s->pputs("%\307\354\217\242\n");
  }
  
  if (!params.CompressPages)
		compression = pdf_compress_none;
  else if (CompatibilityLevel < 1.2)
		compression = pdf_compress_LZW;
  else if (params.UseFlateCompression)
		compression = pdf_compress_Flate;
  else
		compression = pdf_compress_LZW;
}

int  XWPSDevicePDF::openGState(XWPSPDFResource **ppres)
{
	if (*ppres)
		return 0;
  return beginResource(resourceExtGState, ps_no_id, ppres);
}

long  XWPSDevicePDF::openObj(long id)
{
	XWPSStream *s = strm;
  if (id <= 0) 
  {
		id = objRef();
  } 
  else 
  {
		long pos = tell();
		QIODevice *tfile = xref->file;
		long tpos = (long)(tfile->pos());

		tfile->seek((id - FirstObjectNumber) * sizeof(pos));
		tfile->write((char*)&pos, sizeof(pos));
		tfile->seek(tpos);
  }
 
  s->print("%ld 0 obj\n", id);
  return id;
}

int XWPSDevicePDF::openPage(PSPDFContext contextA)
{
	if (!isInPage()) 
	{
		if (pageId(next_page + 1) == 0)
	    return (int)(XWPSError::VMError);
		openDocument();
  }
  
  return openContents(contextA);
}

long XWPSDevicePDF::pageId(int page_num)
{
	if (page_num < 1)
		return 0;
		
  if (page_num >= num_pages)
  {
  	uint new_num_pages = qMax(page_num + 10, num_pages << 1);
  	XWPSPDFPage *new_pages = new XWPSPDFPage[new_num_pages];
  	for (int i = 0; i < num_pages; i++)
  	{
  		new_pages[i].Page = pages[i].Page;
  		pages[i].Page = 0;
  		pages[i].Annots = 0;
  	}
  	
  	delete [] pages;
  	pages = new_pages;
  	num_pages = new_num_pages;
  }
  
  XWPSCosObject * Page = pages[page_num - 1].Page;
  if (Page == 0)
  {
  	pages[page_num - 1].Page = Page = new XWPSCosObject(this, &cos_dict_procs);
		Page->id = objRef();
  }
  return Page->id;
}

int  XWPSDevicePDF::patternSpace(XWPSCosValue *pvalue,
		  							XWPSPDFResource **ppres, 
		  							const char *cs_name)
{
	if (!*ppres) 
	{
		int code = beginResourceBody(resourceColorSpace, ps_no_id, ppres);

		if (code < 0)
	    return code;
		strm->print("%s\n", cs_name);
		endResource();
		(*ppres)->object->written = true;
  }
  pvalue->resourceValue((*ppres)->object);
  if ((*ppres)->object)
  	(*ppres)->object->incRef();
  return 0;
}

int XWPSDevicePDF::pdfFunction(XWPSFunction *pfn, XWPSCosValue *pvalue)
{
	XWPSFunctionInfo info;
  XWPSPDFResource *pres;
  XWPSCosObject *pcfn;
  XWPSCosObject *pcd;
  XWPSCosValue v;
  int code = allocResource(resourceFunction, ps_no_id, &pres, 0L);

  if (code < 0)
		return code;
		
	pcfn = pres->object;
  pfn->getInfo(&info);
  if (info.DataSource != 0)
  {
  	XWPSPSDFBinaryWriter writer;
		XWPSStream *save = strm;
		XWPSCosObject *pcos;
		XWPSStream *s;
		pcfn->become(&cos_stream_procs);
		pcos = pcfn;
		pcd = pcos;
		s = new XWPSStream(pcos, this);
		strm = s;
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
	    	s->close();
			}
			
			strm = save;
			if (code < 0)
	    	return code;
	  }
	  
	  if (s->decRef() == 0)
	  	delete s;
  }
  else
  {
  	pcfn->become(&cos_dict_procs);
		pcd = pcfn;
  }
  
  if (info.Functions != 0)
  {
		XWPSCosObject *functions = new XWPSCosObject(this, &cos_array_procs);
		for (int i = 0; i < info.num_Functions; ++i)
		{
			if ((code = pdfFunction(info.Functions[i], &v)) < 0 ||	
				  (code = functions->arrayAdd(&v)) < 0)
			{
				delete functions;
				return code;
	    }
		}
		
		v.objectValue(functions);
		code = pcd->dictPut("/Functions", &v);
  }
  
  XWPSCosParamListWriter rlist(pcd, PRINT_BINARY_OK);
  
  code = pfn->getParams(context_state, &rlist);
  if (code < 0)
		return code;
		
	pcd->incRef();
  pvalue->objectValue(pcd);
  return 0;
}

void XWPSDevicePDF::pdfmarkAdjustParentCount(XWPSPDFOutlineLevel * plevel)
{
	XWPSPDFOutlineLevel *parent = plevel - 1;
  int count = plevel->last.count;

  if (count > 0) 
  {
		if (parent->last.count < 0)
	    parent->last.count -= count;
		else
	    parent->last.count += count;
  }
}

int  XWPSDevicePDF::pdfmarkAnnot(PSParamString * pairs, 
	                               uint count,
	                               XWPSMatrix * pctm, 
	                               PSParamString *objname,
	                               const char *subtype)
{
	PSAOParams params;
  XWPSCosObject *pcd;
  int page_index = next_page;
  XWPSCosObject *annots;
  int code;
  
  params.pdev = this;
  params.subtype = subtype;
  params.src_pg = -1;
  code = makeNamedDict(objname, &pcd, true);
  if (code < 0)
		return code;
		
	code = pcd->dictPut("/Type", "/Annot");
  if (code < 0)
		return code;
  code = pdfmarkPutAoPairs(pcd, pairs, count, pctm, &params, false);
  if (code < 0)
		return code;
  if (params.src_pg >= 0)
		page_index = params.src_pg;
  if (pageId(page_index + 1) <= 0)
		return (int)(XWPSError::RangeCheck);
  annots = pages[page_index].Annots;
  if (annots == 0) 
  {
		annots = new XWPSCosObject(this, &cos_array_procs);
		if (annots == 0)
	    return (int)(XWPSError::VMError);
		pages[page_index].Annots = annots;
  }
  
  if (!objname)
  	pcd->writeObject(this);
  	
 	XWPSCosValue value(pcd);
 	return annots->arrayAdd(&value);
}

int XWPSDevicePDF::pdfmarkANN(PSParamString *pairs, 
	                            uint count, 
	                            XWPSMatrix *pctm, 
	                            PSParamString *objname)
{
	return pdfmarkAnnot(pairs, count, pctm, objname, "/Text");
}

int XWPSDevicePDF::pdfmarkARTICLE(PSParamString * pairs, 
	                                uint count,
		                              XWPSMatrix * pctm, 
		                              PSParamString * )
{
	PSParamString title;
  PSParamString rectstr;
  XWPSRect rect;
  long bead_id;
  XWPSPDFArticle *part;
  int code;

  if (!pdfmarkFindKey("/Title", pairs, count, &title) ||
			!pdfmarkFindKey("/Rect", pairs, count, &rectstr))
		return (int)(XWPSError::RangeCheck);
  if ((code = pdfmarkScanRect(&rect, &rectstr, pctm)) < 0)
		return code;
  bead_id = objRef();

  for (part = articles; part != 0; part = part->next) 
  {
		XWPSCosValue *a_title = part->contents->dictFind((const uchar *)"/Title", 6);
		if (a_title != 0 && !a_title->isObject() &&
	    	!bytes_compare(a_title->contents.chars.data,
			   a_title->contents.chars.size,
			   title.data, title.size))
	    break;
  }
  if (part == 0) 
  {
		XWPSCosObject *contents = new XWPSCosObject(this, &cos_dict_procs);
		part = new XWPSPDFArticle;
		
		contents->id = objRef();
		part->next = articles;
		articles = part;
		contents->dictPut((const uchar *)"/Title", 6, title.data, title.size);
		part->contents = contents;
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
  part->last.article_id = part->contents->id;
  part->last.next_id = 0;
  part->last.rect = rect;
  {
		PSParamString page_string;
		int page = 0;
		if (pdfmarkFindKey("/Page", pairs, count, &page_string))
	    page = pdfmarkPageNumber(&page_string);
		part->last.page_id = pageId(page);
		for (uint i = 0; i < count; i += 2) 
		{
	    if (pairs[i].pdfKeyEq("/Rect") || pairs[i].pdfKeyEq("/Page"))
				continue;
	    pdfmarkPutPair(part->contents, &pairs[i]);
		}
  }
  if (part->first.id == 0) 
  {
		part->first = part->last;
		part->last.id = 0;
  }
  return 0;
}

int  XWPSDevicePDF::pdfmarkBP(PSParamString * pairs, 
	                            uint count,
	                            XWPSMatrix * pctm, 
	                            PSParamString * objname)
{
	XWPSRect bbox;
  XWPSCosObject *pcs;
  XWPSPDFGraphicsSave *pdgs;
  int code;
  double xscale = HWResolution[0] / 72.0,	yscale = HWResolution[1] / 72.0;
  char bbox_str[6 + 4 * 12];

  if (objname == 0 || count != 2 || !pairs[0].pdfKeyEq("/BBox"))
		return (int)(XWPSError::RangeCheck);
  if (sscanf((const char *)pairs[1].data, "[%lg %lg %lg %lg]",
	       &bbox.p.x, &bbox.p.y, &bbox.q.x, &bbox.q.y) != 4)
		return (int)(XWPSError::RangeCheck);
  code = makeNamed(objname, &cos_stream_procs, (XWPSCosObject **)&pcs, true);
  if (code < 0)
		return code;
  pcs->is_graphics = true;
  bbox.bboxTransform(pctm, &bbox);
  sprintf(bbox_str, "[%.8g %.8g %.8g %.8g]", bbox.p.x * xscale, bbox.p.y * yscale, bbox.q.x * xscale, bbox.q.y * yscale);
  if ((code = pcs->dictPut("/Type", "/XObject")) < 0 ||
			(code = pcs->dictPut("/Subtype", "/Form")) < 0 ||
			(code = pcs->dictPut("/FormType", "1")) < 0 ||
			(code = pcs->dictPut("/Matrix", "[1 0 0 1 0 0]")) < 0 ||
			(code = pcs->dictPut("/BBox", (uchar *)bbox_str, strlen(bbox_str))) < 0)
		return code;
  pdgs = new XWPSPDFGraphicsSave;
  if (context != PDF_IN_NONE) 
  {
		code = openPage(PDF_IN_STREAM);
		if (code < 0) 
		{
	    delete pdgs;
	    return code;
		}
  }
  if (!open_graphics) 
  {
		pictures->save_strm = strm;
		strm = pictures->strm;
  }
  pdgs->prev = open_graphics;
  pdgs->object = pcs;
  pdgs->position = pictures->strm->tell();
  pdgs->save_context = context;
  pdgs->save_contents_id = contents_id;
  open_graphics = pdgs;
  context = PDF_IN_STREAM;
  contents_id = pcs->id;
  return 0;
}

int  XWPSDevicePDF::pdfmarkCLOSE(PSParamString * pairs, 
	                               uint count,
	                               XWPSMatrix * , 
	                               PSParamString * )
{
	XWPSCosObject *pco;
  int code;

  if (count != 1)
		return (int)(XWPSError::RangeCheck);
  if ((code = getNamed(&pairs[0], &cos_stream_procs, &pco)) < 0)
  {
		return code;
	}
  if (!pco->is_open)
		return (int)(XWPSError::RangeCheck);
  pco->is_open = false;
  return 0;
}

int  XWPSDevicePDF::pdfmarkDEST(PSParamString * pairs, 
	                              uint count,
	                              XWPSMatrix * , 
	                              PSParamString * objname)
{
	int present;
  char dest[MAX_DEST_STRING];
  PSParamString key;
  
  if (!pdfmarkFindKey("/Dest", pairs, count, &key) ||
			(present = pdfmarkMakeDest(dest, "/Page", "/View", pairs, count)) < 0)
		return (int)(XWPSError::RangeCheck);
 	XWPSCosValue value((uchar *)dest, strlen(dest));
  if (!Dests) 
  {
		Dests = new XWPSCosObject(this, &cos_dict_procs);
		Dests->id = objRef();
  }
  if (objname || count > (present + 1) * 2) 
  {
		XWPSCosObject *ddict;
		int code = makeNamedDict(objname, &ddict, false);
		if (code < 0)
	    return code;
		code = ddict->dictPut("/D", (uchar *)dest, strlen(dest));
		for (int i = 0; code >= 0 && i < count; i += 2)
	    if (!pairs[i].pdfKeyEq("/Dest") &&
					!pairs[i].pdfKeyEq("/Page") &&
					!pairs[i].pdfKeyEq("/View"))
			code = pdfmarkPutPair(ddict, &pairs[i]);
		if (code < 0)
	   	return code;
	   	
	  value.objectValue(ddict);
  }
  return Dests->dictPut(key.data, key.size, &value);
}

int XWPSDevicePDF::pdfmarkDOCINFO(PSParamString * pairs, 
	                                uint count,
		                              XWPSMatrix * , 
		                              PSParamString * )
{
	XWPSCosObject * pcd = Info;
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
		code = pdfmarkPutPair(pcd, pair);
#undef VDATA
#undef VSIZE
  }
  return code;
}

int XWPSDevicePDF::pdfmarkDOCVIEW(PSParamString * pairs, 
	                                uint count,
		                              XWPSMatrix * , 
		                              PSParamString * )
{
	char dest[MAX_DEST_STRING];
  if (count & 1)
		return (int)(XWPSError::RangeCheck);
  if (pdfmarkMakeDest(dest, "/Page", "/View", pairs, count)) 
  {
		int code = Catalog->dictPut("/OpenAction",(uchar *)dest, strlen(dest));
		for (uint i = 0; code >= 0 && i < count; i += 2)
	    if (!(pairs[i].pdfKeyEq("/Page") || pairs[i].pdfKeyEq("/View")))
				code = pdfmarkPutPair(Catalog, pairs + i);
			return code;
   } 
   else
		return pdfmarkPutPairs(Catalog, pairs, count);
}

int  XWPSDevicePDF::pdfmarkEMC(PSParamString *, 
	               uint , 
	               XWPSMatrix *, 
	               PSParamString *)
{
	return 0;
}

int XWPSDevicePDF::pdfmarkEP(PSParamString * , 
	                           uint count,
	                           XWPSMatrix * , 
	                           PSParamString * )
{
	XWPSPDFGraphicsSave *pdgs = open_graphics;
  XWPSPDFResource *pres;
  XWPSCosObject *pcs;
  long start;
  uint size;
  int code;

  if (count != 0 || pdgs == 0 || !(pcs = pdgs->object)->is_open)
		return (int)(XWPSError::RangeCheck);
  code = openContents(PDF_IN_STREAM);
  if (code < 0)
		return code;
  code = allocResource(resourceXObject, ps_no_id, &pres, pcs->id);
  if (code < 0)
		return code;
  pres->object = pcs;
  start = pdgs->position;
  pcs->is_open = false;
  size = strm->tell() - start;
  open_graphics = pdgs->prev;
  context = pdgs->save_context;
  contents_id = pdgs->save_contents_id;
  pdgs->object = 0;
  delete pdgs;
  strm->flush();
  strm->seek(start);
  pictures->file->seek(start);
  streams->strm->copyData(pictures->file, size);
  code = pcs->streamAdd(size);
  pictures->file->seek(start);
  if (!open_graphics) 
  {
		strm = pictures->save_strm;
		pictures->save_strm = 0;
  }
  return code;
}

int XWPSDevicePDF::pdfmarkLNK(PSParamString *pairs, 
	                            uint count, 
	                            XWPSMatrix *pctm, 
	                            PSParamString *objname)
{
	return pdfmarkAnnot(pairs, count, pctm, objname, "/Link");
}

int XWPSDevicePDF::pdfmarkNamespacePop(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int XWPSDevicePDF::pdfmarkNamespacePush(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDevicePDF::pdfmarkNI(PSParamString *, 
	               uint , 
	               XWPSMatrix *, 
	               PSParamString *)
{
	return 0;
}

int  XWPSDevicePDF::pdfmarkOBJ(PSParamString * pairs, 
	                             uint count,
	                             XWPSMatrix * , 
	                             PSParamString * objname)
{
	PSCosObjectProcs * cotype;
  XWPSCosObject *pco;
  int code;

  if (objname == 0 || count != 2 || !pairs[0].pdfKeyEq("/type"))
		return (int)(XWPSError::RangeCheck);
  if (pairs[1].pdfKeyEq("/array"))
		cotype = &cos_array_procs;
  else if (pairs[1].pdfKeyEq("/dict"))
		cotype = &cos_dict_procs;
  else if (pairs[1].pdfKeyEq("/stream"))
		cotype = &cos_stream_procs;
  else
		return (int)(XWPSError::RangeCheck);
  if ((code = makeNamed(objname, cotype, &pco, true)) < 0) 
  {
		if (code == XWPSError::RangeCheck &&
	    	referNamed(objname, &pco) >= 0)
	    return 0;
		return code;
  }
  return 0;
}

int  XWPSDevicePDF::pdfmarkOUT(PSParamString * pairs, 
	                             uint count,
	                             XWPSMatrix * pctm, 
	                             PSParamString * )
{
	int depth = outline_depth;
  XWPSPDFOutlineLevel *plevel = &outline_levels[depth];
  int sub_count = 0;
  uint i;
  XWPSPDFOutlineNode node;
  PSAOParams ao;
  int code;

  for (i = 0; i < count; i += 2) 
  {
		PSParamString *pair = &pairs[i];

		if (pair->pdfKeyEq("/Count"))
	    (pair + 1)->pdfmarkScanInt(&sub_count);
  }
  if (sub_count != 0 && depth == MAX_OUTLINE_DEPTH - 1)
		return (int)(XWPSError::LimitCheck);
  node.action = new XWPSCosObject(this, &cos_dict_procs);
  if (node.action == 0)
		return (int)(XWPSError::VMError);
		
  ao.pdev = this;
  ao.subtype = 0;
  ao.src_pg = -1;
  code = pdfmarkPutAoPairs(node.action, pairs, count, pctm, &ao, true);
  if (code < 0)
		return code;
  if (outlines_id == 0)
		outlines_id = objRef();
  node.id = objRef();
  node.parent_id = (depth == 0 ? outlines_id : plevel[-1].last.id);
  node.prev_id = plevel->last.id;
  node.count = sub_count;
  if (plevel->first.id == 0) 
  {
		if (depth > 0)
	    plevel[-1].last.first_id = node.id;
			node.prev_id = 0;
			plevel->first = node;
			plevel->first.action = 0;
  } 
  else 
  {
		if (depth > 0)
	    pdfmarkAdjustParentCount(plevel);
		pdfmarkWriteOutline(&plevel->last, node.id);
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
  } 
  else 
  {
		while ((depth = outline_depth) > 0 && outline_levels[depth].left == 0 )
	    pdfmarkCloseOutline();
  }
  return 0;
}

int  XWPSDevicePDF::pdfmarkPAGE(PSParamString * pairs, 
	                              uint count,
	                              XWPSMatrix * , 
	                              PSParamString * )
{
	return pdfmarkPutPairs(currentPageDict(), pairs, count);
}

int  XWPSDevicePDF::pdfmarkPAGES(PSParamString * pairs, 
	                               uint count,
	                               XWPSMatrix * , 
	                               PSParamString * )
{
	return pdfmarkPutPairs(Pages, pairs, count);
}

int  XWPSDevicePDF::pdfmarkPS(PSParamString * pairs, 
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
		XWPSStream *s = strm;
		s->write(source.data, source.size);
		s->pputs(" PS\n");
  } 
  else 
  {
		XWPSPDFResource *pres;
		XWPSCosObject *pcs = 0;
		uint size;
		int code;

		code = makeNamed(objname, &cos_stream_procs, (XWPSCosObject **)&pcs, true);
		if (code < 0)
	    return code;
		code = allocResource(resourceXObject, ps_no_id, &pres, pcs->id);
		if (code < 0)
		{
			if (!objname)
				if (pcs)
					if (pcs->decRef() == 0)
						delete pcs;
	    return code;
	  }
	  if (pres->object != pcs)
	  {
	  	if (pres->object)
	  		if (pres->object->decRef() == 0)
	  			delete pres->object;
		}
		if (objname)
			pcs->incRef();
		pres->object = pcs;
		code = pcs->dictPut("/Type", "/XObject");
		if (code < 0)
	    return code;
		code = pcs->dictPut("/Subtype", "/PS");
		if (code < 0)
	    return code;
		if (level1.data != 0) 
		{
	    long level1_id = objRef();
	    char r[10 + 5];
	    long length_id = objRef();

	    sprintf(r, "%ld 0 R", level1_id);
	    code = pcs->dictPut("/Level1", (uchar *)r, strlen(r));
	    if (code < 0)
				return code;
	   	openSeparate(level1_id);
	    XWPSStream * s = strm;
	    s->print("<</Length %ld 0 R>>stream\n", length_id);
	    size = pdfmarkWritePs(s, &level1);
	    s->pputs("endstream\n");
	    endSeparate();
	    openSeparate(length_id);
	    s->print("%ld\n", (long)size);
	    endSeparate();
		}
		size = pdfmarkWritePs(streams->strm, &source);
		code = pcs->streamAdd(size);
		if (code < 0)
	    return code;
		if (objname)
	    pres->named = true;
		else 
		{
	    pcs->writeObject(this);
		}
		code = openContents(PDF_IN_STREAM);
		if (code < 0)
	    return code;
		strm->print("/R%ld Do\n", pcs->id);
  }
  return 0;
}

int  XWPSDevicePDF::pdfmarkPUT(PSParamString * pairs, 
	                             uint count,
	                             XWPSMatrix * , 
	                             PSParamString * )
{
	XWPSCosObject *pco;
  int code, index;

  if (count != 3)
		return (int)(XWPSError::RangeCheck);
  if ((code = getNamed(&pairs[0], &cos_array_procs, &pco)) < 0)
		return code;
  if ((code = pairs[1].pdfmarkScanInt(&index)) < 0)
		return code;
  if (index < 0)
		return (int)(XWPSError::RangeCheck);
			
	XWPSCosValue value(pairs[2].data, pairs[2].size);
  return pco->arrayPut(index,&value);
}

int  XWPSDevicePDF::pdfmarkPUTDICT(PSParamString * pairs, 
	                                 uint count,
	                                 XWPSMatrix * , 
	                                 PSParamString * )
{
	XWPSCosObject *pco;
  int code;

  if ((code = referNamed(&pairs[0], &pco)) < 0)
		return code;
		
  return pdfmarkPutPairs(pco, pairs + 1, count - 1);
}

int  XWPSDevicePDF::pdfmarkPUTINTERVAL(PSParamString * pairs, 
	                                     uint count,
	                                     XWPSMatrix * , 
	                                     PSParamString * )
{
	XWPSCosObject *pco;
  int code, index, i;

  if (count < 2)
		return (int)(XWPSError::RangeCheck);
  if ((code = getNamed(&pairs[0], &cos_array_procs, &pco)) < 0)
		return code;
  if ((code = pairs[1].pdfmarkScanInt(&index)) < 0)
		return code;
  if (index < 0)
		return (int)(XWPSError::RangeCheck);

  for (i = 2; code >= 0 && i < count; ++i)
  {
  	XWPSCosValue value(pairs[i].data, pairs[i].size);
		code = pco->arrayPut(index + i - 2,&value);
	}
  return code;
}

int  XWPSDevicePDF::pdfmarkPUTSTREAM(PSParamString * pairs, 
	                                   uint count,
	                                   XWPSMatrix * , 
	                                   PSParamString * )
{
	XWPSCosObject *pco;
  int code, i;

  if (count < 2)
		return (int)(XWPSError::RangeCheck);
  if ((code = getNamed(&pairs[0], &cos_stream_procs, &pco)) < 0)
		return code;
  if (!pco->is_open)
		return (int)(XWPSError::RangeCheck);
  for (i = 1; code >= 0 && i < count; ++i)
		code = pco->streamAddBytes(pairs[i].data, pairs[i].size);
  return code;
}

int  XWPSDevicePDF::pdfmarkSP(PSParamString * pairs, 
	                            uint count,
	                            XWPSMatrix * pctm, 
	                            PSParamString * )
{
	XWPSCosObject *pco;
  XWPSMatrix ctm;
  int code;

  if (count != 1)
		return (int)(XWPSError::RangeCheck);
  if ((code = getNamed(&pairs[0], &cos_stream_procs, &pco)) < 0)
		return code;
  if (pco->is_open || !pco->is_graphics)
		return (int)(XWPSError::RangeCheck);
  code = openContents(PDF_IN_STREAM);
  if (code < 0)
		return code;
  ctm = *pctm;
  ctm.tx *= HWResolution[0] / 72.0;
  ctm.ty *= HWResolution[1] / 72.0;
  putMatrix("q ", &ctm, "cm\n");
  strm->print("/R%ld Do Q\n", pco->id);
  return 0;
}

int  XWPSDevicePDF::pdfmarkStAttr(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDevicePDF::pdfmarkStBDC(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDevicePDF::pdfmarkStBMC(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDevicePDF::pdfmarkStBookmarkRoot(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDevicePDF::pdfmarkStClassMap(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDevicePDF::pdfmarkStOBJ(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDevicePDF::pdfmarkStPNE(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int XWPSDevicePDF::pdfmarkStPop(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDevicePDF::pdfmarkStPopAll(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDevicePDF::pdfmarkStPush(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDevicePDF::pdfmarkStRetrieve(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDevicePDF::pdfmarkStRoleMap(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDevicePDF::pdfmarkStStore(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *)
{
	return 0;
}

int  XWPSDevicePDF::pdfmarkCloseOutline()
{
	int depth = outline_depth;
  XWPSPDFOutlineLevel *plevel = &outline_levels[depth];
  int code;

   if (plevel->last.id) 
  {
		code = pdfmarkWriteOutline(&plevel->last, 0);
		if (code < 0)
	    return code;
  }
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

int XWPSDevicePDF::pdfmarkCoerceDest(PSParamString *dstr, char * dest)
{
	uchar *data = dstr->data;
  uint size = dstr->size;

  if (size == 0 || data[0] != '(')
		return 0;
		
  memcpy(dest, data, size - 1);
  dest[0] = '/';
  dest[size - 1] = 0;
  if (!dstr->persistent)
  {
  	if (dstr->data)
  	{
  		delete [] dstr->data;
  		dstr->data = 0;
  	}
  }
  dstr->persistent = false;
  dstr->data = new uchar[size];
  memcpy(dstr->data, dest, size - 1);
  dstr->size = size - 1;
  return 1;
}

bool XWPSDevicePDF::pdfmarkFindKey(const char *key, 
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

int XWPSDevicePDF::pdfmarkMakeDest(char * dstr,
		  								             const char *Page_key, 
		  								             const char *View_key,
		                               PSParamString * pairs, 
		                               uint count)
{
	PSParamString page_string, view_string;
  int present =	pdfmarkFindKey(Page_key, pairs, count, &page_string) + pdfmarkFindKey(View_key, pairs, count, &view_string);
  int page = pdfmarkPageNumber(&page_string);
  PSParamString action;
  int len;

  if (view_string.size == 0)
		view_string.fromString("[/XYZ 0 0 1]");
  if (page == 0)
		strcpy(dstr, "[null ");
  else if (pdfmarkFindKey("/Action", pairs, count, &action) && action.pdfKeyEq("/GoToR"))
		sprintf(dstr, "[%d ", page - 1);
  else
		sprintf(dstr, "[%ld 0 R ", pageId(page));
  len = strlen(dstr);
  if (len + view_string.size > MAX_DEST_STRING)
		return (int)(XWPSError::LimitCheck);
  if (view_string.data[0] != '[' ||	view_string.data[view_string.size - 1] != ']')
		return (int)(XWPSError::RangeCheck);
  memcpy(dstr + len, view_string.data + 1, view_string.size - 1);
  dstr[len + view_string.size - 1] = 0;
  return present;
}

void XWPSDevicePDF::pdfmarkMakeRect(char*str, XWPSRect * prect)
{
	XWPSStream s;
	s.writeString((uchar *)str, MAX_RECT_STRING - 1, false);
	s.print("[%g %g %g %g]", prect->p.x, prect->p.y, prect->q.x, prect->q.y);
  str[s.tell()] = 0;
}

const uchar * XWPSDevicePDF::pdfmarkNextObject(const uchar * scan, 
	                                             const uchar * end, 
	                                             const uchar **pname,
		                                           XWPSCosObject **ppco)
{
	int code;
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
			code = referNamed(&sname, ppco);
			if (code < 0)
	    	continue;
			return scan;
		}
	}
	*ppco = 0;
  return end;
}

int XWPSDevicePDF::pdfmarkPageNumber(PSParamString * pnstr)
{
	int page = next_page + 1;
  if (pnstr->data == 0);
  else if (pnstr->pdfKeyEq("/Next"))
		++page;
  else if (pnstr->pdfKeyEq("/Prev"))
		--page;
  else if (pnstr->pdfmarkScanInt(&page) < 0)
		page = 0;
  return page;
}

int  XWPSDevicePDF::pdfmarkProcess(PSParamStringArray * pma)
{
	PSParamString *data = pma->data;
  uint size = pma->size;
  PSParamString *pts = &data[size - 1];
  PSParamString *objname = 0;
  XWPSMatrix ctm;
  PSPDFMarkName *pmn;
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

int  XWPSDevicePDF::pdfmarkPutAoPairs(XWPSCosObject *pcd,
		                                  PSParamString * pairs, 
		                                  uint count,
		                                  XWPSMatrix * pctm, 
		                                  PSAOParams * params,
		                                  bool for_outline)
{
	PSParamString *Action = 0;
  PSParamString *File = 0;
  PSParamString Dest;
  PSParamString Subtype;
  int code;
  char dest[MAX_DEST_STRING];
  bool coerce_dest = false;
  
  if (!for_outline) 
  {
		code = pdfmarkMakeDest(dest,"/Page", "/View", pairs, count);
		if (code < 0)
	    return code;
		else if (code != 0)
	    Dest.fromString(dest);
  }
  if (params->subtype)
		Subtype.fromString(params->subtype);
		
	for (uint i = 0; i < count; i += 2)
	{
		PSParamString *pair = &pairs[i];
		long src_pg;
		
		if (pair->pdfKeyEq("/SrcPg") &&	sscanf((const char *)pair[1].data, "%ld", &src_pg) == 1)
	    params->src_pg = src_pg - 1;
		else if (!for_outline && pair->pdfKeyEq("/Color"))
	    pdfmarkPutCPair(pcd, "/C", pair + 1);
		else if (!for_outline && pair->pdfKeyEq("/Title"))
	    pdfmarkPutCPair(pcd, "/T", pair + 1);
		else if (pair->pdfKeyEq("/Action") || pair->pdfKeyEq("/A"))
	    Action = pair;
		else if (pair->pdfKeyEq("/File") || pair->pdfKeyEq("/F"))
	    File = pair;
		else if (pair->pdfKeyEq("/Dest")) 
		{
	    Dest = pair[1];
	    coerce_dest = true;
		}
		else if (pair->pdfKeyEq("/Page") || pair->pdfKeyEq("/View"))
		{
			if (Dest.data == 0) 
			{
				code = pdfmarkMakeDest(dest, "/Page", "/View", pairs, count);
				if (code < 0)
		    	return code;
				Dest.fromString(dest);
				coerce_dest = false;
	    }
		}
		else if (pair->pdfKeyEq("/Subtype"))
	    Subtype = pair[1];
	  else if (pair->pdfKeyEq("/Contents"))
	  {
	  	uchar *cstr;
	    uint csize = pair[1].size;
	    XWPSCosValue *pcv;
	    
	     pdfmarkPutPair(pcd, pair);
	     pcv = (XWPSCosValue *)pcd->dictFind((const uchar *)"/Contents", 9);
	     cstr = pcv->contents.chars.data;
	     uint i, j;
	     for (i = j = 0; i < csize;)
	     	if (csize - i >= 2 && !memcmp(cstr + i, "\\n", 2) && (i == 0 || cstr[i - 1] != '\\'))
	     	{
	     		cstr[j] = '\\', cstr[j + 1] = 'r';
		    	i += 2, j += 2;
	     	}
	     	else if (csize - i >= 4 && !memcmp(cstr + i, "\\012", 4) && (i == 0 || cstr[i - 1] != '\\'))
	     	{
	     		cstr[j] = '\\', cstr[j + 1] = 'r';
		    	i += 4, j += 2;
	     	}
	     	else
	     		cstr[j++] = cstr[i++];
	     		
	    if (j != i)
	    	pcv->contents.chars.data = (uchar*)realloc(pcv->contents.chars.data, (j+1) * sizeof(uchar));
	  }
	  else if (pair->pdfKeyEq("/Rect"))
	  {
	  	XWPSRect rect;
	    char rstr[MAX_RECT_STRING];
	    int code = pdfmarkScanRect(&rect, pair + 1, pctm);

	    if (code < 0)
				return code;
	    pdfmarkMakeRect(rstr, &rect);
	    pcd->dictPut("/Rect", (uchar *)rstr,  strlen(rstr));
	  }
	  else if (pair->pdfKeyEq("/Border"))
	  {
	  	char bstr[MAX_BORDER_STRING + 1];
	    int code;
	    
	    XWPSStream s;
	    s.writeString((uchar *)bstr, MAX_BORDER_STRING + 1, false);
	    code = pdfmarkWriteBorder(&s, pair + 1, pctm);
	    if (code < 0)
	    	return code;
		
	    if (s.tell() > MAX_BORDER_STRING)
				return (int)(XWPSError::LimitCheck);
					
	    bstr[s.tell()] = 0;
	    pcd->dictPut("/Border", (uchar *)bstr, strlen(bstr));
	  }
	  else if (for_outline && pair->pdfKeyEq("/Count"))
	  		;
	  else
	  	pdfmarkPutPair(pcd, pair);
	}
	
	if (!for_outline && Subtype.pdfKeyEq("/Link"))
	{
		if (Action)
		{
			PSParamString * tmpa = Action + 1;
			if (tmpa->pdfKeyEq("/GoTo") || (File && tmpa->pdfKeyEq("/GoToR")))
				;
		}
	}
	
	if (Action)
	{
		uchar *astr = Action[1].data;
		uint asize = Action[1].size;
		PSParamString * tmpa = Action + 1;
		if ((File != 0 || Dest.data != 0) &&
	     (tmpa->pdfKeyEq("/Launch") ||
	     (tmpa->pdfKeyEq("/GoToR") && File) ||
	      tmpa->pdfKeyEq("/Article")))
	  {
	  	XWPSCosObject * adict = new XWPSCosObject(this, &cos_dict_procs);
	  	if (!for_outline)
	  		adict->dictPut("/Type", "/Action");
	  	if (tmpa->pdfKeyEq("/Article")) 
	  	{
				adict->dictPut("/S", "/Thread");
				coerce_dest = false;
	    }
	    else
	    	pdfmarkPutCPair(adict, "/S", tmpa);
	    	
	    if (Dest.data) 
	    {
				if (coerce_dest)
		    	pdfmarkCoerceDest(&Dest, dest);
				pdfmarkPutCPair(adict, "/D", &Dest);
				Dest.data = 0;
	    }
	    
	    if (File) 
	    {
				pdfmarkPutCPair(adict, "/F", File + 1);
				File = 0;
	    }
	    
	    XWPSCosValue avalue(adict);
	    pcd->dictPut((const uchar *)"/A", 2, &avalue);
	  }
	  else if (asize >= 4 && !memcmp(astr, "<<", 2))
	  {
	  	uchar *scan = astr + 2;
	    uchar *end = astr + asize;
	    PSParamString key, value;
	    XWPSCosObject * adict = new XWPSCosObject(this, &cos_dict_procs);
	    while ((code = scanToken((const uchar**)&scan, end, (const uchar**)&key.data)) > 0)
	    {
	    	key.size = scan - key.data;
				if (key.data[0] != '/' || (code = scanTokenComposite((const uchar**)&scan, end, (const uchar**)&value.data)) != 1)
		    	break;
				value.size = scan - value.data;
				if (key.pdfKeyEq("/Dest") || key.pdfKeyEq("/D")) 
				{
		    	key.fromString("/D");
		    	if (value.data[0] == '(') 
		    	{
						pdfmarkCoerceDest(&value, dest);
		    	}
				} 
				else if (key.pdfKeyEq("/File"))
		    	key.fromString("/F");
				else if (key.pdfKeyEq("/Subtype"))
		    	key.fromString("/S");
				adict->dictPut(key.data, key.size, value.data, value.size);
	    }
	    
	    if (code <= 0 || !key.pdfKeyEq(">>"))
	    {
	    	delete adict;
				return (int)(XWPSError::RangeCheck);
			}
			
			XWPSCosValue avalue(adict);
	    pcd->dictPut((const uchar *)"/A", 2, &avalue);
	  }
	  else if (Action[1].pdfKeyEq("/GoTo"))
	    pdfmarkPutPair(pcd, Action);
	}
	
	if (Dest.data) 
	{
		if (coerce_dest)
	    pdfmarkCoerceDest(&Dest, dest);
		pdfmarkPutCPair(pcd, "/Dest", &Dest);
  }
  if (File)
		pdfmarkPutPair(pcd, File);
  if (Subtype.data)
		pdfmarkPutCPair(pcd, "/Subtype", &Subtype);
  return 0;
}

int  XWPSDevicePDF::pdfmarkPutCPair(XWPSCosObject *pcd, 
	                                  const char *key,
		                                PSParamString * pvalue)
{
	return pcd->dictPut(key, pvalue->data, pvalue->size);
}

int  XWPSDevicePDF::pdfmarkPutPair(XWPSCosObject *pcd, PSParamString * pair)
{
	return pcd->dictPut(pair->data, pair->size, pair[1].data, pair[1].size);
}

int  XWPSDevicePDF::pdfmarkPutPairs(XWPSCosObject *pcd, PSParamString * pairs, uint count)
{
	int code = 0;

  if (count & 1)
		return (int)(XWPSError::RangeCheck);
  for (int i = 0; code >= 0 && i < count; i += 2)
		code = pdfmarkPutPair(pcd, pairs + i);
  return code;
}

int  XWPSDevicePDF::pdfmarkScanRect(XWPSRect * prect, 
	                                  PSParamString * str,
		                                XWPSMatrix * pctm)
{
	uint size = str->size;
  double v[4];

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

int  XWPSDevicePDF::pdfmarkWriteArticle(XWPSPDFArticle * part)
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
  openSeparate(part->contents->id);
  XWPSStream * s = strm;
  s->print("<</F %ld 0 R/I<<", part->first.id);
  part->contents->dictElementsWrite(this);
  s->pputs(">> >>\n");
  return endSeparate();
}

int  XWPSDevicePDF::pdfmarkWriteBead(XWPSPDFBead * pbead)
{
	char rstr[MAX_RECT_STRING];

  openSeparate(pbead->id);
  XWPSStream * s = strm;
  s->print("<</T %ld 0 R/V %ld 0 R/N %ld 0 R", pbead->article_id, pbead->prev_id, pbead->next_id);
  if (pbead->page_id != 0)
		s->print("/P %ld 0 R", pbead->page_id);
  pdfmarkMakeRect(rstr, &pbead->rect);
  s->print("/R%s>>\n", rstr);
  return endSeparate();
}

int XWPSDevicePDF::pdfmarkWriteBorder(XWPSStream *s, 
	                                     PSParamString *str,
		     									             XWPSMatrix *pctm)
{
	uint size = str->size;

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
  s->print("[%g %g %g", fabs(bpt.x), fabs(bpt.y), fabs(cpt.x + cpt.y));
  next = strchr(chars + 1, ']');
  if (next == 0)
		return (int)(XWPSError::RangeCheck);
  if (next[1] != 0)
  {
  	double v;

		s->pputc('[');
		while (next != 0 && sscanf(++next, "%lg", &v) == 1) 
		{
	  	XWPSPoint vpt;

	  	vpt.distanceTransform(0.0, v, pctm);
	  	s->print("%g ", fabs(vpt.x + vpt.y));
	  	next = strchr(next, ' ');
		}
		s->pputc(']');
  }
  s->pputc(']');
  return 0;
}

int  XWPSDevicePDF::pdfmarkWriteOutline(XWPSPDFOutlineNode * pnode, long next_id)
{
	openSeparate(pnode->id);
  XWPSStream * s = strm;
  s->pputs("<< ");
  pnode->action->dictElementsWrite(this);
  if (pnode->count)
		s->print("/Count %d ", pnode->count);
  s->print("/Parent %ld 0 R\n", pnode->parent_id);
  if (pnode->prev_id)
		s->print("/Prev %ld 0 R\n", pnode->prev_id);
  if (next_id)
		s->print("/Next %ld 0 R\n", next_id);
  if (pnode->first_id)
		s->print("/First %ld 0 R /Last %ld 0 R\n", pnode->first_id, pnode->last_id);
  s->pputs(">>\n");
  endSeparate();
  delete pnode->action;
  pnode->action = 0;
  return 0;
}

uint XWPSDevicePDF::pdfmarkWritePs(XWPSStream *s, PSParamString * psource)
{
	uint size = psource->size - 2;
  s->write(psource->data + 1, size);
  s->pputc('\n');
  return size + 1;
}

int  XWPSDevicePDF::pdfPattern(XWPSDeviceColor *pdc,
	    						XWPSColorTile *p_tile, 
	    						XWPSColorTile *m_tile,
	    						XWPSCosObject *pcs_image, 
	    						XWPSPDFResource **ppres)
{
	XWPSPDFResource *pres;
  int code = allocResource(resourcePattern, pdc->mask.id, ppres, 0L);
  XWPSCosObject *pcos;
  XWPSCosObject *pcd;
  XWPSCosObject *pcd_Resources = new XWPSCosObject(this, &cos_dict_procs);
  XWPSColorTile *tile = (p_tile ? p_tile : m_tile);
  XWPSStripBitmap *btile = (p_tile ? &p_tile->tbits : &m_tile->tmask);
  uint p_size =	(p_tile == 0 ? 0 : p_tile->tbits.tileSize(color_info.depth));
  uint m_size =	(m_tile == 0 ? 0 : m_tile->tmask.tileSize(1));
  bool mask = p_tile == 0;
  XWPSPoint step;
  XWPSMatrix smat;

  if (code < 0)
		return code;
		
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
  pres = *ppres;
  {
		XWPSCosObject *pcd_XObject = new XWPSCosObject(this, &cos_dict_procs);
		char key[MAX_REF_CHARS + 3];
		XWPSCosValue v(pcs_image);

		sprintf(key, "/R%ld", pcs_image->id);
		if ((code = pcd_XObject->dictPut((uchar *)key, strlen(key), &v)) < 0 ||
	    	(code = pcd_Resources->dictPut("/XObject", pcd_XObject)) < 0)
	    return code;
  }
  
  if ((code = pcd_Resources->dictPut("/ProcSet", (mask ? "[/PDF/ImageB]" :	"[/PDF/ImageC]"))) < 0)
		return code;
  pres->object->become(&cos_stream_procs);
  pcos = pres->object;
  pcd = pcos;
  if ((code = pcd->dictPut("/PatternType", 1)) < 0 ||
			(code = pcd->dictPut("/PaintType", (mask ? 2 : 1))) < 0 ||
			(code = pcd->dictPut("/TilingType",tile->tiling_type)) < 0 ||
			(code = pcd->dictPut("/Resources", pcd_Resources)) < 0 ||
			(code = pcd->dictPut("/BBox", "[0 0 1 1]")) < 0 ||
			(code = cos_dict_put_matrix(pcd, "/Matrix", &smat)) < 0 ||
			(code = pcd->dictPut("/XStep", (float)(step.x / btile->rep_width))) < 0 ||
			(code = pcd->dictPut("/YStep", (float)(step.y / btile->rep_height))) < 0) 
	{
		return code;
  }

  {
		char buf[MAX_REF_CHARS + 6 + 1];

		sprintf(buf, "/R%ld Do\n", pcs_image->id);
		pcos->streamAddBytes((const uchar *)buf, strlen(buf));
  }

  return 0;
}

int  XWPSDevicePDF::prepareDrawing(XWPSImagerState *pis,
		    							const char *ca_format, 
		    							XWPSPDFResource **ppres)
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
	    strm->print("/BM/%s", bm_names[pis->blend_mode]);
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

int  XWPSDevicePDF::prepareFill(XWPSImagerState *pis)
{
	XWPSPDFResource *pres = 0;
  int code = prepareVector(pis, "/ca %g", &pres);

  if (code < 0)
		return code;
		
  if (CompatibilityLevel >= 1.2) 
  {
		if (params.PreserveOverprintSettings &&
	    	fill_overprint != pis->overprint) 
	  {
	    code = openGState(&pres);
	    if (code < 0)
				return code;
	    if (CompatibilityLevel < 1.3) 
	    {
				strm->print("/OP %s", pis->overprint);
				stroke_overprint = pis->overprint;
	    } 
	    else 
	    {
				strm->print("/op %s", pis->overprint);
	    }
	    fill_overprint = pis->overprint;
		}
  }
  return endGState(pres);
}

int  XWPSDevicePDF::prepareImage(XWPSImagerState *pis)
{
	XWPSPDFResource *pres = 0;
  int code = prepareDrawing(pis, "/ca %g", &pres);

  if (code < 0)
		return code;
  return endGState(pres);
}

int  XWPSDevicePDF::prepareImageMask(XWPSImagerState *pis,XWPSDeviceColor *pdcolor)
{
	int code = prepareImage(pis);

  if (code < 0)
		return code;
  return setDrawingColor(pdcolor, fill_color, &psdf_set_fill_color_commands);
}

int  XWPSDevicePDF::prepareStroke(XWPSImagerState *pis)
{
	XWPSPDFResource *pres = 0;
  int code = prepareVector(pis, "/CA %g", &pres);

  if (code < 0)
		return code;
		
  if (CompatibilityLevel >= 1.2) 
  {
		if (params.PreserveOverprintSettings &&
	    	stroke_overprint != pis->overprint) 
	  {
	    code = openGState(&pres);
	    if (code < 0)
				return code;
	    strm->print("/OP %s", pis->overprint);
	    stroke_overprint = pis->overprint;
	    
	    if (CompatibilityLevel < 1.3)
				fill_overprint = pis->overprint;
		}
		if (state->stroke_adjust != pis->stroke_adjust) 
		{
	    code = openGState(&pres);
	    if (code < 0)
				return code;
	    strm->print("/SA %s", pis->stroke_adjust);
	    state->stroke_adjust = pis->stroke_adjust;
		}
  }
  return endGState(pres);
}

int  XWPSDevicePDF::prepareVector(XWPSImagerState *pis,
		   								const char *ca_format, 
		   								XWPSPDFResource **ppres)
{
	int code = prepareDrawing(pis, ca_format, ppres);

  if (code < 0)
		return code;
  
  if (CompatibilityLevel >= 1.2)
  {
  	XWPSIntPoint phase, dev_phase;
		char hts[5 + MAX_FN_CHARS + 1],	trs[5 + MAX_FN_CHARS * 4 + 6 + 1], bgs[5 + MAX_FN_CHARS + 1], ucrs[6 + MAX_FN_CHARS + 1];

		hts[0] = trs[0] = bgs[0] = ucrs[0] = 0;
		if (params.PreserveHalftoneInfo &&
	    	halftone_id != pis->dev_ht->id )
	  {
	  	code = updateHalftone(pis, hts);
	    if (code < 0)
				return code;
	  }
	  if (params.TransferFunctionInfo == tfi_Preserve) 
	  {
	    code = updateTransfer(pis, trs);
	    if (code < 0)
				return code;
		}
		if (params.UCRandBGInfo == ucrbg_Preserve)
		{
			if (black_generation_id != pis->black_generation->id) 
			{
				code = writeTransferMap(pis->black_generation, 0, false, "/BG", bgs);
				if (code < 0)
		    	return code;
				black_generation_id = pis->black_generation->id;
	    }
	    if (undercolor_removal_id != pis->undercolor_removal->id) 
	    {
				code = writeTransferMap(pis->undercolor_removal, -1, false, "/UCR", ucrs);
				if (code < 0)
		    	return code;
				undercolor_removal_id = pis->undercolor_removal->id;
	    }
		}
		
		if (hts[0] || trs[0] || bgs[0] || ucrs[0])
		{
			code = openGState(ppres);
	    if (code < 0)
				return code;
	    strm->pputs(hts);
	    strm->pputs(trs);
	    strm->pputs(bgs);
	    strm->pputs(ucrs);
		}
		
		pis->currentHalftonePhase(&phase);
		state->currentHalftonePhase(&dev_phase);
		if (dev_phase.x != phase.x || dev_phase.y != phase.y)
		{
			code = openGState(ppres);
	    if (code < 0)
				return code;
	    strm->print("/HTP[%d %d]", phase.x, phase.y);
	    state->setScreenPhase(phase.x, phase.y, ps_color_select_all);
		}
  }
  
  if (CompatibilityLevel >= 1.3) 
  {
		if (overprint_mode != params.OPM) 
		{
	    code = openGState(ppres);
	    if (code < 0)
				return code;
	    strm->print("/OPM %d", params.OPM);
	    overprint_mode = params.OPM;
		}
		if (state->smoothness != pis->smoothness) 
		{
	    code = openGState(ppres);
	    if (code < 0)
				return code;
	    strm->print("/SM %g", pis->smoothness);
	    state->smoothness = pis->smoothness;
		}
		if (CompatibilityLevel >= 1.4) 
		{
	    if (state->text_knockout != pis->text_knockout) 
	    {
				code = openGState(ppres);
				if (code < 0)
		    return code;
				strm->print("/TK %s", pis->text_knockout);
				state->text_knockout = pis->text_knockout;
	    }
		}
  }
  return code;
}

int  XWPSDevicePDF::putClipPath(XWPSClipPath * pcpath)
{
	XWPSStream *s = strm;

  if (pcpath == NULL) 
  {
		if (clip_path_id == no_clip_path_id)
	    return 0;
		s->pputs("Q\nq\n");
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
	    s->pputs("Q\nq\n");
	    clip_path_id = no_clip_path_id;
		} 
		else 
		{
	    XWPSDevVectorDoPathState state;
	    XWPSCPathEnum cenum;
	    XWPSFixedPoint vs[3];
	    int pe_op;

	    s->print("Q\nq\n%s\n", (pcpath->rule <= 0 ? "W" : "W*"));
	    state.init(this, ps_path_type_fill, NULL);
	    
	    cenum.init(pcpath);
	    while ((pe_op = cenum.next(vs)) > 0)
				state.segment(pe_op, vs);
	    s->pputs("n\n");
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

int  XWPSDevicePDF::putColoredPattern(XWPSDeviceColor *pdc,
													const PSPSDFSetColorCommands *ppscc,
													XWPSPDFResource **ppres)
{
	XWPSColorTile *m_tile = pdc->mask.m_tile;
  XWPSColorTile *p_tile = pdc->colors.pattern.p_tile;
  int w = p_tile->tbits.rep_width, h = p_tile->tbits.rep_height;
  XWPSColorSpace cs_Device;
  cs_Device.i_ctx_p = context_state;
  XWPSPDFImageWriter writer;
  XWPSImage1 image;
  XWPSCosObject *pcs_image;
  XWPSCosObject *pcs_mask = 0;
  XWPSCosValue v;
  XWPSCosValue cs_value;
  long pos;
  int code = csPatternColored(&v);
  
  if (code < 0)
		return code;
  cs_Device.initDevice(color_info.num_components);
  code = colorSpace(&cs_value, &cs_Device, &pdf_color_space_names, true);
  if (code < 0)
		return code;
		
	image.initAdjust(&cs_Device, false);
  image.BitsPerComponent = 8;
  setPatternImage(&image, &p_tile->tbits);
  if (m_tile) 
  {
		if ((code = putPatternMask(m_tile, &pcs_mask)) < 0)
	    return code;
  }
  if ((code = beginWriteImage(&writer, ps_no_id, w, h, NULL, false)) < 0 ||
			(code = writer.binary.setupLosslessFilters(this, &image)) < 0 ||
			(code = beginImageData(&writer, &image, &cs_value)) < 0)
		return code;
	pcs_image = writer.pres->object;
  pos = streams->strm->tell();
  if ((code = copyColorBits(writer.binary.strm, p_tile->tbits.data + (h - 1) * p_tile->tbits.raster, 0, -p_tile->tbits.raster, w, h, color_info.depth >> 3)) < 0 ||
			(code = pcs_image->streamAddSince(pos)) < 0 ||
			(code = endImageBinary(&writer, h)) < 0)
		return code;
  pcs_image = writer.pres->object;
  if ((pcs_mask != 0 &&
	 		(code = pcs_image->dictPut("/Mask",pcs_mask)) < 0) ||
			(code = endWriteImage(&writer)) < 0)
		return code;
	if (pcs_image)
		pcs_image->incRef();
  code = pdfPattern(pdc, p_tile, m_tile, pcs_image, ppres);
  if (code < 0)
		return code;
  v.write(this);
  strm->print(" %s", ppscc->setcolorspace);
  return 0;
}

int  XWPSDevicePDF::putDrawingColor(XWPSDeviceColor *pdc,
		      						              const PSPSDFSetColorCommands *ppscc)
{
	if (pdc->isPure())
		return setColor(pdc, ppscc);
	else
	{
		int code;
		XWPSPDFResource *pres;
		

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
	    
	  XWPSCosValue v;
	  v.resourceValue(pres->object);
	  if (pres->object)
	  	pres->object->incRef();
		v.write(this);
		strm->print(" %s\n", ppscc->setcolorn);
		return 0;
	}
}

int XWPSDevicePDF::putFilters(XWPSCosObject *pcd, 
	                XWPSStream *s,
		              const PSPDFFilterNames *pfn)
{
	const char *filter_name = 0;
  bool binary_okA = true;
  XWPSStream *fs = s;
  XWPSCosObject *decode_parms = 0;
//  int code;
  
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
			decode_parms = new XWPSCosObject(this, &cos_dict_procs);
			XWPSCosParamListWriter writer(decode_parms, 0);
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

	    decode_parms = new XWPSCosObject(this, &cos_dict_procs);
	    decode_parms->dictPut("/Predictor", (int)(ss->Predictor));
	    decode_parms->dictPut("/Columns", (int)(ss->Columns));
	    if (ss->Colors != 1)
	    	decode_parms->dictPut("/Colors", ss->Colors);
	    if (ss->BitsPerComponent != 8)
	    	decode_parms->dictPut("/BitsPerComponent", ss->BitsPerComponent);
		}
		else if (TEMPLATE_IS(s_RLE_template))
			filter_name = pfn->RunLengthDecode;
#undef TEMPLATE_IS
  }
  
  if (filter_name)
  {
  	if (binary_okA)
  	{
  		pcd->dictPut(pfn->Filter, filter_name);
  		if (decode_parms)
  			pcd->dictPut(pfn->DecodeParms, decode_parms);
  	}
  	else
  	{
  		XWPSCosObject * pca = new XWPSCosObject(this, &cos_array_procs);
  		pca->arrayAdd(pfn->ASCII85Decode);
  		pca->arrayAdd(filter_name);
  		pcd->dictPut(pfn->Filter, pca);
  		if (decode_parms)
  		{
  			pca = new XWPSCosObject(this, &cos_array_procs);
  			pca->arrayAdd("null");
  			pca->arrayAdd(decode_parms);
  			pcd->dictPut(pfn->DecodeParms, pca);
  		}
  	}
  }
  else if (!binary_okA)
  	pcd->dictPut(pfn->Filter, pfn->ASCII85Decode);
  	
  return 0;
}

int  XWPSDevicePDF::putImageFilters(XWPSCosObject *pcd,
		      						 XWPSPSDFBinaryWriter * pbw,
		      							const PSPDFImageNames *pin)
{
	return putFilters(pcd, pbw->strm, &pin->filter_names);
}

int XWPSDevicePDF::putImageValues(XWPSCosObject *pcd, 
		     							XWPSPixelImage *pic,
		     							const PSPDFImageNames *pin,
		     							XWPSCosValue *pcsvalue)
{
	XWPSColorSpace *pcs = pic->ColorSpace;
  int code;

  switch (pic->type.index) 
  {
    case 1: 
    	{
				const XWPSImage1 *pim = (const XWPSImage1 *)pic;

				if (pim->ImageMask) 
				{
	    		code = pcd->dictPut(pin->ImageMask, "true");
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
				XWPSCosObject *pca = new XWPSCosObject(this, &cos_array_procs);
				int i;
    
				for (i = 0; i < num_components; ++i) 
				{
	    		int lo, hi;

	    		if (pim->MaskColor_is_range)
						lo = pim->MaskColor[i * 2], hi = pim->MaskColor[i * 2 + 1];
	    		else
						lo = hi = pim->MaskColor[i];
	    		code = pca->arrayAdd(lo);
	    		code = pca->arrayAdd(hi);
				}
				code = pcd->dictPut("/Mask", pca);
    	}
			break;
			
    default:
			return (int)(XWPSError::RangeCheck);
  }
  return putPixelImageValues(pcd, pic, pcs, pin, pcsvalue);
}

int  XWPSDevicePDF::putLinearShading(XWPSCosObject *pscd, 
	                      const float *Coords,
		                    int num_coords, 
		                    const float *Domain,
		                    XWPSFunction *Function,
		       							const bool *Extend)
{
	int code = pscd->dictPut("/Coords", Coords, num_coords);

  if (code < 0 ||
			((Domain[0] != 0 || Domain[1] != 1) &&
	 	(code = pscd->dictPut("/Domain", Domain, 2)) < 0))
		return code;
  if (Function) 
  {
		XWPSCosValue fn_value;

		if ((code = pscd->pdev->pdfFunction(Function, &fn_value)) < 0 ||
	    (code = pscd->dictPut("/Function", &fn_value)) < 0)
	    return code;
  }
  if (Extend[0] | Extend[1]) 
  {
		char extend_str[1 + 5 + 1 + 5 + 1 + 1]; /* [bool bool] */

		sprintf(extend_str, "[%s %s]",
			(Extend[0] ? "true" : "false"),
			(Extend[1] ? "true" : "false"));
		code = pscd->dictPut("/Extend",	 (const uchar *)extend_str, strlen(extend_str));
  }
  return code;
}

int XWPSDevicePDF::putMeshShading(XWPSCosObject *pscs, XWPSShading *psh)
{
	XWPSCosObject * pscd = pscs;
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

  if (code < 0)
		return code;
		
	cs.nextInit(pmp, NULL);
  if (from_array)
  {
  	XWPSCosObject *pca = new XWPSCosObject(pscd->pdev, &cos_array_procs);
		int i;

		for (i = 0; i < 2; ++i)
	    if ((code = arrayAddInt2(pca, -32768, 32767)) < 0)
				return code;
		if (pcs->getIndex() == ps_color_space_index_Indexed) 
		{
	    is_indexed = true;
	    if ((code = arrayAddInt2(pca, -32768, 32767)) < 0)
				return code;
		} 
		else 
		{
	    is_indexed = false;
	    for (i = 0; i < num_comp; ++i)
				if ((code = arrayAddInt2(pca, -256, 255)) < 0)
		    return code;
		}
		code = pscd->dictPut("/Decode", pca);
		if (code < 0)
	    return code;
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

		code = pscd->dictPut("/Decode", pmp->Decode, 4 + pcs->numComponents() * 2);
		while (cs.s->gets(buf, sizeof(buf), &num_read), num_read > 0)
	    if ((code = pscs->streamAddBytes(buf, num_read)) < 0)
				return code;
		bits_per_coordinate = pmp->BitsPerCoordinate;
		bits_per_component = pmp->BitsPerComponent;
		bits_per_flag = -1;
  }
  
  if (code < 0 ||
			(code = pscd->dictPut("/BitsPerCoordinate",  bits_per_coordinate)) < 0 ||
			(code = pscd->dictPut("/BitsPerComponent",  bits_per_component)) < 0)
		return code;

  switch (psh->getType())
  {
  	case shading_type_Free_form_Gouraud_triangle: 
  		{
				XWPSShadingFfGtParams * params = (XWPSShadingFfGtParams *)pmp;

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
				return pscd->dictPut("/VerticesPerRow", params->VerticesPerRow);
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
				XWPSShadingTppParams *params = (XWPSShadingTppParams *)pmp;

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
  
  return pscd->dictPut("/BitsPerFlag", bits_per_flag);
}

void XWPSDevicePDF::putMatrix(const char *before,
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

void XWPSDevicePDF::putName(const uchar *nstr, uint size)
{
	putNameEscaped(strm, nstr, size, CompatibilityLevel >= 1.2);
}

void XWPSDevicePDF::putNameEscaped(XWPSStream *s, 
	                                 const uchar *nstr, 
	                                 uint size, 
	                                 bool escape)
{
	s->pputc('/');
	for (uint i = 0; i < size; ++i)
	{
		uint c = nstr[i];
		char hex[4];
		switch (c)
		{
			case '#':
				if (escape) 
				{
		    	sprintf(hex, "#%02x", c);
		    	s->pputs(hex);
		    	break;
				}
				
			default:
				if (c >= 0x21 && c <= 0x7e) 
				{
		    	s->pputc(c);
		    	break;
				}
				
			case '%':
	    case '(': case ')':
	    case '<': case '>':
	    case '[': case ']':
	    case '{': case '}':
	    case '/':
	    	if (escape) 
	    	{
		    	sprintf(hex, "#%02x", c);
		    	s->pputs(hex);
		    	break;
				}
				
			case 0:
				s->pputc('?');
		}
	}
}

int  XWPSDevicePDF::putPatternMask(XWPSColorTile *m_tile,
		     							XWPSCosObject **ppcs_mask)
{
	int w = m_tile->tmask.rep_width, h = m_tile->tmask.rep_height;
  XWPSImage1 image;
  XWPSPDFImageWriter writer;
  XWPSCosObject *pcs_image;
  long pos;
  int code;

  image.initMaskAdjust(true, false);
  setPatternImage(&image, &m_tile->tmask);
  if ((code = beginWriteImage(&writer, ps_no_id, w, h, NULL, false)) < 0 ||
			(params.MonoImage.Encode &&
	 		(code = writer.binary.CFEBinary(w, h, true)) < 0) ||
			(code = beginImageData(&writer, (XWPSPixelImage *)&image, NULL)) < 0)
		return code;
  pcs_image = writer.pres->object;
  pos = streams->strm->tell();
  if ((code = copyMaskBits(writer.binary.strm, m_tile->tmask.data + (h - 1) * m_tile->tmask.raster, 0, -m_tile->tmask.raster, w, h, 0)) < 0 ||
			(code = pcs_image->streamAddSince(pos)) < 0 ||
			(code = endImageBinary(&writer, h)) < 0 ||
			(code = endWriteImage(&writer)) < 0)
		return code;
	if (pcs_image)
		pcs_image->incRef();
  *ppcs_mask = pcs_image;
  return 0;
}

int  XWPSDevicePDF::putPattern2(XWPSDeviceColor *pdc,
		 							 const PSPSDFSetColorCommands *ppscc,
		 							 XWPSPDFResource **ppres)
{
	XWPSPattern2Instance *pinst = (XWPSPattern2Instance *)pdc->ccolor.pattern;
  XWPSShading *psh = (XWPSShading*)(pinst->templat.Shading);
  XWPSCosValue v;
  XWPSPDFResource *pres;
  XWPSPDFResource *psres;
  XWPSCosObject *pcd;
  XWPSCosObject *psco;
  int code = csPatternColored(&v);
  XWPSMatrix smat;
  
  if (code < 0)
		return code;
  code = allocResource(resourcePattern, ps_no_id, ppres, 0L);
  if (code < 0)
		return code;
    
  pres = *ppres;
  pres->object->become(&cos_dict_procs);
  pcd = pres->object;
  code = allocResource(resourceShading, ps_no_id, &psres, 0L);
  if (code < 0)
		return code;
  psco = psres->object;
  if (psh->getType() >= 4) 
  {
		psco->become(&cos_stream_procs);
		code = putMeshShading(psco, psh);
  } 
  else 
  {
		psco->become(&cos_dict_procs);
		code = putScalarShading(psco, psh);
  }
  psco->incRef();
  pinst->saved->currentMatrix(&smat);
  {
		double xscale = 72.0 / HWResolution[0], yscale = 72.0 / HWResolution[1];

		smat.xx *= xscale, smat.yx *= xscale, smat.tx *= xscale;
		smat.xy *= yscale, smat.yy *= yscale, smat.ty *= yscale;
  }
  
  if (code < 0 ||
			(code = pcd->dictPut("/PatternType", 2)) < 0 ||
			(code = pcd->dictPut("/Shading", psco)) < 0 ||
			(code = cos_dict_put_matrix(pcd, "/Matrix", &smat)) < 0)
		return code;
  v.write(this);
  strm->print(" %s", ppscc->setcolorspace);
  return 0;
}

int  XWPSDevicePDF::putPixelImageValues(XWPSCosObject *pcd, 
			   									 XWPSPixelImage *pim,
			   										XWPSColorSpace *pcs,
			   										const PSPDFImageNames *pin,
			   										XWPSCosValue *pcsvalue)
{
	int num_components;
  float indexed_decode[2];
  const float *default_decode = NULL;
  int code;

  if (pcs) 
  {
		code = pcd->dictPut(pin->ColorSpace, pcsvalue);
		if (code < 0)
			return code;
			
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
    
  code = pcd->dictPut(pin->Width, pim->Width);
  if (code < 0)
		return code;
  code = pcd->dictPut(pin->Height, pim->Height);
  if (code < 0)
		return code;
  code = pcd->dictPut(pin->BitsPerComponent, pim->BitsPerComponent);
  if (code < 0)
		return code;
  {
		int i;

		for (i = 0; i < num_components * 2; ++i)
	    if (pim->Decode[i] !=	(default_decode ? default_decode[i] : i & 1))
				break;
		if (i < num_components * 2) 
		{
	    XWPSCosObject *pca = new XWPSCosObject(this, &cos_array_procs);
	    for (i = 0; i < num_components * 2; ++i)
				code = pca->arrayAdd(pim->Decode[i]);
	    code = pcd->dictPut(pin->Decode, pca);
		}
  }
  if (pim->Interpolate)
		code = pcd->dictPut(pin->Interpolate, "true");
  return code;
}

int  XWPSDevicePDF::putScalarShading(XWPSCosObject *pscd, XWPSShading *psh)
{
	int code = putShadingCommon(pscd, psh);

  if (code < 0)
		return code;
  switch (psh->getType()) 
  {
    case shading_type_Function_based: 
    	{
				XWPSShadingFbParams * params = (XWPSShadingFbParams *)psh->params;
				XWPSCosValue fn_value;

				if ((code = pscd->dictPut("/Domain", params->Domain, 4)) < 0 ||
	    		(code = pscd->pdev->pdfFunction(params->Function, &fn_value)) < 0 ||
	    		(code = pscd->dictPut("/Function", &fn_value)) < 0 ||
	    		(code = cos_dict_put_matrix(pscd, "/Matrix", &params->Matrix)) < 0)
	    			return code;
				return 0;
    	}
    	
    case shading_type_Axial: 
    	{
				XWPSShadingAParams *params = (XWPSShadingAParams *)&psh->params;

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

int  XWPSDevicePDF::putShadingCommon(XWPSCosObject *pscd, XWPSShading *psh)
{
	PSShadingType type = psh->getType();
  XWPSColorSpace *pcs = psh->params->ColorSpace;
  int code = pscd->dictPut("/ShadingType", (int)type);
  XWPSCosValue cs_value;
  
  if (code < 0 ||
			(psh->params->AntiAlias &&
	 		(code = pscd->dictPut("/AntiAlias", "true")) < 0) ||
			(code = pscd->pdev->colorSpace(&cs_value, pcs,	&pdf_color_space_names, false)) < 0 ||
			(code = pscd->dictPut("/ColorSpace", &cs_value)) < 0)
		return code;
  if (psh->params->Background) 
  {
		code = pscd->dictPut("/Background",  psh->params->Background->paint.values, pcs->numComponents());
		if (code < 0)
	    return code;
  }
  if (psh->params->have_BBox) 
  {
		float bbox[4];

		bbox[0] = psh->params->BBox.p.x;
		bbox[1] = psh->params->BBox.p.y;
		bbox[2] = psh->params->BBox.q.x;
		bbox[3] = psh->params->BBox.q.y;
		code = pscd->dictPut("/BBox", bbox, 4);
		if (code < 0)
	    return code;
  }
  return 0;
}

int XWPSDevicePDF::putString(const uchar * str, uint size)
{
	writeString(strm, str, size); return 0;
}

int  XWPSDevicePDF::putUncoloredPattern(XWPSDeviceColor *pdc,
			  									 const PSPSDFSetColorCommands *ppscc,
			                     XWPSPDFResource **ppres)
{
	XWPSColorTile *m_tile = pdc->mask.m_tile;
  XWPSCosValue v;
  XWPSStream *s = strm;
  int code = csPatternUnColored(&v);
  XWPSCosObject *pcs_image;
  XWPSDeviceColor dc_pure;
  static const PSPSDFSetColorCommands no_scc = {0, 0, 0};

  if (code < 0 ||	(code = putPatternMask(m_tile, &pcs_image)) < 0 ||
			(code = pdfPattern(pdc, NULL, m_tile, pcs_image, ppres)) < 0)
		return code;
    
  v.write(this);
  s->print(" %s ", ppscc->setcolorspace);
  dc_pure.setPure(pdc->pureColor());
  setColor(&dc_pure, &no_scc);
  return 0;
}

int  XWPSDevicePDF::referNamed(PSParamString * pname_orig,	XWPSCosObject **ppco)
{
	PSParamString *pname = pname_orig;
  int code = findNamed(pname, ppco);
  char page_name_chars[6 + 10 + 2];
  PSParamString pnstr;
  int page_number;
  
  if (code != XWPSError::Undefined)
		return code;
		
	if (pname->size >= 7 &&	sscanf((const char *)pname->data, "{Page%d}", &page_number) == 1)
		goto cpage;
		
	if (pname->pdfKeyEq("{ThisPage}"))
		page_number = next_page + 1;
  else if (pname->pdfKeyEq("{NextPage}"))
		page_number = next_page + 2;
  else if (pname->pdfKeyEq("{PrevPage}"))
		page_number = next_page;
  else 
  {
		code = createNamed(pname, &cos_generic_procs, ppco, 0L);
		return (code < 0 ? code : 1);
  }
  if (page_number <= 0)
		return code;
  sprintf(page_name_chars, "{Page%d}", page_number);
  pnstr.fromString(page_name_chars);
  pname = &pnstr;
  code = findNamed(pname, ppco);
  if (code != XWPSError::Undefined)
		return code;
 
cpage:
  if (pageId(page_number) <= 0)
		return (int)(XWPSError::RangeCheck);
  *ppco = pages[page_number - 1].Page;
  return 0;
}

int  XWPSDevicePDF::replaceNames(PSParamString * from, PSParamString * to)
{
	uchar *start = from->data;
  uchar *end = start + from->size;
  uchar *scan;
  uint size = 0;
  XWPSCosObject *pco;
  bool any = false;
  uchar *sto;
  char ref[1 + 10 + 5 + 1];
  for (scan = start; scan < end;)
  {
  	uchar *sname;
		uchar *next =  (uchar*)pdfmarkNextObject(scan, end, (const uchar**)&sname, &pco);

		size += sname - scan;
		if (pco) 
		{
	    sprintf(ref, " %ld 0 R ", pco->id);
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
		if (pco) 
		{
	    sprintf(ref, " %ld 0 R ", pco->id);
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

int  XWPSDevicePDF::resetColor(XWPSDeviceColor *pdc,
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

void XWPSDevicePDF::resetGraphics()
{
	if (!fill_color)
		fill_color = new XWPSDeviceColor;		
	
	if (!stroke_color)
		stroke_color = new XWPSDeviceColor;		
		
	fill_color->setPure(0);
	stroke_color->setPure(0);
	
	if (!state)
		state = new XWPSImagerState;
	else
		state->resetLineParams();
	
	state->i_ctx_p = context_state;
	fill_overprint = false;
  stroke_overprint = false;
  resetText();
}

void XWPSDevicePDF::resetPage()
{
	contents_id = 0;
  resetGraphics();
  procsets = NoMarks;
  
  text.reset();
}

void XWPSDevicePDF::resetText()
{
	text.reset();
}

bool XWPSDevicePDF::scanForStandardFonts(XWPSFontDir *dir)
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

int  XWPSDevicePDF::separationColorSpace(XWPSCosObject *pca, 
	                          const char *csname,
			                      XWPSCosValue *snames,
			                      XWPSColorSpace *alt_space,
			                      XWPSFunction *pfn,
			                      const PSPDFColorSpaceNames *pcsn)
{
	XWPSCosValue v(csname);
	int code;

  if ((code = pca->arrayAdd(&v)) < 0 ||
			(code = pca->arrayAdd(snames)) < 0 ||
			(code = colorSpace(&v, alt_space, pcsn, false)) < 0 ||
			(code = pca->arrayAdd(&v)) < 0 ||
			(code = pdfFunction(pfn, &v)) < 0 ||
			(code = pca->arrayAdd(&v)) < 0)
		return code;
  return 0;
}

int  XWPSDevicePDF::separationName(XWPSCosValue *pvalue, ulong sname)
{
	static const char *const snames[] = {
		"Default", "Gray", "Red", "Green", "Blue", 
    "Cyan", "Magenta", "Yellow", "Black" };
    
  static char buf[sizeof(ulong) * 8 / 3 + 2];
  const char *str;
  uint len;
  uchar *chars;

  if ((ulong)sname < 9) 
  {
		str = snames[(int)sname];
  } 
  else 
  {
		sprintf(buf, "S%ld", (ulong)sname);
		str = buf;
  }
  len = strlen(str);
  chars = new uchar[len + 1];
  chars[0] = '/';
  memcpy(chars + 1, str, len);
  pvalue->stringValue(chars, len + 1);
  delete [] chars;
  return 0;
}

int  XWPSDevicePDF::setDrawingColor(XWPSDeviceColor *pdc,
		      						              XWPSDeviceColor * pdcolor,
		      						              const PSPSDFSetColorCommands *ppscc)
{
	if (pdcolor->equal(pdc))
		return 0;
  return resetColor(pdc, pdcolor, ppscc);
}

int  XWPSDevicePDF::setFontAndSize(XWPSPDFFont * font, float size)
{
	if (font != text.font || size != text.size) 
	{
		int code = openPage(PDF_IN_TEXT);
		XWPSStream *s = strm;

		if (code < 0)
	    return code;
		s->print("/%s ", font->frname);
		s->print("%g Tf\n", size);
		text.font = font;
		text.size = size;
  }
  font->used_on_page = true;
  return 0;
}

int  XWPSDevicePDF::setPureColor(ulong color,  
	                               XWPSDeviceColor * pdcolor,
		                            const PSPSDFSetColorCommands *ppscc)
{
	XWPSDeviceColor dcolor;

  if (pdcolor->isPure() && pdcolor->pureColor() == color)
		return 0;
  dcolor.setPure(color);
  return resetColor(&dcolor, pdcolor, ppscc);
}

int XWPSDevicePDF::streamToNone()
{
	XWPSStream *s = strm;
	if (compression == pdf_compress_Flate)
	{
		XWPSStream *fs = s->strm;
		s->close();
		delete s;
		strm = s = fs;
	}
	
	long length = tell() - contents_pos;
  s->pputs("endstream\n");
  endObj();
  openObj(contents_length_id);
  s->print("%ld\n", length);
  endObj();
  return PDF_IN_NONE;
}

int XWPSDevicePDF::streamToText()
{
	strm->print("q %g 0 0 %g 0 0 cm BT\n", HWResolution[0] / 72.0, HWResolution[1] / 72.0);
  procsets = (PSPDFProcset)((int)procsets | (int)Text);
  text.matrix.reset();
  text.line_start.x = text.line_start.y = 0;
  text.buffer_count = 0;
  return PDF_IN_TEXT;
}

int XWPSDevicePDF::stringToText()
{
	putString(text.buffer, text.buffer_count);
  strm->pputs((text.use_leading ? "'\n" : "Tj\n"));
  text.use_leading = false;
  text.buffer_count = 0;
  return PDF_IN_TEXT;
}

long XWPSDevicePDF::tell()
{
	long pos = strm->tell();

  if (strm == asides->strm)
		pos += ASIDES_BASE_POSITION;
    
  return pos;
}

int XWPSDevicePDF::textToStream()
{
	strm->pputs("ET Q\n");
  resetText();
  return PDF_IN_STREAM;
}

int XWPSDevicePDF::updateAlpha(XWPSImagerState *pis,
		 							const char *ca_format, 
		 							XWPSPDFResource **ppres)
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
  strm->print("/AIS %s", ais);
  strm->print(ca_format, alpha);
  return 0;
}

int XWPSDevicePDF::updateHalftone(XWPSImagerState *pis, char *hts)
{
	XWPSHalftone *pht = pis->halftone;
  XWPSDeviceHalftone *pdht = pis->dev_ht;
  int code;
  long id;

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
  sprintf(hts, "/HT %ld 0 R", id);
  halftone_id = pis->dev_ht->id;
  return code;
}

int XWPSDevicePDF::updateTransfer(XWPSImagerState *pis, char *trs)
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
	    code = writeTransfer(pis->set_transfer.u.indexed[0], "/TR", trs);
	    if (code < 0)
				return code;
		} 
		else 
		{
	    strcpy(trs, "/TR[");
	    for (i = 0; i < 4; ++i) 
	    {
				code = writeTransferMap(pis->set_transfer.u.indexed[i], 0, false, "", trs + strlen(trs));
				if (code < 0)
		    	return code;
	    }
	    strcat(trs, "]");
		}
		memcpy(transfer_ids, transfer_idsA, sizeof(transfer_ids));
  }
  return code;
}

int XWPSDevicePDF::writeColorScreenHalftone(XWPSColorScreenHalftone *pcsht,
			                         XWPSDeviceHalftone *pdht, 
			                         long *pid)
{
	int i;
  XWPSStream *s;
  long ht_ids[4];

  for (i = 0; i < 4; ++i) 
  {
		int code = writeScreenHalftone(pcsht->screens.indexed[i],  &pdht->components[i].corder, &ht_ids[i]);
		if (code < 0)
	    return code;
  }
  *pid = beginSeparate();
  s = strm;
  s->print("<</Type/Halftone/HalftoneType 5/Default %ld 0 R\n", ht_ids[3]);
  s->print("/Red %ld 0 R/Cyan %ld 0 R", ht_ids[0], ht_ids[0]);
  s->print("/Green %ld 0 R/Magenta %ld 0 R", ht_ids[1], ht_ids[1]);
  s->print("/Blue %ld 0 R/Yellow %ld 0 R", ht_ids[2], ht_ids[2]);
  s->print("/Gray %ld 0 R/Black %ld 0 R", ht_ids[3], ht_ids[3]);
  return endSeparate();
}

int XWPSDevicePDF::writeEmbeddedFont(XWPSPDFFontDescriptor *pfd)
{
	XWPSFont *font = pfd->base_font;
  XWPSString font_name;
  uchar *fnchars = pfd->FontName.chars;
  uint fnsize = pfd->FontName.size;
  bool do_subset = pfd->subset_ok && params.SubsetFonts &&	params.MaxSubsetPct > 0;
  long FontFile_id = pfd->FontFile_id;
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
		makeSubsetPrefix(fnchars, FontFile_id);
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
	    	code = embedFontType1((XWPSFontType1 *)font,FontFile_id, subset_list,subset_size, &font_name);
	    	break;
	    
			case ft_TrueType:
	    	if (do_subset) 
	    	{
					subset_size = font->subsetGlyphs(subset_glyphs,pfd->chars_used.data);
					subset_list = subset_glyphs;
	    	}
	    	code = embedFontType42((XWPSFontType42 *)font, FontFile_id, subset_list, subset_size, &font_name);
	    	break;
	    	
			case ft_CID_encrypted:
	    	if (do_subset) 
	    	{
					subset_size = pfd->chars_used.size << 3;
					subset_bits = pfd->chars_used.data;
	    	}
	    	code = embedFontCid0((XWPSFontCid0 *)font, FontFile_id, subset_bits, subset_size, &font_name);
	    	break;
	    	
			case ft_CID_TrueType:
		    if (do_subset) 
		    {
					subset_size = pfd->chars_used.size << 3;
					subset_bits = pfd->chars_used.data;
	    	}
	    	code = embedFontCid2((XWPSFontCid2 *)font, FontFile_id, subset_bits, subset_size, &font_name);
	    	break;
			default:
	    	code = (int)(XWPSError::RangeCheck);
		}
		font->FontMatrix = save_mat;
  }
  return code;
}

int XWPSDevicePDF::writeFontBbox(XWPSIntRect *pbox)
{
	XWPSStream *s = strm;

  s->print("/FontBBox[%d %d %d %d]", pbox->p.x, pbox->p.y, pbox->q.x, pbox->q.y);
  return 0;
}

int XWPSDevicePDF::writeFontDescriptor(XWPSPDFFontDescriptor *pfd)
{
	XWPSFont *font = pfd->getBaseFont();
  bool is_subset =	hasSubsetPrefix(pfd->FontName.chars, pfd->FontName.size);
  long cidset_id = 0;
  XWPSStream *s;
  int code = 0;
  if (font && is_subset)
  {
  	switch (pfd->values.FontType)
  	{
  		case ft_CID_encrypted:
			case ft_CID_TrueType:
				{
					cidset_id = beginSeparate();
	    		s = strm;
	    		s->print("<</Length %d>>stream\n", (int)pfd->chars_used.size);
	    		for (int i = 0; i < pfd->chars_used.size; ++i)
						s->pputc(byte_reverse_bits[pfd->chars_used.data[i]]);
	    		s->pputs("endstream\n");
	    		endSeparate();
				}
				
			default:
				break;
  	}
  }
  
  openSeparate(pfd->fontDescriptorId());
  s = strm;
  s->pputs("<</Type/FontDescriptor/FontName");
  putName(pfd->FontName.chars, pfd->FontName.size);
  if (font)
  {
  	writeFontBbox(&pfd->values.FontBBox);
  	
  	{
  		PSPrinterParams params={0, 0, 0, "\n", 0};
  		XWPSPrinterParamList rlist(s, &params);
  	
  		static const PSParamItem required_items[] = {
  			{"Ascent", ps_param_type_int, _OFFSET_OF_(XWPSPDFFontDescriptorValues,Ascent)},
  			{"CapHeight", ps_param_type_int, _OFFSET_OF_(XWPSPDFFontDescriptorValues,CapHeight)},
  			{"Descent", ps_param_type_int, _OFFSET_OF_(XWPSPDFFontDescriptorValues,Descent)},
  			{"ItalicAngle", ps_param_type_int, _OFFSET_OF_(XWPSPDFFontDescriptorValues,ItalicAngle)},
  			{"StemV", ps_param_type_int, _OFFSET_OF_(XWPSPDFFontDescriptorValues,StemV)},
  			{0, 0, 0}};
  	
  		static const PSParamItem optional_items[] = {
  			{"AvgWidth", ps_param_type_int, _OFFSET_OF_(XWPSPDFFontDescriptorValues,AvgWidth)},
  			{"Leading", ps_param_type_int, _OFFSET_OF_(XWPSPDFFontDescriptorValues,Leading)},
  			{"MaxWidth", ps_param_type_int, _OFFSET_OF_(XWPSPDFFontDescriptorValues,MaxWidth)},
  			{"MissingWidth", ps_param_type_int, _OFFSET_OF_(XWPSPDFFontDescriptorValues,MissingWidth)},
  			{"StemH", ps_param_type_int, _OFFSET_OF_(XWPSPDFFontDescriptorValues,StemH)},
  			{"XHeight", ps_param_type_int, _OFFSET_OF_(XWPSPDFFontDescriptorValues,XHeight)},
  			{0, 0, 0}};
  	
  		int Flags = pfd->values.Flags;
	  	XWPSPDFFontDescriptorValues defaults;
	  	if (pfd->values.FontType == ft_TrueType && hasSubsetPrefix(pfd->FontName.chars, pfd->FontName.size))
				Flags = (Flags & ~(FONT_IS_ADOBE_ROMAN)) | FONT_IS_SYMBOLIC;
			
			rlist.writeInt(context_state, "Flags", &Flags);
			rlist.writeItems(context_state, &pfd->values, NULL, (PSParamItem*)&required_items[0]);
			rlist.writeItems(context_state, &pfd->values, &defaults, (PSParamItem*)&optional_items[0]);
			rlist.release();
		}
		
		if (is_subset)
		{
			switch (pfd->values.FontType)
			{
				case ft_CID_encrypted:
	    	case ft_CID_TrueType:
					s->print("/CIDSet %ld 0 R\n", cidset_id);
					break;
					
	    	case ft_composite:
					return (int)(XWPSError::RangeCheck);
						
				default:
					{
						ulong subset_glyphs[256];
						uint subset_size = font->subsetGlyphs(subset_glyphs, pfd->chars_used.data);
						s->pputs("/CharSet(");
						for (int i = 0; i < subset_size; ++i)
						{
							uint len;
		    			const char *str = font->glyphName(subset_glyphs[i], &len);
		    			if (bytes_compare((const uchar *)str, len, (const uchar *)".notdef", 7))
								putName((const uchar *)str, len);
						}
						s->pputs(")\n");
					}
					break;
			}
		}
		
		if (pfd->FontFile_id)
		{
			const char *FontFile_key;
			switch (pfd->values.FontType)
			{
				case ft_TrueType:
	    	case ft_CID_TrueType:
					FontFile_key = "/FontFile2";
					break;
					
	    	default:
					code = (int)(XWPSError::RangeCheck);
						
				case ft_encrypted:
					if (CompatibilityLevel < 1.2) 
					{
		    		FontFile_key = "/FontFile";
		    		break;
					}
					
				case ft_encrypted2:
	    	case ft_CID_encrypted:
					FontFile_key = "/FontFile3";
					break;
			}
			
			s->pputs(FontFile_key);
	    s->print(" %ld 0 R", pfd->FontFile_id);
		}
  }
  
  s->pputs(">>\n");
  endSeparate();
  return code;
}

int XWPSDevicePDF::writeFontResource(XWPSPDFFont *pef,
												             XWPSString *,
												             XWPSString *pbfname)
{
	XWPSPDFFontDescriptor *pfd = pef->FontDescriptor;
  static const char *const encoding_names[] = {
		"StandardEncoding", "ISOLatin1Encoding", "SymbolEncoding",
  	"DingbatsEncoding", "WinAnsiEncoding", "MacRomanEncoding",
  	"MacExpertEncoding"
    };
    
  openSeparate(pef->fontId());
  XWPSStream * s = strm;
  switch (pef->FontType)
  {
  	case ft_composite:
			s->pputs("<</Subtype/Type0");
			goto bfname;
			
    case ft_encrypted:
    case ft_encrypted2:
    	s->pputs("<</Subtype/Type1");

bfname:
			s->pputs("/BaseFont");
			putName(pbfname->data, pbfname->size);
			break;
			
		case ft_CID_encrypted:
			s->pputs("<</Subtype/CIDFontType0");
			{
				XWPSFontCid0 * cidf = (XWPSFontCid0*)(pfd->getBaseFont());
				writeCIDSystemInfo((XWPSCidSystemInfo*)(cidf->getCIDSysInfo()));
			}
			goto bfname;
			
		case ft_CID_TrueType:
			s->pputs("<</Subtype/CIDFontType2");
			{
				XWPSFontCid2 * cidf = (XWPSFontCid2*)(pfd->getBaseFont());
				writeCIDSystemInfo((XWPSCidSystemInfo*)(cidf->getCIDSysInfo()));
			}
			goto ttname;
			
		case ft_TrueType:
			s->pputs("<</Subtype/TrueType");
ttname:
			s->pputs("/BaseFont");
			putName(pbfname->data, pbfname->size);
			break;
			
		default:
			return (int)(XWPSError::RangeCheck);
  }
  
  s->print("/Type/Font/Name/R%ld", pef->fontId());
  if (pef->index < 0 || pfd->base_font || pfd->FontFile_id)
		s->print("/FontDescriptor %ld 0 R", pfd->fontDescriptorId());
  if (pef->write_Widths)
		writeWidths(pef->FirstChar, pef->LastChar, pef->Widths);
		
	if (pef->Differences) 
	{
		long diff_id = objRef();
		int prev = 256;

		s->print("/Encoding %ld 0 R>>\n", diff_id);
		endSeparate();
		openSeparate(diff_id);
		s = strm;
		s->pputs("<</Type/Encoding");
		if (pef->BaseEncoding != ENCODING_INDEX_UNKNOWN)
	    s->print("/BaseEncoding/%s", encoding_names[pef->BaseEncoding]);
		s->pputs("/Differences[");
		for (int i = 0; i < 256; ++i)
	    if (pef->Differences[i].data != 0) 
	    {
				if (i != prev + 1)
		    	s->print("\n%d", i);
				putName(pef->Differences[i].data, pef->Differences[i].size);
				prev = i;
	    }
		s->pputs("]");
  } 
  else if (pef->BaseEncoding != ENCODING_INDEX_UNKNOWN) 
		s->print("/Encoding/%s", encoding_names[pef->BaseEncoding]);
  s->pputs(">>\n");
  return endSeparate();
}

int XWPSDevicePDF::writeFontResources()
{	
	for (int j = 0; j < NUM_RESOURCE_CHAINS; ++j)
	{
		XWPSPDFFont * ppf = (XWPSPDFFont *)resources[resourceFont].chains[j];
		for (; ppf != 0; ppf = (XWPSPDFFont *)(ppf->next))
		{
			if (ppf->isSynthesized())
				writeSynthesizedType3(ppf);
	    else if (!ppf->skip)
	    {
	    	XWPSString font_name;
	    	XWPSPDFFontDescriptor * pfd = ppf->FontDescriptor;
				font_name.data = pfd->FontName.chars;
				font_name.size = pfd->FontName.size;
				if (hasSubsetPrefix(font_name.data, font_name.size))
		    	ppf->write_Widths = true;
				writeFontResource(ppf, &font_name, &font_name);
	    }
		}
		
		XWPSPDFFontDescriptor * pfd = (XWPSPDFFontDescriptor *)resources[resourceFontDescriptor].chains[j];
		for (; pfd != 0; pfd = (XWPSPDFFontDescriptor *)(pfd->next))
		{
	    if (!pfd->written) 
	    {
				finalizeFontDescriptor(pfd);
	    }
		}
	}
	return 0;
}

int XWPSDevicePDF::writeFunction(XWPSFunction *pfn, long *pid)
{
	XWPSCosValue value;
  int code = pdfFunction(pfn, &value);

  if (code < 0)
  {
  	value.contents.object = 0;
		return code;
	}
    
  *pid = value.contents.object->id;
  value.contents.object = 0;
  return 0;
}

int XWPSDevicePDF::writeMultipleHalftone(XWPSMultipleHalftone *pmht,
			    										XWPSDeviceHalftone *pdht, 
			    										long *pid)
{
	XWPSStream *s;
  int i, code;
  long *ids;

  ids = new long[pmht->num_comp * sizeof(long)];
  for (i = 0; i < pmht->num_comp; ++i) 
  {
		XWPSHalftoneComponent *phtc = &pmht->components[i];
		XWPSHTOrder *porder =  (pdht->components == 0 ? &pdht->order :  &pdht->components[i].corder);

		switch (phtc->type) 
		{
			case ht_type_spot:
	    	code = writeSpotHalftone(phtc->params.spot, porder, &ids[i]);
	    	break;
	    	
			case ht_type_threshold:
	    	code = writeThresholdHalftone(phtc->params.threshold,porder, &ids[i]);
	    	break;
	    	
			case ht_type_threshold2:
	    	code = writeThreshold2Halftone(phtc->params.threshold2,porder, &ids[i]);
	    	break;
	    	
			default:
	    code = XWPSError::RangeCheck;
		}
		if (code < 0) 
		{
			delete [] ids;
	    return code;
		}
  }
  *pid = beginSeparate();
  s = strm;
  s->pputs("<</Type/Halftone/HalftoneType 5\n");
  for (i = 0; i < pmht->num_comp; ++i) 
  {
		XWPSHalftoneComponent *phtc = &pmht->components[i];
		XWPSCosValue value;

		code = separationName(&value, phtc->cname);
		if (code < 0)
		{
			delete [] ids;
	    return code;
	  }
		value.write(this);
		s->print(" %ld 0 R\n", ids[i]);
  }
  s->pputs(">>\n");
  delete [] ids;
  return endSeparate();
}

int XWPSDevicePDF::writePage(int page_num)
{
	long page_id = pageId(page_num);
  XWPSPDFPage *page = &pages[page_num - 1];
  openObj(page_id);
  XWPSStream * s = strm;
  s->print("<</Type/Page/MediaBox [0 0 %d %d]\n", page->MediaBox.x, page->MediaBox.y);
  if (page->text_rotation.Rotate >= 0)
		s->print("/Rotate %d", page->text_rotation.Rotate);
  s->print("/Parent %ld 0 R\n", Pages->id);
  s->pputs("/Resources<</ProcSet[/PDF");
  if (page->procsets & ImageB)
		s->pputs(" /ImageB");
  if (page->procsets & ImageC)
		s->pputs(" /ImageC");
  if (page->procsets & ImageI)
		s->pputs(" /ImageI");
  if (page->procsets & Text)
		s->pputs(" /Text");
  s->pputs("]\n");
  for (int i = 0; i < resourceFont; ++i) 
  {
  	if (page->resource_ids[i]) 
  	{
			s->print("/%s ", resource_type_names[i]);
			s->print("%ld 0 R\n", page->resource_ids[i]);
	  }
	}
	
	if (page->fonts_id)
		s->print("/Font %ld 0 R\n", page->fonts_id);
  s->pputs(">>\n");
  if (page->Annots) 
  {
		s->pputs("/Annots");
		page->Annots->write(this);
  }
  
  if (page->contents_id == 0)
		s->pputs("/Contents []\n");
  else
		s->print("/Contents %ld 0 R\n", page->contents_id);
		
	page->Page->elementsWrite(strm, this);
	s->pputs(">>\n");
  endObj();
  return 0;
}

int XWPSDevicePDF::writeResourceObjects(PSPDFResourceType rtype)
{
	for (int j = 0; j < NUM_RESOURCE_CHAINS; ++j) 
	{
		XWPSPDFResource *pres = resources[rtype].chains[j];

		for (; pres != 0; pres = pres->next)
		{
	    if (!pres->named && !pres->object->written)
				pres->object->writeObject(this);
		}
  }
  
  for (int j = 0; j < NUM_RESOURCE_CHAINS; ++j) 
  {
		XWPSPDFResource **prev = &resources[rtype].chains[j];
		XWPSPDFResource *pres;

		while ((pres = *prev) != 0) 
		{
	    if (pres->named) 
	    {
				prev = &pres->next;
	    } 
	    else 
	    {
	    	if (pres->object->decRef() == 0)
				  delete pres->object;
				pres->object = 0;
				*prev = pres->next;
	    }
		}
  }

  return 0;
}

void XWPSDevicePDF::writeCIDSystemInfo(XWPSCidSystemInfo *pcidsi)
{
	XWPSStream *s = strm;

  s->pputs("/CISystemInfo<<\n/Registry");
  s->writePSString(pcidsi->Registry.data, pcidsi->Registry.size, PRINT_HEX_NOT_OK);
  s->pputs("\n/Ordering");
  s->writePSString(pcidsi->Ordering.data, pcidsi->Ordering.size, PRINT_HEX_NOT_OK);
  s->print("\n/Supplement %d\n>>\n", pcidsi->Supplement);
}

int  XWPSDevicePDF::writeScreenHalftone(XWPSScreenHalftone *psht,
			  									 XWPSHTOrder *porder, 
			  									 long *pid)
{
	XWPSSpotHalftone spot;

  spot.screen = *psht;
  spot.accurate_screens = false;
  return writeSpotHalftone(&spot, porder, pid);
}

int  XWPSDevicePDF::writeSpotFunction(XWPSHTOrder *porder,	long *pid)
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

int XWPSDevicePDF::writeSpotHalftone(XWPSSpotHalftone *psht,
			                   XWPSHTOrder *porder, 
			                   long *pid)
{
	char trs[17 + MAX_FN_CHARS + 1];
  int code = writeTransfer(porder->transfer, "/TransferFunction", trs);
  long id, spot_id;
  XWPSStream *s;
  int i = _COUNT_OF_HT_FUNCS_;

  if (code < 0)
		return code;
		
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
		
  *pid = id = beginSeparate();
  s = strm;
  s->print("<</Type/Halftone/HalftoneType 1/Frequency %g/Angle %g",
	     			psht->screen.actual_frequency, psht->screen.actual_angle);
  if (i < _COUNT_OF_HT_FUNCS_)
		s->print("/SpotFunction/%s", ht_functions[i].fname);
  else
		s->print("/SpotFunction %ld 0 R", spot_id);
  s->pputs(trs);
  if (psht->accurate_screens)
		s->pputs("/AccurateScreens true");
  s->pputs(">>\n");
  return endSeparate();
}

int XWPSDevicePDF::writeSynthesizedType3(XWPSPDFFont *pef)
{
	XWPSIntRect bbox;
  int widths[256];
  memset(widths, 0, sizeof(widths));
  openSeparate(pef->fontId());
  XWPSStream * s = strm;
  s->print("<</Type/Font/Name/%s/Subtype/Type3", pef->frname);
  s->print("/Encoding %ld 0 R/CharProcs", embedded_encoding_id);
  s->pputs("<<");
  XWPSPDFCharProc * pcp = pef->char_procs;
  for (; pcp; pcp = pcp->char_next)
  {
  	bbox.p.y = qMin(bbox.p.y, pcp->y_offset);
	  bbox.q.x = qMax(bbox.q.x, pcp->width);
	  bbox.q.y = qMax(bbox.q.y, pcp->height + pcp->y_offset);
	  widths[pcp->char_code] = pcp->x_width;
	  s->print("/a%ld\n%ld 0 R", (long)pcp->char_code, pcp->charProcId());
  }
  for (int w = 0; w < (sizeof(pef->spaces) / sizeof(pef->spaces[0])); ++w)
  {
  	uchar ch = pef->spaces[w];
	  if (ch) 
	  {
			s->print("/a%ld\n%ld 0 R", (long)ch, space_char_ids[w]);
			widths[ch] = w + X_SPACE_MIN;
	  }
  }
  s->pputs(">>");
  writeFontBbox(&bbox);
  s->pputs("/FontMatrix[1 0 0 1 0 0]");
  writeWidths(0, pef->num_chars - 1, widths);
  s->pputs(">>\n");
  endSeparate();
  return 0;
}

int XWPSDevicePDF::writeTransfer(XWPSTransferMap *map,
		   							const char *key, 
		   							char *ids)
{
	return writeTransferMap(map, 0, true, key, ids);
}

int XWPSDevicePDF::writeTransferMap(XWPSTransferMap *map,
		       						 int range0, 
		       						 bool check_identity,
		                   const char *key, 
		                   char *ids)
{
	XWPSFunctionSdParams params;
  XWPSFunction *pfn;
  long id;
  int code;

  if (map == 0) 
  {
		*ids = 0;	
		return 0;
  }
    
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
	    strcpy(ids, key);
	    strcat(ids, "/Identity");
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
  sprintf(ids, "%s %ld 0 R", key, id);
  return 0;
}

int XWPSDevicePDF::writeThresholdHalftone(XWPSThresholdHalftone *ptht,
			     									  XWPSHTOrder *porder, 
			     									 long *pid)
{
	char trs[17 + MAX_FN_CHARS + 1];
  int code = writeTransfer(porder->transfer, "/TransferFunction", trs);
  long id = beginSeparate();
  XWPSStream *s = strm;
  XWPSPDFDataWriter writer;

  if (code < 0)
		return code;
  *pid = id;
  s->print("<</Type/Halftone/HalftoneType 6/Width %d/Height %d", ptht->width, ptht->height);
  s->pputs(trs);
  code = beginData(&writer);
  if (code < 0)
		return code;
  writer.binary.strm->write(ptht->thresholds.data, ptht->thresholds.size);
  return writer.endData();
}

int XWPSDevicePDF::writeThreshold2Halftone(XWPSThresholdHalftone2 *ptht,
			                        XWPSHTOrder *porder, 
			                        long *pid)
{
	char trs[17 + MAX_FN_CHARS + 1];
  int code = writeTransfer(porder->transfer, "/TransferFunction", trs);
  long id = beginSeparate();
  XWPSStream *s = strm;
  XWPSPDFDataWriter writer;

  if (code < 0)
		return code;
  *pid = id;
  s->print("<</Type/Halftone/HalftoneType 16/Width %d/Height %d", ptht->width, ptht->height);
  if (ptht->width2 && ptht->height2)
		s->print("/Width2 %d/Height2 %d", ptht->width2, ptht->height2);
  s->pputs(trs);
  code = beginData(&writer);
  if (code < 0)
		return code;
  s = writer.binary.strm;
  if (ptht->bytes_per_sample == 2)
		s->write(ptht->thresholds.data, ptht->thresholds.size);
  else 
  {
		int i;

		for (i = 0; i < ptht->thresholds.size; ++i) 
		{
	    uchar b = ptht->thresholds.data[i];

	    s->pputc(b);
	    s->pputc(b);
		}
  }
  return writer.endData();
}

int XWPSDevicePDF::writeWidths(int first, int last, const int widths[256])
{
	XWPSStream * s = strm;
	s->print("/FirstChar %d/LastChar %d/Widths[", first, last);
  for (int i = first; i <= last; ++i)
		s->print((i & 15 ? " %d" : "\n%d"), widths[i]);
  s->pputs("]\n");
  return 0;
}
