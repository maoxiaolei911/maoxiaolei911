/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVIDEV_H
#define XWDVIDEV_H

#include "XWNumberUtil.h"
#include "XWDVIType.h"

#ifndef M_PI
#  define M_PI (4.0*atan(1.0))
#endif

class XWDVIRef;
class XWDVIDev;
class DevFont;
class XWDVICore;

#define PDF_DEV_PARAM_AUTOROTATE  1
#define PDF_DEV_PARAM_COLORMODE   2

#define DEV_COLOR_STACK_MAX 128

class XWDVIColorStack
{
public:
	XWDVIColorStack(XWDVIDev * devA);
	~XWDVIColorStack();
	
	void clear();
	
	void getCurrent(XWDVIColor **sc, XWDVIColor **fc);
		
	void pop(XWDVIRef * xref);
	void push(XWDVIRef * xref, XWDVIColor *sc, XWDVIColor *fc);
	
	void set(XWDVIRef * xref, XWDVIColor *sc, XWDVIColor *fc);
	
private:
	XWDVIDev   *  dev;
	int           current;
  	XWDVIColor * stroke;
  	XWDVIColor * fill;
};


struct PathElement
{
	int       type;
	PDFCoord p[3];
};
	
class XWDVIPath
{
public:
	XWDVIPath();
	XWDVIPath(const XWDVIPath * p0);
	~XWDVIPath();
	
	void clear();
	int  close(PDFCoord *cp);
	void copy(const XWDVIPath * p0);
	int  curveTo(PDFCoord       *cp,
                 const PDFCoord *p0,
                 const PDFCoord *p1,
                 const PDFCoord *p2);
                 
	int  ellipTArc(PDFCoord        *cp,
                   const PDFCoord *ca, 
                   double          r_x,
                   double          r_y,
                   double          xar,
                   double          a_0,
                   double          a_1,
                   int             a_d);
                   
	int flush(XWDVIRef * xref,
	          XWDVIDev * dev,
	          char      *b,
	          char       opchr,
              int        rule,
              int        ignore_rule);
	
	int isRect(int f_ir);
	
	int length() {return num_paths;}
	int lineTo(PDFCoord *cp, const PDFCoord *p0);
                  
	int moveTo(PDFCoord *cp, const PDFCoord *p0);
		
	int transform(const PDFTMatrix *M);
	
private:
	int grow(int max_pe);
	
	PathElement * nextPE(const PDFCoord *cp);
	
private:
	int       num_paths;
  	int       max_paths;
  	PathElement  * path;
};

class XWDVIGState
{
public:
	XWDVIGState();
	XWDVIGState(XWDVIGState * gs2);
	
	int arc(double c_x , double c_y, double r,
            double a_0 , double a_1);
	int arcn(double c_x , double c_y, double r,
             double a_0 , double a_1);
	int arcx(double c_x , double c_y,
             double r_x , double r_y,
             double a_0 , double a_1,
             int    a_d ,
             double xar);
             
	int  bspline(double x0, double y0,
                double x1, double y1, 
                double x2, double y2);
            
	void clear();
	int  closePath();
	void copy(XWDVIGState * gs2);
	int  curveTo(double x0, double y0,
                 double x1, double y1,
                 double x2, double y2);
                 
	void dtransform(PDFCoord *p, const PDFTMatrix *M);
	
	PDFTMatrix * getCTM() {return &matrix;}
	PDFCoord   * getCurrentPoint() {return &cp;}
	XWDVIColor * getFillColor() {return &fillcolor;}
	int          getFlags() {return flags;}
	int          getLineCap() {return linecap;}
	int          getLineJoin() {return linejoin;}
	double       getLineWidth() {return linewidth;}
	void         getMatrix(PDFTMatrix *M);
	double       getMiterLimit() {return miterlimit;}
	XWDVIPath  * getPath() {return &path;}
	XWDVIColor * getStrokeColor() {return &strokecolor;}
	
	void idtransform(PDFCoord *p, const PDFTMatrix *M);
	
	int lineTo(double x, double y);
	
	int moveTo(double x, double y);
	
	int rcurveTo(double x0, double y0,
                  double x1, double y1,
                  double x2, double y2);
	int rlineTo(double x, double y);
	int rmoveTo(double x, double y);
	
	void setFlags(int f) {flags = f;}
	void setFlatness(int v) {flatness = v;}	
	void setLineCap(int v) {linecap = v;}
	void setLineDash(int count, double *pattern, double offset);
	void setLineJoin(int v)  {linejoin = v;}
	void setLineWidth(double width) {linewidth = width;}
	void setMiterLimit(double v) {miterlimit = v;}
	
	void transform(PDFCoord *p, const PDFTMatrix *M);
	
private:
	void init();
	
private:
	PDFCoord   cp;
	PDFTMatrix matrix;
	XWDVIColor   strokecolor;
    XWDVIColor   fillcolor;
    
    struct LineDash
    {
    	int     num_dash;
    	double  pattern[PDF_DASH_SIZE_MAX];
    	double  offset;
  	} ;
  	
  	LineDash linedash;
  	double   linewidth;
  	int      linecap;
  	int      linejoin;
  	double   miterlimit;
  	int      flatness;
  	XWDVIPath path;
  	long      flags;
};

class XWDVIGStateStack
{
public:
	XWDVIGStateStack();
	~XWDVIGStateStack();
	
	int depth() {return size;}
	
	XWDVIGState * getTop();
	
	XWDVIGState * pop();
	void push(XWDVIGState * data);
	
	int restore();
	
	void save();
	
private:
	struct StackElement
	{
		XWDVIGState  * data;
		StackElement * prev;
	};
	
	int           size;
  	StackElement *top;
  	StackElement *bottom;
};

class XWDVIDev
{
public:
	XWDVIDev(XWDVICore * coreA, XWDVIRef * xref);
	~XWDVIDev();
	
	int arc(double c_x , double c_y, double r,
            double a_0 , double a_1);
	int arcn(double c_x , double c_y, double r,
             double a_0 , double a_1);
	int arcx(double c_x , double c_y,
             double r_x , double r_y,
             double a_0 , double a_1,
             int    a_d ,
             double xar);
             
	void bop(XWDVIRef * xref, const PDFTMatrix *M);
	int  bspline(double x0, double y0,
                double x1, double y1, 
                double x2, double y2);
	
	void clear(XWDVIRef * xref);
	void clearColorStack();
	int  clip(XWDVIRef * xref);
	int  closePath();
	int  concat(XWDVIRef * xref, const PDFTMatrix *M);
	int  currentDepth() {return gs_stack->depth() - 1;}
	int  currentMatrix(PDFTMatrix *M);
	int  currentPoint(PDFCoord *p);
	int  curveTo(double x0, double y0,
                double x1, double y1,
                double x2, double y2);
	
	void dtransform(PDFCoord *p, const PDFTMatrix *M);

	int  eoclip(XWDVIRef * xref);
	void eop(XWDVIRef * xref);
	
	int flushPath(XWDVIRef * xref, char p_op, int fill_rule);
	
	void getCoord(double *xpos, double *ypos);
	void getCurrentColor(XWDVIColor **sc, XWDVIColor **fc);
	int  getDirMode() {return text_state.dir_mode;}
	int  getFontWMode(int font_id);
	int  getParam(int param_type);
	void graphicsMode(XWDVIRef * xref);
	int  grestore(XWDVIRef * xref);
	void grestoreTo(XWDVIRef * xref, int depth);
	int  gsave(XWDVIRef * xref);
	
	void idtransform(PDFCoord *p, const PDFTMatrix *M);
	
	int lineTo(double x, double y);
	int locateFont(const char *font_name, long ptsize);

	int moveTo(double x, double y);

	int newPath(XWDVIRef * xref);	
	
	void popColor(XWDVIRef * xref);
	void popCoord();
	int  popGState();
	void pushColor(XWDVIRef * xref, XWDVIColor *sc, XWDVIColor *fc);
	void pushCoord(double xpos, double ypos);
	int  pushGState();
	int  putImage(XWDVIRef * xref,
	              int             id,
                  TransformInfo *p,
                  double          ref_x,
                  double          ref_y);
	
	int  rcurveTo(double x0, double y0,
                  double x1, double y1,
                  double x2, double y2);
	int  rectClip(XWDVIRef * xref, 
	              double x, double y,
                  double w, double h);
	int  rectFill(XWDVIRef * xref, 
	              double x, double y,
                  double w, double h);
	void resetColor(XWDVIRef * xref, int force);
	void resetFonts();
	int  rlineTo(double x, double y);
	int  rmoveTo(double x, double y);
	
	double scale() {return 1.0;}
	void   setAutoRotate(int v) {setParam(PDF_DEV_PARAM_AUTOROTATE, v);}
	void   setColor(XWDVIRef * xref, XWDVIColor *sc, XWDVIColor *fc);
	void   setColor(XWDVIRef * xref, 
	                XWDVIColor *color, 
	                char mask, 
	                int force);
	void   setDecimalDigits(int precision);
	void   setDirMode(int text_dir);
	void   setDvi2Pts(double dvi2pts);
	void   setIgnoreColors(int is_bw);
	int    setLineCap(XWDVIRef * xref, int capstyle);
	int    setLineDash(XWDVIRef * xref, 
	                   int count, 
	                   double *pattern, 
	                   double offset);
	int    setLineJoin(XWDVIRef * xref, int joinstyle);
	int    setLineWidth(XWDVIRef * xref, double width);
	int    setMiterLimit(XWDVIRef * xref, double mlimit);
	void   setNonStrokingColor(XWDVIRef * xref, XWDVIColor *color)
		{setColor(xref, color, 0x20, 0);}
	void   setParam(int param_type, int value);
	void   setRect(PDFRect *rect,
                  long x_user, 
                  long y_user,
                  long width,  
                  long height, 
                  long depth);
	void   setRule(XWDVIRef * xref,
	               long xpos, 
	               long ypos, 
	               long width, 
	               long height);
	void   setString(XWDVIRef * xref,
	                 long xpos, 
	                 long ypos,
                     const void *instr_ptr, 
                     int instr_len,
                     long width,
                     int   font_id, 
                     int ctype);
	void   setStrokingColor(XWDVIRef * xref, XWDVIColor *color) 
		{setColor(xref, color, 0, 0);}
	int    sprintCoord(char *buf, const PDFCoord *p);
	int    sprintLength (char *buf, double value);
	int    sprintMatrix(char *buf, const PDFTMatrix *M);
	static int sprintNumber(char *buf, double value);
	int    sprintRect(char *buf, const PDFRect *rect);
	
	void  transform(PDFCoord *p, const PDFTMatrix *M);
	
	double unitDVIUnit() {return (1.0/dev_unit.dvi2pts);}
	
private:
	long bpt2spt(double b) {return round(b / dev_unit.dvi2pts);}
	
	static int doubleToAscii(double value, int prec, char *buf);
	
	int flushPath(XWDVIRef * xref,
	              XWDVIPath  *pa,
                  char       opchr,
                  int        rule,
                  int        ignore_rule);
	
	static int intToAscii(long value, char *buf);
	
	int rectShape(XWDVIRef * xref,
                  const PDFRect    *r,
                  const PDFTMatrix *M,
                  char               opchr);
	void resetTextState(XWDVIRef * xref);
	
	int    setFont(XWDVIRef * xref, int font_id);
	void   setParamAutoRotate(int auto_rotate);
	void   setTextMatrix(XWDVIRef * xref, 
	                     long xpos, 
	                     long ypos, 
	                     double slant, 
	                     double extend, 
	                     int rotate);
	
	int    sprintBP(char *buf, long value, long *error);
	int    sprintLine(char *buf, 
	                  long width,
                 	  long p0_x, 
                 	  long p0_y, 
                 	  long p1_x, 
                 	  long p1_y);
	double spt2bpt(long s) {return s * dev_unit.dvi2pts;}
	void   startString(XWDVIRef * xref, 
	                   long xpos, 
	                   long ypos, 
	                   double slant, 
	                   double extend, 
	                   int rotate);
	void   stringMode(XWDVIRef * xref, 
	                  long xpos, 
	                  long ypos, 
	                  double slant, 
	                  double extend, 
	                  int rotate);
	
	void textMode(XWDVIRef * xref);
	
private:
	XWDVICore * core;
	
	struct DevUnit
	{
		double dvi2pts;
		long   min_bp_val;
		int    precision;
	};
	
	struct DevParam
	{
		int autorotate;
		int colormode;
	};
	
	struct Matrix
	{
    	double  slant;
    	double  extend;
    	int     rotate; /* TEXT_WMODE_XX */
  	};
  	
	struct TextState
	{
		int   font_id;
		long  offset;
		long  ref_x;
  		long  ref_y;
  		long  raise;
  		long  leading;
  		
  		Matrix matrix;
  		
  		double bold_param;
  		int    dir_mode;
  		int    force_reset;
  		int    is_mb;
	};
	
	DevUnit  dev_unit;
	DevParam dev_param;
	int motion_state;
	char * format_buffer;
	TextState text_state;
	
	DevFont ** dev_fonts;
	int num_dev_fonts;
	int max_dev_fonts;
	int num_phys_fonts;
	
	PDFCoord * dev_coords;
	int num_dev_coords;
	int max_dev_coords;
	
	XWDVIGStateStack * gs_stack;
	XWDVIColorStack  * color_stack;
};

#endif //XWDVIDEV_H

