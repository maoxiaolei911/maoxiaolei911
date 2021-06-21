/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDEVICEPDF_H
#define XWPSDEVICEPDF_H

#include "XWPSDevicePsdf.h"
#include "XWPSPDFResource.h"

#define ASIDES_BASE_POSITION min_long

class XWPSCidSystemInfo;
class XWPSFontDir;
class XWPSFontBase;
class XWPSFontType1;
class XWPSFontCid0;
class XWPSFontCid1;
class XWPSFontType42;
class XWPSFontCid2;
class XWPSFontType0;
class XWPSCosObject;
class XWPSCosValue;
class XWPSFunction;
class XWPSColorTile;
class XWPSColorSpace;
class XWPSShading;
class XWPSTransferMap;
class XWPSColorScreenHalftone;
class XWPSDeviceHalftone;
class XWPSMultipleHalftone;
class XWPSScreenHalftone;
class XWPSSpotHalftone;
class XWPSThresholdHalftone;
class XWPSThresholdHalftone2;
class XWPSHTOrder;
struct PSCosObjectProcs;
class XWPSPDFTextProcessState;

class XWPSPDFPage
{
public:
	XWPSPDFPage();
	~XWPSPDFPage();
	
	XWPSPDFPage & operator=(XWPSPDFPage & other);
	
public:
	XWPSCosObject *Page;
  XWPSIntPoint MediaBox;
  PSPDFProcset procsets;
  long contents_id;
  long resource_ids[resourceFont];
  long fonts_id;
  XWPSCosObject *Annots;
  PSPDFTextRotation text_rotation;
};

class XWPSPDFOutlineNode
{
public:
	XWPSPDFOutlineNode();
	~XWPSPDFOutlineNode();
	
public:
	long id, parent_id, prev_id, first_id, last_id;
  int count;
  XWPSCosObject *action;
};

class XWPSPDFOutlineLevel
{
public:
	XWPSPDFOutlineLevel();
	
public:
	XWPSPDFOutlineNode first;
  XWPSPDFOutlineNode last;
  int left;
};

class XWPSPDFArticle
{
public:
	XWPSPDFArticle();
	~XWPSPDFArticle();
	
public:
	XWPSPDFArticle *next;
  XWPSCosObject *contents;
  XWPSPDFBead first;
  XWPSPDFBead last;
};

class XWPSPDFGraphicsSave
{
public:
	XWPSPDFGraphicsSave();
	~XWPSPDFGraphicsSave();
	
public:
	XWPSPDFGraphicsSave *prev;
  XWPSCosObject *object;
  long position;
  PSPDFContext save_context;
  long save_contents_id;
};

class XWPSPDFDataWriter
{
public:
	XWPSPDFDataWriter();
	
	int endData();
	
public:
	XWPSPSDFBinaryWriter binary;
  long start;
  long length_id;
};

struct PSAOParams
{
	XWPSDevicePDF *pdev;
	const char *subtype;
	long src_pg;
};

class XWPSPDFImageWriter
{
public:
	XWPSPDFImageWriter();
	~XWPSPDFImageWriter();
	
public:
	XWPSPSDFBinaryWriter binary;
  const PSPDFImageNames *pin;
  XWPSPDFResource *pres;
  int height;
  XWPSCosObject *data;
  const char *end_string;
};

typedef int (XWPSDevicePDF::*context_proc)();

struct PSPDFMarkName
{
	const char *mname;
  int (XWPSDevicePDF::*pdfmark_proc)(PSParamString *pairs, uint count, XWPSMatrix *pctm, PSParamString *objname);
  uchar options;
};

class XWPSDevicePDF : public XWPSDevicePsdf
{
	Q_OBJECT 
	
public:
	XWPSDevicePDF(QObject * parent = 0);
	virtual ~XWPSDevicePDF();
	
	int appendChars(const uchar * str, uint size);
	
	int  beginTypedImage(XWPSImagerState * pis,
		                   XWPSMatrix *pmat, 
		                   XWPSImageCommon *pic,
		                   XWPSIntRect * prect,
		                   XWPSDeviceColor * pdcolor,
		                   XWPSClipPath * pcpath, 
		                   XWPSImageEnumCommon ** pinfo,
		                   PSPDFTypedImageContext contextA);
	int beginTypedImagePDF(XWPSImagerState * pis,
			                        XWPSMatrix *pmat, 
			                        XWPSImageCommon *pic,
			   											XWPSIntRect * prect,
			                        XWPSDeviceColor * pdcolor,
			                        XWPSClipPath * pcpath, 
			                        XWPSImageEnumCommon ** pinfo);
	
	int closePDF();	
	int copyColorPDF(const uchar * base, 
	                 int sourcex,
		               int raster, 
		               ulong id, 
		               int x, 
		               int y, 
		               int w, 
		               int h);
	int  copyDevice(XWPSDevice **pnew);
	void copyDeviceParamPDF(XWPSDevicePDF * proto);
	int copyMonoPDF(const uchar * base, 
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
		                XWPSPDFFont **pppf);
	
	int doImage(XWPSPDFResource * pres,
	     				XWPSMatrix * pimat, 
	     				bool in_contents);
	int doRectPDF(long x0, long y0, long x1, long y1, PSPathType type);
	
	int encodeChar(int chr, XWPSFontBase *bfont,	XWPSPDFFont *ppf);
	int endImageBinary(XWPSPDFImageWriter *piw, int data_h);
	int endPathPDF(PSPathType ) {return 0;}
	int endSeparate();
	int endWriteImage(XWPSPDFImageWriter * piw);
	
	int fillMaskPDF(const uchar * data, 
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
	int fillPathPDF(XWPSImagerState * pis, 
	                XWPSPath * ppath,
		              XWPSFillParams * params,
	                XWPSDeviceColor * pdcolor, 
	                XWPSClipPath * pcpath);
	int fillRectanglePDF(int x, int y, int  w, int  h, ulong color);
	int findOrigFont(XWPSFont *font, XWPSMatrix *pfmat);
	XWPSPDFResource * findResourceByPsId(PSPDFResourceType rtype, ulong rid);
	
	int  getParamsPDF(XWPSContextState * ctx, XWPSParamList *plist);
	
	void initialize();

	int  openPDF();
	long openSeparate(long id);
	int  outputPagePDF(int num_copies, int flush);	
	
	void putImageMatrix(XWPSMatrix * pmat, float y_scale);
	int putParamsPDF(XWPSContextState * ctx, XWPSParamList *plist);
	
	int setFillColorPDF(XWPSDeviceColor * pdc);
	int setLineWidthPDF(double widthA);
	int setStrokeColorPDF(XWPSDeviceColor * pdc);
	int setTextMatrix(XWPSMatrix * pmat);
	int stripTileRectanglePDF(XWPSStripBitmap * tiles,
			      								int x, 
			      								int y, 
			      								int w, 
			      								int h,
			                      ulong color0, 
			                      ulong color1,
			      								int px, 
			      								int py);
	int strokePathPDF(XWPSImagerState * pis,
		                XWPSPath * ppath, 
		                XWPSStrokeParams * params,
	                  XWPSDeviceColor * pdcolor, 
	                  XWPSClipPath * pcpath);
	                  
	int textBeginPDF(XWPSImagerState * pis,
		               XWPSTextParams *text, 
		               XWPSFont * font,
		               XWPSPath * path, 
		               XWPSDeviceColor * pdcolor,
		               XWPSClipPath * pcpath,
		               XWPSTextEnum ** ppte);
	
	int  writeTextProcessState(XWPSPDFTextProcessState *ppts, XWPSString *pstr);
	void writeValue(const uchar * vstr, uint size);
	
public:
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
    
  XWPSPDFTempFile * xref;
  XWPSPDFTempFile * asides;
  XWPSPDFTempFile * streams;
  XWPSPDFTempFile * pictures;
  XWPSPDFFont *open_font;
  bool use_open_font;	
  long embedded_encoding_id;
  long next_id;
  XWPSCosObject *Catalog;
  XWPSCosObject *Info;
  XWPSCosObject *Pages;
#define pdf_num_initial_ids 3
  long outlines_id;
  int next_page;
  long contents_id;
  PSPDFContext context;
  long contents_length_id;
  long contents_pos;
  PSPDFProcset procsets;	
  XWPSPDFTextState text;
  XWPSPDFStdFont std_fonts[PDF_NUM_STD_FONTS];
  long space_char_ids[X_SPACE_MAX - X_SPACE_MIN + 1];
  PSPDFTextRotation text_rotation;
#define initial_num_pages 50
  XWPSPDFPage *pages;
  int num_pages;
  XWPSPDFResourceList resources[NUM_RESOURCE_TYPES];
  XWPSPDFResource *cs_Patterns[5];
  XWPSPDFResource *last_resource;
  XWPSPDFOutlineLevel outline_levels[MAX_OUTLINE_DEPTH];
  int outline_depth;
  int closed_outline_depth;
  int outlines_open;
  XWPSPDFArticle *articles;
  XWPSCosObject *Dests;
  XWPSCosObject *named_objects;
  XWPSPDFGraphicsSave *open_graphics;
  QList<XWPSPDFFontDescriptor*> registeredFonts;
  QList<int> registeredStdFonts;
  
  static context_proc context_procs[4][4];
  static PSPDFMarkName mark_names[];
  
protected:
	int  addEncodingDifference(XWPSPDFFont *ppf, 
	                           int chr,
			                       XWPSFontBase *bfont, 
			                       ulong glyph);
	int  adjustFontName(XWPSPDFFontDescriptor *pfd, bool is_standard);
	int  allocAside(XWPSPDFResource ** plist,
		              XWPSPDFResource **ppres,
		              int rtype,
		              long id);
	int  allocFont(ulong rid, 
	               XWPSPDFFont **ppfres,
	       				XWPSPDFFontDescriptor *pfd_in);
	int  allocResource(int rtype, 
	                   ulong rid,
		                 XWPSPDFResource ** ppres, 
		                 long id);
	int  arrayAddInt2(XWPSCosObject *pca, int lower, int upper);
	int  assignCharCode();
	
	int  beginAside(XWPSPDFResource ** plist, XWPSPDFResource ** ppres, int rtype);
	int  beginCharProc(int w, 
	                   int h, 
	                   int x_width,
                     int y_offset, 
                     ulong id, 
                     XWPSPDFCharProc ** ppcp, 
                     PSPDFStreamPosition * ppos);
	int  beginData(XWPSPDFDataWriter *pdw);
	int  beginFontFile(long FontFile_id,
		   								long *plength_id, 
		   								const char *entries,
		                  long len, 
		                  long *pstart, 
		                  XWPSPSDFBinaryWriter *pbw);
	int  beginImageData(XWPSPDFImageWriter * piw,
		     							XWPSPixelImage * pim, 
		     							XWPSCosValue *pcsvalue);
	long beginObj();
	int  beginResource(PSPDFResourceType rtype, 
	                   ulong rid,
		                 XWPSPDFResource ** ppres);
	int  beginResourceBody(PSPDFResourceType rtype,	
	                       ulong rid, 
	                       XWPSPDFResource ** ppres);
	long beginSeparate();
	int  beginWriteImage(XWPSPDFImageWriter * piw,
		      						 ulong id, 
		      						 int w, 
		      						 int h, 
		      						 XWPSPDFResource *pres,
		                   bool in_line);
	
	bool canWriteImageInLine(XWPSImage1 *pim);
	int closeContents(bool last);
	int closeFiles(int code);
	int closePage();
	int colorSpace(XWPSCosValue *pvalue,
								 XWPSColorSpace *pcs,
									const PSPDFColorSpaceNames *pcsn,
									bool by_name);
	void colorSpaceProcsets(XWPSColorSpace *pcs);
	int computeFontDescriptor(XWPSPDFFontDescriptor *pfd,
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
		                XWPSPDFImageWriter *piw,
		                int for_pattern);
	int copyMaskBits(XWPSStream *s, 
	                 const uchar *base, 
	                 int sourcex, 
	                 int raster,
		   							int w, 
		   							int h, 
		   							uchar invert);	
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
	int copyMaskData(const uchar * base, 
	                 int sourcex,
		               int raster, 
		               ulong id, 
		               int x, 
		               int y, 
		               int w,
		               int h,
		   						 XWPSImage1 *pim, 
		   						 XWPSPDFImageWriter *piw,
		   						 int for_pattern);
	int cosStreamPutCStrings(XWPSCosObject *pcs, const char *key, const char *value);
	int createNamed(PSParamString *pname,
		 							PSCosObjectProcs * cotype, 
		 							XWPSCosObject **ppco, 
		 							long id);
	int createNamedDict(PSParamString *pname, XWPSCosObject **ppco, long id);
	int csPatternColored(XWPSCosValue *pvalue);
	int csPatternUnColored(XWPSCosValue *pvalue);
	XWPSPDFPage   * currentPage();
	XWPSCosObject * currentPageDict();
	
	int doCharImage(XWPSPDFCharProc * pcp, XWPSMatrix * pimat);
	int dominantRotation(PSPDFTextRotation * ptr);
	
	int embedFontAsType1(XWPSFontType1 *font,	
	                    long FontFile_id, 
	                    ulong * subset_glyphs,
			                uint subset_size, 
			                XWPSString *pfname);
	int embedFontAsType2(XWPSFontType1 *font,
											 long FontFile_id, 
											 ulong * subset_glyphs,
											 uint subset_size, 
											 XWPSString *pfname);
	int embedFontCid0(XWPSFontCid0 *font,
		    						long FontFile_id, 
		    						const uchar *subset_cids,
		    						uint subset_size, 
		    						XWPSString *pfname);
	int embedFontCid2(XWPSFontCid2 *font,
		                long FontFile_id, 
		                const uchar *subset_bits,
		    						uint subset_size, 
		    						XWPSString *pfname);
	int embedFontType1(XWPSFontType1 *font,
		     						 long FontFile_id, 
		     						 ulong * subset_glyphs,
		                 uint subset_size, 
		                 XWPSString *pfname);
	int embedFontType42(XWPSFontType42 *font,
		      						long FontFile_id, 
		      						ulong * subset_glyphs,
		      						uint subset_size, 
		      						XWPSString *pfname);
	int endAside();
	int endCharProc(PSPDFStreamPosition * ppos);
	int endFontFile(long start, long length_id, XWPSPSDFBinaryWriter *pbw);
	int endGState(XWPSPDFResource *pres);
	int endObj();
	int endResource();
	
	int finalizeFontDescriptor(XWPSPDFFontDescriptor *pfd);
	int findNamed(PSParamString * pname, XWPSCosObject **ppco);
	int findStdAppearance(XWPSFontBase *bfont, int mask, int *psame);
	int flateBinary(XWPSPSDFBinaryWriter *pbw);
	PSPDFFontEmbed fontEmbedStatus(XWPSFont *font, int *pindex, int *psame);
	
	int getNamed(PSParamString * pname,
	             PSCosObjectProcs * cotype, 
	             XWPSCosObject **ppco);
	
	bool isInPage() {return contents_id != 0;}
	
	int makeNamed(PSParamString * pname,
	              PSCosObjectProcs * cotype, 
	              XWPSCosObject **ppco, 
	              bool assign_id);
	int makeNamedDict(PSParamString * pname,
		    						XWPSCosObject **ppcd, 
		    						bool assign_id);
	
	long nextId() {return next_id++;}
	int  noneToStream();
	
	long objRef();
	int  openContents(PSPDFContext contextA);
	void openDocument();
	int  openGState(XWPSPDFResource **ppres);
	long openObj(long id);
	int  openPage(PSPDFContext contextA);
	
	long pageId(int page_num);
	int  patternSpace(XWPSCosValue *pvalue,
		  							XWPSPDFResource **ppres, 
		  							const char *cs_name);
	int  pdfFunction(XWPSFunction *pfn, XWPSCosValue *pvalue);
	void pdfmarkAdjustParentCount(XWPSPDFOutlineLevel * plevel);
	int  pdfmarkAnnot(PSParamString * pairs, 
	                  uint count,
	                  XWPSMatrix * pctm, 
	                  PSParamString *objname,
	                  const char *subtype);
	int  pdfmarkANN(PSParamString *pairs, 
	                uint count, 
	                XWPSMatrix *pctm, 
	                PSParamString *objname);
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
	int pdfmarkLNK(PSParamString *pairs, 
	                uint count, 
	                XWPSMatrix *pctm, 
	                PSParamString *objname);
	int pdfmarkNamespacePop(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int pdfmarkNamespacePush(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkNI(PSParamString *, 
	               uint , 
	               XWPSMatrix *, 
	               PSParamString *);
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
	int  pdfmarkPAGES(PSParamString * pairs, 
	                  uint count,
	                  XWPSMatrix * pctm, 
	                  PSParamString * no_objname);
	int  pdfmarkPUT(PSParamString * pairs, 
	                uint count,
	                XWPSMatrix * pctm, 
	                PSParamString * no_objname);
	int  pdfmarkPUTDICT(PSParamString * pairs, 
	                    uint count,
	                    XWPSMatrix * pctm, 
	                    PSParamString * no_objname);
	int  pdfmarkPUTINTERVAL(PSParamString * pairs, 
	                        uint count,
	                        XWPSMatrix * pctm, 
	                        PSParamString * no_objname);
	int  pdfmarkPUTSTREAM(PSParamString * pairs, 
	                      uint count,
	                      XWPSMatrix * pctm, 
	                      PSParamString * no_objname);
	int  pdfmarkPS(PSParamString * pairs, 
	               uint count,
	               XWPSMatrix * pctm, 
	               PSParamString * objname);
	int  pdfmarkSP(PSParamString * pairs, 
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
	int  pdfmarkStStore(PSParamString *, 
	                         uint , 
	                         XWPSMatrix *, 
	                         PSParamString *);
	int  pdfmarkCloseOutline();
	int  pdfmarkCoerceDest(PSParamString *dstr, char * dest);
	bool pdfmarkFindKey(const char *key, 
	                    PSParamString * pairs, 
	                    uint count,
		                  PSParamString * pstr);
	int  pdfmarkMakeDest(char * dstr,
		  								 const char *Page_key, 
		  								 const char *View_key,
		                   PSParamString * pairs, 
		                   uint count);
	void pdfmarkMakeRect(char*str, XWPSRect * prect);
	const uchar * pdfmarkNextObject(const uchar * scan, 
	                                const uchar * end, 
	                                const uchar **pname,
		                              XWPSCosObject **ppco);
	int  pdfmarkPageNumber(PSParamString * pnstr);
	int  pdfmarkProcess(PSParamStringArray * pma);
	int  pdfmarkPutAoPairs(XWPSCosObject *pcd,
		                     PSParamString * pairs, 
		                     uint count,
		                     XWPSMatrix * pctm, 
		                     PSAOParams * params,
		                     bool for_outline);
	int  pdfmarkPutCPair(XWPSCosObject *pcd, 
	                     const char *key,
		                   PSParamString * pvalue);
	int  pdfmarkPutPair(XWPSCosObject *pcd, PSParamString * pair);
	int  pdfmarkPutPairs(XWPSCosObject *pcd, PSParamString * pairs, uint count);
	int  pdfmarkScanRect(XWPSRect * prect, 
	                     PSParamString * str,
		                   XWPSMatrix * pctm);
	int  pdfmarkWriteArticle(XWPSPDFArticle * part);
	int  pdfmarkWriteBead(XWPSPDFBead * pbead);
	int  pdfmarkWriteBorder(XWPSStream *s, 
	                        PSParamString *str,
		     									XWPSMatrix *pctm);
	int  pdfmarkWriteOutline(XWPSPDFOutlineNode * pnode, long next_id);
	uint pdfmarkWritePs(XWPSStream *s, PSParamString * psource);
	int  pdfPattern(XWPSDeviceColor *pdc,
	    						XWPSColorTile *p_tile, 
	    						XWPSColorTile *m_tile,
	    						XWPSCosObject *pcs_image, 
	    						XWPSPDFResource **ppres);
	int  prepareDrawing(XWPSImagerState *pis,
		    							const char *ca_format, 
		    							XWPSPDFResource **ppres);
	int  prepareFill(XWPSImagerState *pis);
	int  prepareImage(XWPSImagerState *pis);
	int  prepareImageMask(XWPSImagerState *pis,
		      							XWPSDeviceColor *pdcolor);
	int  prepareStroke(XWPSImagerState *pis);
	int  prepareVector(XWPSImagerState *pis,
		   								const char *ca_format, 
		   								XWPSPDFResource **ppres);
	int  putClipPath(XWPSClipPath * pcpath);
	int  putColoredPattern(XWPSDeviceColor *pdc,
													const PSPSDFSetColorCommands *ppscc,
													XWPSPDFResource **ppres);
	int  putDrawingColor(XWPSDeviceColor *pdc,
		      						 const PSPSDFSetColorCommands *ppscc);
	int  putFilters(XWPSCosObject *pcd, 
	                XWPSStream *s,
		              const PSPDFFilterNames *pfn);
	int  putImageFilters(XWPSCosObject *pcd,
		      						 XWPSPSDFBinaryWriter * pbw,
		      							const PSPDFImageNames *pin);
	int  putImageValues(XWPSCosObject *pcd, 
		     							XWPSPixelImage *pic,
		     							const PSPDFImageNames *pin,
		     							XWPSCosValue *pcsvalue);
	int  putLinearShading(XWPSCosObject *pscd, 
	                      const float *Coords,
		                    int num_coords, 
		                    const float *Domain,
		                    XWPSFunction *Function,
		       							const bool *Extend);
	int  putMeshShading(XWPSCosObject *pscs, XWPSShading *psh);
	void putMatrix(const char *before,
	       				 XWPSMatrix * pmat, 
	       				 const char *after);
	void putName(const uchar *nstr, uint size);
	void putNameEscaped(XWPSStream *s, 
	                    const uchar *nstr, 
	                    uint size, 
	                    bool escape);
	int  putPatternMask(XWPSColorTile *m_tile,
		     							XWPSCosObject **ppcs_mask);
	int  putPattern2(XWPSDeviceColor *pdc,
		 							 const PSPSDFSetColorCommands *ppscc,
		 							 XWPSPDFResource **ppres);
	int  putPixelImageValues(XWPSCosObject *pcd, 
			   									 XWPSPixelImage *pim,
			   										XWPSColorSpace *pcs,
			   										const PSPDFImageNames *pin,
			   										XWPSCosValue *pcsvalue);
	int  putScalarShading(XWPSCosObject *pscd, XWPSShading *psh);
	int  putShadingCommon(XWPSCosObject *pscd, XWPSShading *psh);
	int  putString(const uchar * str, uint size);
	int  putUncoloredPattern(XWPSDeviceColor *pdc,
			  									 const PSPSDFSetColorCommands *ppscc,
			                     XWPSPDFResource **ppres);
	
	int  referNamed(PSParamString * pname_orig,	XWPSCosObject **ppco);
	int  replaceNames(PSParamString * from, PSParamString * to);
	int  resetColor(XWPSDeviceColor *pdc,
		              XWPSDeviceColor * pdcolor,
		              const PSPSDFSetColorCommands *ppscc);
	void resetGraphics();
	void resetPage();
	void resetText();
	XWPSPDFResource ** resourceChain(PSPDFResourceType type, ulong rid) 
	   {return &(resources[type].chains[ps_id_hash(rid) % NUM_RESOURCE_CHAINS]);}
	
	bool scanForStandardFonts(XWPSFontDir *dir);
	int  separationColorSpace(XWPSCosObject *pca, 
	                          const char *csname,
			                      XWPSCosValue *snames,
			                      XWPSColorSpace *alt_space,
			                      XWPSFunction *pfn,
			                      const PSPDFColorSpaceNames *pcsn);
	int  separationName(XWPSCosValue *pvalue, ulong sname);
	int  setDrawingColor(XWPSDeviceColor *pdc,
		      						 XWPSDeviceColor * pdcolor,
		      						 const PSPSDFSetColorCommands *ppscc);
	int  setFontAndSize(XWPSPDFFont * font, float size);
	int  setPureColor(ulong color,  
	                  XWPSDeviceColor * pdcolor,
		                const PSPSDFSetColorCommands *ppscc);
	int  streamToNone();
	int  streamToText();
	int  stringToText();
	
	long tell();
	int  textToStream();
	
	int updateAlpha(XWPSImagerState *pis,
		 							const char *ca_format, 
		 							XWPSPDFResource **ppres);
	int updateHalftone(XWPSImagerState *pis, char *hts);
	int updateTransfer(XWPSImagerState *pis, char *trs);
	
	int writeColorScreenHalftone(XWPSColorScreenHalftone *pcsht,
			                         XWPSDeviceHalftone *pdht, 
			                         long *pid);
	int writeEmbeddedFont(XWPSPDFFontDescriptor *pfd);
	int writeFontBbox(XWPSIntRect *pbox);
	int writeFontDescriptor(XWPSPDFFontDescriptor *pfd);
	int writeFontResource(XWPSPDFFont *pef,
												XWPSString *pfname,
												XWPSString *pbfname);
	int writeFontResources();
	int writeFunction(XWPSFunction *pfn, long *pid);
	int writeMultipleHalftone(XWPSMultipleHalftone *pmht,
			    										XWPSDeviceHalftone *pdht, 
			    										long *pid);
	int writePage(int page_num);
	int writeResourceObjects(PSPDFResourceType rtype);
	void writeCIDSystemInfo(XWPSCidSystemInfo *pcidsi);
	int  writeScreenHalftone(XWPSScreenHalftone *psht,
			  									 XWPSHTOrder *porder, 
			  									 long *pid);
	int  writeSpotFunction(XWPSHTOrder *porder,	long *pid);
	int  writeSpotHalftone(XWPSSpotHalftone *psht,
			                   XWPSHTOrder *porder, 
			                   long *pid);
	int writeSynthesizedType3(XWPSPDFFont *pef);
	int writeTransfer(XWPSTransferMap *map,
		   							const char *key, 
		   							char *ids);
	int writeTransferMap(XWPSTransferMap *map,
		       						 int range0, 
		       						 bool check_identity,
		                   const char *key, 
		                   char *ids);
	int writeThresholdHalftone(XWPSThresholdHalftone *ptht,
			     									 XWPSHTOrder *porder, 
			     									 long *pid);
	int writeThreshold2Halftone(XWPSThresholdHalftone2 *ptht,
			                        XWPSHTOrder *porder, 
			                        long *pid);
	int writeWidths(int first, int last, const int widths[256]);
};

#endif //
