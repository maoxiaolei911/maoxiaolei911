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
#include "XWRecentFileWindow.h"
#include "XWTexFormatWindow.h"
#include "XWTexFontWindow.h"
#include "XWTeXConsole.h"
#include "XWTexDocFontSetting.h"
#include "XWTeXConsole.h"
#include "LaTeXKeyWord.h"
#include "BeamerKeyWord.h"
#include "XWToPDFWindow.h"
#include "XWTeXDocTemplateWindow.h"
#include "XWFontInfoWindow.h"
#include "XWExternalToolWindow.h"
#include "XWTeXDocumentObject.h"
#include "XWLaTeXDocument.h"
#include "XWTeXDocumentCore.h"
#include "XWToImageWindow.h"
#include "XWLaTeXEditorDev.h"
#include "XWLaTeXEditorCore.h"
#include "XWLaTeXFormularMainWindow.h"
#include "XWTikzMainWindow.h"
#include "XWTeXDocSearcher.h"
#include "XWLaTeXEditorMainWindow.h"

XWLaTeXEditorMainWindow::XWLaTeXEditorMainWindow()
{
	searcher = new XWTeXDocSearcher(this);
	topPage = 0;
	QString title = xwApp->getProductName();
 	setWindowTitle(title);
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

 	dev = new XWLaTeXEditorDev(this);

 	mdiArea = new QMdiArea;
   mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   setCentralWidget(mdiArea);

   windowMapper = new QSignalMapper(this);

   folderDock = new QDockWidget(tr("Folder"), this);
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

	 XWTeXDocSearchWidget * sw = new XWTeXDocSearchWidget(searcher, tab);
	tab->addTab(sw,tr("Search"));
	connect(sw, SIGNAL(positionActivated(int, double,double,double,double)), 
	         this, SLOT(showSearchResult(int, double,double,double,double)));

	XWTeXDocReplaceWidget * rw = new XWTeXDocReplaceWidget(searcher, tab);
	tab->addTab(rw,tr("Replace"));
	connect(rw, SIGNAL(positionActivated(int, double,double,double,double)), 
	            this, SLOT(showSearchResult(int, double,double,double,double)));

   coreDock = new QDockWidget(tr("Preview"), this);
   coreDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
   termDock = new QDockWidget(tr("Term Out"), this);
   termDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

	 core = new XWLaTeXEditorCore(this, this);
   coreDock->setWidget(core);
   addDockWidget(Qt::RightDockWidgetArea, coreDock);
   coreDock->toggleViewAction()->setChecked(false);

   termWin = new XWTeXConsole(this);
   termDock->setWidget(termWin);
   addDockWidget(Qt::BottomDockWidgetArea, termDock);
   termDock->toggleViewAction()->setChecked(false);

   createActions();
   createMenus();

   restoreToolsBarStatus();
	 setBackground();
	 updateMenus();

   connect(folder, SIGNAL(activated(const QModelIndex &)), this, SLOT(openFile(const QModelIndex &)));
	 connect(dev, SIGNAL(messageAdded(const QString &)), this, SLOT(insertTermText(const QString &)));
   connect(dev, SIGNAL(finished()), this, SLOT(updateShow()));
   connect(dev, SIGNAL(needFile(const QString &)), this, SLOT(openFile(const QString &)));
   connect(this, SIGNAL(fileDownloaded(const QString &)), dev, SLOT(setInputText(const QString &)));
	 connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
	 connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));
	 connect(dev, SIGNAL(requestInput()), termWin, SLOT(requstInput()));
   connect(termWin, SIGNAL(inputFinished(const QString &)), dev, SLOT(setInputText(const QString &)));
}

XWLaTeXEditorMainWindow::~XWLaTeXEditorMainWindow()
{}

bool XWLaTeXEditorMainWindow::loadFile(const QString & filename)
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
  	XWTeXDocumentCore *child = createMdiChild(fullname);
  	child->show();
  }

	return true;
}

bool XWLaTeXEditorMainWindow::loadFmt(const QString & fmt)
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

void XWLaTeXEditorMainWindow::editFormular()
{
	XWLaTeXFormularMainWindow * fedit = new XWLaTeXFormularMainWindow;
	XWLaTeXDocument * ldoc = currentDoc();
	if (ldoc)
	{
		QString str = ldoc->getFormular();
		if (!str.isEmpty())
		  fedit->setFormular(str);

		connect(fedit, SIGNAL(insert(const QString & )), ldoc, SLOT(insertFormular(const QString & )));
	}
	fedit->show();
}

void XWLaTeXEditorMainWindow::editTikz()
{
	XWTikzMainWindow * tedit = new XWTikzMainWindow;
	XWLaTeXDocument * ldoc = currentDoc();
	if (ldoc)
	{
		QString str = ldoc->getTikzPicture();
		if (!str.isEmpty())
		  tedit->setText(str);

		connect(tedit, SIGNAL(insert(const QString & )), ldoc, SLOT(insertTikz(const QString & )));
	}

	tedit->show();
}

void XWLaTeXEditorMainWindow::abstract()
{
	currentDoc()->abstract();
}

void XWLaTeXEditorMainWindow::address()
{
	currentDoc()->address();
}

void XWLaTeXEditorMainWindow::appendix()
{
	currentDoc()->appendix();
}

void XWLaTeXEditorMainWindow::array()
{
	currentDoc()->array();
}

void XWLaTeXEditorMainWindow::assumption()
{
	currentDoc()->insertAssumption();
}

void XWLaTeXEditorMainWindow::author()
{
	currentDoc()->author();
}

void XWLaTeXEditorMainWindow::axiom()
{
	currentDoc()->insertAxiom();
}

void XWLaTeXEditorMainWindow::build()
{
	topPage = core->getPageNum();
	XWLaTeXDocument * doc = currentDoc();
	QString filename = doc->getFileName();
	if (!doc->isMainFile())
	{
		filename = QFileDialog::getOpenFileName(this,
			                                      tr("Open main file"),
			                                      filename,
			                                      tr("Files (*.tex)"));
	}

	if (filename.isEmpty())
		return ;

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		xwApp->openError(filename, false);
		xwApp->showErrs();
		return ;
	}

	QTextStream in(&file);
	QString first = in.readLine();
	QString olddump = dev->getDumpName();
	bool hasfmt = dev->hasDumpName();
	if (!hasfmt && first.length() > 2 && first[0] == QChar('%') && first[1] == QChar('&'))
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
			return ;

		QString fmt = dlg.getFmt();
		hasfmt = loadFmt(fmt);
	}

	if (!hasfmt)
	{
		xwApp->showErrs();
		return ;
	}

	if (!dev->loadFile(filename))
	{
		xwApp->showErrs();
		return ;
	}

	core->setDoc(0);
	dev->compile();
}

void XWLaTeXEditorMainWindow::center()
{
	currentDoc()->insertCenter();
}

void XWLaTeXEditorMainWindow::cite()
{
	currentDoc()->cite();
}

void XWLaTeXEditorMainWindow::conjecture()
{
	currentDoc()->insertConjecture();
}

void XWLaTeXEditorMainWindow::corollary()
{
	currentDoc()->insertCorollary();
}

void XWLaTeXEditorMainWindow::date()
{
	currentDoc()->date();
}

void XWLaTeXEditorMainWindow::definition()
{
	currentDoc()->insertDefinition();
}

void XWLaTeXEditorMainWindow::description()
{
	currentDoc()->insertDescription();
}

void XWLaTeXEditorMainWindow::disformular()
{
	currentDoc()->displayFormular();
}

void XWLaTeXEditorMainWindow::eenumerate()
{
	currentDoc()->insertEnumerate();
}

void XWLaTeXEditorMainWindow::eqnarray()
{
	currentDoc()->eqnarray();
}

void XWLaTeXEditorMainWindow::eqnarrayStar()
{
	currentDoc()->eqnarrayStar();
}

void XWLaTeXEditorMainWindow::equation()
{
	currentDoc()->equation();
}

void XWLaTeXEditorMainWindow::example()
{
	currentDoc()->insertExample();
}

void XWLaTeXEditorMainWindow::exercise()
{
	currentDoc()->insertExercise();
}

void XWLaTeXEditorMainWindow::flushLeft()
{
	currentDoc()->insertFlushLeft();
}

void XWLaTeXEditorMainWindow::flushRight()
{
	currentDoc()->insertFlushRight();
}

void XWLaTeXEditorMainWindow::footnote()
{
	currentDoc()->footnote();
}

void XWLaTeXEditorMainWindow::formular()
{
	currentDoc()->formular();
}

void XWLaTeXEditorMainWindow::glossary()
{
	currentDoc()->glossary();
}

void XWLaTeXEditorMainWindow::include()
{
	currentDoc()->include();
}

void XWLaTeXEditorMainWindow::includeOnly()
{
	currentDoc()->includeOnly();
}

void XWLaTeXEditorMainWindow::includeGraphics()
{
	currentDoc()->includeGraphics();
}

void XWLaTeXEditorMainWindow::index()
{
	currentDoc()->index();
}

void XWLaTeXEditorMainWindow::input()
{
	currentDoc()->input();
}

void XWLaTeXEditorMainWindow::insertText(const QString & txt)
{
	XWLaTeXDocument * doc = currentDoc();
	if (!doc)
	  return ;

	doc->insertText(txt);
}

void XWLaTeXEditorMainWindow::insertTermText(const QString & txt)
{
	termWin->insertPlainText(txt);
	termWin->ensureCursorVisible();
}

void XWLaTeXEditorMainWindow::itemize()
{
	currentDoc()->insertItemize();
}

void XWLaTeXEditorMainWindow::label()
{
	currentDoc()->label();
}

void XWLaTeXEditorMainWindow::lemma()
{
	currentDoc()->insertLemma();
}

void XWLaTeXEditorMainWindow::list()
{
	currentDoc()->insertList();
}

void XWLaTeXEditorMainWindow::loadFmt()
{
	XWTexFormatWindow dlg(this);
	if (dlg.exec() != QDialog::Accepted)
		return ;

	QString fmt = dlg.getFmt();
	if (fmt.isEmpty())
		return ;

	loadFmt(fmt);
}

void XWLaTeXEditorMainWindow::location()
{
	currentDoc()->location();
}

void XWLaTeXEditorMainWindow::makeTitle()
{
	currentDoc()->maketitle();
}

void XWLaTeXEditorMainWindow::marginpar()
{
	currentDoc()->marginpar();
}

void XWLaTeXEditorMainWindow::name()
{
	currentDoc()->name();
}

void XWLaTeXEditorMainWindow::newFromTmplate()
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

	XWTeXDocumentCore *child = createMdiChild(filename);
  child->show();
}

void XWLaTeXEditorMainWindow::nocite()
{
	currentDoc()->nocite();
}

void XWLaTeXEditorMainWindow::open()
{
	QString path = getLocalPath();
	QString filename = QFileDialog::getOpenFileName(this,
		                                            tr("Open file"),
		                                            path,
		                                            tr("Files (*.tex *.txt)"));
	if (filename.isEmpty())
		return ;

	QMdiSubWindow *existing = findMdiChild(filename);
	if (existing)
	{
     mdiArea->setActiveSubWindow(existing);
     return;
  }

	XWTeXDocumentCore *child = createMdiChild(filename);
  child->show();
}

void XWLaTeXEditorMainWindow::openFolder()
{
	QString path = getLocalPath();
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 path,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
  if (dir.isEmpty())
	  return ;
	folderModel->setRootPath(dir);
	folder->setRootIndex(folderModel->index(dir));
	QString wintitle = QString(tr("Folder %1")).arg(dir);
	folder->setWindowTitle(wintitle);
}

void XWLaTeXEditorMainWindow::pageref()
{
	currentDoc()->pageref();
}

void XWLaTeXEditorMainWindow::picture()
{
	currentDoc()->picture();
}

void XWLaTeXEditorMainWindow::printIndex()
{
	currentDoc()->printIndex();
}

void XWLaTeXEditorMainWindow::printGlossary()
{
	currentDoc()->printGlossary();
}

void XWLaTeXEditorMainWindow::problem()
{
	currentDoc()->insertProblem();
}

void XWLaTeXEditorMainWindow::proof()
{
	currentDoc()->insertProof();
}

void XWLaTeXEditorMainWindow::proposition()
{
	currentDoc()->insertProposition();
}

void XWLaTeXEditorMainWindow::pstricksPicture()
{
	currentDoc()->pstricksPicture();
}

void XWLaTeXEditorMainWindow::quote()
{
	currentDoc()->insertQuote();
}

void XWLaTeXEditorMainWindow::quotation()
{
	currentDoc()->insertQuotation();
}

void XWLaTeXEditorMainWindow::ref()
{
	currentDoc()->ref();
}

void XWLaTeXEditorMainWindow::remark()
{
	currentDoc()->insertRemark();
}

void XWLaTeXEditorMainWindow::save()
{
	if (activeMdiChild())
    activeMdiChild()->save();
}

void XWLaTeXEditorMainWindow::saveToPicture()
{
	QString fullname = activeMdiChild()->getFileName();
	XWFileName fn(fullname);
	QString basename = fn.baseName();
	QString path = getLocalPath();
	path += QString("/%1").arg(basename);
	int pgnum = activeMdiChild()->getNumPages();
	XWToImageWindow d(path, pgnum, this);
	if (d.exec() != QDialog::Accepted)
		return ;

	QString filename = d.getFileName();
	if (filename.isEmpty())
		return ;

	QString fmt = d.getFormat();
	activeMdiChild()->saveToPic(filename, fmt);
}

void XWLaTeXEditorMainWindow::saveToDvi()
{
	XWLaTeXDocument * doc = currentDoc();
	QString fullname = doc->getFileName();
	if (!doc->isMainFile())
	{
		fullname = QFileDialog::getOpenFileName(this,
			                                      tr("Open main file"),
			                                      fullname,
			                                      tr("Files (*.tex)"));
	}

	if (fullname.isEmpty())
		return ;

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

void XWLaTeXEditorMainWindow::saveToPDF()
{
	XWLaTeXDocument * doc = currentDoc();
	QString fullname = doc->getFileName();
	if (!doc->isMainFile())
	{
		fullname = QFileDialog::getOpenFileName(this,
			                                      tr("Open main file"),
			                                      fullname,
			                                      tr("Files (*.tex)"));
	}

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

void XWLaTeXEditorMainWindow::setActiveSubWindow(QWidget *window)
{
	if (!window)
     return;
  mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void XWLaTeXEditorMainWindow::setEditor()
{
	XWTexDocFontSetting d(this);
	d.exec();
}

void XWLaTeXEditorMainWindow::setInputCodec(QAction * a)
{
	if (!a)
		return ;

	QString n = a->text();
	if (activeMdiChild())
    activeMdiChild()->setInputCodec(n);
}

void XWLaTeXEditorMainWindow::setOutputCodec(QAction * a)
{
	if (!a)
		return ;

	QString n = a->text();
	if (activeMdiChild())
    activeMdiChild()->setOutputCodec(n);
}

void XWLaTeXEditorMainWindow::setTexFonts()
{
	XWTexFontWindow win(this);
	win.exec();
}

void XWLaTeXEditorMainWindow::setTools()
{
	XWExternalToolWindow d(this);
	d.exec();
}

void XWLaTeXEditorMainWindow::showAboutHard()
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

void XWLaTeXEditorMainWindow::showAboutSoft()
{
	QString product = xwApp->getProductName();
	QString version = xwApp->getVersion();
	QString text = QString(tr("<h3>%1 %2</h3>"
	                         "<p>%3 is a LaTeX/Text document editor with source edit and preview.</p>")).arg(product).arg(version).arg(product);
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

void XWLaTeXEditorMainWindow::showFonts()
{
	XWFontInfoWindow win(this);
	win.exec();
}

void XWLaTeXEditorMainWindow::showLaTeXHelp()
{
	xwApp->getHelp("latex/latex");
}

void XWLaTeXEditorMainWindow::showPGFHelp()
{
	xwApp->getHelp("pgf/tikz");
}

void XWLaTeXEditorMainWindow::showPreamble()
{
	XWLaTeXDocument * doc = currentDoc();
	doc->setShowPreamble(!(doc->isShowPreamble()));
}

void XWLaTeXEditorMainWindow::showProductHelp()
{
	xwApp->getHelp("xwtex");
}

void XWLaTeXEditorMainWindow::showPSTHelp()
{
	xwApp->getHelp("pstricks/pstricks");
}

void XWLaTeXEditorMainWindow::signature()
{
	currentDoc()->signature();
}

void XWLaTeXEditorMainWindow::lecture()
{
	currentDoc()->matter(BMlecture);
}

void XWLaTeXEditorMainWindow::note()
{
	currentDoc()->matter(BMnote);
}

void XWLaTeXEditorMainWindow::mode()
{
	currentDoc()->matter(BMmode);
}

void XWLaTeXEditorMainWindow::againFrame()
{
	currentDoc()->matter(BMagainframe);
}

void XWLaTeXEditorMainWindow::tableOfContents()
{
	currentDoc()->tableOfContents();
}

void XWLaTeXEditorMainWindow::tabular()
{
	currentDoc()->tabular();
}

void XWLaTeXEditorMainWindow::tabularStar()
{
	currentDoc()->tabularStar();
}

void XWLaTeXEditorMainWindow::telephone()
{
	currentDoc()->telephone();
}

void XWLaTeXEditorMainWindow::text()
{
	currentDoc()->text();
}

void XWLaTeXEditorMainWindow::thanks()
{
	currentDoc()->thanks();
}

void XWLaTeXEditorMainWindow::thebibliography()
{
	currentDoc()->thebibliography();
}

void XWLaTeXEditorMainWindow::theorem()
{
	currentDoc()->insertTheorem();
}

void XWLaTeXEditorMainWindow::tikz()
{
	currentDoc()->tikz();
}

void XWLaTeXEditorMainWindow::tikzPicture()
{
	currentDoc()->tikzPicture();
}

void XWLaTeXEditorMainWindow::title()
{
	currentDoc()->title();
}

void XWLaTeXEditorMainWindow::titlePage()
{
	currentDoc()->titlePage();
}

void XWLaTeXEditorMainWindow::updateMenus()
{
	XWTeXDocumentCore * cur = activeMdiChild();
	if (!cur)
	{
		saveAct->setVisible(false);
		fmtAct->setVisible(false);
		toPicAct->setVisible(false);
		toDviAct->setVisible(false);
		toPDFAct->setVisible(false);
		editMenu->setEnabled(false);
		viewMenu->setEnabled(false);
		insertMenu->setEnabled(false);

		editToolBar->setEnabled(false);
		buildToolBar->setEnabled(false);
		sectionBar->setEnabled(false);
		docToolsBar->setEnabled(false);
		viewToolsBar->setEnabled(false);
		markToolsBar->setEnabled(false);

		return ;
	}

  toPicAct->setVisible(true);
  bool v = xwApp->getLicenseState() == APP_STATE_NORMAL;
	toDviAct->setVisible(v);
	toPDFAct->setVisible(v);

	saveAct->setVisible(true);
	fmtAct->setVisible(true);
	editMenu->setEnabled(true);
	viewMenu->setEnabled(true);
	insertMenu->setEnabled(true);
	editToolBar->setEnabled(true);
	buildToolBar->setEnabled(true);
	sectionBar->setEnabled(true);
	docToolsBar->setEnabled(true);
	viewToolsBar->setEnabled(true);
	markToolsBar->setEnabled(true);

	XWLaTeXDocument * doc = currentDoc();
	v = doc->isMainFile();
	usepackageAct->setVisible(v);
	useTikzLibraryAct->setVisible(v);
	useThemeAct->setVisible(v && doc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER);
	includeLectureAct->setVisible(v && doc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER);
	includeOnlyAct->setVisible(v);

  dateAct->setVisible(v && !doc->hasDate());

	v = doc->isMainFile() && (doc->getClassIndex() == XW_LTX_DOC_CLS_LETTER);
	addressAct->setVisible(v && !doc->hasAddress());
	locationAct->setVisible(v && !doc->hasLocation());
	nameAct->setVisible(v && !doc->hasName());
	signatureAct->setVisible(v && !doc->hasSignature());
	telephoneAct->setVisible(v && !doc->hasTelephone());

  v = doc->isMainFile() && (doc->getClassIndex() != XW_LTX_DOC_CLS_LETTER);
	authorAct->setVisible(v && !doc->hasAuthor());
	thanksAct->setVisible(v && !doc->hasThanks());
	titleAct->setVisible(v && !doc->hasTitle());

  classCombo->clear();
	int t = doc->getClassIndex();
	switch (t)
	{
		case XW_LTX_DOC_CLS_ARTICLE:
		  classCombo->addItem(tr("article"), XW_LTX_DOC_CLS_ARTICLE);
			columnCombo->setDisabled(false);
		  break;

		case XW_LTX_DOC_CLS_LETTER:
		  classCombo->addItem(tr("letter"), XW_LTX_DOC_CLS_LETTER);
			columnCombo->setDisabled(true);
		  break;

		case XW_LTX_DOC_CLS_BEAMER:
		  classCombo->addItem(tr("beamer"), XW_LTX_DOC_CLS_BEAMER);
		  columnCombo->setDisabled(true);
		  break;

		default:
		  columnCombo->setDisabled(false);
		  classCombo->addItem(tr("book"), XW_LTX_DOC_CLS_BOOK);
			classCombo->addItem(tr("report"), XW_LTX_DOC_CLS_REPORT);
			if (t == XW_LTX_DOC_CLS_BOOK)
			  classCombo->setCurrentIndex(0);
			else
			  classCombo->setCurrentIndex(1);
		  break;
	}

	paperCombo->setDisabled(false);
	paperCombo->clear();
	if (doc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
	{
		if (doc->isShowPreamble())
		{
			QStringList papers = XWTeXDocumentCore::getPapers();
			paperCombo->addItems(papers);
			paperCombo->setCurrentIndex(4);
			paperCombo->setDisabled(true);
		}
		else
		{
			QStringList aspects = XWTeXDocumentCore::getSlideAspect();
			paperCombo->addItems(aspects);
			t = cur->getPaperIndex();
			paperCombo->setCurrentIndex(t);
		}
	}
	else
	{
		QStringList papers = XWTeXDocumentCore::getPapers();
		paperCombo->addItems(papers);
		t = cur->getPaperIndex();
		paperCombo->setCurrentIndex(t);
	}

	t = cur->getFontIndex();
	fontCombo->setCurrentIndex(t);
	t = cur->getDirection();
	int i = dirCombo->findData(t);
	dirCombo->setCurrentIndex(i);
	XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(cur->document());
	if (ldoc->isTwocolumn())
	  columnCombo->setCurrentIndex(1);
	else
	  columnCombo->setCurrentIndex(0);

	if (cur->isContinuousMode())
	  modeCombo->setCurrentIndex(0);
	else
	  modeCombo->setCurrentIndex(1);

	t = cur->getZoomIdx();
	zoomCombo->setCurrentIndex(t);

	QString c = cur->getInputCodecName();
	findInputCodecName(c);
	c = cur->getOutputCodecName();
	findOutputCodecName(c);
}

void XWLaTeXEditorMainWindow::updateShow()
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

	if (topPage > 0)
		core->displayPage(topPage);
}

void XWLaTeXEditorMainWindow::updateWindowMenu()
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
  	XWTeXDocumentCore *child = qobject_cast<XWTeXDocumentCore *>(windows.at(i)->widget());
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

void XWLaTeXEditorMainWindow::usepackage()
{
	currentDoc()->usePackage();
}

void XWLaTeXEditorMainWindow::useTikzLibrary()
{
	currentDoc()->tikzUseTikzLibrary();
}

void XWLaTeXEditorMainWindow::useTheme()
{
	currentDoc()->useTheme();
}

void XWLaTeXEditorMainWindow::includeOnlyLecture()
{
	currentDoc()->includeOnlyLecture();
}

void XWLaTeXEditorMainWindow::verbatim()
{
	currentDoc()->verbatim();
}

void XWLaTeXEditorMainWindow::verse()
{
	currentDoc()->insertVerse();
}

void XWLaTeXEditorMainWindow::zoom(int idx)
{
	activeMdiChild()->zoom(idx);
}

void XWLaTeXEditorMainWindow::clearMark()
{
	activeMdiChild()->clearMark();
}

void XWLaTeXEditorMainWindow::openFile(const QString & filename)
{
	QString ret = xwApp->getFile(filename);
	emit fileDownloaded(ret);
}

void XWLaTeXEditorMainWindow::openFile(const QModelIndex & index)
{
	QString filename = folderModel->filePath(index);
	QMdiSubWindow *existing = findMdiChild(filename);
	if (existing)
	{
     mdiArea->setActiveSubWindow(existing);
     return;
  }

	XWTeXDocumentCore *child = createMdiChild(filename);
  child->show();
}

void XWLaTeXEditorMainWindow::sections(int index)
{
	int t = sectionCombo->itemData(index).toInt();
	XWLaTeXDocument * doc = currentDoc();
	if (t == doc->getCurrenrObjectType())
	  return ;

	switch (t)
	{
		case LAabstract:
		  doc->abstract();
		  break;

		case LApart:
		case LAchapter:
		case LAsection:
		case LAsubsection:
		case LAsubsubsection:
		case LAparagraph:
		case LAsubparagraph:
		case LArealparagraph:
		  doc->section(t);
		  break;

		case LAdescription:
		case LAenumerate:
		case LAitemize:
		case LAlist:
		  doc->itemEnv(t);
		  break;

		case LAcenter:
		case LAflushleft:
		case LAflushright:
		case LAverse:
		  doc->alignEnv(t);
		  break;

		case LAquote:
	 	case LAquotation:
		case LAtheorem:
		case LAproof:
		case LAassumption:
		case LAdefinition:
		case LAproposition:
		case LAlemma:
		case LAaxiom:
		case LAcorollary:
		case LAconjecture:
		case LAexercise:
		case LAexample:
		case LAremark:
		case LAproblem:
		case BMonlyenv:
		case BMactionenv:
		case BMstructureenv:
		case BMalertenv:
		  doc->parEnv(t);
		  break;

		case BMoverlayarea:
		case BMoverprint:
		  doc->beamerDynChange(t);
			break;

		case BMblock:
		case BMalertblock:
		case BMexampleblock:
		  doc->beamerBlock(t);
			break;

		case BMbeamercolorbox:
		case BMbeamerboxesrounded:
		  doc->beamerBoxed(t);
			break;

		case BMcolumns:
		  doc->beamerColumns();
		  break;

		default:
		  break;
	}
}

void XWLaTeXEditorMainWindow::setClass(int index)
{
	int t = classCombo->itemData(index).toInt();
	if (t == XW_LTX_DOC_CLS_BOOK || t == XW_LTX_DOC_CLS_REPORT)
	  currentDoc()->setClassIndex(t);
}

void XWLaTeXEditorMainWindow::setColumn(int index)
{
	if (index == 0)
	  currentDoc()->setTwoColumn(false);
	else
	  currentDoc()->setTwoColumn(true);
}

void XWLaTeXEditorMainWindow::setDirection(int index)
{
	int t = dirCombo->itemData(index).toInt();
	activeMdiChild()->setDirection(t);
}

void XWLaTeXEditorMainWindow::setFont(int index)
{
	activeMdiChild()->setFontSize(index);
}

void XWLaTeXEditorMainWindow::setMarking(bool e)
{
	activeMdiChild()->setMarking(e);
}

void XWLaTeXEditorMainWindow::setMode(int index)
{
	if (index == 0)
	  activeMdiChild()->setContinuousMode(true);
	else
	  activeMdiChild()->setContinuousMode(false);
}

void XWLaTeXEditorMainWindow::setPaper(int index)
{
	activeMdiChild()->setPaper(index);
}

void XWLaTeXEditorMainWindow::showFindDialog()
{
	XWTeXDocumentCore * cur = activeMdiChild();
	if (!cur)
	  return ;

	cur->showFindDialog();
}

void XWLaTeXEditorMainWindow::showReplaceDialog()
{
	XWTeXDocumentCore * cur = activeMdiChild();
	if (!cur)
	  return ;

	cur->showReplaceDialog();
}

void XWLaTeXEditorMainWindow::showSearchResult(int pg, double minx, double miny, double maxx,double maxy)
{
	XWTeXDocumentCore * cur = activeMdiChild();
	if (!cur)
	  return ;

	cur->displayPage(pg, minx, miny, maxx, maxy);
}

void XWLaTeXEditorMainWindow::updateActions()
{
	XWLaTeXDocument * doc = currentDoc();
	bool v = doc->canMatter();
	maketitleAct->setVisible(v);
	titlePageAct->setVisible(v);
	tableOfContentsAct->setVisible(v);
	abstractAct->setVisible(v);
	appendixAct->setVisible(v);
	printIndexAct->setVisible(v);
	printGlossaryAct->setVisible(v);
	thebibliographyAct->setVisible(v);
	lectureAct->setVisible(v && doc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER);
	noteAct->setVisible(v && doc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER);
	modeAct->setVisible(v && doc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER);
	againFrameAct->setVisible(v && doc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER);

	v = doc->canInsertObject();
	quoteAct->setVisible(v);
	quotationAct->setVisible(v);
	verseAct->setVisible(v);
	theoremAct->setVisible(v);
	proofAct->setVisible(v);
	assumptionAct->setVisible(v);
	definitionAct->setVisible(v);
	propositionAct->setVisible(v);
	lemmaAct->setVisible(v);
	axiomAct->setVisible(v);
	corollaryAct->setVisible(v);
	conjectureAct->setVisible(v);
	exerciseAct->setVisible(v);
	exampleAct->setVisible(v);
	remarkAct->setVisible(v);
	problemAct->setVisible(v);
	centerAct->setVisible(v);
	flushLeftAct->setVisible(v);
	flushRightAct->setVisible(v);
	descriptionAct->setVisible(v);
	eenumerateAct->setVisible(v);
	itemizeAct->setVisible(v);
	arrayAct->setVisible(v);
	eqnarrayAct->setVisible(v);
	eqnarrayStarAct->setVisible(v);
	equationAct->setVisible(v);
	tabularAct->setVisible(v);
	tabularStarAct->setVisible(v);
	includeGraphicsAct->setVisible(v);
	pictureAct->setVisible(v);
	tikzAct->setVisible(v);
	tikzPictureAct->setVisible(v);
	pstricksPictureAct->setVisible(v);
	verbatimAct->setVisible(v);
	footnoteAct->setVisible(v);
	marginparAct->setVisible(v);
	labelAct->setVisible(v);
	pagerefAct->setVisible(v);
	refAct->setVisible(v);
	indexAct->setVisible(v);
	glossaryAct->setVisible(v);
	nociteAct->setVisible(v);
	citeAct->setVisible(v);
	includeAct->setVisible(v);
	inputAct->setVisible(v);

	v = (doc->geCurrentBlockType() == TEX_DOC_B_FOMULAR || doc->geCurrentBlockType() == TEX_DOC_B_DISFOMULAR);
	textAct->setVisible(v);
	v = (doc->geCurrentBlockType() == TEX_DOC_B_TEXT || doc->geCurrentBlockType() == TEX_DOC_B_DISFOMULAR);
	formularAct->setVisible(v);
	v = (doc->geCurrentBlockType() == TEX_DOC_B_TEXT || doc->geCurrentBlockType() == TEX_DOC_B_FOMULAR);
	disformularAct->setVisible(v);

	int t = doc->getCurrenrObjectType();
	sectionCombo->clear();
	switch (t)
	{
		case LApart:
		case LAchapter:
		case LAsection:
		case LAsubsection:
		case LAsubsubsection:
		case LAparagraph:
		case LAsubparagraph:
		  sectionCombo->addItem(tr("normal"),LArealparagraph);
		  sectionCombo->addItem(tr("part"),LApart);
		  sectionCombo->addItem(tr("chapter"),LAchapter);
		  sectionCombo->addItem(tr("section"),LAsection);
		  sectionCombo->addItem(tr("subsection"),LAsubsection);
		  sectionCombo->addItem(tr("subsubsection"),LAsubsubsection);
		  sectionCombo->addItem(tr("paragraph"),LAparagraph);
		  sectionCombo->addItem(tr("subparagraph"),LAsubparagraph);			
		  break;

		case LArealparagraph:
		  sectionCombo->addItem(tr("normal"),LArealparagraph);
			if (!doc->isFramePar())
			{
				sectionCombo->addItem(tr("part"),LApart);
			  sectionCombo->addItem(tr("chapter"),LAchapter);
			  sectionCombo->addItem(tr("section"),LAsection);
			  sectionCombo->addItem(tr("subsection"),LAsubsection);
			  sectionCombo->addItem(tr("subsubsection"),LAsubsubsection);
			  sectionCombo->addItem(tr("paragraph"),LAparagraph);
			  sectionCombo->addItem(tr("subparagraph"),LAsubparagraph);
			}
		  sectionCombo->addItem(tr("quote"),LAquote);
		  sectionCombo->addItem(tr("quotation"),LAquotation);
		  sectionCombo->addItem(tr("verse"),LAverse);
		  sectionCombo->addItem(tr("description"),LAdescription);
		  sectionCombo->addItem(tr("enumerate"),LAenumerate);
		  sectionCombo->addItem(tr("itemize"),LAitemize);
		  sectionCombo->addItem(tr("list"),LAlist);
		  sectionCombo->addItem(tr("center"),LAcenter);
		  sectionCombo->addItem(tr("flushleft"),LAflushleft);
			sectionCombo->addItem(tr("flushright"),LAflushright);
		  sectionCombo->addItem(tr("theorem"),LAtheorem);
		  sectionCombo->addItem(tr("proof"),LAproof);
		  sectionCombo->addItem(tr("definition"),LAdefinition);
		  sectionCombo->addItem(tr("proposition"),LAproposition);
		  sectionCombo->addItem(tr("lemma"),LAlemma);
		  sectionCombo->addItem(tr("axiom"),LAaxiom);
		  sectionCombo->addItem(tr("corollary"),LAcorollary);
		  sectionCombo->addItem(tr("conjecture"),LAconjecture);
		  sectionCombo->addItem(tr("exercise"),LAexercise);
		  sectionCombo->addItem(tr("example"),LAexample);
		  sectionCombo->addItem(tr("remark"),LAremark);
		  sectionCombo->addItem(tr("problem"),LAproblem);
		  sectionCombo->addItem(tr("abstract"),LAabstract);
			if (doc->isFramePar())
			{
				sectionCombo->addItem(tr("onlyenv"),BMonlyenv);
				sectionCombo->addItem(tr("actionenv"),BMactionenv);
				sectionCombo->addItem(tr("structureenv"),BMstructureenv);
				sectionCombo->addItem(tr("alertenv"),BMalertenv);
				sectionCombo->addItem(tr("overlayarea"),BMoverlayarea);
				sectionCombo->addItem(tr("overprint"),BMoverprint);
				sectionCombo->addItem(tr("block"),BMblock);
				sectionCombo->addItem(tr("alertblock"),BMalertblock);
				sectionCombo->addItem(tr("exampleblock"),BMexampleblock);
				sectionCombo->addItem(tr("colorbox"),BMbeamercolorbox);
				sectionCombo->addItem(tr("boxesrounded"),BMbeamerboxesrounded);
				sectionCombo->addItem(tr("columns"),BMcolumns);
			}
		  break;

		case LAdescription:
		case LAenumerate:
		case LAitemize:
		case LAlist:
		  sectionCombo->addItem(tr("description"),LAdescription);
		  sectionCombo->addItem(tr("enumerate"),LAenumerate);
		  sectionCombo->addItem(tr("itemize"),LAitemize);
		  break;

		case LAcenter:
		case LAflushleft:
		case LAflushright:
		  sectionCombo->addItem(tr("center"),LAcenter);
		  sectionCombo->addItem(tr("flushleft"),LAflushleft);
		  sectionCombo->addItem(tr("flushright"),LAflushright);
		  break;

		case LAquote:
		case LAquotation:
		case LAtheorem:
		case LAproof:
		case LAassumption:
		case LAdefinition:
		case LAproposition:
		case LAlemma:
		case LAaxiom:
		case LAcorollary:
		case LAconjecture:
		case LAexercise:
		case LAexample:
		case LAremark:
		case LAproblem:
		  sectionCombo->addItem(tr("quote"),LAquote);
		  sectionCombo->addItem(tr("quotation"),LAquotation);
			sectionCombo->addItem(tr("theorem"),LAtheorem);
		  sectionCombo->addItem(tr("proof"),LAproof);
		  sectionCombo->addItem(tr("definition"),LAdefinition);
		  sectionCombo->addItem(tr("proposition"),LAproposition);
		  sectionCombo->addItem(tr("lemma"),LAlemma);
		  sectionCombo->addItem(tr("axiom"),LAaxiom);
		  sectionCombo->addItem(tr("corollary"),LAcorollary);
		  sectionCombo->addItem(tr("conjecture"),LAconjecture);
		  sectionCombo->addItem(tr("exercise"),LAexercise);
		  sectionCombo->addItem(tr("example"),LAexample);
		  sectionCombo->addItem(tr("remark"),LAremark);
		  sectionCombo->addItem(tr("problem"),LAproblem);
		  break;

		case BMonlyenv:
		case BMactionenv:
		  sectionCombo->addItem(tr("onlyenv"),BMonlyenv);
		  sectionCombo->addItem(tr("actionenv"),BMactionenv);
			break;

		case BMstructureenv:
		case BMalertenv:
			sectionCombo->addItem(tr("structureenv"),BMstructureenv);
			sectionCombo->addItem(tr("alertenv"),BMalertenv);
			break;

		case BMoverlayarea:
		case BMoverprint:
			sectionCombo->addItem(tr("overlayarea"),BMoverlayarea);
			sectionCombo->addItem(tr("overprint"),BMoverprint);
			break;

		case BMblock:
		case BMalertblock:
		case BMexampleblock:
			sectionCombo->addItem(tr("block"),BMblock);
			sectionCombo->addItem(tr("alertblock"),BMalertblock);
			sectionCombo->addItem(tr("exampleblock"),BMexampleblock);
			break;

		case BMbeamercolorbox:
		case BMbeamerboxesrounded:
			sectionCombo->addItem(tr("colorbox"),BMbeamercolorbox);
			sectionCombo->addItem(tr("boxesrounded"),BMbeamerboxesrounded);
			break;

		case BMcolumns:
			sectionCombo->addItem(tr("columns"),BMcolumns);
			break;

		default:
		  break;
	}


	int i = sectionCombo->findData(t);
	if (i == -1)
	  sectionCombo->setDisabled(true);
	else
	{
		sectionCombo->setCurrentIndex(i);
		sectionCombo->setDisabled(false);
	}
}

void XWLaTeXEditorMainWindow::closeEvent(QCloseEvent *event)
{
	saveToolsBarStatus();
	xwApp->cleanup();
	QMainWindow::closeEvent(event);
}

XWTeXDocumentCore * XWLaTeXEditorMainWindow::activeMdiChild()
{
	if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
	{
		XWTeXDocumentCore * ret = qobject_cast<XWTeXDocumentCore *>(activeSubWindow->widget());
		searcher->setDoc(ret->document());
		return ret;
	}
     
  return 0;
}

void XWLaTeXEditorMainWindow::createActions()
{
	//File
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
	saveAct->setVisible(false);
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	fmtAct = new QAction(tr("Load format"), this);
	fmtAct->setPriority(QAction::LowPriority);
	fmtAct->setVisible(false);
	connect(fmtAct, SIGNAL(triggered()), this, SLOT(loadFmt()));

	toPicAct = new QAction(tr("Save to Picture"), this);
	connect(toPicAct, SIGNAL(triggered()), this, SLOT(saveToPicture()));

	toDviAct = new QAction(tr("Save to DVI"), this);
	toDviAct->setStatusTip(tr("Save as DVI file"));
	toDviAct->setVisible(false);
	connect(toDviAct, SIGNAL(triggered()), this, SLOT(saveToDvi()));

	toPDFAct = new QAction(QIcon(":/images/filepdf.png"), tr("Save to pdf"), this);
	toPDFAct->setStatusTip(tr("Save as pdf file"));
	toPDFAct->setVisible(false);
	connect(toPDFAct, SIGNAL(triggered()), this, SLOT(saveToPDF()));

	exitAct = new QAction(tr("&Quit"), this);
	exitAct->setShortcut(Qt::CTRL + Qt::Key_Q);
	connect(exitAct, SIGNAL(triggered()), xwApp, SLOT(quit()));

	findAct = new QAction(tr("&Find"), this);
	findAct->setShortcuts(QKeySequence::Find);
	connect(findAct, SIGNAL(triggered()), this, SLOT(showFindDialog()));

	replaceAct = new QAction(tr("&Replace"), this);
	replaceAct->setShortcuts(QKeySequence::Replace);
	connect(replaceAct, SIGNAL(triggered()), this, SLOT(showReplaceDialog()));

  //codec
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

  //view
	showPreambleAct = new QAction(tr("Show preamble"), this);
	connect(showPreambleAct, SIGNAL(triggered()), this, SLOT(showPreamble()));

	//insert
	usepackageAct = new QAction(tr("usepackage"), this);
	connect(usepackageAct, SIGNAL(triggered()), this, SLOT(usepackage()));

	useTikzLibraryAct = new QAction(tr("usetikzlibrary"), this);
	connect(useTikzLibraryAct, SIGNAL(triggered()), this, SLOT(useTikzLibrary()));

	useThemeAct = new QAction(tr("usetheme"), this);
	connect(useThemeAct, SIGNAL(triggered()), this, SLOT(useTheme()));

	includeLectureAct = new QAction(tr("includeonlylecture"), this);
	connect(includeLectureAct, SIGNAL(triggered()), this, SLOT(includeOnlyLecture()));

	addressAct = new QAction(tr("address"), this);
	connect(addressAct, SIGNAL(triggered()), this, SLOT(address()));

	authorAct = new QAction(tr("author"), this);
	connect(authorAct, SIGNAL(triggered()), this, SLOT(author()));

	dateAct = new QAction(tr("date"), this);
	connect(dateAct, SIGNAL(triggered()), this, SLOT(date()));

	includeOnlyAct = new QAction(tr("includeonly"), this);
	connect(includeOnlyAct, SIGNAL(triggered()), this, SLOT(includeOnly()));

	locationAct = new QAction(tr("location"), this);
	connect(locationAct, SIGNAL(triggered()), this, SLOT(location()));

	nameAct = new QAction(tr("name"), this);
	connect(nameAct, SIGNAL(triggered()), this, SLOT(name()));

	signatureAct = new QAction(tr("signature"), this);
	connect(signatureAct, SIGNAL(triggered()), this, SLOT(signature()));

	telephoneAct = new QAction(tr("telephone"), this);
	connect(telephoneAct, SIGNAL(triggered()), this, SLOT(telephone()));

	thanksAct = new QAction(tr("thanks"), this);
	connect(thanksAct, SIGNAL(triggered()), this, SLOT(thanks()));

	titleAct = new QAction(tr("title"), this);
	connect(titleAct, SIGNAL(triggered()), this, SLOT(title()));

	maketitleAct = new QAction(tr("maketitle"), this);
	connect(maketitleAct, SIGNAL(triggered()), this, SLOT(makeTitle()));

	titlePageAct = new QAction(tr("titlepage"), this);
	connect(titlePageAct, SIGNAL(triggered()), this, SLOT(titlePage()));

	tableOfContentsAct = new QAction(tr("tableofcontents"), this);
	connect(tableOfContentsAct, SIGNAL(triggered()), this, SLOT(tableOfContents()));

	abstractAct = new QAction(tr("abstract"), this);
	connect(abstractAct, SIGNAL(triggered()), this, SLOT(abstract()));

	appendixAct = new QAction(tr("appendix"), this);
	connect(appendixAct, SIGNAL(triggered()), this, SLOT(appendix()));

	printIndexAct = new QAction(tr("printindex"), this);
	connect(printIndexAct, SIGNAL(triggered()), this, SLOT(printIndex()));

	printGlossaryAct = new QAction(tr("printglossary"), this);
	connect(printGlossaryAct, SIGNAL(triggered()), this, SLOT(printGlossary()));

	thebibliographyAct = new QAction(tr("thebibliography"), this);
	connect(thebibliographyAct, SIGNAL(triggered()), this, SLOT(thebibliography()));

	lectureAct = new QAction(tr("lecture"), this);
	connect(lectureAct, SIGNAL(triggered()), this, SLOT(lecture()));

	noteAct = new QAction(tr("note"), this);
	connect(noteAct, SIGNAL(triggered()), this, SLOT(note()));

	modeAct = new QAction(tr("mode"), this);
	connect(modeAct, SIGNAL(triggered()), this, SLOT(mode()));

	againFrameAct = new QAction(tr("againframe"), this);
	connect(againFrameAct, SIGNAL(triggered()), this, SLOT(againFrame()));

	quoteAct = new QAction(tr("quote"), this);
	connect(quoteAct, SIGNAL(triggered()), this, SLOT(quote()));

	quotationAct = new QAction(tr("quotation"), this);
	connect(quotationAct, SIGNAL(triggered()), this, SLOT(quotation()));

	verseAct = new QAction(tr("verse"), this);
	connect(verseAct, SIGNAL(triggered()), this, SLOT(verse()));

	theoremAct = new QAction(tr("theorem"), this);
	connect(theoremAct, SIGNAL(triggered()), this, SLOT(theorem()));

	proofAct = new QAction(tr("proof"), this);
	connect(proofAct, SIGNAL(triggered()), this, SLOT(proof()));

	assumptionAct = new QAction(tr("assumption"), this);
	connect(assumptionAct, SIGNAL(triggered()), this, SLOT(assumption()));

	definitionAct = new QAction(tr("definition"), this);
	connect(definitionAct, SIGNAL(triggered()), this, SLOT(definition()));

	propositionAct = new QAction(tr("proposition"), this);
	connect(propositionAct, SIGNAL(triggered()), this, SLOT(proposition()));

	lemmaAct = new QAction(tr("lemma"), this);
	connect(lemmaAct, SIGNAL(triggered()), this, SLOT(lemma()));

	axiomAct = new QAction(tr("axiom"), this);
	connect(axiomAct, SIGNAL(triggered()), this, SLOT(axiom()));

	corollaryAct = new QAction(tr("corollary"), this);
	connect(corollaryAct, SIGNAL(triggered()), this, SLOT(corollary()));

	conjectureAct = new QAction(tr("conjecture"), this);
	connect(conjectureAct, SIGNAL(triggered()), this, SLOT(conjecture()));

	exerciseAct = new QAction(tr("exercise"), this);
	connect(exerciseAct, SIGNAL(triggered()), this, SLOT(exercise()));

	exampleAct = new QAction(tr("example"), this);
	connect(exampleAct, SIGNAL(triggered()), this, SLOT(example()));

	remarkAct = new QAction(tr("remark"), this);
	connect(remarkAct, SIGNAL(triggered()), this, SLOT(remark()));

	problemAct = new QAction(tr("problem"), this);
	connect(problemAct, SIGNAL(triggered()), this, SLOT(problem()));

	centerAct = new QAction(tr("center"), this);
	connect(centerAct, SIGNAL(triggered()), this, SLOT(center()));

	flushLeftAct = new QAction(tr("flushleft"), this);
	connect(flushLeftAct, SIGNAL(triggered()), this, SLOT(flushLeft()));

	flushRightAct = new QAction(tr("flushright"), this);
	connect(flushRightAct, SIGNAL(triggered()), this, SLOT(flushRight()));

	descriptionAct = new QAction(tr("description"), this);
	connect(descriptionAct, SIGNAL(triggered()), this, SLOT(description()));

	eenumerateAct = new QAction(tr("enumerate"), this);
	connect(eenumerateAct, SIGNAL(triggered()), this, SLOT(eenumerate()));

	itemizeAct = new QAction(tr("itemize"), this);
	connect(itemizeAct, SIGNAL(triggered()), this, SLOT(itemize()));

	arrayAct = new QAction(tr("array"), this);
	connect(arrayAct, SIGNAL(triggered()), this, SLOT(array()));

	eqnarrayAct = new QAction(tr("eqnarray"), this);
	connect(eqnarrayAct, SIGNAL(triggered()), this, SLOT(eqnarray()));

	eqnarrayStarAct = new QAction(tr("eqnarray*"), this);
	connect(eqnarrayStarAct, SIGNAL(triggered()), this, SLOT(eqnarrayStar()));

	equationAct = new QAction(tr("equation"), this);
	connect(equationAct, SIGNAL(triggered()), this, SLOT(equation()));

	tabularAct = new QAction(tr("tabular"), this);
	connect(tabularAct, SIGNAL(triggered()), this, SLOT(tabular()));

	tabularStarAct = new QAction(tr("tabular*"), this);
	connect(tabularStarAct, SIGNAL(triggered()), this, SLOT(tabularStar()));

	includeGraphicsAct = new QAction(tr("includegraphics"), this);
	connect(includeGraphicsAct, SIGNAL(triggered()), this, SLOT(includeGraphics()));

	pictureAct = new QAction(tr("picture"), this);
	connect(pictureAct, SIGNAL(triggered()), this, SLOT(picture()));

	tikzAct = new QAction(tr("tikz"), this);
	connect(tikzAct, SIGNAL(triggered()), this, SLOT(tikz()));

	tikzPictureAct = new QAction(tr("tikzpicture"), this);
	connect(tikzPictureAct, SIGNAL(triggered()), this, SLOT(tikzPicture()));

	pstricksPictureAct = new QAction(tr("pstrickspicture"), this);
	connect(pstricksPictureAct, SIGNAL(triggered()), this, SLOT(pstricksPicture()));

	verbatimAct = new QAction(tr("verbatim"), this);
	connect(verbatimAct, SIGNAL(triggered()), this, SLOT(verbatim()));

	footnoteAct = new QAction(tr("footnote"), this);
	connect(footnoteAct, SIGNAL(triggered()), this, SLOT(footnote()));

	marginparAct = new QAction(tr("marginpar"), this);
	connect(marginparAct, SIGNAL(triggered()), this, SLOT(marginpar()));

	labelAct = new QAction(tr("label"), this);
	connect(labelAct, SIGNAL(triggered()), this, SLOT(label()));

	pagerefAct = new QAction(tr("pageref"), this);
	connect(pagerefAct, SIGNAL(triggered()), this, SLOT(pageref()));

	refAct = new QAction(tr("ref"), this);
	connect(refAct, SIGNAL(triggered()), this, SLOT(ref()));

	indexAct = new QAction(tr("index"), this);
	connect(indexAct, SIGNAL(triggered()), this, SLOT(index()));

	glossaryAct = new QAction(tr("glossary"), this);
	connect(glossaryAct, SIGNAL(triggered()), this, SLOT(glossary()));

	nociteAct = new QAction(tr("nocite"), this);
	connect(nociteAct, SIGNAL(triggered()), this, SLOT(nocite()));

	citeAct = new QAction(tr("cite"), this);
	connect(citeAct, SIGNAL(triggered()), this, SLOT(cite()));

	includeAct = new QAction(tr("include"), this);
	connect(includeAct, SIGNAL(triggered()), this, SLOT(include()));

	inputAct = new QAction(tr("input"), this);
	connect(inputAct, SIGNAL(triggered()), this, SLOT(input()));

  //tools
	formularEditAct = new QAction(tr("Fomular Editor"), this);
	connect(formularEditAct, SIGNAL(triggered()), this, SLOT(editFormular()));

	tikzEditAct = new QAction(tr("Tikz Editor"), this);
	connect(tikzEditAct, SIGNAL(triggered()), this, SLOT(editTikz()));

	editorSettingAct = new QAction(tr("Editor setting"), this);
	connect(editorSettingAct, SIGNAL(triggered()), this, SLOT(setEditor()));

	texFontSettingAct = new QAction(tr("TeX fonts setting"), this);
	connect(texFontSettingAct, SIGNAL(triggered()), this, SLOT(setTexFonts()));

	fontInfoAct = new QAction(tr("Font information"), this);
	connect(fontInfoAct, SIGNAL(triggered()), this, SLOT(showFonts()));

	externalToolsAct = new QAction(tr("External tools"), this);
	connect(externalToolsAct, SIGNAL(triggered()), this, SLOT(setTools()));

  //help
	QString p = xwApp->getProductName();
	productHelpAct = new QAction(p, this);
	connect(productHelpAct, SIGNAL(triggered()), this, SLOT(showProductHelp()));

	latexHelpAct = new QAction(tr("LaTeX2e"), this);
	connect(latexHelpAct, SIGNAL(triggered()), this, SLOT(showLaTeXHelp()));

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

	textAct = new QAction(tr("text"), this);
  textAct->setPriority(QAction::LowPriority);
	connect(textAct, SIGNAL(triggered()), this, SLOT(text()));

	formularAct = new QAction(tr("formular"), this);
  formularAct->setPriority(QAction::LowPriority);
	connect(formularAct, SIGNAL(triggered()), this, SLOT(formular()));

	disformularAct = new QAction(tr("displayformular"), this);
  disformularAct->setPriority(QAction::LowPriority);
	connect(disformularAct, SIGNAL(triggered()), this, SLOT(disformular()));

	buildAct = new QAction(QIcon(":/images/Run.png"), tr("Build"), this);
  buildAct->setPriority(QAction::LowPriority);
	connect(buildAct, SIGNAL(triggered()), this, SLOT(build()));

	markAct = new QAction(QIcon(":/images/mark.png"), tr("Mark"), this);
	markAct->setCheckable(true);
	markAct->setChecked(core->isMarking());
	connect(markAct, SIGNAL(triggered(bool)), this, SLOT(setMarking(bool)));

	markCAct = new QAction(tr("Clear mark"), this);
	connect(markCAct, SIGNAL(triggered()), this, SLOT(clearMark()));
}

XWTeXDocumentCore *XWLaTeXEditorMainWindow::createMdiChild(const QString &filename)
{
	XWLaTeXDocument * doc = new XWLaTeXDocument;
	XWTeXDocumentCore *child = new XWTeXDocumentCore(doc);
	child->loadFile(filename);
	searcher->setDoc(doc);
  mdiArea->addSubWindow(child);

	connect(doc, SIGNAL(sectionChanged()), this, SLOT(updateActions()));

  return child;
}

void XWLaTeXEditorMainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(importAct);
  fileMenu->addAction(openAct);
	fileMenu->addAction(openFolderAct);
	fileMenu->addSeparator();
  fileMenu->addAction(saveAct);
  fileMenu->addSeparator();
  fileMenu->addAction(fmtAct);
  fileMenu->addSeparator();
	fileMenu->addAction(toPicAct);
	fileMenu->addSeparator();
  fileMenu->addAction(toDviAct);
  fileMenu->addAction(toPDFAct);
	fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

	fileToolsBar = addToolBar(tr("File tools"));

	fileToolsBar->addAction(importAct);
	fileToolsBar->addAction(openAct);
	fileToolsBar->addAction(saveAct);

	editToolBar = addToolBar(tr("Edit tools"));
	editToolBar->addAction(textAct);
	editToolBar->addAction(formularAct);
	editToolBar->addAction(disformularAct);

	sectionBar = addToolBar(tr("Sections tools"));
	sectionCombo = new QComboBox(sectionBar);
	sectionCombo->addItem(tr("normal"),LArealparagraph);
	sectionBar->addWidget(sectionCombo);
	connect(sectionCombo, SIGNAL(activated(int)), this, SLOT(sections(int)));

	docToolsBar = addToolBar(tr("Document tools"));
	classCombo = new QComboBox(docToolsBar);
	classCombo->addItem(tr("book"), XW_LTX_DOC_CLS_BOOK);
	docToolsBar->addWidget(classCombo);
	connect(classCombo, SIGNAL(activated(int)), this, SLOT(setClass(int)));
	paperCombo = new QComboBox(docToolsBar);
	docToolsBar->addWidget(paperCombo);
	connect(paperCombo, SIGNAL(activated(int)), this, SLOT(setPaper(int)));
	fontCombo = new QComboBox(docToolsBar);
	QStringList ls = XWTeXDocumentCore::getFontSizes();
	fontCombo->addItems(ls);
	docToolsBar->addWidget(fontCombo);
	connect(fontCombo, SIGNAL(activated(int)), this, SLOT(setFont(int)));
	dirCombo = new QComboBox(docToolsBar);
	dirCombo->addItem(tr("TLT"),TEX_DOC_WD_TLT);
	dirCombo->addItem(tr("RTT"),TEX_DOC_WD_RTT);
	dirCombo->addItem(tr("TRT"),TEX_DOC_WD_TRT);
	dirCombo->addItem(tr("LTL"),TEX_DOC_WD_LTL);
	docToolsBar->addWidget(dirCombo);
	connect(dirCombo, SIGNAL(activated(int)), this, SLOT(setDirection(int)));
	columnCombo = new QComboBox(docToolsBar);
	columnCombo->addItem(tr("onecolumn"),0);
	columnCombo->addItem(tr("twocolumn"),1);
	docToolsBar->addWidget(columnCombo);
	connect(columnCombo, SIGNAL(activated(int)), this, SLOT(setColumn(int)));

  viewToolsBar = addToolBar(tr("View tools"));
	modeCombo = new QComboBox(viewToolsBar);
	modeCombo->addItem(tr("Continuous page"),0);
	modeCombo->addItem(tr("Double page"),1);
	viewToolsBar->addWidget(modeCombo);
	connect(modeCombo, SIGNAL(activated(int)), this, SLOT(setMode(int)));
	zoomCombo = new QComboBox(viewToolsBar);
	ls = XWTeXDocumentCore::getZooms();
	zoomCombo->addItems(ls);
	viewToolsBar->addWidget(zoomCombo);
	connect(zoomCombo, SIGNAL(activated(int)), this, SLOT(zoom(int)));

	markToolsBar = addToolBar(tr("Mark tools"));
	QMenu *colorMenu = new QMenu(markToolsBar);
	core->addMarkingActions(colorMenu);
	colorMenu->addSeparator();
	colorMenu->addAction(markCAct);

	markToolsButton = new QToolButton(markToolsBar);
	markToolsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	markToolsButton->setPopupMode(QToolButton::MenuButtonPopup);
	markToolsButton->setMenu(colorMenu);
	markToolsButton->setDefaultAction(markAct);
	markToolsBar->addWidget(markToolsButton);

	buildToolBar = addToolBar(tr("Build tools"));
  buildToolBar->addAction(buildAct);

	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(findAct);
  editMenu->addAction(replaceAct);
	editMenu->addSeparator();
	QMenu * cmenu = editMenu->addMenu(tr("input code"));
	QList<QAction *> acts = inputCodecActs->actions();
  for (int i = 0; i <  acts.size(); i++)
  {
  	QAction * a = acts[i];
  	cmenu->addAction(a);
  }

	cmenu = editMenu->addMenu(tr("output code"));
  acts = outputCodecActs->actions();
  for (int i = 0; i <  acts.size(); i++)
  {
  	QAction * a = acts[i];
  	cmenu->addAction(a);
  }

	viewMenu = menuBar()->addMenu(tr("&View"));
	toolsbarMenu = viewMenu->addMenu(tr("Tools bar"));
	toolsbarMenu->addAction(fileToolsBar->toggleViewAction());
  toolsbarMenu->addAction(editToolBar->toggleViewAction());
	toolsbarMenu->addAction(sectionBar->toggleViewAction());
	toolsbarMenu->addAction(docToolsBar->toggleViewAction());
	toolsbarMenu->addAction(viewToolsBar->toggleViewAction());
	toolsbarMenu->addAction(markToolsBar->toggleViewAction());
	toolsbarMenu->addAction(buildToolBar->toggleViewAction());
	viewMenu->addSeparator();
	viewMenu->addAction(showPreambleAct);
	viewMenu->addSeparator();
	viewMenu->addAction(coreDock->toggleViewAction());
  viewMenu->addSeparator();
	viewMenu->addAction(folderDock->toggleViewAction());
	viewMenu->addSeparator();
  viewMenu->addAction(termDock->toggleViewAction());

	insertMenu = menuBar()->addMenu(tr("&Insert"));
	cmenu = insertMenu->addMenu(tr("table"));
	cmenu->addAction(tabularAct);
	cmenu->addAction(tabularStarAct);
	insertMenu->addSeparator();
	cmenu = insertMenu->addMenu(tr("pictures"));
	cmenu->addAction(includeGraphicsAct);
	cmenu->addSeparator();
	cmenu->addAction(tikzPictureAct);
	cmenu->addAction(tikzAct);
	cmenu->addSeparator();
	cmenu->addAction(pstricksPictureAct);
	cmenu->addSeparator();
	cmenu->addAction(pictureAct);
  insertMenu->addSeparator();
	cmenu = insertMenu->addMenu(tr("note"));
	cmenu->addAction(footnoteAct);
	cmenu->addAction(marginparAct);
	insertMenu->addSeparator();
	cmenu = insertMenu->addMenu(tr("cross reference"));
	cmenu->addAction(labelAct);
	cmenu->addAction(pagerefAct);
	cmenu->addAction(refAct);
	cmenu = insertMenu->addMenu(tr("bibliography"));
	cmenu->addAction(citeAct);
	cmenu->addAction(nociteAct);
	insertMenu->addSeparator();
	cmenu = insertMenu->addMenu(tr("index and glossary"));
	cmenu->addAction(indexAct);
	cmenu->addAction(glossaryAct);
	insertMenu->addSeparator();
	cmenu = insertMenu->addMenu(tr("items"));
	cmenu->addAction(descriptionAct);
	cmenu->addAction(eenumerateAct);
	cmenu->addAction(itemizeAct);
	cmenu = insertMenu->addMenu(tr("align"));
	cmenu->addAction(centerAct);
	cmenu->addAction(flushLeftAct);
	cmenu->addAction(flushRightAct);
	cmenu = insertMenu->addMenu(tr("quotations"));
	cmenu->addAction(quoteAct);
	cmenu->addAction(quotationAct);
	cmenu->addAction(verseAct);
	cmenu = insertMenu->addMenu(tr("theorems"));
	cmenu->addAction(theoremAct);
	cmenu->addAction(proofAct);
	cmenu->addAction(assumptionAct);
	cmenu->addAction(definitionAct);
	cmenu->addAction(propositionAct);
	cmenu->addAction(lemmaAct);
	cmenu->addAction(axiomAct);
	cmenu->addAction(corollaryAct);
	cmenu->addAction(conjectureAct);
	cmenu->addSeparator();
	cmenu->addAction(exerciseAct);
	cmenu->addAction(exampleAct);
	cmenu->addAction(problemAct);
	cmenu->addAction(remarkAct);
	cmenu = insertMenu->addMenu(tr("math"));
	cmenu->addAction(arrayAct);
	cmenu->addAction(eqnarrayAct);
	cmenu->addAction(eqnarrayStarAct);
	cmenu->addAction(equationAct);
	insertMenu->addSeparator();
	insertMenu->addAction(verbatimAct);
	cmenu = insertMenu->addMenu(tr("file"));
	cmenu->addAction(includeAct);
	cmenu->addAction(inputAct);
	cmenu = insertMenu->addMenu(tr("preamble"));
	cmenu->addAction(usepackageAct);
	cmenu->addAction(useTikzLibraryAct);
	cmenu->addAction(useThemeAct);
	cmenu->addAction(includeLectureAct);
	cmenu->addAction(includeOnlyAct);
	cmenu->addSeparator();
	cmenu->addAction(titleAct);
	cmenu->addAction(authorAct);
	cmenu->addAction(dateAct);
	cmenu->addSeparator();
	cmenu->addAction(nameAct);
	cmenu->addAction(addressAct);
	cmenu->addAction(locationAct);
	cmenu->addAction(telephoneAct);
	cmenu->addAction(thanksAct);
	cmenu->addAction(signatureAct);
	cmenu = insertMenu->addMenu(tr("front"));
	cmenu->addAction(maketitleAct);
	cmenu->addAction(titlePageAct);
	cmenu->addAction(abstractAct);
	cmenu->addAction(tableOfContentsAct);
	cmenu = insertMenu->addMenu(tr("back"));
	cmenu->addAction(appendixAct);
	cmenu->addAction(printIndexAct);
	cmenu->addAction(printGlossaryAct);
	cmenu->addAction(thebibliographyAct);
	cmenu = insertMenu->addMenu(tr("beamer"));
	cmenu->addAction(lectureAct);
	cmenu->addAction(noteAct);
	cmenu->addAction(modeAct);
	cmenu->addAction(againFrameAct);

	toolsMenu = menuBar()->addMenu(tr("&Tools"));
	toolsMenu->addAction(formularEditAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(tikzEditAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(editorSettingAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(texFontSettingAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(fontInfoAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(externalToolsAct);

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(productHelpAct);
	helpMenu->addSeparator();
	helpMenu->addAction(latexHelpAct);
	helpMenu->addAction(pgfHelpAct);
	helpMenu->addAction(pstHelpAct);
	helpMenu->addSeparator();
  helpMenu->addAction(aboutSoftAct);
	helpMenu->addAction(aboutHardAct);
}

XWLaTeXDocument * XWLaTeXEditorMainWindow::currentDoc()
{
	XWTeXDocumentCore * cur = activeMdiChild();
	searcher->setDoc(cur->document());
	return (XWLaTeXDocument*)(cur->document());
}

void XWLaTeXEditorMainWindow::findInputCodecName(const QString & name)
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

QMdiSubWindow * XWLaTeXEditorMainWindow::findMdiChild(const QString &fileName)
{
	foreach (QMdiSubWindow *window, mdiArea->subWindowList())
	{
      XWTeXDocumentCore *mdiChild = qobject_cast<XWTeXDocumentCore *>(window->widget());
      QString fn = mdiChild->getFileName();
      if (fn == fileName)
			{
				searcher->setDoc(mdiChild->document());
				return window;
			}
  }
  return 0;
}

void XWLaTeXEditorMainWindow::findOutputCodecName(const QString & name)
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

QString XWLaTeXEditorMainWindow::getLocalPath()
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

	return path;
}

void XWLaTeXEditorMainWindow::restoreToolsBarStatus()
{
	QSettings * settings = xwApp->getSettings();
	if (settings->contains("ShowFileToolsBar"))
  	fileToolsBar->setVisible(settings->value("ShowFileToolsBar").toBool());

  if (settings->contains("ShowEditToolsBar"))
  	editToolBar->setVisible(settings->value("ShowEditToolsBar").toBool());

	if (settings->contains("ShowSectionToolsBar"))
	 	sectionBar->setVisible(settings->value("ShowSectionToolsBar").toBool());

  if (settings->contains("ShowBuildToolBar"))
  	buildToolBar->setVisible(settings->value("ShowBuildToolBar").toBool());

  if (settings->contains("ShowDocToolsBar"))
  	docToolsBar->setVisible(settings->value("ShowDocToolsBar").toBool());

  if (settings->contains("ShowViewToolsBar"))
  	viewToolsBar->setVisible(settings->value("ShowViewToolsBar").toBool());

  if (settings->contains("ShowMarkToolsBar"))
  	markToolsBar->setVisible(settings->value("ShowMarkToolsBar").toBool());

  settings->endGroup();
}

void XWLaTeXEditorMainWindow::saveToolsBarStatus()
{
	QSettings * settings = xwApp->getSettings();
	settings->setValue("ShowFileToolsBar", fileToolsBar->isVisible());
	settings->setValue("ShowEditToolsBar", editToolBar->isVisible());
	settings->setValue("ShowSectionToolsBar", sectionBar->isVisible());
	settings->setValue("ShowBuildToolBar", buildToolBar->isVisible());
	settings->setValue("ShowDocToolsBar", docToolsBar->isVisible());
	settings->setValue("ShowViewToolsBar", viewToolsBar->isVisible());
	settings->setValue("ShowMarkToolsBar", markToolsBar->isVisible());
	settings->endGroup();
}

void XWLaTeXEditorMainWindow::setBackground()
{
	if (xwApp->hasSetting("MainWinOpacity"))
	{
		double op = xwApp->getSetting("MainWinOpacity").toDouble();
		if (op >= 0.0 && op <= 1.0)
			setWindowOpacity(op);
	}
}
