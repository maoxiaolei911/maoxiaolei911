/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWREADERMAINWINDOW_H
#define XWREADERMAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QUrl>

class QAction;
class XWString;
class XWDoc;
class XWReaderCore;
class XWOutlineWindow;
class QLabel;
class QMenu;
class QToolBar;
class QComboBox;
class QSpinBox;
class QIcon;
class QColor;
class QToolButton;
class QDoubleSpinBox;

class XWReaderMainWindow : public QMainWindow
{
	Q_OBJECT
	
public:       
	enum FileType
	{
		FilePDF,
		FileDVI,
		FileMPS,
		FileDVX,
		FileUnknown
	};
	
    XWReaderMainWindow();
    ~XWReaderMainWindow(); 
    
    void addToContextMenu(QMenu * menu);
    
    void displayDest(const char *namedDest);
    void displayPage(int pg);
    
    bool isPlaying() {return playing;}
    bool isReading();
    
    bool loadFile(const QString & filename, bool tmpfile = false);
    bool loadURL(const QUrl & url, const QString & localname);

public slots:
	void extractImages();
	
	void gotoPage(int idx);
	
    void open();
    void openNet();
    void openRecent();
    void openURL();
    
    void play();
    void print();
    
    void reading(bool checked);
    void rotate();
    
    void saveAs();
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
    void showDocFonts();
    void showFontDef();
    void showFonts();
		void showGrid(bool e);
    void showHelp();
		void showRule(bool e);
    void stop();
    
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
	
	QString getLocalPath();
	
	void restoreToolsBarStatus();
	
	void saveRecent();
	void saveToolsBarStatus();
	void setBackground();
	
private slots:
	void setAuthor(const QString & a);
	void setCreationDate(const QString & d);
	void setFileName(const QString & n);
	void setKeywords(const QString & kws);
	void setLastModifiedDate(const QString & d);
	void setSubject(const QString & s);
	void setTitle(const QString & t);
	void setToolsBarsVisible(bool e);
	
	void updateActions();
	
private:
	XWReaderCore * core;
	QDockWidget *dock;
	XWOutlineWindow * outlineWin;
	
	QToolBar * fileToolsBar;
	QToolBar * saveToolsBar;
	QMenu * fileMenu;
	
	QAction * openAct;
	QAction * openURLAct;	
	QAction * recentFileAct;
	QAction * openNetAct;
	
	QAction * saveAsAct;	
	QAction * toPDFAct;
	QAction * toPSAct;	
	QAction * toTextAct;
	QAction * toImageAct;
	QAction * imageAct;
	
	QAction * fontAct;
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
	
	QAction * zoomInAct;
	QAction * zoomOutAct;
	QAction * fitPageAct;
	QAction * fitWidthAct;
	
	QAction * rotateAct;
	
	QAction * rtlAct;
	QAction * horiAct;

	QAction * rulerAct;
	QAction * gridAct;
	
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
	QAction * helpAct;
	QAction * aboutSoftAct;
	QAction * aboutHardAct;
	
	QLabel * fileLabel;
	QLabel * authorLabel;
	QLabel * creationDateLabel;
	QLabel * lastModifiedDateLabel;
	QLabel * pagesLabel;
	
	QString  fileName;
	QString  localFileName;
	FileType fileType;
	
	bool playing;
};

#endif // XWREADERMAINWINDOW_H

