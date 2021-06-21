/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <QDateTime>
#include <QByteArray>
#include <QTextStream>
#include <QTextCodec>
#include <QFile>
#include "XWApplication.h"
#include "XWFileName.h"
#include "XWTexSea.h"
#include "XWTeX.h"

void XWTeX::allocateOcpTable(qint32 ocp_number, qint32 ocp_size)
{
	if (ocp_entries==0) 
	{
      	ocptables = (qint32 **)malloc(256 * sizeof(qint32**));
      	ocp_entries=256;
      	
      	for (int i = 0; i < ocp_entries; i++)
      		ocptables[i] = 0;
    } 
    else if ((ocp_number == 256) && (ocp_entries == 256)) 
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
    
    ocptables[ocp_number] = (qint32 *)malloc((2 + ocp_size) * sizeof(qint32));
    ocptables[ocp_number][0] = ocp_size;
    for (int i = 1; i <= ocp_size; i++)
    	ocptables[ocp_number][i]  = 0;
}

void XWTeX::activeCompile()
{
	active_min_ptr = active_max_ptr;
	qint32 min_stack_ocp  = TEX_OCP_MAXINT;
	qint32 max_active = equiv(TEX_OCP_ACTIVE_NUMBER_BASE)-1;
	
	qint32 i = max_active;
	qint32 stack_value, min_index;
	for (; i >= 0; i--)
	{
		holding[i] = ocp_list_list[equiv(TEX_OCP_ACTIVE_BASE + i)];
  		stack_value = ocp_list_lstack_no(holding[i]);
  		if (stack_value < min_stack_ocp)
  		{
  			min_index = i;
    		min_stack_ocp = stack_value;
  		}
	}
	
	while (min_stack_ocp < TEX_OCP_MAXINT)
	{
		addOcpStack(min_index, min_stack_ocp);
		qint32 old_min = min_stack_ocp;
  		min_stack_ocp = TEX_OCP_MAXINT;
  		for (i = max_active; i >= 0; i--)
  		{
  			stack_value = ocp_list_lstack_no(holding[i]);
  			while (old_min == stack_value)
  			{
  				holding[i]  = ocp_list_lnext(holding[i]);
      			stack_value = ocp_list_lstack_no(holding[i]);
  			}
  			
  			if (stack_value < min_stack_ocp)
  			{
  				min_index     = i;
      			min_stack_ocp = stack_value;
  			}
  		}
	}
}

void XWTeX::addOcpStack(qint32 min_index, qint32 )
{
	qint32 p = ocp_list_lstack(holding[min_index]);
	qint32 llstack_no = ocp_list_lstack_no(holding[min_index]);
	qint32 counter = 0;
	while (!(isNullOcpLStack(p)))
	{
		active_ocp(active_max_ptr) = ocp_lstack_ocp(p);
  		active_counter(active_max_ptr) = (quint16)counter;
  		active_lstack_no(active_max_ptr) = llstack_no;
  		p = ocp_lstack_lnext(p);
  		active_max_ptr = active_max_ptr + 2;
  		counter++;
	}
}

qint32 XWTeX::copyOcpList(qint32 list)
{
	qint32 result;
	if (isNullOcpList(list))
		result = makeNullOcpList();
	else
		result = makeOcpListNode(copyOcpLStack(ocp_list_lstack(list)), 
		                         ocp_list_lstack_no(list),
                                 copyOcpList(ocp_list_lnext(list)));
                                 
	return result;
}

qint32 XWTeX::copyOcpLStack(qint32 llstack)
{
	qint32 result;
	if (isNullOcpLStack(llstack))
		result = TEX_MAKE_NULL_OCP_LSTACK;
	else
		result = makeOcpLStackNode(ocp_lstack_ocp(llstack), copyOcpLStack(ocp_lstack_lnext(llstack)));
		
	return result;
}

void XWTeX::doClearOcpLists(qint32 a)
{
	if (a >= 4)
		geqDefine(TEX_OCP_ACTIVE_NUMBER_BASE, (quint16)TEX_DATA, 0);
	else
		eqDefine(TEX_OCP_ACTIVE_NUMBER_BASE, (quint16)TEX_DATA, 0);
		
	activeCompile();
	
	if (a >= 4)
		geqDefine(TEX_OCP_ACTIVE_MIN_PTR_BASE, (quint16)TEX_DATA, active_min_ptr);
	else
		eqDefine(TEX_OCP_ACTIVE_MIN_PTR_BASE, (quint16)TEX_DATA, active_min_ptr);
		
	if (a >= 4)
		geqDefine(TEX_OCP_ACTIVE_MAX_PTR_BASE, (quint16)TEX_DATA, active_max_ptr);
	else
		eqDefine(TEX_OCP_ACTIVE_MAX_PTR_BASE, (quint16)TEX_DATA, active_max_ptr);
}

void XWTeX::doPopOcpList(qint32 a)
{
	qint32 old_number, i;
	
	old_number = equiv(TEX_OCP_ACTIVE_NUMBER_BASE);
	if (old_number == 0)
		printErr(tr("No active ocp lists to be popped"));
	else
	{
		if (a >= 4)
			geqDefine(TEX_OCP_ACTIVE_NUMBER_BASE, (quint16)TEX_DATA, (old_number - 1));
		else
			eqDefine(TEX_OCP_ACTIVE_NUMBER_BASE, (quint16)TEX_DATA, (old_number - 1));
	}
	
	if (equiv(TEX_OCP_TRACE_LEVEL_BASE) == 1)
	{
		printnl(tr("New active ocp list: {"));
		for (i = (old_number - 2); i >= 0; i--)
		{
			printEsc(ocpListIdText(equiv(TEX_OCP_ACTIVE_BASE + i)));
    		print("="); 
    		printOcpList(ocp_list_list[equiv(TEX_OCP_ACTIVE_BASE + i)]);
    		if (i != 0)
    			print(",");
		}
		print("}");
	}
	
	activeCompile();
	
	if (a >= 4)
		geqDefine(TEX_OCP_ACTIVE_MIN_PTR_BASE, (quint16)TEX_DATA, active_min_ptr);
	else
		eqDefine(TEX_OCP_ACTIVE_MIN_PTR_BASE, (quint16)TEX_DATA, active_min_ptr);
		
	if (a >= 4)
		geqDefine(TEX_OCP_ACTIVE_MAX_PTR_BASE, (quint16)TEX_DATA, active_max_ptr);
	else
		eqDefine(TEX_OCP_ACTIVE_MAX_PTR_BASE, (quint16)TEX_DATA, active_max_ptr);
}

void XWTeX::doPushOcpList(qint32 a)
{
	qint32 ocp_list_no, old_number, i;
	
	scanOcpListIdent(); 
	ocp_list_no = cur_val;
	old_number = equiv(TEX_OCP_ACTIVE_NUMBER_BASE);
	if (a >= 4)
		geqDefine(TEX_OCP_ACTIVE_BASE + old_number, (quint16)TEX_DATA, ocp_list_no);
	else
		eqDefine(TEX_OCP_ACTIVE_BASE + old_number, (quint16)TEX_DATA, ocp_list_no);
		
	if (a >= 4)
		geqDefine(TEX_OCP_ACTIVE_NUMBER_BASE, (quint16)TEX_DATA, (old_number + 1));
	else
		eqDefine(TEX_OCP_ACTIVE_NUMBER_BASE, (quint16)TEX_DATA, (old_number + 1));
		
	if (equiv(TEX_OCP_TRACE_LEVEL_BASE) == 1)
	{
		printnl(tr("New active ocp list: {"));
		for (i = old_number; i >= 0; i--)
		{
			printEsc(ocpListIdText(equiv(TEX_OCP_ACTIVE_BASE + i)));
    		print("="); 
    		printOcpList(ocp_list_list[equiv(TEX_OCP_ACTIVE_BASE + i)]);
    		if (i != 0)
    			print(",");
		}
		print("}");
	}
	
	activeCompile();
	
	if (a >= 4)
		geqDefine(TEX_OCP_ACTIVE_MIN_PTR_BASE, (quint16)TEX_DATA, active_min_ptr);
	else
		eqDefine(TEX_OCP_ACTIVE_MIN_PTR_BASE, (quint16)TEX_DATA, active_min_ptr);
		
	if (a >= 4)
		geqDefine(TEX_OCP_ACTIVE_MAX_PTR_BASE, (quint16)TEX_DATA, active_max_ptr);
	else
		eqDefine(TEX_OCP_ACTIVE_MAX_PTR_BASE, (quint16)TEX_DATA, active_max_ptr);
}

bool XWTeX::isLastOcp(qint32 llstack_no, qint32 counter)
{
	active_min_ptr = equiv(TEX_OCP_ACTIVE_MIN_PTR_BASE);
	active_max_ptr = equiv(TEX_OCP_ACTIVE_MAX_PTR_BASE);
	active_real = active_min_ptr;
	while ((active_real < active_max_ptr) && 
			(active_lstack_no(active_real) < llstack_no))
   	{
   		active_real = active_real + 2;
   	}
   	
   	while ((active_real < active_max_ptr) && 
      		(active_lstack_no(active_real) == llstack_no) && 
      		(active_counter(active_real) <= (quint16)counter))
	{
		active_real = active_real + 2;
	}
	
	return (active_real == active_max_ptr);
}

qint32 XWTeX::makeOcpListNode(qint32 llstack, 
	                          qint32 llstack_no, 
	                          qint32 llnext)
{
	qint32 p = ocp_listmem_run_ptr;
	ocp_list_lstack(p) = (quint16)llstack;
	ocp_list_lstack_no(p) = llstack_no;
	ocp_list_lnext(p) = (quint16)llnext;
	ocp_listmem_run_ptr = ocp_listmem_run_ptr + 2;
	return p;
}

qint32 XWTeX::makeOcpLStackNode(qint32 locp, qint32 llnext)
{
	qint32 p = ocp_lstackmem_run_ptr;
	ocp_lstack_ocp(p) = (quint16)locp;
	ocp_lstack_lnext(p) = (quint16)llnext;
	ocp_lstackmem_run_ptr++;
	return p;
}

void XWTeX::newOcp(qint32 a)
{
	qint32 u, f, t, flushable_string;
	bool external_ocp;
	if (job_name == 0)
		openLogFile();
		
	if (cur_chr == 1)
		external_ocp = true;
	else
		external_ocp = false;
		
	getRToken(); 
	u = cur_cs;
	if (u >= TEX_HASH_BASE)
		t = newText(u);
	else if (u >= TEX_SINGLE_BASE)
	{
		if (u == TEX_NULL_CS)
			t = TeXOCP;
		else
			t = u - TEX_SINGLE_BASE;
	}
	else
	{
		old_setting = selector; 
		selector = TEX_NEW_STRING;
		print("OCP"); 
		print(u - TEX_ACTIVE_BASE); 
		selector = old_setting;
		strRoom(1); 
		t = makeString();
	}
	
	if (a >= 4)
		geqDefine(u, (quint16)TEX_SET_OCP, TEX_NULL_OCP);
	else
		eqDefine(u, (quint16)TEX_SET_OCP, TEX_NULL_OCP);
		
	scanOptionalEquals(); 
	scanFileName();
		
	flushable_string = str_ptr - 1;
	for (f = (TEX_OCP_BASE + 1); f <= ocp_ptr; f++)
	{
		if (strEqStr(ocp_name(f), cur_name) && strEqStr(ocp_area(f), cur_area))
		{
			if (cur_name == flushable_string)
			{
				flushString(); 
				cur_name = ocp_name(f);
			}
			
			goto common_ending;
		}
	}
	
	f = readOcpInfo(u, cur_name, cur_area, cur_ext, external_ocp);
	
common_ending:
	setEquiv(u,f); 
	setNewEqtb(TEX_OCP_ID_BASE + f, newEqtb(u));
	setText(TEX_OCP_ID_BASE + f, t);
	if (equiv(TEX_OCP_TRACE_LEVEL_BASE) == 1)
	{
		printnl(""); 
		printEsc(TeXOCP); 
		printEsc(t); 
		print("="); 
		print(cur_name);
	}
}

void XWTeX::newOcpList(qint32 a)
{
	qint32 u, f, t;
	
	if (job_name == 0)
		openLogFile();
		
	getRToken(); 
	u = cur_cs;
	if (u >= TEX_HASH_BASE)
		t = newText(u);
	else if (u >= TEX_SINGLE_BASE)
	{
		if (u == TEX_NULL_CS)
			t = TeXOcpList;
		else
			t = u - TEX_SINGLE_BASE;
	}
	else
	{
		old_setting = selector; 
		selector = TEX_NEW_STRING;
		print("OCPLIST"); 
		print(u - TEX_ACTIVE_BASE); 
		selector = old_setting;
		strRoom(1); 
		t = makeString();
	}
	
	if (a >= 4)
		geqDefine(u, (quint16)TEX_SET_OCP_LIST, TEX_NULL_OCP_LIST);
	else
		eqDefine(u, (quint16)TEX_SET_OCP_LIST, TEX_NULL_OCP_LIST);
		
	scanOptionalEquals();
	f = readOcpList();
    setEquiv(u,f); 
    setNewEqtb(TEX_OCP_LIST_ID_BASE + f, newEqtb(u));
    setText(TEX_OCP_LIST_ID_BASE + f, t);
    if (equiv(TEX_OCP_TRACE_LEVEL_BASE) == 1)
    {
    	printnl(""); 
    	printEsc(TeXOcpList); 
    	printEsc(t); 
    	print("=");
  		printOcpList(ocp_list_list[f]);
    }
}

void XWTeX::ocpApplyAdd(qint32 list_entry, 
	                    bool lbefore, 
	                    qint32 locp)
{
	qint32 p, q;
	p = ocp_list_lstack(list_entry);
	if (lbefore || (p == 0))
	{
		ocp_list_lstack(list_entry) = (quint16)makeOcpLStackNode(locp, p);
	}
	else
	{
		q = ocp_lstack_lnext(p);
		while (q != 0)
		{
			p = q; 
			q = ocp_lstack_lnext(q);
		}
		
		ocp_lstack_lnext(p) = (quint16)makeOcpLStackNode(locp, TEX_NULL);
	}
}

void XWTeX::ocpApplyRemove(qint32 list_entry, bool lbefore)
{
	qint32 p, q, r;
	p = ocp_list_lstack(list_entry);
	if (p == 0)
	{
		printErr(tr("warning: stack entry already empty")); 
		println();
	}
	else
	{
		q = ocp_lstack_lnext(p);
		if (lbefore || (q == 0))
			ocp_list_lstack(list_entry) = (quint16)q;
		else
		{
			r = ocp_lstack_lnext(q);
			while (r != 0)
			{
				p = q; 
				q = r; 
				r = ocp_lstack_lnext(r);
			}
			ocp_lstack_lnext(p) = (quint16)TEX_NULL;
		}
	}
}

qint32 XWTeX::ocpEnsureLStack(qint32 list, qint32 llstack_no)
{
	qint32 p, q;
	p = list;
	if (isNullOcpList(p))
	{
		ocp_list_lstack_no(p) = llstack_no;
  		ocp_list_lnext(p) = (quint16)makeNullOcpList();
	}
	else if (ocp_list_lstack_no(p) > llstack_no)
	{
		ocp_list_lnext(p) = (quint16)makeOcpListNode(ocp_list_lstack(p),
                       						ocp_list_lstack_no(p),
                       						ocp_list_lnext(p));
  		ocp_list_lstack(p) = (quint16)0;
  		ocp_list_lstack_no(p) = llstack_no;
	}
	else
	{
		q = ocp_list_lnext(p);
		while (!(isNullOcpList(q)) && (ocp_list_lstack_no(q) <= llstack_no))
		{
			p = q; 
			q = ocp_list_lnext(q);
		}
		if (ocp_list_lstack_no(p) < llstack_no)
		{
			ocp_list_lnext(p) = (quint16)makeOcpListNode(0, llstack_no, q);
    		p = ocp_list_lnext(p);
		}
	}
	
	return p;
}

qint32 XWTeX::ocpRead()
{
	uchar c;
	if (!ocp_file->getChar((char*)(&c)))
		return 0;
	
	qint32 ocpword = c;
	if (ocpword > 127)
	{
		print(tr("OCP file error ("));
		print(tr("opening file")); 
		print(")"); 
		println(); 
		return 0;
	}
	
	if (!ocp_file->getChar((char*)(&c)))
		return 0;		
	ocpword = ocpword * 256 + c;
	if (!ocp_file->getChar((char*)(&c)))
		return 0;	
	ocpword = ocpword * 256 + c;
	if (!ocp_file->getChar((char*)(&c)))
		return 0;		
	ocpword = ocpword * 256 + c;	
	return ocpword;
}

qint32 XWTeX::readOcpInfo(qint32 u, 
	                      qint32 nom, 
	                      qint32 aire, 
	                      qint32 ext, 
	                      bool external_ocp)
{
	bool file_opened;
	qint32 f, g, ocpmem_run_ptr, ocp_length, real_ocp_length, previous_address;
	qint32 temp_ocp_input, temp_ocp_output, temp_ocp_no_tables, temp_ocp_no_states;
	qint32 i, new_offset, room_for_tables, room_for_states;
	g = TEX_NULL_OCP;
	
	if (external_ocp)
	{
		file_opened = false;
		packFileName(nom, aire, ext);
		XWTexSea sea;
		QString fn0 = getFileName();
		QString fn = sea.findFile(fn0, XWTexSea::OCP);
		if (!fn.isEmpty())
		{
			if (name_of_file)
				free(name_of_file);
				
			name_length = fn.length();
			name_of_file = (qint32*)malloc((name_length + 4) * sizeof(qint32));
			
			for (i = 0; i < name_length; i++)
				name_of_file[i + 1] = fn[i].unicode();
				
			name_of_file[name_length + 1] = 0;
		}
		else
		{
			name_of_file[0] = 0;
			name_length = 0;			
		}
		
		if (name_length == 0)
		{
			print(tr("OCP file error ("));
			print(tr("opening file")); 
			print(")"); 
			println(); 
			goto bad_ocp;
		}
		
		f = ocp_ptr + 1;
		allocateOcpTable(f, 13);
		ocp_file_size(f) = 13;
		for (i = 1; i <= name_length; i++)
			appendChar(name_of_file[i]);
			
		ocp_external(f) = makeString();
		scanStringArgument();
		ocp_external_arg(f) = cur_val;
		ocp_name(f) = TEX_TOO_BIG_CHAR; 
		ocp_area(f) = TEX_TOO_BIG_CHAR;
		ocp_state_base(f) = 0; 
		ocp_table_base(f) = 0;
		ocp_input(f) = 1; 
		ocp_output(f) = 1;
		ocp_info(f, TEX_OFFSET_OCP_INFO) = 0;
		ocp_ptr = f; 
		g = f;
		goto done;
	}
	else
	{
		file_opened = false;
		packFileName(nom, aire, TeXOcp);
		if (ocp_file)
		{
			ocp_file->close();
			delete ocp_file;
		}
		XWTexSea sea;
		QString fn = getFileName();
		ocp_file = sea.openFile(fn, XWTexSea::OCP);
		if (!ocp_file)
		{
			print(tr("OCP file error ("));
			print(tr("opening file")); 
			print(")"); 
			println(); 
			goto bad_ocp;
		}
		
		file_opened = true;
		f  = ocp_ptr + 1;
		ocpmem_run_ptr = TEX_OFFSET_OCP_INFO;
		
		ocp_length = ocpRead();
		real_ocp_length = ocp_length - 7;
		temp_ocp_input  = ocpRead();
		temp_ocp_output = ocpRead();
		temp_ocp_no_tables = ocpRead();
		room_for_tables  = ocpRead();
		temp_ocp_no_states = ocpRead();
		room_for_states = ocpRead();
		
		if (real_ocp_length != 
			(temp_ocp_no_tables + room_for_tables + 
			temp_ocp_no_states + room_for_states))
		{
			print(tr("OCP file error ("));
			print(tr("checking size")); 
			print(")"); 
			println(); 
			goto bad_ocp;
		}
		
		real_ocp_length = real_ocp_length + 12 + temp_ocp_no_states + temp_ocp_no_tables;
		allocateOcpTable(f, real_ocp_length);
		ocp_external(f) = 0;
		ocp_external_arg(f) = 0;
		ocp_file_size(f) = real_ocp_length;
		ocp_input(f) = temp_ocp_input;
		ocp_output(f) = temp_ocp_output;
		ocp_no_tables(f) = temp_ocp_no_tables;
		ocp_no_states(f) = temp_ocp_no_states;
		ocp_table_base(f) = ocpmem_run_ptr;
		if (ocp_no_tables(f) != 0)
		{
			previous_address = ocpmem_run_ptr + 2 * (ocp_no_tables(f));
			for (i = 1; i <= ocp_no_tables(f); i++)
			{
				ocptables[f][ocpmem_run_ptr] = previous_address;
  				ocpmem_run_ptr++;
  				new_offset = ocpRead();
  				ocptables[f][ocpmem_run_ptr] = new_offset;
  				ocpmem_run_ptr++;
  				previous_address = previous_address + new_offset;
			}
		}
		
		if (room_for_tables != 0)
		{
			for (i = 1; i <= room_for_tables; i++)
			{
				ocptables[f][ocpmem_run_ptr] = ocpRead();
  				ocpmem_run_ptr++;
			}
		}
		
		ocp_state_base(f) = ocpmem_run_ptr;
		
		if (ocp_no_states(f) != 0)
		{
			previous_address = ocpmem_run_ptr + 2 * (ocp_no_states(f));
			for (i = 1; i <= ocp_no_states(f); i++)
			{
				ocptables[f][ocpmem_run_ptr] = previous_address;
  				ocpmem_run_ptr++;
    			new_offset = ocpRead();
  				ocptables[f][ocpmem_run_ptr] = new_offset;
  				ocpmem_run_ptr++;
    			previous_address = previous_address + new_offset;
			}
		}
		
		if (room_for_states != 0)
		{
			for (i = 1; i <= room_for_states; i++)
			{
				ocptables[f][ocpmem_run_ptr] = ocpRead();
  				ocpmem_run_ptr++;
			}
		}
		
		ocp_ptr = f; 
		g = f;
		goto done;
	}
	
bad_ocp: 
	printErr(tr("Translation process "));
   	sprintCS(u); 
   	printChar('='); 
   	printFileName(nom, aire, TEX_TOO_BIG_CHAR);
   	if (file_opened)
   		print(tr(" not loadable: Bad ocp file"));
   	else
   		print(tr(" not loadable: ocp file not found"));
   		
   	help2(tr("I wasn't able to read the data for this ocp,"));
	help_line[0] = tr("so I will ignore the ocp specification.");
	error();
	return g;
	
done:
	if (file_opened)
	{
		ocp_file->close();
		delete ocp_file;
		ocp_file = 0;
	}
	
	ocp_name(f) = nom; 
	ocp_area(f) = aire;
	
	return g;
}

qint32 XWTeX::readOcpList()
{
	qint32 g = TEX_NULL_OCP_LIST;
	qint32 f = ocp_list_ptr + 1;
	ocp_listmem_run_ptr = ocp_listmem_ptr;
	ocp_lstackmem_run_ptr = ocp_lstackmem_ptr;
	ocp_list_list[f] = scanOcpList();
	ocp_list_ptr = f;
	ocp_listmem_ptr = ocp_listmem_run_ptr;
	ocp_lstackmem_ptr = ocp_lstackmem_run_ptr;
	g = f;
	return g;
}

void XWTeX::runExternalOcp()
{
	QString infilename = xwApp->getTmpFile("alephin");	
	QFile in_file(infilename);
	in_file.open(QIODevice::WriteOnly);
	qint32 c;
	for (qint32 i = 1; i <= otp_input_end; i++)
	{
		c = otp_input_buf[i];
		if (c > 0x4000000)
		{
			in_file.putChar((char)(0xfc | ((c>>30) & 0x1)));
          	in_file.putChar((char)(0x80 | ((c>>24) & 0x3f)));
          	in_file.putChar((char)(0x80 | ((c>>18) & 0x3f)));
          	in_file.putChar((char)(0x80 | ((c>>12) & 0x3f)));
          	in_file.putChar((char)(0x80 | ((c>>6) & 0x3f)));
          	in_file.putChar((char)(0x80 | (c & 0x3f)));
		}
		else if (c > 0x200000)
		{
			in_file.putChar((char)(0xf8 | ((c>>24) & 0x3)));
          	in_file.putChar((char)(0x80 | ((c>>18) & 0x3f)));
          	in_file.putChar((char)(0x80 | ((c>>12) & 0x3f)));
          	in_file.putChar((char)(0x80 | ((c>>6) & 0x3f)));
          	in_file.putChar((char)(0x80 | (c & 0x3f)));
		}
		else if (c > 0x10000)
		{
			in_file.putChar((char)(0xf0 | ((c>>18) & 0x7)));
          	in_file.putChar((char)(0x80 | ((c>>12) & 0x3f)));
          	in_file.putChar((char)(0x80 | ((c>>6) & 0x3f)));
          	in_file.putChar((char)(0x80 | (c & 0x3f)));
		}
		else if (c > 0x800)
		{
			in_file.putChar((char)(0xe0 | ((c>>12) & 0xf)));
          	in_file.putChar((char)(0x80 | ((c>>6) & 0x3f)));
          	in_file.putChar((char)(0x80 | (c & 0x3f)));
		}
		else if (c > 0x80)
		{
			in_file.putChar((char)(0xc0 | ((c>>6) & 0x1f)));
          	in_file.putChar((char)(0x80 | (c & 0x3f)));
		}
		else
			in_file.putChar((char)(c & 0x7f));
	}
	
	in_file.close();
	
	QString outfilename = xwApp->getTmpFile("alephout");
	QString fn = getFileName();
	QString command_line = QString("%1 <%2 >%3\n").arg(fn).arg(infilename).arg(outfilename);		
	QTextCodec * codec = QTextCodec::codecForLocale();
	QByteArray ba = codec->fromUnicode(command_line);
	system(ba.constData());
	QFile out_file(outfilename);
	out_file.open(QIODevice::ReadOnly);
	otp_output_end = 0;
  	otp_output_buf[otp_output_end] = 0;
  	uchar c_in = 0;
  	while (out_file.getChar((char*)(&c_in)))
  	{
  		if (c_in >= 0xfc)
  		{
  			c = (c_in & 0x1)   << 30;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;  				
  			c |= (c_in & 0x3f) << 24;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;  				
  			c |= (c_in & 0x3f) << 18;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;  
  			c |= (c_in & 0x3f) << 12;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;  
  			c |= (c_in & 0x3f) << 6;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;  
  			c |= c_in & 0x3f;
  		}
  		else if (c_in >= 0xf8)
  		{
  			c = (c_in & 0x3) << 24;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;  
  			c |= (c_in & 0x3f) << 18;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;  
  			c |= (c_in & 0x3f) << 12;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;
  			c |= (c_in & 0x3f) << 6;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;
  			c |= c_in & 0x3f;
  		}
  		else if (c_in >= 0xf0)
  		{
  			c = (c_in & 0x7) << 18;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;
  			c |= (c_in & 0x3f) << 12;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;
  			c |= (c_in & 0x3f) << 6;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;
  			c |= c_in & 0x3f;
  		}
  		else if (c_in >= 0xe0)
  		{
  			c = (c_in & 0xf) << 12;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;
  			c |= (c_in & 0x3f) << 6;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;
  			c |= c_in & 0x3f;
  		}
  		else if (c_in >= 0x80)
  		{
  			c = (c_in & 0x1f) << 6;
  			
  			if (!out_file.getChar((char*)(&c_in)))
  				break;
  			c |= c_in & 0x3f;
  		}
  		else
  			c = c_in & 0x7f;
  			
  		otp_output_buf[++otp_output_end] = c;
  	}
  	
  	out_file.close();
  	
  	in_file.remove();
  	out_file.remove();
}

