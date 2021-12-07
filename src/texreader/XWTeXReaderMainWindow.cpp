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
#include <QSettings>
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
#include "XWOutlineWindow.h"
#include "XWBaseNameWindow.h"
#include "XWDocFontsWindow.h"
#include "XWFontDefWindow.h"
#include "XWTexFormatWindow.h"
#include "XWTeXReaderCore.h"
#include "XWTeXReaderDev.h"
#include "XWTeXConsole.h"
#include "XWSearcher.h"
#include "XWDocSearchWidget.h"
#include "XWTeXReaderMainWindow.h"

XWTeXReaderMainWindow::XWTeXReaderMainWindow()
{
	searcher = new XWPDFSearcher(this);
	QString title = xwApp->getProductName();
	setWindowTitle(title);
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
	dev = new XWTeXReaderDev(this);
	connect(dev, SIGNAL(messageAdded(const QString &)), this, SLOT(insertTermText(const QString &)));
	connect(dev, SIGNAL(finished()), this, SLOT(updateShow()));
	connect(dev, SIGNAL(needFile(const QString &)), this, SLOT(openFile(const QString &)));
	connect(this, SIGNAL(fileDownloaded(const QString &)), dev, SLOT(setInputText(const QString &)));
	
	playing = false;
	setBackground();
    
    core = new XWTeXReaderCore(this, this);
    setCentralWidget(core);
    
    dock = new QDockWidget(this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		addDockWidget(Qt::LeftDockWidgetArea, dock);
    dock->toggleViewAction()->setChecked(false);

		QTabWidget * tab = new QTabWidget(dock);
		dock->setWidget(tab);

    outlineWin = new XWOutlineWindow(core, tab);
    tab->addTab(outlineWin,tr("Outline"));
    XWTeXPDFSearchWidget * sw = new XWTeXPDFSearchWidget(searcher,tab);
		tab->addTab(outlineWin,tr("Search"));
		connect(sw, SIGNAL(positionActivated(int, double,double,double,double)), 
		        this, SLOT(showSearchResult(int, double,double,double,double)));
    
    termDock = new QDockWidget(tr("Term out"), this);
  termDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
  	
  termWin = new XWTeXConsole(this);
  termDock->setWidget(termWin);
  addDockWidget(Qt::BottomDockWidgetArea, termDock);
  termDock->toggleViewAction()->setChecked(false);
  connect(dev, SIGNAL(requestInput()), termWin, SLOT(requstInput()));
  connect(termWin, SIGNAL(inputFinished(const QString &)), dev, SLOT(setInputText(const QString &)));
      
    fileLabel = new QLabel(tr("File: "), this);
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
    	
    createFileAction();
    createEditAction();
    createViewAction();
    createToolsAction();
    toolsbarMenu->addAction(markToolsBar->toggleViewAction());
    createHelpAction();
        	
    dock->toggleViewAction()->setChecked(false);
    setToolsBarsVisible(true);
    updateActions();
    connect(core, SIGNAL(fileLoaded()), this, SLOT(updateActions()));
   	connect(core, SIGNAL(currentChanged(int, int)), this, SLOT(setPageStatus(int, int)));
   	connect(core, SIGNAL(fullScreenChanged(bool)), this, SLOT(setFullScreen(bool)));
   	connect(core, SIGNAL(authorChanged(const QString &)), this, SLOT(setAuthor(const QString &)));
  connect(core, SIGNAL(creationDateChanged(const QString &)), this, SLOT(setCreationDate(const QString &)));
  connect(core, SIGNAL(keywordsChanged(const QString &)), this, SLOT(setKeywords(const QString &)));
  connect(core, SIGNAL(lastModifiedDateChanged(const QString &)), this, SLOT(setLastModifiedDate(const QString &)));
  connect(core, SIGNAL(subjectChanged(const QString &)), this, SLOT(setSubject(const QString &)));
  connect(core, SIGNAL(titleChanged(const QString &)), this, SLOT(setTitle(const QString &)));
  connect(core, SIGNAL(zoomChanged(int)), this, SLOT(setZoomIndex(int)));
  connect(core, SIGNAL(rotateChanged(int)), this, SLOT(setRotate(int)));
}

XWTeXReaderMainWindow::~XWTeXReaderMainWindow()
{
}

void XWTeXReaderMainWindow::addToContextMenu(QMenu * menu)
{
	if (copyAct->isEnabled())
	{
		menu->addAction(copyAct);	
		menu->addSeparator();
	}
	
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
	menu->addAction(fullScreenAct);
	if (!isFullScreen())
	{
		menu->addSeparator();
		menu->addAction(fitPageAct);
		menu->addAction(fitWidthAct);
		menu->addAction(zoomInAct);
		menu->addAction(zoomOutAct);
		menu->addSeparator();
		menu->addAction(readingAct);	
	}
}

void XWTeXReaderMainWindow::displayDest(const char *namedDest)
{
	XWString dest(namedDest);
	core->displayDest(&dest);
}

void XWTeXReaderMainWindow::displayPage(int pg)
{
	core->displayPage(pg);
}

bool XWTeXReaderMainWindow::isReading()
{
	return readingAct->isChecked();
}

bool XWTeXReaderMainWindow::loadFile(const QString & filename)
{
	termWin->clear();
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
	
	dev->setInteractionOption(findInteract());
	dev->setFileLineErrorStyle(fleAct->isChecked());
	dev->setHaltOnError(haltAct->isChecked());
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
	if (first.length() > 1 && first[0] == QChar('%') && first[1] == QChar('&'))
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
	
	if (!hasfmt && !olddump.isEmpty())
		hasfmt = loadFmt(olddump);
	
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
	
	termWin->clear();
	topPage = 0;	
	core->setDoc(0);
	if (!dev->loadFile(fullname))
	{
		xwApp->showErrs();
		return false;
	}
	
	saveRecent();
	localFileName = fullname;	
	
	QString fla = QString(tr("File: %1")).arg(fullname);
	fileLabel->setText(fla);	
	dev->compile();
	return true;
}

bool XWTeXReaderMainWindow::loadFmt(const QString & fmt)
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

bool XWTeXReaderMainWindow::loadURL(const QUrl & url, const QString & localname)
{
	if (localname.isEmpty())
	{
		tmpFileName = xwApp->getFile(url);
		if (tmpFileName.isEmpty())
		{
			xwApp->showErrs();
			return false;
		}
			
		fileName = url.toString(QUrl::RemoveUserInfo);		
		if (!loadFile(tmpFileName))
		{
			localFileName.clear();
			return false;
		}
		
		localFileName.clear();
	}
	else
	{
		tmpFileName = localname;
		if (!xwApp->getFile(url, localname))
		{
			xwApp->showErrs();
			return false;
		}
			
		fileName = url.toString(QUrl::RemoveUserInfo);
			
		if (!loadFile(localname))
			return false;
	}
	
	QString fla = QString(tr("File: %1")).arg(fileName);
	fileLabel->setText(fla);
	return true;
}

void XWTeXReaderMainWindow::insertTermText(const QString & txt)
{
	termWin->insertPlainText(txt);
	termWin->ensureCursorVisible();
}

void XWTeXReaderMainWindow::gotoPage(int idx)
{
	int pg = idx + 1;
	if (pg != core->getPageNum())
		displayPage(pg);
}

void XWTeXReaderMainWindow::loadFmt()
{
	XWTexFormatWindow dlg(this);
	if (dlg.exec() != QDialog::Accepted)
		return ;
			
	QString fmt = dlg.getFmt();
	if (fmt.isEmpty())
		return ;
		
	loadFmt(fmt);
}

void XWTeXReaderMainWindow::open()
{
	QString path = getLocalPath();	
	QString filename = QFileDialog::getOpenFileName(this, 
		                                            tr("Open file"), 
		                                            path, 
		                                            tr("Files (*.tex)"));
	if (filename.isEmpty())
		return ;
		
	loadFile(filename);
}

void XWTeXReaderMainWindow::openFile(const QString & filename)
{
	QString ret = xwApp->getFile(filename);
	emit fileDownloaded(ret);
}

void XWTeXReaderMainWindow::openNet()
{
	XWFTPWindow d(this);
	if (d.exec() == QDialog::Accepted)
	{
		QUrl url = d.getURL();		
		QString local;
		if (d.saveToLocal())
			local = d.getLocalName();
			
		if (!url.isValid())
			return ;
			
		loadURL(url, local);
	}
}

void XWTeXReaderMainWindow::openRecent()
{
	XWRecentFileWindow d(this);
	if (d.exec() == QDialog::Accepted)
	{
		int pg = 1;
		QString filename = d.getFile(&pg);
		XWFileName fn(filename);
		if (fn.isLocal())
			loadFile(filename);
		else
		{
			QUrl url(filename);
			loadURL(url, QString());
		}
	}
}

void XWTeXReaderMainWindow::openURL()
{
	XWURLWindow d(this);
	if (d.exec() == QDialog::Accepted)
	{
		QString tmp = d.getURL();
		QString local;
		if (d.saveToLocal())
			local = d.getLocalName();
			
		QUrl url(tmp);
		if (!url.isValid())
			return ;
			
		loadURL(url, local);
	}
}

void XWTeXReaderMainWindow::play()
{
	int min = intervalBox->value();
	int idx = animateCombo->currentIndex();	
	setFullScreen(true);
	playing = true;		
	core->play(min, idx);
}

void XWTeXReaderMainWindow::print()
{
	QPrinter printer(QPrinter::HighResolution);
	QPrintDialog dlg(&printer, this);
	QString tmp = windowTitle();
	QString title = QString(tr("Print %1")).arg(tmp);
    dlg.setWindowTitle(title);
    
    if (dlg.exec() != QDialog::Accepted)
        return;
        
    core->print(&printer);
    xwApp->showWarns();
		xwApp->showErrs();
}

void XWTeXReaderMainWindow::reading(bool checked)
{
	menuBar()->setVisible(!checked);
	statusBar()->setVisible(!checked);
	
	if (checked)
	{
		readingAct->setIcon(QIcon(":/images/noreading.png"));
		setToolsBarsVisible(false);
	}
	else
	{
		readingAct->setIcon(QIcon(":/images/reading.png"));
		setToolsBarsVisible(true);
	}
}

void XWTeXReaderMainWindow::refresh()
{
	termWin->clear();
	topPage = core->getPageNum();	
	core->setDoc(0);
	dev->compile();
}

void XWTeXReaderMainWindow::rotate()
{
	int r = rotateBox->value();
	r += 90;
	if (r == 360)
		r = 0;
		
	rotateBox->setValue(r);
		
	if (r != core->getRotate())
		core->setRotate(r);

	searcher->setDPI(core->getDPI());
	searcher->setRotate(core->getRotate());
}

void XWTeXReaderMainWindow::saveToDvi()
{
	XWDocSea sea;
	QStringList list = sea.getConfigDir(XWDocSea::DVI);
	QString path = list[0];
	QString basename;
	if (!localFileName.isEmpty())
	{
		XWFileName fn(localFileName);
		basename = fn.baseName();	
	}
	else if (!fileName.isEmpty())
	{
		XWFileName fn(fileName);
		basename = fn.baseName();	
	}
	else
	{
		XWFileName fn(tmpFileName);
		basename = fn.baseName();	
	}
	path += QString("/%1.dvi").arg(basename);
	
	QString files = tr("Files (*.dvi)");
	
	QString filename = QFileDialog::getSaveFileName(this, tr("Save as"), path, files);
	if (filename.isEmpty())
		return ;
		
	core->getDoc()->saveAs(filename);
	xwApp->showWarns();
	xwApp->showErrs();
}

void XWTeXReaderMainWindow::saveToImage()
{
	QString basename;	
	QString fullname;
	if (localFileName.isEmpty())
	{
		fullname = fileName;
		QUrl url(fileName);
		basename = url.path();
	}
	else
	{
		fullname = localFileName;
		basename = localFileName;
	}
		
	XWFileName fn(basename);
	basename = fn.baseName();	
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

void XWTeXReaderMainWindow::saveToPDF()
{
	QString basename;	
	QString fullname;
	if (localFileName.isEmpty())
	{
		fullname = fileName;
		QUrl url(fileName);
		basename = url.path();
	}
	else
	{
		fullname = localFileName;
		basename = localFileName;
	}
		
	XWFileName fn(basename);
	basename = fn.baseName();	
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

void XWTeXReaderMainWindow::saveToPS()
{
	QString basename;	
	QString fullname;
	if (localFileName.isEmpty())
	{
		fullname = fileName;
		QUrl url(fileName);
		basename = url.path();
	}
	else
	{
		fullname = localFileName;
		basename = localFileName;
	}
		
	XWFileName fn(basename);
	basename = fn.baseName();	
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

void XWTeXReaderMainWindow::saveToText()
{
	QString basename;	
	QString fullname;
	if (localFileName.isEmpty())
	{
		fullname = fileName;
		QUrl url(fileName);
		basename = url.path();
	}
	else
	{
		fullname = localFileName;
		basename = localFileName;
	}
		
	XWFileName fn(basename);
	basename = fn.baseName();	
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

void XWTeXReaderMainWindow::setContinuousMode(bool e)
{
	core->setContinuousMode(e);
	searcher->setDPI(core->getDPI());
	searcher->setRotate(core->getRotate());
}

void XWTeXReaderMainWindow::setDisplay()
{
	XWDisplaySettingDialog d(this);
	if (d.exec() != QDialog::Accepted)
		return ;
		
	setBackground();
	core->setDisplay();
	searcher->setDPI(core->getDPI());
	searcher->setRotate(core->getRotate());
}

void XWTeXReaderMainWindow::setDoubleMode(bool e)
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
	searcher->setDPI(core->getDPI());
	searcher->setRotate(core->getRotate());
}

void XWTeXReaderMainWindow::setFonts()
{
	XWFontWindow d(this);
	d.exec();
}

void XWTeXReaderMainWindow::setFullScreen(bool e)
{
	menuBar()->setVisible(!e);
	statusBar()->setVisible(!e);
	if (e)
	{
		if (!fullScreenAct->isChecked())
			fullScreenAct->setChecked(true);
		setToolsBarsVisible(false);
		continuousPagesAct->setChecked(false);
		readingAct->setChecked(false);
		horiAct->setChecked(false);
		core->setFullScreen(e);
		zoomCombo->setCurrentIndex(8);
		setBackgroundRole(QPalette::WindowText);
		showFullScreen();
	}
	else
	{
		if (fullScreenAct->isChecked())
			fullScreenAct->setChecked(false);
		setBackgroundRole(QPalette::Window);
		showMaximized();		
		setToolsBarsVisible(true);
		if (playing)
		{
			core->stop();
		}
		else
			core->setFullScreen(e);
			
		playing = false;
	}
}

void XWTeXReaderMainWindow::setHoriMode(bool e)
{
	if (e)
		continuousPagesAct->setChecked(true);
	core->setHoriMode(e);
	searcher->setDPI(core->getDPI());
	searcher->setRotate(core->getRotate());
}

void XWTeXReaderMainWindow::setPageStatus(int cur, int num)
{
	QString p = QString(tr("Current page: %1/%2")).arg(cur).arg(num);
	pagesLabel->setText(p);
}

void XWTeXReaderMainWindow::setRotate(int r)
{
	rotateBox->setValue(r);
}

void XWTeXReaderMainWindow::setTexFonts()
{
	XWTexFontWindow win(this);
	win.exec();
}

void XWTeXReaderMainWindow::setTools()
{
	XWExternalToolWindow d(this);
	d.exec();
}

void XWTeXReaderMainWindow::setZoomIndex(int idx)
{
	zoomCombo->setCurrentIndex(idx);
}

void XWTeXReaderMainWindow::showAboutHard()
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

void XWTeXReaderMainWindow::showAboutSoft()
{
	QString product = xwApp->getProductName();
	QString version = xwApp->getVersion();
	QString text = QString(tr("<h3>%1 %2</h3>"
	                         "<p>%3 is a TeX document(not format) previewer.</p>")).arg(product).arg(version).arg(product);
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

void XWTeXReaderMainWindow::showFontDef()
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

void XWTeXReaderMainWindow::showFonts()
{
	XWFontInfoWindow win(this);
	win.exec();
}

void XWTeXReaderMainWindow::showProductHelp()
{
	xwApp->getHelp("xwtex");
}

void XWTeXReaderMainWindow::stop()
{
	setFullScreen(false);
	playing = false;
}

void XWTeXReaderMainWindow::updateShow()
{
	if (!dev->isOk())
		return ;
			
	QIODevice * dvifile = dev->takeDviFile();
	dvifile->open(QIODevice::ReadOnly);
	XWDVIDoc * dvidoc = new XWDVIDoc;
	if (!dvidoc->load(dvifile, 0))
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

	searcher->setDoc(dvidoc);
	searcher->setDPI(core->getDPI());
	searcher->setRotate(core->getRotate());
}

void XWTeXReaderMainWindow::zoom(int idx)
{
	core->zoom(idx);
	searcher->setDPI(core->getDPI());
	searcher->setRotate(core->getRotate());
}

void XWTeXReaderMainWindow::closeEvent(QCloseEvent *event)
{
	saveRecent();
	saveToolsBarStatus();
	xwApp->cleanup();
	QMainWindow::closeEvent(event);
}

void XWTeXReaderMainWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape || 
		  e->key() == Qt::Key_Backspace)
	{
		if (isFullScreen())
			setFullScreen(false);
			
		e->accept();
		return ;
	}
	
	QMainWindow::keyPressEvent(e);
}

void XWTeXReaderMainWindow::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
	{
		if (isFullScreen())
			core->gotoNextPage();
	}
}

QIcon XWTeXReaderMainWindow::createColorIcon(QColor color)
{
	QPixmap pixmap(20, 20);
	QPainter painter(&pixmap);
	painter.setPen(Qt::NoPen);
	painter.fillRect(QRect(0, 0, 20, 20), color);
	return QIcon(pixmap);
}

void XWTeXReaderMainWindow::createEditAction()
{
	historyToolsBar = addToolBar(tr("History tools"));
  gotoToolsBar = addToolBar(tr("Goto tools"));
    
  editMenu = new QMenu(tr("&Edit"), this);
	menuBar()->addMenu(editMenu);
	
	backAct = new QAction(QIcon(":/images/back.png"), tr("Back"), this);
	backAct->setShortcuts(QKeySequence::Back);
	connect(backAct, SIGNAL(triggered()), core, SLOT(goBackward()));
	connect(core, SIGNAL(backwardAvailable(bool)), backAct, SLOT(setEnabled(bool)));
	backAct->setEnabled(false);
	
	forwardAct = new QAction(QIcon(":/images/forward.png"), tr("Forward"), this);
	forwardAct->setShortcuts(QKeySequence::Forward);
	connect(forwardAct, SIGNAL(triggered()), core, SLOT(goForward()));
	connect(core, SIGNAL(forwardAvailable(bool)), forwardAct, SLOT(setEnabled(bool)));
	forwardAct->setEnabled(false);
	
	copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
	copyAct->setPriority(QAction::LowPriority);
	copyAct->setShortcut(QKeySequence::Copy);
	connect(copyAct, SIGNAL(triggered()), core, SLOT(copy()));
	connect(core, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));
	copyAct->setEnabled(false);
	
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
	
	findAct = new QAction(QIcon(":/images/find.png"), tr("Find"), this);
	findAct->setPriority(QAction::LowPriority);
	findAct->setShortcut(QKeySequence::Find);
	connect(findAct, SIGNAL(triggered()), core, SLOT(find()));
	
	findNextAct = new QAction(tr("Find next"), this);
	findNextAct->setPriority(QAction::LowPriority);
	findNextAct->setShortcut(QKeySequence::FindNext);
	connect(findNextAct, SIGNAL(triggered()), core, SLOT(findNext()));
	
	fleAct = new QAction(tr("File line error style"), this);
  fleAct->setCheckable(true);
  fleAct->setChecked(true);
  
  haltAct = new QAction(tr("Halt on error"), this);
  haltAct->setCheckable(true);
  haltAct->setChecked(false);
  
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
	
	historyToolsBar->addAction(backAct);
	historyToolsBar->addAction(forwardAct);
	
	gotoToolsBar->addAction(gotoNextAct);
	gotoToolsBar->addAction(gotoPrevAct);
	gotoToolsBar->addAction(gotoFirstAct);
	gotoToolsBar->addAction(gotoLastAct);
	gotoPageCombo = new QComboBox(gotoToolsBar);
	gotoPageCombo->setToolTip(tr("Goto page"));
	gotoToolsBar->addWidget(gotoPageCombo);
	connect(gotoPageCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(gotoPage(int)));
	gotoToolsBar->addAction(findAct);
	
	editMenu->addAction(copyAct);
	editMenu->addSeparator();
	editMenu->addAction(findAct);
	editMenu->addAction(findNextAct);
	editMenu->addSeparator();
	editMenu->addAction(fleAct);
  editMenu->addAction(haltAct);
  QMenu * cmenu = editMenu->addMenu(tr("Interaction mode"));
  QList<QAction *> acts = insertActs->actions();
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
}

void XWTeXReaderMainWindow::createFileAction()
{
	fileToolsBar = addToolBar(tr("File tools"));    
  saveToolsBar = addToolBar(tr("Save tools"));
    
  fileMenu = new QMenu(tr("&File"), this);
	menuBar()->addMenu(fileMenu);
	
	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open a disk file"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
	
	openURLAct = new QAction(QIcon(":/images/openurl.png"), tr("Open URL"), this);
	openURLAct->setStatusTip(tr("Open a file on the World Wide Web"));
	connect(openURLAct, SIGNAL(triggered()), this, SLOT(openURL()));
	
	recentFileAct = new QAction(QIcon(":/images/openrecent.png"), tr("Recent file"), this);
	recentFileAct->setStatusTip(tr("Open a file from recent files list"));
	connect(recentFileAct, SIGNAL(triggered()), this, SLOT(openRecent()));
	
	openNetAct = new QAction(QIcon(":/images/openrecent.png"), tr("Open network file"), this);
	openNetAct->setStatusTip(tr("Open a file on network with user name and passward"));
	connect(openNetAct, SIGNAL(triggered()), this, SLOT(openNet()));
	
	fmtAct = new QAction(tr("Load Format"), this);
	fmtAct->setPriority(QAction::LowPriority);
	connect(fmtAct, SIGNAL(triggered()), this, SLOT(loadFmt()));
	
	toDviAct = new QAction(tr("Save to dvi"), this);
	toDviAct->setStatusTip(tr("Save as dvi file"));
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
	
	printAct = new QAction(QIcon(":/images/printer.png"), tr("&Print"), this);
	printAct->setPriority(QAction::LowPriority);
	printAct->setShortcut(QKeySequence::Print);
	connect(printAct, SIGNAL(triggered()), this, SLOT(print()));
	
	exitAct = new QAction(tr("&Quit"), this);
	exitAct->setShortcut(Qt::CTRL + Qt::Key_Q);
	connect(exitAct, SIGNAL(triggered()), xwApp, SLOT(quit()));
	
	refreshAct = new QAction(QIcon(":/images/refresh.png"), tr("Refresh"), this);
  refreshAct->setPriority(QAction::LowPriority);
	refreshAct->setShortcut(QKeySequence::Refresh);
	connect(refreshAct, SIGNAL(triggered()), this, SLOT(refresh()));
	
	fileToolsBar->addAction(openAct);	
	fileToolsBar->addAction(recentFileAct);
	fileToolsBar->addAction(printAct);
	fileToolsBar->addAction(refreshAct);
	
	saveToolsBar->addAction(toPDFAct);
	saveToolsBar->addAction(toPSAct);
	saveToolsBar->addAction(toTextAct);
	saveToolsBar->addAction(toImageAct);
	
	fileMenu->addAction(openAct);
	fileMenu->addAction(recentFileAct);
	fileMenu->addAction(openURLAct);
	fileMenu->addAction(openNetAct);
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
	
	fileMenu->addAction(printAct);
	fileMenu->addSeparator();
	
	fileMenu->addAction(exitAct);
}

void XWTeXReaderMainWindow::createHelpAction()
{
	helpMenu = new QMenu(tr("&Help"), this);
	menuBar()->addMenu(helpMenu);
		
	QString p = xwApp->getProductName();
	productHelpAct = new QAction(p, this);
	connect(productHelpAct, SIGNAL(triggered()), this, SLOT(showProductHelp()));
	
	aboutSoftAct = new QAction(tr("About software"), this);
	connect(aboutSoftAct, SIGNAL(triggered()), this, SLOT(showAboutSoft()));
	
	aboutHardAct = new QAction(tr("About hardware"), this);
	connect(aboutHardAct, SIGNAL(triggered()), this, SLOT(showAboutHard()));
	
	helpMenu->addAction(productHelpAct);
	helpMenu->addSeparator();
	helpMenu->addAction(aboutSoftAct);
	helpMenu->addAction(aboutHardAct);
}

void XWTeXReaderMainWindow::createToolsAction()
{
	markToolsBar = addToolBar(tr("Mark tool"));
    
	toolsMenu = new QMenu(tr("&Tools"), this);
	menuBar()->addMenu(toolsMenu);
	
	markAct = new QAction(QIcon(":/images/mark.png"), tr("Mark"), this);
	markAct->setCheckable(true);
	markAct->setChecked(core->isMarking());
	connect(markAct, SIGNAL(triggered(bool)), core, SLOT(setMarking(bool)));
	
	markCAct = new QAction(tr("Clear mark"), this);
	connect(markCAct, SIGNAL(triggered()), core, SLOT(clearMark()));
	
	QMenu *colorMenu = new QMenu(markToolsBar);
	core->addMarkingActions(colorMenu);
	
	markToolsButton = new QToolButton(markToolsBar);
	markToolsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	markToolsButton->setPopupMode(QToolButton::MenuButtonPopup);
	markToolsButton->setMenu(colorMenu);
	markToolsButton->setDefaultAction(markAct);
	markToolsBar->addWidget(markToolsButton);
	
	displaySettingAct = new QAction(tr("Display setting"), this);
	connect(displaySettingAct, SIGNAL(triggered()), this, SLOT(setDisplay()));
	
	externalToolsAct = new QAction(tr("External tools"), this);
	connect(externalToolsAct, SIGNAL(triggered()), this, SLOT(setTools()));
	
	fontSettingAct = new QAction(tr("Display fonts setting"), this);
	connect(fontSettingAct, SIGNAL(triggered()), this, SLOT(setFonts()));
	
	texFontSettingAct = new QAction(tr("TeX fonts setting"), this);
	connect(texFontSettingAct, SIGNAL(triggered()), this, SLOT(setTexFonts()));
	
	fontInfoAct = new QAction(tr("Font information"), this);
	connect(fontInfoAct, SIGNAL(triggered()), this, SLOT(showFonts()));
	
	toolsMenu->addAction(markAct);
	toolsMenu->addAction(markCAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(displaySettingAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(externalToolsAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(fontSettingAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(texFontSettingAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(fontInfoAct);
	
	toolsbarMenu->addSeparator();
    toolsbarMenu->addAction(markToolsBar->toggleViewAction());
}

void XWTeXReaderMainWindow::createViewAction()
{
	zoomToolsBar = addToolBar(tr("Zoom tools"));    
  fitToolsBar = addToolBar(tr("Fit tools"));    
  rotateToolsBar = addToolBar(tr("Rotate tools"));    
  modeToolsBar = addToolBar(tr("Page mode"));    
  screenToolsBar = addToolBar(tr("Screen tools"));
  playToolsBar = addToolBar(tr("Play tools"));
        
  viewMenu = new QMenu(tr("&View"), this);
	menuBar()->addMenu(viewMenu);
    
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
	
	rotateAct = new QAction(QIcon(":/images/rotate.png"), tr("Rotate"), this);
	connect(rotateAct, SIGNAL(triggered()), this, SLOT(rotate()));
	
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
		
	readingAct = new QAction(QIcon(":/images/reading.png"), tr("Reading mode"), this);
	readingAct->setCheckable(true);
	readingAct->setChecked(false);
	connect(readingAct, SIGNAL(triggered(bool)), this, SLOT(reading(bool)));
	
	fullScreenAct = new QAction(QIcon(":/images/fullscreen.png"), tr("Full screen"), this);
	fullScreenAct->setCheckable(true);
	fullScreenAct->setChecked(false);
	connect(fullScreenAct, SIGNAL(triggered(bool)), this, SLOT(setFullScreen(bool)));
	
	playAct = new QAction(QIcon(":/images/play.png"), tr("Play"), this);
	connect(playAct, SIGNAL(triggered()), this, SLOT(play()));
		
	zoomToolsBar->addAction(zoomInAct);
	zoomToolsBar->addAction(zoomOutAct);
		
    zoomCombo = new QComboBox(zoomToolsBar);
    zoomToolsBar->addWidget(zoomCombo);
    QStringList zoomlist = core->getZooms();
    zoomCombo->addItems(zoomlist);
    zoomCombo->setCurrentIndex(core->getZoomIdx());
    connect(zoomCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(zoom(int)));
    
    fitToolsBar->addAction(fitPageAct);
    fitToolsBar->addAction(fitWidthAct);
    
    rotateToolsBar->addAction(rotateAct);
    rotateBox = new QSpinBox(rotateToolsBar);
    rotateToolsBar->addWidget(rotateBox);
    rotateBox->setRange(0, 270);
    rotateBox->setSingleStep(90);
    rotateBox->setReadOnly(true);
    rotateBox->setValue(core->getRotate());
    
    modeToolsBar->addAction(singlePageAct);
    modeToolsBar->addAction(continuousPagesAct);
    
    screenToolsBar->addAction(readingAct);
    screenToolsBar->addAction(fullScreenAct);
    
    playToolsBar->addAction(playAct);
    intervalBox = new QSpinBox(playToolsBar);
    intervalBox->setRange(1, 60);
    intervalBox->setValue(5);
    playToolsBar->addWidget(intervalBox);
    
    animateCombo = new QComboBox(playToolsBar);
    QStringList slidelist = core->getSlides();
    animateCombo->addItems(slidelist);
    playToolsBar->addWidget(animateCombo);
        
    toolsbarMenu = viewMenu->addMenu(tr("Tools bar"));
    toolsbarMenu->addAction(fileToolsBar->toggleViewAction());
    toolsbarMenu->addAction(saveToolsBar->toggleViewAction());
    toolsbarMenu->addSeparator();
    toolsbarMenu->addAction(historyToolsBar->toggleViewAction());
    toolsbarMenu->addAction(gotoToolsBar->toggleViewAction());
    toolsbarMenu->addSeparator();
    toolsbarMenu->addAction(zoomToolsBar->toggleViewAction());
    toolsbarMenu->addAction(fitToolsBar->toggleViewAction());
    toolsbarMenu->addSeparator();
    toolsbarMenu->addAction(rotateToolsBar->toggleViewAction());
    toolsbarMenu->addSeparator();
    toolsbarMenu->addAction(modeToolsBar->toggleViewAction());
    toolsbarMenu->addAction(screenToolsBar->toggleViewAction());
    toolsbarMenu->addAction(playToolsBar->toggleViewAction());
    
    viewMenu->addSeparator();
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(fitPageAct);
    viewMenu->addAction(fitWidthAct);
    viewMenu->addSeparator();
    viewMenu->addAction(rtlAct);
    viewMenu->addAction(horiAct);
    viewMenu->addSeparator();
    viewMenu->addAction(dock->toggleViewAction());
    viewMenu->addAction(termDock->toggleViewAction());
    viewMenu->addSeparator();
    viewMenu->addAction(readingAct);
    viewMenu->addAction(fullScreenAct);
}

qint32  XWTeXReaderMainWindow::findInteract()
{
	qint32 ret = 3;
	QAction * a = insertActs->checkedAction();
	if (a)
		ret = (qint32)(a->data().toInt());
		
	return ret;
}

QString XWTeXReaderMainWindow::getLocalPath()
{
	QString path;
	if (localFileName.isEmpty())
	{
		path = QCoreApplication::applicationDirPath();
		int idx = path.lastIndexOf(QChar('/'));
		if (idx > 0)
			path = path.left(idx + 1);
					
		if (path[path.length() - 1] != QChar('/'))
			path += QChar('/');
			
		path += "doc";					
		QDir d(path);
		if (!d.exists())
			d.mkdir(path);
	}
	else
	{
		XWFileName fn(localFileName);
		path = fn.dirPart();
	}
	
	return path;
}

void XWTeXReaderMainWindow::restoreToolsBarStatus()
{
	QSettings * settings = xwApp->getSettings();
	if (settings->contains("ShowFileToolsBar"))
  	fileToolsBar->setVisible(settings->value("ShowFileToolsBar").toBool());
   
  if (settings->contains("ShowSaveToolsBar"))
  	saveToolsBar->setVisible(settings->value("ShowSaveToolsBar").toBool());

  if (settings->contains("ShowHistoryToolsBar"))
  	historyToolsBar->setVisible(settings->value("ShowHistoryToolsBar").toBool());
    	
  if (settings->contains("ShowGotoToolsBar"))
  	gotoToolsBar->setVisible(settings->value("ShowGotoToolsBar").toBool());
    	
  if (settings->contains("ShowZoomToolsBar"))
  	zoomToolsBar->setVisible(settings->value("ShowZoomToolsBar").toBool());
    	
  if (settings->contains("ShowFitToolsBar"))
  	fitToolsBar->setVisible(settings->value("ShowFitToolsBar").toBool());
    	
  if (settings->contains("ShowRotateToolsBar"))
  	rotateToolsBar->setVisible(settings->value("ShowRotateToolsBar").toBool());
  
  if (settings->contains("ShowModeToolsBar"))
  	modeToolsBar->setVisible(settings->value("ShowModeToolsBar").toBool());
    	
  if (xwApp->hasSetting("ShowScreenToolsBar"))
  	screenToolsBar->setVisible(settings->value("ShowScreenToolsBar").toBool());
    	
  if (xwApp->hasSetting("ShowPlayToolsBar"))
  	playToolsBar->setVisible(settings->value("ShowPlayToolsBar").toBool());
    	
  if (xwApp->hasSetting("ShowMarkToolsBar"))
  	markToolsBar->setVisible(settings->value("ShowMarkToolsBar").toBool());
  	
  settings->endGroup();
}

void XWTeXReaderMainWindow::saveRecent()
{
	if (fileName.isEmpty() && localFileName.isEmpty())
		return ;
	
	QString tmp = fileName;
	if (!localFileName.isEmpty())
	{
		XWFileName fn(localFileName);
		tmp = fn.fileName();
	}
			
	QStringList files = xwApp->getSetting("RecentFile").toStringList();		
	int i = 0;
	for (; i < files.size(); i++)
	{
		QString file = files.at(i);
		if (file.startsWith(tmp))
			break;
	}
	
	if (files.size() > 0 && i < files.size())
		files.removeAt(i);
	else if (i > 50)
		files.removeLast();
		
	int pg = core->getPageNum();
	tmp += QString(" %1").arg(pg);
	files.insert(0, tmp);
	QSettings * settings = xwApp->getSettings();
	settings->setValue("RecentFile", files);
	settings->endGroup();
}

void XWTeXReaderMainWindow::saveToolsBarStatus()
{
	QSettings * settings = xwApp->getSettings();
	settings->setValue("ShowFileToolsBar", fileToolsBar->isVisible());
	settings->setValue("ShowSaveToolsBar", saveToolsBar->isVisible());
	settings->setValue("ShowHistoryToolsBar", historyToolsBar->isVisible());
	settings->setValue("ShowGotoToolsBar", gotoToolsBar->isVisible());
	settings->setValue("ShowZoomToolsBar", zoomToolsBar->isVisible());
	settings->setValue("ShowFitToolsBar", fitToolsBar->isVisible());
	settings->setValue("ShowRotateToolsBar", rotateToolsBar->isVisible());
	settings->setValue("ShowModeToolsBar", modeToolsBar->isVisible());
	settings->setValue("ShowScreenToolsBar", screenToolsBar->isVisible());
	settings->setValue("ShowPlayToolsBar", playToolsBar->isVisible());
	settings->setValue("ShowMarkToolsBar", markToolsBar->isVisible());
	settings->endGroup();
}

void XWTeXReaderMainWindow::setBackground()
{
	if (xwApp->hasSetting("MainWinOpacity"))
	{
		double op = xwApp->getSetting("MainWinOpacity").toDouble();
		if (op >= 0.0 && op <= 1.0)
			setWindowOpacity(op);
	}
}

void XWTeXReaderMainWindow::setAuthor(const QString & a)
{
	QString author = QString(tr("Author: %1")).arg(a);
	authorLabel->setText(author);
}

void XWTeXReaderMainWindow::setCreationDate(const QString & d)
{
	QString date = QString(tr("Creation date: %1")).arg(d);
	creationDateLabel->setText(date);
}

void XWTeXReaderMainWindow::setKeywords(const QString & kws)
{
	QString tip = fileLabel->toolTip();
	tip += QString(tr("Keywords: %1\n")).arg(kws);
	fileLabel->setToolTip(tip);
}

void XWTeXReaderMainWindow::setLastModifiedDate(const QString & d)
{
	QString date = QString(tr("Last modified date: %1")).arg(d);
	lastModifiedDateLabel->setText(date);
}

void XWTeXReaderMainWindow::setSubject(const QString & s)
{
	QString tip = QString(tr("Subject: %1\n")).arg(s);
	fileLabel->setToolTip(tip);
}

void XWTeXReaderMainWindow::setTitle(const QString & t)
{
	QString title = xwApp->getProductName();
	if (!t.isEmpty())
		title = QString("%1").arg(t);
	
	setWindowTitle(title);
}

void XWTeXReaderMainWindow::setToolsBarsVisible(bool e)
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
			
		if (historyToolsBar->isVisible())
			historyToolsBar->setVisible(false);
			
		if (gotoToolsBar->isVisible())
			gotoToolsBar->setVisible(false);
			
		if (zoomToolsBar->isVisible())
			zoomToolsBar->setVisible(false);
			
		if (fitToolsBar->isVisible())
			fitToolsBar->setVisible(false);
			
		if (rotateToolsBar->isVisible())
			rotateToolsBar->setVisible(false);
			
		if (modeToolsBar->isVisible())
			modeToolsBar->setVisible(false);
			
		if (screenToolsBar->isVisible())
			screenToolsBar->setVisible(false);
			
		if (playToolsBar->isVisible())
			playToolsBar->setVisible(false);
			
		if (markToolsBar->isVisible())
			markToolsBar->setVisible(false);
	}
	
	QAction * a = fileToolsBar->toggleViewAction();
	a->setChecked(fileToolsBar->isVisible());
	a = saveToolsBar->toggleViewAction();
	a->setChecked(saveToolsBar->isVisible());
	a = historyToolsBar->toggleViewAction();
	a->setChecked(historyToolsBar->isVisible());
	a = gotoToolsBar->toggleViewAction();
	a->setChecked(gotoToolsBar->isVisible());
	a = zoomToolsBar->toggleViewAction();
	a->setChecked(zoomToolsBar->isVisible());
	a = fitToolsBar->toggleViewAction();
	a->setChecked(fitToolsBar->isVisible());
	a = rotateToolsBar->toggleViewAction();
	a->setChecked(rotateToolsBar->isVisible());
	a = modeToolsBar->toggleViewAction();
	a->setChecked(modeToolsBar->isVisible());
	a = screenToolsBar->toggleViewAction();
	a->setChecked(screenToolsBar->isVisible());
	a = playToolsBar->toggleViewAction();
	a->setChecked(playToolsBar->isVisible());
	a = markToolsBar->toggleViewAction();
	a->setChecked(markToolsBar->isVisible());
}

void XWTeXReaderMainWindow::showSearchResult(int pg, double minx, double miny, 
                                          double maxx, double maxy)
{
	core->displayPage(pg, minx, miny, maxx,maxy);
}

void XWTeXReaderMainWindow::updateActions()
{
	if (!(core->getDoc()))
	{
		fileToolsBar->setEnabled(true);
		saveToolsBar->setEnabled(true);
		fileMenu->setEnabled(true);
		toDviAct->setEnabled(false);
		toPDFAct->setEnabled(false);
		toPSAct->setEnabled(false);
		toTextAct->setEnabled(false);
		toImageAct->setEnabled(false);
		fontDefAct->setEnabled(false);
		printAct->setEnabled(false);
		
		copyAct->setEnabled(false);
		gotoNextAct->setEnabled(false);
		gotoPrevAct->setEnabled(false);
		gotoFirstAct->setEnabled(false);
		gotoLastAct->setEnabled(false);
		findAct->setEnabled(false);
		findNextAct->setEnabled(false);
		
		historyToolsBar->setEnabled(false);
		gotoToolsBar->setEnabled(false);
		
		refreshAct->setEnabled(false);
		zoomInAct->setEnabled(false);
		zoomOutAct->setEnabled(false);
		fitPageAct->setEnabled(false);
		fitWidthAct->setEnabled(false);
		rotateAct->setEnabled(false);
		rtlAct->setEnabled(false);
		horiAct->setEnabled(false);
		singlePageAct->setEnabled(false);
		continuousPagesAct->setEnabled(false);
		readingAct->setEnabled(false);
		fullScreenAct->setEnabled(false);
		playAct->setEnabled(false);
		
		zoomToolsBar->setEnabled(false);
		fitToolsBar->setEnabled(false);
		rotateToolsBar->setEnabled(false);
		modeToolsBar->setEnabled(false);
		screenToolsBar->setEnabled(false);
		playToolsBar->setEnabled(false);
		
		markToolsBar->setEnabled(false);
		markAct->setEnabled(false);
		markCAct->setEnabled(false);
		return ;
	}
	
	historyToolsBar->setEnabled(true);
	gotoToolsBar->setEnabled(true);
	
	refreshAct->setEnabled(true);
	zoomInAct->setEnabled(true);
	zoomOutAct->setEnabled(true);
	fitPageAct->setEnabled(true);
	fitWidthAct->setEnabled(true);
	rotateAct->setEnabled(true);
	rtlAct->setEnabled(true);
	horiAct->setEnabled(true);
	singlePageAct->setEnabled(true);
	continuousPagesAct->setEnabled(true);
	readingAct->setEnabled(true);
	fullScreenAct->setEnabled(true);
	playAct->setEnabled(true);
	
	zoomToolsBar->setEnabled(true);
	fitToolsBar->setEnabled(true);
	rotateToolsBar->setEnabled(true);
	modeToolsBar->setEnabled(true);
	screenToolsBar->setEnabled(true);
	playToolsBar->setEnabled(true);
		
	markToolsBar->setEnabled(true);
	markAct->setEnabled(true);
	markCAct->setEnabled(true);
	
	toDviAct->setEnabled(true);
	toPDFAct->setEnabled(true);
	toPSAct->setEnabled(true);
	toTextAct->setEnabled(true);
	toImageAct->setEnabled(true);
	fontDefAct->setEnabled(true);
	printAct->setEnabled(true);
	
	gotoPageCombo->clear();
	int pgnum = core->getNumPages();
	for (int i = 1; i <= pgnum; i++)
	{
		QString tmp = QString("%1").arg(i);
		gotoPageCombo->addItem(tmp);
	}
}
