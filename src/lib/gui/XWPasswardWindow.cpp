/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWApplication.h"
#include "XWPasswardWindow.h"

XWPasswardWindow::XWPasswardWindow(const QString & msg, 
	                 								 const QString & labelA, 
	                 								 QWidget * parent)
	:QDialog(parent)
{
	QString title = xwApp->getProductName();
	title += tr(" Password");
	setWindowTitle(title);
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
   label = new QLabel(msg, this);
    
    QString l = QString("%1: ").arg(labelA);
    pwdlabel = new QLabel(l);
    pwdEdit = new QLineEdit;
    pwdEdit->setEchoMode(QLineEdit::Password);
    
    relabel = new QLabel(tr("Re-enter passward: "));
    reEdit = new QLineEdit;
    reEdit->setEchoMode(QLineEdit::Password);
    
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    QGridLayout * passwardlayout = new QGridLayout;
    passwardlayout->addWidget(pwdlabel, 0, 0);
    passwardlayout->addWidget(pwdEdit, 0, 1);
    passwardlayout->addWidget(relabel, 1, 0);
    passwardlayout->addWidget(reEdit, 1, 1);
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addLayout(passwardlayout);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

void XWPasswardWindow::accept()
{
	QString p = pwdEdit->text();
	QString r = reEdit->text();
	if (p == r)
		QDialog::accept();
}

QString XWPasswardWindow::getPassward()
{
	return pwdEdit->text();
}
