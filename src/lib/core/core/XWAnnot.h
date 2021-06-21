/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWANNOT_H
#define XWANNOT_H

#include "XWGlobal.h"

class XWDoc;
class XWRef;
class XWCatalog;
class XWGraphix;
class XWCoreFont;

#define annotFlagHidden    0x0002
#define annotFlagPrint     0x0004
#define annotFlagNoView    0x0020

#define fieldFlagReadOnly           0x00000001
#define fieldFlagRequired           0x00000002
#define fieldFlagNoExport           0x00000004
#define fieldFlagMultiline          0x00001000
#define fieldFlagPassword           0x00002000
#define fieldFlagNoToggleToOff      0x00004000
#define fieldFlagRadio              0x00008000
#define fieldFlagPushbutton         0x00010000
#define fieldFlagCombo              0x00020000
#define fieldFlagEdit               0x00040000
#define fieldFlagSort               0x00080000
#define fieldFlagFileSelect         0x00100000
#define fieldFlagMultiSelect        0x00200000
#define fieldFlagDoNotSpellCheck    0x00400000
#define fieldFlagDoNotScroll        0x00800000
#define fieldFlagComb               0x01000000
#define fieldFlagRichText           0x02000000
#define fieldFlagRadiosInUnison     0x02000000
#define fieldFlagCommitOnSelChange  0x04000000

#define fieldQuadLeft   0
#define fieldQuadCenter 1
#define fieldQuadRight  2

// distance of Bezier control point from center for circle approximation
// = (4 * (sqrt(2) - 1) / 3) * r
#define bezierCircle 0.55228475

enum AnnotBorderType 
{
    annotBorderSolid,
    annotBorderDashed,
    annotBorderBeveled,
    annotBorderInset,
    annotBorderUnderlined
};

class XW_CORE_EXPORT XWAnnotBorderStyle
{
public:
    XWAnnotBorderStyle(AnnotBorderType typeA, 
                       double widthA,
		               double *dashA, 
		               int dashLengthA,
		               double rA, 
		               double gA, 
		               double bA);
    ~XWAnnotBorderStyle();
    
    void getColor(double *rA, double *gA, double *bA)
        { *rA = r; *gA = g; *bA = b; }
    void getDash(double **dashA, int *dashLengthA)
        { *dashA = dash; *dashLengthA = dashLength; }
    AnnotBorderType getType() { return type; }
    double getWidth() { return width; }
    
private:
    AnnotBorderType type;
    double width;
    double *dash;
    int dashLength;
    double r, g, b;
};

class XW_CORE_EXPORT XWAnnot
{
public:
	XWAnnot(XWDoc *docA, XWDict *dict, ObjRef *refA);
    ~XWAnnot();
    
    void draw(XWGraphix *gfx, bool printing);
    
    XWString *getType() { return type; }
  	double getXMin() { return xMin; }
  	double getYMin() { return yMin; }
  	double getXMax() { return xMax; }
  	double getYMax() { return yMax; }
  	XWObject *getObject(XWObject *obj);
    
    void generateFieldAppearance(XWDict *field, XWDict *annot, XWDict *acroForm);
    XWObject *getAppearance(XWObject *obj) { return appearance.fetch(xref, obj); }
    XWAnnotBorderStyle *getBorderStyle() { return borderStyle; }
    
    bool isOk() { return ok; }
    
    bool match(ObjRef *refA)
    	{ return ref.num == refA->num && ref.gen == refA->gen; }
        
private:
    void drawCircle(double cx, double cy, double r, bool fill);
    void drawCircleBottomRight(double cx, double cy, double r);
    void drawCircleTopLeft(double cx, double cy, double r);
    void drawListBox(XWString **text, 
                     bool *selection,
		             int nOptions, 
		             int topIdx,
		             XWString *da, 
		             XWCoreFontDict *fontDict, 
		             int quadding);
    void drawText(XWString *text, 
                  XWString *da, 
                  XWCoreFontDict *fontDict,
		          bool multiline, 
		          int comb, 
		          int quadding,
		          bool txField, 
		          bool forceZapfDingbats, 
		          int rot);
		          
    XWObject *fieldLookup(XWDict *field, XWDict *acroForm, const char *key, XWObject *obj);
        
    void getNextLine(XWString *text, 
                             int start,
		                     XWCoreFont *font, 
		                     double fontSize, 
		                     double wMax,
		                     int *end, 
		                     double *width, 
		                     int *next);
		             		          
    void setColor(XWArray *a, bool fill, int adjust);
    
private:
	XWDoc * doc;
	XWRef *xref;
    ObjRef ref;
    XWString *type;
    XWObject appearance;
    XWString *appearanceState;
    XWString *appearBuf;
    double xMin, yMin, xMax, yMax;
    uint flags;
    XWAnnotBorderStyle *borderStyle;
    XWObject ocObj;
    bool ok;
};

class XW_CORE_EXPORT XWAnnots
{
public:
    XWAnnots(XWDoc *docA, XWObject *annotsObj);
    ~XWAnnots();
    
    void      generateAppearances(); 
    XWAnnot * getAnnot(int i) { return annots[i]; }
    int       getNumAnnots() { return nAnnots; }
			                  
private:
    XWAnnot *findAnnot(ObjRef *ref);
    
    void scanFieldAppearances(XWDict *node, 
                              ObjRef *ref, 
                              XWDict *parent,
			                  XWDict *acroForm);   
			                  
private:
	XWDoc *doc;
    XWAnnot **annots;
    int nAnnots;
};

#endif // XWANNOT_H

