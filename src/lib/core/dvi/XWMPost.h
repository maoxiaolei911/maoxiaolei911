/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWMPOST_H
#define XWMPOST_H

#include "XWDVIImage.h"

#define PS_STACK_SIZE 1024

class XWLexer;

class XW_DVI_EXPORT XWMPost : public XWDVIImage
{
public:
	XWMPost(XWDVIRef * xref,
	        const char *identA, 
	        int  subtypeA,
	        int  formatA,
	        long page_noA, 	           
	        const QString & filenameA,
	        XWObject *dictA);
	           
	static int checkForMP(QIODevice *fp);
	
	static int doPage(XWDVICore * core,
	                  XWDVIRef * xref, 
	                  XWDVIDev * dev,
	                  QIODevice *image_file);
	
	static void eopCleanup(XWDVIRef * xref);
	static int  execInline(XWDVICore * core,
	                       XWDVIRef * xref, 
	                       XWDVIDev * dev,
	                       char **p, 
	                       char *endptr,
		 				   double x_user, 
		 				   double y_user);
	
	int load(XWDVICore * core,
	         XWDVIRef * xref, 
	         XWDVIDev * dev,
	         QIODevice *fp);
	         
	static void quit();
	
	static int scanBBox(char **pp, char *endptr, PDFRect *bbox);
	static int stackDepth() {return top_stack;}
	
private:
	static void clearFonts();
	static int  cvrArray(XWDVIRef * xref ,
	                     XWObject *array, 
	                     double *values, 
	                     int count);
	
	static int  devCTM(XWDVIDev * dev, PDFTMatrix *M);
	static int  doClear(XWDVIRef * xref);
	static int  doCurrentFont(XWDVIRef * xref);
	static int  doExch();
	static int  doFindFont(XWDVIRef * xref);
	static int  doMpostBindDef(XWDVICore * core,
	                           XWDVIRef * xref, 
	                           XWDVIDev * dev,
	                           const char *ps_code, 
	                           double x_user, 
	                           double y_user);
	static int doOperator(XWDVICore * core,
	                      XWDVIRef * xref, 
	                      XWDVIDev * dev,
	                      const char *token, 
	                      double x_user, 
	                      double y_user);
	static int  doScaleFont(XWDVIRef * xref);
	static int  doSetFont(XWDVIRef * xref, 
	                      XWDVIDev * dev);
	static int  doShow(XWDVIRef * xref, XWDVIDev * dev);
	static int  doTexFigOperator(XWDVICore * core,
	                             XWDVIRef * xref, 
	                             XWDVIDev * dev, 
	                             int opcode, 
	                             double x_user, 
	                             double y_user);
	
	static int  getOpCode(const char *token);
	
	static void init();
	static int  isFontDict(XWObject *dict);
	static int  isFontName(const char *token);
	
	static int  parseBody(XWDVICore * core,
	                      XWDVIRef * xref, 
	                      XWDVIDev * dev, 
	                      XWLexer * lexer,
	                      double x_user, 
	                      double y_user);
	static XWObject * pop()
	       { return top_stack > 0 ? &(stack[--top_stack]) : 0;}
	static int push(XWObject * obj);
	
	static int popGetNumbers(XWDVIRef * xref, double *values, int count);

	static void restoreFont();

	static void saveFont();
	static int  setFont(XWDVIDev * dev,
	                    const char *font_name, 
	                    double pt_size);	                   
	static void skipProlog(XWLexer * lexer);
	static void stackClearTo(XWDVIRef * xref, int depth);
	
private:
	static XWObject * stack;
	static unsigned top_stack;
};

#endif //XWMPOST_H

