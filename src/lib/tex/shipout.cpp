/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QProcess>
#include <QProcessEnvironment>
#include <QTextStream>
#include "XWApplication.h"
#include "XWUtil.h"
#include "XWConst.h"
#include "XWTeXIODev.h"
#include "XWTeX.h"

void XWTeX::dviFontDef(qint32 f)
{
	oval = f - TEX_FONT_BASE - 1; 
	ocmd = DVI_FNTDEF1;
	dev->outCmd(ocmd, oval);
	dev->dviOut(font_check_0(f));
	dev->dviOut(font_check_1(f));
	dev->dviOut(font_check_2(f));
	dev->dviOut(font_check_3(f));
	dev->dviFour(font_size(f));
	dev->dviFour(font_dsize(f));
	dev->dviOut(length(font_area(f)));
	dev->dviOut(length(font_name(f)));
	for (qint32 k = str_start(font_area(f)); k < str_start(font_area(f) + 1); k++)
		dev->dviOut((uchar)(str_pool[k]));
	
	for (qint32 k = str_start(font_name(f)); k < str_start(font_name(f) + 1); k++)
		dev->dviOut((uchar)(str_pool[k]));
}

void XWTeX::hlistOut()
{
	qint32 base_line, c_wd, c_ht, c_dp, c_htdp;
	TeXFourQuarters c_info;
	qint32 edge_v, edge_h, effective_horizontal, basepoint_horizontal, basepoint_vertical;
	qint32 saving_h, saving_v, save_direction, dir_tmp, dir_ptr, dvi_dir_h, dvi_dir_ptr, dvi_temp_ptr;
	qint32 left_edge, save_h, save_v, this_box, g_order, g_sign;
	qint32 p, save_loc, leader_box, leader_wd, lx;
	bool outer_doing_leaders;
	qint32 edge, cur_g, g;
	double glue_temp, cur_glue;
	
	cur_g = 0; 
	cur_glue = 0.0;
	this_box = temp_ptr; 
	g_order = glue_order(this_box);
	g_sign = glue_sign(this_box); 
	p = list_ptr(this_box);
	save_direction = dvi_direction;
	dvi_direction = box_dir(this_box);
	dir_ptr = TEX_NULL; 
	dir_tmp = newDir(dvi_direction);
	link(dir_tmp) = dir_ptr; 
	dir_ptr = dir_tmp;
	dir_dvi_ptr(dir_ptr) = dev->dvi_ptr;
	saving_h = dvi_h; 
	saving_v = dvi_v;
	cur_s++;
	if (cur_s > 0)
		dev->dviPush();
		
	if (cur_s > max_push)
		max_push = cur_s;
		
	save_loc  = dev->dvi_offset + dev->dvi_ptr; 
	base_line = cur_v; 
	left_edge = cur_h;
	while (p != TEX_NULL)
	{
reswitch:
		if (isCharNode(p))
		{
			if (cur_h != dvi_h)
			{
				movement(cur_h - dvi_h, DVI_RIGHT1); 
				dvi_h = cur_h;
			}
			
			if (cur_v != dvi_v)
			{
				movement(cur_v - dvi_v, DVI_DOWN1); 
				dvi_v = cur_v;
			}
			
			do
			{
				fnt = font(p); 
				ch  = character(p);
				if (fnt != dvi_f)
				{
					if (!font_used(fnt))
					{
						dviFontDef(fnt); 
						font_used(fnt) = true;
					}
					
					oval = fnt - TEX_FONT_BASE - 1; 
					ocmd = DVI_FNT1; 
					dev->dviFont(ocmd, oval);
					dvi_f = fnt;
				}
				
				c_info = char_info(fnt, ch);
  				c_htdp = height_depth(c_info);
  				
  				if (is_rotated(dvi_direction))
  				{
  					c_ht = char_width(fnt, c_info) / 2;
    				c_wd = char_height(fnt, c_htdp) + char_depth(fnt, c_htdp);
  				}
  				else
  				{
  					c_ht = char_height(fnt, c_htdp);
    				c_dp = char_depth(fnt, c_htdp);
    				c_wd = char_width(fnt, c_info);
  				}
  				
  				cur_h = cur_h + c_wd;
  				if ((font_natural_dir(fnt) != -1))
  				{
  					switch (font_direction(dvi_direction))
  					{
  						case TEX_DIR__LT:
  						case TEX_DIR__LB:
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_SET1;
  							dev->dviSet(p, ocmd, oval);
  							break;
  							
  						case TEX_DIR__RT:
  						case TEX_DIR__RB:
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
      						dev->dviLeft(c_wd);
  							break;
  							
  						case TEX_DIR__TL:
  						case TEX_DIR__TR:
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
      						dev->dviDown(c_wd);
  							break;
  							
  						case TEX_DIR__BL:
  						case TEX_DIR__BR:
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							dev->dviUp(c_wd);
  							break;
  							
  						case TEX_DIR__LL:
  						case TEX_DIR__LR:
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							dev->dviRight(c_wd);
  							break;
  							
  						case TEX_DIR__RL:
  						case TEX_DIR__RR:
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							dev->dviLeft(c_wd);
  							break;
  							
  						case TEX_DIR__TT:
  						case TEX_DIR__TB:
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							dev->dviDown(c_wd);
  							break;
  							
  						case TEX_DIR__BT:
  						case TEX_DIR__BB:
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							dev->dviUp(c_wd);
  							break;
  							
  						default:
  							break;
  					}
  				}
  				else
  				{
  					switch (font_direction(dvi_direction))
  					{
  						case TEX_DIR__LT:
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_SET1;
  							dev->dviSet(p, ocmd, oval);
  							break;
  							
  						case TEX_DIR__LB:
  							dev->dviDown(c_ht);
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_SET1;
  							dev->dviSet(p, ocmd, oval);
  							dev->dviUp(c_ht);
  							break;
  							
  						case TEX_DIR__RT:
  							dev->dviLeft(c_wd);
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							break;
  							
  						case TEX_DIR__RB:
  							dev->dviLeft(c_wd);
      						dev->dviDown(c_ht);
      						oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							dev->dviUp(c_ht);
  							break;
  							
  						case TEX_DIR__TL:
  							dev->dviDown(c_wd);
      						dev->dviLeft(c_ht);
      						oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							dev->dviRight(c_ht);
  							break;
  							
  						case TEX_DIR__TR:
  							dev->dviDown(c_wd);
      						dev->dviLeft(c_dp);
      						oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							dev->dviRight(c_dp);
  							break;
  							
  						case TEX_DIR__BL:
  							dev->dviLeft(c_ht);
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							dev->dviRight(c_ht);
      						dev->dviUp(c_wd);
  							break;
  							
  						case TEX_DIR__BR:
  							dev->dviLeft(c_dp);
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							dev->dviRight(c_dp);
      						dev->dviUp(c_wd);
  							break;
  							
  						case TEX_DIR__LL:
  						case TEX_DIR__LR:
  							dev->dviDown(c_ht);
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							dev->dviUp(c_ht);
      						dev->dviRight(c_wd);
  							break;
  							
  						case TEX_DIR__RL:
  						case TEX_DIR__RR:
  							dev->dviLeft(c_wd);
      						dev->dviDown(c_ht);
      						oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							dev->dviUp(c_ht);
  							break;
  							
  						case TEX_DIR__TT:
  						case TEX_DIR__TB:
  							dev->dviDown(c_wd);
      						dev->dviLeft(c_ht);
      						oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							dev->dviRight(c_ht);
  							break;
  							
  						case TEX_DIR__BT:
  						case TEX_DIR__BB:
  							dev->dviLeft(c_ht);
  							oval = ch - font_offset(fnt); 
  							ocmd = DVI_PUT1;
  							dev->dviPut(p, ocmd, oval);
  							dev->dviRight(c_ht);
      						dev->dviUp(c_wd);
  							break;
  							
  						default:
  							break;
  					}
  				}
  				
  				p = link(p);
  				
			} while (isCharNode(p));
			
			dvi_h = cur_h;
		}
		else
		{
			switch (type(p))
			{
				case TEX_HLIST_NODE:
				case TEX_VLIST_NODE:
					if (!(dir_orthogonal(dir_primary[box_dir(p)], dir_primary[dvi_direction])))
					{
						effective_horizontal = width(p);
  						basepoint_vertical = 0;
  						if (dir_opposite(dir_secondary[box_dir(p)], dir_secondary[dvi_direction]))
  							basepoint_horizontal = width(p);
  						else
  							basepoint_horizontal = 0;
					}
					else
					{
						effective_horizontal = height(p) + depth(p);
						if (!(is_mirrored(box_dir(p))))
						{
							if (dir_eq(dir_primary[box_dir(p)], dir_secondary[dvi_direction]))
								basepoint_horizontal = height(p);
							else
								basepoint_horizontal = depth(p);
						}
						else
						{
							if (dir_eq(dir_primary[box_dir(p)], dir_secondary[dvi_direction]))
								basepoint_horizontal = depth(p);
							else
								basepoint_horizontal = height(p);
						}
						
						if (dir_eq(dir_secondary[box_dir(p)], dir_primary[dvi_direction]))
							basepoint_vertical = -(width(p) / 2);
						else
							basepoint_vertical = (width(p) / 2);
					}
					
					if (!(is_mirrored(dvi_direction)))
						basepoint_vertical  = basepoint_vertical + shift_amount(p);
					else
						basepoint_vertical = basepoint_vertical - shift_amount(p);
					if (list_ptr(p) == TEX_NULL)
						cur_h = cur_h + effective_horizontal;
					else
					{
						temp_ptr = p; 
						edge  = cur_h; 
						cur_h = cur_h + basepoint_horizontal;
  						edge_v = cur_v; 
  						cur_v  = base_line + basepoint_vertical;
  						if (cur_h != dvi_h)
						{
							movement(cur_h - dvi_h, DVI_RIGHT1); 
							dvi_h = cur_h;
						}
			
						if (cur_v != dvi_v)
						{
							movement(cur_v - dvi_v, DVI_DOWN1); 
							dvi_v = cur_v;
						}
						
						save_h = dvi_h; 
						save_v = dvi_v;
						if (type(p) == (quint16)TEX_VLIST_NODE)
							vlistOut();
						else
							hlistOut();
							
						dvi_h = save_h; 
						dvi_v = save_v;
  						cur_h = edge + effective_horizontal; 
  						cur_v = base_line;
					}
					break;
					
				case TEX_RULE_NODE:
					if (!(dir_orthogonal(dir_primary[rule_dir(p)], dir_primary[dvi_direction])))
					{
						rule_ht = height(p); 
						rule_dp = depth(p); 
						rule_wd = width(p);
					}
					else
					{
						rule_ht = width(p) / 2;
    					rule_dp = width(p) / 2;
    					rule_wd = height(p) + depth(p);
					}
					goto fin_rule;
					break;
					
				case TEX_WHATSIT_NODE:
					if (subtype(p) != (quint16)TEX_DIR_NODE)
						outWhat(p);
					else
					{
						if (dir_dir(p) >= 0)
						{
							if (cur_h != dvi_h)
							{
								movement(cur_h - dvi_h, DVI_RIGHT1); 
								dvi_h = cur_h;
							}
			
							if (cur_v != dvi_v)
							{
								movement(cur_v - dvi_v, DVI_DOWN1); 
								dvi_v = cur_v;
							}
							
							dir_tmp = getNode(TEX_DIR_NODE_SIZE);
							type(dir_tmp) = (quint16)TEX_WHATSIT_NODE; 
							subtype(dir_tmp) = (quint16)TEX_DIR_NODE;
							dir_dir(dir_tmp) = dir_dir(p);
							dir_level(dir_tmp) = dir_level(p);
							dir_dvi_h(dir_tmp) = dir_dvi_h(p);
							dir_dvi_ptr(dir_tmp) = dir_dvi_ptr(p);
							link(dir_tmp) = dir_ptr; 
							dir_ptr = dir_tmp;
							if (dir_opposite(dir_secondary[dir_dir(dir_ptr)], dir_secondary[dvi_direction]))
							{
								dir_dvi_ptr(dir_ptr) = dev->dvi_ptr;
    							dir_dvi_h(dir_ptr) = dvi_h;
								if ((dir_secondary[dvi_direction] == 0) || 
									(dir_secondary[dvi_direction] == 2))
								{
									dev->dviOut(DVI_DOWN4);
								}
								else
									dev->dviOut(DVI_RIGHT4);
									
								dir_dvi_ptr(dir_ptr) = dev->dvi_ptr;
    							dir_dvi_h(dir_ptr) = dvi_h;
    							dev->dviFour(0);
								cur_h = 0; 
								dvi_h = 0;
							}
							else
							{
								dir_dvi_ptr(dir_ptr) = dev->dvi_ptr;
    							dir_dvi_h(dir_ptr) = dvi_h;
							}
							
							dvi_direction = dir_dir(dir_ptr);
						}
						else
						{
							dvi_dir_h = dir_dvi_h(dir_ptr);
  							dvi_dir_ptr = dir_dvi_ptr(dir_ptr);
  							dir_tmp = dir_ptr;
							dir_ptr = link(dir_tmp);
							freeNode(dir_tmp, TEX_DIR_NODE_SIZE);
  							if (cur_h != dvi_h)
							{
								movement(cur_h - dvi_h, DVI_RIGHT1); 
								dvi_h = cur_h;
							}
			
							if (cur_v != dvi_v)
							{
								movement(cur_v - dvi_v, DVI_DOWN1); 
								dvi_v = cur_v;
							}
							
							dvi_direction = dir_dir(dir_ptr);
							if (dir_opposite(dir_secondary[dir_dir(p) + 64], dir_secondary[dvi_direction]))
							{
								dvi_temp_ptr = dev->dvi_ptr;
    							dev->dvi_ptr = dvi_dir_ptr;
    							if ((dir_secondary[dvi_direction] == 0)  || 
       								(dir_secondary[dvi_direction] == 1))
       							{
       								dev->dviFour(dvi_h);
       							}
       							else
       								dev->dviFour(-dvi_h);
       								
       							dev->dvi_ptr = dvi_temp_ptr;
       							if ((dir_secondary[dvi_direction] == 0) || 
       								(dir_secondary[dvi_direction] ==2))
       								dev->dviOut(DVI_DOWN4);
       							else
       								dev->dviOut(DVI_RIGHT4);
       								
       							
       							if ((dir_secondary[dvi_direction] == 0) || 
       								(dir_secondary[dvi_direction] == 1))
       								dev->dviOut(dvi_h);
       							else
       								dev->dviOut(-dvi_h);
       							
							}
							
							cur_h = dvi_dir_h + dvi_h; 
							dvi_h = cur_h;
						}
					}
					break;
					
				case TEX_GLUE_NODE:
					g = glue_ptr(p); 
					rule_wd = width(g) - cur_g;
					if (g_sign != TEX_NORMAL)
					{
						if (g_sign == TEX_STRETCHING)
						{
							if (stretch_order(g) == (quint16)g_order)
							{
								cur_glue = cur_glue + stretch(g);
								glue_temp = glue_set(this_box) * cur_glue;
								if (glue_temp > TEX_BILLION)
									glue_temp = TEX_BILLION;
								else if (glue_temp < -TEX_BILLION)
									glue_temp = -TEX_BILLION;
								cur_g = (qint32)(qRound(glue_temp));
							}
						}
						else if (shrink_order(g) == (quint16)g_order)
						{
							cur_glue = cur_glue - shrink(g);
							glue_temp = glue_set(this_box) * cur_glue;
							if (glue_temp > TEX_BILLION)
								glue_temp = TEX_BILLION;
							else if (glue_temp < -TEX_BILLION)
								glue_temp = -TEX_BILLION;
							cur_g = (qint32)(qRound(glue_temp));
						}
					}
					
					rule_wd = rule_wd + cur_g;
					if (subtype(p) >= (quint16)TEX_A_LEADERS)
					{
						leader_box = leader_ptr(p);
						if (type(leader_box) == (quint16)TEX_RULE_NODE)
						{
							rule_ht = height(leader_box); 
							rule_dp = depth(leader_box);
  							goto fin_rule;
						}
						
						if (!(dir_orthogonal(dir_primary[box_dir(leader_box)], dir_primary[dvi_direction])))
							leader_wd = width(leader_box);
						else
							leader_wd = height(leader_box) + depth(leader_box);
							
						if ((leader_wd > 0) && (rule_wd > 0))
						{
							rule_wd = rule_wd + 10;
							edge = cur_h + rule_wd; 
							lx = 0;
							if (subtype(p) == (quint16)TEX_A_LEADERS)
							{
								save_h = cur_h;
  								cur_h = left_edge + leader_wd *((cur_h - left_edge) / leader_wd);
  								if (cur_h < save_h)
  									cur_h = cur_h + leader_wd;
							}
							else
							{
								lq = rule_wd / leader_wd;
								lr = rule_wd % leader_wd;
								if (subtype(p) == (quint16)TEX_C_LEADERS)
									cur_h = cur_h + (lr / 2);
								else
								{
									lx = lr / (lq+1);
    								cur_h = cur_h +((lr -(lq - 1) * lx) / 2);
								}
							}
							
							while ((cur_h + leader_wd) <= edge)
							{
								if (!(dir_orthogonal(dir_primary[box_dir(leader_box)], dir_primary[dvi_direction])))
								{
									basepoint_vertical = 0;
									if (dir_opposite(dir_secondary[box_dir(leader_box)], dir_secondary[dvi_direction]))
										basepoint_horizontal = width(leader_box);
									else
										basepoint_horizontal = 0;
								}
								else
								{
									if (!(is_mirrored(box_dir(leader_box))))
									{
										if (dir_eq(dir_primary[box_dir(leader_box)], dir_secondary[dvi_direction]))
											basepoint_horizontal = height(leader_box);
										else
											basepoint_horizontal = depth(leader_box);
									}
									else
									{
										if (dir_eq(dir_primary[box_dir(leader_box)], dir_secondary[dvi_direction]))
											basepoint_horizontal = depth(leader_box);
										else
											basepoint_horizontal = height(leader_box);
									}
									
									if (dir_eq(dir_secondary[box_dir(leader_box)], dir_primary[dvi_direction]))
										basepoint_vertical = -(width(leader_box) / 2);
									else
										basepoint_vertical = (width(leader_box) / 2);
								}
								
								if (!(is_mirrored(dvi_direction)))
									basepoint_vertical = basepoint_vertical + shift_amount(leader_box);
								else
									basepoint_vertical = basepoint_vertical - shift_amount(leader_box);
									
								temp_ptr = leader_box;
								edge_h = cur_h; 
								cur_h  = cur_h + basepoint_horizontal;
								edge_v = cur_v; 
								cur_v  = base_line + basepoint_vertical;
								if (cur_h != dvi_h)
								{
									movement(cur_h - dvi_h, DVI_RIGHT1); 
									dvi_h = cur_h;
								}
			
								if (cur_v != dvi_v)
								{
									movement(cur_v - dvi_v, DVI_DOWN1); 
									dvi_v = cur_v;
								}
								
								save_h = dvi_h; 
								save_v = dvi_v;
								outer_doing_leaders = doing_leaders; 
								doing_leaders = true;
								if (type(leader_box) == (quint16)TEX_VLIST_NODE)
									vlistOut();
								else
									hlistOut();
									
								doing_leaders = outer_doing_leaders;
								dvi_h = save_h; 
								dvi_v = save_v;
								cur_h = edge_h + leader_wd+lx; 
								cur_v = base_line;
							}
							
							cur_h = edge - 10; 
							goto next_p;
						}
					}
					goto move_past;
					break;
					
				case TEX_KERN_NODE:
				case TEX_MATH_NODE:
					cur_h = cur_h + width(p);
					break;
					
				case TEX_LIGATURE_NODE:
					mem[lig_trick] = mem[lig_char(p)]; 
					link(lig_trick) = link(p);
					p = lig_trick; 
					goto reswitch;
					break;
					
				default:
					break;
			}
			
			goto next_p;
fin_rule:
			if (isNodeRunning(rule_ht))
				rule_ht = height(this_box);
			if (isNodeRunning(rule_dp))
				rule_dp = depth(this_box);
			rule_ht = rule_ht + rule_dp;
			if ((rule_ht > 0) && (rule_wd > 0))
			{
				if (cur_h != dvi_h)
				{
					movement(cur_h - dvi_h, DVI_RIGHT1); 
					dvi_h = cur_h;
				}
				
				cur_v = base_line + rule_dp;
				
				if (cur_v != dvi_v)
				{
					movement(cur_v - dvi_v, DVI_DOWN1); 
					dvi_v = cur_v;
				}
				
				switch (font_direction(dvi_direction))
				{
					case TEX_DIR__LT:
						dev->dviSetRule(p, rule_ht, rule_wd);
						break;
						
					case TEX_DIR__LB:
						dev->dviDown(rule_ht);
    					dev->dviSetRule(p, rule_ht, rule_wd);
    					dev->dviUp(rule_ht);
						break;
						
					case TEX_DIR__RT:
						dev->dviLeft(rule_wd);
    					dev->dviPutRule(p, rule_ht, rule_wd);
						break;
						
					case TEX_DIR__RB:
						dev->dviLeft(rule_wd);
    					dev->dviDown(rule_ht);
    					dev->dviPutRule(p, rule_ht, rule_wd);
    					dev->dviUp(rule_ht);
						break;
						
					case TEX_DIR__TL:
						dev->dviDown(rule_wd);
    					dev->dviLeft(rule_ht);
    					dev->dviSetRule(p, rule_wd, rule_ht);
						break;
						
					case TEX_DIR__TR:
						dev->dviDown(rule_wd);
    					dev->dviPutRule(p, rule_wd, rule_ht);
						break;
						
					case TEX_DIR__BL:
						dev->dviLeft(rule_ht);
    					dev->dviSetRule(p, rule_wd, rule_ht);
    					dev->dviUp(rule_wd);
						break;
						
					case TEX_DIR__BR:
						dev->dviPutRule(p, rule_wd, rule_ht);
    					dev->dviUp(rule_wd);
						break;
						
					default:
						break;
				}
				
				cur_v = base_line; 
				dvi_h = dvi_h+rule_wd;
			}
			
move_past: 
			cur_h = cur_h + rule_wd;
next_p:
			p = link(p);
		}
	}
	
	cur_h = saving_h; 
	cur_v = saving_v;
	if (cur_h != dvi_h)
	{
		movement(cur_h - dvi_h, DVI_RIGHT1); 
		dvi_h = cur_h;
	}
					
	if (cur_v != dvi_v)
	{
		movement(cur_v - dvi_v, DVI_DOWN1); 
		dvi_v = cur_v;
	}
	
	pruneMovements(save_loc);
	if (cur_s > 0)
		dev->dviPop(save_loc);
		
	cur_s--;
	dvi_direction = save_direction;
	
	while (dir_ptr != TEX_NULL)
	{
		dir_tmp = dir_ptr;
		dir_ptr = link(dir_tmp);
		freeNode(dir_tmp, TEX_DIR_NODE_SIZE);
	}
}

void XWTeX::movement(qint32 w, qint32 o)
{
	switch (box_direction(dvi_direction))
	{
		case TEX_DIR_TL_:
			break;
			
		case TEX_DIR_TR_:
			if (o == DVI_RIGHT1)
				w = -w;
			break;
			
		case TEX_DIR_LT_:
			if (o == DVI_RIGHT1)
				o = DVI_DOWN1;
			else
				o = DVI_RIGHT1;
			break;
			
		case TEX_DIR_LB_:
			if (o == DVI_RIGHT1)
			{
				o = DVI_DOWN1; 
				w = -w;
			}
			else
				o = DVI_RIGHT1;
			break;
			
		case TEX_DIR_BL_:
			if (o == DVI_DOWN1)
				w = -w;
			break;
			
		case TEX_DIR_BR_:
			w = -w;
			break;
			
		case TEX_DIR_RT_:
			if (o == DVI_RIGHT1)
				o = DVI_DOWN1;
			else
			{
				o = DVI_RIGHT1; 
				w = -w;
			}
			break;
			
		case TEX_DIR_RB_:
			if (o == DVI_RIGHT1)
				o = DVI_DOWN1;
			else
				o = DVI_RIGHT1;
			w = -w;
			break;
			
		default:
			break;
	}
		
	dev->dviDownRight(o, w);
	return ;
}

void XWTeX::outWhat(qint32 p)
{
	qint32 j;
	
	switch (subtype(p))
	{
		case TEX_OPEN_NODE:
		case TEX_WRITE_NODE:
		case TEX_CLOSE_NODE:
			if (!doing_leaders)
			{
				j = write_stream(p);
				if (subtype(p) == (quint16)TEX_WRITE_NODE)
					writeOut(p);
				else
				{
					if (write_open[j])
						write_file[j]->close();
					
					if (subtype(p) == (quint16)TEX_CLOSE_NODE)
						write_open[j] = false;
					else if (j < 16)
					{
						cur_name = open_name(p); 
						cur_area = open_area(p);
						cur_ext  = open_ext(p);
						if (cur_ext == TEX_TOO_BIG_CHAR)
							cur_ext = TeXTex;
							
						packCurName();
						QString fn = getFileName();						
						if (write_file[j])
						{
							if (write_file[j]->isOpen())
								write_file[j]->close();
							delete write_file[j];
							write_file[j] = 0;
						}
						
						write_file[j] = dev->openOutput(fn);
						write_open[j] = true;							
						if (log_opened)
						{
							old_setting = selector;
							if (tracingOnline() <= 0)
								selector = TEX_LOG_ONLY;
							else
								selector = TEX_TERM_AND_LOG;
									
							printnl("\\openout");
        			printInt(j);
        			print(" = '");
        			printFileName(cur_name,cur_area,cur_ext);
        			print("'."); 
        			printnl(""); 
        			println();
        			selector = old_setting;
						}
					}
				}
			}
			break;
			
		case TEX_SPECIAL_NODE:
			specialOut(p);
			break;
			
		case TEX_LANGUAGE_NODE:
			break;
			
		case TEX_LOCAL_PAR_NODE:
			break;
			
		default:
			confusion("ext4");
			break;
	}
}

void XWTeX::prepareMag()
{
	if ((mag_set > 0 ) && (mag() != mag_set))
	{
		printErr(tr("Incompatible magnification (")); 
		printInt(mag());
		print(");"); 
		printnl(tr(" the previous value will be retained"));
		help2(tr("I can handle only one magnification ratio per job. So I've "));
		help_line[0] = tr("reverted to the magnification you used earlier on this run.");
		intError(mag_set);
  		geqWordDefine(TEX_INT_BASE + TEX_MAG_CODE, mag_set);
	}
	
	if ((mag() <= 0) || (mag() > 32768))
	{
		printErr(tr("Illegal magnification has been changed to 1000"));
		help1(tr("The magnification ratio must be between 1 and 32768."));
  		intError(mag()); 
  		geqWordDefine(TEX_INT_BASE + TEX_MAG_CODE, 1000);
	}
	
	mag_set = mag();
}

void XWTeX::pruneMovements(qint32 l)
{
	qint32 p;
	while (down_ptr != TEX_NULL)
	{
		if (location(down_ptr) < l)
			goto done;
			
		p = down_ptr; 
		down_ptr = link(p); 
		freeNode(p, TEX_MOVEMENT_NODE_SIZE);
	}
	
done:
	while (right_ptr != TEX_NULL)
	{
		if (location(right_ptr) < l)
			return ;
			
		p = right_ptr; 
		right_ptr = link(p); 
		freeNode(p, TEX_MOVEMENT_NODE_SIZE);
	}
}

void XWTeX::shipOut(qint32 p)
{
	qint32 page_loc, s, l;	
	char   j, k;
	
	if (tracingOutput() > 0)
	{
		println();
  	print(tr("Completed box being shipped out"));
	}
	
	if (term_offset > (max_print_line - 9))
		println();
	else if ((term_offset > 0) || (file_offset > 0))
		printChar(' ');
		
	printChar('['); 
	j = 9;
	while ((count(j) == 0) && (j > 0))
		j--;
		
	for (k = 0; k <= j; k++)
	{
		printInt(count(k));
		if (k < j)
			printChar('.');
	}
	
	if (tracingOutput() > 0)
	{
		printChar(']');
  		beginDiagnostic(); 
  		showBox(p); 
  		endDiagnostic(true);
	}
	
	if ((height(p) > TEX_MAX_DIMEN) || 
		(depth(p) > TEX_MAX_DIMEN) || 
		((height(p) + depth(p) + voffset()) > TEX_MAX_DIMEN) || 
		((width(p) + hoffset()) > TEX_MAX_DIMEN))
	{
		printErr(tr("Huge page cannot be shipped out"));
		help2(tr("The page just created is more than 18 feet tall or"));
   		help_line[0] = tr("more than 18 feet wide, so I suspect something went wrong.");
  		error();
  		if (tracingOutput() <= 0)
  		{
  			beginDiagnostic();
    		printnl(tr("The following box has been deleted:"));
    		showBox(p);
    		endDiagnostic(true);
  		}
  		goto done;
	}
	
	if ((height(p) + depth(p) + voffset()) > max_v)
		max_v = height(p) + depth(p) + voffset();
		
	if ((width(p) + hoffset()) > max_h)
		max_h = width(p) + hoffset();
		
	dvi_h = 0; 
	dvi_v = 0; 
	cur_h = 0; 
	cur_v = 0; 
	dvi_f = TEX_NULL_FONT;
	
	if (total_pages == 0)
	{
		dev->dviPre();
		dev->dviFour(25400000); 
		dev->dviFour(473628672);
		prepareMag(); 
		dev->dviFour(mag());	
		if (output_comment)
		{
			l = strlen(output_comment); 
			dev->dviOut(l);
			for (s = 0; s < l; s++)
				dev->dviOut(output_comment[s]);
		}
		else
		{
			old_setting = selector; 
			selector = TEX_NEW_STRING;				
			print("XWAleph output, Version 3.141592--1.15--2.1, ");
  			printInt(year()); 
  			printChar('.');
  			printTwo(month()); 
  			printChar('.'); 
  			printTwo(day());
  			printChar(':'); 
  			printTwo(time() / 60);
  			printTwo(time() % 60);
  			selector = old_setting;
  			dev->dviOut(curLength());
  			for (s = str_start(str_ptr); s < pool_ptr; s++)
  				dev->dviOut((uchar)(str_pool[s]));
  			pool_ptr = str_start(str_ptr);
  		}
	}
	
	page_loc = dev->dvi_offset + dev->dvi_ptr;
	dev->dviBop();
	for (k = 0; k <= 9; k++)
		dev->dviFour(count(k));
		
	dev->dviFour(last_bop); 
	last_bop = page_loc;
	dvi_direction = pageDirection();
	
	switch (box_direction(dvi_direction))
	{
		case TEX_DIR_TL_:
		case TEX_DIR_LT_:
			break;
			
		case TEX_DIR_TR_:
		case TEX_DIR_RT_:
			dev->dviRight(pageWidth() - pageRightOffset());
  			dvi_h = -pageRightOffset();
			break;
			
		case TEX_DIR_RB_:
		case TEX_DIR_BR_:
			dev->dviRight(pageWidth() - pageRightOffset());
  			dev->dviDown(pageHeight() - pageBottomOffset());
  			dvi_h = -pageRightOffset();
  			dvi_v = -pageBottomOffset();
			break;
			
		case TEX_DIR_BL_:
		case TEX_DIR_LB_:
			dev->dviDown(pageHeight() - pageBottomOffset());
  			dvi_v = -pageBottomOffset();
			break;
			
		default:
			break;
	}
	
	cur_h = hoffset();
	cur_v = height(p) + voffset();
	
	switch (box_direction(dvi_direction))
	{
		case TEX_DIR_TL_:
			dev->dviRight(cur_h);
  			dev->dviDown(cur_v);
			break;
			
		case TEX_DIR_TR_:
			dev->dviRight(-cur_h);
  			dev->dviDown(cur_v);
			break;
			
		case TEX_DIR_LT_:
			dev->dviRight(cur_v);
  			dev->dviDown(cur_h);
			break;
			
		case TEX_DIR_LB_:
			dev->dviRight(cur_v);
  			dev->dviDown(-cur_h);
			break;
			
		case TEX_DIR_BL_:
			dev->dviRight(cur_h);
  			dev->dviDown(-cur_v);
			break;
			
		case TEX_DIR_BR_:
			dev->dviRight(-cur_h);
  			dev->dviDown(-cur_v);
			break;
			
		case TEX_DIR_RT_:
			dev->dviRight(-cur_v);
  			dev->dviDown(cur_h);
			break;
			
		case TEX_DIR_RB_:
			dev->dviRight(-cur_v);
  			dev->dviDown(-cur_h);
			break;
			
		default:
			break;
	}
	
	temp_ptr = p;
	dvi_h = dvi_h + cur_h;
	dvi_v = dvi_v + cur_v;
	if (type(p) == (quint16)TEX_VLIST_NODE)
		vlistOut();
	else
		hlistOut();
		
	dev->dviEop();
	total_pages++;
	cur_s = -1;
	
done:
	if (tracingOutput() <= 0)
		printChar(']');
		
	dead_cycles = 0;
	
#ifdef XW_TEX_STAT
	if (tracingStats() > 1)
	{
		printnl(tr("Memory usage before: "));
		printInt(var_used); 
		printChar('&');
  		printInt(dyn_used); 
  		printChar(';');
	}
#endif //XW_TEX_STAT

	flushNodeList(p);
	
#ifdef XW_TEX_STAT
	if (tracingStats() > 1)
	{
		print(tr(" after: "));
  		printInt(var_used); 
  		printChar('&');
  		printInt(dyn_used); 
  		print(tr("; still untouched: "));
  		printInt(hi_mem_min - lo_mem_max - 1); 
  		println();
	}
#endif //XW_TEX_STAT
}

void XWTeX::specialOut(qint32 p)
{
	if (cur_h != dvi_h)
	{
		movement(cur_h - dvi_h, DVI_RIGHT1); 
		dvi_h = cur_h;
	}
	
	if (cur_v != dvi_v)
	{
		movement(cur_v - dvi_v, DVI_DOWN1); 
		dvi_v = cur_v;
	}
	
	old_setting = selector; 
	selector = TEX_NEW_STRING;
	showTokenList(link(write_tokens(p)), TEX_NULL, pool_size - pool_ptr);
	selector = old_setting;
	strRoom(1);
	dev->dviXXX(p, curLength());
	for (qint32 k = str_start(str_ptr); k < pool_ptr; k++)
		dev->dviOut((uchar)(str_pool[k]));
		
	pool_ptr = str_start(str_ptr);
}

void XWTeX::vlistOut()
{
	qint32 left_edge, top_edge, save_h, save_v, this_box, g_order, g_sign;
	qint32 p, save_loc, leader_box, leader_ht, lx;
	bool   outer_doing_leaders;
	qint32 edge, cur_g, save_direction, effective_vertical, basepoint_horizontal, g;
	double glue_temp, cur_glue;
	qint32 basepoint_vertical, edge_v, saving_v, saving_h;
	
	cur_g = 0; 
	cur_glue = 0.0;
	this_box = temp_ptr; 
	g_order = glue_order(this_box);
	g_sign = glue_sign(this_box); 
	p = list_ptr(this_box);
	saving_v = dvi_v; 
	saving_h = dvi_h;
	save_direction = dvi_direction;
	dvi_direction = box_dir(this_box);
	cur_s++;
	if (cur_s > 0)
		dev->dviPush();
		
	if (cur_s > max_push)
		max_push = cur_s;
		
	save_loc = dev->dvi_offset + dev->dvi_ptr; 
	left_edge = cur_h; 
	cur_v = cur_v - height(this_box);
	top_edge = cur_v;
	while (p != TEX_NULL)
	{
		if (isCharNode(p))
			confusion("vlistout");
		else
		{
			switch (type(p))
			{
				case TEX_HLIST_NODE:
				case TEX_VLIST_NODE:
					if (!(dir_orthogonal(dir_primary[box_dir(p)], dir_primary[dvi_direction])))
					{
						effective_vertical = height(p) + depth(p);
						if ((type(p) == (quint16)TEX_HLIST_NODE) && (is_mirrored(box_dir(p))))
							basepoint_vertical = depth(p);
						else
							basepoint_vertical = height(p);
							
						if (dir_opposite(dir_secondary[box_dir(p)], dir_secondary[dvi_direction]))
							basepoint_horizontal = width(p);
						else
							basepoint_horizontal = 0;
					}
					else
					{
						effective_vertical = width(p);
						if (!(is_mirrored(box_dir(p))))
						{
							if (dir_eq(dir_primary[box_dir(p)], dir_secondary[dvi_direction]))
								basepoint_horizontal = height(p);
							else
								basepoint_horizontal = depth(p);
						}
						else
						{
							if (dir_eq(dir_primary[box_dir(p)], dir_secondary[dvi_direction]))
								basepoint_horizontal = depth(p);
							else
								basepoint_horizontal = height(p);
						}
						
						if (dir_eq(dir_secondary[box_dir(p)], dir_primary[dvi_direction]))
							basepoint_vertical = 0;
						else
							basepoint_vertical = width(p);
					}
					
					basepoint_horizontal = basepoint_horizontal + shift_amount(p);
					if (list_ptr(p) == TEX_NULL)
						cur_v = cur_v + effective_vertical;
					else
					{
						if (cur_h != dvi_h)
						{
							movement(cur_h - dvi_h, DVI_RIGHT1); 
							dvi_h = cur_h;
						}
						
						if (cur_v != dvi_v)
						{
							movement(cur_v - dvi_v, DVI_DOWN1); 
							dvi_v = cur_v;
						}  
						
						edge_v = cur_v;
						
						cur_h = left_edge + basepoint_horizontal;
  						cur_v = cur_v + basepoint_vertical;
  						if (cur_h != dvi_h)
						{
							movement(cur_h - dvi_h, DVI_RIGHT1); 
							dvi_h = cur_h;
						}
						
						if (cur_v != dvi_v)
						{
							movement(cur_v - dvi_v, DVI_DOWN1); 
							dvi_v = cur_v;
						}   
  						save_h = dvi_h; 
  						save_v = dvi_v;
  						temp_ptr = p;
  						
  						if (type(p) == (quint16)TEX_VLIST_NODE)
  							vlistOut();
  						else
  							hlistOut();
  							
  						dvi_h = save_h; 
  						dvi_v = save_v;
  						cur_h = left_edge; 
  						cur_v = edge_v + effective_vertical;
					}
					break;
					
				case TEX_RULE_NODE:
					if (!(dir_orthogonal(dir_primary[rule_dir(p)], dir_primary[dvi_direction])))
					{
						rule_ht = height(p); 
						rule_dp = depth(p); 
						rule_wd = width(p);
					}
					else
					{
						rule_ht = width(p) / 2;
    					rule_dp = width(p) / 2;
    					rule_wd = height(p) + depth(p);
					}
					goto fin_rule;
					break;
					
				case TEX_WHATSIT_NODE:
					outWhat(p);
					break;
					
				case TEX_GLUE_NODE:
					g = glue_ptr(p); 
					rule_ht = width(g) - cur_g;
					if (g_sign != TEX_NORMAL)
					{
						if (g_sign == TEX_STRETCHING)
						{
							if (stretch_order(g) == (quint16)g_order)
							{
								cur_glue = cur_glue + stretch(g);
								glue_temp = glue_set(this_box) * cur_glue;
								if (glue_temp > TEX_BILLION)
									glue_temp = TEX_BILLION;
								else if (glue_temp < -TEX_BILLION)
									glue_temp = -TEX_BILLION;
									
								cur_g = (qint32)(qRound(glue_temp));
							}
						}
						else if (shrink_order(g) == (quint16)g_order)
						{
							cur_glue = cur_glue - shrink(g);
							glue_temp = glue_set(this_box) * cur_glue;
							if (glue_temp > TEX_BILLION)
								glue_temp = TEX_BILLION;
							else if (glue_temp < -TEX_BILLION)
								glue_temp = -TEX_BILLION;
									
      						cur_g = (qint32)(qRound(glue_temp));
						}
					}
					rule_ht = rule_ht + cur_g;
					if (subtype(p) >= (quint16)TEX_A_LEADERS)
					{
						leader_box = leader_ptr(p);
						if (type(leader_box) == (quint16)TEX_RULE_NODE)
						{
							rule_wd = width(leader_box); 
							rule_dp = 0;
  							goto fin_rule;
						}
						
						if (!(dir_orthogonal(dir_primary[box_dir(leader_box)], dir_primary[dvi_direction])))
							leader_ht = height(leader_box) + depth(leader_box);
						else
							leader_ht = width(leader_box);
							
						if ((leader_ht > 0) && (rule_ht > 0))
						{
							rule_ht = rule_ht + 10;
							edge = cur_v + rule_ht; 
							lx = 0;
							if (subtype(p) == (quint16)TEX_A_LEADERS)
							{
								save_v = cur_v;
  								cur_v  = top_edge + leader_ht * ((cur_v - top_edge) / leader_ht);
  								if (cur_v < save_v)
  									cur_v = cur_v + leader_ht;
							}
							else
							{
								lq = rule_ht / leader_ht;
								lr = rule_ht % leader_ht;
								if (subtype(p) == (quint16)TEX_C_LEADERS)
									cur_v = cur_v + (lr / 2);
								else
								{
									lx = lr / (lq + 1);
    								cur_v = cur_v +((lr - (lq - 1) * lx) / 2);
								}
							}
							
							while ((cur_v + leader_ht) <= edge)
							{
								if (!(dir_orthogonal(dir_primary[box_dir(leader_box)], dir_primary[dvi_direction])))
								{
									effective_vertical = height(leader_box) + depth(leader_box);
									if ((type(leader_box) == (quint16)TEX_HLIST_NODE) && (is_mirrored(box_dir(leader_box))))
										basepoint_vertical = depth(leader_box);
									else
										basepoint_vertical = height(leader_box);
										
									if (dir_opposite(dir_secondary[box_dir(leader_box)], dir_secondary[dvi_direction]))
										basepoint_horizontal = width(leader_box);
									else
										basepoint_horizontal = 0;
								}
								else
								{
									effective_vertical = width(leader_box);
									if (!(is_mirrored(box_dir(leader_box))))
									{
										if (dir_eq(dir_primary[box_dir(leader_box)], dir_secondary[dvi_direction]))
											basepoint_horizontal = height(leader_box);
										else
											basepoint_horizontal = depth(leader_box);
									}
									else
									{
										if (dir_eq(dir_primary[box_dir(leader_box)], dir_secondary[dvi_direction]))
											basepoint_horizontal = depth(leader_box);
										else
											basepoint_horizontal = height(leader_box);
									}
									
									if (dir_eq(dir_secondary[box_dir(leader_box)], dir_primary[dvi_direction]))
										basepoint_vertical = width(leader_box);
									else
										basepoint_vertical = 0;
								}
								
								basepoint_horizontal = basepoint_horizontal + shift_amount(leader_box);
								temp_ptr = leader_box;
								cur_h = left_edge + basepoint_horizontal;
								edge_v = cur_v ;
								cur_v = cur_v + basepoint_vertical;
								if (cur_h != dvi_h)
								{
									movement(cur_h - dvi_h, DVI_RIGHT1); 
									dvi_h = cur_h;
								}
	 							if (cur_v != dvi_v)
								{
									movement(cur_v - dvi_v, DVI_DOWN1); 
									dvi_v = cur_v;
								}  
								save_h = dvi_h; 
								save_v = dvi_v;
								outer_doing_leaders = doing_leaders; 
								doing_leaders = true;
								if (type(leader_box) == (quint16)TEX_VLIST_NODE)
									vlistOut();
								else
									hlistOut();
									
								doing_leaders = outer_doing_leaders;
								dvi_h = save_h; 
								dvi_v = save_v;
								cur_h = left_edge; 
								cur_v = edge_v + leader_ht + lx;
							}
							
							cur_v = edge - 10; 
							goto next_p;
						}
					}
					goto move_past;
					break;
					
				case TEX_KERN_NODE:
					cur_v = cur_v + width(p);
					break;
					
				default:
					break;
			}
			
			goto next_p;
			
fin_rule:
			if (isNodeRunning(rule_wd))
				rule_wd = width(this_box);
				
			rule_ht = rule_ht + rule_dp;
			cur_v = cur_v + rule_ht;
			if ((rule_ht > 0) && (rule_wd > 0))
			{
				if (cur_h != dvi_h)
				{
					movement(cur_h - dvi_h, DVI_RIGHT1); 
					dvi_h = cur_h;
				}
				
	 			if (cur_v != dvi_v)
				{
					movement(cur_v - dvi_v, DVI_DOWN1); 
					dvi_v = cur_v;
				}  
				
				switch (font_direction(dvi_direction))
				{
					case TEX_DIR__LT:
						dev->dviPutRule(p, rule_ht, rule_wd);
						break;
						
					case TEX_DIR__LB:
						dev->dviDown(rule_ht);
    					dev->dviPutRule(p, rule_ht, rule_wd);
    					dev->dviUp(rule_ht);
						break;
						
					case TEX_DIR__RT:
						dev->dviLeft(rule_wd);
    					dev->dviSetRule(p, rule_ht, rule_wd);
						break;
						
					case TEX_DIR__RB:
						dev->dviDown(rule_ht);
    					dev->dviLeft(rule_wd);
    					dev->dviSetRule(p, rule_ht, rule_wd);
    					dev->dviUp(rule_ht);
						break;
						
					case TEX_DIR__TL:
						dev->dviDown(rule_wd);
    					dev->dviLeft(rule_ht);
    					dev->dviSetRule(p, rule_wd, rule_ht);
    					dev->dviUp(rule_wd);
						break;
						
					case TEX_DIR__TR:
						dev->dviDown(rule_wd);
    					dev->dviPutRule(p, rule_wd, rule_ht);
    					dev->dviUp(rule_wd);
						break;
						
					case TEX_DIR__BL:
						dev->dviLeft(rule_ht);
    					dev->dviSetRule(p, rule_wd, rule_ht);
						break;
						
					case TEX_DIR__BR:
						dev->dviPutRule(p, rule_wd, rule_ht);
						break;
						
					default:
						break;
				}
			}
			
			goto next_p;
			
move_past: 
			cur_v = cur_v + rule_ht;
		}
		
next_p:
		p = link(p);
	}
	
	cur_v = saving_v; 
	cur_h = saving_h;
	
	if (cur_v != dvi_v)
	{
		movement(cur_v - dvi_v, DVI_DOWN1); 
		dvi_v = cur_v;
	} 
	
	if (cur_h != dvi_h)
	{
		movement(cur_h - dvi_h, DVI_RIGHT1); 
		dvi_h = cur_h;
	}
	
	pruneMovements(save_loc);
	if (cur_s > 0) 
		dev->dviPop(save_loc);
	cur_s--;
	dvi_direction = save_direction;
}

void XWTeX::writeOut(qint32 p)
{
	qint32 old_mode, j, q, r, d;
	bool clobbered;
	
	q = getAvail(); 
	info(q) = TEX_RIGHT_BRACE_TOKEN + '}';
	r = getAvail(); 
	link(q) = r; 
	info(r) = TEX_END_WRITE_TOKEN; 
	insList(q);
	beginTokenList(write_tokens(p), TEX_WRITE_TEXT);
	q = getAvail(); 
	info(q) = TEX_LEFT_BRACE_TOKEN + '{'; 
	insList(q);
	old_mode = mode; 
	mode = 0;
	cur_cs = write_loc; 
	q = scanToks(false, true);
	getToken();
	if (cur_tok != TEX_END_WRITE_TOKEN)
	{
		printErr(tr("Unbalanced write command"));
		help2(tr("On this page there's a \\write with fewer real {'s than }'s."));
		help_line[0] = tr("I can't handle that very well; good luck.");
		error();
		do
		{
			getToken();
		} while (cur_tok != TEX_END_WRITE_TOKEN);
	}
	mode = old_mode;
	endTokenList();
	
	old_setting = selector; 
	j = write_stream(p);
	if (shell_enabled_p && (j == 18))
		selector = TEX_NEW_STRING;
	else if (write_open[j])
		selector = j;
	else
	{
		if ((j == 17) && (selector == TEX_TERM_AND_LOG))
			selector = TEX_LOG_ONLY;
			
		printnl("");
	}
	
	tokenShow(def_ref); 
	println();
	flush();
	flushList(def_ref); 
	if (j == 18)
	{
		if ((tracingOnline() <= 0))
			selector = TEX_LOG_ONLY;
		else
			selector = TEX_TERM_AND_LOG;
			
		printnl(tr("system("));
		for (d = 0; d < curLength(); d++)
			print(str_pool[str_start(str_ptr) + d]);
		print(")...");
		if (shell_enabled_p)
		{
			strRoom(1); 
			appendChar(0);
			clobbered = false;
			for (d = 0; d < curLength(); d++)
			{
				str_pool[str_start(str_ptr) + d]  = xchr[(uchar)str_pool[str_start(str_ptr) + d]];
				if ((str_pool[str_start(str_ptr) + d] == TEX_NULL_CODE) && (d < (curLength() - 1)))
					clobbered = true;
			}
			
			if (clobbered)
				print(tr("clobbered"));
			else
			{
				QString s;
				for (d = 0; d < curLength(); d++)
				{
					outside_string_array[d] = xchr[(uchar)str_pool[str_start(str_ptr) + d]];
					s.append(QChar((int)(outside_string_array[d])));
				}
				
				outside_string_array[curLength()] = TEX_NULL_CODE;
				QProcess program;
				QProcessEnvironment envlist = QProcessEnvironment::systemEnvironment();				
				QString envv = getEnv("CUR_DOC_PATH");
				envlist.insert("CUR_DOC_PATH", envv);
				envv = xwApp->getTmpDir();
				envlist.insert("OUTPUT_PATH", envv);
				program.setProcessEnvironment(envlist);
				if (dev->outputDirectory.isEmpty())
					dev->outputDirectory = xwApp->getTmpDir();
				program.setWorkingDirectory(dev->outputDirectory);
				QStringList tmplist = s.split(QRegExp("\\s+"));
				QStringList arglist;
				QString programname;
				if (tmplist.size() > 0)
					programname = tmplist[0];
				for (int i = 1; i < tmplist.size(); i++)
					arglist << tmplist.at(i);
					
				program.start(programname, arglist);
				if (program.waitForStarted())
					program.waitForFinished();
			}
			
			pool_ptr = str_start(str_ptr);
		}
		else
			print(tr("disabled"));
			
		printChar('.'); 
		printnl(""); 
		println();
	}
	
	selector = old_setting;
}

