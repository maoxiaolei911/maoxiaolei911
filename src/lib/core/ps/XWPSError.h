/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSERROR_H
#define XWPSERROR_H

#include <QObject>
#include "XWGlobal.h"

class XWPSError : public QObject
{
	Q_OBJECT 
	
public:
	enum ErrorCode
	{
		NeedInput = -106,
		VMReclaim = -105,
		ExecStackUnderflow = -104,
		RemapColor = -103,
		InterpreterExit = -102,
		Quit = -101,
		Fatal = -100,
		HitDetected = -99,
		InvalidId = -30,
		Unregistered = -29,
		UndefinedResource = -28,
		InvalidContext = -27,
		ConfigurationError = -26,
		VMError = -25,
		UnmatchedMark = -24,
		UndefinedResult = -23,
		UndefinedFileName = -22,
		Undefined = -21,
		TypeCheck = -20,
		Timeout = -19,
		SyntaxError = -18,
		StackUnderflow = -17,
		StackOverflow = -16,
		RangeCheck = -15,
		NoCurrentPoint = -14,		
		LimitCheck = -13,		
		IOError = -12,
		InvalidRestore = -11,
		InvalidFont = -10,
		InvalidFileAccess = -9,
		InvalidExit = -8,
		InvalidAccess = -7,
		Interrupt = -6,
		ExecstackOverFlow = -5,
		DictStackUnderflow = -4,
		DictStackOverflow = -3,
		DictFull = -2,
		Unknown = -1,
		NoError = 0
	};
	
	XWPSError(QObject * parent = 0);
	~XWPSError() {}
	
	static int countOfName();
	
	static const char * getName(int i);
	
	bool isInterrupt(ErrorCode ecode) {return ((ecode) == Interrupt || (ecode) == Timeout);}
	
	void showErrMsg(int c, char * m = 0, int len = 0);
	
private:
	ErrorCode code;
};

#endif //XWPSERROR_H
