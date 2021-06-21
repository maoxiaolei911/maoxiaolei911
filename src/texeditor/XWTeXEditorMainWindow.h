/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXEDITORMAINWINDOW_H
#define XWTEXEDITORMAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QUrl>

class QModelIndex;
class QAction;
class QActionGroup;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QLabel;
class QMenu;
class QToolBar;
class QComboBox;
class QSpinBox;
class QIcon;
class QColor;
class QToolButton;
class QDoubleSpinBox;
class QFileSystemModel;
class QTreeView;
class XWString;
class XWDoc;
class XWTeXEditorCore;
class XWTeXEditorDev;
class XWTeXTextEdit;
class XWOutlineWindow;
class XWTeXHighlighter;
class XWTeXCompleter;
class XWTeXConsole;
class XWFindTextDialog;
class XWReplaceDialog;
class XWTextFinder;
class XWLaTeXSymbolToolBox;

class XWTeXEditorMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	XWTeXEditorMainWindow();
	~XWTeXEditorMainWindow();

	void addToContextMenu(QMenu * menu);

    void displayDest(const char *namedDest);
    void displayPage(int pg);

    bool loadFile(const QString & filename);
    bool loadFmt(const QString & fmt);

public slots:
	void insertTermText(const QString & txt);

signals:
	void fileDownloaded(const QString & filename);

public slots:
	void build();
	void buildR();
	void copy();
	void cut();

	void editFormular();
	void editTikz();

	void gotoPage(int idx);

	void insertText(const QString & txt);

	void loadFmt();

	void newFile();
	void newFromTmplate();

    void open();
    void openFile(const QString & filename);
		void openFolder();

    void paste();

    void save();
    void saveToDvi();
    void saveToImage();
    void saveToPDF();
    void saveToPS();
    void saveToText();
    void setActiveSubWindow(QWidget *window);
    void setContinuousMode(bool e);
    void setDoubleMode(bool e);
		void setEditor();
    void setFonts();
    void setHoriMode(bool e);
    void setInputCodec(QAction * a);
  	void setOutputCodec(QAction * a);
    void setPageStatus(int cur, int num);
    void setTexFonts();
    void setTools();
    void setZoomIndex(int idx);
    void showAboutHard();
    void showAboutSoft();
    void showFontDef();
    void showFonts();
    void showBeamerHelp();
    void showLaTeXHelp();
    void showPGFHelp();
    void showProductHelp();
    void showPSTHelp();
    void switchLayoutDirection();

    void zoom(int idx);

    void updateMenus();
    void updateShow();
  	void updateWindowMenu();

protected:
	void closeEvent(QCloseEvent *event);

private slots:
  void find();
	void findInFiles();
	void finishedInFiles();

  void openFile(const QModelIndex & index);

	void replace();
	void replaceInFiles();

	void setAuthor(const QString & a);
	void setCreationDate(const QString & d);
	void setKeywords(const QString & kws);
	void setLastModifiedDate(const QString & d);
	void setSourceLabel(int r, int c);
	void setSubject(const QString & s);
	void setTitle(const QString & t);
	void setToolsBarsVisible(bool e);
	void showFindDialog();
	void showFindInFilesDialog();
	void showReplaceDialog();
	void showReplaceInFilesDialog();

	void updateActions();

private:
	XWTeXTextEdit *activeMdiChild();

	void createActions();
	XWTeXTextEdit *createMdiChild();
  void createMenus();

  void findInputCodecName(const QString & name);
  qint32  findInteract();
  QMdiSubWindow *findMdiChild(const QString &fileName);
  void findOutputCodecName(const QString & name);

  QString getLocalPath();

	void restoreToolsBarStatus();

	void saveToolsBarStatus();
	void setBackground();

private:
	QMdiArea *mdiArea;
  QSignalMapper *windowMapper;
	XWTeXEditorDev * dev;
	QDockWidget * folderDock;
	QTreeView * folder;
	QFileSystemModel * folderModel;
	QDockWidget *coreDock;
	XWTeXEditorCore * core;
	QDockWidget *outlineDock;
	XWOutlineWindow * outlineWin;
	QDockWidget *termDock;
	XWTeXConsole * termWin;

  XWFindTextDialog * findDialog;
  XWReplaceDialog  * replaceDialog;
  XWTextFinder * finder;

	QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *viewMenu;
  QMenu *windowMenu;
  QMenu *toolsMenu;
  QMenu *helpMenu;
  QToolBar *fileToolsBar;
  QToolBar *saveToolsBar;
  QToolBar *editToolBar;
  QToolBar *buildToolBar;
  QToolBar * gotoToolsBar;
	QComboBox * gotoPageCombo;
	QToolBar * zoomToolsBar;
	QComboBox * zoomCombo;
	QToolBar * fitToolsBar;
	QToolBar * modeToolsBar;
	QToolBar * markToolsBar;
	QToolButton * markToolsButton;
	QMenu *toolsbarMenu;

  QAction *newAct;
  QAction *importAct;
  QAction *openAct;
	QAction *openFolderAct;
  QAction *saveAct;
  QAction *saveAsAct;

	QAction * fmtAct;

	QAction * toDviAct;
	QAction * toPDFAct;
	QAction * toPSAct;
	QAction * toTextAct;
	QAction * toImageAct;

	QAction * fontDefAct;

	QAction * exitAct;

	QAction *cutAct;
  QAction *copyAct;
  QAction *pasteAct;
	QAction * findAct;
	QAction * replaceAct;
	QAction * findInAct;
	QAction * replaceInAct;

  QActionGroup * inputCodecActs;
  QActionGroup * outputCodecActs;

  QAction * debugAct;
  QAction * fleAct;
  QAction * haltAct;
  QAction * shellAct;
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

	QAction * gotoNextAct;
	QAction * gotoPrevAct;
	QAction * gotoFirstAct;
	QAction * gotoLastAct;

	QAction *buildAct;
  QAction *buildRAct;

	QAction * zoomInAct;
	QAction * zoomOutAct;
	QAction * fitPageAct;
	QAction * fitWidthAct;

	QAction * rtlAct;
	QAction * horiAct;

	QAction * singlePageAct;
	QAction * continuousPagesAct;

  QAction * formularAct;
	QAction * tikzEditAct;
	QAction * markAct;
	QAction * markCAct;
	QAction * externalToolsAct;
	QAction * fontSettingAct;
	QAction * texFontSettingAct;
	QAction * fontInfoAct;
	QAction * editorSettingAct;

	QAction * productHelpAct;
	QAction * latexHelpAct;
	QAction * beamerHelpAct;
	QAction * pgfHelpAct;
	QAction * pstHelpAct;
	QAction * aboutSoftAct;
	QAction * aboutHardAct;

	QAction *closeAct;
  QAction *closeAllAct;
  QAction *tileAct;
  QAction *cascadeAct;
  QAction *nextAct;
  QAction *previousAct;
  QAction *separatorAct;

	QLabel * fileLabel;
	QLabel * authorLabel;
	QLabel * creationDateLabel;
	QLabel * lastModifiedDateLabel;
	QLabel * pagesLabel;

	QLabel * sourceLabel;

	QString  mainInputName;
	int topPage;
};

#endif //XWTEXEDITORMAINWINDOW_H
