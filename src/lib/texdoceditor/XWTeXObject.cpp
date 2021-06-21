/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include "TeXKeyWord.h"
#include "XWTeXPool.h"
#include "XWTeXDocument.h"
#include "XWTeXObject.h"

XWTeXDocumentObject * createTexObject(int t, XWTeXDocument * docA, QObject * parent)
{
  XWTeXDocumentObject * obj = 0;
  switch (t)
  {
    case TeXDef:
		case TeXEDef:
		case TeXGDef:
    case TeXHRule:
  	case TeXVRule:
    case TeXHBox:
  	case TeXVBox:
    case TeXHSkip:
  	case TeXKern:
  	case TeXMKern:
  	case TeXVSkip:
  	case TeXMoveLeft:
  	case TeXMoveRight:
  	case TeXRaise:
      obj = new XWTeXObject(t, docA,parent);
      break;

    default:
      break;
  }

  return obj;
}

XWTeXObject::XWTeXObject(int tA, XWTeXDocument * docA, QObject * parent)
 : XWTeXDocumentBlock(tA, docA,parent)
{}

void XWTeXObject::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setVerbFont();
  switch (type())
  {
    case TeXDef:
		case TeXEDef:
		case TeXGDef:
      newRow = true;
      break;

    default:
      break;
  }

  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      breakPageForTLT(curx,cury,firstcolumn);
      break;

    case TEX_DOC_WD_RTT:
      breakPageForRTT(curx,cury,firstcolumn);
      break;

    case TEX_DOC_WD_LTL:
      breakPageForLTL(curx,cury,firstcolumn);
      break;

    case TEX_DOC_WD_TRT:
      breakPageForTRT(curx,cury,firstcolumn);
      break;

    default:
      break;
  }
}

void XWTeXObject::scan(const QString & str, int & len, int & pos)
{
  switch (type())
  {
    case TeXDef:
		case TeXEDef:
		case TeXGDef:
      newRow = true;
      scanDef(str, len,pos);
      break;

    case TeXHRule:
  	case TeXVRule:
      scanRule(str, len,pos);
      break;

    case TeXHBox:
  	case TeXVBox:
      scanBox(str, len,pos);
      break;

    case TeXHSkip:
  	case TeXKern:
  	case TeXMKern:
  	case TeXVSkip:
  	case TeXMoveLeft:
  	case TeXMoveRight:
  	case TeXRaise:
      scanGlue(str, len,pos);
      break;

    default:
      break;
  }

  text = text.trimmed();
}

void XWTeXObject::write(QTextStream & strm, int & linelen)
{
  switch (type())
  {
    case TeXDef:
		case TeXEDef:
		case TeXGDef:
      strm << text;
      strm << "\n";
      linelen = 0;
      break;

    default:
      writeStr(text, strm, linelen);
      break;
  }
}

void XWTeXObject::scanBox(const QString & str, int & len, int & pos)
{
  int b = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('{'))
      b++;
    else if (str[pos] == QChar('}'))
    {
      b--;
      if (b == 0)
      {
        pos++;
        return ;
      }
    }

    text.append(str[pos]);
    pos++;
  }
}

void XWTeXObject::scanDef(const QString & str, int & len, int & pos)
{
  scanBox(str, len,pos);
}

void XWTeXObject::scanGlue(const QString & str, int & len, int & pos)
{
  while (pos < len)
  {
    if (str[pos].isLetter())
    {
      pos++;
      text.append(str[pos]);
      return ;
    }

    text.append(str[pos]);
    pos++;
  }
}

void XWTeXObject::scanRule(const QString & str, int & len, int & pos)
{
  while (pos < len)
  {
    if (str[pos].isLetter())
    {
      if (pos < (len - 1) && (str[pos] == QChar('b') && str[pos + 1] == QChar('p')) ||
          (str[pos] == QChar('c') && str[pos + 1] == QChar('c')) ||
          (str[pos] == QChar('c') && str[pos + 1] == QChar('m')) ||
          (str[pos] == QChar('d') && str[pos + 1] == QChar('d')) ||
          (str[pos] == QChar('e') && str[pos + 1] == QChar('m')) ||
          (str[pos] == QChar('e') && str[pos + 1] == QChar('x')) ||
          (str[pos] == QChar('i') && str[pos + 1] == QChar('n')) ||
          (str[pos] == QChar('m') && str[pos + 1] == QChar('m')) ||
          (str[pos] == QChar('p') && str[pos + 1] == QChar('c')) ||
          (str[pos] == QChar('p') && str[pos + 1] == QChar('t')) ||
          (str[pos] == QChar('s') && str[pos + 1] == QChar('p')))
      {
        pos++;
        text.append(str[pos]);
        return ;
      }
    }

    text.append(str[pos]);
    pos++;
  }
}
