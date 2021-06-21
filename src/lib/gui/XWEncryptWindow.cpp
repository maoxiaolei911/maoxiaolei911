/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWApplication.h"
#include "XWEncryptWindow.h"

XWEncryptWindow::XWEncryptWindow(const QString & msg, QWidget * parent)
	:QDialog(parent)
{
	QString title = xwApp->getProductName();
	title += tr(" User  And Password");
	setWindowTitle(title);
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
    label = new QLabel(msg, this);
    
    userlabel = new QLabel(tr("User name: "));
    userEdit = new QLineEdit;
    
    passwardlabel = new QLabel(tr("Passward: "));
    passwardEdit = new QLineEdit;
    passwardEdit->setEchoMode(QLineEdit::Password);
    
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    QGridLayout * authlayout = new QGridLayout;
    authlayout->addWidget(userlabel, 0, 0);
    authlayout->addWidget(userEdit, 0, 1);
    authlayout->addWidget(passwardlabel, 1, 0);
    authlayout->addWidget(passwardEdit, 1, 1);
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addStretch(1);
    mainLayout->addLayout(authlayout);
    mainLayout->addStretch(2);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

QString XWEncryptWindow::getUserName()
{
	return userEdit->text();
}

QString XWEncryptWindow::getPassward()
{
	return passwardEdit->text();
}

