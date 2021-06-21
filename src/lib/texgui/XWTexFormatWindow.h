/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXFORMATWINDOW_H
#define XWTEXFORMATWINDOW_H

#include <QDialog>
#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QSpinBox>
#include <QLineEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QLabel>
#include <QComboBox>

#include "XWTeXGuiType.h"
#include "XWTexSea.h"

class QListWidget;
class QListWidgetItem;
class QDialogButtonBox;
class XWTeXFormatSetting;
class XWTeXFormatWidget;

class XW_TEXGUI_EXPORT XWTexFormatWindow : public QDialog
{
	Q_OBJECT
	
public:       
    XWTexFormatWindow(QWidget * parent = 0);
    
    void accept();
    
    QString getFmt();
    
    void setCurrentFmt(const QString & fmt);

private slots:
	void add();
	
	void setCurrentFormat(QListWidgetItem * current, QListWidgetItem * previous);
	    
private:
	QLabel    * nameLabel;
	QLineEdit * nameEdit;
	
	QPushButton * addButton;
	
	QListWidget * fmts;
	
	XWTeXFormatWidget * params;
	
	QDialogButtonBox * buttonBox;
};

class XWTeXFormatDelegate : public QItemDelegate
{
	Q_OBJECT
	
public:
	XWTeXFormatDelegate(QObject *parent = 0);
	
	QWidget *createEditor(QWidget *parent, 
	                      const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, 
	                  QAbstractItemModel *model,
                      const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, 
                              const QModelIndex &index) const;
};

class XWTeXFormatWidget : public QTreeWidget
{
	Q_OBJECT
	
public:
	XWTeXFormatWidget(QWidget *parent = 0);
	
	QStringList getAllFmts();
	
public slots:
	void save();
	void setCurrentFormat(const QString & fmt);
	void setDirty(QTreeWidgetItem * item, int column);
	
private:
	void createItem(const QString & title, 
	                int min,
	                int max);
	void createItem(const QString & title);
	
	void saveItem(int i);
	void saveItem(int i, const QString & key);
	void setDefault();
	void setup();
	void setValue(int i, int v);
	void setValue(int i, const QString & v);
	
private:
	XWTeXFormatSetting setting;
	QStringList        allFmts;
	QString            current;
	bool               dirty;
};

#endif // XWTEXFORMATWINDOW_H

