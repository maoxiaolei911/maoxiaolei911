/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QApplication>
#include "XWTeXReader.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	XWTeXReader * reader = new XWTeXReader(&app);
	reader->start(argc, argv);
	return app.exec();
}

