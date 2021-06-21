/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWLINK_H
#define XWLINK_H

#include <QByteArray>
#include <QRect>
#include <QList>
#include "XWObject.h"

#define LINK_ACTION_GOTO    0
#define LINK_ACTION_GOTOR   1
#define LINK_ACTION_LAUNCH  2
#define LINK_ACTION_URI     3
#define LINK_ACTION_NAMED   4
#define LINK_ACTION_SOUND   5
#define LINK_ACTION_MOVIE   6
#define LINK_ACTION_UNKNOWN 7


#define LINK_DEST_XYZ   0
#define LINK_DEST_FIT   1
#define LINK_DEST_FITH  2
#define LINK_DEST_FITV  3
#define LINK_DEST_FITR  4
#define LINK_DEST_FITB  5
#define LINK_DEST_FITBH 6
#define LINK_DEST_FITBV 7

class XWString;
class XWArray;
class XWDict;

class XW_CORE_EXPORT XWLinkAction
{
public:   
    virtual ~XWLinkAction() {}
    
    static XWString * getFileSpecName(XWObject *fileSpecObj);
    
    virtual int  getKind() = 0;
    
    virtual bool isOk() = 0;
    
    static XWLinkAction * parseAction(XWObject *obj, XWString * baseURI = 0);
    static XWLinkAction * parseDest(XWObject *obj);        
};

class XW_CORE_EXPORT XWLinkDest 
{
public:
    XWLinkDest(XWArray *a);
    
    XWLinkDest *copy() { return new XWLinkDest(this); }

    double getBottom() { return bottom; }
    bool   getChangeLeft() { return changeLeft; }
    bool   getChangeTop() { return changeTop; }
    bool  getChangeZoom() { return changeZoom; }
    int    getKind() { return kind; }
    double getLeft() { return left; }
    int    getPageNum() { return pageNum; }
    ObjRef    getPageRef() { return pageRef; }
    double getRight() { return right; }
    double getTop() { return top; }
    double getZoom() { return zoom; }
    
    bool isOk() { return ok; }  
    bool isPageRef() { return pageIsRef; }
  
private:
    XWLinkDest(XWLinkDest *dest);
    
private:
    int kind;
    bool pageIsRef;
    union 
    {
        ObjRef pageRef;
        int pageNum;
    };
    
    double left, bottom;
    double right, top;
    double zoom;
    bool   changeLeft, changeTop;
    bool   changeZoom;
    bool   ok;
};


class XW_CORE_EXPORT XWLinkGoTo: public XWLinkAction 
{
public:
    XWLinkGoTo(XWObject *destObj);
    virtual ~XWLinkGoTo();

    XWLinkDest * getDest() { return dest; }
    virtual int  getKind() { return LINK_ACTION_GOTO; }
    XWString * getNamedDest() { return namedDest; }
    
    virtual bool isOk() { return dest || namedDest; }
private:
    XWLinkDest * dest;
    XWString * namedDest;
};

class XW_CORE_EXPORT XWLinkGoToR: public XWLinkAction 
{
public:
    XWLinkGoToR(XWObject *fileSpecObj, XWObject *destObj);
    virtual ~XWLinkGoToR();

    XWLinkDest * getDest() { return dest; }
    XWString * getFileName() { return fileName; }    
    virtual int  getKind() { return LINK_ACTION_GOTOR; }
    XWString * getNamedDest() { return namedDest; }
    
    virtual bool isOk() { return fileName && (dest || namedDest); }
    
private:
    XWString * fileName;
    XWLinkDest * dest;
    XWString * namedDest;	
};

class XW_CORE_EXPORT XWLinkLaunch: public XWLinkAction 
{
public:
    XWLinkLaunch(XWObject *actionObj);
    virtual ~XWLinkLaunch();
    
    XWString * getFileName() { return fileName; }
    virtual  int getKind() { return LINK_ACTION_LAUNCH; }
    XWString * getParams() { return params; }
    
    virtual bool isOk() { return fileName != NULL; }

private:
    XWString *fileName;		// file name
    XWString *params;		// parameters
};

class XW_CORE_EXPORT XWLinkURI: public XWLinkAction 
{
public:
    XWLinkURI(XWObject *uriObj, XWString *baseURI);
    virtual ~XWLinkURI();
    
    virtual int getKind() { return LINK_ACTION_URI; }
    XWString *getURI() { return uri; }
    
    virtual bool isOk() { return uri != NULL; }    

private:
    XWString *uri;
};


class XW_CORE_EXPORT XWLinkNamed: public XWLinkAction 
{
public:
    XWLinkNamed(XWObject *nameObj);
    virtual ~XWLinkNamed();

    virtual int  getKind() { return LINK_ACTION_NAMED; }
    XWString * getName() { return name; }
    
    virtual bool isOk() { return name != NULL; }

private:
    XWString *name;
};

class XW_CORE_EXPORT XWLinkSound: public XWLinkAction 
{
public:
    XWLinkSound(XWObject *annotObj);
    virtual ~XWLinkSound();
    
    ObjRef *getAnnotRef() { return &annotRef; }
    virtual int getKind() { return LINK_ACTION_SOUND; }
    
    bool hasAnnotRef() { return annotRef.num >= 0; }
    
    virtual bool isOk() { return annotRef.num >= 0; }
    
private:
    ObjRef annotRef;
};

class XW_CORE_EXPORT XWLinkMovie: public XWLinkAction 
{
public:
    XWLinkMovie(XWObject *annotObj, XWObject *titleObj);
    virtual ~XWLinkMovie();

    ObjRef *getAnnotRef() { return &annotRef; }
    virtual int getKind() { return LINK_ACTION_MOVIE; }
    XWString *getTitle() { return title; }
    
    bool hasAnnotRef() { return annotRef.num >= 0; }
    
    virtual bool isOk() { return annotRef.num >= 0 || title != NULL; }
    
private:
    ObjRef annotRef;
    XWString *title;
};

class XW_CORE_EXPORT XWLinkUnknown: public XWLinkAction 
{
public:
    XWLinkUnknown(char *actionA);
    virtual ~XWLinkUnknown();
    
    XWString *getAction() { return action; }
    virtual int getKind() { return LINK_ACTION_UNKNOWN; }    
    
    virtual bool isOk() { return action != NULL; }   

private:
    XWString *action;
};


class XW_CORE_EXPORT XWLink 
{
public:
    XWLink(XWDict *dict, XWString *baseURI);
    ~XWLink();
    
    XWLinkAction *getAction() { return action; }
    void getRect(double *xa1, double *ya1, double *xa2, double *ya2)
            { *xa1 = x1; *ya1 = y1; *xa2 = x2; *ya2 = y2; }
            
    bool inRect(double l, double r, double t, double b)
            {return x1 >= l && x2 <= r && y1 <= t && y2 >= b;}
    bool inRect(double x, double y)
            { return x1 <= x && x <= x2 && y1 <= y && y <= y2; }
    bool isOk() { return ok; }

private:
    double x1, y1;
    double x2, y2;
    XWLinkAction *action;
    bool ok;
};


class XW_CORE_EXPORT XWLinks 
{
public:
    XWLinks(XWObject *annots, XWString *baseURI);
    ~XWLinks();
	
	void find(double l, double r, double t, double b, 
	          QList<XWLinkAction *> & list);
    XWLinkAction *find(double x, double y);
    
    XWLink * getLink(int i) { return links[i]; }
    int      getNumLinks() { return numLinks; }
    
    bool onLink(double x, double y);

private:
    XWLink **links;
    int numLinks;
};


#endif // XWLINK_H

