/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTPICSPECIAL_H
#define XWTPICSPECIAL_H

#include "XWDVISpecial.h"

class TPICState;

class XWTPICSpecial : public XWDVISpecial
{
	Q_OBJECT
	
public:	
	XWTPICSpecial(XWDVICore * coreA,
	               XWDVIRef * xrefA,
	               XWDVIDev * devA,
	               const char *buffer, 
	               long sizeA,
		           double x_userA, 
		           double y_userA, 
		           double magA,
		           int typeA,
		           QObject * parent = 0);
	~XWTPICSpecial() {}
	
	static int check(XWLexer * lexerA); 
		
	bool exec(int, int);
		
protected:
	int  arc(TPICState *tp,
             const PDFCoord  *c,
           	 int              f_vp,
             double           da,
             double          *v);
	
	int currentPoint(long *pg, PDFCoord *cp);
	
	int doAr();
	int doBk();
	int doDa();
	int doDt();
	int doFp();
	int doIa();
	int doIp();
	int doPa();
	int doPn();
	int doSh();
	int doSp();
	int doTx();
	int doWh();
	
	int  polyLine(TPICState *tp,
                  const PDFCoord  *c,
                  int              f_vp,
                  double           da);
                
	int  setFillStyle(double g, double a, int f_ais);
	int  setLineStyle(double pn, double da);
	void setStyles(TPICState *tp,
	    		   const PDFCoord  *c,
	    		   int              f_fs,
	               int              f_vp,
	               double           pn,
	               double           da);
	void showPath(int f_vp, int f_fs);
	int  spline(TPICState *tp,
                const PDFCoord  *c,
                int              f_vp,
                double           da);
};

#endif //XWTPICSPECIAL_H