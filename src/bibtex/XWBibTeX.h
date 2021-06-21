/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWBIBTEX_H
#define XWBIBTEX_H

#include <QStringList>
#include "XWApplication.h"

#define XW_BIBTEX_ANY_VALUE 0

#define XW_BIBTEX_EMPTY 0
#define XW_BIBTEX_MISSING  XW_BIBTEX_EMPTY

#define XW_BIBTEX_SPOTLESS        0 
#define XW_BIBTEX_WARNING_MESSAGE 1 
#define XW_BIBTEX_ERROR_MESSAGE   2 
#define XW_BIBTEX_FATAL_MESSAGE   3

#define XW_BIBTEX_FIRST_TEXT_CHAR 0
#define XW_BIBTEX_LAST_TEXT_CHAR  127

#define XW_BIBTEX_HASH_PRIME 30011 
#define XW_BIBTEX_HASH_SIZE  35307L 
#define XW_BIBTEX_HASH_BASE  1 
#define XW_BIBTEX_HASH_MAX  (XW_BIBTEX_HASH_BASE + XW_BIBTEX_HASH_SIZE - 1) 
#define XW_BIBTEX_HASH_MAXP1 (XW_BIBTEX_HASH_MAX + 1) 
#define XW_BIBTEX_MAX_HASH_VALUE (XW_BIBTEX_HASH_PRIME + XW_BIBTEX_HASH_PRIME + 125) 
#define XW_BIBTEX_QUOTE_NEXT_FN (XW_BIBTEX_HASH_BASE - 1) 
#define XW_BIBTEX_END_OF_DEF (XW_BIBTEX_HASH_MAX + 1) 
#define XW_BIBTEX_UNDEFINED (XW_BIBTEX_HASH_MAX + 1) 
#define XW_BIBTEX_BUF_SIZE  5000 
#define XW_BIBTEX_MIN_PRINT_LINE 3
#define XW_BIBTEX_MAX_PRINT_LINE 79
#define XW_BIBTEX_AUX_STACK_SIZE 20
#define XW_BIBTEX_MAX_BIB_FILES 20 
#define XW_BIBTEX_POOL_SIZE 65000L 
#define XW_BIBTEX_MAX_STRINGS 35000L
#define XW_BIBTEX_MAX_CITES  5000
#define XW_BIBTEX_WIZ_FN_SPACE  3000
#define XW_BIBTEX_SINGLE_FN_SPACE 100 
#define XW_BIBTEX_MAX_ENT_INTS 3000
#define XW_BIBTEX_MAX_ENT_STRS 3000
#define XW_BIBTEX_ENT_STR_SIZE 250
#define XW_BIBTEX_GLOB_STR_SIZE 5000
#define XW_BIBTEX_MAX_GLOB_STRS 20
#define XW_BIBTEX_MAX_FIELDS 5000
#define XW_BIBTEX_LIT_STK_SIZE 100
#define XW_BIBTEX_NUM_BLT_IN_FNS 37 

#define XW_BIBTEX_ILLEGAL 0 
#define XW_BIBTEX_WHITE_SPACE 1 
#define XW_BIBTEX_ALPHA 2   
#define XW_BIBTEX_NUMERIC 3 
#define XW_BIBTEX_SEP_CHAR 4   
#define XW_BIBTEX_OTHER_LEX 5 
#define XW_BIBTEX_LAST_LEX 5  

#define XW_BIBTEX_ILLEGAL_ID_CHAR  0 
#define XW_BIBTEX_LEGAL_ID_CHAR 1 

#define XW_BIBTEX_NULL_CODE 0
#define XW_BIBTEX_TAB 9
#define XW_BIBTEX_SPACE 32
#define XW_BIBTEX_INVALID_CODE 127

#define XW_BIBTEX_DOUBLE_QUOTE 34
#define XW_BIBTEX_NUMBER_SIGN  35
#define XW_BIBTEX_COMMENT      37
#define XW_BIBTEX_SINGLE_QUOTE 39
#define XW_BIBTEX_LEFT_PAREN 40
#define XW_BIBTEX_RIGHT_PAREN 41
#define XW_BIBTEX_COMMA 44
#define XW_BIBTEX_MINUS_SIGN 45
#define XW_BIBTEX_EQUALS_SIGN 61
#define XW_BIBTEX_AT_SIGN 64
#define XW_BIBTEX_LEFT_BRACE 123
#define XW_BIBTEX_RIGHT_BRACE 125
#define XW_BIBTEX_PERIOD 46
#define XW_BIBTEX_QUESTION_MARK 63
#define XW_BIBTEX_EXCLAMATION_MARK 33
#define XW_BIBTEX_TIE 126
#define XW_BIBTEX_HYPHEN 45
#define XW_BIBTEX_STAR 42
#define XW_BIBTEX_CONCAT_CHAR 35
#define XW_BIBTEX_COLON 58
#define XW_BIBTEX_BACKSLASH 92

#define XW_BIBTEX_SS_WIDTH  500  
#define XW_BIBTEX_AE_WIDTH  722 
#define XW_BIBTEX_OE_WIDTH  778  
#define XW_BIBTEX_UPPER_AE_WIDTH  903 
#define XW_BIBTEX_UPPER_OE_WIDTH 1014

#define XW_BIBTEX_MAX_POP 3

#define hash_next (zzzaa - (int)(XW_BIBTEX_HASH_BASE))
#define hash_text (zzzab - (int)(XW_BIBTEX_HASH_BASE))
#define hash_ilk  (zzzac - (int)(XW_BIBTEX_HASH_BASE))
#define ilk_info (zzzad - (int)(XW_BIBTEX_HASH_BASE))
#define fn_type (zzzae - (int)(XW_BIBTEX_HASH_BASE))
#define fn_info  ilk_info

#define XW_BIBTEX_TEXT_ILK 0
#define XW_BIBTEX_INTEGER_ILK  1 
#define XW_BIBTEX_AUX_COMMAND_ILK  2 
#define XW_BIBTEX_AUX_FILE_ILK  3 
#define XW_BIBTEX_BST_COMMAND_ILK  4 
#define XW_BIBTEX_BST_FILE_ILK  5 
#define XW_BIBTEX_BIB_FILE_ILK  6
#define XW_BIBTEX_FILE_EXT_ILK  7  
#define XW_BIBTEX_FILE_AREA_ILK  8
#define XW_BIBTEX_CITE_ILK  9 
#define XW_BIBTEX_LC_CITE_ILK  10 
#define XW_BIBTEX_BST_FN_ILK  11 
#define XW_BIBTEX_BIB_COMMAND_ILK  12
#define XW_BIBTEX_MACRO_ILK  13 
#define XW_BIBTEX_CONTROL_SEQ_ILK  14 
#define XW_BIBTEX_LAST_ILK  14

#define XW_BIBTEX_N_AUX_BIBDATA  0
#define XW_BIBTEX_N_AUX_BIBSTYLE  1
#define XW_BIBTEX_N_AUX_CITATION 2
#define XW_BIBTEX_N_AUX_INPUT  3
#define XW_BIBTEX_N_BST_ENTRY  0 
#define XW_BIBTEX_N_BST_EXECUTE 1
#define XW_BIBTEX_N_BST_FUNCTION  2
#define XW_BIBTEX_N_BST_INTEGERS  3
#define XW_BIBTEX_N_BST_ITERATE  4
#define XW_BIBTEX_N_BST_MACRO  5  
#define XW_BIBTEX_N_BST_READ  6
#define XW_BIBTEX_N_BST_REVERSE  7
#define XW_BIBTEX_N_BST_SORT  8 
#define XW_BIBTEX_N_BST_STRINGS  9 
#define XW_BIBTEX_N_BIB_COMMENT  0
#define XW_BIBTEX_N_BIB_PREAMBLE  1
#define XW_BIBTEX_N_BIB_STRING  2

#define token_len (buf_ptr2 - buf_ptr1)
#define scan_char (buffer[buf_ptr2])

#define XW_BIBTEX_ID_NULL  0
#define XW_BIBTEX_SPECIFIED_CHAR_ADJACENT  1
#define XW_BIBTEX_OTHER_CHAR_ADJACENT  2
#define XW_BIBTEX_WHITE_ADJACENT  3

#define char_value  (scan_char - '0')

#define XW_BIBTEX_AUX_FOUND     41 
#define XW_BIBTEX_AUX_NOT_FOUND 46

#define cur_aux_str (aux_list[aux_ptr])
#define cur_aux_file (aux_file[aux_ptr])
#define cur_aux_line (aux_ln_stack[aux_ptr])

#define cur_bib_str (bib_list[bib_ptr])
#define cur_bib_file (bib_file[bib_ptr])
#define cur_cite_str (cite_list[cite_ptr])

#define XW_BIBTEX_BUILT_IN  0 
#define XW_BIBTEX_WIZ_DEFINED  1
#define XW_BIBTEX_INT_LITERAL  2 
#define XW_BIBTEX_STR_LITERAL  3 
#define XW_BIBTEX_FIELD  4 
#define XW_BIBTEX_INT_ENTRY_VAR  5
#define XW_BIBTEX_STR_ENTRY_VAR  6 
#define XW_BIBTEX_INT_GLOBAL_VAR  7
#define XW_BIBTEX_STR_GLOBAL_VAR  8
#define XW_BIBTEX_LAST_FN_CLASS  8

#define quote_next_fn (XW_BIBTEX_HASH_BASE - 1)
#define end_of_def    (XW_BIBTEX_HASH_MAX + 1)

#define ex_buf2  ex_buf

#define XW_BIBTEX_END_OF_STRING  XW_BIBTEX_INVALID_CODE

#define field_vl_str  ex_buf
#define field_end     ex_buf_ptr
#define field_start   ex_buf_xptr

#define extra_buf   out_buf
#define ex_buf3     ex_buf
#define ex_buf4     ex_buf 
#define ex_buf4_ptr ex_buf_ptr

#define ex_buf5      ex_buf
#define ex_buf5_ptr  ex_buf_ptr

#define sorted_cites  cite_info

#define XW_BIBTEX_STK_INT  0 
#define XW_BIBTEX_STK_STR  1 
#define XW_BIBTEX_STK_FN   2 
#define XW_BIBTEX_STK_FIELD_MISSING  3
#define XW_BIBTEX_STK_EMPTY  4 
#define XW_BIBTEX_LAST_LIT_TYPE  4


#define XW_BIBTEX_SHORT_LIST  10
#define XW_BIBTEX_END_OFFSET  4 

#define XW_BIBTEX_NEXT_INSERT  24

#define XW_BIBTEX_N_EQUALS  0         
#define XW_BIBTEX_N_GREATER_THAN  1   
#define XW_BIBTEX_N_LESS_THAN  2      
#define XW_BIBTEX_N_PLUS  3           
#define XW_BIBTEX_N_MINUS  4          
#define XW_BIBTEX_N_CONCATENATE  5    
#define XW_BIBTEX_N_GETS  6           
#define XW_BIBTEX_N_ADD_PERIOD  7    
#define XW_BIBTEX_N_CALL_TYPE  8   
#define XW_BIBTEX_N_CHANGE_CASE  9 
#define XW_BIBTEX_N_CHR_TO_INT  10 
#define XW_BIBTEX_N_CITE  11  
#define XW_BIBTEX_N_DUPLICATE  12 
#define XW_BIBTEX_N_EMPTY  13  
#define XW_BIBTEX_N_FORMAT_NAME  14
#define XW_BIBTEX_N_IF  15    
#define XW_BIBTEX_N_INT_TO_CHR  16 
#define XW_BIBTEX_N_INT_TO_STR  17 
#define XW_BIBTEX_N_MISSING  18 
#define XW_BIBTEX_N_NEWLINE  19 
#define XW_BIBTEX_N_NUM_NAMES  20 
#define XW_BIBTEX_N_POP  21      
#define XW_BIBTEX_N_PREAMBLE  22
#define XW_BIBTEX_N_PURIFY  23 
#define XW_BIBTEX_N_QUOTE  24  
#define XW_BIBTEX_N_SKIP  25   
#define XW_BIBTEX_N_STACK  26  
#define XW_BIBTEX_N_SUBSTRING  27 
#define XW_BIBTEX_N_SWAP  28   
#define XW_BIBTEX_N_TEXT_LENGTH  29 
#define XW_BIBTEX_N_TEXT_PREFIX  30 
#define XW_BIBTEX_N_TOP_STACK  31 
#define XW_BIBTEX_N_TYPE  32     
#define XW_BIBTEX_N_WARNING  33  
#define XW_BIBTEX_N_WHILE  34 
#define XW_BIBTEX_N_WIDTH  35 
#define XW_BIBTEX_N_WRITE  36 

#define XW_BIBTEX_N_I  0 
#define XW_BIBTEX_N_J  1 
#define XW_BIBTEX_N_OE  2 
#define XW_BIBTEX_N_OE_UPPER  3 
#define XW_BIBTEX_N_AE  4 
#define XW_BIBTEX_N_AE_UPPER  5 
#define XW_BIBTEX_N_AA  6  
#define XW_BIBTEX_N_AA_UPPER  7 
#define XW_BIBTEX_N_O  8  
#define XW_BIBTEX_N_O_UPPER  9 
#define XW_BIBTEX_N_L  10  
#define XW_BIBTEX_N_L_UPPER  11
#define XW_BIBTEX_N_SS  12

#define name_buf  sv_buffer

#define XW_BIBTEX_OK_PASCAL_I_GIVE_UP  21

#define XW_BIBTEX_TITLE_LOWERS  0 
#define XW_BIBTEX_ALL_LOWERS  1 
#define XW_BIBTEX_ALL_UPPERS  2  
#define XW_BIBTEX_BAD_CONVERSION  3

#define XW_BIBTEX_LONG_TOKEN  3 
#define XW_BIBTEX_LONG_NAME  3 

class XWBibTeX : public XWApplication
{
	Q_OBJECT
	
public:       
    XWBibTeX(QCoreApplication * app);
    ~XWBibTeX(); 
    	                              
    QString      getProductID();
    QString      getProductName();
    const char * getProductName8();
		QString      getVersion();
		const char * getVersion8();
	
		void start(int & argc, char**argv);
		
protected:
	void cleanup();
		
private:
	void addArea(int area);
	void addBufPool(int p_str);
	void addDatabaseCite(int & new_cite);
	void addExtension(int ext);	
	void addOutPool(int p_str);
	void addPoolBufAndPush();
	void alreadySeenFunctionPrint(int seen_fn_loc);
	void appendChar(int c) {str_pool[pool_ptr] = c; pool_ptr++;}
	void auxBibDataCommand();
	void auxBibStyleCommand();
	void auxCitationCommand();
	void auxEnd1ErrPrint();
	void auxEnd2ErrPrint();
	void auxErrIllegalAnotherPrint(int cmd_num);
	void auxErrNoRightBracePrint();
	void auxErrPrint();	
	void auxErrStuffAfterRightBracePrint();
	void auxErrWhiteSpaceInArgumentPrint();
	void auxInputCommand();	
	
	bool badArgumentToken();
	void badCrossReferencePrint(int s);
	void bibCmdConfusion();
	void bibEqualsSignPrint();
	void bibErrPrint();
	void bibFieldTooLongPrint();
	void bibIdentifierScanCheck(const QString & str);
	void bibIdPrint();
	void bibLnNumPrint();
	void bibOneOfTwoPrint(int char1, int char2);
	void bibUnbalancedBracesPrint();
	void bibWarnPrint();
	void braceLvlOneLettersComplaint();
	void bracesUnbalancedComplaint(int pop_lit_var);
	void bstCantMessWithEntriesPrint();
	void bstEntryCommand();
	void bstErrPrintAndLookForBlankLine();
	void bstExecuteCommand();
	void bstExWarnPrint();
	void bstFunctionCommand();
	void bstIdPrint();
	void bstIntegersCommand();
	void bstIterateCommand();
	void bstLeftBracePrint();
	void bstLnNumPrint();
	void bstMacroCommand();
	void bstMildExWarnPrint();
	void bstReadCommand();
	void bstReverseCommand();
	void bstRightBracePrint();
	void bstSortCommand();
	void bstStringsCommand();
	void bstWarnPrint();
	void bst1printStringSizeExceeded();
	void bst2printStringSizeExceeded();
	void bufferOverflow();
	void buildIn(char * pds, int len, int fn_hash_loc, int blt_in_num);
	
	void caseConversionConfusion();
	void checkBraceLevel(int pop_lit_var);
	void checkCiteOverflow(int last_cite);
	void checkCommandExecution();
	void checkFieldOverflow(int total_fields);
	void citeKeyDisappearedConfusion();
	bool compressBibWhite();
	void confusion(const QString & str);
	
	void decrBraceLevel(int pop_lit_var);
	
	void eatBibPrint();
	bool eatBibWhiteSpace();
	void eatBstPrint();
	bool eatBstWhiteSpace();
	bool enoughTextChars(int enough_chars);
	void executeFn(int ex_fn_loc);
	
	void figureOutTheFormattedName();
	void fileNmSizeOverflow();
	bool findCiteLocsForThisCiteKey(int cite_str);
	void flushString() {str_ptr--; pool_ptr = str_start[str_ptr];}
	
	void getAuxCommandAndProcess();
	void getBibCommandOrEntryAndProcess();
	void getBstCommandAndProcess();
	QString getFileName();
	void getTheTopLevelAuxFileName(int & argc, char**argv);
	
	void hashCiteConfusion();
	bool hashIsFull() {return (hash_used == XW_BIBTEX_HASH_BASE);}
	
	void idScanningConfusion();
	void illeglLiteralConfusion();
	void initCommandExecution();
	void initialize(int & argc, char**argv);
	bool inputln(QFile * f);
	void intToASCII(int theint, int * int_buf, int int_begin, int * int_end);
	
	void lastCheckForAuxErrors();
	int  length(int s) {return (str_start[s+1]-str_start[s]);}
	bool lessThan(int arg1, int arg2);
	void lowerCase(int * buf, int bf_ptr, int len);
	
	void macroWarnPrint();
	int  makeString();
	void markError();
	void markFatal();	
	void markWarning();
	
	void nameScanForAnd(int pop_lit_var);
	void nonexistentCrossReferenceError();
	
	void outPoolStr(QFile * f, int s);
	void outputBblLine();
	void outToken(QFile * f);
	void overflow(const QString & str, int c);
	
	void poolOverflow();
	void popLitStk(int * pop_lit, int * pop_type);
	void popTheAuxStack();
	void popTopAndPrint();
	void popWholeStack();
	void predefCertainStrings();
	void predefine(char * pds, int len, int ilk);
	void print(const QString & s);
	void print(uchar c);
	void printAPoolstr(int s);
	void printAuxName();
	void printBadInputLine();
	void printBibName();
	void printBstName();
	void printConfusion();
	void printFnClass(int fn_loc);
	void printLit(int stk_lt, int stk_tp);
	void println(const QString & s);	
	void printMissingEntry(int s);
	void printNewLine();
	void printOverflow();
	void printPoolStr(int s);
	void printRecursionIllegal();
	void printSkippingWhateverRemains();
	void printStkLit(int stk_lt, int stk_tp);		
	void printToken();
	void printWrongStkLit(int stk_lt, int stk_tp1, int stk_tp2);
	void pushLitStk(int push_lt, int push_type);
	
	void quickSort(int left_end, int right_end);
	
	void samTooLongFileNamePrint();
	void samWrongFileNamePrint();
	bool scanAFieldTokenAndEatWhite();
	bool scanAlpha();
	bool scanAndStoreTheFieldValueAndEatWhite();
	bool scanBalancedBraces();
	void scanFnDef(int fn_hash_loc);
	void scanIdentifier(int char1, int char2, int char3);
	bool scanInteger();
	bool scanNonnegInteger();
	bool scanWhiteSpace();
	bool scan1(int char1);
	bool scan2(int char1, int char2);
	bool scan3(int char1, int char2, int char3);
	bool scan1White(int char1);
	bool scan2White(int char1, int char2);
	void singlFnOverflow();
	void skipIllegalStuffAfterTokenPrint();
	void skipStuffAtSpBraceLevelGreaterThanOne();
	void skipTokenPrint();
	void skpTokenUnknownFunctionPrint();
	void startName(int file_name);
	bool strEqBuf(int s, int * buf, int bf_ptr, int len);
	bool strEqStr(int s1, int s2);
	int  strLookup(int * buf, int j, int l, int ilk, bool insert_it);
	void strRoom(int s);
	void swap(int swap1, int swap2);
	
	void tracePr(const QString & s);
	void tracePrLn(const QString & s);
	void tracePrNewLine();
	void tracePrPoolStr(int s);
	void tracePrToken();
	
	void unflushString() {str_ptr++; pool_ptr = str_start[str_ptr];}	
	void unknwnFunctionClassConfusion();
	void unknwnLiteralConfusion();
	void upperCase(int * buf, int bf_ptr, int len);
	void usage(const QString & p);
	void usageHelp();
	
	void vonNameEndsAndLastNameStartsStuff();
	bool vonTokenFound();
	
	void xAddPeriod();
	void xChangeCase();
	void xChrToInt();
	void xCite();
	void xConcatenate();
	void xDuplicate();
	void xEmpty();
	void xEquals();
	void xFormatName();
	void xGets();
	void xGreaterThan();
	void xIntToChr();
	void xIntToStr();
	void xLessThan();
	void xMinus();
	void xMissing();
	void xNumNames();
	void xPlus();	
	void xPreamble();
	void xPurify();
	void xQuote();
	void xSubstring();
	void xSwap();
	void xTextLength();
	void xTextPrefix();
	void xType();
	void xWarning();
	void xWidth();
	void xWrite();
	
private:
	int history;
	int err_count;
	QFile * log_file;
	QFile * term_out;
	uchar xchr[256];
	uchar xord[256];
	uchar lex_class[256];
	uchar id_class[256];
	int char_width[256];
	int string_width;
	int * zzzaa;
	int * zzzab ;
	int * zzzac;
	int * zzzad;
	
	int *  name_of_file;
	int    name_length;
	int    name_ptr;
	
	int *  buffer;
	int    last;
	
	int *  sv_buffer;
	int    sv_ptr1;
	int    sv_ptr2;
	int    tmp_ptr, tmp_end_ptr;
	
	int  * str_pool;
	int  * str_start;
	int   pool_ptr;
	int   pool_size;
	int   str_ptr;
	int   str_num;
	int   p_ptr1, p_ptr2;
	
	int   hash_used;
	bool  hash_found;
	int   dummy_loc;	
	
	int s_aux_extension;
	int s_log_extension;
	int s_bbl_extension;
	int s_bst_extension;
	int s_bib_extension;
	int s_bst_area;
	int s_bib_area;
	int pre_def_loc;
	
	int command_num;
	
	int buf_ptr1;
	int buf_ptr2;
	
	int scan_result;
	int token_value;
	
	int aux_name_length;
	
	bool check_cmnd_line;
	int  verbose;
	int  min_crossrefs;
	
	QFile ** aux_file;
	int   *  aux_list;
	int   *  aux_ln_stack;
	int      aux_ptr;
	int      top_lev_str;
	QFile *  bbl_file;
	
	int * bib_list;
	int   bib_ptr;
	int   num_bib_files;
	bool  bib_seen;
	int   max_bib_files;
	QFile ** bib_file;
	
	bool bst_seen;
	int  bst_str;
	QFile * bst_file;
	
	int   * cite_list;
	int     cite_ptr;
	int   entry_cite_ptr;
	int   num_cites;
	int   old_num_cites;
	int   citation_seen;
	int   cite_loc;
	int   lc_cite_loc;
	int   lc_xcite_loc;
	bool  cite_found;
	bool  all_entries;
	int   all_marker;
	
	int bbl_line_num;
	int bst_line_num;
	
	int   fn_loc;
	int   wiz_loc;
	int   literal_loc;
	int   macro_name_loc;
	int   macro_def_loc;
	int * zzzae;
	int   wiz_def_ptr;
	int   wiz_fn_ptr;
	int * wiz_functions;
	int   int_ent_ptr;
	int * entry_ints;
	int   max_ent_ints;
	int   max_ent_strs;	
	int   max_fields;
	int   wiz_fn_space;
	int   num_ent_ints;
	int   str_ent_ptr;
	int * entry_strs;
	int   num_ent_strs;
	int   str_glb_ptr;
	int   glb_str_ptr[20];
	int **  global_strs;
	int   glb_str_end[20];
	int   num_glb_strs;
	int   field_ptr;
	int   field_parent_ptr, field_end_ptr;
	int   cite_parent_ptr, cite_xptr;
	int * field_info;
	int   num_fields;
	int   num_pre_defined_fields;
	int   crossref_num;
	bool  no_fields;
	
	bool  entry_seen;
	bool  read_seen;
	bool  read_performed;
	bool  reading_completed;
	bool  read_completed;
	
	int   impl_fn_num;
	
	int bib_brace_level;
	
	int   bib_line_num;
	int   entry_type_loc;
	int * type_list;
	bool  type_exists;
	bool * entry_exists;
	bool   store_entry;
	int    field_name_loc;
	int    field_val_loc;
	bool   store_field;
	bool   store_token;
	int    right_outer_delim;
	int    right_str_delim;
	bool   at_bib_command;
	int    cur_macro_loc;
	int  * cite_info;
	bool   cite_hash_found;
	int    preamble_ptr;
	int    num_preamble_strings;
	
	int *  lit_stack;
	int *  lit_stk_type;
	int    lit_stk_ptr;
	int    cmd_str_ptr;
	int    ent_chr_ptr;
	int    glob_chr_ptr;
	int *  ex_buf;
	int    ex_buf_ptr;
	int    ex_buf_length;
	int *  out_buf;
	int    out_buf_ptr;
	int    out_buf_length;
	int    mess_with_entries;
	int    sort_cite_ptr;
	int    sort_key_num;
	int    brace_level;
	
	int b_equals;
	int b_greater_than;
	int b_less_than;
	int b_plus;
	int b_minus;
	int b_concatenate;
	int b_gets;
	int b_add_period;
	int b_call_type;
	int b_change_case;
	int b_chr_to_int;
	int b_cite;
	int b_duplicate;
	int b_empty;
	int b_format_name;
	int b_if;
	int b_int_to_chr;
	int b_int_to_str;
	int b_missing;
	int b_newline;
	int b_num_names;
	int b_pop;
	int b_preamble;
	int b_purify;
	int b_quote;
	int b_skip;
	int b_stack;
	int b_substring;
	int b_swap;
	int b_text_length;
	int b_text_prefix;
	int b_top_stack;
	int b_type;
	int b_warning;
	int b_while;
	int b_width;
	int b_write;
	int b_default;
	
	int s_null;
	int s_default;
	int s_t;
	int s_l;
	int s_u;
	int * s_preamble;
	
	int pop_lit1, pop_lit2, pop_lit3;
	int pop_typ1, pop_typ2, pop_typ3;
	int sp_ptr;
	int sp_xptr1, sp_xptr2;
	int sp_end;
	int sp_length, sp2_length;
	int sp_brace_level;
	int ex_buf_xptr, ex_buf_yptr;
	int control_seq_loc;
	bool preceding_white;
	bool and_found;
	int num_names;
	int name_bf_ptr;
	int name_bf_xptr, name_bf_yptr;
	int nm_brace_level;
	int * name_tok;
	int * name_sep_char;
	int num_tokens;
	bool token_starting;
	bool alpha_found;
	bool double_letter, end_of_group, to_be_written;
	int first_start;
	int first_end;
	int last_end;
	int von_start;
	int von_end;
	int jr_end;
	int cur_token, last_token;
	bool use_default;
	int num_commas;
	int comma1, comma2;
	int num_text_chars;
	
	int conversion_type;
	bool prev_colon;
	
	bool stoped;
	
	QString outputPath;
};

#endif // XWBIBTEX_H

