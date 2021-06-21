/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "XWLogMainWindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	QString bindir = QCoreApplication::applicationDirPath();
  QString top = bindir;
  int ti = bindir.lastIndexOf(QChar('/'));
  if (ti > 0)
   	top = top.left(ti);
  
	QString apptrans = QString("%1/translations/xw_log_%2").arg(top);
	apptrans += QLocale::system().name();
	QTranslator * appTranslator = new QTranslator;
	appTranslator->load(apptrans);
	
	XWLogMainWindow * mainWin = 0;
	QStringList arguments = app.arguments(); 
	if (arguments.size() == 2)
	{
		QString fileName = arguments[1];
		mainWin = new XWLogMainWindow(fileName);
	}
	else
		mainWin = new XWLogMainWindow;
		
	mainWin->show();
	return app.exec();
}
