/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWString.h"
#include "XWGuiCore.h"
#include "XWSearchWindow.h"

XWSearchWindow::XWSearchWindow(XWGuiCore * coreA, QWidget *parent)
    :QDialog(parent)
{
	core = coreA;
	setWindowTitle(tr("Find in document"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
    searchTextLabel = new QLabel(tr("Search content:"));
    searchText = new QLineEdit;
    
    QHBoxLayout * contentlayout = new QHBoxLayout;
    contentlayout->addWidget(searchTextLabel);
    contentlayout->addWidget(searchText);
    
    caseSensitive = new QCheckBox(tr("Case sensitive"));
    wholeWord = new QCheckBox(tr("Only whole word"));
    backward = new QCheckBox(tr("Search backward"));    
    allPage = new QCheckBox(tr("Search whole document"));
    
    findButton = new QPushButton(tr("Find"));
    nextButton = new QPushButton(tr("Find next"));
    closeButton = new QPushButton(tr("Close"));
    
    QHBoxLayout * buttonlayout = new QHBoxLayout;
    buttonlayout->addStretch(1);
    buttonlayout->addWidget(findButton);
    buttonlayout->addWidget(nextButton);
    buttonlayout->addWidget(closeButton);
        
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(contentlayout);
    mainLayout->addWidget(caseSensitive);
    mainLayout->addWidget(wholeWord);
    mainLayout->addWidget(backward);
    mainLayout->addWidget(allPage);
    mainLayout->addStretch(1);
    mainLayout->addLayout(buttonlayout);
    setLayout(mainLayout);
    
    connect(findButton, SIGNAL(clicked()), this, SLOT(find()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(findNext()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

void XWSearchWindow::find()
{
	QString context = searchText->text();
	core->find(context, caseSensitive->isChecked(), false, backward->isChecked(), wholeWord->isChecked(), !(allPage->isChecked()));
}

void XWSearchWindow::findNext()
{
	QString context = searchText->text();
	core->find(context, caseSensitive->isChecked(), true, backward->isChecked(), wholeWord->isChecked(), !(allPage->isChecked()));
}


