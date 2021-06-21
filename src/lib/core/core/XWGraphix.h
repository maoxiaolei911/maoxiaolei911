/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWGRAPHIX_H
#define XWGRAPHIX_H

#include "XWGlobal.h"

class XWList;
class XWString;
class XWDoc;
class XWRef;
class XWArray;
class XWStream;
class XWPDFParser;
class XWDict;
class XWFunction;
class XWOutputDev;
class XWCoreFontDict;
class XWCoreFont;
class XWPattern;
class XWTilingPattern;
class XWShadingPattern;
class XWShading;
class XWFunctionShading;
class XWAxialShading;
class XWRadialShading;
class XWGouraudTriangleShading;
class XWPatchMeshShading;
struct XWPatch;
class XWGraphixState;
struct CoreColor;
class XWColorSpace;
class XWGraphix;
class XWPDFRectangle;
class XWAnnotBorderStyle;


enum GraphicsClipType 
{
  clipNone,
  clipNormal,
  clipEO
};

enum TchkType 
{
  tchkBool,			// boolean
  tchkInt,			// integer
  tchkNum,			// number (integer or real)
  tchkString,			// string
  tchkName,			// name
  tchkArray,			// array
  tchkProps,			// properties (dictionary or name)
  tchkSCN,			// scn/SCN args (number of name)
  tchkNone			// used to avoid empty initializer lists
};

#define maxArgs 33

struct Operator 
{
    char name[4];
    int numArgs;
    TchkType tchk[maxArgs];
    void (XWGraphix::*func)(XWObject args[], int numArgs);
};


class XW_CORE_EXPORT XWGraphixResources
{
public:
    XWGraphixResources(XWRef *xref, 
                       XWDict *resDict, 
                       XWGraphixResources *nextA);
    ~XWGraphixResources();
    
    XWGraphixResources *getNext() { return next; }
    
    void lookupColorSpace(char *name, XWObject *obj);
    XWCoreFont * lookupFont(char *name);
    bool lookupGState(char *name, XWObject *obj);
    XWPattern  * lookupPattern(char *name);
    bool         lookupPropertiesNF(char *name, XWObject *obj);
    XWShading  * lookupShading(char *name);
    bool lookupXObject(char *name, XWObject *obj);
    bool lookupXObjectNF(char *name, XWObject *obj);
    
private:
    XWCoreFontDict *fonts;
    XWObject xObjDict;
    XWObject colorSpaceDict;
    XWObject patternDict;
    XWObject shadingDict;
    XWObject gStateDict;
    XWObject propsDict;
    XWGraphixResources *next;
};


enum MarkedContentKind 
{
  MCOptionalContent,
  MCActualText,
  MCOther
};

class XWMarkedContent {
public:

  XWMarkedContent(MarkedContentKind kindA, bool ocStateA) 
  {
    kind = kindA;
    ocState = ocStateA;
  }
  ~XWMarkedContent() {}

  MarkedContentKind kind;
  bool ocState;		// true if drawing is enabled, false if
				//   disabled
};


class XW_CORE_EXPORT XWGraphix
{
public:
    XWGraphix(XWDoc *docA,  
              XWOutputDev *outA, 
              int pageNum, 
              XWDict *resDict,
              double hDPI, 
              double vDPI, 
              XWPDFRectangle *box,
              XWPDFRectangle *cropBox, 
              int rotate,
              bool (*abortCheckCbkA)(void *data) = 0,
              void *abortCheckCbkDataA = 0);
	XWGraphix(XWDoc *docA, 
              XWOutputDev *outA, 
              XWDict *resDict,
              XWPDFRectangle *box, 
              XWPDFRectangle *cropBox,
              bool (*abortCheckCbkA)(void *data) = 0,
              void *abortCheckCbkDataA = 0);
    ~XWGraphix();
    
    void display(XWObject *obj, bool topLevel = true);
    void drawAnnot(XWObject *str, 
                   XWAnnotBorderStyle *borderStyle,
		           double xMin, 
		           double yMin, 
		           double xMax, 
		           double yMax);
		void drawForm(XWObject *str, 
		              XWDict *resDict, 
		              double *matrix, 
		              double *bbox,
									bool transpGroup = false, 
									bool softMask = false,
									XWColorSpace *blendingColorSpace = 0,
									bool isolated = false, 
									bool knockout = false,
									bool alpha = false, 
									XWFunction *transferFunc = 0,
									CoreColor *backdropColor = 0);
		           
    XWGraphixState *getState() { return state; }
    
    void restoreState();
    
    void saveState();
    
private:
	XWStream *buildImageStream();
	
	bool checkArg(XWObject *arg, TchkType type);
	
    void doAxialShFill(XWAxialShading *shading);
    void doEndPath();
    void doForm(XWObject *str);
    void doForm1(XWObject *str, 
                 XWDict *resDict, 
                 double *matrix, 
                 double *bbox,
	             bool transpGroup = false, 
	             bool softMask = false,
	             XWColorSpace *blendingColorSpace = 0,
	             bool isolated = false, 
	             bool knockout = false,
	             bool alpha = false, 
	             XWFunction *transferFunc = 0,
	             CoreColor *backdropColor = 0);
    void doFunctionShFill(XWFunctionShading *shading);
    void doFunctionShFill1(XWFunctionShading *shading,
			               double x0, double y0,
			               double x1, double y1,
			               CoreColor *colors, int depth);
    void doGouraudTriangleShFill(XWGouraudTriangleShading *shading);
    void doImage(XWObject *ref, XWStream *str, bool inlineImg);
    void doIncCharCount(XWString *s);
    void doPatchMeshShFill(XWPatchMeshShading *shading);
    void doPatternFill(bool eoFill);
    void doPatternImageMask(XWObject *ref, 
                            XWStream *str, 
                            int width, 
                            int height,
			                      bool invert, 
			                      bool inlineImg);
    void doPatternStroke();
    void doPatternText();
    void doRadialShFill(XWRadialShading *shading);
    void doShadingPatternFill(XWShadingPattern *sPat, bool stroke, bool eoFill, bool text);    
    void doShowText(XWString *s);
    void doSoftMask(XWObject *str, 
                    bool alpha,
		            XWColorSpace *blendingColorSpace,
		            bool isolated, 
		            bool knockout,
		            XWFunction *transferFunc, 
		            CoreColor *backdropColor);
    void doTilingPatternFill(XWTilingPattern *tPat, bool stroke, bool eoFill, bool text);
			   
    void execOp(XWObject *cmd, XWObject args[], int numArgs);
    
    void fillPatch(XWPatch *patch, int nComps, int depth);
    Operator *findOp(const char *name);
    
    int  getPos();
    void go(bool topLevel);
    void gouraudFillTriangle(double x0, double y0, CoreColor *color0,
			                 double x1, double y1, CoreColor *color1,
			                 double x2, double y2, CoreColor *color2,
			                 int nComps, int depth);
        
    void opBeginIgnoreUndef(XWObject [], int);
    void opBeginImage(XWObject [], int);
    void opBeginMarkedContent(XWObject args[], int numArgs);
    void opBeginText(XWObject [], int);
    void opClip(XWObject [], int);
    void opCloseEOFillStroke(XWObject [], int);
    void opCloseFillStroke(XWObject [], int);
    void opClosePath(XWObject [], int);
    void opCloseStroke(XWObject [], int);
    void opConcat(XWObject args[], int);
    void opCurveTo(XWObject args[], int);
    void opCurveTo1(XWObject args[], int);
    void opCurveTo2(XWObject args[], int);
    void opEndIgnoreUndef(XWObject [], int);
    void opEndImage(XWObject [], int);
    void opEndMarkedContent(XWObject [], int);
    void opEndPath(XWObject [], int);
    void opEndText(XWObject [], int);
    void opEOClip(XWObject [], int);
    void opEOFill(XWObject [], int);
    void opEOFillStroke(XWObject [], int);
    void opFill(XWObject [], int);
    void opFillStroke(XWObject [], int);
    void opImageData(XWObject [], int);
    void opLineTo(XWObject args[], int);
    void opMarkPoint(XWObject [], int);
    void opMoveSetShowText(XWObject args[], int);
    void opMoveShowText(XWObject args[], int);
    void opMoveTo(XWObject args[], int);
    void opRectangle(XWObject args[], int);
    void opRestore(XWObject [], int);
    void opSave(XWObject [], int);
    void opSetCacheDevice(XWObject args[], int);
    void opSetCharSpacing(XWObject args[], int);
    void opSetCharWidth(XWObject args[], int);    
    void opSetDash(XWObject args[], int);
    void opSetExtGState(XWObject args[], int);
    void opSetFillCMYKColor(XWObject args[], int);
    void opSetFillColor(XWObject args[], int numArgs);
    void opSetFillColorN(XWObject args[], int numArgs);
    void opSetFillColorSpace(XWObject args[], int);
    void opSetFillGray(XWObject args[], int);
    void opSetFillRGBColor(XWObject args[], int);
    void opSetFlat(XWObject args[], int);
    void opSetFont(XWObject args[], int);
    void opSetHorizScaling(XWObject args[], int);
    void opSetLineCap(XWObject args[], int);
    void opSetLineJoin(XWObject args[], int);
    void opSetLineWidth(XWObject args[], int);
    void opSetMiterLimit(XWObject args[], int);
    void opSetRenderingIntent(XWObject [], int);    
    void opSetStrokeCMYKColor(XWObject args[], int);
    void opSetStrokeColor(XWObject args[], int numArgs);    
    void opSetStrokeColorN(XWObject args[], int numArgs);
    void opSetStrokeColorSpace(XWObject args[], int);
    void opSetStrokeGray(XWObject args[], int);
    void opSetStrokeRGBColor(XWObject args[], int);
    void opSetTextLeading(XWObject args[], int);
    void opSetTextMatrix(XWObject args[], int);
    void opSetTextRender(XWObject args[], int);
    void opSetTextRise(XWObject args[], int);
    void opSetWordSpacing(XWObject args[], int);
    void opShFill(XWObject args[], int);
    void opShowSpaceText(XWObject args[], int);
    void opShowText(XWObject args[], int);
    void opStroke(XWObject [], int);
    void opTextMove(XWObject args[], int);
    void opTextMoveSet(XWObject args[], int);
    void opTextNextLine(XWObject [], int);
    void opXObject(XWObject args[], int);
    
    void popResources();
    void pushResources(XWDict *resDict);
    
    void restoreStateStack(XWGraphixState *oldState);
    XWGraphixState *saveStateStack();
    
private:
		XWDoc *doc;
    XWRef *xref;
    XWOutputDev *out;
    bool subPage;
    bool printCommands;
    XWGraphixResources *res;
    int updateLevel;
    
    XWGraphixState *state;
    bool fontChanged;
    GraphicsClipType clip;
    int ignoreUndef;
    double baseMatrix[6];
    
    int formDepth;
    
    double textClipBBox[4];	// text clipping bounding box
  	bool textClipBBoxEmpty;	// true if textClipBBox has not been
				//   initialized yet
  	bool ocState;		// true if drawing is enabled, false if
				//   disabled
  	XWList *markedContentStack;
    
    XWPDFParser *parser;
    
    bool (*abortCheckCbk)(void *data);
    void *abortCheckCbkData;
    
    static Operator opTab[];
};


#endif // XWGRAPHIX_H

