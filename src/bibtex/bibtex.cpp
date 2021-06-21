/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QApplication>
#include "XWBibTeX.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	XWBibTeX bibtex(&app);
	bibtex.start(argc, argv);
	return 0;
}
