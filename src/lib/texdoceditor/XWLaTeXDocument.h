/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWLATEXDOCUMENT_H
#define XWLATEXDOCUMENT_H

#include "XWTeXDocument.h"

#define XW_LTX_DOC_CLS_ARTICLE 0
#define XW_LTX_DOC_CLS_BOOK    1
#define XW_LTX_DOC_CLS_REPORT  2
#define XW_LTX_DOC_CLS_LETTER  3
#define XW_LTX_DOC_CLS_BEAMER  4

#define XW_LTX_INS_TEXT    0
#define XW_LTX_INS_OBJECTS 1
#define XW_LTX_INS_PAR     2
#define XW_LTX_INS_ITEM    3
#define XW_LTX_INS_BIBITEM 4
#define XW_LTX_INS_SEC     5
#define XW_LTX_INS_FRAME   6

class XWTeXDocumentComment;
class XWLTXDocumentClass;
class XWLTXDocumentTitle;
class XWLTXDocumentAuthor;
class XWLTXDocumentName;
class XWLTXDocumentSignature;
class XWLTXDocumentDate;
class XWLTXDocumentThanks;
class XWLTXDocumentLocation;
class XWLTXDocumentTelephone;
class XWLTXDocumentSectioning;
class XWLTXDocumentAddress;
class XWBeamerSubtitle;
class XWBeamerInstitute;
class XWLTXDocumentEnviroment;

class XW_TEXDOCUMENT_EXPORT XWLaTeXDocument: public XWTeXDocument
{
  Q_OBJECT

public:
  XWLaTeXDocument(QObject * parent = 0);
  virtual ~XWLaTeXDocument();

  void alignEnv(int id);

  void back();
  void beamerBlock(int id);
  void beamerBoxed(int id);
  void beamerDynChange(int id);
  void beamerColumns();
  void breakPage();

  bool canFigure();
  bool canFigureStar();
  bool canFrameSec();
  bool canInsertFromMimeData(const QMimeData * source,
                                        int pageno,
                                    const QPointF & p);
  bool canInsertObject();
  bool canMatter();
  bool canPaste();
  bool canRemoveFloat();
  bool canTable();
            QMimeData * createMimeDataFromSelection();
            QMenu * createContextMenu();
  XWTeXDocumentObject * createObject(const QString & key,
                                             const QString & str,
                                             int & pos,
                                             QObject * parent);
  XWTeXDocumentObject * createObject(int id, QObject * parent);

          void del();

      XWLTXDocumentAuthor * getAuthor() {return authorM;}
          int getClassIndex() {return classIndex;}
          int geCurrentBlockType();
          int getCurrenrObjectType();
      XWTeXDocumentObject * getCurrentPart() {return curPart;}
      XWTeXDocumentObject * getCurrentChapter() {return curChapter;}
      XWTeXDocumentObject * getCurrentSection() {return curSection;}
      XWTeXDocumentObject * getCurrentSubsection() {return curSubsection;}
      XWTeXDocumentObject * getCurrentSubsubsection() {return curSubsubsection;}
      XWTeXDocumentObject * getCurrentParagraph() {return curParagraph;}
      XWTeXDocumentObject * getCurrentSubparagraph() {return curSubparagraph;}
      XWLTXDocumentDate * getDate() {return dateM;}
      QString getFormular();
      XWBeamerInstitute * getInstitute() {return instituteM;}
      XWLTXDocumentSignature * getSignature() {return signatureM;}
      XWBeamerSubtitle * getSubtitle() {return subtitleM;}
      QString getTikzPicture();
      XWLTXDocumentTitle * getTitle() {return titleM;}


          bool hasAddress() {return addressM != 0;}
          bool hasAuthor() {return authorM != 0;}
          bool hasDate() {return dateM != 0;}
          bool hasLocation() {return locationM != 0;}
          bool hasName() {return nameM != 0;}
          bool hasSignature() {return signatureM != 0;}
          bool hasTelephone() {return telephoneM != 0;}
          bool hasThanks() {return thanksM != 0;}
          bool hasTitle() {return titleM != 0;}
          bool hitTest(int pageno,const QPointF & p);

          void insertFromMimeData(const QMimeData * source,
                                                int pageno,
                                                const QPointF & p);
          bool isFramePar();
          bool isMainFile();
          bool isShowSection(XWLTXDocumentSectioning * sec) {return showSectioning==sec;}
          bool isTopObject(XWTeXDocumentObject * obj);
          void itemEnv(int id);

          virtual void keyInput(const QString & str);

          void matter(int id);

          void newParagraph();

          void parEnv(int id);

          void replaceFormular(const QString & formular);
          void resetSides();

          void setBeamerClass();
          void setBookClass() {classIndex = XW_LTX_DOC_CLS_BOOK;}
          void setClassIndex(int idx);
          void setCurrentPart(XWTeXDocumentObject * obj) {curPart = obj;}
          void setCurrentChapter(XWTeXDocumentObject * obj) {curChapter = obj;}
          void setCurrentSection(XWTeXDocumentObject * obj) {curSection = obj;}
          void setCurrentSubsection(XWTeXDocumentObject * obj) {curSubsection = obj;}
          void setCurrentSubsubsection(XWTeXDocumentObject * obj) {curSubsubsection = obj;}
          void setCurrentParagraph(XWTeXDocumentObject * obj) {curParagraph = obj;}
          void setCurrentSubparagraph(XWTeXDocumentObject * obj) {curSubparagraph = obj;}
          void section(int id);
          void setLetterClass() {classIndex = XW_LTX_DOC_CLS_LETTER;}


public slots:
  void copy();
  void cut();

  void paste();

  void address();
  void author();
  void date();
  void includeOnly();
  void insertFormular(const QString & str);
  void insertTikz(const QString & str);
  void location();
  void name();
  void newCommand();
  void newCounter();
  void newEnvironment();
  void newFont();
  void newLength();
  void newTheorem();
  void renewCommand();
  void renewEnvironment();
  void signature();
  void telephone();
  void thanks();
  void title();
  void subtitle();
  void institute();
  void usePackage();
  void tikzUseTikzLibrary();
  void useTheme();
  void includeOnlyLecture();

  void delFrame();
  void insertObject(QAction * a);
  void insertFrameSec(QAction * a);
  void insertFrameAtAfter();
  void insertFrameAtBefore();
  void insertSection(QAction * a);

  void maketitle();
  void titlePage();
  void tableOfContents();
  void abstract();
  void appendix();
  void printIndex();
  void printGlossary();
  void thebibliography();

  void insertQuote();
  void insertQuotation();
  void insertTheorem();
  void insertProof();
  void insertAssumption();
  void insertDefinition();
  void insertProposition();
  void insertLemma();
  void insertAxiom();
  void insertCorollary();
  void insertConjecture();
  void insertExercise();
  void insertExample();
  void insertRemark();
  void insertProblem();

  void insertCenter();
  void insertFlushLeft();
  void insertFlushRight();

  void insertVerse();
  void verse();

  void insertDescription();
  void insertEnumerate();
  void insertItemize();
  void insertList();

  void array();
  void cite();
  void eqnarray();
  void eqnarrayStar();
  void equation();
  void footnote();
  void glossary();
  void include();
  void includeGraphics();
  void index();
  void input();
  void label();
  void marginpar();
  void miniPage();
  void nocite();
  void pageref();
  void picture();
  void pstricksPicture();
  void ref();
  void tabular();
  void tabularStar();
  void tikz();
  void tikzPicture();
  void verbatim();

  void figure();
  void figureStar();
  void table();
  void removeFloat();

  void star();
  void removeStar();
  void editShort();
  void closeShort();

  void tabularToStar();
  void starToTabular();

  void eqnarrayToStar();
  void starToEqnarray();

protected:
  void deleteObjs();

  void getSelectedObjs();
  void getSelectedPars();

  void insert(const QString & str);
  void insertObject(int id);

  void makeLetterHead(double & curx,double & cury,bool & firstcolumn);
  void makeTitle(double & curx,double & cury,bool & firstcolumn);

  void setup(const QString & str, int & len, int & pos);

protected:
  int classIndex;

  XWLTXDocumentSectioning * showSectioning;

  XWTeXDocumentComment * firstComment;
  XWLTXDocumentClass * documentClass;

  XWLTXDocumentTitle * titleM;
  XWLTXDocumentAuthor * authorM;
  XWLTXDocumentDate * dateM;
  XWLTXDocumentThanks * thanksM;

  XWLTXDocumentName * nameM;
  XWLTXDocumentSignature * signatureM;
  XWLTXDocumentTelephone * telephoneM;
  XWLTXDocumentLocation * locationM;
  XWLTXDocumentAddress * addressM;

  XWBeamerSubtitle * subtitleM;
  XWBeamerInstitute * instituteM;

  XWLTXDocumentEnviroment * documentEnv;

  XWTeXDocumentObject * curPart;
  XWTeXDocumentObject * curChapter;
  XWTeXDocumentObject * curSection;
  XWTeXDocumentObject * curSubsection;
  XWTeXDocumentObject * curSubsubsection;
  XWTeXDocumentObject * curParagraph;
  XWTeXDocumentObject * curSubparagraph;
};

#endif //XWLATEXDOCUMENT_H
