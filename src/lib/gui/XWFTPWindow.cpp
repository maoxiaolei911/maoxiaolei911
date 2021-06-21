/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include <QCoreApplication>
#include "XWFTPWindow.h"

XWFTPWindow::XWFTPWindow(QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Open file from network"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
	label = new QLabel(tr("Get a file from network(.pdf, .dvi or .mps)"), this);
	
	schemelabel = new QLabel(tr("Scheme:"), this);
	schemelabel->setToolTip(tr("The type (or protocol) of the URL, ftp or http."));
	schemeEdit = new QLineEdit(this);
	schemeEdit->setText("ftp");
	userlabel = new QLabel(tr("User name:"), this);
	userlabel->setToolTip(tr("User name for authentication."));
	userEdit = new QLineEdit(this);
	passwdlabel = new QLabel(tr("Password:"), this);
	passwdlabel->setToolTip(tr("Password for authentication."));
	passwdEdit = new QLineEdit(this);
	passwdEdit->setEchoMode(QLineEdit::Password);
	hostlabel = new QLabel(tr("Host:"), this);
	hostlabel->setToolTip(tr("The host of the URL(net address)."));
	hostEdit = new QLineEdit(this);
	portlabel = new QLabel(tr("Port:"), this);
	portBox = new QSpinBox();
	portBox->setRange(0, 65536);
	portBox->setValue(0);	
	pathlabel = new QLabel(tr("Path:"), this);
	pathlabel->setToolTip(tr("The file path."));
	pathEdit = new QLineEdit(this);
	
	localBox = new QCheckBox(tr("Save as: "), this);
	localBox->setChecked(false);
	localEdit = new QLineEdit(this);
	QPushButton * openButton = new QPushButton(tr("Browse"));
	connect(openButton, SIGNAL(clicked()), this, SLOT(open()));
	
	QGridLayout *urllayout = new QGridLayout;
	urllayout->addWidget(schemelabel, 0, 0);
	urllayout->addWidget(schemeEdit, 0, 1);
	urllayout->addWidget(userlabel, 1, 0);
	urllayout->addWidget(userEdit, 1, 1);
	urllayout->addWidget(passwdlabel, 2, 0);
	urllayout->addWidget(passwdEdit, 2, 1);
	urllayout->addWidget(hostlabel, 3, 0);
	urllayout->addWidget(hostEdit, 3, 1);
	urllayout->addWidget(portlabel, 4, 0);
	urllayout->addWidget(portBox, 4, 1);
	urllayout->addWidget(pathlabel, 5, 0);
	urllayout->addWidget(pathEdit, 5, 1);
	urllayout->addWidget(localBox, 6, 0);
	urllayout->addWidget(localEdit, 6, 1);
	urllayout->addWidget(openButton, 6, 2);
	
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addLayout(urllayout);
    mainLayout->addStretch(1);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

QString XWFTPWindow::getLocalName()
{
	QString filename = localEdit->text();
	return filename;
}

QUrl XWFTPWindow::getURL()
{
	QUrl url;
	QString scheme = schemeEdit->text();
	url.setScheme(scheme);
	QString user = userEdit->text();
	if (!user.isEmpty())
		url.setUserName(user);
	QString passwd = passwdEdit->text();
	if (!passwd.isEmpty())
		url.setPassword(passwd);
		
	QString host = hostEdit->text();
	url.setHost(host);
	int port = portBox->value();
	if (port != 0)
		url.setPort(port);
		
	QString path = pathEdit->text();
	url.setPath(path);
	return url;
}

bool XWFTPWindow::saveToLocal()
{
	return localBox->isChecked();
}

void XWFTPWindow::open()
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

