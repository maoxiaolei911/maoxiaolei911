/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWOUTLINE_H
#define XWOUTLINE_H

#include "XWObject.h"


class XWString;
class XWList;
class XWRef;
class XWLinkAction;


class XW_CORE_EXPORT XWOutline
{
public:
    XWOutline(XWObject *outlineObj, XWRef *xref);
    ~XWOutline();
    
    XWList *getItems() { return items; }
    
private:
    XWList *items;
};

class XW_CORE_EXPORT XWOutlineItem 
{
public:
    XWOutlineItem(XWDict *dict, XWRef *xrefA);
    ~XWOutlineItem();
    
    void close();
    
    XWLinkAction *getAction() { return action; }
    XWList * getKids() { return kids; }
    uint   * getTitle() { return title; }
    int      getTitleLength() { return titleLen; }
    
    bool hasKids() { return firstRef.isRef(); }
    bool isOpen() { return startsOpen; }
    
    void open();
    
    static XWList *readItemList(XWObject *firstItemRef, 
                                XWObject *lastItemRef,
			                    XWRef *xrefA);
    
private:
    XWRef *xref;
    uint *title;
    int titleLen;
    XWLinkAction *action;
    XWObject firstRef;
    XWObject lastRef;
    XWObject nextRef;
    bool startsOpen;
    XWList *kids;	
};

#endif // XWOUTLINE_H

