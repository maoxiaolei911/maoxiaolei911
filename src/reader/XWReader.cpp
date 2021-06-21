/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include <QTextCodec>
#include <QByteArray>
#include "XWFileName.h"
#include "XWOptionParser.h"
#include "XWTFMFile.h"
#include "XWDoc.h"
#include "XWDVICore.h"
#include "XWEncryptWindow.h"
#include "XWPasswardWindow.h"
#include "XWReaderMainWindow.h"
#include "XWReader.h"

XWReader::XWReader(QCoreApplication * app)
	:XWApplication(app),
	 mainWin(0)
{
	installTranslator("xw_guicore_");
	installTranslator("xw_reader_");
	XWTFMFile::setNeedMetric(true);
}

XWReader::~XWReader()
{
	if (mainWin)
		delete mainWin;
}

void XWReader::badAlloc()
{
	QString msg = QString(tr("No enough memery. Please close some windows or programs."));
	critical(msg);
}

void XWReader::critical(const QString & msg)
{
//	errors << msg;
}

void XWReader::error(const QString & msg)
{
	critical(msg);
}

QString XWReader::getFile(const QUrl & url)
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

bool XWReader::getFile(const QUrl & url, const QString & localname)
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

QString  XWReader::getPassward(const QString & msg,
	                           const QString & labelA)
{
	XWPasswardWindow d(msg, labelA, mainWin);
	QString ret;
	if (d.exec() == QDialog::Accepted)
		ret = d.getPassward();

	return ret;
}

QString XWReader::getProductID()
{
	return "xwreader";
}

QString XWReader::getProductName()
{
	return tr("Xiuwen Reader");
}

const char * XWReader::getProductName8()
{
	return "Xiuwen Reader";
}

bool XWReader::getUserAndPassward(const QString & msg,
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

QString XWReader::getVersion()
{
	return "1.02";
}

const char * XWReader::getVersion8()
{
	return "1.02";
}

void XWReader::message(const QString & msg)
{
	QMessageBox::information(mainWin, tr("Message"), msg, QMessageBox::Close, QMessageBox::Close);
}

bool XWReader::openError(const QString & filename, bool w)
{
	QString msg;
	if (w)
		msg = QString(tr("Fail to open file %1 for writing\n")).arg(filename);
	else
		msg = QString(tr("Fail to open file %1 for reading\n")).arg(filename);

	error(msg);
	return false;
}

void XWReader::showErrs()
{
	if (errors.isEmpty())
		return ;

	QString m = errors.join("\n");
	QMessageBox::critical(mainWin, tr("Error"), m, QMessageBox::Close);
	errors.clear();
}

void XWReader::showWarns()
{
	if (warns.isEmpty())
		return ;

	QString m = warns.join("\n");
	QMessageBox::warning(mainWin, tr("Warning"), m, QMessageBox::Close, QMessageBox::Close);
	warns.clear();
}

void XWReader::start(int & argc, char**argv)
{
	QString tmp;
	QString dest;
	QString filename;

	XWOptionParser parse(argc, argv, 0, 0);
	int c = -1;
	while( -1 != (c = parse.getOpt("d:f:p:")))
	{
		switch (c)
		{
			case 'd':
				dest = parse.optArgToQString();
				break;

			case 'f':
				filename = parse.optArgToQString();
				break;

			case 'p':
				tmp = parse.optArgToQString();
				break;
		}
	}

	if (filename.isEmpty() && argc == 2)
	{
		QTextCodec * codec = QTextCodec::codecForLocale();
		filename = codec->toUnicode(argv[1]);
	}

	mainWin = new XWReaderMainWindow;
	if (!filename.isEmpty())
	{
		XWFileName fn(filename);
		bool loaded = false;
		if (fn.isLocal())
			loaded = mainWin->loadFile(filename, false);
		else
		{
			QUrl url(filename);
			loaded = mainWin->loadURL(filename, QString());
		}

		if (loaded)
		{
			if (!dest.isEmpty())
			{
				QTextCodec * codec = QTextCodec::codecForLocale();
				QByteArray ba = codec->fromUnicode(dest);
				mainWin->displayDest(ba.constData());
			}
			else if (!tmp.isEmpty())
			{
				int pg = tmp.toInt();
				if (pg > 0)
					mainWin->displayPage(pg);
			}
			else
				mainWin->displayPage(1);
		}
	}

	mainWin->show();
}

void XWReader::warning(const QString & msg)
{
//	warns << msg;
}

void XWReader::cleanup()
{
	XWDoc::quit();
	XWDVICore::quit();
	XWApplication::cleanup();
}

void XWReader::quit()
{
	qApp->closeAllWindows();
	XWApplication::quit();
}

void XWReader::cancelDownload()
{
	XWApplication::cancelDownload();
	progressDialog->hide();
}

void XWReader::downloadFile(const QString & filename,
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

void XWReader::downloadFinished()
{
	XWApplication::downloadFinished();
	progressDialog->hide();
}

void XWReader::setDownloadTitle(const QString & msg)
{
	progressDialog->setLabelText(msg);
}

void XWReader::updateDataTransferProgress(qint64 readBytes,
	                                      qint64 totalBytes)
{
	progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(readBytes);
}
