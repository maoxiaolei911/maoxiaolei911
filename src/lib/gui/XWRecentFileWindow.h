/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWRECENTFILEWINDOW_H
#define XWRECENTFILEWINDOW_H

#include <QDialog>
#include <QString>
#include "XWGlobal.h"

class QListWidget;
class QDialogButtonBox;

class XW_GUI_EXPORT XWRecentFileWindow : public QDialog
{
	Q_OBJECT

public:
	XWRecentFileWindow(QWidget * parent = 0);
	
	QString getFile(int * pg);
	
private:
	QListWidget * filesWidget;
	QDialogButtonBox * buttonBox;
};

#endif // XWRECENTFILEWINDOW_H
