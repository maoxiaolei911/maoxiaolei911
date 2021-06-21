/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QCoreApplication>
#include <QBuffer>
#include "XWApplication.h"
#include "XWFileName.h"
#include "XWTeXEditorDev.h"

XWTeXEditorDev::XWTeXEditorDev(QObject * parent)
	:XWTeXIODev(parent)
{
	setIniVersion(false);
}

XWTeXEditorDev::~XWTeXEditorDev()
{
}

void XWTeXEditorDev::compile()
{
	dvi_file = new QBuffer;
	XWTeXIODev::compile();
}
