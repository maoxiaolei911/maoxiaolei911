/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWBMPIMAGE_H
#define XWBMPIMAGE_H

#include "XWDVIImage.h"

class XW_DVI_EXPORT XWBMPImage : public XWDVIImage
{
public:
	XWBMPImage(XWDVIRef * xref,
	           const char *identA, 
	           int  subtypeA,
	           int  formatA,
	           long page_noA, 	           
	           const QString & filenameA,
	           XWObject *dictA);
	           
	static int checkForBMP(QIODevice *fp);
	
	int load(XWDVIRef * xref, QIODevice *fp);
	
private:
	long readRasterRLE4(uchar *data_ptr, 
		  				long width, 
		  				long height, 
		  				QIODevice *fp);
	long readRasterRLE8(uchar *data_ptr,
		                long width, 
		                long height, 
		                QIODevice *fp);
};

#endif //XWBMPIMAGE_H

