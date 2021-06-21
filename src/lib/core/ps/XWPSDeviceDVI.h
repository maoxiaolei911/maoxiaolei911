/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDEVICEDVI_H
#define XWPSDEVICEDVI_H

#include <QHash>
#include "XWPSDevicePsdf.h"
#include "XWObject.h"

class XWDVIColor;
class XWDVIRef;
class XWDVICore;
class XWDVIDev;
class XWPSCidSystemInfo;
class XWPSFontDir;
class XWPSFont;
class XWPSFontBase;
class XWPSFontType1;
class XWPSFontCid0;
class XWPSFontCid1;
class XWPSFontType42;
class XWPSFontCid2;
class XWPSFontType0;
class XWPSFunction;
class XWPSColorTile;
class XWPSColorSpace;
class XWPSColorScreenHalftone;
class XWPSDeviceHalftone;
class XWPSMultipleHalftone;
class XWPSScreenHalftone;
class XWPSSpotHalftone;
class XWPSThresholdHalftone;
class XWPSThresholdHalftone2;
class XWPSHTOrder;
class XWPSShading;
class XWPSTransferMap;
class XWPSDVIArticle;
class XWPSDVITextProcessState;
class XWPSDeviceDVI;
class XWPSDVIFont;

class XWPSDVIResource
{
public:
	XWPSDVIResource();
	virtual ~XWPSDVIResource();
	
	virtual void reset();
	long resourceId();
	
public:
	XWPSDVIResource * next;
	XWPSDVIResource * prev;
	ulong rid;
	bool named;
	bool used_on_page;
	XWObject indirect;
	XWObject object;
};

class XWPSDVIFontFile
{
public:
	XWPSDVIFontFile();
	~XWPSDVIFontFile();
	
	void reset();
	
public:
	XWObject indirect;
	XWObject object;
	XWPSStream * strm;
  XWPSStream * save_strm;
};

class XWPSDVIFontDescriptor : public XWPSDVIResource
{
public:
	XWPSDVIFontDescriptor();
	~XWPSDVIFontDescriptor();
	
	long fontDescriptorId();
	
	XWPSFont * getBaseFont() {return base_font;}
	
	void recordUsed(int c) {chars_used.data[c >> 3] |= 1 << (c & 7);}
	
	void reset();
	
public:
	PSPDFFontName FontName;
  XWPSPDFFontDescriptorValues values;
  XWPSMatrix orig_matrix;
  XWPSString chars_used;
  XWPSString glyphs_used;
  bool subset_ok;
  XWPSDVIFontFile * FontFile;
  XWPSFont *base_font;
  bool notified;
  bool written;
};

class XWPSDVICharProc : public XWPSDVIResource
{
public:
	XWPSDVICharProc();
	~XWPSDVICharProc();
	
	long charProcId();
	
	void reset();
	
public:
	XWPSDVIFont *font;
  XWPSDVICharProc *char_next;
  int width, height;
  int x_width;
  int y_offset;
  uchar char_code;
  XWPSStream * strm;
  XWPSStream * save_strm;
};

class XWPSDVIFont : public XWPSDVIResource
{
public:
	XWPSDVIFont();
	~XWPSDVIFont();
	
	int charWidth(int ch, XWPSFont *fontA, int *pwidth);
	
	long fontId();
	
	bool isSynthesized() {return (num_chars != 0);}
	
	void reset();
	
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
  XWPSDVIFontDescriptor *FontDescriptor;
  int FirstChar, LastChar;
  bool write_Widths;
  int Widths[256];
  uchar widths_known[32];
  bool skip;
  int num_chars;
  XWPSDVICharProc *char_procs;
  int max_y_offset;
  uchar spaces[X_SPACE_MAX - X_SPACE_MIN + 1];
};

class XWPSDVIXObjects : public XWPSDVIResource
{
public:
	XWPSDVIXObjects();
	
public:
	int width, height;
	int data_height;
};


class XWPSDVIStdFont
{
public:
	XWPSDVIStdFont();
	~XWPSDVIStdFont();
	
public:
	XWPSFont *font;
	XWPSDVIFontDescriptor *pfd;
	XWPSMatrix orig_matrix;
	XWPSUid uid;
};

class XWPSDVITextState
{
public:
	XWPSDVITextState();
	~XWPSDVITextState();
	
	void reset();
	
public:
	float character_spacing;
  XWPSDVIFont *font;
  double size;
  float word_spacing;
  float leading;
  bool use_leading;
  XWPSMatrix matrix;
  XWPSPoint line_start;
  XWPSPoint current;
  
	uchar buffer[max_text_buffer];
  int buffer_count;
};


class XWPSDVIResourceList
{
public:
	XWPSDVIResourceList();
	~XWPSDVIResourceList();
	
	void reset();
	
public:
	XWPSDVIResource*chains[NUM_RESOURCE_CHAINS];
};


class XWPSDVIGraphicsSave
{
public:
	XWPSDVIGraphicsSave();
	~XWPSDVIGraphicsSave();
	
public:
	XWPSDVIGraphicsSave *prev;
  XWObject indirect;
	XWObject object;
  long position;
  PSPDFContext save_context;
  long save_contents_id;
  XWPSStream * strm;
  XWPSStream * save_strm;
};


class XWPSDVIDataWriter
{
public:
	XWPSDVIDataWriter();
	~XWPSDVIDataWriter();
	
public:
	XWPSPSDFBinaryWriter binary;
  long start;
  long length_id;
  XWObject * object;
  XWPSStream * strm;
  XWPSStream * save_strm;
};


struct PSDVIAOParams
{
	XWPSDeviceDVI *pdev;
	const char *subtype;
	long src_pg;
};


class XWPSDVIImageWriter
{
public:
	XWPSDVIImageWriter();
	~XWPSDVIImageWriter();
	
public:
	XWPSPSDFBinaryWriter binary;
  const PSPDFImageNames *pin;
  XWPSDVIResource *pres;
  int height;
  XWObject data;
  const char *end_string;
  XWPSStream * strm;
  XWPSStream * save_strm;
  XWDict * dict;
};


class XWPSDVIOutlineNode
{
public:
	XWPSDVIOutlineNode();
	~XWPSDVIOutlineNode();
	
public:
	long id, parent_id, prev_id, first_id, last_id;
  int count;
};

class XWPSDVIOutlineLevel
{
public:
	XWPSDVIOutlineLevel();
	
public:
	XWPSDVIOutlineNode first;
  XWPSDVIOutlineNode last;
  int left;
};

class XWPSDVIArticle
{
public:
	XWPSDVIArticle();
	~XWPSDVIArticle();
	
public:
	XWPSDVIArticle *next;
  XWObject contents;
  XWPSPDFBead first;
  XWPSPDFBead last;
  long id;
};

class XWPSDVILabel
{
public:
	XWPSDVILabel() : label(0),used(false) {}
	
public:
	long label;
	bool used;
};

struct XWPSDVIPage
{
	long xobjCounter;
	long csCounter;
	long gsCounter;
	long patternCounter;
	long shadingCounter;
	long functionCounter;
	
	long * xobjLabels;
	long * csLabels;
	long * gsLabels;
	long * patternLabels;
	long * shadingLabels;
	long * functionLabels;
};


struct PSDVIMarkName
{
	const char *mname;
  int (XWPSDeviceDVI::*pdfmark_proc)(PSParamString *pairs, uint count, XWPSMatrix *pctm, PSParamString *objname);
  uchar options;
};

typedef int (XWPSDeviceDVI::*context_proc_dvi)();

class XWPSDeviceDVI : public XWPSDevicePsdf
{
	Q_OBJECT 
	
public:
	XWPSDeviceDVI(int w, int h, QObject * parent = 0);
	~XWPSDeviceDVI();
	
	int appendChars(const uchar * str, uint size);
	
	int  beginTypedImage(XWPSImagerState * pis,
		                   XWPSMatrix *pmat, 
		                   XWPSImageCommon *pic,
		                   XWPSIntRect * prect,
		                   XWPSDeviceColor * pdcolor,
		                   XWPSClipPath * pcpath, 
		                   XWPSImageEnumCommon ** pinfo,
		                   PSPDFTypedImageContext contextA);
	int beginTypedImageDVI(XWPSImagerState * pis,
			                        XWPSMatrix *pmat, 
			                        XWPSImageCommon *pic,
			   											XWPSIntRect * prect,
			                        XWPSDeviceColor * pdcolor,
			                        XWPSClipPath * pcpath, 
			                        XWPSImageEnumCommon ** pinfo);
	int closeDVI();
	int closePathDVI(double, 
	                      double,
		                    double, 
		                    double, 
		                    PSPathType);
  int curveToDVI(double x0, 
	                    double y0,
			                double x1, 
			                double y1, 
			                double x2, 
			                double y2,
			                double x3, 
			                double y3, 
			                PSPathType);
	
	int copyColorDVI(const uchar * base, 
	                 int sourcex,
		               int raster, 
		               ulong id, 
		               int x, 
		               int y, 
		               int w, 
		               int h);
	int  copyDevice(XWPSDevice **pnew);
	void copyDeviceParamDVI(XWPSDeviceDVI * proto);
	int copyMonoDVI(const uchar * base, 
	                int sourcex, 
	                int raster, 
	                ulong id,
		   						int x, 
		   						int y, 
		   						int w, 
		   						int h, 
		   						ulong zero,
		   						ulong one);
	int createPDFFont(XWPSFont *font, 
	                  XWPSMatrix *pomat,
		                XWPSDVIFont **pppf);
		
	int doImage(XWPSDVIResource * pres,
	     				XWPSMatrix * pimat);
	int doRectDVI(long x0, long y0, long x1, long y1, PSPathType type);
	
	int encodeChar(int chr, XWPSFontBase *bfont,	XWPSDVIFont *ppf);
	int endImageBinary(XWPSDVIImageWriter *piw, int data_h);
	int endPathDVI(PSPathType) {return 0;}
	int endWriteImage(XWPSDVIImageWriter * piw);
	
	int fillMaskDVI(const uchar * data, 
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
		   						XWPSClipPath * pcpath);
	int fillPathDVI(XWPSImagerState * pis, 
	                XWPSPath * ppath,
		              XWPSFillParams * params,
	                XWPSDeviceColor * pdcolor, 
	                XWPSClipPath * pcpath);
	int fillRectangleDVI(int x, int y, int  w, int  h, ulong color);
	int findOrigFont(XWPSFont *font, XWPSMatrix *pfmat);
	XWPSDVIResource * findResourceByPsId(PSPDFResourceType rtype, ulong rid);
	
	int  getParamsDVI(XWPSContextState * ctx, XWPSParamList *plist);
	
	void initDVI(XWDVICore * coreA,
	             XWDVIRef * xrefA,
	             XWDVIDev * devA);
	             
	int lineToDVI(double, double, double x, double y, PSPathType);
	
	int moveToDVI(double, double, double x, double y, PSPathType);
	
	int  openDVI();
	int  outputPageDVI(int num_copies, int flush);
	
	void putImageMatrix(XWPSMatrix * pmat, float y_scale);
	int putParamsDVI(XWPSContextState * ctx, XWPSParamList *plist);
	
	int setDashDVI(const float *pattern, uint count, double offset);
	int setFillColorDVI(XWPSDeviceColor * pdc);
	int setFlatDVI(double flatness);
	int setLineCapDVI(PSLineCap cap);
	int setLineJoinDVI(PSLineJoin join);
	int setLineWidthDVI(double widthA);
	int setMiterLimitDVI(double limit);
	int setStrokeColorDVI(XWPSDeviceColor * pdc);
	int setTextMatrix(XWPSMatrix * pmat);
	int stripTileRectangleDVI(XWPSStripBitmap * tiles,
			      								int x, 
			      								int y, 
			      								int w, 
			      								int h,
			                      ulong color0, 
			                      ulong color1,
			      								int px, 
			      								int py);
	int strokePathDVI(XWPSImagerState * pis,
		                XWPSPath * ppath, 
		                XWPSStrokeParams * params,
	                  XWPSDeviceColor * pdcolor, 
	                  XWPSClipPath * pcpath);
	                  
	int textBeginDVI(XWPSImagerState * pis,
		               XWPSTextParams *text, 
		               XWPSFont * font,
		               XWPSPath * path, 
		               XWPSDeviceColor * pdcolor,
		               XWPSClipPath * pcpath,
		               XWPSTextEnum ** ppte);
		               
	int  writeTextProcessState(XWPSDVITextProcessState *ppts,
			     									 XWPSString *pstr);
	
public:
	XWDVICore * core;
	XWDVIRef * xref;
	XWDVIDev * dev;
	
	double CompatibilityLevel;
	int EndPage;
  int StartPage;
  bool Optimize;
  bool ParseDSCCommentsForDocInfo;
  bool ParseDSCComments;
  bool EmitDSCWarnings;
  bool CreateJobTicket;
  bool PreserveEPSInfo;
  bool AutoPositionEPSFiles;
  bool PreserveCopyPage;
  bool UsePrologue;
  bool ReAssignCharacters;
  bool ReEncodeCharacters;
  long FirstObjectNumber;
  bool fill_overprint, stroke_overprint;
  int overprint_mode;
  ulong halftone_id;
  ulong transfer_ids[4];
  ulong black_generation_id, undercolor_removal_id;
  
	enum 
  {
		pdf_compress_none,
		pdf_compress_LZW,
		pdf_compress_Flate
  } compression;
	
  XWPSDVIFont *open_font;
  
  bool use_open_font;	
  long embedded_encoding_id;
	
	long outlines_id;
	long next_id;
	PSPDFContext context;
	PSPDFProcset procsets;
	
	XWPSDVITextState text;
	XWPSDVIStdFont std_fonts[PDF_NUM_STD_FONTS];
	long space_char_ids[X_SPACE_MAX - X_SPACE_MIN + 1];
  PSPDFTextRotation text_rotation;
  XWPSDVIPage *pages;
  int num_pages;
  
	XWPSDVIResourceList resources[NUM_RESOURCE_TYPES];
	XWPSDVIResource *cs_Patterns[5];
  XWPSDVIResource *last_resource;
  XWPSDVIOutlineLevel outline_levels[MAX_OUTLINE_DEPTH];
  int outline_depth;
  int closed_outline_depth;
  int outlines_open;
  
  XWPSDVIArticle *articles;
  XWPSDVIGraphicsSave *open_graphics;
  bool iswritestrm;
  bool innaming;
  
  QList<XWPSDVIFontDescriptor*> registeredFonts;
  QList<int> registeredStdFonts;
  QHash<QString, XWPSDVILabel*> fontLabels;
  QHash<QString, XWPSDVILabel*> fontDesLabels;
  
  static context_proc_dvi context_procs_dvi[4][4];
  static PSDVIMarkName mark_names[];
	
private:
	int  addEncodingDifference(XWPSDVIFont *ppf, 
	                           int chr,
			                       XWPSFontBase *bfont, 
			                       ulong glyph);
	void  addVector3(XWObject *pca, const PSVector3 *pvec);
	ulong adjustColorIndexDVI(ulong color);
	int  adjustFontName(XWPSDVIFontDescriptor *pfd, bool is_standard);
	int  allocAside(XWPSDVIResource ** plist,
		              XWPSDVIResource **ppres,
		              int rtype);
	int  allocFont(ulong rid, 
	               XWPSDVIFont **ppfres,
	       				XWPSDVIFontDescriptor *pfd_in);
	int  allocResource(int rtype, 
	                   ulong rid,
		                 XWPSDVIResource ** ppres);
	int  assignCharCode();

	int  beginCharProc(int w, 
	                   int h, 
	                   int x_width,
                     int y_offset, 
                     ulong id, 
                     XWPSDVICharProc ** ppcp, 
                     PSPDFStreamPosition * ppos);
	int  beginData(XWPSDVIDataWriter *pdw);
	int  beginFontFile(XWPSDVIFontFile * fontfile,
		                  long len, 
		                  XWPSPSDFBinaryWriter *pbw);
	int  beginImageData(XWPSDVIImageWriter * piw,
		     							XWPSPixelImage * pim, 
		     							XWObject *pcsvalue);
	int  beginResource(PSPDFResourceType rtype, 
	                   ulong rid,
		                 XWPSDVIResource ** ppres);
	int beginResourceBody(PSPDFResourceType rtype,	
	                       ulong rid, 
	                       XWPSDVIResource ** ppres);
	int  beginWriteImage(XWPSDVIImageWriter * piw,
		      						 ulong id, 
		      						 int w, 
		      						 int h, 
		      						 XWPSDVIResource *pres,
		                   bool in_line);
	                       
	bool canWriteImageInLine(XWPSImage1 *pim);
	bool canWriteStream();
	void checkFont(XWPSDVIFont * ppf);
	void checkFontDescriptor(XWPSDVIFontDescriptor * pfd);
	int closeContents(bool last);
	int closePage();
	int colorSpace(XWObject *pvalue,
								 XWPSColorSpace *pcs,
									const PSPDFColorSpaceNames *pcsn,
									bool by_name);
	void colorSpaceProcsets(XWPSColorSpace *pcs);
	int computeFontDescriptor(XWPSDVIFontDescriptor *pfd,
			                      XWPSFont *font, 
			                      const uchar *used);
	int copyColorBits(XWPSStream *s, 
	                 const uchar *base, 
	                 int sourcex, 
	                 int raster,
		               int w, 
		               int h, 
		               int bytes_per_pixel);
	int copyColorData(const uchar * base, 
	                  int sourcex,
		                int raster, 
		                ulong id, 
		                int x, 
		                int y, 
		                int w, 
		                int h,
		                XWPSImage1 *pim, 
		                XWPSDVIImageWriter *piw,
		                int for_pattern);
	int copyMaskBits(XWPSStream *s, 
	                 const uchar *base, 
	                 int sourcex, 
	                 int raster,
		   							int w, 
		   							int h, 
		   							uchar invert);	
	int copyMaskData(const uchar * base, 
	                 int sourcex,
		               int raster, 
		               ulong id, 
		               int x, 
		               int y, 
		               int w,
		               int h,
		   						 XWPSImage1 *pim, 
		   						 XWPSDVIImageWriter *piw,
		   						 int for_pattern);
	int copyMono(const uchar *base, 
	             int sourcex, 
	             int raster, 
	             ulong id,
	      			 int x, 
	      			 int y, 
	      			 int w, 
	      			 int h, 
	      			 ulong zero,
	      			 ulong one, 
	      			 XWPSClipPath *pcpath);
	int createNamedArray(PSParamString *pname, XWObject * pvalue);
	int createNamedDict(PSParamString *pname, XWObject * pvalue);
	int createNamedStream(PSParamString *pname, XWObject * pvalue);
	int csPatternColored(XWObject *pvalue);
	int csPatternUnColored(XWObject *pvalue);
	
	int doCharImage(XWPSDVICharProc * pcp, XWPSMatrix * pimat);
	int dominantRotation(PSPDFTextRotation * ptr);
	
	int embedFontAsType1(XWPSFontType1 *font,	
	                    XWPSDVIFontFile * FontFile, 
	                    ulong * subset_glyphs,
			                uint subset_size, 
			                XWPSString *pfname);
	int embedFontAsType2(XWPSFontType1 *font,
											 XWPSDVIFontFile * FontFile, 
											 ulong * subset_glyphs,
											 uint subset_size, 
											 XWPSString *pfname);
	int embedFontCid0(XWPSFontCid0 *font,
		    						XWPSDVIFontFile * FontFile, 
		    						const uchar *subset_cids,
		    						uint subset_size, 
		    						XWPSString *pfname);
	int embedFontCid2(XWPSFontCid2 *font,
		                XWPSDVIFontFile * FontFile, 
		                const uchar *subset_bits,
		    						uint subset_size, 
		    						XWPSString *pfname);
	int embedFontType1(XWPSFontType1 *font,
		     						 XWPSDVIFontFile * FontFile, 
		     						 ulong * subset_glyphs,
		                 uint subset_size, 
		                 XWPSString *pfname);
	int embedFontType42(XWPSFontType42 *font,
		      						XWPSDVIFontFile * FontFile, 
		      						ulong * subset_glyphs,
		      						uint subset_size, 
		      						XWPSString *pfname);
	int endCharProc(XWPSDVICharProc * pcp);
	int endData(XWPSDVIDataWriter *pdw);
	int endFontFile(XWPSDVIFontFile * fontfile, XWPSPSDFBinaryWriter *pbw);
	
	int finalizeFontDescriptor(XWPSDVIFontDescriptor *pfd);
	int findNamed(PSParamString * pname, XWObject *pco);
	int findStdAppearance(XWPSFontBase *bfont, int mask, int *psame);
	int flateBinary(XWPSPSDFBinaryWriter *pbw);
	PSPDFFontEmbed fontEmbedStatus(XWPSFont *font, int *pindex, int *psame);
	int  function(XWPSFunction *pfn, XWObject *obj);
	
	char * getName(PSParamString * pname);
	int getNamed(PSParamString * pname, XWObject *pco);
	int getObject(PSParamString * pvalue, XWObject *obj);
	int getRefence(PSParamString * pname, XWObject *pco);
	
	void init();
	
	int makeNamedArray(PSParamString * pname,
		    						XWObject *pcd, 
		    						XWObject * ref);
	int makeNamedDict(PSParamString * pname,
		    						XWObject *pcd, 
		    						XWObject * ref);
	int makeNamedStream(PSParamString * pname,
		    						XWObject *pcd, 
		    						XWObject * ref);
	
	int noneToStream();
	
	int  openContents(PSPDFContext contextA);
	int  openGState(XWPSDVIResource **ppres);
	int  openPage(PSPDFContext contextA);
	
	void pageIDs();
	void pageReset(XWPSDVIPage * p);
	int  pattern(XWPSDeviceColor *pdc,
	    						XWPSColorTile *p_tile, 
	    						XWPSColorTile *m_tile,
	    						XWObject *pcs_image, 
	    						XWPSDVIResource **ppres);
	int  patternSpace(XWObject *pvalue,
		  							XWPSDVIResource **ppres, 
		  							const char *cs_name);
	void pdfmarkAdjustParentCount(XWPSDVIOutlineLevel * plevel);
	int  pdfmarkANN(PSParamString *pairs, 
	                uint count, 
	                XWPSMatrix *pctm, 
	                PSParamString *objname);
	int  pdfmarkAnnot(PSParamString * pairs, 
	                  uint count,
	                  XWPSMatrix * pctm, 
	                  PSParamString *objname,
	                  const char *subtype);
	int pdfmarkARTICLE(PSParamString * pairs, 
	                   uint count,
		                 XWPSMatrix * pctm, 
		                 PSParamString * no_objname);
	int  pdfmarkBP(PSParamString * pairs, 
	               uint count,
	               XWPSMatrix * pctm, 
	               PSParamString * objname);
	int  pdfmarkCLOSE(PSParamString * pairs, 
	                  uint count,
	                  XWPSMatrix * pctm, 
	                  PSParamString * no_objname);
	int  pdfmarkCloseOutline();
	int  pdfmarkDEST(PSParamString * pairs, 
	                 uint count,
	                 XWPSMatrix * pctm, 
	                 PSParamString * objname);
	int pdfmarkDOCINFO(PSParamString * pairs, 
	                   uint count,
		                 XWPSMatrix * pctm, 
		                 PSParamString * no_objname);
	int pdfmarkDOCVIEW(PSParamString * pairs, 
	                   uint count,
		                 XWPSMatrix * pctm, 
		                 PSParamString * no_objname);
	int  pdfmarkEMC(PSParamString *, 
	               uint , 
	               XWPSMatrix *, 
	               PSParamString *);
	int pdfmarkEP(PSParamString * pairs, 
	              uint count,
	              XWPSMatrix * pctm, 
	              PSParamString * no_objname);
	bool pdfmarkFindKey(const char *key, 
	                    PSParamString * pairs, 
	                    uint count,
		                  PSParamString * pstr);	
	int  pdfmarkMakeDest(XWObject * dest,
												const char *Page_key, 
		  								 const char *View_key,
		                   PSParamString * pairs, 
		                   uint count);
	int pdfmarkLNK(PSParamString *pairs, 
	                uint count, 
	                XWPSMatrix *pctm, 
	                PSParamString *objname);
	int  pdfmarkNI(PSParamString *, 
	               uint , 
	               XWPSMatrix *, 
	               PSParamString *);	
	int pdfmarkNamespacePop(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int pdfmarkNamespacePush(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	const uchar * pdfmarkNextObject(const uchar * scan, 
	                                const uchar * end, 
	                                const uchar **pname,
		                              XWObject *ppco);
	int  pdfmarkOBJ(PSParamString * pairs, 
	                uint count,
	                XWPSMatrix * pctm, 
	                PSParamString * objname);
	int  pdfmarkOUT(PSParamString * pairs, 
	                uint count,
	                XWPSMatrix * pctm, 
	                PSParamString * no_objname);
	int  pdfmarkPAGE(PSParamString * pairs, 
	                 uint count,
	                 XWPSMatrix * pctm, 
	                 PSParamString * no_objname);
	int  pdfmarkPageNumber(PSParamString * pnstr);
	int  pdfmarkPAGES(PSParamString * pairs, 
	                  uint count,
	                  XWPSMatrix * pctm, 
	                  PSParamString * no_objname);
	int  pdfmarkProcess(PSParamStringArray * pma);
	int  pdfmarkPS(PSParamString * pairs, 
	               uint count,
	               XWPSMatrix * pctm, 
	               PSParamString * objname);
	int  pdfmarkPUT(PSParamString * pairs, 
	                uint count,
	                XWPSMatrix * pctm, 
	                PSParamString * no_objname);
	int  pdfmarkPutAoPairs(XWDict *pcd,
		                     PSParamString * pairs, 
		                     uint count,
		                     XWPSMatrix * pctm, 
		                     PSDVIAOParams * params,
		                     bool for_outline,
		                     XWPSRect * prect);
	int  pdfmarkPutCPair(XWDict *pcd, 
	                     const char *key,
		                   PSParamString * pvalue);
	int  XWPSDeviceDVI::pdfmarkPUTDICT(PSParamString * pairs, 
	                    uint count,
	                    XWPSMatrix * pctm, 
	                    PSParamString * no_objname);
	int  pdfmarkPUTINTERVAL(PSParamString * pairs, 
	                        uint count,
	                        XWPSMatrix * pctm, 
	                        PSParamString * no_objname);
	int  pdfmarkPutPair(XWDict *pcd, PSParamString * pair);
	int  pdfmarkPutPairs(XWDict *pcd, PSParamString * pairs, uint count);
	int  pdfmarkPUTSTREAM(PSParamString * pairs, 
	                      uint count,
	                      XWPSMatrix * pctm, 
	                      PSParamString * no_objname);
	int  pdfmarkStAttr(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkStBDC(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkStBMC(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkStBookmarkRoot(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkStClassMap(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkStOBJ(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkStPNE(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkStPop(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkStPopAll(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkStPush(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkStRetrieve(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkStRoleMap(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkScanRect(XWPSRect * prect, 
	                     PSParamString * str,
		                   XWPSMatrix * pctm);
	int  pdfmarkSP(PSParamString * pairs, 
	               uint count,
	               XWPSMatrix * pctm, 
	               PSParamString * no_objname);
	int  pdfmarkStStore(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkWriteArticle(XWPSDVIArticle * part);
	int  pdfmarkWriteBead(XWPSPDFBead * pbead);
	int  pdfmarkWriteBorder(XWObject *a, 
	                        PSParamString *str,
		     									XWPSMatrix *pctm);
	uint pdfmarkWritePs(XWPSStream *s, PSParamString * psource);
	int  prepareDrawing(XWPSImagerState *pis,
		    							const char *ca_format, 
		    							XWPSDVIResource **ppres);
	int  prepareFill(XWPSImagerState *pis);
	int  prepareImage(XWPSImagerState *pis);
	int  prepareImageMask(XWPSImagerState *pis,
		      							XWPSDeviceColor *pdcolor);
	int  prepareStroke(XWPSImagerState *pis);
	int  prepareVector(XWPSImagerState *pis,
		   								const char *ca_format, 
		   								XWPSDVIResource **ppres);
	int  putClipPath(XWPSClipPath * pcpath);
	int  putColoredPattern(XWPSDeviceColor *pdc,
													const PSPSDFSetColorCommands *ppscc,
													XWPSDVIResource **ppres);
	int  putDrawingColor(XWPSDeviceColor *pdc,
		      						 const PSPSDFSetColorCommands *ppscc);
	int  putFilters(XWDict *pcd, 
	                XWPSStream *s,
		              const PSPDFFilterNames *pfn);
	int  putImageFilters(XWDict *pcd,
	                     XWPSStream *s,
		      							const PSPDFImageNames *pin);
	int  putImageValues(XWDict *pcd, 
		     							XWPSPixelImage *pic,
		     							const PSPDFImageNames *pin,
		     							XWObject *pcsvalue);
	int  putLinearShading(XWDict *pscd, 
	                      const float *Coords,
		                    int num_coords, 
		                    const float *Domain,
		                    XWPSFunction *Function,
		       							const bool *Extend);
	void putMatrix(XWDict * dict, const char *key, XWPSMatrix *pmat);
	void putMatrix(const char *before,
	       				 XWPSMatrix * pmat, 
	       				 const char *after);
	int  putMeshShading(XWObject *pscs, XWPSShading *psh);
	int  putPatternMask(XWPSColorTile *m_tile,
		     							XWObject *ppcs_mask);
	int  putPattern2(XWPSDeviceColor *pdc,
		 							 const PSPSDFSetColorCommands *ppscc,
		 							 XWPSDVIResource **ppres);
	int  putPixelImageValues(XWDict *pcd, 
			   									 XWPSPixelImage *pim,
			   										XWPSColorSpace *pcs,
			   										const PSPDFImageNames *pin,
			   										XWObject *pcsvalue);
	int  putScalarShading(XWDict *pscd, XWPSShading *psh);
	int  putShadingCommon(XWDict *pscd, XWPSShading *psh);
	int putString(const uchar * str, uint size);
	int  putUncoloredPattern(XWPSDeviceColor *pdc,
			  									 const PSPSDFSetColorCommands *ppscc,
			                     XWPSDVIResource **ppres);
	void putVector3(XWDict *pcd, 
                  const char *key,
			            const PSVector3 *pvec);
	
	int  referNamed(PSParamString * pname_orig,	XWObject *pco);
	int  replaceNames(PSParamString * from, PSParamString * to);
	int  resetColor(XWPSDeviceColor *pdc,
		              XWPSDeviceColor * pdcolor,
		              const PSPSDFSetColorCommands *ppscc);
	void resetGraphics();
	void resetText();
	XWPSDVIResource ** resourceChain(PSPDFResourceType type, ulong rid) 
	   {return &(resources[type].chains[ps_id_hash(rid) % NUM_RESOURCE_CHAINS]);}
	
	bool scanForStandardFonts(XWPSFontDir *dir);
	int  separationColorSpace(XWObject *pca, 
	                          const char *csname,
			                      XWObject *snames,
			                      XWPSColorSpace *alt_space,
			                      XWPSFunction *pfn,
			                      const PSPDFColorSpaceNames *pcsn);
	int separationName(XWObject *pvalue, ulong sname);
	int  setDrawingColor(XWPSDeviceColor *pdc,
		      						 XWPSDeviceColor * pdcolor,
		      						 const PSPSDFSetColorCommands *ppscc);
	int  setFontAndSize(XWPSDVIFont * font, float size);
	int  setPureColor(ulong color,  
	                  XWPSDeviceColor * pdcolor,
		                const PSPSDFSetColorCommands *ppscc);
	int streamToNone();
	int streamToText();
	int stringToText();
	
	int  textToStream();
	
	int updateAlpha(XWPSImagerState *pis,
		 							const char *ca_format, 
		 							XWPSDVIResource **ppres);
	int updateHalftone(XWPSImagerState *pis, XWDict *hts);
	int updateTransfer(XWPSImagerState *pis, XWDict *trs);
	
	void writeArray(XWObject * obj, XWPSStream * s, bool in_line=true);
	void writeBoolean(XWObject * obj, XWPSStream * s);
	void writeCIDSystemInfo(XWDict * dict, XWPSCidSystemInfo *pcidsi);
	int writeColorScreenHalftone(XWPSColorScreenHalftone *pcsht,
			                         XWPSDeviceHalftone *pdht, 
			                         XWObject *r);
	void writeDict(XWObject * obj, XWPSStream * s, bool in_line = true);
	int writeEmbeddedFont(XWPSDVIFontDescriptor *pfd);
	int writeFontDescriptor(XWPSDVIFontDescriptor *pfd);
	int writeFontResource(XWPSDVIFont *pef,
												XWPSString *pfname,
												XWPSString *pbfname);
	int  writeFontResources();
	int writeFunction(XWPSFunction *pfn, XWObject * pid);
	void writeImage(XWPSDVIImageWriter * piw, XWPSStream * s);
	int writeIndirect(XWObject * obj, XWPSStream * s);
	int writeMultipleHalftone(XWPSMultipleHalftone *pmht,
			    										XWPSDeviceHalftone *pdht, 
			    										XWObject *r);
	void writeName(XWObject * obj, XWPSStream * s);
	void writeNull(XWPSStream * s);
	void writeNumber(XWObject * obj, XWPSStream * s);
	void writeObj(XWObject * obj, XWPSStream * s, bool in_line = true);
	int writeResourceObjects(PSPDFResourceType rtype);
	int writeResourceObjectsLast(PSPDFResourceType rtype);
	int  writeScreenHalftone(XWPSScreenHalftone *psht,
			  									 XWPSHTOrder *porder, 
			  									 XWObject *r);
	int writeSpotFunction(XWPSHTOrder *porder,	XWObject *pid);
	int  writeSpotHalftone(XWPSSpotHalftone *psht,
			                   XWPSHTOrder *porder, 
			                   XWObject *r);
	int writeSynthesizedType3(XWPSDVIFont *pef);
	int writeThresholdHalftone(XWPSThresholdHalftone *ptht,
			     									 XWPSHTOrder *porder, 
			     									 XWObject *r);
	int writeThreshold2Halftone(XWPSThresholdHalftone2 *ptht,
			                        XWPSHTOrder *porder,
			                        XWObject *r);
	int writeTransfer(XWPSTransferMap *map,
		   							const char *key, 
		   							XWDict *ids);
	int writeTransferMap(XWPSTransferMap *map,
		       						 int range0, 
		       						 bool check_identity,
		                   const char *key, 
		                   XWDict *ids);
	int writeWidths(XWDict * dict, int first, int last, const int widths[256]);
};

#endif //XWPSDEVICEDVI_H
