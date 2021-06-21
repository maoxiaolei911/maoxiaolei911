/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include <QTextCodec>
#include <QByteArray>
#include "XWFileName.h"
#include "XWDoc.h"
#include "XWDVICore.h"
#include "XWEncryptWindow.h"
#include "XWPasswardWindow.h"
#include "XWDVITypeMainWindow.h"
#include "XWDVIShow.h"

XWDVIShow::XWDVIShow(QCoreApplication * app)
	:XWApplication(app),
	 mainWin(0)
{
	installTranslator("xw_dvitype_");
}

XWDVIShow::~XWDVIShow()
{
	if (mainWin)
		delete mainWin;
}

void XWDVIShow::badAlloc()
{
	QString msg = QString(tr("No enough memery. Please close some windows or programs."));
	critical(msg);
}

void XWDVIShow::critical(const QString & msg)
{
	QMessageBox::critical(mainWin, tr("Error"), msg);
}

void XWDVIShow::error(const QString & msg)
{
	critical(msg);
}

QString XWDVIShow::getFile(const QUrl & url)
{
	curUrl = url;
	requestAborted = false;
	QDir tmpdir(tmpDir);
	if (!tmpdir.exists())
		tmpdir.mkpath(tmpDir);
			
	QString path = curUrl.path();
	QFileInfo fileInfo(path);
	QString filename = fileInfo.fileName();	
	QString tmpname = QString("%1/%2").arg(tmpDir).arg(filename);
	downloadFile(path, tmpname);
			
	QString ret;
	if (downloadingFile)
	{
		ret = tmpname;
		delete downloadingFile;
		downloadingFile = 0;
	}
	
	return ret;
}

bool XWDVIShow::getFile(const QUrl & url, const QString & localname)
{
	curUrl = url;
	QString path = curUrl.path();
	requestAborted = false;
	downloadFile(path, localname);

	if (downloadingFile)
	{
		delete downloadingFile;
		downloadingFile = 0;
		return true;
	}
	
	return false;
}

QString  XWDVIShow::getPassward(const QString & msg, 
	                           const QString & labelA)
{
	XWPasswardWindow d(msg, labelA, mainWin);
	QString ret;
	if (d.exec() == QDialog::Accepted)
		ret = d.getPassward();
		
	return ret;
}

QString XWDVIShow::getProductID()
{
	return "xwdvitype";
}

QString XWDVIShow::getProductName()
{
	return tr("Xiuwen DVI Type");
}

const char * XWDVIShow::getProductName8()
{
	return "Xiuwen DVI Type";
}

bool XWDVIShow::getUserAndPassward(const QString & msg,
                                  QString & user, 
                                  QString & passwd)
{
	XWEncryptWindow d(msg, mainWin);
	bool ret = d.exec() == QDialog::Accepted;
	if (ret)
	{
		user = d.getUserName();
		passwd = d.getPassward();
	}
	
	return ret;
}

QString XWDVIShow::getVersion()
{
	return "0.9";
}

const char * XWDVIShow::getVersion8()
{
	return "0.9";
}

void XWDVIShow::message(const QString & msg)
{
	QMessageBox::information(mainWin, tr("Message"), msg, QMessageBox::Close, QMessageBox::Close);
}

bool XWDVIShow::openError(const QString & filename, bool w)
{
	QString msg;
	if (w)
		msg = QString(tr("Fail to open file %1 for writing\n")).arg(filename);
	else
		msg = QString(tr("Fail to open file %2 for reading\n")).arg(filename);
		
	error(msg);
	return false;
}

void XWDVIShow::start(int & , char**)
{
	mainWin = new XWDVITypeMainWindow;	
	mainWin->show();
}

void XWDVIShow::warning(const QString & msg)
{
	QMessageBox::critical(mainWin, tr("Warn"), msg);
}

void XWDVIShow::cleanup()
{
	XWDVICore::quit();
	XWApplication::cleanup();
}

void XWDVIShow::quit()
{
	cleanup();
	exit(1);
}

void XWDVIShow::cancelDownload()
{
	XWApplication::cancelDownload();
	progressDialog->hide();
}

void XWDVIShow::downloadFile(const QString & filename,
	                        const QString & localname)
{
	if (!progressDialog)
	{
		progressDialog = new QProgressDialog(mainWin);
		QString name = getProductName();
		progressDialog->setWindowTitle(name);
    	progressDialog->setWindowIcon(QIcon(":/images/xiuwen.png"));
    	connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
	}
		
	XWApplication::downloadFile(filename, localname);
		
	if (downloadingFile)
		progressDialog->exec();
}

void XWDVIShow::downloadFinished()
{
	XWApplication::downloadFinished();
	progressDialog->hide();
}

void XWDVIShow::setDownloadTitle(const QString & msg)
{
	progressDialog->setLabelText(msg);
}

void XWDVIShow::updateDataTransferProgress(qint64 readBytes, 
	                                      qint64 totalBytes)
{
	progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(readBytes);
}

QString XWDVIShow::getLicensesName()
{
	return "xwdvitype.dat";
}
