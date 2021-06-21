/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTRAYDIALOG_H
#define XWTRAYDIALOG_H

#include <QDialog>
#include <QHash>
#include <QList>
#include <QString>
#include <QNetworkAccessManager>
#include <QSystemTrayIcon>

class QLabel;
class QProgressBar;
class QTextBrowser;
class QPushButton;
class QFile;
class QMenu;
class QActionGroup;
class QNetworkReply;
class QSettings;

class XWUpdateFile
{
public:
	XWUpdateFile();
	
public:
	bool updated;
	QString id;
	QString package;
	QString directory;
	QString description;
	QString bin;
	QString menu;
	QString fileName;
	QString dest;
};

class XWTrayDialog : public QDialog
{
	Q_OBJECT

public:
	XWTrayDialog(QWidget * parent = 0);
	~XWTrayDialog();
	
	void setVisible(bool visible);
	
protected:
	void closeEvent(QCloseEvent *event);
	
private slots:
	void allFinished();
	
	void checkFinished();
	void checkForUpdate();
	void cleanup();
	
	void createUpdateItems();
	
	void download();
	
	void downloadFinished();
	
	void getFeature();
	
	void readyRead();
	
	void startProgram(QAction * a);
	
	void updateProgress(qint64 bytesReceived,qint64 bytesTotal);
	
private:
	void clearUpdateItems();
	void createActions();
	
	void reply();
	
	bool updateFile(const QString & p, const QString & d);
	
private:
	QLabel * logoLabel;
	QLabel * noteLabel;
	QLabel * totalLabel;
	QLabel * progressLabel;
	QProgressBar * progressBar;
	QPushButton * okButton;
	QTextBrowser * descriptionEdit;
	
	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;
	
	QAction *featureAction;
	QAction *updateAction;
	QAction *quitAction;
	QActionGroup * programGrp;
	
	QString os;
	QString lang;
	QString hardId;
	
	QString binDir;
	QString rootDir;
	QString tmpDir;
	QString orderID;
	
	QString curFile;
	
	int total;
	int remnant;
	int succ;
	
	QFile * output;
	QHash<QString, XWUpdateFile*> updateItems;
	QList<XWUpdateFile*> updatedItems;
	
	QNetworkReply * networkReply;
	QNetworkAccessManager manager;
	
	QSettings * setting;
};

#endif //XWTRAYDIALOG_H
