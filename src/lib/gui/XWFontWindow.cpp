/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWFontSetting.h"
#include "XWFontWindow.h"

FontMapTable::FontMapTable(int typeA, QWidget * parent)
	:QTableWidget(parent)
{
	type = typeA;
	dirty = false;	
	QStringList header;
	XWFontSetting setting;
	QString key;
	if (type == FONT_FILE)
	{
		QStringList list = setting.getFonts();
		setColumnCount(2);	
		header << tr("Font name") << tr("Font file");
		setHorizontalHeaderLabels(header);
		setRowCount(list.size());
		for (int i = 0; i < list.size(); i++)
		{
			QString fontname = list[i];
			QString fontfile = setting.getFontFile(fontname);
			QTableWidgetItem * item0 = new QTableWidgetItem(fontname);
			setItem(i, 0, item0);
			QTableWidgetItem * item1 = new QTableWidgetItem(fontfile);
			setItem(i, 1, item1);
		}
		
		resizeColumnToContents(0);
		resizeColumnToContents(1);
	}
	else if (typeA == NAME_UNICODE_MAP)
	{
		header << tr("Map file");	
		setColumnCount(1);
		setHorizontalHeaderLabels(header);
		QStringList list = setting.getNameToUnicode();
		setRowCount(list.size());
		for (int i = 0; i < list.size(); i++)
		{
			QTableWidgetItem * item0 = new QTableWidgetItem(list.at(i));
			setItem(i, 0, item0);
		}
		
		resizeColumnToContents(0);
	}
	else
	{
		setColumnCount(2);
		QStringList list;
		switch (typeA)
		{				
			case CMAP_DIR:
				header << tr("Registry-Ordering") << tr("Search directory");
				break;
				
			case UNICODE_MAP:
				header << tr("Encoding name") << tr("Map file");
				break;
			
			case CID_UNICODE_MAP:
				header << tr("Registry-Ordering") << tr("Map file");
				break;
				
			case UNICODE_UNICODE_MAP:
				header << tr("PDF font name") << tr("Map file");
				break;
				
			default:
				break;
		}
		
		setHorizontalHeaderLabels(header);
		setRowCount(list.size());
		for (int i = 0; i < list.size(); i++)
		{
			QStringList tmp = list.at(i).split(",", QString::SkipEmptyParts);
			if (tmp.size() == 2)
			{
				QTableWidgetItem * item0 = new QTableWidgetItem(tmp.at(0));
				setItem(i, 0, item0);
				QTableWidgetItem * item1 = new QTableWidgetItem(tmp.at(1));
				setItem(i, 1, item1);
			}
		}
		
		resizeColumnToContents(0);
		resizeColumnToContents(1);
	}
}

void FontMapTable::add(const QString & txt0)
{
	int r = rowCount();
	insertRow(r);
	QTableWidgetItem * item0 = new QTableWidgetItem(txt0);
	setItem(r, 0, item0);
}

void FontMapTable::add(const QString & txt0, const QString & txt1)
{
	int r = rowCount();
	insertRow(r);
	QTableWidgetItem * item0 = new QTableWidgetItem(txt0);
	setItem(r, 0, item0);
	QTableWidgetItem * item1 = new QTableWidgetItem(txt1);
	setItem(r, 1, item1);
	dirty = true;
}

void FontMapTable::add(const QString & txt0, 
	                   const QString & txt1,
	                   const QString & txt2,
	                   const QString & txt3)
{
	int r = rowCount();
	insertRow(r);
	QTableWidgetItem * item0 = new QTableWidgetItem(txt0);
	setItem(r, 0, item0);
	QTableWidgetItem * item1 = new QTableWidgetItem(txt1);
	setItem(r, 1, item1);
	QTableWidgetItem * item2 = new QTableWidgetItem(txt2);
	setItem(r, 2, item2);
	QTableWidgetItem * item3 = new QTableWidgetItem(txt3);
	setItem(r, 3, item3);
	dirty = true;
}

void FontMapTable::save()
{
	if (!dirty)
		return ;
		
	XWFontSetting setting;
	if (type == FONT_FILE)
	{
		for (int i = 0; i < rowCount(); i++)
		{
			QString name = item(i, 0)->text();
			QString file = item(i, 1)->text();
			setting.setFontFile(name, file);
		}
	}
	else if (type == NAME_UNICODE_MAP)
	{
		QStringList list;
		for (int i = 0; i < rowCount(); i++)
		{
			QString txt0 = item(i, 0)->text();
			list << txt0;
		}
		setting.setNameToUnicode(list);
	}
	else
	{
		QStringList list;
		for (int i = 0; i < rowCount(); i++)
		{
			QString txt0 = item(i, 0)->text();
			QString txt1 = item(i, 1)->text();
			QString tmp = QString("%1,%2").arg(txt0).arg(txt1);
			list << tmp;
		}
		
		switch (type)
		{
			case CMAP_DIR:
				setting.setCMapDirs(list);
				break;
				
			case UNICODE_MAP:
				setting.setUnicodeMap(list);
				break;
			
			case CID_UNICODE_MAP:
				setting.setCidToUnicodes(list);
				break;
				
			case UNICODE_UNICODE_MAP:
				setting.setUnicodeToUnicodes(list);
				break;
		}
	}
}

void FontMapTable::contextMenuEvent(QContextMenuEvent *event)
{
	if (currentRow() < 0 || currentRow() > rowCount())
		return ;
		
	QMenu menu(this);
	QAction * act = menu.addAction(tr("Delete"));
	connect(act, SIGNAL(triggered()), this, SLOT(deleteRow()));
	menu.exec(event->globalPos());
}

void FontMapTable::deleteRow()
{
	removeRow(currentRow());
	dirty = true;
}

FontMapWindow::FontMapWindow(int typeA, QWidget * parent)
	:QWidget(parent)
{
	type = typeA;	
	QGridLayout * inputlayout = new QGridLayout;	
	if (type == FONT_FILE)
	{
		input0Label = new QLabel(tr("Font name:"));	
		input1Label = new QLabel(tr("Font file:"));	
		input0Edit = new QLineEdit;
		input1Edit = new QLineEdit;
		
		inputlayout->addWidget(input0Label, 0, 0);
		inputlayout->addWidget(input0Edit, 0, 1);
		inputlayout->addWidget(input1Label, 1, 0);
		inputlayout->addWidget(input1Edit, 1, 1);
	}
	else if (type == NAME_UNICODE_MAP)
	{
		input0Label = new QLabel(tr("Map file:"));
		input0Edit = new QLineEdit;	
		
		inputlayout->addWidget(input0Label, 0, 0);
		inputlayout->addWidget(input0Edit, 0, 1);
	}
	else
	{
		switch (type)
		{
			case CMAP_DIR:
				input0Label = new QLabel(tr("Registry-Ordering:"));
				input1Label = new QLabel(tr("Search directory:"));
				break;
				
			case UNICODE_MAP:
				input0Label = new QLabel(tr("Encoding name:"));
				input1Label = new QLabel(tr("Map file:"));
				break;
			
			case CID_UNICODE_MAP:
				input0Label = new QLabel(tr("Registry-Ordering:"));
				input1Label = new QLabel(tr("Map file:"));
				break;
				
			case UNICODE_UNICODE_MAP:
				input0Label = new QLabel(tr("Font name:"));
				input1Label = new QLabel(tr("Map file:"));
				break;
		}
		
		input0Edit = new QLineEdit;
		input1Edit = new QLineEdit;
		
		inputlayout->addWidget(input0Label, 0, 0);
		inputlayout->addWidget(input0Edit, 0, 1);
		inputlayout->addWidget(input1Label, 1, 0);
		inputlayout->addWidget(input1Edit, 1, 1);
	}
	addButton = new QPushButton(tr("Add"));
	inputlayout->addWidget(addButton, 2, 1);
	
	table = new FontMapTable(type, this);
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(inputlayout);
    mainLayout->addWidget(table);
    setLayout(mainLayout);
    
	connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
}

void FontMapWindow::save()
{
	table->save();
}

void FontMapWindow::add()
{
	if (type == NAME_UNICODE_MAP)
	{
		QString txt0 = input0Edit->text();
		if (!txt0.isEmpty())
			table->add(txt0);
	}
	else
	{
		QString txt0 = input0Edit->text();
		QString txt1 = input1Edit->text();
		if (!txt0.isEmpty() && !txt1.isEmpty())
			table->add(txt0, txt1);
	}
}

XWFontWindow::XWFontWindow(QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Display Font Setting"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
  contentsWidget = new QListWidget;  
  QListWidgetItem * item = new QListWidgetItem;
  item->setText(tr("Font File"));
  item->setToolTip(tr("Map font name to font file"));
  contentsWidget->addItem(item);
  
  item = new QListWidgetItem;
  item->setText(tr("CMap search directory"));
  item->setToolTip(tr("Specifies a search directory, dir, for CMaps for\n"
				              "the registry-ordering character collection."));
	contentsWidget->addItem(item);
			              
	item = new QListWidgetItem;
  item->setText(tr("Unicode map"));
  item->setToolTip(tr("Specifies the file with mapping from Unicode to \n"
				              "encoding-name."));
	contentsWidget->addItem(item);
	
	item = new QListWidgetItem;
  item->setText(tr("CID to unicode map"));
  item->setToolTip(tr("Specifies the file with the mapping from character \n"
				              "collection to Unicode."));
	contentsWidget->addItem(item);
				              
	item = new QListWidgetItem;
  item->setText(tr("Unicode to unicode map"));
  item->setToolTip(tr("This is used to work around PDF fonts which have \n"
				              "incorrect Unicode information. It specifies a file \n"
				              "which  maps from the given (incorrect) Unicode \n"
				              "indexes to the correct ones."));	
	contentsWidget->addItem(item);
	
	item = new QListWidgetItem;
  item->setText(tr("Character name to unicode map file"));
  item->setToolTip(tr("Specifies a file with the mapping from \n"
	                    "character names to Unicode."));
  contentsWidget->addItem(item);
	
	pagesWidget = new QStackedWidget;
	
	fontFileWin = new FontMapWindow(FONT_FILE);
	cmapDirWin = new FontMapWindow(CMAP_DIR);
	unicodeMapWin = new FontMapWindow(UNICODE_MAP);
	cidUnicodeMapWin = new FontMapWindow(CID_UNICODE_MAP);
	unicodeToUnicodeWin = new FontMapWindow(UNICODE_UNICODE_MAP);
	nameToUnicodeWin = new FontMapWindow(NAME_UNICODE_MAP);
	
	pagesWidget->addWidget(fontFileWin);
	pagesWidget->addWidget(cmapDirWin);
	pagesWidget->addWidget(unicodeMapWin);
	pagesWidget->addWidget(cidUnicodeMapWin);
	pagesWidget->addWidget(unicodeToUnicodeWin);
	pagesWidget->addWidget(nameToUnicodeWin);
	
	contentsWidget->setCurrentRow(0);
	
	QHBoxLayout *horizontalLayout = new QHBoxLayout;
  horizontalLayout->addWidget(contentsWidget);
  horizontalLayout->addWidget(pagesWidget, 1);
	
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    
 connect(contentsWidget,
             SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
             this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
    
    resize(730, 300);
}

void XWFontWindow::accept()
{
	fontFileWin->save();
	cmapDirWin->save();
	unicodeMapWin->save();
	cidUnicodeMapWin->save();
	unicodeToUnicodeWin->save();
	nameToUnicodeWin->save();
	QDialog::accept();
}

void XWFontWindow::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (!current)
         current = previous;

  pagesWidget->setCurrentIndex(contentsWidget->row(current));
}
