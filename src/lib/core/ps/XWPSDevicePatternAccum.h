/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDEVICEPATTERNACCUM_H
#define XWPSDEVICEPATTERNACCUM_H

#include "XWPSDevice.h"

class XWPSDeviceMem;
class XWPSPattern1Instance;

class XWPSDevicePatternAccum : public XWPSDeviceForward
{
	Q_OBJECT 
	
public:
	XWPSDevicePatternAccum(QObject * parent = 0);
	~XWPSDevicePatternAccum();
	
	int closePatternAccum();
	int copyColorPatternAccum(const uchar * data, 
	                          int data_x,
		                        int raster, 
		                        ulong id, 
		                        int x, 
		                        int y, 
		                        int w, 
		                        int h);
	int  copyDevice(XWPSDevice **pnew);
	int copyMonoPatternAccum(const uchar * data, 
	                         int data_x,
		                       int raster, 
		                       ulong id, 
		                       int x, 
		                       int y, 
		                       int w, 
		                       int h,
			                     ulong color0, 
			                     ulong color1);
	
	int fillRectanglePatternAccum(int x, int y, int w, int h, ulong color);
	
	int getBitsRectanglePatternAccum(XWPSIntRect * prect,
		                               XWPSGetBitsParams * params, 
		                               XWPSIntRect ** unread);
	
	int openPatternAccum();
	
public:
	XWPSPattern1Instance *instance;
	XWPSDeviceMem *bits;
	XWPSDeviceMem *mask;
};

#endif //XWPSDEVICEPATTERNACCUM_H
