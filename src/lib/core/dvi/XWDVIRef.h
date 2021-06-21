/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVIREF_H
#define XWDVIREF_H

#include <QString>
#include <QList>
#include <QMultiHash>
#include <QHash>
#include <QPair>
#include <QDateTime>
#include <QBuffer>

#include "XWRef.h"
#include "XWDVIType.h"

#define PDF_VERSION_MIN  3
#define PDF_VERSION_MAX  7
#define PDF_VERSION_DEFAULT 4

#define PDF_OBJ_MAX_DEPTH  30

class XWDVICore;
class XWDVIDev;
class XWPDFNames;

struct DVIRefEntry
{
	uchar  type;
	int  offset;
	int  gen;
	XWObject    direct;
	XWObject    indirect;
};

struct ObjectSteam
{
	XWObject       stream;
	long         * objstm_data;
  	unsigned long  stream_length;
  	unsigned long  max_length;
  	unsigned char  _flags;
};

struct PDFForm
{
	char       *ident;
	PDFTMatrix  matrix;
	PDFRect     cropbox;

	XWObject    resources;
	XWObject    contents;
};

struct FormListNode
{
  	int      q_depth;
  	PDFForm  form;

  	FormListNode * prev;
};

typedef struct PDFPage
{
  	XWObject page_obj;
  	XWObject page_ref;

  	int       flags;

  	double   ref_x, ref_y;
  	PDFRect  cropbox;

  	XWObject resources;

  	/* Contents */
  	XWObject background;
  	XWObject contents;

  	/* global bop, background, contents, global eop */
  	XWObject content_refs[4];

  	XWObject  annots;
  	XWObject  beads;
  	
  	int * gsLabels;
  	int gsCounter;
  	
} PDFPage;

typedef struct PDFOLItem
{
  	XWObject  dict;

  	int      is_open;

  	PDFOLItem *first;
  	PDFOLItem *parent;

  	PDFOLItem *next;
} PDFOLItem;

typedef struct PDFBead
{
  	char    *id;
  	long     page_no;
  	PDFRect rect;
} PDFBead;

typedef struct PDFArticle
{
  	char     *id;
  	XWObject info;
  	long      num_beads;
  	long      max_beads;
  	PDFBead *beads;
} PDFArticle;

struct PDFRoot
{
    XWObject dict;

    XWObject viewerpref;
    XWObject pagelabels;
    XWObject pages;
    XWObject names;
    XWObject threads;
};

struct PDFPages
{
    PDFRect mediabox;
    XWObject bop;
    XWObject eop;

    long      num_entries;
    long      max_entries;
    PDFPage * entries;
};

struct PDFOutlines
{
    PDFOLItem *first;
    PDFOLItem *current;
    int        current_depth;
};

struct PDFArticles
{
    long         num_entries;
    long         max_entries;
    PDFArticle * entries;
};

struct PDFOpt
{
    int    outline_open_depth;
    double annot_grow;
};

struct NameDict
{
	char  *category;
	XWPDFNames * data;
};

class XW_DVI_EXPORT XWDVIRef : public XWRef
{
public:
	XWDVIRef();
	XWDVIRef(QIODevice * outfileA);
	virtual ~XWDVIRef();
	
		   void addAnnot(XWDVIDev * dev,
		                 unsigned page_no, 
		                 const PDFRect *rect,
		   				 XWObject *annot_dict, 
		   				 int new_annot);
		   void addBead(const char *article_id,
                        const char *bead_id, 
                        long page_no, 
                        const PDFRect *rect);
		   void addGState(char * resname, XWObject * dict);
		   int  addNames(const char *category,
                         const void *key, 
                         int keylen, 
                         XWObject *value);
		   void addObjects(int inc) {numObjects += inc;}
		   void addPageContent(const char *buffer, unsigned length);
		   void addPageResource(const char *category,
                                const char *resource_name, 
                                XWObject *resource_ref);
                                
		   void beginAnnot(XWObject *dict);
		   void beginArticle(const char *article_id, 
		                     XWObject *article_info);
		   int  beginGrabbing(XWDVICore * core, 
		                      XWDVIDev * dev,
		                      const char *ident,
                              double ref_x, 
                              double ref_y, 
                              const PDFRect *cropbox);
		   void beginPage(XWDVIDev * dev,
		                  double scale, 
		                  double x_origin, 
		                  double y_origin);
		   void bookmarksAdd(XWObject *dict, int is_open);
		   int  bookmarksDepth() {return outlines.current_depth;}
		   int  bookmarksDown();
		   int  bookmarksUp();
		   void breakAnnot(XWDVIDev * dev);
		   
		   void clear();
		   void close();
		   long       currentPageNumber();
		   XWObject * currentPageResources();
	
		   void enableManualThumbNails() {manualThumbEnabled = 1;}
		   void endAnnot(XWDVIDev * dev);
		   void endGrabbing(XWDVIDev * dev, XWObject *attrib);
		   void endPage(XWDVICore * core, XWDVIDev * dev);
	static int  escapeStr(char *buffer, 
	                      int bufsize, 
	                      const uchar *s, 
	                      int len);
		   void expandBox(const PDFRect *rect);
	                     
	virtual XWObject *fetch(int num, int gen, XWObject *obj, int recursion = 0);
	
	XWObject * getCatalog(XWObject *obj);
	QIODevice    * getCurrentPageFile();
	XWObject * getDictionary(const char *category);
	XWObject * getDocInfo(XWObject * obj);
	XWObject * getDocInfoNF(XWObject * obj);
	void     getMediaBox(unsigned page_no, PDFRect *mediabox);
	XWObject * getCatalog() {return getDictionary("Catalog");}
	XWObject * getDocInfo() {return getDictionary("Info");}
	XWObject * getNames() {return getDictionary("Names");}
	int   getNumObjects() { return numObjects + 1; }
	QIODevice * getOutputFile() {return outputFile;}
	XWObject * getPageDict(unsigned long page_no);
	XWObject * getPageTree() {return getDictionary("Pages");}
	XWObject * getReference(const char *category, XWObject * ref);
	bool  getStreamEnd(uint streamStart, uint *streamEnd);
	XWObject * getThisPage() {return getDictionary("@THISPAGE");}
	XWObject * getTrailerDict() {return &trailerDict;}
	unsigned getVersion() {return pdfVersion;}
	
	bool isBatch() {return !deleteFile;}
	bool isOk() { return true; }
#ifdef XW_BUILD_PS_LIB
  bool isScanning() {return scanning;}
#else
	bool isScanning() {return pageFile == 0;}
#endif //XW_BUILD_PS_LIB
	
	void labelObj(XWObject * obj);
	
	XWObject * nextPageRef(XWObject * ref) {return getReference("@NEXTPAGE", ref);}
	
	virtual void open();
	virtual void outFlush();
		              
	XWObject * prevPageRef(XWObject * ref) {return getReference("@PREVPAGE", ref);}
	
	void refObj(XWObject * obj, XWObject * ref);
	XWObject * refPage(unsigned long page_no, XWObject * ref);
	virtual void releaseObj(XWObject *object);
	
	void setAnnotGrow(double a);
	void setBGColor(const XWDVIColor * color);
	void setBookMarkOpen(int i);
	void setBopContent(const char *content, unsigned length);
	void setCheckGotos(bool e);
	void setCompressionLevel(int level);
	void setCreator(const char *doccreator);
	void setCurrentPageNumber(long page_no);
	void setEncryption(int do_encryption);
	void setEopContent(const char *content, unsigned length);
	void setMediaBox(double media_width, double media_height);
	void setMediaBox(unsigned page_no, const PDFRect *mediabox);
	void setThumbBaseName(const char * n);
	void setVersion(char version);
	
	XWObject * thisPageRef(XWObject * ref) {return getReference("@THISPAGE", ref);}
	
protected:
	void addGoto(XWObject *annot_dict);
	long addObjStm(ObjectSteam *objstm, XWObject *object);
	void addXRefEntry(int label, 
	                  uchar type, 
	                  int field2, 
	                  int field3);
	long asnDate(char *date_string);
	
	XWObject * buildPageTree(PDFPage *firstpage, 
	                         long num_pages,
                             XWObject *parent_ref,
                             XWObject * self);
	                  
	void cleanArticle(PDFArticle *article);
	void cleanBookMarks(PDFOLItem *item);
	void closeArticles();
	void closeBookMarks();
	void closeCatalog();
	void closeDocInfo();
	void closeNames();
	void closePageTree();
	
	virtual void dumpTrailerDict();
	virtual void dumpXRefStream();
	virtual void dumpXRefTable();
	
	void      fillPageBackground(XWDVIDev * dev);
	PDFBead * findBead(PDFArticle *article, 
	                   const char *bead_id);
	void  finishPage(XWDVICore * core);
	int   flushBookMarks(PDFOLItem *node,
                    	 XWObject *parent_ref, 
                 		 XWObject *parent_dict);
	virtual void flushObj(XWObject * obj, QIODevice * file);
	void    flushPage(PDFPage *page, XWObject *parent_ref);
	PDFPage  *  getPageEntry(long page_no);
	XWObject *  getPageResources(const char *category);	
	
	virtual void init();
	virtual void initBookmarks();
	virtual void initCatalog();
	virtual void initDocInfo();
	virtual void initNames();
	
	XWObject * makeArticle(PDFArticle *article,
              			   const char **bead_ids, 
              			   int num_beads,
                           XWObject *article_info,
                           XWObject *art_dict);
	void       makeXForm(XWObject     *xform,
                         PDFRect    *bbox,
                         PDFTMatrix *matrix,
                         XWObject     *resources,
                         XWObject     *attrib);
	
	int  needWhite(XWObject::ObjectType type1, XWObject::ObjectType type2);
	
	virtual void out(QIODevice * file, 
	                 const char * buffer, 
	                 long length);
	virtual void outChar(QIODevice *file, char c);
	void    outWhite(QIODevice *file);
	void    outXChar(QIODevice * file, uchar c);
		
	XWObject * readThumbNail(XWDVICore * core,
	                         const char *thumb_filename, 
	                         XWObject * image_ref);
	void releaseObjstm(ObjectSteam *objstm);
	void resetBox();
	void resizePageEntries(long size);
	
	void warnUndefDests(XWPDFNames *dests, XWPDFNames *gotosA);
	void writeArray(XWObject * obj, QIODevice * file);
	void writeBoolean(XWObject * obj, QIODevice * file);
	void writeDict(XWObject * obj, QIODevice * file);
	void writeIndirect(XWObject * obj, QIODevice * file);
	void writeName(XWObject * obj, QIODevice * file);
	void writeNull(QIODevice * file);
	void writeNumber(XWObject * obj, QIODevice * file);
	void writeObj(XWObject * obj, QIODevice * file);
	void writeStream(XWObject * obj, QIODevice * file);
	void writeString(XWObject * obj, QIODevice * file);
	
protected:
	QIODevice * outputFile;
	bool deleteFile;
	QBuffer * pageFile;
	QIODevice * curFile;
	int nextLabel;
	int usedLabel;
	
	int compressionLevel;
	char pdfVersion;
	
	XWObject xrefStream;
	ObjectSteam * currentObjStm;
	ObjectSteam * outputStream;
	ulong startXRef;
	int  doObjStm;
	
	DVIRefEntry * outputXRef;
	ulong         maxIndObjects;
	
	XWObject docInfo;
	XWObject trailerDict;
	int infoNum;
	int infoGen;
	int numObjects;
	
	int outputFilePosition;
	int outputLinePosition;
	
	int  encMode;
  	int  docEncMode;
	
	PDFRoot root;
	PDFOpt  opt;
	PDFOutlines outlines;
	PDFArticles articles;
	int rootNum;
	int rootGen;
	
	NameDict * names;
	int checkGotos;
	XWPDFNames * gotos;
	
	PDFPages pages;
  	
  	XWDVIColor bgcolor;
  	
  	int    manualThumbEnabled;
  	char * thumbBaseName;
  	
  	FormListNode * pendingForms;
  	
  	struct
	{
  		int      dirty;
  		int      broken;
  		XWObject annot_dict;
  		PDFRect rect;
	} breakingState;
	
	int    streamEndsLen;
	int    streamEndsSize;
	uint * streamEnds;
	
	bool debugMode;
	
#ifdef XW_BUILD_PS_LIB
  bool scanning;
#endif //XW_BUILD_PS_LIB
	
	int      q_depth;
	
	QMultiHash<long, QPair<int, int> > fileHash;
	QMultiHash<long, QPair<int, int> > pageHash;
	QHash<int, int> pageLabels;
};

inline int XWDVIRef::needWhite(XWObject::ObjectType type1, XWObject::ObjectType type2)
{
	return !(type1 == XWObject::String || type1 == XWObject::Array || 
		     type1 == XWObject::Dict || type2 == XWObject::String || 
		     type2 == XWObject::Name || type2 == XWObject::Array || 
		     type2 == XWObject::Dict);
}

inline void XWDVIRef::outXChar(QIODevice * file, uchar c)
{
	int __tmpnum = ((c) >> 4) & 0x0f;
	outChar((file), (((__tmpnum) >= 10) ? (__tmpnum) + 'W' : (__tmpnum)+'0'));
	__tmpnum = (c) & 0x0f;
	outChar((file), (((__tmpnum) >= 10) ? (__tmpnum)+'W' : (__tmpnum)+'0'));
}

#endif //XWDVIREF_H

