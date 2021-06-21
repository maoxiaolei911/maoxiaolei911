/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXREADERDEV_H
#define XWTEXREADERDEV_H

#include "XWTeXIODev.h"

class XWTeXReaderDev : public XWTeXIODev
{
	Q_OBJECT
	
public:
	XWTeXReaderDev(QObject * parent = 0);
	~XWTeXReaderDev();
	
	void compile();
};

#endif //XWTEXREADERDEV_H

