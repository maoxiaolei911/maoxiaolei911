/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTWINDOW_H
#define XWFONTWINDOW_H

#include <QString>
#include <QStringList>
#include <QWidget>
#include <QTableWidget>
#include <QDialog>
#include <QString>
#include <QList>
#include "XWGlobal.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QSpinBox;
class QGroupBox;
class QComboBox;
class QDoubleSpinBox;

class QListWidgetItem;
class QListWidget;
class QStackedWidget;

class QDialogButtonBox;

#define FONT_FILE           0
#define CMAP_DIR            1
#define UNICODE_MAP         2
#define CID_UNICODE_MAP     3
#define UNICODE_UNICODE_MAP 4
#define NAME_UNICODE_MAP    5


class FontMapTable : public QTableWidget
{
	Q_OBJECT

public:
	FontMapTable(int typeA, QWidget * parent = 0);
	             
	void add(const QString & txt0);
	void add(const QString & txt0, const QString & txt1);
	void add(const QString & txt0, 
	         const QString & txt1,
	         const QString & txt2,
	         const QString & txt3);
		             
	void save();
	             
protected:
	void contextMenuEvent(QContextMenuEvent *event);
	
private slots:
	void deleteRow();
	
private:
	int  type;
	bool dirty;
};

class FontMapWindow : public QWidget
{
	Q_OBJECT

public:
	FontMapWindow(int typeA, QWidget * parent = 0);
	
	void save();
	
private slots:
	void add();
	
private:
	int type;
	QLabel * input0Label;
	QLineEdit * input0Edit;
	
	QLabel * input1Label;
	QLineEdit * input1Edit;
	
	QPushButton * addButton;
	
	FontMapTable * table;
};

class XW_GUI_EXPORT XWFontWindow : public QDialog
{
	Q_OBJECT

public:
	XWFontWindow(QWidget * parent = 0);
	
	void accept();
				
private slots:
	void changePage(QListWidgetItem *current, QListWidgetItem *previous);
				
private:
	QListWidget * contentsWidget;
	QStackedWidget * pagesWidget;
	
	FontMapWindow * fontFileWin;
	FontMapWindow * cmapDirWin;
	FontMapWindow * unicodeMapWin;
	FontMapWindow * cidUnicodeMapWin;
	FontMapWindow * unicodeToUnicodeWin;
	FontMapWindow * nameToUnicodeWin;
	
	QDialogButtonBox * buttonBox;
};

#endif // XWFONTWINDOW_H
