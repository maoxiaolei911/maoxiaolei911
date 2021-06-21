/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSCOMPOSITE_H
#define XWPSCOMPOSITE_H

#include "XWPSType.h"

class XWPSDevice;
class XWPSImagerState;
class XWPSComposite;
class XWPSCompositeAlpha;

#define alpha_out_notS_notD\
  (1<<composite_Dissolve)
#define _alpha_out_either\
  (alpha_out_notS_notD|(1<<composite_Satop)|(1<<composite_Datop)|\
    (1<<composite_Xor)|(1<<composite_PlusD)|(1<<composite_PlusL))
#define alpha_out_S_notD\
  (_alpha_out_either|(1<<composite_Copy)|(1<<composite_Sover)|\
    (1<<composite_Din)|(1<<composite_Dout))
#define alpha_out_notS_D\
  (_alpha_out_either|(1<<composite_Sin)|(1<<composite_Sout)|\
    (1<<composite_Dover)|(1<<composite_Highlight))
    

class PSPixelRow 
{
public:
	PSPixelRow();
	~PSPixelRow();
	
public:
	uchar *data;
  int bits_per_value;
  int initial_x;
  PSImageAlpha alpha;
};

struct PSCompositeTypeProcs
{
	union
	{
		int (XWPSComposite::*create_default_compositor)(XWPSDevice **,
		                                              XWPSDevice *, 
		                                              const XWPSImagerState *);
		int (XWPSCompositeAlpha::*create_default_compositoralpha)(XWPSDevice **,
		                                              XWPSDevice *, 
		                                              const XWPSImagerState *);
	}create_default_compositor_;
	
	union
	{
		bool (XWPSComposite::*equal)(XWPSComposite*);
		bool (XWPSCompositeAlpha::*equalalpha)(XWPSComposite*);
	}equal_;
	
	union
	{
		int  (XWPSComposite::*write)(uchar *, uint *);
		int  (XWPSCompositeAlpha::*writealpha)(uchar *, uint *);
	}write_;
};

struct PSCompositeType
{
	PSCompositeTypeProcs procs;
};

class XWPSComposite : public XWPSStruct
{
public:
	XWPSComposite();
	
	int createDefaultCompositor(XWPSDevice** pcdev,
	                            XWPSDevice * dev,
	                            const XWPSImagerState *pis);
	                            
	bool equal(XWPSComposite * pc2);
	
	virtual int getLength();
	virtual const char * getTypeName();
		
	int write(uchar * b, uint *s);
	
public:
	PSCompositeType type;
	ulong id;
};

class XWPSAlphaCompositeState
{
public:
	XWPSAlphaCompositeState();
	~XWPSAlphaCompositeState();
	
public:
	PSCompositeAlphaParams params;
	XWPSComposite *pcte;
  XWPSDevice *cdev;
  XWPSDevice *orig_dev;
};

class XWPSCompositeAlpha :  public XWPSComposite
{
public:
	XWPSCompositeAlpha() {}
	XWPSCompositeAlpha(const PSCompositeAlphaParams * pparams);
	
	int createDefaultCompositorDCA(XWPSDevice ** pcdev, 
	                                XWPSDevice * dev, 
	                                const XWPSImagerState * pis);
	
	bool equalDCA(XWPSComposite * pcte2);
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	static int readDCA(XWPSComposite ** ppcte, const uchar * data, uint size);
	
	int writeDCA(uchar * data, uint * psize);
	
public:
	PSCompositeAlphaParams params;
};

int composite_values(const PSPixelRow * pdest, 
                     const PSPixelRow * psource,
                     int values_per_pixel, 
                     uint num_pixels, 
                     const PSCompositeParams * pcp);
#endif //XWPSCOMPOSITE_H
