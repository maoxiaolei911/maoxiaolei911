/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWJPGIMAGE_H
#define XWJPGIMAGE_H

#include "XWDVIImage.h"

class XW_DVI_EXPORT XWJPGImage : public XWDVIImage
{
public:
	XWJPGImage(XWDVIRef * xref,
	           const char *identA, 
	           int  subtypeA,
	           int  formatA,
	           long page_noA, 	           
	           const QString & filenameA,
	           XWObject *dictA);
	           
	static int checkForJPEG(QIODevice *fp);
	
	static int getBBox(QIODevice *fp, 
	                   long *width, 
	                   long *height,
	                   double *xdensity, 
	                   double *ydensity);
	                   
	int load(XWDVICore * core, XWDVIRef * xref, QIODevice *fp);
};


#endif //XWJPGIMAGE_H
