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
#include "XWTeX.h"

qint32  XWTeX::finiteShrink(qint32 p)
{
	if (no_shrink_error_yet)
	{
		no_shrink_error_yet = false;
		printErr(tr("Infinite glue shrinkage found in a paragraph"));
		help5(tr("The paragraph just ended includes some glue that has"));
		help_line[3] = tr("infinite shrinkability, e.g., `\\hskip 0pt minus 1fil'.");
		help_line[2] = tr("Such glue doesn't belong there---it allows a paragraph");
		help_line[1] = tr("of any length to fit on one line. But it's safe to proceed,");
		help_line[0] = tr("since the offensive shrinkability has been made finite.");
		error();
	}
	
	qint32 q = newSpec(p); 
	shrink_order(q) = (quint16)TEX_NORMAL;
	deleteGlueRef(p);
	return q;
}

void XWTeX::lineBreak(bool d)
{
	bool auto_breaking;
	qint32 prev_p, q, r, s, prev_s, f, j, c;
	
	pack_begin_line = mode_line;
	
	link(temp_head) = link(head);
	if (isCharNode(tail))
		tailAppend(newPenalty(TEX_INF_PENALTY));
	else if (type(tail) != (quint16)TEX_GLUE_NODE)
		tailAppend(newPenalty(TEX_INF_PENALTY));
	else
	{
		type(tail) = (quint16)TEX_PENALTY_NODE; 
		deleteGlueRef(glue_ptr(tail));
  		flushNodeList(leader_ptr(tail)); 
  		penalty(tail) = TEX_INF_PENALTY;
	}
	
	final_par_glue = newParamGlue(TEX_PAR_FILL_SKIP_CODE);
	link(tail) = final_par_glue;
	last_line_fill = link(tail);
	init_cur_lang = prev_graf % 0x10000;
	init_l_hyf = prev_graf / 0x400000;
	init_r_hyf = (prev_graf / 0x10000) % 0x40;
	paragraph_dir = local_par_dir(link(head));
	line_break_dir = paragraph_dir;
	dir_ptr = TEX_NULL; 
	dir_tmp = newDir(paragraph_dir);
	link(dir_tmp) = dir_ptr; 
	dir_ptr = dir_tmp;
	popNest();
	
	no_shrink_error_yet = true;
	if ((shrink_order(leftSkip()) != (quint16)TEX_NORMAL) && (shrink(leftSkip()) != 0))
		setEquiv(TEX_GLUE_BASE + TEX_LEFT_SKIP_CODE, finiteShrink(leftSkip()));
		
	if ((shrink_order(rightSkip()) != (quint16)TEX_NORMAL) && (shrink(rightSkip()) != 0))
		setEquiv(TEX_GLUE_BASE + TEX_RIGHT_SKIP_CODE, finiteShrink(rightSkip()));
		
	q = leftSkip(); 
	r = rightSkip(); 
	background[1] = width(q) + width(r);
	background[2] = 0; 
	background[3] = 0; 
	background[4] = 0; 
	background[5] = 0;
	background[6] = 0;
	background[2 + stretch_order(q)] = stretch(q);
	background[2 + stretch_order(r)] = background[2 + stretch_order(r)] + stretch(r);
	background[7] = shrink(q) + shrink(r);
	do_last_line_fit = false; 
	active_node_size = TEX_ACTIVE_NODE_SIZE_NORMAL;
	if (lastLineFit() > 0)
	{
		q = glue_ptr(last_line_fill);
		if ((stretch(q) > (quint16)0) && (stretch_order(q) > (quint16)TEX_NORMAL))
		{
			if ((background[3] == 0) && 
				(background[4] == 0) && 
				(background[5] == 0))
			{
				do_last_line_fit = true;
				active_node_size = TEX_ACTIVE_NODE_SIZE_EXTENDED;
    			fill_width[0] = 0; 
    			fill_width[1] = 0; 
    			fill_width[2] = 0;
    			fill_width[stretch_order(q) - 1] = stretch(q);
			}
		}
	}
	
	minimum_demerits = TEX_AWFUL_BAD;
	minimal_demerits[TEX_TIGHT_FIT] = TEX_AWFUL_BAD;
	minimal_demerits[TEX_DECENT_FIT] = TEX_AWFUL_BAD;
	minimal_demerits[TEX_LOOSE_FIT] = TEX_AWFUL_BAD;
	minimal_demerits[TEX_VERY_LOOSE_FIT] = TEX_AWFUL_BAD;
	
	if (parShapePtr() == TEX_NULL)
	{
		if (hangIndent() == 0)
		{
			last_special_line = 0; 
			second_width  = hsize();
    		second_indent = 0;
		}
		else
		{
			last_special_line = qAbs(hangAfter());
			if (hangAfter() < 0)
			{
				first_width = hsize() - qAbs(hangIndent());
				if (hangIndent() >= 0)
					first_indent = hangIndent();
				else
					first_indent = 0;
			}
			else
			{
				first_width  = hsize(); 
				first_indent = 0;
				second_width = hsize() - qAbs(hangIndent());
				if (hangIndent() >= 0)
					second_indent = hangIndent();
				else
					second_indent = 0;
			}
		}
	}
	else
	{
		last_special_line = info(parShapePtr()) - 1;
  		second_width  = mem[parShapePtr() + 2 * (last_special_line + 1)].ii.CINT0;
  		second_indent = mem[parShapePtr() + 2 * last_special_line + 1].ii.CINT0;
	}
	
	if (looseness() == 0)
		easy_line = last_special_line;
	else
		easy_line = TEX_MAX_HALFWORD;
		
	threshold = pretolerance();	
	if (threshold >= 0)
	{
#ifdef XW_TEX_STAT
		
		if (tracingParagraphs() > 0)
		{
			beginDiagnostic(); 
			printnl("@@firstpass");
		}
		
#endif //XW_TEX_STAT

		second_pass = false; 
		final_pass = false;
	}
	else
	{
		threshold = tolerance(); 
		second_pass =true;
        final_pass = (emergencyStretch() <= 0);
        
#ifdef XW_TEX_STAT
		if (tracingParagraphs() > 0)
			beginDiagnostic(); 
#endif //XW_TEX_STAT
	}
	
	while (true)
	{
		if (threshold > TEX_INF_BAD)
			threshold = TEX_INF_BAD;
			
		if (second_pass)
		{
			if (ini_version)
			{
				if (trie_not_ready)
					initTrie();
			}
			
			cur_lang = init_cur_lang; 
			l_hyf = init_l_hyf; 
			r_hyf = init_r_hyf;
			if (trie_char(hyph_start + cur_lang) != (quint16)cur_lang)
				hyph_index = 0;
			else
				hyph_index = trie_link(hyph_start + cur_lang);
		}
		
		q = getNode(active_node_size);
		type(q) = (quint16)TEX_UNHYPHENATED; 
		fitness(q) = (quint16)TEX_DECENT_FIT;
		link(q) = last_active; 
		break_node(q) = TEX_NULL;
		line_number(q) = prev_graf + 1; 
		total_demerits(q) = 0; 
		link(active) = q;
		
		if (do_last_line_fit)
		{
			active_short(q) = 0; 
			active_glue(q)  = 0;
		}
		
		active_width[1] = background[1];
		active_width[2] = background[2];
		active_width[3] = background[3];
		active_width[4] = background[4];
		active_width[5] = background[5];
		active_width[6] = background[6];
		active_width[7] = background[7];
		
		passive = TEX_NULL; 
		printed_node = temp_head; 
		pass_number  = 0;
		font_in_short_display = TEX_NULL_FONT;
		cur_p = link(temp_head); 
		auto_breaking = true;
		prev_p = cur_p;
		
		if (subtype(cur_p) == (quint16)TEX_LOCAL_PAR_NODE)
		{
			internal_pen_inter = local_pen_inter(cur_p);
  			internal_pen_broken = local_pen_broken(cur_p);
  			init_internal_left_box = local_box_left(cur_p);
  			init_internal_left_box_width = local_box_left_width(cur_p);
  			internal_left_box = init_internal_left_box;
  			internal_left_box_width = init_internal_left_box_width;
  			internal_right_box = local_box_right(cur_p);
  			internal_right_box_width = local_box_right_width(cur_p);
		}
		
		while ((cur_p != TEX_NULL) && (link(active) != last_active))
		{
			if (isCharNode(cur_p))
			{
				prev_p = cur_p;
				do
				{
					f = font(cur_p);
					if (is_rotated(line_break_dir))
					{
						break_c_htdp = height_depth(char_info(f, character(cur_p)));
    					act_width = act_width + char_height(f, break_c_htdp) + char_depth(f, break_c_htdp);
					}
					else
						act_width = act_width + char_width(f, char_info(f, character(cur_p)));
						
					cur_p = link(cur_p);
				} while (isCharNode(cur_p));
			}
			
			switch (type(cur_p))
			{
				case TEX_HLIST_NODE:
				case TEX_VLIST_NODE:
					if (!(dir_orthogonal(dir_primary[box_dir(cur_p)], dir_primary[line_break_dir])))
						act_width = act_width + width(cur_p);
					else
						act_width = act_width +(depth(cur_p) + height(cur_p));
					break;
					
				case TEX_RULE_NODE: 
					act_width = act_width + width(cur_p);
					break;
					
				case TEX_WHATSIT_NODE:
					if (subtype(cur_p) == (quint16)TEX_LANGUAGE_NODE)
					{
						cur_lang = what_lang(cur_p); 
						l_hyf = what_lhm(cur_p); 
						r_hyf = what_rhm(cur_p);
					}
					else if (subtype(cur_p) == (quint16)TEX_LOCAL_PAR_NODE)
					{
						internal_pen_inter  = local_pen_inter(cur_p);
  						internal_pen_broken = local_pen_broken(cur_p);
  						internal_left_box   = local_box_left(cur_p);
  						internal_left_box_width = local_box_left_width(cur_p);
  						internal_right_box = local_box_right(cur_p);
  						internal_right_box_width = local_box_right_width(cur_p);
					}
					else if (subtype(cur_p) == (quint16)TEX_DIR_NODE)
					{
						if (dir_dir(cur_p) >= 0)
						{
							line_break_dir = dir_dir(cur_p);
  							dir_tmp = getNode(TEX_DIR_NODE_SIZE);
							type(dir_tmp) = (quint16)TEX_WHATSIT_NODE; 
							subtype(dir_tmp) = (quint16)TEX_DIR_NODE;
							dir_dir(dir_tmp) = dir_dir(cur_p);
							dir_level(dir_tmp) = dir_level(cur_p);
							dir_dvi_h(dir_tmp) = dir_dvi_h(cur_p);
							dir_dvi_ptr(dir_tmp) = dir_dvi_ptr(cur_p);
							link(dir_tmp) = dir_ptr; 
							dir_ptr = dir_tmp;
						}
						else
						{
							dir_tmp = dir_ptr;
							dir_ptr = link(dir_tmp);
							freeNode(dir_tmp, TEX_DIR_NODE_SIZE);
  							line_break_dir = dir_dir(dir_ptr);
						}
					}
					break;
					
				case TEX_GLUE_NODE:
					if (auto_breaking)
					{
						if (isCharNode(prev_p))
							tryBreak(0, TEX_UNHYPHENATED);
						else if (precedesBreak(prev_p))
							tryBreak(0, TEX_UNHYPHENATED);
						else if ((type(prev_p) == (quint16)TEX_KERN_NODE) && (subtype(prev_p) != (quint16)TEX_EXPLICIT))
							tryBreak(0, TEX_UNHYPHENATED);
					}
					if ((shrink_order(glue_ptr(cur_p)) != (quint16)TEX_NORMAL) && (shrink(glue_ptr(cur_p)) != 0))
						glue_ptr(cur_p) = finiteShrink(glue_ptr(cur_p));
					q = glue_ptr(cur_p);
					act_width = act_width + width(q);
					active_width[2 + stretch_order(q)] = active_width[2 + stretch_order(q)] + stretch(q);
					active_width[7] = active_width[7] + shrink(q);
					if (second_pass && auto_breaking)
					{
						prev_s = cur_p; 
						s = link(prev_s);
						if (s != TEX_NULL)
						{
							while (true)
							{
								if (isCharNode(s))
								{
									c  = character(s); 
									hf = font(s);
								}
								else if (type(s) == (quint16)TEX_LIGATURE_NODE)
								{
									if (lig_ptr(s) == TEX_NULL)
										goto tcontinue;
									else
									{
										q = lig_ptr(s); 
										c = character(q); 
										hf = font(q);
									}
								}
								else if ((type(s) == (quint16)TEX_KERN_NODE) && (subtype(s) == (quint16)TEX_NORMAL))
									goto tcontinue;
								else if (type(s) == (quint16)TEX_WHATSIT_NODE)
								{
									if (subtype(s) == (quint16)TEX_LANGUAGE_NODE)
									{
										cur_lang = what_lang(s); 
										l_hyf = what_lhm(s); 
										r_hyf = what_rhm(s);
  										if (trie_char(hyph_start + cur_lang) != (quint16)cur_lang)
											hyph_index = 0;
										else
											hyph_index = trie_link(hyph_start + cur_lang);
									}
									
									goto tcontinue;
								}
								else
									goto done1;
									
								if (hyph_index == 0)
									hc[0] = lcCode(c);
								else if (trie_char(hyph_index + c) != (quint16)c)
									hc[0] = 0;
								else
									hc[0] = trie_op(hyph_index + c);
									
								if (hc[0] != 0)
								{
									if ((hc[0] == c) || (ucHyph() > 0))
										goto done2;
									else
										goto done1;
								}
								
tcontinue: 
								prev_s = s; 
								s = link(prev_s);
							}
							
done2: 
							hyf_char = hyphen_char(hf);
							if (hyf_char < 0)
								goto done1;
								
							if (hyf_char > TEX_BIGGEST_CHAR)
								goto done1;
								
							ha = prev_s;
							
							if ((l_hyf + r_hyf) > 63)
								goto done1;
								
							hn = 0;
							while (true)
							{
								if (isCharNode(s))
								{
									if (font(s) != (quint16)hf)
										goto done3;
										
									hyf_bchar = character(s); 
									c = hyf_bchar;
									
									if (hyph_index == 0)
										hc[0] = lcCode(c);
									else if (trie_char(hyph_index + c) != (quint16)c)
										hc[0] = 0;
									else
										hc[0] = trie_op(hyph_index + c);
										
									if (hc[0] == 0)
										goto done3;
										
									if (lcCode(c) > max_hyph_char)
										goto done3;
										
									if (hn == 63)
										goto done3;
										
									hb = s; 
									hn++; 
									hu[hn] = c; 
									hc[hn] = hc[0]; 
									hyf_bchar = TEX_NON_CHAR;
								}
								else if (type(s) == (quint16)TEX_LIGATURE_NODE)
								{
									if (font(lig_char(s)) != (quint16)hf)
										goto done3;
										
									j = hn; 
									q = lig_ptr(s);
									if (q > TEX_NULL)
										hyf_bchar = character(q);
										
									while (q > TEX_NULL)
									{
										c = character(q);
										if (hyph_index == 0)
											hc[0] = lcCode(c);
										else if (trie_char(hyph_index + c) != (quint16)c)
											hc[0] = 0;
										else
											hc[0] = trie_op(hyph_index + c);
											
										if (hc[0] == 0)
											goto done3;
											
										if (lcCode(c) > max_hyph_char)
											goto done3;
											
										if (j == 63)
											goto done3;
											
										j++; 
										hu[j] = c; 
										hc[j] = hc[0];
										
										q = link(q);
									}
									
									hb = s; 
									hn = j;
									
									if (odd(subtype(s)))
										hyf_bchar = font_bchar(hf);
									else
										hyf_bchar = TEX_NON_CHAR;
								}
								else if ((type(s) == (quint16)TEX_KERN_NODE) && (subtype(s) == (quint16)TEX_NORMAL))
								{
									hb = s;
    								hyf_bchar = font_bchar(hf);
								}
								else
									goto done3;
									
								s = link(s);
							}
							
done3:
							if (hn < (l_hyf + r_hyf))
								goto done1;
								
							while (true)
							{
								if (!(isCharNode(s)))
								{
									switch (type(s))
									{
										case TEX_LIGATURE_NODE:
											break;
											
										case TEX_KERN_NODE:
											if (subtype(s) != (quint16)TEX_NORMAL)
												goto done4;
											break;
											
										case TEX_WHATSIT_NODE:
										case TEX_GLUE_NODE:
										case TEX_PENALTY_NODE:
										case TEX_INS_NODE:
										case TEX_ADJUST_NODE:
										case TEX_MARK_NODE:
											goto done4;
											break;
											
										default:
											goto done1;
											break;
									}
								}
								
								s = link(s);
							}
done4:
							hyphenate();
						}
						
done1:
						;
					}
					break;
					
				case TEX_KERN_NODE:
					if (subtype(cur_p) == (quint16)TEX_EXPLICIT)
					{
						if (!isCharNode(link(cur_p)) && auto_breaking)
						{
							if (type(link(cur_p)) == (quint16)TEX_GLUE_NODE)
								tryBreak(0, TEX_UNHYPHENATED);
						}
						
						act_width = act_width + width(cur_p);
					}
					else
						act_width = act_width + width(cur_p);
					break;
					
				case TEX_LIGATURE_NODE:
					f = font(lig_char(cur_p));
					if (is_rotated(line_break_dir))
					{
						break_c_htdp = height_depth(char_info(f, character(lig_char(cur_p))));
    					act_width = act_width+char_height(f, break_c_htdp) + char_depth(f, break_c_htdp);
					}
					else
						act_width = act_width + char_width(f, char_info(f, character(lig_char(cur_p))));
					break;
					
				case TEX_DISC_NODE:
					s = pre_break(cur_p); 
					disc_width = 0;
					if (s == TEX_NULL)
						tryBreak(exhyphenPenalty(), TEX_HYPHENATED);
					else
					{
						do
						{
							if (isCharNode(s))
							{
								f = font(s);
								if (is_rotated(line_break_dir))
								{
									break_c_htdp = height_depth(char_info(f, character(s)));
      								disc_width = disc_width + char_height(f, break_c_htdp) + char_depth(f, break_c_htdp);
								}
								else
									disc_width = disc_width + char_width(f, char_info(f, character(s)));
							}
							else
							{
								switch (type(s))
								{
									case TEX_LIGATURE_NODE:
										f = font(lig_char(s));
										if (is_rotated(line_break_dir))
										{
											break_c_htdp = height_depth(char_info(f, character(lig_char(s))));
        									disc_width = disc_width + char_height(f, break_c_htdp) + char_depth(f, break_c_htdp);
										}
										else
											disc_width = disc_width + char_width(f, char_info(f, character(lig_char(s))));
										break;
										
									case TEX_HLIST_NODE:
									case TEX_VLIST_NODE:
										if (!(dir_orthogonal(dir_primary[box_dir(s)], dir_primary[line_break_dir])))
											disc_width = disc_width + width(s);
										else
											disc_width = disc_width + (depth(s) + height(s));
										break;
										
									case TEX_RULE_NODE:
									case TEX_KERN_NODE:
										disc_width = disc_width + width(s);
										break;
										
									default:
										confusion("disc3");
										break;
								}
							}
							
							s = link(s);
						} while (s != TEX_NULL);
						
						act_width = act_width + disc_width;
  						tryBreak(hyphenPenalty(), TEX_HYPHENATED);
  						act_width = act_width - disc_width;
					}
					
					r = replace_count(cur_p); 
					s = link(cur_p);
					while (r > 0)
					{
						if (isCharNode(s))
						{
							f = font(s);
							if (is_rotated(line_break_dir))
							{
								break_c_htdp = height_depth(char_info(f, character(s)));
      							act_width = act_width + char_height(f, break_c_htdp) + char_depth(f, break_c_htdp);
							}
							else
								act_width = act_width + char_width(f, char_info(f, character(s)));
						}
						else
						{
							switch (type(s))
							{
								case TEX_LIGATURE_NODE:
									f = font(lig_char(s));
									if (is_rotated(line_break_dir))
									{
										break_c_htdp = height_depth(char_info(f, character(lig_char(s))));
        								act_width = act_width + char_height(f, break_c_htdp) + char_depth(f, break_c_htdp);
									}
									else
										act_width = act_width + char_width(f, char_info(f, character(lig_char(s))));
									break;
									
								case TEX_HLIST_NODE:
								case TEX_VLIST_NODE:
									if (!(dir_orthogonal(dir_primary[box_dir(s)], dir_primary[line_break_dir])))
										act_width = act_width + width(s);
									else
										act_width = act_width +(depth(s) + height(s));
									break;
									
								case TEX_RULE_NODE:
								case TEX_KERN_NODE:
									act_width = act_width + width(s);
									break;
									
								default:
									confusion("disc4");
									break;
							}
						}
						r--;
						s = link(s);
					}
					
					prev_p = cur_p; 
					cur_p = s; 
					goto done5;
					break;
					
				case TEX_MATH_NODE:
					auto_breaking = (subtype(cur_p) == (quint16)TEX_AFTER); 
					if (!isCharNode(link(cur_p)) && auto_breaking)
					{
						if (type(link(cur_p)) == (quint16)TEX_GLUE_NODE)
							tryBreak(0, TEX_UNHYPHENATED);
					}
						
					act_width = act_width + width(cur_p);
					break;
					
				case TEX_PENALTY_NODE: 
					tryBreak(penalty(cur_p), TEX_UNHYPHENATED);
					break;
					
				case TEX_MARK_NODE:
				case TEX_INS_NODE:
				case TEX_ADJUST_NODE:
					break;
					
				default:
					confusion("paragraph");
					break;
			}
			
			prev_p = cur_p; 
			cur_p = link(cur_p);
done5:		
			;
		}
		
		if (cur_p == TEX_NULL)
		{
			tryBreak(TEX_EJECT_PENALTY, TEX_HYPHENATED);
			if (link(active) != last_active)
			{
				r = link(active); 
				fewest_demerits = TEX_AWFUL_BAD;
				do
				{
					if (type(r) != (quint16)TEX_DELTA_NODE)
					{
						if (total_demerits(r) < fewest_demerits)
						{
							fewest_demerits = total_demerits(r); 
							best_bet = r;
						}
					}
					
					r = link(r);
				} while (r != last_active);
				
				best_line = line_number(best_bet);
				if (looseness() == 0)
					goto done;
					
				r = link(active); 
				actual_looseness = 0;
				do
				{
					if (type(r) != (quint16)TEX_DELTA_NODE)
					{
						line_diff = line_number(r)-best_line;
						if (((line_diff < actual_looseness) && 
							(looseness() <= line_diff)) || ((line_diff > actual_looseness) && 
							(looseness() >= line_diff)))
						{
							best_bet = r; 
							actual_looseness = line_diff;
    						fewest_demerits = total_demerits(r);
						}
						else if ((line_diff == actual_looseness) && 
							     (total_demerits(r) < fewest_demerits))
						{
							best_bet = r; 
							fewest_demerits = total_demerits(r);
						}
					}
					
					r = link(r);
				} while (r != last_active);
				
				best_line = line_number(best_bet);
				
				if ((actual_looseness == looseness()) || final_pass)
					goto done;
			}
		}
		
		q = link(active);
		while (q != last_active)
		{
			cur_p = link(q);
			if (type(q) == (quint16)TEX_DELTA_NODE)
				freeNode(q, TEX_DELTA_NODE_SIZE);
			else
				freeNode(q, active_node_size);
				
			q = cur_p;
		}
		
		q = passive;
		while (q != TEX_NULL)
		{
			cur_p = link(q);
  			freeNode(q, TEX_PASSIVE_NODE_SIZE);
  			q = cur_p;
		}
		
		if (!second_pass)
		{
#ifdef XW_TEX_STAT

			if (tracingParagraphs() > 0)
				printnl("@@secondpass");
			
#endif //XW_TEX_STAT

			threshold = tolerance(); 
			second_pass = true; 
			final_pass = (emergencyStretch() <= 0);
		}
		else
		{
#ifdef XW_TEX_STAT
			
			if (tracingParagraphs() > 0)
				printnl("@@emergencypass");
				
#endif //XW_TEX_STAT

			background[2] = background[2] + emergencyStretch(); 
			final_pass = true;
		}
	}
	
done:
	
#ifdef XW_TEX_STAT
	if (tracingParagraphs() > 0)
	{
		endDiagnostic(true); 
		normalizeSelector();
	}
#endif //XW_TEX_STAT

	if (do_last_line_fit)
	{
		if (active_short(best_bet) == 0)
			do_last_line_fit = false;
		else
		{
			q = newSpec(glue_ptr(last_line_fill));
  			deleteGlueRef(glue_ptr(last_line_fill));
  			width(q) = width(q) + active_short(best_bet) - active_glue(best_bet);
  			stretch(q) = 0; 
  			glue_ptr(last_line_fill) = q;
		}
	}
	
	postLineBreak(d);
	
	q = link(active);
	while (q != last_active)
	{
		cur_p = link(q);
		if (type(q) == (quint16)TEX_DELTA_NODE)
			freeNode(q, TEX_DELTA_NODE_SIZE);
		else
			freeNode(q, active_node_size);
			
		q = cur_p;
	}
		
	q = passive;
	while (q != TEX_NULL)
	{
		cur_p = link(q);
  		freeNode(q, TEX_PASSIVE_NODE_SIZE);
  		q = cur_p;
	}
	
	pack_begin_line = 0;
}

void XWTeX::postLineBreak(bool d)
{
	qint32 q, r, s;
	bool disc_break, post_disc_break;
	qint32 cur_width, cur_indent, t, pen, cur_line;
	
	dir_ptr = dir_save;
	q = break_node(best_bet); 
	cur_p = TEX_NULL;
	do
	{
		r = q; 
		q = prev_break(q); 
		next_break(r) = cur_p; 
		cur_p = r;
	} while (q != TEX_NULL);
	
	cur_line = prev_graf + 1;
	do
	{
		dir_rover = dir_ptr;
		while (dir_rover != TEX_NULL)
		{
			dir_tmp = newDir(dir_dir(dir_rover)); 
			link(dir_tmp) = link(temp_head);
  			link(temp_head) = dir_tmp; 
  			dir_rover = link(dir_rover);
		}
		
		flushNodeList(dir_ptr); 
		dir_ptr = TEX_NULL;
		q = link(temp_head);
		while (q != cur_break(cur_p))
		{
			if (!isCharNode(q))
			{
				if (type(q) == (quint16)TEX_WHATSIT_NODE)
				{
					if (subtype(q) == (quint16)TEX_DIR_NODE)
					{
						if (dir_dir(q) >= 0)
						{
							dir_tmp = getNode(TEX_DIR_NODE_SIZE);
							type(dir_tmp) = (quint16)TEX_WHATSIT_NODE; 
							subtype(dir_tmp) = (quint16)TEX_DIR_NODE;
							dir_dir(dir_tmp) = dir_dir(q);
							dir_level(dir_tmp) = dir_level(q);
							dir_dvi_h(dir_tmp) = dir_dvi_h(q);
							dir_dvi_ptr(dir_tmp) = dir_dvi_ptr(q);
							link(dir_tmp) = dir_ptr; 
							dir_ptr = dir_tmp;
						}
						else if (dir_ptr != TEX_NULL)
						{
							if (dir_dir(dir_ptr) == (dir_dir(q) + 64))
							{
								dir_tmp = dir_ptr;
								dir_ptr = link(dir_tmp);
								freeNode(dir_tmp, TEX_DIR_NODE_SIZE);
							}
						}
					}
				}
			}
			
			q = link(q);
		}
		
		q = cur_break(cur_p); 
		disc_break = false; 
		post_disc_break = false;
		
		if (q != TEX_NULL)
		{
			if (type(q) == (quint16)TEX_GLUE_NODE)
			{
				if (passive_right_box(cur_p) != TEX_NULL)
				{
					r = temp_head;
					while (link(r) != q)
						r = link(r);
						
					if (dir_ptr != TEX_NULL)
					{
						if (link(r) == q)
						{
							dir_rover = dir_ptr;
							while (dir_rover != TEX_NULL)
							{
								dir_tmp = newDir(dir_dir(dir_rover) - 64);
      							link(dir_tmp) = q;
      							link(r) = dir_tmp;
      							r = dir_tmp;
      							dir_rover = link(dir_rover);
							}
						}
						else if (r == final_par_glue)
						{
							s = temp_head;
							while (link(s) != r)
								s = link(s);
							dir_rover = dir_ptr;
							while (dir_rover != TEX_NULL)
							{
								dir_tmp = newDir(dir_dir(dir_rover) - 64);
      							link(dir_tmp) = q;
      							link(s) = dir_tmp;
      							s = dir_tmp;
      							dir_rover = link(dir_rover);
							}
						}
						else
						{
							dir_rover = dir_ptr;
							s = link(q);
							while (dir_rover != TEX_NULL)
							{
								dir_tmp = newDir(dir_dir(dir_rover) - 64);
      							link(dir_tmp) = s;
      							link(q) = dir_tmp;
      							q = dir_tmp;
      							dir_rover = link(dir_rover);
							}
							
							r = q;
						}
					}
					
					s = copyNodeList(passive_right_box(cur_p));
      				link(r) = s;
      				link(s) = q;
				}
				else
				{
					r = temp_head;
					while (link(r) != q)
						r = link(r);
						
					if (dir_ptr != TEX_NULL)
					{
						if (link(r) == q)
						{
							dir_rover = dir_ptr;
							while (dir_rover != TEX_NULL)
							{
								dir_tmp = newDir(dir_dir(dir_rover) - 64);
      							link(dir_tmp) = q;
      							link(r) = dir_tmp;
      							r = dir_tmp;
      							dir_rover = link(dir_rover);
							}
						}
						else if (r == final_par_glue)
						{
							s = temp_head;
							while (link(s) != r)
								s = link(s);
							dir_rover = dir_ptr;
							while (dir_rover != TEX_NULL)
							{
								dir_tmp = newDir(dir_dir(dir_rover) - 64);
      							link(dir_tmp) = q;
      							link(s) = dir_tmp;
      							s = dir_tmp;
      							dir_rover = link(dir_rover);
							}
						}
						else
						{
							dir_rover = dir_ptr;
							s = link(q);
							while (dir_rover != TEX_NULL)
							{
								dir_tmp = newDir(dir_dir(dir_rover) - 64);
      							link(dir_tmp) = s;
      							link(q) = dir_tmp;
      							q = dir_tmp;
      							dir_rover = link(dir_rover);
							}
							
							r = q;
						}
					}
				}
				
				deleteGlueRef(glue_ptr(q));
    			glue_ptr(q) = rightSkip();
    			subtype(q) = (quint16)(TEX_RIGHT_SKIP_CODE + 1); 
    			add_glue_ref(rightSkip());
    			goto done;
			}
			else
			{
				if (type(q) == (quint16)TEX_DISC_NODE)
				{
					t = replace_count(q);
					if (t == 0)
						r = link(q);
					else
					{
						r = q;
						while (t > 1)
						{
							r = link(r); 
							t--;
						}
						
						s = link(r);
  						r = link(s); 
  						link(s) = TEX_NULL;
  						flushNodeList(link(q)); 
  						replace_count(q) = (quint16)0;
					}
					
					if (post_break(q) != TEX_NULL)
					{
						s = post_break(q);
						while (link(s) != TEX_NULL)
							s = link(s);
							
						link(s) = r; 
						r = post_break(q); 
						post_break(q) = TEX_NULL; 
						post_disc_break = true;
					}
					
					if (pre_break(q) != TEX_NULL)
					{
						s = pre_break(q); 
						link(q) = s;
						while (link(s) != TEX_NULL)
							s = link(s);
						pre_break(q) = TEX_NULL; 
						q = s;
					}
					
					link(q) = r; 
					disc_break = true;
				}
				else if ((type(q) == (quint16)TEX_MATH_NODE) || (type(q) == (quint16)TEX_KERN_NODE))
					width(q) = 0;
			}
		}
		else
		{
			q = temp_head;
			while (link(q) != TEX_NULL)
				q = link(q);
		}
		
		r = q;
		if (dir_ptr != TEX_NULL)
		{
			if (link(r) == q)
			{
				dir_rover = dir_ptr;
				while (dir_rover != TEX_NULL)
				{
					dir_tmp = newDir(dir_dir(dir_rover) - 64);
      				link(dir_tmp) = q;
      				link(r) = dir_tmp;
      				r = dir_tmp;
      				dir_rover = link(dir_rover);
				}
			}
			else if (r == final_par_glue)
			{
				s = temp_head;
				while (link(s) != r)
					s = link(s);
				dir_rover = dir_ptr;
				while (dir_rover != TEX_NULL)
				{
					dir_tmp = newDir(dir_dir(dir_rover) - 64);
      				link(dir_tmp) = q;
      				link(s) = dir_tmp;
      				s = dir_tmp;
      				dir_rover = link(dir_rover);
				}
			}
			else
			{
				dir_rover = dir_ptr;
				s = link(q);
				while (dir_rover != TEX_NULL)
				{
					dir_tmp = newDir(dir_dir(dir_rover) - 64);
      				link(dir_tmp) = s;
      				link(q) = dir_tmp;
      				q = dir_tmp;
      				dir_rover = link(dir_rover);
				}
						
				r = q;
			}
		}
		
		if (passive_right_box(cur_p) != TEX_NULL)
		{
			r = copyNodeList(passive_right_box(cur_p));
  			link(r) = link(q);
  			link(q) = r;
  			q = r;
		}
		
		r = newParamGlue(TEX_RIGHT_SKIP_CODE); 
		link(r) = link(q); 
		link(q) = r; 
		q = r;
		
done:
		r = link(q); 
		link(q) = TEX_NULL; 
		q = link(temp_head); 
		link(temp_head) = r;
		if (passive_left_box(cur_p) != TEX_NULL)
		{
			r = copyNodeList(passive_left_box(cur_p));
  			s = link(q);
  			link(r) = q;
  			q = r;
  			if ((cur_line == (prev_graf + 1)) && (s != TEX_NULL))
  			{
  				if (type(s) == (quint16)TEX_HLIST_NODE)
  				{
  					if (list_ptr(s) == TEX_NULL)
  					{
  						q = link(q);
        				link(r) = link(s);
        				link(s) = r;
  					}
  				}
  			}
		}
		
		if (leftSkip() != zero_glue)
		{
			r = newParamGlue(TEX_LEFT_SKIP_CODE);
  			link(r) = q; 
  			q = r;
		}
		
		if (cur_line > last_special_line)
		{
			cur_width  = second_width; 
			cur_indent = second_indent;
		}
		else if (parShapePtr() == TEX_NULL)
		{
			cur_width  = first_width; 
			cur_indent = first_indent;
		}
		else
		{
			cur_width  = mem[parShapePtr() + 2 * cur_line].ii.CINT0;
  			cur_indent = mem[parShapePtr() + 2 * cur_line-1].ii.CINT0;
		}
		
		adjust_tail    = adjust_head;
		pack_direction = paragraph_dir;
		just_box = hpack(q, cur_width, TEX_EXACTLY);
		shift_amount(just_box) = cur_indent;
		
		appendToVList(just_box);
		if (adjust_head != adjust_tail)
		{
			link(tail) = link(adjust_head); 
			tail = adjust_tail;
		}
		
		adjust_tail = TEX_NULL;
		
		if ((cur_line + 1) != best_line)
		{
			q = interLinePenaltiesPtr();
			if (q != TEX_NULL)
			{
				r = cur_line;
				if (r > penalty(q))
					r = penalty(q);
					
				pen = penalty(q+r);
			}
			else
			{
				if (passive_pen_inter(cur_p) != 0)
					pen = passive_pen_inter(cur_p);
				else
					pen = interLinePenalty();
			}
			
			q = clubPenaltiesPtr();
			if (q != TEX_NULL)
			{
				r = cur_line - prev_graf;
				if (r > penalty(q))
					r = penalty(q);
					
				pen = pen + penalty(q + r);
			}
			else if (cur_line == (prev_graf + 1))
				pen = pen + clubPenalty();
				
			if (d)
				q = displayWidowPenaltiesPtr();
			else
				q = widowPenaltiesPtr();
				
			if (q != TEX_NULL)
			{
				r = best_line - cur_line - 1;
				if (r > penalty(q))
					r = penalty(q);
				pen = pen + penalty(q+r);
			}
			else if ((cur_line + 2) == best_line)
			{
				if (d)
					pen = pen + displayWidowPenalty();
				else
					pen = pen + widowPenalty();
			}
			
			if (disc_break)
			{
				if (passive_pen_broken(cur_p) != 0)
					pen = pen + passive_pen_broken(cur_p);
				else
					pen = pen + brokenPenalty();
			}
			
			if (pen != 0)
			{
				r = newPenalty(pen);
    			link(tail) = r; 
    			tail = r;
			}
		}
		
		cur_line++;
		cur_p = next_break(cur_p);
		if (cur_p != TEX_NULL)
		{
			if (!post_disc_break)
			{
				r = temp_head;
				while (true)
				{
					q = link(r);
					if (q == cur_break(cur_p))
						goto done1;
						
					if (isCharNode(q))
						goto done1;
						
					if (((type(q) == (quint16)TEX_WHATSIT_NODE) && (subtype(q) == (quint16)TEX_LOCAL_PAR_NODE)))
					{
						;
					}
					else
					{
						if (nonDiscardable(q))
							goto done1;
							
						if (type(q) == (quint16)TEX_KERN_NODE)
						{
							if (subtype(q) != (quint16)TEX_EXPLICIT)
								goto done1;
						}
					}
					
					r = q;
				}
done1:
				if (r != temp_head)
				{
					link(r) = TEX_NULL; 
					flushNodeList(link(temp_head));
  					link(temp_head) = q;
				}
			}
		}
	} while (cur_p != TEX_NULL);
	
	if ((cur_line != best_line) || (link(temp_head) != TEX_NULL))
		confusion("line breaking");
		
	prev_graf = best_line-1; 
	dir_save  = dir_ptr;
}

void  XWTeX::tryBreak(qint32 pi, char break_type)
{
	qint32 r, prev_r, old_l, prev_prev_r;
	bool no_break_yet, node_r_stays_active, artificial_demerits;
	qint32 s, q, v, t, f, l, b, d, g;
	qint32 line_width, save_link, shortfall;
	char fit_class;
	
	if (qAbs(pi) >= TEX_INF_PENALTY)
	{
		if (pi > 0)
			return ;
		else
			pi = TEX_EJECT_PENALTY;
	}
	
	no_break_yet = true; 
	prev_r = active; 
	old_l = 0;
	cur_active_width[1] = active_width[1];
	cur_active_width[2] = active_width[2];
	cur_active_width[3] = active_width[3];
	cur_active_width[4] = active_width[4];
	cur_active_width[5] = active_width[5];
	cur_active_width[6] = active_width[6];
	cur_active_width[7] = active_width[7];
	
	while (true)
	{
		
tcontinue: 
		r = link(prev_r);
		if (type(r) == (quint16)TEX_DELTA_NODE)
		{
			cur_active_width[1] = cur_active_width[1] + mem[r + 1].ii.CINT0;
			cur_active_width[2] = cur_active_width[2] + mem[r + 2].ii.CINT0;
			cur_active_width[3] = cur_active_width[3] + mem[r + 3].ii.CINT0;
			cur_active_width[4] = cur_active_width[4] + mem[r + 4].ii.CINT0;
			cur_active_width[5] = cur_active_width[5] + mem[r + 5].ii.CINT0;
			cur_active_width[6] = cur_active_width[6] + mem[r + 6].ii.CINT0;
			cur_active_width[7] = cur_active_width[7] + mem[r + 7].ii.CINT0;
			prev_prev_r = prev_r; 
			prev_r = r; 
			goto tcontinue;
		}
		
		l = line_number(r);
		if (l > old_l)
		{
			if ((minimum_demerits < TEX_AWFUL_BAD) && 
				((old_l != easy_line) || (r == last_active)))
			{
				if (no_break_yet)
				{
					no_break_yet = false;
					break_width[1] = background[1];
					break_width[2] = background[2];
					break_width[3] = background[3];
					break_width[4] = background[4];
					break_width[5] = background[5];
					break_width[6] = background[6];
					break_width[7] = background[7];
					s = cur_p;
					if (break_type > TEX_UNHYPHENATED)
					{
						if (cur_p != TEX_NULL)
						{
							t = replace_count(cur_p); 
							v = cur_p; 
							s = post_break(cur_p);
							while (t > 0)
							{
								t--;
								v = link(v);
								if (isCharNode(v))
								{
									f = font(v);
									if (is_rotated(line_break_dir))
									{
										break_c_htdp = height_depth(char_info(f, character(v)));
    									break_width[1] = break_width[1] - char_height(f, break_c_htdp) - char_depth(f, break_c_htdp);
									}
									else
										break_width[1] = break_width[1] - char_width(f, char_info(f, character(v)));
								}
								else
								{
									switch (type(v))
									{
										case TEX_LIGATURE_NODE:
											f = font(lig_char(v));
											if (is_rotated(line_break_dir))
											{
												break_c_htdp = height_depth(char_info(f, character(lig_char(v))));
     											break_width[1] = break_width[1] - char_height(f, break_c_htdp) - char_depth(f, break_c_htdp);
											}
											else
												break_width[1] = break_width[1] - char_width(f, char_info(f, character(lig_char(v))));
											break;
											
										case TEX_HLIST_NODE:
										case TEX_VLIST_NODE:
											if (!(dir_orthogonal(dir_primary[box_dir(v)], dir_primary[line_break_dir])))
												break_width[1] = break_width[1] - width(v);
											else
												break_width[1] = break_width[1] - (depth(v) + height(v));
											break;
											
										case TEX_RULE_NODE:
										case TEX_KERN_NODE:
											break_width[1] = break_width[1] - width(v);
											break;
											
										default:
											confusion("disc1");
											break;
									}
								}
							}
							
							while (s != TEX_NULL)
							{
								if (isCharNode(s))
								{
									f = font(s);
									if (is_rotated(line_break_dir))
									{
										break_c_htdp = height_depth(char_info(f, character(s)));
    									break_width[1] = break_width[1] + char_height(f, break_c_htdp) + char_depth(f, break_c_htdp);
									}
									else
										break_width[1] = break_width[1] + char_width(f, char_info(f, character(s)));
								}
								else
								{
									switch (type(s))
									{
										case TEX_LIGATURE_NODE:
											f = font(lig_char(s));
											if (is_rotated(line_break_dir))
											{
												break_c_htdp = height_depth(char_info(f, character(lig_char(s))));
     											break_width[1] = break_width[1] + char_height(f, break_c_htdp) + char_depth(f, break_c_htdp);
											}
											else
												break_width[1] = break_width[1] + char_width(f, char_info(f, character(lig_char(s))));
											break;
											
										case TEX_HLIST_NODE:
										case TEX_VLIST_NODE:
											if (!(dir_orthogonal(dir_primary[box_dir(s)], dir_primary[line_break_dir])))
												break_width[1] = break_width[1] + width(s);
											else
												break_width[1] = break_width[1] + (depth(s) + height(s));
											break;
											
										case TEX_RULE_NODE:
										case TEX_KERN_NODE:
											break_width[1] = break_width[1] + width(s);
											break;
											
										default:
											confusion("disc2");
											break;
									}
								}
								
								s = link(s);
							}
							
							break_width[1] = break_width[1] + disc_width;
							if (post_break(cur_p) == TEX_NULL)
								s = link(v);
						}
					}
					
					while (s != TEX_NULL)
					{
						if (isCharNode(s))
							goto done;
							
						switch (type(s))
						{
							case TEX_GLUE_NODE:
								v = glue_ptr(s); 
								break_width[1] = break_width[1] - width(v);
								break_width[2 + stretch_order(v)] = break_width[2 + stretch_order(v)] - stretch(v);
								break_width[7] = break_width[7] - shrink(v);
								break;
								
							case TEX_PENALTY_NODE:
								break;
								
							case TEX_MATH_NODE:
								break_width[1] = break_width[1] - width(s);
								break;
								
							case TEX_KERN_NODE:
								if (subtype(s) != (quint16)TEX_EXPLICIT)
									goto done;
								else
									break_width[1] = break_width[1] - width(s);
								break;
								
							default:
								goto done;
								break;
						}
						
						s = link(s);
					}
done:
					;
				}
				
				if (type(prev_r) == (quint16)TEX_DELTA_NODE)
				{
					mem[prev_r + 1].ii.CINT0 = mem[prev_r + 1].ii.CINT0 - cur_active_width[1] + break_width[1];
					mem[prev_r + 2].ii.CINT0 = mem[prev_r + 2].ii.CINT0 - cur_active_width[2] + break_width[2];
					mem[prev_r + 3].ii.CINT0 = mem[prev_r + 3].ii.CINT0 - cur_active_width[3] + break_width[3];
					mem[prev_r + 4].ii.CINT0 = mem[prev_r + 4].ii.CINT0 - cur_active_width[4] + break_width[4];
					mem[prev_r + 5].ii.CINT0 = mem[prev_r + 5].ii.CINT0 - cur_active_width[5] + break_width[5];
					mem[prev_r + 6].ii.CINT0 = mem[prev_r + 6].ii.CINT0 - cur_active_width[6] + break_width[6];
					mem[prev_r + 7].ii.CINT0 = mem[prev_r + 7].ii.CINT0 - cur_active_width[7] + break_width[7];
				}
				else if (prev_r == active)
				{
					active_width[1] = break_width[1];
					active_width[2] = break_width[2];
					active_width[3] = break_width[3];
					active_width[4] = break_width[4];
					active_width[5] = break_width[5];
					active_width[6] = break_width[6];
					active_width[7] = break_width[7];
				}
				else
				{
					q = getNode(TEX_DELTA_NODE_SIZE); 
					link(q) = r; 
					type(q) = (quint16)TEX_DELTA_NODE;
					subtype(q) = (quint16)0;
					mem[q + 1].ii.CINT0 = break_width[1] - cur_active_width[1];
					mem[q + 2].ii.CINT0 = break_width[2] - cur_active_width[2];
					mem[q + 3].ii.CINT0 = break_width[3] - cur_active_width[3];
					mem[q + 4].ii.CINT0 = break_width[4] - cur_active_width[4];
					mem[q + 5].ii.CINT0 = break_width[5] - cur_active_width[5];
					mem[q + 6].ii.CINT0 = break_width[6] - cur_active_width[6];
					mem[q + 7].ii.CINT0 = break_width[7] - cur_active_width[7];
					link(prev_r) = q; 
					prev_prev_r = prev_r; 
					prev_r = q;
				}
				
				if (qAbs(adjDemerits()) >= TEX_AWFUL_BAD - minimum_demerits)
					minimum_demerits = TEX_AWFUL_BAD - 1;
				else
					minimum_demerits = minimum_demerits + qAbs(adjDemerits());
					
				for (fit_class = TEX_VERY_LOOSE_FIT; fit_class <= TEX_TIGHT_FIT; fit_class++)
				{
					if (minimal_demerits[fit_class] <= minimum_demerits)
					{
						q = getNode(TEX_PASSIVE_NODE_SIZE);
						link(q) = passive; 
						passive = q; 
						cur_break(q) = cur_p;
#ifdef XW_TEX_STAT

						pass_number++; 
						serial(q) = pass_number;
#endif //XW_TEX_STAT
						
						prev_break(q) = best_place[fit_class];
						passive_pen_inter(q)  = internal_pen_inter;
						passive_pen_broken(q) = internal_pen_broken;
						passive_last_left_box(q) = internal_left_box;
						passive_last_left_box_width(q) = internal_left_box_width;
						if (prev_break(q) != TEX_NULL)
						{
							passive_left_box(q) = passive_last_left_box(prev_break(q));
  							passive_left_box_width(q) = passive_last_left_box_width(prev_break(q));
						}
						else
						{
							passive_left_box(q) = init_internal_left_box;
  							passive_left_box_width(q) = init_internal_left_box_width;
						}
						
						passive_right_box(q) = internal_right_box;
						passive_right_box_width(q) = internal_right_box_width;
						q = getNode(active_node_size); 
						break_node(q) = passive;
						line_number(q) = best_pl_line[fit_class] + 1;
						fitness(q) = (quint16)fit_class; 
						type(q) = (quint16)break_type;
						total_demerits(q) = minimal_demerits[fit_class];
						if (do_last_line_fit)
						{
							active_short(q) = best_pl_short[fit_class];
							active_glue(q) = best_pl_glue[fit_class];
						}
						
						link(q) = r; 
						link(prev_r) = q; 
						prev_r = q;
						
#ifdef XW_TEX_STAT
						if (tracingParagraphs() > 0)
						{
							printnl("@@@@"); 
							printInt(serial(passive));
							print(tr(": line ")); 
							printInt(line_number(q) - 1);
							printChar('.'); 
							printInt(fit_class);
							if (break_type == TEX_HYPHENATED)
								printChar('-');
								
							print(" t="); 
							printInt(total_demerits(q));
							if (do_last_line_fit)
							{
								print(" s="); 
								printScaled(active_short(q));
								if (cur_p == TEX_NULL)
									print(" a=");
								else
									print(" g=");
									
								printScaled(active_glue(q));
							}
							
							print(" -> @@@@");
							if (prev_break(passive) == TEX_NULL)
								printChar('0');
							else
								printInt(serial(prev_break(passive)));
						}
#endif //XW_TEX_STAT
					}
					
					minimal_demerits[fit_class] = TEX_AWFUL_BAD;
				}
				
				minimum_demerits = TEX_AWFUL_BAD;
				if (r != last_active)
				{
					 q = getNode(TEX_DELTA_NODE_SIZE); 
					 link(q) = r; 
					 type(q) = (quint16)TEX_DELTA_NODE;
					 subtype(q) = (quint16)0;
					 mem[q + 1].ii.CINT0 = cur_active_width[1] - break_width[1];
					 mem[q + 2].ii.CINT0 = cur_active_width[2] - break_width[2];
					 mem[q + 3].ii.CINT0 = cur_active_width[3] - break_width[3];
					 mem[q + 4].ii.CINT0 = cur_active_width[4] - break_width[4];
					 mem[q + 5].ii.CINT0 = cur_active_width[5] - break_width[5];
					 mem[q + 6].ii.CINT0 = cur_active_width[6] - break_width[6];
					 mem[q + 7].ii.CINT0 = cur_active_width[7] - break_width[7];
					 link(prev_r) = q; 
					 prev_prev_r = prev_r; 
					 prev_r = q;
				}
			}
			
			if (r == last_active)
				return ;
				
			if (l > easy_line)
			{
				line_width = second_width; 
				old_l = TEX_MAX_HALFWORD - 1;
			}
			else
			{
				old_l = l;
				if (l > last_special_line)
					line_width = second_width;
				else if (parShapePtr() == TEX_NULL)
					line_width = first_width;
				else
					line_width = mem[parShapePtr() + 2 * l].ii.CINT0;
			}
		}
		
		artificial_demerits = false;
		shortfall = line_width - cur_active_width[1];
		if (break_node(r) == TEX_NULL)
			shortfall = shortfall - init_internal_left_box_width;
		else
			shortfall = shortfall - passive_last_left_box_width(break_node(r));
			
		shortfall = shortfall - internal_right_box_width;
		if (shortfall > 0)
		{
			if ((cur_active_width[3] != 0) || 
				(cur_active_width[4] != 0) || 
				(cur_active_width[5] != 0) || 
				(cur_active_width[6] != 0))
			{
				if (do_last_line_fit)
				{
					if (cur_p == TEX_NULL)
					{
						if ((active_short(r) == 0) || (active_glue(r) <= 0))
							goto not_found;
							
						if ((cur_active_width[3] != fill_width[0]) || 
							(cur_active_width[4] != fill_width[1]) || 
							(cur_active_width[5] != fill_width[2]))
						{
							goto not_found;
						}
						
						if (active_short(r) > 0)
							g = cur_active_width[2];
						else
							g = cur_active_width[6];
							
						if (g <= 0)
							goto not_found;
							
						arith_error = false; 
						g = fract(g, active_short(r), active_glue(r), TEX_MAX_DIMEN);
						if (lastLineFit() < 1000)
							g = fract(g, lastLineFit(), 1000, TEX_MAX_DIMEN);
							
						if (arith_error)
						{
							if (active_short(r) > 0)
								g = TEX_MAX_DIMEN;
							else
								g = -TEX_MAX_DIMEN;
						}
						
						if (g > 0)
						{
							if (g > shortfall)
								g = shortfall;
								
							if (g > 7230584)
							{
								if (cur_active_width[2] < 1663497)
								{
									b = TEX_INF_BAD; 
									fit_class = TEX_VERY_LOOSE_FIT; 
									goto found;
								}
							}
							
							b = badness(g, cur_active_width[2]);
							if (b > 12)
							{
								if (b > 99)
									fit_class = TEX_VERY_LOOSE_FIT;
								else
									fit_class = TEX_LOOSE_FIT;
							}
							else
								fit_class = TEX_DECENT_FIT;
								
							goto found;
						}
						else if (g < 0)
						{
							if (-g > cur_active_width[6])
								g = -cur_active_width[6];
								
							b = badness(-g, cur_active_width[6]);
							if (b > 12)
								fit_class = TEX_TIGHT_FIT;
							else
								fit_class = TEX_DECENT_FIT;
								
							goto found;
						}
not_found:
						;
					}
					
					shortfall = 0;
				}
				
				b = 0; 
				fit_class = TEX_DECENT_FIT;
			}
			else
			{
				if (shortfall > 7230584)
				{
					if (cur_active_width[2] < 1663497)
					{
						b = TEX_INF_BAD; 
						fit_class = TEX_VERY_LOOSE_FIT; 
						goto done1;
					}
				}
				
				b = badness(shortfall, cur_active_width[2]);
				if (b > 12)
				{
					if (b > 99)
						fit_class = TEX_VERY_LOOSE_FIT;
					else
						fit_class = TEX_LOOSE_FIT;
				}
				else
					fit_class = TEX_DECENT_FIT;
					
done1:
				;
			}
		}
		else
		{
			if (-shortfall > cur_active_width[7])
				b = TEX_INF_BAD + 1;
			else
				b = badness(-shortfall, cur_active_width[7]);
				
			if (b > 12)
				fit_class = TEX_TIGHT_FIT;
			else
				fit_class = TEX_DECENT_FIT;
		}
		
		if (do_last_line_fit)
		{
			if (cur_p == TEX_NULL)
				shortfall = 0;
				
			if (shortfall > 0)
				g = cur_active_width[2];
			else if (shortfall < 0)
				g = cur_active_width[6];
			else
				g = 0;
		}
		
found:
		if ((b > TEX_INF_BAD) || (pi == TEX_EJECT_PENALTY))
		{
			if (final_pass && 
				(minimum_demerits == TEX_AWFUL_BAD) && 
				(link(r) == last_active) && 
				(prev_r == active))
			{
				artificial_demerits = true;
			}
			else if (b > threshold)
				goto deactivate;
				
			node_r_stays_active = false;
		}
		else
		{
			prev_r = r;
			if (b > threshold)
				goto tcontinue;
				
			node_r_stays_active = true;
		}
		
		if (artificial_demerits)
			d = 0;
		else
		{
			d = linePenalty() + b;
			if (qAbs(d) >= 10000)
				d = 100000000;
			else
				d = d * d;
				
			if (pi != 0)
			{
				if (pi > 0)
					d = d + pi * pi;
				else if (pi > TEX_EJECT_PENALTY)
					d = d -pi * pi;
			}
			
			if ((break_type == TEX_HYPHENATED) && 
				(type(r) == (quint16)TEX_HYPHENATED))
			{
				if (cur_p != TEX_NULL)
					d = d + doubleHyphenDemerits();
				else
					d = d + finalHyphenDemerits();
			}
			
			if (qAbs((qint32)fit_class-(qint32)fitness(r)) > (qint32)1)
				d = d + adjDemerits();
		}
		
#ifdef XW_TEX_STAT
		
		if (tracingParagraphs() > 0)
		{
			if (printed_node != cur_p)
			{
				printnl("");
				if (cur_p == TEX_NULL)
					shortDisplay(link(printed_node));
				else
				{
					save_link = link(cur_p);
  					link(cur_p) = TEX_NULL; 
  					printnl(""); 
  					shortDisplay(link(printed_node));
  					link(cur_p) = save_link;
				}
				
				printed_node = cur_p;
			}
			
			printnl("@@");
			if (cur_p == TEX_NULL)
				printEsc(TeXPar);
			else if (type(cur_p) != (quint16)TEX_GLUE_NODE)
			{
				if (type(cur_p) == (quint16)TEX_PENALTY_NODE)
					printEsc(TeXPenalty);
				else if (type(cur_p) == (quint16)TEX_DISC_NODE)
					printEsc(TeXDiscretionary);
				else if (type(cur_p) == (quint16)TEX_KERN_NODE)
					printEsc(TeXKern);
				else
					printEsc(TeXMath);
			}
			
			print(" via @@@@");
			if (break_node(r) == TEX_NULL)
				printChar('0');
			else
				printInt(serial(break_node(r)));
				
			print(" b=");			
			if (b > TEX_INF_BAD)
				printChar('*');
			else
				printInt(b);
				
			print(" p="); 
			printInt(pi); 
			print(" d=");
			
			if (artificial_demerits)
				printChar('*');
			else
				printInt(d);
		}

#endif //XW_TEX_STAT

		d = d + total_demerits(r);
		if (d <= minimal_demerits[fit_class])
		{
			minimal_demerits[fit_class] = d;
  			best_place[fit_class] = break_node(r); 
  			best_pl_line[fit_class] = l;
  			if (do_last_line_fit)
  			{
  				best_pl_short[fit_class] = shortfall; 
  				best_pl_glue[fit_class] = g;
  			}
  			
  			if (d < minimum_demerits)
  				minimum_demerits = d;
		}
		
		if (node_r_stays_active)
			goto tcontinue;
			
deactivate:
		link(prev_r) = link(r); 
		freeNode(r, active_node_size);
		if (prev_r == active)
		{
			r = link(active);
			if (type(r) == (quint16)TEX_DELTA_NODE)
			{
				active_width[1] = active_width[1] + mem[r + 1].ii.CINT0;
				active_width[2] = active_width[2] + mem[r + 2].ii.CINT0;
				active_width[3] = active_width[3] + mem[r + 3].ii.CINT0;
				active_width[4] = active_width[4] + mem[r + 4].ii.CINT0;
				active_width[5] = active_width[5] + mem[r + 5].ii.CINT0;
				active_width[6] = active_width[6] + mem[r + 6].ii.CINT0;
				active_width[7] = active_width[7] + mem[r + 7].ii.CINT0;
				
				cur_active_width[1] = active_width[1];
				cur_active_width[2] = active_width[2];
				cur_active_width[3] = active_width[3];
				cur_active_width[4] = active_width[4];
				cur_active_width[5] = active_width[5];
				cur_active_width[6] = active_width[6];
				cur_active_width[7] = active_width[7];
				
				link(active) = link(r); 
				freeNode(r, TEX_DELTA_NODE_SIZE);
			}
		}
		else if (type(prev_r) == (quint16)TEX_DELTA_NODE)
		{
			r = link(prev_r);
			if (r == last_active)
			{
				cur_active_width[1] = cur_active_width[1] - mem[prev_r + 1].ii.CINT0;
				cur_active_width[2] = cur_active_width[2] - mem[prev_r + 2].ii.CINT0;
				cur_active_width[3] = cur_active_width[3] - mem[prev_r + 3].ii.CINT0;
				cur_active_width[4] = cur_active_width[4] - mem[prev_r + 4].ii.CINT0;
				cur_active_width[5] = cur_active_width[5] - mem[prev_r + 5].ii.CINT0;
				cur_active_width[6] = cur_active_width[6] - mem[prev_r + 6].ii.CINT0;
				cur_active_width[7] = cur_active_width[7] - mem[prev_r + 7].ii.CINT0;
				
				link(prev_prev_r) = last_active;
    			freeNode(prev_r, TEX_DELTA_NODE_SIZE); 
    			prev_r = prev_prev_r;
			}
			else if (type(r) == (quint16)TEX_DELTA_NODE)
			{
				cur_active_width[1] = cur_active_width[1] + mem[r + 1].ii.CINT0;
				cur_active_width[2] = cur_active_width[2] + mem[r + 2].ii.CINT0;
				cur_active_width[3] = cur_active_width[3] + mem[r + 3].ii.CINT0;
				cur_active_width[4] = cur_active_width[4] + mem[r + 4].ii.CINT0;
				cur_active_width[5] = cur_active_width[5] + mem[r + 5].ii.CINT0;
				cur_active_width[6] = cur_active_width[6] + mem[r + 6].ii.CINT0;
				cur_active_width[7] = cur_active_width[7] + mem[r + 7].ii.CINT0;
				
				mem[prev_r + 1].ii.CINT0 = mem[prev_r + 1].ii.CINT0 + mem[r + 1].ii.CINT0;
				mem[prev_r + 2].ii.CINT0 = mem[prev_r + 2].ii.CINT0 + mem[r + 2].ii.CINT0;
				mem[prev_r + 3].ii.CINT0 = mem[prev_r + 3].ii.CINT0 + mem[r + 3].ii.CINT0;
				mem[prev_r + 4].ii.CINT0 = mem[prev_r + 4].ii.CINT0 + mem[r + 4].ii.CINT0;
				mem[prev_r + 5].ii.CINT0 = mem[prev_r + 5].ii.CINT0 + mem[r + 5].ii.CINT0;
				mem[prev_r + 6].ii.CINT0 = mem[prev_r + 6].ii.CINT0 + mem[r + 6].ii.CINT0;
				mem[prev_r + 7].ii.CINT0 = mem[prev_r + 7].ii.CINT0 + mem[r + 7].ii.CINT0;
				
				link(prev_r) = link(r); 
				freeNode(r, TEX_DELTA_NODE_SIZE);
			}
		}
	}
	
#ifdef XW_TEX_STAT

	if (cur_p == printed_node)
	{
		if (cur_p != TEX_NULL)
		{
			if (type(cur_p) == (quint16)TEX_DISC_NODE)
			{
				t = replace_count(cur_p);
				while (t > 0)
				{
					t--;
					printed_node = link(printed_node);
				}
			}
		}
	}
	
#endif //XW_TEX_STAT
}

