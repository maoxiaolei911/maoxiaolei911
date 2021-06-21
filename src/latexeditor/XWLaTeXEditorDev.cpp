/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QCoreApplication>
#include <QBuffer>
#include "XWApplication.h"
#include "XWFileName.h"
#include "XWLaTeXEditorDev.h"

XWLaTeXEditorDev::XWLaTeXEditorDev(QObject * parent)
	:XWTeXIODev(parent)
{
	setIniVersion(false);
}

XWLaTeXEditorDev::~XWLaTeXEditorDev()
{
}

void XWLaTeXEditorDev::compile()
{
	dvi_file = new QBuffer;
	XWTeXIODev::compile();
}
