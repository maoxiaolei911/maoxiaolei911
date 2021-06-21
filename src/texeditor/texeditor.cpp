/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QApplication>
#include "XWTeXEditor.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	XWTeXEditor * editor = new XWTeXEditor(&app);
	editor->start(argc, argv);
	return app.exec();
}

