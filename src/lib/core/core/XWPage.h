/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPAGE_H
#define XWPAGE_H

#include "XWObject.h"

class XWDoc;
class XWDict;
class XWRef;
class XWOutputDev;
class XWLinks;
class XWCatalog;

class XW_CORE_EXPORT XWPDFRectangle
{
public:
    XWPDFRectangle() {x1 = y1 = x2 = y2 = 0;}
    XWPDFRectangle(double x1A, double y1A, double x2A, double y2A)
        { x1 = x1A; y1 = y1A; x2 = x2A; y2 = y2A; }
        
    void clipTo(XWPDFRectangle *rect);
        
    bool isValid() { return x1 != 0 || y1 != 0 || x2 != 0 || y2 != 0; }
    
public:
    double x1, y1, x2, y2;
};

class XW_CORE_EXPORT XWPageAttrs
{
public:
	XWPageAttrs();
    XWPageAttrs(XWPageAttrs *attrs, XWDict *dict);
    ~XWPageAttrs();
    
    void clipBoxes();
    
    XWPDFRectangle * getArtBox() { return &artBox; }
    XWPDFRectangle * getBleedBox() { return &bleedBox; }
    XWDict         * getBoxColorInfo()
        { return boxColorInfo.isDict() ? boxColorInfo.getDict() : (XWDict *)0; }
    XWPDFRectangle * getCropBox() { return &cropBox; }
    XWDict         * getGroup()
        { return group.isDict() ? group.getDict() : (XWDict *)0; }
    XWString       * getLastModified()
        { return lastModified.isString() ? lastModified.getString() : (XWString *)0; }
    XWPDFRectangle * getMediaBox() { return &mediaBox; }
    XWStream       * getMetadata()
        { return metadata.isStream() ? metadata.getStream() : (XWStream *)0; }
    XWDict         * getPieceInfo()
        { return pieceInfo.isDict() ? pieceInfo.getDict() : (XWDict *)0; }
    XWDict         * getResourceDict()
        { return resources.isDict() ? resources.getDict() : (XWDict *)0; }
    int getRotate() { return rotate; }
    XWDict         * getSeparationInfo()
        { return separationInfo.isDict() ? separationInfo.getDict() : (XWDict *)0; }
    XWPDFRectangle * getTrimBox() { return &trimBox; }
    
    bool isCropped() { return haveCropBox; }
    
private:
    bool readBox(XWDict *dict, char *key, XWPDFRectangle *box);
    
private:
    XWPDFRectangle mediaBox;
    XWPDFRectangle cropBox;
    bool haveCropBox;
    XWPDFRectangle bleedBox;
    XWPDFRectangle trimBox;
    XWPDFRectangle artBox;
    int rotate;
    XWObject lastModified;
    XWObject boxColorInfo;
    XWObject group;
    XWObject metadata;
    XWObject pieceInfo;
    XWObject separationInfo;
    XWObject resources;
};

class XW_CORE_EXPORT XWPage
{
public:
    XWPage(XWDoc *docA, int numA, XWDict *pageDict, XWPageAttrs *attrsA);
    XWPage(XWDoc *docA, int numA);
    ~XWPage();
    
    void display(XWOutputDev *out, 
                 double hDPI, 
                 double vDPI,
	             int rotate, 
	             bool useMediaBox, 
	             bool crop,
	             bool printing, 
	             bool (*abortCheckCbk)(void *data) = 0,
	             void *abortCheckCbkData = 0);
	             
	void displaySlice(XWOutputDev *out, 
	                  double hDPI, 
	                  double vDPI,
		              int rotate, 
		              bool useMediaBox, 
		              bool crop,
		              int sliceX, 
		              int sliceY, 
		              int sliceW, 
		              int sliceH,
		              bool printing, 
		              bool (*abortCheckCbk)(void *data) = 0,
		              void *abortCheckCbkData = 0);
        
    XWObject *getAnnots(XWObject *obj) { return annots.fetch(xref, obj); }
    XWPDFRectangle * getArtBox() { return attrs->getArtBox(); }
    XWPDFRectangle * getBleedBox() { return attrs->getBleedBox(); }
    XWDict * getBoxColorInfo() { return attrs->getBoxColorInfo(); }
    XWObject *getContents(XWObject *obj) { return contents.fetch(xref, obj); }
    XWPDFRectangle * getCropBox() { return attrs->getCropBox(); }
    double getCropHeight()
        { return attrs->getCropBox()->y2 - attrs->getCropBox()->y1; }
    double getCropWidth() 
        { return attrs->getCropBox()->x2 - attrs->getCropBox()->x1; }
    void getDefaultCTM(double *ctm, 
                       double hDPI, 
                       double vDPI,
		               int rotate, 
		               bool useMediaBox, 
		               bool upsideDown);
    XWDict * getGroup() { return attrs->getGroup(); }
    XWString * getLastModified() { return attrs->getLastModified(); }
    XWLinks  * getLinks(XWCatalog *catalog);
    XWPDFRectangle * getMediaBox() { return attrs->getMediaBox(); }    
    double getMediaHeight()
        { return attrs->getMediaBox()->y2 - attrs->getMediaBox()->y1; }
    double getMediaWidth() 
        { return attrs->getMediaBox()->x2 - attrs->getMediaBox()->x1; }
    XWStream * getMetadata() { return attrs->getMetadata(); }
    int getNum() { return num; }
    XWDict *getPieceInfo() { return attrs->getPieceInfo(); }
    XWDict *getResourceDict() { return attrs->getResourceDict(); }
    int getRotate() { return attrs->getRotate(); }
    XWDict *getSeparationInfo() { return attrs->getSeparationInfo(); }
    XWPDFRectangle * getTrimBox() { return attrs->getTrimBox(); }
    
    bool isCropped() { return attrs->isCropped(); }
    bool isOk() { return ok; }
    
    void makeBox(double hDPI, 
                 double vDPI, 
                 int rotate,
	             bool useMediaBox, 
	             bool upsideDown,
	             double sliceX, 
	             double sliceY, 
	             double sliceW, 
	             double sliceH,
	             XWPDFRectangle *box, 
	             bool *crop);
    
    void processLinks(XWOutputDev *out, XWCatalog *catalog);
    
private:
	XWDoc *doc;
    XWRef *xref;
    int num;
    XWPageAttrs *attrs;
    XWObject annots;
    XWObject contents;
    bool ok;
};

#endif // XWPAGE_H

