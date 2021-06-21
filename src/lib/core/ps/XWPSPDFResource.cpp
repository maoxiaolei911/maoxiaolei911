/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include "XWPSError.h"
#include "XWPSFont.h"
#include "XWPSCosObject.h"
#include "XWPSPDFResource.h"

const PSPDFStandardFont pdf_standard_fonts[] = {
  {"Courier", ENCODING_INDEX_STANDARD},
  {"Courier-Bold", ENCODING_INDEX_STANDARD},
  {"Courier-Oblique", ENCODING_INDEX_STANDARD},
  {"Courier-BoldOblique", ENCODING_INDEX_STANDARD},
  {"Helvetica", ENCODING_INDEX_STANDARD},
  {"Helvetica-Bold", ENCODING_INDEX_STANDARD},
  {"Helvetica-Oblique", ENCODING_INDEX_STANDARD},
  {"Helvetica-BoldOblique", ENCODING_INDEX_STANDARD},
  {"Symbol", ENCODING_INDEX_SYMBOL},
  {"Times-Roman", ENCODING_INDEX_STANDARD},
  {"Times-Bold", ENCODING_INDEX_STANDARD},
  {"Times-Italic", ENCODING_INDEX_STANDARD},
  {"Times-BoldItalic", ENCODING_INDEX_STANDARD},
  {"ZapfDingbats", ENCODING_INDEX_DINGBATS},
    {0, ENCODING_INDEX_STANDARD}
};

int pdf_find_standard_font(const uchar *str, uint size)
{
	const PSPDFStandardFont *ppsf;

  for (ppsf = pdf_standard_fonts; ppsf->fname; ++ppsf)
		if (strlen(ppsf->fname) == size && !strncmp(ppsf->fname, (const char *)str, size))
	    return ppsf - pdf_standard_fonts;
  return -1;
}

XWPSPDFResource::XWPSPDFResource()
{
	next = 0;
	prev = 0;
	rid = 0;
	named = false;
	used_on_page = false;
	object = 0;
}

XWPSPDFResource::~XWPSPDFResource()
{
	if (object)
	{
		if (object->decRef() == 0)
		    delete object;
		object = 0;
	}
}

long XWPSPDFResource::resourceId()
{
	return object->id;
}

XWPSPDFFontDescriptor::XWPSPDFFontDescriptor()
	:XWPSPDFResource()
{
	memset(FontName.chars, 0, MAX_PDF_FONT_NAME);
	FontName.size = 0;
	subset_ok = false;
	FontFile_id = 0;
	base_font = 0;
	notified = false;
	written = false;
}

XWPSPDFFontDescriptor::~XWPSPDFFontDescriptor()
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
	
	if (base_font)
	{
		if (base_font->decRef() == 0)
			delete base_font;
		base_font = 0;
	}
}

long XWPSPDFFontDescriptor::fontDescriptorId()
{
	return resourceId();
}

XWPSPDFCharProc::XWPSPDFCharProc()
	:XWPSPDFResource()
{
	font = 0;
	char_next = 0;
	width = 0; 
	height = 0;
	x_width = 0;
	y_offset = 0;
	char_code = 0;
}

XWPSPDFCharProc::~XWPSPDFCharProc()
{
}

long XWPSPDFCharProc::charProcId()
{
	return resourceId();
}

XWPSPDFFont::XWPSPDFFont()
	:XWPSPDFResource()
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

XWPSPDFFont::~XWPSPDFFont()
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
}

int XWPSPDFFont::charWidth(int ch, XWPSFont *fontA, int *pwidth)
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
	    gnstr.data = (uchar *)bfont->glyphName(glyph, &gnstr.size);
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
	    {
				pscale = &tt_scale;
	    }
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

long XWPSPDFFont::fontId()
{
	return resourceId();
}

XWPSPDFXObjects::XWPSPDFXObjects()
	:XWPSPDFResource()
{
	width = height = 0;
	data_height = 0;
}
