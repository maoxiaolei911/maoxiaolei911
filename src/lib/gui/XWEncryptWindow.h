/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWENCRYPTWINDOW_H
#define XWENCRYPTWINDOW_H

#include <QDialog>
#include <QString>
#include "XWGlobal.h"

class QLabel;
class QLineEdit;
class QDialogButtonBox;

class XW_GUI_EXPORT XWEncryptWindow : public QDialog
{
	Q_OBJECT

public:
	XWEncryptWindow(const QString & msg, QWidget * parent = 0);
	
	QString getUserName();
	QString getPassward();
	
private:
	QLabel * label;
	
	QLabel * userlabel;
	QLineEdit * userEdit;
	
	QLabel * passwardlabel;
	QLineEdit * passwardEdit;
	
	QDialogButtonBox * buttonBox;
};

#endif // XWENCRYPTWINDOW_H
