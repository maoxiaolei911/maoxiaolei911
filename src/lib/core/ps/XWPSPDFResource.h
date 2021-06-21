/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSPDFRESOURCE_H
#define XWPSPDFRESOURCE_H

#include "XWPSType.h"

class XWPSCosObject;
class XWPSFont;
class XWPSPDFFont;

#define SUBSET_PREFIX_SIZE 7
#define MAX_PDF_FONT_NAME\
  (SUBSET_PREFIX_SIZE + ps_font_name_max + 1 + 1 + sizeof(long) * 2)

#define PDF_NUM_STD_FONTS 14

extern const PSPDFStandardFont pdf_standard_fonts[];

int pdf_find_standard_font(const uchar *str, uint size);

class XWPSPDFResource
{
public:
	XWPSPDFResource();
	virtual ~XWPSPDFResource();
	
	long resourceId();
	
public:
	XWPSPDFResource * next;
	XWPSPDFResource * prev;
	ulong rid;
	bool named;
	bool used_on_page;
	XWPSCosObject *object;
};

class XWPSPDFFontDescriptor : public XWPSPDFResource
{
public:
	XWPSPDFFontDescriptor();
	~XWPSPDFFontDescriptor();
	
	long fontDescriptorId();
	
	XWPSFont * getBaseFont() {return base_font;}
	
	void recordUsed(int c) {chars_used.data[c >> 3] |= 1 << (c & 7);}
	
public:
	PSPDFFontName FontName;
  XWPSPDFFontDescriptorValues values;
  XWPSMatrix orig_matrix;
  XWPSString chars_used;
  XWPSString glyphs_used;
  bool subset_ok;
  long FontFile_id;
  XWPSFont *base_font;
  bool notified;
  bool written;
};

class XWPSPDFCharProc : public XWPSPDFResource
{
public:
	XWPSPDFCharProc();
	~XWPSPDFCharProc();
	
	long charProcId();
	
public:
	XWPSPDFFont *font;
  XWPSPDFCharProc *char_next;
  int width, height;
  int x_width;
  int y_offset;
  uchar char_code;
};

class XWPSPDFFont : public XWPSPDFResource
{
public:
	XWPSPDFFont();
	~XWPSPDFFont();
	
	int charWidth(int ch, XWPSFont *fontA, int *pwidth);
	
	long fontId();
	
	bool isSynthesized() {return (num_chars != 0);}
	
public:
	PSPDFFontName fname;
  PSFontType FontType;
  XWPSFont *font;
  int index;
  XWPSMatrix orig_matrix;
  bool is_MM_instance;
  char frname[1 + (sizeof(long) * 8 / 3 + 1) + 1];
  PSEncodingIndex BaseEncoding;
  XWPSPDFEncodingElement *Differences;
  XWPSPDFFontDescriptor *FontDescriptor;
  int FirstChar, LastChar;
  bool write_Widths;
  int Widths[256];
  uchar widths_known[32];
  bool skip;
  int num_chars;
  XWPSPDFCharProc *char_procs;
  int max_y_offset;
  uchar spaces[X_SPACE_MAX - X_SPACE_MIN + 1];
};

class XWPSPDFXObjects : public XWPSPDFResource
{
public:
	XWPSPDFXObjects();
	
public:
	int width, height;
	int data_height;
};

#endif //XWPSPDFRESOURCE_H
