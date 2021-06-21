/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWBASENAMEWINDOW_H
#define XWBASENAMEWINDOW_H

#include <QDialog>
#include <QString>

#include "XWGlobal.h"

class QLabel;
class QLineEdit;
class QPushButton;

class XW_GUI_EXPORT XWBaseNameWindow : public QDialog
{
	Q_OBJECT

public:
	XWBaseNameWindow(const QString & title, 
	                 QWidget * parent = 0);	
	                 
	QString getBaseName();
	                
private slots:
	void open();
			
private:
	QLabel * fileNameLabel;
	QLineEdit * fileNameEdit;
	QPushButton * browseButton;
};

#endif // XWBASENAMEWINDOW_H
