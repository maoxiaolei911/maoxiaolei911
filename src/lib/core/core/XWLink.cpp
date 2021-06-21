/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stddef.h>
#include <string.h>
#include "XWString.h"
#include "XWObject.h"
#include "XWArray.h"
#include "XWDict.h"
#include "XWLink.h"

XWString * XWLinkAction::getFileSpecName(XWObject *fileSpecObj)
{
    if (!fileSpecObj)
        return 0;
        
    XWString * name = 0;
    XWObject obj1;
    if (fileSpecObj->isString())
        name = fileSpecObj->getString()->copy();
    else if (fileSpecObj->isDict())
    {
#ifdef Q_OS_WIN
        if (!fileSpecObj->dictLookup("DOS", &obj1)->isString())
        {
#else
        if (!fileSpecObj->dictLookup("Unix", &obj1)->isString())
        {
#endif
            obj1.free();
            fileSpecObj->dictLookup("F", &obj1);
        }
        
        if (obj1.isString()) 
            name = obj1.getString()->copy();
        obj1.free();
    }
    else
        return 0;
        
    if (name)
    {
#ifdef Q_OS_WIN
        int i = 0;
        if (name->getChar(0) == '/') 
        {
            if (name->getLength() >= 2 && name->getChar(1) == '/') 
            {
	            name->del(0);
	            i = 0;
            } 
            else if (name->getLength() >= 2 && 
		            ((name->getChar(1) >= 'a' && name->getChar(1) <= 'z') ||
		            (name->getChar(1) >= 'A' && name->getChar(1) <= 'Z')) && 
		            (name->getLength() == 2 || name->getChar(2) == '/')) 
		    {
	            name->setChar(0, name->getChar(1));
	            name->setChar(1, ':');
	            i = 2;
            } 
            else 
            {
                int j = 2;
	            for (; j < name->getLength(); ++j) 
	            {
	                if (name->getChar(j-1) != '\\' && name->getChar(j) == '/') 
	                {
	                    break;
	                }
	            }
	            if (j < name->getLength()) 
	            {
	                name->setChar(0, '\\');
	                name->insert(0, '\\');
	                i = 2;
	            }
            }
        }
        for (; i < name->getLength(); ++i) 
        {
            if (name->getChar(i) == '/') 
            {
	            name->setChar(i, '\\');
            } 
            else if (name->getChar(i) == '\\' &&
		            i+1 < name->getLength() &&
		            name->getChar(i+1) == '/') 
		    {
	            name->del(i);
            }
        }
#else

#endif
    }
    
    return name;
}

XWLinkAction * XWLinkAction::parseAction(XWObject *obj, XWString * baseURI )
{
    if (!obj || !obj->isDict())
        return 0;
        
    XWObject obj2, obj3, obj4;
    obj->dictLookup("S", &obj2);
    XWLinkAction *action = 0;
    // GoTo action
    if (obj2.isName("GoTo")) 
    {
        obj->dictLookup("D", &obj3);
        action = new XWLinkGoTo(&obj3);
        obj3.free();
    } 
    else if (obj2.isName("GoToR")) 
    {
        obj->dictLookup("F", &obj3);
        obj->dictLookup("D", &obj4);
        action = new XWLinkGoToR(&obj3, &obj4);
        obj3.free();
        obj4.free();

    } 
    else if (obj2.isName("Launch")) 
        action = new XWLinkLaunch(obj);
    else if (obj2.isName("URI")) 
    {
        obj->dictLookup("URI", &obj3);
        action = new XWLinkURI(&obj3, baseURI);
        obj3.free();        
    } 
    else if (obj2.isName("Named")) 
    {
        obj->dictLookup("N", &obj3);
        action = new XWLinkNamed(&obj3);
        obj3.free();
    } 
    else if (obj2.isName("Sound")) 
    {
    	obj->dictLookupNF("Annot", &obj3);
    	action = new XWLinkSound(&obj3);
        obj3.free();
    }
    else if (obj2.isName("Movie")) 
    {
        obj->dictLookupNF("Annot", &obj3);
        obj->dictLookup("T", &obj4);
        action = new XWLinkMovie(&obj3, &obj4);
        obj3.free();
        obj4.free();
    } 
    else if (obj2.isName()) 
        action = new XWLinkUnknown(obj2.getName());

    obj2.free();

    if (action && !action->isOk()) 
    {
        delete action;
        return 0;
    }
    return action;
}

XWLinkAction * XWLinkAction::parseDest(XWObject *obj)
{
    XWLinkAction * action = new XWLinkGoTo(obj);
    if (!action->isOk()) 
    {
        delete action;
        return 0;
    }
    return action;
}

XWLinkDest::XWLinkDest(XWArray *a)
{
    left = bottom = right = top = zoom = 0;
    ok = false;
    if (a->getLength() < 2) 
        return;
    
    XWObject obj1, obj2;
    a->getNF(0, &obj1);
    if (obj1.isInt()) 
    {
        pageNum = obj1.getInt() + 1;
        pageIsRef = false;
    } 
    else if (obj1.isRef()) 
    {
        pageRef.num = obj1.getRefNum();
        pageRef.gen = obj1.getRefGen();
        pageIsRef = true;
    } 
    else 
        goto err2;
    obj1.free();

    a->get(1, &obj1);
    if (obj1.isName("XYZ")) 
    {
        kind = LINK_DEST_XYZ;
        if (a->getLength() < 3) 
            changeLeft = false;
        else 
        {
            a->get(2, &obj2);
            if (obj2.isNull()) 
	            changeLeft = false;
            else if (obj2.isNum()) 
            {
	            changeLeft = true;
	            left = obj2.getNum();
            } 
            else 
	            goto err1;
            obj2.free();
        }
        
        if (a->getLength() < 4) 
            changeTop = false;
        else 
        {
            a->get(3, &obj2);
            if (obj2.isNull()) 
	            changeTop = false;
            else if (obj2.isNum()) 
            {
	            changeTop = true;
	            top = obj2.getNum();
            } 
            else 
	            goto err1;
            obj2.free();
        }
        
        if (a->getLength() < 5) 
            changeZoom = false;
        else 
        {
            a->get(4, &obj2);
            if (obj2.isNull()) 
            {
	            changeZoom = false;
            } 
            else if (obj2.isNum()) 
            {
	            changeZoom = true;
	            zoom = obj2.getNum();
            } 
            else 
	            goto err1;
            obj2.free();
        }
        // Fit link
    } 
    else if (obj1.isName("Fit")) 
    {
        if (a->getLength() < 2) 
            goto err2;
            
        kind = LINK_DEST_FIT;
    } 
    else if (obj1.isName("FitH")) 
    {
        if (a->getLength() < 3) 
            goto err2;
        
        kind = LINK_DEST_FITH;
        if (!a->get(2, &obj2)->isNum()) 
            kind = LINK_DEST_FIT;
        top = obj2.getNum();
        obj2.free();
    } 
    else if (obj1.isName("FitV")) 
    {
        if (a->getLength() < 3) 
            goto err2;
            
        kind = LINK_DEST_FITV;
        if (!a->get(2, &obj2)->isNum()) 
            kind = LINK_DEST_FIT;
        
        left = obj2.getNum();
        obj2.free();
    } 
    else if (obj1.isName("FitR")) 
    {
        if (a->getLength() < 6) 
            goto err2;
            
        kind = LINK_DEST_FITR;
        if (!a->get(2, &obj2)->isNum()) 
            kind = LINK_DEST_FIT;
            
        left = obj2.getNum();
        obj2.free();
        if (!a->get(3, &obj2)->isNum()) 
            kind = LINK_DEST_FIT;
        bottom = obj2.getNum();
        obj2.free();
        if (!a->get(4, &obj2)->isNum()) 
            kind = LINK_DEST_FIT;
        right = obj2.getNum();
        obj2.free();
        if (!a->get(5, &obj2)->isNum()) 
            kind = LINK_DEST_FIT;
        top = obj2.getNum();
        obj2.free();
    } 
    else if (obj1.isName("FitB")) 
    {
        if (a->getLength() < 2) 
            goto err2;
        kind = LINK_DEST_FITB;
    } 
    else if (obj1.isName("FitBH")) 
    {
        if (a->getLength() < 3) 
            goto err2;
            
        kind = LINK_DEST_FITBH;
        if (!a->get(2, &obj2)->isNum()) 
            kind = LINK_DEST_FIT;
        top = obj2.getNum();
        obj2.free();
    } 
    else if (obj1.isName("FitBV")) 
    {
        if (a->getLength() < 3) 
            goto err2;
        kind = LINK_DEST_FITBV;
        if (!a->get(2, &obj2)->isNum()) 
            kind = LINK_DEST_FIT;
        left = obj2.getNum();
        obj2.free();
    } 
    else 
        goto err2;

    obj1.free();
    ok = true;
    return;

err1:
    obj2.free();
err2:
    obj1.free();
}

XWLinkDest::XWLinkDest(XWLinkDest *dest)
{
    kind = dest->kind;
    pageIsRef = dest->pageIsRef;
    if (pageIsRef)
        pageRef = dest->pageRef;
    else
        pageNum = dest->pageNum;
    left = dest->left;
    bottom = dest->bottom;
    right = dest->right;
    top = dest->top;
    zoom = dest->zoom;
    changeLeft = dest->changeLeft;
    changeTop = dest->changeTop;
    changeZoom = dest->changeZoom;
    ok = true;
}

XWLinkGoTo::XWLinkGoTo(XWObject *destObj)
{    
    dest = 0;
    namedDest = 0;
    if (destObj->isName()) 
        namedDest = new XWString(destObj->getName());
    else if (destObj->isString()) 
        namedDest = destObj->getString()->copy();
    else if (destObj->isArray()) 
    {
        dest = new XWLinkDest(destObj->getArray());
        if (!dest->isOk()) 
        {
            delete dest;
            dest = 0;
        }
    } 
}

XWLinkGoTo::~XWLinkGoTo()
{
    if (dest)
        delete dest;
        
    if (namedDest)
        delete namedDest;
}

XWLinkGoToR::XWLinkGoToR(XWObject *fileSpecObj, XWObject *destObj)
{
    dest = 0;
    namedDest = 0;
    
    fileName = getFileSpecName(fileSpecObj);
    if (destObj->isName()) 
        namedDest = new XWString(destObj->getName());
    else if (destObj->isString()) 
        namedDest = destObj->getString()->copy();
    else if (destObj->isArray()) 
    {
        dest = new XWLinkDest(destObj->getArray());
        if (!dest->isOk()) 
        {
            delete dest;
            dest = 0;
        }
    }
}

XWLinkGoToR::~XWLinkGoToR()
{
    if (fileName)
        delete fileName;
        
    if (dest)
        delete dest;
        
    if (namedDest)
        delete namedDest;
}

XWLinkLaunch::XWLinkLaunch(XWObject *actionObj)
{
    fileName = 0;
    params = 0;
    
    XWObject obj1, obj2;        
    if (actionObj->isDict()) 
    {
        if (!actionObj->dictLookup("F", &obj1)->isNull()) 
            fileName = getFileSpecName(&obj1);
        else
        {
            obj1.free();
#ifdef Q_OS_WIN
            if (actionObj->dictLookup("Win", &obj1)->isDict()) 
            {
	            obj1.dictLookup("F", &obj2);
	            fileName = getFileSpecName(&obj2);
	            obj2.free();
	            if (obj1.dictLookup("P", &obj2)->isString()) 
	            {
	                params = obj2.getString()->copy();
	            }
	            obj2.free();
            } 
#else
            if (actionObj->dictLookup("Unix", &obj1)->isDict()) 
            {
	            obj1.dictLookup("F", &obj2);
	            fileName = getFileSpecName(&obj2);
	            obj2.free();
	            if (obj1.dictLookup("P", &obj2)->isString()) 
	                params = obj2.getString()->copy();
	            obj2.free();
            } 
#endif
        }
        obj1.free();
    }
}

XWLinkLaunch::~XWLinkLaunch()
{
    if (fileName)
        delete fileName;
        
    if (params)
        delete params;
}

XWLinkURI::XWLinkURI(XWObject *uriObj, XWString *baseURI)
{
  XWString *uri2;
  int n;
  char c;

  uri = NULL;
  if (uriObj->isString()) {
    uri2 = uriObj->getString();
    n = (int)strcspn(uri2->getCString(), "/:");
    if (n < uri2->getLength() && uri2->getChar(n) == ':') {
      // "http:..." etc.
      uri = uri2->copy();
    } else if (!uri2->cmpN("www.", 4)) {
      // "www.[...]" without the leading "http://"
      uri = new XWString("http://");
      uri->append(uri2);
    } else {
      // relative URI
      if (baseURI) {
	uri = baseURI->copy();
	c = uri->getChar(uri->getLength() - 1);
	if (c != '/' && c != '?') {
	  uri->append('/');
	}
	if (uri2->getChar(0) == '/') {
	  uri->append(uri2->getCString() + 1, uri2->getLength() - 1);
	} else {
	  uri->append(uri2);
	}
      } else {
	uri = uri2->copy();
      }
    }
  }
}

XWLinkURI::~XWLinkURI()
{
    if (uri)
        delete uri;
}

XWLinkNamed::XWLinkNamed(XWObject *nameObj)
{
    name = 0;
    if (nameObj->isName()) 
        name = new XWString(nameObj->getName());
}

XWLinkNamed::~XWLinkNamed()
{
    if (name) 
        delete name;
}

XWLinkSound::XWLinkSound(XWObject *annotObj)
{
	annotRef.num = -1;
    if (annotObj->isRef()) 
        annotRef = annotObj->getRef();
}

XWLinkSound::~XWLinkSound()
{
}

XWLinkMovie::XWLinkMovie(XWObject *annotObj, XWObject *titleObj)
{
    annotRef.num = -1;
    title = 0;
    if (annotObj->isRef()) 
        annotRef = annotObj->getRef();
    else if (titleObj->isString()) 
        title = titleObj->getString()->copy();
}

XWLinkMovie::~XWLinkMovie()
{
    if (title)
        delete title;
}

XWLinkUnknown::XWLinkUnknown(char *actionA)
{
    action = new XWString(actionA);
}

XWLinkUnknown::~XWLinkUnknown()
{
    if (action)
        delete action;
}

XWLink::XWLink(XWDict *dict, XWString *baseURI)
{
    action = 0;
    ok = false;
    
    XWObject obj1, obj2;
    if (!dict->lookup("Rect", &obj1)->isArray()) 
        goto err2;
    
    if (!obj1.arrayGet(0, &obj2)->isNum()) 
        goto err1;
    
    x1 = obj2.getNum();
    obj2.free();
    if (!obj1.arrayGet(1, &obj2)->isNum()) 
        goto err1;
    
    y1 = obj2.getNum();
    obj2.free();
    if (!obj1.arrayGet(2, &obj2)->isNum()) 
        goto err1;
    x2 = obj2.getNum();
    obj2.free();
    if (!obj1.arrayGet(3, &obj2)->isNum()) 
        goto err1;
    y2 = obj2.getNum();
    obj2.free();
    obj1.free();
    if (x1 > x2) 
    {
        double t = x1;
        x1 = x2;
        x2 = t;
    }
    
    if (y1 > y2) 
    {
        double t = y1;
        y1 = y2;
        y2 = t;
    }

    if (!dict->lookup("Dest", &obj1)->isNull()) 
        action = XWLinkAction::parseDest(&obj1);
    else 
    {
        obj1.free();
        if (dict->lookup("A", &obj1)->isDict()) 
            action = XWLinkAction::parseAction(&obj1, baseURI);
    }
    obj1.free();

    if (action) 
        ok = true;

    return;

err1:
    obj2.free();
err2:
    obj1.free();
}

XWLink::~XWLink()
{
    if (action)
        delete action;
}

XWLinks::XWLinks(XWObject *annots, XWString *baseURI)
{
    links = NULL;
    int size = 0;
    numLinks = 0;
    
    XWObject obj1, obj2;
    XWLink *link = 0;
    if (annots->isArray()) 
    {
        for (int i = 0; i < annots->arrayGetLength(); ++i) 
        {
            if (annots->arrayGet(i, &obj1)->isDict()) 
            {
	            if (obj1.dictLookup("Subtype", &obj2)->isName("Link")) 
	            {
	                link = new XWLink(obj1.getDict(), baseURI);
	                if (link->isOk()) 
	                {
	                    if (numLinks >= size) 
	                    {
	                        size += 16;
	                        links = (XWLink **)realloc(links, size * sizeof(XWLink *));
	                    }
	                    links[numLinks++] = link;
	                } 
	                else 
	                    delete link;
	            }
	            obj2.free();
            }
            obj1.free();
        }
    }
}

XWLinks::~XWLinks()
{
    if (links)
    {
        for (int i = 0; i < numLinks; ++i)
            delete links[i];
        free(links);
    }
}

void XWLinks::find(double l, double r, double t, double b, 
	               QList<XWLinkAction *> & list)
{
	if (!links)
		return ;
		
	for (int i = numLinks - 1; i >= 0; --i) 
    {
        if (links[i]->inRect(l, r, t, b)) 
            list << links[i]->getAction();
    }
}

XWLinkAction * XWLinks::find(double x, double y)
{
    for (int i = numLinks - 1; i >= 0; --i) 
    {
        if (links[i]->inRect(x, y)) 
            return links[i]->getAction();
    }
    return 0;
}

bool XWLinks::onLink(double x, double y)
{
    for (int i = 0; i < numLinks; ++i) 
    {
        if (links[i]->inRect(x, y))
            return true;
    }
    return false;
}

