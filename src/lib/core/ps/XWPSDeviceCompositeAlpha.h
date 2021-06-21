/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDEVICCOMPOSITEALPHA_H
#define XWPSDEVICCOMPOSITEALPHA_H

#include "XWPSDevice.h"

class XWPSDeviceCompositeAlpha : public XWPSDeviceForward
{
	Q_OBJECT 
	
public:
	XWPSDeviceCompositeAlpha(QObject * parent = 0);
	
	int closeDCA() {return 0;}
	int copyAlphaDCA(const uchar * data, 
	                 int data_x,
	                 int raster, 
	                 ulong id, 
	                 int x, 
	                 int y, 
	                 int widthA, 
	                 int heightA,
	                 ulong color, 
	                 int depth);
	int copyColorDCA(const uchar * data,
	       					 int dx, 
	       					 int raster, 
	       					 ulong id,
	                 int x, 
	                 int y, 
	                 int w, 
	                 int h);
	int  copyDevice(XWPSDevice **pnew);
	void copyDeviceParamDCA(XWPSDeviceCompositeAlpha * proto);
	int copyMonoDCA(const uchar * data,
	                  int dx, 
	                  int raster, 
	                  ulong id, 
	                  int x, 
	                  int y, 
	                  int w, 
	                  int h,
	                   ulong zero, 
	                   ulong one);
	
	int fillRectangleDCA(int x, int y, int w, int h, ulong color);
	
	int mapColorRGBAlphaDCA(ulong color, ushort * prgba);
	int mapColorRGBDCA(ulong color, ushort * prgb);
	ulong mapRGBAlphaColorDCA(ushort red, ushort green, ushort blue, ushort alpha);
	ulong mapRGBColorDCA(ushort r, ushort g, ushort b);
	
public:
	PSCompositeAlphaParams params;
};

#endif //XWPSDEVICCOMPOSITEALPHA_H