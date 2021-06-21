/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWEXTERNALTOOLWINDOW_H
#define XWEXTERNALTOOLWINDOW_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QHash>

#include "XWGlobal.h"

class XWDocSetting;
class XWPictureSea;
class QLabel;
class QGroupBox;
class QLineEdit;
class QComboBox;
class QPushButton;
class QTabWidget;
class QDialogButtonBox;

class XW_GUI_EXPORT XWExternalToolWindow : public QDialog
{
	Q_OBJECT

public:
	XWExternalToolWindow(QWidget * parent = 0);
	~XWExternalToolWindow();
	
	void accept();
		
private slots:
	void setImage(int idx);
	void setImageConvert();
	
private:
	XWDocSetting * setting;
	XWPictureSea * sea;
	
	QWidget * muWidget;
	QLabel * mulabel;	
	QLabel * movieLabel;
	QLineEdit * movieEdit;
	
	QLabel * urlLabel;
	QLineEdit * urlEdit;
	
	QWidget * imageWidget;
	QLabel * imageLabel;
	QLabel * fmtLabel;
	QComboBox * fmtCombo;
	QLabel * imageCmdLabel;
	QLineEdit * cmdEdit;
	QLabel * infileLabel;
	QLineEdit * infileEdit;
	QLabel * outfileLabel;
	QLineEdit * outfileEdit;
	QLabel * outextLabel;
	QLineEdit * outextEdit;
	
	QTabWidget * tab;
		
	QDialogButtonBox * buttonBox;
	
	QString path;
	
	QHash<int, QStringList> hash;
};

#endif // XWEXTERNALTOOLWINDOW_H
