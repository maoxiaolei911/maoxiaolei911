/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWMAILSEA_H
#define XWMAILSEA_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>

#include "XWGlobal.h"
#include "XWPathSea.h"

class XW_UTIL_EXPORT XWMailSea : public QObject
{
	Q_OBJECT
    
public:
	enum FileFormat
	{
		AddrBook,
		Cert,
		History,
		IMAP,
		MailBox,
		News,
		Pref,
		TagDB,
		Template,
		Tmp,
		Last
	};
	
	XWMailSea(QObject * parent = 0);
	
	void addConfig(const QString & key, 
                   const QString & value, 
                   const QString & prog = QString());
	void addFile(const QString & path);
	
	void createAllDefaultDir(const QString & topdir = QString());
	void createDefaultDir(FileFormat format,
	                      const QString & topdir = QString());
	
	QString getAccountRC();
	QString getAddrBookDir();
	QString getAddrBookAttrRC();
	QString getCertDir();
	QString getCommandHistory();
	QString getCommonRC();
	QString getComposeSaveToHistory();
	QStringList getConfigDir(FileFormat format);
	QString getCustomHeaderRC();
	QString getDispHeaderRC();
	QString getDraftDir(const QString & acname);
	QString getDraftedFile();
	QFile * getDraftFile(const QString & acname);
	QString getFilteringRC();
	QString getFolderItemRC();
	QString getFolderList();
	QString getHistoryDir();
	QString getImapCacheDir();
	QString getInBoxDir(const QString & acname);
	QString getLocalMessageDir();
	QString getMailBoxDir();
	QString getMatcherRC();
	QString getMessageSearchHistory();
	QFile * getMessageTmpFile();
	QString getMimeTmpDir();
	QFile * getMimeTmpFile();
	QString getNewsCacheDir();
	QString getPrefDir();
	QString getQueueDir(const QString & acname);
	QFile * getQueueFile(const QString & acname);
	QString getQuickSearchHistory();
	QString getScoringRC();
	QString getSentDir(const QString & acname);
	QString getSummarySearchAdvHistory();
	QString getSummarySearchBodyHistory();
	QString getSummarySearchFromHistory();
	QString getSummarySearchSubjectHistory();
	QString getSummarySearchToHistory();
	QString getTagDBDir();
	QString getTagRC();
	QString getTemplateDir();
	QString getTmpDir();
	QFile * getTmpFile();
	QFile * getTmpNntpFile();
	QStringList getTopDirs();
	QString getTrashDir(const QString & acname);    
	
	void insertFile(const QString & passedfname);
	
	static void quit();
	
	void removeConfig(const QString & key);
	
	void setDirs(FileFormat format, 
                 const QString & dirs = QString());
	
	QString varExpand(const QString & s);
    QString varValue(const QString & s);
    
public slots:
	void save();
	void updateDBs();
    
signals:
	void directoryChangrd(const QString & path);
    
private:
	QString getDir(FileFormat format);
	QFile * getTmp(const QString & prefix);
};

class XWMailSeaPrivate : public XWPathSea
{
	Q_OBJECT
    
public:
	XWMailSeaPrivate(QObject * parent = 0);
	~XWMailSeaPrivate() {}
    
	QString initFormat(int format);
};

#endif //XWMAILSEA_H

