/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPSError.h"
#include "XWPSPath.h"
#include "XWPSContextState.h"
#include "XWPSDeviceCPathAccum.h"

XWPSDeviceCPathAccum::XWPSDeviceCPathAccum(QObject * parent)
	:XWPSDevice("clip list accumulator", 0, 0, 1, 1, 1, 1, 1, 0, 2, 0, parent)
{
	procs.open__.openpathac = &XWPSDeviceCPathAccum::openCPathAccum;
	procs.close__.closepathac = &XWPSDeviceCPathAccum::closeCPathAccum;
	procs.fill_rectangle_.fill_rectanglepathac = &XWPSDeviceCPathAccum::fillRectangleCPathAccum;
	procs.get_clipping_box_.get_clipping_box = &XWPSDevice::getClipingBoxLarge;
}

XWPSDeviceCPathAccum::~XWPSDeviceCPathAccum()
{
}

void XWPSDeviceCPathAccum::begin()
{
	open();
}

int XWPSDeviceCPathAccum::closeCPathAccum()
{
	list.xmin = bbox.p.x;
  list.xmax = bbox.q.x;
  return 0;
}

int  XWPSDeviceCPathAccum::copyDevice(XWPSDevice **pnew)
{
	XWPSDeviceCPathAccum * ret = new XWPSDeviceCPathAccum;
	ret->copyDeviceParamCPathAccum(this);
	*pnew = ret;
	return 0;
}

void XWPSDeviceCPathAccum::copyDeviceParamCPathAccum(XWPSDeviceCPathAccum * proto)
{
	copyDeviceParam(proto);
	bbox = proto->bbox;
}

int XWPSDeviceCPathAccum::end(XWPSClipPath * pcpath)
{
	int code = close();
  XWPSClipPath apath;

  if (code < 0)
		return code;

  *(apath.rect_list->list) = list;
  list.init();
  apath.path.bbox.p.x = int2fixed(bbox.p.x);
  apath.path.bbox.p.y = int2fixed(bbox.p.y);
  apath.path.bbox.q.x = int2fixed(bbox.q.x);
  apath.path.bbox.q.y = int2fixed(bbox.q.y);
  apath.path.bbox_set = 1;
  if (list.isRectangle())
		apath.inner_box = apath.path.bbox;
  else 
  {
		apath.inner_box.p.x = apath.inner_box.p.y = 0;
		apath.inner_box.q.x = apath.inner_box.q.y = 0;
  }
  apath.setOuterBox();
  apath.path_valid = false;
  if (context_state)
  apath.id = context_state->nextIDS(1);
  pcpath->assign(&apath, false);
  return 0;
}

#define ACCUM_ALLOC(ar, px, py, qx, qy)\
	if (++(list.count) == 1)\
	  ar = &list.single;\
	else if ((ar = allocRect()) == 0)\
	  return (int)(XWPSError::VMError);\
	ACCUM_SET(ar, px, py, qx, qy)
#define ACCUM_SET(ar, px, py, qx, qy)\
	(ar)->xmin = px, (ar)->ymin = py, (ar)->xmax = qx, (ar)->ymax = qy;
	
/* Link or unlink a rectangle in the list. */
#define ACCUM_ADD_LAST(ar)\
	ACCUM_ADD_BEFORE(ar, list.tail)
#define ACCUM_ADD_AFTER(ar, rprev)\
	ar->prev = (rprev), (ar->next = (rprev)->next)->prev = ar,\
	  (rprev)->next = ar
#define ACCUM_ADD_BEFORE(ar, rnext)\
	(ar->prev = (rnext)->prev)->next = ar, ar->next = (rnext),\
	  (rnext)->prev = ar
#define ACCUM_REMOVE(ar)\
	ar->next->prev = ar->prev, ar->prev->next = ar->next
/* Free a rectangle that was removed from the list. */
#define ACCUM_FREE(ar)\
	if (--(list.count)) {\
	  delete ar;\
	}

int XWPSDeviceCPathAccum::fillRectangleCPathAccum(int x, int y, int w, int h, ulong )
{
	int xe = x + w, ye = y + h;
  XWPSClipRect *nr;
  XWPSClipRect *ar;
  XWPSClipRect *rptr;
  int ymin, ymax;
  
  if (y < clip_box.p.y)
		y = clip_box.p.y;
  if (ye > clip_box.q.y)
		ye = clip_box.q.y;
  if (y >= ye)
		return 0;
  if (x < clip_box.p.x)
		x = clip_box.p.x;
  if (xe > clip_box.q.x)
		xe = clip_box.q.x;
  if (x >= xe)
		return 0;
		
	if (x < bbox.p.x)
		bbox.p.x = x;
  if (y < bbox.p.y)
		bbox.p.y = y;
  if (xe > bbox.q.x)
		bbox.q.x = xe;
  if (ye > bbox.q.y)
		bbox.q.y = ye;

top:
	if (list.count == 0) 
	{
		list.count = 1;
		ACCUM_SET(&list.single, x, y, xe, ye);
		return 0;
  }
  
  if (list.count == 1) 
  {
		rptr = &list.single;
		if (x == rptr->xmin && xe == rptr->xmax &&
	    	y <= rptr->ymax && ye >= rptr->ymin) 
	  {
	    if (y < rptr->ymin)
				rptr->ymin = y;
	    if (ye > rptr->ymax)
				rptr->ymax = ye;
	    return 0;
		}
  }
  else
		rptr = list.tail->prev;
		
	if (y >= rptr->ymax) 
	{
		if (y == rptr->ymax && x == rptr->xmin && xe == rptr->xmax &&
	    	(rptr->prev == 0 || y != rptr->prev->ymax)) 
	  {
	    rptr->ymax = ye;
	    return 0;
		}
		ACCUM_ALLOC(nr, x, y, xe, ye);
		ACCUM_ADD_LAST(nr);
		return 0;
  } 
  else if (y == rptr->ymin && ye == rptr->ymax && x >= rptr->xmin) 
  {
		if (x <= rptr->xmax) 
		{
	    if (xe > rptr->xmax)
				rptr->xmax = xe;
	    return 0;
		}
		ACCUM_ALLOC(nr, x, y, xe, ye);
		ACCUM_ADD_LAST(nr);
		return 0;
  }
  
  ACCUM_ALLOC(nr, x, y, xe, ye);
  rptr = list.tail->prev;
  while (ye <= rptr->ymin)
		rptr = rptr->prev;
  ymin = rptr->ymin;
  ymax = rptr->ymax;
  if (ye > ymax)
  {
  	if (y >= ymax) 
  	{
	    ACCUM_ADD_AFTER(nr, rptr);
	    return 0;
		}
		
		ACCUM_ALLOC(ar, x, ymax, xe, ye);
		ACCUM_ADD_AFTER(ar, rptr);
		ye = nr->ymax = ymax;
  }
  
  if (ye < ymax)
  {
  	XWPSClipRect *rsplit = rptr;

		while (rsplit->ymax == ymax) 
		{
	    ACCUM_ALLOC(ar, rsplit->xmin, ye, rsplit->xmax, ymax);
	    ACCUM_ADD_AFTER(ar, rptr);
	    rsplit->ymax = ye;
	    rsplit = rsplit->prev;
		}
		ymax = ye;
  }
  
  if (y > ymin) 
  {
		XWPSClipRect *rbot = rptr, *rsplit;

		while (rbot->prev->ymin == ymin)
	    rbot = rbot->prev;
		for (rsplit = rbot;;) 
		{
	    ACCUM_ALLOC(ar, rsplit->xmin, ymin, rsplit->xmax, y);
	    ACCUM_ADD_BEFORE(ar, rbot);
	    rsplit->ymin = y;
	    if (rsplit == rptr)
				break;
	    rsplit = rsplit->next;
		}
		ymin = y;
  }
  nr->ymin = ymin;
  for (; rptr->ymin == ymin; rptr = rptr->prev)
  {
  	if (xe < rptr->xmin)
	    continue;
		if (x > rptr->xmax)
	    break;	
		if (xe > rptr->xmax) 
		{
	    rptr->xmax = nr->xmax;
		}
		ACCUM_FREE(nr);
		if (x >= rptr->xmin)
	    goto out;
		rptr->xmin = x;
		nr = rptr;
		ACCUM_REMOVE(rptr);
  }
  ACCUM_ADD_AFTER(nr, rptr);
  
out:
	if (list.count <= 1)
	{
		XWPSClipRect *single = list.head->next;

		if (single != list.tail) 
		{
	    list.single = *single;
	    if (single)
	    	delete single;
	    list.single.next = list.single.prev = 0;
		}
		if (list.tail)
			delete list.tail;
		if (list.head)
			delete list.head;
		list.head = 0;
		list.tail = 0;
	}
	
	if (y < ymin) 
	{
		ye = ymin;
		goto top;
  }
  return 0;
}

int XWPSDeviceCPathAccum::openCPathAccum()
{
	list.init();
	bbox.p.x = bbox.p.y = max_int;
  bbox.q.x = bbox.q.y = min_int;
  clip_box.p.x = clip_box.p.y = min_int;
  clip_box.q.x = clip_box.q.y = max_int;
  return 0;
}

void XWPSDeviceCPathAccum::setCBox(const XWPSFixedRect * pbox)
{
	 clip_box.p.x = fixed2int_var(pbox->p.x);
   clip_box.p.y = fixed2int_var(pbox->p.y);
   clip_box.q.x = fixed2int_var_ceiling(pbox->q.x);
   clip_box.q.y = fixed2int_var_ceiling(pbox->q.y);
}

XWPSClipRect * XWPSDeviceCPathAccum::allocRect()
{
	XWPSClipRect *ar = new XWPSClipRect;
	if (ar == 0)
		return 0;
  if (list.count == 2)
  {
  	XWPSClipRect *head = ar;
		XWPSClipRect *tail = new XWPSClipRect;
		XWPSClipRect *single = new XWPSClipRect;
		ar = new XWPSClipRect;
		head->ymin = min_int; 
		head->ymax = min_int;
		head->xmin = min_int;
		head->xmax = min_int;
		head->next = single;
		*single = list.single;
		single->prev = head;
		single->next = tail;
		tail->ymin = max_int; 
		tail->ymax = max_int;
		tail->xmin = max_int;
		tail->xmax = max_int;
		tail->prev = single;
		list.head = head;
		list.tail = tail;
  }
  
  return ar;
}
