/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXREADERMAINWINDOW_H
#define XWTEXREADERMAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QUrl>

class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QToolBar;
class QComboBox;
class QSpinBox;
class QIcon;
class QColor;
class QToolButton;
class QDoubleSpinBox;
class XWString;
class XWDoc;
class XWTeXReaderCore;
class XWTeXReaderDev;
class XWOutlineWindow;
class XWTeXConsole;
class XWPDFSearcher;

class XWTeXReaderMainWindow : public QMainWindow
{
	Q_OBJECT
	
public:  
	XWTeXReaderMainWindow();
	~XWTeXReaderMainWindow();
	
	void addToContextMenu(QMenu * menu);
    
    void displayDest(const char *namedDest);
    void displayPage(int pg);
    
    bool isPlaying() {return playing;}
    bool isReading();
    
    bool loadFile(const QString & filename);
    bool loadFmt(const QString & fmt);
    bool loadURL(const QUrl & url, const QString & localname);
    
public slots:
	void insertTermText(const QString & txt);
    
signals:
	void fileDownloaded(const QString & filename);
    
public slots:
	void gotoPage(int idx);
	
	void loadFmt();
	
    void open();
    void openFile(const QString & filename);
    void openNet();
    void openRecent();
    void openURL();
    
    void play();
    void print();
    
    void reading(bool checked);
    void refresh();
    void rotate();
    
    void saveToDvi();
    void saveToImage();
    void saveToPDF();
    void saveToPS();
    void saveToText();
    void setContinuousMode(bool e);
    void setDisplay();
    void setDoubleMode(bool e);
    void setFonts();
    void setFullScreen(bool e);
    void setHoriMode(bool e);
    void setPageStatus(int cur, int num);
    void setRotate(int r);
    void setTexFonts();
    void setTools();
    void setZoomIndex(int idx);
    void showAboutHard();
    void showAboutSoft(); 
    void showFontDef();
    void showFonts();
    void showProductHelp();
    void stop();
    
    void updateShow();
    
    void zoom(int idx);
    
protected:
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *e);
	
	void mousePressEvent(QMouseEvent *e);
    
private:
	QIcon createColorIcon(QColor color);		
	void createEditAction();
	void createFileAction();
	void createHelpAction();
	void createToolsAction();
	void createViewAction();
	
	qint32  findInteract();
	
	QString getLocalPath();
	
	void restoreToolsBarStatus();
	
	void saveRecent();
	void saveToolsBarStatus();
	void setBackground();
	
private slots:
	void setAuthor(const QString & a);
	void setCreationDate(const QString & d);
	void setKeywords(const QString & kws);
	void setLastModifiedDate(const QString & d);
	void setSubject(const QString & s);
	void setTitle(const QString & t);
	void setToolsBarsVisible(bool e);
	void showSearchResult(int pg, double minx, double miny, 
                         double maxx, double maxy);
	
	void updateActions();
	
private:
  XWPDFSearcher * searcher;
	XWTeXReaderDev * dev;
	XWTeXReaderCore * core;
	QDockWidget *dock;
	XWOutlineWindow * outlineWin;
	QDockWidget *termDock;
	XWTeXConsole * termWin;
	
	QToolBar * fileToolsBar;
	QToolBar * saveToolsBar;
	QMenu * fileMenu;
	
	QAction * openAct;
	QAction * openURLAct;	
	QAction * recentFileAct;
	QAction * openNetAct;
	
	QAction * fmtAct;	
	QAction * toDviAct;
	QAction * toPDFAct;
	QAction * toPSAct;	
	QAction * toTextAct;
	QAction * toImageAct;
	
	QAction * fontDefAct;
	
	QAction * printAct;
	QAction * exitAct;
	
	QToolBar * historyToolsBar;
	QToolBar * gotoToolsBar;
	QComboBox * gotoPageCombo;
	QMenu    * editMenu;
	
	QAction * backAct;
	QAction * forwardAct;
	
	QAction * copyAct;
	QAction * gotoNextAct;
	QAction * gotoPrevAct;
	QAction * gotoFirstAct;
	QAction * gotoLastAct;
	QAction * findAct;
	QAction * findNextAct;
	
	QAction * fleAct;
  QAction * haltAct;
  
  QActionGroup * insertActs;
  
  QAction * srcAct;
  QAction * autoAct;
  QAction * crAct;
  QAction * displayAct;
  QAction * hboxAct;
  QAction * mathAct;
  QAction * parAct;
  QAction * parendAct;
  QAction * vboxAct;
	
	QToolBar * zoomToolsBar;
	QComboBox * zoomCombo;
	QToolBar * fitToolsBar;
	QToolBar * rotateToolsBar;
	QSpinBox * rotateBox;
	QToolBar * modeToolsBar;
	QToolBar * screenToolsBar;
	QToolBar * playToolsBar;
	QSpinBox * intervalBox;
	QComboBox * animateCombo;
	
	QMenu * viewMenu;
	QMenu * toolsbarMenu;
	
	QAction * refreshAct;
	
	QAction * zoomInAct;
	QAction * zoomOutAct;
	QAction * fitPageAct;
	QAction * fitWidthAct;
	
	QAction * rotateAct;
	
	QAction * rtlAct;
	QAction * horiAct;
	
	QAction * singlePageAct;
	QAction * continuousPagesAct;
	
	QAction * readingAct;
	QAction * fullScreenAct;
	QAction * playAct;
	
	QToolBar * markToolsBar;
	QToolButton * markToolsButton;
	QMenu * toolsMenu;
	
	QAction * markAct;	
	QAction * markCAct;
	QAction * displaySettingAct;
	QAction * externalToolsAct;
	QAction * fontSettingAct;
	QAction * texFontSettingAct;
	QAction * fontInfoAct;
	
	QMenu * helpMenu;
	QAction * productHelpAct;
	QAction * aboutSoftAct;
	QAction * aboutHardAct;
	
	QLabel * fileLabel;
	QLabel * authorLabel;
	QLabel * creationDateLabel;
	QLabel * lastModifiedDateLabel;
	QLabel * pagesLabel;
	
	QString  fileName;
	QString  localFileName;
	QString  tmpFileName;
	
	bool playing;
	int  topPage;
};

#endif //XWTEXREADERMAINWINDOW_H
