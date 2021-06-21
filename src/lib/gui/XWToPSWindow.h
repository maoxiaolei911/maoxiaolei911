/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTOPSWINDOW_H
#define XWTOPSWINDOW_H

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

class QDialogButtonBox;
class QTabWidget;

class XW_GUI_EXPORT XWToPSWindow : public QDialog
{
	Q_OBJECT

public:
	XWToPSWindow(const QString & basename, 
	             int pgnum, 
	             QWidget * parent = 0);
	~XWToPSWindow();
	
	void accept();
	bool allPages();
	
	QString getFileName();	
	
	QList<int> getPages(bool dvi);
	
private slots:
	void open();
	
	void setImageableDirty();
	void setPaperSize(int idx);
	
private:
	XWDocSetting * setting;
	bool dirty;
	
	QWidget * outWidget;
	QLabel * fileNameLabel;
	QLineEdit * fileNameEdit;
	QPushButton * browseButton;
	QRadioButton * allButton;
	QRadioButton * rangButton;
	QLineEdit * rangeEdit;
	
	QWidget * pageWidget;	
	QCheckBox * cropBox;
	QCheckBox * expandSmallerBox;
	QCheckBox * shrinkLargerBox;
	QCheckBox * centerBox;
	QCheckBox * duplexBox;
	
	QWidget * fontWidget;	
	QLabel * levelLabel;
	QComboBox * levelCombo;
	QCheckBox * embedType1Box;
	QCheckBox * embedTrueTypeBox;
	QCheckBox * embedCIDPostScriptBox;
	QCheckBox * embedCIDTrueTypeBox;
	
	QWidget * paperWidget;
	QLabel * paperSizeLabel;
	QComboBox * paperCombo;
	QLabel * paperWidthLabel;
	QDoubleSpinBox * paperWidthBox;
	QLabel * paperHeightLabel;
	QDoubleSpinBox * paperHeightBox;
	
	
	QWidget * imageableWidget;
	QLabel * imageableLLXLabel;
	QSpinBox * imageableLLXBox;
	QLabel * imageableLLYLabel;
	QSpinBox * imageableLLYBox;
	QLabel * imageableURXLabel;
	QSpinBox * imageableURXBox;
	QLabel * imageableURYLabel;
	QSpinBox * imageableURYBox;
	
	QWidget * miscWidget;
	QCheckBox * preloadBox;
	QCheckBox * opiBox;
	QCheckBox * asciiHexBox;
	
	QTabWidget * tab;
	
	QDialogButtonBox * buttonBox;
	
	QString path;
};

#endif // XWTOPSWINDOW_H
