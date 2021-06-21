/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include <QTextCodec>
#include <QByteArray>
#include "XWFileName.h"
#include "XWFontSea.h"
#include "XWDocSea.h"
#include "XWHelpSea.h"
#include "XWMediaSea.h"
#include "XWPictureSea.h"
#include "XWTexSea.h"
#include "XWOptionParser.h"
#include "XWTFMFile.h"
#include "XWDoc.h"
#include "XWDVICore.h"
#include "XWEncryptWindow.h"
#include "XWPasswardWindow.h"
#include "XWTeXReaderMainWindow.h"
#include "XWTeXReader.h"

XWTeXReader::XWTeXReader(QCoreApplication * app)
	:XWApplication(app),
	 mainWin(0)
{
	installTranslator("xw_guicore_");
	installTranslator("xw_texg_");
	installTranslator("xw_texgui_");
	installTranslator("xw_texreader_");
	XWDocSea::init();
	XWFontSea::init();
	XWHelpSea::init();
	XWMediaSea::init();
	XWPictureSea::init();
	XWTexSea::init();
	XWBibTexSea::init();
	XWMakeIndexSea::init();
	XWXWTexSea::init();
	XWTFMFile::setNeedMetric(true);
}

XWTeXReader::~XWTeXReader()
{
	if (mainWin)
		delete mainWin;
}

void XWTeXReader::badAlloc()
{
	QString msg = QString(tr("No enough memery. Please close some windows or programs."));
	critical(msg);
}

void XWTeXReader::critical(const QString & msg)
{
//	errors << msg;
}

void XWTeXReader::error(const QString & msg)
{
	critical(msg);
}

QString XWTeXReader::getFile(const QUrl & url)
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

bool XWTeXReader::getFile(const QUrl & url, const QString & localname)
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

QString  XWTeXReader::getPassward(const QString & msg,
	                           const QString & labelA)
{
	XWPasswardWindow d(msg, labelA, mainWin);
	QString ret;
	if (d.exec() == QDialog::Accepted)
		ret = d.getPassward();

	return ret;
}

QString XWTeXReader::getProductID()
{
	return "xwtexreader";
}

QString XWTeXReader::getProductName()
{
	return tr("Xiuwen TeX Reader");
}

const char * XWTeXReader::getProductName8()
{
	return "Xiuwen TeX Reader";
}

bool XWTeXReader::getUserAndPassward(const QString & msg,
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

QString XWTeXReader::getVersion()
{
	return "1.02";
}

const char * XWTeXReader::getVersion8()
{
	return "1.02";
}

void XWTeXReader::message(const QString & msg)
{
	mainWin->insertTermText(msg);
}

bool XWTeXReader::openError(const QString & filename, bool w)
{
	QString msg;
	if (w)
		msg = QString(tr("Fail to open file %1 for writing\n")).arg(filename);
	else
		msg = QString(tr("Fail to open file %2 for reading\n")).arg(filename);

	error(msg);
	return false;
}

void XWTeXReader::showErrs()
{
	if (errors.isEmpty())
		return ;

	QString m = errors.join("\n");
	mainWin->insertTermText(m);
	errors.clear();
}

void XWTeXReader::showWarns()
{
	if (warns.isEmpty())
		return ;

	QString m = warns.join("\n");
	mainWin->insertTermText(m);
	warns.clear();
}

void XWTeXReader::start(int & argc, char**argv)
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

	mainWin = new XWTeXReaderMainWindow;
	if (!filename.isEmpty())
	{
		XWFileName fn(filename);
		bool loaded = false;
		if (fn.isLocal())
			loaded = mainWin->loadFile(filename);
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

void XWTeXReader::warning(const QString & msg)
{
//	warns << msg;
}

void XWTeXReader::cleanup()
{
	XWDoc::quit();
	XWDVICore::quit();
	XWApplication::cleanup();
}

void XWTeXReader::quit()
{
	cleanup();
	exit(1);
}

void XWTeXReader::cancelDownload()
{
	XWApplication::cancelDownload();
	progressDialog->hide();
}

void XWTeXReader::downloadFile(const QString & filename,
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

void XWTeXReader::downloadFinished()
{
	XWApplication::downloadFinished();
	progressDialog->hide();
}

void XWTeXReader::setDownloadTitle(const QString & msg)
{
	progressDialog->setLabelText(msg);
}

void XWTeXReader::updateDataTransferProgress(qint64 readBytes,
	                                      qint64 totalBytes)
{
	progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(readBytes);
}
