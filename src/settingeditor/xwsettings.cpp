/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include <QLocale>
#include <QCoreApplication>
#include <QTranslator>
#include "XWSettingEditor.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QString bindir = QCoreApplication::applicationDirPath();
  QString top = bindir;
  int ti = bindir.lastIndexOf(QChar('/'));
  if (ti > 0)
   	top = top.left(ti);

	QString apptrans = QString("%1/translations/xwsettings_").arg(top);
	apptrans += QLocale::system().name();
	QTranslator * appTranslator = new QTranslator;
	appTranslator->load(apptrans);
	qApp->installTranslator(appTranslator);

	XWSettingDialog dlg;
	dlg.show();

  return app.exec();
}
