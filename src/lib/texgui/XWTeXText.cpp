/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QApplication>
#include <QClipboard>
#include <QFont>
#include <QFontMetricsF>
#include <QPainter>
#include "XWFTFont.h"
#include "XWTFMFile.h"
#include "LaTeXKeyWord.h"
#include "XWLaTeXPool.h"
#include "XWPDFDriver.h"
#include "XWTeXTextBox.h"
#include "XWTeXText.h"

XWTeXText::XWTeXText(int keywordA,XWTeXTextBox * boxA,QObject * parent)
:QObject(parent),
 keyWord(keywordA),
 box(boxA),
 head(0),
 last(0),
 next(0),
 prev(0),
 x(0),
 y(0),
 width(0),
 height(0),
 depth(0),
 family(XW_TEX_FONT_CMR),
 family2(0),
 fontSize(XW_TEX_FONT_NORMAL),
 textPos(0),
 anchorPos(0),
 bold(false),
 slant(false),
 italic(false),
 cal(false),
 sc(false),
 sf(false),
 width2(0),
 height2(0),
 depth2(0),
 align(0)
{
  switch (keyWord)
  {
    case LAfrac:
    case LAstackrel:
      append(new XWTeXText(XW_TEX_PARAM,box,this));
      append(new XWTeXText(XW_TEX_PARAM,box,this));
      break;

    case LAsqrt:
      append(new XWTeXText(XW_TEX_OPT,box,this));
      append(new XWTeXText(XW_TEX_PARAM,box,this));
      head->fontSize = XW_TEX_FONT_SCRIPT;
      break;

    case LAoverbrace:
    case LAoverline:
    case LAunderbrace:
    case LAunderline:        
    case LAhat:
    case LAcheck:
    case LAtilde:
    case LAgrave:
    case LAdot:
    case LAddot:
    case LAbar:
    case LAvec:
    case LAwidehat:
    case LAacute:
    case LAbreve:
    case LAwidetilde:
    case LAoverrightarrow:
    case LAunderrightarrow:
    case LAoverleftarrow:
    case LAunderleftarrow:
    case LAoverleftrightarrow:
    case LAunderleftrightarrow:
    case LAarray:
      append(new XWTeXText(XW_TEX_PARAM,box,this));
      break;

    case LAmathrm:
    case LArm:
      family = XW_TEX_FONT_CMR;
      break;

    case LAmathbf:
    case LAtextbf:
    case LAbf:
      bold = true;
      break;

    case LAmathsf:
    case LAtextsf:
    case LAsf:
      sf = true;
      break;

    case LAtextsl:
    case LAsl:
      slant = true;
      break;

    case LAmathit:
    case LAtextit:
    case LAit:
    case LAem:
      italic = true;
      break;

    case LAmathtt:
    case LAtexttt:
    case LAtt:
      family = XW_TEX_FONT_CMTT;
      break;

    case LAmathcal:
    case LAcal:
      cal = true;
      break;

    case LAtextsc:
    case LAsc:
      sc = true;
      break;

    case XW_TEX_SUB:
    case XW_TEX_SUP:
      fontSize = XW_TEX_FONT_SCRIPT;
      break;

    case LAtiny:
      fontSize = XW_TEX_FONT_TINY;
      break;

    case LAscriptsize:
      fontSize = XW_TEX_FONT_SCRIPT;
      break;

    case LAfootnotesize:
      fontSize = XW_TEX_FONT_NOTE;
      break;

    case LAsmall:
      fontSize = XW_TEX_FONT_SMALL;
      break;

    case LAnormalsize:
      fontSize = XW_TEX_FONT_NORMAL;
      break;

    case LAlarge:
      fontSize = XW_TEX_FONT_large;
      break;

    case LALarge:
      fontSize = XW_TEX_FONT_Large;
      break;

    case LALARGE:
      fontSize = XW_TEX_FONT_LARGE;
      break;

    case LAhuge:
      fontSize = XW_TEX_FONT_huge;
      break;

    case LAHuge:
      fontSize = XW_TEX_FONT_Huge;
      break;

    default:
      break;
  }
}

void XWTeXText::append(const QString & t)
{
  text.append(t);
}

void XWTeXText::append(XWTeXText*obj)
{
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
    last->next = 0;
  }
}

void XWTeXText::breakLine(double maxW, double & curw)
{
  XWTeXTextRow * row = box->getCurrentRow();
  switch (keyWord)
  {
    case XW_TEX_TEXT:
      if ((curw + width) <= maxW)
      {
        curw += width;
        row->add(this);
        row->width = curw;
        row->height = qMax(row->height, height);
        row->depth = qMax(row->depth, depth);
      }
      else
      {
        QFont font = QApplication::font();
        font.setBold(bold);
        font.setItalic(italic);
        font.setPointSize(fontSize);
        if (slant)
          font.setStyle(QFont::StyleOblique);
        if (sc)
          font.setCapitalization(QFont::SmallCaps);
        if (sf)
          font.setStyleHint(QFont::SansSerif);
        QFontMetricsF metrics(font);
        if (text.length() <= 0)
        {
          curw += width;
          row->add(this);
        }
        for (int i = 0; i < text.length(); i++)
        {
          double w = metrics.width(text[i]);
          if ((curw + w) > maxW)
          {
            if (i > 0)
            {
              row->add(this);
              row->setEndPos(i);
              row->width = curw;
              row->height = qMax(row->height, height);
              row->depth = qMax(row->depth, depth);
            }
            
            row = box->getNewRow();
            row->add(this);
            row->setStartPos(i);
            curw = 0;
          }

          curw += w;
        }
        row->setEndPos(text.length());
        row->width = curw;
        row->height = qMax(row->height, height);
        row->depth = qMax(row->depth, depth);
      }
      break;

    case XW_TEX_LETTER:
    case XW_TEX_DIGIT:
    case XW_TEX_UNKNOWN:
      if ((curw + width) <= maxW)
      {
        curw += width;
        row->add(this);
        row->width = curw;
        row->height = qMax(row->height, height);
        row->depth = qMax(row->depth, depth);
      }
      else
      {
        QString fontname = getFontName();
        QByteArray tfmname = fontname.toAscii();
        XWTFMFile tfm(tfmname.constData());
        if (text.length() <= 0)
        {
          curw += width;
          row->add(this);
        }
        for (int i = 0; i < text.length(); i++)
        {
          double w = tfm.getWidth(text[i].unicode());
          if ((curw + w) > maxW)
          {
            if (i > 0)
            {
              row->add(this);
              row->setEndPos(i);
              row->width = curw;
              row->height = qMax(row->height, height);
              row->depth = qMax(row->depth, depth);
            }
            
            row = box->getNewRow();
            row->add(this);
            row->setStartPos(i);
            curw = 0;
          }

          curw += w;
        }

        row->setEndPos(text.length());
        row->width = curw;
        row->height = qMax(row->height, height);
        row->depth = qMax(row->depth, depth);
      }
      break;

    case LAlinebreak:
      row = box->getNewRow();
      curw = 0;
      break;

    default:
      if ((curw + width + 4) > maxW)
      {
        row = box->getNewRow();
        curw = width;
        row->width = curw;
        row->height = height;
        row->depth = depth;        
      }
      else
      {
        curw += width;
        row = box->getCurrentRow();
        row->width = curw;
        row->height = qMax(row->height, height);
        row->depth = qMax(row->depth, depth);
      }   
      row->add(this);
      break;
  }
}

bool XWTeXText::contains(double xA, double yA)
{
  return (xA >= x && xA < (x + width) && yA <= (y + height) && yA >= (y - depth));
}

bool XWTeXText::cut()
{
  if (keyWord != XW_TEX_LETTER && 
      keyWord != XW_TEX_TEXT &&
      keyWord != XW_TEX_DIGIT)
    return false;

  if (!hasSelect())
    return false;

  XWTeXRemoveString * cmd = 0;
  QString str;
  if (textPos < anchorPos)
  {
    str = text.mid(textPos,anchorPos-textPos);
    cmd = new XWTeXRemoveString(this,textPos,anchorPos);
  }
  else
  {
    str = text.mid(textPos,textPos - anchorPos);
    cmd = new XWTeXRemoveString(this,anchorPos,textPos);
  }
  QApplication::clipboard()->setText(str);
  box->push(cmd);
  return true;
}

QChar XWTeXText::deleteChar(int pos)
{
  QChar c;
  if (pos >= 0 && pos < text.length())
  {
    textPos = pos;
    anchorPos = textPos;
    c = text[pos];
    text.remove(pos, 1);
  }
  return c;
}

void XWTeXText::doText(XWPDFDriver * driver, double xA, double yA,int s, int e)
{
  if (!driver)
    return ;

  QString fontname = getFontName();
  switch (keyWord)
  {
    case XW_TEX_TEXT:
    case XW_TEX_UNKNOWN:
    case XW_TEX_LETTER:
    case XW_TEX_DIGIT:
      if (!text.isEmpty())
      {
        if (s == 0 && e == text.length())
        {
          driver->setFont(fontname,(double)fontSize, true);
          driver->setString(x,y,text);
        }
        else
        {
          if (s == 0)
            driver->setFont(fontname,(double)fontSize, true);

          QString tmp = text.mid(s,e - s);
          driver->setString(xA,yA,tmp);
        }
      }
      doChildren(driver,xA,yA,s,e);
      break;

    case LAarccos:
    case LAarcsin:
    case LAarctan:
    case LAarg:    
    case LAcos:
    case LAcosh:
    case LAcot:
    case LAcoth:
    case LAcsc:
    case LAdeg:
    case LAdet:
    case LAdim:
    case LAexp:
    case LAgcd:
    case LAhom:    
    case LAker:
    case LAlg:    
    case LAln:
    case LAlog:
    case LAmax:
    case LAmin:    
    case LAPr:
    case LAsec:
    case LAsin:
    case LAsinh:    
    case LAtan:
    case LAtanh:
    case LApmod:
    case LAbmod:
    case LAsup:
    case LAinf:
    case LAlim:
    case LAoverbrace:
    case LAunderbrace:  
    case LAlongmapsto:
    case LAdashleftarrow:
    case LAdashrightarrow:
    case LAcdots:
    case LAdots:
      driver->setFont(fontname,(double)fontSize, false);
      driver->setString(x,y,text);
      doChildren(driver,xA,yA,s,e);
      break;

    case LAliminf:
    case LAlimsup:
      driver->setFont(fontname,(double)fontSize, false);
      {
        QByteArray tfmname = fontname.toAscii();
        XWTFMFile tfm(tfmname.constData());
        double tmpx = x;
        for (int i = 0; i < 3; i++)
        {
          double w = tfm.getWidth(text[i].unicode());  
          tmpx += w;
        }
        QString txt1 = text.left(3);
        driver->setString(x,y,txt1);
        QString txt2 = text.right(3);
        driver->setString(tmpx + 2,y,txt2);
        doChildren(driver,xA,yA,s,e);
      }
      break;

    case LAarray:
      {
        XWTeXText * obj = head->next;
        while (obj)
        {
          obj->doChildren(driver,xA,yA,s,e);
          obj = obj->next;
        }
      }
      break;

    case XW_TEX_FOMULAR:
    case XW_TEX_DISFOMULAR:
    case LAeqnarray:
    case LAeqnarrayStar:
    case XW_TEX_ROW:
    case XW_TEX_COL:    
    case XW_TEX_PARAM:
    case XW_TEX_OPT:
    case LAequation:
    case LAmathrm:
    case LAmathbf:
    case LAmathsf:
    case LAmathtt:
    case LAmathit:
    case LAmathcal:
    case LAmathnormal:
    case XW_TEX_SUB:
    case XW_TEX_SUP:
    case LAstackrel:
    case LAtextit:
	  case LAtextsl:
	  case LAtextbf:
	  case LAtextsf:
	  case LAtextsc:
	  case LAtexttt:
	  case LAtextmd:
	  case LAtextup:
	  case LAtextrm:
	  case LAtextnormal:
    case LAem:
	  case LAit:
	  case LAsl:
	  case LAbf:
	  case LAsf:
	  case LAsc:
	  case LAtt:
	  case LAcal:
	  case LArm:
    case LAtiny:
	  case LAscriptsize:
	  case LAfootnotesize:
	  case LAsmall:
	  case LAnormalsize:
	  case LAlarge:
	  case LALarge:
	  case LALARGE:
	  case LAhuge:
	  case LAHuge:
      doChildren(driver,xA,yA,s,e);
      break;

    case LAfrac:
      doChildren(driver,xA,yA,s,e);
      driver->moveTo(x,y - head->depth - 2);
      driver->lineTo(x + width, y - head->depth - 2);
      break;

    case LAsqrt:
      {
        doChildren(driver,xA,yA,s,e);
        driver->moveTo(x + width2,y + head->height + 1);
        driver->lineTo(x + width2 + head->width,y + head->height + 1);
        driver->setFont(fontname,(double)fontSize, false);  
        driver->gsave();      
        double sy = (height + depth + 1) / (height2 + depth2);
        driver->setTextMatrix(1,0,0,sy,0,0);
        driver->setChar(x,y,text[0]);
        driver->grestore();
      }
      break;

    case LAoverleftrightarrow:
      doChildren(driver,xA,yA,s,e);
      driver->setFont(fontname,(double)fontSize, false); 
      {
        QByteArray tfmname = fontname.toAscii();
        XWTFMFile tfm(tfmname.constData());
        double w2 = tfm.getWidth(text[0].unicode());
        double d2 = tfm.getDepth(text[0].unicode());
        driver->setChar(x,y + head->height + d2,text[0]);
        driver->setChar(x + w2,y + head->height + d2,text[1]);
      }
      break;

    case LAunderleftrightarrow:
      doChildren(driver,xA,yA,s,e);
      driver->setFont(fontname,(double)fontSize, false);
      {
        QByteArray tfmname = fontname.toAscii();
        XWTFMFile tfm(tfmname.constData());
        double w2 = tfm.getWidth(text[0].unicode());
        driver->setChar(x,y - head->depth,text[0]);
        driver->setChar(x + w2,y - head->depth,text[1]);
      }
      break;

    case LAoverline:  
      doChildren(driver,xA,yA,s,e);
      driver->moveTo(x,y + head->height);
      driver->lineTo(x + width,y + head->height);
      break;

    case LAunderline:
      doChildren(driver,xA,yA,s,e);
      driver->moveTo(x,y - head->depth);
      driver->lineTo(x + width,y - head->depth);
      break;

    case LAhat: 
    case LAcheck: 
    case LAtilde:
    case LAgrave:
    case LAdot:
    case LAddot: 
    case LAbar:  
    case LAvec:
    case LAwidehat:
    case LAacute:   
    case LAbreve:  
    case LAwidetilde:
      break;

    case LAdoteq:
      doChildren(driver,xA,yA,s,e);
      driver->setFont(fontname,(double)fontSize, false);
      {
        QByteArray tfmname = fontname.toAscii();
        XWTFMFile tfm(tfmname.constData());
        double d = tfm.getDepth(text[0].unicode());
        double h = tfm.getHeight(text[1].unicode());
        driver->setChar(x,y + h + d,text[0]);
        driver->setChar(x,y,text[1]);
      }
      break;

    case LAcong:
      break;

    case LAbowtie:    
    case LAlongleftarrow:
    case LAlongrightarrow:
    case LAlongleftrightarrow:
    case LALongleftarrow:
    case LALongrightarrow:
    case LALongleftrightarrow:
    case LAmapsto:    
    case LAiff:
      doChildren(driver,xA,yA,s,e);
      driver->setFont(fontname,(double)fontSize, false); 
      {
        QByteArray tfmname = fontname.toAscii();
        XWTFMFile tfm(tfmname.constData());
        double w2 = tfm.getWidth(text[0].unicode());
        driver->setChar(x,y,text[0]);
        driver->setChar(x + w2,y,text[1]);
      }
      break;

    case LAhookleftarrow:
    case LAhookrightarrow:
    case LAmodels:
      break;

    case LAnotin:
      doChildren(driver,xA,yA,s,e);
      driver->setFont(fontname,(double)fontSize, false); 
      driver->setChar(x,y,text[0]);
      driver->setChar(x,y,text[1]);
      break;

    case LAneq:
      driver->setFont(fontname,(double)fontSize, false); 
      driver->setChar(x,y,text[0]);
      fontname = getFontName(family2,fontSize,bold,slant,italic,cal);
      driver->setFont(fontname,(double)fontSize, false); 
      driver->setChar(x,y,text[1]);
      break;

    case LAleft:
      {
        driver->gsave();
        double sy = 1;
        if (text.isEmpty())
        {
          if (next)
            sy = (next->height + next->depth) / (head->height + head->depth);
     
          driver->setTextMatrix(1,0,0,sy,0,0);
          head->doText(driver,xA,yA,s,e);     
          driver->grestore(); 
        }
        else
        {
          if (next)
            sy = (next->height + next->depth) / (height2 + depth2);

          driver->setTextMatrix(1,0,0,sy,0,0);
          driver->setChar(x,y,text[0]);
          driver->grestore(); 
          doChildren(driver,xA,yA,s,e);
        } 
      }
      break;

    case LAright: 
      {
        driver->gsave();
        double sy = 1;
        if (text.isEmpty())
        {
          if (prev)
            sy = (prev->height + prev->depth) / (head->height + head->depth);
          driver->setTextMatrix(1,0,0,sy,0,0);
          head->doText(driver,xA,yA,s,e); 
          driver->grestore(); 
        }
        else
        {
          driver->setFont(fontname,(double)fontSize, false);
          if (prev)
            sy = (prev->height + prev->depth) / (height2 + depth2);

          driver->setTextMatrix(1,0,0,sy,0,0);
          if (text[0] != QChar('.'))
            driver->setChar(x,y,text[0]);
          driver->grestore(); 
          doChildren(driver,xA,yA,s,e);
        }  
      }
      break;

    case LAddots:
      driver->setFont(fontname,(double)fontSize, false); 
      {
        QByteArray tfmname = fontname.toAscii();
        XWTFMFile tfm(tfmname.constData());
        double w2 = tfm.getWidth(text[0].unicode());
        double h2 = tfm.getHeight(text[0].unicode());
        double d2 = tfm.getDepth(text[0].unicode());
        driver->setChar(x,y + 2 * h2 + 2 * d2,text[0]);
        driver->setChar(x + w2,y + h2 + d2,text[0]);
        driver->setChar(x + 2 * w2, y,text[0]);
      }
      doChildren(driver,xA,yA,s,e);
      break;

    case LAvdots:
      driver->setFont(fontname,(double)fontSize, false); 
      {
        QByteArray tfmname = fontname.toAscii();
        XWTFMFile tfm(tfmname.constData());
        double h2 = tfm.getHeight(text[0].unicode());
        double d2 = tfm.getDepth(text[0].unicode());
        driver->setChar(x,y,text[0]);
        driver->setChar(x,y + h2 + d2,text[0]);
        driver->setChar(x,y + (2 * h2 + 2 * d2),text[0]);
      }
      doChildren(driver,xA,yA,s,e);
      break;

    default:
      driver->setFont(fontname,(double)fontSize, false);
      driver->setChar(x,y,text[0]);
      doChildren(driver,xA,yA,s,e);
      break;
  }
}

void XWTeXText::draw(QPainter * painter,const QRectF & r)
{
  QRectF rr(x,y - height,width,height);
  if (rr.intersects(r))
    draw(painter);
}

void XWTeXText::draw(QPainter * painter)
{
  switch (keyWord)
  {
    case XW_TEX_TEXT:
      drawNormalText(painter);
      break;

    case XW_TEX_UNKNOWN:
    case XW_TEX_LETTER:
    case XW_TEX_DIGIT:
    case LAarccos:
    case LAarcsin:
    case LAarctan:
    case LAarg:    
    case LAcos:
    case LAcosh:
    case LAcot:
    case LAcoth:
    case LAcsc:
    case LAdeg:
    case LAdet:
    case LAdim:
    case LAexp:
    case LAgcd:
    case LAhom:    
    case LAker:
    case LAlg:    
    case LAln:
    case LAlog:
    case LAmax:
    case LAmin:    
    case LAPr:
    case LAsec:
    case LAsin:
    case LAsinh:    
    case LAtan:
    case LAtanh:
    case LApmod:
    case LAbmod:
    case LAsup:
    case LAinf:
    case LAlim:
      drawText(painter);
      break;

    case LAliminf:
    case LAlimsup:
      drawText(painter, 3);
      break;

    case LAarray:
      {
        XWTeXText * obj = head->next;
        while (obj)
        {
          obj->draw(painter);
          obj = obj->next;
        }
      }
      break;

    case XW_TEX_FOMULAR:
    case XW_TEX_DISFOMULAR:
    case LAeqnarray:
    case LAeqnarrayStar:
    case XW_TEX_ROW:
    case XW_TEX_COL:    
    case XW_TEX_PARAM:
    case XW_TEX_OPT:
    case LAequation:
    case LAmathrm:
    case LAmathbf:
    case LAmathsf:
    case LAmathtt:
    case LAmathit:
    case LAmathcal:
    case LAmathnormal:
    case XW_TEX_SUB:
    case XW_TEX_SUP:
    case LAstackrel:
    case LAtextit:
	  case LAtextsl:
	  case LAtextbf:
	  case LAtextsf:
	  case LAtextsc:
	  case LAtexttt:
	  case LAtextmd:
	  case LAtextup:
	  case LAtextrm:
	  case LAtextnormal:
    case LAem:
	  case LAit:
	  case LAsl:
	  case LAbf:
	  case LAsf:
	  case LAsc:
	  case LAtt:
	  case LAcal:
	  case LArm:
    case LAtiny:
	  case LAscriptsize:
	  case LAfootnotesize:
	  case LAsmall:
	  case LAnormalsize:
	  case LAlarge:
	  case LALarge:
	  case LALARGE:
	  case LAhuge:
	  case LAHuge:
      drawChildren(painter);
      break;

    case LAfrac:
      drawChildren(painter);
      painter->drawLine(x,y - head->depth - 2,x + width, y - head->depth - 2);
      break;

    case LAsqrt:
      {
        drawChildren(painter);
        painter->drawLine(x + width2,y + head->height + 1,x + width2 + head->width,y + head->height + 1); 
        XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
        double sy = (height + depth + 1) / (height2 + depth2);
        painter->save();
        painter->scale(1,sy);
        font->drawChar(painter,x,y + height2,text[0].unicode());
        painter->restore();
      }
      break;

    case LAoverleftrightarrow:
      drawOverTwoChars(painter);
      break;

    case LAunderleftrightarrow:
      drawUnderTwoChars(painter);
      break;

    case LAoverbrace:
      drawOverBrace(painter);
      break;

    case LAunderbrace:  
      drawUnderBrace(painter);
      break;

    case LAhat: 
    case LAcheck: 
    case LAtilde:
    case LAgrave:
    case LAdot:
    case LAddot: 
    case LAbar:  
    case LAvec:
    case LAwidehat:
    case LAacute:   
    case LAbreve:  
    case LAwidetilde:
      drawAccent(painter);
      break;

    case LAoverline:  
      drawChildren(painter);
      painter->drawLine(x,y + head->height,x + width,y + head->height);
      break;

    case LAunderline:
      drawChildren(painter);
      painter->drawLine(x,y - head->depth,x + width,y - head->depth);
      break;

    case LAdoteq:    
      drawTwoCharsTB(painter);
      break;

    case LAcong:
      drawTwoTwoCharsTB(painter);
      break;

    case LAbowtie:    
    case LAlongleftarrow:
    case LAlongrightarrow:
    case LAlongleftrightarrow:
    case LALongleftarrow:
    case LALongrightarrow:
    case LALongleftrightarrow:
    case LAmapsto:    
    case LAiff:
      drawTwoCharsLR(painter);
      break;

    case LAhookleftarrow:
    case LAhookrightarrow:
    case LAmodels:
      drawHookArrow(painter);
      break;

    case LAnotin:
      drawNotin(painter);
      break;

    case LAneq:
      drawNoteq(painter);
      break;

    case LAlongmapsto:
    case LAdashleftarrow:
    case LAdashrightarrow:
      drawThreeCharsLR(painter);
      break;

    case LAleft:
      drawLeft(painter);
      break;

    case LAright:
      drawRight(painter);
      break;

    case LAcdots:
    case LAdots:
      drawDotsLR(painter);
      break;

    case LAddots:
      drawDDots(painter);
      break;

    case LAvdots:
      drawVDots(painter);
      break;

    default:
      drawChar(painter);
      break;
  }
}

void XWTeXText::dragTo(XWPDFDriver * driver, double xA, double yA)
{
  if (!hasSelect())
    return ;

  QString str = getSelectedText();
  switch (keyWord)
  {
    case XW_TEX_TEXT:
    case XW_TEX_LETTER:
    case XW_TEX_DIGIT:
      {
        QString fontname = getFontName();
        driver->setFont(fontname,(double)fontSize, true);
        driver->setString(xA,yA,str);
      }
      break;

    default:
      break;
  }
}

void XWTeXText::dragTo(QPainter * painter, double xA, double yA)
{
  if (!hasSelect())
    return ;

  QString str = getSelectedText();
  switch (keyWord)
  {
    case XW_TEX_TEXT:
      {
        QFont font = QApplication::font();
        font.setBold(bold);
        font.setItalic(italic);
        font.setPointSize(fontSize);
        if (slant)
          font.setStyle(QFont::StyleOblique);
        if (sc)
          font.setCapitalization(QFont::SmallCaps);
        if (sf)
          font.setStyleHint(QFont::SansSerif);

        painter->drawText(xA,yA,str);
      }
      break;

    case XW_TEX_LETTER:
    case XW_TEX_DIGIT:
      {
        QString fontname = getFontName();
        QByteArray tfmname = fontname.toAscii();
        XWTFMFile tfm(tfmname.constData());
        XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
        double w,h,d;
        double tmpx = xA;
        for (int i = 0; i < str.length(); i++)
        {
          w = tfm.getWidth(str[i].unicode());
          h = tfm.getHeight(str[i].unicode());
          d = tfm.getDepth(str[i].unicode());
          font->drawChar(painter,tmpx,yA,str[i].unicode());
          tmpx += w;
        }
      }
      break;

    default:
      break;
  }
}

void XWTeXText::dropTo(double xA,double yA)
{
  int i = getTextPosition(xA,yA);
  if (hasSelect())
  {
    QString str = getSelectedText();
    if (i >= 0)
    {
      QUndoCommand * cmd = new QUndoCommand;
      if (textPos < anchorPos)
      {
        if (i > textPos)
          i -= str.length();
        new XWTeXRemoveString(this,textPos,anchorPos,cmd);
      }
      else
      {
        if (i > anchorPos)
          i -= str.length();

        new XWTeXRemoveString(this,anchorPos,textPos,cmd);
      }
      new XWTeXInsertString(this,i,str,cmd);
      box->push(cmd);
    }
  }
  else
    anchorPos = i;
}

QString XWTeXText::getCurrentText()
{
  QString ret;
  if (keyWord == XW_TEX_TEXT)
    return text.left(textPos);

  return ret;
}

void XWTeXText::getDimension()
{
  XWTeXText * obj = 0;
  width = 0;
  height = 0;
  depth = 0;
  width2 = 0;
  height2 = 0;
  depth2 = 0;
  switch (keyWord)
  {
    case XW_TEX_UNKNOWN:
      unknownDimension();
      break;

    case XW_TEX_TEXT:
      textDimension();
      break;

    case XW_TEX_LETTER:
      lettersDimension();
      break;

    case XW_TEX_DIGIT:
      operatorsDimension();
      break;

    case XW_TEX_ASE:
      aseDimension();
      break;

    case LAarccos:
    case LAarcsin:
    case LAarctan:
    case LAarg:    
    case LAcos:
    case LAcosh:
    case LAcot:
    case LAcoth:
    case LAcsc:
    case LAdeg:
    case LAdet:
    case LAdim:
    case LAexp:
    case LAgcd:
    case LAhom:    
    case LAker:
    case LAlg:    
    case LAln:
    case LAlog:
    case LAmax:
    case LAmin:    
    case LAPr:
    case LAsec:
    case LAsin:
    case LAsinh:    
    case LAtan:
    case LAtanh:
    case LAsup:
    case LAinf:
    case LAlim:
      operatorsDimension();
      break;

    case LApmod:
      operatorsDimension();
      width += 4;
      break;

    case LAbmod:
      operatorsDimension();
      width += 8;
      break;

    case LAliminf:
    case LAlimsup:
      operatorsDimension(3);
      break;

    case LAarray:
      obj = head->next;
      while (obj)
      {
        obj->family = family;
        obj->fontSize = fontSize;
        obj->bold = bold;
        obj->slant = slant;
        obj->italic = italic;
        obj->cal = cal;
        obj->sc = sc;    
        obj->sf = sf; 
        obj->getDimension();
        height += obj->height + obj->depth + fontSize;
        obj = obj->next;
      }
      height -= fontSize;
      height /= 2;
      depth = height;
      {
        QList<double> ws;
        obj = head->next->head;
        while(obj)
        {
          ws << obj->width;
          obj = obj->next;
        }
        obj = head->next->next;
        while(obj)
        {
          obj->getMaxColumnWidth(ws);
          obj = obj->next;
        }

        QList<int> as;
        for (int i = 0; i < ws.size(); i++)
          as << XW_TEX_CENTER;

        QString atxt = head->head->text;
        for (int i = 0; i < atxt.length(); i++)
        {
          if (atxt[i] == QChar('l'))
            as[i] = XW_TEX_LEFT;
          else if (atxt[i] == QChar('r'))
            as[i] = XW_TEX_RIGHT;
        }

        obj = head->next;
        while (obj)
        {
          obj->setMaxColumnWidthAndAlign(ws,as);
          obj = obj->next;
        }

        for (int i = 0; i < ws.size(); i++)
          width += ws[i];

        width += (ws.size() - 1) * fontSize;
      }      
      break;

    case LAeqnarray:
    case LAeqnarrayStar:
      obj = head;
      while (obj)
      {
        obj->family = family;
        obj->fontSize = fontSize;
        obj->bold = bold;
        obj->slant = slant;
        obj->italic = italic;
        obj->cal = cal;
        obj->sc = sc;    
        obj->sf = sf; 
        obj->getDimension();
        width = qMax(width,obj->width);
        height += obj->height + obj->depth + fontSize;
        obj = obj->next;
      }
      height -= fontSize;
      height /= 2;
      depth = height;
      break;

    case LAfrac:
      obj = head;
      while (obj)
      {
        obj->family = family;
        obj->fontSize = fontSize;
        obj->bold = bold;
        obj->slant = slant;
        obj->italic = italic;
        obj->cal = cal;
        obj->sc = sc;    
        obj->sf = sf; 
        obj->getDimension();
        width = qMax(width,obj->width);
        obj = obj->next;
      }
      height = head->height + head->depth + 3;
      depth = head->next->height + head->next->depth + 2;
      break;

    case LAstackrel:
      obj = head;
      while (obj)
      {
        obj->family = family;
        obj->fontSize = fontSize;
        obj->bold = bold;
        obj->slant = slant;
        obj->italic = italic;
        obj->cal = cal;
        obj->sc = sc;    
        obj->sf = sf; 
        obj->getDimension();
        width = qMax(width,obj->width);
        obj = obj->next;
      }
      height = head->height + head->depth;
      depth = head->next->height + head->next->depth;
      break;

    case LAsqrt:
      symbolDimension(0x70);
      obj = head;
      while (obj)
      {
        obj->family = family;
        obj->bold = bold;
        obj->slant = slant;
        obj->italic = italic;
        obj->cal = cal;
        obj->sc = sc;    
        obj->sf = sf; 
        obj->getDimension();
        width += obj->width;
        obj = obj->next;
      }
      height = head->height + head->next->height;
      depth = head->next->depth;
      break;

    case LAhat: 
      operatorDimension(0x5E);
      break;

    case LAcheck: 
      operatorDimension(0x14);
      break;

    case LAtilde:
      operatorDimension(0x7E);
      break;

    case LAgrave:
      operatorDimension(0x12);
      break;

    case LAdot:
      operatorDimension(0x5F);
      break;

    case LAddot: 
      operatorDimension(0x7F);
      break;

    case LAbar:  
      operatorDimension(0x16);
      break;

    case LAvec:
      letterDimension(0x7E);
      break;

    case LAwidehat:
      largeSymbolDimension(0x62);
      break;

    case LAacute:   
      operatorDimension(0x13);
      break;

    case LAbreve:  
      operatorDimension(0x15);
      break;

    case LAwidetilde:
      largeSymbolDimension(0x65);
      break;

    case LAoverrightarrow:
    case LAunderrightarrow:
      symbolDimension(0x21);
      break;

    case LAoverleftarrow:
    case LAunderleftarrow:
      symbolDimension(0x20);
      break;

    case LAoverleftrightarrow:
    case LAunderleftrightarrow:
      symbolDimensionLR(0x20,0x21);
      break;

    case LAoverbrace:
      largeSymbolDimensionLR(0x7A,0x7D,0x7C,0x7B);
      break;

    case LAunderbrace:      
      largeSymbolDimensionLR(0x7C,0x7B,0x7A,0x7D);
      break;

    case LAoverline:    
    case LAunderline:
      head->getDimension();
      width = head->width;
      height = head->height;
      depth = head->depth;
      break;

    case LAsum:
      largeSymbolDimension(0x50);
      break;

    case LAbigcup:
      largeSymbolDimension(0x53);
      break;

    case LAbigvee:
      largeSymbolDimension(0x57);
      break;

    case LAprod:
      largeSymbolDimension(0x51);
      break;

    case LAbigcap:
      largeSymbolDimension(0x54);
      break;

    case LAbigwedge:
      largeSymbolDimension(0x56);
      break;

    case LAcoprod:
      largeSymbolDimension(0x60);
      break;

    case LAbigsqcup:
      largeSymbolDimension(0x46);
      break;

    case LAbiguplus:
      largeSymbolDimension(0x55);
      break;

    case LAbigodot:
      largeSymbolDimension(0x4A);
      break;

    case LAbigoplus:
      largeSymbolDimension(0x4C);
      break;

    case LAbigotimes:
      largeSymbolDimension(0x4E);
      break;

    case LAint:
      largeSymbolDimension(0x52);
      break;

    case LAoint:
      largeSymbolDimension(0x48);
      break;

    case LAalpha:
      letterDimension(0x0B);
      break;

    case LAbeta:
      letterDimension(0x0C);
      break;

    case LAgamma:
      letterDimension(0x0D);
      break;

    case LAdelta:
      letterDimension(0x0E);
      break;

    case LAepsilon:
      letterDimension(0x0F);
      break;

    case LAzeta:
      letterDimension(0x10);
      break;

    case LAeta:
      letterDimension(0x11);
      break;

    case LAtheta:
      letterDimension(0x12);
      break;

    case LAiota:
      letterDimension(0x13);
      break;

    case LAkappa:
      letterDimension(0x14);
      break;

    case LAlambda:
      letterDimension(0x15);
      break;

    case LAmu:
      letterDimension(0x16);
      break;

    case LAnu:
      letterDimension(0x17);
      break;

    case LAxi:
      letterDimension(0x18);
      break;

    case LApi:
      letterDimension(0x19);
      break;

    case LArho:
      letterDimension(0x1A);
      break;

    case LAsigma:
      letterDimension(0x1B);
      break;

    case LAtau:
      letterDimension(0x1C);
      break;

    case LAupsilon:
      letterDimension(0x1D);
      break;

    case LAphi:
      letterDimension(0x1E);
      break;

    case LAchi:
      letterDimension(0x1F);
      break;

    case LApsi:
      letterDimension(0x20);
      break;

    case LAomega:
      letterDimension(0x21);
      break;

    case LAvarepsilon:
      letterDimension(0x22);
      break;

    case LAvartheta:
      letterDimension(0x23);
      break;

    case LAvarpi:
      letterDimension(0x24);
      break;

    case LAvarrho:
      letterDimension(0x25);
      break;

    case LAvarsigma:
      letterDimension(0x26);
      break;

    case LAvarphi:
      letterDimension(0x27);
      break;

    case LAGamma:
      letterDimension(0x00);
      break;

    case LADelta:
      letterDimension(0x01);
      break;

    case LATheta:
      letterDimension(0x02);
      break;

    case LALambda:
      letterDimension(0x03);
      break;

    case LAXi:
      letterDimension(0x04);
      break;

    case LAPi:
      letterDimension(0x05);
      break;

    case LASigma:
      letterDimension(0x06);
      break;

    case LAUpsilon:
      letterDimension(0x07);
      break;

    case LAPhi:
      letterDimension(0x08);
      break;

    case LAPsi:
      letterDimension(0x09);
      break;

    case LAOmega:
      letterDimension(0x0A);
      break;

    case LAaleph:
      letterDimension(0x40);
      break;

    case LAleq:
    case LAle:
      symbolDimension(0x14);
      break;

    case LAgeq:
    case LAge:
      symbolDimension(0x15);
      break;

    case LAequiv:
      symbolDimension(0x11);
      break;

    case LAll:
      symbolDimension(0x1C);
      break;

    case LAgg:
      symbolDimension(0x1D);
      break;

    case LAdoteq:
      operatorDimensionTB(0x2E,0x3D);
      break;

    case LAprec:
      symbolDimension(0x1E);
      break;

    case LAsucc:
      symbolDimension(0x1F);
      break;

    case LAsim:
      symbolDimension(0x18);
      break;

    case LApreceq:
      symbolDimension(0x16);
      break;

    case LAsucceq:
      symbolDimension(0x17);
      break;

    case LAsimeq:
      symbolDimension(0x27);
      break;

    case LAsubset:
      symbolDimension(0x1A);
      break;

    case LAsupset:
      symbolDimension(0x18);
      break;

    case LAapprox:
      symbolDimension(0x19);
      break;

    case LAsubseteq:
      symbolDimension(0x12);
      break;

    case LAsupseteq:
      symbolDimension(0x13);
      break;

    case LAcong:
      symbolDimensionTB(0x18,0x3D);
      break;

    case LAsqsubset:
      lasyDimension(0x3C);
      break;

    case LAsqsupset:
      lasyDimension(0x3D);
      break;

    case LAJoin:
      lasyDimension(0x31);
      break;

    case LAsqsubseteq:
      symbolDimension(0x76);
      break;

    case LAsqsupseteq:
      symbolDimension(0x77);
      break;

    case LAbowtie:
      letterDimensionLR(0x2E,0x2F);
      break;

    case LAin:
      symbolDimension(0x32);
      break;

    case LAni:
      symbolDimension(0x33);
      break;

    case LApropto:
      symbolDimension(0x2F);
      break;

    case LAvdash:
      symbolDimension(0x60);
      break;

    case LAdashv:
      symbolDimension(0x61);
      break;

    case LAmodels:
      modelsDimension();
      break;

    case LAmid:
      symbolDimension(0x6A);
      break;

    case LAparallel:
      symbolDimension(0x6B);
      break;

    case LAperp:
      symbolDimension(0x3F);
      break;

    case LAsmile:
      letterDimension(0x5E);
      break;

    case LAfrown:
      letterDimension(0x5F);
      break;

    case LAasymp:
      symbolDimension(0x10);
      break;

    case LAnotin:
      notinDimension();      
      break;

    case LAneq:
      neqDimension();      
      break;

    case LApm:
      symbolDimension(0x06);
      break;

    case LAmp:
      symbolDimension(0x07);
      break;

    case LAtriangleleft:
      letterDimension(0x2E);
      break;

    case LAcdot:
      symbolDimension(0x01);
      break;

    case LAdiv:
      symbolDimension(0x04);
      break;

    case LAtriangleright:
      letterDimension(0x2F);
      break;

    case LAtimes:
      symbolDimension(0x02);
      break;

    case LAsetminus:
      symbolDimension(0x6E);
      break;

    case LAstar:
      letterDimension(0x3F);
      break;

    case LAcup:
      symbolDimension(0x5C);
      break;

    case LAcap:
      symbolDimension(0x5B);
      break;

    case LAast:
      symbolDimension(0x03);
      break;

    case LAsqcup:
      symbolDimension(0x74);
      break;

    case LAsqcap:
      symbolDimension(0x75);
      break;

    case LAcirc:
      symbolDimension(0x0E);
      break;

    case LAvee:
      symbolDimension(0x5F);
      break;

    case LAwedge:
      symbolDimension(0x5E);
      break;

    case LAbullet:
      symbolDimension(0x0F);
      break;

    case LAoplus:
      symbolDimension(0x08);
      break;

    case LAominus:
      symbolDimension(0x09);
      break;

    case LAdiamond:
      symbolDimension(0x05);
      break;

    case LAodot:
      symbolDimension(0x0C);
      break;

    case LAoslash:
      symbolDimension(0x0B);
      break;

    case LAuplus:
      symbolDimension(0x5D);
      break;

    case LAotimes:
      symbolDimension(0x0A);
      break;

    case LAbigcirc:
      symbolDimension(0x0D);
      break;

    case LAamalg:
      symbolDimension(0x71);
      break;

    case LAbigtriangleup:
      symbolDimension(0x34);
      break;

    case LAbigtriangledown:
      symbolDimension(0x35);
      break;

    case LAdagger:
      symbolDimension(0x79);
      break;

    case LAlhd:
      lasyDimension(0x01);
      break;

    case LArhd:
      lasyDimension(0x03);
      break;

    case LAddagger:
      lasyDimension(0x7A);
      break;

    case LAunlhd:
      lasyDimension(0x02);
      break;

    case LAunrhd:
      lasyDimension(0x04);
      break;

    case LAwr:
      symbolDimension(0x6F);
      break;

    case LAgets:
    case LAleftarrow:
      symbolDimension(0x20);
      break;

    case LAlongleftarrow:
      symbolDimensionLR(0x20,0x00);
      break;

    case LAto:
    case LArightarrow:
      symbolDimension(0x21);
      break;

    case LAlongrightarrow:
      symbolDimensionLR(0x00,0x21);
      break;

    case LAlongleftrightarrow:
      symbolDimensionLR(0x20,0x21);
      break;

    case LALeftarrow:
      symbolDimension(0x28);
      break;

    case LALongleftarrow:
      LongArrowDimension(0x28,0x3D);
      break;

    case LALongrightarrow:
      LongArrowDimension(0x29,0x3D);
      break;

    case LALeftrightarrow:
      symbolDimension(0x2C);
      break;

    case LALongleftrightarrow:
      symbolDimensionLR(0x28,0x29);
      break;

    case LAmapsto:
      symbolDimensionLR(0x37,0x21);
      break;

    case LAlongmapsto:
      symbolDimensionLR(0x37,0x00,0x21);
      break;

    case LAhookleftarrow:
      hookArrowDimension(0x20,0x2D);
      break;

    case LAhookrightarrow:
      hookArrowDimension(0x21,0x2C);
      break;

    case LAleftharpoonup:
      letterDimension(0x28);
      break;

    case LArightharpoonup:
      letterDimension(0x2A);
      break;

    case LAleftharpoondown:
      letterDimension(0x29);
      break;

    case LArightharpoondown:
      letterDimension(0x2B);
      break;

    case LAiff:
      symbolDimensionLR(0x28,0x29);
      break;

    case LAuparrow:
      symbolDimension(0x22,0x78);
      break;

    case LAdownarrow:
      symbolDimension(0x23,0x79);
      break;

    case LAupdownarrow:
      symbolDimension(0x6C);
      break;

    case LAUparrow:
      symbolDimension(0x2A);
      break;

    case LADownarrow:
      symbolDimension(0x2B);
      break;

    case LAUpdownarrow:
      symbolDimension(0x6D);
      break;

    case LAnearrow:
      symbolDimension(0x25);
      break;

    case LAsearrow:
      symbolDimension(0x26);
      break;

    case LAswarrow:
      symbolDimension(0x2E);
      break;

    case LAnwarrow:
      symbolDimension(0x2D);
      break;

    case LAleadsto:
      symbolDimension(0x3B);
      break;

    case LAleft:
      break;

    case LAright:
      rightDimension();
      break;

    case LAbig:
    case LABig:
    case LAbigg:
    case LABigg:      
      delimiterDimension();
      break;

    case LAlbrack:
      operatorDimension(0x5B,0x02);
      break;

    case LArbrack:
      operatorDimension(0x5D,0x03);
      break;

    case LAlbrace:
      symbolDimension(0x66,0x08);
      break;

    case LArbrace:
      symbolDimension(0x67,0x09);
      break;

    case LAlangle:
      symbolDimension(0x68,0x0A);
      break;

    case LArangle:
      symbolDimension(0x69,0x0B);
      break;

    case LAvert:
    case LAvvert:
      symbolDimension(0x6A,0x0C);
      break;

    case LAlfloor:
      symbolDimension(0x62,0x04);
      break;

    case LArfloor:
      symbolDimension(0x63,0x05);
      break;

    case LAlceil:
      symbolDimension(0x64,0x06);
      break;

    case LAbackslash:
      symbolDimension(0x6E,0x0F);
      break;

    case LAVert:
      symbolDimension(0x6B,0X0D);
      break;

    case LArceil:
      symbolDimension(0x65,0x07);
      break;

    case LAlgroup:
      largeSymbolDimension(0x3A);
      break;

    case LArgroup:
      largeSymbolDimension(0x3B);
      break;

    case LAlmoustache:
      largeSymbolDimension(0X7A, 0x40);
      break;

    case LAarrowvert:
      symbolDimension(0x6A, 0x3C);
      break;

    case LAArrowvert:
      symbolDimension(0x6B,0x3D);
      break;

    case LAbracevert:
      largeSymbolDimension(0x3E);
      break;

    case LArmoustache:
      largeSymbolDimension(0x7B,0x41);
      break;

    case LAdots:
      dotsDimension();
      break;

    case LAcdots:
      cdotsDimension();
      break;

    case LAvdots:
      vdotsDimension();
      break;

    case LAddots:
      ddotsDimension();
      break;

    case LAimath:
      letterDimension(0x7B);
      break;

    case LAjmath:
      letterDimension(0x7C);
      break;

    case LAell:
      letterDimension(0x60);
      break;

    case LARe:
      letterDimension(0x3C);
      break;

    case LAIm:
      letterDimension(0x3D);
      break;

    case LAwp:
      letterDimension(0x7D);
      break;

    case LAforall:
      symbolDimension(0x38);
      break;

    case LAexists:
      symbolDimension(0x39);
      break;

    case LApartial:
      letterDimension(0x40);
      break;

    case LAprime:
      symbolDimension(0x30);
      break;

    case LAemptyset:
      symbolDimension(0x3B);
      break;

    case LAinfty:
      symbolDimension(0x31);
      break;

    case LAnabla:
      symbolDimension(0x72);
      break;

    case LAtriangle:
      symbolDimension(0x34);
      break;

    case LABox:
      lasyDimension(0x32);
      break;

    case LADiamond:
      lasyDimension(0x33);
      break;

    case LAbot:
      symbolDimension(0x3F);
      break;

    case LAtop:
      symbolDimension(0x3E);
      break;

    case LAangle:
      amsaDimension(0x5C);
      break;

    case LAsurd:
      symbolDimension(0x70);
      break;

    case LAdiamondsuit:
      symbolDimension(0x7D);
      break;

    case LAheartsuit:
      symbolDimension(0x7E);
      break;

    case LAclubsuit:
      symbolDimension(0x7C);
      break;

    case LAspadesuit:
      symbolDimension(0x7F);
      break;

    case LAneg:
      symbolDimension(0x3A);
      break;

    case LAflat:
      letterDimension(0x5B);
      break;

    case LAnatural:
      letterDimension(0x5C);
      break;

    case LAsharp:
      letterDimension(0x5D);
      break;

    case LAulcorner:
      amsaDimension(0x70);
      break;

    case LAurcorner:
      amsaDimension(0x71);
      break;

    case LAllcorner:
      amsaDimension(0x78);
      break;

    case LAlrcorner:
      amsaDimension(0x79);
      break;

    case LAlvert:
    case LArvert:
      vertDimension(0x6A);
      break;

    case LAlVert:
    case LArVert:
      vertDimension(0x6B);
      break;

    case LAdigamma:
      amsbDimension(0x7A);
      break;

    case LAvarkappa:
      amsbDimension(0x7B);
      break;

    case LAbeth:
      amsbDimension(0x69);
      break;

    case LAgimel:
      amsbDimension(0x6A);
      break;

    case LAdaleth:
      amsbDimension(0x6B);
      break;

    case LAlessdot:
      amsbDimension(0x6C);
      break;

    case LAgtrdot:
      amsbDimension(0x6D);
      break;

    case LAdoteqdot:
      amsaDimension(0x2B);
      break;

    case LAleqslant:
      amsaDimension(0x36);
      break;

    case LAgeqslant:
      amsaDimension(0x3E);
      break;

    case LArisingdotseq:
      amsaDimension(0x3A);
      break;

    case LAeqslantless:
      amsaDimension(0x30);
      break;

    case LAeqslantgtr:
      amsaDimension(0x31);
      break;

    case LAfallingdotseq:
      amsaDimension(0x3B);
      break;

    case LAleqq:
      amsaDimension(0x35);
      break;

    case LAgeqq:
      amsaDimension(0x3D);
      break;

    case LAeqcirc:
      amsaDimension(0x50);
      break;

    case LAlll:
      amsaDimension(0x6E);
      break;

    case LAggg:
      amsaDimension(0x6F);
      break;

    case LAcirceq:
      amsaDimension(0x24);
      break;

    case LAlesssim:
      amsaDimension(0x2E);
      break;

    case LAgtrsim:
      amsaDimension(0x26);
      break;

    case LAtriangleq:
      amsaDimension(0x2C);
      break;

    case LAlessapprox:
      amsaDimension(0x2F);
      break;

    case LAgtrapprox:
      amsaDimension(0x27);
      break;

    case LAbumpeq:
      amsaDimension(0x6C);
      break;

    case LAlessgtr:
      amsaDimension(0x37);
      break;

    case LAgtrless:
      amsaDimension(0x3F);
      break;

    case LABumpeq:
      amsaDimension(0x6D);
      break;

    case LAlesseqgtr:
      amsaDimension(0x51);
      break;

    case LAgtreqless:
      amsaDimension(0x52);
      break;

    case LAthicksim:
      amsbDimension(0x73);
      break;

    case LAlesseqqgtr:
      amsaDimension(0x53);
      break;

    case LAgtreqqless:
      amsaDimension(0x54);
      break;

    case LAthickapprox:
      amsbDimension(0x74);
      break;

    case LApreccurlyeq:
      amsaDimension(0x34);
      break;

    case LAsucccurlyeq:
      amsaDimension(0x3C);
      break;

    case LAapproxeq:
      amsbDimension(0x75);
      break;

    case LAcurlyeqprec:
      amsaDimension(0x32);
      break;

    case LAcurlyeqsucc:
      amsaDimension(0x33);
      break;

    case LAbacksim:
      amsaDimension(0x76);
      break;

    case LAprecsim:
      amsaDimension(0x2D);
      break;

    case LAsuccsim:
      amsaDimension(0x25);
      break;

    case LAbacksimeq:
      amsaDimension(0x77);
      break;

    case LAprecapprox:
      amsbDimension(0x77);
      break;

    case LAsuccapprox:
      amsbDimension(0x76);
      break;

    case LAvDash:
      amsaDimension(0x0F);
      break;

    case LAsubseteqq:
      amsaDimension(0x6A);
      break;

    case LAsupseteqq:
      amsaDimension(0x6B);
      break;

    case LAVdash:
      amsaDimension(0x0D);
      break;

    case LAshortparallel:
      amsbDimension(0x71);
      break;

    case LASupset:
      amsaDimension(0x63);
      break;

    case LAVvdash:
      amsaDimension(0x0E);
      break;

    case LAblacktriangleleft:
      amsaDimension(0x4A);
      break;

    case LAbackepsilon:
      amsbDimension(0x7F);
      break;

    case LAvartriangleright:
      amsaDimension(0x42);
      break;

    case LAbecause:
      amsaDimension(0x2A);
      break;

    case LAvarpropto:
      amsaDimension(0x5F);
      break;

    case LAblacktriangleright:
      amsaDimension(0x49);
      break;

    case LASubset:
      amsaDimension(0x62);
      break;

    case LAbetween:
      amsaDimension(0x47);
      break;

    case LAtrianglerighteq:
      amsaDimension(0x44);
      break;

    case LAsmallfrown:
      amsaDimension(0x61);
      break;

    case LApitchfork:
      amsaDimension(0x74);
      break;

    case LAvartriangleleft:
      amsaDimension(0x43);
      break;

    case LAshortmid:
      amsbDimension(0x70);
      break;

    case LAsmallsmile:
      amsaDimension(0x60);
      break;

    case LAtrianglelefteq:
      amsaDimension(0x45);
      break;

    case LAtherefore:
      amsaDimension(0x29);
      break;

    case LAdashleftarrow:
      amsaDimensionLR(0x4C,0x39,0x39);
      break;

    case LAdashrightarrow:
      amsaDimensionLR(0x39,0x39,0x4B);
      break;

    case LAleftleftarrows:
      amsaDimension(0x12);
      break;

    case LArightrightarrows:
      amsaDimension(0x13);
      break;

    case LAleftrightarrows:
      amsaDimension(0x1C);
      break;

    case LArightleftarrows:
      amsaDimension(0x1D);
      break;

    case LALleftarrow:
      amsaDimension(0x57);
      break;

    case LARrightarrow:
      amsaDimension(0x56);
      break;

    case LAtwoheadleftarrow:
      amsaDimension(0x11);
      break;

    case LAtwoheadrightarrow:
      amsaDimension(0x10);
      break;

    case LAleftarrowtail:
      amsaDimension(0x1B);
      break;

    case LArightarrowtail:
      amsaDimension(0x1A);
      break;

    case LAleftrightharpoons:
      amsaDimension(0x0B);
      break;

    case LArightleftharpoons:
      amsaDimension(0x0A);
      break;

    case LALsh:
      amsaDimension(0x1E);
      break;

    case LARsh:
      amsaDimension(0x1F);
      break;

    case LAlooparrowleft:
      amsaDimension(0x22);
      break;

    case LAlooparrowright:
      amsaDimension(0x23);
      break;

    case LAcurvearrowleft:
      amsbDimension(0x78);
      break;

    case LAcurvearrowright:
      amsbDimension(0x79);
      break;

    case LAcirclearrowleft:
      amsaDimension(0x09);
      break;

    case LAcirclearrowright:
      amsaDimension(0x08);
      break;

    case LAmultimap:
      amsaDimension(0x28);
      break;

    case LAupuparrows:
      amsaDimension(0x14);
      break;

    case LAdowndownarrows:
      amsaDimension(0x15);
      break;

    case LAupharpoonleft:
      amsaDimension(0x18);
      break;

    case LAupharpoonright:
      amsaDimension(0x16);
      break;

    case LAdownharpoonright:
      amsaDimension(0x17);
      break;

    case LArightsquigarrow:
      amsaDimension(0x20);
      break;

    case LAleftrightsquigarrow:
      amsaDimension(0x21);
      break;

    case LAnless:
      amsbDimension(0x04);
      break;

    case LAngtr:
      amsbDimension(0x05);
      break;

    case LAvarsubsetneqq:
      amsbDimension(0x26);
      break;

    case LAlneq:
      amsbDimension(0x0C);
      break;

    case LAgneq:
      amsbDimension(0x0D);
      break;

    case LAvarsupsetneqq:
      amsbDimension(0x27);
      break;

    case LAnleq:
      amsbDimension(0x02);
      break;

    case LAngeq:
      amsbDimension(0x03);
      break;

    case LAnsubseteqq:
      amsbDimension(0x22);
      break;

    case LAnleqslant:
      amsbDimension(0x0A);
      break;

    case LAngeqslant:
      amsbDimension(0x0B);
      break;

    case LAnsupseteqq:
      amsbDimension(0x23);
      break;

    case LAlneqq:
      amsbDimension(0x08);
      break;

    case LAgneqq:
      amsbDimension(0x09);
      break;

    case LAnmid:
      amsbDimension(0x2D);
      break;

    case LAlvertneqq:
      amsbDimension(0x00);
      break;

    case LAgvertneqq:
      amsbDimension(0x01);
      break;

    case LAnparallel:
      amsbDimension(0x2C);
      break;

    case LAnleqq:
      amsbDimension(0x14);
      break;

    case LAngeqq:
      amsbDimension(0x15);
      break;

    case LAnshortmid:
      amsbDimension(0x2E);
      break;

    case LAlnsim:
      amsbDimension(0x12);
      break;

    case LAgnsim:
      amsbDimension(0x13);
      break;

    case LAnshortparallel:
      amsbDimension(0x2F);
      break;

    case LAlnapprox:
      amsbDimension(0x1A);
      break;

    case LAgnapprox:
      amsbDimension(0x1B);
      break;

    case LAnsim:
      amsbDimension(0x1C);
      break;

    case LAnprec:
      amsbDimension(0x06);
      break;

    case LAnsucc:
      amsbDimension(0x07);
      break;

    case LAncong:
      amsbDimension(0x1D);
      break;

    case LAnpreceq:
      amsbDimension(0x0E);
      break;

    case LAnsucceq:
      amsbDimension(0x0F);
      break;

    case LAnvdash:
      amsbDimension(0x30);
      break;

    case LAprecneqq:
      amsbDimension(0x16);
      break;

    case LAsuccneqq:
      amsbDimension(0x17);
      break;

    case LAnvDash:
      amsbDimension(0x32);
      break;

    case LAprecnsim:
      amsbDimension(0x10);
      break;

    case LAsuccnsim:
      amsbDimension(0x11);
      break;

    case LAnVdash:
      amsbDimension(0x31);
      break;

    case LAprecnapprox:
      amsbDimension(0x18);
      break;

    case LAsuccnapprox:
      amsbDimension(0x19);
      break;

    case LAnVDash:
      amsbDimension(0x33);
      break;

    case LAsubsetneq:
      amsbDimension(0x28);
      break;

    case LAsupsetneq:
      amsbDimension(0x29);
      break;

    case LAntriangleleft:
      amsbDimension(0x36);
      break;

    case LAvarsubsetneq:
      amsbDimension(0x20);
      break;

    case LAvarsupsetneq:
      amsbDimension(0x21);
      break;

    case LAntriangleright:
      amsbDimension(0x37);
      break;

    case LAnsubseteq:
      amsbDimension(0x2A);
      break;

    case LAnsupseteq:
      amsbDimension(0x2B);
      break;

    case LAntrianglelefteq:
      amsbDimension(0x35);
      break;

    case LAsubsetneqq:
      amsbDimension(0x24);
      break;

    case LAsupsetneqq:
      amsbDimension(0x25);
      break;

    case LAntrianglerighteq:
      amsbDimension(0x34);
      break;

    case LAnleftarrow:
      amsbDimension(0x38);
      break;

    case LAnrightarrow:
      amsbDimension(0x39);
      break;

    case LAnleftrightarrow:
      amsbDimension(0x3D);
      break;

    case LAnLeftarrow:
      amsbDimension(0x3A);
      break;

    case LAnRightarrow:
      amsbDimension(0x3B);
      break;

    case LAnLeftrightarrow:
      amsbDimension(0x3C);
      break;

    case LAdotplus:
      amsaDimension(0x75);
      break;

    case LAcenterdot:
      amsaDimension(0x05);
      break;

    case LAltimes:
      amsbDimension(0x6E);
      break;

    case LArtimes:
      amsbDimension(0x6F);
      break;

    case LAdivideontimes:
      amsbDimension(0x3E);
      break;

    case LAdoublecup:
      amsaDimension(0x64);
      break;

    case LAdoublecap:
      amsaDimension(0x65);
      break;

    case LAsmallsetminus:
      amsaDimension(0x72);
      break;

    case LAveebar:
      amsaDimension(0x59);
      break;

    case LAbarwedge:
      amsaDimension(0x5A);
      break;

    case LAdoublebarwedge:
      amsaDimension(0x5B);
      break;

    case LAboxplus:
      amsaDimension(0x01);
      break;

    case LAboxminus:
      amsaDimension(0x0C);
      break;

    case LAcircleddash:
      amsaDimension(0x7F);
      break;

    case LAboxtimes:
      amsaDimension(0x02);
      break;

    case LAboxdot:
      amsaDimension(0x00);
      break;

    case LAcircledcirc:
      amsaDimension(0x7D);
      break;

    case LAintercal:
      amsaDimension(0x7C);
      break;

    case LAcircledast:
      amsaDimension(0x7E);
      break;

    case LArightthreetimes:
      amsaDimension(0x69);
      break;

    case LAcurlyvee:
      amsaDimension(0x67);
      break;

    case LAcurlywedge:
      amsaDimension(0x66);
      break;

    case LAleftthreetimes:
      amsaDimension(0x68);
      break;

    case LAhbar:
      amsbDimension(0x7E);
      break;

    case LAhslash:
      amsbDimension(0x7D);
      break;

    case LABbbk:
      amsbDimension(0x7C);
      break;

    case LAsquare:
      amsaDimension(0x03);
      break;

    case LAblacksquare:
      amsaDimension(0x04);
      break;

    case LAcircledS:
      amsaDimension(0x73);
      break;

    case LAvartriangle:
      amsaDimension(0x4D);
      break;

    case LAblacktriangle:
      amsaDimension(0x4E);
      break;

    case LAcomplement:
      amsaDimension(0x7B);
      break;

    case LAtriangledown:
      amsaDimension(0x4F);
      break;

    case LAblacktriangledown:
      amsaDimension(0x48);
      break;

    case LAGame:
      amsbDimension(0x61);
      break;

    case LAlozenge:
      amsaDimension(0x06);
      break;

    case LAblacklozenge:
      amsaDimension(0x07);
      break;

    case LAbigstar:
      amsaDimension(0x46);
      break;

    case LAmeasuredangle:
      amsaDimension(0x5D);
      break;

    case LAdiagup:
      amsbDimension(0x1E);
      break;

    case LAdiagdown:
      amsbDimension(0x1F);
      break;

    case LAbackprime:
      amsaDimension(0x38);
      break;

    case LAnexists:
      amsbDimension(0x40);
      break;

    case LAFinv:
      amsbDimension(0x60);
      break;

    case LAvarnothing:
      amsbDimension(0x3F);
      break;

    case LAeth:
      amsbDimension(0x67);
      break;

    case LAsphericalangle:
      amsaDimension(0x5E);
      break;

    case LAmho:
      amsbDimension(0x66);
      break;

    case XW_TEX_SUB:
    case XW_TEX_SUP:
      obj = head;
      while (obj)
      {
        obj->family = family;
        switch (fontSize)
        {
          default:
            obj->fontSize = XW_TEX_FONT_SCRIPT;
            break;

          case XW_TEX_FONT_SMALL:
          case XW_TEX_FONT_NOTE:
          case XW_TEX_FONT_SCRIPT:
          case XW_TEX_FONT_TINY:
            obj->fontSize = XW_TEX_FONT_TINY;
            break;

          case XW_TEX_FONT_large:
          case XW_TEX_FONT_Large:
          case XW_TEX_FONT_LARGE:
            obj->fontSize = XW_TEX_FONT_NOTE;
            break;

          case XW_TEX_FONT_Huge:
          case XW_TEX_FONT_huge:
            obj->fontSize = XW_TEX_FONT_Large;
            break;
        }
        
        obj->bold = bold;
        obj->slant = slant;
        obj->italic = italic;
        obj->cal = cal;        
        obj->sc = sc;    
        obj->sf = sf; 
        obj->getDimension();
        height = qMax(height,obj->height);
        depth = qMax(depth,obj->depth);
        width += obj->width;
        obj = obj->next;
      }   
      break;

    default:
      obj = head;
      while (obj)
      {
        obj->family = family;
        obj->fontSize = fontSize;
        obj->bold = bold;
        obj->slant = slant;
        obj->italic = italic;
        obj->cal = cal;        
        obj->sc = sc;    
        obj->sf = sf; 
        obj->getDimension();
        height = qMax(height,obj->height);
        depth = qMax(depth,obj->depth);
        width += obj->width;
        obj = obj->next;
      }   
      break;
  }
  
  if (width == 0)
    width = fontSize;

  if (height == 0)
    height = fontSize;
}

QString XWTeXText::getFontName(int fam, int s,
                             bool bold,bool slant,
                             bool italic,bool cal)
{
  QString fontname;
  switch (fam)
  {
    case XW_TEX_FONT_CMR:
      {
        switch (s)
        {
          case XW_TEX_FONT_NORMAL:
            if (bold && slant)
              fontname = "cmbxsl10";
            else if (bold)
              fontname = "cmb10";
            else if (slant)
              fontname = "cmsl10";
            else if (italic)
              fontname = "cmti10";
            else if (cal)
              fontname = "cmcsc10";
            else
              fontname = "cmr10";
            break;

          case XW_TEX_FONT_SMALL:
            if (bold)
              fontname = "cmbx9";
            else if (slant)
              fontname = "cmsl9";
            else if (italic)
              fontname = "cmti9";
            else
              fontname = "cmr9";
            break;

          case XW_TEX_FONT_TINY:
            if (bold)
              fontname = "cmbx5";
            else
              fontname = "cmr5";
            break;

          case XW_TEX_FONT_SCRIPT:
            if (bold)
              fontname = "cmbx7";
            else if (italic)
              fontname = "cmti7";
            else
              fontname = "cmr7";
            break;

          case XW_TEX_FONT_NOTE:
            if (bold)
              fontname = "cmbx8";
            else if (slant)
              fontname = "cmsl8";
            else if (italic)
              fontname = "cmti8";
            else
              fontname = "cmr8";
            break;

          case XW_TEX_FONT_large:
            if (bold)
              fontname = "cmbx12";
            else if (slant)
              fontname = "cmsl12";
            else if (italic)
              fontname = "cmti12";
            else
              fontname = "cmr12";
            break;

          case XW_TEX_FONT_Large:
            fontname = "cmr14";
            break;

          default:
            fontname = "cmr17";
            break;
        }
      }
      break;

    case XW_TEX_FONT_CMM:
      {
        switch (s)
        {
          case XW_TEX_FONT_NORMAL:
            if (bold && slant)
              fontname = "cmmib10";
            else if (italic)
              fontname = "cmmib10";
            else
              fontname = "cmmi10";
            break;

          case XW_TEX_FONT_SMALL:
            fontname = "cmmi9";
            break;

          case XW_TEX_FONT_TINY:
            fontname = "cmmi5";
            break;

          case XW_TEX_FONT_SCRIPT:
            fontname = "cmmi7";
            break;

          case XW_TEX_FONT_NOTE:
            fontname = "cmmi8";
            break;

          default:
            fontname = "cmmi12";
            break;
        }
      }
      break;

    case XW_TEX_FONT_CMTT:
      {
        switch (s)
        {
          case XW_TEX_FONT_NORMAL:
            if (slant)
              fontname = "cmsltt10";
            else if (italic)
              fontname = "cmitt10";
            else if (cal)
              fontname = "cmtcsc10";
            else
              fontname = "cmtt10";
            break;

          case XW_TEX_FONT_SMALL:
            fontname = "cmtt9";
            break;

          case XW_TEX_FONT_NOTE:
            fontname = "cmtt8";
            break;

          default:
            fontname = "cmtt12";
            break;
        }
      }
      break;

    case XW_TEX_FONT_CMSY:
      {
        switch (s)
        {
          case XW_TEX_FONT_NORMAL:
            if (bold)
              fontname = "cmsy10";
            else
              fontname = "cmbsy10";
            break;

          case XW_TEX_FONT_SMALL:
            fontname = "cmsy9";
            break;

          case XW_TEX_FONT_TINY:
            fontname = "cmsy5";
            break;

          case XW_TEX_FONT_SCRIPT:
            fontname = "cmsy7";
            break;

          case XW_TEX_FONT_NOTE:
            fontname = "cmsy8";
            break;

          default:
            fontname = "cmsy10";
            break;
        }
      }
      break;

    case XW_TEX_FONT_CMEX:
      {
        switch (s)
        {
          case XW_TEX_FONT_SCRIPT:
            fontname = "cmex7";
            break;

          case XW_TEX_FONT_NOTE:
            fontname = "cmex8";
            break;

          case XW_TEX_FONT_SMALL:
            fontname = "cmex9";
            break;

          default:
            fontname = "cmex10";
            break;
        }
      }      
      break;

    case XW_TEX_FONT_LASY:
      {
        switch (s)
        {
          case XW_TEX_FONT_NORMAL:
            fontname = "lasy10";
            break;

          case XW_TEX_FONT_SMALL:
            fontname = "lasy9";
            break;

          case XW_TEX_FONT_TINY:
            fontname = "lasy5";
            break;

          case XW_TEX_FONT_SCRIPT:
            fontname = "lasy7";
            break;

          case XW_TEX_FONT_NOTE:
            fontname = "lasy8";
            break;

          default:
            fontname = "lasy10";
            break;
        }
      }
      break;

    case XW_TEX_FONT_MSAM:
      {
        switch (s)
        {
          case XW_TEX_FONT_NORMAL:
            fontname = "msam10";
            break;

          case XW_TEX_FONT_SMALL:
            fontname = "msam9";
            break;

          case XW_TEX_FONT_TINY:
            fontname = "msam5";
            break;

          case XW_TEX_FONT_SCRIPT:
            fontname = "msam7";
            break;

          case XW_TEX_FONT_NOTE:
            fontname = "msam8";
            break;

          default:
            fontname = "msam10";
            break;
        }
      }
      break;

    case XW_TEX_FONT_MSBM:
      {
        switch (s)
        {
          case XW_TEX_FONT_NORMAL:
            fontname = "msbm10";
            break;

          case XW_TEX_FONT_SMALL:
            fontname = "msbm9";
            break;

          case XW_TEX_FONT_TINY:
            fontname = "msbm5";
            break;

          case XW_TEX_FONT_SCRIPT:
            fontname = "msbm7";
            break;

          case XW_TEX_FONT_NOTE:
            fontname = "msbm8";
            break;

          default:
            fontname = "msbm10";
            break;
        }
      }
      break;

    default:
      break;
  }

  return fontname;
}

QString XWTeXText::getFontName()
{
  return getFontName(family,fontSize,bold,slant,italic,cal);
}

QString XWTeXText::getSelectedText()
{
  return text.mid(textPos,anchorPos);
}

QString XWTeXText::getSubstring(int pos, int len)
{
  return text.mid(pos,len);
}

QString XWTeXText::getText()
{
  QString ret;
  switch (keyWord)
  {
    case XW_TEX_TEXT:
    case XW_TEX_LETTER:
    case XW_TEX_DIGIT:
    case XW_TEX_UNKNOWN:
      {
        ret = text;
        XWTeXText * obj = head;
        while (obj)
        {
          QString tmp = obj->getText();
          ret += tmp;
          obj = obj->next;
        }
      }      
      break;

    case LAtextit:
	  case LAtextsl:
	  case LAtextbf:
	  case LAtextsf:
	  case LAtextsc:
	  case LAtexttt:
	  case LAtextmd:
	  case LAtextup:
	  case LAtextrm:
	  case LAtextnormal:
    case LAem:
	  case LAit:
	  case LAsl:
	  case LAbf:
	  case LAsf:
	  case LAsc:
	  case LAtt:
	  case LAcal:
	  case LArm:
    case LAtiny:
	  case LAscriptsize:
	  case LAfootnotesize:
	  case LAsmall:
	  case LAnormalsize:
	  case LAlarge:
	  case LALarge:
	  case LALARGE:
	  case LAhuge:
	  case LAHuge:
      {
        QString tmp = getLaTeXString(keyWord);
        ret = QString("\\%1").arg(tmp);
        if (head)
        {
          tmp = head->getText();
          if (head->keyWord != XW_TEX_PARAM)
            tmp += " ";
          ret += tmp;
        }
      }
      break;

    case XW_TEX_SUB:
      if (head)
      {
        XWTeXText * obj = head;        
        if (obj->keyWord == XW_TEX_PARAM) 
        {
          QString tmp = obj->getText();
          if (!tmp.isEmpty())
          {
            ret.append(QChar('_'));
            ret += tmp;
          }
        }
        else
        {
          QString str;
          while (obj)
          {
            QString tmp = obj->getText();
            str += tmp;
            obj = obj->next;
          }

          if (str.length() > 0)
          {
            ret.append(QChar('_'));
            if (str.length() == 1)
              ret += str;
            else
            {
              QString tmp = QString("{%1}").arg(str);
              ret += tmp;
            }            
          }
        }
      }
      break;

    case XW_TEX_SUP:
      if (head)
      {     
        XWTeXText * obj = head;  
        if (obj->keyWord == XW_TEX_PARAM) 
        {
          QString tmp = obj->getText();
          if (!tmp.isEmpty())
          {
            ret.append(QChar('^'));
            ret += tmp;
          }
        }
        else
        {
          QString str;
          while (obj)
          {
            QString tmp = obj->getText();
            str += tmp;
            obj = obj->next;
          }

          if (str.length() > 0)
          {
            ret.append(QChar('^'));
            if (str.length() == 1)
              ret += str;
            else
            {
              QString tmp = QString("{%1}").arg(str);
              ret += tmp;
            }            
          }
        }
      }
      break;

    case XW_TEX_FOMULAR:
      {
        QString txt;
        XWTeXText * obj = head;
        while (obj)
        {
          QString tmp = obj->getText();
          txt += tmp;
          obj = obj->next;
        }

        ret = QString("$%1$").arg(txt);
      }
      break;

    case XW_TEX_DISFOMULAR:
      {
        QString txt;
        XWTeXText * obj = head;
        while (obj)
        {
          QString tmp = obj->getText();
          txt += tmp;
          obj = obj->next;
        }

        ret = QString("$$%1$$").arg(txt);
      }
      break;

    case XW_TEX_PARAM:
      {
        QString txt;
        XWTeXText * obj = head;
        while (obj)
        {
          QString tmp = obj->getText();
          txt += tmp;
          obj = obj->next;
        }

        ret = QString("{%1}").arg(txt);
      }
      break;

    case XW_TEX_OPT:
      {
        QString txt;
        XWTeXText * obj = head;
        while (obj)
        {
          QString tmp = obj->getText();
          txt += tmp;
          obj = obj->next;
        }
        
        if (!txt.isEmpty())
          ret = QString("[%1]").arg(txt);
      }
      break;

    case XW_TEX_COL:
      {
        XWTeXText * obj = head;
        while (obj)
        {
          QString tmp = obj->getText();
          ret += tmp;
          obj = obj->next;
        }
      }
      break;

    case XW_TEX_ROW:
      {
        XWTeXText * obj = head;
        while (obj)
        {
          QString tmp = obj->getText();
          ret += tmp;
          if (obj != last)
            ret += "&";
          obj = obj->next;
        }

        ret += "\\\\\n";
      }
      break;

    case LAarray:
      {
        XWTeXText * obj = head;
        QString opt = head->getText();
        QString table;
        obj = obj->next;
        while (obj)
        {
          QString tmp = obj->getText();
          table += tmp;
          obj = obj->next;
        }

        ret = QString("\\begin{array}%1\n%2\\end{array}").arg(opt).arg(table);
      }      
      break;

    case LAeqnarray:
      {
        XWTeXText * obj = head;
        QString table;
        while (obj)
        {
          QString tmp = obj->getText();
          table += tmp;
          obj = obj->next;
        }

        ret = QString("\\begin{eqnarray}\n%1\\end{eqnarray}").arg(table);
      }
      break;

    case LAeqnarrayStar:
      {
        XWTeXText * obj = head;
        QString table;
        while (obj)
        {
          QString tmp = obj->getText();
          table += tmp;
          obj = obj->next;
        }

        ret = QString("\\begin{eqnarray*}\n%1\\end{eqnarray*}").arg(table);
      }
      break;

    case LAequation:
      {
        XWTeXText * obj = head;
        QString table;
        while (obj)
        {
          QString tmp = obj->getText();
          table += tmp;
          obj = obj->next;
        }

        ret = QString("\\begin{equation}\n%1\n\\end{equation}").arg(table);
      }      
      break;

    case LAleft:
    case LAright:
    case LAbig:
    case LABig:
    case LAbigg:
    case LABigg:
      {
        QString tmp = getLaTeXString(keyWord);
        ret = QString("\\%1").arg(tmp);
        ret += text;
        XWTeXText * obj = head;
        while (obj)
        {
          tmp = obj->getText();
          ret += tmp;
          obj = obj->next;
        }
      }      
      break;

    default:
      {
        QString tmp = getLaTeXString(keyWord);
        ret = QString("\\%1").arg(tmp);
        XWTeXText * obj = head;
        while (obj)
        {
          tmp = obj->getText();
          ret += tmp;
          obj = obj->next;
        }

        if (next && (next->keyWord == XW_TEX_LETTER || 
                     next->keyWord == XW_TEX_DIGIT))
          ret += " ";
      }      
      break;
  }

  return ret;
}

int XWTeXText::getTextPosition(double xA,double yA)
{
  int i = 0;
  switch (keyWord)
  {
    case XW_TEX_TEXT:
      {
        QFont font = QApplication::font();
        font.setBold(bold);
        font.setItalic(italic);
        font.setPointSize(fontSize);
        if (slant)
          font.setStyle(QFont::StyleOblique);
        if (sc)
          font.setCapitalization(QFont::SmallCaps);
        if (sf)
          font.setStyleHint(QFont::SansSerif);
        QFontMetricsF metrics(font);
        double h = metrics.height();
        double curx = x;
        for (; i < text.length(); i++)
        {
          double w = metrics.width(text[i]);
          if ((curx <= xA) && (xA <= (curx + w)) && (yA >= y) && (yA <= (y + h)))
          {
            if (xA < (curx + 0.5 * w))
              return i;
            
            return i + 1;
          }
        }
      }
      break;

    default:
      {
        QString fontname = getFontName();
          QByteArray tfmname = fontname.toAscii();
          XWTFMFile tfm(tfmname.constData());
          double curx = x;
          for (int i = 0; i < text.length(); i++)
          {
            int c = text[i].unicode();
            double w = tfm.getWidth(c);
            double h = tfm.getHeight(c);
            double d = tfm.getDepth(c);
            if ((curx <= xA) && (xA <= (curx + w)) && (yA >= (y - d)) && (yA <= (y + h)))
            {
              if (xA < (curx + 0.5 * w))
                return i;
            
              return i + 1;
            }
          }
      }
      break;
  }

  return -1;
}

bool XWTeXText::hitTest(double xA, double yA, double xr, double yr,int s, int e)
{
  bool ret = false;
  if (xA >= x && xA < (x + width) && yA <= (y + height) && yA >= (y - depth))
  {
    XWTeXText * obj = head;
    while (obj)
    {
      if (obj->hitTest(xA, yA,xr,yr,s,e))
        return true;

      obj = obj->next;
    }

    ret = true;
    double minx,miny,maxx,maxy;
    switch (keyWord)
    {
      case XW_TEX_TEXT:
        {
          QFont font = QApplication::font();
          font.setBold(bold);
          font.setItalic(italic);
          font.setPointSize(fontSize);
          if (slant)
            font.setStyle(QFont::StyleOblique);
          if (sc)
            font.setCapitalization(QFont::SmallCaps);
          if (sf)
            font.setStyleHint(QFont::SansSerif);
          QFontMetricsF metrics(font);
          double h = metrics.height();          
          miny = y - h;
          maxy = y;
          double curx = xr;
          for (int i = s; i < e; i++)
          {
            double w = metrics.width(text[i]);
            if ((curx <= xA) && (xA <= (curx + w)))
            {
              if (xA < (curx + 0.5 * w))
              {
                minx = curx - 0.5;              
                maxx = curx + 0.5;  
                anchorPos = i;
                textPos = i;
              }
              else
              {
                minx = curx + w - 0.5;
                maxx = curx + w + 0.5;
                anchorPos = i + 1;
                textPos = i + 1;
              }  
              break;            
            }
            curx += w;
          }
        }
        break;

      case XW_TEX_LETTER:
      case XW_TEX_DIGIT:
      case XW_TEX_UNKNOWN:
        {
          QString fontname = getFontName();
          QByteArray tfmname = fontname.toAscii();
          XWTFMFile tfm(tfmname.constData());
          double curx = xr;
          for (int i = s; i < e; i++)
          {
            int c = text[i].unicode();
            double w = tfm.getWidth(c);
            double h = tfm.getHeight(c);
            double d = tfm.getDepth(c);
            if ((curx <= xA) && (xA <= (curx + w)))
            {
              miny = y - h;
              maxy = y + d;
              if (xA < (curx + 0.5 * w))
              {
                minx = curx - 0.5;              
                maxx = curx + 0.5;
                textPos = i;
              }
              else
              {
                minx = curx + w - 0.5;
                maxx = curx + w + 0.5;
                textPos = i + 1;
              }  
              break;            
            }

            curx += w;
          }
        }          
        break;

      default:
        miny = y - height;
        maxy = y + depth;
        if (xA < (x + 0.5 * width))
        {
          minx = x - 0.5;              
          maxx = x + 0.5;
          textPos = XW_TEX_POS_BEFORE;
          anchorPos = textPos;
        }
        else
        {
          minx = x + width - 0.5;              
          maxx = x + width + 0.5;
          textPos = XW_TEX_POS_AFTER;
          anchorPos = textPos;
        }        
        break;
    }

    box->updateCursor(minx, miny, maxx, maxy, textPos);
    box->setCurrentObject(this);
  }

  return ret;
}

int  XWTeXText::insert(QChar c)
{
  int ret = textPos;
  if (textPos < text.length())
    text.insert(textPos, c);
  else
    text.append(c);

  textPos = textPos + 1;
  anchorPos = textPos;
  return ret;
}

void XWTeXText::insert(int pos, const QString & t)
{
  text.insert(pos,t);
  textPos = pos + t.length();
  anchorPos = textPos;
}

void XWTeXText::insert(XWTeXText * obj)
{
  if (textPos == XW_TEX_POS_BEFORE)
  {
    obj->textPos = XW_TEX_POS_BEFORE;
    obj->prev = prev;
    if (prev)
      prev->next = obj;
    prev = obj;
    obj->next = this;
    if (parent())
    {
      XWTeXText * pobj = (XWTeXText*)(parent());
      if (this == pobj->head)
        pobj->head = obj;
    }
  }
  else if (textPos == XW_TEX_POS_AFTER || textPos == text.length())
  {
    obj->textPos = XW_TEX_POS_AFTER;
    obj->next = next;    
    obj->prev = this;
    if (next)
      next->prev = obj;
    next = obj;
    if (parent())
    {
      XWTeXText * pobj = (XWTeXText*)(parent());
      if (this == pobj->last)
      {
        pobj->last = obj;
        obj->next = 0;
      }        
    }
  }
  else
  {
    XWTeXText * ntext = new XWTeXText(keyWord,box,parent());
    ntext->text = text.mid(textPos,-1);
    text.remove(textPos,ntext->text.length());
    obj->next = ntext;
    ntext->prev = obj;
    obj->prev = this;
    next = obj;
  }
  
  obj->setParent(parent());
}

void XWTeXText::insertAtAfter(XWTeXText * obj)
{
  obj->next = next;
  if (next)
    next->prev = obj;

  obj->prev = this;
  next = obj;
  if (parent())
  {
    XWTeXText * pobj = (XWTeXText*)(parent());
    if (this == pobj->last)
    {
      pobj->last = obj;
      obj->next = 0;
    }      
  }
  obj->setParent(parent());
}

bool XWTeXText::moveToNextChar()
{
  if (keyWord != XW_TEX_LETTER && 
      keyWord != XW_TEX_TEXT &&
      keyWord != XW_TEX_DIGIT)
    return false;

  if (textPos >= text.length())
    return false;

  double curx = x;
  double h = 0.0;
  double d = 0.0;
  int i = 0;
  if (keyWord == XW_TEX_TEXT)
  {
    QFont font = QApplication::font();
    font.setBold(bold);
    font.setItalic(italic);
    font.setPointSize(fontSize);
    if (slant)
      font.setStyle(QFont::StyleOblique);
    if (sc)
      font.setCapitalization(QFont::SmallCaps);
    if (sf)
      font.setStyleHint(QFont::SansSerif);
    QFontMetricsF metrics(font);
    
    for (; i <= textPos; i++)
      curx += metrics.width(text[i]);

    h = height;
  }
  else
  {
    QString fontname = getFontName();
    QByteArray tfmname = fontname.toAscii();
    XWTFMFile tfm(tfmname.constData());
    for (; i <= textPos; i++)
    {
      double w = tfm.getWidth(text[i].unicode());
      h = tfm.getHeight(text[i].unicode());
      d = tfm.getDepth(text[i].unicode());
      curx += w;
    }
  }

  textPos = i;
  box->updateCursor(curx - 0.5, y - h, curx + 0.5, y  + d, textPos);
  
  return true;
}

bool XWTeXText::moveToPrevChar()
{
  if (keyWord != XW_TEX_LETTER && 
      keyWord != XW_TEX_TEXT &&
      keyWord != XW_TEX_DIGIT)
    return false;

  if (textPos <= 0)
    return false;

  int i = 0;
  double curx = x;
  double h = 0.0;
  double d = 0.0;
  if (keyWord == XW_TEX_TEXT)
  {
    QFont font = QApplication::font();
    font.setBold(bold);
    font.setItalic(italic);
    font.setPointSize(fontSize);
    if (slant)
      font.setStyle(QFont::StyleOblique);
    if (sc)
      font.setCapitalization(QFont::SmallCaps);
    if (sf)
      font.setStyleHint(QFont::SansSerif);
    QFontMetricsF metrics(font);
    
    for (; i < textPos - 1; i++)
      curx += metrics.width(text[i]);

    h = height;
  }
  else
  {
    QString fontname = getFontName();
    QByteArray tfmname = fontname.toAscii();
    XWTFMFile tfm(tfmname.constData());
    for (; i < textPos - 1; i++)
    {
      double w = tfm.getWidth(text[i].unicode());
      h = tfm.getHeight(text[i].unicode());
      d = tfm.getDepth(text[i].unicode());
      curx += w;
    }
  }

  textPos = i;

  box->updateCursor(curx - 0.5, y - h, curx + 0.5, y + d, i);
  
  return true;
}

void XWTeXText::remove()
{
  if (prev && next && prev->keyWord == next->keyWord && 
     (prev->keyWord == XW_TEX_LETTER || 
      prev->keyWord == XW_TEX_DIGIT ||
      prev->keyWord == XW_TEX_TEXT))
  {
    prev->next = next->next;
    if (next->next)
      next->next->prev = prev;

    textPos = prev->text.length();
    anchorPos = textPos;
    prev->append(next->text);
  }
  else
  {
    if (prev)
      prev->next = next;
    else if (parent())
    {
      XWTeXText * pobj = (XWTeXText*)(parent());
      pobj->head = next;
      if (next)
        next->prev = 0;
    }

    if (next)
      next->prev = prev;
    else if (parent())
    {
      XWTeXText * pobj = (XWTeXText*)(parent());
      pobj->last = prev;
      if (prev)
        prev->next = 0;
    }  
  } 
}

void XWTeXText::remove(int pos, int len)
{
  text.remove(pos,len);
}

void XWTeXText::scan(const QString & str, int & len, int & pos)
{
  switch (keyWord)
  {
    case XW_TEX_FOMULAR:
    case XW_TEX_DISFOMULAR:
      scanFormular(str,len,pos);
      break;

    case XW_TEX_TEXT:
    case XW_TEX_LETTER:
      scanText(str,len,pos);
      break;

    case XW_TEX_DIGIT:
      scanDigit(str,len,pos);
      break;

    case XW_TEX_ASE:
      scanASE(str,len,pos);
      break;

    case XW_TEX_SUB:
    case XW_TEX_SUP:
      scanScript(str,len,pos);
      break;

    case XW_TEX_PARAM:
      scanParam(str,len,pos);
      break;

    case XW_TEX_OPT:
      scanOption(str,len,pos);
      break;

    case XW_TEX_ROW:
      scanRow(str,len,pos);
      break;

    case XW_TEX_COL:
      scanColumn(str,len,pos);
      break;

    case LAfrac:
    case LAstackrel:
      head->scanParam(str,len,pos);
      head->next->scanParam(str,len,pos);
      break;

    case LAsqrt:
      head->scanOption(str,len,pos);
      head->next->scanParam(str,len,pos);
      break;

    case LAleft:
    case LAright:
    case LAbig:
    case LABig:
    case LAbigg:
    case LABigg:
      if (str[pos] == QChar('\\'))
      {
        QString key = XWTeXBox::scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        XWTeXText * obj = new XWTeXText(id,box,this);
        append(obj);
        obj->scan(str,len,pos);
      }
      else
        text.append(str[pos++]);
      break;

    case LAoverbrace:
    case LAoverline:
    case LAunderbrace:
    case LAunderline:
    case LAmathrm:
    case LAmathbf:
    case LAmathsf:
    case LAmathtt:
    case LAmathit:
    case LAmathcal:
    case LAmathnormal:
    case LAhat:
    case LAcheck:
    case LAtilde:
    case LAgrave:
    case LAdot:
    case LAddot:
    case LAbar:
    case LAvec:
    case LAwidehat:
    case LAacute:
    case LAbreve:
    case LAwidetilde:
    case LAoverrightarrow:
    case LAunderrightarrow:
    case LAoverleftarrow:
    case LAunderleftarrow:
    case LAoverleftrightarrow:
    case LAunderleftrightarrow:
      head->scanParam(str,len,pos);
      break;

    case LAarray:
      head->scanParam(str,len,pos);
      scanArray(str,len,pos);
      break;

    case LAeqnarray:
    case LAeqnarrayStar:
      scanArray(str,len,pos);
      break;

    case LAequation:
      scanEnvironment(str,len,pos);
      break;

    case LAarccos:
    case LAarcsin:
    case LAarctan:
    case LAarg:    
    case LAcos:
    case LAcosh:
    case LAcot:
    case LAcoth:
    case LAcsc:
    case LAdeg:
    case LAdet:
    case LAdim:
    case LAexp:
    case LAgcd:
    case LAhom:
    case LAinf:
    case LAker:
    case LAlg:    
    case LAln:
    case LAlog:
    case LAmax:
    case LAmin:
    case LAPr:
    case LAsec:
    case LAsin:
    case LAsinh:
    case LAsup:
    case LAtan:
    case LAtanh:
    case LAlim:
    case LAliminf:
    case LAlimsup:
      text = getLaTeXString(keyWord);
      break;

    case LAbmod:
    case LApmod:
      text = "mod";
      break;

    case LAtextit:
	  case LAtextsl:
	  case LAtextbf:
	  case LAtextsf:
	  case LAtextsc:
	  case LAtexttt:
	  case LAtextmd:
	  case LAtextup:
	  case LAtextrm:
	  case LAtextnormal:
    case LAem:
	  case LAit:
	  case LAsl:
	  case LAbf:
	  case LAsf:
	  case LAsc:
	  case LAtt:
	  case LAcal:
	  case LArm:
    case LAtiny:
	  case LAscriptsize:
	  case LAfootnotesize:
	  case LAsmall:
	  case LAnormalsize:
	  case LAlarge:
	  case LALarge:
	  case LALARGE:
	  case LAhuge:
	  case LAHuge:
      {
        while (str[pos].isSpace())
          pos++;

        XWTeXText * obj = 0;
        if (str[pos] == QChar('{'))
          obj = new XWTeXText(XW_TEX_PARAM,box,this);
        else
          obj = new XWTeXText(XW_TEX_TEXT,box,this);
        append(obj);
        obj->scan(str,len,pos);
      }
      break;

    default:
      break;
  }

  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str, len, pos);  
    else
    {
      XWTeXText * obj = 0;
      if (str[pos] != QChar('^') && str[pos] != QChar('_'))
        break;

      if (str[pos] == QChar('^'))
        obj = new XWTeXText(XW_TEX_SUP,box,this);
      else
        obj = new XWTeXText(XW_TEX_SUB,box,this);
      pos++;

      append(obj);
      obj->scanScript(str,len,pos);
    }
  }
}

void XWTeXText::setFontSize(int s)
{
  fontSize = s;
}

void XWTeXText::setXY(double & xA, double yA)
{
  x = xA;
  y = yA;
  double tmpx = xA;
  double tmpy = yA;
  switch (keyWord)
  {
    case XW_TEX_TEXT:
    case XW_TEX_LETTER:
    case XW_TEX_DIGIT:
    case XW_TEX_UNKNOWN:
      if (head)
      {
        XWTeXText * obj = head;
        if (obj)
        {
          while (obj)
          {
            tmpx = x + width2 + 2;
            if (obj->keyWord == XW_TEX_SUP)
              tmpy = y + (height - obj->height);
            else
              tmpy = y - depth2;          
          
            obj->setXY(tmpx, tmpy);
            obj = obj->next;
          }
        }
      }
      xA += width;
      updateCursor(xA, yA);
      break;

    case XW_TEX_ASE:
      xA += width;
      updateCursor(xA, yA);
      break;

    case XW_TEX_FOMULAR:
    case XW_TEX_DISFOMULAR:
    case XW_TEX_PARAM:
    case XW_TEX_OPT:
    case LAequation:
    case LAmathrm:
    case LAmathbf:
    case LAmathsf:
    case LAmathtt:
    case LAmathit:
    case LAmathcal:
    case LAmathnormal:
    case XW_TEX_SUB:
    case XW_TEX_SUP:
    case LAeqnarray:
    case LAeqnarrayStar:
    case XW_TEX_ROW:
    case LAtextit:
	  case LAtextsl:
	  case LAtextbf:
	  case LAtextsf:
	  case LAtextsc:
	  case LAtexttt:
	  case LAtextmd:
	  case LAtextup:
	  case LAtextrm:
	  case LAtextnormal:
    case LAem:
	  case LAit:
	  case LAsl:
	  case LAbf:
	  case LAsf:
	  case LAsc:
	  case LAtt:
	  case LAcal:
	  case LArm:
    case LAtiny:
	  case LAscriptsize:
	  case LAfootnotesize:
	  case LAsmall:
	  case LAnormalsize:
	  case LAlarge:
	  case LALarge:
	  case LALARGE:
	  case LAhuge:
	  case LAHuge:
      {
        XWTeXText * obj = head;        
        while (obj)
        {
          obj->setXY(tmpx, tmpy);
          obj = obj->next;
        }
        xA += width;
        updateCursor(xA, yA);
      }
      break;

    case XW_TEX_COL:    
      {
        switch (align)
        {
          case XW_TEX_CENTER:
            x = xA + (width2 - width) / 2;
            break;

          case XW_TEX_RIGHT:
            x = xA + (width2 - width);
            break;

          default:
            break;
        }

        tmpx = x;
        tmpy = y;
        XWTeXText * obj = head;        
        while (obj)
        {
          obj->setXY(tmpx,tmpy);          
          obj = obj->next;
        }     
        xA += width2;
        if (next)
          xA += fontSize;   
      }
      break;

    case LAarray:
      {
        XWTeXText * obj = head->next; 
        tmpy = y + height; 
        while (obj)
        {
          tmpx = x;
          tmpy -= obj->height;
          obj->setXY(tmpx, tmpy);
          tmpy -= obj->depth - fontSize;
          obj = obj->next;
        }
        xA += width;
        updateCursor(xA, yA);
      }
      break;

    case LAbmod:
      x += 4;      
      xA = x;
      width -= 8;
      xA += width;
      updateCursor(xA, yA);
      xA += 4;
      break;

    case LApmod:
      xA += 4;
      width -= 4;
      xA += width;
      updateCursor(xA, yA);
      break;

    case LAfrac:
      tmpx = x + (width - head->width) / 2;
      tmpy = y - 3 - head->depth;
      head->setXY(tmpx, tmpy);
      tmpx = x + (width - head->next->width) / 2;
      tmpy = y + 2 + head->next->height;
      head->next->setXY(tmpx, tmpy);
      xA += width;
      updateCursor(xA, yA);
      break;

    case LAstackrel:
      tmpx = x + (width - head->width) / 2;
      tmpy = y - head->depth;
      head->setXY(tmpx, tmpy);
      tmpx = x + (width - head->next->width) / 2;
      tmpy = y + head->next->height;
      head->next->setXY(tmpx, tmpy);
      xA += width;
      updateCursor(xA, yA);
      break;

    case LAsqrt:
      {
        tmpx = x + width2 - head->width - 2;
        tmpy = y + head->next->height;
        head->setXY(tmpx, tmpy);
        tmpx = x + width2;
        tmpy = y;
        head->next->setXY(tmpx, tmpy);
        xA += width;
        updateCursor(xA, yA);
      }
      break;

    case LAleft:
    case LAright:
    case LAbig:
    case LABig:
    case LAbigg:
    case LABigg:
      if (text.isEmpty())
        head->setXY(tmpx, tmpy);
      else
      {
        XWTeXText * obj = head;
        while (obj)
        {
          tmpx = x + width2 + 2;
          if (obj->keyWord == XW_TEX_SUP)
            tmpy = y + (height - obj->height);
          else
            tmpy = y - depth2;          
          
          obj->setXY(tmpx, tmpy);
          obj = obj->next;
        }
      }
      updateCursor(xA, yA);
      xA += width;      
      break;

    case LAsup:
    case LAinf:    
    case LAsum:
    case LAbigcup:
    case LAbigvee:
    case LAprod:
    case LAbigcap:
    case LAbigwedge:
    case LAcoprod:
    case LAbigsqcup:
    case LAbiguplus:
      {
        XWTeXText * obj = head;
        while (obj)
        {
          if (obj->width > width2)
            tmpx = x - (obj->width - width2) / 2;
          else
            tmpx = x + (width2 - obj->width) / 2;

          if (obj->keyWord == XW_TEX_SUP)
            tmpy = y + height2 + obj->depth;
          else
            tmpy = y - depth2 - obj->height;

          obj->setXY(tmpx, tmpy);
          obj = obj->next;
        }
        xA += width;
        updateCursor(xA, yA);
      }
      break;

    case LAlim:
    case LAliminf:
    case LAlimsup:
      {
        XWTeXText * obj = head;
        while (obj)
        {
          if (obj->width > width2)
            tmpx = x - (obj->width - width2) / 2;
          else
            tmpx = x + (width2 - obj->width) / 2;

          if (obj->keyWord == XW_TEX_SUP)
            tmpy = y + height2 + obj->depth;
          else
            tmpy = y - depth2 - obj->height;

          obj->setXY(tmpx, tmpy);
          obj = obj->next;
        }
        xA += width;
        xA += 2;
        updateCursor(xA, yA);
      }
      break;

    case LAoverrightarrow:
    case LAoverleftarrow:
    case LAoverleftrightarrow:
    case LAoverbrace:
    case LAoverline:
    case LAunderrightarrow:    
    case LAunderleftarrow:    
    case LAunderleftrightarrow:    
    case LAunderbrace:    
    case LAunderline:
      head->setXY(tmpx, tmpy);
      {
        XWTeXText * obj = head->next;
        while (obj)
        {
          if (obj->width > width2)
            tmpx = x - (obj->width - width2) / 2;
          else
            tmpx = x + (width2 - obj->width) / 2;

          if (obj->keyWord == XW_TEX_SUP)
            tmpy = y + head->height + height2 + depth2 + obj->depth;
          else
            tmpy = y - head->depth - height2 - depth2 - obj->height;

          obj->setXY(tmpx, tmpy);
          obj = obj->next;
        }
      }
      xA += width;
      updateCursor(xA, yA);
      break;

    case LAhat:
    case LAcheck:
    case LAtilde:
    case LAgrave:
    case LAdot:
    case LAddot:
    case LAbar:
    case LAvec:
    case LAwidehat:
    case LAacute:
    case LAbreve:
    case LAwidetilde:
      head->setXY(tmpx, tmpy);
      if (head->next)
      {
        XWTeXText * obj = head->next;
        while (obj)
        {
          tmpx = x + head->width + 2;
          if (obj->keyWord == XW_TEX_SUP)
            tmpy = y + (height - obj->height);
          else
            tmpy = y - depth2;          
          
          obj->setXY(tmpx, tmpy);
          obj = obj->next;
        }
      }
      xA += width;
      updateCursor(xA, yA);
      break;

    default:
      if (head)
      {
        XWTeXText * obj = head;
        while (obj)
        {
          tmpx = x + width2 + 2;
          if (obj->keyWord == XW_TEX_SUP)
            tmpy = y + (height - obj->height);
          else
            tmpy = y - depth2;          
          
          obj->setXY(tmpx, tmpy);
          obj = obj->next;
        }
      }
      xA += width;
      updateCursor(xA, yA);
      break;
  }
}

void XWTeXText::aseDimension()
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  width = tfm.getWidth(text[0].unicode());
  height = tfm.getHeight(text[0].unicode());
  depth = tfm.getDepth(text[0].unicode());
  width2 = width;
  height2 = height;
  depth2 = depth;
  scriptDimension();
}

void XWTeXText::amsaDimension(int c)
{
  charDimension(XW_TEX_FONT_MSAM, c);
}

void XWTeXText::amsaDimensionLR(int c1, int c2, int c3)
{
  charsThreeLR(XW_TEX_FONT_MSAM,c1,c2,c3);
}

void XWTeXText::amsbDimension(int c)
{
  charDimension(XW_TEX_FONT_MSBM, c);
}

void XWTeXText::cdotsDimension()
{
  charsThreeLR(XW_TEX_FONT_CMSY,0x01);
}

void XWTeXText::charDimension(int fam, int c)
{
  family = fam;
  text.resize(1);
  text[0] = c;
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  width = tfm.getWidth(text[0].unicode());
  height = tfm.getHeight(text[0].unicode());
  depth = tfm.getDepth(text[0].unicode());
  width2 = width;
  height2 = height;
  depth2 = depth;
  scriptDimension();
}

void XWTeXText::charsFourLR(int fam, int c1, int c2, int c3, int c4)
{
  family = fam;
  text.resize(4);
  text[0] = c1;
  text[1] = c2;
  text[2] = c3;
  text[3] = c4;
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  head->getDimension();
  width = qMax(head->width,tfm.getWidth(text[0].unicode()) + tfm.getWidth(text[1].unicode()) + tfm.getWidth(text[2].unicode()) + tfm.getWidth(text[3].unicode()));
  height = qMax(tfm.getWidth(text[0].unicode()),tfm.getWidth(text[1].unicode()));
  height = qMax(height,tfm.getWidth(text[2].unicode()));
  height = qMax(height,tfm.getWidth(text[3].unicode()));  
  depth = qMax(tfm.getDepth(text[0].unicode()), tfm.getDepth(text[1].unicode()));
  depth = qMax(depth, tfm.getDepth(text[2].unicode()));
  depth = qMax(depth, tfm.getDepth(text[3].unicode()));
  if (keyWord == LAoverbrace)
  {
    height += head->height + depth;
    depth = head->depth;
  }
  else
  {
    depth = head->depth + height + depth;
    height = head->height;    
  }

  width2 = width;
  height2 = height;
  depth2 = depth;
  scriptDimension();
}

void XWTeXText::charsThreeLR(int fam, int c)
{
  family = fam;
  text.resize(3);
  text[0] = c;
  text[1] = c;
  text[2] = c;
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  width = 3 * tfm.getWidth(text[0].unicode());
  height = tfm.getHeight(text[0].unicode());
  depth = tfm.getDepth(text[0].unicode());
  width2 = width;
  height2 = height;
  depth2 = depth;
  scriptDimension();
}

void XWTeXText::charsThreeLR(int fam, int c1, int c2, int c3)
{
  family = fam;
  text.resize(3);
  text[0] = c1;
  text[1] = c2;
  text[2] = c3;
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  width = tfm.getWidth(text[0].unicode()) + tfm.getWidth(text[1].unicode()) + tfm.getWidth(text[2].unicode());
  height = qMax(tfm.getHeight(text[0].unicode()),tfm.getHeight(text[1].unicode()));
  height = qMax(height,tfm.getHeight(text[2].unicode()));
  depth = qMax(tfm.getDepth(text[0].unicode()),tfm.getDepth(text[1].unicode()));
  depth = qMax(depth,tfm.getDepth(text[2].unicode()));
  width2 = width;
  height2 = height;
  depth2 = depth;
  scriptDimension();
}

void XWTeXText::charTwo(int fam, int c1, int c2)
{
  family = fam;
  text.resize(2);
  text[0] = c1;
  text[1] = c2;
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  width = qMax(tfm.getWidth(text[0].unicode()),tfm.getWidth(text[1].unicode()));
  height = qMax(tfm.getHeight(text[0].unicode()),tfm.getHeight(text[1].unicode()));
  depth = qMax(tfm.getDepth(text[0].unicode()),tfm.getDepth(text[1].unicode()));
  width2 = width;
  height2 = height;
  depth2 = depth;
  scriptDimension();
}

void XWTeXText::charTwo(int fam, int c1, int fam2, int c2)
{
  family = fam;
  family2 = fam2;
  text.resize(2);
  text[0] = c1;
  text[1] = c2;
  QString fontname = getFontName(family,fontSize,bold,slant,italic,cal);
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm1(tfmname.constData());
  fontname = getFontName(family2,fontSize,bold,slant,italic,cal);
  tfmname = fontname.toAscii();
  XWTFMFile tfm2(tfmname.constData());
  width = qMax(tfm1.getWidth(text[0].unicode()),tfm2.getWidth(text[1].unicode()));
  height = qMax(tfm1.getHeight(text[0].unicode()),tfm2.getHeight(text[1].unicode()));
  depth = qMax(tfm1.getDepth(text[0].unicode()),tfm2.getDepth(text[1].unicode()));  
  width2 = width;
  height2 = height;
  depth2 = depth;
  scriptDimension();
}

void XWTeXText::charsTwoLR(int fam, int c1, int c2)
{
  family = fam;
  text.resize(2);
  text[0] = c1;
  text[1] = c2;
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  width = tfm.getWidth(text[0].unicode()) + tfm.getWidth(text[1].unicode());
  height = qMax(tfm.getHeight(text[0].unicode()),tfm.getHeight(text[1].unicode()));
  depth = qMax(tfm.getDepth(text[0].unicode()),tfm.getDepth(text[1].unicode()));  
  width2 = width;
  height2 = height;
  depth2 = depth;
  scriptDimension();
}

void XWTeXText::charsTwoLR(int fam, int c1, int fam2, int c2)
{
  family = fam;
  family2 = fam2;
  text.resize(2);
  text[0] = c1;
  text[1] = c2;
  QString fontname = getFontName(family,fontSize,bold,slant,italic,cal);
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm1(tfmname.constData());
  fontname = getFontName(family2,fontSize,bold,slant,italic,cal);
  tfmname = fontname.toAscii();
  XWTFMFile tfm2(tfmname.constData());
  width = tfm1.getWidth(text[0].unicode()) + tfm2.getWidth(text[1].unicode());
  height = qMax(tfm1.getHeight(text[0].unicode()),tfm2.getHeight(text[1].unicode()));
  depth = qMax(tfm1.getDepth(text[0].unicode()),tfm2.getDepth(text[1].unicode()));    
  width2 = width;
  height2 = height;
  depth2 = depth;
  scriptDimension();
}

void XWTeXText::charsTwoTB(int fam, int c1, int fam2, int c2)
{
  family = fam;
  family2 = fam2;
  text.resize(2);
  text[0] = c1;
  text[1] = c2;
  QString fontname = getFontName(family,fontSize,bold,slant,italic,cal);
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm1(tfmname.constData());
  fontname = getFontName(family2,fontSize,bold,slant,italic,cal);
  tfmname = fontname.toAscii();
  XWTFMFile tfm2(tfmname.constData());
  width = qMax(tfm1.getWidth(text[0].unicode()),tfm2.getWidth(text[1].unicode()));
  height = tfm1.getHeight(text[0].unicode()) + tfm1.getDepth(text[0].unicode()) + tfm2.getHeight(text[1].unicode());
  depth = tfm2.getDepth(text[1].unicode());  
  width2 = width;
  height2 = height;
  depth2 = depth;
  scriptDimension();
}

void XWTeXText::ddotsDimension()
{
  family = XW_TEX_FONT_CMR;
  text.resize(3);
  text[0] = 0x2E;
  text[1] = 0x2E;
  text[2] = 0x2E;
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  width = 3 * tfm.getWidth(text[0].unicode());
  height = 3 * tfm.getHeight(text[0].unicode()) + 2 * tfm.getDepth(text[0].unicode());
  depth = tfm.getDepth(text[0].unicode());  
  width2 = width;
  height2 = height;
  depth2 = depth;
  scriptDimension();
}

void XWTeXText::delimiterDimension()
{
  switch (keyWord)
  {
    case LAbig:
      height = 8.5;
      fontSize = XW_TEX_FONT_SMALL;
      break;

    case LABig:
      height = 11.5;
      fontSize = XW_TEX_FONT_large;
      break;

    case LAbigg:
      height = 14.5;
      fontSize = XW_TEX_FONT_Large;
      break;

    case LABigg: 
      height = 17.5;
      fontSize = XW_TEX_FONT_LARGE;
      break;
  }

  if (text.isEmpty())
  {
    head->fontSize = fontSize;
    head->getDimension();
    width = head->width;
  }
  else
  {
    if (text[0] == QChar('('))
      operatorDimension(0x28,0x00);
    else if (text[0] == QChar(')'))
      operatorDimension(0x29,0x01);
    else if (text[0] == QChar('['))
      operatorDimension(0x5B,0x02);
    else if (text[0] == QChar(']'))
      operatorDimension(0x5D,0x03);
    else if (text[0] == QChar('<'))
      symbolDimension(0x68,0x0A);
    else if (text[0] == QChar('>'))
      symbolDimension(0x69,0x0B);
    else if (text[0] == QChar('/'))
      letterDimension(0x2F);
    else if (text[0] == QChar('|'))
      symbolDimension(0x6A,0x0C);
  }  
}

void XWTeXText::doChildren(XWPDFDriver * driver, double xA, double yA,int s, int e)
{
  XWTeXText * obj = head;
  while (obj)
  {
    obj->doText(driver,xA,yA,s,e);
    obj = obj->next;
  }
}

void XWTeXText::dotsDimension()
{
  charsThreeLR(XW_TEX_FONT_CMR,0x2E);
}

void XWTeXText::drawAccent(QPainter * )
{}

void XWTeXText::drawChar(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  double h2 = tfm.getHeight(text[0].unicode());
  XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    painter->setPen(pen);
    font->drawChar(painter,x,y + h2,text[0].unicode());
    painter->restore();
  }
  else
    font->drawChar(painter,x,y + h2,text[0].unicode());
  
  drawChildren(painter);
}

void XWTeXText::drawChildren(QPainter * painter)
{
  XWTeXText * obj = head;
  while (obj)
  {
    obj->draw(painter);
    obj = obj->next;
  }
}

void XWTeXText::drawDDots(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  double w2 = tfm.getWidth(text[0].unicode());
  double h2 = tfm.getHeight(text[0].unicode());
  double d2 = tfm.getDepth(text[0].unicode());
  XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    painter->setPen(pen);
    font->drawChar(painter,x,y + 3 * h2 + d2,text[0].unicode());
    font->drawChar(painter,x + w2,y + 2 * h2 + d2,text[0].unicode());
    font->drawChar(painter,x + 2 * w2, y + h2,text[0].unicode());
    painter->restore();
  }
  else
  {
    font->drawChar(painter,x,y + 3 * h2 + d2,text[0].unicode());
    font->drawChar(painter,x + w2,y + 2 * h2 + d2,text[0].unicode());
    font->drawChar(painter,x + 2 * w2, y + h2,text[0].unicode());
  }
  
  drawChildren(painter);
}

void XWTeXText::drawDotsLR(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  double w2 = tfm.getWidth(text[0].unicode());
  double h2 = tfm.getHeight(text[0].unicode());
  XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    painter->setPen(pen);
    font->drawChar(painter,x,y + h2,text[0].unicode());
    font->drawChar(painter,x + w2, y + h2,text[0].unicode());
    font->drawChar(painter,x + 2 * w2, y + h2,text[0].unicode());
    painter->restore();
  }
  else
  {
    font->drawChar(painter,x,y + h2,text[0].unicode());
    font->drawChar(painter,x + w2, y + h2,text[0].unicode());
    font->drawChar(painter,x + 2 * w2, y + h2,text[0].unicode());
  }
  
  drawChildren(painter);
}

void XWTeXText::drawHookArrow(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  double w2 = tfm.getWidth(text[0].unicode());
  double h2 = tfm.getHeight(text[0].unicode());
  double h3 = tfm.getHeight(text[1].unicode());
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    painter->setPen(pen);
    XWFTFont * font1 = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
    font1->drawChar(painter,x,y + h2,text[0].unicode());
    XWFTFont * font2 = box->loadFTFont(family2,fontSize,bold,slant,italic,cal);
    font2->drawChar(painter,x + w2,y + h3,text[1].unicode());
    drawChildren(painter);
    painter->restore();
  }
  else
  {
    XWFTFont * font1 = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
    font1->drawChar(painter,x,y + h2,text[0].unicode());
    XWFTFont * font2 = box->loadFTFont(family2,fontSize,bold,slant,italic,cal);
    font2->drawChar(painter,x + w2,y + h3,text[1].unicode());
    drawChildren(painter);
  }
  
  drawChildren(painter);
}

void XWTeXText::drawLeft(QPainter * painter)
{
  double sy = 1;
  painter->save();
  if (text.isEmpty())
  {
     if (next)
       sy = (next->height + next->depth) / (head->height + head->depth);
     
     painter->scale(1,sy);
     head->draw(painter);     
  }
  else
  {
    if (next)
       sy = (next->height + next->depth) / (height2 + depth2);

    painter->scale(1,sy);
    drawChar(painter);
  }  

  painter->restore();
}

void XWTeXText::drawNormalText(QPainter * painter)
{
  QFont font = QApplication::font();
  font.setBold(bold);
  font.setItalic(italic);
  font.setPointSize(fontSize);
  if (slant)
    font.setStyle(QFont::StyleOblique);
  if (sc)
    font.setCapitalization(QFont::SmallCaps);
  if (sf)
    font.setStyleHint(QFont::SansSerif);
  if (hasSelect())
  {
    QFontMetricsF metrics(font);
    if (anchorPos == 0)
    {
      if (textPos == text.length())
      {
        painter->save();
        painter->fillRect(x,y,width,height + depth,Qt::black);
        QPen pen = painter->pen();
        pen.setColor(Qt::white);
        painter->setPen(pen);
        painter->drawText(x,y + height,text);
        painter->restore();
      }        
      else
      {
        painter->save();
        QPen pen = painter->pen();
        pen.setColor(Qt::white);
        painter->setPen(pen);
        QString ss = text.left(textPos);
        double w = metrics.width(ss);
        painter->fillRect(x,y,w,height,Qt::black);
        painter->drawText(x,y + height,ss);
        painter->restore();
        ss = text.mid(textPos);
        painter->drawText(x + w,y + height,ss);
      }      
    }
    else
    {
      QString ss = text.left(anchorPos);
      painter->drawText(x,y + height,ss);      
      if (textPos == text.length())
      {
        painter->save();        
        double w = metrics.width(ss);
        painter->fillRect(x + w,y,width - w,height,Qt::black);
        QPen pen = painter->pen();
        pen.setColor(Qt::white);
        painter->setPen(pen);
        ss = text.mid(anchorPos,-1);
        painter->drawText(x + w,y + height,ss);
        painter->restore();
      }
      else
      {
        double w = metrics.width(ss);
        ss = text.mid(anchorPos,textPos);
        QString ps = text.mid(textPos,-1);        
        double w2 = metrics.width(ps);
        painter->save();
        painter->fillRect(x + w,y,width - w - w2,height,Qt::black);
        QPen pen = painter->pen();
        pen.setColor(Qt::white);
        painter->setPen(pen);
        painter->drawText(x + w,y + height,ss);
        painter->restore();
        painter->drawText(x + width - w2,y + height,ps);
      }
    }        
  }
  else
    painter->drawText(x,y + height,text);
}

void XWTeXText::drawNoteq(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  double h2 = tfm.getHeight(text[1].unicode());
  double h3 = tfm.getHeight(text[1].unicode());
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    painter->setPen(pen);
    XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
    font->drawChar(painter,x,y + h2,text[0].unicode());
    font = box->loadFTFont(family2,fontSize,bold,slant,italic,cal);
    font->drawChar(painter,x,y + h3,text[1].unicode());
    painter->restore();
  }
  else
  {
    XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
    font->drawChar(painter,x,y + h2,text[0].unicode());
    font = box->loadFTFont(family2,fontSize,bold,slant,italic,cal);
    font->drawChar(painter,x,y + h3,text[1].unicode());
  }  
}

void XWTeXText::drawNotin(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  double h2 = tfm.getHeight(text[0].unicode());
  double h3 = tfm.getHeight(text[1].unicode());
  XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    painter->setPen(pen);
    font->drawChar(painter,x,y + h2,text[0].unicode());
    font->drawChar(painter,x,y + h3,text[1].unicode());
    painter->restore();
  }
  else
  {
    font->drawChar(painter,x,y + h2,text[0].unicode());
    font->drawChar(painter,x,y + h3,text[1].unicode());
  }  
}

void XWTeXText::drawOverBrace(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  double w2 = tfm.getWidth(text[0].unicode());
  double h2 = tfm.getHeight(text[0].unicode());
  double d2 = tfm.getDepth(text[0].unicode());
  double w3 = tfm.getWidth(text[1].unicode());
  double w4 = tfm.getWidth(text[2].unicode());
  double w5 = tfm.getWidth(text[3].unicode());
  drawChildren(painter);
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    pen.setWidth(2);
    painter->setPen(pen);
    double w =(head->width - (w2 + w3 + w4 + w5)) / 2;
    double tmpx = x;
    double tmpy = y + head->height + h2 + d2;
    XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
    font->drawChar(painter,tmpx,tmpy,text[0].unicode());
    tmpx += width2;
    painter->drawLine(tmpx,tmpy, tmpx + w,tmpy);
    tmpx += w;
    font->drawChar(painter,tmpx,tmpy,text[1].unicode());
    tmpx += w3;
    font->drawChar(painter,tmpx,tmpy,text[2].unicode());
    tmpx += w4;
    painter->drawLine(tmpx,tmpy, tmpx + w,y + tmpy);
    tmpx += w;
    font->drawChar(painter,tmpx,tmpy,text[3].unicode());
    painter->restore();
  }
  else
  {
    painter->save();
    QPen pen = painter->pen();
    pen.setWidth(2);
    painter->setPen(pen);
    double w =(head->width - (w2 + w3 + w4 + w5)) / 2;
    double tmpx = x;
    double tmpy = y + head->height + h2 + d2;
    XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
    font->drawChar(painter,tmpx,tmpy,text[0].unicode());
    tmpx += w2;
    painter->drawLine(tmpx,tmpy, tmpx + w,tmpy);
    tmpx += w;
    font->drawChar(painter,tmpx,tmpy,text[1].unicode());
    tmpx += w3;
    font->drawChar(painter,tmpx,tmpy,text[2].unicode());
    tmpx += w4;
    painter->drawLine(tmpx,tmpy, tmpx + w,y + tmpy);
    tmpx += w;
    font->drawChar(painter,tmpx,tmpy,text[3].unicode());
    painter->restore();
  }
}

void XWTeXText::drawOverTwoChars(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  double w2 = tfm.getWidth(text[0].unicode());
  double d2 = tfm.getDepth(text[0].unicode());
  drawChildren(painter);
  XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    painter->setPen(pen);
    font->drawChar(painter,x,y + head->height + d2,text[0].unicode());
    font->drawChar(painter,x + w2,y + head->height + d2,text[1].unicode());
    painter->restore();
  }
  else
  {
    font->drawChar(painter,x,y + head->height + d2,text[0].unicode());
    font->drawChar(painter,x + w2,y + head->height + d2,text[1].unicode());
  }  
}

void XWTeXText::drawRight(QPainter * painter)
{
  double sy = 1;
  painter->save();
  if (text.isEmpty())
  {
     if (prev)
       sy = (prev->height + prev->depth) / (head->height + head->depth);
     
     painter->scale(1,sy);
     head->draw(painter);     
  }
  else
  {
    if (prev)
       sy = (prev->height + prev->depth) / (height2 + depth2);

    painter->scale(1,sy);
    if (text[0] != QChar('.'))
      drawChar(painter);
  }  

  painter->restore();
}

void XWTeXText::drawText(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
  double w,h,d;
  double tmpx = x;
  for (int i = 0; i < text.length(); i++)
  {
    w = tfm.getWidth(text[i].unicode());
    h = tfm.getHeight(text[i].unicode());
    d = tfm.getDepth(text[i].unicode());
    if (i >= anchorPos && i < textPos)
    {
      painter->save();
      painter->fillRect(x,y,w,h + d,Qt::black);
      QPen pen = painter->pen();
      pen.setColor(Qt::white);
      painter->setPen(pen);
      font->drawChar(painter,tmpx,y + h,text[i].unicode());
      painter->restore();
    }
    else
      font->drawChar(painter,tmpx,y + h,text[i].unicode());
    tmpx += w;
  }

  drawChildren(painter);
}

void XWTeXText::drawText(QPainter * painter,  int n)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
  double w,h,d;
  double tmpx = x;
  int i = 0;
  for (; i < n; i++)
  {
    w = tfm.getWidth(text[i].unicode());
    h = tfm.getHeight(text[i].unicode());
    d = tfm.getDepth(text[i].unicode());
    if (i >= anchorPos && i < textPos)
    {
      painter->save();
      painter->fillRect(x,y,w,h + d,Qt::black);
      QPen pen = painter->pen();
      pen.setColor(Qt::white);
      painter->setPen(pen);
      font->drawChar(painter,tmpx,y + h,text[i].unicode());
      painter->restore();
    }
    else
      font->drawChar(painter,tmpx,y + h,text[i].unicode());
      
    tmpx += w;
  }

  tmpx += 2;

  for (; i < text.length(); i++)
  {
    w = tfm.getWidth(text[i].unicode());
    h = tfm.getHeight(text[i].unicode());
    d = tfm.getDepth(text[i].unicode());
    if (i >= anchorPos && i < textPos)
    {
      painter->save();
      painter->fillRect(x,y,w,h + d,Qt::black);
      QPen pen = painter->pen();
      pen.setColor(Qt::white);
      painter->setPen(pen);
      font->drawChar(painter,tmpx,y + h,text[i].unicode());
      painter->restore();
    }
    else
      font->drawChar(painter,tmpx,y + h,text[i].unicode());
    tmpx += w;
  }

  drawChildren(painter);
}

void XWTeXText::drawThreeCharsLR(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  double w2 = tfm.getWidth(text[0].unicode());
  double h2 = tfm.getHeight(text[0].unicode());
  double w3 = tfm.getWidth(text[1].unicode());
  double h3 = tfm.getHeight(text[1].unicode());
  double h4 = tfm.getHeight(text[2].unicode());
  XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    painter->setPen(pen);
    font->drawChar(painter,x,y + h2,text[0].unicode());
    font->drawChar(painter,x + w2,y + h3,text[1].unicode());
    font->drawChar(painter,x + w2 + w3,y + h4,text[2].unicode());
    painter->restore();
  }
  else
  {
    font->drawChar(painter,x,y + h2,text[0].unicode());
    font->drawChar(painter,x + w2,y + h3,text[1].unicode());
    font->drawChar(painter,x + w2 + w3,y + h4,text[2].unicode());
  }  
}

void XWTeXText::drawTwoCharsLR(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  double w2 = tfm.getWidth(text[0].unicode());
  double h2 = tfm.getHeight(text[0].unicode());
  double h3 = tfm.getHeight(text[1].unicode());
  XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    painter->setPen(pen);
    font->drawChar(painter,x,y + h2,text[0].unicode());
    font->drawChar(painter,x + w2,y + h3,text[1].unicode());
    painter->restore();
  }
  else
  {
    font->drawChar(painter,x,y + h2,text[0].unicode());
    font->drawChar(painter,x + w2,y + h3,text[1].unicode());
  }  
}

void XWTeXText::drawTwoCharsTB(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  double h2 = tfm.getHeight(text[0].unicode());
  double d2 = tfm.getDepth(text[0].unicode());
  double h3 = tfm.getHeight(text[1].unicode());
  XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    painter->setPen(pen);
    font->drawChar(painter,x,y + h3 + h2 + d2,text[0].unicode());
    font->drawChar(painter,x,y + h3,text[1].unicode());
    painter->restore();
  }
  else
  {
    font->drawChar(painter,x,y + h3 + h2 + d2,text[0].unicode());
    font->drawChar(painter,x,y + h3,text[1].unicode());
  }  
}

void XWTeXText::drawTwoTwoCharsTB(QPainter * painter)
{
  QString fontname = getFontName(family,fontSize,bold,slant,italic,cal);
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm1(tfmname.constData());
  double h2 = tfm1.getHeight(text[0].unicode());
  double d2 = tfm1.getDepth(text[0].unicode());
  fontname = getFontName(family2,fontSize,bold,slant,italic,cal);
  tfmname = fontname.toAscii();
  XWTFMFile tfm2(tfmname.constData());
  double h3 = tfm2.getHeight(text[1].unicode());
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    painter->setPen(pen);
    XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
    font->drawChar(painter,x,y + h3 + h2 + d2,text[0].unicode());
    font = box->loadFTFont(family2,fontSize,bold,slant,italic,cal);
    font->drawChar(painter,x,y + h3,text[1].unicode());
    painter->restore();
  }
  else
  {
    XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
    font->drawChar(painter,x,y + h3 + h2 + d2,text[0].unicode());
    font = box->loadFTFont(family2,fontSize,bold,slant,italic,cal);
    font->drawChar(painter,x,y + h3,text[1].unicode());
  }  
}

void XWTeXText::drawUnderBrace(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  double w2 = tfm.getWidth(text[0].unicode());
  double w3 = tfm.getWidth(text[1].unicode());
  double w4 = tfm.getWidth(text[2].unicode());
  double w5 = tfm.getWidth(text[3].unicode());
  drawChildren(painter);
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    pen.setWidth(2);
    painter->setPen(pen);
    double w =(head->width - (w2 + w3 + w4 + w5)) / 2;
    double tmpy = y - head->depth;
    double tmpx = x;
    XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
    font->drawChar(painter,tmpx,tmpy,text[0].unicode());
    tmpx += w2;
    painter->drawLine(tmpx,tmpy, tmpx + w,tmpy);
    tmpx += w;
    font->drawChar(painter,tmpx,tmpy,text[1].unicode());
    tmpx += w3;
    font->drawChar(painter,tmpx,tmpy,text[2].unicode());
    tmpx += w4;
    painter->drawLine(tmpx,tmpy, tmpx + w,tmpy);
    tmpx += w;
    font->drawChar(painter,tmpx,tmpy,text[3].unicode());
    painter->restore();
  }
  else
  {
    painter->save();
    QPen pen = painter->pen();
    pen.setWidth(2);
    painter->setPen(pen);
    double w =(head->width - (w2 + w3 + w4 + w5)) / 2;
    double tmpy = y - head->depth;
    double tmpx = x;
    XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
    font->drawChar(painter,tmpx,tmpy,text[0].unicode());
    tmpx += w2;
    painter->drawLine(tmpx,tmpy, tmpx + w,tmpy);
    tmpx += w;
    font->drawChar(painter,tmpx,tmpy,text[1].unicode());
    tmpx += w3;
    font->drawChar(painter,tmpx,tmpy,text[2].unicode());
    tmpx += w4;
    painter->drawLine(tmpx,tmpy, tmpx + w,tmpy);
    tmpx += w;
    font->drawChar(painter,tmpx,tmpy,text[3].unicode());
    painter->restore();
  }  
}

void XWTeXText::drawUnderTwoChars(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  double w2 = tfm.getWidth(text[0].unicode());
  drawChildren(painter);
  XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    painter->setPen(pen);
    font->drawChar(painter,x,y - head->depth,text[0].unicode());
    font->drawChar(painter,x + w2,y - head->depth,text[1].unicode());
    painter->restore();
  }
  else
  {
    font->drawChar(painter,x,y - head->depth,text[0].unicode());
    font->drawChar(painter,x + w2,y - head->depth,text[1].unicode());
  }
}

void XWTeXText::drawVDots(QPainter * painter)
{
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  double h2 = tfm.getHeight(text[0].unicode());
  double d2 = tfm.getDepth(text[0].unicode());
  XWFTFont * font = box->loadFTFont(family,fontSize,bold,slant,italic,cal);
  if (hasSelect())
  {
    painter->save();
    painter->fillRect(x,y,width,height + depth,Qt::black);
    QPen pen = painter->pen();
    pen.setColor(Qt::white);
    painter->setPen(pen);
    font->drawChar(painter,x,y + height2,text[0].unicode());
    font->drawChar(painter,x,y + (2 * h2 + d2),text[0].unicode());
    font->drawChar(painter,x,y + (3 * h2 + 2 * d2) ,text[0].unicode());
    painter->restore();
  }
  else
  {
    font->drawChar(painter,x,y + h2,text[0].unicode());
    font->drawChar(painter,x,y + (2 * h2 + d2),text[0].unicode());
    font->drawChar(painter,x,y + (3 * h2 + 2 * d2) ,text[0].unicode());
  }  
}

void XWTeXText::getMaxColumnWidth(QList<double> & list)
{
  XWTeXText * obj = head;
  int i = 0;
  while (obj)
  {
    if (list[i] < obj->width)
      list[i] = obj->width;
    i++;
    obj = obj->next;
  }
}

void XWTeXText::hookArrowDimension(int c1, int c2)
{
  charsTwoLR(XW_TEX_FONT_CMSY,c1,XW_TEX_FONT_CMM,c2);
}

void XWTeXText::largeSymbolDimension(int c)
{
  charDimension(XW_TEX_FONT_CMEX,c);
}

void XWTeXText::largeSymbolDimension(int och, int lsch)
{
  int ch = och;
  if (fontSize > XW_TEX_FONT_NORMAL)
    ch = lsch;

  charDimension(XW_TEX_FONT_CMEX, ch);
}

void XWTeXText::largeSymbolDimensionLR(int c1, int c2, int c3, int c4)
{
  charsFourLR(XW_TEX_FONT_CMEX,c1,c2,c3,c4);
}

void XWTeXText::lasyDimension(int c)
{
  charDimension(XW_TEX_FONT_LASY, c);
}

void XWTeXText::leftDimension()
{
  if (next)
    fontSize = (int)(next->height + next->depth);

  if (text.isEmpty())
  {
    head->fontSize = fontSize;
    head->getDimension();
    width = head->width;
  }
  else
  {
    if (text[0] == QChar('('))
      operatorDimension(0x28,0x00);
    else if (text[0] == QChar('['))
      operatorDimension(0x5D,0x03);
    else if (text[0] == QChar('<'))
      symbolDimension(0x69,0x0B);
    else if (text[0] == QChar('/'))
      letterDimension(0x2F);
    else if (text[0] == QChar('|'))
      symbolDimension(0x6A,0x0C);
  } 
}

void XWTeXText::lettersDimension()
{
  if (family == XW_TEX_FONT_CMTT)
    textDimension(XW_TEX_FONT_CMTT);
  else
    textDimension(XW_TEX_FONT_CMM);
}

void XWTeXText::letterDimension(int c)
{
  if (family == XW_TEX_FONT_CMTT)
    charDimension(XW_TEX_FONT_CMTT,c);
  else
    charDimension(XW_TEX_FONT_CMM, c);
}

void XWTeXText::letterDimensionLR(int c1, int c2)
{
  charsTwoLR(XW_TEX_FONT_CMM,c1,c2);
}

void XWTeXText::LongArrowDimension(int c1, int c2)
{
  charsTwoLR(XW_TEX_FONT_CMSY,c1,XW_TEX_FONT_CMR,c2);
}

void XWTeXText::modelsDimension()
{
  charsTwoLR(XW_TEX_FONT_CMSY,0x6A,XW_TEX_FONT_CMR,0x3D);
}

void XWTeXText::neqDimension()
{
  charTwo(XW_TEX_FONT_CMSY,0x36,XW_TEX_FONT_CMR,0x3D);
}

void XWTeXText::notinDimension()
{
  charTwo(XW_TEX_FONT_CMSY,0x32,0x36);
}

void XWTeXText::operatorsDimension()
{
  if (family == XW_TEX_FONT_CMTT)
    textDimension(XW_TEX_FONT_CMTT);
  else
    textDimension(XW_TEX_FONT_CMR);
}

void XWTeXText::operatorsDimension(int n)
{
  if (family == XW_TEX_FONT_CMTT)
    textDimension(XW_TEX_FONT_CMTT,n);
  else
    textDimension(XW_TEX_FONT_CMR,n);
}

void XWTeXText::operatorDimension(int c)
{
  if (family == XW_TEX_FONT_CMTT)
    charDimension(XW_TEX_FONT_CMTT,c);
  else
    charDimension(XW_TEX_FONT_CMR, c);
}

void XWTeXText::operatorDimension(int och, int lsch)
{
  family = XW_TEX_FONT_CMR;
  int ch = och;
  if (fontSize > XW_TEX_FONT_NORMAL)
  {
    family = XW_TEX_FONT_CMEX;
    ch = lsch;
  }

  charDimension(family, ch);
}

void XWTeXText::operatorDimensionTB(int c1, int c2)
{
  charsTwoTB(XW_TEX_FONT_CMR,c1,XW_TEX_FONT_CMR,c2);
}

void XWTeXText::rightDimension()
{
  if (prev)
    fontSize = (int)(prev->height + prev->depth);

  if (text.isEmpty())
  {
    head->fontSize = fontSize;
    head->getDimension();
    width = head->width;
  }
  else
  {
    if (text[0] == QChar(')'))
      operatorDimension(0x29,0x01);
    else if (text[0] == QChar(']'))
      operatorDimension(0x5D,0x03);
    else if (text[0] == QChar('>'))
      symbolDimension(0x69,0x0B);
    else if (text[0] == QChar('/'))
      letterDimension(0x2F);
    else if (text[0] == QChar('|'))
      symbolDimension(0x6A,0x0C);
  }  
}

void XWTeXText::scanArray(const QString & str, int & len, int & pos)
{
  XWTeXText * obj = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str, len, pos);     
    else if (str[pos].isSpace())
      pos++;
    else
    {
      if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = XWTeXBox::scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        if (id == LAend)
        {
          key = XWTeXBox::scanEnviromentName(str,len,pos);
          return ;
        }

        pos = p;
      }

      obj = new XWTeXText(XW_TEX_ROW,box,this);
      append(obj);
      obj->scanRow(str,len,pos);
    }    
  }
}

void XWTeXText::scanASE(const QString & str, int & len, int & pos)
{
  if (pos >= len)
    return ;

  text.resize(1);
  if (str[pos] == QChar('-'))
  {
    family = XW_TEX_FONT_CMSY;
    text[0] = 0x00;
  }
  else if (str[pos] == QChar('*'))
  {
    family = XW_TEX_FONT_CMSY;
    text[0] = 0x03;
  }
  else
  {
    family = XW_TEX_FONT_CMR;
    text[0] = str[pos].unicode();
  }  
  pos++;
}

void XWTeXText::scanColumn(const QString & str, int & len, int & pos)
{
  XWTeXText * obj = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('&'))
    {
      pos++;
      return ;
    }      
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str, len, pos);     
    else if (str[pos].isSpace())
      pos++;
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
        obj = new XWTeXText(id,box,this);
      }
      else if (str[pos] == QChar('\\'))
      {
        if (str[pos + 1] == QChar('\\'))
          return ;

        int p = pos;
        QString key = XWTeXBox::scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        if (id == LAend)
        {
          pos = p;
          return ;
        }
        
        if (id == LAbegin)
        {
          key = XWTeXBox::scanEnviromentName(str,len,pos);
          id = lookupLaTeXID(key);
        }
        
        obj = new XWTeXText(id,box,this);
      }
      else if (str[pos] == QChar('.') || str[pos].isDigit())
        obj = new XWTeXText(XW_TEX_DIGIT,box,this);
      else if (str[pos] == QChar('*') ||
               str[pos] == QChar('-') || 
               str[pos] == QChar('=') ||               
               str[pos] == QChar('+') || 
               str[pos] == QChar(';') ||
               str[pos] == QChar('!') ||
               str[pos] == QChar(':') ||
               str[pos] == QChar('?'))
      {
        obj = new XWTeXText(XW_TEX_ASE,box,this);
      }      
      else
        obj = new XWTeXText(XW_TEX_LETTER,box,this);

      append(obj);
      obj->scan(str,len,pos);
    }
  }
}

void XWTeXText::scanDigit(const QString & str, int & len, int & pos)
{
  while (pos < len)
  {
    if (!str[pos].isDigit() && str[pos] != QChar('.'))
      break;

    text.append(str[pos]);
    pos++;
  }
}

void XWTeXText::scanEnvironment(const QString & str, int & len, int & pos)
{
  XWTeXText * obj = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str, len, pos);     
    else if (str[pos].isSpace())
      pos++;
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
        obj = new XWTeXText(id,box,this);
      }
      else if (str[pos] == QChar('\\'))
      {
        QString key = XWTeXBox::scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        if (id == LAend)
        {
          key = XWTeXBox::scanEnviromentName(str,len,pos);
          return ;
        }
        
        if (id == LAbegin)
        {
          key = XWTeXBox::scanEnviromentName(str,len,pos);
          id = lookupLaTeXID(key);
        }

        obj = new XWTeXText(id,box,this);
      }
      else if (str[pos] == QChar('.') || str[pos].isDigit())
        obj = new XWTeXText(XW_TEX_DIGIT,box,this);
      else if (str[pos] == QChar('*') ||
               str[pos] == QChar('-') || 
               str[pos] == QChar('=') ||               
               str[pos] == QChar('+') || 
               str[pos] == QChar(';') ||
               str[pos] == QChar('!') ||
               str[pos] == QChar(':') ||
               str[pos] == QChar('?'))
      {
        obj = new XWTeXText(XW_TEX_ASE,box,this);
      }      
      else       
        obj = new XWTeXText(XW_TEX_LETTER,box,this);
      append(obj);
      obj->scan(str,len,pos);
    }    
  }
}

void XWTeXText::scanFormular(const QString & str, int & len, int & pos)
{
  XWTeXText * obj = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str, len, pos);     
    else if (str[pos].isSpace())
      pos++;
    else
    {
      if (str[pos] == QChar('$') )
      {
        pos++;
        if (str[pos+1] == QChar('$') )
          pos++;
        return;
      }
      else if (str[pos] == QChar('\\'))
      {
        QString key = XWTeXBox::scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        if (id == LAbegin)
        {
          key = XWTeXBox::scanEnviromentName(str,len,pos);
          id = lookupLaTeXID(key);
        }

        obj = new XWTeXText(id,box,this);
      }
      else if (str[pos] == QChar('.') || str[pos].isDigit())
        obj = new XWTeXText(XW_TEX_DIGIT,box,this);
      else if (str[pos] == QChar('*') ||
               str[pos] == QChar('-') || 
               str[pos] == QChar('=') ||               
               str[pos] == QChar('+') || 
               str[pos] == QChar(';') ||
               str[pos] == QChar('!') ||
               str[pos] == QChar(':') ||
               str[pos] == QChar('?'))
      {
        obj = new XWTeXText(XW_TEX_ASE,box,this);
      }      
      else       
        obj = new XWTeXText(XW_TEX_LETTER,box,this);
      append(obj);
      obj->scan(str,len,pos);
    }    
  }
}

void XWTeXText::scanOption(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos >= len || str[pos] != QChar('['))
    return ;

  int b = 0;
  pos++;
  XWTeXText * obj = 0;
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str, len, pos); 
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
        obj = new XWTeXText(id,box,this);
      }
      else if (str[pos] == QChar('\\'))
      {
        QString key = XWTeXBox::scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        if (id == LAbegin)
        {
          key = XWTeXBox::scanEnviromentName(str,len,pos);
          id = lookupLaTeXID(key);
        }
        obj = new XWTeXText(id,box,this);
      }
      else if (str[pos] == QChar('.') || str[pos].isDigit())
        obj = new XWTeXText(XW_TEX_DIGIT,box,this);
      else if (str[pos] == QChar('*') ||
               str[pos] == QChar('-') || 
               str[pos] == QChar('=') ||
               str[pos] == QChar('+') || 
               str[pos] == QChar(';') ||
               str[pos] == QChar('!') ||
               str[pos] == QChar(':') ||
               str[pos] == QChar('?'))
      {
        obj = new XWTeXText(XW_TEX_ASE,box,this);
      }  
      else
      {
        if (str[pos] == QChar(']'))
        {
          b--;
          if (b < 0)
          {
            pos++;
            return ;
          }
        }
        else if (str[pos] == QChar('['))
          b++;

        obj = new XWTeXText(XW_TEX_LETTER,box,this);
      }

      append(obj);
      obj->scan(str,len,pos);
    }
  }
}

void XWTeXText::scanParam(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos >= len || str[pos] != QChar('{'))
    return ;

  int b = 0;
  pos++;
  XWTeXText * obj = 0;
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str, len, pos); 
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
        obj = new XWTeXText(id,box,this);
      }
      else if (str[pos] == QChar('\\'))
      {
        QString key = XWTeXBox::scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        if (id == LAbegin)
        {
          key = XWTeXBox::scanEnviromentName(str,len,pos);
          id = lookupLaTeXID(key);
        }
        obj = new XWTeXText(id,box,this);
      }
      else if (str[pos] == QChar('.') || str[pos].isDigit())
        obj = new XWTeXText(XW_TEX_DIGIT,box,this);
      else if (str[pos] == QChar('*') ||
               str[pos] == QChar('-') || 
               str[pos] == QChar('=') ||
               str[pos] == QChar('+') || 
               str[pos] == QChar(';') ||
               str[pos] == QChar('!') ||
               str[pos] == QChar(':') ||
               str[pos] == QChar('?'))
      {
        obj = new XWTeXText(XW_TEX_ASE,box,this);
      }      
      else
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

        obj = new XWTeXText(XW_TEX_LETTER,box,this);
      }

      append(obj);
      obj->scan(str,len,pos);
    }      
  }
}

void XWTeXText::scanRow(const QString & str, int & len, int & pos)
{
  XWTeXText * obj = 0;
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str, len, pos);
    else
    {
      if (str[pos] == QChar('\\'))
      {
        if (str[pos + 1] == QChar('\\'))
        {
          pos += 2;
          return ;
        }

        int p = pos;
        QString key = XWTeXBox::scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        pos = p;
        if (id == LAend)
          return ;
      }
      
      obj = new XWTeXText(XW_TEX_COL,box,this);
      append(obj);
      obj->scanColumn(str,len,pos);
    }    
  }
}

void XWTeXText::scanScript(const QString & str, int & len, int & pos)
{
  XWTeXText * obj = 0;
  if (str[pos] == QChar('\\'))
  {
    QString key = XWTeXBox::scanControlSequence(str,len,pos);
    int id = lookupLaTeXID(key);
    obj = new XWTeXText(id,box,this);
  }
  else if (str[pos] == QChar('{'))
    obj = new XWTeXText(XW_TEX_PARAM,box,this);
  else if (str[pos] == QChar('.') || str[pos].isDigit())
    obj = new XWTeXText(XW_TEX_DIGIT,box,this);
  else if (str[pos] == QChar('*') ||
               str[pos] == QChar('-') || 
               str[pos] == QChar('=') ||
               str[pos] == QChar('+') || 
               str[pos] == QChar(';') ||
               str[pos] == QChar('!') ||
               str[pos] == QChar(':') ||
               str[pos] == QChar('?'))
  {
    obj = new XWTeXText(XW_TEX_ASE,box,this);
  }      
  else
    obj = new XWTeXText(XW_TEX_LETTER,box,this);
  append(obj);
  obj->scan(str,len,pos);
}

void XWTeXText::scanText(const QString & str, int & len, int & pos)
{
  bool opt = false;
  bool param = false;
  if (parent())
  {
    XWTeXText * obj = (XWTeXText*)(parent());
    opt = (obj->keyWord == XW_TEX_OPT);
    param = (obj->keyWord == XW_TEX_PARAM);
  }

  int b = 0;
  while (pos < len)
  {
    if ((str[pos] == QChar('$')) ||
        (str[pos] == QChar('\\')) ||
        (str[pos] == QChar('&')) ||
        (str[pos] == QChar('_')) ||
        (str[pos] == QChar('^')) ||
        (str[pos] == QChar('-')) ||
        (str[pos] == QChar('*')) ||
        (str[pos] == QChar('=')) ||
        (str[pos] == QChar('+')) ||
        (str[pos] == QChar(':')) ||
        (str[pos] == QChar(';')) ||
        (str[pos] == QChar('?')) ||
        (str[pos] == QChar('!')) ||
        (str[pos] == QChar('.')) ||
         str[pos].isDigit())
    {
      return ;
    }
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str, len, pos);
    else if (str[pos] == QChar('\n'))
    {
      text.append(' ');
      pos++;
    }      
    else
    {
      if (opt)
      {
        if (str[pos] == QChar('['))
          b++;
        else if (str[pos] == QChar(']'))
        {
          b--;
          if (b < 0)
            return ;
        }
      }
      else if (param)
      {
        if (str[pos] == QChar('{'))
          b++;
        else if (str[pos] == QChar('}'))
        {
          b--;
          if (b < 0)
            return ;
        }
      }
      
      text.append(str[pos]);
      pos++;
    }          
  }
}

void XWTeXText::scriptDimension()
{
  XWTeXText * obj = 0;
  switch (keyWord)
  {
    case LAsup:
    case LAinf:
    case LAlim:
    case LAliminf:
    case LAlimsup:
    case LAsum:
    case LAbigcup:
    case LAbigvee:
    case LAprod:
    case LAbigcap:
    case LAbigwedge:
    case LAcoprod:
    case LAbigsqcup:
    case LAbiguplus:
      obj = head;
      while (obj)
      {
        obj->getDimension();
        width = qMax(width,obj->width);
        if (obj->keyWord == XW_TEX_SUP)
        {
          height += obj->height;
          height += obj->depth;
        }            
        else
        {
          depth += obj->height;
          depth += obj->depth;
        }
        
        obj = obj->next;
      }
      break;

    case LAoverrightarrow:
    case LAunderrightarrow:
    case LAoverleftarrow:
    case LAunderleftarrow:
    case LAoverleftrightarrow:
    case LAunderleftrightarrow:
    case LAoverbrace:
    case LAunderbrace:
    case LAoverline:
    case LAunderline:
      obj = head->next;
      while (obj)
      {
        obj->getDimension();
        width = qMax(width,obj->width);
        if (obj->keyWord == XW_TEX_SUP)
        {
          height += obj->height;
          height += obj->depth;
        }            
        else
        {
          depth += obj->height;
          depth += obj->depth;
        }
         
        obj = obj->next;
      }
      break;

    case LAleft:
    case LAright:
    case LAbig:
    case LABig:
    case LAbigg:
    case LABigg:
      if (!text.isEmpty() && head)
      {
        obj = head;
        double w = 0;
        while (obj)
        {
          obj->getDimension();
          w = qMax(w,obj->width);
          if (obj->keyWord == XW_TEX_SUP)
            height += obj->height;
          else
            depth += obj->depth;
          
          obj = obj->next;
        }

        width += w;
        width += 2;
      }
      break;

    case LAfrac:
    case LAstackrel:
    case LAsqrt:
      obj = head->next->next;
      if (obj)
      {
        double w = 0;
        while (obj)
        {
          obj->getDimension();
          w = qMax(w,obj->width);
          if (obj->keyWord == XW_TEX_SUP)
            height += obj->height;
          else
            depth += obj->depth;
          
          obj = obj->next;
        }

        width += w;
        width += 2;
      }
      break;

    case LAmathrm:
    case LAmathbf:
    case LAmathsf:
    case LAmathtt:
    case LAmathit:
    case LAmathcal:
    case LAmathnormal:
    case LAhat:
    case LAcheck:
    case LAtilde:
    case LAgrave:
    case LAdot:
    case LAddot:
    case LAbar:
    case LAvec:
    case LAwidehat:
    case LAacute:
    case LAbreve:
    case LAwidetilde:
      obj = head->next;
      if (obj)
      {
        double w = 0;
        while (obj)
        {
          obj->getDimension();
          w = qMax(w,obj->width);
          if (obj->keyWord == XW_TEX_SUP)
            height += obj->height;
          else
            depth += obj->depth;
          
          obj = obj->next;
        }
        width += w;
        width += 2;
      }
      break;

    default:
      if (head)
      {
        obj = head;
        double w = 0;
        while (obj)
        {
          obj->getDimension();
          w = qMax(w,obj->width);
          if (obj->keyWord == XW_TEX_SUP)
            height += obj->height;
          else
            depth += obj->depth;
          
          obj = obj->next;
        }

        width += w;
        width += 2;
      }
      break;
  }  
}

void XWTeXText::setMaxColumnWidthAndAlign(QList<double> & w, QList<int> & a)
{
  XWTeXText * obj = head;
  int i = 0;
  while (obj)
  {
    obj->width2 = w[i];
    obj->align = a[i];
    i++;
    obj = obj->next;
  }
}

void XWTeXText::symbolDimension(int c)
{
  charDimension(XW_TEX_FONT_CMSY,c);
}

void XWTeXText::symbolDimension(int sch, int lsch)
{
  family = XW_TEX_FONT_CMSY;
  int ch = sch;
  if (fontSize > XW_TEX_FONT_NORMAL)
  {
    family = XW_TEX_FONT_CMEX;
    ch = lsch;
  }

  charDimension(family,ch);
}

void XWTeXText::symbolDimensionLR(int c1, int c2)
{
  charsTwoLR(XW_TEX_FONT_CMSY,c1,c2);
}

void XWTeXText::symbolDimensionLR(int c1, int c2, int c3)
{
  charsThreeLR(XW_TEX_FONT_CMSY,c1,c2,c3);
}

void XWTeXText::symbolDimensionTB(int c1, int c2)
{
  charsTwoTB(XW_TEX_FONT_CMSY,c1,XW_TEX_FONT_CMR,c2);
}

void XWTeXText::textDimension()
{
  QFont font = QApplication::font();
  font.setBold(bold);
  font.setItalic(italic);
  font.setPointSize(fontSize);
  if (slant)
    font.setStyle(QFont::StyleOblique);
  if (sc)
    font.setCapitalization(QFont::SmallCaps);
  if (sf)
    font.setStyleHint(QFont::SansSerif);
  QFontMetricsF metrics(font);
  width = metrics.width(text);
  height = metrics.height();
}

void XWTeXText::textDimension(int fam)
{
  family = fam;
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  for (int i = 0; i < text.length(); i++)
  {
    double w = tfm.getWidth(text[i].unicode());
    double h = tfm.getHeight(text[i].unicode());
    double d = tfm.getDepth(text[i].unicode());

    width += w;
    height = qMax(height, h);
    depth = qMax(depth, d);
  }

  width2 = width;
  height2 = height;
  depth2 = depth;

  scriptDimension();
}

void XWTeXText::textDimension(int fam, int n)
{
  family = fam;
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  int i = 0;
  for (; i < n; i++)
  {
    double w = tfm.getWidth(text[i].unicode());
    double h = tfm.getHeight(text[i].unicode());
    double d = tfm.getDepth(text[i].unicode());
    width += w;
    height = qMax(height,h);
    depth = qMax(depth, d);
  }

  width += 2;

  for (;i < text.length(); i++)
  {
    double w = tfm.getWidth(text[i].unicode());
    double h = tfm.getHeight(text[i].unicode());
    double d = tfm.getDepth(text[i].unicode());
    width += w;
    height = qMax(height,h);
    depth = qMax(depth, d);
  }

  width2 = width;
  height2 = height;
  depth2 = depth;

  scriptDimension();
}

void XWTeXText::unknownDimension()
{
  textDimension(XW_TEX_FONT_CMTT);
}

void XWTeXText::updateCursor(double xA, double yA)
{
  if (box->getCurrent() == this)
  {
    if (keyWord == XW_TEX_TEXT)
    {
      QFont font = QApplication::font();
      font.setBold(bold);
      font.setItalic(italic);
      font.setPointSize(fontSize);
      if (slant)
        font.setStyle(QFont::StyleOblique);
      if (sc)
        font.setCapitalization(QFont::SmallCaps);
      if (sf)
        font.setStyleHint(QFont::SansSerif);
      QFontMetricsF metrics(font);

      double tmpx = x;
      for (int i = 0; i < textPos; i++)
        tmpx += metrics.width(text[i]);

      box->updateCursor(tmpx - 0.5,y + height, tmpx + 0.5, y, textPos);
    }
    else if (keyWord == XW_TEX_LETTER || keyWord == XW_TEX_DIGIT)
    {
      QString fontname = getFontName();
      QByteArray tfmname = fontname.toAscii();
      XWTFMFile tfm(tfmname.constData());
      double w,h,d;
      double tmpx = x;
      for (int i = 0; i < textPos; i++)
      {
        w = tfm.getWidth(text[i].unicode());
        h = tfm.getHeight(text[i].unicode());
        d = tfm.getDepth(text[i].unicode());
        tmpx += w;
      }

      box->updateCursor(tmpx - 0.5,y + h, tmpx + 0.5, y - d, textPos);
    }
    else if (textPos == XW_TEX_POS_BEFORE)
    {
      box->updateCursor(xA - width - 0.5, yA + height, xA - width + 0.5, yA - depth, XW_TEX_POS_BEFORE);
    }      
    else
    {
      box->updateCursor(xA - 0.5, yA + height, xA + 0.5, yA - depth, XW_TEX_POS_AFTER);
    }
  }
}

void XWTeXText::vdotsDimension()
{
  family = XW_TEX_FONT_CMR;
  text.resize(3);
  text[0] = 0x2E;
  text[1] = 0x2E;
  text[2] = 0x2E;
  QString fontname = getFontName();
  QByteArray tfmname = fontname.toAscii();
  XWTFMFile tfm(tfmname.constData());
  width = tfm.getWidth(text[0].unicode());
  height = 3 * tfm.getHeight(text[0].unicode()) + 2 * tfm.getDepth(text[0].unicode());
  depth = tfm.getDepth(text[0].unicode());  
  width2 = width;
  height2 = height;
  depth2 = depth;
  scriptDimension();
}

void XWTeXText::vertDimension(int c)
{
  bold = true;
  charDimension(XW_TEX_FONT_CMSY,c);
}

XWTeXDeleteChar::XWTeXDeleteChar(XWTeXText * textA, 
                                   int posA,
                                   QUndoCommand * parent)
:QUndoCommand(parent),
 text(textA),
 pos(posA)
{
  setText("Insert char");
}

void XWTeXDeleteChar::redo()
{
  c = text->deleteChar(pos);
  text->box->clear();
}

void XWTeXDeleteChar::undo()
{
  text->insert(c);
  text->box->clear();
}

XWTeXInsert::XWTeXInsert(XWTeXText * newobjA,
                   XWTeXText * prevA,
                   QUndoCommand * parent)
:QUndoCommand(parent),
 newObj(newobjA),
 prev(prevA)
{
  setText("Insert Object");
}

void XWTeXInsert::redo()
{
  if (prev)
    prev->insertAtAfter(newObj);
  else if (newObj->parent())
  {
    XWTeXText * pobj = (XWTeXText*)(newObj->parent());
    newObj->next = pobj->head;
    if (pobj->head)
      pobj->head->prev = newObj;
    pobj->head = newObj;
    newObj->prev = 0;
  }
  else
    newObj->box->insert(newObj);

  newObj->box->clear();
}

void XWTeXInsert::undo()
{
  newObj->remove();
  newObj->box->clear();
}

XWTeXRemove::XWTeXRemove(XWTeXText * objA, QUndoCommand * parent)
:QUndoCommand(parent),
 obj(objA)
{
  setText("Remove Object");
}

void XWTeXRemove::redo()
{
  obj->box->remove(obj);
  obj->box->clear();
}

void XWTeXRemove::undo()
{
  obj->box->insert(obj);
  obj->box->clear();
}

XWTeXNew::XWTeXNew(XWTeXText * newobjA, QUndoCommand * parent)
:QUndoCommand(parent),
 newObj(newobjA)
{
  setText("New Object");
}

void XWTeXNew::redo()
{
  newObj->box->insert(newObj);
  newObj->box->clear();
}

void XWTeXNew::undo()
{
  newObj->box->remove(newObj);
  newObj->box->clear();
}

XWTeXAppend::XWTeXAppend(XWTeXText * newobjA,
                          XWTeXText * pobjA,
                            QUndoCommand * parent)
:QUndoCommand(parent),
 newObj(newobjA),
 pobj(pobjA)
{
  setText("Append Child");
}

void XWTeXAppend::redo()
{
  pobj->append(newObj);
  pobj->box->clear();
}

void XWTeXAppend::undo()
{
  newObj->remove();
  newObj->box->clear();
}

XWTeXRemoveString::XWTeXRemoveString(XWTeXText * objA,
                          int sposA,
                          int eposA,
                            QUndoCommand * parent)
:QUndoCommand(parent),
 obj(objA),
 spos(sposA),
 epos(eposA)
{
  setText("Remove String");
}

void XWTeXRemoveString::redo()
{
  text = obj->getSubstring(spos,epos - spos);
  obj->remove(spos,epos - spos);
  obj->box->clear();
}

void XWTeXRemoveString::undo()
{
  obj->insert(spos,text);
  obj->box->clear();
}

XWTeXInsertString::XWTeXInsertString(XWTeXText * objA,
                          int posA,
                          const QString & str,
                            QUndoCommand * parent)
:QUndoCommand(parent),
 obj(objA),
 pos(posA),
 text(str)
{
  setText("Insert String");
}

void XWTeXInsertString::redo()
{
  obj->insert(pos,text);
  obj->box->clear();
}

void XWTeXInsertString::undo()
{
  obj->remove(pos,text.length());
  obj->box->clear();
}
