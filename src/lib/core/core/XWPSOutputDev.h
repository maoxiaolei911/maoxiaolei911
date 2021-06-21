/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSOUTPUTDEV_H
#define XWPSOUTPUTDEV_H

#include <stddef.h>
#include "XWFontSetting.h"
#include "XWDocSetting.h"
#include "XWOutputDev.h"
#include "XWGraphixState.h"

class XWFunction;
class XWPath;
class XWCoreFont;
class XWColorSpace;
class XWSeparationColorSpace;
class XWPDFRectangle;
struct PSFont8Info;
struct PSFont16Enc;
struct PST1FontName;
class PSOutCustomColor;
class XWPSOutputDev;

enum PSOutMode {
  psModePS,
  psModeEPS,
  psModeForm
};

enum PSFileType {
  psFile,			// write to file
  psPipe,			// write to pipe
  psStdout,			// write to stdout
  psGeneric			// write to a generic stream
};

enum PSOutCustomCodeLocation {
  psOutCustomDocSetup,
  psOutCustomPageSetup
};

typedef void (*PSOutputFunc)(void *stream, char *data, int len);


typedef XWString *(*PSOutCustomCodeCbk)(XWPSOutputDev *psOut,
				       PSOutCustomCodeLocation loc, int n, 
				       void *data);


class XW_CORE_EXPORT XWPSOutputDev : public XWOutputDev
{
public:   
    XWPSOutputDev(char *fileName, 
                  XWDoc *docA,
	      		  int firstPage, 
	      		  int lastPage, PSOutMode modeA,
	      		  int imgLLXA = 0, 
	      		  int imgLLYA = 0,
	      		  int imgURXA = 0, 
	      		  int imgURYA = 0,
	      		  bool manualCtrlA = false,
	      		  PSOutCustomCodeCbk customCodeCbkA = 0,
	      			void *customCodeCbkDataA = 0);
	XWPSOutputDev(PSOutputFunc outputFuncA, 
	              void *outputStreamA,
	              XWDoc *docA,
	              int firstPage, 
	              int lastPage, 
	              PSOutMode modeA,
	      		  int imgLLXA = 0, 
	      		  int imgLLYA = 0,
	      		  int imgURXA = 0, 
	      		  int imgURYA = 0,
	      		  bool manualCtrlA = false,
	      		  PSOutCustomCodeCbk customCodeCbkA = 0,
	      			void *customCodeCbkDataA = 0);
    virtual ~XWPSOutputDev();
    			       
	virtual bool axialShadedFill(XWGraphixState *state, XWAxialShading *shading);
	
    virtual bool checkPageSlice(XWPage *page, 
                                double , 
                                double ,
			                    int rotateA, 
			                    bool useMediaBox, 
			                    bool crop,
			       				int sliceX, 
			       				int sliceY, 
			       				int sliceW, 
			       				int sliceH,
			       				bool printing, 
			                    bool (*abortCheckCbk)(void *data) = 0,
			       				void *abortCheckCbkData = 0);
    virtual void clip(XWGraphixState *state);
    virtual void clipToStrokePath(XWGraphixState *state);
    
    virtual void drawForm(ObjRef id);
	virtual void drawImage(XWGraphixState *, 
	                       XWObject *ref, 
	                       XWStream *str,
			               int width, 
			               int height, 
			               XWImageColorMap *colorMap,
			               int *maskColors, 
			               bool inlineImg);
	virtual void drawImageMask(XWGraphixState *, 
	                           XWObject *ref, 
	                           XWStream *str,
			                   int width, 
			                   int height, 
			                   bool invert,
			                   bool inlineImg);
    virtual void drawMaskedImage(XWGraphixState *, 
                                 XWObject *ref, 
                                 XWStream *str,
			                     int width, 
			                     int height,
			                     XWImageColorMap *colorMap,
			                     XWStream *maskStr, 
			                     int maskWidth, 
			                     int maskHeight,
			                     bool maskInvert);
	virtual void drawString(XWGraphixState *state, XWString *s);			   		       
    
    virtual void endPage();    
    virtual void endTextObject(XWGraphixState *);
    virtual void eoClip(XWGraphixState *state);
    virtual void eoFill(XWGraphixState *state);
    
    virtual void fill(XWGraphixState *state);
    virtual bool functionShadedFill(XWGraphixState *, XWFunctionShading *shading);
        
    virtual bool interpretType3Chars() { return false; }
    virtual bool isOk() { return ok; }
        
    virtual void opiBegin(XWGraphixState *state, XWDict *opiDict);
  	virtual void opiEnd(XWGraphixState *, XWDict *opiDict);
  
  	virtual void psXObject(XWStream *psStream, XWStream *level1Stream);
  	
    virtual bool radialShadedFill(XWGraphixState *state, XWRadialShading *shading);
    virtual void restoreState(XWGraphixState *);
    virtual void restoreTextPos(XWGraphixState *state);
    
    virtual void saveState(XWGraphixState *);
    virtual void saveTextPos(XWGraphixState *state);
    void setClip(double llx, double lly, double urx, double ury)
    	{ clipLLX0 = llx; clipLLY0 = lly; clipURX0 = urx; clipURY0 = ury; }
    void setOffset(double x, double y) { tx0 = x; ty0 = y; }
    void setOverlayCbk(void (*cbk)(XWPSOutputDev *psOut, void *data), void *data)
    	{ overlayCbk = cbk; overlayCbkData = data; }
    void setRotate(int rotateA) { rotate0 = rotateA; }
    void setScale(double x, double y) { xScale0 = x; yScale0 = y; }
    void setUnderlayCbk(void (*cbk)(XWPSOutputDev *psOut, void *data), void *data)
    	{ underlayCbk = cbk; underlayCbkData = data; }
    virtual void startPage(int pageNum, XWGraphixState *state);
    virtual void stroke(XWGraphixState *state);
    
    virtual void tilingPatternFill(XWGraphixState *, 
                                   XWObject *str,
				                   int paintType, 
				                   XWDict *resDict,
				                   double *mat, 
				                   double *bbox,
				                   int x0, 
				                   int y0, 
				                   int x1, 
				                   int y1,
				                   double xStep, 
				                   double yStep);
    virtual void type3D0(XWGraphixState *, double wx, double wy);
    virtual void type3D1(XWGraphixState *, double wx, double wy,
		                 double llx, double lly, double urx, double ury);
    
    virtual void updateCharSpace(XWGraphixState *state);
    virtual void updateCTM(XWGraphixState *, double m11, double m12,
			               double m21, double m22, double m31, double m32);
    virtual void updateFillColor(XWGraphixState *state);
    virtual void updateFillColorSpace(XWGraphixState *state);
    virtual void updateFillOverprint(XWGraphixState *state);
    virtual void updateFlatness(XWGraphixState *state);
    virtual void updateFont(XWGraphixState *state);
    virtual void updateHorizScaling(XWGraphixState *state);
    virtual void updateLineCap(XWGraphixState *state);
    virtual void updateLineDash(XWGraphixState *state);
    virtual void updateLineJoin(XWGraphixState *state);
    virtual void updateLineWidth(XWGraphixState *state);
    virtual void updateMiterLimit(XWGraphixState *state);
    virtual void updateRender(XWGraphixState *state);
    virtual void updateRise(XWGraphixState *state);
    virtual void updateStrokeColor(XWGraphixState *state);
    virtual void updateStrokeColorSpace(XWGraphixState *state);
    virtual void updateStrokeOverprint(XWGraphixState *state);
    virtual void updateTextMat(XWGraphixState *state);
    virtual void updateTextPos(XWGraphixState *state);
    virtual void updateTextShift(XWGraphixState *state, double shift);
    virtual void updateTransfer(XWGraphixState *state);
    virtual void updateWordSpace(XWGraphixState *state);
    virtual bool upsideDown() { return false; }
    virtual bool useDrawChar() { return false; }
    virtual bool useDrawForm() { return preload; }
    virtual bool useShadedFills() { return level >= psLevel2; }
    virtual bool useTilingPatternFill() { return true; }
    
    void writeDocSetup(XWCatalog *catalog, int firstPage, int lastPage);
    void writeHeader(int firstPage, 
                     int lastPage,
		             XWPDFRectangle *mediaBox, 
		             XWPDFRectangle *cropBox,
		             int pageRotate);
	void writePageTrailer();
	void writeTrailer();
	void writeXpdfProcset();
    
private:
	void addCustomColor(XWSeparationColorSpace *sepCS);
	void addProcessColor(double c, double m, double y, double k);
	
	void cvtFunction(XWFunction *func);
	
	void doImageL1(XWObject *ref, 
	               XWImageColorMap *colorMap,
		           bool invert, 
		           bool inlineImg,
		 		   XWStream *str, 
		 		   int width, 
		 		   int height, 
		 		   int len);
	void doImageL2(XWObject *ref, 
	               XWImageColorMap *colorMap,
		           bool invert, 
		           bool inlineImg,
		 		   XWStream *str, 
		 		   int width, 
		 		   int height, 
		 		   int len,
		 		   int *maskColors, 
		 		   XWStream *maskStr,
		           int maskWidth, 
		           int maskHeight, 
		           bool maskInvert);
	void doImageL3(XWObject *ref, 
	               XWImageColorMap *colorMap,
		           bool invert, 
		           bool inlineImg,
		 		   XWStream *str, 
		 		   int width, 
		 		   int height, 
		 		   int len,
		 		   int *maskColors, 
		 		   XWStream *maskStr,
		           int maskWidth, 
		           int maskHeight, 
		           bool maskInvert);
	void doImageL1Sep(XWImageColorMap *colorMap,
		              bool , 
		              bool ,
		              XWStream *str, 
		              int width, 
		              int height, 
		              int );
	void doPath(XWPath *path);
	void dumpColorSpaceL2(XWColorSpace *colorSpace,
			              bool genXform, 
			              bool updateColors,
						  bool map01);
	
	XWString *filterPSName(XWString *name);
	
	bool getFileSpec(XWObject *fileSpec, XWObject *fileName);
	
	void init(PSOutputFunc outputFuncA, 
	          void *outputStreamA,
	          PSFileType fileTypeA, 
	          XWDoc *docA, 
	    	  int firstPage, 
	    	  int lastPage, 
	    	  PSOutMode modeA,
	    	  int imgLLXA, 
	    	  int imgLLYA, 
	    	  int imgURXA, 
	    	  int imgURYA,
	    	  bool manualCtrlA);
	    	  
	XWString *makePSFontName(XWCoreFont *font, ObjRef *id);
	
	void opiBegin13(XWGraphixState *state, XWDict *dict);
	void opiBegin20(XWGraphixState *, XWDict *dict);
	void opiTransform(XWGraphixState *state, 
	                  double x0, 
	                  double y0,
		              double *x1, 
		              double *y1);
	
	void setupEmbeddedCIDTrueTypeFont(XWCoreFont *font, 
	                                  ObjRef *id, 
	                                  XWString *psName,
				                      bool needVerticalMetrics);
	void setupEmbeddedCIDType0Font(XWCoreFont *font, 
	                               ObjRef *id, 
	                               XWString *psName);
	void setupEmbeddedOpenTypeCFFFont(XWCoreFont *font, 
	                                  ObjRef *id, 
	                                  XWString *psName);
	void setupEmbeddedOpenTypeT1CFont(XWCoreFont *font, 
	                                  ObjRef *id, 
	                                  XWString *psName);   
	
	void setupEmbeddedTrueTypeFont(XWCoreFont *font, 
	                               ObjRef *id, 
	                               XWString *psName); 	  
	void setupEmbeddedType1CFont(XWCoreFont *font, 
	                             ObjRef *id, 
	                             XWString *psName);
	void setupEmbeddedType1Font(ObjRef *id, XWString *psName);
	
	void setupExternalCIDTrueTypeFont(XWCoreFont *font,
				    												XWString *fileName,
				    												XWString *psName,
				    												bool needVerticalMetrics);
	void setupExternalTrueTypeFont(XWCoreFont *font, 
	                               XWString *fileName,
					                       XWString *psName);
	void setupExternalType1Font(XWString *fileName, XWString *psName);
	void setupFont(XWCoreFont *font, XWDict *parentResDict);
	void setupFonts(XWDict *resDict);
	void setupForm(ObjRef id, XWObject *strObj);
	void setupForms(XWDict *resDict);
	void setupImage(ObjRef id, XWStream *str, bool mask);
	void setupImages(XWDict *resDict);
	void setupResources(XWDict *resDict);
	void setupType3Font(XWCoreFont *font, 
	                    XWString *psName, 
	                    XWDict *parentResDict);
    
    void writePS(char *s);
    void writePSChar(char c);
    void writePSFmt(const char *fmt, ...);
    void writePSName(char *s);
    void writePSString(XWString *s);
    void writePSTextLine(XWString *s);
    
private:
	PSLevel level;
	PSOutMode mode;
	int paperWidth;
	int paperHeight;
	bool paperMatch;
	int imgLLX, imgLLY, imgURX, imgURY;
	bool preload;
	PSOutputFunc outputFunc;
	void *outputStream;
	bool needClose;
  	PSFileType fileType;
  	bool manualCtrl;
  	int seqPage;
  	void (*underlayCbk)(XWPSOutputDev *psOut, void *data);
  	void *underlayCbkData;
  	void (*overlayCbk)(XWPSOutputDev *psOut, void *data);
  	void *overlayCbkData;
  	XWString *(*customCodeCbk)(XWPSOutputDev *psOut,
			    PSOutCustomCodeLocation loc, int n, 
			    void *data);
  void *customCodeCbkData;

  XWDoc *doc;
  	
  	XWRef *xref;
  	ObjRef *fontIDs;
  	
  	int fontIDLen;
  	int fontIDSize;
  	XWHash *fontNames;		// all used font names
  PST1FontName *t1FontNames;	// font names for Type 1/1C fonts
  int t1FontNameLen;		// number of entries in t1FontNames array
  int t1FontNameSize;		// size of t1FontNames array
  PSFont8Info *font8Info;	// info for 8-bit fonts
  int font8InfoLen;		// number of entries in font8Info array
  int font8InfoSize;		// size of font8Info array
  PSFont16Enc *font16Enc;	// encodings for substitute 16-bit fonts
  int font16EncLen;		// number of entries in font16Enc array
  int font16EncSize;		// size of font16Enc array
  ObjRef *imgIDs;			// list of image IDs for in-memory images
  int imgIDLen;			// number of entries in imgIDs array
  int imgIDSize;		// size of imgIDs array
  ObjRef *formIDs;			// list of IDs for predefined forms
  int formIDLen;		// number of entries in formIDs array
  int formIDSize;		// size of formIDs array
  XWList *xobjStack;		// stack of XObject dicts currently being
				//   processed
  int numSaves;			// current number of gsaves
  int numTilingPatterns;	// current number of nested tiling patterns
  int nextFunc;			// next unique number to use for a function

  XWList *paperSizes;		// list of used paper sizes, if paperMatch
				//   is true [PSOutPaperSize]
  double tx0, ty0;		// global translation
  double xScale0, yScale0;	// global scaling
  int rotate0;			// rotation angle (0, 90, 180, 270)
  double clipLLX0, clipLLY0,
         clipURX0, clipURY0;
  double tx, ty;		// global translation for current page
  double xScale, yScale;	// global scaling for current page
  int rotate;			// rotation angle for current page
  double epsX1, epsY1,		// EPS bounding box (unrotated)
         epsX2, epsY2;

  XWString *embFontList;		// resource comments for embedded fonts

  int processColors;		// used process colors
  PSOutCustomColor		// used custom colors
    *customColors;

  bool haveTextClip;		// set if text has been drawn with a
				//   clipping render mode

  bool inType3Char;		// inside a Type 3 CharProc
  XWString *t3String;		// Type 3 content string
  double t3WX, t3WY,		// Type 3 character parameters
         t3LLX, t3LLY, t3URX, t3URY;
  bool t3FillColorOnly;	// operators should only use the fill color
  bool t3Cacheable;		// cleared if char is not cacheable
  bool t3NeedsRestore;		// set if a 'q' operator was issued

  int opi13Nest;		// nesting level of OPI 1.3 objects
  int opi20Nest;		// nesting level of OPI 2.0 objects

  bool ok;			// set up ok?

  friend class WinPDFPrinter;
};

class DeviceNRecoder: public XWFilterStream 
{
public:
  	DeviceNRecoder(XWStream *strA, 
  	               int widthA, 
  	               int heightA,
		           XWImageColorMap *colorMapA);
  	virtual ~DeviceNRecoder();
  	
  	virtual int getChar()
    	{ return (bufIdx >= bufSize && !fillBuf()) ? EOF : buf[bufIdx++]; }
  	virtual int getKind() { return STREAM_WEIRD; }
  	virtual XWString *getPSFilter(int , const char *) { return NULL; }
  	
  	virtual bool isBinary(bool) { return true; }
  	virtual bool isEncoder() { return true; }
  	
  	virtual int lookChar()
    	{ return (bufIdx >= bufSize && !fillBuf()) ? EOF : buf[bufIdx]; }
  	
  	virtual void reset();

private:
  	bool fillBuf();
  
private:
  	int width, height;
  	XWImageColorMap *colorMap;
  	XWFunction *func;
  	XWImageStream *imgStr;
  	int buf[COLOR_MAX_COMPS];
  	int pixelIdx;
  	int bufIdx;
  	int bufSize;
};

#endif // XWPSOUTPUTDEV_H

