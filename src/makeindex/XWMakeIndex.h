/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWMAKEINDEX_H
#define XWMAKEINDEX_H

#include <QStringList>
#include "XWApplication.h"

#define TOLOWER(C) (isupper((unsigned char)(C)) ? \
	(unsigned char)tolower((unsigned char)(C)) : (unsigned char)(C))
#define TOUPPER(C) (isupper((unsigned char)(C)) ? \
	(unsigned char)(C) : (unsigned char)toupper((unsigned char)(C)))
		
#define TOASCII(i) (char)((i) + 48)

#define ARABIC_MAX    10
#define FIELD_MAX	    3
#define LINE_MAX      72
#define STRING_MAX    256
#define NUMBER_MAX    16
#define ARRAY_MAX     1024
#define ARGUMENT_MAX  1024
#define PAGETYPE_MAX  5
#define PAGEFIELD_MAX 10
#define ROMAN_MAX     16

#define INDEX_IDX  ".idx"
#define INDEX_ILG  ".ilg"
#define INDEX_IND  ".ind"
#define INDEX_STY  ".mst"
#define INDEX_LOG  ".log"


#undef NUL
#define NUL '\0'

#undef NIL
#define NIL ""


#define TAB '\t'
#define LFD '\n'
#define SPC ' '
#define LSQ '['
#define RSQ ']'
#define BSH '\\'


#ifdef Q_OS_WIN
#define ENV_SEPAR ';'
#define DIR_DELIM '/'
#define ALT_DIR_DELIM '\\'
#define IS_DIR_DELIM(c) ((c) == DIR_DELIM || (c) == ALT_DIR_DELIM)
#else
#define ENV_SEPAR ':'
#define DIR_DELIM '/'
#define IS_DIR_DELIM(c) ((c) == DIR_DELIM)
#endif

#define EXT_DELIM  '.'
#define ROMAN_SIGN '*'
#define EVEN       "even"
#define ODD        "odd"
#define ANY        "any"


#define DOT     "."
#define DOT_MAX 1000
#define CMP_MAX 1500


#define ISDIGIT(C)  ('0' <= C && C <= '9')
#define ISSYMBOL(C) (('!' <= C && C <= '@') || \
		     ('[' <= C && C <= '`') || \
		     ('{' <= C && C <= '~'))

#define EMPTY     -9999
#define ROML      0
#define ROMU      1
#define ARAB      2
#define ALPL      3
#define ALPU      4
#define DUPLICATE 9999

#define SYMBOL -1
#define ALPHA  -2

#define GERMAN 0


#define IS_COMPOSITOR (strncmp(&no[i], page_comp, comp_len) == 0)

#define ROMAN_I 'I'
#define ROMAN_V 'V'
#define ROMAN_X 'X'
#define ROMAN_L 'L'
#define ROMAN_C 'C'
#define ROMAN_D 'D'
#define ROMAN_M 'M'

#define ROMAN_i 'i'
#define ROMAN_v 'v'
#define ROMAN_x 'x'
#define ROMAN_l 'l'
#define ROMAN_c 'c'
#define ROMAN_d 'd'
#define ROMAN_m 'm'

#define VAL_I 1
#define VAL_V 5
#define VAL_X 10
#define VAL_L 50
#define VAL_C 100
#define VAL_D 500
#define VAL_M 1000

#define ROMAN_LOWER_VAL(C) \
    ((C == ROMAN_i) ? VAL_I : \
     (C == ROMAN_v) ? VAL_V : \
     (C == ROMAN_x) ? VAL_X : \
     (C == ROMAN_l) ? VAL_L : \
     (C == ROMAN_c) ? VAL_C : \
     (C == ROMAN_d) ? VAL_D : \
     (C == ROMAN_m) ? VAL_M : 0)

#define ROMAN_UPPER_VAL(C) \
    ((C == ROMAN_I) ? VAL_I : \
     (C == ROMAN_V) ? VAL_V : \
     (C == ROMAN_X) ? VAL_X : \
     (C == ROMAN_L) ? VAL_L : \
     (C == ROMAN_C) ? VAL_C : \
     (C == ROMAN_D) ? VAL_D : \
     (C == ROMAN_M) ? VAL_M : 0)

#define IS_ROMAN_LOWER(C) \
    ((C == ROMAN_i) || (C == ROMAN_v) || (C == ROMAN_x) || \
     (C == ROMAN_l) || (C == ROMAN_c) || (C == ROMAN_d) || (C == ROMAN_m))

#define IS_ROMAN_UPPER(C) \
    ((C == ROMAN_I) || (C == ROMAN_V) || (C == ROMAN_X) || \
     (C == ROMAN_L) || (C == ROMAN_C) || (C == ROMAN_D) || (C == ROMAN_M))

#define ALPHA_VAL(C) \
    ((('A' <= C) && (C <= 'Z')) ? C - 'A' : \
     (('a' <= C) && (C <= 'z')) ? C - 'a' : 0)

#define IS_ALPHA_LOWER(C) \
    (('a' <= C) && (C <= 'z'))

#define IS_ALPHA_UPPER(C) \
    (('A' <= C) && (C <= 'Z'))

#define COMMENT   '%'
#define STR_DELIM '"'
#define CHR_DELIM '\''

#define KEYWORD "keyword"
#define AOPEN   "arg_open"
#define ACLOSE  "arg_close"
#define ROPEN   "range_open"
#define RCLOSE  "range_close"
#define LEVEL   "level"
#define QUOTE   "quote"
#define ACTUAL  "actual"
#define ENCAP   "encap"
#define ESCAPE  "escape"

#define IDX_KEYWORD "\\indexentry"
#define IDX_AOPEN   '{'
#define IDX_ACLOSE  '}'
#define IDX_ROPEN   '('
#define IDX_RCLOSE  ')'
#define IDX_LEVEL   '!'
#define IDX_QUOTE   '"'
#define IDX_ACTUAL  '@'
#define IDX_ENCAP   '|'
#define IDX_ESCAPE  '\\'

#define COMPOSITOR         "page_compositor"
#define COMPOSITOR_DEF     "-"
#define PRECEDENCE         "page_precedence"
#define PRECEDENCE_DEF     "rnaRA"
#define ROMAN_LOWER        'r'
#define ROMAN_UPPER        'R'
#define ARABIC             'n'
#define ALPHA_LOWER        'a'
#define ALPHA_UPPER        'A'
#define ROMAN_LOWER_OFFSET 10000
#define ROMAN_UPPER_OFFSET 10000
#define ARABIC_OFFSET      10000
#define ALPHA_LOWER_OFFSET 26
#define ALPHA_UPPER_OFFSET 26

#define PREAMBLE      "preamble"
#define PREAMBLE_DEF  "\\begin{theindex}\n"
#define PREAMBLE_LEN  1
#define POSTAMBLE     "postamble"
#define POSTAMBLE_DEF "\n\n\\end{theindex}\n"
#define POSTAMBLE_LEN 3

#define SETPAGEOPEN  "setpage_prefix"
#define SETPAGECLOSE "setpage_suffix"

#define SETPAGEOPEN_DEF "\n  \\setcounter{page}{"

#define SETPAGECLOSE_DEF "}\n"
#define SETPAGE_LEN      2

#define GROUP_SKIP        "group_skip"
#define GROUPSKIP_DEF "\n\n  \\indexspace\n"
#define GROUPSKIP_LEN 3

#define HEADINGS_FLAG    "headings_flag"
#define HEADINGSFLAG_DEF 0
#define HEADING_PRE      "heading_prefix"
#define HEADINGPRE_DEF   ""
#define HEADINGPRE_LEN   0
#define HEADING_SUF      "heading_suffix"
#define HEADINGSUF_DEF   ""
#define HEADINGSUF_LEN   0
#define SYMHEAD_POS      "symhead_positive"
#define SYMHEADPOS_DEF   "Symbols"
#define SYMHEAD_NEG      "symhead_negative"
#define SYMHEADNEG_DEF   "symbols"
#define NUMHEAD_POS      "numhead_positive"
#define NUMHEADPOS_DEF   "Numbers"
#define NUMHEAD_NEG      "numhead_negative"
#define NUMHEADNEG_DEF   "numbers"

#define ITEM_0  "item_0"
#define ITEM_1  "item_1"
#define ITEM_2  "item_2"
#define ITEM_01 "item_01"
#define ITEM_x1 "item_x1"
#define ITEM_12 "item_12"
#define ITEM_x2 "item_x2"

#define ITEM0_DEF "\n  \\item "
#define ITEM1_DEF "\n    \\subitem "
#define ITEM2_DEF "\n      \\subsubitem "

#define ITEM_LEN 1

#define DELIM_0    "delim_0"
#define DELIM_1    "delim_1"
#define DELIM_2    "delim_2"
#define DELIM_N    "delim_n"
#define DELIM_R    "delim_r"
#define DELIM_T    "delim_t"
#define DELIM_DEF  ", "
#define DELIMR_DEF "--"
#define DELIMT_DEF ""

#define SUFFIX_2P  "suffix_2p"
#define SUFFIX_3P  "suffix_3p"
#define SUFFIX_MP  "suffix_mp"

#define ENCAP_0    "encap_prefix"
#define ENCAP_1    "encap_infix"
#define ENCAP_2    "encap_suffix"
#define ENCAP0_DEF "\\"
#define ENCAP1_DEF "{"
#define ENCAP2_DEF "}"

#define LINEMAX       "line_max"
#define INDENT_SPACE  "indent_space"
#define INDENT_LENGTH "indent_length"

#define INDENTSPC_DEF "\t\t"

#define INDENTLEN_DEF 16

#define THRESH  4	
#define MTHRESH 6		

struct KFIELD
{
	char    *sf[FIELD_MAX];		/* sort key */
  char    *af[FIELD_MAX];		/* actual key */
  int     group;			/* key group */
  char    lpg[NUMBER_MAX];		/* literal page */
  int     npg[PAGEFIELD_MAX];		/* page field array */
  short   count;			/* page field count */
  short   type;			/* page number type */
  char    *encap;			/* encapsulator */
  char    *fn;			/* input filename */
  int     lc;				/* line number */
};
	
struct KNODE
{
	KFIELD   data;
  struct KNODE *next;
};

class XWMakeIndex : public XWApplication
{
	Q_OBJECT
	
public:       
    XWMakeIndex(QCoreApplication * app);
    ~XWMakeIndex(); 
    
    QString      getProductID();
    QString      getProductName();
    const char * getProductName8();
		QString      getVersion();
		const char * getVersion8();
	
		void start(int & argc, char**argv);
		
protected:
	void cleanup();
		
private:
	void checkAll(char *fn, 
	              int ind_given, 
	              int ilg_given, 
	              int log_given);
	void checkIdx(char *fn, int open_fn);
	int  checkMixsym(char *x, char *y);
	int  compare(KFIELD **a, KFIELD **b);
	int  compareOne(char *x,char *y);
	int  comparePage(KFIELD **a, KFIELD **b);
	int  compareString(unsigned char *a, unsigned char *b);
	int  countLfd(char *str);
	
	void  findPageNo();
	uchar firstLetter(char *term);
	void  flushLine(int print);
	void  flushToEol();
	
	void genInd();
	int  groupType(char *str);
	
	void insertPage();
	
	int  makeEntry(int n);
	void makeItem(char *iterm);
	int  makeKey();
	void makeString(char **ppstr, int n);
	
	void newEntry();
	int  newStrCmp(unsigned char *s1, unsigned char *s2, int option);
	int  nextNonBlank();
	
	void oldEntry();
	void openSty(char *fn);
	
	int  pageDiff(KFIELD * a,KFIELD * b);
	void prepareIdx();
	void processIdx(char *fn[], 
	                int use_stdin, 
	                int sty_given, 
	                int ind_given,
	                int ilg_given, 
	                int log_given);
	int  processPrecedence();
	void putHeader(int let);
	
	void qqsort(char *base, int n);
	void qst(char *base, char *max);
	
	int  scanAlphaLower(char no[], 
	                    int npg[], 
	                    short *count);
	int  scanAlphaUpper(char no[], 
	                    int npg[], 
	                    short *count);
	int  scanArabic(char no[], 
	                int npg[], 
	                short *count);
	int  scanArg1();
	int  scanArg2();
	int  scanChar(char *c);
	int  scanField(int *n, 
	               char field[], 
	               int len_field, 
	               int ck_level, 
	               int ck_encap, 
	               int ck_actual);
	void scanIdx();
	int  scanKey(KFIELD * data);
	int  scanNo(char no[], 
	             int npg[], 
	             short *count, 
	             short *type);
	int  scanRomanLower(char no[], 
	                    int npg[], 
	                    short *count);
	int  scanRomanUpper(char no[], 
	                    int npg[], 
	                    short *count);
	int  scanSpec(char spec[]);
	int  scanString(char str[]);
	void scanSty();
	void searchQuote(char **sort_key, char **actual_key);
	void sortIdx();
	int  strtoint(char *str);
	
	void wrapLine(int print);
	
private:
	int     letter_ordering;
	int     compress_blanks;
	int     merge_page;
	int     init_page;
	int     even_odd;
	int     verbose;
	int     german_sort;
	int     thai_sort;
	int     locale_sort;
	
	char idx_keyword[ARRAY_MAX];
	char idx_aopen;
	char idx_aclose;
	char idx_level;
	char idx_ropen;
	char idx_rclose;
	char idx_quote;
	char idx_actual;
	char idx_encap;
	char idx_escape;
	
	char page_comp[ARRAY_MAX];
	
	char preamble[ARRAY_MAX];
	char postamble[ARRAY_MAX];
	char setpage_open[ARRAY_MAX];
	char setpage_close[ARRAY_MAX];
	char group_skip[ARRAY_MAX];
	int headings_flag;
	char heading_pre[ARRAY_MAX];
	char heading_suf[ARRAY_MAX];
	char symhead_pos[ARRAY_MAX];
	char symhead_neg[ARRAY_MAX];
	char numhead_pos[ARRAY_MAX];
	char numhead_neg[ARRAY_MAX];
	int prelen;
	int postlen;
	int skiplen;
	int headprelen;
	int headsuflen;
	int setpagelen;

	char item_r[FIELD_MAX][ARRAY_MAX];
	char item_u[FIELD_MAX][ARRAY_MAX];
	char item_x[FIELD_MAX][ARRAY_MAX];
	int ilen_r[FIELD_MAX];
	int ilen_u[FIELD_MAX];
	int ilen_x[FIELD_MAX];

	char delim_p[FIELD_MAX][ARRAY_MAX];
	char delim_n[ARRAY_MAX];
	char delim_r[ARRAY_MAX];
	char delim_t[ARRAY_MAX];

	char suffix_2p[ARRAY_MAX];
	char suffix_3p[ARRAY_MAX];
	char suffix_mp[ARRAY_MAX];

	char encap_p[ARRAY_MAX];
	char encap_i[ARRAY_MAX];
	char encap_s[ARRAY_MAX];

	int linemax;
	char indent_space[ARRAY_MAX];
	int indent_length;
	
	char   *pgm_fn;
	char    sty_fn[LINE_MAX];
	char   *idx_fn;
	char    ind[STRING_MAX];
	char   *ind_fn;
	char    ilg[STRING_MAX];
	char   *ilg_fn;
	char    pageno[NUMBER_MAX];
	
	char log_fn[STRING_MAX];
	char base[STRING_MAX];
	int  need_version;

	int     fn_no;		       /* total number of files */
	int     idx_dot;		     /* flag which shows dot in ilg being * active */
	int     idx_tt;		       /* total entry count (all files) */
	int     idx_et;		       /* erroneous entry count (all files) */
	int     idx_gt;
	int     idx_dc;
		
	KFIELD ** idx_key;
	KNODE  * head;
	KNODE  * tail;
	
	QFile   *log_fp;
	QFile   *sty_fp;
	QFile   *idx_fp;
	QFile   *ind_fp;
	QFile   *ilg_fp;
		
	KFIELD * curr;
	KFIELD * prev;
	KFIELD * begin;
	KFIELD * the_end;
	KFIELD * range_ptr;
	int level;
	int prev_level;
	char *encap;
	char *prev_encap;
	int in_range;
	int encap_range;
	char buff[2 * ARGUMENT_MAX];
	char line[2 * ARGUMENT_MAX];	/* output buffer */
	int ind_lc;			/* overall line count */
	int ind_ec;			/* erroneous line count */
	int ind_indent;
	
	int first_entry;
	int comp_len;
	char key[ARGUMENT_MAX];
	char no[NUMBER_MAX];
	
	char page_prec[ARRAY_MAX];
	int put_dot;
	
	int  idx_lc;
	int  idx_tc;
	int  idx_ec;
	long idx_gc;
	
	int sty_lc;                 /* line count */
	int sty_tc;                 /* total count */
	int sty_ec;
	
	int qsz;			       /* size of each record */
	int thresh;		       /* THRESHold in chars */
	int mthresh;	
//	int	(*qcmp)(char*,char*);
	
	QString outputPath;
};

#endif //XWMAKEINDEX_H
