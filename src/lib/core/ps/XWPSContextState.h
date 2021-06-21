/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSCONTEXTSTATE_H
#define XWPSCONTEXTSTATE_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QList>
#include "XWPSType.h"
#include "XWPSStreamState.h"
#include "XWPSStream.h"
#include "XWPSDict.h"

#define PS_O_PUSH_ESTACK 5
#define PS_O_POP_ESTACK 14
#define PS_O_RESCHEDULE 22

#define SCAN_FROM_STRING 1
#define SCAN_CHECK_ONLY 2
#define SCAN_PROCESS_COMMENTS 4
#define SCAN_PROCESS_DSC_COMMENTS 8

#define PS_ES_OTHER 0
#define PS_ES_SHOW 1	
#define PS_ES_FOR 2
#define PS_ES_STOPPED 3

#define CVP_MAX_STRING 200

#define scan_BOS 1
#define scan_EOF 2
#define scan_Refill 3	
#define scan_Comment 4
#define scan_DSC_Comment 5

#define bt_num_array_value 149

#define num_csme 5
#define csme_num_components (-4)	
#define csme_map (-3)	
#define csme_proc (-2)	
#define csme_hival (-1) 
#define csme_index 0	

class QIODevice;
class XWDVIRef;
class XWDVICore;
class XWDVIDev;
class PSParamString;
class XWPSParamList;
class XWPSPath;
class XWPSImageCommon;
class XWPSImageEnumCommon;
class XWPSDataImage;
class XWPSPixelImage;
class XWPSImageParams;
class XWPSImage3xMask;
class XWPSImage1;
class XWPSColorSpace;
class XWPSShading;
class XWPSShadingParams;
class XWPSShadingMeshParams;
class XWPSFunction;
class XWPSFunctionParams;
class XWPSSpotHalftone;
class XWPSScreenHalftone;
class XWPSThresholdHalftoneCommon;
class XWPSThresholdHalftone2;
class XWPSThresholdHalftone;
class XWPSHTOrder;
class XWPSIndexedMap;
class XWPSTransferMap;
class XWPSAlphaCompositeState;
class XWPSCie;
class XWPSCieABC;
class XWPSColorLookupTable;
class XWPSState;
class XWPSStream;
class XWPSDevice;
class XWPSIODevice;
class XWPSParsedFileName;
struct PSPaintColor;
class XWPSCieRender;
class XWPSScannerState;
class XWPSContextState;
class XWPSNameTable;
class XWPSStrEnum;
class XWPSKeyEnum;
class XWPSDSCParser;
class XWPSCMap;
class XWPSCidSystemInfo;
class XWPSFont;
class XWPSFontDir;
class XWPSFontBase;
class XWPSFontType1;
class XWPSFontCid0;
class XWPSFontCid2;
class XWPSFontType42;
class XWPSCFontDictKeys;
class XWPSType1execState;
class XWPSTextEnum;
class XWPSContext;
struct PSCFontProcs;
struct PSStreamTemplate;
struct PSStreamProc;
struct PSParamSet;

enum BinSeqType
{
    BS_TYPE_NULL = 0,
    BS_TYPE_INTEGER = 1,
    BS_TYPE_REAL = 2,
    BS_TYPE_NAME = 3,
    BS_TYPE_BOOLEAN = 4,
    BS_TYPE_STRING = 5,
    BS_TYPE_EVAL_NAME = 6,
    BS_TYPE_ARRAY = 9,
    BS_TYPE_MARK = 10,
    BS_TYPE_DICTIONARY = 15
};

#define BS_EXECUTABLE 128
#define SIZEOF_BIN_SEQ_OBJ ((uint)8)

struct PSOperator 
{
  const char *oname;
  op_proc_t proc;
};

struct PSBuildFunctionType
{
	int type;
  int (XWPSContextState::*build_function_proc_t)(XWPSRef *, 
                                     XWPSFunctionParams *, 
                                     int , 
                                     XWPSFunction **);
};


#define op_def_begin_dict(dname) {dname, 0}
#define op_def_begin_filter() op_def_begin_dict("filterdict")
#define op_def_begin_level2() op_def_begin_dict("level2dict")
#define op_def_begin_ll3() op_def_begin_dict("ll3dict")
#define op_def_is_begin_dict(def) ((def)->proc == 0)
#define op_def_end(iproc) {0, iproc}

#define OP_DEFS_LOG2_MAX_SIZE 4
#define OP_DEFS_MAX_SIZE (1 << OP_DEFS_LOG2_MAX_SIZE)

#define op_def_is_internal(def) ((def)->oname[1] == '%')

class XWPSOpArrayTable
{
public:
	XWPSOpArrayTable();
	~XWPSOpArrayTable();
	
	void init(XWPSContextState * ctx, uint size, uint space, uint bidx);
	
public:
	XWPSRef table;
	ushort *nx_table;
	uint count;
	uint base_index;
	uint attrs;
	XWPSRef *root_p;
};

enum PSScanType
{
	ScanningNone,
	ScanningBinary,
	ScanningComment,
	ScanningName,
	ScanningString
};

#define max_comment_line 255	
#define max_dsc_line max_comment_line	
#define da_buf_size (max_comment_line + 2)

class XWPSDynamicArea
{
public:
	XWPSDynamicArea();
	~XWPSDynamicArea();
	
	void copy(const XWPSDynamicArea & other);
	
	int grow(uchar * nextA, uint max_size);
	
	void init();
	
	void release();
	int resize(uint new_size);
	
	void save();
	
public:
	uchar *base;
  uchar *next;
  uchar *limit;
  bool is_dynamic;
  uchar buf[da_buf_size];
  XWPSBytes * dynamic_buf;
};
	
struct PSScanBinaryState
{
	int num_format;
  int (XWPSContextState::*cont)(XWPSStream *, XWPSRef *, XWPSScannerState *);
  uint index;
  uint max_array_index;
  uint min_string_index;
  uint top_size;
  uint size;
};

class XWPSScannerState : public XWPSStruct
{
public:
	XWPSScannerState();
	~XWPSScannerState();
	
	int getLength();
	const char * getTypeName();
	
	void init(int options);
	void init(bool from_string);
	void initCheck(bool from_string, bool check_only);
	
	XWPSScannerState & operator=(XWPSScannerState & other);
	
public:
	uint s_pstack;
	uint s_pdepth;
	int s_options;
	PSScanType s_scan_type;
	XWPSDynamicArea s_da;
	XWPSRef bin_array;
	union sss_ 
	{
		PSScanBinaryState binary;	
		struct sns_ 
		{	
	    int s_name_type;
	    bool s_try_number;
		} s_name;
		
		PSStreamState st;
		PSStreamA85DState a85d;	
		PSStreamAXDState axd;	
		PSStreamPSSDState pssd;	
  } s_ss;
};


typedef int (XWPSContextState::*build_shading_proc_t)(XWPSRef *, const XWPSShadingParams *, XWPSShading **);
     
typedef int (XWPSContextState::*ccfont_fproc)(const PSCFontProcs *, XWPSRef *);

typedef int (XWPSContextState::*build_base_font_proc_t)(XWPSRef *, 
	                                                     XWPSFontBase **, 
	                                                     PSFontType,
	      																							XWPSBuildProcRefs *,
	      																							PSBuildFontOptions);
	 
class XWPSContextState
{
public:
	XWPSContextState();
	~XWPSContextState();
		
	int callInterpret(XWPSRef * pref);
	int checkEStack(int n);
	int checkOp(XWPSRef * op, int nargs);
	int checkOStack(int n);
	void close();
	uint currentSpace() {return memory->iallocSpace();}
	
	int dictCopy(XWPSRef * from, XWPSRef * to);
	int dictCopyNew(XWPSRef * from, XWPSRef * to);
	XWPSRef * dictFindName(XWPSRef * pnref);
	int dictGrow(XWPSRef * dict);
	int dictPut(XWPSRef * dict, XWPSRef * pkey, XWPSRef * pvalue);
	int dictPutString(XWPSRef * dict, const char *kstr, XWPSRef * pvalue);
	int dictResize(XWPSRef * dict, uint new_size);
	int dictUndef(XWPSRef * dict, XWPSRef * pkey);
	int dictUnpack(XWPSRef * dict);
	
	int estackUnderflow();
	int errorName(int code, XWPSRef * perror_name);
	
	XWPSIODevice * findIODevice(const uchar * str, uint len);
	int floatParams(XWPSRef * op, int count, float *pval);
	
	XWPSDevice * getDevice(int i);
	XWPSExecStack * getExecStack() {return &exec_stack;};
	XWPSIODevice * getIODevice(int index);
	XWPSStream * getInvalidFileEntry() {return &invalid_file_stream;}
	XWPSOpStack * getOpStack() {return &op_stack;};
	XWPSState * getState() {return pgs;}
	
	static bool haveLevel2();
	
	XWPSDualMemory * idmemory() {return memory;}
	XWPSRefMemory * iimemory() {return memory->current;}
	XWPSRefMemory * iimemoryGlobal() {return memory->spaces.memories.named.global;}
	XWPSRefMemory * iimemoryLocal() {return memory->spaces.memories.named.local;}	
	XWPSRefMemory * iimemorySystem() {return memory->spaces.memories.named.system;}
	XWPSRefMemory * imemory() {return memory->current;}
	XWPSRefMemory * imemoryGlobal() {return memory->spaces.memories.named.global;}
	XWPSRefMemory * imemoryLocal() {return memory->spaces.memories.named.local;}
	XWPSRefMemory * imemorySystem() {return memory->spaces.memories.named.system;}
	int  interpret(XWPSRef * pref, XWPSRef * perror_object);
	bool isDictAutoExpand() {return dict_auto_expand;}
	
	bool level2Enabled() {return language_level >= 2;}
	bool level3Enabled() {return language_level >= 3;}
	
	int  nameEnterString(const char *str, XWPSRef * pref);
	static bool nameEq(XWPSRef * pnref1, XWPSRef * pnref2);
	int  nameFromString(XWPSRef * psref, XWPSRef * pnref);
	uint nameIndex(XWPSRef * pnref);
	XWPSName * nameIndexPtr(uint nidx);
	void nameIndexRef(uint nidx, XWPSRef * pnref);
	void nameInvalidateValueCache(XWPSRef * pnref);
	bool nameMarkIndex(uint nidx);
	uint nameNextValidIndex(uint nidx);
	int  nameRef(const uchar *ptr, uint size, XWPSRef *pref, int enterflag);	
	XWPSName * nameRefSubTable(XWPSRef * pnref);	
	void nameStringRef(XWPSRef * pnref, XWPSRef * psref);
	ulong nextIDS(uint count);
	int numParams(XWPSRef * op, int count, double *pval);
	
	static ushort opFindIndex(XWPSRef * ref);
	static ushort opIndex(XWPSRef * ref);
	static PSOperator * opIndexDef(uint index);
	static bool   opIndexIsOperator(uint index);
	XWPSOpArrayTable * opIndexOpArrayTable(uint index);	
	static op_proc_t opIndexProc(uint index);	
	void   opIndexRef(uint index, XWPSRef *pref);
	static uint   opNumArgs(XWPSRef * ref);	
	
	void packedGet(ushort * packed, XWPSRef * pref);
	void pop(int n);
	void popEStack(uint count);
	int push(XWPSRef ** op, int n);
	
	XWPSRef * registeredEncoding(int i);
	void reset();
	void resetNextID();
	void resetPageNextID(ulong pgno);
	
	int runStringBegin();
	int runStringContinue(const char *str, uint length);
	int runStringEnd();
	int runStart();
	int runString(const char *str, uint length);
	
	void saveNextID();
	void savePageNextID(ulong pgno);
	XWPSRef * StandardEncoding();
	int stringToRef(const char *cstr, XWPSRef * pref);
	
	int toPDF(XWDVICore * coreA,
	             XWDVIRef * xrefA,
	             XWDVIDev * devA,
	             const uchar * buf, 
	             uint len);
	QString toPDF(const QString & filename, 
	              double pw, 
	              double ph);
	
	int opArrayCleanup();
	XWPSRef * oparrayFind();
	int opArrayNoCleanup() {return 0;}
	int opArrayPop();
	int zcurrentStackProtect();
	int zsetStackProtect();
	
	int zclearStack();
	int zclearToMark();
	int zcount();
	int zcountToMark();
	int zdup();
	int zexch();
	int zindex();
	int zmark();
	int zpop();
	int zroll();
	
	int zabs();
	int zadd();
	int zbitAdd();
	int zceiling();
	int zdiv();
	int zfloor();
	int zidiv();
	int zmod();
	int zmul();
	int zneg();
	int zopAdd(XWPSRef * op);
	int zopSub(XWPSRef * op);
	int zround();
	int zsub();
	int ztruncate();
	
	int darc(double (*afunc)(double));
	int dcompare(int mask);
	int dlog(double (*lfunc)(double));
	int doubleParams(XWPSRef * op, int count, double *pval);
	int doubleParamsResult(XWPSRef * op, int count, double *pval);
	int doubleResult(int count, double result);
	int doubleUnary(double (*func)(double));
	int zcvd();
	int zcvsd();
	int zdabs();
	int zdadd();
	int zdarccos();
	int zdarcsin();
	int zdatan();
	int zdceiling();
	int zdcos();
	int zdcvi();
	int zdcvr();
	int zdcvs();
	int zddiv();
	int zdeq();
	int zdexp();
	int zdfloor();
	int zdge();
	int zdgt();
	int zdle();
	int zdln();
	int zdlog();
	int zdlt();
	int zdmul();
	int zdne();
	int zdneg();
	int zdround();
	int zdsin();
	int zdsqrt();
	int zdsub();
	int zdtruncate();
	
	int zarccos();
	int zarcsin();
	int zatan();
	int zcos();
	int zexp();
	int zln();
	int zlog();
	int zrand();
	int zrrand();
	int zsin();
	int zsqrt();
	int zsrand();
	
	int commonTransform(int (XWPSState::*ptproc)(float, float, XWPSPoint *),
											int (XWPSPoint::*matproc)(double, double, XWPSMatrix *));
	int zconcat();
	int zconcatMatrix();
	int zcurrentMatrix();
	int zdefaultMatrix();
	int zdtransform();
	int zidtransform();
	int zinitMatrix();
	int zinvertMatrix();
	int zitransform();
	int zrotate();
	int zscale();
	int zsetDefaultMatrix();
	int zsetMatrix();
	int ztranslate();
	int ztransform();
	
	int arrayContinue();
	int copyInterval(XWPSRef * prto, uint index, XWPSRef* prfrom);
	int dictContinue();
	int forallCleanup() {return 0;}
	int makePackedArray(XWPSRef * parr, XWPSRefStack * pstack, uint size);
	int packedArrayContinue();
	int stringContinue();
	int zaload();
	int zarray();
	int zastore();
	int zcopy();
	int zcopyInteger();
	int zcopyInterval();
	int zcurrentPacking();
	int zforall();
	int zforcePut();
	int zget();
	int zgetInterval();
	int zlength();
	int zpackedArray();
	int zput();
	int zputInterval();
	int zsetPacking();
		
	int zbegin();
	int zclearDictStack();
	int zcopyDict();
	int zcountDictStack();
	int zcurrentDict();
	int zdef();
	int zdict();
	int zdictCopyNew();
	int zdictStack();
	int zdictToMark();
	int zend();
	int zforceUndef();
	int zknown();
	int zknownGet();
	int zknownUndef();
	int zload();
	int zmaxlength();
	int zopDef();
	int zsetMaxLength();
	int zundef();
	int zwhere();
	
	int zanchorSearch();
	int zbyteString();
	int znameString();
	int zsearch();
	int zstring();
	int zstringMatch();
	
	int tokenContinue(XWPSStream * s, XWPSScannerState * pstate, bool save);
	int tokenExecContinue(XWPSStream * s, XWPSScannerState * pstate, bool save);
	int ztoken();
	int ztokenContinue();
	int ztokenExec();
	int ztokenExecContinue();
	int ztokenHandleComment(XWPSRef *fop, 
	                       XWPSScannerState *sstate,
		                     XWPSRef *ptoken, 
		                     int scan_code,
		                     bool save, 
		                     bool push_file, 
		                     op_proc_t cont);
	int ztokenScannerOptions(XWPSRef *upref, int old_options);
	
	bool objIdentEq(XWPSRef * pref1, XWPSRef * pref2);
	int objLe(XWPSRef * op1, XWPSRef * op);
	int zand();
	int zbitShift();
	int zeq();
	int zge();
	int zgt();
	int zidenteq();
	int zidentne();
	int zle();
	int zlt();
	int zmax();
	int zmin();
	int zne();
	int znot();
	int zor();
	int zxor();
	
	int condContinue();
	uint countExecStack(bool include_marks);
	uint countToStopped(long mask);
	int doExecStack(bool include_marks, XWPSRef * op1);
	int endSuperExec() {in_superexec--;return 0;}
	int execStackContinue();
	int execStack2Continue();
	int forFractionContinue();
	int forNegIntContinue();
	int forPosIntContinue();	
	int forRealContinue();
	int loopContinue();
	int noCleanup() {return 0;}
	int pushExecStack(XWPSRef * op1, bool include_marks, op_proc_t cont);
	int repeatContinue();
	int stoppedPush();
	int unmatchedExit(XWPSRef * op, op_proc_t opproc);
	int zcond();	
	int zcountExecStack();
	int zcountExecStack1();
	int zcurrentFile();
	int zexec();
	int zexecn();
	int zexecStack();
	int zexecStack2();
	int zexit();
	int zfor();
	int zforFraction();
	XWPSRef * zgetCurrentFile();
	int zif();
	int zifelse();
	int zinstopped();
	int zloop();
	int zneedInput();
	int zquit();
	int zrepeat();
	int zstop();
	int zstopped();
	int zsuperExec();
	int zzstop();
	int zzstopped();
	
	int  accessCheck(int access, bool modify);
	int  convertToString(XWPSRef * op1, XWPSRef * op);
	void ensureDot(char *buf);
	int  objCvp(XWPSRef * op, uchar * str, 
	             uint len, uint * prlen,
	             int full_print, uint start_pos);
	int objCvs(XWPSRef * op, uchar * str, uint len, uint * prlen, const uchar ** pchars);
	int objEq(XWPSRef * pref1, XWPSRef * pref2);
	int objStringData(XWPSRef *op, const uchar **pchars, uint *plen);
	int zcvi();
	int zcvlit();
	int zcvn();
	int zcvr();
	int zcvrs();
	int zcvs();
	int zcvx();
	int zexecuteOnly();
	int znoaccess();
	int zrcheck();
	int zreadOnly();
	int ztype();
	int ztypeNames();
	int zwcheck();
	int zxcheck();
	
	int copyErrorString(XWPSRef *fop);
	int execFileCleanup();
	int execFileFinish();
	int fileCleanup();
	int fileContinue();
	int fileReadString(const uchar *str, uint len, XWPSRef *pfile);
	int fileSwitchToRead(XWPSRef * op);
	int fileSwitchToWrite(XWPSRef * op);
	int filterOpen(const char *file_access, 
	                uint buffer_size, 
	                XWPSRef * pfile,
	                PSStreamProc * procs, 
	                const PSStreamTemplate * templat,
	                const PSStreamState * st);
	int handleIntc(XWPSRef *pstate, int nstate, op_proc_t cont);
	int handleReadException(int status, 
	                        XWPSRef * fop,
                          XWPSRef * pstate, 
                          int nstate, 
                          op_proc_t cont);
	int handleReadStatus(int ch, 
	                      XWPSRef * fop,
		                    const uint * pindex, 
		                    op_proc_t cont);
	int handleWriteException(int status, 
	                         XWPSRef * fop,
                           XWPSRef * pstate, 
                           int nstate, 
                           op_proc_t cont);
	int handleWriteStatus(int ch, 
	                      XWPSRef * fop,
		                    const uint * pindex, 
		                    op_proc_t cont);
	int libFileOpen(const char *fname, 
	                uint len, 
	                uchar * cname, 
	                uint max_clen,
	                 uint * pclen, 
	                 XWPSRef * pfile);
	QIODevice * libFOpen(const char *bname);
	void makeInvalidFile(XWPSRef * fp);
	int  makeRFS(XWPSRef * op, XWPSStream *fs, long offset, long length);
	int  makeRSS(XWPSRef * op, 
	             const uchar * data, 
	             uint size,
	             int string_space, 
	             long offset, 
	             long length, 
	             bool is_bytestring);
	void makeStreamFile(XWPSRef * pfile, XWPSStream * s, const char *access);
	int parseFileName(XWPSRef * op, XWPSParsedFileName * pfn);
	int parseRealFileName(XWPSRef *op, XWPSParsedFileName *pfn);
	int procReadContinue();
	int procWriteContinue();
	int writeString(XWPSRef * op, XWPSStream * s);
	int zbytesAvailable();
	int zcloseFile();
	int zdeleteFile();
	int zecho();
	int zexecFile();
	int zfile();
	int zfileInit();
	int zfileName();
	int zfileNameDirSeparator();
	int zfileNameForall();
	int zfileNameListSeparator();
	int zfileNameSplit();
	int zfilePosition();
	int zflush();
	int zflushFile();
	int zgetStderr(XWPSStream ** ps);
	int zgetStdin(XWPSStream ** ps);
	int zgetStdout(XWPSStream ** ps);
	int zisProcFilter();
	int zlibFile();
	int zopenFile(const XWPSParsedFileName *pfn, const char *file_access, XWPSStream **ps);
	int zpeekString();
	int zprint();
	int zread();
	int zreadHexString();
	int zreadHexStringAt(XWPSRef * op, uint start);
	int zreadHexStringContinue();
	int zreadLine();
	int zreadLineAt(XWPSRef * op, uint count, bool in_eol);
	int zreadLineContinue();
	int zreadLineFrom(XWPSStream *s, XWPSString *buf, uint *pcount, bool *pin_eol);
	int zreadString();
	int zreadStringAt(XWPSRef * op, uint start);
	int zreadStringContinue();
	int zrenameFile();
	int zresetFile();
	int zreusableStream();
	int zrsdParams();
	int zsetFilePosition();
	int zstatus();
	int zunread();
	int zwrite();
	int zwriteCvp();
	int zwriteCvpAt(XWPSRef * op, uint start, bool first);
	int zwriteCvpContinue();
	int zwriteHexString();
	int zwriteHexStringAt(XWPSRef * op, uint odd);
	int zwriteHexStringContinue();
	int zwriteString();
	int zxfilePosition();
	
	int zcurrentAccurateCurves();
	int zcurrentBool(bool (XWPSState::*current_proc)());
	int zcurrentCurveJoin();
	int zcurrentDash();
	int zcurrentDashAdapt();
	int zcurrentDotLength();
	int zcurrentFillAdjust2();
	int zcurrentFlat();
	int zcurrentLimitClamp();
	int zcurrentLineCap();
	int zcurrentLineJoin();
	int zcurrentLineWidth();
	int zcurrentMiterLimit();
	int zdotOrientation();
	int zinitGraphics();
	int zgrestore();
	int zgrestoreAll();
	int zgsave();
	int zsetAccurateCurves();
	int zsetBool(void (XWPSState::*set_proc)(bool));
	int zsetCurveJoin();
	int zsetDash();
	int zsetDashAdapt();
	int zsetDotLength();
	int zsetDotOrientation();
	int zsetFillAdjust2();
	int zsetFlat();
	int zsetLimitClamp();
	int zsetLineCap();
	int zsetLineJoin();
	int zsetLineWidth();
	int zsetMiterLimit();
	int zsetReal(void (XWPSState::*set_proc)(float));
		
	int beginComposite(XWPSAlphaCompositeState * pcp);
	int cacheAbcCommon(XWPSCieABC * pcie,
		                 XWPSRefCieProcs * pcprocs,
		                 void *container);
	int cacheColorRendering1(XWPSCieRender * pcrd,
		      								XWPSRefCieRenderProcs * pcrprocs);
	int cacheCommon(XWPSCie * pcie,
	                XWPSRefCieProcs * pcprocs,
	                void *container);
	int cieAbcParam(XWPSRef * pdref, 
	                 XWPSCieABC * pcie, 
	                 XWPSRefCieProcs * pcprocs);
	int cieCacheFinish();
	int cieCacheFinish1();
	int cieCacheFinishStore(bool replicate);
	int cieCacheJoint(XWPSRefCieRenderProcs * pcrprocs,
		                XWPSCie *pcie, 
		                XWPSState * pgsA);
	int cieCachePushFinish(op_proc_t finish_proc, void *data);
	int cieCacheRenderFinish();
	int cieAFinish();
	int cieABCFinish();
	int cieDEFFinish();
	int cieDEFGFinish();
	int cieExecTpqr();
	int cieLmnpParam(XWPSRef * pdref, 
	                 XWPSCie * pcie, 
	                 XWPSRefCieProcs * pcprocs);	
	int ciePointsParam(XWPSRef * pdref, PSCieWB * pwb);
	int ciePostExecTpqr();
	int ciePrepareCache(PSRange * domain, 
	                    XWPSRef * proc,
		                  PSCieCacheFloats * pcache, 
		                  void *container);
	int ciePrepareCaches4(PSRange * domains,
		                    XWPSRef * procs,
		                    PSCieCacheFloats * pc0, 
		                    PSCieCacheFloats * pc1,
		                    PSCieCacheFloats * pc2, 
		                    PSCieCacheFloats * pc3,
		                    void *container);
	int cieTableParam(XWPSRef * ptref, XWPSColorLookupTable * pclt);
	int cieTpqrFinish();
	int cie3dTableParam(XWPSRef * ptable, 
	                     uint count, 
	                     uint nbytes,
		                   XWPSString * strings);
	int compositeImage(const PSCompositeAlphaParams * params);
	int deviceNRemapCleanup();
	int deviceNRemapFinish();
	int deviceNRemapPrepare();
	int dictMatrix3Param(XWPSRef *pdref, 
	                     const char *kstr, 
	                     PSMatrix3 *pmat3);
	int dictProcArrayParam(XWPSRef * pdict, 
	                          const char *kstr,
		                        uint count, 
		                        XWPSRef * pparray);
	int dictProc3Param(XWPSRef *pdref, const char *kstr, XWPSRef proc3[3]);
	int dictRangesParam(XWPSRef * pdref, 
	                    const char *kstr, 
	                    int count,
		                  PSRange * prange);
	int dictRange3Param(XWPSRef *pdref, 
	                    const char *kstr, 
	                    PSRange3 *prange3);	
	void endComposite(XWPSAlphaCompositeState * pcp);
	int indexedMap1();
	int loadColorParams(XWPSRef * op, 
	                    PSPaintColor * pc, 
	                    XWPSColorSpace * pcs);
	int patternPaintCleanup();
	int patternPaintFinish();
	int patternPaintPrepare();
	int setCieFinish(XWPSColorSpace * pcs,
	       					 XWPSRefCieProcs * pcprocs, 
	       					 int edepth, 
	       					 int code);
	int separationMap1();
	int storeColorParams(XWPSRef * op, 
	                     const PSPaintColor * pc,
		                   XWPSColorSpace * pcs);
	int zalphaImage();
	int zbuildColorRendering1();
	int zbuildDeviceColorRendering1();
	int zbuildPattern1();
	int zcolorImage();
	int zcolorRemapColor();
	int zcolorRemapOne(XWPSRef * pproc,
		                 XWPSTransferMap * pmap, 
		                 const XWPSState * pgsA,
		                 op_proc_t finish_proc);
	int zcolorRemapOneFinish();
	int zcolorRemapOneSignedFinish();
	int zcolorRemapOneStore(float min_value);
	int zcolorResetTransfer();
	int zcomposite();
	int zcompositeRect();
	int zcrd1Params(XWPSRef * op, 
	                 XWPSCieRender * pcrd,
	                 XWPSRefCieRenderProcs * pcprocs);
	int zcrd1ProcParams(XWPSRef * op, XWPSRefCieRenderProcs * pcprocs);
	int zcsBeginMap(XWPSIndexedMap ** pmap, 
	                 XWPSRef * pproc,
	                 int num_entries,  
	                 XWPSColorSpace * base_space,
	      						op_proc_t map1);
	int zcurrentAlpha();
	int zcurrentBlackGeneration();
	int zcurrentCMYKColor();
	int zcurrentColor();
	int zcurrentColorRendering();
	int zcurrentColorSpace();
	int zcurrentColorTransfer();
	int zcurrentGray();
	int zcurrentHSBColor();
	int zcurrentOverprint();
	int zcurrentOverprintMode();
	int zcurrentRGBColor();
	int zcurrentTransfer();
	int zcurrentUnderColorRemoval();
	int zdissolve();
	int zpcolorInit();
	int zprocessColors();
	int zsetAlpha();
	int zsetBlackGeneration();
	int zsetCieASpace();
	int zsetCieABCSpace();
	int zsetCieDEFSpace();
	int zsetCieDEFGSpace();
	int zsetCMYKColor();
	int zsetColor();
	int zsetColorRendering1();
	int zsetColorSpace();
	int zsetColorTransfer();
	int zsetDeviceColorRendering1();
	int zsetDeviceNSpace();
	int zsetDevicePixelSpace();
	int zsetGray();	
	int zsetHSBColor();
	int zsetIndexedSpace();
	int zsetOverprint();
	int zsetOverprintMode();
	int zsetPatternSpace();
	int zsetRGBColor();
	int zsetSeparationSpace();
	int zsetTransfer();
	int zsetUnderColorRemoval();	
	int zsizeImageBox();
	int zsizeImageParams();
	int xywhParam(XWPSRef * op, double rect[4]);
	
	int dictRealResult(XWPSRef * pdict, const char *kstr, float val);
	int dictSpotParams(XWPSRef * pdict, 
	                   XWPSSpotHalftone * psp,
		                 XWPSRef * psproc, 
		                 XWPSRef * ptproc);
	int dictSpotResults(XWPSRef * pdict, const XWPSSpotHalftone * psp);
	int dictThresholdCommonParams(XWPSRef * pdict,
			                          XWPSThresholdHalftoneCommon * ptp,
			                          XWPSRef **pptstring, 
			                          XWPSRef *ptproc);
	int dictThresholdParams(XWPSRef * pdict, 
	                        XWPSThresholdHalftone * ptp,
		                      XWPSRef * ptproc);
	int dictThreshold2Params(XWPSRef * pdict, 
	                         XWPSThresholdHalftone2 * ptp,
		                       XWPSRef * ptproc);
	int screenCleanup();
	int screenSample();
	int setColorScreenCleanup();
	int setColorScreenFinish();
	int setHalftoneCleanup();
	int setHalftoneFinish();
	int setScreenContinue();
	int setScreenFinish();
	int zcurrentHalftone();
	int zcurrentScreenLevels();
	int zcurrentScreenPhase();
	int zimage2();
	int zscreenEnumInit(XWPSHTOrder * porder,
		                   XWPSScreenHalftone * psp, 
		                   XWPSRef * pproc, 
		                   int npop,
		                   op_proc_t finish_proc);	
	int zscreenParams(XWPSRef * op, XWPSScreenHalftone * phs);	
	int zsetColorScreen();
	int zsetHalftone5();
	int zsetScreen();	
	int zsetScreenPhase();
	
	int buildFunction0(XWPSRef *op, 
	                   XWPSFunctionParams * mnDR,
		                 int depth, 
		                 XWPSFunction ** ppfn);
	int buildFunction2(XWPSRef *op, 
	                   XWPSFunctionParams * mnDR,
		                 int depth, 
		                 XWPSFunction ** ppfn);
	int buildFunction3(XWPSRef *op, 
	                   XWPSFunctionParams * mnDR,
		                 int depth, 
		                 XWPSFunction ** ppfn);
	int buildFunction4(XWPSRef *op, 
	                   XWPSFunctionParams * mnDR,
		                 int depth, 
		                 XWPSFunction ** ppfn);
	int fnbuildFloatArray(XWPSRef * op, 
	                      const char *kstr, 
	                      bool required,
		                    bool even, 
		                    float **pparray);
	int checkPSCFunction(XWPSRef *pref, int depth, uchar *ops, int *psize);
	int fnbuildFunction(XWPSRef * op, XWPSFunction ** ppfn);
	int fnbuildSubFunction(XWPSRef * op, 
	                       XWPSFunction ** ppfn, 
	                       int depth);
	XWPSFunction * refFunction(XWPSRef *op);
	int zbuildFunction();
	int zexecFunction();
	
	int buildDirectionalShading(XWPSRef * op, 
	                            float *Coords, 
	                            int num_Coords,
			                        float Domain[2], 
			                        XWPSFunction ** pFunction,
			                        bool Extend[2]);
	int buildMeshShading(XWPSRef * op, 
	                     XWPSShadingMeshParams * params,
		                   float **pDecode, 
		                   XWPSFunction ** pFunction);
	int buildShading(build_shading_proc_t proc);
	int buildShadingFunction(XWPSRef * op, XWPSFunction ** ppfn, int num_inputs);
	int buildShading1(XWPSRef * op, 
	                  const XWPSShadingParams * pcommon,
		                XWPSShading ** ppsh);
	int buildShading2(XWPSRef * op, 
	                  const XWPSShadingParams * pcommon,
		                XWPSShading ** ppsh);
	int buildShading3(XWPSRef * op, 
	                  const XWPSShadingParams * pcommon,
		                XWPSShading ** ppsh);
	int buildShading4(XWPSRef * op, 
	                  const XWPSShadingParams * pcommon,
		                XWPSShading ** ppsh);
	int buildShading5(XWPSRef * op, 
	                  const XWPSShadingParams * pcommon,
		                XWPSShading ** ppsh);
	int buildShading6(XWPSRef * op, 
	                  const XWPSShadingParams * pcommon,
		                XWPSShading ** ppsh);
	int buildShading7(XWPSRef * op, 
	                  const XWPSShadingParams * pcommon,
		                XWPSShading ** ppsh);
	int flagBitsParam(XWPSRef * op, 
	                  XWPSShadingMeshParams * params,
		                int *pBitsPerFlag);
	int shadingParam(XWPSRef * op, XWPSShading ** ppsh);
	int zbuildShadingPattern();
	int zbuildShading1();
	int zbuildShading2();
	int zbuildShading3();
	int zbuildShading4();
	int zbuildShading5();
	int zbuildShading6();
	int zbuildShading7();
	int zcurrentSmoothness();	
	int zsetSmoothness();
	int zshfill();
	
	int currentFloatValue(float (XWPSState::*current_value)());
	int enumParam(XWPSRef *pnref, const char *const names[]);
	int maskDictParam(XWPSRef * op, 
	                  XWPSImageParams *pip_data, 
	                  const char *dict_name,
		                int num_components, 
		                XWPSImage3xMask *pixm);
	int maskOp(int (XWPSState::*mask_proc)(PSTransparencyChannelSelector));
	int rectParam(XWPSRect *prect, XWPSRef * op);
	int setFloatValue(int (XWPSState::*set_value)(float));
	int zbeginTransparencyGroup();
	int zbeginTransparencyMask();
	int zcurrentBlendMode();
	int zcurrentOpacityAlpha();
	int zcurrentRasterOp();
	int zcurrentShapeAlpha();
	int zcurrentSourceTransparent();
	int zcurrentTextKnockout();
	int zcurrentTextureTransparent();
	int zdiscardTransparencyGroup();
	int zdiscardTransparencyMask();
	int zendTransparencyGroup();
	int zendTransparencyMask();
	int zimage3x();
	int zinitTransparencyMask();
	int zsetBlendMode();
	int zsetOpacityAlpha();
	int zsetRasterOp();
	int zsetShapeAlpha();
	int zsetSourceTransparent();
	int zsetTextKnockout();
	int zsetTextureTransparent();
	
	int dataImageParams(XWPSRef *op, 
	                    XWPSDataImage *pim,
		                  XWPSImageParams *pip, 
		                  bool require_DataSource,
		                  int num_components, 
		                  int max_bits_per_component);
	int imageCleanup();
	int imageFileContinue();
	int imageProcContinue();
	int imageProcProcess();
	int imageSetup(XWPSRef * op, 
	               XWPSImage1 * pim,
	               XWPSColorSpace * pcs, 
	               int npop);
	int imageStringContinue();
	int pixelImageParams(XWPSRef *op, 
	                     XWPSPixelImage *pim,
		                   XWPSImageParams *pip, 
		                   int max_bits_per_component);
	int processNonSourceImage(XWPSImageCommon * pic);
	int zimage();
	int zimageDataSetup(const XWPSPixelImage * pim,
		                  XWPSImageEnumCommon * pie, 
		                  XWPSRef * sources, 
		                  int npop);
	int zimageMask();
	int zimageMask1();
	int zimageMultiple(bool has_alpha);
	int zimageOpaqueSetup(XWPSRef * op, 
	                      bool multi,
		                    PSImageAlpha alpha, 
		                    XWPSColorSpace * pcs,
		                    int npop);
	XWPSRef * zimagePopEStack(XWPSRef * tep);
	int zimageSetup(XWPSPixelImage * pim,
	                XWPSRef * sources, 
	                bool uses_color, 
	                int npop);
	int zimage1();
	int zimage3();
	int zimage4();
	
	int commonCurve(int (XWPSState::*add_proc)(float, float, float, float, float, float));
	int commonTo(int (XWPSState::*add_proc)(float, float));
	int zclip();
	int zclosePath();
	int zcurrentPoint();
	int zcurveTo();
	int zdefineUserName();
	int zeoclip();
	int zeoviewClip();
	int zinitClip();
	int zinitViewClip();
	int zlineTo();
	int zmoveTo();
	int znewPath();
	int zrcurveTo();
	int zrlineTo();
	int zrmoveTo();
	int zviewClip();
	int zviewClipPath();
	
	int commonArc(int (XWPSState::*aproc)(float, float, float, float, float));
	int commonArct(float *tanxy);
	int  pathCleanup();
	void pfPush(XWPSPoint * ppts, int n);
	int zarc();
	int zarcn();
	int zarct();
	int zarcTo();
	int zclipPath();
	int zdashPath();
	int zflattenPath();
	int zpathBbox();
	int pathContinue();
	int zpathForall();
	int zreversePath();
	int zstrokePath();
	
	int zeofill();
	int zfill();
	int zfillPage();
	int zimagePath();
	int zstroke();
	
	int gstateUnshare();
	int rectGet(XWPSLocalRects * plr, XWPSRef * op);
	int zcopyGState();
	int zcurrentGState();
	int zcurrentStrokeAdjust();
	int zgstate();
	int zrectAppend();
	int zrectClip();
	int zrectFill();
	int zrectStroke();
	int zsetBbox();
	int zsetGState();
	int zsetStrokeAdjust();
	int z1copy();
	
	int inPath(XWPSRef * oppath, XWPSDevice * phdev);
	int inPathResult(int npop, int code);
	int inTest(int (XWPSState::*paintproc)());
	int inUTest(int (XWPSState::*paintproc)());
	int inUPath(XWPSDevice * phdev);
	int inUPathResult(int npop, int code);
	int makeUPath(XWPSRef *rupath, 
	              XWPSState *pgsA, 
	              XWPSPath *ppath,
	              bool with_ucache);
	int upathAppend(XWPSRef * oppath);
	int upathStroke(XWPSMatrix *pmat);
	int zineofill();
	int zinfill();
	int zinstroke();
	int zinueofill();
	int zinufill();
	int zinustroke();
	int zuappend();
	int zucache() {return 0;}
	int zueofill();
	int zufill();
	int zupath();
	int zustroke();
	int zustrokePath();
	
	int cfontNameArrayCreate(XWPSRef * parray, const char * ksa,int size);
	int cfontNameCreate(XWPSRef * pnref, const char *str);
	int cfontNumDictCreate(XWPSRef * pdict,
		                     const XWPSCFontDictKeys * kp, 
		                     const char * ksa,
		                     XWPSRef * values, 
		                     const char *lengths);
	int cfontRefDictCreate(XWPSRef *pdict,
		                     const XWPSCFontDictKeys *kp, 
		                     const char * ksa,
		      								XWPSRef *values);
	int cfontRefFromString(XWPSRef * pref, const char *str, uint len);
	int cfontScalarArrayCreate(XWPSRef * parray,
			  										 XWPSRef *va, 
			  										 int size, 
			  										 uint attrs);
	int cfontStringArrayCreate(XWPSRef * parray,
			                      const char * ksa, 
			                      int size, 
			                      uint attrs);
	int cfontStringDictCreate(XWPSRef *pdict,
			                      const XWPSCFontDictKeys *kp, 
			                      const char * ksa,
			                       const char * kva);
	int zgetCCFont();
	int buildFDArrayFont(XWPSRef *op,
		      						 XWPSFontBase **ppfont,
		      						PSFontType ftype, 
		      						XWPSBuildProcRefs * pbuild);
	int buildFDArraySubFont(XWPSRef *op,
		       								XWPSFontBase **ppfont,
		       								PSFontType ftype,
		       								XWPSBuildProcRefs * pbuild,
		       								PSBuildFontOptions ignore_options);
	int buildFont(XWPSRef * op, 
	             XWPSFont ** ppfont, 
	             PSFontType ftype,
	             XWPSBuildProcRefs * pbuild,
	             PSBuildFontOptions options);
	int buildFontProcs(XWPSRef * op, XWPSBuildProcRefs * pbuild);
	int buildOutlineFont(XWPSRef * op, 
	                     XWPSFontBase ** ppfont,
		                   PSFontType ftype, 
		                   XWPSBuildProcRefs * pbuild,
		                   PSBuildFontOptions options,
		                   build_base_font_proc_t build_base_font);
	int buildPrimitiveFont(XWPSRef * op, 
	                       XWPSFontBase ** ppfont,
			                   PSFontType ftype, 
			                   XWPSBuildProcRefs * pbuild,
			                   PSBuildFontOptions options);
	int buildProcNameRefs(XWPSBuildProcRefs * pbuild,
		                    const char *bcstr, 
		                    const char *bgstr);
	int buildSimpleFont(XWPSRef * op, 
	                    XWPSFontBase ** ppfont,
		                  PSFontType ftype, 
		                  XWPSBuildProcRefs * pbuild,
		                  PSBuildFontOptions options);
	int buildSubFont(XWPSRef *op, 
	                 XWPSFont **ppfont,
		               PSFontType ftype, 
		               XWPSBuildProcRefs * pbuild, 
		               XWPSRef *pencoding,
		               XWPSRef *fid_op);
	int  addFID(XWPSRef * fp, XWPSFont * pfont, bool c);
	int  acquireCidSystemInfo(XWPSRef *psia, XWPSRef *op);
	int  buildCharstringFont(XWPSRef * op, 
	                         XWPSBuildProcRefs *pbuild,
		                       PSFontType ftype, 
		                       XWPSCharstringFontRefs *pfr,
		                       XWPSFontType1 *pdata1, 
		                       PSBuildFontOptions options);
	int  buildFont1Or4(XWPSRef * op, 
	                   XWPSBuildProcRefs * pbuild,
	                    PSFontType ftype, 
	                    PSBuildFontOptions options);
	int  buildTrueTypeFont(XWPSRef * op, 
	                       XWPSFontType42 **ppfont,
		       							 PSFontType ftype,
		                     const char *bcstr, 
		                     const char *bgstr,
		                     PSBuildFontOptions options);
	bool ccNoMarkGlyph(ulong , void *) {return false;}
	int  charstringFontGetRefs(XWPSRef * op, XWPSCharstringFontRefs *pfr);
	int  charstringFontParams(XWPSRef * op, 
	                          XWPSCharstringFontRefs *pfr,
		                        XWPSFontType1 *pdata1);
	int  cidFontDataParam(XWPSRef * op, XWPSFontCid0 *pdata, XWPSRef *pGlyphDirectory);
	int  cidFontDataParam(XWPSRef * op, XWPSFontCid2 *pdata, XWPSRef *pGlyphDirectory);
	int  cidFontSystemInfoParam(XWPSCidSystemInfo *pcidsi, XWPSRef *prfont);
	int  cidSystemInfoParam(XWPSCidSystemInfo *pcidsi, XWPSRef *prcidsi);
	void copyFontName(PSFontName * pfstr, XWPSRef * pfname);
	int  cshowContinue();
	int  cshowRestoreFont();
	int  defineFont(XWPSFont * pfont);
	int  ensureCharEntry(XWPSRef * op, 
	                     const char *kstr,
		                   uchar * pvalue, 
		                   int default_value);
	int  fdArrayElement(XWPSFontType1 **ppfont, XWPSRef *prfd);
	void findZoneHeight(float *pmax_height, int count, const float *values);
	int  fontGDirGetOutline(XWPSRef *pgdir, 
	                        long glyph_index,
		                      XWPSString * pgstr);
	int  fontGlyphDirectoryParam(XWPSRef * op, XWPSRef *pGlyphDirectory);
	int  fontParam(XWPSRef * pfdict, XWPSFont ** ppfont);
	int  fontStringArrayParam(XWPSRef * op, const char *kstr, XWPSRef *psa);
	int  getCidSystemInfo(XWPSCidSystemInfo *pcidsi, 
	                      XWPSRef *psia, 
	                      uint index);
	void getFontName(XWPSRef * pfname, XWPSRef * op);
	void lookupSimpleFontEncoding(XWPSFontBase * pfont);
	int  makeFont(const XWPSMatrix * pmat);
	void makeUintArray(XWPSRef * op, const uint * intp, int count);
	bool sameFontDict(XWPSFontData *pdata, 
	                  XWPSFontData *podata,
	                  const char *key);
	int  stringArrayAccessProc(XWPSRef *psa, 
	                           int modulus, 
	                           ulong offset,
			                       uint length, 
			                       const uchar **pdata);
	int  subFontParams(XWPSRef *op, XWPSMatrix *pmat, XWPSRef *pfname);
	int  type2FontParams(XWPSRef * op, 
	                     XWPSCharstringFontRefs *pfr,
		                   XWPSFontType1 *pdata1);
	int zbfontInit();
	int zbuildCMap();
	int zbuildFont0();
	int zbuildFont1();
	int zbuildFont2();
	int zbuildFont3();	
	int zbuildFont4();
	int zbuildFont9();
	int zbuildFont10();
	int zbuildFont11();
	int zbuildFont32();
	int zbuildFont42();
	int zcacheStatus();
	int zcharEnumerateGlyph(XWPSRef *prdict, int *pindex, ulong *pglyph);
	int zcshow();
	int zcurrentCacheParams();
	int zcurrentFont();
	bool z1fontInfoHas(XWPSRef *pfidict, const char *key, XWPSString *pmember);
	int  zfcmapGlyphName(ulong glyph, XWPSString *pstr, void *proc_data);
	int zfontInit();
	bool zfontMarkGlyphName(ulong glyph, void *ignore_data);
	int zmakeFont();
	int zregisterFont();
	int zrootFont();
	int zscaleFont();
	int zsetCacheLimit();
	int zsetCacheParams();
	int zsetFont();
	int ztype0GetCMap(XWPSCMap **ppcmap, 
	                  XWPSRef *pfdepvector,
		                XWPSRef *op);
	int ztype9MapCid();
	int ztype11MapCid();
	int zwriteFont9();
	
	int bboxContinue();
	int bboxDraw(int (XWPSState::*draw)());
	int bboxFill();
	int bboxFinish(int (XWPSContextState::*cont)());
	int bboxFinishFill();
	int bboxFinishStroke();
	int bboxGetSbwContinue();
	int bboxStroke();
	int charStringExecChar(int font_type_mask);
	int  charStringMakeNotdef(XWPSString *pstr, XWPSFont *font);
	bool charStringIsNotdefProc(XWPSRef *pcstr);
	int finishShow() {return 0;}
	int finishStringWidth();
	int fontBboxParam(XWPSRef * pfdict, double bbox[4]);
	void glyphRef(ulong glyph, XWPSRef * gref);
	int  glyphShowSetup(ulong *pglyph);
	bool mapGlyphToChar(XWPSRef * pgref, XWPSRef * pencoding, XWPSRef * pch);
	int  moveShow(bool have_x, bool have_y);
	int  nobboxContinue();
	int  nobboxDraw(int (XWPSState::*draw)());
	int  nobboxFill();
	int  nobboxFinish(XWPSType1execState * pcxs);
	int  nobboxStroke();
	int opShowCleanup();
	int opShowContinue();
	int opShowContinueDispatch(int npop, int code);
	int opShowContinuePop(int npop);
	int opShowEnumSetup();
	XWPSTextEnum * opShowFind();
	uint opShowFindIndex();
	int opShowFinishSetup(XWPSTextEnum * penum, int npop, op_proc_t endproc);
	int opShowFree(int code);
	int opShowRestore(bool for_error);
	int opShowReturnWidth(uint npop, double *pwidth);
	int opShowSetup(XWPSRef * op);
	int opType1Cleanup();
	void opType1Free();
	int type1CalloutDispatch(int (XWPSContextState::*cont)(), int num_args);
	int type1CallOtherSubr(XWPSType1execState * pcxs,
		     								 int (XWPSContextState::*cont) (),
		                     XWPSRef * pos);
	int type1ContinueDispatch(XWPSType1execState *pcxs,
			                     XWPSRef * pcref, 
			                     XWPSRef *pos, 
			                     int num_args);
	int type1ExecBbox(XWPSType1execState * pcxs, XWPSFont * pfont);
	int type1PushOtherSubr(XWPSType1execState *pcxs,
		                     int (XWPSContextState::*cont)(), 
		                     XWPSRef *pos);
	int type42Fill();
	int type42Finish(int (XWPSState::*cont)());
	int type42Stroke();
	int zashow();
	int zawidthShow();
	int zcharBoxPath();
	int zcharCharStringData(XWPSFont *font, XWPSRef *pgref, XWPSString *pstr);
	int zcharExecCharProc();
	int zcharGetMetrics(const XWPSFontBase * pbfont, 
	                    XWPSRef * pcnref,
		                  double psbw[4]);
	int zcharGetMetrics2(const XWPSFontBase * pbfont, 
	                     XWPSRef * pcnref,
		                   double pwv[4]);
	int zcharPath(int (XWPSState::*begin)(const uchar *, uint, bool, XWPSTextEnum **));
	int zcharPath();
	int zcharSetCache(const XWPSFontBase * pbfont,
		                XWPSRef * pcnref, 
		                const double psb[2],
		                const double pwidth[2], 
		                const XWPSRect * pbbox,
		                op_proc_t cont_fill, 
		                op_proc_t cont_stroke,
		                const double Metrics2_sbw_default[4]);
	int zcharStringOutline(XWPSFontType1 *pfont1, 
	                       XWPSRef *pgref,
		                     const XWPSString *pgstr,
		                     const XWPSMatrix *pmat, 
		                     XWPSPath *ppath);
	int zfontBbox();
	int zgetMetrics32();
	int zglyphShow();
	int zglyphWidth();
	int zkshow();
	int zmakeGlyph32();
	int zremoveGlyphs();
	int zsetCacheDevice();
	int zsetCacheDevice2();
	int zsetCharWidth();
	int zshow();
	int zstringWidth();
	int ztype1ExecChar();
	int ztype2ExecChar();
	int ztype42ExecChar();
	int zwidthShow();
	static int z1pop(void *callback_data, long * pf);
	static int z1push(void *callback_data, const long * pf, int count);
	int zxshow();
	int zxyshow();
	int zyshow();
	
	int filterEnsureBuf(XWPSStream ** ps, uint min_buf_size, bool writing);
	void filterMarkStrmTemp(XWPSRef * fop, int is_temp);
	int filterRead(int npop, 
	               const PSStreamTemplate * templat,
	                PSStreamState * st, 
	                uint space);
	int filterReadPredictor(int npop, const PSStreamTemplate * templat, PSStreamState * st);
	int filterReadSimple(const PSStreamTemplate * templat, PSStreamState * st);
	int filterWrite(int npop, 
	                const PSStreamTemplate * templat,
	                PSStreamState * st, 
	                uint space);
	int filterWritePredictor(int npop,
		       								 const PSStreamTemplate * templat, 
		       								 PSStreamState * st);
	int filterWriteSimple(const PSStreamTemplate * templat, PSStreamState * st);
	int filterZLib(PSStreamZLibState *pzls);
	int bhcSetup(XWPSRef * op, PSStreamBHCState * pbhcs);
	int btSetup(XWPSRef * op, PSStreamBTState * pbts);
	int bwbsSetup(XWPSRef * op, PSStreamBWBSState * pbwbss);
	int rlsetup(XWPSRef * dop, bool * eod);
	int zAXD();
	int zAXE();
	int zA85D();
	int zA85E();
	int zBCPD();
	int zBCPE();
	int zBHCD();
	int zBHCE();
	int zBTD();
	int zBTE();
	int zBWBSD();
	int zBWBSE();
	int zCFD();	
	int zCFE();
	int zcfsetup(XWPSRef * op, PSStreamCFState *pcfs);
	int zcomputeCodes();
	int zDCTD();
	int zDCTE();
	int zFlateD();
	int zFlateE();
	int zLZWD();
	int zLZWE();
	int zMD5E();
	int zMTFD();
	int zMTFE();
	int zNullE();
	int zPDiffD();
	int zPDiffE();
	int zpdsetup(XWPSRef * op, PSStreamPDiffState * ppds);
	int zPFBD();
	int zPNGPD();
	int zPNGPE();
	int zppsetup(XWPSRef * op, PSStreamPNGPState * ppps);
	int zPSSE();
	int zRLD();
	int zRLE();
	int zSFD();
	int zTBCPD();
	int zTBCPE();
	int zzlibD();
	int zzlibE();
	int zlzsetup(XWPSRef * op, PSStreamLZWState * plzs);
	
	int  pushCallout(const char *callout_name);
	bool restorePageDevice(XWPSState * pgs_old, XWPSState * pgs_new);
	bool savePageDevice(XWPSState *pgsA);
	int  zcallBeginPage();
	int zcallEndPage();
	int zcallInstall();
	int zcopyDevice2();
	int zcurrentDevice();
	int zcurrentPageDevice();
	int zcurrentShowPageCount();
	int zdeviceName();
	int zdoneShowPage();
	int zflushPage();
	int zgetBitsRect();
	int zgetDevice();
	int zgetDeviceParams(bool is_hardware);
	int zgetDeviceParams();
	int zgetDevParams();
	int zgetHardwareParams();
	int zgetIODevice();
	int zmakeWordImageDevice();
	int znullDevice();
	int zoutputPage();
	int zputDeviceParams();
	int zputDevParams();
	int zsetDevice();
	int zsetPageDevice();
	int z2copy();
	int z2copyGState();
	int z2currentGState();
	int z2grestore();
	int z2grestoreAll();
	int z2gsave();
	int z2gstate();
	int z2restore();
	int z2save();
	int z2setGState();
	
	int dscadobeHeader(XWPSParamList *plist, const XWPSDSCParser *pData);
	int dscboundingBox(XWPSParamList *plist, const XWPSDSCParser *pData);
	int dsccreator(XWPSParamList *plist, const XWPSDSCParser *pData);
	int dsccreationDate(XWPSParamList *plist, const XWPSDSCParser *pData);
	int dscfor(XWPSParamList *, const XWPSDSCParser *) {return 0;}
	int dscorientation(XWPSParamList *plist, const XWPSDSCParser *pData);
	int dscpage(XWPSParamList *plist, const XWPSDSCParser *pData);
	int dscpageBoundingBox(XWPSParamList *plist, const XWPSDSCParser *pData);
	int dscpageOrientation(XWPSParamList *plist, const XWPSDSCParser *pData);
	int dscpages(XWPSParamList *plist, const XWPSDSCParser *pData);
	int dscputBoundingBox(XWPSParamList *plist, const char *keyname, const CDSCBBOX *pbbox);
	int dscputInt(XWPSParamList *plist, const char *keyname, int value);
	int dscPutString(XWPSParamList *plist, const char *keyname, const char *string);
	int dsctitle(XWPSParamList *plist, const XWPSDSCParser *pData);
	int zinitializeDSCParser();
	int zparseDSCComments();
	
	void makeAdjustmentMatrix(const XWPSPoint * request, 
	                          const XWPSRect * medium,
		                        XWPSMatrix * pmat, 
		                        bool scale, 
		                        int rotate);
	int matchPageSize(const XWPSPoint * request, 
	                  const XWPSRect * medium, 
	                  int policy,
		                int orient, 
		                bool roll, 
		                float *best_mismatch, 
		                XWPSMatrix * pmat,
		                XWPSPoint * pmsize);
	int zmatchMedia();
	int zmatchPageSize();
	int zmatchPageSize(XWPSRef * pvreq, 
	                   XWPSRef * pvmed,
		                 int policy, 
		                 int orient, 
		                 bool roll,
		                 float *best_mismatch, 
		                 XWPSMatrix * pmat, 
		                 XWPSPoint * pmsize);
		                 
	int eexecParam(XWPSRef * op, ushort * pcstate);
	int setLanguageLevel(int new_level);
	int swapEntry(XWPSRef elt[2], XWPSRef * pdict, XWPSRef * pdict2);
	int swapLevelDict(const char *dict_name);
	int type1Crypt(int (*proc)(uchar *, const uchar *, uint, ushort *));
	int zbind();	
	int zclipRestore();
	int zclipSave();
	int zeqproc();
	int zexD();
	int zexE();	
	int zgetEnv();
	int zlanguageLevel();
	int zmakeOperator();
	int zoserrno();
	int zoserrorString();
	int zrealTime();
	int zserialNumber();
	int zsetDebug();
	int zsetLanguageLevel();
	int zsetOSErrno();
	int zsetSubstituteColorSpace();
	int ztype1Decrypt();
	int ztype1Encrypt();
	int zuserTime();
	
	int awaitLock();
	long contextUserTime();
	bool currentAccurateScreens();
	long currentBuildTime() {return 0;}
	bool currentByteOrder();
	long currentCurFontCache();
	long currentJobTimeout() {return 0;}
	long currentMaxDictStack();
	long currentMaxExecStack();
	long currentMaxFontCache();
	long currentMaxFontItem();
	long currentMaxGlobalVM();
	long currentMaxLocalVM() {return 0;}	
	long currentMaxOpStack();
	long currentMinFontCompress();
	long currentMinScreenLevels();
	int  currentParamList(const PSParamSet * pset, XWPSRef * psref);
	int  currentParams(const PSParamSet * pset);
	int  currentParam1(const PSParamSet * pset);
	void currentRealFormat(PSParamString * pval);
	long currentRevision();
	long currentVMThreshold()  {return 0;}
	long currentVMReclaim() {return -2;}
	long currentWaitTimeout() {return 0;}
	int doFork(XWPSRef * op, 
	          const XWPSRef * pstdin, 
	          XWPSRef * pstdout,
	          uint mcount, 
	          bool local);
	int finishJoin();
	int forkDone();
	int interpExit();
	int lockAcquire(XWPSRef *op, XWPSContext * pctx);
	int lockRelease(XWPSRef * op);
	int monitorCleanup();
	int monitorRelease();
	int setAccurateScreens(bool val);
	int setJobTimeout(long ) {return 0;}
	int setMaxFontCache(long val);
	int setMaxGlobalVM(long val);
	int setMaxLocalVM(long ) {return 0;}
	int setMaxDictStack(long val);
	int setMaxExecStack(long val);
	int setMaxFontItem(long val);
	int setMaxOpStack(long val);
	int setMinFontCompress(long val);
	int setMinScreenLevels(long val);
	int setParams(XWPSParamList * plist, const PSParamSet * pset);
	int setUserParams(XWPSRef *paramdict);
	int setVMReclaim(long ) {return 0;}
	int setVMThreshold(long ) {return 0;}
	int setWaitTimeout(long ) {return 0;}
	int specificVMOp(op_proc_t opproc, uint space);
	void stackCopy(XWPSRefStack * to, 
	               XWPSRefStack * from, 
	               uint count,
	               uint from_index);
	int zbosObject();
	int zbseqInit();
	int zcheckPassword();
	int zcondition();
	int zcontextInit() {return 0;}
	int zcurrentContext();
	int zcurrentGlobal();
	int zcurrentObjectFormat();
	int zcurrentSystemParams();
	int zcurrentUserParams();
	int zdetach();
	int zforgetSave();
	int zfork();
	int zgcheck();
	int zgetSystemParam();
	int zgetUserParam();
	int zglobalVMArray();
	int zglobalVMDict();
	int zglobalVMPackedArray();
	int zglobalVMString();
	int zinstallSystemNames();
	int zjoin();
	int zlock();
	int zlocalFork();
	int zlocalVMArray();
	int zlocalVMDict();
	int zlocalVMPackedArray();
	int zlocalVMString();
	int zmonitor();
	int znotify();
	int zrestore();
	int zsave();
	int zsetGlobal();
	int zsetObjectFormat();
	int zsetSystemParams();
	int zsetTrapParams();
	int zsetTrapZone();
	int zsetUserParams();
	int zsystemVMArray();
	int zsystemVMCheck();
	int zsystemVMDict();
	int zsystemVMPackedArray();
	int zsystemVMString();
	int zusertimeContext();
	int zvmreclaim();
	int zvmstatus();
	int zwait();
	int zyield() {return 0;}
		
private:
	int ccfontFProcs(int *pnum_fprocs, const ccfont_fproc ** pfprocs);
	int copyStack(XWPSRefStack * pstack, XWPSRef * arr);
	
	int dynamicMakeString(XWPSRef * pref, XWPSDynamicArea * pda, uchar * next);
	
	int encodeBinaryToken(XWPSRef *obj, 
	                      long *ref_offset,
		                    long *char_offset, 
		                    uchar *str);
	int errorInfoPutString(const char *str);
	
	int  index();
	void init();
	int  init2();
	int  initialEnterName(const char *nstr, XWPSRef * pref);	
	int  initialEnterNameIn(XWPSRef *pdict, const char *nstr, XWPSRef * pref);
	void initialRemoveName(const char *nstr);
	int  initState();
	
	int load();
	
	XWPSRef * makeInitialDict(const char *iname, XWPSRef idicts[]);
	void makeOper(XWPSRef * opref, op_proc_t proc, int idx);
	
	int numArrayFormat(XWPSRef * op);
	int numArrayGet(XWPSRef * op, int format, uint index, XWPSRef * np);
	uint numArraySize(XWPSRef * op, int format);
	
	int objInit();
	int opInit();
	int zopInit();
	
	bool recognizeBtokens();
	int  runInitFile();
	
	int scanBinaryToken(XWPSStream *s, 
	                    XWPSRef *pref,
		  								XWPSScannerState *pstate);
	int scanBinGetName(XWPSRef *pnames, int index, XWPSRef *pref);
	int scanBinNumArrayContinue(XWPSStream * s, 
	                            XWPSRef * pref,
			                        XWPSScannerState * pstate);
	int scanBinStringContinue(XWPSStream * s, 
	                          XWPSRef * pref,
			 											XWPSScannerState * pstate);
	int scanBosContinue(XWPSStream * s, 
	                    XWPSRef * pref,
		                  XWPSScannerState * pstate);
	uchar * scanBosResize(XWPSScannerState * pstate, 
	                      uint new_size,
												uint index);
	int scanBosStringContinue(XWPSStream * s, 
	                          XWPSRef * pref,
			 										  XWPSScannerState * pstate);
	int scanComment(XWPSRef *pref, 
	                XWPSScannerState *pstate,
	                uchar * base, 
	                uchar * end, 
	                bool saved);
	int scanHandleRefill(XWPSRef * fop, 
	                     XWPSScannerState * sstate,
		                   bool save, 
		                   bool push_file, 
		                   op_proc_t cont);
	int scanNumber(uchar * str, 
	               uchar * end, 
	               int sign,
	               XWPSRef * pref, 
	               uchar ** psp);            
	int scanStringToken(XWPSRef * pstr, XWPSRef * pref);
	int scanStringTokenOptions(XWPSRef * pstr, XWPSRef * pref, int options);
	int scanToken(XWPSStream * s, XWPSRef * pref, XWPSScannerState * pstate);
	
private:
	XWPSDevice * dev;
	QList<XWPSDevice *> device_list;
	QList<XWPSDevice *> device_instances;
	XWPSDualMemory * memory;
			
	ulong ps_next_id;
	ulong saved_next_id;
	
	XWPSState * pgs;
	int language_level;
	XWPSRef array_packing;
	XWPSRef binary_object_format;
	long rand_state;
	long rand_state_initial;
	long usertime_total;
	bool keep_usertime;
	int in_superexec;
	XWPSRef userparams;
	int scanner_options;
	XWPSDictStack dict_stack;
	XWPSExecStack exec_stack;
	XWPSOpStack op_stack;
	
	XWPSRef * system_names_p;
	XWPSRef * user_names_p;
	
	uint name_max_string;
	
	XWPSNameTable * the_name_table;
	
	bool dict_auto_expand;
	
	XWPSOpArrayTable op_array_table_global;
	XWPSOpArrayTable op_array_table_local;
	
	XWPSStream invalid_file_stream;
	
	XWPSRef registered_Encodings;
	
	int device_count;
	XWPSDevice ** ps_device_list;
	
	XWPSIODevice ** io_device_table;
	int io_device_table_count;
	
	XWPSFontDir * ifont_dir;
	
	XWPSRef stdio[3];
	
	QHash<ulong, XWPSState*> saved_states;
	QHash<ulong, ulong> pagenextids;
	
	static PSBuildFunctionType  build_function_type_table[4];
	
	static int ps_interp_num_special_ops;
	static int tx_next_index;
	static PSOperator * op_defs_all[];
	static uint op_def_count;
	static const ccfont_fproc fprocs[];
	
	static int zcolor_remap_one_ostack;
	static int zcolor_remap_one_estack;
};

#endif //XWPSCONTEXTSTATE_H
