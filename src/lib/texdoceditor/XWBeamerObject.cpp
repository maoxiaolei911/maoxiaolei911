/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
 #include "BeamerKeyWord.h"
 #include "XWBeamerPool.h"
 #include "XWTeXDocumentPage.h"
 #include "XWLaTeXDocument.h"
 #include "XWLaTeXDocumentObject.h"
 #include "XWBeamerObject.h"

XWTeXDocumentObject * createBeamerObject(int t, XWTeXDocument * docA, QObject * parent)
{
  XWTeXDocumentObject * obj = 0;
  switch (t)
  {
    case BMsubtitle:
      obj = new XWBeamerSubtitle(docA,parent);
      break;

    case BMinstitute:
      obj = new XWBeamerInstitute(docA,parent);
      break;

    case BMtitlegraphic:
      obj = new XWBeamerTitleGraphic(docA,parent);
      break;

    case BMsubject:
      obj = new XWBeamerSubject(docA,parent);
      break;

    case BMkeywords:
      obj = new XWBeamerKeywords(docA,parent);
      break;

    case BMlogo:
      obj = new XWBeamerLogo(docA,parent);
      break;

    case BMsetbeameroption:
      obj = new XWTeXParamCommand(BMsetbeameroption,docA,true,"\\setbeameroption","options:",parent);
      break;

    case BMsetjobnamebeamerversion:
      obj = new XWTeXParamCommand(BMsetjobnamebeamerversion,docA,true,"\\setjobnamebeamerversion","options:",parent);
      break;

    case BMsetbeamersize:
      obj = new XWTeXParamCommand(BMsetbeamersize,docA,true,"\\setbeamersize","options:",parent);
      break;

    case BMsetbeamercovered:
      obj = new XWTeXParamCommand(BMsetbeamercovered,docA,true,"\\setbeamercovered","options:",parent);
      break;

    case BMusetheme:
      obj = new XWTeXOptionParamCommand(BMusetheme,docA,"\\usetheme","options:","name list:",parent);
      break;

    case BMusecolortheme:
      obj = new XWTeXOptionParamCommand(BMusecolortheme,docA,"\\usecolortheme","options:","name list:",parent);
      break;

    case BMusefonttheme:
      obj = new XWTeXOptionParamCommand(BMusefonttheme,docA,"\\usefonttheme","options:","name:",parent);
      break;

    case BMuseinnertheme:
      obj = new XWTeXOptionParamCommand(BMuseinnertheme,docA,"\\useinnertheme","options:","name:",parent);
      break;

    case BMuseoutertheme:
      obj = new XWTeXOptionParamCommand(BMuseoutertheme,docA,"\\useoutertheme","options:","name:",parent);
      break;

    case BMincludeslide:
      obj = new XWTeXOptionParamCommand(BMincludeslide,docA,"\\includeslide","options:","label name:",parent);
      break;

    case BMincludeonlylecture:
      obj = new XWBeamerIncludeOnlyLecture(docA,parent);
      break;

    case BMusebeamertemplate:
      obj = new XWUseBeamerTemplate(docA,parent);
      break;

    case BMexpandbeamertemplate:
      obj = new XWTeXParamCommand(BMexpandbeamertemplate,docA,true,"\\expandbeamertemplate","beamer template name:",parent);
      break;

    case BMsetbeamertemplate:
      obj = new XWSetBeamerTemplate(docA,parent);
      break;

    case BMifbeamercolorempty:
      obj = new XWIfBeamerColorEmpty(docA,parent);
      break;

    case BMusebeamercolor:
      obj = new XWUseBeamerColor(docA,parent);
      break;

    case BMsetbeamercolor:
      obj = new XWSetBeamerColor(docA,parent);
      break;

    case BMsetbeamerfont:
      obj = new XWSetBeamerFont(docA,parent);
      break;

    case BMusebeamerfont:
      obj = new XWUseBeamerFont(docA,parent);
      break;

    case BMlecture:
      obj = new XWBeamerLecture(docA,parent);
      break;

    case BMnote:
      obj = new XWBeamerNode(docA,parent);
      break;

    case BMmode:
      obj = new XWBeamerMode(docA,parent);
      break;

    case BMAtBeginPart:
      obj = new XWTeXParamCommand(BMAtBeginPart,docA,true,"\\AtBeginPart","text:",parent);
      break;

    case BMAtBeginLecture:
      obj = new XWTeXParamCommand(BMAtBeginLecture,docA,true,"\\AtBeginLecture","text:",parent);
      break;

    case BMAtBeginNote:
      obj = new XWTeXParamCommand(BMAtBeginNote,docA,true,"\\AtBeginNote","text:",parent);
      break;

    case BMAtEndNote:
      obj = new XWTeXParamCommand(BMAtEndNote,docA,true,"\\AtEndNote","text:",parent);
      break;

    case BMagainframe:
      obj = new XWBeamerAgainFrame(docA,parent);
      break;

    case BMframe:
      obj = new XWBeamerFrameEnv(docA,parent);
      break;

    case BMframetitle:
      obj = new XWBeamerFrameTitle(docA,parent);
      break;

    case BMframesubtitle:
      obj = new XWBeamerFrameSubitle(docA,parent);
      break;

    case BMframezoom:
      obj = new XWBeamerFrameZoom(docA,parent);
      break;

    case BMtitlepage:
      obj = new XWBeamerTitlePage(docA,parent);
      break;

    case BMpartpage:
      obj = new XWBeamerPartPage(docA,parent);
      break;

    case BMpause:
      obj = new XWTeXOptionCommand(BMpause,docA,true,"\\pause","number:",parent);
      break;

    case BMonslide:
      obj = new XWBeamerOnSlide(docA,parent);
      break;

    case BMonly:
      obj = new XWBeamerOnly(docA,parent);
      break;

    case BMuncover:
      obj = new XWBeamerSpecCommand(BMuncover,docA,false,"\\uncover",parent);
      break;

    case BMvisible:
      obj = new XWBeamerSpecCommand(BMvisible,docA,false,"\\visible",parent);
      break;

    case BMinvisible:
      obj = new XWBeamerSpecCommand(BMinvisible,docA,false,"\\invisible",parent);
      break;

    case BMalt:
      obj = new XWBeamerAlt(docA,parent);
      break;

    case BMonlyenv:
      obj = new XWBeamerOnlyEnv(docA,parent);
      break;

    case BMaltenv:
      obj = new XWBeamerAltEnv(docA,parent);
      break;

    case BMoverlayarea:
      obj = new XWBeamerOverlayArea(docA,parent);
      break;

    case BMoverprint:
      obj = new XWBeamerOverPrint(docA,parent);
      break;

    case BMactionenv:
      obj = new XWBeamerActionEnv(docA,parent);
      break;

    case BMaction:
      obj = new XWBeamerSpecCommand(BMinvisible,docA,false,"\\action",parent);
      break;

    case BMhypertarget:
      obj = new XWBeamerHyperTarget(docA,parent);
      break;

    case BMbeamerbutton:
      obj = new XWTeXParamCommand(BMbeamerbutton,docA,true,"\\beamerbutton","text:",parent);
      break;

    case BMbeamergotobutton:
      obj = new XWTeXParamCommand(BMbeamerbutton,docA,true,"\\beamergotobutton","text:",parent);
      break;

    case BMbeamerskipbutton:
      obj = new XWTeXParamCommand(BMbeamerbutton,docA,true,"\\beamerskipbutton","text:",parent);
      break;

    case BMbeamerreturnbutton:
      obj = new XWTeXParamCommand(BMbeamerbutton,docA,true,"\\beamerreturnbutton","text:",parent);
      break;

    case BMhyperlink:
      obj = new XWBeamerHyperLink(docA,parent);
      break;

    case BMhyperlinkslideprev:
      obj = new XWBeamerSpecParamCommand(BMhyperlinkslideprev,docA,"\\hyperlinkslideprev","link text:",parent);
      break;

    case BMhyperlinkslidenext:
      obj = new XWBeamerSpecParamCommand(BMhyperlinkslidenext,docA,"\\hyperlinkslidenext","link text:",parent);
      break;

    case BMhyperlinkframestart:
      obj = new XWBeamerSpecParamCommand(BMhyperlinkframestart,docA,"\\hyperlinkframestart","link text:",parent);
      break;

    case BMhyperlinkframeend:
      obj = new XWBeamerSpecParamCommand(BMhyperlinkframeend,docA,"\\hyperlinkframeend","link text:",parent);
      break;

    case BMhyperlinkframestartnext:
      obj = new XWBeamerSpecParamCommand(BMhyperlinkframestartnext,docA,"\\hyperlinkframestartnext","link text:",parent);
      break;

    case BMhyperlinkframeendprev:
      obj = new XWBeamerSpecParamCommand(BMhyperlinkframeendprev,docA,"\\hyperlinkframeendprev","link text:",parent);
      break;

    case BMhyperlinkpresentationstart:
      obj = new XWBeamerSpecParamCommand(BMhyperlinkpresentationstart,docA,"\\hyperlinkpresentationstart","link text:",parent);
      break;

    case BMhyperlinkpresentationend:
      obj = new XWBeamerSpecParamCommand(BMhyperlinkpresentationend,docA,"\\hyperlinkpresentationend","link text:",parent);
      break;

    case BMhyperlinkappendixstart:
      obj = new XWBeamerSpecParamCommand(BMhyperlinkappendixstart,docA,"\\hyperlinkappendixstart","link text:",parent);
      break;

    case BMhyperlinkappendixend:
      obj = new XWBeamerSpecParamCommand(BMhyperlinkappendixend,docA,"\\hyperlinkappendixend","link text:",parent);
      break;

    case BMhyperlinkdocumentstart:
      obj = new XWBeamerSpecParamCommand(BMhyperlinkdocumentstart,docA,"\\hyperlinkdocumentstart","link text:",parent);
      break;

    case BMhyperlinkdocumentend:
      obj = new XWBeamerSpecParamCommand(BMhyperlinkdocumentend,docA,"\\hyperlinkdocumentend","link text:",parent);
      break;

    case BMstructure:
      obj = new XWBeamerSpecParamCommand(BMstructure,docA,"\\structure","text:",parent);
      break;

    case BMstructureenv:
      obj = new XWBeamerStructureEnv(docA,parent);
      break;

    case BMalert:
      obj = new XWBeamerSpecParamCommand(BMalert,docA,"\\alert","highlighted text:",parent);
      break;

    case BMalertenv:
      obj = new XWBeamerAlertEnv(docA,parent);
      break;

    case BMblock:
      obj = new XWBeamerBlock(docA,parent);
      break;

    case BMalertblock:
      obj = new XWBeamerAlertBlock(docA,parent);
      break;

    case BMexampleblock:
      obj = new XWBeamerExampleBlock(docA,parent);
      break;

    case BMbeamercolorbox:
      obj = new XWBeamerColorBox(docA,parent);
      break;

    case BMbeamerboxesrounded:
      obj = new XWBeamerBoxesRounded(docA,parent);
      break;

    case BMcolumn:
      obj = new XWBeamerColumn(docA,parent);
      break;

    case BMcolumns:
      obj = new XWBeamerColumns(docA,parent);
      break;

    case BMsemiverbatim:
      obj = new XWBeamerSemiVerbatim(docA,parent);
      break;

    case BMmovie:
      obj = new XWBeamerMovie(docA,parent);
      break;

    case BMhyperlinkmovie:
      obj = new XWBeamerHyperLinkMovie(docA,parent);
      break;

    case BManimate:
      obj = new XWBeamerSpecCommand(BManimate,docA,false,"\\animate",parent);
      break;

    case BManimatevalue:
      obj = new XWBeamerAnimateValue(docA,parent);
      break;

    case BMmultiinclude:
      obj = new XWBeamerMultiInclude(docA,parent);
      break;

    case BMsound:
      obj = new XWBeamerSound(docA,parent);
      break;

    case BMhyperlinksound:
      obj = new XWBeamerHyperLinkSound(docA,parent);
      break;

    case BMhyperlinkmute:
      obj = new XWTeXParamCommand(BMhyperlinkmute,docA,true,"\\hyperlinkmute","text:",parent);
      break;

    case BMtransblindshorizontal:
      obj = new XWBeamerSpecOptionCommand(BMtransblindshorizontal,docA,"\\transblindshorizontal",parent);
      break;

    case BMtransblindsvertical:
      obj = new XWBeamerSpecOptionCommand(BMtransblindsvertical,docA,"\\transblindsvertical",parent);
      break;

    case BMtransboxin:
      obj = new XWBeamerSpecOptionCommand(BMtransboxin,docA,"\\transboxin",parent);
      break;

    case BMtransboxout:
      obj = new XWBeamerSpecOptionCommand(BMtransboxout,docA,"\\transboxout",parent);
      break;

    case BMtransdissolve:
      obj = new XWBeamerSpecOptionCommand(BMtransdissolve,docA,"\\transdissolve",parent);
      break;

    case BMtransglitter:
      obj = new XWBeamerSpecOptionCommand(BMtransglitter,docA,"\\transglitter",parent);
      break;

    case BMtransreplace:
      obj = new XWBeamerSpecOptionCommand(BMtransreplace,docA,"\\transreplace",parent);
      break;

    case BMtranssplitverticalin:
      obj = new XWBeamerSpecOptionCommand(BMtranssplitverticalin,docA,"\\transsplitverticalin",parent);
      break;

    case BMtranssplitverticalout:
      obj = new XWBeamerSpecOptionCommand(BMtranssplitverticalout,docA,"\\transsplitverticalout",parent);
      break;

    case BMtranssplithorizontalin:
      obj = new XWBeamerSpecOptionCommand(BMtranssplithorizontalin,docA,"\\transsplithorizontalin",parent);
      break;

    case BMtranssplithorizontalout:
      obj = new XWBeamerSpecOptionCommand(BMtranssplithorizontalout,docA,"\\transsplithorizontalout",parent);
      break;

    case BMtranswipe:
      obj = new XWBeamerSpecOptionCommand(BMtranswipe,docA,"\\transwipe",parent);
      break;

    case BMtransduration:
      obj = new XWBeamerSpecParamCommand(BMtransduration,docA,"\\transduration","number of seconds:",parent);
      break;

    case BMopaqueness:
      obj = new XWBeamerSpecParamCommand(BMopaqueness,docA,"\\opaqueness","percentage of opaqueness:",parent);
      break;

    default:
      break;
  }

  return obj;
}

XWBeamerCode::XWBeamerCode(XWTeXDocument * docA, const QString & nameA,QObject * parent)
: XWTeXCode(XW_BEAMER_CODE,docA,nameA,parent)
{}

void XWBeamerCode::scan(const QString & str, int & len, int & pos)
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
      int id = lookupBeamerID(key);
      switch (id)
      {
        case BMend:
          key = scanEnviromentName(str,len,pos);
          id = lookupBeamerID(key);
          if (id == pobj->type())
          {
            b--;
            if (b < 0)
              return ;
          }
          break;

        case BMbegin:
          key = scanEnviromentName(str,len,pos);
          id = lookupBeamerID(key);
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

XWBeamerModifier::XWBeamerModifier(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentObject(XW_BEAMER_MODIFIER,docA,parent)
{
  name = new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, false,tr("modifier:"),this);
  add(new XWTeXDocumentBlock(XW_BEAMER_MODIFIER, docA, this));
}

void XWBeamerModifier::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  dir = doc->getDirection();
  setFont();

  XWTeXDocumentBlock * b = (XWTeXDocumentBlock*)(head);
  if (b->text.isEmpty())
  {
    name->newPage = false;
    name->newRow = false;
    name->indent = indent;
    QColor lcolor = color.darker(TEX_TIP_DARKER);
    name->breakPage(font, lcolor, curx,cury,firstcolumn);
  }

  head->newPage = false;
  head->newRow = false;
  head->breakPage(font, color, curx,cury,firstcolumn);
}

void XWBeamerModifier::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentBlock * m = (XWTeXDocumentBlock*)head;
  while (pos < len)
  {
    if (str[pos] != QChar('*') && str[pos] != QChar('+'))
      return ;

    m->text.append(str[pos]);
    pos++;
  }
}

void XWBeamerModifier::setFont()
{
  setOptionFont();
}

XWBeamerSubtitle::XWBeamerSubtitle(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(BMsubtitle,docA,true, "\\subtitle", parent)
{
  add(new XWLTXOption(docA,this));
  add(new XWLTXParam(docA,this));
}

void XWBeamerSubtitle::breakPage(double & curx,double & cury,bool & firstcolumn)
{
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
  doc->Large();
  setNormalFont();
  head->next->next->specialFont = false;
  head->next->next->newPage = false;
  head->next->next->newRow = true;

  doc->bfseries();
  head->next->next->centered(font, color, curx, cury,firstcolumn);
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

  doc->popWeight();

  doc->popSize();
  doc->restoreTwoColumn();
  head->next->next->newRow = false;
}

XWBeamerInstitute::XWBeamerInstitute(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(BMinstitute,docA,true, "\\institute", parent)
{
  add(new XWLTXOption(docA,this));
  add(new XWLTXParam(docA,this));
}

void XWBeamerInstitute::breakPage(double & curx,double & cury,bool & firstcolumn)
{
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
  doc->large();
  setNormalFont();
  head->next->next->newPage = false;
  head->next->next->newRow = true;
  doc->normalSize();
  setNormalFont();
  head->next->next->centered(font, color, curx, cury,firstcolumn);
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

  doc->popSize();
  doc->restoreTwoColumn();

  head->next->next->newRow = false;
}

XWBeamerTitleGraphic::XWBeamerTitleGraphic(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMtitlegraphic,docA,true, "\\titlegraphic", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("text: "), this));
}

XWBeamerSubject::XWBeamerSubject(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMsubject,docA,true, "\\subject", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("text: "), this));
}

XWBeamerKeywords::XWBeamerKeywords(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMkeywords,docA,true, "\\keywords", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("text: "), this));
}

XWBeamerLogo::XWBeamerLogo(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMlogo,docA,true, "\\logo", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("text: "), this));
}

XWBeamerIncludeOnlyLecture::XWBeamerIncludeOnlyLecture(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMincludeonlylecture,docA,true, "\\includeonlylecture", parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("lecture label: "), this));
}

XWUseBeamerTemplate::XWUseBeamerTemplate(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMusebeamertemplate,docA,true, "\\usebeamertemplate", parent)
{
  add(new XWBeamerModifier(docA,this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("element name: "), this));
}

XWSetBeamerTemplate::XWSetBeamerTemplate(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMsetbeamertemplate,docA,true, "\\setbeamertemplate", parent)
{
  add(new XWBeamerModifier(docA,this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("element name: "), this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION, docA,tr("predefined option: "), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("args: "), this));
}

XWIfBeamerColorEmpty::XWIfBeamerColorEmpty(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMifbeamercolorempty,docA,true, "\\ifbeamercolorempty", parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION, docA,tr("fg or bg: "), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("beamer-color name: "), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("if undefined: "), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("if defined: "), this));
}

XWUseBeamerColor::XWUseBeamerColor(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMusebeamercolor,docA,true, "\\usebeamercolor", parent)
{
  add(new XWBeamerModifier(docA,this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION, docA,tr("fg or bg: "), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("beamer-color name: "), this));
}

XWSetBeamerColor::XWSetBeamerColor(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMsetbeamercolor,docA,true, "\\setbeamercolor", parent)
{
  add(new XWBeamerModifier(docA,this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("beamer-color name: "), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("options: "), this));
}

XWSetBeamerFont::XWSetBeamerFont(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMsetbeamerfont,docA,true, "\\setbeamerfont", parent)
{
  add(new XWBeamerModifier(docA,this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("beamer-font name: "), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("attributes: "), this));
}

XWUseBeamerFont::XWUseBeamerFont(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMusebeamerfont,docA,true, "\\usebeamerfont", parent)
{
  add(new XWBeamerModifier(docA,this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("beamer-font name: "), this));
}

XWBeamerLecture::XWBeamerLecture(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMlecture,docA,true, "\\lecture", parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION, docA,tr("short lecture name: "), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("lecture name: "), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("lecture label: "), this));
}

XWBeamerMode::XWBeamerMode(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMmode,docA,true, "\\mode", parent)
{
  add(new XWBeamerModifier(docA,this));
  add(new XWTeXDocumentBeamerSpec(docA,tr("mode:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM, docA,tr("text: "), this));
}

void XWBeamerMode::write(QTextStream & strm, int & linelen)
{
  head->write(strm,linelen);
  head->next->write(strm,linelen);
  last->prev->write(strm,linelen);
  if (!last->isEmpty())
    last->write(strm,linelen);
}

XWBeamerNode::XWBeamerNode(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMnote,docA,true, "\\note", parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("options:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("note text:"),this));
}

XWBeamerAgainFrame::XWBeamerAgainFrame(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(BMagainframe, docA,true, "\\againframe",parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWBeamerDefaultSpec(docA,tr("default overlay specification:"),this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("options:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("name:"),this));
}

XWBeamerFrameEnv::XWBeamerFrameEnv(XWTeXDocument * docA, QObject * parent)
: XWLTXEnviroment(BMframe,docA,"\\begin{frame}", parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWBeamerDefaultSpec(docA,tr("default overlay specification:"), this));
  add(new XWTeXDocumentOption(XW_BEAMER_OPTION,docA,tr("options:"), this));
  add(new XWTeXDocumentParam(XW_LTX_PARAM,docA,tr("title:"), this));
  add(new XWTeXDocumentParam(XW_LTX_PARAM,docA,tr("subtitle:"), this));
  add(new XWLTXDocumentPar(docA,this));
  add(new XWTeXControlSequence(docA, true, "\\end{frame}", this));
}

void XWBeamerFrameEnv::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  ldoc->resetSides();
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && !doc->isInPreamble())
  {
    bool newpage = false;
    switch (head->next->next->next->next->next->next->type())
    {
      case BMtitlepage:
      case BMpartpage:
        break;

      default:
        {
          XWTeXDocumentObject *   sect = ldoc->getCurrentPart();
          if (sect)
          {
            if (!newpage)
            {
              newpage = true;
              sect->newPage = true;
            }
            sect->breakPage(curx,cury,firstcolumn);
            sect->newPage = false;
          }

          sect = ldoc->getCurrentSection();
          if (sect)
          {
            if (!newpage)
            {
              newpage = true;
              sect->newPage = true;
            }
            sect->breakPage(curx,cury,firstcolumn);
            sect->newPage = false;
          }
          sect = ldoc->getCurrentSubsection();
          if (sect)
          {
            if (!newpage)
            {
              newpage = true;
              sect->newPage = true;
            }
            sect->breakPage(curx,cury,firstcolumn);
            sect->newPage = false;
          }
          sect = ldoc->getCurrentSubsubsection();
          if (sect)
          {
            if (!newpage)
            {
              newpage = true;
              sect->newPage = true;
            }
            sect->breakPage(curx,cury,firstcolumn);
            sect->newPage = false;
          }
          break;

          switch (dir)
          {
            case TEX_DOC_WD_TLT:
            case TEX_DOC_WD_TRT:
              if (head->next->next->next->next->next->next->type() == BMframetitle ||
                 !head->next->next->next->next->isEmpty())
                cury += doc->getCurrentFontSize();
              else
                cury += 4 * doc->getCurrentFontSize();
              break;

            case TEX_DOC_WD_RTT:
              if (head->next->next->next->next->next->next->type() == BMframetitle ||
                 !head->next->next->next->next->isEmpty())
                curx -= doc->getCurrentFontSize();
              else
                curx -= 3 * doc->getCurrentFontSize();
              break;

            default:
              if (head->next->next->next->next->next->next->type() == BMframetitle ||
                 !head->next->next->next->next->isEmpty())
                curx += doc->getCurrentFontSize();
              else
                curx += 3 * doc->getCurrentFontSize();
              break;
          }
        }
    }

    XWTeXDocumentObject * obj = head->next->next->next->next;
    if (!obj->isEmpty())
    {
      doc->bfseries();
      ldoc->huge();
      obj->head->next->newRow = true;
      if (!newpage)
      {
        newpage = true;
        obj->head->next->newPage = true;
      }
      obj->head->next->breakPage(font, color, curx, cury, firstcolumn);
      obj->head->next->newPage = false;

      if (obj->next->isEmpty())
      {
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
      }

      doc->popSize();
      doc->popWeight();
      obj->head->next->newRow = false;
    }

    obj = obj->next;
    if (!obj->isEmpty())
    {
      doc->bfseries();
      ldoc->Large();
      obj->head->next->newRow = true;
      if (!newpage)
      {
        newpage = true;
        obj->head->next->newPage = true;
      }
      obj->head->next->breakPage(font, color, curx, cury, firstcolumn);
      obj->head->next->newPage = false;

      switch (dir)
      {
        case TEX_DOC_WD_TLT:
        case TEX_DOC_WD_TRT:
          cury += 1.5 * doc->getCurrentFontSize();
          break;

        case TEX_DOC_WD_RTT:
          curx -= doc->getCurrentFontSize();
          break;

        default:
          curx += doc->getCurrentFontSize();
          break;
      }

      doc->popSize();
      doc->popWeight();
      obj->head->next->newRow = false;
    }

    obj = obj->next;
    while (obj != last)
    {
      if (obj->type() != BMframetitle && obj->type() != BMframesubtitle)
      {
        obj->indent = 2.0;
        obj->firstIndent = 0.0;
      }

      obj->newRow = true;
      if (!newpage)
      {
        newpage = true;
        obj->newPage = true;
        ldoc->setSavedTop(72);
        ldoc->setSavedBottom(72);
      }
      obj->breakPage(curx,cury, firstcolumn);
      obj->newPage = false;
      obj = obj->next;
    }
    obj->newRow = true;
    obj->newPage = false;
  }
  else
  {
    XWTeXDocumentObject * obj = head;
    obj->newRow = true;
    obj->newPage = false;
    obj->breakPage(curx,cury, firstcolumn);
    obj = obj->next;
    obj->newRow = false;
    obj->newPage = false;
    obj->breakPage(curx,cury, firstcolumn);
    obj = obj->next;
    obj->newRow = false;
    obj->newPage = false;
    obj->breakPage(curx,cury, firstcolumn);
    obj = obj->next;
    obj->newRow = false;
    obj->newPage = false;
    obj->breakPage(curx,cury, firstcolumn);
    obj = obj->next;
    obj->newRow = false;
    obj->newPage = false;
    obj->breakPage(curx,cury, firstcolumn);
    obj = obj->next;
    obj->newRow = false;
    obj->newPage = false;
    obj->breakPage(curx,cury, firstcolumn);
    obj = obj->next;
    while (obj != last)
    {
      obj->newRow = true;
      obj->newPage = false;
      obj->breakPage(curx,cury, firstcolumn);
      obj = obj->next;
    }

    obj->newRow = true;
    obj->newPage = false;
    obj->breakPage(curx,cury, firstcolumn);
    obj = obj->next;
  }
}

QString XWBeamerFrameEnv::getMimeData()
{
  XWTeXDocumentObject * obj = head;
  QString str = obj->getMimeData();
  obj = obj->next;
  if (!obj->isEmpty())
  {
    QString tmp = obj->getMimeData();
    str += tmp;
  }

  obj = obj->next;
  if (!obj->isEmpty())
  {
    QString tmp = obj->getMimeData();
    str += tmp;
  }

  obj = obj->next;
  if (!obj->isEmpty())
  {
    QString tmp = obj->getMimeData();
    str += tmp;
  }

  obj = obj->next;
  if (!obj->isEmpty())
  {
    QString tmp = obj->getMimeData();
    str += tmp;
  }

  obj = obj->next;
  if (!obj->isEmpty())
  {
    QString tmp = obj->getMimeData();
    str += tmp;
  }

  obj = obj->next;
  while (obj != last)
  {
    QString tmp = obj->getMimeData();
    str += tmp;
    obj = obj->next;
  }

  QString tmp = obj->getMimeData();
  str += tmp;

  return str;
}

void XWBeamerFrameEnv::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  sobj->prev = head->next->next->next->next->next;
  head->next->next->next->next->next->next = sobj;
  eobj->next = last;
  last->prev = eobj;    
}

bool XWBeamerFrameEnv::isAllSelected()
{
  return head->next->next->next->next->next->next->isAllSelected() && last->prev->isAllSelected();
}

void XWBeamerFrameEnv::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next->next->next->next->next;
  *eobj = last->prev;
  head->next->next->next->next->next->next = last;
  last->prev = head->next->next->next->next->next;
}

void XWBeamerFrameEnv::removeChildrenTo(XWTeXDocumentObject**sobj, 
                                        XWTeXDocumentObject**eobj,
                                        XWTeXDocumentObject**obj)
{
  *sobj = head->next->next->next->next->next->next;
  *obj = (*sobj)->prev;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWBeamerFrameEnv::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentObject * obj = head->next;
  obj->scan(str,len,pos);
  obj = obj->next;
  obj->scan(str,len,pos);
  obj = obj->next;
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
        int id = lookupBeamerID(key);
        switch (id)
        {
          case BMend:
            key = scanEnviromentName(str,len,pos);
            id = lookupBeamerID(key);
            if (id == BMframe)
              return ;
            break;

          case BMtitlepage:
          case BMpartpage:
          case BMframetitle:
          case BMframesubtitle:
          case BMtableofcontents:
          case BMappendix:
          case BMframezoom:
          case BMmovie:
          case BMhyperlinkmovie:
          case BManimate:
          case BManimatevalue:
          case BMmultiinclude:
          case BMsound:
          case BMhyperlinksound:
          case BMhyperlinkmute:
          case BMtransblindshorizontal:
          case BMtransblindsvertical:
          case BMtransboxin:
          case BMtransboxout:
          case BMtransdissolve:
          case BMtransglitter:
          case BMtransreplace:
          case BMtranssplitverticalin:
          case BMtranssplitverticalout:
          case BMtranssplithorizontalin:
          case BMtranssplithorizontalout:
          case BMtranswipe:
          case BMtransduration:
            obj = doc->createObject(key,str,pos, this);
            break;

          default:
            pos = p;
            obj = new XWLTXDocumentPar(doc, this);
            break;
        }
      }
      else
        obj = new XWLTXDocumentPar(doc, this);

      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

void XWBeamerFrameEnv::write(QTextStream & strm, int & linelen)
{
  XWTeXDocumentObject * obj = head;
  obj->write(strm, linelen);
  obj = obj->next;
  if (!obj->isEmpty())
    obj->write(strm, linelen);

  obj = obj->next;
  if (!obj->isEmpty())
    obj->write(strm, linelen);

  obj = obj->next;
  if (!obj->isEmpty())
    obj->write(strm, linelen);

  obj = obj->next;
  if (!obj->isEmpty())
    obj->write(strm, linelen);

  obj = obj->next;
  if (!obj->isEmpty())
    obj->write(strm, linelen);

  if (linelen != 0)
  {
    strm << "\n";
    linelen = 0;
  }

  obj = obj->next;
  while (obj != last)
  {
    obj->write(strm, linelen);
    obj = obj->next;
  }

  obj->write(strm, linelen);
  if (linelen != 0)
  {
    strm << "\n";
    linelen = 0;
  }

  strm << "\n";
}

XWBeamerFrameCmd::XWBeamerFrameCmd(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(BMframe,docA,true, "\\frame", parent)
{
  add(new XWTeXDocumentOption(XW_BEAMER_OPTION,docA,tr("options:"), this));
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true,"{",this));
  add(new XWLTXDocumentPar(docA,this));
  add(new XWTeXDocumentBlock(TEX_DOC_B_NAME, docA, true,"}",this));
}

void XWBeamerFrameCmd::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  ldoc->resetSides();
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && !doc->isInPreamble())
  {
    bool newpage = false;
    switch (head->next->next->next->type())
    {
      case BMtitlepage:
      case BMpartpage:
        break;

      default:
        {
          XWTeXDocumentObject * sect = ldoc->getCurrentPart();
          if (sect)
          {
            if (!newpage)
            {
              newpage = true;
              sect->newPage = true;
            }
            sect->breakPage(curx,cury,firstcolumn);
            sect->newPage = false;
          }
          sect = ldoc->getCurrentSection();
          if (sect)
          {
            if (!newpage)
            {
              newpage = true;
              sect->newPage = true;
            }
            sect->breakPage(curx,cury,firstcolumn);
            sect->newPage = false;
          }
          sect = ldoc->getCurrentSubsection();
          if (sect)
          {
            if (!newpage)
            {
              newpage = true;
              sect->newPage = true;
            }
            sect->breakPage(curx,cury,firstcolumn);
            sect->newPage = false;
          }
          sect = ldoc->getCurrentSubsubsection();
          if (sect)
          {
            if (!newpage)
            {
              newpage = true;
              sect->newPage = true;
            }
            sect->breakPage(curx,cury,firstcolumn);
            sect->newPage = false;
          }
          break;
        }

        switch (dir)
        {
          case TEX_DOC_WD_TLT:
          case TEX_DOC_WD_TRT:
            if (head->next->next->next->type() == BMframetitle)
              cury += doc->getCurrentFontSize();
            else
              cury += 4 * doc->getCurrentFontSize();
            break;

          case TEX_DOC_WD_RTT:
            if (head->next->next->next->type() == BMframetitle)
              curx -= doc->getCurrentFontSize();
            else
              curx -= 3 * doc->getCurrentFontSize();
            break;

          default:
            if (head->next->next->next->type() == BMframetitle)
              curx += doc->getCurrentFontSize();
            else
              curx += 3 * doc->getCurrentFontSize();
            break;
        }
    }

    XWTeXDocumentObject * obj = head->next->next->next;
    while (obj != last)
    {
      if (obj->type() != BMframetitle && obj->type() != BMframesubtitle)
      {
        obj->indent = 2.0;
        obj->firstIndent = 0.0;
      }

      if (!newpage)
      {
        newpage = true;
        obj->newPage = true;
        ldoc->setSavedTop(72);
        ldoc->setSavedBottom(72);
      }
      obj->newRow = true;
      obj->breakPage(curx,cury, firstcolumn);
      obj->newPage = false;
      obj = obj->next;
    }
  }
  else
  {
    XWTeXDocumentObject * obj = head;
    obj->newRow = true;
    obj->newPage = false;
    obj->breakPage(curx,cury, firstcolumn);
    obj = obj->next;
    obj->newRow = false;
    obj->newPage = false;
    obj->breakPage(curx,cury, firstcolumn);
    obj = obj->next;
    obj->newRow = true;
    obj->newPage = false;
    obj->breakPage(curx,cury, firstcolumn);
    obj = obj->next;
    while (obj != last)
    {
      obj->newRow = true;
      obj->newPage = false;
      obj->breakPage(curx,cury, firstcolumn);
      obj = obj->next;
    }

    obj->newRow = true;
    obj->newPage = false;
    obj->breakPage(curx,cury, firstcolumn);
    obj = obj->next;
  }
}

QString XWBeamerFrameCmd::getMimeData()
{
  XWTeXDocumentObject * obj = head;
  QString str = obj->getMimeData();
  obj = obj->next;
  if (!obj->isEmpty())
  {
    QString tmp = obj->getMimeData();
    str += tmp;
  }

  obj = obj->next;
  QString tmp = obj->getMimeData();
  str += tmp;

  obj = obj->next;
  while (obj != last)
  {
    tmp = obj->getMimeData();
    str += tmp;
    obj = obj->next;
  }

  tmp = obj->getMimeData();
  str += tmp;

  return str;
}

bool XWBeamerFrameCmd::isAllSelected()
{
  return head->next->next->next->isAllSelected() && last->prev->prev->isAllSelected();
}

void XWBeamerFrameCmd::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentObject * obj = head->next;
  obj->scan(str,len,pos);
  while (str[pos].isSpace())
    pos++;
  if (pos > len)
    return ;

  if (str[pos] != QChar('{'))
    return ;

  del(head->next->next->next);
  pos++;
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('\\'))
    {
      int p = pos;
      QString key = scanControlSequence(str,len,pos);
      int id = lookupBeamerID(key);
      switch (id)
      {
        case BMtitlepage:
        case BMpartpage:
        case BMframetitle:
        case BMframesubtitle:
        case BMtableofcontents:
        case BMappendix:
        case BMframezoom:
        case BMmovie:
        case BMhyperlinkmovie:
        case BManimate:
        case BManimatevalue:
        case BMmultiinclude:
        case BMsound:
        case BMhyperlinksound:
        case BMhyperlinkmute:
        case BMtransblindshorizontal:
        case BMtransblindsvertical:
        case BMtransboxin:
        case BMtransboxout:
        case BMtransdissolve:
        case BMtransglitter:
        case BMtransreplace:
        case BMtranssplitverticalin:
        case BMtranssplitverticalout:
        case BMtranssplithorizontalin:
        case BMtranssplithorizontalout:
        case BMtranswipe:
        case BMtransduration:
          obj = doc->createObject(key,str,pos, this);
          last->insertAtBefore(obj);
          obj->scan(str,len,pos);
          break;

        default:
          pos = p;
          scanPar(str,len,pos);
          break;
      }
    }
    else if (str[pos] == QChar('}'))
    {
      pos++;
      return ;
    }
    else
      scanPar(str,len,pos);
  }
}

void XWBeamerFrameCmd::scanPar(const QString & str, int & len, int & pos)
{
  XWLTXDocumentPar * par = new XWLTXDocumentPar(doc, this);
  del(par->head);
  last->insertAtBefore(par);
  int b = 0;
  XWTeXDocumentText * tobj = 0;
  XWTeXDocumentObject * obj = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('\\'))
    {
      QString key = scanControlSequence(str, len,pos);
      if (key == "par")
        return;

      obj = doc->createObject(key,str,pos, par);
      par->add(obj);
      obj->scan(str,len,pos);
      tobj = 0;
    }
    else if (str[pos] == QChar('$'))
    {
      if (pos < (len - 1) && str[pos + 1] == QChar('$'))
        obj = new XWTeXDocumentDisplayFormular(doc, par);
      else
        obj = new XWTeXDocumentFormular(doc, par);
      par->add(obj);
      obj->scan(str,len,pos);
      tobj = 0;
    }
    else if (str[pos] == QChar('%'))
    {
      obj = new XWTeXDocumentComment(doc,par);
      par->add(obj);
      obj->scan(str,len,pos);
      tobj = 0;
    }
    else if (str[pos] == QChar('}'))
    {
      b--;
      if (b < 0)
        return;

      if (!tobj)
      {
        tobj = new XWTeXDocumentText(doc,par);
        par->add(tobj);
      }

      tobj->text.append(str[pos]);
      pos++;
    }
    else if (str[pos] == QChar('{'))
    {
      b++;
      if (!tobj)
      {
        tobj = new XWTeXDocumentText(doc,par);
        par->add(tobj);
      }

      tobj->text.append(str[pos]);
      pos++;
    }
    else
    {
      if (str[pos] == QChar('\n'))
      {
        if ((pos + 1) > len)
        {
          pos++;
          return ;
        }

        if (str[pos + 1] == QChar('\n'))
        {
          pos += 2;
          return ;
        }

        if (!tobj)
        {
          tobj = new XWTeXDocumentText(doc,par);
          par->add(tobj);
        }

        tobj->text.append(QChar(' '));
        pos++;
      }
      else
      {
        if (!tobj)
        {
          tobj = new XWTeXDocumentText(doc,par);
          par->add(tobj);
        }

        tobj->text.append(str[pos]);
        pos++;
      }
    }
  }
}

XWBeamerFrameTitle::XWBeamerFrameTitle(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMframetitle,docA,true, "\\frametitle", parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWLTXOption(docA,this));
  add(new XWLTXParam(docA,this));
}

void XWBeamerFrameTitle::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && !doc->isInPreamble())
  {
    doc->bfseries();
    ldoc->huge();
    head->next->next->next->newRow = true;
    head->next->next->next->newPage = false;
    head->next->next->next->breakPage(font, color, curx, cury, firstcolumn);
    if (!next || next->type() != BMframesubtitle)
    {
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

      doc->popSize();
      doc->popWeight();
      head->next->next->next->newRow = false;
    }
  }
  else
    XWTeXDocumentObject::breakPage(curx, cury, firstcolumn);
}

XWBeamerFrameSubitle::XWBeamerFrameSubitle(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMframesubtitle,docA,true, "\\framesubtitle", parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWLTXParam(docA,this));
}

void XWBeamerFrameSubitle::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && !doc->isInPreamble())
  {
    doc->bfseries();
    ldoc->Large();
    head->next->next->newRow = true;
    head->next->next->newPage = false;
    head->next->next->breakPage(font, color, curx, cury, firstcolumn);

    switch (dir)
    {
      case TEX_DOC_WD_TLT:
      case TEX_DOC_WD_TRT:
        cury += 1.5 * doc->getCurrentFontSize();
        break;

      case TEX_DOC_WD_RTT:
        curx -= doc->getCurrentFontSize();
        break;

      default:
        curx += doc->getCurrentFontSize();
        break;
    }

    doc->popSize();
    doc->popWeight();
    head->next->next->newRow = false;
  }
  else
    XWTeXDocumentObject::breakPage(curx, cury, firstcolumn);
}

XWBeamerFrameZoom::XWBeamerFrameZoom(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(BMframezoom,docA,true, "\\framezoom", parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("button overlay specification:"),this));
  add(new XWTeXDocumentBeamerSpec(docA,tr("zoomed overlay specification:"),this));
  add(new XWTeXDocumentOption(XW_BEAMER_OPTION,docA,tr("options:"), this));
  add(new XWTeXCoord(XW_BEAMER_COORD,docA,tr("upper lefter:"),this));
  add(new XWTeXCoord(XW_BEAMER_COORD,docA,tr("zoom area:"),this));
}

XWBeamerTitlePage::XWBeamerTitlePage(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(BMtitlepage,docA,true, "\\titlepage", parent)
{}

void XWBeamerTitlePage::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  dir = doc->getDirection();
  setBoldFont();

  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && !doc->isInPreamble())
  {
    XWTeXDocumentObject * obj = ldoc->getTitle();
    if (obj)
      obj->breakPage(curx, cury,firstcolumn);

    obj = ldoc->getSubtitle();
    if (obj)
      obj->breakPage(curx, cury,firstcolumn);

    obj = ldoc->getAuthor();
    if (obj)
      obj->breakPage(curx, cury,firstcolumn);

    obj = ldoc->getInstitute();
    if (obj)
      obj->breakPage(curx, cury,firstcolumn);

    obj = ldoc->getDate();
    if (obj)
      obj->breakPage(curx, cury,firstcolumn);
  }
  else
    head->breakPage(curx, cury,firstcolumn);
}

XWBeamerPartPage::XWBeamerPartPage(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(BMpartpage,docA,true, "\\partpage", parent)
{}

void XWBeamerPartPage::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  dir = doc->getDirection();
  setBoldFont();

  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && !doc->isInPreamble())
  {
    doc->bfseries();
    ldoc->huge();
    XWTeXDocumentObject * obj = ldoc->getCurrentPart();
    if (obj)
      obj->last->prev->breakPage(font, color, curx, cury, firstcolumn);

    doc->popSize();
    doc->popWeight();
  }
  else
    head->breakPage(curx, cury,firstcolumn);
}

XWBeamerOnSlide::XWBeamerOnSlide(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(BMonslide,docA,false, "\\onslide", parent)
{
  add(new XWBeamerModifier(docA,this));
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWTeXOptionParam(docA,tr("text:"),this));
}

XWBeamerOnly::XWBeamerOnly(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(BMonly,docA,false, "\\only", parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("text:"),this));
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
}

XWBeamerAlt::XWBeamerAlt(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(BMalt,docA,false, "\\alt", parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("default text:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("alternative text:"),this));
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
}

XWBeamerTemporal::XWBeamerTemporal(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(BMtemporal,docA,false, "\\temporal", parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("before slide text:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("default text:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("after slide text:"),this));
}

XWBeamerOnlyEnv::XWBeamerOnlyEnv(XWTeXDocument * docA, QObject * parent)
: XWLTXParEnv(BMonlyenv, docA,"\\begin{onlyenv}",parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{onlyenv}", this));
}

XWBeamerAltEnv::XWBeamerAltEnv(XWTeXDocument * docA, QObject * parent)
: XWLTXEnviroment(BMaltenv, docA,"\\begin{altenv}",parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("begin text:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("end text:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("alternate begin text:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("alternate end text:"),this));
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWLTXDocumentPar(docA,this));
  add(new XWTeXControlSequence(docA, true, "\\end{altenv}", this));
}

void XWBeamerAltEnv::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  sobj->prev = head->next->next->next->next->next->next;
  eobj->next = last;
  last->prev = eobj;    
  head->next->next->next->next->next->next = sobj;
}

void XWBeamerAltEnv::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next->next->next->next->next->next;
  *eobj = last->prev;
  head->next->next->next->next->next->next->next = last;
  last->prev = head->next->next->next->next->next->next;
}

void XWBeamerAltEnv::removeChildrenTo(XWTeXDocumentObject**sobj, 
                                        XWTeXDocumentObject**eobj,
                                        XWTeXDocumentObject**obj)
{
  *sobj = head->next->next->next->next->next->next->next;
  *obj = (*sobj)->prev;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWBeamerAltEnv::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentObject * obj = head->next;
  obj->scan(str,len,pos);
  obj = obj->next;
  obj->scan(str,len,pos);
  obj = obj->next;
  obj->scan(str,len,pos);
  obj = obj->next;
  obj->scan(str,len,pos);
  obj = obj->next;
  obj->scan(str,len,pos);
  obj = obj->next;
  obj->scan(str,len,pos);
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
        int id = lookupBeamerID(key);
        switch (id)
        {
          case BMend:
            key = scanEnviromentName(str,len,pos);
            id = lookupBeamerID(key);
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

XWBeamerOverlayArea::XWBeamerOverlayArea(XWTeXDocument * docA, QObject * parent)
: XWLTXEnviroment(BMoverlayarea, docA,"\\begin{overlayarea}",parent)
{
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("area width:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("area height:"),this));
  add(new XWLTXDocumentPar(docA,this));
  add(new XWTeXControlSequence(docA, true, "\\end{overlayarea}", this));
}

void XWBeamerOverlayArea::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
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
  eobj->next = last;
  last->prev = eobj;    
  head->next->next->next = sobj;
}

void XWBeamerOverlayArea::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next->next;
  *eobj = last->prev;
  head->next->next->next = last;
  last->prev = head->next->next;
}

void XWBeamerOverlayArea::removeChildrenTo(XWTeXDocumentObject**sobj, 
                                        XWTeXDocumentObject**eobj,
                                        XWTeXDocumentObject**obj)
{
  *sobj = head->next->next->next;
  *obj = (*sobj)->prev;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWBeamerOverlayArea::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentObject * obj = head->next;
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
        int id = lookupBeamerID(key);
        switch (id)
        {
          case BMend:
            key = scanEnviromentName(str,len,pos);
            id = lookupBeamerID(key);
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

XWBeamerOverPrint::XWBeamerOverPrint(XWTeXDocument * docA, QObject * parent)
: XWLTXEnviroment(BMoverprint, docA,"\\begin{overprint}",parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("area height:"),this));
  add(new XWLTXDocumentPar(docA,this));
  add(new XWTeXControlSequence(docA, true, "\\end{overprint}", this));
}

void XWBeamerOverPrint::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
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
  eobj->next = last;
  last->prev = eobj;    
  head->next->next = sobj;
}

void XWBeamerOverPrint::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next;
  *eobj = last->prev;
  head->next->next = last;
  last->prev = head->next;
}

void XWBeamerOverPrint::removeChildrenTo(XWTeXDocumentObject**sobj, 
                                        XWTeXDocumentObject**eobj,
                                        XWTeXDocumentObject**obj)
{
  *sobj = head->next->next;
  *obj = (*sobj)->prev;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWBeamerOverPrint::scan(const QString & str, int & len, int & pos)
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
        int id = lookupBeamerID(key);
        switch (id)
        {
          case BMend:
            key = scanEnviromentName(str,len,pos);
            id = lookupBeamerID(key);
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

XWBeamerActionEnv::XWBeamerActionEnv(XWTeXDocument * docA, QObject * parent)
: XWLTXParEnv(BMactionenv, docA,"\\begin{actionenv}",parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{actionenv}", this));
}

XWBeamerHyperTarget::XWBeamerHyperTarget(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(BMhypertarget, docA,false, "\\hypertarget",parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("target name:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("text:"),this));
}

XWBeamerHyperLink::XWBeamerHyperLink(XWTeXDocument * docA, QObject * parent)
: XWTeXDocumentCommand(BMhyperlink, docA,false, "\\hyperlink",parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("target name:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("link text:"),this));
  add(new XWTeXDocumentBeamerSpec(docA,tr("overlay specification:"),this));
}

XWBeamerStructureEnv::XWBeamerStructureEnv(XWTeXDocument * docA, QObject * parent)
: XWLTXParEnv(BMstructureenv, docA,"\\begin{structureenv}",parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{structureenv}", this));
}

XWBeamerAlertEnv::XWBeamerAlertEnv(XWTeXDocument * docA, QObject * parent)
: XWLTXParEnv(BMalertenv, docA,"\\begin{alertenv}",parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{alertenv}", this));
}

XWBeamerBlockEnv::XWBeamerBlockEnv(int tA, XWTeXDocument * docA, const QString & nameA, QObject * parent)
: XWLTXEnviroment(tA, docA,nameA,parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("action specification:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("block title:"),this));
  add(new XWTeXDocumentBeamerSpec(docA,tr("action specification:"),this));
  add(new XWLTXDocumentPar(docA,this));
}

void XWBeamerBlockEnv::breakPage(double & curx,double & cury,bool & firstcolumn)
{
  double oldindent = indent;
  indent += 2.0;
  dir = doc->getDirection();
  XWLaTeXDocument * ldoc = (XWLaTeXDocument*)(doc);
  if (ldoc->getClassIndex() == XW_LTX_DOC_CLS_BEAMER && !doc->isInPreamble())
  {
    doc->bfseries();
    doc->large();
    setBoldFont();

    head->next->next->indent = indent;
    head->next->next->newRow = true;
    head->next->next->specialFont = false;
    head->next->next->breakPage(font, color, curx, cury, firstcolumn);

    doc->popSize();
    doc->setItalic(true);
    setItalicFont();

    XWTeXDocumentObject * obj = head->next->next->next->next;
    while (obj)
    {
      if (obj == last)
        break;

      obj->indent = indent;
      if (obj == head->next->next->next->next)
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
  else
  {
    head->next->next->next->next->newRow = true;
    XWTeXDocumentObject::breakPage(curx, cury, firstcolumn);
  }

  indent = oldindent;
}

void XWBeamerBlockEnv::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
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
  eobj->next = last;
  last->prev = eobj;    
  head->next->next->next->next = sobj;
}

void XWBeamerBlockEnv::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next->next->next;
  *eobj = last->prev;
  head->next->next->next->next = last;
  last->prev = head->next->next->next;
}

void XWBeamerBlockEnv::removeChildrenTo(XWTeXDocumentObject**sobj, 
                                        XWTeXDocumentObject**eobj,
                                        XWTeXDocumentObject**obj)
{
  *sobj = head->next->next->next->next;
  *obj = (*sobj)->prev;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWBeamerBlockEnv::scan(const QString & str, int & len, int & pos)
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
        int id = lookupBeamerID(key);
        switch (id)
        {
          case BMend:
            key = scanEnviromentName(str,len,pos);
            id = lookupBeamerID(key);
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

XWBeamerBlock::XWBeamerBlock(XWTeXDocument * docA, QObject * parent)
:XWBeamerBlockEnv(BMblock,docA, "\\begin{block}",parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{block}", this));
}

XWBeamerAlertBlock::XWBeamerAlertBlock(XWTeXDocument * docA, QObject * parent)
:XWBeamerBlockEnv(BMalertblock,docA, "\\begin{alertblock}",parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{alertblock}", this));
}

XWBeamerExampleBlock::XWBeamerExampleBlock(XWTeXDocument * docA, QObject * parent)
:XWBeamerBlockEnv(BMexampleblock,docA, "\\begin{exampleblock}",parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{exampleblock}", this));
}

XWBeamerBoxed::XWBeamerBoxed(int tA, XWTeXDocument * docA, const QString & nameA, const QString & pA, QObject * parent)
: XWLTXEnviroment(tA, docA,nameA,parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("options:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,pA,this));
  add(new XWLTXDocumentPar(docA,this));
}

void XWBeamerBoxed::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
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
  eobj->next = last;
  last->prev = eobj;    
  head->next->next->next = sobj;
}

void XWBeamerBoxed::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next->next;
  *eobj = last->prev;
  head->next->next->next = last;
  last->prev = head->next->next;
}

void XWBeamerBoxed::removeChildrenTo(XWTeXDocumentObject**sobj, 
                                        XWTeXDocumentObject**eobj,
                                        XWTeXDocumentObject**obj)
{
  *sobj = head->next->next->next;
  *obj = (*sobj)->prev;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWBeamerBoxed::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentObject * obj = head->next;
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
        int id = lookupBeamerID(key);
        switch (id)
        {
          case BMend:
            key = scanEnviromentName(str,len,pos);
            id = lookupBeamerID(key);
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

XWBeamerColorBox::XWBeamerColorBox(XWTeXDocument * docA, QObject * parent)
:XWBeamerBoxed(BMbeamercolorbox,docA, "\\begin{colorbox}",tr("beamer color"),parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{colorbox}", this));
}

XWBeamerBoxesRounded::XWBeamerBoxesRounded(XWTeXDocument * docA, QObject * parent)
:XWBeamerBoxed(BMbeamerboxesrounded,docA, "\\begin{boxesrounded}",tr("head"),parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{boxesrounded}", this));
}

XWBeamerColumnEnv::XWBeamerColumnEnv(XWTeXDocument * docA, QObject * parent)
:XWBeamerBoxed(BMcolumn,docA, "\\begin{column}",tr("column width"),parent)
{
  add(new XWTeXControlSequence(docA, true, "\\end{column}", this));
}

XWBeamerColumn::XWBeamerColumn(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMcolumn, docA, true,"\\column", parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("placement:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA,tr("column width:"),this));
  add(new XWLTXDocumentPar(docA,this));
}

void XWBeamerColumn::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
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
  eobj->next = last;
  last->prev = eobj;    
  head->next->next->next = sobj;
}

void XWBeamerColumn::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next->next;
  *eobj = last;
  head->next->next->next = 0;
  last = head->next->next;
}

void XWBeamerColumn::removeChildrenTo(XWTeXDocumentObject**sobj, 
                                        XWTeXDocumentObject**eobj,
                                        XWTeXDocumentObject**obj)
{
  *sobj = head->next->next->next;
  *obj = (*sobj)->prev;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWBeamerColumn::scan(const QString & str, int & len, int & pos)
{
  XWTeXDocumentObject * obj = head->next;
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
        int id = lookupBeamerID(key);
        pos = p;
        switch (id)
        {
          case BMend:
          case BMcolumn:
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

XWBeamerColumns::XWBeamerColumns(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(BMcolumns, docA, "\\begin{columns}",parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA,tr("options:"),this));  
  add(new XWBeamerColumn(docA,parent));
  add(new XWTeXControlSequence(docA, true, "\\end{columns}", this));
}

void XWBeamerColumns::insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj)
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
  eobj->next = last;
  last->prev = eobj;    
  head->next->next = sobj;
}

void XWBeamerColumns::removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj)
{
  *sobj = head->next->next;
  *eobj = last->prev;
  head->next->next = last;
  last->prev = head->next;
}

void XWBeamerColumns::removeChildrenTo(XWTeXDocumentObject**sobj, 
                                        XWTeXDocumentObject**eobj,
                                        XWTeXDocumentObject**obj)
{
  *sobj = head->next->next;
  *obj = (*sobj)->prev;
  (*sobj)->prev->next = (*eobj)->next;
  (*eobj)->next->prev = (*sobj)->prev;
}

void XWBeamerColumns::scan(const QString & str, int & len, int & pos)
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
        QString key = scanControlSequence(str,len,pos);
        int id = lookupBeamerID(key);
        switch (id)
        {
          case BMend:
            key = scanEnviromentName(str,len,pos);
            id = lookupBeamerID(key);
            if (id == type())
              return ;
            break;

          case BMbegin:
            key = scanEnviromentName(str,len,pos);
            obj = new XWBeamerColumnEnv(doc, this);
            break;

          default:
            obj = new XWBeamerColumn(doc, this);
            break;
        }
      }
      last->insertAtBefore(obj);
      obj->scan(str,len,pos);
    }
  }
}

XWBeamerSemiVerbatim::XWBeamerSemiVerbatim(XWTeXDocument * docA, QObject * parent)
:XWLTXEnviroment(BMsemiverbatim, docA, "\\begin{semiverbatim}", parent)
{
  add(new XWBeamerCode(docA, tr("enviroment contemts:"), this));
  add(new XWTeXControlSequence(docA, true, "\\end{semiverbatim}", this));
}

void XWBeamerSemiVerbatim::breakPage(double & curx,double & cury,bool & firstcolumn)
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

XWBeamerMovie::XWBeamerMovie(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMmovie,docA,true, "\\movie",parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("options:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("movie label:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("text:"), this));
}

XWBeamerHyperLinkMovie::XWBeamerHyperLinkMovie(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMhyperlinkmovie,docA,true, "\\hyperlinkmovie",parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("options:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("movie label:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("text:"), this));
}

XWBeamerAnimateValue::XWBeamerAnimateValue(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BManimatevalue,docA,true, "\\animatevalue",parent)
{
  add(new XWTeXDocumentBeamerSpec(docA,tr("start slide - end slide:"),this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("name:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("start value:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("end value:"), this));
}

XWBeamerMultiInclude::XWBeamerMultiInclude(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMmultiinclude,docA,true, "\\multiinclude",parent)
{
  add(new XWBeamerDefaultSpec(docA,tr("default overlay specification:"),this));
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("options:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("base file name:"), this));
}

XWBeamerSound::XWBeamerSound(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMsound,docA,true, "\\sound",parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("options:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("sound poster text:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("sound filename:"), this));
}

XWBeamerHyperLinkSound::XWBeamerHyperLinkSound(XWTeXDocument * docA, QObject * parent)
:XWTeXDocumentCommand(BMhyperlinksound,docA,true, "\\hyperlinksound",parent)
{
  add(new XWTeXDocumentOption(TEX_DOC_OPTION,docA, tr("options:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("sound label:"), this));
  add(new XWTeXDocumentParam(TEX_DOC_PARAM,docA, tr("text:"), this));
}
