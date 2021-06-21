/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <QDateTime>
#include <QTextStream>
#include <QByteArray>
#include "XWApplication.h"
#include "XWTeXIODev.h"
#include "XWTeX.h"


void XWTeX::alignError()
{
	if (qAbs(align_state) > 2)
	{
		printErr(tr("Misplaced ")); 
		printCmdChr((quint16)cur_cmd, cur_chr);
		if (cur_tok == (TEX_TAB_TOKEN + '&'))
		{
			help6(tr("I can't figure out why you would want to use a tab mark"));
			help_line[4] = tr("here. If you just want an ampersand, the remedy is");
			help_line[3] = tr("simple: Just type `I\\&' now. But if some right brace");
			help_line[2] = tr("up above has ended a previous alignment prematurely,");
			help_line[1] = tr("you're probably due for more error messages, and you");
			help_line[0] = tr("might try typing `S' now just to see what is salvageable.");
		}
		else
		{
			help5(tr("I can't figure out why you would want to use a tab mark"));
			help_line[3] = tr("or \\cr or \\span just now. If something like a right brace");
			help_line[2] = tr("up above has ended a previous alignment prematurely,");
			help_line[1] = tr("you're probably due for more error messages, and you");
			help_line[0] = tr("might try typing `S' now just to see what is salvageable.");
		}
		error();
	}
	else
	{
		backInput();
		if (align_state < 0)
		{
			printErr(tr("Missing { inserted"));
			align_state++; 
			cur_tok = TEX_LEFT_BRACE_TOKEN + '{';
		}
		else
		{
			printErr(tr("Missing } inserted"));
			align_state--; 
			cur_tok = TEX_RIGHT_BRACE_TOKEN + '}';
		}
		
		help3(tr("I've put in what seems to be necessary to fix"));
		help_line[1] = tr("the current column of the current alignment.");
		help_line[0] = tr("Try to go on, since this might almost work.");
		insError();
	}
}

void XWTeX::backError()
{
	OK_to_interrupt = false; 
	backInput(); 
	OK_to_interrupt = true; 
	error();
}

void XWTeX::beginDiagnostic()
{
	old_setting = selector;
	if ((tracingOnline() <= 0) && (selector == TEX_TERM_AND_LOG))
	{
		selector--;
		if (history == TEX_SPOTLESS)
			history = TEX_WARNING_ISSUED;
	}
}

void XWTeX::boxError(qint32 n)
{
	error(); 
	beginDiagnostic();
	printnl(tr("The following box has been deleted:"));
	showBox(box(n)); 
	endDiagnostic(true);
	flushNodeList(box(n)); 
	setEquiv(TEX_BOX_BASE + n, TEX_NULL);
}

void XWTeX::bufferOverFlow()
{
	if (format_ident == 0)
	{
		QString msg = QString(tr("Buffer size exceeded!\n"));
		print(msg);
	}
	else
	{
		cur_input.loc_field   = first; 
		cur_input.limit_field = last - 1;
		overFlow(tr("buffer size"), buf_size);
	}
}

void XWTeX::charWarning(qint32 f, qint32 c)
{
	if (tracingLostChars() > 0)
	{
		old_setting = tracingOnline();
		if (eTeX_ex && (tracingLostChars() > 1))
			setNewEqtbInt(TEX_INT_BASE + TEX_TRACING_ONLINE_CODE, 1);
			
		beginDiagnostic();
  		printnl(tr("Missing character: There is no "));
  		printChar(c); 
  		print(tr(" in font "));
  		slowPrint(font_name(f)); 
  		printChar('!'); 
  		endDiagnostic(false);
  		setNewEqtbInt(TEX_INT_BASE + TEX_TRACING_ONLINE_CODE, old_setting);
	}
}

bool XWTeX::checkFullSaveStack()
{
	if (save_ptr > max_save_stack)
	{
		max_save_stack = save_ptr;
		if (max_save_stack > (save_size - 7))
		{
			overFlow(tr("save size"), save_size);
			return false;
		}
	}
	
	return true;
}

void XWTeX::checkInterrupt()
{
	if (interrupt != 0)
		pauseForInstructions();
}

#ifdef XW_TEX_DEBUG

void XWTeX::checkMem(bool print_locs)
{
	qint32 p = mem_min;
	for (; p <= lo_mem_max; p++)
		mfree[p] = false;
		
	for (p = hi_mem_min; p <= mem_end; p++)
		mfree[p] = false;
		
	p = avail; 
	qint32 q = TEX_NULL; 
	bool clobbered = false;
	while (p != TEX_NULL)
	{
		if ((p > mem_end) || (p < hi_mem_min))
			clobbered = true;
		else if (mfree[p])
			clobbered = true;
			
		if (clobbered)
		{
			QString msg = QString(tr("AVAIL list clobbered at %1")).arg(q);
			printnl(msg);
			goto done1;
		}
		
		mfree[p] = true; 
		q = p; 
		p = link(q);
	}
	
done1:
	p = rover; 
	q = TEX_NULL; 
	clobbered = false;
	do
	{
		if ((p >= lo_mem_max) || (p < mem_min))
			clobbered = true;
		else if ((rlink(p) >= lo_mem_max) || (rlink(p) < mem_min))
			clobbered = true;
		else if (!isEmpty(p) || 
			     (node_size(p) < 2) || 
			     (p + node_size(p) > lo_mem_max) || 
			     (llink(rlink(p)) != p))
		{
			clobbered = true;
		}
		
		if (clobbered)
		{
			QString msg = QString(tr("Double-AVAIL list clobbered at %1")).arg(q);
			printnl(msg);
			goto done2;
		}
		
		for (q = p; q < (p + node_size(p)); p++)
		{
			if (mfree[q])
			{
				QString msg = QString(tr("Doubly free location at %1")).arg(q);
				printnl(msg);
				goto done2;
			}
			
			mfree[q] = true;
		}
		
		q = p; 
		p = rlink(p);	
	} while (p != rover);
	
done2:
	p = mem_min;
	while (p <= lo_mem_max)
	{
		if (isEmpty(p))
		{
			QString msg = QString(tr("Bad flag at %1")).arg(p);
			printnl(msg);
		}
		
		while ((p <= lo_mem_max) && !mfree[p])
			p++;
			
		while ((p <= lo_mem_max) && mfree[p])
			p++;
	}
	
	if (print_locs)
	{
		printnl(tr("New busy locs:"));
		for (p = mem_min; p <= lo_mem_max; p++)
		{
			if (!mfree[p] && ((p > was_lo_max) || was_free[p]))
			{
				printChar(' '); 
				printInt(p);
			}
		}
		
		for (p = hi_mem_min; p <= mem_end; p++)
		{
			if (!mfree[p] && ((p < was_hi_min) || (p > was_mem_end) || was_free[p]))
			{
				printChar(' '); 
				printInt(p);
			}
		}
	}
	
	for (p = mem_min; p <= lo_mem_max; p++)
		was_free[p] = mfree[p];
		
	for (p = hi_mem_min; p <= mem_end; p++)
		was_free[p] = mfree[p];
		
	was_mem_end = mem_end; 
	was_lo_max = lo_mem_max; 
	was_hi_min = hi_mem_min;
}

#endif //XW_TEX_DEBUG

void XWTeX::checkOuterValidity()
{
	if (scanner_status != TEX_NORMAL)
	{
		deletions_allowed = false;
		if (cur_cs != 0)
		{
			if ((state == TEX_TOKEN_LIST) || (texname < 1) || (texname > 17))
			{
				qint32 p = getAvail(); 
				info(p) = TEX_CS_TOKEN_FLAG + cur_cs;
				backList(p);
			}
			
			cur_cmd = TEX_SPACER; 
			cur_chr = ' ';
		}
		
		if (scanner_status > TEX_SKIPPING)
		{
			runaway();
			if (cur_cs == 0)
				printErr(tr("File ended"));
			else
			{
				cur_cs = 0; 
				printErr(tr("Forbidden control sequence found"));
			}
			
			print(tr(" while scanning "));
			qint32 p = getAvail();
			qint32 q = 0;
			switch (scanner_status)
			{
				case TEX_DEFINING: 
					print(tr("definition")); 
					info(p) = TEX_RIGHT_BRACE_TOKEN + '}';
					break;
					
				case TEX_MATCHING:
					print("use"); 
					info(p) = par_token; 
					long_state = TEX_OUTER_CALL;
					break;
					
				case TEX_ALIGNING:
					print("preamble"); 
					info(p) = TEX_RIGHT_BRACE_TOKEN + '}'; 
					q = p;
  					p = getAvail(); 
  					link(p) = q; 
  					info(p) = TEX_CS_TOKEN_FLAG + TEX_FROZEN_CR;
  					align_state = -1000000;
  					break;
  					
  				case TEX_ABSORBING:
  					print("text"); 
  					info(p) = TEX_RIGHT_BRACE_TOKEN + '}';
  					break;
			}
			insList(p);
			print(tr(" of ")); 
			sprintCS(warning_index);
			help4(tr("I suspect you have forgotten a `}', causing me"));
			help_line[2] = tr("to read past where you wanted me to stop.");
			help_line[1] = tr("I'll try to recover; but if the error is serious,");
			help_line[0] = tr("you'd better type `E' or `X' now and fix your file.");
			error();
		}
		else
		{
			printErr(tr("Incomplete ")); 
			printCmdChr((quint16)TEX_IF_TEST, cur_if);
			print(tr("; all text was ignored after line ")); 
			printInt(skip_line);
			help3(tr("A forbidden control sequence occurred in skipped text."));
			help_line[1] = tr("This kind of error happens when you say `\\if...' and forget");
			help_line[0] = tr("the matching `\\fi'. I've inserted a `\\fi'; this might work.");
			if (cur_cs != 0)
				cur_cs = 0;
			else
				help_line[2] = tr("The file ended while I was skipping conditional text.");
				
			cur_tok = TEX_CS_TOKEN_FLAG + TEX_FROZEN_FI; 
			insError();
		}
		
		deletions_allowed = true;
	}
}

void XWTeX::clearForErrorPrompt()
{
	while ((state != TEX_TOKEN_LIST) && 
	       terminal_input && 
	       (input_ptr > 0) && (loc > limit)) 
	{
		endFileReading();
	}
	
	println();
}

void XWTeX::confusion(const QString & s)
{
	normalizeSelector();
	if (history < TEX_ERROR_MESSAGE_ISSUED)
	{
		printErr(tr("This can't happen (")); 
		print(s); 
		printChar(')');
		help1(tr("I'm broken. Please show this to someone who can fix can fix"));
	}
	else
	{
		printErr(tr("I can't go on meeting you like this"));
		help2(tr("One of your faux pas seems to have wounded me deeply..."));
		help_line[0] = tr("in fact, I'm barely conscious. Please fix it and try again.");
	}
	
	succumb();
}

void XWTeX::csError()
{
	printErr(tr("Extra ")); 
	printEsc(TeXEndCsName);
	help1(tr("I'm ignoring this, since I wasn't doing a \\csname."));
	error();
}

#ifdef XW_TEX_DEBUG
	
void XWTeX::debugHelp()
{
	qint32 k, l;
	char m, n;
	QString ba;
	while (true)
	{
		printnl(tr("debug # (-1 to exit):"));
		ba = dev->termInput();
		m = ba.toInt();
		if (m < 0)
			return ;
			
		if (m == 0)
		{
			goto breakpoint;
breakpoint: 
			m = 0;
		}
		else
		{
			ba = dev->termInput();
			n = ba.toInt();
			if (n < 0)
				return ;
				
			switch (m)
			{
				case 1: 
					printWord(mem[n]);
					break;
					
				case 2: 
					printInt(info(n));
					break;
					
				case 3: 
					printInt(link(n));
					break;
					
				case 4: 
					printWord(newEqtb(n));
					break;
					
				case 5:
					break;
					
				case 6: 
					printWord(save_stack[n]);
					break;
					
				case 7: 
					showBox(n);
					break;
					
				case 8:
					breadth_max = 10000; 
					depth_threshold = pool_size - pool_ptr - 10;
  					showNodeList(n);
					break;
					
				case 9: 
					showTokenList(n, TEX_NULL, 1000);
					break;
					
				case 10: 
					slowPrint(n);
					break;
					
				case 11: 
					checkMem(n > 0);
					break;
					
				case 12: 
					searchMem(n);
					break;
					
				case 13:
					ba = dev->termInput();
					l = ba.toInt();
					printCmdChr((quint16)n, l);
					break;
					
				case 14:
					for (k = 0; k <= n; k++)
						print(buffer[k]);
					break;
					
				case 15:
					font_in_short_display = TEX_NULL_FONT; 
					shortDisplay(n);
					break;
					
				case 16:
					panicking = !panicking;
					break;
					
				default:
					print('?');
					break;
			}
		}
	}
}

#endif //XW_TEX_DEBUG

void XWTeX::endDiagnostic(bool blank_line)
{
	printnl("");
	if (blank_line)
		println();
		
	selector = old_setting;
}

void XWTeX::error()
{
	if (history < TEX_ERROR_MESSAGE_ISSUED)
		history = TEX_ERROR_MESSAGE_ISSUED;
		
	if (!file_line_error_style_p)
	{
		printChar('.'); 
		showContext();
	}
	
	if (halt_on_error_p)
	{
		history = TEX_FATAL_ERROR_STOP; 
		jumpOut();
		return ;
	}
	
	if (interaction == TEX_ERROR_STOP_MODE)
	{
		while (true)
		{
tcontinue:
			clearForErrorPrompt();
			promptInput("? ");
			if (last == first)
				return ;
			
			qint32 c = buffer[first];
			if (c >= 'a')
				c = c + 'A' - 'a';
			
			switch (c)
			{
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				if (deletions_allowed)
				{
					qint32 s1 = cur_tok; 
					qint32 s2 = cur_cmd; 
					qint32 s3 = cur_chr; 
					qint32 s4 = align_state;
					align_state = 1000000; 
					OK_to_interrupt = false;
					if ((last > first + 1) && 
						(buffer[first + 1] >= '0') && 
						(buffer[first + 1] <= '9'))
					{
						c = c * 10 + buffer[first + 1] - '0' * 11;
					}
					else
						c = c - '0';
						
					while (c > 0)
					{
						getToken();
						c--;
					}
					
					cur_tok = s1; 
					cur_cmd = s2; 
					cur_chr = s3; 
					align_state = s4; 
					OK_to_interrupt = true;
					help2(tr("I have just deleted some text, as you asked."));
					help_line[0] = tr("You can now delete more, or insert, or whatever.");
					showContext();
					goto tcontinue;
				}
				break;
				
#ifdef XW_TEX_DEBUG

				case 'D':
					debugHelp();
					goto tcontinue;
					break;
					
#endif //XW_TEX_DEBUG
				
				case 'E':
					if (base_ptr > 0)
					{
						edit_name_start  = str_start(edit_file.name_field);
    					edit_name_length = str_start(edit_file.name_field+1) - str_start(edit_file.name_field);
    					edit_line = line;
						jumpOut();
						return ;
					}
					break;
				
				case 'H':
					if (use_err_help)
					{
						giveErrHelp();
						use_err_help = false;
					}
					else
					{
						if (help_ptr == 0)
						{
							help2(tr("Sorry, I don't know how to help in this situation."));
							help_line[0] = tr("Maybe you should try asking a human?");
						}
					
						do
						{
							help_ptr--;
							print(help_line.at(help_ptr));
							print("\n");
						} while (help_ptr != 0);
					}
				
					help4(tr("Sorry, I already gave what help I could..."));
				    help_line[2] = tr("Maybe you should try asking a human? ");
				    help_line[1] = tr("An error might have occurred before I noticed any problems.");
				    help_line[0] = tr("``If all else fails, read the instructions.''");
				    goto tcontinue;
					break;
				
				case 'I':
					beginFileReading();
					if (last > (first + 1))
					{
						loc = first + 1; 
						buffer[first] = ' ';
					}
					else
					{
						promptInput(tr("insert>")); 
						loc = first;
					}
					first = last;
					cur_input.limit_field = last - 1;
					return ;
					break;
				
				case 'Q':
				case 'R':
				case 'S':
					error_count = 0; 
					interaction = TEX_BATCH_MODE + c - 'Q';
					print(tr("OK, entering "));
					if (c == 'Q')
					{
						printEsc(TeXBatchMode);
						selector--;
					}
					else if (c == 'R')
						printEsc(TeXNonStopMode);
					else
						printEsc(TeXScrollMode);
					print("...");
					println();
					return ;
					break;
				
				case 'X':
					jumpOut();
					return ;
					break;
				
				default:
					break;
			}
		
			print(tr("Type <return> to proceed, S to scroll future error messages, "));
			printnl(tr("R to run without stopping, Q to run quietly, "));
			printnl(tr("I to insert something, "));
			if (base_ptr > 0)
				print(tr("E to edit your file,"));
			
			if (deletions_allowed)
				printnl(tr("1 or ... or 9 to ignore the next 1 to 9 tokens of input,"));
			
			printnl(tr("H for help, X to quit."));
		}
		
		return ;
	}
	
	error_count++;
	
	if (error_count == 100)
	{
		print(tr("(That makes 100 errors; please try again.)\n"));
		history = TEX_FATAL_ERROR_STOP;
		jumpOut();
		return ;
	}
	
	if (interaction > TEX_BATCH_MODE)
		selector--;
		
	if (use_err_help)
	{
		println(); 
		giveErrHelp();
	}
	else
	{
		while (help_ptr > 0)
		{
			help_ptr--;
			print(help_line.at(help_ptr));
			print("\n");
		}
	}
	
	println();
	
	if (interaction > TEX_BATCH_MODE)
		selector++;
		
	println();
}

void XWTeX::extraRightBrace()
{
	printErr(tr("Extra }, or forgotten "));
	switch (cur_group)
	{
		case TEX_SEMI_SIMPLE_GROUP: 
			printEsc(TeXEndGroup);
			break;
			
		case TEX_MATH_SHIFT_GROUP: 
			printChar('$');
			break;
			
		case TEX_MATH_LEFT_GROUP: 
			printEsc(TeXRight);
			break;
	}
	
	help5(tr("I've deleted a group-closing symbol because it seems to be"));
	help_line[3] = tr("spurious, as in `$x}$'. But perhaps the } is legitimate and");
	help_line[2] = tr("you forgot something else, as in `\\hbox{$x}'. In such cases");
	help_line[1] = tr("the way to recover is to insert both the forgotten and the");
	help_line[0] = tr("deleted material, e.g., by typing `I$}'.");
	error();
	align_state++;
}

void XWTeX::fatalError(const QString & str)
{
	normalizeSelector();
	printErr(tr("Emergency stop\n"));
	help1(str);
	succumb();
}

void XWTeX::fileWarning()
{
	qint32 p, l, c, i;
	
	p = save_ptr; 
	l = cur_level; 
	c = cur_group; 
	save_ptr = cur_boundary;
	while (grp_stack[in_open] != save_ptr)
	{
		cur_level--;
  		printnl(tr("Warning: end of file when "));
  		printGroup(true); 
  		print(tr(" is incomplete"));
  		cur_group = save_level(save_ptr); 
  		save_ptr  = save_index(save_ptr);
	}
	
	save_ptr  = p; 
	cur_level = l; 
	cur_group = c;
	p = cond_ptr; 
	l = if_limit; 
	c = cur_if; 
	i = if_line;
	while (if_stack[in_open] != cond_ptr)
	{
		printnl(tr("Warning: end of file when "));
		printCmdChr((quint16)TEX_IF_TEST, cur_if);
		if (if_limit == TEX_FI_CODE)
			printEsc(TeXElse);
				
		printIfLine(if_line); 
		print(tr(" is incomplete"));
		if_line  = if_line_field(cond_ptr); 
		cur_if   = subtype(cond_ptr);
  		if_limit = type(cond_ptr); 
  		cond_ptr = link(cond_ptr);
	}
	
	cond_ptr = p; 
	if_limit = l; 
	cur_if = c; 
	if_line = i;
	println();
	
	if (tracingNesting() > 1)
		showContext();
			
	if (history == TEX_SPOTLESS)
		history = TEX_WARNING_ISSUED;
}

void XWTeX::giveErrHelp()
{
	tokenShow(errHelp());
}

#ifdef XW_TEX_STAT
void XWTeX::groupTrace(bool e)
{
	beginDiagnostic(); 
	printChar('{');
	if (e)
		print(tr("leaving "));
	else
		print(tr("entering "));
		
	printGroup(e); 
	printChar('}'); 
	endDiagnostic(false);
}
#endif //XW_TEX_STAT

void XWTeX::groupWarning()
{
	base_ptr = input_ptr;
	input_stack[base_ptr] = cur_input;
	qint32 i = in_open; 
	bool w = false;
	while ((grp_stack[i] == cur_boundary) && (i > 0))
	{
		if (tracingNesting() > 0)
		{
			while ((input_stack[base_ptr].state_field == TEX_TOKEN_LIST) || 
			       (input_stack[base_ptr].index_field > i))
			{
				base_ptr--;
			}
			
			if (input_stack[base_ptr].name_field > 17)
				w = true;
		}
		
		grp_stack[i] = save_index(save_ptr); 
		i--;
	}
	
	if (w)
	{
		printnl(tr("Warning: end of ")); 
		printGroup(true);
		print(tr(" of a different file")); 
		println();
		if (tracingNesting() > 1)
			showContext();
			
		if (history == TEX_SPOTLESS)
			history = TEX_WARNING_ISSUED;
	}
}

void XWTeX::ifWarning()
{
	base_ptr = input_ptr;
	input_stack[base_ptr] = cur_input;
	qint32 i = in_open; 
	bool w = false;
	while (if_stack[i] == cond_ptr)
	{
		if (tracingNesting() > 0)
		{
			while ((input_stack[base_ptr].state_field == TEX_TOKEN_LIST) || 
			       (input_stack[base_ptr].index_field > i))
			{
				base_ptr--;
			}
			
			if (input_stack[base_ptr].name_field > 17)
				w = true;
		}
		
		if_stack[i] = link(cond_ptr); 
		i--;
	}
	
	if (w)
	{
		printnl(tr("Warning: end of ")); 
		printCmdChr((quint16)TEX_IF_TEST, cur_if);
		printIfLine(if_line); 
		print(tr(" of a different file")); 
		println();
		if (tracingNesting() > 1)
			showContext();
			
		if (history == TEX_SPOTLESS)
			history = TEX_WARNING_ISSUED;
	}
}

void XWTeX::insError()
{
	OK_to_interrupt = false; 
	backInput(); 
	token_type = TEX_INSERTED;
	OK_to_interrupt = true; 
	error();
}

void XWTeX::insertDollarSign()
{
	backInput(); 
	cur_tok = TEX_MATH_SHIFT_TOKEN + '$';
	printErr(tr("Missing $ inserted"));
	help2(tr("I've inserted a begin-math/end-math symbol since I think"));
	help_line[0] = tr("you left one out. Proceed, with fingers crossed.");
	insError();
}

void XWTeX::intError(qint32 n)
{
	printChar(' ');
	printChar('(');
	printInt(n); 
	printChar(')'); 
	error();
}

void XWTeX::jumpOut()
{
//	closeFilesAndTerminate();
	ready_already = 0;
	if ((history != TEX_SPOTLESS) && (history != TEX_WARNING_ISSUED))
		dev->quit(1);
	else
		dev->quit(0);
		
	history = TEX_FATAL_ERROR_STOP;
}

void XWTeX::muError()
{
	printErr(tr("Incompatible glue units"));
	help1(tr("I'm going to assume that 1mu=1pt when they're mixed."));
	error();
}

void XWTeX::noalignError()
{
	printErr(tr("Misplaced ")); 
	printEsc(TeXNoAlign);
	help2(tr("I expect to see \\noalign only after the \\cr of"));
	help_line[0] = tr("an alignment. Proceed, and I'll ignore this case.");
	error();
}

void XWTeX::offSave()
{
	qint32 p;
	if (cur_group == TEX_BOTTOM_LEVEL)
	{
		printErr(tr("Extra ")); 
		printCmdChr((quint16)cur_cmd, cur_chr);
		help1(tr("Things are pretty mixed up, but I think the worst is over."));
		error();
	}
	else
	{
		backInput(); 
		p = getAvail(); 
		link(temp_head) = p;
		printErr(tr("Missing "));
		switch (cur_group)
		{
			case TEX_SEMI_SIMPLE_GROUP:
				info(p) = TEX_CS_TOKEN_FLAG + TEX_FROZEN_END_GROUP;
  				printEsc(TeXEndGroup);
				break;
				
			case TEX_MATH_SHIFT_GROUP:
				info(p) = TEX_MATH_SHIFT_TOKEN + '$'; 
				printChar('$');
				break;
				
			case TEX_MATH_LEFT_GROUP:
				info(p) = TEX_CS_TOKEN_FLAG + TEX_FROZEN_RIGHT; 
				link(p) = getAvail();
  				p = link(p); 
  				info(p) = TEX_OTHER_TOKEN + '.'; 
  				printEsc(TeXRight);
  				printChar('.');
				break;
				
			default:
				info(p) = TEX_RIGHT_BRACE_TOKEN + '}'; 
				printChar('}');
				break;
		}
		
		print(tr(" inserted")); 
		insList(link(temp_head));
  		help5(tr("I've inserted something that you may have forgotten."));
  		help_line[3] = tr("(See the <inserted text> above.)");
  		help_line[2] = tr("With luck, this will get me unwedged. But if you");
  		help_line[1] = tr("really didn't forget anything, try typing `2' now; then");
  		help_line[0] = tr("my insertion and my current dilemma will both disappear.");
  		error();
	}
}

void XWTeX::omitError()
{
	printErr(tr("Misplaced ")); 
	printEsc(TeXOmit);
	help2(tr("I expect to see \\omit only after tab marks or the \\cr of"));
	help_line[0] = tr("an alignment. Proceed, and I'll ignore this case.");
	error();
}

void XWTeX::openLogFile()
{
	old_setting = selector;
	if (job_name == 0)
		job_name = TeXTeXinputs;
	dev->openLogFile();
	selector = TEX_LOG_ONLY; 
	log_opened = true;
	
	printBanner();
	print("** ");
	input_stack[input_ptr] = cur_input;
	qint32 l = input_stack[0].limit_field;
	if (buffer[l] == endLineChar())
		l--;
		
	for (qint32 k = 0; k <= l; k++)
		print(buffer[k]);
	println();
	
	selector = old_setting + 2;
}

void XWTeX::overFlow(const QString & str, qint32 n)
{
	normalizeSelector();
	printErr(tr("XWAleph capacity exceeded, sorry ["));
	print(str);	
	printChar('='); 
	printInt(n); 
	printChar(']');
	help2(tr("If you really absolutely need more capacity,"));
	help_line[0] = tr("you can ask a wizard to enlarge me.");
	succumb();
}

void XWTeX::overFlowOcpBufSize()
{
	overFlow("ocp_buf_size", ocp_buf_size);
}

void XWTeX::overFlowOcpStackSize()
{
	overFlow("ocp_stack_size", ocp_stack_size);
}

void XWTeX::pauseForInstructions()
{
	if (OK_to_interrupt)
	{
		interaction = TEX_ERROR_STOP_MODE;
		if ((selector == TEX_LOG_ONLY) || (selector == TEX_NO_PRINT))
			selector--;
			
		printErr(tr("Interruption"));
		help3(tr("You rang?"));
		help_line[1] = tr("Try to insert some instructions for me (e.g.,`I\\showlists'),");
		help_line[0] = tr("unless you just want to quit by typing `X'.");
		deletions_allowed = false; 
		error(); 
		deletions_allowed = true;
        interrupt = 0;
	}
}

void XWTeX::promptFileName(const QString & s, const QString & e, bool w)
{
	if (!w)
		printErr(tr("I can't find file '"));
	else
		printErr(tr("I can't write on file '"));
		
	printFileName(cur_name,cur_area,cur_ext);
	print(tr("'."));
	if (e == ".tex")
		showContext();
		
	printnl(tr("Please type another "));
	print(s);
	if (interaction < TEX_SCROLL_MODE)
	{
		fatalError(tr("*** (job aborted, file error in nonstop mode)"));
		return ;
	}
	
	qint32 saved_cur_name = cur_name;
	
	promptInput(": ");
	beginName(); 
	qint32 k = first;
	while (buffer[k] == ' ' && k < last)
	    k++;
	    
	while (true)
	{
		if (k == last)
			goto done;
			
		if (!moreName(buffer[k]))
			goto done;
			
		k++;
	}
	
done:
	endName();
	if (cur_ext == TEX_TOO_BIG_CHAR)
		cur_ext = TeXTex;
		
	if (length(cur_name)==0)
		cur_name = saved_cur_name;
		
	packCurName();
}

void XWTeX::promptInput(const QString & str)
{
	print(str);
	termInput();
}

void XWTeX::reportIllegalCase()
{
	youCant();
	help4(tr("Sorry, but I'm not programmed to handle this case;"));
	help_line[2] = tr("I'll just pretend that you didn't ask for it.");
	help_line[1] = tr("If you're in the wrong mode, you might be able to");
	help_line[0] = tr("return to the right one by typing `I}' or `I$' or `I\\par'.");
	error();
}

void XWTeX::runaway()
{
	qint32 p;
	if (scanner_status > TEX_SKIPPING)
	{
		printnl(tr("Runaway "));
		switch (scanner_status)
		{
			case TEX_DEFINING: 
				print(tr("definition")); 
				p = def_ref;
    			break;
    			
  			case TEX_MATCHING: 
  				print(tr("argument")); 
  				p = temp_head;
    			break;
    			
  			case TEX_ALIGNING: 
  				print(tr("preamble")); 
  				p = hold_head;
    			break;
    			
  			case TEX_ABSORBING: 
  				print(tr("text")); 
  				p = def_ref;
    			break;
		}
		
		printChar('?');
		println(); 
		showTokenList(link(p), TEX_NULL, error_line - 10);
	}
}

#ifdef XW_TEX_DEBUG

void XWTeX::searchMem(qint32 p)
{
	qint32 q = mem_min;
	for (; q <= lo_mem_max; q++)
	{
		if (link(q) == p)
		{
			printnl("LINK("); 
			printInt(q); 
			printChar(')');
		}
		
		if (info(q) == p)
		{
			printnl("INFO("); 
			printInt(q); 
			printChar(')');
		}
	}
	
	q = hi_mem_min;
	for (; q <= mem_end; q++)
	{
		if (link(q) == p)
		{
			printnl("LINK("); 
			printInt(q); 
			printChar(')');
		}
		
		if (info(q) == p)
		{
			printnl("INFO("); 
			printInt(q); 
			printChar(')');
		}
	}
	
	q = TEX_ACTIVE_BASE;
	for (; q <= (TEX_BOX_BASE + TEX_BIGGEST_REG); q++)
	{
		if (equiv(q) == p)
		{
			printnl("EQUIV("); 
			printInt(q); 
			printChar(')');
		}
	}
	
	if (save_ptr > 0)
	{
		q = 0;
		for (; q < save_ptr; q++)
		{
			if (equiv_field(save_stack[q]) == p)
			{
				printnl("SAVE("); 
				printInt(q); 
			    printChar(')');
			}
		}
	}
	
	q = 0;
	for (; q <= hyph_size; q++)
	{
		if (hyph_list[q] == p)
		{
			printnl("HYPH("); 
			printInt(q); 
			printChar(')');
		}
	}
}

#endif //XW_TEX_DEBUG

void XWTeX::succumb()
{
	if (interaction == TEX_ERROR_STOP_MODE)
		interaction = TEX_SCROLL_MODE;
		
	if (log_opened)
		error();
		
#ifdef XW_TEX_DEBUG
	if (interaction > TEX_BATCH_MODE)
		debugHelp();
#endif //XW_TEX_DEBUG
		
	history = TEX_FATAL_ERROR_STOP;
	jumpOut();
}

void XWTeX::termInput()
{
	QString termin = dev->termInput();
	if (termin.isEmpty())
	{
		fatalError(tr("End of file on the terminal!\n"));
		return ;
	}
	
	last = first;
	int i = 0;
	while (last < buf_size && i < termin.length())
	{
		buffer[last++] = termin[i++].unicode();
	}
	
	buffer[last] = ' ';
	
	term_offset = 0;
	selector--;
	
	if (last != first)
	{
		for (qint32 k = first; k < last; k++)
			print(buffer[k]);
	}
	
	println(); 
	selector++;
}

void XWTeX::youCant()
{
	printErr(tr("You can't use `"));
	printCmdChr((quint16)cur_cmd, cur_chr);
	print("' in "); 
	printMode(mode);
}
