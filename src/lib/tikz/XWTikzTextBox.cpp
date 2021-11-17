/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include "LaTeXKeyWord.h"
#include "XWLaTeXPool.h"
#include "PGFKeyWord.h"
#include "XWPGFPool.h"
#include "XWTeXText.h"
#include "XWTikzGraphic.h"
#include "XWTikzState.h"
#include "XWTikzNode.h"
#include "XWTikzTextBox.h"

XWTikzTextBox::XWTikzTextBox(XWTikzGraphic * graphicA,QObject * parent)
:XWTeXTextBox(parent),
graphic(graphicA),
state(0)
{
}

XWTikzTextBox::~XWTikzTextBox()
{
}

QString XWTikzTextBox::getText()
{
   QString ret;
  for (int i = 0; i < parts.size(); i++)
  {
    QString tmp = parts[i]->getText();
    ret += tmp;
    if (i < nodePart.size())
    {
      tmp = nodePart[i]->getText();
      ret += tmp;
    }
  }

  return ret;
}

void XWTikzTextBox::push(QUndoCommand * cmdA)
{
  graphic->push(cmdA);
}

void XWTikzTextBox::scan(const QString & str, int & len, int & pos)
{
  cur = 0;
  XWTeXTextBoxPart * part = new XWTeXTextBoxPart(this,this);
  parts << part;

  XWTeXText * obj = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('}') )
    {
      pos++;
      break;
    }
    else if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
      skipComment(str,len,pos);
    else
    {
      if (str[pos] == QChar('$') )
      {
        int id = XW_TEX_FOMULAR;
        pos++;
        if (str[pos+1] == QChar('$') )
        {
          id = XW_TEX_DISFOMULAR;
          pos++;
        }
        obj = new XWTeXText(id,this,0);
      }
      else if (str[pos] == QChar('\\'))
      {
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        if (id == LAbegin)
        {
          key = scanEnviromentName(str,len,pos);
          id = lookupLaTeXID(key);
          obj = new XWTeXText(id,this,0);
        }
        else if (id > 0)
          obj = new XWTeXText(id,this,0);
        else
        {
          id = lookupPGFID(key);
          if (id == PGFnodepart)
          {
            XWTikzNodePart * np = new XWTikzNodePart(graphic,this);
            nodePart << np;
            cur++;
          }
          continue;
        }
      }
      else if (str[pos] == QChar('+') || 
               str[pos] == QChar('-') || 
               str[pos] == QChar('=') ||
               str[pos] == QChar('*') ||
               str[pos] == QChar(';') ||
               str[pos] == QChar('!') ||
               str[pos] == QChar(':') ||
               str[pos] == QChar('?') )
      {
        obj = new XWTeXText(XW_TEX_ASE,this,0);
      }        
      else if (str[pos] == QChar('.') || str[pos].isDigit())
        obj = new XWTeXText(XW_TEX_DIGIT,this,0);
      else if (str[pos].isLetter())
        obj = new XWTeXText(XW_TEX_LETTER,this,0);
      else
        obj = new XWTeXText(XW_TEX_TEXT,this,0);

      parts[cur]->append(obj);
      
      obj->scan(str,len,pos);
    }    
  }
}

void XWTikzTextBox::setState(XWTikzState * stateA)
{
  if (!state)
    new XWTikzState(false,this);

  stateA->copy(state);
}

void XWTikzTextBox::updateCursor(double minx, double miny, double maxx,
                    double maxy, int textposA)
{
  if (cur >= 0)
    parts[cur]->setTextPos(textposA);
  QPointF p1(minx,miny);
  QPointF p2(maxx,maxy);
  if (state)
  {
    p1 = state->map(p1);
    p2 = state->map(p2);
  }
  graphic->updateCursor(p1.x(),p1.y(),p2.x(),p2.y());
}
