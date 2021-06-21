/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QApplication>
#include "XWReader.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	XWReader * reader = new XWReader(&app);
	reader->start(argc, argv);
	return app.exec();
}

