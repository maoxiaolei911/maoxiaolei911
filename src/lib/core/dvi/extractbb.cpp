/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QApplication>
#include "XWImageBoundingBox.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	XWImageBoundingBox * bbx = new XWImageBoundingBox(&app);
	bbx->start(argc, argv);
	return app.exec();
}
