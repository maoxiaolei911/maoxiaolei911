/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QCoreApplication>
#include <QFile>
#include "XWTeXFmtEditorDev.h"

XWTeXFmtEditorDev::XWTeXFmtEditorDev(QObject * parent)
	:XWTeXIODev(parent)
{
	setIniVersion(true);
}

XWTeXFmtEditorDev::~XWTeXFmtEditorDev()
{
}
