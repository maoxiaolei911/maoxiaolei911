/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
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
