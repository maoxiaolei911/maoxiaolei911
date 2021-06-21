/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVIIMAGE_H
#define XWDVIIMAGE_H

#include <QString>
#include <QList>
#include <QHash>
#include <QByteArray>
#include <QIODevice>
#include <QFile>
#include <QBuffer>

#include "XWObject.h"
#include "XWDVIType.h"


#define PDF_XOBJECT_TYPE_FORM  0
#define PDF_XOBJECT_TYPE_IMAGE 1


#define IMAGE_TYPE_UNKNOWN -1
#define IMAGE_TYPE_PDF      0
#define IMAGE_TYPE_JPEG     1
#define IMAGE_TYPE_PNG      2
#define IMAGE_TYPE_MPS      4
#define IMAGE_TYPE_EPS      5
#define IMAGE_TYPE_BMP      6

/*
#define IMAGE_TYPE_GIF      7
#define IMAGE_TYPE_PBM      8
#define IMAGE_TYPE_PGM      9
#define IMAGE_TYPE_PPM      10
#define IMAGE_TYPE_TIFF     11
#define IMAGE_TYPE_XBM      12
#define IMAGE_TYPE_XPM      13
#define IMAGE_TYPE_MNG      14
*/

class XWDVIRef;
class XWDVICore;
class XWDVIDev;

class XW_DVI_EXPORT XWDVIImageInfo
{
public:
	XWDVIImageInfo();
	
public:
	int  flags;
  	long width;
  	long height;
  	int  bits_per_component;
  	int  num_components;
  	long min_dpi;
  	double xdensity;
  	double ydensity;
};

class XWDVIXFormInfo
{
public:
	XWDVIXFormInfo();
	
public:
	int         flags;
  
  	PDFRect    bbox;
  	PDFTMatrix matrix;
};

class XW_DVI_EXPORT XWDVIImage
{
public:
	friend class XWDVIImageCache;
	
	XWDVIImage(XWDVIRef * xref,
	           const char *identA, 
	           int  subtypeA,
	           int  formatA,
	           long page_noA, 	           
	           const QString & filenameA,
	           XWObject *dictA);
	virtual ~XWDVIImage();
	
	void clean(bool finish);
	
	static void doMPSPage(XWDVICore * core,
	                      XWDVIRef * xref, 
	                      XWDVIDev * dev,
	                      QIODevice *image_file);
	
	int        getPage() {return page_no;}
	XWObject * getReference(XWDVIRef * xref, XWObject * obj);
	char     * getResName() {return res_name;}
	int        getSubtype() {return subtype;}
	
	static XWDVIImage * loadImage(XWDVICore * core,
	                              XWDVIRef * xref, 
	                              XWDVIDev * dev,
	                              int id,
	                              const char *identA, 
	                              long page_noA, 
	                              XWObject *dictA);
	                              
	int  scale(PDFTMatrix    *M,
               PDFRect       *r,
               TransformInfo *p);
	void setAttr(long width, 
	             long height, 
	             double xdensity, 
	             double ydensity, 
	             double llx, 
	             double lly, 
	             double urx, 
	             double ury);
	void setForm(XWDVIRef * xref, 
	             void *form_info, 
	             XWObject *resourceA);
	void setImage(XWDVIRef * xref, 
	              void *image_info, 
	              XWObject *resourceA);
	              
	void setPage(long page_noA, long page_countA);
	
protected:	
	void scaleToFitF(PDFTMatrix *T, TransformInfo *p);
	void scaleToFitI(PDFTMatrix *T, TransformInfo *p);
	
	static int sourceImageType(QIODevice *fp);

protected:
	char  *  ident;  	
	int      subtype;
  	int      format;
  	long     page_no;  	
  	QString  filename; 

	long  page_count;
	char  res_name[16];
	
  	struct Attr
  	{
  		long     width, height;
  		double   xdensity, ydensity;
  		PDFRect  bbox;
  	};
  	
  	Attr attr;
  	
  	XWObject  reference;
  	XWObject  resource;
  	XWObject  attr_dict;
};

class XWDVIImageCache : public QObject
{
	Q_OBJECT
	
public:
	XWDVIImageCache(QObject * parent = 0);
	~XWDVIImageCache();
	
	void close(bool finished = true);
	
	int  defineResource(XWDVIRef * xref, 
	                    const char *ident,
			            int subtype, 
			            void *info, 
			            XWObject *resource);
	
	int findResource(XWDVICore * core,
	                 XWDVIRef * xref, 
	                 XWDVIDev * dev, 
	                 const char *identA, 
	                 long page_noA, 
	                 XWObject *dictA);
	                 
	
	XWObject * getReference(XWDVIRef * xref, int id, int page_no, XWObject * obj);
	char     * getResName(int id);
	int        getSubtype(int id);
	
	void load(XWDVICore * core,
	          XWDVIRef * xref, 
	          long page_no);
	                 
	int scaleImage(int            id,
                   PDFTMatrix    *M,
                   PDFRect       *r,
                   TransformInfo *p);
	void setAttr(int id, 
	             long width, 
	             long height, 
	             double xdensity, 
	             double ydensity, 
	             double llx, 
	             double lly, 
	             double urx, 
	             double ury);
	
private:
	int  count;
	int  capacity;
  	XWDVIImage **ximages;
  	
  	struct PageImages
  	{
  		QList<int> ids;
  	};
  	
  	QHash<long, PageImages*> pageImages;
};

#endif //XWDVIIMAGE_H
