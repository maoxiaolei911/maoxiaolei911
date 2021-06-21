/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWApplication.h"
#include "XWRecentFileWindow.h"

XWRecentFileWindow::XWRecentFileWindow(QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Recent Files"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
	filesWidget = new QListWidget;
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(filesWidget);
    mainLayout->addStretch(1);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    
    if (xwApp->hasSetting("RecentFile"))
    {
    	QStringList files = xwApp->getSetting("RecentFile").toStringList();
    	for (int i = 0; i < files.size(); i++)
    	{
    		QString file = files.at(i);
    		int n = 1;
    		int idx = file.lastIndexOf(QChar(' '));
    		if (idx > 0 && idx < file.length())
    		{
    			file = file.left(idx);
    			idx++;
    			QString tmp = file.right(file.length() - idx);
    			bool ok = true;
    			n = tmp.toInt(&ok);
    			if (!ok || n <= 0)
    				n = 1;
    		}
    	
    		QListWidgetItem * item = new QListWidgetItem(filesWidget);
    		item->setText(file);
    		item->setData(Qt::UserRole, n);
    	}
    	
    	if (files.size() > 0)
    		filesWidget->setCurrentRow(0);
    }
}

QString XWRecentFileWindow::getFile(int * pg)
{
	*pg = 0;
	QListWidgetItem * item = filesWidget->currentItem();
	if (!item)
		return QString();
		
	*pg = item->data(Qt::UserRole).toInt();
		
	return item->text();
}

