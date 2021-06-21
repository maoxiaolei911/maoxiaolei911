/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXDOCTEMPLATEWINDOW_H
#define XWTEXDOCTEMPLATEWINDOW_H

#include <QDialog>
#include <QString>
#include "XWTeXGuiType.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QListWidget;
class QPlainTextEdit;
class QDialogButtonBox;
class XWTeXHighlighter;

class XW_TEXGUI_EXPORT XWTeXDocTemplateWindow : public QDialog
{
	Q_OBJECT

public:
	XWTeXDocTemplateWindow(QWidget * parent = 0);
	
	QString getFileName();
	
public slots:
	void accept();
	
private slots:
	void saveAs();
	void setTemplate(int row);
	
private:
	void findTemplates();
	void travers(const QString & parentDir, const QString & curDir);
	
private:
	QLabel    *  fileNameLabel;
	QLineEdit * fileNameEdit;
	
	QPushButton * browseButton;
	
	QListWidget    * templatesList;
	QPlainTextEdit * tmpbrowser;
	
	QDialogButtonBox * buttonBox;
	
	XWTeXHighlighter * highlighter;
};

#endif //XWTEXDOCTEMPLATEWINDOW_H
