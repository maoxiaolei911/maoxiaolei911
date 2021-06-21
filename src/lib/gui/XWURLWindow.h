/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWURLWINDOW_H
#define XWURLWINDOW_H

#include <QDialog>
#include <QString>
#include "XWGlobal.h"

class QLabel;
class QLineEdit;
class QCheckBox;
class QDialogButtonBox;

class XW_GUI_EXPORT XWURLWindow : public QDialog
{
	Q_OBJECT

public:
	XWURLWindow(QWidget * parent = 0);
	
	QString getLocalName();
	QString getURL();
	
	bool saveToLocal();
	
private slots:
	void open();
	
private:
	QLabel * label;
	QLabel * openlabel;
	QLineEdit * urlEdit;
	
	QCheckBox * localBox;
	QLineEdit * localEdit;
	
	QDialogButtonBox * buttonBox;
};

#endif // XWURLWINDOW_H
