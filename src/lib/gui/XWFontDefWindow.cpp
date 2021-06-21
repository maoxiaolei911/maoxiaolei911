/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWFontDefWindow.h"

XWFontDefWindow::XWFontDefWindow(DVIFontDef * defs, int num, QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Font Definiens"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
  table = new QTableWidget(num, 4, this);
  QStringList labels;
  labels << tr("TeX ID") << tr("Name") << tr("Point size") << tr("Design size");
  table->setHorizontalHeaderLabels(labels);
  
  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	
	QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(table);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);
  
  int row = 0;
  QString tmp;
  for (int i = 0; i < num; i++)
  {
  	DVIFontDef * font = &(defs[i]);
  	if (font)
  	{
  		table->insertRow(row);
  		QTableWidgetItem *item = new QTableWidgetItem(tr("%1").arg(font->texId));
  		table->setItem(row, 0, item);
  		if (font->fontName)
				tmp = QString::fromAscii(font->fontName);
			else
				tmp = tr("None");
				
			item = new QTableWidgetItem(tmp);
			table->setItem(row, 1, item);
			
			item = new QTableWidgetItem(tr("%1").arg(font->pointSize));
  		table->setItem(row, 2, item);
  		
  		item = new QTableWidgetItem(tr("%1").arg(font->designSize));
  		table->setItem(row, 3, item);
  		row++;
  	}
  }
  
  table->resizeColumnToContents(1);
  table->resizeColumnToContents(2);
  table->resizeColumnToContents(3);
  
  int w = table->columnWidth(0);
  w += table->columnWidth(1);
  w += table->columnWidth(2);
  w += table->columnWidth(3);
  w += 70;
  
  resize(w, 320);
}
