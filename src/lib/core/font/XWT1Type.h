/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWT1TYPE_H
#define XWT1TYPE_H

#include "XWFontFileCFF.h"

struct T1CPath 
{
  	int     type;
  	int     num_args;
  	double  args[CS_ARG_STACK_MAX];
  	T1CPath *next;
};

struct T1Stem
{
	int id;
  	int dir;
  	double pos, del;
} ;

struct T1StemGroup
{
  	int    num_stems;
  	double stems[CS_STEM_ZONE_MAX];
};

class XWCFFIndex;

class XWT1CharDesc : public QObject
{
	Q_OBJECT
	
public:	
	friend class XWT1GInfo;
	
	XWT1CharDesc(QObject * parent = 0);
	~XWT1CharDesc();
	
	void buildCharPath(uchar **data, 
	                   uchar *endptr, 
	                   XWCFFIndex *subrs);
	                   
	void doPostProc();
	
	long encodeCharPath(double default_width, 
	                    double nominal_width,
			            uchar *dst, 
			            uchar *endptr);
			            
	void sortStems();
	
private:
	void addCharPath(int type, double *argv, int argn);
	void addPath(int t, int n);
	int  addStem(double pos, double del, int dir);
	
	void copyArgs(double *args1, double *args2, int count);
	
	void doCallOtherSubr();
	void doOperator1(uchar **data);
	void doOperator2(uchar **data, uchar *endptr);
	void doOtherSubr0();
	void doOtherSubr1();
	void doOtherSubr2();
	void doOtherSubr3();
	void doOtherSubr12();
	void doOtherSubr13();
	
	void getInteger(uchar **data, uchar *endptr);
	void getLongInt(uchar **data, uchar *endptr);
	int  getStem(int stem_id);
	
	void putNumbers(double *argv, 
	                int argn, 
	                uchar **dest, 
	                uchar *limit);

private:
	int flags;
  	struct 
  	{
    	double sbx, sby, wx, wy;
  	} sbw;
  	
  	struct 
  	{
    	double llx, lly, urx, ury;
  	} bbox;
  	
  	struct 
  	{
    	double asb, adx, ady;
    	uchar bchar, achar;
  	} seac;
  	
  	int      num_stems;
  	T1Stem   stems[CS_STEM_ZONE_MAX];
  	T1CPath *charpath;
  	T1CPath *lastpath;
};

class XWT1GInfo
{
public:
	XWT1GInfo();
	
	long convertCharString(uchar *dst, 
	                       long dstlen,
			   			   uchar *src, 
			   			   long srclen, 
			   			   XWCFFIndex *subrs,
			   			   double default_width, 
			   			   double nominal_width);
	
	int getMetrics(uchar *src, 
	               long srclen, 
	               XWCFFIndex *subrs);
	
public:
	int use_seac;
  	double wx, wy;
  	struct 
  	{
    	double llx, lly, urx, ury;
  	} bbox;
  	
  	struct 
  	{
    	double asb, adx, ady;
    	uchar bchar, achar;
  	} seac;
};

#endif //XWT1TYPE_H