/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QApplication>
#include "XWDVIShow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	XWDVIShow * shower = new XWDVIShow(&app);
	shower->start(argc, argv);
	return app.exec();
}

