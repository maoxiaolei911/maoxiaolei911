/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QMutex>
#include <QDateTime>
#include <QDir>
#include "XWMailSea.h"

static QMutex mailSeaMutex;

static PathSeaParams mailSeaParams[] = 
{
	{"addrbook", 0, 0, "addrbook", "ADDRBOOKDIR", 0, 0, false, false},
	{"cert", 0, 0, "cert", "CERTDIR", 0, 0, false, false},
	{"history", 0, 0, "history", "HISTORYDIR", 0, 0, false, false},
	{"imap", 0, 0, "imap", "IMAPDIR", 0, 0, false, false},
	{"mailbox", 0, 0, "mailbox", "MAILBOXDIR", 0, 0, false, false},
	{"news", 0, 0, "news", "NEWSDIR", 0, 0, false, false},
	{"prefer", 0, 0, "pref", "PREFDIR", 0, 0, false, false},
	{"tagdb", 0, 0, "tagdb", "TAGDBDIR", 0, 0, false, false},
	{"template", 0, 0, "template", "TEMPLATEDIR", 0, 0, false, false},
	{"tmp", 0, 0, "tmp", "TMPDIR", 0, 0, false, false}
};

XWMailSeaPrivate::XWMailSeaPrivate(QObject * parent)
	:XWPathSea("mail", (int)(XWMailSea::Last), "mail.cnf", "mail.lsr", parent)
{
}

QString XWMailSeaPrivate::initFormat(int format)
{
	FormatInfo * f = &formatInfo[format];
    if (!(f->path.isEmpty()))
        return f->path;
        
    initFormatByParams(format, &mailSeaParams[format]);

    return f->path;
}

static XWMailSeaPrivate * mailSea = 0;

XWMailSea::XWMailSea(QObject * parent)
	:QObject(parent)
{
	if (!mailSea)
		mailSea = new XWMailSeaPrivate;
}

void XWMailSea::addConfig(const QString & key, 
                          const QString & value, 
                          const QString & prog)
{
	mailSea->addConfig(key, value, prog);
}

void XWMailSea::createAllDefaultDir(const QString & topdir)
{
	int format = AddrBook;
	while (format < Last)
	{
		createDefaultDir((FileFormat)format, topdir);
		format++;
	}
}

void XWMailSea::createDefaultDir(FileFormat format,
	                             const QString & topdir)
{
	mailSea->createDefaultDir(&mailSeaParams[format], topdir);
}

QString XWMailSea::getAccountRC()
{
	QString ret = getPrefDir();
	ret += "/accountrc";
	return ret;
}

QString XWMailSea::getAddrBookDir()
{
	return getDir(AddrBook);
}

QString XWMailSea::getAddrBookAttrRC()
{
	QString ret = getAddrBookDir();
	ret += "/attributesrc";
	return ret;
}

QString XWMailSea::getCertDir()
{
	return getDir(Cert);
}

QString XWMailSea::getCommandHistory()
{
	QString ret = getHistoryDir();
	ret += "/commandhistory";
	return ret;
}

QString XWMailSea::getCommonRC()
{
	QString ret = getPrefDir();
	ret += "/postmanrc";
	return ret;
}

QString XWMailSea::getComposeSaveToHistory()
{
	QString ret = getHistoryDir();
	ret += "/composesavetohistory";
	return ret;
}

QStringList XWMailSea::getConfigDir(FileFormat format)
{
	return mailSea->getConfigDir(&mailSeaParams[format]);
}

QString XWMailSea::getCustomHeaderRC()
{
	QString ret = getPrefDir();
	ret += "/customheaderrc";
	return ret;
}

QString XWMailSea::getDispHeaderRC()
{
	QString ret = getPrefDir();
	ret += "/dispheaderrc";
	return ret;
}

QString XWMailSea::getDraftDir(const QString & acname)
{
	QString ret = getMailBoxDir();
	ret += QString("/%1/draft").arg(acname);
	QDir d(ret);
	if (!d.exists())
		d.mkpath(ret);
	return ret;
}

QString XWMailSea::getDraftedFile()
{
	QString ret = getMailBoxDir();
	ret += "/draftedatexit";
	return ret;
}

QFile * XWMailSea::getDraftFile(const QString & acname)
{
	QString prefix = getDraftDir(acname);
	prefix += "/draf";
	return getTmp(prefix);
}

QString XWMailSea::getFilteringRC()
{
	QString ret = getPrefDir();
	ret += "/filteringrc";
	return ret;
}

QString XWMailSea::getFolderItemRC()
{
	QString ret = getPrefDir();
	ret += "/folderitemrc";
	return ret;
}

QString XWMailSea::getFolderList()
{
	QString ret = getPrefDir();
	ret += "/folderlist.xml";
	return ret;
}

QString XWMailSea::getHistoryDir()
{
	return getDir(History);
}

QString XWMailSea::getImapCacheDir()
{
	return getDir(IMAP);
}

QString XWMailSea::getInBoxDir(const QString & acname)
{
	QString ret = getMailBoxDir();
	ret += QString("/%1/inbox").arg(acname);
	QDir d(ret);
	if (!d.exists())
		d.mkpath(ret);
	return ret;
}

QString XWMailSea::getLocalMessageDir()
{
	QString ret = getMailBoxDir();
	ret += "/local";
	QDir d(ret);
	if (!d.exists())
		d.mkpath(ret);
	return ret;
}

QString XWMailSea::getMailBoxDir()
{
	return getDir(MailBox);
}

QString XWMailSea::getMatcherRC()
{
	QString ret = getPrefDir();
	ret += "/matcherrc";
	return ret;
}

QString XWMailSea::getMessageSearchHistory()
{
	QString ret = getHistoryDir();
	ret += "/messagesearchhistory";
	return ret;
}

QFile * XWMailSea::getMessageTmpFile()
{
	QString prefix = getTmpDir();
	prefix += "/tmpmsg";
	return getTmp(prefix);
}

QString XWMailSea::getMimeTmpDir()
{
	QString ret = getTmpDir();
	ret += "/mime";
	QDir d(ret);
	if (!d.exists())
		d.mkpath(ret);
	return ret;
}

QFile * XWMailSea::getMimeTmpFile()
{
	QString prefix = getTmpDir();
	prefix += "/tmpmime";
	return getTmp(prefix);
}

QString XWMailSea::getNewsCacheDir()
{
	return getDir(News);
}

QString XWMailSea::getPrefDir()
{
	return getDir(Pref);
}

QString XWMailSea::getQueueDir(const QString & acname)
{
	QString ret = getMailBoxDir();
	ret += QString("/%1/queue").arg(acname);
	QDir d(ret);
	if (!d.exists())
		d.mkpath(ret);
	return ret;
}

QFile * XWMailSea::getQueueFile(const QString & acname)
{
	QString prefix = getQueueDir(acname);
	prefix += "/que";
	return getTmp(prefix);
}

QString XWMailSea::getQuickSearchHistory()
{
	QString ret = getHistoryDir();
	ret += "/quicksearchhistory";
	return ret;
}

QString XWMailSea::getScoringRC()
{
	QString ret = getPrefDir();
	ret += "/scoringrc";
	return ret;
}

QString XWMailSea::getSentDir(const QString & acname)
{
	QString ret = getMailBoxDir();
	ret += QString("/%1/sent").arg(acname);
	QDir d(ret);
	if (!d.exists())
		d.mkpath(ret);
	return ret;
}

QString XWMailSea::getSummarySearchAdvHistory()
{
	QString ret = getHistoryDir();
	ret += "/summarysearchadvhistory";
	return ret;
}

QString XWMailSea::getSummarySearchBodyHistory()
{
	QString ret = getHistoryDir();
	ret += "/summarysearchbodyhistory";
	return ret;
}

QString XWMailSea::getSummarySearchFromHistory()
{
	QString ret = getHistoryDir();
	ret += "/summarysearchfromhistory";
	return ret;
}

QString XWMailSea::getSummarySearchSubjectHistory()
{
	QString ret = getHistoryDir();
	ret += "/summarysearchsubjecthistory";
	return ret;
}

QString XWMailSea::getSummarySearchToHistory()
{
	QString ret = getHistoryDir();
	ret += "/summarysearchtohistory";
	return ret;
}

QString XWMailSea::getTagDBDir()
{
	return getDir(TagDB);
}

QString XWMailSea::getTagRC()
{
	QString ret = getPrefDir();
	ret += "/tagsrc";
	return ret;
}

QString XWMailSea::getTemplateDir()
{
	return getDir(Template);
}

QString XWMailSea::getTmpDir()
{
	return getDir(Tmp);
}

QFile * XWMailSea::getTmpFile()
{
	QString prefix = getTmpDir();
	prefix += "/tmpfile";
	return getTmp(prefix);
}

QFile * XWMailSea::getTmpNntpFile()
{
	QString prefix = getTmpDir();
	prefix += "/tmpnntp";
	return getTmp(prefix);
}

QStringList XWMailSea::getTopDirs()
{
	return mailSea->getTopDirs();
}

QString XWMailSea::getTrashDir(const QString & acname)
{
	QString ret = getMailBoxDir();
	ret += QString("/%1/trash").arg(acname);
	QDir d(ret);
	if (!d.exists())
		d.mkpath(ret);
	return ret;
}

void XWMailSea::insertFile(const QString & passedfname)
{
	mailSea->insertFile(passedfname);
}

void XWMailSea::quit()
{
	mailSeaMutex.lock();
	if (mailSea)
		delete mailSea;
		
	mailSea = 0;
	mailSeaMutex.unlock();
}

void XWMailSea::removeConfig(const QString & key)
{
	mailSea->removeConfig(key);
}

void XWMailSea::setDirs(FileFormat format, 
                        const QString & dirs)
{
	PathSeaParams * params = &mailSeaParams[format];
	QString tmp = QString::fromAscii(params->envs);
	QStringList envs = tmp.split(";", QString::SkipEmptyParts);
	if (envs.isEmpty())
		return ;
		
	tmp = envs[0];
	
	mailSea->setDirs(tmp, dirs);
}

QString XWMailSea::varExpand(const QString & s)
{
	return mailSea->varExpand(s);
}

QString XWMailSea::varValue(const QString & s)
{
	return mailSea->varValue(s);
}

void XWMailSea::save()
{
	mailSea->save();
}

void XWMailSea::updateDBs()
{
	mailSea->updateDBs();
}

QString XWMailSea::getDir(XWMailSea::FileFormat format)
{
	QStringList tmplist = getConfigDir(format);
	QString ret;
	if (tmplist.isEmpty())
	{
		createDefaultDir(format);
		tmplist = getConfigDir(format);
	}
	
	if (tmplist.isEmpty())
	{
		return ret;
	}
		
	ret = tmplist[0];
	return ret;
}

QFile * XWMailSea::getTmp(const QString & prefix)
{
	QDateTime ct = QDateTime::currentDateTime();
	uint d = ct.toTime_t();
	QString fn = QString("%1%2").arg(prefix).arg(d);
	QFile * ret = new QFile(fn);
	if (!ret->open(QIODevice::ReadWrite))
	{
		delete ret;
		ret = 0;
	}
	return ret;
}

