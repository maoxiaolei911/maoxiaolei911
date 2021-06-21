/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include <QLocale>
#include <QCoreApplication>
#include <QTranslator>
#include "XWTrayDialog.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	QString bindir = QCoreApplication::applicationDirPath();
  QString top = bindir;
  int ti = bindir.lastIndexOf(QChar('/'));
  if (ti > 0)
   	top = top.left(ti);
  
	QString apptrans = QString("%1/translations/xwtray_").arg(top);
	apptrans += QLocale::system().name();
	QTranslator * appTranslator = new QTranslator;
	appTranslator->load(apptrans);
	qApp->installTranslator(appTranslator);

	QApplication::setQuitOnLastWindowClosed(false);
  XWTrayDialog window;
  window.show();
  return app.exec();	
}
