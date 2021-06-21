/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include "PGFKeyWord.h"
#include "XWPGFPool.h"
#include "XWTeXDocument.h"
#include "XWTikzObject.h"

XWTeXDocumentObject * createTikzObject(int t, XWTeXDocument * docA, QObject * parent)
{
  XWTeXDocumentObject * obj = 0;
  switch (t)
  {
    case PGFusetikzlibrary:
      obj = new XWUsetikzlibrary(docA, parent);
      break;

    case PGFtikzdeclarecoordinatesystem:
      obj = new XWTikzdeclarecoordinatesystem(docA, parent);
      break;

    case PGFtikzaliascoordinatesystem:
      obj = new XWTikzaliascoordinatesystem(docA, parent);
      break;

    case PGFtikzfading:
      obj = new XWTikzfading(docA, parent);
      break;

    case PGFtikzfadingfrompicture:
      obj = new XWTikzfadingfrompicture(docA, parent);
      break;

    case PGFtikzexternalize:
      obj = new XWTikzexternalize(docA, parent);
      break;

    case PGFtikzsetexternalprefix:
      obj = new XWTikzsetexternalprefix(docA, parent);
      break;

    case PGFtikzsetnextfilename:
      obj = new XWTikzsetnextfilename(docA, parent);
      break;

    case PGFtikzsetfigurename:
      obj = new XWTikzsetfigurename(docA, parent);
      break;

    case PGFtikzappendtofigurename:
      obj = new XWTikzappendtofigurename(docA, parent);
      break;

    case PGFtikzpicturedependsonfile:
      obj = new XWTikzpicturedependsonfile(docA, parent);
      break;

    case PGFtikzexternalfiledependsonfile:
      obj = new XWTikzexternalfiledependsonfile(docA, parent);
      break;

    case PGFtikzset:
      obj = new XWTikzset(docA, parent);
      break;

    case PGFtikzstyle:
      obj = new XWTikzstyle(docA, parent);
      break;

    case PGFtikz:
      obj = new XWTikz(docA, parent);
      break;

    case PGFtikzpicture:
      obj = new XWTikzpicture(docA, parent);
      break;

    default:
      break;
  }

  return obj;
}

XWTikzBlock::XWTikzBlock(int tA, XWTeXDocument * docA, QObject * parent)
 : XWTeXDocumentBlock(tA,docA,parent)
{}

XWTikzBlock::XWTikzBlock(int tA, XWTeXDocument * docA,
                     bool newrow,
                     const QString & str,
                     QObject * parent)
  : XWTeXDocumentBlock(tA,docA,newrow,str, parent)
{}

void XWTikzBlock::scanEnvironment(int endid, const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  int b = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('\\'))
    {
      int p = pos;
      QString key = scanControlSequence(str,len,pos);
      int id = lookupPGFID(key);
      switch (id)
      {
        case PGFend:
          key = scanEnviromentName(str,len,pos);
          id = lookupPGFID(key);
          if (id == endid)
          {
            b--;
            if (b < 0)
              return ;
          }
          break;

        case PGFbegin:
          key = scanEnviromentName(str,len,pos);
          id = lookupPGFID(key);
          if (id == endid)
            b++;
          break;

        default:
          break;
      }

      key = str.mid(p,pos - p);
      text.append(key);
    }
    else
    {
      text.append(str[pos]);
      pos++;
    }
  }

  text = text.trimmed();
}

void XWTikzBlock::scanOption(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos >= len || str[pos] != QChar('['))
    return ;

  pos++;
  int b = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('['))
      b++;
    else if (str[pos] == QChar(']'))
    {
      b--;
      if (b < 0)
      {
        pos++;
        return ;
      }
    }

    text.append(str[pos]);
    pos++;
  }
  text = text.trimmed();
}

void XWTikzBlock::scanParam(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos >= len || str[pos] != QChar('{'))
    return ;

  pos++;
  int b = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('{'))
      b++;
    else if (str[pos] == QChar('}'))
    {
      b--;
      if (b < 0)
      {
        pos++;
        return ;
      }
    }

    text.append(str[pos]);
    pos++;
  }
  text = text.trimmed();
}

void XWTikzBlock::scanPath(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  while (pos < len)
  {
    if (str[pos] == QChar(';'))
    {
      text.append(str[pos]);
      pos++;
      return ;
    }

    text.append(str[pos]);
    pos++;
  }
  text = text.trimmed();
}

XWTikzCode::XWTikzCode(XWTeXDocument * docA, const QString & nameA,QObject * parent)
 : XWTeXDocumentObject(XW_TIKZ_CODE,docA,parent)
{
  name = new XWTikzBlock(TEX_DOC_B_NAME, docA, true, nameA, this);
  name->specialFont = false;
  head = new XWTikzBlock(TEX_DOC_B_VERB, docA, this);
  last = head;
  last->specialFont = false;
}

void XWTikzCode::breakPage(double & curx, double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setVerbFont();
  XWTikzBlock * b = (XWTikzBlock*)head;
  if (b->text.isEmpty())
  {
    name->newPage = false;
    name->newRow = newRow;
    name->indent = indent;
    head->newPage = false;
    head->newRow = false;
    QColor lcolor = color.darker(TEX_TIP_DARKER);
    name->breakPage(font, lcolor, curx,cury,firstcolumn);
  }
  else
  {
    head->newPage = false;
    head->newRow = newRow;
  }

  head->indent = indent;
  head->breakPage(font, color, curx,cury,firstcolumn);
}

QString XWTikzCode::getMimeData()
{
  QString str = head->getMimeData();
  str = str.trimmed();
  return str;
}

bool XWTikzCode::hasSelected()
{
  return head->hasSelected();
}

bool XWTikzCode::isAtEnd()
{
  return head->isAtEnd();
}

bool XWTikzCode::isAtStart()
{
  return head->isAtStart();
}

bool XWTikzCode::isEmpty()
{
  return head->isEmpty();
}

void XWTikzCode::scanEnvironment(int endid, const QString & str, int & len, int & pos)
{
  XWTikzBlock * b = (XWTikzBlock*)head;
  b->scanEnvironment(endid, str,len, pos);
}

void XWTikzCode::scanParam(const QString & str, int & len, int & pos)
{
  XWTikzBlock * b = (XWTikzBlock*)head;
  b->scanParam(str, len,pos);
}

void XWTikzCode::scanPath(const QString & str, int & len, int & pos)
{
  XWTikzBlock * b = (XWTikzBlock*)head;
  b->scanPath(str,len, pos);
}

void XWTikzCode::setCursorAtStart()
{
  head->setCursorAtStart();
}

XWTikzOption::XWTikzOption(XWTeXDocument * docA, const QString & nameA,QObject * parent)
 : XWTeXDocumentObject(XW_TIKZ_OPTION, docA, parent)
{
  name = new XWTikzBlock(TEX_DOC_B_NAME, docA, false,nameA,this);
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"[",this));
  add(new XWTikzBlock(TEX_DOC_B_VERB, doc, this));
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"]",this));
}

void XWTikzOption::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setOptionFont();
  head->newPage = newPage;
  head->newRow = newRow;
  head->indent = indent;
  head->breakPage(font, color, curx,cury,firstcolumn);
  XWTeXDocumentBlock * b = (XWTeXDocumentBlock*)(head->next);
  if (b->text.isEmpty())
  {
    name->newPage = false;
    name->newRow = false;
    name->indent = indent;
    QColor lcolor = color.darker(TEX_TIP_DARKER);
    name->breakPage(font, lcolor, curx,cury,firstcolumn);
  }

  head->next->newPage = false;
  head->next->newRow = false;
  head->next->indent = indent;
  head->next->breakPage(font, color, curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = false;
  last->indent = indent;
  last->breakPage(font, color, curx,cury,firstcolumn);
}

QString XWTikzOption::getMimeData()
{
  QString text = head->next->getMimeData();
  text = text.trimmed();
  if (text.isEmpty())
    return QString();

  return XWTeXDocumentObject::getMimeData();
}

bool XWTikzOption::hasSelected()
{
  return head->next->hasSelected();
}

bool XWTikzOption::isAtStart()
{
  return head->next->isAtStart();
}

bool XWTikzOption::isEmpty()
{
  return head->next->isEmpty();
}

void XWTikzOption::scan(const QString & str, int & len, int & pos)
{
  XWTikzBlock * b = (XWTikzBlock*)(head->next);
  b->scanOption(str, len,pos);
}

void XWTikzOption::setCursor()
{
  head->next->setCursor();
}

void XWTikzOption::setCursorAtEnd()
{
  head->next->setCursorAtEnd();
}

void XWTikzOption::setCursorAtStart()
{
  head->next->setCursorAtStart();
}

void XWTikzOption::write(QTextStream & strm, int & linelen)
{
  QString text = head->next->getMimeData();
  text = text.trimmed();
  if (text.isEmpty())
    return ;

  XWTeXDocumentObject::write(strm,linelen);
}

XWTikzParam::XWTikzParam(XWTeXDocument * docA, const QString & nameA,QObject * parent)
: XWTeXDocumentObject(XW_TIKZ_PARAM, docA, parent)
{
  name = new XWTikzBlock(TEX_DOC_B_NAME, docA, false,nameA,this);
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"{",this));
  add(new XWTikzBlock(TEX_DOC_B_VERB, doc, this));
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"}",this));
}

void XWTikzParam::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setParamFont();
  head->newPage = newPage;
  head->newRow = newRow;
  head->indent = indent;
  head->breakPage(font, color, curx,cury,firstcolumn);
  XWTeXDocumentBlock * b = (XWTeXDocumentBlock*)(head->next);
  if (b->text.isEmpty())
  {
    name->newPage = false;
    name->newRow = false;
    name->indent = indent;
    QColor lcolor = color.darker(TEX_TIP_DARKER);
    name->breakPage(font, lcolor, curx,cury,firstcolumn);
  }

  head->next->newPage = false;
  head->next->newRow = false;
  head->next->indent = indent;
  head->next->breakPage(font, color, curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = false;
  last->indent = indent;
  last->breakPage(font, color, curx,cury,firstcolumn);
}

bool XWTikzParam::hasSelected()
{
  return head->next->hasSelected();
}

bool XWTikzParam::isAtStart()
{
  return head->next->isAtStart();
}

bool XWTikzParam::isEmpty()
{
  return head->next->isEmpty();
}

void XWTikzParam::scan(const QString & str, int & len, int & pos)
{
  XWTikzBlock * b = (XWTikzBlock*)(head->next);
  b->scanParam(str, len,pos);
}

void XWTikzParam::setCursorAtStart()
{
  head->next->setCursorAtStart();
}

XWUsetikzlibrary::XWUsetikzlibrary(XWTeXDocument * docA, QObject * parent)
 : XWTeXDocumentCommand(PGFusetikzlibrary, docA, true, "\\usetikzlibrary", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("list of libraries: "), this));
}

XWTikzdeclarecoordinatesystem::XWTikzdeclarecoordinatesystem(XWTeXDocument * docA, QObject * parent)
 : XWTeXDocumentCommand(PGFtikzdeclarecoordinatesystem, docA, true, "\\tikzdeclarecoordinatesystem",parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("name:"), this));
  add(new XWTikzCode(docA, tr("code:"), this));
}

void XWTikzdeclarecoordinatesystem::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  double oldindent = indent;
  indent += 2.0;
  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  head->next->newRow = false;
  head->next->indent = indent;
  head->next->breakPage(curx,cury,firstcolumn);
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

QString XWTikzdeclarecoordinatesystem::getMimeData()
{
  QString n = head->getMimeData();
  QString cn = head->next->getMimeData();
  QString c = last->getMimeData();
  QString tmp = QString("%1%2{%3}").arg(n).arg(cn).arg(c);
  return tmp;
}

void XWTikzdeclarecoordinatesystem::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str,len, pos);
  XWTikzCode * code = (XWTikzCode*)last;
  code->scanParam(str, len,pos);
}

void XWTikzdeclarecoordinatesystem::setCursor()
{
  head->next->setCursor();
}

void XWTikzdeclarecoordinatesystem::write(QTextStream & strm, int & linelen)
{
  if (linelen != 0)
  {
    strm << "\n";
    linelen = 0;
  }

  QString n = head->getMimeData();
  QString param = head->next->getMimeData();
  QString tmp = QString("%1%2\\%\n").arg(n).arg(param);
  strm << tmp;

  QString body = last->getMimeData();
  if (body.length() == 0)
    tmp = "{}\n";
  else
  {
    if (body[0] == QChar('\\%'))
      tmp = QString("{%1}\n").arg(body);
    else
      tmp = QString("{\\%\n%1\n}\n").arg(body);
  }

  strm << tmp;
}

XWTikzaliascoordinatesystem::XWTikzaliascoordinatesystem(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(PGFtikzaliascoordinatesystem, docA, true, "\\tikzaliascoordinatesystem",parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("new name:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("old name:"), this));
}

void XWTikzaliascoordinatesystem::setCursor()
{
  head->next->setCursor();
}

XWTikzfading::XWTikzfading(XWTeXDocument * docA, QObject * parent)
 : XWTeXDocumentCommand(PGFtikzfading, docA, true, "\\tikzfading",parent)
{
  afterNewRow = true;
  add(new XWTikzOption(docA, tr("options:"), this));
}

XWTikzfadingfrompicture::XWTikzfadingfrompicture(XWTeXDocument * docA, QObject * parent)
 : XWTeXDocumentCommand(PGFtikzfadingfrompicture, docA, true, "\\begin{tikzfadingfrompicture}",parent)
{
  afterNewRow = true;
  add(new XWTikzOption(docA, tr("options:"), this));
  add(new XWTikzCode(docA, tr("environment contents:"), this));
  add(new XWTeXControlSequence(docA, true, "\\end{tikzfadingfrompicture}", this));
}

void XWTikzfadingfrompicture::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  double oldindent = indent;
  indent += 2.0;
  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  head->next->newRow = false;
  head->next->indent = indent;
  head->next->breakPage(curx,cury,firstcolumn);
  head->next->next->newRow = true;
  head->next->next->indent = indent;
  head->next->next->breakPage(curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

bool XWTikzfadingfrompicture::isAllSelected()
{
  return head->next->next->hasSelected() && (head->hasSelected() || last->hasSelected());
}

bool XWTikzfadingfrompicture::isEmpty()
{
  return head->next->next->isEmpty();
}

void XWTikzfadingfrompicture::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str,len,pos);
  XWTikzCode * code = (XWTikzCode*)(head->next->next);
  code->scanEnvironment(PGFtikzfadingfrompicture, str,len,pos);
}

void XWTikzfadingfrompicture::setCursor()
{
  head->next->next->setCursor();
}

XWTikzexternalize::XWTikzexternalize(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(PGFtikzexternalize, docA, true, "\\tikzexternalize", parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("optional arguments:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA, tr("realfile name:"), this));
}

XWTikzsetexternalprefix::XWTikzsetexternalprefix(XWTeXDocument * docA, QObject * parent)
 : XWTeXDocumentCommand(PGFtikzsetexternalprefix, docA, true, "\\tikzsetexternalprefix", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("File name prefix:"), this));
}

XWTikzsetnextfilename::XWTikzsetnextfilename(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(PGFtikzsetnextfilename, docA, true,"\\tikzsetnextfilename", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("file name:"), this));
}

XWTikzsetfigurename::XWTikzsetfigurename(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(PGFtikzsetfigurename, docA, true, "\\tikzsetfigurename", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("name:"), this));
}

XWTikzappendtofigurename::XWTikzappendtofigurename(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(PGFtikzappendtofigurename, docA, true, "\\tikzappendtofigurename", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("surffix:"), this));
}

XWTikzpicturedependsonfile::XWTikzpicturedependsonfile(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(PGFtikzpicturedependsonfile, docA, true, "\\tikzpicturedependsonfile", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("file name:"), this));
}

XWTikzexternalfiledependsonfile::XWTikzexternalfiledependsonfile(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(PGFtikzexternalfiledependsonfile, docA, true, "\\tikzexternalfiledependsonfile",parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("external graphics:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("file name:"), this));
}

void XWTikzexternalfiledependsonfile::setCursor()
{
  head->next->setCursor();
}

XWTikzset::XWTikzset(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(PGFtikzset, docA, true, "\\tikzset",parent)
{
  afterNewRow = true;
  add(new XWTikzParam(docA, tr("options:"), this));
}

void XWTikzset::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  double oldindent = indent;
  indent += 2.0;
  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = false;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

XWTikzstyle::XWTikzstyle(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(PGFtikzstyle, docA, true, "\\tikzstyle", parent)
{
  afterNewRow = true;
  add(new XWTikzParam(docA, tr("options:"), this));
  add(new XWTikzOption(docA, tr("values:"), this));
}

void XWTikzstyle::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  double oldindent = indent;
  indent += 2.0;
  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  head->next->newPage = false;
  head->next->newRow = false;
  head->next->indent = indent;
  head->next->breakPage(curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = false;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

QString XWTikzstyle::getMimeData()
{
  QString str = head->getMimeData();
  QString tmp = head->next->getMimeData();
  str += tmp;
  tmp = last->getMimeData();
  if (!tmp.isEmpty())
    str = QString("%1=%2").arg(str).arg(tmp);
  return str;
}

bool XWTikzstyle::isAllSelected()
{
  return head->hasSelected() && head->next->hasSelected();
}

bool XWTikzstyle::isEmpty()
{
  return head->next->isEmpty();
}

void XWTikzstyle::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str,len,pos);
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos < len && str[pos] == QChar('='))
    pos++;
  last->scan(str,len,pos);
}

void XWTikzstyle::setCursor()
{
  head->next->setCursor();
}

void XWTikzstyle::write(QTextStream & strm, int & linelen)
{
  if (linelen > 10)
  {
    strm << "\n";
    linelen = 0;
  }

  QString str = head->getMimeData();
  strm << str;
  str = head->next->getMimeData();
  strm << str;
  str = last->getMimeData();
  if (!str.isEmpty())
  {
    strm << "=";
    strm << str;
  }
  strm << "\n";
  linelen = 0;
}

XWTikz::XWTikz(XWTeXDocument * docA, QObject * parent)
 : XWTeXDocumentCommand(PGFtikz, docA,true,"\\tikz",parent)
{
  add(new XWTikzOption(docA, tr("options:"), this));
  add(new XWTikzCode(docA, tr("path commands:"), this));
}

bool XWTikz::hasSelected()
{
  return head->next->hasSelected() || last->hasSelected();
}

bool XWTikz::isEmpty()
{
  return last->isEmpty();
}

void XWTikz::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str, len,pos);
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos >= len)
    return ;

  XWTikzCode * code = (XWTikzCode*)last;
  if (str[pos] == QChar('{'))
    code->scanParam(str,len,pos);
  else
    code->scanPath(str,len,pos);
}

void XWTikz::write(QTextStream & strm, int & linelen)
{
  QString str = head->getMimeData();
  QString tmp = head->next->getMimeData();
  str += tmp;
  tmp = last->getMimeData();
  str = QString("%1{%2}").arg(str).arg(tmp);
  writeStr(str, strm, linelen);
}

XWTikzpicture::XWTikzpicture(XWTeXDocument * docA, QObject * parent)
 : XWTeXDocumentCommand(PGFtikzpicture, docA,true, "\\begin{tikzpicture}", parent)
{
  afterNewRow = true;
  add(new XWTikzOption(docA, tr("options:"), this));
  add(new XWTikzCode(docA, tr("environment contents:"), this));
  add(new XWTeXControlSequence(docA, true, "\\end{tikzpicture}", this));
  last->isOnlyRead = true;
}

void XWTikzpicture::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  double oldindent = indent;
  indent += 2.0;
  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  head->next->newRow = false;
  head->next->indent = indent;
  head->next->breakPage(curx,cury,firstcolumn);
  head->next->next->newRow = true;
  head->next->next->indent = indent;
  head->next->next->breakPage(curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

bool XWTikzpicture::hasSelected()
{
  return head->next->hasSelected() || head->next->next->hasSelected();
}

bool XWTikzpicture::isAllSelected()
{
  return head->next->next->hasSelected() &&
             (head->hasSelected() || last->hasSelected());
}

bool XWTikzpicture::isEmpty()
{
  return head->next->next->isEmpty();
}

void XWTikzpicture::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentObject * obj = head;
  obj = obj->next;
  obj->scan(str,len,pos);
  XWTikzCode * code = (XWTikzCode*)(obj->next);
  code->scanEnvironment(PGFtikzpicture, str,len,pos);
}

void XWTikzpicture::setCursor()
{
  head->next->next->setCursor();
}
