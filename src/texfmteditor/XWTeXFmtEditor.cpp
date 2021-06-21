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
#include "XWTeXFmtEditorMainWindow.h"
#include "XWTeXFmtEditor.h"

XWTeXFmtEditor::XWTeXFmtEditor(QCoreApplication * app)
	:XWApplication(app),
	 mainWin(0)
{
	installTranslator("xw_guicore_");
	installTranslator("xw_tex_");
	installTranslator("xw_texgui_");
	installTranslator("xw_texfmteditor_");

	XWDocSea::init();
	XWFontSea::init();
	XWHelpSea::init();
	XWMediaSea::init();
	XWPictureSea::init();
	XWTexSea::init();
	XWBibTexSea::init();
	XWMakeIndexSea::init();
	XWXWTexSea::init();
}

XWTeXFmtEditor::~XWTeXFmtEditor()
{
	if (mainWin)
		delete mainWin;
}

void XWTeXFmtEditor::badAlloc()
{
	QString msg = QString(tr("No enough memery. Please close some windows or programs."));
	critical(msg);
}

void XWTeXFmtEditor::critical(const QString & msg)
{
	mainWin->insertTermText(msg);
}

void XWTeXFmtEditor::error(const QString & msg)
{
	critical(msg);
}

QString XWTeXFmtEditor::getFile(const QUrl & url)
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

bool XWTeXFmtEditor::getFile(const QUrl & url, const QString & localname)
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

QString  XWTeXFmtEditor::getPassward(const QString & msg,
	                           const QString & labelA)
{
	XWPasswardWindow d(msg, labelA, mainWin);
	QString ret;
	if (d.exec() == QDialog::Accepted)
		ret = d.getPassward();

	return ret;
}

QString XWTeXFmtEditor::getProductID()
{
	return "xwtexfmteditor";
}

QString XWTeXFmtEditor::getProductName()
{
	return tr("Xiuwen TeX Format Editor");
}

const char * XWTeXFmtEditor::getProductName8()
{
	return "Xiuwen TeX Format Editor";
}

bool XWTeXFmtEditor::getUserAndPassward(const QString & msg,
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

QString XWTeXFmtEditor::getVersion()
{
	return "1.4";
}

const char * XWTeXFmtEditor::getVersion8()
{
	return "1.4";
}

void XWTeXFmtEditor::message(const QString & msg)
{
	mainWin->insertTermText(msg);
}

bool XWTeXFmtEditor::openError(const QString & filename, bool w)
{
	QString msg;
	if (w)
		msg = QString(tr("Fail to open file %1 for writing\n")).arg(filename);
	else
		msg = QString(tr("Fail to open file %2 for reading\n")).arg(filename);

	error(msg);
	return false;
}

void XWTeXFmtEditor::start(int & , char**)
{
	mainWin = new XWTeXFmtEditorMainWindow;
	mainWin->show();
}

void XWTeXFmtEditor::warning(const QString & msg)
{
	mainWin->insertTermText(msg);
}

void XWTeXFmtEditor::cleanup()
{
	XWApplication::cleanup();
}

void XWTeXFmtEditor::quit()
{
	cleanup();
	exit(1);
}

void XWTeXFmtEditor::cancelDownload()
{
	XWApplication::cancelDownload();
	progressDialog->hide();
}

void XWTeXFmtEditor::downloadFile(const QString & filename,
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

void XWTeXFmtEditor::downloadFinished()
{
	XWApplication::downloadFinished();
	progressDialog->hide();
}

void XWTeXFmtEditor::setDownloadTitle(const QString & msg)
{
	progressDialog->setLabelText(msg);
}

void XWTeXFmtEditor::updateDataTransferProgress(qint64 readBytes,
	                                      qint64 totalBytes)
{
	progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(readBytes);
}
