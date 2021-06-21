/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include <QCoreApplication>
#include <QDir>
#include "XWBaseNameWindow.h"

XWBaseNameWindow::XWBaseNameWindow(const QString & title, 
	                                 QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(title);
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
	fileNameLabel = new QLabel(tr("Base name:"));
    fileNameEdit = new QLineEdit;
    browseButton = new QPushButton(tr("Browse"));
    connect(browseButton, SIGNAL(clicked()), this, SLOT(open()));
    
    browseButton = new QPushButton(tr("Browse"));
    connect(browseButton, SIGNAL(clicked()), this, SLOT(open()));
    
   QDialogButtonBox *  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    QGridLayout * outlayout = new QGridLayout;
    outlayout->addWidget(fileNameLabel, 0, 0);
    outlayout->addWidget(fileNameEdit, 0, 1);
    outlayout->addWidget(browseButton, 0, 2);
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(outlayout);
    mainLayout->addStretch(1);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

QString XWBaseNameWindow::getBaseName()
{
	QString filename = fileNameEdit->text();
	return filename;
}

void XWBaseNameWindow::open()
{
	QString path = QCoreApplication::applicationDirPath();
	int idx = path.lastIndexOf(QChar('/'));
	if (idx > 0)
		path = path.left(idx + 1);
					
	if (path[path.length() - 1] != QChar('/'))
		path += QChar('/');
			
	path += "images";
	QDir d(path);
	if (!d.exists())
		d.mkdir(path);
		
	QString filename = QFileDialog::getSaveFileName(this, tr("Save file"),
                            path, tr("Image file (*.*)"));
	fileNameEdit->setText(filename);
}
