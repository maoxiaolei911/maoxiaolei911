/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
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
