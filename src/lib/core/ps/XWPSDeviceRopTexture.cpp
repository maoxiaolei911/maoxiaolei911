/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPSDeviceRopTexture.h"


XWPSDeviceRopTexture::XWPSDeviceRopTexture(QObject * parent)
	:XWPSDeviceForward("rop source", 0,0,1,1, 1,1,1,0,2,0,parent)
{
	procs.fill_rectangle_.fill_rectangleropt = &XWPSDeviceRopTexture::fillRectangleRopTexture;
	procs.copy_mono_.copy_monoropt = &XWPSDeviceRopTexture::copyMonoRopTexture;
	procs.copy_color_.copy_colorropt = &XWPSDeviceRopTexture::copyColorRopTexture;
	log_op = 0;
}

XWPSDeviceRopTexture::~XWPSDeviceRopTexture()
{
}

int XWPSDeviceRopTexture::copyColorRopTexture(const uchar *data, 
	                                           int dx, 
	                                           int raster, 
	                                           ulong id,
	                                           int x, 
	                                           int y, 
	                                           int w, 
	                                           int h)
{
	PSRopSource source;
	source.sdata = data;
  source.sourcex = dx;
  source.sraster = raster;
  source.id = id;
  source.scolors[0] = source.scolors[1] = ps_no_color_index;
  source.use_scolors = false;
  return texture.fillRectangle(x, y, w, h, target, log_op, &source);
}

int  XWPSDeviceRopTexture::copyDevice(XWPSDevice **pnew)
{
	XWPSDeviceRopTexture * ret = new XWPSDeviceRopTexture;
	ret->copyDeviceParamRopTexture(this);
	*pnew = ret;
	return 0;
}

void XWPSDeviceRopTexture::copyDeviceParamRopTexture(XWPSDeviceRopTexture * proto)
{
	copyDeviceParam(proto);
	log_op = proto->log_op;
	texture = proto->texture;
}

int XWPSDeviceRopTexture::copyMonoRopTexture(const uchar *data, 
	                     int dx, 
	                     int raster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one)
{
	PSRopSource source;
  ulong lop = log_op;

  source.sdata = data;
  source.sourcex = dx;
  source.sraster = raster;
  source.id = id;
  source.scolors[0] = zero;
  source.scolors[1] = one;
  source.use_scolors = true;
    
  if (zero == ps_no_color_index)
		lop = rop3_use_D_when_S_0(lop);
  else if (one == ps_no_color_index)
		lop = rop3_use_D_when_S_1(lop);
  return texture.fillRectangle(x, y, w, h, target, lop, &source);
}

int XWPSDeviceRopTexture::fillRectangleRopTexture(int x, int y, int w, int h, ulong color)
{
	PSRopSource source;

  source.sdata = NULL;
  source.sourcex = 0;
  source.sraster = 0;
  source.id = ps_no_bitmap_id;
  source.scolors[0] = source.scolors[1] = color;
  source.use_scolors = true;
  return texture.fillRectangle(x, y, w, h, target,log_op, &source);
}

void XWPSDeviceRopTexture::makeRopTextureDevice(XWPSDevice * targetA,
	                                              ulong , 
	                                              XWPSDeviceColor * )
{
	setTarget(targetA);
}
