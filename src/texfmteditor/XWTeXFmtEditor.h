/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXFMTEDITOR_H
#define XWTEXFMTEDITOR_H

#include <QStringList>
#include "XWApplication.h"

class QProgressDialog;
class XWTeXFmtEditorMainWindow;

class XWTeXFmtEditor : public XWApplication
{
	Q_OBJECT
	
public:       
    XWTeXFmtEditor(QCoreApplication * app);
    ~XWTeXFmtEditor(); 
    
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
	XWTeXFmtEditorMainWindow * mainWin;
	QProgressDialog * progressDialog;
};

#endif // XWTEXFMTEDITOR_H

