/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVISPECIAL_H
#define XWDVISPECIAL_H

#include <QObject>
#include "XWDVIType.h"

class XWLexer;
class XWObject;
class XWDVIRef;
class XWDVICore;
class XWDVIDev;

class XWDVISpecial : public QObject
{
	Q_OBJECT
	
public:
	XWDVISpecial(XWDVICore * coreA,
	             XWDVIRef * xrefA,
	             XWDVIDev * devA,
	             const char *buffer, 
	             long sizeA,
		         double x_userA, 
		         double y_userA, 
		         double magA,
		         int typeA,
		         QObject * parent = 0);
	virtual ~XWDVISpecial();
	
	virtual bool exec(int, int ) = 0;
	
	static XWDVISpecial * parse(XWDVICore * coreA,
	                            XWDVIRef * xrefA,
	                            XWDVIDev * devA,
	                            const char *buffer, 
	                            long sizeA,
		                        double x_userA, 
		                        double y_userA, 
		                        double magA);
	
protected:
	int beginAnnot(XWObject *dict);
	
	int checkResourceStatus(const char *category, const char *resname);
	XWObject * createXGState(double a, 
	                         int f_ais, 
	                         char * resname, 
	                         XWObject * dict);
	
	int endAnnot();
	
	void makeTransmatrix(PDFTMatrix *M,
                         double xoffset, 
                         double yoffset,
                         double xscale,  
                         double yscale,
                         double rotate);
	
	int readColorColor(XWDVIColor *colorspec);
	int readColorPDF(XWDVIColor *colorspec);
	int readColorSpec(XWDVIColor *colorspec);	
	int readDimTrns(TransformInfo *ti, long *page_no, int syntax);
	int readDimTrnsDvips(TransformInfo *t);
	int readDimTrnsPDFM(TransformInfo *p, long *page_no);
	int readNumbers(double *values, int num_values);
	int readPDFColor(XWDVIColor *colorspec, XWDVIColor *defaultcolor);
	int resumeAnnot();
	
	int suspendAnnot();
	
protected:
	XWDVICore * core;
	XWDVIRef * xref;
	XWDVIDev * dev;
	XWLexer  * lexer;
	double x_user;
	double y_user;
  	double mag;  	
  	int type;
};

#endif //XWDVISPECIAL_H