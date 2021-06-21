/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <QDateTime>
#include <QTextCodec>
#include <QTextStream>
#include "XWApplication.h"
#include "XWTeXIODev.h"
#include "XWTeX.h"

qint32  XWTeX::getcTwo(QIODevice * f)
{
	if (f->atEnd())
		return EOF;
		
	char i = EOF;
	f->getChar((char*)(&i));
	if (f->atEnd())
		return (qint32)i;
		
	char j;
	f->getChar((char*)(&j));
	
	return (qint32)((i << 8) | j);
}

qint32 XWTeX::getcTwoLE(QIODevice * f)
{
	if (f->atEnd())
		return EOF;
		
	char i = EOF;
	f->getChar((char*)(&i));
	if (f->atEnd())
		return (qint32)i;
		
	char j;
	f->getChar((char*)(&j));
	return (qint32)((j << 8) | i);
}

qint32  XWTeX::getFileMode(QIODevice * f, qint32 def)
{
	char m = def;
	if (def == 0 || f->atEnd())
		m = TEX_NO_MODE;
	else
	{
		char buf[5];
		qint64 len = f->read(buf, 4);
		f->seek(0);
		if (len > 0)
		{
			if (buf[0] == 0x5c)
			{
				if (len <= 2)
					m = TEX_NO_MODE;
				else
				{
					if (buf[1] == 0)
						m = TEX_TWOBYTELE_MODE;
					else
						m = TEX_ONEBYTE_MODE;
				}
			}
			else if (buf[0] == 0x25)
			{
				if (len <= 2)
					m = TEX_NO_MODE;
				else
				{
					if (buf[1] == 0)
						m = TEX_TWOBYTELE_MODE;
					else
						m = TEX_ONEBYTE_MODE;
				}
			}
			else if (buf[0] == 0xe0)
				m = TEX_EBCDIC_MODE;
			else if (buf[0] == 0x6c)
				m = TEX_EBCDIC_MODE;
			else if (buf[0] == 0)
			{
				if (len <= 2)
					m = TEX_NO_MODE;
				else if (buf[1] == 0x5c)
					m = TEX_TWOBYTE_MODE;
				else if (buf[1] == 0x25)
					m = TEX_TWOBYTE_MODE;
			}
		}
	}
	
	return (qint32)m;
}

void XWTeX::backInput()
{
	while ((state == TEX_TOKEN_LIST) && 
	       (loc == TEX_NULL) && 
	       (token_type != TEX_V_TEMPLATE))
	{
		endTokenList();
	}
	
	qint32 p = getAvail(); 
	info(p) = cur_tok;
	if (cur_tok < TEX_RIGHT_BRACE_LIMIT)
	{
		if (cur_tok < TEX_LEFT_BRACE_LIMIT)
			align_state--;
		else
			align_state++;
	}
	
	pushInput(); 
	state = TEX_TOKEN_LIST; 
	texstart = p; 
	token_type = TEX_BACKED_UP;
	loc = p;
}

void XWTeX::beginFileReading()
{
	if (in_open == max_in_open)
	{
		overFlow(tr("text input levels"), max_in_open);
		return ;
	}
	
	if (first == buf_size)
	{
		overFlow(tr("buffer size"), buf_size);
		return ;
	}
	
	in_open++;
	pushInput(); 
	index = in_open;
	source_filename_stack[index] = 0;
	full_source_filename_stack[index] = 0;
	eof_seen[index] = false;
	grp_stack[index] = cur_boundary; 
	if_stack[index] = cond_ptr;
	line_stack[index] = line; 
	texstart = first; 
	state = TEX_MID_LINE;
	texname = 0;
}

void XWTeX::beginTokenList(qint32 p, qint16 t)
{
	pushInput(); 
	state = TEX_TOKEN_LIST; 
	texstart = p; 
	token_type = t;
	if (t >= TEX_MACRO)
	{
		add_token_ref(p);
		if (t == TEX_MACRO)
			param_start = param_ptr;
		else
		{
			loc = link(p);
			if (tracingMacros() > 1)
			{
				beginDiagnostic(); 
				printnl("");
				switch (t)
				{
					case TEX_MARK_TEXT:
						printEsc(TeXMark);
						break;
						
					case TEX_WRITE_TEXT:
						printEsc(TeXWrite);
						break;
						
					default:
						printCmdChr((quint16)TEX_ASSIGN_TOKS, t - TEX_OUTPUT_TEXT + TEX_OUTPUT_ROUTINE_LOC);
						break;
				}
				
				print("->"); 
				tokenShow(p); 
				endDiagnostic(false);
			}
		}
	}
	else
		loc = p;;
}

void XWTeX::endFileReading()
{
	first = texstart; 
	line = line_stack[index];
	if ((texname == 18) || (texname == 19))
		pseudoClose();
	else if (texname > 17)
	{
		if (cur_file)
		{
		 cur_file->close();
			delete cur_file;
			cur_file = 0;
		}
	}
		
	popInput(); 
	in_open--;
}

void XWTeX::endTokenList()
{
	if (token_type >= TEX_BACKED_UP)
	{
		if (token_type <= TEX_INSERTED)
			flushList(texstart);
		else
		{
			deleteTokenRef(texstart);
			if (token_type == TEX_MACRO)
			{
				while (param_ptr > param_start)
				{
					param_ptr--;
					flushList(param_stack[param_ptr]);
				}
			}
		}
	}
	else if (token_type == TEX_U_TEMPLATE)
	{
		if (align_state > 500000)
			align_state = 0;
		else
			fatalError(tr("(interwoven alignment preambles are not allowed)"));
	}
	
	popInput();
	checkInterrupt();
}

void XWTeX::expand()
{
	qint32 t, p, r, q, j, cv_backup, backup_backup;
	qint16 cvl_backup, radix_backup, co_backup, save_scanner_status;
	cv_backup     = cur_val; 
	cvl_backup    = cur_val_level; 
	radix_backup  = radix;
	co_backup     = cur_order; 
	backup_backup = link(backup_head);
	
reswitch:
	if (cur_cmd < TEX_CALL)
	{
		if (tracingCommands() > 1)
			 showCurCmdChr();
			 
		switch (cur_cmd)
		{
			case TEX_TOP_BOT_MARK:
				t = cur_chr % TEX_MARKS_CODE;
				if (cur_chr >= TEX_MARKS_CODE)
					scanRegisterNum();
				else
					cur_val = 0;
				if (cur_val == 0)
					cur_ptr = cur_mark[t];
				else
				{
					findSaElement(TEX_MARK_VAL, cur_val, false);
					if (cur_ptr != TEX_NULL)
					{
						if (odd(t))
							cur_ptr = link(cur_ptr + (t / 2) + 1);
						else
							cur_ptr = info(cur_ptr + (t / 2) + 1);
					}
				}				
				if (cur_ptr != TEX_NULL)
					beginTokenList(cur_ptr, TEX_MARK_TEXT);
				break;
				
			case TEX_EXPAND_AFTER:
				if (cur_chr == 0)
				{
					getToken(); 
					t= cur_tok; 
					getToken();
					if (cur_cmd > TEX_MAX_COMMAND)
						expand();
					else
						backInput();
						
					cur_tok = t; 
					backInput();
				}
				else
				{
					getToken();
					if ((cur_cmd == TEX_IF_TEST) && (cur_chr != TEX_IF_CASE_CODE))
					{
						cur_chr = cur_chr + TEX_UNLESS_CODE; 
						goto reswitch;
					}
					
					printErr(tr("You can't use `")); 
					printEsc(TeXUnless); 
					print(tr("' before `"));
					printCmdChr((quint16)cur_cmd, cur_chr); 
					printChar('\'');
					help1(tr("Continue, and I'll forget that it ever happened."));
					backError();
				}
				break;
				
			case TEX_NO_EXPAND:
				save_scanner_status = scanner_status; 
				scanner_status = TEX_NORMAL;
				getToken(); 
				scanner_status = save_scanner_status; 
				t = cur_tok;
				backInput();
				if (t >= TEX_CS_TOKEN_FLAG)
				{
					p = getAvail(); 
					info(p) = TEX_CS_TOKEN_FLAG + TEX_FROZEN_DONT_EXPAND;
  					link(p) = loc; 
  					texstart   = p; 
  					loc     = p;
				}
				break;
				
			case TEX_CS_NAME:
				r = getAvail(); 
				p = r;
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
				
				j = first; 
				p = link(r);
				while (p != TEX_NULL)
				{
					if (j >= max_buf_stack)
					{
						max_buf_stack = j + 1;
						if (max_buf_stack == buf_size)
						{
							overFlow(tr("buffer size"), buf_size);
							return ;
						}
					}
					
					buffer[j] = info(p) % TEX_MAX_CHAR_VAL; 
					j++; 
					p = link(p);
				}
				if (j > (first + 1))
				{
					no_new_control_sequence = false; 
					cur_cs = idLookup(first, j - first);
  					no_new_control_sequence = true;
				}
				else if (j == first)
					cur_cs = TEX_NULL_CS;
				else
					cur_cs = TEX_SINGLE_BASE + buffer[first];
				flushList(r);
				if (eqType(cur_cs) == TEX_UNDEFINED_CS)
				{
					eqDefine(cur_cs, (quint16)TEX_RELAX, TEX_TOO_BIG_CHAR);
				}
				cur_tok = cur_cs + TEX_CS_TOKEN_FLAG; 
				backInput();
				break;
				
			case TEX_CONVERT:
				convToks();
				break;
				
			case TEX_THE:
				insTheToks();
				break;
				
			case TEX_IF_TEST:
				conditional();
				break;
				
			case TEX_FI_OR_ELSE:
				if (tracingIfs() > 0)
				{
					if (tracingCommands() <= 1)
						showCurCmdChr();
				}
				if (cur_chr > if_limit)
				{
					if (if_limit == TEX_IF_CODE)
						insertRelax();
					else
					{
						printErr(tr("Extra ")); 
						printCmdChr((quint16)TEX_FI_OR_ELSE, cur_chr);
						help1(tr("I'm ignoring this; it doesn't match any \\if."));
    					error();
					}
				}
				else
				{
					while (cur_chr != TEX_FI_CODE)
						passText();
						
					if (if_stack[in_open] == cond_ptr)
						ifWarning();
						
					p = cond_ptr; 
					if_line = if_line_field(p);
					cur_if = subtype(p); 
					if_limit = type(p); 
					cond_ptr = link(p);
					freeNode(p, TEX_IF_NODE_SIZE);
				}
				break;
				
			case TEX_INPUT:
				if (cur_chr == 1)
					force_eof = true;
				else if (cur_chr == 2)
					pseudoStart();
				else if (name_in_progress)
					insertRelax();
				else
					startInput();
				break;
				
			default:
				printErr(tr("Undefined control sequence: "));
				for (qint32 i = 0; i < buffered_cs_len; i++)
				  printChar(buffered_cs[i]);
				help5(tr(" The control sequence at the end of the top line"));
				help_line[3] = tr("of your error message was never \\def'ed. If you have");
				help_line[2] = tr("misspelled it (e.g., `\\hobx'), type `I' and the correct");
				help_line[1] = tr("spelling (e.g., `I\\hbox'). Otherwise just continue,");
				help_line[0] = tr("and I'll forget about whatever was undefined.");
				error();
				break;
		}
	}
	else if (cur_cmd < TEX_END_TEMPLATE)
		macroCall();
	else
	{
		cur_tok = TEX_CS_TOKEN_FLAG + TEX_FROZEN_ENDV; 
		backInput();
	}
	
	cur_val           = cv_backup; 
	cur_val_level     = cvl_backup; 
	radix             = radix_backup;
    cur_order         = co_backup; 
    link(backup_head) = backup_backup;
}

void XWTeX::firmUpTheLine()
{
	limit = last;
	if (pausing() > 0)
	{
		if (interaction > TEX_NONSTOP_MODE)
		{
			println();
			if (texstart < limit)
			{
				for (qint32 k = texstart; k < limit; k++)
					print(buffer[k]);
			}
			
			first = limit; 
			promptInput("=>");
			if (last > first)
			{
				for (qint32 k = first; k < last; k++)
					buffer[k + texstart - first] = buffer[k];
					
				limit = texstart + last - first;
			}
		}
	}
}

void XWTeX::getNext()
{
	qint32 k, t, c, cc, ccc, cccc;
	char cat, d;
	
restart:	
	cur_cs = 0 ;
	if (state != TEX_TOKEN_LIST)
	{
tswitch:
		if (loc <= limit)
		{
			cur_chr = buffer[loc];
			loc++;
			
reswitch:
			cur_cmd = catCode(cur_chr);			
			switch (state + cur_cmd)
			{
				case TEX_MID_LINE    + TEX_IGNORE:
				case TEX_SKIP_BLANKS + TEX_IGNORE:
				case TEX_NEW_LINE    + TEX_IGNORE:
				case TEX_SKIP_BLANKS + TEX_SPACER:
				case TEX_NEW_LINE    + TEX_SPACER:
					goto tswitch;
					break;
					
				case TEX_MID_LINE + TEX_ESCAPE:
				case TEX_SKIP_BLANKS  + TEX_ESCAPE:
				case TEX_NEW_LINE     + TEX_ESCAPE:										
					if (loc > limit)
						cur_cs = TEX_NULL_CS;
					else
					{
start_cs:
						k = loc; 
						cur_chr = buffer[k];
						cat = catCode(cur_chr);
						k++;
						if (cat == TEX_LETTER)
							state = TEX_SKIP_BLANKS;
						else if (cat == TEX_SPACER)
							state = TEX_SKIP_BLANKS;
						else
							state = TEX_MID_LINE;
							
						if ((cat == TEX_LETTER) && (k <= limit))
						{
							do
							{
								cur_chr = buffer[k]; 
								cat = catCode(cur_chr); 
								k++;
							} while ((cat == TEX_LETTER) && (k <= limit));
							
							if (buffer[k] == cur_chr)
							{
								if (cat == TEX_SUP_MARK)
								{
									if (k < limit)
									{
										if ((cur_chr == buffer[k + 1]) && 
											(cur_chr == buffer[k + 2]) && 
           									((k + 6) <= limit))
           								{
           									c    = buffer[k + 3]; 
           									cc   = buffer[k + 4];
           									ccc  = buffer[k + 5]; 
           									cccc = buffer[k + 6];
           									if (isHex(c) && 
           										isHex(cc) && 
           										isHex(ccc) && 
           										isHex(cccc))
           									{
           										d = 7;
           										if (c <= '9')
           											cur_chr = c - '0';
           										else
           											cur_chr = c - 'a' + 10;
           											
           										if (cc <= '9')
           											cur_chr = 16 * cur_chr + cc - '0';
           										else
           											cur_chr = 16 * cur_chr + cc - 'a' + 10;
           												
           										if (ccc <= '9')
           											cur_chr = 16 * cur_chr + ccc - '0';
           										else
           											cur_chr = 16 * cur_chr + ccc - 'a' + 10;
           											
           										if (cccc <= '9')
           											cur_chr = 16 * cur_chr + cccc - '0';
           										else
           											cur_chr = 16 * cur_chr + cccc - 'a' + 10;
           											
           										buffer[k - 1] = cur_chr;           											
           										while (k <= limit)
           										{
           											buffer[k] = buffer[k + d]; 
           											k++;
           										}
           										goto start_cs;
           									}
           								}
           								else
           								{
           									c = buffer[k + 1];
           									if (c < 128)
           									{
           										d = 2;
           										if (isHex(c))
           										{
           											if ((k + 2) <= limit)
           											{
           												cc = buffer[k + 2];
           												if (isHex(cc))
           													d++;
           											}
           										}
           											
           										if (d > 2)
           										{
           											if (c <= '9')
           												cur_chr = c - '0';
           											else
           												cur_chr = c - 'a' + 10;
           												
           											if (cc <= '9')
           												cur_chr = 16 * cur_chr + cc - '0';
           											else
           												cur_chr = 16 * cur_chr + cc - 'a' + 10;
           													
           											buffer[k - 1] = cur_chr;
           										}
           										else if (c < 64)
           											buffer[k - 1] = c + 64;
           										else
           											buffer[k - 1] = c - 64;
           										
           										limit = limit - d; 
           										first = first - d;
           										while (k <= limit)
           										{
           											buffer[k] = buffer[k + d]; 
           											k++;
           										}
												goto start_cs;
           									}
           								}
									}
								}
							}
							
							if (cat != TEX_LETTER)
							{
								k--;
							}
								
							if (k > (loc + 1))
							{
								cur_cs = idLookup(loc, k - loc); 
								loc = k; 
								goto found;
							}
						}
						else
						{
							if (buffer[k] == cur_chr)
							{
								if (cat == TEX_SUP_MARK)
								{
									if (k < limit)
									{
										if ((cur_chr == buffer[k + 1]) && 
											(cur_chr == buffer[k+2]) && 
           									((k + 6) <= limit))
           								{
           									c    = buffer[k + 3]; 
           									cc   = buffer[k + 4];
           									ccc  = buffer[k + 5]; 
           									cccc = buffer[k + 6];
           									if (isHex(c) && 
           										isHex(cc) && 
           										isHex(ccc) && 
           										isHex(cccc))
           									{
           										d = 7;
           										if (c <= '9')
           											cur_chr = c - '0';
           										else
           											cur_chr = c - 'a' + 10;
           											
           										if (cc <= '9')
           											cur_chr = 16 * cur_chr + cc - '0';
           										else
           											cur_chr = 16 * cur_chr + cc - 'a' + 10;
           												
           										if (ccc <= '9')
           											cur_chr = 16 * cur_chr + ccc - '0';
           										else
           											cur_chr = 16 * cur_chr + ccc - 'a' + 10;
           											
           										if (cccc <= '9')
           											cur_chr = 16 * cur_chr + cccc - '0';
           										else
           											cur_chr = 16 * cur_chr + cccc - 'a' + 10;
           											
           										buffer[k - 1] = cur_chr;           											
           										while (k <= limit)
           										{
           											buffer[k] = buffer[k + d]; 
           											k++;
           										}
           										goto start_cs;
           									}
           								}
           								else
           								{
           									c = buffer[k + 1];
           									if (c < 128)
           									{
           										d = 2;
           										if (isHex(c))
           										{
           											if ((k + 2) <= limit)
           											{
           												cc = buffer[k + 2];
           												if (isHex(cc))
           													d++;
           											}
           										}
           										
           										if (d > 2)
           										{
           											if (c <= '9')
           												cur_chr = c - '0';
           											else
           												cur_chr = c - 'a' + 10;
           												
           											if (cc <= '9')
           												cur_chr = 16 * cur_chr + cc - '0';
           											else
           												cur_chr = 16 * cur_chr + cc - 'a' + 10;
           													
           											buffer[k - 1] = cur_chr;
           										}
           										else if (c < 64)
           											buffer[k - 1] = c + 64;
           										else
           											buffer[k - 1] = c - 64;
           									
           										limit = limit - d; 
           										first = first - d;
           										while (k <= limit)
           										{
           											buffer[k] = buffer[k + d]; 
           											k++;
           										}
												goto start_cs;
           									}
           								}
									}
								}
							}
						}
						
						cur_cs = TEX_SINGLE_BASE + buffer[loc]; 
						loc++;
					}
						
found:
					cur_cmd = eqType(cur_cs); 
					cur_chr = equiv(cur_cs);
					if (cur_cmd >= TEX_OUTER_CALL)
						checkOuterValidity();
					break;
					
				case TEX_MID_LINE    + TEX_ACTIVE_CHAR:
				case TEX_SKIP_BLANKS + TEX_ACTIVE_CHAR:
				case TEX_NEW_LINE    + TEX_ACTIVE_CHAR:
					cur_cs  = cur_chr + TEX_ACTIVE_BASE;
					cur_cmd = eqType(cur_cs); 
					cur_chr = equiv(cur_cs); 
					state = TEX_MID_LINE;
					if (cur_cmd >= TEX_OUTER_CALL)
						checkOuterValidity();
					break;
					
				case TEX_MID_LINE    + TEX_SUP_MARK: 
				case TEX_SKIP_BLANKS + TEX_SUP_MARK:
				case TEX_NEW_LINE    + TEX_SUP_MARK:
					if (cur_chr == buffer[loc])
					{
						if (loc < limit)
						{
							if ((cur_chr == buffer[loc + 1]) && 
								(cur_chr == buffer[loc + 2]) && 
								((loc + 6) <= limit))
							{
								c    = buffer[loc + 3]; 
								cc   = buffer[loc + 4];
       							ccc  = buffer[loc + 5]; 
       							cccc = buffer[loc + 6];
       							if (isHex(c) && isHex(cc) && isHex(ccc) && isHex(cccc))
       							{
       								loc = loc + 7; 
           							if (c <= '9')
           								cur_chr = c - '0';
           							else
           								cur_chr = c - 'a' + 10;
           											
           							if (cc <= '9')
           								cur_chr = 16 * cur_chr + cc - '0';
           							else
           								cur_chr = 16 * cur_chr + cc - 'a' + 10;
           												
           							if (ccc <= '9')
           								cur_chr = 16 * cur_chr + ccc - '0';
           							else
           								cur_chr = 16 * cur_chr + ccc - 'a' + 10;
           										
           							if (cccc <= '9')
           								cur_chr = 16 * cur_chr + cccc - '0';
           							else
           								cur_chr = 16 * cur_chr + cccc - 'a' + 10; 
           								
       								goto reswitch;
       							}
							}
							
							c = buffer[loc + 1];
							if (c < 128)
							{
								loc = loc + 2;
								if (isHex(c))
								{
									if (loc <= limit)
									{
										cc = buffer[loc];
										if (isHex(cc))
										{
											loc++;
											if (c <= '9')
           										cur_chr = c - '0';
           									else
           										cur_chr = c - 'a' + 10;
           												
           									if (cc <= '9')
           										cur_chr = 16 * cur_chr + cc - '0';
           									else
           										cur_chr = 16 * cur_chr + cc - 'a' + 10;
           									goto reswitch;
										}
									}
								}
								
								if (c < 64)
									cur_chr = c + 64;
								else
									cur_chr = c - 64;
									
								goto reswitch;
							}
						}
					}
					state = TEX_MID_LINE;
					break;
					
				case TEX_MID_LINE    + TEX_INVALID_CHAR:
				case TEX_SKIP_BLANKS + TEX_INVALID_CHAR:
				case TEX_NEW_LINE    + TEX_INVALID_CHAR:
					printErr(tr("Text line contains an invalid character"));
					help2(tr("A funny symbol that I can't read has just been input."));
					help_line[0] = tr("Continue, and I'll forget that it ever happened.");
					deletions_allowed = false; 
					error(); 
					deletions_allowed = true;
					goto restart;
					break;
					
				case TEX_MID_LINE + TEX_SPACER:
					state = TEX_SKIP_BLANKS; 
					cur_chr = ' ';
					break;
					
				case TEX_MID_LINE + TEX_CAR_RET:
					loc = limit + 1;
					cur_cmd = TEX_SPACER; 
					cur_chr = ' ';
					break;
					
				case TEX_SKIP_BLANKS + TEX_CAR_RET:
				case TEX_MID_LINE    + TEX_COMMENT:
				case TEX_SKIP_BLANKS + TEX_COMMENT:
				case TEX_NEW_LINE    + TEX_COMMENT:
					loc = limit + 1;
					goto tswitch;
					break;
					
				case TEX_NEW_LINE + TEX_CAR_RET:
					loc     = limit + 1;
					cur_cs  =   par_loc; 
					cur_cmd = eqType(cur_cs);
					cur_chr = equiv(cur_cs);
					if (cur_cmd >= TEX_OUTER_CALL)
						checkOuterValidity();
					break;
					
				case TEX_MID_LINE + TEX_LEFT_BRACE:
					align_state++;
					break;
					
				case TEX_SKIP_BLANKS + TEX_LEFT_BRACE:
				case TEX_NEW_LINE    + TEX_LEFT_BRACE:
					state = TEX_MID_LINE; 
					align_state++;
					break;
					
				case TEX_MID_LINE + TEX_RIGHT_BRACE:
					align_state--;
					break;
					
				case TEX_SKIP_BLANKS + TEX_RIGHT_BRACE:
				case TEX_NEW_LINE    + TEX_RIGHT_BRACE:
					state = TEX_MID_LINE; 
					align_state--;
					break;
					
				case TEX_SKIP_BLANKS + TEX_MATH_SHIFT:
				case TEX_SKIP_BLANKS + TEX_TAB_MARK:
				case TEX_SKIP_BLANKS + TEX_MAC_PARAM:
  				case TEX_SKIP_BLANKS + TEX_SUB_MARK:
  				case TEX_SKIP_BLANKS + TEX_LETTER:
  				case TEX_SKIP_BLANKS + TEX_OTHER_CHAR:
  				case TEX_NEW_LINE    + TEX_MATH_SHIFT:
  				case TEX_NEW_LINE    + TEX_TAB_MARK:
  				case TEX_NEW_LINE    + TEX_MAC_PARAM:
  				case TEX_NEW_LINE    + TEX_SUB_MARK:
  				case TEX_NEW_LINE    + TEX_LETTER:
  				case TEX_NEW_LINE    + TEX_OTHER_CHAR:
  					state = TEX_MID_LINE;
  					break;
  					
  				default:
  					break;
			}
		}
		else
		{
			if (current_ocp_lstack > 0)
			{
				popInput(); 
				goto restart;
			}
			
			state = TEX_NEW_LINE;
			if (texname > 17)
			{
				line++; 
				first = texstart;
				if (!force_eof)
				{
					if (texname <= 19)
					{
						if (pseudoInput())
							firmUpTheLine();
						else if ((everyEof() != TEX_NULL) && !eof_seen[index])
						{
							limit = first - 1; 
							eof_seen[index] = true;
							beginTokenList(everyEof(), TEX_EVERY_EOF_TEXT); 
							goto restart;
						}
						else
							force_eof = true;
					}
					else
					{
						if (newInputLn(cur_file, input_file_mode[index], input_file_translation[index], true))
						{
							firmUpTheLine();
						}
						else if ((everyEof() != TEX_NULL) && !eof_seen[index])
						{
							limit = first - 1; 
							eof_seen[index] = true;
							beginTokenList(everyEof(), TEX_EVERY_EOF_TEXT); 
							goto restart;
						}
						else
							force_eof = true;
					}
				}
				
				if (force_eof)
				{
					if (tracingNesting() > 0)
					{
						if ((grp_stack[in_open] != cur_boundary) ||
        					(if_stack[in_open]  != cond_ptr))
        				{
        					fileWarning();
        				}
					}
					
					if (texname >= 19)
					{
						printChar(')'); 
						open_parens--;
					}
					
					force_eof = false;
  				endFileReading();  					
  				checkOuterValidity(); 
  				goto restart;
				}
				
				if (endLineCharInactive())
					limit--;
				else
					buffer[limit] = endLineChar();
					
				first = limit + 1; 
				loc = texstart;
			}
			else
			{
				if (!terminal_input)
				{
					cur_cmd = 0; 
					cur_chr = 0; 
					return;
				}
				
				if (input_ptr > 0)
				{
					endFileReading(); 
					goto restart;
				}
				
				if (selector < TEX_LOG_ONLY)
					openLogFile();
					
				if (interaction > TEX_NONSTOP_MODE)
				{
					if (endLineCharInactive())
						limit++;
						
					if (limit == texstart)
						printnl(tr("(Please type a command or say `\\end')"));
						
					println(); 
					first = texstart;
					promptInput("*");
					limit = last;
					if (endLineCharInactive())
						limit--;
					else
						buffer[limit] = endLineChar();
						
					first =limit + 1;
    				loc = texstart;
				}
				else
				{
					fatalError(tr("*** (job aborted, no legal \\end found)"));
					return ;
				}
			}
			
			checkInterrupt();
  			goto tswitch;
		}
	}
	else
	{
		if (loc != TEX_NULL)
		{
			t   = info(loc); 
			loc = link(loc);
			if (t >= TEX_CS_TOKEN_FLAG)
			{
				cur_cs  = t - TEX_CS_TOKEN_FLAG;
    			cur_cmd = eqType(cur_cs); 
    			cur_chr = equiv(cur_cs);
    			if (cur_cmd >= TEX_OUTER_CALL)
    			{
    				if (cur_cmd == TEX_DONT_EXPAND)
    				{
    					cur_cs = info(loc) - TEX_CS_TOKEN_FLAG; 
    					loc = TEX_NULL;
    					cur_cmd = eqType(cur_cs); 
    					cur_chr = equiv(cur_cs);
    					if (cur_cmd > TEX_MAX_COMMAND)
    					{
    						cur_cmd = TEX_RELAX; 
    						cur_chr = TEX_NO_EXPAND_FLAG;
    					}
    				}
    				else
    					checkOuterValidity();
    			}
			}
			else
			{
				cur_cmd = t / TEX_MAX_CHAR_VAL; 
				cur_chr = t % TEX_MAX_CHAR_VAL;
				switch (cur_cmd)
				{
					case TEX_LEFT_BRACE: 
						align_state++;
						break;
						
					case TEX_RIGHT_BRACE: 
						align_state--;
						break;
						
					case TEX_OUT_PARAM:
						beginTokenList(param_stack[param_start + cur_chr - 1], TEX_PARAMETER);
						goto restart;
						break;
						
					default:
						break;
				}
			}
		}
		else
		{
			endTokenList(); 
			goto restart;
		}
	}
	
	if (cur_cmd <= TEX_CAR_RET)
	{
		if (cur_cmd >= TEX_TAB_MARK)
		{
			if (align_state == 0)
			{
				if ((scanner_status == TEX_ALIGNING) || (cur_align == TEX_NULL))
				{
					fatalError(tr("(interwoven alignment preambles are not allowed)"));
					return ;
				}
				
				cur_cmd = extra_info(cur_align); 
				extra_info(cur_align) = cur_chr;
				if (cur_cmd == TEX_OMIT)
					beginTokenList(omit_template, TEX_V_TEMPLATE);
				else
					beginTokenList(v_part(cur_align), TEX_V_TEMPLATE);
					
				align_state = 1000000; 
				goto restart;
			}
		}
	}
}

void XWTeX::getPreambleToken()
{
restart: 
	getToken();
	while ((cur_chr == TEX_SPAN_CODE) && (cur_cmd == TEX_TAB_MARK))
	{
		getToken();
		if (cur_cmd > TEX_MAX_COMMAND)
		{
			expand(); 
			getToken();
		}
	}
	
	if (cur_cmd == TEX_ENDV)
	{
		fatalError(tr("(interwoven alignment preambles are not allowed)"));
		return ;
	}
	
	if ((cur_cmd == TEX_ASSIGN_GLUE) && 
		(cur_chr == (TEX_GLUE_BASE + TEX_TAB_SKIP_CODE)))
	{
		scanOptionalEquals(); 
		scanGlue(TEX_GLUE_VAL);
		if (globalDefs() > 0)
			geqDefine(TEX_GLUE_BASE + TEX_TAB_SKIP_CODE, (quint16)TEX_GLUE_REF, cur_val);
		else
			eqDefine(TEX_GLUE_BASE + TEX_TAB_SKIP_CODE, (quint16)TEX_GLUE_REF, cur_val);
			
		goto restart;
	}
}

void XWTeX::getRToken()
{
restart:
	do
	{
		getToken();
	} while (cur_tok == TEX_SPACE_TOKEN);
	
	if ((cur_cs == 0) || (cur_cs > TEX_FROZEN_CONTROL_SEQUENCE))
	{
		printErr(tr("Missing control sequence inserted"));
		help5(tr("Please don't say `\\def cs{...}', say `\\def\\cs{...}'."));
		help_line[3] = tr("I've inserted an inaccessible control sequence so that your");
		help_line[2] = tr("definition will be completed without mixing me up too badly.");
		help_line[1] = tr("You can recover graciously from this error, if you're");
		help_line[0] = tr("careful; see exercise 27.2 in The TeXbook.");
		if (cur_cs == 0)
			backInput();
			
		cur_tok = TEX_CS_TOKEN_FLAG + TEX_FROZEN_PROTECTION; 
		insError(); 
		goto restart;
	}
}

void XWTeX::getToken()
{
	no_new_control_sequence = false; 
	getNext(); 
	no_new_control_sequence = true;
	if (cur_cs == 0)
		cur_tok = (cur_cmd * TEX_MAX_CHAR_VAL) + cur_chr;
	else
		cur_tok = TEX_CS_TOKEN_FLAG + cur_cs;
}

void XWTeX::getXOrProtected()
{
	while (true)
	{
		getToken();
		if (cur_cmd <= TEX_MAX_COMMAND)
			return ;
			
		if ((cur_cmd >= TEX_CALL) && (cur_cmd < TEX_END_TEMPLATE))
		{
			if (info(link(cur_chr)) == TEX_PROTECTED_TOKEN)
				return ;
		}
		
		expand();
	}
}

void XWTeX::getXToken()
{
restart: 
	getNext();
	if (cur_cmd <= TEX_MAX_COMMAND)
		goto done;
		
	if (cur_cmd >= TEX_CALL)
	{
		if (cur_cmd < TEX_END_TEMPLATE)
			macroCall();
		else
		{
			cur_cs  = TEX_FROZEN_ENDV; 
			cur_cmd = TEX_ENDV;
			goto done;
		}
	}
	else
		expand();
		
	goto restart;
	
done:
	if (cur_cs == 0)
		cur_tok = (cur_cmd * TEX_MAX_CHAR_VAL) + cur_chr;
	else
		cur_tok = TEX_CS_TOKEN_FLAG + cur_cs;
}

void XWTeX::headForVMode()
{
	if (mode < 0)
	{
		if (cur_cmd != TEX_HRULE)
			offSave();
		else
		{
			printErr(tr("You can't use `"));
			printEsc(TeXHRule); 
			print(tr("' here except with leaders"));
			help2(tr("To put a horizontal rule in an hbox or an alignment,"));
			help_line[0] = tr("you should use \\leaders or \\hrulefill (see The TeXbook).");
			error();
		}
	}
	else
	{
		backInput(); 
		cur_tok = par_token; 
		backInput(); 
		token_type = TEX_INSERTED;
	}
}

bool XWTeX::inputLn(QIODevice * f)
{
	if (f->atEnd())
		return false;
		
	uchar c = 0;
	last = first;
  	while (last < buf_size && f->getChar((char*)(&c)) && c != '\n' && c != '\r')
    	buffer[last++] = c;
    	
    if (f->atEnd() && last == first)
    	return false;
    	
    if (!f->atEnd() && c != '\n' && c != '\r')
    {
    	bufferOverFlow();
		return false;
    }
    
    buffer[last] = ' ';
  	if (last >= max_buf_stack)
    	max_buf_stack = last;
    	
    if (c == '\r') 
    {
    	f->getChar((char*)(&c));
    	if (c != '\n')
      		f->ungetChar((char)c);
  	}
  	
  	while (last > first && ISBLANK(buffer[last - 1]))
    	--last;
    	
    return true;
}

void XWTeX::insertRelax()
{
	cur_tok = TEX_CS_TOKEN_FLAG + cur_cs; 
	backInput();
	cur_tok = TEX_CS_TOKEN_FLAG + TEX_FROZEN_RELAX; 
	backInput(); 
	token_type = TEX_INSERTED;
}

void XWTeX::insTheToks()
{
	link(garbage) = theToks(); 
	insList(link(temp_head));
}

void XWTeX::macroCall()
{
	qint32 r, p, q, s, t, u, v, rbrace_ptr;
	qint32 unbalance, m, ref_count, save_warning_index, match_chr;
	char n, save_scanner_status; 
	
	save_scanner_status = scanner_status; 
	save_warning_index = warning_index;
	warning_index = cur_cs; 
	ref_count = cur_chr; 
	r = link(ref_count); 
	n = 0;
	if (tracingMacros() > 0)
	{
		beginDiagnostic(); 
		println(); 
		printCS(warning_index);
		tokenShow(ref_count); 
		endDiagnostic(false);
	}
	
	if (info(r) == TEX_PROTECTED_TOKEN)
		r = link(r);
		
	if (info(r) != TEX_END_MATCH_TOKEN)
	{
		scanner_status = TEX_MATCHING; 
		unbalance  = 0;
		long_state = eqType(cur_cs);
		if (long_state >= TEX_OUTER_CALL)
			long_state = long_state - 2;
			
		do
		{
			link(temp_head) = TEX_NULL;
			if ((info(r) >= TEX_END_MATCH_TOKEN) || (info(r) < TEX_MATCH_TOKEN))
				s = TEX_NULL;
			else
			{
				match_chr = info(r) - TEX_MATCH_TOKEN; 
				s = link(r); 
				r = s;
  				p = temp_head; 
  				m = 0;
			}
			
tcontinue: 
			getToken();
			if (cur_tok == info(r))
			{
				r = link(r);
				if ((info(r) >= TEX_MATCH_TOKEN) && (info(r) <= TEX_END_MATCH_TOKEN))
				{
					if (cur_tok < TEX_LEFT_BRACE_LIMIT)
						align_state--;
						
					goto found;
				}
				else
					goto tcontinue;
			}
			
			if (s != r)
			{
				if (s == TEX_NULL)
				{
					printErr(tr("Use of ")); 
					sprintCS(warning_index);
					print(tr(" doesn't match its definition"));
					help4(tr("If you say, e.g., `\\def\\a1{...}', then you must always"));
					help_line[2] = tr("put `1' after `\\a', since control sequence names are");
					help_line[1] = tr("made up of letters only. The macro here has not been");
					help_line[0] = tr("followed by the required stuff, so I'm ignoring it.");
					error(); 
					scanner_status = save_scanner_status; 
					warning_index  = save_warning_index;
					return;
				}
				else
				{
					t = s;
					do
					{
						q = getAvail(); 
						link(p) = q; 
						info(q) = info(t);
						p = q;
						m++; 
						u = link(t); 
						v = s;
						while (true)
						{
							if (u == r)
							{
								if (cur_tok != info(v))
									goto done;
								else
								{
									r = link(v); 
									goto tcontinue;
								}
							}
							
							if (info(u) != info(v))
								goto done;
								
							u = link(u); 
							v = link(v);
						}
						
done: 
						t = link(t);
					} while (t != r);
					
					r = s;
				}
			}
			
			if (cur_tok == par_token)
			{
				if (long_state != TEX_LONG_CALL)
				{
					if (long_state == TEX_CALL)
					{
						runaway(); 
						printErr(tr("Paragraph ended before "));
						sprintCS(warning_index); 
						print(tr(" was complete"));
						help3(tr("I suspect you've forgotten a `}', causing me to apply this"));
						help_line[1] = tr("control sequence to too much text. How can we recover?");
						help_line[0] = tr("My plan is to forget the whole thing and hope for the best.");
						backError();
					}
					
					pstack[n] = link(temp_head); 
					align_state = align_state - unbalance;
					for (m = 0; m <= n; m++)
						flushList(pstack[m]);
						
					scanner_status = save_scanner_status; 
					warning_index  = save_warning_index;
					return ;
				}
			}
			
			if (cur_tok < TEX_RIGHT_BRACE_LIMIT)
			{
				if (cur_tok < TEX_LEFT_BRACE_LIMIT)
				{
					unbalance = 1;
					while (true)
					{
						fastGetAvail(q); 
						link(p) = q; 
						info(q) = cur_tok;
  						p = q;
  						getToken();
  						if (cur_tok == par_token)
  						{
  							if (long_state != TEX_LONG_CALL)
  							{
  								if (long_state == TEX_CALL)
								{
									runaway(); 
									printErr(tr("Paragraph ended before "));
									sprintCS(warning_index); 
									print(tr(" was complete"));
									help3(tr("I suspect you've forgotten a `}', causing me to apply this"));
									help_line[1] = tr("control sequence to too much text. How can we recover?");
									help_line[0] = tr("My plan is to forget the whole thing and hope for the best.");
									backError();
								}
					
								pstack[n] = link(temp_head); 
								align_state = align_state - unbalance;
								for (m = 0; m <= n; m++)
									flushList(pstack[m]);
									
								scanner_status = save_scanner_status; 
								warning_index  = save_warning_index;
									
								return ;
  							}
  						}
  						
  						if (cur_tok < TEX_RIGHT_BRACE_LIMIT)
  						{
  							if (cur_tok < TEX_LEFT_BRACE_LIMIT)
  								unbalance++;
  							else
  							{
  								unbalance--;
  								if (unbalance == 0)
  									goto done1;
  							}
  						}
					}
					
done1:
					rbrace_ptr = p; 
					q = getAvail(); 
					link(p) = q; 
					info(q) = cur_tok;
  					p = q;
				}
				else
				{
					backInput(); 
					printErr(tr("Argument of ")); 
					sprintCS(warning_index);
					print(tr(" has an extra }"));
					help6(tr("I've run across a `}' that doesn't seem to match anything."));
					help_line[4] = tr("For example, `\\def\\a#1{...}' and `\\a}' would produce");
					help_line[3] = tr("this error. If you simply proceed now, the `\\par' that");
					help_line[2] = tr("I've just inserted will cause me to report a runaway");
					help_line[1] = tr("argument that might be the root of the problem. But if");
					help_line[0] = tr("your `}' was spurious, just type `2' and it will go away.");
					align_state++;
					long_state = TEX_CALL; 
					cur_tok = par_token; 
					insError();
				}
			}
			else
			{
				if (cur_tok == TEX_SPACE_TOKEN)
				{
					if (info(r) <= TEX_END_MATCH_TOKEN)
					{
						if (info(r) >= TEX_MATCH_TOKEN)
							goto tcontinue;
					}
				}
				
				q = getAvail(); 
				link(p) = q; 
				info(q) = cur_tok;
  				p = q;
			}
			
			m++;
			if (info(r) > TEX_END_MATCH_TOKEN)
				goto tcontinue;
				
			if (info(r) < TEX_MATCH_TOKEN)
				goto tcontinue;
				
found:
			if (s != TEX_NULL)
			{
				if ((m == 1) && (info(p) < TEX_RIGHT_BRACE_LIMIT) && (p != temp_head))
				{
					link(rbrace_ptr) = TEX_NULL; 
					freeAvail(p);
  					p = link(temp_head); 
  					pstack[n] = link(p); 
  					freeAvail(p);
				}
				else
					pstack[n] = link(temp_head);
					
				n++;
				if (tracingMacros() > 0)
				{
					beginDiagnostic(); 
					printnl(match_chr); 
					printInt(n);
  					print("<-"); 
  					showTokenList(pstack[n - 1], TEX_NULL, 1000);
  					endDiagnostic(false);
				}
			}
		} while (info(r) != TEX_END_MATCH_TOKEN);
	}
	
	while ((state == TEX_TOKEN_LIST) && 
	       (loc == TEX_NULL) && 
	       (token_type != TEX_V_TEMPLATE))
	{
		endTokenList();
	}
	
	beginTokenList(ref_count, TEX_MACRO); 
	texname = warning_index; 
	loc  = link(r);
	if (n > 0)
	{
		if ((param_ptr + n) > max_param_stack)
		{
			max_param_stack = param_ptr + n;
			if (max_param_stack > param_size)
			{
				overFlow(tr("parameter stack size"), param_size);
				scanner_status = save_scanner_status; 
				warning_index  = save_warning_index;
				return ;
			}
		}
		
		for (m = 0; m < n; m++)
			param_stack[param_ptr+m] = pstack[m];
		param_ptr = param_ptr+n;
	}
	
	scanner_status = save_scanner_status; 
	warning_index  = save_warning_index;
}

bool XWTeX::newInputLine(QIODevice * f, qint32 themode)
{
	qint32 i = EOF;
	char c = EOF;
	last = first;
  otp_input_end = 0;
  if (themode == 1)
  {
   	while ((otp_input_end < ocp_buf_size) && 
    	       f->getChar((char*)(&c)) && 
    	       c != EOF &&
    	       c != '\r' && 
    	       c != '\n')
   	{
   		otp_input_buf[++otp_input_end] = c;
   		c = EOF;
   	}
    	
   	if (c == '\r')
   	{
   		if (f->getChar((char*)(&c)))
   		{
   			if (c != '\n')
   				f->ungetChar((char)c);
   		}
   	}
    	
   	i = c;
  }
  else if (themode == 2)
  {
   	while ((otp_input_end < ocp_buf_size) && 
   	       f->getChar((char*)(&c)) && 
   	       c != EOF &&
   	       c != 0x25)
   	{
   		otp_input_buf[++otp_input_end] = c;
   		c = EOF;
   	}
    	
   	i = c;
  }
  else if (themode == 3)
  {
   	while ((otp_input_end < ocp_buf_size) && 
  	       ((i = getcTwo(f)) != EOF) && 
   	       i != '\r' && 
   	       i != '\n')
   	{
   		otp_input_buf[++otp_input_end] = i;
   	}
    	
   	if (i == '\r')
   	{
   		i = getcTwo(f);
   		if (i != '\n')
   			ungetcTwo(i, f);
   	}
   }
   else
   {
   	while ((otp_input_end < ocp_buf_size) && 
   	       ((i = getcTwoLE(f)) != EOF) && 
   	       i != '\r' && 
   	       i != '\n')
   	{
   		otp_input_buf[++otp_input_end] = i;
   	}
    	
   	if (i == '\r')
   	{
   		i = getcTwoLE(f);
   		if (i != '\n')
   			ungetcTwoLE(i, f);
   	}
  }
    
  if (f->atEnd() && otp_input_end == 0)
     return false;
      
  if ((!(f->atEnd())) && 
    	(((themode != 2) && 
    	(i != '\n')) || ((themode == 2) && 
    	(i != 0x25))))
  {
   	overFlowOcpBufSize();
   	return false;
  }
   
  return true;
}

bool XWTeX::newInputLn(QIODevice * f, 
	                   qint32 themode, 
	                   qint32 translation, 
	                   bool bypass)
{
	return pnewInputLn(f, themode, translation, bypass);
}

bool XWTeX::pnewInputLn(QIODevice * f, 
	                    qint32 the_mode, 
	                    qint32 translation, 
	                    bool )
{
	if ((the_mode == TEX_NO_MODE) || 
		((the_mode == TEX_ONEBYTE_MODE) && (translation == 0)))
	{
		return inputLn(f);
	}
	else if (!newInputLine(f, the_mode))
		return false;
	else if (translation == 0)
	{
		if ((first + otp_input_end) >= buf_size)
		{
			overFlow(tr("buffer size"),  buf_size);
			return false;
		}
		
		state = TEX_NEW_LINE;
   		texstart = first;
   		last  = texstart;
   		loc   = texstart;
   		for (otp_counter = 1; otp_counter <= otp_input_end; otp_counter++)
   		{
   			buffer[last] = otp_input_buf[otp_counter];
      		last++;
   		}
   		
   		while (buffer[last - 1] == ' ')
   			last--;
   			
   		limit = last-1;
   		return true;
	}
	else
	{
		otp_input_start = 0;
   		otp_input_last = 0;
   		otp_stack_used = 0;
   		otp_stack_last = 0;
   		otp_stack_new = 0;
   		otp_output_end = 0;
   		otp_pc = 0;
   		otp_finished = false;
   		otp_calc_ptr = 0;
   		otp_calcs[otp_calc_ptr] = 0;
   		otp_state_ptr = 0;
   		otp_states[otp_state_ptr] = 0;
   		otp_input_ocp = translation;
   		
   		while (!otp_finished)
   		{
   			if (otp_pc >= ocp_state_no(otp_input_ocp, otp_cur_state))
   			{
   				printErr(tr("bad OCP program -- PC not valid"));
  				succumb();
  				return false;
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
						return false;
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
						return false;
					}
					otp_output_buf[otp_output_end] = otp_arg;
    				otp_pc++;
					break;
					
				case TEX_OTP_RIGHT_CHAR:
					if ((1 > otp_arg) || (otp_arg > otp_no_input_chars))
					{
						printErr(tr("right hand side of OCP expression is bad"));
  						succumb();
  						return false;
					}
					if (otp_arg > otp_stack_last)
						otp_calculated_char = otp_input_buf[otp_input_start + otp_arg - otp_stack_last];
					else
						otp_calculated_char = otp_stack_buf[otp_arg];
						
					otp_output_end++;
					if (otp_output_end > ocp_buf_size)
					{
						overFlowOcpBufSize();
						return false;
					}
					otp_output_buf[otp_output_end] = otp_calculated_char;
    				otp_pc++;
					break;
					
				case TEX_OTP_RIGHT_LCHAR:
					if ((1 > (otp_no_input_chars - otp_arg)) || ((otp_no_input_chars - otp_arg) > otp_no_input_chars))
					{
						printErr(tr("right hand side of OCP expression is bad"));
  						succumb();
  						return false;
					}
					if ((otp_no_input_chars - otp_arg) > otp_stack_last)
						otp_calculated_char = otp_input_buf[otp_input_start + (otp_no_input_chars - otp_arg) - otp_stack_last];
					else
						otp_calculated_char = otp_stack_buf[(otp_no_input_chars - otp_arg)];
					otp_output_end++;
					if (otp_output_end > ocp_buf_size)
					{
						overFlowOcpBufSize();
						return false;
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
  						return false;
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
  							return false;
						}
						if (otp_counter > otp_stack_last)
							otp_calculated_char = otp_input_buf[otp_input_start + otp_counter - otp_stack_last];
						else
							otp_calculated_char = otp_stack_buf[otp_counter];
						otp_output_end++;
						if (otp_output_end > ocp_buf_size)
						{
							overFlowOcpBufSize();
							return false;
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
						return false;
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
						return false;
					}
					otp_stack_buf[otp_stack_new] = otp_arg;
    				otp_pc++;
					break;
					
				case TEX_OTP_PBACK_CHAR:
					if ((1 > otp_arg) || (otp_arg > otp_no_input_chars))
					{
						printErr(tr("right hand side of OCP expression is bad"));
  						succumb();
  						return false;
					}
					if (otp_arg > otp_stack_last)
						otp_calculated_char = otp_input_buf[otp_input_start + otp_arg - otp_stack_last];
					else
						otp_calculated_char = otp_stack_buf[otp_arg];
					otp_stack_new++;
					if (otp_stack_new >= ocp_stack_size)
					{
						overFlowOcpStackSize();
						return false;
					}
					otp_stack_buf[otp_stack_new] = otp_calculated_char;
    				otp_pc++;
					break;
					
				case TEX_OTP_PBACK_LCHAR:
					if ((1 > (otp_no_input_chars - otp_arg)) || ((otp_no_input_chars - otp_arg) > otp_no_input_chars))
					{
						printErr(tr("right hand side of OCP expression is bad"));
  						succumb();
  						return false;
					}
					if ((otp_no_input_chars - otp_arg) > otp_stack_last)
						otp_calculated_char = otp_input_buf[otp_input_start + (otp_no_input_chars - otp_arg) - otp_stack_last];
					else
						otp_calculated_char = otp_stack_buf[(otp_no_input_chars - otp_arg)];
					otp_stack_new++;
					if (otp_stack_new >= ocp_stack_size)
					{
						overFlowOcpStackSize();
						return false;
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
  						return false;
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
  							return false;
						}
						if (otp_counter > otp_stack_last)
							otp_calculated_char = otp_input_buf[otp_input_start + otp_counter - otp_stack_last];
						else
							otp_calculated_char = otp_stack_buf[otp_counter];
							
						otp_stack_new++;
						if (otp_stack_new >= ocp_stack_size)
						{
							overFlowOcpStackSize();
							return false;
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
      					return false;
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
						return false;
					}
					otp_calcs[otp_calc_ptr] = otp_arg;
    				otp_pc++;
					break;
					
				case TEX_OTP_PUSH_CHAR:
					if ((1 > otp_arg) || (otp_arg > otp_no_input_chars))
					{
						printErr(tr("right hand side of OCP expression is bad"));
  						succumb();
  						return false;
					}
					if (otp_arg > otp_stack_last)
						otp_calculated_char = otp_input_buf[otp_input_start + otp_arg - otp_stack_last];
					else
						otp_calculated_char = otp_stack_buf[otp_arg];
					otp_calc_ptr++;
					if (otp_calc_ptr >= ocp_stack_size)
					{
						overFlowOcpStackSize();
						return false;
					}
					otp_calcs[otp_calc_ptr] = otp_calculated_char;
    				otp_pc++;
					break;
					
				case TEX_OTP_PUSH_LCHAR:
					if ((1 > (otp_no_input_chars - otp_arg)) || ((otp_no_input_chars - otp_arg) > otp_no_input_chars))
					{
						printErr(tr("right hand side of OCP expression is bad"));
  						succumb();
  						return false;
					}
					if ((otp_no_input_chars - otp_arg) > otp_stack_last)
						otp_calculated_char = otp_input_buf[otp_input_start + (otp_no_input_chars - otp_arg) - otp_stack_last];
					else
						otp_calculated_char = otp_stack_buf[(otp_no_input_chars - otp_arg)];
					otp_calc_ptr++;
					if (otp_calc_ptr >= ocp_stack_size)
					{
						overFlowOcpStackSize();
						return false;
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
    					return false;
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
  						return false;
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
  						return false;
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
  						return false;
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
  						return false;
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
  						return false;
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
  						return false;
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
    				return false;
					break;
			}
   		}
   		
   		if ((first + otp_output_end) >= buf_size)
   		{
   			overFlow(tr("buffer size"),  buf_size);
			return false;
   		}
   		
   		state = TEX_NEW_LINE;
   		texstart = first;
   		last  = texstart;
   		loc   = texstart;
   		for (otp_counter = 1; otp_counter <= otp_output_end; otp_counter++)
   		{
   			buffer[last] = otp_output_buf[otp_counter];
      		last++;
   		}
   		
   		while (buffer[last - 1] == ' ')
   			last--;
   			
   		limit = last - 1;
	}
	
	return true;
}

void XWTeX::pushInput()
{
	if (input_ptr > max_in_stack)
	{
		max_in_stack = input_ptr;
		if (input_ptr == stack_size)
		{
			overFlow(tr("input stack size"), stack_size);
			return ;
		}
	}
	input_stack[input_ptr] = cur_input;
	input_ptr++;
}

void XWTeX::readToks(qint32 n, qint32 r, qint32 j)
{
	scanner_status = TEX_DEFINING; 
	warning_index  = r;
	def_ref = getAvail(); 
	token_ref_count(def_ref) = TEX_NULL;
	qint32 p = def_ref;
	qint32 q = getAvail(); 
  	link(p) = q; 
  	info(q) = TEX_END_MATCH_TOKEN;
  	p = q;
  	qint16 m = 0;
  	if ((n < 0) || (n > 15))
  		m = 16;
  	else
  		m = n;
  		
  	qint32 s = align_state; 
  	align_state = 1000000;
  	do
  	{
  		beginFileReading(); 
  		texname = m + 1;
  		if (read_open[m] == TEX_CLOSED)
  		{
  			if (interaction > TEX_NONSTOP_MODE)
  			{
  				if (n < 0)
  					promptInput("");
  				else
  				{
  					println(); 
  					sprintCS(r); 
  					promptInput("="); 
  					n = -1;
  				}
  			}
  			else
  				fatalError(tr("*** (cannot \\read from terminal in nonstop modes)"));
  		}
  		else if (read_open[m] == TEX_JUST_OPEN)
  		{
  			read_file_mode[m] = getFileMode(read_file[m], equiv(TEX_OCP_INPUT_MODE_BASE));
  			if (read_file_mode[m] > 0)
  				read_file_translation[m] = equiv(TEX_OCP_INPUT_ONEBYTE_TRANSLATION_BASE + read_file_mode[m] - 1);
  			
  			if (newInputLn(read_file[m],read_file_mode[m], read_file_translation[m], false))
  				read_open[m] = TEX_NORMAL;
  			else
  			{
  				read_file[m]->close();
  				read_open[m] = TEX_CLOSED;
  			}
  		}
  		else
  		{
  			if (!newInputLn(read_file[m], read_file_mode[m], read_file_translation[m], true))
  			{
  				read_file[m]->close();
  				read_open[m] = TEX_CLOSED;
  				if (align_state != 1000000)
  				{
  					runaway();
  					printErr(tr("File ended within ")); 
  					printEsc(TeXRead);
  					help1(tr("This \\read has unbalanced braces."));
    				align_state = 1000000; 
    				error();
  				}
  			}
  		}
  		
  		limit = last;
  		if (endLineCharInactive())
  			limit--;
  		else
  			buffer[limit] = endLineChar();
  			
  		first = limit + 1; 
  		loc = texstart; 
  		state = TEX_NEW_LINE;
  		if (j == 1)
  		{
  			while (loc <= limit)
  			{
  				cur_chr = buffer[loc]; 
  				loc++;
  				if (cur_chr == ' ')
  					cur_tok = TEX_SPACE_TOKEN;
  				else
  					cur_tok = cur_chr + TEX_OTHER_TOKEN;
  					
  				q = getAvail(); 
  				link(p) = q; 
  				info(q) = cur_tok;
  				p = q;
  			}
  			
  			goto done;
  		}
  		
  		while (true)
  		{
  			getToken();
  			if (cur_tok == 0)
  				goto done;
  				
  			if (align_state < 1000000)
  			{
  				do
  				{
  					getToken();
  				} while (cur_tok != 0);
  				
  				align_state = 1000000; 
  				goto done;
  			}
  			
  			q = getAvail(); 
  			link(p) = q; 
  			info(q) = cur_tok;
  			p = q;
  		}
done: 
		endFileReading();
  	} while (align_state != 1000000);
  	
  	cur_val = def_ref; 
  	scanner_status = TEX_NORMAL; 
  	align_state = s;
}

void XWTeX::startEqNo()
{
	 saved(0) = cur_chr; 
	 save_ptr++;
	 pushMath(TEX_MATH_SHIFT_GROUP); 
	 eqWordDefine(TEX_INT_BASE + TEX_CUR_FAM_CODE, -1);
	 if (insert_src_special_every_math)
	 	insertSrcSpecial();
	 	
	 if (everyMath() != TEX_NULL)
	 	beginTokenList(everyMath(), TEX_EVERY_MATH_TEXT);
}

void XWTeX::startInput()
{
	scanFileName();	
	packCurName();
	while (true)
	{
		tex_input_type = 1;
		QString fn = getFileName();	
	  beginFileReading();	
	  cur_file = dev->openInput(fn);
	  if (cur_file)
	  {
	  	break;
	  }
	  	
	  endFileReading();
	  promptFileName(tr("input file name"), ".tex", false);
	  if (history == TEX_FATAL_ERROR_STOP)
	  	return ;
	}
	
	texname = makeNameString();
	source_filename_stack[in_open] = texname;
	full_source_filename_stack[in_open] = makeFullNameString();
	if (texname == (str_ptr - 1))
	{
		qint32 temp_str = searchString(texname);
		if (temp_str > 0)
		{
			texname = temp_str; 
			flushString();
		}
	}
	
	if (job_name == 0)
	{
		job_name = getJobName(); 
		openLogFile();
	}
	
	if (term_offset + length(texname) > max_print_line - 2)
		println();
	else if ((term_offset > 0) || (file_offset > 0))
		printChar(' ');
		
	printChar('('); 
	open_parens++; 
	slowPrint(texname);
	printChar(' ');
	state = TEX_NEW_LINE;
	
	line = 1;
	input_file_mode[index] = getFileMode(cur_file, equiv(TEX_OCP_INPUT_MODE_BASE));		
	if (input_file_mode[index] > 0)
	{
		input_file_translation[index] = 
     			equiv(TEX_OCP_INPUT_ONEBYTE_TRANSLATION_BASE + input_file_mode[index] - 1);
    }
        
    newInputLn(cur_file, input_file_mode[index], input_file_translation[index], false);
    firmUpTheLine();
    if (endLineCharInactive())
    	limit--;
    else
    	buffer[limit] = endLineChar();
    	
    first = limit + 1; 
    loc   = texstart;
}

qint32 XWTeX::theToks()
{
	qint32 p, q, r, b;
	qint32 c;
	
	if (odd(cur_chr))
	{
		c = cur_chr; 
		scanGeneralText();
		if (c == 1)
			return cur_val;
		else
		{
			old_setting = selector; 
			selector = TEX_NEW_STRING; 
			b = pool_ptr;
    		p = getAvail(); 
    		link(p) = link(temp_head);
    		tokenShow(p); 
    		flushList(p);
    		selector = old_setting; 
    		return strToks(b);
		}
	}
	
	getXToken(); 
	scanSomethingInternal(TEX_TOK_VAL, false);
	if (cur_val_level >= TEX_IDENT_VAL)
	{
		p = temp_head; 
		link(p) = TEX_NULL;
		if (cur_val_level == TEX_IDENT_VAL)
		{
			q = getAvail(); 
			link(p) = q; 
			info(q) = TEX_CS_TOKEN_FLAG + cur_val;
  			p = q;
		}
		else if (cur_val != TEX_NULL)
		{
			r = link(cur_val);
			while (r != TEX_NULL)
			{
				fastGetAvail(q); 
				link(p) = q; 
				info(q) = info(r);
  				p = q;
  				r = link(r);
			}
		}
		
		return p;
	}
	else
	{
		old_setting = selector; 
		selector = TEX_NEW_STRING; 
		b = pool_ptr;
		
		switch (cur_val_level)
		{
			case TEX_INT_VAL:
				printInt(cur_val);
				break;
				
  			case TEX_DIR_VAL:
  				printDir(cur_val);
  				break;
  				
  			case TEX_DIMEN_VAL:
  				printScaled(cur_val); 
  				print(TeXPT);
  				break;
  				
  			case TEX_GLUE_VAL: 
  				printSpec(cur_val, "pt"); 
  				deleteGlueRef(cur_val);
  				break;
  				
  			case TEX_MU_VAL: 
  				printSpec(cur_val, "mu"); 
  				deleteGlueRef(cur_val);
  				break;
  				
  			default:
  				break;
		}
		
		selector = old_setting;
	}
	
	return strToks(b);
}

void XWTeX::ungetcTwo(qint32 c, QIODevice * f)
{
	f->ungetChar(c & 0377);
    f->ungetChar(c >> 8);
}

void XWTeX::ungetcTwoLE(qint32 c, QIODevice * f)
{
	f->ungetChar(c >> 8);
	f->ungetChar(c & 0377);
}

void XWTeX::xToken()
{
	while (cur_cmd > TEX_MAX_COMMAND)
	{
		expand();
  		getNext();
	}
	
	if (cur_cs == 0)
		cur_tok = (cur_cmd * TEX_MAX_CHAR_VAL) + cur_chr;
	else
		cur_tok = TEX_CS_TOKEN_FLAG + cur_cs;
}

