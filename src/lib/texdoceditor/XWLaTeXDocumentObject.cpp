/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include "LaTeXKeyWord.h"
#include "BeamerKeyWord.h"
#include "XWLaTeXPool.h"
#include "XWBeamerPool.h"
#include "XWTeXDocumentPage.h"
#include "XWLaTeXDocument.h"
#include "XWBeamerObject.h"
#include "XWLaTeXDocumentObject.h"

XWTeXDocumentObject * createLaTeXObject(int t, XWTeXDocument * docA, QObject * parent)
{
  XWTeXDocumentObject * obj = 0;
  switch (t)
  {
    case LAdocumentclass:
      obj = new XWLTXDocumentClass(docA, parent);
      break;

    case LAusepackage:
      obj = new XWLTXDocumentUsePackage(docA, parent);
      break;

    case LAmakeindex:
      obj = new XWTeXOptionCommand(LAmakeindex,docA,true,"\\makeindex","key-values:",parent);
      break;

    case LAprintindex:
      obj = new XWTeXOptionCommand(LAmakeindex,docA,true,"\\printindex","name:",parent);
      break;

    case LAnewcommand:
     obj = new XWLTXDocumentNewCommand(docA, parent);
     break;

    case LArenewcommand:
      obj = new XWLTXDocumentRenewCommand(docA, parent);
      break;

    case LAnewenvironment:
      obj = new XWLTXDocumentNewEnvironment(docA, parent);
      break;

    case LArenewenvironment:
      obj = new XWLTXDocumentRenewEnvironment(docA, parent);
      break;

    case LAnewtheorem:
      obj = new XWLTXDocumentNewTheorem(docA, parent);
      break;

    case LAnewfont:
      obj = new XWLTXDocumentNewFont(docA, parent);
      break;

    case LAnewcounter:
      obj = new XWLTXDocumentNewCounter(docA, parent);
      break;

    case LAnewlength:
      obj = new XWLTXDocumentNewLength(docA, parent);
      break;

    case LAincludeonly:
      obj = new XWLTXDocumentIncludeOnly(docA, parent);
      break;

    case LAdocument:
      obj = new XWLTXDocumentEnviroment(docA, parent);
      break;

    case LACJK:
      obj = new XWLTXDocumentCJK(docA, parent);
      break;

    case LACJKStar:
      obj = new XWLTXDocumentCJKStar(docA, parent);
      break;

    case LAinclude:
      obj = new XWLTXDocumentInclude(docA, parent);
      break;

    case LAinput:
      obj = new XWLTXDocumentInput(docA, parent);
      break;

    case LAtitle:
      obj = new XWLTXDocumentTitle(docA, parent);
      break;

    case LAauthor:
      obj = new XWLTXDocumentAuthor(docA, parent);
      break;

    case LAdate:
      obj = new XWLTXDocumentDate(docA, parent);
      break;

    case LAthanks:
      obj = new XWLTXDocumentThanks(docA, parent);
      break;

    case LAtitlepage:
      obj = new XWLTXDocumentTitlePage(docA, parent);
      break;

    case LAtableofcontents:
      obj = new XWLTXTableOfContents(docA, parent);
      break;

    case LAappendix:
      obj = new XWLTXAppendix(docA, parent);
      break;

    case LAabstract:
      obj = new XWLTXDocumentAbstract(docA, parent);
      break;

    case LApart:
      obj = new XWLTXDocumentPart(docA, parent);
      break;

    case LAchapter:
      obj = new XWLTXDocumentChapter(docA, parent);
      break;

    case LAsection:
      obj = new XWLTXDocumentSection(docA, parent);
      break;

    case LAsubsection:
      obj = new XWLTXDocumentSubsection(docA, parent);
      break;

    case LAsubsubsection:
      obj = new XWLTXDocumentSubsubsection(docA, parent);
      break;

    case LAparagraph:
      obj = new XWLTXDocumentParagraph(docA, parent);
      break;

    case LAsubparagraph:
      obj = new XWLTXDocumentSubparagraph(docA, parent);
      break;

    case LArealparagraph:
      obj = new XWLTXDocumentPar(docA, parent);
      break;

    case LAquote:
      obj = new XWLTXDocumentQuote(docA, parent);
      break;

    case LAquotation:
      obj = new XWLTXDocumentQuotation(docA, parent);
      break;

    case LAverse:
      obj = new XWLTXDocumentVerse(docA, parent);
      break;

    case LAitem:
      obj = new XWLTXDocumentItem(docA, parent);
      break;

    case LAdescription:
     obj = new XWLTXDocumentDescription(docA, parent);
     break;

    case LAenumerate:
      obj = new XWLTXDocumentEnumerate(docA, parent);
      break;

    case LAitemize:
      obj = new XWLTXDocumentItemize(docA, parent);
      break;

    case LAarray:
      obj = new XWLTXDocumentArray(docA, parent);
      break;

    case LAdisplaymath:
      obj = new XWLTXDocumentDisplayMath(docA, parent);
      break;

    case LAeqnarray:
      obj = new XWLTXDocumentEqnarray(docA, parent);
      break;

    case LAeqnarrayStar:
      obj = new XWLTXDocumentEqnarrayStar(docA, parent);
      break;

    case LAequation:
      obj = new XWLTXDocumentEquation(docA, parent);
      break;

    case LAfigure:
      obj = new XWLTXDocumentFigure(docA, parent);
      break;

    case LAfigureStar:
      obj = new XWLTXDocumentFigureStar(docA, parent);
      break;

    case LAcenter:
      obj = new XWLTXDocumentCenter(docA, parent);
      break;

    case LAflushleft:
      obj = new XWLTXDocumentFlushLeft(docA, parent);
      break;

    case LAflushright:
      obj = new XWLTXDocumentFlushRight(docA, parent);
      break;

    case LAlist:
      obj = new XWLTXDocumentList(docA, parent);
      break;

    case LAmath:
      obj = new XWLTXDocumentMath(docA, parent);
      break;

    case LAminipage:
      obj = new XWLTXDocumentMiniPage(docA, parent);
      break;

    case LApicture:
      obj = new XWLTXDocumentPicture(docA, parent);
      break;

    case LAtabbing:
      obj = new XWLTXDocumentTabbing(docA, parent);
      break;

    case LAtable:
      obj = new XWLTXDocumentTable(docA, parent);
      break;

    case LAtabular:
      obj = new XWLTXDocumentTabular(docA, parent);
      break;

    case LAtabularStar:
      obj = new XWLTXDocumentTabularStar(docA, parent);
      break;

    case LAbibitem:
      obj = new XWLTXDocumentBibitem(docA, parent);
      break;

    case LAthebibliography:
      obj = new XWLTXDocumentThebibliography(docA, parent);
      break;

    case LAtheorem:
      obj = new XWLTXDocumentTheorem(docA, parent);
      break;

    case LAproof:
      obj = new XWLTXDocumentProof(docA, parent);
      break;

    case LAassumption:
      obj = new XWLTXDocumentAssumption(docA, parent);
      break;

    case LAdefinition:
      obj = new XWLTXDocumentDefinition(docA, parent);
      break;

    case LAproposition:
      obj = new XWLTXDocumentProposition(docA, parent);
      break;

    case LAlemma:
      obj = new XWLTXDocumentLemma(docA, parent);
      break;

    case LAaxiom:
      obj = new XWLTXDocumentAxiom(docA, parent);
      break;

    case LAcorollary:
      obj = new XWLTXDocumentCorollary(docA, parent);
      break;

    case LAconjecture:
      obj = new XWLTXDocumentConjecture(docA, parent);
      break;

    case LAexercise:
      obj = new XWLTXDocumentExercise(docA, parent);
      break;

    case LAexample:
      obj = new XWLTXDocumentExample(docA, parent);
      break;

    case LAremark:
      obj = new XWLTXDocumentRemark(docA, parent);
      break;

    case LAproblem:
      obj = new XWLTXDocumentProblem(docA, parent);
      break;

    case LAverbatim:
      obj = new XWLTXDocumentVerbatim(docA, parent);
      break;

    case LAverb:
      obj = new XWLTXDocumentVerb(docA, parent);
      break;

    case LAfootnote:
      obj = new XWLTXDocumentFootnote(docA, parent);
      break;

    case LAmarginpar:
      obj = new XWLTXDocumentMarginpar(docA, parent);
      break;

    case LAcaption:
      obj = new XWLTXDocumentCaption(docA, parent);
      break;

    case LAlabel:
      obj = new XWLTXDocumentLabel(docA, parent);
      break;

    case LApageref:
      obj = new XWLTXDocumentPageref(docA, parent);
      break;

    case LAref:
      obj = new XWLTXDocumentRef(docA, parent);
      break;

    case LAindex:
      obj = new XWLTXDocumentIndex(docA, parent);
      break;

    case LAglossary:
      obj = new XWLTXDocumentGlossary(docA, parent);
      break;

    case LAcite:
      obj = new XWLTXDocumentCite(docA, parent);
      break;

    case LAnocite:
      obj = new XWLTXDocumentNocite(docA, parent);
      break;

    case LAbibliographystyle:
      obj = new XWLTXDocumentBibliographystyle(docA, parent);
      break;

    case LAbibliography:
      obj = new XWLTXDocumentBibliography(docA, parent);
      break;

    case LAaddress:
      obj = new XWLTXDocumentAddress(docA, parent);
      break;

    case LAlocation:
      obj = new XWLTXDocumentLocation(docA, parent);
      break;

    case LAtelephone:
      obj = new XWLTXDocumentTelephone(docA, parent);
      break;

    case LAsignature:
      obj = new XWLTXDocumentSignature(docA, parent);
      break;

    case LAopening:
      obj = new XWLTXDocumentOpening(docA, parent);
      break;

    case LAname:
      obj = new XWLTXDocumentName(docA, parent);
      break;

    case LAmakelabels:
      obj = new XWLTXDocumentMakelabels(docA, parent);
      break;

    case LAencl:
      obj = new XWLTXDocumentEncl(docA, parent);
      break;

    case LAclosing:
      obj = new XWLTXDocumentClosing(docA, parent);
      break;

    case LAcc:
      obj = new XWLTXDocumentCc(docA, parent);
      break;

    case LAletter:
      obj =  new XWLTXDocumentLetter(docA, parent);
      break;

    case LAincludegraphics:
      obj =  new XWLTXDocumentIncludegraphics(docA, parent);
      break;

    default:
      break;
  }

  return obj;
}

XWLTXRow::XWLTXRow(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentBlock(XW_LTX_ROW,docA,parent)
{
  specialFont = false;
  newRow = true;
}

QString XWLTXRow::getMimeData()
{
  text = text.trimmed();
  QString str = text;
  if (next && text.length() > 0)
  {
    if (text[0] != QChar('\\'))
    {
      if (text.length() > 2)
      {
        if (text[text.length() - 1] != QChar('\\'))
          str += "\\";

        if (text[text.length() - 2] != QChar('\\'))
          str += "\\";
      }
      else if (text.length() == 2)
      {
        if (text[1] == QChar('\\'))
          str += "\\";
        else
          str += "\\\\";
      }
      else
        str += "\\\\";
    }
    else
    {
      int pos = 0;
      int len = text.length();
      QString key = scanControlSequence(text,len,pos);
      int id = lookupLaTeXID(key);
      switch (id)
      {
        case LAhline:
        case LAvline:
        case LAmorecmidrules:
        case LAtoprule:
        case LAmidrule:
        case LAbottomrule:
        case LAcmidrule:
        case LArowcolor:
        case LAhhline:
          break;

        default:
          {
            if (text.length() > 2)
            {
              if (text[text.length() - 1] != QChar('\\'))
                str += "\\";

              if (text[text.length() - 2] != QChar('\\'))
                str += "\\";
            }
            else if (text.length() == 2)
            {
              if (text[1] == QChar('\\'))
                str += "\\";
              else
                str += "\\\\";
            }
            else
              str += "\\\\";
          }
          break;
      }
    }
  }

  return str;
}

void XWLTXRow::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  int b = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('\\'))
    {
      if ((pos + 1) >= len || str[pos + 1] == QChar('\\'))
      {
        pos += 2;
        goto endline;
      }

      int i = pos;
      QString key = scanControlSequence(str,len,pos);
      int id = lookupLaTeXID(key);
      switch (id)
      {
        case LAhline:
        case LAvline:
        case LAmorecmidrules:
          text = str.mid(i,pos - i);
          goto endline;
          break;

        case LAtoprule:
        case LAmidrule:
        case LAbottomrule:
          skipOption(str,len,pos);
          text = str.mid(i,pos - i);
          goto endline;
          break;

        case LAcmidrule:
          skipCoord(str,len,pos);
          skipGroup(str,len,pos);
          text = str.mid(i,pos - i);
          goto endline;
          break;

        case LArowcolor:
          skipOption(str,len,pos);
          skipGroup(str,len,pos);
          skipOption(str,len,pos);
          skipOption(str,len,pos);
          text = str.mid(i,pos - i);
          goto endline;
          break;

        case LAhhline:
          skipGroup(str,len,pos);
          text = str.mid(i,pos - i);
          goto endline;
          break;

        case LAmulticolumn:
          skipGroup(str,len,pos);
          skipGroup(str,len,pos);
          skipGroup(str,len,pos);
          text += str.mid(i,pos - i);
          break;

        case LAcline:
          skipGroup(str,len,pos);
          text += str.mid(i,pos - i);
          break;

        case LAcellcolor:
          skipOption(str,len,pos);
          skipGroup(str,len,pos);
          skipOption(str,len,pos);
          skipOption(str,len,pos);
          text += str.mid(i,pos - i);
          break;

        case LAend:
          {
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
            if (id == pobj->type())
            {
              pos = i;
              goto endline;
            }

            text += str.mid(i,pos - i);
          }
          break;

        default:
          text += str.mid(i,pos - i);
          break;
      }
    }
    else if (str[pos] == QChar('}'))
    {
      b--;
      if (b < 0)
        goto endline;

      text.append(str[pos]);
      pos++;
    }
    else if (str[pos] == QChar('{'))
    {
      b++;
      text.append(str[pos]);
      pos++;
    }
    else
    {
      text.append(str[pos]);
      pos++;
    }
  }

endline:
  while (pos < len && str[pos].isSpace())
    pos++;
  text = text.trimmed();
}

XWLTXCode::XWLTXCode(XWTeXDocument * docA, const QString & nameA,QObject * parent)
: XWTeXCode(XW_LTX_CODE,docA,nameA,parent)
{
}

void XWLTXCode::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
  XWTeXDocumentBlock * code = (XWTeXDocumentBlock*)last;

  int b = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('\\'))
    {
      int p = pos;
      QString key = scanControlSequence(str,len,pos);
      int id = lookupLaTeXID(key);
      switch (id)
      {
        case LAend:
          key = scanEnviromentName(str,len,pos);
          id = lookupLaTeXID(key);
          if (id == pobj->type())
          {
            b--;
            if (b < 0)
              return ;
          }
          break;

        case LAbegin:
          key = scanEnviromentName(str,len,pos);
          id = lookupLaTeXID(key);
          if (id == pobj->type())
            b++;

        default:
          break;
      }

      key = str.mid(p,pos - p);
      code->text.append(key);
    }
    else
    {
      code->text.append(str[pos]);
      pos++;
    }
  }
  code->text = code->text.trimmed();
}

XWLTXStar::XWLTXStar(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentBlock(XW_LTX_STAR,docA,parent)
{}

void XWLTXStar::clear()
{
  text.clear();
}

bool XWLTXStar::isEmpty()
{
  return text.isEmpty();
}

void XWLTXStar::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos > len || str[pos] != QChar('*'))
    return ;

  text.append(str[pos++]);
}

void XWLTXStar::star()
{
  text = "*";
}

XWLTXOption::XWLTXOption(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentObject(XW_LTX_OPTION,docA,parent)
{
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"[",this));
  add(new XWTeXDocumentText(docA,this));
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"]",this));
}

void XWLTXOption::breakPage(const QFont & fontA,const QColor & c,
              double & curx,double & cury,
              bool & firstcolumn)
{
  dir = doc->getDirection();
  font = fontA;
  color = c;

  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
  switch (pobj->type())
  {
    case LApart:
    case LAchapter:
    case LAsection:
    case LAsubsection:
    case LAsubsubsection:
    case LAparagraph:
    case LAsubparagraph:
      {
        XWTeXDocumentObject * obj = head->next;
        while (obj != last)
        {
          if (obj == head->next)
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
            obj->breakPage(curx,cury,firstcolumn);
          else
            obj->breakPage(font, color, curx,cury,firstcolumn);
          obj = obj->next;
        }
      }
      break;

    default:
      XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
      break;
  }
}

void XWLTXOption::flushBottom(const QFont & fontA,const QColor & c,
                  double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  font = fontA;
  color = c;

  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
  switch (pobj->type())
  {
    case LApart:
    case LAchapter:
    case LAsection:
    case LAsubsection:
    case LAsubsubsection:
    case LAparagraph:
    case LAsubparagraph:
      {
        XWTeXDocumentObject * obj = head->next;
        while (obj != last)
        {
          if (obj == head->next)
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
      break;

    default:
      XWTeXDocumentObject::flushBottom(curx,cury,firstcolumn);
      break;
  }
}

QString XWLTXOption::getMimeData()
{
  QString str;
  XWTeXDocumentObject * obj = head->next;
  while (obj)
  {
    if (obj == last)
      break;

    QString tmp = obj->getMimeData();
    str += tmp;
    obj = obj->next;
  }

  if (str.isEmpty())
    return QString();

  return XWTeXDocumentObject::getMimeData();
}

bool XWLTXOption::hasSelected()
{
  return head->next->hasSelected() || last->prev->hasSelected();
}

double XWLTXOption::height()
{
  double h = head->next->height();
  XWTeXDocumentObject * obj = head->next->next;
  while (obj != last)
  {
    h += obj->height();
    obj = obj->next;
  }

  return h;
}

bool XWLTXOption::isAllSelected()
{
  if (isEmpty())
    return true;
  return head->next->isAllSelected() && last->prev->isAllSelected();
}

bool XWLTXOption::isEmpty()
{
  return head->next == last->prev && head->next->isEmpty();
}

void XWLTXOption::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos > len || str[pos] != QChar('['))
    return ;

  pos++;
  while (pos < len && str[pos].isSpace())
    pos++;

  del(head->next);

  int b = 0;
  XWTeXDocumentObject * obj = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('\\'))
    {
      QString key = scanControlSequence(str, len,pos);
      obj = doc->createObject(key, str, pos,this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
    else if (str[pos] == QChar('$'))
    {
      if (pos < (len - 1) && str[pos + 1] == QChar('$'))
        obj = new XWTeXDocumentDisplayFormular(doc, this);
      else
        obj = new XWTeXDocumentFormular(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
    else if (str[pos] == QChar('%'))
    {
      obj = new XWTeXDocumentComment(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
    else
    {
      XWTeXDocumentText * txt = new XWTeXDocumentText(doc, this);
      last->insertAtBefore(txt);
      while (pos < len)
      {
        if (str[pos] == QChar('\\') ||
            str[pos] == QChar('$') ||
            str[pos] == QChar('%'))
        {
          break;
        }

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

        txt->text.append(str[pos]);
        pos++;
      }
    }
  }
}

double XWLTXOption::width()
{
  double w = head->next->width();
  XWTeXDocumentObject * obj = head->next->next;
  while (obj != last)
  {
    w += obj->width();
    obj = obj->next;
  }

  return w;
}

void XWLTXOption::write(QTextStream & strm, int & linelen)
{
  QString str;
  XWTeXDocumentObject * obj = head->next;
  while (obj)
  {
    if (obj == last)
      break;

    QString tmp = obj->getMimeData();
    str += tmp;
    obj = obj->next;
  }

  if (str.isEmpty())
    return ;

  XWTeXDocumentObject::write(strm,linelen);
}

XWLTXParam::XWLTXParam(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentObject(XW_LTX_PARAM,docA,parent)
{
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"{",this));
  add(new XWTeXDocumentText(docA,this));
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"}",this));
}

void XWLTXParam::append(XWTeXDocumentObject * obj)
{
  obj->setParent(this);
  obj->next = last;
  obj->prev = last->prev;
  last->prev->next = obj;
  last->prev = obj;
}

void XWLTXParam::append(XWTeXDocumentObject * sobj,XWTeXDocumentObject * eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  sobj->prev = last->prev;
  last->prev->next = sobj;
  eobj->next = last;
  last->prev = eobj;
}

void XWLTXParam::breakPage(const QFont & fontA,const QColor & c,
              double & curx,double & cury,
              bool & firstcolumn)
{
  dir = doc->getDirection();
  font = fontA;
  color = c;

  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
  switch (pobj->type())
  {
    case LAtitle:
    case LAauthor:
    case LAdate:
    case LAthanks:
    case LAname:
    case LAsignature:
    case LAtelephone:
    case LAlocation:
    case LAaddress:
    case LApart:
    case LAchapter:
    case LAsection:
    case LAsubsection:
    case LAsubsubsection:
    case LAparagraph:
    case LAsubparagraph:
    case BMsubtitle:
    case BMinstitute:
    case BMframetitle:
    case BMframesubtitle:
      {
        XWTeXDocumentObject * obj = head->next;
        while (obj != last)
        {
          if (obj == head->next)
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
            obj->breakPage(curx,cury,firstcolumn);
          else
            obj->breakPage(font, color, curx,cury,firstcolumn);
          obj = obj->next;
        }
      }
      break;

    default:
      XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
      break;
  }
}

void XWLTXParam::centered(const QFont & fontA,const QColor & c,
              double & curx,double & cury,
              bool & firstcolumn)
{
  dir = doc->getDirection();
  font = fontA;
  color = c;

  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
  switch (pobj->type())
  {
    case LAtitle:
    case LAauthor:
    case LAdate:
    case LAthanks:
    case LAname:
    case LAsignature:
    case LAtelephone:
    case LAlocation:
    case LAaddress:
    case LApart:
    case LAchapter:
    case LAsection:
    case LAsubsection:
    case LAsubsubsection:
    case LAparagraph:
    case LAsubparagraph:
    case BMsubtitle:
    case BMinstitute:
    case BMframetitle:
    case BMframesubtitle:
      {
        XWTeXDocumentObject * obj = head->next;
        while (obj != last)
        {
          if (obj == head->next)
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
            obj->centered(curx,cury,firstcolumn);
          else
            obj->centered(font, color, curx,cury,firstcolumn);
          obj = obj->next;
        }
      }
      break;

    default:
      XWTeXDocumentObject::centered(curx,cury,firstcolumn);
      break;
  }
}

void XWLTXParam::flushBottom(const QFont & fontA,const QColor & c,
                  double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  font = fontA;
  color = c;

  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
  switch (pobj->type())
  {
    case LAtitle:
    case LAauthor:
    case LAdate:
    case LAthanks:
    case LAname:
    case LAsignature:
    case LAtelephone:
    case LAlocation:
    case LAaddress:
    case LApart:
    case LAchapter:
    case LAsection:
    case LAsubsection:
    case LAsubsubsection:
    case LAparagraph:
    case LAsubparagraph:
    case BMsubtitle:
    case BMinstitute:
    case BMframetitle:
    case BMframesubtitle:
      {
        XWTeXDocumentObject * obj = head->next;
        while (obj != last)
        {
          if (obj == head->next)
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
      break;

    default:
      XWTeXDocumentObject::flushBottom(curx,cury,firstcolumn);
      break;
  }
}

double XWLTXParam::height()
{
  double h = head->next->height();
  XWTeXDocumentObject * obj = head->next->next;
  while (obj != last)
  {
    h += obj->height();
    obj = obj->next;
  }

  return h;
}

bool XWLTXParam::hasSelected()
{
  return head->next->hasSelected() || last->prev->hasSelected();
}

void XWLTXParam::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }
  head->next = sobj;
  sobj->prev = head;
  eobj->next = last;
  last->prev = eobj;
}

bool XWLTXParam::isAllSelected()
{
  if (isEmpty())
    return true;

  return head->next->isAllSelected() && last->prev->isAllSelected();
}

bool XWLTXParam::isEmpty()
{
  return head->next == last->prev && head->next->isEmpty();
}

void XWLTXParam::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next;
  *eobj = last->prev;
  head->next = last;
  last->prev = head;
}

void XWLTXParam::removeChildren(XWTeXDocumentObject**sobj, 
                                           XWTeXDocumentObject**eobj,
                                           XWTeXDocumentObject**obj)
{
  if (*sobj == *eobj)
  {
    *obj = (*sobj)->prev;
    (*sobj)->remove();
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
      nextobj = entext;
      XWTeXDocumentCursor * cor = doc->getCursor(eltext);
      int pos = cor->getHitPos();
      entext->text = eltext->text.mid(pos, -1);
      eltext->text.remove(pos,entext->text.length());
      *eobj = eltext;
    }

    prevobj->next = nextobj;
    nextobj->prev = prevobj;
  }
}

void XWLTXParam::removeChildrenFrom(XWTeXDocumentObject**sobj, 
                            XWTeXDocumentObject**eobj,
                            XWTeXDocumentObject**obj)
{
  *eobj = last->prev;
  if ((*sobj)->type() == TEX_DOC_B_TEXT && !(*sobj)->isAtStart() && !(*sobj)->isAtEnd())
  {
    XWTeXDocumentText * ltext = (XWTeXDocumentText*)(*sobj);
    XWTeXDocumentText * ntext = new XWTeXDocumentText(doc,this);
    XWTeXDocumentCursor * cor = doc->getCursor(ltext);
    int pos = cor->getHitPos();
    ntext->text = ltext->text.mid(pos, -1);
    ltext->text.remove(pos,ntext->text.length());
    ntext->prev = ltext;
    ltext->next = ntext;
    *sobj = ntext;
  }  

  *obj = (*sobj)->prev;
}

void XWLTXParam::removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next; 
  if ((*eobj)->type() == TEX_DOC_B_TEXT && !(*eobj)->isAtStart() && !(*eobj)->isAtEnd())
  {
    XWTeXDocumentText * ltext = (XWTeXDocumentText*)(*eobj);
    XWTeXDocumentText * ntext = new XWTeXDocumentText(doc,this);
    XWTeXDocumentCursor * cor = doc->getCursor(ltext);
    int pos = cor->getHitPos();
    ntext->text = ltext->text.mid(pos, -1);
    ltext->text.remove(pos,ntext->text.length());
    ntext->prev = ltext;
    ltext->next = ntext;
    *eobj = ltext;
  }
}

void XWLTXParam::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos > len || str[pos] != QChar('{'))
    return ;

  pos++;
  while (pos < len && str[pos].isSpace())
    pos++;

  del(head->next);
  int b = 0;
  XWTeXDocumentObject * obj = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('\\'))
    {
      QString key = scanControlSequence(str, len,pos);
      obj = doc->createObject(key, str, pos,this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
    else if (str[pos] == QChar('$'))
    {
      if (pos < (len - 1) && str[pos + 1] == QChar('$'))
        obj = new XWTeXDocumentDisplayFormular(doc, this);
      else
        obj = new XWTeXDocumentFormular(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
    else if (str[pos] == QChar('%'))
    {
      obj = new XWTeXDocumentComment(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
    else
    {
      XWTeXDocumentText * txt = new XWTeXDocumentText(doc, this);
      last->insertAtBefore(txt);
      while (pos < len)
      {
        if (str[pos] == QChar('\\') ||
            str[pos] == QChar('$') ||
            str[pos] == QChar('%'))
        {
          break;
        }

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

        txt->text.append(str[pos]);
        pos++;
      }
    }
  }
}

double XWLTXParam::width()
{
  double w = head->next->width();
  XWTeXDocumentObject * obj = head->next->next;
  while (obj != last)
  {
    w += obj->width();
    obj = obj->next;
  }

  return w;
}

XWLTXEnviroment::XWLTXEnviroment(int tA, XWTeXDocument * docA,
                     const QString & headA,
                     QObject * parent)
: XWTeXDocumentObject(tA,docA,parent)
{
  newRow = true;
  head = new XWTeXControlSequence(docA, true, headA, this);
  head->isOnlyRead = true;
  last = head;
}

void XWLTXEnviroment::append(XWTeXDocumentObject * obj)
{
  obj->setParent(this);
  last->insertAtBefore(obj);
}

void XWLTXEnviroment::append(XWTeXDocumentObject * sobj,XWTeXDocumentObject * eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  sobj->prev = last->prev;
  last->prev->next = sobj;
  last->prev = eobj;
  eobj->next = last;
}

void XWLTXEnviroment::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setFont();

  double oldindent = indent;
  indent += 2.0;

  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && doc->isInPreamble())
  {
    head->next->newRow = false;
    XWTeXDocumentObject::breakPage(font,color,curx,cury,firstcolumn);
  }
  else
  {
    XWTeXDocumentObject * obj = head->next->next;
    while (obj)
    {
      obj->newRow = true;
      if (obj == last)
        break;

      obj->firstIndent = firstIndent;
      obj->setIndent(indent);
      obj->breakPage(font,color,curx,cury,firstcolumn);
      obj = obj->next;
    }
  }

  indent = oldindent;
}

void XWLTXEnviroment::clear()
{
  XWTeXDocumentObject * obj = head->next;
  while (obj != last)
  {
    XWTeXDocumentObject * tmp = obj->next;
    delete obj;
    obj = tmp;
  }
}

bool XWLTXEnviroment::hasSelected()
{
  XWTeXDocumentObject * obj = head->next->next;
  while (obj)
  {
    if (obj == last)
      break;

    if (obj->hasSelected())
      return true;
    obj = obj->next;
  }
  return false;
}

void XWLTXEnviroment::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  head->next = sobj;
  sobj->prev = head;
  last->prev = eobj;
  eobj->next = last;
}

bool XWLTXEnviroment::isAllSelected()
{
  return head->next->next->isAllSelected() && last->prev->isAllSelected();
}

bool XWLTXEnviroment::isAtEnd()
{
  XWTeXDocumentObject * block = doc->getCurrentBlock();
  if (block == last && last->isAtEnd())
    return true;

  return last->prev->isAtEnd();
}

bool XWLTXEnviroment::isAtStart()
{
  XWTeXDocumentObject * block = doc->getCurrentBlock();
  if (block == head && head->isAtStart())
    return true;

  return head->isAtStart() || head->next->isAtStart();
}

bool XWLTXEnviroment::isEmpty()
{
  XWTeXDocumentObject * obj = head->next;
  while (obj != last)
  {
    if (!obj->isEmpty())
      return false;

    obj = obj->next;
  }

  return true;
}

void XWLTXEnviroment::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next;
  *eobj = last->prev;
  head->next = last;
  last->prev = head;
}

void XWLTXEnviroment::removeChildrenFrom(XWTeXDocumentObject**sobj, 
                                         XWTeXDocumentObject**eobj,
                                         XWTeXDocumentObject**obj)
{
  *eobj = last->prev;
  *obj = (*sobj)->prev;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWLTXEnviroment::removeChildrenTo(XWTeXDocumentObject**sobj, 
                                       XWTeXDocumentObject**eobj,
                                       XWTeXDocumentObject**obj)
{
  *sobj = head->next;
  *obj = (*sobj)->prev;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWLTXEnviroment::setCursor()
{
  last->prev->setCursor();
}

void XWLTXEnviroment::setCursorAtEnd()
{
  last->prev->setCursor();
}

void XWLTXEnviroment::setCursorAtStart()
{
  last->prev->setCursorAtStart();
}

XWLTXDocumentClass::XWLTXDocumentClass(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAdocumentclass,docA,true, "\\documentclass", parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("options:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA, tr("name:"), this));
}

XWLTXDocumentUsePackage::XWLTXDocumentUsePackage(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAusepackage,docA,true, "\\usepackage", parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("options:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA, tr("name:"), this));
}

XWLTXDocumentDefine::XWLTXDocumentDefine(int tA, XWTeXDocument * docA,const QString & nameA, QObject * parent)
:XWTeXDocumentCommand(tA,docA,true, nameA, parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("cmd:"), this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("nargs:"), this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("default:"), this));
  add(new XWLTXCode(docA, tr("defn:"), this));
}

QString XWLTXDocumentDefine::getMimeData()
{
  QString str = head->getMimeData();
  QString tmp = head->next->getMimeData();
  str += tmp;
  tmp = head->next->next->getMimeData();
  str += tmp;
  tmp = last->prev->getMimeData();
  str += tmp;
  tmp = last->getMimeData();
  str = QString("%1{%2}").arg(str).arg(tmp);
  return str;
}

bool XWLTXDocumentDefine::hasSelected()
{
  return last->hasSelected();
}

void XWLTXDocumentDefine::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str, len,pos);

  head->next->next->scan(str, len,pos);
  last->prev->scan(str, len,pos);
  XWLTXCode * code = (XWLTXCode*)last;
  code->scanParam(str, len,pos);
}

void XWLTXDocumentDefine::setCursor()
{
  head->next->setCursor();
}

void XWLTXDocumentDefine::write(QTextStream & strm, int & linelen)
{
  if (linelen != 0)
  {
    strm << "\n";
    linelen = 0;
  }

  QString str = head->getMimeData();
  QString tmp = head->next->getMimeData();
  str += tmp;
  tmp = head->next->next->getMimeData();
  str += tmp;
  tmp = last->prev->getMimeData();
  str += tmp;
  strm << str;

  tmp = last->getMimeData();
  str = QString("{\n%1}\n").arg(tmp);
  strm << str;
}

XWLTXDocumentNewCommand::XWLTXDocumentNewCommand(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentDefine(LAnewcommand,docA,"\\newcommand",parent)
{}

XWLTXDocumentRenewCommand::XWLTXDocumentRenewCommand(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentDefine(LArenewcommand,docA,"\\renewcommand",parent)
{}

XWLTXDocumentDefineEnv::XWLTXDocumentDefineEnv(int tA, XWTeXDocument * docA, const QString & nameA, QObject * parent)
:XWTeXDocumentCommand(tA,docA,true, nameA, parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("env:"), this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("nargs:"), this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("default:"), this));
  add(new XWLTXCode(docA, tr("begdef:"), this));
  add(new XWLTXCode(docA, tr("enddef:"), this));
}

QString XWLTXDocumentDefineEnv::getMimeData()
{
  QString str = head->getMimeData();
  QString tmp = head->next->getMimeData();
  str += tmp;
  tmp = head->next->next->getMimeData();
  str += tmp;
  tmp = last->prev->prev->getMimeData();
  str += tmp;
  tmp = last->prev->getMimeData();
  str = QString("%1{%2}").arg(str).arg(tmp);
  tmp = last->getMimeData();
  str = QString("%1{%2}").arg(str).arg(tmp);
  return str;
}

bool XWLTXDocumentDefineEnv::hasSelected()
{
  return last->prev->hasSelected() || last->hasSelected();
}

bool XWLTXDocumentDefineEnv::isAllSelected()
{
  return (last->prev->hasSelected() || last->hasSelected()) && head->hasSelected();
}

bool XWLTXDocumentDefineEnv::isEmpty()
{
  return head->next->isEmpty() && last->prev->isEmpty() && last->isEmpty();
}

void XWLTXDocumentDefineEnv::setCursor()
{
  head->next->setCursor();
}

void XWLTXDocumentDefineEnv::write(QTextStream & strm, int & linelen)
{
  if (linelen != 0)
  {
    strm << "\n";
    linelen = 0;
  }

  QString str = head->getMimeData();
  QString tmp = head->next->getMimeData();
  str += tmp;
  tmp = head->next->next->getMimeData();
  str += tmp;
  tmp = last->prev->prev->getMimeData();
  str += tmp;
  strm << str;

  tmp = last->prev->getMimeData();
  str = QString("{\n%1}\n").arg(tmp);
  strm << str;

  tmp = last->getMimeData();
  str = QString("{\n%1}\n").arg(tmp);
  strm << str;
}

XWLTXDocumentNewEnvironment::XWLTXDocumentNewEnvironment(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentDefineEnv(LAnewenvironment,docA,"\\newenvironment",parent)
{}

XWLTXDocumentRenewEnvironment::XWLTXDocumentRenewEnvironment(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentDefineEnv(LArenewenvironment,docA,"\\renewenvironment",parent)
{}

XWLTXDocumentNewTheorem::XWLTXDocumentNewTheorem(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAnewtheorem,docA,true, "\\newtheorem", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("newenv:"), this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("numbered-like:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("label:"), this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("within:"), this));
}

QString XWLTXDocumentNewTheorem::getMimeData()
{
  QString str = head->getMimeData();
  QString tmp = head->next->getMimeData();
  str += tmp;
  tmp = head->next->next->getMimeData();
  str += tmp;
  tmp = last->prev->getMimeData();
  str += tmp;
  tmp = last->getMimeData();
  str += tmp;
  return str;
}

bool XWLTXDocumentNewTheorem::hasSelected()
{
  return head->next->hasSelected();
}

bool XWLTXDocumentNewTheorem::isAllSelected()
{
  return head->next->hasSelected() && head->hasSelected();
}

bool XWLTXDocumentNewTheorem::isEmpty()
{
  return head->next->isEmpty();
}

void XWLTXDocumentNewTheorem::setCursor()
{
  head->next->setCursor();
}

XWLTXDocumentNewFont::XWLTXDocumentNewFont(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAnewfont,docA,true, "\\newfont", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("cmd:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("font name:"), this));
}

void XWLTXDocumentNewFont::setCursor()
{
  head->next->setCursor();
}

XWLTXDocumentNewCounter::XWLTXDocumentNewCounter(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAnewcounter,docA,true, "\\newcounter", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("cnt:"), this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("counter name:"), this));
}

void XWLTXDocumentNewCounter::setCursor()
{
  head->next->setCursor();
}

XWLTXDocumentNewLength::XWLTXDocumentNewLength(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(LAnewlength, docA, true,"\\newlength", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("arg:"), this));
}

XWLTXDocumentIncludeOnly::XWLTXDocumentIncludeOnly(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(LAincludeonly, docA, true,"\\includeonly", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("files:"), this));
}

XWLTXDocumentEnviroment::XWLTXDocumentEnviroment(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAdocument,docA,"\\begin{document}", parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{document}", this));
}

void XWLTXDocumentEnviroment::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  dir = doc->getDirection();
  XWTeXDocumentObject * obj = head->next;
  while (obj)
  {
    obj->newRow = true;
    if (obj == last)
      break;

    if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
    {
      if (doc->isInPreamble())
        obj->breakPage(curx,cury,firstcolumn);
      else
      {
        if (obj->type() == BMframe)
        {
          obj->newPage = true;
          obj->breakPage(curx,cury,firstcolumn);
        }
      }
    }
    else
    {
      switch (obj->type())
      {
        case LAmaketitle:
        case LAtableofcontents:
          if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_ARTICLE)
            obj->newPage = false;
          else
            obj->newPage = true;
          obj->breakPage(curx,cury,firstcolumn);
          break;

        case LAprintindex:
        case LAprintglossary:
        case LAthebibliography:
          obj->newPage = true;
          obj->breakPage(curx,cury,firstcolumn);
          break;

        case LAappendix:
          ldoc->setAppendix();
          obj->newPage = true;
          obj->breakPage(curx,cury,firstcolumn);
          break;

        case LAtitle:
        case LAauthor:
        case LAdate:
        case LAthanks:
        case LAname:
        case LAsignature:
        case LAtelephone:
        case LAlocation:
        case LAaddress:
        case BMsubtitle:
        case BMinstitute:
          break;

        default:
          obj->breakPage(curx,cury,firstcolumn);
          break;
      }
    }

    obj = obj->next;
  }
}

void XWLTXDocumentEnviroment::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  XWTeXDocumentObject * obj = 0;
  XWTeXDocumentObject * nobj = head;
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else
    {
      if (str[pos] == QChar('%'))
        obj = new XWTeXDocumentComment(doc,this);
      else if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        if (id == LAend)
        {
          key = scanEnviromentName(str,len,pos);
          id = lookupLaTeXID(key);
          if (id == LAdocument)
            return ;
        }

        obj = doc->createObject(key,str,pos,this);
        switch (obj->type())
        {
          case LAinput:
          case LAinclude:
          case LApart:
          case LAsection:
          case LAsubsection:
          case LAsubsubsection:
          case LAparagraph:
          case LAsubparagraph:
          case LAbibliographystyle:
          case LAbibliography:
          case LAabstract:
          case LAthebibliography:
          case LACJK:
          case LACJKStar:
          case LAmaketitle:
          case LAprintindex:
          case LAprintglossary:
          case LAtableofcontents:
          case LAappendix:
          case BMlecture:
          case BMagainframe:
          case BMmode:
          case BMAtBeginPart:
          case BMAtBeginLecture:
          case BMAtBeginNote:
          case BMAtEndNote:
          case BMnote:
            break;

          case BMframe:
            ldoc->setBeamerClass();
            break;

          case LAchapter:
            ldoc->setBookClass();
            break;

          case LAletter:
            ldoc->setLetterClass();
            break;

          default:
            pos = p;
            delete obj;
            obj = new XWLTXDocumentPar(doc,this);
            break;
        }
      }
      else
        obj = new XWLTXDocumentPar(doc,this);

      nobj->insertAtAfter(obj);
      obj->scan(str,len,pos);
      nobj = obj;
    }
  }
}

XWLTXDocumentCJK::XWLTXDocumentCJK(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LACJK,docA,true, "\\begin{CJK}", parent),
 topCJK(true)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("encode:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("font:"), this));
  add(new XWTeXControlSequence(docA, true, "\\end{CJK}", this));
}

void XWLTXDocumentCJK::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (topCJK)
  {
    XWTeXDocumentObject * obj = head->next->next->next;
    while (obj)
    {
      obj->newRow = true;
      if (obj == last)
        break;

      if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && !doc->isInPreamble())
      {
        if (obj->type() == BMframe)
        {
          obj->newPage = true;
          obj->breakPage(curx,cury,firstcolumn);
        }
      }
      else
        obj->breakPage(curx,cury,firstcolumn);

      obj = obj->next;
    }
  }
  else
  {
    XWTeXDocumentObject * obj = head;
    while (obj)
    {
      obj->breakPage(curx,cury,firstcolumn);
      obj = obj->next;
    }
  }
}

void XWLTXDocumentCJK::clear()
{
  XWTeXDocumentObject * obj = head->next->next->next;
  while (obj != last)
  {
    XWTeXDocumentObject * tmp = obj->next;
    delete obj;
    obj = tmp;
  }
}

void XWLTXDocumentCJK::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  sobj->prev = head->next->next;
  eobj->next = head->next->next->next;
  head->next->next->next->prev = eobj;    
  head->next->next->next = sobj;
}

void XWLTXDocumentCJK::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next->next;
  *eobj = last->prev;
  head->next->next->next = last;
  last->prev = head->next->next;
}

void XWLTXDocumentCJK::removeChildrenTo(XWTeXDocumentObject**sobj, 
                                        XWTeXDocumentObject**eobj,
                                        XWTeXDocumentObject**obj)
{
  *sobj = head->next->next->next;
  *obj = (*sobj)->prev;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWLTXDocumentCJK::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str,len,pos);
  head->next->next->scan(str,len,pos);
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)doc;
  topCJK = ldoc->isTopObject((XWTeXDocumentObject*)this);
  while (pos < len && str[pos].isSpace())
    pos++;

  XWTeXDocumentObject * obj = 0;
  XWTeXDocumentObject * nobj = head;
  if (topCJK)
  {
    while (pos < len)
    {
      if (str[pos].isSpace())
        pos++;
      else
      {
        if (str[pos] == QChar('%'))
          obj = new XWTeXDocumentComment(doc,this);
        else if (str[pos] == QChar('\\'))
        {
          int p = pos;
          QString key = scanControlSequence(str,len,pos);
          int id = lookupLaTeXID(key);
          if (id == LAend)
          {
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == LACJK)
              return ;
          }

          obj = doc->createObject(key,str,pos,this);
          switch (obj->type())
          {
            case LAinput:
            case LAinclude:
            case LApart:
            case LAsection:
            case LAsubsection:
            case LAsubsubsection:
            case LAparagraph:
            case LAsubparagraph:
            case LAbibliographystyle:
            case LAbibliography:
            case LAabstract:
            case LAthebibliography:
            case LACJK:
            case LACJKStar:
            case LAmaketitle:
            case LAprintindex:
            case LAprintglossary:
            case LAtableofcontents:
            case LAappendix:
            case BMlecture:
            case BMagainframe:
            case BMmode:
            case BMAtBeginPart:
            case BMAtBeginLecture:
            case BMAtBeginNote:
            case BMAtEndNote:
            case BMnote:
              break;

            case BMframe:
              ldoc->setBeamerClass();
              break;

            case LAchapter:
              ldoc->setBookClass();
              break;

            case LAletter:
              ldoc->setLetterClass();
              break;

            default:
              pos = p;
              delete obj;
              obj = new XWLTXDocumentPar(doc,this);
              break;
          }
        }
        else
          obj = new XWLTXDocumentPar(doc,this);

        nobj->insertAtAfter(obj);
        obj->scan(str,len,pos);
        nobj = obj;
      }
    }
  }
  else
  {
    while (pos < len)
    {
      if (str[pos] == QChar('\\'))
      {
        QString key = XWTeXDocumentObject::scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        if (id == LAend)
        {
          key = scanEnviromentName(str,len,pos);
          return;
        }

        obj = doc->createObject(key,str,pos,this);
      }
      else if (str[pos] == QChar('$'))
      {
        if (pos < (len - 1) && str[pos + 1] == QChar('$'))
          obj = new XWTeXDocumentDisplayFormular(doc,this);
        else
          obj = new XWTeXDocumentFormular(doc,this);
      }
      else if (str[pos] == QChar('%'))
        obj = new XWTeXDocumentComment(doc,this);
      else
        obj = new XWTeXDocumentText(doc,this);

      nobj->insertAtAfter(obj);
      obj->scan(str,len,pos);
      nobj = obj;
    }
  }
}

XWLTXDocumentCJKStar::XWLTXDocumentCJKStar(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LACJKStar,docA,true, "\\begin{CJK*}", parent),
 topCJK(true)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("encode: "), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("font: "), this));
  add(new XWTeXControlSequence(docA, true, "\\end{CJK*}", this));
}

void XWLTXDocumentCJKStar::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (topCJK)
  {
    XWTeXDocumentObject * obj = head->next->next->next;
    while (obj)
    {
      obj->newRow = true;
      if (obj == last)
        break;

        if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && !doc->isInPreamble())
        {
          if (obj->type() == BMframe)
          {
            obj->newPage = true;
            obj->breakPage(curx,cury,firstcolumn);
          }
        }
        else
          obj->breakPage(curx,cury,firstcolumn);

      obj = obj->next;
    }
  }
  else
  {
    XWTeXDocumentObject * obj = head;
    while (obj)
    {
      obj->breakPage(curx,cury,firstcolumn);
      obj = obj->next;
    }
  }
}

void XWLTXDocumentCJKStar::clear()
{
  XWTeXDocumentObject * obj = head->next->next->next;
  while (obj != last)
  {
    XWTeXDocumentObject * tmp = obj->next;
    delete obj;
    obj = tmp;
  }
}

void XWLTXDocumentCJKStar::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  sobj->prev = head->next->next;
  eobj->next = head->next->next->next;
  head->next->next->next->prev = eobj;    
  head->next->next->next = sobj;
}

void XWLTXDocumentCJKStar::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next->next;
  *eobj = last->prev;
  head->next->next->next = last;
  last->prev = head->next->next;
}

void XWLTXDocumentCJKStar::removeChildrenTo(XWTeXDocumentObject**sobj, 
                                        XWTeXDocumentObject**eobj,
                                        XWTeXDocumentObject**obj)
{
  *sobj = head->next->next->next;
  *obj = (*sobj)->prev;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWLTXDocumentCJKStar::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str,len,pos);
  head->next->next->scan(str,len,pos);
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)doc;
  topCJK = ldoc->isTopObject((XWTeXDocumentObject*)this);
  while (pos < len && str[pos].isSpace())
    pos++;

  XWTeXDocumentObject * obj = 0;
  XWTeXDocumentObject * nobj = head;
  if (topCJK)
  {
    while (pos < len)
    {
      if (str[pos].isSpace())
        pos++;
      else
      {
        if (str[pos] == QChar('%'))
          obj = new XWTeXDocumentComment(doc,this);
        else if (str[pos] == QChar('\\'))
        {
          int p = pos;
          QString key = scanControlSequence(str,len,pos);
          int id = lookupLaTeXID(key);
          if (id == LAend)
          {
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == LACJKStar)
              return ;
          }

          obj = doc->createObject(key,str,pos,this);
          switch (obj->type())
          {
            case LAinput:
            case LAinclude:
            case LApart:
            case LAsection:
            case LAsubsection:
            case LAsubsubsection:
            case LAparagraph:
            case LAsubparagraph:
            case LAbibliographystyle:
            case LAbibliography:
            case LAabstract:
            case LAthebibliography:
            case LACJK:
            case LACJKStar:
            case LAmaketitle:
            case LAprintindex:
            case LAprintglossary:
            case LAtableofcontents:
            case LAappendix:
            case BMlecture:
            case BMagainframe:
            case BMmode:
            case BMAtBeginPart:
            case BMAtBeginLecture:
            case BMAtBeginNote:
            case BMAtEndNote:
            case BMnote:
              break;

            case BMframe:
              ldoc->setBeamerClass();
              break;

            case LAchapter:
              ldoc->setBookClass();
              break;

            case LAletter:
              ldoc->setLetterClass();
              break;

            default:
              pos = p;
              delete obj;
              obj = new XWLTXDocumentPar(doc,this);
              break;
          }
        }
        else
          obj = new XWLTXDocumentPar(doc,this);

        nobj->insertAtAfter(obj);
        obj->scan(str,len,pos);
        nobj = obj;
      }
    }
  }
  else
  {
    while (pos < len)
    {
      if (str[pos] == QChar('\\'))
      {
        QString key = XWTeXDocumentObject::scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        if (id == LAend)
        {
          key = scanEnviromentName(str,len,pos);
          return;
        }
        obj = doc->createObject(key,str,pos,this);
      }
      else if (str[pos] == QChar('$'))
      {
        if (pos < (len - 1) && str[pos + 1] == QChar('$'))
          obj = new XWTeXDocumentDisplayFormular(doc,this);
        else
          obj = new XWTeXDocumentFormular(doc,this);
      }
      else if (str[pos] == QChar('%'))
        obj = new XWTeXDocumentComment(doc,this);
      else
        obj = new XWTeXDocumentText(doc,this);

      nobj->insertAtAfter(obj);
      obj->scan(str,len,pos);
      nobj = obj;
    }
  }
}

XWLTXDocumentInclude::XWLTXDocumentInclude(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(LAinclude, docA, false,"\\include", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("file: "), this));
}

XWLTXDocumentInput::XWLTXDocumentInput(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(LAinput, docA, true, "\\input", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("file: "), this));
}

XWLTXDocumentTitle::XWLTXDocumentTitle(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(LAtitle, docA, true,"\\title", parent)
{
  add(new XWLTXOption(docA,this));
  add(new XWLTXParam(docA,this));
  head->next->specialFont = false;
  head->next->next->specialFont = false;
}

void XWLTXDocumentTitle::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  dir = doc->getDirection();
  if (doc->isInPreamble())
  {
    head->newRow = true;
    head->next->specialFont = true;
    head->next->newPage = false;
    head->next->newRow = false;
    head->next->next->specialFont = true;
    head->next->next->newPage = false;
    head->next->next->newRow = false;
    XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
    return ;
  }

  firstcolumn = true;
  doc->savedTwoColumn();
  doc->LARGE();
  setNormalFont();
  head->next->next->specialFont = false;
  head->next->next->newPage = true;
  head->next->next->newRow = true;

  switch (ldoc->getClassIndex())
  {
    case XW_LTX_DOC_CLS_BOOK:
      {
        switch (dir)
        {
          case TEX_DOC_WD_TLT:
            cury = doc->getTop() + 60;
            head->next->next->breakPage(font, color, curx, cury,firstcolumn);
            cury += 3 * doc->getCurrentFontSize();
            doc->setSavedTop(cury);
            break;

          case TEX_DOC_WD_RTT:
            curx = doc->getRight() - 2 * doc->getCurrentFontSize();
            head->next->next->breakPage(font, color, curx, cury,firstcolumn);
            curx -= doc->getCurrentFontSize();
            doc->setSavedTop(curx);
            break;

          case TEX_DOC_WD_LTL:
            curx = doc->getLeft() + 2 * doc->getCurrentFontSize();
            head->next->next->breakPage(font, color, curx, cury,firstcolumn);
            curx += doc->getCurrentFontSize();
            doc->setSavedTop(curx);
            break;

          default:
            cury = doc->getTop() + 60;
            head->next->next->breakPage(font, color, curx, cury,firstcolumn);
            cury += 3 * doc->getCurrentFontSize();
            doc->setSavedTop(cury);
            break;
        }
      }
      break;

    case XW_LTX_DOC_CLS_REPORT:
      {
        switch (dir)
        {
          case TEX_DOC_WD_TLT:
          case TEX_DOC_WD_TRT:
            cury = doc->getTop() + 60;
            head->next->next->centered(font, color, curx, cury,firstcolumn);
            cury += 3 * doc->getCurrentFontSize();
            doc->setSavedTop(cury);
            break;

          case TEX_DOC_WD_RTT:
            curx = doc->getRight() - 2 * doc->getCurrentFontSize();
            head->next->next->centered(font, color, curx, cury,firstcolumn);
            curx -= doc->getCurrentFontSize();
            doc->setSavedTop(curx);
            break;

          default:
            curx = doc->getLeft() + 2 * doc->getCurrentFontSize();
            head->next->next->centered(font, color,curx, cury,firstcolumn);
            curx += doc->getCurrentFontSize();
            doc->setSavedTop(curx);
            break;
        }
      }
      break;

    case XW_LTX_DOC_CLS_ARTICLE:
      {
        switch (dir)
        {
          case TEX_DOC_WD_TLT:
          case TEX_DOC_WD_TRT:
            cury = doc->getTop() + 2 * doc->getCurrentFontSize();
            head->next->next->centered(font, color, curx, cury,firstcolumn);
            cury += 1.5 * doc->getCurrentFontSize();
            doc->setSavedTop(cury);
            break;

          case TEX_DOC_WD_RTT:
            curx = doc->getRight() - doc->getCurrentFontSize();
            head->next->next->centered(font, color, curx, cury,firstcolumn);
            curx -= doc->getCurrentFontSize();
            doc->setSavedTop(curx);
            break;

          default:
            curx = doc->getLeft() + doc->getCurrentFontSize();
            head->next->next->centered(font, color, curx, cury,firstcolumn);
            curx += doc->getCurrentFontSize();
            doc->setSavedTop(curx);
            break;
        }
      }
      break;

    default:
      {
        doc->bfseries();
        switch (dir)
        {
          case TEX_DOC_WD_TLT:
          case TEX_DOC_WD_TRT:
            cury = (doc->getBottom() - doc->getTop()) / 4;
            head->next->next->centered(font, color, curx, cury,firstcolumn);
            cury += doc->getCurrentFontSize();
            doc->setSavedTop(cury);
            break;

          case TEX_DOC_WD_RTT:
            curx = doc->getRight() - (doc->getRight() - doc->getLeft()) / 5;
            head->next->next->centered(font, color, curx, cury,firstcolumn);
            curx -= doc->getCurrentFontSize();
            doc->setSavedTop(curx);
            break;

          default:
            curx = (doc->getRight() - doc->getLeft()) / 5;
            head->next->next->centered(font, color, curx, cury,firstcolumn);
            curx += doc->getCurrentFontSize();
            doc->setSavedTop(curx);
            break;
        }

        doc->popWeight();
      }
      break;
  }

  head->next->next->newPage = false;
  head->next->next->newRow = false;

  doc->popSize();
  doc->restoreTwoColumn();
}

XWLTXDocumentAuthor::XWLTXDocumentAuthor(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(LAauthor, docA, true, "\\author", parent)
{
  add(new XWTeXDocumentOption(XW_BEAMER_OPTION,docA,tr("short name:"), this));
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"{",this));
  add(new XWLTXRow(docA,this));
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"}",this));
  andObj = new XWTeXControlSequence(LAAnd,docA, " \\and ",this);
}

void XWLTXDocumentAuthor::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  dir = doc->getDirection();
  setNormalFont();
  if (doc->isInPreamble())
  {
    head->newRow = true;
    head->breakPage(curx, cury,firstcolumn);

    XWTeXDocumentObject * obj = head->next;
    obj->newPage = false;
    obj->newRow = false;
    obj = obj->next;
    while (obj)
    {
      obj->newPage = false;
      obj->newRow = false;
      obj->breakPage(font,color,curx, cury,firstcolumn);
      if (obj != last->prev)
        andObj->breakPage(curx, cury,firstcolumn);

      obj = obj->next;
    }
    return ;
  }

  firstcolumn = true;
  doc->savedTwoColumn();
  doc->large();
  setNormalFont();
  XWTeXDocumentObject * obj = head->next->next->next;
  while (obj)
  {
    if (obj == last)
      break;

    obj->newPage = false;
    obj->newRow = true;
    switch (ldoc->getClassIndex())
    {
      case XW_LTX_DOC_CLS_ARTICLE:
      case XW_LTX_DOC_CLS_BEAMER:
        {
          switch (dir)
          {
            case TEX_DOC_WD_TLT:
            case TEX_DOC_WD_TRT:
              obj->centered(font, color, curx,cury,firstcolumn);
              cury += doc->getCurrentFontSize();
              doc->setSavedTop(cury);
              break;

            case TEX_DOC_WD_RTT:
              obj->centered(font, color, curx,cury,firstcolumn);
              curx -= doc->getCurrentFontSize();
              doc->setSavedTop(curx);
              break;

            default:
              obj->centered(font, color, curx,cury,firstcolumn);
              curx += doc->getCurrentFontSize();
              doc->setSavedTop(curx);
              break;
          }
        }
        break;

      case XW_LTX_DOC_CLS_REPORT:
        {
          switch (dir)
          {
            case TEX_DOC_WD_TLT:
            case TEX_DOC_WD_TRT:
              obj->centered(font, color, curx,cury,firstcolumn);
              cury += 1.5 * doc->getCurrentFontSize();
              doc->setSavedTop(cury);
              break;

            case TEX_DOC_WD_RTT:
              obj->centered(font, color, curx,cury,firstcolumn);
              curx -= doc->getCurrentFontSize();
              doc->setSavedTop(curx);
              break;

            default:
              obj->centered(font, color, curx,cury,firstcolumn);
              curx += doc->getCurrentFontSize();
              doc->setSavedTop(curx);
              break;
          }
        }
        break;

      default:
        {
          switch (dir)
          {
            case TEX_DOC_WD_TLT:
              obj->breakPage(font, color, curx,cury,firstcolumn);
              cury += 1.5 * doc->getCurrentFontSize();
              doc->setSavedTop(cury);
              break;

            case TEX_DOC_WD_TRT:
              obj->breakPage(font, color, curx,cury,firstcolumn);
              cury += 1.5 * doc->getCurrentFontSize();
              doc->setSavedTop(cury);
              break;

            case TEX_DOC_WD_RTT:
              obj->breakPage(font, color, curx,cury,firstcolumn);
              curx -= doc->getCurrentFontSize();
              doc->setSavedTop(curx);
              break;

            default:
              obj->breakPage(font, color, curx,cury,firstcolumn);
              curx += doc->getCurrentFontSize();
              doc->setSavedTop(curx);
              break;
          }
        }
        break;
    }

    obj->newRow = false;
    obj = obj->next;
  }

  doc->popSize();
  doc->restoreTwoColumn();
}

QString XWLTXDocumentAuthor::getMimeData()
{
  QString str = head->getMimeData();
  QString tmp = head->next->getMimeData();
  if (!tmp.isEmpty())
    str += tmp;

  XWTeXDocumentObject * obj = head->next->next->next;
  while (obj)
  {
    if (obj == last)
      break;

    XWLTXRow * row = (XWLTXRow*)(obj);
    str += row->text;
    if (obj != last->prev)
      str += andObj->text;
  }
  tmp = last->getMimeData();
  str += tmp;
  return str;
}

void XWLTXDocumentAuthor::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str,len,pos);
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos >= len || str[pos] != QChar('{'))
    return ;

  pos++;
  int b = 0;
  XWLTXRow * obj = (XWLTXRow*)(head->next->next->next);
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
    else if (str[pos] == QChar('\\'))
    {
      int p = pos;
      QString key = scanControlSequence(str,len,pos);
      if (key == "and")
      {
        obj->text = obj->text.trimmed();
        obj = new XWLTXRow(doc, this);
        last->insertAtBefore(obj);
        if (str[pos].isSpace())
          pos++;
        continue;
      }

      pos = p;
    }

    obj->text.append(str[pos]);
    pos++;
  }

  if (obj)
    obj->text = obj->text.trimmed();
}

void XWLTXDocumentAuthor::write(QTextStream & strm, int & linelen)
{
  if (linelen !=  0)
  {
    linelen = 0;
    strm << "\n";
  }

  QString str = getMimeData();
  strm << str;
  strm << "\n";
}

XWLTXDocumentDate::XWLTXDocumentDate(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(LAdate, docA, true,"\\date",parent)
{
  add(new XWTeXDocumentOption(XW_BEAMER_OPTION,docA,tr("short name:"), this));
  add(new XWLTXParam(docA,this));
  head->next->specialFont = false;
}

void XWLTXDocumentDate::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  dir = doc->getDirection();
  if (doc->isInPreamble())
  {
    head->next->newPage = false;
    head->next->newRow = false;
    head->next->next->newPage = false;
    head->next->next->newRow = false;
    XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
    return ;
  }

  firstcolumn = true;
  doc->savedTwoColumn();
  doc->large();
  setNormalFont();
  head->next->next->newPage = false;
  head->next->next->newRow = true;
  switch (ldoc->getClassIndex())
  {
    case XW_LTX_DOC_CLS_ARTICLE:
    case XW_LTX_DOC_CLS_REPORT:
    case XW_LTX_DOC_CLS_BEAMER:
      {
        switch (dir)
        {
          case TEX_DOC_WD_TLT:
          case TEX_DOC_WD_TRT:
            head->next->next->centered(font, color, curx, cury,firstcolumn);
            cury += 1.5 * doc->getCurrentFontSize();
            doc->setSavedTop(cury);
            break;

          case TEX_DOC_WD_RTT:
            head->next->next->centered(font, color, curx, cury,firstcolumn);
            curx -= doc->getCurrentFontSize();
            doc->setSavedTop(curx);
            break;

          default:
            head->next->next->centered(font, color, curx, cury,firstcolumn);
            curx += doc->getCurrentFontSize();
            doc->setSavedTop(curx);
            break;
        }
      }
      break;

    case XW_LTX_DOC_CLS_BOOK:
      {
        switch (dir)
        {
          case TEX_DOC_WD_TLT:
            head->next->next->breakPage(font, color, curx, cury,firstcolumn);
            cury += 1.5 * doc->getCurrentFontSize();
            doc->setSavedTop(cury);
            break;

          case TEX_DOC_WD_TRT:
            head->next->next->breakPage(font, color, curx, cury,firstcolumn);
            cury += 1.5 * doc->getCurrentFontSize();
            doc->setSavedTop(cury);
            break;

          case TEX_DOC_WD_RTT:
            head->next->next->breakPage(font, color, curx, cury,firstcolumn);
            curx -= doc->getCurrentFontSize();
            doc->setSavedTop(curx);
            break;

          default:
            head->next->next->breakPage(font, color, curx, cury,firstcolumn);
            curx += doc->getCurrentFontSize();
            doc->setSavedTop(curx);
            break;
        }
      }
      break;

    default:
        doc->normalSize();
        setNormalFont();
        head->next->next->flushBottom(font, color, curx, cury,firstcolumn);
      break;
  }

  head->next->next->newRow = false;

  doc->popSize();
  doc->restoreTwoColumn();
}

XWLTXDocumentThanks::XWLTXDocumentThanks(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(LAthanks, docA, true, "\\thanks",parent)
{
  add(new XWLTXParam(docA,this));
  head->next->specialFont = false;
}

void XWLTXDocumentThanks::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  if (doc->isInPreamble())
  {
    head->next->setNewRow(false);
    XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
    return ;
  }

  firstcolumn = true;
  doc->savedTwoColumn();
  doc->footnoteSize();
  setNormalFont();
  head->next->setNewRow(true);
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      cury = doc->getBottom() - 2 * doc->getCurrentFontSize();
      doc->setSavedBottom(cury - 2 * doc->getCurrentFontSize());
      break;

    case TEX_DOC_WD_TRT:
      cury = doc->getBottom() - 2 * doc->getCurrentFontSize();
      doc->setSavedBottom(cury - 2 * doc->getCurrentFontSize());
      break;

    case TEX_DOC_WD_RTT:
      curx = doc->getLeft() + 2 * doc->getCurrentFontSize();
      doc->setSavedBottom(curx - 2 * doc->getCurrentFontSize());
      break;

    default:
      curx = doc->getRight() - 2 * doc->getCurrentFontSize();
      doc->setSavedBottom(curx - 2 * doc->getCurrentFontSize());
      break;
  }

  head->next->breakPage(font, color, curx, cury,firstcolumn);

  head->next->setNewRow(false);
  doc->popSize();
  doc->restoreTwoColumn();
}

XWLTXDocumentTitlePage::XWLTXDocumentTitlePage(XWTeXDocument * docA, QObject * parent)
: XWLTXEnviroment(LAtitlepage, docA,"\\begin{titlepage}",parent)
{
  add(new XWLTXCode(docA, tr("contents: "), this));
  add(new XWTeXControlSequence(docA, true, "\\end{titlepage}", this));
}

void XWLTXDocumentTitlePage::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  firstcolumn = true;
  doc->savedTwoColumn();
  indent = 2.0;
  head->newPage = true;
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
  doc->restoreTwoColumn();
  if (next)
    next->newPage = true;
}

void XWLTXDocumentTitlePage::scan(const QString & str, int & len, int & pos)
{
  XWLTXCode * code = (XWLTXCode*)(head->next);
  code->scan(str,len,pos);
}

bool XWLTXDocumentTitlePage::isEmpty()
{
  return head->next->isEmpty();
}

void XWLTXDocumentTitlePage::setCursor()
{
  head->next->setCursor();
}

XWLTXTableOfContents::XWLTXTableOfContents(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(LAtableofcontents, docA,true, "\\tableofcontents",parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("comma-separated option list:"),this));
}

void XWLTXTableOfContents::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
    XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
  else
    head->breakPage(curx,cury,firstcolumn);
}

XWLTXAppendix::XWLTXAppendix(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(LAappendix, docA,true, "\\appendix",parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("mode specification:"),this));
}

void XWLTXAppendix::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
    XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
  else
    head->breakPage(curx,cury,firstcolumn);
}

XWLTXParEnv::XWLTXParEnv(int tA, XWTeXDocument * docA, const QString & nameA, QObject * parent)
: XWLTXEnviroment(tA, docA,nameA,parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("action specification:"),this));
  afterNewRow = true;
  add(new XWLTXDocumentPar(docA,this));
}

void XWLTXParEnv::clear()
{
  XWTeXDocumentObject * obj = head->next->next;
  while (obj != last)
  {
    XWTeXDocumentObject * tmp = obj->next;
    delete obj;
    obj = tmp;
  }
}

void XWLTXParEnv::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  sobj->prev = head->next;
  head->next->next = sobj;
  eobj->next = last->prev;
  last->prev = eobj;   
}

bool XWLTXParEnv::isAtStart()
{
  return (head->next->next->isAtStart() || head->next->isAtStart());
}

void XWLTXParEnv::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next;
  *eobj = last->prev;
  head->next->next = last;
  last->prev = head->next;
}

void XWLTXParEnv::removeChildrenTo(XWTeXDocumentObject**sobj, 
                                        XWTeXDocumentObject**eobj,
                                        XWTeXDocumentObject**obj)
{
  *sobj = head->next->next;
  *obj = (*sobj)->prev;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWLTXParEnv::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentObject * obj = head->next;
  obj->scan(str,len,pos);
  obj = obj->next;
  del(obj);
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else
    {
      if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        switch (id)
        {
          case LAend:
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (type() == id)
              return ;

            break;

          default:
            break;
        }

        pos = p;
      }

      obj = new XWLTXDocumentPar(doc,this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

XWLTXDocumentAbstract::XWLTXDocumentAbstract(XWTeXDocument * docA, QObject * parent)
: XWLTXParEnv(LAabstract, docA,"\\begin{abstract}",parent)
{
  name = new XWTeXDocumentText(doc,this);
  name->text = QString(tr("Abstract"));
  add(new XWTeXControlSequence(docA, true, "\\end{abstract}", this));
}

void XWLTXDocumentAbstract::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && doc->isInPreamble())
  {
    XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
    return ;
  }
  doc->savedTwoColumn();
  dir = doc->getDirection();
  doc->small();
  setBoldFont();

  name->newPage = (ldoc->getClassIndex() == XW_LTX_DOC_CLS_REPORT);
  name->newRow = true;
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
    case TEX_DOC_WD_TRT:
        name->centered(font, color, curx,cury,firstcolumn);
        cury += doc->getCurrentFontSize();
      break;

    case TEX_DOC_WD_RTT:
        name->centered(font, color, curx,cury,firstcolumn);
        curx -= doc->getCurrentFontSize();
      break;

    default:
        name->centered(font, color, curx,cury,firstcolumn);
        curx += doc->getCurrentFontSize();
      break;
  }

  doc->popSize();

  doc->normalSize();
  setNormalFont();

  XWTeXDocumentObject * obj = head->next->next;
  while (obj)
  {
    if (obj == last)
      break;

    obj->firstIndent = 2.0;
    obj->setIndent(4.0);
    obj->breakPage(font,color,curx,cury,firstcolumn);
    obj = obj->next;
  }

  doc->popSize();
  doc->restoreTwoColumn();

  switch (dir)
  {
    case TEX_DOC_WD_TLT:
    case TEX_DOC_WD_TRT:
        doc->setSavedTop(cury + doc->getCurrentFontSize());
      break;

    default:
        doc->setSavedTop(curx + doc->getCurrentFontSize());
      break;
  }

  if (next)
    next->newPage = name->newPage;
}

bool XWLTXDocumentAbstract::isAtStart()
{
  return (head->next->next->isAtStart() || head->next->isAtStart() || name->isAtStart()); 
}

XWLTXDocumentSectioning::XWLTXDocumentSectioning(int tA,
                                              XWTeXDocument * docA,
                                              const QString & nameA,
                                              QObject * parent)
: XWTeXDocumentCommand(tA, docA,true, nameA, parent)
{
  add(new XWLTXStar(docA,this));
  add(new XWTeXDocumentBeamerSpec(docA,tr("mode specification:"),this));
  add(new XWLTXOption(docA,this));
  add(new XWLTXParam(docA,this));
  countText = new XWTeXDocumentText(docA,this);
  head->next->next->next->specialFont = false;
  head->next->next->next->next->specialFont = false;
}

void XWLTXDocumentSectioning::addStar()
{
  XWLTXStar * star = (XWLTXStar*)(head->next->next);
  star->star();
}

void XWLTXDocumentSectioning::append(XWTeXDocumentObject * obj)
{
  last->append(obj);
}

void XWLTXDocumentSectioning::append(XWTeXDocumentObject * sobj,XWTeXDocumentObject * eobj)
{
  last->append(sobj, eobj);
}

void XWLTXDocumentSectioning::clear()
{
  last->clear();
}

bool XWLTXDocumentSectioning::hasStar()
{
  XWLTXStar * star = (XWLTXStar*)(head->next->next);
  return !(star->isEmpty());
}

void XWLTXDocumentSectioning::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  last->insertChildren(sobj,eobj);
}

void XWLTXDocumentSectioning::insertChildren(XWTeXDocumentObject*obj, 
                                             XWTeXDocumentObject*sobj, 
                                             XWTeXDocumentObject*eobj)
{
  last->insertChildren(obj,sobj,eobj);
}

bool XWLTXDocumentSectioning::isAllSelected()
{
  return last->isAllSelected();
}

bool XWLTXDocumentSectioning::isAtEnd()
{
  return last->isAtEnd();
}

bool XWLTXDocumentSectioning::isAtStart()
{
  return last->isAtStart();
}

bool XWLTXDocumentSectioning::isEmpty()
{
  return last->isEmpty();
}

void XWLTXDocumentSectioning::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  last->removeChildren(sobj,eobj);
}

void XWLTXDocumentSectioning::removeChildren(XWTeXDocumentObject**sobj, 
                                             XWTeXDocumentObject**eobj,
                                             XWTeXDocumentObject**obj)
{
  last->removeChildren(sobj,eobj,obj);
}

void XWLTXDocumentSectioning::removeChildrenFrom(XWTeXDocumentObject**sobj, 
                                                 XWTeXDocumentObject**eobj,
                                                 XWTeXDocumentObject**obj)
{
  last->removeChildrenFrom(sobj,eobj,obj);
}

void XWLTXDocumentSectioning::removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  last->removeChildrenTo(sobj,eobj);
}

void XWLTXDocumentSectioning::removeStar()
{
  XWLTXStar * star = (XWLTXStar*)(head->next->next);
  star->clear();
}

void XWLTXDocumentSectioning::write(QTextStream & strm, int & linelen)
{
  last->newRow = false;
  last->newPage = false;
  last->prev->newRow = false;
  last->prev->newPage = false;
  XWTeXDocumentObject::write(strm,linelen);
}

XWLTXDocumentPart::XWLTXDocumentPart(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentSectioning(LApart,docA,"\\part",parent)
{}

void XWLTXDocumentPart::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  doc->resetChapterCounter();
  doc->resetSectionCounter();
  ldoc->setCurrentPart(this);
  ldoc->setCurrentChapter(0);

  dir = doc->getDirection();

  countText->indent = 0;
  last->indent = 0;
  last->prev->indent = 0;

  int count = doc->getPartCount();
  countText->text = QString(tr("Part %1")).arg(count);

  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
  {
    if (doc->isInPreamble())
    {
      last->newRow = false;
      last->newPage = false;
      last->prev->newRow = false;
      last->prev->newPage = false;
      XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
    }
    else
    {
      last->newRow = true;
      doc->tiny();
      last->centered(curx,cury,firstcolumn);
      doc->popSize();
    }
  }
  else
  {
    doc->bfseries();

    switch (ldoc->getClassIndex())
    {
      case XW_LTX_DOC_CLS_ARTICLE:
          doc->Large();
          setBoldFont();
          countText->setNewRow(true);
          countText->setNewPage(false);
          countText->breakPage(font, color, curx, cury,firstcolumn);

          doc->popSize();

          doc->huge();
          setBoldFont();

          if (ldoc->isShowSection(this))
          {
            last->prev->newPage = false;
            last->prev->newRow = true;
            last->prev->breakPage(font, color, curx, cury, firstcolumn);
          }

          last->newRow = true;
          last->newPage = false;
          last->breakPage(font, color, curx, cury, firstcolumn);
        break;

      case XW_LTX_DOC_CLS_BOOK:
        {
          firstcolumn = true;
          doc->savedTwoColumn();
          doc->huge();
          setBoldFont();
          switch (dir)
          {
            case TEX_DOC_WD_TLT:
              {
                cury = doc->getTop() + 2 * doc->getCurrentFontSize();
                countText->setNewPage(true);
                countText->setNewRow(true);
                countText->flushBottom(font, color, curx, cury,firstcolumn);

                cury += 2 * doc->getCurrentFontSize();

                doc->popSize();

                doc->Huge();
                setBoldFont();
                if (ldoc->isShowSection(this))
                {
                  last->prev->newPage = false;
                  last->prev->newRow = true;
                  last->prev->flushBottom(font, color, curx, cury,firstcolumn);
                }

                last->newPage = false;
                last->newRow = true;
                last->flushBottom(font, color, curx, cury,firstcolumn);
                cury += 2 * doc->getCurrentFontSize();
                doc->setSavedTop(cury);
              }
              break;

            case TEX_DOC_WD_RTT:
              {
                curx = doc->getRight() - 2 * doc->getCurrentFontSize();
                countText->setNewPage(true);
                countText->setNewRow(true);
                countText->breakPage(font, color, curx, cury,firstcolumn);

                curx -= 2 * doc->getCurrentFontSize();

                doc->popSize();

                doc->Huge();
                setBoldFont();
                if (ldoc->isShowSection(this))
                {
                  last->prev->newPage = false;
                  last->prev->newRow = true;
                  last->prev->breakPage(font, color, curx, cury,firstcolumn);
                }

                last->newPage = false;
                last->newRow = true;
                last->breakPage(font, color, curx, cury,firstcolumn);
                curx -= 2 * doc->getCurrentFontSize();
                doc->setSavedTop(curx);
              }
              break;

            case TEX_DOC_WD_TRT:
              {
                cury = doc->getTop() + 2 * doc->getCurrentFontSize();
                countText->setNewPage(true);
                countText->setNewRow(true);
                countText->flushBottom(font, color, curx, cury,firstcolumn);

                cury += 2 * doc->getCurrentFontSize();

                doc->popSize();

                doc->Huge();
                setBoldFont();

                if (ldoc->isShowSection(this))
                {
                  last->prev->newPage = false;
                  last->prev->newRow = true;
                  last->prev->flushBottom(font, color, curx, cury,firstcolumn);
                }

                last->newPage = false;
                last->newRow = true;
                last->flushBottom(font, color, curx, cury,firstcolumn);
                cury += 2 * doc->getCurrentFontSize();
                doc->setSavedTop(cury);
              }
              break;

            default:
              {
                curx = doc->getLeft() + 2 * doc->getCurrentFontSize();

                countText->setNewPage(true);
                countText->setNewRow(true);
                countText->breakPage(font, color, curx, cury,firstcolumn);

                curx += 2 * doc->getCurrentFontSize();

                doc->popSize();

                doc->Huge();
                setBoldFont();

                if (ldoc->isShowSection(this))
                {
                  last->prev->newPage = false;
                  last->prev->newRow = true;
                  last->prev->breakPage(font, color, curx, cury,firstcolumn);
                }

                last->newPage = false;
                last->newRow = true;
                last->breakPage(font, color, curx, cury,firstcolumn);
                curx += 2 * doc->getCurrentFontSize();
                doc->setSavedTop(curx);
              }
              break;
          }

          doc->restoreTwoColumn();
        }
        break;

      default:
        {
          firstcolumn = true;
          doc->savedTwoColumn();
          doc->huge();
          setBoldFont();
          switch (dir)
          {
            case TEX_DOC_WD_TLT:
            case TEX_DOC_WD_TRT:
              {
                cury = doc->getTop() + 2 * doc->getCurrentFontSize();
                countText->setNewPage(true);
                countText->setNewRow(true);
                countText->centered(font, color, curx, cury,firstcolumn);

                cury += 2 * doc->getCurrentFontSize();

                doc->popSize();

                doc->Huge();
                setBoldFont();

                if (ldoc->isShowSection(this))
                {
                  last->prev->newPage = false;
                  last->prev->newRow = true;
                  last->prev->centered(font, color, curx, cury,firstcolumn);
                }

                last->newPage = false;
                last->newRow = true;
                last->centered(font, color, curx, cury,firstcolumn);
                cury += 2 * doc->getCurrentFontSize();
                doc->setSavedTop(cury);
              }
              break;

            case TEX_DOC_WD_RTT:
              {
                curx = doc->getRight() - 2 * doc->getCurrentFontSize();
                countText->setNewPage(true);
                countText->setNewRow(true);
                countText->centered(font, color, curx, cury,firstcolumn);

                curx -= 2 * doc->getCurrentFontSize();

                doc->popSize();

                doc->Huge();
                setBoldFont();

                if (ldoc->isShowSection(this))
                {
                  last->prev->newPage = false;
                  last->prev->newRow = true;
                  last->prev->centered(font, color, curx, cury,firstcolumn);
                }

                last->newPage = false;
                last->newRow = true;
                last->centered(font, color, curx, cury,firstcolumn);
                curx -= 2 * doc->getCurrentFontSize();
                doc->setSavedTop(curx);
              }
              break;

            default:
              {
                curx = doc->getLeft() + 2 * doc->getCurrentFontSize();
                countText->setNewPage(true);
                countText->setNewRow(true);
                countText->centered(font, color, curx, cury,firstcolumn);

                curx += 2 * doc->getCurrentFontSize();

                doc->popSize();

                ldoc->Huge();
                setBoldFont();

                if (ldoc->isShowSection(this))
                {
                  last->prev->newPage = false;
                  last->prev->newRow = true;
                  last->prev->centered(font, color, curx, cury,firstcolumn);
                }

                last->newPage = false;
                last->newRow = true;
                last->centered(font, color, curx, cury,firstcolumn);
                curx += 2 * doc->getCurrentFontSize();
                doc->setSavedTop(curx);
              }
              break;
          }

          doc->restoreTwoColumn();
        }
        break;
    }

    doc->popSize();
    doc->popWeight();
  }

  last->prev->newPage = false;
  last->prev->newRow = false;
  last->newPage = false;
  last->newRow = false;
}

XWLTXDocumentChapter::XWLTXDocumentChapter(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentSectioning(LAchapter,docA,"\\chapter",parent)
{}

void XWLTXDocumentChapter::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  firstcolumn = true;
  doc->savedTwoColumn();
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  doc->resetSectionCounter();
  ldoc->setCurrentChapter(this);
  ldoc->setCurrentSection(0);

  dir = doc->getDirection();

  countText->indent = 0;
  last->indent = 0;
  last->prev->indent = 0;

  countText->newPage = true;
  countText->newRow = true;

  int count = doc->getChapterCount();
  if (doc->isAppendix())
    countText->text = QString(tr("Appendix %1")).arg(count);
  else
    countText->text = QString(tr("Chapter %1")).arg(count);

  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
  {
    if (doc->isInPreamble())
    {
      last->newRow = false;
      last->newPage = false;
      last->prev->newRow = false;
      last->prev->newPage = false;
      XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
    }
    else
    {
      last->newRow = true;
      doc->tiny();
      last->centered(curx,cury,firstcolumn);
      doc->popSize();
    }
  }
  else
  {
    doc->bfseries();
    doc->huge();
    setBoldFont();

    switch (ldoc->getClassIndex())
    {
      case XW_LTX_DOC_CLS_BOOK:
        {
          switch (dir)
          {
            case TEX_DOC_WD_TLT:
              {
                cury = doc->getTop() + 50;
                countText->setNewPage(true);
                countText->setNewRow(true);
                countText->flushBottom(font, color, curx, cury,firstcolumn);

                cury += 20;

                doc->popSize();

                ldoc->Huge();
                setBoldFont();

                if (ldoc->isShowSection(this))
                {
                  last->prev->newPage = false;
                  last->prev->newRow = true;
                  last->prev->flushBottom(font, color, curx, cury,firstcolumn);
                }

                last->newPage = false;
                last->newRow = true;
                last->flushBottom(font, color, curx, cury,firstcolumn);
                cury += 40;
                doc->setSavedTop(cury);
              }
              break;

            case TEX_DOC_WD_RTT:
              {
                curx = doc->getRight() - 50;
                countText->setNewPage(true);
                countText->setNewRow(true);
                countText->breakPage(font, color, curx, cury,firstcolumn);

                curx -= 20;
                doc->popSize();
                ldoc->Huge();
                setBoldFont();

                if (ldoc->isShowSection(this))
                {
                  last->prev->newPage = false;
                  last->prev->newRow = true;
                  last->prev->breakPage(font, color, curx, cury,firstcolumn);
                }

                last->newPage = false;
                last->newRow = true;
                last->breakPage(font, color, curx, cury,firstcolumn);
                curx -= 40;
                doc->setSavedTop(curx);
              }
              break;

            case TEX_DOC_WD_TRT:
              {
                cury = doc->getTop() + 50;
                countText->setNewPage(true);
                countText->setNewRow(true);
                countText->flushBottom(font, color, curx, cury,firstcolumn);

                cury += 20;
                doc->popSize();
                ldoc->Huge();
                setBoldFont();

                if (ldoc->isShowSection(this))
                {
                  last->prev->newPage = false;
                  last->prev->newRow = true;
                  last->prev->flushBottom(font, color, curx, cury,firstcolumn);
                }

                last->newPage = false;
                last->newRow = true;
                last->flushBottom(font, color, curx, cury,firstcolumn);
                cury += 40;
                doc->setSavedTop(cury);
              }
              break;

            default:
              {
                curx = doc->getLeft() + 50;
                countText->setNewPage(true);
                countText->setNewRow(true);
                countText->breakPage(font, color, curx, cury,firstcolumn);

                curx += 20;
                doc->popSize();
                ldoc->Huge();
                setBoldFont();

                if (ldoc->isShowSection(this))
                {
                  last->prev->newPage = false;
                  last->prev->newRow = true;
                  last->prev->breakPage(font, color, curx, cury,firstcolumn);
                }

                last->newPage = false;
                last->newRow = true;
                last->breakPage(font, color, curx, cury,firstcolumn);
                curx += 40;
                doc->setSavedTop(curx);
              }
              break;
          }
        }
        break;

      case XW_LTX_DOC_CLS_REPORT:
        {
          switch (dir)
          {
            case TEX_DOC_WD_TLT:
            case TEX_DOC_WD_TRT:
              {
                cury = doc->getTop() + 50;
                countText->setNewPage(true);
                countText->setNewRow(true);
                countText->centered(font, color, curx, cury, firstcolumn);

                cury += 20;
                doc->popSize();
                ldoc->Huge();
                setBoldFont();

                if (ldoc->isShowSection(this))
                {
                  last->prev->newPage = false;
                  last->prev->newRow = true;
                  last->prev->centered(font, color, curx, cury, firstcolumn);
                }

                last->newPage = false;
                last->newRow = true;
                last->centered(font, color, curx, cury, firstcolumn);
                cury += 40;
                doc->setSavedTop(cury);
              }
              break;

            case TEX_DOC_WD_RTT:
              {
                curx = doc->getRight() - 50;
                countText->setNewPage(true);
                countText->setNewRow(true);
                countText->centered(font, color, curx, cury, firstcolumn);

                curx -= 20;
                doc->popSize();
                ldoc->Huge();
                setBoldFont();

                if (ldoc->isShowSection(this))
                {
                  last->prev->newPage = false;
                  last->prev->newRow = true;
                  last->prev->centered(font, color, curx, cury, firstcolumn);
                }

                last->newPage = false;
                last->newRow = true;
                last->centered(font, color, curx, cury, firstcolumn);
                curx -= 40;
                doc->setSavedTop(curx);
              }
              break;

            default:
              {
                curx = doc->getLeft() + 50;
                countText->setNewPage(true);
                countText->setNewRow(true);
                countText->centered(font, color, curx, cury, firstcolumn);

                curx += 20;
                doc->popSize();
                ldoc->Huge();
                setBoldFont();

                if (ldoc->isShowSection(this))
                {
                  last->prev->newPage = false;
                  last->prev->newRow = true;
                  last->prev->centered(font, color, curx, cury, firstcolumn);
                }

                last->newPage = false;
                last->newRow = true;
                last->centered(font, color, curx, cury, firstcolumn);
                curx += 40;
                doc->setSavedTop(curx);
              }
              break;
          }
        }
        break;

      default:
        countText->setNewPage(false);
        countText->setNewRow(true);
        countText->breakPage(font, color, curx, cury, firstcolumn);
        if (ldoc->isShowSection(this))
        {
          last->prev->newPage = false;
          last->prev->newRow = true;
          last->prev->breakPage(font, color, curx, cury, firstcolumn);
        }
        last->newPage = false;
        last->newRow = true;
        last->breakPage(font, color, curx, cury, firstcolumn);
        break;
    }

    doc->popSize();
    doc->popWeight();
  }

  last->prev->newPage = false;
  last->prev->newRow = false;
  last->newPage = false;
  last->newRow = false;

  doc->restoreTwoColumn();
}

XWLTXDocumentSection::XWLTXDocumentSection(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentSectioning(LAsection,docA,"\\section",parent)
{}

void XWLTXDocumentSection::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  doc->resetSubsectionCounter();
  ldoc->setCurrentSection(this);
  ldoc->setCurrentSubsection(0);

  dir = doc->getDirection();

  countText->indent = 0;
  last->indent = 0;
  last->prev->indent = 0;

  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
  {
    if (doc->isInPreamble())
    {
      last->newRow = false;
      last->newPage = false;
      last->prev->newRow = false;
      last->prev->newPage = false;
      XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
    }
    else
    {
      last->newRow = true;
      doc->tiny();
      last->centered(curx,cury,firstcolumn);
      doc->popSize();
    }
  }
  else
  {
    doc->bfseries();
    doc->Large();
    setBoldFont();
    switch (dir)
    {
      case TEX_DOC_WD_TLT:
      case TEX_DOC_WD_TRT:
        {
          cury += 2 * doc->getCurrentFontSize();
          int count = doc->getSectionCount();
          if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_ARTICLE)
            countText->text = QString(tr("%1 ")).arg(count);
          else
          {
            int chapt = doc->getCurrentChapterCount();
            countText->text = QString(tr("%1.%2 ")).arg(chapt).arg(count);
          }

          countText->setNewRow(true);
          countText->setNewPage(false);
          countText->breakPage(font, color, curx, cury, firstcolumn);
          if (ldoc->isShowSection(this))
          {
            last->prev->newPage = false;
            last->prev->newRow = false;
            last->prev->breakPage(font, color, curx, cury, firstcolumn);
            last->newRow = true;
          }
          else
            last->newRow = false;
          last->newPage = false;
          last->breakPage(font, color, curx, cury, firstcolumn);
          cury += doc->getCurrentFontSize();
        }
        break;

      case TEX_DOC_WD_RTT:
        curx -= 2 * doc->getCurrentFontSize();
        if (ldoc->isShowSection(this))
        {
          last->prev->newPage = false;
          last->prev->newRow = true;
          last->prev->breakPage(font, color, curx, cury, firstcolumn);
        }

        last->newRow = true;
        last->newPage = false;
        last->breakPage(font, color, curx, cury, firstcolumn);
        curx -= doc->getCurrentFontSize();
        break;

      default:
        curx += 2 * doc->getCurrentFontSize();
        if (ldoc->isShowSection(this))
        {
          last->prev->newPage = false;
          last->prev->newRow = true;
          last->prev->breakPage(font, color, curx, cury, firstcolumn);
        }

        last->newRow = true;
        last->newPage = false;
        last->breakPage(font, color, curx, cury, firstcolumn);
        curx += doc->getCurrentFontSize();
        break;
    }

    doc->popSize();
    doc->popWeight();
  }

  last->prev->newPage = false;
  last->prev->newRow = false;
  last->newPage = false;
  last->newRow = false;
}

XWLTXDocumentSubsection::XWLTXDocumentSubsection(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentSectioning(LAsubsection,docA,"\\subsection",parent)
{}

void XWLTXDocumentSubsection::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  doc->resetSubsubsectionCounter();
  ldoc->setCurrentSubsection(this);
  ldoc->setCurrentSubsubsection(0);

  dir = doc->getDirection();

  countText->indent = 0;
  last->indent = 0;
  last->prev->indent = 0;

  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
  {
    if (doc->isInPreamble())
    {
      last->newRow = false;
      last->newPage = false;
      last->prev->newRow = false;
      last->prev->newPage = false;
      XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
    }
    else
    {
      last->newRow = true;
      doc->tiny();
      last->centered(curx,cury,firstcolumn);
      doc->popSize();
    }
  }
  else
  {
    doc->bfseries();
    doc->large();
    setBoldFont();

    switch (dir)
    {
      case TEX_DOC_WD_TLT:
      case TEX_DOC_WD_TRT:
        {
          cury += 1.5 * doc->getCurrentFontSize();
          int sect = doc->getCurrentSectionCount();
          int subsect = doc->getSubsectionCount();
          if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_ARTICLE)
            countText->text = QString(tr("%1.%2 ")).arg(sect).arg(subsect);
          else
          {
            int chpt = doc->getCurrentChapterCount();
            countText->text = QString(tr("%1.%2.%3 ")).arg(chpt).arg(sect).arg(subsect);
          }
          countText->setNewRow(true);
          countText->setNewPage(false);
          countText->breakPage(font, color, curx, cury, firstcolumn);

          if (ldoc->isShowSection(this))
          {
            last->prev->newPage = false;
            last->prev->newRow = false;
            last->prev->breakPage(font, color, curx, cury, firstcolumn);
            last->newRow = true;
          }
          else
            last->newRow = false;
          last->newPage = false;
          last->breakPage(font, color, curx, cury, firstcolumn);
          cury += doc->getCurrentFontSize();
        }
        break;

      case TEX_DOC_WD_RTT:
        curx -= 1.5 * doc->getCurrentFontSize();
        if (ldoc->isShowSection(this))
        {
          last->prev->newPage = false;
          last->prev->newRow = true;
          last->prev->breakPage(font, color, curx, cury, firstcolumn);
        }

        last->newRow = true;
        last->newPage = false;
        last->breakPage(font, color, curx, cury, firstcolumn);
        curx -= doc->getCurrentFontSize();
        break;

      default:
        curx += 1.5 * doc->getCurrentFontSize();
        if (ldoc->isShowSection(this))
        {
          last->prev->newPage = false;
          last->prev->newRow = true;
          last->prev->breakPage(font, color, curx, cury, firstcolumn);
        }

        last->newRow = true;
        last->newPage = false;
        last->breakPage(font, color, curx, cury, firstcolumn);
        curx += doc->getCurrentFontSize();
        break;
    }

    doc->popSize();
    doc->popWeight();
  }

  last->prev->newPage = false;
  last->prev->newRow = false;
  last->newPage = false;
  last->newRow = false;
}

XWLTXDocumentSubsubsection::XWLTXDocumentSubsubsection(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentSectioning(LAsubsubsection,docA,"\\subsubsection",parent)
{}

void XWLTXDocumentSubsubsection::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  doc->resetParagraphCounter();
  ldoc->setCurrentSubsubsection(this);
  ldoc->setCurrentParagraph(0);

  dir = doc->getDirection();

  countText->indent = 0;
  last->indent = 0;
  last->prev->indent = 0;

  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
  {
    if (doc->isInPreamble())
    {
      last->newRow = false;
      last->newPage = false;
      last->prev->newRow = false;
      last->prev->newPage = false;
      XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
    }
    else
    {
      last->newRow = true;
      doc->tiny();
      last->centered(curx,cury,firstcolumn);
      doc->popSize();
    }
  }
  else
  {
    doc->bfseries();
    setBoldFont();

    switch (dir)
    {
      case TEX_DOC_WD_TLT:
      case TEX_DOC_WD_TRT:
        {
          cury += doc->getCurrentFontSize();
          int sect = doc->getCurrentSectionCount();
          int subsect = doc->getCurrentSubsectionCount();
          int subsubsect = doc->getSubsubsectionCount();
          if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_ARTICLE)
            countText->text = QString(tr("%1.%2.%3 ")).arg(sect)
                               .arg(subsect).arg(subsubsect);
          else
          {
            int chpt = doc->getCurrentChapterCount();
            countText->text = QString(tr("%1.%2.%3.%4 ")).arg(chpt).arg(sect)
                                 .arg(subsect).arg(subsubsect);
          }

          countText->setNewRow(true);
          countText->setNewPage(false);
          countText->breakPage(font, color, curx, cury, firstcolumn);

          if (ldoc->isShowSection(this))
          {
            last->prev->newPage = false;
            last->prev->newRow = false;
            last->prev->breakPage(font, color, curx, cury, firstcolumn);
            last->newRow = true;
          }
          else
            last->newRow = false;
          last->newPage = false;
          last->breakPage(font, color, curx, cury, firstcolumn);
          cury += doc->getCurrentFontSize();
        }
        break;

      case TEX_DOC_WD_RTT:
        curx -= doc->getCurrentFontSize();
        if (ldoc->isShowSection(this))
        {
          last->prev->newPage = false;
          last->prev->newRow = true;
          last->prev->breakPage(font, color, curx, cury, firstcolumn);
        }

        last->newRow = true;
        last->newPage = false;
        last->breakPage(font, color, curx, cury, firstcolumn);
        curx -= doc->getCurrentFontSize();
        break;

      default:
        curx += doc->getCurrentFontSize();
        if (ldoc->isShowSection(this))
        {
          last->prev->newPage = false;
          last->prev->newRow = true;
          last->prev->breakPage(font, color, curx, cury, firstcolumn);
        }

        last->newRow = true;
        last->newPage = false;
        last->breakPage(font, color, curx, cury, firstcolumn);
        curx += doc->getCurrentFontSize();
        break;
    }

    doc->popWeight();
  }

  last->prev->newPage = false;
  last->prev->newRow = false;
  last->newPage = false;
  last->newRow = false;
}

XWLTXDocumentParagraph::XWLTXDocumentParagraph(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentSectioning(LAparagraph,docA,"\\paragraph",parent)
{}

void XWLTXDocumentParagraph::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  doc->resetSubparagraphCounter();
  ldoc->setCurrentParagraph(this);
  ldoc->setCurrentSubparagraph(0);

  dir = doc->getDirection();

  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
  {
    if (doc->isInPreamble())
    {
      last->newRow = false;
      last->newPage = false;
      last->prev->newRow = false;
      last->prev->newPage = false;
      XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
    }
    else
    {
      last->newRow = true;
      doc->tiny();
      last->centered(curx,cury,firstcolumn);
      doc->popSize();
    }
  }
  else
  {
    doc->bfseries();
    setBoldFont();

    switch (dir)
    {
      case TEX_DOC_WD_TLT:
      case TEX_DOC_WD_TRT:
        cury += doc->getCurrentFontSize();
        break;

      case TEX_DOC_WD_RTT:
        curx -= doc->getCurrentFontSize();
        break;

      default:
        curx += doc->getCurrentFontSize();
        break;
    }

    countText->indent = 0;
    last->indent = 0;
    last->prev->indent = 0;

    if (ldoc->isShowSection(this))
    {
      last->prev->newPage = false;
      last->prev->newRow = true;
      last->prev->breakPage(font, color, curx, cury, firstcolumn);
    }

    last->newRow = true;
    last->newPage = false;
    last->breakPage(font, color, curx, cury, firstcolumn);

    switch (dir)
    {
      case TEX_DOC_WD_TLT:
      case TEX_DOC_WD_TRT:
        cury += doc->getCurrentFontSize();
        break;

      case TEX_DOC_WD_RTT:
        curx -= doc->getCurrentFontSize();
        break;

      default:
        curx += doc->getCurrentFontSize();
        break;
    }

    doc->popWeight();
  }

  last->prev->newPage = false;
  last->prev->newRow = false;
  last->newPage = false;
  last->newRow = false;
}

XWLTXDocumentSubparagraph::XWLTXDocumentSubparagraph(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentSectioning(LAparagraph,docA,"\\subparagraph",parent)
{}

void XWLTXDocumentSubparagraph::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  doc->resetSubparagraphCounter();
  ldoc->setCurrentSubparagraph(this);

  dir = doc->getDirection();

  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
  {
    if (doc->isInPreamble())
    {
      last->newRow = false;
      last->newPage = false;
      last->prev->newRow = false;
      last->prev->newPage = false;
      XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
    }
    else
    {
      last->newRow = true;
      doc->tiny();
      last->centered(curx,cury,firstcolumn);
      doc->popSize();
    }
  }
  else
  {
    doc->bfseries();
    setBoldFont();

    switch (dir)
    {
      case TEX_DOC_WD_TLT:
      case TEX_DOC_WD_TRT:
        cury += doc->getCurrentFontSize();
        break;

      case TEX_DOC_WD_RTT:
        curx -= doc->getCurrentFontSize();
        break;

      default:
        curx += doc->getCurrentFontSize();
        break;
    }

    countText->indent = 2.0;
    last->indent = 2.0;
    last->prev->indent = 2.0;

    if (ldoc->isShowSection(this))
    {
      last->prev->newPage = false;
      last->prev->newRow = true;
      last->prev->breakPage(font, color, curx, cury, firstcolumn);
    }

    last->newRow = true;
    last->newPage = false;
    last->breakPage(font, color, curx, cury, firstcolumn);

    switch (dir)
    {
      case TEX_DOC_WD_TLT:
      case TEX_DOC_WD_TRT:
        cury += doc->getCurrentFontSize();
        break;

      case TEX_DOC_WD_RTT:
        curx -= doc->getCurrentFontSize();
        break;

      default:
        curx += doc->getCurrentFontSize();
        break;
    }

    doc->popWeight();
  }

  last->prev->newPage = false;
  last->prev->newRow = false;
  last->newPage = false;
  last->newRow = false;
}

XWLTXDocumentPar::XWLTXDocumentPar(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentObject(LArealparagraph,docA,parent)
{
  newPage = false;
  newRow = true;
  firstIndent = 2.0;
  head = new XWTeXDocumentText(doc,this);
  last = head;
}

QString XWLTXDocumentPar::getMimeData()
{
  QString str;
  XWTeXDocumentObject * obj = head;
  while (obj)
  {
    QString tmp = obj->getMimeData();
    str += tmp;
    obj = obj->next;
  }

  str += "\n\n";
  return str;
}

bool XWLTXDocumentPar::isEmpty()
{
  if (head)
    return head == last && head->isEmpty();

  return true;
}

void XWLTXDocumentPar::scan(const QString & str, int & len, int & pos)
{
  del(head);

  XWTeXDocumentObject * obj = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('\\'))
    {
      int p = pos;
      QString key = scanControlSequence(str, len,pos);
      if (key == "par")
        return;

      int id = lookupLaTeXID(key);
      switch (id)
      {
        case LAmaketitle:
        case LAtableofcontents:
        case LAappendix:
        case LAend:
        case LAitem:
        case LApart:
        case LAchapter:
        case LAsection:
        case LAsubsection:
        case LAsubsubsection:
        case LAparagraph:
        case LAsubparagraph:
        case LAbibitem:
        case LAclosing:
        case LAprintindex:
        case LAprintglossary:
        case LAbibliographystyle:
        case LAbibliography:
        case LAcc:
        case LAencl:
        case LAps:
        case BMcolumn:
        case BMlecture:
        case BMagainframe:
        case BMmode:
        case BMAtBeginPart:
        case BMAtBeginLecture:
        case BMAtBeginNote:
        case BMAtEndNote:
        case BMnote:
          pos = p;
          return ;
          break;

        case LAbegin:
          {
            QString cs = key;
            int i = pos;
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == LAabstract ||
                id == LAletter ||
                id == LAthebibliography ||
                id == BMframe)
            {
              pos = p;
              return ;
            }

            pos = i;
            key = cs;
          }
          break;

        default:
          break;
      }

      obj = doc->createObject(key,str,pos, this);
      add(obj);
      obj->scan(str,len,pos);
    }
    else if (str[pos] == QChar('$'))
    {
      if (pos < (len - 1) && str[pos + 1] == QChar('$'))
        obj = new XWTeXDocumentDisplayFormular(doc, this);
      else
        obj = new XWTeXDocumentFormular(doc, this);
      add(obj);
      obj->scan(str,len,pos);
    }
    else if (str[pos] == QChar('%'))
    {
      obj = new XWTeXDocumentComment(doc,this);
      add(obj);
      obj->scan(str,len,pos);
    }
    else if (str[pos] == QChar('\n'))
    {
      pos++;
      if (pos >= len || str[pos] == QChar('\n'))
      {
        pos++;
        break;
      }
    }
    else
    {
      obj = new XWTeXDocumentText(doc, this);
      add(obj);
      obj->scan(str,len,pos);
    }
  }
}

void XWLTXDocumentPar::write(QTextStream & strm, int & linelen)
{
  if (linelen != 0)
  {
    strm << "\n";
    linelen = 0;
  }

  if (prev)
  {
    if (prev->type() == LApart ||
        prev->type() == LAchapter ||
        prev->type() == LAsection ||
        prev->type() == LAsubsection ||
        prev->type() == LAsubsubsection ||
        prev->type() == LAmaketitle ||
        prev->type() == LAtableofcontents ||
        prev->type() == BMframe ||
        prev->type() == BMmode ||
        prev->type() == BMnote ||
        prev->type() == BMlecture ||
        prev->type() == BMagainframe)
      strm << "\n";
  }

  XWTeXDocumentObject * obj = head;
  while (obj)
  {
    obj->write(strm, linelen);
    obj = obj->next;
  }

  if (linelen != 0)
  {
    strm << "\n";
    linelen = 0;
  }

  if (next)
  {
    if (next->type() == LArealparagraph ||
        next->type() == LApart ||
        next->type() == LAchapter ||
        next->type() == LAsection ||
        next->type() == LAsubsection ||
        next->type() == LAsubsubsection ||
        next->type() == LAparagraph ||
        next->type() == LAsubparagraph ||
        next->type() == LAinput ||
        next->type() == LAinclude ||
        next->type() == LAinclude ||
        next->type() == LAthebibliography ||
        next->type() == LAbibliographystyle ||
        next->type() == LAappendix ||
        next->type() == LAprintindex ||
        next->type() == LAprintglossary ||
        next->type() == BMframe ||
        next->type() == BMmode ||
        next->type() == BMnote ||
        next->type() == BMlecture ||
        next->type() == BMagainframe)
      strm << "\n";
  }
}

XWLTXDocumentQuote::XWLTXDocumentQuote(XWTeXDocument * docA, QObject * parent)
:XWLTXParEnv(LAquote,docA,"\\begin{quote}",parent)
{
  firstIndent = 0.0;
  add(new XWTeXControlSequence(docA, true, "\\end{quote}", this));
}

XWLTXDocumentQuotation::XWLTXDocumentQuotation(XWTeXDocument * docA, QObject * parent)
:XWLTXParEnv(LAquotation,docA,"\\begin{quotation}",parent)
{
  firstIndent = 2.0;
  add(new XWTeXControlSequence(docA, true, "\\end{quotation}", this));
}

XWLTXDocumentVerse::XWLTXDocumentVerse(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAverse,docA,"\\begin{verse}",parent)
{
  afterNewRow = true;
  add(new XWTeXDocumentBeamerSpec(docA,tr("action specification:"),this));
  add(new XWLTXRow(docA,this));
  add(new XWTeXControlSequence(docA, true, "\\end{verse}", this));
}

void XWLTXDocumentVerse::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && doc->isInPreamble())
  {
    XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
    return ;
  }

  double oldindent = indent;
  indent += 2.0;

  XWTeXDocumentObject * obj = head->next->next;
  while (obj)
  {
    obj->newRow = true;
    if (obj == last)
      break;
    obj->indent = indent;
    obj->centered(font, color, curx,cury,firstcolumn);
    obj = obj->next;
  }

  indent = oldindent;
}

void XWLTXDocumentVerse::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentObject * obj = head->next;
  obj->scan(str,len,pos);
  obj = obj->next;
  del(obj);
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else
    {
      if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        switch (id)
        {
          case LAend:
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == LAverse)
              return ;
            break;
        }

        pos = p;
      }

      obj = new XWLTXRow(doc,this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

XWLTXDocumentItem::XWLTXDocumentItem(XWTeXDocument * docA, QObject * p)
:XWTeXDocumentCommand(LAitem, docA, true,"\\item", p),
 mark(0)
{
  afterNewRow = true;
  add(new XWTeXDocumentBeamerSpec(docA,tr("alert specification:"),this));
  add(new XWLTXOption(docA,this));
  add(new XWTeXDocumentBeamerSpec(docA,tr("alert specification:"),this));
  add(new XWLTXDocumentPar(docA, this));
  mark = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, this);
}

void XWLTXDocumentItem::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);

  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && doc->isInPreamble())
  {
    XWTeXDocumentObject::breakPage(curx, cury, firstcolumn);
    return ;
  }

  double oldindent = indent;
  indent += 2.0;

  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
  switch (pobj->type())
  {
    case LAdescription:
      {
        doc->bfseries();
        setBoldFont();
        head->next->next->newRow = true;
        head->next->next->indent = indent;
        head->next->next->breakPage(font, color, curx, cury, firstcolumn);
        doc->popWeight();
        setNormalFont();
        XWTeXDocumentObject * obj = head->next->next->next->next;
        while (obj)
        {
          if (obj == head->next->next->next->next)
            obj->newRow = false;
          else
            obj->newRow = true;
          obj->indent = head->next->next->indent + 2;
          obj->breakPage(font, color, curx, cury,firstcolumn);
          obj = obj->next;
        }
      }
      break;

    case LAenumerate:
      {
        setNormalFont();
        int count = doc->getItemCount();

        switch (doc->getItemLevel())
        {
          case 1:
            mark->text = QString("%1 ").arg(count);
            break;

          case 2:
            mark->text = QString("(%1) ").arg((char)(count + 0x60));
            break;

          case 3:
            mark->text = getRomanNumber(count);
            mark->text += " ";
            break;

          default:
            mark->text = QString("%1 ").arg((char)(count + 0x40));
            break;
        }

        mark->indent = indent;
        mark->newRow = true;
        mark->breakPage(font, color, curx, cury, firstcolumn);
        XWTeXDocumentObject * obj = head->next->next->next->next;
        while (obj)
        {
          if (obj == head->next->next->next->next)
            obj->newRow = false;
          else
            obj->newRow = true;
          obj->indent = mark->indent + mark->text.length();
          obj->breakPage(curx, cury,firstcolumn);
          obj = obj->next;
        }
      }
      break;

    default:
      {
        setNormalFont();
        switch (doc->getItemLevel())
        {
          case 1:
            mark->text = QChar(0x2022);
            mark->text += " ";
            break;

          case 2:
            mark->text = "-- ";
            break;

          case 3:
            mark->text = "* ";
            break;

          default:
            mark->text = QChar(0xB7);
            mark->text += " ";
            break;
        }

        mark->indent = indent;
        mark->newRow = true;
        mark->breakPage(font, color, curx, cury, firstcolumn);
        XWTeXDocumentObject * obj = head->next->next->next->next;
        while (obj)
        {
          if (obj == head->next->next->next->next)
            obj->newRow = false;
          else
            obj->newRow = true;
          obj->indent = mark->indent + mark->text.length();
          obj->breakPage(curx, cury, firstcolumn);
          obj = obj->next;
        }
      }
      break;
  }

  indent = oldindent;
}

void XWLTXDocumentItem::clear()
{
  XWTeXDocumentObject * obj = head->next->next->next->next;
  while (obj)
  {
    XWTeXDocumentObject * tmp = obj->next;
    delete obj;
    obj = tmp;
  }
}

void XWLTXDocumentItem::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  head->next->next->next->next = sobj;
  sobj->prev = head->next->next->next;
  last = eobj;
  last->next = 0;
}

bool XWLTXDocumentItem::isAllSelected()
{
  return last->isAllSelected() && head->next->next->next->next->isAllSelected();
}

bool XWLTXDocumentItem::isAtEnd()
{
  return last->isAtEnd();
}

bool XWLTXDocumentItem::isAtStart()
{
  return head->next->next->next->next->isAtStart();
}

bool XWLTXDocumentItem::isEmpty()
{
  return head->next->next->next->next == last && head->next->next->next->next->isEmpty();
}

void XWLTXDocumentItem::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next->next->next;
  *eobj = last;
  head->next->next->next->next = 0;
  last = head->next->next->next;
}

void XWLTXDocumentItem::removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next->next->next;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWLTXDocumentItem::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentObject * obj = head->next;
  obj->scan(str,len,pos);
  obj = obj->next;
  obj->scan(str,len,pos);
  obj = obj->next;
  obj->scan(str,len,pos);
  obj = obj->next;
  del(obj);

  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else
    {
      if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        pos = p;
        switch (id)
        {
          case LAend:
          case LAitem:
            return ;
            break;

          default:
            break;
        }
      }

      obj = new XWLTXDocumentPar(doc, this);
      add(obj);
      obj->scan(str,len,pos);
    }
  }
}

void XWLTXDocumentItem::setCursorAtStart()
{
  head->next->next->next->next->setCursorAtStart();
}

XWLTXDocumentItemEnv::XWLTXDocumentItemEnv(int tA, XWTeXDocument * docA, const QString & nameA,QObject * parent)
:XWLTXEnviroment(tA, docA, nameA,parent)
{
  afterNewRow = true;
  add(new XWBeamerDefaultSpec(docA,tr("default overlay specification:"),this));
  add(new XWLTXDocumentItem(docA, this));
}

void XWLTXDocumentItemEnv::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  doc->resetItemCounter();
  doc->incItemLevel();
  dir = doc->getDirection();
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && doc->isInPreamble())
  {
    XWTeXDocumentObject::breakPage(curx,cury,firstcolumn);
    return ;
  }

  double oldindent = indent;
  indent += 2.0;

  XWTeXDocumentObject * obj = head->next->next;
  while (obj)
  {
    obj->newRow = true;
    if (obj == last)
      break;

    obj->indent = indent;
    obj->breakPage(curx,cury,firstcolumn);
    obj = obj->next;
  }

  doc->decItemLevel();
  indent = oldindent;
}

void XWLTXDocumentItemEnv::clear()
{
  XWTeXDocumentObject * obj = head->next->next;
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (type() != LAitemize && ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
    obj = obj->next;
  while (obj != last)
  {
    XWTeXDocumentObject * tmp = obj->next;
    delete obj;
    obj = tmp;
  }
}

void XWLTXDocumentItemEnv::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (type() != LAitemize && ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
  {
    sobj->prev = head->next->next;
    head->next->next->next = sobj;
    eobj->next = last;
    last->prev = eobj;    
  }
  else
  {
    sobj->prev = head->next;
    head->next->next = sobj;
    eobj->next = last;
    last->prev = eobj;     
  }  
}

bool XWLTXDocumentItemEnv::isAllSelected()
{
  XWTeXDocumentObject * obj = head->next->next;
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (type() != LAitemize && ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
    obj = obj->next;
  return obj->isAllSelected() && last->prev->isAllSelected();
}

bool XWLTXDocumentItemEnv::isAtEnd()
{
  return last->prev->isAtEnd();
}

bool XWLTXDocumentItemEnv::isAtStart()
{
  XWTeXDocumentObject * obj = head->next->next;
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (type() != LAitemize && ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
    obj = obj->next;

  return obj->isAtStart();
}

bool XWLTXDocumentItemEnv::isEmpty()
{
  XWTeXDocumentObject * obj = head->next->next;
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (type() != LAitemize && ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
    obj = obj->next;
  return obj == last->prev && obj->isEmpty();
}

void XWLTXDocumentItemEnv::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (type() != LAitemize && ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
  {
    *sobj = head->next->next->next;
    head->next->next->next = last;
    last->prev = head->next->next;
  }    
  else
  {
    *sobj = head->next->next;  
    head->next->next = last;
    last->prev = head->next;
  }    
  *eobj = last->prev;
}

void XWLTXDocumentItemEnv::removeChildrenTo(XWTeXDocumentObject**sobj, 
                                        XWTeXDocumentObject**eobj,
                                        XWTeXDocumentObject**obj)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (type() != LAitemize && ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
    *sobj = head->next->next->next;
  else
    *sobj = head->next->next;
  *obj = (*sobj)->prev;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWLTXDocumentItemEnv::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentObject * obj = head->next;
  obj->scan(str,len,pos);
  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (pobj->type() != LAitemize && ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
  {
    obj = obj->next;
    obj->scan(str,len,pos);
  }
  obj = obj->next;
  del(obj);
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else
    {
      if (str[pos] == QChar('\\'))
      {
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        switch (id)
        {
          case LAend:
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == type())
              return ;
            break;

          default:
            break;
        }
      }
      obj = new XWLTXDocumentItem(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

void XWLTXDocumentItemEnv::setCursorAtStart()
{
  XWTeXDocumentObject * obj = head->next->next;
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (type() != LAitemize && ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
    obj = obj->next;
  obj->setCursorAtStart();
}

XWLTXDocumentDescription::XWLTXDocumentDescription(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentItemEnv(LAdescription, docA, "\\begin{description}",parent)
{
  XWTeXDocumentObject * mobj = new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("long text:"), this);
  head->next->insertAtAfter(mobj);
  add(new XWTeXControlSequence(docA, true, "\\end{description}", this));
}

XWLTXDocumentEnumerate::XWLTXDocumentEnumerate(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentItemEnv(LAenumerate, docA, "\\begin{enumerate}",parent)
{
  XWTeXDocumentObject * mobj = new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("mini template:"), this);
  head->next->insertAtAfter(mobj);
  add(new XWTeXControlSequence(docA, true, "\\end{enumerate}", this));
}

XWLTXDocumentItemize::XWLTXDocumentItemize(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentItemEnv(LAitemize, docA, "\\begin{itemize}", parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{itemize}", this));
}

XWLTXDocumentArray::XWLTXDocumentArray(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAarray, docA, "\\begin{array}",parent)
{
  afterNewRow = true;
  add(new XWTeXDocumentParam(XW_LTX_PARAM,docA, tr("template:"), this));
  add(new XWLTXRow(docA, this));
  add(new XWTeXControlSequence(docA, true, "\\end{array}", this));
}

void XWLTXDocumentArray::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setMathFont();
  double oldindent = indent;
  indent += 2.0;

  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);

  head->next->newRow = false;
  head->next->indent = indent;
  head->next->breakPage(curx,cury,firstcolumn);

  XWTeXDocumentObject * obj = head->next->next;
  while (obj)
  {
    obj->newRow = true;
    if (obj == last)
      break;

    obj->indent = indent;
    obj->breakPage(font,color,curx,cury,firstcolumn);
    obj = obj->next;
  }

  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);

  indent = oldindent;
}

void XWLTXDocumentArray::clear()
{
  XWTeXDocumentObject * obj = head->next->next;
  while (obj != last)
  {
    XWTeXDocumentObject * tmp = obj->next;
    delete obj;
    obj = tmp;
  }
}

void XWLTXDocumentArray::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str,len,pos);
  XWTeXDocumentObject * obj = head->next->next;
  del(obj);
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
    {
      obj = new XWTeXDocumentComment(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
    else
    {
      if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        switch (id)
        {
          case LAend:
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == LAarray)
              return ;
            break;

          default:
            break;
        }

        pos = p;
      }

      obj = new XWLTXRow(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

void XWLTXDocumentArray::setFont()
{
  setMathFont();
}

XWLTXDocumentDisplayMath::XWLTXDocumentDisplayMath(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAdisplaymath, docA, "\\begin{displaymath}", parent)
{
  afterNewRow = true;
  add(new XWLTXCode(docA, tr("math:"), this));
  add(new XWTeXControlSequence(docA, true, "\\end{displaymath}", this));
}

void XWLTXDocumentDisplayMath::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setFont();
  double oldindent = indent;
  indent += 2.0;
  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  head->next->newRow = false;
  head->next->indent = indent;
  head->next->breakPage(font,color,curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

void XWLTXDocumentDisplayMath::setFont()
{
  setMathFont();
}

XWLTXDocumentEqnarray::XWLTXDocumentEqnarray(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAeqnarray, docA, "\\begin{eqnarray}",parent)
{
  afterNewRow = true;
  add(new XWLTXRow(docA, this));
  add(new XWTeXControlSequence(docA, true, "\\end{eqnarray}", this));
}

void XWLTXDocumentEqnarray::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setMathFont();
  double oldindent = indent;
  indent += 2.0;

  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);

  XWTeXDocumentObject * obj = head->next;
  while (obj)
  {
    obj->newRow = true;
    if (obj == last)
      break;

    obj->indent = indent;
    obj->breakPage(font,color,curx,cury,firstcolumn);
    obj = obj->next;
  }

  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

void XWLTXDocumentEqnarray::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentObject * obj = head->next;
  del(obj);
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
    {
      obj = new XWTeXDocumentComment(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
    else
    {
      if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        switch (id)
        {
          case LAend:
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == LAeqnarray)
              return ;
            break;

          default:
            break;
        }

        pos = p;
      }

      obj = new XWLTXRow(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

void XWLTXDocumentEqnarray::setFont()
{
  setMathFont();
}

XWLTXDocumentEqnarrayStar::XWLTXDocumentEqnarrayStar(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAeqnarrayStar, docA, "\\begin{eqnarray*}",parent)
{
  afterNewRow = true;
  add(new XWLTXRow(docA, this));
  add(new XWTeXControlSequence(docA, true, "\\end{eqnarray*}", this));
}

void XWLTXDocumentEqnarrayStar::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setMathFont();
  double oldindent = indent;
  indent += 2.0;

  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  XWTeXDocumentObject * obj = head->next;
  while (obj)
  {
    obj->newRow = true;
    if (obj == last)
      break;

    obj->indent = indent;
    obj->breakPage(font,color,curx,cury,firstcolumn);
    obj = obj->next;
  }

  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

void XWLTXDocumentEqnarrayStar::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentObject * obj = head->next;
  del(obj);
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
    {
      obj = new XWTeXDocumentComment(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
    else
    {
      if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        switch (id)
        {
          case LAend:
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == LAeqnarrayStar)
              return ;
            break;

          default:
            break;
        }

        pos = p;
      }

      obj = new XWLTXRow(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

void XWLTXDocumentEqnarrayStar::setFont()
{
  setMathFont();
}

XWLTXDocumentEquation::XWLTXDocumentEquation(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAequation, docA, "\\begin{equation}",parent)
{
  afterNewRow = true;
  add(new XWLTXCode(docA, tr("math:"), this));
  add(new XWTeXControlSequence(docA, true, "\\end{equation}", this));
  head->next->specialFont = false;
}

void XWLTXDocumentEquation::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setFont();
  double oldindent = indent;
  indent += 2.0;
  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  head->next->newRow = true;
  head->next->indent = indent;
  head->next->breakPage(font,color,curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

void XWLTXDocumentEquation::setFont()
{
  setMathFont();
}

XWLTXDocumentFloat::XWLTXDocumentFloat(int tA,XWTeXDocument * docA, const QString & nameA,QObject * parent)
:XWLTXEnviroment(tA, docA, nameA,parent)
{
  afterNewRow = true;
  add(new XWTeXDocumentOption(TEX_DOC_OPTION, docA,tr("placement:"), this));
}

void XWLTXDocumentFloat::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  double oldindent = indent;
  indent += 2.0;

  XWTeXDocumentObject * obj = head;
  while (obj)
  {
    obj->indent = indent;
    obj->firstIndent = 0;
    obj->newPage = false;
    if (obj == head->next)
      obj->newRow = false;
    else
      obj->newRow = true;
    obj->breakPage(curx,cury, firstcolumn);
    obj = obj->next;
  }
  indent = oldindent;
}

void XWLTXDocumentFloat::clear()
{
  XWTeXDocumentObject * obj = head->next->next;
  while (obj != last)
  {
    XWTeXDocumentObject * tmp = obj->next;
    delete obj;
    obj = tmp;
  }
}

void XWLTXDocumentFloat::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str,len,pos);
  XWTeXDocumentObject * obj = 0;
  QString key;
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
    {
     obj = new XWTeXDocumentComment(doc, this);
     last->insertAtBefore(obj);
     obj->scan(str,len,pos);
    }
    else
    {
      if (str[pos] == QChar('\\'))
      {
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        switch (id)
        {
          case LAend:
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == type())
              return ;
            break;

          default:
            break;
        }

        obj = doc->createObject(key, str, pos, this);
      }
      else
        obj = new XWLTXDocumentPar(doc, this);

      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

XWLTXDocumentFigure::XWLTXDocumentFigure(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentFloat(LAfigure,docA,"\\begin{figure}",parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{figure}", this));
}

XWLTXDocumentFigureStar::XWLTXDocumentFigureStar(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentFloat(LAfigureStar,docA,"\\begin{figure*}",parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{figure*}", this));
}

XWLTXDocumentAlign::XWLTXDocumentAlign(int tA, XWTeXDocument * docA, const QString & nameA,QObject * parent)
:XWLTXEnviroment(tA,docA,nameA,parent)
{
  afterNewRow = true;
  add(new XWLTXRow(docA,this));
}

void XWLTXDocumentAlign::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentObject * obj = head->next;
  del(obj);
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
    {
      obj = new XWTeXDocumentComment(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
    else
    {
      if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        switch (id)
        {
          case LAend:
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == type())
              return ;
            break;

          default:
            break;
        }

        pos = p;
      }

      obj = new XWLTXRow(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

XWLTXDocumentCenter::XWLTXDocumentCenter(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentAlign(LAcenter,docA, "\\begin{center}",parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{center}", this));
}

void XWLTXDocumentCenter::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();

  XWTeXDocumentObject * obj = head->next;
  while (obj)
  {
    obj->newRow = true;
    if (obj == last)
      break;
    obj->indent = indent;
    obj->specialFont = false;
    obj->centered(font, color, curx,cury,firstcolumn);
    obj = obj->next;
  }
}

XWLTXDocumentFlushLeft::XWLTXDocumentFlushLeft(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentAlign(LAflushleft,docA,"\\begin{flushleft}",parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{flushleft}", this));
}

void XWLTXDocumentFlushLeft::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();

  XWTeXDocumentObject * obj = head->next;
  while (obj)
  {
    obj->newRow = true;
    if (obj == last)
      break;
    obj->indent = indent;
    obj->specialFont = false;
    obj->breakPage(font, color, curx,cury,firstcolumn);
    obj = obj->next;
  }
}

XWLTXDocumentFlushRight::XWLTXDocumentFlushRight(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentAlign(LAflushright,docA,"\\begin{flushright}",parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{flushright}", this));
}

void XWLTXDocumentFlushRight::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();

  XWTeXDocumentObject * obj = head->next;
  while (obj)
  {
    obj->newRow = true;
    if (obj == last)
      break;
    obj->indent = indent;
    obj->specialFont = false;
    obj->flushBottom(font, color, curx,cury,firstcolumn);
    obj = obj->next;
  }
}

XWLTXDocumentList::XWLTXDocumentList(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAlist,docA,"\\begin{list}",parent)
{
  afterNewRow = true;
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("labeling:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("spacing:"), this));
  add(new XWLTXDocumentItem(docA,this));
  add(new XWTeXControlSequence(docA, true, "\\end{list}", this));
}

void XWLTXDocumentList::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str,len,pos);
  head->next->next->scan(str,len,pos);

  while (pos < len && str[pos].isSpace())
    pos++;

  XWTeXDocumentObject * obj = head->next->next->next;
  del(obj);
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
    {
      obj = new XWTeXDocumentComment(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
    else
    {
      if (str[pos] == QChar('\\'))
      {
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        switch (id)
        {
          case LAend:
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == LAlist)
              return ;
            break;

          default:
            break;
        }
      }

      obj = new XWLTXDocumentItem(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

XWLTXDocumentMath::XWLTXDocumentMath(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAdisplaymath, docA, "\\begin{math}", parent)
{
  add(new XWLTXCode(docA, tr("math:"), this));
  add(new XWTeXControlSequence(docA, true, "\\end{math}", this));
}

XWLTXDocumentMiniPage::XWLTXDocumentMiniPage(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAminipage,docA,"\\begin{minipage}", parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("position:"), this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("height:"), this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("inner-pos:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("width:"), this));
  add(new XWLTXCode(docA, tr("text:"), this));
  add(new XWTeXControlSequence(docA, true, "\\end{minipage}", this));
}

void XWLTXDocumentMiniPage::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  firstcolumn = true;
  doc->savedTwoColumn();
  double oldindent = indent;
  indent += 2.0;
  head->newPage = true;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  head->next->newRow = false;
  head->next->indent = indent;
  head->next->breakPage(curx,cury,firstcolumn);
  head->next->next->newRow = false;
  head->next->next->indent = indent;
  head->next->next->breakPage(curx,cury,firstcolumn);
  head->next->next->next->newRow = false;
  head->next->next->next->indent = indent;
  head->next->next->next->breakPage(curx,cury,firstcolumn);
  head->next->next->next->next->newRow = true;
  head->next->next->next->next->indent = indent;
  head->next->next->next->next->breakPage(curx,cury,firstcolumn);
  last->prev->newPage = false;
  last->prev->newRow = true;
  last->prev->indent = indent;
  last->prev->breakPage(curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
  doc->restoreTwoColumn();
  if (next)
    next->newPage = true;
}

XWLTXDocumentPicture::XWLTXDocumentPicture(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LApicture,docA,"\\begin{picture}", parent)
{
  add(new XWTeXCoord(XW_LTX_COORD,docA, tr("size:"), this));
  add(new XWTeXCoord(XW_LTX_COORD,docA, tr("offset:"), this));
  add(new XWLTXCode(docA, tr("command:"), this));
  add(new XWTeXControlSequence(docA, true, "\\end{picture}", this));
}

void XWLTXDocumentPicture::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  double oldindent = indent;
  indent += 2.0;

  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  head->next->newRow = false;
  head->next->indent = indent;
  head->next->breakPage(curx,cury,firstcolumn);
  head->next->next->newRow = false;
  head->next->next->indent = indent;
  head->next->next->breakPage(curx,cury,firstcolumn);
  head->next->next->next->newRow = true;
  head->next->next->next->indent = indent;
  head->next->next->next->breakPage(curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

XWLTXDocumentTabbing::XWLTXDocumentTabbing(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentAlign(LAtabbing,docA,"\\begin{tabbing}", parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{tabbing}", this));
}

void XWLTXDocumentTabbing::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  double oldindent = indent;
  indent += 2.0;

  XWTeXDocumentObject * obj = head;
  while (obj)
  {
    obj->newRow = true;
    obj->indent = indent;

    obj->firstIndent = 0;
    obj->newPage = false;
    obj->breakPage(curx,cury, firstcolumn);
    obj = obj->next;
  }
  indent = oldindent;
}

XWLTXDocumentTable::XWLTXDocumentTable(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentFloat(LAtable,docA, "\\begin{table}", parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{table}", this));
}

XWLTXDocumentTabular::XWLTXDocumentTabular(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAtabular,docA,"\\begin{tabular}", parent)
{
  afterNewRow = true;
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("pos:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("cols:"), this));
  add(new XWLTXRow(docA, this));
  add(new XWTeXControlSequence(docA, true, "\\end{tabular}", this));
}

void XWLTXDocumentTabular::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();

  double oldindent = indent;
  indent += 2.0;

  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  head->next->newRow = false;
  head->next->indent = indent;
  head->next->breakPage(curx,cury,firstcolumn);
  head->next->next->newRow = false;
  head->next->next->indent = indent;
  head->next->next->breakPage(curx,cury,firstcolumn);
  XWTeXDocumentObject * obj = head->next->next->next;
  while (obj)
  {
    obj->newRow = true;
    if (obj == last)
      break;
    obj->indent = indent;
    obj->breakPage(font,color,curx,cury,firstcolumn);
    obj = obj->next;
  }

  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

void XWLTXDocumentTabular::clear()
{
  XWTeXDocumentObject * obj = head->next->next->next;
  while (obj != last)
  {
    XWTeXDocumentObject * tmp = obj->next;
    delete obj;
    obj = tmp;
  }
}

void XWLTXDocumentTabular::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  sobj->prev = head;
  head->next = sobj;
  eobj->next = last;
  last->prev = eobj;   
}

void XWLTXDocumentTabular::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next;
  *eobj = last->prev;
  head->next = last;
  last->prev = head;
}

void XWLTXDocumentTabular::removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWLTXDocumentTabular::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str,len,pos);
  head->next->next->scan(str,len,pos);
  XWTeXDocumentObject * obj = head->next->next->next;
  del(obj);
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
    {
       obj = new XWTeXDocumentComment(doc, this);
       last->insertAtBefore(obj);
       obj->scan(str,len,pos);
    }
    else
    {
      if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        switch (id)
        {
          case LAend:
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == LAtabular)
              return ;
            break;

          default:
            break;
        }

        pos = p;
      }

      obj = new XWLTXRow(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

XWLTXDocumentTabularStar::XWLTXDocumentTabularStar(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAtabularStar,docA,"\\begin{tabular*}", parent)
{
  afterNewRow = true;
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("width:"), this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("pos:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("cols:"), this));
  add(new XWLTXRow(docA, this));
  add(new XWTeXControlSequence(docA, true, "\\end{tabular*}", this));
}

void XWLTXDocumentTabularStar::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();

  double oldindent = indent;
  indent += 2.0;
  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  head->next->newRow = false;
  head->next->indent = indent;
  head->next->breakPage(curx,cury,firstcolumn);
  head->next->next->newRow = false;
  head->next->next->indent = indent;
  head->next->next->breakPage(curx,cury,firstcolumn);
  head->next->next->next->newRow = false;
  head->next->next->next->indent = indent;
  head->next->next->next->breakPage(curx,cury,firstcolumn);
  XWTeXDocumentObject * obj = head->next->next->next->next;
  while (obj)
  {
    obj->newRow = true;
    if (obj == last)
      break;
    obj->specialFont = false;
    obj->indent = indent;
    obj->breakPage(font,color,curx,cury,firstcolumn);
    obj = obj->next;
  }

  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

void XWLTXDocumentTabularStar::clear()
{
  XWTeXDocumentObject * obj = head->next->next->next->next;
  while (obj != last)
  {
    XWTeXDocumentObject * tmp = obj->next;
    delete obj;
    obj = tmp;
  }
}

void XWLTXDocumentTabularStar::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  sobj->prev = head->next;
  head->next->next = sobj;
  eobj->next = last;
  last->prev = eobj;    
}

void XWLTXDocumentTabularStar::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next;
  *eobj = last->prev;
  head->next->next = last;
  last->prev = head->next;
}

void XWLTXDocumentTabularStar::removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWLTXDocumentTabularStar::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str,len,pos);
  head->next->next->scan(str,len,pos);
  head->next->next->next->scan(str,len,pos);
  XWTeXDocumentObject * obj = head->next->next->next->next;
  del(obj);
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
    {
       obj = new XWTeXDocumentComment(doc, this);
       last->insertAtBefore(obj);
       obj->scan(str,len,pos);
    }
    else
    {
      if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        switch (id)
        {
          case LAend:
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == LAtabularStar)
              return ;
            break;

          default:
            break;
        }

        pos = p;
      }

      obj = new XWLTXRow(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

XWLTXDocumentBibitem::XWLTXDocumentBibitem(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAbibitem,docA,true,"\\bibitem",parent)
{
  mark = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, this);
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("label:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("cite-key:"), this));
  add(new XWLTXDocumentPar(docA, this));
}

void XWLTXDocumentBibitem::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();

  int count = doc->getBibitemCount();
  mark->text = QString("[%1] ").arg(count);
  mark->newPage = false;
  mark->newRow = true;
  mark->specialFont = false;
  mark->breakPage(font, color,curx,cury,firstcolumn);

  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
  {
    head->next->newPage = false;
    head->next->newRow = false;
    head->next->breakPage(curx,cury,firstcolumn);
  }

  head->next->next->newPage = false;
  head->next->next->newRow = false;
  head->next->next->breakPage(curx,cury,firstcolumn);
  head->next->next->next->newPage = false;
  head->next->next->next->newRow = false;
  head->next->next->next->breakPage(curx,cury,firstcolumn);
  XWTeXDocumentObject * obj = head->next->next->next->next;
  while (obj)
  {
    obj->newPage = false;
    if (obj == head->next->next->next)
      obj->newRow = false;
    else
      obj->newRow = true;
    obj->breakPage(curx, cury, firstcolumn);
    obj = obj->next;
  }
}

void XWLTXDocumentBibitem::clear()
{
  XWTeXDocumentObject * obj = head->next->next->next->next;
  while (obj)
  {
    XWTeXDocumentObject * tmp = obj->next;
    delete obj;
    obj = tmp;
  }
}

void XWLTXDocumentBibitem::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  head->next->next->next->next = sobj;
  sobj->prev = head->next->next->next;
  last = eobj;
  last->next = 0;
}

void XWLTXDocumentBibitem::insertChildren(XWTeXDocumentObject*obj, 
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
    eobj->next = head->next->next->next->next;
    head->next->next->next->next->prev = eobj;
    head->next->next->next->next = sobj;
    sobj->prev = head->next->next->next;
  }
  else
  {
    sobj->prev = obj;
    eobj->next = obj->next;
    obj->next->prev = eobj;    
    obj->next = sobj;
  }  
}

bool XWLTXDocumentBibitem::isAtEnd()
{
  return last->isAtEnd();
}

bool XWLTXDocumentBibitem::isAtStart()
{
  return head->next->next->next->next->isAtStart();
}

void XWLTXDocumentBibitem::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next->next->next;
  *eobj = last;
  head->next->next->next->next = 0;
  last = head->next->next->next;
  last->next = 0;
}

void XWLTXDocumentBibitem::removeChildren(XWTeXDocumentObject**sobj, 
                                          XWTeXDocumentObject**eobj,
                                          XWTeXDocumentObject**obj)
{
  if (*sobj == *eobj)
  {
    *obj = (*sobj)->prev;
    (*sobj)->remove();
  }
  else
  {
    XWTeXDocumentObject * prevobj = (*sobj)->prev;
    *obj = prevobj;
    XWTeXDocumentObject * nextobj = (*eobj)->next;
    prevobj->next = nextobj;
    if (nextobj)
      nextobj->prev = prevobj;
    else
    {
      last = prevobj;
      last->next = 0;
    }
  }  
}

void XWLTXDocumentBibitem::removeChildrenFrom(XWTeXDocumentObject**sobj, 
                                              XWTeXDocumentObject**eobj,
                                              XWTeXDocumentObject**obj)
{
  *eobj = last;
  XWTeXDocumentObject * prevobj = (*sobj)->prev;
  *obj = prevobj;
  last = prevobj;
  last->next = 0;
}

void XWLTXDocumentBibitem::removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next->next->next;
  head->next->next->next->next = (*eobj)->next;
  if ((*eobj) == last)
  {
    last = head->next->next->next;
    last->next = 0;
  }    
  else
    (*eobj)->next->prev = head->next->next->next;  
}

void XWLTXDocumentBibitem::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str,len,pos);
  head->next->next->scan(str,len,pos);
  head->next->next->next->scan(str,len,pos);
  XWTeXDocumentObject * obj = head->next->next->next->next;
  del(obj);
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else
    {
      if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        pos = p;
        switch (id)
        {
          case LAend:
          case LAbibitem:
            return ;
            break;

          default:
            break;
        }
      }

      obj = new XWLTXDocumentPar(doc, this);
      add(obj);
      obj->scan(str,len,pos);
    }
  }
}

XWLTXDocumentThebibliography::XWLTXDocumentThebibliography(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAthebibliography,docA, "\\begin{thebibliography}",parent)
{
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("Thebibliography"), this);
  add(new XWTeXDocumentParam(TEX_DOC_OPTION,docA, tr("Widest label:"), this));
  add(new XWLTXDocumentBibitem(docA,this));
  add(new XWTeXControlSequence(docA, true, "\\end{thebibliography}", this));
}

void XWLTXDocumentThebibliography::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  firstcolumn = true;
  doc->savedTwoColumn();
  dir = doc->getDirection();
  doc->bfseries();
  doc->huge();
  setBoldFont();

  title->newPage = true;
  title->newRow = true;
  title->specialFont = false;
  title->breakPage(font, color,curx,cury, firstcolumn);

  switch (doc->getDirection())
  {
    case TEX_DOC_WD_TLT:
    case TEX_DOC_WD_TRT:
      cury += doc->getCurrentFontSize();
      break;

    case TEX_DOC_WD_RTT:
      curx -= doc->getCurrentFontSize();
      break;

    default:
      curx += doc->getCurrentFontSize();
      break;
  }

  doc->popSize();
  doc->large();
  setBoldFont();

  head->next->newPage = false;
  head->next->newRow = true;
  head->next->breakPage(font, color, curx,cury, firstcolumn);

  doc->popSize();
  doc->popWeight();

  XWTeXDocumentObject * obj = head->next->next;
  while (obj)
  {
    obj->newRow = true;
    if (obj == last)
      break;

    obj->breakPage(curx,cury, firstcolumn);
    obj = obj->next;
  }

  doc->restoreTwoColumn();
}

void XWLTXDocumentThebibliography::clear()
{
  XWTeXDocumentObject * obj = head->next->next;
  while (obj)
  {
    XWTeXDocumentObject * tmp = obj->next;
    delete obj;
    obj = tmp;
  }
}

void XWLTXDocumentThebibliography::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str,len,pos);
  XWTeXDocumentObject * obj = head->next->next;
  del(obj);
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else
    {
      if (str[pos] == QChar('\\'))
      {
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        switch (id)
        {
          case LAend:
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == LAthebibliography)
              return ;
            break;

          default:
            break;
        }
      }

      obj = new XWLTXDocumentBibitem(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

XWLTXDocumentTheoremEnv::XWLTXDocumentTheoremEnv(int tA, XWTeXDocument * docA, const QString & nameA,QObject * parent)
:XWLTXEnviroment(tA,docA,nameA,parent)
{
  afterNewRow = true;
  add(new XWTeXDocumentBeamerSpec(docA,tr("action specification:"),this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("additional text:"),this));
  add(new XWTeXDocumentBeamerSpec(docA,tr("action specification:"),this));
  add(new XWLTXDocumentPar(docA, this));
}

void XWLTXDocumentTheoremEnv::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  double oldindent = indent;
  indent += 2.0;
  dir = doc->getDirection();

  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && doc->isInPreamble())
  {
    head->next->next->next->next->newRow = true;
    XWTeXDocumentObject::breakPage(curx, cury, firstcolumn);
  }
  else
  {
    doc->bfseries();
    doc->large();
    setBoldFont();

    title->indent = indent;
    title->newRow = true;
    title->specialFont = false;
    title->breakPage(font, color, curx, cury, firstcolumn);

    doc->popSize();
    doc->setItalic(true);
    setItalicFont();

    XWTeXDocumentObject * obj = head->next->next->next->next;
    while (obj)
    {
      if (obj == last)
        break;

      obj->indent = indent;
      if (obj == head->next->next->next->next && (ldoc->getClassIndex() != XW_LTX_DOC_CLS_BEAMER))
      {
        obj->firstIndent = 0.0;
        obj->newRow = false;
      }
      else
      {
        obj->firstIndent = 2.0;
        obj->newRow = true;
      }

      obj->breakPage(font, color, curx, cury, firstcolumn);
      obj = obj->next;
    }

    doc->popItalic();
    doc->popWeight();
  }
  indent = oldindent;
}

void XWLTXDocumentTheoremEnv::clear()
{
  XWTeXDocumentObject * obj = head->next->next->next->next;
  while (obj)
  {
    XWTeXDocumentObject * tmp = obj->next;
    delete obj;
    obj = tmp;
  }
}

void XWLTXDocumentTheoremEnv::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  sobj->prev = head->next->next->next;
  head->next->next->next->next = sobj;
  eobj->next = last;
  last->prev = eobj;      
}

bool XWLTXDocumentTheoremEnv::isAtStart()
{
  return head->next->next->next->next->isAtStart();
}

void XWLTXDocumentTheoremEnv::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next->next->next;
  *eobj = last->prev;
  head->next->next->next->next = last;
  last->prev = head->next->next->next;
}

void XWLTXDocumentTheoremEnv::removeChildrenTo(XWTeXDocumentObject**sobj, 
                                        XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next->next->next;
  head->next->next->next->next = (*eobj)->next;
  (*eobj)->next->prev = head->next->next->next;
}

void XWLTXDocumentTheoremEnv::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  XWTeXDocumentObject * obj = head->next;
  obj->scan(str,len,pos);
  obj = obj->next;
  obj->scan(str,len,pos);
  obj = obj->next;
  obj->scan(str,len,pos);
  obj = obj->next;
  del(obj);
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else
    {
      if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        switch (id)
        {
          case LAend:
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == type())
              return ;
            break;

          default:
            break;
        }

        pos = p;
      }

      obj = new XWLTXDocumentPar(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

XWLTXDocumentTheorem::XWLTXDocumentTheorem(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentTheoremEnv(LAtheorem,docA,"\\begin{theorem}", parent)
{
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("Theorem"), this);
  add(new XWTeXControlSequence(docA, true, "\\end{theorem}", this));
}

XWLTXDocumentProof::XWLTXDocumentProof(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentTheoremEnv(LAproof,docA,"\\begin{proof}", parent)
{
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("Proof"), this);
  add(new XWTeXControlSequence(docA, true, "\\end{proof}", this));
}

XWLTXDocumentAssumption::XWLTXDocumentAssumption(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentTheoremEnv(LAassumption,docA,"\\begin{assumption}", parent)
{
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("Assumption"), this);
  add(new XWTeXControlSequence(docA, true, "\\end{assumption}", this));
}

XWLTXDocumentDefinition::XWLTXDocumentDefinition(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentTheoremEnv(LAdefinition,docA,"\\begin{definition}", parent)
{
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("Definition"), this);
  add(new XWTeXControlSequence(docA, true, "\\end{definition}", this));
}

XWLTXDocumentProposition::XWLTXDocumentProposition(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentTheoremEnv(LAproposition,docA,"\\begin{proposition}", parent)
{
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("Proposition"), this);
  add(new XWTeXControlSequence(docA, true, "\\end{proposition}", this));
}

XWLTXDocumentLemma::XWLTXDocumentLemma(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentTheoremEnv(LAlemma,docA,"\\begin{lemma}", parent)
{
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("Lemma"), this);
  add(new XWTeXControlSequence(docA, true, "\\end{lemma}", this));
}

XWLTXDocumentAxiom::XWLTXDocumentAxiom(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentTheoremEnv(LAaxiom,docA,"\\begin{axiom}", parent)
{
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("Axiom"), this);
  add(new XWTeXControlSequence(docA, true, "\\end{axiom}", this));
}

XWLTXDocumentCorollary::XWLTXDocumentCorollary(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentTheoremEnv(LAcorollary,docA,"\\begin{corollary}", parent)
{
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("Corollary"), this);
  add(new XWTeXControlSequence(docA, true, "\\end{corollary}", this));
}

XWLTXDocumentConjecture::XWLTXDocumentConjecture(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentTheoremEnv(LAconjecture,docA,"\\begin{conjecture}", parent)
{
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("Conjecture"), this);
  add(new XWTeXControlSequence(docA, true, "\\end{conjecture}", this));
}

XWLTXDocumentExercise::XWLTXDocumentExercise(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentTheoremEnv(LAexercise,docA,"\\begin{exercise}", parent)
{
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("Exercise"), this);
  add(new XWTeXControlSequence(docA, true, "\\end{exercise}", this));
}

XWLTXDocumentExample::XWLTXDocumentExample(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentTheoremEnv(LAexample,docA,"\\begin{example}", parent)
{
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("Example"), this);
  add(new XWTeXControlSequence(docA, true, "\\end{example}", this));
}

XWLTXDocumentRemark::XWLTXDocumentRemark(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentTheoremEnv(LAremark,docA,"\\begin{remark}", parent)
{
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("Remark"), this);
  add(new XWTeXControlSequence(docA, true, "\\end{remark}", this));
}

XWLTXDocumentProblem::XWLTXDocumentProblem(XWTeXDocument * docA, QObject * parent)
:XWLTXDocumentTheoremEnv(LAproblem,docA,"\\begin{problem}", parent)
{
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("Problem"), this);
  add(new XWTeXControlSequence(docA, true, "\\end{problem}", this));
}

XWLTXDocumentVerbatim::XWLTXDocumentVerbatim(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAverbatim, docA, "\\begin{verbatim}", parent)
{
  add(new XWLTXCode(docA, tr("literal-text:"), this));
  add(new XWTeXControlSequence(docA, true, "\\end{verbatim}", this));
}

void XWLTXDocumentVerbatim::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setVerbFont();
  double oldindent = indent;
  indent += 2.0;
  head->newPage = false;
  head->newRow = true;
  head->indent = indent;
  head->breakPage(curx,cury,firstcolumn);
  head->next->newRow = true;
  head->next->indent = indent;
  head->next->breakPage(font,color,curx,cury,firstcolumn);
  last->newPage = false;
  last->newRow = true;
  last->indent = indent;
  last->breakPage(curx,cury,firstcolumn);
  indent = oldindent;
}

XWLTXDocumentVerb::XWLTXDocumentVerb(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentBlock(LAverb,docA,false, "\\verb",parent)
{
  isOnlyRead = false;
}

void XWLTXDocumentVerb::scan(const QString & str, int & len, int & pos)
{
  if (str[pos] == QChar('*'))
  {
    text.append(str[pos]);
    pos++;
  }

  QChar c = str[pos];
  while (pos < len)
  {
    if (str[pos] == c)
    {
      pos++;
      break;
    }
    text.append(str[pos]);
    pos++;
  }
}

void XWLTXDocumentVerb::setFont()
{
  setVerbFont();
}

XWLTXDocumentFootnote::XWLTXDocumentFootnote(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAfootnote, docA, false, "\\footnote",parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("number:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("note:"), this));
}

void XWLTXDocumentFootnote::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  dir = doc->getDirection();
  doc->footnoteSize();
  setNormalFont();

  head->newPage = false;
  head->newRow = false;
  head->breakPage(curx,cury,firstcolumn);

  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
  {
    head->next->newPage = false;
    head->next->newRow = false;
    head->next->breakPage(curx,cury,firstcolumn);
  }

  last->prev->newPage = false;
  last->prev->newRow = false;
  last->prev->breakPage(curx,cury,firstcolumn);

  last->newPage = false;
  last->newRow = false;
  last->breakPage(font, color, curx,cury,firstcolumn);

  doc->popSize();
}

XWLTXDocumentMarginpar::XWLTXDocumentMarginpar(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAmarginpar, docA, false, "\\marginpar",parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("left:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("right:"), this));
}

void XWLTXDocumentMarginpar::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  doc->footnoteSize();
  setNormalFont();

  head->newPage = false;
  head->newRow = false;
  head->breakPage(curx,cury,firstcolumn);

  last->prev->newPage = false;
  last->prev->newRow = false;
  last->prev->breakPage(curx,cury,firstcolumn);

  last->newPage = false;
  last->newRow = false;
  last->breakPage(font, color, curx,cury,firstcolumn);

  doc->popSize();
}

XWLTXDocumentCaption::XWLTXDocumentCaption(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAcaption, docA, true,"\\caption", parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION, docA,tr("loftitle: "), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("text:"), this));
}

XWLTXDocumentLabel::XWLTXDocumentLabel(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAlabel, docA, false,"\\label", parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("key:"), this));
}

void XWLTXDocumentLabel::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  head->breakPage(curx,cury,firstcolumn);
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
    head->next->breakPage(curx,cury,firstcolumn);

  head->next->next->breakPage(curx,cury,firstcolumn);
}

XWLTXDocumentPageref::XWLTXDocumentPageref(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LApageref, docA, false, "\\pageref", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("key:"), this));
}

XWLTXDocumentRef::XWLTXDocumentRef(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAref, docA, false,"\\ref", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("key:"), this));
}

XWLTXDocumentIndex::XWLTXDocumentIndex(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAindex, docA, false,"\\index", parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION, docA,tr("name:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("entry:"), this));
}

XWLTXDocumentGlossary::XWLTXDocumentGlossary(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAglossary, docA, false,"\\glossary", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("text:"), this));
}

XWLTXDocumentCite::XWLTXDocumentCite(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAcite, docA, false,"\\cite", parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION, docA,tr("subcite:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("keys:"), this));
}

XWLTXDocumentNocite::XWLTXDocumentNocite(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAnocite, docA, false,"\\nocite", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("keys:"), this));
}

XWLTXDocumentBibliographystyle::XWLTXDocumentBibliographystyle(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAbibliographystyle, docA, true, "\\bibliographystyle", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("bibstyle:"), this));
}

XWLTXDocumentBibliography::XWLTXDocumentBibliography(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAbibliography, docA, true,"\\bibliography", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("files:"), this));
}

XWLTXDocumentRows::XWLTXDocumentRows(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentObject(XW_LTX_ROWS,docA,parent)
{
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"{",this));
  add(new XWLTXRow(docA,this));
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"}",this));
  lineEndObj = new XWTeXControlSequence(LANewLine,docA, " \\\\ ",this);
}

void XWLTXDocumentRows::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();

  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(parent());
  if (pobj->type() == LAaddress || pobj->type() == LAlocation)
  {
    if (doc->isInPreamble())
    {
      XWTeXDocumentObject * obj = head;
      while (obj)
      {
        obj->newRow = false;
        obj->indent = indent;
        obj->breakPage(font,color,curx,cury,firstcolumn);
        if (obj != last->prev)
           lineEndObj->breakPage(font,color,curx,cury,firstcolumn);
        obj = obj->next;
      }
    }
    else
    {
      XWTeXDocumentObject * obj = head->next;
      doc->savedTwoColumn();
      while (obj)
      {
        if (obj == last)
          break;

        obj->newRow = true;
        obj->specialFont = false;
        obj->indent = indent;
        obj->flushBottom(font,color, curx,cury,firstcolumn);
        obj = obj->next;
      }

      switch (dir)
      {
        case TEX_DOC_WD_TLT:
        case TEX_DOC_WD_TRT:
          cury += doc->getCurrentFontSize();
          doc->setSavedTop(cury);
          break;

        case TEX_DOC_WD_RTT:
          curx -= doc->getCurrentFontSize();
          doc->setSavedTop(curx);
          break;

        default:
          curx += doc->getCurrentFontSize();
          doc->setSavedTop(curx);
          break;
      }

      doc->restoreTwoColumn();
    }
  }
  else
  {
    XWTeXDocumentObject * obj = head->next;
    while (obj)
    {
      if (obj == last)
        break;

      obj->newRow = true;
      obj->specialFont = false;
      obj->indent = indent;
      obj->breakPage(font,color, curx,cury,firstcolumn);
      obj = obj->next;
    }
  }
}

QString XWLTXDocumentRows::getMimeData()
{
  QString str = head->getMimeData();
  XWTeXDocumentObject * obj = head->next;
  while (obj)
  {
    if (obj == last)
      break;

    XWLTXRow * row = (XWLTXRow*)(obj);
    str += row->text;
    if (obj != last->prev)
      str += lineEndObj->text;
  }
  QString tmp = last->getMimeData();
  str += tmp;
  return str;
}

bool XWLTXDocumentRows::isEmpty()
{
  return head->next == last->prev && head->next->isEmpty();
}

void XWLTXDocumentRows::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (pos >= len || str[pos] != QChar('{'))
    return ;

  pos++;
  XWTeXDocumentObject * obj = head->next;
  del(obj);
  while (pos < len)
  {
    if (str[pos].isSpace())
     pos++;
    else
    {
      if (str[pos] == QChar('}'))
      {
        pos++;
        break;
      }

      obj = new XWLTXRow(doc,this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

void XWLTXDocumentRows::write(QTextStream & strm, int & linelen)
{
  QString str = getMimeData();
  strm << str;
  linelen += str.length();
}

XWLTXDocumentAddress::XWLTXDocumentAddress(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAaddress,docA,true, "\\address",parent)
{
  add(new XWLTXDocumentRows(docA, this));
}

void XWLTXDocumentAddress::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  if (doc->isInPreamble())
    head->breakPage(curx,cury,firstcolumn);

  head->next->breakPage(curx,cury,firstcolumn);
}

XWLTXDocumentLocation::XWLTXDocumentLocation(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAlocation,docA,true, "\\location", parent)
{
  add(new XWLTXDocumentRows(docA, this));
}

void XWLTXDocumentLocation::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  if (doc->isInPreamble())
    head->breakPage(curx,cury,firstcolumn);

  head->next->breakPage(curx,cury,firstcolumn);
}

XWLTXDocumentTelephone::XWLTXDocumentTelephone(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAtelephone,docA,true, "\\telephone",parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("your telephone number:"), this));
}

void XWLTXDocumentTelephone::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();
  if (doc->isInPreamble())
  {
    head->next->newRow = false;
    head->breakPage(curx,cury,firstcolumn);
    head->next->breakPage(curx,cury,firstcolumn);
  }
  else
  {
    doc->savedTwoColumn();
    head->next->head->next->newRow = true;
    head->next->head->next->flushBottom(font,color, curx,cury,firstcolumn);
    switch (dir)
    {
      case TEX_DOC_WD_TLT:
      case TEX_DOC_WD_TRT:
        cury += doc->getCurrentFontSize();
        doc->setSavedTop(cury);
        break;

      case TEX_DOC_WD_RTT:
        curx -= doc->getCurrentFontSize();
        doc->setSavedTop(curx);
        break;

      default:
        curx += doc->getCurrentFontSize();
        doc->setSavedTop(curx);
        break;
    }
    head->next->head->next->newRow = false;
    doc->restoreTwoColumn();
  }
}

XWLTXDocumentSignature::XWLTXDocumentSignature(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAsignature,docA,true, "\\signature",parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("your name:"), this));
}

void XWLTXDocumentSignature::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();
  if (doc->isInPreamble())
  {
    head->next->newRow = false;
    head->breakPage(curx,cury,firstcolumn);
    head->next->breakPage(curx,cury,firstcolumn);
  }
  else
  {
    firstcolumn = true;
    head->next->head->next->newRow = true;
    head->next->head->next->flushBottom(font,color, curx,cury,firstcolumn);
    head->next->head->next->newRow = false;
  }
}

XWLTXDocumentOpening::XWLTXDocumentOpening(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAopening,docA,true, "\\opening",parent)
{
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"{",this));
  add(new XWTeXDocumentText(docA,this));
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"}",this));
}

void XWLTXDocumentOpening::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();

  last->prev->newRow = true;
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
    case TEX_DOC_WD_TRT:
      cury += doc->getCurrentFontSize();
      break;

    case TEX_DOC_WD_RTT:
      curx -= doc->getCurrentFontSize();
      break;

    default:
      curx += doc->getCurrentFontSize();
      break;
  }

  last->prev->breakPage(font, color,curx,cury,firstcolumn);
  last->prev->newRow = false;
}

void XWLTXDocumentOpening::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentText * txt = (XWTeXDocumentText*)(last->prev);
  txt->scanParam(str,len,pos);
}

void XWLTXDocumentOpening::write(QTextStream & strm, int & linelen)
{
  if (linelen != 0)
  {
    strm << "\n";
    linelen = 0;
  }

  QString str = getMimeData();
  strm << str;
  strm << "\n";
}

XWLTXDocumentName::XWLTXDocumentName(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(LAname, docA, true, "\\name",parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("your name for envelope:"), this));
}

void XWLTXDocumentName::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();
  if (doc->isInPreamble())
  {
    head->next->newRow = false;
    head->breakPage(curx,cury,firstcolumn);
    head->next->breakPage(curx,cury,firstcolumn);
  }
  else
  {
    doc->savedTwoColumn();
    XWTeXDocumentObject * obj = head->next->head->next;
    obj->newRow = true;
    obj->flushBottom(font,color, curx,cury,firstcolumn);
    obj->newRow = false;
    switch (dir)
    {
      case TEX_DOC_WD_TLT:
      case TEX_DOC_WD_TRT:
        cury += doc->getCurrentFontSize();
        doc->setSavedTop(cury);
        break;

      case TEX_DOC_WD_RTT:
        curx -= doc->getCurrentFontSize();
        doc->setSavedTop(curx);
        break;

      default:
        curx += doc->getCurrentFontSize();
        doc->setSavedTop(curx);
        break;
    }
    doc->restoreTwoColumn();
  }
}

XWLTXDocumentMakelabels::XWLTXDocumentMakelabels(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(LAmakelabels, docA, true, "\\makelabels", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("number:"), this));
}

XWLTXDocumentEncl::XWLTXDocumentEncl(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAencl,docA,true,"\\encl",parent)
{
  add(new XWLTXDocumentRows(docA,this));
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("encl: "), this);
}

void XWLTXDocumentEncl::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();

  head->next->newPage = false;
  head->next->newRow = false;
  head->next->indent = title->text.length();

  title->newRow = true;

  switch (dir)
  {
    case TEX_DOC_WD_TLT:
    case TEX_DOC_WD_TRT:
      cury += doc->getCurrentFontSize();
      break;

    case TEX_DOC_WD_RTT:
      curx -= doc->getCurrentFontSize();
      break;

    default:
      curx += doc->getCurrentFontSize();
      break;
  }

  title->breakPage(font,color,curx,cury,firstcolumn);
  head->next->breakPage(curx,cury,firstcolumn);
}

XWLTXDocumentClosing::XWLTXDocumentClosing(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAclosing,docA,true, "\\closing", parent)
{
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"{",this));
  add(new XWTeXDocumentText(docA,this));
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,"}",this));
}

void XWLTXDocumentClosing::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();

  switch (dir)
  {
    case TEX_DOC_WD_TLT:
    case TEX_DOC_WD_TRT:
      cury += doc->getCurrentFontSize();
      break;

    case TEX_DOC_WD_RTT:
      curx -= doc->getCurrentFontSize();
      break;

    default:
      curx += doc->getCurrentFontSize();
      break;
  }

  last->prev->newRow = true;
  last->prev->centered(font, color, curx,cury,firstcolumn);
}

void XWLTXDocumentClosing::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentText * txt = (XWTeXDocumentText*)(last->prev);
  txt->scanParam(str,len,pos);
}

void XWLTXDocumentClosing::write(QTextStream & strm, int & linelen)
{
  if (linelen != 0)
  {
    strm << "\n";
    linelen = 0;
  }

  QString str = getMimeData();
  strm << str;
  strm << "\n";
}

XWLTXDocumentCc::XWLTXDocumentCc(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAcc,docA,true,"\\cc",parent)
{
  add(new XWLTXDocumentRows(docA,this));
  title = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true, tr("cc: "), this);
}

void XWLTXDocumentCc::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setNormalFont();

  head->next->newPage = false;
  head->next->newRow = false;
  head->next->indent = title->text.length();

  title->newRow = true;
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
    case TEX_DOC_WD_TRT:
      cury += doc->getCurrentFontSize();
      break;

    case TEX_DOC_WD_RTT:
      curx -= doc->getCurrentFontSize();
      break;

    default:
      curx += doc->getCurrentFontSize();
      break;
  }

  title->breakPage(font,color,curx,cury,firstcolumn);
  head->next->breakPage(curx,cury,firstcolumn);
}

XWLTXDocumentLetter::XWLTXDocumentLetter(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(LAletter,docA,"\\begin{letter}",parent),
 closing(0),
 ps(0),
 cc(0),
 encl(0)
{
  add(new XWLTXDocumentRows(docA,this));
  add(new XWLTXDocumentOpening(docA,this));
  add(new XWLTXDocumentPar(docA,this));
  add(new XWLTXDocumentClosing(docA,this));
  add(new XWTeXControlSequence(docA, true, "\\end{letter}", this));
}

void XWLTXDocumentLetter::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();

  XWTeXDocumentObject * obj = head->next;
  while (obj && obj != last)
  {
    obj->newRow = true;
    switch (obj->type())
    {
      case LAclosing:
        closing = obj;
        break;

      case LAencl:
        encl = obj;
        break;

      case LAcc:
        cc = obj;
        break;

      case LAps:
        ps = obj;
        break;

      default:
        obj->breakPage(curx,cury, firstcolumn);
        break;
    }

    obj = obj->next;
  }

  if (closing)
    closing->breakPage(curx,cury, firstcolumn);

  if (!doc->isInPreamble())
  {
    XWLaTeXDocument * ldoc = (XWLaTeXDocument*)doc;
    XWLTXDocumentSignature * s = ldoc->getSignature();
    if (s)
      s->breakPage(curx,cury,firstcolumn);

    if (dir == TEX_DOC_WD_RTT || dir == TEX_DOC_WD_LTL)
    {
      XWLTXDocumentDate * d = ldoc->getDate();
      if (d)
        d->breakPage(curx,cury,firstcolumn);
    }
  }

  if (ps)
    ps->breakPage(curx,cury, firstcolumn);

  if (encl)
    encl->breakPage(curx,cury, firstcolumn);

  if (cc)
    cc->breakPage(curx,cury, firstcolumn);
}

void XWLTXDocumentLetter::scan(const QString & str, int & len, int & pos)
{
  head->next->scan(str,len,pos);

  XWTeXDocumentObject * obj = head->next->next;
  XWTeXDocumentObject * nobj = obj->next;
  del(obj);
  obj = nobj;
  nobj = obj->next;
  del(obj);
  obj = nobj;
  nobj = obj->next;
  del(obj);
  closing = 0;
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
    {
      obj = new XWTeXDocumentComment(doc, this);
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
    else
    {
      if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        switch (id)
        {
          case LAend:
            key = scanEnviromentName(str,len,pos);
            id = lookupLaTeXID(key);
            if (id == LAletter)
              return ;
            pos = p;
            obj = new XWLTXDocumentPar(doc,this);//不可能
            break;

          case LAopening:
            obj = createLaTeXObject(id, doc,this);
            break;

          case LAclosing:
            obj = createLaTeXObject(id, doc,this);
            closing = obj;
            break;

          case LAencl:
            obj = createLaTeXObject(id, doc,this);
            encl = obj;
            break;

          case LAcc:
            obj = createLaTeXObject(id, doc,this);
            cc = obj;
            break;

          case LAps:
            obj = createLaTeXObject(id, doc,this);
            ps = obj;
            break;

          default:
            pos = p;
            obj = new XWLTXDocumentPar(doc,this);
            break;
        }
      }
      else
        obj = new XWLTXDocumentPar(doc,this);

      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

void XWLTXDocumentLetter::setCursor()
{
  head->next->setCursor();
}

XWLTXDocumentIncludegraphics::XWLTXDocumentIncludegraphics(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(LAincludegraphics, docA, false,"\\includegraphics", parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("options:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("file name:"), this));
}

void XWLTXDocumentIncludegraphics::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  head->breakPage(curx,cury,firstcolumn);
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER)
    head->next->breakPage(curx,cury,firstcolumn);
  head->next->next->breakPage(curx,cury,firstcolumn);
  head->next->next->next->breakPage(curx,cury,firstcolumn);
}
