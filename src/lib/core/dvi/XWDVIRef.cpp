/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <QtDebug>
#include <QTextCodec>
#include <zlib.h>
#include  <QtDebug>

#include "XWNumberUtil.h"
#include "XWLexer.h"
#include "XWApplication.h"
#include "XWFileName.h"
#include "XWTexFontMap.h"
#include "XWTFMFile.h"
#include "XWPictureSea.h"
#include "XWPDFLexer.h"
#include "XWPDFParser.h"
#include "XWDVICore.h"
#include "XWDVIDev.h"
#include "XWDVIRef.h"
#include "XWPDFFile.h"
#include "XWDVIEncrypt.h"
#include "XWPDFNames.h"
#include "XWDVIImage.h"
#include "XWPNGImage.h"
#include "XWJPGImage.h"


#define IND_OBJECTS_ALLOC_SIZE 512
#define OBJSTM_MAX_OBJS  200

#define USE_MY_MEDIABOX (1 << 0)

#define PDFDOC_PAGES_ALLOC_SIZE   128u
#define PDFDOC_ARTICLE_ALLOC_SIZE 16
#define PDFDOC_BEAD_ALLOC_SIZE    16

#define PAGE_CLUSTER 4

static const char *name_dict_categories[] = {
  "Dests", "AP", "JavaScript", "Pages",
  "Templates", "IDS", "URLS", "EmbeddedFiles",
  "AlternatePresentations", "Renditions"
};
#define NUM_NAME_CATEGORY (sizeof(name_dict_categories)/sizeof(name_dict_categories[0]))

#define LASTPAGE  (&(pages.entries[pages.num_entries]))
#define FIRSTPAGE (&(pages.entries[0]))
#define PAGECOUNT (pages.num_entries)
#define MAXPAGES  (pages.max_entries)


XWDVIRef::XWDVIRef()
{
	outputFile = new QBuffer;
	deleteFile = true;
	init();
}

XWDVIRef::XWDVIRef(QIODevice * outfileA)
{
	outputFile = outfileA;
	deleteFile = false;
	init();
}

XWDVIRef::~XWDVIRef()
{
	if (outputFile->isOpen())
		outputFile->close();
		
	if (deleteFile)
		delete outputFile;
	
	if (thumbBaseName)
		delete [] thumbBaseName;
		
	clear();
}

void XWDVIRef::addAnnot(XWDVIDev * dev,
		                unsigned page_no, 
		                const PDFRect *rect,
		   				XWObject *annot_dict, 
		   				int new_annot)
{
	if (!isScanning())
		return ;
		
	PDFPage * page = getPageEntry(page_no);
  	if (page->annots.isNull())
    	page->annots.initArray(this);
    	
    XWObject obj;
    	
    PDFRect  mediabox;
    getMediaBox(page_no, &mediabox);
    double    xpos, ypos;
    dev->getCoord(&xpos, &ypos);
    
    PDFRect  annbox;
    annbox.llx = rect->llx - xpos; 
    annbox.lly = rect->lly - ypos;
  	annbox.urx = rect->urx - xpos; 
  	annbox.ury = rect->ury - ypos;
  	
  	if (annbox.llx < mediabox.llx || annbox.urx > mediabox.urx ||
      	annbox.lly < mediabox.lly || annbox.ury > mediabox.ury) 
    {
    	xwApp->warning("Annotation out of page boundary.\n");
    	QString msg = QString("current page's MediaBox: [%1 %2 %3 %4]\n")
    	                .arg(mediabox.llx).arg(mediabox.lly).arg(mediabox.urx).arg(mediabox.ury);
    	xwApp->warning(msg);
    	msg = QString("Annotation: [%1 %2 %3 %4]\n")
    	               .arg(annbox.llx).arg(annbox.lly).arg(annbox.urx).arg(annbox.ury);
    	xwApp->warning(msg);
    	xwApp->warning("maybe incorrect paper size specified.\n");
  	}
  	
  	if (annbox.llx > annbox.urx || annbox.lly > annbox.ury) 
  	{
  		QString msg = QString("Rectangle with negative width/height: [%1 %2 %3 %4]\n")
    	               .arg(annbox.llx).arg(annbox.lly).arg(annbox.urx).arg(annbox.ury);
    	xwApp->warning(msg);
  	}

	double    annot_grow = opt.annot_grow;
  	XWObject rect_array;
  	rect_array.initArray(this);
  	obj.initInt(round(annbox.llx - annot_grow, 0.001));
  	rect_array.arrayAdd(&obj);
  	obj.initInt(round(annbox.lly - annot_grow, 0.001));
  	rect_array.arrayAdd(&obj);  	
  	obj.initInt(round(annbox.urx + annot_grow, 0.001));
  	rect_array.arrayAdd(&obj);
  	obj.initInt(round(annbox.ury + annot_grow, 0.001));
  	rect_array.arrayAdd(&obj);
  	annot_dict->dictAdd(qstrdup("Rect"), &rect_array);
  	
  	refObj(annot_dict, &obj);
  	page->annots.arrayAdd(&obj);
  	if (new_annot)
    	addGoto(annot_dict);
}

void XWDVIRef::addBead(const char *article_id,
                       const char *bead_id, 
                       long page_no, 
                       const PDFRect *rect)
{
	if (!article_id) 
	{
    	xwApp->error("no article identifier specified.\n");
    	return ;
  	}
  	
  	PDFArticle * article = 0;
  	for (int i = 0; i < articles.num_entries; i++) 
  	{
    	if (!strcmp(articles.entries[i].id, article_id)) 
    	{
      		article = &(articles.entries[i]);
      		break;
    	}
  	}
  	
  	if (!article) 
  	{
    	xwApp->error("specified article thread that doesn't exist.\n");
    	return;
  	}

  	PDFBead * bead = bead_id ? findBead(article, bead_id) : NULL;
  	if (!bead) 
  	{
    	if (article->num_beads >= article->max_beads) 
    	{
      		article->max_beads += PDFDOC_BEAD_ALLOC_SIZE;
      		article->beads = (PDFBead*)realloc(article->beads, article->max_beads * sizeof(PDFBead));
      		for (int i = article->num_beads; i < article->max_beads; i++) 
      		{
        		article->beads[i].id = 0;
        		article->beads[i].page_no = -1;
      		}
    	}
    	bead = &(article->beads[article->num_beads]);
    	if (bead_id) 
      		bead->id = qstrdup(bead_id);
    	else 
      		bead->id = 0;
    	article->num_beads++;
  	}
  	
  	bead->rect.llx = rect->llx;
  	bead->rect.lly = rect->lly;
  	bead->rect.urx = rect->urx;
  	bead->rect.ury = rect->ury;
  	bead->page_no  = page_no;
}

void XWDVIRef::addGState(char * resname, XWObject * dict)
{
	PDFPage * currentpage = LASTPAGE;
	if (isScanning())
	{
		XWObject obj;
		refObj(dict, &obj);		
		currentpage->gsLabels = (int*)realloc(currentpage->gsLabels, (currentpage->gsCounter + 1) * sizeof(int));
		currentpage->gsLabels[currentpage->gsCounter] = dict->label;
	}
	else
		dict->label = currentpage->gsLabels[currentpage->gsCounter];
	
	sprintf(resname, "GS%d%d", PAGECOUNT, currentpage->gsCounter);
	currentpage->gsCounter++;
}

int  XWDVIRef::addNames(const char *category,
                        const void *key, 
                        int keylen, 
                        XWObject *value)
{
	int i = 0;
	for (; names[i].category != NULL; i++) 
	{
    	if (!strcmp(names[i].category, category)) 
      		break;
  	}
  	
  	if (names[i].category == NULL) 
  	{
  		QString msg = QString("unknown name dictionary category \"%1\".\n").arg(category);
    	xwApp->warning(msg);
    	return -1;
  	}
  	
  	if (!names[i].data) 
    	names[i].data = new XWPDFNames;

  	return names[i].data->addObject(key, keylen, value);
}

void XWDVIRef::addPageContent(const char *buffer, unsigned length)
{
	PDFPage * currentpage = LASTPAGE;	
#ifdef XW_BUILD_PS_LIB
  if (!isScanning())
  {
  	if (pendingForms) 
  		pendingForms->form.contents.streamAdd(buffer, length);
  	else 
    	currentpage->contents.streamAdd(buffer, length);
  }
  else
  {
  	if (pendingForms) 
  		pendingForms->form.contents.streamAdd(buffer, length);
  }
#else
	if (!deleteFile)
	{
		if (pendingForms) 
    		pendingForms->form.contents.streamAdd(buffer, length);
  	else 
    	currentpage->contents.streamAdd(buffer, length);
  }
  else
  { 
  	if (pageFile && !pendingForms)
  	{
  		currentpage->contents.streamAdd(buffer, length);
  	}
  	else if (isScanning() && pendingForms)
  	{
  		pendingForms->form.contents.streamAdd(buffer, length);
  	}
  }
#endif //XW_BUILD_PS_LIB
}

void XWDVIRef::addPageResource(const char *category,
                               const char *resource_name, 
                               XWObject *resource_ref)
{
	if (!resource_ref->isIndirect())
		return ;
		
	XWObject * resources = getPageResources(category);
	if (!resources)
		return ;
		
	XWObject * duplicate = resources->dictLookupNFOrg(resource_name);
	if (duplicate && (isScanning()) && 
		(duplicate->pf != resource_ref->pf || 
		 duplicate->label != resource_ref->label || 
		duplicate->generation != resource_ref->generation))
	{
		QString msg = QString("conflicting page resource found (page: %1, category: %2, name: %3). Ignoring...\n")
		                           .arg(currentPageNumber()).arg(category).arg(resource_name);
		xwApp->warning(msg);
	}
	else if (!duplicate)
		resources->dictAdd(qstrdup(resource_name), resource_ref);
}

void XWDVIRef::beginAnnot(XWObject *dict)
{
	breakingState.annot_dict = *dict;
  	breakingState.broken = 0;
  	resetBox();
}

void XWDVIRef::beginArticle(const char *article_id, 
		                    XWObject *article_info)
{
	if (article_id == NULL || strlen(article_id) == 0)
	{
    	xwApp->error("Article thread without internal identifier.\n");
    	return ;
    }

  	if (articles.num_entries >= articles.max_entries) 
  	{
    	articles.max_entries += PDFDOC_ARTICLE_ALLOC_SIZE;
    	articles.entries = (PDFArticle*)realloc(articles.entries, articles.max_entries * sizeof(PDFArticle));
  	}
  	PDFArticle * article = &(articles.entries[articles.num_entries]);

  	article->id = qstrdup(article_id);
  	article->info = *article_info;
  	article->num_beads = 0;
  	article->max_beads = 0;
  	article->beads     = 0;

  	articles.num_entries++;
}

int XWDVIRef::beginGrabbing(XWDVICore * core,
	                        XWDVIDev * dev,
		                    const char *ident,
                            double ref_x, 
                            double ref_y, 
                            const PDFRect *cropbox)
{
	dev->pushGState();
  	FormListNode * fnode = new FormListNode;
  	fnode->prev    = pendingForms;
  	fnode->q_depth = dev->currentDepth();
  	int xobj_id = 0;
  	if (isScanning())
  	{
  		PDFForm * form = &(fnode->form);
  		form->matrix.a = 1.0; 
  		form->matrix.b = 0.0;
  		form->matrix.c = 0.0; 
  		form->matrix.d = 1.0;
  		form->matrix.e = -ref_x;
  		form->matrix.f = -ref_y;

  		form->cropbox.llx = ref_x + cropbox->llx;
  		form->cropbox.lly = ref_y + cropbox->lly;
  		form->cropbox.urx = ref_x + cropbox->urx;
  		form->cropbox.ury = ref_y + cropbox->ury;
  		form->contents.initStream(STREAM_COMPRESS, this);
  		form->resources.initDict(this);
  		XWDVIXFormInfo info;
  		info.matrix.a = 1.0; 
  		info.matrix.b = 0.0;
  		info.matrix.c = 0.0; 
  		info.matrix.d = 1.0;
  		info.matrix.e = -ref_x;
  		info.matrix.f = -ref_y;

  		info.bbox.llx = cropbox->llx;
  		info.bbox.lly = cropbox->lly;
  		info.bbox.urx = cropbox->urx;
  		info.bbox.ury = cropbox->ury;
  	
  		XWObject obj;
  		refObj(&(form->contents), &obj);
  		
  		xobj_id = core->defineImageResource(ident, PDF_XOBJECT_TYPE_FORM, &info, &obj);
	}
	
  	pendingForms = fnode;
  	dev->resetFonts();
  	dev->resetColor(this, 1);

  	return xobj_id;
}

void XWDVIRef::beginPage(XWDVIDev * dev,
		                 double scale, 
		                 double x_origin, 
		                 double y_origin)
{
	PDFTMatrix  M;
  	M.a = scale; 
  	M.b = 0.0;
  	M.c = 0.0; 
  	M.d = scale;
  	M.e = x_origin;
  	M.f = y_origin;

	PDFPage * currentpage = 0;
	if (isScanning())
	{
		if (PAGECOUNT >= MAXPAGES) 
    		resizePageEntries(MAXPAGES + PDFDOC_PAGES_ALLOC_SIZE);
    	
    	currentpage = LASTPAGE;
    	if (currentpage->page_ref.isNull()) 
    	{
    		currentpage->page_obj.initDict(this);
    		refObj(&(currentpage->page_obj), &(currentpage->page_ref));
  		}

  		currentpage->contents.initStream(STREAM_COMPRESS, this);  	
  		currentpage->resources.initDict(this);
  	}
  	else
  	{
  		if (!isBatch())
  		{
  			if (pageFile)
  			{
  				pageFile->close();
  				delete pageFile;
  			}
				pageFile = new QBuffer;
					
				pageFile->open(QBuffer::ReadWrite | QIODevice::Truncate);
			}
  			
  		currentpage = LASTPAGE;
  		
  		pages.bop.label = currentpage->content_refs[0].label;	
  		pages.bop.generation = currentpage->content_refs[0].generation;
  		
  		currentpage->background.label = currentpage->content_refs[1].label;	
  		currentpage->background.generation = currentpage->content_refs[1].generation;
  		
  		if (currentpage->contents.isNull())
  			currentpage->contents.initStream(STREAM_COMPRESS, this);
  		currentpage->contents.label = currentpage->content_refs[2].label;	
  		currentpage->contents.generation = currentpage->content_refs[2].generation;	
  		
  		pages.eop.label = currentpage->content_refs[3].label;	
  		pages.eop.generation = currentpage->content_refs[3].generation;
  		
		pageHash.clear();
		if (!isBatch())
		{
			outputFilePosition = 0;
			outputLinePosition = 0;
		}
  	}
  	
  	currentpage->gsCounter = 0;
  		
  	dev->bop(this, &M);
}

void XWDVIRef::bookmarksAdd(XWObject *dict, int is_open)
{
	PDFOLItem * item = outlines.current;

  	if (!item) 
  	{
    	item = new PDFOLItem;
    	item->dict.initNull();
    	item->parent = 0;
    	outlines.first = item;
  	} 
  	else if (!item->dict.isNull()) 
    	item = item->next;

#define BMOPEN(b,p) (((b) < 0) ? ((outlines.current_depth > opt.outline_open_depth) ? 0 : 1) : (b))

	item->dict    = *dict; 
  	item->first   = 0;
  	item->is_open = BMOPEN(is_open, p);

  	PDFOLItem * next = 0;
  	item->next    = next = new PDFOLItem;
  	next->dict.initNull();
  	next->parent  = item->parent;
  	next->first   = 0;
  	next->is_open = -1;
  	next->next    = 0;

  	outlines.current = item;

  	addGoto(dict);
}

int XWDVIRef::bookmarksDown()
{
	PDFOLItem * item = outlines.current;
  	if (item->dict.isNull()) 
  	{
    	xwApp->warning("empty bookmark node!\n");
    	xwApp->warning("you have tried to jump more than 1 level.\n");

    	item->dict.initDict(this);

		XWObject obj;
		obj.initString(new XWString("<No Title>", strlen("<No Title>")));
    	item->dict.dictAdd(qstrdup("Title"), &obj);

    	XWObject tcolor;
    	tcolor.initArray(this);
    	obj.initReal(1.0);
    	tcolor.arrayAdd(&obj);
    	obj.initReal(0.0);
    	tcolor.arrayAdd(&obj);
    	obj.initReal(0.0);
    	tcolor.arrayAdd(&obj);
    	tcolor.copy(&obj);
    	item->dict.dictAdd(qstrdup("C"), &tcolor);
		obj.initReal(1.0);
    	item->dict.dictAdd(qstrdup("F"), &obj);

#define JS_CODE "app.alert(\"The author of this document made this bookmark item empty!\", 3, 0)"
    	XWObject action;
    	action.initDict(this);
    	obj.initName("JavaScript");    	
    	action.dictAdd(qstrdup("S"), &obj);
    	obj.initString(new XWString(JS_CODE, strlen(JS_CODE)));    	
    	action.dictAdd(qstrdup("JS"), &obj);
    	item->dict.dictAdd(qstrdup("A"), &action);
  	}

  	PDFOLItem * first = 0;
  	item->first    = first = new PDFOLItem;
  	first->dict.initNull();
  	first->is_open = 0;
  	first->parent  = item;
  	first->next    = 0;
  	first->first   = 0;

  	outlines.current = first;
  	outlines.current_depth++;

  	return 0;
}

int XWDVIRef::bookmarksUp()
{
	PDFOLItem * item = outlines.current;
  	if (!item || !item->parent) 
  	{
    	xwApp->warning("can't go up above the bookmark root node!\n");
    	return -1;
  	}
  	
  	PDFOLItem * parent = item->parent;
  	item   = parent->next;
  	if (!parent->next) 
  	{
    	parent->next  = item = new PDFOLItem;
    	item->dict.initNull();
    	item->first   = 0;
    	item->next    = 0;
    	item->is_open = 0;
    	item->parent  = parent->parent;
  	}
  	
  	outlines.current = item;
  	outlines.current_depth--;

  	return 0;
}

void XWDVIRef::breakAnnot(XWDVIDev * dev)
{
	if (breakingState.dirty && isScanning())
	{
    	XWObject annot_dict;
    	annot_dict.initDict(this);
    	annot_dict.dictMerge(&(breakingState.annot_dict));
    	addAnnot(dev, currentPageNumber(), &(breakingState.rect), &annot_dict, !breakingState.broken);
    	releaseObj(&annot_dict);

    	breakingState.broken = 1;
  	}
  	resetBox();
}

void XWDVIRef::clear()
{
	if (pages.entries)
	{
		for (int i = 0; i < MAXPAGES; i++)
		{
			if (pages.entries[i].gsLabels)
				free(pages.entries[i].gsLabels);
				
			pages.entries[i].page_obj.free();
			pages.entries[i].resources.free();
			pages.entries[i].background.free();
			pages.entries[i].contents.free();
			pages.entries[i].annots.free();
			pages.entries[i].beads.free();
		}
		
		free(pages.entries);
		pages.entries = 0;
	}
	
	pages.max_entries = 0;
		
	if (pageFile)
	{
		delete pageFile;
		pageFile = 0;
	}
		
	root.dict.free();
	root.dict.initNull();
	root.viewerpref.free();
	root.viewerpref.initNull();
	root.pagelabels.free();
	root.pagelabels.initNull();
	root.pages.free();
	root.pages.initNull();
	root.names.free();
	root.names.initNull();
	root.threads.free();
	root.threads.initNull();
	
	docInfo.free();
	docInfo.initNull();
	
	trailerDict.free();
	trailerDict.initNull();
	
	xrefStream.free();
	xrefStream.initNull();
	
	if (currentObjStm)
	{
		currentObjStm->stream.free();
		currentObjStm->stream.initNull();
		if (currentObjStm->objstm_data)
			delete [] currentObjStm->objstm_data;
			
		delete currentObjStm;
		currentObjStm = 0;
	}
	
	outputStream = 0;
	
	if (outputXRef)
	{
		free(outputXRef);
		outputXRef = 0;
	}
	
	if (pendingForms)
		delete pendingForms;
  pendingForms = 0;
	
	fileHash.clear();
	pageHash.clear();
	pageLabels.clear();
	
	outputFilePosition = 0;
	outputLinePosition = 0;
	
	numObjects = 0;
	
	if (streamEnds)
		free(streamEnds);
		
	streamEndsLen = 0;
	streamEndsSize = 0;
	streamEnds = 0;
}

void XWDVIRef::close()
{
#ifdef XW_BUILD_PS_LIB
  if (scanning)
  {
  	scanning = false;
  	closeArticles();
	closeNames();
	closeBookMarks();
	closePageTree();
	closeDocInfo();
	closeCatalog();
  }
#else
  closeArticles();
  closeNames();
  closeBookMarks();
  closePageTree();
  closeDocInfo();
  closeCatalog();
#endif //XW_BUILD_PS_LIB
}

long  XWDVIRef::currentPageNumber()
{
	return (long) (PAGECOUNT + 1);
}

XWObject * XWDVIRef::currentPageResources()
{
	XWObject  *resources = 0;
	if (pendingForms) 
	{
    	if ((pendingForms->form.resources.isNull())) 
    		pendingForms->form.resources.initDict(this);
      	resources = &(pendingForms->form.resources);
  	} 
  	else 
  	{
    	PDFPage * currentpage = LASTPAGE;
    	if (currentpage->resources.isNull()) 
    		currentpage->resources.initDict(this);
      	resources = &(currentpage->resources);
  	}

  	return resources;
}

void XWDVIRef::endAnnot(XWDVIDev * dev)
{
	breakAnnot(dev);
  	breakingState.annot_dict.initNull();
}

void XWDVIRef::endGrabbing(XWDVIDev * dev, XWObject *attrib)
{
	if (!pendingForms) 
	{
    	xwApp->warning("tried to close a nonexistent form XOject.\n");
    	return;
  	}
  	
  	FormListNode * fnode = pendingForms;
  	PDFForm * form  = &(fnode->form);

	dev->grestoreTo(this, fnode->q_depth);
	if (isScanning())
	{
  		XWObject procset, obj;
  		procset.initArray(this);
  		obj.initName("PDF");
  		procset.arrayAdd(&obj);
  		obj.initName("Text");
  		procset.arrayAdd(&obj);
  		obj.initName("ImageC");
  		procset.arrayAdd(&obj);
  		obj.initName("ImageB");
  		procset.arrayAdd(&obj);
  		obj.initName("ImageI");
  		procset.arrayAdd(&obj);  	
  		form->resources.dictAdd(qstrdup("ProcSet"), &procset);

		refObj(&(form->resources), &obj);
  		makeXForm(&(form->contents), &(form->cropbox), &(form->matrix), &obj, attrib);
  		releaseObj(&(form->resources));
  		releaseObj(&(form->contents));
  		if (attrib) 
  		{
  			attrib->free();
  			attrib->initNull();
  		}
  	}
  	
  	delete pendingForms;

  	pendingForms = fnode->prev;
  
  	dev->popGState();
  	dev->resetFonts();
  	dev->resetColor(this, 0);

  	delete fnode;
}

void XWDVIRef::endPage(XWDVICore * core, XWDVIDev * dev)
{
	dev->eop(this);
  fillPageBackground(dev);
  finishPage(core);
}

int XWDVIRef::escapeStr(char *buffer, 
	                    int bufsize, 
	                    const uchar *s, 
	                    int len)
{
	int result = 0;
	for (int i = 0; i < len; i++)
	{
		uchar ch = s[i];
		if (ch < 32 || ch > 126)
		{
			buffer[result++] = '\\';
			result += sprintf(buffer+result, "%03o", ch);
		}
		else
		{
			switch (ch)
			{
				case '(':
					buffer[result++] = '\\';
					buffer[result++] = '(';
					break;
					
      			case ')':
					buffer[result++] = '\\';
					buffer[result++] = ')';
					break;
					
      			case '\\':
					buffer[result++] = '\\';
					buffer[result++] = '\\';
					break;
					
      			default:
					buffer[result++] = ch;
					break;
			}
		}
	}
	
	return result;
}

void XWDVIRef::expandBox(const PDFRect *rect)
{
	breakingState.rect.llx = qMin(breakingState.rect.llx, rect->llx);
  	breakingState.rect.lly = qMin(breakingState.rect.lly, rect->lly);
  	breakingState.rect.urx = qMax(breakingState.rect.urx, rect->urx);
  	breakingState.rect.ury = qMax(breakingState.rect.ury, rect->ury);
  	breakingState.dirty    = 1;
}

XWObject * XWDVIRef::fetch(int num, int gen, XWObject *obj, int recursion)
{
	if (num < 0)
		return 0;
		
	XWFileStream * strA = 0;
	if (fileHash.contains(num))
	{
		QMultiHash<long, QPair<int, int> >::iterator i = fileHash.find(num);
		while (i != fileHash.end() && i.key() == num)
		{
			if (i.value().first == gen)
			{
				strA = new XWFileStream(outputFile, i.value().second, false, 0, obj);
				break;
			}
			
			++i;
		}
	}
	else if (pageHash.contains(num))
	{
		QMultiHash<long, QPair<int, int> >::iterator i = pageHash.find(num);
		while (i != pageHash.end() && i.key() == num)
		{
			if (i.value().first == gen)
			{
				strA = new XWFileStream(pageFile, i.value().second, false, 0, obj);
				break;
			}
			
			++i;
		}
	}
	else
		return 0;
	
	if (strA)
	{
		XWPDFParser parser(this, new XWPDFLexer(this, strA), true);
		parser.getObj(obj, false, 0, cryptRC4, 0, 0, 0, recursion);
	}
	else
	{
		obj->initNull();
		return 0;
	}
	
	return obj;
}

XWObject * XWDVIRef::getCatalog(XWObject *obj)
{
	return fetch(rootNum, rootGen, obj);
}

QIODevice * XWDVIRef::getCurrentPageFile()
{
	if (isScanning())
		return outputFile;
	PDFPage * currentpage = LASTPAGE;	
	return currentpage->contents.streamGetIO();
}

XWObject * XWDVIRef::getDocInfo(XWObject * obj)
{
	return obj->initRef(infoNum, infoGen);
}

XWObject * XWDVIRef::getDocInfoNF(XWObject * obj)
{
	return fetch(infoNum, infoGen, obj);
}

XWObject * XWDVIRef::getDictionary(const char *category)
{
	XWObject *dict = 0;
	if (!strcmp(category, "Names")) 
	{
    	if (root.names.isNull())
      		root.names.initDict(this);
    	dict = &(root.names);
  	} 
  	else if (!strcmp(category, "Pages")) 
  	{
    	if (root.pages.isNull())
      		root.pages.initDict(this);
    	dict = &(root.pages);
  	} 
  	else if (!strcmp(category, "Catalog")) 
  	{
    	if (root.dict.isNull())
      		root.dict.initDict(this);
    	dict = &(root.dict);
  	} 
  	else if (!strcmp(category, "Info")) 
  	{
    	if (docInfo.isNull())
      		docInfo.initDict(this);
    	dict = &docInfo;
  	} 
  	else if (!strcmp(category, "@THISPAGE")) 
  	{
    	PDFPage * currentpage = LASTPAGE;
    	dict =  &(currentpage->page_obj);
  	}

  	if (!dict) 
  	{
  		QString msg = QString("Document dict. \"%1\" not exist.\n").arg(category);
    	xwApp->error(msg);
    	return 0;
  	}

  	return dict;
}

void XWDVIRef::getMediaBox(unsigned page_no, PDFRect *mediabox)
{
	if (page_no == 0) 
	{
    	mediabox->llx = pages.mediabox.llx;
    	mediabox->lly = pages.mediabox.lly;
    	mediabox->urx = pages.mediabox.urx;
    	mediabox->ury = pages.mediabox.ury;
  	} 
  	else 
  	{
    	PDFPage * page = getPageEntry(page_no);
    	if (page->flags & USE_MY_MEDIABOX) 
    	{
      		mediabox->llx = page->cropbox.llx;
      		mediabox->lly = page->cropbox.lly;
      		mediabox->urx = page->cropbox.urx;
      		mediabox->ury = page->cropbox.ury;
    	} 
    	else 
    	{
      		mediabox->llx = pages.mediabox.llx;
      		mediabox->lly = pages.mediabox.lly;
      		mediabox->urx = pages.mediabox.urx;
      		mediabox->ury = pages.mediabox.ury;
    	}
  	}
}

XWObject * XWDVIRef::getPageDict(unsigned long page_no)
{
	PDFPage * page = getPageEntry(page_no);
  if (page->page_ref.isNull()) 
  {
   	page->page_obj.initDict(this);
   	refObj(&(page->page_obj), &(page->page_ref));
  }
  	
  return &(page->page_obj);
}

XWObject * XWDVIRef::getReference(const char *category, XWObject * ref)
{
	long page_no = currentPageNumber();
  	if (!strcmp(category, "@THISPAGE")) 
    	refPage(page_no, ref);
  	else if (!strcmp(category, "@PREVPAGE")) 
  	{
    	if (page_no <= 1) 
    	{
      		xwApp->error("Reference to previous page, but no pages have been completed yet.\n");
      		return 0;
    	}
    	refPage(page_no - 1, ref);
  	} 
  	else if (!strcmp(category, "@NEXTPAGE")) 
    	refPage(page_no + 1, ref);

  	return ref;
}

bool XWDVIRef::getStreamEnd(uint streamStart, uint *streamEnd)
{
	int a, b, m;

  if (streamEndsLen == 0 ||
      streamStart > streamEnds[streamEndsLen - 1]) {
    return false;
  }

  a = -1;
  b = streamEndsLen - 1;
  // invariant: streamEnds[a] < streamStart <= streamEnds[b]
  while (b - a > 1) {
    m = (a + b) / 2;
    if (streamStart <= streamEnds[m]) {
      b = m;
    } else {
      a = m;
    }
  }
  *streamEnd = streamEnds[b];
  return true;
}

void XWDVIRef::labelObj(XWObject * obj)
{
	if (obj->label == 0)
	{
		obj->label = nextLabel++;
		obj->generation = 0;
	}
}

void XWDVIRef::open()
{	
	XWTFMFile::setNeedMetric(true);
		
	if (pageFile)
	{
		pageFile->close();
		delete pageFile;		
		pageFile = 0;
	}
	
	nextLabel = 1;
	usedLabel = 1;
	
	if (root.dict.isNull())
		initCatalog();
		
	if (docInfo.isNull())
		initDocInfo();
		
	if (trailerDict.isNull())
		setVersion(pdfVersion);
		
	if (!outlines.current)
		initBookmarks();
		
	if (!names)
		initNames();
		
	setBGColor(0);
}


void XWDVIRef::outFlush()
{
	if (!outputFile)
		return ;
		
	if (currentObjStm) 
	{
      	releaseObjstm(currentObjStm);
      	currentObjStm = 0;
    }
    
    if (!xrefStream.isNull())
      	labelObj(&xrefStream);
      	
    startXRef = outputFilePosition;
    XWObject obj;
    obj.initInt(nextLabel);
    trailerDict.dictAdd(qstrdup("Size"), &obj);
    
    if (!xrefStream.isNull())
      	dumpXRefStream();
    else 
    {
      	dumpXRefTable();
      	dumpTrailerDict();
    }
    
    if (outputXRef)
    	free(outputXRef);
    outputXRef = 0;
    	
    out(outputFile, "startxref\n", 10);
    char buf[1024];
    int length = sprintf(buf, "%lu\n", startXRef);
    out(outputFile, buf, length);
    out(outputFile, "%%EOF\n", 6);
}

void XWDVIRef::refObj(XWObject * obj, XWObject * ref)
{
	if (!obj || !ref)
		return ;
		
	if (obj->isIndirect())
	{
		obj->copy(ref);
		ref->refcount++;
	}
	else
	{
		labelObj(obj);
		ref->initIndirect(0, obj->label, obj->generation);
	}
	
	if (isScanning())
		numObjects++;
}

XWObject * XWDVIRef::refPage(unsigned long page_no, XWObject * ref)
{
	PDFPage * page = getPageEntry(page_no);
  	if (page->page_ref.isNull()) 
  	{
    	page->page_obj.initDict(this);
    	refObj(&(page->page_obj), &(page->page_ref));
  	}

  	page->page_ref.copy(ref);
  	
  	return ref;
}

void XWDVIRef::releaseObj(XWObject *object)
{
	if (!object)
		return ;
		
	if (object->refcount <= 0)
	{
		object->free();
		object->initNull();
		return ;
	}
	
	object->refcount -= 1;
  if (object->refcount == 0)
  {
  	if (object->label && (outputFile != 0))
  	{
  		if (!doObjStm || object->flags & OBJ_NO_OBJSTM || 
  			(docEncMode && object->flags & OBJ_NO_ENCRYPT) || 
  			object->generation)
  		{
  			if (pageFile == 0)
  			{
  				flushObj(object, outputFile);
  			}
  			else
  			{
  				flushObj(object, pageFile);
  			}
			}
			else
			{
				if (!currentObjStm)
				{
					currentObjStm = new ObjectSteam;
					currentObjStm->stream.initStream(STREAM_COMPRESS, this);
					long *data = new long[2*OBJSTM_MAX_OBJS+2];
					currentObjStm->objstm_data = data;
					data[0] = data[1] = 0;
					labelObj(&(currentObjStm->stream));
				}
				
				if (addObjStm(currentObjStm, object) == OBJSTM_MAX_OBJS)
				{
					releaseObjstm(currentObjStm);
					currentObjStm = 0;
				}
			}
  	}
  }
  	
  object->free();
  object->initNull();
}

void XWDVIRef::setAnnotGrow(double a)
{
	opt.annot_grow = a;
}

void XWDVIRef::setBGColor(const XWDVIColor * color)
{
	if (color)
		bgcolor.copy(color);
	else
		bgcolor.white();
}

void XWDVIRef::setBookMarkOpen(int i)
{
	opt.outline_open_depth = ((i >= 0) ? i : 256 - i);
}

void XWDVIRef::setBopContent(const char *content, unsigned length)
{
	if (!pages.bop.isNull()) 
    	releaseObj(&(pages.bop));

  	if (length > 0) 
  	{
    	pages.bop.initStream(STREAM_COMPRESS, this);
    	pages.bop.streamAdd(content, length);
  	} 
}

void XWDVIRef::setCheckGotos(bool e)
{
	checkGotos = e;
}

void XWDVIRef::setCompressionLevel(int level)
{
	if (level >= 0 && level <= 9)
		compressionLevel = level;
}

void XWDVIRef::setCreator(const char *doccreator)
{
	if (!doccreator || doccreator[0] == '\0')
    return;
    
	XWObject obj;
	obj.initString(new XWString(doccreator));
	docInfo.dictAdd(qstrdup("Creator"), &obj);
}

void XWDVIRef::setCurrentPageNumber(long page_no)
{
	if (page_no < 0 || pages.max_entries == 0)
		page_no = 0;
	else if (page_no >= pages.max_entries)
		page_no = pages.max_entries - 1;
		
	pages.num_entries = page_no;
}

void XWDVIRef::setEncryption(int do_encryption)
{
	docEncMode = do_encryption;
	encMode = 0;
}

void XWDVIRef::setEopContent(const char *content, unsigned length)
{
	if (!pages.eop.isNull()) 
    	releaseObj(&(pages.eop));

  	if (length > 0) 
  	{
    	pages.eop.initStream(STREAM_COMPRESS, this);
    	pages.eop.streamAdd(content, length);
  	} 
}

void XWDVIRef::setMediaBox(double media_width, double media_height)
{
	pages.mediabox.urx = media_width;
  pages.mediabox.ury = media_height;
}

void XWDVIRef::setMediaBox(unsigned page_no, const PDFRect *mediabox)
{
	if (page_no == 0) 
	{
    	pages.mediabox.llx = mediabox->llx;
    	pages.mediabox.lly = mediabox->lly;
    	pages.mediabox.urx = mediabox->urx;
    	pages.mediabox.ury = mediabox->ury;
  	} 
  	else 
  	{
    	PDFPage * page = getPageEntry(page_no);
    	page->cropbox.llx = mediabox->llx;
    	page->cropbox.lly = mediabox->lly;
    	page->cropbox.urx = mediabox->urx;
    	page->cropbox.ury = mediabox->ury;
    	page->flags |= USE_MY_MEDIABOX;
  	}
}

void XWDVIRef::setThumbBaseName(const char * n)
{
	if (thumbBaseName)
	{
		delete [] thumbBaseName;
		thumbBaseName = 0;
		manualThumbEnabled = 0;
	}
		
	if (!n)
		return ;
		
	thumbBaseName = qstrdup(n);
	manualThumbEnabled = 1;
}

#define BINARY_MARKER "%\344\360\355\370\n"

void XWDVIRef::setVersion(char version)
{
	if (version >= PDF_VERSION_MIN && version <= PDF_VERSION_MAX) 
    	pdfVersion = version;
  
  nextLabel = 1;
	outputFilePosition = 0;
	outputLinePosition = 0;	
	maxIndObjects = 0;
	numObjects = 0;
	addXRefEntry(0, 0, 0, 0xffff);
	
	if (outputFile->isOpen())
		outputFile->close();
	outputFile->open(QIODevice::ReadWrite | QIODevice::Truncate);	
    	
  trailerDict.free();
  doObjStm = 0;
	if (pdfVersion >= 5)
	{
		doObjStm = 1;		
		xrefStream.initStream(STREAM_COMPRESS, this);
		xrefStream.flags |= OBJ_NO_ENCRYPT;
		trailerDict.initDict(xrefStream.streamGetDict());
		XWObject obj1;
		obj1.initName("XRef");
		trailerDict.dictAdd(qstrdup("Type"), &obj1);
	}
	else
		trailerDict.initDict(this);
		
	outputFile->seek(0);
	
	out(outputFile, "%PDF-1.", 7);
	char v = '0' + pdfVersion;
	out(outputFile, &v, 1);
	out(outputFile, "\n", 1);
	out(outputFile, BINARY_MARKER, strlen(BINARY_MARKER));
	
	XWObject obj;
	refObj(&(root.dict), &obj);
	trailerDict.dictAdd(qstrdup("Root"), &obj);
	
	refObj(&docInfo, &obj);
	trailerDict.dictAdd(qstrdup("Info"), &obj);
	
	if (docEncMode)
	{
    root.dict.flags |= OBJ_NO_OBJSTM;
    XWObject encrypt;
    XWDVIEncrypt::encryptObj(&encrypt, this);
  	refObj(&encrypt, &obj);
  	trailerDict.dictAdd(qstrdup("Encrypt"), &obj);
  	encrypt.flags |= OBJ_NO_ENCRYPT;
    releaseObj(&encrypt);
  }
	
	XWDVIEncrypt::idArray(&obj, this);
  trailerDict.dictAdd(qstrdup("ID"), &obj);
}

void XWDVIRef::addGoto(XWObject *annot_dict)
{
	if (!checkGotos)
    	return;
    	
    XWObject subtype, obj, D, A, S;
    annot_dict->dictLookupNF("Subtype", &obj);
    if (XWPDFFile::derefObj(this, &obj, &subtype))
    {
    	obj.free();
    	if (subtype.isNull())
      		goto undefined;
    	else if (!subtype.isName())
      		goto error;
    	else if (strcmp(subtype.getName(), "Link"))
      		goto cleanup;
    }
    else
    	obj.free();
    	
    XWObject * dict = annot_dict;
    char * key = "Dest";
    annot_dict->dictLookupNF(key, &obj);
    
    if (XWPDFFile::derefObj(this, &obj, &D))
    {
    	if (obj.isUnknown())
    	{
    		obj.free();
    		goto undefined;
    	}
    }
    
    obj.free();
    annot_dict->dictLookupNF("A", &obj);
    if (XWPDFFile::derefObj(this, &obj, &A))
    {
    	obj.free();
    	if (A.isNull())
      		goto undefined;
    	else if (!D.isNull() || !A.isDict())
      		goto error;
    	else 
    	{
    		A.dictLookupNF("S", &obj);
    		XWObject S;
      		XWPDFFile::derefObj(this, &obj, &S);
      		obj.free();
      		if (S.isNull())
				goto undefined;
      		else if (!S.isName())
				goto error;
      		else if (strcmp(S.getName(), "GoTo"))
				goto cleanup;

      		dict = &A;
      		key = "D";
      		A.dictLookupNF(key, &obj);
      		if (!D.isNull())
      			D.free();
      		XWPDFFile::derefObj(this, &obj, &D);
      		obj.free();
    	}
    }
    else
    	obj.free();
    	
    char * dest = 0;
    if (D.isString())
    	dest = (char *)(D.getString()->getCString());
    else if (D.isArray())
    	goto cleanup;
    else if (D.isNull())
    	goto undefined;
  	else
    	goto error;
    	
    XWObject * D_new = gotos->lookupTable(dest, strlen(dest));
    obj.initNull();
    if (!D_new)
    {
    	char buf[10];
    	sprintf(buf, "%lx", gotos->tableSize());
    	obj.initString(new XWString(buf, strlen(buf)));
    	D_new = &obj;
    	gotos->insertTable(dest, strlen(dest), &obj);
    }
    
    {
    	XWObject tmp;
    	D_new->copy(&tmp);
    	dict->dictAdd(qstrdup(key), &tmp);
  	}
  	
cleanup:
  	if (!subtype.isNull())
    	subtype.free();
  	if (!A.isNull())
    	A.free();
  	if (!S.isNull())
    	S.free();
  	if (!D.isNull())
    	D.free();

  	return;

error:
  	xwApp->warning("unknown PDF annotation format. Output file may be broken.\n");
  	goto cleanup;

undefined:
  	xwApp->warning("cannot optimize PDF annotations. Output file may be broken."
       				" Please restart with option \"-C 0x10\"\n");
  	goto cleanup;
}

long XWDVIRef::addObjStm(ObjectSteam *objstm, XWObject *object)
{
	long * data = objstm->objstm_data;
  	long   pos = ++data[0];
  	data[2*pos]   = object->label;
  	data[2*pos+1] = objstm->stream.streamGetLength();
  	addXRefEntry(object->label, 2, objstm->stream.label, pos - 1);
  	outputStream = objstm;
  	encMode = 0;
  	writeObj(object, outputFile);
  	outChar(outputFile, '\n');
  	outputStream = 0;

  	return pos;
}

void XWDVIRef::addXRefEntry(int label, 
	                        uchar type, 
	                        int field2, 
	                        int field3)
{
	if ((ulong)label >= maxIndObjects) 
	{
    	maxIndObjects = (label / IND_OBJECTS_ALLOC_SIZE + 1)*IND_OBJECTS_ALLOC_SIZE;
    	outputXRef = (DVIRefEntry*)realloc(outputXRef, maxIndObjects * sizeof(DVIRefEntry));
  	}

  	outputXRef[label].type   = type;
  	outputXRef[label].offset = field2;
  	outputXRef[label].gen = field3;
  	outputXRef[label].direct.initNull();
  	outputXRef[label].indirect.initNull();
}

long XWDVIRef::asnDate(char *date_string)
{
	QDateTime dt = QDateTime::currentDateTime();
	QString dtstr = dt.toString("yyyyMMddhhmmss");
	
	QDateTime ut = dt.toUTC();		
	dtstr.insert(0, "D:");
	
	long h = dt.time().hour() - ut.time().hour();
	if (h >= 0)
		dtstr += "+";
	else
		dtstr += "-";
		
	h = qAbs(h);
	dtstr += QString("%1").arg(h, 2, 10, QChar('0'));
	long s = dt.time().minute() - ut.time().minute();
	h = qAbs(s);
	dtstr += QString("'%1'").arg(s, 2, 10, QChar('0'));
		
	QByteArray ba = dtstr.toAscii();
	long len = sprintf(date_string, "%s", ba.constData());
	
	return len;
}

XWObject * XWDVIRef::buildPageTree(PDFPage *firstpage, 
	                               long num_pages,
                                   XWObject *parent_ref,
                                   XWObject * self)
{
	self->initDict(this);
	
	XWObject self_ref;
	if (parent_ref)
		refObj(self, &self_ref);
	else
		refObj(&(root.pages), &self_ref);
		
	XWObject obj;
	obj.initName("Pages");
	self->dictAdd(qstrdup("Type"), &obj);
	obj.initInt(num_pages);
	self->dictAdd(qstrdup("Count"), &obj);
	if (parent_ref != 0)
    	self->dictAdd(qstrdup("Parent"), parent_ref);
    	
    XWObject kids;
    kids.initArray(this);
    if (num_pages > 0 && num_pages <= PAGE_CLUSTER)
    {
    	for (int i = 0; i < num_pages; i++) 
    	{
      		PDFPage * page = firstpage + i;
      		if (page->page_ref.isNull())
        		refObj(&(page->page_obj), &(page->page_ref));
      		kids.arrayAdd(&(page->page_ref));
      		flushPage(page, &self_ref);
    	}
    }
    else if (num_pages > 0)
    {
    	for (int i = 0; i < PAGE_CLUSTER; i++)
    	{
    		long start = (i * num_pages) / PAGE_CLUSTER;
      		long end   = ((i + 1) * num_pages) / PAGE_CLUSTER;
      		if (end - start > 1) 
      		{
        		XWObject subtree;
        		buildPageTree(firstpage + start, end - start, &self_ref, &subtree);
        		refObj(&subtree, &obj);
        		kids.arrayAdd(&obj);
        		releaseObj(&subtree);
      		}
      		else 
      		{
        		PDFPage * page = firstpage + start;
        		if (page->page_ref.isNull())
          			refObj(&(page->page_obj), &(page->page_ref));
        		kids.arrayAdd(&(page->page_ref));
        		flushPage(page, &self_ref);
      		}
    	}
    }
    
    self->dictAdd(qstrdup("Kids"), &kids);

  	return self;
}

void XWDVIRef::cleanArticle(PDFArticle *article)
{
	if (!article)
    	return;
    
  	if (article->beads) 
  	{
    	for (int i = 0; i < article->num_beads; i++) 
    	{
      		if (article->beads[i].id)
        		delete [] (article->beads[i].id);
    	}
    	free(article->beads);
    	article->beads = 0;
  	}
    
  	if (article->id)
    	delete [] article->id;
  	article->id = 0;
  	article->num_beads = 0;
  	article->max_beads = 0;
}

void XWDVIRef::cleanBookMarks(PDFOLItem *item)
{
	while (item)
	{
		PDFOLItem * next = item->next;
		if (!item->dict.isNull())
		{
			item->dict.free();
			item->dict.initNull();
		}
			
		if (item->first)
			cleanBookMarks(item->first);
		delete item;
		item = next;
	}
}

void XWDVIRef::closeArticles()
{
	XWObject obj;
	for (int i = 0; i < articles.num_entries; i++) 
	{
    	PDFArticle * article = &(articles.entries[i]);
    	if (article->beads) 
    	{
      		XWObject art_dict;

      		makeArticle(article, NULL, 0, NULL, &art_dict);
      		if (root.threads.isNull()) 
        		root.threads.initArray(this);
        		
        	refObj(&art_dict, &obj);
      		root.threads.arrayAdd(&obj);
      		releaseObj(&art_dict);
    	}
    	cleanArticle(article);
  	}
  	
  	if (articles.entries)
  		free(articles.entries);
  	articles.entries = 0;
  	articles.num_entries = 0;
  	articles.max_entries = 0;

  	if (!root.threads.isNull()) 
  	{
  		refObj(&(root.threads), &obj);
    	root.dict.dictAdd(qstrdup("Threads"), &obj);
    	releaseObj(&(root.threads));
  	}
}

void XWDVIRef::closeBookMarks()
{
	PDFOLItem * item = outlines.first;
  	if (!item->dict.isNull()) 
  	{
    	XWObject bm_root, bm_root_ref, obj;
    	bm_root.initDict(this);
    	refObj(&bm_root, &bm_root_ref);
    	int count = flushBookMarks(item, &bm_root_ref, &bm_root);
    	obj.initInt(count);
    	bm_root.dictAdd(qstrdup("Count"), &obj);
    	root.dict.dictAdd(qstrdup("Outlines"), &bm_root_ref);
    	releaseObj(&bm_root);
  	}
  	
  	cleanBookMarks(item);

  	outlines.first   = 0;
  	outlines.current = 0;
  	outlines.current_depth = 0;
}

void XWDVIRef::closeCatalog()
{
	XWObject tmp, obj;
	tmp.initNull();
	obj.initNull();
	if (!root.viewerpref.isNull())
	{
		root.dict.dictLookupNF("ViewerPreferences", &tmp);
		if (tmp.isNull())
		{
			refObj(&(root.viewerpref), &obj);
			root.dict.dictAdd(qstrdup("ViewerPreferences"), &obj);
		}
		else if (tmp.isDict())
		{
			root.viewerpref.dictMerge(&tmp);
			refObj(&(root.viewerpref), &obj);
			root.dict.dictAdd(qstrdup("ViewerPreferences"), &obj);
		}
			
		tmp.free();
		releaseObj(&(root.viewerpref));
	}
	
	if (!root.pagelabels.isNull())
	{
		root.dict.dictLookupNF("PageLabels", &tmp);
		if (tmp.isNull())
		{
			tmp.initDict(this);
			root.pagelabels.copy(&obj);
			tmp.dictAdd(qstrdup("Nums"), &obj);
			refObj(&tmp, &obj);
			root.dict.dictAdd(qstrdup("PageLabels"), &obj);
			releaseObj(&tmp);
		}
		else
			tmp.free();
			
		root.pagelabels.free();
		root.pagelabels.initNull();
	}
	rootNum = root.dict.label;
	rootGen = root.dict.generation;
	
	releaseObj(&(root.dict));
}

void XWDVIRef::closeDocInfo()
{
	const char *keys[] = {
    	"Title", "Author", "Subject", "Keywords", "Creator", "Producer",
    	"CreationDate", "ModDate", /* Date */
    	NULL
  	};
  	
  	XWObject value;
  	value.initNull();
  	for (int i = 0; keys[i] != NULL; i++) 
  	{
    	docInfo.dictLookupNF(keys[i], &value);
    	if (!value.isNull()) 
    	{
      		if (!value.isString()) 
      		{
      			QString msg = QString("\"%1\" in DocInfo dictionary is not string type and "
      			                      "is removed from DocInfo.\n").arg(keys[i]);
				xwApp->warning(msg);
        		docInfo.dictRemove(keys[i]);
      		} 
      		else if (value.getString()->getLength() == 0) 
        		docInfo.dictRemove(keys[i]);
      		
      		value.free();
      		value.initNull();
    	}
  	}
  	
  	char banner[1024];
  	sprintf(banner, "%s (%s)", xwApp->getProductName8(), xwApp->getVersion8());
  	value.initString(new XWString(banner, strlen(banner)));
  	docInfo.dictAdd(qstrdup("Producer"), &value);
  	value.initNull();
  	docInfo.dictLookupNF("CreationDate", &value);
  	if (value.isNull())
  	{
  		char now[32];

    	asnDate(now);
    	value.initString(new XWString(now, strlen(now)));
    	docInfo.dictAdd(qstrdup("CreationDate"), &value);
  	}
  	else
  		value.free();
  	
  	infoNum = docInfo.label;
  	infoGen = docInfo.generation;
  	releaseObj(&docInfo);
}

void XWDVIRef::closeNames()
{
	for (int i = 0; names[i].category != NULL; i++)
	{
		if (names[i].data) 
		{
      		XWPDFNames *data = names[i].data;
      		XWObject name_tree, obj;
      		name_tree.initNull();
      		long count;

      		if (!checkGotos || strcmp(names[i].category, "Dests"))
				data->createTree(this, &count, 0, &name_tree);
      		else 
      		{
				data->createTree(this, &count, gotos, &name_tree);
				if (count < gotos->tableSize())
	  				warnUndefDests(data, gotos);
      		}

      		if (!name_tree.isNull()) 
      		{
				if (root.names.isNull())
	  				root.names.initDict(this);
	  			refObj(&name_tree, &obj);
				root.names.dictAdd(qstrdup(names[i].category), &obj);
				releaseObj(&name_tree);
      		}
      		
      		delete (names[i].data);
      		names[i].data = 0;
    	}
	}
	
	if (!root.names.isNull()) 
	{
		XWObject tmp;
    	root.dict.dictLookupNF("Names", &tmp);
    	if (tmp.isNull()) 
    	{
    		refObj(&(root.names), &tmp);
      		root.dict.dictAdd(qstrdup("Names"), &tmp);
    	} 
    	else if (tmp.isDict()) 
    	{
      		root.names.dictMerge(&tmp);
      		tmp.free();
      		refObj(&(root.names), &tmp);
      		root.dict.dictAdd(qstrdup("Names"), &tmp);
    	} 
    	else 
    	{ /* Maybe reference */
      		/* What should I do? */
      		tmp.free();
      		xwApp->warning("could not modify Names dictionary.\n");
    	}
    	releaseObj(&(root.names));
  	}

  	free(names);
  	names = 0;

  	gotos->close(this);
  	delete gotos;
  	gotos = 0;
}

void XWDVIRef::closePageTree()
{
	for (long page_no = PAGECOUNT + 1; page_no <= MAXPAGES; page_no++)
	{
		PDFPage * page = getPageEntry(page_no);
		if (!(page->page_obj.isNull()))
		{
			QString msg = QString("nonexistent page #%1 refered.\n").arg(page_no);
      		xwApp->warning(msg);
      		page->page_ref.free();
      		page->page_ref.initNull();
    	}
    	
    	if (!(page->page_obj.isNull())) 
    	{
    		QString msg = QString("entry for a nonexistent page #%1 created.\n").arg(page_no);
      		xwApp->warning(msg);
      		page->page_obj.free();
      		page->page_obj.initNull();
    	}
    	
    	if (!(page->annots.isNull()))
    	{
    		QString msg = QString("Annotation attached to a nonexistent page #%1.\n").arg(page_no);
      		xwApp->warning(msg);
      		page->annots.free();
      		page->annots.initNull();
    	}
    	
    	if (!(page->beads.isNull()))
    	{
    		QString msg = QString("Article beads attached to a nonexistent page #%1.\n").arg(page_no);
      		xwApp->warning(msg);
      		page->beads.free();
      		page->beads.initNull();
    	}
    	
    	if (!(page->resources.isNull()))
    	{
      		page->resources.free();
      		page->resources.initNull();
      	}
	}
	
	XWObject page_tree_root;
	buildPageTree(FIRSTPAGE, PAGECOUNT, NULL, &page_tree_root);
  	root.pages.dictMerge(&page_tree_root);
  	page_tree_root.free();
  	page_tree_root.initNull();
  	
  	if (!pages.bop.isNull()) 
  	{
    	pages.bop.streamAdd("\n", 1);
    	releaseObj(&(pages.bop));
  	}
  	
  	if (!pages.eop.isNull()) 
  	{
    	pages.eop.streamAdd("\n", 1);
    	releaseObj(&(pages.eop));
  	}
  	
  	XWObject mediabox, obj;
  	mediabox.initArray(this);
  	obj.initInt(round(pages.mediabox.llx, 0.01));
  	mediabox.arrayAdd(&obj);
  	obj.initInt(round(pages.mediabox.lly, 0.01));
  	mediabox.arrayAdd(&obj);
  	obj.initInt(round(pages.mediabox.urx, 0.01));
  	mediabox.arrayAdd(&obj);
  	obj.initInt(round(pages.mediabox.ury, 0.01));
  	mediabox.arrayAdd(&obj);
  	root.pages.dictAdd(qstrdup("MediaBox"), &mediabox);

  	refObj(&(root.pages), &obj);
  	root.dict.dictAdd(qstrdup("Pages"), &obj);
  	releaseObj(&(root.pages));

		pages.num_entries = 0;
}

void XWDVIRef::dumpTrailerDict()
{
	out(outputFile, "trailer\n", 8);
  	encMode = 0;
  	writeDict(&trailerDict, outputFile);  	
  	outChar(outputFile, '\n');
  	trailerDict.free();
  	trailerDict.initNull();
}

void XWDVIRef::dumpXRefStream()
{
	uchar buf[7] = {0, 0, 0, 0, 0};
	ulong pos = startXRef;
	uint poslen = 1;
  	while (pos >>= 8)
    	poslen++;
    	
    XWObject w;
    w.initArray(this);
    XWObject obj;
    obj.initInt(1);
    w.arrayAdd(&obj);
    obj.initInt(poslen);
    w.arrayAdd(&obj);
    obj.initInt(2);
    w.arrayAdd(&obj);
    
    trailerDict.dictAdd(qstrdup("W"), &w);
    addXRefEntry(nextLabel - 1, 1, startXRef, 0);
    for (int i = 0; i < nextLabel; i++)
    {
    	buf[0] = outputXRef[i].type;
    	pos = outputXRef[i].offset;
    	for (ulong j = poslen; j--; ) 
    	{
      		buf[1+j] = (uchar)pos;
      		pos >>= 8;
    	}
    	int f3 = outputXRef[i].gen;
    	buf[poslen + 1] = (uchar) (f3 >> 8);
    	buf[poslen + 2] = (uchar) (f3);
    	xrefStream.streamAdd((const char*)buf, poslen + 3);
    }
    
    releaseObj(&xrefStream);
}

void XWDVIRef::dumpXRefTable()
{
	out(outputFile, "xref\n", 5);
	char buf[100];
	int length = sprintf(buf, "%d %lu\n", 0, nextLabel);
  	out(outputFile, buf, length);
  	for (int i = 0; i < nextLabel; i++)
  	{
  		uchar type = outputXRef[i].type;
  		if (type <= 1)
  		{
  			length = sprintf(buf, "%010lu %05hu %c \n", 
  			                 outputXRef[i].offset, outputXRef[i].gen,
		                     type ? 'n' : 'f');
            out(outputFile, buf, length);
  		}
  	}
}

void  XWDVIRef::fillPageBackground(XWDVIDev * dev)
{
	int cm = dev->getParam(PDF_DEV_PARAM_COLORMODE);
  	if (!cm || bgcolor.isWhite()) 
    	return;
    	
    PDFRect   r;
    getMediaBox(currentPageNumber(), &r);

  	PDFPage * currentpage = LASTPAGE;
  	if (currentpage->background.isNull())
    	currentpage->background.initStream(STREAM_COMPRESS, this);

  	XWObject saved_content = currentpage->contents;
  	currentpage->contents = currentpage->background;

  	dev->gsave(this);
  	dev->setNonStrokingColor(this, &bgcolor);
  	dev->rectFill(this, r.llx, r.lly, r.urx - r.llx, r.ury - r.lly);
  	dev->grestore(this);

  	currentpage->contents = saved_content;
}

PDFBead * XWDVIRef::findBead(PDFArticle *article, 
	                         const char *bead_id)
{
	PDFBead * bead = NULL;
  	for (int i = 0; i < article->num_beads; i++) 
  	{
    	if (!strcmp(article->beads[i].id, bead_id)) 
    	{
      		bead = &(article->beads[i]);
      		break;
    	}
  	}

  	return bead;
}

void  XWDVIRef::finishPage(XWDVICore * core)
{
	XWObject procset, obj;
	if (pendingForms) 
	{
    	xwApp->error("a pending form XObject at the end of page.\n");
    	return;
  	}
  	
  	PDFPage * currentpage = LASTPAGE;  	
  	if (currentpage->page_ref.isNull())
  	{
    	currentpage->page_obj.initDict(this);    	
    	refObj(&(currentpage->page_obj), &(currentpage->page_ref));
    }
        
    if (!pages.bop.isNull() && pages.bop.streamGetLength() > 0) 
    		refObj(&(pages.bop), &(currentpage->content_refs[0]));
  	else 
    	currentpage->content_refs[0].initNull();
    		
    if (!currentpage->background.isNull() || !bgcolor.isWhite()) 
    {
    	if (currentpage->background.streamGetLength() > 0 || !bgcolor.isWhite()) 
    	{
    		refObj(&(currentpage->background), &(currentpage->content_refs[1]));
      	currentpage->background.streamAdd ("\n", 1);
    	}
    	
    	releaseObj(&(currentpage->background));
  	}
    
    if (!currentpage->contents.isNull())
    {
    	if (currentpage->content_refs[2].isNull())
    		refObj(&(currentpage->contents), &(currentpage->content_refs[2]));
  		  			
  		if (!isScanning())
  		{
  			currentpage->contents.streamAdd("\n", 1);
  			releaseObj(&(currentpage->contents));
  		}
  	}
  	
  	if (!pages.eop.isNull() && pages.eop.streamGetLength() > 0) 
    	refObj(&(pages.eop), &(currentpage->content_refs[3]));
    else
    	currentpage->content_refs[3].initNull();
  	
  	if (!currentpage->resources.isNull() && (isScanning()))
  	{
    	procset.initArray(this);
    	obj.initName("PDF");
    	procset.arrayAdd(&obj);
    	obj.initName("Text");
    	procset.arrayAdd(&obj);
    	obj.initName("ImageC");
    	procset.arrayAdd(&obj);
    	obj.initName("ImageB");
    	procset.arrayAdd(&obj);
    	obj.initName("ImageI");
    	procset.arrayAdd(&obj);
    	
    	currentpage->resources.dictAdd(qstrdup("ProcSet"), &procset);

			refObj(&(currentpage->resources), &obj);
    	currentpage->page_obj.dictAdd(qstrdup("Resources"), &obj);
    	releaseObj(&(currentpage->resources));
  	}

  	if (manualThumbEnabled && thumbBaseName) 
  	{
    	char * thumb_filename = new char[strlen(thumbBaseName)+7];
    	sprintf(thumb_filename, "%s.%ld", thumbBaseName, (pages.num_entries % 99999) + 1L);
    	XWObject thumb_ref;
    	thumb_ref.initNull();
    	readThumbNail(core, thumb_filename, &thumb_ref);
    	delete [] thumb_filename;
    	if (!thumb_ref.isNull() && (isScanning()))
      		currentpage->page_obj.dictAdd(qstrdup("Thumb"), &thumb_ref);
  	}

  	pages.num_entries++;
}

int  XWDVIRef::flushBookMarks(PDFOLItem *node,
                 		      XWObject *parent_ref, 
                 		      XWObject *parent_dict)
{
	XWObject this_ref, obj;
	refObj(&(node->dict), &this_ref);
	parent_dict->dictAdd(qstrdup("First"), &this_ref);
	int retval = 0;
	int count = 0;
	PDFOLItem *item = 0;
	XWObject prev_ref, next_ref;	
	prev_ref.initNull();
	next_ref.initNull();
	for (item = node; item && !item->dict.isNull(); item = item->next)
	{
		if (item->first && !(item->first->dict.isNull()))
		{
      		count = flushBookMarks(item->first, &this_ref, &(item->dict));
      		if (item->is_open) 
      		{
      			obj.initInt(count);
        		item->dict.dictAdd(qstrdup("Count"), &obj);
        		retval += count;
      		} 
      		else 
      		{
      			obj.initInt(-count);
        		item->dict.dictAdd(qstrdup("Count"), &obj);
      		}
    	}
    	
    	item->dict.dictAdd(qstrdup("Parent"), parent_ref);
    	if (!prev_ref.isNull()) 
    		item->dict.dictAdd(qstrdup("Prev"), &prev_ref);
    	
    	if (item->next && !(item->next->dict.isNull())) 
    	{
      		refObj(&(item->next->dict), &next_ref);
      		item->dict.dictAdd(qstrdup("Next"), &next_ref);
    	} 
    	else 
      		next_ref.initNull();

    	releaseObj(&(item->dict));

    	prev_ref = this_ref;
    	this_ref = next_ref;
    	retval++;    
	}
	
	parent_dict->dictAdd(qstrdup("Last"), &prev_ref);

  	releaseObj(&(node->dict));

  	return retval;
}

void XWDVIRef::flushObj(XWObject * obj, QIODevice * file)
{
    if (!deleteFile)
		  addXRefEntry(obj->label, 1, outputFilePosition, obj->generation);		
	
	char buf[50];
	long length = sprintf(buf, "%lu %hu obj\n", obj->label, obj->generation);
	encMode = docEncMode && !(obj->flags & OBJ_NO_ENCRYPT);
  	XWDVIEncrypt::setLabel(obj->label);
  	XWDVIEncrypt::setGeneration(obj->generation);
  	out(file, buf, length);
  	if (deleteFile)
  	{
  		int p = file->pos();
  		QPair<int, int> pair(obj->generation, p);
  		if (file == outputFile)
  			fileHash.insert(obj->label, pair);
  		else
  			pageHash.insert(obj->label, pair);
  	}
  	
  	writeObj(obj, file);
  	out(file, "\nendobj\n", 8);
  	
  	if (nextLabel > usedLabel)
  		usedLabel = nextLabel;
}

void XWDVIRef::flushPage(PDFPage *page, XWObject *parent_ref)
{
	XWObject obj;
	obj.initName("Page");
	page->page_obj.dictAdd(qstrdup("Type"), &obj);
	page->page_obj.dictAdd(qstrdup("Parent"), parent_ref);
			
	if (page->flags & USE_MY_MEDIABOX)
	{
		XWObject mediabox;
		mediabox.initArray(this);
		obj.initInt(round(page->cropbox.llx, 0.01));
		mediabox.arrayAdd(&obj);
		obj.initInt(round(page->cropbox.lly, 0.01));
		mediabox.arrayAdd(&obj);
		obj.initInt(round(page->cropbox.urx, 0.01));
		mediabox.arrayAdd(&obj);
		obj.initInt(round(page->cropbox.ury, 0.01));
		mediabox.arrayAdd(&obj);
		page->page_obj.dictAdd(qstrdup("MediaBox"), &mediabox);
	}
	
	int count = 0;
  	XWObject contents_array;
  	contents_array.initArray(this);
  	if (!(page->content_refs[0].isNull()))
  	{ /* global bop */
    	contents_array.arrayAdd(&(page->content_refs[0]));
    	count++;
  	} 
  	else if (!(pages.bop.isNull()) && pages.bop.streamGetLength() > 0) 
  	{
  		refObj(&(pages.bop), &obj);
  		contents_array.arrayAdd(&obj);
    	count++;
  	}
  	
  	if (!(page->content_refs[1].isNull()))
  	{ /* background */
  		contents_array.arrayAdd(&(page->content_refs[1]));
    	count++;
  	}
  	
  	if (!(page->content_refs[2].isNull()))
  	{ /* page body */
    	contents_array.arrayAdd(&(page->content_refs[2]));
    	count++;
  	}
  	
  	if (!(page->content_refs[3].isNull()))
  	{ /* global eop */
  		contents_array.arrayAdd(&(page->content_refs[3]));
    	count++;
  	} 
  	else if (!pages.eop.isNull() && pages.eop.streamGetLength() > 0) 
  	{
  		refObj(&(pages.eop), &obj);
    	contents_array.arrayAdd(&obj);
    	count++;
  	}

  	if (count == 0) 
    	xwApp->warning("page with empty content found!!!\n");
    
  	page->page_obj.dictAdd(qstrdup("Contents"), &contents_array);

  	if (!(page->annots.isNull()))
  	{
  		refObj(&(page->annots), &obj);
    	page->page_obj.dictAdd(qstrdup("Annots"), &obj);
    	releaseObj(&(page->annots));
  	}
  	
  	if (!(page->beads.isNull()))
  	{
  		refObj(&(page->beads), &obj);
    	page->page_obj.dictAdd(qstrdup("B"), &obj);
    	releaseObj(&(page->beads));
  	}
  	
  	releaseObj(&(page->page_obj));
}

PDFPage * XWDVIRef::getPageEntry(long page_no)
{
  	if (page_no > 65535ul) 
  	{
  		QString msg = QString("page number %1 too large!\n").arg(page_no);
    	xwApp->error(msg);
    	return 0;
  	} 
  	else if (page_no == 0) 
  	{
  		QString msg = QString("invalid Page number %1.\n").arg(page_no);
    	xwApp->error(msg);
    	return 0;
  	}

  	if (page_no > MAXPAGES) 
    	resizePageEntries(page_no + PDFDOC_PAGES_ALLOC_SIZE);

  	PDFPage * page = &(pages.entries[page_no - 1]);

  	return page;
}

XWObject * XWDVIRef::getPageResources(const char *category)
{
	if (!category)
		return 0;
		
	XWObject *res_dict = 0;
	if (pendingForms) 
	{
    	if (pendingForms->form.resources.isNull()) 
    		pendingForms->form.resources.initDict(this);
      	res_dict = &(pendingForms->form.resources);
  	} 
  	else 
  	{
    	PDFPage * currentpage = LASTPAGE;
    	if (currentpage->resources.isNull()) 
    		currentpage->resources.initDict(this);
    	res_dict = &(currentpage->resources);
  	}
  	
  	XWObject * resources = res_dict->dictLookupNFOrg(category);
  	if (!resources)
  	{
  		XWObject obj;
  		obj.initDict(this);
  		res_dict->dictAdd(qstrdup(category), &obj);
  		resources = res_dict->dictLookupNFOrg(category);
  	}
  	
  	return resources;
}

void XWDVIRef::init()
{
#ifdef XW_BUILD_PS_LIB
  scanning = true;
#endif
	XWTFMFile::setNeedMetric(true);
		
	pageFile = 0;
	
	nextLabel = 1;
	usedLabel = 1;
		
	compressionLevel = 9;
	pdfVersion = 4;
		
	xrefStream.initNull();
	currentObjStm = 0;
	outputStream = 0;
	outputXRef = 0;
	
	startXRef = 0;
	doObjStm = 0;
	maxIndObjects = 0;
	
	trailerDict.initNull();
	
	infoNum = -1;
	infoGen = -1;
	rootNum = -1;
	rootGen = -1;
	numObjects = 0;
	outputFilePosition = 0;
	outputLinePosition = 0;
	encMode = 0;
	docEncMode = 0;
	
	pages.entries = 0;
	manualThumbEnabled = false;
	thumbBaseName = 0;
	pendingForms = 0;
	
	streamEndsLen = 0;
	streamEndsSize = 0;
	streamEnds = 0;
	
	debugMode = false;
	
	trailerDict.initNull();
	
	initCatalog();
	initDocInfo();
	
	opt.outline_open_depth = 0;
	initBookmarks();
  
  articles.num_entries = 0;
	articles.max_entries = 0;
	articles.entries     = 0;
	
	initNames();
  checkGotos = true;
		
	pages.num_entries = 0;
	pages.max_entries = 0;
	pages.entries     = 0;
	pages.bop.initNull();
	pages.eop.initNull();
	pages.mediabox.llx = 0.0;
  pages.mediabox.lly = 0.0;
  
  setBGColor(0);
	
	breakingState.dirty = 0;
	breakingState.broken = 0;
	breakingState.annot_dict.initNull();
	breakingState.rect.llx = 0.0;
	breakingState.rect.lly = 0.0;
	breakingState.rect.urx = 0.0;
	breakingState.rect.ury = 0.0;
}

void XWDVIRef::initBookmarks()
{
	outlines.current_depth = 1;
	PDFOLItem *item = new PDFOLItem;
	item->dict.initNull();
  item->next    = NULL;
  item->first   = NULL;
  item->parent  = NULL;
  item->is_open = 1;

  outlines.current = item;
  outlines.first   = item;
}

void XWDVIRef::initCatalog()
{
	root.dict.initDict(this);	
	XWObject obj;
	obj.initName("Catalog");
	root.dict.dictAdd(qstrdup("Type"), &obj);
	
	root.pages.initDict(this);
	root.viewerpref.initNull();
	root.pagelabels.initNull();
	root.names.initNull();
	root.threads.initNull();
}

void XWDVIRef::initDocInfo()
{
	docInfo.initDict(this);
}

void XWDVIRef::initNames()
{
	names = (NameDict*)malloc((NUM_NAME_CATEGORY + 1) * sizeof(NameDict));
	for (int i = 0; i < NUM_NAME_CATEGORY; i++)
	{
		names[i].category = (char *) name_dict_categories[i];
    names[i].data     = strcmp(name_dict_categories[i], "Dests") ? 0 : new XWPDFNames;
	}
	
	names[NUM_NAME_CATEGORY].category = 0;
  names[NUM_NAME_CATEGORY].data     = 0;
  gotos = new XWPDFNames;
}

XWObject * XWDVIRef::makeArticle(PDFArticle *article,
              			         const char **bead_ids, 
              			         int num_beads,
                                 XWObject *article_info,
                                 XWObject *art_dict)
{
	art_dict->initDict(this);
	int n = bead_ids ? num_beads : article->num_beads;
		
	XWObject *first, *prev, *last, obj;
	first = 0;
	prev = 0;
	last = 0;
  	for (int i = 0; i < n; i++)
  	{
  		PDFBead * bead = bead_ids ? findBead(article, bead_ids[i]) : &(article->beads[i]);
  		if (!bead || bead->page_no < 0) 
      		continue;
      		
      	last = new XWObject;
      	last->initDict(this);
      	if (prev == 0) 
      	{
      		first = last;
      		refObj(art_dict, &obj);
      		first->dictAdd(qstrdup("T"), &obj);
    	} 
    	else 
    	{
    		refObj(last, &obj);
      		prev->dictAdd(qstrdup("N"), &obj);
      		refObj(prev, &obj);
      		last->dictAdd(qstrdup("V"), &obj);
      		/* We must link first to last. */
      		if (prev != first)
      		{
        		releaseObj(prev);
        		delete prev;
        		prev = 0;
        	}
    	}
    	
    	{
      		PDFPage * page = getPageEntry(bead->page_no);
      		if (page->beads.isNull()) 
        		page->beads.initArray(this);
      		
      		page->page_ref.copy(&obj);
      		last->dictAdd(qstrdup("P"), &obj);
      		XWObject rect;
      		rect.initArray(this);
      		obj.initInt(round(bead->rect.llx, 0.01));
      		rect.arrayAdd(&obj);
      		obj.initInt(round(bead->rect.lly, 0.01));
      		rect.arrayAdd(&obj);
      		obj.initInt(round(bead->rect.urx, 0.01));
      		rect.arrayAdd(&obj);
      		obj.initInt(round(bead->rect.ury, 0.01));
      		rect.arrayAdd(&obj);
      		
      		last->dictAdd(qstrdup("R"), &rect);
      		refObj(last, &obj);
      		page->beads.arrayAdd(&obj);
    	}

    	prev = last;
  	}
  	
  	if (first && last)
  	{
  		refObj(first, &obj);
  		last->dictAdd(qstrdup("N"), &obj);
  		refObj(last, &obj);
  		first->dictAdd(qstrdup("V"), &obj);
  		if (first != last) 
  		{
      		releaseObj(last);
      		delete last;
    	}
    	
    	refObj(first, &obj);
    	art_dict->dictAdd(qstrdup("F"), &obj);
    	if (article_info)
    		art_dict->dictAdd(qstrdup("I"), article_info);
    	else if (!article->info.isNull())
    	{
    		refObj(&(article->info), &obj);
    		art_dict->dictAdd(qstrdup("I"), &obj);
    		releaseObj(&(article->info));
    	}
    	
    	releaseObj(first);
    	delete first;
  	}
  	else 
  		releaseObj(art_dict);
  		
  	return art_dict;
}

void XWDVIRef::makeXForm(XWObject     *xform,
                         PDFRect    *bbox,
                         PDFTMatrix *matrix,
                         XWObject     *resources,
                         XWObject     *attrib)
{
	XWObject obj;
	XWDict * xform_dict = xform->streamGetDict();
	obj.initName("XObject");
	xform_dict->add(qstrdup("Type"), &obj);
	obj.initName("Form");
	xform_dict->add(qstrdup("Subtype"), &obj);
	obj.initReal(1.0);
	xform_dict->add(qstrdup("FormType"), &obj);
	if (!bbox)
	{
    	xwApp->error("no BoundingBox supplied.\n");
    	return ;
    }
    
    XWObject tmp;
    tmp.initArray(this);
    obj.initInt(round(bbox->llx, .001));
    tmp.arrayAdd(&obj);
    obj.initInt(round(bbox->lly, .001));
    tmp.arrayAdd(&obj);
    obj.initInt(round(bbox->urx, .001));
    tmp.arrayAdd(&obj);
    obj.initInt(round(bbox->ury, .001));
    tmp.arrayAdd(&obj);
    
  	xform_dict->add(qstrdup("BBox"), &tmp);

  	if (matrix) 
  	{
    	tmp.initArray(this);
    	obj.initInt(round(matrix->a, .00001));
    	tmp.arrayAdd(&obj);
    	obj.initInt(round(matrix->b, .00001));
    	tmp.arrayAdd(&obj);
    	obj.initInt(round(matrix->c, .00001));    	
    	tmp.arrayAdd(&obj);
    	obj.initInt(round(matrix->d, .00001));
    	tmp.arrayAdd(&obj);
    	obj.initInt(round(matrix->e, .001));
    	tmp.arrayAdd(&obj);
    	obj.initInt(round(matrix->f, .001));
    	tmp.arrayAdd(&obj);
    	xform_dict->add(qstrdup("Matrix"), &tmp);
  	}

  	if (attrib && attrib->isDict()) 
    	xform_dict->merge(attrib->getDict());

  	xform_dict->add(qstrdup("Resources"), resources);
}

void XWDVIRef::out(QIODevice * file, 
	               const char * buffer, 
	               long length)
{
	if (outputStream && (file == outputFile || file == pageFile))
		outputStream->stream.streamAdd(buffer, length);
	else
	{
		file->write(buffer, length);
		if (file == outputFile || file == pageFile)
		{
			outputFilePosition += length;
      		outputLinePosition += length;
      		if (length > 0 && (buffer[length-1] == '\n'))
        		outputLinePosition = 0;
		}
	}
}

void XWDVIRef::outChar(QIODevice *file, char c)
{
	if (outputStream && (file == outputFile || file == pageFile))
		outputStream->stream.streamAdd(&c, 1);
	else
	{
		file->putChar(c);
		if (file == outputFile || file == pageFile) 
		{
      		outputFilePosition += 1;
      		if (c == '\n')
        		outputLinePosition  = 0;
      		else
        		outputLinePosition += 1;
    	}
	}
}

void XWDVIRef::outWhite(QIODevice *file)
{
	if ((file == outputFile || file == pageFile) && outputLinePosition >= 80) 
    	outChar(file, '\n');
  	else 
    	outChar(file, ' ');
}

XWObject * XWDVIRef::readThumbNail(XWDVICore * core,
	                               const char *thumb_filename, 
	                               XWObject * image_ref)
{
	QTextCodec * codec = QTextCodec::codecForLocale();
	QString fn = codec->toUnicode(thumb_filename);
	XWPictureSea sea;
	bool is_tmp = false;
	QFile * fp = sea.openFile(fn, &is_tmp);
	if (!fp)
	{
		QString msg = QString("could not open thumbnail file \"%1\"\n").arg(fn);
		xwApp->warning(msg);
    	return 0;
	}
	
	if (!XWPNGImage::checkForPNG(fp) && !XWJPGImage::checkForJPEG(fp)) 
	{
		QString msg = QString("Thumbnail \"%1\" not a png/jpeg file!\n").arg(fn);
    	xwApp->warning(msg);
    	fp->close();
    	delete fp;
    	return 0;
  	}
  	fp->close();
    delete fp;
    
    int xobj_id = core->findImageResource(thumb_filename, 0, 0);
  	if (xobj_id < 0) 
  	{
  		QString msg = QString("could not read thumbnail file \"%s\".\n").arg(fn);
    	xwApp->warning(msg);
    	return 0;
  	} 
  	else
    	core->getImageReference(xobj_id, image_ref);

  	return image_ref;
}

void XWDVIRef::releaseObjstm(ObjectSteam *objstm)
{
	long *data = objstm->objstm_data;
  long  pos = data[0];  	
  QByteArray oldbuf = objstm->stream.streamReadAll();
  QBuffer * fp = objstm->stream.streamGetIO();
  fp->close();
  fp->open(QIODevice::ReadWrite | QIODevice::Truncate);
  long stream_length = 0;
  {
  	long i = 2*pos;
  	long *val = data+2;
  	char tmpbuf[100];
   	while (i--) 
   	{
     		long length = sprintf(tmpbuf, "%ld ", *(val++));
     		fp->write(tmpbuf, length);
     		stream_length += length;
   	}
  }
    
  XWDict * dict = objstm->stream.streamGetDict();
  XWObject obj1;
  obj1.initName("ObjStm");
  dict->add(qstrdup("Type"), &obj1);
  obj1.initInt((int)pos);
  dict->add(qstrdup("N"), &obj1);
  obj1.initInt((int)stream_length);
  dict->add(qstrdup("First"), &obj1);
  if (!oldbuf.isEmpty())
  	fp->write(oldbuf);
    
  releaseObj(&(objstm->stream));
  delete [] objstm->objstm_data;
  delete objstm;
}

void XWDVIRef::resetBox()
{
	breakingState.rect.llx = breakingState.rect.lly =  HUGE_VAL;
  	breakingState.rect.urx = breakingState.rect.ury = -HUGE_VAL;
  	breakingState.dirty    = 0;
}

void XWDVIRef::resizePageEntries(long size)
{
	if (size > MAXPAGES) 
	{
    	pages.entries = (PDFPage*)realloc(pages.entries, size * sizeof(PDFPage));
    	for (long i = pages.max_entries; i < size; i++) 
    	{
      		pages.entries[i].page_obj.initNull();
      		pages.entries[i].page_ref.initNull();
      		pages.entries[i].flags      = 0;
      		pages.entries[i].resources.initNull();
      		pages.entries[i].background.initNull();
      		pages.entries[i].contents.initNull();
      		pages.entries[i].content_refs[0].initNull();
      		pages.entries[i].content_refs[1].initNull();
      		pages.entries[i].content_refs[2].initNull();
      		pages.entries[i].content_refs[3].initNull();
      		pages.entries[i].annots.initNull();
      		pages.entries[i].beads.initNull();
      		pages.entries[i].gsLabels = 0;
      		pages.entries[i].gsCounter = 0;
    	}
    	pages.max_entries = size;
  	}
}

void XWDVIRef::warnUndefDests(XWPDFNames *dests, XWPDFNames *gotosA)
{
	if (!gotosA->setIterator())
    	return;
    	
    do 
    {
    	int keylen;
    	char *key = gotosA->getKey(&keylen);
    	if (!dests->lookupTable(key, keylen)) 
    	{
      		char *dest = new char[keylen+1];
      		memcpy(dest, key, keylen);
      		dest[keylen] = 0;
      		QString msg = QString("PDF destination \"%s\" not defined.\n").arg(dest);
      		xwApp->warning(msg);
      		delete [] dest;
    	}
  	} while (gotosA->next());
}

void XWDVIRef::writeArray(XWObject * obj, QIODevice * file)
{
	outChar(file, '[');
	XWArray * array = obj->getArray();
	int length = array->getLength();
	if (array && length > 0)
	{
		XWObject obj1;
		XWObject::ObjectType type1 = XWObject::None;
		for (int i = 0; i < length; i++)
		{
			array->getNF(i, &obj1);
			XWObject::ObjectType type2 = obj1.getType();
			if (needWhite(type1, type2))
				outWhite(file);
				
			type1 = type2;
			writeObj(&obj1, file);
			obj1.free();
			obj1.initNull();
		}
	}
	outChar(file, ']');
}

void XWDVIRef::writeBoolean(XWObject * obj, QIODevice * file)
{
	if (obj->getBool())
	{
		file->write("true", 4);
	}
	else
	{
		file->write("false", 5);
	}
}

void XWDVIRef::writeDict(XWObject * obj, QIODevice * file)
{
	out(file, "<<", 2);
	XWDict * dict = obj->getDict();
	int length = dict->getLength();
	if (dict && length > 0)
	{
		XWObject key, value;
		for (int i = 0; i < length; i++)
		{
			char * s = dict->getKey(i);
			key.initName(s);
			writeName(&key, file);			
			key.free();
			dict->getValNF(i, &value);
			if (needWhite(XWObject::Name, value.getType()))
				outWhite(file);
			writeObj(&value, file);
			value.free();
		}
	}
	
	out(file, ">>", 2);
}

void XWDVIRef::writeIndirect(XWObject * obj, QIODevice * file)
{
	char buf[1024];
	int length = sprintf(buf, "%lu %hu R", obj->label, obj->generation);
  	out(file, buf, length);
}

void XWDVIRef::writeName(XWObject * obj, QIODevice * file)
{
	char * s   = obj->getName();
  	int length = strlen(s);
  	
  	outChar(file, '/');
		
  	for (int i = 0; i < length; i++) 
  	{
    	if (s[i] < '!' || s[i] > '~' || s[i] == '#' || XWLexer::isDelim(s[i])) 
    	{
      		/*     ^ "space" is here. */
      		outChar(file, '#');
      		outXChar(file, s[i]);
    	} 
    	else 
      		outChar(file, s[i]);
  	}
}

void XWDVIRef::writeNull(QIODevice * file)
{
	out(file, "null", 4);
}

void XWDVIRef::writeNumber(XWObject * obj, QIODevice * file)
{
	double num = 0;
	if (obj->isInt())
		num = obj->getInt();
	else
		num = obj->getReal();
		
	char buf[1024];
	int length = XWDVIDev::sprintNumber(buf, num);
	out(file, buf, length);
}

void XWDVIRef::writeObj(XWObject * obj, QIODevice * file)
{
	if (!obj || obj->isNull())
	{
		writeNull(file);
		return ;
	}
	
	switch (obj->getType())
	{
		case XWObject::Bool:
			writeBoolean(obj, file);
			break;
			
		case XWObject::Int:
		case XWObject::Real:
			writeNumber(obj, file);
			break;
			
		case XWObject::String:
			writeString(obj, file);
			break;
			
		case XWObject::Name:
			writeName(obj, file);
			break;
						
		case XWObject::Array:
			writeArray(obj, file);
			break;
			
		case XWObject::Dict:
			writeDict(obj, file);
			break;
			
		case XWObject::Stream:
			writeStream(obj, file);
			break;
			
		case XWObject::Ref:
		case XWObject::Indirect:
			writeIndirect(obj, file);
			break;
			
		default:
			break;
	}
}

void XWDVIRef::writeStream(XWObject * obj, QIODevice * file)
{
	QByteArray filtered = obj->streamReadAll();
	ulong len = (ulong)filtered.length();
	uchar * buf = (uchar*)filtered.data();
	XWDict * dict = obj->streamGetDict();
	bool d = false;
	if (filtered.size() > 0 && (obj->getFlags() & STREAM_COMPRESS) && compressionLevel > 0)
	{		
		len = filtered.length() + filtered.length() / 1000 + 14;
		buf = new uchar[len + 1];
		d = true;
		compress2(buf, &len, (uchar*)filtered.data(), filtered.length(), compressionLevel);		
		XWObject filters;
		dict->lookupNF("Filter", &filters);
		if (filters.isNull())
		{
			filters.initArray(this);
			XWObject filter_name;
			filter_name.initName("FlateDecode");
			filters.arrayAdd(&filter_name);
			dict->add(qstrdup("Filter"), &filters);
		}
		else
		{
			filters.getArray()->unshift("FlateDecode");
			filters.free();
		}
	}
	
	XWObject obj1;
	obj1.initInt(len);
	dict->add(qstrdup("Length"), &obj1);
	obj1.initDict(dict);
	writeObj(&obj1, file);
	obj1.free();
	out(file, "\nstream\n", 8);
	if (encMode)
    	XWDVIEncrypt::encrypt(buf, len);
    		
    if (filtered.size() > 0) 
    	out(file, (const char*)buf, len);
    	
    out(file, "\n", 1);
  	out(file, "endstream", 9);
  	
  if (d)
  	delete [] buf;
}

void XWDVIRef::writeString(XWObject * obj, QIODevice * file)
{
	uchar * s = (uchar*)(obj->getString()->getCString());
	int length = obj->getString()->getLength();
	if (encMode)
    	XWDVIEncrypt::encrypt(s, length);
    		
    int  nescc = 0;
    for (int i = 0; i < length; i++) 
    {
    	if (!isprint(s[i]))
      		nescc++;
  	}
  	
  	if (nescc > length / 3) 
  	{
    	outChar(file, '<');
    	for (int i = 0; i < length; i++) 
      		outXChar(file, s[i]);
    	outChar(file, '>');
  	}
  	else
  	{
  		outChar(file, '(');
  		char wbuf[4096];
  		for (int i = 0; i < length; i++) 
  		{
      		int c = escapeStr(wbuf, 4096, &(s[i]), 1);
      		out(file, wbuf, c);
    	}
    	outChar(file, ')');
  	}
}

