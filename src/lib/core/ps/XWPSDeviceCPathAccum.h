/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDEVICECPATHACCUM_H
#define XWPSDEVICECPATHACCUM_H

#include "XWPSDevice.h"
#include "XWPSPath.h"

class XWPSDeviceCPathAccum : public XWPSDevice
{
	Q_OBJECT 
	
public:
	XWPSDeviceCPathAccum(QObject * parent = 0);
	~XWPSDeviceCPathAccum();
	
	void begin();
	
	int closeCPathAccum();
	int  copyDevice(XWPSDevice **pnew);
	void copyDeviceParamCPathAccum(XWPSDeviceCPathAccum * proto);
	
	int end(XWPSClipPath * pcpath);
	
	int fillRectangleCPathAccum(int x, int y, int w, int h, ulong color);
	
	int openCPathAccum();
	
	void setCBox(const XWPSFixedRect * pbox);
	
public:
	XWPSIntRect clip_box;
  XWPSIntRect bbox;
  XWPSClipList list;
  
private:
	XWPSClipRect * allocRect();
};

#endif //XWPSDEVICECPATHACCUM_H
