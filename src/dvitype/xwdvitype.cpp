/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
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

