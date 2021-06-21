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

qint32 XWTeX::compressTrie(qint32 p)
{
	if (p == 0)
		return 0;
		
	trie_l[p] = compressTrie(trie_l[p]);
  	trie_r[p] = compressTrie(trie_r[p]);
  	return trieNode(p);
}

void XWTeX::firstFit(qint32 p)
{
	qint32 h, z, q, c, l, r, ll;
	
	c = trie_c[p];
	z = trie_min[c];
	while (true)
	{
		h = z - c;
		if (trie_max < (h + max_hyph_char))
		{
			if (trie_size <= (h + max_hyph_char))
			{
				overFlow(tr("pattern memory"), trie_size);
				return ;
			}
			
			do
			{
				trie_max++; 
				trie_taken[trie_max] = false;
  				trie_link(trie_max) = trie_max + 1; 
  				trie_back(trie_max) = trie_max - 1;
			} while (trie_max != (h + max_hyph_char));
		}
		
		if (trie_taken[h])
			goto not_found;
			
		q = trie_r[p];
		while (q > 0)
		{
			if (trie_link(h + trie_c[q]) == 0)
				goto not_found;
				
			q = trie_r[q];
		}
		
		goto found;
		
not_found: 
		z = trie_link(z);
	}
	
found: 
	trie_taken[h] = true; 
	trie_ref[p] = h; 
	q = p;
	do
	{
		z = h +trie_c[q]; 
		l = trie_back(z); 
		r = trie_link(z);
		trie_back(r) = l; 
		trie_link(l) = r; 
		trie_link(z) = 0;
		if (l < max_hyph_char)
		{
			if (z < max_hyph_char)
				ll = z;
			else
				ll = max_hyph_char;
				
			do
			{
				trie_min[l] = r; 
				l++;
			} while (l != ll);
		}
		
		q = trie_r[q];
	} while (q != 0);
}

void XWTeX::hyphenate()
{
	qint32 i, j, l, q, r, s, bchar, h, k, u;
	qint32 major_tail, minor_tail, c, c_loc, r_count, hyf_node, z, v;
	
	for (j = 0; j <= hn; j++)
		hyf[j] = 0;
		
	h = hc[1]; 
	hn++; 
	hc[hn] = cur_lang;
	for (j = 2; j <= hn; j++)
		h = (h + h + hc[j]) % TEX_HYPH_PRIME;
		
	while (true)
	{
		k = hyph_word[h];
		if (k == 0)
			goto not_found;
			
		if (length(k) == hn)
		{
			j = 1; 
			u = str_start(k);
			do
			{
				if (str_pool[u] != hc[j])
					goto done;
					
				j++; 
				u++;
			} while (j <= hn);
			
			s = hyph_list[h];
			while (s != TEX_NULL)
			{
				hyf[info(s)] = 1; 
				s = link(s);
			}
			
			hn--;
			goto found;
		}
done:
		h = hyph_link[h];
		if (h == 0)
			goto not_found;
			
		h--;
	}
	
not_found: 
	hn--;
	
	if (trie_char(cur_lang + 1) != (quint16)cur_lang)
		return ;
		
	hc[0] = 0; 
	hc[hn + 1] = 0; 
	hc[hn + 2] = max_hyph_char;
	
	for (j = 0; j <= (hn-r_hyf + 1); j++)
	{
		z = trie_link(cur_lang + 1) + hc[j]; 
		l = j;
		while (hc[l] == trie_char(z))
		{
			if (trie_op(z) != (quint16)TEX_MIN_QUARTERWORD)
			{
				v = trie_op(z);
				do
				{
					v = v + op_start[cur_lang]; 
					i = l - hyf_distance[v];
					if (hyf_num[v] > hyf[i])
						hyf[i] = hyf_num[v];
					v = hyf_next[v];
				} while (v != TEX_MIN_QUARTERWORD);
			}
			
			l++; 
			z = trie_link(z) + hc[l];
		}
	}
	
found:
	for (j = 0; j < l_hyf; j++)
		hyf[j] = 0;
		
	for (j = 0; j < r_hyf; j++)
		hyf[hn - j] = 0;
		
	for (j = l_hyf; j <= (hn - r_hyf); j++)
	{
		if (odd(hyf[j]))
			goto found1;
	}
	
	return ;
	
found1:
	q = link(hb); 
	link(hb) = TEX_NULL; 
	r = link(ha); 
	link(ha) = TEX_NULL; 
	bchar = hyf_bchar;
	if (isCharNode(ha))
	{
		if (font(ha) != (quint16)hf)
			goto found2;
		else
		{
			init_list = ha; 
			init_lig  = false; 
			hu[0] = character(ha);
		}
	}
	else if (type(ha) == (quint16)TEX_LIGATURE_NODE)
	{
		if (font(lig_char(ha)) != (quint16)hf)
			goto found2;
		else
		{
			init_list = lig_ptr(ha); 
			init_lig  = true; 
			init_lft = (subtype(ha) > (quint16)1);
			hu[0] = character(lig_char(ha));
			if (init_list == TEX_NULL)
			{
				if (init_lft)
				{
					hu[0] = max_hyph_char; 
					init_lig = false;
				}
			}
			
			freeNode(ha, TEX_SMALL_NODE_SIZE);
		}
	}
	else
	{
		if (!isCharNode(r))
		{
			if (type(r) == (quint16)TEX_LIGATURE_NODE)
			{
				if (subtype(r) > (quint16)1)
					goto found2;
			}
		}
		
		j = 1; 
		s = ha; 
		init_list = TEX_NULL; 
		goto common_ending;
	}
	
	s = cur_p;
	while (link(s) != ha)
		s = link(s);
		
	j = 0; 
	goto common_ending;
	
found2: 
	s = ha; 
	j = 0; 
	hu[0] = max_hyph_char; 
	init_lig = false; 
	init_list = TEX_NULL;
	
common_ending: 
	flushNodeList(r);
	do
	{
		l = j; 
		j = reconstitute(j, hn, bchar, hyf_char) + 1;
		if (hyphen_passed == 0)
		{
			link(s) = link(hold_head);
			while (link(s) > TEX_NULL)
				s = link(s);
				
			if (odd(hyf[j - 1]))
			{
				l = j; 
				hyphen_passed = j - 1; 
				link(hold_head) = TEX_NULL;
			}
		}
		
		if (hyphen_passed > 0)
		{
			do
			{
				r = getNode(TEX_SMALL_NODE_SIZE);
				link(r) = link(hold_head); 
				type(r) = (quint16)TEX_DISC_NODE;
				major_tail = r; 
				r_count = 0;
				while (link(major_tail) > TEX_NULL)
				{
					major_tail = link(major_tail); 
					r_count++;
				}
				
				i = hyphen_passed; 
				hyf[i] = 0;
				minor_tail = TEX_NULL; 
				pre_break(r) = TEX_NULL; 
				hyf_node = newCharacter(hf, hyf_char);
				if (hyf_node != TEX_NULL)
				{
					i++; 
					c = hu[i]; 
					hu[i] = hyf_char; 
					freeAvail(hyf_node);
				}
				
				while (l <= i)
				{
					l = reconstitute(l, i, font_bchar(hf), TEX_NON_CHAR) + 1;
					if (link(hold_head) > TEX_NULL)
					{
						if (minor_tail == TEX_NULL)
							pre_break(r) = link(hold_head);
						else
							link(minor_tail) = link(hold_head);
							
						minor_tail = link(hold_head);
						while (link(minor_tail) > TEX_NULL)
							minor_tail = link(minor_tail);
					}
				}
				
				if (hyf_node != TEX_NULL)
				{
					hu[i] = c;
					l = i; 
					i--;
				}
				
				minor_tail = TEX_NULL; 
				post_break(r) = TEX_NULL; 
				c_loc = 0;
				if (bchar_label(hf) != TEX_NON_ADDRESS)
				{
					l--; 
					c = hu[l]; 
					c_loc = l; 
					hu[l] = max_hyph_char;
				}
				while (l < j)
				{
					do
					{
						l = reconstitute(l, hn, bchar, TEX_NON_CHAR) + 1;
						if (c_loc > 0)
						{
							hu[c_loc] = c; 
							c_loc = 0;
						}
						
						if (link(hold_head) > TEX_NULL)
						{
							if (minor_tail == TEX_NULL)
								post_break(r) = link(hold_head);
							else
								link(minor_tail) = link(hold_head);
								
							minor_tail = link(hold_head);
							while (link(minor_tail) > TEX_NULL)
								minor_tail = link(minor_tail);
								
							while (l > j)
							{
								j = reconstitute(j, hn, bchar, TEX_NON_CHAR) + 1;
								link(major_tail) = link(hold_head);
								while (link(major_tail) > TEX_NULL)
								{
									major_tail = link(major_tail); 
									r_count++;
								}
							}
						}
					} while (l < j);
				}
				
				if (r_count > 127)
				{
					link(s) = link(r); 
					link(r) = TEX_NULL; 
					flushNodeList(r);
				}
				else
				{
					link(s) = r; 
					replace_count(r) = (quint16)r_count;
				}
				
				s = major_tail;
				
				hyphen_passed = j - 1; 
				link(hold_head) = TEX_NULL;
			} while (odd(hyf[j - 1]));
		}
	} while (j <= hn);
	
	link(s) = q;
	flushList(init_list);
}

void XWTeX::initTrie()
{
	qint32 p, j, k, s, t, r;
	TeXTwoHalves h;
		
	max_hyph_char++;
	
	op_start[0] = -TEX_MIN_QUARTERWORD;
	for (j = 1; j <= TEX_BIGGEST_LANG; j++)
		op_start[j] = op_start[j - 1] + trie_used[j - 1];
		
	for (j = 1; j <= trie_op_ptr; j++)
		trie_op_hash[j] = op_start[trie_op_lang[j]] + trie_op_val[j];
		
	for (j = 1; j <= trie_op_ptr; j++)
	{
		while (trie_op_hash[j] > j)
		{
			k = trie_op_hash[j];
			t = hyf_distance[k]; 
			hyf_distance[k] = hyf_distance[j]; 
			hyf_distance[j] = t;
			t = hyf_num[k]; 
			hyf_num[k] = hyf_num[j]; 
			hyf_num[j] = t;
			t = hyf_next[k]; 
			hyf_next[k] = hyf_next[j]; 
			hyf_next[j] = t;
			trie_op_hash[j] = trie_op_hash[k]; 
			trie_op_hash[k] = k;
		}
	}
	
	for (p = 0; p <= trie_size; p++)
		trie_hash[p] = 0;
		
	hyph_root = compressTrie(hyph_root);
	trie_root = compressTrie(trie_root);
	
	for (p = 0; p <= trie_ptr; p++)
		trie_ref[p] = 0;
		
	for (p = 0; p <= TEX_BIGGEST_CHAR; p++)
		trie_min[p] = p + 1;
		
	trie_link(0) = 1; 
	trie_max = 0;
	
	if (trie_root != 0)
	{
		firstFit(trie_root); 
		triePack(trie_root);
	}
	
	if (hyph_root != 0)
	{
		if (trie_root == 0)
		{
			for (p = 0; p <= 255; p++)
				trie_min[p] = p + 2;
		}
		
		firstFit(hyph_root); 
		triePack(hyph_root);
		hyph_start = trie_ref[hyph_root];
	}
	
	h.v.RH = 0; 
	h.u.B0 = TEX_MIN_QUARTERWORD; 
	h.u.B1 = TEX_MIN_QUARTERWORD;
	if (trie_max == 0)
	{
		for (r = 0; r <= max_hyph_char; r++)
			trie[r] = h;
			
		trie_max = max_hyph_char;
	}
	else
	{
		if (hyph_root > 0)
			trieFix(hyph_root);
			
		if (trie_root > 0)
			trieFix(trie_root);
			
		r = 0;
		do
		{
			s = trie_link(r); 
			trie[r] = h; 
			r = s;
		} while (r <= trie_max);
	}
	
	trie_char(0) = (quint16)'?';
	
	trie_not_ready = false;
}

void XWTeX::newHyphExceptions()
{
	qint32 n, j, h, k, p, q, s, u, v;
	
	scanLeftBrace();
	
	if (language() <= 0)
		cur_lang = 0;
	else if (language() > TEX_BIGGEST_LANG)
		cur_lang = 0;
	else 
		cur_lang = language();
		
	if (ini_version)
	{
		if (trie_not_ready)
		{
			hyph_index = 0; 
			goto not_found1;
		}
	}
	
	if (trie_char(hyph_start + cur_lang) != (quint16)cur_lang)
		hyph_index = 0;
	else
		hyph_index = trie_link(hyph_start + cur_lang);
	
not_found1:
	n = 0; 
	p = TEX_NULL;
	while (true)
	{
		getXToken();
		
reswitch:
		switch (cur_cmd)
		{
			case TEX_LETTER:
			case TEX_OTHER_CHAR:
			case TEX_CHAR_GIVEN:
				if (cur_chr == '-')
				{
					if (n < 63)
					{
						q = getAvail(); 
						link(q) = p; 
						info(q) = n; 
						p = q;
					}
				}
				else
				{
					if (hyph_index == 0)
						hc[0] = lcCode(cur_chr);
					else if (trie_char(hyph_index + cur_chr) != (quint16)cur_chr)
						hc[0] = 0;
					else
						hc[0] = trie_op(hyph_index + cur_chr);
						
					if (hc[0] == 0)
					{
						printErr(tr("Not a letter"));
						help2(tr("Letters in \\hyphenation words must have \\lccode>0."));
						help_line[0] = tr("Proceed; I'll ignore the character I just read.");
						error();
					}
					else if (n < 63)
					{
						n++; 
						hc[n] = hc[0];
					}
				}
				break;
				
			case TEX_CHAR_NUM:
				scanCharNum(); 
				cur_chr = cur_val; 
				cur_cmd = TEX_CHAR_GIVEN;
    			goto reswitch;
				break;
				
			case TEX_SPACER:
			case TEX_RIGHT_BRACE:
				if (n > 1)
				{
					n++; 
					hc[n] = cur_lang; 
					strRoom(n); 
					h = 0;
					for (j = 1; j <= n; j++)
					{
						h =(h + h + hc[j]) % TEX_HYPH_PRIME;
  						appendChar(hc[j]);
					}
					
					s = makeString();
					if (hyph_next <= TEX_HYPH_PRIME)
					{
						while ((hyph_next > 0) && (hyph_word[hyph_next - 1] > 0))
							hyph_next--;
					}
					if ((hyph_count == hyph_size) || (hyph_next == 0))
					{
						overFlow(tr("exception dictionary"), hyph_size);
						return ;
					}
					hyph_count++;
					while (hyph_word[h] != 0)
					{
						k = hyph_word[h];
						if (length(k) != length(s))
							goto not_found;
							
						u = str_start(k); 
						v = str_start(s);
						do
						{
							if (str_pool[u] != str_pool[v])
								goto not_found;
								
							u++; 
							v++;
						} while (u != str_start(k + 1));
						
						flushString(); 
						s = hyph_word[h];
						hyph_count--;
						goto found;
						
not_found:
						if (hyph_link[h] == 0)
						{
							hyph_link[h] = hyph_next;
							if (hyph_next >= hyph_size)
								hyph_next = TEX_HYPH_PRIME;
								
							if (hyph_next > TEX_HYPH_PRIME)
								hyph_next++;
						}
						
						h = hyph_link[h] - 1;
					}
					
found:
					
					hyph_word[h] = s; 
					hyph_list[h] = p;
				}
				if (cur_cmd == TEX_RIGHT_BRACE)
					return ;
				n = 0; 
				p = TEX_NULL;
				break;
				
			default:
				printErr(tr("Improper ")); 
				printEsc(TeXHyphenation);
				print(tr(" will be flushed"));
				help2(tr("Hyphenation exceptions must contain only letters"));
				help_line[0] = tr("and hyphens. But continue; I'll forgive and forget.");
				error();
				break;
		}
	}
}

void XWTeX::newPatterns()
{
	qint32 k, l, v, p, q, c;
	bool digit_sensed, first_child;
	
	if (trie_not_ready)
	{
		if (language() <= 0)
			cur_lang = 0;
		else if (language() > TEX_BIGGEST_LANG)
			cur_lang = 0;
		else 
			cur_lang = language();
			
		scanLeftBrace();
		
		k = 0; 
		hyf[0] = 0; 
		digit_sensed = false;
		
		while (true)
		{
			getXToken();
			switch (cur_cmd)
			{
				case TEX_LETTER:
				case TEX_OTHER_CHAR:
					if (digit_sensed || (cur_chr < '0') || (cur_chr > '9'))
					{
						if (cur_chr == '.')
							cur_chr = 0;
						else
						{
							cur_chr = lcCode(cur_chr);
							if (cur_chr == 0)
							{
								printErr(tr("Nonletter"));
								help1(tr("(See Appendix H.)")); 
								error();
							}
						}
						
						if (cur_chr > max_hyph_char)
							max_hyph_char = cur_chr;
							
						if (k < 63)
						{
							k++; 
							hc[k]  = cur_chr; 
							hyf[k] = 0; 
							digit_sensed = false;
						}
					}
					else if (k < 63)
					{
						hyf[k] = cur_chr - '0'; 
						digit_sensed = true;
					}
					break;
					
				case TEX_SPACER:
				case TEX_RIGHT_BRACE:
					if (k > 0)
					{
						if (hc[1] == 0)
							hyf[0] = 0;
							
						if (hc[k] == 0)
							hyf[k] = 0;
							
						l = k; 
						v = TEX_MIN_QUARTERWORD;
						while (true)
						{
							if (hyf[l] != 0)
								v = newTrieOp(k - l, hyf[l], v);
								
							if (l > 0)
								l--;
							else
								goto done1;
						}
						
done1:
						q = 0; 
						hc[0] = cur_lang;
						while (l <= k)
						{
							c = hc[l]; 
							l++; 
							p = trie_l[q]; 
							first_child = true;
							while ((p > 0) && (c > trie_c[p]))
							{
								q = p; 
								p = trie_r[q]; 
								first_child = false;
							}
							
							if ((p == 0) || (c < trie_c[p]))
							{
								if (trie_ptr == trie_size)
								{
									overFlow(tr("pattern memory"), trie_size);
									return ;
								}
								
								trie_ptr++; 
								trie_r[trie_ptr] = p; 
								p = trie_ptr; 
								trie_l[p] = 0;
								if (first_child)
									trie_l[q] = p;
								else
									trie_r[q] = p;
									
								trie_c[p] = c; 
								trie_o[p] = TEX_MIN_QUARTERWORD;
							}
							
							q = p;
						}
						
						if (trie_o[q] != TEX_MIN_QUARTERWORD)
						{
							printErr(tr("Duplicate pattern"));
							help1(tr("(See Appendix H.)")); 
							error();
						}
						
						trie_o[q] = v;
					}
					if (cur_cmd == TEX_RIGHT_BRACE)
						goto done;
					k = 0; 
					hyf[0] = 0; 
					digit_sensed = false;
					break;
					
				default:
					printErr(tr("Bad ")); 
					printEsc(TeXPatterns);
					help1(tr("(See Appendix H.)")); 
					error();
					break;
			}
		}
		
done:
		if (savingHyphCodes() > 0)
		{
			c = cur_lang; 
			first_child = false; 
			p = 0;
			do
			{
				q = p; 
				p = trie_r[q];
			} while (!(p == 0) || (c <= trie_c[p]));
			
			if ((p == 0) || (c < trie_c[p]))
			{
				if (trie_ptr == trie_size)
				{
					overFlow(tr("pattern memory"), trie_size);
					return ;
				}
				
				trie_ptr++; 
				trie_r[trie_ptr] = p; 
				p = trie_ptr; 
				trie_l[p] = 0;
				if (first_child)
					trie_l[q] = p;
				else
					trie_r[q] = p;
					
				trie_c[p] = c; 
				trie_o[p] = TEX_MIN_QUARTERWORD;
			}
			
			q = p;
			p = trie_l[q]; 
			first_child = true;
			for (c = 0; c <= 255; c++)
			{
				if ((lcCode(c) > 0) || ((c == 255) && first_child))
				{
					if (p == 0)
					{
						if (trie_ptr == trie_size)
						{
							overFlow(tr("pattern memory"), trie_size);
							return ;
						}
				
						trie_ptr++; 
						trie_r[trie_ptr] = p; 
						p = trie_ptr; 
						trie_l[p] = 0;
						if (first_child)
							trie_l[q] = p;
						else
							trie_r[q] = p;
					
						trie_c[p] = c; 
						trie_o[p] = TEX_MIN_QUARTERWORD;
					}
					else
						trie_c[p] = c;
						
					trie_o[p] = lcCode(c);
    				q = p; 
    				p = trie_r[q]; 
    				first_child = false;
				}
			}
			
			if (first_child)
				trie_l[q] = 0;
			else
				trie_r[q] = 0;
		}
	}
	else
	{
		printErr(tr("Too late for ")); 
		printEsc(TeXPatterns);
		
		help1(tr("All patterns must be given before typesetting begins."));
  		error(); 
  		link(garbage) = scanToks(false, false); 
  		flushList(def_ref);
	}
}

qint32 XWTeX::newTrieOp(qint32 d, qint32 n, qint32 v)
{
	qint32 h, u, l;
	h = qAbs(n + 313 * d + 361 * v + 1009 * cur_lang) % (trie_op_size - neg_trie_op_size) + neg_trie_op_size;
	while (true)
	{
		l = trie_op_hash[h];
		if (l == 0)
		{
			if (trie_op_ptr == trie_op_size)
			{
				overFlow(tr("pattern memory ops"), trie_op_size);
				return TEX_NULL;
			}
			
			u = trie_used[cur_lang];
			if (u == TEX_MAX_QUARTERWORD)
			{
				overFlow(tr("pattern memory ops per language"), TEX_MAX_QUARTERWORD - TEX_MIN_QUARTERWORD);
				return TEX_NULL;
			}
			
			trie_op_ptr++; 
			u++; 
			trie_used[cur_lang] = u;
    		hyf_distance[trie_op_ptr] = d;
    		hyf_num[trie_op_ptr] = n; 
    		hyf_next[trie_op_ptr] = v;
    		trie_op_lang[trie_op_ptr] = cur_lang; 
    		trie_op_hash[h] = trie_op_ptr;
    		trie_op_val[trie_op_ptr] = u; 
    		return u;
		}
		
		if ((hyf_distance[l] == d) && 
			(hyf_num[l] == n) && 
			(hyf_next[l] == v) && 
			(trie_op_lang[l] == cur_lang))
		{
			return trie_op_val[l];
		}
		
		if (h > -trie_op_size)
			h--;
		else
			h = trie_op_size;
	}
	
	return TEX_NULL;
}

qint32 XWTeX::reconstitute(qint32 j, 
	                       qint32 n, 
	                       qint32 bchar, 
	                       qint32 hchar)
{
	qint32 p, t, cur_rh, test_char, w, k;
	TeXFourQuarters q;
	
	hyphen_passed = 0; 
	t = hold_head; 
	w = 0; 
	link(hold_head) = TEX_NULL;
	
	cur_l = hu[j]; 
	cur_q = t;
	if (j == 0)
	{
		ligature_present = init_lig; 
		p = init_list;
		if (ligature_present)
			lft_hit = init_lft;
			
		while (p > TEX_NULL)
		{
			link(t) = getAvail(); 
			t = link(t);
    		font(t) = (quint16)hf; 
    		character(t) = (quint16)character(p);
    		p = link(p);
		}
	}
	else if (cur_l < TEX_NON_CHAR)
	{
		link(t) = getAvail(); 
		t = link(t);
    	font(t) = (quint16)hf; 
    	character(t) = (quint16)cur_l;
	}
	
	lig_stack = TEX_NULL;
	if (j < n)
		cur_r = hu[j + 1];
	else
		cur_r = bchar;
		
	if (odd(hyf[j]))
		cur_rh = hchar;
	else
		cur_rh = TEX_NON_CHAR;
		
tcontinue:
	if (cur_l == TEX_NON_CHAR)
	{
		k = bchar_label(hf);
		if (k == TEX_NON_ADDRESS)
			goto done;
		else
			q = font_info(hf, k).qqqq;
	}
	else 
	{
		q = char_info(hf, cur_l);
		if (char_tag(q) != TEX_LIG_TAG)
			goto done;
			
		k = lig_kern_start(hf, q); 
		q = font_info(hf, k).qqqq;
		if (skip_byte(q) > TEX_STOP_FLAG)
		{
			k = lig_kern_restart(hf, q); 
			q = font_info(hf, k).qqqq;
		}
	}
	
	if (cur_rh < TEX_NON_CHAR)
		test_char = cur_rh;
	else
		test_char = cur_r;
		
	while (true)
	{
		if ((next_char(hf, q)) == test_char)
		{
			if (skip_byte(q) <= TEX_STOP_FLAG)
			{
				if (cur_rh < TEX_NON_CHAR)
				{
					hyphen_passed = j; 
					hchar = TEX_NON_CHAR; 
					cur_rh = TEX_NON_CHAR;
      				goto tcontinue;
				}
				else
				{
					if (hchar < TEX_NON_CHAR)
					{
						if (odd(hyf[j]))
						{
							hyphen_passed = j; 
							hchar = TEX_NON_CHAR;
						}
					}
					
					if (op_byte(q) < (quint16)TEX_KERN_FLAG)
					{
						if (cur_l == TEX_NON_CHAR)
							lft_hit = true;
							
						if (j == n)
						{
							if (lig_stack == TEX_NULL)
								rt_hit = true;
						}
						
						checkInterrupt();
						switch (op_byte(q))
						{
							case 1:
							case 5:
								cur_l = rem_byte(q);
								ligature_present = true;
								break;
								
							case 2:
							case 6:
								cur_r = rem_byte(q);
								if (lig_stack > TEX_NULL)
									character(lig_stack) = (quint16)cur_r;
								else
								{
									lig_stack = newLigItem(cur_r);
									if (j == n)
										bchar = TEX_NON_CHAR;
									else
									{
										p = getAvail(); 
										lig_ptr(lig_stack) = p;
      									character(p) = (quint16)(hu[j+1]); 
      									font(p) = (quint16)hf;
									}
								}
								break;
								
							case 3:
								cur_r = rem_byte(q);
								p = lig_stack; 
								lig_stack = newLigItem(cur_r); 
								link(lig_stack) = p;
								break;
								
							case 7:
							case 11:
								if (ligature_present)
								{
									p = newLigature(hf, cur_l, link(cur_q));
									if (lft_hit)
									{
										subtype(p) = (quint16)2; 
										lft_hit = false;
									}
									
									if (false)
									{
										if (lig_stack == TEX_NULL)
										{
											(subtype(p))++; 
											rt_hit = false;
										}
									}
									
									link(cur_q) = p; 
									t = p; 
									ligature_present = false;
								}
								
								cur_q = t; 
								cur_l = rem_byte(q); 
								ligature_present = true;
								break;
								
							default:
								cur_l =rem_byte(q); 
								ligature_present = true;
								if (lig_stack > TEX_NULL)
								{
									if (lig_ptr(lig_stack) > TEX_NULL)
									{
										link(t) = lig_ptr(lig_stack);
										t = link(t); 
										j++;
									}
									
									p = lig_stack; 
									lig_stack = link(p); 
									freeNode(p, TEX_SMALL_NODE_SIZE);
									if (lig_stack == TEX_NULL)
									{
										if (j < n)
											cur_r = hu[j + 1];
										else
											cur_r = bchar;
											
										if (odd(hyf[j]))
											cur_rh = hchar;
										else
											cur_rh = TEX_NON_CHAR;
									}
									else
										cur_r = character(lig_stack);
								}
								else if (j == n)
									goto done;
								else
								{
									link(t) = getAvail(); 
									t = link(t);
    								font(t) = (quint16)hf; 
    								character(t) = (quint16)cur_r;
    								j++;
    								if (j < n)
    									cur_r = hu[j + 1];
    								else
    									cur_r = bchar;
    									
    								if (odd(hyf[j]))
										cur_rh = hchar;
									else
										cur_rh = TEX_NON_CHAR;
								}
								break;
						}
						
						if (op_byte(q) > (quint16)4)
						{
							if (op_byte(q) != (quint16)7)
								goto done;
						}
						
						goto tcontinue;
					}
					
					w = char_kern(hf, q); 
					goto done;
				}
			}
		}
		
		if (skip_byte(q) >= TEX_STOP_FLAG)
		{
			if (cur_rh == TEX_NON_CHAR)
				goto done;
			else
			{
				cur_rh = TEX_NON_CHAR; 
				goto tcontinue;
			}
		}
		
		k =k + skip_byte(q) + 1; 
		q = font_info(hf, k).qqqq;
	}
	
done:
	if (ligature_present)
	{
		p = newLigature(hf, cur_l, link(cur_q));
		if (lft_hit)
		{
			subtype(p) = (quint16)2; 
			lft_hit = false;
		}
		
		if (rt_hit)
		{
			if (lig_stack == TEX_NULL)
			{
				(subtype(p))++; 
				rt_hit = false;
			}
		}
		
		link(cur_q) = p; 
		t = p; 
		ligature_present = false;
	}
	
	if (w != 0)
	{
		link(t) = newKern(w); 
		t = link(t); 
		w = 0;
	}
	
	if (lig_stack > TEX_NULL)
	{
		cur_q = t; 
		cur_l = character(lig_stack); 
		ligature_present = true;
		if (lig_ptr(lig_stack) > TEX_NULL)
		{
			link(t) = lig_ptr(lig_stack);
			t = link(t); 
			j++;
		}
		
		p = lig_stack; 
		lig_stack = link(p); 
		freeNode(p, TEX_SMALL_NODE_SIZE);
		if (lig_stack == TEX_NULL)
		{
			if (j < n)
    			cur_r = hu[j + 1];
    		else
    			cur_r = bchar;
    									
    		if (odd(hyf[j]))
				cur_rh = hchar;
			else
				cur_rh = TEX_NON_CHAR;
		}
		else
			cur_r = character(lig_stack);
			
		goto tcontinue;
	}
	
	return j;
}

void XWTeX::trieFix(qint32 p)
{
	qint32 q, z, c;
	
	z = trie_ref[p];
	do
	{
		q = trie_l[p]; 
		c = trie_c[p];
		trie_link(z + c) = trie_ref[q]; 
		trie_char(z + c) = (quint16)c; 
		trie_op(z + c) = (quint16)trie_o[p];
		if (q > 0)
			trieFix(q);
			
		p = trie_r[p];
	} while (p != 0);
}

qint32 XWTeX::trieNode(qint32 p)
{
	qint32 h, q;
	
	h = qAbs(trie_c[p] + 1009 * trie_o[p] + 2718 * trie_l[p] + 3142 * trie_r[p]) % trie_size;
	while (true)
	{
		q = trie_hash[h];
		if (q == 0)
		{
			trie_hash[h] = p; 
			return p;
		}
		
		if ((trie_c[q] == trie_c[p]) && 
			(trie_o[q] == trie_o[p]) && 
			(trie_l[q] == trie_l[p]) && 
			(trie_r[q] == trie_r[p]))
		{
			return q;
		}
		
		if (h > 0)
			h--;
		else
			h = trie_size;
	}
	
	return 0;
}

void XWTeX::triePack(qint32 p)
{
	qint32 q;
	do
	{
		q = trie_l[p];
		if ((q > 0) && (trie_ref[q] == 0))
		{
			firstFit(q); 
			triePack(q);
		}
		p = trie_r[p];
	} while (p != 0);
}

