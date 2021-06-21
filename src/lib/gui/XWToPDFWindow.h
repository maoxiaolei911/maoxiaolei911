/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTOPDFWINDOW_H
#define XWTOPDFWINDOW_H

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

class XW_GUI_EXPORT XWToPDFWindow : public QDialog
{
	Q_OBJECT

public:
	XWToPDFWindow(const QString & basename, 
	              int pgnum, 
	              QWidget * parent = 0);
	~XWToPDFWindow();
	
	void accept();
	
	bool  allPages();
	bool  enableThumbnail();
	
	QString getFileName();	
	QString getOwnerPassward();
	QList<int> getPages();
	QString getThumbBase();
	QString getUserPassward();
	
private slots:
	void open();
	
	void setPaperDirty();
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
	QCheckBox * thumbBox;
	QLabel * thumbLabel;
	QLineEdit * thumbEdit;
	
	QWidget * pdfWidget;	
	QLabel * versionLabel;
	QSpinBox * versionBox;
	QLabel * compressLevelLabel;
	QSpinBox * compressLevelBox;
	QLabel * precisionLabel;
	QSpinBox * precisionBox;
	QCheckBox * encryptBox;
	QLabel    * owpwdLabel;
	QLineEdit * owpwdEdit;
	QLabel    * uspwdLabel;
	QLineEdit * uspwdEdit;
	
	QLabel * keyBitsLabel;
	QSpinBox * keyBitsBox;
	QLabel * permissionLabel;
	QSpinBox * permissionBox;
	
	QWidget * paperWidget;
	QLabel * paperSizeLabel;
	QComboBox * paperCombo;
	QLabel * paperWidthLabel;
	QDoubleSpinBox * paperWidthBox;
	QLabel * paperHeightLabel;
	QDoubleSpinBox * paperHeightBox;
	QLabel * hoffLabel;
	QDoubleSpinBox * hoffBox;
	QLabel * voffLabel;
	QDoubleSpinBox * voffBox;
	
	QWidget * miscWidget;
	QLabel * magLabel;
	QDoubleSpinBox * magBox;
	
	QLabel * annotGrowLabel;
	QDoubleSpinBox * annotGrowBox;
	
	QCheckBox * ignoreColorsBox;
	QCheckBox * CIDFontFixedPitchBox;
	QCheckBox * noDestRemoveBox;
	QCheckBox * tpicTransparentBox;
	
	QTabWidget * tab;
		
	QDialogButtonBox * buttonBox;
	
	QString path;
};

#endif // XWTOPDFWINDOW_H
