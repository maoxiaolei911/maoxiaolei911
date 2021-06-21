/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWBEAMEROBJECT_H
#define XWBEAMEROBJECT_H

#include "XWLaTeXDocumentObject.h"

#define XW_BEAMER_CODE   -40
#define XW_BEAMER_OPTION -41
#define XW_BEAMER_PARAM  -42
#define XW_BEAMER_MODIFIER  -43
#define XW_BEAMER_COORD  -43

XW_TEXDOCUMENT_EXPORT XWTeXDocumentObject * createBeamerObject(int t, XWTeXDocument * docA, QObject * parent);

class XW_TEXDOCUMENT_EXPORT XWBeamerCode : public XWTeXCode
{
  Q_OBJECT

public:
  XWBeamerCode(XWTeXDocument * docA, const QString & nameA,QObject * parent = 0);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerModifier : public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWBeamerModifier(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void scan(const QString & str, int & len, int & pos);
  void setFont();

public:
  XWTeXDocumentBlock * name;
};

class XW_TEXDOCUMENT_EXPORT XWBeamerSubtitle : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerSubtitle(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerInstitute : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerInstitute(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerTitleGraphic : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerTitleGraphic(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerSubject : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerSubject(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerKeywords : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerKeywords(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerLogo : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerLogo(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerIncludeOnlyLecture : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerIncludeOnlyLecture(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWUseBeamerTemplate : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWUseBeamerTemplate(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWSetBeamerTemplate : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWSetBeamerTemplate(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWIfBeamerColorEmpty : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWIfBeamerColorEmpty(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWUseBeamerColor : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWUseBeamerColor(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWSetBeamerColor : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWSetBeamerColor(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWSetBeamerFont : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWSetBeamerFont(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWUseBeamerFont : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWUseBeamerFont(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerLecture : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerLecture(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerMode : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerMode(XWTeXDocument * docA, QObject * parent = 0);

  void write(QTextStream & strm, int & linelen);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerNode : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerNode(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerAgainFrame : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerAgainFrame(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerFrameEnv : public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWBeamerFrameEnv(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  QString getMimeData();

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);  
  bool isAllSelected();

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);

  void scan(const QString & str, int & len, int & pos);

  void write(QTextStream & strm, int & linelen);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerFrameCmd : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerFrameCmd(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  QString getMimeData();

  bool isAllSelected();

  void scan(const QString & str, int & len, int & pos);
  void scanPar(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerFrameTitle : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerFrameTitle(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerFrameSubitle : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerFrameSubitle(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerFrameZoom : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerFrameZoom(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerTitlePage : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerTitlePage(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerPartPage : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerPartPage(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerOnSlide : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerOnSlide(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerOnly : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerOnly(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerAlt : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerAlt(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerTemporal : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerTemporal(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerOnlyEnv : public XWLTXParEnv
{
  Q_OBJECT

public:
  XWBeamerOnlyEnv(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerAltEnv : public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWBeamerAltEnv(XWTeXDocument * docA, QObject * parent = 0);

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);  

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerOverlayArea : public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWBeamerOverlayArea(XWTeXDocument * docA, QObject * parent = 0);

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);  

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerOverPrint : public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWBeamerOverPrint(XWTeXDocument * docA, QObject * parent = 0);

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);  

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerActionEnv : public XWLTXParEnv
{
  Q_OBJECT

public:
  XWBeamerActionEnv(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerHyperTarget : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerHyperTarget(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerHyperLink : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerHyperLink(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerStructureEnv : public XWLTXParEnv
{
  Q_OBJECT

public:
  XWBeamerStructureEnv(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerAlertEnv : public XWLTXParEnv
{
  Q_OBJECT

public:
  XWBeamerAlertEnv(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerBlockEnv : public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWBeamerBlockEnv(int tA, XWTeXDocument * docA, const QString & nameA, QObject * parent = 0);

  virtual void breakPage(double & curx,double & cury,bool & firstcolumn);

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);  

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);

  virtual void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerBlock : public XWBeamerBlockEnv
{
  Q_OBJECT

public:
  XWBeamerBlock(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerAlertBlock : public XWBeamerBlockEnv
{
  Q_OBJECT

public:
  XWBeamerAlertBlock(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerExampleBlock : public XWBeamerBlockEnv
{
  Q_OBJECT

public:
  XWBeamerExampleBlock(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerBoxed : public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWBeamerBoxed(int tA, XWTeXDocument * docA, const QString & nameA, const QString & pA, QObject * parent = 0);

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);  

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);

  virtual void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerColorBox : public XWBeamerBoxed
{
  Q_OBJECT

public:
  XWBeamerColorBox(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerBoxesRounded : public XWBeamerBoxed
{
  Q_OBJECT

public:
  XWBeamerBoxesRounded(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerColumnEnv : public XWBeamerBoxed
{
  Q_OBJECT

public:
  XWBeamerColumnEnv(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerColumn : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerColumn(XWTeXDocument * docA, QObject * parent = 0);

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);  

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerColumns : public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWBeamerColumns(XWTeXDocument * docA, QObject * parent = 0);

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);  

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerSemiVerbatim: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWBeamerSemiVerbatim(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerMovie : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerMovie(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerHyperLinkMovie : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerHyperLinkMovie(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerAnimateValue : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerAnimateValue(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerMultiInclude : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerMultiInclude(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerSound : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerSound(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerHyperLinkSound : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerHyperLinkSound(XWTeXDocument * docA, QObject * parent = 0);
};

#endif //XWBEAMEROBJECT_H
