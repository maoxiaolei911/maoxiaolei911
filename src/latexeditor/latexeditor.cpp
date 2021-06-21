/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QApplication>
#include "XWLaTeXEditor.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	XWLaTeXEditor * editor = new XWLaTeXEditor(&app);
	editor->start(argc, argv);
	return app.exec();
}
