/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWAPPLICATION_H
#define XWAPPLICATION_H

#include <QObject>
#include <QString>
#include <QtNetwork>
#include <QUrl>
#include <QHash>
#include <QVariant>
#include <QDateTime>

#include "XWGlobal.h"

#define xwApp XWApplication::instance()

#define APP_STATE_NOLICENSESFILE  -4
#define APP_STATE_BADLICENSES     -3
#define APP_STATE_ERRMEDIUM       -2
#define APP_STATE_OUTOFDATE       -1
#define APP_STATE_TRY              0
#define APP_STATE_NORMAL           1

class QCoreApplication;
class QAuthenticator;
class QNetworkReply;
class QFile;
class QSettings;

class XW_UTIL_EXPORT XWApplication : public QObject
{
    Q_OBJECT

public:
    XWApplication(QCoreApplication * app);
    virtual ~XWApplication();

    QString aboutHardware();

    virtual void badAlloc();

    virtual void clearTmpFiles();
    virtual void critical(const QString & msg);

    virtual void error(const QString & msg);

    virtual QString getCopyRight();
    virtual const char * getCopyRight8();
    		QString   getDevType() {return devType;}
    		QString   getDevSerialNumber() {return devSerialNumber;}
    virtual QString   getFile(const QString & url);
    virtual QString   getFile(const QUrl & url);
    virtual bool      getFile(const QUrl & url, const QString & localname);
    virtual	void getHelp(const QString & prefix);
    		int           getLicenseState() {return licenseState;}

    virtual QString getPassward(const QString & msg,
	                              const QString & labelA);
    virtual QString getProductID();
    virtual QString getProductName();
    virtual const char * getProductName8();
    QSettings * getSettings();
    virtual QString getTmpDir() {return tmpDir;}
    virtual QString getTmpFile(const QString & filename = QString());
    virtual bool    getUserAndPassward(const QString & msg,
                                       QString & user,
                                       QString & passwd);

	QVariant getSetting(const QString & key);
	virtual QString getVersion();
	virtual const char * getVersion8();

	bool hasSetting(const QString & key);

    void installTranslator(const QString & trans);

    static XWApplication * instance() {return _instance;}

    virtual void message(const QString & msg);

    virtual bool openError(const QString & filename, bool w);
    virtual QFile * openTmpFile(const QString & filename = QString());

            void saveSetting(const QString & key, const QVariant & value);
		virtual void showErrs() {}
		virtual void showWarns() {}
    virtual void start(int & argc, char**argv) = 0;

    virtual void warning(const QString & msg);

public slots:
	virtual void cleanup();
	virtual void quit();

protected slots:
	virtual void authenticationRequired(QNetworkReply*,QAuthenticator *);

	virtual void cancelDownload();

	virtual void downloadFile(const QString & filename,
	                          const QString & localname);
	virtual void downloadFinished();

	virtual void readyRead();

	virtual void setDownloadTitle(const QString & msg);

	virtual void updateDataTransferProgress(qint64 readBytes,
	                                        qint64 totalBytes);

protected:
	virtual void checkLicense();

protected:
	QCoreApplication * realApp;
	QNetworkReply *networkReply;
	QFile * downloadingFile;
	bool requestAborted;

	QString tmpDir;
	QUrl curUrl;
	QNetworkAccessManager networkAM;

	QHash<QString, QString> downloadedFiles;

	QSettings * settings;

	int licenseState;
	QString devType;
	QString devSerialNumber;
	QDateTime startDate;
	QDateTime endDate;
	int fontCnt;

	static XWApplication * _instance;

private:
	void    initDir();
	QString inputLine();

	void showMessage(const QString & msg);
};

#endif // XWAPPLICATION_H
