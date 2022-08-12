/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWLATEXEDITORMAINWINDOW_H
#define XWLATEXEDITORMAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QUrl>

class QAction;
class QActionGroup;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QLabel;
class QMenu;
class QToolBar;
class QModelIndex;
class QComboBox;
class QSpinBox;
class QIcon;
class QColor;
class QToolButton;
class QDoubleSpinBox;
class QFileSystemModel;
class QTreeView;
class XWString;
class XWTeXConsole;
class XWLaTeXDocument;
class XWTeXDocumentCore;
class XWTeXEditorDev;
class XWLaTeXEditorCore;
class XWLaTeXEditorDev;
class XWLaTeXSymbolToolBox;
class XWTeXDocSearcher;

class XWLaTeXEditorMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	XWLaTeXEditorMainWindow();
	~XWLaTeXEditorMainWindow();

  bool loadFmt(const QString & fmt);

public slots:
  void editFormular();
	void editTikz();

	void insertTermText(const QString & txt);

signals:
	void fileDownloaded(const QString & filename);

public slots:
  void abstract();
  void address();
	void appendix();
	void array();
	void assumption();
	void author();
	void axiom();

	void build();

  void center();
	void cite();
  void conjecture();
	void corollary();

	void date();
	void definition();
	void description();
	void disformular();

  void eenumerate();
	void eqnarray();
	void eqnarrayStar();
	void equation();
  void example();
	void exercise();

	void flushLeft();
	void flushRight();
	void footnote();
	void formular();

	void glossary();

  void include();
	void includeOnly();
	void includeGraphics();
	void index();
	void input();
	void insertText(const QString & txt);
	void itemize();

  void label();
  void lemma();
	void list();
	bool loadFile(const QString & filename);
	void loadFmt();
	void location();

	void makeTitle();
	void marginpar();

  void name();
	void newFromTmplate();
	void nocite();

  void open();
  void openFile(const QString & filename);
	void openFolder();

  void pageref();
  void picture();
	void printIndex();
	void printGlossary();
	void problem();
	void proof();
	void proposition();
	void pstricksPicture();

	void quote();
	void quotation();

  void ref();
	void remark();

  void save();
	void saveToPicture();
  void saveToDvi();
  void saveToPDF();
  void setActiveSubWindow(QWidget *window);
	void setEditor();
  void setInputCodec(QAction * a);
  void setOutputCodec(QAction * a);
  void setTexFonts();
  void setTools();
  void showAboutHard();
  void showAboutSoft();
  void showFonts();
  void showLaTeXHelp();
  void showPGFHelp();
	void showPreamble();
  void showProductHelp();
  void showPSTHelp();
	void signature();

	void lecture();
	void note();
	void mode();
	void againFrame();

  void tableOfContents();
	void tabular();
	void tabularStar();
	void telephone();
	void text();
	void thanks();
	void thebibliography();
	void theorem();
	void tikz();
	void tikzPicture();
	void title();
	void titlePage();

  void updateMenus();
  void updateShow();
  void updateWindowMenu();
	void usepackage();
	void useTikzLibrary();
	void useTheme();
	void includeOnlyLecture();

  void verbatim();
	void verse();

	void zoom(int idx);

protected:
	void closeEvent(QCloseEvent *event);

private slots:
  void clearMark();

  void openFile(const QModelIndex & index);

	void sections(int index);
	void setClass(int index);
	void setColumn(int index);
	void setDirection(int index);
	void setFont(int index);
	void setMarking(bool e);
	void setMode(int index);
	void setPaper(int index);
	void showFindDialog();
	void showReplaceDialog();
	void showSearchResult(int pg, double minx, double miny, double maxx,double maxy);

	void updateActions();

	void wizard();

private:
	XWTeXDocumentCore *activeMdiChild();

	void createActions();
	XWTeXDocumentCore *createMdiChild(const QString &fileName);
  void createMenus();

	XWLaTeXDocument * currentDoc();

  void findInputCodecName(const QString & name);
  QMdiSubWindow *findMdiChild(const QString &fileName);
  void findOutputCodecName(const QString & name);

  QString getLocalPath();

	void restoreToolsBarStatus();

	void saveToolsBarStatus();
	void setBackground();

private:
  XWTeXDocSearcher * searcher;
	QMdiArea *mdiArea;
  QSignalMapper *windowMapper;
	XWLaTeXEditorDev * dev;
	QDockWidget * folderDock;
	QTreeView * folder;
	QFileSystemModel * folderModel;
	QDockWidget *coreDock;
	XWLaTeXEditorCore * core;
	QDockWidget *termDock;
	XWTeXConsole * termWin;

	QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *viewMenu;
	QMenu *insertMenu;
  QMenu *windowMenu;
  QMenu *toolsMenu;
  QMenu *helpMenu;

  QToolBar *fileToolsBar;
  QToolBar *editToolBar;
  QToolBar *buildToolBar;
	QToolBar * sectionBar;
	QComboBox * sectionCombo;

	QToolBar * docToolsBar;
	QComboBox * classCombo;
	QComboBox * paperCombo;
	QComboBox * fontCombo;
	QComboBox * dirCombo;
	QComboBox * columnCombo;

	QToolBar * viewToolsBar;
	QComboBox * modeCombo;
	QComboBox * zoomCombo;

	QMenu *toolsbarMenu;

	QToolBar * markToolsBar;
	QToolButton * markToolsButton;

  QAction *newWizardAct;
  QAction *importAct;
  QAction *openAct;
	QAction *openFolderAct;
  QAction *saveAct;

	QAction * fmtAct;

  QAction * toPicAct;
	QAction * toDviAct;
	QAction * toPDFAct;

	QAction * exitAct;

	QAction * findAct;
	QAction * replaceAct;

  QActionGroup * inputCodecActs;
  QActionGroup * outputCodecActs;

	QAction * showPreambleAct;

	QAction * usepackageAct;
	QAction * useTikzLibraryAct;
	QAction * useThemeAct;
	QAction * includeLectureAct;
	QAction * addressAct;
	QAction * authorAct;
	QAction * dateAct;
	QAction * includeOnlyAct;
	QAction * locationAct;
	QAction * nameAct;
	QAction * signatureAct;
	QAction * telephoneAct;
	QAction * thanksAct;
	QAction * titleAct;

	QAction * maketitleAct;
	QAction * titlePageAct;
	QAction * tableOfContentsAct;
	QAction * abstractAct;

	QAction * appendixAct;
	QAction * printIndexAct;
	QAction * printGlossaryAct;
	QAction * thebibliographyAct;

	QAction * lectureAct;
	QAction * noteAct;
	QAction * modeAct;
	QAction * againFrameAct;

	QAction * quoteAct;
	QAction * quotationAct;
	QAction * verseAct;
	QAction * theoremAct;
	QAction * proofAct;
	QAction * assumptionAct;
	QAction * definitionAct;
	QAction * propositionAct;
	QAction * lemmaAct;
	QAction * axiomAct;
	QAction * corollaryAct;
	QAction * conjectureAct;
	QAction * exerciseAct;
	QAction * exampleAct;
	QAction * remarkAct;
	QAction * problemAct;

	QAction * centerAct;
	QAction * flushLeftAct;
	QAction * flushRightAct;

	QAction * descriptionAct;
	QAction * eenumerateAct;
	QAction * itemizeAct;
	QAction * listAct;

	QAction * arrayAct;
	QAction * eqnarrayAct;
	QAction * eqnarrayStarAct;
	QAction * equationAct;

	QAction * tabularAct;
	QAction * tabularStarAct;

	QAction * includeGraphicsAct;

	QAction * pictureAct;

	QAction * tikzAct;
	QAction * tikzPictureAct;

	QAction * pstricksPictureAct;

	QAction * verbatimAct;

	QAction * footnoteAct;
	QAction * marginparAct;

	QAction * labelAct;
	QAction * pagerefAct;
	QAction * refAct;

	QAction * indexAct;
	QAction * glossaryAct;

	QAction * nociteAct;
	QAction * citeAct;

	QAction * includeAct;
	QAction * inputAct;

	QAction * textAct;
	QAction * formularAct;
	QAction * disformularAct;

	QAction *buildAct;
  QAction *buildRAct;

	QAction * markAct;
	QAction * markCAct;

  QAction * formularEditAct;
	QAction * tikzEditAct;
	QAction * externalToolsAct;
	QAction * editorSettingAct;
	QAction * texFontSettingAct;
	QAction * fontInfoAct;

	QAction * productHelpAct;
	QAction * latexHelpAct;
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

	int topPage;
};

#endif //XWLATEXEDITORMAINWINDOW_H
