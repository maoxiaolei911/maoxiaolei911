/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QApplication>
#include "XWTeXFmtEditor.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	XWTeXFmtEditor * editor = new XWTeXFmtEditor(&app);
	editor->start(argc, argv);
	return app.exec();
}

