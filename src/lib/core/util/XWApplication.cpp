/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <stdio.h>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDataStream>
#include <QDateTime>
#include <QCryptographicHash>
#include <QtNetwork>
#include <QLocale>
#include <QTranslator>
#include <QSettings>
#include <QDesktopServices>
#include "XWUtil.h"
#include "XWDeviceInfo.h"
#include "XWAes.h"
#include "XWBase64.h"
#include "XWDes.h"
#include "XWApplication.h"

XWApplication * XWApplication::_instance = 0;

XWApplication::XWApplication(QCoreApplication * app)
	:realApp(app),
	 networkReply(0),
	 downloadingFile(0),
	 requestAborted(true)
{
	_instance = this;
	initDir();

	installTranslator("qt_");
	installTranslator("xw_core_");

	QString bindir = QCoreApplication::applicationDirPath();
	XWDeviceInfo dev(bindir);
	devType = dev.type();
	devSerialNumber = dev.serialNumber();
	licenseState = APP_STATE_NOLICENSESFILE;
	checkLicense();

	QString regfilename = QString("%1/registry.dat").arg(bindir);
	settings = new QSettings(regfilename, QSettings::IniFormat, this);
	fontCnt = 0;

	connect(&networkAM, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
             this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
	if (app)
		connect(app, SIGNAL(aboutToQuit()), this, SLOT(cleanup()));
}

XWApplication::~XWApplication()
{
}

QString XWApplication::aboutHardware()
{
	QString lang = QLocale::system().name();
	QString h = QString(tr("<h3>Hardware ID: %1%2</h3>"
	                       "<p>Please provide this hardware identifier when purchase xiuwen service.</p>"
	                       "<p>See <a href=\"http://www.xwtex.com/purchase%3.html\">xiuwen service license</a>.</p>"))
	                       .arg(devType).arg(devSerialNumber).arg(lang);
	return h;
}

void XWApplication::badAlloc()
{
	QString msg = QString(tr("no enough memery to use.\n"));
	critical(msg);
}

void XWApplication::clearTmpFiles()
{
	QDir tmpdir(tmpDir);
	if (tmpdir.exists())
	{
		QStringList files = tmpdir.entryList(QDir::Files);
   	for (int i = 0; i < files.size(); i++)
   	{
       	QString name = files.at(i);
       	QString filename = QString("%1/%2").arg(tmpDir).arg(name);
       	QFile::remove(filename);
   	}

   	tmpdir.rmdir(tmpDir);
  }

  fontCnt = 0;
}

void XWApplication::critical(const QString & msg)
{
	QString newmsg = QString(tr("Critical: %1 quit!\n")).arg(msg);
	showMessage(newmsg);
	quit();
}

void XWApplication::error(const QString & msg)
{
	QString newmsg = QString(tr("Error: %1")).arg(msg);
	showMessage(newmsg);
}

QString XWApplication::getCopyRight()
{
	return tr("Copyright(C) 2006-2007 Xiuwen Computer Solutions, Inc");
}

const char * XWApplication::getCopyRight8()
{
	return "Copyright(C) 2006-2007 Xiuwen Computer Solutions, Inc";
}

QString XWApplication::getFile(const QString & url)
{
	QString ret;
	if (downloadedFiles.contains(url))
		ret = downloadedFiles[url];
	else
	{
		QUrl uurl(url);
		ret = getFile(uurl);
		if (!ret.isEmpty())
			downloadedFiles[url] = ret;
	}

	return ret;
}

QString  XWApplication::getFile(const QUrl & url)
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
	while (!requestAborted)
		realApp->processEvents();

	QString ret;
	if (downloadingFile)
	{
		ret = tmpname;
		delete downloadingFile;
		downloadingFile = 0;
	}

	return ret;
}

bool XWApplication::getFile(const QUrl & url, const QString & localname)
{
	curUrl = url;
	QString path = curUrl.path();
	requestAborted = false;
	downloadFile(path, localname);
	while (!requestAborted)
		realApp->processEvents();

	if (downloadingFile)
	{
		delete downloadingFile;
		downloadingFile = 0;
		return true;
	}

	return false;
}

void XWApplication::getHelp(const QString & prefix)
{
	QString lang = QLocale::system().name();
	QString path = QString("http://www.xwtex.com/%1%2.html").arg(prefix).arg(lang);
	QUrl url(path);
	QDesktopServices::openUrl(url);
}

QString XWApplication::getPassward(const QString & msg,
	                                 const QString & labelA)
{
	QString ret;
	showMessage(msg);

	while (1)
	{
		QString m = QString("%1: ").arg(labelA);
		showMessage(labelA);
		QString p = inputLine();
		showMessage(tr("Re-enter passward: "));
		QString r = inputLine();
		if (p == r)
		{
			ret = p;
			break;
		}
	}

	return ret;
}

QString XWApplication::getProductID()
{
	return QString();
}

QString XWApplication::getProductName()
{
	return QString();
}

const char * XWApplication::getProductName8()
{
	return 0;
}

QSettings * XWApplication::getSettings()
{
	QString prefix = getProductName();
	settings->beginGroup(prefix);
	return settings;
}

QVariant XWApplication::getSetting(const QString & key)
{
	QString prefix = getProductName();
	settings->beginGroup(prefix);
	QVariant ret = settings->value(key);
	settings->endGroup();

	return ret;
}

QString XWApplication::getTmpFile(const QString & filename)
{
	QString tmpfile = tmpDir;
	if (!filename.isEmpty())
		tmpfile += QString("/%1").arg(filename);
	else
	{
		if (fontCnt > 65536)
			fontCnt = 0;
		tmpfile += QString("/f%1").arg(fontCnt);
		fontCnt++;
	}

	return tmpfile;
}

bool XWApplication::getUserAndPassward(const QString & msg,
                                       QString & user,
                                       QString & passwd)
{
	showMessage(msg);
	showMessage(tr("User Name: "));
	user = inputLine();
	showMessage(tr("Passward: "));
	passwd = inputLine();

	return true;
}

QString XWApplication::getVersion()
{
	return QString();
}

const char * XWApplication::getVersion8()
{
	return "1.0";
}

bool XWApplication::hasSetting(const QString & key)
{
	QString prefix = getProductName();
	settings->beginGroup(prefix);

	bool ret = settings->contains(key);

	settings->endGroup();

	return ret;
}

void XWApplication::installTranslator(const QString & trans)
{
	QString bindir = QCoreApplication::applicationDirPath();
  QString top = bindir;
  int ti = bindir.lastIndexOf(QChar('/'));
  if (ti > 0)
   	top = top.left(ti);

	QString apptrans = QString("%1/translations/%2").arg(top).arg(trans);
	apptrans += QLocale::system().name();
	QTranslator * appTranslator = new QTranslator;
	appTranslator->load(apptrans);
	realApp->installTranslator(appTranslator);
}

void XWApplication::message(const QString & msg)
{
	QString newmsg = QString(tr("Message: %1")).arg(msg);
	showMessage(newmsg);
}

bool XWApplication::openError(const QString & filename, bool w)
{
	QString msg;
	if (w)
		msg = QString(tr("fail to open file '%1' for writing\n")).arg(filename);
	else
		msg = QString(tr("fail to open file '%2' for reading\n")).arg(filename);

	error(msg);
	return false;
}

QFile * XWApplication::openTmpFile(const QString & filename)
{
	QString tmpfile = tmpDir;
	if (!filename.isEmpty())
		tmpfile += QString("/%1").arg(filename);
	else
	{
		if (fontCnt > 65536)
			fontCnt = 0;
		tmpfile += QString("/f%1").arg(fontCnt);
		fontCnt++;
	}

	QFile * ret = new QFile(tmpfile);
	if (!ret->open(QIODevice::ReadWrite))
	{
		delete ret;
		ret = 0;
	}

	return ret;
}

void XWApplication::saveSetting(const QString & key, const QVariant & value)
{
	QString prefix = getProductName();
	settings->beginGroup(prefix);
	settings->setValue(key, value);
	settings->endGroup();
}

void XWApplication::warning(const QString & msg)
{
	QString newmsg = QString(tr("Warning: %1")).arg(msg);
	showMessage(newmsg);
}

void XWApplication::cleanup()
{
	if (networkReply)
	{
		delete networkReply;
		networkReply = 0;
	}

	if (downloadingFile)
	{
		delete downloadingFile;
		downloadingFile = 0;
	}

	clearTmpFiles();

  if (settings)
  {
  	delete settings;
  	settings = 0;
  }
}

void XWApplication::quit()
{
}

void XWApplication::authenticationRequired(QNetworkReply*,
	                                       QAuthenticator * authenticator)
{
	QString msg = QString(tr("%1 at %2")).arg(authenticator->realm()).arg(curUrl.host());
	QString user;
	QString passwd;
	if (getUserAndPassward(msg, user, passwd))
	{
		authenticator->setUser(user);
		authenticator->setUser(passwd);
	}
}

void XWApplication::cancelDownload()
{
	if (networkReply)
	{
		networkReply->abort();
		networkReply->deleteLater();
		networkReply = 0;
	}

	if (downloadingFile)
	{
		downloadingFile->flush();
		downloadingFile->close();
		downloadingFile->remove();
		delete downloadingFile;
		downloadingFile = 0;
	}

	requestAborted = true;
}

void XWApplication::downloadFile(const QString & filename,
	                             const QString & localname)
{
	QString host = curUrl.host();
	downloadingFile = new QFile(localname);
	if (downloadingFile && downloadingFile->open(QIODevice::WriteOnly))
	{
		networkReply = networkAM.get(QNetworkRequest(curUrl));
		QString msg = QString(tr("Download %1 from %2\n")).arg(filename).arg(host);
		setDownloadTitle(msg);
		connect(networkReply, SIGNAL(finished()), this, SLOT(downloadFinished()));
     	connect(networkReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
     	connect(networkReply, SIGNAL(downloadProgress(qint64,qint64)),
     		        this, SLOT(updateDataTransferProgress(qint64,qint64)));
	}
	else if (downloadingFile)
	{
		QString msg = QString(tr("Unable to save the file %1: %2."))
		                     .arg(filename).arg(downloadingFile->errorString());
		warning(msg);

		delete downloadingFile;
		downloadingFile = 0;
		requestAborted = true;
	}
	else
	{
		requestAborted = true;
		badAlloc();
	}
}

void XWApplication::downloadFinished()
{
	if (networkReply)
	{
		if (networkReply->error())
		{
			QString msg = QString(tr("fail to download. canceled download.\n"));
			warning(msg);
			cancelDownload();
		}
		else
		{
			networkReply->deleteLater();
			networkReply = 0;
		}
	}

	if (downloadingFile)
	{
		downloadingFile->flush();
		downloadingFile->close();
	}

	requestAborted = true;
}

void XWApplication::readyRead()
{
	if (downloadingFile)
         downloadingFile->write(networkReply->readAll());
}

void XWApplication::setDownloadTitle(const QString & msg)
{
	showMessage(msg);
}

void XWApplication::updateDataTransferProgress(qint64 readBytes,
	                                           qint64 totalBytes)
{
	static int blen = 0;
	QString msg;
	for (int i = 0; i < blen; i++)
		msg += QString("\b");

	if (!msg.isEmpty())
		showMessage(msg);

	msg = QString("...%1/%2").arg(readBytes).arg(totalBytes);
	blen = msg.length();
	showMessage(msg);
}

void XWApplication::checkLicense()
{
#ifdef XW_BUILD_PS_LIB
	licenseState = APP_STATE_NOLICENSESFILE;
	QString bindir = QCoreApplication::applicationDirPath();
	QString filename = QString("%1/xwtex.dat").arg(bindir);
	QFile file(filename);
	QString hard = devType;
	hard += devSerialNumber;
	if (file.exists())
	{
		licenseState = APP_STATE_BADLICENSES;
		if (file.open(QIODevice::ReadOnly))
		{
			QByteArray line = file.readLine();
			QByteArray chk = file.readLine();
			line = line.trimmed();
			chk = chk.trimmed();
			file.close();
			if (line.length() > 0)
			{
				QString topdir = bindir;
    		int ti = topdir.lastIndexOf(QChar('/'));
    		if (ti > 0)
    			topdir = topdir.left(ti);

				QByteArray key = hard.toAscii();
				XWDes des;
				QByteArray res = des.decrypt(line, key);
				QString filename = QString::fromAscii(res);
				QString path = QString("%1/%2").arg(topdir).arg(filename);
				QFileInfo pfile(path);
				if (pfile.exists())
				{
					XWAes aes;
					QByteArray nchk = aes.decrypt128(chk, key);
					QString dstr = QString::fromUtf8(nchk.data());
					QDate date = QDate::fromString(dstr,"yyyy-MM-dd");
					QDateTime dt = pfile.lastModified();
					QDate dd = dt.date();
					if (date.year() == dd.year() && date.month() == dd.month())
						licenseState = APP_STATE_NORMAL;
				}
			}
		}
	}
#else
	licenseState = APP_STATE_NORMAL;
#endif
}

void XWApplication::initDir()
{
	QString bindir = QCoreApplication::applicationDirPath();
    tmpDir = bindir;
    int ti = bindir.lastIndexOf(QChar('/'));
    if (ti > 0)
    	tmpDir = tmpDir.left(ti);

    QString top = tmpDir;

    qint64 pid = QCoreApplication::applicationPid();
    tmpDir += QString("/tmp/p%1").arg(pid);
    QDir tmpd(tmpDir);
    if (!tmpd.mkpath(tmpDir))
    {
    	tmpDir = QDir::tempPath();
    	tmpDir += QString("/p%1").arg(pid);
    	tmpd.setPath(tmpDir);
    	tmpd.mkpath(tmpDir);
    }

    QCoreApplication::addLibraryPath(bindir);
    QString plugindir = QString("%1/plugins").arg(top);
    QCoreApplication::addLibraryPath(plugindir);
}

QString XWApplication::inputLine()
{
	QTextStream qin(stdin, QFile::ReadOnly);
	QString line = qin.readLine();
	return line;
}

void XWApplication::showMessage(const QString & msg)
{
	QFile file;
	file.open(stderr, QIODevice::WriteOnly);
	puts(&file, msg);
	file.close();
}
