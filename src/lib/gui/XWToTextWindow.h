/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTOTEXTWINDOW_H
#define XWTOTEXTWINDOW_H

#include <QDialog>
#include <QString>
#include <QList>
#include "XWGlobal.h"

class XWDocSetting;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QSpinBox;
class QGroupBox;
class QComboBox;
class QDoubleSpinBox;
class QTabWidget;

class QDialogButtonBox;

class XW_GUI_EXPORT XWToTextWindow : public QDialog
{
	Q_OBJECT

public:
	XWToTextWindow(const QString & basename, 
	               int pgnum, 
	               QWidget * parent = 0);
	~XWToTextWindow();
	
	void  accept();
	bool  allPages();
	
	QString getFileName();		
	QList<int> getPages(bool dvi);
	
private slots:
	void open();
		
private:
	XWDocSetting * setting;
	
	QWidget * outWidget;
	QLabel * fileNameLabel;
	QLineEdit * fileNameEdit;
	QPushButton * browseButton;
	QRadioButton * allButton;
	QRadioButton * rangButton;
	QLineEdit * rangeEdit;
	
	QWidget * miscWidget;
	QLabel * encodingLabel;
	QLineEdit * encodingEdit;
	QLabel * eolLabel;
	QComboBox * eolCombo;
	QCheckBox * pageBreaksBox;
	QCheckBox * keepTinyCharsBox;
	
	QTabWidget * tab;
	
	QDialogButtonBox * buttonBox;
	
	QString path;
};

#endif // XWTOTEXTWINDOW_H
