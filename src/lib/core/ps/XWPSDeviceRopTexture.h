/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDEVICEROPTEXTURE_H
#define XWPSDEVICEROPTEXTURE_H

#include "XWPSDevice.h"
#include "XWPSColor.h"

class XWPSDeviceRopTexture : public XWPSDeviceForward
{
	Q_OBJECT 
	
public:
	XWPSDeviceRopTexture(QObject * parent = 0);
	~XWPSDeviceRopTexture();
	
	int copyColorRopTexture(const uchar *data, 
	                      int dx, 
	                      int raster, 
	                      ulong id,
	                      int x, 
	                      int y, 
	                      int w, 
	                      int h);
	int  copyDevice(XWPSDevice **pnew);
	void copyDeviceParamRopTexture(XWPSDeviceRopTexture * proto);
	int copyMonoRopTexture(const uchar *data, 
	                     int dx, 
	                     int raster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one);
	                     
	int fillRectangleRopTexture(int x, int y, int w, int h, ulong color);
	
	void makeRopTextureDevice(XWPSDevice * targetA,
	                          ulong log_opA, 
	                          XWPSDeviceColor * textureA);
	
public:
	ulong log_op;
  XWPSDeviceColor texture;
};

#endif //XWPSDEVICEROPTEXTURE_H
