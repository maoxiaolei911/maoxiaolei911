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
#include "XWTeX.h"

void  XWTeX::alignPeek()
{
restart: 
	align_state = 1000000;
	do
	{
		getXOrProtected();
	} while (cur_cmd == TEX_SPACER);
	
	if (cur_cmd == TEX_NO_ALIGN)
	{
		scanLeftBrace(); 
		newSaveLevel(TEX_NO_ALIGN_GROUP);
		if (mode == -TEX_VMODE)
			normalParagraph();
	}
	else if (cur_cmd == TEX_RIGHT_BRACE)
		finAlign();
	else if ((cur_cmd == TEX_CAR_RET) && (cur_chr == TEX_CR_CR_CODE))
		goto restart;
	else
	{
		initRow();
		initCol();
	}
}

void XWTeX::changeIfLimit(quint16 l, qint32 p)
{
	if (p == cond_ptr)
		if_limit = l;
	else
	{
		qint32 q = cond_ptr;
		while (true)
		{
			if (q == TEX_NULL)
			{
				confusion("if");
				return ;
			}
			
			if (link(q) == p)
			{
				type(q) = (quint16)l; 
				return;
			}
			
			q = link(q);
		}
	}
}

void XWTeX::conditional()
{
	bool b, is_unless;
	qint32 r, m, n, p, q, save_cond_ptr;
	char save_scanner_status, this_if;
	
	if (tracingIfs() > 0)
	{
		if (tracingCommands() <= 1)
			showCurCmdChr();
	}
	
	p = getNode(TEX_IF_NODE_SIZE); 
	link(p) = cond_ptr; 
	type(p) = (quint16)if_limit;
	subtype(p) = (quint16)cur_if; 
	if_line_field(p) = if_line;
	cond_ptr = p; 
	cur_if = cur_chr; 
	if_limit = TEX_IF_CODE; 
	if_line = line;
	save_cond_ptr = cond_ptr;
	is_unless = (cur_chr >= TEX_UNLESS_CODE); 
	this_if = cur_chr % TEX_UNLESS_CODE;	
	switch (this_if)
	{
		case TEX_IF_CHAR_CODE: 
		case TEX_IF_CAT_CODE:
			getXToken();
			if (cur_cmd == TEX_RELAX)
			{
				if (cur_chr == TEX_NO_EXPAND_FLAG)
				{
					cur_cmd = TEX_ACTIVE_CHAR;
    				cur_chr = cur_tok - TEX_CS_TOKEN_FLAG - TEX_ACTIVE_BASE;
				}
			}
			if ((cur_cmd > TEX_ACTIVE_CHAR) || (cur_chr > TEX_BIGGEST_CHAR))
			{
				m = TEX_RELAX; 
				n = TEX_TOO_BIG_CHAR;
			}
			else 
			{
				m = cur_cmd; 
				n = cur_chr;
			}
			getXToken();
			if (cur_cmd == TEX_RELAX)
			{
				if (cur_chr == TEX_NO_EXPAND_FLAG)
				{
					cur_cmd = TEX_ACTIVE_CHAR;
    				cur_chr = cur_tok - TEX_CS_TOKEN_FLAG - TEX_ACTIVE_BASE;
				}
			}
			if ((cur_cmd > TEX_ACTIVE_CHAR) || (cur_chr > TEX_BIGGEST_CHAR))
			{
				cur_cmd = TEX_RELAX; 
				cur_chr = TEX_TOO_BIG_CHAR;
			}
			if (this_if == TEX_IF_CHAR_CODE)
				b = (n == cur_chr);
			else
				b = (m == cur_cmd);
			break;
			
		case TEX_IF_INT_CODE: 
		case TEX_IF_DIM_CODE:
			if (this_if == TEX_IF_INT_CODE)
				scanInt();
			else
				scanNormalDimen();
			n = cur_val;
			do
			{
				getXToken();
			} while (cur_cmd == TEX_SPACER);
			
			if ((cur_tok >= (TEX_OTHER_TOKEN + '<')) && (cur_tok <= (TEX_OTHER_TOKEN + '>')))
				r = (char)(cur_tok - TEX_OTHER_TOKEN);
			else
			{
				printErr(tr("Missing = inserted for "));
				printCmdChr((quint16)TEX_IF_TEST, this_if);
  				help1(tr("I was expecting to see `<', `=', or `>'. Didn't."));
  				backError(); 
  				r = '=';
			}
			
			if (this_if == TEX_IF_INT_CODE)
				scanInt();
			else
				scanNormalDimen();
			{
				switch (r)
				{
					case '<': 
						b = (n < cur_val);
						break;
						
					case '=': 
						b = (n == cur_val);
						break;
						
					case '>': 
						b =(n > cur_val);
						break;
						
					default:
						break;
				}
			}
			break;
			
		case TEX_IF_ODD_CODE:
			scanInt(); 
			b = odd(cur_val);
			break;
			
		case TEX_IF_VMODE_CODE: 
			b = (qAbs(mode) == TEX_VMODE);
			break;
			
		case TEX_IF_HMODE_CODE: 
			b = (qAbs(mode) == TEX_HMODE);
			break;
			
		case TEX_IF_MMODE_CODE: 
			b = (qAbs(mode) == TEX_MMODE);
			break;
			
		case TEX_IF_INNER_CODE: 
			b = (mode < 0);
			break;
			
		case TEX_IF_VOID_CODE: 
		case TEX_IF_HBOX_CODE: 
		case TEX_IF_VBOX_CODE:
			scanRegisterNum(); 
			if (cur_val < TEX_NUMBER_REGS)
				p = box(cur_val);
			else
			{
				findSaElement(TEX_BOX_VAL, cur_val, false);
				if (cur_ptr == TEX_NULL)
					p = TEX_NULL;
				else
					p = sa_ptr(cur_ptr);
			}
			if (this_if == TEX_IF_VOID_CODE)
				b = (p == TEX_NULL);
			else if (p == TEX_NULL)
				b = false;
			else if (this_if == TEX_IF_HBOX_CODE)
				b = (type(p) == (quint16)TEX_HLIST_NODE);
			else
				b = (type(p) == (quint16)TEX_VLIST_NODE);
			break;
			
		case TEX_IFX_CODE:
			save_scanner_status = scanner_status; 
			scanner_status = TEX_NORMAL;
			getNext(); 
			n = cur_cs; 
			p = cur_cmd; 
			q = cur_chr;
			getNext(); 
			if (cur_cmd != p) 
				b = false;
			else if (cur_cmd < TEX_CALL) 
				b = (cur_chr == q);
			else
			{
				p = link(cur_chr); 
				q = link(equiv(n));
				if (p == q)
					b = true;
				else
				{
					while ((p != TEX_NULL) && (q != TEX_NULL))
					{
						if (info(p) != info(q))
							p = TEX_NULL;
						else
						{
							p = link(p); 
							q = link(q);
						}
					}
					
					b = ((p == TEX_NULL) && (q == TEX_NULL));
				}
			}
			scanner_status = save_scanner_status;
			break;
			
		case TEX_IF_EOF_CODE:
			scanFourBitIntOr18(); 
			if (cur_val == 18)
				b = !shell_enabled_p;
			else
				b = (read_open[cur_val] == TEX_CLOSED);
			break;
			
		case TEX_IF_TRUE_CODE: 
			b = true;
			break;
			
		case TEX_IF_FALSE_CODE: 
			b = false;
			break;
			
		case TEX_IF_DEF_CODE:
			save_scanner_status = scanner_status;
  			scanner_status = TEX_NORMAL;
  			getNext(); 
  			b= (cur_cmd != TEX_UNDEFINED_CS);
  			scanner_status = save_scanner_status;
			break;
			
		case TEX_IF_CS_CODE:
			n = getAvail(); 
			p = n;
			do
			{
				getXToken();
				if (cur_cs == 0)
				{ 
					q = getAvail(); 
  					link(p) = q; 
  					info(q) = cur_tok;
  					p = q;
				}
			} while (cur_cs == 0);
			if (cur_cmd != TEX_END_CS_NAME)
			{
				printErr(tr("Missing ")); 
				printEsc(TeXEndCsName); 
				print(tr(" inserted"));
				help2(tr("The control sequence marked <to be read again> should"));
				help_line[0] = tr("not appear between \\csname and \\endcsname.");
				backError();
			}
			
			m = first; 
			p = link(n);
			while (p != TEX_NULL)
			{
				if (m >= max_buf_stack)
				{
					max_buf_stack = m + 1;
					if (max_buf_stack == buf_size)
					{
						overFlow(tr("buffer size"), buf_size);
						return ;
					}
				}
				
				buffer[m] = info(p) % 256; 
				m++; 
				p = link(p);
			}
			if (m > (first + 1))
				cur_cs = idLookup(first, m - first);
			else if (m == first)
				cur_cs = TEX_NULL_CS;
			else
				cur_cs = TEX_SINGLE_BASE + buffer[first];
			flushList(n);
  			b = (eqType(cur_cs) != TEX_UNDEFINED_CS);
			break;
			
		case TEX_IF_FONT_CHAR_CODE:
			scanFontIdent(); 
			n = cur_val; 
			scanCharNum();
			if ((font_bc(n) <= cur_val) && (font_ec(n) >= cur_val))
				b = char_exists(char_info(n, cur_val));
			else
				b = false;
			break;
			
		case TEX_IF_CASE_CODE:
			scanInt(); 
			n = cur_val;
			if (tracingCommands() > 1)
			{
				beginDiagnostic(); 
				print("{case "); 
				printInt(n); 
				printChar('}');
  				endDiagnostic(false);
			}
			while (n != 0)
			{
				passText();
				if (cond_ptr == save_cond_ptr)
				{
					if (cur_chr == TEX_OR_CODE)
						n--;
					else
						goto common_ending;
				}
				else if (cur_chr == TEX_FI_CODE)
				{
					if (if_stack[in_open] == cond_ptr)
						ifWarning();
						
					p        = cond_ptr; 
					if_line  = if_line_field(p);
					cur_if   = subtype(p); 
					if_limit = type(p); 
					cond_ptr = link(p);
					freeNode(p, TEX_IF_NODE_SIZE);
				}
			}
			changeIfLimit((quint16)TEX_OR_CODE, save_cond_ptr);
			return;
			break;
			
		default:
			break;
	}
	
	if (is_unless)
		b = !b;
		
	if (tracingCommands() > 1)
	{
		beginDiagnostic();
		if (b)
			print("{true}");
		else
			print("{false}");
		endDiagnostic(false);
	}
	
	if (b)
	{
		changeIfLimit((quint16)TEX_ELSE_CODE, save_cond_ptr);
  		return;
	}
	
	while (true)
	{
		passText();
		if (cond_ptr == save_cond_ptr)
		{
			if (cur_chr != TEX_OR_CODE)
				goto common_ending;
			printErr(tr("Extra ")); 
			printEsc(TeXOR);
			help1(tr("I'm ignoring this; it doesn't match any \\if."));
    		error();
		}
		else if (cur_chr == TEX_FI_CODE)
		{
			if (if_stack[in_open] == cond_ptr)
				ifWarning();
						
			p        = cond_ptr; 
			if_line  = if_line_field(p);
			cur_if   = subtype(p); 
			if_limit = type(p); 
			cond_ptr = link(p);
			freeNode(p, TEX_IF_NODE_SIZE);
		}
	}
	
common_ending: 
	if (cur_chr == TEX_FI_CODE)
	{
		if (if_stack[in_open] == cond_ptr)
			ifWarning();
						
		p        = cond_ptr; 
		if_line  = if_line_field(p);
		cur_if   = subtype(p); 
		if_limit = type(p); 
		cond_ptr = link(p);
		freeNode(p, TEX_IF_NODE_SIZE);
	}
	else
		if_limit = TEX_FI_CODE;
}

void XWTeX::convToks()
{
	char save_scanner_status;
	qint32 b;
	qint32 c = cur_chr;
	switch (c)
	{
		case TEX_NUMBER_CODE:
		case TEX_ROMAN_NUMERAL_CODE: 
			scanInt();
			break;
			
		case TEX_STRING_CODE: 
		case TEX_MEANING_CODE:
			save_scanner_status = scanner_status;
  			scanner_status = TEX_NORMAL; 
  			getToken(); 
  			scanner_status = save_scanner_status;
			break;
			
		case TEX_FONT_NAME_CODE: 
			scanFontIdent();
			break;
			
		case TEX_ETEX_CODE: 
			break;
			
		case TEX_OMEGA_CODE: 
			break;
			
		case TEX_ALEPH_CODE: 
			break;
			
		case TEX_JOB_NAME_CODE:
			if (job_name == 0)
				openLogFile();
			break;
			
		case TEX_ALEPH_REVISION_CODE: 
			break;
			
		case TEX_OMEGA_REVISION_CODE: 
			break;
			
		case TEX_ETEX_REVISION_CODE: 
			break;
			
		default:
			break;
	}
	
	old_setting = selector; 
	selector = TEX_NEW_STRING; 
	b = pool_ptr;
	
	switch (c)
	{
		case TEX_NUMBER_CODE: 
			printInt(cur_val);
			break;
			
		case TEX_ROMAN_NUMERAL_CODE: 
			printRomanInt(cur_val);
			break;
			
		case TEX_STRING_CODE:
			if (cur_cs != 0)
				sprintCS(cur_cs);
			else
				printChar(cur_chr);
			break;
			
		case TEX_MEANING_CODE: 
			printMeaning();
			break;
			
		case TEX_FONT_NAME_CODE:
			print(font_name(cur_val));
			if (font_size(cur_val) != font_dsize(cur_val))
			{
				print(" at "); 
				printScaled(font_size(cur_val));
    			print(TeXPT);
			}
			break;
			
		case TEX_ETEX_CODE: 
			print("2.1");
			break;
			
		case TEX_OMEGA_CODE: 
			print("1.15");
			break;
			
		case TEX_ALEPH_CODE: 
			print("0 (RC2)");
			break;
			
		case TEX_JOB_NAME_CODE: 
			print(job_name);
			break;
			
		case TEX_ALEPH_REVISION_CODE: 
			print("(RC2)");
			break;
			
		case TEX_OMEGA_REVISION_CODE: 
			print(".15");
			break;
			
		case TEX_ETEX_REVISION_CODE: 
			print(".1");
			break;
			
		default:
			break;
	}
	
	selector = old_setting; 
	link(garbage) = strToks(b); 
	insList(link(temp_head));
}

void XWTeX::findFontDimen(bool writing)
{
	scanInt(); 
	qint32 n = cur_val; 
	scanFontIdent(); 
	qint32 f = cur_val;
	dimen_font = f;
	if (n <= 0)
		cur_val = font_file_size(f);
	else
	{
		if (writing && 
			(n <= TEX_SPACE_SHRINK_CODE) && 
			(n >= TEX_SPACE_CODE) && 
			(font_glue(f) != TEX_NULL))
		{
			deleteGlueRef(font_glue(f));
    		font_glue(f) = TEX_NULL;
		}
		
		if (n > font_params(f))
		{
			if (f < font_ptr)
				cur_val = font_file_size(f);
			else
			{
				if ((n + font_params(f)) > font_file_size(f))
				{
					printnl(tr("Out of font parameter space")); 
					succumb();
				}
				
				do
				{
					font_info(f, param_base(f) + font_params(f)).ii.CINT0 = 0; 
					(font_params(f))++;
				} while (n != font_params(f));
				
				cur_val = param_base(f) + font_params(f);
			}
		}
		else
			cur_val = n + param_base(f);
	}
	
	if (cur_val == font_file_size(f))
	{
		printErr(tr("Font ")); 
		printEsc(fontIdText(f));
  		print(tr(" has only ")); 
  		printInt(font_params(f));
  		print(tr(" fontdimen parameters"));
  		help2(tr("To increase the number of font parameters, you must"));
    	help_line[0] = tr("use \\fontdimen immediately after the \\font is loaded.");
  		error();
	}
}

void XWTeX::passText()
{
	char save_scanner_status = scanner_status; 
	scanner_status = TEX_SKIPPING; 
	qint32 l = 0;
	skip_line = line;
	
	while (true)
	{
		getNext();
		if (cur_cmd == TEX_FI_OR_ELSE)
		{
			if (l == 0)
				goto done;
				
			if (cur_chr == TEX_FI_CODE)
				l--;
		}
		else if (cur_cmd == TEX_IF_TEST)
			l++;
	}
	
done: 
	scanner_status = save_scanner_status;
	if (tracingIfs() > 0)
		showCurCmdChr();
}

void XWTeX::scanBigFifteenBitInt()
{
	scanInt();
	if ((cur_val < 0) || (cur_val > 0x7FFFFFF))
	{
		printErr(tr("Bad number"));
		help2(tr("Since I expected to read a number between 0 and 0x7FFFFFF,"));
		help_line[0] = tr("I changed this one to zero.");
		intError(cur_val); 
		cur_val = 0;
	}
}

void XWTeX::scanBigFourBitInt()
{
	scanInt();
	if ((cur_val < 0) || (cur_val > 255))
	{
		printErr(tr("Bad number"));
		help2(tr("Since I expected to read a number between 0 and 255,"));
		help_line[0] = tr("I changed this one to zero.");
		intError(cur_val); 
		cur_val = 0;
	}
}

void XWTeX::scanBox(qint32 box_context)
{
	do
	{
		getXToken();
	} while ((cur_cmd == TEX_SPACER) || (cur_cmd == TEX_RELAX));
	
	if (cur_cmd == TEX_MAKE_BOX)
		beginBox(box_context);
	else if ((box_context >= TEX_LEADER_FLAG) && 
			((cur_cmd == TEX_HRULE) || (cur_cmd == TEX_VRULE)))
	{
		cur_box = scanRuleSpec();
		boxEnd(box_context);
	}
	else
	{
		printErr(tr("A <box> was supposed to be here"));
		help3(tr("I was expecting to see \\hbox or \\vbox or \\copy or \\box or"));
		help_line[1] = tr("something like that. So you might find something missing in");
		help_line[0] = tr("your output. But keep trying; you can fix this later.");
		backError();
	}
}

void XWTeX::scanCharNum()
{
	scanInt();
	if ((cur_val < 0) || (cur_val > TEX_BIGGEST_CHAR))
	{
		printErr(tr("Bad character code"));
		help2(tr("A character number must be between 0 and 65535."));
		help_line[0] = tr("I changed this one to zero.");
		intError(cur_val); 
		cur_val = 0;
	}
}

void XWTeX::scanDelimiter(qint32 p, qint32 r)
{
	if (r == 1)
		scanTwentySevenBitInt();
	else if (r == 2)
		scanFiftyOneBitInt();
	else
	{
		do
		{
			getXToken();
		} while ((cur_cmd == TEX_SPACER) || (cur_cmd == TEX_RELAX));
		
		switch (cur_cmd)
		{
			case TEX_LETTER:
			case TEX_OTHER_CHAR:
				cur_val = delCode0(cur_chr); 
				cur_val1 = delCode1(cur_chr);
				break;
				
			case TEX_DELIM_NUM:
				if (cur_chr == 0)
					scanTwentySevenBitInt();
				else
					scanFiftyOneBitInt();
				break;
				
			default:
				cur_val = -1; 
				cur_val1 = -1;
				break;
		}
	}
	
	if (cur_val < 0)
	{
		printErr(tr("Missing delimiter (. inserted)"));
		help6(tr("I was expecting to see something like `(' or `\\{' or"));
		help_line[4] = tr("`\\}' here. If you typed, e.g., `{' instead of `\\{', you");
		help_line[3] = tr("should probably delete the `{' by typing `1' now, so that");
		help_line[2] = tr("braces don't get unbalanced. Otherwise just proceed.");
		help_line[1] = tr("Acceptable delimiters are characters whose \\delcode is");
		help_line[0] = tr("nonnegative, or you can use `\\delimiter <delimiter code>'.");
		backError(); 
		cur_val = 0;
		cur_val1 = 0;
	}
	
	small_fam(p) = (quint16)((cur_val / 0x10000) % 0x100);
	small_char(p) = (quint16)(cur_val % 0x10000);
	large_fam(p) = (quint16)((cur_val1 / 0x10000) % 0x100);
	large_char(p) = (quint16)(cur_val1 % 0x10000);
}

void XWTeX::scanDimen(bool mu, bool inf, bool shortcut)
{
	qint32 f = 0; 
	arith_error = false; 
	cur_order = TEX_NORMAL; 
	qint32 num, denom, p, q, v, save_cur_val;
	char k, kk;
	bool negative = false;
	if (!shortcut)
	{
		do
		{
			do
			{
				getXToken();
			} while (cur_cmd == TEX_SPACER);
		
			if (cur_tok == (TEX_OTHER_TOKEN + '-'))
			{
				negative = !negative; 
				cur_tok  = TEX_OTHER_TOKEN + '+';
			}
		} while (cur_tok == (TEX_OTHER_TOKEN + '+'));
		
		if ((cur_cmd >= TEX_MIN_INTERNAL) && (cur_cmd <= TEX_MAX_INTERNAL))
		{
			if (mu)
			{
				scanSomethingInternal(TEX_MU_VAL, false);
				if (cur_val_level >= TEX_GLUE_VAL)
				{
					v = width(cur_val); 
					deleteGlueRef(cur_val); 
					cur_val = v;
				}
				
				if (cur_val_level == TEX_MU_VAL)
					goto attach_sign;
					
				if (cur_val_level != TEX_INT_VAL)
					muError();
			}
			else
			{
				scanSomethingInternal(TEX_DIMEN_VAL, false);
				if (cur_val_level == TEX_DIMEN_VAL)
					goto attach_sign;
			}
		}
		else
		{
			backInput();
			if (cur_tok == TEX_CONTINENTAL_POINT_TOKEN)
				cur_tok = TEX_POINT_TOKEN;
				
			if (cur_tok != TEX_POINT_TOKEN)
				scanInt();
			else
			{
				radix   = 10; 
				cur_val = 0;
			}
			
			if (cur_tok == TEX_CONTINENTAL_POINT_TOKEN)
				cur_tok = TEX_POINT_TOKEN;
				
			if ((radix == 10) && (cur_tok == TEX_POINT_TOKEN))
			{
				k = 0; 
				p = TEX_NULL; 
				getToken();
				while (true)
				{
					getXToken();
					if ((cur_tok > (TEX_ZERO_TOKEN + 9)) || (cur_tok < TEX_ZERO_TOKEN))
						goto done1;
						
					if (k < 17)
					{
						q = getAvail(); 
						link(q) = p; 
						info(q) = cur_tok - TEX_ZERO_TOKEN;
						p = q; 
						k++;
					}
				}
				
done1:
				for (kk = k; kk > 0; kk--)
				{
					dig[kk -  1] = info(p); 
					q = p; 
					p = link(p); 
					freeAvail(q);
				}
				
				f = roundDecimals(k);
				if (cur_cmd != TEX_SPACER)
					backInput();
			}
		}
	}
	
	if (cur_val < 0)
	{
		negative = !negative; 
		cur_val = -cur_val;
	}
	
	if (inf)
	{
		if (scanKeyWord(TeXFI))
		{
			cur_order = TEX_SFI;
			while (scanKeyWord('l'))
			{
				if (cur_order == TEX_FILLL)
				{
					printErr(tr("Illegal unit of measure ("));
					print(tr("replaced by filll)"));
					help1(tr("I dddon't go any higher than filll.")); 
					error();
				}
				else
					cur_order++;
			}
			
			goto attach_fraction;
		}
	}
	
	save_cur_val = cur_val;
	do
	{
		getXToken();
	} while (cur_cmd == TEX_SPACER);
	
	if ((cur_cmd < TEX_MIN_INTERNAL) || (cur_cmd > TEX_MAX_INTERNAL))
		backInput();
	else
	{
		if (mu)
		{
			scanSomethingInternal(TEX_MU_VAL, false);
			if (cur_val_level >= TEX_GLUE_VAL)
			{
				v = width(cur_val); 
				deleteGlueRef(cur_val); 
				cur_val = v;
			}
			
			if (cur_val_level != TEX_MU_VAL)
				muError();
		}
		else
			scanSomethingInternal(TEX_DIMEN_VAL, false);
			
		v = cur_val; 
		goto found;
	}
	
	if (mu)
		goto not_found;
		
	if (scanKeyWord(TeXEM))
		v = quad(curFont());
	else if (scanKeyWord(TeXEX))
		v = x_height(curFont());
	else
		goto not_found;
		
	getXToken();
	if (cur_cmd != TEX_SPACER)
		backInput();
		
found:
	cur_val = nxPlusY(save_cur_val, v, xnOverD(v, f, 0x10000));
	goto attach_sign;
	
not_found:
	if (mu)
	{
		if (scanKeyWord(TeXMU))
			goto attach_fraction;
		else
		{
			printErr(tr("Illegal unit of measure (")); 
			print(tr("mu inserted)"));
			help4(tr("The unit of measurement in math glue must be mu."));
			help_line[2] = tr("To recover gracefully from this error, it's best to");
			help_line[1] = tr("delete the erroneous units; e.g., type `2' to delete");
			help_line[0] = tr("two letters. (See Chapter 27 of The TeXbook.)");
			error(); 
			goto attach_fraction;
		}
	}
	
	if (scanKeyWord(TeXTrue))
	{
		prepareMag();
		if (mag() != 1000)
		{
			cur_val = xnOverD(cur_val, 1000, mag());
  			f = (1000 * f + 0x10000 * remainder) / mag();
  			cur_val = cur_val +(f / 0x10000); 
  			f =f % 0x10000;
		}
	}
	
	if (scanKeyWord(TeXPT))
		goto attach_fraction;
		
	if (scanKeyWord(TeXIN))
	{
		num   = 7227;
		denom = 100;
	}
	else if (scanKeyWord(TeXPC))
	{
		num   = 12;
		denom = 1;
	}
	else if (scanKeyWord(TeXCM))
	{
		num   = 7227;
		denom = 254;
	}
	else if (scanKeyWord(TeXMM))
	{
		num   = 7227;
		denom = 2540;
	}
	else if (scanKeyWord(TeXBP))
	{
		num   = 7227;
		denom = 7200;
	}
	else if (scanKeyWord(TeXDD))
	{
		num   = 1238;
		denom = 1157;
	}
	else if (scanKeyWord(TeXCC))
	{
		num   = 14856;
		denom = 1157;
	}
	else if (scanKeyWord(TeXSP))
		goto done;
	else
	{
		printErr(tr("Illegal unit of measure (")); 
		print(tr("pt inserted)"));
		help6(tr("Dimensions can be in units of em, ex, in, pt, pc,"));
		help_line[4] = tr("cm, mm, dd, cc, bp, or sp; but yours is a new one!");
		help_line[3] = tr("I'll assume that you meant to say pt, for printer's points.");
		help_line[2] = tr("To recover gracefully from this error, it's best to");
		help_line[1] = tr("delete the erroneous units; e.g., type `2' to delete");
		help_line[0] = tr("two letters. (See Chapter 27 of The TeXbook.)");
		error(); 
		goto done2;
	}
	
	cur_val = xnOverD(cur_val, num, denom);
    f = (num * f + 0x10000 * remainder) / denom;
    cur_val = cur_val +(f / 0x10000); 
    f = f % 0x10000;
    
done2:
attach_fraction: 
	if (cur_val >= 0x4000) 
		arith_error = true;
	else
		cur_val = cur_val * TEX_UNITY + f;
		
done:
	getXToken();
	if (cur_cmd != TEX_SPACER)
		backInput();
		
attach_sign:
	if (arith_error || (qAbs(cur_val) >= 0x40000000))
	{
		printErr(tr("Dimension too large"));
		help2(tr("I can't work with sizes bigger than about 19 feet."));
		help_line[0] = tr("Continue and I'll use the largest value I can.");
		error(); 
		cur_val = TEX_MAX_DIMEN; 
		arith_error = false;
	}
	
	if (negative)
		cur_val = -cur_val;
}

void XWTeX::scanDir()
{
	getXToken();
	if (cur_cmd == TEX_ASSIGN_DIR)
	{
		cur_val = newEqtbInt(cur_chr);
		return ;
	}
	else
		backInput();
		
	qint32 d1;
	if (scanKeyWord('T'))
		d1 = TEX_DIR_T;
	else if (scanKeyWord('L'))
		d1 = TEX_DIR_L;
	else if (scanKeyWord('B'))
		d1 = TEX_DIR_B;
	else if (scanKeyWord('R'))
		d1 = TEX_DIR_R;
	else
	{
		printErr(tr("Bad direction")); 
		cur_val = 0;
		return ;
	}
	
	qint32 d2;
	if (scanKeyWord('T'))
		d2 = TEX_DIR_T;
	else if (scanKeyWord('L'))
		d2 = TEX_DIR_L;
	else if (scanKeyWord('B'))
		d2 = TEX_DIR_B;
	else if (scanKeyWord('R'))
		d2 = TEX_DIR_R;
	else
	{
		printErr(tr("Bad direction")); 
		cur_val = 0;
		return ;
	}
	
	if (dir_parallel(d1, d2))
	{
		printErr(tr("Bad direction")); 
		cur_val = 0;
		return ;
	}
	
	qint32 d3;
	if (scanKeyWord('T'))
		d3 = TEX_DIR_T;
	else if (scanKeyWord('L'))
		d3 = TEX_DIR_L;
	else if (scanKeyWord('B'))
		d3 = TEX_DIR_B;
	else if (scanKeyWord('R'))
		d3 = TEX_DIR_R;
	else
	{
		printErr(tr("Bad direction")); 
		cur_val = 0;
		return ;
	}
	
	cur_val = d1 * 8 + dir_rearrange[d2] * 4 + d3;
}

void XWTeX::scanEightBitInt()
{
	scanInt();
	if ((cur_val < 0) || (cur_val > TEX_BIGGEST_REG))
	{
		printErr(tr("Bad register code"));
		help2(tr("A register number must be between 0 and 65535."));
		help_line[0] = tr("I changed this one to zero.");
		intError(cur_val); 
		cur_val = 0;
	}
}

void XWTeX::scanExpr()
{
	bool a, b;
	qint32 l, r, s, o, e, t, f, n, p, q;
	
	l = cur_val_level; 
	a = arith_error; 
	b = false; 
	p = TEX_NULL;
	
restart: 
	r = TEX_EXPR_NONE; 
	e = 0; 
	s = TEX_EXPR_NONE; 
	t = 0; 
	n = 0;
	
tcontinue:
	if (s == TEX_EXPR_NONE)
		o = l;
	else
		o = TEX_INT_VAL;
		
	do
	{
		getXToken();
	} while (cur_cmd == TEX_SPACER);
	
	if (cur_tok == (TEX_OTHER_TOKEN + '('))
	{
		q = getNode(TEX_EXPR_NODE_SIZE); 
		link(q) = p; 
		type(q) = (quint16)l;
		subtype(q) = (quint16)(4 * s + r);
		expr_e_field(q) = e; 
		expr_t_field(q) = t; 
		expr_n_field(q) = n;
		p = q; 
		l = o; 
		goto restart;
	}
	
	backInput();
	if (o == TEX_INT_VAL)
		scanInt();
	else if (o == TEX_DIMEN_VAL)
		scanNormalDimen();
	else if (o == TEX_GLUE_VAL)
		scanNormalGlue();
	else
		scanMuGlue();
	f = cur_val;
	
found:
	do
	{
		getXToken();
	} while (cur_cmd == TEX_SPACER);
	
	if (cur_tok == (TEX_OTHER_TOKEN + '+'))
		o = TEX_EXPR_ADD;
	else if (cur_tok == (TEX_OTHER_TOKEN + '-'))
		o = TEX_EXPR_SUB;
	else if (cur_tok == (TEX_OTHER_TOKEN + '*'))
		o = TEX_EXPR_MULT;
	else if (cur_tok == (TEX_OTHER_TOKEN + '/'))
		o = TEX_EXPR_DIV;
	else
	{
		o = TEX_EXPR_NONE;
		if (p == TEX_NULL)
		{
			if (cur_cmd != TEX_RELAX)
				backInput();
		}
		else if (cur_tok != (TEX_OTHER_TOKEN + ')'))
		{
			printErr(tr("Missing ) inserted for expression"));
			help1(tr("I was expecting to see `+', `-', `*', `/', or `)'. Didn't."));
    		backError();
		}
	}
	
	arith_error = b;
	if ((l == TEX_INT_VAL) || (s > TEX_EXPR_SUB))
	{
		if ((f > TEX_INFINITY) || (f < -TEX_INFINITY))
			numError(f);
	}
	else if (l == TEX_DIMEN_VAL)
	{
		if (qAbs(f) > TEX_MAX_DIMEN)
			numError(f);
	}
	else
	{
		if ((qAbs(width(f)) > TEX_MAX_DIMEN) || 
			(qAbs(stretch(f)) > TEX_MAX_DIMEN) || 
			(qAbs(shrink(f)) > TEX_MAX_DIMEN))
		{
			glueError(f);
		}
	}
	
	switch (s)
	{
		case TEX_EXPR_NONE:
			if ((l >= TEX_GLUE_VAL) && (o != TEX_EXPR_NONE))
			{
				t = newSpec(f); 
				deleteGlueRef(f); 
				if (stretch(t) == 0)
					stretch_order(t) = (quint16)TEX_NORMAL;
					
				if (shrink(t) == 0)
					shrink_order(t) = (quint16)TEX_NORMAL;
			}
			else
				t = f;
			break;
			
		case TEX_EXPR_MULT:
			if (o == TEX_EXPR_DIV)
			{
				n = f; 
				o = TEX_EXPR_SCALE;
			}
			else if (l == TEX_INT_VAL)
				t = multIntegers(t, f);
			else if (l == TEX_DIMEN_VAL)
				t = nxPlusY(t, f, 0);
			else
			{
				t = nxPlusY(width(t), f, 0);
				t = nxPlusY(stretch(t), f, 0);
				t = nxPlusY(shrink(t), f, 0);
			}
			break;
			
		case TEX_EXPR_DIV:
			if (l < TEX_GLUE_VAL)
				t = quotient(t, f);
			else
			{
				t = quotient(width(t), f); 
				t = quotient(stretch(t), f); 
				t = quotient(shrink(t), f);
			}
			break;
			
		case TEX_EXPR_SCALE:
			if (l == TEX_INT_VAL)
				t = fract(t, n, f, TEX_INFINITY);
			else if (l == TEX_DIMEN_VAL)
				t = fract(t, n, f, TEX_MAX_DIMEN);
			else
			{
				t = fract(width(t), n, f, TEX_MAX_DIMEN); 
				t = fract(stretch(t), n, f, TEX_MAX_DIMEN); 
				t = fract(shrink(t), n, f, TEX_MAX_DIMEN);
			}
			break;
	}
	
	if (o > TEX_EXPR_SUB)
		s = o;
	else
	{
		s = TEX_EXPR_NONE;
		if (r == TEX_EXPR_NONE)
			e = t;
		else if (l == TEX_INT_VAL)
			e = addOrSub(e, t, TEX_INFINITY, r == TEX_EXPR_SUB);
		else if (l == TEX_DIMEN_VAL)
			e = addOrSub(e, t, TEX_MAX_DIMEN, r == TEX_EXPR_SUB);
		else
		{
			width(e) = addOrSub(width(e),width(t), TEX_MAX_DIMEN, r == TEX_EXPR_SUB);
			if (stretch_order(e) == stretch_order(t))
				stretch(e) = addOrSub(stretch(e), stretch(t), TEX_MAX_DIMEN, r == TEX_EXPR_SUB);
			else if ((stretch_order(e) < stretch_order(t)) && (stretch(t) != 0))
			{
				stretch(e) = stretch(t); 
				stretch_order(e) = stretch_order(t);
			}
			
			if(shrink_order(e) == shrink_order(t))
				shrink(e) = addOrSub(shrink(e),shrink(t), TEX_MAX_DIMEN, r == TEX_EXPR_SUB);
			else if ((shrink_order(e) < shrink_order(t)) && (shrink(t) != 0))
			{
				shrink(e) = shrink(t); 
				shrink_order(e) = shrink_order(t);
			}
			
			deleteGlueRef(t); 
			if (stretch(e) == 0)
				stretch_order(e) = (quint16)TEX_NORMAL;
				
			if (shrink(e) == 0)
				shrink_order(e) = (quint16)TEX_NORMAL;
		}
		
		r = o;
	}
	
	b = arith_error;
	if (o != TEX_EXPR_NONE)
		goto tcontinue;
		
	if (p != TEX_NULL)
	{
		f = e; 
		q = p;
		e = expr_e_field(q); 
		t = expr_t_field(q); 
		n = expr_n_field(q);
		s = subtype(q) / 4; 
		r = subtype(q) % 4;
		l = type(q); 
		p = link(q); 
		freeNode(q, TEX_EXPR_NODE_SIZE);
		goto found;
	}
	
	if (b)
	{
		printErr(tr("Arithmetic overflow"));
		help2(tr("I can't evaluate this expression,"));
		help_line[0] = tr("since the result is out of range.");
		error();
		if (l >= TEX_GLUE_VAL)
		{
			deleteGlueRef(e); 
			e = zero_glue; 
			add_glue_ref(e);
		}
		else
			e = 0;
	}
	
	arith_error = a; 
	cur_val = e; 
	cur_val_level = l;
}

void XWTeX::scanFifteenBitInt()
{
	scanInt();
	if ((cur_val < 0) || (cur_val > 32767))
	{
		printErr(tr("Bad number"));
		help2(tr("A mathchar number must be between 0 and 32767."));
		help_line[0] = tr("I changed this one to zero.");
		intError(cur_val); 
		cur_val = 0;
	}
	
	cur_val = ((cur_val / 0x1000) * 0x1000000) +
              (((cur_val % 0x1000) / 0x100) * 0x10000) +
              (cur_val % 0x100);
}

void XWTeX::scanFiftyOneBitInt()
{
	scanInt();
	if ((cur_val < 0) || (cur_val > 0x7FFFFFF))
	{
		printErr(tr("Bad delimiter code"));
		help2(tr("A numeric delimiter (first part) must be between 0 and 0x7FFFFFF."));
		help_line[0] = tr("I changed this one to zero.");
		intError(cur_val); 
		cur_val = 0;
	}
	
	qint32 iiii = cur_val;
	scanInt();
	if ((cur_val < 0) || (cur_val > 0xFFFFFF))
	{
		printErr(tr("Bad delimiter code"));
		help2(tr("A numeric delimiter (second part) must be between 0 and 0xFFFFFF."));
		help_line[0] = tr("I changed this one to zero.");
		intError(cur_val); 
		cur_val = 0;
	}
	
	cur_val1 = cur_val;
	cur_val  = iiii;
}

void XWTeX::scanFileName()
{
	name_in_progress = true; 
	beginName();
	
	do
	{
		getXToken();
	} while (cur_cmd == TEX_SPACER);
	
	while (true)
	{
		if ((cur_cmd > TEX_OTHER_CHAR) || (cur_chr > TEX_BIGGEST_CHAR))
		{
			backInput(); 
			goto done;
		}
		
		if (!moreName(cur_chr))
			goto done;
			
		getXToken();
	}
	
done: 
	endName(); 
	name_in_progress = false;
}

void XWTeX::scanFileReferrent()
{
	if (scanKeyWord(TeXCurrentFile))
		cur_val = TEX_MAX_HALFWORD;
	else
		scanInt();
}

void XWTeX::scanFontIdent()
{
	do
	{
		getXToken();
	} while(cur_cmd == TEX_SPACER);
	
	qint32 f, m;
	if (cur_cmd == TEX_DEF_FONT)
		f = curFont();
	else if (cur_cmd == TEX_SET_FONT)
		f = cur_chr;
	else if (cur_cmd == TEX_DEF_FAMILY)
	{
		m = cur_chr; 
		scanBigFourBitInt(); 
		f = equiv(m + cur_val);
	}
	else
	{
		printErr(tr("Missing font identifier"));
		help2(tr("I was looking for a control sequence whose"));
		help_line[0] = tr("current meaning has been defined by \\font.");
		backError(); 
		f = TEX_NULL_FONT;
	}
	cur_val = f;
}

void XWTeX::scanFourBitInt()
{
	scanInt();
	if ((cur_val < 0) || (cur_val > 15))
	{
		printErr(tr("Bad number"));
		help2(tr("Since I expected to read a number between 0 and 15,"));
		help_line[0] = tr("I changed this one to zero.");
		intError(cur_val); 
		cur_val = 0;
	}
}

void XWTeX::scanFourBitIntOr18()
{
	scanInt();
	if ((cur_val < 0) || ((cur_val > 15) && (cur_val != 18)))
	{
		printErr(tr("Bad number"));
		help2(tr("Since I expected to read a number between 0 and 15,"));
		help_line[0] = tr("I changed this one to zero.");
		intError(cur_val); 
		cur_val = 0;
	}
}

void XWTeX::scanGeneralText()
{
	qint32 s, w, d, p, q, unbalance;
	s = scanner_status; 
	w = warning_index; 
	d = def_ref;
	scanner_status = TEX_ABSORBING; 
	warning_index = cur_cs;
	def_ref = getAvail(); 
	token_ref_count(def_ref) = TEX_NULL; 
	p = def_ref;
	scanLeftBrace();
	unbalance = 1;
	while (true)
	{
		getToken();
		if (cur_tok < TEX_RIGHT_BRACE_LIMIT)
		{
			if (cur_cmd < TEX_RIGHT_BRACE)
				unbalance++;
			else
			{
				unbalance--;
				if (unbalance == 0)
					goto found;
			}
		}
		
		q = getAvail(); 
		link(p) = q; 
		info(q) = cur_tok;
  		p = q;
	}
	
found: 
	q = link(def_ref); 
	freeAvail(def_ref);
	if (q == TEX_NULL)
		cur_val = temp_head;
	else
		cur_val = p;
		
	link(temp_head) = q;
	scanner_status  = s; 
	warning_index   = w; 
	def_ref = d;
}

void XWTeX::scanGlue(char level)
{
	bool mu = level == TEX_MU_VAL;
	bool negative = false;
	do
	{
		do
		{
			getXToken();
		} while (cur_cmd == TEX_SPACER);
		
		if (cur_tok == (TEX_OTHER_TOKEN + '-'))
		{
			negative = !negative; 
			cur_tok  = TEX_OTHER_TOKEN + '+';
		}
	} while (cur_tok == (TEX_OTHER_TOKEN + '+'));
	
	if ((cur_cmd >= TEX_MIN_INTERNAL) && (cur_cmd <= TEX_MAX_INTERNAL))
	{
		scanSomethingInternal(level, negative);
		if (cur_val_level >= TEX_GLUE_VAL)
		{
			if (cur_val_level != level)
				muError();
				
			return ;
		}
		
		if (cur_val_level == TEX_INT_VAL)
			scanDimen(mu, false, true);
		else if (level == TEX_MU_VAL)
			muError();
	}
	else
	{
		backInput(); 
		scanDimen(mu, false, false);
		if (negative)
			cur_val = -cur_val;
	}
	
	qint32 q = newSpec(zero_glue); 
	width(q) = cur_val;
	if (scanKeyWord(TeXPlus))
	{
		scanDimen(mu, true, false);
  		stretch(q) = cur_val; 
  		stretch_order(q) = (quint16)cur_order;
	}
	
	if (scanKeyWord(TeXMinus))
	{
		scanDimen(mu, true, false);
  		shrink(q) = cur_val; 
  		shrink_order(q) = (quint16)cur_order;
	}
	
	cur_val = q;
}

void XWTeX::scanInt()
{
	bool negative, vacuous, OK_so_far;
	qint32 m; 
	qint16 d;
	
	radix = 0; 
	OK_so_far = true;
	negative = false;
	do
	{
		do
		{
			getXToken();
		} while (cur_cmd == TEX_SPACER);
		
		if (cur_tok == (TEX_OTHER_TOKEN + '-'))
		{
			negative = !negative; 
			cur_tok  = TEX_OTHER_TOKEN + '+';
		}
	} while (cur_tok == (TEX_OTHER_TOKEN + '+'));
	
	if (cur_tok == TEX_ALPHA_TOKEN)
	{
		getToken();
		if (cur_tok < TEX_CS_TOKEN_FLAG)
		{
			cur_val = cur_chr;
			if (cur_cmd <= TEX_RIGHT_BRACE)
			{
				if (cur_cmd == TEX_RIGHT_BRACE)
					align_state++;
				else
					align_state--;
			}
		}
		else if (cur_tok < (TEX_CS_TOKEN_FLAG + TEX_SINGLE_BASE))
			cur_val = cur_tok - TEX_CS_TOKEN_FLAG - TEX_ACTIVE_BASE;
		else
			cur_val = cur_tok - TEX_CS_TOKEN_FLAG - TEX_SINGLE_BASE;
			
		if (cur_val > TEX_BIGGEST_CHAR)
		{
			printErr(tr("Improper alphabetic constant"));
			help2(tr("A one-character control sequence belongs after a ` mark."));
			help_line[0] = tr("So I'm essentially inserting \\0 here.");
			cur_val = '0'; 
			backError();
		}
		else
		{
			getXToken();
			if (cur_cmd != TEX_SPACER)
				backInput();
		}
	}
	else if ((cur_cmd >= TEX_MIN_INTERNAL) && (cur_cmd <= TEX_MAX_INTERNAL))
		scanSomethingInternal(TEX_INT_VAL, false);
	else
	{
		radix = 10; 
		m = 214748364;
		if (cur_tok == TEX_OCTAL_TOKEN)
		{
			radix = 8; 
			m = 0x10000000; 
			getXToken();
		}
		else if (cur_tok == TEX_HEX_TOKEN)
		{
			radix = 16; 
			m = 0x8000000; 
			getXToken();
		}
		
		vacuous = true; 
		cur_val = 0;
		
		while (true)
		{
			if ((cur_tok < (TEX_ZERO_TOKEN + radix)) && 
				(cur_tok >= TEX_ZERO_TOKEN) && 
				(cur_tok <= (TEX_ZERO_TOKEN + 9)))
			{
				d = cur_tok - TEX_ZERO_TOKEN;
			}
			else if (radix == 16)
			{
				if ((cur_tok <= (TEX_A_TOKEN + 5)) && (cur_tok >= TEX_A_TOKEN))
					d = cur_tok - TEX_A_TOKEN + 10;
				else if ((cur_tok <= (TEX_OTHER_A_TOKEN + 5)) && (cur_tok >= TEX_OTHER_A_TOKEN))
					d = cur_tok - TEX_OTHER_A_TOKEN + 10;
				else
					goto done;
			}
			else
				goto done;
				
			vacuous = false;
			
			if ((cur_val >= m) && ((cur_val > m) || (d > 7) || (radix != 10)))
			{
				if (OK_so_far)
				{
					printErr(tr("Number too big"));
					help2(tr("I can only go up to 2147483647='17777777777=\"7FFFFFFF,"));
					help_line[0] = tr("so I'm using that number instead of yours.");
					error(); 
					cur_val = TEX_INFINITY; 
					OK_so_far = false;
				}
			}
			else
				cur_val = cur_val * radix + d;
				
			getXToken();
		}
		
done:
		if (vacuous)
		{
			printErr(tr("Missing number, treated as zero"));
			help3(tr("A number should have been here; I inserted `0'."));
			help_line[1] = tr("(If you can't figure out why I needed to see a number,");
			help_line[0] = tr("look up `weird error' in the index to The TeXbook.)");
			backError();			
		}
		else if (cur_cmd != TEX_SPACER)
			backInput();
	}
	
	if (negative)
		cur_val = -cur_val;
}

bool XWTeX::scanKeyWord(qint32 s)
{
	qint32 q = 0;
	qint32 p = backup_head; 
	link(p)  = TEX_NULL;
	if (s < TEX_TOO_BIG_CHAR)
	{
		while (true)
		{
			getXToken();
			if ((cur_cs == 0) && ((cur_chr == s) || (cur_chr == (s - 'a' + 'A'))))
			{
				q = getAvail(); 
				link(p) = q; 
				info(q) = cur_tok;
  				p = q;
  				flushList(link(backup_head));
  				return true;
			}
			else if ((cur_cmd != TEX_SPACER) || (p != backup_head))
			{
				backInput();
				if (p != backup_head)
					backList(link(backup_head));
				
				return false;
			}
		}
	}
	
	qint32 k = str_start(s);
	while (k < str_start(s + 1))
	{
		getXToken();
		if ((cur_cs == 0) && 
			((cur_chr == str_pool[k]) || (cur_chr == (str_pool[k] - 'a'+ 'A'))))
		{
			q = getAvail(); 
			link(p) = q; 
			info(q) = cur_tok;
  			p = q;
  			k++;
		}
		else if ((cur_cmd != TEX_SPACER) || (p != backup_head))
		{
			backInput();
			if (p != backup_head)
				backList(link(backup_head));
			
			return false;
		}
	}
	
	flushList(link(backup_head));
	return true;
}

void XWTeX::scanLeftBrace()
{
	do
	{
		getXToken();
	} while ((cur_cmd == TEX_SPACER) || (cur_cmd == TEX_RELAX));
	
	if (cur_cmd != TEX_LEFT_BRACE)
	{
		printErr(tr("Missing { inserted"));
		help4(tr("A left brace was mandatory here, so I've put one in."));
		help_line[2] = tr("You might want to delete and/or insert some corrections");
		help_line[1] = tr("so that I will find a matching right brace soon.");
		help_line[0] = tr("(If you're confused by all this, try typing `I}' now.)");
		backError(); 
		cur_tok = TEX_LEFT_BRACE_TOKEN + '}'; 
		cur_cmd = TEX_LEFT_BRACE;
  		cur_chr = '}'; 
  		align_state++;
	}
}

void XWTeX::scanMath(qint32 p)
{
	qint32 c;
	
restart:
	do
	{
		getXToken();
	} while ((cur_cmd == TEX_SPACER) || (cur_cmd == TEX_RELAX));
	
reswitch:
	switch (cur_cmd)
	{
		case TEX_LETTER:
		case TEX_OTHER_CHAR:
		case TEX_CHAR_GIVEN:
			c = mathCode(cur_chr);
			if (c == 0x8000000)
			{
				cur_cs = cur_chr + TEX_ACTIVE_BASE;
				cur_cmd = eqType(cur_cs); 
				cur_chr = equiv(cur_cs);
				xToken(); 
				backInput();
				goto restart;
			}
			break;
			
		case TEX_CHAR_NUM:
			scanCharNum(); 
			cur_chr = cur_val; 
			cur_cmd = TEX_CHAR_GIVEN;
  			goto reswitch;
			break;
			
		case TEX_MATH_CHAR_NUM:
			if (cur_chr == 0)
				scanFifteenBitInt();
			else
				scanBigFifteenBitInt();
			c = cur_val;
			break;
			
		case TEX_MATH_GIVEN:
			c = ((cur_chr / 0x1000) * 0x1000000) + (((cur_chr % 0x1000) / 0x100) * 0x10000) + (cur_chr % 0x100);
			break;
			
		case TEX_OMATH_GIVEN:
			c = cur_chr;
			break;
			
		case TEX_DELIM_NUM:
			if (cur_chr == 0)
				scanTwentySevenBitInt();
			else
				scanFiftyOneBitInt();
  			c = cur_val;
			break;
			
		default:
			backInput(); 
			scanLeftBrace();
			saved(0) = p; 
			save_ptr++; 
			pushMath(TEX_MATH_GROUP); 
			return;
			break;
	}
	
	math_type(p) = TEX_MATH_CHAR; 
	character(p) = (quint16)(c % 0x10000);
	if ((c >= TEX_VAR_CODE) && ((curFam() >= 0) && (curFam() < 16)))
		fam(p) = (quint16)curFam();
	else
		fam(p) = (quint16)((c / 0x10000) % 0x100);
}

void XWTeX::scanMode()
{
	if (scanKeyWord(TeXOneByte))
		cur_val = TEX_ONEBYTE_MODE;
	else if (scanKeyWord(TeXEBCDIC))
		cur_val = TEX_EBCDIC_MODE;
	else if (scanKeyWord(TeXTwoByte))
		cur_val = TEX_TWOBYTE_MODE;
	else if (scanKeyWord(TeXTwoByteLE))
		cur_val = TEX_TWOBYTELE_MODE;
	else
	{
		printErr(tr("Invalid input mode")); 
		cur_val = 0;
	}
}

void XWTeX::scanOcpIdent()
{
	do
	{
		getXToken();
	} while (cur_cmd == TEX_SPACER);
	
	qint32 f;
	if (cur_cmd == TEX_SET_OCP)
		f = cur_chr;
	else
	{
		printErr(tr("Missing ocp identifier"));
		help2(tr("I was looking for a control sequence whose"));
  		help_line[0] = tr("current meaning has been defined by \\ocp.");
  		backError(); 
  		f = TEX_NULL_OCP;
	}
	
	cur_val = f;
}

qint32 XWTeX::scanOcpList()
{
	qint32 llstack_no, lop, lstack_entry, other_list, ocp_ident, result;
	getRToken();
	if (cur_cmd == TEX_SET_OCP_LIST)
		result = copyOcpList(ocp_list_list[cur_chr]);
	else if (cur_cmd != TEX_OCP_LIST_OP)
	{
		printErr(tr("Bad ocp list specification"));
		help1(tr("I was looking for a ocp list specification."));
  		result = makeNullOcpList();
	}
	else
	{
		lop = cur_chr;
		scanScaled(); 
		llstack_no = cur_val;
		if ((llstack_no <= 0) || (llstack_no >= TEX_OCP_MAXINT))
		{
			printErr(tr("Stack numbers must be between 0 and 4096 (exclusive)"));
    		result = makeNullOcpList();
		}
		else
		{
			if (lop <= TEX_ADD_AFTER_OP)
			{
				scanOcpIdent(); 
				ocp_ident = cur_val;
			}
			
			other_list = scanOcpList();
			lstack_entry = ocpEnsureLStack(other_list, llstack_no);
			if (lop <= TEX_ADD_AFTER_OP)
				ocpApplyAdd(lstack_entry, (lop == TEX_ADD_BEFORE_OP), ocp_ident);
			else
				ocpApplyRemove(lstack_entry, (lop == TEX_REMOVE_BEFORE_OP));
				
			result = other_list;
		}
	}
	
	return result;
}

void XWTeX::scanOcpListIdent()
{
	do
	{
		getXToken();
	} while (cur_cmd == TEX_SPACER);
	
	qint32 f;
	if (cur_cmd == TEX_SET_OCP_LIST)
		f = cur_chr;
	else
	{
		printErr(tr("Missing ocp list identifier"));
		help2(tr("I was looking for a control sequence whose"));
  		help_line[0] = tr("current meaning has been defined by \\ocplist.");
  		backError(); 
  		f = TEX_NULL_OCP_LIST;
	}
	
	cur_val = f;
}

void XWTeX::scanOptionalEquals()
{
	do
	{
		getXToken();
	} while (cur_cmd == TEX_SPACER);
	
	if (cur_tok != TEX_OTHER_TOKEN + '=')
		backInput();
}

void  XWTeX::scanRealFifteenBitInt()
{
	scanInt();
	if ((cur_val < 0) || (cur_val > 32767))
	{
		printErr(tr("Bad mathchar"));
		help2(tr("A mathchar number must be between 0 and 32767."));
		help_line[0] = tr("I changed this one to zero.");
		intError(cur_val); 
		cur_val = 0;
	}
}

void XWTeX::scanRegisterNum()
{
	scanInt();
	if ((cur_val < 0) || (cur_val > max_reg_num))
	{
		printErr(tr("Bad register code"));
		help2(max_reg_help_line);
		help_line[0] = tr("I changed this one to zero.");
		intError(cur_val); 
		cur_val = 0;
	}
}

void XWTeX::scanRightBrace()
{
	do
	{
		getXToken();
	} while ((cur_cmd == TEX_SPACER) || (cur_cmd == TEX_RELAX));
	
	if (cur_cmd != TEX_RIGHT_BRACE)
	{
		printErr(tr("Missing } inserted"));
		help4(tr("A right brace was mandatory here, so I've put one in."));
		help_line[2] = tr("You might want to delete and/or insert some corrections");
		help_line[1] = tr("so that I will find a matching right brace soon.");
		help_line[0] = tr("If you're confused by all this, try typing `I}' now.)");
		backError(); 
		cur_tok = TEX_RIGHT_BRACE_TOKEN + '}'; 
		cur_cmd = TEX_RIGHT_BRACE;
  		cur_chr = '}'; 
  		align_state++;
	}
}

qint32 XWTeX::scanRuleSpec()
{
	qint32 q = newRule();
	if (cur_cmd == TEX_VRULE)
	{
		width(q) = TEX_DEFAULT_RULE;
  		rule_dir(q) = bodyDirection();
	}
	else
	{
		height(q) = TEX_DEFAULT_RULE; 
		depth(q) = 0;
  		rule_dir(q) = textDirection();
	}
	
reswitch:
	if (scanKeyWord(TeXWidth))
	{
		scanNormalDimen(); 
		width(q) = cur_val; 
		goto reswitch;
	}
	
	if (scanKeyWord(TeXHeight))
	{
		scanNormalDimen(); 
		height(q) = cur_val; 
		goto reswitch;
	}
	
	if (scanKeyWord(TeXDepth))
	{
		scanNormalDimen(); 
		depth(q) = cur_val; 
		goto reswitch;
	}
	
	return q;
}

void XWTeX::scanScaled()
{
	qint32 f = 0; 
	arith_error = false; 
	bool negative = false;
	do
	{
		do
		{
			getXToken();
		} while (cur_cmd == TEX_SPACER);
		
		if (cur_tok == (TEX_OTHER_TOKEN + '-'))
		{
			negative = !negative; 
			cur_tok  = TEX_OTHER_TOKEN + '+';
		}
	} while (cur_tok == (TEX_OTHER_TOKEN + '+'));
	
	backInput();
	if (cur_tok == TEX_CONTINENTAL_POINT_TOKEN)
		cur_tok = TEX_POINT_TOKEN;
		
	if (cur_tok != TEX_POINT_TOKEN)
		scanInt();
	else
	{
		radix   = 10; 
		cur_val = 0;
	}
	
	if (cur_tok == TEX_CONTINENTAL_POINT_TOKEN)
		cur_tok = TEX_POINT_TOKEN;
		
	if ((radix == 10) && (cur_tok == TEX_POINT_TOKEN))
	{
		char k = 0; 
		qint32 p = TEX_NULL; 
		getToken();
		while (true)
		{
			getXToken();
			if ((cur_tok > (TEX_ZERO_TOKEN + 9)) || (cur_tok < TEX_ZERO_TOKEN))
				goto done1;
				
			if (k < 17)
			{
				qint32 q = getAvail(); 
				link(q) = p; 
				info(q) = cur_tok - TEX_ZERO_TOKEN;
    			p = q; 
    			k++;
			}
		}
done1: 
		for (char kk = k; kk > 0; kk--)
		{
			dig[kk - 1] = info(p); 
			qint32 q = p; 
			p = link(p); 
			freeAvail(q);
		}
		
		f = roundDecimals(k);
		if (cur_cmd != TEX_SPACER)
			backInput();
	}
	
	if (cur_val < 0)
	{
		negative = !negative; 
		cur_val = -cur_val;
	}
	
	if (cur_val > 0x4000)
		arith_error = true;
	else
		cur_val = cur_val * TEX_UNITY + f;
		
	if (arith_error || (qAbs(cur_val) >= 0x40000000))
		printErr(tr("Stack number too large"));
		
	if (negative)
		cur_val = -cur_val;
}

void XWTeX::scanSpec(qint32 c, bool three_codes)
{
	qint32 s;
	char spec_code;
	
	if (three_codes)
		s = saved(0);
		
	if ((c != TEX_ALIGN_GROUP) && (c != TEX_VCENTER_GROUP))
	{
		if (scanKeyWord(TeXDir))
		{
			scanDir(); 
			spec_direction = cur_val;
		}
	}
	
	if (scanKeyWord(TeXTo))
		spec_code = TEX_EXACTLY;
	else if (scanKeyWord(TeXSpread))
		spec_code = TEX_ADDITIONAL;
	else
	{
		spec_code = TEX_ADDITIONAL; 
		cur_val = 0;
  		goto found;
	}
	
	scanNormalDimen();
	
found:
	if (three_codes)
	{
		saved(0) = s; 
		save_ptr++;
	}
	
	saved(0) = spec_code; 
	saved(1) = cur_val;
	
	if ((c != TEX_ALIGN_GROUP) && (c != TEX_VCENTER_GROUP))
	{
		saved(2) = spec_direction;
		if (spec_direction != -1)
		{
			saved(3) = text_dir_ptr;
  			text_dir_ptr = newDir(spec_direction);
		}
		else
			saved(3) = TEX_NULL;
			
		save_ptr = save_ptr + 4;
  		newSaveLevel(c); 
  		scanLeftBrace();
  		eqWordDefine(TEX_DIR_BASE + TEX_BODY_DIRECTION_CODE, spec_direction);
  		eqWordDefine(TEX_DIR_BASE + TEX_PAR_DIRECTION_CODE,  spec_direction);
  		eqWordDefine(TEX_DIR_BASE + TEX_TEXT_DIRECTION_CODE, spec_direction);
  		eqWordDefine(TEX_INT_BASE + TEX_LEVEL_LOCAL_DIR_CODE, cur_level);
	}
	else
	{
		save_ptr = save_ptr + 2;
  		newSaveLevel(c); 
  		scanLeftBrace();
	}
	
	spec_direction = -1;
}

void XWTeX::scanSomethingInternal(char level, bool negative)
{
	qint32 m, q, p;
	TeXFourQuarters i;
	
	m = cur_chr;
	switch (cur_cmd)
	{
		case TEX_DEF_CODE:
			scanCharNum();
			if (m == TEX_MATH_CODE_BASE)
			{
				cur_val1 = mathCode(cur_val);
				if (((cur_val1 / 0x1000000)>8) || 
					(((cur_val1 % 0x1000000) / 0x10000)>15) || 
					((cur_val1 % 0x10000)>255))
				{
					printErr(tr("Extended mathchar used as mathchar"));
					help2(tr("A mathchar number must be between 0 and 0x7FFF."));
					help_line[0] = tr("I changed this one to zero.");
					intError(cur_val1); 
					cur_val1 = 0;
				}
				
				cur_val1 = ((cur_val1 / 0x1000000) * 0x1000) +
            				(((cur_val1 % 0x1000000) / 0x10000) * 0x100) + (cur_val1 % 0x10000);
  				scannedResult(cur_val1, TEX_INT_VAL);
			}
			else if (m == (TEX_MATH_CODE_BASE + 256))
				scannedResult(mathCode(cur_val), TEX_INT_VAL);
			else if (m < TEX_MATH_CODE_BASE)
				scannedResult(equiv(m + cur_val), TEX_INT_VAL);
			else
				scannedResult(newEqtbInt(m + cur_val), TEX_INT_VAL);
			break;
			
		case TEX_TOKS_REGISTER:
		case TEX_ASSIGN_TOKS:
		case TEX_DEF_FAMILY:
		case TEX_SET_FONT:
		case TEX_DEF_FONT:
			if (level != TEX_TOK_VAL)
			{
				printErr(tr("Missing number, treated as zero"));
				help3(tr("A number should have been here; I inserted `0'."));
				help_line[1] = tr("(If you can't figure out why I needed to see a number,");
				help_line[0] = tr("look up `weird error' in the index to The TeXbook.)");
				backError(); 
				scannedResult(0, TEX_DIMEN_VAL);
			}
			else if (cur_cmd <= TEX_ASSIGN_TOKS)
			{
				if (cur_cmd < TEX_ASSIGN_TOKS)
				{
					scanEightBitInt(); 
					m = TEX_TOKS_BASE + cur_val;
				}
				
				scannedResult(equiv(m), TEX_TOK_VAL);
			}
			else
			{
				backInput(); 
				scanFontIdent();
  				scannedResult(TEX_FONT_ID_BASE + cur_val, TEX_IDENT_VAL);
			}
			break;
			
		case TEX_ASSIGN_INT: 
			scannedResult(newEqtbInt(m), TEX_INT_VAL);
			break;
			
		case TEX_ASSIGN_BOX_DIR:
			scanEightBitInt();
  			m = cur_val;
  			if (box(m) != TEX_NULL) 
  				cur_val = box_dir(box(m));
  			else 
  				cur_val = 0;
  			cur_val_level = TEX_DIR_VAL;
			break;
			
		case TEX_ASSIGN_DIR: 
			scannedResult(newEqtbInt(m), TEX_DIR_VAL);
			break;
			
		case TEX_ASSIGN_DIMEN: 
			scannedResult(newEqtbSC(m), TEX_DIMEN_VAL);
			break;
			
		case TEX_ASSIGN_GLUE: 
			scannedResult(equiv(m), TEX_GLUE_VAL);
			break;
			
		case TEX_ASSIGN_MU_GLUE: 
			scannedResult(equiv(m), TEX_MU_VAL);
			break;
			
		case TEX_SET_AUX:
			if (qAbs(mode) != m)
			{
				printErr(tr("Improper ")); 
				printCmdChr((quint16)TEX_SET_AUX, m);
				help4(tr("You can refer to \\spacefactor only in horizontal mode;"));
				help_line[2] = tr("you can refer to \\prevdepth only in vertical mode; and");
				help_line[1] = tr("neither of these is meaningful inside \\write. So");
				help_line[0] = tr("I'm forgetting what you said and using zero instead.");
				error();
				if (level != TEX_TOK_VAL)
					scannedResult(0, TEX_DIMEN_VAL);
				else
					scannedResult(0, TEX_INT_VAL);
			}
			else if (m == TEX_VMODE)
			{
				cur_val = prev_depth; 
				cur_val_level = TEX_DIMEN_VAL;
			}
			else
			{
				cur_val = space_factor; 
				cur_val_level = TEX_INT_VAL;
			}
			break;
			
		case TEX_SET_PREV_GRAF:
			if (mode == 0)
				scannedResult(0, TEX_INT_VAL);
			else
			{
				nest[nest_ptr] = cur_list; 
				p = nest_ptr;
				while (qAbs(nest[p].mode_field) != TEX_VMODE)
					p--;
					
				scannedResult(nest[p].pg_field, TEX_INT_VAL);
			}
			break;
			
		case TEX_SET_PAGE_INT:
			if (m == 0)
				cur_val = dead_cycles;
			else if (m == 2)
				cur_val = interaction;
			else
				cur_val = insert_penalties;
			cur_val_level = TEX_INT_VAL;
			break;
			
		case TEX_SET_PAGE_DIMEN:
			if ((page_contents == TEX_EMPTY) && (!output_active))
			{
				if (m == 0)
					cur_val = TEX_MAX_DIMEN;
				else
					cur_val = 0;
			}
			else
				cur_val = page_so_far[m];
			cur_val_level = TEX_DIMEN_VAL;
			break;
			
		case TEX_SET_SHAPE:
			if (m > TEX_PAR_SHAPE_LOC)
			{
				scanInt();
				if ((equiv(m) == TEX_NULL) || (cur_val < 0))
					cur_val = 0;
				else
				{
					if (cur_val > penalty(equiv(m)))
						cur_val = penalty(equiv(m));
					cur_val = penalty(equiv(m) + cur_val);
				}
			}
			else if (parShapePtr() == TEX_NULL)
				cur_val = 0;
			else
				cur_val = info(parShapePtr());
			cur_val_level = TEX_INT_VAL;
			break;
			
		case TEX_SET_BOX_DIMEN:
			if (m <= TEX_HEIGHT_OFFSET)
			{
				scanRegisterNum();
				if (cur_val < TEX_NUMBER_REGS)
					q = box(cur_val);
				else
				{
					findSaElement(TEX_BOX_VAL, cur_val, false);
					if (cur_ptr == TEX_NULL)
						q = TEX_NULL;
					else
						q = sa_ptr(cur_ptr);
				}
				
				if (q == TEX_NULL)
					cur_val = 0;
				else
					cur_val = mem[q + m].ii.CINT0;
				cur_val_level = TEX_DIMEN_VAL;
			}
			else
				confusion("boxdimen");
			break;
			
		case TEX_CHAR_GIVEN:
		case TEX_MATH_GIVEN:
		case TEX_OMATH_GIVEN: 
			scannedResult(cur_chr, TEX_INT_VAL);
			break;
			
		case TEX_ASSIGN_FONT_DIMEN:
			if (m == TEX_FONT_DIMEN_CODE)
			{
				findFontDimen(false);
				font_info(dimen_font, font_file_size(dimen_font)).ii.CINT0 = 0;
				scannedResult(font_info(dimen_font, cur_val).ii.CINT0, TEX_DIMEN_VAL);
			}
			else
			{
				scanCharNum();
				if (m == TEX_CHAR_WIDTH_CODE)
					cur_val = char_width(main_f, char_info(main_f, cur_val));
				else if (m == TEX_CHAR_HEIGHT_CODE)
					cur_val = char_height(main_f, height_depth(char_info(main_f, cur_val)));
				else if (m == TEX_CHAR_DEPTH_CODE)
					cur_val = char_depth(main_f, height_depth(char_info(main_f, cur_val)));
				else if (m == TEX_CHAR_ITALIC_CODE)
					cur_val = char_italic(main_f, char_info(main_f, cur_val));
				else
					confusion("fontdimen");
				cur_val_level = TEX_DIMEN_VAL;
			}
			break;
			
		case TEX_ASSIGN_FONT_INT:
			scanFontIdent();
			if (m == 0)
				scannedResult(hyphen_char(cur_val), TEX_INT_VAL);
			else
				scannedResult(skew_char(cur_val), TEX_INT_VAL);
			break;
			
		case TEX_REGISTER:
			scanEightBitInt();
			{
				switch (m)
				{
					case TEX_INT_VAL:
						cur_val = count(cur_val);
						break;
						
					case TEX_DIMEN_VAL:
						cur_val = dimen(cur_val);
						break;
						
					case TEX_GLUE_VAL: 
						cur_val = skip(cur_val);
						break;
						
					case TEX_MU_VAL: 
						cur_val = muskip(cur_val);
						break;
						
					default:
						break;
				}
			}
			
			cur_val_level = (char)m;
			break;
			
		case TEX_LAST_ITEM:
			if (m > TEX_LAST_NODE_TYPE_CODE)
			{
				if (m >= TEX_ETEX_GLUE)
				{
					if (m < TEX_ETEX_MU)
					{
						switch (m)
						{
							case TEX_MU_TO_GLUE_CODE: 
								scanMuGlue();
								break;
								
							default:
								break;
						}
						cur_val_level = TEX_GLUE_VAL;
					}
					else if (m < TEX_ETEX_EXPR)
					{
						switch (m)
						{
							case TEX_GLUE_TO_MU_CODE: 
								scanNormalGlue();
								break;
								
							default:
								break;
						}
						
						cur_val_level = TEX_MU_VAL;
					}
					else
					{
						cur_val_level = m - TEX_ETEX_EXPR + TEX_INT_VAL; 
						scanExpr();
					}
					
					while (cur_val_level > level)
					{
						if (cur_val_level == TEX_GLUE_VAL)
						{
							m = cur_val; 
							cur_val = width(m); 
							deleteGlueRef(m);
						}
						else if (cur_val_level == TEX_MU_VAL)
							muError();
							
						cur_val_level--;
					}
					
					if (negative)
					{
						if (cur_val_level >= TEX_GLUE_VAL)
						{
							m = cur_val; 
							cur_val = newSpec(m); 
							deleteGlueRef(m);
							width(cur_val) = -width(cur_val);
							stretch(cur_val) = -stretch(cur_val);
							shrink(cur_val) = -shrink(cur_val);
						}
						else
							cur_val = -cur_val;
					}
					
					return ;
				}
				else if (m >= TEX_ETEX_DIM)
				{
					switch (m)
					{
						case TEX_FONT_CHAR_WD_CODE:
						case TEX_FONT_CHAR_HT_CODE:
						case TEX_FONT_CHAR_DP_CODE:
						case TEX_FONT_CHAR_IC_CODE:
							scanFontIdent(); 
							q = cur_val; 
							scanCharNum();
							if ((font_bc(q) <= cur_val) && (font_ec(q) >= cur_val))
							{
								i = char_info(q, cur_val);
								switch (m)
								{
									case TEX_FONT_CHAR_WD_CODE: 
										cur_val = char_width(q, i);
										break;
										
    								case TEX_FONT_CHAR_HT_CODE: 
    									cur_val = char_height(q, height_depth(i));
    									break;
    									
    								case TEX_FONT_CHAR_DP_CODE: 
    									cur_val = char_depth(q, height_depth(i));
    									break;
    									
    								case TEX_FONT_CHAR_IC_CODE: 
    									cur_val = char_italic(q, i);
    									break;
    									
    								default:
    									break;
								}
							}
							else
								cur_val = 0;
							break;
							
						case TEX_PAR_SHAPE_LENGTH_CODE:
						case TEX_PAR_SHAPE_INDENT_CODE:
						case TEX_PAR_SHAPE_DIMEN_CODE:
							q = cur_chr - TEX_PAR_SHAPE_LENGTH_CODE; 
							scanInt();
							if ((parShapePtr() == TEX_NULL) || (cur_val <= 0))
								cur_val = 0;
							else
							{
								if (q == 2)
								{
									q = cur_val % 2; 
									cur_val = (cur_val + q ) / 2;
								}
								
								if (cur_val > info(parShapePtr()))
									cur_val = info(parShapePtr());
									
								cur_val = mem[parShapePtr() + 2 * cur_val - q].ii.CINT0;
							}
							cur_val_level = TEX_DIMEN_VAL;
							break;
							
						case TEX_GLUE_STRETCH_CODE: 
						case TEX_GLUE_SHRINK_CODE:
							scanNormalGlue(); 
							q = cur_val;
							if (m == TEX_GLUE_STRETCH_CODE)
								cur_val = stretch(q);
							else
								cur_val = shrink(q);
							deleteGlueRef(q);
							break;
							
						default:
							break;
					}
					
					cur_val_level = TEX_DIMEN_VAL;
				}
				else
				{
					switch (m)
					{
						case TEX_INPUT_LINE_NO_CODE: 
							cur_val = line;
							break;
							
  						case TEX_BADNESS_CODE: 
  							cur_val = last_badness;
  							break;
  							
  						case TEX_ALEPH_VERSION_CODE: 
  							cur_val = 0;
  							break;
  							
						case TEX_OMEGA_VERSION_CODE: 
							cur_val = 1;
							break;
							
						case TEX_ETEX_VERSION_CODE: 
							cur_val = 2;
							break;
							
						case TEX_ALEPH_MINOR_VERSION_CODE: 
							cur_val = 0;
							break;
							
						case TEX_OMEGA_MINOR_VERSION_CODE: 
							cur_val = 15;
							break;
							
						case TEX_ETEX_MINOR_VERSION_CODE: 
							cur_val = 1;
							break;
							
						case TEX_CURRENT_GROUP_LEVEL_CODE: 
							cur_val = cur_level - TEX_LEVEL_ONE;
							break;
							
						case TEX_CURRENT_GROUP_TYPE_CODE: 
							cur_val = cur_group;
							break;
							
						case TEX_CURRENT_IF_LEVEL_CODE:
							q = cond_ptr; 
							cur_val = 0;
							while (q != TEX_NULL)
							{
								cur_val++;
								q = link(q);
							}
							break;
							
						case TEX_CURRENT_IF_TYPE_CODE:
							if (cond_ptr == TEX_NULL)
								cur_val = 0;
							else if (cur_if < TEX_UNLESS_CODE)
								cur_val = cur_if + 1;
							else
								cur_val = -(cur_if - TEX_UNLESS_CODE + 1);
							break;
							
						case TEX_CURRENT_IF_BRANCH_CODE:
							if ((if_limit == TEX_OR_CODE) || (if_limit == TEX_ELSE_CODE))
								cur_val = 1;
							else if (if_limit == TEX_FI_CODE)
								cur_val = -1;
							else
								cur_val = 0;
							break;
							
						case TEX_GLUE_STRETCH_ORDER_CODE: 
						case TEX_GLUE_SHRINK_ORDER_CODE:
							scanNormalGlue(); 
							q = cur_val;
							if (m == TEX_GLUE_STRETCH_ORDER_CODE)
								cur_val = stretch_order(q);
							else
								cur_val = shrink_order(q);
							deleteGlueRef(q);
							break;
							
						default:
							break;
					}
					
					cur_val_level = TEX_INT_VAL;
				}
			}
			else
			{
				if (cur_chr == TEX_GLUE_VAL)
					cur_val = zero_glue;
				else
					cur_val = 0;
					
				if (cur_chr == TEX_LAST_NODE_TYPE_CODE)
				{
					cur_val = TEX_INT_VAL;
					if ((tail == head) || (mode == 0))
						cur_val = -1;
				}
				else
					cur_val_level = cur_chr;
					
				if (!isCharNode(tail) && (mode != 0))
				{
					switch (cur_chr)
					{
						case TEX_INT_VAL:
							if (type(tail) == TEX_PENALTY_NODE)
								cur_val = penalty(tail);
							break;
							
						case TEX_DIMEN_VAL:
							if (type(tail) == TEX_KERN_NODE)
								cur_val = width(tail);
							break;
							
						case TEX_GLUE_VAL:
							if (type(tail) == TEX_GLUE_NODE)
							{
								cur_val = glue_ptr(tail);
								if (subtype(tail) == (quint16)TEX_MU_GLUE)
									cur_val_level = TEX_MU_VAL;
							}
							break;
							
						case TEX_LAST_NODE_TYPE_CODE:
							if ((type(tail) != TEX_MATH_NODE))
							{
								if (type(tail) <= TEX_UNSET_NODE)
									cur_val = type(tail) + 1;
								else
									cur_val = TEX_UNSET_NODE + 2;
							}
							break;
							
						default:
							break;
					}
				}
				else if ((mode == TEX_VMODE) && (tail == head))
				{
					switch (cur_chr)
					{
						case TEX_INT_VAL: 
							cur_val = last_penalty;
							break;
							
						case TEX_DIMEN_VAL: 
							cur_val = last_kern;
							break;
							
						case TEX_GLUE_VAL:
							if (last_glue != TEX_MAX_HALFWORD)
								cur_val = last_glue;
							break;
							
						case TEX_LAST_NODE_TYPE_CODE: 
							cur_val = last_node_type;
							break;
							
						default:
							break;
					}
				}
			}
			break;
			
		default:
			printErr(tr("You can't use `")); 
			printCmdChr((quint16)cur_cmd, cur_chr);
			print(tr("' after ")); 
			printEsc(TeXThe);
			help1(tr("I'm forgetting what you said and using zero instead."));
			error();
			if (level != TEX_TOK_VAL)
				scannedResult(0, TEX_DIMEN_VAL);
			else
				scannedResult(0, TEX_INT_VAL);
			break;
	}
	
	while (cur_val_level > level)
	{
		if (cur_val_level == TEX_GLUE_VAL)
			cur_val = width(cur_val);
		else if (cur_val_level == TEX_MU_VAL)
			muError();
			
		cur_val_level--;
	}
	
	if (negative)
	{
		if (cur_val_level >= TEX_GLUE_VAL)
		{
			cur_val = newSpec(cur_val);
			width(cur_val)   = -width(cur_val);
			stretch(cur_val) = -stretch(cur_val);
			shrink(cur_val)  = -shrink(cur_val);
		}
		else
			cur_val = -cur_val;
	}
	else if ((cur_val_level >= TEX_GLUE_VAL) && (cur_val_level <= TEX_MU_VAL))
		add_glue_ref(cur_val);
}

void XWTeX::scanStringArgument()
{
	scanLeftBrace();
	getXToken();
	while ((cur_cmd != TEX_RIGHT_BRACE))
	{
		if ((cur_cmd == TEX_LETTER) || (cur_cmd == TEX_OTHER_CHAR))
		{
			strRoom(1); 
			appendChar(cur_chr);
		}
		else if ((cur_cmd == TEX_SPACER))
		{
			strRoom(1); 
			appendChar(' ');
		}
		else
			print(tr("Bad token appearing in string argument"));
			
		getXToken();
	}
	
	qint32 s = makeString();
	if (strEqStr(TeXMI, s))
		s = TeXMI;
	else if (strEqStr(TeXMO, s))
		s = TeXMO;
	else if (strEqStr(TeXMN, s))
		s = TeXMN;
		
	cur_val = s;
}

qint32 XWTeX::scanToks(bool macro_def, bool xpand)
{
	qint32 t, s, p, q, unbalance, hash_brace;
	if (macro_def)
		scanner_status = TEX_DEFINING;
	else
		scanner_status = TEX_ABSORBING;
		
	warning_index = cur_cs; 
	def_ref = getAvail(); 
	token_ref_count(def_ref) = TEX_NULL;
	p = def_ref; 
	hash_brace = 0; 
	t = TEX_ZERO_TOKEN;
	if (macro_def)
	{
		while (true)
		{
			getToken();
			if (cur_tok < TEX_RIGHT_BRACE_LIMIT)
				goto done1;
				
			if (cur_cmd == TEX_MAC_PARAM)
			{
				s = TEX_MATCH_TOKEN + cur_chr; 
				getToken();
				if (cur_cmd == TEX_LEFT_BRACE)
				{
					hash_brace = cur_tok;
  					q = getAvail(); 
  					link(p) = q; 
  					info(q) = cur_tok;
  					p = q;
  					q = getAvail(); 
  					link(p) = q; 
  					info(q) = TEX_END_MATCH_TOKEN;
  					p = q;
  					goto done;
				}
				
				if (t == (TEX_ZERO_TOKEN + 9))
				{
					printErr(tr("You already have nine parameters"));
					help1(tr("I'm going to ignore the # sign you just used.")); 
					error();
				}
				else
				{
					t++;
					if (cur_tok != t)
					{
						printErr(tr("Parameters must be numbered consecutively"));
						help2(tr("I've inserted the digit you should have used after the #."));
						help_line[0] = tr("Type `1' to delete what you did use.");
						backError();
					}
					
					cur_tok = s;
				}
			}
			
			q = getAvail(); 
  			link(p) = q; 
  			info(q) = cur_tok;
  			p = q;
		}
done1: 
		q = getAvail(); 
  		link(p) = q; 
  		info(q) = TEX_END_MATCH_TOKEN;
  		p = q;
  		
  		if (cur_cmd == TEX_RIGHT_BRACE)
  		{
  			printErr(tr("Missing { inserted")); 
  			align_state++;
  			help2(tr("Where was the left brace? You said something like `\\def\\a}',"));
  			help_line[0] = tr("which I'm going to interpret as `\\def\\a{}'.");
  			error(); 
  			goto found;
  		}
  		
done:
		;
	}
	else
		scanLeftBrace();
		
	unbalance = 1;
	while (true)
	{
		if (xpand)
		{
			while (true)
			{
				getNext();
				if (cur_cmd >= TEX_CALL)
				{
					if (info(link(cur_chr)) == TEX_PROTECTED_TOKEN)
					{
						cur_cmd = TEX_RELAX; 
						cur_chr = TEX_NO_EXPAND_FLAG;
					}
				}
				
				if (cur_cmd <= TEX_MAX_COMMAND)
					goto done2;
					
				if (cur_cmd != TEX_THE)
					expand();
				else
				{
					q = theToks();
					if (link(temp_head) != TEX_NULL)
					{
						link(p) = link(temp_head); 
						p = q;
					}
				}
			}
done2: 
			xToken();
		}
		else
			getToken();
			
		if (cur_tok < TEX_RIGHT_BRACE_LIMIT)
		{
			if (cur_cmd < TEX_RIGHT_BRACE)
				unbalance++;
			else
			{
				unbalance--;
				if (unbalance == 0)
					goto found;
			}
		}
		else if (cur_cmd == TEX_MAC_PARAM)
		{
			if (macro_def)
			{
				s = cur_tok;
				if (xpand)
					getXToken();
				else
					getToken();
					
				if (cur_cmd != TEX_MAC_PARAM)
				{
					if ((cur_tok <= TEX_ZERO_TOKEN) || (cur_tok > t))
					{
						printErr(tr("Illegal parameter number in definition of "));
						sprintCS(warning_index);
    					help3(tr("You meant to type ## instead of #, right?"));
    					help_line[1] = tr("Or maybe a } was forgotten somewhere earlier, and things");
    					help_line[0] = tr("are all screwed up? I'm going to assume that you meant ##.");
    					backError(); 
    					cur_tok = s;
					}
					else
						cur_tok = TEX_OUT_PARAM_TOKEN - '0' + cur_chr;
				}
			}
		}
		
		q = getAvail(); 
  		link(p) = q; 
  		info(q) = cur_tok;
  		p = q;
	}
	
found: 
	scanner_status = TEX_NORMAL;
	if (hash_brace != 0)
	{
		q = getAvail(); 
  		link(p) = q; 
  		info(q) = hash_brace;
  		p = q;
	}
	return p;
}

void XWTeX::scanTwentySevenBitInt()
{
	scanInt();
	if ((cur_val < 0) || (cur_val > 0x7FFFFFF))
	{
		printErr(tr("Bad delimiter code"));
		help2(tr("A numeric delimiter code must be between 0 and 0x7FFFFFF."));
		help_line[0] = tr("I changed this one to zero.");
		intError(cur_val); 
		cur_val = 0;
	}
	
	cur_val1 = (((cur_val % 0x1000) / 0x100) * 0x10000) + (cur_val % 0x100);
	cur_val  = cur_val / 0x1000;
	cur_val = ((cur_val / 0x1000) * 0x1000000) +
           		(((cur_val % 0x1000) / 0x100) * 0x10000) +
           		(cur_val % 0x100);
}

qint32 XWTeX::strToks(qint32 b)
{
	strRoom(1);
	qint32 p = temp_head; 
	link(p) = TEX_NULL; 
	qint32 k = b;
	qint32 q = 0;
	while (k < pool_ptr)
	{
		qint32 t = str_pool[k];
		if (t == ' ')
			t = TEX_SPACE_TOKEN;
		else
			t = TEX_OTHER_TOKEN + t;
			
		fastGetAvail(q); 
		link(p) = q; 
		info(q) = t;
  		p = q;
  		k++;
	}
	
	pool_ptr = b;
	return p;
}

