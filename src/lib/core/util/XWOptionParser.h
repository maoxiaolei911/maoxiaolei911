/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWUNICODE_H
#define XWUNICODE_H

#include <QObject>
#include <QString>

#include "XWGlobal.h"

struct XW_UTIL_EXPORT XWOption
{
	char * name;
	int has_arg;
    int *flag;
    int val;
};

class XW_UTIL_EXPORT XWOptionParser : public QObject
{
	Q_OBJECT
	
public:
	XWOptionParser(int & argcA, 
	               char ** argvA, 
	               XWOption * optionA,
	               QObject * parent = 0);
	~XWOptionParser();
	
	bool getArgv(QString & ret);
	
	int    getOpt(const char *optstring);
	char * getOptArg() {return optArg;}
	int    getOptErr() {return optErr;}
	int    getOptInd() {return optInd;}
	int    getOptLong(const char *options, int *opt_index);
	int    getOptLongOnly(const char *options, int *opt_index);
		
	bool isArgument(int option_index, const char * a);
	
	double  optArgToDouble();
	int     optArgToInt();
	QString optArgToQString();
	
	void setErr(int e) {optErr = e;}
	
private:
	void   exchange();
	
	const char * getOptInitialize(const char *optstring);
	int    getOptInternal(const char *optstring, 
	                      int *longind,
	                      int long_only);
	
	char * index(const char * str, int chr);
	
	bool isNonOption() {return (argv[optInd][0] != '-' || argv[optInd][1] == '\0');}
	
private:
	int argc;
	char ** argv;
	XWOption * longOpts;
	
	char * optArg;
	int    optInd;
	int    initialized;
	char * nextChar;
	int    optErr;
	int    optOpt;
	char * posixlyCorrect;
	int    firstNonOpt;
	int    lastNonOpt;
};

#endif // XWUNICODE_H
