/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXFMTEDITORMAINWINDOW_H
#define XWTEXFMTEDITORMAINWINDOW_H

#include <QMainWindow>
#include <QString>

class QModelIndex;
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QFileSystemModel;
class QTreeView;
class XWTeXTextEdit;
class XWTeXFmtEditorDev;
class XWTeXHighlighter;
class XWTeXCompleter;
class XWTeXConsole;
class XWFindTextDialog;
class XWReplaceDialog;
class XWTextFinder;
class XWLaTeXSymbolToolBox;

class XWTeXFmtEditorMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	XWTeXFmtEditorMainWindow();

public slots:
	void insertTermText(const QString & txt);
  void insertText(const QString & txt);

signals:
	void fileDownloaded(const QString & filename);

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void build();
	void buildR();

	void copy();
	void cut();

  void editFormular();
  void editTikz();

  void find();
  void findInFiles();
  void finishedInFiles();

	void newFile();
	void newFromTmplate();

  void open();
  void openFile(const QString & filename);
	void openFile(const QModelIndex & index);
	void openFolder();

  void paste();

  void replace();
  void replaceInFiles();

  void save();
  void setActiveSubWindow(QWidget *window);
	void setEditor();
  void setInputCodec(QAction * a);
  void setOutputCodec(QAction * a);
  void setSourceLabel(int r, int c);
  void setTexFonts();
  void showAboutHard();
  void showAboutSoft();
  void showFindDialog();
	void showFindInFilesDialog();
	void showReplaceDialog();
	void showReplaceInFilesDialog();
  void showFonts();
  void showProductHelp();
  void switchLayoutDirection();

  void updateMenus();
  void updateWindowMenu();

private:
	XWTeXTextEdit *activeMdiChild();

	void createActions();
	XWTeXTextEdit *createMdiChild();
  void createMenus();
  void createToolBars();

  void findInputCodecName(const QString & name);
  qint32  findInteract();
  QMdiSubWindow *findMdiChild(const QString &fileName);
  void findOutputCodecName(const QString & name);

  QString getLocalPath();

private:
	XWTeXFmtEditorDev * dev;
  QMdiArea *mdiArea;
  QSignalMapper *windowMapper;

	QDockWidget * folderDock;
	QTreeView * folder;
	QFileSystemModel * folderModel;

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
  QToolBar *fileToolBar;
  QToolBar *editToolBar;
  QToolBar *buildToolBar;

  QAction *newAct;
  QAction *importAct;
  QAction *openAct;
	QAction *openFolderAct;
  QAction *saveAct;
  QAction *saveAsAct;
  QAction *exitAct;

  QAction *cutAct;
  QAction *copyAct;
  QAction *pasteAct;
  QAction * findAct;
	QAction * replaceAct;
	QAction * findInAct;
	QAction * replaceInAct;

  QActionGroup * inputCodecActs;
  QActionGroup * outputCodecActs;

  QAction *etexAct;

  QAction *fleAct;
  QAction *halteAct;

  QActionGroup * insertActs;

  QAction *buildAct;
  QAction *buildRAct;

  QAction * formularAct;
  QAction * tikzEditAct;
	QAction * texFontSettingAct;
	QAction * fontInfoAct;
	QAction * editorSettingAct;

  QAction *closeAct;
  QAction *closeAllAct;
  QAction *tileAct;
  QAction *cascadeAct;
  QAction *nextAct;
  QAction *previousAct;
  QAction *separatorAct;

  QAction * productHelpAct;
  QAction * aboutSoftAct;
	QAction * aboutHardAct;

  QLabel * fileLabel;
	QLabel * sourceLabel;
};

#endif //XWTEXFMTEDITORMAINWINDOW_H
