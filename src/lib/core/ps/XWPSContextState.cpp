/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <QCoreApplication>
#include "XWStringUtil.h"
#include "XWApplication.h"
#include "XWPictureSea.h"
#include "XWDocSea.h"
#include "XWFontSea.h"
#include "XWDviPsSpecial.h"
#include "XWDVICore.h"
#include "XWDVIRef.h"
#include "XWDVIDev.h"
#include "XWPSError.h"
#include "XWPSStream.h"
#include "XWPSName.h"
#include "XWPSDevicePDF.h"
#include "XWPSDeviceDVI.h"
#include "XWPSState.h"
#include "XWPSIODevice.h"
#include "XWPSParam.h"
#include "XWPSTransferMap.h"
#include "XWPSFont.h"
#include "XWPSContextState.h"

#ifndef SYSTEMDICT_SIZE
#  define SYSTEMDICT_SIZE 631
#endif
#ifndef SYSTEMDICT_LEVEL2_SIZE
#  define SYSTEMDICT_LEVEL2_SIZE 983
#endif
#ifndef LEVEL2DICT_SIZE
#  define LEVEL2DICT_SIZE 233
#endif
#ifndef LL3DICT_SIZE
#  define LL3DICT_SIZE 43
#endif
#ifndef FILTERDICT_SIZE
#  define FILTERDICT_SIZE 43
#endif
#ifndef OP_ARRAY_TABLE_SIZE
#  define OP_ARRAY_TABLE_SIZE 180
#endif
#ifndef OP_ARRAY_TABLE_GLOBAL_SIZE
#  define OP_ARRAY_TABLE_GLOBAL_SIZE OP_ARRAY_TABLE_SIZE
#endif
#ifndef OP_ARRAY_TABLE_LOCAL_SIZE
#  define OP_ARRAY_TABLE_LOCAL_SIZE (OP_ARRAY_TABLE_SIZE / 2)
#endif
#define OP_ARRAY_TABLE_TOTAL_SIZE\
  (OP_ARRAY_TABLE_GLOBAL_SIZE + OP_ARRAY_TABLE_LOCAL_SIZE)
  
int XWPSContextState::zcolor_remap_one_ostack = 4;
int XWPSContextState::zcolor_remap_one_estack = 3;

int XWPSContextState::ps_interp_num_special_ops = num_special_ops;
int XWPSContextState::tx_next_index = tx_next_op;

const struct {
    const char *name;
    uint size;
    bool local;
} initial_dictionaries[] = {
#ifdef INITIAL_DICTIONARIES
    INITIAL_DICTIONARIES
#else
    /* systemdict is created and named automagically */
    {
	"level2dict", LEVEL2DICT_SIZE, false
    },
    {
	"ll3dict", LL3DICT_SIZE, false
    },
    {
	"globaldict", 0, false
    },
    {
	"userdict", 0, true
    },
    {
	"filterdict", FILTERDICT_SIZE, false
    },
#endif
};

const char *const initial_dstack[] =
{
#ifdef INITIAL_DSTACK
    INITIAL_DSTACK
#else
    "userdict"
#endif
};

#define MIN_DSTACK_SIZE ((sizeof(initial_dstack) / sizeof(initial_dstack[0])) + 1)

PSOperator interp_op_defs[] = {
	op_def_begin_dict("systemdict"),
	{"2add", &XWPSContextState::zadd},
  {"2def", &XWPSContextState::zdef},
  {"1dup", &XWPSContextState::zdup},
  {"2exch", &XWPSContextState::zexch},
  {"2if", &XWPSContextState::zif},
  {"3ifelse", &XWPSContextState::zifelse},
  {"1index", &XWPSContextState::zindex},
  {"1pop", &XWPSContextState::zpop},
  {"2roll", &XWPSContextState::zroll},
  {"2sub", &XWPSContextState::zsub},
  {"0.currentstackprotect", &XWPSContextState::zcurrentStackProtect},
  {"1.setstackprotect", &XWPSContextState::zsetStackProtect},
  {"0%interp_exit", &XWPSContextState::interpExit},
  {"0%oparray_pop", &XWPSContextState::opArrayPop},
  op_def_end(0)
};

PSOperator zstack_op_defs[] =
{
    {"0clear", &XWPSContextState::zclearStack},
    {"0cleartomark", &XWPSContextState::zclearToMark},
    {"0count", &XWPSContextState::zcount},
    {"0counttomark", &XWPSContextState::zcountToMark},
    {"1dup", &XWPSContextState::zdup},
    {"2exch", &XWPSContextState::zexch},
    {"2index", &XWPSContextState::zindex},
    {"0mark", &XWPSContextState::zmark},
    {"1pop", &XWPSContextState::zpop},
    {"2roll", &XWPSContextState::zroll},
    op_def_end(0)
};

PSOperator zarith_op_defs[] =
{
    {"1abs", &XWPSContextState::zabs},
    {"2add", &XWPSContextState::zadd},
    {"2.bitadd", &XWPSContextState::zbitAdd},
    {"1ceiling", &XWPSContextState::zceiling},
    {"2div", &XWPSContextState::zdiv},
    {"2idiv", &XWPSContextState::zidiv},
    {"1floor", &XWPSContextState::zfloor},
    {"2mod", &XWPSContextState::zmod},
    {"2mul", &XWPSContextState::zmul},
    {"1neg", &XWPSContextState::zneg},
    {"1round", &XWPSContextState::zround},
    {"2sub", &XWPSContextState::zsub},
    {"1truncate", &XWPSContextState::ztruncate},
    op_def_end(0)
};

PSOperator zdouble1_op_defs[] = {
    {"3.dadd", &XWPSContextState::zdadd},
    {"3.ddiv", &XWPSContextState::zddiv},
    {"3.dmul", &XWPSContextState::zdmul},
    {"3.dsub", &XWPSContextState::zdsub},
    {"2.deq", &XWPSContextState::zdeq},
    {"2.dge", &XWPSContextState::zdge},
    {"2.dgt", &XWPSContextState::zdgt},
    {"2.dle", &XWPSContextState::zdle},
    {"2.dlt", &XWPSContextState::zdlt},
    {"2.dne", &XWPSContextState::zdne},
    {"2.cvd", &XWPSContextState::zcvd},
    {"2.cvsd", &XWPSContextState::zcvsd},
    {"1.dcvi", &XWPSContextState::zdcvi},
    {"1.dcvr", &XWPSContextState::zdcvr},
    {"2.dcvs", &XWPSContextState::zdcvs},
    op_def_end(0)
};

PSOperator zdouble2_op_defs[] = {
    {"2.dabs", &XWPSContextState::zdabs},
    {"2.dceiling", &XWPSContextState::zdceiling},
    {"2.dfloor", &XWPSContextState::zdfloor},
    {"2.dneg", &XWPSContextState::zdneg},
    {"2.dround", &XWPSContextState::zdround},
    {"2.dsqrt", &XWPSContextState::zdsqrt},
    {"2.dtruncate", &XWPSContextState::zdtruncate},
    {"2.darccos", &XWPSContextState::zdarccos},
    {"2.darcsin", &XWPSContextState::zdarcsin},
    {"3.datan", &XWPSContextState::zdatan},
    {"2.dcos", &XWPSContextState::zdcos},
    {"3.dexp", &XWPSContextState::zdexp},
    {"2.dln", &XWPSContextState::zdln},
    {"2.dlog", &XWPSContextState::zdlog},
    {"2.dsin", &XWPSContextState::zdsin},
    op_def_end(0)
};

PSOperator zmath_op_defs[] =
{
    {"1arccos", &XWPSContextState::zarccos},
    {"1arcsin", &XWPSContextState::zarcsin},
    {"2atan", &XWPSContextState::zatan},
    {"1cos", &XWPSContextState::zcos},
    {"2exp", &XWPSContextState::zexp},
    {"1ln", &XWPSContextState::zln},
    {"1log", &XWPSContextState::zlog},
    {"0rand", &XWPSContextState::zrand},
    {"0rrand", &XWPSContextState::zrrand},
    {"1sin", &XWPSContextState::zsin},
    {"1sqrt", &XWPSContextState::zsqrt},
    {"1srand", &XWPSContextState::zsrand},
    op_def_end(0)
};

PSOperator zmatrix_op_defs[] =
{
    {"1concat", &XWPSContextState::zconcat},
    {"2dtransform", &XWPSContextState::zdtransform},
    {"3concatmatrix", &XWPSContextState::zconcatMatrix},
    {"0.currentmatrix", &XWPSContextState::zcurrentMatrix},
    {"1defaultmatrix", &XWPSContextState::zdefaultMatrix},
    {"2idtransform", &XWPSContextState::zidtransform},
    {"0initmatrix", &XWPSContextState::zinitMatrix},
    {"2invertmatrix", &XWPSContextState::zinvertMatrix},
    {"2itransform", &XWPSContextState::zitransform},
    {"1rotate", &XWPSContextState::zrotate},
    {"2scale", &XWPSContextState::zscale},
    {"6.setmatrix", &XWPSContextState::zsetMatrix},
    {"1.setdefaultmatrix", &XWPSContextState::zsetDefaultMatrix},
    {"2transform", &XWPSContextState::ztransform},
    {"2translate", &XWPSContextState::ztranslate},
    op_def_end(0)
};

PSOperator zarray_op_defs[] =
{
    {"1aload", &XWPSContextState::zaload},
    {"1array", &XWPSContextState::zarray},
    {"1astore", &XWPSContextState::zastore},
    op_def_end(0)
};

PSOperator zpacked_op_defs[] =
{
    {"0currentpacking", &XWPSContextState::zcurrentPacking},
    {"1packedarray", &XWPSContextState::zpackedArray},
    {"1setpacking", &XWPSContextState::zsetPacking},
    op_def_end(0)
};

PSOperator zgeneric_op_defs[] =
{
    {"1copy", &XWPSContextState::zcopy},
    {"2forall", &XWPSContextState::zforall},
    {"3.forceput", &XWPSContextState::zforcePut},
    {"2get", &XWPSContextState::zget},
    {"3getinterval", &XWPSContextState::zgetInterval},
    {"1length", &XWPSContextState::zlength},
    {"3put", &XWPSContextState::zput},
    {"3putinterval", &XWPSContextState::zputInterval},
    {"0%array_continue", &XWPSContextState::arrayContinue},
    {"0%dict_continue", &XWPSContextState::dictContinue},
    {"0%packedarray_continue", &XWPSContextState::packedArrayContinue},
    {"0%string_continue", &XWPSContextState::stringContinue},
    op_def_end(0)
};

PSOperator zdict1_op_defs[] = {
    {"0cleardictstack", &XWPSContextState::zclearDictStack},
    {"1begin", &XWPSContextState::zbegin},
    {"0countdictstack", &XWPSContextState::zcountDictStack},
    {"0currentdict", &XWPSContextState::zcurrentDict},
    {"2def", &XWPSContextState::zdef},
    {"1dict", &XWPSContextState::zdict},
    {"0dictstack", &XWPSContextState::zdictStack},
    {"0end", &XWPSContextState::zend},
    {"2known", &XWPSContextState::zknown},
    {"1load", &XWPSContextState::zload},
    {"1maxlength", &XWPSContextState::zmaxlength},
    {"2.undef", &XWPSContextState::zundef},	
    {"1where", &XWPSContextState::zwhere},
    op_def_end(0)
};

PSOperator zdict2_op_defs[] = {
    {"2.dictcopynew", &XWPSContextState::zdictCopyNew},
    {"1.dicttomark", &XWPSContextState::zdictToMark},
    {"2.forceundef", &XWPSContextState::zforceUndef},
    {"2.knownget", &XWPSContextState::zknownGet},
    {"1.knownundef", &XWPSContextState::zknownUndef},
    {"2.setmaxlength", &XWPSContextState::zsetMaxLength},
    op_def_end(0)
};

PSOperator zstring_op_defs[] =
{
    {"1.bytestring", &XWPSContextState::zbyteString},
    {"2anchorsearch", &XWPSContextState::zanchorSearch},
    {"1.namestring", &XWPSContextState::znameString},
    {"2search", &XWPSContextState::zsearch},
    {"1string", &XWPSContextState::zstring},
    {"2.stringmatch", &XWPSContextState::zstringMatch},
    op_def_end(0)
};

PSOperator ztoken_op_defs[] =
{
    {"1token", &XWPSContextState::ztoken},
    {"1.tokenexec", &XWPSContextState::ztokenExec},
    {"2%ztokenexec_continue", &XWPSContextState::ztokenExecContinue},
    op_def_end(0)
};

PSOperator zrelbit_op_defs[] =
{
    {"2and", &XWPSContextState::zand},
    {"2bitshift", &XWPSContextState::zbitShift},
    {"2eq", &XWPSContextState::zeq},
    {"2ge", &XWPSContextState::zge},
    {"2gt", &XWPSContextState::zgt},
    {"2le", &XWPSContextState::zle},
    {"2lt", &XWPSContextState::zlt},
    {"2.max", &XWPSContextState::zmax},
    {"2.min", &XWPSContextState::zmin},
    {"2ne", &XWPSContextState::zne},
    {"1not", &XWPSContextState::znot},
    {"2or", &XWPSContextState::zor},
    {"2xor", &XWPSContextState::zxor},
    {"2.identeq", &XWPSContextState::zidenteq},
    {"2.identne", &XWPSContextState::zidentne},
    op_def_end(0)
};

PSOperator zcontrol1_op_defs[] = {
    {"1.cond", &XWPSContextState::zcond},
    {"0countexecstack", &XWPSContextState::zcountExecStack},
    {"1.countexecstack", &XWPSContextState::zcountExecStack1},
    {"0currentfile", &XWPSContextState::zcurrentFile},
    {"1exec", &XWPSContextState::zexec},
    {"1.execn", &XWPSContextState::zexecn},
    {"1execstack", &XWPSContextState::zexecStack},
    {"2.execstack", &XWPSContextState::zexecStack2},
    {"0exit", &XWPSContextState::zexit},
    {"2if", &XWPSContextState::zif},
    {"3ifelse", &XWPSContextState::zifelse},
    {"0.instopped", &XWPSContextState::zinstopped},
    {"0.needinput", &XWPSContextState::zneedInput},
    op_def_end(0)
};

PSOperator zcontrol2_op_defs[] = {
    {"4for", &XWPSContextState::zfor},
    {"1loop", &XWPSContextState::zloop},
    {"2.quit", &XWPSContextState::zquit},
    {"2repeat", &XWPSContextState::zrepeat},
    {"0stop", &XWPSContextState::zstop},
    {"1.stop", &XWPSContextState::zzstop},
    {"1stopped", &XWPSContextState::zstopped},
    {"2.stopped", &XWPSContextState::zzstopped},
    op_def_end(0)
};

PSOperator zcontrol3_op_defs[] = {
    {"1%cond_continue", &XWPSContextState::condContinue},
    {"1%execstack_continue", &XWPSContextState::execStackContinue},
    {"2%execstack2_continue", &XWPSContextState::execStack2Continue},
    {"0%for_pos_int_continue", &XWPSContextState::forPosIntContinue},
    {"0%for_neg_int_continue", &XWPSContextState::forNegIntContinue},
    {"0%for_real_continue", &XWPSContextState::forRealContinue},
    {"4%for_fraction", &XWPSContextState::zforFraction},
    {"0%for_fraction_continue", &XWPSContextState::forFractionContinue},
    {"0%loop_continue", &XWPSContextState::loopContinue},
    {"0%repeat_continue", &XWPSContextState::repeatContinue},
    {"0%stopped_push", &XWPSContextState::stoppedPush},
    {"1superexec", &XWPSContextState::zsuperExec},
    {"0%end_superexec", &XWPSContextState::endSuperExec},
    op_def_end(0)
};

PSOperator ztype_op_defs[] =
{
    {"1cvi", &XWPSContextState::zcvi},
    {"1cvlit", &XWPSContextState::zcvlit},
    {"1cvn", &XWPSContextState::zcvn},
    {"1cvr", &XWPSContextState::zcvr},
    {"3cvrs", &XWPSContextState::zcvrs},
    {"2cvs", &XWPSContextState::zcvs},
    {"1cvx", &XWPSContextState::zcvx},
    {"1executeonly", &XWPSContextState::zexecuteOnly},
    {"1noaccess", &XWPSContextState::znoaccess},
    {"1rcheck", &XWPSContextState::zrcheck},
    {"1readonly", &XWPSContextState::zreadOnly},
    {"2.type", &XWPSContextState::ztype},
    {"0.typenames", &XWPSContextState::ztypeNames},
    {"1wcheck", &XWPSContextState::zwcheck},
    {"1xcheck", &XWPSContextState::zxcheck},
    op_def_end(0)
};

PSOperator zfile_op_defs[] =
{
    {"1deletefile", &XWPSContextState::zdeleteFile},
    {"1.execfile", &XWPSContextState::zexecFile},
    {"2file", &XWPSContextState::zfile},
    {"3filenameforall", &XWPSContextState::zfileNameForall},
    {"2.filenamedirseparator", &XWPSContextState::zfileNameDirSeparator},
    {"0.filenamelistseparator", &XWPSContextState::zfileNameListSeparator},
    {"1.filenamesplit", &XWPSContextState::zfileNameSplit},
    {"1.libfile", &XWPSContextState::zlibFile},
    {"2renamefile", &XWPSContextState::zrenameFile},
    {"1status", &XWPSContextState::zstatus},
    {"0%file_continue", &XWPSContextState::fileContinue},
    {"0%execfile_finish", &XWPSContextState::execFileFinish},
    op_def_end(&XWPSContextState::zfileInit)
};

PSOperator zfileio1_op_defs[] = {
    {"1bytesavailable", &XWPSContextState::zbytesAvailable},
    {"1closefile", &XWPSContextState::zcloseFile},
    {"1echo", &XWPSContextState::zecho},
    {"1.filename", &XWPSContextState::zfileName},
    {"1.fileposition", &XWPSContextState::zxfilePosition},
    {"1fileposition", &XWPSContextState::zfilePosition},
    {"0flush", &XWPSContextState::zflush},
    {"1flushfile", &XWPSContextState::zflushFile},
    {"1.isprocfilter", &XWPSContextState::zisProcFilter},
    {"2.peekstring", &XWPSContextState::zpeekString},
    {"1print", &XWPSContextState::zprint},
    {"1read", &XWPSContextState::zread},
    {"2readhexstring", &XWPSContextState::zreadHexString},
    {"2readline", &XWPSContextState::zreadLine},
    {"2readstring", &XWPSContextState::zreadString},
    op_def_end(0)
};

PSOperator zfileio2_op_defs[] = {
    {"1resetfile", &XWPSContextState::zresetFile},
    {"2setfileposition", &XWPSContextState::zsetFilePosition},
    {"2.unread", &XWPSContextState::zunread},
    {"2write", &XWPSContextState::zwrite},
    {"3.writecvp", &XWPSContextState::zwriteCvp},
    {"2writehexstring", &XWPSContextState::zwriteHexString},
    {"2writestring", &XWPSContextState::zwriteString},
    {"3%zreadhexstring_continue", &XWPSContextState::zreadHexStringContinue},
    {"3%zreadline_continue", &XWPSContextState::zreadLineContinue},
    {"3%zreadstring_continue", &XWPSContextState::zreadStringContinue},
    {"4%zwritecvp_continue", &XWPSContextState::zwriteCvpContinue},
    {"3%zwritehexstring_continue", &XWPSContextState::zwriteHexStringContinue},
    op_def_end(0)
};

PSOperator zfproc_op_defs[] =
{
    {"2%s_proc_read_continue", &XWPSContextState::procReadContinue},
    {"2%s_proc_write_continue",&XWPSContextState::procWriteContinue},
    op_def_end(0)
};

PSOperator zfrsd_op_defs[] =
{
    {"2.reusablestream", &XWPSContextState::zreusableStream},
    {"2.rsdparams", &XWPSContextState::zrsdParams},
    op_def_end(0)
};

PSOperator zgstate1_op_defs[] = {
    {"0.currentaccuratecurves", &XWPSContextState::zcurrentAccurateCurves},
    {"0.currentcurvejoin", &XWPSContextState::zcurrentCurveJoin},
    {"0currentdash", &XWPSContextState::zcurrentDash},
    {"0.currentdashadapt", &XWPSContextState::zcurrentDashAdapt},
    {"0.currentdotlength", &XWPSContextState::zcurrentDotLength},
    {"0.currentfilladjust2", &XWPSContextState::zcurrentFillAdjust2},
    {"0currentflat", &XWPSContextState::zcurrentFlat},
    {"0.currentlimitclamp", &XWPSContextState::zcurrentLimitClamp},
    {"0currentlinecap", &XWPSContextState::zcurrentLineCap},
    {"0currentlinejoin", &XWPSContextState::zcurrentLineJoin},
    {"0currentlinewidth", &XWPSContextState::zcurrentLineWidth},
    {"0currentmiterlimit", &XWPSContextState::zcurrentMiterLimit},
    {"0.dotorientation", &XWPSContextState::zdotOrientation},
    {"0grestore", &XWPSContextState::zgrestore},
    {"0grestoreall", &XWPSContextState::zgrestoreAll},
    op_def_end(0)
};

PSOperator zgstate2_op_defs[] = {
    {"0gsave", &XWPSContextState::zgsave},
    {"0initgraphics", &XWPSContextState::zinitGraphics},
    {"1.setaccuratecurves", &XWPSContextState::zsetAccurateCurves},
    {"1.setcurvejoin", &XWPSContextState::zsetCurveJoin},
    {"2setdash", &XWPSContextState::zsetDash},
    {"1.setdashadapt", &XWPSContextState::zsetDashAdapt},
    {"2.setdotlength", &XWPSContextState::zsetDotLength},
    {"0.setdotorientation", &XWPSContextState::zsetDotOrientation},
    {"2.setfilladjust2", &XWPSContextState::zsetFillAdjust2},
    {"1.setlimitclamp", &XWPSContextState::zsetLimitClamp},
    {"1setflat", &XWPSContextState::zsetFlat},
    {"1.setlinecap", &XWPSContextState::zsetLineCap},
    {"1.setlinejoin", &XWPSContextState::zsetLineJoin},
    {"1setlinewidth", &XWPSContextState::zsetLineWidth},
    {"1setmiterlimit", &XWPSContextState::zsetMiterLimit},
    op_def_end(0)
};

PSOperator zcolor_op_defs[] =
{
    {"0currentgray", &XWPSContextState::zcurrentGray},
    {"0currentrgbcolor", &XWPSContextState::zcurrentRGBColor},
    {"0currenttransfer", &XWPSContextState::zcurrentTransfer},
    {"0processcolors", &XWPSContextState::zprocessColors},
    {"1setgray", &XWPSContextState::zsetGray},
    {"3setrgbcolor", &XWPSContextState::zsetRGBColor},
    {"1settransfer", &XWPSContextState::zsetTransfer},
    {"1%zcolor_remap_one_finish", &XWPSContextState::zcolorRemapOneFinish},
    {"1%zcolor_remap_one_signed_finish", &XWPSContextState::zcolorRemapOneSignedFinish},
    {"0%zcolor_reset_transfer", &XWPSContextState::zcolorResetTransfer},
    {"0%zcolor_remap_color", &XWPSContextState::zcolorRemapColor},
    op_def_end(0)
};

PSOperator zcolor1_op_defs[] =
{
    {"0currentblackgeneration", &XWPSContextState::zcurrentBlackGeneration},
    {"0currentcmykcolor", &XWPSContextState::zcurrentCMYKColor},
    {"0currentcolortransfer", &XWPSContextState::zcurrentColorTransfer},
    {"0currentundercolorremoval", &XWPSContextState::zcurrentUnderColorRemoval},
    {"1setblackgeneration", &XWPSContextState::zsetBlackGeneration},
    {"4setcmykcolor", &XWPSContextState::zsetCMYKColor},
    {"4setcolortransfer", &XWPSContextState::zsetColorTransfer},
    {"1setundercolorremoval", &XWPSContextState::zsetUnderColorRemoval},
    {"7colorimage", &XWPSContextState::zcolorImage},
    op_def_end(0)
};

PSOperator zcolor2_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"0currentcolor", &XWPSContextState::zcurrentColor},
    {"0.currentcolorspace", &XWPSContextState::zcurrentColorSpace},
    {"1setcolor", &XWPSContextState::zsetColor},
    {"1.setcolorspace", &XWPSContextState::zsetColorSpace},
    op_def_end(0)
};

PSOperator zcrd_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"0currentcolorrendering", &XWPSContextState::zcurrentColorRendering},
    {"2.setcolorrendering1", &XWPSContextState::zsetColorRendering1},
    {"2.setdevicecolorrendering1", &XWPSContextState::zsetDeviceColorRendering1},
    {"1.buildcolorrendering1", &XWPSContextState::zbuildColorRendering1},
    {"1.builddevicecolorrendering1", &XWPSContextState::zbuildDeviceColorRendering1},
    {"1%cie_render_finish", &XWPSContextState::cieCacheRenderFinish},
    {"3%cie_exec_tpqr", &XWPSContextState::cieExecTpqr},
    {"2%cie_post_exec_tpqr", &XWPSContextState::ciePostExecTpqr},
    {"1%cie_tpqr_finish", &XWPSContextState::cieTpqrFinish},
    op_def_end(0)
};

PSOperator zcie_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"1.setcieaspace", &XWPSContextState::zsetCieASpace},
    {"1.setcieabcspace", &XWPSContextState::zsetCieABCSpace},
    {"1.setciedefspace", &XWPSContextState::zsetCieDEFSpace},
    {"1.setciedefgspace", &XWPSContextState::zsetCieDEFGSpace},
    {"1%cie_defg_finish", &XWPSContextState::cieDEFGFinish},
    {"1%cie_def_finish", &XWPSContextState::cieDEFFinish},
    {"1%cie_abc_finish", &XWPSContextState::cieABCFinish},
    {"1%cie_a_finish", &XWPSContextState::cieAFinish},
    {"0%cie_cache_finish", &XWPSContextState::cieCacheFinish},
    {"1%cie_cache_finish1", &XWPSContextState::cieCacheFinish1},
    op_def_end(0)
};


PSOperator zcsdevn_op_defs[] =
{
    op_def_begin_ll3(),
    {"1.setdevicenspace", &XWPSContextState::zsetDeviceNSpace},
    {"0%devicen_remap_prepare", &XWPSContextState::deviceNRemapPrepare},
    op_def_end(0)
};

PSOperator zcsindex_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"1.setindexedspace", &XWPSContextState::zsetIndexedSpace},
    {"1%indexed_map1", &XWPSContextState::indexedMap1},
    op_def_end(0)
};

PSOperator zcspixel_op_defs[] =
{
    {"1.setdevicepixelspace", &XWPSContextState::zsetDevicePixelSpace},
    op_def_end(0)
};

PSOperator zcssepr_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"0currentoverprint", &XWPSContextState::zcurrentOverprint},
    {"0.currentoverprintmode", &XWPSContextState::zcurrentOverprintMode},
    {"1setoverprint", &XWPSContextState::zsetOverprint},
    {"1.setoverprintmode", &XWPSContextState::zsetOverprintMode},
    {"1.setseparationspace", &XWPSContextState::zsetSeparationSpace},
    {"1%separation_map1", &XWPSContextState::separationMap1},
    op_def_end(0)
};

PSOperator zdpnext_op_defs[] =
{
    {"0currentalpha", &XWPSContextState::zcurrentAlpha},
    {"1setalpha", &XWPSContextState::zsetAlpha},
    {"7alphaimage", &XWPSContextState::zalphaImage},
    {"8composite", &XWPSContextState::zcomposite},
    {"5compositerect", &XWPSContextState::zcompositeRect},
    {"8dissolve", &XWPSContextState::zdissolve},
    {"5.sizeimagebox", &XWPSContextState::zsizeImageBox},
    {"0.sizeimageparams", &XWPSContextState::zsizeImageParams},
    op_def_end(0)
};

PSOperator zpcolor_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"2.buildpattern1", &XWPSContextState::zbuildPattern1},
    {"1.setpatternspace", &XWPSContextState::zsetPatternSpace},
    {"0%pattern_paint_prepare", &XWPSContextState::patternPaintPrepare},
    {"0%pattern_paint_finish", &XWPSContextState::patternPaintFinish},
    op_def_end(&XWPSContextState::zpcolorInit)
};

PSOperator zhsb_op_defs[] =
{
    {"0currenthsbcolor", &XWPSContextState::zcurrentHSBColor},
    {"3sethsbcolor", &XWPSContextState::zsetHSBColor},
    op_def_end(0)
};

PSOperator zht_op_defs[] =
{
    {"0.currenthalftone", &XWPSContextState::zcurrentHalftone},
    {"0.currentscreenlevels", &XWPSContextState::zcurrentScreenLevels},
    {"3setscreen", &XWPSContextState::zsetScreen},
    {"0%screen_sample", &XWPSContextState::screenSample},
    {"1%set_screen_continue", &XWPSContextState::setScreenContinue},
    {"0%setscreen_finish", &XWPSContextState::setScreenFinish},
    op_def_end(0)
};

PSOperator zht1_op_defs[] =
{
    {"<setcolorscreen", &XWPSContextState::zsetColorScreen},
    {"0%setcolorscreen_finish", &XWPSContextState::setColorScreenFinish},
    op_def_end(0)
};

PSOperator zht2_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"2.sethalftone5", &XWPSContextState::zsetHalftone5},
    {"0%sethalftone_finish", &XWPSContextState::setHalftoneFinish},
    op_def_end(0)
};


PSOperator zdps_op_defs[] =
{
    {"1.currentscreenphase", &XWPSContextState::zcurrentScreenPhase},
    {"3.setscreenphase", &XWPSContextState::zsetScreenPhase},
    {"1.image2", &XWPSContextState::zimage2},
    {"0eoviewclip", &XWPSContextState::zeoviewClip},
    {"0initviewclip", &XWPSContextState::zinitViewClip},
    {"0viewclip", &XWPSContextState::zviewClip},
    {"0viewclippath", &XWPSContextState::zviewClipPath},
    {"2defineusername", &XWPSContextState::zdefineUserName},
    op_def_end(0)
};

PSOperator zfunc_op_defs[] =
{
    {"1.buildfunction", &XWPSContextState::zbuildFunction},
    {"1%execfunction", &XWPSContextState::zexecFunction},
    op_def_end(0)
};

PSOperator zshade_op_defs[] =
{
    op_def_begin_ll3(),
    {"0currentsmoothness", &XWPSContextState::zcurrentSmoothness},
    {"1setsmoothness", &XWPSContextState::zsetSmoothness},
    {"1.shfill", &XWPSContextState::zshfill},
    {"1.buildshading1", &XWPSContextState::zbuildShading1},
    {"1.buildshading2", &XWPSContextState::zbuildShading2},
    {"1.buildshading3", &XWPSContextState::zbuildShading3},
    {"1.buildshading4", &XWPSContextState::zbuildShading4},
    {"1.buildshading5", &XWPSContextState::zbuildShading5},
    {"1.buildshading6", &XWPSContextState::zbuildShading6},
    {"1.buildshading7", &XWPSContextState::zbuildShading7},
    {"3.buildshadingpattern", &XWPSContextState::zbuildShadingPattern},
    op_def_end(0)
};

PSOperator ztrans_op_defs[] = {
    {"1.setblendmode", &XWPSContextState::zsetBlendMode},
    {"0.currentblendmode", &XWPSContextState::zcurrentBlendMode},
    {"1.setopacityalpha", &XWPSContextState::zsetOpacityAlpha},
    {"0.currentopacityalpha", &XWPSContextState::zcurrentOpacityAlpha},
    {"1.setshapealpha", &XWPSContextState::zsetShapeAlpha},
    {"0.currentshapealpha", &XWPSContextState::zcurrentShapeAlpha},
    {"1.settextknockout", &XWPSContextState::zsetTextKnockout},
    {"0.currenttextknockout", &XWPSContextState::zcurrentTextKnockout},
    {"5.begintransparencygroup", &XWPSContextState::zbeginTransparencyGroup},
    {"0.discardtransparencygroup", &XWPSContextState::zdiscardTransparencyGroup},
    {"0.endtransparencygroup", &XWPSContextState::zendTransparencyGroup},
    {"5.begintransparencymask", &XWPSContextState::zbeginTransparencyMask},
    {"0.discardtransparencymask", &XWPSContextState::zdiscardTransparencyMask},
    {"1.endtransparencymask", &XWPSContextState::zendTransparencyMask},
    {"1.inittransparencymask", &XWPSContextState::zinitTransparencyMask},
    {"1.image3x", &XWPSContextState::zimage3x},
    op_def_end(0)
};

PSOperator zrop_op_defs[] =
{
    {"0.currentrasterop", &XWPSContextState::zcurrentRasterOp},
    {"0.currentsourcetransparent", &XWPSContextState::zcurrentSourceTransparent},
    {"0.currenttexturetransparent", &XWPSContextState::zcurrentTextureTransparent},
    {"1.setrasterop", &XWPSContextState::zsetRasterOp},
    {"1.setsourcetransparent", &XWPSContextState::zsetSourceTransparent},
    {"1.settexturetransparent", &XWPSContextState::zsetTextureTransparent},
    op_def_end(0)
};

PSOperator zimage_op_defs[] =
{
    {"5image", &XWPSContextState::zimage},
    {"5imagemask", &XWPSContextState::zimageMask},
    {"1%image_proc_continue", &XWPSContextState::imageProcContinue},
    {"0%image_file_continue", &XWPSContextState::imageFileContinue},
    {"0%image_string_continue", &XWPSContextState::imageStringContinue},
    op_def_end(0)
};

PSOperator zimage2_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"1.image1", &XWPSContextState::zimage1},
    {"1.imagemask1", &XWPSContextState::zimageMask1},
    op_def_end(0)
};

PSOperator zimage3_op_defs[] =
{
    op_def_begin_ll3(),
    {"1.image3", &XWPSContextState::zimage3},
    {"1.image4", &XWPSContextState::zimage4},
    op_def_end(0)
};

PSOperator zpath_op_defs[] =
{
    {"0clip", &XWPSContextState::zclip},
    {"0closepath", &XWPSContextState::zclosePath},
    {"0currentpoint", &XWPSContextState::zcurrentPoint},
    {"6curveto", &XWPSContextState::zcurveTo},
    {"0eoclip", &XWPSContextState::zeoclip},
    {"0initclip", &XWPSContextState::zinitClip},
    {"2lineto", &XWPSContextState::zlineTo},
    {"2moveto", &XWPSContextState::zmoveTo},
    {"0newpath", &XWPSContextState::znewPath},
    {"6rcurveto", &XWPSContextState::zrcurveTo},
    {"2rlineto", &XWPSContextState::zrlineTo},
    {"2rmoveto", &XWPSContextState::zrmoveTo},
    op_def_end(0)
};

PSOperator zpath1_op_defs[] =
{
    {"5arc", &XWPSContextState::zarc},
    {"5arcn", &XWPSContextState::zarcn},
    {"5arct", &XWPSContextState::zarct},
    {"5arcto", &XWPSContextState::zarcTo},
    {"0clippath", &XWPSContextState::zclipPath},
    {"0.dashpath", &XWPSContextState::zdashPath},
    {"0flattenpath", &XWPSContextState::zflattenPath},
    {"4pathforall", &XWPSContextState::zpathForall},
    {"0reversepath", &XWPSContextState::zreversePath},
    {"0strokepath", &XWPSContextState::zstrokePath},
    {"1.pathbbox", &XWPSContextState::zpathBbox},
    {"0%path_continue", &XWPSContextState::pathContinue},
    op_def_end(0)
};

PSOperator zpaint_op_defs[] =
{
    {"0eofill", &XWPSContextState::zeofill},
    {"0fill", &XWPSContextState::zfill},
    {"0stroke", &XWPSContextState::zstroke},
    {"0.fillpage", &XWPSContextState::zfillPage},
    {"3.imagepath", &XWPSContextState::zimagePath},
    op_def_end(0)
};

PSOperator zdps1_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"0currentstrokeadjust", &XWPSContextState::zcurrentStrokeAdjust},
    {"1setstrokeadjust", &XWPSContextState::zsetStrokeAdjust},
    {"1copy", &XWPSContextState::z1copy},
    {"1currentgstate", &XWPSContextState::zcurrentGState},
    {"0gstate", &XWPSContextState::zgstate},
    {"1setgstate", &XWPSContextState::zsetGState},
    {"1.rectappend", &XWPSContextState::zrectAppend},
    {"1rectclip", &XWPSContextState::zrectClip},
    {"1rectfill", &XWPSContextState::zrectFill},
    {"1rectstroke", &XWPSContextState::zrectStroke},
    {"4setbbox", &XWPSContextState::zsetBbox},
    op_def_end(0)
};

PSOperator zupath_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"1ineofill", &XWPSContextState::zineofill},
    {"1infill", &XWPSContextState::zinfill},
    {"1instroke", &XWPSContextState::zinstroke},
    {"2inueofill", &XWPSContextState::zinueofill},
    {"2inufill", &XWPSContextState::zinufill},
    {"2inustroke", &XWPSContextState::zinustroke},
    {"1uappend", &XWPSContextState::zuappend},
    {"0ucache", &XWPSContextState::zucache},
    {"1ueofill", &XWPSContextState::zueofill},
    {"1ufill", &XWPSContextState::zufill},
    {"1upath", &XWPSContextState::zupath},
    {"1ustroke", &XWPSContextState::zustroke},
    {"1ustrokepath", &XWPSContextState::zustrokePath},
    op_def_end(0)
};

PSOperator ccfonts_op_defs[] =
{
    {"0.getccfont", &XWPSContextState::zgetCCFont},
    op_def_end(0)
};

PSOperator zbfont_op_defs[] =
{
    {"2.buildfont3", &XWPSContextState::zbuildFont3},
    op_def_end(&XWPSContextState::zbfontInit)
};

PSOperator zfont_op_defs[] =
{
    {"0currentfont", &XWPSContextState::zcurrentFont},
    {"2makefont", &XWPSContextState::zmakeFont},
    {"2scalefont", &XWPSContextState::zscaleFont},
    {"1setfont", &XWPSContextState::zsetFont},
    {"0cachestatus", &XWPSContextState::zcacheStatus},
    {"1setcachelimit", &XWPSContextState::zsetCacheLimit},
    {"1setcacheparams", &XWPSContextState::zsetCacheParams},
    {"0currentcacheparams", &XWPSContextState::zcurrentCacheParams},
    {"1.registerfont", &XWPSContextState::zregisterFont},
    op_def_end(&XWPSContextState::zfontInit)
};

PSOperator zfont0_op_defs[] =
{
    {"2.buildfont0", &XWPSContextState::zbuildFont0},
    op_def_end(0)
};

PSOperator zfont1_op_defs[] =
{
    {"2.buildfont1", &XWPSContextState::zbuildFont1},
    {"2.buildfont4", &XWPSContextState::zbuildFont4},
    op_def_end(0)
};

PSOperator zfont2_op_defs[] =
{
    {"2.buildfont2", &XWPSContextState::zbuildFont2},
    op_def_end(0)
};

PSOperator zfont32_op_defs[] =
{
    {"2.buildfont32", &XWPSContextState::zbuildFont32},
    op_def_end(0)
};

PSOperator zfont42_op_defs[] =
{
    {"2.buildfont42", &XWPSContextState::zbuildFont42},
    op_def_end(0)
};

PSOperator zcfont_op_defs[] =
{
    {"2cshow", &XWPSContextState::zcshow},
    {"0rootfont", &XWPSContextState::zrootFont},
    {"0%cshow_continue", &XWPSContextState::cshowContinue},
    {"0%cshow_restore_font", &XWPSContextState::cshowRestoreFont},
    op_def_end(0)
};

PSOperator zfcid0_op_defs[] =
{
    {"2.buildfont9", &XWPSContextState::zbuildFont9},
    {"2.type9mapcid", &XWPSContextState::ztype9MapCid},
//#if defined(DEBUG) || defined(PROFILE)
//    {"2.writefont9", &XWPSContextState::zwriteFont9},
//#endif
    op_def_end(0)
};

PSOperator zfcid1_op_defs[] =
{
    {"2.buildfont10", &XWPSContextState::zbuildFont10},
    {"2.buildfont11", &XWPSContextState::zbuildFont11},
    {"2.type11mapcid", &XWPSContextState::ztype11MapCid},
    op_def_end(0)
};

PSOperator zfcmap_op_defs[] =
{
    {"1.buildcmap", &XWPSContextState::zbuildCMap},
//#if defined(DEBUG) || defined(PROFILE) || defined(TEST)
//    {"2.writecmap", zwritecmap},
//#endif
    op_def_end(0)
};

PSOperator zchar_op_defs[] =
{
    {"3ashow", &XWPSContextState::zashow},
    {"6awidthshow", &XWPSContextState::zawidthShow},
    {"2charpath", &XWPSContextState::zcharPath},
    {"2.charboxpath", &XWPSContextState::zcharBoxPath},
    {"2kshow", &XWPSContextState::zkshow},
    {"6setcachedevice", &XWPSContextState::zsetCacheDevice},
    {":setcachedevice2", &XWPSContextState::zsetCacheDevice2},
    {"2setcharwidth", &XWPSContextState::zsetCharWidth},
    {"1show", &XWPSContextState::zshow},
    {"1stringwidth", &XWPSContextState::zstringWidth},
    {"4widthshow", &XWPSContextState::zwidthShow},
    {"1.fontbbox", &XWPSContextState::zfontBbox},
    {"0%finish_show", &XWPSContextState::finishShow},
    {"0%finish_stringwidth", &XWPSContextState::finishStringWidth},
    {"0%op_show_continue", &XWPSContextState::opShowContinue},
    op_def_end(0)
};

PSOperator zchar1_op_defs[] =
{
    {"4.type1execchar", &XWPSContextState::ztype1ExecChar},
    {"4%bbox_getsbw_continue", &XWPSContextState::bboxGetSbwContinue},
    {"4%bbox_continue", &XWPSContextState::bboxContinue},
    {"4%bbox_finish_fill", &XWPSContextState::bboxFinishFill},
    {"4%bbox_finish_stroke", &XWPSContextState::bboxFinishStroke},
    {"4%nobbox_continue", &XWPSContextState::nobboxContinue},
    {"4%nobbox_fill", &XWPSContextState::nobboxFill},
    {"4%nobbox_stroke", &XWPSContextState::nobboxStroke},
    op_def_end(0)
};

PSOperator zchar2_op_defs[] =
{
    {"4.type2execchar", &XWPSContextState::ztype2ExecChar},
    op_def_end(0)
};

PSOperator zchar32_op_defs[] =
{
    {"1.getmetrics32", &XWPSContextState::zgetMetrics32},
    {"4.makeglyph32", &XWPSContextState::zmakeGlyph32},
    {"3.removeglyphs", &XWPSContextState::zremoveGlyphs},
    op_def_end(0)
};

PSOperator zchar42_op_defs[] =
{
    {"4.type42execchar", &XWPSContextState::ztype42ExecChar},
    op_def_end(0)
};

PSOperator zcharx_op_defs[] =
{
    op_def_begin_level2(),
    {"1glyphshow", &XWPSContextState::zglyphShow},
    {"1.glyphwidth", &XWPSContextState::zglyphWidth},
    {"2xshow", &XWPSContextState::zxshow},
    {"2xyshow", &XWPSContextState::zxyshow},
    {"2yshow", &XWPSContextState::zyshow},
    op_def_end(0)
};

PSOperator zfilter_op_defs[] = {
    {"1.psstringencode", &XWPSContextState::zPSSE},
    {"2.subfiledecode", &XWPSContextState::zSFD},
    op_def_begin_filter(),
    {"1ASCIIHexEncode", &XWPSContextState::zAXE},
    {"1ASCIIHexDecode", &XWPSContextState::zAXD},
    {"1NullEncode", &XWPSContextState::zNullE},
    {"2PFBDecode", &XWPSContextState::zPFBD},
    {"1PSStringEncode", &XWPSContextState::zPSSE},
    {"2RunLengthEncode", &XWPSContextState::zRLE},
    {"1RunLengthDecode", &XWPSContextState::zRLD},
    {"3SubFileDecode", &XWPSContextState::zSFD},
    op_def_end(0)
};

PSOperator zfilter2_op_defs[] =
{
    op_def_begin_filter(),
    {"2CCITTFaxEncode", &XWPSContextState::zCFE},
    {"1LZWEncode", &XWPSContextState::zLZWE},
    op_def_end(0)
};

PSOperator zfilterx_op_defs[] =
{
    {"2.computecodes", &XWPSContextState::zcomputeCodes},	
    op_def_begin_filter(),
    {"2BoundedHuffmanEncode", &XWPSContextState::zBHCE},
    {"2BoundedHuffmanDecode", &XWPSContextState::zBHCD},
    {"2BWBlockSortEncode", &XWPSContextState::zBWBSE},
    {"2BWBlockSortDecode", &XWPSContextState::zBWBSD},
    {"2ByteTranslateEncode", &XWPSContextState::zBTE},
    {"2ByteTranslateDecode", &XWPSContextState::zBTD},
    {"1MoveToFrontEncode", &XWPSContextState::zMTFE},
    {"1MoveToFrontDecode", &XWPSContextState::zMTFD},
    op_def_end(0)
};

PSOperator zfbcp_op_defs[] =
{
    op_def_begin_filter(),
    {"1BCPEncode", &XWPSContextState::zBCPE},
    {"1BCPDecode", &XWPSContextState::zBCPD},
    {"1TBCPEncode", &XWPSContextState::zTBCPE},
    {"1TBCPDecode", &XWPSContextState::zTBCPD},
    op_def_end(0)
};

PSOperator zfdctd_op_defs[] =
{
    op_def_begin_filter(),
    {"2DCTDecode", &XWPSContextState::zDCTD},
    op_def_end(0)
};

PSOperator zfdcte_op_defs[] =
{
    op_def_begin_filter(),
    {"2DCTEncode", &XWPSContextState::zDCTE},
    op_def_end(0)
};

PSOperator zfdecode_op_defs[] = {
    op_def_begin_filter(),
    {"1ASCII85Encode", &XWPSContextState::zA85E},
    {"1ASCII85Decode", &XWPSContextState::zA85D},
    {"2CCITTFaxDecode", &XWPSContextState::zCFD},
    {"1LZWDecode", &XWPSContextState::zLZWD},
    {"2PixelDifferenceDecode", &XWPSContextState::zPDiffD},
    {"2PixelDifferenceEncode", &XWPSContextState::zPDiffE},
    {"2PNGPredictorDecode", &XWPSContextState::zPNGPD},
    {"2PNGPredictorEncode", &XWPSContextState::zPNGPE},
    op_def_end(0)
};

PSOperator zfmd5_op_defs[] =
{
    op_def_begin_filter(),
    {"1MD5Encode", &XWPSContextState::zMD5E},
    op_def_end(0)
};

PSOperator zfzlib_op_defs[] =
{
    op_def_begin_filter(),
    {"1zlibEncode", &XWPSContextState::zzlibE},
    {"1zlibDecode", &XWPSContextState::zzlibD},
    {"1FlateEncode", &XWPSContextState::zFlateE},
    {"1FlateDecode", &XWPSContextState::zFlateD},
    op_def_end(0)
};

PSOperator zdevice_op_defs[] =
{
    {"1.copydevice2", &XWPSContextState::zcopyDevice2},
    {"0currentdevice", &XWPSContextState::zcurrentDevice},
    {"1.devicename", &XWPSContextState::zdeviceName},
    {"0.doneshowpage", &XWPSContextState::zdoneShowPage},
    {"0flushpage", &XWPSContextState::zflushPage},
    {"7.getbitsrect", &XWPSContextState::zgetBitsRect},
    {"1.getdevice", &XWPSContextState::zgetDevice},
    {"2.getdeviceparams", &XWPSContextState::zgetDeviceParams},
    {"2.gethardwareparams", &XWPSContextState::zgetHardwareParams},
    {"5makewordimagedevice", &XWPSContextState::zmakeWordImageDevice},
    {"0nulldevice", &XWPSContextState::znullDevice},
    {"2.outputpage", &XWPSContextState::zoutputPage},
    {"3.putdeviceparams", &XWPSContextState::zputDeviceParams},
    {"1.setdevice", &XWPSContextState::zsetDevice},
    op_def_end(0)
};

PSOperator zdevice2_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"0.currentshowpagecount", &XWPSContextState::zcurrentShowPageCount},
    {"0.currentpagedevice", &XWPSContextState::zcurrentPageDevice},
    {"1.setpagedevice", &XWPSContextState::zsetPageDevice},
    {"1copy", &XWPSContextState::z2copy},	
    {"0gsave", &XWPSContextState::z2gsave},	
    {"0save", &XWPSContextState::z2save},	
    {"0gstate", &XWPSContextState::z2gstate},	
    {"1currentgstate", &XWPSContextState::z2currentGState},	
    {"0grestore", &XWPSContextState::z2grestore},	
    {"0grestoreall", &XWPSContextState::z2grestoreAll},	
    {"1restore", &XWPSContextState::z2restore},	
    {"1setgstate", &XWPSContextState::z2setGState},	
    {"0.callinstall", &XWPSContextState::zcallInstall},
    {"1.callbeginpage", &XWPSContextState::zcallBeginPage},
    {"2.callendpage", &XWPSContextState::zcallEndPage},
    op_def_end(0)
};

PSOperator ziodev_op_defs[] =
{
    {"1.getiodevice", &XWPSContextState::zgetIODevice},
    op_def_end(0)
};

PSOperator ziodev2_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"1.getdevparams", &XWPSContextState::zgetDevParams},
    {"2.putdevparams", &XWPSContextState::zputDevParams},
    op_def_end(0)
};

/*
PSOperator zdosio_op_defs[] =
{
    {"1.inport", zinport},
    {"1.inportb", zinportb},
    {"2.outport", zoutport},
    {"2.outportb", zoutportb},
    {"1.peek", zpeek},
    {"2.poke", zpoke},
    op_def_end(0)
};*/


PSOperator zdscpars_op_defs[] = {
    {"1.initialize_dsc_parser", &XWPSContextState::zinitializeDSCParser},
    {"2.parse_dsc_comments", &XWPSContextState::zparseDSCComments},
    op_def_end(0)
};

PSOperator zmedia2_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"4.matchmedia", &XWPSContextState::zmatchMedia},
    {"6.matchpagesize", &XWPSContextState::zmatchPageSize},
    op_def_end(0)
};

PSOperator zmisc_op_defs[] =
{
    {"1bind", &XWPSContextState::zbind},
    {"1getenv", &XWPSContextState::zgetEnv},
    {"2.makeoperator", &XWPSContextState::zmakeOperator},
    {"0.oserrno", &XWPSContextState::zoserrno},
    {"1.oserrorstring", &XWPSContextState::zoserrorString},
    {"0realtime", &XWPSContextState::zrealTime},
    {"1serialnumber", &XWPSContextState::zserialNumber},
    {"2.setdebug", &XWPSContextState::zsetDebug},
    {"1.setoserrno", &XWPSContextState::zsetOSErrno},
    {"0usertime", &XWPSContextState::zuserTime},
    op_def_end(0)
};

PSOperator zmisc1_op_defs[] =
{
    {"3.type1encrypt", &XWPSContextState::ztype1Encrypt},
    {"3.type1decrypt", &XWPSContextState::ztype1Decrypt},
    op_def_begin_filter(),
    {"2eexecEncode", &XWPSContextState::zexE},
    {"2eexecDecode", &XWPSContextState::zexD},
    op_def_end(0)
};

PSOperator zmisc2_op_defs[] =
{
    {"0.languagelevel", &XWPSContextState::zlanguageLevel},
    {"1.setlanguagelevel", &XWPSContextState::zsetLanguageLevel},
    op_def_end(0)
};

PSOperator zmisc3_op_defs[] =
{
    op_def_begin_ll3(),
    {"0cliprestore", &XWPSContextState::zclipRestore},
    {"0clipsave", &XWPSContextState::zclipSave},
    {"2.eqproc", &XWPSContextState::zeqproc},
    {"2.setsubstitutecolorspace", &XWPSContextState::zsetSubstituteColorSpace},
    op_def_end(0)
};

PSOperator zcontext1_op_defs[] = {
    {"0condition", &XWPSContextState::zcondition},
    {"0currentcontext", &XWPSContextState::zcurrentContext},
    {"1detach", &XWPSContextState::zdetach},
    {"2.fork", &XWPSContextState::zfork},
    {"1join", &XWPSContextState::zjoin},
    {"4.localfork", &XWPSContextState::zlocalFork},
    {"0lock", &XWPSContextState::zlock},
    {"2monitor", &XWPSContextState::zmonitor},
    {"1notify", &XWPSContextState::znotify},
    {"2wait", &XWPSContextState::zwait},
    {"0yield", &XWPSContextState::zyield},
    {"0usertime", &XWPSContextState::zusertimeContext},	
    op_def_end(0)
};

PSOperator zcontext2_op_defs[] = {
    {"0%fork_done", &XWPSContextState::forkDone},
    {"1%finish_join", &XWPSContextState::finishJoin},
    {"0%monitor_cleanup", &XWPSContextState::monitorCleanup},
    {"0%monitor_release", &XWPSContextState::monitorRelease},
    {"2%await_lock", &XWPSContextState::awaitLock},
    op_def_end(&XWPSContextState::zcontextInit)
};

PSOperator zbseq_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"1.installsystemnames", &XWPSContextState::zinstallSystemNames},
    {"0currentobjectformat", &XWPSContextState::zcurrentObjectFormat},
    {"1setobjectformat", &XWPSContextState::zsetObjectFormat},
    {"4.bosobject", &XWPSContextState::zbosObject},
    op_def_end(&XWPSContextState::zbseqInit)
};

PSOperator zvmem_op_defs[] =
{
    {"1.forgetsave", &XWPSContextState::zforgetSave},
    {"1restore", &XWPSContextState::zrestore},
    {"0save", &XWPSContextState::zsave},
    {"0vmstatus", &XWPSContextState::zvmstatus},
    op_def_end(0)
};

PSOperator zvmem2_op_defs[] =
{
    {"0.currentglobal", &XWPSContextState::zcurrentGlobal},
    {"1.gcheck", &XWPSContextState::zgcheck},
    {"1.setglobal", &XWPSContextState::zsetGlobal},
    op_def_begin_level2(),
    {"1.vmreclaim", &XWPSContextState::zvmreclaim},
    op_def_end(0)
};

PSOperator zsysvm_op_defs[] =
{
    {"1.globalvmarray", &XWPSContextState::zglobalVMArray},
    {"1.globalvmdict", &XWPSContextState::zglobalVMDict},
    {"1.globalvmpackedarray", &XWPSContextState::zglobalVMPackedArray},
    {"1.globalvmstring", &XWPSContextState::zglobalVMString},
    {"1.localvmarray", &XWPSContextState::zlocalVMArray},
    {"1.localvmdict", &XWPSContextState::zlocalVMDict},
    {"1.localvmpackedarray", &XWPSContextState::zlocalVMPackedArray},
    {"1.localvmstring", &XWPSContextState::zlocalVMString},
    {"1.systemvmarray", &XWPSContextState::zsystemVMArray},
    {"1.systemvmcheck", &XWPSContextState::zsystemVMCheck},
    {"1.systemvmdict", &XWPSContextState::zsystemVMDict},
    {"1.systemvmpackedarray", &XWPSContextState::zsystemVMPackedArray},
    {"1.systemvmstring", &XWPSContextState::zsystemVMString},
    op_def_end(0)
};

PSOperator ztrap_op_defs[] =
{
    op_def_begin_ll3(),
    {"1.settrapparams", &XWPSContextState::zsetTrapParams},
    {"0settrapzone", &XWPSContextState::zsetTrapZone},
    op_def_end(0)
};

PSOperator zusparam_op_defs[] =
{
    {"0.currentsystemparams", &XWPSContextState::zcurrentSystemParams},
    {"0.currentuserparams", &XWPSContextState::zcurrentUserParams},
    {"1.getsystemparam", &XWPSContextState::zgetSystemParam},
    {"1.getuserparam", &XWPSContextState::zgetUserParam},
    {"1.setsystemparams", &XWPSContextState::zsetSystemParams},
    {"1.setuserparams", &XWPSContextState::zsetUserParams},
    op_def_begin_level2(),
    {"1.checkpassword", &XWPSContextState::zcheckPassword},
    op_def_end(0)
};

PSOperator * XWPSContextState::op_defs_all[] = {
	interp_op_defs,
	zstack_op_defs,
	zarith_op_defs,
	zdouble1_op_defs,
	zdouble2_op_defs,
	zmath_op_defs,
	zmatrix_op_defs,
	zarray_op_defs,
	zpacked_op_defs,
	zgeneric_op_defs,
	zdict1_op_defs,
	zdict2_op_defs,
	zstring_op_defs,
	ztoken_op_defs,
	zrelbit_op_defs,
	zcontrol1_op_defs,
	zcontrol2_op_defs,
	zcontrol3_op_defs,
	ztype_op_defs,
	zfile_op_defs,
	zfileio1_op_defs,
	zfileio2_op_defs,
	zfproc_op_defs,
	zfrsd_op_defs,
	zgstate1_op_defs,
	zgstate2_op_defs,
	zcolor_op_defs,
	zcolor1_op_defs,
	zcolor2_l2_op_defs,
	zcrd_l2_op_defs,
	zcie_l2_op_defs,
	zcsdevn_op_defs,
	zcsindex_l2_op_defs,
	zcspixel_op_defs,
	zcssepr_l2_op_defs,
	zdpnext_op_defs,
	zpcolor_l2_op_defs,
	zhsb_op_defs,
	zht_op_defs,
	zht1_op_defs,
	zht2_l2_op_defs,
	zdps_op_defs,
	zfunc_op_defs,
	zshade_op_defs,
	ztrans_op_defs,
	zrop_op_defs,
	zimage_op_defs,
	zimage2_l2_op_defs,
	zimage3_op_defs,
	zpath_op_defs,
	zpath1_op_defs,
	zpaint_op_defs,
	zdps1_l2_op_defs,
	zupath_l2_op_defs,
	ccfonts_op_defs,
	zbfont_op_defs,
	zfont_op_defs,
	zfont0_op_defs,
	zfont1_op_defs,
	zfont2_op_defs,
	zfont32_op_defs,
	zfont42_op_defs,
	zcfont_op_defs,
	zfcid0_op_defs,
	zfcid1_op_defs,
	zfcmap_op_defs,
	zchar_op_defs,
	zchar1_op_defs,
	zchar2_op_defs,
	zchar32_op_defs,
	zchar42_op_defs,
	zcharx_op_defs,
	zfilter_op_defs,
	zfilter2_op_defs,
	zfilterx_op_defs,
	zfbcp_op_defs,
	zfdctd_op_defs,
	zfdcte_op_defs,
	zfdecode_op_defs,
	zfmd5_op_defs,
	zfzlib_op_defs,
	zdevice_op_defs,
	zdevice2_l2_op_defs,
	ziodev_op_defs,
	ziodev2_l2_op_defs,
	zdscpars_op_defs,
	zmedia2_l2_op_defs,
	zmisc_op_defs,
	zmisc1_op_defs,
	zmisc2_op_defs,
	zmisc3_op_defs,
	zcontext1_op_defs,
	zcontext2_op_defs,
	zbseq_l2_op_defs,
	zvmem_op_defs,
	zvmem2_op_defs,
	zsysvm_op_defs,
	ztrap_op_defs,
	zusparam_op_defs,
	0
};

PSBuildFunctionType XWPSContextState::build_function_type_table[4] = {
	{0, &XWPSContextState::buildFunction0},
	{2, &XWPSContextState::buildFunction2},
	{3, &XWPSContextState::buildFunction3},
	{4, &XWPSContextState::buildFunction4}
};

const ccfont_fproc XWPSContextState::fprocs[] = 
{
	0
};

static void esc_strcat(char *dest, const char *src, int len)
{
    char *d = dest + strlen(dest);
    const char *p;
    const char * endptr = src + len;
    static const char *const hex = "0123456789abcdef";

    *d++ = '<';
    for (p = src; p < endptr; p++) {
	uchar c = (uchar) * p;

	*d++ = hex[c >> 4];
	*d++ = hex[c & 0xf];
    }
    *d++ = '>';
    *d = 0;
}

uint XWPSContextState::op_def_count = ((sizeof(op_defs_all) / sizeof(op_defs_all[0])) - 1) * OP_DEFS_MAX_SIZE;

XWPSContextState::XWPSContextState()
{
	dev = 0;
	memory = 0;
	
	pgs = 0;
	ps_next_id = 1;
	saved_next_id = 1;
	language_level = 1;
	array_packing.makeFalse();
	binary_object_format.makeInt(0);
	rand_state = 1;
	rand_state_initial = 1;
	usertime_total = 0;
	keep_usertime = false;
	in_superexec = 0;
	scanner_options = 0;
	name_max_string = max_name_string;
	dict_auto_expand = false;
	system_names_p = 0;
	user_names_p = 0;	
	device_count = 1;
	ps_device_list = 0;
	ifont_dir = 0;
	
	the_name_table = new XWPSNameTable(65536);
	
	io_device_table_count = 4;
	io_device_table = (XWPSIODevice**)malloc(io_device_table_count*sizeof(XWPSIODevice*));
	io_device_table[0] = new XWPSOs;
	io_device_table[0]->state = this;
	io_device_table[1] = new XWPSStdin;
	io_device_table[1]->state = this;
	io_device_table[2] = new XWPSStdout;
	io_device_table[2]->state = this;
	io_device_table[3] = new XWPSStderr;
	io_device_table[3]->state = this;
	
	invalid_file_stream.incRef();
	invalid_file_stream.incRef();
	invalid_file_stream.incRef();
	stdio[0].makeFile(PS_A_READONLY | PS_I_VM_FOREIGN, 1, &invalid_file_stream);
	stdio[1].makeFile(PS_A_ALL | PS_I_VM_FOREIGN, 1, &invalid_file_stream);
	stdio[2].makeFile(PS_A_ALL | PS_I_VM_FOREIGN, 1, &invalid_file_stream);
}

XWPSContextState::~XWPSContextState()
{	
	if (pgs)
	{		
		pgs->restoreAll();
		XWPSState *saved = pgs->getSaved();
		saved->swapSaved(saved);
		pgs->restore();
		pgs->swapSaved(0);
		delete pgs;
		pgs = 0;
	}
	
	QHash<ulong, XWPSState*>::iterator i = saved_states.begin();
	while (i != saved_states.end())
	{
		XWPSState * pg = i.value();
		i = saved_states.erase(i);
		if (pg)
			delete pg;
	}
	
	if (the_name_table)
	{
		delete the_name_table;
		the_name_table = 0;
	}
	
	if (system_names_p)
	{
		delete system_names_p;
		system_names_p = 0;
	}
	
	if (user_names_p)
	{
		delete user_names_p;
		user_names_p = 0;
	}
	
	if (ifont_dir)
	{
		delete ifont_dir;
		ifont_dir = 0;
	}
	
	while (!device_instances.isEmpty())
	{
		XWPSDevice * pdev = device_instances.takeFirst();
		if (pdev)
			delete pdev;
	}
	
	while (!device_list.isEmpty())
	{
		XWPSDevice * pdev = device_list.takeFirst();
		if (pdev)
			delete pdev;
	}
	
	if (io_device_table)
	{
		for (int i = 0; i < io_device_table_count; i++)
		{
			if (io_device_table[i])
			{
				delete io_device_table[i];
				io_device_table[i] = 0;
			}
		}
		
		free(io_device_table);
		io_device_table = 0;
	}
			
	if (memory)
	{
		delete memory;
		memory = 0;
	}
}

#define ERROR_IS_INTERRUPT(ecode)\
  ((ecode) == XWPSError::Interrupt || (ecode) == XWPSError::Timeout)
  
int XWPSContextState::callInterpret(XWPSRef * pref)
{
	XWPSRef doref, saref, errorobj, error_name;
	XWPSRef * epref = pref;
	XWPSRef * p;
	XWPSRef * perrordict;
	int code, ccode;
	 
	idmemory()->resetRequested();
	
again:
	code = interpret(epref, &errorobj);
	if (exec_stack.getCurrentTop() < exec_stack.getBottom())
		exec_stack.setCurrentTop(exec_stack.getBottom());
		
	switch (code)
	{
		case XWPSError::Fatal:
	    return code;
	    
	  case XWPSError::Quit:
	    op_stack.incCurrentTop(-2);
	    return	(code == 0 ? XWPSError::Quit : code < 0 && code > -100 ? code : XWPSError::Fatal);
	    	
	  case XWPSError::InterpreterExit:
	    return 0;
	    
	  case XWPSError::ExecStackUnderflow:
	    exec_stack.popBlock();
	    goto again;
	    break;
	    
	  case XWPSError::VMReclaim:
	    doref.makeOper(0, &XWPSContextState::zpop);
	    epref = &doref;
	    goto again;
	    
		case XWPSError::NeedInput:
	    return code;
	}
	
	if (op_stack.getCurrentTop() < (op_stack.getBottom() - 1))
		op_stack.setCurrentTop(op_stack.getBottom() - 1);
		
	switch (code)
	{
		case XWPSError::DictStackOverflow:
	    if (dict_stack.extend(dict_stack.getStack()->requested) >= 0) 
	    {
				dict_stack.setTop();
				doref.assign(&errorobj);
				epref = &doref;
				goto again;
	    }
	    
	    if (op_stack.getCurrentTop() >= op_stack.getTop()) 
	    {
				if ((ccode = op_stack.extend(1)) < 0)
				{
		    	return ccode;
		    }
	    }
	    ccode = copyStack(dict_stack.getStack(), &saref);
	    if (ccode < 0)
	    {
				return ccode;
			}
	    dict_stack.pop(dict_stack.count() - dict_stack.getMinSize());
	    dict_stack.setTop();
	    p = op_stack.incCurrentTop(1);
	    p->assign(&saref);
	    break;
	    
	  case XWPSError::DictStackUnderflow:
	    if (dict_stack.popBlock() >= 0) 
	    {
				dict_stack.setTop();
				doref.assign(&errorobj);
				epref = &doref;
				goto again;
	    }
	    break;
	    
	  case XWPSError::ExecstackOverFlow:
	    if (op_stack.getCurrentTop() >= op_stack.getTop()) 
	    {
				if ((ccode = op_stack.extend(1)) < 0)
				{
		    	return ccode;
		    }
	    }
	    ccode = copyStack(exec_stack.getStack(), &saref);
	    if (ccode < 0)
	    {
				return ccode;
			}
	    {
				uint count = exec_stack.count();
				uint limit = exec_stack.maxCount() - 20;

				if (count > limit) 
				{
		    	int skip = count - limit;
		    	int i;

		    	for (i = skip; i < skip + MIN_BLOCK_ESTACK; ++i) 
		    	{
						XWPSRef *ep = exec_stack.index(i);

						if (ep->hasTypeAttrs(XWPSRef::Null, PS_A_EXECUTABLE)) 
						{
			    		skip = i + 1;
			    		break;
						}
		    	}
		    	popEStack(skip);
				}
	    }
	    p = op_stack.incCurrentTop(1);
	    p->assign(&saref);
	    break;
	    
	  case XWPSError::StackOverflow:
	    if (op_stack.extend(op_stack.getStack()->requested) >= 0) 
	    {	
				doref.assign(&errorobj);
				if (doref.isProc()) 
				{
					p = op_stack.incCurrentTop(1);
	    		p->assign(&doref);
		    	doref.makeArray(PS_A_EXECUTABLE + PS_A_READONLY);
				}
				epref = &doref;
				goto again;
	    }
	    ccode = copyStack(op_stack.getStack(), &saref);
	    if (ccode < 0)
	    {
				return ccode;
			}
	    op_stack.pop(0);
	    p = op_stack.incCurrentTop(1);
	    p->assign(&saref);
	    break;
	    
		case XWPSError::StackUnderflow:
	    if (op_stack.popBlock() >= 0) 
	    {
				doref.assign(&errorobj);
				epref = &doref;
				goto again;
	    }
	    break;
	    
	  default:
	  	break;
	}
	
	if (errorName(code, &error_name) < 0)
	{
		return code;
	}
		
  if (dict_stack.getSystemDict()->dictFindString(this, "errordict", &perrordict) <= 0 ||
			perrordict->dictFind(this, &error_name, &epref) <= 0)
	{
		return code;
	}
		
  doref.assign(epref);
  epref = &doref;
  
  if (!ERROR_IS_INTERRUPT(code))
  {
  	p = op_stack.incCurrentTop(1);
	  p->assign(&errorobj);
	}
  goto again;
}

int XWPSContextState::checkEStack(int n)
{
	if ( exec_stack.getCurrentTop() > (exec_stack.getTop() - n))
	{
		int es_code_ = exec_stack.extend(n);
		if (es_code_ < 0)
			return es_code_;
	}
	
	return 0;
}

int XWPSContextState::checkOp(XWPSRef * op, int nargs)
{
	if (op < (op_stack.getBottom() + (nargs - 1)))
	{
		return (int)(XWPSError::StackUnderflow);
	}
	
	return 0;
}

int XWPSContextState::checkOStack(int n)
{
	if ((op_stack.getTop() - op_stack.getCurrentTop()) < n)
	{
		op_stack.setRequested(n);
		return (int)(XWPSError::StackOverflow);
	}
		
	return 0;
}

void XWPSContextState::close()
{
	for (int i = 0; i < device_instances.size(); i++)
	{
		XWPSDevice * pdev = device_instances[i];
		if (pdev)
			pdev->close();
	}
}

int XWPSContextState::dictCopy(XWPSRef * from, XWPSRef * to)
{
	return to->dictCopy(this, from, &dict_stack);
}

int XWPSContextState::dictCopyNew(XWPSRef * from, XWPSRef * to)
{
	return to->dictCopyNew(this, from, &dict_stack);
}

XWPSRef * XWPSContextState::dictFindName(XWPSRef * pnref)
{
	return dict_stack.findNameByIndex(this, nameIndex(pnref));
}

int XWPSContextState::dictGrow(XWPSRef * dict)
{
	return dict->dictGrow(this, &dict_stack);
}

int XWPSContextState::dictPut(XWPSRef * dict, XWPSRef * pkey, XWPSRef * pvalue)
{
	return dict->dictPut(this, pkey, pvalue, &dict_stack);
}

int XWPSContextState::dictPutString(XWPSRef * dict, const char *kstr, XWPSRef * pvalue)
{
	return dict->dictPutString(this, kstr, pvalue, &dict_stack);
}

int XWPSContextState::dictResize(XWPSRef * dict, uint new_size)
{
	return dict->dictResize(this, new_size, &dict_stack);
}

int XWPSContextState::dictUndef(XWPSRef * dict, XWPSRef * pkey)
{
	return dict->dictUndef(this, pkey, &dict_stack);
}

int XWPSContextState::dictUnpack(XWPSRef * dict)
{
	return dict->dictUnpack(this, &dict_stack);
}

int XWPSContextState::estackUnderflow()
{
    return (int)(XWPSError::ExecStackUnderflow);
}

int XWPSContextState::errorName(int code, XWPSRef * perror_name)
{
	XWPSRef *perrordict, *pErrorNames;

  if (dict_stack.getSystemDict()->dictFindString(this, "errordict", &perrordict) <= 0 ||
			dict_stack.getSystemDict()->dictFindString(this, "ErrorNames", &pErrorNames) <= 0)
	{
		return (int)(XWPSError::Undefined);
	}
	
  return pErrorNames->arrayGet(this, (long)(-code - 1), perror_name);
}

XWPSIODevice * XWPSContextState::findIODevice(const uchar * str, uint len)
{
	int i;

  if (len > 1 && str[len - 1] == '%')
		len--;
  for (i = 0; i < io_device_table_count; ++i) 
  {
		XWPSIODevice *iodev = io_device_table[i];
		const char *dname = iodev->dname;

		if (dname && strlen(dname) == len + 1 && !memcmp(str, dname, len))
	    return iodev;
  }
  return 0;
}

int XWPSContextState::floatParams(XWPSRef * op, int count, float *pval)
{
	for (pval += count; --count >= 0; --op)
		switch (op->type()) 
		{
	    case XWPSRef::Real:
				*--pval = op->value.realval;
				break;
				
	    case XWPSRef::Integer:
				*--pval = op->value.intval;
				break;
				
	    case XWPSRef::Invalid:
				return (int)(XWPSError::StackUnderflow);
				
	    default:
				return (int)(XWPSError::TypeCheck);
		}
  return 0;
}

XWPSDevice * XWPSContextState::getDevice(int i)
{
	if (i < 0 || i >= device_list.size())
		return 0;
	return device_list[i];
}

XWPSIODevice * XWPSContextState::getIODevice(int index)
{
	if (index < 0 || index >= (io_device_table_count - 1))
		return 0;		
  return io_device_table[index];
}

bool XWPSContextState::haveLevel2()
{
	PSOperator ** tptr = op_defs_all;
  for (; *tptr != 0; ++tptr) 
  {
		PSOperator *def = *tptr;
		for (; def->oname != 0; ++def)
		{
	    if (op_def_is_begin_dict(def) && !strcmp(def->oname, "level2dict"))
				return true;
		}
  }
  return false;
}

int XWPSContextState::nameEnterString(const char *str, XWPSRef * pref)
{
	return the_name_table->namesEnterString(str, pref);
}

bool XWPSContextState::nameEq(XWPSRef * pnref1, XWPSRef * pnref2)
{
	XWPSName * n1 = pnref1->getName();
	XWPSName * n2 = pnref2->getName();
	return n1 == n2;
}

int XWPSContextState::nameFromString(XWPSRef * psref, XWPSRef * pnref)
{
	return the_name_table->namesFromString(psref, pnref);
}

uint XWPSContextState::nameIndex(XWPSRef * pnref)
{
	return the_name_table->namesIndex(pnref);
}

XWPSName * XWPSContextState::nameIndexPtr(uint nidx)
{
	return the_name_table->namesIndexPtr(nidx);
}

void XWPSContextState::nameIndexRef(uint nidx, XWPSRef * pnref)
{
	the_name_table->namesIndexRef(nidx, pnref);
}

void XWPSContextState::nameInvalidateValueCache(XWPSRef * pnref)
{
	the_name_table->namesInvalidateValueCache(pnref);
}

bool XWPSContextState::nameMarkIndex(uint nidx)
{
	return the_name_table->namesMarkIndex(nidx);
}

uint XWPSContextState::nameNextValidIndex(uint nidx)
{
	return the_name_table->namesNextValidIndex(nidx);
}

int XWPSContextState::nameRef(const uchar *ptr, uint size, XWPSRef *pref, int enterflag)
{
	return the_name_table->namesRef(ptr, size, pref, enterflag);
}

XWPSName * XWPSContextState::nameRefSubTable(XWPSRef * pnref)
{
	return the_name_table->namesRefSubTable(pnref);
}

void XWPSContextState::nameStringRef(XWPSRef * pnref, XWPSRef * psref)
{
	the_name_table->namesStringRef(pnref, psref);
}

ulong XWPSContextState::nextIDS(uint count)
{
	ulong id = ps_next_id;

  ps_next_id += count;
  return id;
}

int XWPSContextState::numParams(XWPSRef * op, int count, double *pval)
{
	int mask = 0;

  pval += count;
  while (--count >= 0) 
  {
		mask <<= 1;
		switch (op->type()) 
		{
	    case XWPSRef::Real:
				*--pval = op->value.realval;
				break;
				
	    case XWPSRef::Integer:
				*--pval = op->value.intval;
				mask++;
				break;
				
	    case XWPSRef::Invalid:
				return (int)(XWPSError::StackUnderflow);
				
	    default:
				return (int)(XWPSError::TypeCheck);
		}
		op--;
  }
  
  return (mask < 0 ? 0 : mask);
}

ushort XWPSContextState::opFindIndex(XWPSRef * ref)
{
	op_proc_t proc = ref->getOper();
	
	PSOperator ** opp = op_defs_all;
	PSOperator ** opend = opp + (op_def_count / OP_DEFS_MAX_SIZE);
	for (; opp < opend; ++opp)
	{
		PSOperator *def = *opp;
		for (; def->oname != 0; ++def)
	    if (def->proc == proc)
		return (opp - op_defs_all) * OP_DEFS_MAX_SIZE + (def - *opp);
	}
	
	return 0;
}

ushort XWPSContextState::opIndex(XWPSRef * ref)
{
	if (ref->size() == 0)
		return opFindIndex(ref);
		
	return ref->size();
}

PSOperator * XWPSContextState::opIndexDef(uint index)
{
	return (&op_defs_all[(index) >> OP_DEFS_LOG2_MAX_SIZE][(index) & (OP_DEFS_MAX_SIZE - 1)]);
}

bool XWPSContextState::opIndexIsOperator(uint index)
{
	return (index < op_def_count);
}

XWPSOpArrayTable * XWPSContextState::opIndexOpArrayTable(uint index)
{
	return (index < op_array_table_local.base_index ? &op_array_table_global : &op_array_table_local);
}

op_proc_t XWPSContextState::opIndexProc(uint index)
{
	return (opIndexDef(index)->proc);
}

void XWPSContextState::opIndexRef(uint index, XWPSRef *pref)
{
	if (opIndexIsOperator(index)) 
	{
		pref->makeOper(index, opIndexProc(index));
		return;
  }
  
  XWPSOpArrayTable * opt = opIndexOpArrayTable(index);
  pref->makeOpArray((ushort)opt->attrs, index, (opt->table.getArray() + index - opt->base_index));
}

uint XWPSContextState::opNumArgs(XWPSRef * ref)
{
	return (opIndexDef(opIndex(ref))->oname[0] - '0');
}

void XWPSContextState::packedGet(ushort * packed, XWPSRef * pref)
{
	ushort elt = *packed;
  uint value = elt & packed_value_mask;
  switch (elt >> PS_R_PACKED_TYPE_SHIFT)
  {
  	default:
  		pref->makeNull();
  		break;
  		
  	case pt_executable_operator:
	    opIndexRef(value, pref);
	    break;
	    
	  case pt_integer:
	    pref->makeInt((int)value + packed_min_intval);
	    break;
	    
	  case pt_literal_name:
	    nameIndexRef(value, pref);
	    break;
	    
	  case pt_executable_name:
	    nameIndexRef(value, pref);
	    pref->setAttrs(PS_A_EXECUTABLE);
	    break;
	    
	  case pt_full_ref:
		case pt_full_ref + 1:
	    pref->assign((XWPSRef *)packed);
	    break;
  }
}

void XWPSContextState::pop(int n)
{
	op_stack.incCurrentTop(-n);
}

void XWPSContextState::popEStack(uint count)
{
	uint idx = 0;
  uint popped = 0;

  exec_stack.clearCache();
  for (; idx < count; idx++) 
  {
		XWPSRef *ep = exec_stack.index(idx - popped);

		if (ep->isEStackMark()) 
		{
	    exec_stack.pop(idx + 1 - popped);
	    popped = idx + 1;
	    ep->callProc(this);
		}
  }
  exec_stack.pop(count - popped);
}

int XWPSContextState::push(XWPSRef ** op, int n)
{
	XWPSRef *p = *op;
	p += n;
	*op = p;
	if (p > op_stack.getTop())
	{
		op_stack.setRequested(n);
		return (int)(XWPSError::StackOverflow);
	}
	
	op_stack.setCurrentTop(p);
	return 0;
}

XWPSRef * XWPSContextState::registeredEncoding(int i)
{
	return registered_Encodings.getArray() + i;
}

void XWPSContextState::reset()
{
	op_stack.pop(op_stack.count());
	exec_stack.pop(exec_stack.count());
	XWPSRef * p = exec_stack.incCurrentTop(1);
	p->makeOper(0, &XWPSContextState::interpExit);
	dict_stack.pop(dict_stack.count() - dict_stack.getMinSize());
	dict_stack.setTop();
}

void XWPSContextState::resetNextID()
{
	ps_next_id = saved_next_id;
}

void XWPSContextState::resetPageNextID(ulong pgno)
{
	ps_next_id = pagenextids[pgno];
}

int XWPSContextState::runStringBegin()
{
	const char *setup = ".runstringbegin";
  XWPSRef rstr;
  int code;
  
  rstr.makeStringSta(PS_AVM_FOREIGN | PS_A_READONLY | PS_A_EXECUTABLE, strlen(setup), (uchar *)setup);
  code = callInterpret(&rstr);
  return (code == XWPSError::NeedInput ? 0 : code == 0 ? XWPSError::Fatal : code);
}

int XWPSContextState::runStringContinue(const char *str, uint length)
{
	if (length == 0)
		return 0;
		
	XWPSRef rstr;
	rstr.makeStringSta(PS_AVM_FOREIGN | PS_A_READONLY, length, (uchar *)str);
	return callInterpret(&rstr);
}

int XWPSContextState::runStringEnd()
{
	XWPSRef rstr;

  rstr.makeString(PS_AVM_FOREIGN | PS_A_READONLY);
  return callInterpret(&rstr);
}

int XWPSContextState::runStart()
{
	const char *str = "systemdict /start get exec";
	int code = runString(str, strlen(str));
	if (code != 0)
		zflushPage();
		
	return code;
}

int XWPSContextState::runString(const char *str, uint length)
{
	int code = runStringBegin();
	if (code < 0)
		return code;
		
	code = runStringContinue(str, length);
	if (code != XWPSError::NeedInput)
		return code;
  code = runStringEnd();
  return code;
}

void XWPSContextState::saveNextID()
{
	saved_next_id = ps_next_id;
}

void XWPSContextState::savePageNextID(ulong pgno)
{
	pagenextids[pgno] = ps_next_id;
}

XWPSRef * XWPSContextState::StandardEncoding()
{
	return registeredEncoding(0);
}

int XWPSContextState::stringToRef(const char *cstr, XWPSRef * pref)
{
	uint s = strlen(cstr);
	pref->makeString(PS_A_ALL, s);
	uchar * d = pref->getBytes();
	memcpy(d, cstr, s);
  return 0;
}

int XWPSContextState::toPDF(XWDVICore * coreA,
	             XWDVIRef * xrefA,
	             XWDVIDev * devA,
	             const uchar * buf, 
	             uint len)
{
	XWPSDeviceDVI * pdev = 0;
	if (!dev)
	{	
		double pw = coreA->getPageWidth();
		double ph = coreA->getPageHeight();
		int w = qRound(((pw / 10) / 2.54) * 10);
		int h = qRound(((ph / 10) / 2.54) * 10);		
		pdev = new XWPSDeviceDVI(w, h);
		dev = pdev;
		device_list << pdev;

		pdev->context_state = this;
		pdev->initDVI(coreA, xrefA, devA);
		
		int code = init2();
		if (code < 0)
			return code;
						
		init();
	}
	
	{
		if (device_instances.size() > 0)
		{
			pdev = (XWPSDeviceDVI*)(device_instances[0]);
			pdev->initDVI(coreA, xrefA, devA);
		}
	}	
	char * nbuf = new char[3 * len + 11];
	nbuf[0] = '\0';
	esc_strcat(nbuf, (const char*)buf, len);
	strcat(nbuf, ".runstring");
	int code = runString(nbuf, strlen(nbuf));
	delete [] nbuf;
	if (code != 0)
		zflushPage();		
	
	return code;
}

QString XWPSContextState::toPDF(const QString & filename, 
	              double pw, 
	              double ph)
{
	XWPictureSea psea;
	QString afn = psea.findFile(filename);
	if (afn.isEmpty())
	{
		XWDocSea dsea;
	  afn = dsea.findFile(filename);
	}
	
	if (afn.isEmpty())
	{
		XWGhostscriptSea gssea;
		afn = gssea.findFile(filename);
	}
	
	if (afn.isEmpty())
	{
		XWDviPsSea dvipssea;
		afn = dvipssea.findFile(filename);
	}
	
	if (afn.isEmpty())
		return afn;
	
	QString outname = xwApp->getTmpFile();
	outname += ".pdf";
	QFile * out = new QFile(outname);
	if (!out->open(QIODevice::WriteOnly))
	{
		delete out;
		return 0;
	}
	
	XWPSDevicePDF * pdev = new XWPSDevicePDF;
	pdev->context_state = this;
	pdev->file = out;
	dev = pdev;
	device_list << pdev;
	
	pdev->context_state = this;
	
	int code = init2();
	if (code < 0)
		return outname;
	
	init();
	
	QByteArray ba = filename.toLocal8Bit();
	char * nbuf = new char[3 * ba.size() + 9];
	nbuf[0] = '\0';
	esc_strcat(nbuf, ba.data(), ba.size());
	strcat(nbuf, ".runfile");
	code = runString(nbuf, strlen(nbuf));
	delete [] nbuf;
	close();
	out->close();
	delete out;
	return outname;
}

int XWPSContextState::copyStack(XWPSRefStack * pstack, XWPSRef * arr)
{
	uint size = pstack->count();
	uint save_space = idmemory()->iallocSpace();
	idmemory()->setSpace(PS_AVM_LOCAL);
	arr->makeArray(currentSpace() | PS_A_ALL, size);
	pstack->store(arr, size, 0, 1, true);
	idmemory()->setSpace(save_space);
	return 0;
}

int XWPSContextState::dynamicMakeString(XWPSRef * pref, XWPSDynamicArea * pda, uchar * next)
{
	uint size = (pda->next = next) - pda->base;
  int code = pda->resize(size);
  if (code < 0)
		return code;
  
  if (pda->dynamic_buf)
		pda->dynamic_buf->incRef();
	pref->makeString(PS_AVM_SYSTEM | PS_A_ALL);
	pref->setSize(size);
	pref->value.bytes->arr = pda->dynamic_buf;
	pref->value.bytes->ptr = pda->base;
  return 0;
}

int XWPSContextState::encodeBinaryToken(XWPSRef *obj, 
	                                      long *ref_offset,
		                                    long *char_offset, 
		                                    uchar *str)
{
	int t;
  uint size = 0;
  long value;
  XWPSRef nstr;

  switch (obj->type()) 
  {
		case XWPSRef::Null:
	    t = BS_TYPE_NULL;
	    goto tx;
	    
		case XWPSRef::Mark:
	    t = BS_TYPE_MARK;
	    goto tx;
	    
		case XWPSRef::Integer:
	    t = BS_TYPE_INTEGER;
	    value = obj->getInt();
	    break;
	    
		case XWPSRef::Real:
	    t = BS_TYPE_REAL;
	    if (sizeof(obj->value.realval) == sizeof(int))
	    	value = qRound(obj->getReal());
	    else
	    	return (int)(XWPSError::RangeCheck);
	    break;
	    
		case XWPSRef::Boolean:
	    t = BS_TYPE_BOOLEAN;
	    value = obj->getBool();
	    break;
	    
		case XWPSRef::Array:
	    t = BS_TYPE_ARRAY;
	    size = obj->size();
	    goto aod;
	    
		case XWPSRef::Dictionary:
	    t = BS_TYPE_DICTIONARY;
	    size = obj->dictLength() << 1;
	    
aod:
			value = *ref_offset;
	    *ref_offset += size * (ulong) SIZEOF_BIN_SEQ_OBJ;
	    break;
	    
		case XWPSRef::String:
	    t = BS_TYPE_STRING;
nos:
	    size = obj->size();
	    value = *char_offset;
	    *char_offset += size;
	    break;
	    
		case XWPSRef::Name:
	    t = BS_TYPE_NAME;
	    nameStringRef(obj, &nstr);
	    nstr.copyAttrs(PS_A_EXECUTABLE, obj);
	    obj = &nstr;
	    goto nos;
	    
		default:
	    return (int)(XWPSError::RangeCheck);
  }
  
  {
		uchar s0 = (uchar) size, s1 = (uchar) (size >> 8);
		uchar v0 = (uchar) value;
		uchar v1 = (uchar) (value >> 8);
		uchar v2 = (uchar) (value >> 16);
	  uchar v3 = (uchar) (value >> 24);
		int order = (int)binary_object_format.getInt() - 1;

		if (order & 1) 
		{
	    str[2] = s0, str[3] = s1;
	    str[4] = v0, str[5] = v1, str[6] = v2, str[7] = v3;
		} 
		else 
		{
	    str[2] = s1, str[3] = s0;
	    str[4] = v3, str[5] = v2, str[6] = v1, str[7] = v0;
		}
  }
  
tx:
  if (obj->hasAttr(PS_A_EXECUTABLE))
		t += BS_EXECUTABLE;
    
  str[0] = (uchar) t;
  return 0;
}

int XWPSContextState::errorInfoPutString(const char *str)
{
	XWPSRef rstr;
  XWPSRef *pderror;
  int code = stringToRef(str, &rstr);

  if (code < 0)
		return code;
    
  if (dict_stack.getSystemDict()->dictFindString(this, "$error", &pderror) <= 0 ||
			!pderror->hasType(XWPSRef::Dictionary) ||
			dictPutString(pderror, "errorinfo", &rstr) < 0)
	{
		return (int)(XWPSError::Fatal);
	}
  return 0;
}

void XWPSContextState::init()
{	
	char setpdf[15];
	strncpy(setpdf, ".setpdfwrite", 12);
	setpdf[12] = '\0';
	char str[40];
	str[0] = '\0';
	esc_strcat(str, setpdf, 12);
	strcat(str, ".runstring");
	runString(str, strlen(str));
	saved_next_id = ps_next_id;
}

int  XWPSContextState::init2()
{
	if (!memory)
		memory = new XWPSDualMemory(20000, haveLevel2());
	int code = objInit();
	if (code < 0)
		return code;
		
	XWPSRef quiet,safe,nopause,batch;
	quiet.makeTrue();
	initialEnterName("QUIET", &quiet);
	safe.makeTrue();
	initialEnterName("SAFER", &safe);
	nopause.makeTrue();
	initialEnterName("NOPAUSE", &nopause);
	batch.makeTrue();
	initialEnterName("BATCH", &batch);
	
	XWPSRef CompatibilityLevel;
	CompatibilityLevel.makeReal(1.4);
	initialEnterName("CompatibilityLevel", &CompatibilityLevel);
	
	const char *str = "pdfwrite";
	XWPSRef pdfwrite;
	pdfwrite.makeString(PS_A_READONLY | PS_AVM_FOREIGN, 8, (uchar*)str);
	initialEnterName("DEVICE", &pdfwrite);
	
	XWPSRef ouputfile;
	ouputfile.makeString(PS_A_READONLY | PS_AVM_FOREIGN, 10, (uchar*)"pdffile");
	initialEnterName("OutputFile", &ouputfile);
		
	code = zopInit();
	if (code < 0)
		return code;
		
	code = opInit();
	if (code < 0)
		return code;
	
  XWPSRef ifa;    
  ifa.makeArray(PS_A_READONLY | PS_AVM_FOREIGN, 35);
  initialEnterName("INITFILES", &ifa);
  
	XWPSRef * init_file_array = ifa.getArray();	
	init_file_array[0].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_type1.ps");	
	init_file_array[1].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 10, (uchar*)"gs_dps1.ps");	
	init_file_array[2].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 10, (uchar*)"gs_lev2.ps");	
	init_file_array[3].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 9, (uchar*)"gs_res.ps");
	init_file_array[4].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_typ42.ps");
	init_file_array[5].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_cidfn.ps");
	init_file_array[6].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_cidcm.ps");	
	init_file_array[7].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 10, (uchar*)"gs_cmap.ps");
	init_file_array[8].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_btokn.ps");
	init_file_array[9].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 10, (uchar*)"gs_dps2.ps");	
	init_file_array[10].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_setpd.ps");
	init_file_array[11].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_typ32.ps");
	init_file_array[12].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 10, (uchar*)"gs_frsd.ps");
	init_file_array[13].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 9, (uchar*)"gs_ll3.ps");
	init_file_array[14].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_mex_e.ps");
	init_file_array[15].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_mro_e.ps");	
	init_file_array[16].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_pdf_e.ps");
	init_file_array[17].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_wan_e.ps");
	init_file_array[18].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 10, (uchar*)"pdf_ops.ps");	
	init_file_array[19].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_l2img.ps");
	init_file_array[20].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"pdf_base.ps");
	init_file_array[21].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"pdf_draw.ps");
	init_file_array[22].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"pdf_font.ps");
	init_file_array[23].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"pdf_main.ps");	
	init_file_array[24].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 10, (uchar*)"pdf_sec.ps");	
	init_file_array[25].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_css_e.ps");
	init_file_array[26].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 9, (uchar*)"gs_cff.ps");
	init_file_array[27].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_mgl_e.ps");
	init_file_array[28].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 9, (uchar*)"gs_agl.ps");
	init_file_array[29].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 9, (uchar*)"gs_ttf.ps");
	init_file_array[30].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 9, (uchar*)"gs_dps.ps");	
	init_file_array[31].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_dpnxt.ps");
	init_file_array[32].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_pdfwr.ps");
	init_file_array[33].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_lgo_e.ps");
	init_file_array[34].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 11, (uchar*)"gs_lgx_e.ps");
	
	XWPSRef ifa1;    
  ifa1.makeArray(PS_A_READONLY | PS_AVM_FOREIGN, 4);
  initialEnterName("EMULATORS", &ifa1);
  XWPSRef * emulator_name_array = ifa1.getArray();	
  emulator_name_array[0].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 10, (uchar*)"PostScript");	
  emulator_name_array[1].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 16, (uchar*)"PostScriptLevel1");	
  emulator_name_array[2].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 16, (uchar*)"PostScriptLevel2");
  emulator_name_array[3].makeString(PS_A_READONLY + PS_AVM_FOREIGN, 3, (uchar*)"PDF");
  
  QString bindir = QCoreApplication::applicationDirPath();
  QString librootpath = bindir;
  int ti = librootpath.lastIndexOf(QChar('/'));
  if (ti > 0)
   	librootpath = librootpath.left(ti);
   	
  QString gspath = QString("%1/Ghostscript").arg(librootpath);
  QString gslibpath = QString("%1/lib").arg(gspath);
  QString gsfontspath = QString("%1/fonts").arg(gspath);
  
  QByteArray bgslibpath = gslibpath.toLocal8Bit();
  QByteArray bgsfontspath = gsfontspath.toLocal8Bit();
  
  XWPSRef gslibpathlist; 
  gslibpathlist.makeArray(PS_A_READONLY | PS_AVM_FOREIGN, 2);
  XWPSRef * path_array = gslibpathlist.getArray();
  path_array[0].makeString(PS_A_READONLY + PS_AVM_FOREIGN, bgslibpath.size(), (uchar*)bgslibpath.data());
  path_array[1].makeString(PS_A_READONLY + PS_AVM_FOREIGN, bgsfontspath.size(), (uchar*)bgsfontspath.data());
  
  initialEnterName("LIBPATH", &gslibpathlist);
  
  return runInitFile();
}

int  XWPSContextState::initialEnterName(const char *nstr, XWPSRef * pref)
{
	return initialEnterNameIn(dict_stack.getSystemDict(), nstr, pref);
}

int XWPSContextState::initialEnterNameIn(XWPSRef *pdict, const char *nstr, XWPSRef * pref)
{
	return pdict->dictPutString(this, nstr, pref, &dict_stack);
}

void XWPSContextState::initialRemoveName(const char *nstr)
{
	XWPSRef nref;

  if (nameRef((const uchar *)nstr, strlen(nstr), &nref, -1) >= 0)
		dictUndef(dict_stack.getSystemDict(), &nref); 
}

int XWPSContextState::initState()
{
	XWPSDeviceNull * nulldev = new XWPSDeviceNull;
	nulldev->context_state = this;
	pgs = new XWPSState(nulldev);
	pgs->setDeviceNoErase(nulldev);
	pgs->setLimitClamp(true);
	
	XWPSIntGState * iigs = new XWPSIntGState;
	pgs->client_data = iigs;
	
	iigs->dash_pattern.makeArray(PS_A_ALL);
	iigs->black_generation.makeArray(iimemoryLocal()->space | (PS_A_READONLY + PS_A_EXECUTABLE), 2);
	XWPSRef * pref = iigs->black_generation.getArray();
	pref->makeOper(0, &XWPSContextState::zpop);
	pref++;
	pref->makeReal(0.0);
	
	iigs->undercolor_removal.makeArray(iimemoryLocal()->space | (PS_A_READONLY + PS_A_EXECUTABLE), 2);
	pref = iigs->undercolor_removal.getArray();
	pref->makeOper(0, &XWPSContextState::zpop);
	pref++;
	pref->makeReal(0.0);
	
	XWPSIntRemapColorInfo * prci = new XWPSIntRemapColorInfo;
	iigs->remap_color_info.makeStruct(imemoryGlobal()->space, prci);
	return 0;
}

int XWPSContextState::load()
{
	idmemory()->setNotInSave();
	uint space = dict_stack.getSystemDict()->space();
	dict_stack.getSystemDict()->setSpace(PS_AVM_MAX);
	XWPSRef *puserdict = dict_stack.getUserDict();
	if (puserdict)
	{
		XWPSRef *plocaldicts = 0;
		if (puserdict->dictFindString(this, "localdicts", &plocaldicts) > 0 &&  
			  plocaldicts->hasType(XWPSRef::Dictionary)) 
		{
	    plocaldicts->dictCopy(this, dict_stack.getSystemDict(), &dict_stack);
		}
	}
	
	int code = dict_stack.getSystemDict()->dictPutString(this, "userparams", &userparams, &dict_stack);
	if (code >= 0)
		code = setUserParams(&userparams);
	dict_stack.getSystemDict()->setSpace(space);
	if (iimemoryLocal()->save_level > 0)
		idmemory()->setInSave();
	dict_stack.setTop();
	return code;
}

XWPSRef * XWPSContextState::makeInitialDict(const char *iname, XWPSRef idicts[])
{
	if (!strcmp(iname, "systemdict"))
		return dict_stack.getSystemDict();
		
	int count = sizeof(initial_dictionaries) / sizeof(initial_dictionaries[0]);
	for (int i = 0; i < count; i++) 
	{
		const char *dname = initial_dictionaries[i].name;
		const int dsize = initial_dictionaries[i].size;

		if (!strcmp(iname, dname)) 
		{
	    XWPSRef * dref = &idicts[i];
	    if (dref->hasType(XWPSRef::Null)) 
	    {
	    	XWPSRefMemory *mem =(initial_dictionaries[i].local ? iimemoryLocal() : iimemoryGlobal());
				int code = dref->dictAlloc(mem->space, dsize);
				if (code < 0)
		    	return 0;
	    }
	    return dref;
		}
  }
  return 0;
}

void XWPSContextState::makeOper(XWPSRef * opref, op_proc_t proc, int idx)
{
	int i ;
	for (i = num_special_ops; i > 0 && proc != interp_op_defs[i].proc; --i)
		;
    
  if (i > 0)
		opref->makeOper(tx_op + (i - 1), i, PS_A_EXECUTABLE, proc);
  else
		opref->makeOper(idx, PS_A_EXECUTABLE, proc);
}

int XWPSContextState::numArrayFormat(XWPSRef * op)
{
	switch (op->type()) 
	{
		case XWPSRef::String:
	    {
				uchar *bp = op->getBytes();
				if (op->size() < 4 || bp[0] != bt_num_array_value)
		    	return (int)(XWPSError::RangeCheck);
				int format = bp[1];
				if (!num_is_valid(format) || 
		        sdecodeshort(bp + 2, format) != (op->size() - 4) / encoded_number_bytes(format))
		    	return (int)(XWPSError::RangeCheck);
				return format;
	    }
	    
		case XWPSRef::Array:
		case XWPSRef::MixedArray:
		case XWPSRef::ShortArray:
	    return num_array;
	    
		default:
	    return (int)(XWPSError::TypeCheck);
  }
}

int XWPSContextState::numArrayGet(XWPSRef * op, int format, uint index, XWPSRef * np)
{
	if (format == num_array) 
	{
		int code = op->arrayGet(this, (long)index, np);
		if (code < 0)
	    return XWPSRef::Null;
	    	
		switch (np->type()) 
		{
	    case XWPSRef::Integer:
				return XWPSRef::Integer;
				
	    case XWPSRef::Real:
				return XWPSRef::Real;
				
	    default:
				return (int)(XWPSError::RangeCheck);
		}
  } 
  else 
  {
		uint nbytes = encoded_number_bytes(format);
		if (index >= (op->size() - 4) / nbytes)
	    return XWPSRef::Null;
		return sdecode_number(op->getBytes() + 4 + index * nbytes, format, np);
  }
}

uint XWPSContextState::numArraySize(XWPSRef * op, int format)
{
	return (format == num_array ? op->size() : (op->size() - 4) / encoded_number_bytes(format));
}

int XWPSContextState::objInit()
{
	XWPSRef system_dict;
	
	bool level2 = haveLevel2();
	int code = system_dict.dictAlloc(iimemoryGlobal()->space, level2 ? SYSTEMDICT_LEVEL2_SIZE : SYSTEMDICT_SIZE);
	if (code < 0)
		return code;
		
	code = initState();
	if (code < 0)
		return code;
	
	dict_stack.setSystemDict(&system_dict);
	uint size = 20;
	XWPSRef * puserparams = 0;
	if (system_dict.dictFindString(this, "userparams", &puserparams) >= 0)
		size = puserparams->dictLength();
	
	userparams.dictAlloc(iimemoryLocal()->space, size);	
	load();
	
	dict_stack.setMinSize(MIN_DSTACK_SIZE);
	
	XWPSRef * p = dict_stack.getCurrentTop();
	if (level2) 
	{
	  p = dict_stack.incCurrentTop(2);
	  p[-1].assign(&system_dict);
	  dict_stack.setMinSize(dict_stack.getMinSize() + 1);
	} 
	else 
	  p = dict_stack.incCurrentTop(1);
	  
	p->assign(&system_dict);
	  
#define icount (sizeof(initial_dictionaries) / sizeof(initial_dictionaries[0]))
	XWPSRef idicts[icount];
	
	PSOperator **tptr = op_defs_all;	
	for (; *tptr != 0; tptr++) 
	{
	  const PSOperator *def = *tptr;
	  for (; def->oname != 0; def++)
			if (op_def_is_begin_dict(def)) 
		    makeInitialDict(def->oname, idicts);
	}
	
	for (int i = 0; i < (sizeof(initial_dstack) / sizeof(initial_dstack[0])); i++) 
	{
	  const char *dname = initial_dstack[i];			
	  p = dict_stack.incCurrentTop(1);
	  if (!strcmp(dname, "userdict"))
			dict_stack.setUserDictIndex(p - dict_stack.getBottom());
	  p->assign(makeInitialDict(dname, idicts));
	}
	
	initialEnterName("systemdict", dict_stack.getSystemDict());
	for (int i = 0; i < icount; i++) 
	{
	    XWPSRef *idict = &idicts[i];
	    if (!idict->hasType(XWPSRef::Null)) 
	    {
				uint save_space = dict_stack.getSystemDict()->space();

				dict_stack.getSystemDict()->setSpace(PS_AVM_LOCAL);
				code = initialEnterName(initial_dictionaries[i].name, idict);
				dict_stack.getSystemDict()->setSpace(save_space);
				if (code < 0)
					return code;
	    }
	}
#undef icount
	
	reset();
	
	{
		XWPSRef vnull, vtrue, vfalse;
		vnull.makeNull();
		vtrue.makeTrue();
		vfalse.makeFalse();
		initialEnterName("null", &vnull);
		initialEnterName("true", &vtrue);
		initialEnterName("false", &vfalse);
	}
	
	int n = XWPSError::countOfName() - 1;
	XWPSRef era;
	era.makeArray(currentSpace() | PS_A_READONLY, n);
	for (int i = 0; i < n; i++)
	{
		XWPSRef * pvalue = era.getArray() + i;
		nameEnterString(XWPSError::getName(i), pvalue);
	}
	initialEnterName("ErrorNames", &era);
	
	return 1;
}

int XWPSContextState::opInit()
{
	int code;
	PSOperator **tptr = op_defs_all;	
	
	for (; *tptr != 0; tptr++) 
	{
		XWPSRef * dict = dict_stack.getSystemDict();
		const char *nstr;
		PSOperator *def = *tptr;
	  for (; (nstr = def->oname) != 0; def++)
	  {
	  	if (op_def_is_begin_dict(def))
	  	{
	  		XWPSRef nref;
	  		code = nameRef((const uchar *)nstr, strlen(nstr), &nref, -1);
	  		if (code < 0)
	  			return code;
	  		if (!(dict_stack.getSystemDict()->dictFind(this, &nref, &dict)))
	  			return (int)(XWPSError::Fatal);
	  		if (!(dict->hasType(XWPSRef::Dictionary)))
	  			return (int)(XWPSError::Fatal);
	  	}
	  	else
	  	{
	  		XWPSRef oper;
				uint index_in_table = def - *tptr;
				uint opidx = (tptr - op_defs_all) * OP_DEFS_MAX_SIZE + index_in_table;
				makeOper(&oper, def->proc, opidx);
				if (*nstr - '0' > 16)
		    	return (int)(XWPSError::Fatal);
				nstr++;
				if (*nstr != '%' && oper.size() == opidx)
				{
					code = initialEnterNameIn(dict, nstr, &oper);
					if (code < 0)
						return code;
				}
	  	}
	  }
	}
	
	op_array_table_global.init(this, OP_ARRAY_TABLE_GLOBAL_SIZE, PS_AVM_GLOBAL, op_def_count);
	op_array_table_local.init(this, OP_ARRAY_TABLE_LOCAL_SIZE, PS_AVM_LOCAL, op_def_count + op_def_count);
	
	return 0;
}

int XWPSContextState::zopInit()
{
	int code;
	
	PSOperator **tptr = op_defs_all;	
	for (; *tptr != 0; tptr++) 
	{
	  PSOperator *def = *tptr;
	  for (; def->oname != 0; def++)
	  	;
	  	
	  if (def->proc != 0)
	  {
	  	code = (this->*def->proc)();
	  	if (code < 0)
	  		return code;
	  }
	}
	
	const char * copyright = xwApp->getCopyRight8();
	const char * product = "Ghostscript";
	const char * productfamily = "Xiuwen";
	QString revi("650");
	QString revisiondates("20140622");
	long revisions = revi.toLong();
	long revisiondate = revisiondates.toLong();	
	
	XWPSRef vcr, vpr, vpf, vre, vrd;
	vcr.makeString(PS_A_READONLY | PS_AVM_FOREIGN, strlen(copyright), (uchar*)copyright);
	vpr.makeString(PS_A_READONLY | PS_AVM_FOREIGN, strlen(product), (uchar*)product);
	vpf.makeString(PS_A_READONLY | PS_AVM_FOREIGN, strlen(productfamily), (uchar*)productfamily);
	vre.makeInt(revisions);
	vrd.makeInt(revisiondate);
	initialEnterName("copyright", &vcr);
	initialEnterName("product", &vpr);
	initialEnterName("productfamily", &vpf);
	initialEnterName("revision", &vre);
	initialEnterName("revisiondate", &vrd);
	
	return 0;
}

bool XWPSContextState::recognizeBtokens()
{
	return (binary_object_format.getInt() != 0 && level2Enabled());
}

int  XWPSContextState::runInitFile()
{
	XWPSRef ifile, first_token;
	uchar fn[200];
  uint len;
	int code = libFileOpen("gs_init.ps", 10, fn, 200, &len, &ifile);
	if (code < 0)
		return code;
		
	ifile.setAttrs(PS_A_EXECUTE + PS_A_EXECUTABLE);
	XWPSScannerState state;
	state.init(false);
  code = scanToken(ifile.getStream(), &first_token,  &state);
  if (code != 0 || !first_token.hasType(XWPSRef::Integer)) 
  {
		return (int)(XWPSError::Fatal);
  }
  
  XWPSRef * osp = op_stack.incCurrentTop(1);
  osp->assign(&first_token);
  ifile.setAttrs(PS_A_EXECUTABLE);
  return callInterpret(&ifile);
}
