/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <stddef.h>
#include <limits.h>
#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include "XWApplication.h"
#include "XWList.h"
#include "XWObject.h"
#include "XWDoc.h"
#include "XWRef.h"
#include "XWArray.h"
#include "XWDict.h"
#include "XWPage.h"
#include "XWLink.h"
#include "XWDocEncoding.h"
#include "XWCatalog.h"


class PageTreeNode 
{
public:

  PageTreeNode(ObjRef refA, int countA, PageTreeNode *parentA);
  ~PageTreeNode();

  ObjRef ref;
  int count;
  PageTreeNode *parent;
  XWList *kids;			// [PageTreeNode]
  XWPageAttrs *attrs;
};

PageTreeNode::PageTreeNode(ObjRef refA, int countA, PageTreeNode *parentA) 
{
  ref = refA;
  count = countA;
  parent = parentA;
  kids = NULL;
  attrs = NULL;
}

PageTreeNode::~PageTreeNode() 
{
	if (attrs)
  	delete attrs;
  if (kids) 
  {
    deleteXWList(kids, PageTreeNode);
  }
}


class EmbeddedFile 
{
public:

  EmbeddedFile(int *nameA, int nameLenA, XWObject *streamRefA);
  ~EmbeddedFile();

  int *name;
  int nameLen;
  XWObject streamRef;
};

EmbeddedFile::EmbeddedFile(int *nameA, int nameLenA,XWObject *streamRefA) 
{
  name = nameA;
  nameLen = nameLenA;
  streamRefA->copy(&streamRef);
}

EmbeddedFile::~EmbeddedFile() 
{
	if (name)
  	free(name);
  streamRef.free();
}

XWCatalog::XWCatalog(XWDoc *docA)
{
  XWObject catDict;
  XWObject obj, obj2;

  ok = true;
  doc = docA;
  xref = doc->getXRef();
  pageTree = NULL;
  pages = NULL;
  pageRefs = NULL;
  numPages = 0;
  baseURI = NULL;
  embeddedFiles = NULL;

  xref->getCatalog(&catDict);
  if (!catDict.isDict()) 
  {
  	QString msg = QString("Catalog object is wrong type ({0:%1})").arg(catDict.getTypeName());
    xwApp->error(msg);
    goto err1;
  }

  // read page tree
  if (!readPageTree(&catDict)) 
  {
    goto err1;
  }

  // read named destination dictionary
  catDict.dictLookup("Dests", &dests);

  // read root of named destination tree
  if (catDict.dictLookup("Names", &obj)->isDict())
    obj.dictLookup("Dests", &nameTree);
  else
    nameTree.initNull();
  obj.free();

  // read base URI
  if (catDict.dictLookup("URI", &obj)->isDict()) 
  {
    if (obj.dictLookup("Base", &obj2)->isString()) 
    {
      baseURI = obj2.getString()->copy();
    }
    obj2.free();
  }
  obj.free();
  if (!baseURI || baseURI->getLength() == 0) 
  {
    if (baseURI) 
    {
      delete baseURI;
    }
    if (doc->getFileName()) 
    {
    	QString tmp = QFile::decodeName(doc->getFileName()->getCString());
    	QFileInfo info(tmp);
    	QString dir = info.absolutePath();
    	QByteArray ba = QFile::encodeName(dir);
      baseURI = new XWString(ba.data());
      if (baseURI->getChar(0) == '/') 
      {
				baseURI->insert(0, "file://localhost");
      } 
      else 
      {
				baseURI->insert(0, "file://localhost/");
      }
    } 
    else 
    {
      baseURI = new XWString("file://localhost/");
    }
  }

  // get the metadata stream
  catDict.dictLookup("Metadata", &metadata);

  // get the structure tree root
  catDict.dictLookup("StructTreeRoot", &structTreeRoot);

  // get the outline dictionary
  catDict.dictLookup("Outlines", &outline);

  // get the AcroForm dictionary
  catDict.dictLookup("AcroForm", &acroForm);

  // get the OCProperties dictionary
  catDict.dictLookup("OCProperties", &ocProperties);

  // get the list of embedded files
  readEmbeddedFileList(catDict.getDict());

  catDict.free();
  return;

 err1:
  catDict.free();
  dests.initNull();
  nameTree.initNull();
  ok = false;
}

XWCatalog::~XWCatalog()
{
    if (pageTree) 
    {
    	delete pageTree;
  	}
  if (pages) 
  {
    for (int i = 0; i < numPages; ++i) 
   	{
      if (pages[i]) 
      {
				delete pages[i];
      }
    }
    free(pages);
    free(pageRefs);
  }
  dests.free();
  nameTree.free();
  if (baseURI) 
  {
    delete baseURI;
  }
  
  metadata.free();
  structTreeRoot.free();
  outline.free();
  acroForm.free();
  ocProperties.free();
  if (embeddedFiles) 
  {
    deleteXWList(embeddedFiles, EmbeddedFile);
  }
}

void XWCatalog::doneWithPage(int i)
{
	if (pages[i-1]) 
	{
    delete pages[i-1];
    pages[i-1] = NULL;
  }
}

XWLinkDest * XWCatalog::findDest(XWString *name)
{
  XWLinkDest *dest;
  XWObject obj1, obj2;
  bool found;

  // try named destination dictionary then name tree
  found = false;
  if (dests.isDict()) {
    if (!dests.dictLookup(name->getCString(), &obj1)->isNull())
      found = true;
    else
      obj1.free();
  }
  if (!found && nameTree.isDict()) {
    if (!findDestInTree(&nameTree, name, &obj1)->isNull())
      found = true;
    else
      obj1.free();
  }
  if (!found)
    return NULL;

  // construct LinkDest
  dest = NULL;
  if (obj1.isArray()) {
    dest = new XWLinkDest(obj1.getArray());
  } else if (obj1.isDict()) {
    if (obj1.dictLookup("D", &obj2)->isArray())
      dest = new XWLinkDest(obj2.getArray());
    else
      xwApp->error("Bad named destination value");
    obj2.free();
  } else {
    xwApp->error("Bad named destination value");
  }
  obj1.free();
  if (dest && !dest->isOk()) {
    delete dest;
    dest = NULL;
  }

  return dest;
}

int XWCatalog::findPage(int num, int gen)
{
    for (int i = 0; i < numPages; ++i) 
    {
    	if (!pages[i]) 
    	{
      	loadPage(i+1);
    	}
    	
    	if (pageRefs[i].num == num && pageRefs[i].gen == gen)
      	return i + 1;
  	}
  	return 0;
}

int * XWCatalog::getEmbeddedFileName(int idx)
{
	return ((EmbeddedFile *)embeddedFiles->get(idx))->name;
}

int XWCatalog::getEmbeddedFileNameLength(int idx)
{
	return ((EmbeddedFile *)embeddedFiles->get(idx))->nameLen;
}

XWObject * XWCatalog::getEmbeddedFileStreamObj(int idx, XWObject *strObj)
{
	((EmbeddedFile *)embeddedFiles->get(idx))->streamRef.fetch(xref, strObj);
  if (!strObj->isStream()) 
  {
    strObj->free();
    return NULL;
  }
  return strObj;
}

int XWCatalog::getNumEmbeddedFiles()
{
	return embeddedFiles ? embeddedFiles->getLength() : 0;
}

XWPage * XWCatalog::getPage(int i)
{
	if (!pages[i-1]) 
	{
    loadPage(i);
  }
  
  return pages[i-1];
}

ObjRef * XWCatalog::getPageRef(int i)
{
	if (!pages[i-1]) 
	{
    loadPage(i);
  }
  return &pageRefs[i-1];
}

XWString * XWCatalog::readMetadata()
{
  XWString *s;
  XWDict *dict;
  XWObject obj;
  int c;

  if (!metadata.isStream()) 
  {
    return NULL;
  }
  dict = metadata.streamGetDict();
  if (!dict->lookup("Subtype", &obj)->isName("XML")) 
  {
  	QString msg = QString("Unknown Metadata type: '{0:%1}'").arg(obj.isName() ? obj.getName() : "???");
    xwApp->error(msg);
  }
  obj.free();
  s = new XWString();
  metadata.streamReset();
  while ((c = metadata.streamGetChar()) != EOF) 
 	{
    s->append(c);
  }
  metadata.streamClose();
  return s;
}

int XWCatalog::countPageTree(XWObject *pagesObj)
{
	XWObject kids, kid;
  int n, n2, i;

  if (!pagesObj->isDict()) 
  {
    return 0;
  }
  if (pagesObj->dictLookup("Kids", &kids)->isArray()) 
  {
    n = 0;
    for (i = 0; i < kids.arrayGetLength(); ++i) 
    {
      kids.arrayGet(i, &kid);
      n2 = countPageTree(&kid);
      if (n2 < INT_MAX - n) 
      {
				n += n2;
      } 
      else 
      {
				xwApp->error("Page tree contains too many pages");
				n = INT_MAX;
      }
      kid.free();
    }
  } 
  else 
  {
    n = 1;
  }
  kids.free();
  return n;
}

XWObject * XWCatalog::findDestInTree(XWObject *tree, XWString *name, XWObject *obj)
{
  XWObject names, name1;
  XWObject kids, kid, limits, low, high;
  bool done, found;
  int cmp, i;

  // leaf node
  if (tree->dictLookup("Names", &names)->isArray()) {
    done = found = false;
    for (i = 0; !done && i < names.arrayGetLength(); i += 2) {
      if (names.arrayGet(i, &name1)->isString()) {
	cmp = name->cmp(name1.getString());
	if (cmp == 0) {
	  names.arrayGet(i+1, obj);
	  found = true;
	  done = true;
	} else if (cmp < 0) {
	  done = true;
	}
      }
      name1.free();
    }
    names.free();
    if (!found)
      obj->initNull();
    return obj;
  }
  names.free();

  // root or intermediate node
  done = false;
  if (tree->dictLookup("Kids", &kids)->isArray()) {
    for (i = 0; !done && i < kids.arrayGetLength(); ++i) {
      if (kids.arrayGet(i, &kid)->isDict()) {
	if (kid.dictLookup("Limits", &limits)->isArray()) {
	  if (limits.arrayGet(0, &low)->isString() &&
	      name->cmp(low.getString()) >= 0) {
	    if (limits.arrayGet(1, &high)->isString() &&
		name->cmp(high.getString()) <= 0) {
	      findDestInTree(&kid, name, obj);
	      done = true;
	    }
	    high.free();
	  }
	  low.free();
	}
	limits.free();
      }
      kid.free();
    }
  }
  kids.free();

  // name was outside of ranges of all kids
  if (!done)
    obj->initNull();

  return obj;
}

void XWCatalog::loadPage(int pg)
{
	loadPage2(pg, pg - 1, pageTree);
}

void XWCatalog::loadPage2(int pg, int relPg, PageTreeNode *node)
{
	XWObject pageRefObj, pageObj, kidsObj, kidRefObj, kidObj, countObj;
  PageTreeNode *kidNode, *p;
  XWPageAttrs *attrs;
  int count, i;

  if (relPg >= node->count) 
  {
    xwApp->error("Internal error in page tree");
    pages[pg-1] = new XWPage(doc, pg);
    return;
  }

  // if this node has not been filled in yet, it's either a leaf node
  // or an unread internal node
  if (!node->kids) 
  {

    // check for a loop in the page tree
    for (p = node->parent; p; p = p->parent) 
    {
      if (node->ref.num == p->ref.num && node->ref.gen == p->ref.gen) 
      {
				xwApp->error("Loop in Pages tree");
				pages[pg-1] = new XWPage(doc, pg);
				return;
      }
    }

    // fetch the Page/Pages object
    pageRefObj.initRef(node->ref.num, node->ref.gen);
    if (!pageRefObj.fetch(xref, &pageObj)->isDict()) 
    {
    	QString msg = QString("Page tree object is wrong type ({0:%1})").arg(pageObj.getTypeName());
      xwApp->error(msg);
      pageObj.free();
      pageRefObj.free();
      pages[pg-1] = new XWPage(doc, pg);
      return;
    }

    // merge the PageAttrs
    attrs = new XWPageAttrs(node->parent ? node->parent->attrs : (XWPageAttrs *)NULL, pageObj.getDict());

    // if "Kids" exists, it's an internal node
    if (pageObj.dictLookup("Kids", &kidsObj)->isArray()) 
    {

      // save the PageAttrs
      node->attrs = attrs;

      // read the kids
      node->kids = new XWList();
      for (i = 0; i < kidsObj.arrayGetLength(); ++i) 
      {
				if (kidsObj.arrayGetNF(i, &kidRefObj)->isRef()) 
				{
	  			if (kidRefObj.fetch(xref, &kidObj)->isDict()) 
	  			{
	    			if (kidObj.dictLookup("Count", &countObj)->isInt()) 
	    			{
	      			count = countObj.getInt();
	    			} 
	    			else 
	    			{
	      			count = 1;
	    			}
	    			countObj.free();
	    			node->kids->append(new PageTreeNode(kidRefObj.getRef(), count, node));
	  			} 
	  			else 
	  			{
	  				QString msg = QString("Page tree object is wrong type ({0:%1})").arg(kidObj.getTypeName());
	    			xwApp->error(msg);
	  			}
	  			kidObj.free();
				} 
				else 
				{
					QString msg = QString("Page tree reference is wrong type ({0:%1})").arg(kidRefObj.getTypeName());
	  			xwApp->error(msg);
				}
				kidRefObj.free();
      }

    } 
    else 
    {
      
      // create the Page object
      pageRefs[pg-1] = node->ref;
      pages[pg-1] = new XWPage(doc, pg, pageObj.getDict(), attrs);
      if (!pages[pg-1]->isOk()) 
      {
				delete pages[pg-1];
				pages[pg-1] = new XWPage(doc, pg);
      }

    }

    kidsObj.free();
    pageObj.free();
    pageRefObj.free();
  }

  // recursively descend the tree
  if (node->kids) 
  {
    for (i = 0; i < node->kids->getLength(); ++i) 
   	{
      kidNode = (PageTreeNode *)node->kids->get(i);
      if (relPg < kidNode->count) 
      {
      
				loadPage2(pg, relPg, kidNode);
				break;
      }
      relPg -= kidNode->count;
    }

    // this will only happen if the page tree is invalid
    // (i.e., parent count > sum of children counts)
    if (i == node->kids->getLength()) 
    {
      xwApp->error("Invalid page count in page tree");
      pages[pg-1] = new XWPage(doc, pg);
    }
  }
}

void XWCatalog::readEmbeddedFile(XWObject *fileSpec, XWObject *name1)
{
	XWObject name2, efObj, streamObj;
  XWString *s;
  int *name;
  int nameLen, i;

  if (fileSpec->isDict()) 
  {
    if (fileSpec->dictLookup("UF", &name2)->isString()) 
    {
      s = name2.getString();
    } 
    else 
    {
      name2.free();
      if (fileSpec->dictLookup("F", &name2)->isString()) 
      {
					s = name2.getString();
      } 
      else if (name1 && name1->isString()) 
      {
				s = name1->getString();
      } 
      else 
      {
				s = NULL;
      }
    }
    if (s) 
    {
      if ((s->getChar(0) & 0xff) == 0xfe && (s->getChar(1) & 0xff) == 0xff) 
      {
				nameLen = (s->getLength() - 2) / 2;
				name = (int *)malloc(nameLen * sizeof(int));
				for (i = 0; i < nameLen; ++i) 
				{
	  				name[i] = ((s->getChar(2 + 2*i) & 0xff) << 8) | (s->getChar(3 + 2*i) & 0xff);
				}
      } 
      else 
      {
				nameLen = s->getLength();
				name = (int *)malloc(nameLen * sizeof(int));
				for (i = 0; i < nameLen; ++i) 
				{
	  			name[i] = pdfDocEncoding[s->getChar(i) & 0xff];
				}
      }
    } 
    else 
    {
      nameLen = 1;
      name = (int *)malloc(nameLen * sizeof(int));
      name[0] = '?';
    }
    name2.free();
    if (fileSpec->dictLookup("EF", &efObj)->isDict()) 
    {
      if (efObj.dictLookupNF("F", &streamObj)->isRef()) 
      {
				if (!embeddedFiles) 
				{
	  			embeddedFiles = new XWList();
				}
				embeddedFiles->append(new EmbeddedFile(name, nameLen, &streamObj));
      } 
      else 
      {
				free(name);
      }
      streamObj.free();
    } 
    else 
    {
      free(name);
    }
    efObj.free();
  }
}

void XWCatalog::readEmbeddedFileList(XWDict *catDict)
{
	XWObject obj1, obj2;
  char *touchedObjs;

  // read the embedded file name tree
  if (catDict->lookup("Names", &obj1)->isDict()) 
  {
    if (obj1.dictLookup("EmbeddedFiles", &obj2)->isDict()) 
    {
      readEmbeddedFileTree(&obj2);
    }
    obj2.free();
  }
  obj1.free();

  // look for file attachment annotations
  touchedObjs = (char *)malloc(xref->getNumObjects() * sizeof(char));
  memset(touchedObjs, 0, xref->getNumObjects() * sizeof(char));
  readFileAttachmentAnnots(catDict->lookupNF("Pages", &obj1), touchedObjs);
  obj1.free();
  free(touchedObjs);
}

void XWCatalog::readEmbeddedFileTree(XWObject *node)
{
	XWObject kidsObj, kidObj;
  XWObject namesObj, nameObj, fileSpecObj;
  int i;

  if (node->dictLookup("Kids", &kidsObj)->isArray()) 
  {
    for (i = 0; i < kidsObj.arrayGetLength(); ++i) 
    {
      if (kidsObj.arrayGet(i, &kidObj)->isDict()) 
      {
				readEmbeddedFileTree(&kidObj);
      }
      kidObj.free();
    }
  } 
  else 
  {
    if (node->dictLookup("Names", &namesObj)->isArray()) 
    {
      for (i = 0; i+1 < namesObj.arrayGetLength(); ++i) 
      {
				namesObj.arrayGet(i, &nameObj);
				namesObj.arrayGet(i+1, &fileSpecObj);
				readEmbeddedFile(&fileSpecObj, &nameObj);
				nameObj.free();
				fileSpecObj.free();
      }
    }
    namesObj.free();
  }
  kidsObj.free();
}

void XWCatalog::readFileAttachmentAnnots(XWObject *pageNodeRef, char *touchedObjs)
{
	XWObject pageNode, kids, kid, annots, annot, subtype, fileSpec, contents;
  int i;

  // check for a page tree loop
  if (pageNodeRef->isRef()) 
  {
    if (touchedObjs[pageNodeRef->getRefNum()]) 
    {
      return;
    }
    touchedObjs[pageNodeRef->getRefNum()] = 1;
    xref->fetch(pageNodeRef->getRefNum(), pageNodeRef->getRefGen(), &pageNode, 0);
  } 
  else 
  {
    pageNodeRef->copy(&pageNode);
  }

  if (pageNode.isDict()) 
  {
    if (pageNode.dictLookup("Kids", &kids)->isArray()) 
    {
      for (i = 0; i < kids.arrayGetLength(); ++i) 
      {
				readFileAttachmentAnnots(kids.arrayGetNF(i, &kid), touchedObjs);
				kid.free();
      }
    } 
    else 
    {
      if (pageNode.dictLookup("Annots", &annots)->isArray()) 
      {
				for (i = 0; i < annots.arrayGetLength(); ++i) 
				{
	  			if (annots.arrayGet(i, &annot)->isDict()) 
	  			{
	    			if (annot.dictLookup("Subtype", &subtype)->isName("FileAttachment")) 
	    			{
	      			if (annot.dictLookup("FS", &fileSpec)) 
	      			{
								readEmbeddedFile(&fileSpec, annot.dictLookup("Contents", &contents));
								contents.free();
	      			}
	      			fileSpec.free();
	    			}
	    			subtype.free();
	  			}
	  			annot.free();
				}
      }
      annots.free();
    }
    kids.free();
  }

  pageNode.free();
}

bool XWCatalog::readPageTree(XWObject *catDict)
{
  XWObject topPagesRef, topPagesObj, countObj;
  int i;

  if (!catDict->dictLookupNF("Pages", &topPagesRef)->isRef()) 
  {
  	QString msg = QString("Top-level pages reference is wrong type ({0:%1})").arg(topPagesRef.getTypeName());
    xwApp->error(msg);
    topPagesRef.free();
    return false;
  }
  if (!topPagesRef.fetch(xref, &topPagesObj)->isDict()) 
  {
  	QString msg = QString("Top-level pages object is wrong type ({0:%1})").arg(topPagesObj.getTypeName());
    xwApp->error(msg);
    topPagesObj.free();
    topPagesRef.free();
    return false;
  }
  if (topPagesObj.dictLookup("Count", &countObj)->isInt()) 
  {
    numPages = countObj.getInt();
    if (numPages == 0) 
    {
      // Acrobat apparently scans the page tree if it sees a zero count
      numPages = countPageTree(&topPagesObj);
    }
  } 
  else 
  {
    // assume we got a Page node instead of a Pages node
    numPages = 1;
  }
  countObj.free();
  if (numPages < 0) 
  {
    xwApp->error("Invalid page count");
    topPagesObj.free();
    topPagesRef.free();
    numPages = 0;
    return false;
  }
  pageTree = new PageTreeNode(topPagesRef.getRef(), numPages, NULL);
  topPagesObj.free();
  topPagesRef.free();
  pages = (XWPage **)realloc(pages, numPages * sizeof(XWPage *));
  pageRefs = (ObjRef *)realloc(pageRefs, numPages * sizeof(ObjRef));
  for (i = 0; i < numPages; ++i) 
  {
    pages[i] = NULL;
    pageRefs[i].num = -1;
    pageRefs[i].gen = -1;
  }
  return true;
}

