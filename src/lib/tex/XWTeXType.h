/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXTYPE_H
#define XWTEXTYPE_H

#include <ctype.h>
#include <QtGlobal>

#ifdef Q_OS_WIN
#define isascii __isascii
#else
#ifndef isascii
#define isascii(c) 1
#endif
#endif

#ifdef XW_TEX

#ifndef isblank
#define isblank(c) ((c) == ' ' || (c) == '\t')
#endif

#define ISBLANK(c) (isascii (c) && isblank (c))

#define TEX_SSUP_ERROR_LINE  255
#define TEX_SSUP_MAX_STRINGS 200000
#define TEX_SSUP_TRIE_OPCODE 65535
#define TEX_SSUP_TRIE_SIZE   262143
#define TEX_SSUP_HYPH_SIZE   65535
#define TEX_IINF_HYPHEN_SIZE 610

#define TEX_INF_MAIN_MEMORY 2000000
#define TEX_SUP_MAIN_MEMORY 16000000

#define TEX_INF_TRIE_SIZE    80000
#define TEX_SUP_TRIE_SIZE   (TEX_SSUP_TRIE_SIZE)
#define TEX_INF_MAX_STRINGS  100000
#define TEX_SUP_MAX_STRINGS (TEX_SSUP_MAX_STRINGS)

#define TEX_INF_STRINGS_FREE 100
#define TEX_SUP_STRINGS_FREE (TEX_SUP_MAX_STRINGS)

#define TEX_INF_BUF_SIZE 500
#define TEX_SUP_BUF_SIZE 200000

#define TEX_INF_NEST_SIZE 40
#define TEX_SUP_NEST_SIZE 400

#define TEX_INF_MAX_IN_OPEN  6
#define TEX_SUP_MAX_IN_OPEN  127

#define TEX_INF_PARAM_SIZE 60
#define TEX_SUP_PARAM_SIZE 600

#define TEX_INF_SAVE_SIZE 600
#define TEX_SUP_SAVE_SIZE 40000

#define TEX_INF_STACK_SIZE 200
#define TEX_SUP_STACK_SIZE 3000

#define TEX_INF_DVI_BUF_SIZE      800
#define TEX_SUP_DVI_BUF_SIZE      65536

#define TEX_INF_POOL_SIZE         32000
#define TEX_SUP_POOL_SIZE         10000000
#define TEX_INF_POOL_FREE         1000
#define TEX_SUP_POOL_FREE         TEX_SUP_POOL_SIZE
#define TEX_INF_STRING_VACANCIES  8000
#define TEX_SUP_STRING_VACANCIES (TEX_SUP_POOL_SIZE - 23000)

#define TEX_SUP_HYPH_SIZE      TEX_SSUP_HYPH_SIZE
#define TEX_INF_HYPH_SIZE      TEX_IINF_HYPHEN_SIZE
#define TEX_SUP_OCP_LIST_SIZE  1000000
#define TEX_INF_OCP_LIST_SIZE  1000
#define TEX_SUP_OCP_BUF_SIZE   1000000
#define TEX_INF_OCP_BUF_SIZE   1000
#define TEX_SUP_OCP_STACK_SIZE 1000000
#define TEX_INF_OCP_STACK_SIZE 1000

#define TEX_EMPTY 0

#define TEX_ETEX_STATES 1

#define TEX_FONT_MAX  65535
#define TEX_FILE_NAME_SIZE  1024
#define TEX_ACTIVE_MEM_SIZE 2000
#define TEX_OCP_MAXINT 0x10000000

#define TEX_FONT_BASE 0
#define TEX_HASH_SIZE  65536
#define TEX_HASH_PRIME 55711
#define TEX_HYPH_PRIME 607
#define TEX_OCP_BASE             0
#define TEX_OCP_BIGGEST          65535
#define TEX_NUMBER_OCPS          (TEX_OCP_BIGGEST - TEX_OCP_BASE + 1)
#define TEX_OCP_LIST_BASE        0
#define TEX_OCP_LIST_BIGGEST     65535
#define TEX_NUMBER_OCP_LISTS     (TEX_OCP_LIST_BIGGEST - TEX_OCP_LIST_BASE + 1)
#define TEX_MAX_ACTIVE_OCP_LISTS 65536
#define TEX_BIGGEST_CHAR 65535
#define TEX_TOO_BIG_CHAR 65536
#define TEX_SPECIAL_CHAR 65537
#define TEX_NUMBER_CHARS 65536
#define TEX_BIGGEST_REG  65535
#define TEX_NUMBER_REGS  65536
#define TEX_FONT_BIGGEST      65535
#define TEX_NUMBER_FONTS      (TEX_FONT_BIGGEST - TEX_FONT_BASE + 2)
#define TEX_NUMBER_MATH_FONTS 768
#define TEX_MATH_FONT_BIGGEST 767
#define TEX_TEXT_SIZE          0
#define TEX_SCRIPT_SIZE        256
#define TEX_SCRIPT_SCRIPT_SIZE 512
#define TEX_BIGGEST_LANG 255
#define TEX_TOO_BIG_LANG 256

#define TEX_NULL_CODE 0
#define TEX_CARRIAGE_RETURN 0x0D
#define TEX_INVALID_CODE    0x7F

#define TEX_NO_MODE        0
#define TEX_ONEBYTE_MODE   1
#define TEX_EBCDIC_MODE    2
#define TEX_TWOBYTE_MODE   3
#define TEX_TWOBYTELE_MODE 4
#define TEX_UTF8_MODE      5
#define TEX_UTF16_MODE     6
#define TEX_UTF32_MODE     7
#define TEX_UTF32BE_MODE   8
#define TEX_UTF32LE_MODE   9
#define TEX_GB18030_MODE   10
#define TEX_GBK_MODE       11
#define TEX_GB_MODE        12
#define TEX_BIG5_MODE      13
#define TEX_EUCJP_MODE     14
#define TEX_EUCKR_MODE     15
#define TEX_HKSCS_MODE     16
#define TEX_KSLATEX_MODE   17
#define TEX_SJIS_MODE      18
		
#define TEX_TRANS_INPUT             0
#define TEX_TRANS_OUTPUT            1
#define TEX_TRANS_DEFAULT_INPUT     2
#define TEX_TRANS_DEFAULT_OUTPUT    3
#define TEX_TRANS_NO_INPUT          4
#define TEX_TRANS_NO_OUTPUT         5
#define TEX_TRANS_NO_DEFAULT_INPUT  6
#define TEX_TRANS_NO_DEFAULT_OUTPUT 7

#define TEX_MODE_INPUT             0
#define TEX_MODE_OUTPUT            1
#define TEX_MODE_DEFAULT_INPUT     2
#define TEX_MODE_DEFAULT_OUTPUT    3
#define TEX_MODE_NO_INPUT          4
#define TEX_MODE_NO_OUTPUT         5
#define TEX_MODE_NO_DEFAULT_INPUT  6
#define TEX_MODE_NO_DEFAULT_OUTPUT 7

#define loc (cur_input.loc_field)

#define str_start(s) (str_start_ar[(s) - TEX_TOO_BIG_CHAR])

#define TEX_NO_PRINT     16
#define TEX_TERM_ONLY    17
#define TEX_LOG_ONLY     18
#define TEX_TERM_AND_LOG 19
#define TEX_PSEUDO       20
#define TEX_NEW_STRING   21
#define TEX_MAX_SELECTOR 21

#define TEX_BATCH_MODE       0
#define TEX_NONSTOP_MODE     1
#define TEX_SCROLL_MODE      2
#define TEX_ERROR_STOP_MODE  3
#define TEX_UNSPECIFIED_MODE 4

#define TEX_SPOTLESS             0
#define TEX_WARNING_ISSUED       1
#define TEX_ERROR_MESSAGE_ISSUED 2
#define TEX_FATAL_ERROR_STOP     3

#define TEX_UNITY  65536
#define TEX_TWO    131072

#define TEX_INF_BAD 10000

#define TEX_MIN_QUARTERWORD 0
#define TEX_MAX_QUARTERWORD 0xFFFF
#define TEX_MIN_HALFWORD    0
#define TEX_MAX_HALFWORD    0x3FFFFFFF
#define TEX_NULL            TEX_MIN_HALFWORD
#define TEX_EMPTY_FLAG      TEX_MAX_HALFWORD

typedef union
{
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
	struct
	{
		qint32 RH, LH;
	} v;
	
	struct
	{
		qint32 junk;
		quint16 B0, B1;
	} u;
#else
	struct
	{
		qint32 LH, RH;
	} v;
	
	struct
	{
		quint16 B1, B0;
	} u;
#endif
	
} TeXTwoHalves;

typedef struct
{
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
	struct
	{
		quint16 B0, B1, B2, B3;
	} u;
#else
	struct
	{
		quint16 B3, B2, B1, B0;
	} u;
#endif
} TeXFourQuarters;

typedef struct
{
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
	qint32 CINT0, CINT1;
#else
	qint32 CINT1, CINT0;
#endif	
} TeXTwoInts;

typedef struct
{
  	double GLUE;
} TeXGlues;


typedef union
{
  TeXTwoHalves hh;
  TeXFourQuarters qqqq;
  TeXTwoInts ii;
  TeXGlues gg;
} TeXMemoryWord;

#define link(s)   (mem[(s)].hh.v.RH)
#define info(s)   (mem[(s)].hh.v.LH)
#define node_size(s) (info((s)))
#define llink(s)  (info((s)+1))
#define rlink(s)  (link((s)+1))

#define type(s) (mem[(s)].hh.u.B0) //u16
#define subtype(s) (mem[(s)].hh.u.B1) //u16

#define font(s) (type((s))) //u16
#define character(s) (subtype((s))) //u16

#define TEX_HLIST_NODE    0
#define TEX_BOX_NODE_SIZE 8
#define TEX_WIDTH_OFFSET  1
#define TEX_DEPTH_OFFSET  2
#define TEX_HEIGHT_OFFSET 3

#define width(s)         (mem[(s) + TEX_WIDTH_OFFSET].ii.CINT0)
#define depth(s)         (mem[(s) + TEX_DEPTH_OFFSET].ii.CINT0)
#define height(s)        (mem[(s) + TEX_HEIGHT_OFFSET].ii.CINT0)
#define shift_amount(s)  (mem[(s) + 4].ii.CINT0)

#define TEX_LIST_OFFSET 5

#define list_ptr(s) (link((s) + TEX_LIST_OFFSET))
#define glue_order(s) (subtype((s) + (quint16)TEX_LIST_OFFSET)) //u16
#define glue_sign(s) (type((s) + (quint16)TEX_LIST_OFFSET)) //u16

#define TEX_NORMAL      0
#define TEX_STRETCHING  1
#define TEX_SHRINKING   2
#define TEX_GLUE_OFFSET 6

#define glue_set(s) (mem[(s) + TEX_GLUE_OFFSET].gg.GLUE)

#define TEX_DIR_OFFSET 7

#define box_dir(s) (mem[(s) + TEX_DIR_OFFSET].ii.CINT0)

#define TEX_VLIST_NODE     1
#define TEX_RULE_NODE      2
#define TEX_RULE_NODE_SIZE 5
#define TEX_NULL_FLAG      (-1073741824)

#define rule_dir(s) (info((s) + 4))

#define TEX_INS_NODE      3
#define TEX_INS_NODE_SIZE 5

#define float_cost(s)    (mem[(s) + 1].ii.CINT0)
#define ins_ptr(s)       (info((s) + 4))
#define split_top_ptr(s) (link((s) + 4))

#define TEX_MARK_NODE       4
#define TEX_SMALL_NODE_SIZE 2

#define mark_ptr(s)   (link((s) + 1))
#define mark_class(s) (info((s) + 1))

#define TEX_ADJUST_NODE 5

#define adjust_ptr(s) (mem[(s) + 1].ii.CINT0)

#define TEX_LIGATURE_NODE 6

#define lig_char(s) ((s) + 1)
#define lig_ptr(s)  (link(lig_char((s))))

#define TEX_DISC_NODE 7

#define replace_count(s) (subtype((s))) //u16
#define pre_break(s)     (llink((s)))
#define post_break(s)    (rlink((s)))

#define TEX_WHATSIT_NODE 8
#define TEX_MATH_NODE    9
#define TEX_BEFORE       0
#define TEX_AFTER        1

#define TEX_GLUE_NODE      10
#define TEX_COND_MATH_GLUE 98
#define TEX_MU_GLUE        99
#define TEX_A_LEADERS      100
#define TEX_C_LEADERS      101
#define TEX_X_LEADERS      102

#define glue_ptr(s) (llink((s)))
#define leader_ptr(s) (rlink((s)))

#define TEX_GLUE_SPEC_SIZE  4

#define glue_ref_count(s)  (link((s)))
#define stretch(s)  (mem[(s) + 2].ii.CINT0)
#define shrink(s)   (mem[(s) + 3].ii.CINT0)
#define stretch_order(s)  (type((s))) //u16
#define shrink_order(s)   (subtype((s))) //u16

#define TEX_SFI   1
#define TEX_FIL   2
#define TEX_FILL  3
#define TEX_FILLL 4

#define TEX_KERN_NODE  11
#define TEX_EXPLICIT 1
#define TEX_ACC_KERN 2

#define TEX_PENALTY_NODE 12
#define TEX_INF_PENALTY TEX_INF_BAD
#define TEX_EJECT_PENALTY (-TEX_INF_PENALTY)

#define penalty(s)  (mem[(s) + 1].ii.CINT0)

#define TEX_UNSET_NODE 13

#define glue_stretch(s) (mem[(s) + TEX_GLUE_OFFSET].ii.CINT0)

#define glue_shrink(s) (shift_amount((s)))
#define span_count(s)  (subtype((s))) //u16

#define zero_glue       mem_bot
#define sfi_glue        (zero_glue + TEX_GLUE_SPEC_SIZE)
#define fil_glue        (sfi_glue + TEX_GLUE_SPEC_SIZE)
#define fill_glue       (fil_glue + TEX_GLUE_SPEC_SIZE)
#define ss_glue         (fill_glue + TEX_GLUE_SPEC_SIZE)
#define fil_neg_glue    (ss_glue + TEX_GLUE_SPEC_SIZE)
#define lo_mem_stat_max (fil_neg_glue + TEX_GLUE_SPEC_SIZE - 1)
#define page_ins_head   mem_top
#define contrib_head    (mem_top - 1)
#define page_head       (mem_top - 2)
#define temp_head       (mem_top - 3)
#define hold_head       (mem_top - 4)
#define adjust_head     (mem_top - 5)
#define active          (mem_top - 7)
#define align_head      (mem_top - 8)
#define end_span        (mem_top - 9)
#define omit_template   (mem_top - 10)
#define null_list       (mem_top - 11)
#define lig_trick       (mem_top - 12)
#define garbage         (mem_top - 12)
#define backup_head     (mem_top - 13)
#define hi_mem_stat_min (mem_top - 13)
#define hi_mem_stat_usage 14

#define token_ref_count(s)  (info((s)))

#define add_token_ref(s) ((token_ref_count((s)))++)
#define add_glue_ref(s)  ((glue_ref_count((s)))++)

#define TEX_ESCAPE        0
#define TEX_RELAX         0
#define TEX_LEFT_BRACE    1
#define TEX_RIGHT_BRACE   2
#define TEX_MATH_SHIFT    3
#define TEX_TAB_MARK      4
#define TEX_CAR_RET       5
#define TEX_OUT_PARAM     5
#define TEX_MAC_PARAM     6
#define TEX_SUP_MARK      7
#define TEX_SUB_MARK      8
#define TEX_IGNORE        9
#define TEX_ENDV          9
#define TEX_SPACER        10
#define TEX_LETTER        11
#define TEX_OTHER_CHAR    12
#define TEX_ACTIVE_CHAR   13
#define TEX_PAR_END       13
#define TEX_MATCH         13
#define TEX_COMMENT       14
#define TEX_END_MATCH     14
#define TEX_STOP          14
#define TEX_INVALID_CHAR  15
#define TEX_DELIM_NUM     15
#define TEX_MAX_CHAR_CODE 15

#define TEX_CHAR_NUM      16
#define TEX_MATH_CHAR_NUM 17
#define TEX_MARK          18
#define TEX_XRAY          19
#define TEX_MAKE_BOX      20
#define TEX_HMOVE         21
#define TEX_VMOVE         22
#define TEX_UN_HBOX       23
#define TEX_UN_VBOX       24
#define TEX_REMOVE_ITEM   25
#define TEX_HSKIP         26
#define TEX_VSKIP         27
#define TEX_MSKIP         28
#define TEX_KERN          29
#define TEX_MKERN         30
#define TEX_LEADER_SHIP   31
#define TEX_HALIGN        32
#define TEX_VALIGN        33
#define TEX_NO_ALIGN      34
#define TEX_VRULE         35
#define TEX_HRULE         36
#define TEX_INSERT        37
#define TEX_VADJUST       38
#define TEX_IGNORE_SPACES 39
#define TEX_AFTER_ASSIGNMENT 40
#define TEX_AFTER_GROUP   41
#define TEX_BREAK_PENALTY 42
#define TEX_START_PAR     43
#define TEX_ITAL_CORR     44
#define TEX_ACCENT        45
#define TEX_MATH_ACCENT   46
#define TEX_DISCRETIONARY 47
#define TEX_EQ_NO         48
#define TEX_LEFT_RIGHT    49
#define TEX_MATH_COMP     50
#define TEX_LIMIT_SWITCH  51
#define TEX_ABOVE         52
#define TEX_MATH_STYLE    53
#define TEX_MATH_CHOICE   54
#define TEX_NON_SCRIPT    55
#define TEX_VCENTER       56
#define TEX_CASE_SHIFT    57
#define TEX_MESSAGE       58
#define TEX_EXTENSION     59
#define TEX_IN_STREAM     60
#define TEX_BEGIN_GROUP   61
#define TEX_END_GROUP     62
#define TEX_OMIT          63
#define TEX_EX_SPACE      64
#define TEX_NO_BOUNDARY   65
#define TEX_RADICAL       66
#define TEX_END_CS_NAME   67
#define TEX_CHAR_GHOST        (TEX_END_CS_NAME + 1)
#define TEX_ASSIGN_LOCAL_BOX  (TEX_CHAR_GHOST + 1)
#define TEX_MIN_INTERNAL      (TEX_ASSIGN_LOCAL_BOX + 1)
#define TEX_CHAR_GIVEN         TEX_MIN_INTERNAL
#define TEX_MATH_GIVEN        (TEX_CHAR_GIVEN + 1)
#define TEX_OMATH_GIVEN       (TEX_MATH_GIVEN + 1)
#define TEX_LAST_ITEM         (TEX_OMATH_GIVEN + 1)
#define TEX_MAX_NON_PREFIXED_COMMAND  TEX_LAST_ITEM

#define TEX_TOKS_REGISTER     (TEX_MAX_NON_PREFIXED_COMMAND + 1)
#define TEX_ASSIGN_TOKS       (TEX_TOKS_REGISTER + 1)
#define TEX_ASSIGN_INT        (TEX_ASSIGN_TOKS + 1)
#define TEX_ASSIGN_DIMEN      (TEX_ASSIGN_INT + 1)
#define TEX_ASSIGN_GLUE       (TEX_ASSIGN_DIMEN + 1)
#define TEX_ASSIGN_MU_GLUE    (TEX_ASSIGN_GLUE + 1)
#define TEX_ASSIGN_FONT_DIMEN  (TEX_ASSIGN_MU_GLUE + 1)
#define TEX_FONT_DIMEN_CODE    0
#define TEX_CHAR_WIDTH_CODE   (TEX_FONT_DIMEN_CODE + 1)
#define TEX_CHAR_HEIGHT_CODE  (TEX_CHAR_WIDTH_CODE + 1)
#define TEX_CHAR_DEPTH_CODE   (TEX_CHAR_HEIGHT_CODE + 1)
#define TEX_CHAR_ITALIC_CODE  (TEX_CHAR_DEPTH_CODE + 1)
#define TEX_ASSIGN_FONT_INT   (TEX_ASSIGN_FONT_DIMEN + 1)
#define TEX_SET_AUX           (TEX_ASSIGN_FONT_INT + 1)
#define TEX_SET_PREV_GRAF     (TEX_SET_AUX + 1)
#define TEX_SET_PAGE_DIMEN    (TEX_SET_PREV_GRAF + 1)
#define TEX_SET_PAGE_INT      (TEX_SET_PAGE_DIMEN + 1)
#define TEX_SET_BOX_DIMEN     (TEX_SET_PAGE_INT + 1)
#define TEX_SET_SHAPE         (TEX_SET_BOX_DIMEN + 1)
#define TEX_DEF_CODE          (TEX_SET_SHAPE + 1)
#define TEX_DEF_FAMILY        (TEX_DEF_CODE + 1)
#define TEX_SET_FONT          (TEX_DEF_FAMILY + 1)
#define TEX_DEF_FONT          (TEX_SET_FONT + 1)
#define TEX_REGISTER          (TEX_DEF_FONT + 1)
#define TEX_ASSIGN_NEXT_FAKE_MATH (TEX_REGISTER + 1)
#define TEX_ASSIGN_BOX_DIR        (TEX_REGISTER + 2)
#define TEX_ASSIGN_DIR            (TEX_REGISTER + 3)
#define TEX_MAX_INTERNAL          TEX_ASSIGN_DIR
#define TEX_ADVANCE               (TEX_MAX_INTERNAL + 1)
#define TEX_MULTIPLY              (TEX_ADVANCE + 1)
#define TEX_DIVIDE                (TEX_MULTIPLY + 1)
#define TEX_PREFIX                (TEX_DIVIDE + 1)
#define TEX_LET                   (TEX_PREFIX + 1)
#define TEX_SHORTHAND_DEF         (TEX_LET + 1)
#define TEX_READ_TO_CS            (TEX_SHORTHAND_DEF + 1)
#define TEX_DEF                   (TEX_READ_TO_CS + 1)
#define TEX_SET_BOX               (TEX_DEF + 1)
#define TEX_HYPH_DATA             (TEX_SET_BOX + 1)
#define TEX_SET_INTERACTION       (TEX_HYPH_DATA + 1)
#define TEX_SET_OCP               (TEX_SET_INTERACTION + 1)
#define TEX_DEF_OCP               (TEX_SET_OCP + 1)
#define TEX_SET_OCP_LIST          (TEX_DEF_OCP + 1)
#define TEX_DEF_OCP_LIST          (TEX_SET_OCP_LIST + 1)
#define TEX_CLEAR_OCP_LISTS       (TEX_DEF_OCP_LIST + 1)
#define TEX_PUSH_OCP_LIST         (TEX_CLEAR_OCP_LISTS + 1)
#define TEX_POP_OCP_LIST          (TEX_PUSH_OCP_LIST + 1)
#define TEX_OCP_LIST_OP           (TEX_POP_OCP_LIST + 1)
#define TEX_OCP_TRACE_LEVEL       (TEX_OCP_LIST_OP + 1)
#define TEX_CHAR_TRANS            (TEX_OCP_TRACE_LEVEL + 1)
#define TEX_CHAR_MODE             (TEX_CHAR_TRANS + 1)
#define TEX_MAX_COMMAND            TEX_CHAR_MODE

#define TEX_UNDEFINED_CS          (TEX_MAX_COMMAND + 1)
#define TEX_EXPAND_AFTER          (TEX_MAX_COMMAND + 2)
#define TEX_NO_EXPAND             (TEX_MAX_COMMAND + 3)
#define TEX_INPUT                 (TEX_MAX_COMMAND + 4)
#define TEX_IF_TEST               (TEX_MAX_COMMAND + 5)
#define TEX_FI_OR_ELSE            (TEX_MAX_COMMAND + 6)
#define TEX_CS_NAME               (TEX_MAX_COMMAND + 7)
#define TEX_CONVERT               (TEX_MAX_COMMAND + 8)
#define TEX_THE                   (TEX_MAX_COMMAND + 9)
#define TEX_TOP_BOT_MARK          (TEX_MAX_COMMAND + 10)
#define TEX_CALL                  (TEX_MAX_COMMAND + 11)
#define TEX_LONG_CALL             (TEX_MAX_COMMAND + 12)
#define TEX_OUTER_CALL            (TEX_MAX_COMMAND + 13)
#define TEX_LONG_OUTER_CALL       (TEX_MAX_COMMAND + 14)
#define TEX_END_TEMPLATE          (TEX_MAX_COMMAND + 15)
#define TEX_DONT_EXPAND           (TEX_MAX_COMMAND + 16)
#define TEX_GLUE_REF              (TEX_MAX_COMMAND + 17)
#define TEX_SHAPE_REF             (TEX_MAX_COMMAND + 18)
#define TEX_BOX_REF               (TEX_MAX_COMMAND + 19)
#define TEX_DATA                  (TEX_MAX_COMMAND + 20)

#define TEX_VMODE    1
#define TEX_HMODE   (TEX_VMODE + TEX_MAX_COMMAND + 1)
#define TEX_MMODE   (TEX_HMODE + TEX_MAX_COMMAND + 1)

#define TEX_IGNORE_DEPTH  -65536000

typedef struct ListStateRecord
{
	qint32 mode_field;
	qint32 head_field;
	qint32 tail_field;
	qint32 eTeX_aux_field;
	qint32 pg_field;
	qint32 ml_field;
	TeXMemoryWord aux_field;
	qint32 dirs_field;
	qint32 math_field;
	qint32 local_par_field;
	bool local_par_bool_field;
} TeXListStateRecord;

#define mode             (cur_list.mode_field)
#define head             (cur_list.head_field)
#define tail             (cur_list.tail_field)
#define eTeX_aux         (cur_list.eTeX_aux_field)
#define delim_ptr         eTeX_aux
#define prev_graf        (cur_list.pg_field)
#define aux              (cur_list.aux_field)
#define prev_depth       (aux.ii.CINT0)
#define space_factor     (aux.hh.v.LH)
#define clang            (aux.hh.v.RH)
#define incompleat_noad  (aux.ii.CINT0)
#define mode_line        (cur_list.ml_field)
#define dir_save         (cur_list.dirs_field)
#define dir_math_save    (cur_list.math_field)
#define local_par        (cur_list.local_par_field)
#define local_par_bool   (cur_list.local_par_bool_field)

typedef struct HashWord
{
	qint32 p;
  	HashWord * ptr;
  	TeXMemoryWord mw;
} TeXHashWord;

#define TEX_HASHTABLESIZE	23123

#define eq_level_field(mw) ((mw).hh.u.B1) //u16
#define eq_type_field(mw)  ((mw).hh.u.B0) //u16
#define equiv_field(mw)    ((mw).hh.v.RH)

#define TEX_LEVEL_ZERO   TEX_MIN_QUARTERWORD
#define TEX_LEVEL_ONE   (TEX_LEVEL_ZERO + 1)

#define TEX_ACTIVE_BASE                  1
#define TEX_SINGLE_BASE                 (TEX_ACTIVE_BASE + TEX_NUMBER_CHARS)
#define TEX_NULL_CS                     (TEX_SINGLE_BASE + TEX_NUMBER_CHARS)
#define TEX_HASH_BASE                   (TEX_NULL_CS + 1)
#define TEX_FROZEN_CONTROL_SEQUENCE     (TEX_HASH_BASE + TEX_HASH_SIZE)
#define TEX_FROZEN_PROTECTION           (TEX_FROZEN_CONTROL_SEQUENCE)
#define TEX_FROZEN_CR                   (TEX_FROZEN_CONTROL_SEQUENCE + 1)
#define TEX_FROZEN_END_GROUP            (TEX_FROZEN_CONTROL_SEQUENCE + 2)
#define TEX_FROZEN_RIGHT                (TEX_FROZEN_CONTROL_SEQUENCE + 3)
#define TEX_FROZEN_FI                   (TEX_FROZEN_CONTROL_SEQUENCE + 4)
#define TEX_FROZEN_END_TEMPLATE         (TEX_FROZEN_CONTROL_SEQUENCE + 5)
#define TEX_FROZEN_ENDV                 (TEX_FROZEN_CONTROL_SEQUENCE + 6)
#define TEX_FROZEN_RELAX                (TEX_FROZEN_CONTROL_SEQUENCE + 7)
#define TEX_END_WRITE                   (TEX_FROZEN_CONTROL_SEQUENCE + 8)
#define TEX_FROZEN_DONT_EXPAND          (TEX_FROZEN_CONTROL_SEQUENCE + 9)
#define TEX_FROZEN_NULL_FONT            (TEX_FROZEN_CONTROL_SEQUENCE + 10)
#define TEX_FONT_ID_BASE                (TEX_FROZEN_NULL_FONT - TEX_FONT_BASE)
#define TEX_FROZEN_NULL_OCP             (TEX_FROZEN_NULL_FONT + TEX_NUMBER_FONTS)
#define TEX_OCP_ID_BASE                 (TEX_FROZEN_NULL_OCP  - TEX_OCP_BASE)
#define TEX_FROZEN_NULL_OCP_LIST        (TEX_FROZEN_NULL_OCP  + TEX_NUMBER_OCPS)
#define TEX_OCP_LIST_ID_BASE            (TEX_FROZEN_NULL_OCP_LIST - TEX_OCP_LIST_BASE)
#define TEX_UNDEFINED_CONTROL_SEQUENCE  (TEX_FROZEN_NULL_OCP_LIST + TEX_NUMBER_OCP_LISTS)
#define TEX_GLUE_BASE                   (TEX_UNDEFINED_CONTROL_SEQUENCE + 1)

#define TEX_LINE_SKIP_CODE                 0
#define TEX_BASELINE_SKIP_CODE             1
#define TEX_PAR_SKIP_CODE                  2
#define TEX_ABOVE_DISPLAY_SKIP_CODE        3
#define TEX_BELOW_DISPLAY_SKIP_CODE        4
#define TEX_ABOVE_DISPLAY_SHORT_SKIP_CODE  5
#define TEX_BELOW_DISPLAY_SHORT_SKIP_CODE  6
#define TEX_LEFT_SKIP_CODE                 7
#define TEX_RIGHT_SKIP_CODE                8
#define TEX_TOP_SKIP_CODE                  9
#define TEX_SPLIT_TOP_SKIP_CODE            10
#define TEX_TAB_SKIP_CODE                  11
#define TEX_SPACE_SKIP_CODE                12
#define TEX_XSPACE_SKIP_CODE               13
#define TEX_PAR_FILL_SKIP_CODE             14
#define TEX_THIN_MU_SKIP_CODE              15
#define TEX_MED_MU_SKIP_CODE               16
#define TEX_THICK_MU_SKIP_CODE             17
#define TEX_GLUE_PARS                      18
#define TEX_SKIP_BASE                      (TEX_GLUE_BASE + TEX_GLUE_PARS)
#define TEX_MU_SKIP_BASE                   (TEX_SKIP_BASE + TEX_NUMBER_REGS)
#define TEX_LOCAL_BASE                     (TEX_MU_SKIP_BASE + TEX_NUMBER_REGS)

#define TEX_PAR_SHAPE_LOC                  TEX_LOCAL_BASE
#define TEX_INTER_LINE_PENALTIES_LOC      (TEX_LOCAL_BASE + 1)
#define TEX_CLUB_PENALTIES_LOC            (TEX_LOCAL_BASE + 2)
#define TEX_WIDOW_PENALTIES_LOC           (TEX_LOCAL_BASE + 3)
#define TEX_DISPLAY_WIDOW_PENALTIES_LOC   (TEX_LOCAL_BASE + 4)
#define TEX_TOKEN_BASE                    (TEX_LOCAL_BASE + 5)
#define TEX_OUTPUT_ROUTINE_LOC             TEX_TOKEN_BASE
#define TEX_EVERY_PAR_LOC                 (TEX_TOKEN_BASE + 1)
#define TEX_EVERY_MATH_LOC                (TEX_TOKEN_BASE + 2)
#define TEX_EVERY_DISPLAY_LOC             (TEX_TOKEN_BASE + 3)
#define TEX_EVERY_HBOX_LOC                (TEX_TOKEN_BASE + 4)
#define TEX_EVERY_VBOX_LOC                (TEX_TOKEN_BASE + 5)
#define TEX_EVERY_JOB_LOC                 (TEX_TOKEN_BASE + 6)
#define TEX_EVERY_CR_LOC                  (TEX_TOKEN_BASE + 7)
#define TEX_EVERY_EOF_LOC                 (TEX_TOKEN_BASE + 8)
#define TEX_ERR_HELP_LOC                  (TEX_TOKEN_BASE + 9)
#define TEX_OCP_TRACE_LEVEL_BASE          (TEX_TOKEN_BASE + 10)
#define TEX_OCP_ACTIVE_NUMBER_BASE        (TEX_OCP_TRACE_LEVEL_BASE + 1)
#define TEX_OCP_ACTIVE_MIN_PTR_BASE       (TEX_OCP_ACTIVE_NUMBER_BASE + 1)
#define TEX_OCP_ACTIVE_MAX_PTR_BASE       (TEX_OCP_ACTIVE_MIN_PTR_BASE + 1)
#define TEX_OCP_ACTIVE_BASE               (TEX_OCP_ACTIVE_MAX_PTR_BASE + 1)
#define TEX_OCP_INPUT_MODE_BASE           (TEX_OCP_ACTIVE_BASE + TEX_MAX_ACTIVE_OCP_LISTS)
#define TEX_OCP_INPUT_ONEBYTE_TRANSLATION_BASE    (TEX_OCP_INPUT_MODE_BASE + 1)
#define TEX_OCP_INPUT_EBCDIC_TRANSLATION_BASE     (TEX_OCP_INPUT_MODE_BASE + 2)
#define TEX_OCP_INPUT_TWOBYTE_TRANSLATION_BASE    (TEX_OCP_INPUT_MODE_BASE + 3)
#define TEX_OCP_INPUT_TWOBYTELE_TRANSLATION_BASE  (TEX_OCP_INPUT_MODE_BASE + 4)

#define TEX_OCP_OUTPUT_MODE_BASE                  (TEX_OCP_INPUT_MODE_BASE + 5)
#define TEX_OCP_OUTPUT_ONEBYTE_TRANSLATION_BASE   (TEX_OCP_INPUT_MODE_BASE + 6)
#define TEX_OCP_OUTPUT_EBCDIC_TRANSLATION_BASE    (TEX_OCP_INPUT_MODE_BASE + 7)
#define TEX_OCP_OUTPUT_TWOBYTE_TRANSLATION_BASE   (TEX_OCP_INPUT_MODE_BASE + 8)
#define TEX_OCP_OUTPUT_TWOBYTELE_TRANSLATION_BASE (TEX_OCP_INPUT_MODE_BASE + 9)

#define TEX_TOKS_BASE                             (TEX_OCP_INPUT_MODE_BASE + 10)
#define TEX_LOCAL_LEFT_BOX_BASE                   (TEX_TOKS_BASE + TEX_NUMBER_REGS)
#define TEX_LOCAL_RIGHT_BOX_BASE                  (TEX_LOCAL_LEFT_BOX_BASE + 1)
#define TEX_BOX_BASE                              (TEX_LOCAL_RIGHT_BOX_BASE + 1)
#define TEX_CUR_FONT_LOC                          (TEX_BOX_BASE + TEX_NUMBER_REGS)
#define TEX_MATH_FONT_BASE                        (TEX_CUR_FONT_LOC + 1)
#define TEX_CAT_CODE_BASE                         (TEX_MATH_FONT_BASE + TEX_NUMBER_MATH_FONTS)
#define TEX_LC_CODE_BASE                          (TEX_CAT_CODE_BASE + TEX_NUMBER_CHARS)
#define TEX_UC_CODE_BASE                          (TEX_LC_CODE_BASE + TEX_NUMBER_CHARS)
#define TEX_SF_CODE_BASE                          (TEX_UC_CODE_BASE + TEX_NUMBER_CHARS)
#define TEX_MATH_CODE_BASE                        (TEX_SF_CODE_BASE + TEX_NUMBER_CHARS)
#define TEX_INT_BASE                              (TEX_MATH_CODE_BASE + TEX_NUMBER_CHARS)

#define TEX_NULL_FONT      TEX_FONT_BASE
#define TEX_NULL_OCP       TEX_OCP_BASE
#define TEX_NULL_OCP_LIST  TEX_OCP_LIST_BASE
#define TEX_VAR_CODE       0x7000000

#define TEX_PRETOLERANCE_CODE           0
#define TEX_TOLERANCE_CODE              1
#define TEX_LINE_PENALTY_CODE           2
#define TEX_HYPHEN_PENALTY_CODE         3
#define TEX_EX_HYPHEN_PENALTY_CODE      4
#define TEX_CLUB_PENALTY_CODE           5
#define TEX_WIDOW_PENALTY_CODE          6
#define TEX_DISPLAY_WIDOW_PENALTY_CODE  7
#define TEX_BROKEN_PENALTY_CODE         8
#define TEX_BIN_OP_PENALTY_CODE         9
#define TEX_REL_PENALTY_CODE            10
#define TEX_PRE_DISPLAY_PENALTY_CODE    11
#define TEX_POST_DISPLAY_PENALTY_CODE   12
#define TEX_INTER_LINE_PENALTY_CODE     13
#define TEX_DOUBLE_HYPHEN_DEMERITS_CODE 14
#define TEX_FINAL_HYPHEN_DEMERITS_CODE  15
#define TEX_ADJ_DEMERITS_CODE           16
#define TEX_MAG_CODE                    17
#define TEX_DELIMITER_FACTOR_CODE       18
#define TEX_LOOSENESS_CODE              19
#define TEX_TIME_CODE                   20
#define TEX_DAY_CODE                    21
#define TEX_MONTH_CODE                  22
#define TEX_YEAR_CODE                   23
#define TEX_SHOW_BOX_BREADTH_CODE       24
#define TEX_SHOW_BOX_DEPTH_CODE         25
#define TEX_HBADNESS_CODE               26
#define TEX_VBADNESS_CODE               27
#define TEX_PAUSING_CODE                28
#define TEX_TRACING_ONLINE_CODE         29
#define TEX_TRACING_MACROS_CODE         30
#define TEX_TRACING_STATS_CODE          31
#define TEX_TRACING_PARAGRAPHS_CODE     32
#define TEX_TRACING_PAGES_CODE          33
#define TEX_TRACING_OUTPUT_CODE         34
#define TEX_TRACING_LOST_CHARS_CODE     35
#define TEX_TRACING_COMMANDS_CODE       36
#define TEX_TRACING_RESTORES_CODE       37
#define TEX_UC_HYPH_CODE                38
#define TEX_OUTPUT_PENALTY_CODE         39
#define TEX_MAX_DEAD_CYCLES_CODE        40
#define TEX_HANG_AFTER_CODE             41
#define TEX_FLOATING_PENALTY_CODE       42
#define TEX_GLOBAL_DEFS_CODE            43
#define TEX_CUR_FAM_CODE                44
#define TEX_ESCAPE_CHAR_CODE            45
#define TEX_DEFAULT_HYPHEN_CHAR_CODE    46
#define TEX_DEFAULT_SKEW_CHAR_CODE      47
#define TEX_END_LINE_CHAR_CODE          48
#define TEX_NEW_LINE_CHAR_CODE          49
#define TEX_LANGUAGE_CODE               50
#define TEX_LEFT_HYPHEN_MIN_CODE        51
#define TEX_RIGHT_HYPHEN_MIN_CODE       52
#define TEX_HOLDING_INSERTS_CODE        53
#define TEX_ERROR_CONTEXT_LINES_CODE    54
#define TEX_LOCAL_INTER_LINE_PENALTY_CODE 55
#define TEX_LOCAL_BROKEN_PENALTY_CODE     56
#define TEX_NO_LOCAL_WHATSITS_CODE        57
#define TEX_NO_LOCAL_DIRS_CODE            58
#define TEX_LEVEL_LOCAL_DIR_CODE          59
#define TEX_INT_PARS                      61
#define TEX_TRACING_ASSIGNS_CODE           TEX_INT_PARS
#define TEX_TRACING_GROUPS_CODE           (TEX_INT_PARS + 1)
#define TEX_TRACING_IFS_CODE              (TEX_INT_PARS + 2)
#define TEX_TRACING_SCAN_TOKENS_CODE      (TEX_INT_PARS + 3)
#define TEX_TRACING_NESTING_CODE          (TEX_INT_PARS + 4)
#define TEX_PRE_DISPLAY_DIRECTION_CODE    (TEX_INT_PARS + 5)
#define TEX_LAST_LINE_FIT_CODE            (TEX_INT_PARS + 6)
#define TEX_SAVING_VDISCARDS_CODE         (TEX_INT_PARS + 7)
#define TEX_SAVING_HYPH_CODES_CODE        (TEX_INT_PARS + 8)
#define TEX_ETEX_STATE_CODE               (TEX_INT_PARS + 9)
#define TEX_DIR_BASE                      (TEX_INT_BASE + TEX_ETEX_STATE_CODE + TEX_ETEX_STATES)
#define TEX_PAGE_DIRECTION_CODE            0
#define TEX_BODY_DIRECTION_CODE            1
#define TEX_PAR_DIRECTION_CODE             2
#define TEX_TEXT_DIRECTION_CODE            3
#define TEX_MATH_DIRECTION_CODE            4
#define TEX_DIR_PARS                       5
#define TEX_COUNT_BASE                    (TEX_DIR_BASE   + TEX_DIR_PARS)
#define TEX_DEL_CODE_BASE                 (TEX_COUNT_BASE + TEX_NUMBER_REGS)
#define TEX_DIMEN_BASE                    (TEX_DEL_CODE_BASE + TEX_NUMBER_CHARS)


#define TEX_PAR_INDENT_CODE           0
#define TEX_MATH_SURROUND_CODE        1
#define TEX_LINE_SKIP_LIMIT_CODE      2
#define TEX_HSIZE_CODE                3
#define TEX_VSIZE_CODE                4
#define TEX_MAX_DEPTH_CODE            5
#define TEX_SPLIT_MAX_DEPTH_CODE      6
#define TEX_BOX_MAX_DEPTH_CODE        7
#define TEX_HFUZZ_CODE                8
#define TEX_VFUZZ_CODE                9
#define TEX_DELIMITER_SHORTFALL_CODE  10
#define TEX_NULL_DELIMITER_SPACE_CODE 11
#define TEX_SCRIPT_SPACE_CODE         12
#define TEX_PRE_DISPLAY_SIZE_CODE     13
#define TEX_DISPLAY_WIDTH_CODE        14
#define TEX_DISPLAY_INDENT_CODE       15
#define TEX_OVERFULL_RULE_CODE        16
#define TEX_HANG_INDENT_CODE          17
#define TEX_H_OFFSET_CODE             18
#define TEX_V_OFFSET_CODE             19
#define TEX_EMERGENCY_STRETCH_CODE    20
#define TEX_PAGE_WIDTH_CODE           21
#define TEX_PAGE_HEIGHT_CODE          22
#define TEX_PAGE_RIGHT_OFFSET_CODE    23
#define TEX_PAGE_BOTTOM_OFFSET_CODE   24
#define TEX_DIMEN_PARS                25
#define TEX_SCALED_BASE              (TEX_DIMEN_BASE  + TEX_DIMEN_PARS)
#define TEX_EQTB_SIZE                (TEX_SCALED_BASE + TEX_BIGGEST_REG)

#define save_type(s)   (save_stack[(s)].hh.u.B0) //u16
#define save_level(s)  (save_stack[(s)].hh.u.B1) //u16
#define save_index(s)  (save_stack[(s)].hh.v.RH)

#define TEX_RESTORE_OLD_VALUE 0
#define TEX_RESTORE_ZERO      1
#define TEX_INSERT_TOKEN      2
#define TEX_LEVEL_BOUNDARY    3
#define TEX_RESTORE_SA        4

#define TEX_BOTTOM_LEVEL        0
#define TEX_SIMPLE_GROUP        1
#define TEX_HBOX_GROUP          2
#define TEX_ADJUSTED_HBOX_GROUP 3
#define TEX_VBOX_GROUP          4
#define TEX_VTOP_GROUP          5
#define TEX_ALIGN_GROUP         6
#define TEX_NO_ALIGN_GROUP      7
#define TEX_OUTPUT_GROUP        8
#define TEX_MATH_GROUP          9
#define TEX_DISC_GROUP          10
#define TEX_INSERT_GROUP        11
#define TEX_VCENTER_GROUP       12
#define TEX_MATH_CHOICE_GROUP   13
#define TEX_SEMI_SIMPLE_GROUP   14
#define TEX_MATH_SHIFT_GROUP    15
#define TEX_MATH_LEFT_GROUP     16
#define TEX_LOCAL_BOX_GROUP     17
#define TEX_MAX_GROUP_CODE      17

#define saved(s)    (save_stack[save_ptr + (s)].ii.CINT0)

#define TEX_CS_TOKEN_FLAG      0xFFFFF
#define TEX_MAX_CHAR_VAL       0x10000
#define TEX_LEFT_BRACE_TOKEN   0x10000
#define TEX_LEFT_BRACE_LIMIT   0x20000
#define TEX_RIGHT_BRACE_TOKEN  0x20000
#define TEX_RIGHT_BRACE_LIMIT  0x30000
#define TEX_MATH_SHIFT_TOKEN   0x30000
#define TEX_TAB_TOKEN          0x40000
#define TEX_OUT_PARAM_TOKEN    0x50000
#define TEX_SPACE_TOKEN        0xA0020
#define TEX_LETTER_TOKEN       0xB0000
#define TEX_OTHER_TOKEN        0xC0000
#define TEX_MATCH_TOKEN        0xD0000
#define TEX_END_MATCH_TOKEN    0xE0000
#define TEX_PROTECTED_TOKEN    0xE0001


struct TeXInStateRecord
{
	qint32 state_field;
	qint32 index_field;
	qint32 start_field;
	qint32 loc_field;
	qint32 limit_field;
	qint32 name_field;
	qint32 ocp_lstack_field;
	qint32 ocp_no_field;
};

#define state                   (cur_input.state_field)
#define index                   (cur_input.index_field)
#define texstart                   (cur_input.start_field)
#define limit                   (cur_input.limit_field)
#define texname                    (cur_input.name_field)
#define current_ocp_lstack      (cur_input.ocp_lstack_field)
#define current_ocp_no          (cur_input.ocp_no_field)

#define TEX_MID_LINE      1
#define TEX_SKIP_BLANKS  (2 + TEX_MAX_CHAR_CODE)
#define TEX_NEW_LINE     (3 + TEX_MAX_CHAR_CODE + TEX_MAX_CHAR_CODE)

#define terminal_input (texname == 0)
#define cur_file       (input_file[index])

#define edit_file (input_stack[base_ptr])

#define TEX_SKIPPING  1
#define TEX_DEFINING  2
#define TEX_MATCHING  3
#define TEX_ALIGNING  4
#define TEX_ABSORBING 5

#define TEX_TOKEN_LIST    0

#define token_type       (index)
#define param_start      (limit)

#define TEX_PARAMETER          0
#define TEX_U_TEMPLATE         1
#define TEX_V_TEMPLATE         2
#define TEX_BACKED_UP          3
#define TEX_INSERTED           4
#define TEX_MACRO              5
#define TEX_OUTPUT_TEXT        6
#define TEX_EVERY_PAR_TEXT     7
#define TEX_EVERY_MATH_TEXT    8
#define TEX_EVERY_DISPLAY_TEXT 9
#define TEX_EVERY_HBOX_TEXT    10
#define TEX_EVERY_VBOX_TEXT    11
#define TEX_EVERY_JOB_TEXT     12
#define TEX_EVERY_CR_TEXT      13
#define TEX_EVERY_EOF_TEXT     14
#define TEX_MARK_TEXT          15
#define TEX_WRITE_TEXT         16

#define TEX_NO_EXPAND_FLAG  TEX_SPECIAL_CHAR

#define TEX_MARKS_CODE            5
#define TEX_TOP_MARK_CODE         0
#define TEX_FIRST_MARK_CODE       1
#define TEX_BOT_MARK_CODE         2
#define TEX_SPLIT_FIRST_MARK_CODE 3
#define TEX_SPLIT_BOT_MARK_CODE   4

#define top_mark          cur_mark[TEX_TOP_MARK_CODE]
#define first_mark        cur_mark[TEX_FIRST_MARK_CODE]
#define bot_mark          cur_mark[TEX_BOT_MARK_CODE]
#define split_first_mark  cur_mark[TEX_SPLIT_FIRST_MARK_CODE]
#define split_bot_mark    cur_mark[TEX_SPLIT_BOT_MARK_CODE]

#define TEX_INT_VAL   0
#define TEX_DIMEN_VAL 1
#define TEX_GLUE_VAL  2
#define TEX_MU_VAL    3
#define TEX_DIR_VAL   4
#define TEX_IDENT_VAL 5
#define TEX_TOK_VAL   6

#define TEX_LAST_NODE_TYPE_CODE (TEX_GLUE_VAL + 1)
#define TEX_INPUT_LINE_NO_CODE  (TEX_GLUE_VAL + 2)
#define TEX_BADNESS_CODE        (TEX_GLUE_VAL + 3)
#define TEX_ALEPH_INT           (TEX_GLUE_VAL + 4)
#define TEX_ALEPH_INT_NUM        5
#define TEX_ETEX_INT            (TEX_ALEPH_INT + TEX_ALEPH_INT_NUM)
#define TEX_ETEX_DIM            (TEX_ETEX_INT + 8)
#define TEX_ETEX_GLUE           (TEX_ETEX_DIM + 9)
#define TEX_ETEX_MU             (TEX_ETEX_GLUE + 1)
#define TEX_ETEX_EXPR           (TEX_ETEX_MU + 1)

#define TEX_MAX_DIMEN   0x3FFFFFFF

#define TEX_DIR_T  0
#define TEX_DIR_L  1
#define TEX_DIR_B  2
#define TEX_DIR_R  3

#define dir_eq(d1, d2)          ((d1) == (d2))
#define dir_ne(d1, d2)          ((d1) != (d2))
#define dir_opposite(d1, d2)    ((((d1)+2) % 4) == ((d2) % 4))
#define dir_parallel(d1, d2)    (((d1) % 2) == ((d2) % 2))
#define dir_orthogonal(d1, d2)  (((d1) % 2) != ((d2) % 2))
#define dir_next(d1, d2)        ((((d1)+3) % 4) == ((d2) % 4))
#define dir_prev(d1, d2)        ((((d1)+1) % 4) == ((d2) % 4))

#define TEX_DIR_TL_  0
#define TEX_DIR_TR_  1
#define TEX_DIR_LT_  2
#define TEX_DIR_LB_  3
#define TEX_DIR_BL_  4
#define TEX_DIR_BR_  5
#define TEX_DIR_RT_  6
#define TEX_DIR_RB_  7

#define TEX_DIR__LT  0
#define TEX_DIR__LL  1
#define TEX_DIR__LB  2
#define TEX_DIR__LR  3
#define TEX_DIR__RT  4
#define TEX_DIR__RL  5
#define TEX_DIR__RB  6
#define TEX_DIR__RR  7
#define TEX_DIR__TT  8
#define TEX_DIR__TL  9
#define TEX_DIR__TB  10
#define TEX_DIR__TR  11
#define TEX_DIR__BT  12
#define TEX_DIR__BL  13
#define TEX_DIR__BB  14
#define TEX_DIR__BR  15

#define is_mirrored(p)     (dir_opposite(dir_primary[(p)], dir_tertiary[(p)]))
#define is_rotated(p)      (dir_parallel(dir_secondary[(p)], dir_tertiary[(p)]))
#define font_direction(p)  ((p) % 16) 
#define box_direction(p)   ((p) / 4)

#define TEX_OCTAL_TOKEN             (TEX_OTHER_TOKEN + '\'')
#define TEX_HEX_TOKEN               (TEX_OTHER_TOKEN + '"')
#define TEX_ALPHA_TOKEN             (TEX_OTHER_TOKEN + '`')
#define TEX_POINT_TOKEN             (TEX_OTHER_TOKEN + '.')
#define TEX_CONTINENTAL_POINT_TOKEN (TEX_OTHER_TOKEN + ',')

#define TEX_INFINITY   0x7FFFFFFF

#define TEX_ZERO_TOKEN      (TEX_OTHER_TOKEN  + '0')
#define TEX_A_TOKEN         (TEX_LETTER_TOKEN + 'A')
#define TEX_OTHER_A_TOKEN   (TEX_OTHER_TOKEN  + 'A')

#define TEX_ATTACH_FRACTION 88
#define TEX_ATTACH_SIGN     89

#define TEX_DEFAULT_RULE    26214

#define TEX_NUMBER_CODE        0
#define TEX_ROMAN_NUMERAL_CODE 1
#define TEX_STRING_CODE        2
#define TEX_MEANING_CODE       3
#define TEX_FONT_NAME_CODE     4
#define TEX_ETEX_CODE          5
#define TEX_OMEGA_CODE         6
#define TEX_ALEPH_CODE         7
#define TEX_JOB_NAME_CODE      8

#define TEX_CLOSED     2
#define TEX_JUST_OPEN  1

#define TEX_UNLESS_CODE  32

#define TEX_IF_CHAR_CODE  0
#define TEX_IF_CAT_CODE   1
#define TEX_IF_INT_CODE   2
#define TEX_IF_DIM_CODE   3
#define TEX_IF_ODD_CODE   4
#define TEX_IF_VMODE_CODE 5
#define TEX_IF_HMODE_CODE 6
#define TEX_IF_MMODE_CODE 7
#define TEX_IF_INNER_CODE 8
#define TEX_IF_VOID_CODE  9 
#define TEX_IF_HBOX_CODE  10
#define TEX_IF_VBOX_CODE  11
#define TEX_IFX_CODE      12
#define TEX_IF_EOF_CODE   13
#define TEX_IF_TRUE_CODE  14
#define TEX_IF_FALSE_CODE 15
#define TEX_IF_CASE_CODE  16

#define TEX_IF_NODE_SIZE   2

#define if_line_field(p)  (mem[(p) + 1].ii.CINT0)

#define TEX_IF_CODE   1
#define TEX_FI_CODE   2
#define TEX_ELSE_CODE 3
#define TEX_OR_CODE   4

#define TEX_FORMAT_AREA_LENGTH    11
#define TEX_FORMAT_EXT_LENGTH     5

#define TEX_NO_TAG    0
#define TEX_LIG_TAG   1
#define TEX_LIST_TAG  2
#define TEX_EXT_TAG   3

#define TEX_STOP_FLAG 128
#define TEX_KERN_FLAG 128

#define font_info(i, j)  (fonttables[(i)][(j)])

#define TEX_OFFSET_FILE_SIZE         0
#define TEX_OFFSET_CHECK             1
#define TEX_OFFSET_OFFSET           (TEX_OFFSET_CHECK  + 4)
#define TEX_OFFSET_SIZE             (TEX_OFFSET_OFFSET + 1)
#define TEX_OFFSET_DSIZE            (TEX_OFFSET_SIZE + 1)
#define TEX_OFFSET_PARAMS           (TEX_OFFSET_DSIZE + 1)
#define TEX_OFFSET_NAME_SORT        (TEX_OFFSET_PARAMS + 1)
#define TEX_OFFSET_NAME             (TEX_OFFSET_NAME_SORT + 1)
#define TEX_OFFSET_AREA             (TEX_OFFSET_NAME + 1)
#define TEX_OFFSET_BC               (TEX_OFFSET_AREA + 1)
#define TEX_OFFSET_EC               (TEX_OFFSET_BC + 1)
#define TEX_OFFSET_GLUE             (TEX_OFFSET_EC + 1)
#define TEX_OFFSET_USED             (TEX_OFFSET_GLUE + 1)
#define TEX_OFFSET_HYPHEN           (TEX_OFFSET_USED + 1)
#define TEX_OFFSET_SKEW             (TEX_OFFSET_HYPHEN + 1)
#define TEX_OFFSET_BCHAR_LABEL      (TEX_OFFSET_SKEW + 1)
#define TEX_OFFSET_BCHAR            (TEX_OFFSET_BCHAR_LABEL + 1)
#define TEX_OFFSET_FALSE_BCHAR      (TEX_OFFSET_BCHAR + 1)
#define TEX_OFFSET_NATURAL_DIR      (TEX_OFFSET_FALSE_BCHAR + 1)
#define TEX_OFFSET_IVALUES_START    (TEX_OFFSET_NATURAL_DIR + 1)
#define TEX_OFFSET_FVALUES_START    (TEX_OFFSET_IVALUES_START + 1)
#define TEX_OFFSET_MVALUES_START    (TEX_OFFSET_FVALUES_START + 1)
#define TEX_OFFSET_RULES_START      (TEX_OFFSET_MVALUES_START + 1)
#define TEX_OFFSET_GLUES_START      (TEX_OFFSET_RULES_START + 1)
#define TEX_OFFSET_PENALTIES_START  (TEX_OFFSET_GLUES_START + 1)
#define TEX_OFFSET_IVALUES_BASE     (TEX_OFFSET_PENALTIES_START + 1)
#define TEX_OFFSET_FVALUES_BASE     (TEX_OFFSET_IVALUES_BASE + 1)
#define TEX_OFFSET_MVALUES_BASE     (TEX_OFFSET_FVALUES_BASE + 1)
#define TEX_OFFSET_RULES_BASE       (TEX_OFFSET_MVALUES_BASE + 1)
#define TEX_OFFSET_GLUES_BASE       (TEX_OFFSET_RULES_BASE + 1)
#define TEX_OFFSET_PENALTIES_BASE   (TEX_OFFSET_GLUES_BASE + 1)
#define TEX_OFFSET_CHAR_BASE        (TEX_OFFSET_PENALTIES_BASE + 1)
#define TEX_OFFSET_CHAR_ATTR_BASE   (TEX_OFFSET_CHAR_BASE + 1)
#define TEX_OFFSET_WIDTH_BASE       (TEX_OFFSET_CHAR_ATTR_BASE + 1)
#define TEX_OFFSET_HEIGHT_BASE      (TEX_OFFSET_WIDTH_BASE + 1)
#define TEX_OFFSET_DEPTH_BASE       (TEX_OFFSET_HEIGHT_BASE + 1)
#define TEX_OFFSET_ITALIC_BASE      (TEX_OFFSET_DEPTH_BASE + 1)
#define TEX_OFFSET_LIG_KERN_BASE    (TEX_OFFSET_ITALIC_BASE + 1)
#define TEX_OFFSET_KERN_BASE        (TEX_OFFSET_LIG_KERN_BASE + 1)
#define TEX_OFFSET_EXTEN_BASE       (TEX_OFFSET_KERN_BASE + 1)
#define TEX_OFFSET_PARAM_BASE       (TEX_OFFSET_EXTEN_BASE + 1)
#define TEX_OFFSET_CHARINFO_BASE    (TEX_OFFSET_PARAM_BASE + 1)

#define  font_file_size(i)    (font_info((i), TEX_OFFSET_FILE_SIZE).ii.CINT0)
#define  font_check_0(i)      (font_info((i), TEX_OFFSET_CHECK).ii.CINT0)
#define  font_check_1(i)      (font_info((i), TEX_OFFSET_CHECK + 1).ii.CINT0)
#define  font_check_2(i)      (font_info((i), TEX_OFFSET_CHECK + 2).ii.CINT0)
#define  font_check_3(i)      (font_info((i), TEX_OFFSET_CHECK + 3).ii.CINT0)
#define  font_offset(i)       (font_info((i), TEX_OFFSET_OFFSET).ii.CINT0)
#define  font_size(i)         (font_info((i), TEX_OFFSET_SIZE).ii.CINT0)
#define  font_dsize(i)        (font_info((i), TEX_OFFSET_DSIZE).ii.CINT0)
#define  font_params(i)       (font_info((i), TEX_OFFSET_PARAMS).ii.CINT0)
#define  font_name_sort(i)    (font_info((i), TEX_OFFSET_NAME_SORT).ii.CINT0)
#define  font_name(i)         (font_info((i), TEX_OFFSET_NAME).ii.CINT0)
#define  font_area(i)         (font_info((i), TEX_OFFSET_AREA).ii.CINT0)
#define  font_bc(i)           (font_info((i), TEX_OFFSET_BC).ii.CINT0)
#define  font_ec(i)           (font_info((i), TEX_OFFSET_EC).ii.CINT0)
#define  font_glue(i)         (font_info((i), TEX_OFFSET_GLUE).ii.CINT0)
#define  font_used(i)         (font_info((i), TEX_OFFSET_USED).ii.CINT0)
#define  hyphen_char(i)       (font_info((i), TEX_OFFSET_HYPHEN).ii.CINT0)
#define  skew_char(i)         (font_info((i), TEX_OFFSET_SKEW).ii.CINT0)
#define  bchar_label(i)       (font_info((i), TEX_OFFSET_BCHAR_LABEL).ii.CINT0)
#define  font_bchar(i)        (font_info((i), TEX_OFFSET_BCHAR).ii.CINT0)
#define  font_false_bchar(i)  (font_info((i), TEX_OFFSET_FALSE_BCHAR).ii.CINT0)
#define  font_natural_dir(i)  (font_info((i), TEX_OFFSET_NATURAL_DIR).ii.CINT0)
#define  ivalues_start(i)     (font_info((i), TEX_OFFSET_IVALUES_START).ii.CINT0)
#define  fvalues_start(i)     (font_info((i), TEX_OFFSET_FVALUES_START).ii.CINT0)
#define  mvalues_start(i)     (font_info((i), TEX_OFFSET_MVALUES_START).ii.CINT0)
#define  rules_start(i)       (font_info((i), TEX_OFFSET_RULES_START).ii.CINT0)
#define  glues_start(i)       (font_info((i), TEX_OFFSET_GLUES_START).ii.CINT0)
#define  penalties_start(i)   (font_info((i), TEX_OFFSET_PENALTIES_START).ii.CINT0)
#define  ivalues_base(i)      (font_info((i), TEX_OFFSET_IVALUES_BASE).ii.CINT0)
#define  fvalues_base(i)      (font_info((i), TEX_OFFSET_FVALUES_BASE).ii.CINT0)
#define  mvalues_base(i)      (font_info((i), TEX_OFFSET_MVALUES_BASE).ii.CINT0)
#define  rules_base(i)        (font_info((i), TEX_OFFSET_RULES_BASE).ii.CINT0)
#define  glues_base(i)        (font_info((i), TEX_OFFSET_GLUES_BASE).ii.CINT0)
#define  penalties_base(i)    (font_info((i), TEX_OFFSET_PENALTIES_BASE).ii.CINT0)
#define  char_attr_base(i)    (font_info((i), TEX_OFFSET_CHAR_ATTR_BASE).ii.CINT0)
#define  char_base(i)         (font_info((i), TEX_OFFSET_CHAR_BASE).ii.CINT0)
#define  width_base(i)        (font_info((i), TEX_OFFSET_WIDTH_BASE).ii.CINT0)
#define  height_base(i)       (font_info((i), TEX_OFFSET_HEIGHT_BASE).ii.CINT0)
#define  depth_base(i)        (font_info((i), TEX_OFFSET_DEPTH_BASE).ii.CINT0)
#define  italic_base(i)       (font_info((i), TEX_OFFSET_ITALIC_BASE).ii.CINT0)
#define  lig_kern_base(i)     (font_info((i), TEX_OFFSET_LIG_KERN_BASE).ii.CINT0)
#define  kern_base(i)         (font_info((i), TEX_OFFSET_KERN_BASE).ii.CINT0)
#define  exten_base(i)        (font_info((i), TEX_OFFSET_EXTEN_BASE).ii.CINT0)
#define  param_base(i)        (font_info((i), TEX_OFFSET_PARAM_BASE).ii.CINT0)

#define top_skip_byte(f)  ((f.u.B0) / 256)
#define skip_byte(f)      ((f.u.B0) % 256)

#define next_char(b, o)      (font_offset(b) + (o.u.B1))
#define op_byte(f)           (f.u.B2) //u16
#define rem_byte(f)          (f.u.B3) //u16
#define rem_char_byte(b, o)  (font_offset(b) + (o.u.B3))
#define rem_top_byte(f)      ((f.u.B3) / 256) //u16
#define rem_bot_byte(f)      ((f.u.B3) % 256) //u16

#define ext_top(b, o)  (font_offset(b) + (o.u.B0))
#define ext_mid(b, o)  (font_offset(b) + (o.u.B1))
#define ext_bot(b, o)  (font_offset(b) + (o.u.B2))
#define ext_rep(b, o)  (font_offset(b) + (o.u.B3))

#define TEX_SLANT_CODE         1
#define TEX_SPACE_CODE         2
#define TEX_SPACE_STRETCH_CODE 3
#define TEX_SPACE_SHRINK_CODE  4
#define TEX_X_HEIGHT_CODE      5
#define TEX_QUAD_CODE          6
#define TEX_EXTRA_SPACE_CODE   7

#define TEX_NON_CHAR     TEX_TOO_BIG_CHAR
#define TEX_NON_ADDRESS  0

#define char_info(i, j)   (fonttables[i][char_base(i) - font_offset(i) + j].qqqq)
#define char_width(i, f)  (fonttables[i][width_base(i) + (f.u.B0)].ii.CINT0)
#define char_exists(f)    ((f.u.B0) > (quint16)TEX_MIN_QUARTERWORD)
#define char_italic(i, f) (fonttables[i][italic_base(i) + ((f.u.B2) / 256)].ii.CINT0)
#define height_depth(f)   (f.u.B1) //u16
#define char_height(i, j) (fonttables[i][height_base(i)+ ((j) / 256)].ii.CINT0)
#define char_depth(i, j)  (fonttables[i][depth_base(i) + ((j) % 256)].ii.CINT0)
#define char_tag(f)       ((f.u.B2) % 4) //u16

#define char_kern(i, f)   (fonttables[i][kern_base(i) + (256 * op_byte(f) + rem_byte(f))].ii.CINT0)

#define TEX_KERN_BASE_OFFSET  (256 * (128 + TEX_MIN_QUARTERWORD))

#define lig_kern_start(i, f)    (lig_kern_base(i) + rem_byte(f))
#define lig_kern_restart(i, f)  (lig_kern_base(i) + (256 * op_byte(f)+ rem_byte(f) + 32768 - TEX_KERN_BASE_OFFSET))

#define attr_zero_char_ivalue(i, j)   (fonttables[i][fonttables[i][char_attr_base(i) + j].ii.CINT0].ii.CINT0)
#define attr_zero_ivalue(i, j)        (fonttables[i][ivalues_base(i) + j].ii.CINT0)
#define attr_zero_penalty(i, j)       (fonttables[i][penalties_base(i) + j].ii.CINT0)
#define attr_zero_kern(i, j)          (fonttables[i][mvalues_base(i) + j].ii.CINT0)

#define slant(c)          (font_info(c, param_base(c) + TEX_SLANT_CODE).ii.CINT0)
#define space(c)          (font_info(c, param_base(c) + TEX_SPACE_CODE).ii.CINT0)
#define space_stretch(c)  (font_info(c, param_base(c) + TEX_SPACE_STRETCH_CODE).ii.CINT0)
#define space_shrink(c)   (font_info(c, param_base(c) + TEX_SPACE_SHRINK_CODE).ii.CINT0)
#define x_height(c)       (font_info(c, param_base(c) + TEX_X_HEIGHT_CODE).ii.CINT0)
#define quad(c)           (font_info(c, param_base(c) + TEX_QUAD_CODE).ii.CINT0)
#define extra_space(c)    (font_info(c, param_base(c) + TEX_EXTRA_SPACE_CODE).ii.CINT0)

#define active_ocp(p)        (active_info[(p)].hh.u.B0) //u16
#define active_counter(p)    (active_info[(p)].hh.u.B1) //u16
#define active_lstack_no(p)  (active_info[(p) + 1].ii.CINT0)


#define ocp_info(i, j)  (ocptables[(i)][(j)])

#define TEX_OFFSET_OCP_FILE_SIZE     0
#define TEX_OFFSET_OCP_NAME          1
#define TEX_OFFSET_OCP_AREA         (TEX_OFFSET_OCP_NAME + 1)
#define TEX_OFFSET_OCP_EXTERNAL     (TEX_OFFSET_OCP_AREA + 1)
#define TEX_OFFSET_OCP_EXTERNAL_ARG (TEX_OFFSET_OCP_EXTERNAL + 1)
#define TEX_OFFSET_OCP_INPUT        (TEX_OFFSET_OCP_EXTERNAL_ARG + 1)
#define TEX_OFFSET_OCP_OUTPUT       (TEX_OFFSET_OCP_INPUT + 1)
#define TEX_OFFSET_OCP_NO_TABLES    (TEX_OFFSET_OCP_OUTPUT + 1)
#define TEX_OFFSET_OCP_NO_STATES    (TEX_OFFSET_OCP_NO_TABLES + 1)
#define TEX_OFFSET_OCP_TABLE_BASE   (TEX_OFFSET_OCP_NO_STATES + 1)
#define TEX_OFFSET_OCP_STATE_BASE   (TEX_OFFSET_OCP_TABLE_BASE + 1)
#define TEX_OFFSET_OCP_INFO         (TEX_OFFSET_OCP_STATE_BASE + 1)

#define ocp_file_size(i)     (ocp_info(i, TEX_OFFSET_OCP_FILE_SIZE))
#define ocp_name(i)          (ocp_info(i, TEX_OFFSET_OCP_NAME))
#define ocp_area(i)          (ocp_info(i, TEX_OFFSET_OCP_AREA))
#define ocp_external(i)      (ocp_info(i, TEX_OFFSET_OCP_EXTERNAL))
#define ocp_external_arg(i)  (ocp_info(i, TEX_OFFSET_OCP_EXTERNAL_ARG))
#define ocp_input(i)         (ocp_info(i, TEX_OFFSET_OCP_INPUT))
#define ocp_output(i)        (ocp_info(i, TEX_OFFSET_OCP_OUTPUT))
#define ocp_no_tables(i)     (ocp_info(i, TEX_OFFSET_OCP_NO_TABLES))
#define ocp_no_states(i)     (ocp_info(i, TEX_OFFSET_OCP_NO_STATES))
#define ocp_table_base(i)    (ocp_info(i, TEX_OFFSET_OCP_TABLE_BASE))
#define ocp_state_base(i)    (ocp_info(i, TEX_OFFSET_OCP_STATE_BASE))

#define ocp_state_entry(i, j, k) (ocptables[(i)][ocptables[(i)][ocp_state_base(i) + (j) * 2] + (k)])

#define ocp_state_no(i, j)   (ocptables[(i)][ocp_state_base(i) + (j) * 2 + 1])

#define ocp_table_entry(i, j, k)  (ocptables[(i)][ocptables[(i)][ocp_table_base(i) + (j) * 2] + (k)])
#define ocp_table_no(i, j)        (ocptables[(i)][ocp_table_base(i) + (j) * 2 + 1])

#define ocp_list_lnext(p)      (ocp_list_info[(p)].hh.u.B0) //u16
#define ocp_list_lstack(p)     (ocp_list_info[(p)].hh.u.B1) //u16
#define ocp_list_lstack_no(p)  (ocp_list_info[(p) + 1].ii.CINT0)
#define ocp_lstack_lnext(p)    (ocp_lstack_info[(p)].hh.u.B0) //u16
#define ocp_lstack_ocp(p)      (ocp_lstack_info[(p)].hh.u.B1) //u16

#define TEX_MAKE_NULL_OCP_LSTACK  0
#define TEX_ADD_BEFORE_OP         1
#define TEX_ADD_AFTER_OP          2
#define TEX_REMOVE_BEFORE_OP      3
#define TEX_REMOVE_AFTER_OP       4

#define TEX_MOVEMENT_NODE_SIZE    3

#define location(p)    (mem[(p) + 2].ii.CINT0)

#define TEX_Y_HERE  1
#define TEX_Z_HERE  2
#define TEX_YZ_OK   3 
#define TEX_Y_OK    4
#define TEX_Z_OK    5
#define TEX_D_FIXED 6

#define TEX_NONE_SEEN 0
#define TEX_Y_SEEN    6
#define TEX_Z_SEEN    12

#define TEX_MOVE_PAST 13
#define TEX_FIN_RULE  14
#define TEX_NEXT_P    15

#define TEX_BILLION   1000000000.0

#define TEX_EXACTLY    0
#define TEX_ADDITIONAL 1

#define TEX_NOAD_SIZE 4

#define nucleus(p)    ((p) + 1)
#define supscr(p)     ((p) + 2)
#define subscr(p)     ((p) + 3)
#define math_type(p)  (link((p)))
#define fam(p)        (font((p))) //u16

#define TEX_MATH_CHAR      1
#define TEX_SUB_BOX        2
#define TEX_SUB_MLIST      3
#define TEX_MATH_TEXT_CHAR 4

#define TEX_ORD_NOAD    (TEX_UNSET_NODE + 3)
#define TEX_OP_NOAD     (TEX_ORD_NOAD + 1)
#define TEX_BIN_NOAD    (TEX_ORD_NOAD + 2)
#define TEX_REL_NOAD    (TEX_ORD_NOAD + 3)
#define TEX_OPEN_NOAD   (TEX_ORD_NOAD + 4)
#define TEX_CLOSE_NOAD  (TEX_ORD_NOAD + 5)
#define TEX_PUNCT_NOAD  (TEX_ORD_NOAD + 6)
#define TEX_INNER_NOAD  (TEX_ORD_NOAD + 7)
#define TEX_LIMITS       1
#define TEX_NO_LIMITS    2 

#define left_delimiter(p)  ((p) + 4)
#define right_delimiter(p) ((p) + 5)

#define TEX_RADICAL_NOAD       (TEX_INNER_NOAD + 1)
#define TEX_RADICAL_NOAD_SIZE   5
#define TEX_FRACTION_NOAD      (TEX_RADICAL_NOAD + 1)
#define TEX_FRACTION_NOAD_SIZE  6

#define small_fam(p)   (mem[(p)].qqqq.u.B0) //u16
#define small_char(p)  (mem[(p)].qqqq.u.B1) //u16
#define large_fam(p)   (mem[(p)].qqqq.u.B2) //u16
#define large_char(p)  (mem[(p)].qqqq.u.B3) //u16
#define thickness(p)   (width(p))

#define TEX_DEFAULT_CODE 0x40000000

#define numerator(p)   (supscr(p))
#define denominator(p) (subscr(p))

#define TEX_UNDER_NOAD       (TEX_FRACTION_NOAD + 1)
#define TEX_OVER_NOAD        (TEX_UNDER_NOAD + 1)
#define TEX_ACCENT_NOAD      (TEX_OVER_NOAD + 1)
#define TEX_ACCENT_NOAD_SIZE  5

#define accent_chr(p)     ((p) + 4)

#define TEX_VCENTER_NOAD (TEX_ACCENT_NOAD + 1)
#define TEX_LEFT_NOAD    (TEX_VCENTER_NOAD + 1)
#define TEX_RIGHT_NOAD   (TEX_LEFT_NOAD + 1)

#define delimiter(p)     (nucleus(p))

#define TEX_MIDDLE_NOAD 1

#define scripts_allowed(p)  ((type(p) >= (quint16)TEX_ORD_NOAD) && (type(p) < (quint16)TEX_LEFT_NOAD))

#define TEX_STYLE_NODE          (TEX_UNSET_NODE + 1)
#define TEX_STYLE_NODE_SIZE      3
#define TEX_DISPLAY_STYLE        0
#define TEX_TEXT_STYLE           2
#define TEX_SCRIPT_STYLE         4
#define TEX_SCRIPT_SCRIPT_STYLE  6
#define TEX_CRAMPED              1

#define TEX_CHOICE_NODE         (TEX_UNSET_NODE + 2)

#define display_mlist(p)        (info((p) + 1))
#define text_mlist(p)           (link((p) + 1))
#define script_mlist(p)         (info((p) + 2))
#define script_script_mlist(p)  (link((p) + 2))

#define mathsy(p, s)       (font_info(famFnt(2 + s), (p) + param_base(famFnt(2 + s))).ii.CINT0)
#define math_x_height(s)   mathsy(5, s)
#define math_quad(s)       mathsy(6, s)
#define num1(s)            mathsy(8, s)
#define num2(s)            mathsy(9, s)
#define num3(s)            mathsy(10, s)
#define denom1(s)          mathsy(11, s)
#define denom2(s)          mathsy(12, s)
#define sup1(s)            mathsy(13, s)
#define sup2(s)            mathsy(14, s)
#define sup3(s)            mathsy(15, s)
#define sub1(s)            mathsy(16, s)
#define sub2(s)            mathsy(17, s)
#define sup_drop(s)        mathsy(18, s)
#define sub_drop(s)        mathsy(19, s)
#define delim1(s)          mathsy(20, s)
#define delim2(s)          mathsy(21, s)
#define axis_height(s)     mathsy(22, s)

#define TEX_TOTAL_MATHSY_PARAMS 22

#define mathex(p)               (font_info(famFnt(3 + cur_size), (p) + param_base(famFnt(3 + cur_size))).ii.CINT0)
#define default_rule_thickness  mathex(8)
#define big_op_spacing1         mathex(9)
#define big_op_spacing2         mathex(10)
#define big_op_spacing3         mathex(11)
#define big_op_spacing4         mathex(12)
#define big_op_spacing5         mathex(13)

#define TEX_TOTAL_MATHEX_PARAMS  13

#define cramped_style(p)   (2 * ((p) / 2) + TEX_CRAMPED)
#define sub_style(p)       (2 * ((p) / 4) + TEX_SCRIPT_STYLE + TEX_CRAMPED)
#define sup_style(p)       (2 * ((p) / 4) + TEX_SCRIPT_STYLE + ((p) % 2))
#define num_style(p)       ((p) + 2 - 2 * ((p) / 6))
#define denom_style(p)     (2 * ((p) / 2) + TEX_CRAMPED + 2 - 2 * ((p) / 6))

#define new_hlist(p)   (mem[nucleus((p))].ii.CINT0)

#define u_part(p)      (mem[(p) + TEX_HEIGHT_OFFSET].ii.CINT0)
#define v_part(p)      (mem[(p) + TEX_DEPTH_OFFSET].ii.CINT0)
#define extra_info(p)  (info((p) + TEX_LIST_OFFSET))

#define preamble link(align_head)

#define TEX_ALIGN_STACK_NODE_SIZE 5

#define TEX_SPAN_CODE            TEX_SPECIAL_CHAR
#define TEX_CR_CODE             (TEX_SPAN_CODE + 1)
#define TEX_CR_CR_CODE          (TEX_CR_CODE + 1)
#define TEX_END_TEMPLATE_TOKEN  (TEX_CS_TOKEN_FLAG + TEX_FROZEN_END_TEMPLATE)

#define TEX_SPAN_NODE_SIZE 2

#define TEX_TIGHT_FIT      3
#define TEX_LOOSE_FIT      1
#define TEX_VERY_LOOSE_FIT 0
#define TEX_DECENT_FIT     2

#define TEX_ACTIVE_NODE_SIZE_NORMAL 3

#define fitness(p)      subtype(p) //u16
#define break_node(p)   rlink(p)
#define line_number(p)  llink(p)
#define total_demerits(p)  (mem[(p) + 2].ii.CINT0)

#define TEX_UNHYPHENATED  0
#define TEX_HYPHENATED    1

#define last_active  active

#define TEX_PASSIVE_NODE_SIZE 10

#define cur_break(p)                    rlink(p)
#define prev_break(p)                   llink(p)
#define serial(p)                       info(p)
#define passive_pen_inter(p)            (mem[(p) + 2].ii.CINT0)
#define passive_pen_broken(p)           (mem[(p) + 3].ii.CINT0)
#define passive_left_box(p)             (mem[(p) + 4].ii.CINT0)
#define passive_left_box_width(p)       (mem[(p) + 5].ii.CINT0)
#define passive_last_left_box(p)        (mem[(p) + 6].ii.CINT0)
#define passive_last_left_box_width(p)  (mem[(p) + 7].ii.CINT0)
#define passive_right_box(p)            (mem[(p) + 8].ii.CINT0)
#define passive_right_box_width(p)      (mem[(p) + 9].ii.CINT0)

#define TEX_DELTA_NODE_SIZE 8
#define TEX_DELTA_NODE      2

#define TEX_DEACTIVATE 60

#define TEX_AWFUL_BAD  0x3FFFFFFF

#define next_break(p)  prev_break(p)

#define trie_link(p)  (trie[(p)].v.RH)
#define trie_char(p)  (trie[(p)].u.B1) //u16
#define trie_op(p)    (trie[(p)].u.B0) //u16

#define trie_root     trie_l[0]

#define trie_ref       trie_hash
#define trie_back(p)  (trie[(p)].v.LH)

#define active_height  active_width
#define cur_height     active_height[1]

#define act_width  (active_width[1])

#define TEX_UPDATE_HEIGHTS 90

#define TEX_DEPLORABLE  100000

#define TEX_INSERTS_ONLY 1
#define TEX_BOX_THERE    2

#define TEX_PAGE_INS_NODE_SIZE 4
#define TEX_INSERTING          0
#define TEX_SPLIT_UP           1

#define broken_ptr(p)    link((p)+1)
#define broken_ins(p)    info((p) + 1)
#define last_ins_ptr(p)  link((p) + 2)
#define best_ins_ptr(p)  info((p) + 2)

#define page_goal    page_so_far[0]
#define page_total   page_so_far[1]
#define page_shrink  page_so_far[6]
#define page_depth   page_so_far[7]

#define contrib_tail  (nest[0].tail_field)

#define TEX_FIL_CODE     0
#define TEX_FILL_CODE    1
#define TEX_SS_CODE      2
#define TEX_FIL_NEG_CODE 3
#define TEX_SKIP_CODE    4
#define TEX_MSKIP_CODE   5

#define TEX_BOX_FLAG    0x40000000

#define TEX_GLOBAL_BOX_FLAG     (TEX_BOX_FLAG + TEX_NUMBER_REGS)
#define TEX_MAX_GLOBAL_BOX_FLAG (TEX_GLOBAL_BOX_FLAG + TEX_NUMBER_REGS)
#define TEX_SHIP_OUT_FLAG       (TEX_MAX_GLOBAL_BOX_FLAG + 1)
#define TEX_LEADER_FLAG         (TEX_SHIP_OUT_FLAG + 1)
#define TEX_BOX_CODE             0
#define TEX_COPY_CODE            1
#define TEX_LAST_BOX_CODE        2
#define TEX_VSPLIT_CODE          3
#define TEX_VTOP_CODE            4

#define TEX_ABOVE_CODE     0
#define TEX_OVER_CODE      1
#define TEX_ATOP_CODE      2
#define TEX_DELIMITED_CODE 3

#define TEX_CHAR_DEF_CODE       0
#define TEX_MATH_CHAR_DEF_CODE  1
#define TEX_OMATH_CHAR_DEF_CODE 2
#define TEX_COUNT_DEF_CODE      3
#define TEX_DIMEN_DEF_CODE      4
#define TEX_SKIP_DEF_CODE       5
#define TEX_MU_SKIP_DEF_CODE    6
#define TEX_TOKS_DEF_CODE       7

#define TEX_SHOW_CODE      0
#define TEX_SHOW_BOX_CODE  1
#define TEX_SHOW_THE_CODE  2
#define TEX_SHOW_LISTS     3

#define TEX_WRITE_NODE_SIZE  2
#define TEX_OPEN_NODE_SIZE   3
#define TEX_OPEN_NODE        0
#define TEX_WRITE_NODE       1
#define TEX_CLOSE_NODE       2
#define TEX_SPECIAL_NODE     3
#define TEX_LANGUAGE_NODE    4
#define TEX_DIR_NODE_SIZE    4

#define dir_dir(p)      info((p) + 1)
#define dir_level(p)    link((p) + 1)
#define dir_dvi_ptr(p)  info((p) + 2)
#define dir_dvi_h(p)    info((p) + 3)
#define what_lang(p)    link((p) + 1)
#define what_lhm(p)     type((p) + 1) //u16
#define what_rhm(p)     subtype((p) + 1) //u16
#define write_tokens(p) link((p) + 1)
#define write_stream(p) info((p) + 1)
#define open_name(p)    link((p) + 1) 
#define open_area(p)    info((p) + 2) 
#define open_ext(p)     link((p) + 2)

#define TEX_END_WRITE_TOKEN (TEX_CS_TOKEN_FLAG + TEX_END_WRITE)

#define TEX_IMMEDIATE_CODE    4
#define TEX_SET_LANGUAGE_CODE 5
#define TEX_LOCAL_PAR_NODE    6
#define TEX_DIR_NODE          7

#define local_pen_inter(p)       mem[(p) + 1].ii.CINT0
#define local_pen_broken(p)      mem[(p) + 2].ii.CINT0
#define local_box_left(p)        mem[(p) + 3].ii.CINT0
#define local_box_left_width(p)  mem[(p) + 4].ii.CINT0
#define local_box_right(p)       mem[(p) + 5].ii.CINT0
#define local_box_right_width(p) mem[(p) + 6].ii.CINT0
#define local_par_dir(p)         mem[(p) + 7].ii.CINT0

#define TEX_LOCAL_PAR_SIZE 8

#define TEX_ETEX_STATE_BASE (TEX_INT_BASE + TEX_ETEX_STATE_CODE)

#define eTeX_state(p)   newEqtbInt(TEX_ETEX_STATE_BASE + (P))

#define TEX_ALEPH_VERSION_CODE         TEX_ALEPH_INT
#define TEX_OMEGA_VERSION_CODE        (TEX_ALEPH_INT + 1)
#define TEX_ALEPH_MINOR_VERSION_CODE  (TEX_ALEPH_INT + 2)
#define TEX_OMEGA_MINOR_VERSION_CODE  (TEX_ALEPH_INT + 3)
#define TEX_ETEX_MINOR_VERSION_CODE   (TEX_ALEPH_INT + 4)
#define TEX_ETEX_VERSION_CODE          TEX_ETEX_INT
#define TEX_ALEPH_REVISION_CODE       (TEX_JOB_NAME_CODE + 1)
#define TEX_OMEGA_REVISION_CODE       (TEX_JOB_NAME_CODE + 2)
#define TEX_ETEX_REVISION_CODE        (TEX_JOB_NAME_CODE + 3)

#define eTeX_ex  (eTeX_mode == 1)

#define every_eof  equiv(TEX_EVERY_EOF_LOC)

#define TEX_CURRENT_GROUP_LEVEL_CODE  (TEX_ETEX_INT + 1)
#define TEX_CURRENT_GROUP_TYPE_CODE   (TEX_ETEX_INT + 2)

#define TEX_CURRENT_IF_LEVEL_CODE   (TEX_ETEX_INT + 3)
#define TEX_CURRENT_IF_TYPE_CODE    (TEX_ETEX_INT + 4)
#define TEX_CURRENT_IF_BRANCH_CODE  (TEX_ETEX_INT + 5)

#define TEX_FONT_CHAR_WD_CODE   TEX_ETEX_DIM
#define TEX_FONT_CHAR_HT_CODE  (TEX_ETEX_DIM + 1)
#define TEX_FONT_CHAR_DP_CODE  (TEX_ETEX_DIM + 2)
#define TEX_FONT_CHAR_IC_CODE  (TEX_ETEX_DIM + 3)

#define TEX_PAR_SHAPE_LENGTH_CODE  (TEX_ETEX_DIM + 4)
#define TEX_PAR_SHAPE_INDENT_CODE  (TEX_ETEX_DIM + 5)
#define TEX_PAR_SHAPE_DIMEN_CODE   (TEX_ETEX_DIM + 6)

#define TEX_SHOW_GROUPS 4
#define TEX_SHOW_TOKENS 5
#define TEX_SHOW_IFS    6

#define TEX_IF_DEF_CODE       17
#define TEX_IF_CS_CODE        18
#define TEX_IF_FONT_CHAR_CODE 19

#define TEX_EXPR_NONE  0
#define TEX_EXPR_ADD   1
#define TEX_EXPR_SUB   2
#define TEX_EXPR_MULT  3
#define TEX_EXPR_DIV   4
#define TEX_EXPR_SCALE 5

#define TEX_EXPR_NODE_SIZE 4

#define expr_e_field(p)  mem[(p) + 1].ii.CINT0
#define expr_t_field(p)  mem[(p) + 2].ii.CINT0
#define expr_n_field(p)  mem[(p) + 3].ii.CINT0

#define TEX_GLUE_STRETCH_ORDER_CODE  (TEX_ETEX_INT + 6)
#define TEX_GLUE_SHRINK_ORDER_CODE   (TEX_ETEX_INT + 7)
#define TEX_GLUE_STRETCH_CODE        (TEX_ETEX_DIM + 7)
#define TEX_GLUE_SHRINK_CODE         (TEX_ETEX_DIM + 8)

#define TEX_MU_TO_GLUE_CODE  TEX_ETEX_GLUE
#define TEX_GLUE_TO_MU_CODE  TEX_ETEX_MU

#define TEX_BOX_VAL         7
#define TEX_MARK_VAL        8
#define TEX_INDEX_NODE_SIZE 9

#define sa_index(p)   type(p) //u16
#define sa_used(p)    subtype(p) //u16

#define hex_dig1(n)  ((n) / 4096)
#define hex_dig2(n)  (((n) / 256) % 16)
#define hex_dig3(n)  (((n) / 16) % 16)
#define hex_dig4(n)  ((n) % 16)

#define sa_lev(p)   sa_used(p) //u16

#define TEX_POINTER_NODE_SIZE 2

#define sa_type(p)  (sa_index(p) / 256) //u16
#define sa_ref(p)    info((p) + 1)
#define sa_ptr(p)    link((p) + 1)

#define TEX_WORD_NODE_SIZE 3

#define sa_num(p)  sa_ptr(p)
#define sa_int(p)  mem[(p) + 2].ii.CINT0
#define sa_dim(p)  mem[(p) + 2].ii.CINT0

#define TEX_MARK_CLASS_NODE_SIZE 4

#define add_sa_ref(p)  (sa_ref(p))++

#define TEX_VSPLIT_INIT   0
#define TEX_FIRE_UP_INIT  1
#define TEX_FIRE_UP_DONE  2
#define TEX_DESTROY_MARKS 3

#define sa_top_mark(p)          info((p) + 1)
#define sa_first_mark(p)        link((p) + 1)
#define sa_bot_mark(p)          info((p) + 2)
#define sa_split_first_mark(p)  link((p) + 2)
#define sa_split_bot_mark(p)    info((p) + 3)

#define sa_loc(q)  sa_ref(q)

#define TEX_ACTIVE_NODE_SIZE_EXTENDED 5

#define active_short(p)  (mem[(p) + 3].ii.CINT0)
#define active_glue(p)   (mem[(p) + 4].ii.CINT0)

#define hyph_root  trie_r[0]

#define tail_page_disc (disc_ptr[TEX_COPY_CODE])
#define page_disc      (disc_ptr[TEX_LAST_BOX_CODE])
#define split_disc     (disc_ptr[TEX_VSPLIT_CODE])

#define TEX_OTP_RIGHT_OUTPUT 1
#define TEX_OTP_RIGHT_NUM    2
#define TEX_OTP_RIGHT_CHAR   3
#define TEX_OTP_RIGHT_LCHAR  4
#define TEX_OTP_RIGHT_SOME   5

#define TEX_OTP_PBACK_OUTPUT 6
#define TEX_OTP_PBACK_NUM    7
#define TEX_OTP_PBACK_CHAR   8
#define TEX_OTP_PBACK_LCHAR  9
#define TEX_OTP_PBACK_SOME   10

#define TEX_OTP_ADD         11
#define TEX_OTP_SUB         12
#define TEX_OTP_MULT        13
#define TEX_OTP_DIV         14
#define TEX_OTP_MOD         15
#define TEX_OTP_LOOKUP      16
#define TEX_OTP_PUSH_NUM    17
#define TEX_OTP_PUSH_CHAR   18
#define TEX_OTP_PUSH_LCHAR  19

#define TEX_OTP_STATE_CHANGE  20
#define TEX_OTP_STATE_PUSH    21
#define TEX_OTP_STATE_POP     22

#define TEX_OTP_LEFT_START    23
#define TEX_OTP_LEFT_RETURN   24
#define TEX_OTP_LEFT_BACKUP   25

#define TEX_OTP_GOTO             26
#define TEX_OTP_GOTO_NE          27
#define TEX_OTP_GOTO_EQ          28
#define TEX_OTP_GOTO_LT          29
#define TEX_OTP_GOTO_LE          30
#define TEX_OTP_GOTO_GT          31
#define TEX_OTP_GOTO_GE          32
#define TEX_OTP_GOTO_NO_ADVANCE  33
#define TEX_OTP_GOTO_BEG         34
#define TEX_OTP_GOTO_END         35

#define TEX_OTP_STOP 36

#define otp_cur_state  otp_states[otp_state_ptr]


#define TEX_OT_FONT_FLAG   65534
#define TEX_AAT_FONT_FLAG  65535

#define is_atsu_font(p) (font_area[(p)] == TEX_AAT_FONT_FLAG)
#define is_ot_font(p)   (font_area[(p)] == TEX_OT_FONT_FLAG)
#define is_native_font(p) (is_atsu_font((p)) or is_ot_font((p)))

#define trie_op_hash (zzzaa - (int)(neg_trie_op_size))

#endif //XW_TEX

#endif //XWTEXTYPE_H
