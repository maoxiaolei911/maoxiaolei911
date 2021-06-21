/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QCoreApplication>
#include "XWApplication.h"
#include "XWTeXPool.h"
#include "TeXKeyWord.h"
#include "XWTeXGuiType.h"
#include "XWTeXGuiDev.h"
#include "XWTeXDoc.h"

XWTeXDoc::XWTeXDoc(QObject * parent)
	:XWDoc(parent)
{
	dev = new XWTeXGuiDev(this);	
	undoStack = new QUndoStack;
	undoStack->setUndoLimit(50);
	ok = true;
}

XWTeXDoc::~XWTeXDoc()
{
	if (dev)
		delete dev;
		
	if (undoStack)
		delete undoStack;
}

bool XWTeXDoc::atEnd()
{
	return dev->atEnd();
}

void XWTeXDoc::compile()
{
	setCatalog(0);
	setOutline(0);	
	setOptContent(0);
	dev->compile();
}

QAction * XWTeXDoc::createRedoAction(QObject * parent, const QString & prefix) const
{
	return undoStack->createRedoAction(parent, prefix);
}

QAction * XWTeXDoc::createUndoAction(QObject * parent, const QString & prefix) const
{
	return undoStack->createUndoAction(parent, prefix);
}

void XWTeXDoc::displayPage(XWOutputDev *out, 
                           int page,
		                       double hDPI, 
		                       double vDPI, 
		                       int rotate,
		                       bool useMediaBox, 
		                       bool crop, 
		                       bool printing,
		                       bool (*abortCheckCbk)(void *data),
		                       void *abortCheckCbkData)
{
	if (!catalog)
		return ;
		
	dev->setDPI(hDPI, vDPI);
	dev->doPage(page - 1);
	
	XWDoc::displayPage(out, page, 
		                 hDPI, vDPI, 
		                 rotate, useMediaBox, 
		                 crop, printing, 
		                 abortCheckCbk, abortCheckCbkData);
}

void XWTeXDoc::displayPageSlice(XWOutputDev *out, 
	                              int page,
			                          double hDPI, 
			                          double vDPI, 
			                          int rotate,
			                          bool useMediaBox, 
			                          bool crop, 
			                          bool printing,
			                          int sliceX, 
			                          int sliceY, 
			                          int sliceW, 
			                          int sliceH,
			                          bool (*abortCheckCbk)(void *data),
			                          void *abortCheckCbkData)
{
	if (!catalog)
		return ;
		
	dev->setDPI(hDPI, vDPI);
	dev->doPage(page - 1);
	
	XWDoc::displayPageSlice(out, page, 
		                      hDPI, vDPI, 
		                      rotate, useMediaBox, 
		                      crop, printing, 
		                      sliceX, sliceY,
		                      sliceW, sliceH,
		                      abortCheckCbk, abortCheckCbkData);
}

void XWTeXDoc::endInput()
{
	dev->endInput();
}

XWLinkDest * XWTeXDoc::findDest(XWString * name)
{
	if (catalog)
		return catalog->findDest(name);
		
	return 0;
}

int XWTeXDoc::findPage(int num, int gen)
{
	if (catalog)
		return catalog->findPage(num, gen);
		
	return 0;
}

QByteArray XWTeXDoc::getByteArray(const QString & txt)
{
	return dev->getByteArray(txt);
}

TeXCategoryCode XWTeXDoc::getCategory()
{
	return dev->getCategory();
}

qint32  XWTeXDoc::getChar()
{
	return dev->getChar();
}

qint32 XWTeXDoc::getControlSequence()
{
	qint32 buf[100];
	qint32 len = getControlSequence(buf);
	return lookupTexID(buf, len);
}

qint32  XWTeXDoc::getControlSequence(qint32 * buf)
{
	return dev->getControlSequence(buf);
}

QString XWTeXDoc::getCoord()
{
	return dev->getCoord();
}

QString XWTeXDoc::getDumpName()
{
	return dev->getDumpName();
}

TeXFileMode  XWTeXDoc::getFileMode()
{
	return dev->getFileMode();
}

DVIFontDef * XWTeXDoc::getFontDefs()
{
	return dev->getFontDefs();
}

QString XWTeXDoc::getJobName()
{
	return dev->getJobName();
}

QString XWTeXDoc::getKey()
{
	return dev->getKey();
}

XWLinks * XWTeXDoc::getLinks(int page)
{
	if (catalog)
		return catalog->getPage(page)->getLinks(catalog);
	
	return 0;
}

QString XWTeXDoc::getMainInputFile()
{
	return dev->getMainInputFile();
}

int  XWTeXDoc::getNumberOfDefFont()
{
	return dev->getNumberOfDefFont();
}

int XWTeXDoc::getNumPages()
{
	if (catalog)
		return catalog->getNumPages();
		
	return 0;
}

QString XWTeXDoc::getOption()
{
	return dev->getOption();
}

QString XWTeXDoc::getOutputComment()
{
	return dev->getOutputComment();
}

double  XWTeXDoc::getPageCropHeight(int page)
{
	if (catalog)
		return catalog->getPage(page)->getCropHeight();
	
	return 0.0;
}

double XWTeXDoc::getPageCropWidth(int page)
{
	if (catalog)
		return catalog->getPage(page)->getCropWidth();
		
	return 0.0;
}

double XWTeXDoc::getPageMediaHeight(int page)
{
	if (catalog)
		return catalog->getPage(page)->getMediaHeight();
		
	return 0.0;
}

double  XWTeXDoc::getPageMediaWidth(int page)
{
	if (catalog)
		return catalog->getPage(page)->getMediaWidth();
		
	return 0.0;
}

int  XWTeXDoc::getPageRotate(int page)
{
	if (catalog)
		return catalog->getPage(page)->getRotate();
		
	return 0;
}

QString XWTeXDoc::getParam()
{
	return dev->getParam();
}

qint32  XWTeXDoc::getPos()
{
	return dev->getPos();
}

qint32 XWTeXDoc::getSize()
{
	return dev->getPos();
}

QString XWTeXDoc::getString(qint32 filepos, qint32 len)
{
	return dev->getString(filepos, len);
}

QString XWTeXDoc::getText()
{
	return dev->getText();
}

QString  XWTeXDoc::getValue()
{
	return dev->getValue();
}

QString  XWTeXDoc::getWord()
{
	return dev->getWord();
}

bool XWTeXDoc::hasFmt()
{
	return dev->hasDumpName();
}

qint32 XWTeXDoc::insert(qint32 filepos, 
	                    const QByteArray & txt)
{
	qint32 len = dev->insert(filepos, txt);
	emit contentChanged(filepos, len);
	return len;
}

qint32 XWTeXDoc::insert(qint32 filepos, 
	                    const QString & txt)
{
	qint32 len = dev->insert(filepos, txt);
	emit contentChanged(filepos, len);
	return len;
}

qint32 XWTeXDoc::insertFile(qint32 filepos, 
	                        const QString & filename)
{
	qint32 len = dev->insertFile(filepos, filename);
	emit contentChanged(filepos, len);
	return len;
}

bool XWTeXDoc::loadFile(const QString & filename)
{
	undoStack->clear();
	return dev->loadFile(filename);
}

bool XWTeXDoc::loadFmt(const QString & filename)
{
	return dev->loadFmt(filename);
}

void XWTeXDoc::processLinks(XWOutputDev *out, int page)
{
	if (catalog)
		catalog->getPage(page)->processLinks(out, catalog);
}

void XWTeXDoc::pushCommand(QUndoCommand * cmd)
{
	undoStack->push(cmd);
}

QByteArray  XWTeXDoc::read(qint32 filepos, qint32 len)
{
	return dev->read(filepos, len);
}

QString XWTeXDoc::readLine()
{
	return dev->readLine();
}

void XWTeXDoc::redo()
{
	undoStack->redo();
}

void XWTeXDoc::remove(qint32 filepos, qint32 len)
{
	dev->remove(filepos, len);
	emit contentChanged(filepos, -len);
}

qint32 XWTeXDoc::removeFile(qint32 filepos, const QString & filename)
{
	qint32 len = dev->removeFile(filepos, filename);
	emit contentChanged(filepos, len);
	return len;
}

qint32 XWTeXDoc::replace(qint32 filepos, 
	                     qint32 len, 
	                     const QByteArray & txt)
{
	len = dev->replace(filepos, len, txt);
	emit contentChanged(filepos, len);
	return len;
}

qint32 XWTeXDoc::replace(qint32 filepos, 
	                     qint32 len, 
	                     const QString & txt)
{
	len = dev->replace(filepos, len, txt);
	emit contentChanged(filepos, len);
	return len;
}

void XWTeXDoc::save()
{
	dev->save();
}

void XWTeXDoc::save(const QString & filename)
{
	dev->save(filename);
}

void XWTeXDoc::saveToDVI(QIODevice * outfileA)
{
	dev->saveToDVI(outfileA);
}

void XWTeXDoc::saveToPDF(QIODevice * outfileA,
	                       char *dviname, 
	                       char *pdfname,
	                       const QString & thumbbasename,
	                       const QList<int> & pages,
	                       char * owner_pw,
	                       char * user_pw)
{
	dev->saveToPDF(outfileA, dviname, pdfname, thumbbasename, pages, owner_pw, user_pw);
}

bool XWTeXDoc::seek(qint32 filepos)
{
	return dev->seek(filepos);
}

void XWTeXDoc::setOutputComment(const QString & s)
{
	dev->setOutputComment(s); 
}

void XWTeXDoc::skipComment()
{
	dev->skipComment();
}

void XWTeXDoc::skipControlSequence()
{
	dev->skipControlSequence();
}

void XWTeXDoc::skipCoord()
{
	dev->skipCoord();
}

void XWTeXDoc::skipFormula()
{
	dev->skipFormula();	
}

void XWTeXDoc::skipGroup()
{
	dev->skipGroup();
}

void XWTeXDoc::skipOption()
{
	dev->skipOption();
}

void XWTeXDoc::skipSpacer()
{
	dev->skipSpacer();
}

void XWTeXDoc::skipSpacerAndComment()
{
	dev->skipSpacerAndComment();
}

void XWTeXDoc::skipStarred()
{
	skipSpacerAndComment();
	qint32 pos = getPos();
	qint32 c = getChar();
	if (c != '*')
		seek(pos);
}

void XWTeXDoc::skipWord()
{
	dev->skipWord();
}

void XWTeXDoc::undo()
{
	undoStack->undo();
}
