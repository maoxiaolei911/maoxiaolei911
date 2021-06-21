/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <QTextStream>
#include "XWUtil.h"
#include "XWFileName.h"
#include "XWTexSea.h"
#include "XWMakeIndex.h"

#define CHECK_LENGTH()	if (i > len_field) goto FIELD_OVERFLOW
	
static int     page_offset[PAGETYPE_MAX] = {
0,
ROMAN_LOWER_OFFSET,
ROMAN_LOWER_OFFSET + ARABIC_OFFSET,
ROMAN_LOWER_OFFSET + ARABIC_OFFSET + ALPHA_LOWER_OFFSET,
ROMAN_LOWER_OFFSET + ARABIC_OFFSET + ALPHA_LOWER_OFFSET + ROMAN_UPPER_OFFSET
};
	
XWMakeIndex::XWMakeIndex(QCoreApplication * app)
	:XWApplication(app)
{
	installTranslator("xw_makeindex_");
	
	letter_ordering = 0;
	compress_blanks = 0;
	merge_page = 1;
	init_page = 0;
	even_odd = -1;
	verbose = 1;
	german_sort = 0;
	thai_sort = 0;
	locale_sort = 0;
	fn_no = -1;
	idx_dot = 1;
	idx_tt = 0;
	idx_et = 0;
	idx_gt = 0;
	idx_dc = 0;
	
	idx_key = 0;
	log_fp = 0;
	sty_fp = 0;
	idx_fp = 0;
	ind_fp = 0;
	ilg_fp = 0;
	
	pgm_fn = 0;
	idx_fn = 0;
	ind_fn = 0;
	ilg_fn = 0;
	
	need_version = 1;
	
	head = 0;
	tail = 0;
	
	curr = 0;
	prev = 0;
	begin = 0;
	the_end = 0;
	range_ptr = 0;
	level = 0;
	prev_level = 0;
	encap = 0;
	prev_encap = 0;
	in_range = 0;
	encap_range = 0;
	ind_lc = 0;
	ind_ec = 0;
	ind_indent = 0;
	
	first_entry = 1;
	
	strcpy(idx_keyword, IDX_KEYWORD);
	idx_aopen = IDX_AOPEN;
	idx_aclose = IDX_ACLOSE;
	idx_level = IDX_LEVEL;
	idx_ropen = IDX_ROPEN;
	idx_rclose = IDX_RCLOSE;
	idx_quote = IDX_QUOTE;
	idx_actual = IDX_ACTUAL;
	idx_encap = IDX_ENCAP;
	idx_escape = IDX_ESCAPE;

	strcpy(preamble, PREAMBLE_DEF);
	strcpy(postamble, POSTAMBLE_DEF);
	prelen = PREAMBLE_LEN;
	postlen = POSTAMBLE_LEN;

	strcpy(setpage_open, SETPAGEOPEN_DEF);
	strcpy(setpage_close, SETPAGECLOSE_DEF);
	setpagelen = SETPAGE_LEN;

	strcpy(group_skip, GROUPSKIP_DEF);
	skiplen = GROUPSKIP_LEN;

	headings_flag = HEADINGSFLAG_DEF;
	strcpy(heading_pre, HEADINGPRE_DEF);
	strcpy(heading_suf, HEADINGSUF_DEF);
	headprelen = HEADINGPRE_LEN;
	headsuflen = HEADINGSUF_LEN;

	strcpy(symhead_pos, SYMHEADPOS_DEF);
	strcpy(symhead_neg, SYMHEADNEG_DEF);

	strcpy(numhead_pos, NUMHEADPOS_DEF);
	strcpy(numhead_neg, NUMHEADNEG_DEF);

	strcpy(item_r[0], ITEM0_DEF);
	strcpy(item_r[1], ITEM1_DEF);
	strcpy(item_r[2], ITEM2_DEF);
	
	strcpy(item_u[0], "");
	strcpy(item_u[1], ITEM1_DEF);
	strcpy(item_u[2], ITEM2_DEF);
	
	strcpy(item_x[0], "");
	strcpy(item_x[1], ITEM1_DEF);
	strcpy(item_x[2], ITEM2_DEF);

	ilen_r[0] = ITEM_LEN;
	ilen_r[1] = ITEM_LEN;
	ilen_r[2] = ITEM_LEN;
	
	ilen_u[0] = ITEM_LEN;
	ilen_u[1] = ITEM_LEN;
	ilen_u[2] = ITEM_LEN;
	
	ilen_x[0] = ITEM_LEN;
	ilen_x[1] = ITEM_LEN;
	ilen_x[2] = ITEM_LEN;
	
	strcpy(delim_p[0], DELIM_DEF);
	strcpy(delim_p[1], DELIM_DEF);
	strcpy(delim_p[2], DELIM_DEF);
	
	strcpy(delim_n, DELIM_DEF); /* page number separator */
	strcpy(delim_r, DELIMR_DEF);/* page range designator */
	strcpy(delim_t, DELIMT_DEF);/* page list terminating delimiter */

	strcpy(suffix_2p, "");      /* suffix for two page ranges */
	strcpy(suffix_3p, "");      /* suffix for three page ranges */
	strcpy(suffix_mp, "");      /* suffix for multiple page ranges */

	strcpy(encap_p, ENCAP0_DEF);/* encapsulator prefix */
	strcpy(encap_i, ENCAP1_DEF);/* encapsulator infix */
	strcpy(encap_s, ENCAP2_DEF);/* encapsulator postfix */

	linemax = LINE_MAX;

	indent_length = INDENTLEN_DEF;
	strcpy(indent_space, INDENTSPC_DEF);

	strcpy(page_comp, COMPOSITOR_DEF);
	strcpy(page_prec, PRECEDENCE_DEF);
	
	idx_lc = 0;
	idx_tc = 0;
	idx_ec = 0;
	idx_gc = 0;
	
	sty_lc = 0;                 /* line count */
	sty_tc = 0;                 /* total count */
	sty_ec = 0;
	
	qsz = 0;
	thresh = 0;
	mthresh = 0;
//	qcmp = 0;
}

XWMakeIndex::~XWMakeIndex()
{
}

QString XWMakeIndex::getProductID()
{
	return "xwmakeindex";
}

QString XWMakeIndex::getProductName()
{
	return tr("Xiuwen MakeIndex");
}

const char * XWMakeIndex::getProductName8()
{
	return "Xiuwen MakeIndex";
}

QString  XWMakeIndex::getVersion()
{
	return "0.9";
}

const char * XWMakeIndex::getVersion8()
{
	return "0.9";
}

void XWMakeIndex::start(int & argc, char**argv)
{
	int     use_stdin = 0;
  int     sty_given = 0;
  int     ind_given = 0;
  int     ilg_given = 0;
  int     log_given = 0;
  
  outputPath = getEnv("OUTPUT_PATH");
  
  char   *fns[ARRAY_MAX];
  char   *ap;
  QTextStream term(stderr);
  QTextStream log(log_fp);
  while (--argc > 0)
  {
  	if (**++argv == '-')
  	{
  		if (*(*argv + 1) == NUL)
  			break;
  			
  		for (ap = ++*argv; *ap != NUL; ap++)
  		{
  			switch (*ap)
  			{
  				case 'i':
  					use_stdin = 1;
		    		break;
		    	
		    	case 'l':
		    		letter_ordering = 1;
		    		break;
		    	
		    	case 'r':
		    		merge_page = 0;
		    		break;
		    	
		    	case 'q':
		    		verbose = 0;
		    		break;
		    	
		    	case 'c':
		    		compress_blanks = 1;
		    		break;
		    	
		    	case 's':
		    		argc--;
		    		if (argc <= 0)
		    		{
		    			term << tr("Expected -s <stylefile>\n");
		    			cleanup();
		    			exit(1);
		    		}
		    		openSty(*++argv);
		    		sty_given = 1;
		    		break;
		    	
		    	case 'o':
		    		argc--;
		    		if (argc <= 0)
		    		{
		    			term << tr("Expected -o <ind>\n");
		    			cleanup();
		    			exit(1);
		    		}
		    		ind_fn = *++argv;
		    		ind_given = 1;
		    		break;
		    	
		    	case 't':
		    		argc--;
		    		if (argc <= 0)
		    		{
		    			term << tr("Expected -t <logfile>\n");
		    			cleanup();
		    			exit(1);
		    		}
		    		ilg_fn = *++argv;
		    		ilg_given = 1;
		    		break;
		    	
		    	case 'p':
		    		argc--;
		    		if (argc <= 0)
		    		{
		    			term << tr("Expected -p <num>\n");
		    			cleanup();
		    			exit(1);
		    		}
		    		strcpy(pageno, *++argv);
		    		init_page = 1;
		    		if (0 == strcmp(pageno, EVEN))
		    		{
		    			log_given = 1;
							even_odd = 2;
		    		}
		    		else if (0 == strcmp(pageno, ODD))
		    		{
		    			log_given = 1;
							even_odd = 1;
		    		}
		    		else if (0 == strcmp(pageno, ANY))
		    		{
		    			log_given = 1;
							even_odd = 0;
		    		}
		    		break;
		    	
		    	case 'g':
		    		german_sort = 1;
		    		break;
		    	
		    	case 'L':
		    		locale_sort = 1;
		    		break;
		    	
		    	case 'T':
		    		thai_sort = locale_sort = 1;
		    		break;
		    	
		    	default:
		    		term << tr("Unknown option -") << *ap;
		    		cleanup();
		    		exit(1);
		    		break;
		    }
  		}
  	}
  	else
  	{
  		if (fn_no < ARRAY_MAX)
  		{
  			checkIdx(*argv, 0);
				fns[++fn_no] = *argv;
  		}
  		else
  		{
  			term << tr("Too many input files (max 1024).\n");
  			cleanup();
  			exit(1);
  		}
  	}
  }
  
  if (fn_no == 0 && !sty_given)
  {
  	char tmp[STRING_MAX + 5];
  	sprintf (tmp, "%s%s", base, INDEX_STY);
  	openSty (tmp);
		sty_given = 1;
  }
  
  processIdx(fns, use_stdin, sty_given, ind_given, ilg_given, log_given);
  idx_gt = idx_tt - idx_et;
  
  if (fn_no > 0)
  {
  	QString str = QString(tr("Overall %1 files read (%2 entries accepted, %3 rejected).\n"))
  		                      .arg(fn_no+1).arg(idx_gt).arg(idx_et);
  	if (verbose)
  		term << str;
  	log << str;
  }
  
  QTextCodec * codec = QTextCodec::codecForLocale();
  QString filename = codec->toUnicode(ind_fn);
  if (idx_gt > 0)
  {
  	prepareIdx();
		sortIdx();
		genInd();
		QString str = QString(tr("Output written in %1.\n")).arg(filename);
		term << str;
		log << str;
  }
  else
  {
  	QString str = QString(tr("Nothing written in %1.\n")).arg(filename);
		term << str;
		log << str;
  }
  
  filename = codec->toUnicode(ilg_fn);
  filename = QString(tr("Transcript written in %1.\n")).arg(filename);

  cleanup();
}

void XWMakeIndex::cleanup()
{
	if (log_fp)
	{
		log_fp->close();
		delete log_fp;
		log_fp = 0;
	}
	
	if (sty_fp)
	{
		sty_fp->close();
		delete sty_fp;
		sty_fp = 0;
	}
	
	if (idx_fp)
	{
		idx_fp->close();
		delete idx_fp;
		idx_fp = 0;
	}
	
	if (ind_fp)
	{
		ind_fp->close();
  	delete ind_fp;
  	ind_fp = 0;
  }
  
  if (ilg_fp)
  {
  	ilg_fp->close();
  	delete ilg_fp;
  	ilg_fp = 0;
  }
  
  KNODE * tnode = head;
  while (head)
  {
  	tnode = head->next;
  	free(head);
  	head = tnode;
  }
  
  if (idx_key)
  	free(idx_key);
  	
  XWApplication::cleanup();
}

void XWMakeIndex::checkAll(char *fn, 
	                         int ind_given, 
	                         int ilg_given, 
	                         int log_given)
{
	checkIdx(fn, 1);
	
	QTextStream term(stderr);
	QTextCodec * codec = QTextCodec::codecForLocale();
	if (!ind_given) 
	{
		sprintf(ind, "%s%s", base, INDEX_IND);
		ind_fn = ind;
  }
  
  if (!ind_fp)
  {
  	QString indname = codec->toUnicode(ind_fn);
  	if (!outputPath.isEmpty())
			indname = QString("%1/%2").arg(outputPath).arg(indname);
  	ind_fp = new QFile(indname);
  	if (!ind_fp->open(QIODevice::WriteOnly))
  	{
  		delete ind_fp;
  		ind_fp = 0;
  		term << tr("Can't create output index file ") << indname << ".\n";
  		cleanup();
  		exit(1);
  	}
  }
  
  if (!ilg_given)
  {
  	sprintf(ilg, "%s%s", base, INDEX_ILG);
		ilg_fn = ilg;
  }  
  
  if (!ilg_fp)
  {
  	QString ilgname = codec->toUnicode(ilg_fn);
  	ilg_fp = new QFile(ilgname);
		if (!ilg_fp->open(QIODevice::WriteOnly))
		{
			delete ilg_fp;
			ilg_fp = 0;
			term << tr("Can't create transcript file ") << ilgname << ".\n";
			cleanup();
			exit(1);
		}
	}
	if (log_given)
	{
		if (!log_fp)
		{
			sprintf(log_fn, "%s%s", base, INDEX_LOG);
			QString logname = codec->toUnicode(log_fn);
  		log_fp = new QFile(logname);
  		if (!log_fp->open(QIODevice::ReadOnly))
  		{
  			delete log_fp;
  			log_fp = 0;
  			term << tr("Source log file ") << logname << tr(" not found.\n");
  			cleanup();
				exit(1);
  		}
		}
		
		findPageNo();
		log_fp->close();
	}
}

void XWMakeIndex::checkIdx(char *fn, int open_fn)
{
	char   *ptr = fn;
  char   *ext;
  int     with_ext = 0;
  int i = 0;
  QTextStream term(stderr);
  ext = strrchr(fn, EXT_DELIM);
  if ((ext != NULL) && (ext != fn) && 
  	  (strchr(ext + 1, DIR_DELIM) == NULL)
#ifdef Q_OS_WIN
  	   && (strchr(ext + 1, ALT_DIR_DELIM) == NULL)
#endif
  	   )
  {
  	with_ext = 1;
		while ((ptr != ext) && (i < STRING_MAX))
	    base[i++] = *ptr++;
  }
  else
  {
  	while ((*ptr != NUL) && (i < STRING_MAX))
	    base[i++] = *ptr++;
  }
  
  if (i < STRING_MAX)
		base[i] = NUL;
	else
	{
		term << tr("Index file name ") << base << tr(" too long (max 256).\n");
		cleanup();
		exit(1);
	}
	
	QTextCodec * codec = QTextCodec::codecForLocale();
	idx_fn = fn;
	QString idxname = codec->toUnicode(idx_fn);
	if (!outputPath.isEmpty())
		idxname = QString("%1/%2").arg(outputPath).arg(idxname);
		
	if (!with_ext && !(QFile::exists(idxname)))
	{
		idxname += ".idx";
	}
		
	if (open_fn)
	{
		if (!idx_fp)
		{
			idx_fp = new QFile(idxname);
			if (!idx_fp->open(QIODevice::ReadOnly))
			{
				delete idx_fp;
				idx_fp = 0;
				term << tr("Input index file ") << idxname << tr(" not found.\n");
				cleanup();
				exit(1);
			}
		}
	}
}

int  XWMakeIndex::checkMixsym(char *x, char *y)
{
	int     m;
    int     n;

    m = ISDIGIT(x[0]);
    n = ISDIGIT(y[0]);

    if (m && !n)
	return (1);

    if (!m && n)
	return (-1);

    return (locale_sort ? strcoll(x, y) : strcmp(x, y));
}

int  XWMakeIndex::compare(KFIELD **a, KFIELD **b)
{
	int     i;
    int     dif;

    idx_gc++;
    idx_dot = 1;
    if (idx_dc++ == 0)
    {
    	QTextStream term(stderr);
    	QTextStream log(ilg_fp);
    	if (verbose)
    		term << DOT;
    	log << DOT;
    }
    
    if (idx_dc == CMP_MAX)
    idx_dc = 0;

    for (i = 0; i < FIELD_MAX; i++) {
	/* compare the sort fields */
	if ((dif = compareOne((*a)->sf[i], (*b)->sf[i])) != 0)
	    break;

	/* compare the actual fields */
	if ((dif = compareOne((*a)->af[i], (*b)->af[i])) != 0)
	    break;
    }

    /* both key aggregates are identical, compare page numbers */
    if (i == FIELD_MAX)
	dif = comparePage(a, b);
    return (dif);
}

int  XWMakeIndex::compareOne(char *x,char *y)
{
	int     m;
    int     n;

    if ((x[0] == NUL) && (y[0] == NUL))
	return (0);

    if (x[0] == NUL)
	return (-1);

    if (y[0] == NUL)
	return (1);

    m = groupType(x);
    n = groupType(y);

    /* both pure digits */
    if ((m >= 0) && (n >= 0))
	return (m - n);

    /* x digit, y non-digit */
    if (m >= 0) {
	if (german_sort)
	    return (1);
	else
	    return ((n == -1) ? 1 : -1);
    }
    /* x non-digit, y digit */
    if (n >= 0) {
	if (german_sort)
	    return (-1);
	else
	    return ((m == -1) ? -1 : 1);
    }
    /* strings started with a symbol (including digit) */
    if ((m == SYMBOL) && (n == SYMBOL))
	return (checkMixsym(x, y));

    /* x symbol, y non-symbol */
    if (m == SYMBOL)
	return (-1);

    /* x non-symbol, y symbol */
    if (n == SYMBOL)
	return (1);

    /* strings with a leading letter, the ALPHA type */
    return (compareString((unsigned char*)x, (unsigned char*)y));
}

int  XWMakeIndex::comparePage(KFIELD **a, KFIELD **b)
{
	int     m = 0;
    short   i = 0;

    while ((i < (*a)->count) && (i < (*b)->count) && ((m = (*a)->npg[i] - (*b)->npg[i]) == 0))
    {
			i++;
    }
    if (m == 0)
    {				/* common leading page numbers match */
			if ((i == (*a)->count) && (i == (*b)->count))
			{			/* all page numbers match */
	    	/***********************************************************
	    	We have identical entries, except possibly in encap fields.
	    	The ordering is tricky here.  Consider the following input
	    	sequence of index names, encaps, and page numbers:

				foo|(	2
				foo|)	6
				foo|(	6
				foo|)	10

	    	This might legimately occur when a page range ends, and
	    	subsequently, a new range starts, on the same page.  If we
	    	just order by range_open and range_close (here, parens),
	    	then we will produce

				foo|(	2
				foo|(	6
				foo|)	6
				foo|)	10

	    	This will later generate the index entry

				foo, 2--6, \({6}, 10

	    	which is not only wrong, but has also introduced an illegal
	    	LaTeX macro, \({6}, because the merging step treated this
	    	like a \see{6} entry.

	    	The solution is to preserve the original input order, which
	    	we can do by treating range_open and range_close as equal,
	    	and then ordering by input line number.  This will then
	    	generate the correct index entry

				foo, 2--10

	    	Ordering inconsistencies from missing range open or close
	    	entries, or mixing roman and arabic page numbers, will be
	    	detected later.
	    	***********************************************************/

#define isrange(c) ( ((c) == idx_ropen) || ((c) == idx_rclose) )

	    /* Order two range values by input line number */

	    if (isrange(*(*a)->encap) && isrange(*(*b)->encap))
				m = (*a)->lc - (*b)->lc;

	    /* Handle identical encap fields; neither is a range delimiter */

	    else if (0 == strcmp((*a)->encap, (*b)->encap))
	    {
				/* If neither are yet marked duplicate, mark the second
				of them to be ignored. */
				if (((*a)->type != DUPLICATE) &&
		    		((*b)->type != DUPLICATE))
		    		(*b)->type = DUPLICATE;
				/* leave m == 0 to show equality */
	    }

	    /* Encap fields differ: only one may be a range delimiter, */
	    /* or else neither of them is.   If either of them is a range */
	    /* delimiter, order by input line number; otherwise, order */
	    /* by name. */

	    else
	    {
				if ( isrange(*(*a)->encap) || isrange(*(*b)->encap) )
		   	 	m = (*a)->lc - (*b)->lc; /* order by input line number */
				else			/* order non-range items by */
					/* their encap strings */
		    	m = compareString((unsigned char*)((*a)->encap), (unsigned char*)((*b)->encap));
	    }
		}
		else if ((i == (*a)->count) && (i < (*b)->count))
	    m = -1;
		else if ((i < (*a)->count) && (i == (*b)->count))
	    m = 1;
   }
   return (m);
}

int  XWMakeIndex::compareString(unsigned char *a, unsigned char *b)
{
	int     i = 0;
    int     j = 0;
    int     al;
    int     bl;

    if (locale_sort) 
    	return strcoll((const char *)a, (const char *)b);

    while ((a[i] != NUL) || (b[j] != NUL)) 
    {
			if (a[i] == NUL)
	   	 	return (-1);
			if (b[j] == NUL)
	    	return (1);
			if (letter_ordering) 
			{
	    	if (a[i] == SPC)
					i++;
	    	if (b[j] == SPC)
					j++;
			}
			al = TOLOWER(a[i]);
			bl = TOLOWER(b[j]);

			if (al != bl)
	    	return (al - bl);
			i++;
			j++;
    }
    if (german_sort)
		return (newStrCmp(a, b, GERMAN));
    	else
		return (strcmp((char*)a, (char*)b));
}

int  XWMakeIndex::countLfd(char *str)
{
	int     i = 0;
    int     n = 0;

    while (str[i] != NUL) 
    {
			if (str[i] == LFD)
	    	n++;
			i++;
    }
    return (n);
}

void XWMakeIndex::findPageNo()
{
	
	char c = 0;
	char p = 0;
	int i = 0;
	qint64 pos = log_fp->size();
	pos--;	
	log_fp->seek(pos);	
	if (log_fp->getChar(&p))
	{
		pos--;
		log_fp->seek(pos);
	
		do
		{
			c = p;
			log_fp->getChar(&p);
			pos = pos - 2;
			log_fp->seek(pos);
		} while (!(((p == LSQ) && isdigit(c)) || (pos > 0)));
	}
	
	if (p == LSQ)
	{
		while (log_fp->getChar(&c) && (c == SPC));
		do
		{
			pageno[i++] = (char) c;
			log_fp->getChar(&c);
		} while (isdigit(c));
		pageno[i] = NUL;
	}
	else
	{
		QTextStream log(ilg_fp);
		log << tr("Couldn't find any page number in ") << log_fn << tr("...ignored\n");
		init_page = 0;
	}
}

uchar XWMakeIndex::firstLetter(char *term)
{
	if (thai_sort)
		return strchr("噌忏?", term[0]) ? term[1] : term[0];
		
	return TOLOWER(term[0]);
}

void  XWMakeIndex::flushLine(int print)
{
	char    tmp[sizeof(buff)];

  if (pageDiff(begin, the_end) != 0)
  {
		if (encap_range || (pageDiff(begin, prev) > (*suffix_2p ? 0 : 1)))
		{
			int diff = pageDiff(begin, the_end);
		
			if ((diff == 1) && *suffix_2p)
		    sprintf(buff, "%s%s", begin->lpg, suffix_2p);
			else if ((diff == 2) && *suffix_3p)
		    sprintf(buff, "%s%s", begin->lpg, suffix_3p);
			else if ((diff >= 2) && *suffix_mp)
		    sprintf(buff, "%s%s", begin->lpg, suffix_mp);
			else
		    sprintf(buff, "%s%s%s", begin->lpg, delim_r, the_end->lpg);

	    encap_range = 0;
		}
		else
	   	sprintf(buff, "%s%s%s", begin->lpg, delim_n, the_end->lpg);
	}
  else
  {
		encap_range = 0; /* might be true from page range on same page */
		strcpy(buff, begin->lpg);
  }

  if (*prev_encap != NUL)
  {
		strcpy(tmp, buff);
		sprintf(buff, "%s%s%s%s%s",
		encap_p, prev_encap, encap_i, tmp, encap_s);
  }
  wrapLine(print);
}

void  XWMakeIndex::flushToEol()
{
	char a;
	while (!idx_fp->atEnd())
	{
		idx_fp->getChar(&a);
		if (a == LFD)
			break;
	}
}

void XWMakeIndex::genInd()
{
	QTextStream term(stderr);
	QTextStream log(ilg_fp);
	QString str = QString(tr("Generating output file %1...\n")).arg(ind_fn);
	term << str;
	log << str;
	
	ind_fp->write(preamble);
	ind_lc += prelen;
  if (init_page)
		insertPage();
		
	idx_dc = 0;
	int n = 0;
  for (; n < idx_gt; n++)
  {
  	if (idx_key[n]->type != DUPLICATE)
  	{
  		if (makeEntry(n))
  		{
  			idx_dot = 1;
  			if (idx_dc++ == 0)
  			{
  				if (verbose)
  					term << QString(DOT);
  					
  				log << QString(DOT);
  			}
  			
  			if (idx_dc == DOT_MAX)
  				idx_dc = 0;
  		}
  	}
  }
  
  int tmp_lc = ind_lc;
  if (in_range)
  {
  	curr = range_ptr;
  	QString str = QString(tr("Unmatched range opening operator %1.\n")).arg(idx_ropen);
  	QString tmpstr = QString(tr("## Warning (input = %1, line = %2; output = %3, line = %4):\n   -- "))
  		                 .arg(curr->fn).arg(curr->lc).arg(ind_fn).arg(ind_lc+1);
  	if (idx_dot)
  	{
  		log << "\n";
  		idx_dot = 0;
  	}
  	
  	log << tmpstr;
  	log << str;
  	ind_ec++;
  }
  
  prev = curr;
  flushLine(1);
  ind_fp->write(delim_t);
  ind_fp->write(postamble);
  tmp_lc = ind_lc + postlen;
  if (ind_ec == 1)
  {
  	if (verbose)
  	{
  		str = QString(tr("done (%1 lines written, %2 warning).\n")).arg(tmp_lc).arg(ind_ec);
  		term << str;
  		log << str;
  	}
  }
  else
  {
  	if (verbose)
  	{
  		str = QString(tr("done (%1 lines written, %2 warnings).\n")).arg(tmp_lc).arg(ind_ec);
  		term << str;
  		log << str;
  	}
  }
}

int  XWMakeIndex::groupType(char *str)
{
	int     i = 0;

  while ((str[i] != NUL) && ISDIGIT(str[i]))
		i++;

    if (str[i] == NUL) 
    {
			sscanf(str, "%d", &i);
			return (i);
    } 
    else if (ISSYMBOL(str[0]))
			return (SYMBOL);
    else
		return (ALPHA);
}

void XWMakeIndex::insertPage()
{
	int     i = 0;
  int     j = 0;
  int     page = 0;

  if (even_odd >= 0) 
  {
		/* find the rightmost digit */
		while (pageno[i++] != NUL);
		j = --i;
		/* find the leftmost digit */
		while (isdigit(pageno[--i]) && i > 0);
		if (!isdigit(pageno[i]))
	    i++;
		/* convert page from literal to numeric */
		page = strtoint(&pageno[i]) + 1;
		/* check even-odd numbering */
		if (((even_odd == 1) && (page % 2 == 0)) ||
	    	((even_odd == 2) && (page % 2 == 1)))
	    	page++;
		pageno[j + 1] = NUL;
		/* convert page back to literal */
		while (page >= 10) 
		{
	    pageno[j--] = TOASCII(page % 10);
	    page = page / 10;
		}
		pageno[j] = TOASCII(page);
		if (i < j) 
		{
	    while (pageno[j] != NUL)
				pageno[i++] = pageno[j++];
	    pageno[i] = NUL;
		}
  }
  ind_fp->write(setpage_open);
  ind_fp->write(pageno);
  ind_fp->write(setpage_close);
  ind_lc += setpagelen;
}

int XWMakeIndex::makeEntry(int n)
{
	QTextStream term(stderr);
	QTextStream log(ilg_fp);
	prev = curr;
  curr = idx_key[n];
  if ((*curr->encap == idx_ropen) || (*curr->encap == idx_rclose))
		encap = &(curr->encap[1]);
  else
		encap = curr->encap;
		
	int let = 0;
	if (n == 0)
	{
		prev_level = level = 0;
		let = *curr->sf[0];
		putHeader(let);
		makeItem(NIL);
	}
	else
	{
		prev_level = level;
		for (level = 0; level < FIELD_MAX; level++)
		{
	    if (0 != strcmp(curr->sf[level], prev->sf[level]) || 0 != strcmp(curr->af[level], prev->af[level]))
				break;
		}
		if (level < FIELD_MAX)
	    newEntry();
		else
	    oldEntry();
	}
	
	if (*curr->encap == idx_ropen)
	{
		if (in_range)
		{
			QString str = QString(tr("Extra range opening operator %1.\n")).arg(idx_ropen);
  		QString tmpstr = QString(tr("## Warning (input = %1, line = %2; output = %3, line = %4):\n   -- "))
  		                 					.arg(curr->fn).arg(curr->lc).arg(ind_fn).arg(ind_lc+1);
  		if (idx_dot)
  		{
  			log << "\n";
  			idx_dot = 0;
  		}
  	
  		log << tmpstr;
  		log << str;
  		ind_ec++;
		}
		else
		{
			in_range = 1;
	    range_ptr = curr;
		}
	}
	else if (*curr->encap == idx_rclose)
	{
		if (in_range)
		{
			in_range = FALSE;
	    if (0 != strcmp(&(curr->encap[1]), "") && 0 != strcmp(prev_encap, &(curr->encap[1])))
	    {
	    	QString str = QString(tr("Range closing operator has an inconsistent encapsulator %1.\n")).arg(&(curr->encap[1]));
  			QString tmpstr = QString(tr("## Warning (input = %1, line = %2; output = %3, line = %4):\n   -- "))
  		                 					.arg(curr->fn).arg(curr->lc).arg(ind_fn).arg(ind_lc+1);
  			if (idx_dot)
  			{
  				log << "\n";
  				idx_dot = 0;
  			}
  	
  			log << tmpstr;
  			log << str;
  			ind_ec++;
	    }
		}
		else
		{
			QString str = QString(tr("Unmatched range closing operator %1.\n")).arg(idx_rclose);
  		QString tmpstr = QString(tr("## Warning (input = %1, line = %2; output = %3, line = %4):\n   -- "))
  		                 					.arg(curr->fn).arg(curr->lc).arg(ind_fn).arg(ind_lc+1);
  		if (idx_dot)
  		{
  			log << "\n";
  			idx_dot = 0;
  		}
  	
  		log << tmpstr;
  		log << str;
  		ind_ec++;
		}
	}
	else if ((*curr->encap != NUL) && 0 != strcmp(curr->encap, prev_encap) && in_range)
	{
		QString str = QString(tr("Inconsistent page encapsulator %1 within range.\n")).arg(curr->encap);
  	QString tmpstr = QString(tr("## Warning (input = %1, line = %2; output = %3, line = %4):\n   -- "))
  		                 					.arg(curr->fn).arg(curr->lc).arg(ind_fn).arg(ind_lc+1);
  	if (idx_dot)
  	{
  		log << "\n";
  		idx_dot = 0;
  	}
  	
  	log << tmpstr;
  	log << str;
  	ind_ec++;
	}
	
	return 1;
}

void XWMakeIndex::makeItem(char *iterm)
{
	if (level > prev_level) 
	{
		/* ascending level */
		if (*curr->af[level] == NUL)
	    sprintf(line, "%s%s%s", iterm, item_u[level], curr->sf[level]);
		else
	    sprintf(line, "%s%s%s", iterm, item_u[level], curr->af[level]);
		ind_lc += ilen_u[level];
   } 
   else 
   {
			/* same or descending level */
			if (*curr->af[level] == NUL)
	    	sprintf(line, "%s%s%s", iterm, item_r[level], curr->sf[level]);
			else
	    	sprintf(line, "%s%s%s", iterm, item_r[level], curr->af[level]);
			ind_lc += ilen_r[level];
   }

   int i = level + 1;
   while (i < FIELD_MAX && *curr->sf[i] != NUL) 
   {
			ind_fp->write(line);
			if (*curr->af[i] == NUL)
	    	sprintf(line, "%s%s", item_x[i], curr->sf[i]);
			else
	    	sprintf(line, "%s%s", item_x[i], curr->af[i]);
			ind_lc += ilen_x[i];
			level = i;		/* Added at 2.11 <brosig@gmdzi.gmd.de> */
			i++;
   }

   ind_indent = 0;
   strcat(line, delim_p[level]);
   begin = the_end = curr;
   prev_encap = encap;
}

int XWMakeIndex::makeKey()
{
	QTextStream term(stderr);
	int     i;
  static size_t len = sizeof(KNODE);
	KNODE* ptr  = (KNODE*)malloc(len);
	if (ptr == NULL)
		term << tr("Not enough core...abort.\n");
		
	memset(ptr, 0, sizeof(KNODE));
	ptr->next = 0;
	
	for (i = 0; i < FIELD_MAX; i++)
  {
		makeString(&(ptr->data.sf[i]), 1);
		makeString(&(ptr->data.af[i]), 1);
  }
  makeString(&(ptr->data.encap), 1);
  ptr->data.lpg[0] = NUL;
  ptr->data.count = 0;
  ptr->data.type = EMPTY;

    /* process index key */
  if (!scanKey(&(ptr->data)))
		return (0);

    /* determine group type */
  ptr->data.group = groupType(ptr->data.sf[0]);

    /* process page number */
  strcpy(ptr->data.lpg, no);
  if (!scanNo(no, ptr->data.npg, &(ptr->data.count), &(ptr->data.type)))
		return (0);

  if (first_entry) 
  {
		head = tail = ptr;
		first_entry = 0;
  } 
  else 
  {
		tail->next = ptr;
		tail = ptr;
  }
  ptr->data.lc = idx_lc;
  ptr->data.fn = idx_fn;
  tail->next = NULL;

  return (1);
}

void XWMakeIndex::makeString(char **ppstr, int n)
{
	if (*ppstr)	
		free(*ppstr);
		
	(*ppstr) = (char*)malloc(n);
	if (!(*ppstr))
		return ;
	
	memset(*ppstr, 0, n);
}

void XWMakeIndex::newEntry()
{
	int    let;
  KFIELD * ptr;
  if (in_range)
  {
  	QTextStream term(stderr);
		QTextStream log(ilg_fp);
  	ptr = curr;
		curr = range_ptr;
		QString str = QString(tr("Unmatched range opening operator %1.\n")).arg(idx_ropen);
  	QString tmpstr = QString(tr("## Warning (input = %1, line = %2; output = %3, line = %4):\n   -- "))
  		                 					.arg(curr->fn).arg(curr->lc).arg(ind_fn).arg(ind_lc+1);
  	if (idx_dot)
  	{
  		log << "\n";
  		idx_dot = 0;
  	}
  	
  	log << tmpstr;
  	log << str;
  	ind_ec++;
  	in_range = 0;
		curr = ptr;
  }
  
  flushLine(1);
  if (((curr->group != ALPHA) && 
  		(curr->group != prev->group) && 
  		(prev->group == SYMBOL)) || 
  		((curr->group == ALPHA) && 
	 		((unsigned char)(let = firstLetter(curr->sf[0])) != firstLetter(prev->sf[0]))) ||
			(german_sort &&
	 		(curr->group != ALPHA) && (prev->group == ALPHA))) 
	{
		ind_fp->write(delim_t);
		ind_fp->write(group_skip);
		ind_lc += skiplen;
		/* beginning of a new letter? */
		putHeader(let);
		makeItem(NIL);
  } 
  else
		makeItem(delim_t);
}

int  XWMakeIndex::newStrCmp(unsigned char *s1, unsigned char *s2, int option)
{
	int     i;

    i = 0;
    while (s1[i] == s2[i])
	if (s1[i++] == NUL)
	    return (0);
    if (option)			       /* ASCII */
	return (isupper(s1[i]) ? -1 : 1);
    else			       /* GERMAN */
	return (isupper(s1[i]) ? 1 : -1);
}

int  XWMakeIndex::nextNonBlank()
{
	char     c;

    while (!sty_fp->atEnd()) 
    {
    	sty_fp->getChar(&c);
			switch (c) 
			{
				case EOF:
	    		return (-1);

				case LFD:
	    		sty_lc++;
				case SPC:
				case TAB:
	    		break;
	    		
				default:
	    		return (c);
			}
    }
}

void XWMakeIndex::oldEntry()
{
	int diff = pageDiff(the_end, curr);
	if ((prev->type == curr->type) && (diff != -1) &&
			(((diff == 0) && (prev_encap != NULL) && 
			 0 == strcmp(encap, prev_encap)) ||
	 		(merge_page && (diff == 1) &&
	  		(prev_encap != NULL) && 0 == strcmp(encap, prev_encap)) ||
	 			in_range)) 
	{
		the_end = curr;
	/* extract in-range encaps out */
		if (in_range &&
	    	(*curr->encap != NUL) &&
	    	(*curr->encap != idx_rclose) &&
	    	0 != strcmp(curr->encap, prev_encap)) 
	  {
	    sprintf(buff, "%s%s%s%s%s", encap_p, curr->encap, encap_i, curr->lpg, encap_s);
	    wrapLine(0);
		}
		if (in_range)
	    encap_range = 1;
  } 
  else 
  {
		flushLine(0);
	
		QTextStream term(stderr);
		QTextStream log(ilg_fp);
		if ((diff == 0) && (prev->type == curr->type))
		{
			QString str = QString(tr("Conflicting entries: multiple encaps for the same page under same key.\n"));
  		QString tmpstr = QString(tr("## Warning (input = %1, line = %2; output = %3, line = %4):\n   -- "))
  		                 					.arg(curr->fn).arg(curr->lc).arg(ind_fn).arg(ind_lc+1);
  		if (idx_dot)
  		{
  			log << "\n";
  			idx_dot = 0;
  		}
  	
  		log << tmpstr;
  		log << str;
  		ind_ec++;
		}
		else if (in_range && (prev->type != curr->type))
		{
			QString str = QString(tr("Illegal range formation: starting & ending pages are of different types.\n"));
  		QString tmpstr = QString(tr("## Warning (input = %1, line = %2; output = %3, line = %4):\n   -- "))
  	                 					.arg(curr->fn).arg(curr->lc).arg(ind_fn).arg(ind_lc+1);
  		if (idx_dot)
  		{
  			log << "\n";
  			idx_dot = 0;
  		}
  	
  		log << tmpstr;
  		log << str;
  		ind_ec++;
		}
		else if (in_range && (diff == -1))
		{
			QString str = QString(tr("Illegal range formation: starting & ending pages cross chap/sec breaks.\n"));
  		QString tmpstr = QString(tr("## Warning (input = %1, line = %2; output = %3, line = %4):\n   -- "))
  		                 					.arg(curr->fn).arg(curr->lc).arg(ind_fn).arg(ind_lc+1);
  		if (idx_dot)
  		{
  			log << "\n";
  			idx_dot = 0;
  		}
  	
  		log << tmpstr;
  		log << str;
  		ind_ec++;
		}
	
		begin = the_end = curr;
  	prev_encap = encap;
  }
}

void XWMakeIndex::openSty(char *fn)
{
	QTextStream term(stderr);
	QTextCodec * codec = QTextCodec::codecForLocale(); 
	QString styname = codec->toUnicode(fn);
	if (styname.isEmpty())
	{
		term << tr("Index style file ") << fn << tr(" not found.\n");
		cleanup();
		exit(1);
	}
	
	strcpy(sty_fn,fn);
	sty_fp = new QFile(styname);
	if (!sty_fp->open(QIODevice::ReadOnly))
	{
		delete sty_fp;
		sty_fp = 0;
		term << tr("Could not open style file ") << fn << (".\n");
		cleanup();
		exit(1);
	}
}

int XWMakeIndex::pageDiff(KFIELD * a,KFIELD * b)
{
	if (a->count != b->count)
		return (-1);
		
  for (short i = 0; i < a->count - 1; i++)
		if (a->npg[i] != b->npg[i])
	    return (-1);
  return (b->npg[b->count - 1] - a->npg[a->count - 1]);
}

void XWMakeIndex::prepareIdx()
{
	QTextStream term(stderr);
	
	KNODE * ptr = head;
	if (head == NULL)
	{
		term << tr("No valid index entries collected.\n");
		cleanup();
		exit(1);
	}
	
	if ((idx_key = (KFIELD**) calloc(idx_gt, sizeof(KFIELD*))) == NULL)
	{
		term << tr("Not enough core...abort.\n");
		cleanup();
		exit(1);
	}
	
	for (int i = 0; i < idx_gt; i++)
	{
		idx_key[i] = &(ptr->data);
		ptr = ptr->next;
	}
}

void XWMakeIndex::processIdx(char *fn[], 
	                           int use_stdin, 
	                            int sty_given, 
	                						int ind_given,
	                						int ilg_given, 
	                						int log_given)
{
	QTextStream term(stderr);
	QTextCodec * codec = QTextCodec::codecForLocale(); 
	if (fn_no == -1)
		use_stdin = 1;
	else
	{
		checkAll(fn[0], ind_given, ilg_given, log_given);
		QString p = getProductName();
		QString v = getVersion();
		term << tr("This is ") << p << ", " << v << ".\n";
		need_version = 0;
		if (sty_given)
			scanSty();
		if (german_sort && (idx_quote == '"'))
		{
			term << tr("Option -g invalid, quote character must be different from '\"'.\n");
			cleanup();
			exit(1);
		}
		
		scanIdx();
		ind_given = 1;
		ilg_given = 1;
		
		for (int i = 1; i <= fn_no; i++)
		{
			checkIdx(fn[i], 1);
	    scanIdx();
		}
	}
	
	if (use_stdin)
	{
		idx_fn = "stdin";
		if (!idx_fp)
		{
			idx_fp = new QFile;
			idx_fp->open(stdin, QIODevice::ReadOnly);		
		}
		
		if (ind_given)
		{
			if (!ind_fp)
			{
				QString indname = codec->toUnicode(ind_fn);
				if (!outputPath.isEmpty())
					indname = QString("%1/%2").arg(outputPath).arg(indname);
				
				ind_fp = new QFile(indname);
				if (!ind_fp->open(QIODevice::WriteOnly))
				{
					delete ind_fp;
					ind_fp = 0;
					term << tr("Can't create output index file ") << indname << ".\n";
					cleanup();
					exit(1);
				}
			}
		}
		else
		{
			if (!ind_fp)
			{
				ind_fp = new QFile;
				ind_fp->open(stdout, QIODevice::WriteOnly);
			}
		}
		
		if (ilg_given)
		{
			if (!ilg_fp)
			{
				QString ilgname = codec->toUnicode(ilg_fn);
				ilg_fp = new QFile(ilgname);
				if (!ilg_fp->open(QIODevice::WriteOnly))
				{
					delete ilg_fp;
					ilg_fp = 0;
					term << tr("Can't create transcript file ") << ilgname << ".\n";
					cleanup();
					exit(1);
				}
			}
		}
		else
		{
			if (!ilg_fp)
			{
				ilg_fp = new QFile;
				ilg_fp->open(stderr, QIODevice::WriteOnly);
				verbose = 0;
			}
		}
		
		if ((fn_no == -1) && (sty_given))
	    scanSty();
		if (german_sort && (idx_quote == '"'))
		{
			term << tr("Option -g invalid, quote character must be different from '\"'.\n");
			cleanup();
			exit(1);
		}
		
		if (need_version)
		{
			QString p = getProductName();
			QString v = getVersion();
			term << tr("This is ") << p << " " << v << "\n";
			need_version = 0;
		}
		
		scanIdx();
		fn_no++;
	}
}

int  XWMakeIndex::processPrecedence()
{
	int     order[PAGETYPE_MAX];
    int     type[PAGETYPE_MAX];
    int     i = 0;
    int     last;
    int     roml = 0;
    int     romu = 0;
    int     arab = 0;
    int     alpl = 0;
    int     alpu = 0;
		QTextStream log(ilg_fp);
    /* check for llegal specifiers first */
    while ((i < PAGETYPE_MAX) && (page_prec[i] != NUL)) 
    {
			switch (page_prec[i]) 
			{
				case ROMAN_LOWER:
	    		if (roml) 
	    		{
	    			char a;
	    			while (!sty_fp->atEnd())
	    			{
	    				sty_fp->getChar(&a);
	    				if (a == LFD)
	    					break;
	    			}
	    			sty_lc++;
	    			if (idx_dot)
	    			{
	    				log << "\n";
	    				idx_dot = 0;
	    			}
	    	
	    			QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 					"Multiple instances of type `%3' in page precedence specification `%4'.\n"))
	    	                  				.arg(sty_fn).arg(sty_lc).arg(ROMAN_LOWER).arg(page_prec);
						log << str;
						sty_ec++;
						put_dot = 0;
						return (0);
	    		} 
	    		else
						roml = 1;
	    		break;
	    
				case ROMAN_UPPER:
	    		if (romu) 
	    		{
	    			if (idx_dot)
	    			{
	    				log << "\n";
	    				idx_dot = 0;
	    			}
	    	
	    			QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 					"Multiple instances of type `%3' in page precedence specification `%4'.\n"))
	    	                  				.arg(sty_fn).arg(sty_lc).arg(ROMAN_UPPER).arg(page_prec);
						log << str;
						sty_ec++;
						put_dot = 0;
						return (0);
	    		} 
	    		else
						romu = 1;
	    		break;
	    		
				case ARABIC:
	    		if (arab) 
	    		{
	    			if (idx_dot)
	    			{
	    				log << "\n";
	    				idx_dot = 0;
	    			}
	    	
	    			QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 					"Multiple instances of type `%3' in page precedence specification `%4'.\n"))
	    	                  				.arg(sty_fn).arg(sty_lc).arg(ARABIC).arg(page_prec);
						log << str;
						sty_ec++;
						put_dot = 0;
						return (0);
	    		} 
	    		else
						arab = 1;
	    		break;
	    		
				case ALPHA_LOWER:
	   		 	if (alpl) 
	   		 	{
	   		 		if (idx_dot)
	    			{
	    				log << "\n";
	    				idx_dot = 0;
	    			}
	    	
	    			QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 					"Multiple instances of type `%3' in page precedence specification `%4'.\n"))
	    	                  				.arg(sty_fn).arg(sty_lc).arg(ALPHA_UPPER).arg(page_prec);
						log << str;
						sty_ec++;
						put_dot = 0;
						return (0);
	    		} 
	    		else
						alpl = 1;
	    		break;
	    		
				case ALPHA_UPPER:
	    		if (alpu) 
	    		{
	    			if (idx_dot)
	    			{
	    				log << "\n";
	    				idx_dot = 0;
	    			}
	    	
	    			QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 					"Multiple instances of type `%3' in page precedence specification `%4'.\n"))
	    	                  				.arg(sty_fn).arg(sty_lc).arg(ALPHA_UPPER).arg(page_prec);
						log << str;
						sty_ec++;
						put_dot = 0;
						return (0);
	    		} 
	    		else
						alpu = 1;
	    		break;
	    		
			default:
				{
					char a;
	    			while (!sty_fp->atEnd())
	    			{
	    				sty_fp->getChar(&a);
	    				if (a == LFD)
	    					break;
	    			}
	    			sty_lc++;
				}
				{
					if (idx_dot)
	    		{
	    			log << "\n";
	    			idx_dot = 0;
	    		}
	    	
	    		QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 					"Unknow type `%3' in page precedence specification.\n"))
	    	                  				.arg(sty_fn).arg(sty_lc).arg(page_prec[i]);
					log << str;
					sty_ec++;
					put_dot = 0;
				}
	    	return (0);
			}
			i++;
  	}
    
   if (page_prec[i] != NUL) 
   {
			char a;
	    while (!sty_fp->atEnd())
	    {
	    	sty_fp->getChar(&a);
	    	if (a == LFD)
	    		break;
	    }
	    sty_lc++;
	    if (idx_dot)
	    {
	    	log << "\n";
	    	idx_dot = 0;
	    }
	    	
	    QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	                   					"Page precedence specification string too long.\n"))
	                     				.arg(sty_fn).arg(sty_lc);
			log << str;
			sty_ec++;
			put_dot = 0;
					
			return (0);
    }
    
    last = i;
    switch (page_prec[0]) 
    {
    	case ROMAN_LOWER:
				order[0] = ROMAN_LOWER_OFFSET;
				type[0] = ROML;
				break;
				
    	case ROMAN_UPPER:
				order[0] = ROMAN_UPPER_OFFSET;
				type[0] = ROMU;
				break;
				
    	case ARABIC:
				order[0] = ARABIC_OFFSET;
				type[0] = ARAB;
				break;
				
    	case ALPHA_LOWER:
				order[0] = ALPHA_LOWER_OFFSET;
				type[0] = ALPL;
				break;
				
    	case ALPHA_UPPER:
				order[0] = ALPHA_LOWER_OFFSET;
				type[0] = ALPU;
				break;
				
    }

    i = 1;
    while (i < last) 
    {
			switch (page_prec[i]) 
			{
				case ROMAN_LOWER:
	    		order[i] = order[i - 1] + ROMAN_LOWER_OFFSET;
	    		type[i] = ROML;
	    		break;
	    		
				case ROMAN_UPPER:
	    		order[i] = order[i - 1] + ROMAN_UPPER_OFFSET;
	    		type[i] = ROMU;
	    		break;
	    		
				case ARABIC:
	    		order[i] = order[i - 1] + ARABIC_OFFSET;
	    		type[i] = ARAB;
	    		break;
	    		
				case ALPHA_LOWER:
	    		order[i] = order[i - 1] + ALPHA_LOWER_OFFSET;
	    		type[i] = ALPL;
	    		break;
	    		
				case ALPHA_UPPER:
	    		order[i] = order[i - 1] + ALPHA_LOWER_OFFSET;
	    		type[i] = ALPU;
	    		break;
	    		
			}
			i++;
    }

    for (i = 0; i < PAGETYPE_MAX; i++) 
    {
			page_offset[i] = -1;
    }
    page_offset[type[0]] = 0;
    for (i = 1; i < last; i++) 
    {
			page_offset[type[i]] = order[i - 1];
    }
    for (i = 0; i < PAGETYPE_MAX; i++) 
    {
			if (page_offset[i] == -1) 
			{
	    	switch (type[last - 1]) 
	    	{
	    		case ROML:
						order[last] = order[last - 1] + ROMAN_LOWER_OFFSET;
						break;
						
	    		case ROMU:
						order[last] = order[last - 1] + ROMAN_UPPER_OFFSET;
						break;
						
	    		case ARAB:
						order[last] = order[last - 1] + ARABIC_OFFSET;
						break;
						
	    		case ALPL:
						order[last] = order[last - 1] + ALPHA_LOWER_OFFSET;
						break;
						
	    		case ALPU:
						order[last] = order[last - 1] + ALPHA_UPPER_OFFSET;
						break;
						
	    	}
	    	type[last] = i;
	    	page_offset[i] = order[last];
	    	last++;
			}
    }
    return (1); 
}

void XWMakeIndex::putHeader(int let)
{
	if (headings_flag)
  {
		ind_fp->write(heading_pre);
		ind_lc += headprelen;
		switch (curr->group)
		{
			case SYMBOL:
	    	if (headings_flag > 0)
	    	{
					ind_fp->write(symhead_pos);
	    	}
	    	else
	    	{
					ind_fp->write(symhead_neg);
	    	}
	    	break;
	    
			case ALPHA:
	  	  if (headings_flag > 0)
	    	{
					let = TOUPPER(let);
					ind_fp->putChar((char)let);
	    	}
	    	else
	    	{
					let = TOLOWER(let);
					ind_fp->putChar((char)let);
	    	}
	   	 	break;
	    
			default:
	    	if (headings_flag > 0)
	    	{
					ind_fp->write(numhead_pos);
	    	}
	    	else
	    	{
					ind_fp->write(numhead_neg);
	    	}
	    	break;
		}
		ind_fp->write(heading_suf);
		ind_lc += headsuflen;
  }
}

void XWMakeIndex::qqsort(char *base, int n)
{
	char *i;
  char *j;
  char *lo;
  char *hi;
  char *min;
  char c;
  char   *max;

    if (n <= 1)
			return;
    qsz = sizeof(KFIELD*);
//    qcmp = compar;
    thresh = qsz * THRESH;
    mthresh = qsz * MTHRESH;
    max = base + n * qsz;
    if (n >= THRESH) 
    {
			qst(base, max);
			hi = base + thresh;
    } 
    else 
    {
			hi = max;
    }
    /*
     * First put smallest element, which must be in the first THRESH, in the
     * first position as a sentinel.  This is done just by searching the
     * first THRESH elements (or the first n if n < THRESH), finding the min,
     * and swapping it into the first position.
     */
    for (j = lo = base; (lo += qsz) < hi;) 
    {
			if (compare((KFIELD**)j, (KFIELD**)lo) > 0)
	    j = lo;
    }
    if (j != base) 
    {		       /* swap j into place */
			for (i = base, hi = base + qsz; i < hi;) 
			{
	    	c = *j;
	    	*j++ = *i;
	    	*i++ = c;
			}
    }
    /*
     * With our sentinel in place, we now run the following hyper-fast
     * insertion sort. For each remaining element, min, from [1] to [n-1],
     * set hi to the index of the element AFTER which this one goes. Then, do
     * the standard insertion sort shift on a character at a time basis for
     * each element in the frob.
     */
    for (min = base; (hi = min += qsz) < max;) 
    {
			while (compare((KFIELD**)(hi -= qsz), (KFIELD**)min) > 0);
			if ((hi += qsz) != min) 
			{
	    	for (lo = min + qsz; --lo >= min;) 
	    	{
					c = *lo;
					for (i = j = lo; (j -= qsz) >= hi; i = j)
		    		*i = *j;
					*i = c;
	    	}
			}
    }
}

void XWMakeIndex::qst(char *base, char *max)
{
		char *i;
    char *j;
    char *jj;
    char *mid;
    int ii;
    char c;
    char   *tmp;
    int     lo;
    int     hi;

    lo = (int)(max - base);		/* number of elements as chars */
    do 
    {
			/*
	 		* At the top here, lo is the number of characters of elements in the
	 		* current partition.  (Which should be max - base). Find the median
	 		* of the first, last, and middle element and make that the middle
	 		* element.  Set j to largest of first and middle.  If max is larger
	 		* than that guy, then it's that guy, else compare max with loser of
	 		* first and take larger.  Things are set up to prefer the middle,
	 		* then the first in case of ties.
	 		*/
			mid = i = base + qsz * ((unsigned) (lo / qsz) >> 1);
			if (lo >= mthresh) 
			{
	    	j = (compare((KFIELD**)(jj = base), (KFIELD**)i) > 0 ? jj : i);
	    	if (compare((KFIELD**)j, (KFIELD**)(tmp = max - qsz)) > 0) 
	    	{
					/* switch to first loser */
					j = (j == jj ? i : jj);
					if (compare((KFIELD**)j, (KFIELD**)tmp) < 0)
		    		j = tmp;
	    	}
	    	if (j != i) 
	    	{
					ii = qsz;
					do 
					{
		    		c = *i;
		    		*i++ = *j;
		    		*j++ = c;
					} while (--ii);
	    	}
			}
			/* Semi-standard quicksort partitioning/swapping */
			for (i = base, j = max - qsz;;) 
			{
	    	while (i < mid && compare((KFIELD**)i, (KFIELD**)mid) <= 0)
					i += qsz;
	    	while (j > mid) 
	    	{
					if (compare((KFIELD**)mid, (KFIELD**)j) <= 0) 
					{
		    		j -= qsz;
		    		continue;
					}
					tmp = i + qsz;	       /* value of i after swap */
					if (i == mid) 
					{	       /* j <-> mid, new mid is j */
		    		mid = jj = j;
					} 
					else 
					{	       /* i <-> j */
		    		jj = j;
		    		j -= qsz;
					}
					goto swap;
	    	}
	    	if (i == mid) 
	    	{
					break;
	    	} 
	    	else 
	    	{		       /* i <-> mid, new mid is i */
					jj = mid;
					tmp = mid = i;	       /* value of i after swap */
					j -= qsz;
	    	}
swap:
	    	ii = qsz;
	    	do 
	    	{
					c = *i;
					*i++ = *jj;
					*jj++ = c;
	    	} while (--ii);
	    	i = tmp;
			}
			/*
	 		* Look at sizes of the two partitions, do the smaller one first by
	 		* recursion, then do the larger one by making sure lo is its size,
	 		* base and max are update correctly, and branching back. But only
	 		* repeat (recursively or by branching) if the partition is of at
	 		* least size THRESH.
	 		*/
			i = (j = mid) + qsz;
			if ((lo = (int)(j - base)) <= (hi = (int)(max - i))) 
			{
	    	if (lo >= thresh)
					qst(base, j);
	    	base = i;
	    	lo = hi;
			} 
			else 
			{
		    if (hi >= thresh)
					qst(i, max);
	    	max = j;
			}
    } while (lo >= thresh);
}

int  XWMakeIndex::scanAlphaLower(char no[], 
	                               int npg[], 
	                               short *count)
{
	if (*count >= PAGEFIELD_MAX)
	{
		QTextStream log(ilg_fp);
		if (idx_dot)
	  {
	  	log << "\n";
	  	idx_dot = 0;
	  }
	  QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              			"Page number %3 has too many fields (max. %4)."))
	    	               			.arg(idx_fn).arg(idx_lc).arg(no).arg(PAGEFIELD_MAX);
		log << str;
		idx_ec++;
	  return (0);
	}
	
    npg[*count] = (ALPHA_VAL(no[0]) + page_offset[ALPL]);
    ++*count;

    short i = 1;
    if (IS_COMPOSITOR)
			return (scanNo(&no[comp_len + 1], npg, count, &i));
    else
			return (1);
}

int  XWMakeIndex::scanAlphaUpper(char no[], 
	                               int npg[], 
	                               short *count)
{
	short   i;
	
	if (*count >= PAGEFIELD_MAX)
	{
		QTextStream log(ilg_fp);
		if (idx_dot)
	  {
	  	log << "\n";
	  	idx_dot = 0;
	  }
	  QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              			"Page number %3 has too many fields (max. %4)."))
	    	               			.arg(idx_fn).arg(idx_lc).arg(no).arg(PAGEFIELD_MAX);
		log << str;
		idx_ec++;
	  return (0);
	}
	
    npg[*count] = (ALPHA_VAL(no[0]) + page_offset[ALPU]);
    ++*count;

    i = 1;
    if (IS_COMPOSITOR)
	return (scanNo(&no[comp_len + 1], npg, count, &i));
    else
	return (TRUE);
}

int  XWMakeIndex::scanArabic(char no[], 
	                           int npg[], 
	                           short *count)
{
	short   i = 0;
    char    str[ARABIC_MAX+1];		/* space for trailing NUL */
		QTextStream log(ilg_fp);
    while ((no[i] != NUL) && (i <= ARABIC_MAX) && (!IS_COMPOSITOR)) 
    {
			if (isdigit(no[i])) 
			{
	    	str[i] = no[i];
	    	i++;
			} 
			else 
			{
				if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              			"Illegal Arabic digit: position %3 in %4.\n"))
	    	               			.arg(idx_fn).arg(idx_lc).arg(i + 1).arg(no);
				log << str;
				idx_ec++;
	    	return (0);
			}
    }
    
    if (i > ARABIC_MAX) 
    {
    	if (idx_dot)
	    {
	    	log << "\n";
	    	idx_dot = 0;
	    }
	    QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	                  			"Arabic page number %3 too big (max %4 digits).\n"))
	                   			.arg(idx_fn).arg(idx_lc).arg(no).arg(ARABIC_MAX);
			log << str;
			idx_ec++;
			return (0);
    }
    str[i] = NUL;
    if (*count >= PAGEFIELD_MAX)
    {
    	if (idx_dot)
	    {
	    	log << "\n";
	    	idx_dot = 0;
	    }
	    QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	                  			"Page number %3 has too many fields (max. %4).\n"))
	                   			.arg(idx_fn).arg(idx_lc).arg(no).arg(PAGEFIELD_MAX);
			log << str;
			idx_ec++;
			return 0;
    }
		
		if (*count >= PAGEFIELD_MAX)
		{	
			if (idx_dot)
	  	{
	  		log << "\n";
	  		idx_dot = 0;
	  	}
	  	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              			"Page number %3 has too many fields (max. %4)."))
	    	               			.arg(idx_fn).arg(idx_lc).arg(no).arg(PAGEFIELD_MAX);
			log << str;
			idx_ec++;
	  	return (0);
		}
    npg[*count] = (strtoint(str) + page_offset[ARAB]);
    ++*count;

    if (IS_COMPOSITOR)
			return (scanNo(&no[i + comp_len], npg, count, &i));
    else
			return (1);
}

int  XWMakeIndex::scanArg1()
{
	int     i = 0;
  int     n = 0;		       /* delimiter count */
  char     a;
  if (compress_blanks)
  {
  	while (!idx_fp->atEnd())
  	{
  		idx_fp->getChar(&a);
  		if (a != SPC && a != TAB)
  			break;
  	}
  }
  else
  	idx_fp->getChar(&a);
  	
	QTextStream log(ilg_fp);
  	
  while ((i < ARGUMENT_MAX) && (a != EOF))
  {
  	if ((a == idx_quote) || (a == idx_escape))
  	{
  		key[i++] = a;	/* but preserve quote or escape */
	    idx_fp->getChar(&a);
	    key[i++] = a;
  	}
  	else if (a == idx_aopen)
  	{
  		key[i++] = (char) a;
	    n++;
  	}
  	else if (a == idx_aclose)
  	{
  		if (n == 0)			/* end of argument */
	    {
				if (compress_blanks && key[i - 1] == SPC)
		    	key[i - 1] = NUL;
				else
		    	key[i] = NUL;
				return (1);
	    }
	    else			/* nested delimiters */
	    {
				key[i++] = (char) a;
				n--;
	    }
  	}
  	else
  	{
  		switch (a)
	    {
	    	case LFD:
					idx_lc++;
					{
						if (idx_dot)
	    			{
	    				log << "\n";
	    				idx_dot = 0;
	    			}
	    			QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              								"Incomplete first argument (premature LFD).\n"))
	    	               								.arg(idx_fn).arg(idx_lc).arg(idx_level);
						log << str;
						idx_ec++;
					}
					return (0);
					
	    	case TAB:
	    	case SPC:
					/* compress successive SPC's to one SPC */
					if (compress_blanks)
					{
		    		if ((i > 0) && (key[i - 1] != SPC) && (key[i - 1] != TAB))
							key[i++] = SPC;
		    		break;
					}
					
	    	default:
					key[i++] = (char) a;
					break;
	    }
  	}
  	
  	idx_fp->getChar(&a);
  }
  
  flushToEol();			/* Skip to end of line */
  idx_lc++;
  if (idx_dot)
	{
		log << "\n";
		idx_dot = 0;
	}
	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	 	              			"First argument too long (max 1024).\n"))
	 	               			.arg(idx_fn).arg(idx_lc);
	log << str;
	idx_ec++;
	return (0);
}

int  XWMakeIndex::scanArg2()
{
	int     i = 0;
  char     a;
  int     hit_blank = 0;
  
  while (!idx_fp->atEnd())
  {
  	idx_fp->getChar(&a);
  	if (a != SPC && a != TAB)
  		break;
  }
  
	QTextStream log(ilg_fp);
  QString str;
  while (i < NUMBER_MAX)
  {
  	if (a == idx_aclose) 
  	{
	    no[i] = NUL;
	    return (1);
		}
		else
		{
			switch (a)
			{
				 case LFD:
						idx_lc++;
						{
							if (idx_dot)
	    				{
	    					log << "\n";
	    					idx_dot = 0;
	    				}
	    				str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              								"Incomplete second argument (premature LFD).\n"))
	    	               								.arg(idx_fn).arg(idx_lc);
							log << str;
							idx_ec++;
						}
						return (0);
					
					case TAB:
	    		case SPC:
						hit_blank = 1;
						break;
						
					default:
						flushToEol();	/* Skip to end of line */
		    		idx_lc++;
		    		if (hit_blank)
		    		{
							if (idx_dot)
	    				{
	    					log << "\n";
	    					idx_dot = 0;
	    				}
	    				str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              								"Illegal space within numerals in second argument.\n"))
	    	               								.arg(idx_fn).arg(idx_lc);
							log << str;
							idx_ec++;
							return (0);
						}
						no[i++] = a;
						break;
			}
		}
		idx_fp->getChar(&a);
  }
  
  flushToEol();	/* Skip to end of line */
  idx_lc++;
  if (idx_dot)
	{
		log << "\n";
		idx_dot = 0;
	}
	str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	         								"Second argument too long (max 16).\n"))
	         								.arg(idx_fn).arg(idx_lc);
	log << str;
	idx_ec++;
  return (0);
}

int XWMakeIndex::scanChar(char *c)
{
	char     clone, tmp;
	QTextStream log(ilg_fp);
    switch (clone = (char)nextNonBlank()) 
    {
    	case CHR_DELIM:
    		sty_fp->getChar(&clone);
				switch (clone) 
				{
					case CHR_DELIM:
	    			{
	    				char a;
	    				while (!sty_fp->atEnd())
	    				{
	    					sty_fp->getChar(&a);
	    					if (a == LFD)
	    						break;
	    				}
	    				sty_lc++;
	    			}
	    			{
	    				if (idx_dot)
	    				{
	    					log << "\n";
	    					idx_dot = 0;
	    				}
	    	
	    				QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 							"Premature closing delimiter.\n"))
	    	                 			 				.arg(sty_fn).arg(sty_lc);
							log << str;
							sty_ec++;
							put_dot = 0;
	    			}
	    			return (0);
	    			
					case LFD:
	    			sty_lc++;
	    			
					case EOF:
						{
							if (idx_dot)
	    				{
	    					log << "\n";
	    					idx_dot = 0;
	    				}
	    	
	    				QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 							"No character (premature EOF).\n"))
	    	                 			 				.arg(sty_fn).arg(sty_lc);
							log << str;
							sty_ec++;
							put_dot = 0;
						}
	    			return (0);
	    			
					case BSH:
	    			sty_fp->getChar(&clone);
					default:
						sty_fp->getChar(&tmp);
	    			if (tmp == CHR_DELIM) 
	    			{
							*c = (char) clone;
							return (1);
	    			} 
	    			else 
	    			{
	    				if (idx_dot)
	    				{
	    					log << "\n";
	    					idx_dot = 0;
	    				}
	    	
	    				QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 							"No closing delimiter or too many letters.\n"))
	    	                 			 				.arg(sty_fn).arg(sty_lc);
							log << str;
							sty_ec++;
							put_dot = 0;
							return (0);
	    			}
					}
					/* break; */				/* NOT REACHED */
    		case COMMENT:
					{
						char a;
	    				while (!sty_fp->atEnd())
	    				{
	    					sty_fp->getChar(&a);
	    					if (a == LFD)
	    						break;
	    				}
	    				sty_lc++;
					}
					break;
					
    		default:
					{
						char a;
	    				while (!sty_fp->atEnd())
	    				{
	    					sty_fp->getChar(&a);
	    					if (a == LFD)
	    						break;
	    				}
	    				sty_lc++;
					}
					{
						if (idx_dot)
	    				{
	    					log << "\n";
	    					idx_dot = 0;
	    				}
	    	
	    				QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 							"No opening delimiter.\n"))
	    	                 			 				.arg(sty_fn).arg(sty_lc);
							log << str;
							sty_ec++;
							put_dot = 0;
					}
					return (0);
    	}
    	return (1); 
}

int XWMakeIndex::scanField(int *n, 
	                         char field[], 
	                         int len_field, 
	                         int ck_level, 
	                         int ck_encap, 
	                         int ck_actual)
{
	int     i = 0;
  int     nbsh;		       /* backslash count */
  QTextStream term(stderr);
	QTextStream log(ilg_fp);

  if (compress_blanks && ((key[*n] == SPC) || (key[*n] == TAB)))
		++* n;

  while (1) 
  {
		nbsh = 0;
		while (key[*n] == idx_escape)
		{
	    nbsh++;
	    field[i++] = key[*n];
	    CHECK_LENGTH();
	    ++*n;
		}

		if (key[*n] == idx_quote)
		{
	    if (nbsh % 2 == 0)
				field[i++] = key[++*n];
	    else
				field[i++] = key[*n];
	    CHECK_LENGTH();
		}
		else if ((ck_level && (key[*n] == idx_level)) ||
		 				(ck_encap && (key[*n] == idx_encap)) ||
		 				(ck_actual && (key[*n] == idx_actual)) ||
		 				(key[*n] == NUL))
		{
	    if ((i > 0) && compress_blanks && (field[i - 1] == SPC))
					field[i - 1] = NUL;
	    else
				field[i] = NUL;
	    return (1);
		} 
		else 
		{
	    field[i++] = key[*n];
	    CHECK_LENGTH();
	    if ((!ck_level) && (key[*n] == idx_level)) 
	    {
	    	if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              			"Extra `%3' at position %4 of first argument.\n"))
	    	               			.arg(idx_fn).arg(idx_lc).arg(idx_level).arg(*n + 1);
				log << str;
				idx_ec++;
				return (0);
	    } 
	    else if ((!ck_encap) && (key[*n] == idx_encap)) 
	    {
	    	if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              			"Extra `%3' at position %4 of first argument.\n"))
	    	               			.arg(idx_fn).arg(idx_lc).arg(idx_encap).arg(*n + 1);
				log << str;
				idx_ec++;
				return (0);
	    } 
	    else if ((!ck_actual) && (key[*n] == idx_actual)) 
	    {
	    	if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              			"Extra `%3' at position %4 of first argument.\n"))
	    	               			.arg(idx_fn).arg(idx_lc).arg(idx_actual).arg(*n + 1);
				log << str;
				idx_ec++;
				return (0);
	    }
		}
	/* check if max field length is reached */
		if (i > len_field)
		{
FIELD_OVERFLOW:
	    if (!ck_encap) 
	    {
	    	if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              			"Encapsulator of page number too long (max. %3).\n"))
	    	               			.arg(idx_fn).arg(idx_lc).arg(len_field);
				log << str;
				idx_ec++;				
	    } 
	    else if (ck_actual) 
	    {
	    	if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              			"Index sort key too long (max. %3).\n"))
	    	               			.arg(idx_fn).arg(idx_lc).arg(len_field);
				log << str;
				idx_ec++;			
	    } 
	    else 
	    {
	    	if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              			"Text of key entry too long (max. %3).\n"))
	    	               			.arg(idx_fn).arg(idx_lc).arg(len_field);
				log << str;
				idx_ec++;	
	    }
	    return (0);
		}
		++*n;
  }
}

void XWMakeIndex::scanIdx()
{
	QTextStream term(stderr);
	QTextStream log(ilg_fp);
	QString str = QString(tr("Scanning input file %1...\n")).arg(idx_fn);
	term << str;
	log << str;
	
	char    keyword[ARRAY_MAX];
  char    c;
  int     i = 0;
  int     not_eof = 1;
  int     arg_count = -1;
    
  idx_lc = idx_tc = idx_ec = idx_dc = 0;
  comp_len = strlen(page_comp);
  while (not_eof) 
  {
  	if (idx_fp->atEnd())
  		c = EOF;
  	else
  		idx_fp->getChar(&c);
  			
		switch (c) 
		{
			case EOF:
	    	if (arg_count == 2) 
	    	{
					idx_lc++;
					if (makeKey())
					{
						idx_dot = 1;
						if (idx_dc++ == 0)
						{
							if (verbose)
								term << DOT;
							log << DOT;
						}
						
						if (idx_dc == DOT_MAX)
							idx_dc = 0;
					}
					arg_count = -1;
	    	} 
	    	else
					not_eof = 0;
	    	break;

			case LFD:
	    	idx_lc++;
	    	if (arg_count == 2) 
	    	{
					if (makeKey())
		    	{
		    		idx_dot = 1;
						if (idx_dc++ == 0)
						{
							if (verbose)
								term << DOT;
							log << DOT;
						}
						
						if (idx_dc == DOT_MAX)
							idx_dc = 0;
		    	}		    	
					arg_count = -1;
	    	} 
	    	else if (arg_count > -1) 
	    	{
	    		if (idx_dot)
	    		{
	    			log << "\n";
	    			idx_dot = 0;
	    		}
	    		str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    		                 "Missing arguments -- need two (premature LFD).\n"))
	    		                 .arg(idx_fn).arg(idx_lc);
					log << str;
					idx_ec++;
					arg_count = -1;
	   	 	}
			case TAB:
			case SPC:
	    	break;

			default:
	    	switch (arg_count) 
	    	{
	    		case -1:
						i = 0;
						keyword[i++] = (char) c;
						arg_count++;
						idx_tc++;
						break;
						
	    		case 0:
						if (c == idx_aopen) 
						{
		    			arg_count++;
		    			keyword[i] = NUL;
		    			if (0 == strcmp(keyword, idx_keyword)) 
		    			{
								if (!scanArg1()) 
								{
			    				arg_count = -1;
								}
		    			} 
		    			else 
		    			{
		    				char tmp;
		    				while (!idx_fp->atEnd())
		    				{
		    					idx_fp->getChar(&tmp);
		    					if (tmp == LFD)
		    						break;
		    				}		    				
								idx_lc++;
								arg_count = -1;
								if (idx_dot)
	    					{
	    						log << "\n";
	    						idx_dot = 0;
	    					}
	    					str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    		                 			"Unknown index keyword %3.\n"))
	    		                 			.arg(idx_fn).arg(idx_lc).arg(keyword);
								log << str;
								idx_ec++;
		    			}
						} 
						else 
						{							
		    			if (i < ARRAY_MAX)
								keyword[i++] = (char) c;
		    			else 
		    			{
								char tmp;
		    				while (!idx_fp->atEnd())
		    				{
		    					idx_fp->getChar(&tmp);
		    					if (tmp == LFD)
		    						break;
		    				}		    				
								idx_lc++;
								arg_count = -1;
								if (idx_dot)
	    					{
	    						log << "\n";
	    						idx_dot = 0;
	    					}
	    					str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    		                 			"Index keyword %3 too long (max 1024).\n"))
	    		                 			.arg(idx_fn).arg(idx_lc).arg(keyword);
								log << str;
								idx_ec++;
		    			}
						}
						break;
					
	    		case 1:
						if (c == idx_aopen) 
						{
		    			arg_count++;
		    			if (!scanArg2()) 
		    			{
								arg_count = -1;
		    			}
						} 
						else 
						{
		    			char tmp;
		    				while (!idx_fp->atEnd())
		    				{
		    					idx_fp->getChar(&tmp);
		    					if (tmp == LFD)
		    						break;
		    				}		    				
								idx_lc++;
								arg_count = -1;
								if (idx_dot)
	    					{
	    						log << "\n";
	    						idx_dot = 0;
	    					}
	    					str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    		                 			"No opening delimiter for second argument (illegal character `%3').\n"))
	    		                 			.arg(idx_fn).arg(idx_lc).arg(c);
								log << str;
								idx_ec++;
						}
						break;
						
	    		case 2:
	    			{
	    				char tmp;
		    				while (!idx_fp->atEnd())
		    				{
		    					idx_fp->getChar(&tmp);
		    					if (tmp == LFD)
		    						break;
		    				}		    				
								idx_lc++;
								arg_count = -1;
								if (idx_dot)
	    					{
	    						log << "\n";
	    						idx_dot = 0;
	    					}
	    					str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    		                 			"No closing delimiter for second argument (illegal character `%3').\n"))
	    		                 			.arg(idx_fn).arg(idx_lc).arg(c);
								log << str;
								idx_ec++;
	    			}
						break;
	    		}
	    		break;
				}
    }

    /* fixup the total counts */
    idx_tt += idx_tc;
    idx_et += idx_ec;
    
    str = QString(tr("done (%1 entries accepted, %2 rejected).\n")).arg(idx_tc - idx_ec).arg(idx_ec);
    term << str;
    log << str;

    idx_fp->close();
    delete idx_fp;
    idx_fp = 0;
}

int XWMakeIndex::scanKey(KFIELD * data)
{
		int     i = 0;		       /* current level */
    int     n = 0;		       /* index to the key[] array */
    int     second_round = 0;
    int     last = FIELD_MAX - 1;
    size_t  len;

    while (1) 
    {
			if (key[n] == NUL)
	    	break;
			len = strlen(key);
			if (key[n] == idx_encap)
			{
	    	n++;
	    	makeString(&(data->encap), len + 1);
	    	if (scanField(&n, data->encap, len, 0, 0, 0))
					break;
	    	else
					return (0);
			}
			if (key[n] == idx_actual) 
			{
	   	 	n++;

	    	makeString(&(data->af[i]), len + 1);
	    	if (i == last)
	    	{
					if (!scanField(&n, data->af[i], len, 0, 1, 0))
		    		return (0);
	    	}
	    	else
	    	{
					if (!scanField(&n, data->af[i], len, 1, 1, 0))
		    		return (0);
	    	}
			} 
			else 
			{
	    	/* Next nesting level */
	    	if (second_round) 
	    	{
					i++;
					n++;
	    	}

	    	makeString(&(data->sf[i]), len + 1);
	    	if (i == last)
	    	{
					if (!scanField(&n, data->sf[i], len, 0, 1, 1))
		    		return (0);
	    	}
	    	else
	    	{
					if (!scanField(&n, data->sf[i], len, 1, 1, 1))
		    		return (0);
	    	}
	    	second_round = 1;
	    	if (german_sort && strchr(data->sf[i], '"'))
	    	{
					makeString(&(data->af[i]),strlen(data->sf[i]) + 1);
					searchQuote(&(data->sf[i]), &(data->af[i]));
	    	}
			}
    }

    /* check for empty fields which shouldn't be empty */
    if (*data->sf[0] == NUL) 
    {
			return 0;
    }
    for (i = 1; i < FIELD_MAX - 1; i++)
   	{
			if ((*data->sf[i] == NUL) && ((*data->af[i] != NUL) || (*data->sf[i + 1] != NUL))) 
			{
	    	return 0;
			}
		}
    	/* i == FIELD_MAX-1 */
    if ((*data->sf[i] == NUL) && (*data->af[i] != NUL)) 
    {
			return 0;
    }
    return (1);
}

int  XWMakeIndex::scanNo(char no[], 
	                       int npg[], 
	                       short *count, 
	                       short *type)
{
	int     i = 1;

    if (isdigit(no[0])) 
    {
			*type = ARAB;
			if (!scanArabic(no, npg, count))
	    	return (0);
			/* simple heuristic to determine if a letter is Roman or Alpha */
    } 
    else if (IS_ROMAN_LOWER(no[0]) && (!IS_COMPOSITOR)) 
    {
			*type = ROML;
			if (!scanRomanLower(no, npg, count))
	    	return (0);
			/* simple heuristic to determine if a letter is Roman or Alpha */
    } 
    else if (IS_ROMAN_UPPER(no[0]) && ((no[0] == ROMAN_I) || (!IS_COMPOSITOR))) 
    {
			*type = ROMU;
			if (!scanRomanUpper(no, npg, count))
	    	return (0);
    } 
    else if (IS_ALPHA_LOWER(no[0])) 
    {
			*type = ALPL;
			if (!scanAlphaLower(no, npg, count))
	    	return (0);
    } 
    else if (IS_ALPHA_UPPER(no[0])) 
    {
			*type = ALPU;
			if (!scanAlphaUpper(no, npg, count))
	    	return (0);
    } 
    else 
    {
			QTextStream log(ilg_fp);
    	if (idx_dot)
	    {
	    	log << "\n";
	    	idx_dot = 0;
	    }
	    QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	                			"Illegal page number %3.\n"))
	                 			.arg(idx_fn).arg(idx_lc).arg(no);
			log << str;
			idx_ec++;
			return (0);
    }
    return (1);
}

int  XWMakeIndex::scanRomanLower(char no[], 
	                               int npg[], 
	                               short *count)
{
	short   i = 0;
    int     inp = 0;
    int     prev = 0;
    int     the_new;
    ;
	QTextStream log(ilg_fp);

    while ((no[i] != NUL) && (i < ROMAN_MAX) && (!IS_COMPOSITOR)) 
    {
			if ((IS_ROMAN_LOWER(no[i])) && ((the_new = ROMAN_LOWER_VAL(no[i])) != 0)) 
			{
	    	if (prev == 0)
					prev = the_new;
	    	else 
	    	{
					if (prev < the_new) 
					{
		    		prev = the_new - prev;
		    		the_new = 0;
					}
					inp += prev;
					prev = the_new;
	    	}
			} 
			else 
			{
				if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	                			"Illegal Roman number: position %3 in %4.\n"))
	                 			.arg(idx_fn).arg(idx_lc).arg(i + 1).arg(no);
				log << str;
				idx_ec++;
	    	return (0);
			}
			i++;
    }
    if (i == ROMAN_MAX) 
    {
    	if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	                			"Roman page number %3 too big (max %4 digits).\n"))
	                 			.arg(idx_fn).arg(idx_lc).arg(no).arg(ROMAN_MAX);
				log << str;
				idx_ec++;
			return (0);
    }
    inp += prev;

		if (*count >= PAGEFIELD_MAX)
		{
			if (idx_dot)
	  	{
	  		log << "\n";
	  		idx_dot = 0;
	  	}
	  	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              			"Page number %3 has too many fields (max. %4)."))
	    	               			.arg(idx_fn).arg(idx_lc).arg(no).arg(PAGEFIELD_MAX);
			log << str;
			idx_ec++;
	  	return (0);
		}
    npg[*count] = (inp + page_offset[ROML]);
    ++*count;

    if (IS_COMPOSITOR)
			return (scanNo(&no[i + comp_len], npg, count, &i));
    else
			return (1);
}

int XWMakeIndex::scanRomanUpper(char no[], 
	                              int npg[], 
	                              short *count)
{
	short   i = 0;
    int     inp = 0;
    int     prev = 0;
    int     the_new;

	QTextStream log(ilg_fp);
	
    while ((no[i] != NUL) && (i < ROMAN_MAX) && (!IS_COMPOSITOR)) 
    {
			if ((IS_ROMAN_UPPER(no[i])) && ((the_new = ROMAN_UPPER_VAL(no[i])) != 0)) 
			{
	    	if (prev == 0)
					prev = the_new;
	    	else 
	    	{
					if (prev < the_new) 
					{
		    		prev = the_new - prev;
		    		the_new = 0;
					}
					inp += prev;
					prev = the_new;
	    	}
			} 
			else 
			{
				if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	                			"Illegal Roman number: position %3 in %4.\n"))
	                 			.arg(idx_fn).arg(idx_lc).arg(i + 1).arg(no);
				log << str;
				idx_ec++;
	    	return (0);
			}
			i++;
    }
    if (i == ROMAN_MAX) 
    {
    	if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	                			"Roman page number %3 too big (max %4 digits).\n"))
	                 			.arg(idx_fn).arg(idx_lc).arg(no).arg(ROMAN_MAX);
				log << str;
				idx_ec++;
			return (0);
    }
    inp += prev;

		if (*count >= PAGEFIELD_MAX)
		{
			if (idx_dot)
	  	{
	  		log << "\n";
	  		idx_dot = 0;
	  	}
	  	QString str = QString(tr("!! Input index error (file = %1, line = %2):\n   -- "
	    	              			"Page number %3 has too many fields (max. %4)."))
	    	               			.arg(idx_fn).arg(idx_lc).arg(no).arg(PAGEFIELD_MAX);
			log << str;
			idx_ec++;
	  	return (0);
		}
    npg[*count] = (inp + page_offset[ROMU]);
    ++*count;

    if (IS_COMPOSITOR)
			return (scanNo(&no[i + comp_len], npg, count, &i));
    else
			return (1);
}

int  XWMakeIndex::scanSpec(char spec[])
{
	int     i = 0;
    char     c;
	QTextStream log(ilg_fp);
    while (1)
    {
			if ((c = nextNonBlank()) == -1)
	    	return (0);
			else if (c == COMMENT) 
			{
	    	char a;
	    	while (!sty_fp->atEnd())
	    	{
	    		sty_fp->getChar(&a);
	    		if (a == LFD)
	    			break;
	    	}
	    	sty_lc++;
			} 
			else
	    	break;
	  }

    spec[0] = TOLOWER(c);
    while ((i++ < STRING_MAX) && 
           !sty_fp->atEnd() && 
    				sty_fp->getChar(&c) && 
          (c != SPC) && 
          (c != TAB) && 
          (c != LFD) && (c != EOF))
    {
			spec[i] = TOLOWER(c);
		}
	
    if (i < STRING_MAX) 
    {
			spec[i] = NUL;
			if (sty_fp->atEnd()) 
			{
				if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	
	    	QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 "No attribute for specifier %3 (premature EOF)\n"))
	    	                  .arg(sty_fn).arg(sty_lc).arg(spec);
				log << str;
				sty_ec++;
				put_dot = 0;
	    	return (-1);
			}
			
			if (c == LFD)
	    	sty_lc++;
			return (1);
    } 
    else 
    {
    	if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	
	    	QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 "Specifier %3 too long (max %4).\n"))
	    	                  .arg(sty_fn).arg(sty_lc).arg(spec).arg(STRING_MAX);
				log << str;
				sty_ec++;
				put_dot = 0;
			return (0);
    }
}

int  XWMakeIndex::scanString(char str[])
{
	char    clone[ARRAY_MAX];
    int     i = 0;
    char     c;
		QTextStream log(ilg_fp);
    switch (c = (char)nextNonBlank()) 
    {
    	case STR_DELIM:
				while (!sty_fp->atEnd())
				{
					sty_fp->getChar(&c);
	    		switch (c) 
	    		{
	    			case EOF:
	    				{
	    					if (idx_dot)
	    					{
	    						log << "\n";
	    						idx_dot = 0;
	    					}
	    	
	    					QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 					"No closing delimiter in %3.\n"))
	    	                  				.arg(sty_fn).arg(sty_lc).arg(clone);
								log << str;
								sty_ec++;
								put_dot = 0;
	    				}
							return (0);
							
	    			case STR_DELIM:
							clone[i] = NUL;
							strcpy(str, clone);
							return (1);
							
	    			case BSH:
	    				sty_fp->getChar(&c);
							switch (c) 
							{
								case 't':
		    					clone[i++] = TAB;
		    					break;
								case 'n':
		    					clone[i++] = LFD;
		    					break;

								default:
		    					clone[i++] = (char) c;
		    					break;
							}
							break;
							
	    			default:
							if (c == LFD)
		    				sty_lc++;
							if (i < ARRAY_MAX)
		    				clone[i++] = (char) c;
							else 
							{
		    				char a;
	    					while (!sty_fp->atEnd())
	    					{
	    						sty_fp->getChar(&a);
	    						if (a == LFD)
	    							break;
	    					}
	    					sty_lc++;
	    					if (idx_dot)
	    					{
	    						log << "\n";
	    						idx_dot = 0;
	    					}
	    	
	    					QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 					"Attribute string %3 too long (max %4).\n"))
	    	                  				.arg(sty_fn).arg(sty_lc).arg(clone).arg(ARRAY_MAX);
								log << str;
								sty_ec++;
								put_dot = 0;
		    				return (0);
							}
							break;
	    			}
	    		}
					break;
						
    		case COMMENT:
					{
						char a;
	    			while (!sty_fp->atEnd())
	    			{
	    				sty_fp->getChar(&a);
	    				if (a == LFD)
	    					break;
	    			}
	    			sty_lc++;
					}
					break;
						
    		default:
    			{
    				char a;
    				while (!sty_fp->atEnd())
	    			{
	    				sty_fp->getChar(&a);
	    				if (a == LFD)
	    					break;
	    			}
	    			sty_lc++;
    			}
    			{
    				if (idx_dot)
	    			{
	    				log << "\n";
	    				idx_dot = 0;
	    			}
	    	
	    			QString str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                					"No opening delimiter.\n"))
	    	                 				.arg(sty_fn).arg(sty_lc);
						log << str;
						sty_ec++;
						put_dot = 0;
    			}
					return (0);
    }
    return (1); 
}

void XWMakeIndex::scanSty()
{
	QTextStream term(stderr);
	QTextStream log(ilg_fp);
	QString str = QString(tr("Scanning style file %1...\n")).arg(sty_fn);
	term << str;
	log << str;
	
	char    spec[STRING_MAX];
    int     tmp;
	
	while (scanSpec(spec))
	{
		sty_tc++;
		put_dot = 1;
		
		/* output pre- and post-ambles */
		if (0 == strcmp(spec, PREAMBLE)) 
		{
	    (void) scanString(preamble);
	    prelen = countLfd(preamble);
		} 
		else if (0 == strcmp(spec, POSTAMBLE)) 
		{
	    (void) scanString(postamble);
	    postlen = countLfd(postamble);
		} 
		else if (0 == strcmp(spec, GROUP_SKIP)) 
		{
	    (void) scanString(group_skip);
	    skiplen = countLfd(group_skip);
		} 
		else if (0 == strcmp(spec, HEADINGS_FLAG)) 
		{
			QTextStream fstream(sty_fp);
			fstream >> headings_flag;
		} 
		else if (0 == strcmp(spec, HEADING_PRE)) 
		{
	    (void) scanString(heading_pre);
	    headprelen = countLfd(heading_pre);
		} 
		else if (0 == strcmp(spec, HEADING_SUF)) 
		{
	    (void) scanString(heading_suf);
	    headsuflen = countLfd(heading_suf);
		} 
		else if (0 == strcmp(spec, SYMHEAD_POS)) 
		{
	    (void) scanString(symhead_pos);
		} 
		else if (0 == strcmp(spec, SYMHEAD_NEG)) 
		{
	    (void) scanString(symhead_neg);
		} 
		else if (0 == strcmp(spec, NUMHEAD_POS)) 
		{
	    (void) scanString(numhead_pos);
		} 
		else if (0 == strcmp(spec, NUMHEAD_NEG)) 
		{
	    (void) scanString(numhead_neg);
		} 
		else if (0 == strcmp(spec, SETPAGEOPEN)) 
		{
	    (void) scanString(setpage_open);
	    setpagelen = countLfd(setpage_open);
		} 
		else if (0 == strcmp(spec, SETPAGECLOSE)) 
		{
	    (void) scanString(setpage_close);
	    setpagelen = countLfd(setpage_close);
	    /* output index item commands */
		} 
		else if (0 == strcmp(spec, ITEM_0)) 
		{
	    (void) scanString(item_r[0]);
	    ilen_r[0] = countLfd(item_r[0]);
		} 
		else if (0 == strcmp(spec, ITEM_1)) 
		{
	    (void) scanString(item_r[1]);
	    ilen_r[1] = countLfd(item_r[1]);
		} 
		else if (0 == strcmp(spec, ITEM_2)) 
		{
	    (void) scanString(item_r[2]);
	    ilen_r[2] = countLfd(item_r[2]);
		} 
		else if (0 == strcmp(spec, ITEM_01)) 
		{
	    (void) scanString(item_u[1]);
	    ilen_u[1] = countLfd(item_u[1]);
		} 
		else if (0 == strcmp(spec, ITEM_12)) 
		{
	    (void) scanString(item_u[2]);
	    ilen_u[2] = countLfd(item_u[2]);
		} 
		else if (0 == strcmp(spec, ITEM_x1)) 
		{
	    (void) scanString(item_x[1]);
	    ilen_x[1] = countLfd(item_x[1]);
		} 
		else if (0 == strcmp(spec, ITEM_x2)) 
		{
	    (void) scanString(item_x[2]);
	    ilen_x[2] = countLfd(item_x[2]);
	    /* output encapsulators */
		} 
		else if (0 == strcmp(spec, ENCAP_0))
	    (void) scanString(encap_p);
		else if (0 == strcmp(spec, ENCAP_1))
	    (void) scanString(encap_i);
		else if (0 == strcmp(spec, ENCAP_2))
	    (void) scanString(encap_s);
			/* output delimiters */
		else if (0 == strcmp(spec, DELIM_0))
	    (void) scanString(delim_p[0]);
		else if (0 == strcmp(spec, DELIM_1))
	    (void) scanString(delim_p[1]);
		else if (0 == strcmp(spec, DELIM_2))
	    (void) scanString(delim_p[2]);
		else if (0 == strcmp(spec, DELIM_N))
	    (void) scanString(delim_n);
		else if (0 == strcmp(spec, DELIM_R))
	    (void) scanString(delim_r);
		else if (0 == strcmp(spec, DELIM_T))
	    (void) scanString(delim_t);
		else if (0 == strcmp(spec, SUFFIX_2P))
	    (void) scanString(suffix_2p);
		else if (0 == strcmp(spec, SUFFIX_3P))
	    (void) scanString(suffix_3p);
		else if (0 == strcmp(spec, SUFFIX_MP))
	    (void) scanString(suffix_mp);
	/* output line width */
		else if (0 == strcmp(spec, LINEMAX)) 
		{
			QTextStream fstream(sty_fp);
			fstream >> tmp;
			
	    if (tmp > 0)
				linemax = tmp;
	    else
	    {
	    	if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	
	    	str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 "%3 must be positive (got %4)"))
	    	                  .arg(sty_fn).arg(sty_lc).arg(LINEMAX).arg(tmp);
				log << str;
				sty_ec++;
				put_dot = 0;
			}
	    /* output line indentation length */
		} 
		else if (0 == strcmp(spec, INDENT_LENGTH)) 
		{
	    QTextStream fstream(sty_fp);
			fstream >> tmp;
	    if (tmp >= 0)
				indent_length = tmp;
	    else
	    {
	    	if (idx_dot)
	    	{
	    		log << "\n";
	    		idx_dot = 0;
	    	}
	    	
	    	str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	    	                 "%3 must be nonnegative (got %4)"))
	    	                  .arg(sty_fn).arg(sty_lc).arg(INDENT_LENGTH).arg(tmp);
				log << str;
				sty_ec++;
				put_dot = 0;
	    }
	    /* output line indentation */
		} 
		else if (0 == strcmp(spec, INDENT_SPACE)) 
		{
	    (void) scanString(indent_space);
	    /* composite page delimiter */
		} 
		else if (0 == strcmp(spec, COMPOSITOR)) 
		{
	    (void) scanString(page_comp);
	    /* page precedence */
		} 
		else if (0 == strcmp(spec, PRECEDENCE)) 
		{
	    (void) scanString(page_prec);
	    (void) processPrecedence();
	    /* index input format */
		} 
		else if (0 == strcmp(spec, KEYWORD))
	    (void) scanString(idx_keyword);
		else if (0 == strcmp(spec, AOPEN))
	    (void) scanChar(&idx_aopen);
		else if (0 == strcmp(spec, ACLOSE))
	    (void) scanChar(&idx_aclose);
		else if (0 == strcmp(spec, LEVEL))
	    (void) scanChar(&idx_level);
		else if (0 == strcmp(spec, ROPEN))
	    (void) scanChar(&idx_ropen);
		else if (0 == strcmp(spec, RCLOSE))
	    (void) scanChar(&idx_rclose);
		else if (0 == strcmp(spec, QUOTE))
	    (void) scanChar(&idx_quote);
		else if (0 == strcmp(spec, ACTUAL))
	    (void) scanChar(&idx_actual);
		else if (0 == strcmp(spec, ENCAP))
	    (void) scanChar(&idx_encap);
		else if (0 == strcmp(spec, ESCAPE))
	    (void) scanChar(&idx_escape);
		else 
		{
	    (void) nextNonBlank();
	    char a;
	    while (!sty_fp->atEnd())
	    {
	    	sty_fp->getChar(&a);
	    	if (a == LFD)
	    		break;
	    }
	    sty_lc++;
	    if (idx_dot)
	    {
	    	log << "\n";
	    	idx_dot = 0;
	    }
	    	
	    str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	                     "Unknown specifier %3.\n"))
	                      .arg(sty_fn).arg(sty_lc).arg(spec);
			log << str;
			sty_ec++;
			put_dot = 0;
		}
		if (put_dot) 
		{
			idx_dot = 1;
			if (verbose)
				term << DOT;
			log << DOT;
		}
	}
	
	if (idx_quote == idx_escape) 
	{
		if (idx_dot)
	  {
	   	log << "\n";
	   	idx_dot = 0;
	  }
	    	
	  str = QString(tr("** Input style error (file = %1, line = %2):\n   -- "
	                   "Quote and escape symbols must be distinct (both `%3' now).\n"))
	                   .arg(sty_fn).arg(sty_lc).arg(idx_quote);
		log << str;
		sty_ec++;
		put_dot = 0;
		idx_quote = IDX_QUOTE;
		idx_escape = IDX_ESCAPE;
  }
  
  str = QString(tr("done (%1 ttributes redefined, %2 ignored).\n")).arg(sty_ec).arg(sty_ec);
  if (verbose)
  	term << str;
  log << str;
  sty_fp->close();
  delete sty_fp;
  sty_fp = 0;
}

void XWMakeIndex::searchQuote(char **sort_key, char **actual_key)
{
	char   *ptr;		       /* pointer to sort_key */
    char   *sort;		       /* contains sorting text */
    int     char_found = FALSE;

    strcpy(*actual_key, *sort_key);
    ptr = strchr(*sort_key, '"');       /* look for first double quote */
    while (ptr != (char*)NULL)
    {
			sort = NULL;
			switch (*(ptr + 1))
			{				/* skip to umlaut or sharp S */
				case 'a':
				case 'A':
	    		sort = isupper(*(ptr + 1)) ? "Ae" : "ae";
	    		break;
	    		
				case 'o':
				case 'O':
	    		sort = isupper(*(ptr + 1)) ? "Oe" : "oe";
	    		break;
	    		
				case 'u':
				case 'U':
	    		sort = isupper(*(ptr + 1)) ? "Ue" : "ue";
	    		break;
	    		
				case 's':
	    		sort = "ss";
	    		break;
	    		
				default:
	    		break;
			}
			if (sort != NULL)
			{
	    	char_found = TRUE;
	    	*ptr = sort[0];	       /* write to sort_key */
	    	*(ptr + 1) = sort[1];
			}
			ptr = strchr(ptr + 1, '"');    /* next double quote */
    }
    if (!char_found)		       /* reset actual_key ? */
			makeString(actual_key, 1);
    return;
}

void XWMakeIndex::sortIdx()
{
	QTextStream term(stderr);
	QTextStream log(ilg_fp);
	QString str = QString(tr("Sorting entries...\n"));
	term << str;
	log << str;
	
	idx_dc = 0;
    idx_gc = 0L;
    qqsort((char *) idx_key, (int) idx_gt);
    str = QString(tr("done (%1 comparisons).\n")).arg(idx_gc);
	term << str;
	log << str;
}

int XWMakeIndex::strtoint(char *str)
{
	int     val = 0;

    while (*str != NUL) 
    {
			val = 10 * val + *str - 48;
			str++;
    }
    return (val);
}

void XWMakeIndex::wrapLine(int print)
{
	int len = strlen(line) + strlen(buff) + ind_indent;
  if (print) 
  {
		if (len > linemax) 
		{
	    ind_fp->write(line);
	    ind_fp->putChar('\n');
	    ind_lc++;
	    ind_fp->write(indent_space);
	    ind_indent = indent_length;
		} 
		else
	    ind_fp->write(line);
		ind_fp->write(buff);
   } 
   else 
   {
			if (len > linemax) 
			{
	    	ind_fp->write(line);
	    	ind_fp->putChar('\n');
	    	ind_lc++;
	    	sprintf(line, "%s%s%s", indent_space, buff, delim_n);
	    	ind_indent = indent_length;
			} 
			else 
			{
	    	strcat(buff, delim_n);
	    	strcat(line, buff);
			}
   }
}
