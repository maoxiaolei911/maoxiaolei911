/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFTPWINDOW_H
#define XWFTPWINDOW_H

#include <QDialog>
#include <QUrl>
#include <QString>
#include "XWGlobal.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QCheckBox;

class QDialogButtonBox;

class XW_GUI_EXPORT XWFTPWindow : public QDialog
{
	Q_OBJECT

public:
	XWFTPWindow(QWidget * parent = 0);
	
	QString getLocalName();
	QUrl    getURL();
	
	bool saveToLocal();
	
private slots:
	void open();
	
private:
	QLabel * label;
	QLabel * schemelabel;
	QLineEdit * schemeEdit;
	QLabel * userlabel;
	QLineEdit * userEdit;
	QLabel * passwdlabel;
	QLineEdit * passwdEdit;
	QLabel * hostlabel;
	QLineEdit * hostEdit;
	QLabel * portlabel;
	QSpinBox * portBox;
	QLabel * pathlabel;
	QLineEdit * pathEdit;
	
	QCheckBox * localBox;
	QLineEdit * localEdit;
	
	QDialogButtonBox * buttonBox;
};

#endif // XWFTPWINDOW_H
