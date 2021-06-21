/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWGotoPageWindow.h"

XWGotoPageWindow::XWGotoPageWindow(int num, QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Goto Page"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
    pageslabel = new QLabel(tr("Page: "), this);
    pagesCombo = new QComboBox;
    for (int i = 0; i < num; i++)
    {
    	QString tmp = QString("%1").arg(i);
    	pagesCombo->addItem(tmp);
    }
    
    QHBoxLayout * pagelayout = new QHBoxLayout;
    pagelayout->addWidget(pageslabel);
    pagelayout->addWidget(pagesCombo);
    
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(pagelayout);
    mainLayout->addStretch(1);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

int XWGotoPageWindow::getPage()
{
	QString tmp = pagesCombo->currentText();
	return tmp.toInt();
}

