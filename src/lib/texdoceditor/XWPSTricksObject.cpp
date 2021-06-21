/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include "PSTricksKeyWord.h"
#include "XWPSTricksPool.h"
#include "XWTeXDocument.h"
#include "XWPSTricksObject.h"

XWTeXDocumentObject * createPSTricksObject(int t, XWTeXDocument * docA, QObject * parent)
{
  XWTeXDocumentObject * obj = 0;
  switch (t)
  {
    case PSTPSTricksOff:
      obj = new XWPSTricksOff(docA,parent);
      break;

    case PSTaltcolormode:
      obj = new XWPSTricksAltcolormode(docA,parent);
      break;

    case PSTpspicture:
      obj = new XWPSTricksPspicture(docA,parent);
      break;

    case PSTpsmatrix:
      obj = new XWPSTricksPsmatrix(docA,parent);
      break;

    case PSTpsTree:
      obj = new XWpsTree(docA,parent);
      break;

    case PSTpstree:
      obj = new XWpstree(docA,parent);
      break;

    default:
      break;
  }

  return obj;
}

XWPSTricksBlock::XWPSTricksBlock(int tA, XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentBlock(tA,docA,parent)
{}

XWPSTricksBlock::XWPSTricksBlock(int tA, XWTeXDocument * docA,
                     bool newrow,
                     const QString & str,
                     QObject * parent)
: XWTeXDocumentBlock(tA,docA,newrow,str, parent)
{}

void XWPSTricksBlock::scanEnvironment(int endid, const QString & str, int & len, int & pos)
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
      int id = lookupPSTrickID(key);
      switch (id)
      {
        case PSTend:
          key = scanEnviromentName(str,len,pos);
          id = lookupPSTrickID(key);
          if (id == endid)
          {
            b--;
            if (b < 0)
              return ;
          }
          break;

        case PSTbegin:
          key = scanEnviromentName(str,len,pos);
          id = lookupPSTrickID(key);
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

XWPSTricksCode::XWPSTricksCode(XWTeXDocument * docA, const QString & nameA,QObject * parent)
:XWTeXDocumentObject(XW_PSTRICKS_CODE,docA,parent)
{
  name = new XWPSTricksBlock(TEX_DOC_B_NAME, docA, true, nameA, this);
  name->specialFont = false;
  head = new XWPSTricksBlock(TEX_DOC_B_VERB, docA, this);
  last = head;
  last->specialFont = false;
}

void XWPSTricksCode::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setVerbFont();
  XWPSTricksBlock * b = (XWPSTricksBlock*)last;
  if (b->text.isEmpty())
  {
    name->newPage = false;
    name->newRow = newRow;
    name->indent = indent;
    last->newPage = false;
    last->newRow = false;
    QColor lcolor = color.darker(TEX_TIP_DARKER);
    name->breakPage(font, lcolor, curx,cury,firstcolumn);
  }
  else
  {
    last->newPage = false;
    last->newRow = newRow;
  }

  last->indent = indent;
  last->breakPage(font, color, curx,cury,firstcolumn);
}

bool XWPSTricksCode::isAllSelected()
{
  return last->isAllSelected();
}

bool XWPSTricksCode::isAtEnd()
{
  return last->isAtEnd();
}

bool XWPSTricksCode::isAtStart()
{
  return last->isAtStart();
}

bool XWPSTricksCode::isEmpty()
{
  return last->isEmpty();
}

QString XWPSTricksCode::getMimeData()
{
  QString str = last->getMimeData();
  str = str.trimmed();
  return str;
}

bool XWPSTricksCode::hasSelected()
{
  return last->hasSelected();
}

void XWPSTricksCode::scanEnvironment(int endid, const QString & str, int & len, int & pos)
{
  XWPSTricksBlock * b = (XWPSTricksBlock*)last;
  b->scanEnvironment(endid, str, len,pos);
}

void XWPSTricksCode::scanParam(const QString & str, int & len, int & pos)
{
  XWPSTricksBlock * b = (XWPSTricksBlock*)last;
  b->scanParam(str, len,pos);
}

void XWPSTricksCode::setCursorAtStart()
{
  last->setCursorAtStart();
}

XWPSTricksOff::XWPSTricksOff(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(PSTPSTricksOff, docA, true, "\\PSTricksOff", parent)
{
  afterNewRow = true;
}

XWPSTricksAltcolormode::XWPSTricksAltcolormode(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(PSTaltcolormode, docA, true, "\\altcolormode", parent)
{
  afterNewRow = true;
}

XWPSTricksPspicture::XWPSTricksPspicture(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(PSTpspicture,docA,true, "\\begin{pspicture}",parent)
{
  afterNewRow = true;
  add(new XWPSTricksCode(docA, tr("picture objects:"), this));
  add(new XWTeXControlSequence(docA, true, "\\end{pspicture}", this));
}

void XWPSTricksPspicture::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  double oldindent = indent;
  indent += 2.0;
  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  head->next->newPage = false;
  head->next->newRow = true;
  head->next->indent = indent;
  head->next->breakPage(curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

bool XWPSTricksPspicture::hasSelected()
{
  return head->next->hasSelected();
}

bool XWPSTricksPspicture::isAllSelected()
{
  return head->next->hasSelected() && (head->hasSelected() || last->hasSelected());
}

bool XWPSTricksPspicture::isEmpty()
{
  return head->next->isEmpty();
}

void XWPSTricksPspicture::scan(const QString & str, int & len, int & pos)
{
  XWPSTricksCode * code = (XWPSTricksCode*)(head->next);
  code->scanEnvironment(PSTpspicture, str,len,pos);
}

void XWPSTricksPspicture::setCursor()
{
  head->next->setCursor();
}

XWPSTricksPsmatrix::XWPSTricksPsmatrix(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(PSTpsmatrix,docA,true, "\\begin{psmatrix}", parent)
{
  afterNewRow = true;
  add(new XWPSTricksCode(docA, tr("nodes:"), this));
  add(new XWTeXControlSequence(docA, true, "\\end{psmatrix}", this));
}

void XWPSTricksPsmatrix::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  double oldindent = indent;
  indent += 2.0;
  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  head->next->newPage = false;
  head->next->newRow = true;
  head->next->indent = indent;
  head->next->breakPage(curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

bool XWPSTricksPsmatrix::hasSelected()
{
  return head->next->hasSelected();
}

bool XWPSTricksPsmatrix::isAllSelected()
{
  return head->next->hasSelected() && (head->hasSelected() || last->hasSelected());
}

bool XWPSTricksPsmatrix::isEmpty()
{
  return head->next->isEmpty();
}

void XWPSTricksPsmatrix::scan(const QString & str, int & len, int & pos)
{
  XWPSTricksCode * code = (XWPSTricksCode*)(head->next);
  code->scanEnvironment(PSTpsmatrix, str,len,pos);
}

void XWPSTricksPsmatrix::setCursor()
{
  head->next->setCursor();
}

XWpsTree::XWpsTree(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(PSTpsTree,docA,true, "\\begin{psTree}", parent)
{
  afterNewRow = true;
  add(new XWPSTricksCode(docA, tr("root:"), this));
  add(new XWPSTricksCode(docA, tr("successors:"), this));
  add(new XWTeXControlSequence(docA, true, "\\end{psTree}", this));
}

void XWpsTree::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setFont();
  double oldindent = indent;
  indent += 2.0;
  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(font, color, curx,cury,firstcolumn);
  head->next->newPage = false;
  head->next->newRow = true;
  head->next->indent = indent;
  head->next->breakPage(curx,cury,firstcolumn);
  head->next->next->newPage = false;
  head->next->next->newRow = true;
  head->next->next->indent = indent;
  head->next->next->breakPage(curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(font, color, curx,cury,firstcolumn);
  indent = oldindent;
}

QString XWpsTree::getMimeData()
{
  QString str = head->getMimeData();
  QString tmp = head->next->getMimeData();
  str = QString("%1{%2}").arg(str).arg(tmp);
  tmp = head->next->next->getMimeData();
  str += tmp;
  tmp = last->getMimeData();
  str += tmp;
  return str;
}

bool XWpsTree::hasSelected()
{
  return head->next->hasSelected() || head->next->next->hasSelected();
}

bool XWpsTree::isAllSelected()
{
  return (head->next->hasSelected() || head->next->next->hasSelected()) &&
         (head->hasSelected() || last->hasSelected());
}

bool XWpsTree::isEmpty()
{
  return head->next->isEmpty() && head->next->next->isEmpty();
}

void XWpsTree::scan(const QString & str, int & len, int & pos)
{
  XWPSTricksCode * code = (XWPSTricksCode*)(head->next);
  code->scanParam(str,len,pos);
  code = (XWPSTricksCode*)(head->next->next);
  code->scanEnvironment(PSTpsTree, str,len,pos);
}

void XWpsTree::setCursor()
{
  head->next->setCursor();
}

void XWpsTree::write(QTextStream & strm, int & linelen)
{
  if (linelen > 10)
  {
    strm << "\n";
    linelen = 0;
  }

  QString str = head->getMimeData();
  QString tmp = head->next->getMimeData();
  str = QString("%1{%2}").arg(str).arg(tmp);
  strm << str;
  strm << "\n";
  str = head->next->next->getMimeData();
  strm << str;
  strm << "\n";
  str = last->getMimeData();
  strm << str;
  strm << "\n";
  linelen = 0;
}

XWpstree::XWpstree(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(PSTpstree,docA,true, "\\pstree", parent)
{
  afterNewRow = true;
  add(new XWPSTricksCode(docA, tr("root:"), this));
  add(new XWPSTricksCode(docA, tr("successors:"), this));
}

void XWpstree::breakPage(double & curx,double & cury,bool & firstcolumn)
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
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

QString XWpstree::getMimeData()
{
  QString str = head->getMimeData();
  QString tmp = head->next->getMimeData();
  str = QString("%1{%2}").arg(str).arg(tmp);
  tmp = last->getMimeData();
  str = QString("%1{%2}").arg(str).arg(tmp);
  return str;
}

bool XWpstree::hasSelected()
{
  return head->next->hasSelected() || last->hasSelected();
}

bool XWpstree::isAllSelected()
{
  return head->hasSelected() && (head->next->hasSelected() || last->hasSelected());
}

bool XWpstree::isEmpty()
{
  return head->next->isEmpty() && last->isEmpty();
}

void XWpstree::scan(const QString & str, int & len, int & pos)
{
  XWPSTricksCode * code = (XWPSTricksCode*)(head->next);
  code->scanParam(str,len,pos);
  code = (XWPSTricksCode*)(last);
  code->scanParam(str,len,pos);
}

void XWpstree::setCursor()
{
  head->next->setCursor();
}

void XWpstree::write(QTextStream & strm, int & linelen)
{
  if (linelen > 10)
  {
    strm << "\n";
    linelen = 0;
  }

  QString str = head->getMimeData();
  QString tmp = head->next->getMimeData();
  str = QString("%1{%2}").arg(str).arg(tmp);
  strm << str;
  strm << "\n";
  str = last->getMimeData();
  str = QString("{%2}").arg(str);
  strm << str;
  strm << "\n";
  linelen = 0;
}
