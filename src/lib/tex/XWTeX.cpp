/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <QDataStream>
#include <QDateTime>
#include <QTextStream>
#include <QByteArray>
#include <QTextCodec>
#include "XWApplication.h"
#include "XWFileName.h"
#include "XWTexSea.h"
#include "XWTeXIODev.h"
#include "XWTeX.h"

static char default_fmt_ident[] = "TeXformats:plain.afmt";

XWTeX::XWTeX(XWTeXIODev * devA, QObject * parent)
	:QObject(parent),
	 dev(devA),
	 fmt_file(0),
	 ini_version(false)
{
	mem_max = 30000;
	mem_min = 0;
	buf_size = 500;
	error_line = 72;
	half_error_line = 42;
	max_print_line = 79;
	stack_size = 200;
	max_in_open = 6;
	param_size = 60;
	nest_size  = 40;
	max_strings = 3000;
	string_vacancies = 8000;
	pool_size = 32000;
	save_size  = 600;
	trie_size  = 8000;
	trie_op_size = 15011;
	neg_trie_op_size = -15011;
	mem_bot = 0;
	mem_top = 30000;
	hyph_size = 307;
	rover = 0;

	parse_first_line_p = false;
	file_line_error_style_p = false;
	halt_on_error_p = false;
	src_specials_p = false;
	insert_src_special_auto = false;
	insert_src_special_every_par = false;
	insert_src_special_every_parend = false;
	insert_src_special_every_cr = false;
	insert_src_special_every_math = false;
	insert_src_special_every_hbox = false;
	insert_src_special_every_vbox = false;
	insert_src_special_every_display = false;

	interaction_option = TEX_UNSPECIFIED_MODE;

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

	for (int i = 0; i < 32; i++)
		xchr[i] = (uchar)i;

	for (int i = 127; i < 256; i++)
		xchr[i] = (char)i;

	selector = TEX_TERM_ONLY;

	name_of_file = 0;
	name_length = 0;

	stop_at_space = true;

	trick_buf = (qint32 *)malloc(TEX_SSUP_ERROR_LINE * sizeof(qint32));

	buffer = 0;
	first = 0;
	buffered_cs = 0;
	buffered_cs_len = 0;

	str_pool = 0;
	str_start_ar = 0;
	str_ptr = TEX_TOO_BIG_CHAR;
	init_str_ptr = 0;
	init_pool_ptr = 0;

	help_line.resize(6);

	input_stack = 0;

	yzmem = 0;
	zmem = 0;
	mem = 0;

#ifdef XW_TEX_DEBUG
	mfree = 0;
	was_free = 0;
#endif //XW_TEX_DEBUG

	nest = 0;

	hashtable = (TeXHashWord*)malloc(TEX_HASHTABLESIZE * sizeof(TeXHashWord));
	initEqtbTable();

	save_stack = 0;

	input_file = 0;
	input_file_mode = 0;
	input_file_translation = 0;
	line_stack = 0;
	source_filename_stack = 0;
	full_source_filename_stack = 0;

	param_stack = 0;

	job_name = 0;
	output_file_name = 0;
	log_opened = false;

	qint32 len = strlen(default_fmt_ident);
	qint32 k = 0;
	for (; k < len; k++)
		TEX_format_default[k + 1] = default_fmt_ident[k];
	TEX_format_default[k] = 0;

	fonttables = 0;
	font_entries = 0;

	active_info = (TeXMemoryWord*)malloc(TEX_ACTIVE_MEM_SIZE * sizeof(TeXMemoryWord));
	holding     = (qint32*)malloc(TEX_ACTIVE_MEM_SIZE * sizeof(qint32));

	ocp_file = 0;

	ocptables = 0;
	ocp_entries = 0;
	ocp_list_info = 0;
	ocp_lstack_info = 0;
	ocp_list_list = 0;

	trie = 0;
	op_start = (qint32*)malloc(TEX_BIGGEST_LANG * sizeof(qint32));

	hyph_word = 0;
	hyph_list = 0;

	zzzaa = (qint32*)malloc(2 * (trie_op_size + 1) * sizeof(qint32));
	trie_used = (qint32*)malloc((TEX_BIGGEST_LANG + 2) * sizeof(qint32));
	trie_op_lang = (qint32*)malloc((trie_op_size + 2) * sizeof(qint32));
	trie_op_val = (qint32*)malloc((trie_op_size + 2) * sizeof(qint32));
	hyf_distance = (qint32*)malloc((trie_op_size + 2) * sizeof(qint32));
	hyf_num = (qint32*)malloc((trie_op_size + 2) * sizeof(qint32));
	hyf_next = (qint32*)malloc((trie_op_size + 2) * sizeof(qint32));

	trie_c = 0;
	trie_o = 0;
	trie_l = 0;
	trie_r = 0;
	trie_hash = 0;

	trie_taken = 0;
	trie_min = 0;

	format_ident = 0;

	outside_string_array = (qint32*)malloc(1002 * sizeof(qint32));

	eof_seen = 0;

	grp_stack = 0;
	if_stack = 0;

	output_comment = 0;

	debug_format_file = false;

	otp_init_input_buf = 0;
	otp_input_buf = 0;
	otp_output_buf = 0;
	otp_stack_buf = 0;
	otp_calcs = 0;
	otp_states = 0;
	active_min_ptr = 0;
	active_max_ptr = 0;

	for (int i = 0; i < 17; i++)
	{
		read_file[i] = 0;
		read_file_mode[i] = 0;
		read_file_translation[i] = 0;
		write_file[i] = 0;
		write_file_mode[i] = 0;
		write_file_translation[i] = 0;
	}

	for (int i = 0; i < 18; i++)
		read_open[i] = TEX_CLOSED;

	for (int i = 0; i < 19; i++)
		write_open[i] = false;

	reloadFmt = false;
}

XWTeX::~XWTeX()
{
	if (fmt_file)
	{
		fmt_file->close();
		delete fmt_file;
		fmt_file = 0;
	}

	if (name_of_file)
		free(name_of_file);

	free(trick_buf);

	if (active_info)
		free(active_info);

	if (holding)
		free(holding);

	if (ocp_file)
	{
		ocp_file->close();
		delete ocp_file;
	}

	free(op_start);

	free(zzzaa);
	free(trie_used);
	free(trie_op_lang);
	free(trie_op_val);
	free(outside_string_array);

	free(hyf_distance);
	free(hyf_num);
	free(hyf_next);

	if (output_comment)
		free(output_comment);

	clear();

	free(hashtable);
}

void XWTeX::compile(const QString & maininputfile, bool etex)
{
	initialize();
	initOutput();
	if (src_specials_p || file_line_error_style_p || parse_first_line_p)
		print(tr("This is XWAlephk, Version 3.141592--1.15--2.1 (RC2)\n"));
	else
		print(tr("This is XWAleph, Version 3.141592--1.15--2.1 (RC2)\n"));

	if (shell_enabled_p)
		print(tr("\\write18 enabled.\n"));

	if (src_specials_p)
		print(tr("Source specials enabled.\n"));

	if (file_line_error_style_p)
		print(tr("file:line:error style messages enabled.\n"));

	if (parse_first_line_p)
		print(tr("%&-line parsing enabled.\n"));

	if (ini_version)
	{
		if (checkConstant() != 0)
			return ;

		if (!initPrim())
			return ;

		if (etex)
			enableETeX();

		if (fmt_file)
		{
			loadFmt();
			fmt_file->close();
			delete fmt_file;
			fmt_file = 0;
		}
	}
	else
	{
		if (!loadFmt())
			return ;
	}

	setBuffer(maininputfile);
	initInput();

	output_file_name = getOutputFile();

	if (eTeX_ex)
		print(tr("entering extended mode** \n"));

	if (endLineCharInactive())
		limit--;
	else
		buffer[limit] = endLineChar();

	fixDateAndTime();

	magic_offset = str_start(TeXMath_Spacing) - 9 * TEX_ORD_NOAD;
	if (interaction == TEX_BATCH_MODE)
		selector = TEX_NO_PRINT;
	else
		selector = TEX_TERM_ONLY;

	if ((loc < limit) && (catCode(buffer[loc]) != TEX_ESCAPE))
		startInput();

	text_dir_ptr = newDir(0);

	history = TEX_SPOTLESS;
	mainControl();
	finalCleanup();
	closeFilesAndTerminate();
}

void XWTeX::initInput()
{
	input_ptr = 0;
	max_in_stack = 0;
	source_filename_stack[0] = 0;
	full_source_filename_stack[0] = 0;
	in_open = 0;
	open_parens = 0;
	max_buf_stack = 0;
	grp_stack[0] = 0;
	if_stack[0] = TEX_NULL;
	param_ptr = 0;
	max_param_stack = 0;
	geqDefine(TEX_OCP_INPUT_MODE_BASE, (quint16)TEX_DATA, 1);
	term_in_mode = equiv(TEX_OCP_INPUT_MODE_BASE);
	term_out_mode = equiv(TEX_OCP_INPUT_MODE_BASE);
	if (term_in_mode > 0)
		term_in_translation = equiv(TEX_OCP_INPUT_ONEBYTE_TRANSLATION_BASE + term_in_mode - 1);

	if (term_out_mode > 0)
		term_out_translation = equiv(TEX_OCP_OUTPUT_ONEBYTE_TRANSLATION_BASE + term_in_mode - 1);

	scanner_status = TEX_NORMAL;
	warning_index = TEX_NULL;
	first = 1;
	state = TEX_NEW_LINE;
	texstart = 1;
	index = 0;
	line = 0;
	texname = 0;
	current_ocp_lstack = 0;
	current_ocp_no = 0;
	force_eof = false;
	align_state = 1000000;
}

void XWTeX::initOutput()
{
	selector = TEX_TERM_ONLY;
	tally = 0;
	term_offset = 0;
	file_offset = 0;
	job_name = 0;
	name_in_progress = false;
	log_opened = false;
}

bool XWTeX::isOk()
{
	return (history != TEX_FATAL_ERROR_STOP);
}

void XWTeX::parseInterActionOption(const char * optarg)
{
	if (!optarg)
		return ;

	if (strcmp(optarg, "batchmode") == 0)
		interaction_option = 0;
	else if (strcmp(optarg, "nonstopmode") == 0)
		interaction_option = 1;
	else if (strcmp(optarg, "scrollmode") == 0)
		interaction_option = 2;
	else if (strcmp(optarg, "errorstopmode") == 0)
		interaction_option = 3;
}

void XWTeX::parseSrcSpecialsOption(const char * opt_list)
{
	if (!opt_list)
	{
		insert_src_special_every_par = true;
		insert_src_special_auto = true;
        src_specials_p = true;
	}
	else
	{
		char * toklist = qstrdup(opt_list);
		char * tok = strtok(toklist, ", ");
		while (tok)
		{
			if (strcmp(tok, "everypar") == 0 ||
				strcmp(tok, "par") == 0 ||
				strcmp(tok, "auto") == 0)
			{
      			insert_src_special_auto = true;
      			insert_src_special_every_par = true;
    		}
    		else if (strcmp(tok, "everyparend") == 0 ||
    			     strcmp(tok, "parend") == 0)
    		{
      			insert_src_special_every_parend = true;
      		}
      		else if (strcmp(tok, "everycr") == 0 ||
      			     strcmp(tok, "cr") == 0)
      		{
      			insert_src_special_every_cr = true;
      		}
      		else if (strcmp(tok, "everymath") == 0 ||
      			     strcmp(tok, "math") == 0)
      		{
      			insert_src_special_every_math = true;
      		}
    		else if (strcmp(tok, "everyhbox") == 0 ||
    			     strcmp(tok, "hbox") == 0)
    		{
      			insert_src_special_every_hbox = true;
      		}
    		else if (strcmp(tok, "everyvbox") == 0 ||
    			     strcmp(tok, "vbox") == 0)
    		{
      			insert_src_special_every_vbox = true;
      		}
    		else if (strcmp(tok, "everydisplay") == 0 ||
    			     strcmp(tok, "display") == 0)
    		{
      			insert_src_special_every_display = true;
      		}
      		else if (strcmp (tok, "none") == 0)
      		{
      			insert_src_special_auto = false;
				insert_src_special_every_par = false;
				insert_src_special_every_parend = false;
				insert_src_special_every_cr = false;
				insert_src_special_every_math = false;
				insert_src_special_every_hbox = false;
				insert_src_special_every_vbox = false;
				insert_src_special_every_display = false;
      		}

      		tok = strtok(0, ", ");
		}

		delete toklist;

		src_specials_p = (insert_src_special_auto || insert_src_special_every_par ||
    					 insert_src_special_every_parend || insert_src_special_every_cr ||
    					 insert_src_special_every_math ||  insert_src_special_every_hbox ||
    					 insert_src_special_every_vbox || insert_src_special_every_display);
	}
}

bool XWTeX::setBuffer(const QString & s)
{
	if (s.length() <= 0)
		return false;

  	first = buf_size;
  	do
  	{
  		buffer[first] = 0;
  		first--;
  	} while (first > 0);

  	buffer[first] = 0;
  	int k = first;
    for (int i = 0; i < s.length(); i++)
     	buffer[k++] = s[i].unicode();

    buffer[k++] = ' ';
    buffer[k] = 0;

  	last = first;
  	while (buffer[last])
  		++last;

  	--last;
  	while (last >= first &&
  	       ISBLANK (buffer[last]) &&
  	       buffer[last] != '\r')
    {
    	--last;
    }

    last++;

    loc = first;
    while ((loc < last) && (buffer[loc] == ' '))
        loc++;

    limit = last;
    first = last + 1;

    return true;
}

void XWTeX::setOutputComment(const QString & s)
{
	if (output_comment)
	{
		free(output_comment);
		output_comment = 0;
	}

	if (s.length() <= 0)
		return ;

	QByteArray ba = s.toUtf8();
	int len = ba.size();
	if (len > 255)
		len = 255;

	output_comment = (char*)malloc(len + 1);
	strncpy(output_comment, ba.constData(), len);
}

void XWTeX::closeFilesAndTerminate()
{
	qint32 k;
	for (k = 0; k < 16; k++)
	{
		if (write_open[k])
		{
			if (write_file[k]->isOpen())
				write_file[k]->close();
			delete write_file[k];
			write_file[k] = 0;
			write_open[k] = false;
		}
	}

#ifdef XW_TEX_STAT

	if (tracingStats() > 0)
	{
		if (log_opened)
		{
			QTextStream streamA(dev->log_file);
			streamA << "\n";
			streamA << tr("Here is how much of Omega's memory, you used:\n");
			QString msg = QString(tr(" %1 strings out of %2\n")).
			                arg(str_ptr - init_str_ptr).
			                arg(max_strings - init_str_ptr);
			streamA << msg;

			msg = QString(tr(" %1 string characters out of %2\n")).
			               arg(pool_ptr - init_pool_ptr).
			               arg(pool_size-init_pool_ptr);

			streamA << msg;

			msg = QString(tr(" %1  words of memory out of %2\n"))
			               .arg(lo_mem_max - mem_min+mem_end - hi_mem_min + 2)
			               .arg(mem_end + 1 - mem_min);
			streamA << msg;

			msg = QString(tr(" %1 multiletter control sequences out of %2\n"))
			              .arg(cs_count).arg(TEX_HASH_SIZE);
			streamA << msg;

			msg = QString(tr(" %1 font(s)\n")).arg(font_ptr - TEX_FONT_BASE);
			streamA << msg;

			msg = QString(tr(" %1 hyphenation exceptions out of %2\n"))
			              .arg(hyph_count).arg(hyph_size);
			streamA << msg;

			msg = QString(tr(" %1i, %2n, %3p, %4b, "))
			             .arg(max_in_stack)
			             .arg(max_nest_stack)
			             .arg(max_param_stack)
			             .arg(max_buf_stack);
			msg += QString(tr("%1s stack positions out of %2, %3n, %4p, %5b, %6s\n"))
			               .arg(max_save_stack + 6)
			               .arg(stack_size)
			               .arg(nest_size)
			               .arg(param_size)
			               .arg(buf_size)
			               .arg(save_size);
			streamA << msg;
		}
	}

#endif

	while (cur_s > -1)
	{
		if (cur_s > 0)
			dev->dviPop();
		else
		{
			dev->dviEop();
			total_pages++;
		}
		cur_s--;
	}

	if (total_pages == 0)
		printnl(tr("No pages of output."));
	else
	{
		dev->dviPost();
		dev->dviFour(last_bop);
		last_bop = dev->dvi_offset + dev->dvi_ptr - 5;
		dev->dviFour(25400000);
		dev->dviFour(473628672);
		prepareMag();
		dev->dviFour(mag());
		dev->dviFour(max_v);
		dev->dviFour(max_h);
		dev->dviOut(max_push / 256);
		dev->dviOut(max_push % 256);
		dev->dviOut((total_pages / 256) % 256);
		dev->dviOut(total_pages % 256);
		while (font_ptr > TEX_FONT_BASE)
		{
			if (font_used(font_ptr))
				dviFontDef(font_ptr);

			font_ptr--;
		}

		dev->dviOut(DVI_POSTPOST);
		dev->dviFour(last_bop);
		dev->dviOut(DVI_ID_BYTE);
		dev->dviEnd();
		printnl(tr("\nOutput written on "));
		slowPrint(output_file_name);
		print(" (");
		printInt(total_pages);
		print(" page");
		if (total_pages != 1)
			printChar('s');

		print(", ");
		printInt(dev->dvi_offset + dev->dvi_ptr);
		print(tr(" bytes)."));

		if (log_opened)
			selector = selector - 2;
	}
}

void XWTeX::finalCleanup()
{
	qint32 c = cur_chr;
	if (job_name == 0)
		openLogFile();

	while (input_ptr > 0)
	{
		if (state == TEX_TOKEN_LIST)
			endTokenList();
		else
			endFileReading();
	}

	while (open_parens > 0)
	{
		print(" )");
		open_parens--;
	}

	if (cur_level > TEX_LEVEL_ONE)
	{
		printnl(tr("(\\end occurred inside a group at level "));
  		printInt(cur_level - TEX_LEVEL_ONE);
  		printChar(')');
  		if (eTeX_ex)
  			showSaveGroups();
	}

	while (cond_ptr != TEX_NULL)
	{
		printnl("(\\end occurred when ");
		printCmdChr((quint16)TEX_IF_TEST, cur_if);
		if (if_line != 0)
		{
			print(tr(" on line "));
			printInt(if_line);
		}
		print(tr(" was incomplete)"));
  		if_line = if_line_field(cond_ptr);
  		cur_if = subtype(cond_ptr);
  		temp_ptr = cond_ptr;
  		cond_ptr = link(cond_ptr);
  		freeNode(temp_ptr, TEX_IF_NODE_SIZE);
	}

	if (history != TEX_SPOTLESS)
	{
		if ((history == TEX_WARNING_ISSUED) ||
			(interaction < TEX_ERROR_STOP_MODE))
		{
			if (selector == TEX_TERM_AND_LOG)
			{
				selector = TEX_TERM_ONLY;
  				printnl(tr("(see the transcript file for additional information)"));
  				selector = TEX_TERM_AND_LOG;
			}
		}
	}

	if (c == 1)
	{
		if (ini_version)
		{
			for (c = TEX_TOP_MARK_CODE; c <= TEX_SPLIT_BOT_MARK_CODE; c++)
			{
				if (cur_mark[c] != TEX_NULL)
					deleteTokenRef(cur_mark[c]);
			}

			if (sa_mark != TEX_NULL)
			{
				if (doMarks(TEX_DESTROY_MARKS, 0, sa_mark))
					sa_mark = TEX_NULL;
			}

			for (c = TEX_LAST_BOX_CODE; c <= TEX_VSPLIT_CODE; c++)
				flushNodeList(disc_ptr[c]);

			storeFmtFile();
			return;
		}

		printnl(tr("(\\dump is performed only by INIALEPH)"));
		return;
	}
}

uchar XWTeX::getByte(QIODevice * f)
{
	uchar c = 0;
	f->getChar((char*)(&c));
	return c;
}

QString XWTeX::getFileName()
{
	QString ret;
	for (qint32 k = 1; k <= name_length; k++)
	{
		QChar c(name_of_file[k]);
		ret.append(c);
	}

	return ret;
}

void XWTeX::normalizeSelector()
{
	if (log_opened)
		selector = TEX_TERM_AND_LOG;
	else
		selector = TEX_TERM_ONLY;

	if (job_name == 0)
		openLogFile();

	if (interaction == TEX_BATCH_MODE)
		selector--;
}

long XWTeX::readSixteen(QIODevice * f)
{
	long c = getByte(f);
	if (c <= 127)
		c = c * 256 + getByte(f);

	return c;
}

long XWTeX::readSixteenUnsigned(QIODevice * f)
{
	long c = getByte(f);
	c = c * 256 + getByte(f);
	return c;
}

long XWTeX::readThirtyTwo(QIODevice * f)
{
	long c = getByte(f);
	if (c <= 127)
	{
		c = c * 256 + getByte(f);
		c = c * 256 + getByte(f);
		c = c * 256 + getByte(f);
	}

	return c;
}
