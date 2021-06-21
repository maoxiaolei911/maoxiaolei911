/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWEPDFIMAGE_H
#define XWEPDFIMAGE_H

#include "XWDVIImage.h"

class XWEPDFImage : public XWDVIImage
{
public:
	XWEPDFImage(XWDVIRef * xref,
	            const char *identA, 
	            int  subtypeA,
	            int  formatA,
	            long page_noA, 	           
	            const QString & filenameA,
	            XWObject *dictA);
	                   
	int load(XWDVIRef * xref, QIODevice *fp);
};


#endif //XWEPDFIMAGE_H
