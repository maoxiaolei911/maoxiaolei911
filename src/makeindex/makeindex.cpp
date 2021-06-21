/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QApplication>
#include "XWMakeIndex.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	XWMakeIndex makeindex(&app);
	makeindex.start(argc, argv);
	return 0;
}
