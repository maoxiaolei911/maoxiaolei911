/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QTextCodec>
#include "XWApplication.h"
#include "XWPSError.h"

const char *const ps_error_names[] =
{
	//level1
	"unknownerror", 
	"dictfull", 
	"dictstackoverflow", 
	"dictstackunderflow",
	"execstackoverflow", 
	"interrupt", 
	"invalidaccess", 
	"invalidexit",
	"invalidfileaccess", 
	"invalidfont", 
	"invalidrestore", 
	"ioerror",
	"limitcheck", 
	"nocurrentpoint", 
	"rangecheck", 
	"stackoverflow",
	"stackunderflow", 
	"syntaxerror", 
	"timeout", 
	"typecheck", 
	"undefined",
	"undefinedfilename", 
	"undefinedresult", 
	"unmatchedmark", 
	"VMerror",
	//level2
	"configurationerror", 
	"invalidcontext", 
	"undefinedresource",
	"unregistered", 
	"invalidid"
};

XWPSError::XWPSError(QObject * parent)
	:QObject(parent)
{
	code = NoError;
}

int XWPSError::countOfName()
{
	return sizeof(ps_error_names) / sizeof(ps_error_names[0]);
}

const char * XWPSError::getName(int i)
{
	return ps_error_names[i];
}

void XWPSError::showErrMsg(int c, char * m, int len)
{
	QString estr;
	switch (c)
	{
		case NeedInput:
			estr = tr("need more input.");
			break;
			
		case VMReclaim:
			estr = tr("interpreter reclain.");
			break;
			
		case ExecStackUnderflow:
			estr = tr("execution stack underflow.");
			break;
			
		case RemapColor:
			estr = tr("remap color.");
			break;
			
		case InterpreterExit:
			estr = tr("interpreter exit.");
			break;
			
		case Quit:
			estr = tr("quit.");
			break;
			
		case Fatal:
			estr = tr("fatal error occur.");
			break;
			
		case HitDetected:
			estr = tr("hit detected.");
			break;
			
		case InvalidId:
			estr = tr("invalid id.");
			break;
			
		case Unregistered:
			estr = tr("unregistered.");
			break;
			
		case UndefinedResource:
			estr = tr("undefined resource.");
			break;
			
		case InvalidContext:
			estr = tr("invalid context.");
			break;
			
		case ConfigurationError:
			estr = tr("configuration error.");
			break;
			
		case VMError:
			estr = tr("memory error.");
			break;
			
		case UnmatchedMark:
			estr = tr("unmatched mark.");
			break;
			
		case UndefinedResult:
			estr = tr("undefined result.");
			break;
			
		case UndefinedFileName:
			estr = tr("undefined file name.");
			break;
			
		case Undefined:
			estr = tr("undefined.");
			break;
			
		case TypeCheck:
			estr = tr("type error.");
			break;
			
		case Timeout:
			estr = tr("timeout.");
			break;
			
		case SyntaxError:
			estr = tr("syntax error.");
			break;
			
		case StackUnderflow:
			estr = tr("stack underflow.");
			break;
			
		case StackOverflow:
			estr = tr("stack overflow.");
			break;
			
		case RangeCheck:
			estr = tr("not in range.");
			break;
			
		case NoCurrentPoint:
			estr = tr("no current point.");
			break;
			
		case LimitCheck:
			estr = tr("exceeding limit.");
			break;
			
		case IOError:
			estr = tr("io error.");
			break;
			
		case InvalidRestore:
			estr = tr("invalid restore.");
			break;
			
		case InvalidFont:
			estr = tr("invalid font.");
			break;
			
		case InvalidFileAccess:
			estr = tr("invalid file access.");
			break;
			
		case InvalidExit:
			estr = tr("invalid exit.");
			break;
			
		case InvalidAccess:
			estr = tr("invalid access.");
			break;
			
		case Interrupt:
			estr = tr("interrupt.");
			break;
			
		case ExecstackOverFlow:
			estr = tr("execution stack overflow.");
			break;
			
		case DictStackUnderflow:
			estr = tr("dictionary stack underflow.");
			break;
			
		case DictStackOverflow:
			estr = tr("dictionary stack overflow.");
			break;
			
		case DictFull:
			estr = tr("dictionary full.");
			break;
			
		case Unknown:
			estr = tr("unknown error.");
			break;
			
		default:
			break;
	}
	
	if (!estr.isEmpty())
	{
		QString msg = estr;
		if (m && len > 0)
		{
			QTextCodec *codec = QTextCodec::codecForLocale();
			QString t = codec->toUnicode((const char *)m, len, 0);
			msg += t;
		}
		xwApp->error(msg);
	}
}
