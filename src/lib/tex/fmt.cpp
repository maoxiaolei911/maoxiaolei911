/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <QCoreApplication>
#include <QDataStream>
#include <QDateTime>
#include <QTextStream>
#include <QByteArray>
#include "XWApplication.h"
#include "XWFileName.h"
#include "XWTexSea.h"
#include "XWTeXIODev.h"
#include "XWTeX.h"

static const char * enginename = "XWAleph";


bool XWTeX::loadFmt(const QString & fmtname)
{
	reloadFmt = false;	
	setInitVersion(false);
	setupBound(fmtname);
	if (checkConstant() != 0)
		return false;
	
	XWXWTexSea sea;
	fmt_file = sea.openFile(fmtname, XWXWTexSea::FMT);
	if (!fmt_file)
	{
		QString msg = QString(tr("Fail to open fmt file %1.\n")).arg(fmtname);
		dev->termOut(msg);
		return false;
	}
	
	return true;
}

bool XWTeX::loadFmt()
{
	if (!fmt_file)
		return false;
	
	fmt_file->seek(0);
	
	QString formatengine;
	
	font_entries = 0;
	ocp_entries = 0;
	
	qint32 j, k, p, q, x;
	qint32 d = 8;
	if (sizeof(double) > 8)
		d = sizeof(double);
		
	QDataStream streamA(fmt_file);
	streamA >> x;
	if (x != TEX_MAX_HALFWORD)
		goto bad_fmt;
		
	streamA >> x;
	if (x != d)
		goto bad_fmt;
		
	streamA >> x;
	if (x < 0 || x > 255)
		goto bad_fmt;
		
	streamA >> formatengine;
	
	if (x != formatengine.length() || formatengine != enginename)
		goto bad_fmt;
		
	streamA >> x;
	eTeX_mode = (char)x;
	if (eTeX_ex)
	{
		max_reg_num = 65535;
		max_reg_help_line = tr("A register number must be between 0 and 65535.");
	}
	else
	{
		max_reg_num = 255;
		max_reg_help_line = tr("A register number must be between 0 and 255.");
	}
	
	streamA >> x;
	if (x != mem_bot)
		goto bad_fmt;
		
	streamA >> x;
	if (x != mem_top)
		goto bad_fmt;
		
	if ((mem_bot + 1100) > mem_top)
		goto bad_fmt;
		
	streamA >> x;
	if (x != TEX_EQTB_SIZE)
		goto bad_fmt;
		
	streamA >> x;
	if (x != TEX_HASH_PRIME)
		goto bad_fmt;
		
	streamA >> x;
	if (x != TEX_HYPH_PRIME)
		goto bad_fmt;
		
	head = contrib_head; 
	tail = contrib_head;
    page_tail = page_head;
    
    mem_min = mem_bot - extra_mem_bot;
	mem_max = mem_top + extra_mem_top;
	
	if (!reloadFmt)
	{
		if (yzmem)
		{
			free(yzmem);
			yzmem = 0;
		}
	}
	
	if (!yzmem)	
		yzmem = (TeXMemoryWord*)malloc((mem_max - mem_min + 1) * sizeof(TeXMemoryWord));
	zmem  = yzmem - mem_min;
	mem   = zmem;
	
#ifdef XW_TEX_DEBUG
	if (!mfree)
		mfree = (bool*)malloc((mem_max - mem_min + 2) * sizeof(bool));
	
	if (!mfree)
		was_free = (bool*)malloc((mem_max - mem_min + 2) * sizeof(bool));
#endif //XW_TEX_DEBUG
		
	if (!undumpSize(streamA, 0, TEX_SUP_POOL_SIZE - pool_free, "string pool size", pool_ptr))
		goto bad_fmt;
		
	init_pool_ptr = pool_ptr;
		
	if (pool_size < (pool_ptr + pool_free))
		pool_size = pool_ptr + pool_free;
		
	if (!undumpSize(streamA, 0, TEX_SUP_MAX_STRINGS - strings_free, "sup strings", str_ptr))
		goto bad_fmt;
		
	init_str_ptr = str_ptr;
		
	if (max_strings < (str_ptr + strings_free))
		max_strings = str_ptr + strings_free;
		
	if (!str_start_ar)
		str_start_ar = (qint32*)malloc((max_strings - TEX_BIGGEST_CHAR + 1) * sizeof(qint32));
	streamA.readRawData((char*)str_start_ar, 4 * (str_ptr - TEX_TOO_BIG_CHAR + 1));
	
	if (!str_pool)
		str_pool = (qint32*)malloc((pool_size + 1) * sizeof(qint32));
		
	streamA.readRawData((char*)str_pool, 4 * pool_ptr);
	println(); 
	printInt(str_ptr); 
	print(tr(" strings of total length "));
	printInt(pool_ptr);
	println(); 
	
	streamA >> x;
	if (x < (lo_mem_stat_max + 1000) || x > (hi_mem_stat_min - 1))
		goto bad_fmt;		
	lo_mem_max = x;
	
	streamA >> x;
	if (x < (lo_mem_stat_max + 1) || x > lo_mem_max)
		goto bad_fmt;
	rover = x;
	p = mem_bot; 
	q = rover;
	do
	{
		streamA.readRawData((char*)(&mem[p]), d * (q + 2 - p));
		p = q + node_size(q);
		if ((p > lo_mem_max) || ((q >= rlink(q)) && (rlink(q) != rover)))
			goto bad_fmt;
		q = rlink(q);
	} while (q != rover);
	
	streamA.readRawData((char*)(&mem[p]), d * (lo_mem_max + 1 - p));
	if (mem_min < (mem_bot - 2))
	{
		p = llink(rover); 
		q = mem_min + 1;
  		link(mem_min) = TEX_NULL; 
  		info(mem_min) = TEX_NULL;
  		rlink(p) = q; 
  		llink(rover) = q;
  		rlink(q) = rover; 
  		llink(q) = p; 
  		link(q) = TEX_EMPTY_FLAG;
  		node_size(q) = mem_bot - q;
	}
	
	streamA >> x;
	if (x < (lo_mem_max + 1) || x > hi_mem_stat_min)
		goto bad_fmt;
	hi_mem_min = x;
	streamA >> x;
	if (x < TEX_NULL || x > mem_top)
		goto bad_fmt;
	avail = x;
	mem_end = mem_top;
	streamA.readRawData((char*)(&mem[hi_mem_min]), d * (mem_end + 1 - hi_mem_min));
	streamA >> var_used;
	streamA >> dyn_used;
	
	qint32 i = 0;
	if (reloadFmt)
	{
		for (i = 0; i < TEX_HASHTABLESIZE; i++)
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
	}
	
	streamA >> i;
	j = 0;
	while (j < TEX_HASHTABLESIZE) 
	{
      	i = (-i);
      	while (j < i) 
      	{
         	hashtable[j].p = -1;
         	j++;
      	}
      	
      	if (i < TEX_HASHTABLESIZE) 
      	{
         	TeXHashWord * runner = &(hashtable[j]);
         	streamA >> i;
         	while (i >=0 ) 
         	{
            	runner->p = i;
            	streamA.readRawData((char*)(&(runner->mw)), d);
            	streamA >> i;
            	runner->ptr = (TeXHashWord *) malloc(sizeof(TeXHashWord));
            	runner = runner->ptr;
         	}
         	runner->p = -1;
         	j++;
      	}
   	}
   	
   	streamA >> x;
   	if (x < TEX_HASH_BASE || x > TEX_FROZEN_CONTROL_SEQUENCE)
		goto bad_fmt;
	par_loc = x;
	par_token = TEX_CS_TOKEN_FLAG + par_loc;
	
	streamA >> x;
   	if (x < TEX_HASH_BASE || x > TEX_FROZEN_CONTROL_SEQUENCE)
		goto bad_fmt;
	write_loc = x;
	
	streamA >> x;
   	if (x < TEX_HASH_BASE || x > TEX_FROZEN_CONTROL_SEQUENCE)
		goto bad_fmt;
	special_loc = x;
	special_token = TEX_CS_TOKEN_FLAG + special_loc;
	
	streamA >> x;
   	if (x < TEX_HASH_BASE || x > TEX_FROZEN_CONTROL_SEQUENCE)
		goto bad_fmt;
	hash_used = x;
	cs_count = TEX_FROZEN_CONTROL_SEQUENCE - 1 - hash_used;
	
	if (!undumpSize(streamA, TEX_FONT_BASE, TEX_FONT_MAX, "font max", font_ptr))
		goto bad_fmt;
		
	if (reloadFmt)
	{
		for (i = 0; i < font_entries; i++)
		{
			if (fonttables[i])
				free(fonttables[i]);
		}
		
		free(fonttables);
		fonttables = 0;
		font_entries = 0;
	}
		
	TeXMemoryWord sizeword;
	for (k = TEX_NULL_FONT; k <= font_ptr; k++)
	{
    	if (font_entries==0) 
    	{
      		fonttables = (TeXMemoryWord **)malloc(256*sizeof(TeXMemoryWord**));
      		font_entries=256;
      		for (int i = 0; i < font_entries; i++)
    				fonttables[i] = 0;
    	} 
    	else if ((k==256)&&(font_entries==256)) 
    	{
      		fonttables = (TeXMemoryWord **)realloc(fonttables, 65536 * sizeof(TeXMemoryWord**));
      		TeXMemoryWord ** t = (TeXMemoryWord **)malloc(65536 * sizeof(TeXMemoryWord**));
					for (int i = 0; i < 65536; i++)
    				t[i] = 0;
    	
    			for (int i = 0; i < font_entries; i++)
    				t[i] = fonttables[i];
    	
    			free(fonttables);
    			fonttables = t;
    			font_entries=65536;
    	}

		streamA.readRawData((char*)&sizeword, d);
    	fonttables[k] = (TeXMemoryWord *)malloc((sizeword.ii.CINT0 + 1) * sizeof(TeXMemoryWord));
    	fonttables[k][0].ii.CINT0 = sizeword.ii.CINT0;
    	streamA.readRawData((char*)(&(fonttables[k][1])), d * (sizeword.ii.CINT0));
	}
	
	if (!undumpSize(streamA, 0, TEX_ACTIVE_MEM_SIZE, "active start point", active_min_ptr))
		goto bad_fmt;
		
	if (!undumpSize(streamA, 0, TEX_ACTIVE_MEM_SIZE, "active mem size", active_max_ptr))
		goto bad_fmt;
		
	if (active_max_ptr > 0)
		streamA.readRawData((char*)active_info, d * active_max_ptr);
	
	if (!undumpSize(streamA, TEX_OCP_BASE, TEX_OCP_BIGGEST, "ocp max", ocp_ptr))
		goto bad_fmt;
		
	if (reloadFmt)
	{
		for (i = 0; i < ocp_entries; i++)
		{
			if (ocptables[i])
				free(ocptables[i]);
		}
		
		free(ocptables);
		ocptables = 0;
		ocp_entries = 0;
	}
		
	for (k = 0; k <= ocp_ptr; k++)
	{
    	if (ocp_entries==0) 
    	{
      		ocptables   = (qint32 **)malloc(256 * sizeof(qint32*));
      		ocp_entries = 256;
      		for (int i = 0; i < ocp_entries; i++)
      			ocptables[i] = 0;
    	} 
    	else if ((k==256)&&(ocp_entries==256)) 
    	{
      		qint32** t = (qint32 **)malloc(65536 * sizeof(qint32**));
    			for (int i = 0; i < 65536; i++)
    				t[i] = 0;
    		
    			for (int i = 0; i < ocp_entries; i++)
    				t[i] = ocptables[i];
    			free(ocptables);
    			ocptables = t;
      		ocp_entries = 65536;
    	}
    	
    	streamA >> x;
    	ocptables[k] = (qint32 *)malloc((2 + x) * sizeof(qint32));
    	ocptables[k][0] = x;
    	streamA.readRawData((char*)(&ocptables[k][1]), 4 * x);
	}
	
	if (!undumpSize(streamA, 1, 1000000, "ocp list mem size", ocp_listmem_ptr))
		goto bad_fmt;
				
	if (ocp_listmem_ptr > 0)
		streamA.readRawData((char*)ocp_list_info, d * ocp_listmem_ptr);
	
	if (!undumpSize(streamA, 0, 1000000, "ocp list max", ocp_list_ptr))
		goto bad_fmt;
		
	streamA.readRawData((char*)(&ocp_list_list[TEX_NULL_OCP_LIST]), 4 * (ocp_list_ptr + 1 - TEX_NULL_OCP_LIST));
	
	if (!undumpSize(streamA, 0, 1000000, "ocp lstack mem size", ocp_lstackmem_ptr))
		goto bad_fmt;
		
	if (ocp_lstackmem_ptr > 0)
		streamA.readRawData((char*)ocp_lstack_info, d * ocp_lstackmem_ptr);
	
	if (!undumpSize(streamA, 0, hyph_size, "hyph_size", hyph_count))
		goto bad_fmt;
		
	if (!undumpSize(streamA, TEX_HYPH_PRIME, hyph_size, "hyph_size", hyph_next))
		goto bad_fmt;
		
	j = 0;
	for (k = 1; k <= hyph_count; k++)
	{
		streamA >> j;
		if (j < 0)
			goto bad_fmt;
			
		if (j > 65535)
		{
			hyph_next = j / 65536; 
			j = j - hyph_next * 65536;
		}
		else
			hyph_next = 0;
			
		if ((j >= hyph_size) || (hyph_next > hyph_size))
			goto bad_fmt;
			
		hyph_link[j] = hyph_next;
		
		streamA >> x;
		if (x < 0 || x > str_ptr)
			goto bad_fmt;
		hyph_word[j] = x;
		
		streamA >> x;
		if (x < TEX_MIN_HALFWORD || x > TEX_MAX_HALFWORD)
			goto bad_fmt;
		hyph_list[j] = x;
		
		j++;
		if (j < TEX_HYPH_PRIME)
			j = TEX_HYPH_PRIME;
			
		hyph_next = j;
		if (hyph_next >= hyph_size)
			hyph_next = TEX_HYPH_PRIME;
		else if (hyph_next >= TEX_HYPH_PRIME)
			hyph_next++;
	}
	
	if (!undumpSize(streamA, 0, trie_size, "trie size", j))
		goto bad_fmt;
		
	if (ini_version)
		trie_max = j;
		
	streamA >> x;
	if (x < 0 || x > j)
		goto bad_fmt;		
	hyph_start = x;
	
	if (!trie)
		trie = (TeXTwoHalves*)malloc(( j + 2) * sizeof(TeXTwoHalves));
	streamA.readRawData((char *)trie, 8 * (j + 1));
	
	streamA >> max_hyph_char;
	
	if (!undumpSize(streamA, 0, trie_op_size, "trie op size", j))
		goto bad_fmt;
		
	if (ini_version)
		trie_op_ptr = j;
		
	if (j > 0)
	{
		streamA.readRawData((char*)(&hyf_distance[1]), 4 * j);
		streamA.readRawData((char*)(&hyf_num[1]), 4 * j);
		streamA.readRawData((char*)(&hyf_next[1]), 4 * j);
	}
	
	if (ini_version)
	{
		for (k = 0; k <= TEX_BIGGEST_LANG; k++)
			trie_used[k] = TEX_MIN_QUARTERWORD;
	}
	
	k = TEX_BIGGEST_LANG + 1;
	while (j > 0)
	{
		streamA >> x;
		if (x < 0 || x > (k -1 ))
			goto bad_fmt;
			
		k = x;
		
		streamA >> x;
		if (x < 1 || x > j)
			goto bad_fmt;
			
		if (ini_version)
			trie_used[k] = x;
			
		j = j - x; 
		op_start[k] = j;
	}
	
	if (ini_version)
		trie_not_ready = false;
		
	streamA >> x;
	if (x < TEX_BATCH_MODE || x > TEX_ERROR_STOP_MODE)
		goto bad_fmt;		
	interaction = x;
	if (interaction_option != TEX_UNSPECIFIED_MODE)
		interaction = interaction_option;
	
	streamA >> x;
	if (x < 0 || x > str_ptr)
		goto bad_fmt;		
	format_ident = x;
	
	streamA >> x;
	
	if (x != 69069)
		goto bad_fmt;	
	
	if (!ini_version)
	{
		if (!reloadFmt)
			reloadFmt = true;
	}
	
	return true;
	
bad_fmt:
	fmt_file->close();
	delete fmt_file;
	fmt_file = 0;
	
	dev->termOut(tr("(Fatal format file error; I''m stymied)\n"));
	return false;
}

void XWTeX::storeFmtFile()
{
	qint32 j, k, p, q, x;
	
	if (save_ptr != 0)
	{
		printErr(tr("You can't dump inside a group"));
		help1(tr("`{...\\dump}' is a no-no.")); 
		succumb();
		return ;
	}
	
	qint32 d = 8;
	if (sizeof(double) > 8)
		d = sizeof(double);
	
	selector = TEX_NEW_STRING;
	print(tr(" (format=")); 
	print(job_name); 
	printChar(' ');
	printInt(year()); 
	printChar('.');
	printInt(month()); 
	printChar('.'); 
	printInt(day()); 
	printChar(')');
	if (interaction == TEX_BATCH_MODE)
		selector = TEX_LOG_ONLY;
	else 
		selector = TEX_TERM_AND_LOG;
		
	strRoom(1);
	format_ident = makeString();
	packJobName(TeXAFmt);
	XWXWTexSea sea;
	QStringList tmplist = sea.getConfigDir(XWXWTexSea::Format);
	QString fullname;
	fullname = tmplist[0];
	fullname += "/";
	fullname += dev->dumpName;
	fullname += ".afmt";
	fmt_file = new QFile(fullname);
	fmt_file->open(QIODevice::WriteOnly | QIODevice::Truncate);
	print(tr("\nBeginning to dump on file "));
	print(fullname);
	print("\n");
	slowPrint(format_ident);
	
	QDataStream streamA(fmt_file);
	streamA << (qint32)TEX_MAX_HALFWORD;
	
	streamA << d;
	
	QString formatengine(enginename);
	streamA << (qint32)(formatengine.length());
	streamA << formatengine;
	
	streamA << (qint32)eTeX_mode;
	for (j = 0; j < TEX_ETEX_STATES; j++)
		setNewEqtbInt(TEX_ETEX_STATE_BASE + j, 0);
		
	while (pseudo_files != TEX_NULL)
		pseudoClose();
		
	streamA << (qint32)mem_bot;
	streamA << (qint32)mem_top;
	streamA << (qint32)TEX_EQTB_SIZE;
	streamA << (qint32)TEX_HASH_PRIME;
	streamA << (qint32)TEX_HYPH_PRIME;
	streamA << (qint32)pool_ptr;
	streamA << (qint32)str_ptr;
			
	streamA.writeRawData((const char *)(str_start_ar), 4 * (str_ptr - TEX_TOO_BIG_CHAR + 1));	
	streamA.writeRawData((const char *)(str_pool), 4 * pool_ptr);
	println(); 
	printInt(str_ptr); 
	print(tr(" strings of total length "));
	printInt(pool_ptr);
	
	sortAvail(); 
	var_used = 0;	
	streamA << (qint32)lo_mem_max;
	streamA << (qint32)rover;
	p = mem_bot; 
	q = rover; 
	x = 0;
	do
	{
		streamA.writeRawData((const char *)(&mem[p]), d * (q + 2 - p));
		
		x = x + q + 2 - p; 
		var_used = var_used + q - p;
		p = q + node_size(q); 
		q = rlink(q);
	} while (q != rover);	
	var_used = var_used + lo_mem_max-p; 
	dyn_used = mem_end + 1 - hi_mem_min;
	streamA.writeRawData((const char *)(&mem[p]), d * (lo_mem_max + 1 - p));
	x = x + lo_mem_max + 1 - p;
	streamA << (qint32)hi_mem_min;
	streamA << (qint32)avail;
	streamA.writeRawData((const char *)(&mem[hi_mem_min]), d * (mem_end + 1 - hi_mem_min));
	x = x + mem_end + 1 - hi_mem_min;
	p = avail;
	while (p != TEX_NULL)
	{
		dyn_used--; 
		p = link(p);
	}
	streamA << (qint32)var_used;
	streamA << (qint32)dyn_used;
	println(); 
	printInt(x);
	print(tr(" memory locations dumped; current usage is "));
	printInt(var_used); 
	printChar('&'); 
	printInt(dyn_used);
	
	for (qint32 i = 0; i < TEX_HASHTABLESIZE; i++) 
	{
      	TeXHashWord * runner = &(hashtable[i]);
      	if (runner->p != -1) 
      	{
         	streamA << (qint32)(-i);
         	while (runner->p != -1) 
         	{
            	streamA << (qint32)(runner->p);
            	streamA.writeRawData((const char *)(&(runner->mw)), d);
            	runner = runner->ptr;
         	}
      	}
   	}   	
   	streamA << (qint32)(-TEX_HASHTABLESIZE);
   	streamA << (qint32)par_loc;
   	streamA << (qint32)write_loc;
   	streamA << (qint32)special_loc;
   	streamA << (qint32)hash_used;
   	
   	streamA << (qint32)font_ptr;
   	for (k = TEX_NULL_FONT; k <= font_ptr; k++)
   	{
   		fonttables[k][0].ii.CINT0 = param_base(k) + font_params(k) + 1;
   		streamA.writeRawData((const char *)(&(fonttables[k][0])), d * (fonttables[k][0].ii.CINT0 + 1));
   		printnl("\\font"); 
   		printEsc(fontIdText(k)); 
   		printChar('=');
		printFileName(font_name(k), font_area(k), TEX_TOO_BIG_CHAR);
		if (font_size(k) != font_dsize(k))
		{
			print(" at "); 
			printScaled(font_size(k)); 
			print("pt");
		}
   	}
   	println(); 
   	printInt(font_ptr - TEX_FONT_BASE); 
   	print(tr(" preloaded font"));
   	if (font_ptr != (TEX_FONT_BASE + 1))
   		printChar('s');
   		
   	streamA << (qint32)active_min_ptr;
   	streamA << (qint32)active_max_ptr;
   	if (active_max_ptr > 0)
   		streamA.writeRawData((const char *)(active_info), d * active_max_ptr);   		
   	println(); 
   	printInt(active_max_ptr); 
   	print(" words of active ocps");
   	
   	streamA << (qint32)ocp_ptr;
   	for (k = TEX_NULL_OCP; k <= ocp_ptr; k++)
   	{
   		streamA << ocptables[k][0];
   		streamA.writeRawData((const char *)(&ocptables[k][1]), 4 * (ocptables[k][0]));    		
   		printnl("\\ocp"); 
   		printEsc(ocpIdText(k)); 
   		printChar('=');
			printFileName(ocp_name(k), ocp_area(k), TEX_TOO_BIG_CHAR);
		}
	println(); 
	printInt(ocp_ptr - TEX_OCP_BASE); 
	print(tr(" preloaded ocp"));
	if (ocp_ptr != (TEX_OCP_BASE + 1))
		printChar('s');
		
	streamA << (qint32)ocp_listmem_ptr;
	if (ocp_listmem_ptr > 0)
		streamA.writeRawData((const char *)ocp_list_info, d * ocp_listmem_ptr);
	streamA << (qint32)ocp_list_ptr;
	streamA.writeRawData((const char*)(ocp_list_list), 4 * (ocp_list_ptr + 1 - TEX_NULL_OCP_LIST));
	for (k = TEX_NULL_OCP_LIST; k <= ocp_list_ptr; k++)
	{
		printnl("\\ocplist");
  		printEsc(ocpListIdText(k));
  		printChar('=');
  		printOcpList(ocp_list_list[k]);
	}
	
	streamA << (qint32)ocp_lstackmem_ptr;
	if (ocp_lstackmem_ptr > 0)
	streamA.writeRawData((const char*)ocp_lstack_info, d * ocp_lstackmem_ptr);
		
	streamA << (qint32)hyph_count;
	if (hyph_next <= TEX_HYPH_PRIME)
		hyph_next = hyph_size;
	streamA << (qint32)hyph_next;
	for (k = 0; k <= hyph_size; k++)
	{
		if (hyph_word[k] != 0)
		{
			streamA << (qint32)(k + 65536 * hyph_link[k]);
			streamA << (qint32)hyph_word[k];
			streamA << (qint32)hyph_list[k];
		}
	}	
	println(); 
	printInt(hyph_count); 
	print(tr(" hyphenation exception"));
	if (hyph_count != 1)
		printChar('s');
	if (trie_not_ready)
		initTrie();
	streamA << (qint32)trie_max;
	streamA << (qint32)hyph_start;
	streamA.writeRawData((const char *)(trie), d * (trie_max + 1));
	streamA << (qint32)max_hyph_char;
	streamA << (qint32)trie_op_ptr;
	if (trie_op_ptr > 0)
	{
		streamA.writeRawData((const char *)(&hyf_distance[1]), 4 * trie_op_ptr);
		streamA.writeRawData((const char *)(&hyf_num[1]), 4 * trie_op_ptr);
		streamA.writeRawData((const char *)(&hyf_next[1]), 4 * trie_op_ptr);
	}
	
	printnl(tr("Hyphenation trie of length ")); 
	printInt(trie_max);
	print(" has "); 
	printInt(trie_op_ptr); 
	print(" op");
	if (trie_op_ptr != 1)
		printChar('s');
	print(tr(" out of ")); 
	printInt(trie_op_size);
	for (k = TEX_BIGGEST_LANG; k >= 0; k--)
	{
		if (trie_used[k] > TEX_MIN_QUARTERWORD)
		{
			printnl("  "); 
			printInt(trie_used[k]);
  		print(tr(" for language ")); 
  		printInt(k);
  		streamA << (qint32)k;
  		streamA << (qint32)(trie_used[k]);
  	}
	}
	
	streamA << (qint32)interaction;
	streamA << (qint32)format_ident;
	streamA << (qint32)69069;
	setNewEqtbInt(TEX_INT_BASE + TEX_TRACING_STATS_CODE, 0);
	
	fmt_file->close();
	delete fmt_file;
	fmt_file = 0;
}

bool XWTeX::undumpSize(QDataStream & streamA,
	                   qint32 min, 
	                   qint32 v, 
	                   const QString & str, 
	                   qint32 & y)
{
	y = min;
	qint32 x;
	streamA >> x;
	if (x < min)
		return false;
	
	if (x > v)
	{
		QString msg = QString(tr("---! Must increase the %1\n")).arg(str);		
		dev->termOut(msg);
		return false;
	}
	
	if (debug_format_file)
	{
		QString msg = QString(tr("fmtdebug: %1 = %2\n")).arg(str).arg(x);
		dev->termOut(msg);
	}	
	
	y = x;
	
	return true;
}
