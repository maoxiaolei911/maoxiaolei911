/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include <QCoreApplication>
#include "XWURLWindow.h"

XWURLWindow::XWURLWindow(QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Open URL"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
	label = new QLabel(tr("Enter the Internet address of a file(.pdf, .dvi or .mps)"), this);
	openlabel = new QLabel(tr("Open:"), this);
	urlEdit = new QLineEdit(this);
	
	localBox = new QCheckBox(tr("Save as: "), this);
	localBox->setChecked(false);
	localEdit = new QLineEdit(this);
	QPushButton * openButton = new QPushButton(tr("Browse"));
	connect(openButton, SIGNAL(clicked()), this, SLOT(open()));
	
	QGridLayout *urllayout = new QGridLayout;
	urllayout->addWidget(openlabel, 0, 0);
	urllayout->addWidget(urlEdit, 0, 1);
	urllayout->addWidget(localBox, 1, 0);
	urllayout->addWidget(localEdit, 1, 1);
	urllayout->addWidget(openButton, 1, 2);
	
	
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addLayout(urllayout);
    mainLayout->addStretch(4);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

QString XWURLWindow::getLocalName()
{
	QString filename = localEdit->text();
	return filename;
}

QString XWURLWindow::getURL()
{
	return urlEdit->text();
}

bool XWURLWindow::saveToLocal()
{
	return localBox->isChecked();
}

void XWURLWindow::open()
{
	QString path = QCoreApplication::applicationDirPath();
	int idx = path.indexOf(QChar('/'));
	if (idx > 0)
		path = path.left(idx + 1);
					
	if (path[path.length() - 1] != QChar('/'))
		path += QChar('/');
			
	path += "doc";
	
	QString filename = QFileDialog::getSaveFileName(this, tr("Save as"),
                            path, tr("Files (*.*)"));
	localEdit->setText(filename);
}

