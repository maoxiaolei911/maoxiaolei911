/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSparam.h"
#include "XWPSDevice.h"
#include "XWPSIODevice.h"
#include "XWPSState.h"
#include "XWPSContextState.h"

int XWPSContextState:: pushCallout(const char *callout_name)
{
	int code;

  code = checkEStack(1);
  if (code < 0)
  	return code;
  	
 	XWPSRef * esp = exec_stack.getCurrentTop();
  code = nameEnterString(callout_name, esp + 1);
  if (code < 0)
		return code;
    
  esp = exec_stack.incCurrentTop(1);
  esp->setAttrs(PS_A_EXECUTABLE);
  return PS_O_PUSH_ESTACK;
}

bool XWPSContextState::restorePageDevice(XWPSState * pgs_old, XWPSState * pgs_new)
{
	XWPSDevice *dev_old = pgs_old->currentDevice();
  XWPSDevice *dev_new;
  XWPSDevice *dev_t1;
  XWPSDevice *dev_t2;

  if ((dev_t1 = dev_old->getPageDevice()) == 0)
		return false;
  
  dev_new = pgs_new->currentDevice();
  if (dev_old != dev_new) 
  {
		if ((dev_t2 = dev_new->getPageDevice()) == 0)
	    return false;
		if (dev_t1 != dev_t2)
	    return true;
  }
  
  XWPSIntGState * iigs_old = (XWPSIntGState*)pgs_old->client_data;
  XWPSIntGState * iigs_new = (XWPSIntGState*)pgs_new->client_data;
  return !iigs_old->pagedevice.objEq(this,   &iigs_new->pagedevice);
}

bool XWPSContextState::savePageDevice(XWPSState *pgsA)
{
	XWPSIntGState * iigs = (XWPSIntGState*)pgsA->client_data;
	
	return 	(iigs->pagedevice.hasType(XWPSRef::Null) &&	 pgsA->currentDevice()->getPageDevice() != 0);
}

int XWPSContextState::zcallBeginPage()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSDevice *dev = pgs->currentDevice();

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if ((dev = dev->getPageDevice()) != 0) 
  {
		int code = dev->beginPage(pgs);

		if (code < 0)
	    return code;
  }
  pop(1);
  return 0;
}

int XWPSContextState::zcallEndPage()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSDevice *dev = pgs->currentDevice();
  int code;

  code = op[-1].checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if ((dev = dev->getPageDevice()) != 0) 
  {
		code = dev->endPage((int)op->value.intval, pgs);
		if (code < 0)
	    return code;
		if (code > 1)
			return (int)(XWPSError::RangeCheck);
  } 
  else 
		code = (op->value.intval == 2 ? 0 : 1);
			
  op[-1].makeBool(code);
  pop(1);
  return 0;
}

int XWPSContextState::zcallInstall()
{
	XWPSDevice *dev = pgs->currentDevice();
  
  if ((dev = dev->getPageDevice()) != 0) 
  {
		int code = dev->install(pgs);

		if (code < 0)
	    return code;
  }
  return 0;
}

int XWPSContextState::zcopyDevice2()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSDevice *new_dev;
  int code;

  code = op[-1].checkReadType(XWPSRef::Device);
  if (code < 0)
  	return code;
  	
  code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  code = op[-1].getDevice()->copyDevice2(&new_dev, op->value.boolval);
  if (code < 0)
		return code;
	
	new_dev->incRef();
	device_instances << new_dev;
  op[-1].makeDevice(idmemory()->iallocSpace() | PS_A_ALL, new_dev);
  pop(1);
  return 0;
}

int XWPSContextState::zcurrentDevice()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSDevice *dev = pgs->currentDevice();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  dev->incRef();
  op->makeDevice(PS_AVM_FOREIGN | PS_A_ALL,dev);
  return 0;
}

int XWPSContextState::zcurrentPageDevice()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSDevice *dev = pgs->currentDevice();

  int code = push(&op, 2);
  if (code < 0)
		return code;
  if (dev->getPageDevice() != 0) 
  {
  	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
		op[-1].assign(&iigs->pagedevice);
		op->makeTrue();
  } 
  else 
  {
		op[-1].makeNull();
		op->makeFalse();
  }
  return 0;
}

int XWPSContextState::zcurrentShowPageCount()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSDevice *dev = pgs->currentDevice();

  if (dev->getPageDevice() == 0) 
  {
		int code = push(&op, 1);
		if (code < 0)
			return code;
		op->makeFalse();
  } 
  else 
  {
		int code = push(&op, 2);
		if (code < 0)
			return code;
		op[-1].makeInt(dev->ShowpageCount);
		op->makeTrue();
  }
  return 0;
}

int XWPSContextState::zdeviceName()
{
	XWPSRef * op = op_stack.getCurrentTop();
  const char *dname;

  int code = op->checkReadType(XWPSRef::Device);
  if (code < 0)
		return code;
		
  dname = op->getDevice()->dname;
  op->makeString(PS_AVM_FOREIGN | PS_A_READONLY, strlen(dname),  (uchar *)dname);
  return 0;
}

int XWPSContextState::zdoneShowPage()
{
	XWPSDevice *dev = pgs->currentDevice();
  XWPSDevice *tdev = dev->getPageDevice();

  if (tdev != 0)
		tdev->ShowpageCount++;
  return 0;
}

int XWPSContextState::zflushPage()
{
	return pgs->flushPage();
}

int XWPSContextState::zgetBitsRect()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSDevice *dev;
  XWPSIntRect rect;
  XWPSGetBitsParams params;
  int w, h;
  ulong options =GB_ALIGN_ANY | GB_RETURN_COPY | GB_OFFSET_0 | GB_RASTER_STANDARD | GB_PACKING_CHUNKY;
  int depth;
  uint raster;
  int num_rows;
  int code;

  code = op[-7].checkReadType(XWPSRef::Device);
  if (code < 0)
		return code;
		
  dev = op[-7].getDevice();
  op[-6].checkIntLEU(dev->width);
  rect.p.x = op[-6].value.intval;
  op[-5].checkIntLEU(dev->height);
  rect.p.y = op[-5].value.intval;
  op[-4].checkIntLEU(dev->width);
  w = op[-4].value.intval;
  op[-3].checkIntLEU(dev->height);
  h = op[-3].value.intval;
  op[-2].checkType(XWPSRef::Integer);
  	
  if (op[-2].value.intval == -1)
		options |= GB_ALPHA_FIRST;
  else if (op[-2].value.intval == 0)
		options |= GB_ALPHA_NONE;
  else if (op[-2].value.intval == 1)
		options |= GB_ALPHA_LAST;
  else
		return (int)(XWPSError::RangeCheck);
  
  if (op[-1].hasType(XWPSRef::Null)) 
  {
		options |= GB_COLORS_NATIVE;
		depth = dev->color_info.depth;
  } 
  else 
  {
		static const ulong depths[17] = {
	    0, GB_DEPTH_1, GB_DEPTH_2, 0, GB_DEPTH_4, 0, 0, 0, GB_DEPTH_8,
	    0, 0, 0, GB_DEPTH_12, 0, 0, 0, GB_DEPTH_16};
		ulong depth_option;
		int std_depth;

		op[-1].checkIntLEU(16);
		std_depth = (int)op[-1].value.intval;
		depth_option = depths[std_depth];
		if (depth_option == 0)
			return (int)(XWPSError::RangeCheck);
				
		options |= depth_option | dev->gbcolorForDevice();
		depth = (dev->color_info.num_components +	 (options & GB_ALPHA_NONE ? 0 : 1)) * std_depth;
  }
  
  raster = (w * depth + 7) >> 3;
  op->checkWriteType(XWPSRef::String);
  num_rows = op->size() / raster;
  h = qMin(h, num_rows);
  if (h == 0)
		return (int)(XWPSError::RangeCheck);
			
  rect.q.x = rect.p.x + w;
  rect.q.y = rect.p.y + h;
  params.options = options;
  params.data[0] = op->getBytes();
  code = dev->getBitsRectangle(&rect, &params, NULL);
  if (code < 0)
		return code;
    
  op[-7].makeInt(h);
  op[-6].assign(op);
  op[-6].setSize(h * raster);
  pop(6);
  return 0;
}

int XWPSContextState::zgetDevice()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSDevice *dev;

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  if (op->value.intval != (int)(op->value.intval))
		return (int)(XWPSError::RangeCheck);
    
  dev = getDevice((int)(op->value.intval));
  if (dev == 0)	
		return (int)(XWPSError::RangeCheck);
	dev->incRef();
  op->makeDevice(PS_AVM_FOREIGN | PS_A_READONLY, dev);
  return 0;
}

int XWPSContextState::zgetDeviceParams(bool is_hardware)
{
	
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef rkeys;
  XWPSDevice *dev;
  int code;
  XWPSRef *pmark;

  code = op[-1].checkReadType(XWPSRef::Device);
  if (code < 0)
		return code;
		
  rkeys.assign(op);
  dev = op[-1].getDevice();
  pop(1);
  XWPSStackParamList list(op_stack.getStack(), &rkeys);
  list.writing = true;
  code = dev->getDeviceOrHWParams(&list, is_hardware);
  if (code < 0) 
  {
		if (list.count > 0)
	    op_stack.pop(list.count * 2 - 1);
		else
	    op_stack.push(1);
		op_stack.getCurrentTop()->assign(&rkeys);
		return code;
  }
  pmark = op_stack.index(list.count * 2);
  pmark->makeMark();
  return 0;
}

int XWPSContextState::zgetDeviceParams()
{
	return zgetDeviceParams(false);
}

int XWPSContextState::zgetDevParams()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSIODevice *iodev;
  int code;
  XWPSRef *pmark;

  code = op->checkReadType(XWPSRef::String);
  if (code < 0)
		return code;
		
  iodev = findIODevice(op->getBytes(), op->size());
  if (iodev == 0)
		return (int)(XWPSError::UndefinedFileName);
	
	XWPSStackParamList list(op_stack.getStack(), 0, (QObject*)0);
	list.writing = true;
  if ((code = iodev->getParams(&list)) < 0) 
  {
		op_stack.pop(list.count * 2);
		return code;
  }
  pmark = op_stack.index(list.count * 2);
  pmark->makeMark();
  return 0;
}

int XWPSContextState::zgetHardwareParams()
{
	return zgetDeviceParams(true);
}

int XWPSContextState::zgetIODevice()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSIODevice *iodev;
  uchar *dname;
    
  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  if (op->value.intval != (int)op->value.intval)
		return (int)(XWPSError::RangeCheck);
  
  iodev = getIODevice((int)(op->value.intval));
  if (iodev == 0)	
		return (int)(XWPSError::RangeCheck);
    
  dname = (uchar *)(iodev->dname);
  if (dname == 0)
		op->makeNull();
  else
		op->makeString(PS_A_READONLY | PS_AVM_FOREIGN, strlen((const char *)dname), dname);
  return 0;
}

int XWPSContextState::zmakeWordImageDevice()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  XWPSMatrix imat;
//  XWPSDevice *new_dev;
  uchar *colors;
  int colors_size;
  int code;

  code = op[-3].checkIntLEU(max_uint >> 1);
  if (code < 0)
		return code;
		
  code = op[-2].checkIntLEU(max_uint >> 1);	
  if (code < 0)
		return code;
		
  code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
		return code;
		
  if (op1->hasType(XWPSRef::Null)) 
  {	
		colors = 0;
		colors_size = -24;
  } 
  else if (op1->hasType(XWPSRef::Integer)) 
  {
		if (op1->value.intval != 16 && op1->value.intval != 24 &&
	    op1->value.intval != 32)
	  {
			return (int)(XWPSError::RangeCheck);
	  }
		colors = 0;
		colors_size = -op1->value.intval;
  } 
  else 
  {
		op1->checkType(XWPSRef::String);	
		if (op1->size() > 3 * 256)
			return (int)(XWPSError::RangeCheck);
		colors = op1->getBytes();
		colors_size = op1->size();
  }
  
  if ((code = op[-4].readMatrix(this, &imat)) < 0)
		return code;
		
	code = (int)(XWPSError::RangeCheck);
  
  return code;
}

int XWPSContextState::znullDevice()
{
	pgs->nullDevice();
	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
	
  iigs->pagedevice.makeNull();
  return 0;
}

int XWPSContextState::zoutputPage()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op[-1].checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  op->checkType(XWPSRef::Boolean);
  if (code < 0)
		return code;
		
	code = pgs->outputPage((int)op[-1].value.intval, op->value.boolval);
  if (code < 0)
		return code;
  pop(2);
  return 0;
}

int XWPSContextState::zputDeviceParams()
{
	uint count = op_stack.countToMark();
  XWPSRef *prequire_all;
  XWPSRef *ppolicy;
  XWPSRef *pdev;
  XWPSDevice *dev;
  int code;
  int old_width, old_height;
  int i, dest;

  if (count == 0)
		return (int)(XWPSError::UnmatchedMark);
	
  prequire_all = op_stack.index(count);
  ppolicy = op_stack.index(count + 1);
  pdev = op_stack.index(count + 2);
  if (pdev == 0)
		return (int)(XWPSError::StackUnderflow);
  
  code = prequire_all->checkTypeOnly(XWPSRef::Boolean);
  if (code < 0)
		return code;
		
  code = pdev->checkWriteTypeOnly(XWPSRef::Device);
  if (code < 0)
		return code;
		
  dev = pdev->getDevice();
  XWPSStackParamList list(op_stack.getStack(), 0, ppolicy, prequire_all->value.boolval);
  old_width = dev->width;
  old_height = dev->height;
  code = dev->putDeviceParams(this, &list);
  
  for (dest = count - 2, i = 0; i < count >> 1; i++)
		if (list.results[i] < 0) 
		{
	    op_stack.index(dest)->assign(op_stack.index(count - (i << 1) - 2));
	    errorName(list.results[i], op_stack.index(dest - 1));
	    dest -= 2;
		}
    
  if (code < 0) 
  {	
		op_stack.pop(dest + 1);
		return 0;
  }
  if (code > 0 || (code == 0 && (dev->width != old_width || dev->height != old_height))) 
  {
		if (pgs->currentDevice() == dev) 
		{
	    bool was_open = dev->is_open;

	    code = pgs->setDeviceNoErase(dev);
	    if (was_open && code >= 0)
				code = 1;
		}
  }
  if (code < 0)
		return code;
    
  op_stack.pop(count + 1);
  XWPSRef * op = op_stack.getCurrentTop();
  op->makeBool(code);
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
	
  iigs->pagedevice.makeNull();
  return 0;
}

int XWPSContextState::zputDevParams()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSIODevice *iodev;
  int code;
  PSPassword system_params_password;

  code = op->checkReadType(XWPSRef::String);
  if (code < 0)
		return code;
		
  iodev = findIODevice(op->getBytes(), op->size());
  if (iodev == 0)
		return (int)(XWPSError::UndefinedFileName);
	
	XWPSStackParamList list(op_stack.getStack(), 1, NULL, false);
  code = dict_stack.getSystemDict()->dictReadPassword(this, "SystemParamsPassword", &system_params_password);
  if (code < 0)
		return code;
    
  code = list.checkPassword(this, &system_params_password);
  if (code != 0) 
		return (code < 0 ? code : XWPSError::InvalidAccess);
  code = iodev->putParams(&list);
  if (code < 0)
		return code;
  op_stack.pop(list.count * 2 + 2);
  return 0;
}

int XWPSContextState::zsetDevice()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op->checkWriteType(XWPSRef::Device);
  if (code < 0)
		return code;
		
  code = pgs->setDeviceNoErase(op->getDevice());
  if (code < 0)
		return code;
  op->makeBool(code != 0);
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
	
  iigs->pagedevice.makeNull();
  return code;
}

int XWPSContextState::zsetPageDevice()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  if (op->hasType(XWPSRef::Dictionary)) 
  {
		op->checkDictRead();
		code = zreadOnly();
		if (code < 0)
	    return code;
  } 
  else 
		op->checkType(XWPSRef::Null);
  
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->pagedevice.assign(op);
  pop(1);
  return 0;
}

int XWPSContextState::z2copy()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = zcopy();

  if (code >= 0)
		return code;
  
  if (!op->hasType(XWPSRef::AStruct))
		return code;
  return z2copyGState();
}

int XWPSContextState::z2copyGState()
{
	if (!savePageDevice(pgs))
		return zcopyGState();
  return pushCallout("%copygstatepagedevice");
}

int XWPSContextState::z2currentGState()
{
	if (!savePageDevice(pgs))
		return zcurrentGState();
  return pushCallout("%currentgstatepagedevice");
}

int XWPSContextState::z2grestore()
{
	if (!restorePageDevice(pgs, pgs->getSaved()))
		return pgs->restore();
  return pushCallout("%grestorepagedevice");
}

int XWPSContextState::z2grestoreAll()
{
	for (;;) 
	{
		if (!restorePageDevice(pgs, pgs->getSaved())) 
		{
	    bool done = !pgs->getSaved()->getSaved();

	    pgs->restore();
	    if (done)
				break;
		} 
		else
	    return pushCallout("%grestoreallpagedevice");
  }
  return 0;
}

int XWPSContextState::z2gsave()
{
	if (!savePageDevice(pgs))
		return pgs->save();
  return pushCallout("%gsavepagedevice");
}

int XWPSContextState::z2gstate()
{
	if (!savePageDevice(pgs))
		return zgstate();
  return pushCallout("%gstatepagedevice");
}

int XWPSContextState::z2restore()
{
	for (;;) 
	{
		if (!restorePageDevice(pgs, pgs->getSaved())) 
		{
	    if (!pgs->getSaved()->getSaved())
				break;
	    pgs->restore();
		} 
		else
	    return pushCallout("%restorepagedevice");
  }
  return zrestore();
}

int XWPSContextState::z2save()
{
	if (!savePageDevice(pgs))
		return zsave();
  return pushCallout("%savepagedevice");
}

int XWPSContextState::z2setGState()
{
	XWPSRef * op = op_stack.getCurrentTop();
	int code = op->checkType("igstateobj");
	if (code < 0)
		return code;
		
	XWPSIGStateObj * igs = (XWPSIGStateObj*)(op->getStruct());
	XWPSState * pn = (XWPSState*)(igs->gstate.getStruct());

  if (!restorePageDevice(pgs, pn))
		return zsetGState();
  return pushCallout("%setgstatepagedevice");
}
