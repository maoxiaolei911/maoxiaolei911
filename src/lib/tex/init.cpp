/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <QDateTime>
#include <QTextStream>
#include "XWApplication.h"
#include "XWTeXSea.h"
#include "XWTeXIODev.h"
#include "XWTeX.h"


int  XWTeX::checkConstant()
{
	int bad = 0;
	if ((half_error_line < 30) || (half_error_line > (error_line - 15)))
		bad = 1;
		
	if (max_print_line < 60)
		bad = 2;
		
	if ((mem_bot + 1100) > mem_top) 
		bad = 4;
		
	if (max_in_open >= 128)
		bad = 6;
			
	if (mem_top < (256 + 11)) 
		bad = 7;
		
	if (ini_version)
	{
		if ((mem_min != mem_bot) || (mem_max != mem_top))
			bad = 10;
	}
	
	if ((mem_min > mem_bot) || (mem_max < mem_top))
		bad = 10;
		
	if ((mem_min < TEX_MIN_HALFWORD) || 
		(mem_max >= TEX_MAX_HALFWORD) || 
		((mem_bot - mem_min) > (TEX_MAX_HALFWORD+1)))
	{
		bad = 14;
	}
	
	if (max_strings > TEX_MAX_HALFWORD)
		bad = 17;
	
	if (save_size > TEX_MAX_HALFWORD)
		bad = 17;
		
	if (buf_size > TEX_MAX_HALFWORD)
		bad = 18;
		
	if ((2 * TEX_MAX_HALFWORD) < (mem_top - mem_min))
		bad = 41;
		
	if (bad != 0)
	{
		QString msg = QString(tr("Ouch---my internal constants have been clobbered!\n"
		                         "---case %1\n")).arg(bad);
		dev->termOut(msg);
	}
	
	return bad;
}

void  XWTeX::enableETeX()
{
	if (ini_version)
	{
		no_new_control_sequence = false;
		primitive(TeXLastNodeType, (quint16)TEX_LAST_ITEM, TEX_LAST_NODE_TYPE_CODE);
		primitive(TeXAlephversion, (quint16)TEX_LAST_ITEM, TEX_ALEPH_VERSION_CODE);
		primitive(TeXOmegaversion, (quint16)TEX_LAST_ITEM, TEX_OMEGA_VERSION_CODE);
		primitive(TeXeTeXversion,  (quint16)TEX_LAST_ITEM, TEX_ETEX_VERSION_CODE);
		primitive(TeXAlephminorversion, (quint16)TEX_LAST_ITEM, TEX_ALEPH_MINOR_VERSION_CODE);
		primitive(TeXOmegaminorversion, (quint16)TEX_LAST_ITEM, TEX_OMEGA_MINOR_VERSION_CODE);
		primitive(TeXeTeXminorversion,  (quint16)TEX_LAST_ITEM, TEX_ETEX_MINOR_VERSION_CODE);
		primitive(TeXAlephrevision, (quint16)TEX_CONVERT, TEX_ALEPH_REVISION_CODE);
		primitive(TeXOmegarevision, (quint16)TEX_CONVERT, TEX_OMEGA_REVISION_CODE);
		primitive(TeXeTeXrevision, (quint16)TEX_CONVERT, TEX_ETEX_REVISION_CODE);
			
		primitive(TeXEveryEof, (quint16)TEX_ASSIGN_TOKS, TEX_EVERY_EOF_LOC);
		primitive(TeXTracingAssigns, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TRACING_ASSIGNS_CODE);
		primitive(TeXTracingGroups, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TRACING_GROUPS_CODE);
		primitive(TeXTracingIfs, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TRACING_IFS_CODE);
		primitive(TeXTracingScanTokens, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TRACING_SCAN_TOKENS_CODE);
		primitive(TeXTracingNesting, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TRACING_NESTING_CODE);
		primitive(TeXPredisplayDirection, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_PRE_DISPLAY_DIRECTION_CODE);
		primitive(TeXLastLineFit, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE+ TEX_LAST_LINE_FIT_CODE);
		primitive(TeXSavingVDiscards, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_SAVING_VDISCARDS_CODE);
		primitive(TeXSavingHyphCodes, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_SAVING_HYPH_CODES_CODE);
		
		primitive(TeXCurrentGroupLevel, (quint16)TEX_LAST_ITEM, TEX_CURRENT_GROUP_LEVEL_CODE);
		primitive(TeXCurrentGroupType, (quint16)TEX_LAST_ITEM, TEX_CURRENT_GROUP_TYPE_CODE);
			
		primitive(TeXCurrentIfLevel, (quint16)TEX_LAST_ITEM, TEX_CURRENT_IF_LEVEL_CODE);
		primitive(TeXCurrentIfType, (quint16)TEX_LAST_ITEM, TEX_CURRENT_IF_TYPE_CODE);
		primitive(TeXCurrentIfBranch, (quint16)TEX_LAST_ITEM, TEX_CURRENT_IF_BRANCH_CODE);
		
		primitive(TeXFontCharWD, (quint16)TEX_LAST_ITEM, TEX_FONT_CHAR_WD_CODE);
		primitive(TeXFontCharHT, (quint16)TEX_LAST_ITEM, TEX_FONT_CHAR_HT_CODE);
		primitive(TeXFontCharDP, (quint16)TEX_LAST_ITEM, TEX_FONT_CHAR_DP_CODE);
		primitive(TeXFontCharIC, (quint16)TEX_LAST_ITEM, TEX_FONT_CHAR_IC_CODE);
			
		primitive(TeXParShapeLength, (quint16)TEX_LAST_ITEM, TEX_PAR_SHAPE_LENGTH_CODE);
		primitive(TeXParShapeIndent, (quint16)TEX_LAST_ITEM, TEX_PAR_SHAPE_INDENT_CODE);
		primitive(TeXParShapeDimen, (quint16)TEX_LAST_ITEM, TEX_PAR_SHAPE_DIMEN_CODE);
			
		primitive(TeXShowGroups, (quint16)TEX_XRAY, TEX_SHOW_GROUPS);
			
		primitive(TeXShowTokens, (quint16)TEX_XRAY, TEX_SHOW_TOKENS);
			
		primitive(TeXUnExpanded, (quint16)TEX_THE, 1);
		primitive(TeXDetokenize, (quint16)TEX_THE, TEX_SHOW_TOKENS);
			
		primitive(TeXShowIfs, (quint16)TEX_XRAY, TEX_SHOW_IFS);
			
		primitive(TeXInteractionMode, (quint16)TEX_SET_PAGE_INT, 2);
			
		primitive(TeXMiddle, (quint16)TEX_LEFT_RIGHT, TEX_MIDDLE_NOAD);
			
		primitive(TeXScanTokens, (quint16)TEX_INPUT, 2);
			
		primitive(TeXReadLine, (quint16)TEX_READ_TO_CS, 1);
			
		primitive(TeXUnless, (quint16)TEX_EXPAND_AFTER, 1);
		primitive(TeXIfDefined, (quint16)TEX_IF_TEST, TEX_IF_DEF_CODE);
		primitive(TeXIfCsName, (quint16)TEX_IF_TEST, TEX_IF_CS_CODE);
		primitive(TeXIfFontChar, (quint16)TEX_IF_TEST, TEX_IF_FONT_CHAR_CODE);
			
		primitive(TeXProtected, (quint16)TEX_PREFIX, 8);
			
		primitive(TeXNumExpr, (quint16)TEX_LAST_ITEM, TEX_ETEX_EXPR - TEX_INT_VAL + TEX_INT_VAL);
		primitive(TeXDimExpr, (quint16)TEX_LAST_ITEM, TEX_ETEX_EXPR - TEX_INT_VAL + TEX_DIMEN_VAL);
		primitive(TeXGlueExpr, (quint16)TEX_LAST_ITEM, TEX_ETEX_EXPR - TEX_INT_VAL + TEX_GLUE_VAL);
		primitive(TeXMuExpr, (quint16)TEX_LAST_ITEM, TEX_ETEX_EXPR - TEX_INT_VAL + TEX_MU_VAL);
			
		primitive(TeXGlueStretchOrder, (quint16)TEX_LAST_ITEM, TEX_GLUE_STRETCH_ORDER_CODE);
		primitive(TeXGlueShrinkOrder, (quint16)TEX_LAST_ITEM, TEX_GLUE_SHRINK_ORDER_CODE);
		primitive(TeXGlueStretch, (quint16)TEX_LAST_ITEM, TEX_GLUE_STRETCH_CODE);
		primitive(TeXGlueShrink, (quint16)TEX_LAST_ITEM, TEX_GLUE_SHRINK_CODE);
			
		primitive(TeXMuToGlue, (quint16)TEX_LAST_ITEM, TEX_MU_TO_GLUE_CODE);
		primitive(TeXGlueToMu, (quint16)TEX_LAST_ITEM, TEX_GLUE_TO_MU_CODE);
			
		primitive(TeXMarks, (quint16)TEX_MARK, TEX_MARKS_CODE);
		primitive(TeXTopMarks, (quint16)TEX_TOP_BOT_MARK, TEX_TOP_MARK_CODE + TEX_MARKS_CODE);
		primitive(TeXFirstMarks, (quint16)TEX_TOP_BOT_MARK, TEX_FIRST_MARK_CODE + TEX_MARKS_CODE);
		primitive(TeXBotMarks, (quint16)TEX_TOP_BOT_MARK, TEX_BOT_MARK_CODE + TEX_MARKS_CODE);
		primitive(TeXSplitFirstMarks, (quint16)TEX_TOP_BOT_MARK, TEX_SPLIT_FIRST_MARK_CODE + TEX_MARKS_CODE);
		primitive(TeXSplitBotMarks, (quint16)TEX_TOP_BOT_MARK, TEX_SPLIT_BOT_MARK_CODE + TEX_MARKS_CODE);
			
		primitive(TeXPageDiscards, (quint16)TEX_UN_VBOX, TEX_LAST_BOX_CODE);
		primitive(TeXSplitDiscards, (quint16)TEX_UN_VBOX, TEX_VSPLIT_CODE);
			
		primitive(TeXInterLinePenalties, (quint16)TEX_SET_SHAPE, TEX_INTER_LINE_PENALTIES_LOC);
		primitive(TeXClubPenalties, (quint16)TEX_SET_SHAPE, TEX_CLUB_PENALTIES_LOC);
		primitive(TeXWidowPenalties, (quint16)TEX_SET_SHAPE, TEX_WIDOW_PENALTIES_LOC);
		primitive(TeXDisplayWidowPenalties, (quint16)TEX_SET_SHAPE, TEX_DISPLAY_WIDOW_PENALTIES_LOC);
			
//		loc++;
		eTeX_mode = 1;
			
		max_reg_num = 65535;
		max_reg_help_line = tr("A register number must be between 0 and 65535.");
	}
}

void XWTeX::fixDateAndTime()
{
	qint32 minutesA;
	qint32 dayA;
    qint32 monthA;
    qint32 yearA;
    getDateAndTime(minutesA, dayA, monthA, yearA);
    setNewEqtbInt(TEX_INT_BASE + TEX_TIME_CODE, minutesA);
    setNewEqtbInt(TEX_INT_BASE + TEX_DAY_CODE, dayA);
    setNewEqtbInt(TEX_INT_BASE + TEX_MONTH_CODE, monthA);
    setNewEqtbInt(TEX_INT_BASE + TEX_YEAR_CODE, yearA);
}

void XWTeX::getDateAndTime(qint32 & minutesA, 
	                       qint32 & dayA,
                      	   qint32 & monthA,  
                      	   qint32 & yearA)
{
	QDateTime ldt = QDateTime::currentDateTime().toLocalTime();
	minutesA = (qint32)(ldt.time().hour() * 60 + ldt.time().minute());	
	dayA = (qint32)(ldt.date().day());
	monthA = (qint32)(ldt.date().month());
	yearA = (qint32)(ldt.date().year());
}

void XWTeX::initialize()
{
	qint32 k, z;
	
	if (interaction_option == TEX_UNSPECIFIED_MODE)
		interaction = TEX_ERROR_STOP_MODE;
	else
		interaction = interaction_option;
			
	deletions_allowed = true; 
	set_box_allowed = true;
	error_count = 0;
	
	help_ptr = 0; 
	use_err_help = false;
	
	interrupt = 0; 
	OK_to_interrupt = true;
	
#ifdef XW_TEX_DEBUG
	was_mem_end = mem_min;
	was_lo_max  = mem_min; 
	was_hi_min  = mem_max;
	panicking   = false;
#endif //XW_TEX_DEBUG

	nest_ptr = 0;
	max_nest_stack = 0;
    mode = TEX_VMODE; 
    head = contrib_head; 
    tail = contrib_head;
    eTeX_aux = TEX_NULL;
	prev_depth = TEX_IGNORE_DEPTH; 
	mode_line = 0;
	prev_graf = 0; 
	shown_mode = 0;
	dir_save = TEX_NULL; 
	dir_math_save = false;
	local_par = TEX_NULL; 
	local_par_bool = false;	
	page_contents = TEX_EMPTY; 
	page_tail = page_head; 
//	link(page_head) = TEX_NULL;
	last_glue = TEX_MAX_HALFWORD; 
	last_penalty = 0; 
	last_kern = 0;
//    last_node_type = -1;
    page_depth = 0; 
    page_max_depth = 0;
    
    no_new_control_sequence = true;
    
    save_ptr = 0; 
    cur_level = TEX_LEVEL_ONE; 
    cur_group = TEX_BOTTOM_LEVEL; 
    cur_boundary = 0;
	max_save_stack = 0;
	
	mag_set = 0;
	
	top_mark = TEX_NULL; 
	first_mark = TEX_NULL; 
	bot_mark = TEX_NULL;
	split_first_mark = TEX_NULL; 
	split_bot_mark = TEX_NULL;
	
	cur_val = 0; 
	cur_val_level = TEX_INT_VAL; 
	radix = 0; 
	cur_order = 0;
	
	for (k = 0; k < 16; k++)
	{
		if (read_file[k])
		{
			if (read_file[k]->isOpen())
				read_file[k]->close();
			delete read_file[k];
			read_file[k] = 0;
			read_file_mode[k] = 0;
			read_file_translation[k] = 0;
		}
	}
	
	for (k = 0; k < 17; k++)
		read_open[k] = TEX_CLOSED;
		
	cond_ptr = TEX_NULL; 
	if_limit = TEX_NORMAL; 
	cur_if   = 0; 
	if_line  = 0;
	
	null_character.u.B0 = TEX_MIN_QUARTERWORD; 
	null_character.u.B1 = TEX_MIN_QUARTERWORD;
	null_character.u.B2 = TEX_MIN_QUARTERWORD; 
	null_character.u.B3 = TEX_MIN_QUARTERWORD;

    total_pages = 0; 
    max_v = 0; 
    max_h = 0; 
    max_push = 0; 
    last_bop = -1;
	doing_leaders = false; 
	dead_cycles = 0; 
	cur_s = -1;
	
	fnt = 0;
    ch = 0;
	
	down_ptr  = TEX_NULL; 
	right_ptr = TEX_NULL;
	
	adjust_tail  = TEX_NULL; 
	last_badness = 0;
	
	pack_begin_line = 0;
	
	empty_field.v.RH = TEX_EMPTY; 
	empty_field.v.LH = TEX_NULL;
	null_delimiter.u.B0 = 0; 
	null_delimiter.u.B1 = TEX_MIN_QUARTERWORD;
	null_delimiter.u.B2 = 0; 
	null_delimiter.u.B3 = TEX_MIN_QUARTERWORD;
	
	align_ptr = TEX_NULL; 
	cur_align = TEX_NULL; 
	cur_span  = TEX_NULL; 
	cur_loop  = TEX_NULL;
	cur_head  = TEX_NULL; 
	cur_tail  = TEX_NULL;
	
	temp_no_whatsits = 0; 
	temp_no_dirs = 0; 
	temporary_dir = 0;
	dir_ptr = TEX_NULL; 
	dir_tmp = TEX_NULL; 
	dir_rover = TEX_NULL;
	temp_no_whatsits = 0;
	
	max_hyph_char = TEX_TOO_BIG_LANG;
	
	for (z = 0; z <= hyph_size; z++)
	{
		hyph_word[z] = 0; 
		hyph_list[z] = TEX_NULL;
		hyph_link[z] = 0;
	}
	
	hyph_count = 0;
	hyph_next = TEX_HYPH_PRIME + 1;
	if (hyph_next > hyph_size)
		hyph_next = TEX_HYPH_PRIME;
	
	output_active = false; 
	insert_penalties = 0;
	
	ligature_present = false; 
	cancel_boundary = false; 
	lft_hit = false; 
	rt_hit = false;
	left_ghost = false; 
	right_ghost = false;
	new_left_ghost = false; 
	new_right_ghost = false;
	ins_disc = false;
	
	after_token = 0;
	
	long_help_seen = false;
	
	format_ident = 0;
	
	for (k = 0; k < 18; k++)
	{
		if (write_file[k])
		{
			if (write_file[k]->isOpen())
				write_file[k]->close();
			delete write_file[k];
			write_file[k] = 0;
			write_file_mode[k] = 0;
			write_file_translation[k] = 0;
		}
	}
	
	for (k = 0; k < 19; k++)
		write_open[k] = false;	
		
	for (qint32 i = 0; i <= max_in_open; i++)
	{
		if (input_file[i])
		{
			if (input_file[i]->isOpen())
				input_file[i]->close();
			delete input_file[i];
			input_file[i] = 0;
		}
	}
	
	setNewEqtbInt(TEX_DIR_BASE + TEX_PAGE_DIRECTION_CODE, 0);
	setNewEqtbInt(TEX_DIR_BASE + TEX_BODY_DIRECTION_CODE, 0);
	setNewEqtbInt(TEX_DIR_BASE + TEX_PAR_DIRECTION_CODE, 0);
	setNewEqtbInt(TEX_DIR_BASE + TEX_TEXT_DIRECTION_CODE, 0);
	setNewEqtbInt(TEX_DIR_BASE + TEX_MATH_DIRECTION_CODE, 0);
	pack_direction = -1;
	setNewEqtbSC(TEX_DIMEN_BASE + TEX_PAGE_HEIGHT_CODE, 55380984);
	setNewEqtbSC(TEX_DIMEN_BASE + TEX_PAGE_WIDTH_CODE, 39158276);
	setNewEqtbSC(TEX_DIMEN_BASE + TEX_PAGE_BOTTOM_OFFSET_CODE, 4736287);
	setNewEqtbSC(TEX_DIMEN_BASE + TEX_PAGE_RIGHT_OFFSET_CODE, 4736287);
	for (k = 0; k <= 7; k++)
	{
		dir_primary  [k   ]   = TEX_DIR_T;
  		dir_primary  [k + 8]  = TEX_DIR_L;
  		dir_primary  [k + 16] = TEX_DIR_B;
  		dir_primary  [k + 24] = TEX_DIR_R;
	}    
    for (k = 0; k <= 3; k++)
    {
    	dir_secondary[k   ]   = TEX_DIR_L;
  		dir_secondary[k + 4]  = TEX_DIR_R;
  		dir_secondary[k + 8]  = TEX_DIR_T;
  		dir_secondary[k + 12] = TEX_DIR_B;

  		dir_secondary[k + 16] = TEX_DIR_L;
  		dir_secondary[k + 20] = TEX_DIR_R;
  		dir_secondary[k + 24] = TEX_DIR_T;
  		dir_secondary[k + 28] = TEX_DIR_B;
    }
    for (k = 0; k <= 7; k++)
    {
    	dir_tertiary[k * 4  ]   = TEX_DIR_T;
  		dir_tertiary[k * 4 + 1] = TEX_DIR_L;
  		dir_tertiary[k * 4 + 2] = TEX_DIR_B;
  		dir_tertiary[k * 4 + 3] = TEX_DIR_R;
    }
    dir_rearrange[0] = 0;
	dir_rearrange[1] = 0;
	dir_rearrange[2] = 1;
	dir_rearrange[3] = 1;
	dir_names[0] = 'T';
	dir_names[1] = 'L';
	dir_names[2] = 'B';
	dir_names[3] = 'R';
	
	pseudo_files = TEX_NULL;
	
	sa_mark = TEX_NULL; 
	sa_null.hh.v.LH = TEX_NULL; 
	sa_null.hh.v.RH = TEX_NULL;
	
	sa_chain = TEX_NULL; 
	sa_level = TEX_LEVEL_ZERO;
	
	page_disc  = TEX_NULL; 
	split_disc = TEX_NULL;
	
	edit_name_start = 0;
	stop_at_space = true;
		
    if (ini_version)
	{
		k = mem_bot + 1;
		for (; k <= lo_mem_stat_max; k++)
			mem[k].ii.CINT0 = 0;
			
		k = mem_bot;
		while (k <= lo_mem_stat_max)
		{
			glue_ref_count(k) = TEX_NULL + 1;
  			stretch_order(k) = (quint16)TEX_NORMAL; 
  			shrink_order(k) = (quint16)TEX_NORMAL;
  			k = k + TEX_GLUE_SPEC_SIZE;
		}
		
		stretch(sfi_glue) = TEX_UNITY; 
		stretch_order(sfi_glue) = (quint16)TEX_SFI;
		stretch(fil_glue) = TEX_UNITY; 
		stretch_order(fil_glue) = (quint16)TEX_FIL;
		stretch(fill_glue) = TEX_UNITY; 
		stretch_order(fill_glue) = (quint16)TEX_FILL;
		stretch(ss_glue) = TEX_UNITY; 
		stretch_order(ss_glue) = (quint16)TEX_FIL;
		shrink(ss_glue) = TEX_UNITY; 
		shrink_order(ss_glue) = (quint16)TEX_FIL;
		stretch(fil_neg_glue) = -TEX_UNITY; 
		stretch_order(fil_neg_glue) = (quint16)TEX_FIL;
		rover = lo_mem_stat_max + 1;
		link(rover) = TEX_EMPTY_FLAG;
		node_size(rover) = 1000;
		llink(rover) = rover; 
		rlink(rover) = rover;
		lo_mem_max = rover + 1000; 
		link(lo_mem_max) = TEX_NULL; 
		info(lo_mem_max) = TEX_NULL;
		k = hi_mem_stat_min;
		for(; k < mem_top; k++)
  			mem[k] = mem[lo_mem_max];
  			
  		info(omit_template) = TEX_END_TEMPLATE_TOKEN;
  		link(end_span) = TEX_MAX_QUARTERWORD + 1; 
  		info(end_span) = TEX_NULL;
  		type(last_active) = (quint16)TEX_HYPHENATED; 
  		line_number(last_active) = TEX_MAX_HALFWORD;
		subtype(last_active) = (quint16)0;		
		subtype(page_ins_head) = (quint16)255;
		type(page_ins_head) = (quint16)TEX_SPLIT_UP; 
		link(page_ins_head) = page_ins_head;
		type(page_head) = (quint16)TEX_GLUE_NODE; 
		subtype(page_head) = (quint16)TEX_NORMAL;
		
		avail = TEX_NULL; 
		mem_end = mem_top;
		hi_mem_min = hi_mem_stat_min;
		var_used = lo_mem_stat_max + 1 - mem_bot; 
		dyn_used = hi_mem_stat_usage;
		
		setEqType(TEX_UNDEFINED_CONTROL_SEQUENCE, TEX_UNDEFINED_CS);
		setEquiv(TEX_UNDEFINED_CONTROL_SEQUENCE, TEX_NULL);
		setEqLevel(TEX_UNDEFINED_CONTROL_SEQUENCE, TEX_LEVEL_ZERO);

		setEquiv(TEX_GLUE_BASE, zero_glue); 
		setEqLevel(TEX_GLUE_BASE, TEX_LEVEL_ONE);
		setEqType(TEX_GLUE_BASE, TEX_GLUE_REF);
		glue_ref_count(zero_glue) = glue_ref_count(zero_glue) + TEX_LOCAL_BASE - TEX_GLUE_BASE;
		
		setEquiv(TEX_PAR_SHAPE_LOC, TEX_NULL); 
		setEqType(TEX_PAR_SHAPE_LOC, TEX_SHAPE_REF);
		setEqLevel(TEX_PAR_SHAPE_LOC, TEX_LEVEL_ONE);
		setEquiv(TEX_CAT_CODE_BASE + TEX_CARRIAGE_RETURN, TEX_CAR_RET);
		setEquiv(TEX_CAT_CODE_BASE + ' ', TEX_SPACER);
		setEquiv(TEX_CAT_CODE_BASE + '\\', TEX_ESCAPE);
		setEquiv(TEX_CAT_CODE_BASE + '%', TEX_COMMENT);
		setEquiv(TEX_CAT_CODE_BASE + TEX_INVALID_CODE, TEX_INVALID_CHAR);
		setEquiv(TEX_CAT_CODE_BASE + TEX_NULL_CODE, TEX_IGNORE);
		for (k = '0'; k <= '9'; k++)
			setEquiv(TEX_MATH_CODE_BASE + k, k + TEX_VAR_CODE);
			
		for (k = 'A'; k <= 'Z'; k++)
		{
  			setEquiv(TEX_CAT_CODE_BASE + k, TEX_LETTER);
  			setEquiv(TEX_CAT_CODE_BASE + k + 'a' - 'A', TEX_LETTER);
  			setEquiv(TEX_MATH_CODE_BASE + k, k + TEX_VAR_CODE + 0x10000);
  			setEquiv(TEX_MATH_CODE_BASE + k + 'a'- 'A', k+ 'a' - 'A' + TEX_VAR_CODE + 0x10000);
  			setEquiv(TEX_LC_CODE_BASE + k, k + 'a' - 'A');
  			setEquiv(TEX_LC_CODE_BASE + k + 'a' - 'A', k +'a' - 'A');
  			setEquiv(TEX_UC_CODE_BASE + k, k);
  			setEquiv(TEX_UC_CODE_BASE + k + 'a' - 'A', k);
  			setEquiv(TEX_SF_CODE_BASE + k, 999);
  		}
  		
  		setNewEqtbInt(TEX_INT_BASE + TEX_MAG_CODE, 1000);
		setNewEqtbInt(TEX_INT_BASE + TEX_TOLERANCE_CODE, 10000);
		setNewEqtbInt(TEX_INT_BASE + TEX_HANG_AFTER_CODE, 1);
		setNewEqtbInt(TEX_INT_BASE + TEX_MAX_DEAD_CYCLES_CODE, 25);
		setNewEqtbInt(TEX_INT_BASE + TEX_ESCAPE_CHAR_CODE, '\\');
		setNewEqtbInt(TEX_INT_BASE + TEX_END_LINE_CHAR_CODE, TEX_CARRIAGE_RETURN);
		setNewEqtbInt(TEX_INT_BASE + TEX_LEVEL_LOCAL_DIR_CODE, TEX_LEVEL_ONE);
		setEquiv(TEX_DEL_CODE_BASE + '.', 0);
		setEquiv1(TEX_DEL_CODE_BASE + '.', 0);
		
		hash_used = TEX_FROZEN_CONTROL_SEQUENCE;
		cs_count = 0;
		setEqType(TEX_FROZEN_DONT_EXPAND, TEX_DONT_EXPAND);
		setText(TEX_FROZEN_DONT_EXPAND, TeXNotExpanded);
		
		font_ptr = TEX_NULL_FONT;
		allocateFontTable(TEX_NULL_FONT, TEX_OFFSET_CHARINFO_BASE + 108);
		font_file_size(TEX_NULL_FONT) = TEX_OFFSET_CHARINFO_BASE + 108;
		font_used(TEX_NULL_FONT)      = false;
		font_name(TEX_NULL_FONT)      = TeXNullFont; 
		font_area(TEX_NULL_FONT)      = TEX_TOO_BIG_CHAR;
		hyphen_char(TEX_NULL_FONT)    = '-'; 
		skew_char(TEX_NULL_FONT)      = -1;
		bchar_label(TEX_NULL_FONT)    = TEX_NON_ADDRESS;
		font_bchar(TEX_NULL_FONT)         = TEX_NON_CHAR; 
		font_false_bchar(TEX_NULL_FONT)   = TEX_NON_CHAR;
		font_bc(TEX_NULL_FONT)            = 1; 
		font_ec(TEX_NULL_FONT)            = 0;
		font_size(TEX_NULL_FONT)          = 0; 
		font_dsize(TEX_NULL_FONT)         = 0;
		char_base(TEX_NULL_FONT)          = TEX_OFFSET_CHARINFO_BASE;
		char_attr_base(TEX_NULL_FONT)     = TEX_OFFSET_CHARINFO_BASE;
		ivalues_start(TEX_NULL_FONT)      = TEX_OFFSET_CHARINFO_BASE;
		fvalues_start(TEX_NULL_FONT)      = TEX_OFFSET_CHARINFO_BASE;
		mvalues_start(TEX_NULL_FONT)      = TEX_OFFSET_CHARINFO_BASE;
		rules_start(TEX_NULL_FONT)        = TEX_OFFSET_CHARINFO_BASE;
		glues_start(TEX_NULL_FONT)        = TEX_OFFSET_CHARINFO_BASE;
    	penalties_start(TEX_NULL_FONT)    = TEX_OFFSET_CHARINFO_BASE;
		ivalues_base(TEX_NULL_FONT)       = TEX_OFFSET_CHARINFO_BASE;
		fvalues_base(TEX_NULL_FONT)       = TEX_OFFSET_CHARINFO_BASE;
		mvalues_base(TEX_NULL_FONT)       = TEX_OFFSET_CHARINFO_BASE;
		rules_base(TEX_NULL_FONT)         = TEX_OFFSET_CHARINFO_BASE;
		glues_base(TEX_NULL_FONT)         = TEX_OFFSET_CHARINFO_BASE;
		penalties_base(TEX_NULL_FONT)     = TEX_OFFSET_CHARINFO_BASE;
		width_base(TEX_NULL_FONT)         = TEX_OFFSET_CHARINFO_BASE;
		height_base(TEX_NULL_FONT)        = TEX_OFFSET_CHARINFO_BASE;
		depth_base(TEX_NULL_FONT)         = TEX_OFFSET_CHARINFO_BASE;
		italic_base(TEX_NULL_FONT)        = TEX_OFFSET_CHARINFO_BASE;
		lig_kern_base(TEX_NULL_FONT)      = TEX_OFFSET_CHARINFO_BASE;
		kern_base(TEX_NULL_FONT)          = TEX_OFFSET_CHARINFO_BASE;
		exten_base(TEX_NULL_FONT)         = TEX_OFFSET_CHARINFO_BASE;
		font_glue(TEX_NULL_FONT)          = TEX_NULL;
		font_params(TEX_NULL_FONT)        = 7;
		param_base(TEX_NULL_FONT)         = TEX_OFFSET_CHARINFO_BASE - 1;
		font_offset(TEX_NULL_FONT)        = 0;
		for (char k = 1; k <= 7; k++)
			font_info(TEX_NULL_FONT, param_base(TEX_NULL_FONT) + k).ii.CINT0 = 0;
			
		active_min_ptr = 0;
		active_real = 0;
			
		ocp_ptr = TEX_NULL_OCP;
		allocateOcpTable(TEX_NULL_OCP, 17);
		ocp_file_size(TEX_NULL_OCP) = 17;
		ocp_name(TEX_NULL_OCP) = TeXNullOcp; 
		ocp_area(TEX_NULL_OCP) = TEX_TOO_BIG_CHAR;
		ocp_external(TEX_NULL_OCP) = 0; 
		ocp_external_arg(TEX_NULL_OCP) = 0;
		ocp_input(TEX_NULL_OCP) = 1; 
		ocp_output(TEX_NULL_OCP) = 1;
		ocp_no_tables(TEX_NULL_OCP) = 0;
		ocp_no_states(TEX_NULL_OCP) = 1;
		ocp_table_base(TEX_NULL_OCP) = TEX_OFFSET_OCP_INFO;
		ocp_state_base(TEX_NULL_OCP) = TEX_OFFSET_OCP_INFO;
		ocp_info(TEX_NULL_OCP, TEX_OFFSET_OCP_INFO)     = TEX_OFFSET_OCP_INFO + 2;
		ocp_info(TEX_NULL_OCP, TEX_OFFSET_OCP_INFO + 1) = TEX_OFFSET_OCP_INFO + 5;
		ocp_info(TEX_NULL_OCP, TEX_OFFSET_OCP_INFO + 2) = 23;
		ocp_info(TEX_NULL_OCP, TEX_OFFSET_OCP_INFO + 3) = 3;
		ocp_info(TEX_NULL_OCP, TEX_OFFSET_OCP_INFO + 4) = 36;
		
		ocp_listmem_ptr = 2;
		ocp_list_lstack(0) = (quint16)0;
		ocp_list_lstack_no(0) = TEX_OCP_MAXINT;
		ocp_list_lnext(0) = (quint16)0;
		ocp_list_ptr = TEX_NULL_OCP_LIST;
		ocp_list_list[TEX_NULL_OCP_LIST] = 0;
		ocp_lstackmem_ptr = 1;
		
		for (qint32 k = -trie_op_size; k <= trie_op_size; k++)
			trie_op_hash[k] = 0;
			
		for (qint32 k = 0; k <= TEX_BIGGEST_LANG; k++)
			trie_used[k] = TEX_MIN_QUARTERWORD;
			
		trie_op_ptr = 0 ;
    trie_not_ready = true ;
			
  		setText(TEX_FROZEN_PROTECTION, TeXInAccessible);
  		
  		setText(TEX_END_WRITE, TeXEndWrite); 
  		setEqLevel(TEX_END_WRITE, TEX_LEVEL_ONE);
		setEqType(TEX_END_WRITE, TEX_OUTER_CALL); 
		setEquiv(TEX_END_WRITE, TEX_NULL);
		
		eTeX_mode = 0;
		max_reg_num = 255;
		max_reg_help_line = tr("A register number must be between 0 and 255.");
	}
	
	if (!ini_version)
		ready_already = 314159;
}

bool XWTeX::initPrim()
{
	ready_already = 0;
	no_new_control_sequence = false;
	
	if (!loadPoolFile("tex.pool"))
		return false;
			
	primitive(TeXLineSkip, (quint16)TEX_ASSIGN_GLUE, TEX_GLUE_BASE + TEX_LINE_SKIP_CODE);
	primitive(TeXBaseLineSkip, (quint16)TEX_ASSIGN_GLUE, TEX_GLUE_BASE + TEX_BASELINE_SKIP_CODE);
	primitive(TeXParSkip, (quint16)TEX_ASSIGN_GLUE,TEX_GLUE_BASE + TEX_PAR_SKIP_CODE);
	primitive(TeXAboveDisplaySkip, (quint16)TEX_ASSIGN_GLUE,TEX_GLUE_BASE + TEX_ABOVE_DISPLAY_SKIP_CODE);
	primitive(TeXBelowDisplaySkip, (quint16)TEX_ASSIGN_GLUE,TEX_GLUE_BASE + TEX_BELOW_DISPLAY_SKIP_CODE);
	primitive(TeXAboveDisplayShortSkip,
  				(quint16)TEX_ASSIGN_GLUE, TEX_GLUE_BASE + TEX_ABOVE_DISPLAY_SHORT_SKIP_CODE);
	primitive(TeXBelowDisplayShortSkip,
  				(quint16)TEX_ASSIGN_GLUE, TEX_GLUE_BASE + TEX_BELOW_DISPLAY_SHORT_SKIP_CODE);
	primitive(TeXLeftSkip, (quint16)TEX_ASSIGN_GLUE, TEX_GLUE_BASE + TEX_LEFT_SKIP_CODE);
	primitive(TeXRightSkip, (quint16)TEX_ASSIGN_GLUE, TEX_GLUE_BASE + TEX_RIGHT_SKIP_CODE);
	primitive(TeXTopSkip, (quint16)TEX_ASSIGN_GLUE, TEX_GLUE_BASE + TEX_TOP_SKIP_CODE);
	primitive(TeXSplitTopSkip, (quint16)TEX_ASSIGN_GLUE, TEX_GLUE_BASE + TEX_SPLIT_TOP_SKIP_CODE);
	primitive(TeXTabSkip, (quint16)TEX_ASSIGN_GLUE, TEX_GLUE_BASE + TEX_TAB_SKIP_CODE);
	primitive(TeXSpaceSkip, (quint16)TEX_ASSIGN_GLUE, TEX_GLUE_BASE + TEX_SPACE_SKIP_CODE);
	primitive(TeXXSpaceSkip, (quint16)TEX_ASSIGN_GLUE, TEX_GLUE_BASE + TEX_XSPACE_SKIP_CODE);
	primitive(TeXParFillSkip, (quint16)TEX_ASSIGN_GLUE, TEX_GLUE_BASE + TEX_PAR_FILL_SKIP_CODE);
	primitive(TeXThinMuSkip,(quint16)TEX_ASSIGN_MU_GLUE, TEX_GLUE_BASE + TEX_THIN_MU_SKIP_CODE);
	primitive(TeXMedMuSkip,(quint16)TEX_ASSIGN_MU_GLUE, TEX_GLUE_BASE + TEX_MED_MU_SKIP_CODE);
	primitive(TeXThickMuSkip, (quint16)TEX_ASSIGN_MU_GLUE, TEX_GLUE_BASE + TEX_THICK_MU_SKIP_CODE);
	
	primitive(TeXOutput, (quint16)TEX_ASSIGN_TOKS, TEX_OUTPUT_ROUTINE_LOC);
	primitive(TeXEveryPar, (quint16)TEX_ASSIGN_TOKS, TEX_EVERY_PAR_LOC);
	primitive(TeXEveryMath, (quint16)TEX_ASSIGN_TOKS, TEX_EVERY_MATH_LOC);
	primitive(TeXEveryDisplay, (quint16)TEX_ASSIGN_TOKS, TEX_EVERY_DISPLAY_LOC);
	primitive(TeXEveryHBox, (quint16)TEX_ASSIGN_TOKS, TEX_EVERY_HBOX_LOC);
	primitive(TeXEveryVBox, (quint16)TEX_ASSIGN_TOKS, TEX_EVERY_VBOX_LOC);
	primitive(TeXEveryJob, (quint16)TEX_ASSIGN_TOKS, TEX_EVERY_JOB_LOC);
	primitive(TeXEveryCr, (quint16)TEX_ASSIGN_TOKS, TEX_EVERY_CR_LOC);
	primitive(TeXErrHelp, (quint16)TEX_ASSIGN_TOKS, TEX_ERR_HELP_LOC);
	
	primitive(TeXPreTolerance, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_PRETOLERANCE_CODE);
	primitive(TeXTolerance, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TOLERANCE_CODE);
	primitive(TeXLinePenalty, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_LINE_PENALTY_CODE);
	primitive(TeXHyphenPenalty,(quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_HYPHEN_PENALTY_CODE);
	primitive(TeXExhyphenPenalty,(quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_EX_HYPHEN_PENALTY_CODE);
	primitive(TeXClubPenalty, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_CLUB_PENALTY_CODE);
	primitive(TeXWidowPenalty, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_WIDOW_PENALTY_CODE);
	primitive(TeXDisplayWidowPenalty,
  				(quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_DISPLAY_WIDOW_PENALTY_CODE);
	primitive(TeXBrokenPenalty, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_BROKEN_PENALTY_CODE);
	primitive(TeXBinOpPenalty, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_BIN_OP_PENALTY_CODE);
	primitive(TeXRelPenalty, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_REL_PENALTY_CODE);
	primitive(TeXPredisplayPenalty, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_PRE_DISPLAY_PENALTY_CODE);
	primitive(TeXPostdisplayPenalty, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_POST_DISPLAY_PENALTY_CODE);
	primitive(TeXInterLinePenalty, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_INTER_LINE_PENALTY_CODE);
	primitive(TeXDoubleHyphenDemerits,
  				(quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_DOUBLE_HYPHEN_DEMERITS_CODE);
	primitive(TeXFinalHyphenDemerits,
  				(quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_FINAL_HYPHEN_DEMERITS_CODE);
	primitive(TeXAdjDemerits, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_ADJ_DEMERITS_CODE);
	primitive(TeXMag, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_MAG_CODE);
	primitive(TeXDelimiterFactor, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_DELIMITER_FACTOR_CODE);
	primitive(TeXLooseness, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_LOOSENESS_CODE);
	primitive(TeXTime, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TIME_CODE);
	primitive(TeXDay, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_DAY_CODE);
	primitive(TeXMonth, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_MONTH_CODE);
	primitive(TeXYear, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_YEAR_CODE);
	primitive(TeXShowBoxBreadth, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_SHOW_BOX_BREADTH_CODE);
	primitive(TeXShowBoxDepth, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_SHOW_BOX_DEPTH_CODE);
	primitive(TeXHBadness, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_HBADNESS_CODE);
	primitive(TeXVBadness, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_VBADNESS_CODE);
	primitive(TeXPausing, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_PAUSING_CODE);
	primitive(TeXTracingOnline, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TRACING_ONLINE_CODE);
	primitive(TeXTracingMacros, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TRACING_MACROS_CODE);
	primitive(TeXTracingStats, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TRACING_STATS_CODE);
	primitive(TeXTracingParagraphs, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TRACING_PARAGRAPHS_CODE);
	primitive(TeXTracingPages, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TRACING_PAGES_CODE);
	primitive(TeXTracingOutput, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TRACING_OUTPUT_CODE);
	primitive(TeXTracingLostChars, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TRACING_LOST_CHARS_CODE);
	primitive(TeXTracingCommands, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TRACING_COMMANDS_CODE);
	primitive(TeXTracingRestores, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_TRACING_RESTORES_CODE);
	primitive(TeXUcHyph, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_UC_HYPH_CODE);
	primitive(TeXOutputPenalty, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_OUTPUT_PENALTY_CODE);
	primitive(TeXMaxDeadCycles, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_MAX_DEAD_CYCLES_CODE);
	primitive(TeXHangAfter, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_HANG_AFTER_CODE);
	primitive(TeXFloatingPenalty, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_FLOATING_PENALTY_CODE);
	primitive(TeXGlobalDefs, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_GLOBAL_DEFS_CODE);
	primitive(TeXFam, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_CUR_FAM_CODE);
	primitive(TeXEscapeChar, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_ESCAPE_CHAR_CODE);
	primitive(TeXDefaultHyphenChar, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_DEFAULT_HYPHEN_CHAR_CODE);
	primitive(TeXDefaultSkewChar, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_DEFAULT_SKEW_CHAR_CODE);
	primitive(TeXEndLineChar, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_END_LINE_CHAR_CODE);
	primitive(TeXNewLineChar, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_NEW_LINE_CHAR_CODE);
	primitive(TeXLanguage, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_LANGUAGE_CODE);
	primitive(TeXLeftHyphenMin, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_LEFT_HYPHEN_MIN_CODE);
	primitive(TeXRightHyphenMin, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_RIGHT_HYPHEN_MIN_CODE);
	primitive(TeXHoldingInserts, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_HOLDING_INSERTS_CODE);
	primitive(TeXErrorContextLines, (quint16)TEX_ASSIGN_INT, TEX_INT_BASE + TEX_ERROR_CONTEXT_LINES_CODE);
	
	primitive(TeXParIndent, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_PAR_INDENT_CODE);
	primitive(TeXMathSurround, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_MATH_SURROUND_CODE);
	primitive(TeXLineSkipLimit, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_LINE_SKIP_LIMIT_CODE);
	primitive(TeXHSize, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_HSIZE_CODE);
	primitive(TeXVSize, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_VSIZE_CODE);
	primitive(TeXMaxDepth, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_MAX_DEPTH_CODE);
	primitive(TeXSplitMaxDepth, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_SPLIT_MAX_DEPTH_CODE);
	primitive(TeXBoxMaxDepth, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_BOX_MAX_DEPTH_CODE);
	primitive(TeXHFuzz, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_HFUZZ_CODE);
	primitive(TeXVFuzz, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_VFUZZ_CODE);
	primitive(TeXDelimiterShortFall,
  				(quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_DELIMITER_SHORTFALL_CODE);
	primitive(TeXNullDelimiterSpace,
  				(quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_NULL_DELIMITER_SPACE_CODE);
	primitive(TeXScriptSpace, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_SCRIPT_SPACE_CODE);
	primitive(TeXPredisplaySize, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_PRE_DISPLAY_SIZE_CODE);
	primitive(TeXDisplayWidth, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_DISPLAY_WIDTH_CODE);
	primitive(TeXDisplayIndent, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_DISPLAY_INDENT_CODE);
	primitive(TeXOverFullRule, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_OVERFULL_RULE_CODE);
	primitive(TeXHangIndent, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_HANG_INDENT_CODE);
	primitive(TeXHOffset, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_H_OFFSET_CODE);
	primitive(TeXVOffset, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_V_OFFSET_CODE);
	primitive(TeXEmergencyStretch, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_EMERGENCY_STRETCH_CODE);
	
	primitive(' ', (quint16)TEX_EX_SPACE, 0);
	primitive('/', (quint16)TEX_ITAL_CORR, 0);
	primitive(TeXAccent, (quint16)TEX_ACCENT, 0);
	primitive(TeXAdvance, (quint16)TEX_ADVANCE, 0);
	primitive(TeXAfterAssignment, (quint16)TEX_AFTER_ASSIGNMENT, 0);
	primitive(TeXAfterGroup, (quint16)TEX_AFTER_GROUP, 0);
	primitive(TeXBeginGroup, (quint16)TEX_BEGIN_GROUP, 0);
	primitive(TeXChar, (quint16)TEX_CHAR_NUM, 0);
	primitive(TeXCsName, (quint16)TEX_CS_NAME, 0);
	primitive(TeXDelimiter, (quint16)TEX_DELIM_NUM, 0);
	primitive(TeXODelimiter, (quint16)TEX_DELIM_NUM, 1);
	primitive(TeXDivide, (quint16)TEX_DIVIDE, 0);
	primitive(TeXEndCsName, (quint16)TEX_END_CS_NAME, 0);
	primitive(TeXEndGroup, (quint16)TEX_END_GROUP, 0);
	setText(TEX_FROZEN_END_GROUP, TeXEndGroup);
	setNewEqtb(TEX_FROZEN_END_GROUP, newEqtb(cur_val));
	primitive(TeXExpandAfter, (quint16)TEX_EXPAND_AFTER, 0);
	primitive(TeXFont, (quint16)TEX_DEF_FONT, 0);
	primitive(TeXFontDimen, (quint16)TEX_ASSIGN_FONT_DIMEN, TEX_FONT_DIMEN_CODE);
	primitive(TeXCharWD, (quint16)TEX_ASSIGN_FONT_DIMEN, TEX_CHAR_WIDTH_CODE);
	primitive(TeXCharHT, (quint16)TEX_ASSIGN_FONT_DIMEN, TEX_CHAR_HEIGHT_CODE);
	primitive(TeXCharDP, (quint16)TEX_ASSIGN_FONT_DIMEN, TEX_CHAR_DEPTH_CODE);
	primitive(TeXCharIT, (quint16)TEX_ASSIGN_FONT_DIMEN, TEX_CHAR_ITALIC_CODE);
	primitive(TeXHAlign, (quint16)TEX_HALIGN, 0);
	primitive(TeXHRule, (quint16)TEX_HRULE, 0);
	primitive(TeXIgnoreSpaces, (quint16)TEX_IGNORE_SPACES, 0);
	primitive(TeXInsert, (quint16)TEX_INSERT, 0);
	primitive(TeXLeftGHost, (quint16)TEX_CHAR_GHOST, 0);
	primitive(TeXMark, (quint16)TEX_MARK, 0);
	primitive(TeXMathAccent, (quint16)TEX_MATH_ACCENT, 0);
	primitive(TeXMathChar, (quint16)TEX_MATH_CHAR_NUM, 0);
	primitive(TeXOMathAccent, (quint16)TEX_MATH_ACCENT, 1);
	primitive(TeXOMathChar, (quint16)TEX_MATH_CHAR_NUM, 1);
	primitive(TeXMathChoice, (quint16)TEX_MATH_CHOICE, 0);
	primitive(TeXMultiply, (quint16)TEX_MULTIPLY, 0);
	primitive(TeXNoAlign, (quint16)TEX_NO_ALIGN, 0);
	primitive(TeXNoBoundary, (quint16)TEX_NO_BOUNDARY, 0);
	primitive(TeXNoExpand, (quint16)TEX_NO_EXPAND, 0);
	primitive(TeXNonScript, (quint16)TEX_NON_SCRIPT, 0);
	primitive(TeXOmit, (quint16)TEX_OMIT, 0);
	primitive(TeXParShape, (quint16)TEX_SET_SHAPE, TEX_PAR_SHAPE_LOC);
	primitive(TeXPenalty, (quint16)TEX_BREAK_PENALTY, 0);
	primitive(TeXPrevGraf, (quint16)TEX_SET_PREV_GRAF, 0);
	primitive(TeXRadical, (quint16)TEX_RADICAL, 0);
	primitive(TeXORadical, (quint16)TEX_RADICAL, 1);
	primitive(TeXRead, (quint16)TEX_READ_TO_CS, 0);
	primitive(TeXRelax, (quint16)TEX_RELAX, TEX_TOO_BIG_CHAR);
	setText(TEX_FROZEN_RELAX, TeXRelax);
	setNewEqtb(TEX_FROZEN_RELAX, newEqtb(cur_val));
	primitive(TeXRightGHost, (quint16)TEX_CHAR_GHOST, 1);
	primitive(TeXSetBox, (quint16)TEX_SET_BOX, 0);
	primitive(TeXThe, (quint16)TEX_THE, 0);
	primitive(TeXToks, (quint16)TEX_TOKS_REGISTER, mem_bot);
	primitive(TeXVAdjust, (quint16)TEX_VADJUST, 0);
	primitive(TeXVAlign, (quint16)TEX_VALIGN, 0);
	primitive(TeXVCenter, (quint16)TEX_VCENTER, 0);
	primitive(TeXVRule, (quint16)TEX_VRULE, 0);
	
	primitive(TeXPar, (quint16)TEX_PAR_END, TEX_TOO_BIG_CHAR);
	par_loc = cur_val; 
	par_token = TEX_CS_TOKEN_FLAG + par_loc;
	
	primitive(TeXInput, (quint16)TEX_INPUT, 0);
	primitive(TeXEndInput, (quint16)TEX_INPUT, 1);
	
	primitive(TeXTopMark, (quint16)TEX_TOP_BOT_MARK, TEX_TOP_MARK_CODE);
	primitive(TeXFirstMark, (quint16)TEX_TOP_BOT_MARK, TEX_FIRST_MARK_CODE);
	primitive(TeXBotMark, (quint16)TEX_TOP_BOT_MARK, TEX_BOT_MARK_CODE);
	primitive(TeXSplitFirstMark, (quint16)TEX_TOP_BOT_MARK, TEX_SPLIT_FIRST_MARK_CODE);
	primitive(TeXSplitBotMark, (quint16)TEX_TOP_BOT_MARK, TEX_SPLIT_BOT_MARK_CODE);
	
	primitive(TeXCount,  (quint16)TEX_REGISTER, mem_bot + TEX_INT_VAL);
	primitive(TeXDimen,  (quint16)TEX_REGISTER, mem_bot + TEX_DIMEN_VAL);
	primitive(TeXSkip,   (quint16)TEX_REGISTER, mem_bot + TEX_GLUE_VAL);
	primitive(TeXMuSkip, (quint16)TEX_REGISTER, mem_bot + TEX_MU_VAL);
	
	primitive(TeXSpaceFactor, (quint16)TEX_SET_AUX, TEX_HMODE);
	primitive(TeXPrevDepth, (quint16)TEX_SET_AUX, TEX_VMODE);
	primitive(TeXDeadCycles, (quint16)TEX_SET_PAGE_INT, 0);
	primitive(TeXInsertPenalties, (quint16)TEX_SET_PAGE_INT, 1);
	primitive(TeXWD, (quint16)TEX_SET_BOX_DIMEN, TEX_WIDTH_OFFSET);
	primitive(TeXHT, (quint16)TEX_SET_BOX_DIMEN, TEX_HEIGHT_OFFSET);
	primitive(TeXDP, (quint16)TEX_SET_BOX_DIMEN, TEX_DEPTH_OFFSET);
	primitive(TeXLastPenalty, (quint16)TEX_LAST_ITEM, TEX_INT_VAL);
	primitive(TeXLastKern, (quint16)TEX_LAST_ITEM, TEX_DIMEN_VAL);
	primitive(TeXLastSkip, (quint16)TEX_LAST_ITEM, TEX_GLUE_VAL);
	primitive(TeXInputLineNo, (quint16)TEX_LAST_ITEM, TEX_INPUT_LINE_NO_CODE);
	primitive(TeXBadness, (quint16)TEX_LAST_ITEM, TEX_BADNESS_CODE);
	
	primitive(TeXNumber, (quint16)TEX_CONVERT, TEX_NUMBER_CODE);
	primitive(TeXRomanNumeral, (quint16)TEX_CONVERT, TEX_ROMAN_NUMERAL_CODE);
	primitive(TeXString, (quint16)TEX_CONVERT, TEX_STRING_CODE);
	primitive(TeXMeaning, (quint16)TEX_CONVERT, TEX_MEANING_CODE);
	primitive(TeXFontName, (quint16)TEX_CONVERT, TEX_FONT_NAME_CODE);
	primitive(TeXeTeXVersion, (quint16)TEX_CONVERT, TEX_ETEX_CODE);
	primitive(TeXOmegaVersion, (quint16)TEX_CONVERT, TEX_OMEGA_CODE);
	primitive(TeXAlephVersion, (quint16)TEX_CONVERT, TEX_ALEPH_CODE);
	primitive(TeXJobName, (quint16)TEX_CONVERT, TEX_JOB_NAME_CODE);
	
	primitive(TeXIF, (quint16)TEX_IF_TEST, TEX_IF_CHAR_CODE);
	primitive(TeXIfCat, (quint16)TEX_IF_TEST, TEX_IF_CAT_CODE);
	primitive(TeXIfNum, (quint16)TEX_IF_TEST, TEX_IF_INT_CODE);
	primitive(TeXIfDim, (quint16)TEX_IF_TEST, TEX_IF_DIM_CODE);
	primitive(TeXIfOdd, (quint16)TEX_IF_TEST, TEX_IF_ODD_CODE);
	primitive(TeXIfVMode, (quint16)TEX_IF_TEST, TEX_IF_VMODE_CODE);
	primitive(TeXIfHMode, (quint16)TEX_IF_TEST, TEX_IF_HMODE_CODE);
	primitive(TeXIfMMode, (quint16)TEX_IF_TEST, TEX_IF_MMODE_CODE);
	primitive(TeXIfInner, (quint16)TEX_IF_TEST, TEX_IF_INNER_CODE);
	primitive(TeXIfVoid, (quint16)TEX_IF_TEST, TEX_IF_VOID_CODE);
	primitive(TeXIfHBox, (quint16)TEX_IF_TEST, TEX_IF_HBOX_CODE);
	primitive(TeXIfVBox, (quint16)TEX_IF_TEST, TEX_IF_VBOX_CODE);
	primitive(TeXIfX, (quint16)TEX_IF_TEST, TEX_IFX_CODE);
	primitive(TeXIfEof, (quint16)TEX_IF_TEST, TEX_IF_EOF_CODE);
	primitive(TeXIfTrue, (quint16)TEX_IF_TEST, TEX_IF_TRUE_CODE);
	primitive(TeXIfFalse, (quint16)TEX_IF_TEST, TEX_IF_FALSE_CODE);
	primitive(TeXIfCase, (quint16)TEX_IF_TEST, TEX_IF_CASE_CODE);
	
	primitive(TeXFI, (quint16)TEX_FI_OR_ELSE, TEX_FI_CODE);
	setText(TEX_FROZEN_FI, TeXFI); 
	setNewEqtb(TEX_FROZEN_FI, newEqtb(cur_val));
	primitive(TeXOR, (quint16)TEX_FI_OR_ELSE, TEX_OR_CODE);
	primitive(TeXElse, (quint16)TEX_FI_OR_ELSE, TEX_ELSE_CODE);
		
	primitive(TeXNullFont, (quint16)TEX_SET_FONT, TEX_NULL_FONT);
	setText(TEX_FROZEN_NULL_FONT, TeXNullFont);
	setNewEqtb(TEX_FROZEN_NULL_FONT, newEqtb(cur_val));
	
	primitive(TeXNullOcp, (quint16)TEX_SET_OCP, TEX_NULL_OCP);
	setText(TEX_FROZEN_NULL_OCP, TeXNullOcp);
	setNewEqtb(TEX_FROZEN_NULL_OCP, newEqtb(cur_val));
	geqDefine(TEX_OCP_ACTIVE_NUMBER_BASE,  (quint16)TEX_DATA, 0);
	geqDefine(TEX_OCP_ACTIVE_MIN_PTR_BASE, (quint16)TEX_DATA, 0);
	geqDefine(TEX_OCP_ACTIVE_MAX_PTR_BASE, (quint16)TEX_DATA, 0);
	
	primitive(TeXNullOcpList, (quint16)TEX_SET_OCP_LIST, TEX_NULL_OCP_LIST);
	setText(TEX_FROZEN_NULL_OCP_LIST, TeXNullOcpList);
	setNewEqtb(TEX_FROZEN_NULL_OCP_LIST, newEqtb(cur_val));
	
	primitive(TeXSpan, (quint16)TEX_TAB_MARK, TEX_SPAN_CODE);
	primitive(TeXCr, (quint16)TEX_CAR_RET, TEX_CR_CODE);
	setText(TEX_FROZEN_CR, TeXCr);
	setNewEqtb(TEX_FROZEN_CR, newEqtb(cur_val));
	primitive(TeXCrCr, (quint16)TEX_CAR_RET, TEX_CR_CR_CODE);
	setText(TEX_FROZEN_END_TEMPLATE, TeXEndTemplate); 
	setText(TEX_FROZEN_ENDV, TeXEndTemplate);
	setEqType(TEX_FROZEN_ENDV, TEX_ENDV); 
	setEquiv(TEX_FROZEN_ENDV, null_list);
	setEqLevel(TEX_FROZEN_ENDV, TEX_LEVEL_ONE);
	setNewEqtb(TEX_FROZEN_END_TEMPLATE, newEqtb(TEX_FROZEN_ENDV));
	setEqType(TEX_FROZEN_END_TEMPLATE, TEX_END_TEMPLATE);
	
	primitive(TeXPageGoal, (quint16)TEX_SET_PAGE_DIMEN, 0);
	primitive(TeXPageTotal, (quint16)TEX_SET_PAGE_DIMEN, 1);
	primitive(TeXPageStretch, (quint16)TEX_SET_PAGE_DIMEN, 2);
	primitive(TeXPageFilStretch, (quint16)TEX_SET_PAGE_DIMEN, 3);
	primitive(TeXPageFillStretch, (quint16)TEX_SET_PAGE_DIMEN, 4);
	primitive(TeXPageFilllStretch, (quint16)TEX_SET_PAGE_DIMEN, 5);
	primitive(TeXPageShrink, (quint16)TEX_SET_PAGE_DIMEN, 6);
	primitive(TeXPageDepth, (quint16)TEX_SET_PAGE_DIMEN, 7);
	
	primitive(TeXEnd, (quint16)TEX_STOP, 0);
	primitive(TeXDump, (quint16)TEX_STOP, 1);
	
	primitive(TeXHSkip, (quint16)TEX_HSKIP, TEX_SKIP_CODE);
	primitive(TeXHFil, (quint16)TEX_HSKIP, TEX_FIL_CODE);
	primitive(TeXHFill, (quint16)TEX_HSKIP, TEX_FILL_CODE);
	primitive(TeXHSs, (quint16)TEX_HSKIP, TEX_SS_CODE);
	primitive(TeXHFilNeg, (quint16)TEX_HSKIP, TEX_FIL_NEG_CODE);
	primitive(TeXVSkip, (quint16)TEX_VSKIP, TEX_SKIP_CODE);
	primitive(TeXVFil, (quint16)TEX_VSKIP, TEX_FIL_CODE);
	primitive(TeXVFill, (quint16)TEX_VSKIP, TEX_FILL_CODE);
	primitive(TeXVSs, (quint16)TEX_VSKIP, TEX_SS_CODE);
	primitive(TeXVFilNeg, (quint16)TEX_VSKIP, TEX_FIL_NEG_CODE);
	primitive(TeXMSkip, (quint16)TEX_MSKIP, TEX_MSKIP_CODE);
	primitive(TeXKern, (quint16)TEX_KERN, TEX_EXPLICIT);
	primitive(TeXMKern, (quint16)TEX_MKERN, TEX_MU_GLUE);
	
	primitive(TeXMoveLeft, (quint16)TEX_HMOVE, 1);
	primitive(TeXMoveRight, (quint16)TEX_HMOVE, 0);
	primitive(TeXRaise, (quint16)TEX_VMOVE, 1);
	primitive(TeXLower, (quint16)TEX_VMOVE, 0);
	primitive(TeXBox, (quint16)TEX_MAKE_BOX, TEX_BOX_CODE);
	primitive(TeXCopy, (quint16)TEX_MAKE_BOX, TEX_COPY_CODE);
	primitive(TeXLastBox, (quint16)TEX_MAKE_BOX, TEX_LAST_BOX_CODE);
	primitive(TeXVSplit, (quint16)TEX_MAKE_BOX, TEX_VSPLIT_CODE);
	primitive(TeXVTop, (quint16)TEX_MAKE_BOX, TEX_VTOP_CODE);
	primitive(TeXVBox, (quint16)TEX_MAKE_BOX, TEX_VTOP_CODE + TEX_VMODE);
	primitive(TeXHBox, (quint16)TEX_MAKE_BOX, TEX_VTOP_CODE + TEX_HMODE);
	primitive(TeXShipOut, (quint16)TEX_LEADER_SHIP, TEX_A_LEADERS - 1);
	primitive(TeXLeaders, (quint16)TEX_LEADER_SHIP, TEX_A_LEADERS);
	primitive(TeXCLeaders, (quint16)TEX_LEADER_SHIP, TEX_C_LEADERS);
	primitive(TeXXLeaders, (quint16)TEX_LEADER_SHIP, TEX_X_LEADERS);
	primitive(TeXBoxDir, (quint16)TEX_ASSIGN_BOX_DIR, 0);
	
	primitive(TeXIndent, (quint16)TEX_START_PAR, 1);
	primitive(TeXNoIndent, (quint16)TEX_START_PAR, 0);
	
	primitive(TeXUnPenalty, (quint16)TEX_REMOVE_ITEM, TEX_PENALTY_NODE);
	primitive(TeXUnKern, (quint16)TEX_REMOVE_ITEM, TEX_KERN_NODE);
	primitive(TeXUnSkip, (quint16)TEX_REMOVE_ITEM, TEX_GLUE_NODE);
	primitive(TeXUnHBox, (quint16)TEX_UN_HBOX, TEX_BOX_CODE);
	primitive(TeXUnHCopy, (quint16)TEX_UN_HBOX, TEX_COPY_CODE);
	primitive(TeXUnVBox, (quint16)TEX_UN_VBOX, TEX_BOX_CODE);
	primitive(TeXUnVCopy, (quint16)TEX_UN_VBOX, TEX_COPY_CODE);
	
	primitive(TeXDiscret, (quint16)TEX_DISCRETIONARY, 1);
	primitive(TeXDiscretionary, (quint16)TEX_DISCRETIONARY, 0);
	primitive(TeXLocalLeftBox, (quint16)TEX_ASSIGN_LOCAL_BOX, 0);
	primitive(TeXLocalRightBox, (quint16)TEX_ASSIGN_LOCAL_BOX, 1);
	
	primitive(TeXEqNo, (quint16)TEX_EQ_NO, 0);
	primitive(TeXLEqNo, (quint16)TEX_EQ_NO, 1);
	
	primitive(TeXMathOrd, (quint16)TEX_MATH_COMP, TEX_ORD_NOAD);
	primitive(TeXMathOp, (quint16)TEX_MATH_COMP, TEX_OP_NOAD);
	primitive(TeXMathBin, (quint16)TEX_MATH_COMP, TEX_BIN_NOAD);
	primitive(TeXMathRel, (quint16)TEX_MATH_COMP, TEX_REL_NOAD);
	primitive(TeXMathOpen, (quint16)TEX_MATH_COMP, TEX_OPEN_NOAD);
	primitive(TeXMathClose, (quint16)TEX_MATH_COMP, TEX_CLOSE_NOAD);
	primitive(TeXMathPunct, (quint16)TEX_MATH_COMP, TEX_PUNCT_NOAD);
	primitive(TeXMathInner, (quint16)TEX_MATH_COMP, TEX_INNER_NOAD);
	primitive(TeXUnderLine, (quint16)TEX_MATH_COMP, TEX_UNDER_NOAD);
	primitive(TeXOverLine, (quint16)TEX_MATH_COMP, TEX_OVER_NOAD);
	primitive(TeXDisplayLimits, (quint16)TEX_LIMIT_SWITCH, TEX_NORMAL);
	primitive(TeXLimits, (quint16)TEX_LIMIT_SWITCH, TEX_LIMITS);
	primitive(TeXNoLimits, (quint16)TEX_LIMIT_SWITCH, TEX_NO_LIMITS);
	
	primitive(TeXDisplayStyle, (quint16)TEX_MATH_STYLE, TEX_DISPLAY_STYLE);
	primitive(TeXTextStyle, (quint16)TEX_MATH_STYLE, TEX_TEXT_STYLE);
	primitive(TeXScriptStyle, (quint16)TEX_MATH_STYLE, TEX_SCRIPT_STYLE);
	primitive(TeXScriptScriptStyle, (quint16)TEX_MATH_STYLE, TEX_SCRIPT_SCRIPT_STYLE);
	
	primitive(TeXAbove, (quint16)TEX_ABOVE, TEX_ABOVE_CODE);
	primitive(TeXOver, (quint16)TEX_ABOVE, TEX_OVER_CODE);
	primitive(TeXATop, (quint16)TEX_ABOVE, TEX_ATOP_CODE);
	primitive(TeXAboveWithDelims, (quint16)TEX_ABOVE, TEX_DELIMITED_CODE + TEX_ABOVE_CODE);
	primitive(TeXOverWithDelims, (quint16)TEX_ABOVE, TEX_DELIMITED_CODE + TEX_OVER_CODE);
	primitive(TeXATopWithDelims, (quint16)TEX_ABOVE, TEX_DELIMITED_CODE + TEX_ATOP_CODE);
	
	primitive(TeXLeft, (quint16)TEX_LEFT_RIGHT, TEX_LEFT_NOAD);
	primitive(TeXRight, (quint16)TEX_LEFT_RIGHT, TEX_RIGHT_NOAD);
	setText(TEX_FROZEN_RIGHT, TeXRight);
	setNewEqtb(TEX_FROZEN_RIGHT, newEqtb(cur_val));
	
	primitive(TeXLong, (quint16)TEX_PREFIX, 1);
	primitive(TeXOuter, (quint16)TEX_PREFIX, 2);
	primitive(TeXGlobal, (quint16)TEX_PREFIX, 4);
	primitive(TeXDef, (quint16)TEX_DEF, 0);
	primitive(TeXGDef, (quint16)TEX_DEF, 1);
	primitive(TeXEDef, (quint16)TEX_DEF, 2);
	primitive(TeXXDef, (quint16)TEX_DEF, 3);
	
	primitive(TeXLet, (quint16)TEX_LET, TEX_NORMAL);
	primitive(TeXFutureLet, (quint16)TEX_LET, TEX_NORMAL + 1);
	
	primitive(TeXCharDef, (quint16)TEX_SHORTHAND_DEF, TEX_CHAR_DEF_CODE);
	primitive(TeXMathCharDef, (quint16)TEX_SHORTHAND_DEF, TEX_MATH_CHAR_DEF_CODE);
	primitive(TeXOMathCharDef, (quint16)TEX_SHORTHAND_DEF, TEX_OMATH_CHAR_DEF_CODE);
	primitive(TeXCountDef, (quint16)TEX_SHORTHAND_DEF, TEX_COUNT_DEF_CODE);
	primitive(TeXDimenDef, (quint16)TEX_SHORTHAND_DEF, TEX_DIMEN_DEF_CODE);
	primitive(TeXSkipDef, (quint16)TEX_SHORTHAND_DEF, TEX_SKIP_DEF_CODE);
	primitive(TeXMuSkipDef, (quint16)TEX_SHORTHAND_DEF, TEX_MU_SKIP_DEF_CODE);
	primitive(TeXToksDef, (quint16)TEX_SHORTHAND_DEF, TEX_TOKS_DEF_CODE);
	
	primitive(TeXCatCode, (quint16)TEX_DEF_CODE, TEX_CAT_CODE_BASE);
	primitive(TeXMathCode, (quint16)TEX_DEF_CODE, TEX_MATH_CODE_BASE);
	primitive(TeXOMathCode, (quint16)TEX_DEF_CODE, TEX_MATH_CODE_BASE + 256);
	primitive(TeXLcCode, (quint16)TEX_DEF_CODE, TEX_LC_CODE_BASE);
	primitive(TeXUcCode, (quint16)TEX_DEF_CODE, TEX_UC_CODE_BASE);
	primitive(TeXSfCode, (quint16)TEX_DEF_CODE, TEX_SF_CODE_BASE);
	primitive(TeXDelCode, (quint16)TEX_DEF_CODE, TEX_DEL_CODE_BASE);
	primitive(TeXODelCode, (quint16)TEX_DEF_CODE, TEX_DEL_CODE_BASE + 256);
	primitive(TeXTextFont, (quint16)TEX_DEF_FAMILY, TEX_MATH_FONT_BASE);
	primitive(TeXScriptFont, (quint16)TEX_DEF_FAMILY, TEX_MATH_FONT_BASE + TEX_SCRIPT_SIZE);
	primitive(TeXScriptScriptFont, (quint16)TEX_DEF_FAMILY, TEX_MATH_FONT_BASE + TEX_SCRIPT_SCRIPT_SIZE);
	
	primitive(TeXHyphenation, (quint16)TEX_HYPH_DATA, 0);
	primitive(TeXPatterns, (quint16)TEX_HYPH_DATA, 1);
	
	primitive(TeXHyphenChar, (quint16)TEX_ASSIGN_FONT_INT, 0);
	primitive(TeXSkewChar, (quint16)TEX_ASSIGN_FONT_INT, 1);
	
	primitive(TeXBatchMode, (quint16)TEX_SET_INTERACTION, TEX_BATCH_MODE);
	primitive(TeXNonStopMode, (quint16)TEX_SET_INTERACTION, TEX_NONSTOP_MODE);
	primitive(TeXScrollMode, (quint16)TEX_SET_INTERACTION, TEX_SCROLL_MODE);
	primitive(TeXErrorStopMode, (quint16)TEX_SET_INTERACTION, TEX_ERROR_STOP_MODE);
	
	primitive(TeXOpenIn, (quint16)TEX_IN_STREAM, 1);
	primitive(TeXCloseIn, (quint16)TEX_IN_STREAM, 0);
	
	primitive(TeXMessage, (quint16)TEX_MESSAGE, 0);
	primitive(TeXErrMessage, (quint16)TEX_MESSAGE, 1);
	
	primitive(TeXLowerCase, (quint16)TEX_CASE_SHIFT, TEX_LC_CODE_BASE);
	primitive(TeXUpperCase, (quint16)TEX_CASE_SHIFT, TEX_UC_CODE_BASE);
	
	primitive(TeXShow, (quint16)TEX_XRAY, TEX_SHOW_CODE);
	primitive(TeXShowBox, (quint16)TEX_XRAY, TEX_SHOW_BOX_CODE);
	primitive(TeXShowThe, (quint16)TEX_XRAY, TEX_SHOW_THE_CODE);
	primitive(TeXShowLists, (quint16)TEX_XRAY, TEX_SHOW_LISTS);
	
	primitive(TeXOpenOut, (quint16)TEX_EXTENSION, TEX_OPEN_NODE);
	primitive(TeXWrite, (quint16)TEX_EXTENSION, TEX_WRITE_NODE); 
	write_loc = cur_val;
	primitive(TeXCloseOut, (quint16)TEX_EXTENSION, TEX_CLOSE_NODE);
	primitive(TeXSpecial, (quint16)TEX_EXTENSION, TEX_SPECIAL_NODE);
	special_loc = cur_val; 
	special_token = TEX_CS_TOKEN_FLAG + special_loc;
	primitive(TeXImmediate, (quint16)TEX_EXTENSION, TEX_IMMEDIATE_CODE);
	primitive(TeXSetLanguage, (quint16)TEX_EXTENSION, TEX_SET_LANGUAGE_CODE);
	primitive(TeXLocalInterLinePenalty, (quint16)TEX_ASSIGN_INT, TEX_LOCAL_INTER_LINE_PENALTY_CODE);
	primitive(TeXLocalBrokenPenalty, (quint16)TEX_ASSIGN_INT, TEX_LOCAL_BROKEN_PENALTY_CODE);
	primitive(TeXPageDir, (quint16)TEX_ASSIGN_DIR, TEX_DIR_BASE + TEX_PAGE_DIRECTION_CODE);
	primitive(TeXBodyDir, (quint16)TEX_ASSIGN_DIR, TEX_DIR_BASE + TEX_BODY_DIRECTION_CODE);
	primitive(TeXParDir, (quint16)TEX_ASSIGN_DIR, TEX_DIR_BASE + TEX_PAR_DIRECTION_CODE);
	primitive(TeXTextDir, (quint16)TEX_ASSIGN_DIR, TEX_DIR_BASE + TEX_TEXT_DIRECTION_CODE);
	primitive(TeXMathDir, (quint16)TEX_ASSIGN_DIR, TEX_DIR_BASE + TEX_MATH_DIRECTION_CODE);
	primitive(TeXPageHeight, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_PAGE_HEIGHT_CODE);
	primitive(TeXPageWidth, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_PAGE_WIDTH_CODE);
	primitive(TeXPageRightOffset, (quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_PAGE_RIGHT_OFFSET_CODE);
	primitive(TeXPageBottomOffset,(quint16)TEX_ASSIGN_DIMEN, TEX_DIMEN_BASE + TEX_PAGE_BOTTOM_OFFSET_CODE);
	
	primitive(TeXInputTranslation,           (quint16)TEX_CHAR_TRANS, TEX_TRANS_INPUT);
	primitive(TeXOutputTranslation,          (quint16)TEX_CHAR_TRANS, TEX_TRANS_OUTPUT);
	primitive(TeXDefaultInputTranslation,    (quint16)TEX_CHAR_TRANS, TEX_TRANS_DEFAULT_INPUT);
	primitive(TeXDefaultOutputTranslation,   (quint16)TEX_CHAR_TRANS, TEX_TRANS_DEFAULT_OUTPUT);
	primitive(TeXnoInputTranslation,         (quint16)TEX_CHAR_TRANS, TEX_TRANS_NO_INPUT);
	primitive(TeXnoOutputTranslation,        (quint16)TEX_CHAR_TRANS, TEX_TRANS_NO_OUTPUT);
	primitive(TeXnoDefaultInputTranslation,  (quint16)TEX_CHAR_TRANS, TEX_TRANS_NO_DEFAULT_INPUT);
	primitive(TeXnoDefaultOutputTranslation, (quint16)TEX_CHAR_TRANS, TEX_TRANS_NO_DEFAULT_OUTPUT);
	primitive(TeXInputMode,                  (quint16)TEX_CHAR_MODE,  TEX_MODE_INPUT);
	primitive(TeXOutputMode,                 (quint16)TEX_CHAR_MODE,  TEX_MODE_OUTPUT);
	primitive(TeXDefaultInputMode,           (quint16)TEX_CHAR_MODE,  TEX_MODE_DEFAULT_INPUT);
	primitive(TeXDefaultOutputMode,          (quint16)TEX_CHAR_MODE,  TEX_MODE_DEFAULT_OUTPUT);
	primitive(TeXnoInputMode,                (quint16)TEX_CHAR_MODE,  TEX_MODE_NO_INPUT);
	primitive(TeXnoOutputMode,               (quint16)TEX_CHAR_MODE,  TEX_MODE_NO_OUTPUT);
	primitive(TeXnoDefaultInputMode,         (quint16)TEX_CHAR_MODE,  TEX_MODE_NO_DEFAULT_INPUT);
	primitive(TeXnoDefaultOutputMode,        (quint16)TEX_CHAR_MODE,  TEX_MODE_NO_DEFAULT_OUTPUT);

	geqDefine(TEX_OCP_INPUT_ONEBYTE_TRANSLATION_BASE, (quint16)TEX_DATA, 0);
	geqDefine(TEX_OCP_INPUT_EBCDIC_TRANSLATION_BASE, (quint16)TEX_DATA, 0);
	geqDefine(TEX_OCP_INPUT_TWOBYTE_TRANSLATION_BASE, (quint16)TEX_DATA, 0);
	geqDefine(TEX_OCP_INPUT_TWOBYTELE_TRANSLATION_BASE, (quint16)TEX_DATA, 0);
	geqDefine(TEX_OCP_INPUT_MODE_BASE, (quint16)TEX_DATA, 0);

	geqDefine(TEX_OCP_OUTPUT_ONEBYTE_TRANSLATION_BASE, (quint16)TEX_DATA, 0);
	geqDefine(TEX_OCP_OUTPUT_EBCDIC_TRANSLATION_BASE, (quint16)TEX_DATA, 0);
	geqDefine(TEX_OCP_OUTPUT_TWOBYTE_TRANSLATION_BASE, (quint16)TEX_DATA, 0);
	geqDefine(TEX_OCP_OUTPUT_TWOBYTELE_TRANSLATION_BASE, (quint16)TEX_DATA, 0);
	geqDefine(TEX_OCP_OUTPUT_MODE_BASE, (quint16)TEX_DATA, 0);
	
	primitive(TeXOCP, (quint16)TEX_DEF_OCP, 0);
	primitive(TeXExternalOcp, (quint16)TEX_DEF_OCP, 1);
	primitive(TeXOcpList, (quint16)TEX_DEF_OCP_LIST, 0);
	primitive(TeXPushOcpList, (quint16)TEX_PUSH_OCP_LIST, 0);
	primitive(TeXPopOcpList, (quint16)TEX_POP_OCP_LIST, 0);
	primitive(TeXClearOcpLists, (quint16)TEX_CLEAR_OCP_LISTS, 0);
	primitive(TeXAddBeforeOcpList, (quint16)TEX_OCP_LIST_OP, TEX_ADD_BEFORE_OP);
	primitive(TeXAddAfterOcpList, (quint16)TEX_OCP_LIST_OP, TEX_ADD_AFTER_OP);
	primitive(TeXRemoveBeforeOcpList, (quint16)TEX_OCP_LIST_OP, TEX_REMOVE_BEFORE_OP);
	primitive(TeXRemoveAfterOcpList, (quint16)TEX_OCP_LIST_OP, TEX_REMOVE_AFTER_OP);
	primitive(TeXOcpTraceLevel, (quint16)TEX_OCP_TRACE_LEVEL, 0);
	setEquiv(TEX_OCP_TRACE_LEVEL_BASE,0);
	
	no_new_control_sequence = true;
	
	init_str_ptr = str_ptr; 
	init_pool_ptr = pool_ptr; 
	fixDateAndTime();
	
	ready_already = 314159;
	
	return true;
}

bool XWTeX::setupBound(const QString & fmtname)
{
	if (buffer)
		clear();
		
	XWTeXFormatSetting setting;
	setting.setFormat(fmtname);
	main_memory = setting.setupBoundVar("main_memory", 1000000);
	extra_mem_top = setting.setupBoundVar("extra_mem_top", 0);
	extra_mem_bot = setting.setupBoundVar("extra_mem_bot", 0);
	pool_size = setting.setupBoundVar("pool_size", 300000);
	string_vacancies = setting.setupBoundVar("string_vacancies", 200000);
	pool_free = setting.setupBoundVar("pool_free", 5000);
	max_strings = setting.setupBoundVar("max_strings", 100000);
	strings_free = setting.setupBoundVar("strings_free", 100);
	trie_size = setting.setupBoundVar("trie_size", 20000);
	hyph_size = setting.setupBoundVar("hyph_size", 659);
	buf_size = setting.setupBoundVar("buf_size", 20000);
	nest_size = setting.setupBoundVar("nest_size", 50);
	max_in_open = setting.setupBoundVar("max_in_open", 15);
	param_size = setting.setupBoundVar("param_size", 60);
	save_size = setting.setupBoundVar("save_size", 4000);
	stack_size = setting.setupBoundVar("stack_size", 300);
	error_line = setting.setupBoundVar("error_line", 79);
	half_error_line = setting.setupBoundVar("half_error_line", 50);
	max_print_line = setting.setupBoundVar("max_print_line", 79);
	ocp_list_size = setting.setupBoundVar("ocp_list_size", 1000);
	ocp_buf_size = setting.setupBoundVar("ocp_buf_size", 1000);
	ocp_stack_size = setting.setupBoundVar("ocp_stack_size", 1000);
	
	if (main_memory < TEX_INF_MAIN_MEMORY)
		main_memory = TEX_INF_MAIN_MEMORY;
	else if (main_memory > TEX_SUP_MAIN_MEMORY)
		main_memory = TEX_SUP_MAIN_MEMORY;
		
	if (ini_version)
	{
		extra_mem_top = 0;
    	extra_mem_bot = 0;
	}
	
	if (extra_mem_bot > TEX_SUP_MAIN_MEMORY)
		extra_mem_bot = TEX_SUP_MAIN_MEMORY;
		
	if (extra_mem_top > TEX_SUP_MAIN_MEMORY)
		extra_mem_top = TEX_SUP_MAIN_MEMORY;
		
	mem_top = mem_bot + main_memory;
  	mem_min = mem_bot;
  	mem_max = mem_top;
  	
  	if (trie_size < TEX_INF_TRIE_SIZE)
  		trie_size = TEX_INF_TRIE_SIZE;
  	else if (trie_size > TEX_SUP_TRIE_SIZE)
  		trie_size = TEX_SUP_TRIE_SIZE;
  		
  	if (hyph_size < TEX_INF_HYPH_SIZE)
  		hyph_size = TEX_INF_HYPH_SIZE;
  	else if (hyph_size > TEX_SUP_HYPH_SIZE)
  		hyph_size = TEX_SUP_HYPH_SIZE;
  		
  	if (buf_size < TEX_INF_BUF_SIZE)
  		buf_size = TEX_INF_BUF_SIZE;
  	else if (buf_size > TEX_SUP_BUF_SIZE)
  		buf_size = TEX_SUP_BUF_SIZE;
  		
  	if (nest_size < TEX_INF_NEST_SIZE)
  		nest_size = TEX_INF_NEST_SIZE;
  	else if (nest_size > TEX_SUP_NEST_SIZE)
  		nest_size = TEX_SUP_NEST_SIZE;
  		
  	if (max_in_open < TEX_INF_MAX_IN_OPEN)
  		max_in_open = TEX_INF_MAX_IN_OPEN;
  	else if (max_in_open > TEX_SUP_MAX_IN_OPEN)
  		max_in_open = TEX_SUP_MAX_IN_OPEN;
  		
  	if (param_size < TEX_INF_PARAM_SIZE)
  		param_size = TEX_INF_PARAM_SIZE;
  	else if (param_size > TEX_SUP_PARAM_SIZE)
  		param_size = TEX_SUP_PARAM_SIZE;
  		
  	if (save_size < TEX_INF_SAVE_SIZE)
  		save_size = TEX_INF_SAVE_SIZE;
  	else if (save_size > TEX_SUP_SAVE_SIZE)
  		save_size = TEX_SUP_SAVE_SIZE;
  		
  	if (stack_size < TEX_INF_STACK_SIZE)
  		stack_size = TEX_INF_STACK_SIZE;
  	else if (stack_size > TEX_SUP_STACK_SIZE)
  		stack_size = TEX_SUP_STACK_SIZE;
  		
  	if (pool_size < TEX_INF_POOL_SIZE)
  		pool_size = TEX_INF_POOL_SIZE;
  	else if (pool_size > TEX_SUP_POOL_SIZE)
  		pool_size = TEX_SUP_POOL_SIZE;
  		
  	if (string_vacancies < TEX_INF_STRING_VACANCIES)
  		string_vacancies = TEX_INF_STRING_VACANCIES;
  	else if (string_vacancies > TEX_SUP_STRING_VACANCIES)
  		string_vacancies = TEX_SUP_STRING_VACANCIES;
  		
  	if (pool_free < TEX_INF_POOL_FREE)
  		pool_free = TEX_INF_POOL_FREE;
  	else if (pool_free > TEX_SUP_POOL_FREE)
  		pool_free = TEX_SUP_POOL_FREE;
  		
  	if (max_strings < TEX_INF_MAX_STRINGS)
  		max_strings = TEX_INF_MAX_STRINGS;
  	else if (max_strings > TEX_SUP_MAX_STRINGS)
  		max_strings = TEX_SUP_MAX_STRINGS;
  		
  	if (strings_free < TEX_INF_STRINGS_FREE)
  		strings_free = TEX_INF_STRINGS_FREE;
  	else if (strings_free > TEX_SUP_STRINGS_FREE)
  		strings_free = TEX_SUP_STRINGS_FREE;
	
	buffer = (qint32 *)malloc((buf_size + 1) * sizeof(qint32));
	first  = buf_size;
	do
	{
		buffer[first] = 0;
		first--;
	} while (first != 0);	

	buffered_cs = (qint32 *)malloc((250) * sizeof(qint32));
	
	nest        = (TeXListStateRecord*)malloc((nest_size + 1) * sizeof(TeXListStateRecord));
	save_stack  = (TeXMemoryWord*)malloc((save_size + 1) * sizeof(TeXMemoryWord));
	input_stack = (TeXInStateRecord*)malloc((stack_size + 1) * sizeof(TeXInStateRecord));
	input_file  = (QIODevice**)malloc((max_in_open + 1) * sizeof(QIODevice*));
	for (qint32 i = 0; i <= max_in_open; i++)
	{
		input_file[i] = 0;
	}
		
	input_file_mode        = (qint32*)malloc((max_in_open + 1) * sizeof(qint32));
	input_file_translation = (qint32*)malloc((max_in_open + 1) * sizeof(qint32));
	line_stack             = (qint32*)malloc((max_in_open + 1) * sizeof(qint32));
	eof_seen               = (bool*)malloc((max_in_open + 1) * sizeof(bool));
	grp_stack              = (qint32*)malloc((max_in_open + 1) * sizeof(qint32));
	if_stack               = (qint32*)malloc((max_in_open + 1) * sizeof(qint32));
	source_filename_stack  = (qint32*)malloc((max_in_open + 1) * sizeof(qint32));
	full_source_filename_stack = (qint32*)malloc((max_in_open + 1) * sizeof(qint32));
	
	param_stack = (qint32 *)malloc((param_size + 1) * sizeof(qint32));
	
	hyph_word = (qint32 *)malloc((hyph_size + 1) * sizeof(qint32));
	hyph_list = (qint32 *)malloc((hyph_size + 1) * sizeof(qint32));
	hyph_link = (qint32 *)malloc((hyph_size + 1) * sizeof(qint32));
	
	ocp_list_info   = (TeXMemoryWord*)malloc((ocp_list_size + 1) * sizeof(TeXMemoryWord));
	ocp_lstack_info = (TeXMemoryWord*)malloc((ocp_list_size + 1) * sizeof(TeXMemoryWord));
	ocp_list_list   = (qint32 *)malloc((ocp_list_size + 1) * sizeof(qint32));
	
	otp_init_input_buf = (qint32 *)malloc((ocp_buf_size + 1) * sizeof(qint32));
	otp_input_buf      = (qint32 *)malloc((ocp_buf_size + 1) * sizeof(qint32));
	otp_output_buf     = (qint32 *)malloc((ocp_buf_size + 1) * sizeof(qint32));
	
	otp_stack_buf = (qint32 *)malloc((ocp_stack_size + 1) * sizeof(qint32));
	otp_calcs     = (qint32 *)malloc((ocp_stack_size + 1) * sizeof(qint32));
	otp_states    = (qint32 *)malloc((ocp_stack_size + 1) * sizeof(qint32));
	
	if (ini_version)
	{
		yzmem = (TeXMemoryWord*)malloc((mem_top - mem_bot + 1) * sizeof(TeXMemoryWord));
		zmem = yzmem - mem_bot;
		mem = zmem;
		str_start_ar = (qint32 *)malloc((max_strings - TEX_BIGGEST_CHAR + 1) * sizeof(qint32));
  		str_pool     = (qint32 *)malloc(pool_size * sizeof(qint32));
  		
#ifdef XW_TEX_DEBUG
		mfree = (bool*)malloc((mem_max - mem_min + 2) * sizeof(bool));
		was_free = (bool*)malloc((mem_max - mem_min + 2) * sizeof(bool));
#endif //XW_TEX_DEBUG
  		pool_ptr = 0;
		str_ptr = TEX_TOO_BIG_CHAR;
		str_start_ar[0] = 0; 
		str_start_ar[1] = 0;
		
		trie = (TeXTwoHalves*)malloc((trie_size + 1) * sizeof(TeXTwoHalves));
		trie_c = (qint32*)malloc((trie_size + 1) * sizeof(qint32));
  	trie_o = (qint32*)malloc((trie_size + 1) * sizeof(qint32));
  	trie_l = (qint32*)malloc((trie_size + 1) * sizeof(qint32));
  	trie_r = (qint32*)malloc((trie_size + 1) * sizeof(qint32));
  	trie_hash = (qint32*)malloc((trie_size + 1) * sizeof(qint32));
  	trie_taken = (bool*)malloc((trie_size + 1) * sizeof(bool));
  	trie_min = (qint32*)malloc((TEX_TOO_BIG_CHAR + 1) * sizeof(qint32));
  	trie_op_ptr = 0;
  	trie_l[0 ]= 0 ;
  	trie_c[0 ]= 0 ;
  	trie_ptr = 0 ;
  	trie_r[0 ]= 0 ;
  	hyph_root = 0; 
  	hyph_start = 0;
  	QString preload = setting.getPreload();
  	if (!preload.isEmpty())
  	{
  		if (fmt_file)
  		{
  			fmt_file->close();
  			delete fmt_file;
  			fmt_file = 0;
  		}
  		XWXWTexSea sea;
  		fmt_file = sea.openFile(preload, XWXWTexSea::FMT);
  	}
	}
	
	return true;
}

void XWTeX::clear()
{
	if (buffer)
	{
		free(buffer);		
		buffer = 0;
	}

	if (buffered_cs)
	{
		free(buffered_cs);
		buffered_cs = 0;
		buffered_cs_len = 0;
	}
	
	if (str_pool)
	{
		free(str_pool);
		str_pool = 0;
	}
		
	if (str_start_ar)
	{
		free(str_start_ar);
		str_start_ar = 0;
	}
		
	if (nest)
	{
		free(nest);
		nest = 0;
	}
		
	if (input_stack)
	{
		free(input_stack);
		input_stack = 0;
	}
		
	if (input_file)
	{
		for (qint32 i = 0; i <= max_in_open; i++)
		{
			if (input_file[i])
			{
				input_file[i]->close();
				delete input_file[i];
				input_file[i] = 0;
			}
		}
			
		free(input_file);
		input_file = 0;
	}
		
	if (input_file_mode)
	{
		free(input_file_mode);
		input_file_mode = 0;
	}
		
	if (input_file_translation)
	{
		free(input_file_translation);
		input_file_translation = 0;
	}
		
	if (line_stack)
	{
		free(line_stack);
		line_stack = 0;
	}
		
	if (param_stack)
	{
		free(param_stack);
		param_stack = 0;
	}
		
	for (qint32 i = 0; i < TEX_HASHTABLESIZE; i++)
	{
		if (hashtable[i].p != -1)
		{
			TeXHashWord * runner = hashtable[i].ptr;
			while (runner->p != -1)
			{
				TeXHashWord * tmp = runner->ptr;
				free(runner);
				runner = tmp;
			}
			free(runner);
		}
		
		hashtable[i].p = -1;
	}

#ifdef XW_TEX_DEBUG
	if (mfree)
	{
		free(mfree);
		mfree = 0;
	}
	if (was_free)
	{
		free(was_free);
		was_free = 0;
	}
#endif //XW_TEX_DEBUG
	
	if (yzmem)
	{
		free(yzmem);
		yzmem = 0;
	}
		
	if (fonttables)
	{
		for (qint32 i = 0; i < font_entries; i++)
		{
			if (fonttables[i])
				free(fonttables[i]);
		}
		
		free(fonttables);
		fonttables = 0;
		font_entries = 0;
	}
	
	if (ocptables)
	{
		for (qint32 i = 0; i < ocp_entries; i++)
		{
			if (ocptables[i])
				free(ocptables[i]);
		}
		
		free(ocptables);
		ocptables = 0;
		ocp_entries = 0;
	}
	
	
	if (ocp_list_info)
	{
		free(ocp_list_info);
		ocp_list_info = 0;
	}
		
	if (ocp_lstack_info)
	{
		free(ocp_lstack_info);
		ocp_lstack_info = 0;
	}
		
	if (ocp_list_list)
	{
		free(ocp_list_list);
		ocp_list_list = 0;
	}
	
	if (trie)
	{
		free(trie);
		trie = 0;
	}
	
	if (hyph_word)
	{
		free(hyph_word);
		hyph_word = 0;
	}
	
	if (hyph_list)
	{
		free(hyph_list);
		hyph_list = 0;
	}
	
	if (trie_c)
	{
		free(trie_c);
		trie_c = 0;
	}
	
	if (trie_o)
	{
		free(trie_o);
		trie_o = 0;
	}
	
	if (trie_l)
	{
		free(trie_l);
		trie_l = 0;
	}
	
	if (trie_r)
	{
		free(trie_r);
		trie_r = 0;
	}
	
	if (trie_hash)
	{
		free(trie_hash);
		trie_hash = 0;
	}
	
	if (trie_taken)
	{
		free(trie_taken);
		trie_taken = 0;
	}
	
	if (trie_min)
	{
		free(trie_min);
		trie_min = 0;
	}
	
	if (eof_seen)
	{
		free(eof_seen);
		eof_seen = 0;
	}
	
	active_min_ptr = 0;
	active_max_ptr = 0;
}

bool XWTeX::eTeXEnabled(bool b, qint32 j, qint32 k)
{
	if (!b)
	{
		printErr("Improper "); 
		printCmdChr((quint16)j,k);
  		help1(tr("Sorry, this optional e-TeX feature has been disabled.")); 
  		error();
	}
	
	return b;
}

void XWTeX::initEqtbTable()
{
	for (qint32 i = 0; i < TEX_HASHTABLESIZE; i++) 
      	hashtable[i].p = -1;
}

void XWTeX::primitive(int s, quint16 c, qint32 o)
{
	if (s <= TEX_BIGGEST_CHAR)
		cur_val = s + TEX_SINGLE_BASE;
	else
	{
		qint32 k = str_start(s); 
		qint32 l = str_start(s + 1) - k;
		if ((first + l) > (buf_size + 1))
		{
			bufferOverFlow();			
			return ;
		}
		
		for (qint32 j = 0; j < l; j++)
			buffer[first + j] = str_pool[k + j];
			
		cur_val = idLookup(first, l);
		flushString(); 
		setText(cur_val, s);
	}
	
	setEqLevel(cur_val, TEX_LEVEL_ONE); 
	setEqType(cur_val, c);
	setEquiv(cur_val, o);
}
