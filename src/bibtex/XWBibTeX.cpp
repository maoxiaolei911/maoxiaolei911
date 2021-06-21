/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <QTextStream>
#include "XWUtil.h"
#include "XWFileName.h"
#include "XWOptionParser.h"
#include "XWTexSea.h"
#include "XWBibTeX.h"

XWBibTeX::XWBibTeX(QCoreApplication * app)
	:XWApplication(app)
{
	installTranslator("xw_bibtex_");
	
	log_file = 0;
	term_out = 0;
	zzzaa = 0;
	zzzab = 0;
	zzzac = 0;
	zzzad = 0;
	name_of_file = 0;
	buffer = 0;
	ex_buf = 0;
	out_buf = 0;
	sv_buffer = 0;
	str_pool = 0;
	str_start = 0;
	aux_file = 0;
	aux_list = 0;
	aux_ln_stack = 0;
	bbl_file = 0;
	bib_list = 0;
	bib_file = 0;
	cite_list = 0;
	zzzae = 0;
	wiz_functions = 0;
	entry_ints = 0;
	entry_strs = 0;
	global_strs = 0;
	field_info = 0;
	type_list = 0;
	entry_exists = 0;
	cite_info = 0;
	lit_stack = 0;
	lit_stk_type = 0;
	s_preamble = 0;
	name_tok = 0;
	name_sep_char = 0;
}

XWBibTeX::~XWBibTeX()
{
}

QString XWBibTeX::getProductID()
{
	return "xwbibtex";
}

QString XWBibTeX::getProductName()
{
	return tr("Xiuwen BibTeX");
}

const char * XWBibTeX::getProductName8()
{
	return "Xiuwen BibTeX";
}

QString  XWBibTeX::getVersion()
{
	return "0.9";
}

const char * XWBibTeX::getVersion8()
{
	return "0.9";
}

void XWBibTeX::start(int & argc, char**argv)
{
	QString logname = xwApp->getProductName8();
	logname.remove(QChar(' '));
	logname.append(".log");
	log_file = new QFile(logname);
	log_file->open(QIODevice::WriteOnly);
	term_out = new QFile;
	term_out->open(stderr, QIODevice::WriteOnly);
		
	outputPath = getEnv("OUTPUT_PATH");
	
	max_ent_ints = XW_BIBTEX_MAX_ENT_INTS;
	max_ent_strs = XW_BIBTEX_MAX_ENT_STRS;
	pool_size = XW_BIBTEX_POOL_SIZE;
	max_bib_files = XW_BIBTEX_MAX_BIB_FILES;
	max_fields = XW_BIBTEX_MAX_FIELDS;
	wiz_fn_space = XW_BIBTEX_WIZ_FN_SPACE;
		
	bib_file = (QFile**)malloc((max_bib_files + 2) * sizeof(QFile*));
	for (int i = 0; i < (max_bib_files + 2); i++)
		bib_file[i] = 0;
		
	bib_list = (int*)malloc((max_bib_files + 2) * sizeof(int));
	
	bst_file = 0;
	
	entry_ints = (int*)malloc((max_ent_ints + 2) * sizeof(int));
	entry_strs = (int*)malloc((max_ent_ints + 2) * (XW_BIBTEX_ENT_STR_SIZE + 2) * sizeof(int));
	wiz_functions = (int*)malloc((wiz_fn_space + 2) * sizeof(int));
	field_info = (int*)malloc((max_fields + 2) * sizeof(int));
	s_preamble = (int*)malloc((max_bib_files + 2) * sizeof(int));
	str_pool = (int*)malloc((pool_size + 2) * sizeof(int));
	
	initialize(argc, argv);
	println(tr("This is XWBibTeX, Version 0.9."));
	
	print(tr("The top-level auxiliary file: "));
	printAuxName();
	
	stoped = false;
	while (!stoped)
	{
		cur_aux_line++;
		if (!inputln(cur_aux_file))
		{
			popTheAuxStack();
			if (aux_ptr==0)
				break;
		}
		else
			getAuxCommandAndProcess();
	}
	
	lastCheckForAuxErrors();
	
	if (bst_str == 0)
		goto no_bst_file;
		
	bst_line_num = 0;
	bbl_line_num = 1;
	buf_ptr2 = last;
	while (true)
	{
		if (!eatBstWhiteSpace())
			break;
			getBstCommandAndProcess();
	}
	
	bst_file->close();
	delete bst_file;
	bst_file = 0;

no_bst_file:
	bbl_file->close();
	delete bbl_file;
	bbl_file = 0;
	
	cleanup();
}

void XWBibTeX::cleanup()
{
	if ((read_performed) && (! reading_completed))
	{
		QString str = QString(tr("'Aborted at line %1 of file ")).arg(bib_line_num);
		print (str);
    printBibName();
	}
	
	switch (history)
	{
		case XW_BIBTEX_SPOTLESS:
			break;
			
		case XW_BIBTEX_WARNING_MESSAGE:
			if (err_count == 1)
				println(tr("(There was 1 warning)"));
			else
			{
				QString str = QString(tr("(There were %1 warnings)")).arg(err_count);
				print(str);
			}
			break;
			
		case XW_BIBTEX_ERROR_MESSAGE:
			if (err_count == 1)
				println(tr("(There was 1 error message)"));
			else
			{
				QString str = QString(tr("(There were %1 error message)")).arg(err_count);
				print(str);
			}
			break;
			
		case XW_BIBTEX_FATAL_MESSAGE:
			println(tr("(That was a fatal error)"));
			break;
			
		default:
			print(tr("History is bunk"));
			printConfusion();
			break;
	}
	
	if (log_file)
	{
		log_file->close();
		delete log_file;
	}
	
	if (term_out)
	{
		term_out->close();
		delete term_out;
	}
	
	if (bst_file)
	{
		bst_file->close();
		delete bst_file;
	}
	
	if (bbl_file)
	{
		bbl_file->close();
		delete bbl_file;
	}
	
	if (bib_file)
	{
		for (int i = 0; i < max_bib_files; i++)
		{
			if (bib_file[i])
			{
				bib_file[i]->close();
				delete bib_file[i];
			}
		}
		
		free(bib_file);
	}
	
	if (aux_file)
	{
		for (int i = 0; i < XW_BIBTEX_AUX_STACK_SIZE; i++)
		{
			if (aux_file[i])
			{
				aux_file[i]->close();
				delete aux_file[i];
			}
		}
		free(aux_file);
	}
	
	if (zzzaa)
		free(zzzaa);
		
	if (zzzab)
		free(zzzab);
		
	if (zzzac)
		free(zzzac);
		
	if (zzzad)
		free(zzzad);
		
	if (zzzae)
		free(zzzae);
		
	if (name_of_file)
		free(name_of_file);
		
	if (buffer)
		free(buffer);
		
	if (ex_buf)
		free(ex_buf);
		
	if (out_buf)
		free(out_buf);
		
	if (sv_buffer)
		free(sv_buffer);
		
	if (str_pool)
		free(str_pool);
		
	if (str_start)
		free(str_start);
		
	if (aux_list)
		free(aux_list);
		
	if (aux_ln_stack)
		free(aux_ln_stack);
		
	if (bib_list)
		free(bib_list);
		
	if (cite_list)
		free(cite_list);
	
	if (wiz_functions)
		free(wiz_functions);
		
	if (entry_ints)
		free(entry_ints);
		
	if (entry_strs)
		free(entry_strs);
		
	for (int i = 0; i < 20; i++)
	{
		if (global_strs[i])
		{
			free(global_strs[i]);
		}
	}
	
	if (global_strs)
		free(global_strs);
		
	if (field_info)
		free(field_info);
		
	if (type_list)
		free(type_list);
		
	if (entry_exists)
		free(entry_exists);
		
	if (cite_info)
		free(cite_info);
		
	if (lit_stack)
		free(lit_stack);
		
	if (lit_stk_type)
		free(lit_stk_type);	
	
	if (s_preamble)
		free(s_preamble);
		
	if (name_tok)
		free(name_tok);
		
	if (name_sep_char)
		free(name_sep_char);
		
	XWApplication::cleanup();
}

void XWBibTeX::addArea(int area)
{
	name_ptr = name_length ;
  while ((name_ptr > 0)) 
  {      
    name_of_file[name_ptr + ( str_start[area + 1 ]- str_start[area ]) ]= name_of_file[name_ptr];
    name_ptr = name_ptr - 1 ;
  } 
  name_ptr = 1 ;
  int p_ptr = str_start[area ];
  while ((p_ptr < str_start[area + 1 ])) 
  {      
    name_of_file[name_ptr ]= str_pool[p_ptr ];
    name_ptr = name_ptr + 1 ;
    p_ptr = p_ptr + 1 ;
  } 
  name_length = name_length + (str_start[area + 1 ]- str_start[area ]) ;
}

void XWBibTeX::addBufPool(int p_str)
{
	p_ptr1 = str_start[p_str];
	p_ptr2 = str_start[p_str+1];
	if (ex_buf_length+(p_ptr2-p_ptr1) > XW_BIBTEX_BUF_SIZE)
	{
		bufferOverflow();
		return ;
	}
	
	ex_buf_ptr = ex_buf_length;
	while (p_ptr1 < p_ptr2)
	{
		ex_buf[ex_buf_ptr] = str_pool[p_ptr1];
		ex_buf_ptr++;
		p_ptr1++;
	}
	
	ex_buf_length = ex_buf_ptr;
}

void XWBibTeX::addDatabaseCite(int & new_cite)
{
	checkCiteOverflow(new_cite);
	checkFieldOverflow(num_fields*new_cite);
	cite_list[new_cite] = hash_text[cite_loc];
	ilk_info[cite_loc] = new_cite;
	ilk_info[lc_cite_loc] = cite_loc;
	new_cite++;
}

void XWBibTeX::addExtension(int ext)
{
	name_ptr = name_length + 1 ;
  int p_ptr = str_start[ext];
  while ((p_ptr < str_start[ext + 1])) 
  {      
    name_of_file[name_ptr ]= str_pool[p_ptr];
    name_ptr = name_ptr + 1 ;
    p_ptr = p_ptr + 1 ;
  } 
  name_length = name_length + ( str_start[ext + 1 ]- str_start[ext ]) ;
  name_of_file [name_length + 1 ]= 0 ;
}

void XWBibTeX::addOutPool(int p_str)
{
	p_ptr1 = str_start[p_str];
	p_ptr2 = str_start[p_str+1];
	if (out_buf_length+(p_ptr2-p_ptr1) > XW_BIBTEX_BUF_SIZE)
	{
		overflow(tr("output buffer size "),XW_BIBTEX_BUF_SIZE);
		return ;
	}
	
	out_buf_ptr = out_buf_length;
	while (p_ptr1 < p_ptr2)
	{
		out_buf[out_buf_ptr] = str_pool[p_ptr1];
		p_ptr1++;
		out_buf_ptr++;
	}
	
	out_buf_length = out_buf_ptr;
	int end_ptr = 0;
	int break_ptr = 0;
	while (out_buf_length > XW_BIBTEX_MAX_PRINT_LINE)
	{
		end_ptr = out_buf_length;
		out_buf_ptr = XW_BIBTEX_MAX_PRINT_LINE;
		while ((lex_class[out_buf[out_buf_ptr]] != XW_BIBTEX_WHITE_SPACE) && 
           (out_buf_ptr >= XW_BIBTEX_MIN_PRINT_LINE))
		{
			out_buf_ptr--;
		}
		
		if (out_buf_ptr == XW_BIBTEX_MIN_PRINT_LINE-1)
		{
			out_buf[end_ptr] = out_buf[XW_BIBTEX_MAX_PRINT_LINE-1];
			out_buf[XW_BIBTEX_MAX_PRINT_LINE-1] = XW_BIBTEX_COMMENT;
			out_buf_length = XW_BIBTEX_MAX_PRINT_LINE;
			break_ptr = out_buf_length - 1;
			outputBblLine();
			out_buf[XW_BIBTEX_MAX_PRINT_LINE-1] = out_buf[end_ptr];
			out_buf_ptr = 0;
			tmp_ptr = break_ptr;
			while (tmp_ptr < end_ptr)
			{
				out_buf[out_buf_ptr] = out_buf[tmp_ptr];
				out_buf_ptr++;
				tmp_ptr++;
			}
			out_buf_length = end_ptr - break_ptr;
		}
		else
		{
			out_buf_length = out_buf_ptr;
    	break_ptr = out_buf_length + 1;
    	outputBblLine();
    	out_buf[0] = XW_BIBTEX_SPACE;
    	out_buf[1] = XW_BIBTEX_SPACE;
    	out_buf_ptr = 2;
    	tmp_ptr = break_ptr;
    	while (tmp_ptr < end_ptr)
    	{
    		out_buf[out_buf_ptr] = out_buf[tmp_ptr];
    		out_buf_ptr++;
    		tmp_ptr++;
    	}
    	out_buf_length = end_ptr - break_ptr + 2;
		}
	}
}

void XWBibTeX::addPoolBufAndPush()
{
	strRoom(ex_buf_length);
	ex_buf_ptr = 0;
	while (ex_buf_ptr < ex_buf_length)
	{
		appendChar(ex_buf[ex_buf_ptr]);
    ex_buf_ptr++;
	}
	pushLitStk(makeString(), XW_BIBTEX_STK_STR);
}

void XWBibTeX::alreadySeenFunctionPrint(int seen_fn_loc)
{
	printPoolStr(hash_text[seen_fn_loc]);
	print(tr(" is already a type ,"));
	printFnClass(seen_fn_loc);
	println(tr("' function name"));
	bstErrPrintAndLookForBlankLine();
}

void XWBibTeX::auxBibDataCommand()
{
	if (bib_seen)
	{
		auxErrIllegalAnotherPrint(XW_BIBTEX_N_AUX_BIBDATA);
		auxErrPrint();
		return ;
	}
	
	bib_seen = true;
	while (scan_char != XW_BIBTEX_RIGHT_BRACE)
	{
		buf_ptr2++;
		if (!scan2White(XW_BIBTEX_RIGHT_BRACE, XW_BIBTEX_COMMA))
		{
			auxErrNoRightBracePrint();
			auxErrPrint();
			return ;
		}
		
		if (lex_class[scan_char] == XW_BIBTEX_WHITE_SPACE)
		{
			auxErrWhiteSpaceInArgumentPrint();
			auxErrPrint();
			return ;
		}
		
		if ((last > buf_ptr2+1) && (scan_char == XW_BIBTEX_RIGHT_BRACE))
		{
			auxErrStuffAfterRightBracePrint();
			auxErrPrint();
			return ;
		}
		
		if (bib_ptr == max_bib_files)
		{
			int * tmp_num = (int*)malloc((max_bib_files + XW_BIBTEX_MAX_BIB_FILES) * sizeof(int));
			for (int i = 0; i < max_bib_files; i++)
				tmp_num[i] = bib_list[i];				
			free(bib_list);
			bib_list = tmp_num;
			
			QFile** tmp_file = (QFile**)malloc((max_bib_files + XW_BIBTEX_MAX_BIB_FILES) * sizeof(QFile*));
			for (int i = 0; i < (max_bib_files + XW_BIBTEX_MAX_BIB_FILES); i++)
				tmp_file[i] = 0;				
			for (int i = 0; i < max_bib_files; i++)
				tmp_file[i] = bib_file[i];				
			free(bib_file);
			bib_file = tmp_file;
			
			tmp_num = (int*)malloc((max_bib_files + XW_BIBTEX_MAX_BIB_FILES) * sizeof(int));
			for (int i = 0; i < max_bib_files; i++)
				tmp_num[i] = s_preamble[i];				
			free(s_preamble);
			s_preamble = tmp_num;
			max_bib_files += XW_BIBTEX_MAX_BIB_FILES;
		}
		
		cur_bib_str = hash_text[strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BIB_FILE_ILK,true)];
		if (hash_found)
		{
			print(tr("This database file appears more than once: "));
			printBibName();
			auxErrPrint();	
			return ;
		}
		
		startName(cur_bib_str);
		QString bibfilename = getFileName();
		XWBibTexSea sea;
		bibfilename = sea.findFile(bibfilename, XWBibTexSea::BIB, false);
		cur_bib_file = new QFile(bibfilename);
		if (bibfilename.isEmpty() || !cur_bib_file->open(QIODevice::ReadOnly | QIODevice::Text))
		{
			delete cur_bib_file;
			cur_bib_file = 0;
			print(tr("I couldn't open database file "));
			printBibName();
			auxErrPrint();	
			return ;
		}
		
		bib_ptr++;
	}
}

void XWBibTeX::auxBibStyleCommand()
{
	if (bst_seen)
	{
		auxErrIllegalAnotherPrint(XW_BIBTEX_N_AUX_BIBSTYLE);
		auxErrPrint();
		return ;
	}
	
	bst_seen = true;
	buf_ptr2++;
	if (!scan1White(XW_BIBTEX_RIGHT_BRACE))
	{
		auxErrNoRightBracePrint();
		auxErrPrint();
		return ;
	}
	
	if (lex_class[scan_char] == XW_BIBTEX_WHITE_SPACE)
	{
		auxErrWhiteSpaceInArgumentPrint();
		auxErrPrint();
		return ;
	}
	
	if (last > buf_ptr2+1)
	{
		auxErrStuffAfterRightBracePrint();
		auxErrPrint();
		return ;
	}
	
	bst_str = hash_text[strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BST_FILE_ILK,true)];
	if (hash_found)
	{
		confusion(tr("Already encountered style file"));
		return ;
	}
	
	startName(bst_str);
	QString bstfilename = getFileName();
	XWBibTexSea sea;
	bstfilename = sea.findFile(bstfilename, XWBibTexSea::BST, false);
	bst_file = new QFile(bstfilename);
	if (bstfilename.isEmpty() || !bst_file->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		delete bst_file;
		bst_file = 0;
		bst_str = 0;
		print(tr("I couldn''t open style file "));
    printBstName();
    auxErrPrint();
		return ;
	}
	
	print(tr("The style file: "));
	printBstName();
}

void XWBibTeX::auxCitationCommand()
{
	citation_seen = true;
	while (scan_char != XW_BIBTEX_RIGHT_BRACE)
	{
		buf_ptr2++;
		if (!scan2White(XW_BIBTEX_RIGHT_BRACE,XW_BIBTEX_COMMA))
		{
			auxErrNoRightBracePrint();
			auxErrPrint();
			return ;
		}
		
		if (lex_class[scan_char] == XW_BIBTEX_WHITE_SPACE)
		{
			auxErrWhiteSpaceInArgumentPrint();
			auxErrPrint();
			return ;
		}
		
		if ((last > buf_ptr2+1) && (scan_char == XW_BIBTEX_RIGHT_BRACE))
		{
			auxErrStuffAfterRightBracePrint();
			auxErrPrint();
			return ;
		}
		
		if (token_len == 1)
		{
			if (buffer[buf_ptr1] == XW_BIBTEX_STAR)
			{
				if (all_entries)
				{
					println(tr("Multiple inclusions of entire database"));
					auxErrPrint();
					return ;
				}
				else
				{
					all_entries = true;
        	all_marker = cite_ptr;
        	goto next_cite;
				}
			}
		}
		
		tmp_ptr = buf_ptr1;
		while (tmp_ptr < buf_ptr2)
		{
			ex_buf[tmp_ptr] = buffer[tmp_ptr];
    	tmp_ptr++;
		}
		
		lowerCase(ex_buf, buf_ptr1, token_len);
		lc_cite_loc = strLookup(ex_buf,buf_ptr1,token_len,XW_BIBTEX_LC_CITE_ILK,true);
		if (hash_found)
		{
			dummy_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_CITE_ILK,false);
			if (!hash_found)
			{
				print(tr("Case mismatch error between cite keys "));
    		printToken();
    		print(tr(" and "));
    		printPoolStr(cite_list[ilk_info[ilk_info[lc_cite_loc]]]);
    		printNewLine();
    		auxErrPrint();
				return ;
			}
		}
		else
		{
			cite_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_CITE_ILK,true);
			if (hash_found)
			{
				hashCiteConfusion();
				return ;
			}
			
			checkCiteOverflow(cite_ptr);
			cur_cite_str = hash_text[cite_loc];
			ilk_info[cite_loc] = cite_ptr;
			ilk_info[lc_cite_loc] = cite_loc;
			cite_ptr++;
		}
next_cite:
		;
	}
}

void XWBibTeX::auxEnd1ErrPrint()
{
	print(tr("I found no "));
}

void XWBibTeX::auxEnd2ErrPrint()
{
	print(tr("---while reading file "));
	printAuxName();
	markError();
}

void XWBibTeX::auxErrIllegalAnotherPrint(int cmd_num)
{
	print(tr("Illegal, another \\bib"));
	switch (cmd_num)
	{
		case XW_BIBTEX_N_AUX_BIBDATA:
			print(tr("data"));
			break;
			
		case XW_BIBTEX_N_AUX_BIBSTYLE:
			print(tr("style"));
			break;
			
		default:
			confusion(tr("Illegal auxiliary-file command"));
			break;
	}
	
	print(tr(" command"));
}

void XWBibTeX::auxErrNoRightBracePrint()
{
	print(tr("No '}' "));
}

void XWBibTeX::auxErrPrint()
{
	QString str = QString(tr("---line %1 of file ")).arg(cur_aux_line);
	print(str);
	printAuxName();
	printBadInputLine();
	printSkippingWhateverRemains();
	println(tr("command"));
}

void XWBibTeX::auxErrStuffAfterRightBracePrint()
{
	print(tr("Stuff after '}' "));
}

void XWBibTeX::auxErrWhiteSpaceInArgumentPrint()
{
	print(tr("White space in argument"));
}

void XWBibTeX::auxInputCommand()
{
	buf_ptr2++;
	if (!scan1White(XW_BIBTEX_RIGHT_BRACE))
	{
		auxErrNoRightBracePrint();
		auxErrPrint();
		return ;
	}
	
	if (lex_class[scan_char] == XW_BIBTEX_WHITE_SPACE)
	{
		auxErrWhiteSpaceInArgumentPrint();
		auxErrPrint();
		return ;
	}
	
	if (last > buf_ptr2+1)
	{
		auxErrStuffAfterRightBracePrint();
		auxErrPrint();
		return ;
	}
	
	aux_ptr++;
	if (aux_ptr == XW_BIBTEX_AUX_STACK_SIZE)
	{
		printToken(); 
		print(": ");
    overflow(tr("auxiliary file depth "),XW_BIBTEX_AUX_STACK_SIZE);
    return ;
	}
	
	bool aux_extension_ok = true;
	if (token_len < length(s_aux_extension))
		aux_extension_ok = false;
	else if (!strEqBuf(s_aux_extension, buffer, buf_ptr2-length(s_aux_extension), length(s_aux_extension)))
		aux_extension_ok = false;
		
	if (!aux_extension_ok)
	{
		printToken();
    print(tr(" has a wrong extension"));
    aux_ptr--;
    auxErrPrint();
		return ;
	}
	
	cur_aux_str = hash_text[strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_AUX_FILE_ILK,true)];
	if (hash_found)
	{
		print(tr("Already encountered file "));
    printAuxName();
    aux_ptr--;
    auxErrPrint();
		return ;
	}
	
	startName(cur_aux_str);
	name_ptr = name_length+1;
	name_of_file[name_ptr ]= 0 ;
	QString auxfilename = getFileName();
	if (!auxfilename.endsWith(".aux"))
		auxfilename += ".aux";
		
	if (!outputPath.isEmpty())
		auxfilename = QString("%1/%2").arg(outputPath).arg(auxfilename);
		
	cur_aux_file = new QFile(auxfilename);
  if (!cur_aux_file->open(QIODevice::ReadOnly | QIODevice::Text))
  {
  	delete cur_aux_file;
  	cur_aux_file = 0;
  	print(tr("I couldn''t open auxiliary file "));
    printAuxName();
    aux_ptr--;
    auxErrPrint();
		return ;
  }
  
  QString str = QString(tr("A level-%1 auxiliary file:")).arg(aux_ptr);
  print(str);
  printAuxName();
	cur_aux_line = 0;
}

bool XWBibTeX::badArgumentToken()
{
	lowerCase(buffer, buf_ptr1, token_len);
	fn_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BST_FN_ILK,false);
	if (!hash_found)
	{
		printToken();
		print(tr(" is an unknown function"));
		bstErrPrintAndLookForBlankLine();
		return true;
	}
	else if ((fn_type[fn_loc] != XW_BIBTEX_BUILT_IN) &&
         (fn_type[fn_loc] != XW_BIBTEX_WIZ_DEFINED))
	{
		printToken();
    print(tr(" has bad function type "));
    printFnClass(fn_loc);
    bstErrPrintAndLookForBlankLine();
		return true;
	}
	
	return false;
}

void XWBibTeX::badCrossReferencePrint(int s)
{
	print(tr("--entry '"));
	printPoolStr(cur_cite_str);
	println("'");
	print(tr("refers to entry '"));
	printPoolStr(s);
}

void XWBibTeX::bibCmdConfusion()
{
	confusion(tr("Unknown database-file command"));
}

void XWBibTeX::bibEqualsSignPrint()
{
	print(tr("I was expecting an '='"));
	bibErrPrint();
}

void XWBibTeX::bibErrPrint()
{
	print("-");
	bibLnNumPrint();
	printBadInputLine();
	printSkippingWhateverRemains();
	if (at_bib_command)
		println(tr("command"));
	else
		println(tr("entry"));
}

void XWBibTeX::bibFieldTooLongPrint()
{
	QString str = QString(tr("Your field is more than %1 characters")).arg(XW_BIBTEX_BUF_SIZE);
	print(str);
	bibErrPrint();
}

void XWBibTeX::bibIdentifierScanCheck(const QString & str)
{
	if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) || 
			(scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
	{
		;
	}
	else
	{
		bibIdPrint();
		print(str);
		bibErrPrint();
	}
}

void XWBibTeX::bibIdPrint()
{
	if (scan_result == XW_BIBTEX_ID_NULL)
		print(tr("You're missing "));
	else if (scan_result == XW_BIBTEX_OTHER_CHAR_ADJACENT)
	{
		QString str = QString(tr("'%1' immediately follows ")).arg((char)(xchr[scan_char]));
		print(str);
	}
	else
		idScanningConfusion();
}

void XWBibTeX::bibLnNumPrint()
{
	QString str = QString(tr("--line %1 of file ")).arg(bib_line_num);
	print(str);
}

void XWBibTeX::bibOneOfTwoPrint(int char1, int char2)
{
	QString str = QString(tr("I was expecting a '%1' or '%2'")).arg((char)char1).arg((char)char2);
	print(str);
	bibErrPrint();
}

void XWBibTeX::bibUnbalancedBracesPrint()
{
	print(tr("Unbalanced braces"));
	bibErrPrint();
}

void XWBibTeX::bibWarnPrint()
{
	bibLnNumPrint();
	markWarning();
}

void XWBibTeX::braceLvlOneLettersComplaint()
{
	print(tr("The format string '"));
	printPoolStr(pop_lit1);
	print(tr("' has an illegal brace-level-1 letter"));
	bstExWarnPrint() ;
}

void XWBibTeX::bracesUnbalancedComplaint(int pop_lit_var)
{
	print(tr("Warning--'"));
	printPoolStr(pop_lit_var);
	print(tr("' isn't a brace-balanced string"));
	bstMildExWarnPrint() ;
}

void XWBibTeX::bstCantMessWithEntriesPrint()
{
	print(tr("You can't mess with entries here"));
	bstExWarnPrint () ;
}

void XWBibTeX::bstEntryCommand()
{
	if (entry_seen)
	{
		print(tr("Illegal, another entry command"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	entry_seen = true;
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("entry"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_LEFT_BRACE)
	{
		bstLeftBracePrint();
		print(tr("entry"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("entry"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	while (scan_char != XW_BIBTEX_RIGHT_BRACE)
	{
		scanIdentifier(XW_BIBTEX_RIGHT_BRACE,XW_BIBTEX_COMMENT,XW_BIBTEX_COMMENT);
		if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) || 
			  (scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
		{
			;
		}
		else
		{
			bstIdPrint();
			eatBstPrint();
			print(tr("entry"));
			bstErrPrintAndLookForBlankLine();
			return ;
		}
		
		lowerCase(buffer, buf_ptr1, token_len); 
		fn_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BST_FN_ILK,true);
		if (hash_found)
		{
			alreadySeenFunctionPrint(fn_loc);
      return;
		}
		
		fn_type[fn_loc] = XW_BIBTEX_FIELD;
		fn_info[fn_loc] = num_fields;
		num_fields++;
		if (!eatBstWhiteSpace())
		{
			eatBstPrint();
			print(tr("entry"));
			bstErrPrintAndLookForBlankLine();
			return ;
		}
	}	
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("entry"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (num_fields == num_pre_defined_fields)
	{
		print(tr("Warning--I didn't find any fields"));
    bstWarnPrint();
	}
	
	if (scan_char != XW_BIBTEX_LEFT_BRACE)
	{
		bstLeftBracePrint();
		print(tr("entry"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("entry"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	while (scan_char != XW_BIBTEX_RIGHT_BRACE)
	{
		scanIdentifier(XW_BIBTEX_RIGHT_BRACE,XW_BIBTEX_COMMENT,XW_BIBTEX_COMMENT);
		if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) || 
			  (scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
		{
			;
		}
		else
		{
			bstIdPrint();
			eatBstPrint();
			print(tr("entry"));
			bstErrPrintAndLookForBlankLine();
			return ;
		}
		
		lowerCase(buffer, buf_ptr1, token_len);
		fn_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BST_FN_ILK,true);
		if (hash_found)
		{
			alreadySeenFunctionPrint(fn_loc);
      return;
		}
		
		fn_type[fn_loc] = XW_BIBTEX_INT_ENTRY_VAR;
		fn_info[fn_loc] = num_ent_ints;
		num_ent_ints++;
		if (!eatBstWhiteSpace())
		{
			eatBstPrint();
			print(tr("entry"));
			bstErrPrintAndLookForBlankLine();
			return ;
		}
	}
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("entry"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_LEFT_BRACE)
	{
		bstLeftBracePrint();
		print(tr("entry"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("entry"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	while (scan_char != XW_BIBTEX_RIGHT_BRACE)
	{
		scanIdentifier(XW_BIBTEX_RIGHT_BRACE,XW_BIBTEX_COMMENT,XW_BIBTEX_COMMENT);
		if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) || 
			  (scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
		{
			;
		}
		else
		{
			bstIdPrint();
			eatBstPrint();
			print(tr("entry"));
			bstErrPrintAndLookForBlankLine();
			return ;
		}
		
		lowerCase(buffer, buf_ptr1, token_len);
		fn_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BST_FN_ILK,true);
		if (hash_found)
		{
			alreadySeenFunctionPrint(fn_loc);
      return;
		}
		fn_type[fn_loc] = XW_BIBTEX_STR_ENTRY_VAR;
		fn_info[fn_loc] = num_ent_strs;
		num_ent_strs++;
	}
}

void XWBibTeX::bstErrPrintAndLookForBlankLine()
{
	print("-");
	bstLnNumPrint();
	printBadInputLine();
	while (last != 0)
	{
		if (!inputln(bst_file))
			return ;
			
		bst_line_num++;
	}
	
	buf_ptr2 = last;
}

void XWBibTeX::bstExecuteCommand()
{
	if (!read_seen)
	{
		print(tr("Illegal, execute command before read command"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("execute"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_LEFT_BRACE)
	{
		bstLeftBracePrint();
		print(tr("execute"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("execute"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	scanIdentifier(XW_BIBTEX_RIGHT_BRACE,XW_BIBTEX_COMMENT,XW_BIBTEX_COMMENT);
	if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) || 
		  (scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
	{
		;
	}
	else
	{
		bstIdPrint();
		eatBstPrint();
		print(tr("execute"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (badArgumentToken())
		return ;
		
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("execute"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_RIGHT_BRACE)
	{
		bstRightBracePrint();
		print(tr("execute"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	initCommandExecution();
	mess_with_entries = false;
	executeFn(fn_loc);
	checkCommandExecution();
}

void XWBibTeX::bstExWarnPrint()
{
	if (mess_with_entries)
	{
		print(tr(" for entry "));
    printPoolStr(cur_cite_str);
	}
	printNewLine();
	print(tr("while executing-"));
	bstLnNumPrint();
	markError();
}

void XWBibTeX::bstFunctionCommand()
{
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("function"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_LEFT_BRACE)
	{
		bstLeftBracePrint();
		print(tr("function"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("function"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	scanIdentifier(XW_BIBTEX_RIGHT_BRACE,XW_BIBTEX_COMMENT,XW_BIBTEX_COMMENT);
	if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) || 
		  (scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
	{
		;
	}
	else
	{
		bstIdPrint();
		eatBstPrint();
		print(tr("function"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	lowerCase(buffer, buf_ptr1, token_len);
	wiz_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BST_FN_ILK,true);
	if (hash_found)
	{
		alreadySeenFunctionPrint(wiz_loc);
     return;
	}
	
	fn_type[wiz_loc] = XW_BIBTEX_WIZ_DEFINED;
	if (hash_text[wiz_loc] == s_default)
		b_default = wiz_loc;
		
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("function"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_RIGHT_BRACE)
	{
		bstRightBracePrint();
		print(tr("function"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("function"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_LEFT_BRACE)
	{
		bstLeftBracePrint();
		print(tr("function"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	scanFnDef(wiz_loc);
}

void XWBibTeX::bstIdPrint()
{
	if (scan_result == XW_BIBTEX_ID_NULL)
	{
		QString str = QString(tr("'%1' begins identifier, command: ")).arg((char)(xchr[scan_char]));
		print(str);
	}
	else if (scan_result == XW_BIBTEX_OTHER_CHAR_ADJACENT)
	{
		QString str = QString(tr("'%1' immediately follows identifier, command: ")).arg((char)(xchr[scan_char]));
		print(str);
	}
	else
		idScanningConfusion();
}

void XWBibTeX::bstIntegersCommand()
{
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("integers"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_LEFT_BRACE)
	{
		bstLeftBracePrint();
		print(tr("integers"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("integers"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	while (scan_char != XW_BIBTEX_RIGHT_BRACE)
	{
		scanIdentifier(XW_BIBTEX_RIGHT_BRACE,XW_BIBTEX_COMMENT,XW_BIBTEX_COMMENT);
		if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) || 
		  (scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
		{
			;
		}
		else
		{
			bstIdPrint();
			eatBstPrint();
			print(tr("integers"));
			bstErrPrintAndLookForBlankLine();
			return ;
		}
		
		lowerCase(buffer, buf_ptr1, token_len);
		fn_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BST_FN_ILK,true);
		if (hash_found)
		{
			alreadySeenFunctionPrint(fn_loc);
      return;
		}
		
		fn_type[fn_loc] = XW_BIBTEX_INT_GLOBAL_VAR;
		fn_info[fn_loc] = 0;
		
		if (!eatBstWhiteSpace())
		{
			eatBstPrint();
			print(tr("integers"));
			bstErrPrintAndLookForBlankLine();
			return ;
		}
	}
	
	buf_ptr2++;
}

void XWBibTeX::bstIterateCommand()
{
	if (!read_seen)
	{
		print(tr("Illegal, iterate command before read command"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("iterate"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_LEFT_BRACE)
	{
		bstLeftBracePrint();
		print(tr("iterate"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("iterate"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	scanIdentifier(XW_BIBTEX_RIGHT_BRACE,XW_BIBTEX_COMMENT,XW_BIBTEX_COMMENT);
	if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) || 
			(scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
	{
		;
	}
	else
	{
		bstIdPrint();
		eatBstPrint();
		print(tr("integers"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (badArgumentToken())
		return ;
		
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("iterate"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_RIGHT_BRACE)
	{
		bstRightBracePrint();
		print(tr("iterate"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	initCommandExecution();
	mess_with_entries = true;
	sort_cite_ptr = 0;
	while (sort_cite_ptr < num_cites)
	{
		cite_ptr = cite_info[sort_cite_ptr];
		executeFn(fn_loc);
    checkCommandExecution();
    sort_cite_ptr++;
	}
}

void XWBibTeX::bstLeftBracePrint()
{
	print(tr("'{' is missing in command: "));
}

void XWBibTeX::bstLnNumPrint()
{
	QString str = QString(tr("--line %1 of file ")).arg(bst_line_num);
	print(str);
	printBstName();
}

void XWBibTeX::bstMacroCommand()
{
	if (read_seen)
	{
		print(tr("Illegal, macro command after read command"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("macro"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_LEFT_BRACE)
	{
		bstLeftBracePrint();
		print(tr("macro"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("macro"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	scanIdentifier(XW_BIBTEX_RIGHT_BRACE,XW_BIBTEX_COMMENT,XW_BIBTEX_COMMENT);
	if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) || 
			(scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
	{
		;
	}
	else
	{
		bstIdPrint();
		eatBstPrint();
		print(tr("macro"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	lowerCase(buffer, buf_ptr1, token_len);
	macro_name_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_MACRO_ILK,true);
	if (hash_found)
	{
		printToken();
		print(tr(" is already defined as a macro"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	ilk_info[macro_name_loc] = hash_text[macro_name_loc];
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("macro"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_RIGHT_BRACE)
	{
		bstRightBracePrint();
		print(tr("macro"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("macro"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_LEFT_BRACE)
	{
		bstLeftBracePrint();
		print(tr("macro"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("macro"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_DOUBLE_QUOTE)
	{
		print(tr("A macro definition must be '\"'-delimited"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	buf_ptr2++;
	if (!scan1(XW_BIBTEX_DOUBLE_QUOTE))
	{
		print(tr("There''s no '\"' to end macro definition"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	macro_def_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_TEXT_ILK,true);
	fn_type[macro_def_loc] = XW_BIBTEX_STR_LITERAL;
	ilk_info[macro_name_loc] = hash_text[macro_def_loc];
	buf_ptr2++;
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("macro"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_RIGHT_BRACE)
	{
		bstRightBracePrint();
		print(tr("iterate"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
}

void XWBibTeX::bstMildExWarnPrint()
{
	if (mess_with_entries)
	{
		print(tr(" for entry "));
    printPoolStr(cur_cite_str);
	}
	printNewLine();
	print(tr("while executing"));
	bstLnNumPrint() ;
  markError() ;
}

void XWBibTeX::bstReadCommand()
{
	if (read_seen)
	{
		print(tr("Illegal, another read command"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	read_seen = true;
	if (!entry_seen)
	{
		print(tr("Illegal, read command before entry command"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	sv_ptr1 = buf_ptr2;
	sv_ptr2 = last;
	tmp_ptr = sv_ptr1;
	while (tmp_ptr < sv_ptr2)
	{
		sv_buffer[tmp_ptr] = buffer[tmp_ptr];
    tmp_ptr++;
	}
	
	checkFieldOverflow(num_fields*num_cites);
	field_ptr = 0;
	while (field_ptr < max_fields)
	{
		field_info[field_ptr] = XW_BIBTEX_MISSING;
    field_ptr++;
	}
	cite_ptr = 0;
	while (cite_ptr < XW_BIBTEX_MAX_CITES)
	{
		type_list[cite_ptr] = XW_BIBTEX_EMPTY;
		cite_info[cite_ptr] = XW_BIBTEX_ANY_VALUE;
		cite_ptr++;
	}
	old_num_cites = num_cites;
	if (all_entries)
	{
		cite_ptr = all_marker;
		while (cite_ptr < old_num_cites)
		{
			cite_info[cite_ptr] = cite_list[cite_ptr];
      entry_exists[cite_ptr] = false;
      cite_ptr++;
		}
		cite_ptr = all_marker; 
	}
	else
	{
		cite_ptr = num_cites;
		all_marker = XW_BIBTEX_ANY_VALUE;
	}
	
	read_performed = true;
	bib_ptr = 0;
	while (bib_ptr < num_bib_files)
	{
		QString str = QString(tr("Database file #%1: ")).arg((char)(bib_ptr+1));
		print(str);
    printBibName();
    bib_line_num = 0;
    buf_ptr2 = last;
    while (!cur_bib_file->atEnd())
    	getBibCommandOrEntryAndProcess();
    cur_bib_file->close();
    delete cur_bib_file;
    cur_bib_file = 0;
    bib_ptr++;
	}
	
	reading_completed = true;
	num_cites = cite_ptr;
	num_preamble_strings = preamble_ptr;
	if ((num_cites - 1) * num_fields + crossref_num >= max_fields)
	{
		confusion(tr("field_info index is out of range"));
		return ;
	}
	
	cite_ptr = 0;
	while (cite_ptr < num_cites)
	{
		field_ptr = cite_ptr * num_fields + crossref_num;
    if (field_info[field_ptr] != XW_BIBTEX_MISSING)
    {
    	if (findCiteLocsForThisCiteKey(field_info[field_ptr]))
    	{
    		cite_loc = ilk_info[lc_cite_loc];
        field_info[field_ptr] = hash_text[cite_loc];
        cite_parent_ptr = ilk_info[cite_loc];
        field_ptr = cite_ptr * num_fields + num_pre_defined_fields;
        field_end_ptr = field_ptr - num_pre_defined_fields + num_fields;
        field_parent_ptr = cite_parent_ptr * num_fields + num_pre_defined_fields;
        while (field_ptr < field_end_ptr)
        {
        	if (field_info[field_ptr] == XW_BIBTEX_MISSING)
        		field_info[field_ptr] = field_info[field_parent_ptr];
        	field_ptr++;
        	field_parent_ptr++;
        }
    	}
    }
    cite_ptr++;
	}
	
	if ((num_cites - 1) * num_fields + crossref_num >= max_fields)
	{
		confusion(tr("field_info index is out of range"));
		return ;
	}
	
	cite_ptr = 0;
	while (cite_ptr < num_cites)
	{
		field_ptr = cite_ptr * num_fields + crossref_num;
    if (field_info[field_ptr] != XW_BIBTEX_MISSING)
    {
    	if (!findCiteLocsForThisCiteKey(field_info[field_ptr]))
    	{
    		if (cite_hash_found)
    			hashCiteConfusion();
    			
    		nonexistentCrossReferenceError();
        field_info[field_ptr] = XW_BIBTEX_MISSING;
    	}
    	else
    	{
    		if (cite_loc != ilk_info[lc_cite_loc])
    			hashCiteConfusion();
        cite_parent_ptr = ilk_info[cite_loc];
        if (type_list[cite_parent_ptr] == XW_BIBTEX_EMPTY)
        {
        	nonexistentCrossReferenceError();
        	field_info[field_ptr] = XW_BIBTEX_MISSING;
        }
        else
        {
        	field_parent_ptr = cite_parent_ptr * num_fields + crossref_num;
          if (field_info[field_parent_ptr] != XW_BIBTEX_MISSING)
          {
          	print(tr("Warning--you''ve nested cross references"));
						badCrossReferencePrint(cite_list[cite_parent_ptr]);
						println(tr("', which also refers to something"));
						markWarning();
          }
          
          if ((!all_entries) && 
          	  (cite_parent_ptr >= old_num_cites) &&
               (cite_info[cite_parent_ptr] < min_crossrefs))
					{
						field_info[field_ptr] = XW_BIBTEX_MISSING;
					}
        }
    	}
    }    
    cite_ptr++;
	}
	
	cite_ptr = 0;
	while (cite_ptr < num_cites)
	{
		if (type_list[cite_ptr] == XW_BIBTEX_EMPTY)
			printMissingEntry(cur_cite_str);
		else if ((all_entries) || 
			       (cite_ptr < old_num_cites) ||
             (cite_info[cite_ptr] >= min_crossrefs))
		{
			if (cite_ptr > cite_xptr)
			{
				if ((cite_xptr + 1) * num_fields > max_fields)
				{
					confusion(tr("field_info index is out of range"));
					return ;
				}
				
				cite_list[cite_xptr] = cite_list[cite_ptr];
				type_list[cite_xptr] = type_list[cite_ptr];
				if (!findCiteLocsForThisCiteKey(cite_list[cite_ptr]))
					citeKeyDisappearedConfusion();
					
				if ((!cite_hash_found) || (cite_loc != ilk_info[lc_cite_loc]))
					hashCiteConfusion();
					
				ilk_info[cite_loc] = cite_xptr;
				field_ptr = cite_xptr * num_fields;
				field_end_ptr = field_ptr + num_fields;
				tmp_ptr = cite_ptr * num_fields;
				while (field_ptr < field_end_ptr)
				{
					field_info[field_ptr] = field_info[tmp_ptr];
    			field_ptr++;
    			tmp_ptr++;
				}
			}
			cite_xptr++;
		}
		cite_ptr++;
	}
	
	num_cites = cite_xptr;
	if (all_entries)
	{
		cite_ptr = all_marker;
		while (cite_ptr < old_num_cites)
		{
			if (!entry_exists[cite_ptr])
				printMissingEntry(cite_info[cite_ptr]);
			cite_ptr++;
		}
	}
	
	if (num_ent_ints*num_cites > max_ent_ints)
	{
		int * tmp_num = (int*)malloc((num_ent_ints + 1) * (num_cites + 1) * sizeof(int));
		for (int i = 0; i < max_ent_ints; i++)
			tmp_num[i] = entry_ints[i];
			
		free(entry_ints);
		entry_ints = tmp_num;
		max_ent_ints = (num_ent_ints * num_cites);
	}
	
	int_ent_ptr = 0;
	while (int_ent_ptr < num_ent_ints*num_cites)
	{
		entry_ints[int_ent_ptr] = 0;
    int_ent_ptr++;
	}
	
	if (num_ent_strs * num_cites > max_ent_strs)
	{
		int * tmp_num = (int*)malloc((num_ent_strs + 1) * (num_cites + 1) * sizeof(int));
		for (int i = 0; i < max_ent_strs; i++)
			tmp_num[i] = entry_strs[i];
			
		free(entry_strs);
		entry_strs = tmp_num;
		max_ent_strs = (num_ent_strs * num_cites);
	}
	
	str_ent_ptr = 0;
	while (str_ent_ptr < num_ent_strs*num_cites)
	{
		entry_strs[( str_ent_ptr ) * ( XW_BIBTEX_ENT_STR_SIZE + 1 ) + (0)] = XW_BIBTEX_END_OF_STRING;
    str_ent_ptr++;
	}
	
	cite_ptr = 0;
	while (cite_ptr < num_cites)
	{
		sorted_cites[cite_ptr] = cite_ptr;
    cite_ptr++;
	}
	read_completed = true;
	buf_ptr2 = sv_ptr1;
	last = sv_ptr2;
	tmp_ptr = buf_ptr2;
	while (tmp_ptr < last)
	{
		buffer[tmp_ptr] = sv_buffer[tmp_ptr];
    tmp_ptr++;
	}
}

void XWBibTeX::bstReverseCommand()
{
	if (!read_seen)
	{
		print(tr("Illegal, reverse command before read command"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("reverse"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_LEFT_BRACE)
	{
		bstLeftBracePrint();
		print(tr("reverse"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("reverse"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	scanIdentifier(XW_BIBTEX_RIGHT_BRACE,XW_BIBTEX_COMMENT,XW_BIBTEX_COMMENT);
	if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) || 
			(scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
	{
		;
	}
	else
	{
		bstIdPrint();
		eatBstPrint();
		print(tr("reverse"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (badArgumentToken())
		return ;
		
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("reverse"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_RIGHT_BRACE)
	{
		bstRightBracePrint();
		print(tr("reverse"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	initCommandExecution();
	mess_with_entries = true;
	if (num_cites > 0)
	{
		sort_cite_ptr = num_cites;
		do
		{
			sort_cite_ptr--;
			cite_ptr = sorted_cites[sort_cite_ptr];
			executeFn(fn_loc);
      checkCommandExecution();
		} while (sort_cite_ptr != 0);
	}
}

void XWBibTeX::bstRightBracePrint()
{
	print(tr("'}' is missing in command: "));
}

void XWBibTeX::bstSortCommand()
{
	if (!read_seen)
	{
		print(tr("Illegal, sort command before read command"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (num_cites > 1)
		quickSort(0, num_cites-1);
}

void XWBibTeX::bstStringsCommand()
{
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("strings"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	if (scan_char != XW_BIBTEX_LEFT_BRACE)
	{
		bstLeftBracePrint();
		print(tr("strings"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}	
	buf_ptr2++;
	
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("strings"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	while (scan_char != XW_BIBTEX_RIGHT_BRACE)
	{
		scanIdentifier(XW_BIBTEX_RIGHT_BRACE,XW_BIBTEX_COMMENT,XW_BIBTEX_COMMENT);
		if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) || 
				(scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
		{
			;
		}
		else
		{
			bstIdPrint();
			eatBstPrint();
			print(tr("strings"));
			bstErrPrintAndLookForBlankLine();
			return ;
		}
		
		lowerCase(buffer, buf_ptr1, token_len);
		fn_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BST_FN_ILK,true);
		if (hash_found)
		{
			alreadySeenFunctionPrint(fn_loc);
			return ;
		}
		fn_type[fn_loc] = XW_BIBTEX_STR_GLOBAL_VAR;
		fn_info[fn_loc] = num_glb_strs;
		if (num_glb_strs == XW_BIBTEX_MAX_GLOB_STRS)
		{
			overflow(tr("number of string global-variables "),XW_BIBTEX_MAX_GLOB_STRS);
			return ;
		}
		num_glb_strs++;
		
		if (!eatBstWhiteSpace())
		{
			eatBstPrint();
			print(tr("strings"));
			bstErrPrintAndLookForBlankLine();
			return ;
		}
	}
	
	buf_ptr2++;
}

void XWBibTeX::bstWarnPrint()
{
	bstLnNumPrint();
	markWarning();
}

void XWBibTeX::bst1printStringSizeExceeded()
{
	print(tr("Warning--you've exceeded "));
}

void XWBibTeX::bst2printStringSizeExceeded()
{
	print(tr("-string-size,"));
	bstMildExWarnPrint();
	println(tr("*Please notify the bibstyle designer*"));
}

void XWBibTeX::bufferOverflow()
{
	overflow("buffer size", XW_BIBTEX_BUF_SIZE);
}

void XWBibTeX::buildIn(char * pds, int len, int fn_hash_loc, int blt_in_num)
{
	predefine(pds,len,XW_BIBTEX_BST_FN_ILK);
	fn_hash_loc = pre_def_loc;
	fn_type[fn_hash_loc] = XW_BIBTEX_BUILT_IN;
	fn_info[fn_hash_loc] = blt_in_num;
}

void XWBibTeX::caseConversionConfusion()
{
	confusion(tr("Unknown type of case conversion"));
}

void XWBibTeX::checkBraceLevel(int pop_lit_var)
{
	if (brace_level > 0)
		bracesUnbalancedComplaint(pop_lit_var);
}

void XWBibTeX::checkCiteOverflow(int last_cite)
{
	if (last_cite == XW_BIBTEX_MAX_CITES)
	{
		printPoolStr(hash_text[cite_loc]);
    println(tr(" is the key:"));
    overflow(tr("number of cite keys "),XW_BIBTEX_MAX_CITES);
	}
}

void XWBibTeX::checkCommandExecution()
{
	if (lit_stk_ptr != 0)
	{
		QString str = QString("ptr=%1, stack=").arg(lit_stk_ptr);
		print(str);
		popWholeStack();
		print(tr("---the literal stack isn't empty"));
		bstExWarnPrint();
	}
	
	if (cmd_str_ptr != str_ptr)
	{
		confusion(tr("Nonempty empty string stack"));
	}
}

void XWBibTeX::checkFieldOverflow(int total_fields)
{
	if (total_fields > max_fields)
	{
		int * tmp = (int*)malloc((max_fields + XW_BIBTEX_MAX_FIELDS) * sizeof(int));
		for (int i = 0; i < total_fields; i++)
			tmp[i] = field_info[i];
			
		for (int i = total_fields; i < (max_fields + XW_BIBTEX_MAX_FIELDS); i++)
			tmp[i] = 0;
			
		free(field_info);
		field_info = tmp;
		max_fields += XW_BIBTEX_MAX_FIELDS;
	}
}

void XWBibTeX::citeKeyDisappearedConfusion()
{
	confusion(tr("A cite key disappeared"));
}

bool XWBibTeX::compressBibWhite()
{
	if (field_end == XW_BIBTEX_BUF_SIZE)
	{
		bibFieldTooLongPrint();
		return false;
	}
	else
	{
		field_vl_str[field_end] = XW_BIBTEX_SPACE;
		field_end++;
	}
	
	while (!scanWhiteSpace())
	{
		if (!inputln(cur_bib_file))
		{
			eatBibPrint();
			return false;
		}
		bib_line_num++;
		buf_ptr2 = 0;
	}
	
	return true;
}

void XWBibTeX::confusion(const QString & str)
{
	QTextStream log(log_file);
	log << str;
	QTextStream term(term_out);
	term << str;
	printConfusion();
	stoped = true;
	cleanup();
	exit(1);
}

void XWBibTeX::decrBraceLevel(int pop_lit_var)
{
	if (brace_level == 0)
		bracesUnbalancedComplaint(pop_lit_var);
	else
		brace_level--;
}

void XWBibTeX::eatBibPrint()
{
	print(tr("Illegal end of database file"));
  bibErrPrint();
}

bool XWBibTeX::eatBibWhiteSpace()
{
	while (!scanWhiteSpace())
	{
		if (!inputln(cur_bib_file))
			return false;
		bib_line_num++;
		buf_ptr2 = 0;
	}
	
	return true;
}

void XWBibTeX::eatBstPrint()
{
	print(tr("Illegal end of style file in command: "));
}

bool XWBibTeX::eatBstWhiteSpace()
{
	while (true)
	{
		if (scanWhiteSpace())
		{
			if (scan_char != XW_BIBTEX_COMMENT)
				return true;
		}
		
		if (!inputln(bst_file))
			return false;
			
		bst_line_num++;
		buf_ptr2 = 0;
	}
	
	return false;
}

bool XWBibTeX::enoughTextChars(int enough_chars)
{
	num_text_chars = 0;
	ex_buf_yptr = ex_buf_xptr;
	while ((ex_buf_yptr < ex_buf_ptr) && (num_text_chars < enough_chars))
	{
		ex_buf_yptr++;
		if (ex_buf[ex_buf_yptr-1] == XW_BIBTEX_LEFT_BRACE)
		{
			brace_level++;
			if ((brace_level = 1) && (ex_buf_yptr < ex_buf_ptr))
			{
				if (ex_buf[ex_buf_yptr] == XW_BIBTEX_BACKSLASH)
				{
					ex_buf_yptr++;
					while ((ex_buf_yptr < ex_buf_ptr) && (brace_level > 0))
					{
						if (ex_buf[ex_buf_yptr] == XW_BIBTEX_RIGHT_BRACE)
							brace_level--;
						else if (ex_buf[ex_buf_yptr] == XW_BIBTEX_LEFT_BRACE)
							brace_level++;
						ex_buf_yptr++;
					}
				}
			}
		}
		else if (ex_buf[ex_buf_yptr-1] == XW_BIBTEX_RIGHT_BRACE)
			brace_level--;
		num_text_chars++;
	}
	
	if (num_text_chars < enough_chars)
		return false;
		
	return true;
}

void XWBibTeX::executeFn(int ex_fn_loc)
{
	int r_pop_lt1, r_pop_lt2, r_pop_tp1, r_pop_tp2;
	int wiz_ptr;
	
	switch (fn_type[ex_fn_loc])
	{
		case XW_BIBTEX_BUILT_IN:
			{
				switch (fn_info[ex_fn_loc])
				{
					case XW_BIBTEX_N_EQUALS :          
						xEquals();
						break;
						
    			case XW_BIBTEX_N_GREATER_THAN :    
    				xGreaterThan();
    				break;
    				
    			case XW_BIBTEX_N_LESS_THAN :       
    				xLessThan();
    				break;
    				
    			case XW_BIBTEX_N_PLUS :            
    				xPlus();
    				break;
    				
    			case XW_BIBTEX_N_MINUS :           
    				xMinus();
    				break;
    				
    			case XW_BIBTEX_N_CONCATENATE :     
    				xConcatenate();
    				break;
    				
    			case XW_BIBTEX_N_GETS :            
    				xGets();
    				break;
    				
    			case XW_BIBTEX_N_ADD_PERIOD :      
    				xAddPeriod();
    				break;
    				
    			case XW_BIBTEX_N_CALL_TYPE:
    				if (!mess_with_entries)
    					bstCantMessWithEntriesPrint();
    				else if (type_list[cite_ptr] == XW_BIBTEX_UNDEFINED)
    					executeFn(b_default);
    				else if (type_list[cite_ptr] == XW_BIBTEX_EMPTY)
    					;
    				else
    					executeFn(type_list[cite_ptr]);
    				break;
    				
    			case XW_BIBTEX_N_CHANGE_CASE :     
    				xChangeCase();
    				break;
    				
    			case XW_BIBTEX_N_CHR_TO_INT :      
    				xChrToInt();
    				break;
    				
    			case XW_BIBTEX_N_CITE :            
    				xCite();
    				break;
    				
    			case XW_BIBTEX_N_DUPLICATE :       
    				xDuplicate();
    				break;
    				
    			case XW_BIBTEX_N_EMPTY :           
    				xEmpty();
    				break;
    				
    			case XW_BIBTEX_N_FORMAT_NAME :     
    				xFormatName();
    				break;
    				
    			case XW_BIBTEX_N_IF:
    				popLitStk(&pop_lit1,&pop_typ1);
						popLitStk(&pop_lit2,&pop_typ2);
						popLitStk(&pop_lit3,&pop_typ3);
						if (pop_typ1 != XW_BIBTEX_STK_FN)
							printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_FN);
						else if (pop_typ2 != XW_BIBTEX_STK_FN)
							printWrongStkLit(pop_lit2,pop_typ2,XW_BIBTEX_STK_FN);
						else if (pop_typ3 != XW_BIBTEX_STK_INT)
							printWrongStkLit(pop_lit3,pop_typ3,XW_BIBTEX_STK_INT);
						else if (pop_lit3 > 0)
							executeFn(pop_lit2);
						else
							executeFn(pop_lit1);
    				break;
    				
    			case XW_BIBTEX_N_INT_TO_CHR :      
    				xIntToChr();
    				break;
    				
    			case XW_BIBTEX_N_INT_TO_STR :      
    				xIntToStr();
    				break;
    				
    			case XW_BIBTEX_N_MISSING :         
    				xMissing();	
    				break;
    				
    			case XW_BIBTEX_N_NEWLINE:
    				outputBblLine();
    				break;
    				
    			case XW_BIBTEX_N_NUM_NAMES :       
    				xNumNames();
    				break;
    				
    			case XW_BIBTEX_N_POP:
    				popLitStk(&pop_lit1,&pop_typ1);
    				break;
    				
    			case XW_BIBTEX_N_PREAMBLE :        
    				xPreamble();
    				break;
    				
    			case XW_BIBTEX_N_PURIFY :          
    				xPurify();
    				break;
    				
    			case XW_BIBTEX_N_QUOTE :           
    				xQuote();
    				break;
    				
    			case XW_BIBTEX_N_SKIP:
    				break;
    				
    			case XW_BIBTEX_N_STACK:
    				popWholeStack();
    				break;
    				
    			case XW_BIBTEX_N_SUBSTRING :       
    				xSubstring();
    				break;
    				
    			case XW_BIBTEX_N_SWAP :            
    				xSwap();
    				break;
    				
    			case XW_BIBTEX_N_TEXT_LENGTH :     
    				xTextLength();
    				break;
    				
    			case XW_BIBTEX_N_TEXT_PREFIX :     
    				xTextPrefix();
    				break;
    				
    			case XW_BIBTEX_N_TOP_STACK:
    				popTopAndPrint();
    				break;
    				
    			case XW_BIBTEX_N_TYPE :            
    				xType();
    				break;
    				
    			case XW_BIBTEX_N_WARNING :         
    				xWarning();
    				break;
    				
    			case XW_BIBTEX_N_WHILE:
    				popLitStk(&r_pop_lt1,&r_pop_tp1);
						popLitStk(&r_pop_lt2,&r_pop_tp2);
						if (r_pop_tp1 != XW_BIBTEX_STK_FN)
							printWrongStkLit(r_pop_lt1,r_pop_tp1,XW_BIBTEX_STK_FN);
						else if (r_pop_tp2 != XW_BIBTEX_STK_FN)
							printWrongStkLit(r_pop_lt2,r_pop_tp2,XW_BIBTEX_STK_FN);
						else
						{
							while (true)
							{
								executeFn(r_pop_lt2);
								popLitStk(&pop_lit1,&pop_typ1);
								if (pop_typ1 != XW_BIBTEX_STK_INT)
								{
									printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_INT);
									goto end_while;
								}
								else if (pop_lit1 > 0)
									executeFn(r_pop_lt1);
								else
									goto end_while;
							}
							
end_while:
							;
						}
    				break;
    				
    			case XW_BIBTEX_N_WIDTH :           
    				xWidth();
    				break;
    				
    			case XW_BIBTEX_N_WRITE :           
    				xWrite();
    				break;
    				
    			default:
    				confusion(tr("Unknown built-in function"));
    				return ;
    				break;
				}
			}
			break;
			
		case XW_BIBTEX_WIZ_DEFINED:
			wiz_ptr = fn_info[ex_fn_loc];
			while (wiz_functions[wiz_ptr] != end_of_def)
			{
				if (wiz_functions[wiz_ptr] != quote_next_fn)
					executeFn(wiz_functions[wiz_ptr]);
				else
				{
					wiz_ptr++;
					pushLitStk(wiz_functions[wiz_ptr], XW_BIBTEX_STK_FN);
				}
				wiz_ptr++;
			}
			break;
			
		case XW_BIBTEX_INT_LITERAL:
			pushLitStk(fn_info[ex_fn_loc], XW_BIBTEX_STK_INT);
			break;
			
		case XW_BIBTEX_STR_LITERAL:
			pushLitStk(hash_text[ex_fn_loc], XW_BIBTEX_STK_STR);
			break;
			
		case XW_BIBTEX_FIELD:
			if (!mess_with_entries)
				bstCantMessWithEntriesPrint();
			else
			{
				field_ptr = cite_ptr*num_fields + fn_info[ex_fn_loc];
				if (field_info[field_ptr] == XW_BIBTEX_MISSING)
					pushLitStk(hash_text[ex_fn_loc], XW_BIBTEX_STK_FIELD_MISSING);
				else
					pushLitStk(field_info[field_ptr], XW_BIBTEX_STK_STR);
			}
			break;
			
		case XW_BIBTEX_INT_ENTRY_VAR:
			if (!mess_with_entries)
				bstCantMessWithEntriesPrint();
			else
				pushLitStk(entry_ints[cite_ptr*num_ent_ints+fn_info[ex_fn_loc]],XW_BIBTEX_STK_INT);
			break;
			
		case XW_BIBTEX_STR_ENTRY_VAR:
			if (!mess_with_entries)
				bstCantMessWithEntriesPrint();
			else
			{
				str_ent_ptr = cite_ptr*num_ent_strs + fn_info[ex_fn_loc];
				ex_buf_ptr = 0;
				while (entry_strs[( str_ent_ptr ) * ( XW_BIBTEX_ENT_STR_SIZE + 1 ) + ( ex_buf_ptr)] != XW_BIBTEX_END_OF_STRING)
				{
					ex_buf[ex_buf_ptr] = entry_strs[( str_ent_ptr ) * ( XW_BIBTEX_ENT_STR_SIZE + 1 ) + (ex_buf_ptr)];
          ex_buf_ptr++;
				}
				ex_buf_length = ex_buf_ptr;
    		addPoolBufAndPush();
			}
			break;
			
		case XW_BIBTEX_INT_GLOBAL_VAR:
			pushLitStk(fn_info[ex_fn_loc], XW_BIBTEX_STK_INT);
			break;
			
		case XW_BIBTEX_STR_GLOBAL_VAR:
			str_glb_ptr = fn_info[ex_fn_loc];
			if (glb_str_ptr[str_glb_ptr] > 0)
				pushLitStk(glb_str_ptr[str_glb_ptr],XW_BIBTEX_STK_STR);
			else
			{
				strRoom(glb_str_end[str_glb_ptr]);
    		glob_chr_ptr = 0;
    		while (glob_chr_ptr < glb_str_end[str_glb_ptr])
    		{
    			appendChar(global_strs[str_glb_ptr][glob_chr_ptr]);
        	glob_chr_ptr++;
    		}
    		pushLitStk(makeString(), XW_BIBTEX_STK_STR);
			}
			break;
			
		default:
			unknwnFunctionClassConfusion();
			break;
	}
}

void XWBibTeX::figureOutTheFormattedName()
{
	ex_buf_ptr = 0;
	sp_brace_level = 0;
	sp_ptr = str_start[pop_lit1];
	sp_end = str_start[pop_lit1+1];
	while (sp_ptr < sp_end)
	{
		if (str_pool[sp_ptr] == XW_BIBTEX_LEFT_BRACE)
		{
			sp_brace_level++;
			sp_ptr++;
			sp_xptr1 = sp_ptr;
			alpha_found = false;
			double_letter = false;
			end_of_group = false;
			to_be_written = true;
			while ((!end_of_group) && (sp_ptr < sp_end))
			{
				if (lex_class[str_pool[sp_ptr]] == XW_BIBTEX_ALPHA)
				{
					sp_ptr++;
					if (alpha_found)
					{
						braceLvlOneLettersComplaint();
    				to_be_written = false;
					}
					else
					{
						switch (str_pool[sp_ptr-1])
						{
							case 'f':
							case 'F':
								cur_token = first_start;
								last_token = first_end;
								if (cur_token == last_token)
									to_be_written = false;
								if ((str_pool[sp_ptr] == 'f') || (str_pool[sp_ptr] == 'F'))
									double_letter = true;
								break;
								
							case 'v':
							case 'V':
								cur_token = von_start;
								last_token = von_end;
								if (cur_token == last_token)
									to_be_written = false;
								if ((str_pool[sp_ptr] == 'v') || (str_pool[sp_ptr] == 'V'))
									double_letter = true;
								break;
								
							case 'l':
							case 'L':
								cur_token = von_end;
								last_token = last_end;
								if (cur_token == last_token)
									to_be_written = false;
								if ((str_pool[sp_ptr] == 'l') || (str_pool[sp_ptr] == 'L'))
									double_letter = true;
								break;
								
							case 'j':
							case 'J':
								cur_token = last_end;
								last_token = jr_end;
								if (cur_token == last_token)
									to_be_written = false;
								if ((str_pool[sp_ptr] == 'j') || (str_pool[sp_ptr] == 'J'))
									double_letter = true;
								break;
								
							default:
								braceLvlOneLettersComplaint();
                to_be_written = false;
								break;
						}
						if (double_letter)
						sp_ptr++;
					}
					alpha_found = true;					
				}
				else if (str_pool[sp_ptr] == XW_BIBTEX_RIGHT_BRACE)
				{
					sp_brace_level--;
					sp_ptr++;
					end_of_group = true;
				}
				else if (str_pool[sp_ptr] == XW_BIBTEX_LEFT_BRACE)
				{
					sp_brace_level++;
					sp_ptr++;
					skipStuffAtSpBraceLevelGreaterThanOne();
				}
				else
					sp_ptr++;
			}
			
			if ((end_of_group) && (to_be_written))
			{
				ex_buf_xptr = ex_buf_ptr;
				sp_ptr = sp_xptr1;
				sp_brace_level = 1;
				while (sp_brace_level > 0)
				{
					if ((lex_class[str_pool[sp_ptr]] == XW_BIBTEX_ALPHA) && 
						  (sp_brace_level == 1))
					{
						sp_ptr++;
						if (double_letter)
							sp_ptr++;
						use_default = true;
						sp_xptr2 = sp_ptr;
						if (str_pool[sp_ptr] == XW_BIBTEX_LEFT_BRACE)
						{
							use_default = false;
							sp_brace_level++;
							sp_ptr++;
							sp_xptr1 = sp_ptr;
    					skipStuffAtSpBraceLevelGreaterThanOne();
    					sp_xptr2 = sp_ptr - 1;
						}
						
						while (cur_token < last_token)
						{
							if (double_letter)
							{
								name_bf_ptr = name_tok[cur_token];
								name_bf_xptr = name_tok[cur_token+1];
								if (ex_buf_length+(name_bf_xptr-name_bf_ptr) > XW_BIBTEX_BUF_SIZE)
								{
									bufferOverflow();
									return ;
								}
								
								while (name_bf_ptr < name_bf_xptr)
								{
									ex_buf[ex_buf_ptr] = name_buf[name_bf_ptr];
									ex_buf_ptr++;
									name_bf_ptr++;
								}
							}
							else
							{
								name_bf_ptr = name_tok[cur_token];
								name_bf_xptr = name_tok[cur_token+1];
								while (name_bf_ptr < name_bf_xptr)
								{
									if (lex_class[name_buf[name_bf_ptr]] == XW_BIBTEX_ALPHA)
									{
										if (ex_buf_ptr == XW_BIBTEX_BUF_SIZE)
										{
											bufferOverflow();
											return ;
										}
										
										ex_buf[ex_buf_ptr] = name_buf[name_bf_ptr];
										ex_buf_ptr++;
										goto loop_exit;
									}
									else if ((name_buf[name_bf_ptr] == XW_BIBTEX_LEFT_BRACE) && 
										       (name_bf_ptr + 1 < name_bf_xptr))
									{
										if (name_buf[name_bf_ptr+1] == XW_BIBTEX_BACKSLASH)
										{
											if (ex_buf_ptr + 2 > XW_BIBTEX_BUF_SIZE)
											{
												bufferOverflow();
												return ;
											}
											ex_buf[ex_buf_ptr] = XW_BIBTEX_LEFT_BRACE;
											ex_buf_ptr++;
											ex_buf[ex_buf_ptr] = XW_BIBTEX_BACKSLASH;
											ex_buf_ptr++;
											name_bf_ptr = name_bf_ptr + 2;
											nm_brace_level = 1;
											while ((name_bf_ptr < name_bf_xptr) && (nm_brace_level > 0))
											{
												if (name_buf[name_bf_ptr] == XW_BIBTEX_RIGHT_BRACE)
													nm_brace_level--;
												else if (name_buf[name_bf_ptr] == XW_BIBTEX_LEFT_BRACE)
													nm_brace_level++;
													
												if (ex_buf_ptr == XW_BIBTEX_BUF_SIZE)
												{
													bufferOverflow();
													return ;
												}
										
												ex_buf[ex_buf_ptr] = name_buf[name_bf_ptr];
												ex_buf_ptr++;
												name_bf_ptr++;
											}
											goto loop_exit;
										}
									}
									name_bf_ptr++;
								}
loop_exit:
								;
							}
							cur_token++;
							if (cur_token < last_token)
							{
								if (use_default)
								{
									if (!double_letter)
									{
										if (ex_buf_ptr == XW_BIBTEX_BUF_SIZE)
										{
											bufferOverflow();
											return ;
										}
										
										ex_buf[ex_buf_ptr] = XW_BIBTEX_PERIOD;
										ex_buf_ptr++;
									}
									
									if (lex_class[name_sep_char[cur_token]] == XW_BIBTEX_SEP_CHAR)
									{
										if (ex_buf_ptr == XW_BIBTEX_BUF_SIZE)
										{
											bufferOverflow();
											return ;
										}
										
										ex_buf[ex_buf_ptr] = name_sep_char[cur_token];
										ex_buf_ptr++;
									}									
									else if ((cur_token = last_token-1) || (!enoughTextChars(XW_BIBTEX_LONG_TOKEN)))
                  {
                  	if (ex_buf_ptr == XW_BIBTEX_BUF_SIZE)
										{
											bufferOverflow();
											return ;
										}
										
										ex_buf[ex_buf_ptr] = XW_BIBTEX_TIE;
										ex_buf_ptr++;
                  }
                  else
                  {
                  	if (ex_buf_ptr == XW_BIBTEX_BUF_SIZE)
										{
											bufferOverflow();
											return ;
										}
										
										ex_buf[ex_buf_ptr] = XW_BIBTEX_SPACE;
										ex_buf_ptr++;
                  }
								}
								else
								{
									if (ex_buf_length+(sp_xptr2-sp_xptr1) > XW_BIBTEX_BUF_SIZE)
									{
										bufferOverflow();
										return ;
									}
									sp_ptr = sp_xptr1;
									while (sp_ptr < sp_xptr2)
									{
										ex_buf[ex_buf_ptr] = str_pool[sp_ptr];
										ex_buf_ptr++;
										sp_ptr++;
									}
								}
							}
						}
						
						if (!use_default)
							sp_ptr = sp_xptr2 + 1;
					}
					else if (str_pool[sp_ptr] == XW_BIBTEX_RIGHT_BRACE)
					{
						sp_brace_level--;
						sp_ptr++;
						if (sp_brace_level > 0)
						{
							if (ex_buf_ptr == XW_BIBTEX_BUF_SIZE)
							{
								bufferOverflow();
								return ;
							}
										
							ex_buf[ex_buf_ptr] = XW_BIBTEX_RIGHT_BRACE;
							ex_buf_ptr++;
						}
					}
					else if (str_pool[sp_ptr] == XW_BIBTEX_LEFT_BRACE)
					{
						sp_brace_level++;
						sp_ptr++;
						if (ex_buf_ptr == XW_BIBTEX_BUF_SIZE)
						{
							bufferOverflow();
							return ;
						}
										
						ex_buf[ex_buf_ptr] = XW_BIBTEX_LEFT_BRACE;
						ex_buf_ptr++;
					}
					else
					{
						if (ex_buf_ptr == XW_BIBTEX_BUF_SIZE)
						{
							bufferOverflow();
							return ;
						}
										
						ex_buf[ex_buf_ptr] = str_pool[sp_ptr];
						ex_buf_ptr++;
						sp_ptr++;
					}
				}
				
				if (ex_buf_ptr > 0)
				{
					if (ex_buf[ex_buf_ptr-1] == XW_BIBTEX_TIE)
					{
						ex_buf_ptr--;
						if (ex_buf[ex_buf_ptr-1] == XW_BIBTEX_TIE)
							;
						else if (!enoughTextChars(XW_BIBTEX_LONG_NAME))
							ex_buf_ptr++;
						else
						{
							ex_buf[ex_buf_ptr] = XW_BIBTEX_SPACE;
							ex_buf_ptr++;
						}
					}
				}
			}
		}
		else if (str_pool[sp_ptr] == XW_BIBTEX_RIGHT_BRACE)
		{
			bracesUnbalancedComplaint(pop_lit1);
			sp_ptr++;
		}
		else
		{
			if (ex_buf_ptr == XW_BIBTEX_BUF_SIZE)
			{
				bufferOverflow();
				return ;
			}
										
			ex_buf[ex_buf_ptr] = str_pool[sp_ptr];
			ex_buf_ptr++;
		}
	}
	
	if (sp_brace_level > 0)
		bracesUnbalancedComplaint(pop_lit1);
	ex_buf_length = ex_buf_ptr;
}

void XWBibTeX::fileNmSizeOverflow()
{
	overflow("file name size ",INT_MAX);
}

bool XWBibTeX::findCiteLocsForThisCiteKey(int cite_str)
{
	ex_buf5_ptr = 0;
	tmp_ptr = str_start[cite_str];
	tmp_end_ptr = str_start[cite_str+1];
	while (tmp_ptr < tmp_end_ptr)
	{
		ex_buf5[ex_buf5_ptr] = str_pool[tmp_ptr];
		ex_buf5_ptr++;
		tmp_ptr++;
	}
	cite_loc = strLookup(ex_buf5,0,length(cite_str),XW_BIBTEX_CITE_ILK,false);
	cite_hash_found = hash_found;
	lowerCase(ex_buf5, 0, length(cite_str));
	lc_cite_loc = strLookup(ex_buf5,0,length(cite_str),XW_BIBTEX_LC_CITE_ILK,false);
	if (hash_found)
		return true;
		
	return false;
}

void XWBibTeX::getAuxCommandAndProcess()
{
	buf_ptr2 = 0;
	if (!scan1(XW_BIBTEX_LEFT_BRACE))
	{
		return ;
	}
	
	command_num = ilk_info[strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_AUX_COMMAND_ILK,false)];
	if (hash_found)
	{
		switch (command_num)
		{
			case XW_BIBTEX_N_AUX_BIBDATA:
				auxBibDataCommand();
				break;
				
			case XW_BIBTEX_N_AUX_BIBSTYLE: 
				auxBibStyleCommand();
				break;
				
			case XW_BIBTEX_N_AUX_CITATION: 
				auxCitationCommand();
				break;
				
			case XW_BIBTEX_N_AUX_INPUT: 
				auxInputCommand();
				break;
				
			default:
				confusion(tr("Unknown auxiliary-file command"));
				break;
		}
	}
}

void XWBibTeX::getBibCommandOrEntryAndProcess()
{
	at_bib_command = false;
	while (!scan1(XW_BIBTEX_AT_SIGN))
	{
		if (!inputln(cur_bib_file))
			return ;
		bib_line_num++;
		buf_ptr2 = 0;
	}
	
	if (scan_char != XW_BIBTEX_AT_SIGN)
	{
		confusion(tr("An '@' disappeared"));
		return ;
	}
	buf_ptr2++;
	if (!eatBibWhiteSpace())
	{
		eatBibPrint();
		return ;
	}
	scanIdentifier(XW_BIBTEX_LEFT_BRACE,XW_BIBTEX_LEFT_PAREN,XW_BIBTEX_LEFT_PAREN);
	if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) || 
			(scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
	{
		;
	}
	else
	{
		bibIdPrint();
		print(" an entry type");
		bibErrPrint();
		return ;
	}
	
	lowerCase(buffer, buf_ptr1, token_len);
	command_num = ilk_info[strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BIB_COMMAND_ILK,false)];
	if (hash_found)
	{
		at_bib_command = true;
		switch (command_num)
		{
			case XW_BIBTEX_N_BIB_COMMENT:
				break;
				
			case XW_BIBTEX_N_BIB_PREAMBLE:
				if (preamble_ptr == max_bib_files)
				{
					int * tmp_num = (int*)malloc((max_bib_files + XW_BIBTEX_MAX_BIB_FILES) * sizeof(int));
					for (int i = 0; i < max_bib_files; i++)
						tmp_num[i] = bib_list[i];
					free(bib_list);
					bib_list = tmp_num;
					
					QFile ** tmp_files = (QFile**)malloc((max_bib_files + XW_BIBTEX_MAX_BIB_FILES) * sizeof(QFile*));
					for (int i = 0; i < (max_bib_files + XW_BIBTEX_MAX_BIB_FILES); i++)
						tmp_files[i] = 0;
					for (int i = 0; i < max_bib_files; i++)
						tmp_files[i] = bib_file[i];
					free(bib_file);
					bib_file = tmp_files;
					
					tmp_num = (int*)malloc((max_bib_files + XW_BIBTEX_MAX_BIB_FILES) * sizeof(int));
					for (int i = 0; i < max_bib_files; i++)
						tmp_num[i] = s_preamble[i];
					free(s_preamble);
					s_preamble = tmp_num;
					max_bib_files += XW_BIBTEX_MAX_BIB_FILES;
				}
				if (!eatBibWhiteSpace())
				{
					eatBibPrint();
					return ;
				}
				if (scan_char == XW_BIBTEX_LEFT_BRACE)
					right_outer_delim = XW_BIBTEX_RIGHT_BRACE;
				else if (scan_char == XW_BIBTEX_LEFT_PAREN)
					right_outer_delim = XW_BIBTEX_RIGHT_PAREN;
				else
				{
					bibOneOfTwoPrint(XW_BIBTEX_LEFT_BRACE,XW_BIBTEX_LEFT_PAREN);
					return ;
				}
				buf_ptr2++;
				if (!eatBibWhiteSpace())
				{
					eatBibPrint();
					return ;
				}
				store_field = true;
				if (!scanAndStoreTheFieldValueAndEatWhite())
					return ;
				if (scan_char != right_outer_delim)
				{
					QString str = QString(tr("Missing '%1' in preamble command")).arg((char)(xchr[right_outer_delim]));
					print(str);
					bibErrPrint();
					return ;
				}
				buf_ptr2++;
				return ;
				break;
				
			case XW_BIBTEX_N_BIB_STRING:
				if (!eatBibWhiteSpace())
				{
					eatBibPrint();
					return ;
				}
				if (scan_char == XW_BIBTEX_LEFT_BRACE)
					right_outer_delim = XW_BIBTEX_RIGHT_BRACE;
				else if (scan_char == XW_BIBTEX_LEFT_PAREN)
					right_outer_delim = XW_BIBTEX_RIGHT_PAREN;
				else
				{
					bibOneOfTwoPrint(XW_BIBTEX_LEFT_BRACE,XW_BIBTEX_LEFT_PAREN);
					return ;
				}
				buf_ptr2++;
				if (!eatBibWhiteSpace())
				{
					eatBibPrint();
					return ;
				}
				scanIdentifier(XW_BIBTEX_EQUALS_SIGN,XW_BIBTEX_EQUALS_SIGN,XW_BIBTEX_EQUALS_SIGN);
				if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) || 
						(scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
				{
						;
				}
				else
				{
					bibIdPrint();
					print("a string name");
					bibErrPrint();
					return ;
				}
				lowerCase(buffer, buf_ptr1, token_len);
				cur_macro_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_MACRO_ILK,true);
				ilk_info[cur_macro_loc] = hash_text[cur_macro_loc];
				if (!eatBibWhiteSpace())
				{
					eatBibPrint();
					return ;
				}
				if (scan_char != XW_BIBTEX_EQUALS_SIGN)
				{
					bibEqualsSignPrint();
					return ;
				}
				buf_ptr2++;
				if (!eatBibWhiteSpace())
				{
					eatBibPrint();
					return ;
				}
				store_field = true;
				if (!scanAndStoreTheFieldValueAndEatWhite())
					return ;
				if (scan_char != right_outer_delim)
				{
					QString str = QString(tr("Missing '%1' in string command")).arg((char)(xchr[right_outer_delim]));
					print(str);
					bibErrPrint();
					return ;
				}
				buf_ptr2++;
				return ;
				break;
				
			default:
				bibCmdConfusion();
				break;
		}
	}
	else
	{
		entry_type_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BST_FN_ILK,false);
		if ((!hash_found) || (fn_type[entry_type_loc] != XW_BIBTEX_WIZ_DEFINED))
			type_exists = false;
		else
			type_exists = true;
	}
	if (!eatBibWhiteSpace())
	{
		eatBibPrint();
		return ;
	}
	if (scan_char == XW_BIBTEX_LEFT_BRACE)
		right_outer_delim = XW_BIBTEX_RIGHT_BRACE;
	else if (scan_char == XW_BIBTEX_LEFT_PAREN)
		right_outer_delim = XW_BIBTEX_RIGHT_PAREN;
	else
	{
		bibOneOfTwoPrint(XW_BIBTEX_LEFT_BRACE,XW_BIBTEX_LEFT_PAREN);
		return ;
	}
	buf_ptr2++;
	if (!eatBibWhiteSpace())
	{
		eatBibPrint();
		return ;
	}
	if (right_outer_delim == XW_BIBTEX_RIGHT_PAREN)
	{
		if (scan1White(XW_BIBTEX_COMMA))
				;
	}
	else if (scan2White(XW_BIBTEX_COMMA,XW_BIBTEX_RIGHT_BRACE))
	{
		tmp_ptr = buf_ptr1;
		while (tmp_ptr < buf_ptr2)
		{
			ex_buf[tmp_ptr] = buffer[tmp_ptr];
			tmp_ptr++;
		}
		lowerCase(ex_buf3, buf_ptr1, token_len);
		if (all_entries)
			lc_cite_loc = strLookup(ex_buf,buf_ptr1,token_len,XW_BIBTEX_LC_CITE_ILK,true);
		else
			lc_cite_loc = strLookup(ex_buf,buf_ptr1,token_len,XW_BIBTEX_LC_CITE_ILK, false);
		if (hash_found)
		{
			entry_cite_ptr = ilk_info[ilk_info[lc_cite_loc]];
			if ((!all_entries) || 
				  (entry_cite_ptr < all_marker) || 
				  (entry_cite_ptr >= old_num_cites))
			{
				if (type_list[entry_cite_ptr] == XW_BIBTEX_EMPTY)
				{
					if ((!all_entries) && (entry_cite_ptr >= old_num_cites))
					{
						cite_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_CITE_ILK,true);
						if (!hash_found)
						{
							ilk_info[lc_cite_loc] = cite_loc;
        			ilk_info[cite_loc] = entry_cite_ptr;
       				cite_list[entry_cite_ptr] = hash_text[cite_loc];
       				hash_found = true;
						}
					}
					goto first_time_entry;
				}
			}
			else if (!entry_exists[entry_cite_ptr])
			{
				ex_buf_ptr = 0;
				tmp_ptr = str_start[cite_info[entry_cite_ptr]];
				tmp_end_ptr = str_start[cite_info[entry_cite_ptr]+1];
				while (tmp_ptr < tmp_end_ptr)
				{
					ex_buf[ex_buf_ptr] = str_pool[tmp_ptr];
    			ex_buf_ptr++;
    			tmp_ptr++;
				}
				lowerCase(ex_buf, 0, length(cite_info[entry_cite_ptr]));
				lc_xcite_loc = strLookup(ex_buf,0,length(cite_info[entry_cite_ptr]), XW_BIBTEX_LC_CITE_ILK,false);
				if (!hash_found)
					citeKeyDisappearedConfusion();
				if (lc_xcite_loc == lc_cite_loc)
					goto first_time_entry;
			}
			if (type_list[entry_cite_ptr] == XW_BIBTEX_EMPTY)
			{
				confusion(tr("The cite list is messed up"));
				return ;
			}
			
			print(tr("Repeated entry"));
			bibErrPrint();
			return ;
			
first_time_entry:
			;
		}
		
		store_entry = true;
		if (all_entries)
		{
			if (hash_found)
			{
				if (entry_cite_ptr < all_marker)
				{
					goto cite_already_set;
				}
				else
				{
					entry_exists[entry_cite_ptr] = true;
        	cite_loc = ilk_info[lc_cite_loc];
				}
			}
			else
			{
				cite_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_CITE_ILK,true);
				if (hash_found)
				{
					hashCiteConfusion();
				}
			}
			
			entry_cite_ptr = cite_ptr;
			addDatabaseCite(cite_ptr);
			
cite_already_set:
			;
		}
		else if (!hash_found)
		{
			store_entry = false;
		}
		
		if (store_entry)
		{
			if (type_exists)
				type_list[entry_cite_ptr] = entry_type_loc;
			else
			{
				type_list[entry_cite_ptr] = XW_BIBTEX_UNDEFINED;
    		print(tr("Warning--entry type for '"));
    		printToken();
    		println(tr("' isn't style-file defined"));
    		bibWarnPrint();
			}
		}
	}
	if (!eatBibWhiteSpace())
	{
		eatBibPrint();
		return ;
	}
	
	while (scan_char != right_outer_delim)
	{
		if (scan_char != XW_BIBTEX_COMMA)
		{
			bibOneOfTwoPrint(XW_BIBTEX_COMMA,right_outer_delim);
			return ;
		}
		buf_ptr2++;
		if (!eatBibWhiteSpace())
		{
			eatBibPrint();
			return ;
		}
		
		if (scan_char == right_outer_delim)
			goto loop_exit;
			
		scanIdentifier(XW_BIBTEX_EQUALS_SIGN,XW_BIBTEX_EQUALS_SIGN,XW_BIBTEX_EQUALS_SIGN);
		if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) ||
        (scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
		{
			;
		}
		else
		{
			bibIdPrint();
      print(tr("a field name"));
      bibErrPrint();
      return ;
		}
		
		store_field = false;
		if (store_entry)
		{
			lowerCase(buffer, buf_ptr1, token_len);
			field_name_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BST_FN_ILK,false);
			if (hash_found)
			{
				if (fn_type[field_name_loc]==XW_BIBTEX_FIELD)
					store_field = true;
			}
		}
		if (!eatBibWhiteSpace())
		{
			eatBibPrint();
			return ;
		}
		if (scan_char != XW_BIBTEX_EQUALS_SIGN)
		{
			bibEqualsSignPrint();
			return ;
		}
		buf_ptr2++;
		if (!eatBibWhiteSpace())
		{
			eatBibPrint();
			return ;
		}
		
		if (!scanAndStoreTheFieldValueAndEatWhite())
			return ;
	}
	
loop_exit:
	buf_ptr2++;
}

void XWBibTeX::getBstCommandAndProcess()
{
	if (!scanAlpha())
	{
		QString str = QString(tr("'%1'  can't start a style-file command")).arg((char)xchr[scan_char]);
		print(str);
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	lowerCase(buffer, buf_ptr1, token_len);
	command_num = ilk_info[strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BST_COMMAND_ILK,false)];
	if (hash_found)
	{
		printToken();
		print(tr(" is an illegal style-file command"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	switch (command_num)
	{
		case XW_BIBTEX_N_BST_ENTRY: 
			bstEntryCommand();
			break;
			
    case XW_BIBTEX_N_BST_EXECUTE: 
    	bstExecuteCommand();
    	break;
    	
    case XW_BIBTEX_N_BST_FUNCTION: 
    	bstFunctionCommand();
    	break;
    	
    case XW_BIBTEX_N_BST_INTEGERS: 
    	bstIntegersCommand();
    	break;
    	
    case XW_BIBTEX_N_BST_ITERATE: 
    	bstIterateCommand();
    	break;
    	
    case XW_BIBTEX_N_BST_MACRO : 
    	bstMacroCommand();
    	break;
    	
    case XW_BIBTEX_N_BST_READ : 
    	bstReadCommand();
    	break;
    	
    case XW_BIBTEX_N_BST_REVERSE : 
    	bstReverseCommand();
    	break;
    	
    case XW_BIBTEX_N_BST_SORT : 
    	bstSortCommand();
    	break;
    	
    case XW_BIBTEX_N_BST_STRINGS : 
    	bstStringsCommand();
    	break;
    	
    default:
    	confusion(tr("Unknown style-file command"));
    	break;
	}
}

QString XWBibTeX::getFileName()
{
	QByteArray ba;
	for (int k = 1; k <= name_length; k++)
	{
		if (name_of_file[k] < 256)
			ba.append((char)(name_of_file[k]));
		else
		{
			ba.append((char)(name_of_file[k] >> 8));
			ba.append((char)(name_of_file[k] & 0x00FF));
		}
	}
	
	QTextStream streamA(ba);
	
	QString ret = streamA.readAll();
	return ret;
}

void XWBibTeX::getTheTopLevelAuxFileName(int & argc, char**argv)
{
	XWOption long_options[5];
	int current_option = 0;
	int option_index = 0;
	int getopt_return_val = 0;
	verbose = 1 ;
  min_crossrefs = 2 ;
	long_options[0].name = "terse";
	long_options[0].has_arg = 0;
	long_options[0].flag = &verbose;
	long_options[0].val = 0;
	current_option++;
	
	long_options[current_option].name = "min-crossrefs";
	long_options[current_option].has_arg = 1;
	long_options[current_option].flag = 0;
	long_options[current_option].val = 0;
	current_option++;
	
	long_options[current_option].name = "help";
	long_options[current_option].has_arg = 0;
	long_options[current_option].flag = 0;
	long_options[current_option].val = 0;
	current_option++;
	
	long_options[current_option].name = "version";
	long_options[current_option].has_arg = 0;
	long_options[current_option].flag = 0;
	long_options[current_option].val = 0;
	current_option++;
	
	long_options[current_option].name = 0;
	long_options[current_option].has_arg = 0;
	long_options[current_option].flag = 0;
	long_options[current_option].val = 0;
	XWOptionParser parser(argc, argv, &long_options[0], 0);
	QTextStream term(term_out);
	do
	{
		getopt_return_val = parser.getOptLongOnly("", &option_index);
		if (getopt_return_val == '?')
			usage("bibtex");
		else if (parser.isArgument(option_index, "min-crossrefs"))
			min_crossrefs = parser.optArgToInt();
		else if (parser.isArgument(option_index, "help"))
			usageHelp();
		else if (parser.isArgument(option_index, "version"))
		{
			term << tr("This is XWBibTeX, Version 0.9, Xiuwen Corporation.");
			cleanup();
			exit(0);
		}
	} while (getopt_return_val != -1);
	
	if (parser.getOptInd() != argc - 1)
	{
		term << tr("bibtex: Need exactly one file argument.\n");
		cleanup();
		exit(0);
	}
	
	QTextCodec * codec = QTextCodec::codecForLocale();
	QString filename = codec->toUnicode((const char*)((argv[parser.getOptInd()])));
	XWFileName fn(filename);
	QString basename = fn.baseName();
	name_of_file = (int*)malloc((basename.length() + 256) * sizeof(int));
	for (int i = 0; i < basename.length(); i++)
		name_of_file[i + 1] = basename[i].unicode();
		
	aux_name_length = basename.length();
	if ((aux_name_length + length(s_aux_extension) > INT_MAX) || 
        (aux_name_length + length(s_log_extension) > INT_MAX) || 
        (aux_name_length + length(s_bbl_extension) > INT_MAX))
  {
  	samTooLongFileNamePrint();
  	cleanup();
  	exit(1);
  }
  
  name_length = aux_name_length;
  addExtension(s_aux_extension);
  aux_ptr = 0;
  QString auxfilename = basename;
  if (!outputPath.isEmpty())
  	auxfilename = QString("%1/%2").arg(outputPath).arg(basename);
  auxfilename += ".aux";
  cur_aux_file = new QFile(auxfilename);
  if (!cur_aux_file->exists() || !cur_aux_file->open(QIODevice::ReadOnly | QIODevice::Text))
  {
  	delete cur_aux_file;
  	cur_aux_file = 0;
  	samWrongFileNamePrint();
  	exit(1);
  }
  
  name_length = aux_name_length;
  addExtension(s_bbl_extension);
  QString bblfilename = basename;
  if (!outputPath.isEmpty())
  	bblfilename = QString("%1/%2").arg(outputPath).arg(filename);
  bblfilename += ".bbl";
  bbl_file = new QFile(bblfilename);
  if (!bbl_file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
  {
  	delete bbl_file;
  	bbl_file = 0;
  	samWrongFileNamePrint();
  	exit(1);
  }
  
  name_length = aux_name_length;
  addExtension(s_aux_extension);
  name_ptr = 1;
  while (name_ptr <= name_length)
  {
  	buffer[name_ptr] = xord[name_of_file[name_ptr]];
    name_ptr++;
  }
  
  top_lev_str = hash_text[strLookup(buffer,1,aux_name_length,XW_BIBTEX_TEXT_ILK,true)];
	cur_aux_str = hash_text[strLookup(buffer,1,name_length,XW_BIBTEX_AUX_FILE_ILK,true)];
	if (hash_found)
	{
		confusion(tr("Already encountered auxiliary file"));
		cleanup();
		exit(1);
	}
	cur_aux_line = 0;
}

void XWBibTeX::hashCiteConfusion()
{
	confusion(tr("Cite hash error"));
}

void XWBibTeX::idScanningConfusion()
{
	confusion(tr("Identifier scanning error"));
}

void XWBibTeX::illeglLiteralConfusion()
{
	confusion(tr("Illegal literal type"));
}

void XWBibTeX::initCommandExecution()
{
	lit_stk_ptr = 0;
	cmd_str_ptr = str_ptr;
}

void XWBibTeX::initialize(int & argc, char**argv)
{
	err_count = 0;
	history = 0;
	xchr[32] = ' ';
	xchr[33] = '!';
	xchr[34] = '"';
	xchr[35] = '#';
	xchr[36] = '$';
	xchr[37] = '%';
	xchr[38] = '&';
	xchr[39] = '\'';
	xchr[40] = '(';
	xchr[41] = ')';
	xchr[42] = '*';
	xchr[43] = '+';
	xchr[44] = ',';
	xchr[45] = '-';
	xchr[46] = '.';
	xchr[47] = '/';
	xchr[48] = '0';
	xchr[49] = '1';
	xchr[50] = '2';
	xchr[51] = '3';
	xchr[52] = '4';
	xchr[53] = '5';
	xchr[54] = '6';
	xchr[55] = '7';
	xchr[56] = '8';
	xchr[57] = '9';
	xchr[58] = ':';
	xchr[59] = ';';
	xchr[60] = '<';
	xchr[61] = '=';
	xchr[62] = '>';
	xchr[63] = '?';
	xchr[64] = '@';
	xchr[65] = 'A';
	xchr[66] = 'B';
	xchr[67] = 'C';
	xchr[68] = 'D';
	xchr[69] = 'E';
	xchr[70] = 'F';
	xchr[71] = 'G';
	xchr[72] = 'H';
	xchr[73] = 'I';
	xchr[74] = 'J';
	xchr[75] = 'K';
	xchr[76] = 'L';
	xchr[77] = 'M';
	xchr[78] = 'N';
	xchr[79] = 'O';
	xchr[80] = 'P';
	xchr[81] = 'Q';
	xchr[82] = 'R';
	xchr[83] = 'S';
	xchr[84] = 'T';
	xchr[85] = 'U';
	xchr[86] = 'V';
	xchr[87] = 'W';
	xchr[88] = 'X';
	xchr[89] = 'Y';
	xchr[90] = 'Z';
	xchr[91] = '[';
	xchr[92] = '\\';
	xchr[93] = ']';
	xchr[94] = '^';
	xchr[95] = '_';
	xchr[96] = '`';
	xchr[97] = 'a';
	xchr[98] = 'b';
	xchr[99] = 'c';
	xchr[100] = 'd';
	xchr[101] = 'e';
	xchr[102] = 'f';
	xchr[103] = 'g';
	xchr[104] = 'h';
	xchr[105] = 'i';
	xchr[106] = 'j';
	xchr[107] = 'k';
	xchr[108] = 'l';
	xchr[109] = 'm';
	xchr[110] = 'n';
	xchr[111] = 'o';
	xchr[112] = 'p';
	xchr[113] = 'q';
	xchr[114] = 'r';
	xchr[115] = 's';
	xchr[116] = 't';
	xchr[117] = 'u';
	xchr[118] = 'v';
	xchr[119] = 'w';
	xchr[120] = 'x';
	xchr[121] = 'y';
	xchr[122] = 'z';
	xchr[123] = '{';
	xchr[124] = '|';
	xchr[125] = '}';
	xchr[126] = '~';
	xchr [0 ] = ' ' ;
  xchr [127 ] = ' ' ;
  
  for (int i = 0; i < 32; i++)
		xchr[i] = (uchar)i;
		
	for (int i = 127; i < 256; i++)
		xchr[i] = (uchar)i;
		
	for (int i = 0; i < 256; i++)
		xord[xchr[i]] = (uchar)i;
		
	for (int i = 0; i < 128; i++)
		lex_class[i]  = XW_BIBTEX_OTHER_LEX;
		
	for (int i = 128; i < 256; i++)
		lex_class[i]  = XW_BIBTEX_ALPHA;
		
	for (int i = 0; i < 32; i++)
		lex_class[i]  = XW_BIBTEX_ILLEGAL;
		
	lex_class[XW_BIBTEX_INVALID_CODE] = XW_BIBTEX_ILLEGAL;
	lex_class[XW_BIBTEX_TAB] = XW_BIBTEX_WHITE_SPACE;
	lex_class[13] = XW_BIBTEX_WHITE_SPACE;
	lex_class[XW_BIBTEX_SPACE] = XW_BIBTEX_WHITE_SPACE ;
	lex_class[XW_BIBTEX_TIE] = XW_BIBTEX_SEP_CHAR;	
	lex_class[XW_BIBTEX_HYPHEN] = XW_BIBTEX_SEP_CHAR;
	
	for (int i = 48; i < 58; i++)
		lex_class[i]  = XW_BIBTEX_NUMERIC;
		
	for (int i = 65; i < 91; i++)
		lex_class[i]  = XW_BIBTEX_ALPHA;
		
	for (int i = 97; i < 123; i++)
		lex_class[i]  = XW_BIBTEX_ALPHA;
		
	for (int i = 0; i < 256; i++)
		id_class[i]  = XW_BIBTEX_LEGAL_ID_CHAR;
		
	for (int i = 0; i < 32; i++)
		id_class[i]  = XW_BIBTEX_ILLEGAL_ID_CHAR;
		
	id_class[XW_BIBTEX_SPACE] = XW_BIBTEX_ILLEGAL_ID_CHAR;
	id_class[XW_BIBTEX_TAB] = XW_BIBTEX_ILLEGAL_ID_CHAR;
	id_class[XW_BIBTEX_DOUBLE_QUOTE] = XW_BIBTEX_ILLEGAL_ID_CHAR;
	id_class[XW_BIBTEX_NUMBER_SIGN] = XW_BIBTEX_ILLEGAL_ID_CHAR;
	id_class[XW_BIBTEX_COMMENT] = XW_BIBTEX_ILLEGAL_ID_CHAR;
	id_class[XW_BIBTEX_SINGLE_QUOTE] = XW_BIBTEX_ILLEGAL_ID_CHAR;
	id_class[XW_BIBTEX_LEFT_PAREN] = XW_BIBTEX_ILLEGAL_ID_CHAR;
	id_class[XW_BIBTEX_RIGHT_PAREN] = XW_BIBTEX_ILLEGAL_ID_CHAR;
	id_class[XW_BIBTEX_COMMA] = XW_BIBTEX_ILLEGAL_ID_CHAR;
	id_class[XW_BIBTEX_EQUALS_SIGN] = XW_BIBTEX_ILLEGAL_ID_CHAR;
	id_class[XW_BIBTEX_LEFT_BRACE] = XW_BIBTEX_ILLEGAL_ID_CHAR;
	id_class[XW_BIBTEX_RIGHT_BRACE] = XW_BIBTEX_ILLEGAL_ID_CHAR;
	
	for (int i = 0; i < 128; i++)
		char_width[i]  = 0;
		
	char_width[32 ]= 278 ;
  char_width[33 ]= 278 ;
  char_width[34 ]= 500 ;
  char_width[35 ]= 833 ;
  char_width[36 ]= 500 ;
  char_width[37 ]= 833 ;
  char_width[38 ]= 778 ;
  char_width[39 ]= 278 ;
  char_width[40 ]= 389 ;
  char_width[41 ]= 389 ;
  char_width[42 ]= 500 ;
  char_width[43 ]= 778 ;
  char_width[44 ]= 278 ;
  char_width[45 ]= 333 ;
  char_width[46 ]= 278 ;
  char_width[47 ]= 500 ;
  char_width[48 ]= 500 ;
  char_width[49 ]= 500 ;
  char_width[50 ]= 500 ;
  char_width[51 ]= 500 ;
  char_width[52 ]= 500 ;
  char_width[53 ]= 500 ;
  char_width[54 ]= 500 ;
  char_width[55 ]= 500 ;
  char_width[56 ]= 500 ;
  char_width[57 ]= 500 ;
  char_width[58 ]= 278 ;
  char_width[59 ]= 278 ;
  char_width[60 ]= 278 ;
  char_width[61 ]= 778 ;
  char_width[62 ]= 472 ;
  char_width[63 ]= 472 ;
  char_width[64 ]= 778 ;
  char_width[65 ]= 750 ;
  char_width[66 ]= 708 ;
  char_width[67 ]= 722 ;
  char_width[68 ]= 764 ;
  char_width[69 ]= 681 ;
  char_width[70 ]= 653 ;
  char_width[71 ]= 785 ;
  char_width[72 ]= 750 ;
  char_width[73 ]= 361 ;
  char_width[74 ]= 514 ;
  char_width[75 ]= 778 ;
  char_width[76 ]= 625 ;
  char_width[77 ]= 917 ;
  char_width[78 ]= 750 ;
  char_width[79 ]= 778 ;
  char_width[80 ]= 681 ;
  char_width[81 ]= 778 ;
  char_width[82 ]= 736 ;
  char_width[83 ]= 556 ;
  char_width[84 ]= 722 ;
  char_width[85 ]= 750 ;
  char_width[86 ]= 750 ;
  char_width[87 ]= 1028 ;
  char_width[88 ]= 750 ;
  char_width[89 ]= 750 ;
  char_width[90 ]= 611 ;
  char_width[91 ]= 278 ;
  char_width[92 ]= 500 ;
  char_width[93 ]= 278 ;
  char_width[94 ]= 500 ;
  char_width[95 ]= 278 ;
  char_width[96 ]= 278 ;
  char_width[97 ]= 500 ;
  char_width[98 ]= 556 ;
  char_width[99 ]= 444 ;
  char_width[100 ]= 556 ;
  char_width[101 ]= 444 ;
  char_width[102 ]= 306 ;
  char_width[103 ]= 500 ;
  char_width[104 ]= 556 ;
  char_width[105 ]= 278 ;
  char_width[106 ]= 306 ;
  char_width[107 ]= 528 ;
  char_width[108 ]= 278 ;
  char_width[109 ]= 833 ;
  char_width[110 ]= 556 ;
  char_width[111 ]= 500 ;
  char_width[112 ]= 556 ;
  char_width[113 ]= 528 ;
  char_width[114 ]= 392 ;
  char_width[115 ]= 394 ;
  char_width[116 ]= 389 ;
  char_width[117 ]= 556 ;
  char_width[118 ]= 528 ;
  char_width[119 ]= 722 ;
  char_width[120 ]= 528 ;
  char_width[121 ]= 528 ;
  char_width[122 ]= 444 ;
  char_width[123 ]= 500 ;
  char_width[124 ]= 1000 ;
  char_width[125 ]= 500 ;
  char_width[126 ]= 500 ;
  
  zzzaa = (int*)malloc((XW_BIBTEX_HASH_MAX - XW_BIBTEX_HASH_BASE + 2) * sizeof(int));
  zzzab = (int*)malloc((XW_BIBTEX_HASH_MAX - XW_BIBTEX_HASH_BASE + 2) * sizeof(int));
  zzzac = (int*)malloc((XW_BIBTEX_HASH_MAX - XW_BIBTEX_HASH_BASE + 2) * sizeof(int));
  zzzad = (int*)malloc((XW_BIBTEX_HASH_MAX - XW_BIBTEX_HASH_BASE + 2) * sizeof(int));
  
  for (int i = XW_BIBTEX_HASH_BASE; i <= XW_BIBTEX_HASH_MAX; i++)
  {
  	hash_next[i] = XW_BIBTEX_EMPTY;
    hash_text[i] = 0;
  }
  
  hash_used = XW_BIBTEX_HASH_MAX + 1;
  
  str_start = (int*)malloc((XW_BIBTEX_MAX_STRINGS + 2) * sizeof(int));
  pool_ptr = 0; 
  str_ptr = 1;
  str_start[str_ptr] = pool_ptr;
  bib_ptr = 0;
  bib_seen = false;
  bst_str = 0; 
  bst_seen = false;
  
  cite_list = (int*)malloc((XW_BIBTEX_MAX_CITES + 2) * sizeof(int));
  cite_ptr = 0;
  citation_seen = false;
  all_entries = false;
  
  zzzae = (int*)malloc((XW_BIBTEX_HASH_MAX - XW_BIBTEX_HASH_BASE + 2) * sizeof(int));
  
  global_strs = (int**)malloc(20 * sizeof(int*));
  for (int i = 0; i < 20; i++)
  	global_strs[i] =  (int*)malloc((XW_BIBTEX_GLOB_STR_SIZE + 1) * sizeof(int));
  	
  wiz_def_ptr = 0;
	num_ent_ints = 0;
	num_ent_strs = 0;
	num_fields = 0;
	str_glb_ptr = 0;
	while (str_glb_ptr < XW_BIBTEX_MAX_GLOB_STRS)
	{
		glb_str_ptr[str_glb_ptr] = 0;
    glb_str_end[str_glb_ptr] = 0;
    str_glb_ptr++;
	}
	num_glb_strs = 0;
	entry_seen = false;
	read_seen = false;
	read_performed = false;
	reading_completed = false;
	read_completed = false;
	impl_fn_num = 0;
	type_list = (int*)malloc((XW_BIBTEX_MAX_CITES + 2) * sizeof(int));
	entry_exists = (bool*)malloc((XW_BIBTEX_MAX_CITES + 2) * sizeof(bool));
	cite_info = (int*)malloc((XW_BIBTEX_MAX_CITES + 2) * sizeof(int));
	
	out_buf_length = 0;
	
	lit_stack = (int*)malloc((XW_BIBTEX_LIT_STK_SIZE + 2) * sizeof(int));
	lit_stk_type = (int*)malloc((XW_BIBTEX_LIT_STK_SIZE + 2) * sizeof(int));
	
	name_of_file = 0;
	last = 0;
	name_length = 0;
	buffer = (int*)malloc((XW_BIBTEX_BUF_SIZE + 2) * sizeof(int));
	ex_buf = (int*)malloc((XW_BIBTEX_BUF_SIZE + 2) * sizeof(int));
	out_buf = (int*)malloc((XW_BIBTEX_BUF_SIZE + 2) * sizeof(int));
	sv_buffer = (int*)malloc((XW_BIBTEX_BUF_SIZE + 2) * sizeof(int));
	
	aux_file = (QFile**)malloc((XW_BIBTEX_AUX_STACK_SIZE + 2) * sizeof(QFile*));
	for (int i = 0; i < (XW_BIBTEX_AUX_STACK_SIZE + 2); i++)
		aux_file[i] = 0;
	aux_list = (int*)malloc((XW_BIBTEX_AUX_STACK_SIZE + 2) * sizeof(int));
	aux_ln_stack = (int*)malloc((XW_BIBTEX_AUX_STACK_SIZE + 2) * sizeof(int));
	
	name_tok = (int*)malloc((XW_BIBTEX_BUF_SIZE + 2) * sizeof(int));
	name_sep_char = (int*)malloc((XW_BIBTEX_BUF_SIZE + 2) * sizeof(int));
	
	predefCertainStrings();
	getTheTopLevelAuxFileName(argc, argv);
}

bool XWBibTeX::inputln(QFile * f)
{
	if (f->atEnd())
		return false;
		
	uchar c = EOF;
	while (!f->atEnd())
	{
		if (last > XW_BIBTEX_BUF_SIZE)
		{
			bufferOverflow();
			return false;
		}
			
		if (!f->getChar((char*)(&c)))
			return false;
  		
  	if (c == '\n')
  		goto line_end;
  			
  	if (c == '\r') 
   	{
   		f->getChar((char*)(&c));
   		if (c != '\n')
     			f->ungetChar((char)c);
     	else
     		goto line_end;
  	}
  		
  	buffer[last] = xord[c];
  	last++;
	}
		
line_end:
	while (last > 0)
	{
		if (lex_class[buffer[last - 1]] == XW_BIBTEX_WHITE_SPACE)
			last--;
		else
			break;
	}
	
	return true;
}

void XWBibTeX::intToASCII(int theint, int * int_buf, int int_begin, int * int_end)
{
	int int_ptr = int_begin;
	if (theint < 0)
	{
		if (int_ptr == XW_BIBTEX_BUF_SIZE)
		{
			bufferOverflow();
			return ;
		}
		
		int_buf[int_ptr] = XW_BIBTEX_MINUS_SIGN;
    int_ptr++;
    theint = -theint;
	}
	
	int int_xptr = int_ptr;
	do
	{
		if (int_ptr == XW_BIBTEX_BUF_SIZE)
		{
			bufferOverflow();
			return ;
		}
		
		int_buf[int_ptr] = 48 + ( theint % 10 ) ;
    int_ptr++;
    theint = theint / 10 ;
	} while (theint != 0);
	*int_end = int_ptr;
	int_ptr--;
	while (int_xptr < int_ptr)
	{
		int int_tmp_val = int_buf[int_xptr];
    int_buf[int_xptr] = int_buf[int_ptr];
    int_buf[int_ptr] = int_tmp_val;
    int_ptr--;
    int_xptr++;
	}
}

void XWBibTeX::lastCheckForAuxErrors()
{
	num_cites = cite_ptr;
	num_bib_files = bib_ptr; 
	if (!citation_seen)
	{
		auxEnd1ErrPrint();
		print(tr("\\citation commands"));
		auxEnd2ErrPrint();
	}
	else if ((num_cites == 0) && (! all_entries))
	{
		auxEnd1ErrPrint();
		print(tr("cite keys"));
		auxEnd2ErrPrint();
	}
	
	if (!bib_seen)
	{
		auxEnd1ErrPrint();
		print(tr("\\bibdata command"));
		auxEnd2ErrPrint();
	}
	else if (num_bib_files == 0)
	{
		auxEnd1ErrPrint();
		print(tr("database files"));
		auxEnd2ErrPrint();
	}
	
	if (!bst_seen)
	{
		auxEnd1ErrPrint();
		print(tr("\\bibstyle command"));
		auxEnd2ErrPrint();
	}
	else if (bst_str == 0)
	{
		auxEnd1ErrPrint();
		print(tr("style file"));
		auxEnd2ErrPrint();
	}
}

bool XWBibTeX::lessThan(int arg1, int arg2)
{
	int ptr1 = arg1*num_ent_strs + sort_key_num;
	int ptr2 = arg2*num_ent_strs + sort_key_num;
	int char_ptr = 0;
	while (true)
	{
		int char1 = entry_strs[(ptr1) * (XW_BIBTEX_ENT_STR_SIZE + 1) + (char_ptr)];
    int char2 = entry_strs[(ptr2) * (XW_BIBTEX_ENT_STR_SIZE + 1) + (char_ptr)];
    if (char1 == XW_BIBTEX_END_OF_STRING)
    {
    	if (char2 == XW_BIBTEX_END_OF_STRING)
    	{
    		if (arg1 < arg2)
    			return true;
    		else if (arg1 > arg2)
    			return false;
    		else
    		{
    			confusion(tr("Duplicate sort key"));
    			return false;
    		}
    	}
    	else
    		return true;
    }
    else if (char2 == XW_BIBTEX_END_OF_STRING)
    	return false;
    else if (char1 < char2)
    	return true;
    else if (char1 > char2)
    	return false;
    char_ptr++;
	}
	
	return false;
}

void XWBibTeX::lowerCase(int * buf, int bf_ptr, int len)
{
	if (len > 0)
	{
		for (int i = bf_ptr; i < (bf_ptr+len); i++)
		{
			if (buf[i] >= 'A' && buf[i] <= 'Z')
				buf[i] = buf[i] + 32;
		}
	}
}

void XWBibTeX::macroWarnPrint()
{
	print(tr("Warning--string name '"));
	printToken();
	print(tr("' is "));
}

int XWBibTeX::makeString()
{
	if (str_ptr == XW_BIBTEX_MAX_STRINGS)
	{
		overflow(tr("number of strings "), XW_BIBTEX_MAX_STRINGS);
		return 0;
	}
	
	str_ptr++;
	str_start[str_ptr] = pool_ptr;
	
	return str_ptr - 1;
}

void XWBibTeX::markError()
{
	if (history < XW_BIBTEX_ERROR_MESSAGE)
	{
		history = XW_BIBTEX_ERROR_MESSAGE;
    err_count = 1;
	}
	else
		err_count++;
}

void XWBibTeX::markFatal()
{
	history = XW_BIBTEX_FATAL_MESSAGE;
}

void XWBibTeX::markWarning()
{
	if (history == XW_BIBTEX_WARNING_MESSAGE)
		err_count++;
	else if (history == XW_BIBTEX_SPOTLESS)
	{
		history = XW_BIBTEX_WARNING_MESSAGE;
    err_count = 1;
	}
}

void XWBibTeX::nameScanForAnd(int pop_lit_var)
{
	brace_level = 0;
	preceding_white = false;
	and_found = false;
	while ((!and_found) && (ex_buf_ptr < ex_buf_length))
	{
		switch (ex_buf[ex_buf_ptr])
		{
			case 'a':
			case 'A':
				ex_buf_ptr++;
				if (preceding_white)
				{
					if (ex_buf_ptr <= (ex_buf_length - 3))
					{
						if ((ex_buf[ex_buf_ptr] == 'n') || (ex_buf[ex_buf_ptr] == 'N'))
						{
							if ((ex_buf[ex_buf_ptr+1] == 'd') || (ex_buf[ex_buf_ptr+1] == 'D'))
							{
								if (lex_class[ex_buf[ex_buf_ptr+2]] == XW_BIBTEX_WHITE_SPACE)
								{
									ex_buf_ptr = ex_buf_ptr + 2;
                	and_found = true;
								}
							}
						}
					}
				}
				preceding_white = false;
				break;
				
			case XW_BIBTEX_LEFT_BRACE:
				brace_level++;
				ex_buf_ptr++;
				while ((brace_level > 0) && (ex_buf_ptr < ex_buf_length))
				{
					if (ex_buf[ex_buf_ptr] == XW_BIBTEX_RIGHT_BRACE)
						brace_level--;
					else if (ex_buf[ex_buf_ptr] == XW_BIBTEX_LEFT_BRACE)
						brace_level++;
					ex_buf_ptr++;
				}
				preceding_white = false;
				break;
				
			case XW_BIBTEX_RIGHT_BRACE:
				decrBraceLevel(pop_lit_var);
				ex_buf_ptr++;
				preceding_white = false;				
				break;
				
			default:
				if (lex_class[ex_buf[ex_buf_ptr]] == XW_BIBTEX_WHITE_SPACE)
				{
					ex_buf_ptr++;
					preceding_white = true;
				}
				else
				{
					ex_buf_ptr++;
					preceding_white = false;
				}
				break;
		}
	}
	checkBraceLevel(pop_lit_var);
}

void XWBibTeX::nonexistentCrossReferenceError()
{
	print(tr("A bad cross reference-"));
	badCrossReferencePrint(field_info[field_ptr]);
	println(tr("', which doesn't exist"));
	markError();
}

void XWBibTeX::outPoolStr(QFile * f, int s)
{
	if ((s < 0) || 
		  (s >= (str_ptr + XW_BIBTEX_MAX_POP)) || 
		  (s >= XW_BIBTEX_MAX_STRINGS))
	{
		QString msg = QString(tr("Illegal string number:%1")).arg(s);
		confusion(msg);
		return ;
	}
	
	for (int i = str_start[s]; i < str_start[s+1]; i++)
		f->putChar((char)(xchr[str_pool[i]]));
}

void XWBibTeX::outputBblLine()
{
	if (out_buf_length != 0)
	{
		while (out_buf_length > 0)
		{
			if (lex_class[out_buf[out_buf_length-1]] == XW_BIBTEX_WHITE_SPACE)
				out_buf_length--;
			else
				goto loop_exit;
		}
		
loop_exit:
		if (out_buf_length == 0)
			return ;
			
		out_buf_ptr = 0;
		while (out_buf_ptr < out_buf_length)
		{
			bbl_file->putChar((char)(xchr[out_buf[out_buf_ptr]]));
			out_buf_ptr++;
		}
	}
	
	bbl_file->putChar('\n');
	bbl_line_num++;
	out_buf_length = 0; 
}

void XWBibTeX::outToken(QFile * f)
{
	int i = buf_ptr1 ;
  while ( ( i < buf_ptr2 ) ) 
  {
    f->putChar((char)xchr[buffer[i ]]);
    i = i + 1 ;
  } 
}

void XWBibTeX::overflow(const QString & str, int c)
{
	printOverflow();
	QString msg = QString("%1:%2\n").arg(str).arg(c);
	QTextStream log(log_file);
	log << msg;
	QTextStream term(term_out);
	term << msg;
	
	cleanup();
	exit(1);
}

void XWBibTeX::poolOverflow()
{
	overflow("str_pool", pool_size);
}

void XWBibTeX::popLitStk(int * pop_lit, int * pop_type)
{
	if (lit_stk_ptr == 0)
	{
		print(tr("You can''t pop an empty literal stack"));
		bstExWarnPrint();
		*pop_type = XW_BIBTEX_STK_EMPTY ;
	}
	else
	{
		lit_stk_ptr--;
		*pop_lit = lit_stack[lit_stk_ptr];
    *pop_type = lit_stk_type[lit_stk_ptr];
    if (*pop_type == XW_BIBTEX_STK_STR)
    {
    	if (*pop_lit >= cmd_str_ptr)
    	{
    		if (!pop_lit != str_ptr-1)
    		{
    			confusion(tr("Nontop top of string stack"));
    			return ;
    		}
    		flushString();
    	}
    }
	}
}

void XWBibTeX::popTheAuxStack()
{
	if (cur_aux_file)
	{
		cur_aux_file->close();
		delete cur_aux_file;
		cur_aux_file = 0;
	}
	if (aux_ptr != 0)
		aux_ptr--;
}

void XWBibTeX::popTopAndPrint()
{
	int stk_lt, stk_tp;
	popLitStk(&stk_lt,&stk_tp);
	if (stk_tp == XW_BIBTEX_STK_EMPTY)
		println(tr("Empty literal"));
	else
		printLit(stk_lt,stk_tp);
}

void XWBibTeX::popWholeStack()
{
	while (lit_stk_ptr > 0)
		popTopAndPrint();
}

void XWBibTeX::predefCertainStrings()
{
	predefine(".aux        ",4,XW_BIBTEX_FILE_EXT_ILK);
	s_aux_extension = hash_text[pre_def_loc];
	predefine(".bbl        ",4,XW_BIBTEX_FILE_EXT_ILK);
	s_bbl_extension = hash_text[pre_def_loc];
	predefine(".blg        ",4,XW_BIBTEX_FILE_EXT_ILK);
	s_log_extension = hash_text[pre_def_loc];
	predefine(".bst        ",4,XW_BIBTEX_FILE_EXT_ILK);
	s_bst_extension = hash_text[pre_def_loc];
	predefine(".bib        ",4,XW_BIBTEX_FILE_EXT_ILK);
	s_bib_extension = hash_text[pre_def_loc];
	predefine("texinputs:  ",10,XW_BIBTEX_FILE_AREA_ILK);
	s_bst_area = hash_text[pre_def_loc];
	predefine("texbib:     ",7,XW_BIBTEX_FILE_AREA_ILK);
	s_bib_area = hash_text[pre_def_loc];
	predefine("\\citation   ",9,XW_BIBTEX_AUX_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_AUX_CITATION;
	predefine("\\bibdata    ",8,XW_BIBTEX_AUX_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_AUX_BIBDATA;
	predefine("\\bibstyle   ",9,XW_BIBTEX_AUX_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_AUX_BIBSTYLE;
	predefine("\\@input     ",7,XW_BIBTEX_AUX_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_AUX_INPUT;
	predefine("entry       ",5,XW_BIBTEX_BST_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_BST_ENTRY;
	predefine("execute     ",7,XW_BIBTEX_BST_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_BST_EXECUTE;
	predefine("function    ",8,XW_BIBTEX_BST_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_BST_FUNCTION;
	predefine("integers    ",8,XW_BIBTEX_BST_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_BST_INTEGERS;
	predefine("iterate     ",7,XW_BIBTEX_BST_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_BST_ITERATE;
	predefine("macro       ",5,XW_BIBTEX_BST_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_BST_MACRO;
	predefine("read        ",4,XW_BIBTEX_BST_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_BST_READ;
	predefine("reverse     ",7,XW_BIBTEX_BST_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_BST_REVERSE;
	predefine("sort        ",4,XW_BIBTEX_BST_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_BST_SORT;
	predefine("strings     ",7,XW_BIBTEX_BST_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_BST_STRINGS;
	predefine("comment     ",7,XW_BIBTEX_BIB_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_BIB_COMMENT;
	predefine("preamble    ",8,XW_BIBTEX_BIB_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_BIB_PREAMBLE;
	predefine("string      ",6,XW_BIBTEX_BIB_COMMAND_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_BIB_STRING;
	buildIn("=           ",1,b_equals,XW_BIBTEX_N_EQUALS);
	buildIn(">           ",1,b_greater_than,XW_BIBTEX_N_GREATER_THAN);
	buildIn("<           ",1,b_less_than,XW_BIBTEX_N_LESS_THAN);
	buildIn("+           ",1,b_plus,XW_BIBTEX_N_PLUS);
	buildIn("-           ",1,b_minus,XW_BIBTEX_N_MINUS);
	buildIn("*           ",1,b_concatenate,XW_BIBTEX_N_CONCATENATE);
	buildIn(":=          ",2,b_gets,XW_BIBTEX_N_GETS);
	buildIn("add.period$ ",11,b_add_period,XW_BIBTEX_N_ADD_PERIOD);
	buildIn("call.type$  ",10,b_call_type,XW_BIBTEX_N_CALL_TYPE);
	buildIn("change.case$",12,b_change_case,XW_BIBTEX_N_CHANGE_CASE);
	buildIn("chr.to.int$ ",11,b_chr_to_int,XW_BIBTEX_N_CHR_TO_INT);
	buildIn("cite$       ",5,b_cite,XW_BIBTEX_N_CITE);
	buildIn("duplicate$  ",10,b_duplicate,XW_BIBTEX_N_DUPLICATE);
	buildIn("empty$      ",6,b_empty,XW_BIBTEX_N_EMPTY);
	buildIn("format.name$",12,b_format_name,XW_BIBTEX_N_FORMAT_NAME);
	buildIn("if$         ",3,b_if,XW_BIBTEX_N_IF);
	buildIn("int.to.chr$ ",11,b_int_to_chr,XW_BIBTEX_N_INT_TO_CHR);
	buildIn("int.to.str$ ",11,b_int_to_str,XW_BIBTEX_N_INT_TO_STR);
	buildIn("missing$    ",8,b_missing,XW_BIBTEX_N_MISSING);
	buildIn("newline$    ",8,b_newline,XW_BIBTEX_N_NEWLINE);
	buildIn("num.names$  ",10,b_num_names,XW_BIBTEX_N_NUM_NAMES);
	buildIn("pop$        ",4,b_pop,XW_BIBTEX_N_POP);
	buildIn("preamble$   ",9,b_preamble,XW_BIBTEX_N_PREAMBLE);
	buildIn("purify$     ",7,b_purify,XW_BIBTEX_N_PURIFY);
	buildIn("quote$      ",6,b_quote,XW_BIBTEX_N_QUOTE);
	buildIn("skip$       ",5,b_skip,XW_BIBTEX_N_SKIP);
	buildIn("stack$      ",6,b_stack,XW_BIBTEX_N_STACK);
	buildIn("substring$  ",10,b_substring,XW_BIBTEX_N_SUBSTRING);
	buildIn("swap$       ",5,b_swap,XW_BIBTEX_N_SWAP);
	buildIn("text.length$",12,b_text_length,XW_BIBTEX_N_TEXT_LENGTH);
	buildIn("text.prefix$",12,b_text_prefix,XW_BIBTEX_N_TEXT_PREFIX);
	buildIn("top$        ",4,b_top_stack,XW_BIBTEX_N_TOP_STACK);
	buildIn("type$       ",5,b_type,XW_BIBTEX_N_TYPE);
	buildIn("warning$    ",8,b_warning,XW_BIBTEX_N_WARNING);
	buildIn("width$      ",6,b_width,XW_BIBTEX_N_WIDTH);
	buildIn("while$      ",6,b_while,XW_BIBTEX_N_WHILE);
	buildIn("width$      ",6,b_width,XW_BIBTEX_N_WIDTH);
	buildIn("write$      ",6,b_write,XW_BIBTEX_N_WRITE);
	predefine("            ",0,XW_BIBTEX_TEXT_ILK);  
	s_null = hash_text[pre_def_loc];
	fn_type[pre_def_loc] = XW_BIBTEX_STR_LITERAL;
	predefine("default.type",12,XW_BIBTEX_TEXT_ILK); 
	s_default = hash_text[pre_def_loc];
	fn_type[pre_def_loc] = XW_BIBTEX_STR_LITERAL;
	b_default = b_skip;
	preamble_ptr = 0; 
	predefine("i           ",1,XW_BIBTEX_CONTROL_SEQ_ILK);
	ilk_info[pre_def_loc]= XW_BIBTEX_N_I;
	predefine("j           ",1,XW_BIBTEX_CONTROL_SEQ_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_J;
	predefine("oe          ",2,XW_BIBTEX_CONTROL_SEQ_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_OE;
	predefine("OE          ",2,XW_BIBTEX_CONTROL_SEQ_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_OE_UPPER;
	predefine("ae          ",2,XW_BIBTEX_CONTROL_SEQ_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_AE;
	predefine("AE          ",2,XW_BIBTEX_CONTROL_SEQ_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_AE_UPPER;
	predefine("aa          ",2,XW_BIBTEX_CONTROL_SEQ_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_AA;
	predefine("AA          ",2,XW_BIBTEX_CONTROL_SEQ_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_AA_UPPER;
	predefine("o           ",1,XW_BIBTEX_CONTROL_SEQ_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_O;
	predefine("O           ",1,XW_BIBTEX_CONTROL_SEQ_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_O_UPPER;
	predefine("l           ",1,XW_BIBTEX_CONTROL_SEQ_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_L;
	predefine("L           ",1,XW_BIBTEX_CONTROL_SEQ_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_L_UPPER;
	predefine("ss          ",2,XW_BIBTEX_CONTROL_SEQ_ILK);
	ilk_info[pre_def_loc] = XW_BIBTEX_N_SS;
	predefine("crossref    ",8,XW_BIBTEX_BST_FN_ILK);
	fn_type[pre_def_loc] = XW_BIBTEX_FIELD;
	fn_info[pre_def_loc] = num_fields;
	crossref_num = num_fields;
	num_fields++;
	num_pre_defined_fields = num_fields;
	predefine("sort.key$   ",9,XW_BIBTEX_BST_FN_ILK);
	fn_type[pre_def_loc] = XW_BIBTEX_STR_ENTRY_VAR;
	fn_info[pre_def_loc] = num_ent_strs; 
	sort_key_num = num_ent_strs;
	num_ent_strs++;
	predefine("entry.max$  ",10,XW_BIBTEX_BST_FN_ILK);
	fn_type[pre_def_loc] = XW_BIBTEX_INT_GLOBAL_VAR;
	fn_info[pre_def_loc] = XW_BIBTEX_ENT_STR_SIZE; 
	predefine("global.max$ ",11,XW_BIBTEX_BST_FN_ILK);
	fn_type[pre_def_loc] = XW_BIBTEX_INT_GLOBAL_VAR;
	fn_info[pre_def_loc] = XW_BIBTEX_GLOB_STR_SIZE; 
}

void XWBibTeX::predefine(char * pds, int len, int ilk)
{
	for (int i = 1; i <= len; i++)
		buffer[i] = xord[pds[i - 1]];
		
	pre_def_loc = strLookup(buffer,1,len,ilk,true);
}

void XWBibTeX::print(const QString & s)
{
	QTextStream log(log_file);
	log << s;
	QTextStream term(term_out);
	term << s;
}

void XWBibTeX::print(uchar c)
{
	QString str(QChar((int)c));
	print(str);
}

void XWBibTeX::printAPoolstr(int s)
{
	outPoolStr(term_out, s);
	outPoolStr(log_file, s);
}

void XWBibTeX::printAuxName()
{
	printPoolStr(cur_aux_str);
	printNewLine();
}

void XWBibTeX::printBadInputLine()
{
	print(" : ");
	int bf_ptr = 0;
	while (bf_ptr < buf_ptr2)
	{
		if (lex_class[buffer[bf_ptr]] == XW_BIBTEX_WHITE_SPACE)
			print(xchr[XW_BIBTEX_SPACE]);
		else
			print(xchr[buffer[bf_ptr]]);
			
		bf_ptr++;
	}
	printNewLine();
	print(" : ");
	bf_ptr = 0;
	while (bf_ptr < buf_ptr2)
	{
		print(xchr[XW_BIBTEX_SPACE]);
    bf_ptr++;
	}
	
	bf_ptr = buf_ptr2;
	while (bf_ptr < last)
	{
		if (lex_class[buffer[bf_ptr]] == XW_BIBTEX_WHITE_SPACE)
			print(xchr[XW_BIBTEX_SPACE]);
		else
			print(xchr[buffer[bf_ptr]]);
    bf_ptr++;
	}
	printNewLine();
	bf_ptr = 0;
	while (((bf_ptr < buf_ptr2) && (lex_class[buffer[bf_ptr]] == XW_BIBTEX_WHITE_SPACE)))
	{
		bf_ptr++;
	}
	
	if (bf_ptr == buf_ptr2)
		println(tr("(Error may have been on previous line)"));
		
	markError();
}

void XWBibTeX::printBibName()
{
	printPoolStr(cur_bib_str);
	printPoolStr(s_bib_extension);
	printNewLine();
}

void XWBibTeX::printBstName()
{
	printPoolStr(bst_str);
	printPoolStr(s_bst_extension);
	printNewLine();
}

void XWBibTeX::printConfusion()
{
	QTextStream log(log_file);
	QString str = tr("---this can't happen\n*Please notify Xiuwen Corporation*\n");
	log << str;
	QTextStream term(term_out);
	term << str;
	markFatal();
}

void XWBibTeX::printFnClass(int fn_loc)
{
	switch (fn_loc)
	{
		case XW_BIBTEX_BUILT_IN : 
			print(tr("built-in"));
			break;
			
    case XW_BIBTEX_WIZ_DEFINED : 
    	print(tr("wizard-defined"));
    	break;
    	
    case XW_BIBTEX_INT_LITERAL : 
    	print(tr("integer-literal"));
    	break;
    	
    case XW_BIBTEX_STR_LITERAL : 
    	print(tr("string-literal"));
    	break;
    	
    case XW_BIBTEX_FIELD : 
    	print(tr("field"));
    	break;
    	
    case XW_BIBTEX_INT_ENTRY_VAR : 
    	print(tr("integer-entry-variable"));
    	break;
    	
    case XW_BIBTEX_STR_ENTRY_VAR : 
    	print(tr("string-entry-variable"));
    	break;
    	
    case XW_BIBTEX_INT_GLOBAL_VAR : 
    	print(tr("integer-global-variable"));
    	break;
    	
    case XW_BIBTEX_STR_GLOBAL_VAR : 
    	print(tr("string-global-variable"));
    	break;
    	
    default: 
    	unknwnFunctionClassConfusion();
    	break;
	}
}

void XWBibTeX::printLit(int stk_lt, int stk_tp)
{
	QString str;
	switch (stk_tp)
	{
		case XW_BIBTEX_STK_INT : 
			str = QString("%1").arg(stk_lt);
			println(str);
			break;
			
		case XW_BIBTEX_STK_STR:
			printPoolStr(stk_lt);
      printNewLine();
			break;
			
		case XW_BIBTEX_STK_FN:
			printPoolStr(hash_text[stk_lt]);
      printNewLine();
			break;
			
		case XW_BIBTEX_STK_FIELD_MISSING:
			printPoolStr(stk_lt);
      printNewLine();
			break;
			
		case XW_BIBTEX_STK_EMPTY:
			illeglLiteralConfusion();
			break;
			
		default:
			unknwnLiteralConfusion();
			break;
	}
}

void XWBibTeX::println(const QString & s)
{
	QTextStream log(log_file);
	log << s << "\n";
	QTextStream term(term_out);
	term << s << "\n";
}

void XWBibTeX::printMissingEntry(int s)
{
	print(tr("Warning--I didn't find a database entry for '"));
	printPoolStr(s);
	println("'");
	markWarning();
}

void XWBibTeX::printNewLine()
{
	QTextStream log(log_file);
	log << "\n";
	QTextStream term(term_out);
	term << "\n";
}

void XWBibTeX::printOverflow()
{
	QString str = tr("Sorry---you've exceeded BibTeX's ");
	QTextStream log(log_file);
	log << str;
	QTextStream term(term_out);
	term << str;
	markFatal();
}

void XWBibTeX::printPoolStr(int s)
{
	printAPoolstr(s);
}

void XWBibTeX::printRecursionIllegal()
{
	println(tr("Curse you, wizard, before you recurse me:"));
	print(tr("function "));
	printToken();
	println(tr(" is illegal in its own definition"));
	skipTokenPrint(); 
}

void XWBibTeX::printSkippingWhateverRemains()
{
	print(tr("I'm skipping whatever remains of this "));
}

void XWBibTeX::printStkLit(int stk_lt, int stk_tp)
{
	QString str;
	switch (stk_tp)
	{
		case XW_BIBTEX_STK_INT:
			str = QString(tr("%1 is an integer literal")).arg(stk_lt);
			print(str);
			break;
			
		case XW_BIBTEX_STK_STR:
			print("'");
			printPoolStr(stk_lt);
			print(tr("' is a string literal"));
			break;
			
		case XW_BIBTEX_STK_FN:
			print("'");
			printPoolStr(hash_text[stk_lt]);
			print(tr("' is a function literal"));
			break;
			
		case XW_BIBTEX_STK_FIELD_MISSING:
			print("'");
			printPoolStr(stk_lt);
			print(tr("' is a missing field"));
			break;
			
		case XW_BIBTEX_STK_EMPTY:
			illeglLiteralConfusion();
			break;
			
		default:
			unknwnLiteralConfusion();
			break;
	}
}

void XWBibTeX::printToken()
{
	outToken(log_file) ;
  outToken(term_out) ;
}

void XWBibTeX::printWrongStkLit(int stk_lt, int stk_tp1, int stk_tp2)
{
	if (stk_tp1 != XW_BIBTEX_STK_EMPTY)
	{
		printStkLit(stk_lt, stk_tp1);
		switch (stk_tp2)
		{
			case XW_BIBTEX_STK_INT : 
				print(tr(", not an integer,"));
				break;
				
			case XW_BIBTEX_STK_STR : 
				print(tr(", not a string,"));
				break;
				
      case XW_BIBTEX_STK_FN : 
      case XW_BIBTEX_STK_FIELD_MISSING:
      	print(tr(", not a function,"));
      	break;
      
      case XW_BIBTEX_STK_EMPTY : 
      	illeglLiteralConfusion();
      	break;
      	
      default: 
      	unknwnLiteralConfusion();
      	break;
		}
		bstExWarnPrint();
	}
}

void XWBibTeX::pushLitStk(int push_lt, int push_type)
{
	lit_stack[lit_stk_ptr] = push_lt;
	lit_stk_type[lit_stk_ptr] = push_type;
	if (lit_stk_ptr == XW_BIBTEX_LIT_STK_SIZE)
	{
		overflow(tr("literal-stack size "),XW_BIBTEX_LIT_STK_SIZE);
		return ;
	}
	lit_stk_ptr++;
}

void XWBibTeX::quickSort(int left_end, int right_end)
{
	int left, right, insert_ptr, middle, partition; 
	if (right_end - left_end < XW_BIBTEX_SHORT_LIST)
	{
		for (insert_ptr = left_end+1; insert_ptr <= right_end; insert_ptr++)
		{
			for (right = insert_ptr; right >= left_end+1; right--)
			{
				if (lessThan(sorted_cites[right-1], sorted_cites[right]))
					goto next_insert;
				swap(right-1, right);
			}
next_insert:
			;
		}
	}
	else
	{
		left = left_end + XW_BIBTEX_END_OFFSET;
		middle = (left_end+right_end) / 2;
		right = right_end - XW_BIBTEX_END_OFFSET;
		if (lessThan(sorted_cites[left], sorted_cites[middle]))
		{
			if (lessThan(sorted_cites[middle], sorted_cites[right]))
				swap(left_end,middle);
			else if (lessThan(sorted_cites[left], sorted_cites[right]))
				swap(left_end,right);
			else
				swap(left_end,left);
		}
		else if (lessThan(sorted_cites[right], sorted_cites[middle]))
			swap(left_end,middle);
		else if (lessThan(sorted_cites[right], sorted_cites[left]))
			swap(left_end,right);
		else
			swap(left_end,left);
			
		partition = sorted_cites[left_end];
		left = left_end + 1;
		right = right_end;
		do
		{
			while (lessThan(sorted_cites[left], partition))
				left++;
			while (lessThan(partition, sorted_cites[right]))
				right--;
			if (left < right)
			{
				swap(left,right);
        left++;
        right--;
			}
		} while (left != right + 1);
		swap(left_end,right);
		quickSort(left_end,right-1);
		quickSort(left,right_end);
	}
}

void XWBibTeX::samTooLongFileNamePrint()
{
	QTextStream term(term_out);
	term << tr("File name '");
	name_ptr = 1;
	while (name_ptr <= aux_name_length)
	{
		QString str(QChar((int)(name_of_file[name_ptr])));
		term << str;
		name_ptr++;
	}
	
	term << tr("' is too long\n");
}

void XWBibTeX::samWrongFileNamePrint()
{
	QTextStream term(term_out);
	term << tr("I couldn't open file name '");
	name_ptr = 1;
	while (name_ptr <= name_length)
	{
		QString str(QChar((int)(name_of_file[name_ptr])));
		term << str;
		name_ptr++;
	}
	term << "'\n";
}

bool XWBibTeX::scanAFieldTokenAndEatWhite()
{
	switch (scan_char)
	{
		case XW_BIBTEX_LEFT_BRACE :
			right_str_delim = XW_BIBTEX_RIGHT_BRACE;
			if (!scanBalancedBraces())
				return false;
			break;
			
		case XW_BIBTEX_DOUBLE_QUOTE :
			right_str_delim = XW_BIBTEX_DOUBLE_QUOTE;
			if (!scanBalancedBraces())
				return false;
			break;
			
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
			if (!scanNonnegInteger())
			{
				confusion(tr("A digit disappeared"));
				return false;
			}
			if (store_field)
			{
				tmp_ptr = buf_ptr1;
				while (tmp_ptr < buf_ptr2)
				{
					if (field_end == XW_BIBTEX_BUF_SIZE)
					{
						bibFieldTooLongPrint();
						return false;
					}
					else
					{
						field_vl_str[field_end] = buffer[tmp_ptr];
						field_end++;
					}
				}
			}
			break;
			
		default:
			scanIdentifier(XW_BIBTEX_COMMA,right_outer_delim,XW_BIBTEX_CONCAT_CHAR);
			if ((scan_result == XW_BIBTEX_WHITE_ADJACENT) || 
			(scan_result == XW_BIBTEX_SPECIFIED_CHAR_ADJACENT))
			{	
				;
			}
			else
			{
				bibIdPrint();
				print(tr("a field part"));
				bibErrPrint();
				return false;
			}
			if (store_field)
			{
				lowerCase(buffer, buf_ptr1, token_len);
				macro_name_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_MACRO_ILK,false);
				store_token = true;
				if (at_bib_command)
				{
					if (command_num == XW_BIBTEX_N_BIB_STRING)
					{
						if (macro_name_loc == cur_macro_loc)
						{
							store_token = false;
							macroWarnPrint();
							print(tr("used in its own definition"));
							bibWarnPrint();
						}
					}
				}
				if (!hash_found)
				{
					store_token = false;
					macroWarnPrint();
					print(tr("undefined"));
					bibWarnPrint();
				}
				
				if (store_token)
				{
					tmp_ptr = str_start[ilk_info[macro_name_loc]];
					tmp_end_ptr = str_start[ilk_info[macro_name_loc]+1];
					if (field_end == 0)
					{
						if ((lex_class[str_pool[tmp_ptr]] == XW_BIBTEX_WHITE_SPACE) && 
							  (tmp_ptr < tmp_end_ptr))
						{
							if (field_end == XW_BIBTEX_BUF_SIZE)
							{
								bibFieldTooLongPrint();
								return false;
							}
							else
							{
								field_vl_str[field_end] = XW_BIBTEX_SPACE;
								field_end++;
							}
							tmp_ptr++;
							while ((lex_class[str_pool[tmp_ptr]] == XW_BIBTEX_WHITE_SPACE) &&
                     (tmp_ptr <  tmp_end_ptr))
							{
								tmp_ptr++;
							}
						}
					}
					
					while (tmp_ptr < tmp_end_ptr)
					{
						if (lex_class[str_pool[tmp_ptr]] != XW_BIBTEX_WHITE_SPACE)
						{
							if (field_end == XW_BIBTEX_BUF_SIZE)
							{
								bibFieldTooLongPrint();
								return false;
							}
							else
							{
								field_vl_str[field_end] = str_pool[tmp_ptr];
								field_end++;
							}
						}
						else if (field_vl_str[field_end-1] != XW_BIBTEX_SPACE)
						{
							if (field_end == XW_BIBTEX_BUF_SIZE)
							{
								bibFieldTooLongPrint();
								return false;
							}
							else
							{
								field_vl_str[field_end] = XW_BIBTEX_SPACE;
								field_end++;
							}
						}
						
						tmp_ptr++;
					}
				}
			}
			break;
	}
	
	if (!eatBibWhiteSpace())
	{
		eatBibPrint();
		return false;
	}
	return true;
}

bool XWBibTeX::scanAlpha()
{
	buf_ptr1 = buf_ptr2;
	while (((lex_class[scan_char] == XW_BIBTEX_ALPHA) && (buf_ptr2 < last)))
	{
		buf_ptr2++;
	}
	if (token_len == 0)
		return false;
		
	return true;
}

bool XWBibTeX::scanAndStoreTheFieldValueAndEatWhite()
{
	field_end = 0;
	if (!scanAFieldTokenAndEatWhite())
		return false;
		
	while (scan_char == XW_BIBTEX_CONCAT_CHAR)
	{
		buf_ptr2++;
		if (!eatBibWhiteSpace())
		{
			eatBibPrint();
			return false;
		}
		
		if (!scanAFieldTokenAndEatWhite())
			return false;
	}
	
	if (store_field)
	{
		if (!at_bib_command)
		{
			if (field_end > 0)
			{
				if (field_vl_str[field_end-1] == XW_BIBTEX_SPACE)
					field_end--;
			}
		}
		
		if ((!at_bib_command) && 
			  (field_vl_str[0] == XW_BIBTEX_SPACE) && 
			  (field_end > 0))
		{
			field_start = 1;
		}
		else
			field_start = 0;
			
		field_val_loc = strLookup(field_vl_str,field_start,field_end-field_start, XW_BIBTEX_TEXT_ILK,true);
		fn_type[field_val_loc] = XW_BIBTEX_STR_LITERAL; 
		if (at_bib_command)
		{
			switch (command_num)
			{
				case XW_BIBTEX_N_BIB_PREAMBLE:
					s_preamble[preamble_ptr] = hash_text[field_val_loc];
					preamble_ptr++;
					break;
					
				case XW_BIBTEX_N_BIB_STRING:
					ilk_info[cur_macro_loc] = hash_text[field_val_loc];
					break;
					
				default:
					bibCmdConfusion();
					break;
			}
		}
		else
		{
			field_ptr = entry_cite_ptr * num_fields + fn_info[field_name_loc];
			if (field_info[field_ptr] != XW_BIBTEX_MISSING)
			{
				print(tr("Warning--I'm ignoring "));
				printPoolStr(cite_list[entry_cite_ptr]);
				print(tr("'s extra '"));
				printPoolStr(hash_text[field_name_loc]);
				print(tr("' field"));
				bibWarnPrint();
				return false;
			}
			else
			{
				field_info[field_ptr] = hash_text[field_val_loc];
				if ((fn_info[field_name_loc] == crossref_num) && (!all_entries))
				{
					tmp_ptr = field_start;
					while (tmp_ptr < field_end)
					{
						extra_buf[tmp_ptr] = field_vl_str[tmp_ptr];
						tmp_ptr++;
					}
					lowerCase(extra_buf, field_start, field_end-field_start);
					lc_cite_loc = strLookup(extra_buf,field_start,field_end-field_start,XW_BIBTEX_LC_CITE_ILK,true);
					if (hash_found)
					{
						cite_loc = ilk_info[lc_cite_loc];
						if (ilk_info[cite_loc] >= old_num_cites)
							cite_info[ilk_info[cite_loc]]++;
					}
					else
					{
						cite_loc = strLookup(field_vl_str,field_start,field_end-field_start, XW_BIBTEX_CITE_ILK,true);
						if (hash_found)
							hashCiteConfusion();
						addDatabaseCite(cite_ptr);
						cite_info[ilk_info[cite_loc]] = 1;
					}
				}
			}
		}
	}
	
	return true;
}

bool XWBibTeX::scanBalancedBraces()
{
	buf_ptr2++;
	if ((lex_class[scan_char] == XW_BIBTEX_WHITE_SPACE) || 
		  (buf_ptr2 == last))
	{
		if (!compressBibWhite())
			return false;
	}
	
	if (field_end > 1)
	{
		if (field_vl_str[field_end-1] == XW_BIBTEX_SPACE)
		{
			if (field_vl_str[field_end-2] == XW_BIBTEX_SPACE)
				field_end--;
		}
	}
	
	bib_brace_level = 0;
	if (store_field)
	{
		while (scan_char != right_str_delim)
		{
			switch (scan_char)
			{
				case XW_BIBTEX_LEFT_BRACE:
					bib_brace_level++;
					if (field_end == XW_BIBTEX_BUF_SIZE)
					{
						bibFieldTooLongPrint();
						return false;
					}
					else
					{
						field_vl_str[field_end] = XW_BIBTEX_LEFT_BRACE;
						field_end++;
					}
					buf_ptr2++;
					if ((lex_class[scan_char] == XW_BIBTEX_WHITE_SPACE) || 
		  				(buf_ptr2 == last))
					{
						if (!compressBibWhite())
							return false;
					}
					while (true)
					{
						switch (scan_char)
						{
							case XW_BIBTEX_RIGHT_BRACE:
								bib_brace_level--;
								if (field_end == XW_BIBTEX_BUF_SIZE)
								{
									bibFieldTooLongPrint();
									return false;
								}
								else
								{
									field_vl_str[field_end] = XW_BIBTEX_RIGHT_BRACE;
									field_end++;
								}
								buf_ptr2++;
								if ((lex_class[scan_char] == XW_BIBTEX_WHITE_SPACE) || 
		  						(buf_ptr2 == last))
								{
									if (!compressBibWhite())
										return false;
								}
								if (bib_brace_level == 0)
									goto loop_exit;
								break;
								
							case XW_BIBTEX_LEFT_BRACE:
								bib_brace_level++;
								if (field_end == XW_BIBTEX_BUF_SIZE)
								{
									bibFieldTooLongPrint();
									return false;
								}
								else
								{
									field_vl_str[field_end] = XW_BIBTEX_LEFT_BRACE;
									field_end++;
								}
								buf_ptr2++;
								if ((lex_class[scan_char] == XW_BIBTEX_WHITE_SPACE) || 
		  						(buf_ptr2 == last))
								{
									if (!compressBibWhite())
										return false;
								}
								break;
								
							default:
								if (field_end == XW_BIBTEX_BUF_SIZE)
								{
									bibFieldTooLongPrint();
									return false;
								}
								else
								{
									field_vl_str[field_end] = scan_char;
									field_end++;
								}
								buf_ptr2++;
								if ((lex_class[scan_char] == XW_BIBTEX_WHITE_SPACE) || 
		  						(buf_ptr2 == last))
								{
									if (!compressBibWhite())
										return false;
								}
								break;
						}
					}
loop_exit:
					break;
					
				case XW_BIBTEX_RIGHT_BRACE:
					bibUnbalancedBracesPrint();
					return false;
					break;
					
				default:
					if (field_end == XW_BIBTEX_BUF_SIZE)
					{
						bibFieldTooLongPrint();
						return false;
					}
					else
					{
						field_vl_str[field_end] = scan_char;
						field_end++;
					}
					buf_ptr2++;
					if ((lex_class[scan_char] == XW_BIBTEX_WHITE_SPACE) || 
		  				(buf_ptr2 == last))
					{
						if (!compressBibWhite())
							return false;
					}
					break;
			}
		}
	}
	else
	{
		while (scan_char != right_str_delim)
		{
			if (scan_char == XW_BIBTEX_LEFT_BRACE)
			{
				bib_brace_level++;
				buf_ptr2++;
				if (!eatBibWhiteSpace())
				{
					eatBibPrint();
					return false;
				}
				
				while (bib_brace_level > 0)
				{
					if (scan_char == XW_BIBTEX_RIGHT_BRACE)
					{
						bib_brace_level--;
						buf_ptr2++;
						if (!eatBibWhiteSpace())
						{
							eatBibPrint();
							return false;
						}
					}
					else if (scan_char == XW_BIBTEX_LEFT_BRACE)
					{
						bib_brace_level++;
						buf_ptr2++;
						if (!eatBibWhiteSpace())
						{
							eatBibPrint();
							return false;
						}
					}
					else
					{
						buf_ptr2++;
						if (!scan2(XW_BIBTEX_RIGHT_BRACE, XW_BIBTEX_LEFT_BRACE))
						{
							if (!eatBibWhiteSpace())
							{	
								eatBibPrint();
								return false;
							}
						}
					}
				}
			}
			else if (scan_char == XW_BIBTEX_RIGHT_BRACE)
			{
				bibUnbalancedBracesPrint();
				return false;
			}
			else
			{
				buf_ptr2++;
				if (!scan3(right_str_delim, XW_BIBTEX_LEFT_BRACE, XW_BIBTEX_RIGHT_BRACE))
				{
					if (!eatBibWhiteSpace())
					{	
						eatBibPrint();
						return false;
					}
				}
			}
		}
	}
	buf_ptr2++;
	return true;
}

void XWBibTeX::scanFnDef(int fn_hash_loc)
{
	if (!eatBstWhiteSpace())
	{
		eatBstPrint();
		print(tr("function"));
		bstErrPrintAndLookForBlankLine();
		return ;
	}
	
	int singl_function[XW_BIBTEX_SINGLE_FN_SPACE + 1];
	int copy_ptr, impl_fn_loc;
	int end_of_num = 0;
	int single_ptr = 0;
	while (scan_char != XW_BIBTEX_RIGHT_BRACE)
	{
		switch (scan_char)
		{
			case XW_BIBTEX_NUMBER_SIGN:
				buf_ptr2++;
				if (!scanInteger())
				{
					print(tr("Illegal integer in integer literal"));
          skipTokenPrint();
          goto next_token;
				}
				
				literal_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_INTEGER_ILK,true);
				if (!hash_found)
				{
					fn_type[literal_loc] = XW_BIBTEX_INT_LITERAL;
					fn_info[literal_loc] = token_value;
				}
				
				if ((lex_class[scan_char] != XW_BIBTEX_WHITE_SPACE) && 
					  (buf_ptr2<last) && 
            (scan_char != XW_BIBTEX_RIGHT_BRACE) && 
            (scan_char != XW_BIBTEX_COMMENT))
        {
        	skipIllegalStuffAfterTokenPrint();
          goto next_token;
        }
        singl_function[single_ptr] = literal_loc;
        if (single_ptr == XW_BIBTEX_SINGLE_FN_SPACE)
        {
        	singlFnOverflow();
        	goto next_token;
        }
        single_ptr++;
				break;
				
			case XW_BIBTEX_DOUBLE_QUOTE:
				buf_ptr2++;
				if (!scan1(XW_BIBTEX_DOUBLE_QUOTE))
				{
					QString str = QString(tr("'%1' to end string literal")).arg((char)(xchr[XW_BIBTEX_DOUBLE_QUOTE]));
					print(str);
          skipTokenPrint();
          goto next_token;
				}
				literal_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_TEXT_ILK,true);
				fn_type[literal_loc] = XW_BIBTEX_STR_LITERAL;
				buf_ptr2++;
				if ((lex_class[scan_char] != XW_BIBTEX_WHITE_SPACE) && 
						(buf_ptr2<last) && 
        		(scan_char != XW_BIBTEX_RIGHT_BRACE) && 
        		(scan_char != XW_BIBTEX_COMMENT))
        {
        	skipIllegalStuffAfterTokenPrint();
          goto next_token;
        }
        singl_function[single_ptr] = literal_loc;
        if (single_ptr == XW_BIBTEX_SINGLE_FN_SPACE)
        {
        	singlFnOverflow();
        	goto next_token;
        }
        single_ptr++;
				break;
				
			case XW_BIBTEX_SINGLE_QUOTE:
				buf_ptr2++;
				scan2White(XW_BIBTEX_RIGHT_BRACE,XW_BIBTEX_COMMENT);
				lowerCase(buffer, buf_ptr1, token_len);
				fn_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BST_FN_ILK,false);
				if (!hash_found)
				{
					skpTokenUnknownFunctionPrint();
					goto next_token;
				}
				else
				{
					if (fn_loc == wiz_loc)
					{
						printRecursionIllegal();
						goto next_token;
					}
					else
					{
						singl_function[single_ptr] = quote_next_fn;
        		if (single_ptr == XW_BIBTEX_SINGLE_FN_SPACE)
        		{
        			singlFnOverflow();
        			goto next_token;
        		}
        		single_ptr++;
        		
        		singl_function[single_ptr] = fn_loc;
        		if (single_ptr == XW_BIBTEX_SINGLE_FN_SPACE)
        		{
        			singlFnOverflow();
        			goto next_token;
        		}
        		single_ptr++;
					}
				}
				break;
				
			case XW_BIBTEX_LEFT_BRACE:
				ex_buf[0] = XW_BIBTEX_SINGLE_QUOTE;
				intToASCII(impl_fn_num,ex_buf,1,&end_of_num);
				impl_fn_loc = strLookup(ex_buf2,0,end_of_num,XW_BIBTEX_BST_FN_ILK,true);
				if (hash_found)
				{
					confusion(tr("Already encountered implicit function"));
					return;
				}
				impl_fn_num++;
				fn_type[impl_fn_loc] = XW_BIBTEX_WIZ_DEFINED;
				singl_function[single_ptr] = quote_next_fn;
        if (single_ptr == XW_BIBTEX_SINGLE_FN_SPACE)
        {
        	singlFnOverflow();
        	goto next_token;
        }
        single_ptr++;
        singl_function[single_ptr] = impl_fn_loc;
        if (single_ptr == XW_BIBTEX_SINGLE_FN_SPACE)
        {
        	singlFnOverflow();
        	goto next_token;
        }
        single_ptr++;
        buf_ptr2++;
        scanFnDef(impl_fn_loc);
				break;
				
			default:
				scan2White(XW_BIBTEX_RIGHT_BRACE,XW_BIBTEX_COMMENT);
				lowerCase(buffer, buf_ptr1, token_len);
				fn_loc = strLookup(buffer,buf_ptr1,token_len,XW_BIBTEX_BST_FN_ILK,false);
				if (!hash_found)
				{
					skpTokenUnknownFunctionPrint();
					goto next_token;
				}
				else if (fn_loc == wiz_loc)
				{
					printRecursionIllegal();
					goto next_token;
				}
				else
				{
					singl_function[single_ptr] = fn_loc;
        	if (single_ptr == XW_BIBTEX_SINGLE_FN_SPACE)
        	{
        		singlFnOverflow();
        		goto next_token;
        	}
        	single_ptr++;
				}
				break;
		}
		
next_token:
		if (!eatBstWhiteSpace())
		{
			eatBstPrint();
			print(tr("function"));
			bstErrPrintAndLookForBlankLine();
			return ;
		}
	}
	
	singl_function[single_ptr] = end_of_def;
  if (single_ptr == XW_BIBTEX_SINGLE_FN_SPACE)
  {
  		singlFnOverflow();
  		goto next_token;
  }
  single_ptr++;
  
  if (single_ptr + wiz_def_ptr > wiz_fn_space)
  {
  	int * tmp_num = (int*)malloc((wiz_fn_space + XW_BIBTEX_WIZ_FN_SPACE) * sizeof(int));
  	for (int i = 0; i < wiz_fn_space; i++)
  		tmp_num[i] = wiz_functions[i];
  		
  	free(wiz_functions);
  	wiz_functions = tmp_num;
  	wiz_fn_space += XW_BIBTEX_WIZ_FN_SPACE;
  }
  
  fn_info[fn_hash_loc] = wiz_def_ptr;
  copy_ptr = 0;
  while (copy_ptr < single_ptr)
  {
  	wiz_functions[wiz_def_ptr] = singl_function[copy_ptr];
    copy_ptr++;
    wiz_def_ptr++;
  }
  buf_ptr2++;
}

void XWBibTeX::scanIdentifier(int char1, int char2, int char3)
{
	buf_ptr1 = buf_ptr2;
	if ((lex_class[scan_char] != XW_BIBTEX_NUMERIC))
	{
		while (((id_class[scan_char] == XW_BIBTEX_LEGAL_ID_CHAR) && 
		       (buf_ptr2 < last)))
		{
			buf_ptr2++;
		}
	}
	
	if (token_len == 0)
		scan_result = XW_BIBTEX_ID_NULL;
	else if ((lex_class[scan_char] == XW_BIBTEX_WHITE_SPACE) || (buf_ptr2 == last))
		scan_result = XW_BIBTEX_WHITE_ADJACENT;
	else if ((scan_char == char1) || (scan_char == char2) || (scan_char == char3))
		scan_result = XW_BIBTEX_SPECIFIED_CHAR_ADJACENT;
	else
		scan_result = XW_BIBTEX_OTHER_CHAR_ADJACENT;
}

bool XWBibTeX::scanInteger()
{
	buf_ptr1 = buf_ptr2;
	int sign_length = 0;
	if (scan_char == XW_BIBTEX_MINUS_SIGN)
	{
		 sign_length = 1;
     buf_ptr2++;
	}
	
	token_value = 0;
	while (((lex_class[scan_char] == XW_BIBTEX_NUMERIC) && 
	       (buf_ptr2 < last)))
	{
		token_value = token_value * 10 + char_value;
    buf_ptr2++;
	}
	
	if (sign_length == 1)
		token_value = -token_value;
		
	if (token_len == sign_length)
		return false;
		
	return true;
}

bool XWBibTeX::scanNonnegInteger()
{
	buf_ptr1 = buf_ptr2;
	token_value = 0;
	while (((lex_class[scan_char] == XW_BIBTEX_NUMERIC) && (buf_ptr2 < last)))
	{
		token_value = token_value * 10 + char_value;
    buf_ptr2++;
	}
	
	if (token_len == 0)
		return false;
		
	return true;
}

bool XWBibTeX::scanWhiteSpace()
{
	while (((lex_class[scan_char] == XW_BIBTEX_WHITE_SPACE) && (buf_ptr2 < last)))
	{
		buf_ptr2++;
	}
	
	if (buf_ptr2 < last)
		return true;
		
	return false;
}

bool XWBibTeX::scan1(int char1)
{
	buf_ptr1 = buf_ptr2;
	while (((scan_char != char1) && (buf_ptr2 < last)))
	{
		buf_ptr2++;
	}
	
	if (buf_ptr2 < last)
		return true;
		
	return false;
}

bool XWBibTeX::scan2(int char1, int char2)
{
	buf_ptr1 = buf_ptr2;
	while (((scan_char != char1) && 
	       (scan_char != char2) && 
	       (buf_ptr2 < last)))
	{
		buf_ptr2++;
	}
	if (buf_ptr2 < last)
		return true;
		
	return false;
}

bool XWBibTeX::scan3(int char1, int char2, int char3)
{
	buf_ptr1 = buf_ptr2;
	while (((scan_char != char1) && 
	        (scan_char != char2) && 
	        (scan_char != char3) && 
	        (buf_ptr2 < last)))
	{
		buf_ptr2++;
	}
	if (buf_ptr2 < last)
		return true;
		
	return false;
}

bool XWBibTeX::scan1White(int char1)
{
	buf_ptr1 = buf_ptr2;
	while (((lex_class[scan_char] != XW_BIBTEX_WHITE_SPACE) && 
	       (scan_char != char1) && 
	       (buf_ptr2 < last)))
	{
		buf_ptr2++;
	}
	if (buf_ptr2 < last)
		return true;
		
	return false;
}

bool XWBibTeX::scan2White(int char1, int char2)
{
	buf_ptr1 = buf_ptr2;
	while (((scan_char != char1) && 
	       (scan_char != char2) && 
	       (lex_class[scan_char] != XW_BIBTEX_WHITE_SPACE) && 
	       (buf_ptr2 < last)))
	{
		buf_ptr2++;
	}
	if (buf_ptr2 < last)
		return true;
		
	return false;
}

void XWBibTeX::singlFnOverflow()
{
	overflow(tr("single function space "),XW_BIBTEX_SINGLE_FN_SPACE);
}

void XWBibTeX::skipIllegalStuffAfterTokenPrint()
{
	QString str = QString(tr("'%1' can't follow a literal")).arg((char)(xchr[scan_char]));
	print(str);
	skipTokenPrint();
}

void XWBibTeX::skipStuffAtSpBraceLevelGreaterThanOne()
{
	while ((sp_brace_level > 1) && (sp_ptr < sp_end))
	{
		if (str_pool[sp_ptr] == XW_BIBTEX_RIGHT_BRACE)
			sp_brace_level--;
		else if (str_pool[sp_ptr] == XW_BIBTEX_LEFT_BRACE)
			sp_brace_level++;
		sp_ptr++;
	}
}

void XWBibTeX::skipTokenPrint()
{
	print(tr("-"));
	bstLnNumPrint();
	markError();
	scan2White(XW_BIBTEX_RIGHT_BRACE,XW_BIBTEX_COMMENT);
}

void XWBibTeX::skpTokenUnknownFunctionPrint()
{
	printToken();
	print(tr(" is an unknown function"));
	skipTokenPrint(); 
}

void XWBibTeX::startName(int file_name)
{
	if (((str_start [file_name + 1 ]- str_start [file_name ]) > INT_MAX))
	{
		print("File=");
    printPoolStr(file_name);
    println (",");
    fileNmSizeOverflow();
    return ;
	}
	
	name_ptr = 1;
	if (name_of_file)
		free(name_of_file);
		
	name_of_file = (int*)malloc(((str_start[file_name + 1 ]- str_start[file_name ]) + 2) * sizeof(int));
	int p_ptr = str_start[file_name ];
	while((p_ptr < str_start [file_name + 1 ]))
	{
		name_of_file [name_ptr ]= str_pool[p_ptr ];
    name_ptr = name_ptr + 1 ;
    p_ptr = p_ptr + 1 ;
	}
	name_length = (str_start[file_name + 1]- str_start[file_name]) ;
  name_of_file[name_length + 1]= 0 ;
}

bool XWBibTeX::strEqBuf(int s, int * buf, int bf_ptr, int len)
{
	if (length(s) != len)
		return false;
		
	int i = bf_ptr;
	int j = str_start[s];
	while (j < str_start[s+1])
	{
		if (str_pool[j] != buf[i])
			return false;
			
		i++;
		j++;
	}
	
	return true;
}

bool XWBibTeX::strEqStr(int s1, int s2)
{
	if (length(s1) != length(s2))
		return false;
		
	p_ptr1 = str_start[s1];
	p_ptr2 = str_start[s2];
	while ((p_ptr1 < str_start[s1+1]))
	{
		if ((str_pool[p_ptr1] != str_pool[p_ptr2]))
			return false;
			
		p_ptr1++;
    p_ptr2++;
	}
	
	return true;
}

int XWBibTeX::strLookup(int * buf, int j, int l, int ilk, bool insert_it)
{
	int ret = 0;
	int h = 0;
	int k = j;
	while (k < j+l)
	{
		h =h + h + buf[k];
		while (h >= XW_BIBTEX_HASH_PRIME)
			h = h - XW_BIBTEX_HASH_PRIME;
		k++;
	}
	int p = h + XW_BIBTEX_HASH_BASE ;
  hash_found = false ;
  bool old_string = false ;
  int strnum = 0;
  while (true)
  {
  	if (hash_text[p] > 0)
  	{
  		if ((strEqBuf(hash_text[p ], buf , j , l)))
  		{
  			if ((hash_ilk[p ]== ilk)) 
  			{
  				hash_found = true ;
  				goto str_found;
  			}
  			else
  			{
  				old_string = true ;
					strnum = hash_text[p ];
  			}
  		}
  	}
  	
  	if ((hash_next[p ]== XW_BIBTEX_EMPTY))
  	{
  		if (!insert_it)
  			goto str_not_found;
  			
  		if ((hash_text [p ]> 0))
  		{
  			do
  			{
  				if (((hash_used == XW_BIBTEX_HASH_BASE)))
  				{
  					overflow("hash size ", XW_BIBTEX_HASH_SIZE);
  					return 0;
  				}
  				hash_used--;  				
  			} while (hash_text[hash_used ] != 0);
  			hash_next[p] = hash_used;
  			p = hash_used;
  		}
  		
  		if (old_string)
  			hash_text[p] = str_num;
  		else
  		{
  			strRoom(l);
  			k = j;
  			while (k < (j+l))
  			{
  				appendChar(buf[k]);
        	k++;
  			}
  			hash_text[p] = makeString();
  		}
  		
  		hash_ilk[p] = ilk;
  		
  		goto str_found;
  	}
  	
  	p = hash_next[p];
  }
  
str_not_found: 
str_found:
	ret = p;
	return ret;
}

void XWBibTeX::strRoom(int s)
{
	if ((pool_ptr + s) > pool_size)
		poolOverflow();
}

void XWBibTeX::swap(int swap1, int swap2)
{
	int innocent_bystander = sorted_cites[swap2];
	sorted_cites[swap2] = sorted_cites[swap1];
	sorted_cites[swap1] = innocent_bystander;
}

void XWBibTeX::tracePr(const QString & s)
{
	QTextStream log(log_file);
	log << s;
}

void XWBibTeX::tracePrLn(const QString & s)
{
	QTextStream log(log_file);
	log << s << "\n";
}

void XWBibTeX::tracePrNewLine()
{
	QTextStream log(log_file);
	log << "\n";
}

void XWBibTeX::tracePrPoolStr(int s)
{
	outPoolStr(log_file, s);
}

void XWBibTeX::tracePrToken()
{
	outToken(log_file);
}

void XWBibTeX::unknwnFunctionClassConfusion()
{
	confusion(tr("Unknown function class"));
}

void XWBibTeX::unknwnLiteralConfusion()
{
	confusion(tr("Unknown literal type"));
}

void XWBibTeX::upperCase(int * buf, int bf_ptr, int len)
{
	if (len > 0)
	{
		for (int i = bf_ptr; i < (bf_ptr+len); i++)
		{
			if (buf[i] >= 'a' && buf[i] <= 'z')
				buf[i] = buf[i] - 32;
		}
	}
}

void XWBibTeX::usage(const QString & p)
{
	QTextStream term(term_out);
	QString str = QString(tr("Try '%1 --help' for more information.\n")).arg(p);
	term << str;
}

void XWBibTeX::usageHelp()
{
	QTextStream term(term_out);
	term << tr("Usage: bibtex [OPTION]... AUXFILE[.aux]\n");
	term << tr("    Write bibliography for entries in AUXFILE to AUXFILE.bbl.\n\n");
	term << tr("-min-crossrefs=NUMBER  include item after NUMBER cross-refs; default 2\n");
	term << tr("-terse                 do not print progress reports\n");
	term << tr("-help                  display this help and exit\n");
	term << tr("-version               output version information and exit\n");
}

void XWBibTeX::vonNameEndsAndLastNameStartsStuff()
{
	von_end = last_end - 1;
	while (von_end > von_start)
	{
		name_bf_ptr = name_tok[von_end-1];
    name_bf_xptr = name_tok[von_end];
    if (vonTokenFound())
    	return ;
    von_end--;
	}
}

bool XWBibTeX::vonTokenFound()
{
	nm_brace_level = 0;
	while (name_bf_ptr < name_bf_xptr)
	{
		if ((name_buf[name_bf_ptr] >= 'A') && 
			  (name_buf[name_bf_ptr] <= 'Z'))
		{
			return false;
		}
		else if ((name_buf[name_bf_ptr] >= 'a') && 
			       (name_buf[name_bf_ptr] <= 'z'))
		{
			return true;
		}
		else if (name_buf[name_bf_ptr] == XW_BIBTEX_LEFT_BRACE)
		{
			nm_brace_level++;
			name_bf_ptr++;
			if ((name_bf_ptr + 2 < name_bf_xptr) && 
				  (name_buf[name_bf_ptr] == XW_BIBTEX_BACKSLASH))
			{
				name_bf_ptr++;
				name_bf_yptr = name_bf_ptr;
				while ((name_bf_ptr < name_bf_xptr) && 
				       (lex_class[name_buf[name_bf_ptr]] == XW_BIBTEX_ALPHA))
				{
					name_bf_ptr++;
				}
				control_seq_loc = strLookup(name_buf,name_bf_yptr,name_bf_ptr-name_bf_yptr, XW_BIBTEX_CONTROL_SEQ_ILK,false);
				if (hash_found)
				{
					switch (ilk_info[control_seq_loc])
					{
						case XW_BIBTEX_N_OE_UPPER:
    				case XW_BIBTEX_N_AE_UPPER:
    				case XW_BIBTEX_N_AA_UPPER:
   				  case XW_BIBTEX_N_O_UPPER:
    				case XW_BIBTEX_N_L_UPPER:
    					return false;
    					break;
    					
    				case XW_BIBTEX_N_I:
    				case XW_BIBTEX_N_J:
    				case XW_BIBTEX_N_OE:
    				case XW_BIBTEX_N_AE:
    				case XW_BIBTEX_N_AA:
    				case XW_BIBTEX_N_O:
    				case XW_BIBTEX_N_L:
    				case XW_BIBTEX_N_SS:
    					return true;
    					break;
    					
    				default:
    					confusion(tr("Control-sequence hash error"));
    					return false;
    					break;
					}
				}
				while ((name_bf_ptr < name_bf_xptr) && (nm_brace_level > 0))
				{
					if ((name_buf[name_bf_ptr] >= 'A') && 
						  (name_buf[name_bf_ptr] <= 'Z'))
					{
						return false;
					}
					else if ((name_buf[name_bf_ptr] >= 'a') && 
						       (name_buf[name_bf_ptr] <= 'z'))
					{
						return true;
					}
					else if (name_buf[name_bf_ptr] == XW_BIBTEX_RIGHT_BRACE)
						nm_brace_level--;
					else if (name_buf[name_bf_ptr] == XW_BIBTEX_LEFT_BRACE)
						nm_brace_level++;
					name_bf_ptr++;
				}
				return false;
			}
			else
			{
				while ((nm_brace_level > 0) && (name_bf_ptr < name_bf_xptr))
				{
					if (name_buf[name_bf_ptr] == XW_BIBTEX_RIGHT_BRACE)
						nm_brace_level--;
					else if (name_buf[name_bf_ptr] == XW_BIBTEX_LEFT_BRACE)
						nm_brace_level++;
					name_bf_ptr++;
				}
			}
		}
		else
			name_bf_ptr++;
	}
	
	return false;
}

void XWBibTeX::xAddPeriod()
{
	popLitStk(&pop_lit1,&pop_typ1);
	if (pop_typ1 != XW_BIBTEX_STK_STR)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_STR);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else if (length(pop_lit1) == 0)
		pushLitStk(s_null, XW_BIBTEX_STK_STR);
	else
	{
		sp_ptr = str_start[pop_lit1+1];
		sp_end = str_start[pop_lit1];
		while (sp_ptr > sp_end)
		{
			sp_ptr--;
			if (str_pool[sp_ptr] != XW_BIBTEX_RIGHT_BRACE)
				goto loop_exit;
		}
		
loop_exit:
		switch (str_pool[sp_ptr])
		{
			case XW_BIBTEX_PERIOD:
    	case XW_BIBTEX_QUESTION_MARK:
    	case XW_BIBTEX_EXCLAMATION_MARK:
    		if (lit_stack[lit_stk_ptr] >= cmd_str_ptr)
    			unflushString();
    		lit_stk_ptr++;
    		break;
    		
    	default:
    		if (pop_lit1 < cmd_str_ptr)
    		{
    			strRoom(length(pop_lit1)+1);
    			sp_ptr = str_start[pop_lit1];
    			sp_end = str_start[pop_lit1+1];
    			while (sp_ptr < sp_end)
    			{
    				appendChar(str_pool[sp_ptr]);
        		sp_ptr++;
    			}
    		}
    		else
    		{
    			pool_ptr = str_start[pop_lit1+1];
    			strRoom(1);
    		}
    		appendChar(XW_BIBTEX_PERIOD);
				pushLitStk(makeString(), XW_BIBTEX_STK_STR);
    		break;
		}
	}
}

void XWBibTeX::xChangeCase()
{
	popLitStk(&pop_lit1,&pop_typ1);
	popLitStk(&pop_lit2,&pop_typ2);
	if (pop_typ1 != XW_BIBTEX_STK_STR)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_STR);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else if (pop_typ2 != XW_BIBTEX_STK_STR)
	{
		printWrongStkLit(pop_lit2,pop_typ2,XW_BIBTEX_STK_STR);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else
	{
		switch (str_pool[str_start[pop_lit1]])
		{
			case 't':
			case 'T':
				conversion_type = XW_BIBTEX_TITLE_LOWERS;
				break;
				
			case 'l':
			case 'L':
				conversion_type = XW_BIBTEX_ALL_LOWERS;
				break;
				
			case 'u':
			case 'U':
				conversion_type = XW_BIBTEX_ALL_UPPERS;
				break;
				
			default:
				conversion_type = XW_BIBTEX_BAD_CONVERSION;
				break;
		}
		
		if ((length(pop_lit1) != 1) || (conversion_type == XW_BIBTEX_BAD_CONVERSION))
		{
			conversion_type = XW_BIBTEX_BAD_CONVERSION;
    	printPoolStr(pop_lit1);
    	print(tr(" is an illegal case-conversion string"));
    	bstExWarnPrint();
		}
		
		ex_buf_length = 0;
    addBufPool(pop_lit2);
    brace_level = 0;
    ex_buf_ptr = 0;
    while (ex_buf_ptr < ex_buf_length)
    {
    	if (ex_buf[ex_buf_ptr] == XW_BIBTEX_LEFT_BRACE)
    	{
    		brace_level++;
    		if (brace_level != 1)
    			goto ok_pascal_i_give_up;
    			
    		if (ex_buf_ptr + 4 > ex_buf_length)
    			goto ok_pascal_i_give_up;
    		else if (ex_buf[ex_buf_ptr+1] != XW_BIBTEX_BACKSLASH)
    			goto ok_pascal_i_give_up;
    		if (conversion_type == XW_BIBTEX_TITLE_LOWERS)
    		{
    			if (ex_buf_ptr == 0)
    				goto ok_pascal_i_give_up;
    			else if ((prev_colon) && 
    				      (lex_class[ex_buf[ex_buf_ptr-1]] == XW_BIBTEX_WHITE_SPACE))
    			{
    				goto ok_pascal_i_give_up;
    			}
    		}
    		ex_buf_ptr++;
    		while ((ex_buf_ptr < ex_buf_length) && (brace_level > 0))
    		{
    			ex_buf_ptr++;
    			ex_buf_xptr = ex_buf_ptr;
    			while ((ex_buf_ptr < ex_buf_length) &&
                (lex_class[ex_buf[ex_buf_ptr]] == XW_BIBTEX_ALPHA))
					{
						ex_buf_ptr++;
						control_seq_loc = strLookup(ex_buf,ex_buf_xptr,ex_buf_ptr-ex_buf_xptr,XW_BIBTEX_CONTROL_SEQ_ILK,false);
						if (hash_found)
						{
							switch (conversion_type)
							{
								case XW_BIBTEX_TITLE_LOWERS:
    						case XW_BIBTEX_ALL_LOWERS :
    							{
    								switch (ilk_info[control_seq_loc])
    								{
    									case XW_BIBTEX_N_L_UPPER:
            					case XW_BIBTEX_N_O_UPPER:
            					case XW_BIBTEX_N_OE_UPPER:
            					case XW_BIBTEX_N_AE_UPPER:
            					case XW_BIBTEX_N_AA_UPPER :
            						lowerCase(ex_buf, ex_buf_xptr, ex_buf_ptr-ex_buf_xptr);
            						break;
            						
            					default:
            						break;
    								}
    							}
    							break;
    							
    						case XW_BIBTEX_ALL_UPPERS:
    							{
    								switch (ilk_info[control_seq_loc])
    								{
    									case XW_BIBTEX_N_L:
            					case XW_BIBTEX_N_O:
            					case XW_BIBTEX_N_OE:
            					case XW_BIBTEX_N_AE:
            					case XW_BIBTEX_N_AA :
                				upperCase(ex_buf, ex_buf_xptr, ex_buf_ptr-ex_buf_xptr);
                				break;
                				
                			case XW_BIBTEX_N_I:
            					case XW_BIBTEX_N_J:
            					case XW_BIBTEX_N_SS :
            						upperCase(ex_buf, ex_buf_xptr, ex_buf_ptr-ex_buf_xptr);
            						while (ex_buf_xptr < ex_buf_ptr)
            						{
            							ex_buf[ex_buf_xptr-1] = ex_buf[ex_buf_xptr];
            							ex_buf_xptr++;
            						}
            						ex_buf_xptr--;
            						while ((ex_buf_ptr < ex_buf_length) && 
            										(lex_class[ex_buf[ex_buf_ptr]] == XW_BIBTEX_WHITE_SPACE))
            						{
            							ex_buf_ptr++;
            						}
            						tmp_ptr = ex_buf_ptr;
            						while (tmp_ptr < ex_buf_length)
            						{
            							ex_buf[tmp_ptr-(ex_buf_ptr-ex_buf_xptr)] = ex_buf[tmp_ptr];
            							tmp_ptr++;
            						}
            						ex_buf_length = tmp_ptr - (ex_buf_ptr - ex_buf_xptr);
            						ex_buf_ptr = ex_buf_xptr;
            						break;
            						
            					default:
            						break;
    								}
    							}
    							break;
    							
    						case XW_BIBTEX_BAD_CONVERSION:
    							break;
    							
    						default:
    							caseConversionConfusion();
    							break;
							}							
						}
						ex_buf_xptr = ex_buf_ptr;
						while ((ex_buf_ptr < ex_buf_length) && (brace_level > 0) && 
                    (ex_buf[ex_buf_ptr] != XW_BIBTEX_BACKSLASH))
						{
							if (ex_buf[ex_buf_ptr] == XW_BIBTEX_RIGHT_BRACE)
								brace_level--;
							else if (ex_buf[ex_buf_ptr] == XW_BIBTEX_LEFT_BRACE)
								brace_level++;
								
							ex_buf_ptr++;
						}
						
						switch (conversion_type)
						{
							case XW_BIBTEX_TITLE_LOWERS:
    					case XW_BIBTEX_ALL_LOWERS :
        				lowerCase(ex_buf, ex_buf_xptr, ex_buf_ptr-ex_buf_xptr);
        				break;
        				
    					case XW_BIBTEX_ALL_UPPERS :
        				upperCase(ex_buf, ex_buf_xptr, ex_buf_ptr-ex_buf_xptr);
        				break;
        				
    					case XW_BIBTEX_BAD_CONVERSION :
    						break;
    						
    					default:
    						caseConversionConfusion();
    						break;
						}
					}
    		}
    		ex_buf_ptr--;
    		
ok_pascal_i_give_up:
			prev_colon = false;
    	}
    	else if (ex_buf[ex_buf_ptr] == XW_BIBTEX_RIGHT_BRACE)
    	{
    		decrBraceLevel(pop_lit2);
        prev_colon = false;
    	}
    	else if (brace_level == 0)
    	{
    		switch (conversion_type)
    		{
    			case XW_BIBTEX_TITLE_LOWERS:
    				if (ex_buf_ptr == 0)
    					;
    				else if ((prev_colon) && (lex_class[ex_buf[ex_buf_ptr-1]] == XW_BIBTEX_WHITE_SPACE))
    					;
    				else
    					lowerCase(ex_buf, ex_buf_ptr, 1);
    				if (ex_buf[ex_buf_ptr] == XW_BIBTEX_COLON)
    					prev_colon = true;
    				else if (lex_class[ex_buf[ex_buf_ptr]] != XW_BIBTEX_WHITE_SPACE)
    					prev_colon = false;
    				break;
    				
    			case XW_BIBTEX_ALL_LOWERS :
        		lowerCase(ex_buf, ex_buf_ptr, 1);
        		break;
        		
        	case XW_BIBTEX_ALL_UPPERS :
        		upperCase(ex_buf, ex_buf_ptr, 1);
        		break;
        		
        	case XW_BIBTEX_BAD_CONVERSION:
        		break;
        		
        	default:
        		caseConversionConfusion();
        		break;
    		}
    	}
    	ex_buf_ptr++;
    }
    checkBraceLevel(pop_lit2);
    addPoolBufAndPush();
	}
}

void XWBibTeX::xChrToInt()
{
	popLitStk(&pop_lit1,&pop_typ1);
	if (pop_typ1 != XW_BIBTEX_STK_STR)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_STR);
    pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else if (length(pop_lit1) != 1)
	{
		print(",");
		printPoolStr(pop_lit1);
		print(tr("' isn''t a single character"));
		bstExWarnPrint() ;
		pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else
		pushLitStk(str_pool[str_start[pop_lit1]], XW_BIBTEX_STK_INT);
}

void XWBibTeX::xCite()
{
	if (!mess_with_entries)
		bstCantMessWithEntriesPrint();
	else
		pushLitStk(cur_cite_str, XW_BIBTEX_STK_STR);
}

void XWBibTeX::xConcatenate()
{
	popLitStk(&pop_lit1,&pop_typ1);
	popLitStk(&pop_lit2,&pop_typ2);
	if (pop_typ1 != XW_BIBTEX_STK_STR)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_STR);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else if (pop_typ2 != XW_BIBTEX_STK_STR)
	{
		printWrongStkLit(pop_lit2,pop_typ2,XW_BIBTEX_STK_STR);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else
	{
		if (pop_lit2 >= cmd_str_ptr)
		{
			if (pop_lit1 >= cmd_str_ptr)
			{
				str_start[pop_lit1] = str_start[pop_lit1+1];
        unflushString();
        lit_stk_ptr++;
			}
			else if (length(pop_lit2) == 0)
				pushLitStk(pop_lit1, XW_BIBTEX_STK_STR);
			else
			{
				pool_ptr = str_start[pop_lit2+1];
        strRoom(length(pop_lit1));
        sp_ptr = str_start[pop_lit1];
        sp_end = str_start[pop_lit1+1];
        while (sp_ptr < sp_end)
        {
        	appendChar(str_pool[sp_ptr]);
          sp_ptr++;
        }
        pushLitStk(makeString(), XW_BIBTEX_STK_STR);
			}
		}
		else
		{
			if (pop_lit1 >= cmd_str_ptr)
			{
				if (length(pop_lit2) == 0)
				{
					unflushString();
        	lit_stack[lit_stk_ptr] = pop_lit1;
        	lit_stk_ptr++;
				}
				else if (length(pop_lit1) == 0)
					lit_stk_ptr++;
				else
				{
					sp_length = length(pop_lit1);
        	sp2_length = length(pop_lit2);
        	strRoom(sp_length + sp2_length);
        	sp_ptr = str_start[pop_lit1+1];
        	sp_end = str_start[pop_lit1];
        	sp_xptr1 = sp_ptr + sp2_length;
        	while (sp_ptr > sp_end)
        	{
        		sp_ptr--;
        		sp_xptr1--;
        		str_pool[sp_xptr1] = str_pool[sp_ptr];
        	}
        	sp_ptr = str_start[pop_lit2];
        	sp_end = str_start[pop_lit2+1];
        	while (sp_ptr < sp_end)
        	{
        		appendChar(str_pool[sp_ptr]);
            sp_ptr++;
        	}
        	pool_ptr = pool_ptr + sp_length;
        	pushLitStk(makeString(), XW_BIBTEX_STK_STR);
				}
			}
			else
			{
				if (length(pop_lit1) == 0)
					lit_stk_ptr++;
				else if (length(pop_lit2) == 0)
					pushLitStk(pop_lit1, XW_BIBTEX_STK_STR);
				else
				{
					strRoom(length(pop_lit1) + length(pop_lit2));
    			sp_ptr = str_start[pop_lit2];
    			sp_end = str_start[pop_lit2+1];
    			while (sp_ptr < sp_end)
    			{
    				appendChar(str_pool[sp_ptr]);
        		sp_ptr++;
    			}
    			sp_ptr = str_start[pop_lit1];
    			sp_end = str_start[pop_lit1+1];
    			while (sp_ptr < sp_end)
    			{
    				appendChar(str_pool[sp_ptr]);
        		sp_ptr++;
    			}
    			pushLitStk(makeString(), XW_BIBTEX_STK_STR);
				}
			}
		}
	}
}

void XWBibTeX::xDuplicate()
{
	popLitStk(&pop_lit1,&pop_typ1);
	if (pop_typ1 != XW_BIBTEX_STK_STR)
	{
		pushLitStk(pop_lit1, pop_typ1);
    pushLitStk(pop_lit1, pop_typ1);
	}
	else
	{
		if (lit_stack[lit_stk_ptr] >= cmd_str_ptr)
			unflushString();
		lit_stk_ptr++;
		if (pop_lit1 < cmd_str_ptr)
			pushLitStk(pop_lit1, pop_typ1);
		else
		{
			strRoom(length(pop_lit1));
      sp_ptr = str_start[pop_lit1];
      sp_end = str_start[pop_lit1+1];
      while (sp_ptr < sp_end)
      {
      	appendChar(str_pool[sp_ptr]);
      	sp_ptr++;
      }
      pushLitStk(makeString(), XW_BIBTEX_STK_STR);
		}
	}
}

void XWBibTeX::xEmpty()
{
	popLitStk(&pop_lit1,&pop_typ1);
	switch (pop_typ1)
	{
		case XW_BIBTEX_STK_STR:
			sp_ptr = str_start[pop_lit1];
			sp_end = str_start[pop_lit1+1];
			while (sp_ptr < sp_end)
			{
				if (lex_class[str_pool[sp_ptr]] != XW_BIBTEX_WHITE_SPACE)
				{
					pushLitStk(0, XW_BIBTEX_STK_INT);
        	return;
				}
				sp_ptr++;
			}
			pushLitStk(1, XW_BIBTEX_STK_INT);
			break;
			
		case XW_BIBTEX_STK_FIELD_MISSING:
			pushLitStk(1, XW_BIBTEX_STK_INT);
			break;
			
		case XW_BIBTEX_STK_EMPTY:
			pushLitStk(0, XW_BIBTEX_STK_INT);
			break;
			
		default:
			printStkLit(pop_lit1,pop_typ1);
			print(tr(", not a string or missing field,"));
			bstExWarnPrint() ;
			pushLitStk(0, XW_BIBTEX_STK_INT);
			break;
	}
}

void XWBibTeX::xEquals()
{
	popLitStk(&pop_lit1,&pop_typ1);
	popLitStk(&pop_lit2,&pop_typ2);
	if (pop_typ1 != pop_typ2)
	{
		if ((pop_typ1 != XW_BIBTEX_STK_EMPTY) && 
			 (pop_typ2 != XW_BIBTEX_STK_EMPTY))
		{
			 printStkLit(pop_lit1,pop_typ1);
       print(", ");
       printStkLit(pop_lit2,pop_typ2);
       printNewLine();
       print(tr("---they aren't the same literal types"));
       bstExWarnPrint() ;
		}
		pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else if ((pop_typ1 != XW_BIBTEX_STK_INT) && 
		       (pop_typ1 != XW_BIBTEX_STK_STR))
	{
		if (pop_typ1 != XW_BIBTEX_STK_EMPTY)
		{
			printStkLit(pop_lit1,pop_typ1);
			print(tr(", not an integer or a string,"));
      bstExWarnPrint() ;
		}
		pushLitStk (0, XW_BIBTEX_STK_INT);
	}
	else if (pop_typ1 == XW_BIBTEX_STK_INT)
	{
		if (pop_lit2 == pop_lit1)
			pushLitStk(1, XW_BIBTEX_STK_INT);
		else
			pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else if (strEqStr(pop_lit2,pop_lit1))
		pushLitStk(1, XW_BIBTEX_STK_INT);
	else
		pushLitStk(0, XW_BIBTEX_STK_INT);
}

void XWBibTeX::xFormatName()
{
	popLitStk(&pop_lit1,&pop_typ1);
	popLitStk(&pop_lit2,&pop_typ2);
	popLitStk(&pop_lit3,&pop_typ3);
	if (pop_typ1 != XW_BIBTEX_STK_STR)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_STR);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else if (pop_typ2 != XW_BIBTEX_STK_INT)
	{
		printWrongStkLit(pop_lit2,pop_typ2,XW_BIBTEX_STK_INT);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else if (pop_typ3 != XW_BIBTEX_STK_STR)
	{
		printWrongStkLit(pop_lit3,pop_typ3,XW_BIBTEX_STK_STR);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else
	{
		ex_buf_length = 0;
    addBufPool(pop_lit3);
    ex_buf_ptr = 0;
		num_names = 0;
		while ((num_names < pop_lit2) && (ex_buf_ptr < ex_buf_length))
		{
			num_names++;
			ex_buf_xptr = ex_buf_ptr;
    	nameScanForAnd(pop_lit3);
		}
		if (ex_buf_ptr < ex_buf_length)
			ex_buf_ptr = ex_buf_ptr - 4;
		if (num_names < pop_lit2)
		{
			if (pop_lit2 == 1)
				print(tr("There is no name in '"));
			else
			{
				QString str = QString(tr("There aren't %1 names in '")).arg(pop_lit2);
				print(str);
			}
			printPoolStr(pop_lit3);
			print("'");
			bstExWarnPrint() ;
		}
		
		while ((ex_buf_xptr < ex_buf_ptr) &&
           (lex_class[ex_buf[ex_buf_ptr]] == XW_BIBTEX_WHITE_SPACE) && 
           (lex_class[ex_buf[ex_buf_ptr]] == XW_BIBTEX_SEP_CHAR))
		{
			ex_buf_xptr++;
		}
		while (ex_buf_ptr > ex_buf_xptr)
		{
			switch (lex_class[ex_buf[ex_buf_ptr-1]])
			{
				case XW_BIBTEX_WHITE_SPACE:
        case XW_BIBTEX_SEP_CHAR :
        	ex_buf_ptr--;
        	break;
        	
        default:
        	if (ex_buf[ex_buf_ptr-1] == XW_BIBTEX_COMMA)
        	{
        		QString str = QString(tr("Name %1 in '")).arg(pop_lit2);
						print(str);
						printPoolStr(pop_lit3);
            print(tr("' has a comma at the end"));
            bstExWarnPrint() ;
            ex_buf_ptr--;
        	}
        	else
        		goto loop1_exit;
        	break;
			}
		
		}
loop1_exit:
		name_bf_ptr = 0;
		num_commas = 0;
		num_tokens = 0;
		token_starting = true;
		while (ex_buf_xptr < ex_buf_ptr)
		{
			switch (ex_buf[ex_buf_xptr])
			{
				case XW_BIBTEX_COMMA :
					if (num_commas == 2)
					{
						QString str = QString(tr("Too many commas in name %1 of '")).arg(pop_lit2);
						print(str);
						printPoolStr(pop_lit3);
    				print("'");
    				bstExWarnPrint() ;
					}
					else
					{
						num_commas++;
						if (num_commas == 1)
							comma1 = num_tokens;
						else
							comma2 = num_tokens;
						name_sep_char[num_tokens] = XW_BIBTEX_COMMA;
					}
					ex_buf_xptr++;
					token_starting = true;
					break;
					
				case XW_BIBTEX_LEFT_BRACE:
					brace_level++;
					if (token_starting)
					{
						name_tok[num_tokens] = name_bf_ptr;
						num_tokens++;
					}
					name_buf[name_bf_ptr] = ex_buf[ex_buf_xptr];
					name_bf_ptr++;
					ex_buf_xptr++;
					while ((brace_level > 0) && (ex_buf_xptr < ex_buf_ptr))
					{
						if (ex_buf[ex_buf_xptr] == XW_BIBTEX_RIGHT_BRACE)
							brace_level--;
						else  if (ex_buf[ex_buf_xptr] == XW_BIBTEX_LEFT_BRACE)
							brace_level++;
						name_buf[name_bf_ptr] = ex_buf[ex_buf_xptr];
						name_bf_ptr++;
						ex_buf_xptr++;
					}
					token_starting = false;
					break;
					
				case XW_BIBTEX_RIGHT_BRACE:
					if (token_starting)
					{
						name_tok[num_tokens] = name_bf_ptr;
						num_tokens++;						
					}
					{
						QString str = QString(tr("Name %1' of '")).arg(pop_lit2);
						print(str);
					}
					printPoolStr(pop_lit3);
					print(tr("' isn''t brace balanced"));
					ex_buf_xptr++;
					token_starting = false;
					break;
					
				default:
					{
						switch (lex_class[ex_buf[ex_buf_xptr]])
						{
							case XW_BIBTEX_WHITE_SPACE:
								if (!token_starting)
									name_sep_char[num_tokens] = XW_BIBTEX_SPACE;
								ex_buf_xptr++;
								token_starting = true;
								break;
								
							case XW_BIBTEX_SEP_CHAR:
								if (!token_starting)
									name_sep_char[num_tokens] = ex_buf[ex_buf_xptr];
								ex_buf_xptr++;
								token_starting = true;
								break;
								
							default:
								if (token_starting)
								{
									name_tok[num_tokens] = name_bf_ptr;
									num_tokens++;
								}
								name_buf[name_bf_ptr] = ex_buf[ex_buf_xptr];
								name_bf_ptr++;
								ex_buf_xptr++;
								token_starting = false;
								break;
						}
					}
					break;
			}
		}
		
		name_tok[num_tokens] = name_bf_ptr;
		if (num_commas == 0)
		{
			first_start = 0;
    	last_end = num_tokens;
    	jr_end = last_end;
    	von_start = 0;
    	while (von_start < last_end-1)
    	{
    		name_bf_ptr = name_tok[von_start];
    		name_bf_xptr = name_tok[von_start+1];
    		if (vonTokenFound())
    		{
    			vonNameEndsAndLastNameStartsStuff();
        	goto von_found;
    		}
    		von_start++;
    	}
    	
    	while (von_start > 0)
    	{
    		if ((lex_class[name_sep_char[von_start]] != XW_BIBTEX_SEP_CHAR) ||
            (name_sep_char[von_start] == XW_BIBTEX_TIE))
				{
					goto loop2_exit;
				}
				von_start--;
    	}
    	
loop2_exit:
			von_end = von_start;
			
von_found:
			first_end = von_start;
		}
		else if (num_commas == 1)
		{
			von_start = 0;
    	last_end = comma1;
    	jr_end = last_end;
    	first_start = jr_end;
    	first_end = num_tokens;
    	vonNameEndsAndLastNameStartsStuff();
		}
		else if (num_commas == 2)
		{
			von_start = 0;
    	last_end = comma1;
    	jr_end = comma2;
    	first_start = jr_end;
    	first_end = num_tokens;
    	vonNameEndsAndLastNameStartsStuff();
		}
		else
			confusion(tr("Illegal number of comma,s"));
			
		ex_buf_length = 0;
    addBufPool(pop_lit1);
    figureOutTheFormattedName();
    addPoolBufAndPush();
	}
}

void XWBibTeX::xGets()
{
	popLitStk(&pop_lit1,&pop_typ1);
	popLitStk(&pop_lit2,&pop_typ2);
	if (pop_typ1 != XW_BIBTEX_STK_FN)
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_FN);
	else if ((!mess_with_entries) &&
        ((fn_type[pop_lit1] == XW_BIBTEX_STR_ENTRY_VAR) ||
         (fn_type[pop_lit1] == XW_BIBTEX_INT_ENTRY_VAR)))
	{
		bstCantMessWithEntriesPrint();
	} 
	else
	{
		switch (fn_type[pop_lit1])
		{
			case XW_BIBTEX_INT_ENTRY_VAR:
				if (pop_typ2 != XW_BIBTEX_STK_INT)
					printWrongStkLit(pop_lit2,pop_typ2,XW_BIBTEX_STK_INT);
				else
					entry_ints[cite_ptr*num_ent_ints+fn_info[pop_lit1]] = pop_lit2;
				break;
				
			case XW_BIBTEX_STR_ENTRY_VAR:
				if (pop_typ2 != XW_BIBTEX_STK_STR)
					printWrongStkLit(pop_lit2,pop_typ2,XW_BIBTEX_STK_STR);
				else
				{
					str_ent_ptr = cite_ptr*num_ent_strs + fn_info[pop_lit1];
    			ent_chr_ptr = 0;
    			sp_ptr = str_start[pop_lit2];
    			sp_xptr1 = str_start[pop_lit2+1];
    			if (sp_xptr1-sp_ptr > XW_BIBTEX_ENT_STR_SIZE)
    			{
    				bst1printStringSizeExceeded();
    				QString str = QString(tr("%1, the entry")).arg(XW_BIBTEX_ENT_STR_SIZE);
    				print(str);
    				bst2printStringSizeExceeded();
    				sp_xptr1 = sp_ptr + XW_BIBTEX_ENT_STR_SIZE;
    			}
    			
    			while (sp_ptr < sp_xptr1)
    			{
    				entry_strs[( str_ent_ptr ) * ( XW_BIBTEX_ENT_STR_SIZE + 1 ) + ( ent_chr_ptr ) ] = str_pool[sp_ptr];
        		ent_chr_ptr++;
        		sp_ptr++;
    			}
    			entry_strs[( str_ent_ptr ) * ( XW_BIBTEX_ENT_STR_SIZE + 1 ) + ( ent_chr_ptr )] = XW_BIBTEX_END_OF_STRING;
				}
				break;
				
			case XW_BIBTEX_INT_GLOBAL_VAR:
				if (pop_typ2 != XW_BIBTEX_STK_INT)
					printWrongStkLit(pop_lit2,pop_typ2,XW_BIBTEX_STK_INT);
				else
					fn_info[pop_lit1] = pop_lit2;
				break;
				
			case XW_BIBTEX_STR_GLOBAL_VAR:
				if (pop_typ2 != XW_BIBTEX_STK_STR)
					printWrongStkLit(pop_lit2,pop_typ2,XW_BIBTEX_STK_STR);
				else
				{
					str_glb_ptr = fn_info[pop_lit1];
					if (pop_lit2 < cmd_str_ptr)
						glb_str_ptr[str_glb_ptr] = pop_lit2;
					else
					{
						glb_str_ptr[str_glb_ptr] = 0;
        		glob_chr_ptr = 0;
        		sp_ptr = str_start[pop_lit2];
        		sp_end = str_start[pop_lit2+1];
        		if (sp_end - sp_ptr > XW_BIBTEX_GLOB_STR_SIZE)
        		{
        			bst1printStringSizeExceeded();
    					QString str = QString(tr("%1, the global")).arg(XW_BIBTEX_GLOB_STR_SIZE);
    					print(str);
    					bst2printStringSizeExceeded();
    					sp_end = sp_ptr + XW_BIBTEX_GLOB_STR_SIZE;
        		}
        		while (sp_ptr < sp_end)
        		{
        			global_strs[str_glb_ptr][glob_chr_ptr] = str_pool[sp_ptr];
            	glob_chr_ptr++;
            	sp_ptr++;
        		}
        		glb_str_end[str_glb_ptr] = glob_chr_ptr;
					}
				}
				break;
				
			default:
				print(tr("You can't assign to type "));
        printFnClass(pop_lit1);
        print(tr(", a nonvariable function class"));
        bstExWarnPrint();
				break;
		}
	}
}

void XWBibTeX::xGreaterThan()
{
	popLitStk(&pop_lit1,&pop_typ1);
	popLitStk(&pop_lit2,&pop_typ2);
	if (pop_typ1 != XW_BIBTEX_STK_INT)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_INT);
    pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else if (pop_typ2 != XW_BIBTEX_STK_INT)
	{
		printWrongStkLit(pop_lit2,pop_typ2,XW_BIBTEX_STK_INT);
    pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else if (pop_lit2 > pop_lit1)
		pushLitStk(1, XW_BIBTEX_STK_INT);
	else
		pushLitStk(0, XW_BIBTEX_STK_INT);
}

void XWBibTeX::xIntToChr()
{
	popLitStk(&pop_lit1,&pop_typ1);
	if (pop_typ1 != XW_BIBTEX_STK_INT)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_INT);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else if ((pop_lit1 < 0) || (pop_lit1 > 127))
	{
		QString str = QString(tr("%1 isn't valid ASCII")).arg(pop_lit2);
		print(str);
    bstExWarnPrint() ;
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else
	{
		strRoom(1);
    appendChar(pop_lit1);
    pushLitStk(makeString(), XW_BIBTEX_STK_STR);
	}
}

void XWBibTeX::xIntToStr()
{
	popLitStk(&pop_lit1,&pop_typ1);
	if (pop_typ1 != XW_BIBTEX_STK_INT)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_INT);
    pushLitStk (s_null, XW_BIBTEX_STK_STR);
	}
	else
	{
		intToASCII(pop_lit1, ex_buf, 0, &ex_buf_length);
    addPoolBufAndPush();
	}
}

void XWBibTeX::xLessThan()
{
	popLitStk(&pop_lit1,&pop_typ1);
	popLitStk(&pop_lit2,&pop_typ2);
	if (pop_typ1 != XW_BIBTEX_STK_INT)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_INT);
    pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else if (pop_typ2 != XW_BIBTEX_STK_INT)
	{
		printWrongStkLit(pop_lit2,pop_typ2,XW_BIBTEX_STK_INT);
    pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else if (pop_lit2 < pop_lit1)
		pushLitStk(1, XW_BIBTEX_STK_INT);
	else
		pushLitStk(0, XW_BIBTEX_STK_INT);
}

void XWBibTeX::xMinus()
{
	popLitStk(&pop_lit1,&pop_typ1);
	popLitStk(&pop_lit2,&pop_typ2);
	if (pop_typ1 != XW_BIBTEX_STK_INT)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_INT);
    pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else if (pop_typ2 != XW_BIBTEX_STK_INT)
	{
		printWrongStkLit(pop_lit2,pop_typ2,XW_BIBTEX_STK_INT);
    pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else
		pushLitStk(pop_lit2-pop_lit1, XW_BIBTEX_STK_INT);
}

void XWBibTeX::xMissing()
{
	popLitStk(&pop_lit1,&pop_typ1);
	if (!mess_with_entries)
		bstCantMessWithEntriesPrint();
	else if ((pop_typ1 != XW_BIBTEX_STK_STR) && 
		     (pop_typ1 != XW_BIBTEX_STK_FIELD_MISSING))
	{
		if (pop_typ1 != XW_BIBTEX_STK_EMPTY)
		{
			pushLitStk(pop_lit1,pop_typ1);
			print(tr(", not a string or missing field,"));
			bstExWarnPrint();
		}
		pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else if (pop_typ1 == XW_BIBTEX_STK_FIELD_MISSING)
		pushLitStk(1, XW_BIBTEX_STK_INT);
	else
		pushLitStk(0, XW_BIBTEX_STK_INT);
}

void XWBibTeX::xNumNames()
{
	popLitStk(&pop_lit1,&pop_typ1);
	if (pop_typ1 != XW_BIBTEX_STK_STR)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_STR);
    pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else
	{
		ex_buf_length = 0;
    addBufPool(pop_lit1);
    ex_buf_ptr = 0;
		num_names = 0;
		while (ex_buf_ptr < ex_buf_length)
		{
			nameScanForAnd(pop_lit1);
			num_names++;
		}
		pushLitStk(num_names, XW_BIBTEX_STK_INT);
	}
}

void XWBibTeX::xPlus()
{
	popLitStk(&pop_lit1,&pop_typ1);
	popLitStk(&pop_lit2,&pop_typ2);
	if (pop_typ1 != XW_BIBTEX_STK_INT)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_INT);
    pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else if (pop_typ2 != XW_BIBTEX_STK_INT)
	{
		printWrongStkLit(pop_lit2,pop_typ2,XW_BIBTEX_STK_INT);
    pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else
		pushLitStk(pop_lit2+pop_lit1, XW_BIBTEX_STK_INT);
}

void XWBibTeX::xPreamble()
{
	ex_buf_length = 0;
	preamble_ptr = 0;
	while (preamble_ptr < num_preamble_strings)
	{
		addBufPool(s_preamble[preamble_ptr]);
		preamble_ptr++;
	}
	addPoolBufAndPush();
}

void XWBibTeX::xPurify()
{
	popLitStk(&pop_lit1,&pop_typ1);
	if (pop_typ1 != XW_BIBTEX_STK_STR)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_STR);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else
	{
		ex_buf_length = 0;
    addBufPool(pop_lit1);
    brace_level = 0;
    ex_buf_xptr = 0;
    ex_buf_ptr = 0;
    while (ex_buf_ptr < ex_buf_length)
    {
    	switch (lex_class[ex_buf[ex_buf_ptr]])
    	{
    		case XW_BIBTEX_WHITE_SPACE:
    		case XW_BIBTEX_SEP_CHAR:
    			ex_buf[ex_buf_xptr] = XW_BIBTEX_SPACE;
    			ex_buf_xptr++;
    			break;
    			
    		case XW_BIBTEX_ALPHA:
    		case XW_BIBTEX_NUMERIC:
    			ex_buf[ex_buf_xptr] = ex_buf[ex_buf_ptr];
    			ex_buf_xptr++;
    			break;
    			
    		default:
    			if (ex_buf[ex_buf_ptr] == XW_BIBTEX_LEFT_BRACE)
    			{
    				brace_level++;
    				if ((brace_level = 1) && (ex_buf_ptr + 1 < ex_buf_length))
    				{
    					if (ex_buf[ex_buf_ptr+1] == XW_BIBTEX_BACKSLASH)
    					{
    						ex_buf_ptr++;
    						while ((ex_buf_ptr < ex_buf_length) && (brace_level > 0))
    						{
    							ex_buf_ptr++;
    							ex_buf_yptr = ex_buf_ptr;
    							while ((ex_buf_ptr < ex_buf_length) && 
    							       (lex_class[ex_buf[ex_buf_ptr]] == XW_BIBTEX_ALPHA))
    							{
    								ex_buf_ptr++;
    							}
    							control_seq_loc = strLookup(ex_buf,ex_buf_yptr,ex_buf_ptr-ex_buf_yptr, XW_BIBTEX_CONTROL_SEQ_ILK,false);
    							if (hash_found)
    							{
    								ex_buf[ex_buf_xptr] = ex_buf[ex_buf_yptr];
    								ex_buf_xptr++;
    								switch (ilk_info[control_seq_loc])
    								{
    									case XW_BIBTEX_N_OE:
    									case XW_BIBTEX_N_OE_UPPER:
    									case XW_BIBTEX_N_AE:
    									case XW_BIBTEX_N_AE_UPPER:
    									case XW_BIBTEX_N_SS :
    										ex_buf[ex_buf_xptr] = ex_buf[ex_buf_yptr+1];
    										ex_buf_xptr++;
    										break;
    										
    									default:
    										break;
    								}
    							}
    							
    							while ((ex_buf_ptr < ex_buf_length) && (brace_level > 0) && 
    										(ex_buf[ex_buf_ptr] != XW_BIBTEX_BACKSLASH))
    							{
    								switch (lex_class[ex_buf[ex_buf_ptr]])
    								{
    									case XW_BIBTEX_ALPHA:
            					case XW_BIBTEX_NUMERIC :
            						ex_buf[ex_buf_xptr] = ex_buf[ex_buf_ptr];
            						break;
            						
            					default:
            						if (ex_buf[ex_buf_ptr] == XW_BIBTEX_RIGHT_BRACE)
            							brace_level--;
            						else if (ex_buf[ex_buf_ptr] == XW_BIBTEX_LEFT_BRACE)
            							brace_level++;
            						break;
    								}
    								ex_buf_ptr++;
    							}
    						}
    						ex_buf_ptr++;
    					}
    				}
    			}
    			else if (ex_buf[ex_buf_ptr] == XW_BIBTEX_RIGHT_BRACE)
    			{
    				if (brace_level > 0)
    					brace_level--;
    			}
    			break;
    	}
    	ex_buf_ptr++;
    }
    ex_buf_length = ex_buf_xptr;
    addPoolBufAndPush();
	}
}

void XWBibTeX::xQuote()
{
	strRoom(1);
	appendChar(XW_BIBTEX_DOUBLE_QUOTE);
	pushLitStk(makeString(), XW_BIBTEX_STK_STR);
}

void XWBibTeX::xSubstring()
{
	popLitStk(&pop_lit1,&pop_typ1);
	popLitStk(&pop_lit2,&pop_typ2);
	popLitStk(&pop_lit3,&pop_typ3);
	if (pop_typ1 != XW_BIBTEX_STK_INT)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_INT);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else if (pop_typ2 != XW_BIBTEX_STK_INT)
	{
		printWrongStkLit(pop_lit2,pop_typ2,XW_BIBTEX_STK_INT);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else if (pop_typ3 != XW_BIBTEX_STK_STR)
	{
		printWrongStkLit(pop_lit3,pop_typ3,XW_BIBTEX_STK_STR);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else
	{
		sp_length = length(pop_lit3);
		if (pop_lit1 >= sp_length)
		{
			if ((pop_lit2 == 1) || (pop_lit2 == -1))
			{
				if (lit_stack[lit_stk_ptr] >= cmd_str_ptr)
					unflushString();
				lit_stk_ptr++;
				
				return ;
			}
		}
		
		if ((pop_lit1 <= 0) || 
			  (pop_lit2 == 0) || 
			  (pop_lit2 > sp_length) ||
        (pop_lit2 < -sp_length))
		{
			pushLitStk(s_null, XW_BIBTEX_STK_STR);
      return;
		}
		else
		{
			if (pop_lit2 > 0)
			{
				if (pop_lit1 > sp_length - (pop_lit2-1))
					pop_lit1 = sp_length - (pop_lit2-1);
				sp_ptr = str_start[pop_lit3] + (pop_lit2-1);
    		sp_end = sp_ptr + pop_lit1;
    		if (pop_lit2 == 1)
    		{
    			if (pop_lit3 >= cmd_str_ptr)
    			{
    				str_start[pop_lit3+1] = sp_end;
        		unflushString();
        		lit_stk_ptr++;
        		return ;
    			}
    		}
			}
			else
			{
				pop_lit2 = -pop_lit2;
				if (pop_lit1 > sp_length - (pop_lit2-1))
					pop_lit1 = sp_length - (pop_lit2-1);
				sp_end = str_start[pop_lit3+1] - (pop_lit2-1);
    		sp_ptr = sp_end - pop_lit1;
			}
			
			while (sp_ptr < sp_end)
			{
				appendChar(str_pool[sp_ptr]);
				sp_ptr++;
			}
			pushLitStk(makeString(), XW_BIBTEX_STK_STR); 
		}
	}
}

void XWBibTeX::xSwap()
{
	popLitStk(&pop_lit1,&pop_typ1);
	popLitStk(&pop_lit2,&pop_typ2);
	if ((pop_typ1 != XW_BIBTEX_STK_STR) || (pop_lit1 < cmd_str_ptr))
	{
		pushLitStk(pop_lit1, pop_typ1);
		if ((pop_typ2 == XW_BIBTEX_STK_STR) && (pop_lit2 >= cmd_str_ptr))
			unflushString();
		pushLitStk(pop_lit2, pop_typ2);
	}
	else if ((pop_typ2 != XW_BIBTEX_STK_STR) || (pop_lit2 < cmd_str_ptr))
	{
		unflushString();
		pushLitStk(pop_lit1, XW_BIBTEX_STK_STR);
    pushLitStk(pop_lit2, pop_typ2);
	}
	else
	{
		ex_buf_length = 0;
		addBufPool(pop_lit2);
		sp_ptr = str_start[pop_lit1];
		sp_end = str_start[pop_lit1+1];
		while (sp_ptr < sp_end)
		{
			appendChar(str_pool[sp_ptr]);
			sp_ptr++;
		}
		pushLitStk(makeString(), XW_BIBTEX_STK_STR);
		addPoolBufAndPush();
	}
}

void XWBibTeX::xTextLength()
{
	popLitStk(&pop_lit1,&pop_typ1);
	if (pop_typ1 != XW_BIBTEX_STK_STR)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_STR);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else
	{
		num_text_chars = 0;
		sp_ptr = str_start[pop_lit1];
		sp_end = str_start[pop_lit1+1];
		sp_brace_level = 0;
		while (sp_ptr < sp_end)
		{
			sp_ptr++;
			if (str_pool[sp_ptr-1] == XW_BIBTEX_LEFT_BRACE)
			{
				sp_brace_level++;
				if ((sp_brace_level == 1) && (sp_ptr < sp_end))
				{
					if (str_pool[sp_ptr] == XW_BIBTEX_BACKSLASH)
					{
						sp_ptr++;
						while ((sp_ptr < sp_end) && (sp_brace_level > 0))
						{
							if (str_pool[sp_ptr] == XW_BIBTEX_RIGHT_BRACE)
								sp_brace_level--;
							else if (str_pool[sp_ptr] == XW_BIBTEX_LEFT_BRACE)
								sp_brace_level++;
							sp_ptr++;
						}
						num_text_chars++;
					}
				}
			}
			else if (str_pool[sp_ptr-1] == XW_BIBTEX_RIGHT_BRACE)
			{
				if (sp_brace_level > 0)
					sp_brace_level--;
			}
			else
			{
				num_text_chars++;
			}
		}
		pushLitStk(num_text_chars, XW_BIBTEX_STK_INT);
	}
}

void XWBibTeX::xTextPrefix()
{
	popLitStk(&pop_lit1,&pop_typ1);
	popLitStk(&pop_lit2,&pop_typ2);
	if (pop_typ1 != XW_BIBTEX_STK_INT)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_INT);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else if (pop_typ2 != XW_BIBTEX_STK_STR)
	{
		printWrongStkLit(pop_lit2,pop_typ2,XW_BIBTEX_STK_STR);
    pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else if (pop_lit1 <= 0)
	{
		pushLitStk(s_null, XW_BIBTEX_STK_STR);
    return;
	}
	else
	{
		sp_ptr = str_start[pop_lit2];
		sp_end = str_start[pop_lit2+1];
		num_text_chars = 0;
		sp_brace_level = 0;
		sp_xptr1 = sp_ptr;
		while ((sp_xptr1 < sp_end) && (num_text_chars < pop_lit1))
		{
			sp_xptr1++;
			if (str_pool[sp_xptr1-1] == XW_BIBTEX_LEFT_BRACE)
			{
				sp_brace_level++;
				if ((sp_brace_level == 1) && (sp_xptr1 < sp_end))
				{
					if (str_pool[sp_xptr1] == XW_BIBTEX_BACKSLASH)
					{
						sp_xptr1++;
						while ((sp_xptr1 < sp_end) && (sp_brace_level > 0))
						{
							if (str_pool[sp_xptr1] == XW_BIBTEX_RIGHT_BRACE)
								sp_brace_level--;
							else if (str_pool[sp_xptr1] == XW_BIBTEX_LEFT_BRACE)
								sp_brace_level++;
							sp_xptr1++;
						}
						num_text_chars++;
					}
				}
			}
			else if (str_pool[sp_xptr1-1] == XW_BIBTEX_RIGHT_BRACE)
			{
				if (sp_brace_level > 0)
					sp_brace_level--;
			}
			else
				num_text_chars++;
		}
		sp_end = sp_xptr1;
		if (pop_lit2 >= cmd_str_ptr)
			pool_ptr = sp_end;
		else
		{
			while (sp_ptr < sp_end)
			{
				appendChar(str_pool[sp_ptr]);
				sp_ptr++;
			}
		}
		
		while (sp_brace_level > 0)
		{
			appendChar(XW_BIBTEX_RIGHT_BRACE);
			sp_brace_level--;
		}
		pushLitStk(makeString(), XW_BIBTEX_STK_STR);
	}
}

void XWBibTeX::xType()
{
	if (!mess_with_entries)
		bstCantMessWithEntriesPrint();
	else if ((type_list[cite_ptr] == XW_BIBTEX_UNDEFINED) || 
						(type_list[cite_ptr] == XW_BIBTEX_EMPTY))
	{
		pushLitStk(s_null, XW_BIBTEX_STK_STR);
	}
	else
		pushLitStk(hash_text[type_list[cite_ptr]], XW_BIBTEX_STK_STR);
}

void XWBibTeX::xWarning()
{
	popLitStk(&pop_lit1,&pop_typ1);
	if (pop_typ1 != XW_BIBTEX_STK_STR)
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_STR);
	else
	{
		print(tr("Warning--"));
    printLit(pop_lit1,pop_typ1);
    markWarning();
	}
}

void XWBibTeX::xWidth()
{
	popLitStk(&pop_lit1,&pop_typ1);
	if (pop_typ1 != XW_BIBTEX_STK_STR)
	{
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_STR);
    pushLitStk(0, XW_BIBTEX_STK_INT);
	}
	else
	{
		ex_buf_length = 0;
    addBufPool(pop_lit1);
    string_width = 0;
    brace_level = 0;
    ex_buf_ptr = 0;
    while (ex_buf_ptr < ex_buf_length)
    {
    	if (ex_buf[ex_buf_ptr] == XW_BIBTEX_LEFT_BRACE)
    	{
    		brace_level++;
    		if ((brace_level == 1) && (ex_buf_ptr + 1 < ex_buf_length))
    		{
    			if (ex_buf[ex_buf_ptr+1] == XW_BIBTEX_BACKSLASH)
    			{
    				ex_buf_ptr++;
    				while ((ex_buf_ptr < ex_buf_length) && (brace_level > 0))
    				{
    					ex_buf_ptr++;
    					ex_buf_xptr = ex_buf_ptr;
    					while ((ex_buf_ptr < ex_buf_length) && 
                (lex_class[ex_buf[ex_buf_ptr]] == XW_BIBTEX_ALPHA))
              {
              	ex_buf_ptr++;
              }
              if ((ex_buf_ptr < ex_buf_length) && (ex_buf_ptr == ex_buf_xptr))
              	ex_buf_ptr++;
              else
              {
              	control_seq_loc = strLookup(ex_buf,ex_buf_xptr, ex_buf_ptr-ex_buf_xptr,XW_BIBTEX_CONTROL_SEQ_ILK,false);
              	if (hash_found)
              	{
              		switch (ilk_info[control_seq_loc])
              		{
              			case XW_BIBTEX_N_SS : 
              				string_width = string_width + XW_BIBTEX_SS_WIDTH;
              				break;
		              				
    								case XW_BIBTEX_N_AE : 
    									string_width = string_width + XW_BIBTEX_AE_WIDTH;
    									break;
    									
    								case XW_BIBTEX_N_OE : 
    									string_width = string_width + XW_BIBTEX_OE_WIDTH;
    									break;
    									
    								case XW_BIBTEX_N_AE_UPPER : 
    									string_width = string_width + XW_BIBTEX_UPPER_AE_WIDTH;
    									break;
    									
    								case XW_BIBTEX_N_OE_UPPER : 
    									string_width = string_width + XW_BIBTEX_UPPER_OE_WIDTH;
    									break;
    									
    								default:
    									string_width = string_width + char_width[ex_buf[ex_buf_xptr]];
    									break;
              		}
              	}
              }
              
              while ((ex_buf_ptr < ex_buf_length) && 
                (lex_class[ex_buf[ex_buf_ptr]] == XW_BIBTEX_WHITE_SPACE))
              {
              	ex_buf_ptr--;
              }
              while ((ex_buf_ptr < ex_buf_length) && 
                     (brace_level > 0) && 
                     (ex_buf[ex_buf_ptr] != XW_BIBTEX_BACKSLASH))
							{
								if (ex_buf[ex_buf_ptr] == XW_BIBTEX_RIGHT_BRACE)
									brace_level--;
								else if (ex_buf[ex_buf_ptr] == XW_BIBTEX_LEFT_BRACE)
									brace_level++;
								else
									string_width = string_width + char_width[ex_buf[ex_buf_ptr]];
								ex_buf_ptr++;
							}
    				}
    				ex_buf_ptr++;
    			}
    			else
    			{
    				string_width = string_width + char_width[XW_BIBTEX_LEFT_BRACE];
    			}
    		}
    		else
    			string_width = string_width + char_width[XW_BIBTEX_LEFT_BRACE];
    	}
    	else if (ex_buf[ex_buf_ptr] == XW_BIBTEX_RIGHT_BRACE)
    	{
    		decrBraceLevel(pop_lit1);
        string_width = string_width + char_width[XW_BIBTEX_RIGHT_BRACE];
    	}
    	else
    		string_width = string_width + char_width[ex_buf[ex_buf_ptr]];
    	ex_buf_ptr++;
    }
    checkBraceLevel(pop_lit1);
    pushLitStk(string_width, XW_BIBTEX_STK_INT);
	}
}

void XWBibTeX::xWrite()
{
	popLitStk(&pop_lit1,&pop_typ1);
	if (pop_typ1 != XW_BIBTEX_STK_STR)
		printWrongStkLit(pop_lit1,pop_typ1,XW_BIBTEX_STK_STR);
	else
		addOutPool(pop_lit1);
}
