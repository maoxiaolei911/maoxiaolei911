/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDEVICEMASKCLIP_H
#define XWPSDEVICEMASKCLIP_H

#include "XWPSDevice.h"
#include "XWPSBitmap.h"

#define tile_clip_buffer_request 300
#define tile_clip_buffer_size\
  ((tile_clip_buffer_request / arch_sizeof_long) * arch_sizeof_long)
  
class XWPSDeviceMem;
class XWPSBitmap;

class XWPSDeviceMaskClip : public XWPSDeviceForward
{
	Q_OBJECT 
	
public:
	XWPSDeviceMaskClip(QObject * parent = 0);
	~XWPSDeviceMaskClip();
	
	int copyAlphaMaskClip(const uchar * data, 
	                      int sourcex, 
	                      int raster, 
	                      ulong id,
												int x, 
												int y, 
												int w, 
												int h, 
												ulong color, 
												int depth);
	int copyAlphaTileClip(const uchar * data, 
	                      int sourcex, 
	                      int raster, 
	                      ulong id,
												int x, 
												int y, 
												int w, 
												int h, 
												ulong color, 
												int depth);
	int copyColorMaskClip(const uchar * data, 
	                      int sourcex, 
	                      int raster, 
	                      ulong id,
		     							  int x, 
		     							  int y, 
		     							  int w, 
		     							  int h);
	int copyColorTileClip(const uchar * data, 
	                      int sourcex, 
	                      int raster, 
	                      ulong id,
		     							  int x, 
		     							  int y, 
		     							  int w, 
		     							  int h);
	int  copyDevice(XWPSDevice **pnew);
	void copyDeviceParamMaskClip(XWPSDeviceMaskClip * proto);
	int copyMonoMaskClip(const uchar * data, 
	                     int sourcex, 
	                     int raster, 
	                     ulong id,
		    							 int x, 
		    							 int y, 
		    							 int w, 
		    							 int h,
		                   ulong color0, 
		                   ulong color1);
	int copyMonoTileClip(const uchar * data, 
	                     int sourcex, 
	                     int raster, 
	                     ulong id,
		    							 int x, 
		    							 int y, 
		    							 int w, 
		    							 int h,
		                   ulong color0, 
		                   ulong color1);
	
	int fillRectangleMaskClip(int x, 
	                          int y, 
	                          int w, 
	                          int h,
			                      ulong color);
	int fillRectangleTileClip(int x, 
	                          int y, 
	                          int w, 
	                          int h,
			                      ulong color);
			                      
	void getClipingBoxMaskClip(XWPSFixedRect * pbox);
	
	void makeTileDevice();
	int  maskClipInitialize(int proto,
												 XWPSBitmap * bits, 
												 XWPSDevice * tdev,
			                   int tx, 
			                   int ty);
			                      
	int stripCopyRopMaskClip(const uchar * data, 
	                         int sourcex, 
	                         uint raster, 
	                         ulong id,
			 										 const ulong * scolors,
	   											 XWPSStripBitmap * textures, 
	   											 const ulong * tcolors,
			                     int x, 
			                     int y, 
			                     int w, 
			                     int h,
		                       int phase_x, 
		                       int phase_y, 
		                       ulong lop);
	int stripCopyRopTileClip(const uchar * data, 
	                         int sourcex, 
	                         uint raster, 
	                         ulong id,
			 										 const ulong * scolors,
	   											 XWPSStripBitmap * textures, 
	   											 const ulong * tcolors,
			                     int x, 
			                     int y, 
			                     int w, 
			                     int h,
		                       int phase_x, 
		                       int phase_y, 
		                       ulong lop);
	int stripTileRectangleMaskClip(XWPSStripBitmap * tiles,
			       										 int x, 
			       										 int y, 
			       										 int w, 
			       										 int h,
			                           ulong color0, 
			                           ulong color1,
			                           int phase_x, 
			                           int phase_y);
			                           
	int tileClipInitialize(XWPSStripBitmap * tilesA,
		                     XWPSDevice * tdev, 
		                     int px, 
		                     int py);
	void tileClipSetPhase(int px, int py);
	
public:
	XWPSStripBitmap tiles;
	XWPSDeviceMem * mdev;
	XWPSIntPoint phase;
	union _b
	{
		uchar bytes[tile_clip_buffer_size];
		ulong longs[tile_clip_buffer_size / arch_sizeof_long];
  } buffer;
  
private:
	int clipRunsEnumerate(int (XWPSClipCallbackData::*process)(int xc, int yc, int xec, int yec),
		    									XWPSClipCallbackData * pccd);
};

#endif //XWPSDEVICEMASKCLIP_H

