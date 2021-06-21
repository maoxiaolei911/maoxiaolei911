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

void XWTeX::afterMath()
{
	bool   l, danger;
	qint32 m, p, q, b, w, z, e, d, s, g1, g2, r, t, a;
	
	danger = false;
	if ((font_params(famFnt(2 + TEX_TEXT_SIZE)) < TEX_TOTAL_MATHSY_PARAMS) || 
   		(font_params(famFnt(2 + TEX_SCRIPT_SIZE)) < TEX_TOTAL_MATHSY_PARAMS) || 
   		(font_params(famFnt(2 + TEX_SCRIPT_SCRIPT_SIZE)) < TEX_TOTAL_MATHSY_PARAMS))
   	{
   		printErr(tr("Math formula deleted: Insufficient symbol fonts"));
   		help3(tr("Sorry, but I can't typeset math unless \\textfont 2"));
   		help_line[1] = tr("and \\scriptfont 2 and \\scriptscriptfont 2 have all");
   		help_line[0] = tr("the \\fontdimen values needed in math symbol fonts.");
   		error(); 
   		flushMath(); 
   		danger = true;
   	}
   	else if ((font_params(famFnt(3 + TEX_TEXT_SIZE)) < TEX_TOTAL_MATHEX_PARAMS) || 
   			(font_params(famFnt(3 + TEX_SCRIPT_SIZE)) < TEX_TOTAL_MATHEX_PARAMS) || 
   			(font_params(famFnt(3 + TEX_SCRIPT_SCRIPT_SIZE)) < TEX_TOTAL_MATHEX_PARAMS))
	{
		printErr("Math formula deleted: Insufficient extension fonts");
		help3(tr("Sorry, but I can't typeset math unless \\textfont 3"));
		help_line[1] = tr("and \\scriptfont 3 and \\scriptscriptfont 3 have all");
		help_line[0] = tr("the \\fontdimen values needed in math extension fonts.");
		error(); 
		flushMath(); 
		danger = true;
	}
	
	m = mode; 
	l = false; 
	p = finMList(TEX_NULL);
	if (mode == -m)
	{
		getXToken();
		if (cur_cmd != TEX_MATH_SHIFT)
		{
			printErr(tr("Display math should end with $$"));
			help2(tr("The `$' that I just saw supposedly matches a previous `$$'."));
			help_line[0] = tr("So I shall assume that you typed `$$' both times.");
			backError();
		}
		
		cur_mlist = p; 
		cur_style = TEX_TEXT_STYLE; 
		mlist_penalties = false;
  		mlistToHList(); 
  		a = hpack(link(temp_head), 0, TEX_ADDITIONAL);
  		unsave();
		save_ptr--;
		flushNodeList(text_dir_ptr);
		text_dir_ptr = saved(0);
		save_ptr--;
		if (saved(0) == 1)
			l = true;
  		danger = false;
  		
		if ((font_params(famFnt(2 + TEX_TEXT_SIZE)) < TEX_TOTAL_MATHSY_PARAMS) || 
   			(font_params(famFnt(2 + TEX_SCRIPT_SIZE)) < TEX_TOTAL_MATHSY_PARAMS) || 
   			(font_params(famFnt(2 + TEX_SCRIPT_SCRIPT_SIZE)) < TEX_TOTAL_MATHSY_PARAMS))
   		{
   			printErr(tr("Math formula deleted: Insufficient symbol fonts"));
   			help3(tr("Sorry, but I can't typeset math unless \\textfont 2"));
   			help_line[1] = tr("and \\scriptfont 2 and \\scriptscriptfont 2 have all");
   			help_line[0] = tr("the \\fontdimen values needed in math symbol fonts.");
   			error(); 
   			flushMath(); 
   			danger = true;
   		}
   		else if ((font_params(famFnt(3 + TEX_TEXT_SIZE)) < TEX_TOTAL_MATHEX_PARAMS) || 
   				(font_params(famFnt(3 + TEX_SCRIPT_SIZE)) < TEX_TOTAL_MATHEX_PARAMS) || 
   				(font_params(famFnt(3 + TEX_SCRIPT_SCRIPT_SIZE)) < TEX_TOTAL_MATHEX_PARAMS))
		{
			printErr("Math formula deleted: Insufficient extension fonts");
			help3(tr("Sorry, but I can't typeset math unless \\textfont 3"));
			help_line[1] = tr("and \\scriptfont 3 and \\scriptscriptfont 3 have all");
			help_line[0] = tr("the \\fontdimen values needed in math extension fonts.");
			error(); 
			flushMath(); 
			danger = true;
		}
		
		m = mode; 
		p = finMList(TEX_NULL);
	}
	else
		a = TEX_NULL;
		
	if (m < 0)
	{
		tailAppend(newMath(mathSurround(), TEX_BEFORE));
		if (dir_math_save)
			tailAppend(newDir(mathDirection()));
			
		cur_mlist = p; 
		cur_style = TEX_TEXT_STYLE; 
		mlist_penalties = (mode > 0); 
		mlistToHList();
		link(tail) = link(temp_head);
		while (link(tail) != TEX_NULL)
			tail = link(tail);
			
		if (dir_math_save)
			tailAppend(newDir(mathDirection() - 64));
			
		dir_math_save = false;
		tailAppend(newMath(mathSurround(), TEX_AFTER));
		space_factor = 1000;
		unsave();
		save_ptr--;
		flushNodeList(text_dir_ptr);
		text_dir_ptr = saved(0);
	}
	else
	{
		if (a == TEX_NULL)
		{
			getXToken();
			if (cur_cmd != TEX_MATH_SHIFT)
			{
				printErr(tr("Display math should end with $$"));
				help2(tr("The `$' that I just saw supposedly matches a previous `$$'."));
				help_line[0] = tr("So I shall assume that you typed `$$' both times.");
				backError();
			}
		}
		
		cur_mlist = p; 
		cur_style = TEX_DISPLAY_STYLE; 
		mlist_penalties = false;
		mlistToHList(); 
		p = link(temp_head);
		adjust_tail = adjust_head; 
		b = hpack(p, 0, TEX_ADDITIONAL); 
		p = list_ptr(b);
		t = adjust_tail; 
		adjust_tail = TEX_NULL;
		w = width(b); 
		z = displayWidth(); 
		s = displayIndent();
		if ((a == TEX_NULL) || danger)
		{
			e = 0; 
			q = 0;
		}
		else
		{
			e = width(a); 
			q = e + math_quad(TEX_TEXT_SIZE);
		}
		
		if ((w +q) > z)
		{
			if ((e != 0) && (((w - total_shrink[TEX_NORMAL] + q) <= z) || 
   				(total_shrink[TEX_SFI] != 0) || (total_shrink[TEX_FIL] != 0) || 
   				(total_shrink[TEX_FILL] != 0) || (total_shrink[TEX_FILLL] != 0)))
   			{
   				freeNode(b, TEX_BOX_NODE_SIZE);
  				b = hpack(p, z - q, TEX_EXACTLY);
   			}
   			else
   			{
   				e = 0;
   				if (w > z)
   				{
   					freeNode(b, TEX_BOX_NODE_SIZE);
    				b = hpack(p, z, TEX_EXACTLY);
   				}
   			}
   			
   			w = width(b);
		}
		
		d = half(z - w);
		if ((e > 0) && (d < (2 * e)))
		{
			d =half(z - w - e);
			if (p != TEX_NULL)
			{
				if (!isCharNode(p))
				{
					if (type(p) == (quint16)TEX_GLUE_NODE)
						d = 0;
				}
			}
		}
		
		tailAppend(newPenalty(predisplayPenalty()));
		if (((d + s) <= predisplaySize()) || l)
		{
			g1 = TEX_ABOVE_DISPLAY_SKIP_CODE; 
			g2 = TEX_BELOW_DISPLAY_SKIP_CODE;
		}
		else
		{
			g1 = TEX_ABOVE_DISPLAY_SHORT_SKIP_CODE;
  			g2 = TEX_BELOW_DISPLAY_SHORT_SKIP_CODE;
		}
		
		if (l && (e == 0))
		{
			shift_amount(a) = s; 
			appendToVList(a);
  			tailAppend(newPenalty(TEX_INF_PENALTY));
		}
		else
			tailAppend(newParamGlue(g1));
			
		if (e != 0)
		{
			r = newKern(z - w - e - d);
			if (l)
			{
				link(a) = r; 
				link(r) = b; 
				b = a; 
				d = 0;
			}
			else
			{
				link(b) = r; 
				link(r) = a;
			}
			
			b = hpack(b, 0, TEX_ADDITIONAL);
		}
		
		shift_amount(b) = s + d; 
		appendToVList(b);
		
		if ((a != TEX_NULL) && (e == 0) && !l)
		{
			tailAppend(newPenalty(TEX_INF_PENALTY));
  			shift_amount(a) = s + z - width(a);
  			appendToVList(a);
  			g2 = 0;
		}
		
		if (t != adjust_head)
		{
			link(tail) = link(adjust_head); 
			tail = t;
		}
		
		tailAppend(newPenalty(postdisplayPenalty()));
		if (g2 > 0)
			tailAppend(newParamGlue(g2));
			
		resumeAfterDisplay();
	}
}

void XWTeX::appendChoices()
{
	tailAppend(newChoice()); 
	save_ptr++; 
	saved(-1) = 0;
	pushMath(TEX_MATH_CHOICE_GROUP); 
	scanLeftBrace();
}

void  XWTeX::appendDiscretionary()
{
	qint32 c;
	
	tailAppend(newDisc());
	if (cur_chr == 1)
	{
		c = hyphen_char(curFont());
		if (c >= 0)
		{
			if (c <= TEX_BIGGEST_CHAR)
				pre_break(tail) = newCharacter(curFont(), c);
		}
	}
	else
	{
		save_ptr++; 
		saved(-1) = 0; 
		newSaveLevel(TEX_DISC_GROUP);
  		scanLeftBrace(); 
  		pushNest(); 
  		mode = -TEX_HMODE; 
  		space_factor = 1000;
	}
}

void XWTeX::appendGlue()
{
	qint32 s = cur_chr;
	
	switch (s)
	{
		case TEX_FIL_CODE: 
			cur_val = fil_glue;
			break;
			
		case TEX_FILL_CODE: 
			cur_val = fill_glue;
			break;
			
		case TEX_SS_CODE: 
			cur_val = ss_glue;
			break;
			
		case TEX_FIL_NEG_CODE: 
			cur_val = fil_neg_glue;
			break;
			
		case TEX_SKIP_CODE: 
			scanGlue(TEX_GLUE_VAL);
			break;
			
		case TEX_MSKIP_CODE: 
			scanGlue(TEX_MU_VAL);
			break;
			
		default:
			break;
	}
	
	tailAppend(newGlue(cur_val));
	if (s >= TEX_SKIP_CODE)
	{
		(glue_ref_count(cur_val))--;
		if (s > TEX_SKIP_CODE)
			subtype(tail) = (quint16)TEX_MU_GLUE;
	}
}

void XWTeX::appendItalicCorrection()
{
	qint32 p, f;
	
	if (tail != head)
	{
		if (isCharNode(tail))
			p = tail;
		else if (type(tail) == (quint16)TEX_LIGATURE_NODE)
			p = lig_char(tail);
		else
			return;
			
		f = font(p);
  		tailAppend(newKern(char_italic(f, char_info(f, character(p)))));
  		subtype(tail) = (quint16)TEX_EXPLICIT;
	}
}

void XWTeX::appendKern()
{
	qint32 s = cur_chr; 
	scanDimen(s == TEX_MU_GLUE, false, false);
	tailAppend(newKern(cur_val)); 
	subtype(tail) = (quint16)s;
}

void XWTeX::appendPenalty()
{
	scanInt(); 
	tailAppend(newPenalty(cur_val));
	if (mode == TEX_VMODE)
		buildPage();
}

void XWTeX::appendToVList(qint32 b)
{
	qint32 d, p;
	if (prev_depth > TEX_IGNORE_DEPTH)
	{
		if ((type(b) == (quint16)TEX_HLIST_NODE) && (is_mirrored(box_dir(b))))
			d = width(baseLineSkip()) - prev_depth - depth(b);
		else
			d = width(baseLineSkip()) - prev_depth - height(b);
			
		if (d < lineSkipLimit())
			p = newParamGlue(TEX_LINE_SKIP_CODE);
		else
		{
			p = newSkipParam(TEX_BASELINE_SKIP_CODE);
    		width(temp_ptr) = d;
		}
		
		link(tail) = p; 
		tail = p;
	}
	
	link(tail) = b; 
	tail = b;
	if ((type(b) == (quint16)TEX_HLIST_NODE) && (is_mirrored(box_dir(b))))
		prev_depth = height(b);
	else
		prev_depth = depth(b);
}

void XWTeX::appSpace()
{
	qint32 q;
	
	if ((space_factor >= 2000) && (xspaceSkip() != zero_glue))
		q = newParamGlue(TEX_XSPACE_SKIP_CODE);
	else
	{
		if (spaceSkip() != zero_glue)
			main_p = spaceSkip();
		else
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
		}
		
		main_p = newSpec(main_p);
		if (space_factor >= 2000)
			width(main_p) = width(main_p) + extra_space(curFont());
		stretch(main_p) = xnOverD(stretch(main_p), space_factor, 1000);
		shrink(main_p)  = xnOverD(shrink(main_p), 1000, space_factor);
		
		q = newGlue(main_p); 
		glue_ref_count(main_p) = TEX_NULL;
	}
	
	link(tail) = q; 
	tail = q;
}

void XWTeX::beginBox(qint32 box_context)
{
	qint32 p, q, m, k, n;
	switch (cur_chr)
	{
		case TEX_BOX_CODE:
			scanEightBitInt(); 
			cur_box = box(cur_val);
  		setEquiv(TEX_BOX_BASE + cur_val, TEX_NULL);
			break;
			
		case TEX_COPY_CODE: 
			scanEightBitInt(); 
			cur_box = copyNodeList(box(cur_val));
			break;
			
		case TEX_LAST_BOX_CODE:
			cur_box = TEX_NULL;
			if (qAbs(mode) == TEX_MMODE)
			{
				youCant(); 
				help1(tr("Sorry; this \\lastbox will be void.")); 
				error();
			}
			else if ((mode == TEX_VMODE) && (head == tail))
			{
				youCant();
  				help2(tr("Sorry...I usually can't take things from the current page."));
  				help_line[0] = tr("This \\lastbox will therefore be void.");
  				error();
			}
			else
			{
				if (!isCharNode(tail))
				{
					if ((type(tail) == (quint16)TEX_HLIST_NODE) || 
						(type(tail) == (quint16)TEX_VLIST_NODE))
					{
						q = head;
						do
						{
							p = q;
							if (!isCharNode(q))
							{
								if (type(q) == (quint16)TEX_DISC_NODE)
								{
									for (m = 1; m <= replace_count(q); m++)
										p = link(p);
										
									if (p == tail)
										goto done;
								}
							}
							
							q = link(p);
						} while (q != tail);
						
						cur_box = tail; 
						shift_amount(cur_box) = 0;
						tail = p; 
						link(p) = TEX_NULL;
						
done:
						;
					}
				}
			}
			break;
			
		case TEX_VSPLIT_CODE:
			scanEightBitInt(); 
			n = cur_val;
			if (!scanKeyWord(TeXTo))
			{
				printErr(tr("Missing `to' inserted"));
				help2(tr("I'm working on `\\vsplit<box number> to <dimen>';"));
				help_line[0] = tr("will look for the <dimen> next.");
				error();
			}
			scanNormalDimen();
			cur_box = vsplit(n, cur_val);
			break;
			
		default:
			k = cur_chr - TEX_VTOP_CODE; 
			saved(0) = box_context;
			{
				switch (qAbs(mode))
				{
					case TEX_VMODE: 
						spec_direction = bodyDirection();
						break;
						
					case TEX_HMODE: 
						spec_direction = textDirection();
						break;
						
					case TEX_MMODE: 
						spec_direction = mathDirection();
						break;
						
					default:
						break;
				}
			}
			
			if (k == TEX_HMODE)
			{
				if ((box_context < TEX_BOX_FLAG) && (qAbs(mode) == TEX_VMODE))
					scanSpec(TEX_ADJUSTED_HBOX_GROUP, true);
				else
					scanSpec(TEX_HBOX_GROUP, true);
			}
			else
			{
				if (k == TEX_VMODE)
					scanSpec(TEX_VBOX_GROUP, true);
				else
				{
					scanSpec(TEX_VTOP_GROUP, true); 
					k = TEX_VMODE;
				}
				
				normalParagraph();
			}
			pushNest(); 
			mode = -k;
			if (k == TEX_VMODE)
			{
				prev_depth = TEX_IGNORE_DEPTH;
				if (insert_src_special_every_vbox)
					insertSrcSpecial();
				if (everyVBox() != TEX_NULL)
					beginTokenList(everyVBox(), TEX_EVERY_VBOX_TEXT);
			}
			else
			{
				space_factor = 1000;
				if (everyHBox() != TEX_NULL)
					beginTokenList(everyHBox(), TEX_EVERY_HBOX_TEXT);
			}
			return ;
			break;
	}
	boxEnd(box_context);
}

void XWTeX::boxEnd(qint32 box_context)
{
	qint32 p;
	if (box_context < TEX_BOX_FLAG)
	{
		if (cur_box != TEX_NULL)
		{
			shift_amount(cur_box) = box_context;
			if (qAbs(mode) == TEX_VMODE)
			{
				appendToVList(cur_box);
				if (adjust_tail != TEX_NULL)
				{
					if (adjust_head != adjust_tail)
					{
						link(tail) = link(adjust_head); 
						tail = adjust_tail;
					}
					
					adjust_tail = TEX_NULL;
				}
				
				if (mode > 0)
					buildPage();
			}
			else
			{
				if (qAbs(mode) == TEX_HMODE)
					space_factor = 1000;
				else
				{
					p = newNoad();
      				math_type(nucleus(p)) = TEX_SUB_BOX;
      				info(nucleus(p)) = cur_box; 
      				cur_box = p;
				}
				
				link(tail) = cur_box; 
				tail = cur_box;
			}
		}
	}
	else if (box_context < TEX_SHIP_OUT_FLAG)
	{
		if (box_context < TEX_GLOBAL_BOX_FLAG)
			eqDefine(TEX_BOX_BASE + box_context - TEX_BOX_FLAG, (quint16)TEX_BOX_REF, cur_box);
		else
			geqDefine(TEX_BOX_BASE + box_context - TEX_GLOBAL_BOX_FLAG, (quint16)TEX_BOX_REF, cur_box);
	}
	else if (cur_box != TEX_NULL)
	{	
		if (box_context > TEX_SHIP_OUT_FLAG)
		{
			do
			{
				getXToken();
			} while ((cur_cmd == TEX_SPACER) || (cur_cmd == TEX_RELAX));
			
			if (((cur_cmd == TEX_HSKIP) && (qAbs(mode) != TEX_VMODE)) || 
				((cur_cmd == TEX_VSKIP) && (qAbs(mode) == TEX_VMODE)) || 
				((cur_cmd == TEX_MSKIP) && (qAbs(mode) == TEX_MMODE)))
			{
				appendGlue(); 
				subtype(tail) = (quint16)(box_context - (TEX_LEADER_FLAG - TEX_A_LEADERS));
  				leader_ptr(tail) = cur_box;
			}
			else
			{
				printErr(tr("Leaders not followed by proper glue"));
				help3(tr("You should say `\\leaders <box or rule><hskip or vskip>'."));
				help_line[1] = tr("I found the <box or rule>, but there's no suitable");
				help_line[0] = tr("<hskip or vskip>, so I'm ignoring these leaders.");
				backError();
  				flushNodeList(cur_box);
			}
		}
		else
			shipOut(cur_box);
	}
}

qint32  XWTeX::charBox(qint32 f, qint32 c)
{
	TeXFourQuarters q  = char_info(f, c); 
	qint32 hd = height_depth(q);
	qint32 b  = newNullBox(); 
	width(b)  = char_width(f, q) + char_italic(f, q);
	height(b) = char_height(f, hd); 
	depth(b)  = char_depth(f, hd);
	qint32 p  = getAvail(); 
	character(p) = (quint16)c; 
	font(p) = (quint16)f; 
	list_ptr(b) = p; 
	return b;
}

qint32 XWTeX::cleanBox(qint32 p, qint32 s)
{
	qint32 q, save_style, x, r;
	
	switch (math_type(p))
	{
		case TEX_MATH_CHAR:
			cur_mlist = newNoad(); 
			mem[nucleus(cur_mlist)] = mem[p];
			break;
			
		case TEX_SUB_BOX:
			q = info(p); 
			goto found;
			break;
			
		case TEX_SUB_MLIST:
			cur_mlist = info(p);
			break;
			
		default:
			q = newNullBox(); 
			goto found;
			break;
	}
	
	save_style = cur_style; 
	cur_style  = s; 
	mlist_penalties = false;
	mlistToHList(); 
	q = link(temp_head);
	cur_style = save_style;
	if (cur_style < TEX_SCRIPT_STYLE)
		cur_size = TEX_TEXT_SIZE;
	else
		cur_size = TEX_SCRIPT_SIZE * ((cur_style - TEX_TEXT_STYLE) / 2);
		
	cur_mu = xOverN(math_quad(cur_size), 18);
	
found: 
	if (isCharNode(q) || (q == TEX_NULL))
		x = hpack(q, 0, TEX_ADDITIONAL);
	else if ((link(q) == TEX_NULL) && 
		     (type(q) <= (quint16)TEX_VLIST_NODE) && 
		     (shift_amount(q) == 0))
	{
		x = q;
	}
	else
		x = hpack(q, 0, TEX_ADDITIONAL);
		
	q = list_ptr(x);
	if (isCharNode(q))
	{
		r = link(q);
		if (r != TEX_NULL)
		{
			if (link(r) == TEX_NULL)
			{
				if (!isCharNode(r))
				{
					if (type(r) == (quint16)TEX_KERN_NODE)
					{
						freeNode(r, TEX_SMALL_NODE_SIZE); 
						link(q) = TEX_NULL;
					}
				}
			}
		}
	}
	
	return x;
}

qint32 XWTeX::copyNodeList(qint32 p)
{
	qint32 h = getAvail(); 
	qint32 q = h;
	qint32 r = 0;
	while (p != TEX_NULL)
	{
		char words = 1;
		if (isCharNode(p))
			r = getAvail();
		else
		{
			switch (type(p))
			{
				case TEX_HLIST_NODE:
				case TEX_VLIST_NODE:
				case TEX_UNSET_NODE:
					r = getNode(TEX_BOX_NODE_SIZE);
  					mem[r + 7] = mem[p + 7]; 
  					mem[r + 6] = mem[p + 6]; 
  					mem[r + 5] = mem[p + 5];
  					list_ptr(r) = copyNodeList(list_ptr(p));
  					words = 5;
					break;
					
				case TEX_RULE_NODE:
					r = getNode(TEX_RULE_NODE_SIZE); 
					words = TEX_RULE_NODE_SIZE;
					break;
					
				case TEX_INS_NODE:
					r = getNode(TEX_INS_NODE_SIZE); 
					mem[r + 4] = mem[p + 4];
					add_glue_ref(split_top_ptr(p));
					ins_ptr(r) = copyNodeList(ins_ptr(p));
					words = TEX_INS_NODE_SIZE - 1;
					break;
					
				case TEX_WHATSIT_NODE:
					{
						switch (subtype(p))
						{
							case TEX_OPEN_NODE:
								r = getNode(TEX_OPEN_NODE_SIZE); 
								words = TEX_OPEN_NODE_SIZE;
								break;
								
							case TEX_WRITE_NODE:
							case TEX_SPECIAL_NODE:
								r = getNode(TEX_WRITE_NODE_SIZE);
								add_token_ref(write_tokens(p)); 
								words = TEX_WRITE_NODE_SIZE;
								break;
								
							case TEX_CLOSE_NODE:
							case TEX_LANGUAGE_NODE:
								r = getNode(TEX_SMALL_NODE_SIZE);
								words = TEX_SMALL_NODE_SIZE;
								break;
								
							case TEX_DIR_NODE:
								r = getNode(TEX_DIR_NODE_SIZE);
  								words = TEX_DIR_NODE_SIZE;
								break;
								
							case TEX_LOCAL_PAR_NODE:
								r = getNode(TEX_LOCAL_PAR_SIZE);
  								words = TEX_LOCAL_PAR_SIZE;
								break;
								
							default:
								confusion("ext2");
								break;
						}
					}
					break;
					
				case TEX_GLUE_NODE:
					r = getNode(TEX_SMALL_NODE_SIZE); 
					add_glue_ref(glue_ptr(p));
  					glue_ptr(r) = glue_ptr(p); 
  					leader_ptr(r) = copyNodeList(leader_ptr(p));
					break;
					
				case TEX_KERN_NODE:
				case TEX_MATH_NODE:
				case TEX_PENALTY_NODE:
					r = getNode(TEX_SMALL_NODE_SIZE);
  					words = TEX_SMALL_NODE_SIZE;
					break;
					
				case TEX_LIGATURE_NODE:
					r = getNode(TEX_SMALL_NODE_SIZE);
  					mem[lig_char(r)] = mem[lig_char(p)];
  					lig_ptr(r) = copyNodeList(lig_ptr(p));
					break;
					
				case TEX_DISC_NODE:
					r = getNode(TEX_SMALL_NODE_SIZE);
  					pre_break(r) = copyNodeList(pre_break(p));
  					post_break(r) = copyNodeList(post_break(p));
					break;
					
				case TEX_MARK_NODE:
					r = getNode(TEX_SMALL_NODE_SIZE); 
					add_token_ref(mark_ptr(p));
  					words = TEX_SMALL_NODE_SIZE;
					break;
					
				case TEX_ADJUST_NODE:
					r = getNode(TEX_SMALL_NODE_SIZE);
  					adjust_ptr(r) = copyNodeList(adjust_ptr(p));
					break;
					
				default:
					confusion("copying");
					break;
			}
		}
		
		while (words > 0)
		{
			words--;
			mem[r + words] = mem[p + words];
		}
		
		link(q) = r; 
		q = r; 
		p = link(p);
	}
	
	link(q) = TEX_NULL; 
	q = link(h); 
	freeAvail(h);
	return q;
}

void XWTeX::deleteLast()
{
	qint32 p, q, m;
	
	if ((mode == TEX_VMODE) && (tail == head))
	{
		if ((cur_chr != TEX_GLUE_NODE) || 
			(last_glue != TEX_MAX_HALFWORD))
		{
			youCant();
  			help2(tr("Sorry...I usually can't take things from the current page."));
  			help_line[0] = tr("Try `I\\vskip-\\lastskip' instead.");
  			if (cur_chr == TEX_KERN_NODE)
  				help_line[0] = tr("Try `I\\kern-\\lastkern' instead.");
  			else if (cur_chr != TEX_GLUE_NODE)
  				help_line[0] = tr("Perhaps you can make the output routine do it.");
  				
  			error();
		}
	}
	else
	{
		if (!isCharNode(tail))
		{
			if (type(tail) == (quint16)cur_chr)
			{
				q = head;
				do
				{
					p = q;
					if (!isCharNode(q))
					{
						if (type(q) == (quint16)TEX_DISC_NODE)
						{
							for (m = 1; m <= replace_count(q); m++)
								p = link(p);
								
							if (p == tail)
								return ;
						}
					}
					
					q = link(p);
				} while (q != tail);
				
				link(p) = TEX_NULL; 
				flushNodeList(tail); 
				tail = p;
			}
		}
	}
}

void XWTeX::deleteSaRef(qint32 q)
{
	qint32 p, i, s;
	
	(sa_ref(q))--;
	if (sa_ref(q) != TEX_NULL)
		return ;
		
	if (false)
	{
		if (sa_int(q) == 0)
			s = TEX_WORD_NODE_SIZE;
		else
			return ;
	}
	else
	{
		if (false)
		{
			if (sa_ptr(q) == zero_glue)
				deleteGlueRef(zero_glue);
			else
				return ;
		}
		else if (sa_ptr(q) != TEX_NULL)
			return;
			
		s = TEX_POINTER_NODE_SIZE;
	}
	
	do
	{
		i = hex_dig4(sa_index(q)); 
		p = q; 
		q = link(p); 
		freeNode(p, s);
		if (q == TEX_NULL)
		{
			sa_mark = TEX_NULL;
			return ;
		}
		
		if (odd(i))
			link(q + (i / 2) + 1) = TEX_NULL;
		else
			info(q + (i / 2) + 1) = TEX_NULL;
		(sa_used(q))--;
		s = TEX_INDEX_NODE_SIZE;
	} while (sa_used(q) <= (quint16)0);
}

void XWTeX::deleteTokenRef(qint32 p)
{
	if (token_ref_count(p) == TEX_NULL)
		flushList(p);
	else
		(token_ref_count(p))--;
}

void XWTeX::doExtension()
{
	qint32 k, p;
	switch (cur_chr)
	{
		case TEX_OPEN_NODE:
			newWriteWhatsIt(TEX_OPEN_NODE_SIZE);
			scanOptionalEquals(); 
			scanFileName();
			open_name(tail) = cur_name; 
			open_area(tail) = cur_area; 
			open_ext(tail)  = cur_ext;
			break;
			
		case TEX_WRITE_NODE:
			k = cur_cs; 
			newWriteWhatsIt(TEX_WRITE_NODE_SIZE);
			cur_cs = k; 
			p = scanToks(false, false); 
			write_tokens(tail) = def_ref;
			break;
			
		case TEX_CLOSE_NODE:
			newWriteWhatsIt(TEX_WRITE_NODE_SIZE); 
			write_tokens(tail) = TEX_NULL;
			break;
			
		case TEX_SPECIAL_NODE:
			newWhatsIt(TEX_SPECIAL_NODE, TEX_WRITE_NODE_SIZE); 
			write_stream(tail) = TEX_NULL;
			p = scanToks(false, true); 
			write_tokens(tail) = def_ref;
			break;
			
		case TEX_IMMEDIATE_CODE:
			getXToken();
			if ((cur_cmd == TEX_EXTENSION) && (cur_chr <= TEX_CLOSE_NODE))
			{
				 p = tail; 
				 doExtension();
				 outWhat(tail);
				 flushNodeList(tail); 
				 tail = p; 
				 link(p) = TEX_NULL;
			}
			else
				backInput();
			break;
			
		case TEX_SET_LANGUAGE_CODE:
			if (qAbs(mode) != TEX_HMODE)
				reportIllegalCase();
			else
			{
				newWhatsIt(TEX_LANGUAGE_NODE, TEX_SMALL_NODE_SIZE);
				scanInt();
				if (cur_val <= 0)
					clang = 0;
				else if (cur_val > TEX_BIGGEST_LANG)
					clang = 0;
				else
					clang = cur_val;
					
				what_lang(tail) = clang;
  				what_lhm(tail)  = (quint16)normMin(leftHyphenMin());
  				what_rhm(tail)  = (quint16)normMin(rightHyphenMin());
			}
			break;
			
		default:
			confusion("ext1");
			break;
	}
}

bool XWTeX::doMarks(qint32 a, qint32 l, qint32 q)
{
	qint32 i;
	if (l < 4)
	{
		for (i = 0; i <= 15; i++)
		{
			if (odd(i))
				cur_ptr = link(q + (i / 2) + 1);
			else
				cur_ptr = info(q + (i / 2) + 1);
				
			if (cur_ptr != TEX_NULL)
			{
				if (doMarks(a, l + 1, cur_ptr))
				{
					if (odd(i))
						link(q + (i / 2) + 1) = TEX_NULL;
					else
						info(q + (i / 2) + 1) = TEX_NULL;
					(sa_used(q))--;
				}
			}
		}
		
		if (sa_used(q) == 0)
		{
			freeNode(q, TEX_INDEX_NODE_SIZE); 
			q = TEX_NULL;
		}
	}
	else
	{
		switch (a)
		{
			case TEX_VSPLIT_INIT:
				if (sa_split_first_mark(q) != TEX_NULL)
				{
					deleteTokenRef(sa_split_first_mark(q)); 
					sa_split_first_mark(q) = TEX_NULL;
  					deleteTokenRef(sa_split_bot_mark(q)); 
  					sa_split_bot_mark(q) = TEX_NULL;
				}
				break;
				
			case TEX_FIRE_UP_INIT:
				if (sa_bot_mark(q) != TEX_NULL)
				{
					if (sa_top_mark(q) != TEX_NULL)
						deleteTokenRef(sa_top_mark(q));
						
					deleteTokenRef(sa_first_mark(q)); 
					sa_first_mark(q) = TEX_NULL;
					if (link(sa_bot_mark(q)) == TEX_NULL)
					{
						deleteTokenRef(sa_bot_mark(q)); 
						sa_bot_mark(q) = TEX_NULL;
					}
					else
						add_token_ref(sa_bot_mark(q));
						
					sa_top_mark(q) = sa_bot_mark(q);
				}
				break;
				
			case TEX_FIRE_UP_DONE:
				if ((sa_top_mark(q) != TEX_NULL) && (sa_first_mark(q) == TEX_NULL))
				{
					sa_first_mark(q) = sa_top_mark(q); 
					add_token_ref(sa_top_mark(q));
				}
				break;
				
			case TEX_DESTROY_MARKS:
				if (ini_version)
				{
					for (i = TEX_TOP_MARK_CODE; i <= TEX_SPLIT_BOT_MARK_CODE; i++)
					{
						if (odd(i))
							cur_ptr = link(q + (i / 2) + 1);
						else
							cur_ptr = info(q + (i / 2) + 1);
							
						if (cur_ptr != TEX_NULL)
						{
							deleteTokenRef(cur_ptr); 
							if (odd(i))
								link(q + (i / 2) + 1) = TEX_NULL;
							else
								info(q + (i / 2) + 1) = TEX_NULL;
						}
					}
				}
				break;
		}
		
		if (sa_bot_mark(q) == TEX_NULL)
		{
			if (sa_split_bot_mark(q) == TEX_NULL)
			{
				freeNode(q, TEX_MARK_CLASS_NODE_SIZE); 
				q = TEX_NULL;
			}
		}
	}
	
	return (q == TEX_NULL);
}

void XWTeX::fetch(qint32 a)
{
	cur_c = character(a); 
	cur_f = famFnt(fam(a) + cur_size);
	if (cur_f == TEX_NULL_FONT)
	{
		printErr(""); 
		printSize(cur_size); 
		printChar(' ');
		printInt(fam(a)); 
		print(tr(" is undefined (character "));
		print(cur_c); 
		printChar(')');
		help4(tr("Somewhere in the math formula just ended, you used the"));
		help_line[2] = tr("stated character from an undefined font family. For example,");
		help_line[1] = tr("plain TeX doesn't allow \\it or \\sl in subscripts. Proceed,");
		help_line[0] = tr("and I'll try to forget that I needed that character.");
		error(); 
		cur_i = null_character; 
		math_type(a) = TEX_EMPTY;
	}
	else
	{
		if ((cur_c >= font_bc(cur_f)) && (cur_c <= font_ec(cur_f)))
			cur_i = char_info(cur_f, cur_c);
		else
			cur_i = null_character;
			
		if (!char_exists(cur_i))
		{
			charWarning(cur_f, cur_c);
    		math_type(a) = TEX_EMPTY;
		}
	}
}

void XWTeX::findSaElement(qint32 t, qint32 n, bool w)
{
	qint32 q, i;
	
	if (t != TEX_MARK_VAL)
	{
		cur_ptr = TEX_NULL;
  		return;
	}
	
	cur_ptr = sa_mark;
	if (cur_ptr == TEX_NULL)
	{
		if (w)
			goto not_found;
		else
			return;
	}
	
	q = cur_ptr; 
	i = hex_dig1(n); 
	if (odd(i))
		cur_ptr = link(q + (i / 2) + 1);
	else
		cur_ptr = info(q + (i / 2) + 1);
		
	if (cur_ptr == TEX_NULL)
	{
		if (w)
			goto not_found1;
		else
			return;
	}
	
	q = cur_ptr; 
	i = hex_dig2(n); 
	if (odd(i))
		cur_ptr = link(q + (i / 2) + 1);
	else
		cur_ptr = info(q + (i / 2) + 1);
		
	if (cur_ptr == TEX_NULL)
	{
		if (w)
			goto not_found2;
		else
			return;
	}
	
	q = cur_ptr; 
	i = hex_dig3(n); 
	if (odd(i))
		cur_ptr = link(q + (i / 2) + 1);
	else
		cur_ptr = info(q + (i / 2) + 1);
		
	if (cur_ptr == TEX_NULL)
	{
		if (w)
			goto not_found3;
		else
			return;
	}
	
	q = cur_ptr; 
	i = hex_dig4(n); 
	if (odd(i))
		cur_ptr = link(q + (i / 2) + 1);
	else
		cur_ptr = info(q + (i / 2) + 1);
		
	if ((cur_ptr == TEX_NULL) && w)
		goto not_found4;
		
	return ;
	
not_found: 
	newIndex(t, TEX_NULL);
	sa_mark = cur_ptr; 
	q = cur_ptr; 
	i = hex_dig1(n);
	
not_found1: 
	newIndex(i, q);
	if (odd(i))
		link(q + (i / 2) + 1) = cur_ptr;
	else
		info(q + (i / 2) + 1) = cur_ptr;
	(sa_used(q))++;
	q = cur_ptr; 
	i = hex_dig2(n);
	
not_found2: 
	newIndex(i, q);
	if (odd(i))
		link(q + (i / 2) + 1) = cur_ptr;
	else
		info(q + (i / 2) + 1) = cur_ptr;
	(sa_used(q))++;
	q = cur_ptr; 
	i = hex_dig3(n);
	
not_found3: 
	newIndex(i, q);
	if (odd(i))
		link(q + (i / 2) + 1) = cur_ptr;
	else
		info(q + (i / 2) + 1) = cur_ptr;
	(sa_used(q))++;
	q = cur_ptr; 
	i = hex_dig4(n);
	
not_found4:
	if (t == TEX_MARK_VAL)
	{
		cur_ptr = getNode(TEX_MARK_CLASS_NODE_SIZE);
  		mem[cur_ptr + 1] = sa_null; 
  		mem[cur_ptr + 2] = sa_null; 
  		mem[cur_ptr + 3] = sa_null;
	}
	
	sa_index(cur_ptr) = (quint16)(256 * t + i); 
	sa_lev(cur_ptr) = (quint16)TEX_LEVEL_ONE;
	
	link(cur_ptr) = q; 
	if (odd(i))
		link(q + (i / 2) + 1) = cur_ptr;
	else
		info(q + (i / 2) + 1) = cur_ptr;
	(sa_used(q))++;;
}

void XWTeX::finAlign()
{
	qint32 p, q, r, s, u, v, t, w, o, n, rule_save;
	TeXMemoryWord aux_save;
	
	if (cur_group != TEX_ALIGN_GROUP)
		confusion("align1");
		
	unsave();
	
	if (cur_group != TEX_ALIGN_GROUP)
		confusion("align0");
		
	unsave();
	
	if (nest[nest_ptr-1].mode_field == TEX_MMODE)
		o = displayIndent();
	else
		o = 0;
		
	q = link(preamble);
	do
	{
		flushList(u_part(q)); 
		flushList(v_part(q));
		p = link(link(q));
		if (width(q) == TEX_NULL_FLAG)
		{
			width(q) = 0; 
			r = link(q); 
			s = glue_ptr(r);
			if (s != zero_glue)
			{
				add_glue_ref(zero_glue); 
				deleteGlueRef(s);
  				glue_ptr(r) = zero_glue;
			}
		}
		
		if (info(q) != end_span)
		{
			t = width(q) + width(glue_ptr(link(q)));
			r = info(q); 
			s = end_span; 
			info(s) = p; 
			n = TEX_MIN_QUARTERWORD + 1;
			do
			{
				width(r) = width(r) - t; 
				u = info(r);
				while (link(r) > n)
				{
					s = info(s); 
					n = link(info(s)) + 1;
				}
				
				if (link(r) < n)
				{
					info(r) = info(s); 
					info(s) = r; 
					(link(r))--; 
					s = r;
				}
				else
				{
					if (width(r) > width(info(s)))
						width(info(s)) = width(r);
					freeNode(r, TEX_SPAN_NODE_SIZE);
				}
				
				r = u;
			} while (r != end_span);
		}
		
		type(q) = (quint16)TEX_UNSET_NODE; 
		span_count(q) = (quint16)TEX_MIN_QUARTERWORD; 
		height(q) = 0;
		depth(q) = 0; 
		glue_order(q) = (quint16)TEX_NORMAL; 
		glue_sign(q)  = (quint16)TEX_NORMAL;
		glue_stretch(q) = 0; 
		glue_shrink(q) = 0; 
		q = p;
	} while (q != TEX_NULL);
	
	save_ptr = save_ptr - 2; 
	pack_begin_line = -mode_line;
	if (mode == -TEX_VMODE)
	{
		rule_save = overFullRule();
  		setNewEqtbSC(TEX_DIMEN_BASE + TEX_OVERFULL_RULE_CODE, 0);
  		p = hpack(preamble, saved(1), saved(0));
  		setNewEqtbSC(TEX_DIMEN_BASE + TEX_OVERFULL_RULE_CODE, rule_save);
	}
	else
	{
		q = link(preamble);
		do
		{
			height(q) = width(q); 
			width(q)  = 0; 
			q = link(link(q));
		} while (q != TEX_NULL);
		
		p = vpack(preamble, saved(1), saved(0));
  		q = link(preamble);
  		
  		do
  		{
  			width(q)  = height(q); 
  			height(q) = 0; 
  			q = link(link(q));
  		} while (q != TEX_NULL);
	}
	
	pack_begin_line = 0;
	
	q = link(head); 
	s = head;
	while (q != TEX_NULL)
	{
		if (!isCharNode(q))
		{
			if (type(q) == (quint16)TEX_UNSET_NODE)
			{
				if (mode == -TEX_VMODE)
				{
					type(q)  = (quint16)TEX_HLIST_NODE; 
					width(q) = width(p);
				}
				else
				{
					type(q)   = (quint16)TEX_VLIST_NODE; 
					height(q) = height(p);
				}
				
				glue_order(q) = glue_order(p); 
				glue_sign(q)  = glue_sign(p);
				glue_set(q)   = glue_set(p); 
				shift_amount(q) = o;
				r = link(list_ptr(q)); 
				s = link(list_ptr(p));
				do
				{
					n = span_count(r); 
					t = width(s); 
					w = t; 
					u = hold_head;
					while (n > TEX_MIN_QUARTERWORD)
					{
						n--;
						s = link(s); 
						v = glue_ptr(s); 
						link(u) = newGlue(v); 
						u = link(u);
						subtype(u) = (quint16)(TEX_TAB_SKIP_CODE + 1); 
						t = t + width(v);
						if (glue_sign(p) == (quint16)TEX_STRETCHING)
						{
							if (stretch_order(v) == glue_order(p))
								t = t + (qint32)qRound(glue_set(p) * stretch(v));
						}
						else if (glue_sign(p) == (quint16)TEX_SHRINKING)
						{
							if (shrink_order(v) == glue_order(p))
								t = t - (qint32)qRound(glue_set(p) * shrink(v));
						}
						
						s = link(s); 
						link(u) = newNullBox(); 
						u = link(u); 
						t = t + width(s);
						if (mode == -TEX_VMODE)
							width(u) = width(s);
						else 
						{
							type(u)   = (quint16)TEX_VLIST_NODE; 
							height(u) = width(s);
						}
					}
					
					if (mode == -TEX_VMODE)
					{
						height(r) = height(q); 
						depth(r)  = depth(q);
						if (t == width(r))
						{
							glue_sign(r) = (quint16)TEX_NORMAL; 
							glue_order(r) = (quint16)TEX_NORMAL;
							glue_set(r) = 0.0;
						}
						else if (t > width(r))
						{
							glue_sign(r) = (quint16)TEX_STRETCHING;
							if (glue_stretch(r) == 0)
								glue_set(r) = 0.0;
							else
								glue_set(r) = (t - width(r)) / glue_stretch(r);
						}
						else
						{
							glue_order(r) = glue_sign(r); 
							glue_sign(r)  = (quint16)TEX_SHRINKING;
							if (glue_shrink(r) == 0)
								glue_set(r) = 0.0;
							else if ((glue_order(r) == (quint16)TEX_NORMAL) && ((width(r) - t) > glue_shrink(r)))
								glue_set(r) = 1.0;
							else
								glue_set(r) = (width(r) - t) / glue_shrink(r);
						}
						
						width(r) = w; 
						type(r) = (quint16)TEX_HLIST_NODE;
					}
					else
					{
						width(r) = width(q);
						if (t == height(r))
						{
							glue_sign(r)  = (quint16)TEX_NORMAL; 
							glue_order(r) = (quint16)TEX_NORMAL;
							glue_set(r) = 0.0;
						}
						else if (t > height(r))
						{
							glue_sign(r) = (quint16)TEX_STRETCHING;
							if (glue_stretch(r) == 0)
								glue_set(r) = 0.0;
							else
								glue_set(r) = (t - height(r)) / glue_stretch(r);
						}
						else
						{
							glue_order(r) = glue_sign(r); 
							glue_sign(r)  = (quint16)TEX_SHRINKING;
							if (glue_shrink(r) == 0)
								glue_set(r) = 0.0;
							else if ((glue_order(r) == (quint16)TEX_NORMAL) && ((height(r) - t) > glue_shrink(r)))
								glue_set(r) = 1.0;
							else
								glue_set(r) = (height(r) - t) / glue_shrink(r);
						}
						
						height(r) = w; 
						type(r) = (quint16)TEX_VLIST_NODE;
					}
					
					shift_amount(r) = 0;
					if (u != hold_head)
					{
						link(u) = link(r); 
						link(r) = link(hold_head); 
						r = u;
					}
					
					r = link(link(r)); 
					s = link(link(s));
				} while (r != TEX_NULL);
			}
			else if (type(q) == (quint16)TEX_RULE_NODE)
			{
				if (isNodeRunning(width(q)))
					width(q) = width(p);
					
				if (isNodeRunning(height(q)))
					height(q) = height(p);
					
				if (isNodeRunning(depth(q)))
					depth(q) = depth(p);
					
				if (o != 0)
				{
					r = link(q); 
					link(q) = TEX_NULL; 
					q = hpack(q, 0, TEX_ADDITIONAL);
  					shift_amount(q) = o; 
  					link(q) = r; 
  					link(s) = q;
				}
			}
		}
		
		s = q; 
		q = link(q);
	}
	
	flushNodeList(p); 
	popAlignment();
	
	aux_save = aux; 
	p = link(head); 
	q = tail; 
	popNest();
	
	if (mode == TEX_MMODE)
	{
		doAssignments();
		if (cur_cmd != TEX_MATH_SHIFT)
		{
			printErr(tr("Missing $$ inserted"));
			help2(tr("Displays can use special alignments (like \\eqalignno)"));
			help_line[0] = tr("only if nothing but the alignment itself is between $$'s.");
			backError();
		}
		else
		{
			getXToken();
			if (cur_cmd != TEX_MATH_SHIFT)
			{
				printErr(tr("Display math should end with $$"));
				help2(tr("The `$' that I just saw supposedly matches a previous `$$'."));
				help_line[0] = tr("So I shall assume that you typed `$$' both times.");
				backError();
			}
		}
		
		popNest();
		tailAppend(newPenalty(predisplayPenalty()));
		tailAppend(newParamGlue(TEX_ABOVE_DISPLAY_SKIP_CODE));
		link(tail) = p;
		if (p != TEX_NULL)
			tail = q;
			
		tailAppend(newPenalty(postdisplayPenalty()));
		tailAppend(newParamGlue(TEX_BELOW_DISPLAY_SKIP_CODE));
		prev_depth = aux_save.ii.CINT0; 
		resumeAfterDisplay();
	}
	else
	{
		aux = aux_save; 
		link(tail) = p;
		if (p != TEX_NULL)
			tail = q;
			
		if (mode == TEX_VMODE)
			buildPage();
	}
}

bool XWTeX::finCol()
{
	qint32 p, q, r, s, u, w, o, n;
	
	if (cur_align == TEX_NULL)
		confusion("endv");
		
	q = link(cur_align);
	if (q == TEX_NULL)
		confusion("endv");
		
	if (align_state < 500000)
	{
		fatalError(tr("(interwoven alignment preambles are not allowed)"));
		return false;
	}
	
	p = link(q);
	if ((p == TEX_NULL) && (extra_info(cur_align) < TEX_CR_CODE))
	{
		if (cur_loop != TEX_NULL)
		{
			link(q) = newNullBox(); 
			p = link(q);
			info(p) = end_span; 
			width(p) = TEX_NULL_FLAG; 
			cur_loop = link(cur_loop);
			q = hold_head; 
			r = u_part(cur_loop);
			while (r != TEX_NULL)
			{
				link(q) = getAvail(); 
				q = link(q); 
				info(q) = info(r); 
				r = link(r);
			}
			
			link(q)   = TEX_NULL; 
			u_part(p) = link(hold_head);
			q = hold_head; 
			r = v_part(cur_loop);
			
			while (r != TEX_NULL)
			{
				link(q) = getAvail(); 
				q = link(q); 
				info(q) = info(r); 
				r = link(r);
			}
			
			link(q)   = TEX_NULL; 
			v_part(p) = link(hold_head);
			cur_loop  = link(cur_loop);
			link(p)   = newGlue(glue_ptr(cur_loop));
		}
		else
		{
			printErr(tr("Extra alignment tab has been changed to "));
			printEsc(TeXCr);
  			help3(tr("You have given more \\span or & marks than there were"));
  			help_line[1] = tr("in the preamble to the \\halign or \\valign now in progress.");
  			help_line[0] = tr("So I'll assume that you meant to type \\cr instead.");
  			extra_info(cur_align) = TEX_CR_CODE; 
  			error();
		}
	}
	
	if (extra_info(cur_align) != TEX_SPAN_CODE)
	{
		unsave(); 
		newSaveLevel(TEX_ALIGN_GROUP);
		if (mode == -TEX_HMODE)
		{
			adjust_tail = cur_tail; 
			u = hpack(link(head), 0, TEX_ADDITIONAL); 
			w = width(u);
  			cur_tail = adjust_tail; 
  			adjust_tail = TEX_NULL;
		}
		else
		{
			u = vpackage(link(head), 0, TEX_ADDITIONAL, 0); 
			w = height(u);
		}
		
		n = TEX_MIN_QUARTERWORD;
		if (cur_span != cur_align)
		{
			q = cur_span;
			do
			{
				n++;
				q = link(link(q));
			} while(q != cur_align);
			
			if (n > TEX_MAX_QUARTERWORD)
				confusion("too many spans");
				
			q = cur_span;
			while (link(info(q)) < n)
				q = info(q);
			if (link(info(q)) > n)
			{
				s = getNode(TEX_SPAN_NODE_SIZE); 
				info(s)  = info(q); 
				link(s)  = n;
  				info(q)  = s; 
  				width(s) = w;
			}
			else if (width(info(q)) < w)
				width(info(q)) = w;
		}
		else if (w > width(cur_align))
			width(cur_align) = w;
			
		type(u) = (quint16)TEX_UNSET_NODE; 
		span_count(u) = (quint16)n;
		
		if (total_stretch[TEX_FILLL] != 0)
			o = TEX_FILLL;
		else if (total_stretch[TEX_FILL] != 0)
			o = TEX_FILL;
		else if (total_stretch[TEX_FIL] != 0)
			o = TEX_FIL;
		else if (total_stretch[TEX_SFI] != 0)
			o = TEX_SFI;
		else
			o = TEX_NORMAL;
			
		glue_order(u)   = (quint16)o; 
		glue_stretch(u) = total_stretch[o];
		
		if (total_shrink[TEX_FILLL] != 0)
			o = TEX_FILLL;
		else if (total_shrink[TEX_FILL] != 0)
			o = TEX_FILL;
		else if (total_shrink[TEX_FIL] != 0)
			o = TEX_FIL;
		else if (total_shrink[TEX_SFI] != 0)
			o = TEX_SFI;
		else
			o = TEX_NORMAL;
			
		glue_sign(u)   = (quint16)o; 
		glue_shrink(u) = total_shrink[o];
		popNest(); 
		link(tail) = u; 
		tail = u;
		
		tailAppend(newGlue(glue_ptr(link(cur_align))));
		subtype(tail) = (quint16)(TEX_TAB_SKIP_CODE + 1);
		if (extra_info(cur_align) >= TEX_CR_CODE)
			return true;
			
		initSpan(p);
	}
	
	align_state = 1000000;
	
	do
	{
		getXOrProtected();
	} while(cur_cmd == TEX_SPACER);
	
	cur_align = p;
	initCol();
	return false;
}

qint32 XWTeX::finMList(qint32 p)
{
	qint32 q;
	if (incompleat_noad != TEX_NULL)
	{
		math_type(denominator(incompleat_noad)) = TEX_SUB_MLIST;
		info(denominator(incompleat_noad)) = link(head);
		if (p == TEX_NULL)
			q = incompleat_noad;
		else
		{
			q = info(numerator(incompleat_noad));
			if ((type(q) != (quint16)TEX_LEFT_NOAD) || (delim_ptr == TEX_NULL))
				confusion("right");
				
			info(numerator(incompleat_noad)) = link(delim_ptr);
  			link(delim_ptr) = incompleat_noad; 
  			link(incompleat_noad) = p;
		}
	}
	else
	{
		link(tail) = p; 
		q = link(head);
	}
	
	popNest(); 
	return q;
}

void XWTeX::finRow()
{
	qint32 p;
	
	if (mode == -TEX_HMODE)
	{
		p = hpack(link(head), 0, TEX_ADDITIONAL);
		popNest(); 
		appendToVList(p);
		if (cur_head != cur_tail)
		{
			link(tail) = link(cur_head); 
			tail = cur_tail;
		}
	}
	else
	{
		p = vpack(link(head), 0, TEX_ADDITIONAL); 
		popNest();
  		link(tail) = p; 
  		tail = p; 
  		space_factor = 1000;
	}
	
	type(p) = (quint16)TEX_UNSET_NODE; 
	glue_stretch(p) = 0;
	if (everyCr() != TEX_NULL)
		beginTokenList(everyCr(), TEX_EVERY_CR_TEXT);
		
	alignPeek();
}

void XWTeX::fixLanguage()
{
	qint32 l;
	if (language() <= 0)
		l = 0;
	else if (language() > TEX_BIGGEST_LANG)
		l = 0;
	else
		l = language();
	if (l != clang)
	{
		newWhatsIt(TEX_LANGUAGE_NODE, TEX_SMALL_NODE_SIZE);
  		what_lang(tail) = l; 
  		clang = l;
  		what_lhm(tail) = (quint16)normMin(leftHyphenMin());
  		what_rhm(tail) = (quint16)normMin(rightHyphenMin());
	}
}

void XWTeX::flushList(qint32 p)
{
	if (p != TEX_NULL)
	{
		qint32 r = p;
		qint32 q = TEX_NULL;
		do
		{
			q = r; 
			r = link(r);
#ifdef XW_TEX_STAT
			dyn_used--;
#endif //XW_TEX_STAT
		} while (r != TEX_NULL);
		
		link(q) = avail; 
		avail = p;
	}
}

void XWTeX::flushMath()
{
	flushNodeList(link(head)); 
	flushNodeList(incompleat_noad);
	link(head) = TEX_NULL; 
	tail = head; 
	incompleat_noad = TEX_NULL;
}

void XWTeX::flushNodeList(qint32 p)
{
	while (p != TEX_NULL)
	{
		qint32 q = link(p);
		if (isCharNode(p))
			freeAvail(p);
		else
		{
			switch (type(p))
			{
				case TEX_HLIST_NODE:
				case TEX_VLIST_NODE:
				case TEX_UNSET_NODE:
					flushNodeList(list_ptr(p));
      				freeNode(p, TEX_BOX_NODE_SIZE); 
      				goto done;
					break;
					
				case TEX_RULE_NODE:
					freeNode(p, TEX_RULE_NODE_SIZE); 
					goto done;
					break;
					
				case TEX_INS_NODE:
					flushNodeList(ins_ptr(p));
      				deleteGlueRef(split_top_ptr(p));
      				freeNode(p, TEX_INS_NODE_SIZE); 
      				goto done;
					break;
					
				case TEX_WHATSIT_NODE:
					{
						switch (subtype(p))
						{
							case TEX_OPEN_NODE:
								freeNode(p, TEX_OPEN_NODE_SIZE);
								break;
								
							case TEX_WRITE_NODE:
							case TEX_SPECIAL_NODE:
								deleteTokenRef(write_tokens(p));
								freeNode(p, TEX_WRITE_NODE_SIZE); 
								goto done;
								break;
								
							case TEX_CLOSE_NODE:
							case TEX_LANGUAGE_NODE:
								freeNode(p, TEX_SMALL_NODE_SIZE);
								break;
								
							case TEX_DIR_NODE:
								freeNode(p, TEX_DIR_NODE_SIZE);
								break;
								
							case TEX_LOCAL_PAR_NODE:
								freeNode(p, TEX_LOCAL_PAR_SIZE);
								break;
								
							default:
								confusion("ext3");
								break;
						}
						
						goto done;
					}
					break;
					
				case TEX_GLUE_NODE:
					fastDeleteGlueRef(glue_ptr(p));
					if (leader_ptr(p) != TEX_NULL)
						flushNodeList(leader_ptr(p));
					break;
					
				case TEX_KERN_NODE:
				case TEX_MATH_NODE:
				case TEX_PENALTY_NODE:
					break;
					
				case TEX_LIGATURE_NODE:
					flushNodeList(lig_ptr(p));
					break;
					
				case TEX_MARK_NODE:
					deleteTokenRef(mark_ptr(p));
					break;
					
				case TEX_DISC_NODE:
					flushNodeList(pre_break(p));
					flushNodeList(post_break(p));
					break;
					
				case TEX_ADJUST_NODE:
					flushNodeList(adjust_ptr(p));
					break;
					
				case TEX_STYLE_NODE:
					freeNode(p, TEX_STYLE_NODE_SIZE); 
					goto done;
					break;
					
				case TEX_CHOICE_NODE:
					flushNodeList(display_mlist(p));
  					flushNodeList(text_mlist(p));
  					flushNodeList(script_mlist(p));
  					flushNodeList(script_script_mlist(p));
  					freeNode(p, TEX_STYLE_NODE_SIZE); 
  					goto done;
					break;
					
				case TEX_ORD_NOAD:
				case TEX_OP_NOAD:
				case TEX_BIN_NOAD:
				case TEX_REL_NOAD:
				case TEX_OPEN_NOAD:
				case TEX_CLOSE_NOAD:
				case TEX_PUNCT_NOAD:
				case TEX_INNER_NOAD:
				case TEX_RADICAL_NOAD:
				case TEX_OVER_NOAD:
				case TEX_UNDER_NOAD:
				case TEX_VCENTER_NOAD:
				case TEX_ACCENT_NOAD:				
					if (math_type(nucleus(p)) >= TEX_SUB_BOX)
						flushNodeList(info(nucleus(p)));
					if (math_type(supscr(p)) >= TEX_SUB_BOX)
						flushNodeList(info(supscr(p)));
					if (math_type(subscr(p)) >= TEX_SUB_BOX)
						flushNodeList(info(subscr(p)));
					if (type(p) == (quint16)TEX_RADICAL_NOAD)
						freeNode(p, TEX_RADICAL_NOAD_SIZE);
					else if (type(p) == (quint16)TEX_ACCENT_NOAD)
						freeNode(p, TEX_ACCENT_NOAD_SIZE);
					else
						freeNode(p, TEX_NOAD_SIZE);
  					goto done;
					break;
					
				case TEX_LEFT_NOAD:
				case TEX_RIGHT_NOAD:
					freeNode(p, TEX_NOAD_SIZE); 
					goto done;
					break;
					
				case TEX_FRACTION_NOAD:
					flushNodeList(info(numerator(p)));
  					flushNodeList(info(denominator(p)));
  					freeNode(p, TEX_FRACTION_NOAD_SIZE); 
  					goto done;
					break;
					
				default:
					confusion("flushing");
					break;
			}
			
			freeNode(p, TEX_SMALL_NODE_SIZE);
		}
done:
		p = q;
	}
}

qint32 XWTeX::fractionRule(qint32 t)
{
	qint32 p = newRule(); 
	rule_dir(p) = mathDirection();
	height(p) = t; 
	depth(p) = 0;
	return p;
}

void XWTeX::freeNode(qint32 p, qint32 s)
{
	node_size(p) = s; 
	link(p) = TEX_EMPTY_FLAG;
	qint32 q = llink(rover); 
	llink(p) = q; 
	rlink(p) = rover;
	llink(rover) = p; 
	rlink(q) = p;
#ifdef XW_TEX_STAT
	var_used = var_used - s;
#endif //XW_TEX_STAT
}

qint32 XWTeX::getAvail()
{
	qint32 p = avail;
	if (p != TEX_NULL)
		avail = link(avail);
	else if (mem_end < mem_max)
	{
		mem_end++; 
		p = mem_end;
	}
	else
	{
		hi_mem_min--; 
		p = hi_mem_min;
		if (hi_mem_min <= lo_mem_max)
		{
			runaway();
			overFlow(tr("main memory size"), mem_max + 1 - mem_min);
			return TEX_NULL;
		}
	}
	
	link(p) = TEX_NULL;
#ifdef XW_TEX_STAT
	dyn_used++;
#endif //XW_TEX_STAT
	return p;
}

qint32 XWTeX::getNode(qint32 s)
{
	qint32 p, q, r, t;
	
restart: 
	p = rover;
	do
	{
		q = p + node_size(p);
		while (isEmpty(q))
		{
			t = rlink(q);
			if (q == rover)
				rover = t;
				
			llink(t) = llink(q); 
			rlink(llink(q)) = t;
			q = q + node_size(q);
		}
		
		r = q - s;
		if (r > (p + 1))
		{
			node_size(p) = r - p;
			rover = p;
			goto found;
		}
		
		if (r == p)
		{
			if (rlink(p) != p)
			{
				rover = rlink(p); 
				t = llink(p);
				llink(rover) = t; 
				rlink(t) = rover;
				goto found;
			}
		}
		
		node_size(p) = q - p;
		p = rlink(p);
	} while (p != rover);
	
	if (s == 1073741824)
		return TEX_MAX_HALFWORD;
	
	if ((lo_mem_max + 2) < hi_mem_min)
	{
		if ((lo_mem_max + 2) <= (mem_bot + TEX_MAX_HALFWORD))
		{
			if ((hi_mem_min - lo_mem_max) >= 1998)
				t = lo_mem_max + 1000;
			else
				t = lo_mem_max + 1 + (hi_mem_min - lo_mem_max) / 2;
				
			p = llink(rover); 
			q = lo_mem_max; 
			rlink(p) = q; 
			llink(rover) = q;
			
			if (t > (mem_bot + TEX_MAX_HALFWORD))
				t = mem_bot + TEX_MAX_HALFWORD;
				
			rlink(q) = rover; 
			llink(q) = p; 
			link(q) = TEX_EMPTY_FLAG; 
			node_size(q) = t - lo_mem_max;
			lo_mem_max = t; 
			link(lo_mem_max) = TEX_NULL; 
			info(lo_mem_max) = TEX_NULL;
			rover = q; 
			goto restart;
		}
	}
	
	overFlow(tr("main memory size"), mem_max + 1 - mem_min);
	return TEX_NULL;
	
found: 
	link(r) = TEX_NULL;
#ifdef XW_TEX_STAT
	var_used = var_used + s;
#endif //XW_TEX_STAT
	return r;
}

void XWTeX::gsaDef(qint32 p, qint32 e)
{
	add_sa_ref(p);
#ifdef XW_TEX_STAT
	if (tracingAssigns() > 0)
		showSa(p, "globally changing");
#endif //XW_TEX_STAT

	saDestroy(p); 
	sa_lev(p) = (quint16)TEX_LEVEL_ONE; 
	sa_ptr(p) = e;
	
#ifdef XW_TEX_STAT
	if (tracingAssigns() > 0)
		showSa(p, "into");
#endif //XW_TEX_STAT
	deleteSaRef(p);
}

void XWTeX::gsaWDef(qint32 p, qint32 w)
{
	add_sa_ref(p);
#ifdef XW_TEX_STAT
	if (tracingAssigns() > 0)
		showSa(p, "globally changing");
#endif //XW_TEX_STAT
	sa_lev(p) = (quint16)TEX_LEVEL_ONE; 
	sa_int(p) = w;
#ifdef XW_TEX_STAT
	if (tracingAssigns() > 0)
		showSa(p, "into");
#endif //XW_TEX_STAT
	deleteSaRef(p);
}

qint32 XWTeX::heightPlusDepth(qint32 f, qint32 c)
{
	TeXFourQuarters q  = char_info(f, c); 
	qint32 hd = height_depth(q);
	return char_height(f, hd) + char_depth(f, hd);
}

qint32 XWTeX::hpack(qint32 p, qint32 w, char m)
{
	qint32 r, q, h, d, x, s, g, o, f;
	TeXFourQuarters i;
	qint32 hd, dir_tmp, dir_ptr, hpack_dir;
	
	last_badness = 0; 
	r = getNode(TEX_BOX_NODE_SIZE); 
	type(r) = (quint16)TEX_HLIST_NODE;
	if (pack_direction == -1)
		box_dir(r) = textDirection();
	else
	{
		box_dir(r) = pack_direction; 
		pack_direction = -1;
	}
	
	hpack_dir = box_dir(r);
	dir_ptr = TEX_NULL; 
	dir_tmp = newDir(hpack_dir);
	link(dir_tmp) = dir_ptr; 
	dir_ptr = dir_tmp;
	
	subtype(r) = (quint16)TEX_MIN_QUARTERWORD; 
	shift_amount(r) = 0;
	q = r + TEX_LIST_OFFSET; 
	link(q) = p;
	h = 0;
	d = 0; 
	x = 0;
	total_stretch[TEX_NORMAL] = 0; 
	total_shrink[TEX_NORMAL] = 0;
	total_stretch[TEX_SFI] = 0; 
	total_shrink[TEX_SFI] = 0;
	total_stretch[TEX_FIL] = 0; 
	total_shrink[TEX_FIL] = 0;
	total_stretch[TEX_FILL] = 0; 
	total_shrink[TEX_FILL] = 0;
	total_stretch[TEX_FILLL] = 0; 
	total_shrink[TEX_FILLL] = 0;
	
	while (p != TEX_NULL)
	{
		
reswitch:
		while (isCharNode(p))
		{
			f = font(p); 
			i = char_info(f, character(p)); 
			hd = height_depth(i);
			if (is_rotated(hpack_dir))
			{
				x = x + char_height(f, hd) + char_depth(f, hd);
  				s = char_width(f, i) / 2;
  				if (s > h) 
  					h = s;
  				s = char_width(f, i) / 2;
  				if (s > d) 
  					d = s;
			}
			else if (dir_opposite(dir_tertiary[hpack_dir], dir_tertiary[box_dir(r)]))
			{
				x = x + char_width(f, i);
  				s = char_depth(f, hd);
  				if (s > h) 
  					h = s;
  				s = char_height(f, hd);
  				if (s > d) 
  					d = s;
			}
			else
			{
				x = x + char_width(f, i); 
  				s = char_height(f, hd);
  				if (s > h) 
  					h = s;
  				s= char_depth(f, hd);
  				if (s > d) 
  					d = s;
			}
			
			p = link(p);
		}
		
		if (p != TEX_NULL)
		{
			switch (type(p))
			{
				case TEX_HLIST_NODE:
				case TEX_VLIST_NODE:
				case TEX_RULE_NODE:
				case TEX_UNSET_NODE:
					if ((type(p) == (quint16)TEX_HLIST_NODE) || (type(p) == (quint16)TEX_VLIST_NODE))
					{
						if (dir_orthogonal(dir_primary[box_dir(p)], dir_primary[hpack_dir]))
						{
							x = x + height(p) + depth(p);
    						s = shift_amount(p);
    						if (((width(p) / 2) - s) > h)
    							h = (width(p) / 2) - s;
    							
    						if (((width(p) / 2) + s) > d)
    							d = (width(p) / 2) + s;
						}
						else if ((type(p) == (quint16)TEX_HLIST_NODE) && (is_mirrored(hpack_dir)))
						{
							x = x + width(p);
    						s = shift_amount(p);
    						if ((depth(p) - s) > h)
    							h = depth(p) - s;
    							
    						if ((height(p) + s) > d)
    							d = height(p) + s;
						}
						else
						{
							x = x + width(p);
    						s = shift_amount(p);
    						if ((height(p) - s) > h)
    							h = height(p) - s;
    							
    						if ((depth(p) + s) > d)
    							d = depth(p) + s;
						}
					}
					else
					{
						x = x + width(p);
						if (type(p) >= (quint16)TEX_RULE_NODE)
							s = 0;
						else
							s = shift_amount(p);
							
						if ((height(p) - s) > h)
							h = height(p) - s;
							
						if ((depth(p) + s) > d)
							d = depth(p) + s;
					}
					break;
					
				case TEX_INS_NODE:
				case TEX_MARK_NODE:
				case TEX_ADJUST_NODE:
					if (adjust_tail != TEX_NULL)
					{
						while (link(q) != p)
							q = link(q);
							
						if (type(p) == (quint16)TEX_ADJUST_NODE)
						{
							link(adjust_tail) = adjust_ptr(p);
							while (link(adjust_tail) != TEX_NULL)
								adjust_tail = link(adjust_tail);
								
							p = link(p); 
							freeNode(link(q), TEX_SMALL_NODE_SIZE);
						}
						else
						{
							link(adjust_tail) = p; 
							adjust_tail = p; 
							p = link(p);
						}
						
						link(q) = p; 
						p = q;
					}
					break;
					
				case TEX_WHATSIT_NODE:
					if (subtype(p) == (quint16)TEX_DIR_NODE)
					{
						if (dir_dir(p) >= 0)
						{
							hpack_dir = dir_dir(p);
							dir_tmp = getNode(TEX_DIR_NODE_SIZE);
							type(dir_tmp) = (quint16)TEX_WHATSIT_NODE; 
							subtype(dir_tmp) = (quint16)TEX_DIR_NODE;
							dir_dir(dir_tmp) = dir_dir(p);
							dir_level(dir_tmp) = dir_level(p);
							dir_dvi_h(dir_tmp) = dir_dvi_h(p);
							dir_dvi_ptr(dir_tmp) = dir_dvi_ptr(p);
							link(dir_tmp) = dir_ptr; 
							dir_ptr = dir_tmp;
						}
						else
						{
							dir_tmp = dir_ptr;
							dir_ptr = link(dir_tmp);
							freeNode(dir_tmp, TEX_DIR_NODE_SIZE);
							hpack_dir = dir_dir(dir_ptr);
						}
					}
					break;
					
				case TEX_GLUE_NODE:
					g = glue_ptr(p); 
					x = x + width(g);
					o = stretch_order(g); 
					total_stretch[o] = total_stretch[o] + stretch(g);
					o = shrink_order(g); 
					total_shrink[o] = total_shrink[o] + shrink(g);
					if (subtype(p) >= (quint16)TEX_A_LEADERS)
					{
						g = leader_ptr(p);
						if (height(g) > h)
							h = height(g);
							
						if (depth(g) > d)
							d = depth(g);
					}
					break;
					
				case TEX_KERN_NODE:
				case TEX_MATH_NODE:
					x = x + width(p);
					break;
					
				case TEX_LIGATURE_NODE:
					mem[lig_trick] = mem[lig_char(p)]; 
					link(lig_trick) = link(p);
					p = lig_trick; 
					goto reswitch;
					break;
					
				default:
					break;
			}
			
			p = link(p);
		}
	}
	
	if (adjust_tail != TEX_NULL)
		link(adjust_tail) = TEX_NULL;
		
	height(r) = h; 
	depth(r) = d;
	if (m == TEX_ADDITIONAL)
		w = x + w;
		
	width(r) = w; 
	x = w - x;
	if (x == 0)
	{
		glue_sign(r) = (quint16)TEX_NORMAL; 
		glue_order(r) = (quint16)TEX_NORMAL;
  		glue_set(r) = 0.0;
  		while (dir_ptr != TEX_NULL)
		{
			dir_tmp = dir_ptr;
			dir_ptr = link(dir_tmp);
			freeNode(dir_tmp, TEX_DIR_NODE_SIZE);
		}
  		return r;
	}
	else if (x > 0)
	{
		if (total_stretch[TEX_FILLL] != 0)
			o = TEX_FILLL;
		else if (total_stretch[TEX_FILL] != 0)
			o = TEX_FILL;
		else if (total_stretch[TEX_FIL] != 0)
			o = TEX_FIL;
		else if (total_stretch[TEX_SFI] != 0)
			o = TEX_SFI;
		else
			o = TEX_NORMAL;
			
		glue_order(r) = (quint16)o; 
		glue_sign(r) = (quint16)TEX_STRETCHING;
		if (total_stretch[o] != 0)
			glue_set(r) = (x / total_stretch[o]);
		else
		{
			glue_sign(r) = (quint16)TEX_NORMAL;
			glue_set(r) = 0.0;
		}
		
		if (o == TEX_NORMAL)
		{
			if (list_ptr(r) != TEX_NULL)
			{
				last_badness = badness(x, total_stretch[TEX_NORMAL]);
				if (last_badness > hbadness())
				{
					println();
					if (last_badness > 100)
						printnl(tr("Underfull"));
					else
						printnl(tr("Loose"));
						
					print(tr(" \\hbox (badness ")); 
					printInt(last_badness);
					goto common_ending;
				}
			}
		}
		
		while (dir_ptr != TEX_NULL)
		{
			dir_tmp = dir_ptr;
			dir_ptr = link(dir_tmp);
			freeNode(dir_tmp, TEX_DIR_NODE_SIZE);
		}
		
		return r;
	}
	else
	{
		if (total_shrink[TEX_FILLL] != 0)
			o = TEX_FILLL;
		else if (total_shrink[TEX_FILL] != 0)
			o = TEX_FILL;
		else if (total_shrink[TEX_FIL] != 0)
			o = TEX_FIL;
		else if (total_shrink[TEX_SFI] != 0)
			o = TEX_SFI;
		else
			o = TEX_NORMAL;
			
		glue_order(r) = (quint16)o; 
		glue_sign(r) = (quint16)TEX_SHRINKING;
		if (total_shrink[o] != 0)
			glue_set(r) = ((-x) / total_shrink[o]);
		else
		{
			glue_sign(r) = (quint16)TEX_NORMAL;
			glue_set(r) = 0.0;
		}
		
		if ((total_shrink[o] < -x) && 
			(o == TEX_NORMAL) && 
			(list_ptr(r) != TEX_NULL))
		{
			last_badness = 1000000;
			glue_set(r) = 1.0;
			if ((-x - total_shrink[TEX_NORMAL] > hfuzz()) || (hbadness() < 100))
			{
				if ((overFullRule() > 0) && (-x - total_shrink[TEX_NORMAL] > hfuzz()))
				{
					while (link(q) != TEX_NULL)
						q = link(q);
						
					link(q) = newRule(); 
					rule_dir(link(q)) = box_dir(r);
    				width(link(q)) = overFullRule();
				}
				
				println(); 
				printnl(tr("Overfull \\hbox ("));
				printScaled(-x - total_shrink[TEX_NORMAL]); 
				print(tr("pt too wide"));
  				goto common_ending;
			}
		}
		while (dir_ptr != TEX_NULL)
		{
			dir_tmp = dir_ptr;
			dir_ptr = link(dir_tmp);
			freeNode(dir_tmp, TEX_DIR_NODE_SIZE);
		}
		return r;
	}
	
common_ending:
	if (output_active)
		print(tr(") has occurred while \\output is active"));
	else
	{
		if (pack_begin_line != 0)
		{
			if (pack_begin_line > 0)
				print(tr(") in paragraph at lines "));
			else
				print(tr(") in alignment at lines "));
				
			printInt(qAbs(pack_begin_line));
    		print("--");
		}
		else
			print(tr(") detected at line "));
			
		printInt(line);		
	}
	
	println();
	font_in_short_display = TEX_NULL_FONT; 
	shortDisplay(list_ptr(r)); 
	println();
	beginDiagnostic(); 
	showBox(r); 
	endDiagnostic(true);
	
	while (dir_ptr != TEX_NULL)
	{
		dir_tmp = dir_ptr;
		dir_ptr = link(dir_tmp);
		freeNode(dir_tmp, TEX_DIR_NODE_SIZE);
	}
	
	return r;
}

void XWTeX::indentInHMode()
{
	qint32 p, q;
	
	if (cur_chr > 0)
	{
		p = newNullBox(); 
		width(p) = parIndent();
		if (qAbs(mode) == TEX_HMODE)
			space_factor = 1000;
		else
		{
			q = newNoad(); 
			math_type(nucleus(q)) = TEX_SUB_BOX;
    		info(nucleus(q)) = p; 
    		p = q;
		}
		
		tailAppend(p);
	}
}

void XWTeX::initAlign()
{
	qint32 save_cs_ptr, p;
	
	save_cs_ptr = cur_cs;
	pushAlignment(); 
	align_state = -1000000;
	if ((mode == TEX_MMODE) && 
		((tail != head) || (incompleat_noad != TEX_NULL)))
	{
		printErr(tr("Improper ")); 
		printEsc(TeXHAlign); 
		print(tr(" inside $$'s"));
		help3(tr("Displays can use special alignments (like \\eqalignno)"));
		help_line[1] = tr("only if nothing but the alignment itself is between $$'s.");
		help_line[0] = tr("So I've deleted the formulas that preceded this alignment.");
		error(); 
		flushMath();
	}
	
	pushNest();
	if (mode == TEX_MMODE)
	{
		mode = -TEX_VMODE; 
		prev_depth = nest[nest_ptr-2].aux_field.ii.CINT0;
	}
	else if (mode > 0)
		mode = -mode;
		
	scanSpec(TEX_ALIGN_GROUP, false);
	preamble  = TEX_NULL; 
	cur_align = align_head; 
	cur_loop  = TEX_NULL; 
	scanner_status = TEX_ALIGNING;
	warning_index = save_cs_ptr; 
	align_state = -1000000;
	
	while (true)
	{
		link(cur_align) = newParamGlue(TEX_TAB_SKIP_CODE);
		cur_align = link(cur_align);
		if (cur_cmd == TEX_CAR_RET)
			goto done;
			
		p = hold_head; 
		link(p) = TEX_NULL;
		while (true)
		{
			getPreambleToken();
			if (cur_cmd == TEX_MAC_PARAM)
				goto done1;
				
			if ((cur_cmd <= TEX_CAR_RET) && 
				(cur_cmd >= TEX_TAB_MARK) && 
				(align_state == -1000000))
			{
				if ((p == hold_head) && 
					(cur_loop == TEX_NULL) && 
					(cur_cmd == TEX_TAB_MARK))
				{
					cur_loop = cur_align;
				}
				else
				{
					printErr(tr("Missing # inserted in alignment preamble"));
					help3(tr("There should be exactly one # between &'s, when an"));
					help_line[1] = tr("\\halign or \\valign is being set up. In this case you had");
					help_line[0] = tr("none, so I've put one in; maybe that will work.");
					backError(); 
					goto done1;
				}
			}
			else if ((cur_cmd != TEX_SPACER) || (p != hold_head))
			{
				link(p) = getAvail(); 
				p = link(p); 
				info(p) = cur_tok;
			}
		}
		
done1:
		link(cur_align) = newNullBox(); 
		cur_align = link(cur_align);
		info(cur_align) = end_span; 
		width(cur_align) = TEX_NULL_FLAG;
		u_part(cur_align) = link(hold_head);
		p = hold_head; 
		link(p) = TEX_NULL;
		while (true)
		{
tcontinue:
			getPreambleToken();
			if ((cur_cmd <= TEX_CAR_RET) && 
				(cur_cmd >= TEX_TAB_MARK) && 
				(align_state=-1000000))
			{
				goto done2;
			}
			
			if (cur_cmd == TEX_MAC_PARAM)
			{
				printErr(tr("Only one # is allowed per tab"));
				help3(tr("There should be exactly one # between &'s, when an"));
				help_line[1] = tr("\\halign or \\valign is being set up. In this case you had");
				help_line[0] = tr("more than one, so I'm ignoring all but the first.");
				error(); 
				goto tcontinue;
			}
			
			link(p) = getAvail(); 
			p = link(p); 
			info(p) = cur_tok;
		}
done2: 
		link(p) = getAvail(); 
		p = link(p);
		info(p)=TEX_END_TEMPLATE_TOKEN;
		v_part(cur_align) = link(hold_head);
	}
	
done: 
	scanner_status = TEX_NORMAL;
	newSaveLevel(TEX_ALIGN_GROUP);
	if (everyCr() != TEX_NULL)
		beginTokenList(everyCr(), TEX_EVERY_CR_TEXT);
		
	alignPeek();
}

void XWTeX::initCol()
{
	extra_info(cur_align) = cur_cmd;
	if (cur_cmd == TEX_OMIT)
		align_state = 0;
	else
	{
		backInput(); 
		beginTokenList(u_part(cur_align), TEX_U_TEMPLATE);
	}
}

void XWTeX::initMath()
{
	qint32 w, l, s, p, q, f, n, v, d;
	
	getToken();
	if ((cur_cmd == TEX_MATH_SHIFT) && (mode > 0))
	{
		if (head == tail)
		{
			popNest(); 
			w = -TEX_MAX_DIMEN;
		}
		else
		{
			lineBreak(displayWidowPenalty());
			v = shift_amount(just_box) + 2 * quad(curFont()); 
			w = -TEX_MAX_DIMEN;
			p = list_ptr(just_box);
			while (p != TEX_NULL)
			{
reswitch:
				if (isCharNode(p))
				{
					f = font(p); 
					d = char_width(f, char_info(f, character(p)));
  					goto found;
				}
				
				switch (type(p))
				{
					case TEX_HLIST_NODE:
					case TEX_VLIST_NODE:
					case TEX_RULE_NODE:
						d = width(p); 
						goto found;
						break;
						
					case TEX_LIGATURE_NODE:
						mem[lig_trick] = mem[lig_char(p)]; 
						link(lig_trick) = link(p);
						p = lig_trick; 
						goto reswitch;
						break;
						
					case TEX_KERN_NODE:
					case TEX_MATH_NODE: 
						d = width(p);
						break;
						
					case TEX_GLUE_NODE:
						q = glue_ptr(p); 
						d = width(q);
						if (glue_sign(just_box) == (quint16)TEX_STRETCHING)
						{
							if ((glue_order(just_box) == stretch_order(q)) && (stretch(q) != 0))
								v = TEX_MAX_DIMEN;
						}
						else if (glue_sign(just_box) == (quint16)TEX_SHRINKING)
						{
							if ((glue_order(just_box) == shrink_order(q)) && (shrink(q) != 0))
								v = TEX_MAX_DIMEN;
						}
						if (subtype(p) >= (quint16)TEX_A_LEADERS)
							goto found;
						break;
						
					case TEX_WHATSIT_NODE:
						d = 0;
						break;
						
					default:
						d = 0;
						break;
				}
				
				if (v < TEX_MAX_DIMEN)
					v = v + d;
  				goto not_found;
  				
found:
				if (v < TEX_MAX_DIMEN)
				{
					v = v + d; 
					w = v;
				}
				else
				{
					w = TEX_MAX_DIMEN; 
					goto done;
				}
				
not_found: 
				p = link(p);
			}
			
done:
			;
		}
		
		if (parShapePtr() == TEX_NULL)
		{
			if ((hangIndent() != 0) && 
				(((hangAfter() >= 0) && 
				((prev_graf + 2) > hangAfter())) || 
				((prev_graf + 1) < -hangAfter())))
			{
				l = hsize() - qAbs(hangIndent());
				if (hangIndent() > 0)
					s = hangIndent();
				else
					s = 0;
			}
			else
			{
				l = hsize(); 
				s = 0;
			}
		}
		else
		{
			n = info(parShapePtr());
			if ((prev_graf + 2) >= n)
				p = parShapePtr() + 2 * n;
			else
				p = parShapePtr() + 2 * (prev_graf + 2);
				
			s = mem[p-1].ii.CINT0; 
			l = mem[p].ii.CINT0;
		}
		
		pushMath(TEX_MATH_SHIFT_GROUP); 
		mode = TEX_MMODE;
		eqWordDefine(TEX_INT_BASE + TEX_CUR_FAM_CODE, -1);
		eqWordDefine(TEX_DIMEN_BASE + TEX_PRE_DISPLAY_SIZE_CODE, w);
		eqWordDefine(TEX_DIMEN_BASE + TEX_DISPLAY_WIDTH_CODE, l);
		eqWordDefine(TEX_DIMEN_BASE + TEX_DISPLAY_INDENT_CODE, s);
		if (everyDisplay() != TEX_NULL)
			beginTokenList(everyDisplay(), TEX_EVERY_DISPLAY_TEXT);
			
		if (nest_ptr == 1)
			buildPage();
	}
	else
	{
		backInput();
		pushMath(TEX_MATH_SHIFT_GROUP); 
		eqWordDefine(TEX_INT_BASE + TEX_CUR_FAM_CODE, -1);
		if (insert_src_special_every_math)
			insertSrcSpecial();
		if (everyMath() != TEX_NULL)
			beginTokenList(everyMath(), TEX_EVERY_MATH_TEXT);
	}
}

void XWTeX::initRow()
{
	pushNest(); 
	mode = (-TEX_HMODE - TEX_VMODE) - mode;
	if (mode == -TEX_HMODE)
		space_factor = 0;
	else
		prev_depth = 0;
		
	tailAppend(newGlue(glue_ptr(preamble)));
	subtype(tail) = (quint16)(TEX_TAB_SKIP_CODE + 1);
	cur_align = link(preamble); 
	cur_tail = cur_head; 
	initSpan(cur_align);
}

void XWTeX::initSpan(qint32 p)
{
	pushNest();
	if (mode == -TEX_HMODE)
		space_factor = 1000;
	else
	{
		prev_depth = TEX_IGNORE_DEPTH; 
		normalParagraph();
	}
	
	cur_span = p;
}

void XWTeX::makeAccent()
{
	qint32 p, q, r, f, a, x, w, h, delta;
	TeXFourQuarters i;
	double s, t;
	
	scanCharNum(); 
	f = curFont(); 
	p = newCharacter(f, cur_val);
	if (p != TEX_NULL)
	{
		x = x_height(f); 
		s = slant(f) / 65536.0;
		a = char_width(f, char_info(f, character(p)));
  		doAssignments();
  		q = TEX_NULL; 
  		f = curFont();
  		if ((cur_cmd == TEX_LETTER) || (cur_cmd == TEX_OTHER_CHAR) || (cur_cmd == TEX_CHAR_GIVEN))
  			q = newCharacter(f, cur_chr);
  		else if (cur_cmd == TEX_CHAR_NUM)
  		{
  			scanCharNum(); 
  			q = newCharacter(f, cur_val);
  		}
  		else 
  			backInput();
  			
  		if (q != TEX_NULL)
  		{
  			t = slant(f) / 65536.0;
  			i = char_info(f, character(q));
			w = char_width(f, i); 
			h = char_height(f, height_depth(i));
			if (h != x)
			{
				p = hpack(p, 0, TEX_ADDITIONAL); 
				shift_amount(p) = x - h;
			}
			
			delta = round((w - a) / 2.0 + h * t - x * s);
			r = newKern(delta); 
			subtype(r) = (quint16)TEX_ACC_KERN; 
			link(tail) = r; 
			link(r) = p;
			tail = newKern(-a - delta); 
			subtype(tail) = (quint16)TEX_ACC_KERN; 
			link(p) = tail; 
			p = q;
  		}
  		
  		link(tail) = p; 
  		tail = p; 
  		space_factor = 1000;
	}
}

void XWTeX::makeFraction(qint32 q)
{
	qint32 p, v, x, y, z;
	qint32 delta, delta1, delta2, shift_up, shift_down, clr;
	if (thickness(q) == TEX_DEFAULT_CODE)
		thickness(q) = default_rule_thickness;
		
	x = cleanBox(numerator(q), num_style(cur_style));
	z = cleanBox(denominator(q), denom_style(cur_style));
	if (width(x) < width(z))
		x = rebox(x, width(z));
	else
		z = rebox(z, width(x));
		
	if (cur_style < TEX_TEXT_STYLE)
	{
		shift_up = num1(cur_size); 
		shift_down = denom1(cur_size);
	}
	else
	{
		shift_down = denom2(cur_size);
		if (thickness(q) != 0)
			shift_up = num2(cur_size);
		else
			shift_up = num3(cur_size);
	}
	
	if (thickness(q) == 0)
	{
		if (cur_style < TEX_TEXT_STYLE)
			clr = 7 * default_rule_thickness;
		else
			clr = 3 * default_rule_thickness;
			
		delta = half(clr - ((shift_up - depth(x)) - (height(z) - shift_down)));
		if (delta > 0)
		{
			shift_up = shift_up + delta;
  			shift_down = shift_down + delta;
		}
	}
	else
	{
		if (cur_style < TEX_TEXT_STYLE)
			clr = 3 * thickness(q);
		else
			clr = thickness(q);
			
		delta = half(thickness(q));
		delta1 = clr - ((shift_up - depth(x)) - (axis_height(cur_size) + delta));
		delta2 = clr - ((axis_height(cur_size) - delta) - (height(z) - shift_down));
		if (delta1 > 0)
			shift_up = shift_up + delta1;
			
		if (delta2 > 0)
			shift_down = shift_down + delta2;
	}
	
	v = newNullBox(); 
	type(v) = (quint16)TEX_VLIST_NODE;
	height(v) = shift_up + height(x); 
	depth(v) = depth(z) + shift_down;
	width(v) = width(x);
	if (thickness(q) == 0)
	{
		p = newKern((shift_up - depth(x)) - (height(z) - shift_down));
  		link(p) = z;
	}
	else
	{
		y = fractionRule(thickness(q));
  		p = newKern((axis_height(cur_size) - delta)- (height(z) - shift_down));
  		link(y) = p; 
  		link(p) = z;
  		p = newKern((shift_up - depth(x)) - (axis_height(cur_size) + delta));
  		link(p) = y;
	}
	link(x) = p; 
	list_ptr(v) = x;
	
	if (cur_style < TEX_TEXT_STYLE)
		delta = delim1(cur_size);
	else
		delta = delim2(cur_size);
		
	x = varDelimiter(left_delimiter(q), cur_size, delta); 
	link(x) = v;
	z = varDelimiter(right_delimiter(q), cur_size, delta); 
	link(v) = z;
	new_hlist(q) = hpack(x, 0, TEX_ADDITIONAL);
}

qint32 XWTeX::makeLeftRight(qint32 q, 
	                        qint32 style, 
	                        qint32 max_d, 
	                        qint32 max_h)
{
	qint32 delta, delta1, delta2;
	cur_style = style;
	if (cur_style < TEX_SCRIPT_STYLE)
		cur_size = TEX_TEXT_SIZE;
	else
		cur_size = TEX_SCRIPT_SIZE * ((cur_style - TEX_TEXT_STYLE) / 2);
		
	cur_mu = xOverN(math_quad(cur_size), 18);
	
	delta2 = max_d + axis_height(cur_size);
	delta1 = max_h + max_d - delta2;
	if (delta2 > delta1)
		delta1 = delta2;
		
	delta  = (delta1 / 500) * delimiterFactor();
	delta2 = delta1 + delta1 - delimiterShortFall();
	if (delta < delta2)
		delta = delta2;
	new_hlist(q) = varDelimiter(delimiter(q), cur_size, delta);
	return (type(q) - (quint16)(TEX_LEFT_NOAD - TEX_OPEN_NOAD));
}

qint32 XWTeX::makeLocalParNode()
{
	qint32 p = getNode(TEX_LOCAL_PAR_SIZE); 
	type(p)  = (quint16)TEX_WHATSIT_NODE;
	subtype(p) = (quint16)TEX_LOCAL_PAR_NODE; 
	link(p) = TEX_NULL;
	local_pen_inter(p)  = localInterLinePenalty();
	local_pen_broken(p) = localBrokenPenalty();
	if (localLeftBox() == TEX_NULL)
	{
		local_box_left(p) = TEX_NULL;
  		local_box_left_width(p) = 0;
	}
	else
	{
		local_box_left(p) = copyNodeList(localLeftBox());
  		local_box_left_width(p) = width(localLeftBox());
	}
	
	if (localRightBox() == TEX_NULL)
	{
		local_box_right(p) = TEX_NULL;
  		local_box_right_width(p) = 0;
	}
	else
	{
		local_box_right(p) = copyNodeList(localRightBox());
  		local_box_right_width(p) = width(localRightBox());
	}
	
	local_par_dir(p) = parDirection();
	return p;
}

void  XWTeX::makeMark()
{
	qint32 p, c;
	
	if (cur_chr == 0)
		c = 0;
	else 
	{
		scanRegisterNum(); 
		c = cur_val;
	}
	
	p = scanToks(false,true); 
	p = getNode(TEX_SMALL_NODE_SIZE);
	mark_class(p) = c;
	type(p) = (quint16)TEX_MARK_NODE; 
	subtype(p) = (quint16)0;
	mark_ptr(p) = def_ref; 
	link(tail) = p; 
	tail = p;
}

void XWTeX::makeMathAccent(qint32 q)
{
	qint32 p, x, y, a, c, f, s, h, delta, w;
	TeXFourQuarters i;
	
	fetch(accent_chr(q));
	if (char_exists(cur_i))
	{
		i = cur_i; 
		c = cur_c; 
		f = cur_f;
		s = 0;
		if (math_type(nucleus(q)) == TEX_MATH_CHAR)
		{
			fetch(nucleus(q));
			if (char_tag(cur_i) == (quint16)TEX_LIG_TAG)
			{
				a = lig_kern_start(cur_f, cur_i);
				cur_i = font_info(cur_f, a).qqqq;
				if (skip_byte(cur_i) > TEX_STOP_FLAG)
				{
					a = lig_kern_restart(cur_f, cur_i);
      				cur_i = font_info(cur_f, a).qqqq;
				}
				
				while (true)
				{
					if (next_char(cur_f, cur_i) == skew_char(cur_f))
					{
						if (op_byte(cur_i) >= (quint16)TEX_KERN_FLAG)
						{
							if (skip_byte(cur_i) <= TEX_STOP_FLAG)
								s = char_kern(cur_f, cur_i);
								
							goto done1;
						}
					}
					
					if (skip_byte(cur_i) >= TEX_STOP_FLAG)
						goto done1;
						
					a = a + skip_byte(cur_i) + 1;
      				cur_i = font_info(cur_f, a).qqqq;
				}
			}
		}
		
done1:
		x = cleanBox(nucleus(q), cramped_style(cur_style)); 
		w = width(x); 
		h = height(x);
		while (true)
		{
			if (char_tag(i) != (quint16)TEX_LIST_TAG)
				goto done;
				
			y = rem_byte(i);
  			i = char_info(f, y);
  			if (!char_exists(i))
  				goto done;
  				
  			if (char_width(f, i) > w)
  				goto done;
  				
  			c = y;
		}
		
done:
		if (h < x_height(f))
			delta = h;
		else
			delta = x_height(f);
			
		if ((math_type(supscr(q)) != TEX_EMPTY) || 
			(math_type(subscr(q)) != TEX_EMPTY))
		{
			if (math_type(nucleus(q)) == TEX_MATH_CHAR)
			{
				flushNodeList(x); 
				x = newNoad();
				mem[nucleus(x)] = mem[nucleus(q)];
				mem[supscr(x)]  = mem[supscr(q)];
				mem[subscr(x)]  = mem[subscr(q)];
				mem[supscr(q)].hh = empty_field;
				mem[subscr(q)].hh = empty_field;
				math_type(nucleus(q)) = TEX_SUB_MLIST; 
				info(nucleus(q)) = x;
				x = cleanBox(nucleus(q), cur_style); 
				delta = delta + height(x) - h; 
				h = height(x);
			}
		}
		
		y = charBox(f, c);
  		shift_amount(y) = s + half(w - width(y));
  		width(y) = 0; 
  		p = newKern(-delta); 
  		link(p) = x; 
  		link(y) = p;
  		pack_direction = mathDirection();
  		y = vpack(y, 0, TEX_ADDITIONAL); 
  		width(y) = width(x);
  		if (height(y) < h)
  		{
  			p = newKern(h - height(y)); 
  			link(p) = list_ptr(y); 
  			list_ptr(y) = p;
			height(y) = h;
  		}
  		
  		info(nucleus(q)) = y;
  		math_type(nucleus(q)) = TEX_SUB_BOX;
	}
}

qint32 XWTeX::makeOp(qint32 q)
{
	qint32 delta, p, v, x, y, z, shift_up, shift_down;
	quint16 c;
	TeXFourQuarters i;
	
	if ((subtype(q) == (quint16)TEX_NORMAL) && (cur_style < TEX_TEXT_STYLE))
		subtype(q) = (quint16)TEX_LIMITS;
		
	if (math_type(nucleus(q)) == TEX_MATH_CHAR)
	{
		fetch(nucleus(q));
		if ((cur_style < TEX_TEXT_STYLE) && (char_tag(cur_i) == (quint16)TEX_LIST_TAG))
		{
			c = rem_byte(cur_i); 
			i = char_info(cur_f, c);
			if (char_exists(i))
			{
				cur_c = c; 
				cur_i = i; 
				character(nucleus(q)) = (quint16)c;
			}
		}
		
		delta = char_italic(cur_f, cur_i); 
		x = cleanBox(nucleus(q), cur_style);
		if ((math_type(subscr(q)) != (quint16)TEX_EMPTY) && (subtype(q) != (quint16)TEX_LIMITS))
			width(x) = width(x) - delta;
			
		shift_amount(x) = half(height(x) - depth(x)) - axis_height(cur_size);
		math_type(nucleus(q)) = TEX_SUB_BOX; 
		info(nucleus(q)) = x;
	}
	else
		delta = 0;
		
	if (subtype(q) == (quint16)TEX_LIMITS)
	{
		x = cleanBox(supscr(q), sup_style(cur_style));
		y = cleanBox(nucleus(q), cur_style);
		z = cleanBox(subscr(q), sub_style(cur_style));
		v = newNullBox(); 
		type(v) = (quint16)TEX_VLIST_NODE; 
		width(v) = width(y);
		if (width(x) > width(v))
			width(v) = width(x);
			
		if (width(z) > width(v))
			width(v) = width(z);
			
		x = rebox(x,width(v)); 
		y = rebox(y,width(v)); 
		z = rebox(z,width(v));
		shift_amount(x) = half(delta); 
		shift_amount(z) = -shift_amount(x);
		height(v) = height(y); 
		depth(v) = depth(y);
		if (math_type(supscr(q)) == TEX_EMPTY)
		{
			freeNode(x, TEX_BOX_NODE_SIZE); 
			list_ptr(v) = y;
		}
		else
		{
			shift_up = big_op_spacing3 - depth(x);
			if (shift_up < big_op_spacing1)
				shift_up = big_op_spacing1;
				
			p = newKern(shift_up); 
			link(p) = y; 
			link(x) = p;
  			p = newKern(big_op_spacing5); 
  			link(p) = x; 
  			list_ptr(v) = p;
  			height(v) = height(v) + big_op_spacing5 + height(x) + depth(x) + shift_up;
		}
		
		if (math_type(subscr(q)) == TEX_EMPTY)
			freeNode(z, TEX_BOX_NODE_SIZE);
		else
		{
			shift_down = big_op_spacing4 - height(z);
			if (shift_down < big_op_spacing2)
				shift_down = big_op_spacing2;
				
			p = newKern(shift_down); 
			link(y) = p; 
			link(p) = z;
  			p = newKern(big_op_spacing5); 
  			link(z) = p;
  			depth(v) = depth(v) + big_op_spacing5 + height(z) + depth(z) + shift_down;
		}
		
		new_hlist(q) = v;
	}
	
	return delta;
}

void XWTeX::makeOrd(qint32 q)
{
	qint32 a, p, r;
	
restart:
	if (math_type(subscr(q)) == TEX_EMPTY)
	{
		if (math_type(supscr(q)) == TEX_EMPTY)
		{
			if (math_type(nucleus(q)) == TEX_MATH_CHAR)
			{
				p = link(q);
				if (p != TEX_NULL)
				{
					if ((type(p) >= (quint16)TEX_ORD_NOAD) && (type(p) <= (quint16)TEX_PUNCT_NOAD))
					{
						if (math_type(nucleus(p)) == TEX_MATH_CHAR)
						{
							if (fam(nucleus(p)) == fam(nucleus(q)))
							{
								math_type(nucleus(q)) = TEX_MATH_TEXT_CHAR;
								fetch(nucleus(q));
								if (char_tag(cur_i) == (quint16)TEX_LIG_TAG)
								{
									a = lig_kern_start(cur_f, cur_i);
        							cur_c = character(nucleus(p));
        							cur_i = font_info(cur_f, a).qqqq;
        							if (skip_byte(cur_i) > TEX_STOP_FLAG)
        							{
        								a = lig_kern_restart(cur_f, cur_i);
          								cur_i = font_info(cur_f, a).qqqq;
        							}
        							
        							while (true)
        							{
        								if ((next_char(cur_f, cur_i)) == cur_c)
        								{
        									if (skip_byte(cur_i) <= TEX_STOP_FLAG)
        									{
        										if (op_byte(cur_i) >= (quint16)TEX_KERN_FLAG)
        										{
        											p = newKern(char_kern(cur_f, cur_i));
    												link(p) = link(q); 
    												link(q) = p; 
    												return;
        										}
        										else
        										{
        											checkInterrupt();
        											switch (op_byte(cur_i))
        											{
        												case 1:
        												case 5:
        													character(nucleus(q)) = (quint16)rem_byte(cur_i);
        													break;
        													
        												case 2:
        												case 6:
        													character(nucleus(p)) = (quint16)rem_byte(cur_i);
        													break;
        													
        												case 3:
        												case 7:
        												case 11:
        													r = newNoad();
        													character(nucleus(r)) = (quint16)rem_byte(cur_i);
      														fam(nucleus(r)) = fam(nucleus(q));
      														link(q) = r; 
      														link(r) = p;
      														if (op_byte(cur_i) < (quint16)11)
      															math_type(nucleus(r)) = TEX_MATH_CHAR;
      														else
      															math_type(nucleus(r)) = TEX_MATH_TEXT_CHAR;
        													break;
        													
        												default:
        													link(q) = link(p);
      														character(nucleus(q)) = (quint16)rem_byte(cur_i);
      														mem[subscr(q)] = mem[subscr(p)]; 
      														mem[supscr(q)] = mem[supscr(p)];
      														freeNode(p, TEX_NOAD_SIZE);
        													break;
        											}
        											
        											if (op_byte(cur_i) > (quint16)3)
        												return;
        												
        											math_type(nucleus(q)) = TEX_MATH_CHAR; 
        											goto restart;
        										}
        									}
        								}
        								
        								if (skip_byte(cur_i) >= TEX_STOP_FLAG)
        									return;
        									
        								a = a + (skip_byte(cur_i)) + 1;
          								cur_i = font_info(cur_f, a).qqqq;
        							}
								}
							}
						}
					}
				}
			}
		}
	}
}

void XWTeX::makeOver(qint32 q)
{
	info(nucleus(q)) = overbar(cleanBox(nucleus(q), cramped_style(cur_style)), 
	                           3 * default_rule_thickness, default_rule_thickness);
	math_type(nucleus(q)) = TEX_SUB_BOX;
}

void XWTeX::makeRadical(qint32 q)
{
	qint32 x = cleanBox(nucleus(q), cramped_style(cur_style));
	qint32 clr;
	if (cur_style < TEX_TEXT_STYLE)
		clr = default_rule_thickness + (qAbs(math_x_height(cur_size)) / 4);
	else
	{
		clr = default_rule_thickness; 
		clr = clr + (qAbs(clr) / 4);
	}
	
	qint32 y = varDelimiter(left_delimiter(q), cur_size, height(x) + depth(x) + clr + default_rule_thickness);
	qint32 delta = depth(y) -(height(x) + depth(x) + clr);
	if (delta > 0)
		clr = clr + half(delta);
		
	shift_amount(y) = -(height(x) + clr);
	link(y) = overbar(x, clr, height(y));
	info(nucleus(q)) = hpack(y, 0, TEX_ADDITIONAL); 
	math_type(nucleus(q)) = TEX_SUB_BOX;
}

void XWTeX::makeScripts(qint32 q, qint32 delta)
{
	qint32 p, x, y, z, shift_up, shift_down, clr, t;
	
	p = new_hlist(q);
	if (isCharNode(p))
	{
		shift_up   = 0; 
		shift_down = 0;
	}
	else
	{
		z = hpack(p, 0, TEX_ADDITIONAL);
  		t = cur_size;
  		if (cur_style < TEX_SCRIPT_STYLE)
  			cur_size = TEX_SCRIPT_SIZE;
  		else
  			cur_size = TEX_SCRIPT_SCRIPT_SIZE;
  			
  		shift_up = height(z) - sup_drop(cur_size);
  		shift_down = depth(z) + sub_drop(cur_size);
  		cur_size = t;
  		freeNode(z, TEX_BOX_NODE_SIZE);
	}
	
	if (math_type(supscr(q)) == TEX_EMPTY)
	{
		x = cleanBox(subscr(q), sub_style(cur_style));
		width(x) = width(x) + scriptSpace();
		if (shift_down < sub1(cur_size))
			shift_down = sub1(cur_size);
		clr = height(x) - (qAbs(math_x_height(cur_size) * 4) / 5);
		if (shift_down < clr)
			shift_down = clr;
		shift_amount(x) = shift_down;
	}
	else
	{
		x = cleanBox(supscr(q),sup_style(cur_style));
		width(x) = width(x) + scriptSpace();
		if (odd(cur_style))
			clr = sup3(cur_size);
		else if (cur_style < TEX_TEXT_STYLE)
			clr = sup1(cur_size);
		else
			clr = sup2(cur_size);
			
		if (shift_up < clr)
			shift_up = clr;
		clr = depth(x) + (qAbs(math_x_height(cur_size)) / 4);
		if (shift_up < clr)
			shift_up = clr;
			
		if (math_type(subscr(q)) == TEX_EMPTY)
			shift_amount(x) = -shift_up;
		else
		{
			y = cleanBox(subscr(q), sub_style(cur_style));
			width(y) = width(y) + scriptSpace();
			if (shift_down < sub2(cur_size))
				shift_down = sub2(cur_size);
				
			clr = 4 * default_rule_thickness - ((shift_up - depth(x)) - (height(y) - shift_down));
			if (clr > 0)
			{
				shift_down = shift_down + clr;
  				clr = (qAbs(math_x_height(cur_size) * 4) / 5) - (shift_up - depth(x));
  				if (clr > 0)
  				{
  					shift_up = shift_up + clr;
    				shift_down = shift_down - clr;
  				}
			}
			
			shift_amount(x) = delta;
			p = newKern((shift_up - depth(x)) - (height(y) - shift_down)); 
			link(x) = p; 
			link(p) = y;
			pack_direction = mathDirection();
			x = vpack(x, 0, TEX_ADDITIONAL); 
			shift_amount(x) = shift_down;
		}
	}
	
	if (new_hlist(q) == TEX_NULL)
		new_hlist(q) = x;
	else
	{
		p = new_hlist(q);
		while (link(p) != TEX_NULL)
			p = link(p);
			
		link(p) = x;
	}
}

void XWTeX::makeUnder(qint32 q)
{
	qint32 x = cleanBox(nucleus(q), cur_style);
	qint32 p = newKern(3 * default_rule_thickness); 
	link(x) = p;
	link(p) = fractionRule(default_rule_thickness);
	pack_direction = mathDirection(); 
	qint32 y = vpack(x, 0, TEX_ADDITIONAL);
	qint32 delta = height(y) + depth(y) + default_rule_thickness;
	height(y) = height(x); 
	depth(y) = delta - height(y);
	info(nucleus(q)) = y; 
	math_type(nucleus(q)) = TEX_SUB_BOX;
}

void XWTeX::makeVCenter(qint32 q)
{
	qint32 v = info(nucleus(q));
	if (type(v) != (quint16)TEX_VLIST_NODE)
		confusion("vcenter");
		
	qint32 delta = height(v) + depth(v);
	height(v) = axis_height(cur_size) + half(delta);
	depth(v) = delta - height(v);
}

void XWTeX::mathAC()
{
	if (cur_cmd == TEX_ACCENT)
	{
		printErr(tr("Please use ")); 
		printEsc(TeXMathAccent);
		print(tr(" for accents in math mode"));
		help2(tr("I'm changing \\accent to \\mathaccent here; wish me luck."));
		help_line[0] = tr("(Accents are not the same in formulas as they are in text.)");
		error();
	}
	
	tailAppend(getNode(TEX_ACCENT_NOAD_SIZE));
	type(tail) = (quint16)TEX_ACCENT_NOAD; 
	subtype(tail) = (quint16)TEX_NORMAL;
	mem[nucleus(tail)].hh = empty_field;
	mem[subscr(tail)].hh = empty_field;
	mem[supscr(tail)].hh = empty_field;
	math_type(accent_chr(tail)) = TEX_MATH_CHAR;
	if (cur_chr == 0)
		scanFifteenBitInt();
	else
		scanBigFifteenBitInt();
		
	character(accent_chr(tail)) = (quint16)(cur_val % 0x10000);
	if ((cur_val >= TEX_VAR_CODE) && ((curFam() >= 0) && (curFam() < 16)))
		fam(accent_chr(tail)) = (quint16)curFam();
	else
		fam(accent_chr(tail)) = (quint16)((cur_val / 0x10000) % 0x100);
		
	scanMath(nucleus(tail));
}

void XWTeX::mathFraction()
{
	qint32 c = cur_chr;
	if (incompleat_noad != TEX_NULL)
	{
		if (c >= TEX_DELIMITED_CODE)
		{
			scanDelimiter(garbage, false); 
			scanDelimiter(garbage, false);
		}
		
		if ((c % TEX_DELIMITED_CODE) == TEX_ABOVE_CODE)
			scanNormalDimen();
		
		printErr(tr("Ambiguous; you need another { and }"));
		help3(tr("I'm ignoring this fraction specification, since I don't"));
		help_line[1] = tr("know whether a construction like `x \\over y \\over z'");
		help_line[0] = tr("means `{x \\over y} \\over z' or `x \\over {y \\over z}'.");
		error();
	}
	else
	{
		incompleat_noad = getNode(TEX_FRACTION_NOAD_SIZE);
  		type(incompleat_noad) = (quint16)TEX_FRACTION_NOAD;
  		subtype(incompleat_noad) = (quint16)TEX_NORMAL;
  		math_type(numerator(incompleat_noad)) = TEX_SUB_MLIST;
  		info(numerator(incompleat_noad)) = link(head);
  		mem[denominator(incompleat_noad)].hh = empty_field;
  		mem[left_delimiter(incompleat_noad)].qqqq = null_delimiter;
  		mem[right_delimiter(incompleat_noad)].qqqq = null_delimiter;
  		link(head) = TEX_NULL; 
  		tail = head;
  		if (c >= TEX_DELIMITED_CODE)
  		{
  			scanDelimiter(left_delimiter(incompleat_noad), false);
  			scanDelimiter(right_delimiter(incompleat_noad), false);
  		}
  		
  		switch (c % TEX_DELIMITED_CODE)
  		{
  			case TEX_ABOVE_CODE:
  				scanNormalDimen();
  				thickness(incompleat_noad) = cur_val;
  				break;
  				
  			case TEX_OVER_CODE: 
  				thickness(incompleat_noad) = TEX_DEFAULT_CODE;
  				break;
  				
			case TEX_ATOP_CODE: 
				thickness(incompleat_noad) = 0;
				break;
				
			default:
				break;
  		}
	}
}

qint32 XWTeX::mathGlue(qint32 g, qint32 m)
{
	qint32 n = xOverN(m, 0x10000); 
	qint32 f = remainder;
	if (f < 0)
	{
		n--;
		f = f + 0x10000;
	}
	
	qint32 p = getNode(TEX_GLUE_SPEC_SIZE);
	width(p) = nxPlusY(n, width(g), xnOverD(width(g), f, 0x10000));
	stretch_order(p) = stretch_order(g);
	if (stretch_order(p) == (quint16)TEX_NORMAL)
		stretch(p) = nxPlusY(n, stretch(g), xnOverD(stretch(g), f, 0x10000));
	else
		stretch(p) = stretch(g);
		
	shrink_order(p) = shrink_order(g);
	if (shrink_order(p) == (quint16)TEX_NORMAL)
		shrink(p) = nxPlusY(n, shrink(g), xnOverD(shrink(g), f, 0x10000));
	else
		shrink(p) = shrink(g);
		
	return p;
}

void XWTeX::mathKern(qint32 p, qint32 m)
{
	qint32 n, f;
	if (subtype(p) == (quint16)TEX_MU_GLUE)
	{
		n = xOverN(m, 0x10000); 
		f = remainder;
		if (f < 0)
		{
			n--;
			f = f + 0x10000;
		}
		
		width(p) = nxPlusY(n, width(p), xnOverD(width(p), f, 0x10000));
		subtype(p) = (quint16)TEX_EXPLICIT;
	}
}

void XWTeX::mathLeftRight()
{
	qint32 t, p, q;
	
	t = cur_chr;
	if ((t != TEX_LEFT_NOAD) && (cur_group != TEX_MATH_LEFT_GROUP))
	{
		if (cur_group == TEX_MATH_SHIFT_GROUP)
		{
			scanDelimiter(garbage, false);
  			printErr(tr("Extra "));
  			if (t == TEX_MIDDLE_NOAD)
  			{
  				printEsc(TeXMiddle);
  				help1(tr("I'm ignoring a \\middle that had no matching \\left."));
  			}
  			else
  			{
  				printEsc(TeXRight);
  				help1(tr("I'm ignoring a \\right that had no matching \\left."));
  			}
  			
  			error();
		}
		else
			offSave();
	}
	else
	{
		p = newNoad(); 
		type(p) = (quint16)t;
  		scanDelimiter(delimiter(p), false);
  		if (t == TEX_MIDDLE_NOAD)
  		{
  			type(p) = (quint16)TEX_RIGHT_NOAD; 
  			subtype(p) = (quint16)TEX_MIDDLE_NOAD;
  		}
  		
  		if (t == TEX_LEFT_NOAD)
  			q = p;
  		else
  		{
  			q = finMList(p);
  			unsave();
			save_ptr--;
			flushNodeList(text_dir_ptr);
			text_dir_ptr = saved(0);
  		}
  		
  		if (t != TEX_RIGHT_NOAD)
  		{
  			pushMath(TEX_MATH_LEFT_GROUP); 
  			link(head) = q; 
  			tail = p;
    		delim_ptr = p;
  		}
  		else
  		{
  			tailAppend(newNoad()); 
  			type(tail) = (quint16)TEX_INNER_NOAD;
    		math_type(nucleus(tail)) = TEX_SUB_MLIST;
    		info(nucleus(tail)) = q;
  		}
	}
}

void XWTeX::mathLimitSwitch()
{
	if (head != tail)
	{
		if (type(tail) == (quint16)TEX_OP_NOAD)
		{
			subtype(tail) = (quint16)cur_chr; 
			return;
		}
	}
	
	printErr(tr("Limit controls must follow a math operator"));
	help1(tr("I'm ignoring this misplaced \\limits or \\nolimits command.")); 
	error();
}

void XWTeX::mathRadical()
{
	tailAppend(getNode(TEX_RADICAL_NOAD_SIZE));
	type(tail) = (quint16)TEX_RADICAL_NOAD; 
	subtype(tail) = (quint16)TEX_NORMAL;
	mem[nucleus(tail)].hh = empty_field;
	mem[subscr(tail)].hh = empty_field;
	mem[supscr(tail)].hh = empty_field;
	scanDelimiter(left_delimiter(tail), cur_chr + 1); 
	scanMath(nucleus(tail));
}

void XWTeX::mlistToHList()
{
	qint32 mlist, style, save_style, q, r, r_type, t, p, z, x, y, pen, s, max_h, max_d, delta;
	bool   penalties;
	
	mlist = cur_mlist; 
	penalties = mlist_penalties;
	style = cur_style;
	q = mlist; 
	r = TEX_NULL; 
	r_type = TEX_OP_NOAD; 
	max_h = 0; 
	max_d = 0;
	
	if (cur_style < TEX_SCRIPT_STYLE)
		cur_size = TEX_TEXT_SIZE;
	else
		cur_size = TEX_SCRIPT_SIZE * ((cur_style - TEX_TEXT_STYLE) / 2);
		
	cur_mu = xOverN(math_quad(cur_size), 18);
	
	while (q != TEX_NULL)
	{
		
reswitch: 
		delta = 0;
		
		switch (type(q))
		{
			case TEX_BIN_NOAD:
				{
					switch (r_type)
					{
						case TEX_BIN_NOAD:
						case TEX_OP_NOAD:
						case TEX_REL_NOAD:
						case TEX_OPEN_NOAD:
						case TEX_PUNCT_NOAD:
						case TEX_LEFT_NOAD:
							type(q) = (quint16)TEX_ORD_NOAD; 
							goto reswitch;
							break;
							
						default:
							break;
					}
				}
				break;
				
			case TEX_REL_NOAD:
			case TEX_CLOSE_NOAD:
			case TEX_PUNCT_NOAD:
			case TEX_RIGHT_NOAD:
				if (r_type == TEX_BIN_NOAD)
					type(r) = (quint16)TEX_ORD_NOAD;
				if (type(q) == (quint16)TEX_RIGHT_NOAD)
					goto done_with_noad;
				break;
				
			case TEX_LEFT_NOAD: 
				goto done_with_noad;
				break;
				
			case TEX_FRACTION_NOAD:
				makeFraction(q); 
				goto check_dimensions;
				break;
				
			case TEX_OP_NOAD:
				delta = makeOp(q);
				if (subtype(q) == (quint16)TEX_LIMITS)
					goto check_dimensions;
				break;
				
			case TEX_ORD_NOAD: 
				makeOrd(q);
				break;
				
			case TEX_OPEN_NOAD:
			case TEX_INNER_NOAD: 
				break;
				
			case TEX_RADICAL_NOAD: 
				makeRadical(q);
				break;
				
			case TEX_OVER_NOAD: 
				makeOver(q);
				break;
				
			case TEX_UNDER_NOAD: 
				makeUnder(q);
				break;
				
			case TEX_ACCENT_NOAD: 
				makeMathAccent(q);
				break;
				
			case TEX_VCENTER_NOAD: 
				makeVCenter(q);
				break;
				
			case TEX_STYLE_NODE:
				cur_style = subtype(q);
				if (cur_style < TEX_SCRIPT_STYLE)
					cur_size = TEX_TEXT_SIZE;
				else
					cur_size = TEX_SCRIPT_SIZE * ((cur_style - TEX_TEXT_STYLE) / 2);		
				cur_mu = xOverN(math_quad(cur_size), 18);
				goto done_with_node;
				break;
				
			case TEX_CHOICE_NODE:
				{
					switch (cur_style / 2)
					{
						case 0:
							p = display_mlist(q); 
							display_mlist(q) = TEX_NULL;
							break;
							
						case 1:
							p = text_mlist(q); 
							text_mlist(q) = TEX_NULL;
							break;
							
						case 2:
							p = script_mlist(q); 
							script_mlist(q) = TEX_NULL;
							break;
							
						case 3:
							p = script_script_mlist(q); 
							script_script_mlist(q) = TEX_NULL;
							break;
					}
					
					flushNodeList(display_mlist(q));
					flushNodeList(text_mlist(q));
					flushNodeList(script_mlist(q));
					flushNodeList(script_script_mlist(q));
					type(q) = (quint16)TEX_STYLE_NODE; 
					subtype(q) = (quint16)cur_style; 
					width(q) = 0; 
					depth(q) = 0;
					if (p != TEX_NULL)
					{
						z = link(q); 
						link(q) = p;
						while (link(p) != TEX_NULL)
							p = link(p);
							
						link(p) = z;
					}
					goto done_with_node;
				}
				break;
				
			case TEX_INS_NODE:
			case TEX_MARK_NODE:
			case TEX_ADJUST_NODE:
  			case TEX_WHATSIT_NODE:
  			case TEX_PENALTY_NODE:
  			case TEX_DISC_NODE: 
  				goto done_with_node;
  				break;
  				
  			case TEX_RULE_NODE:
  				if (height(q) > max_h)
  					max_h = height(q);  					
  				if (depth(q) > max_d)
  					max_d = depth(q);
  				goto done_with_node;
  				break;
  				
  			case TEX_GLUE_NODE:
  				if (subtype(q) == (quint16)TEX_MU_GLUE)
  				{
  					x = glue_ptr(q);
  					y = mathGlue(x, cur_mu); 
  					deleteGlueRef(x); 
  					glue_ptr(q) = y;
  					subtype(q) = (quint16)TEX_NORMAL;
  				}
  				else if ((cur_size != TEX_TEXT_SIZE) && 
  						(subtype(q) == (quint16)TEX_COND_MATH_GLUE))
  				{
  					p = link(q);
  					if (p != TEX_NULL)
  					{
  						if ((type(p) == (quint16)TEX_GLUE_NODE) || 
  							(type(p) == (quint16)TEX_KERN_NODE))
  						{
  							link(q) = link(p); 
  							link(p) = TEX_NULL; 
  							flushNodeList(p);
  						}
  					}
  				}
  				goto done_with_node;
  				break;
  				
  			case TEX_KERN_NODE:
  				mathKern(q, cur_mu); 
  				goto done_with_node;
  				break;
  				
  			default:
  				confusion("mlist1");
  				break;
		}
		
		switch (math_type(nucleus(q)))
		{
			case TEX_MATH_CHAR:
			case TEX_MATH_TEXT_CHAR:
				fetch(nucleus(q));
				if (char_exists(cur_i))
				{
					delta = char_italic(cur_f, cur_i); 
					p = newCharacter(cur_f, cur_c);
					if ((math_type(nucleus(q)) == TEX_MATH_TEXT_CHAR) && (space(cur_f) != 0))
						delta = 0;
					if ((math_type(subscr(q)) == TEX_EMPTY) && (delta != 0))
					{
						link(p) = newKern(delta); 
						delta = 0;
					}
				}
				else
					p = TEX_NULL;
				break;
				
			case TEX_EMPTY: 
				p = TEX_NULL;
				break;
				
			case TEX_SUB_BOX: 
				p = info(nucleus(q));
				break;
				
			case TEX_SUB_MLIST:
				cur_mlist = info(nucleus(q)); 
				save_style = cur_style;
  				mlist_penalties = false; 
  				mlistToHList();
  				cur_style = save_style;
  				if (cur_style < TEX_SCRIPT_STYLE)
					cur_size = TEX_TEXT_SIZE;
				else
					cur_size = TEX_SCRIPT_SIZE * ((cur_style - TEX_TEXT_STYLE) / 2);		
				cur_mu = xOverN(math_quad(cur_size), 18);
				p = hpack(link(temp_head), 0, TEX_ADDITIONAL);
				break;
				
			default:
				confusion("mlist2");
				break;
		}
		
		new_hlist(q) = p;
		if ((math_type(subscr(q)) == TEX_EMPTY) && (math_type(supscr(q)) == TEX_EMPTY))
			goto check_dimensions;
			
		makeScripts(q, delta);
		
check_dimensions:
		z = hpack(new_hlist(q), 0, TEX_ADDITIONAL);
		if (height(z) > max_h)
			max_h = height(z);
		if (depth(z) > max_d)
			max_d = depth(z);
			
		freeNode(z, TEX_BOX_NODE_SIZE);
		
done_with_noad:
		r = q; 
		r_type = type(r);
		if (r_type == TEX_RIGHT_NOAD)
		{
			r_type = TEX_LEFT_NOAD; 
			cur_style = style;
			if (cur_style < TEX_SCRIPT_STYLE)
				cur_size = TEX_TEXT_SIZE;
			else
				cur_size = TEX_SCRIPT_SIZE * ((cur_style - TEX_TEXT_STYLE) / 2);		
			cur_mu = xOverN(math_quad(cur_size), 18);
		}
		
done_with_node: 
		q = link(q);
	}
	
	if (r_type == TEX_BIN_NOAD)
		type(r) = (quint16)TEX_ORD_NOAD;
		
	p = temp_head; 
	link(p) = TEX_NULL; 
	q = mlist; 
	r_type = 0; 
	cur_style = style;
	if (cur_style < TEX_SCRIPT_STYLE)
		cur_size = TEX_TEXT_SIZE;
	else
		cur_size = TEX_SCRIPT_SIZE * ((cur_style - TEX_TEXT_STYLE) / 2);		
	cur_mu = xOverN(math_quad(cur_size), 18);
	
	while (q != TEX_NULL)
	{
		t = TEX_ORD_NOAD; 
		s = TEX_NOAD_SIZE; 
		pen = TEX_INF_PENALTY;
		switch (type(q))
		{
			case TEX_OP_NOAD:
			case TEX_OPEN_NOAD:
			case TEX_CLOSE_NOAD:
			case TEX_PUNCT_NOAD:
			case TEX_INNER_NOAD: 
				t = type(q);
				break;
				
			case TEX_BIN_NOAD:
				t = TEX_BIN_NOAD; 
				pen = binOpPenalty();
				break;
				
			case TEX_REL_NOAD:
				t = TEX_REL_NOAD; 
				pen = relPenalty();
				break;
				
			case TEX_ORD_NOAD:
			case TEX_VCENTER_NOAD:
			case TEX_OVER_NOAD:
			case TEX_UNDER_NOAD: 
				break;
				
			case TEX_RADICAL_NOAD: 
				s = TEX_RADICAL_NOAD_SIZE;
				break;
				
			case TEX_ACCENT_NOAD: 
				s = TEX_ACCENT_NOAD_SIZE;
				break;
				
			case TEX_FRACTION_NOAD:
				t = TEX_INNER_NOAD; 
				s = TEX_FRACTION_NOAD_SIZE;
				break;
				
			case TEX_LEFT_NOAD:
			case TEX_RIGHT_NOAD: 
				t = makeLeftRight(q, style, max_d, max_h);
				break;
				
			case TEX_STYLE_NODE:
				cur_style = subtype(q); 
				s = TEX_STYLE_NODE_SIZE;
				if (cur_style < TEX_SCRIPT_STYLE)
					cur_size = TEX_TEXT_SIZE;
				else
					cur_size = TEX_SCRIPT_SIZE * ((cur_style - TEX_TEXT_STYLE) / 2);		
				cur_mu = xOverN(math_quad(cur_size), 18);
				goto delete_q;
				break;
				
			case TEX_WHATSIT_NODE:
			case TEX_PENALTY_NODE:
			case TEX_RULE_NODE:
			case TEX_DISC_NODE:
			case TEX_ADJUST_NODE:
			case TEX_INS_NODE:
			case TEX_MARK_NODE:
 			case TEX_GLUE_NODE:
 			case TEX_KERN_NODE:
 				link(p) = q; 
 				p = q; 
 				q = link(q); 
 				link(p) = TEX_NULL; 
 				goto done;
 				break;
 				
 			default:
 				confusion("mlist3");
 				break;
		}
		
		if (r_type > 0)
		{
			switch ((char)(str_pool[r_type * 8 + t + magic_offset]))
			{
				case '0': 
					x = 0;
					break;
					
				case '1':
					if (cur_style < TEX_SCRIPT_STYLE)
						x = TEX_THIN_MU_SKIP_CODE;
					else
						x = 0;
					break;
					
				case '2': 
					x = TEX_THIN_MU_SKIP_CODE;
					break;
					
				case '3':
					if (cur_style < TEX_SCRIPT_STYLE)
						x = TEX_MED_MU_SKIP_CODE;
					else
						x = 0;
					break;
					
				case '4':
					if (cur_style < TEX_SCRIPT_STYLE)
						x = TEX_THICK_MU_SKIP_CODE;
					else
						x = 0;
					break;
					
				default:
					confusion("mlist4");
					break;
			}
			
			if (x != 0)
			{
				y = mathGlue(gluePar(x), cur_mu);
    			z = newGlue(y); 
    			glue_ref_count(y) = TEX_NULL; 
    			link(p) = z; 
    			p = z;
    			subtype(z) = (quint16)(x + 1);
			}
		}
		
		if (new_hlist(q) != TEX_NULL)
		{
			link(p) = new_hlist(q);
			do
			{
				p = link(p);
			} while (link(p) != TEX_NULL);
		}
		
		if (penalties)
		{
			if (link(q) != TEX_NULL)
			{
				if (pen < TEX_INF_PENALTY)
				{
					r_type = type(link(q));
					if (r_type != TEX_PENALTY_NODE)
					{
						if (r_type != TEX_REL_NOAD)
						{
							z = newPenalty(pen); 
							link(p) = z; 
							p = z;
						}
					}
				}
			}
		}
		
		if (type(q) == (quint16)TEX_RIGHT_NOAD)
			t = TEX_OPEN_NOAD;
		r_type = t;
		
delete_q: 
	    r = q; 
	    q = link(q); 
	    freeNode(r, s);
	    
done:
		;
	}
}

qint32 XWTeX::newCharacter(qint32 f, qint32 c)
{
	if (font_bc(f) <= c)
	{
		if (font_ec(f) >= c)
		{
			if (char_exists(char_info(f, c)))
			{
				qint32 p = getAvail(); 
				font(p) = (quint16)f; 
				character(p) = (quint16)c;
				return p;
			}
		}
	}
	
	charWarning(f, c);
	return TEX_NULL;
}

qint32 XWTeX::newChoice()
{
	qint32 p = getNode(TEX_STYLE_NODE_SIZE); 
	type(p) = (quint16)TEX_CHOICE_NODE;
	subtype(p) = (quint16)0;
	display_mlist(p) = TEX_NULL; 
	text_mlist(p) = TEX_NULL; 
	script_mlist(p) = TEX_NULL;
	script_script_mlist(p) = TEX_NULL;
	
	return p;
}

qint32 XWTeX::newDir(qint32 s)
{
	qint32 p = getNode(TEX_DIR_NODE_SIZE); 
	type(p) = (quint16)TEX_WHATSIT_NODE;
	subtype(p) = (quint16)TEX_DIR_NODE; 
	dir_dir(p) = s;
	dir_dvi_h(p) = 0; 
	dir_dvi_ptr(p) = -1;
	dir_level(p) = cur_level; 
	return p;
}

qint32 XWTeX::newDisc()
{
	qint32 p  = getNode(TEX_SMALL_NODE_SIZE); 
	type(p) = (quint16)TEX_DISC_NODE;
	replace_count(p) = (quint16)0; 
	pre_break(p)  = TEX_NULL; 
	post_break(p) = TEX_NULL;
	return p;
}

qint32 XWTeX::newGlue(qint32 q)
{
	qint32 p  = getNode(TEX_SMALL_NODE_SIZE); 
	type(p) = (quint16)TEX_GLUE_NODE; 
	subtype(p) = (quint16)TEX_NORMAL;
	leader_ptr(p) = TEX_NULL; 
	glue_ptr(p) = q; 
	(glue_ref_count(q))++;
	return p;
}

void XWTeX::newGraf(bool indented)
{
	qint32 p,q,dir_graf_tmp;
	
	prev_graf = 0;
	if ((mode == TEX_VMODE) || (head != tail))
		tailAppend(newParamGlue(TEX_PAR_SKIP_CODE));
		
	pushNest(); 
	mode = TEX_HMODE; 
	space_factor = 1000; 
	if (language() <= 0)
		cur_lang = 0;
	else if (language() > TEX_BIGGEST_LANG)
		cur_lang = 0;
	else
		cur_lang = language();
	clang = cur_lang;
	prev_graf = (normMin(leftHyphenMin()) * 0x40 + normMin(rightHyphenMin())) * 0x10000 + cur_lang;
	tailAppend(makeLocalParNode());
	if (indented)
	{
		p = newNullBox(); 
		box_dir(p) = parDirection();
  		width(p) = parIndent();
  		tailAppend(p);
  	if (insert_src_special_every_par)
  		insertSrcSpecial();
	}
	
	q = tail;
	dir_rover = text_dir_ptr;
	while (dir_rover != TEX_NULL)
	{
		if ((link(dir_rover) != TEX_NULL) || 
			(dir_dir(dir_rover) != parDirection()))
		{
			dir_graf_tmp = newDir(dir_dir(dir_rover));
    		link(dir_graf_tmp) = link(q); 
    		link(q) = dir_graf_tmp;
		}
		
		dir_rover = link(dir_rover);
	}
	
	q = head;
	
	while (link(q) != TEX_NULL)
		q = link(q);
		
	tail = q;
	if (everyPar() != TEX_NULL)
		beginTokenList(everyPar(), TEX_EVERY_PAR_TEXT);
		
	if (nest_ptr == 1)
		buildPage();
}

void XWTeX::newIndex(qint32 i, qint32 q)
{
	cur_ptr = getNode(TEX_INDEX_NODE_SIZE); 
	sa_index(cur_ptr) = (quint16)i;
	sa_used(cur_ptr) = (quint16)0; 
	link(cur_ptr) = q;
	for (qint32 k = 1; k < TEX_INDEX_NODE_SIZE; k++)
		mem[cur_ptr + k] = sa_null;
}

qint32 XWTeX::newKern(qint32 w)
{
	qint32 p = getNode(TEX_SMALL_NODE_SIZE); 
	type(p) = (quint16)TEX_KERN_NODE;
	subtype(p) = (quint16)TEX_NORMAL;
	width(p) = w;
	return p;
}

qint32 XWTeX::newLigature(qint32 f, qint32 c, qint32 q)
{
	qint32 p = getNode(TEX_SMALL_NODE_SIZE); 
	type(p) = (quint16)TEX_LIGATURE_NODE;
	font(lig_char(p)) = (quint16)f; 
	character(lig_char(p)) = (quint16)c; 
	lig_ptr(p) = q;
	subtype(p) = (quint16)0;
	return p;
}

qint32 XWTeX::newLigItem(qint32 c)
{
	qint32 p = getNode(TEX_SMALL_NODE_SIZE); 
	character(p) = (quint16)c; 
	lig_ptr(p) = TEX_NULL;
	return p;
}

qint32 XWTeX::newMath(qint32 w, qint32 s)
{
	qint32 p = getNode(TEX_SMALL_NODE_SIZE); 
	type(p) = (quint16)TEX_MATH_NODE;
	subtype(p) = (quint16)s; 
	width(p) = w;
	return p;
}

qint32 XWTeX::newNoad()
{
	qint32 p = getNode(TEX_NOAD_SIZE);
	type(p) = (quint16)TEX_ORD_NOAD; 
	subtype(p) = (quint16)TEX_NORMAL;
	mem[nucleus(p)].hh = empty_field;
	mem[subscr(p)].hh  = empty_field;
	mem[supscr(p)].hh  = empty_field;
	return p;
}

qint32 XWTeX::newNullBox()
{
	qint32 p  = getNode(TEX_BOX_NODE_SIZE); 
	type(p) = (quint16)TEX_HLIST_NODE;
	subtype(p) = (quint16)TEX_MIN_QUARTERWORD;
	width(p)  = 0; 
	depth(p)  = 0; 
	height(p) = 0; 
	shift_amount(p) = 0; 
	list_ptr(p)   = TEX_NULL;
	glue_sign(p)  = (quint16)TEX_NORMAL; 
	glue_order(p) = (quint16)TEX_NORMAL; 
	glue_set(p) = 0.0;
	box_dir(p) = textDirection();
	return p;
}

qint32 XWTeX::newParamGlue(qint32 n)
{
	qint32 p = getNode(TEX_SMALL_NODE_SIZE);
	type(p) = (quint16)TEX_GLUE_NODE; 
	subtype(p) = (quint16)(n + 1);
	leader_ptr(p) = TEX_NULL;
	qint32 q = gluePar(n);
	glue_ptr(p) = q; 
	(glue_ref_count(q))++;
	return p;
}

qint32 XWTeX::newPenalty(qint32 m)
{
	qint32 p = getNode(TEX_SMALL_NODE_SIZE);
	type(p) = (quint16)TEX_PENALTY_NODE;
	subtype(p) = (quint16)0;
	penalty(p) = m;
	return p;
}

qint32 XWTeX::newRule()
{
	qint32 p  = getNode(TEX_RULE_NODE_SIZE); 
	type(p) = (quint16)TEX_RULE_NODE;
	subtype(p) = (quint16)0;
	width(p)  = TEX_NULL_FLAG; 
	depth(p)  = TEX_NULL_FLAG; 
	height(p) = TEX_NULL_FLAG;
    rule_dir(p) = -1;
    return p;
}

void XWTeX::newSaveLevel(qint32 c)
{
	if (!checkFullSaveStack())
		return ;
		
	if (eTeX_ex)
	{
		saved(0) = line; 
		save_ptr++;
	}
	
	save_type(save_ptr) = (quint16)TEX_LEVEL_BOUNDARY; 
	save_level(save_ptr) = (quint16)cur_group;
	save_index(save_ptr) = cur_boundary;
	
	if (cur_level == TEX_MAX_QUARTERWORD)
	{
		overFlow(tr("grouping levels"), TEX_MAX_QUARTERWORD - TEX_MIN_QUARTERWORD);
		return ;
	}
	
	cur_boundary = save_ptr; 
	cur_group = c;
		
#ifdef XW_TEX_STAT
	if (tracingGroups() > 0)
		groupTrace(false);
#endif

	cur_level++;
	save_ptr++;
}

qint32 XWTeX::newSkipParam(qint32 n)
{
	temp_ptr = newSpec(gluePar(n));
	qint32 p = newGlue(temp_ptr); 
	glue_ref_count(temp_ptr) = TEX_NULL; 
	subtype(p) = (quint16)(n + 1);
	return p;
}

qint32 XWTeX::newSpec(qint32 p)
{
	qint32 q = getNode(TEX_GLUE_SPEC_SIZE);
	mem[q] = mem[p]; 
	glue_ref_count(q) = TEX_NULL;
	width(q) = width(p); 
	stretch(q) = stretch(p); 
	shrink(q) = shrink(p);
	return q;
}

qint32 XWTeX::newStyle(qint32 s)
{
	qint32 p = getNode(TEX_STYLE_NODE_SIZE); 
	type(p) = (quint16)TEX_STYLE_NODE;
	subtype(p) = (quint16)s; 
	width(p) = 0; 
	depth(p) = 0;
	return p;
}

void XWTeX::newWhatsIt(qint32 s, qint32 w)
{
	qint32 p = getNode(w); 
	type(p) = (quint16)TEX_WHATSIT_NODE; 
	subtype(p) = (quint16)s;
	link(tail) = p; 
	tail = p;
}

void XWTeX::newWriteWhatsIt(qint32 w)
{
	newWhatsIt(cur_chr, w);
	if (w != TEX_WRITE_NODE_SIZE)
		scanFourBitInt();
	else
	{
		scanInt();
		if (cur_val < 0)
			cur_val = 17;
		else if (cur_val > 15 && cur_val != 18)
			cur_val = 16;
	}
	
	write_stream(tail) = cur_val;
}

qint32 XWTeX::overbar(qint32 b, qint32 k, qint32 t)
{
	qint32 p = newKern(k); 
	link(p) = b; 
	qint32 q = fractionRule(t); 
	link(q) = p;
	p = newKern(t); 
	link(p) = q;
	pack_direction = mathDirection(); 
	return vpack(p, 0, TEX_ADDITIONAL);
}

void XWTeX::package(qint32 c)
{
	qint32 h, p, d;
	
	d = boxMaxDepth(); 
	unsave(); 
	save_ptr = save_ptr - 5;
	pack_direction = saved(3);
	if (mode == -TEX_HMODE)
		cur_box = hpack(link(head), saved(2), saved(1));
	else
	{
		cur_box = vpackage(link(head), saved(2), saved(1), d);
		if (c == TEX_VTOP_CODE)
		{
			h = 0; 
			p = list_ptr(cur_box);
			if (p != TEX_NULL)
			{
				if (type(p) <= (quint16)TEX_RULE_NODE)
					h = height(p);
			}
			
			depth(cur_box)  = depth(cur_box) - h + height(cur_box); 
			height(cur_box) = h;
		}
	}
	
	if (saved(4) != TEX_NULL)
	{
		flushNodeList(text_dir_ptr);
		text_dir_ptr = saved(4);
	}
	
	popNest(); 
	boxEnd(saved(0));
}

void XWTeX::popAlignment()
{
	freeAvail(cur_head);
	qint32 p = align_ptr;
	cur_tail = link(p + 4); 
	cur_head = info(p + 4);
	align_state = mem[p + 3].ii.CINT0; 
	cur_loop = mem[p + 2].ii.CINT0;
	cur_span = rlink(p); 
	preamble = llink(p);
	cur_align = info(p); 
	align_ptr = link(p);
	freeNode(p, TEX_ALIGN_STACK_NODE_SIZE);
}

void XWTeX::popNest()
{
	if (local_par != TEX_NULL)
	{
		if (!(local_par_bool))
			freeNode(local_par, TEX_LOCAL_PAR_SIZE);
	}
	
	freeAvail(head); 
	nest_ptr--; 
	cur_list = nest[nest_ptr];
}

void XWTeX::pseudoClose()
{
	qint32 p = link(pseudo_files); 
	qint32 q = info(pseudo_files);
	freeAvail(pseudo_files); 
	pseudo_files = p;
	while (q != TEX_NULL)
	{
		p = q; 
		q = link(p); 
		freeNode(p, info(p));
	}
}

bool XWTeX::pseudoInput()
{
	qint32 p, sz, r;
	TeXFourQuarters w;
	
	last = first;
	p = info(pseudo_files);
	if (p == TEX_NULL)
		return false;
		
	info(pseudo_files) = link(p); 
	sz = info(p);
	if ((4 * sz - 3) >= (buf_size - last))
	{
		bufferOverFlow();
		return false;
	}
	
	last = first;
	for (r = (p + 1); r < (p + sz); r++)
	{
		w = mem[r].qqqq;
    	buffer[last]     = w.u.B0; 
		buffer[last + 1] = w.u.B1;
    	buffer[last + 2] = w.u.B2; 
    	buffer[last + 3] = w.u.B3;
    	last = last + 4;
	}
	
	if (last >= max_buf_stack)
		max_buf_stack = last + 1;
		
	while ((last > first) && (buffer[last-1] == ' '))
		last--;
		
	freeNode(p, sz);
	return true;
}

void XWTeX::pseudoStart()
{
	qint32 s, l, m, p, q, r, nl, sz;
	TeXFourQuarters w;
	
	scanGeneralText();
	old_setting= selector; 
	selector = TEX_NEW_STRING;
	tokenShow(temp_head); 
	selector = old_setting;
	flushList(link(temp_head));
	strRoom(1); 
	s = makeString();
	str_pool[pool_ptr] = ' '; 
	l = str_start(s);
	nl = newLineChar();
	p = getAvail(); 
	q = p;
	while (l < pool_ptr)
	{
		m = l;
		while ((l < pool_ptr) && (str_pool[l] != nl))
			l++;
			
		sz = (l - m + 7) / 4;
		if (sz == 1)
			sz = 2;
		r = getNode(sz); 
		link(q) = r; 
		q = r; 
		info(q) = sz;
		while (sz > 2)
		{
			sz--; 
			r++;
    		w.u.B0 = (str_pool[m]); 
    		w.u.B1 = (str_pool[m+1]);
    		w.u.B2 = (str_pool[m + 2]); 
    		w.u.B3 = (str_pool[m + 3]);
    		mem[r].qqqq = w; 
    		m = m + 4;
		}
		
		w.u.B0 = ' '; 
		w.u.B1 = ' '; 
		w.u.B2 = ' '; 
		w.u.B3 = ' ';
		if (l > m)
		{
			w.u.B0 = (str_pool[m]);
			if (l > (m + 1))
			{
				w.u.B1 = (str_pool[m + 1]);
				if (l > (m + 2))
				{
					w.u.B2 = (str_pool[m + 2]);
					if (l > (m + 3))
						w.u.B3 = (str_pool[m + 3]);
				}
			}
		}
		
		mem[r + 1].qqqq = w;
		if (str_pool[l] == nl)
			l++;
	}
	
	info(p) = link(p); 
	link(p) = pseudo_files; 
	pseudo_files = p;
	flushString();
	beginFileReading();
	line  = 0; 
	limit = texstart; 
	loc   = limit + 1;
	if (tracingScanTokens() > 0)
	{
		if (term_offset > (max_print_line - 3))
			println();
		else if ((term_offset > 0) || (file_offset > 0))
			printChar(' ');
			
		texname = 19; 
		print("( "); 
		open_parens++;
	}
	else
		texname = 18;
}

void XWTeX::pushAlignment()
{
	qint32 p = getNode(TEX_ALIGN_STACK_NODE_SIZE);
	link(p)  = align_ptr; 
	info(p)  = cur_align;
	llink(p) = preamble; 
	rlink(p) = cur_span;
	mem[p + 2].ii.CINT0 = cur_loop; 
	mem[p + 3].ii.CINT0 = align_state;
	info(p + 4) = cur_head; 
	link(p + 4) = cur_tail;
	align_ptr = p;
	cur_head  = getAvail();
}

void XWTeX::pushNest()
{
	if (nest_ptr > max_nest_stack)
	{
		max_nest_stack = nest_ptr;
		if (nest_ptr == nest_size)
		{
			overFlow(tr("semantic nest size"), nest_size);
			return ;
		}
	}
	
	nest[nest_ptr] = cur_list;
	nest_ptr++;
	head = getAvail(); 
	tail = head; 
	prev_graf = 0; 
	mode_line = line;
	eTeX_aux = TEX_NULL;
	dir_save = TEX_NULL; 
	dir_math_save = false;
	local_par = makeLocalParNode(); 
	local_par_bool = false;
}

qint32 XWTeX::rebox(qint32 b, qint32 w)
{
	qint32 p, f, v;
	if ((width(b) != w) && (list_ptr(b) != TEX_NULL))
	{
		if (type(b) == (quint16)TEX_VLIST_NODE)
			b = hpack(b, 0, TEX_ADDITIONAL);
			
		p = list_ptr(b);
		if ((isCharNode(p)) && (link(p) == TEX_NULL))
		{
			f = font(p); 
			v = char_width(f, char_info(f, character(p)));
			if (v != width(b))
				link(p) = newKern(width(b) - v);
		}
		
		freeNode(b, TEX_BOX_NODE_SIZE);
  		b = newGlue(ss_glue); 
  		link(b) = p;
  		while (link(p) != TEX_NULL)
  			p = link(p);
  			
  		link(p) = newGlue(ss_glue);
  		return hpack(b, w, TEX_EXACTLY);
	}
	
	width(b) = w;
	return b;
}

void XWTeX::saDef(qint32 p, qint32 e)
{
	add_sa_ref(p);
	if (sa_ptr(p) == e)
	{
#ifdef XW_TEX_STAT
		if (tracingAssigns() > 0)
			showSa(p, "reassigning");
#endif //XW_TEX_STAT
		saDestroy(p);
	}
	else
	{
#ifdef XW_TEX_STAT
		if (tracingAssigns() > 0)
			showSa(p,"changing");

#endif //XW_TEX_STAT

		if (sa_lev(p) == (quint16)cur_level)
			saDestroy(p);
		else
			saSave(p);
			
		sa_lev(p) = (quint16)cur_level; 
		sa_ptr(p) = e;
#ifdef XW_TEX_STAT
		if (tracingAssigns() > 0)
			showSa(p,"into");
#endif //XW_TEX_STAT
	}
	
	deleteSaRef(p);
}

void XWTeX::saDestroy(qint32 p)
{
	if (false)
		deleteGlueRef(sa_ptr(p));
	else if (sa_ptr(p) != TEX_NULL)
	{
		if (false)
			flushNodeList(sa_ptr(p));
		else
			deleteTokenRef(sa_ptr(p));
	}
}

void XWTeX::saRestore()
{
	qint32 p;
	do
	{
		p = sa_loc(sa_chain);
		if (sa_lev(p) == (quint16)TEX_LEVEL_ONE)
		{
			saDestroy(sa_chain);
#ifdef XW_TEX_STAT
			if (tracingRestores() > 0)
				showSa(p,"retaining");
#endif //XW_TEX_STAT
		}
		else
		{
			if (false)
			{
				if (false)
					sa_int(p) = sa_int(sa_chain);
				else
					sa_int(p) = 0;
			}
			else
			{
				saDestroy(p); 
				sa_ptr(p) = sa_ptr(sa_chain);
				sa_lev(p) = sa_lev(sa_chain);
#ifdef XW_TEX_STAT
				if (tracingRestores() > 0)
					showSa(p, "restoring");
#endif //XW_TEX_STAT
			}
		}
		
		deleteSaRef(p);
		p = sa_chain; 
		sa_chain = link(p);
		if (false)
			freeNode(p, TEX_WORD_NODE_SIZE);
		else
			freeNode(p, TEX_POINTER_NODE_SIZE);
	} while (sa_chain != TEX_NULL);
}

void XWTeX::saSave(qint32 p)
{
	qint32 q, i;
	
	if (cur_level != sa_level)
	{
		checkFullSaveStack();
		save_type(save_ptr) = (quint16)TEX_RESTORE_SA;
		save_level(save_ptr) = (quint16)sa_level; 
		save_index(save_ptr) = sa_chain;
  		save_ptr++; 
  		sa_chain = TEX_NULL; 
  		sa_level = cur_level;
	}
	
	i = sa_index(p);
	if (false)
	{
		if (sa_int(p) == 0)
		{
			q = getNode(TEX_POINTER_NODE_SIZE); 
			i = i;
		}
		else
		{
			q = getNode(TEX_WORD_NODE_SIZE); 
			sa_int(q) = sa_int(p);
		}
		
		sa_ptr(q) = TEX_NULL;
	}
	else
	{
		q = getNode(TEX_POINTER_NODE_SIZE); 
		sa_ptr(q) = sa_ptr(p);
	}
	
	sa_loc(q) = p; 
	sa_index(q) = (quint16)i; 
	sa_lev(q) = sa_lev(p);
	link(q) = sa_chain; 
	sa_chain = q; 
	add_sa_ref(p);
}

void XWTeX::saWDef(qint32 p, qint32 w)
{
	add_sa_ref(p);
	if (sa_int(p) == w)
	{
		;
#ifdef XW_TEX_STAT
		if (tracingAssigns() > 0)
			showSa(p, "reassigning");
#endif //XW_TEX_STAT
	}
	else
	{
#ifdef XW_TEX_STAT
		if (tracingAssigns() > 0)
			showSa(p, "changing");
#endif //XW_TEX_STAT
		
		if (sa_lev(p) != (quint16)cur_level)
			saSave(p);
			
		sa_lev(p) = (quint16)cur_level; 
		sa_int(p) = w;
		
#ifdef XW_TEX_STAT
		if (tracingAssigns() > 0)
			showSa(p, "into");
#endif //XW_TEX_STAT
	}
	
	deleteSaRef(p);
}

void XWTeX::setMathChar(qint32 c)
{
	if (c >= 0x8000000)
	{
		cur_cs = cur_chr + TEX_ACTIVE_BASE;
		cur_cmd = eqType(cur_cs); 
		cur_chr = equiv(cur_cs);
		xToken(); 
		backInput();
	}
	else
	{
		qint32 p = newNoad(); 
		math_type(nucleus(p)) = TEX_MATH_CHAR;
  		character(nucleus(p)) = (quint16)(c % 0x10000);
  		fam(nucleus(p)) = (quint16)((c / 0x10000) % 0x100);
  		if (c >= TEX_VAR_CODE)
  		{
  			if ((curFam() >= 0) && (curFam() < 16))
  				fam(nucleus(p)) = (quint16)curFam();
  				
  			type(p) = (quint16)TEX_ORD_NOAD;
  		}
  		else
  			type(p) = (quint16)(TEX_ORD_NOAD + (c / 0x1000000));
  			
  		link(tail) = p; 
  		tail = p;
	}
}

void XWTeX::shiftCase()
{
	qint32 b, p, t, c;
	
	b = cur_chr; 
	p = scanToks(false, false); 
	p = link(def_ref);
	while (p != TEX_NULL)
	{
		t = info(p);
		if (t < (TEX_CS_TOKEN_FLAG + TEX_SINGLE_BASE))
		{
			c = t % TEX_MAX_CHAR_VAL;
			if (equiv(b + c) != 0)
				info(p) = t - c + equiv(b + c);
		}
		
		p = link(p);
	}
	
	backList(link(def_ref)); 
	freeAvail(def_ref);
}

void XWTeX::sortAvail()
{
	qint32 p = getNode(1073741824);
	p = rlink(rover); 
	rlink(rover) = TEX_MAX_HALFWORD; 
	qint32 old_rover = rover;
	while (p != old_rover)
	{
		if (p < rover)
		{
			qint32 q = p; 
			p = rlink(q); 
			rlink(q) = rover; 
			rover = q;
		}
		else
		{
			qint32 q = rover;
			while (rlink(q) < p)
				q = rlink(q);
				
			qint32 r = rlink(p); 
			rlink(p) = rlink(q); 
			rlink(q) = p; 
			p = r;
		}
	}
	
	p = rover;
	
	while (rlink(p) != TEX_MAX_HALFWORD)
	{
		llink(rlink(p)) = p; 
		p = rlink(p);
	}
	
	rlink(p) = rover; 
	llink(rover) = p;
}

void XWTeX::stackIntoBox(qint32 b, qint32 f, qint32 c)
{
	qint32 p = charBox(f, c); 
	link(p) = list_ptr(b); 
	list_ptr(b) = p;
	height(b) = height(p);
}

void XWTeX::subSup()
{
	qint32 p, t;
	
	t = TEX_EMPTY; 
	p = TEX_NULL;
	if (tail != head)
	{
		if (scripts_allowed(tail))
		{
			p = supscr(tail) + cur_cmd - TEX_SUP_MARK;
			t = math_type(p);
		}
	}
	
	if ((p == TEX_NULL) || (t != TEX_EMPTY))
	{
		tailAppend(newNoad());
		p = supscr(tail) + cur_cmd - TEX_SUP_MARK;
		if (t != TEX_EMPTY)
		{
			if (cur_cmd == TEX_SUP_MARK)
			{
				printErr(tr("Double superscript"));
				help1(tr("I treat `x^1^2' essentially like `x^1{}^2'."));
			}
			else
			{
				printErr(tr("Double subscript"));
				help1(tr("I treat `x_1_2' essentially like `x_1{}_2'."));
			}
			
			error();
		}
	}
	
	scanMath(p);
}

void XWTeX::trapZeroGlue()
{
	if ((width(cur_val) == 0) && 
		(stretch(cur_val) == 0) && 
		(shrink(cur_val) == 0))
	{
		add_glue_ref(zero_glue);
  		deleteGlueRef(cur_val); 
  		cur_val = zero_glue;
	}
}

qint32 XWTeX::varDelimiter(qint32 d, qint32 s, qint32 v)
{
	qint32 b, f, g, c, x, y, m, n, u, w, hd, z;
	TeXFourQuarters q, r;
	f = TEX_NULL_FONT; 
	w = 0; 
	bool large_attempt = false;
	z = small_fam(d); 
	x = small_char(d);
	
	while (true)
	{
		if ((z != 0) || (x != TEX_MIN_QUARTERWORD))
		{
			z = z + s + TEX_SCRIPT_SIZE;
			do
			{
				z = z - TEX_SCRIPT_SIZE; 
				g = famFnt(z);
				if (g != TEX_NULL_FONT)
				{
					y = x;
					if (((quint16)y >= font_bc(g)) && ((quint16)y <= font_ec(g)))
					{
tcontinue:
						q = char_info(g, y);
						if (char_exists(q))
						{
							if (char_tag(q) == (quint16)TEX_EXT_TAG)
							{
								f = g; 
								c = y; 
								goto found;
							}
							
							hd = height_depth(q);
							u  = char_height(g, hd) + char_depth(g, hd);
							if (u > w)
							{
								f = g; 
								c = y; 
								w = u;
								if (u >= v)
									goto found;
							}
							
							if (char_tag(q) == (quint16)TEX_LIST_TAG)
							{
								y = rem_byte(q); 
								goto tcontinue;
							}
						}
					}
				}
			} while (z >= TEX_SCRIPT_SIZE);
		}
		
		if (large_attempt)
			goto found;
			
		large_attempt = true; 
		z = large_fam(d); 
		x = large_char(d);
	}
	
found:
	if (f != TEX_NULL_FONT)
	{
		if (char_tag(q) == (quint16)TEX_EXT_TAG)
		{
			b = newNullBox();
			type(b) = (quint16)TEX_VLIST_NODE;
			r = font_info(f, exten_base(f) + rem_byte(q)).qqqq;
			c = ext_rep(f, r); 
			u = heightPlusDepth(f, c);
			w = 0; 
			q = char_info(f, c); 
			width(b) = char_width(f, q) + char_italic(f, q);
			c = ext_bot(f, r); 
			if (c != TEX_MIN_QUARTERWORD) 
				w = w + heightPlusDepth(f, c);
			c = ext_mid(f, r);
			if (c != TEX_MIN_QUARTERWORD) 
				w = w + heightPlusDepth(f, c);
			c = ext_top(f, r);
			if (c != TEX_MIN_QUARTERWORD) 
				w = w + heightPlusDepth(f, c);
			n = 0;
			if (u > 0)
			{
				while (w < v)
				{
					w = w + u; 
					n++;
					if ((ext_mid(f, r)) != TEX_MIN_QUARTERWORD)
						w = w + u;
				}
			}
			
			c = ext_bot(f, r);
			if (c != TEX_MIN_QUARTERWORD)
				stackIntoBox(b, f, c);
				
			c = ext_rep(f, r);
			for (m = 1; m <= n; m++)
				stackIntoBox(b, f, c);
			c = ext_mid(f, r);
			if (c != TEX_MIN_QUARTERWORD)
			{
				stackIntoBox(b, f, c); 
				c = ext_rep(f, r);
				for (m = 1; m <= n; m++)
					stackIntoBox(b, f, c);
			}
			c = ext_top(f, r);
			if (c != TEX_MIN_QUARTERWORD)
				stackIntoBox(b, f, c);
				
			depth(b) = w - height(b);
		}
		else
			b = charBox(f, c);
	}
	else
	{
		b = newNullBox();
  		width(b) = nullDelimiterSpace();
	}
	
	z = cur_size; 
	cur_size = s;
	shift_amount(b) = half(height(b) - depth(b)) - axis_height(cur_size);
	cur_size = z;
	
	return b;
}

qint32 XWTeX::vpackage(qint32 p, qint32 h, char m, qint32 l)
{
	qint32 r, w, d, x, s, g, o;
	
	last_badness = 0; 
	r = getNode(TEX_BOX_NODE_SIZE); 
	type(r) = (quint16)TEX_VLIST_NODE;
	if (pack_direction == -1)
		box_dir(r) = bodyDirection();
	else
	{
		box_dir(r) = pack_direction; 
		pack_direction = -1;
	}
	
	subtype(r) = (quint16)TEX_MIN_QUARTERWORD; 
	shift_amount(r) = 0;
	list_ptr(r) = p;
	w = 0;
	d = 0; 
	x = 0;
	total_stretch[TEX_NORMAL] = 0; 
	total_shrink[TEX_NORMAL] = 0;
	total_stretch[TEX_SFI] = 0; 
	total_shrink[TEX_SFI] = 0;
	total_stretch[TEX_FIL] = 0; 
	total_shrink[TEX_FIL] = 0;
	total_stretch[TEX_FILL] = 0; 
	total_shrink[TEX_FILL] = 0;
	total_stretch[TEX_FILLL] = 0; 
	total_shrink[TEX_FILLL] = 0;
	
	while (p != TEX_NULL)
	{
		if (isCharNode(p))
			confusion("vpack");
		else
		{
			switch (type(p))
			{
				case TEX_HLIST_NODE:
				case TEX_VLIST_NODE:
				case TEX_RULE_NODE:
				case TEX_UNSET_NODE:
					if ((type(p) == (quint16)TEX_HLIST_NODE) || (type(p) == (quint16)TEX_VLIST_NODE))
					{
						if (dir_orthogonal(dir_primary[box_dir(p)], dir_primary[box_dir(r)]))
						{
							x = x + d +(width(p) / 2); 
							d = width(p) / 2;
    						s = shift_amount(p);
    						if ((depth(p) + height(p) + s) > w)
    							w = depth(p) + height(p) + s;
						}
						else if ((type(p) == (quint16)TEX_HLIST_NODE) && (is_mirrored(box_dir(p))))
						{
							x = x + d + depth(p); 
							d = height(p);
    						s = shift_amount(p);
    						if ((width(p) + s) > w)
    							w = width(p) + s;
						}
						else
						{
							x = x + d + height(p); 
							d = depth(p);
    						s = shift_amount(p);
    						if ((width(p) + s) > w)
    							w = width(p) + s;
						}
					}
					else
					{
						x = x + d + height(p); 
						d = depth(p);
						if (type(p) >= (quint16)TEX_RULE_NODE)
							s = 0;
						else
							s = shift_amount(p);
							
						if ((width(p) + s) > w)
							w = width(p) + s;
					}
					break;
					
				case TEX_WHATSIT_NODE:
					break;
					
				case TEX_GLUE_NODE:
					x = x + d; 
					d = 0;
					g = glue_ptr(p); 
					x = x + width(g);
					o = stretch_order(g); 
					total_stretch[o] = total_stretch[o] + stretch(g);
					o = shrink_order(g); 
					total_shrink[o] = total_shrink[o] + shrink(g);
					if (subtype(p) >= (quint16)TEX_A_LEADERS)
					{
						g = leader_ptr(p);
						if (width(g) > w)
							w = width(g);
					}
					break;
					
				case TEX_KERN_NODE:
					x =x + d + width(p); 
					d = 0;
					break;
					
				default:
					break;
			}
		}
		
		p = link(p);
	}
	
	width(r) = w;
	if (d > l)
	{
		x = x + d - l; 
		depth(r) = l;
	}
	else
		depth(r) = d;
		
	if (m == TEX_ADDITIONAL)
		h = x + h;
		
	height(r) = h; 
	x = h - x;
	
	if (x == 0)
	{
		glue_sign(r)  = (quint16)TEX_NORMAL; 
		glue_order(r) = (quint16)TEX_NORMAL;
		glue_set(r) = 0.0;
		return r;
	}
	else if (x > 0)
	{
		if (total_stretch[TEX_FILLL] != 0)
			o = TEX_FILLL;
		else if (total_stretch[TEX_FILL] != 0)
			o = TEX_FILL;
		else if (total_stretch[TEX_FIL] != 0)
			o = TEX_FIL;
		else if (total_stretch[TEX_SFI] != 0)
			o = TEX_SFI;
		else
			o = TEX_NORMAL;
			
		glue_order(r) = (quint16)o; 
		glue_sign(r)  = (quint16)TEX_STRETCHING;
		
		if (total_stretch[o] != 0)
			glue_set(r) = (x / total_stretch[o]);
		else
		{
			glue_sign(r) = (quint16)TEX_NORMAL;
			glue_set(r)  = 0.0;
		}
		
		if (o == TEX_NORMAL)
		{
			if (list_ptr(r) != TEX_NULL)
			{
				last_badness = badness(x, total_stretch[TEX_NORMAL]);
				if (last_badness > vbadness())
				{
					println();
					if (last_badness > 100)
						printnl(tr("Underfull"));
					else
						printnl(tr("Loose"));
						
					print(tr(" \\vbox (badness ")); 
					printInt(last_badness);
					goto common_ending;
				}
			}
		}
		
		return r;
	}
	else
	{
		if (total_shrink[TEX_FILLL] != 0)
			o = TEX_FILLL;
		else if (total_shrink[TEX_FILL] != 0)
			o = TEX_FILL;
		else if (total_shrink[TEX_FIL] != 0)
			o = TEX_FIL;
		else if (total_shrink[TEX_SFI] != 0)
			o = TEX_SFI;
		else
			o = TEX_NORMAL;
			
		glue_order(r) = (quint16)o; 
		glue_sign(r)  = (quint16)TEX_SHRINKING;
		if (total_shrink[o] != 0)
			glue_set(r) = ((-x) / total_shrink[o]);
		else
		{
			glue_sign(r) = (quint16)TEX_NORMAL;
			glue_set(r) = 0.0;
		}
		
		if ((total_shrink[o] < -x) && (o == TEX_NORMAL) && (list_ptr(r) != TEX_NULL))
		{
			last_badness = 1000000;
			glue_set(r) = 1.0;
			if ((-x - total_shrink[TEX_NORMAL] > vfuzz()) || (vbadness() < 100))
			{
				println(); 
				printnl(tr("Overfull \\vbox ("));
				printScaled(-x - total_shrink[TEX_NORMAL]); 
				print(tr("pt too high"));
  				goto common_ending;
			}
		}
		else if (o == TEX_NORMAL)
		{
			if (list_ptr(r) != TEX_NULL)
			{
				last_badness = badness(-x, total_shrink[TEX_NORMAL]);
				if (last_badness > vbadness())
				{
					println(); 
					printnl(tr("Tight \\vbox (badness ")); 
					printInt(last_badness);
					goto common_ending;
				}
			}
		}
		
		return r;
	}
	
common_ending:
	if (output_active)
		print(tr(") has occurred while \\output is active"));
	else
	{
		if (pack_begin_line != 0)
		{
			print(tr(") in alignment at lines "));
    		printInt(qAbs(pack_begin_line));
    		print("--");
		}
		else
			print(tr(") detected at line "));
		
		printInt(line);
  	println();
	}
	
	beginDiagnostic(); 
	showBox(r); 
	endDiagnostic(true);
	
	return r;
}

void XWTeX::unpackage()
{
	qint32 p, c;
	
	if (cur_chr > TEX_COPY_CODE)
	{
		link(tail) = disc_ptr[cur_chr]; 
		disc_ptr[cur_chr] = TEX_NULL;
		goto done;
	}
	
	c = cur_chr; 
	scanEightBitInt(); 
	p = box(cur_val);
	if (p == TEX_NULL)
		return ;
		
	if ((qAbs(mode) == TEX_MMODE) || 
		((qAbs(mode) == TEX_VMODE) && (type(p) != (quint16)TEX_VLIST_NODE)) || 
		((qAbs(mode) == TEX_HMODE) && (type(p) != (quint16)TEX_HLIST_NODE)))
	{
		printErr(tr("Incompatible list can't be unboxed"));
		help3(tr("Sorry, Pandora. (You sneaky devil.)"));
		help_line[1] = tr("I refuse to unbox an \\hbox in vertical mode or vice versa.");
		help_line[0] = tr("And I can't open any boxes in math mode.");
		error(); 
		return;
	}
	
	if (c == TEX_COPY_CODE)
		link(tail) = copyNodeList(list_ptr(p));
	else
	{
		link(tail) = list_ptr(p);
		setEquiv(TEX_BOX_BASE + cur_val, TEX_NULL);
  		freeNode(p, TEX_BOX_NODE_SIZE);
	}
	
done:
	while (link(tail) != TEX_NULL)
		tail = link(tail);
}

