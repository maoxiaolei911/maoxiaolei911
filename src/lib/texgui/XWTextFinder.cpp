/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWTeXTextEdit.h"
#include "XWTextFinder.h"

 XWTextFinder::XWTextFinder(QObject * parent)
 :QThread(parent),
  mdiArea(0)
 {}

void XWTextFinder::find(QMdiArea * area, const QString & pathA,
                        const QString & str, bool casesensitivity, 
                        bool wholeword, bool regexpA)
{
  isReplace = false;
  mdiArea = area;
  path = pathA;
  findString = str;
  caseSensitivity = casesensitivity;
  wholeWord = wholeword;
  isRegexpFind = regexpA;
  if (isRegexpFind)
    regexp.setPattern(findString);

  start();
}

void XWTextFinder::replace(QMdiArea * area, const QString & pathA,
                 const QString & str, const QString & bystr, 
                 bool casesensitivity, bool wholeword, bool regexpA)
{
  isReplace = true;
  mdiArea = area;
  path = pathA;
  findString = str;
  replaceString = bystr;
  caseSensitivity = casesensitivity;
  wholeWord = wholeword;
  isRegexpFind = regexpA;
  if (isRegexpFind)
    regexp.setPattern(findString);

  start();
}

XWTeXTextEdit * XWTextFinder::takeNewEditor()
{
  return edits.takeFirst();
}

void XWTextFinder::run()
{
  findOrReplaceInPatn(path);
}

XWTeXTextEdit * XWTextFinder::findMdiChild(const QString & fileName)
{
  foreach (QMdiSubWindow *window, mdiArea->subWindowList())
	{
    XWTeXTextEdit *mdiChild = qobject_cast<XWTeXTextEdit *>(window->widget());
    QString fn = mdiChild->getFileName();
    if (fn == fileName)
      return mdiChild;
  }
  return 0;
}

void XWTextFinder::findOrReplaceInPatn(const QString & pathA)
{
  QFileInfo finfo(pathA);
  if (finfo.isFile())
    findOrReplaceInFile(pathA);
  else if (finfo.isDir())
  {
    QDir dir(pathA);
    QStringList filters;
	  filters << "*.tex" << "*.txt" << "*.cls" << "*.clo" << "*.sty" << "*.ini" ;
    filters << "..ltx" << "*.fd" << "*.cfg" << "*.def" << "*.dfu" << "*.otp";
    QStringList files = dir.entryList(filters, QDir::Files);
    for (int i = 0; i < files.size(); i++)
    {
      QString name = files.at(i);
      QString filename = QString("%1/%2").arg(pathA).arg(name);
      findOrReplaceInFile(filename);
    }
    QStringList dirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < dirs.size(); i++)
    {
      QString d = dirs.at(i);
      QString p = QString("%1/%2").arg(pathA).arg(d);
      findOrReplaceInPatn(p);
    }
  }
}

void XWTextFinder::findOrReplaceInFile(const QString & fileName)
{
  XWTeXTextEdit *edit = findMdiChild(fileName);
  if (!edit)
  {
    QFile file(fileName);
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
      file.close();
      Qt::CaseSensitivity cs = Qt::CaseSensitive;
      if (!caseSensitivity)
        cs = Qt::CaseInsensitive;
      bool has = false;
      if (isRegexpFind)
        has = all.contains(regexp);
      else
        has = all.contains(findString,cs);
        
      if (has)
      {
        edit = new XWTeXTextEdit;
        edits << edit;
        mdiArea->addSubWindow(edit);
        edit->loadFile(fileName);
      }
    }
  }

  if (edit)
  {
    if (isReplace)
      edit->replaceAll(findString,replaceString,caseSensitivity,wholeWord,isRegexpFind);
    else
      edit->findAll(findString,caseSensitivity,wholeWord,isRegexpFind);
  }
}