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
#include "XWFileName.h"
#include "XWString.h"
#include "XWApplication.h"
#include "XWPictureSea.h"
#include "XWDocSea.h"
#include "XWHelpSea.h"
#include "XWTeXSea.h"
#include "XWDVIDoc.h"
#include "XWPDFDoc.h"
#include "XWURLWindow.h"
#include "XWFTPWindow.h"
#include "XWRecentFileWindow.h"
#include "XWToPDFWindow.h"
#include "XWToPSWindow.h"
#include "XWToTextWindow.h"
#include "XWToImageWindow.h"
#include "XWDisplaySettingDialog.h"
#include "XWExternalToolWindow.h"
#include "XWFontWindow.h"
#include "XWTexFontWindow.h"
#include "XWFontInfoWindow.h"
#include "XWTeXEditSetting.h"
#include "XWOutlineWindow.h"
#include "XWBaseNameWindow.h"
#include "XWFontDefWindow.h"
#include "XWTexFormatWindow.h"
#include "XWTeXHighlighter.h"
#include "XWTeXTextEdit.h"
#include "XWTeXConsole.h"
#include "XWTeXEditorCore.h"
#include "XWTeXEditorDev.h"
#include "XWTeXDocTemplateWindow.h"
#include "XWFindTextDialog.h"
#include "XWReplaceDialog.h"
#include "XWLaTeXFormularMainWindow.h"
#include "XWTikzMainWindow.h"
#include "XWSearcher.h"
#include "XWDocSearchWidget.h"
#include "XWTeXEditorMainWindow.h"

XWTeXEditorMainWindow::XWTeXEditorMainWindow()
{
	searcher = new XWQTextDocumentSearcher(this);

	QString title = xwApp->getProductName();
	setWindowTitle(title);
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

	setBackground();
	dev = new XWTeXEditorDev(this);
	connect(dev, SIGNAL(messageAdded(const QString &)), this, SLOT(insertTermText(const QString &)));
	connect(dev, SIGNAL(finished()), this, SLOT(updateShow()));
	connect(dev, SIGNAL(needFile(const QString &)), this, SLOT(openFile(const QString &)));
	connect(this, SIGNAL(fileDownloaded(const QString &)), dev, SLOT(setInputText(const QString &)));

	mdiArea = new QMdiArea;
  mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setCentralWidget(mdiArea);
  connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
  windowMapper = new QSignalMapper(this);
  connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

  folderDock = new QDockWidget(this);
	folderDock->setAllowedAreas(Qt::LeftDockWidgetArea);
	addDockWidget(Qt::LeftDockWidgetArea, folderDock);
	folderDock->toggleViewAction()->setChecked(false);

	QTabWidget * tab = new QTabWidget(folderDock);
	folderDock->setWidget(tab);

	folder = new QTreeView(this);
	tab->addTab(folder,tr("Folder"));
	folderModel = new QFileSystemModel;
	QStringList filters;
	filters << "*.tex" << "*.txt";
	folderModel->setNameFilters(filters);
	folderModel->setNameFilterDisables(false);
	folderModel->setRootPath(QDir::currentPath());
	folder->setModel(folderModel);

	XWQTextDocumentSearchWidget * sw = new XWQTextDocumentSearchWidget(searcher, tab);
	tab->addTab(sw,tr("Search"));
	connect(sw, SIGNAL(positionActivated(int, int)), this, SLOT(showSearchResult(int, int)));
	connect(sw, SIGNAL(fileActivated(const QString&)), this, SLOT(activeFile(const QString&)));

	XWQTextDocumentReplaceWidget * rw = new XWQTextDocumentReplaceWidget(searcher, tab);
	tab->addTab(rw,tr("Replace"));
	connect(rw, SIGNAL(positionActivated(int, int)), this, SLOT(showSearchResult(int, int)));
	connect(rw, SIGNAL(fileActivated(const QString&)), this, SLOT(activeFile(const QString&)));

  outlineDock = new QDockWidget(tr("Outline"), this);
  outlineDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  coreDock = new QDockWidget(tr("Preview"), this);
  coreDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  termDock = new QDockWidget(tr("Term Out"), this);
  termDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

  core = new XWTeXEditorCore(this, this);
  coreDock->setWidget(core);
  addDockWidget(Qt::RightDockWidgetArea, coreDock);
  coreDock->toggleViewAction()->setChecked(false);

  outlineWin = new XWOutlineWindow(core, outlineDock);
  outlineDock->setWidget(outlineWin);
  addDockWidget(Qt::RightDockWidgetArea, outlineDock);
  outlineDock->toggleViewAction()->setChecked(false);

  termWin = new XWTeXConsole(this);
  termDock->setWidget(termWin);
  addDockWidget(Qt::BottomDockWidgetArea, termDock);
  termDock->toggleViewAction()->setChecked(false);
  connect(dev, SIGNAL(requestInput()), termWin, SLOT(requstInput()));
  connect(termWin, SIGNAL(inputFinished(const QString &)), dev, SLOT(setInputText(const QString &)));

	findDialog = new XWFindTextDialog(this);
	replaceDialog = new XWReplaceDialog(this);

  fileLabel = new QLabel(tr("Title: "), this);
  QPalette newPalette = fileLabel->palette();
	newPalette.setBrush(QPalette::WindowText, Qt::black);
	fileLabel->setPalette(newPalette);
  statusBar()->addWidget(fileLabel, 1);

  authorLabel = new QLabel(tr("Author: "), this);
  authorLabel->setPalette(newPalette);
  statusBar()->addWidget(authorLabel, 1);

  creationDateLabel = new QLabel(tr("Creation date: "), this);
  creationDateLabel->setPalette(newPalette);
  statusBar()->addWidget(creationDateLabel, 1);

  lastModifiedDateLabel = new QLabel(tr("Last modified date: "), this);
  lastModifiedDateLabel->setPalette(newPalette);
  statusBar()->addWidget(lastModifiedDateLabel, 1);

  pagesLabel = new QLabel(tr("Current page: "), this);
  pagesLabel->setPalette(newPalette);
  statusBar()->addWidget(pagesLabel, 1);

  sourceLabel = new QLabel(tr("Row 0  Column 0"), this);
  sourceLabel->setPalette(newPalette);
  statusBar()->addWidget(sourceLabel, 1);

  createActions();
  createMenus();
  setToolsBarsVisible(true);
  updateActions();

  updateMenus();
	connect(folder, SIGNAL(activated(const QModelIndex &)), this, SLOT(openFile(const QModelIndex &)));
  connect(core, SIGNAL(fileLoaded()), this, SLOT(updateActions()));
  connect(core, SIGNAL(currentChanged(int, int)), this, SLOT(setPageStatus(int, int)));
  connect(core, SIGNAL(authorChanged(const QString &)), this, SLOT(setAuthor(const QString &)));
  connect(core, SIGNAL(creationDateChanged(const QString &)), this, SLOT(setCreationDate(const QString &)));
  connect(core, SIGNAL(keywordsChanged(const QString &)), this, SLOT(setKeywords(const QString &)));
  connect(core, SIGNAL(lastModifiedDateChanged(const QString &)), this, SLOT(setLastModifiedDate(const QString &)));
  connect(core, SIGNAL(subjectChanged(const QString &)), this, SLOT(setSubject(const QString &)));
  connect(core, SIGNAL(zoomChanged(int)), this, SLOT(setZoomIndex(int)));
  connect(core, SIGNAL(titleChanged(const QString &)), this, SLOT(setTitle(const QString &)));

	connect(findDialog, SIGNAL(findClicked()), this, SLOT(find()));
	connect(findDialog, SIGNAL(editingFinished()), this, SLOT(find()));
	connect(replaceDialog, SIGNAL(replaceClicked()), this, SLOT(replace()));
}

XWTeXEditorMainWindow::~XWTeXEditorMainWindow()
{
}

void XWTeXEditorMainWindow::addToContextMenu(QMenu * menu)
{
	QMenu * colorMenu = menu->addMenu(tr("Mark"));
	colorMenu->addAction(markAct);
	colorMenu->addSeparator();
	core->addMarkingActions(colorMenu);
	colorMenu->addSeparator();
	colorMenu->addAction(markCAct);
	menu->addSeparator();
	menu->addAction(gotoNextAct);
	menu->addAction(gotoPrevAct);
	menu->addAction(gotoFirstAct);
	menu->addAction(gotoLastAct);
	menu->addSeparator();
	menu->addAction(zoomInAct);
	menu->addAction(zoomOutAct);
}

void XWTeXEditorMainWindow::displayDest(const char *namedDest)
{
	XWString dest(namedDest);
	core->displayDest(&dest);
}

void XWTeXEditorMainWindow::displayPage(int pg)
{
	core->displayPage(pg);
}

bool XWTeXEditorMainWindow::loadFile(const QString & filename)
{
	if (filename.isEmpty())
		return false;

	XWDocSea sea;
	QString fullname = sea.findTex(filename);
	if (fullname.isEmpty())
	{
		xwApp->openError(filename, false);
		xwApp->showErrs();
		return false;
	}

	QFile file(fullname);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		xwApp->openError(fullname, false);
		xwApp->showErrs();
		return false;
	}

	QMdiSubWindow *existing = findMdiChild(fullname);
	if (existing)
     mdiArea->setActiveSubWindow(existing);
  else
  {
  	XWTeXTextEdit *child = createMdiChild();
		child->loadFile(fullname);
  	child->show();
  }

	dev->setInteractionOption(findInteract());
	dev->setDebugFormatFile(debugAct->isChecked());
	dev->setFileLineErrorStyle(fleAct->isChecked());
	dev->setHaltOnError(haltAct->isChecked());
	dev->setShellEnabled(shellAct->isChecked());
	dev->setSrcSpecialsP(srcAct->isChecked());

	dev->setInsertSrcSpecialAuto(autoAct->isChecked());
	dev->setInsertSrcSpecialEveryCr(crAct->isChecked());
	dev->setInsertSrcSpecialEveryDisplay(displayAct->isChecked());
	dev->setInsertSrcSpecialEveryHBox(hboxAct->isChecked());
	dev->setInsertSrcSpecialEveryMath(mathAct->isChecked());
	dev->setInsertSrcSpecialEveryPar(parAct->isChecked());
	dev->setInsertSrcSpecialEveryParEnd(parendAct->isChecked());
	dev->setInsertSrcSpecialEveryVBox(vboxAct->isChecked());

	QTextStream in(&file);
	QString first = in.readLine();
	QString olddump = dev->getDumpName();
	bool hasfmt = dev->hasDumpName();
	if (!hasfmt && first.length() > 1 && first[0] == QChar('%') && first[1] == QChar('&'))
	{
		int i = 2;
		while (first[i] != QChar(' ') &&
		       first[i] != QChar('\n') &&
		       first[i] != QChar('\t') &&
		       first[i] != QChar('\r'))
		{
			i++;
		}

		QString fmt = first.mid(2, i - 2);
		hasfmt = loadFmt(fmt);
	}

	if (!hasfmt)
	{
		XWTexFormatWindow dlg(this);
		if (dlg.exec() != QDialog::Accepted)
			return false;

		QString fmt = dlg.getFmt();
		hasfmt = loadFmt(fmt);
	}

	if (!hasfmt)
	{
		xwApp->showErrs();
		return false;
	}

	if (!dev->loadFile(fullname))
	{
		xwApp->showErrs();
		return false;
	}

	return true;
}

bool XWTeXEditorMainWindow::loadFmt(const QString & fmt)
{
	termWin->clear();
	bool ret = dev->loadFmt(fmt);
	if (!ret)
	{
		XWTeXStdIODev builder;
		connect(&builder, SIGNAL(messageAdded(const QString &)), this, SLOT(insertTermText(const QString &)));
		connect(&builder, SIGNAL(needFile(const QString &)), this, SLOT(openFile(const QString &)));
		connect(this, SIGNAL(fileDownloaded(const QString &)), &builder, SLOT(setInputText(const QString &)));
		connect(&builder, SIGNAL(requestInput()), termWin, SLOT(requstInput()));
  	connect(termWin, SIGNAL(inputFinished(const QString &)), &builder, SLOT(setInputText(const QString &)));
		builder.buildFmtNow(fmt, true);
		ret = builder.isOk();
		if (ret)
			ret = dev->loadFmt(fmt);
	}

	if (!ret)
	{
		QString msg = QString(tr("Fail to load afmt file %1. Please check if "
		                         "format files exist, or format setting is ok.")).arg(fmt);
		xwApp->error(msg);
	}

	return ret;
}

void XWTeXEditorMainWindow::insertTermText(const QString & txt)
{
	termWin->insertPlainText(txt);
	termWin->ensureCursorVisible();
}

void XWTeXEditorMainWindow::build()
{
	QString fullname;
	XWTeXTextEdit * cur = activeMdiChild();
	if (cur)
		fullname = cur->getFileName();
	else
		fullname = getLocalPath();

	fullname = QFileDialog::getOpenFileName(this,
		                                      tr("Open file"),
		                                      fullname,
		                                      tr("Files (*.tex)"));
	if (fullname.isEmpty())
		return ;

	mainInputName = fullname;
	if (!loadFile(fullname))
		return ;

	topPage = 0;
	core->setDoc(0);
	dev->compile();
}

void XWTeXEditorMainWindow::buildR()
{
	if (mainInputName.isEmpty())
		build();
	else
	{
		termWin->clear();
		topPage = core->getPageNum();
		core->setDoc(0);
		dev->compile();
	}
}

void XWTeXEditorMainWindow::copy()
{
	if (activeMdiChild())
    activeMdiChild()->copy();
}

void XWTeXEditorMainWindow::cut()
{
	if (activeMdiChild())
    activeMdiChild()->cut();
}

void XWTeXEditorMainWindow::editFormular()
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

void XWTeXEditorMainWindow::editTikz()
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

void XWTeXEditorMainWindow::gotoPage(int idx)
{
	int pg = idx + 1;
	if (pg != core->getPageNum())
		displayPage(pg);
}

void XWTeXEditorMainWindow::insertText(const QString & txt)
{
	if (activeMdiChild())
    activeMdiChild()->insertPlainText(txt);
}

void XWTeXEditorMainWindow::loadFmt()
{
	XWTexFormatWindow dlg(this);
	if (dlg.exec() != QDialog::Accepted)
		return ;

	QString fmt = dlg.getFmt();
	if (fmt.isEmpty())
		return ;

	loadFmt(fmt);
}

void XWTeXEditorMainWindow::newFile()
{
	QString path = getLocalPath();
	if (!mainInputName.isEmpty())
	{
		XWFileName fn(mainInputName);
		path = fn.dirPart();
	}

	QString files = tr("Files (*.tex)");
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

void XWTeXEditorMainWindow::newFromTmplate()
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

void XWTeXEditorMainWindow::open()
{
	QString path = getLocalPath();
	if (!mainInputName.isEmpty())
	{
		XWFileName fn(mainInputName);
		path = fn.dirPart();
	}

	searcher->setPath(path);

	QString filename = QFileDialog::getOpenFileName(this,
		                                            tr("Open file"),
		                                            path,
		                                            tr("Files (*.tex)"));
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

void XWTeXEditorMainWindow::openFile(const QString & filename)
{
	QString ret = xwApp->getFile(filename);
	emit fileDownloaded(ret);
}

void XWTeXEditorMainWindow::openFolder()
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

void XWTeXEditorMainWindow::paste()
{
	if (activeMdiChild())
    activeMdiChild()->paste();
}

void XWTeXEditorMainWindow::save()
{
	if (activeMdiChild())
    activeMdiChild()->save();
}

void XWTeXEditorMainWindow::saveToDvi()
{
	QString fullname;
	XWTeXTextEdit * cur = activeMdiChild();
	if (cur)
		fullname = cur->getFileName();
	else
		fullname = mainInputName;

	XWFileName fn(fullname);
	QString basename = fn.baseName();
	XWDocSea sea;
	QStringList list = sea.getConfigDir(XWDocSea::DVI);
	QString path = list[0];
	path += QString("/%1.dvi").arg(basename);
	QString files = tr("Files (*.dvi)");

	QString filename = QFileDialog::getSaveFileName(this, tr("Save as"), path, files);
	if (filename.isEmpty())
		return ;

	core->getDoc()->saveAs(filename);
	xwApp->showWarns();
	xwApp->showErrs();
}

void XWTeXEditorMainWindow::saveToImage()
{
	QString fullname;
	XWTeXTextEdit * cur = activeMdiChild();
	if (cur)
		fullname = cur->getFileName();
	else
		fullname = mainInputName;

	XWFileName fn(fullname);
	QString basename = fn.baseName();
	QString path = getLocalPath();
	path += QString("/%1").arg(basename);
	int pgnum = core->getNumPages();
	if (core->hasSelection())
	{
		XWToImageWindow d(path, this);
		if (d.exec() != QDialog::Accepted)
			return ;

		QString filename = d.getFileName();
		if (filename.isEmpty())
			return ;

		QString fmt = d.getFormat();
		QColor bg = d.getBgColor();
		bool r = d.getReverseVideo();
		core->saveToImage(filename, fmt, bg, r);
	}
	else
	{
		XWToImageWindow d(path, pgnum, this);
		if (d.exec() != QDialog::Accepted)
			return ;

		QList<int> pages = d.getPages(false);
		if (pages.isEmpty())
			return ;

		QString filename = d.getFileName();
		if (filename.isEmpty())
			return ;

		QString fmt = d.getFormat();
		core->saveToImage(filename, fmt, pages);
	}

	xwApp->showWarns();
	xwApp->showErrs();
}

void XWTeXEditorMainWindow::saveToPDF()
{
	QString fullname;
	XWTeXTextEdit * cur = activeMdiChild();
	if (cur)
		fullname = cur->getFileName();
	else
		fullname = mainInputName;

	if (fullname.isEmpty())
		return ;

	XWFileName fn(fullname);
	QString basename = fn.baseName();
	XWDocSea sea;
	QStringList list = sea.getConfigDir(XWDocSea::PDF);
	QString path = list[0];
	path += QString("/%1.pdf").arg(basename);
	int pgnum = core->getNumPages();
	XWToPDFWindow d(path, pgnum, this);
	if (d.exec() != QDialog::Accepted)
		return ;

	QList<int> pages = d.getPages();
	if (pages.isEmpty())
		return ;

	QString filename = d.getFileName();
	if (filename.isEmpty())
		return ;

	QString thumb = d.getThumbBase();
	QByteArray dviba = fullname.toLocal8Bit();
	QByteArray pdfba = filename.toLocal8Bit();

	QString owpwd = d.getOwnerPassward();
	QString uspwd = d.getUserPassward();
	char * owner_pw = 0;
	char * user_pw = 0;
	if (!owpwd.isEmpty())
	{
		QByteArray pwdba = owpwd.toLocal8Bit();
		if (!pwdba.isEmpty())
			owner_pw = qstrdup(pwdba.data());
	}

	if (!uspwd.isEmpty())
	{
		QByteArray pwdba = uspwd.toLocal8Bit();
		if (!pwdba.isEmpty())
			user_pw = qstrdup(pwdba.data());
	}

	QFile *fileA = new QFile(filename);
	if (!fileA->open(QIODevice::WriteOnly))
	{
		delete fileA;
		xwApp->openError(filename, true);
		xwApp->showWarns();
		xwApp->showErrs();
		return ;
	}

	XWDVIDoc * dvidoc = (XWDVIDoc*)(core->getDoc());
	dvidoc->saveToPDF(fileA, dviba.data(), pdfba.data(), thumb, pages, owner_pw, user_pw);
	fileA->close();
	delete fileA;
	if (owner_pw)
		delete [] owner_pw;

	if (user_pw)
		delete [] user_pw;

	xwApp->showWarns();
	xwApp->showErrs();
}

void XWTeXEditorMainWindow::saveToPS()
{
	QString fullname;
	XWTeXTextEdit * cur = activeMdiChild();
	if (cur)
		fullname = cur->getFileName();
	else
		fullname = mainInputName;

	if (fullname.isEmpty())
		return ;

	XWFileName fn(fullname);
	QString basename = fn.baseName();
	XWDocSea sea;
	QStringList list = sea.getConfigDir(XWDocSea::PS);
	QString path = list[0];
	path += QString("/%1.ps").arg(basename);
	int pgnum = core->getNumPages();
	XWToPSWindow d(path, pgnum, this);
	if (d.exec() != QDialog::Accepted)
		return ;

	QList<int> pages = d.getPages(true);
	if (pages.isEmpty())
		return ;

	QString filename = d.getFileName();
	if (filename.isEmpty())
		return ;

	core->saveDVIToPS(filename, pages);
	xwApp->showWarns();
	xwApp->showErrs();
}

void XWTeXEditorMainWindow::saveToText()
{
	QString fullname;
	XWTeXTextEdit * cur = activeMdiChild();
	if (cur)
		fullname = cur->getFileName();
	else
		fullname = mainInputName;

	if (fullname.isEmpty())
		return ;

	XWFileName fn(fullname);
	QString basename = fn.baseName();
	XWDocSea sea;
	QStringList list = sea.getConfigDir(XWDocSea::TXT);
	QString path = list[0];
	path += QString("/%1.txt").arg(basename);
	int pgnum = core->getNumPages();

	XWToTextWindow d(path, pgnum, this);
	if (d.exec() != QDialog::Accepted)
		return ;

	QList<int> pages = d.getPages(false);
	if (pages.isEmpty())
		return ;

	QString filename = d.getFileName();
	if (filename.isEmpty())
		return ;

	core->saveToText(filename, pages);
	xwApp->showWarns();
	xwApp->showErrs();
}

void XWTeXEditorMainWindow::setActiveSubWindow(QWidget *window)
{
	if (!window)
     return;
  mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void XWTeXEditorMainWindow::setContinuousMode(bool e)
{
	core->setContinuousMode(e);
}

void XWTeXEditorMainWindow::setDoubleMode(bool e)
{
	core->setDoubleMode(!e);
	if (e)
	{
		singlePageAct->setIcon(QIcon(":/images/singlepage.png"));
		singlePageAct->setText(tr("Single page"));
	}
	else
	{
		singlePageAct->setIcon(QIcon(":/images/doublepage.png"));
		singlePageAct->setText(tr("Double page"));
	}
}

void XWTeXEditorMainWindow::setEditor()
{
	XWTexEditSetting d;
	d.exec();
}

void XWTeXEditorMainWindow::setFonts()
{
	XWFontWindow d(this);
	d.exec();
}

void XWTeXEditorMainWindow::setHoriMode(bool e)
{
	if (e)
		continuousPagesAct->setChecked(true);
	core->setHoriMode(e);
}

void XWTeXEditorMainWindow::setInputCodec(QAction * a)
{
	if (!a)
		return ;

	QString n = a->text();
	if (activeMdiChild())
    activeMdiChild()->setInputCodec(n);
}

void XWTeXEditorMainWindow::setOutputCodec(QAction * a)
{
	if (!a)
		return ;

	QString n = a->text();
	if (activeMdiChild())
    activeMdiChild()->setOutputCodec(n);
}

void XWTeXEditorMainWindow::setPageStatus(int cur, int num)
{
	QString p = QString(tr("Current page: %1/%2")).arg(cur).arg(num);
	pagesLabel->setText(p);
}

void XWTeXEditorMainWindow::setTexFonts()
{
	XWTexFontWindow win(this);
	win.exec();
}

void XWTeXEditorMainWindow::setTools()
{
	XWExternalToolWindow d(this);
	d.exec();
}

void XWTeXEditorMainWindow::setZoomIndex(int idx)
{
	zoomCombo->setCurrentIndex(idx);
}

void XWTeXEditorMainWindow::showAboutHard()
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

void XWTeXEditorMainWindow::showAboutSoft()
{
	QString product = xwApp->getProductName();
	QString version = xwApp->getVersion();
	QString text = QString(tr("<h3>%1 %2</h3>"
	                         "<p>%3 is a TeX/LaTeX document IDE with source edit and preview.</p>")).arg(product).arg(version).arg(product);
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

void XWTeXEditorMainWindow::showFontDef()
{
	XWDoc * doc = core->getDoc();
	if (!doc)
		return ;

	XWDVIDoc * dvidoc = (XWDVIDoc*)doc;
	DVIFontDef * defs = dvidoc->getFontDefs();
	int num = dvidoc->getNumberOfDefFont();
	if (num <= 0 || !defs)
		return ;

	XWFontDefWindow d(defs, num, this);
	d.exec();
}

void XWTeXEditorMainWindow::showFonts()
{
	XWFontInfoWindow win(this);
	win.exec();
}

void XWTeXEditorMainWindow::showBeamerHelp()
{
	xwApp->getHelp("beamer/beamer");
}

void XWTeXEditorMainWindow::showLaTeXHelp()
{
	xwApp->getHelp("latex/latex");
}

void XWTeXEditorMainWindow::showPGFHelp()
{
	xwApp->getHelp("pgf/tikz");
}

void XWTeXEditorMainWindow::showProductHelp()
{
	xwApp->getHelp("xwtex");
}

void XWTeXEditorMainWindow::showPSTHelp()
{
	xwApp->getHelp("pstricks/pstricks");
}

void XWTeXEditorMainWindow::switchLayoutDirection()
{
	if (layoutDirection() == Qt::LeftToRight)
     qApp->setLayoutDirection(Qt::RightToLeft);
  else
     qApp->setLayoutDirection(Qt::LeftToRight);
}

void XWTeXEditorMainWindow::zoom(int idx)
{
	core->zoom(idx);
}

void XWTeXEditorMainWindow::updateMenus()
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
  	QString t = child->getInputCodecName();
  	findInputCodecName(t);
  	t = child->getOutputCodecName();
  	findOutputCodecName(t);
  }

	findAct->setEnabled(hasMdiChild);
	replaceAct->setEnabled(hasMdiChild);
}

void XWTeXEditorMainWindow::updateShow()
{
	if (!dev->isOk())
		return ;

	QIODevice * dvifile = dev->takeDviFile();
	dvifile->open(QIODevice::ReadOnly);
	XWDVIDoc * dvidoc = new XWDVIDoc;
	if (!dvidoc->load(dvifile, 0, 0))
	{
		delete dvidoc;
		xwApp->showErrs();
		return ;
	}

	core->setDoc(dvidoc);
	if (topPage == 0)
		topPage = 1;

	if (topPage > core->getNumPages())
		topPage = core->getNumPages();
	outlineWin->setup();

	if (topPage > 0)
		core->displayPage(topPage);
}

void XWTeXEditorMainWindow::updateWindowMenu()
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

void XWTeXEditorMainWindow::closeEvent(QCloseEvent *event)
{
	saveToolsBarStatus();
	xwApp->cleanup();
	QMainWindow::closeEvent(event);
}

void XWTeXEditorMainWindow::activeFile(const QString & filename)
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

void XWTeXEditorMainWindow::find()
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

void XWTeXEditorMainWindow::openFile(const QModelIndex & index)
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

void XWTeXEditorMainWindow::replace()
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

void XWTeXEditorMainWindow::setAuthor(const QString & a)
{
	QString author = QString(tr("Author: %1")).arg(a);
	authorLabel->setText(author);
}

void XWTeXEditorMainWindow::setCreationDate(const QString & d)
{
	QString date = QString(tr("Creation date: %1")).arg(d);
	creationDateLabel->setText(date);
}

void XWTeXEditorMainWindow::setKeywords(const QString & kws)
{
	QString tip = fileLabel->toolTip();
	tip += QString(tr("Keywords: %1\n")).arg(kws);
	fileLabel->setToolTip(tip);
}

void XWTeXEditorMainWindow::setLastModifiedDate(const QString & d)
{
	QString date = QString(tr("Last modified date: %1")).arg(d);
	lastModifiedDateLabel->setText(date);
}

void XWTeXEditorMainWindow::setSourceLabel(int r, int c)
{
	QString s = QString(tr("Row %1  Column %2")).arg(r).arg(c);
	sourceLabel->setText(s);
}

void XWTeXEditorMainWindow::setSubject(const QString & s)
{
	QString tip = QString(tr("Subject: %1\n")).arg(s);
	fileLabel->setToolTip(tip);
}

void XWTeXEditorMainWindow::setTitle(const QString & t)
{
	QString f = QString(tr("Title: %1")).arg(t);
	fileLabel->setText(f);
}

void XWTeXEditorMainWindow::setToolsBarsVisible(bool e)
{
	if (e)
		restoreToolsBarStatus();
	else
	{
		saveToolsBarStatus();
		if (fileToolsBar->isVisible())
			fileToolsBar->setVisible(false);

		if (saveToolsBar->isVisible())
			saveToolsBar->setVisible(false);

		if (editToolBar->isVisible())
			editToolBar->setVisible(false);

		if (gotoToolsBar->isVisible())
			gotoToolsBar->setVisible(false);

		if (zoomToolsBar->isVisible())
			zoomToolsBar->setVisible(false);

		if (fitToolsBar->isVisible())
			fitToolsBar->setVisible(false);

		if (modeToolsBar->isVisible())
			modeToolsBar->setVisible(false);

		if (markToolsBar->isVisible())
			markToolsBar->setVisible(false);
	}

	QAction * a = fileToolsBar->toggleViewAction();
	a->setChecked(fileToolsBar->isVisible());
	a = saveToolsBar->toggleViewAction();
	a->setChecked(saveToolsBar->isVisible());
	a = editToolBar->toggleViewAction();
	a->setChecked(editToolBar->isVisible());
	a = gotoToolsBar->toggleViewAction();
	a->setChecked(gotoToolsBar->isVisible());
	a = zoomToolsBar->toggleViewAction();
	a->setChecked(zoomToolsBar->isVisible());
	a = fitToolsBar->toggleViewAction();
	a->setChecked(fitToolsBar->isVisible());
	a = modeToolsBar->toggleViewAction();
	a->setChecked(modeToolsBar->isVisible());
	a = markToolsBar->toggleViewAction();
	a->setChecked(markToolsBar->isVisible());
}

void XWTeXEditorMainWindow::showFindDialog()
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
	
void XWTeXEditorMainWindow::showReplaceDialog()
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

void XWTeXEditorMainWindow::showSearchResult(int pos, int len)
{
	XWTeXTextEdit *cur = activeMdiChild();
	if (!cur)
	  return ;

	cur->setSelected(pos, len);
}

void XWTeXEditorMainWindow::updateActions()
{
	if (!(core->getDoc()))
	{
		toDviAct->setEnabled(false);
		toPDFAct->setEnabled(false);
		toPSAct->setEnabled(false);
		toTextAct->setEnabled(false);
		toImageAct->setEnabled(false);
		fontDefAct->setEnabled(false);

		gotoNextAct->setEnabled(false);
		gotoPrevAct->setEnabled(false);
		gotoFirstAct->setEnabled(false);
		gotoLastAct->setEnabled(false);

		zoomInAct->setEnabled(false);
		zoomOutAct->setEnabled(false);
		fitPageAct->setEnabled(false);
		fitWidthAct->setEnabled(false);
		rtlAct->setEnabled(false);
		horiAct->setEnabled(false);
		singlePageAct->setEnabled(false);
		continuousPagesAct->setEnabled(false);
		markAct->setEnabled(false);
		markCAct->setEnabled(false);
		return ;
	}

	toDviAct->setEnabled(true);
	toPDFAct->setEnabled(true);
	toPSAct->setEnabled(true);
	toTextAct->setEnabled(true);
	toImageAct->setEnabled(true);
	fontDefAct->setEnabled(true);

	gotoNextAct->setEnabled(true);
	gotoPrevAct->setEnabled(true);
	gotoFirstAct->setEnabled(true);
	gotoLastAct->setEnabled(true);

	zoomInAct->setEnabled(true);
	zoomOutAct->setEnabled(true);
	fitPageAct->setEnabled(true);
	fitWidthAct->setEnabled(true);
	rtlAct->setEnabled(true);
	horiAct->setEnabled(true);
	singlePageAct->setEnabled(true);
	continuousPagesAct->setEnabled(true);
	markAct->setEnabled(true);
	markCAct->setEnabled(true);

	gotoPageCombo->clear();
	int pgnum = core->getNumPages();
	for (int i = 1; i <= pgnum; i++)
	{
		QString tmp = QString("%1").arg(i);
		gotoPageCombo->addItem(tmp);
	}
}

XWTeXTextEdit * XWTeXEditorMainWindow::activeMdiChild()
{
	if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
	{
		XWTeXTextEdit *ret = qobject_cast<XWTeXTextEdit *>(activeSubWindow->widget());
		searcher->setDoc(ret->document());
		return ret;
	}

  return 0;
}

void XWTeXEditorMainWindow::createActions()
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

  fmtAct = new QAction(tr("Load format"), this);
	fmtAct->setPriority(QAction::LowPriority);
	connect(fmtAct, SIGNAL(triggered()), this, SLOT(loadFmt()));

	toDviAct = new QAction(tr("Save to DVI"), this);
	toDviAct->setStatusTip(tr("Save as DVI file"));
	connect(toDviAct, SIGNAL(triggered()), this, SLOT(saveToDvi()));

	toPDFAct = new QAction(QIcon(":/images/filepdf.png"), tr("Save to pdf"), this);
	toPDFAct->setStatusTip(tr("Save as pdf file"));
	connect(toPDFAct, SIGNAL(triggered()), this, SLOT(saveToPDF()));

	toPSAct = new QAction(QIcon(":/images/fileps.png"), tr("Save to ps"), this);
	toPSAct->setStatusTip(tr("Save as ps file"));
	connect(toPSAct, SIGNAL(triggered()), this, SLOT(saveToPS()));

	toTextAct = new QAction(QIcon(":/images/filetxt.png"), tr("Save to text"), this);
	toTextAct->setStatusTip(tr("Save as text file"));
	connect(toTextAct, SIGNAL(triggered()), this, SLOT(saveToText()));

	toImageAct = new QAction(QIcon(":/images/filepng.png"), tr("Save to image"), this);
	toImageAct->setStatusTip(tr("Save as image"));
	connect(toImageAct, SIGNAL(triggered()), this, SLOT(saveToImage()));

	fontDefAct = new QAction(tr("Font defines in document"), this);
	connect(fontDefAct, SIGNAL(triggered()), this, SLOT(showFontDef()));

	exitAct = new QAction(tr("&Quit"), this);
	exitAct->setShortcut(Qt::CTRL + Qt::Key_Q);
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

  debugAct = new QAction(tr("Debug format file"), this);
  debugAct->setCheckable(true);
  debugAct->setChecked(false);

  fleAct = new QAction(tr("File line error style"), this);
  fleAct->setCheckable(true);
  fleAct->setChecked(true);

  haltAct = new QAction(tr("Halt on error"), this);
  haltAct->setCheckable(true);
  haltAct->setChecked(false);

  shellAct = new QAction(tr("Shell enable"), this);
  shellAct->setCheckable(true);
  shellAct->setChecked(true);

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
  a->setChecked(false);
  insertActs->addAction(a);
  a = new QAction(tr("Error stop"), this);
  a->setData(3);
  a->setCheckable(true);
  a->setChecked(true);
  insertActs->addAction(a);
  a = new QAction(tr("Unspecified"), this);
  a->setData(4);
  a->setCheckable(true);
  a->setChecked(false);
  insertActs->addAction(a);

  srcAct = new QAction(tr("Source specials"), this);
  srcAct->setCheckable(true);
  srcAct->setChecked(false);

  autoAct = new QAction(tr("Auto"), this);
  autoAct->setCheckable(true);
  autoAct->setChecked(false);

  crAct = new QAction(tr("Carriage return"), this);
  crAct->setCheckable(true);
  crAct->setChecked(false);

  displayAct = new QAction(tr("Display"), this);
  displayAct->setCheckable(true);
  displayAct->setChecked(false);

  hboxAct = new QAction(tr("Horizontal box"), this);
  hboxAct->setCheckable(true);
  hboxAct->setChecked(false);

  mathAct = new QAction(tr("Math formular"), this);
  mathAct->setCheckable(true);
  mathAct->setChecked(false);

  parAct = new QAction(tr("Paragraph"), this);
  parAct->setCheckable(true);
  parAct->setChecked(false);

  parendAct = new QAction(tr("Paragraph end"), this);
  parendAct->setCheckable(true);
  parendAct->setChecked(false);

  vboxAct = new QAction(tr("Vertical box"), this);
  vboxAct->setCheckable(true);
  vboxAct->setChecked(false);

	gotoNextAct = new QAction(QIcon(":/images/gotonext.png"), tr("Goto next page"), this);
	gotoNextAct->setPriority(QAction::LowPriority);
	gotoNextAct->setShortcut(QKeySequence::MoveToNextPage);
	connect(gotoNextAct, SIGNAL(triggered()), core, SLOT(gotoNextPage()));
	connect(core, SIGNAL(nextPageAvailable(bool)), gotoNextAct, SLOT(setEnabled(bool)));
	gotoNextAct->setEnabled(false);

	gotoPrevAct = new QAction(QIcon(":/images/gotoprev.png"), tr("Goto previous page"), this);
	gotoPrevAct->setPriority(QAction::LowPriority);
	gotoPrevAct->setShortcut(QKeySequence::MoveToPreviousPage);
	connect(gotoPrevAct, SIGNAL(triggered()), core, SLOT(gotoPrevPage()));
	connect(core, SIGNAL(prevPageAvailable(bool)), gotoPrevAct, SLOT(setEnabled(bool)));
	gotoPrevAct->setEnabled(false);

	gotoFirstAct = new QAction(QIcon(":/images/gotofirst.png"), tr("Goto first page"), this);
	gotoFirstAct->setPriority(QAction::LowPriority);
	gotoFirstAct->setShortcut(QKeySequence::MoveToStartOfDocument);
	connect(gotoFirstAct, SIGNAL(triggered()), core, SLOT(gotoFirstPage()));
	connect(core, SIGNAL(firstPageAvailable(bool)), gotoFirstAct, SLOT(setEnabled(bool)));
	gotoFirstAct->setEnabled(false);

	gotoLastAct = new QAction(QIcon(":/images/gotolast.png"), tr("Goto last page"), this);
	gotoLastAct->setPriority(QAction::LowPriority);
	gotoLastAct->setShortcut(QKeySequence::MoveToEndOfDocument);
	connect(gotoLastAct, SIGNAL(triggered()), core, SLOT(gotoLastPage()));
	connect(core, SIGNAL(lastPageAvailable(bool)), gotoLastAct, SLOT(setEnabled(bool)));
	gotoLastAct->setEnabled(false);

	buildAct = new QAction(QIcon(":/images/Run.png"), tr("Build"), this);
  buildAct->setPriority(QAction::LowPriority);
	connect(buildAct, SIGNAL(triggered()), this, SLOT(build()));

	buildRAct = new QAction(QIcon(":/images/refresh.png"), tr("Rebuild"), this);
	buildRAct->setShortcut(QKeySequence::Refresh);
  connect(buildRAct, SIGNAL(triggered()), this, SLOT(buildR()));

	zoomInAct = new QAction(QIcon(":/images/zoomin.png"), tr("Zoom in"), this);
    zoomInAct->setPriority(QAction::LowPriority);
	zoomInAct->setShortcut(QKeySequence::ZoomIn);
	connect(zoomInAct, SIGNAL(triggered()), core, SLOT(zoomIn()));

	zoomOutAct = new QAction(QIcon(":/images/zoomout.png"), tr("Zoom out"), this);
	zoomOutAct->setPriority(QAction::LowPriority);
	zoomOutAct->setShortcut(QKeySequence::ZoomOut);
	connect(zoomOutAct, SIGNAL(triggered()), core, SLOT(zoomOut()));

	fitPageAct = new QAction(QIcon(":/images/fitpage.png"), tr("Fit to page"), this);
	connect(fitPageAct, SIGNAL(triggered()), core, SLOT(fitToPage()));

	fitWidthAct = new QAction(QIcon(":/images/fitwidth.png"), tr("Fit to width"), this);
	connect(fitWidthAct, SIGNAL(triggered()), core, SLOT(fitToWidth()));

	rtlAct = new QAction(tr("Right to left"), this);
	rtlAct->setCheckable(true);
	rtlAct->setChecked(core->getRightToLeft());
	connect(rtlAct, SIGNAL(triggered(bool)), core, SLOT(setRightToLeft(bool)));

	horiAct = new QAction(tr("Horizontal mode"), this);
	horiAct->setCheckable(true);
	horiAct->setChecked(core->getHoriMode());
	connect(horiAct, SIGNAL(triggered(bool)), this, SLOT(setHoriMode(bool)));

	singlePageAct = new QAction(QIcon(":/images/singlepage.png"), tr("Single page"), this);
	singlePageAct->setCheckable(true);
	singlePageAct->setChecked(!(core->getDoubleMode()));
	connect(singlePageAct, SIGNAL(triggered(bool)), this, SLOT(setDoubleMode(bool)));

	continuousPagesAct = new QAction(QIcon(":/images/continuepages.png"), tr("Continuous pages"), this);
	continuousPagesAct->setCheckable(true);
	continuousPagesAct->setChecked(core->getContinuousMode());
	connect(continuousPagesAct, SIGNAL(triggered(bool)), this, SLOT(setContinuousMode(bool)));

	markAct = new QAction(QIcon(":/images/mark.png"), tr("Mark"), this);
	markAct->setCheckable(true);
	markAct->setChecked(core->isMarking());
	connect(markAct, SIGNAL(triggered(bool)), core, SLOT(setMarking(bool)));

	markCAct = new QAction(tr("Clear mark"), this);
	connect(markCAct, SIGNAL(triggered()), core, SLOT(clearMark()));

  formularAct = new QAction(tr("Fomular Editor"), this);
	connect(formularAct, SIGNAL(triggered()), this, SLOT(editFormular()));

	tikzEditAct = new QAction(tr("Tikz Editor"), this);
	connect(tikzEditAct, SIGNAL(triggered()), this, SLOT(editTikz()));

	externalToolsAct = new QAction(tr("External tools"), this);
	connect(externalToolsAct, SIGNAL(triggered()), this, SLOT(setTools()));

	fontSettingAct = new QAction(tr("Display fonts setting"), this);
	connect(fontSettingAct, SIGNAL(triggered()), this, SLOT(setFonts()));

	texFontSettingAct = new QAction(tr("TeX fonts setting"), this);
	connect(texFontSettingAct, SIGNAL(triggered()), this, SLOT(setTexFonts()));

	fontInfoAct = new QAction(tr("Font information"), this);
	connect(fontInfoAct, SIGNAL(triggered()), this, SLOT(showFonts()));

	editorSettingAct = new QAction(tr("Editor setting"), this);
	connect(editorSettingAct, SIGNAL(triggered()), this, SLOT(setEditor()));

	QString p = xwApp->getProductName();
	productHelpAct = new QAction(p, this);
	connect(productHelpAct, SIGNAL(triggered()), this, SLOT(showProductHelp()));

	latexHelpAct = new QAction(tr("LaTeX2e"), this);
	connect(latexHelpAct, SIGNAL(triggered()), this, SLOT(showLaTeXHelp()));

	beamerHelpAct = new QAction(tr("Beamer"), this);
	connect(beamerHelpAct, SIGNAL(triggered()), this, SLOT(showBeamerHelp()));

	pgfHelpAct = new QAction(tr("PGF"), this);
	connect(pgfHelpAct, SIGNAL(triggered()), this, SLOT(showPGFHelp()));

	pstHelpAct = new QAction(tr("PSTricks"), this);
	connect(pstHelpAct, SIGNAL(triggered()), this, SLOT(showPSTHelp()));

	aboutSoftAct = new QAction(tr("About software"), this);
	connect(aboutSoftAct, SIGNAL(triggered()), this, SLOT(showAboutSoft()));

	aboutHardAct = new QAction(tr("About hardware"), this);
	connect(aboutHardAct, SIGNAL(triggered()), this, SLOT(showAboutHard()));

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
}

XWTeXTextEdit * XWTeXEditorMainWindow::createMdiChild()
{
	XWTeXTextEdit *child = new XWTeXTextEdit;

  mdiArea->addSubWindow(child);
  connect(child, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
  connect(child, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));
  connect(child, SIGNAL(cursorChanged(int, int)), this, SLOT(setSourceLabel(int, int)));
  searcher->setDoc(child->document());
  return child;
}

void XWTeXEditorMainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(importAct);
  fileMenu->addAction(openAct);
	fileMenu->addAction(openFolderAct);
	fileMenu->addSeparator();
  fileMenu->addAction(saveAct);
  fileMenu->addSeparator();
  fileMenu->addAction(fmtAct);
  fileMenu->addSeparator();
  fileMenu->addAction(toDviAct);
  fileMenu->addAction(toPDFAct);
  fileMenu->addAction(toPSAct);
  fileMenu->addAction(toTextAct);
  fileMenu->addAction(toImageAct);
  fileMenu->addSeparator();
  fileMenu->addAction(fontDefAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  fileToolsBar = addToolBar(tr("File tools"));

	fileToolsBar->addAction(newAct);
	fileToolsBar->addAction(importAct);
	fileToolsBar->addAction(openAct);
	fileToolsBar->addAction(saveAct);

	saveToolsBar = addToolBar(tr("Save tools"));
  saveToolsBar->addAction(toPDFAct);
  saveToolsBar->addAction(toPSAct);
  saveToolsBar->addAction(toTextAct);
  saveToolsBar->addAction(toImageAct);

  editToolBar = addToolBar(tr("Edit tools"));
  editToolBar->addAction(cutAct);
  editToolBar->addAction(copyAct);
  editToolBar->addAction(pasteAct);

  buildToolBar = addToolBar(tr("Build tools"));
  buildToolBar->addAction(buildAct);
  buildToolBar->addAction(buildRAct);

  gotoToolsBar = addToolBar(tr("Goto"));
  gotoToolsBar->addAction(gotoNextAct);
  gotoToolsBar->addAction(gotoPrevAct);
  gotoToolsBar->addAction(gotoFirstAct);
  gotoToolsBar->addAction(gotoLastAct);
  gotoPageCombo = new QComboBox(gotoToolsBar);
	gotoPageCombo->setToolTip(tr("Goto page"));
	gotoToolsBar->addWidget(gotoPageCombo);
	connect(gotoPageCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(gotoPage(int)));

	zoomToolsBar = addToolBar(tr("Zoom"));
  zoomToolsBar->addAction(zoomInAct);
	zoomToolsBar->addAction(zoomOutAct);
	zoomCombo = new QComboBox(zoomToolsBar);
  zoomToolsBar->addWidget(zoomCombo);
  QStringList zoomlist = core->getZooms();
  zoomCombo->addItems(zoomlist);
  zoomCombo->setCurrentIndex(core->getZoomIdx());
  connect(zoomCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(zoom(int)));

  fitToolsBar = addToolBar(tr("Fit tools"));
  fitToolsBar->addAction(fitPageAct);
  fitToolsBar->addAction(fitWidthAct);

  modeToolsBar = addToolBar(tr("Page mode"));
  modeToolsBar->addAction(singlePageAct);
  modeToolsBar->addAction(continuousPagesAct);

  markToolsBar = addToolBar(tr("Mark tools"));
  QMenu *colorMenu = new QMenu(markToolsBar);
	core->addMarkingActions(colorMenu);

	markToolsButton = new QToolButton(markToolsBar);
	markToolsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	markToolsButton->setPopupMode(QToolButton::MenuButtonPopup);
	markToolsButton->setMenu(colorMenu);
	markToolsButton->setDefaultAction(markAct);
	markToolsBar->addWidget(markToolsButton);

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
  editMenu->addAction(debugAct);
  editMenu->addAction(fleAct);
  editMenu->addAction(haltAct);
  editMenu->addAction(shellAct);
  cmenu = editMenu->addMenu(tr("Interaction mode"));
  acts = insertActs->actions();
  for (int i = 0; i <  acts.size(); i++)
  {
  	QAction * a = acts[i];
  	cmenu->addAction(a);
  }

  editMenu->addAction(srcAct);

  QMenu * smenu = editMenu->addMenu(tr("Insert source specials at"));
  smenu->addAction(autoAct);
  smenu->addAction(crAct);
  smenu->addAction(displayAct);
  smenu->addAction(hboxAct);
  smenu->addAction(mathAct);
  smenu->addAction(parAct);
  smenu->addAction(parendAct);
  smenu->addAction(vboxAct);

  editMenu->addSeparator();
  editMenu->addAction(gotoNextAct);
  editMenu->addAction(gotoPrevAct);
  editMenu->addAction(gotoFirstAct);
  editMenu->addAction(gotoLastAct);

  viewMenu = menuBar()->addMenu(tr("&View"));
  toolsbarMenu = viewMenu->addMenu(tr("Tools bar"));
  toolsbarMenu->addAction(fileToolsBar->toggleViewAction());
  toolsbarMenu->addAction(saveToolsBar->toggleViewAction());
  toolsbarMenu->addSeparator();
  toolsbarMenu->addAction(editToolBar->toggleViewAction());
  toolsbarMenu->addSeparator();
  toolsbarMenu->addAction(buildToolBar->toggleViewAction());
  toolsbarMenu->addSeparator();
  toolsbarMenu->addAction(gotoToolsBar->toggleViewAction());
  toolsbarMenu->addSeparator();
  toolsbarMenu->addAction(zoomToolsBar->toggleViewAction());
  toolsbarMenu->addAction(fitToolsBar->toggleViewAction());
  toolsbarMenu->addSeparator();
  toolsbarMenu->addAction(modeToolsBar->toggleViewAction());
  toolsbarMenu->addSeparator();
  toolsbarMenu->addAction(markToolsBar->toggleViewAction());
  viewMenu->addSeparator();
  viewMenu->addAction(coreDock->toggleViewAction());
  viewMenu->addAction(outlineDock->toggleViewAction());
  viewMenu->addSeparator();
  viewMenu->addAction(termDock->toggleViewAction());
  viewMenu->addSeparator();
	viewMenu->addAction(folderDock->toggleViewAction());
	viewMenu->addSeparator();
  viewMenu->addAction(rtlAct);
  viewMenu->addAction(horiAct);
  viewMenu->addSeparator();
  viewMenu->addAction(singlePageAct);
  viewMenu->addAction(continuousPagesAct);

  windowMenu = menuBar()->addMenu(tr("&Window"));
  updateWindowMenu();
  connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

  toolsMenu = menuBar()->addMenu(tr("&Tools"));
  toolsMenu->addAction(markAct);
	toolsMenu->addAction(markCAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(formularAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(tikzEditAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(externalToolsAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(fontSettingAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(texFontSettingAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(fontInfoAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(editorSettingAct);

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(productHelpAct);
	helpMenu->addSeparator();
	helpMenu->addAction(latexHelpAct);
	helpMenu->addAction(beamerHelpAct);
	helpMenu->addAction(pgfHelpAct);
	helpMenu->addAction(pstHelpAct);
	helpMenu->addSeparator();
  helpMenu->addAction(aboutSoftAct);
	helpMenu->addAction(aboutHardAct);
}

void XWTeXEditorMainWindow::findInputCodecName(const QString & name)
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

qint32  XWTeXEditorMainWindow::findInteract()
{
	qint32 ret = 3;
	QAction * a = insertActs->checkedAction();
	if (a)
		ret = (qint32)(a->data().toInt());

	return ret;
}

QMdiSubWindow *XWTeXEditorMainWindow::findMdiChild(const QString &fileName)
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

void XWTeXEditorMainWindow::findOutputCodecName(const QString & name)
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

QString XWTeXEditorMainWindow::getLocalPath()
{
	QString path = QCoreApplication::applicationDirPath();
	int idx = path.lastIndexOf(QChar('/'));
	if (idx > 0)
		path = path.left(idx + 1);

	if (path[path.length() - 1] != QChar('/'))
		path += QChar('/');

	path += "doc";
	QDir d(path);
	if (!d.exists())
		d.mkdir(path);

	searcher->setPath(path);

	return path;
}

void XWTeXEditorMainWindow::restoreToolsBarStatus()
{
	QSettings * settings = xwApp->getSettings();
	if (settings->contains("ShowFileToolsBar"))
  	fileToolsBar->setVisible(settings->value("ShowFileToolsBar").toBool());

  if (settings->contains("ShowSaveToolsBar"))
  	saveToolsBar->setVisible(settings->value("ShowSaveToolsBar").toBool());

  if (settings->contains("ShowEditToolsBar"))
  	editToolBar->setVisible(settings->value("ShowEditToolsBar").toBool());

  if (settings->contains("ShowGotoToolsBar"))
  	gotoToolsBar->setVisible(settings->value("ShowGotoToolsBar").toBool());

  if (settings->contains("ShowZoomToolsBar"))
  	zoomToolsBar->setVisible(settings->value("ShowZoomToolsBar").toBool());

  if (settings->contains("ShowFitToolsBar"))
  	fitToolsBar->setVisible(settings->value("ShowFitToolsBar").toBool());

  if (settings->contains("ShowModeToolsBar"))
  	modeToolsBar->setVisible(settings->value("ShowModeToolsBar").toBool());

  if (settings->contains("ShowMarkToolsBar"))
  	markToolsBar->setVisible(settings->value("ShowMarkToolsBar").toBool());

  settings->endGroup();
}

void XWTeXEditorMainWindow::saveToolsBarStatus()
{
	QSettings * settings = xwApp->getSettings();
	settings->setValue("ShowFileToolsBar", fileToolsBar->isVisible());
	settings->setValue("ShowSaveToolsBar", saveToolsBar->isVisible());
	settings->setValue("ShowEditToolsBar", editToolBar->isVisible());
	settings->setValue("ShowGotoToolsBar", gotoToolsBar->isVisible());
	settings->setValue("ShowZoomToolsBar", zoomToolsBar->isVisible());
	settings->setValue("ShowFitToolsBar", fitToolsBar->isVisible());
	settings->setValue("ShowModeToolsBar", modeToolsBar->isVisible());
	settings->setValue("ShowMarkToolsBar", markToolsBar->isVisible());
	settings->endGroup();
}

void XWTeXEditorMainWindow::setBackground()
{
	if (xwApp->hasSetting("MainWinOpacity"))
	{
		double op = xwApp->getSetting("MainWinOpacity").toDouble();
		if (op >= 0.0 && op <= 1.0)
			setWindowOpacity(op);
	}
}
