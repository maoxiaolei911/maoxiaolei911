/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSTEXT_H
#define XWPSTEXT_H

#include "XWPSType.h"

class XWPSCachedChar;
class XWPSFont;
class XWPSPath;
class XWPSClipPath;
class XWPSState;
class XWPSImagerState;
class XWPSDevice;
class XWPSDeviceNull;
class XWPSDeviceColor;
class XWPSDeviceMem;
class XWPSPDFFont;
class XWPSDVIFont;
class XWPSShowEnum;

#define TEXT_HAS_MORE_THAN_ONE_(op, any)\
  ( ((op) & any) & (((op) & any) - 1) )
#define TEXT_OPERATION_IS_INVALID(op)\
  (!((op) & TEXT_FROM_ANY) ||\
   !((op) & TEXT_DO_ANY) ||\
   TEXT_HAS_MORE_THAN_ONE_(op, TEXT_FROM_ANY) ||\
   TEXT_HAS_MORE_THAN_ONE_(op, TEXT_DO_ANY) ||\
   (((op) & TEXT_ADD_ANY) && ((op) & TEXT_REPLACE_WIDTHS))\
   )
#define TEXT_PARAMS_ARE_INVALID(params)\
  (TEXT_OPERATION_IS_INVALID((params)->operation) ||\
   ( ((params)->operation & TEXT_FROM_ANY_SINGLE) && ((params)->size != 1) )\
   )

#define TEXT_FROM_STRING          0x00001
#define TEXT_FROM_BYTES           0x00002
#define TEXT_FROM_CHARS           0x00004
#define TEXT_FROM_GLYPHS          0x00008
#define TEXT_FROM_SINGLE_CHAR     0x00010
#define TEXT_FROM_SINGLE_GLYPH    0x00020
#define TEXT_FROM_ANY_SINGLE\
  (TEXT_FROM_SINGLE_CHAR | TEXT_FROM_SINGLE_GLYPH)
#define TEXT_FROM_ANY\
  (TEXT_FROM_STRING | TEXT_FROM_BYTES | TEXT_FROM_CHARS | TEXT_FROM_GLYPHS |\
   TEXT_FROM_ANY_SINGLE)
   
#define TEXT_ADD_TO_ALL_WIDTHS    0x00040
#define TEXT_ADD_TO_SPACE_WIDTH   0x00080
#define TEXT_ADD_ANY\
  (TEXT_ADD_TO_ALL_WIDTHS | TEXT_ADD_TO_SPACE_WIDTH)
#define TEXT_REPLACE_WIDTHS       0x00100

#define TEXT_DO_NONE              0x00200
#define TEXT_DO_DRAW              0x00400
#define TEXT_DO_CHARWIDTH         0x00800
#define TEXT_DO_FALSE_CHARPATH    0x01000
#define TEXT_DO_TRUE_CHARPATH     0x02000
#define TEXT_DO_FALSE_CHARBOXPATH 0x04000
#define TEXT_DO_TRUE_CHARBOXPATH  0x08000
#define TEXT_DO_ANY_CHARPATH\
  (TEXT_DO_CHARWIDTH | TEXT_DO_FALSE_CHARPATH | TEXT_DO_TRUE_CHARPATH |\
   TEXT_DO_FALSE_CHARBOXPATH | TEXT_DO_TRUE_CHARBOXPATH)
#define TEXT_DO_ANY\
  (TEXT_DO_NONE | TEXT_DO_DRAW | TEXT_DO_ANY_CHARPATH)
  
#define TEXT_INTERVENE            0x10000

#define TEXT_RETURN_WIDTH         0x20000

class XWPSTextParams
{
public:
	XWPSTextParams();
	
	int replacedWidth(uint index, XWPSPoint *pwidth);
	
public:
	uint operation;
	union sd_ 
	{
		uchar *bytes;
		ulong *chars;
		ulong *glyphs;
		ulong d_char;
		ulong d_glyph;
  } data;
  
  uint size;
  XWPSPoint delta_all;
  XWPSPoint delta_space;
  
  union s_ 
  {
		ulong s_char;	
		ulong s_glyph;
  } space;
  
  const float *x_widths;
  const float *y_widths;
  uint widths_size;
  XWPSBytes * p_bytes;
};

class XWPSTextReturned
{
public:
	XWPSTextReturned();
	
public:
	ulong current_char;
  ulong current_glyph;
  XWPSPoint total_width;
};

#define MAX_FONT_STACK 5

class XWPSFontStackItem
{
public:
	XWPSFontStackItem();
	
public:
	XWPSFont *font;
	uint index;
};

class XWPSFontStack
{
public:
	XWPSFontStack();
	
public:
	int depth;
  XWPSFontStackItem items[1 + MAX_FONT_STACK];
};

class XWPSTextEnum : public XWPSStruct
{
public:
	XWPSTextEnum();
	virtual ~XWPSTextEnum();
	
	virtual void copy(XWPSTextEnum * pfrom);
	        void copyDynamic(XWPSTextEnum *pfrom, bool for_return);
	
					ulong    currentChar() {return returned.current_char;}
					XWPSFont * currentFont() {return current_font;}
					ulong currentGlyph() {return returned.current_glyph;}
	virtual int currentWidth(XWPSPoint*) {return -1;}
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	virtual int  init(XWPSDevice *devA, 
	                  XWPSImagerState *pisA,
		  							XWPSTextParams *textA, 
		  							XWPSFont *fontA, 
		  							XWPSPath *pathA,
		  							XWPSDeviceColor *pdcolorA, 
		  							XWPSClipPath *pcpathA);
	virtual bool isWidthOnly() {return false;}
	
	ulong nextChar();
	int nextCharGlyphDefault(ulong *pchr, ulong *pglyph);
	int nextCharGlyphType0(ulong *pchr, ulong *pglyph);
	
	virtual int process() {return 0;}
	
	        int restart(XWPSTextParams *textA);
	virtual int resync(XWPSTextEnum *) {return 0;}
	        uint rootEscChar();
	
	virtual int setCache(const double *,PSTextCacheControl) {return 0;}
	        int setCacheDevice(const double wbox[6]);
	        int setCacheDevice2(const double wbox2[10]);
	        int setCharWidth(const double wxy[2]);
	        bool showIs(int op_mask) {return ((text.operation & op_mask) != 0);}
	        bool showIsAddToAll();
	        bool showIsAddToSpace();
	        bool showIsAllOf(int op_mask) {return ((text.operation & op_mask) == op_mask);}
	        bool showIsDoKern();
					bool showIsDrawing();
					bool showIsSlow();
					bool showIsStringWidth();	
	        int stackModalFonts();
	        
	        int totalWidth(XWPSPoint *pwidth);
	
	virtual int  retry() {return 0;}
	
	        bool zcharShowWidthOnly();
	
public:
	XWPSTextParams text;
	XWPSDevice *dev;
	XWPSImagerState *pis;
	XWPSFont *orig_font;
	XWPSPath *path;
	XWPSDeviceColor *pdcolor;
	XWPSClipPath *pcpath;	
	XWPSFont *current_font;
	XWPSLog2ScalePoint log2_scale;
	uint index;
	uint xy_index;
	XWPSFontStack fstack;
	int cmap_code;
	XWPSPoint FontBBox_as_Metrics2;
	XWPSTextReturned returned;
	
protected:
	int initDynamic(XWPSFont *fontA);
};

enum PSShowWidthStatus
{
	sws_none,
  sws_cache,
  sws_no_cache,
  sws_cache_width_only
};

struct PSShowEnumContinuationProc
{
	union
	{
  	int (XWPSShowEnum::*continue_proc)();
  }continue_proc_;
};

class XWPSShowEnum : public XWPSTextEnum
{
public:
	XWPSShowEnum(XWPSState *pgsA);
	~XWPSShowEnum();
	
	int  continueKShow();
	int  continueShow();
	int  continueShowUpdate();
	int  currentWidth(XWPSPoint *pwidth);
	
	int getLength();
	const char * getTypeName();
	
	int  imageCachedChar(XWPSCachedChar * ccA);
	bool isWidthOnly();
	
	int process();
	
	int resync(XWPSTextEnum *pfrom);
	int  retry();
	
	int setCache(const double *pw,PSTextCacheControl control);
	int setCacheDevice(XWPSState * pgs, 
	                   double llx, 
	                   double lly,
		                 double urx, 
		                 double ury);
	int setCharWidth(XWPSState *pgs, double wx, double wy);
	
	int  showCacheSetup();
	XWPSFont * showCurrentFont();
	int  showFastMove(XWPSState * pgs, XWPSFixedPoint * pwxy);
	int  showFinish();
	PSCharPathMode showInCharPath() {return charpath_flag;}	
	int  showMove();
	int  showOriginSetup(XWPSState * pgs, 
	                    long cpt_x, 
	                    long cpt_y,
		                  PSCharPathMode charpath_flag);
	int  showProceed();
	void showSetScale();
	int  showStateSetup();
	int  showUpdate();

public:
	bool auto_release;
	XWPSState *pgs;
	int level;
	PSCharPathMode charpath_flag;
	XWPSState *show_gstate;
	int can_cache;
	XWPSIntRect ibox;
	XWPSIntRect obox;
	int ftx, fty;
	XWPSLog2ScalePoint log2_suggested_scale;
	XWPSDeviceMem *dev_cache;
	XWPSDeviceMem *dev_cache2;
	XWPSDeviceNull *dev_null;
	XWPSFixedPoint wxy;
	XWPSFixedPoint origin;
	XWPSCachedChar *cc;
	PSShowWidthStatus width_status;
	
	PSShowEnumContinuationProc proc;
};

class XWPSBboxTextEnum : public XWPSTextEnum
{
public:
	XWPSBboxTextEnum();
	~XWPSBboxTextEnum();
	
	int  currentWidth(XWPSPoint *pwidth);
	
	int getLength();
	const char * getTypeName();
	
	bool isWidthOnly();
	
	int process();
	
	int resync(XWPSTextEnum *pfrom);
	int  retry();
	
	int setCache(const double *values,PSTextCacheControl control);
	
public:
	XWPSTextEnum *target_info;
};

class XWPSPDFTextProcessState
{
public:
	XWPSPDFTextProcessState();
	
public:
	float chars;
	float words;
	float size;
	XWPSMatrix text_matrix;
	XWPSPDFFont *pdfont;
};

class XWPSPDFTextEnum : public XWPSTextEnum
{
public:
	XWPSPDFTextEnum();
	~XWPSPDFTextEnum();
	
	int  currentWidth(XWPSPoint *pwidth);
	
	int getLength();
	const char * getTypeName();
	
	bool isWidthOnly();
	
	int process();
	
	int resync(XWPSTextEnum *pfrom);
	int  retry();
	
	int setCache(const double *pw,PSTextCacheControl control);
	
	int updateTextState(XWPSPDFTextProcessState *ppts, XWPSMatrix *pfmat);
	
public:
	XWPSTextEnum *pte_default;
  XWPSFixedPoint origin;
};


class XWPSDVITextProcessState
{
public:
	XWPSDVITextProcessState();
	
public:
	float chars;
	float words;
	float size;
	XWPSMatrix text_matrix;
	XWPSDVIFont *pdfont;
};

class XWPSDVITextEnum : public XWPSTextEnum
{
public:
	XWPSDVITextEnum();
	~XWPSDVITextEnum();
	
	int  currentWidth(XWPSPoint *pwidth);
	
	int getLength();
	const char * getTypeName();
	
	bool isWidthOnly();
	
	int process();
	
	int resync(XWPSTextEnum *pfrom);
	int  retry();
	
	int setCache(const double *pw,PSTextCacheControl control);
	
	int updateTextState(XWPSDVITextProcessState *ppts, XWPSMatrix *pfmat);
	
public:
	XWPSTextEnum *pte_default;
  XWPSFixedPoint origin;
};

#endif //XWPSTEXT_H
