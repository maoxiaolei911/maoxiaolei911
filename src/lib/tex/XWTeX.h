/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEX_H
#define XWTEX_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include "XWNumberUtil.h"
#include "TeXKeyWord.h"
#include "XWTeXType.h"

class QIODevice;
class QFile;
class QDataStream;
class XWTeXIODev;

class XWTeX : public QObject
{
	Q_OBJECT
	
public:
	XWTeX(XWTeXIODev * devA, QObject * parent = 0);
	virtual ~XWTeX();
	
	qint32  catCode(qint32 a);
	int     checkConstant();
	
	void compile(const QString & maininputfile, bool etex);
	
	void    enableETeX();
	
	void    fixDateAndTime();
	
	qint32  getcTwo(QIODevice * f);
	qint32  getcTwoLE(QIODevice * f);
	void    getDateAndTime(qint32 & minutesA, 
	                       qint32 & dayA,
          	               qint32 & monthA,  
                      	   qint32 & yearA);
	qint32  getFileMode(QIODevice * f, qint32 def);
	QString getTexString(qint32 s);
	
	void   initialize();
	void   initInput();
	void   initOutput();
	bool   initPrim();
	bool   isIniVersion() {return ini_version;}	
	bool   isOk();
	bool   isReadyAlready() {return ready_already == 314159;}
	
	bool  loadFmt(const QString & fmtname);
	bool  loadFmt();
	bool  loadPoolFile(const QString & filename);
	
	qint32 makeTexString(const QString & s);
	
	void parseInterActionOption(const char * optarg);
	void parseSrcSpecialsOption(const char * opt_list);
	
	bool setBuffer(const QString & s);
	void setDebugFormatFile(bool e) {debug_format_file = e;}
	void setFileLineErrorStyle(bool e) {file_line_error_style_p = e;}
	void setHaltOnError(bool e) {halt_on_error_p = e;}
	void setInitVersion(bool e) {ini_version = e;}
	void setInsertSrcSpecialAuto(bool e) {insert_src_special_auto = e;}
	void setInsertSrcSpecialEveryPar(bool e) {insert_src_special_every_par = e;}
	void setInsertSrcSpecialEveryParEnd(bool e) {insert_src_special_every_parend = e;}
	void setInsertSrcSpecialEveryCr(bool e) {insert_src_special_every_cr = e;}
	void setInsertSrcSpecialEveryMath(bool e) {insert_src_special_every_math = e;}
	void setInsertSrcSpecialEveryHBox(bool e) {insert_src_special_every_hbox = e;}
	void setInsertSrcSpecialEveryVBox(bool e) {insert_src_special_every_vbox = e;}
	void setInsertSrcSpecialEveryDisplay(bool e) {insert_src_special_every_display = e;}
	void setInteractionOption(qint32 i) {interaction_option = i;}
	void setOutputComment(const QString & s);
	void setParseFirstLine(bool e) {parse_first_line_p = e;}
	void setShellEnabled(bool e) {shell_enabled_p = e;}
	void setSrcSpecialsP(bool e) {src_specials_p = e;}
	bool setupBound(const QString & fmtname);
	
private:
	qint32  aboveDisplayShortSkip();
	qint32  aboveDisplaySkip();	
	void    activeCompile();
	void    addOcpStack(qint32 min_index, qint32 min_value);
	qint32  addOrSub(qint32 x, 
	                 qint32 y, 
	                 qint32 max_answer, 
	                 bool negative);
	qint32  adjDemerits();
	void    afterMath();
	void    alignError();
	void    alignPeek();
	void    allocateFontTable(qint32 font_number, qint32 font_size);
	void    allocateOcpTable(qint32 ocp_number, qint32 ocp_size);
	void    alterAux();
	void    alterBoxDimen();
	void    alterInteger();
	void    alterPageSoFar();
	void    alterPrevGraf();
	void    appendChar(qint32 c);
	void    appendChoices();
	void    appendDiscretionary();
	void    appendGlue();
	void    appendItalicCorrection();
	void    appendKern();
	void    appendLocalBox(qint32 kind);
	void    appendPenalty();
	void    appendSrcSpecial();
	void    appendToVList(qint32 b);
	void    appSpace();
	
#ifdef XW_TEX_STAT
	void   assignTrace(qint32 p, const QString & s);
#endif //XW_TEX_STAT
	
	void   backError();
	void   backList(qint32 p);
	void   backInput();
	qint32 badness(qint32 t, qint32 s);
	qint32 baseLineSkip();
	void   beginBox(qint32 box_context);
	void   beginDiagnostic();
	void   beginFileReading();
	void   beginInsertOrAdjust();
	void   beginName();
	void   beginTokenList(qint32 p, qint16 t);
	qint32 belowDisplaySkip();
	qint32 belowDisplayShortSkip();
	qint32 binOpPenalty();
	qint32 bodyDirection();
	qint32 box(qint32 a);
	void   boxEnd(qint32 box_context);
	void   boxError(qint32 n);
	qint32 boxMaxDepth();
	qint32 brokenPenalty();	
	void   bufferOverFlow();
	void   buildChoices();
	void   buildDiscretionary();
	void   buildLocalBox();
	void   buildPage();
	
	void   changeIfLimit(quint16 l, qint32 p);
	qint32 charBox(qint32 f, qint32 c);
	void   charWarning(qint32 f, qint32 c);
	bool   checkFullSaveStack();
	void   checkInterrupt();
		
#ifdef XW_TEX_DEBUG
	void   checkMem(bool print_locs);
#endif //XW_TEX_DEBUG
	
	void   checkOuterValidity();
	void   chrCmd(const QString & s, qint32 chr_code);
	qint32 cleanBox(qint32 p, qint32 s);
	void   clear();
	void   clearForErrorPrompt();
	void   closeFilesAndTerminate();
	qint32 clubPenaltiesPtr();
	qint32 clubPenalty();
	qint32 compressTrie(qint32 p);
	void   conditional();
	void   confusion(const QString & s);
	void   convToks();
	qint32 copyNodeList(qint32 p);	
	qint32 copyOcpList(qint32 list);
	qint32 copyOcpLStack(qint32 llstack);
	qint32 count(qint32 a);
	
	TeXHashWord * createEqtbPos(qint32 p);
	TeXHashWord * createHashPos(qint32 p);
	TeXHashWord * createXEqLevel(qint32 p);
	
	void   csError();
	qint32 curFam();
	qint32 curFont();
	qint32 curLength();
	
	qint32 day();
	
#ifdef XW_TEX_DEBUG
	void debugHelp();
#endif //XW_TEX_DEBUG
	
	qint32 defaultHyphenChar();
	qint32 defaultSkewChar();
	qint32 delCode0(qint32 a);
	qint32 delCode1(qint32 a);
	void   delEqWordDefine(qint32 p, qint32 w, qint32 wone);
	void   deleteLast();
	void   deleteSaRef(qint32 q);
	void   delGeqWordDefine(qint32 p, qint32 w, qint32 wone);
	void   deleteGlueRef(qint32 p);
	void   deleteTokenRef(qint32 p);
	qint32 delimiterFactor();
	qint32 delimiterShortFall();
	qint32 dimen(qint32 a);
	qint32 dimenPar(qint32 a);
	qint32 dirPar(qint32 a);
	qint32 displayIndent();
	qint32 displayWidowPenaltiesPtr();
	qint32 displayWidowPenalty();
	qint32 displayWidth();
	void   doAssignments();
	void   doCharMode();
	void   doCharTranslation();
	void   doClearOcpLists(qint32 a);
	void   doEndV();
	void   doExtension();
	bool   doMarks(qint32 a, qint32 l, qint32 q);
	void   doPopOcpList(qint32 a);
	void   doPushOcpList(qint32 a);
	void   doRegisterCommand(quint16 a);
	qint32 doubleHyphenDemerits();
	void   dviFontDef(qint32 f);
	
	qint32 emergencyStretch();
	void   endDiagnostic(bool blank_line);
	void   endFileReading();
	void   endGraf();
	qint32 endLineChar();
	bool   endLineCharInactive();
	void   endName();
	void   endTokenList();
	void   ensureVBox(qint32 n);
	void   eqDefine(qint32 p, quint16 t, qint32 e);
	void   eqDestroy(TeXMemoryWord & w);
	quint16 eqLevel(qint32 a);
	void   eqSave(qint32 p, qint16 l);
	qint16 eqType(qint32 a);
	qint32 equiv(qint32 a);
	qint32 equiv1(qint32 a);
	void   eqWordDefine(qint32 p, qint32 w);	
	qint32 errHelp();
	void   error();
	qint32 errorContextLines();
	qint32 escapeChar();
	bool   eTeXEnabled(bool b, qint32 j, qint32 k);
	qint32 everyCr();
	qint32 everyDisplay();
	qint32 everyEof();
	qint32 everyHBox();
	qint32 everyJob();
	qint32 everyMath();
	qint32 everyPar();
	qint32 everyVBox();
	qint32 exhyphenPenalty();
	void   expand();	
	void   extraRightBrace();
	
	qint32 famFnt(qint32 a);
	void   fastDeleteGlueRef(qint32 p);
	void   fastGetAvail(qint32 & p);
	void   fatalError(const QString & str);
	void   fetch(qint32 a);
	void   fileWarning();
	qint32 finalHyphenDemerits();	
	void   finalCleanup();
	void   finAlign();
	bool   finCol();
	void   findFontDimen(bool writing);	
	void   findSaElement(qint32 t, qint32 n, bool w);
	qint32 finiteShrink(qint32 p);
	qint32 finMList(qint32 p);
	void   finRow();
	void   firmUpTheLine();
	void   firstFit(qint32 p);
	void   fireUp(qint32 c);
	
	void   fixLanguage();
	qint32 floatingPenalty();
	void   flush();
	void   flushChar();
	void   flushList(qint32 p);
	void   flushMath();
	void   flushNodeList(qint32 p);
	void   flushString();
	qint32 fontIdText(qint32 a);
	void   formatDebug(const QString & str, qint32 x);
	qint32 fract(qint32 x, 
	             qint32 n, 
	             qint32 d, 
	             qint32 max_answer);
	qint32 fractionRule(qint32 t);
	void   freeAvail(qint32 p);
	void   freeNode(qint32 p, qint32 s);
	void   freezePageSpecs(qint32 s);
	
	void   geqDefine(qint32 p, quint16 t, qint32 e);
	void   geqWordDefine(qint32 p, qint32 w);
	qint32 getAvail();
	
	uchar   getByte(QIODevice * f);
	QString getFileName();
	void    giveErrHelp();
	qint32  getJobName();
	void   getNext();
	qint32 getNode(qint32 s);
	qint32 getOutputFile();
	void   getPreambleToken();
	void   getRToken();	
	void   getToken();	
	void   getXOrProtected();
	void   getXToken();
	qint32 globalDefs();
	void   glueError(qint32 & p);
	qint32 gluePar(qint32 a);
#ifdef XW_TEX_STAT
	void   groupTrace(bool e);
#endif //XW_TEX_STAT

	void groupWarning();
	void gsaDef(qint32 p, qint32 e);
	void gsaWDef(qint32 p, qint32 w);
	
	qint32 half(qint32 x);
	void   handleRightBrace();
	qint32 hangAfter();
	qint32 hangIndent();
	bool   hashIsFull() {return (hash_used == TEX_HASH_BASE);}
	qint32 hbadness();
	void   headForVMode();
	qint32 heightPlusDepth(qint32 f, qint32 c);
	void   help0();
	void   help1(const QString & str);
	void   help2(const QString & str);
	void   help3(const QString & str);
	void   help4(const QString & str);
	void   help5(const QString & str);
	void   help6(const QString & str);
	qint32 hfuzz();
	void   hlistOut();
	qint32 hoffset();
	qint32 holdingInserts();
	qint32 hpack(qint32 p, qint32 w, char m);
	qint32 hsize();
	void   hyphenate();
	qint32 hyphenPenalty();
	
	qint32 idLookup(qint32 j, qint32 l);
	void   ifWarning();
	void   indentInHMode();
	void   initAlign();
	void   initCol();
	void   initEqtbEntry(qint32 p, TeXMemoryWord & mw);
	void   initEqtbTable();	
	void   initMath();
	void   initRow();
	void   initSpan(qint32 p);
	void   initTrie();
	bool   inputLn(QIODevice * f);
	void   insError();
	void   insertDollarSign();
	void   insertRelax();
	void   insertSrcSpecial();
	void   insList(qint32 p);
	void   insTheToks();
	qint32 interLinePenaltiesPtr();
	qint32 interLinePenalty();
	void   intError(qint32 n);
	qint32 intPar(qint32 a);
	bool   isCharNode(qint32 p) {return (p >= hi_mem_min);}
	bool   isEmpty(qint32 p) {return (link(p) == TEX_EMPTY_FLAG);}
	bool   isHex(qint32 c) {return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'));}	
	bool   isLastOcp(qint32 llstack_no, qint32 counter);
	bool   isNewSource(qint32 srcfilename, qint32 lineno);
	bool   isNullOcpList(qint32 p) {return (ocp_list_lstack_no(p) == TEX_OCP_MAXINT);}
	bool   isNullOcpLStack(qint32 p)  { return (p == 0); }
	bool   isNodeRunning(qint32 p) {return (p == TEX_NULL_FLAG);}
	void   issueMessage();
	bool   itsAllOver();
		
	void   jumpOut();
	
	qint32 language();
	qint32 lastLineFit();
	qint32 lcCode(qint32 a);
	qint32 leftHyphenMin();
	qint32 leftSkip();
	qint32 length(qint32 s);
	qint32 levelLocalDir();
	void   lineBreak(bool d);
	qint32 linePenalty();
	qint32 lineSkip();
	qint32 lineSkipLimit();
	qint32 localBrokenPenalty();
	qint32 localInterLinePenalty();
	qint32 localLeftBox();
	qint32 localRightBox();
	qint32 looseness();
	
	void   macroCall();
	qint32 mag();	
	void   mainControl();
	void   makeAccent();
	void   makeFraction(qint32 q);
	qint32 makeFullNameString();
	qint32 makeLeftRight(qint32 q, 
	                     qint32 style, 
	                     qint32 max_d, 
	                     qint32 max_h);
	qint32 makeLocalParNode();
	void   makeMark();
	void   makeMathAccent(qint32 q);
	qint32 makeNameString();
	qint32 makeNullOcpList();
	qint32 makeOcpListNode(qint32 llstack, 
	                       qint32 llstack_no, 
	                       qint32 llnext);
	qint32 makeOcpLStackNode(qint32 locp, qint32 llnext);
	qint32 makeOp(qint32 q);
	void   makeOrd(qint32 q);
	void   makeOver(qint32 q);
	void   makeRadical(qint32 q);
	void   makeScripts(qint32 q, qint32 delta);
	qint32 makeSrcSpecial(qint32 srcfilename, qint32 lineno);
	qint32 makeString();
	void   makeUnder(qint32 q);
	void   makeVCenter(qint32 q);
	void   mathAC();
	qint32 mathCode(qint32 a);
	qint32 mathDirection();
	void   mathFraction();
	qint32 mathGlue(qint32 g, qint32 m);
	void   mathKern(qint32 p, qint32 m);
	void   mathLeftRight();
	void   mathLimitSwitch();
	void   mathRadical();
	qint32 mathSurround();
	qint32 maxDeadCycles();
	qint32 maxDepth();
	qint32 medMuSkip();
	void   mlistToHList();
	qint32 month();
	bool   moreName(qint32 c);
	void   movement(qint32 w, qint32 o);
	void   muError();
	qint32 multAndAdd(qint32 n, qint32 x, qint32 y, qint32 max_answer);
	qint32 multIntegers(qint32 n, qint32 x);	
	qint32 muskip(qint32 a);
	
	qint32 newCharacter(qint32 f, qint32 c);
	qint32 newChoice();
	qint32 newDir(qint32 s);
	qint32 newDisc();
	
	quint16        newEqLevel(qint32 a);
	TeXMemoryWord newEqtb(qint32 a);	
	qint32        newEqtbInt(qint32 a);
	qint32        newEqtbSC(qint32 a);
	qint16        newEqType(qint32 a);
	qint32        newEquiv(qint32 a);
	qint32        newEquiv1(qint32 a);
	
	void   newFont(quint16 a);
	qint32 newGlue(qint32 q);	
	void   newGraf(bool indented);
	qint32 newHashNext(qint32 a);
	qint32 newHashText(qint32 a);	
	void   newHyphExceptions();
	void   newIndex(qint32 i, qint32 q);
	bool   newInputLine(QIODevice * f, qint32 themode);
	bool   newInputLn(QIODevice * f, 
	                  qint32 themode, 
	                  qint32 translation, 
	                  bool bypass);
	void   newInteraction();
	qint32 newKern(qint32 w);
	qint32 newLigature(qint32 f, qint32 c, qint32 q);
	qint32 newLigItem(qint32 c);
	qint32 newLineChar();
	qint32 newMath(qint32 w, qint32 s);
	qint32 newNext(qint32 a);
	qint32 newNoad();
	qint32 newNullBox();
	void   newOcp(qint32 a);
	void   newOcpList(qint32 a);
	qint32 newParamGlue(qint32 n);
	void   newPatterns();
	qint32 newPenalty(qint32 m);
	qint32 newRule();
	void   newSaveLevel(qint32 c);
	qint32 newSkipParam(qint32 n);
	qint32 newSpec(qint32  p);
	qint32 newStyle(qint32 s);
	qint32 newTrieOp(qint32 d, qint32 n, qint32 v);
	qint32 newText(qint32 a);
	void   newWhatsIt(qint32 s, qint32 w);
	void   newWriteWhatsIt(qint32 w);
	qint32 newXEqLevel(qint32 a);
	
	void   nodeListDisplay(qint32 p);
	void   noalignError();
	bool   nonDiscardable(qint32 p) {return (type(p) < (quint16)TEX_MATH_NODE);}
	qint32 nolocalDirs();
	qint32 nolocalWhatsits();
	void   normalParagraph();
	void   normalizeSelector();
	char   normMin(qint32 h);
	qint32 nullDelimiterSpace();
	void   numError(qint32 & f);
	qint32 nxPlusY(qint32 n, qint32 x, qint32 y);
	
	void   ocpApplyAdd(qint32 list_entry, 
	                   bool lbefore, 
	                   qint32 locp);
	void   ocpApplyRemove(qint32 list_entry, bool lbefore);
	qint32 ocpEnsureLStack(qint32 list, qint32 llstack_no);
	qint32 ocpIdText(qint32 a);
	qint32 ocpListIdText(qint32 a);
	qint32 ocpRead();
	void   offSave();
	void   omegaFileWrite(qint32 fn, qint32 s);
	void   omegaPrint(qint32 s);
	void   omegaWrite(QIODevice * f, qint32 s);
	void   omitError();
	void   openOrCloseIn();
	void   openLogFile();
	qint32 outputPenalty();
	qint32 outputRoutine();	
	void   outWhat(qint32 p);
	qint32 overbar(qint32 b, qint32 k, qint32 t);
	void   overFlow(const QString & str, qint32 n);
	void   overFlowOcpBufSize();
	void   overFlowOcpStackSize();
	qint32 overFullRule();
	
	void   package(qint32 c);
	void   packBufferedName(qint32 n, qint32 a, qint32 b);
	void   packCurName();
	void   packFileName(qint32 n, qint32 a, qint32 e);
	void   packJobName(qint32 s);
	qint32 pageBottomOffset();
	qint32 pageDirection();
	qint32 pageHeight();
	qint32 pageRightOffset();
	qint32 pageWidth();
	qint32 parDirection();
	qint32 parFillSkip();
	qint32 parIndent();
	qint32 parShapePtr();
	qint32 parSkip();
	void   passText();
	void   pauseForInstructions();
	qint32 pausing();	
	bool   pnewInputLn(QIODevice * f, 
	                   qint32 the_mode, 
	                   qint32 translation, 
	                   bool bypass_eoln);
	void   popAlignment();
	void   popInput();
	void   popNest();
	qint32 postdisplayPenalty();
	void   postLineBreak(bool d);
	bool   precedesBreak(qint32 p) {return (type(p) < (quint16)TEX_MATH_NODE);}	
	qint32 predisplayDirection();
	qint32 predisplaySize();
	qint32 predisplayPenalty();	
	void   prefixedCommand();
	qint32 pretolerance();
	void   prepareMag();
	void   primitive(int s, quint16 c, qint32 o);	
	void   print(const QString & str);
	void   print(qint32 s);
	void   printActiveOcps();
	void   printBanner();
	void   printChar(qint32 s);
	void   printCmdChr(quint16 cmd, qint32 char_code);
	void   printCS(qint32 p);
	void   printCsNames(qint32 hstart, qint32 hfinish);
	void   printCurrentString();
	void   printDelimiter(qint32 p);
	void   printDir(qint32 d);
	void   printErr(const QString & str);	
	void   printEsc(qint32 s);
	void   printEsc(const QString & s);
	void   printFamAndChar(qint32 p);
	void   printFileLineStyleLineAndFile();
	void   printFileName(qint32 n, qint32 a, qint32 e);
	void   printFontAndChar(qint32 p);
	void   printGlue(qint32 d, qint32 order, qint32 s);
	void   printGlue(qint32 d, qint32 order, const QString & s);
	void   printGroup(bool e);
	void   printHex(qint32 n);
	void   printIfLine(qint32 l);
	void   printInt(qint32 n);
	void   printLCHex(qint32 c);
	void   printLengthParam(qint32 n);
	void   println();
	void   printMark(qint32 p);
	void   printMeaning();
	void   printMode(qint32 m);
	void   printnl(qint32 s);
	void   printnl(const QString & s);
	void   printOcpList(qint32 list_entry);
	void   printOcpLStack(qint32 lstack_entry);
	void   printParam(qint32 n);
	void   printRomanInt(qint32 n);
	void   printRuleDimen(qint32 d);
	void   printSaNum(qint32 q);
	void   printScaled(qint32 s);
	void   printSize(qint32 s);
	void   printSkipParam(qint32 n);
	void   printSpec(qint32 p, qint32 s);
	void   printSpec(qint32 p, const QString & s);
	void   printStyle(qint32 c);
	void   printSubsidiaryData(qint32 p, qint32 c);
	void   printTheDigs(char k);
	void   printTotals();
	void   printTwo(qint32 n);
	void   printWriteWhatsIt(qint32 s, qint32 p);
#ifdef XW_TEX_DEBUG
	void   printWord(const TeXMemoryWord & w);
#endif //XW_TEX_DEBUG

	bool   privileged();	
	void   promptFileName(const QString & s, const QString & e, bool w = true);
	void   promptInput(const QString & str);
	void   pruneMovements(qint32 l);
	qint32 prunePageTop(qint32 p, bool s);
	void   pseudoClose();
	bool   pseudoInput();
	void   pseudoStart();
	void   pushAlignment();
	void   pushInput();
	void   pushMath(qint32 c);
	void   pushNest();
	
	qint32 quotient(qint32 n, qint32 d);
	
	qint32 readFontInfo(qint32 u, 
	                    qint32 nom, 
	                    qint32 aire, 
	                    qint32 s,
	                    qint32 offset,
	                    qint32 natural_dir);
	qint32 readOcpInfo(qint32 u, 
	                   qint32 nom, 
	                   qint32 aire, 
	                   qint32 ext, 
	                   bool external_ocp);
	qint32 readOcpList();
	long   readSixteen(QIODevice * f);
  long   readSixteenUnsigned(QIODevice * f);
  long   readThirtyTwo(QIODevice * f);
	void   readToks(qint32 n, qint32 r, qint32 j);
	qint32 rebox(qint32 b, qint32 w);
	qint32 reconstitute(qint32 j, 
	                    qint32 n, 
	                    qint32 bchar, 
	                    qint32 hchar);
	qint32 relPenalty();
	void   rememberSourceInfo(qint32 srcfilename, qint32 lineno);
	void   reportIllegalCase();
	
#ifdef XW_TEX_STAT
	void   restoreTrace(qint32 p, const QString & s);
#endif //XW_TEX_STAT

	void   resumeAfterDisplay();
	qint32 rightHyphenMin();
	qint32 rightSkip();	
	qint32 roundDecimals(char k);	
	void   runaway();
	void   runExternalOcp();
	void   runOTP();
	
	void   saDef(qint32 p, qint32 e);
	void   saDestroy(qint32 p);
	void   saRestore();
	void   saSave(qint32 p);
	void   saveForAfter(qint32 t);
	qint32 savingHyphCodes();
	qint32 savingVDiscards();
	void   saWDef(qint32 p, qint32 w);
	void   scanBigFifteenBitInt();
	void   scanBigFourBitInt();
	void   scanBox(qint32 box_context);
	void   scanCharNum();
	void   scanDelimiter(qint32 p, qint32 r);
	void   scanDimen(bool mu, bool inf, bool shortcut);
	void   scanDir();
	void   scanEightBitInt();
	void   scanExpr();
	void   scanFifteenBitInt();
	void   scanFiftyOneBitInt();
	void   scanFileName();
	void   scanFileReferrent();
	void   scanFontIdent();
	void   scanFourBitInt();
	void   scanFourBitIntOr18();
	void   scanGeneralText();
	void   scanGlue(char level);
	void   scanInt();
	bool   scanKeyWord(qint32 s);
	void   scanLeftBrace();
	void   scanMath(qint32 p);
	void   scanMode();
	void   scanMuGlue();
	void   scanNormalDimen();
	void   scanNormalGlue();
	void   scanOcpIdent();
	qint32 scanOcpList();
	void   scanOcpListIdent();
	void   scanOptionalEquals();
	void   scannedResult(qint32 v, char l);
	void   scanRealFifteenBitInt();
	void   scanRegisterNum();
	void   scanRightBrace();
	qint32 scanRuleSpec();
	void   scanScaled();
	void   scanSomethingInternal(char level, bool negative);
	void   scanSpec(qint32 c, bool three_codes);
	void   scanStringArgument();
	qint32 scanToks(bool macro_def, bool xpand);
	void   scanTwentySevenBitInt();
	qint32 scriptSpace();
	
#ifdef XW_TEX_DEBUG
	void   searchMem(qint32 p);
#endif //XW_TEX_DEBUG
	
	qint32 searchString(qint32 search);
	void   setEqLevel(qint32 a, quint16 v);
	void   setEqType(qint32 a, quint16 v);
	void   setEquiv(qint32 a, qint32 v);
	void   setEquiv1(qint32 a, qint32 v);
	void   setHashNext(qint32 a, qint32 d);
	void   setHashText(qint32 a, qint32 d);
	void   setMathChar(qint32 c);
	void   setNewEqtb(qint32 a, const TeXMemoryWord & v);
	void   setNewEqtbInt(qint32 a, qint32 v);
	void   setNewEqtbSC(qint32 a, qint32 v);
	void   setNewLineChar(qint32 nl);
	void   setNext(qint32 a, qint32 d);
	void   setText(qint32 a, qint32 d);
	void   setTrickCount();
	void   setXEqLevel(qint32 a, qint32 v);
	qint32 sfCode(qint32 a);
	void   shiftCase();
	void   shipOut(qint32 p);
	void   shortDisplay(qint32 p);
	void   showActivities();
	void   showBox(qint32 p);
	qint32 showBoxBreadth();
	qint32 showBoxDepth();
	void   showContext();
	void   showCurCmdChr();
#ifdef XW_TEX_STAT
	void   showEqtb(qint32 n);
#endif //XW_TEX_STAT
	void   showInfo();
	void   showNodeList(qint32 p);	
#ifdef XW_TEX_STAT
	void   showSa(qint32 p, const QString & s);
#endif //XW_TEX_STAT
	void   showSaveGroups();
	void   showTokenList(qint32 p, qint32 q, qint32 l);
	void   showWhatEver();
	qint32 skip(qint32 a);	
	qint32 slowMakeString();
	void   slowPrint(qint32 s);
	void   sortAvail();
	qint32 spaceSkip();
	void   specialOut(qint32 p);
	qint32 splitMaxDepth();
	qint32 splitTopSkip();
	void   sprintCS(qint32 p);
	void   stackIntoBox(qint32 b, qint32 f, qint32 c);
	void   startEqNo();
	void   startInput();
	void   storeFmtFile();
	bool   strEqBuf(qint32 s, qint32 k);
	bool   strEqStr(qint32 s, qint32 t);
	bool   strRoom(qint32 l);
	qint32 strToks(qint32 b);
	void   subSup();
	void   succumb();
	
	qint32 tabSkip();
	void   tailAppend(qint32 p);
	void   termInput();
	qint32 textDirection();
	qint32 theToks();
	qint32 thickMuSkip();
	qint32 thinMuSkip();
	qint32 time();
	qint32 toks(qint32 a);
	qint32 tolerance();	
	void   tokenShow(qint32 p);
	qint32 topSkip();
	qint32 tracingAssigns();
	qint32 tracingCommands();
	qint32 tracingGroups();
	qint32 tracingIfs();
	qint32 tracingLostChars();
	qint32 tracingMacros();
	qint32 tracingNesting();
	qint32 tracingOnline();
	qint32 tracingOutput();
	qint32 tracingPages();
	qint32 tracingParagraphs();
	qint32 tracingRestores();
	qint32 tracingScanTokens();
	qint32 tracingStats();
	void   trapZeroGlue();
	void   trieFix(qint32 p);
	qint32 trieNode(qint32 p);
	void   triePack(qint32 p);
	void   tryBreak(qint32 pi, char break_type);
	
	qint32 ucCode(qint32 a);
	qint32 ucHyph();
	bool   undumpSize(QDataStream & streamA,
	                  qint32 min, 
	                  qint32 v, 
	                  const QString & str, 
	                  qint32 & y);
	void   ungetcTwo(qint32 c, QIODevice * f);
	void   ungetcTwoLE(qint32 c, QIODevice * f);
	void   unpackage();
	void   unsave();
	
	qint32 varDelimiter(qint32 d, qint32 s, qint32 v);
	qint32 vbadness();
	qint32 vertBreak(qint32 p, qint32 h, qint32 d);
	qint32 vfuzz();
	void   vlistOut();
	qint32 voffset();
	qint32 vpack(qint32 p, qint32 h, char m);
	qint32 vpackage(qint32 p, qint32 h, char m, qint32 l);
	qint32 vsize();
	qint32 vsplit(qint32 n, qint32 h);
	
	qint32 widowPenaltiesPtr();
	qint32 widowPenalty();
	void   writeOut(qint32 p);
	
	qint32 xeqLevel(qint32 a);
	qint32 xnOverD(qint32 x, qint32 n, qint32 d);
	qint32 xOverN(qint32 x, qint32 n);
	qint32 xspaceSkip();
	void   xToken();
	
	qint32 year();
	void   youCant();
		
private:
	XWTeXIODev * dev;
	QIODevice * fmt_file;
	bool   ini_version;
	
	qint32 main_memory;
	qint32 extra_mem_bot;
	qint32 mem_max;
	qint32 mem_min;	
	qint32 extra_mem_top;
	qint32 buf_size;
	qint32 error_line;
	qint32 half_error_line;	
	qint32 max_print_line;
	qint32 ocp_list_size;
	qint32 ocp_buf_size;
	qint32 ocp_stack_size;
	qint32 stack_size;	
	qint32 max_in_open;
	qint32 param_size;
	qint32 nest_size;
	qint32 max_strings;
	qint32 strings_free;
	qint32 string_vacancies;
	qint32 pool_size;
	qint32 pool_free;
	qint32 save_size;
	qint32 trie_size;	
	qint32 trie_op_size;
	qint32 neg_trie_op_size;
	bool quoted_filename;
	qint32 special_loc;
	qint32 special_token;
	
	bool parse_first_line_p;
	bool file_line_error_style_p;
	bool halt_on_error_p;
	bool src_specials_p;
	bool insert_src_special_auto;
	bool insert_src_special_every_par;
	bool insert_src_special_every_parend;
	bool insert_src_special_every_cr;
	bool insert_src_special_every_math;
	bool insert_src_special_every_hbox;
	bool insert_src_special_every_vbox;
	bool insert_src_special_every_display;
	
	uchar xchr[256];
	
	qint32 * name_of_file;
	int      name_length;
	bool     stop_at_space;
	
	int tex_input_type;
	
	qint32 * buffer;
	qint32   first;
	qint32   last;
	qint32   max_buf_stack;	
	qint32   term_in_mode;
	qint32   term_in_translation;
	qint32 * buffered_cs;
	qint32   buffered_cs_len;
	
	qint32 * str_pool;
	qint32 * str_start_ar;
	qint32   pool_ptr;
	qint32   init_pool_ptr;
	qint32   str_ptr;
	qint32   init_str_ptr;
	
	int    term_out_mode;
	int    term_out_translation;
	int    selector;
	int    old_setting;
	qint32 dig[25];
	qint32 tally;
	qint32 term_offset;
	qint32 file_offset;
	qint32 * trick_buf;
	qint32 trick_count;
	qint32 first_count;
	
	qint32  interaction;
	qint32  init_interaction;
	qint32  interaction_option;
	bool deletions_allowed;
	bool set_box_allowed;
	int  history;
	int  error_count;
	
	QVector<QString> help_line;
	int  help_ptr;
	bool use_err_help;
	
	qint32 interrupt;
	bool   OK_to_interrupt;
		
	qint32 mem_bot;
	qint32 mem_top;	
	qint32 hyph_size;
		
	bool arith_error;
	qint32 remainder;
	
	qint32 temp_ptr;
	
	TeXMemoryWord * yzmem;
	TeXMemoryWord * zmem;
	TeXMemoryWord * mem;	
	qint32 lo_mem_max;
	qint32 hi_mem_min;
	
	qint32 var_used;
	qint32 dyn_used;
	
	qint32 avail;
	qint32 mem_end;
	
	qint32 rover;
	
#ifdef XW_TEX_DEBUG
	bool * mfree;
	bool * was_free;
	qint32 was_mem_end;
	qint32 was_lo_max;
	qint32 was_hi_min;
	bool panicking;
#endif //XW_TEX_DEBUG
	
	qint32 font_in_short_display;
	
	qint32 depth_threshold;
	qint32 breadth_max;
	
	TeXListStateRecord * nest;
	qint32 nest_ptr;
	qint32 max_nest_stack;
	TeXListStateRecord cur_list;
	qint32 shown_mode;
	
	TeXHashWord * hashtable;
	
	qint32 hash_used;
	bool no_new_control_sequence;
	qint32 cs_count;
	
	TeXMemoryWord * save_stack;
	qint32 save_ptr;
	qint32 max_save_stack;
	qint32 cur_level;
	qint32 cur_group;
	qint32 cur_boundary;
	
	qint32 cur_cmd;
	qint32 cur_chr;
	qint32 cur_cs;
	qint32 cur_tok;
	
	qint32 mag_set;
	
	TeXInStateRecord * input_stack;
	qint32 input_ptr;
	qint32 max_in_stack;
	TeXInStateRecord cur_input;
	
	qint32 in_open;
	qint32 open_parens;
	
	QIODevice ** input_file;
	qint32     * input_file_mode;
	qint32     * input_file_translation;
	qint32       line;
	qint32     * line_stack;
	qint32     * source_filename_stack;
	qint32     * full_source_filename_stack;
	
	char   scanner_status;
	qint32 warning_index;
	qint32 def_ref;
	
	qint32 * param_stack;
	qint32   param_ptr;
	qint32   max_param_stack;
	
	qint32 base_ptr;
		
	qint32 align_state;
	
	qint32 par_loc;
	qint32 par_token;
	
	bool force_eof;
	
	qint32 cur_mark[TEX_SPLIT_BOT_MARK_CODE + 1];
	
	qint16 long_state;
	
	qint32 pstack[9];
	
	qint32 cur_val;
	qint32 cur_val1;
	qint32 cur_val_level;
	
	qint32 radix;
	
	qint32 cur_order;
	
	QIODevice * read_file[17];
	qint32      read_file_mode[17];
	qint32      read_file_translation[17];
	char        read_open[18];
	
	qint32 cond_ptr;
	qint32 if_limit;
	qint32 cur_if;
	qint32 if_line;
	
	qint32 skip_line;
	
	qint32 cur_name;
	qint32 cur_area;
	qint32 cur_ext;
	
	qint32 area_delimiter;
	qint32 ext_delimiter;
	
	qint32 TEX_format_default[256];
	
	bool   name_in_progress;
	qint32 job_name;
	qint32 output_file_name;
	bool   log_opened;
	
	qint32 font_ptr;
	
	qint32 dimen_font;
	
	TeXMemoryWord ** fonttables;
	qint32           font_entries;
	
	TeXFourQuarters null_character;
	
	TeXMemoryWord * active_info;
	qint32 active_min_ptr;
	qint32 active_max_ptr;
	qint32 active_real;
	qint32 * holding;
	
	QIODevice * ocp_file;
	
	qint32 ocp_ptr;
	
	qint32 ** ocptables;
	qint32 ocp_entries;
	
	TeXMemoryWord * ocp_list_info;
	qint32          ocp_listmem_ptr;
	qint32          ocp_listmem_run_ptr;
	TeXMemoryWord * ocp_lstack_info;
	qint32          ocp_lstackmem_ptr;
	qint32          ocp_lstackmem_run_ptr;
	qint32          ocp_list_ptr;
	qint32        * ocp_list_list;
		
	qint32 total_pages;
	qint32 max_v;
	qint32 max_h;
	qint32 max_push;
	qint32 last_bop;
	qint32 dead_cycles;
	bool   doing_leaders;
	qint32 ch;
	qint32 fnt;
	qint32 oval;
	qint32 ocmd;
	qint32 rule_ht;
	qint32 rule_dp;
	qint32 rule_wd;
	qint32 glue;
	qint32 lq;
	qint32 lr;
	
	qint32 down_ptr;
	qint32 right_ptr;
	
	qint32 dvi_h;
	qint32 dvi_v;
	qint32 cur_h;
	qint32 cur_v;
	qint32 dvi_f;
	qint32 cur_s;
	
	qint32 pack_direction;
	qint32 spec_direction;
	
	qint32 total_stretch[6];
	qint32 total_shrink[6];
	
	qint32 last_badness;
	
	qint32 adjust_tail;
	
	qint32 pack_begin_line;
	
	TeXTwoHalves empty_field;
	TeXFourQuarters null_delimiter;
	
	qint32 cur_mlist;
	qint32 cur_style;
	qint32 cur_size;
	qint32 cur_mu;
	bool   mlist_penalties;
	
	qint32 cur_f;
	qint32 cur_c;
	TeXFourQuarters cur_i;
	
	qint32 magic_offset;
	
	qint32 cur_align;
	qint32 cur_span;
	qint32 cur_loop;
	qint32 align_ptr;
	qint32 cur_head;
	qint32 cur_tail;
	
	qint32 just_box;
	qint32 internal_pen_inter;
	qint32 internal_pen_broken;
	qint32 internal_left_box;
	qint32 internal_left_box_width;
	qint32 init_internal_left_box;
	qint32 init_internal_left_box_width;
	qint32 internal_right_box;
	qint32 internal_right_box_width;
	qint32 paragraph_dir;
	qint32 line_break_dir;
	qint32 break_c_htdp;
	qint32 temp_no_whatsits;
	qint32 temp_no_dirs;
	qint32 temporary_dir;
	qint32 dir_ptr;
	qint32 dir_tmp;
	qint32 dir_rover;
	
	qint32 passive;
	qint32 printed_node;
	qint32 pass_number;
	qint32 final_par_glue;
	
	qint32 active_width[10];
	qint32 cur_active_width[10];
	qint32 background[10];
	qint32 break_width[10];
	
	bool no_shrink_error_yet;
	
	qint32 cur_p;
	bool   second_pass;
	bool   final_pass;
	qint32 threshold;
	
	qint32 minimal_demerits[6];
	qint32 minimum_demerits;
	qint32 best_place[6];
	qint32 best_pl_line[6];
	
	qint32 disc_width;
	
	qint32 easy_line;
	qint32 last_special_line;
	qint32 first_width;
	qint32 second_width;
	qint32 first_indent;
	qint32 second_indent;
	
	qint32 best_bet;
	qint32 fewest_demerits;
	qint32 best_line;
	qint32 actual_looseness;
	qint32 line_diff;
	
	qint32 hc[68];
	qint32 hn;
	qint32 ha;
	qint32 hb;
	qint32 hf;
	qint32 hu[68];
	qint32 hyf_char;
	qint32 cur_lang;
	qint32 init_cur_lang;
	qint32 l_hyf;
	qint32 r_hyf;
	qint32 init_l_hyf;
	qint32 init_r_hyf;
	qint32 hyf_bchar;
	qint32 max_hyph_char;
	
	qint32 hyf[68];
	qint32 init_list;
	bool   init_lig;
	bool   init_lft;
	
	qint32 hyphen_passed; 
	
	qint32 cur_l;
	qint32 cur_r;
	qint32 cur_q;
	qint32 lig_stack;
	bool   ligature_present;
	bool   lft_hit;
	bool   rt_hit;
	
	TeXTwoHalves * trie;
	qint32 * hyf_distance;
	qint32 * hyf_num;
	qint32 * hyf_next;
	qint32 * op_start;
	
	qint32 * hyph_word;
	qint32 * hyph_list;
	qint32 * hyph_link;
	qint32   hyph_count;
	qint32   hyph_next;
	
	qint32 * zzzaa;
	qint32 * trie_used;
	qint32 * trie_op_lang;
	qint32 * trie_op_val;
	qint32   trie_op_ptr;
	
	qint32 * trie_c;
	qint32 * trie_o;
	qint32 * trie_l;
	qint32 * trie_r;
	qint32   trie_ptr;
	qint32 * trie_hash;
	
	bool   * trie_taken;
	qint32 * trie_min;
	qint32   trie_max;
	bool     trie_not_ready;
	
	qint32 best_height_plus_depth;
	
	qint32 page_tail;
	qint32 page_contents;
	qint32 page_max_depth;
	qint32 best_page_break;
	qint32 least_page_cost;
	qint32 best_size;
	
	qint32 page_so_far[10];
	qint32 last_glue;
	qint32 last_penalty;
	qint32 last_kern;
	qint32 last_node_type;
	qint32 insert_penalties;
	
	bool output_active;
	
	qint32 main_f;
	TeXFourQuarters main_i;
	TeXFourQuarters main_j;
	qint32 main_k;
	qint32 main_p;
	qint32 main_s;
	qint32 bchar;
	qint32 false_bchar;
	bool   cancel_boundary;
	bool   left_ghost;
	bool   right_ghost;
	bool   new_left_ghost;
	bool   new_right_ghost;
	bool   ins_disc;
	qint32 k_glue;
	qint32 font_glue_spec;
	
	qint32 cur_box;
	
	qint32 after_token;
	
	bool long_help_seen;
	
	qint32 format_ident;
		
	qint32 ready_already;
	
	qint32 * outside_string_array;
	
	QFile     * write_file[17];
	qint32      write_file_mode[17];
	qint32      write_file_translation[17];
	bool        write_open[19];
	qint32      neg_wd;
	qint32      pos_wd;
	qint32      neg_ht;
	qint32      dvi_direction;
	qint32      dir_primary[35];
	qint32      dir_secondary[35];
	qint32      dir_tertiary[35];
	qint32      dir_rearrange[5];
	qint32      dir_names[5];
	qint32      text_dir_ptr;
	qint32      text_dir_tmp;
	
	qint32 write_loc;
	
	char eTeX_mode;
	
	bool * eof_seen;
	
	qint32 pseudo_files;
	
	qint32 * grp_stack;
	qint32 * if_stack;
	
	qint32 max_reg_num;
	QString max_reg_help_line;
	
	qint32 sa_mark;
	qint32 cur_ptr;	
	TeXMemoryWord sa_null;
	
	qint32 sa_chain;
	qint32 sa_level;
	
	qint32 last_line_fill;
	bool   do_last_line_fit;
	qint32 active_node_size;
	qint32 fill_width[5];
	qint32 best_pl_short[5];
	qint32 best_pl_glue[5];
	
	qint32 hyph_start;
	qint32 hyph_index;
	
	qint32 disc_ptr[6];
	
	qint32 edit_name_start;
	qint32 edit_name_length;
	qint32 edit_line;
	
	qint32 save_str_ptr;
	qint32 save_pool_ptr;
	bool   shell_enabled_p;
	char * output_comment;
	
	bool debug_format_file;
	
	qint32 otp_init_input_start;
	qint32 otp_init_input_last;
	qint32 otp_init_input_end;
	qint32 otp_i;
	qint32 * otp_init_input_buf;
	
	qint32 otp_input_start;
	qint32 otp_input_last;
	qint32 otp_input_end;
	qint32 * otp_input_buf;
	
	qint32 otp_output_end;
	qint32 * otp_output_buf;
	
	qint32 otp_stack_used;
	qint32 otp_stack_last;
	qint32 otp_stack_new;
	qint32 * otp_stack_buf;
	
	qint32 otp_pc;
	
	qint32 otp_calc_ptr;
	qint32 * otp_calcs;
	qint32 otp_state_ptr;
	qint32 * otp_states;
	
	qint32 otp_input_char;
	qint32 otp_calculated_char;
	qint32 otp_no_input_chars;
	qint32 otp_instruction;
	qint32 otp_instr;
	qint32 otp_arg;
	qint32 otp_first_arg;
	qint32 otp_second_arg;
	qint32 otp_input_ocp;
	qint32 otp_counter;
	
	bool   otp_finished;
	qint32 otp_ext_str;
	qint32 otp_ext_str_arg;
	qint32 otp_ext_i;
	
	QString last_source_name;
	qint32  last_lineno;
	
	bool reloadFmt;
};

#ifdef XW_TEX_STAT
inline void XWTeX::assignTrace(qint32 p, const QString & s)
{
	if (tracingAssigns() > 0)
		restoreTrace(p, s);
}
#endif //XW_TEX_STAT

inline qint32 XWTeX::aboveDisplayShortSkip()
{
	return gluePar(TEX_ABOVE_DISPLAY_SHORT_SKIP_CODE);
}

inline qint32 XWTeX::aboveDisplaySkip()
{
	return gluePar(TEX_ABOVE_DISPLAY_SKIP_CODE);
}

inline qint32 XWTeX::adjDemerits()
{
	return intPar(TEX_ADJ_DEMERITS_CODE);
}

inline void XWTeX::appendChar(qint32 c)
{
	str_pool[pool_ptr] = c; 
	pool_ptr++;
}

inline void XWTeX::backList(qint32 p)
{
	beginTokenList(p, TEX_BACKED_UP);
}

inline qint32 XWTeX::baseLineSkip()
{
	return gluePar(TEX_BASELINE_SKIP_CODE);
}

inline void XWTeX::beginName()
{
	area_delimiter = 0; 
	ext_delimiter  = 0;
	quoted_filename = false;
}

inline qint32 XWTeX::belowDisplaySkip()
{
	return gluePar(TEX_BELOW_DISPLAY_SKIP_CODE);
}

inline qint32 XWTeX::belowDisplayShortSkip()
{
	return gluePar(TEX_BELOW_DISPLAY_SHORT_SKIP_CODE);
}

inline qint32 XWTeX::binOpPenalty()
{
	return intPar(TEX_BIN_OP_PENALTY_CODE);
}

inline qint32 XWTeX::bodyDirection()
{
	return dirPar(TEX_BODY_DIRECTION_CODE);
}

inline qint32 XWTeX::box(qint32 a)
{
	return equiv(TEX_BOX_BASE + a);
}

inline qint32 XWTeX::boxMaxDepth()
{
	return dimenPar(TEX_BOX_MAX_DEPTH_CODE);
}

inline qint32 XWTeX::brokenPenalty()
{
	return intPar(TEX_BROKEN_PENALTY_CODE);
}

inline qint32 XWTeX::catCode(qint32 a)
{
	return equiv(TEX_CAT_CODE_BASE + a);
}

inline void XWTeX::chrCmd(const QString & s, qint32 chr_code)
{
	print(s);
	print(chr_code);
}

inline qint32 XWTeX::clubPenaltiesPtr()
{
	return equiv(TEX_CLUB_PENALTIES_LOC);
}

inline qint32 XWTeX::clubPenalty()
{
	return intPar(TEX_CLUB_PENALTY_CODE);
}

inline qint32 XWTeX::count(qint32 a)
{
	return newEqtbInt(TEX_COUNT_BASE + a);
}

inline qint32 XWTeX::curFam()
{
	return intPar(TEX_CUR_FAM_CODE);
}

inline qint32 XWTeX::curFont()
{
	return equiv(TEX_CUR_FONT_LOC);
}

inline qint32 XWTeX::curLength()
{
	return (pool_ptr - str_start(str_ptr));
}

inline qint32 XWTeX::day()
{
	return intPar(TEX_DAY_CODE);
}

inline qint32 XWTeX::defaultHyphenChar()
{
	return intPar(TEX_DEFAULT_HYPHEN_CHAR_CODE);
}

inline qint32 XWTeX::defaultSkewChar()
{
	return intPar(TEX_DEFAULT_SKEW_CHAR_CODE);
}

inline qint32 XWTeX::delCode0(qint32 a)
{
	return newEquiv(TEX_DEL_CODE_BASE + a);
}

inline qint32 XWTeX::delCode1(qint32 a)
{
	return newEquiv1(TEX_DEL_CODE_BASE + a);
}

inline void XWTeX::deleteGlueRef(qint32 p)
{
	fastDeleteGlueRef(p);
}

inline qint32 XWTeX::delimiterFactor()
{
	return intPar(TEX_DELIMITER_FACTOR_CODE);
}

inline qint32 XWTeX::delimiterShortFall()
{
	return dimenPar(TEX_DELIMITER_SHORTFALL_CODE);
}

inline qint32 XWTeX::dimen(qint32 a)
{
	return newEqtbSC(TEX_SCALED_BASE + (a));
}

inline qint32 XWTeX::dimenPar(qint32 a)
{
	return newEqtbSC(TEX_DIMEN_BASE + (a));
}

inline qint32 XWTeX::dirPar(qint32 a)
{
	return newEqtbInt(TEX_DIR_BASE + a);
}

inline qint32 XWTeX::displayIndent()
{
	return dimenPar(TEX_DISPLAY_INDENT_CODE);
}

inline qint32 XWTeX::displayWidowPenaltiesPtr()
{
	return equiv(TEX_DISPLAY_WIDOW_PENALTIES_LOC);
}

inline qint32 XWTeX::displayWidowPenalty()
{
	return intPar(TEX_DISPLAY_WIDOW_PENALTY_CODE);
}

inline qint32 XWTeX::displayWidth()
{
	return dimenPar(TEX_DISPLAY_WIDTH_CODE);
}

inline qint32 XWTeX::doubleHyphenDemerits()
{
	return intPar(TEX_DOUBLE_HYPHEN_DEMERITS_CODE);
}

inline qint32 XWTeX::emergencyStretch()
{
	return dimenPar(TEX_EMERGENCY_STRETCH_CODE);
}

inline qint32 XWTeX::endLineChar()
{
	return intPar(TEX_END_LINE_CHAR_CODE);
}

inline bool XWTeX::endLineCharInactive()
{
	return ((endLineChar() < 0) || (endLineChar() > 255));
}

inline quint16 XWTeX::eqLevel(qint32 a)
{
	return newEqLevel(a);
}

inline qint16 XWTeX::eqType(qint32 a)
{
	return newEqType(a);
}

inline qint32 XWTeX::equiv(qint32 a)
{
	return newEquiv(a);
}

inline qint32 XWTeX::equiv1(qint32 a)
{
	return newEquiv1(a);
}

inline qint32 XWTeX::errHelp()
{
	return equiv(TEX_ERR_HELP_LOC);
}

inline qint32 XWTeX::errorContextLines()
{
	return intPar(TEX_ERROR_CONTEXT_LINES_CODE);
}

inline qint32 XWTeX::escapeChar()
{
	return intPar(TEX_ESCAPE_CHAR_CODE);
}

inline qint32 XWTeX::everyCr()
{
	return equiv(TEX_EVERY_CR_LOC);
}

inline qint32 XWTeX::everyDisplay()
{
	return equiv(TEX_EVERY_DISPLAY_LOC);
}

inline qint32 XWTeX::everyEof()
{
	return equiv(TEX_EVERY_EOF_LOC);
}

inline qint32 XWTeX::everyHBox()
{
	return equiv(TEX_EVERY_HBOX_LOC);
}

inline qint32 XWTeX::everyJob()
{
	return equiv(TEX_EVERY_JOB_LOC);
}

inline qint32 XWTeX::everyMath()
{
	return equiv(TEX_EVERY_MATH_LOC);
}

inline qint32 XWTeX::everyPar()
{
	return equiv(TEX_EVERY_PAR_LOC);
}

inline qint32 XWTeX::everyVBox()
{
	return equiv(TEX_EVERY_VBOX_LOC);
}

inline qint32 XWTeX::exhyphenPenalty()
{
	return intPar(TEX_EX_HYPHEN_PENALTY_CODE);
}

inline qint32 XWTeX::famFnt(qint32 a)
{
	return equiv(TEX_MATH_FONT_BASE + a);
}

inline void XWTeX::fastDeleteGlueRef(qint32 p)
{
	if (glue_ref_count(p) == TEX_NULL)
		freeNode(p, TEX_GLUE_SPEC_SIZE);
	else
		(glue_ref_count(p))--;
}

inline void XWTeX::fastGetAvail(qint32 & p)
{
	p = avail;
	if (p == TEX_NULL)
		p = getAvail();
	else
	{
		avail = link(p); 
		link(p) = TEX_NULL;
#ifdef XW_TEX_STAT
		dyn_used++;
#endif //XW_TEX_STAT
	}
}

inline qint32 XWTeX::finalHyphenDemerits()
{
	return intPar(TEX_FINAL_HYPHEN_DEMERITS_CODE);
}

inline qint32 XWTeX::floatingPenalty()
{
	return intPar(TEX_FLOATING_PENALTY_CODE);
}

inline void XWTeX::flushChar()
{
	pool_ptr--;
}

inline void XWTeX::flushString()
{
	str_ptr--;
	pool_ptr = str_start(str_ptr);
}

inline qint32 XWTeX::fontIdText(qint32 a)
{
	return newText(TEX_FONT_ID_BASE + a);
}

inline void XWTeX::freeAvail(qint32 p)
{
	link(p) = avail; 
	avail = p;
#ifdef XW_TEX_STAT
	dyn_used--;
#endif //XW_TEX_STAT
}

inline qint32 XWTeX::globalDefs()
{
	return intPar(TEX_GLOBAL_DEFS_CODE);
}

inline void XWTeX::glueError(qint32 & p)
{
	arith_error = true; 
	deleteGlueRef(p); 
	p = newSpec(zero_glue);
}

inline qint32 XWTeX::gluePar(qint32 a)
{
	return equiv(TEX_GLUE_BASE + a);
}

inline qint32 XWTeX::half(qint32 x)
{
	if (odd(x))
		return (x + 1) / 2;
		
	return x / 2;
}

inline qint32 XWTeX::hangAfter()
{
	return intPar(TEX_HANG_AFTER_CODE);
}

inline qint32 XWTeX::hangIndent()
{
	return dimenPar(TEX_HANG_INDENT_CODE);
}

inline qint32 XWTeX::hbadness()
{
	return intPar(TEX_HBADNESS_CODE);
}

inline void XWTeX::help0()
{
	help_ptr = 0;
}

inline void XWTeX::help1(const QString & str)
{
	help_ptr = 1;
	help_line[0] = str;
}

inline void XWTeX::help2(const QString & str)
{
	help_ptr = 2;
	help_line[1] = str;
}

inline void XWTeX::help3(const QString & str)
{
	help_ptr = 3;
	help_line[2] = str;
}

inline void XWTeX::help4(const QString & str)
{
	help_ptr = 4;
	help_line[3] = str;
}

inline void XWTeX::help5(const QString & str)
{
	help_ptr = 5;
	help_line[4] = str;
}

inline void XWTeX::help6(const QString & str)
{
	help_ptr = 6;
	help_line[5] = str;
}

inline qint32 XWTeX::hfuzz()
{
	return dimenPar(TEX_HFUZZ_CODE);
}

inline qint32 XWTeX::hoffset()
{
	return dimenPar(TEX_H_OFFSET_CODE);
}

inline qint32 XWTeX::holdingInserts()
{
	return intPar(TEX_HOLDING_INSERTS_CODE);
}

inline qint32 XWTeX::hsize()
{
	return dimenPar(TEX_HSIZE_CODE);
}

inline qint32 XWTeX::hyphenPenalty()
{
	return intPar(TEX_HYPHEN_PENALTY_CODE);
}

inline void XWTeX::insList(qint32 p)
{
	beginTokenList(p, TEX_INSERTED);
}

inline qint32 XWTeX::interLinePenaltiesPtr()
{
	return equiv(TEX_INTER_LINE_PENALTIES_LOC);
}

inline qint32 XWTeX::interLinePenalty()
{
	return intPar(TEX_INTER_LINE_PENALTY_CODE);
}

inline qint32 XWTeX::intPar(qint32 a)
{
	return newEqtbInt(TEX_INT_BASE + a);
}

inline qint32 XWTeX::language()
{
	return intPar(TEX_LANGUAGE_CODE);
}

inline qint32 XWTeX::lastLineFit()
{
	return intPar(TEX_LAST_LINE_FIT_CODE);
}

inline qint32 XWTeX::lcCode(qint32 a)
{
	return equiv(TEX_LC_CODE_BASE + a);
}

inline qint32 XWTeX::leftHyphenMin()
{
	return intPar(TEX_LEFT_HYPHEN_MIN_CODE);
}

inline qint32 XWTeX::leftSkip()
{
	return gluePar(TEX_LEFT_SKIP_CODE);
}

inline qint32 XWTeX::levelLocalDir()
{
	return intPar(TEX_LEVEL_LOCAL_DIR_CODE);
}

inline qint32 XWTeX::linePenalty()
{
	return intPar(TEX_LINE_PENALTY_CODE);
}

inline qint32 XWTeX::lineSkip()
{
	return gluePar(TEX_LINE_SKIP_CODE);
}

inline qint32 XWTeX::lineSkipLimit()
{
	return dimenPar(TEX_LINE_SKIP_LIMIT_CODE);
}

inline qint32 XWTeX::localBrokenPenalty()
{
	return intPar(TEX_LOCAL_BROKEN_PENALTY_CODE);
}

inline qint32 XWTeX::localInterLinePenalty()
{
	return intPar(TEX_LOCAL_INTER_LINE_PENALTY_CODE);
}

inline qint32 XWTeX::localLeftBox()
{
	return equiv(TEX_LOCAL_LEFT_BOX_BASE);
}

inline qint32 XWTeX::localRightBox()
{
	return equiv(TEX_LOCAL_RIGHT_BOX_BASE);
}

inline qint32 XWTeX::looseness()
{
	return intPar(TEX_LOOSENESS_CODE);
}

inline qint32 XWTeX::mag()
{
	return intPar(TEX_MAG_CODE);
}

inline qint32 XWTeX::makeNullOcpList()
{
	return makeOcpListNode(0, TEX_OCP_MAXINT, 0);
}

inline qint32 XWTeX::mathCode(qint32 a)
{
	return equiv(TEX_MATH_CODE_BASE + a);
}

inline qint32 XWTeX::mathDirection()
{
	return dirPar(TEX_MATH_DIRECTION_CODE);
}

inline qint32 XWTeX::mathSurround()
{
	return dimenPar(TEX_MATH_SURROUND_CODE);
}

inline qint32 XWTeX::maxDeadCycles()
{
	return intPar(TEX_MAX_DEAD_CYCLES_CODE);
}

inline qint32 XWTeX::maxDepth()
{
	return dimenPar(TEX_MAX_DEPTH_CODE);
}

inline qint32 XWTeX::medMuSkip()
{
	return gluePar(TEX_MED_MU_SKIP_CODE);
}

inline qint32 XWTeX::month()
{
	return intPar(TEX_MONTH_CODE);
}

inline qint32 XWTeX::multIntegers(qint32 n, qint32 x)
{
	return multAndAdd(n, x, 0, 0x7FFFFFFF);
}

inline qint32 XWTeX::muskip(qint32 a)
{
	return equiv(TEX_MU_SKIP_BASE + a);
}

inline quint16 XWTeX::newEqLevel(qint32 a)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		return hashtable[a % TEX_HASHTABLESIZE].mw.hh.u.B1;
		
	return createEqtbPos(a)->mw.hh.u.B1;
}

inline TeXMemoryWord XWTeX::newEqtb(qint32 a)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		return hashtable[a % TEX_HASHTABLESIZE].mw;
		
	return createEqtbPos(a)->mw;
}

inline qint32 XWTeX::newEqtbInt(qint32 a)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		return hashtable[a % TEX_HASHTABLESIZE].mw.ii.CINT0;
		
	return createEqtbPos(a)->mw.ii.CINT0;
}

inline qint32 XWTeX::newEqtbSC(qint32 a)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		return hashtable[a % TEX_HASHTABLESIZE].mw.ii.CINT0;
		
	return createEqtbPos(a)->mw.ii.CINT0;
}

inline qint16 XWTeX::newEqType(qint32 a)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		return hashtable[a % TEX_HASHTABLESIZE].mw.hh.u.B0;
		
	return createEqtbPos(a)->mw.hh.u.B0;
}

inline qint32 XWTeX::newEquiv(qint32 a)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		return hashtable[a % TEX_HASHTABLESIZE].mw.hh.v.RH;
		
	return createEqtbPos(a)->mw.hh.v.RH;
}

inline qint32 XWTeX::newEquiv1(qint32 a)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		return hashtable[a % TEX_HASHTABLESIZE].mw.ii.CINT1;
		
	return createEqtbPos(a)->mw.ii.CINT1;
}

inline qint32 XWTeX::newHashNext(qint32 a)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		return hashtable[a % TEX_HASHTABLESIZE].mw.hh.v.LH;
		
	return createHashPos(a)->mw.hh.v.LH;
}

inline qint32 XWTeX::newHashText(qint32 a)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		return hashtable[a % TEX_HASHTABLESIZE].mw.hh.v.RH;
		
	return createHashPos(a)->mw.hh.v.RH;
}

inline qint32 XWTeX::newLineChar()
{
	return intPar(TEX_NEW_LINE_CHAR_CODE);
}

inline qint32 XWTeX::newNext(qint32 a)
{
	return newHashNext(TEX_EQTB_SIZE + a);
}

inline qint32 XWTeX::newText(qint32 a)
{
	return newHashText(TEX_EQTB_SIZE + a);
}

inline qint32 XWTeX::newXEqLevel(qint32 a)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		return hashtable[a % TEX_HASHTABLESIZE].mw.ii.CINT0;
		
	return createXEqLevel(a)->mw.ii.CINT0;
}

inline qint32 XWTeX::nolocalDirs()
{
	return intPar(TEX_NO_LOCAL_DIRS_CODE);
}

inline qint32 XWTeX::nolocalWhatsits()
{
	return intPar(TEX_NO_LOCAL_WHATSITS_CODE);
}

inline void XWTeX::nodeListDisplay(qint32 p)
{
	appendChar('.'); 
	showNodeList(p); 
	flushChar();
}

inline qint32 XWTeX::nullDelimiterSpace()
{
	return dimenPar(TEX_NULL_DELIMITER_SPACE_CODE);
}

inline void XWTeX::numError(qint32 & f)
{
	arith_error = true; 
	f = 0;
}

inline qint32 XWTeX::nxPlusY(qint32 n, qint32 x, qint32 y)
{
	return multAndAdd(n, x, y, 0x3FFFFFFF);
}

inline qint32 XWTeX::ocpIdText(qint32 a)
{
	return newText(TEX_OCP_ID_BASE + a);
}

inline qint32 XWTeX::ocpListIdText(qint32 a)
{
	return newText(TEX_OCP_LIST_ID_BASE + a);
}

inline qint32 XWTeX::outputPenalty()
{
	return intPar(TEX_OUTPUT_PENALTY_CODE);
}

inline qint32 XWTeX::outputRoutine()
{
	return equiv(TEX_OUTPUT_ROUTINE_LOC);
}

inline qint32 XWTeX::overFullRule()
{
	return dimenPar(TEX_OVERFULL_RULE_CODE);
}

inline void XWTeX::packCurName()
{
	packFileName(cur_name,cur_area,cur_ext);
}

inline qint32 XWTeX::pageBottomOffset()
{
	return dimenPar(TEX_PAGE_BOTTOM_OFFSET_CODE);
}

inline qint32 XWTeX::pageDirection()
{
	return dirPar(TEX_PAGE_DIRECTION_CODE);
}

inline qint32 XWTeX::pageHeight()
{
	return dimenPar(TEX_PAGE_HEIGHT_CODE);
}

inline qint32 XWTeX::pageRightOffset()
{
	return dimenPar(TEX_PAGE_RIGHT_OFFSET_CODE);
}

inline qint32 XWTeX::pageWidth()
{
	return dimenPar(TEX_PAGE_WIDTH_CODE);
}

inline qint32 XWTeX::parDirection()
{
	return dirPar(TEX_PAR_DIRECTION_CODE);
}

inline qint32 XWTeX::parFillSkip()
{
	return gluePar(TEX_PAR_FILL_SKIP_CODE);
}

inline qint32 XWTeX::parIndent()
{
	return dimenPar(TEX_PAR_INDENT_CODE);
}

inline qint32 XWTeX::parShapePtr()
{
	return equiv(TEX_PAR_SHAPE_LOC);
}

inline qint32 XWTeX::parSkip()
{
	return gluePar(TEX_PAR_SKIP_CODE);
}

inline qint32 XWTeX::pausing()
{
	return intPar(TEX_PAUSING_CODE);
}

inline void XWTeX::popInput()
{
	input_ptr--;
	if (input_ptr < 0)
		return ;
		
	cur_input = input_stack[input_ptr];
}

inline qint32 XWTeX::postdisplayPenalty()
{
	return intPar(TEX_POST_DISPLAY_PENALTY_CODE);
}

inline qint32 XWTeX::predisplayDirection()
{
	return intPar(TEX_PRE_DISPLAY_DIRECTION_CODE);
}

inline qint32 XWTeX::predisplayPenalty()
{
	return intPar(TEX_PRE_DISPLAY_PENALTY_CODE);
}

inline qint32 XWTeX::predisplaySize()
{
	return dimenPar(TEX_PRE_DISPLAY_SIZE_CODE);
}

inline qint32 XWTeX::pretolerance()
{
	return intPar(TEX_PRETOLERANCE_CODE);
}

inline void XWTeX::printLCHex(qint32 c)
{
	if (c < 10) 
		printChar(c + '0');
	else 
		printChar(c - 10 + 'a');
}

inline qint32 XWTeX::relPenalty()
{
	return intPar(TEX_REL_PENALTY_CODE);
}

inline qint32 XWTeX::rightHyphenMin()
{
	return intPar(TEX_RIGHT_HYPHEN_MIN_CODE);
}

inline qint32 XWTeX::rightSkip()
{
	return gluePar(TEX_RIGHT_SKIP_CODE);
}

inline qint32 XWTeX::roundDecimals(char k)
{
	qint32 a = 0;
	while (k > 0)
	{
		k--;
		a = (a + dig[k] * TEX_TWO) / 10;
	}
	
	a = (a + 1) / 2;
	return a;
}

inline qint32 XWTeX::savingHyphCodes()
{
	return intPar(TEX_SAVING_HYPH_CODES_CODE);
}

inline qint32 XWTeX::savingVDiscards()
{
	return intPar(TEX_SAVING_VDISCARDS_CODE);
}

inline void XWTeX::scannedResult(qint32 v, char l)
{
	cur_val = v;
	cur_val_level = l;
}

inline void XWTeX::scanMuGlue()
{
	scanGlue(TEX_MU_VAL);
}

inline void XWTeX::scanNormalDimen()
{
	scanDimen(false, false, false);
}

inline void XWTeX::scanNormalGlue()
{
	scanGlue(TEX_GLUE_VAL);
}

inline qint32 XWTeX::scriptSpace()
{
	return dimenPar(TEX_SCRIPT_SPACE_CODE);
}

inline void XWTeX::setEqLevel(qint32 a, quint16 v)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		hashtable[a % TEX_HASHTABLESIZE].mw.hh.u.B1 = (quint16)v;
		
	createEqtbPos(a)->mw.hh.u.B1 = (quint16)v;
}

inline void XWTeX::setEqType(qint32 a, quint16 v)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		hashtable[a % TEX_HASHTABLESIZE].mw.hh.u.B0 = (quint16)v;
		
	createEqtbPos(a)->mw.hh.u.B0 = (quint16)v;
}

inline void XWTeX::setEquiv(qint32 a, qint32 v)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		hashtable[a % TEX_HASHTABLESIZE].mw.hh.v.RH = v;
		
	createEqtbPos(a)->mw.hh.v.RH = v;
}

inline void XWTeX::setEquiv1(qint32 a, qint32 v)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		hashtable[a % TEX_HASHTABLESIZE].mw.ii.CINT1 = v;
		
	createEqtbPos(a)->mw.ii.CINT1 = v;
}

inline void XWTeX::setHashNext(qint32 a, qint32 d)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		hashtable[a % TEX_HASHTABLESIZE].mw.hh.v.LH = d;
		
	createHashPos(a)->mw.hh.v.LH = d;
}

inline void XWTeX::setHashText(qint32 a, qint32 d)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		hashtable[a % TEX_HASHTABLESIZE].mw.hh.v.RH = d;
		
	createHashPos(a)->mw.hh.v.RH = d;
}

inline void XWTeX::setNewEqtb(qint32 a, const TeXMemoryWord & v)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		hashtable[a % TEX_HASHTABLESIZE].mw = v;
		
	createEqtbPos(a)->mw = v;
}

inline void XWTeX::setNewEqtbInt(qint32 a, qint32 v)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		hashtable[a % TEX_HASHTABLESIZE].mw.ii.CINT0 = v;
		
	createEqtbPos(a)->mw.ii.CINT0 = v;
}

inline void XWTeX::setNewEqtbSC(qint32 a, qint32 v)
{
	if (a == hashtable[a % TEX_HASHTABLESIZE].p)
		hashtable[a % TEX_HASHTABLESIZE].mw.ii.CINT0 = v;
		
	createEqtbPos(a)->mw.ii.CINT0 = v;
}

inline void XWTeX::setNewLineChar(qint32 nl)
{
	setNewEqtbInt(TEX_INT_BASE + TEX_NEW_LINE_CHAR_CODE, nl);
}

inline void XWTeX::setNext(qint32 a, qint32 d)
{
	setHashNext(TEX_EQTB_SIZE + a, d);
}

inline void XWTeX::setText(qint32 a, qint32 d)
{
	setHashText(TEX_EQTB_SIZE + a, d);
}

inline void XWTeX::setTrickCount()
{
	first_count = tally;
  	trick_count = tally + 1 + error_line - half_error_line;
  	if (trick_count < error_line)
  		trick_count = error_line;
}

inline void XWTeX::setXEqLevel(qint32 a, qint32 v)
{
	setNewEqtbInt(TEX_EQTB_SIZE + TEX_EQTB_SIZE + a, v);
}

inline qint32 XWTeX::sfCode(qint32 a)
{
	return equiv(TEX_SF_CODE_BASE + a);
}

inline qint32 XWTeX::showBoxBreadth()
{
	return intPar(TEX_SHOW_BOX_BREADTH_CODE);
}

inline qint32 XWTeX::showBoxDepth()
{
	return intPar(TEX_SHOW_BOX_DEPTH_CODE);
}

inline qint32 XWTeX::skip(qint32 a)
{
	return equiv(TEX_SKIP_BASE + a);
}

inline qint32 XWTeX::spaceSkip()
{
	return gluePar(TEX_SPACE_SKIP_CODE);
}

inline qint32 XWTeX::splitMaxDepth()
{
	return dimenPar(TEX_SPLIT_MAX_DEPTH_CODE);
}

inline qint32 XWTeX::splitTopSkip()
{
	return gluePar(TEX_SPLIT_TOP_SKIP_CODE);
}

inline qint32 XWTeX::tabSkip()
{
	return gluePar(TEX_TAB_SKIP_CODE);
}

inline void XWTeX::tailAppend(qint32 p)
{
	link(tail) = p; 
	tail = link(tail);
}

inline qint32 XWTeX::textDirection()
{
	return dirPar(TEX_TEXT_DIRECTION_CODE);
}

inline qint32 XWTeX::thickMuSkip()
{
	return gluePar(TEX_THICK_MU_SKIP_CODE);
}

inline qint32 XWTeX::thinMuSkip()
{
	return gluePar(TEX_THIN_MU_SKIP_CODE);
}

inline qint32 XWTeX::time()
{
	return intPar(TEX_TIME_CODE);
}

inline qint32 XWTeX::toks(qint32 a)
{
	return equiv(TEX_TOKS_BASE + a);
}

inline qint32 XWTeX::tolerance()
{
	return intPar(TEX_TOLERANCE_CODE);
}

inline qint32 XWTeX::topSkip()
{
	return gluePar(TEX_TOP_SKIP_CODE);
}

inline qint32 XWTeX::tracingAssigns()
{
	return intPar(TEX_TRACING_ASSIGNS_CODE);
}

inline qint32 XWTeX::tracingCommands()
{
	return intPar(TEX_TRACING_COMMANDS_CODE);
}

inline qint32 XWTeX::tracingGroups()
{
	return intPar(TEX_TRACING_GROUPS_CODE);
}

inline qint32 XWTeX::tracingIfs()
{
	return intPar(TEX_TRACING_IFS_CODE);
}

inline qint32 XWTeX::tracingLostChars()
{
	return intPar(TEX_TRACING_LOST_CHARS_CODE);
}

inline qint32 XWTeX::tracingMacros()
{
	return intPar(TEX_TRACING_MACROS_CODE);
}

inline qint32 XWTeX::tracingNesting()
{
	return intPar(TEX_TRACING_NESTING_CODE);
}

inline qint32 XWTeX::tracingOnline()
{
	return intPar(TEX_TRACING_ONLINE_CODE);
}

inline qint32 XWTeX::tracingOutput()
{
	return intPar(TEX_TRACING_OUTPUT_CODE);
}

inline qint32 XWTeX::tracingPages()
{
	return intPar(TEX_TRACING_PAGES_CODE);
}

inline qint32 XWTeX::tracingParagraphs()
{
	return intPar(TEX_TRACING_PARAGRAPHS_CODE);
}

inline qint32 XWTeX::tracingRestores()
{
	return intPar(TEX_TRACING_RESTORES_CODE);
}

inline qint32 XWTeX::tracingScanTokens()
{
	return intPar(TEX_TRACING_SCAN_TOKENS_CODE);
}

inline qint32 XWTeX::tracingStats()
{
	return intPar(TEX_TRACING_STATS_CODE);
}

inline qint32 XWTeX::ucCode(qint32 a)
{
	return equiv(TEX_UC_CODE_BASE + a);
}

inline qint32 XWTeX::ucHyph()
{
	return intPar(TEX_UC_HYPH_CODE);
}

inline qint32 XWTeX::vbadness()
{
	return intPar(TEX_VBADNESS_CODE);
}

inline qint32 XWTeX::vfuzz()
{
	return dimenPar(TEX_VFUZZ_CODE);
}

inline qint32 XWTeX::voffset()
{
	return dimenPar(TEX_V_OFFSET_CODE);
}

inline qint32 XWTeX::vpack(qint32 p, qint32 h, char m)
{
	return vpackage(p, h, m, TEX_MAX_DIMEN);
}

inline qint32 XWTeX::vsize()
{
	return dimenPar(TEX_VSIZE_CODE);
}

inline qint32 XWTeX::widowPenaltiesPtr()
{
	return equiv(TEX_WIDOW_PENALTIES_LOC);
}

inline qint32 XWTeX::widowPenalty()
{
	return intPar(TEX_WIDOW_PENALTY_CODE);
}

inline qint32 XWTeX::xeqLevel(qint32 a)
{
	return newXEqLevel(TEX_EQTB_SIZE + TEX_EQTB_SIZE + a);
}

inline qint32 XWTeX::xspaceSkip()
{
	return gluePar(TEX_XSPACE_SKIP_CODE);
}

inline qint32 XWTeX::year()
{
	return intPar(TEX_YEAR_CODE);
}

#endif //XWTEX_H
