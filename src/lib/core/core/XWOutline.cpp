/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWString.h"
#include "XWList.h"
#include "XWLink.h"
#include "XWDocEncoding.h"
#include "XWOutline.h"

XWOutline::XWOutline(XWObject *outlineObj, XWRef *xref)
{
  XWObject first, last;

  items = NULL;
  if (!outlineObj->isDict()) {
    return;
  }
  outlineObj->dictLookupNF("First", &first);
  outlineObj->dictLookupNF("Last", &last);
  if (first.isRef() && last.isRef()) {
    items = XWOutlineItem::readItemList(&first, &last, xref);
  }
  first.free();
  last.free();
}

XWOutline::~XWOutline()
{
    if (items) 
        deleteXWList(items, XWOutlineItem);
}

XWOutlineItem::XWOutlineItem(XWDict *dict, XWRef *xrefA)
{
  XWObject obj1;
  XWString *s;
  int i;

  xref = xrefA;
  title = NULL;
  action = NULL;
  kids = NULL;

  if (dict->lookup("Title", &obj1)->isString()) {
    s = obj1.getString();
    if ((s->getChar(0) & 0xff) == 0xfe &&
	(s->getChar(1) & 0xff) == 0xff) {
      titleLen = (s->getLength() - 2) / 2;
      title = (uint *)malloc(titleLen * sizeof(uint));
      for (i = 0; i < titleLen; ++i) {
	title[i] = ((s->getChar(2 + 2*i) & 0xff) << 8) |
	           (s->getChar(3 + 2*i) & 0xff);
      }
    } else {
      titleLen = s->getLength();
      title = (uint *)malloc(titleLen * sizeof(uint));
      for (i = 0; i < titleLen; ++i) {
	title[i] = pdfDocEncoding[s->getChar(i) & 0xff];
      }
    }
  } else {
    titleLen = 0;
  }
  obj1.free();

  if (!dict->lookup("Dest", &obj1)->isNull()) {
    action = XWLinkAction::parseDest(&obj1);
  } else {
    obj1.free();
    if (!dict->lookup("A", &obj1)->isNull()) {
      action = XWLinkAction::parseAction(&obj1);
    }
  }
  obj1.free();

  dict->lookupNF("First", &firstRef);
  dict->lookupNF("Last", &lastRef);
  dict->lookupNF("Next", &nextRef);

  startsOpen = false;
  if (dict->lookup("Count", &obj1)->isInt()) {
    if (obj1.getInt() > 0) {
      startsOpen = true;
    }
  }
  obj1.free();
}

XWOutlineItem::~XWOutlineItem()
{
    close();
    if (title) 
        free(title);
        
    if (action) 
        delete action;
        
    firstRef.free();
    lastRef.free();
    nextRef.free();
}

void XWOutlineItem::close()
{
    if (kids) 
    {
        deleteXWList(kids, XWOutlineItem);
        kids = NULL;
    }
}

XWList * XWOutlineItem::readItemList(XWObject *firstItemRef, 
                                     XWObject *lastItemRef,
			                         XWRef *xrefA)
{
  XWList *items;
  XWOutlineItem *item;
  XWObject obj;
  XWObject *p, *refObj;
  int i;

  items = new XWList();
  if (!firstItemRef->isRef() || !lastItemRef->isRef()) {
    return items;
  }
  p = firstItemRef;
  do {
    if (!p->fetch(xrefA, &obj)->isDict()) {
      obj.free();
      break;
    }
    item = new XWOutlineItem(obj.getDict(), xrefA);
    obj.free();
    items->append(item);
    if (p->getRefNum() == lastItemRef->getRef().num &&
	p->getRefGen() == lastItemRef->getRef().gen) {
      break;
    }
    p = &item->nextRef;
    if (!p->isRef()) {
      break;
    }
    for (i = 0; i < items->getLength(); ++i) {
      refObj = (i == 0) ? firstItemRef
	                : &((XWOutlineItem *)items->get(i - 1))->nextRef;
      if (refObj->getRefNum() == p->getRefNum() &&
	  refObj->getRefGen() == p->getRefGen()) {
	p = NULL;
	break;
      }
    }
  } while (p);
  return items;
}

void XWOutlineItem::open()
{
    if (!kids) 
        kids = readItemList(&firstRef, &lastRef, xref);
}

