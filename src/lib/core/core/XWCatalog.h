/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWCATALOG_H
#define XWCATALOG_H

#include <QObject>

#include "XWGlobal.h"

class XWList;
class XWDoc;
class XWRef;
class XWObject;
class XWPage;
class XWPageAttrs;
struct ObjRef;
class XWLinkDest;
class PageTreeNode;


class XW_CORE_EXPORT XWCatalog
{
public:   
    XWCatalog(XWDoc *docA);
    virtual ~XWCatalog();
    
    virtual void doneWithPage(int i);
    
    virtual XWLinkDest * findDest(XWString *name);
    virtual int          findPage(int num, int gen);
    
    virtual XWObject * getAcroForm() { return &acroForm; }
    virtual XWString * getBaseURI() { return baseURI; }
    virtual XWObject * getDests() { return &dests; }
    virtual int * getEmbeddedFileName(int idx);
    virtual int   getEmbeddedFileNameLength(int idx);
    virtual XWObject * getEmbeddedFileStreamObj(int idx, XWObject *strObj);
    virtual XWObject * getNameTree() { return &nameTree; }
    virtual int   getNumEmbeddedFiles();    
    virtual int        getNumPages() { return numPages; }
    virtual XWObject * getOCProperties() { return &ocProperties; }
    virtual XWObject * getOutline() { return &outline; }
    virtual XWPage   * getPage(int i);
    virtual ObjRef   * getPageRef(int i);
    virtual XWObject * getStructTreeRoot() { return &structTreeRoot; }
    
    virtual bool isOk() { return ok; }
    
    virtual XWString *readMetadata();
    
private:
	int countPageTree(XWObject *pagesObj);
	
	XWObject * findDestInTree(XWObject *tree, 
	                          XWString *name, 
	                          XWObject *obj);
	
	void loadPage(int pg);
	void loadPage2(int pg, int relPg, PageTreeNode *node);
	
	void readEmbeddedFile(XWObject *fileSpec, XWObject *name1);
	void readEmbeddedFileList(XWDict *catDict);
	void readEmbeddedFileTree(XWObject *node);
	void readFileAttachmentAnnots(XWObject *pageNodeRef, char *touchedObjs);
	bool readPageTree(XWObject *catDict);
    
protected:
	XWDoc * doc;
	XWRef *xref;
	bool ok;
	XWPage **pages;
	PageTreeNode *pageTree;
	ObjRef *pageRefs;
	int numPages;
	int pagesSize;
	XWString *baseURI;
	XWObject dests;
	XWObject nameTree;
	XWObject metadata;	
	XWObject structTreeRoot;
	XWObject outline;	
	XWObject acroForm;
	XWObject ocProperties;
	XWList *embeddedFiles;
};

#endif // XWCATALOG_H

