/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWPASSWARDWINDOW_H
#define XWPASSWARDWINDOW_H

#include <QDialog>
#include <QString>
#include "XWGlobal.h"

class QLabel;
class QLineEdit;
class QDialogButtonBox;

class XW_GUI_EXPORT XWPasswardWindow : public QDialog
{
	Q_OBJECT

public:
	XWPasswardWindow(const QString & msg, 
	                 const QString & labelA,
	                 QWidget * parent = 0);
	
	
	void accept();
	
	QString getPassward();
	
private:
	QLabel * label;
	QLabel * pwdlabel;
	QLineEdit * pwdEdit;
	
	QLabel * relabel;
	QLineEdit * reEdit;
	
	QDialogButtonBox * buttonBox;
};

#endif // XWPASSWARDWINDOW_H
