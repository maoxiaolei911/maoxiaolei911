/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QCoreApplication>
#include <QTextCodec>
#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QLocale>
#include <QProcess>
#include <QtGui>
#include "XWApplication.h"
#include "XWHelpSea.h"
#include "XWTexFontWindow.h"
#include "XWFontInfoWindow.h"
#include "XWTeXHighlighter.h"
#include "XWTeXTextEdit.h"
#include "XWTexEditSetting.h"
#include "XWTeXConsole.h"
#include "XWTexFormatWindow.h"
#include "XWTeXFmtEditorDev.h"
#include "XWTeXDocTemplateWindow.h"
#include "XWFindTextDialog.h"
#include "XWReplaceDialog.h"
#include "XWLaTeXFormularMainWindow.h"
#include "XWTikzMainWindow.h"
#include "XWSearcher.h"
#include "XWDocSearchWidget.h"
#include "XWTeXFmtEditorMainWindow.h"

XWTeXFmtEditorMainWindow::XWTeXFmtEditorMainWindow()
{
	searcher = new XWQTextDocumentSearcher(this);
	QString title = xwApp->getProductName();
	setWindowTitle(title);
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

	folderDock = new QDockWidget(tr("Folder"),this);
	folderDock->setAllowedAreas(Qt::LeftDockWidgetArea);
	addDockWidget(Qt::LeftDockWidgetArea, folderDock);
	folderDock->toggleViewAction()->setChecked(false);

	QTabWidget * tab = new QTabWidget(folderDock);
	folderDock->setWidget(tab);

	folder = new QTreeView(this);
	tab->addTab(folder,tr("Folder"));
	folderModel = new QFileSystemModel;
	folderModel->setRootPath(QDir::currentPath());
	folder->setModel(folderModel);
	
	XWQTextDocumentSearchWidget * sw = new XWQTextDocumentSearchWidget(searcher,tab);
	tab->addTab(sw,tr("Search"));
	connect(sw, SIGNAL(positionActivated(int, int)), this, SLOT(showSearchResult(int, int)));
	connect(sw, SIGNAL(fileActivated(const QString&)), this, SLOT(activeFile(const QString&)));

	XWQTextDocumentReplaceWidget * rw = new XWQTextDocumentReplaceWidget(searcher,tab);
	tab->addTab(rw,tr("Replace"));
	connect(rw, SIGNAL(positionActivated(int, int)), this, SLOT(showSearchResult(int, int)));
	connect(rw, SIGNAL(fileActivated(const QString&)), this, SLOT(activeFile(const QString&)));

	mdiArea = new QMdiArea;
  mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setCentralWidget(mdiArea);

  windowMapper = new QSignalMapper(this);

  termDock = new QDockWidget(tr("Term out"), this);
  termDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

  termWin = new XWTeXConsole(this);
  termDock->setWidget(termWin);
  addDockWidget(Qt::BottomDockWidgetArea, termDock);
  termDock->toggleViewAction()->setChecked(false);

  dev = new XWTeXFmtEditorDev(this);

  findDialog = new XWFindTextDialog(this);
	replaceDialog = new XWReplaceDialog(this);
  
	fileLabel = new QLabel(tr("File: "), this);
  QPalette newPalette = fileLabel->palette();
	newPalette.setBrush(QPalette::WindowText, Qt::black);
	fileLabel->setPalette(newPalette);
  statusBar()->addWidget(fileLabel, 1);

  sourceLabel = new QLabel(tr("Row 0  Column 0"), this);
  sourceLabel->setPalette(newPalette);
  statusBar()->addWidget(sourceLabel, 1);

  createActions();
  createMenus();
  createToolBars();
  viewMenu->addAction(fileToolBar->toggleViewAction());
  viewMenu->addAction(editToolBar->toggleViewAction());
  viewMenu->addAction(buildToolBar->toggleViewAction());
  viewMenu->addSeparator();
  viewMenu->addAction(termDock->toggleViewAction());
	viewMenu->addAction(folderDock->toggleViewAction());

  updateMenus();

	connect(folder, SIGNAL(activated(const QModelIndex &)), this, SLOT(openFile(const QModelIndex &)));
	connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
	connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));
	connect(dev, SIGNAL(messageAdded(const QString &)), this, SLOT(insertTermText(const QString &)));
  connect(dev, SIGNAL(requestInput()), termWin, SLOT(requstInput()));
  connect(termWin, SIGNAL(inputFinished(const QString &)), dev, SLOT(setInputText(const QString &)));
  connect(dev, SIGNAL(needFile(const QString &)), this, SLOT(openFile(const QString &)));
	connect(this, SIGNAL(fileDownloaded(const QString &)), dev, SLOT(setInputText(const QString &)));

	connect(findDialog, SIGNAL(findClicked()), this, SLOT(find()));
	connect(findDialog, SIGNAL(editingFinished()), this, SLOT(find()));
	connect(replaceDialog, SIGNAL(replaceClicked()), this, SLOT(replace()));
}

void XWTeXFmtEditorMainWindow::insertTermText(const QString & txt)
{
	termWin->insertPlainText(txt);
	termWin->ensureCursorVisible();
}

void XWTeXFmtEditorMainWindow::insertText(const QString & txt)
{
	if (activeMdiChild())
    activeMdiChild()->insertPlainText(txt);
}

void XWTeXFmtEditorMainWindow::closeEvent(QCloseEvent *event)
{
	xwApp->cleanup();
	QMainWindow::closeEvent(event);
}

void XWTeXFmtEditorMainWindow::activeFile(const QString & filename)
{
	QMdiSubWindow *existing = findMdiChild(filename);
	if (existing)
	{
     mdiArea->setActiveSubWindow(existing);
     return;
  }

	XWTeXTextEdit *child = createMdiChild();
  child->loadFile(filename);
  child->show();
}

void XWTeXFmtEditorMainWindow::build()
{
	XWTexFormatWindow d(this);
	QString fmt = dev->getJobName();
	d.setCurrentFmt(fmt);
	if (d.exec() == QDialog::Accepted)
	{
		fmt = d.getFmt();
		if (!fmt.isEmpty())
		{
			dev->setInteractionOption(findInteract());
			dev->setFileLineErrorStyle(fleAct->isChecked());
			dev->setHaltOnError(halteAct->isChecked());
			termWin->clear();
			dev->buildFmt(fmt, etexAct->isChecked());
		}
	}
}

void XWTeXFmtEditorMainWindow::buildR()
{
	QString fmt = dev->getJobName();
	if (fmt.isEmpty())
		build();
	else
	{
		dev->setInteractionOption(findInteract());
		dev->setFileLineErrorStyle(fleAct->isChecked());
		dev->setHaltOnError(halteAct->isChecked());
		termWin->clear();
		dev->buildFmt(fmt, etexAct->isChecked());
	}
}

void XWTeXFmtEditorMainWindow::copy()
{
	if (activeMdiChild())
    activeMdiChild()->copy();
}

void XWTeXFmtEditorMainWindow::cut()
{
	if (activeMdiChild())
    activeMdiChild()->cut();
}

void XWTeXFmtEditorMainWindow::editFormular()
{
	XWLaTeXFormularMainWindow * fedit = new XWLaTeXFormularMainWindow;
	XWTeXTextEdit * cur = activeMdiChild();
	if (cur)
	{
		QString str = cur->getFormular();
		if (!str.isEmpty())
		  fedit->setFormular(str);

		connect(fedit, SIGNAL(insert(const QString & )), cur, SLOT(insertFormular(const QString & )));
	}
	fedit->show();
}

void XWTeXFmtEditorMainWindow::editTikz()
{
	XWTikzMainWindow * tedit = new XWTikzMainWindow;
	XWTeXTextEdit * cur = activeMdiChild();
	if (cur)
	{
		QString str = cur->getTikzPicture();
		if (!str.isEmpty())
		  tedit->setText(str);

		connect(tedit, SIGNAL(insert(const QString & )), cur, SLOT(insertTikz(const QString & )));
	}

	tedit->show();
}

void XWTeXFmtEditorMainWindow::find()
{
  XWTeXTextEdit *cur = activeMdiChild();
	if (!cur)
	  return ;

	QString str = findDialog->getText();
	if (str.isEmpty())
	  return ;

	bool casematch = findDialog->isCaseSensitive();
	bool wholewords = findDialog->isWholeWords();
	bool regexpmatch = findDialog->isRegexpMatch();
	cur->find(str,casematch,wholewords,regexpmatch);
	cur->findNext();
}

void XWTeXFmtEditorMainWindow::newFile()
{
	QString path = getLocalPath();
	QString files = tr("Files (*.*)");
	QString filename = QFileDialog::getSaveFileName(this, tr("Save as"), path, files);
	if (filename.isEmpty())
		return ;

	QMdiSubWindow *existing = findMdiChild(filename);
	if (existing)
	{
     mdiArea->setActiveSubWindow(existing);
     return;
  }

	XWTeXTextEdit *child = createMdiChild();
  child->setFileName(filename);
  child->show();
}

void XWTeXFmtEditorMainWindow::newFromTmplate()
{
	XWTeXDocTemplateWindow dlg(this);
	if (dlg.exec() != QDialog::Accepted)
		return ;

	QString filename = dlg.getFileName();
	if (filename.isEmpty())
		return ;

	QMdiSubWindow *existing = findMdiChild(filename);
	if (existing)
	{
     mdiArea->setActiveSubWindow(existing);
     return;
  }

	XWTeXTextEdit *child = createMdiChild();
  child->loadFile(filename);
  child->show();
}

void XWTeXFmtEditorMainWindow::open()
{
	QString path = getLocalPath();
	QString filename = QFileDialog::getOpenFileName(this,
		                                            tr("Open file"),
		                                            path,
		                                            tr("Files (*.*)"));
	if (filename.isEmpty())
		return ;

	int i = filename.lastIndexOf(QChar('/'));
	if (i > 0)
	{
		QString p = filename.left(i);
		searcher->setPath(p);
	}

	QMdiSubWindow *existing = findMdiChild(filename);
	if (existing)
	{
     mdiArea->setActiveSubWindow(existing);
     return;
  }

	XWTeXTextEdit *child = createMdiChild();
  child->loadFile(filename);
  child->show();
}

void XWTeXFmtEditorMainWindow::openFile(const QString & filename)
{
	QString ret = xwApp->getFile(filename);
	emit fileDownloaded(ret);
}

void XWTeXFmtEditorMainWindow::openFile(const QModelIndex & index)
{
	QString filename = folderModel->filePath(index);
	int i = filename.lastIndexOf(QChar('/'));
	if (i > 0)
	{
		QString p = filename.left(i);
		searcher->setPath(p);
	}

	QMdiSubWindow *existing = findMdiChild(filename);
	if (existing)
	{
     mdiArea->setActiveSubWindow(existing);
     return;
  }

	XWTeXTextEdit *child = createMdiChild();
  child->loadFile(filename);
  child->show();
}

void XWTeXFmtEditorMainWindow::openFolder()
{
	QString path = getLocalPath();
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 path,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
  if (dir.isEmpty())
	  return ;
	searcher->setRoot(dir);
	folderModel->setRootPath(dir);
	folder->setRootIndex(folderModel->index(dir));
	QString wintitle = QString(tr("Folder %1")).arg(dir);
	folder->setWindowTitle(wintitle);
}

void XWTeXFmtEditorMainWindow::paste()
{
	if (activeMdiChild())
    activeMdiChild()->paste();
}

void XWTeXFmtEditorMainWindow::replace()
{
  XWTeXTextEdit *cur = activeMdiChild();
	if (!cur)
	  return ;
	
	QString str = replaceDialog->getText();
	if (str.isEmpty())
	  return ;
	
	QString bystr = replaceDialog->getByText();
	bool casematch = replaceDialog->isCaseSensitive();
	bool wholewords = replaceDialog->isWholeWords();
	bool regexpmatch = replaceDialog->isRegexpMatch();
	cur->replace(str,bystr,casematch,wholewords,regexpmatch);
	cur->replaceNext();
}

void XWTeXFmtEditorMainWindow::save()
{
	if (activeMdiChild())
    activeMdiChild()->save();
}

void XWTeXFmtEditorMainWindow::setActiveSubWindow(QWidget *window)
{
	if (!window)
     return;
  mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void XWTeXFmtEditorMainWindow::setEditor()
{
	XWTexEditSetting d;
	d.exec();
}
void XWTeXFmtEditorMainWindow::setInputCodec(QAction * a)
{
	if (!a)
		return ;

	QString n = a->text();
	if (activeMdiChild())
    activeMdiChild()->setInputCodec(n);
}

void XWTeXFmtEditorMainWindow::setOutputCodec(QAction * a)
{
	if (!a)
		return ;

	QString n = a->text();
	if (activeMdiChild())
    activeMdiChild()->setOutputCodec(n);
}

void XWTeXFmtEditorMainWindow::setSourceLabel(int r, int c)
{
	QString s = QString(tr("Row %1  Column %2")).arg(r).arg(c);
	sourceLabel->setText(s);
}

void XWTeXFmtEditorMainWindow::setTexFonts()
{
	XWTexFontWindow win(this);
	win.exec();
}

void XWTeXFmtEditorMainWindow::showAboutHard()
{
	QMessageBox msgBox(this);
	msgBox.setWindowTitle(tr("About media"));
	QString h = xwApp->aboutHardware();
	msgBox.setText(h);
	QPixmap pm(QLatin1String(":/images/xiuwen64.png"));
  if (!pm.isNull())
     msgBox.setIconPixmap(pm);
  msgBox.exec();
}

void XWTeXFmtEditorMainWindow::showAboutSoft()
{
	QString product = xwApp->getProductName();
	QString version = xwApp->getVersion();
	QString text = QString(tr("<h3>%1 %2</h3>"
	                         "<p>%3 is a TeX format IDE.</p>")).arg(product).arg(version).arg(product);
	QString infomation = QString(tr("<p>Please visit <a href=\"http://www.xwtex.com\">xiuwen</a> for all products and services.</p>"));
	QMessageBox msgBox(this);
	msgBox.setWindowTitle(product);
	msgBox.setText(text);
	msgBox.setInformativeText(infomation);
	QPixmap pm(QLatin1String(":/images/tex64.png"));
  if (!pm.isNull())
     msgBox.setIconPixmap(pm);
  msgBox.exec();
}

void XWTeXFmtEditorMainWindow::showFindDialog()
{
	XWTeXTextEdit *cur = activeMdiChild();
	if (!cur)
	  return ;

	if (cur->hasSelected())
	{
		QString str = cur->getSelected();
		findDialog->setText(str);

	}
	findDialog->show();
}

void XWTeXFmtEditorMainWindow::showReplaceDialog()
{
	XWTeXTextEdit *cur = activeMdiChild();
	if (!cur)
	  return ;

	if (cur->hasSelected())
	{
		QString str = cur->getSelected();
		replaceDialog->setText(str);

	}
	replaceDialog->show();
}

void XWTeXFmtEditorMainWindow::showFonts()
{
	XWFontInfoWindow win(this);
	win.exec();
}

void XWTeXFmtEditorMainWindow::showProductHelp()
{
	xwApp->getHelp("xwtex");
}

void XWTeXFmtEditorMainWindow::showSearchResult(int pos, int len)
{
	XWTeXTextEdit *cur = activeMdiChild();
	if (!cur)
	  return ;

	cur->setSelected(pos, len);
}

void XWTeXFmtEditorMainWindow::switchLayoutDirection()
{
	if (layoutDirection() == Qt::LeftToRight)
     qApp->setLayoutDirection(Qt::RightToLeft);
  else
     qApp->setLayoutDirection(Qt::LeftToRight);
}

void XWTeXFmtEditorMainWindow::updateMenus()
{
	XWTeXTextEdit *  child = activeMdiChild();
	bool hasMdiChild = (child != 0);
  saveAct->setEnabled(hasMdiChild);
  pasteAct->setEnabled(hasMdiChild);
  closeAct->setEnabled(hasMdiChild);
  closeAllAct->setEnabled(hasMdiChild);
  tileAct->setEnabled(hasMdiChild);
  cascadeAct->setEnabled(hasMdiChild);
  nextAct->setEnabled(hasMdiChild);
  previousAct->setEnabled(hasMdiChild);
  separatorAct->setVisible(hasMdiChild);

  bool hasSelection = (child && child->textCursor().hasSelection());
  cutAct->setEnabled(hasSelection);
  copyAct->setEnabled(hasSelection);
  if (child)
  {
  	QString file = child->getFileName();
  	QString l = QString(tr("File: %1")).arg(file);
  	fileLabel->setText(l);
  	QString t = child->getInputCodecName();
  	findInputCodecName(t);
  	t = child->getOutputCodecName();
  	findOutputCodecName(t);
  }

	findAct->setEnabled(hasMdiChild);
	replaceAct->setEnabled(hasMdiChild);
}

void XWTeXFmtEditorMainWindow::updateWindowMenu()
{
	windowMenu->clear();
  windowMenu->addAction(closeAct);
  windowMenu->addAction(closeAllAct);
  windowMenu->addSeparator();
  windowMenu->addAction(tileAct);
  windowMenu->addAction(cascadeAct);
  windowMenu->addSeparator();
  windowMenu->addAction(nextAct);
  windowMenu->addAction(previousAct);
  windowMenu->addAction(separatorAct);
  QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
  separatorAct->setVisible(!windows.isEmpty());
  for (int i = 0; i < windows.size(); ++i)
  {
  	XWTeXTextEdit *child = qobject_cast<XWTeXTextEdit *>(windows.at(i)->widget());
  	QString t = child->getFileName();
  	QString text;
  	if (i < 9)
       text = tr("&%1 %2").arg(i + 1).arg(t);
    else
       text = tr("%1 %2").arg(i + 1).arg(t);
    QAction *action  = windowMenu->addAction(text);
    action->setCheckable(true);
    action ->setChecked(child == activeMdiChild());
    connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
    windowMapper->setMapping(action, windows.at(i));
  }
}

XWTeXTextEdit * XWTeXFmtEditorMainWindow::activeMdiChild()
{
	if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
	{
		XWTeXTextEdit *ret = qobject_cast<XWTeXTextEdit *>(activeSubWindow->widget());
		searcher->setDoc(ret->document());
		return ret;
	}

  return 0;
}

void XWTeXFmtEditorMainWindow::createActions()
{
	newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
  newAct->setShortcuts(QKeySequence::New);
  newAct->setStatusTip(tr("Create a new file"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

  importAct = new QAction(QIcon(":/images/import.png"), tr("&New from template"), this);
  importAct->setStatusTip(tr("Create a new file from template"));
  connect(importAct, SIGNAL(triggered()), this, SLOT(newFromTmplate()));

  openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	openFolderAct = new QAction(QIcon(":/images/open.png"), tr("Open folder"), this);
	openFolderAct->setStatusTip(tr("Open an existing directory"));
  connect(openFolderAct, SIGNAL(triggered()), this, SLOT(openFolder()));

  saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save the document to disk"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcuts(QKeySequence::Quit);
  exitAct->setStatusTip(tr("Exit the application"));
  connect(exitAct, SIGNAL(triggered()), xwApp, SLOT(quit()));

  cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
  cutAct->setShortcuts(QKeySequence::Cut);
  cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                             "clipboard"));
  connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

  copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
  copyAct->setShortcuts(QKeySequence::Copy);
  copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                              "clipboard"));
  connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

  pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
  pasteAct->setShortcuts(QKeySequence::Paste);
  pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                             "selection"));
  connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

	findAct = new QAction(tr("&Find"), this);
	findAct->setShortcuts(QKeySequence::Find);
	connect(findAct, SIGNAL(triggered()), this, SLOT(showFindDialog()));

	replaceAct = new QAction(tr("&Replace"), this);
	replaceAct->setShortcuts(QKeySequence::Replace);
	connect(replaceAct, SIGNAL(triggered()), this, SLOT(showReplaceDialog()));

  inputCodecActs = new QActionGroup(this);
  connect(inputCodecActs, SIGNAL(triggered(QAction*)), this, SLOT(setInputCodec(QAction*)));
  outputCodecActs = new QActionGroup(this);
  connect(outputCodecActs, SIGNAL(triggered(QAction*)), this, SLOT(setOutputCodec(QAction*)));
  foreach (int mib, QTextCodec::availableMibs())
  {
  	QTextCodec *codec = QTextCodec::codecForMib(mib);
  	QString text = tr("%1").arg(QString(codec->name()));
  	QAction *action = new QAction(text, this);
  	action->setCheckable(true);
  	action->setChecked(false);

  	inputCodecActs->addAction(action);
  	action = new QAction(text, this);
  	action->setCheckable(true);
  	action->setChecked(false);
  	outputCodecActs->addAction(action);
  }

  etexAct = new QAction(tr("Extended mode"), this);
  etexAct->setCheckable(true);
  etexAct->setChecked(true);

  fleAct = new QAction(tr("File line error style"), this);
  fleAct->setCheckable(true);
  fleAct->setChecked(true);

  halteAct = new QAction(tr("Halt on error"), this);
  halteAct->setCheckable(true);
  halteAct->setChecked(false);

  insertActs = new QActionGroup(this);
  QAction *a = new QAction(tr("Batch"), this);
  a->setData(0);
  a->setCheckable(true);
  a->setChecked(false);
  insertActs->addAction(a);
  a = new QAction(tr("Non stop"), this);
  a->setData(1);
  a->setCheckable(true);
  a->setChecked(false);
  insertActs->addAction(a);
  a = new QAction(tr("Scroll"), this);
  a->setData(2);
  a->setCheckable(true);
  a->setChecked(true);
  insertActs->addAction(a);
  a = new QAction(tr("Error stop"), this);
  a->setData(3);
  a->setCheckable(true);
  a->setChecked(false);
  insertActs->addAction(a);
  a = new QAction(tr("Unspecified"), this);
  a->setData(4);
  a->setCheckable(true);
  a->setChecked(false);
  insertActs->addAction(a);

  buildAct = new QAction(QIcon(":/images/Run.png"), tr("Build"), this);
  buildAct->setStatusTip(tr("Create format file"));
  connect(buildAct, SIGNAL(triggered()), this, SLOT(build()));

  buildRAct = new QAction(QIcon(":/images/refresh.png"), tr("Rebuild"), this);
  buildRAct->setStatusTip(tr("Rebuild current format file"));
  connect(buildRAct, SIGNAL(triggered()), this, SLOT(buildR()));

	formularAct = new QAction(tr("Fomular Editor"), this);
	connect(formularAct, SIGNAL(triggered()), this, SLOT(editFormular()));

	tikzEditAct = new QAction(tr("Tikz Editor"), this);
	connect(tikzEditAct, SIGNAL(triggered()), this, SLOT(editTikz()));

  texFontSettingAct = new QAction(tr("TeX fonts setting"), this);
	connect(texFontSettingAct, SIGNAL(triggered()), this, SLOT(setTexFonts()));

	fontInfoAct = new QAction(tr("Font information"), this);
	connect(fontInfoAct, SIGNAL(triggered()), this, SLOT(showFonts()));

	editorSettingAct = new QAction(tr("Edit setting"), this);
	connect(editorSettingAct, SIGNAL(triggered()), this, SLOT(setEditor()));

  closeAct = new QAction(tr("Cl&ose"), this);
  closeAct->setStatusTip(tr("Close the active window"));
  connect(closeAct, SIGNAL(triggered()), mdiArea, SLOT(closeActiveSubWindow()));

  closeAllAct = new QAction(tr("Close &all"), this);
  closeAllAct->setStatusTip(tr("Close all the windows"));
  connect(closeAllAct, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));

  tileAct = new QAction(tr("&Tile"), this);
  tileAct->setStatusTip(tr("Tile the windows"));
  connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

  cascadeAct = new QAction(tr("&Cascade"), this);
  cascadeAct->setStatusTip(tr("Cascade the windows"));
  connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

  nextAct = new QAction(tr("Ne&xt"), this);
  nextAct->setShortcuts(QKeySequence::NextChild);
  nextAct->setStatusTip(tr("Move the focus to the next window"));
  connect(nextAct, SIGNAL(triggered()), mdiArea, SLOT(activateNextSubWindow()));

  previousAct = new QAction(tr("Pre&vious"), this);
  previousAct->setShortcuts(QKeySequence::PreviousChild);
  previousAct->setStatusTip(tr("Move the focus to the previous window"));
  connect(previousAct, SIGNAL(triggered()), mdiArea, SLOT(activatePreviousSubWindow()));

  separatorAct = new QAction(this);
  separatorAct->setSeparator(true);

  QString p = xwApp->getProductName();
	productHelpAct = new QAction(p, this);
	connect(productHelpAct, SIGNAL(triggered()), this, SLOT(showProductHelp()));

  aboutSoftAct = new QAction(tr("About software"), this);
	connect(aboutSoftAct, SIGNAL(triggered()), this, SLOT(showAboutSoft()));

	aboutHardAct = new QAction(tr("About hardware"), this);
	connect(aboutHardAct, SIGNAL(triggered()), this, SLOT(showAboutHard()));
}

XWTeXTextEdit * XWTeXFmtEditorMainWindow::createMdiChild()
{
	XWTeXTextEdit *child = new XWTeXTextEdit;
  
  mdiArea->addSubWindow(child);
  connect(child, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
  connect(child, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));
  connect(child, SIGNAL(cursorChanged(int, int)), this, SLOT(setSourceLabel(int, int)));
	searcher->setDoc(child->document());

  return child;
}

void XWTeXFmtEditorMainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(importAct);
  fileMenu->addAction(openAct);
	fileMenu->addAction(openFolderAct);
	fileMenu->addSeparator();
  fileMenu->addAction(saveAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(cutAct);
  editMenu->addAction(copyAct);
  editMenu->addAction(pasteAct);
  editMenu->addSeparator();

	editMenu->addAction(findAct);
  editMenu->addAction(replaceAct);

	editMenu->addSeparator();
  QMenu * cmenu = editMenu->addMenu(tr("Input codec"));
  QList<QAction *> acts = inputCodecActs->actions();
  for (int i = 0; i <  acts.size(); i++)
  {
  	QAction * a = acts[i];
  	cmenu->addAction(a);
  }

  cmenu = editMenu->addMenu(tr("Output codec"));
  acts = outputCodecActs->actions();
  for (int i = 0; i <  acts.size(); i++)
  {
  	QAction * a = acts[i];
  	cmenu->addAction(a);
  }

  editMenu->addSeparator();
  editMenu->addAction(etexAct);
  editMenu->addAction(fleAct);
  editMenu->addAction(halteAct);
  cmenu = editMenu->addMenu(tr("Interaction mode"));
  acts = insertActs->actions();
  for (int i = 0; i <  acts.size(); i++)
  {
  	QAction * a = acts[i];
  	cmenu->addAction(a);
  }

  viewMenu = menuBar()->addMenu(tr("&View"));

  windowMenu = menuBar()->addMenu(tr("&Window"));
  updateWindowMenu();
  connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

  toolsMenu = menuBar()->addMenu(tr("&Tools"));
	toolsMenu->addAction(formularAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(tikzEditAct);
	toolsMenu->addSeparator();
  toolsMenu->addAction(texFontSettingAct);
  toolsMenu->addAction(fontInfoAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(editorSettingAct);

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(productHelpAct);
	helpMenu->addSeparator();
  helpMenu->addAction(aboutSoftAct);
	helpMenu->addAction(aboutHardAct);
}

void XWTeXFmtEditorMainWindow::createToolBars()
{
	fileToolBar = addToolBar(tr("File"));
  fileToolBar->addAction(newAct);
  fileToolBar->addAction(importAct);
  fileToolBar->addAction(openAct);
  fileToolBar->addAction(saveAct);

  editToolBar = addToolBar(tr("Edit"));
  editToolBar->addAction(cutAct);
  editToolBar->addAction(copyAct);
  editToolBar->addAction(pasteAct);

  buildToolBar = addToolBar(tr("Build"));
  buildToolBar->addAction(buildAct);
  buildToolBar->addAction(buildRAct);
}

void XWTeXFmtEditorMainWindow::findInputCodecName(const QString & name)
{
	QAction * a = inputCodecActs->checkedAction();
	if (name.isEmpty())
	{
		if (a)
			a->setChecked(false);

		return ;
	}
	else
	{
		if (a)
		{
			QString t = a->text();
			if (name == t)
				return ;
			else
				a->setChecked(false);
		}

		QList<QAction *> acts = inputCodecActs->actions();
   for (int i = 0; i <  acts.size(); i++)
   {
  	 a = acts[i];
  	 QString t = a->text();
  	 if (t == name)
  	 {
  	 	a->setChecked(true);
  	 	break;
  	 }
   }
	}
}

qint32  XWTeXFmtEditorMainWindow::findInteract()
{
	qint32 ret = 2;
	QAction * a = insertActs->checkedAction();
	if (a)
		ret = (qint32)(a->data().toInt());

	return ret;
}

QMdiSubWindow * XWTeXFmtEditorMainWindow::findMdiChild(const QString & fileName)
{
	foreach (QMdiSubWindow *window, mdiArea->subWindowList())
	{
      XWTeXTextEdit *mdiChild = qobject_cast<XWTeXTextEdit *>(window->widget());
      QString fn = mdiChild->getFileName();
      if (fn == fileName)
			{
				searcher->setDoc(mdiChild->document());
				return window;
			}          
  }
  return 0;
}

void XWTeXFmtEditorMainWindow::findOutputCodecName(const QString & name)
{
	QAction * a = outputCodecActs->checkedAction();
	if (name.isEmpty())
	{
		if (a)
			a->setChecked(false);

		return ;
	}
	else
	{
		if (a)
		{
			QString t = a->text();
			if (name == t)
				return ;
			else
				a->setChecked(false);
		}

		QList<QAction *> acts = outputCodecActs->actions();
   for (int i = 0; i <  acts.size(); i++)
   {
  	 a = acts[i];
  	 QString t = a->text();
  	 if (t == name)
  	 {
  	 	a->setChecked(true);
  	 	break;
  	 }
   }
	}
}

QString XWTeXFmtEditorMainWindow::getLocalPath()
{
	QString path = QCoreApplication::applicationDirPath();
	int idx = path.lastIndexOf(QChar('/'));
	if (idx > 0)
		path = path.left(idx + 1);

	if (path[path.length() - 1] != QChar('/'))
		path += QChar('/');

	path += "tex";
	QDir d(path);
	if (!d.exists())
		d.mkdir(path);

	return path;
}
