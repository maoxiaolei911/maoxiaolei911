/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <QDateTime>
#include "XWPSError.h"
#include "XWPSFont.h"
#include "XWPSState.h"
#include "XWPSParam.h"
#include "XWPSContextState.h"

static int reschedule_interval = 100;

static bool I_VALIDATE_BEFORE_SAVE = true;
static bool I_VALIDATE_AFTER_SAVE = true;
static bool I_VALIDATE_BEFORE_RESTORE = true;
static bool I_VALIDATE_AFTER_RESTORE = true;

enum PSCTXStatus
{
    cs_active,
    cs_done
} ;

class XWPSScheduler;

class XWPSContext : public XWPSStruct
{
public:
	XWPSContext();
	
	int getLength();
	const char * getTypeName();
	
public:
	XWPSContextState * state;	
	
  XWPSScheduler *scheduler;
  PSCTXStatus status;
  long index;	
  bool detach;
  bool saved_local_vm;
  bool visible;	
  long next_index;
  long joiner_index;
  XWPSContext *table_next;	
};

XWPSContext::XWPSContext()
	:XWPSStruct()
{
	state = 0;
	status =cs_active ;
	index = 0;
	detach = false;
	saved_local_vm = false;
	visible = false;
	next_index = 0;
	joiner_index = 0;
	table_next = 0;
}


int XWPSContext::getLength()
{
	return sizeof(XWPSContext);
}

const char * XWPSContext::getTypeName()
{
	return "context";
}
	

struct PSParamDef
{
	const char * pname;
};

struct PSSLongParamDef
{
	const char * pname;
	long min_value, max_value;
  long (XWPSContextState::*current)();
  int (XWPSContextState::*set)(long);
};

#if arch_sizeof_long > arch_sizeof_int
#  define MAX_UINT_PARAM max_uint
#else
#  define MAX_UINT_PARAM max_long
#endif

struct PSBoolParamDef
{
	const char * pname;
	bool (XWPSContextState::*current)();
  int (XWPSContextState::*set)(bool);
};

struct PSStringParamDef
{
	const char * pname;
	void (XWPSContextState::*current)(PSParamString *);
  int (XWPSContextState::*set)(PSParamString *);
};

struct PSParamSet
{
	const PSSLongParamDef *long_defs;
  uint long_count;
  const PSBoolParamDef *bool_defs;
  uint bool_count;
  const PSStringParamDef *string_defs;
  uint string_count;
};

static const PSSLongParamDef system_long_params[] =
{
    {"BuildTime", min_long, max_long, &XWPSContextState::currentBuildTime, NULL},
{"MaxFontCache", 0, MAX_UINT_PARAM, &XWPSContextState::currentMaxFontCache, &XWPSContextState::setMaxFontCache},
    {"CurFontCache", 0, MAX_UINT_PARAM, &XWPSContextState::currentCurFontCache, NULL},
    {"Revision", min_long, max_long, &XWPSContextState::currentRevision, NULL},
    /* Extensions */
    {"MaxGlobalVM", 0, max_long, &XWPSContextState::currentMaxGlobalVM, &XWPSContextState::setMaxGlobalVM}
};

static const PSBoolParamDef system_bool_params[] =
{
    {"ByteOrder", &XWPSContextState::currentByteOrder, NULL}
};

static const PSStringParamDef system_string_params[] =
{
    {"RealFormat", &XWPSContextState::currentRealFormat, NULL}
};

static const PSParamSet system_param_set =
{
    system_long_params, 5,
    system_bool_params, 1,
    system_string_params, 1
};

static const PSSLongParamDef user_long_params[] =
{
  {"JobTimeout", 0, MAX_UINT_PARAM, &XWPSContextState::currentJobTimeout, &XWPSContextState::setJobTimeout},
  {"MaxFontItem", 0, MAX_UINT_PARAM, &XWPSContextState::currentMaxFontItem, &XWPSContextState::setMaxFontItem},
  {"MinFontCompress", 0, MAX_UINT_PARAM, &XWPSContextState::currentMinFontCompress, &XWPSContextState::setMinFontCompress},
  {"MaxOpStack", 0, MAX_UINT_PARAM, &XWPSContextState::currentMaxOpStack, &XWPSContextState::setMaxOpStack},
  {"MaxDictStack", 0, MAX_UINT_PARAM,  &XWPSContextState::currentMaxDictStack, &XWPSContextState::setMaxDictStack},
  {"MaxExecStack", 0, MAX_UINT_PARAM, &XWPSContextState::currentMaxExecStack, &XWPSContextState::setMaxExecStack},
  {"MaxLocalVM", 0, max_long, &XWPSContextState::currentMaxLocalVM, &XWPSContextState::setMaxLocalVM},
  {"VMReclaim", -2, 0, &XWPSContextState::currentVMReclaim, &XWPSContextState::setVMReclaim},
  {"VMThreshold", -1, max_long, &XWPSContextState::currentVMThreshold, &XWPSContextState::setVMThreshold},
  {"WaitTimeout", 0, MAX_UINT_PARAM, &XWPSContextState::currentWaitTimeout, &XWPSContextState::setWaitTimeout},
  {"MinScreenLevels", 0, MAX_UINT_PARAM, &XWPSContextState::currentMinScreenLevels, &XWPSContextState::setMinScreenLevels}
};

static const PSBoolParamDef user_bool_params[] =
{
    {"AccurateScreens", 
    	&XWPSContextState::currentAccurateScreens, 
    	&XWPSContextState::setAccurateScreens}
};

static const PSParamSet user_param_set =
{
    user_long_params, _COUNT_OF_(user_long_params),
    user_bool_params, _COUNT_OF_(user_bool_params),
    0, 0
};

static bool
pname_matches(const char *pname, XWPSRef * psref)
{
   return	(psref == 0 || !bytes_compare((const uchar *)pname, strlen(pname), psref->getBytes(), psref->size()));
}

int XWPSContextState::awaitLock()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = lockAcquire(op - 1, 0);
  if (code == 0) 
  {
		pop(2);
		return 0;
  }
	
	XWPSRef * esp = exec_stack.incCurrentTop(1);
	esp->makeOper(0, &XWPSContextState::awaitLock);
  return code;
}

long XWPSContextState::contextUserTime()
{
	static qint64 offset = 0;
	qint64 ticks = 0;
	if (!offset)
	{
		offset = QDateTime::currentDateTime().toTime_t();
		ticks = QDateTime::currentDateTime().toMSecsSinceEpoch();
		offset -= (ticks / 100);
	}
	
	ticks = QDateTime::currentDateTime().toMSecsSinceEpoch();		
	long secs_ns[2];
	secs_ns[0] = (long)(ticks / 100 + offset);
	secs_ns[1] = (long)((ticks % 100) * (1000 * 1000 / 100));
	return secs_ns[0] * 1000 + secs_ns[1] / 1000000;
}

bool XWPSContextState::currentAccurateScreens()
{
	return pgs->currentAccurateScreens();
}

bool XWPSContextState::currentByteOrder()
{
	return !arch_is_big_endian;
}

long XWPSContextState::currentCurFontCache()
{
	uint cstat[7];

  ifont_dir->cacheStatus(cstat);
  return cstat[0];
}

long XWPSContextState::currentMaxDictStack()
{
	return dict_stack.maxCount();
}

long XWPSContextState::currentMaxExecStack()
{
	return exec_stack.maxCount();
}

long XWPSContextState::currentMaxFontCache()
{
	return ifont_dir->currentCacheSize();
}

long XWPSContextState::currentMaxGlobalVM()
{
	return 0;
}

long XWPSContextState::currentMaxOpStack()
{
	return op_stack.maxCount();
}

long XWPSContextState::currentMinFontCompress()
{
	return ifont_dir->currentCacheLower();
}

long XWPSContextState::currentMaxFontItem()
{
	return ifont_dir->currentCacheUpper();
}

long XWPSContextState::currentMinScreenLevels()
{
	return pgs->currentMinScreenLevels();
}

int XWPSContextState::currentParamList(const PSParamSet * pset, XWPSRef * psref)
{
	XWPSStackParamList list(op_stack.getStack(), 0, (QObject*)0);
  XWPSParamList * plist = (XWPSParamList *)&list;
  int i;

  list.writing = true;
  for (i = 0; i < pset->long_count; i++) 
  {
		const char *pname = pset->long_defs[i].pname;

		if (pname_matches(pname, psref)) 
		{
	    long val = (this->*(pset->long_defs[i].current))();
	    int code = plist->writeLong(this, pname, &val);

	    if (code < 0)
				return code;
		}
  }
  for (i = 0; i < pset->bool_count; i++) 
  {
		const char *pname = pset->bool_defs[i].pname;

		if (pname_matches(pname, psref)) 
		{
	    bool val = (this->*(pset->bool_defs[i].current))();
	    int code = plist->writeBool(this, pname, &val);

	    if (code < 0)
				return code;
		}
  }
  for (i = 0; i < pset->string_count; i++) 
  {
		const char *pname = pset->string_defs[i].pname;

		if (pname_matches(pname, psref)) 
		{
	    PSParamString val;
	    int code;

	    (this->*(pset->string_defs[i].current))(&val);
	    code = plist->writeString(this, pname, &val);
	    if (code < 0)
			return code;
		}
  }
  return 0;
}

int  XWPSContextState::currentParams(const PSParamSet * pset)
{
	return currentParamList(pset, NULL);
}

int XWPSContextState::currentParam1(const PSParamSet * pset)
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef sref;
  int code;

  code = op->checkType(XWPSRef::Name);
  if (code < 0)
  	return code;
  	
  code = checkOStack(2);
  if (code < 0)
  	return code;
  	
  nameStringRef(op, &sref);
  code = currentParamList(pset, &sref);
  if (code < 0)
		return code;
    
  if (op_stack.getCurrentTop() == op)
		return (int)(XWPSError::Undefined);

  op->assign(op + 2);
  pop(2);
  return code;
}

void XWPSContextState::currentRealFormat(PSParamString * pval)
{
#if arch_floats_are_IEEE
    static const char *const rfs = "IEEE";
#else
    static const char *const rfs = "not IEEE";
#endif

    pval->data = (uchar *)rfs;
    pval->size = strlen(rfs);
}

long XWPSContextState::currentRevision()
{
	return 650;
}

int XWPSContextState::doFork(XWPSRef * , 
	          const XWPSRef * , 
	          XWPSRef * ,
	          uint , 
	          bool )
{
  return 0;
}

int XWPSContextState::finishJoin()
{
	return zjoin();
}

int XWPSContextState::forkDone()
{
	return 0;
}

int XWPSContextState::interpExit()
{
	return XWPSError::InterpreterExit;
}

int XWPSContextState::lockAcquire(XWPSRef *, XWPSContext * )
{
	return 0;
}

int XWPSContextState::lockRelease(XWPSRef * )
{
	return 0;
}

int XWPSContextState::monitorCleanup()
{
	int code = lockRelease(exec_stack.getCurrentTop());
  if (code < 0)
		return code;
	
	exec_stack.incCurrentTop(-1);
  return PS_O_POP_ESTACK;
}

int XWPSContextState::monitorRelease()
{
	int code = lockRelease(exec_stack.getCurrentTop() - 1);
  if (code < 0)
		return code;
	
	exec_stack.incCurrentTop(-2);
  return PS_O_POP_ESTACK;
}

int XWPSContextState::setAccurateScreens(bool val)
{
	pgs->setAccurateScreens(val);
	return 0;
}

int XWPSContextState::setMaxFontCache(long val)
{
	return ifont_dir->setCacheSize((uint)(val < 0 ? 0 : val > max_uint ? max_uint :  val));
}

int XWPSContextState::setMaxGlobalVM(long )
{
	return 0;
}

int XWPSContextState::setMaxDictStack(long val)
{
	return dict_stack.setMaxCount(val);
}

int XWPSContextState::setMaxExecStack(long val)
{
	return exec_stack.setMaxCount(val);
}

int XWPSContextState::setMaxFontItem(long val)
{
	return ifont_dir->setCacheUpper(val);
}

int XWPSContextState::setMaxOpStack(long val)
{
	return op_stack.setMaxCount(val);
}

int XWPSContextState::setMinFontCompress(long val)
{
	return ifont_dir->setCacheLower(val);
}

int XWPSContextState::setMinScreenLevels(long val)
{
	pgs->setMinScreenLevels(val);
	return 0;
}

int XWPSContextState::setParams(XWPSParamList * plist, const PSParamSet * pset)
{
	int i, code;

  for (i = 0; i < pset->long_count; i++) 
  {
		const PSSLongParamDef *pdef = &pset->long_defs[i];
		long val;

		if (pdef->set == NULL)
	    continue;
		code = plist->readLong(this, pdef->pname, &val);
		switch (code) 
		{
	    default:
				return code;
				
	    case 1:	
				break;
				
	    case 0:
				if (val < pdef->min_value || val > pdef->max_value)
		    	return (int)(XWPSError::RangeCheck);
				code = (this->*(pdef->set))(val);
				if (code < 0)
		    	return code;
		}
  }
  for (i = 0; i < pset->bool_count; i++) 
  {
		const PSBoolParamDef *pdef = &pset->bool_defs[i];
		bool val;

		if (pdef->set == NULL)
	    continue;
		code = plist->readBool(this, pdef->pname, &val);
		if (code == 0)
	    code = (this->*(pdef->set))(val);
		if (code < 0)
	    return code;
  }

  return 0;
}

int XWPSContextState::setUserParams(XWPSRef *paramdict)
{
	XWPSDictParamList list(false, paramdict, 0, false, (QObject*)0);
  int code;

  code = paramdict->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  list.writing = false;
  code = setParams(&list, &user_param_set);
  return code;
}

int XWPSContextState::specificVMOp(op_proc_t opproc, uint space)
{
	uint save_space = idmemory()->iallocSpace();
	idmemory()->setSpace(space);
	int code = (this->*opproc)();
	idmemory()->setSpace(save_space);
	return code;
}

void XWPSContextState::stackCopy(XWPSRefStack * to, 
	               XWPSRefStack * from, 
	               uint count,
	               uint from_index)
{
	long i;

  for (i = (long)count - 1; i >= 0; --i)
		to->index(i)->assign(from->index(i + from_index));
}

int XWPSContextState::zbosObject()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op[-3].checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  code = op[-2].checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  code = op->checkWriteType(XWPSRef::String);
  if (code < 0)
		return code;
		
  if (op->size() < 8)
		return (int)(XWPSError::RangeCheck);
  code = encodeBinaryToken(op - 1, &op[-3].value.intval, &op[-2].value.intval, op->getBytes());
  if (code < 0)
		return code;
  op[-1].assign(op);
  op[-1].setSize(8);
  pop(1);
  return 0;
}

int XWPSContextState::zbseqInit()
{
	if (!system_names_p)
	{
		system_names_p = new XWPSRef;
		system_names_p->makeArray(PS_A_READONLY);
	}
	return 0;
}

int XWPSContextState::zcheckPassword()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef params[2];
  PSPassword pass;
  int result = 0;
  nameRef((const uchar *)"Password", 8, &params[0], 0);
  
  params[1].assign(op);
  if (dict_stack.getSystemDict()->dictReadPassword(this, "StartJobPassword", &pass) >= 0)
		result = 1;
  if (dict_stack.getSystemDict()->dictReadPassword(this, "SystemParamsPassword", &pass) >= 0 )
		result = 2;
  op->makeInt(result);
  return 0;
}

int XWPSContextState::zcondition()
{
	XWPSStruct *pcond = new XWPSStruct;
	XWPSRef * op = op_stack.getCurrentTop();
  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeStruct(currentSpace() | PS_A_ALL, pcond);
  return 0;
}

int XWPSContextState::zcurrentContext()
{
	XWPSRef * op = op_stack.getCurrentTop();
	int code = push(&op, 1);
	if (code < 0)
		return code;
  op->makeInt(0);
  return 0;
}

int XWPSContextState::zcurrentGlobal()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeBool(idmemory()->iallocSpace() != PS_AVM_LOCAL);
  return 0;
}

int XWPSContextState::zcurrentObjectFormat()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->assign(&binary_object_format);
  return 0;
}

int XWPSContextState::zcurrentSystemParams()
{
	return currentParams(&system_param_set);
}

int XWPSContextState::zcurrentUserParams()
{
	return currentParams(&user_param_set);
}

int XWPSContextState::zdetach()
{
	pop(1);
  return 0;
}

int XWPSContextState::zforgetSave()
{
	XWPSRef * op = op_stack.getCurrentTop();
	int code;
	
	code = op->checkType(XWPSRef::Save);
	if (code < 0)
		return code;
	
//	op_stack.restoreFixStack(false);
//  exec_stack.restoreFixStack(false);
//  dict_stack.restoreFixStack(false);
    
	ulong id = op->value.saveid;
	XWPSState * saved = saved_states[id];
	{
		XWPSState * pgsA = pgs;
		XWPSState * last = pgsA->getSaved();
		
		while (last && (last->getSaved() != 0))
		{
			pgsA = last;
			last = pgsA->getSaved();
		}
		
		if (last)
		{
			last->swapSaved(saved);
			last->restore();
			last->restore();
		}
	}
	
	pop(1);
  return 0;
}

int XWPSContextState::zfork()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint mcount = op_stack.countToMark();
  XWPSRef rnull;

  if (mcount == 0)
		return (int)(XWPSError::UnmatchedMark);
			
  rnull.makeNull();
  return doFork(op, &rnull, &rnull, mcount, false);
}

int XWPSContextState::zgcheck()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = checkOp(op, 1);
  if (code < 0)
  	return code;
  	
  op->makeBool((op->isLocal() ? false : true));
  return 0;
}

int XWPSContextState::zgetSystemParam()
{
	return currentParam1(&system_param_set);
}

int XWPSContextState::zgetUserParam()
{
	return currentParam1(&user_param_set);
}

int XWPSContextState::zglobalVMArray()
{
	return specificVMOp(&XWPSContextState::zarray, PS_AVM_GLOBAL);
}

int XWPSContextState::zglobalVMDict()
{
	return specificVMOp(&XWPSContextState::zdict, PS_AVM_GLOBAL);
}

int XWPSContextState::zglobalVMPackedArray()
{
	return specificVMOp(&XWPSContextState::zpackedArray, PS_AVM_GLOBAL);
}

int XWPSContextState::zglobalVMString()
{
	return specificVMOp(&XWPSContextState::zstring, PS_AVM_GLOBAL);
}

int XWPSContextState::zinstallSystemNames()
{
	XWPSRef * op = op_stack.getCurrentTop();
	if (op->space() != PS_AVM_GLOBAL || iimemoryGlobal()->save_level != 0)
		return (int)(XWPSError::InvalidAccess);
  int code = op->checkReadType(XWPSRef::ShortArray);
  if (code < 0)
		return code;
	
  system_names_p->assign(op);
  pop(1);
  return 0;
}

int XWPSContextState::zjoin()
{
	return 0;
}

int XWPSContextState::zlock()
{
	XWPSRef * op = op_stack.getCurrentTop();
	XWPSStruct * plock = new XWPSStruct;
  int code = push(&op,1);
  if (code < 0)
		return code;
  op->makeStruct(currentSpace() | PS_A_ALL, plock);
  return 0;
}

int XWPSContextState::zlocalFork()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint mcount = op_stack.countToMark();
  int code;

  if (mcount == 0)
		return (int)(XWPSError::UnmatchedMark);
    
  code = doFork(op - 2, op - 1, op, mcount - 2, true);
  if (code < 0)
		return code;
    
  op->assign(op_stack.getCurrentTop());
  op[-2].assign(op);
  pop(2);
  return code;
}

int XWPSContextState::zlocalVMArray()
{
	 return specificVMOp(&XWPSContextState::zarray, PS_AVM_LOCAL);
}

int XWPSContextState::zlocalVMDict()
{
	return specificVMOp(&XWPSContextState::zdict, PS_AVM_LOCAL);
}

int XWPSContextState::zlocalVMPackedArray()
{
	return specificVMOp(&XWPSContextState::zpackedArray, PS_AVM_LOCAL);
}

int XWPSContextState::zlocalVMString()
{
	return specificVMOp(&XWPSContextState::zstring, PS_AVM_LOCAL);
}

int XWPSContextState::zmonitor()
{
  XWPSRef * op = op_stack.getCurrentTop();
  int code = lockAcquire(op - 1, 0);
  if (code != 0) 
  {	
  	exec_stack.incCurrentTop(1);
  	exec_stack.getCurrentTop()->makeOper(0, &XWPSContextState::zmonitor);
		return code;
  }
    
  XWPSRef * esp = exec_stack.incCurrentTop(1);
  esp->assign(&op[-1]);
  esp = exec_stack.incCurrentTop(1);
  esp->makeMarkEStack(PS_ES_OTHER, &XWPSContextState::monitorCleanup);
  esp = exec_stack.incCurrentTop(1);
  esp->makeOper(0, &XWPSContextState::monitorRelease);
  esp = exec_stack.incCurrentTop(1);
  esp->assign(op);
  pop(2);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::znotify()
{
	XWPSRef * op = op_stack.getCurrentTop();
  pop(1);
  op--;
  return zyield();
}

int XWPSContextState::zrestore()
{
	XWPSRef * op = op_stack.getCurrentTop();
	int code;
	
	code = op->checkType(XWPSRef::Save);
	if (code < 0)
		return code;
		
//	op_stack.restoreFixStack(false);
//  exec_stack.restoreFixStack(true);
//  dict_stack.restoreFixStack(false);
  
	ulong id = op->value.saveid;
	XWPSState * orgin_pgs = saved_states.take(id);
	op_stack.incCurrentTop(-1);
	pgs->restoreAllForRestore(orgin_pgs);
	dict_stack.setTop();
	return 0;
}

int XWPSContextState::zsave()
{
	XWPSRef * op = op_stack.getCurrentTop();
	uint space = idmemory()->iallocSpace();
	idmemory()->setSpace(PS_AVM_LOCAL);
	ulong id = nextIDS(2);
	XWPSState * orgin_pgs = 0;
	int code = pgs->saveForSave(&orgin_pgs);
	idmemory()->setSpace(space);
	if (code < 0)
		return code;
		
	code = pgs->save();
  if (code < 0)
		return code;
		
	code = push(&op, 1);
	if (code < 0)
		return code;
  op->makeSave(0, id);
  saved_states[id] = orgin_pgs;
  return 0;
}

int XWPSContextState::zsetGlobal()
{
	XWPSRef * op = op_stack.getCurrentTop();
	
  int code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
		return code;
		
	idmemory()->setSpace(op->value.boolval ? PS_AVM_GLOBAL : PS_AVM_LOCAL);
  pop(1);
  return 0;
}

int XWPSContextState::zsetObjectFormat()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef cont;

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  if (op->value.intval < 0 || op->value.intval > 4)
		return (int)(XWPSError::RangeCheck);
  binary_object_format.assign(op);
  
  pop(1);
  return 0;
}

int XWPSContextState::zsetSystemParams()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  XWPSDictParamList list(false, op, NULL, false, (QObject*)0);
  XWPSParamList * plist = (XWPSParamList *)&list;
  PSPassword pass;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
		return code;
  list.writing = false;
  
  code = dict_stack.getSystemDict()->dictReadPassword(this, "SystemParamsPassword", &pass);
  if (code < 0)
		return code;
    
  code = plist->checkPassword(this, &pass);
  if (code != 0) 
  {
		if (code > 0)
	    code = (XWPSError::InvalidAccess);
		goto out;
  }
  code = plist->readPassword(this, "StartJobPassword", &pass);
  switch (code) 
  {
		default:	
	    goto out;
	    
		case 1:	
	    break;
	    
		case 0:
	    code = dict_stack.getSystemDict()->dictWritePassword(this, &pass, "StartJobPassword");
	    if (code < 0)
				goto out;
  }
  code = plist->readPassword(this, "SystemParamsPassword", &pass);
  switch (code) 
  {
		default:	
	    goto out;
	    
		case 1:	
	    break;
	    
		case 0:
	    code = dict_stack.getSystemDict()->dictWritePassword(this, &pass, "SystemParamsPassword");
	    if (code < 0)
				goto out;
  }
  code = setParams(plist, &system_param_set);
  
out:

  if (code < 0)
		return code;
    
  pop(1);
  return 0;
}

int XWPSContextState::zsetTrapParams()
{
	XWPSRef * op = op_stack.getCurrentTop();
	int code = op->checkType(XWPSRef::Dictionary);
	if (code < 0)
		return code;
	pop(1);
  return 0;
}

int XWPSContextState::zsetTrapZone()
{
	return (int)(XWPSError::Undefined);
}

int XWPSContextState::zsetUserParams()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = setUserParams(op);

  if (code >= 0) 
  {
		scanner_options = ztokenScannerOptions(op, scanner_options);
		pop(1);
  }
  return code;
}

int XWPSContextState::zsystemVMArray()
{
	return specificVMOp(&XWPSContextState::zarray, PS_AVM_SYSTEM);
}

int XWPSContextState::zsystemVMCheck()
{
	XWPSRef * op = op_stack.getCurrentTop();

  op->makeBool((op->space() == PS_AVM_SYSTEM ? true : false));
  return 0;
}

int XWPSContextState::zsystemVMDict()
{
	return specificVMOp(&XWPSContextState::zdict, PS_AVM_SYSTEM);
}

int XWPSContextState::zsystemVMPackedArray()
{
	return specificVMOp(&XWPSContextState::zpackedArray, PS_AVM_SYSTEM);
}

int XWPSContextState::zsystemVMString()
{
	return specificVMOp(&XWPSContextState::zstring, PS_AVM_SYSTEM);
}

int XWPSContextState::zusertimeContext()
{
	XWPSRef * op = op_stack.getCurrentTop();
//	long utime = contextUserTime();
	int code = push(&op, 1);
	if (code < 0)
		return code;
	op->makeInt(0);
	return 0;
}

int XWPSContextState::zvmreclaim()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if (op->value.intval == 1 || op->value.intval == 2) 
		return (int)(XWPSError::VMReclaim);
  
  return (int)(XWPSError::RangeCheck);
}

int XWPSContextState::zvmstatus()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = push(&op, 3);
  if (code < 0)
		return code;
  op[-2].makeInt(iimemoryLocal()->save_level);
  op[-1].makeInt(0);
  op->makeInt(1024*1024*16);
  return 0;
}

int XWPSContextState::zwait()
{
	XWPSRef * op = op_stack.getCurrentTop();
	lockRelease(op - 1);
	XWPSRef * esp = exec_stack.incCurrentTop(1);
	esp->makeOper(0, &XWPSContextState::awaitLock);
  return PS_O_RESCHEDULE;
}
