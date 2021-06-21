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


TeXHashWord * XWTeX::createEqtbPos(qint32 p)
{
	TeXHashWord *runner = &(hashtable[p % TEX_HASHTABLESIZE]);
   	if (runner->p==p) 
   		return runner;
   	while (runner->p != -1) 
   	{
      	runner = runner->ptr; 
      	if (runner->p == p) 
      		return runner;
   	} 
   	
   runner->p = p;
   initEqtbEntry(p, runner->mw);
   runner->ptr = (TeXHashWord *)malloc(sizeof(TeXHashWord));
   (runner->ptr)->p = -1;
   return runner;
}

TeXHashWord * XWTeX::createHashPos(qint32 p)
{
	TeXHashWord * runner = &(hashtable[p % TEX_HASHTABLESIZE]);
	if (runner->p == p) 
		return runner;
		
	while (runner->p != -1) 
	{
      	runner = runner->ptr; 
      	if (runner->p == p) 
      		return runner;
   	} 
   	
   	runner->p = p;
   	runner->mw.ii.CINT0 = 0;
   	runner->mw.ii.CINT1 = 0;
   	runner->ptr = (TeXHashWord *)malloc(sizeof(TeXHashWord));
   	(runner->ptr)->p = -1;
   	return runner;
}

TeXHashWord * XWTeX::createXEqLevel(qint32 p)
{
	TeXHashWord *runner= &(hashtable[p % TEX_HASHTABLESIZE]);
   	if (runner->p==p) 
   		return runner;
   	while (runner->p != -1) 
   	{
      	runner = runner->ptr;
      	if (runner->p == p) 
      		return runner;
   	}
   	runner->p = p;
   	runner->mw.ii.CINT0 = 1;
   	runner->mw.ii.CINT1 = 0;
   	runner->ptr = (TeXHashWord *)malloc(sizeof(TeXHashWord));
   	(runner->ptr)->p = -1;
   	return runner;
}

void XWTeX::delEqWordDefine(qint32 p, qint32 w, qint32 wone)
{
	if (xeqLevel(p) != cur_level)
	{
		eqSave(p, xeqLevel(p)); 
		setXEqLevel(p, cur_level);
	}
	
	setEquiv(p,w); 
	setEquiv1(p, wone);
}

void XWTeX::delGeqWordDefine(qint32 p, qint32 w, qint32 wone)
{
	setEquiv(p,w); 
	setEquiv1(p,wone); 
	setXEqLevel(p, TEX_LEVEL_ONE);
}

void XWTeX::eqDefine(qint32 p, quint16 t, qint32 e)
{
	if (eTeX_ex && (eqType(p) == t) && (equiv(p) ==e))
	{
#ifdef XW_TEX_STAT
		assignTrace(p, tr("reassigning"));
#endif //XW_TEX_STAT
		eqDestroy(newEqtb(p)); 
		return;
	}
#ifdef XW_TEX_STAT
	assignTrace(p, tr("changing"));
#endif //XW_TEX_STAT
	if (eqLevel(p) == cur_level)
		eqDestroy(newEqtb(p));
	else if (cur_level > TEX_LEVEL_ONE)
		eqSave(p, eqLevel(p));
		
	setEqLevel(p, cur_level); 
	setEqType(p,t); 
	setEquiv(p,e);
	
#ifdef XW_TEX_STAT
	assignTrace(p, tr("into"));
#endif //XW_TEX_STAT
}

void XWTeX::eqDestroy(TeXMemoryWord & w)
{
	switch (eq_type_field(w))
	{
		case TEX_CALL:
		case TEX_LONG_CALL:
		case TEX_OUTER_CALL:
		case TEX_LONG_OUTER_CALL:
			deleteTokenRef(equiv_field(w));
			break;
			
		case TEX_GLUE_REF: 
			deleteGlueRef(equiv_field(w));
			break;
			
		case TEX_SHAPE_REF:
			{
				qint32 q = equiv_field(w);
				if (q != TEX_NULL)
					freeNode(q, info(q) + info(q) + 1);
			}
			break;
			
		case TEX_BOX_REF: 
			flushNodeList(equiv_field(w));
			break;
			
		case TEX_TOKS_REGISTER:
		case TEX_REGISTER:
			if ((equiv_field(w) < mem_bot) || (equiv_field(w) > lo_mem_stat_max))
				deleteSaRef(equiv_field(w));
			break;
			
		default:
			break;
	}
}

void XWTeX::eqSave(qint32 p, qint16 l)
{
	if (!checkFullSaveStack())
		return ;
		
	if (l == TEX_LEVEL_ZERO)
		save_type(save_ptr) = (quint16)TEX_RESTORE_ZERO;
	else
	{
		save_stack[save_ptr] = newEqtb(p); 
		save_ptr++;
  		save_type(save_ptr) = (quint16)TEX_RESTORE_OLD_VALUE;
	}
	
	save_level(save_ptr) = (quint16)l; 
	save_index(save_ptr) = p; 
	save_ptr++;
}

void XWTeX::eqWordDefine(qint32 p, qint32 w)
{
	if (eTeX_ex && (newEqtbInt(p) == w))
	{
#ifdef XW_TEX_STAT
		assignTrace(p, tr("reassigning"));
#endif //XW_TEX_STAT
		return;
	}
	
#ifdef XW_TEX_STAT
	assignTrace(p, tr("changing"));
#endif //XW_TEX_STAT

	if (xeqLevel(p) != cur_level)
	{
		eqSave(p, xeqLevel(p)); 
		setXEqLevel(p, cur_level);
	}
	
	setNewEqtbInt(p, w);
#ifdef XW_TEX_STAT
	assignTrace(p, tr("into"));
#endif //XW_TEX_STAT
}

void XWTeX::geqDefine(qint32 p, quint16 t, qint32 e)
{
#ifdef XW_TEX_STAT
	assignTrace(p, tr("globally changing"));
#endif //XW_TEX_STAT

	eqDestroy(newEqtb(p));
	setEqLevel(p, (quint16)TEX_LEVEL_ONE); 
	setEqType(p, t); 
	setEquiv(p,e);

#ifdef XW_TEX_STAT
	assignTrace(p, tr("into"));
#endif //XW_TEX_STAT
}

void XWTeX::geqWordDefine(qint32 p, qint32 w)
{
#ifdef XW_TEX_STAT
	assignTrace(p, tr("globally changing"));
#endif //XW_TEX_STAT

	setNewEqtbInt(p, w); 
	setXEqLevel(p, TEX_LEVEL_ONE);
	
#ifdef XW_TEX_STAT
	assignTrace(p, tr("into"));
#endif //XW_TEX_STAT
}

void XWTeX::initEqtbEntry(qint32 p, TeXMemoryWord & mw)
{	
	if ((p >= TEX_ACTIVE_BASE) && (p <= TEX_UNDEFINED_CONTROL_SEQUENCE))
	{
		//Regions 1 and 2
		equiv_field(mw) = TEX_NULL;
     	eq_type_field(mw) = (quint16)TEX_UNDEFINED_CS;
     	eq_level_field(mw) = (quint16)TEX_LEVEL_ZERO;
	}
	else if ((p >= TEX_GLUE_BASE) && (p <= (TEX_LOCAL_BASE + 1)))
	{
		//Region 3
		equiv_field(mw) = zero_glue;
     	eq_type_field(mw) = (quint16)TEX_GLUE_REF;
     	eq_level_field(mw) = (quint16)TEX_LEVEL_ONE;
	}
	else if ((p >= TEX_PAR_SHAPE_LOC) && (p < TEX_TOKEN_BASE))
	{
		//Region 4
		equiv_field(mw) = TEX_NULL;
     	eq_type_field(mw) = (quint16)TEX_SHAPE_REF;
     	eq_level_field(mw) = (quint16)TEX_LEVEL_ONE;
	}
	else if ((p >= TEX_TOKEN_BASE) && (p <= (TEX_TOKS_BASE + TEX_BIGGEST_REG)))
	{
		equiv_field(mw) = TEX_NULL;
     	eq_type_field(mw) = (quint16)TEX_UNDEFINED_CS;
     	eq_level_field(mw) = (quint16)TEX_LEVEL_ZERO;
	}
	else if ((p >= TEX_BOX_BASE) && (p <= (TEX_BOX_BASE + TEX_BIGGEST_REG)))
	{
		equiv_field(mw) = TEX_NULL;
     	eq_type_field(mw) = (quint16)TEX_BOX_REF;
     	eq_level_field(mw) = (quint16)TEX_LEVEL_ONE;
	}
	else if ((p >= TEX_CUR_FONT_LOC) && (p <= (TEX_MATH_FONT_BASE + TEX_MATH_FONT_BIGGEST)))
	{
		equiv_field(mw) = TEX_NULL_FONT;
     	eq_type_field(mw) = (quint16)TEX_DATA;
     	eq_level_field(mw) = (quint16)TEX_LEVEL_ONE;
	}
	else if ((p >= TEX_CAT_CODE_BASE) && (p <= (TEX_CAT_CODE_BASE + TEX_BIGGEST_CHAR)))
	{
		equiv_field(mw) = TEX_OTHER_CHAR;
     	eq_type_field(mw) = (quint16)TEX_DATA;
     	eq_level_field(mw) = (quint16)TEX_LEVEL_ONE;
	}
	else if ((p >= TEX_LC_CODE_BASE) && (p <= (TEX_UC_CODE_BASE + TEX_BIGGEST_CHAR)))
	{
		equiv_field(mw) = 0;
     	eq_type_field(mw) = (quint16)TEX_DATA;
     	eq_level_field(mw) = (quint16)TEX_LEVEL_ONE;
	}
	else if ((p >= TEX_SF_CODE_BASE) && (p <= (TEX_SF_CODE_BASE + TEX_BIGGEST_CHAR)))
	{
		equiv_field(mw) = 1000;
        eq_type_field(mw) = (quint16)TEX_DATA;
     	eq_level_field(mw) = (quint16)TEX_LEVEL_ONE;
	}
	else if ((p >= TEX_MATH_CODE_BASE) && (p <= (TEX_MATH_CODE_BASE + TEX_BIGGEST_CHAR)))
	{
		equiv_field(mw) = p - TEX_MATH_CODE_BASE;
     	eq_type_field(mw) = (quint16)TEX_DATA;
     	eq_level_field(mw) = (quint16)TEX_LEVEL_ONE;
	}
	else if ((p >= TEX_INT_BASE) && (p <= (TEX_DEL_CODE_BASE - 1)))
	{
		//Region 5
		mw.ii.CINT0 = 0;
		mw.ii.CINT1 = 0;
	}
	else if ((p >= TEX_DEL_CODE_BASE) && (p <= (TEX_DEL_CODE_BASE + TEX_BIGGEST_CHAR)))
	{
		mw.ii.CINT0 = -1;
		mw.ii.CINT1 = -1;
	}
	else if ((p >= TEX_DIMEN_BASE) && (p <= TEX_EQTB_SIZE))
	{
		//Region 6
		mw.ii.CINT0 = 0;
		mw.ii.CINT1 = 0;
	}
	else
	{
		equiv_field(mw) = TEX_NULL;
     	eq_type_field(mw) = (quint16)TEX_UNDEFINED_CS;
     	eq_level_field(mw) = (quint16)TEX_LEVEL_ZERO;
	}
}

void XWTeX::saveForAfter(qint32 t)
{
	if (cur_level > TEX_LEVEL_ONE)
	{
		if (!checkFullSaveStack())
			return ;
			
		save_type(save_ptr) = (quint16)TEX_INSERT_TOKEN; 
		save_level(save_ptr) = (quint16)TEX_LEVEL_ZERO;
  		save_index(save_ptr) = t; 
  		save_ptr++;
	}
}

void XWTeX::unsave()
{
	qint16 l;
	qint32 p, t;
	bool a = false;
	if (cur_level > TEX_LEVEL_ONE)
	{
		cur_level--;
		while (true)
		{
			save_ptr--;
			if (save_type(save_ptr) == (quint16)TEX_LEVEL_BOUNDARY)
				goto done;
				
			p = save_index(save_ptr);
			if (save_type(save_ptr) == (quint16)TEX_INSERT_TOKEN)
			{
				t = cur_tok; 
				cur_tok = p;
				if (a)
				{
					p = getAvail(); 
					info(p) = cur_tok; 
					link(p) = loc; 
					loc = p; 
					texstart = p;
					if (cur_tok < TEX_RIGHT_BRACE_LIMIT)
					{
						if (cur_tok < TEX_LEFT_BRACE_LIMIT)
							align_state--;
						else
							align_state++;
					}
				}
				else
				{
					backInput(); 
					a = eTeX_ex;
				}
				
				cur_tok = t;
			}
			else if (save_type(save_ptr) == (quint16)TEX_RESTORE_SA)
			{
				saRestore(); 
				sa_chain = p; 
				sa_level = save_level(save_ptr);
			}
			else
			{
				if (save_type(save_ptr) == (quint16)TEX_RESTORE_OLD_VALUE)
				{
					l = save_level(save_ptr); 
					save_ptr--;
				}
				else
					save_stack[save_ptr] = newEqtb(TEX_UNDEFINED_CONTROL_SEQUENCE);
					
				if (p < TEX_INT_BASE)
				{
					if (eqLevel(p) == TEX_LEVEL_ONE)
					{
						eqDestroy(save_stack[save_ptr]);
#ifdef XW_TEX_STAT
						if (tracingRestores() > 0)
							restoreTrace(p, tr("retaining"));
#endif //XW_TEX_STAT
					}
					else
					{
						eqDestroy(newEqtb(p));
						setNewEqtb(p, save_stack[save_ptr]);
#ifdef XW_TEX_STAT
						if (tracingRestores() > 0)
							restoreTrace(p, tr("restoring"));
#endif //XW_TEX_STAT
					}
				}
				else if (xeqLevel(p) != TEX_LEVEL_ONE)
				{
					setNewEqtb(p, save_stack[save_ptr]); 
					setXEqLevel(p, l);
#ifdef XW_TEX_STAT
					if (tracingRestores() > 0)
						restoreTrace(p, tr("restoring"));
#endif //XW_TEX_STAT
				}
#ifdef XW_TEX_STAT
				else
				{
					if (tracingRestores() > 0)
						restoreTrace(p, tr("retaining"));
				}
#endif ////XW_TEX_STAT
			}
		}
		
done:
#ifdef XW_TEX_STAT
		if (tracingGroups() > 0)
			groupTrace(true);
#endif //XW_TEX_STAT

		if (grp_stack[in_open] == cur_boundary)
			groupWarning();
			
		cur_group = save_level(save_ptr); 
		cur_boundary = save_index(save_ptr);
		if (eTeX_ex)
			save_ptr--;
	}
	else
		confusion(tr("curlevel"));
}
