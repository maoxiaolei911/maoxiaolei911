/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QTextCodec>
#include <QTextStream>
#include <QTextDocument>
#include <QTextCursor>
#include "XWString.h"
#include "XWDoc.h"
#include "XWTextOutputDev.h"
#include "XWDocSearchWidget.h"
#include "XWSearcher.h"

XWSearcher::XWSearcher(QObject * parent)
:QThread(parent),
 stype(XW_SEACHER_FILE),
 caseSensitivity(true),
 wholeWord(true),
 regexpFind(false),
 isReplace(false)
{}

int XWSearcher::getTextLength()
{
  if (!replacing.isEmpty())
    return replacing.length();

  return text.length();
}

XWPDFSearcher::XWPDFSearcher(QObject * parent)
:XWSearcher(parent),
 doc(0),
 dpi(72),
 rotate(0),
 list(0)
{}

void XWPDFSearcher::run()
{
  if (!doc || !list)
    return ;

  int len = text.length();
  uint * u = new uint[len];
  for (int i = 0; i < len; i++)
		u[i] = text[i].unicode();

  XWTextOutputDev * tdev = new XWTextOutputDev(NULL, true, 0, false, false);
  double xMin, yMin, xMax, yMax;
  for (int i = 1; i <= doc->getNumPages(); ++i)
  {
    xMin = yMin = xMax = yMax = 0;
    doc->displayPage(tdev, i, dpi, dpi, rotate, false, true, false);
    XWTextPage * page = tdev->takeText();
    while (page->findText(u, len, true, true, false, true,
			    						caseSensitivity, false, wholeWord,
			    						&xMin, &yMin, &xMax, &yMax))
    {
      XWString * s = page->getText(xMin - 100, yMin, xMax + 100, yMax);
      QString tmp = s->toQString();
      delete s;
      list->createItem(i, xMin, yMin, xMax, yMax, tmp);
    }

    delete page;
  }

  delete [] u;
  delete tdev;
}

XWQTextDocumentSearcher::XWQTextDocumentSearcher(QObject * parent)
:XWSearcher(parent)
{}

void XWQTextDocumentSearcher::run()
{
  switch (stype)
  {
    default:
      searchDocument();
      break;

    case XW_SEACHER_CUR_DIR:
      searchPath();
      break;

    case XW_SEACHER_DIRS:
      searchRoot();
      break;
  }
}

void XWQTextDocumentSearcher::searchDocument()
{
  if (!doc)
    return ;

  QTextDocument::FindFlags flags = 0;
	if (caseSensitivity)
	  flags |= QTextDocument::FindCaseSensitively;

	if (wholeWord)
	  flags |= QTextDocument::FindWholeWords;

  QRegExp reg(text);
  QTextCursor cursor(doc);
  if (isReplace)
    cursor.beginEditBlock();
  while (!cursor.isNull() && !cursor.atEnd())
  {
    if (regexpFind)
      cursor = doc->find(reg, cursor, flags);
    else
      cursor = doc->find(text, cursor, flags);

    if (!cursor.isNull())
    {
      if (isReplace)
      {
        int pos = cursor.position();
        cursor.insertText(replacing);
        QTextCursor cc = cursor;
        cc.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
        cc.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
        QString tmp = cc.selectedText();
        list->createTextItem(pos, tmp);
      }
      else
      {
        int pos = cursor.position();
        QTextCursor cc = cursor;
        cc.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
        cc.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
        QString tmp = cc.selectedText();
        list->createTextItem(pos, tmp);
      }
    }
  }

  if (isReplace)
    cursor.endEditBlock();
}

void XWQTextDocumentSearcher::searchPath()
{
  if (path.isEmpty())
    return ;
    
  QTextDocument::FindFlags flags = 0;
	if (caseSensitivity)
	  flags |= QTextDocument::FindCaseSensitively;

	if (wholeWord)
	  flags |= QTextDocument::FindWholeWords;

  QRegExp reg(text);
  QString oldpath = path;
  QDir dir(oldpath);
  QStringList filters;
	filters << "*.tex" << "*.txt" << "*.cls" << "*.clo" << "*.sty" << "*.ini" ;
  filters << "*.ltx" << "*.fd" << "*.cfg" << "*.def" << "*.dfu" << "*.otp";
  QStringList files = dir.entryList(filters, QDir::Files);
  for (int i = 0; i < files.size(); i++)
  {
    QString name = files.at(i);
    QString filename = QString("%1/%2").arg(oldpath).arg(name);
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QByteArray cn;
      QByteArray ba = file.readLine();
      if (ba.length() > 0 && ba[0] == '%')
      {
        if (ba.contains("utf-8") || ba.contains("UTF-8"))
	  		  cn = "UTF-8";
		  	else if (ba.contains("gb18030") || ba.contains("GB18030"))
			    cn = "GB18030-0";
			  else if (ba.contains("gbk") || ba.contains("GBK"))
			    cn = "GBK";
			  else if (ba.contains("gb") || ba.contains("GB"))
			    cn = "GB";
			  else if (ba.contains("big5") || ba.contains("Big5") || ba.contains("BIG5"))
			    cn = "Big5";
			  else if (ba.contains("EUC-JP") || ba.contains("euc-jp"))
			    cn = "EUC-JP";
			  else if (ba.contains("EUC-KR") || ba.contains("euc-kr"))
			    cn = "EUC-KR";
			  else if (ba.contains("sjis") || ba.contains("SJIS"))
			    cn = "Shift-JIS";
      }

      QTextStream stream(&file);
		  if (!cn.isEmpty())
		  {
		    QTextCodec *codec = QTextCodec::codecForName(cn);
		    stream.setCodec(codec);
		  }
		  else
		  {
		    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
		    stream.setCodec(codec);
		  }

      QString all = stream.readAll();
      doc = new QTextDocument(this);
      doc->setPlainText(all);
      QTextCursor cursor(doc);
      if (regexpFind)
        cursor = doc->find(reg, cursor, flags);
      else
        cursor = doc->find(text, cursor, flags);
      if (!cursor.isNull())
      {
        list->createFileItem(filename);
        searchDocument();
      }
      delete doc;
      doc = 0;
    }
  }

  QStringList dirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
  for (int i = 0; i < dirs.size(); i++)
  {
    QString d = dirs.at(i);
    path = QString("%1/%2").arg(oldpath).arg(d);
    searchPath();
  }

  path = oldpath;
}

void XWQTextDocumentSearcher::searchRoot()
{
  if (root.isEmpty())
    return ;

  path = root;
  searchPath();
}
