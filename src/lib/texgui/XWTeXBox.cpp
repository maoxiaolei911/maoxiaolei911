/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWTeXBox.h"

XWTeXBox::XWTeXBox(QObject * parent)
:QObject(parent),
 keyWord(-1),
 numberOfParts(0)
{}

bool XWTeXBox::back()
{
  return false;
}

bool XWTeXBox::contains(double , double )
{
  return false;
}

bool XWTeXBox::cut()
{
  return false;
}

bool XWTeXBox::del()
{
  return false;
}

void XWTeXBox::doContent(XWPDFDriver * )
{}

void XWTeXBox::dragTo(XWPDFDriver * ,double ,double )
{}

void XWTeXBox::dragTo(QPainter * ,double ,double)
{}

void XWTeXBox::draw(QPainter * )
{}

bool XWTeXBox::dropTo(double ,double )
{
  return false;
}

int XWTeXBox::getAnchorPosition()
{
  return -1;
}

int XWTeXBox::getCursorPosition()
{
  return -1;
}

QString XWTeXBox::getCurrentText()
{
  return QString();
}

double XWTeXBox::getDepth(double, int)
{
  return 0.0;
}

double XWTeXBox::getHeight(double, int)
{
  return 0.0;
}

QString XWTeXBox::getSelected()
{
  return QString();
}

QString XWTeXBox::getSelectedText()
{
  return QString();
}

QString XWTeXBox::getText()
{
  return QString();
}

double XWTeXBox::getWidth(double, int)
{
  return 0.0;
}

void XWTeXBox::goToEnd()
{}

bool XWTeXBox::goToNext()
{
  return false;
}

bool XWTeXBox::goToPrevious()
{
  return false;
}

void XWTeXBox::goToStart()
{}

bool XWTeXBox::hitTest(double ,double )
{
  return false;
}

bool XWTeXBox::insert(const QString & , QUndoCommand *)
{
  return false;
}

bool XWTeXBox::insertText(const QString & )
{
  return false;
}

bool XWTeXBox::keyInput(const QString &)
{
  return false;
}

bool XWTeXBox::newPar()
{
  return false;
}

bool XWTeXBox::paste()
{
  return false;
}

void XWTeXBox::push(QUndoCommand * )
{}

bool XWTeXBox::removeSelected(QUndoCommand * )
{
  return false;
}

void XWTeXBox::scan(const QString & , int & , int & )
{}


QString XWTeXBox::scanControlSequence(const QString & str, int & len, int & pos)
{
  pos++;
  if (pos >= len)
    return QString();

  int s = pos;
  if (!str[pos].isLetter())
    pos++;
  else
  {
    while (pos < len && str[pos].isLetter())
      pos++;
  }

  return str.mid(s,pos - s);
}

QString XWTeXBox::scanEnviromentName(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos] != QChar('{'))
    pos++;

  pos++;
  int s = pos;
  while (pos < len && str[pos] != QChar('}'))
    pos++;

  QString key = str.mid(s,pos - s);
  pos++;
  key = key.trimmed();
  return key;
}

void XWTeXBox::setContents(const QString & )
{}

void XWTeXBox::setFontSize(int)
{}

void XWTeXBox::setXY(int , double ,double , int)
{}

void XWTeXBox::skipComment(const QString & str, int & len, int & pos)
{
  if (str[pos] != QChar('%'))
    return ;

  pos++;
  while (pos < len)
  {
    if (str[pos] == QChar('\n'))
    {
      pos++;
      if (pos >= len)
        return ;

      if (str[pos] != QChar('%'))
        return ;
    }

    pos++;
  }
}
