/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
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
