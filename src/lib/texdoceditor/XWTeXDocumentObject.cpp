/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QByteArray>
#include <QApplication>
#include <QSettings>
#include <QTextDocument>
#include <QTextCursor>
#include <QFontDatabase>
#include "XWApplication.h"
#include "XWTeXDocument.h"
#include "XWTeXDocumentPage.h"
#include "XWTeXDocumentObject.h"

#define CJK_H_PERIOD 0x3002
#define CJK_V_PERIOD 0xFE12

#define CJK_H_FULLSTOP 0xFF0E
#define CJK_V_FULLSTOP 0xFF65

#define CJK_H_QUESTIONMARK 0xFF1F
#define CJK_V_QUESTIONMARK 0xFE16

#define CJK_H_EXCLAMATIONMARK 0xFF01
#define CJK_V_EXCLAMATIONMARK 0xFE15

#define CJK_H_COMMA 0xFF0C
#define CJK_V_COMMA 0xFE10

#define CJK_H_THECOMMA 0x3001
#define CJK_V_THECOMMA 0xFE11

#define CJK_H_SEMICOLON 0xFF1B
#define CJK_V_SEMICOLON 0xFE14

#define CJK_H_COLON 0xFF1A
#define CJK_V_COLON 0xFE13

#define CJK_H_LSQUOTION 0x2018
#define CJK_V_LSQUOTION 0xFE41

#define CJK_H_LSCBRACKET 0x300C
#define CJK_V_LSCBRACKET 0xFE41

#define CJK_H_RSQUOTION 0x2019
#define CJK_V_RSQUOTION 0xFE42

#define CJK_H_RSCBRACKET 0x300D
#define CJK_V_RSCBRACKET 0xFE42

#define CJK_H_LDQUOTION 0x201C
#define CJK_V_LDQUOTION 0xFE43

#define CJK_H_LWCBRACKET 0x300E
#define CJK_V_LWCBRACKET 0xFE43

#define CJK_H_RDQUOTION 0x201D
#define CJK_V_RDQUOTION 0xFE44

#define CJK_H_RWCBRACKET 0x300F
#define CJK_V_RWCBRACKET 0xFE44

#define CJK_H_LPARENTHESIS 0xFF08
#define CJK_V_LPARENTHESIS 0xFE35

#define CJK_H_RPARENTHESIS 0xFF09
#define CJK_V_RPARENTHESIS 0xFE36

#define CJK_H_LSBRACKET 0xFE5D
#define CJK_V_LSBRACKET 0xFE47

#define CJK_H_RSBRACKET 0xFE5E
#define CJK_V_RSBRACKET 0xFE48

#define CJK_H_LTSBRACKET 0x3014
#define CJK_V_LTSBRACKET 0xFE39

#define CJK_H_RTSBRACKET 0x3015
#define CJK_V_RTSBRACKET 0xFE3A

#define CJK_H_LCUBRACKET 0xFE5B
#define CJK_V_LCUBRACKET 0xFE37

#define CJK_H_RCUBRACKET 0xFE5C
#define CJK_V_RCUBRACKET 0xFE38

#define CJK_H_LBLCBRACKET 0x3010
#define CJK_V_LBLCBRACKET 0xFE3B

#define CJK_H_RBLCBRACKET 0x3011
#define CJK_V_RBLCBRACKET 0xFE3C

#define CJK_H_QUOTIONDASH 0x2015
#define CJK_V_QUOTIONDASH 0xFE31

#define CJK_H_ELLIPSIS 0x2026
#define CJK_V_ELLIPSIS 0xFE19

#define CJK_H_LDABRACKET 0x300A
#define CJK_V_LDABRACKET 0xFE3D

#define CJK_H_RDABRACKET 0x300B
#define CJK_V_RDABRACKET 0xFE3E

#define CJK_H_LABRACKET 0x3008
#define CJK_V_LABRACKET 0xFE3F

#define CJK_H_RABRACKET 0x3009
#define CJK_V_RABRACKET 0xFE40

#define CJK_H_WAVE 0xFE4B
#define CJK_V_WAVE 0xFE34

#define CJK_H_LWLCUBRACKET 0x3016
#define CJK_V_LWLCUBRACKET 0xFE17

#define CJK_H_RWLCUBRACKET 0x3017
#define CJK_V_RWLCUBRACKET 0xFE18

#define CJK_H_HYPHENCHAR 0x2013
#define CJK_V_HYPHENCHAR 0xFE32

struct UnicodeWritingSystem
{
  int left;
  int right;
  int ws;
};

static UnicodeWritingSystem unicodeWritingSystemTable[] = {
  {0x0080,0x00FF, QFontDatabase::Latin},
  {0x0370,0x03FF, QFontDatabase::Greek},
  {0x0400,0x052F, QFontDatabase::Cyrillic},
  {0x0530,0x058F, QFontDatabase::Armenian},
  {0x0590,0x05FF, QFontDatabase::Hebrew},
  {0x0600,0x06FF, QFontDatabase::Arabic},
  {0x0700,0x074F, QFontDatabase::Syriac},
  {0x0780,0x07BF, QFontDatabase::Thaana},
  {0x0900,0x097F, QFontDatabase::Devanagari},
  {0x0980,0x09FF, QFontDatabase::Bengali},
  {0x0A00,0x0A7F, QFontDatabase::Gurmukhi},
  {0x0A80,0x0AFF, QFontDatabase::Gujarati},
  {0x0B00,0x0B7F, QFontDatabase::Oriya},
  {0x0B80,0x0BFF, QFontDatabase::Tamil},
  {0x0C00,0x0C7F, QFontDatabase::Telugu},
  {0x0C80,0x0CFF, QFontDatabase::Kannada},
  {0x0D00,0x0D7F, QFontDatabase::Malayalam},
  {0x0D80,0x0DFF, QFontDatabase::Sinhala},
  {0x0E00,0x0E7F, QFontDatabase::Thai},
  {0x0E80,0x0EFF, QFontDatabase::Lao},
  {0x0F00,0x0FFF, QFontDatabase::Tibetan},
  {0x1000,0x109F, QFontDatabase::Myanmar},
  {0x10A0,0x10FF, QFontDatabase::Georgian},
  {0x1780,0x17FF, QFontDatabase::Khmer},
  {0x4E00,0x9FBF, QFontDatabase::SimplifiedChinese},
  {0xF900,0xFAFF, QFontDatabase::TraditionalChinese},
  {0x3040,0x30FF, QFontDatabase::Japanese},
  {0x31F0,0x31FF, QFontDatabase::Japanese},
  {0x1100,0x11FF, QFontDatabase::Korean},
  {0x3130,0x318F, QFontDatabase::Korean},
  {0xAC00,0xD7AF, QFontDatabase::Korean}
};

#define MAX_WS_IDX 32

QString getUnicodeFontFamily(const QChar & c)
{
  QString ret;
  QFontDatabase db;
  for (int i = 0; i < MAX_WS_IDX; i++)
  {
    if (c.unicode() >= unicodeWritingSystemTable[i].left &&
        c.unicode() <= unicodeWritingSystemTable[i].right)
    {
      QStringList  ls = db.families((QFontDatabase::WritingSystem)(unicodeWritingSystemTable[i].ws));
      if (ls.size() > 0)
        ret = ls[0];
    }
  }

  return ret;
}

XWTeXDocumentObject::XWTeXDocumentObject(int tA, XWTeXDocument * docA, QObject * parent)
 : QObject(parent),
   objectType(tA),
   doc(docA),
   indent(0.0),
   firstIndent(0.0),
   newPage(false),
   newRow(false),
   afterNewRow(false),
   isOnlyRead(false),
   dir(0),
   specialFont(true),
   head(0),
   last(0),
   next(0),
   prev(0)
{}

XWTeXDocumentObject::XWTeXDocumentObject(int tA, XWTeXDocument * docA, bool newrow,
                      QObject * parent)
: QObject(parent),
  objectType(tA),
  doc(docA),
  indent(0.0),
  firstIndent(0.0),
  newPage(false),
  newRow(newrow),
  afterNewRow(false),
  isOnlyRead(true),
  dir(0),
  specialFont(true),
  head(0),
  last(0),
  next(0),
  prev(0)
{}

XWTeXDocumentObject::XWTeXDocumentObject(int tA, XWTeXDocument * docA, bool newpage,
                      bool newrow, QObject * parent)
: QObject(parent),
  objectType(tA),
  doc(docA),
  indent(0.0),
  firstIndent(0.0),
  newPage(newpage),
  newRow(newrow),
  afterNewRow(false),
  isOnlyRead(true),
  dir(0),
  specialFont(true),
  head(0),
  last(0),
  next(0),
  prev(0)
{}

XWTeXDocumentObject::~XWTeXDocumentObject()
{}

void XWTeXDocumentObject::add(XWTeXDocumentObject*obj)
{
  if (!head)
  {
    head = obj;
    last = head;
    head->prev = 0;
    last->next = 0;
  }
  else
  {
    last->next = obj;
    obj->prev = last;
    last = obj;
    last->next = 0;
  }
}

void XWTeXDocumentObject::append(XWTeXDocumentObject * obj)
{
  obj->setParent(this);
  if (!head)
  {
    head = obj;
    last = head;
  }
  else
  {
    last->next = obj;
    obj->prev = last;
    last = obj;    
    if (obj->type() == TEX_DOC_B_TEXT && obj->prev->type() == TEX_DOC_B_TEXT)
    {
      XWTeXDocumentText * ltext = (XWTeXDocumentText*)(obj->prev);
      XWTeXDocumentText * ntext = (XWTeXDocumentText*)(obj);
      ntext->text.insert(0,ltext->text);
      XWTeXDocumentCursor * cor = doc->getCursor(ntext);
      cor->setHitPos(ltext->text.length());
      ntext->prev = ltext->prev;
      if (ltext->prev)
        ltext->prev->next = ntext;
      if (ltext == head)
        head = ntext;
    }
  }

  head->prev = 0;
  last->next = 0;  
}

void XWTeXDocumentObject::append(XWTeXDocumentObject * sobj,XWTeXDocumentObject * eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }
  
  if (!head)
  {
    head = sobj;
    last = eobj;
  }
  else
  {
    last->next = sobj;
    sobj->prev = last;
    last = eobj;    
    if (sobj->type() == TEX_DOC_B_TEXT && sobj->prev->type() == TEX_DOC_B_TEXT)
    {
      XWTeXDocumentText * ltext = (XWTeXDocumentText*)(sobj->prev);
      XWTeXDocumentText * ntext = (XWTeXDocumentText*)(sobj);
      ntext->text.insert(0,ltext->text);
      XWTeXDocumentCursor * cor = doc->getCursor(ntext);
      cor->setHitPos(ltext->text.length());
      ntext->prev = ltext->prev;
      if (ltext->prev)
        ltext->prev->next = ntext;
      if (ltext == head)
        head = ntext;
    }
  }

  head->prev = 0;
  last->next = 0;
}

void XWTeXDocumentObject::breakPage(const QFont & fontA,const QColor & c,
                      double & curx,double & cury, bool & firstcolumn)
{
  font = fontA;
  color = c;
  dir = doc->getDirection();
  bool newrow = false;
  XWTeXDocumentObject * obj = head;
  while (obj)
  {
    if (obj == head)
    {
      obj->firstIndent = firstIndent;
      obj->newPage = newPage;
      obj->newRow = newRow;
    }
    else
    {
      obj->firstIndent = 0;
      obj->newPage = false;
      obj->newRow = newrow;
    }

    obj->indent = indent;
    newrow = obj->afterNewRow;
    if (obj->hasSpecialFont())
      obj->breakPage(curx,cury, firstcolumn);
    else
      obj->breakPage(font, color, curx,cury, firstcolumn);

    obj = obj->next;
  }
}

void XWTeXDocumentObject::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setFont();
  bool newrow = false;
  XWTeXDocumentObject * obj = head;
  while (obj)
  {
    if (obj == head)
    {
      obj->firstIndent = firstIndent;
      obj->newPage = newPage;
      obj->newRow = newRow;
    }
    else
    {
      obj->firstIndent = 0;
      obj->newPage = false;
      obj->newRow = newrow;
    }

    obj->indent = indent;
    newrow = obj->afterNewRow;
    if (obj->hasSpecialFont())
      obj->breakPage(curx,cury, firstcolumn);
    else
      obj->breakPage(font, color, curx,cury, firstcolumn);

    obj = obj->next;
  }
}

void XWTeXDocumentObject::centered(const QFont & fontA,  const QColor & c,double & curx, double & cury, bool & firstcolumn)
{
  dir = doc->getDirection();
  font = fontA;
  color = c;
  double tl = 0;
  double l = 0;
  QFontMetricsF metrics(font);
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
    case TEX_DOC_WD_TRT:
      if (doc->isTwocolumn())
        tl = (doc->getRight() - doc->getLeft() - doc->getColumSep()) / 2;
      else
        tl = doc->getRight() - doc->getLeft();
      l = width();
      indent = tl - l;
      {
        int n = 1;
        while (indent < 0.0)
        {
          n++;
          indent = tl - l / n;
        }
      }
      indent /= 2;
      indent /= metrics.width(QChar('M'));
      break;

    default:
      if (doc->isTwocolumn())
        tl = (doc->getBottom() - doc->getTop() - doc->getColumSep()) / 2;
      else
        tl = doc->getBottom() - doc->getTop();
      l = height();
      indent = tl - l;
      {
        int n = 1;
        while (indent < 0.0)
        {
          n++;
          indent = tl - l / n;
        }
      }
      indent /= 2;
      indent /= metrics.height();
      break;
  }

  breakPage(font, color,curx,cury,firstcolumn);
}

void XWTeXDocumentObject::centered(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setFont();
  double tl = 0;
  double l = 0;
  QFontMetricsF metrics(font);
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
    case TEX_DOC_WD_TRT:
      if (doc->isTwocolumn())
        tl = (doc->getRight() - doc->getLeft() - doc->getColumSep()) / 2;
      else
        tl = doc->getRight() - doc->getLeft();
      l = width();
      indent = tl - l;
      {
        int n = 1;
        while (indent < 0.0)
        {
          n++;
          indent = tl - l / n;
        }
      }
      indent /= 2;
      indent /= metrics.width(QChar('M'));
      break;

    default:
      if (doc->isTwocolumn())
        tl = (doc->getBottom() - doc->getTop() - doc->getColumSep()) / 2;
      else
        tl = doc->getBottom() - doc->getTop();
      l = height();
      indent = tl - l;
      {
        int n = 1;
        while (indent < 0.0)
        {
          n++;
          indent = tl - l / n;
        }
      }
      indent /= 2;
      indent /= metrics.height();
      break;
  }

  breakPage(curx,cury,firstcolumn);
}

void XWTeXDocumentObject::clear()
{
  XWTeXDocumentObject * obj = head;
  while (obj)
  {
    XWTeXDocumentObject * tmp = obj->next;
    delete obj; 
    obj = tmp;
  }
}

void XWTeXDocumentObject::del(XWTeXDocumentObject*obj)
{
  if (!obj)
    return ;

  if (obj->prev)
    obj->prev->next = obj->next;
  else
    head = obj->next;

  if (obj->next)
    obj->next->prev = obj->prev;
  else
    last = obj->prev;

  delete obj;
}

void XWTeXDocumentObject::flushBottom(const QFont & fontA,const QColor & c,
                          double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  font = fontA;
  color = c;
  XWTeXDocumentObject * obj = head;
  while (obj)
  {
    if (obj == head)
    {
      obj->firstIndent = firstIndent;
      obj->newPage = newPage;
      obj->newRow = newRow;
    }
    else
    {
      obj->firstIndent = 0;
      obj->newPage = false;
      obj->newRow = false;
    }

    obj->indent = indent;
    if (obj->hasSpecialFont())
      obj->flushBottom(curx,cury,firstcolumn);
    else
      obj->flushBottom(font, color, curx,cury,firstcolumn);
    obj = obj->next;
  }
}

void XWTeXDocumentObject::flushBottom(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setFont();
  XWTeXDocumentObject * obj = head;
  while (obj)
  {
    if (obj == head)
    {
      obj->firstIndent = firstIndent;
      obj->newPage = newPage;
      obj->newRow = newRow;
    }
    else
    {
      obj->firstIndent = 0;
      obj->newPage = false;
      obj->newRow = false;
    }

    obj->indent = indent;
    if (obj->hasSpecialFont())
      obj->flushBottom(curx,cury,firstcolumn);
    else
      obj->flushBottom(font, color, curx,cury,firstcolumn);
    obj = obj->next;
  }
}

QFont XWTeXDocumentObject::getFont()
{
  return font;
}

QString XWTeXDocumentObject::getMimeData()
{
  QString str;
  XWTeXDocumentObject * obj = head;
  while (obj)
  {
    QString tmp = obj->getMimeData();
    str += tmp;
    obj = obj->next;
  }

  return str;
}

QString XWTeXDocumentObject::getRomanNumber(int n)
{
  static char str[] = "m2d5c2l5x2v5i";
	int v = 1000;
	char len = (char)(strlen(str));
	char j = 0;
  QString ret;
	while (j < len)
	{
		while (n >= v)
		{
			ret.append(QChar(str[j]));
			n = n - v;
		}

		if (n <= 0)
			return ret;

		char k = j + 2;
		int u = v / (str[k - 1] - '0');
		if (str[k - 1] == '2')
		{
			k = k + 2;
			u = u / (str[k - 1] - '0');
		}

		if ((n + u) >= v)
		{
			ret.append(QChar(str[k]));
			n = n + u;
		}
		else
		{
			j = j + 2;
			v = v / (str[j - 1] - '0');
		}
	}

  return ret;
}

QString XWTeXDocumentObject::getSelected()
{
  QString str;
  if (isAllSelected())
    str = getMimeData();
  else
  {
    XWTeXDocumentObject * obj = head;
    while (obj)
    {
      QString tmp = obj->getSelected();
      str += tmp;
      obj = obj->next;
    }
  }

  return str;
}

bool XWTeXDocumentObject::hasSelected()
{
  XWTeXDocumentObject * obj = head->next;
  while (obj)
  {
    if (obj->hasSelected())
      return true;
    obj = obj->next;
  }
  return false;
}

double XWTeXDocumentObject::height()
{
  double h = 0;
  XWTeXDocumentObject * obj = head;
  while (obj)
  {
    if (!obj->newRow)
    {
      double tmph = obj->height();
      if (tmph > h)
       h = tmph;
    }
    else
      h += obj->height();
    obj = obj->next;
  }

  return h;
}

void XWTeXDocumentObject::insert(XWTeXDocumentObject * obj)
{
  if (isAtStart())
  {
    obj->prev = prev;
    if (prev)
      prev->next = obj;
    prev = obj;
    obj->next = this;
    if (parent() != doc)
    {
      XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
      if (this == pobj->head)
      {
        pobj->head = obj;
        obj->prev = 0;
      }
    }
  }
  else
  {
    obj->next = next;    
    obj->prev = this;
    if (next)
      next->prev = obj;
    next = obj;
    if (parent() != doc)
    {
      XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
      if (this == pobj->last)
      {
        pobj->last = obj;
        obj->next = 0;
      }        
    }
  }  
  obj->setParent(parent());
}

void XWTeXDocumentObject::insertAtAfter(XWTeXDocumentObject * obj)
{
  obj->next = next;
  if (next)
    next->prev = obj;

  obj->prev = this;
  next = obj;
  if (parent() != doc)
  {
    XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
    if (this == pobj->last)
    {
      pobj->last = obj;
      obj->next = 0;
    }    
  }
  obj->setParent(parent());
}

void XWTeXDocumentObject::insertAtBefore(XWTeXDocumentObject * obj)
{
  if (!prev)
  {
    obj->next = this;
    prev = obj;
    obj->prev = 0;
    if (parent() != doc)
    {
      XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
      pobj->head = obj;
    }
  }
  else
  {
    prev->next = obj;
    obj->prev = prev;
    obj->next = this;
    prev = obj;
  }

  obj->setParent(parent());
}

void XWTeXDocumentObject::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  head = sobj;
  head->prev = 0;
  last = eobj;
  last->next = 0;
}

void XWTeXDocumentObject::insertChildren(XWTeXDocumentObject*obj, 
                                         XWTeXDocumentObject*sobj, 
                                         XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  if (!obj)
  {
    if (!head)
    {
      head = sobj;
      head->prev = 0;
      last = eobj;
      last->next = 0;
    }
    else
    {
      eobj->next = head;
      head->prev = eobj;
      head = sobj;
      head->prev = 0;
    }
  }
  else if (obj->type() == TEX_DOC_B_TEXT && !obj->isAtStart() && !obj->isAtEnd())
  {
    XWTeXDocumentText * ltext = (XWTeXDocumentText*)(obj);
    XWTeXDocumentText * ntext = new XWTeXDocumentText(doc,this);
    XWTeXDocumentCursor * cor = doc->getCursor(ltext);
    int pos = cor->getHitPos();
    ntext->text = ltext->text.mid(pos, -1);
    ltext->text.remove(pos,ntext->text.length());
    ltext->next = sobj;
    sobj->prev = ltext;
    ntext->prev = eobj;
    eobj->next = ntext;
    if (obj == last)
    {
      last = ntext;
      last->next = 0;
    }      
  }
  else if (obj == last)
  {
    obj->next = sobj;
    sobj->prev = obj;
    last = eobj;
    last->next = 0;
  }
  else
  {
    sobj->prev = obj;
    eobj->next = obj->next;
    obj->next->prev = eobj;    
    obj->next = sobj;
  }

  if (sobj->type() == TEX_DOC_B_TEXT)
  {
    if (sobj->prev && sobj->prev->type() == TEX_DOC_B_TEXT)
    {
      XWTeXDocumentText * ltext = (XWTeXDocumentText*)(sobj->prev);
      XWTeXDocumentText * ntext = (XWTeXDocumentText*)(sobj);
      ntext->text.insert(0,ltext->text);
      XWTeXDocumentCursor * cor = doc->getCursor(ntext);
      cor->setHitPos(ltext->text.length());
      ntext->prev = ltext->prev;
      if (ltext->prev)
        ltext->prev->next = ntext;
      else
      {
        head = ntext;
        ntext->prev = 0;
      }      
    }
  }

  if (eobj->type() == TEX_DOC_B_TEXT)
  {
    if (eobj->next && eobj->next->type() == TEX_DOC_B_TEXT)
    {
      XWTeXDocumentText * ltext = (XWTeXDocumentText*)(eobj);
      XWTeXDocumentCursor * cor = doc->getCursor(ltext);
      cor->setHitPos(ltext->text.length());
      XWTeXDocumentText * ntext = (XWTeXDocumentText*)(eobj->next);
      ltext->text.append(ntext->text);
      eobj->next = eobj->next->next;
      if (eobj->next->next)
        eobj->next->next->prev = eobj;
      else
      {
        last = eobj;
        last->next = 0;
      }        
    }
  }
}

bool XWTeXDocumentObject::isAllSelected()
{
  if (!head || !last)
    return true;

  return head->isAllSelected() && last->isAllSelected();
}

bool XWTeXDocumentObject::isEmpty()
{
  if (!head)
    return true;

  XWTeXDocumentObject * obj = head;
  while (obj)
  {
    if (!obj->isEmpty())
      return false;

    obj = obj->next;
  }

  return true;
}

bool XWTeXDocumentObject::isAtEnd()
{
  if (last)
  {
    if (last->isBlock())
    {
      XWTeXDocumentObject * block = doc->getCurrentBlock();
      return (block == last && last->isAtEnd());
    }

    return last->isAtEnd();
  }    

  return true;
}

bool XWTeXDocumentObject::isAtStart()
{
  if (head)
  {
    if (head->isBlock())
    {
      XWTeXDocumentObject * block = doc->getCurrentBlock();
      return (block == head && head->isAtStart());
    }

    return head->isAtStart();
  }    

  return false;
}

bool XWTeXDocumentObject::isTopLevel()
{
  return (parent() == doc);
}

void XWTeXDocumentObject::remove()
{
  if (prev && prev->type() == TEX_DOC_B_TEXT && next && next->type() == TEX_DOC_B_TEXT)
  {
    prev->next = next->next;
    if (next->next)
      next->next->prev = prev;

    XWTeXDocumentText * prevtext = (XWTeXDocumentText*)(prev);
    XWTeXDocumentText * nexttext = (XWTeXDocumentText*)(next);
    prevtext->text.append(nexttext->text);
    if (parent() != doc)
    {
      XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
      if (next == pobj->last)
      {
        pobj->last = prev;
        if (prev)
          prev->next = 0;
      }
    }
  }
  else
  {
    if (prev)
      prev->next = next;
    else if (parent() != doc)
    {
      XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
      pobj->head = next;
      if (next)
        next->prev = 0;
    }

    if (next)
      next->prev = prev;
    else if (parent() != doc)
    {
      XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
      pobj->last = prev;
      if (prev)
        prev->next = 0;
    }  
  }
}

void XWTeXDocumentObject::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head;
  *eobj = last;
  head = 0;
  last = 0;
}

void XWTeXDocumentObject::removeChildren(XWTeXDocumentObject**sobj, 
                                           XWTeXDocumentObject**eobj,
                                           XWTeXDocumentObject**obj)
{
  if (*sobj == *eobj)
  {
    *obj = (*sobj)->prev;
    (*sobj)->remove();
    if ((*sobj) == head)
    {
      head = (*sobj)->next;
      if (head)
        head->prev = 0;
    }      

    if ((*sobj) == last)
    {
      last = (*sobj)->prev;
      if (last)
        last->next = 0;
    } 
  }
  else
  {
    XWTeXDocumentObject * prevobj = (*sobj)->prev;
    *obj = prevobj;
    XWTeXDocumentObject * nextobj = (*eobj)->next;
    if ((*sobj)->type() == TEX_DOC_B_TEXT && !(*sobj)->isAtStart() && !(*sobj)->isAtEnd())
    {
      XWTeXDocumentText * sltext = (XWTeXDocumentText*)(*sobj);
      XWTeXDocumentText * sntext = new XWTeXDocumentText(doc,this); 
      sntext->prev = sltext;      
      sntext->next = sltext->next;    
      if (sltext->next)
        sltext->next->prev = sntext;
      sltext->next = sntext;
      XWTeXDocumentCursor * cor = doc->getCursor(sltext);
      int pos = cor->getHitPos();
      sntext->text = sltext->text.mid(pos, -1);
      sltext->text.remove(pos,sntext->text.length());
      prevobj = sltext;
      *sobj = sntext;
      *obj = sltext;
    }

    if ((*eobj)->type() == TEX_DOC_B_TEXT && !(*eobj)->isAtStart() && !(*eobj)->isAtEnd())
    {
      XWTeXDocumentText * eltext = (XWTeXDocumentText*)(*eobj);
      XWTeXDocumentText * entext = new XWTeXDocumentText(doc,this);
      entext->prev = eltext;
      entext->next = eltext->next;
      if (eltext->next)
        eltext->next->prev = entext;
      eltext->next = entext;
      nextobj = entext;
      XWTeXDocumentCursor * cor = doc->getCursor(eltext);
      int pos = cor->getHitPos();
      entext->text = eltext->text.mid(pos, -1);
      eltext->text.remove(pos,entext->text.length());
      *eobj = eltext;
    }

    if (prevobj)
      prevobj->next = nextobj;
    else
    {
      head = nextobj;
      if (nextobj)
        nextobj->prev = 0;
    }      

    if (nextobj)
      nextobj->prev = prevobj;
    else
    {
      last = prevobj;
      if (prevobj)
        prevobj->next = 0;
    }      
  }
}

void XWTeXDocumentObject::removeChildrenFrom(XWTeXDocumentObject**sobj, 
                                             XWTeXDocumentObject**eobj,
                                             XWTeXDocumentObject**obj)
{
  *eobj = last;
  XWTeXDocumentObject * nextobj = (*eobj)->next;
  if ((*sobj)->type() == TEX_DOC_B_TEXT && !(*sobj)->isAtStart() && !(*sobj)->isAtEnd())
  {
    XWTeXDocumentText * ltext = (XWTeXDocumentText*)(*sobj);
    XWTeXDocumentText * ntext = new XWTeXDocumentText(doc,this);
    ntext->prev = ltext;      
    ntext->next = ltext->next;    
    if (ltext->next)
      ltext->next->prev = ntext;
    ltext->next = ntext;
    XWTeXDocumentCursor * cor = doc->getCursor(ltext);
    int pos = cor->getHitPos();
    ntext->text = ltext->text.mid(pos, -1);
    ltext->text.remove(pos,ntext->text.length());
    *sobj = ntext;
  }

  XWTeXDocumentObject * prevobj = (*sobj)->prev;
  *obj = prevobj;
  if (prevobj)
    prevobj->next = nextobj;
  else
  {
    head = nextobj;
    if (head)
      head->prev = 0;
  }
    
  if (nextobj)
    nextobj->prev = prevobj;
  else
  {
    last = prevobj;
    if (last)
      last->next = 0;
  }    
}

void XWTeXDocumentObject::removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head;
  if ((*eobj)->type() == TEX_DOC_B_TEXT && !(*eobj)->isAtStart() && !(*eobj)->isAtEnd())
  {
    XWTeXDocumentText * ltext = (XWTeXDocumentText*)(*eobj);
    XWTeXDocumentText * ntext = new XWTeXDocumentText(doc,this);
    ntext->prev = ltext;      
    ntext->next = ltext->next;    
    if (ltext->next)
      ltext->next->prev = ntext;
    ltext->next = ntext;
    XWTeXDocumentCursor * cor = doc->getCursor(ltext);
    int pos = cor->getHitPos();
    ntext->text = ltext->text.mid(pos, -1);
    ltext->text.remove(pos,ntext->text.length());
    *eobj = ltext;
  }

  head = (*eobj)->next;
  if (head)
    head->prev = 0;
}

void XWTeXDocumentObject::scan(const QString & str, int & len, int & pos)
{
  if (!head)
    return ;

  XWTeXDocumentObject * obj = head->next;
  while (obj)
  {
    obj->scan(str,len, pos);
    obj = obj->next;
  }
}

QString XWTeXDocumentObject::scanControlSequence(const QString & str, int & len,int & pos)
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

QString XWTeXDocumentObject::scanEnviromentName(const QString & str, int & len, int & pos)
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

void XWTeXDocumentObject::setBoldFont()
{
  setFont("Bold");
}

void  XWTeXDocumentObject::setCommentFont()
{
  setFont("Comment");
}

void  XWTeXDocumentObject::setControlSequenceFont()
{
  setFont("ControlSequence");
}

void XWTeXDocumentObject::setCursor()
{
  if (last)
    last->setCursor();
}

void XWTeXDocumentObject::setCursorAtEnd()
{
  if (last)
    last->setCursorAtEnd();
}

void XWTeXDocumentObject::setCursorAtStart()
{
  if (head)
    head->setCursorAtStart();
}

void XWTeXDocumentObject::setFont()
{
  setNormalFont();
}

void XWTeXDocumentObject::setFont(const QString & group)
{
  QSettings * settings = xwApp->getSettings();
  QString curstyle = doc->getStyle();
  settings->beginGroup("DocumentStyles");
  if (!curstyle.isEmpty())
  {
    settings->beginGroup(curstyle);
    settings->beginGroup(group);
    QString v = settings->value("Family").toString();
    v = v.simplified();
    if (!v.isEmpty())
      font.setFamily(v);

    QVariant variant = settings->value("Color");
    color = variant.value<QColor>();

    settings->endGroup();
    settings->endGroup();
  }

  settings->endGroup();
  settings->endGroup();

  font.setPointSize(doc->getCurrentFontSize());
  font.setItalic(doc->getItalic());
  font.setWeight(doc->getWeight());
}

void XWTeXDocumentObject::setItalicFont()
{
  setFont("Italic");
}

void XWTeXDocumentObject::setMathFont()
{
  setFont("Math");
}

void XWTeXDocumentObject::setNormalFont()
{
  setFont("Normal");
}

void  XWTeXDocumentObject::setOptionFont()
{
  setFont("Option");
}

void  XWTeXDocumentObject::setParamFont()
{
  setFont("Parameter");
}

void XWTeXDocumentObject::setVerbFont()
{
  setFont("Typewriter");
}

void XWTeXDocumentObject::skipCoord(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos >= len || str[pos] != QChar('('))
    return;

  pos++;
  int b = 0;
  while (pos < len)
  {
    if (str[pos] == QChar(')'))
    {
      b--;
      if (b < 0)
      {
        pos++;
        return;
      }
    }
    else if (str[pos] == QChar('('))
      b++;

    pos++;
  }
}

void XWTeXDocumentObject::skipGroup(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos >= len || str[pos] != QChar('{'))
    return;

  pos++;
  int b = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('}'))
    {
      b--;
      if (b < 0)
      {
        pos++;
        return;
      }
    }
    else if (str[pos] == QChar('{'))
      b++;

    pos++;
  }
}

void XWTeXDocumentObject::skipOption(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos >= len || str[pos] != QChar('['))
    return;

  pos++;
  int b = 0;
  while (pos < len)
  {
    if (str[pos] == QChar(']'))
    {
      b--;
      if (b < 0)
      {
        pos++;
        return;
      }
    }
    else if (str[pos] == QChar('['))
      b++;

    pos++;
  }
}

double XWTeXDocumentObject::width()
{
  double w = 0;
  XWTeXDocumentObject * obj = head;
  while (obj)
  {
    if (obj->newRow)
    {
      double tmpw = obj->width();
      if (tmpw > w)
        w = tmpw;
    }
    else
      w += obj->width();
    obj = obj->next;
  }

  return w;
}

void XWTeXDocumentObject::write(QTextStream & strm, int & linelen)
{
  XWTeXDocumentObject * obj = head;
  if (head)
    head->newRow = newRow;

  while (obj)
  {
    obj->write(strm, linelen);
    obj = obj->next;
  }

  if (linelen != 0 && afterNewRow)
  {
    strm << "\n";
    linelen = 0;
  }
}

XWTeXDocumentRow * XWTeXDocumentObject::getRowForTRT(const QFontMetricsF & metrics,
                                bool reset, double & curx, double & cury,bool & firstcolumn)
{
  double aw = metrics.width(QChar('M'));
  double h = metrics.height();
  double skip = doc->getLineSkip() * h;
  XWTeXDocumentRow * row = 0;
  bool newpage = newPage && !reset;
  bool newrow = newRow;
  if (doc->isTwocolumn())
  {
    double halfX = doc->getRight() - (doc->getRight() - doc->getLeft() - doc->getColumSep()) / 2;
    double halfXX = halfX - doc->getColumSep();
    if (firstcolumn)
    {
      if (newpage)
      {
        firstcolumn = true;
        XWTeXDocumentPage * page = doc->getNewPage();
        row = page->getNewRow();
        row->setDirection(TEX_DOC_WD_TRT);
        cury = doc->getTop();
        if (reset)
          curx = doc->getRight() - indent * aw;
        else
          curx = doc->getRight() - (indent + firstIndent) * aw;

        row->setMinX(halfX + indent * aw);
        row->setMaxX(curx);
        row->setMinY(cury);
        row->setMaxY(cury + h);
        row->setLimit(halfX + indent * aw);
        row->setMaxLength((doc->getRight() - doc->getLeft() - doc->getColumSep()) / 2 - 2 * aw * indent);
      }
      else
      {
        XWTeXDocumentPage * page = doc->getCurrentPage();
        if ((newrow || curx <= (halfX + indent * aw)) && ((cury + skip) >= doc->getBottom()))
        {
          firstcolumn = false;
          row = page->getNewRow();
          row->setDirection(TEX_DOC_WD_TRT);
          cury = doc->getTop();
          if (reset)
            curx = halfXX - indent * aw;
          else
            curx = halfXX - (indent + firstIndent) * aw;

          row->setMinX(doc->getLeft() + indent * aw);
          row->setMaxX(curx);
          row->setMinY(cury);
          row->setMaxY(cury + h);
          row->setLimit(doc->getLeft() + indent * aw);
          row->setMaxLength((doc->getRight() - doc->getLeft() - doc->getColumSep()) / 2 - 2 * aw * indent);
        }
        else
        {
          if (curx <= (halfX + indent * aw))
            newrow = true;

          if (newrow)
          {
            row = page->getNewRow();
            row->setDirection(TEX_DOC_WD_TRT);
            cury += skip;
            if (reset)
              curx = doc->getRight() - indent * aw;
            else
              curx = doc->getRight() - (indent + firstIndent) * aw;
            row->setMinX(halfX + indent * aw);
            row->setMaxX(curx);
            row->setMinY(cury);
            row->setMaxY(cury + h);
            row->setLimit(halfX + indent * aw);
            row->setMaxLength((doc->getRight() - doc->getLeft() - doc->getColumSep()) / 2 - 2 * aw * indent);
          }
          else
          {
            row = page->getCurrentRow();
            if (h > (row->getMaxY() - row->getMinY()))
            {
              cury += h - (row->getMaxY() - row->getMinY());
              row->setMinY(cury);
              row->setMaxY(cury + h);
            }
          }
        }
      }
    }
    else
    {
      if ((newrow || (curx <= (doc->getLeft() + indent * aw))) &&
          ((cury + skip) >= doc->getBottom()))
        newpage = true;

      if (newpage)
      {
        firstcolumn = true;
        XWTeXDocumentPage * page = doc->getNewPage();
        row = page->getNewRow();
        row->setDirection(TEX_DOC_WD_TRT);
        cury = doc->getTop();
        if (reset)
          curx = doc->getRight() - indent * aw;
        else
          curx = doc->getRight() - (indent + firstIndent) * aw;

        row->setMinX(halfX + indent * aw);
        row->setMaxX(curx);
        row->setMinY(cury);
        row->setMaxY(cury + h);
        row->setLimit(halfX + indent * aw);
        row->setMaxLength((doc->getRight() - doc->getLeft() - doc->getColumSep()) / 2 - 2 * aw * indent);
      }
      else
      {
        if (curx <= (doc->getLeft() + indent * aw))
          newrow = true;

        XWTeXDocumentPage * page = doc->getCurrentPage();
        if (newrow)
        {
          row = page->getNewRow();
          row->setDirection(TEX_DOC_WD_TRT);
          cury += skip;
          if (reset)
            curx = halfXX - indent * aw;
          else
            curx = halfXX - (indent + firstIndent) * aw;
          row->setMinX(doc->getLeft() + indent * aw);
          row->setMaxX(curx);
          row->setMinY(cury);
          row->setMaxY(cury + h);
          row->setLimit(doc->getLeft() + indent * aw);
          row->setMaxLength((doc->getRight() - doc->getLeft() - doc->getColumSep()) / 2 - 2 * aw * indent);
        }
        else
        {
          row = page->getCurrentRow();
          if (h > (row->getMaxY() - row->getMinY()))
          {
            cury += h - (row->getMaxY() - row->getMinY());
            row->setMinY(cury);
            row->setMaxY(cury + h);
          }
        }
      }
    }
  }
  else
  {
    if ((newrow || (curx <= (doc->getLeft() + indent * aw))) &&
        ((cury + skip) >= doc->getBottom()))
      newpage = true;

    if (newpage)
    {
      XWTeXDocumentPage * page = doc->getNewPage();
      row = page->getNewRow();
      row->setDirection(TEX_DOC_WD_TRT);
      cury = doc->getTop();
      if (reset)
        curx = doc->getRight() - indent * aw;
      else
        curx = doc->getRight() - (indent + firstIndent) * aw;
      row->setMinX(doc->getLeft() + indent * aw);
      row->setMaxX(curx);
      row->setMinY(cury);
      row->setMaxY(cury + h);
      row->setLimit(doc->getLeft() + indent * aw);
      row->setMaxLength(doc->getRight() - doc->getLeft() - 2 * aw * indent);
    }
    else
    {
      if (curx <= (doc->getLeft() + indent * aw))
        newrow = true;

      XWTeXDocumentPage * page = doc->getCurrentPage();
      if (newrow)
      {
        row = page->getNewRow();
        row->setDirection(TEX_DOC_WD_TRT);
        cury += skip;
        if (reset)
          curx = doc->getRight() - indent * aw;
        else
          curx = doc->getRight() - (indent + firstIndent) * aw;
        row->setMinX(doc->getLeft() + indent * aw);
        row->setMaxX(curx);
        row->setMinY(cury);
        row->setMaxY(cury + h);
        row->setLimit(doc->getLeft() + indent * aw);
        row->setMaxLength(doc->getRight() - doc->getLeft() - 2 * aw * indent);
      }
      else
      {
        row = page->getCurrentRow();
        if (h > (row->getMaxY() - row->getMinY()))
        {
          cury += h - (row->getMaxY() - row->getMinY());
          row->setMinY(cury);
          row->setMaxY(cury + h);
        }
      }
    }
  }

  return row;
}

XWTeXDocumentRow * XWTeXDocumentObject::getRowForTLT(const QFontMetricsF & metrics,
                                bool reset, double & curx, double & cury, bool & firstcolumn)
{
  double h = metrics.height();
  double skip = h * doc->getLineSkip();
  double aw = metrics.width(QChar('M'));
  XWTeXDocumentRow * row = 0;
  bool newpage = newPage && !reset;
  bool newrow = newRow;
  if (doc->isTwocolumn())
  {
    double halfX = doc->getLeft() + (doc->getRight() - doc->getLeft() - doc->getColumSep()) / 2;
    double halfXX = halfX + doc->getColumSep();
    if (firstcolumn)
    {
      if (newpage)
      {
        firstcolumn = true;
        XWTeXDocumentPage * page = doc->getNewPage();
        row = page->getNewRow();
        row->setDirection(TEX_DOC_WD_TLT);

        cury = doc->getTop();
        if (reset)
          curx = doc->getLeft() + indent * aw;
        else
          curx = doc->getLeft() + (indent + firstIndent) * aw;

        row->setMinX(curx);
        row->setMaxX(halfX - indent * aw);
        row->setMinY(cury);
        row->setMaxY(cury + h);
        row->setLimit(halfX - indent * aw);
        row->setMaxLength((doc->getRight() - doc->getLeft() - doc->getColumSep()) / 2 - 2 * aw * indent);
      }
      else
      {
        XWTeXDocumentPage * page = doc->getCurrentPage();
        if ((newrow || curx >= (halfX - indent * aw)) && ((cury + skip) >= doc->getBottom()))
        {
          firstcolumn = false;
          row = page->getNewRow();
          row->setDirection(TEX_DOC_WD_TLT);
          cury = doc->getTop();
          if (reset)
            curx = halfXX + indent * aw;
          else
            curx = halfXX + (indent + firstIndent) * aw;

          row->setMinX(curx);
          row->setMaxX(doc->getRight() - indent * aw);
          row->setMinY(cury);
          row->setMaxY(cury + h);
          row->setLimit(doc->getRight() - indent * aw);
          row->setMaxLength((doc->getRight() - doc->getLeft() - doc->getColumSep()) / 2 - 2 * aw * indent);
        }
        else
        {
          if (curx >= (halfX - indent * aw))
            newrow = true;

          if (newRow)
          {
            row = page->getNewRow();
            row->setDirection(TEX_DOC_WD_TLT);
            cury += skip;
            if (reset)
              curx = doc->getLeft() + indent * aw;
            else
              curx = doc->getLeft() + (indent + firstIndent) * aw;
            row->setMinX(curx);
            row->setMaxX(halfX - indent * aw);
            row->setMinY(cury);
            row->setMaxY(cury + h);
            row->setLimit(halfX - indent * aw);
            row->setMaxLength((doc->getRight() - doc->getLeft() - doc->getColumSep()) / 2 - 2 * aw * indent);
          }
          else
          {
            row = page->getCurrentRow();
            if (h > (row->getMaxY() - row->getMinY()))
            {
              cury += h - (row->getMaxY() - row->getMinY());
              row->setMinY(cury);
              row->setMaxY(cury + h);
            }
          }
        }
      }
    }
    else
    {
      if ((newrow || (curx >= (doc->getRight() - indent * aw))) &&
          ((cury + skip) >= doc->getBottom()))
        newpage = true;

      if (newpage)
      {
        firstcolumn = true;
        XWTeXDocumentPage * page = doc->getNewPage();
        row = page->getNewRow();
        row->setDirection(TEX_DOC_WD_TLT);
        cury = doc->getTop();
        if (reset)
          curx = doc->getLeft() + indent * aw;
        else
          curx = doc->getLeft() + (indent + firstIndent) * aw;

        row->setMinX(curx);
        row->setMaxX(halfX - indent * aw);
        row->setMinY(cury);
        row->setMaxY(cury + h);
        row->setLimit(halfX - indent * aw);
        row->setMaxLength((doc->getRight() - doc->getLeft() - doc->getColumSep()) / 2 - 2 * aw * indent);
      }
      else
      {
        if (curx >= (doc->getRight() - indent * aw))
          newrow = true;

        XWTeXDocumentPage * page = doc->getCurrentPage();
        if (newrow)
        {
          row = page->getNewRow();
          row->setDirection(TEX_DOC_WD_TLT);
          cury += skip;
          if (reset)
            curx = halfXX + indent * aw;
          else
            curx = halfXX + (indent + firstIndent) * aw;
          row->setMinX(curx);
          row->setMaxX(doc->getRight() - indent * aw);
          row->setMinY(cury);
          row->setMaxY(cury + h);
          row->setLimit(doc->getRight() - indent * aw);
          row->setMaxLength((doc->getRight() - doc->getLeft() - doc->getColumSep()) / 2 - 2 * aw * indent);
        }
        else
        {
          row = page->getCurrentRow();
          if (h > (row->getMaxY() - row->getMinY()))
          {
            cury += h - (row->getMaxY() - row->getMinY());
            row->setMinY(cury);
            row->setMaxY(cury + h);
          }
        }
      }
    }
  }
  else
  {
    if ((newrow || (curx >= (doc->getRight() - indent * aw))) &&
        ((cury + skip) >= doc->getBottom()))
      newpage = true;

    if (newpage)
    {
      XWTeXDocumentPage * page = doc->getNewPage();
      row = page->getNewRow();
      row->setDirection(TEX_DOC_WD_TLT);
      cury = doc->getTop();
      if (reset)
        curx = doc->getLeft() + indent * aw;
      else
        curx = doc->getLeft() + (indent + firstIndent) * aw;
      row->setMinX(curx);
      row->setMaxX(doc->getRight() - indent * aw);
      row->setMinY(cury);
      row->setMaxY(cury + h);
      row->setLimit(doc->getRight() - indent * aw);
      row->setMaxLength(doc->getRight() - doc->getLeft() - 2 * aw * indent);
    }
    else
    {
      if (curx >= (doc->getRight() - indent * aw))
        newrow = true;

      XWTeXDocumentPage * page = doc->getCurrentPage();
      if (newrow)
      {
        row = page->getNewRow();
        row->setDirection(TEX_DOC_WD_TLT);
        cury += skip;
        if (reset)
          curx = doc->getLeft() + indent * aw;
        else
          curx = doc->getLeft() + (indent + firstIndent) * aw;
        row->setMinX(curx);
        row->setMaxX(doc->getRight() - indent * aw);
        row->setMinY(cury);
        row->setMaxY(cury + h);
        row->setLimit(doc->getRight() - indent * aw);
        row->setMaxLength(doc->getRight() - doc->getLeft() - 2 * aw * indent);
      }
      else
      {
        row = page->getCurrentRow();
        if (h > (row->getMaxY() - row->getMinY()))
        {
          cury += h - (row->getMaxY() - row->getMinY());
          row->setMinY(cury);
          row->setMaxY(cury + h);
        }
      }
    }
  }

  return row;
}

XWTeXDocumentRow * XWTeXDocumentObject::getRowForRTT(const QFontMetricsF & metrics,
                                bool reset, double & curx, double & cury,bool & firstcolumn)
{
  double aw = metrics.width(QChar('M'));
  double h = metrics.height();
  double skip = h * doc->getLineSkip();
  XWTeXDocumentRow * row = 0;
  bool newpage = newPage && !reset;
  bool newrow = newRow;
  if (doc->isTwocolumn())
  {
    double halfY = doc->getTop() + (doc->getBottom() - doc->getTop() - doc->getColumSep()) / 2;
    double halfYY = halfY + doc->getColumSep();
    if (firstcolumn)
    {
      if (newpage)
      {
        firstcolumn = true;
        XWTeXDocumentPage * page = doc->getNewPage();
        row = page->getNewRow();
        row->setDirection(TEX_DOC_WD_RTT);

        curx = doc->getRight();
        if (reset)
          cury = doc->getTop() + indent * h;
        else
          cury = doc->getTop() + (indent + firstIndent) * h;
        row->setMinX(curx - aw);
        row->setMaxX(curx);
        row->setMinY(cury);
        row->setMaxY(halfY - indent * h);
        row->setLimit(halfY - indent * h);
        row->setMaxLength((doc->getBottom() - doc->getTop() - doc->getColumSep()) / 2 - 2 * h * indent);
      }
      else
      {
        XWTeXDocumentPage * page = doc->getCurrentPage();
        if ((newrow || cury >= (halfY - indent * h)) && ((curx - skip) <= doc->getLeft()))
        {
          firstcolumn = false;
          row = page->getNewRow();
          row->setDirection(TEX_DOC_WD_RTT);
          curx = doc->getRight();
          if (reset)
            cury = halfYY + indent * h;
          else
            cury = halfYY + (indent + firstIndent) * h;

          row->setMinX(curx - aw);
          row->setMaxX(curx);
          row->setMinY(cury);
          row->setMaxY(doc->getBottom() - indent * h);
          row->setLimit(doc->getBottom() - indent * h);
          row->setMaxLength((doc->getBottom() - doc->getTop() - doc->getColumSep()) / 2 - 2 * h * indent);
        }
        else
        {
          if (cury >= (halfY - indent * h))
            newrow = true;

          if (newrow)
          {
            row = page->getNewRow();
            row->setDirection(TEX_DOC_WD_RTT);
            curx -= skip;
            if (reset)
              cury = doc->getTop() + indent * h;
            else
              cury = doc->getTop() + (indent + firstIndent) * h;
            row->setMinX(curx - aw);
            row->setMaxX(curx);
            row->setMinY(cury);
            row->setMaxY(halfY - indent * h);
            row->setLimit(halfY - indent * h);
            row->setMaxLength((doc->getBottom() - doc->getTop() - doc->getColumSep()) / 2 - 2 * h * indent);
          }
          else
          {
            row = page->getCurrentRow();
            if (aw > (row->getMaxX() - row->getMinX()))
            {
              curx -= h - ((row->getMaxX() - row->getMinX()));
              row->setMinX(curx - aw);
              row->setMaxX(curx);
            }
          }
        }
      }
    }
    else
    {
      if ((newrow || (cury >= (doc->getBottom() - indent * h))) &&
          ((curx - skip) <= doc->getLeft()))
        newpage = true;

      if (newpage)
      {
        firstcolumn = true;
        XWTeXDocumentPage * page = doc->getNewPage();
        row = page->getNewRow();
        row->setDirection(TEX_DOC_WD_RTT);
        curx = doc->getRight();
        if (reset)
          cury = doc->getTop() + indent * h;
        else
          cury = doc->getTop() + (indent + firstIndent) * h;

        row->setMinX(curx - aw);
        row->setMaxX(curx);
        row->setMinY(cury);
        row->setMaxY(halfY - indent * h);
        row->setLimit(halfY - indent * h);
        row->setMaxLength((doc->getBottom() - doc->getTop() - doc->getColumSep()) / 2 - 2 * h * indent);
      }
      else
      {
        if (cury >= (doc->getBottom() - indent * h))
          newrow = true;

        XWTeXDocumentPage * page = doc->getCurrentPage();
        if (newrow)
        {
          row = page->getNewRow();
          row->setDirection(TEX_DOC_WD_RTT);
          curx -= skip;
          if (reset)
            cury = halfYY + indent * h;
          else
            cury = halfYY + (indent + firstIndent) * h;
          row->setMinX(curx - aw);
          row->setMaxX(curx);
          row->setMinY(cury);
          row->setMaxY(doc->getBottom() - indent * h);
          row->setLimit(doc->getBottom() - indent * h);
          row->setMaxLength((doc->getBottom() - doc->getTop() - doc->getColumSep()) / 2 - 2 * h * indent);
        }
        else
        {
          row = page->getCurrentRow();
          if (aw > (row->getMaxX() - row->getMinX()))
          {
            curx -= h - ((row->getMaxX() - row->getMinX()));
            row->setMinX(curx - aw);
            row->setMaxX(curx);
          }
        }
      }
    }
  }
  else
  {
    if ((newrow || (cury >= (doc->getBottom() - indent * h))) &&
        ((curx - skip) <= doc->getLeft()))
      newpage = true;

    if (newpage)
    {
      XWTeXDocumentPage * page = doc->getNewPage();
      row = page->getNewRow();
      row->setDirection(TEX_DOC_WD_RTT);
      curx = doc->getRight();
      if (reset)
        cury = doc->getTop() + indent * h;
      else
        cury = doc->getTop() + (indent + firstIndent) * h;
      row->setMinX(curx - aw);
      row->setMaxX(curx);
      row->setMinY(cury);
      row->setMaxY(doc->getBottom() - indent * h);
      row->setLimit(doc->getBottom() - indent * h);
      row->setMaxLength(doc->getBottom() - doc->getTop() - 2 * h * indent);
    }
    else
    {
      if (cury >= (doc->getBottom() - indent * h))
        newrow = true;

      XWTeXDocumentPage * page = doc->getCurrentPage();
      if (newrow)
      {
        row = page->getNewRow();
        row->setDirection(TEX_DOC_WD_RTT);
        curx -= skip;
        if (reset)
          cury = doc->getTop() + indent * h;
        else
          cury = doc->getTop() + (indent + firstIndent) * h;
        row->setMinX(curx - aw);
        row->setMaxX(curx);
        row->setMinY(cury);
        row->setMaxY(doc->getBottom() - indent * h);
        row->setLimit(doc->getBottom() - indent * h);
        row->setMaxLength(doc->getBottom() - doc->getTop() - 2 * h * indent);
      }
      else
      {
        row = page->getCurrentRow();
        if (aw > (row->getMaxX() - row->getMinX()))
        {
          curx -= h - ((row->getMaxX() - row->getMinX()));
          row->setMinX(curx - aw);
          row->setMaxX(curx);
        }
      }
    }
  }

  return row;
}

XWTeXDocumentRow * XWTeXDocumentObject::getRowForLTL(const QFontMetricsF & metrics,
                                bool reset, double & curx, double & cury, bool & firstcolumn)
{
  double aw = metrics.width(QChar('M'));
  double h = metrics.height();
  double skip = h * doc->getLineSkip();
  XWTeXDocumentRow * row = 0;
  bool newpage = newPage && !reset;
  bool newrow = newRow;
  if (doc->isTwocolumn())
  {
    double halfY = doc->getTop() + (doc->getBottom() - doc->getTop() - doc->getColumSep()) / 2;
    double halfYY = halfY + doc->getColumSep();
    if (firstcolumn)
    {
      if (newpage)
      {
        firstcolumn = true;
        XWTeXDocumentPage * page = doc->getNewPage();
        row = page->getNewRow();
        row->setDirection(TEX_DOC_WD_LTL);
        curx = doc->getLeft();
        if (reset)
          cury = doc->getTop() + indent * h;
        else
          cury = doc->getTop() + (indent + firstIndent) * h;
        row->setMinX(curx);
        row->setMaxX(curx + aw);
        row->setMinY(cury);
        row->setMaxY(halfY - indent * h);
        row->setLimit(halfY - indent * h);
        row->setMaxLength((doc->getBottom() - doc->getTop() - doc->getColumSep()) / 2 - 2 * h * indent);
      }
      else
      {
        XWTeXDocumentPage * page = doc->getCurrentPage();
        if ((newrow || cury >= (halfY - indent * h)) && ((curx - skip) >= doc->getRight()))
        {
          firstcolumn = false;
          row = page->getNewRow();
          row->setDirection(TEX_DOC_WD_LTL);
          curx = doc->getLeft();
          if (reset)
            cury = halfYY + indent * h;
          else
            cury = halfYY + (indent + firstIndent) * h;

          row->setMinX(curx);
          row->setMaxX(curx + aw);
          row->setMinY(cury);
          row->setMaxY(doc->getBottom() - indent * h);
          row->setLimit(doc->getBottom() - indent * h);
          row->setMaxLength((doc->getBottom() - doc->getTop() - doc->getColumSep()) / 2 - 2 * h * indent);
        }
        else
        {
          if (cury >= (halfY - indent * h))
            newrow = true;

          if (newrow)
          {
            row = page->getNewRow();
            row->setDirection(TEX_DOC_WD_LTL);
            curx += skip;
            if (reset)
              cury = doc->getTop() + indent * h;
            else
              cury = doc->getTop() + (indent + firstIndent) * h;
            row->setMinX(curx);
            row->setMaxX(curx + aw);
            row->setMinY(cury);
            row->setMaxY(halfY - indent * h);
            row->setLimit(halfY - indent * h);
            row->setMaxLength((doc->getBottom() - doc->getTop() - doc->getColumSep()) / 2 - 2 * h * indent);
          }
          else
          {
            row = page->getCurrentRow();
            if (aw > (row->getMaxX() - row->getMinX()))
            {
              curx += h - ((row->getMaxX() - row->getMinX()));
              row->setMinX(curx);
              row->setMaxX(curx + aw);
            }
          }
        }
      }
    }
    else
    {
      if ((newrow || (cury >= (doc->getBottom() - indent * h))) &&
          ((curx - skip) >= doc->getRight()))
        newpage = true;

      if (newpage)
      {
        firstcolumn = true;
        XWTeXDocumentPage * page = doc->getNewPage();
        row = page->getNewRow();
        row->setDirection(TEX_DOC_WD_LTL);
        curx = doc->getLeft();
        if (reset)
          cury = doc->getTop() + indent * h;
        else
          cury = doc->getTop() + (indent + firstIndent) * h;

        row->setMinX(curx);
        row->setMaxX(curx + aw);
        row->setMinY(cury);
        row->setMaxY(halfY - indent * h);
        row->setLimit(halfY - indent * h);
        row->setMaxLength((doc->getBottom() - doc->getTop() - doc->getColumSep()) / 2 - 2 * h * indent);
      }
      else
      {
        if (cury >= (doc->getBottom() - indent * h))
          newrow = true;

        XWTeXDocumentPage * page = doc->getCurrentPage();
        if (newrow)
        {
          row = page->getNewRow();
          row->setDirection(TEX_DOC_WD_LTL);
          curx += skip;
          if (reset)
            cury = halfYY + indent * h;
          else
            cury = halfYY + (indent + firstIndent) * h;
          row->setMinX(curx);
          row->setMaxX(curx + aw);
          row->setMinY(cury);
          row->setMaxY(doc->getBottom() - indent * h);
          row->setLimit(doc->getBottom() - indent * h);
          row->setMaxLength((doc->getBottom() - doc->getTop() - doc->getColumSep()) / 2 - 2 * h * indent);
        }
        else
        {
          row = page->getCurrentRow();
          if (aw > (row->getMaxX() - row->getMinX()))
          {
            curx += h - ((row->getMaxX() - row->getMinX()));
            row->setMinX(curx);
            row->setMaxX(curx + aw);
          }
        }
      }
    }
  }
  else
  {
    if ((newrow || (cury >= (doc->getBottom() - indent * h))) &&
        ((curx - skip) >= doc->getRight()))
      newpage = true;

    if (newpage)
    {
      XWTeXDocumentPage * page = doc->getNewPage();
      row = page->getNewRow();
      row->setDirection(TEX_DOC_WD_LTL);
      curx = doc->getLeft();
      if (reset)
        cury = doc->getTop() + indent * h;
      else
        cury = doc->getTop() + (indent + firstIndent) * h;
      row->setMinX(curx);
      row->setMaxX(curx + aw);
      row->setMinY(cury);
      row->setMaxY(doc->getBottom() - indent * h);
      row->setLimit(doc->getBottom() - indent * h);
      row->setMaxLength(doc->getBottom() - doc->getTop() - 2 * h * indent);
    }
    else
    {
      if (cury >= (doc->getBottom() - indent * h))
        newrow = true;

      XWTeXDocumentPage * page = doc->getCurrentPage();
      if (newrow)
      {
        row = page->getNewRow();
        row->setDirection(TEX_DOC_WD_LTL);
        curx += skip;
        if (reset)
          cury = doc->getTop() + indent * h;
        else
          cury = doc->getTop() + (indent + firstIndent) * h;
        row->setMinX(curx);
        row->setMaxX(curx + aw);
        row->setMinY(cury);
        row->setMaxY(doc->getBottom() - indent * h);
        row->setLimit(doc->getBottom() - indent * h);
        row->setMaxLength(doc->getBottom() - doc->getTop() - 2 * h * indent);
      }
      else
      {
        row = page->getCurrentRow();
        if (aw > (row->getMaxX() - row->getMinX()))
        {
          curx += h - ((row->getMaxX() - row->getMinX()));
          row->setMinX(curx);
          row->setMaxX(curx + aw);
        }
      }
    }
  }

  return row;
}

void XWTeXDocumentObject::writeStr(const QString & str, QTextStream & strm, int & linelen)
{
  int len = str.length();
  for (int i = 0; i < len; i++)
  {
    if ((linelen >= doc->getLineMax()) ||
         ((linelen + 1) >= doc->getLineMax()) ||
         ((linelen + 2) >= doc->getLineMax()))
    {
      if (str[i].isSpace())
      {
        strm << "\n";
        linelen = 0;
      }
      else if (str[i].isPunct() ||
               (str[i].unicode() >= 0xFE30 && str[i].unicode() <= 0xFE4F) ||
               (str[i].unicode() >= 0xFE10 && str[i].unicode() <= 0xFE1F) ||
               (str[i].unicode() >= 0xF900 && str[i].unicode() <= 0xFAFF) ||
               (str[i].unicode() >= 0x4E00 && str[i].unicode() <= 0x9FBF) ||
               (str[i].unicode() >= 0x4DC0 && str[i].unicode() <= 0x4DFF) ||
               (str[i].unicode() >= 0x3400 && str[i].unicode() <= 0x4DBF) ||
               (str[i].unicode() >= 0x3300 && str[i].unicode() <= 0x33FF) ||
               (str[i].unicode() >= 0x3200 && str[i].unicode() <= 0x32FF) ||
               (str[i].unicode() >= 0x31F0 && str[i].unicode() <= 0x31FF) ||
               (str[i].unicode() >= 0x31C0 && str[i].unicode() <= 0x31EF) ||
               (str[i].unicode() >= 0x3190 && str[i].unicode() <= 0x319F) ||
               (str[i].unicode() >= 0x30A0 && str[i].unicode() <= 0x30FF) ||
               (str[i].unicode() >= 0x3040 && str[i].unicode() <= 0x309F) ||
               (str[i].unicode() >= 0x3000 && str[i].unicode() <= 0x303F) ||
               (str[i].unicode() >= 0x2FF0 && str[i].unicode() <= 0x2FFF) ||
               (str[i].unicode() >= 0x2F00 && str[i].unicode() <= 0x2FDF) ||
               (str[i].unicode() >= 0x2E80 && str[i].unicode() <= 0x2EFF))
      {
        strm << str[i];
        strm << "\n";
        linelen = 0;
      }
      else
      {
        strm << str[i];
        if (str[i] == QChar('\n'))
          linelen = 0;
        else
          linelen++;
      }
    }
    else
    {
      strm << str[i];
      if (str[i] == QChar('\n'))
        linelen = 0;
      else
        linelen++;
    }
  }
}

XWTeXDocumentBlock::XWTeXDocumentBlock(int tA, XWTeXDocument * docA, QObject * parent)
 : XWTeXDocumentObject(tA,docA,parent)
{}

XWTeXDocumentBlock::XWTeXDocumentBlock(int tA, XWTeXDocument * docA,
                                     bool newrow, QObject * parent)
  : XWTeXDocumentObject(tA,docA,newrow,parent)
{}

XWTeXDocumentBlock::XWTeXDocumentBlock(int tA, XWTeXDocument * docA, bool newrow, const QString & str, QObject * parent)
    : XWTeXDocumentObject(tA,docA,newrow, parent),
    text(str)
{
}

XWTeXDocumentBlock::~XWTeXDocumentBlock()
{}

void XWTeXDocumentBlock::append(const QString & str)
{
  if (isReadOnly())
    return ;

  text.append(str);
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  cursor->setHitPos(text.length());
  cursor->setStartPos(text.length());
  cursor->setEndPos(0);
  cursor->setSelected(false);
}

void XWTeXDocumentBlock::append(QChar c)
{
  if (isReadOnly())
    return ;

  text.append(c);
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  cursor->setHitPos(text.length());
  cursor->setStartPos(text.length());
  cursor->setEndPos(0);
  cursor->setSelected(false);
}

void XWTeXDocumentBlock::breakPage(const QFont & fontA,  const QColor & c,
                       double & curx, double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  font = fontA;
  color = c;
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

void XWTeXDocumentBlock::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setFont();
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

bool XWTeXDocumentBlock::canDeleteChar()
{
  if (isOnlyRead)
    return false;

  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  return (cursor->getHitPos() < text.length());
}

bool XWTeXDocumentBlock::canDeletePreviousChar()
{
  if (isOnlyRead)
    return false;

  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  return (cursor->getHitPos() > 0);
}

double XWTeXDocumentBlock::charWidth(QChar c)
{
  QFontMetricsF metrics(font);
  return metrics.width(c);
}

void XWTeXDocumentBlock::clear()
{
  text.clear();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  cursor->setHitPos(0);
  cursor->setStartPos(0);
  cursor->setEndPos(0);
  cursor->setSelected(false);
}

QString XWTeXDocumentBlock::del()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  QString str = text.mid(cursor->getStartPos(), cursor->getEndPos() - cursor->getStartPos());
  text.remove(cursor->getStartPos(), cursor->getEndPos() - cursor->getStartPos());
  cursor->setHitPos(cursor->getStartPos());
  cursor->setStartPos(0);
  cursor->setEndPos(0);
  cursor->setSelected(false);
  return str;
}

QChar XWTeXDocumentBlock::deleteChar()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  QChar c = text[cursor->getHitPos()];
  text.remove(cursor->getHitPos(),1);
  cursor->setStartPos(0);
  cursor->setEndPos(0);
  cursor->setSelected(false);
  return c;
}

QChar XWTeXDocumentBlock::deletePreviousChar()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  QChar c = text[cursor->getHitPos()-1];
  text.remove(cursor->getHitPos()-1, 1);
  cursor->setHitPos(cursor->getHitPos() - 1);
  cursor->setStartPos(0);
  cursor->setEndPos(0);
  cursor->setSelected(false);
  return c;
}

void XWTeXDocumentBlock::draw(QPainter * painter, const QColor & pagecolor, const QRectF & rect,
                  double & curx, double & cury, int from, int to)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      drawForTLT(painter,pagecolor,rect,curx,cury,from,to);
      break;

    case TEX_DOC_WD_RTT:
      drawForRTT(painter,pagecolor,rect,curx,cury,from,to);
      break;

    case TEX_DOC_WD_LTL:
      drawForLTL(painter,pagecolor,rect,curx,cury,from,to);
      break;

    case TEX_DOC_WD_TRT:
      drawForTRT(painter,pagecolor,rect,curx,cury,from,to);
      break;

    default:
      break;
  }
}

void XWTeXDocumentBlock::drawPic(QPainter * painter, double & curx, double & cury, int from, int to)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      drawPicForTLT(painter,curx,cury,from,to);
      break;

    case TEX_DOC_WD_RTT:
      drawPicForRTT(painter,curx,cury,from,to);
      break;

    case TEX_DOC_WD_LTL:
      drawPicForLTL(painter,curx,cury,from,to);
      break;

    case TEX_DOC_WD_TRT:
      drawPicForTRT(painter,curx,cury,from,to);
      break;

    default:
      break;
  }
}

void XWTeXDocumentBlock::draw(QPainter * painter, const QColor & pagecolor, const QRectF & rect,
                  double & curx, double & cury, int from)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      drawForTLT(painter,pagecolor,rect,curx,cury,from,text.length()-1);
      break;

    case TEX_DOC_WD_RTT:
      drawForRTT(painter,pagecolor,rect,curx,cury,from,text.length()-1);
      break;

    case TEX_DOC_WD_LTL:
      drawForLTL(painter,pagecolor,rect,curx,cury,from,text.length()-1);
      break;

    case TEX_DOC_WD_TRT:
      drawForTRT(painter,pagecolor,rect,curx,cury,from,text.length()-1);
      break;

    default:
      break;
  }
}

void XWTeXDocumentBlock::drawPic(QPainter * painter, double & curx, double & cury, int from)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      drawPicForTLT(painter,curx,cury,from,text.length()-1);
      break;

    case TEX_DOC_WD_RTT:
      drawPicForRTT(painter,curx,cury,from,text.length()-1);
      break;

    case TEX_DOC_WD_LTL:
      drawPicForLTL(painter,curx,cury,from,text.length()-1);
      break;

    case TEX_DOC_WD_TRT:
      drawPicForTRT(painter,curx,cury,from,text.length()-1);
      break;

    default:
      break;
  }
}

void XWTeXDocumentBlock::draw(QPainter * painter, const QColor & pagecolor, const QRectF & rect,
                  double & curx, double & cury)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      drawForTLT(painter,pagecolor,rect,curx,cury,0,text.length()-1);
      break;

    case TEX_DOC_WD_RTT:
      drawForRTT(painter,pagecolor,rect,curx,cury,0,text.length()-1);
      break;

    case TEX_DOC_WD_LTL:
      drawForLTL(painter,pagecolor,rect,curx,cury,0,text.length()-1);
      break;

    case TEX_DOC_WD_TRT:
      drawForTRT(painter,pagecolor,rect,curx,cury,0,text.length()-1);
      break;

    default:
      break;
  }
}

void XWTeXDocumentBlock::drawPic(QPainter * painter, double & curx, double & cury)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      drawPicForTLT(painter,curx,cury,0,text.length()-1);
      break;

    case TEX_DOC_WD_RTT:
      drawPicForRTT(painter,curx,cury,0,text.length()-1);
      break;

    case TEX_DOC_WD_LTL:
      drawPicForLTL(painter,curx,cury,0,text.length()-1);
      break;

    case TEX_DOC_WD_TRT:
      drawPicForTRT(painter,curx,cury,0,text.length()-1);
      break;

    default:
      break;
  }
}

bool XWTeXDocumentBlock::find(int & s,int & e, double & cur, double & , double & , QString & )
{
  skip(s, e, cur);
  return false;
}                    

bool XWTeXDocumentBlock::findReplaced(int & s,int & e, double & cur, double & , double & , QString &)
{
  skip(s, e, cur);
  return false;
}                    

void XWTeXDocumentBlock::flushBottom(const QFont & fontA,const QColor & c,double & curx,double & cury, bool & firstcolumn)
{
  dir = doc->getDirection();
  font = fontA;
  color = c;
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      flushRightForTLT(curx,cury,firstcolumn);
      break;

    case TEX_DOC_WD_RTT:
      flushBottomForRTT(curx,cury,firstcolumn);
      break;

    case TEX_DOC_WD_LTL:
      flushBottomForLTL(curx,cury,firstcolumn);
      break;

    case TEX_DOC_WD_TRT:
      flushLeftForTRT(curx,cury,firstcolumn);
      break;

    default:
      break;
  }
}

void XWTeXDocumentBlock::flushBottom(double & curx,double & cury, bool & firstcolumn)
{
  dir = doc->getDirection();
  setFont();
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      flushRightForTLT(curx,cury,firstcolumn);
      break;

    case TEX_DOC_WD_RTT:
      flushBottomForRTT(curx,cury,firstcolumn);
      break;

    case TEX_DOC_WD_LTL:
      flushBottomForLTL(curx,cury,firstcolumn);
      break;

    case TEX_DOC_WD_TRT:
      flushLeftForTRT(curx,cury,firstcolumn);
      break;

    default:
      break;
  }
}

QChar XWTeXDocumentBlock::getChar(int i)
{
  return text[i];
}

int  XWTeXDocumentBlock::getCurrentPos()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  return cursor->getHitPos();
}

int XWTeXDocumentBlock::getEndPos()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  return cursor->getEndPos();
}

QString XWTeXDocumentBlock::getMimeData()
{
  return text;
}

XWTeXDocumentObject * XWTeXDocumentBlock::getObject()
{
  if (type() >= TEX_DOC_B_UNKNOWN)
    return this;

  XWTeXDocumentObject * obj = (XWTeXDocumentObject*)(parent());
  while (obj->type() < TEX_DOC_B_UNKNOWN)
  {
    obj = (XWTeXDocumentObject*)(obj->parent());
  }

  return obj;
}

QString XWTeXDocumentBlock::getSelected()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (!cursor->hasSelect())
    return QString();

  return text.mid(cursor->getStartPos(),cursor->getEndPos() - cursor->getStartPos());
}

int  XWTeXDocumentBlock::getStartPos()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  return cursor->getStartPos();
}

bool XWTeXDocumentBlock::hasSelected()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  return cursor->hasSelect();
}

double XWTeXDocumentBlock::height()
{
  double h = 0;
  double tmph = 0;
  QFontMetricsF metrics(font);
  for (int i = 0; i < text.length();i++)
  {
    if (text[i] == QChar('\n'))
    {
      h = qMax(tmph, h);
      tmph = 0;
    }

    tmph += metrics.height();
  }
  h = qMax(tmph, h);
  return h;
}

bool XWTeXDocumentBlock::hitTest(const QPointF & p,
                    double & curx, double & cury,
                    int from, int to)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      return hitTestForTLT(p,curx,cury,from,to);
      break;

    case TEX_DOC_WD_RTT:
      return hitTestForRTT(p,curx,cury,from,to);
      break;

    case TEX_DOC_WD_LTL:
      return hitTestForLTL(p,curx,cury,from,to);
      break;

    case TEX_DOC_WD_TRT:
      return hitTestForTRT(p,curx,cury,from,to);
      break;

    default:
      break;
  }

  return false;
}

bool XWTeXDocumentBlock::hitTest(const QPointF & p, double & curx, double & cury, int from)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      return hitTestForTLT(p,curx,cury,from,text.length()-1);
      break;

    case TEX_DOC_WD_RTT:
      return hitTestForRTT(p,curx,cury,from,text.length()-1);
      break;

    case TEX_DOC_WD_LTL:
      return hitTestForLTL(p,curx,cury,from,text.length()-1);
      break;

    case TEX_DOC_WD_TRT:
      return hitTestForTRT(p,curx,cury,from,text.length()-1);
      break;

    default:
      break;
  }

  return false;
}

bool XWTeXDocumentBlock::hitTest(const QPointF & p,double & curx, double & cury)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      return hitTestForTLT(p,curx,cury,0,text.length()-1);
      break;

    case TEX_DOC_WD_RTT:
      return hitTestForRTT(p,curx,cury,0,text.length()-1);
      break;

    case TEX_DOC_WD_LTL:
      return hitTestForLTL(p,curx,cury,0,text.length()-1);
      break;

    case TEX_DOC_WD_TRT:
      return hitTestForTRT(p,curx,cury,0,text.length()-1);
      break;

    default:
      break;
  }

  return false;
}

void XWTeXDocumentBlock::insert(XWTeXDocumentObject * obj)
{
  if (isAtEnd())
  {
    obj->next = next;
    if (next)
      next->prev = obj;
    next = obj;
    obj->prev = this;
  }
  else
  {
    obj->prev = prev;
    if (prev)
      prev->next = obj;
    obj->next = this;
    prev = obj;
  }
}

void XWTeXDocumentBlock::insert(int pos, const QString & str)
{
  if (isReadOnly())
    return ;

  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (pos < text.length())
  {
    text.insert(pos, str);
    cursor->setHitPos(pos+str.length());
  }
  else
  {
    text.append(str);
    cursor->setHitPos(text.length());
  }
  cursor->setStartPos(0);
  cursor->setEndPos(0);
  cursor->setSelected(false);
  doc->setCurrentCursor(cursor);
}

void XWTeXDocumentBlock::insert(int pos, QChar c)
{
  if (isReadOnly())
    return ;

  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (pos < text.length())
    text.insert(pos,c);
  else
    text.append(c);

  cursor->setHitPos(pos + 1);
  cursor->setStartPos(0);
  cursor->setEndPos(0);
  cursor->setSelected(false);
  doc->setCurrentCursor(cursor);
}

bool XWTeXDocumentBlock::isAllSelected()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  return (cursor->getStartPos() == 0 && cursor->getEndPos() == text.length());
}

bool XWTeXDocumentBlock::isAtEnd()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  return cursor->getHitPos() >= text.length();
}

bool XWTeXDocumentBlock::isAtStart()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  return cursor->getHitPos() <= 0;
}

void XWTeXDocumentBlock::moveTo(double & curx, double & cury,int from, int to, int pos)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      moveToForTLT(curx,cury,from,to,pos);
      break;

    case TEX_DOC_WD_RTT:
      moveToForRTT(curx,cury,from,to,pos);
      break;

    case TEX_DOC_WD_LTL:
      moveToForLTL(curx,cury,from,to,pos);
      break;

    case TEX_DOC_WD_TRT:
      moveToForTRT(curx,cury,from,to,pos);
      break;

    default:
      break;
  }
}

void XWTeXDocumentBlock::moveTo(double & curx, double & cury,int from, int pos)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      moveToForTLT(curx,cury,from,text.length()-1,pos);
      break;

    case TEX_DOC_WD_RTT:
      moveToForRTT(curx,cury,from,text.length()-1,pos);
      break;

    case TEX_DOC_WD_LTL:
      moveToForLTL(curx,cury,from,text.length()-1,pos);
      break;

    case TEX_DOC_WD_TRT:
      moveToForTRT(curx,cury,from,text.length()-1,pos);
      break;

    default:
      break;
  }
}

void XWTeXDocumentBlock::moveTo(double & curx, double & cury,int pos)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      moveToForTLT(curx,cury,0,text.length()-1,pos);
      break;

    case TEX_DOC_WD_RTT:
      moveToForRTT(curx,cury,0,text.length()-1,pos);
      break;

    case TEX_DOC_WD_LTL:
      moveToForLTL(curx,cury,0,text.length()-1,pos);
      break;

    case TEX_DOC_WD_TRT:
      moveToForTRT(curx,cury,0,text.length()-1,pos);
      break;

    default:
      break;
  }
}

void XWTeXDocumentBlock::moveToEnd()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  cursor->setHitPos(text.length());
  doc->setCurrentCursor(cursor);
}

void XWTeXDocumentBlock::moveToFirst()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  cursor->setHitPos(0);
  doc->setCurrentCursor(cursor);
}

void XWTeXDocumentBlock::moveToHitPos(double & curx, double & cury,int from, int to)
{
  int pos = getCurrentPos();
  moveTo(curx, cury, from, to, pos);
}

void XWTeXDocumentBlock::moveToHitPos(double & curx, double & cury,int from)
{
  int pos = getCurrentPos();
  moveTo(curx, cury, from, pos);
}

void XWTeXDocumentBlock::moveToHitPos(double & curx, double & cury)
{
  int pos = getCurrentPos();
  moveTo(curx, cury, pos);
}

void XWTeXDocumentBlock::moveToLast()
{
  if (isOnlyRead)
    return ;

  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  cursor->setHitPos(text.length() - 1);
  doc->setCurrentCursor(cursor);
}

void XWTeXDocumentBlock::moveToNextChar()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (cursor->getHitPos() < text.length())
    cursor->setHitPos(cursor->getHitPos() + 1);
  doc->setCurrentCursor(cursor);
}

void XWTeXDocumentBlock::moveToNextWord()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  int hitpos = cursor->getHitPos();
  while (text[hitpos].isLetter())
    hitpos++;

  while (!text[hitpos].isLetter())
    hitpos++;

  cursor->setHitPos(hitpos);
  doc->setCurrentCursor(cursor);
}

void XWTeXDocumentBlock::moveToPreviousChar()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  int hitpos = cursor->getHitPos();
  if (hitpos > 0)
  {
    hitpos--;
    cursor->setHitPos(hitpos);
  }
  doc->setCurrentCursor(cursor);
}

void XWTeXDocumentBlock::moveToPreviousWord()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  int hitpos = cursor->getHitPos();

  while (hitpos >= 0 && text[hitpos].isLetter())
    hitpos--;

  while (hitpos >= 0 && !text[hitpos].isLetter())
    hitpos--;

  while (hitpos >= 0 && text[hitpos].isLetter())
    hitpos--;

  cursor->setHitPos(hitpos + 1);
  doc->setCurrentCursor(cursor);
}

void XWTeXDocumentBlock::moveToRowEnd(double & curx, double & cury,int pos)
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  doc->setCurrentCursor(cursor);
  cursor->setHitPos(pos + 1);
  QFontMetricsF metrics(font);
  double cw = doc->getHalfCursorWidth();
  double w = metrics.width(text[pos]);
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      doc->setCursor(curx - cw,cury - metrics.height(), curx + cw, cury);
      break;

    case TEX_DOC_WD_RTT:
      doc->setCursor(curx,cury - cw, curx + w, cury + cw);
      break;

    case TEX_DOC_WD_LTL:
      doc->setCursor(curx - w,cury - cw, curx, cury + cw);
      break;

    case TEX_DOC_WD_TRT:
      doc->setCursor(curx - cw,cury - metrics.height(), curx + cw, cury);
      break;

    default:
      break;
  }
}

void XWTeXDocumentBlock::moveToRowStart(double & curx, double & cury,int pos)
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  cursor->setHitPos(pos);
  doc->setCurrentCursor(cursor);
  QFontMetricsF metrics(font);
  double cw = doc->getHalfCursorWidth();
  double w = metrics.width(text[pos]);
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      doc->setCursor(curx - cw,cury - metrics.height(), curx + cw, cury);
      break;

    case TEX_DOC_WD_RTT:
      doc->setCursor(curx,cury - cw, curx + w, cury + cw);
      break;

    case TEX_DOC_WD_LTL:
      doc->setCursor(curx - w,cury - cw, curx, cury + cw);
      break;

    case TEX_DOC_WD_TRT:
      doc->setCursor(curx - cw,cury - metrics.height(), curx + cw, cury);
    default:
      break;
  }
}

void XWTeXDocumentBlock::moveToStart()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  cursor->setHitPos(0);
  doc->setCurrentCursor(cursor);
}

void XWTeXDocumentBlock::resetSelect()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  cursor->setStartPos(0);
  cursor->setEndPos(0);
  cursor->setSelected(false);
}

void XWTeXDocumentBlock::scanBeamerSpec(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos >= len || str[pos] != QChar('<'))
    return ;

  pos++;
  int b = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('<'))
    {
      b++;
      text.append(str[pos]);
    }
    else if (str[pos] == QChar('>'))
    {
      b--;
      if (b < 0)
      {
        pos++;
        return ;
      }

      text.append(str[pos]);
    }
    else if (str[pos] == QChar('\n'))
      text.append(' ');
    else
      text.append(str[pos]);
    pos++;
  }

  text = text.trimmed();
}

void XWTeXDocumentBlock::scanCoord(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos >= len || str[pos] != QChar('('))
    return ;

  int b = 0;
  pos++;
  while (pos < len)
  {
    if (str[pos] == QChar(')'))
    {
      b--;
      if (b < 0)
      {
        pos++;
        return ;
      }
    }
    else if (str[pos] == QChar('('))
      b++;

    text.append(str[pos]);
    pos++;
  }
  text = text.trimmed();
}

void XWTeXDocumentBlock::scanGroup(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos >= len || str[pos] != QChar('{'))
    return ;

  int b = 0;
  pos++;
  while (pos < len)
  {
    if (str[pos] == QChar('}'))
    {
      b--;
      if (b < 0)
      {
        pos++;
        return ;
      }
    }
    else if (str[pos] == QChar('{'))
      b++;

    text.append(str[pos]);
    pos++;
  }
  text = text.trimmed();
}

void XWTeXDocumentBlock::scanOption(const QString & str, int & len, int & pos)
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
    {
      b++;
      text.append(str[pos]);
    }
    else if (str[pos] == QChar(']'))
    {
      b--;
      if (b < 0)
      {
        pos++;
        return ;
      }

      text.append(str[pos]);
    }
    else if (str[pos] == QChar('\n'))
      text.append(' ');
    else
      text.append(str[pos]);
    pos++;
  }
  text = text.trimmed();
}

void XWTeXDocumentBlock::scanParam(const QString & str, int & len, int & pos)
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
    {
      b++;
      text.append(str[pos]);
    }
    else if (str[pos] == QChar('}'))
    {
      b--;
      if (b < 0)
      {
        pos++;
        return ;
      }

      text.append(str[pos]);
    }
    else if (str[pos] == QChar('\n'))
      text.append(' ');
    else
      text.append(str[pos]);
    pos++;
  }

  text = text.trimmed();
}

void XWTeXDocumentBlock::selectBack(int pos)
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (cursor->getStartPos() > pos)
    cursor->setStartPos(pos);

  cursor->setHitPos(pos);

  if (cursor->hasSelect())
    doc->addSelect(this);
}

void XWTeXDocumentBlock::selectBlock()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (cursor->getStartPos() != 0)
    cursor->setStartPos(0);

  if (cursor->getEndPos() != text.length())
    cursor->setEndPos(text.length());

  cursor->setHitPos(text.length());

  if (cursor->hasSelect())
    doc->addSelect(this);
}

void XWTeXDocumentBlock::selectEndOfBlock()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  cursor->setEndPos(text.length());
  cursor->setHitPos(text.length());
  if (cursor->hasSelect())
    doc->addSelect(this);
}

void XWTeXDocumentBlock::selectForward(int pos)
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (cursor->getEndPos() < pos)
    cursor->setEndPos(pos + 1);
  cursor->setHitPos(pos + 1);
  if (cursor->hasSelect())
    doc->addSelect(this);
}

void XWTeXDocumentBlock::selectNextChar()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (cursor->getEndPos() < text.length())
    cursor->setEndPos(cursor->getEndPos() + 1);

  cursor->setHitPos(cursor->getEndPos());

  if (cursor->hasSelect())
    doc->addSelect(this);
}

void XWTeXDocumentBlock::selectNextWord()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  int endpos = cursor->getEndPos();
  while (endpos < text.length() && text[endpos].isLetter())
    endpos++;

  while (endpos < text.length() && !text[endpos].isLetter())
    endpos++;

  while (endpos < text.length() && text[endpos].isLetter())
    endpos++;
  cursor->setEndPos(endpos);
  cursor->setHitPos(endpos);
  if (cursor->hasSelect())
    doc->addSelect(this);
}

void XWTeXDocumentBlock::selectPreviousChar()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (cursor->getStartPos() > 0)
    cursor->setStartPos(cursor->getStartPos() - 1);
  cursor->setHitPos(cursor->getStartPos());
  if (cursor->hasSelect())
    doc->addSelect(this);
}

void XWTeXDocumentBlock::selectPreviousWord()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  int startpos = cursor->getStartPos();
  while (startpos > -1 && text[startpos].isLetter())
    startpos--;

  while (startpos > -1 && !text[startpos].isLetter())
    startpos--;

  while (startpos > -1 && text[startpos].isLetter())
    startpos--;

  cursor->setStartPos(startpos + 1);
  cursor->setHitPos(startpos + 1);
  if (cursor->hasSelect())
    doc->addSelect(this);
}

void XWTeXDocumentBlock::selectStartOfBlock()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  cursor->setStartPos(0);
  cursor->setHitPos(0);
  if (cursor->hasSelect())
    doc->addSelect(this);
}

void XWTeXDocumentBlock::selectWord()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  int startpos = cursor->getHitPos();
  int endpos = cursor->getHitPos();
  while (startpos > -1 && text[startpos].isLetter())
    startpos--;

  cursor->setStartPos(startpos + 1);

  while (endpos < text.length() && text[endpos].isLetter())
    endpos++;

  cursor->setEndPos(endpos);
  cursor->setHitPos(endpos);

  if (cursor->hasSelect())
    doc->addSelect(this);
}

void XWTeXDocumentBlock::setCursor()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  doc->setCurrentCursor(cursor);
}

void XWTeXDocumentBlock::setCursorAtEnd()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  cursor->setHitPos(text.length());
  doc->setCurrentCursor(cursor);
}

void XWTeXDocumentBlock::setCursorAtStart()
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  cursor->setHitPos(0);
  doc->setCurrentCursor(cursor);
}

void XWTeXDocumentBlock::setSelected(const QRectF & rect, double & curx, double & cury,
                               int from, int to)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      setSelectedForTLT(rect,curx,cury,from,to);
      break;

    case TEX_DOC_WD_RTT:
      setSelectedForRTT(rect,curx,cury,from,to);
      break;

    case TEX_DOC_WD_LTL:
      setSelectedForLTL(rect,curx,cury,from,to);
      break;

    case TEX_DOC_WD_TRT:
      setSelectedForTRT(rect,curx,cury,from,to);
      break;

    default:
      break;
  }
}

void XWTeXDocumentBlock::setSelected(const QRectF & rect, double & curx, double & cury,
                              int from)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      setSelectedForTLT(rect,curx,cury,from,text.length()-1);
      break;

    case TEX_DOC_WD_RTT:
      setSelectedForRTT(rect,curx,cury,from,text.length()-1);
      break;

    case TEX_DOC_WD_LTL:
      setSelectedForLTL(rect,curx,cury,from,text.length()-1);
      break;

    case TEX_DOC_WD_TRT:
      setSelectedForTRT(rect,curx,cury,from,text.length()-1);
      break;

    default:
      break;
  }
}

void XWTeXDocumentBlock::setSelected(const QRectF & rect, double & curx, double & cury)
{
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      setSelectedForTLT(rect,curx,cury,0,text.length()-1);
      break;

    case TEX_DOC_WD_RTT:
      setSelectedForRTT(rect,curx,cury,0,text.length()-1);
      break;

    case TEX_DOC_WD_LTL:
      setSelectedForLTL(rect,curx,cury,0,text.length()-1);
      break;

    case TEX_DOC_WD_TRT:
      setSelectedForTRT(rect,curx,cury,0,text.length()-1);
      break;

    default:
      break;
  }
}

void XWTeXDocumentBlock::setSelected(int s, int e)
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  cursor->setStartPos(s);
  cursor->setEndPos(e);
  cursor->setHitPos(e + 1);
}

void XWTeXDocumentBlock::skip(int s, int e, double & cur)
{
  QFontMetricsF metrics(font);
  double h = metrics.height();
  switch (dir)
  {
    default:
      for (; s <= e; s++)
        cur += h;
      break;

    case TEX_DOC_WD_TLT:
      for (; s <= e; s++)
      {
        double w = metrics.width(text[s]);
        cur += w;
      }
      break;

    case TEX_DOC_WD_TRT:
      for (; s <= e; s++)
      {
        double w = metrics.width(text[s]);
        cur -= w;
      }
      break;
  }
}

double XWTeXDocumentBlock::width()
{
  double w = 0;
  double tmpw = 0;
  int len = text.length();
  QFontMetricsF metrics(font);
  for (int i = 0; i < len; i++)
  {
    if (text[i] == QChar('\n'))
    {
      w = qMax(tmpw,w);
      tmpw = 0;
    }
    tmpw += metrics.width(text[i]);
  }
  w = qMax(tmpw,w);
  return w;
}

void XWTeXDocumentBlock::write(QTextStream & strm, int & linelen)
{
  if (linelen != 0 && (newRow || newPage))
  {
    strm << "\n";
    linelen = 0;
  }

  QString str = getMimeData();
  strm << str;
  int pos = str.lastIndexOf(QChar('\n'));
  if (pos < 0)
    linelen += str.length();
  else
    linelen += str.length() - pos;
}

void XWTeXDocumentBlock::breakPageForTRT(double & curx,double & cury,bool & firstcolumn)
{
  QFontMetricsF metrics(font);
  double h = metrics.height();
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentRow * row = getRowForTRT(metrics,false,curx,cury,firstcolumn);
  row->setDirection(TEX_DOC_WD_TRT);
  row->addBlock(this);
  int len = text.length();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  double aw = metrics.width(QChar('M'));
  if (len == 0)
    curx -= aw;
  else
  {
    for (int i = 0; i < len; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          aw = metrics.width(QChar('M'));
          h = metrics.height();
        }
      }

      if (text[i] == QChar('\n'))
      {
        row->setEndPos(i - 1);
        row->setMinX(curx);
        if (doc->isCurrentBlock(this))
        {
          if (cursor->getHitPos() == i)
            doc->setCursor(curx - cw, row->getMaxY() - h, curx + cw, row->getMaxY());
        }

        if (i < (len - 1))
        {
          row = getRowForTRT(metrics, true,curx,cury,firstcolumn);
          row->setDirection(TEX_DOC_WD_TRT);
          row->setStartPos(i + 1);
          row->addBlock(this);
        }
      }
      else
      {
        if (curx <= row->getLimit())
        {
          row->setEndPos(i - 1);
          row->setMinX(curx);
          if (i < (len - 1))
          {
            row = getRowForTRT(metrics, true,curx,cury,firstcolumn);
            row->setDirection(TEX_DOC_WD_TRT);
            row->setStartPos(i);
            row->addBlock(this);
          }
        }

        double w = metrics.width(text[i]);
        curx -= w;

        if (doc->isCurrentBlock(this))
        {
          if (cursor->getHitPos() == i)
            doc->setCursor(curx + w - cw, row->getMaxY() - h, curx + w + cw, row->getMaxY());
        }
      }
    }

    row->setEndPos(len - 1);
  }

  row->setMinX(curx);
  if (doc->isCurrentBlock(this))
  {
    if (cursor->getHitPos() == len)
      doc->setCursor(curx - cw, row->getMaxY() - h, curx + cw, row->getMaxY());
  }
}

void XWTeXDocumentBlock::breakPageForTLT(double & curx,double & cury,bool & firstcolumn)
{
  QFontMetricsF metrics(font);
  double h = metrics.height();
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentRow * row = getRowForTLT(metrics,false,curx,cury,firstcolumn);
  row->setDirection(TEX_DOC_WD_TLT);
  row->addBlock(this);
  int len = text.length();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  double aw = metrics.width(QChar('M'));
  if (len == 0)
    curx += aw;
  else
  {
    for (int i = 0; i < len; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          aw = metrics.width(QChar('M'));
          h = metrics.height();
        }
      }

      if (text[i] == QChar('\n'))
      {
        row->setEndPos(i - 1);
        row->setMaxX(curx);
        if (doc->isCurrentBlock(this))
        {
          if (cursor->getHitPos() == i)
            doc->setCursor(curx - cw, row->getMaxY() - h, curx + cw, row->getMaxY());
        }

        if (i < (len - 1))
        {
          row = getRowForTLT(metrics, true,curx,cury,firstcolumn);
          row->setDirection(TEX_DOC_WD_TLT);
          row->setStartPos(i + 1);
          row->addBlock(this);
        }
      }
      else
      {
        if (curx >= row->getLimit())
        {
          row->setEndPos(i - 1);
          row->setMaxX(curx);
          if (i < (len - 1))
          {
            row = getRowForTLT(metrics, true,curx,cury,firstcolumn);
            row->setDirection(TEX_DOC_WD_TLT);
            row->setStartPos(i);
            row->addBlock(this);
          }
        }
        double w = metrics.width(text[i]);
        curx += w;
        if (doc->isCurrentBlock(this))
        {
          if (cursor->getHitPos() == i)
            doc->setCursor(curx - w - cw, row->getMaxY() - h, curx - w + cw, row->getMaxY());
        }
      }
    }

    row->setEndPos(len - 1);
  }
  row->setMaxX(curx);
  if (doc->isCurrentBlock(this))
  {
    if (cursor->getHitPos() == len)
      doc->setCursor(curx - cw, row->getMaxY() - h, curx + cw, row->getMaxY());
  }
}

void XWTeXDocumentBlock::breakPageForRTT(double & curx,double & cury,bool & firstcolumn)
{
  QFontMetricsF metrics(font);
  double aw = metrics.width(QChar('M'));
  double h = metrics.height();
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentRow * row = getRowForRTT(metrics,false,curx,cury,firstcolumn);
  row->setDirection(TEX_DOC_WD_RTT);
  row->addBlock(this);
  int len = text.length();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (len == 0)
    cury += h;
  else
  {
    for (int i = 0; i < len; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          aw = metrics.width(QChar('M'));
          h = metrics.height();
        }
      }

      if (text[i] == QChar('\n'))
      {
        row->setEndPos(i - 1);
        row->setMaxY(cury);
        if (doc->isCurrentBlock(this))
        {
          if (cursor->getHitPos() == i)
            doc->setCursor(row->getMinX(), cury - cw, row->getMinX() + aw, cury + cw);
        }

        if (i < (len - 1))
        {
          row = getRowForRTT(metrics,true,curx,cury,firstcolumn);
          row->setDirection(TEX_DOC_WD_RTT);
          row->setStartPos(i + 1);
          row->addBlock(this);
        }
      }
      else
      {
        if (cury >= row->getLimit())
        {
          row->setEndPos(i - 1);
          row->setMaxY(cury);
          if (i < (len - 1))
          {
            row = getRowForRTT(metrics,true,curx,cury,firstcolumn);
            row->setDirection(TEX_DOC_WD_RTT);
            row->setStartPos(i);
            row->addBlock(this);
          }
        }

        cury += h;
        if (doc->isCurrentBlock(this))
        {
          if (cursor->getHitPos() == i)
          {
            double w = metrics.width(text[i]);
            doc->setCursor(row->getMinX(), cury - h - cw, row->getMinX() + w, cury - h + cw);
          }
        }
      }
    }

    row->setEndPos(len - 1);
  }

  row->setMaxY(cury);
  if (doc->isCurrentBlock(this))
  {
    if (cursor->getHitPos() == len)
      doc->setCursor(row->getMinX(), cury - cw, row->getMinX() + aw, cury + cw);
  }
}

void XWTeXDocumentBlock::breakPageForLTL(double & curx,double & cury,bool & firstcolumn)
{
  QFontMetricsF metrics(font);
  double aw = metrics.width(QChar('M'));
  double h = metrics.height();
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentRow * row = getRowForLTL(metrics,false,curx,cury,firstcolumn);
  row->setDirection(TEX_DOC_WD_LTL);
  row->addBlock(this);
  int len = text.length();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (len == 0)
    cury += h;
  else
  {
    for (int i = 0; i < len; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          aw = metrics.width(QChar('M'));
          h = metrics.height();
        }
      }

      if (text[i] == QChar('\n'))
      {
        row->setEndPos(i - 1);
        row->setMaxY(cury);
        if (doc->isCurrentBlock(this))
        {
          if (cursor->getHitPos() == i)
            doc->setCursor(row->getMaxX() - aw, cury - cw, row->getMaxX(), cury + cw);
        }

        if (i < (len - 1))
        {
          row = getRowForLTL(metrics, true,curx,cury,firstcolumn);
          row->setDirection(TEX_DOC_WD_LTL);
          row->setStartPos(i + 1);
          row->addBlock(this);
        }
      }
      else
      {
        if (cury >= row->getLimit())
        {
          row->setEndPos(i - 1);
          row->setMaxY(cury);
          if (i < (len - 1))
          {
            row = getRowForLTL(metrics,true,curx,cury,firstcolumn);
            row->setDirection(TEX_DOC_WD_LTL);
            row->setStartPos(i);
            row->addBlock(this);
          }
        }

        cury += h;
        if (doc->isCurrentBlock(this))
        {
          if (cursor->getHitPos() == i)
          {
            double w = metrics.width(text[i]);
            doc->setCursor(row->getMaxX() - w, cury - h - cw, row->getMaxX(), cury - h + cw);
          }
        }
      }
    }

    row->setEndPos(len - 1);
  }
  row->setMaxY(cury);
  if (doc->isCurrentBlock(this))
  {
    if (cursor->getHitPos() == len)
      doc->setCursor(row->getMaxX() - aw, cury - cw, row->getMaxX(), cury + cw);
  }
}

void XWTeXDocumentBlock::drawForTRT(QPainter * painter,const QColor & pagecolor, const QRectF & rect,
                        double & curx, double & cury,int from, int to)
{

  QFontMetricsF metrics(font);
  double aw = metrics.width(QChar('M'));
  double h = metrics.height();
  if (text.length() == 0)
  {
    curx -= aw;
    QRectF r(curx,cury - h,aw,h);
    if (r.intersects(rect))
    {
      if (isSelected(0))
        painter->fillRect(r,color);
    }
  }
  else
  {
    painter->save();
    painter->setFont(font);
    painter->setPen(color);
    QTextOption opt(Qt::AlignCenter);
    opt.setTextDirection(Qt::RightToLeft);
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          aw = metrics.width(QChar('M'));
          h = metrics.height();
        }
      }

      double w = metrics.width(text[i]);
      curx -= w;
      QRectF r(curx,cury - h,w,h);
      if (r.intersects(rect))
      {
        if (isSelected(i))
        {
          painter->fillRect(r,color);
          painter->setPen(pagecolor);
        }
        else
          painter->setPen(color);

        QString s(text[i]);
        painter->drawText(r,s,opt);
      }
    }
    painter->restore();
  }
}

void XWTeXDocumentBlock::drawPicForTRT(QPainter * painter, double & curx, double & cury,int from, int to)
{
  QFontMetricsF metrics(font);
  double aw = metrics.width(QChar('M'));
  double h = metrics.height();
  if (text.length() == 0)
    curx -= aw;
  else
  {
    painter->setFont(font);
    painter->setPen(color);
    QTextOption opt(Qt::AlignCenter);
    opt.setTextDirection(Qt::RightToLeft);
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          aw = metrics.width(QChar('M'));
          h = metrics.height();
        }
      }

      double w = metrics.width(text[i]);
      curx -= w;
      QRectF r(curx,cury - h,w,h);
      QString s(text[i]);
      painter->drawText(r,s,opt);
    }
  }
}

void XWTeXDocumentBlock::drawForTLT(QPainter * painter,const QColor & pagecolor, const QRectF & rect,
                              double & curx, double & cury,int from, int to)
{

  QFontMetricsF metrics(font);
  double aw = metrics.width(QChar('M'));
  double h = metrics.height();
  if (text.length() == 0)
  {
    QRectF r(curx,cury - h,aw,h);
    if (r.intersects(rect))
    {
      if (isSelected(0))
        painter->fillRect(r,color);
    }
    curx += aw;
  }
  else
  {
    painter->save();
    painter->setFont(font);
    painter->setPen(color);
    QTextOption opt(Qt::AlignCenter);
    opt.setTextDirection(Qt::LeftToRight);
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          h = metrics.height();
        }
      }

      double w = metrics.width(text[i]);
      QRectF r(curx,cury - h,w,h);
      if (r.intersects(rect))
      {
        if (isSelected(i))
        {
          painter->fillRect(r,color);
          painter->setPen(pagecolor);
        }
        else
          painter->setPen(color);

        QString s(text[i]);
        painter->drawText(r,s,opt);
      }
      curx += w;
    }

    painter->restore();
  }
}

void XWTeXDocumentBlock::drawPicForTLT(QPainter * painter,  double & curx, double & cury,int from, int to)
{
  QFontMetricsF metrics(font);
  double aw = metrics.width(QChar('M'));
  double h = metrics.height();
  if (text.length() == 0)
    curx += aw;
  else
  {
    painter->setFont(font);
    painter->setPen(color);
    QTextOption opt(Qt::AlignCenter);
    opt.setTextDirection(Qt::LeftToRight);
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          h = metrics.height();
        }
      }

      double w = metrics.width(text[i]);
      QRectF r(curx,cury - h,w,h);
      QString s(text[i]);
      painter->drawText(r,s,opt);
      curx += w;
    }
  }
}

void XWTeXDocumentBlock::drawForRTT(QPainter * painter,const QColor & pagecolor, const QRectF & rect,
                        double & curx, double & cury,int from, int to)
{
  QFontMetricsF metrics(font);
  double h = metrics.height();
  if (text.length() == 0)
  {
    double aw = metrics.width(QChar('M'));
    QRectF r(curx,cury,aw,h);
    if (r.intersects(rect))
    {
      if (isSelected(0))
        painter->fillRect(r,color);
    }
    cury += h;
  }
  else
  {
    painter->save();
    painter->setFont(font);
    painter->setPen(color);
    QTextOption opt(Qt::AlignCenter);
    opt.setTextDirection(Qt::RightToLeft);
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          h = metrics.height();
        }
      }

      double w = metrics.width(text[i]);
      QRectF r(curx,cury,w,h);
      if (r.intersects(rect))
      {
        if (isSelected(i))
        {
          painter->fillRect(r,color);
          painter->setPen(pagecolor);
        }
        else
          painter->setPen(color);

        switch (text[i].unicode())
        {
          case CJK_H_PERIOD:
            {
              QChar c(CJK_V_PERIOD);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_FULLSTOP:
            {
              QChar c(CJK_V_FULLSTOP);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_QUESTIONMARK:
            {
              QChar c(CJK_V_QUESTIONMARK);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_EXCLAMATIONMARK:
            {
              QChar c(CJK_V_EXCLAMATIONMARK);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_COMMA:
            {
              QChar c(CJK_V_COMMA);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_THECOMMA:
            {
              QChar c(CJK_V_THECOMMA);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_SEMICOLON:
            {
              QChar c(CJK_V_SEMICOLON);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_COLON:
            {
              QChar c(CJK_V_COLON);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_LSQUOTION:
            {
              QChar c(CJK_V_LSQUOTION);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_LSCBRACKET:
            {
              QChar c(CJK_V_LSCBRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_RSQUOTION:
            {
              QChar c(CJK_V_RSQUOTION);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_RSCBRACKET:
            {
              QChar c(CJK_V_RSCBRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_LDQUOTION:
            {
              QChar c(CJK_V_LDQUOTION);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_LWCBRACKET:
            {
              QChar c(CJK_V_LWCBRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_RDQUOTION:
            {
              QChar c(CJK_V_RDQUOTION);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_RWCBRACKET:
            {
              QChar c(CJK_V_RWCBRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_LPARENTHESIS:
            {
              QChar c(CJK_V_LPARENTHESIS);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_RPARENTHESIS:
            {
              QChar c(CJK_V_RPARENTHESIS);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_LSBRACKET:
            {
              QChar c(CJK_V_LSBRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_RSBRACKET:
            {
              QChar c(CJK_V_RSBRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_LTSBRACKET:
            {
              QChar c(CJK_V_LTSBRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_RTSBRACKET:
            {
              QChar c(CJK_V_RTSBRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_LCUBRACKET:
            {
              QChar c(CJK_V_LCUBRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_RCUBRACKET:
            {
              QChar c(CJK_V_RCUBRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_LBLCBRACKET:
            {
              QChar c(CJK_V_LBLCBRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_RBLCBRACKET:
            {
              QChar c(CJK_V_RBLCBRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_QUOTIONDASH:
            {
              QChar c(CJK_V_QUOTIONDASH);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_ELLIPSIS:
            {
              QChar c(CJK_V_ELLIPSIS);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_LDABRACKET:
            {
              QChar c(CJK_V_LDABRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_RDABRACKET:
            {
              QChar c(CJK_V_RDABRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_LABRACKET:
            {
              QChar c(CJK_V_LABRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_RABRACKET:
            {
              QChar c(CJK_V_RABRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_WAVE:
            {
              QChar c(CJK_V_WAVE);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_LWLCUBRACKET:
            {
              QChar c(CJK_V_LWLCUBRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_RWLCUBRACKET:
            {
              QChar c(CJK_V_RWLCUBRACKET);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          case CJK_H_HYPHENCHAR:
            {
              QChar c(CJK_V_HYPHENCHAR);
              QString s(c);
              painter->drawText(r,s,opt);
            }
            break;

          default:
            {
              QString s(text[i]);
              painter->drawText(r,s,opt);
            }
            break;
        }
      }
      cury += h;
    }

    painter->restore();
  }
}

void XWTeXDocumentBlock::drawPicForRTT(QPainter * painter,double & curx, double & cury,int from, int to)
{
  QFontMetricsF metrics(font);
  double h = metrics.height();
  if (text.length() == 0)
    cury += h;
  else
  {
    painter->setFont(font);
    painter->setPen(color);
    QTextOption opt(Qt::AlignCenter);
    opt.setTextDirection(Qt::RightToLeft);
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          h = metrics.height();
        }
      }

      double w = metrics.width(text[i]);
      QRectF r(curx,cury,w,h);
      switch (text[i].unicode())
      {
        case CJK_H_PERIOD:
          {
            QChar c(CJK_V_PERIOD);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_FULLSTOP:
          {
            QChar c(CJK_V_FULLSTOP);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_QUESTIONMARK:
          {
            QChar c(CJK_V_QUESTIONMARK);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_EXCLAMATIONMARK:
          {
            QChar c(CJK_V_EXCLAMATIONMARK);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_COMMA:
          {
            QChar c(CJK_V_COMMA);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_THECOMMA:
          {
            QChar c(CJK_V_THECOMMA);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_SEMICOLON:
          {
            QChar c(CJK_V_SEMICOLON);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_COLON:
          {
            QChar c(CJK_V_COLON);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_LSQUOTION:
          {
            QChar c(CJK_V_LSQUOTION);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_LSCBRACKET:
          {
            QChar c(CJK_V_LSCBRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_RSQUOTION:
          {
            QChar c(CJK_V_RSQUOTION);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_RSCBRACKET:
          {
            QChar c(CJK_V_RSCBRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_LDQUOTION:
          {
            QChar c(CJK_V_LDQUOTION);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_LWCBRACKET:
          {
            QChar c(CJK_V_LWCBRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_RDQUOTION:
          {
            QChar c(CJK_V_RDQUOTION);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_RWCBRACKET:
          {
            QChar c(CJK_V_RWCBRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_LPARENTHESIS:
          {
            QChar c(CJK_V_LPARENTHESIS);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_RPARENTHESIS:
          {
            QChar c(CJK_V_RPARENTHESIS);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_LSBRACKET:
          {
            QChar c(CJK_V_LSBRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_RSBRACKET:
          {
            QChar c(CJK_V_RSBRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_LTSBRACKET:
          {
            QChar c(CJK_V_LTSBRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_RTSBRACKET:
          {
            QChar c(CJK_V_RTSBRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_LCUBRACKET:
          {
            QChar c(CJK_V_LCUBRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_RCUBRACKET:
          {
            QChar c(CJK_V_RCUBRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_LBLCBRACKET:
          {
            QChar c(CJK_V_LBLCBRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_RBLCBRACKET:
          {
            QChar c(CJK_V_RBLCBRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_QUOTIONDASH:
          {
            QChar c(CJK_V_QUOTIONDASH);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_ELLIPSIS:
          {
            QChar c(CJK_V_ELLIPSIS);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_LDABRACKET:
          {
            QChar c(CJK_V_LDABRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_RDABRACKET:
          {
            QChar c(CJK_V_RDABRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_LABRACKET:
          {
            QChar c(CJK_V_LABRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_RABRACKET:
          {
            QChar c(CJK_V_RABRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_WAVE:
          {
            QChar c(CJK_V_WAVE);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_LWLCUBRACKET:
          {
            QChar c(CJK_V_LWLCUBRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_RWLCUBRACKET:
          {
            QChar c(CJK_V_RWLCUBRACKET);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        case CJK_H_HYPHENCHAR:
          {
            QChar c(CJK_V_HYPHENCHAR);
            QString s(c);
            painter->drawText(r,s,opt);
          }
          break;

        default:
          {
            QString s(text[i]);
            painter->drawText(r,s,opt);
          }
          break;
      }
      cury += h;
    }
  }
}

void XWTeXDocumentBlock::drawForLTL(QPainter * painter,const QColor & pagecolor, const QRectF & rect,
                        double & curx, double & cury,int from, int to)
{
  QFontMetricsF metrics(font);
  double h = metrics.height();
  if (text.length() == 0)
  {
    double aw = metrics.width(QChar('M'));
    QRectF r(curx - aw,cury,aw,h);
    if (r.intersects(rect))
    {
      if (isSelected(0))
        painter->fillRect(r,color);
    }
    cury += h;
  }
  else
  {
    painter->save();
    painter->setFont(font);
    painter->setPen(color);
    QTextOption opt(Qt::AlignCenter);
    opt.setTextDirection(Qt::LeftToRight);
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          h = metrics.height();
        }
      }

      double w = metrics.width(text[i]);
      QRectF r(curx - w,cury,w,h);
      if (r.intersects(rect))
      {
        if (isSelected(i))
        {
          painter->fillRect(r,color);
          painter->setPen(pagecolor);
        }
        else
          painter->setPen(color);

        QString s(text[i]);
        painter->drawText(r,s,opt);
      }
      cury += h;
    }

    painter->restore();
  }
}

void XWTeXDocumentBlock::drawPicForLTL(QPainter * painter,double & curx, double & cury,int from, int to)
{
  QFontMetricsF metrics(font);
  double h = metrics.height();
  if (text.length() == 0)
    cury += h;
  else
  {
    painter->setFont(font);
    painter->setPen(color);
    QTextOption opt(Qt::AlignCenter);
    opt.setTextDirection(Qt::LeftToRight);
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          h = metrics.height();
        }
      }

      double w = metrics.width(text[i]);
      QRectF r(curx - w,cury,w,h);
      QString s(text[i]);
      painter->drawText(r,s,opt);
      cury += h;
    }
  }
}

void XWTeXDocumentBlock::flushBottomForRTT(double & curx,double & cury,bool & firstcolumn)
{
  QFontMetricsF metrics(font);
  double aw = metrics.width(QChar('M'));
  double h = metrics.height();
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentRow * row = getRowForRTT(metrics,false,curx,cury,firstcolumn);
  row->addBlock(this);
  int len = text.length();
  if ((row->getMaxY() - row->getMinY()) == row->getMaxLength())
    cury = row->getMaxY();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  XWTeXDocumentRow * currow = 0;
  if (len == 0)
    cury -= h;
  else
  {
    for (int i = 0; i < len; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          aw = metrics.width(QChar('M'));
          h = metrics.height();
        }
      }
        if (cury <= (row->getMaxY() - row->getMaxLength()))
        {
          row->setEndPos(i - 1);
          row->setMinY(cury);
          if (i < (len - 1))
          {
            row = getRowForRTT(metrics,true,curx,cury,firstcolumn);
            row->setStartPos(i);
            row->addBlock(this);
          }
        }

        cury -= h;
        if (doc->isCurrentBlock(this))
        {
          if (cursor->getHitPos() == i)
            currow = row;
        }
    }

    row->setEndPos(len - 1);
  }
  row->setMinY(cury);
  if (doc->isCurrentBlock(this))
  {
    if (currow)
      currow->moveToHitPos();
    else if (cursor->getHitPos() == len)
      doc->setCursor(row->getMinX(), cury - cw, row->getMinX() + aw, cury + cw);
  }
}

void XWTeXDocumentBlock::flushBottomForLTL(double & curx,double & cury,bool & firstcolumn)
{
  QFontMetricsF metrics(font);
  double aw = metrics.width(QChar('M'));
  double h = metrics.height();
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentRow * row = getRowForLTL(metrics,false,curx,cury,firstcolumn);
  row->addBlock(this);
  int len = text.length();
  if ((row->getMaxY() - row->getMinY()) == row->getMaxLength())
    cury = row->getMaxY();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  XWTeXDocumentRow * currow = 0;
  if (len == 0)
    cury -= h;
  else
  {
    for (int i = 0; i < len; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          aw = metrics.width(QChar('M'));
          h = metrics.height();
        }
      }

        if (cury <= (row->getMaxY() - row->getMaxLength()))
        {
          row->setEndPos(i - 1);
          row->setMinY(cury);
          if (i < (len - 1))
          {
            row = getRowForLTL(metrics,true,curx,cury,firstcolumn);
            row->setStartPos(i);
            row->addBlock(this);
          }
        }

        cury -= h;
        if (doc->isCurrentBlock(this))
        {
          if (cursor->getHitPos() == i)
            currow = row;
        }
    }

    row->setEndPos(len - 1);
  }
  row->setMinY(cury);
  if (doc->isCurrentBlock(this))
  {
    if (currow)
      currow->moveToHitPos();
    else if (cursor->getHitPos() == len)
      doc->setCursor(row->getMaxX() - aw, cury - cw, row->getMaxX(), cury + cw);
  }
}

void XWTeXDocumentBlock::flushLeftForTRT(double & curx,double & cury,bool & firstcolumn)
{
  QFontMetricsF metrics(font);
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentRow * row = getRowForTLT(metrics,false,curx,cury,firstcolumn);
  row->addBlock(this);
  row->setDirection(TEX_DOC_WD_TRT);
  int len = text.length();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  XWTeXDocumentRow * currow = 0;
  double aw = metrics.width(QChar('M'));
  if (len == 0)
    curx += aw;
  else
  {
    for (int i = 0; i < len; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          aw = metrics.width(QChar('M'));
        }
      }

      if (curx >= row->getLimit())
      {
        row->setEndPos(i - 1);
        row->setMinX(curx);
        if (i < (len - 1))
        {
          row = getRowForTLT(metrics, true,curx,cury,firstcolumn);
          row->setDirection(TEX_DOC_WD_TRT);
          row->setStartPos(i);
          row->addBlock(this);
        }
      }

      double w = metrics.width(text[i]);
      curx += w;

      if (doc->isCurrentBlock(this))
      {
        if (cursor->getHitPos() == i)
          currow = row;
      }
    }
    row->setEndPos(len - 1);
  }

  row->setMinX(curx);
  if (doc->isCurrentBlock(this))
  {
    if (currow)
      currow->moveToHitPos();
    else if (cursor->getHitPos() == len)
    {
      double h = metrics.height();
      doc->setCursor(row->getMinX() - cw, row->getMaxY() - h, row->getMinX() + cw, row->getMaxY());
    }
  }
}

void XWTeXDocumentBlock::flushRightForTLT(double & curx,double & cury,bool & firstcolumn)
{
  QFontMetricsF metrics(font);
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentRow * row = getRowForTRT(metrics,false,curx,cury,firstcolumn);
  row->setDirection(TEX_DOC_WD_TLT);
  row->addBlock(this);
  int len = text.length();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  XWTeXDocumentRow * currow = 0;
  double aw = metrics.width(QChar('M'));
  if (len == 0)
    curx -= aw;
  else
  {
    for (int i = 0; i < len; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          aw = metrics.width(QChar('M'));
        }
      }

      if (curx <= row->getLimit())
      {
        row->setEndPos(i - 1);
        row->setMinX(curx);
        if (i < (len - 1))
        {
          row = getRowForTRT(metrics, true,curx,cury,firstcolumn);
          row->setDirection(TEX_DOC_WD_TLT);
          row->setStartPos(i);
          row->addBlock(this);
        }
      }
      double w = metrics.width(text[i]);
      curx -= w;
      if (doc->isCurrentBlock(this))
      {
        if (cursor->getHitPos() == i)
          currow = row;
      }
    }
    row->setEndPos(len - 1);
  }
  row->setMinX(curx);
  if (doc->isCurrentBlock(this))
  {
    if (currow)
      currow->moveToHitPos();
    else if (cursor->getHitPos() == len)
    {
      double h = metrics.height();
      doc->setCursor(row->getMaxX() - cw, row->getMaxY() - h, row->getMaxX() + cw, row->getMaxY());
    }
  }
}

bool XWTeXDocumentBlock::hitTestForTRT(const QPointF & p, double & curx, double & cury, int from, int to)
{
  QFontMetricsF metrics(font);
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (text.isEmpty())
  {
    double w = metrics.width(QChar('M'));
    if ((curx >= p.x()) && (p.x() >= (curx - w)))
    {
      cursor->setHitPos(0);
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx - cw,cury - metrics.height(), curx + cw, cury);
      return true;
    }
    curx -= w;
  }
  else
  {
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
        }
      }

      double w = metrics.width(text[i]);
      if ((curx >= p.x()) && (p.x() >= (curx - w)))
      {
        doc->setCurrentCursor(cursor);
        if (p.x() >= (curx - 0.5 * w))
        {
          cursor->setHitPos(i);
          doc->setCursor(curx - cw,cury - metrics.height(), curx + cw, cury);
        }
        else
        {
          cursor->setHitPos(i + 1);
          doc->setCursor(curx - w - cw,cury - metrics.height(), curx - w + cw, cury);
        }

        return true;
      }
      curx -= w;
    }
  }

  return false;
}

bool XWTeXDocumentBlock::hitTestForTLT(const QPointF & p, double & curx, double & cury,int from, int to)
{
  QFontMetricsF metrics(font);
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (text.isEmpty())
  {
    double w = metrics.width(QChar('M'));
    if ((curx <= p.x()) && (p.x() <= (curx + w)))
    {
      cursor->setHitPos(0);
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx - cw,cury - metrics.height(), curx + cw, cury);
      return true;
    }
    curx += w;
  }
  else
  {
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
        }
      }

      double w = metrics.width(text[i]);
      if ((curx <= p.x()) && (p.x() <= (curx + w)))
      {
        doc->setCurrentCursor(cursor);
        if (p.x() < (curx + 0.5 * w))
        {
          cursor->setHitPos(i);
          doc->setCursor(curx - cw,cury - metrics.height(), curx + cw, cury);
        }
        else
        {
          cursor->setHitPos(i + 1);
          doc->setCursor(curx + w - cw,cury - metrics.height(), curx + w + cw, cury);
        }

        return true;
      }
      curx += w;
    }
  }

  return false;
}

bool XWTeXDocumentBlock::hitTestForRTT(const QPointF & p,double & curx, double & cury,int from, int to)
{
  QFontMetricsF metrics(font);
  double cw = doc->getHalfCursorWidth();
  double h = metrics.height();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (text.isEmpty())
  {
    if ((cury <= p.y()) && (p.y() <= (cury + h)))
    {
      double w = metrics.width(QChar('M'));
      cursor->setHitPos(0);
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx, cury - cw, curx + w, cury + cw);
      return true;
    }
    cury += h;
  }
  else
  {
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          h = metrics.height();
        }
      }

      if ((cury <= p.y()) && (p.y() <= (cury + h)))
      {
        double w = metrics.width(text[i]);
        doc->setCurrentCursor(cursor);
        if (p.y() < (cury + 0.5 * h))
        {
          cursor->setHitPos(i);
          doc->setCursor(curx, cury - cw, curx + w, cury + cw);
        }
        else
        {
          cursor->setHitPos(i + 1);
          doc->setCursor(curx, cury + h - cw, curx + w, cury + h + cw);
        }

        return true;
      }
      cury += h;
    }
  }

  return false;
}

bool XWTeXDocumentBlock::isSelected(int i)
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  return (doc->isAllSelect() || cursor->isSelected(i));
}

bool XWTeXDocumentBlock::hitTestForLTL(const QPointF & p,double & curx, double & cury,int from, int to)
{
  QFontMetricsF metrics(font);
  double cw = doc->getHalfCursorWidth();
  double h = metrics.height();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (text.isEmpty())
  {
    if ((cury <= p.y()) && (p.y() <= (cury + h)))
    {
      double w = metrics.width(QChar('M'));
      cursor->setHitPos(0);
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx - w, cury - cw, curx, cury + cw);
      return true;
    }
    cury += h;
  }
  else
  {
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          h = metrics.height();
        }
      }

      if ((cury <= p.y()) && (p.y() <= (cury + h)))
      {
        double w = metrics.width(text[i]);
        doc->setCurrentCursor(cursor);
        if (p.y() < (cury + 0.5 * h))
        {
          cursor->setHitPos(i);
          doc->setCursor(curx - w, cury - cw, curx, cury + cw);
        }
        else
        {
          cursor->setHitPos(i + 1);
          doc->setCursor(curx - w, cury + h - cw, curx, cury + h + cw);
        }

        return true;
      }
      cury += h;
    }
  }

  return false;
}

void XWTeXDocumentBlock::moveToForTRT(double & curx, double & cury,int from, int to, int pos)
{
  QFontMetricsF metrics(font);
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (text.isEmpty())
  {
    double w = metrics.width(QChar('M'));
    if (pos == 0)
    {
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx - cw,cury - metrics.height(), curx + cw, cury);
    }
    curx -= w;
  }
  else
  {
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
        }
      }

      double w = metrics.width(text[i]);
      if (i == pos)
      {
        cursor->setHitPos(i);
        doc->setCurrentCursor(cursor);
        doc->setCursor(curx - cw,cury - metrics.height(), curx + cw, cury);
        return ;
      }
      curx -= w;
    }

    if (pos == (to + 1))
    {
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx - cw,cury - metrics.height(), curx + cw, cury);
    }
  }
}

void XWTeXDocumentBlock::moveToForTLT(double & curx, double & cury,int from, int to, int pos)
{
  QFontMetricsF metrics(font);
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (text.isEmpty())
  {
    double w = metrics.width(QChar('M'));
    if (pos == 0)
    {
      cursor->setHitPos(0);
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx - cw,cury - metrics.height(), curx + cw, cury);
    }
    curx += w;
  }
  else
  {
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
        }
      }
      double w = metrics.width(text[i]);
      if (i == pos)
      {
        cursor->setHitPos(i);
        doc->setCurrentCursor(cursor);
        doc->setCursor(curx - cw,cury - metrics.height(), curx + cw, cury);
        return ;
      }
      curx += w;
    }

    if (pos == (to + 1))
    {
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx - cw,cury - metrics.height(), curx + cw, cury);
    }
  }
}

void XWTeXDocumentBlock::moveToForRTT(double & curx, double & cury,int from, int to, int pos)
{
  QFontMetricsF metrics(font);
  double cw = doc->getHalfCursorWidth();
  double h = metrics.height();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (text.isEmpty())
  {
    double w = metrics.width(QChar('M'));
    if (pos == 0)
    {
      cursor->setHitPos(0);
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx, cury - cw, curx + w, cury + cw);
    }
    cury += h;
  }
  else
  {
    for (int i = from; i <= to; i++)
    {
      if (i == pos)
      {
        if (!metrics.inFont(text[i]))
        {
          QString fam = getUnicodeFontFamily(text[i]);
          if (!fam.isEmpty())
          {
            font.setFamily(fam);
            QFontMetricsF fm(font);
            metrics = fm;
            h = metrics.height();
          }
        }

        double w = metrics.width(text[i]);
        cursor->setHitPos(i);
        doc->setCurrentCursor(cursor);
        doc->setCursor(curx, cury - cw, curx + w, cury + cw);
        return ;
      }
      cury += h;
    }

    if (pos == (to + 1))
    {
      double w = metrics.width(QChar('M'));
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx, cury - cw, curx + w, cury + cw);
    }
  }
}

void XWTeXDocumentBlock::moveToForLTL(double & curx, double & cury,int from, int to, int pos)
{
  QFontMetricsF metrics(font);
  double cw = doc->getHalfCursorWidth();
  double h = metrics.height();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (text.isEmpty())
  {
    double w = metrics.width(QChar('M'));
    if (pos == 0)
    {
      cursor->setHitPos(0);
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx - w, cury - cw, curx, cury + cw);
    }
    cury += h;
  }
  else
  {
    for (int i = from; i <= to; i++)
    {
      if (i == pos)
      {
        if (!metrics.inFont(text[i]))
        {
          QString fam = getUnicodeFontFamily(text[i]);
          if (!fam.isEmpty())
          {
            font.setFamily(fam);
            QFontMetricsF fm(font);
            metrics = fm;
            h = metrics.height();
          }
        }
        double w = metrics.width(text[i]);
        cursor->setHitPos(i);
        doc->setCurrentCursor(cursor);
        doc->setCursor(curx - w, cury - cw, curx, cury + cw);
        return ;
      }
      cury += h;
    }

    if (pos == (to + 1))
    {
      double w = metrics.width(QChar('M'));
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx - w, cury - cw, curx, cury + cw);
    }
  }
}

void XWTeXDocumentBlock::setSelectedForTRT(const QRectF & rect, double & curx,
                                           double & cury,int from, int to)
{
  QFontMetricsF metrics(font);
  double h = metrics.height();
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (text.isEmpty())
  {
    double w = metrics.width(QChar('M'));
    curx -= w;
    QRectF r(curx,cury - h,w,h);
    if (r.intersects(rect))
    {
      cursor->setSelected(true);
      cursor->setStartPos(0);
      cursor->setEndPos(0);
      cursor->setHitPos(0);
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx - cw, cury - h, curx + cw, cury);
    }
  }
  else
  {
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          h = metrics.height();
        }
      }
      double w = metrics.width(text[i]);
      curx -= w;
      QRectF r(curx,cury - h,w,h);
      if (r.intersects(rect))
      {
        if (i < cursor->getStartPos())
          cursor->setStartPos(i);
        else if (i > cursor->getEndPos())
        {
          cursor->setEndPos(i + 1);
          cursor->setHitPos(i + 1);
          doc->setCurrentCursor(cursor);
          doc->setCursor(curx - cw, cury - h, curx + cw, cury);
        }
      }
    }
  }

  if (cursor->hasSelect())
    doc->addSelect(this);
}

void XWTeXDocumentBlock::setSelectedForTLT(const QRectF & rect, double & curx, double & cury,int from, int to)
{
  QFontMetricsF metrics(font);
  double h = metrics.height();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  double cw = doc->getHalfCursorWidth();
  if (text.isEmpty())
  {
    double w = metrics.width(QChar('M'));
    QRectF r(curx,cury - h,w,h);
    if (r.intersects(rect))
    {
      cursor->setSelected(true);
      cursor->setHitPos(0);
      cursor->setStartPos(0);
      cursor->setEndPos(0);
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx - cw, cury - h, curx + cw, cury);
    }
    curx += w;
  }
  else
  {
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          h = metrics.height();
        }
      }
      double w = metrics.width(text[i]);
      QRectF r(curx,cury - h,w,h);
      if (r.intersects(rect))
      {
        if (i < cursor->getStartPos())
          cursor->setStartPos(i);
        else if (i > cursor->getEndPos())
        {
          cursor->setEndPos(i + 1);
          cursor->setHitPos(i + 1);
          doc->setCurrentCursor(cursor);
          doc->setCursor(curx + w - cw, cury - h, curx + w + cw, cury);
        }
      }
      curx += w;
    }
  }

  if (cursor->hasSelect())
    doc->addSelect(this);
}

void XWTeXDocumentBlock::setSelectedForRTT(const QRectF & rect, double & curx, double & cury,
                               int from, int to)
{
  QFontMetricsF metrics(font);
  double h = metrics.height();
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (text.isEmpty())
  {
    double w = metrics.width(QChar('M'));
    QRectF r(curx,cury,w,h);
    if (r.intersects(rect))
    {
      cursor->setSelected(true);
      cursor->setHitPos(0);
      cursor->setStartPos(0);
      cursor->setEndPos(0);
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx, cury - cw, curx + w, cury + cw);
    }
    cury += h;
  }
  else
  {
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          h = metrics.height();
        }
      }
      double w = metrics.width(text[i]);
      QRectF r(curx,cury,w,h);
      if (r.intersects(rect))
      {
        if (i < cursor->getStartPos())
          cursor->setStartPos(i);
        else if (i > cursor->getEndPos())
        {
          cursor->setEndPos(i + 1);
          cursor->setHitPos(i + 1);
          doc->setCurrentCursor(cursor);
          doc->setCursor(curx, cury + h - cw, curx + w, cury + h + cw);
        }
      }
      cury += h;
    }
  }

  if (cursor->hasSelect())
    doc->addSelect(this);
}

void XWTeXDocumentBlock::setSelectedForLTL(const QRectF & rect, double & curx, double & cury,
                               int from, int to)
{
  QFontMetricsF metrics(font);
  double h = metrics.height();
  double cw = doc->getHalfCursorWidth();
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  if (text.isEmpty())
  {
    double w = metrics.width(QChar('M'));
    QRectF r(curx - w,cury,w,h);
    if (r.intersects(rect))
    {
      cursor->setSelected(true);
      cursor->setHitPos(0);
      cursor->setStartPos(0);
      cursor->setEndPos(0);
      doc->setCurrentCursor(cursor);
      doc->setCursor(curx - w, cury - cw, curx, cury + cw);
    }
    cury += h;
  }
  else
  {
    for (int i = from; i <= to; i++)
    {
      if (!metrics.inFont(text[i]))
      {
        QString fam = getUnicodeFontFamily(text[i]);
        if (!fam.isEmpty())
        {
          font.setFamily(fam);
          QFontMetricsF fm(font);
          metrics = fm;
          h = metrics.height();
        }
      }
      double w = metrics.width(text[i]);
      QRectF r(curx - w,cury,w,h);
      if (r.intersects(rect))
      {
        if (i < cursor->getStartPos())
          cursor->setStartPos(i);
        else if (i > cursor->getEndPos())
        {
          cursor->setEndPos(i + 1);
          cursor->setHitPos(i + 1);
          doc->setCurrentCursor(cursor);
          doc->setCursor(curx - w, cury + h - cw, curx, cury + h + cw);
        }
      }
      cury += h;
    }
  }

  if (cursor->hasSelect())
    doc->addSelect(this);
}

XWTeXDocumentText::XWTeXDocumentText(XWTeXDocument * docA, QObject * parent)
 : XWTeXDocumentBlock(TEX_DOC_B_TEXT,docA,parent)
{
  specialFont = false;
}

bool XWTeXDocumentText::find()
{
  QString str  = doc->getFindString();
  QTextDocument::FindFlags op = doc->getFindFlags();
  
  QTextDocument d(text);
  QTextCursor cursor(&d);
  cursor = d.find(str, cursor, op);
  return !cursor.isNull();
}

bool XWTeXDocumentText::find(int & s,int & e, double & cur,
                             double & mi, double & ma,
                             QString & content)
{
  QString str  = doc->getFindString();
  QTextDocument::FindFlags op = doc->getFindFlags();
  
  QTextDocument d(text);
  QTextCursor cursor(&d);
  cursor.setPosition(s);
  cursor = d.find(str, cursor, op);
  if (cursor.isNull() || cursor.position() > e)
  {
    skip(s, e, cur);
    return false;
  }

  content = text.mid(s, e - s + 1);
  QFontMetricsF metrics(font);
  double h = metrics.height();
  for (int i = s; i <= e; i++)
  {
    if (!metrics.inFont(text[i]))
    {
      QString fam = getUnicodeFontFamily(text[i]);
      if (!fam.isEmpty())
      {
        font.setFamily(fam);
        QFontMetricsF fm(font);
        metrics = fm;
        h = metrics.height();
      }
    }

    switch (dir)
    {
      default:
        {
          cur += h;
          if (i == cursor.position())
            mi = cur - h;
          else if ( i = cursor.position() + str.length())
          {
            ma = cur;
            s = cursor.position();
            e = i;
            return true;
          }
        }
        break;

      case TEX_DOC_WD_TLT:
        {
          double w = metrics.width(text[i]);
          cur += w;
          if (i == cursor.position())
            mi = cur - w;
          else if ( i = cursor.position() + str.length())
          {
            ma = cur;
            s = cursor.position();
            e = i;
            return true;
          }
        }
        break;

      case TEX_DOC_WD_TRT:
        {
          double w = metrics.width(text[i]);
          cur -= w;
          if (i == cursor.position())
            ma = cur + w;
          else if ( i = cursor.position() + str.length())
          {
            mi = cur;
            s = cursor.position();
            e = i;
            return true;
          }
        }
        break;
    }
  }
  
  return false;
}

bool XWTeXDocumentText::findNext(int s, int e)
{
  XWTeXDocumentCursor * cor = doc->getCursor(this);
  doc->setCurrentCursor(cor);

  QString str  = doc->getFindString();
  QTextDocument::FindFlags op = doc->getFindFlags();
  
  QTextDocument d(text);
  QTextCursor cursor(&d);
  cursor.setPosition(cor->getEndPos());

  if (doc->isRegexpMatch())
  {
    QRegExp regexp(str);
    cursor = d.find(regexp, cursor, op);
  }
  else
    cursor = d.find(str, cursor, op);

  if (cursor.isNull() || cursor.position() >= e)
  {
    cor->setStartPos(0);
    cor->setHitPos(text.length());
    cor->setEndPos(0);
    cor->setSelected(false);
    return false;
  }

  cor->setStartPos(cursor.selectionStart());  
  cor->setEndPos(cursor.selectionEnd());
  cor->setHitPos(cor->getEndPos());
  return true;
}

bool XWTeXDocumentText::findReplaced(int & s,int & e, double & cur,
                                     double & mi, double & ma,
                                     QString & content)
{
  QString str  = doc->getReplaceString();
  QTextDocument d(text);
  QTextCursor cursor(&d);
  cursor.setPosition(s);
  int len = str.length();
  if (len > 0)
  {
    QTextDocument::FindFlags op = doc->getFindFlags();
    cursor = d.find(str, cursor, op);
    if (cursor.isNull() || cursor.position() > e)
    {
      skip(s, e, cur);
      return false;
    }
  }
  else
  {
    cursor.setPosition(e);
    len++;
  } 
  content = text.mid(s, e - s + 1);
  QFontMetricsF metrics(font);
  double h = metrics.height();
  for (int i = s; i <= e; i++)
  {
    if (!metrics.inFont(text[i]))
    {
      QString fam = getUnicodeFontFamily(text[i]);
      if (!fam.isEmpty())
      {
        font.setFamily(fam);
        QFontMetricsF fm(font);
        metrics = fm;
        h = metrics.height();
      }
    }

    switch (dir)
    {
      default:
        {
          cur += h;
          if (i == cursor.position())
          {
            mi = cur - h;
            s = i;
            for (int j = 0; j < len; j++)
            {
              int k = i + j;
              if (k <= e)
                cur += h;
            } 
            ma = cur;
            e = i + len;
            return true;
          }
        }
        break;

      case TEX_DOC_WD_TLT:
        {
          double w = metrics.width(text[i]);
          cur += w;
          if (i == cursor.position())
          {
            mi = cur - w;
            s = i;
            for (int j = 0; j < len; j++)
            {
              int k = i + j;
              if (k <= e)
              {
                w = metrics.width(text[i + j]);
                cur += w;
              }
            }
            ma = cur;
            e = i + len;
            return true;
          }
        }
        break;

      case TEX_DOC_WD_TRT:
        {
          double w = metrics.width(text[i]);
          cur -= w;
          if (i == cursor.position())
          {
            ma = cur + w;
            s = i;
            for (int j = 0; j < len; j++)
            {
              int k = i + j;
              if (k <= e)
              {
                w = metrics.width(text[i + j]);
                cur -= w;
              }
            }
            ma = cur;
            e = i + len;
            return true;
          }
        }
        break;
    }
  }
  
  return false;
}

void XWTeXDocumentText::insert(XWTeXDocumentObject * obj)
{
  if (isAtStart())
  {
    obj->prev = prev;
    if (prev)
      prev->next = obj;
    prev = obj;
    obj->next = this;
    if (parent() != doc)
    {
      XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
      if (this == pobj->head)
      {
        pobj->head = obj;
        obj->prev = 0;
      }        
    }
  }
  else if (isAtEnd())
  {
    obj->next = next;    
    obj->prev = this;
    if (next)
      next->prev = obj;
    next = obj;
    if (parent() != doc)
    {
      XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
      if (this == pobj->last)
      {
        pobj->last = obj;
        obj->next = 0;
      }        
    }
  }
  else
  {
    XWTeXDocumentCursor * cor = doc->getCursor(this);
    int pos = cor->getHitPos();
    XWTeXDocumentText * newobj = new XWTeXDocumentText(doc,parent());    
    newobj->text = text.mid(pos,-1);
    text.remove(pos, newobj->text.length());
    newobj->next = next;
    if (next)
      next->prev = newobj;
    newobj->prev = obj;
    obj->next = newobj;
    obj->prev = this;
    next = obj;
    if (parent() != doc)
    {
      XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
      if (this == pobj->last)
      {
        pobj->last = newobj;
        newobj->next = 0;
      }        
    }
  }
}

bool XWTeXDocumentText::replaceNext(int s, int e)
{
  XWTeXDocumentCursor * cor = doc->getCursor(this);
  doc->setCurrentCursor(cor);

  QString str  = doc->getFindString();
  QString bystr = doc->getReplaceString();
  QTextDocument::FindFlags op = doc->getFindFlags();  
  
  QTextDocument d(text);
  QTextCursor cursor(&d);
  cursor.setPosition(cor->getEndPos());
  if (doc->isRegexpMatch())
  {
    QRegExp regexp(str);
    cursor = d.find(regexp, cursor, op);
  }
  else
    cursor = d.find(str, cursor, op);

  if (cursor.isNull() || cursor.position() >= e)
  {
    cor->setStartPos(0);
    cor->setHitPos(text.length());
    cor->setEndPos(0);
    cor->setSelected(false);
    return false;
  }

  cor->setStartPos(cursor.selectionStart());
  cor->setEndPos(cursor.selectionStart() + str.length());
  cor->setHitPos(cor->getEndPos());
  doc->insertText(bystr);
  cor->setStartPos(cursor.selectionStart());
  cor->setEndPos(cursor.selectionStart() + bystr.length());
  cor->setHitPos(cor->getEndPos());
  return true;
}

void XWTeXDocumentText::scan(const QString & str, int & len, int & pos)
{
  while (pos < len)
  {
    if ((str[pos] == QChar('\\')) ||
        (str[pos] == QChar('%')) ||
        (str[pos] == QChar('$')))
    {
      return ;
    }
    else if (str[pos] == QChar('\n'))
    {
      if ((pos + 1) >= len ||
          str[pos + 1] == QChar('\n') ||
          str[pos + 1] == QChar('\\') ||
          str[pos + 1] == QChar('%') ||
          str[pos + 1] == QChar('$'))
        return ;

      text.append(' ');
    }
    else
      text.append(str[pos]);
    pos++;
  }
}

void XWTeXDocumentText::write(QTextStream & strm, int & linelen)
{
  if (text.isEmpty())
    return ;

  writeStr(text,strm,linelen);
}

bool XWTeXDocumentText::isSelected(int i)
{
  XWTeXDocumentCursor * cursor = doc->getCursor(this);
  return (doc->isAllSelect() || cursor->isSelected(i));
}

XWTeXDocumentComment::XWTeXDocumentComment(XWTeXDocument * docA, QObject * parent)
 : XWTeXDocumentBlock(TEX_DOC_B_COMMENT,docA,parent)
{}

QString XWTeXDocumentComment::getMimeData()
{
  QString str = QString("\\%%1\n").arg(text);
  return str;
}

void XWTeXDocumentComment::scan(const QString & str, int & len, int & pos)
{
  if (pos >= len || str[pos] != QChar('%'))
    return ;

  pos++;
  while (pos < len)
  {
    if (str[pos] == QChar('\n'))
    {
       if ((pos + 1) >= len)
         return ;

       if (str[pos + 1] == QChar('\n'))
         return ;

       if (str[pos + 1] != QChar('%'))
       {
         pos++;
         return ;
       }
       else
         pos++;

       text.append(' ');
    }
    else
      text.append(str[pos]);

    pos++;
  }
}

void XWTeXDocumentComment::setFont()
{
  setCommentFont();
}

void XWTeXDocumentComment::write(QTextStream & strm, int & linelen)
{
  int len = text.length();
  strm << "%";
  linelen++;
  for (int i = 0; i < len; i++)
  {
    if ((linelen >= doc->getLineMax()) ||
         ((linelen + 1) >= doc->getLineMax()) ||
         ((linelen + 2) >= doc->getLineMax()))
    {
      if (text[i].isSpace())
      {
        strm << "\n";
        if (i < (len - 1))
        {
          strm << "%";
          linelen = 1;
        }
        else
          linelen = 0;
      }
      else if (text[i].isPunct() ||
               (text[i].unicode() >= 0xFE30 && text[i].unicode() <= 0xFE4F) ||
               (text[i].unicode() >= 0xFE10 && text[i].unicode() <= 0xFE1F) ||
               (text[i].unicode() >= 0xF900 && text[i].unicode() <= 0xFAFF) ||
               (text[i].unicode() >= 0x4E00 && text[i].unicode() <= 0x9FBF) ||
               (text[i].unicode() >= 0x4DC0 && text[i].unicode() <= 0x4DFF) ||
               (text[i].unicode() >= 0x3400 && text[i].unicode() <= 0x4DBF) ||
               (text[i].unicode() >= 0x3300 && text[i].unicode() <= 0x33FF) ||
               (text[i].unicode() >= 0x3200 && text[i].unicode() <= 0x32FF) ||
               (text[i].unicode() >= 0x31F0 && text[i].unicode() <= 0x31FF) ||
               (text[i].unicode() >= 0x31C0 && text[i].unicode() <= 0x31EF) ||
               (text[i].unicode() >= 0x3190 && text[i].unicode() <= 0x319F) ||
               (text[i].unicode() >= 0x30A0 && text[i].unicode() <= 0x30FF) ||
               (text[i].unicode() >= 0x3040 && text[i].unicode() <= 0x309F) ||
               (text[i].unicode() >= 0x3000 && text[i].unicode() <= 0x303F) ||
               (text[i].unicode() >= 0x2FF0 && text[i].unicode() <= 0x2FFF) ||
               (text[i].unicode() >= 0x2F00 && text[i].unicode() <= 0x2FDF) ||
               (text[i].unicode() >= 0x2E80 && text[i].unicode() <= 0x2EFF))
      {
        strm << text[i];
        strm << "\n";
        if (i < (len - 1))
        {
          strm << "%";
          linelen = 1;
        }
        else
          linelen = 0;
      }
      else
      {
        strm << text[i];
        linelen++;
      }
    }
    else
    {
      strm << text[i];
      linelen++;
    }
  }

  if (linelen != 0)
  {
    strm << "\n";
    linelen = 0;
  }
}

XWTeXDocumentFormular::XWTeXDocumentFormular(XWTeXDocument * docA, QObject * parent)
 : XWTeXDocumentBlock(TEX_DOC_B_FOMULAR,docA,parent)
{
}

QString XWTeXDocumentFormular::getMimeData()
{
  if (text.isEmpty())
    return QString();

  QString str = QString("$%1$").arg(text);
  return str;
}

void XWTeXDocumentFormular::scan(const QString & str, int & len, int & pos)
{
  if (pos >= len || str[pos] != QChar('$') || (pos + 1) >= len || str[pos+1] == QChar('$'))
    return ;

  pos++;
  while (pos < len)
  {
    if (str[pos] == QChar('$'))
    {
      if (str[pos - 1] != QChar('\\'))
      {
        pos++;
        return ;
      }
      else
        text.append(str[pos]);
    }
    else if (str[pos] == QChar('\n'))
      text.append(' ');
    else
      text.append(str[pos]);
    pos++;
  }
}

void XWTeXDocumentFormular::write(QTextStream & strm, int & linelen)
{
  QString str = getMimeData();
  if (str.isEmpty())
    return ;

  writeStr(str,strm,linelen);
}

void XWTeXDocumentFormular::setFont()
{
  setMathFont();
}

XWTeXDocumentDisplayFormular::XWTeXDocumentDisplayFormular(XWTeXDocument * docA, QObject * parent)
 : XWTeXDocumentBlock(TEX_DOC_B_DISFOMULAR,docA,true, parent)
{
  afterNewRow = true;
}

void XWTeXDocumentDisplayFormular::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  double oldindent = indent;
  indent += 2.0;
  firstIndent = 0.0;
  XWTeXDocumentBlock::breakPage(curx, cury, firstcolumn);
  indent = oldindent;
}

QString XWTeXDocumentDisplayFormular::getMimeData()
{
  text = text.trimmed();
  if (text.isEmpty())
    return QString();

  QString str = QString("$$%1$$").arg(text);
  return str;
}

void XWTeXDocumentDisplayFormular::scan(const QString & str, int & len, int & pos)
{
  if (pos >= len || str[pos] != QChar('$') || (pos + 1) >= len || str[pos+1] != QChar('$'))
    return ;

  pos += 2;
  while (pos < len)
  {
    if (str[pos] == QChar('$'))
    {
      if ((pos + 1) >= len || str[pos + 1] == QChar('$'))
      {
        pos++;
        pos++;
        return ;
      }
    }

    text.append(str[pos]);
    pos++;
  }
}

void XWTeXDocumentDisplayFormular::write(QTextStream & strm, int & linelen)
{
  QString str = getMimeData();
  if (str.isEmpty())
    return ;

  if (linelen != 0)
  {
    strm << "\n";
    linelen = 0;
  }

  strm << str;

  strm << "\n";
}

void XWTeXDocumentDisplayFormular::setFont()
{
  setMathFont();
}

XWTeXControlSequence::XWTeXControlSequence(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentBlock(TEX_DOC_B_UNKNOWN,docA,parent)
{}

XWTeXControlSequence::XWTeXControlSequence(int tA,XWTeXDocument * docA, const QString & str, QObject * parent)
: XWTeXDocumentBlock(tA,docA,true, str,parent)
{}

XWTeXControlSequence::XWTeXControlSequence(XWTeXDocument * docA, bool newrow, const QString & str, QObject * parent)
: XWTeXDocumentBlock(TEX_DOC_B_CS,docA,newrow, str, parent)
{}

void XWTeXControlSequence::setFont()
{
  setControlSequenceFont();
}

XWTeXCode::XWTeXCode(int tA,XWTeXDocument * docA, const QString & nameA,QObject * parent)
: XWTeXDocumentObject(tA,docA,parent)
{
  newRow = true;
  name = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, nameA, this);
  add(new XWTeXDocumentBlock(TEX_DOC_B_VERB, docA, this));
}

void XWTeXCode::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setVerbFont();
  XWTeXDocumentBlock * b = (XWTeXDocumentBlock*)head;
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


QString XWTeXCode::getMimeData()
{
  QString str = head->getMimeData();
  str = str.trimmed();
  return str;
}

bool XWTeXCode::hasSelected()
{
  return head->hasSelected();
}

bool XWTeXCode::isAllSelected()
{
  if (isEmpty())
    return true;

  return head->isAllSelected();
}

bool XWTeXCode::isAtEnd()
{
  XWTeXDocumentObject * block = doc->getCurrentBlock();
  return (block == head && block->isAtEnd());
}

bool XWTeXCode::isAtStart()
{
  XWTeXDocumentObject * block = doc->getCurrentBlock();
  return (block == head && block->isAtStart());
}

bool XWTeXCode::isEmpty()
{
  return head->isEmpty();
}

void XWTeXCode::scanParam(const QString & str, int & len, int & pos)
{
  XWTeXDocumentBlock * b = (XWTeXDocumentBlock*)last;
  b->scanParam(str, len,pos);
}

void XWTeXCode::setFont()
{
  setVerbFont();
}

XWTeXAbstractParam::XWTeXAbstractParam(int tA,
                         XWTeXDocument * docA,
                         const QString & nameA,
                         const QString & hA,
                         const QString & eA,
                         QObject * parent)
 : XWTeXDocumentObject(tA,docA,parent)
{
  name = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,nameA,this);
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,hA,this));
  add(new XWTeXDocumentBlock(TEX_DOC_B_VERB, doc, this));
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,eA,this));
}

void XWTeXAbstractParam::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setFont();
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

void XWTeXAbstractParam::breakPage(const QFont & fontA,  const QColor & c,
                       double & curx, double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  font = fontA;
  color = c;
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

void XWTeXAbstractParam::flushBottom(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setFont();
  head->newPage = newPage;
  head->newRow = newRow;
  head->indent = indent;
  head->flushBottom(font, color, curx,cury,firstcolumn);
  XWTeXDocumentBlock * b = (XWTeXDocumentBlock*)(head->next);
  if (b->text.isEmpty())
  {
    name->newPage = false;
    name->newRow = false;
    name->indent = indent;
    QColor lcolor = color.darker(TEX_TIP_DARKER);
    name->flushBottom(font, lcolor, curx,cury,firstcolumn);
  }

  head->next->newPage = false;
  head->next->newRow = false;
  head->next->indent = indent;
  head->next->flushBottom(font, color, curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = false;
  last->indent = indent;
  last->flushBottom(font, color, curx,cury,firstcolumn);
}

QString XWTeXAbstractParam::getMimeData()
{
  QString text = head->next->getMimeData();
  text = text.trimmed();
  if (text.isEmpty())
    return QString();

  return XWTeXDocumentObject::getMimeData();
}

bool XWTeXAbstractParam::hasSelected()
{
  return head->next->hasSelected();
}

bool XWTeXAbstractParam::isAllSelected()
{
  if (isEmpty())
    return true;

  return head->next->isAllSelected();
}

bool XWTeXAbstractParam::isAtEnd()
{
  XWTeXDocumentObject * block = doc->getCurrentBlock();
  return (block == head->next && block->isAtEnd());
}

bool XWTeXAbstractParam::isEmpty()
{
  return head->next->isEmpty();
}

void XWTeXAbstractParam::setCursor()
{
  head->next->setCursor();
}

void XWTeXAbstractParam::setCursorAtEnd()
{
  head->next->setCursorAtEnd();
}

void XWTeXAbstractParam::setCursorAtStart()
{
  head->next->setCursorAtStart();
}

void XWTeXAbstractParam::setFont()
{
  setOptionFont();
}

void XWTeXAbstractParam::write(QTextStream & strm, int & linelen)
{
  QString str = head->next->getMimeData();
  str = str.trimmed();
  if (str.isEmpty())
    return ;

  XWTeXDocumentObject::write(strm, linelen);
}

XWTeXCoord::XWTeXCoord(int tA, XWTeXDocument * docA, const QString & nameA, QObject * parent)
: XWTeXAbstractParam(tA,docA,nameA, "(",")",parent)
{}

void XWTeXCoord::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentBlock * b = (XWTeXDocumentBlock*)(head->next);
  b->scanCoord(str, len, pos);
}

void XWTeXCoord::setFont()
{
  setVerbFont();
}

XWTeXDocumentOption::XWTeXDocumentOption(int tA, XWTeXDocument * docA, const QString & nameA, QObject * parent)
 : XWTeXAbstractParam(tA,docA,nameA, "[","]",parent)
{
}

void XWTeXDocumentOption::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentBlock * b = (XWTeXDocumentBlock*)(head->next);
  b->scanOption(str, len,pos);
}

XWTeXDocumentParam::XWTeXDocumentParam(int tA, XWTeXDocument * docA, const QString & nameA, QObject * parent)
 : XWTeXAbstractParam(tA,docA,nameA, "{","}",parent)
{
}

void XWTeXDocumentParam::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentBlock * b = (XWTeXDocumentBlock*)(head->next);
  b->scanParam(str, len,pos);
}

void XWTeXDocumentParam::setFont()
{
  setParamFont();
}

void XWTeXDocumentParam::write(QTextStream & strm, int & linelen)
{
  XWTeXDocumentObject::write(strm, linelen);
}

XWTeXOptionParam::XWTeXOptionParam(XWTeXDocument * docA, const QString & nameA, QObject * parent)
: XWTeXAbstractParam(TEX_DOC_OPTION,docA,nameA, "{","}",parent)
{}

void XWTeXOptionParam::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentBlock * b = (XWTeXDocumentBlock*)(head->next);
  b->scanParam(str, len,pos);
}

XWTeXDocumentBeamerSpec::XWTeXDocumentBeamerSpec(XWTeXDocument * docA, const QString & nameA, QObject * parent)
: XWTeXAbstractParam(TEX_DOC_BEAMER_SPEC,docA,nameA, "<",">",parent)
{
}

void XWTeXDocumentBeamerSpec::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentBlock * b = (XWTeXDocumentBlock*)(head->next);
  b->scanBeamerSpec(str, len,pos);
}

XWBeamerDefaultSpec::XWBeamerDefaultSpec(XWTeXDocument * docA, const QString & nameA, QObject * parent)
: XWTeXAbstractParam(TEX_DOC_BEAMER_SPEC,docA,nameA, "[<",">]",parent)
{
}

void XWBeamerDefaultSpec::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos >= len || str[pos] != QChar('['))
    return ;

  pos++;
  if (pos >= len || str[pos] != QChar('<'))
    return ;

  XWTeXDocumentBlock * b = (XWTeXDocumentBlock*)(head->next);
  b->scanBeamerSpec(str, len,pos);

  while (pos < len && str[pos].isSpace())
    pos++;

  pos++;
}

XWTeXDocumentCommand::XWTeXDocumentCommand(int tA, XWTeXDocument * docA,
                     bool newrow,
                     const QString & headA,
                     QObject * parent)
: XWTeXDocumentObject(tA,docA,parent)
{
   newRow = newrow;
   head = new XWTeXControlSequence(docA, newrow, headA, this);
   head->isOnlyRead = true;
   last = head;
}

XWTeXDocumentCommand::~XWTeXDocumentCommand()
{}

bool XWTeXDocumentCommand::hasSelected()
{
  return last->hasSelected();
}

bool XWTeXDocumentCommand::isAllSelected()
{
  return head->next->isAllSelected() && last->isAllSelected();
}

bool XWTeXDocumentCommand::isAtEnd()
{
  return last->isAtEnd();
}

bool XWTeXDocumentCommand::isAtStart()
{
  XWTeXDocumentObject * block = doc->getCurrentBlock();
  return (block == head && block->isAtStart());
}

bool XWTeXDocumentCommand::isEmpty()
{
  return head->next->isEmpty() && last->isEmpty();
}

void XWTeXDocumentCommand::setCursor()
{
  last->setCursor();
}

void XWTeXDocumentCommand::setCursorAtEnd()
{
  last->setCursorAtEnd();
}

void XWTeXDocumentCommand::setCursorAtStart()
{
  last->setCursorAtStart();
}

XWTeXOptionCommand::XWTeXOptionCommand(int tA, XWTeXDocument * docA,
                     bool newrow,
                     const QString & hA,
                     const QString & oA,
                     QObject * parent)
: XWTeXDocumentCommand(tA,docA,newrow, hA,parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_PARAM, docA,oA, this));
}

XWTeXParamCommand::XWTeXParamCommand(int tA, XWTeXDocument * docA,
                     bool newrow,
                     const QString & hA,
                     const QString & pA,
                     QObject * parent)
: XWTeXDocumentCommand(tA,docA,newrow,hA,parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,pA, this));
}

XWBeamerSpecCommand::XWBeamerSpecCommand(int tA, XWTeXDocument * docA,
                     bool newrow,
                     const QString & hA,
                     QObject * parent)
: XWTeXDocumentCommand(tA,docA,newrow,hA,parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
}

XWTeXOptionParamCommand::XWTeXOptionParamCommand(int tA, XWTeXDocument * docA,
                     const QString & hA,
                     const QString & oA,
                     const QString & pA,
                     QObject * parent)
: XWTeXDocumentCommand(tA,docA,false,hA,parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_PARAM, docA,oA, this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,pA, this));
}

XWBeamerSpecParamCommand::XWBeamerSpecParamCommand(int tA, XWTeXDocument * docA,
                     const QString & hA,
                     const QString & pA,
                     QObject * parent)
: XWTeXDocumentCommand(tA,docA,false,hA,parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,pA, this));
}

XWBeamerSpecOptionCommand::XWBeamerSpecOptionCommand(int tA, XWTeXDocument * docA,
                     const QString & hA,
                     QObject * parent)
: XWTeXDocumentCommand(tA,docA,true,hA,parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWTeXDocumentOption(TEX_DOC_PARAM, docA,tr("options:"), this));
}

XWTeXDocumentUnkownEnviroment::XWTeXDocumentUnkownEnviroment(XWTeXDocument * docA,
                     const QString & nameA,
                     QObject * parent)
  :XWTeXDocumentObject(TEX_DOC_ENV,docA,parent),
   name(nameA)
{
  QString cs = QString("\\begin{%1}").arg(name);
  head = new XWTeXControlSequence(docA, true, cs, this);
  head->isOnlyRead = true;
  cs = QString("\\end{%1}").arg(name);
  last = new XWTeXControlSequence(docA, true, cs, this);
  head->next = last;
  last->prev = head;
  last->isOnlyRead = true;
  newRow = true;
  afterNewRow = true;
}

XWTeXDocumentUnkownEnviroment::~XWTeXDocumentUnkownEnviroment()
{}

void XWTeXDocumentUnkownEnviroment::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  double oldindent = indent;
  if (doc->isTwocolumn())
    indent += 2.0;
  else
    indent += 4.0;

  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);

  XWTeXDocumentObject * obj = head->next;
  while (obj)
  {
    if (obj == last)
      break;

    obj->newRow = true;
    obj->indent = indent + 2.0;
    obj->breakPage(curx,cury,firstcolumn);
    obj = obj->next;
  }

  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

void XWTeXDocumentUnkownEnviroment::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  XWTeXDocumentObject * obj = 0;
  if (pos < len && str[pos] == QChar('['))
  {
    obj = new XWTeXDocumentOption(TEX_DOC_OPTION,doc,tr("option:"),this);
    last->insertAtBefore(obj);
    obj->scan(str,len,pos);
    while (pos < len && str[pos].isSpace())
      pos++;
  }

  if (pos < len && str[pos] == QChar('['))
  {
    obj = new XWTeXDocumentOption(TEX_DOC_OPTION,doc,tr("option:"),this);
    last->insertAtBefore(obj);
    obj->scan(str,len,pos);
    while (pos < len && str[pos].isSpace())
      pos++;
  }

  if (pos < len && str[pos] == QChar('{'))
  {
    obj = new XWTeXDocumentParam(TEX_DOC_PARAM,doc,tr("param:"),this);
    last->insertAtBefore(obj);
    obj->scan(str,len,pos);
    while (pos < len && str[pos].isSpace())
      pos++;
  }

  if (pos < len && str[pos] == QChar('{'))
  {
    obj = new XWTeXDocumentParam(TEX_DOC_PARAM,doc,tr("param:"),this);
    last->insertAtBefore(obj);
    obj->scan(str,len,pos);
    while (pos < len && str[pos].isSpace())
      pos++;
  }

  XWTeXDocumentBlock * block = new XWTeXDocumentBlock(TEX_DOC_B_VERB,doc,this);
  last->insertAtBefore(block);
  block->newRow = true;
  int b = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('\\'))
    {
      int p = pos;
      QString key = scanControlSequence(str,len,pos);
      if (key == "end")
      {
        key = scanEnviromentName(str,len,pos);
        if (key == name)
        {
          b--;
          if (b < 0)
            return ;
        }
      }
      else if (key == "begin")
      {
        key = scanEnviromentName(str,len,pos);
        if (key == name)
          b++;
      }

      key = str.mid(p,pos - p);
      block->text.append(key);
    }
    else
    {
      block->text.append(str[pos]);
      pos++;
    }
  }
}
