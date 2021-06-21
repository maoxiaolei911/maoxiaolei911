/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDEVICEBBOX_H
#define XWPSDEVICEBBOX_H

#include "XWPSDevice.h"

struct PSDeviceBboxProcs
{
	bool (XWPSDeviceBbox::*init_box)();
	void (XWPSDeviceBbox::*get_box)(XWPSFixedRect *);
	void (XWPSDeviceBbox::*add_rect)(long, long, long, long);
	bool (XWPSDeviceBbox::*in_rect)(XWPSFixedRect *);
};

class XWPSDeviceBbox : public XWPSDeviceForward
{
	Q_OBJECT 
	
public:
	XWPSDeviceBbox(QObject * parent = 0);
	~XWPSDeviceBbox();
	
	void addIntRect(int x0, int y0, int x1, int y1);
	void addRect(long x0, long y0, long x1, long y1);
	void addRectDefault(long x0, long y0, long x1, long y1);
	void addRectForward(long x0, long y0, long x1, long y1);
	
	void bboxBbox(XWPSRect * pbbox);
	int  beginTypedImageBbox(XWPSImagerState * pis,
			                        XWPSMatrix *pmat, 
			                        XWPSImageCommon *pic,
			   											XWPSIntRect * prect,
			                        XWPSDeviceColor * pdcolor,
			                        XWPSClipPath * pcpath, 
			                        XWPSImageEnumCommon ** pinfo);
	
	int  closeBbox();
	int  copyAlphaBbox(const uchar *data, 
	                      int data_x,
	                      int raster, 
	                      ulong id, 
	                      int x, 
	                      int y, 
	                      int w, 
	                      int h,
	                      ulong color, 
	                      int depth);
	int  copyColorBbox(const uchar *data, 
	                      int dx, 
	                      int raster, 
	                      ulong id,
	                      int x, 
	                      int y, 
	                      int w, 
	                      int h);
	int  copyDevice(XWPSDevice **pnew);
	void copyDeviceParamBbox(XWPSDeviceBbox * proto);
	int  copyMonoBbox(const uchar *data, 
	                     int dx, 
	                     int raster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one);
	void copyParamsBbox(bool remap_colors);
	int  createCompositorBbox(XWPSDevice **pcdev, 
		                   XWPSComposite *pcte,
		                   XWPSImagerState *pis);
	
	int drawThinLineBbox(long fx0, 
                           long fy0, 
                           long fx1, 
                           long fy1,
                           XWPSDeviceColor *pdevc, 
                           ulong lop);
	
	int  fillMaskBbox(const uchar * data, 
	                      int dx, 
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
	int  fillParallelogramBbox(long px, 
	                               long py, 
	                               long ax, 
	                               long ay, 
	                               long bx, 
	                               long by,
	                               XWPSDeviceColor *pdevc, 
	                               ulong lop);
	int  fillPathBbox(XWPSImagerState * pis, 
	                      XWPSPath * ppath,
		                    XWPSFillParams * params,
	                      XWPSDeviceColor * pdevc, 
	                      XWPSClipPath * pcpath);
	int  fillRectangleBbox(int x, int y, int w, int h, ulong color);
	int  fillTriangleBbox(long px, 
	                         long py, 
	                         long ax, 
	                         long ay, 
	                         long bx, 
	                         long by,
	                         XWPSDeviceColor *pdevc, 
	                         ulong lop);
	int  fillTrapezoidBbox(XWPSFixedEdge *left, 
	                          XWPSFixedEdge *right,
	                          long ybot, 
	                          long ytop, 
	                          bool swap_axes,
	                          XWPSDeviceColor *pdevc, 
	                          ulong lop);
	void fwdOpenCloseBbox(bool foc) {forward_open_close=foc;}
	
	void getBoxDefault(XWPSFixedRect * pbox);
	void getBoxForward(XWPSFixedRect * pbox);
	int  getParamsBbox(XWPSContextState * ctx, XWPSParamList *plist);
	
	void initBbox(XWPSDevice * targetA);
	bool initBoxDefault();
	bool initBoxForward();
	bool inRectDefault(XWPSFixedRect * pbox);
	bool inRectForward(XWPSFixedRect * pbox);
	
	int openBbox();
	
	int  putParamsBbox(XWPSContextState * ctx, XWPSParamList *plist);
	
	void setWhiteOpaqueBbox(bool wio);
	int  stripCopyRopBbox(const uchar *sdata, 
	                         int sourcex, 
	                         uint sraster, 
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
	int  stripTileRectangleBbox(XWPSStripBitmap * tiles,
			      										 int x, 
			      										 int y, 
			      										 int w, 
			      										 int h,
			      										 ulong color0, 
			      										 ulong color1,
			      										 int px, 
			      										 int py);
	int strokePathBbox(XWPSImagerState * pis, 
	                       XWPSPath * ppath,
		                     XWPSStrokeParams * params,
	                       XWPSDeviceColor * pdevc, 
	                       XWPSClipPath *pcpath);
	                       
	int textBeginBbox(XWPSImagerState * pis,
		    								XWPSTextParams *text, 
		    								XWPSFont * font,
		    								XWPSPath * path, 
		    								XWPSDeviceColor * pdcolor,
		    								XWPSClipPath * pcpath,
		    								XWPSTextEnum ** ppte);
	
public:
	bool free_standing;
	bool forward_open_close;
	PSDeviceBboxProcs box_procs;
	void *box_proc_data;
	bool white_is_opaque;
	XWPSFixedRect bbox;
	ulong black, white;
	ulong transparent;
	
private:	
	void getBox(XWPSFixedRect * pbox);
	
	bool initBox();
	bool inRect(XWPSFixedRect * pbox);
};

#endif //XWPSDEVICEBBOX_H
