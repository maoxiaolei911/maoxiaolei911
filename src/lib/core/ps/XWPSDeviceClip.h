/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDEVICECLIP_H
#define XWPSDEVICECLIP_H

#include "XWPSDevice.h"
#include "XWPSPath.h"

class XWPSDeviceClip : public XWPSDeviceForward
{
	Q_OBJECT 
	
public:
	XWPSDeviceClip(QObject * parent = 0);
	~XWPSDeviceClip();
	
	int copyAlphaClip(const uchar *data, 
	                      int sourcex,
	                      int raster, 
	                      ulong id, 
	                      int x, 
	                      int y, 
	                      int w, 
	                      int h,
	                      ulong color, 
	                      int depth);
	int copyColorClip(const uchar *, 
	                      int , 
	                      int , 
	                      ulong ,
	                      int , 
	                      int , 
	                      int , 
	                      int);
	int  copyDevice(XWPSDevice **pnew);
	void copyDeviceParamClip(XWPSDeviceClip * proto);
	int copyMonoClip(const uchar *data, 
	                     int sourcex, 
	                     int raster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong color0, 
	                     ulong color1);
	
	int  fillMaskClip(const uchar * data, 
	                      int sourcex, 
	                      int raster, 
	                      ulong id,
		   									int x, 
		   									int y, 
		   									int w, 
		   									int h,
		   									XWPSDeviceColor * pdcolor, 
		   									int depth,
		   									ulong lop, 
		   									XWPSClipPath *pcpath);
	int fillRectangleClip(int x, int y, int w, int h, ulong color);
	
	int getBitsRectangleClip(XWPSIntRect *prect,
	                             XWPSGetBitsParams *params, 
	                             XWPSIntRect **unread);
	void getClipingBoxClip(XWPSFixedRect*pbox);
	
	void makeClipPathDevice(XWPSClipPath * pcpath);
	void makeClipTranslateDevice(XWPSClipList * listA, int tx, int ty);
	
	int openClip();
	
	int stripCopyRopClip(const uchar *sdata, 
	                         int sourcex, 
	                         uint raster, 
	                         ulong id,
	                         const ulong *scolors,
	                         XWPSStripBitmap *textures, 
	                         const ulong *tcolors,
	                         int x, 
	                         int y, 
	                         int w, 
	                         int h,
	                         int phase_x, 
	                         int phase_y, 
	                         ulong lop);
	int stripTileRectangleClip(XWPSStripBitmap * tiles,
			      										 int x, 
			      										 int y, 
			      										 int w, 
			      										 int h,
			      										 ulong color0, 
			      										 ulong color1,
			      										 int phase_x, 
			      										 int phase_y);
	
	void translate(XWPSClipList * listA, int tx, int ty);
	
public:
	XWPSClipList * list;
	XWPSClipRect *current;
	XWPSIntPoint translation;
  XWPSFixedRect clipping_box;
  bool clipping_box_set;
  
private:
	int clipEnumerate(int x, 
	                  int y, 
	                  int w, 
	                  int h,
	                  XWPSClipCallbackData * pccd,
	                  int (XWPSClipCallbackData::*process)(int xc, int yc, int xec, int yec));
	int clipEnumerateRest(int x, 
	                      int y, 
	                      int xe, 
	                      int ye,
	                      XWPSClipCallbackData * pccd,
		                    int (XWPSClipCallbackData::*process)(int xc, int yc, int xec, int yec));
};

#endif //XWPSDEVICECLIP_H
