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

void XWTeX::buildPage()
{
	qint32 p, q, r, b, c, pi, n, delta, h, w;
	
	if ((link(contrib_head) == TEX_NULL) || output_active)
		return ;
		
	do
	{
		
tcontinue: 
		p = link(contrib_head);
		if (last_glue != TEX_MAX_HALFWORD)
			deleteGlueRef(last_glue);
			
		last_penalty = 0; 
		last_kern = 0;
		last_node_type = type(p) + 1;
		if (type(p) == (quint16)TEX_GLUE_NODE)
		{
			last_glue = glue_ptr(p); 
			add_glue_ref(last_glue);
		}
		else
		{
			last_glue = TEX_MAX_HALFWORD;
			if (type(p) == (quint16)TEX_PENALTY_NODE)
				last_penalty = penalty(p);
			else if (type(p) == (quint16)TEX_KERN_NODE)
				last_kern = width(p);
		}
		
		switch (type(p))
		{
			case TEX_HLIST_NODE:
			case TEX_VLIST_NODE:
			case TEX_RULE_NODE:
				if (page_contents < TEX_BOX_THERE)
				{
					if (page_contents == TEX_EMPTY)
						freezePageSpecs(TEX_BOX_THERE);
					else
						page_contents = TEX_BOX_THERE;
						
					q = newSkipParam(TEX_TOP_SKIP_CODE);
					if ((type(p) == (quint16)TEX_HLIST_NODE) && (is_mirrored(bodyDirection())))
					{
						if (width(temp_ptr) > depth(p))
							width(temp_ptr) = width(temp_ptr) - depth(p);
						else
							width(temp_ptr) = 0;
					}
					else
					{
						if (width(temp_ptr) > height(p))
							width(temp_ptr) = width(temp_ptr) - height(p);
						else
							width(temp_ptr) = 0;
					}
					
					link(q) = p; 
					link(contrib_head) = q; 
					goto tcontinue;
				}
				else
				{
					if ((type(p) == (quint16)TEX_HLIST_NODE) && (is_mirrored(bodyDirection())))
					{
						page_total = page_total + page_depth + depth(p);
  						page_depth = height(p);
					}
					else
					{
						page_total = page_total + page_depth + height(p);
  						page_depth = depth(p);
					}
					
					goto contribute;
				}
				break;
				
			case TEX_WHATSIT_NODE:
				goto contribute;
				break;
				
			case TEX_GLUE_NODE:
				if (page_contents < TEX_BOX_THERE)
					goto done1;
				else if (precedesBreak(page_tail))
					pi = 0;
				else
					goto update_heights;
				break;
				
			case TEX_KERN_NODE:
				if (page_contents < TEX_BOX_THERE)
					goto done1;
				else if (link(p) == TEX_NULL)
					return ;
				else if (type(link(p)) == (quint16)TEX_GLUE_NODE)
					pi = 0;
				else
					goto update_heights;
				break;
				
			case TEX_PENALTY_NODE:
				if (page_contents < TEX_BOX_THERE)
					goto done1;
				else
					pi = penalty(p);
				break;
				
			case TEX_MARK_NODE: 
				goto contribute;
				break;
				
			case TEX_INS_NODE:
				if (page_contents == TEX_EMPTY)
					freezePageSpecs(TEX_INSERTS_ONLY);
				n = subtype(p); 
				r = page_ins_head;
				while (n >= subtype(link(r)))
					r = link(r);
				if (subtype(r) != (quint16)n)
				{
					q = getNode(TEX_PAGE_INS_NODE_SIZE); 
					link(q) = link(r); 
					link(r) = q; 
					r = q;
					subtype(r) = (quint16)n; 
					type(r) = (quint16)TEX_INSERTING; 
					ensureVBox(n);
					if (box(n) == TEX_NULL)
						height(r) = 0;
					else
						height(r) = height(box(n)) + depth(box(n));
						
					best_ins_ptr(r) = TEX_NULL;
					q = skip(n);
					if (count(n) == 1000)
						h = height(r);
					else
						h = xOverN(height(r), 1000) * count(n);
						
					page_goal = page_goal - h - width(q);
					page_so_far[2 + stretch_order(q)] = page_so_far[2 + stretch_order(q)] + stretch(q);
					page_shrink = page_shrink + shrink(q);
					if ((shrink_order(q) != (quint16)TEX_NORMAL) && (shrink(q) != 0))
					{
						printErr(tr("Infinite glue shrinkage inserted from ")); 
						printEsc(TeXSkip);
						printInt(n);
  						help3(tr("The correction glue for page breaking with insertions"));
  						help_line[1] = tr("must have finite shrinkability. But you may proceed,");
  						help_line[0] = tr("since the offensive shrinkability has been made finite.");
  						error();
					}
				}
				if (type(r) == (quint16)TEX_SPLIT_UP)
					insert_penalties = insert_penalties + float_cost(p);
				else
				{
					last_ins_ptr(r) = p;
					delta = page_goal - page_total - page_depth + page_shrink;
					if (count(n) == 1000)
						h = height(p);
					else
						h = xOverN(height(p), 1000) * count(n);
						
					if (((h <= 0) || (h <= delta)) && ((height(p) + height(r)) <= dimen(n)))
					{
						page_goal = page_goal - h; 
						height(r) = height(r) + height(p);
					}
					else
					{
						if (count(n) <= 0)
							w = TEX_MAX_DIMEN;
						else
						{
							w = page_goal - page_total - page_depth;
							if (count(n) != 1000)
								w = xOverN(w, count(n)) * 1000;
						}
						
						if (w > (dimen(n) - height(r)))
							w = dimen(n) - height(r);
							
						q = vertBreak(ins_ptr(p), w, depth(p));
						height(r) = height(r) + best_height_plus_depth;
						
#ifdef XW_TEX_STAT

						if (tracingPages() > 0)
						{
							beginDiagnostic(); 
							printnl("% split"); 
							printInt(n);
							print(" to "); 
							printScaled(w);
							printChar(','); 
							printScaled(best_height_plus_depth);
							print(" p=");
							if (q == TEX_NULL)
								printInt(TEX_EJECT_PENALTY);
							else if (type(q) == (quint16)TEX_PENALTY_NODE)
								printInt(penalty(q));
							else
								printChar('0');
								
							endDiagnostic(false);
						}
						
#endif //XW_TEX_STAT
						if (count(n) != 1000)
							best_height_plus_depth = xOverN(best_height_plus_depth, 1000) * count(n);
							
						page_goal = page_goal - best_height_plus_depth;
						type(r) = (quint16)TEX_SPLIT_UP; 
						broken_ptr(r) = q; 
						broken_ins(r) = p;
						if (q == TEX_NULL)
							insert_penalties = insert_penalties + TEX_EJECT_PENALTY;
						else if (type(q) == (quint16)TEX_PENALTY_NODE)
							insert_penalties = insert_penalties + penalty(q);
					}
				}
				goto contribute;
				break;
				
			default:
				confusion("page");
				break;
		}
		
		if (pi < TEX_INF_PENALTY)
		{
			if (page_total < page_goal)
			{
				if ((page_so_far[3] != 0) || (page_so_far[4] != 0) || (page_so_far[5] != 0))
					b = 0;
				else
					b = badness(page_goal - page_total, page_so_far[2]);
			}
			else if ((page_total - page_goal) > page_shrink)
				b = TEX_AWFUL_BAD;
			else
				b = badness(page_total - page_goal, page_shrink);
				
			if (b < TEX_AWFUL_BAD)
			{
				if (pi <= TEX_EJECT_PENALTY)
					c = pi;
				else if (b < TEX_INF_BAD)
					c = b + pi + insert_penalties;
				else
					c = TEX_DEPLORABLE;
			}
			else
				c = b;
				
			if (insert_penalties >= 10000)
				c = TEX_AWFUL_BAD;
				
#ifdef XW_TEX_STAT
			
			if (tracingPages() > 0)
			{
				beginDiagnostic(); 
				printnl("%");
				print(" t="); 
				printTotals();
				print(" g="); 
				printScaled(page_goal);
				print(" b=");
				if (b == TEX_AWFUL_BAD)
					printChar('*');
				else
					printInt(b);
					
				print(" p="); 
				printInt(pi);
				print(" c=");
				if (c == TEX_AWFUL_BAD)
					printChar('*');
				else
					printInt(c);
					
				if (c <= least_page_cost)
					printChar('#');
					
				endDiagnostic(false);
			}
			
#endif //XW_TEX_STAT

			if (c <= least_page_cost)
			{
				best_page_break = p; 
				best_size = page_goal;
    			least_page_cost = c;
    			r = link(page_ins_head);
    			while (r != page_ins_head)
    			{
    				best_ins_ptr(r) = last_ins_ptr(r);
      				r = link(r);
    			}
			}
			
			if ((c == TEX_AWFUL_BAD) || (pi <= TEX_EJECT_PENALTY))
			{
				fireUp(p);
				if (output_active)
					return ;
					
				goto done;
			}
		}
		
		if ((type(p) < (quint16)TEX_GLUE_NODE) || (type(p) > (quint16)TEX_KERN_NODE))
			goto contribute;
			
update_heights:
		if (type(p) == (quint16)TEX_KERN_NODE)
			q= p;
		else
		{
			q = glue_ptr(p);
  			page_so_far[2 + stretch_order(q)] = page_so_far[2 + stretch_order(q)] + stretch(q);
  			page_shrink = page_shrink + shrink(q);
  			if ((shrink_order(q) != (quint16)TEX_NORMAL) && (shrink(q) != 0))
  			{
  				printErr(tr("Infinite glue shrinkage found on current page"));
  				help4(tr("The page about to be output contains some infinitely"));
  				help_line[2] = tr("shrinkable glue, e.g., `\\vss' or `\\vskip 0pt minus 1fil'.");
  				help_line[1] = tr("Such glue doesn't belong there; but you can safely proceed,");
  				help_line[0] = tr("since the offensive shrinkability has been made finite.");
  				error();
    			r = newSpec(q); 
    			shrink_order(r) = (quint16)TEX_NORMAL; 
    			deleteGlueRef(q);
    			glue_ptr(p) = r; 
    			q = r;
  			}
		}
		
		page_total = page_total + page_depth + width(q); 
		page_depth = 0;
		
contribute:
		if (page_depth > page_max_depth)
		{
			page_total = page_total + page_depth - page_max_depth;
  			page_depth = page_max_depth;
		}
		
		link(page_tail) = p; 
		page_tail = p;
		link(contrib_head) = link(p); 
		link(p) = TEX_NULL; 
		goto done;
		
done1:
		link(contrib_head) = link(p); 
		link(p) = TEX_NULL;
		if (savingVDiscards() > 0)
		{
			if (page_disc == TEX_NULL)
				page_disc = p;
			else
				link(tail_page_disc) = p;
				
			tail_page_disc = p;
		}
		else
			flushNodeList(p);
			
done:
		;
	} while (link(contrib_head) != TEX_NULL);
	
	if (nest_ptr == 0)
		tail = contrib_head;
	else
		contrib_tail = contrib_head;
}

void XWTeX::ensureVBox(qint32 n)
{
	qint32 p = box(n);
	if (p != TEX_NULL)
	{
		if (type(p) == (quint16)TEX_HLIST_NODE)
		{
			printErr(tr("Insertions can only be added to a vbox"));
			help3(tr("Tut tut: You're trying to \\insert into a"));
			help_line[1] = tr("\\box register that now contains an \\hbox.");
			help_line[0] = tr("Proceed, and I'll discard its present contents.");
			boxError(n);
		}
	}
}

void XWTeX::fireUp(qint32 c)
{
	qint32 p, q, r, s, prev_p, n, save_vbadness, save_vfuzz, save_split_top_skip;
	bool wait;
	
	if (type(best_page_break) == (quint16)TEX_PENALTY_NODE)
	{
		geqWordDefine(TEX_INT_BASE + TEX_OUTPUT_PENALTY_CODE, penalty(best_page_break));
  		penalty(best_page_break) = TEX_INF_PENALTY;
	}
	else
		geqWordDefine(TEX_INT_BASE + TEX_OUTPUT_PENALTY_CODE, TEX_INF_PENALTY);
		
	if (sa_mark != TEX_NULL)
	{
		if (doMarks(TEX_FIRE_UP_INIT, 0, sa_mark))
			sa_mark = TEX_NULL;
	}
	
	if (bot_mark != TEX_NULL)
	{
		if (top_mark != TEX_NULL)
			deleteTokenRef(top_mark);
			
		top_mark = bot_mark; 
		add_token_ref(top_mark);
  		deleteTokenRef(first_mark); 
  		first_mark = TEX_NULL;
	}
	
	if (c == best_page_break)
		best_page_break = TEX_NULL;
		
	if (box(255) != TEX_NULL)
	{
		printErr(""); 
		printEsc(TeXBox); 
		print(tr("255 is not void"));
		help2(tr("You shouldn't use \\box255 except in \\output routines."));
		help_line[0] = tr("Proceed, and I'll discard its present contents.");
		boxError(255);
	}
	
	insert_penalties = 0;
	save_split_top_skip = splitTopSkip();
	if (holdingInserts() <= 0)
	{
		r = link(page_ins_head);
		while (r != page_ins_head)
		{
			if (best_ins_ptr(r) != TEX_NULL)
			{
				n = subtype(r); 
				ensureVBox(n);
				if (box(n) == TEX_NULL)
					setEquiv(TEX_BOX_BASE + n,newNullBox());
					
				p = box(n) + TEX_LIST_OFFSET;
				while (link(p) != TEX_NULL)
					p = link(p);
					
				last_ins_ptr(r) = p;
			}
			
			r = link(r);
		}
	}
	
	q = hold_head; 
	link(q) = TEX_NULL; 
	prev_p  = page_head; 
	p = link(prev_p);
	while (p != best_page_break)
	{
		if (type(p) == (quint16)TEX_INS_NODE)
		{
			if (holdingInserts() <= 0)
			{
				r = link(page_ins_head);
				while (subtype(r) != subtype(p))
					r = link(r);
					
				if (best_ins_ptr(r) == TEX_NULL)
					wait = true;
				else
				{
					wait = false; 
					s = last_ins_ptr(r); 
					link(s) = ins_ptr(p);
					if (best_ins_ptr(r) == p)
					{
						if (type(r) == (quint16)TEX_SPLIT_UP)
						{
							if ((broken_ins(r) == p) && (broken_ptr(r) != TEX_NULL))
							{
								while (link(s) != broken_ptr(r))
									s = link(s);
									
								link(s) = TEX_NULL;
								setEquiv(TEX_GLUE_BASE + TEX_SPLIT_TOP_SKIP_CODE, split_top_ptr(p));
    							ins_ptr(p) = prunePageTop(broken_ptr(r), false);
    							if (ins_ptr(p) != TEX_NULL)
    							{
    								temp_ptr = vpack(ins_ptr(p), 0, TEX_ADDITIONAL);
      								height(p) = height(temp_ptr) + depth(temp_ptr);
      								freeNode(temp_ptr, TEX_BOX_NODE_SIZE); 
      								wait = true;
    							}
							}
						}
						
						best_ins_ptr(r) = TEX_NULL;
						n = subtype(r);
						temp_ptr = list_ptr(box(n));
						freeNode(box(n), TEX_BOX_NODE_SIZE);
						pack_direction = bodyDirection();
						setEquiv(TEX_BOX_BASE + n, vpack(temp_ptr, 0, TEX_ADDITIONAL));
					}
					else
					{
						while (link(s) != TEX_NULL)
							s = link(s);
							
						last_ins_ptr(r) = s;
					}
				}
				
				link(prev_p) = link(p); 
				link(p) = TEX_NULL;
				if (wait)
				{
					link(q) = p; 
					q = p; 
					insert_penalties++;
				}
				else
				{
					deleteGlueRef(split_top_ptr(p));
  					freeNode(p, TEX_INS_NODE_SIZE);
				}
				
				p = prev_p;
			}
		}
		else if (type(p) == (quint16)TEX_MARK_NODE)
		{
			if (mark_class(p) != 0)
			{
				findSaElement(TEX_MARK_VAL, mark_class(p), true);
				if (sa_first_mark(cur_ptr) == TEX_NULL)
				{
					sa_first_mark(cur_ptr) = mark_ptr(p);
  					add_token_ref(mark_ptr(p));
				}
				
				if (sa_bot_mark(cur_ptr) != TEX_NULL)
					deleteTokenRef(sa_bot_mark(cur_ptr));
					
				sa_bot_mark(cur_ptr) = mark_ptr(p); 
				add_token_ref(mark_ptr(p));
			}
			else
			{
				if (first_mark == TEX_NULL)
				{
					first_mark = mark_ptr(p);
  					add_token_ref(first_mark);
				}
				
				if (bot_mark != TEX_NULL)
					deleteTokenRef(bot_mark);
					
				bot_mark = mark_ptr(p); 
				add_token_ref(bot_mark);
			}
		}
		
		prev_p = p; 
		p = link(prev_p);
	}
	
	setEquiv(TEX_GLUE_BASE + TEX_SPLIT_TOP_SKIP_CODE, save_split_top_skip);
	if (p != TEX_NULL)
	{
		if (link(contrib_head) == TEX_NULL)
		{
			if (nest_ptr == 0)
				tail = page_tail;
			else
				contrib_tail = page_tail;
				
			link(page_tail) = link(contrib_head);
  			link(contrib_head) = p;
  			link(prev_p) = TEX_NULL;
		}
	}
	
	save_vbadness = vbadness();
	setNewEqtbInt(TEX_INT_BASE + TEX_VBADNESS_CODE, TEX_INF_BAD);
	save_vfuzz = vfuzz();
	setNewEqtbSC(TEX_DIMEN_BASE + TEX_VFUZZ_CODE, TEX_MAX_DIMEN);
	pack_direction = bodyDirection();
	setEquiv(TEX_BOX_BASE + 255, vpackage(link(page_head), best_size, TEX_EXACTLY, page_max_depth));
	setNewEqtbInt(TEX_INT_BASE + TEX_VBADNESS_CODE, save_vbadness);
	setNewEqtbSC(TEX_DIMEN_BASE + TEX_VFUZZ_CODE, save_vfuzz);
	if (last_glue != TEX_MAX_HALFWORD)
		deleteGlueRef(last_glue);
		
	page_contents = TEX_EMPTY; 
	page_tail = page_head; 
	link(page_head) = TEX_NULL;
	last_glue = TEX_MAX_HALFWORD; 
	last_penalty = 0; 
	last_kern = 0;
	last_node_type = -1;
	page_depth = 0; 
	page_max_depth = 0;
	
	if (q != hold_head)
	{
		link(page_head) = link(hold_head); 
		page_tail = q;
	}
	
	r = link(page_ins_head);
	while (r != page_ins_head)
	{
		q = link(r); 
		freeNode(r, TEX_PAGE_INS_NODE_SIZE); 
		r = q;
	}
	
	link(page_ins_head) = page_ins_head;
	
	if (sa_mark != TEX_NULL)
	{
		if (doMarks(TEX_FIRE_UP_DONE, 0, sa_mark))
			sa_mark = TEX_NULL;
	}
	
	if ((top_mark != TEX_NULL) && (first_mark == TEX_NULL))
	{
		first_mark = top_mark; 
		add_token_ref(top_mark);
	}
	
	if (outputRoutine() != TEX_NULL)
	{
		if (dead_cycles >= maxDeadCycles())
		{
			printErr(tr("Output loop---")); 
			printInt(dead_cycles);
			print(tr(" consecutive dead cycles"));
			help3(tr("I've concluded that your \\output is awry; it never does a"));
			help_line[1] = tr("\\shipout, so I'm shipping \\box255 out myself. Next time");
			help_line[0] = tr("increase \\maxdeadcycles if you want me to be more patient!");
			error();
		}
		else
		{
			output_active = true;
			dead_cycles++;
			pushNest(); 
			mode = -TEX_VMODE; 
			prev_depth = TEX_IGNORE_DEPTH; 
			mode_line = -line;
			beginTokenList(outputRoutine(), TEX_OUTPUT_TEXT);
			newSaveLevel(TEX_OUTPUT_GROUP); 
			normalParagraph();
			scanLeftBrace();
			return;
		}
	}
	
	if (link(page_head) != TEX_NULL)
	{
		if (link(contrib_head) == TEX_NULL)
		{
			if (nest_ptr == 0)
				tail = page_tail;
			else
				contrib_tail = page_tail;
		}
		else
			link(page_tail) = link(contrib_head);
			
		link(contrib_head) = link(page_head);
  		link(page_head) = TEX_NULL; 
  		page_tail = page_head;
	}
	
	flushNodeList(page_disc); 
	page_disc = TEX_NULL;
	shipOut(box(255)); 
	setEquiv(TEX_BOX_BASE + 255, TEX_NULL);
}

void XWTeX::freezePageSpecs(qint32 s)
{
	page_contents = s;
	page_goal = vsize(); 
	page_max_depth = maxDepth();
	page_depth = 0; 
	page_so_far[1] = 0;
	page_so_far[2] = 0;
	page_so_far[3] = 0;
	page_so_far[4] = 0;
	page_so_far[5] = 0;
	page_so_far[6] = 0;
	page_so_far[7] = 0;
	least_page_cost = TEX_AWFUL_BAD;
	
#ifdef XW_TEX_STAT

	if (tracingPages() > 0)
	{
		beginDiagnostic();
  		printnl("%% goal height="); 
  		printScaled(page_goal);
  		print(", max depth="); 
  		printScaled(page_max_depth);
  		endDiagnostic(false);
	}

#endif //XW_TEX_STAT
}

qint32 XWTeX::prunePageTop(qint32 p, bool s)
{
	qint32 prev_p, q, r;
	
	prev_p = temp_head; 
	link(temp_head) = p;
	while (p != TEX_NULL)
	{
		switch (type(p))
		{
			case TEX_HLIST_NODE:
			case TEX_VLIST_NODE:
			case TEX_RULE_NODE:
				q = newSkipParam(TEX_SPLIT_TOP_SKIP_CODE); 
				link(prev_p) = q; 
				link(q) = p;
				if (width(temp_ptr) > height(p))
					width(temp_ptr) = width(temp_ptr)-height(p);
				else
					width(temp_ptr) = 0;
				p = TEX_NULL;
				break;
				
			case TEX_WHATSIT_NODE:
			case TEX_MARK_NODE:
			case TEX_INS_NODE:
				prev_p = p; 
				p = link(prev_p);
				break;
				
			case TEX_GLUE_NODE:
			case TEX_KERN_NODE:
			case TEX_PENALTY_NODE:
				q = p; 
				p = link(q); 
				link(q) = TEX_NULL;
    			link(prev_p) = p;
    			if (s)
    			{
    				if (split_disc == TEX_NULL)
    					split_disc = q;
    				else
    					link(r) = q;
    					
    				r = q;
    			}
    			else
    				flushNodeList(q);
				break;
				
			default:
				confusion("pruning");
				break;
		}
	}
	
	return link(temp_head);
}

qint32 XWTeX::vertBreak(qint32 p, qint32 h, qint32 d)
{
	qint32 prev_p, q, r, pi, b, least_cost, best_place, prev_dp, t;
	
	prev_p = p;
	least_cost = TEX_AWFUL_BAD;
	active_height[1] = 0;
	active_height[2] = 0;
	active_height[3] = 0;
	active_height[4] = 0;
	active_height[5] = 0;
	active_height[6] = 0;
	active_height[7] = 0;
	prev_dp = 0;
	
	while (true)
	{
		if (p == TEX_NULL)
			pi = TEX_EJECT_PENALTY;
		else
		{
			switch (type(p))
			{
				case TEX_HLIST_NODE:
				case TEX_VLIST_NODE:
				case TEX_RULE_NODE:
					cur_height = cur_height + prev_dp + height(p); 
					prev_dp = depth(p);
  					goto not_found;
					break;
					
				case TEX_WHATSIT_NODE:
					goto not_found;
					break;
					
				case TEX_GLUE_NODE:
					if (precedesBreak(prev_p))
						pi = 0;
					else
						goto update_heights;
					break;
					
				case TEX_KERN_NODE:
					if (link(p) == TEX_NULL)
						t = TEX_PENALTY_NODE;
					else
						t = type(link(p));
					if (t == TEX_GLUE_NODE)
						pi = 0;
					else
						goto update_heights;
					break;
					
				case TEX_PENALTY_NODE: 
					pi = penalty(p);
					break;
					
				case TEX_MARK_NODE:
				case TEX_INS_NODE: 
					goto not_found;
					break;
					
				default:
					confusion("vertbreak");
					break;
			}
		}
		
		if (pi < TEX_INF_PENALTY)
		{
			if (cur_height < h)
			{
				if ((active_height[3] != 0) || 
					(active_height[4] != 0) || 
					(active_height[5] != 0) || 
					(active_height[6] != 0))
				{
					b = 0;
				}
				else
					b = badness(h - cur_height, active_height[2]);
			}
			else if ((cur_height - h) > active_height[7])
				b = TEX_AWFUL_BAD;
			else
				b = badness(cur_height - h, active_height[7]);
				
			if (b < TEX_AWFUL_BAD)
			{
				if (pi <= TEX_EJECT_PENALTY)
					b = pi;
				else if (b < TEX_INF_BAD)
					b = b + pi;
				else
					b = TEX_DEPLORABLE;
			}
			
			if (b <= least_cost)
			{
				best_place = p; 
				least_cost = b;
				best_height_plus_depth = cur_height + prev_dp;
			}
			
			if ((b == TEX_AWFUL_BAD) || (pi <= TEX_EJECT_PENALTY))
				goto done;
		}
		
		if ((type(p) < (quint16)TEX_GLUE_NODE) || (type(p) > (quint16)TEX_KERN_NODE))
			goto not_found;
			
update_heights:
		if (type(p) == (quint16)TEX_KERN_NODE)
			q = p;
		else
		{
			q = glue_ptr(p);
			active_height[2 + stretch_order(q)] = active_height[2 + stretch_order(q)] + stretch(q);
			active_height[7] = active_height[7] + shrink(q);
			if ((shrink_order(q) != TEX_NORMAL) && (shrink(q) != 0))
			{
				printErr(tr("Infinite glue shrinkage found in box being split"));
				help4(tr("The box you are \\vsplitting contains some infinitely"));
				help_line[2] = tr("shrinkable glue, e.g., `\\vss' or `\\vskip 0pt minus 1fil'.");
				help_line[1] = tr("Such glue doesn't belong there; but you can safely proceed,");
				help_line[0] = tr("since the offensive shrinkability has been made finite.");
				error(); 
				r = newSpec(q); 
				shrink_order(r) = (quint16)TEX_NORMAL; 
				deleteGlueRef(q);
    			glue_ptr(p) = r; 
    			q = r;
			}
		}
		
		cur_height = cur_height + prev_dp+width(q); 
		prev_dp = 0;
		
not_found:
		if (prev_dp > d)
		{
			cur_height = cur_height + prev_dp - d;
    		prev_dp = d;
		}
		
		prev_p = p; 
		p = link(prev_p);
	}
	
done: 
	return best_place;
}

qint32 XWTeX::vsplit(qint32 n, qint32 h)
{
	qint32 v, vdir, p, q;
	cur_val = n; 
	if (cur_val < TEX_NUMBER_REGS)
		v = box(cur_val);
	else
	{
		findSaElement(TEX_BOX_VAL, cur_val, false);
		if (cur_ptr == TEX_NULL)
			v = TEX_NULL;
		else
			v = sa_ptr(cur_ptr);
	}
	vdir = box_dir(v);
	flushNodeList(split_disc); 
	split_disc = TEX_NULL;
	if (sa_mark != TEX_NULL)
	{
		if (doMarks(TEX_VSPLIT_INIT, 0, sa_mark))
			sa_mark = TEX_NULL;
	}
	
	if (split_first_mark != TEX_NULL)
	{
		deleteTokenRef(split_first_mark); 
		split_first_mark = TEX_NULL;
  		deleteTokenRef(split_bot_mark); 
  		split_bot_mark = TEX_NULL;
	}
	
	if (v == TEX_NULL)
		return TEX_NULL;
		
	if (type(v) != (quint16)TEX_VLIST_NODE)
	{
		printErr(""); 
		printEsc(TeXVSplit); 
		print(tr(" needs a "));
  		printEsc(TeXVBox);
  		help2(tr("The box you are trying to split is an \\hbox."));
  		help_line[0] = tr("I can't split such a box, so I'll leave it alone.");
  		error(); 
  		return TEX_NULL;
	}
	
	q = vertBreak(list_ptr(v), h, splitMaxDepth());
	p = list_ptr(v);
	if (p == q)
		list_ptr(v) = TEX_NULL;
	else
	{
		while (true)
		{
			if (type(p) == (quint16)TEX_MARK_NODE)
			{
				if (mark_class(p) != 0)
				{
					findSaElement(TEX_MARK_VAL, mark_class(p), true);
					if (sa_split_first_mark(cur_ptr) == TEX_NULL)
					{
						sa_split_first_mark(cur_ptr) = mark_ptr(p);
  						add_token_ref(mark_ptr(p));
					}
					else
						deleteTokenRef(sa_split_bot_mark(cur_ptr));
						
					sa_split_bot_mark(cur_ptr) = mark_ptr(p);
					add_token_ref(mark_ptr(p));
				}
				else if (split_first_mark == TEX_NULL)
				{
					split_first_mark = mark_ptr(p);
      				split_bot_mark = split_first_mark;
      				token_ref_count(split_first_mark) = token_ref_count(split_first_mark) + 2;
				}
				else
				{
					deleteTokenRef(split_bot_mark);
      				split_bot_mark = mark_ptr(p);
      				add_token_ref(split_bot_mark);
				}
			}
			
			if (link(p) == q)
			{
				link(p) = TEX_NULL; 
				goto done;
			}
			
			p = link(p);
		}
done:
		;
	}
	
	q = prunePageTop(q, savingVDiscards() > 0);
	p = list_ptr(v); 
	freeNode(v, TEX_BOX_NODE_SIZE);
	pack_direction = vdir;
	if (q != TEX_NULL)
		q = vpack(q, 0, TEX_ADDITIONAL);
		
	setEquiv(TEX_BOX_BASE + cur_val, q);
	return vpackage(p, h, TEX_EXACTLY, splitMaxDepth());
}

