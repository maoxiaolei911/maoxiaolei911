/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QCoreApplication>
#include <QBuffer>
#include "XWApplication.h"
#include "XWFileName.h"
#include "XWTeXReaderDev.h"

XWTeXReaderDev::XWTeXReaderDev(QObject * parent)
	:XWTeXIODev(parent)
{
	setIniVersion(false);
}

XWTeXReaderDev::~XWTeXReaderDev()
{
}

void XWTeXReaderDev::compile()
{
	dvi_file = new QBuffer;
	XWTeXIODev::compile();
}
