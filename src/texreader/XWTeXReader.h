/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXREADER_H
#define XWTEXREADER_H

#include <QStringList>
#include "XWApplication.h"

class QProgressDialog;
class XWTeXReaderMainWindow;

class XWTeXReader : public XWApplication
{
	Q_OBJECT
	
public:       
    XWTeXReader(QCoreApplication * app);
    ~XWTeXReader(); 
    
    void badAlloc();
    
    void critical(const QString & msg);
    
    void error(const QString & msg);
    
    QString      getFile(const QUrl & url);
    bool         getFile(const QUrl & url, const QString & localname);
		QString      getPassward(const QString & msg, 
	                           const QString & labelA);
	                              
    QString      getProductID();
    QString      getProductName();
    const char * getProductName8();
    bool         getUserAndPassward(const QString & msg,
                                    QString & user, 
                                    QString & passwd);
	QString      getVersion();
	const char * getVersion8();
	
	void message(const QString & msg);
	
	bool openError(const QString & filename, bool w);
	
	void showErrs();
	void showWarns();
	void start(int & argc, char**argv);
	
	void warning(const QString & msg);
	
public slots:
	void cleanup();
	void quit();
    
protected slots:
	void cancelDownload();
	
	void downloadFile(const QString & filename,
	                  const QString & localname);
	                  
	void downloadFinished();
	
	void setDownloadTitle(const QString & msg);
	
	void updateDataTransferProgress(qint64 readBytes, 
	                                qint64 totalBytes);
    
private:
	XWTeXReaderMainWindow * mainWin;
	QProgressDialog * progressDialog;
	
	QStringList errors;
	QStringList warns;
};

#endif // XWTEXREADER_H

