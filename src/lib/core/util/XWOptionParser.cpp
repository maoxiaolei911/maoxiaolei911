/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QByteArray>
#include <QTextCodec>
#include <QString>

#include "XWApplication.h"
#include "XWOptionParser.h"

static enum
{
  REQUIRE_ORDER, PERMUTE, RETURN_IN_ORDER
} ordering;

XWOptionParser::XWOptionParser(int & argcA, 
	                           char ** argvA, 
	                           XWOption * optionA,
	                           QObject * parent)
	:QObject(parent),
	 argc(argcA),
	 argv(argvA),
	 longOpts(optionA),
	 optArg(0),
	 optInd(1),
	 initialized(0),
	 nextChar(0),
	 optErr(1),
	 optOpt('?'),
	 posixlyCorrect(0),
	 firstNonOpt(0),
	 lastNonOpt(0)
{
}

XWOptionParser::~XWOptionParser()
{
	if (posixlyCorrect)
		delete [] posixlyCorrect;
}

bool XWOptionParser::getArgv(QString & ret)
{
	bool r = optInd < argc;
	if (r)
	{
		QTextCodec * codec = QTextCodec::codecForLocale();
		ret = codec->toUnicode(argv[optInd]);
		optInd++;
	}
	
	return r;
}

int XWOptionParser::getOpt(const char *optstring)
{
	return getOptInternal(optstring, 0, 0);
}

int XWOptionParser::getOptLong(const char *options, int *opt_index)
{
	return getOptInternal(options, opt_index, 0);
}

int XWOptionParser::getOptLongOnly(const char *options, int *opt_index)
{
	return getOptInternal(options, opt_index, 1);
}

bool XWOptionParser::isArgument(int option_index, const char * a)
{
	return (0 == qstrcmp(longOpts[option_index].name, a));
}

double  XWOptionParser::optArgToDouble()
{
	if (!optArg)
		return 0.0;
		
	return atof(optArg);
}

int XWOptionParser::optArgToInt()
{
	if (!optArg)
		return 0;
		
	return atoi(optArg);
}

QString XWOptionParser::optArgToQString()
{
	if (!optArg)
		return QString();
		
	QTextCodec * codec = QTextCodec::codecForLocale();
	QString ret = codec->toUnicode(optArg);
	return ret;
}

void XWOptionParser::exchange()
{
	int bottom = firstNonOpt;
  	int middle = lastNonOpt;
  	int top = optInd;
  	char *tem = 0;
  	
  	while (top > middle && middle > bottom)
  	{
  		if (top - middle > middle - bottom)
  		{
  			int len = middle - bottom;
  			for (int i = 0; i < len; i++)
  			{
  				tem = argv[bottom + i];
	      		argv[bottom + i] = argv[top - (middle - bottom) + i];
	      		argv[top - (middle - bottom) + i] = tem;
  			}
  			
  			top -= len;
  		}
  		else
  		{
  			int len = top - middle;
  			for (int i = 0; i < len; i++)
  			{
  				tem = argv[bottom + i];
	      		argv[bottom + i] = argv[middle + i];
	      		argv[middle + i] = tem;
  			}
  			
  			bottom += len;
  		}
  	}
  	
  	firstNonOpt += (optInd - lastNonOpt);
  	lastNonOpt = optInd;
}

const char * XWOptionParser::getOptInitialize(const char *optstring)
{
	firstNonOpt = lastNonOpt = optInd = 1;
  	nextChar = 0;
  	
  	if (posixlyCorrect)
  	{
  		delete [] posixlyCorrect;
  		posixlyCorrect = 0;
  	}
  	
  	QByteArray ba = qgetenv("POSIXLY_CORRECT");
  	if (ba.length() > 0)
  	{
  		posixlyCorrect = qstrdup(ba.constData());
  	}
  	
  	if (optstring[0] == '-')
    {
    	ordering = RETURN_IN_ORDER;
      	++optstring;
    }
  	else if (optstring[0] == '+')
    {
    	ordering = REQUIRE_ORDER;
      	++optstring;
    }
  	else if (posixlyCorrect != 0)
    	ordering = REQUIRE_ORDER;
  	else
    	ordering = PERMUTE;
    	
    return optstring;
}

int XWOptionParser::getOptInternal(const char *optstring, 
	                               int *longind,
	                               int long_only)
{
	optArg = 0;
	if (!initialized || optInd == 0)
	{
		optstring = getOptInitialize(optstring);
		optInd = 1;
      	initialized = 1;
	}
	
	if (nextChar == 0 || *nextChar == '\0')
	{
		if (lastNonOpt > optInd)
			lastNonOpt = optInd;
			
      	if (firstNonOpt > optInd)
			firstNonOpt = optInd;
			
		if (ordering == PERMUTE)
		{
			if (firstNonOpt != lastNonOpt && lastNonOpt != optInd)
	    		exchange();
	  		else if (lastNonOpt != optInd)
	    		firstNonOpt = optInd;
	    		
	    	while (optInd < argc && isNonOption())
	    		optInd++;
	  		lastNonOpt = optInd;
		}
		
		if (optInd != argc && !strcmp(argv[optInd], "--"))
		{
	  		optInd++;

	  		if (firstNonOpt != lastNonOpt && lastNonOpt != optInd)
	    		exchange();
	  		else if (firstNonOpt == lastNonOpt)
	    		firstNonOpt = optInd;
	  		lastNonOpt = argc;

	  		optInd = argc;
		}
		
		if (optInd == argc)
		{
			if (firstNonOpt != lastNonOpt)
	    		optInd = firstNonOpt;
	  		return -1;
		}
		
		if (isNonOption())
		{
	  		if (ordering == REQUIRE_ORDER)
	    		return -1;
	    		
	  		optArg = argv[optInd++];
	  		return 1;
		}
		
		nextChar = (argv[optInd] + 1 + (longOpts != 0 && argv[optInd][1] == '-'));
	}
	
	if (longOpts != 0 && (argv[optInd][1] == '-' || 
		(long_only && (argv[optInd][2] || 
		!index(optstring, argv[optInd][1])))))
	{
		char *nameend = nextChar;
		while (*nameend && *nameend != '=')
			nameend++;
			
		XWOption * p = longOpts;
		XWOption * pfound = 0;
		int exact = 0;
      	int ambig = 0;
      	int indfound = -1;
		int option_index = 0;
		while (p->name)
		{
			if (!strncmp (p->name, nextChar, nameend - nextChar))
			{
				if ((unsigned int)(nameend - nextChar) == (unsigned int)strlen(p->name))
				{
					pfound = p;
					indfound = option_index;
					exact = 1;
					break;
				}
				else if (pfound == 0)
				{
					pfound = p;
					indfound = option_index;
				}
				else
					ambig = 1;
			}
			
			p++; 
			option_index++;
		}
		
		if (ambig && !exact)
		{
	  		if (optErr)
	  		{
	  			QTextCodec * codec = QTextCodec::codecForLocale();
	  			QString av0 = codec->toUnicode(argv[0]);
	  			QString av1 = codec->toUnicode(argv[optInd]);
	  			QString msg = QString(tr("%1: option '%2' is ambiguous\n")).arg(av0).arg(av1);
	  			xwApp->error(msg);
	  		}
	  			
	  		nextChar += strlen(nextChar);
	  		optInd++;
	  		optOpt = 0;
	  		return '?';
		}
		
		if (pfound != 0)
		{
			option_index = indfound;
	  		optInd++;
	  		if (*nameend)
	  		{
	  			if (pfound->has_arg)
					optArg = nameend + 1;
				else
				{
					if (optErr)
					{
						QTextCodec * codec = QTextCodec::codecForLocale();
						QString av0 = codec->toUnicode(argv[0]);
						QString av1;
						if (argv[optInd - 1][1] == '-')
							av1 = "--";
						else
						{
							QChar c(argv[optInd - 1][0]);
							av1.append(c);
						}
						
						av1 += codec->toUnicode(pfound->name);
						QString msg = QString(tr("%1: option '%2' doesn't allow an argument\n"))
							                    .arg(av0).arg(av1);
						xwApp->error(msg);
					}
					
					nextChar += strlen(nextChar);

		  			optOpt = pfound->val;
		  			return '?';
				}
	  		}
	  		else if (pfound->has_arg == 1)
	  		{
	  			if (optInd < argc)
					optArg = argv[optInd++];
				else
				{
					if (optErr)
					{
						QTextCodec * codec = QTextCodec::codecForLocale();
						QString av0 = codec->toUnicode(argv[0]);
						QString av1 = codec->toUnicode(argv[optInd - 1]);
						QString msg = QString(tr("%1: option '%2' requires an argument\n"))
							                    .arg(av0).arg(av1);
						xwApp->error(msg);
					}
					
					nextChar += strlen(nextChar);
		  			optOpt = pfound->val;
		  			return optstring[0] == ':' ? ':' : '?';
				}
	  		}
	  		
	  		nextChar += strlen(nextChar);
	  		
	  		if (longind != 0)
	    		*longind = option_index;
	  		if (pfound->flag)
	    	{
	      		*(pfound->flag) = pfound->val;
	      		return 0;
	    	}
	  		return pfound->val;
		}
		
		if (!long_only || 
			argv[optInd][1] == '-' || 
			index(optstring, *nextChar) == 0)
		{
			if (optErr)
			{
				QTextCodec * codec = QTextCodec::codecForLocale();
				QString av0 = codec->toUnicode(argv[0]);
				QString av1;
				if (argv[optInd][1] == '-')
					av1 = "--";
				else
				{
					QChar c(argv[optInd][0]);
					av1.append(c);
				}
				
				av1 += codec->toUnicode(nextChar);
				QString msg = QString(tr("%1: unrecognized option '%2'\n"))
					                    .arg(av0).arg(av1);
				xwApp->error(msg);
			}
			
			nextChar = (char *)"";
	  		optInd++;
	  		optOpt = 0;
	  		return '?';
		}
	}
	
	{
		char c = *nextChar++;
		QChar ch(c);
    	char *temp = index(optstring, c);
    	if (*nextChar == '\0')
      		++optInd;

    	if (temp == 0 || c == ':')
    	{
    		if (optErr)
    		{
    			QTextCodec * codec = QTextCodec::codecForLocale();
				QString av0 = codec->toUnicode(argv[0]);
				QString av1(ch);
				QString msg = QString(tr("%1: illegal option -- %2\n"))
					                    .arg(av0).arg(av1);
				xwApp->error(msg);
    		}
    		
    		optOpt = c;
			return '?';
    	}
    	
    	if (temp[0] == 'W' && temp[1] == ';')
    	{
    		char *nameend = 0;
			XWOption *p = 0;
			XWOption *pfound = 0;
			int exact = 0;
			int ambig = 0;
			int indfound = 0;
			int option_index = 0;
			if (*nextChar != '\0')
			{
				optArg = nextChar;
				optInd++;
			}
			else if (optInd == argc)
			{
				if (optErr)
				{
					QTextCodec * codec = QTextCodec::codecForLocale();
					QString av0 = codec->toUnicode(argv[0]);
					QString av1(ch);
					QString msg = QString(tr("%1: option requires an argument -- %2\n"))
					                    .arg(av0).arg(av1);
					xwApp->error(msg);
				}
				
				optOpt = c;
	    		if (optstring[0] == ':')
	      			c = ':';
	    		else
	      			c = '?';
	    		return c;
			}
			else
				optArg = argv[optInd++];
				
			nextChar = nameend = optArg;
			while (*nameend && *nameend != '=')
				nameend++;
				
			p = longOpts; 
			option_index = 0;
			while (p->name)
			{
				if (!strncmp (p->name, nextChar, nameend - nextChar))
				{
					if ((unsigned int) (nameend - nextChar) == strlen(p->name))
					{
						pfound = p;
		  				indfound = option_index;
		  				exact = 1;
		  				break;
					}
					else if (pfound == 0)
					{
						pfound = p;
		  				indfound = option_index;
					}
					else
						ambig = 1;
				}
				p++; 
				option_index++;
			}
			
			if (ambig && !exact)
			{
				if (optErr)
				{
					QTextCodec * codec = QTextCodec::codecForLocale();
					QString av0 = codec->toUnicode(argv[0]);
					QString av1 = codec->toUnicode(argv[optInd]);
					QString msg = QString(tr("%1: option requires an argument -- %2\n"))
					                    .arg(av0).arg(av1);
					xwApp->error(msg);
				}
				
				nextChar += strlen(nextChar);
	    		optInd++;
	    		return '?';
			}
			
			if (pfound != 0)
			{
				option_index = indfound;
	    		if (*nameend)
	    		{
	    			if (pfound->has_arg)
		  				optArg = nameend + 1;
					else
					{
						if (optErr)
						{
							QTextCodec * codec = QTextCodec::codecForLocale();
							QString av0 = codec->toUnicode(argv[0]);
							QString av1 = codec->toUnicode(pfound->name);
							QString msg = QString(tr("%1: option '-W %2' doesn't allow an argument\n"))
					                    			.arg(av0).arg(av1);
							xwApp->error(msg);
						}
						
						nextChar += strlen(nextChar);
		    			return '?';
					}
	    		}
	    		else if (pfound->has_arg == 1)
	    		{
	    			if (optInd < argc)
		  				optArg = argv[optInd++];
		  			else
		  			{
		  				if (optErr)
		  				{
		  					QTextCodec * codec = QTextCodec::codecForLocale();
							QString av0 = codec->toUnicode(argv[0]);
							QString av1 = codec->toUnicode(argv[optInd - 1]);
							QString msg = QString(tr("%1: option '%2' requires an argument\n"))
					                    			.arg(av0).arg(av1);
							xwApp->error(msg);
		  				}
		  				
		  				nextChar += strlen(nextChar);
		    			return optstring[0] == ':' ? ':' : '?';
		  			}
	    		}
	    		
	    		nextChar += strlen(nextChar);
	    		if (longind != 0)
	    			*longind = option_index;	    		
	    		if (pfound->flag)
	      		{
					*(pfound->flag) = pfound->val;
					return 0;
	      		}
	    		return pfound->val;
			}
			
			nextChar = 0;
	  		return 'W';
    	}
    	
    	if (temp[1] == ':')
    	{
    		if (temp[2] == ':')
    		{
    			if (*nextChar != '\0')
	      		{
					optArg = nextChar;
					optInd++;
	      		}
	    		else
	      			optArg = 0;
	    		nextChar = 0;
    		}
    		else
    		{
    			if (*nextChar != '\0')
    			{
    				optArg = nextChar;
    				optInd++;
    			}
    			else if (optInd == argc)
    			{
    				if (optErr)
    				{
    					QTextCodec * codec = QTextCodec::codecForLocale();
						QString av0 = codec->toUnicode(argv[0]);
						QString av1(ch);
						QString msg = QString(tr("%1: option requires an argument -- %2\n"))
					                   			.arg(av0).arg(av1);
						xwApp->error(msg);
    				}
    				
    				optOpt = c;
    				if (optstring[0] == ':')
		  				c = ':';
					else
		  				c = '?';
    			}
    			else
    				optArg = argv[optInd++];
	    		nextChar = 0;
    		}
    	}
    	
    	return c;
	}
}

char * XWOptionParser::index(const char * str, int chr)
{
	while (*str)
    {
    	if (*str == chr)
			return (char *)str;
      	str++;
    }
    
  	return 0;
}

