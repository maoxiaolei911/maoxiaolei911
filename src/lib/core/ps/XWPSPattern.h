/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSPATTERN_H
#define XWPSPATTERN_H

#include "XWPSType.h"

#define max_cached_patterns_LARGE 50
#define max_pattern_bits_LARGE 100000
#define max_cached_patterns_SMALL 5
#define max_pattern_bits_SMALL 1000

class XWPSPattern1Instance;
class XWPSPattern2Instance;
class XWPSPattern;
class XWPSPattern1;
class XWPSPattern2;
class XWPSState;
class XWPSDevice;
class XWPSClientColor;
class XWPSDeviceColor;
class XWPSColorSpace;
class XWPSImagerState;
class XWPSShading;
class XWPSPatternInstance;

struct PSPatternType
{
	int PatternType;
	struct pp_
	{
		union
		{
			bool (XWPSPattern::*uses_base_space)();
			bool (XWPSPattern1::*uses_base_space1)();
			bool (XWPSPattern2::*uses_base_space2)();
		}uses_base_space_;
		
		union
		{
			int  (XWPSPattern::*make_pattern)(XWPSClientColor *,
			 										            const XWPSMatrix * , 
			 										            XWPSState *);
			int  (XWPSPattern1::*make_pattern1)(XWPSClientColor *,
			 										            const XWPSMatrix * , 
			 										            XWPSState *);
			int  (XWPSPattern2::*make_pattern2)(XWPSClientColor *,
			 										            const XWPSMatrix * , 
			 										            XWPSState *);
		}make_pattern_;
		
		union
		{
			XWPSPattern * (XWPSPatternInstance::*get_pattern)();
			XWPSPattern * (XWPSPattern1Instance::*get_pattern1)();
			XWPSPattern * (XWPSPattern2Instance::*get_pattern2)();
		}get_pattern_;
		
		union
		{
			int (XWPSImagerState::*remap_color)(XWPSClientColor *, 
																			    XWPSColorSpace * ,
		                                 			XWPSDeviceColor * ,		                                 
		                                 			XWPSDevice * ,
																		      PSColorSelect);
		};
	}procs;
};

class XWPSPatternInstance : public XWPSStruct
{
public:
	XWPSPatternInstance();
	virtual ~XWPSPatternInstance();
	
	virtual int getLength();
	virtual const char * getTypeName();
	XWPSPattern * getPattern();
	
	int remapColor(XWPSImagerState * pis, 
	               XWPSClientColor * pc, 
	               XWPSColorSpace * pcs,
	               XWPSDeviceColor * pdc, 
	               XWPSDevice * dev,
		             PSColorSelect select);
	
	bool usesBaseSpace();
	
public:
	PSPatternType type;
	XWPSState *saved;
};

class XWPSPattern : public XWPSStruct
{
public:
	XWPSPattern();
	virtual ~XWPSPattern();
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	int makePattern(XWPSClientColor *pcc,
			 										const XWPSMatrix * pmat, 
			 										XWPSState * pgs);
	int makePatternCommon(XWPSClientColor *pcc,
			 									const XWPSMatrix * pmat, 
			 									XWPSState * pgs,
			 									XWPSPatternInstance * pinst);
	
	bool usesBaseSpace();
	
public:
	PSPatternType type;
	int PatternType;
	XWPSUid uid;
	void *client_data;
};

class XWPSPattern1 : public XWPSPattern
{
public:
	XWPSPattern1();
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	int makePattern1(XWPSClientColor *pcc,
			 										const XWPSMatrix * pmat, 
			 										XWPSState * pgs);
	
	bool usesBaseSpacePattern1();
	
public:
	int PaintType;
  int TilingType;
  XWPSRect BBox;
  float XStep;
  float YStep;
  int (XWPSClientColor::*PaintProc)(XWPSState *);
};

class XWPSPattern2 : public XWPSPattern
{
public:
	XWPSPattern2();
	~XWPSPattern2();
	
	int getLength();
	const char * getTypeName();
	
	int makePattern2(XWPSClientColor *pcc,
			 										const XWPSMatrix * pmat, 
			 										XWPSState * pgs);
	
	bool usesBaseSpacePattern2() {return false;}
	
public:
	XWPSShading *Shading;
};

class XWPSPattern1Instance : public XWPSPatternInstance
{
public:
	XWPSPattern1Instance();
	
	int computeInstMatrix(XWPSRect * pbbox);
	
	int getLength();
	const char * getTypeName();
	XWPSPattern * getPattern1();
	
public:
	XWPSPattern1 templat;
	XWPSMatrix step_matrix;
	XWPSRect bbox;
	bool is_simple;
	bool uses_mask;
	XWPSIntPoint size;
	ulong id;
};

class XWPSPattern2Instance : public XWPSPatternInstance
{
public:
	XWPSPattern2Instance();
	
	int getLength();
	const char * getTypeName();
	XWPSPattern * getPattern2();
	
public:
	XWPSPattern2 templat;
};

#endif //XWPSPATTERN_H
