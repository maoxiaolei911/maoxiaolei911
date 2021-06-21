/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTOIMAGEWINDOW_H
#define XWTOIMAGEWINDOW_H

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
class QToolButton;

class QDialogButtonBox;

class XW_GUI_EXPORT XWToImageWindow : public QDialog
{
	Q_OBJECT

public:
	XWToImageWindow(const QString & basename, 
	                int pgnum, 
	                QWidget * parent = 0);	                
	XWToImageWindow(const QString & basename, 
	                QWidget * parent = 0);
	bool  allPages();
	
	QColor  getBgColor(); 
	QString getFileName();		
	QString getFormat();
	QList<int> getPages(bool dvi);
	bool    getReverseVideo();
	
private:	
	void changeColor(const QColor &c);
	void createFmtCombo();
	void createPath();
	
private slots:
	void open();
	
	void setBgColor();
			
private:
	QLabel * fileNameLabel;
	QLineEdit * fileNameEdit;
	QPushButton * browseButton;
	
	QRadioButton * allButton;
	QRadioButton * rangButton;
	QLineEdit * rangeEdit;
		
	QLabel * fmtLabel;
	QComboBox * fmtCombo;
		
	QLabel * bgLabel;
	QToolButton * bgButton;
	QCheckBox * reverseVideoBox;
	
	QDialogButtonBox * buttonBox;
	
	QString path;
	
	QColor bgColor;
};

#endif // XWTOIMAGEWINDOW_H
