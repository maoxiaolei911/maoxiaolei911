/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <QDateTime>
#include <QTextStream>
#include "XWApplication.h"
#include "XWTexSea.h"
#include "XWTeXIODev.h"
#include "XWTeX.h"

void XWTeX::alterAux()
{
	qint32 c;
	if (cur_chr != qAbs(mode))
		reportIllegalCase();
	else
	{
		c = cur_chr; 
		scanOptionalEquals();
		if (c == TEX_VMODE)
		{
			scanNormalDimen(); 
			prev_depth = cur_val;
		}
		else
		{
			scanInt();
			if ((cur_val <= 0) || (cur_val > 32767))
			{
				printErr(tr("Bad space factor"));
				help1(tr("I allow only values in the range 1..32767 here."));
      			intError(cur_val);
			}
			else
				space_factor = cur_val;
		}
	}
}

void XWTeX::alterBoxDimen()
{
	char c = (char)cur_chr; 
	scanEightBitInt(); 
	qint32 b = cur_val; 
	scanOptionalEquals();
	scanNormalDimen();
	if (box(b) != TEX_NULL)
		mem[box(b) + c].ii.CINT0 = cur_val;
}

void XWTeX::alterInteger()
{
	char c = (char)cur_chr; 
	scanOptionalEquals(); 
	scanInt();
	if (c == 0)
		dead_cycles = cur_val;
	else if (c == 2)
	{
		if ((cur_val < TEX_BATCH_MODE) || (cur_val > TEX_ERROR_STOP_MODE))
		{
			printErr(tr("Bad interaction mode"));
			help2(tr("Modes are 0=batch, 1=nonstop, 2=scroll, and"));
			help_line[0] = tr("3=errorstop. Proceed, and I'll ignore this case.");
			intError(cur_val);
		}
		else
		{
			cur_chr = cur_val; 
			newInteraction();
		}
	}
	else
		insert_penalties = cur_val;
}

void XWTeX::alterPageSoFar()
{
	char c = (char)cur_chr; 
	scanOptionalEquals(); 
	scanNormalDimen();
	page_so_far[c] = cur_val;
}

void  XWTeX::alterPrevGraf()
{
	qint32 p;
	
	nest[nest_ptr] = cur_list; 
	p = nest_ptr;
	while (qAbs(nest[p].mode_field) != TEX_VMODE)
		p--;
		
	scanOptionalEquals(); 
	scanInt();
	if (cur_val < 0)
	{
		printErr(tr("Bad ")); 
		printEsc(TeXPrevGraf);
		help1(tr("I allow only nonnegative values here."));
  		intError(cur_val);
	}
	else
	{
		nest[p].pg_field = cur_val; 
		cur_list = nest[nest_ptr];
	}
}

void XWTeX::appendLocalBox(qint32 kind)
{
	save_ptr++; 
	saved(-1) = kind; 
	newSaveLevel(TEX_LOCAL_BOX_GROUP);
	scanLeftBrace(); 
	pushNest(); 
	mode = -TEX_HMODE; 
	space_factor = 1000;
}

void XWTeX::appendSrcSpecial()
{
	if ((source_filename_stack[in_open] > 0) && 
		isNewSource(source_filename_stack[in_open], line))
	{
		newWhatsIt(TEX_SPECIAL_NODE, TEX_WRITE_NODE_SIZE);
		write_stream(tail) = TEX_NULL;
    	def_ref = getAvail();
    	token_ref_count(def_ref) = TEX_NULL;
    	strToks(makeSrcSpecial(source_filename_stack[in_open], line));
    	link(def_ref) = link(temp_head);
    	write_tokens(tail) = def_ref;
    	rememberSourceInfo(source_filename_stack[in_open], line);
	}
}

void XWTeX::beginInsertOrAdjust()
{
	if (cur_cmd == TEX_VADJUST)
		cur_val = 255;
	else
	{
		scanEightBitInt();
		if (cur_val == 255)
		{
			printErr(tr("You can't ")); 
			printEsc(TeXInsert); 
			printInt(255);
			help1(tr("I'm changing to \\insert0; box 255 is special."));
    		error(); 
    		cur_val = 0;
		}
	}
	
	saved(0) = cur_val; 
	save_ptr++;
	newSaveLevel(TEX_INSERT_GROUP); 
	scanLeftBrace(); 
	normalParagraph();
	pushNest(); 
	mode = -TEX_VMODE; 
	prev_depth = TEX_IGNORE_DEPTH;
}

void XWTeX::buildChoices()
{
	unsave();
	save_ptr--;
	flushNodeList(text_dir_ptr);
	text_dir_ptr = saved(0);
	qint32 p = finMList(TEX_NULL);
	switch (saved(-1))
	{
		case 0:
			display_mlist(tail) = p;
			break;
			
		case 1:
			text_mlist(tail) = p;
			break;
			
		case 2:
			script_mlist(tail) = p;
			break;
			
		case 3:
			script_script_mlist(tail) = p; 
			save_ptr--; 
			return;
			break;
			
		default:
			break;
	}
	
	(saved(-1))++; 
	pushMath(TEX_MATH_CHOICE_GROUP); 
	scanLeftBrace();
}

void XWTeX::buildDiscretionary()
{
	qint32 p, q, n;
	
	unsave();
	q = head; 
	p = link(q); 
	n = 0;
	
	while (p != TEX_NULL)
	{
		if (!isCharNode(p))
		{
			if (type(p) > (quint16)TEX_RULE_NODE)
			{
				if (type(p) != (quint16)TEX_KERN_NODE)
				{
					if (type(p) != (quint16)TEX_LIGATURE_NODE)
					{
						printErr(tr("Improper discretionary list"));
						help1(tr("Discretionary lists must contain only boxes and kerns."));
      					error();
      					beginDiagnostic();
      					printnl(tr("The following discretionary sublist has been deleted:"));
      					showBox(p);
      					endDiagnostic(true);
      					flushNodeList(p); 
      					link(q) = TEX_NULL; 
      					goto done;
					}
				}
			}
		}
		
		q = p; 
		p = link(q); 
		n++;
	}
	
done:
	p = link(head); 
	popNest();
	
	switch (saved(-1))
	{
		case 0:
			pre_break(tail) = p;
			break;
			
		case 1:
			post_break(tail) = p;
			break;
			
		case 2:
			if ((n > 0) && (qAbs(mode) == TEX_MMODE))
			{
				printErr(tr("Illegal math ")); 
				printEsc(TeXDiscretionary);
				help2(tr("Sorry: The third part of a discretionary break must be"));
				help_line[0] = tr("empty, in math formulas. I had to delete your third part.");
				flushNodeList(p); 
				n = 0; 
				error();
			}
			else
				link(tail) = p;
				
			if (n <= TEX_MAX_QUARTERWORD)
				replace_count(tail) = (quint16)n;
			else
			{
				printErr(tr("Discretionary list is too long"));
				help2(tr("Wow---I never thought anybody would tweak me here."));
				help_line[0] = tr("You can't seriously need such a huge discretionary list?");
				error();
			}
			if (n > 0)
				tail = q;
			save_ptr--;
			return ;
			break;
	}
	
	(saved(-1))++; 
	newSaveLevel(TEX_DISC_GROUP); 
	scanLeftBrace();
	pushNest(); 
	mode = -TEX_HMODE; 
	space_factor = 1000;
}

void XWTeX::buildLocalBox()
{
	qint32 p, kind;
	
	unsave();
	kind = saved(-1);
	save_ptr--;
	p = link(head); 
	popNest();
	p = hpack(p, 0, TEX_ADDITIONAL);
	if (kind == 0)
		eqDefine(TEX_LOCAL_LEFT_BOX_BASE, (quint16)TEX_BOX_REF, p);
	else
		eqDefine(TEX_LOCAL_RIGHT_BOX_BASE, (quint16)TEX_BOX_REF, p);
		
	if (qAbs(mode) == TEX_HMODE)
		tailAppend(makeLocalParNode());
		
	eqWordDefine(TEX_INT_BASE + TEX_NO_LOCAL_WHATSITS_CODE, nolocalWhatsits() + 1);
}

void XWTeX::doAssignments()
{
	while (true)
	{
		do
		{
			getXToken();
		} while ((cur_cmd == TEX_SPACER) || (cur_cmd == TEX_RELAX));
		
		if (cur_cmd <= TEX_MAX_NON_PREFIXED_COMMAND)
			return ;
			
		set_box_allowed = false; 
		prefixedCommand(); 
		set_box_allowed = true;
	}
}

void XWTeX::doCharMode()
{
	qint32 fileref = 0;
	qint32 moderef = 0;
	qint32 kind    = cur_chr;
	if ((kind % 4) <= 1)
	{
		scanFileReferrent();
  		fileref = cur_val;
	}
	
	if (kind < 4)
	{
		scanMode();
  		moderef = cur_val;
	}
	
	switch (kind % 4)
	{
		case TEX_TRANS_INPUT:
			if ((fileref >= 0) && (fileref <= 15))
				read_file_mode[fileref] = moderef;
			else if (fileref != TEX_MAX_HALFWORD)
				term_in_mode = moderef;
			else
			{
				base_ptr = input_ptr; 
				input_stack[base_ptr] = cur_input;
				while (state == TEX_TOKEN_LIST)
				{
					base_ptr--;
        	cur_input = input_stack[base_ptr];
				}
				
				if (texname > 17)
					input_file_mode[index] = moderef;
				cur_input = input_stack[input_ptr];
			}
			break;
			
		case TEX_TRANS_OUTPUT:
			if ((fileref >= 0) && (fileref <= 15))
				write_file_mode[fileref] = moderef;
			else
				term_out_mode = moderef;
			break;
			
		case TEX_TRANS_DEFAULT_INPUT:
			geqDefine(TEX_OCP_INPUT_MODE_BASE, (quint16)TEX_DATA, moderef);
			break;
			
		case TEX_TRANS_DEFAULT_OUTPUT:
			geqDefine(TEX_OCP_OUTPUT_MODE_BASE, (quint16)TEX_DATA, moderef);
			break;
			
		default:
			break;
	}
}

void XWTeX::doCharTranslation()
{
	qint32 fileref = 0;
	qint32 moderef = 0;
	qint32 ocpref = 0;
	qint32 kind = cur_chr;
	if ((kind % 4) <= 1)
	{
		scanFileReferrent();
  		fileref = cur_val;
	}
	else
	{
		scanMode();
  		moderef = cur_val;
	}
	
	if (kind < 4)
	{
		scanOcpIdent();
  		ocpref = cur_val;
	}
	
	switch (kind % 4)
	{
		case TEX_TRANS_INPUT:
			if ((fileref >= 0) && (fileref <= 15))
				read_file_translation[fileref] = ocpref;
			else if (fileref != TEX_MAX_HALFWORD)
				term_in_translation = ocpref;
			else
			{
				base_ptr = input_ptr; 
				input_stack[base_ptr] = cur_input;
				while (state == TEX_TOKEN_LIST)
				{
					base_ptr--;
        	cur_input = input_stack[base_ptr];
				}
				
				if (texname > 17)
					input_file_translation[index] = ocpref;
				cur_input = input_stack[input_ptr];
			}
			break;
			
		case TEX_TRANS_OUTPUT:
			if ((fileref >= 0) && (fileref <= 15))
				write_file_translation[fileref] = ocpref;
			else
				term_out_translation = ocpref;
			break;
			
		case TEX_TRANS_DEFAULT_INPUT:
			geqDefine(TEX_OCP_INPUT_MODE_BASE + moderef, (quint16)TEX_DATA, ocpref);
			break;
			
		case TEX_TRANS_DEFAULT_OUTPUT:
			geqDefine(TEX_OCP_OUTPUT_MODE_BASE + moderef, (quint16)TEX_DATA, ocpref);
			break;
			
		default:
			break;
	}
}

void  XWTeX::doEndV()
{
	base_ptr = input_ptr; 
	input_stack[base_ptr] = cur_input;
	while ((input_stack[base_ptr].index_field != TEX_V_TEMPLATE) && 
           (input_stack[base_ptr].loc_field == TEX_NULL) && 
           (input_stack[base_ptr].state_field == TEX_TOKEN_LIST))
    {
    	base_ptr--;
    }
    
    if ((input_stack[base_ptr].index_field != TEX_V_TEMPLATE) || 
        (input_stack[base_ptr].loc_field != TEX_NULL) || 
        (input_stack[base_ptr].state_field != TEX_TOKEN_LIST))
    {
    	fatalError(tr("(interwoven alignment preambles are not allowed)"));
    	return ;
    }
    
    if (cur_group == TEX_ALIGN_GROUP)
    {
    	endGraf();
    	if (finCol())
    		finRow();
    }
    else
    	offSave();
}

void XWTeX::doRegisterCommand(quint16 a)
{
	qint32 l, q, r, s, p;
	
	q = cur_cmd;
	if (q != TEX_REGISTER)
	{
		getXToken();
		if ((cur_cmd >= TEX_ASSIGN_INT) && (cur_cmd <= TEX_ASSIGN_MU_GLUE))
		{
			l = cur_chr; 
			p = cur_cmd - TEX_ASSIGN_INT; 
			goto found;
		}
		
		if (cur_cmd != TEX_REGISTER)
		{
			printErr(tr("You can't use `")); 
			printCmdChr((quint16)cur_cmd, cur_chr);
			print("' after "); 
			printCmdChr((quint16)q,0);
    		help1(tr("I'm forgetting what you said and not changing anything."));
    		error(); 
    		return;
		}
	}
	
	p = cur_chr; 
	scanEightBitInt();
	switch (p)
	{
		case TEX_INT_VAL: 
			l = cur_val + TEX_COUNT_BASE;
			break;
			
		case TEX_DIMEN_VAL: 
			l = cur_val + TEX_SCALED_BASE;
			break;
			
		case TEX_GLUE_VAL: 
			l = cur_val + TEX_SKIP_BASE;
			break;
			
		case TEX_MU_VAL: 
			l = cur_val + TEX_MU_SKIP_BASE;
			break;
			
		default:
			break;
	}
	
found:
	if (q == TEX_REGISTER)
		scanOptionalEquals();
	else if (scanKeyWord(TeXBy))
		;
	arith_error = false;
	
	if (q < TEX_MULTIPLY)
	{
		if (p < TEX_GLUE_VAL)
		{
			if (p == TEX_INT_VAL)
				scanInt();
			else
				scanNormalDimen();
				
			if (q == TEX_ADVANCE)
				cur_val = cur_val + newEqtbInt(l);
		}
		else 
		{
			scanGlue(p);
			if (q == TEX_ADVANCE)
			{
				q = newSpec(cur_val); 
				r = equiv(l);
				deleteGlueRef(cur_val);
				width(q) = width(q) + width(r);
				if (stretch(q) == 0)
					stretch_order(q) = (quint16)TEX_NORMAL;
				if (stretch_order(q) == stretch_order(r))
					stretch(q) = stretch(q) + stretch(r);
				else if ((stretch_order(q) < stretch_order(r)) && (stretch(r) != 0))
				{
					stretch(q) = stretch(r); 
					stretch_order(q) = stretch_order(r);
				}
				
				if (shrink(q) == 0)
					shrink_order(q) = (quint16)TEX_NORMAL;
				if (shrink_order(q) == shrink_order(r))
					shrink(q) = shrink(q) + shrink(r);
				else if ((shrink_order(q) < shrink_order(r)) && (shrink(r) != 0))
				{
					shrink(q) = shrink(r); 
					shrink_order(q) = shrink_order(r);
				}
				
				cur_val = q;
			}
		}
	}
	else
	{
		scanInt();
		if (p < TEX_GLUE_VAL)
		{
			if (q == TEX_MULTIPLY)
			{
				if (p == TEX_INT_VAL)
					cur_val = multIntegers(newEqtbInt(l), cur_val);
				else
					cur_val = nxPlusY(newEqtbInt(l), cur_val, 0);
			}
			else
				cur_val = xOverN(newEqtbInt(l), cur_val);
		}
		else
		{
			s = equiv(l); 
			r = newSpec(s);
			if (q == TEX_MULTIPLY)
			{
				width(r) = nxPlusY(width(s), cur_val, 0);
    			stretch(r) = nxPlusY(stretch(s), cur_val, 0);
    			shrink(r) = nxPlusY(shrink(s), cur_val, 0);
			}
			else
			{
				width(r) = xOverN(width(s), cur_val);
    			stretch(r) = xOverN(stretch(s), cur_val);
    			shrink(r) = xOverN(shrink(s), cur_val);
			}
			
			cur_val = r;
		}
	}
	
	if (arith_error)
	{
		printErr(tr("Arithmetic overflow"));
		help2(tr("I can't carry out that multiplication or division,"));
		help_line[0] = tr("since the result is out of range.");
		error(); 
		return;
	}
	
	if (p < TEX_GLUE_VAL)
	{
		if (a >= 4)
			geqWordDefine(l, cur_val);
		else
			eqWordDefine(l, cur_val);
	}
	else
	{
		trapZeroGlue();
		if (a >= 4)
			geqDefine(l, (quint16)TEX_GLUE_REF, cur_val);
		else
			eqDefine(l, (quint16)TEX_GLUE_REF, cur_val);
	}
}

void XWTeX::endGraf()
{
	if (mode == TEX_HMODE)
	{
		if ((head == tail) || (link(head) == tail))
			popNest();
		else
			lineBreak(widowPenalty());
			
		if (dir_save != TEX_NULL)
		{
			flushNodeList(dir_save); 
			dir_save = TEX_NULL;
		}
		
		normalParagraph();
  		error_count = 0;
	}
}

void XWTeX::handleRightBrace()
{
	qint32 p, q, d, f;
	
	switch (cur_group)
	{
		case TEX_SIMPLE_GROUP:
			temp_no_whatsits = nolocalWhatsits();
  			temp_no_dirs  = nolocalDirs();
  			temporary_dir = textDirection();
  			if (dir_level(text_dir_ptr) == cur_level)
  			{
  				text_dir_tmp = link(text_dir_ptr);
				freeNode(text_dir_ptr, TEX_DIR_NODE_SIZE);
				text_dir_ptr = text_dir_tmp;
  			}
  			unsave();
  			if (qAbs(mode) == TEX_HMODE)
  			{
  				if (temp_no_dirs != 0)
  				{
  					tailAppend(newDir(textDirection()));
  					dir_dir(tail) = temporary_dir - 64;
  				}
  				
  				if (temp_no_whatsits != 0)
  					tailAppend(makeLocalParNode());
  			}
			break;
			
		case TEX_BOTTOM_LEVEL:
			printErr(tr("Too many }'s"));
			help2(tr("You've closed more groups than you opened."));
			help_line[0] = tr("Such booboos are generally harmless, so keep going.");
			error();
			break;
			
		case TEX_SEMI_SIMPLE_GROUP:
		case TEX_MATH_SHIFT_GROUP:
		case TEX_MATH_LEFT_GROUP: 
			extraRightBrace();
			break;
			
		case TEX_ALIGN_GROUP:
			backInput(); 
			cur_tok = TEX_CS_TOKEN_FLAG + TEX_FROZEN_CR;
			printErr(tr("Missing ")); 
			printEsc(TeXCr); 
			print(tr(" inserted"));
			help1(tr("I'm guessing that you meant to end an alignment here."));
  			insError();
			break;
			
		case TEX_NO_ALIGN_GROUP:
			endGraf(); 
			unsave(); 
			alignPeek();
			break;
			
		case TEX_MATH_CHOICE_GROUP:
			buildChoices();
			break;
			
		case TEX_HBOX_GROUP:
			package(0);
			break;
			
		case TEX_ADJUSTED_HBOX_GROUP:
			adjust_tail = adjust_head; 
			package(0);
			break;
			
		case TEX_VBOX_GROUP:
			endGraf(); 
			package(0);
			break;
			
		case TEX_VTOP_GROUP:
			endGraf(); 
			package(TEX_VTOP_CODE);
			break;
			
		case TEX_INSERT_GROUP:
			endGraf(); 
			q = splitTopSkip(); 
			add_glue_ref(q);
  			d = splitMaxDepth(); 
  			f = floatingPenalty(); 
  			unsave(); 
  			save_ptr--;
  			p = vpack(link(head), 0, TEX_ADDITIONAL); 
  			popNest();
  			if (saved(0) != 255)
  			{
  				tailAppend(getNode(TEX_INS_NODE_SIZE));
    			type(tail) = (quint16)TEX_INS_NODE; 
    			subtype(tail) = (quint16)(saved(0));
    			height(tail) = height(p) + depth(p); 
    			ins_ptr(tail) = list_ptr(p);
    			split_top_ptr(tail) = q; 
    			depth(tail) = d; 
    			float_cost(tail) = f;
  			}
  			else
  			{
  				tailAppend(getNode(TEX_SMALL_NODE_SIZE));
    			type(tail) = (quint16)TEX_ADJUST_NODE;
    			subtype(tail) = (quint16)0;
    			adjust_ptr(tail) = list_ptr(p); 
    			deleteGlueRef(q);
  			}
  			freeNode(p, TEX_BOX_NODE_SIZE);
  			if (nest_ptr == 0)
  				buildPage();
			break;
			
		case TEX_OUTPUT_GROUP:
			if ((loc != TEX_NULL) || 
				((token_type != TEX_OUTPUT_TEXT) && (token_type != TEX_BACKED_UP)))
			{
				printErr(tr("Unbalanced output routine"));
				help2(tr("Your sneaky output routine has problematic {'s and/or }'s."));
				help_line[0] = tr("I can't handle that very well; good luck.");
				error();
				do
				{
					getToken();
				} while (loc != TEX_NULL);
			}			
			endTokenList();
			endGraf(); 
			unsave(); 
			output_active = false; 
			insert_penalties = 0;
			if (box(255) != TEX_NULL)
			{
				printErr(tr("Output routine didn't use all of "));
  				printEsc(TeXBox); 
  				printInt(255);
  				help3(tr("Your \\output commands should empty \\box255,"));
  				help_line[1] = tr("e.g., by saying `\\shipout\\box255'.");
  				help_line[0] = tr("Proceed; I'll discard its present contents.");
  				boxError(255);
			}
			if (tail != head)
			{
				link(page_tail) = link(head);
  				page_tail = tail;
			}
			if (link(page_head) != TEX_NULL)
			{
				if (link(contrib_head) == TEX_NULL)
					contrib_tail = page_tail;
					
				link(page_tail) = link(contrib_head);
  				link(contrib_head) = link(page_head);
  				link(page_head) = TEX_NULL; 
  				page_tail = page_head;
			}
			flushNodeList(page_disc); 
			page_disc = TEX_NULL;
			popNest(); 
			buildPage();
			break;
			
		case TEX_DISC_GROUP: 
			buildDiscretionary();
			break;
			
		case TEX_LOCAL_BOX_GROUP: 
			buildLocalBox();
			break;
						
		case TEX_VCENTER_GROUP:
			endGraf(); 
			unsave(); 
			save_ptr = save_ptr - 2;
  			p = vpack(link(head), saved(1), saved(0)); 
  			popNest();
  			tailAppend(newNoad()); 
  			type(tail) = (quint16)TEX_VCENTER_NOAD;
  			math_type(nucleus(tail)) = TEX_SUB_BOX; 
  			info(nucleus(tail)) = p;
			break;
			
		case TEX_MATH_GROUP: 
			unsave();
			save_ptr--;
			flushNodeList(text_dir_ptr);
			text_dir_ptr = saved(0);
			save_ptr--;
			math_type(saved(0)) = TEX_SUB_MLIST; 
			p = finMList(TEX_NULL); 
			info(saved(0)) = p;
			if (p != TEX_NULL)
			{
				if (link(p) == TEX_NULL)
				{
					if (type(p) == (quint16)TEX_ORD_NOAD)
					{
						if (math_type(subscr(p)) == TEX_EMPTY)
						{
							if (math_type(supscr(p)) == TEX_EMPTY)
							{
								mem[saved(0)].hh = mem[nucleus(p)].hh;
      							freeNode(p, TEX_NOAD_SIZE);
							}
						}
					}
					else if (type(p) == (quint16)TEX_ACCENT_NOAD)
					{
						if (saved(0) == nucleus(tail))
						{
							if (type(tail) == (quint16)TEX_ORD_NOAD)
							{
								q = head; 
								while (link(q) != tail) 
									q = link(q);
								link(q) = p; 
								freeNode(tail, TEX_NOAD_SIZE); 
								tail = p;
							}
						}
					}
				}
			}			
			break;
			
		default:
			confusion("rightbrace");
			break;
	}
}

void XWTeX::insertSrcSpecial()
{
	if ((source_filename_stack[in_open] > 0 && isNewSource(source_filename_stack[in_open], line)))
	{
		qint32 toklist = getAvail();
    	qint32 p = toklist;
    	info(p)  = special_token;
    	link(p) = getAvail(); 
    	p = link(p);
    	info(p) = TEX_LEFT_BRACE_TOKEN + '{';
    	qint32 q = strToks(makeSrcSpecial(source_filename_stack[in_open], line));
    	link(p) = link(temp_head);
    	p = q;
    	link(p) = getAvail(); 
    	p = link(p);
    	info(p) = TEX_RIGHT_BRACE_TOKEN + '}';
    	insList(toklist);
    	rememberSourceInfo(source_filename_stack[in_open], line);
	}
}

bool XWTeX::itsAllOver()
{
	if (privileged())
	{
		if ((page_head == page_tail) && 
			(head == tail) && 
			(dead_cycles == 0))
		{
			return true;
		}
		
		backInput();
		
		tailAppend(newNullBox());
  		width(tail) = hsize();
  		tailAppend(newGlue(fill_glue));
  		tailAppend(newPenalty(-0x40000000));
  		buildPage();
	}
	
	return false;
}

void XWTeX::mainControl()
{
	qint32 t, k;
	if (everyJob() != TEX_NULL)
		beginTokenList(everyJob(), TEX_EVERY_JOB_TEXT);
		
big_switch: 
	getXToken();
	
reswitch: 
	
	if (history == TEX_FATAL_ERROR_STOP)
		return ;
		
	if (interrupt != 0)
	{
		if (OK_to_interrupt)
		{
			backInput(); 
			checkInterrupt(); 
			goto big_switch;
		}
	}
	
#ifdef XW_TEX_DEBUG

	if (panicking)
		checkMem(false);
#endif //XW_TEX_DEBUG

	if (tracingCommands() > 0)
		showCurCmdChr();
		
	switch (qAbs(mode) + cur_cmd)
	{
		case TEX_HMODE + TEX_LETTER:
		case TEX_HMODE + TEX_OTHER_CHAR:
		case TEX_HMODE + TEX_CHAR_GIVEN:
			goto main_loop;
			break;
			
		case TEX_HMODE + TEX_CHAR_NUM:
			scanCharNum(); 
			cur_chr = cur_val; 
			goto main_loop;
			break;
			
		case TEX_HMODE + TEX_NO_BOUNDARY:
			getXToken();
			if ((cur_cmd == TEX_LETTER) || 
				(cur_cmd == TEX_OTHER_CHAR) || 
				(cur_cmd == TEX_CHAR_GIVEN) || 
				(cur_cmd == TEX_CHAR_NUM))
			{
				cancel_boundary = true;
			}
			goto reswitch;
			break;
			
		case TEX_HMODE + TEX_CHAR_GHOST:
			t = cur_chr; 
			getXToken();
			if ((cur_cmd == TEX_LETTER) || 
				(cur_cmd == TEX_OTHER_CHAR) || 
				(cur_cmd == TEX_CHAR_GIVEN) || 
				(cur_cmd == TEX_CHAR_NUM))
			{
				if (t == 0)
					new_left_ghost = true;
				else
					new_right_ghost = true;
			}
			goto reswitch;
			break;
			
		case TEX_HMODE + TEX_SPACER:
			if (space_factor == 1000)
				goto append_normal_space;
			else
				appSpace();
			break;
			
		case TEX_HMODE + TEX_EX_SPACE:
		case TEX_MMODE + TEX_EX_SPACE:
			goto append_normal_space;
			break;
			
		case TEX_VMODE + TEX_RELAX:
		case TEX_HMODE + TEX_RELAX:
		case TEX_MMODE + TEX_RELAX:
		case TEX_VMODE + TEX_SPACER:
		case TEX_MMODE + TEX_SPACER:
		case TEX_MMODE + TEX_NO_BOUNDARY:
			break;
			
		case TEX_VMODE + TEX_IGNORE_SPACES:
		case TEX_HMODE + TEX_IGNORE_SPACES:
		case TEX_MMODE + TEX_IGNORE_SPACES:
			do
			{
				getXToken();
			} while (cur_cmd == TEX_SPACER);
			goto reswitch;
			break;
			
		case TEX_VMODE + TEX_STOP:
			if (itsAllOver())
			{
				return ;
			}
			break;
			
		case TEX_VMODE + TEX_VMOVE:
		case TEX_HMODE + TEX_HMOVE:
		case TEX_MMODE + TEX_HMOVE:
		case TEX_VMODE + TEX_LAST_ITEM:
		case TEX_HMODE + TEX_LAST_ITEM:
		case TEX_MMODE + TEX_LAST_ITEM:
		case TEX_VMODE + TEX_VADJUST:
		case TEX_VMODE + TEX_ITAL_CORR:
		case TEX_VMODE + TEX_EQ_NO:
		case TEX_HMODE + TEX_EQ_NO:
		case TEX_VMODE + TEX_MAC_PARAM:
		case TEX_HMODE + TEX_MAC_PARAM:
		case TEX_MMODE + TEX_MAC_PARAM:
			reportIllegalCase();
			break;
			
		case TEX_VMODE + TEX_SUP_MARK:
		case TEX_HMODE + TEX_SUP_MARK:
		case TEX_VMODE + TEX_SUB_MARK:
		case TEX_HMODE + TEX_SUB_MARK:
		case TEX_VMODE + TEX_MATH_CHAR_NUM:
		case TEX_HMODE + TEX_MATH_CHAR_NUM:
		case TEX_VMODE + TEX_MATH_GIVEN:
		case TEX_HMODE + TEX_MATH_GIVEN:
		case TEX_VMODE + TEX_OMATH_GIVEN:
		case TEX_HMODE + TEX_OMATH_GIVEN:
		case TEX_VMODE + TEX_MATH_COMP:
		case TEX_HMODE + TEX_MATH_COMP:
		case TEX_VMODE + TEX_DELIM_NUM:
		case TEX_HMODE + TEX_DELIM_NUM:
		case TEX_VMODE + TEX_LEFT_RIGHT:
		case TEX_HMODE + TEX_LEFT_RIGHT:
		case TEX_VMODE + TEX_ABOVE:
		case TEX_HMODE + TEX_ABOVE:
		case TEX_VMODE + TEX_RADICAL:
		case TEX_HMODE + TEX_RADICAL:
		case TEX_VMODE + TEX_MATH_STYLE:
		case TEX_HMODE + TEX_MATH_STYLE:
		case TEX_VMODE + TEX_MATH_CHOICE:
		case TEX_HMODE + TEX_MATH_CHOICE:
		case TEX_VMODE + TEX_VCENTER:
		case TEX_HMODE + TEX_VCENTER:
		case TEX_VMODE + TEX_NON_SCRIPT:
		case TEX_HMODE + TEX_NON_SCRIPT:
		case TEX_VMODE + TEX_MKERN:
		case TEX_HMODE + TEX_MKERN:
		case TEX_VMODE + TEX_LIMIT_SWITCH:
		case TEX_HMODE + TEX_LIMIT_SWITCH:
		case TEX_VMODE + TEX_MSKIP:
		case TEX_HMODE + TEX_MSKIP:
		case TEX_VMODE + TEX_MATH_ACCENT:
		case TEX_HMODE + TEX_MATH_ACCENT:
		case TEX_MMODE + TEX_ENDV:
		case TEX_MMODE + TEX_PAR_END:
		case TEX_MMODE + TEX_STOP:
		case TEX_MMODE + TEX_VSKIP:
		case TEX_MMODE + TEX_UN_VBOX:
		case TEX_MMODE + TEX_VALIGN:
		case TEX_MMODE + TEX_HRULE:
			insertDollarSign();
			break;
			
		case TEX_VMODE + TEX_HRULE:
		case TEX_HMODE + TEX_VRULE:
		case TEX_MMODE + TEX_VRULE:
			tailAppend(scanRuleSpec());
			if (qAbs(mode) == TEX_VMODE)
				prev_depth = TEX_IGNORE_DEPTH;
			else if (qAbs(mode) == TEX_HMODE)
				space_factor = 1000;
			break;
			
		case TEX_VMODE + TEX_VSKIP:
		case TEX_HMODE + TEX_HSKIP:
		case TEX_MMODE + TEX_HSKIP:
		case TEX_MMODE + TEX_MSKIP: 
			appendGlue();
			break;
			
		case TEX_VMODE + TEX_KERN:
		case TEX_HMODE + TEX_KERN:
		case TEX_MMODE + TEX_KERN:
		case TEX_MMODE + TEX_MKERN: 
			appendKern();
			break;
			
		case TEX_VMODE + TEX_LEFT_BRACE:
		case TEX_HMODE + TEX_LEFT_BRACE:
			newSaveLevel(TEX_SIMPLE_GROUP);
  			eqWordDefine(TEX_INT_BASE + TEX_NO_LOCAL_WHATSITS_CODE, 0);
  			eqWordDefine(TEX_INT_BASE + TEX_NO_LOCAL_DIRS_CODE, 0);
			break;
			
		case TEX_VMODE + TEX_BEGIN_GROUP:
		case TEX_HMODE + TEX_BEGIN_GROUP:
		case TEX_MMODE + TEX_BEGIN_GROUP:
			newSaveLevel(TEX_SEMI_SIMPLE_GROUP);
  			eqWordDefine(TEX_INT_BASE + TEX_NO_LOCAL_WHATSITS_CODE, 0);
  			eqWordDefine(TEX_INT_BASE + TEX_NO_LOCAL_DIRS_CODE, 0);
			break;
			
		case TEX_VMODE + TEX_END_GROUP:
		case TEX_HMODE + TEX_END_GROUP:
		case TEX_MMODE + TEX_END_GROUP:
			if (cur_group == TEX_SEMI_SIMPLE_GROUP)
			{
				temp_no_whatsits = nolocalWhatsits();
    			temp_no_dirs = nolocalDirs();
    			temporary_dir = textDirection();
    			if (dir_level(text_dir_ptr) == cur_level)
    			{
    				text_dir_tmp = link(text_dir_ptr);
					freeNode(text_dir_ptr, TEX_DIR_NODE_SIZE);
					text_dir_ptr = text_dir_tmp;
    			}
    			
    			unsave();
    			if (qAbs(mode) == TEX_HMODE)
    			{
    				if (temp_no_dirs != 0)
    				{
    					tailAppend(newDir(textDirection()));
    					dir_dir(tail) = temporary_dir - 64;
    				}
    				
    				if (temp_no_whatsits != 0)
    				{
    					tailAppend(makeLocalParNode());
    				}
    			}
			}
			else
				offSave();
			break;
			
		case TEX_VMODE + TEX_RIGHT_BRACE:
		case TEX_HMODE + TEX_RIGHT_BRACE:
		case TEX_MMODE + TEX_RIGHT_BRACE:
			handleRightBrace();
			break;
			
		case TEX_VMODE + TEX_HMOVE:
		case TEX_HMODE + TEX_VMOVE:
		case TEX_MMODE + TEX_VMOVE:
			t = cur_chr;
			scanNormalDimen();
			if (t == 0)
				scanBox(cur_val);
			else
				scanBox(-cur_val);
			break;
			
		case TEX_VMODE + TEX_LEADER_SHIP:
		case TEX_HMODE + TEX_LEADER_SHIP:
		case TEX_MMODE + TEX_LEADER_SHIP:
			scanBox(TEX_LEADER_FLAG - TEX_A_LEADERS + cur_chr);
			break;
			
		case TEX_VMODE + TEX_MAKE_BOX:
		case TEX_HMODE + TEX_MAKE_BOX:
		case TEX_MMODE + TEX_MAKE_BOX:
			beginBox(0);
			break;
			
		case TEX_VMODE + TEX_ASSIGN_BOX_DIR:
		case TEX_HMODE + TEX_ASSIGN_BOX_DIR:
		case TEX_MMODE + TEX_ASSIGN_BOX_DIR:
			scanEightBitInt();
  			cur_box = box(cur_val);
  			scanOptionalEquals(); 
  			scanDir();
  			if (cur_box != TEX_NULL)
  				box_dir(cur_box) = cur_val;
			break;
			
		case TEX_VMODE + TEX_START_PAR:
			newGraf(cur_chr > 0);
			break;
			
		case TEX_VMODE + TEX_LETTER:
		case TEX_VMODE + TEX_OTHER_CHAR:
		case TEX_VMODE + TEX_CHAR_NUM:
		case TEX_VMODE + TEX_CHAR_GIVEN:
		case TEX_VMODE + TEX_CHAR_GHOST:
   		case TEX_VMODE + TEX_MATH_SHIFT:
   		case TEX_VMODE + TEX_UN_HBOX:
   		case TEX_VMODE + TEX_VRULE:
   		case TEX_VMODE + TEX_ACCENT:
   		case TEX_VMODE + TEX_DISCRETIONARY:
   		case TEX_VMODE + TEX_HSKIP:
   		case TEX_VMODE + TEX_VALIGN:
   		case TEX_VMODE + TEX_EX_SPACE:
   		case TEX_VMODE + TEX_NO_BOUNDARY:
   			backInput(); 
   			newGraf(true);
   			break;
   			
   		case TEX_HMODE + TEX_START_PAR:
   		case TEX_MMODE + TEX_START_PAR:
   			indentInHMode();
   			break;
   			
   		case TEX_VMODE + TEX_PAR_END:
   			normalParagraph();
   			if (mode > 0)
   				buildPage();
   			break;
   			
   		case TEX_HMODE + TEX_PAR_END:
   			if (align_state < 0)
   				offSave();
   			endGraf();
   			if (mode == TEX_VMODE)
   				buildPage();
   			break;
   			
   		case TEX_HMODE + TEX_STOP:
   		case TEX_HMODE + TEX_VSKIP:
   		case TEX_HMODE + TEX_HRULE:
   		case TEX_HMODE + TEX_UN_VBOX:
   		case TEX_HMODE + TEX_HALIGN:
   			headForVMode();
   			break;
   			
   		case TEX_VMODE + TEX_INSERT:
		case TEX_HMODE + TEX_INSERT:
		case TEX_MMODE + TEX_INSERT:
		case TEX_HMODE + TEX_VADJUST:
		case TEX_MMODE + TEX_VADJUST:
			beginInsertOrAdjust();
			break;
			
		case TEX_VMODE + TEX_MARK:
		case TEX_HMODE + TEX_MARK:
		case TEX_MMODE + TEX_MARK:
			makeMark();
			break;
			
		case TEX_VMODE + TEX_BREAK_PENALTY:
		case TEX_HMODE + TEX_BREAK_PENALTY:
		case TEX_MMODE + TEX_BREAK_PENALTY:
			appendPenalty();
			break;
			
		case TEX_VMODE + TEX_REMOVE_ITEM:
		case TEX_HMODE + TEX_REMOVE_ITEM:
		case TEX_MMODE + TEX_REMOVE_ITEM:
			deleteLast();
			break;
			
		case TEX_VMODE + TEX_UN_VBOX:
		case TEX_HMODE + TEX_UN_HBOX:
		case TEX_MMODE + TEX_UN_HBOX:
			unpackage();
			break;
			
		case TEX_HMODE + TEX_ITAL_CORR: 
			appendItalicCorrection();
			break;
			
		case TEX_MMODE + TEX_ITAL_CORR: 
			tailAppend(newKern(0));
			break;
			
		case TEX_HMODE + TEX_DISCRETIONARY:
		case TEX_MMODE + TEX_DISCRETIONARY: 
			appendDiscretionary();
			break;
			
		case TEX_VMODE + TEX_ASSIGN_LOCAL_BOX:
		case TEX_HMODE + TEX_ASSIGN_LOCAL_BOX:
		case TEX_MMODE + TEX_ASSIGN_LOCAL_BOX:
			appendLocalBox(cur_chr);
			break;
			
		case TEX_HMODE + TEX_ACCENT: 
			makeAccent();
			break;
			
		case TEX_VMODE + TEX_CAR_RET:
		case TEX_HMODE + TEX_CAR_RET:
		case TEX_MMODE + TEX_CAR_RET:
		case TEX_VMODE + TEX_TAB_MARK:
		case TEX_HMODE + TEX_TAB_MARK:
		case TEX_MMODE + TEX_TAB_MARK:
			alignError();
			break;
			
		case TEX_VMODE + TEX_NO_ALIGN:
		case TEX_HMODE + TEX_NO_ALIGN:
		case TEX_MMODE + TEX_NO_ALIGN:
			noalignError();
			break;
			
		case TEX_VMODE + TEX_OMIT:
		case TEX_HMODE + TEX_OMIT:
		case TEX_MMODE + TEX_OMIT:
			omitError();
			break;
			
		case TEX_VMODE + TEX_HALIGN:
		case TEX_HMODE + TEX_VALIGN:
			initAlign();
			break;
			
		case TEX_MMODE + TEX_HALIGN:
			if (privileged())
			{
				if (cur_group == TEX_MATH_SHIFT_GROUP)
					initAlign();
				else
					offSave();
			}
			break;
			
		case TEX_VMODE + TEX_ENDV:
		case TEX_HMODE + TEX_ENDV:
			doEndV();
			break;
			
		case TEX_VMODE + TEX_END_CS_NAME:
		case TEX_HMODE + TEX_END_CS_NAME:
		case TEX_MMODE + TEX_END_CS_NAME:
			csError();
			break;
			
		case TEX_HMODE + TEX_MATH_SHIFT:
			initMath();
			break;
			
		case TEX_MMODE + TEX_EQ_NO:
			if (privileged())
			{
				if (cur_group == TEX_MATH_SHIFT_GROUP)
					startEqNo();
			}
			else
				offSave();
			break;
			
		case TEX_MMODE + TEX_LEFT_BRACE:
			tailAppend(newNoad());
  			backInput(); 
  			scanMath(nucleus(tail));
			break;
			
		case TEX_MMODE + TEX_LETTER:
		case TEX_MMODE + TEX_OTHER_CHAR:
		case TEX_MMODE + TEX_CHAR_GIVEN:
			setMathChar(mathCode(cur_chr));
			break;
			
		case TEX_MMODE + TEX_CHAR_NUM:
			scanCharNum(); 
			cur_chr = cur_val;
  			setMathChar(mathCode(cur_chr));
			break;
			
		case TEX_MMODE + TEX_MATH_CHAR_NUM:
			if (cur_chr == 0)
				scanFifteenBitInt();
			else
				scanBigFifteenBitInt();
  			setMathChar(cur_val);
			break;
			
		case TEX_MMODE + TEX_MATH_GIVEN:
			setMathChar(((cur_chr / 0x1000) * 0x1000000) + (((cur_chr % 0x1000) / 0x100) * 0x10000) + (cur_chr % 0x100));
			break;
			
		case TEX_MMODE + TEX_OMATH_GIVEN:
			setMathChar(cur_chr);
			break;
			
		case TEX_MMODE + TEX_DELIM_NUM:
			if (cur_chr == 0)
				scanTwentySevenBitInt();
			else
				scanFiftyOneBitInt();
  			setMathChar(cur_val);
			break;
			
		case TEX_MMODE + TEX_MATH_COMP:
			tailAppend(newNoad());
  			type(tail) = (quint16)cur_chr; 
  			scanMath(nucleus(tail));
			break;
			
		case TEX_MMODE + TEX_LIMIT_SWITCH: 
			mathLimitSwitch();
			break;
			
		case TEX_MMODE + TEX_RADICAL:
			mathRadical();
			break;
			
		case TEX_MMODE + TEX_ACCENT:
		case TEX_MMODE + TEX_MATH_ACCENT:
			mathAC();
			break;
			
		case TEX_MMODE + TEX_VCENTER:
			scanSpec(TEX_VCENTER_GROUP, false); 
			normalParagraph();
  			pushNest(); 
  			mode = -TEX_VMODE; 
  			prev_depth = TEX_IGNORE_DEPTH;
  			if (insert_src_special_every_vbox)
  				insertSrcSpecial();
  			if (everyVBox() != TEX_NULL)
  				beginTokenList(everyVBox(), TEX_EVERY_VBOX_TEXT);
			break;
			
		case TEX_MMODE + TEX_MATH_STYLE:
			tailAppend(newStyle(cur_chr));
			break;
			
		case TEX_MMODE + TEX_NON_SCRIPT:
			tailAppend(newGlue(zero_glue));
  			subtype(tail) = (quint16)TEX_COND_MATH_GLUE;
			break;
			
		case TEX_MMODE + TEX_MATH_CHOICE: 
			appendChoices();
			break;
			
		case TEX_MMODE + TEX_SUB_MARK:
		case TEX_MMODE + TEX_SUP_MARK: 
			subSup();
			break;
			
		case TEX_MMODE + TEX_ABOVE: 
			mathFraction();
			break;
			
		case TEX_MMODE + TEX_LEFT_RIGHT:
			mathLeftRight();
			break;
			
		case TEX_MMODE + TEX_MATH_SHIFT:
			if (cur_group == TEX_MATH_SHIFT_GROUP)
				afterMath();
			else
				offSave();
			break;
			
		case TEX_VMODE + TEX_TOKS_REGISTER:
		case TEX_HMODE + TEX_TOKS_REGISTER:
		case TEX_MMODE + TEX_TOKS_REGISTER:
		case TEX_VMODE + TEX_ASSIGN_TOKS:
		case TEX_HMODE + TEX_ASSIGN_TOKS:
		case TEX_MMODE + TEX_ASSIGN_TOKS:
		case TEX_VMODE + TEX_ASSIGN_INT:
		case TEX_HMODE + TEX_ASSIGN_INT:
		case TEX_MMODE + TEX_ASSIGN_INT:
		case TEX_VMODE + TEX_ASSIGN_DIR:
		case TEX_HMODE + TEX_ASSIGN_DIR:
		case TEX_MMODE + TEX_ASSIGN_DIR:
		case TEX_VMODE + TEX_ASSIGN_DIMEN:
		case TEX_HMODE + TEX_ASSIGN_DIMEN:
		case TEX_MMODE + TEX_ASSIGN_DIMEN:
		case TEX_VMODE + TEX_ASSIGN_GLUE:
		case TEX_HMODE + TEX_ASSIGN_GLUE:
		case TEX_MMODE + TEX_ASSIGN_GLUE:
		case TEX_VMODE + TEX_ASSIGN_MU_GLUE:
		case TEX_HMODE + TEX_ASSIGN_MU_GLUE:
		case TEX_MMODE + TEX_ASSIGN_MU_GLUE:
		case TEX_VMODE + TEX_ASSIGN_FONT_DIMEN:
		case TEX_HMODE + TEX_ASSIGN_FONT_DIMEN:
		case TEX_MMODE + TEX_ASSIGN_FONT_DIMEN:
		case TEX_VMODE + TEX_ASSIGN_FONT_INT:
		case TEX_HMODE + TEX_ASSIGN_FONT_INT:
		case TEX_MMODE + TEX_ASSIGN_FONT_INT:
		case TEX_VMODE + TEX_SET_AUX:
		case TEX_HMODE + TEX_SET_AUX:
		case TEX_MMODE + TEX_SET_AUX:
		case TEX_VMODE + TEX_SET_PREV_GRAF:
		case TEX_HMODE + TEX_SET_PREV_GRAF:
		case TEX_MMODE + TEX_SET_PREV_GRAF:
		case TEX_VMODE + TEX_SET_PAGE_DIMEN:
		case TEX_HMODE + TEX_SET_PAGE_DIMEN:
		case TEX_MMODE + TEX_SET_PAGE_DIMEN:
		case TEX_VMODE + TEX_SET_PAGE_INT:
		case TEX_HMODE + TEX_SET_PAGE_INT:
		case TEX_MMODE + TEX_SET_PAGE_INT:
		case TEX_VMODE + TEX_SET_BOX_DIMEN:
		case TEX_HMODE + TEX_SET_BOX_DIMEN:
		case TEX_MMODE + TEX_SET_BOX_DIMEN:
		case TEX_VMODE + TEX_SET_SHAPE:
		case TEX_HMODE + TEX_SET_SHAPE:
		case TEX_MMODE + TEX_SET_SHAPE:
		case TEX_VMODE + TEX_DEF_CODE:
		case TEX_HMODE + TEX_DEF_CODE:
		case TEX_MMODE + TEX_DEF_CODE:
		case TEX_VMODE + TEX_DEF_FAMILY:
		case TEX_HMODE + TEX_DEF_FAMILY:
		case TEX_MMODE + TEX_DEF_FAMILY:
		case TEX_VMODE + TEX_SET_FONT:
		case TEX_HMODE + TEX_SET_FONT:
		case TEX_MMODE + TEX_SET_FONT:
		case TEX_VMODE + TEX_DEF_FONT:
		case TEX_HMODE + TEX_DEF_FONT:
		case TEX_MMODE + TEX_DEF_FONT:
		case TEX_VMODE + TEX_REGISTER:
		case TEX_HMODE + TEX_REGISTER:
		case TEX_MMODE + TEX_REGISTER:
		case TEX_VMODE + TEX_ADVANCE:
		case TEX_HMODE + TEX_ADVANCE:
		case TEX_MMODE + TEX_ADVANCE:
		case TEX_VMODE + TEX_MULTIPLY:
		case TEX_HMODE + TEX_MULTIPLY:
		case TEX_MMODE + TEX_MULTIPLY:
		case TEX_VMODE + TEX_DIVIDE:
		case TEX_HMODE + TEX_DIVIDE:
		case TEX_MMODE + TEX_DIVIDE:
		case TEX_VMODE + TEX_PREFIX:
		case TEX_HMODE + TEX_PREFIX:
		case TEX_MMODE + TEX_PREFIX:
		case TEX_VMODE + TEX_LET:
		case TEX_HMODE + TEX_LET:
		case TEX_MMODE + TEX_LET:
		case TEX_VMODE + TEX_SHORTHAND_DEF:
		case TEX_HMODE + TEX_SHORTHAND_DEF:
		case TEX_MMODE + TEX_SHORTHAND_DEF:
		case TEX_VMODE + TEX_READ_TO_CS:
		case TEX_HMODE + TEX_READ_TO_CS:
		case TEX_MMODE + TEX_READ_TO_CS:
		case TEX_VMODE + TEX_DEF:
		case TEX_HMODE + TEX_DEF:
		case TEX_MMODE + TEX_DEF:
		case TEX_VMODE + TEX_SET_BOX:
		case TEX_HMODE + TEX_SET_BOX:
		case TEX_MMODE + TEX_SET_BOX:
		case TEX_VMODE + TEX_HYPH_DATA:
		case TEX_HMODE + TEX_HYPH_DATA:
		case TEX_MMODE + TEX_HYPH_DATA:
		case TEX_VMODE + TEX_SET_INTERACTION:
		case TEX_HMODE + TEX_SET_INTERACTION:
		case TEX_MMODE + TEX_SET_INTERACTION:
		case TEX_VMODE + TEX_SET_OCP:
		case TEX_HMODE + TEX_SET_OCP:
		case TEX_MMODE + TEX_SET_OCP:
		case TEX_VMODE + TEX_DEF_OCP:
		case TEX_HMODE + TEX_DEF_OCP:
		case TEX_MMODE + TEX_DEF_OCP:
		case TEX_VMODE + TEX_SET_OCP_LIST:
		case TEX_HMODE + TEX_SET_OCP_LIST:
		case TEX_MMODE + TEX_SET_OCP_LIST:
		case TEX_VMODE + TEX_DEF_OCP_LIST:
		case TEX_HMODE + TEX_DEF_OCP_LIST:
		case TEX_MMODE + TEX_DEF_OCP_LIST:
		case TEX_VMODE + TEX_CLEAR_OCP_LISTS:
		case TEX_HMODE + TEX_CLEAR_OCP_LISTS:
		case TEX_MMODE + TEX_CLEAR_OCP_LISTS:
		case TEX_VMODE + TEX_PUSH_OCP_LIST:
		case TEX_HMODE + TEX_PUSH_OCP_LIST:
		case TEX_MMODE + TEX_PUSH_OCP_LIST:
		case TEX_VMODE + TEX_POP_OCP_LIST:
		case TEX_HMODE + TEX_POP_OCP_LIST:
		case TEX_MMODE + TEX_POP_OCP_LIST:
		case TEX_VMODE + TEX_OCP_LIST_OP:
		case TEX_HMODE + TEX_OCP_LIST_OP:
		case TEX_MMODE + TEX_OCP_LIST_OP:
		case TEX_VMODE + TEX_OCP_TRACE_LEVEL:
		case TEX_HMODE + TEX_OCP_TRACE_LEVEL:
		case TEX_MMODE + TEX_OCP_TRACE_LEVEL:
			prefixedCommand();
			break;
			
		case TEX_VMODE + TEX_CHAR_MODE:
		case TEX_HMODE + TEX_CHAR_MODE:
		case TEX_MMODE + TEX_CHAR_MODE:
			doCharMode();
			break;
			
		case TEX_VMODE + TEX_CHAR_TRANS:
		case TEX_HMODE + TEX_CHAR_TRANS:
		case TEX_MMODE + TEX_CHAR_TRANS:
			doCharTranslation();
			break;
			
		case TEX_VMODE + TEX_AFTER_ASSIGNMENT:
		case TEX_HMODE + TEX_AFTER_ASSIGNMENT:
		case TEX_MMODE + TEX_AFTER_ASSIGNMENT:
			getToken(); 
			after_token = cur_tok;
			break;
			
		case TEX_VMODE + TEX_AFTER_GROUP:
		case TEX_HMODE + TEX_AFTER_GROUP:
		case TEX_MMODE + TEX_AFTER_GROUP:
			getToken(); 
			saveForAfter(cur_tok);
			break;
			
		case TEX_VMODE + TEX_IN_STREAM:
		case TEX_HMODE + TEX_IN_STREAM:
		case TEX_MMODE + TEX_IN_STREAM:
			openOrCloseIn();
			break;
			
		case TEX_VMODE + TEX_MESSAGE:
		case TEX_HMODE + TEX_MESSAGE:
		case TEX_MMODE + TEX_MESSAGE:
			issueMessage();
			break;
			
		case TEX_VMODE + TEX_CASE_SHIFT:
		case TEX_HMODE + TEX_CASE_SHIFT:
		case TEX_MMODE + TEX_CASE_SHIFT:
			shiftCase();
			break;
			
		case TEX_VMODE + TEX_XRAY:
		case TEX_HMODE + TEX_XRAY:
		case TEX_MMODE + TEX_XRAY:
			showWhatEver();
			break;
			
		case TEX_VMODE + TEX_EXTENSION:
		case TEX_HMODE + TEX_EXTENSION:
		case TEX_MMODE + TEX_EXTENSION:
			doExtension();
			break;
			
		default:
			break;
			
	}
	
	goto big_switch;
	
main_loop:
	if (isLastOcp(current_ocp_lstack, current_ocp_no))
	{
		if (((head == tail) && (mode > 0)))
		{
			if (insert_src_special_auto)
				appendSrcSpecial();
		}
		
		main_s = sfCode(cur_chr);
		if (main_s == 1000)
			space_factor = 1000;
		else if (main_s < 1000)
		{
			if (main_s > 0)
				space_factor = main_s;
		}
		else if (space_factor < 1000)
			space_factor = 1000;
		else
			space_factor = main_s;
			
		main_f = curFont();
		bchar = font_bchar(main_f); 
		false_bchar = font_false_bchar(main_f);
		if (mode > 0)
		{
			if (language() != clang)
				fixLanguage();
		}
		
		fastGetAvail(lig_stack); 
		font(lig_stack) = (quint16)main_f; 
		cur_l = cur_chr;
		character(lig_stack) = (quint16)cur_l;
		cur_q = tail;
		if (cancel_boundary)
		{
			cancel_boundary = false; 
			main_k = TEX_NON_ADDRESS;
  			left_ghost = new_left_ghost; 
  			right_ghost = new_right_ghost;
  			new_left_ghost = false; 
  			new_right_ghost = false;
		}
		else
			main_k = bchar_label(main_f);
			
		if (main_k == TEX_NON_ADDRESS)
			goto main_loop_move2;
			
		cur_r = cur_l; 
		cur_l = TEX_NON_CHAR;
		if (!new_right_ghost)
			goto main_lig_loop1;
			
main_loop_wrapup:
		
		if (cur_l < TEX_NON_CHAR)
		{
			if (character(tail) == (quint16)(hyphen_char(main_f)))
			{
				if (link(cur_q) > TEX_NULL)
					ins_disc = true;
			}
			
			if (ligature_present)
			{
				main_p = newLigature(main_f, cur_l, link(cur_q));
				if (lft_hit)
				{
					subtype(main_p) = (quint16)2; 
					lft_hit = false;
				}
				
				if (rt_hit)
				{
					if (lig_stack == TEX_NULL)
					{
						subtype(main_p)++; 
						rt_hit = false;
					}
				}
				
				link(cur_q) = main_p; 
				tail = main_p; 
				ligature_present = false;
			}
			
			if (ins_disc)
			{
				ins_disc = false;
				if (mode > 0)
					tailAppend(newDisc());
			}
		}
	
main_loop_move:
	
		if (lig_stack == TEX_NULL)
			goto reswitch;
			
		cur_q = tail; 
		cur_l = character(lig_stack);
		
main_loop_move1:
		if (!isCharNode(lig_stack))
			goto main_loop_move_lig;
			
main_loop_move2:
		left_ghost = new_left_ghost; 
		right_ghost = new_right_ghost;
		new_left_ghost = false; 
		new_right_ghost = false;
		if ((cur_chr < font_bc(main_f)) || (cur_chr > font_ec(main_f)))
		{
			charWarning(main_f, cur_chr); 
			freeAvail(lig_stack); 
			goto big_switch;
		}
		
		main_i = char_info(main_f, cur_l);
		if (!char_exists(main_i))
		{
			charWarning(main_f, cur_chr); 
			freeAvail(lig_stack); 
			goto big_switch;
		}
		
		if (!(left_ghost || right_ghost))
			tailAppend(lig_stack);
	
main_loop_lookahead:
	
		getNext();
		if (cur_cmd == TEX_LETTER)
			goto main_loop_lookahead1;
			
		if (cur_cmd == TEX_OTHER_CHAR)
			goto main_loop_lookahead1;
			
		if (cur_cmd == TEX_CHAR_GIVEN)
			goto main_loop_lookahead1;
			
		xToken();
		
		if (cur_cmd == TEX_LETTER)
			goto main_loop_lookahead1;
			
		if (cur_cmd == TEX_OTHER_CHAR)
			goto main_loop_lookahead1;
			
		if (cur_cmd == TEX_CHAR_GIVEN)
			goto main_loop_lookahead1;
			
		if (cur_cmd == TEX_CHAR_NUM)
		{
			scanCharNum(); 
			cur_chr = cur_val; 
			goto main_loop_lookahead1;
		}
		
		if (cur_cmd == TEX_CHAR_GHOST)
		{
			t = cur_chr; 
			getXToken();
			if ((cur_cmd == TEX_LETTER) || 
				(cur_cmd == TEX_OTHER_CHAR) || 
				(cur_cmd == TEX_CHAR_GIVEN) || 
				(cur_cmd == TEX_CHAR_NUM))
			{
				if (t == 0)
					new_left_ghost = true;
				else
					new_right_ghost = true;
					
				backInput();
    			goto main_loop_lookahead;
			}
		}
		
		if (cur_cmd == TEX_NO_BOUNDARY)
			bchar = TEX_NON_CHAR;
			
		cur_r = bchar; 
		lig_stack = TEX_NULL; 
		goto main_lig_loop;
		
main_loop_lookahead1:
		main_s = sfCode(cur_chr);
		if (main_s == 1000)
			space_factor = 1000;
		else if (main_s < 1000)
		{
			if (main_s > 0)
				space_factor = main_s;
		}
		else if (space_factor < 1000)
			space_factor = 1000;
		else
			space_factor = main_s;
			
		fastGetAvail(lig_stack); 
		font(lig_stack) = (quint16)main_f;
		cur_r = cur_chr; 
		character(lig_stack) = (quint16)cur_r;
		
		if (cur_r == false_bchar)
			cur_r = TEX_NON_CHAR;
	
main_lig_loop:
	
		if (new_right_ghost || left_ghost)
			goto main_loop_wrapup;
		
		if (char_tag(main_i) != (quint16)TEX_LIG_TAG)
			goto main_loop_wrapup;
			
		main_k = lig_kern_start(main_f, main_i);
		main_j = font_info(main_f, main_k).qqqq;
		if (skip_byte(main_j) <= TEX_STOP_FLAG)
			goto main_lig_loop2;
			
		main_k = lig_kern_restart(main_f, main_j);
		
main_lig_loop1: 
		main_j = font_info(main_f, main_k).qqqq;
		
main_lig_loop2:
		if (top_skip_byte(main_j) == 0)
		{
			if (next_char(main_f, main_j) == cur_r)
			{
				if (skip_byte(main_j) <= TEX_STOP_FLAG)
				{
					if (op_byte(main_j) >= (quint16)TEX_KERN_FLAG)
					{
						if (cur_l < TEX_NON_CHAR)
						{
							if (character(tail) == (quint16)(hyphen_char(main_f)))
							{
								if (link(cur_q) > TEX_NULL)
									ins_disc = true;
							}
			
							if (ligature_present)
							{
								main_p = newLigature(main_f, cur_l, link(cur_q));
								if (lft_hit)
								{
									subtype(main_p) = (quint16)2; 
									lft_hit = false;
								}
				
								if (rt_hit)
								{
									if (lig_stack == TEX_NULL)
									{
										subtype(main_p)++; 
										rt_hit = false;
									}
								}
				
								link(cur_q) = main_p; 
								tail = main_p; 
								ligature_present = false;
							}
			
							if (ins_disc)
							{
								ins_disc = false;
								if (mode > 0)
									tailAppend(newDisc());
							}
						}
						
						tailAppend(newKern(char_kern(main_f, main_j)));
						if (new_left_ghost || right_ghost)
							subtype(tail) = (quint16)TEX_EXPLICIT;
							
						goto main_loop_move;
					}
					
					if (new_left_ghost || right_ghost)
						goto main_loop_wrapup;
						
					if (cur_l == TEX_NON_CHAR)
						lft_hit = true;
					else if (lig_stack == TEX_NULL)
						rt_hit = true;
						
					checkInterrupt();
					
					switch (op_byte(main_j))
					{
						case 1:
						case 5:
							cur_l = rem_char_byte(main_f, main_j);
							main_i = char_info(main_f, cur_l); 
							ligature_present = true;
							break;
							
						case 2:
						case 6:
							cur_r = rem_char_byte(main_f, main_j);
							if (lig_stack == TEX_NULL)
							{
								lig_stack = newLigItem(cur_r); 
								bchar = TEX_NON_CHAR;
							}
							else if (isCharNode(lig_stack))
							{
								main_p = lig_stack; 
								lig_stack = newLigItem(cur_r);
    							lig_ptr(lig_stack) = main_p;
							}
							else
								character(lig_stack) = (quint16)cur_r;
							break;
							
						case 3:
							cur_r = rem_char_byte(main_f, main_j);
							main_p = lig_stack; 
							lig_stack = newLigItem(cur_r);
  							link(lig_stack) = main_p;
							break;
							
						case 7:
						case 11:
							if (cur_l < TEX_NON_CHAR)
							{
								if (character(tail) == (quint16)(hyphen_char(main_f)))
								{
									if (link(cur_q) > TEX_NULL)
										ins_disc = true;
								}
			
								if (ligature_present)
								{
									main_p = newLigature(main_f, cur_l, link(cur_q));
									if (lft_hit)
									{
										subtype(main_p) = (quint16)2; 
										lft_hit = false;
									}
				
									if (false)
									{
										if (lig_stack == TEX_NULL)
										{
											subtype(main_p)++; 
											rt_hit = false;
										}
									}
				
									link(cur_q) = main_p; 
									tail = main_p; 
									ligature_present = false;
								}
			
								if (ins_disc)
								{
									ins_disc = false;
									if (mode > 0)
										tailAppend(newDisc());
								}
							}
							cur_q = tail; 
							cur_l = rem_char_byte(main_f, main_j);
  							main_i = char_info(main_f, cur_l); 
  							ligature_present = true;
							break;
							
						default:
							cur_l = rem_char_byte(main_f, main_j);
                 			ligature_present = true;
                 			if (lig_stack == TEX_NULL)
                 				goto main_loop_wrapup;
                 			else
                 				goto main_loop_move1;
							break;
					}
					
					if (op_byte(main_j) > (quint16)4)
					{
						if (op_byte(main_j) != (quint16)7)
							goto main_loop_wrapup;
					}
					
					if (cur_l < TEX_NON_CHAR)
						goto main_lig_loop;
						
					main_k = bchar_label(main_f); 
					goto main_lig_loop1;
				}
			}
		}
		else
		{
			if ((font_bc(main_f) <= cur_r))
			{
				if ((font_ec(main_f) >= cur_r))
				{
					if ((char_exists(char_info(main_f, cur_r))))
					{
						if (cur_r != bchar)
						{
							if (next_char(main_f, main_j) == attr_zero_char_ivalue(main_f, cur_r))
							{
								if (new_left_ghost || right_ghost)
									goto main_loop_wrapup;
									
								switch (op_byte(main_j))
								{
									case 20:
										if (cur_l < TEX_NON_CHAR)
										{
											if (character(tail) == (quint16)(hyphen_char(main_f)))
											{
												if (link(cur_q) > TEX_NULL)
													ins_disc = true;
											}
			
											if (ligature_present)
											{
												main_p = newLigature(main_f, cur_l, link(cur_q));
												if (lft_hit)
												{
													subtype(main_p) = (quint16)2; 
													lft_hit = false;
												}
				
												if (rt_hit)
												{
													if (lig_stack == TEX_NULL)
													{
														subtype(main_p)++; 
														rt_hit = false;
													}
												}
				
												link(cur_q) = main_p; 
												tail = main_p; 
												ligature_present = false;
											}
			
											if (ins_disc)
											{
												ins_disc = false;
												if (mode > 0)
													tailAppend(newDisc());
											}
										}
										tailAppend(newKern(attr_zero_kern(main_f, rem_byte(main_j))));
										break;
							
									case 18:
										if (cur_l < TEX_NON_CHAR)
										{
											if (character(tail) == (quint16)(hyphen_char(main_f)))
											{
												if (link(cur_q) > TEX_NULL)
													ins_disc = true;
											}
			
											if (ligature_present)
											{
												main_p = newLigature(main_f, cur_l, link(cur_q));
												if (lft_hit)
												{
													subtype(main_p) = (quint16)2; 
													lft_hit = false;
												}
				
												if (rt_hit)
												{
													if (lig_stack == TEX_NULL)
													{
														subtype(main_p)++; 
														rt_hit = false;
													}
												}
				
												link(cur_q) = main_p; 
												tail = main_p; 
												ligature_present = false;
											}
			
											if (ins_disc)
											{
												ins_disc = false;
												if (mode > 0)
													tailAppend(newDisc());
											}
										}
										k_glue = glues_base(main_f) + (rem_byte(main_j) * 4);
    									font_glue_spec = newSpec(zero_glue);
    									width(font_glue_spec) = font_info(main_f, k_glue + 1).ii.CINT0;
    									stretch(font_glue_spec) = font_info(main_f, k_glue + 2).ii.CINT0;
    									shrink(font_glue_spec) = font_info(main_f, k_glue+3).ii.CINT0;
    									tailAppend(newGlue(font_glue_spec));
										break;
							
									case 19:
										if (cur_l < TEX_NON_CHAR)
										{
											if (character(tail) == (quint16)(hyphen_char(main_f)))
											{
												if (link(cur_q) > TEX_NULL)
													ins_disc = true;
											}
			
											if (ligature_present)
											{
												main_p = newLigature(main_f, cur_l, link(cur_q));
												if (lft_hit)
												{
													subtype(main_p) = (quint16)2; 
													lft_hit = false;
												}
				
												if (rt_hit)
												{
													if (lig_stack == TEX_NULL)
													{
														subtype(main_p)++; 
														rt_hit = false;
													}
												}
				
												link(cur_q) = main_p; 
												tail = main_p; 
												ligature_present = false;
											}
			
											if (ins_disc)
											{
												ins_disc = false;
												if (mode > 0)
													tailAppend(newDisc());
											}
										}
										tailAppend(newPenalty(attr_zero_penalty(main_f, rem_top_byte(main_j))));
    									k_glue = glues_base(main_f) + (rem_bot_byte(main_j) * 4);
    									font_glue_spec = newSpec(zero_glue);
    									width(font_glue_spec) = font_info(main_f, k_glue + 1).ii.CINT0;
    									stretch(font_glue_spec) = font_info(main_f, k_glue + 2).ii.CINT0;
    									shrink(font_glue_spec) = font_info(main_f, k_glue + 3).ii.CINT0;
    									tailAppend(newGlue(font_glue_spec));
										break;
							
									case 17:
										if (cur_l < TEX_NON_CHAR)
										{
											if (character(tail) == (quint16)(hyphen_char(main_f)))
											{
												if (link(cur_q) > TEX_NULL)
													ins_disc = true;
											}
			
											if (ligature_present)
											{
												main_p = newLigature(main_f, cur_l, link(cur_q));
												if (lft_hit)
												{
													subtype(main_p) = (quint16)2; 
													lft_hit = false;
												}
				
												if (rt_hit)
												{
													if (lig_stack == TEX_NULL)
													{
														subtype(main_p)++; 
														rt_hit = false;
													}
												}
				
												link(cur_q) = main_p; 
												tail = main_p; 
												ligature_present = false;
											}
			
											if (ins_disc)
											{
												ins_disc = false;
												if (mode > 0)
													tailAppend(newDisc());
											}
										}
										tailAppend(newPenalty(attr_zero_penalty(main_f, rem_byte(main_j))));
										break;
							
									default:
										break;
								}
					
								goto main_loop_move;
							}
						}
					}
				}
			}
		}
		
		if (skip_byte(main_j) == 0)
			main_k++;
		else 
		{
			if (skip_byte(main_j) >= TEX_STOP_FLAG)
				goto main_loop_wrapup;
				
			main_k = main_k + skip_byte(main_j) + 1;
		}
		
		goto main_lig_loop1;
		
main_loop_move_lig:
	
		main_p = lig_ptr(lig_stack);
		if (main_p > TEX_NULL)
			tailAppend(main_p);
			
		temp_ptr = lig_stack; 
		lig_stack = link(temp_ptr);
		freeNode(temp_ptr, TEX_SMALL_NODE_SIZE);
		main_i = char_info(main_f, cur_l); 
		ligature_present = true;
		if (lig_stack == TEX_NULL)
		{
			if (main_p > TEX_NULL)
				goto main_loop_lookahead;
			else
				cur_r = bchar;
		}
		else
			cur_r = character(lig_stack);
			
		goto main_lig_loop;
	}
	else
	{
		if (cur_cmd == TEX_CHAR_GIVEN || cur_cmd == TEX_CHAR_NUM)
		{
			backInput();
  		current_ocp_lstack = active_lstack_no(active_real);
  		current_ocp_no = active_counter(active_real);
		}
		else
		{
			otp_init_input_end = 0;
			while ((cur_cmd == TEX_LETTER) || 
						(cur_cmd == TEX_OTHER_CHAR) || 
						(cur_cmd == TEX_CHAR_GIVEN) || 
						(cur_cmd == TEX_SPACER))
      {
      	otp_init_input_end++;
      	if (otp_init_input_end > ocp_buf_size)
      	{
      		overFlowOcpBufSize();
      		return ;
      	}
      		
      	otp_init_input_buf[otp_init_input_end] = cur_chr;
  		  getXToken();
  			if ((cur_cmd == TEX_CHAR_NUM))
  			{
  				scanCharNum(); 
  				cur_chr = cur_val;
  			}
      }
      	
      	backInput();
		otp_input_end = otp_init_input_end;
		for (otp_i = 0; otp_i <= otp_init_input_end; otp_i++)
			otp_input_buf[otp_i] = otp_init_input_buf[otp_i];
			
		otp_input_ocp = active_ocp(active_real);
		if (otp_input_ocp == 0)
		{
			printnl(tr("Null ocp being used: all input lost"));
  			otp_output_end = 0;
		}
		else if (ocp_external(otp_input_ocp) == 0)
		{
			otp_input_start = 0;
  			otp_input_last  = 0;
  			otp_stack_used  = 0;
  			otp_stack_last  = 0;
  			otp_stack_new   = 0;
  			otp_output_end  = 0;
  			otp_pc = 0;
  			otp_finished = false;
  			otp_calc_ptr = 0;
  			otp_calcs[otp_calc_ptr] = 0;
  			otp_state_ptr = 0;
  			otp_states[otp_state_ptr] = 0;
  			while (!otp_finished)
  			{
  				if (otp_pc >= ocp_state_no(otp_input_ocp, otp_cur_state))
   				{
   					printErr(tr("bad OCP program -- PC not valid"));
  					succumb();
  					return ;
   				}
   			
   				otp_instruction = ocp_state_entry(otp_input_ocp, otp_cur_state, otp_pc);
				otp_instr = otp_instruction / 0x1000000;
				otp_arg = otp_instruction % 0x1000000;
			
				switch (otp_instr)
				{
					case TEX_OTP_RIGHT_OUTPUT:
						otp_output_end++;
						if (otp_output_end >ocp_buf_size)
						{
							overFlowOcpBufSize();
							return ;
						}					
						otp_output_buf[otp_output_end] = otp_calcs[otp_calc_ptr];
    					otp_calc_ptr--;
    					otp_pc++;
						break;
					
					case TEX_OTP_RIGHT_NUM:
						otp_output_end++;
						if (otp_output_end > ocp_buf_size)
						{
							overFlowOcpBufSize();
							return ;
						}
						otp_output_buf[otp_output_end] = otp_arg;
    					otp_pc++;
						break;
					
					case TEX_OTP_RIGHT_CHAR:
						if ((1 > otp_arg) || (otp_arg > otp_no_input_chars))
						{
							printErr(tr("right hand side of OCP expression is bad"));
  							succumb();
  							return ;
						}
						if (otp_arg > otp_stack_last)
							otp_calculated_char = otp_input_buf[otp_input_start + otp_arg - otp_stack_last];
						else
							otp_calculated_char = otp_stack_buf[otp_arg];
						
						otp_output_end++;
						if (otp_output_end > ocp_buf_size)
						{
							overFlowOcpBufSize();
							return ;
						}
						otp_output_buf[otp_output_end] = otp_calculated_char;
    					otp_pc++;
						break;
					
					case TEX_OTP_RIGHT_LCHAR:
						if ((1 > (otp_no_input_chars - otp_arg)) || ((otp_no_input_chars - otp_arg) > otp_no_input_chars))
						{
							printErr(tr("right hand side of OCP expression is bad"));
  							succumb();
  							return ;
						}
						if ((otp_no_input_chars - otp_arg) > otp_stack_last)
							otp_calculated_char = otp_input_buf[otp_input_start + (otp_no_input_chars - otp_arg) - otp_stack_last];
						else
							otp_calculated_char = otp_stack_buf[(otp_no_input_chars - otp_arg)];
						otp_output_end++;
						if (otp_output_end > ocp_buf_size)
						{
							overFlowOcpBufSize();
							return ;
						}
						otp_output_buf[otp_output_end] = otp_calculated_char;
    					otp_pc++;
						break;
					
					case TEX_OTP_RIGHT_SOME:
						otp_first_arg = otp_arg + 1;
    					otp_pc++;
    					if (otp_pc >= ocp_state_no(otp_input_ocp, otp_cur_state))
   						{
   							printErr(tr("bad OCP program -- PC not valid"));
  							succumb();
  							return ;
   						}
   			
   						otp_instruction = ocp_state_entry(otp_input_ocp, otp_cur_state, otp_pc);
						otp_instr = otp_instruction / 0x1000000;
						otp_arg = otp_instruction % 0x1000000;
						otp_second_arg = otp_no_input_chars - otp_arg;
						for (otp_counter = otp_first_arg; otp_counter <= otp_second_arg; otp_counter++)
						{
							if ((1 > otp_counter) || (otp_counter > otp_no_input_chars))
							{
								printErr(tr("right hand side of OCP expression is bad"));
  								succumb();
  								return ;
							}
							if (otp_counter > otp_stack_last)
								otp_calculated_char = otp_input_buf[otp_input_start + otp_counter - otp_stack_last];
							else
								otp_calculated_char = otp_stack_buf[otp_counter];
							otp_output_end++;
							if (otp_output_end > ocp_buf_size)
							{
								overFlowOcpBufSize();
								return ;
							}
							otp_output_buf[otp_output_end] = otp_calculated_char;
						}
						otp_pc++;
						break;
					
					case TEX_OTP_PBACK_OUTPUT:
						otp_stack_new++;
						if (otp_stack_new >= ocp_stack_size)
						{
							overFlowOcpStackSize();
							return ;
						}
						otp_stack_buf[otp_stack_new] = otp_calcs[otp_calc_ptr];
    					otp_calc_ptr--;
    					otp_pc++;
						break;
					
					case TEX_OTP_PBACK_NUM:
						otp_stack_new++;
						if (otp_stack_new >= ocp_stack_size)
						{
							overFlowOcpStackSize();
							return ;
						}
						otp_stack_buf[otp_stack_new] = otp_arg;
    					otp_pc++;
						break;
					
					case TEX_OTP_PBACK_CHAR:
						if ((1 > otp_arg) || (otp_arg > otp_no_input_chars))
						{
							printErr(tr("right hand side of OCP expression is bad"));
  							succumb();
  							return ;
						}
						if (otp_arg > otp_stack_last)
							otp_calculated_char = otp_input_buf[otp_input_start + otp_arg - otp_stack_last];
						else
							otp_calculated_char = otp_stack_buf[otp_arg];
						otp_stack_new++;
						if (otp_stack_new >= ocp_stack_size)
						{
							overFlowOcpStackSize();
							return ;
						}
						otp_stack_buf[otp_stack_new] = otp_calculated_char;
    					otp_pc++;
						break;
					
					case TEX_OTP_PBACK_LCHAR:
						if ((1 > (otp_no_input_chars - otp_arg)) || ((otp_no_input_chars - otp_arg) > otp_no_input_chars))
						{
							printErr(tr("right hand side of OCP expression is bad"));
  							succumb();
  							return ;
						}
						if ((otp_no_input_chars - otp_arg) > otp_stack_last)
							otp_calculated_char = otp_input_buf[otp_input_start + (otp_no_input_chars - otp_arg) - otp_stack_last];
						else
							otp_calculated_char = otp_stack_buf[(otp_no_input_chars - otp_arg)];
						otp_stack_new++;
						if (otp_stack_new >= ocp_stack_size)
						{
							overFlowOcpStackSize();
							return ;
						}
						otp_stack_buf[otp_stack_new] = otp_calculated_char;
    					otp_pc++;
						break;
					
					case TEX_OTP_PBACK_SOME:
						otp_first_arg = otp_arg + 1;
    					otp_pc++;
    					if (otp_pc >= ocp_state_no(otp_input_ocp, otp_cur_state))
   						{
   							printErr(tr("bad OCP program -- PC not valid"));
  							succumb();
  							return ;
   						}
   			
   						otp_instruction = ocp_state_entry(otp_input_ocp, otp_cur_state, otp_pc);
						otp_instr = otp_instruction / 0x1000000;
						otp_arg = otp_instruction % 0x1000000;
						otp_second_arg = otp_no_input_chars - otp_arg;
						for (otp_counter = otp_first_arg; otp_first_arg <= otp_second_arg; otp_first_arg++)
						{
							if ((1 > otp_counter) || (otp_counter > otp_no_input_chars))
							{
								printErr(tr("right hand side of OCP expression is bad"));
  								succumb();
  								return ;
							}
							if (otp_counter > otp_stack_last)
								otp_calculated_char = otp_input_buf[otp_input_start + otp_counter - otp_stack_last];
							else
								otp_calculated_char = otp_stack_buf[otp_counter];
							
							otp_stack_new++;
							if (otp_stack_new >= ocp_stack_size)
							{
								overFlowOcpStackSize();
								return ;
							}
							otp_stack_buf[otp_stack_new] = otp_calculated_char;
						}
						otp_pc++;
						break;
					
					case TEX_OTP_ADD:
						otp_calcs[otp_calc_ptr-1] = otp_calcs[otp_calc_ptr - 1] + otp_calcs[otp_calc_ptr];
    					otp_pc++;
    					otp_calc_ptr--;
						break;
					
					case TEX_OTP_SUB:
						otp_calcs[otp_calc_ptr-1] = otp_calcs[otp_calc_ptr - 1] - otp_calcs[otp_calc_ptr];
    					otp_pc++;
    					otp_calc_ptr--;
						break;
					
					case TEX_OTP_MULT:
						otp_calcs[otp_calc_ptr-1] = otp_calcs[otp_calc_ptr - 1] * otp_calcs[otp_calc_ptr];
    					otp_pc++;
    					otp_calc_ptr--;
						break;
					
					case TEX_OTP_DIV:
						otp_calcs[otp_calc_ptr-1] = otp_calcs[otp_calc_ptr - 1] / otp_calcs[otp_calc_ptr];
    					otp_pc++;
    					otp_calc_ptr--;
						break;
					
					case TEX_OTP_MOD:
						otp_calcs[otp_calc_ptr-1] = otp_calcs[otp_calc_ptr - 1] % otp_calcs[otp_calc_ptr];
    					otp_pc++;
    					otp_calc_ptr--;
						break;
					
					case TEX_OTP_LOOKUP:
						if (otp_calcs[otp_calc_ptr] >= ocp_table_no(otp_input_ocp, otp_calcs[otp_calc_ptr - 1]))
						{
							printErr("bad OCP program -- table index not valid");
      						succumb();
      						return ;
						}
						otp_calcs[otp_calc_ptr - 1] = ocp_table_entry(otp_input_ocp, otp_calcs[otp_calc_ptr - 1], otp_calcs[otp_calc_ptr]);
    					otp_pc++;
    					otp_calc_ptr--;
						break;
					
					case TEX_OTP_PUSH_NUM:
						otp_calc_ptr++;
						if (otp_calc_ptr >= ocp_stack_size)
						{
							overFlowOcpStackSize();
							return ;
						}
						otp_calcs[otp_calc_ptr] = otp_arg;
    					otp_pc++;
						break;
					
					case TEX_OTP_PUSH_CHAR:
						if ((1 > otp_arg) || (otp_arg > otp_no_input_chars))
						{
							printErr(tr("right hand side of OCP expression is bad"));
  							succumb();
  							return ;
						}
						if (otp_arg > otp_stack_last)
							otp_calculated_char = otp_input_buf[otp_input_start + otp_arg - otp_stack_last];
						else
							otp_calculated_char = otp_stack_buf[otp_arg];
						otp_calc_ptr++;
						if (otp_calc_ptr >= ocp_stack_size)
						{
							overFlowOcpStackSize();
							return ;
						}
						otp_calcs[otp_calc_ptr] = otp_calculated_char;
    					otp_pc++;
						break;
					
					case TEX_OTP_PUSH_LCHAR:
						if ((1 > (otp_no_input_chars - otp_arg)) || ((otp_no_input_chars - otp_arg) > otp_no_input_chars))
						{
							printErr(tr("right hand side of OCP expression is bad"));
  							succumb();
  							return ;
						}
						if ((otp_no_input_chars - otp_arg) > otp_stack_last)
							otp_calculated_char = otp_input_buf[otp_input_start + (otp_no_input_chars - otp_arg) - otp_stack_last];
						else
							otp_calculated_char = otp_stack_buf[(otp_no_input_chars - otp_arg)];
						otp_calc_ptr++;
						if (otp_calc_ptr >= ocp_stack_size)
						{
							overFlowOcpStackSize();
							return ;
						}
						otp_calcs[otp_calc_ptr] = otp_calculated_char;
    					otp_pc++;
						break;
					
					case TEX_OTP_STATE_CHANGE:
						otp_input_start = otp_input_last;
						for (otp_counter = 1; otp_counter <= (otp_stack_new-otp_stack_used); otp_counter++)
							otp_stack_buf[otp_counter] = otp_stack_buf[otp_counter + otp_stack_used];
						otp_stack_new = otp_stack_new - otp_stack_used;
    					otp_stack_last = otp_stack_new;
    					otp_stack_used = 0;
    					otp_states[otp_state_ptr] = otp_arg;
    					otp_pc = 0;
						break;
					
					case TEX_OTP_STATE_PUSH:
						otp_input_start = otp_input_last;
						for (otp_counter = 1; otp_counter <= (otp_stack_new-otp_stack_used); otp_counter++)
							otp_stack_buf[otp_counter] = otp_stack_buf[otp_counter + otp_stack_used];
						otp_stack_new  = otp_stack_new - otp_stack_used;
    					otp_stack_last = otp_stack_new;
    					otp_stack_used = 0;
    					otp_state_ptr++;
    					if (otp_state_ptr >= ocp_stack_size)
    					{
    						overFlowOcpStackSize();
    						return ;
    					}
    					otp_states[otp_state_ptr] = otp_arg;
    					otp_pc = 0;
						break;
					
					case TEX_OTP_STATE_POP:
						otp_input_start = otp_input_last;
						for (otp_counter = 1; otp_counter <= (otp_stack_new-otp_stack_used); otp_counter++)
							otp_stack_buf[otp_counter] = otp_stack_buf[otp_counter + otp_stack_used];
						otp_stack_new  = otp_stack_new - otp_stack_used;
    					otp_stack_last = otp_stack_new;
    					otp_stack_used = 0;
    					if (otp_state_ptr > 0)
    						otp_state_ptr--;
    					otp_pc = 0;
						break;
					
					case TEX_OTP_LEFT_START:
						otp_input_start = otp_input_last;
    					otp_input_last  = otp_input_start;
    					otp_stack_used  = 0;
    					if ((otp_stack_last == 0) && (otp_input_last >= otp_input_end))
    						otp_finished = true;
    					else if (otp_stack_used < otp_stack_last)
    					{
    						otp_stack_used++;
    						otp_input_char = otp_stack_buf[otp_stack_used];
      						otp_no_input_chars = 1;
      						otp_pc++;
    					}
    					else
    					{
    						otp_input_last++;
    						otp_input_char = otp_input_buf[otp_input_last];
      						otp_no_input_chars = 1;
      						otp_pc++;
    					}
						break;
					
					case TEX_OTP_LEFT_RETURN:
						otp_input_last = otp_input_start;
    					otp_stack_used = 0;
    					if (otp_stack_used < otp_stack_last)
    					{
    						otp_stack_used++;
    						otp_input_char = otp_stack_buf[otp_stack_used];
      						otp_no_input_chars = 1;
      						otp_pc++;
    					}
    					else
    					{
    						otp_input_last++;
    						otp_input_char = otp_input_buf[otp_input_last];
      						otp_no_input_chars = 1;
      						otp_pc++;
    					}
						break;
					
					case TEX_OTP_LEFT_BACKUP:
						if (otp_input_start < otp_input_last)
						{
							otp_input_last--; 
							otp_input_char = otp_input_buf[otp_input_last];
						}
						else
						{	
							otp_stack_used--; 
							otp_input_char = otp_stack_buf[otp_stack_used];
						}
						otp_no_input_chars--;
    					otp_pc++;
						break;
					
					case TEX_OTP_GOTO:
						otp_pc = otp_arg;
						break;
					
					case TEX_OTP_GOTO_NE:
						otp_first_arg = otp_arg;
    					otp_pc++;
    					if (otp_pc >= ocp_state_no(otp_input_ocp, otp_cur_state))
   						{
   							printErr(tr("bad OCP program -- PC not valid"));
  							succumb();
  							return ;
   						}
   			
   						otp_instruction = ocp_state_entry(otp_input_ocp, otp_cur_state, otp_pc);
						otp_instr = otp_instruction / 0x1000000;
						otp_arg = otp_instruction % 0x1000000;
						if (otp_input_char != otp_first_arg)
							otp_pc = otp_arg;
						else
							otp_pc++;
						break;
					
					case TEX_OTP_GOTO_EQ:
						otp_first_arg = otp_arg;
    					otp_pc++;
    					if (otp_pc >= ocp_state_no(otp_input_ocp, otp_cur_state))
   						{
   							printErr(tr("bad OCP program -- PC not valid"));
  							succumb();
  							return ;
   						}
   			
   						otp_instruction = ocp_state_entry(otp_input_ocp, otp_cur_state, otp_pc);
						otp_instr = otp_instruction / 0x1000000;
						otp_arg = otp_instruction % 0x1000000;
						if (otp_input_char == otp_first_arg)
							otp_pc = otp_arg;
						else
							otp_pc++;
						break;
					
					case TEX_OTP_GOTO_LT:
						otp_first_arg = otp_arg;
    					otp_pc++;
    					if (otp_pc >= ocp_state_no(otp_input_ocp, otp_cur_state))
   						{
   							printErr(tr("bad OCP program -- PC not valid"));
  							succumb();
  							return ;
   						}
   			
   						otp_instruction = ocp_state_entry(otp_input_ocp, otp_cur_state, otp_pc);
						otp_instr = otp_instruction / 0x1000000;
						otp_arg = otp_instruction % 0x1000000;
						if (otp_input_char < otp_first_arg)
							otp_pc = otp_arg;
						else
							otp_pc++;
						break;
					
					case TEX_OTP_GOTO_LE:
						otp_first_arg = otp_arg;
    					otp_pc++;
    					if (otp_pc >= ocp_state_no(otp_input_ocp, otp_cur_state))
   						{
   							printErr(tr("bad OCP program -- PC not valid"));
  							succumb();
  							return ;
   						}
   			
   						otp_instruction = ocp_state_entry(otp_input_ocp, otp_cur_state, otp_pc);
						otp_instr = otp_instruction / 0x1000000;
						otp_arg = otp_instruction % 0x1000000;
						if (otp_input_char <= otp_first_arg)
							otp_pc = otp_arg;
						else
							otp_pc++;
						break;
					
					case TEX_OTP_GOTO_GT:
						otp_first_arg = otp_arg;
    					otp_pc++;
    					if (otp_pc >= ocp_state_no(otp_input_ocp, otp_cur_state))
   						{
   							printErr(tr("bad OCP program -- PC not valid"));
  							succumb();
  							return ;
   						}
   			
   						otp_instruction = ocp_state_entry(otp_input_ocp, otp_cur_state, otp_pc);
						otp_instr = otp_instruction / 0x1000000;
						otp_arg = otp_instruction % 0x1000000;
						if (otp_input_char > otp_first_arg)
							otp_pc = otp_arg;
						else
							otp_pc++;
						break;
					
					case TEX_OTP_GOTO_GE:
						otp_first_arg = otp_arg;
    					otp_pc++;
    					if (otp_pc >= ocp_state_no(otp_input_ocp, otp_cur_state))
   						{
   							printErr(tr("bad OCP program -- PC not valid"));
  							succumb();
  							return ;
   						}
   			
   						otp_instruction = ocp_state_entry(otp_input_ocp, otp_cur_state, otp_pc);
						otp_instr = otp_instruction / 0x1000000;
						otp_arg = otp_instruction % 0x1000000;
						if (otp_input_char >= otp_first_arg)
							otp_pc = otp_arg;
						else
							otp_pc++;
						break;
					
					case TEX_OTP_GOTO_NO_ADVANCE:
						if (otp_stack_used < otp_stack_last)
						{
							otp_stack_used++;
							otp_input_char = otp_stack_buf[otp_stack_used];
      						otp_no_input_chars++;
      						otp_pc++;
						}
						else if (otp_input_last >= otp_input_end)
							otp_pc = otp_arg;
						else
						{
							otp_input_last++;
							otp_input_char = otp_input_buf[otp_input_last];
      						otp_no_input_chars++;
      						otp_pc++;
						}
						break;
					
					case TEX_OTP_GOTO_BEG:
						if (otp_input_last == 0)
							otp_pc = otp_arg;
						else
							otp_pc++;
						break;
					
					case TEX_OTP_GOTO_END:
						if (otp_input_last >= otp_input_end)
							otp_pc = otp_arg;
						else
							otp_pc++;
						break;
					
					case TEX_OTP_STOP:
						otp_input_start = otp_input_last;
						for (otp_counter = 1; otp_counter <= (otp_stack_new-otp_stack_used); otp_counter++)
							otp_stack_buf[otp_counter] = otp_stack_buf[otp_counter + otp_stack_used];
						otp_stack_new = otp_stack_new - otp_stack_used;
    					otp_stack_last = otp_stack_new;
    					otp_stack_used = 0;
    					otp_pc = 0;
						break;
					
					default:
						printErr(tr("bad OCP program -- unknown instruction"));
    					succumb();
    					return ;
						break;
				}
  		}
		}
		else
		{
			k = 0;
			qint32 c = 0;
			if (name_of_file)
				free(name_of_file);
				
			otp_ext_str = ocp_external(otp_input_ocp);
  			otp_ext_str_arg = ocp_external_arg(otp_input_ocp);
  			name_of_file = (qint32*)malloc((4 + length(otp_ext_str) + length(otp_ext_str_arg) + 4) * sizeof(qint32));
  			for (otp_ext_i = str_start(otp_ext_str); otp_ext_i < (str_start(otp_ext_str) + length(otp_ext_str)); otp_ext_i++)
  			{
  				c = str_pool[otp_ext_i];
  				if (c != '"')
  				{
  					k++;
  					if (k <= TEX_FILE_NAME_SIZE)
  						name_of_file[k] = c;
  				}
  			}
  			
  			k++;
  			if (k <= TEX_FILE_NAME_SIZE)
  				name_of_file[k] = ' ';
  				
  			for (otp_ext_i = str_start(otp_ext_str_arg); otp_ext_i < (str_start(otp_ext_str_arg)+length(otp_ext_str_arg)); otp_ext_i++)
  			{
  				c = str_pool[otp_ext_i];
  				if (c != '"')
  				{
  					k++;
  					if (k <= TEX_FILE_NAME_SIZE)
  						name_of_file[k] = str_pool[otp_ext_i];
  				}
  			}
  			
  			name_of_file[length(otp_ext_str) + length(otp_ext_str_arg) + 2] = 0;
  			runExternalOcp();
			}
		
			if (otp_output_end>0)
			{
				if ((first + otp_output_end) >= ocp_buf_size)
				{
					overFlowOcpBufSize();
					return ;
				}
		
				pushInput();
				current_ocp_lstack = active_lstack_no(active_real);
				current_ocp_no = active_counter(active_real);
				state = TEX_MID_LINE;
				texstart = first;
				last  = texstart;
				loc   = texstart;
				for (otp_counter = 1; otp_counter <= otp_output_end; otp_counter++)
				{
					buffer[last] = otp_output_buf[otp_counter];
  				last++;
				}
		
				limit = last - 1;
			}
		}
		goto big_switch;
	}
	
append_normal_space:
	
	if (spaceSkip() == zero_glue)
	{
		main_p = font_glue(curFont());
		if (main_p == TEX_NULL)
		{
			main_p = newSpec(zero_glue); 
			main_k = param_base(curFont()) + TEX_SPACE_CODE;
  			width(main_p) = font_info(curFont(), main_k).ii.CINT0;
  			stretch(main_p) = font_info(curFont(), main_k + 1).ii.CINT0;
  			shrink(main_p) = font_info(curFont(), main_k + 2).ii.CINT0;
  			font_glue(curFont()) = main_p;
		}
		
		temp_ptr = newGlue(main_p);
	}
	else
		temp_ptr = newParamGlue(TEX_SPACE_SKIP_CODE);
	link(tail) = temp_ptr; 
	tail = temp_ptr;
	goto big_switch;
}

void XWTeX::newFont(quint16 a)
{
	qint32 u, f, t, s, offset, natural_dir;
	if (job_name == 0)
		openLogFile();
		
	getRToken(); 
	u = cur_cs;
	if (u >= TEX_HASH_BASE)
		t = newText(u);
	else if (u >= TEX_SINGLE_BASE)
	{
		if (u == TEX_NULL_CS)
			t = TeXFONT;
		else
			t = u - TEX_SINGLE_BASE;
	}
	else
	{
		 old_setting = selector; 
		 selector = TEX_NEW_STRING;
		 print(TeXFONT); 
		 print(u - TEX_ACTIVE_BASE); 
		 selector = old_setting;
		 strRoom(1); 
		 t = makeString();
	}
	
	if (a >= 4)
		geqDefine(u, (quint16)TEX_SET_FONT, TEX_NULL_FONT);
	else
		eqDefine(u, (quint16)TEX_SET_FONT, TEX_NULL_FONT);
		
	scanOptionalEquals(); 
	scanFileName();
	name_in_progress = true;
	if (scanKeyWord(TeXAt))
	{
		scanNormalDimen(); 
		s = cur_val;
		if ((s <= 0) || ( s >= 0x8000000))
		{
			printErr(tr("Improper `at' size ("));
  			printScaled(s); 
  			print(tr("pt), replaced by 10pt"));
  			help2(tr("I can only handle fonts at positive sizes that are"));
  			help_line[0] = tr("less than 2048pt, so I've changed what you said to 10pt.");
  			error(); 
  			s = 10 * TEX_UNITY;
		}
	}
	else if (scanKeyWord(TeXScaled))
	{
		scanInt(); 
		s = -cur_val;
		if ((cur_val <= 0) || (cur_val > 32768))
		{
			printErr(tr("Illegal magnification has been changed to 1000"));
			help1(tr("The magnification ratio must be between 1 and 32768."));
    		intError(cur_val); 
    		s = -1000;
		}
	}
	else
		s = -1000;
	
	name_in_progress = true;
	if (scanKeyWord(TeXOffset))
	{
		scanInt();
  		offset = cur_val;
  		if ((cur_val < 0))
  		{
  			printErr(tr("Illegal offset has been changed to 0"));
    		help1(tr("The offset must be bigger than 0.")); 
    		intError(cur_val);
    		offset = 0;
  		}
	}
	else
		offset = 0;
		
	if (scanKeyWord(TeXNaturalDir))
	{
		scanDir();
  		natural_dir = cur_val;
	}
	else
		natural_dir = -1;
		
	name_in_progress = false;
	
	for (f = TEX_FONT_BASE + 1; f <= font_ptr; f++)
	{
		if (strEqStr(font_name(f), cur_name) && strEqStr(font_area(f), cur_area))
		{
			if (s > 0)
			{
				if (s == font_size(f))
					goto common_ending;
			}
			else if (font_size(f) == xnOverD(font_dsize(f), -s, 1000))
				goto common_ending;
		}
	}
	
	f = readFontInfo(u, cur_name, cur_area, s, offset, natural_dir);
	
common_ending: 
	setEquiv(u, f);
	setNewEqtb(TEX_FONT_ID_BASE + f, newEqtb(u)); 
	setText(TEX_FONT_ID_BASE + f, t);
}

void XWTeX::newInteraction()
{
	println();
	interaction = cur_chr;
	if (interaction == TEX_BATCH_MODE)
		selector = TEX_NO_PRINT;
	else
		selector = TEX_TERM_ONLY;
		
	if (log_opened)
		selector = selector + 2;
}

void XWTeX::normalParagraph()
{
	if (looseness() != 0)
		eqWordDefine(TEX_INT_BASE + TEX_LOOSENESS_CODE, 0);
		
	if (hangIndent() != 0)
		eqWordDefine(TEX_DIMEN_BASE + TEX_HANG_INDENT_CODE, 0);
		
	if (hangAfter() != 1)
		eqWordDefine(TEX_INT_BASE + TEX_HANG_AFTER_CODE, 1);
		
	if (parShapePtr() != TEX_NULL)
		eqDefine(TEX_PAR_SHAPE_LOC, (quint16)TEX_SHAPE_REF, TEX_NULL);
		
	if (interLinePenaltiesPtr() != TEX_NULL)
		eqDefine(TEX_INTER_LINE_PENALTIES_LOC, (quint16)TEX_SHAPE_REF, TEX_NULL);
}

void XWTeX::openOrCloseIn()
{
	qint32 c, n;
	
	c = cur_chr; 
	scanFourBitInt(); 
	n = cur_val;
	if (read_open[n] != TEX_CLOSED)
	{
		read_file[n]->close();
		read_open[n] = TEX_CLOSED;
	}
	
	if (c != 0)
	{
		scanOptionalEquals(); 
		scanFileName();			
		packCurName();
		tex_input_type = 0;
		QString fn = getFileName();
		if (read_file[n])
		{
			if (read_file[n]->isOpen())
				read_file[n]->close();
			delete read_file[n];
			read_file[n] = 0;
		}
		
		read_file[n] = dev->openInput(fn);
		if (read_file[n])
		{
			read_open[n] = TEX_JUST_OPEN;			
		}
	}
}

void XWTeX::prefixedCommand()
{
	qint32 a, f, j, k, p, q, n;
	bool   e;
	
	a = 0;
	while (cur_cmd == TEX_PREFIX)
	{
		if (!odd(a / cur_chr))
			a = a + cur_chr;
			
		do
		{
			getXToken();
		} while ((cur_cmd == TEX_SPACER) || (cur_cmd == TEX_RELAX));
		
		if (cur_cmd <= TEX_MAX_NON_PREFIXED_COMMAND)
		{
			printErr(tr("You can't use a prefix with `"));
			printCmdChr((quint16)cur_cmd,cur_chr); 
			printChar('\'');
			help1(tr("I'll pretend you didn't say \\long or \\outer or \\global."));
			backError(); 
			return;
		}
		
		if (tracingCommands() > 2)
		{
			if (eTeX_ex)
				showCurCmdChr();
		}
	}
	
	if (a >= 8)
	{
		j = TEX_PROTECTED_TOKEN; 
		a = a - 8;
	}
	else
		j = 0;
		
	if ((cur_cmd != TEX_DEF) && (((a % 4) != 0) || (j != 0)))
	{
		printErr(tr("You can't use `")); 
		printEsc(TeXLong); 
		print(tr("' or `"));
  		printEsc(TeXOuter); 
  		print(tr("' with `"));
  		printCmdChr((quint16)cur_cmd, cur_chr); 
  		printChar('\'');
  		help1(tr("I'll pretend you didn't say \\long or \\outer here."));
  		error();
	}
	
	if (globalDefs() != 0)
	{
		if (globalDefs() < 0)
		{
			if (a >= 4)
				a = a - 4;
		}
		else
		{
			if (a < 4)
				a = a + 4;
		}
	}
	
	switch (cur_cmd)
	{
		case TEX_SET_FONT:
			if (a >= 4)
				geqDefine(TEX_CUR_FONT_LOC, (quint16)TEX_DATA, cur_chr);
			else 
				eqDefine(TEX_CUR_FONT_LOC, (quint16)TEX_DATA, cur_chr);
			break;
			
		case TEX_DEF:
			if (odd(cur_chr) && (a < 4) && (globalDefs() >=0 ))
				a = a + 4;
			e = (cur_chr>=2); 
			getRToken(); 
			p = cur_cs;
			q = scanToks(true, e);
			if (j != 0)
			{
				q = getAvail(); 
				info(q) = j; 
				link(q) = link(def_ref);
    			link(def_ref) = q;
			}
			if (a >= 4)
				geqDefine(p, (quint16)(TEX_CALL + (a % 4)), def_ref);
			else
				eqDefine(p, (quint16)(TEX_CALL + (a % 4)), def_ref);
			break;
			
		case TEX_LET:
			n = cur_chr;
  			getRToken(); 
  			p = cur_cs;
  			if (n == TEX_NORMAL)
  			{
  				do
  				{
  					getToken();
  				} while (cur_cmd == TEX_SPACER);
  				if (cur_tok == (TEX_OTHER_TOKEN + '='))
  				{
  					getToken();
  					if (cur_cmd == TEX_SPACER)
  						getToken();
  				}
  			}
  			else
  			{
  				getToken(); 
  				q = cur_tok; 
  				getToken(); 
  				backInput();
    			cur_tok = q; 
    			backInput();
  			}  			
  			if (cur_cmd >= TEX_CALL)
  				add_token_ref(cur_chr);
  			if (a >= 4)
				geqDefine(p, (quint16)cur_cmd, cur_chr);
			else
				eqDefine(p, (quint16)cur_cmd, cur_chr);
			break;
			
		case TEX_SHORTHAND_DEF:
			n = cur_chr; 
			getRToken(); 
			p = cur_cs;
			if (a >= 4)
				geqDefine(p, (quint16)TEX_RELAX, TEX_TOO_BIG_CHAR);
			else
				eqDefine(p, (quint16)TEX_RELAX, TEX_TOO_BIG_CHAR);
			scanOptionalEquals();
			{
				switch (n)
				{
					case TEX_CHAR_DEF_CODE:
						scanCharNum();
						if (a >= 4)
							geqDefine(p, (quint16)TEX_CHAR_GIVEN, cur_val);
						else
							eqDefine(p, (quint16)TEX_CHAR_GIVEN, cur_val);
						break;
						
					case TEX_MATH_CHAR_DEF_CODE:
						scanRealFifteenBitInt();
						if (a >= 4)
							geqDefine(p, (quint16)TEX_MATH_GIVEN, cur_val);
						else
							eqDefine(p, (quint16)TEX_MATH_GIVEN, cur_val);
						break;
						
					case TEX_OMATH_CHAR_DEF_CODE:
						scanBigFifteenBitInt();
						if (a >= 4)
							geqDefine(p, (quint16)TEX_OMATH_GIVEN, cur_val);
						else
							eqDefine(p, (quint16)TEX_OMATH_GIVEN, cur_val);
						break;
						
					default:
						scanEightBitInt();
						{
							switch (n)
							{
								case TEX_COUNT_DEF_CODE:
									if (a >= 4)
										geqDefine(p, (quint16)TEX_ASSIGN_INT, TEX_COUNT_BASE + cur_val);
									else
										eqDefine(p, (quint16)TEX_ASSIGN_INT, TEX_COUNT_BASE + cur_val);
									break;
									
								case TEX_DIMEN_DEF_CODE:
									if (a >= 4)
										geqDefine(p, (quint16)TEX_ASSIGN_DIMEN, TEX_SCALED_BASE + cur_val);
									else
										eqDefine(p, (quint16)TEX_ASSIGN_DIMEN, TEX_SCALED_BASE + cur_val);
									break;
									
								case TEX_SKIP_DEF_CODE:
									if (a >= 4)
										geqDefine(p, (quint16)TEX_ASSIGN_GLUE, TEX_SKIP_BASE + cur_val);
									else
										eqDefine(p, (quint16)TEX_ASSIGN_GLUE, TEX_SKIP_BASE + cur_val);
									break;
									
								case TEX_MU_SKIP_DEF_CODE:
									if (a >= 4)
										geqDefine(p, (quint16)TEX_ASSIGN_MU_GLUE, TEX_MU_SKIP_BASE + cur_val);
									else
										eqDefine(p, (quint16)TEX_ASSIGN_MU_GLUE, TEX_MU_SKIP_BASE + cur_val);
									break;
									
								case TEX_TOKS_DEF_CODE:
									if (a >= 4)
										geqDefine(p, (quint16)TEX_ASSIGN_TOKS, TEX_TOKS_BASE + cur_val);
									else
										eqDefine(p, (quint16)TEX_ASSIGN_TOKS, TEX_TOKS_BASE + cur_val);
									break;
									
								default:
									break;
							}
						}
						break;
				}
			}
			break;
			
		case TEX_READ_TO_CS:
			j = cur_chr; 
			scanInt(); 
			n = cur_val;
			if (!scanKeyWord(TeXTo))
			{
				printErr(tr("Missing `to' inserted"));
				help2(tr("You should have said `\\read<number> to \\cs'."));
				help_line[0] = tr("I'm going to look for the \\cs now.");
				error();
			}
			getRToken();
  			p = cur_cs; 
  			readToks(n, p, j);
  			if (a >= 4)
				geqDefine(p, (quint16)TEX_CALL, cur_val);
			else
				eqDefine(p, (quint16)TEX_CALL, cur_val);
			break;
			
		case TEX_TOKS_REGISTER:
		case TEX_ASSIGN_TOKS:
			q = cur_cs;
			if (cur_cmd == TEX_TOKS_REGISTER)
			{
				scanEightBitInt(); 
				p = TEX_TOKS_BASE + cur_val;
			}
			else
				p = cur_chr;
			scanOptionalEquals();
			do
			{
				getXToken();
			} while ((cur_cmd == TEX_SPACER) || (cur_cmd == TEX_RELAX));
			if (cur_cmd != TEX_LEFT_BRACE)
			{
				if (cur_cmd == TEX_TOKS_REGISTER)
				{
					scanEightBitInt(); 
					cur_cmd = TEX_ASSIGN_TOKS; 
					cur_chr = TEX_TOKS_BASE + cur_val;
				}
				
				if (cur_cmd == TEX_ASSIGN_TOKS)
				{
					q = equiv(cur_chr);
					if (q == TEX_NULL)
					{
						if (a >= 4)
							geqDefine(p, (quint16)TEX_UNDEFINED_CS, TEX_NULL);
						else
							eqDefine(p, (quint16)TEX_UNDEFINED_CS, TEX_NULL);
					}
					else
					{
						add_token_ref(q);
						if (a >= 4)
							geqDefine(p, (quint16)TEX_CALL, q);
						else
							eqDefine(p, (quint16)TEX_CALL, q);
					}
					
					goto done;
				}
			}
			backInput(); 
			cur_cs = q; 
			q = scanToks(false, false);
			if (link(def_ref) == TEX_NULL)
			{
				if (a >= 4)
					geqDefine(p, (quint16)TEX_UNDEFINED_CS, TEX_NULL);
				else
					eqDefine(p, (quint16)TEX_UNDEFINED_CS, TEX_NULL);
					
				freeAvail(def_ref);
			}
			else
			{
				if (p == TEX_OUTPUT_ROUTINE_LOC)
				{
					link(q) = getAvail(); 
					q = link(q);
      				info(q) = TEX_RIGHT_BRACE_TOKEN + '}';
      				q = getAvail(); 
      				info(q) = TEX_LEFT_BRACE_TOKEN + '{';
      				link(q) = link(def_ref); 
      				link(def_ref) = q;
				}
				
				if (a >= 4)
					geqDefine(p, (quint16)TEX_CALL, def_ref);
				else
					eqDefine(p, (quint16)TEX_CALL, def_ref);
			}
			break;
			
		case TEX_ASSIGN_INT:
			p = cur_chr; 
			scanOptionalEquals(); 
			scanInt();
			if (a >= 4)
				geqWordDefine(p, cur_val);
			else
				eqWordDefine(p, cur_val);
			if ((qAbs(mode) == TEX_HMODE) && 
     			((p ==(TEX_INT_BASE + TEX_LOCAL_INTER_LINE_PENALTY_CODE)) || 
      			(p == (TEX_INT_BASE + TEX_LOCAL_BROKEN_PENALTY_CODE))))
      		{
      			tailAppend(makeLocalParNode());
      			eqWordDefine(TEX_INT_BASE + TEX_NO_LOCAL_WHATSITS_CODE, nolocalWhatsits() + 1);
    			local_par_bool = true;
      		}
			break;
			
		case TEX_ASSIGN_DIR:
			{
				switch (cur_chr)
				{
					case TEX_DIR_BASE + TEX_PAGE_DIRECTION_CODE:
						if (itsAllOver())
						{
							scanDir();
    						eqWordDefine(TEX_DIR_BASE + TEX_PAGE_DIRECTION_CODE, cur_val);
						}
						break;
						
					case TEX_DIR_BASE + TEX_BODY_DIRECTION_CODE:
						if (itsAllOver())
						{
							scanDir();
    						eqWordDefine(TEX_DIR_BASE + TEX_BODY_DIRECTION_CODE, cur_val);
						}
						break;
						
					case TEX_DIR_BASE + TEX_PAR_DIRECTION_CODE:
						scanDir();
  						eqWordDefine(TEX_DIR_BASE + TEX_PAR_DIRECTION_CODE, cur_val);
						break;
						
					case TEX_DIR_BASE + TEX_TEXT_DIRECTION_CODE:
						scanDir();
						if ((nolocalDirs() > 0) && (qAbs(mode) == TEX_HMODE))
						{
							tailAppend(newDir(textDirection()));
							dir_dir(tail) = dir_dir(tail) - 64;
						}
						if (dir_level(text_dir_ptr) == cur_level)
						{
							text_dir_tmp = link(text_dir_ptr);
							freeNode(text_dir_ptr, TEX_DIR_NODE_SIZE);
							text_dir_ptr = text_dir_tmp;
						}
						eqWordDefine(TEX_DIR_BASE + TEX_TEXT_DIRECTION_CODE, cur_val);
						text_dir_tmp = newDir(textDirection());
						link(text_dir_tmp) = text_dir_ptr;
						text_dir_ptr = text_dir_tmp;
						if (qAbs(mode) == TEX_HMODE)
						{
							tailAppend(newDir(textDirection()));
							dir_level(tail) = cur_level;
						}
						eqWordDefine(TEX_INT_BASE + TEX_NO_LOCAL_DIRS_CODE, nolocalDirs() + 1);
  						eqWordDefine(TEX_INT_BASE + TEX_LEVEL_LOCAL_DIR_CODE, cur_level);
						break;
						
					case TEX_DIR_BASE + TEX_MATH_DIRECTION_CODE:
						scanDir();
  						eqWordDefine(TEX_DIR_BASE + TEX_MATH_DIRECTION_CODE, cur_val);
						break;
						
					default:
						break;
				}
			}
			break;
			
		case TEX_ASSIGN_DIMEN:
			p = cur_chr; 
			scanOptionalEquals();
  			scanNormalDimen(); 
  			if (a >= 4)
				geqWordDefine(p, cur_val);
			else
				eqWordDefine(p, cur_val);
			break;
			
		case TEX_ASSIGN_GLUE:
		case TEX_ASSIGN_MU_GLUE:
			p = cur_chr; 
			n = cur_cmd; 
			scanOptionalEquals();
			if (n == TEX_ASSIGN_MU_GLUE)
				scanGlue(TEX_MU_VAL);
			else
				scanGlue(TEX_GLUE_VAL);
			trapZeroGlue();
			if (a >= 4)
				geqDefine(p, (quint16)TEX_GLUE_REF, cur_val);
			else
				eqDefine(p, (quint16)TEX_GLUE_REF, cur_val);
			break;
			
		case TEX_DEF_CODE:
			if (cur_chr == (TEX_DEL_CODE_BASE + 256))
			{
				p = cur_chr - 256; 
				scanCharNum(); 
				p = p + cur_val; 
				scanOptionalEquals();
   				scanInt(); 
   				cur_val1 = cur_val; 
   				scanInt();
   				if ((cur_val1 > 0xFFFFFF) || (cur_val > 0xFFFFFF))
   				{
   					printErr(tr("Invalid code (")); 
   					printInt(cur_val1); 
   					print(" ");
   					printInt(cur_val);
     				print(tr("), should be at most 0xFFFFFF 0xFFFFFF"));
     				help1(tr("I'm going to use 0 instead of that illegal code value."));
     				error(); 
     				cur_val1 = 0; 
     				cur_val = 0;
   				}
   				
   				if (a >= 4)
   					delGeqWordDefine(p, cur_val1, cur_val);
   				else
   					delEqWordDefine(p, cur_val1, cur_val);
			}
			else
			{
				if (cur_chr == TEX_CAT_CODE_BASE)
					n = TEX_MAX_CHAR_CODE;
				else if (cur_chr == TEX_MATH_CODE_BASE)
					n = 0x8000;
				else if (cur_chr == (TEX_MATH_CODE_BASE + 256))
					n = 0x8000000;
				else if (cur_chr == TEX_SF_CODE_BASE)
					n = 0x7FFF;
				else if (cur_chr == TEX_DEL_CODE_BASE)
					n = 0xFFFFFF;
				else
					n = TEX_BIGGEST_CHAR;
					
				p = cur_chr; 
				scanCharNum(); 
				p = p + cur_val; 
				scanOptionalEquals();
				scanInt();
				
				if (((cur_val < 0) && (p < TEX_DEL_CODE_BASE)) || (cur_val > n))
				{
					printErr(tr("Invalid code (")); 
					printInt(cur_val);
					if (p < TEX_DEL_CODE_BASE)
						print(tr("), should be in the range 0.."));
					else
						print(tr("), should be at most "));
						
					printInt(n);
    				help1(tr("I'm going to use 0 instead of that illegal code value."));
    				error(); 
    				cur_val = 0;
				}
				
				if (p < TEX_MATH_CODE_BASE) 
				{
					if (a >= 4)
						geqDefine(p, (quint16)TEX_DATA, cur_val);
					else
						eqDefine(p, (quint16)TEX_DATA, cur_val);
				}
				else if (p < (TEX_MATH_CODE_BASE + 256))
				{
					if (cur_val == 0x8000)
						cur_val = 0x8000000;
					else
						cur_val = ((cur_val / 0x1000) * 0x1000000) + (((cur_val % 0x1000) / 0x100) * 0x10000) + (cur_val % 0x100);
						
					if (a >= 4)
						geqDefine(p, (quint16)TEX_DATA, cur_val);
					else
						eqDefine(p, (quint16)TEX_DATA, cur_val);
				}
				else if (p < TEX_DEL_CODE_BASE)
				{
					if (a >= 4)
						geqDefine(p - 256, (quint16)TEX_DATA, cur_val);
					else
						eqDefine(p - 256, (quint16)TEX_DATA, cur_val);
				}
				else
				{
					cur_val1 = cur_val / 0x1000;
   					cur_val1 = (cur_val1 / 0x100) * 0x10000 + (cur_val1 % 0x100);
   					cur_val  = cur_val % 0x1000;
   					cur_val  = (cur_val / 0x100) * 0x10000 + (cur_val % 0x100);
   					if (a >= 4)
   						delGeqWordDefine(p, cur_val1, cur_val);
   					else
   						delEqWordDefine(p, cur_val1, cur_val);
				}
			}
			break;
			
		case TEX_DEF_FAMILY:
			p = cur_chr; 
			scanBigFourBitInt(); 
			p = p + cur_val;
  			scanOptionalEquals(); 
  			scanFontIdent();
  			if (a >= 4)
				geqDefine(p, (quint16)TEX_DATA, cur_val);
			else
				eqDefine(p, (quint16)TEX_DATA, cur_val);
			break;
			
		case TEX_REGISTER:
		case TEX_ADVANCE:
		case TEX_MULTIPLY:
		case TEX_DIVIDE: 
			doRegisterCommand((quint16)a);
			break;
			
		case TEX_SET_BOX:
			scanEightBitInt();
			if (a >= 4)
				n = TEX_GLOBAL_BOX_FLAG + cur_val;
			else
				n = TEX_BOX_FLAG + cur_val;
			scanOptionalEquals();
			if (set_box_allowed)
				scanBox(n);
			else
			{
				printErr(tr("Improper ")); 
				printEsc(TeXSetBox);
				help2(tr("Sorry, \\setbox is not allowed after \\halign in a display,"));
				help_line[0] = tr("or between \\accent and an accented character.");
				error();
			}
			break;
			
		case TEX_SET_AUX:
			alterAux();
			break;
			
		case TEX_SET_PREV_GRAF:
			alterPrevGraf();
			break;
			
		case TEX_SET_PAGE_DIMEN:
			alterPageSoFar();
			break;
			
		case TEX_SET_PAGE_INT:
			alterInteger();
			break;
			
		case TEX_SET_BOX_DIMEN:
			alterBoxDimen();
			break;
			
		case TEX_SET_SHAPE:
			q = cur_chr; 
			scanOptionalEquals(); 
			scanInt(); 
			n = cur_val;
			if (n <= 0)
				p = TEX_NULL;
			else if (q > TEX_PAR_SHAPE_LOC)
			{
				n = (cur_val / 2) + 1; 
				p = getNode(2 * n + 1); 
				info(p) = n;
    			n = cur_val; 
    			mem[p + 1].ii.CINT0 = n;
    			for (j = p + 2; j <= (p + n + 1); j++)
    			{
    				scanInt(); 
    				mem[j].ii.CINT0 = cur_val;
    			}
    			
    			if (!odd(n))
    				mem[p+n+2].ii.CINT0 = 0;
			}
			else
			{
				p = getNode(2 * n + 1); 
				info(p) = n;
				for (j = 1; j <= n; j++)
				{
					scanNormalDimen();
      				mem[p + 2 * j - 1].ii.CINT0 = cur_val;
      				scanNormalDimen();
      				mem[p + 2 * j].ii.CINT0 = cur_val;
				}
			}
			if (a >= 4)
				geqDefine(q, (quint16)TEX_SHAPE_REF, p);
			else
				eqDefine(q, (quint16)TEX_SHAPE_REF, p);
			break;
			
		case TEX_HYPH_DATA:
			if (cur_chr == 1)
			{
				if (ini_version)
				{
					newPatterns(); 
					goto done;
				}
				
				printErr(tr("Patterns can be loaded only by INIALEPH"));
				help0(); 
				error();
				do
				{
					getToken();
				} while (cur_cmd != TEX_RIGHT_BRACE);
			}
			else
			{
				newHyphExceptions(); 
				goto done;
			}
			break;
			
		case TEX_ASSIGN_FONT_DIMEN: 
			n = cur_chr;
			if (n == TEX_FONT_DIMEN_CODE)
			{
				findFontDimen(true); 
				k = cur_val;
    			scanOptionalEquals(); 
    			scanNormalDimen();
    			font_info(dimen_font, k).ii.CINT0 = cur_val;
			}
			else
			{
				printErr(tr("Invalid dimension assignment"));
    			help2(tr("You tried to assign a character dimension."));
    			help_line[0] = tr("This is not allowed. Proceed; I'll ignore the assignment.");
    			error();
    			scanCharNum(); 
    			scanOptionalEquals(); 
    			scanNormalDimen();
			}
			break;
			
		case TEX_ASSIGN_FONT_INT:
			n = cur_chr; 
			scanFontIdent(); 
			f = cur_val;
  			scanOptionalEquals(); 
  			scanInt();
  			if (n == 0)
  				hyphen_char(f) = cur_val;
  			else
  				skew_char(f) = cur_val;
			break;
			
		case TEX_DEF_FONT:
			newFont((quint16)a);
			break;
			
		case TEX_SET_INTERACTION: 
			newInteraction();
			break;
			
		case TEX_SET_OCP:
			printErr(tr("To use ocps, use the ")); 
			printEsc(TeXPushOcpList);
  			print(" primitive");
  			println();
			break;
			
		case TEX_DEF_OCP: 
			newOcp(a);
			break;
			
		case TEX_SET_OCP_LIST:
			printErr(tr("To use ocp lists, use the "));
  			printEsc(TeXPushOcpList); 
  			print(" primitive");
  			println();
			break;
			
		case TEX_DEF_OCP_LIST: 
			newOcpList(a);
			break;
			
		case TEX_PUSH_OCP_LIST: 
			doPushOcpList(a);
			break;
			
		case TEX_POP_OCP_LIST: 
			doPopOcpList(a);
			break;
			
		case TEX_CLEAR_OCP_LISTS: 
			doClearOcpLists(a);
			break;
			
		case TEX_OCP_LIST_OP:
			printErr(tr("To build ocp lists, use the "));
  			printEsc(TeXOcpList); 
  			print(" primitive"); 
  			println();
			break;
			
		case TEX_OCP_TRACE_LEVEL:
			scanOptionalEquals(); 
			scanInt();
			if (cur_val != 0)
				cur_val = 1;
			if (a >= 4)
				geqDefine(TEX_OCP_TRACE_LEVEL_BASE, (quint16)TEX_DATA, cur_val);
			else
				eqDefine(TEX_OCP_TRACE_LEVEL_BASE, (quint16)TEX_DATA, cur_val);
			break;
			
		default:
			confusion("prefix");
			break;
	}
	
done:
	if (after_token != 0)
	{
		cur_tok = after_token; 
		backInput(); 
		after_token = 0;
	}
}

bool  XWTeX::privileged()
{
	if (mode > 0)
		return true;
		
	reportIllegalCase();
	return false;
}

void XWTeX::pushMath(qint32 c)
{
	if (mathDirection() != textDirection())
		dir_math_save = true;
		
	pushNest(); 
	mode = -TEX_MMODE; 
	incompleat_noad = TEX_NULL;
	saved(0) = text_dir_ptr;
	text_dir_ptr = newDir(mathDirection());
	save_ptr++;
	newSaveLevel(c);
	eqWordDefine(TEX_DIR_BASE + TEX_BODY_DIRECTION_CODE, mathDirection());
	eqWordDefine(TEX_DIR_BASE + TEX_PAR_DIRECTION_CODE, mathDirection());
	eqWordDefine(TEX_DIR_BASE + TEX_TEXT_DIRECTION_CODE, mathDirection());
	eqWordDefine(TEX_INT_BASE + TEX_LEVEL_LOCAL_DIR_CODE, cur_level);
}

void XWTeX::resumeAfterDisplay()
{
	if (cur_group != TEX_MATH_SHIFT_GROUP)
		confusion("display");
		
	unsave();
	save_ptr--;
	flushNodeList(text_dir_ptr);
	text_dir_ptr = saved(0);
	
	prev_graf = prev_graf + 3;
	pushNest(); 
	mode = TEX_HMODE; 
	space_factor = 1000; 
	if (language() <= 0)
		cur_lang = 0;
	else if (language() > TEX_BIGGEST_LANG)
		cur_lang = 0;
	else
		cur_lang = language();; 
	clang = cur_lang;
	
	prev_graf = (normMin(leftHyphenMin()) * 0x40 + normMin(rightHyphenMin())) * 0x10000 + cur_lang;
	tailAppend(makeLocalParNode());
	getXToken();
	if (cur_cmd != TEX_SPACER)
		backInput();
		
	if (nest_ptr == 1)
		buildPage();
}

