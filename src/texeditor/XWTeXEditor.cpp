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
#include "XWFontSea.h"
#include "XWDocSea.h"
#include "XWHelpSea.h"
#include "XWMediaSea.h"
#include "XWPictureSea.h"
#include "XWTexSea.h"
#include "XWDoc.h"
#include "XWDVICore.h"
#include "XWEncryptWindow.h"
#include "XWPasswardWindow.h"
#include "XWTeXEditorMainWindow.h"
#include "XWTeXEditor.h"

XWTeXEditor::XWTeXEditor(QCoreApplication * app)
	:XWApplication(app),
	 mainWin(0)
{
	installTranslator("xw_guicore_");
	installTranslator("xw_tex_");
	installTranslator("xw_texgui_");
	installTranslator("xw_texeditor_");
	XWDocSea::init();
	XWFontSea::init();
	XWHelpSea::init();
	XWMediaSea::init();
	XWPictureSea::init();
	XWTexSea::init();
	XWTeXFontSetting::init();
	XWBibTexSea::init();
	XWMakeIndexSea::init();
	XWXWTexSea::init();
	XWTFMFile::setNeedMetric(true);
}

XWTeXEditor::~XWTeXEditor()
{
	if (mainWin)
		delete mainWin;
}

void XWTeXEditor::badAlloc()
{
	QString msg = QString(tr("No enough memery. Please close some windows or programs."));
	critical(msg);
}

void XWTeXEditor::critical(const QString & msg)
{
//	errors << msg;
}

void XWTeXEditor::error(const QString & msg)
{
	critical(msg);
}

QString XWTeXEditor::getFile(const QUrl & url)
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

bool XWTeXEditor::getFile(const QUrl & url, const QString & localname)
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

QString  XWTeXEditor::getPassward(const QString & msg,
	                           const QString & labelA)
{
	XWPasswardWindow d(msg, labelA, mainWin);
	QString ret;
	if (d.exec() == QDialog::Accepted)
		ret = d.getPassward();

	return ret;
}

QString XWTeXEditor::getProductID()
{
	return "xwtexeditor";
}

QString XWTeXEditor::getProductName()
{
	return tr("Xiuwen TeX Editor");
}

const char * XWTeXEditor::getProductName8()
{
	return "Xiuwen TeX Editor";
}

bool XWTeXEditor::getUserAndPassward(const QString & msg,
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

QString XWTeXEditor::getVersion()
{
	return "1.4";
}

const char * XWTeXEditor::getVersion8()
{
	return "1.4";
}

void XWTeXEditor::message(const QString & msg)
{
	mainWin->insertTermText(msg);
}

bool XWTeXEditor::openError(const QString & filename, bool w)
{
	QString msg;
	if (w)
		msg = QString(tr("Fail to open file %1 for writing\n")).arg(filename);
	else
		msg = QString(tr("Fail to open file %2 for reading\n")).arg(filename);

	error(msg);
	return false;
}

void XWTeXEditor::showErrs()
{
	if (errors.isEmpty())
		return ;

	QString m = errors.join("\n");
	mainWin->insertTermText(m);
	errors.clear();
}

void XWTeXEditor::showWarns()
{
	if (warns.isEmpty())
		return ;

	QString m = warns.join("\n");
	mainWin->insertTermText(m);
	warns.clear();
}

void XWTeXEditor::start(int & , char**)
{
	mainWin = new XWTeXEditorMainWindow;
	mainWin->show();
}

void XWTeXEditor::warning(const QString & msg)
{
//	warns << msg;
}

void XWTeXEditor::cleanup()
{
	XWDoc::quit();
	XWDVICore::quit();
	XWApplication::cleanup();
}

void XWTeXEditor::quit()
{
	cleanup();
	exit(1);
}

void XWTeXEditor::cancelDownload()
{
	XWApplication::cancelDownload();
	progressDialog->hide();
}

void XWTeXEditor::downloadFile(const QString & filename,
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

void XWTeXEditor::downloadFinished()
{
	XWApplication::downloadFinished();
	progressDialog->hide();
}

void XWTeXEditor::setDownloadTitle(const QString & msg)
{
	progressDialog->setLabelText(msg);
}

void XWTeXEditor::updateDataTransferProgress(qint64 readBytes,
	                                      qint64 totalBytes)
{
	progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(readBytes);
}
