/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDISPLAYSETTINGDIALOG_H
#define XWDISPLAYSETTINGDIALOG_H

#include <QDialog>
#include <QColor>
#include "XWGlobal.h"

class QLabel;
class QToolButton;
class QDoubleSpinBox;
class QCheckBox;
class QTabWidget;
class QDialogButtonBox;

class XW_GUI_EXPORT XWDisplaySettingDialog : public QDialog
{
	Q_OBJECT

public:
	XWDisplaySettingDialog(QWidget * parent = 0);
	
	void accept();
	
private:
	void changeColor(QToolButton * button, const QColor &c);
	
private slots:
//	void setMainBgColor();
	void setMainPaperColor();
//	void setRefBgColor();
	void setRefPaperColor();
	
private:
	QWidget * mainWidget;
//	QLabel * mainWinBgColorLabel;
//	QToolButton * mainWinBgColorButton;
	
	QLabel * mainWinOpacityLabel;	
	QDoubleSpinBox * mainWinOpacityBox;
	
	QLabel * mainPaperColorLabel;
	QToolButton * mainPaperColorButton;
	QCheckBox * mainReverseVideoBox;
	
	QWidget * refWidget;
//	QLabel * refWinBgColorLabel;
//	QToolButton * refWinBgColorButton;
	
	QLabel * refWinOpacityLabel;	
	QDoubleSpinBox * refWinOpacityBox;
	
	QLabel * refPaperColorLabel;
	QToolButton * refPaperColorButton;
	QCheckBox * refReverseVideoBox;
	
	QDialogButtonBox * buttonBox;
	
	QTabWidget * tab;
	
	QColor mainBgColor;
	QColor mainPaperColor;
	
	QColor refBgColor;
	QColor refPaperColor;
};

#endif //XWDISPLAYSETTINGDIALOG_H

