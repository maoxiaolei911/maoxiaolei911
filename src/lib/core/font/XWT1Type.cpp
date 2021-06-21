/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWApplication.h"
#include "XWCFFType.h"
#include "XWT1Type.h"

#define CS_OP_NOSUPPORT -4
#define CS_BUFFER_ERROR -3
#define CS_STACK_ERROR  -2
#define CS_PARSE_ERROR  -1
#define CS_PARSE_OK      0
#define CS_PARSE_END     1
#define CS_SUBR_RETURN   2
#define CS_CHAR_END      3

static int status = CS_PARSE_ERROR;

#define DST_NEED(a,b) {if ((a) < (b)) { status = CS_BUFFER_ERROR ; return ; }}
#define SRC_NEED(a,b) {if ((a) < (b)) { status = CS_PARSE_ERROR  ; return ; }}
#define NEED(a,b)     {if ((a) < (b)) { status = CS_STACK_ERROR  ; return ; }}

#define T1_CS_PHASE_INIT 0
#define T1_CS_PHASE_HINT 1
#define T1_CS_PHASE_PATH 2
#define T1_CS_PHASE_FLEX 3

static int phase = -1;
static int nest  = -1;

#ifndef CS_STEM_GROUP_MAX
#define CS_STEM_GROUP_MAX CS_STEM_ZONE_MAX
#endif

#ifndef PS_ARG_STACK_MAX
#define PS_ARG_STACK_MAX (CS_STEM_ZONE_MAX*2+2)
#endif

#define HSTEM 0
#define VSTEM 1

#define T1_CS_FLAG_NONE 0
#define T1_CS_FLAG_USE_HINTMASK (1 << 0)
#define T1_CS_FLAG_USE_CNTRMASK (1 << 1)
#define T1_CS_FLAG_USE_SEAC     (1 << 2)


static int cs_stack_top = 0;
static int ps_stack_top = 0;
static double cs_arg_stack[CS_ARG_STACK_MAX+1];
static double ps_arg_stack[PS_ARG_STACK_MAX];

#define CS_HINT_DECL -1
#define CS_FLEX_CTRL -2
#define CS_CNTR_CTRL -3

#define cs_hstem      1
/*      RESERVED      2 */
#define cs_vstem      3
#define cs_vmoveto    4
#define cs_rlineto    5
#define cs_hlineto    6
#define cs_vlineto    7
#define cs_rrcurveto  8
#define cs_closepath  9
#define cs_callsubr   10
#define cs_return     11
#define cs_escape     12
#define cs_hsbw       13
#define cs_endchar    14
/*      RESERVED      15 */
/*      RESERVED      16 */
/*      RESERVED      17 */
#define cs_hstemhm    18
#define cs_hintmask   19
#define cs_cntrmask   20
#define cs_rmoveto    21
#define cs_hmoveto    22
#define cs_vstemhm    23
#define cs_rcurveline 24
#define cs_rlinecurve 25
#define cs_vvcurveto  26
#define cs_hhcurveto  27
/*      SHORTINT      28 : first byte of shortint*/
#define cs_callgsubr  29
#define cs_vhcurveto  30
#define cs_hvcurveto  31

/* 2-byte op. flex 34-37 used. */

/*
 * 2-byte CharString operaotrs:
 *  "dotsection" is obsoleted in Type 2 charstring.
 */

#define cs_dotsection 0
#define cs_vstem3     1
#define cs_hstem3     2
#define cs_and        3
#define cs_or         4
#define cs_not        5
#define cs_seac       6
#define cs_sbw        7
/*      RESERVED      8  */
#define cs_abs        9
#define cs_add        10
#define cs_sub        11
#define cs_div        12
/*      RESERVED      13 */
#define cs_neg        14
#define cs_eq         15
#define cs_callothersubr 16
#define cs_pop        17
#define cs_drop       18
/*      RESERVED      19 */
#define cs_put        20
#define cs_get        21
#define cs_ifelse     22 
#define cs_random     23
#define cs_mul        24
/*      RESERVED      25 */
#define cs_sqrt       26
#define cs_dup        27
#define cs_exch       28
#define cs_index      29
#define cs_roll       30
/*      RESERVED      31 */
/*      RESERVED      32 */
#define cs_setcurrentpoint 33
#define cs_hflex      34
#define cs_flex       35
#define cs_hflex1     36
#define cs_flex1      37

#define IS_PATH_OPERATOR(o) (((o) >= cs_vmoveto && (o) <= cs_closepath) || \
                             ((o) >= cs_rmoveto && (o) <= cs_hvcurveto && \
                              (o) != cs_vstemhm && (o) != cs_callgsubr && (o) != 28)\
                            )
static int 
stem_compare (const void *v1, const void *v2)
{
  	int cmp = 0;
  	T1Stem * s1 = (T1Stem *) v1;
  	T1Stem * s2 = (T1Stem *) v2;
  	if (s1->dir == s2->dir) 
  	{
    	if (s1->pos == s2->pos) 
    	{
      		if (s1->del == s2->del)
				cmp = 0;
      		else
				cmp = (s1->del < s2->del) ? -1 : 1;
    	} 
    	else 
    	{
      		cmp = (s1->pos < s2->pos) ? -1 : 1;
    	}
  	} 
  	else 
  	{
    	cmp = (s1->dir == HSTEM) ? -1 : 1;
  	}

  	return cmp;
}

#define LIMITCHECK(n) do {\
                           if (cs_stack_top+(n) > CS_ARG_STACK_MAX) {\
                             status = CS_STACK_ERROR;\
                             return;\
                           }\
                      } while (0)
#define CHECKSTACK(n) do {\
                           if (cs_stack_top < (n)) {\
                             status = CS_STACK_ERROR;\
                             return;\
                           }\
                      } while (0)
#define CLEARSTACK()  do {\
                           cs_stack_top = 0;\
                      } while (0)


#define RESET_STATE() do {\
  status = CS_PARSE_OK;\
  phase  = T1_CS_PHASE_INIT;\
  nest   = 0;\
  ps_stack_top = 0;\
} while (0)


XWT1CharDesc::XWT1CharDesc(QObject * parent)
	:QObject(parent)
{
	flags = T1_CS_FLAG_NONE;
  	num_stems = 0;
  	sbw.wx  = sbw.wy  = 0.0;
  	sbw.sbx = sbw.sby = 0.0;
  	bbox.llx = bbox.lly = bbox.urx = bbox.ury = 0.0;
  	charpath = lastpath = 0;
}

XWT1CharDesc::~XWT1CharDesc()
{
	T1CPath * curr = charpath;
  	while (curr != 0) 
  	{
    	T1CPath * next = curr->next;
    	free(curr);
    	curr = next;
  	}

  	charpath = lastpath = 0;
}

void XWT1CharDesc::buildCharPath(uchar **data, 
	                             uchar *endptr, 
	                             XWCFFIndex *subrs)
{
	if (nest > CS_SUBR_NEST_MAX)
	{
    	xwApp->error(tr("subroutine nested too deeply.\n"));
    	return ;
    }
    
    nest++;
  	while (*data < endptr && status == CS_PARSE_OK)
  	{
  		uchar b0 = **data;
    	if (b0 == 255) 
      		getLongInt(data, endptr); /* Type 1 */
    	else if (b0 == cs_return) 
      		status = CS_SUBR_RETURN;
    	else if (b0 == cs_callsubr) 
    	{
      		if (cs_stack_top < 1) 
				status = CS_STACK_ERROR;
      		else 
      		{
				int idx = cs_arg_stack[--cs_stack_top];
				if (!subrs || idx >= subrs->count)
				{
	  				xwApp->error(tr("invalid Subr#.\n"));
	  				return ;
	  			}
				uchar * subr = subrs->data + subrs->offset[idx] - 1;
				long len  = subrs->offset[idx+1] - subrs->offset[idx];
				buildCharPath(&subr, subr+len, subrs);
				*data += 1;
      		}
    	} 
    	else if (b0 == cs_escape) 
      		doOperator2(data, endptr);
    	else if (b0 < 32 && b0 != 28) 
      		doOperator1(data);
    	else if ((b0 <= 22 && b0 >= 27) || b0 == 31) 
      		status = CS_PARSE_ERROR;
    	else 
      		getInteger(data, endptr);
  	}
  	
  	if (status == CS_SUBR_RETURN) 
  	{
    	status = CS_PARSE_OK;
  	} 
  	else if (status == CS_CHAR_END && *data < endptr) 
  	{
    	if (!(*data == endptr - 1 && **data == cs_return))
    	{
    		QString msg = QString(tr("garbage after endchar. (%1 bytes)\n")).arg((long) (endptr - *data));
      		xwApp->warning(msg);
      	}
  	} 
  	else if (status < CS_PARSE_OK) 
  	{
    	xwApp->error(tr("parsing charstring failed.\n"));
    	return ;
  	}

  	nest--;
}

void XWT1CharDesc::doPostProc()
{
	if (!charpath)
    	return;
    	
    bbox.llx = bbox.lly =  100000.0;
  	bbox.urx = bbox.ury = -100000.0;

  	T1CPath *cur  = charpath;
  	T1CPath *prev = 0;
  	double x, y;
  	x = y = 0.0;

#define UPDATE_BBOX(b,x,y) do {\
  if ((b).llx > (x)) (b).llx = (x);\
  if ((b).urx < (x)) (b).urx = (x);\
  if ((b).lly > (y)) (b).lly = (y);\
  if ((b).ury < (y)) (b).ury = (y);\
} while (0)

#define TRY_COMPACT (prev && cur && ((prev->num_args + cur->num_args) <= CS_ARG_STACK_MAX))
	
	while (cur != 0)
	{
		T1CPath *next = cur->next;
    	switch (cur->type)
    	{
    		case cs_rmoveto:
      			x += cur->args[0]; y += cur->args[1];
      			UPDATE_BBOX(bbox, x, y);
      			break;
      			
    		case cs_rlineto:
      			x += cur->args[0]; y += cur->args[1];
      			UPDATE_BBOX(bbox, x, y);
      			if (TRY_COMPACT) 
      			{
					if (prev->type == cs_rlineto) 
					{
	  					copyArgs(prev->args+prev->num_args, cur->args, cur->num_args);
	  					prev->num_args += cur->num_args;
	  					prev->next = next;
	  					free(cur); 
	  					cur = 0;
					} 
					else if (prev->type == cs_rrcurveto) 
					{
	  					copyArgs(prev->args+prev->num_args, cur->args, cur->num_args);
	  					prev->num_args += cur->num_args;
	  					prev->type = cs_rcurveline;
	  					prev->next = next;
	  					free(cur); 
	  					cur = 0;
					}
      			}
      			break;
      			
    		case cs_hmoveto:
      			x += cur->args[0];
      			UPDATE_BBOX(bbox, x, y);
      			break;
      			
    		case cs_hlineto:
      			x += cur->args[0];
      			UPDATE_BBOX(bbox, x, y);
      			if (TRY_COMPACT) 
      			{
					if ((prev->type == cs_vlineto && (prev->num_args % 2) == 1) ||
	    				(prev->type == cs_hlineto && (prev->num_args % 2) == 0)) 
	    			{
	  					copyArgs(prev->args+prev->num_args, cur->args, cur->num_args);
	  					prev->num_args += cur->num_args;
	  					prev->next = next;
	  					free(cur); 
	  					cur = 0;
					}
      			}
      			break;
      			
    		case cs_vmoveto:
      			y += cur->args[0];
      			UPDATE_BBOX(bbox, x, y);
      			break;
      			
    		case cs_vlineto:
      			y += cur->args[0];
      			UPDATE_BBOX(bbox, x, y);
      			if (TRY_COMPACT) 
      			{
					if ((prev->type == cs_hlineto && (prev->num_args % 2) == 1) ||
	    				(prev->type == cs_vlineto && (prev->num_args % 2) == 0)) 
	    			{
	  					copyArgs(prev->args+prev->num_args, cur->args, cur->num_args);
	  					prev->num_args += cur->num_args;
	  					prev->next = next;
	  					free(cur); 
	  					cur = 0;
					}
      			}
      			break;
      			
    		case cs_rrcurveto:
      			for (int i = 0; i < 3; i++) 
      			{
					x += cur->args[2*i]; 
					y += cur->args[2*i+1];
					UPDATE_BBOX(bbox, x, y);
      			}
      			if (TRY_COMPACT) 
      			{
					if (prev->type == cs_rrcurveto) 
					{
	  					copyArgs(prev->args+prev->num_args, cur->args, cur->num_args);
	  					prev->num_args += cur->num_args;
	  					prev->next = next;
	  					free(cur); 
	  					cur = 0;
					} 
					else if (prev->type == cs_rlineto) 
					{
	  					copyArgs(prev->args+prev->num_args, cur->args, cur->num_args);
	  					prev->num_args += cur->num_args;
	  					prev->type = cs_rlinecurve;
	  					prev->next = next;
	  					free(cur); 
	  					cur = 0;
					}
      			}
      			break;
      			
    		case cs_vhcurveto:
      			y += cur->args[0];
      			UPDATE_BBOX(bbox, x, y);
      			x += cur->args[1]; 
      			y += cur->args[2];
      			UPDATE_BBOX(bbox, x, y);
      			x += cur->args[3];
      			UPDATE_BBOX(bbox, x, y);
      			if (TRY_COMPACT) 
      			{
					if ((prev->type == cs_hvcurveto && ((prev->num_args / 4) % 2) == 1) ||
	    				(prev->type == cs_vhcurveto && ((prev->num_args / 4) % 2) == 0)) 
	    			{
	  					copyArgs(prev->args+prev->num_args, cur->args, cur->num_args);
	  					prev->num_args += cur->num_args;
	  					prev->next = next;
	  					free(cur); 
	  					cur = 0;
					}
      			}
      			break;
      			
    		case cs_hvcurveto:
      			x += cur->args[0];
      			UPDATE_BBOX(bbox, x, y);
      			x += cur->args[1]; 
      			y += cur->args[2];
      			UPDATE_BBOX(bbox, x, y);
      			y += cur->args[3];
      			UPDATE_BBOX(bbox, x, y);
      			if (TRY_COMPACT) 
      			{
					if ((prev->type == cs_vhcurveto && ((prev->num_args / 4) % 2) == 1) ||
	    				(prev->type == cs_hvcurveto && ((prev->num_args / 4) % 2) == 0)) 
	    			{
	  					copyArgs(prev->args+prev->num_args, cur->args, cur->num_args);
	  					prev->num_args += cur->num_args;
	  					prev->next = next;
	  					free(cur); 
	  					cur = 0;
					}
      			}
      			break;
      			
    		case cs_flex:
      			for (int i = 0; i < 6; i++) 
      			{
					x += cur->args[2*i]; 
					y += cur->args[2*1+1];
					UPDATE_BBOX(bbox, x, y);
      			}
      			if (cur->args[12] == 50.0) 
      			{
					if (cur->args[1] == 0.0 && cur->args[11] == 0.0 &&
	    				cur->args[5] == 0.0 && cur->args[7] == 0.0 &&
	    				cur->args[3] + cur->args[9] == 0.0) 
	    			{
	  					/* cur->args[0] = cur->args[0];  dx1 */
	  					cur->args[1] = cur->args[2];  /* dx2 */
	  					cur->args[2] = cur->args[3];  /* dy2 */
	  					cur->args[3] = cur->args[4];  /* dx3 */
	  					cur->args[4] = cur->args[6];  /* dx4 */
	  					cur->args[5] = cur->args[8];  /* dx5 */
	  					cur->args[6] = cur->args[10]; /* dx6 */
	  					cur->num_args = 7;
	  					cur->type = cs_hflex;
					} 
					else if (cur->args[5] == 0.0 && cur->args[7] == 0.0 &&
		   					(cur->args[1] + cur->args[3] +
		    					cur->args[9] + cur->args[11]) == 0) 
		    		{
	  					/* cur->args[0] = cur->args[0];  dx1 */
	  					/* cur->args[1] = cur->args[1];  dy1 */
	  					/* cur->args[2] = cur->args[2];  dx2 */
	  					/* cur->args[3] = cur->args[3];  dy2 */
	  					/* cur->args[4] = cur->args[4];  dx3 */
	  					cur->args[5] = cur->args[6];  /* dx4 */
	  					cur->args[6] = cur->args[8];  /* dx5 */
	  					cur->args[7] = cur->args[9];  /* dy5 */
	  					cur->args[8] = cur->args[10]; /* dx6 */
	  					cur->num_args = 9;
	  					cur->type = cs_hflex1;
					}
      			}
      			break;
      			
    		case CS_HINT_DECL:
    		case cs_cntrmask:
      			/* noop */
      			break;
      			
    		default:
      			xwApp->error(tr("unexpected Type 2 charstring command.\n"));
      			return ;
      			break;
    	}
    	
    	if (cur != 0)
      		prev = cur;
    	cur = next;
	}
	
	if (bbox.llx > bbox.urx)
    	bbox.llx = bbox.urx = sbw.wx;
  	if (bbox.lly > bbox.ury)
    	bbox.lly = bbox.ury = sbw.wy;
}

long XWT1CharDesc::encodeCharPath(double default_width, 
	                              double nominal_width,
			                      uchar *dst, 
			                      uchar *endptr)
{
	uchar * save = dst;
  	T1CPath *curr = charpath;
  	RESET_STATE();
  	CLEARSTACK(); 
  	
  	if (sbw.wx != default_width) 
  	{
    	double wxA = sbw.wx - nominal_width;
    	putNumbers(&wxA, 1, &dst, endptr);
    	if (status != CS_PARSE_OK)
    		return -1;
  	}
  	
  	{
  		int num_hstems = 0, num_vstems = 0;
    	int reset = 1;
    	double stem[2];
    	for (int i = 0; i < num_stems && stems[i].dir == HSTEM; i++)
    	{
    		num_hstems++;
      		stem[0] = (reset ? (stems[i].pos) : (stems[i].pos - (stems[i-1].pos + stems[i-1].del)));
      		stem[1] = stems[i].del;
      		putNumbers(stem, 2, &dst, endptr);
      		if (status != CS_PARSE_OK)
    			return -1;
      		reset = 0;
      		if (2*num_hstems == CS_ARG_STACK_MAX) 
      		{
      			if (dst + 1 >= endptr)
      				return -1;
      				
				*dst++ = (uchar) ((flags & T1_CS_FLAG_USE_HINTMASK) ? cs_hstemhm : cs_hstem);
				reset = 1;
      		}
    	}
    	
    	if (reset == 0) 
    	{
    		if (dst + 1 >= endptr)
      			return -1;
      				
      		*dst++ = (uchar) ((flags & T1_CS_FLAG_USE_HINTMASK) ? cs_hstemhm : cs_hstem);
      	}
    	reset = 1;
    	
    	if (num_stems - num_hstems > 0)
    	{
    		for (int i = num_hstems; i < num_stems; i++) 
    		{
				num_vstems++;
				stem[0] = (reset ? (stems[i].pos) : (stems[i].pos - (stems[i-1].pos + stems[i-1].del)));
				stem[1] = stems[i].del;
				putNumbers(stem, 2, &dst, endptr);
				if (status != CS_PARSE_OK)
    				return -1;
				reset = 0;
				if (2*num_vstems == CS_ARG_STACK_MAX) 
				{
					if (dst + 1 >= endptr)
      					return -1;
	  				*dst++ = (uchar) ((flags & T1_CS_FLAG_USE_HINTMASK) ? cs_vstemhm : cs_vstem);
	  				reset = 1;
				}
      		}
      		
      		if (reset == 0) 
      		{
      			if (dst + 1 >= endptr)
      				return -1;
      				
				if ((flags & T1_CS_FLAG_USE_HINTMASK) || (flags & T1_CS_FLAG_USE_CNTRMASK)) 
				{
	  				if (curr->type != CS_HINT_DECL && curr->type != cs_cntrmask) 
	    				*dst++ = (uchar) cs_vstemhm;
				} 
				else 
	  				*dst++ = (uchar) cs_vstem;
      		}
    	}
  	}
  	
  	while (curr != NULL && curr->type != cs_endchar)
  	{
  		switch (curr->type)
  		{
  			case CS_HINT_DECL:
      			{
					uchar hintmask[(CS_STEM_ZONE_MAX+7)/8];
					memset(hintmask, 0, (num_stems+7)/8);
					while (curr != NULL && curr->type == CS_HINT_DECL) 
					{
	  					int stem_idx = getStem((int) curr->args[0]);
	  					hintmask[stem_idx/8] |= (1 << (7 - (stem_idx % 8)));
	  					curr = curr->next;
					}
					if (flags & T1_CS_FLAG_USE_HINTMASK) 
					{
						if ((dst+ ((num_stems+7)/8 + 1)) >= endptr)
      						return -1;
      				
	  					*dst++ = (uchar) cs_hintmask;
	  					memcpy(dst, hintmask, (num_stems+7)/8);
	  					dst += (num_stems+7)/8;
					}
      			}
      			break;
      			
    		case cs_cntrmask:
      			{
					uchar cntrmask[(CS_STEM_ZONE_MAX+7)/8];
					memset(cntrmask, 0, (num_stems+7)/8);
					for (int i = 0; i < curr->num_args; i++) 
					{
	  					int stem_idx = getStem((int) curr->args[i]);
	  					cntrmask[stem_idx/8] |= (1 << (7 - (stem_idx % 8)));
					}
					if ((dst+ ((num_stems+7)/8 + 1)) >= endptr)
      					return -1;
					*dst++ = (uchar) cs_cntrmask;
					memcpy(dst, cntrmask, (num_stems+7)/8);
					dst += (num_stems+7)/8;
					curr = curr->next;
      			}
      			break;
      			
    		case cs_rmoveto: 
    		case cs_hmoveto: 
    		case cs_vmoveto:
    		case cs_rlineto: 
    		case cs_hlineto: 
    		case cs_vlineto:
    		case cs_rrcurveto:  
    		case cs_hvcurveto: 
    		case cs_vhcurveto:
    		case cs_rlinecurve: 
    		case cs_rcurveline:
      			{
					putNumbers(curr->args, curr->num_args, &dst, endptr);
					if (status != CS_PARSE_OK)
    					return -1;
    				if (dst + 1 >= endptr)
      					return -1;
					*dst++ = (uchar) curr->type;
					curr = curr->next;
      			}
      			break;
      			
    		case cs_flex: 
    		case cs_hflex:
    		case cs_hflex1:
      			{
					putNumbers(curr->args, curr->num_args, &dst, endptr);
					if (status != CS_PARSE_OK)
    					return -1;
    				if (dst + 2 >= endptr)
      					return -1;
					*dst++ = (uchar) cs_escape;
					*dst++ = (uchar) curr->type;
					curr = curr->next;
      			}
      			break;
      			
    		default:
      			xwApp->error(tr("unknown Type 2 charstring command.\n"));
      			return -1;
      			break;
  		}
  	}
  	
  	if (flags & T1_CS_FLAG_USE_SEAC) 
  	{
    	double seacA[4];
    	seacA[0] = seac.adx;
    	seacA[1] = seac.ady;
    	seacA[2] = seac.bchar;
    	seacA[3] = seac.achar;
    	putNumbers(seacA, 4, &dst, endptr);
    	if (status != CS_PARSE_OK)
    		return -1;
    	if (dst + 2 >= endptr)
      		return -1;
    	xwApp->warning(tr("obsolete four arguments of \"endchar\" will be used for Type 1 \"seac\" operator.\n"));
  	}
  	
  	if (dst + 1 >= endptr)
      	return -1;
  	*dst++ = (uchar) cs_endchar;

  	return (long) (dst - save);
}

void XWT1CharDesc::sortStems()
{
	qsort(stems, num_stems, sizeof(T1Stem), stem_compare);
}

void XWT1CharDesc::addCharPath(int type, double *argv, int argn)
{
	T1CPath * p = (T1CPath*)malloc(sizeof(T1CPath));
  	p->type     = type;
  	p->num_args = argn;
  	p->next     = 0;
  	while (argn-- > 0)
    	p->args[argn] = argv[argn];

  	if (!charpath)
    	charpath = p;
  	if (lastpath)
    	lastpath->next = p;
  	lastpath = p;

  	if (type >= 0 && phase != T1_CS_PHASE_FLEX && IS_PATH_OPERATOR(type))
    	phase = T1_CS_PHASE_PATH;
}

void XWT1CharDesc::addPath(int t, int n)
{
	addCharPath(t, &(cs_arg_stack[cs_stack_top - n]), n);
}

int XWT1CharDesc::addStem(double pos, double del, int dir)
{
	pos += (dir == HSTEM) ? sbw.sby : sbw.sbx;
	int i = 0;
  	for (; i < num_stems; i++) 
  	{
    	if (stems[i].dir == dir &&
			stems[i].pos == pos &&
			stems[i].del == del)
      		break;
  	}
  	
  	if (i == num_stems) 
  	{
    	if (num_stems == CS_STEM_ZONE_MAX)
      		return -1;
      		
    	stems[i].dir = dir;
    	stems[i].pos = pos;
    	stems[i].del = del;
    	stems[i].id  = num_stems;
    	num_stems++;
  	}

  	return stems[i].id;
}

void XWT1CharDesc::copyArgs(double *args1, double *args2, int count)
{
	while (count-- > 0) 
	{
    	*args1 = *args2;
    	args1++; 
    	args2++;
  	}
}

void XWT1CharDesc::doCallOtherSubr()
{
	CHECKSTACK(2);
  	int subrno = (int) cs_arg_stack[--cs_stack_top];
  	int argn   = (int) cs_arg_stack[--cs_stack_top];

  	CHECKSTACK(argn);
  	if (ps_stack_top+argn > PS_ARG_STACK_MAX) 
  	{
    	status = CS_PARSE_ERROR;
    	return;
  	}
  	
  	while (argn-- > 0)
    	ps_arg_stack[ps_stack_top++] = cs_arg_stack[--cs_stack_top];

  	switch (subrno) 
  	{
  		case 0:  
  			doOtherSubr0() ; 
  			break;
  			
  		case 1:  
  			doOtherSubr1() ; 
  			break;
  			
  		case 2:  
  			doOtherSubr2() ; 
  			break;
  			
  		case 3:  
  			doOtherSubr3() ; 
  			break;
  			
  		case 12: 
  			doOtherSubr12(); 
  			break;
  			
  		case 13: 
  			doOtherSubr13(); 
  			break;
  			
  		default:
    		xwApp->error(tr("unknown othersubr.\n"));
    		break;
  	}
}

void XWT1CharDesc::doOperator1(uchar **data)
{
	uchar op = **data;
  	*data += 1;

  	switch (op)
  	{
  		case cs_closepath:
  			CLEARSTACK();
    		break;
    		
    	case cs_hsbw:
    		CHECKSTACK(2);
    		sbw.wx  = cs_arg_stack[--cs_stack_top];
    		sbw.wy  = 0;
    		sbw.sbx = cs_arg_stack[--cs_stack_top];
    		sbw.sby = 0;
    		CLEARSTACK();
    		break;
    		
    	case cs_hstem:
  		case cs_vstem:
    		CHECKSTACK(2);
    		{
      			int stem_id = addStem(cs_arg_stack[cs_stack_top-2], 
			 						  cs_arg_stack[cs_stack_top-1],
			 						  ((op == cs_hstem) ? HSTEM : VSTEM));
      			if (stem_id < 0) 
      			{
					xwApp->warning(tr("too many hints...\n"));
					status = CS_PARSE_ERROR;
					return;
      			}
      			cs_arg_stack[cs_stack_top++] = stem_id;
      			addPath(CS_HINT_DECL, 1);
    		}
    		CLEARSTACK();
    		break;
    		
    	case cs_rmoveto:
    		CHECKSTACK(2);
    		{
      			if (phase < T1_CS_PHASE_PATH) 
      			{
					cs_arg_stack[cs_stack_top-2] += sbw.sbx;
					cs_arg_stack[cs_stack_top-1] += sbw.sby;
      			}
      			addPath(op, 2);
    		}
    		CLEARSTACK();
    		break;
    		
    	case cs_hmoveto:
  		case cs_vmoveto:
    		CHECKSTACK(1);
    		{
      			int argn = 1;
      			if (phase < T1_CS_PHASE_PATH) 
      			{
					if (op == cs_hmoveto) 
					{
	  					cs_arg_stack[cs_stack_top-1] += sbw.sbx;
	  					if (sbw.sby != 0.0) 
	  					{
	    					cs_arg_stack[cs_stack_top++] = sbw.sby;
	    					argn = 2;
	    					op = cs_rmoveto;
	  					}
					} 
					else 
					{
	  					cs_arg_stack[cs_stack_top-1] += sbw.sby;
	  					if (sbw.sbx != 0.0) 
	  					{
	    					cs_arg_stack[cs_stack_top]   = cs_arg_stack[cs_stack_top-1];
	    					cs_arg_stack[cs_stack_top-1] = sbw.sbx;
	    					cs_stack_top++;
	    					argn = 2;
	    					op = cs_rmoveto;
	  					}
					}
      			}
      			addPath(op, argn);
    		}
    		CLEARSTACK();
    		break;
    		
  		case cs_endchar:
    		status = CS_CHAR_END;
    		CLEARSTACK();
    		break;
    		
  		case cs_rlineto:
    		CHECKSTACK(2);
    		addPath(op, 2);
    		CLEARSTACK();
    		break;
    		
  		case cs_hlineto:
  		case cs_vlineto:
    		CHECKSTACK(1);
    		addPath(op, 1);
    		CLEARSTACK();
    		break;
    		
  		case cs_rrcurveto:
    		CHECKSTACK(6);
    		addPath(op, 6);
    		CLEARSTACK();
    		break;
    		
  		case cs_vhcurveto:
  		case cs_hvcurveto:
    		CHECKSTACK(4);
    		addPath(op, 4);
    		CLEARSTACK();
    		break;
    		
  		case cs_return:
    		break;
    		
  		case cs_callsubr:
    		xwApp->error(tr("unexpected callsubr.\n"));
    		break;
    		
  		default:
    		xwApp->warning(tr("unknown charstring operator.\n"));
    		status = CS_PARSE_ERROR;
    		break;
  	}
}

void XWT1CharDesc::doOperator2(uchar **data, uchar *endptr)
{
	*data += 1;
	
	SRC_NEED(endptr, *data + 1);

  	uchar op = **data;
  	*data += 1;
  	switch(op)
  	{
  		case cs_sbw:
    		CHECKSTACK(4);
    		sbw.wy  = cs_arg_stack[--cs_stack_top];
    		sbw.wx  = cs_arg_stack[--cs_stack_top];
    		sbw.sby = cs_arg_stack[--cs_stack_top];
    		sbw.sbx = cs_arg_stack[--cs_stack_top];
    		CLEARSTACK();
    		break;
    		
    	case cs_hstem3:
  		case cs_vstem3:
  			CHECKSTACK(6);
  			{
  				for (int i = 2; i >= 0; i--) 
  				{
					int stem_id = addStem(cs_arg_stack[cs_stack_top-2*i-2],
			   							  cs_arg_stack[cs_stack_top-2*i-1],
			   							  ((op == cs_hstem3) ? HSTEM : VSTEM));
					if (stem_id < 0) 
					{
	  					xwApp->warning(tr("too many hints...\n"));
	  					status = CS_PARSE_ERROR;
	  					return;
					}
					cs_arg_stack[cs_stack_top++] = stem_id;
					addPath(CS_HINT_DECL, 1);
					cs_stack_top--;
      			}
  			}
  			CLEARSTACK();
  			break;
  			
  		case cs_setcurrentpoint:
    		CHECKSTACK(2);
    		CLEARSTACK();
    		break;
    		
    	case cs_pop:
    		if (ps_stack_top < 1) 
    		{
      			status = CS_PARSE_ERROR;
      			return;
    		}
    		LIMITCHECK(1);
    		cs_arg_stack[cs_stack_top++] = ps_arg_stack[--ps_stack_top];
    		break;
    		
    	case cs_dotsection:
    		break;
    		
    	case cs_div:
    		CHECKSTACK(2);
    		cs_arg_stack[cs_stack_top-2] /= cs_arg_stack[cs_stack_top-1];
    		cs_stack_top--;
    		break;
    		
  		case cs_callothersubr:
    		doCallOtherSubr();
    		break;
    		
  		case cs_seac:
    		CHECKSTACK(5);
    		flags |= T1_CS_FLAG_USE_SEAC;
    		seac.achar = (uchar) cs_arg_stack[--cs_stack_top];
    		seac.bchar = (uchar) cs_arg_stack[--cs_stack_top];
    		seac.ady   = cs_arg_stack[--cs_stack_top];
    		seac.adx   = cs_arg_stack[--cs_stack_top];
    		seac.ady += sbw.sby;
    		seac.adx += sbw.sbx - cs_arg_stack[--cs_stack_top];
    		CLEARSTACK();
    		break;
    		
  		default:
    		xwApp->warning(tr("unknown charstring operator.\n"));
    		status = CS_PARSE_ERROR;
    		break;
  	}
}

void XWT1CharDesc::doOtherSubr0()
{
	if (ps_stack_top < 1) 
	{
    	status = CS_PARSE_ERROR;
    	return;
  	}
  	
  	T1CPath * cur = charpath;
  	while (cur != 0 && cur->type != CS_FLEX_CTRL)
  		cur = cur->next;
  		
  	T1CPath * flex = cur;
  	{
    	cur = cur->next;
    	int i = 1;
    	for (; i < 7; i++) 
    	{
      		if (cur == NULL || cur->type != CS_FLEX_CTRL || cur->num_args != 2) 
      		{
				status = CS_PARSE_ERROR;
				return;
      		}
      		
      		if (i == 1) 
      		{
				flex->args[0] += cur->args[0];
				flex->args[1] += cur->args[1];
      		} 
      		else 
				copyArgs(&(flex->args[2*i-2]), cur->args, 2);
				
      		T1CPath * next = cur->next;
      		free(cur);
      		cur = next;
    	}
  	}
  	
  	if (cur != NULL) 
  	{
    	status = CS_PARSE_ERROR;
    	return;
  	}
  	
  	flex->type = cs_flex;
  	flex->args[12] = ps_arg_stack[--ps_stack_top]; /* flex depth */
  	flex->num_args = 13;
  	flex->next   = 0;
  	lastpath = flex;

  	phase = T1_CS_PHASE_PATH;
}

void XWT1CharDesc::doOtherSubr1()
{
	phase = T1_CS_PHASE_FLEX;
}

void XWT1CharDesc::doOtherSubr2()
{
	if (phase != T1_CS_PHASE_FLEX || !lastpath) 
	{
    	status = CS_PARSE_ERROR;
    	return;
  	}

  	switch (lastpath->type) 
  	{
  		case cs_rmoveto:
    		break;
    		
  		case cs_hmoveto:
    		lastpath->num_args = 2;
    		lastpath->args[1] = 0.0;
    		break;
    		
  		case cs_vmoveto:
    		lastpath->num_args = 2;
    		lastpath->args[1] = lastpath->args[0];
    		lastpath->args[0] = 0.0;
    		break;
    		
  		default:
    		status = CS_PARSE_ERROR;
    		return;
  	}
  	
  	lastpath->type = CS_FLEX_CTRL;
}

void XWT1CharDesc::doOtherSubr3()
{
	flags |= T1_CS_FLAG_USE_HINTMASK;
}

void XWT1CharDesc::doOtherSubr12()
{
	if (phase != T1_CS_PHASE_INIT) 
	{
    	status = CS_PARSE_ERROR;
    	return;
  	}
}

void XWT1CharDesc::doOtherSubr13()
{
	if (phase != T1_CS_PHASE_INIT) 
	{
    	status = CS_PARSE_ERROR;
    	return;
  	}
  	
  	T1StemGroup stemgroups[CS_STEM_GROUP_MAX];
  	
  	int n = 0;
  	for (; n < CS_STEM_GROUP_MAX; n++) 
    	stemgroups[n].num_stems = 0;
    	
    int num_hgroups = (int) ps_arg_stack[--ps_stack_top];
  	if (num_hgroups < 0 || num_hgroups > CS_STEM_GROUP_MAX) 
  	{
    	status = CS_PARSE_ERROR;
    	return;
  	}
  	
  	n = 0; 
  	double pos = 0.0;  	
  	while (ps_stack_top >= 2 && n < num_hgroups) 
  	{
    	pos += ps_arg_stack[--ps_stack_top];
    	double del  = ps_arg_stack[--ps_stack_top];
    	int stem_id = addStem((del < 0.0) ? pos + del : pos,
		       				 (del < 0.0) ? -del : del,
		       				 HSTEM);
    	stemgroups[n].stems[stemgroups[n].num_stems] = stem_id;
    	stemgroups[n].num_stems += 1;
    	pos += del;
    	if (del < 0.0) 
    	{
      		pos = 0.0;
      		n++;
    	}
  	}
  	
  	if (n != num_hgroups) 
  	{
    	status = CS_STACK_ERROR;
    	return;
  	}
  	
  	int num_vgroups = (int) ps_arg_stack[--ps_stack_top];
  	if (num_vgroups < 0 || num_vgroups > CS_STEM_GROUP_MAX) 
  	{
    	status = CS_PARSE_ERROR;
    	return;
  	}
  	
  	n = 0; 
  	pos = 0.0;
  	while (ps_stack_top >= 2 && n < num_vgroups) 
  	{
    	pos += ps_arg_stack[--ps_stack_top];
    	double del  = ps_arg_stack[--ps_stack_top];
    	int stem_id = addStem((del < 0.0) ? pos + del : pos,
		       				  (del < 0.0) ? -del : del,
		       				  VSTEM);
    	stemgroups[n].stems[stemgroups[n].num_stems] = stem_id;
    	stemgroups[n].num_stems += 1;
    	pos += del;
    	if (del < 0.0) 
    	{
      		pos = 0.0;
      		n++;
    	}
  	}
  	
  	if (n != num_vgroups) 
  	{
    	status = CS_STACK_ERROR;
    	return;
  	}

  	for (n = 0; n < qMax(num_hgroups, num_vgroups); n++) 
    	addCharPath(cs_cntrmask, stemgroups[n].stems, stemgroups[n].num_stems);

  	flags |= T1_CS_FLAG_USE_CNTRMASK;
}

void XWT1CharDesc::getInteger(uchar **data, uchar *endptr)
{
	long result = 0;
  	uchar b0 = **data;

  	*data += 1;

  	if (b0 == 28) 
  	{ /* shortint */
    	SRC_NEED(endptr, *data + 2);
    	uchar b1 = **data;
    	uchar b2 = *(*data+1);
    	result = b1*256+b2;
    	if (result > 0x7fff)
      		result -= 0x10000L;
    	*data += 2;
  	} 
  	else if (b0 >= 32 && b0 <= 246) 
  	{ /* int (1) */
    	result = b0 - 139;
  	} 
  	else if (b0 >= 247 && b0 <= 250) 
  	{ /* int (2) */
    	SRC_NEED(endptr, *data + 1);
    	uchar b1 = **data;
    	result = (b0-247)*256+b1+108;
    	*data += 1;
  	} 
  	else if (b0 >= 251 && b0 <= 254) 
  	{
    	SRC_NEED(endptr, *data + 1);
    	uchar b1 = **data;
    	result = -(b0-251)*256-b1-108;
    	*data += 1;
  	} 
  	else 
  	{
    	status = CS_PARSE_ERROR;
    	return;
  	}

  	LIMITCHECK(1);
  	cs_arg_stack[cs_stack_top++] = (double) result;
}

void XWT1CharDesc::getLongInt(uchar **data, uchar *endptr)
{
	*data += 1;
  	SRC_NEED(endptr, *data + 4);
  	long result = **data;
  	if (result >= 0x80L)
    	result -= 0x100L;
  	*data += 1;
  	for (int i = 1; i < 4; i++) 
  	{
    	result = result*256 + (**data);
    	*data += 1;
  	}

  	LIMITCHECK(1);
  	cs_arg_stack[cs_stack_top++] = (double) result;
}

int XWT1CharDesc::getStem(int stem_id)
{
	int i = 0;
  	for (; i < num_stems; i++) 
  	{
    	if (stems[i].id == stem_id)
      		break;
  	}

  	return ((i < num_stems) ? i : -1);
}

void XWT1CharDesc::putNumbers(double *argv, 
	                          int argn, 
	                          uchar **dest, 
	                          uchar *limit)
{
	for (int i = 0; i < argn; i++)
	{
		double value  = argv[i];
    	long ivalue = (long) floor(value+0.5);
    	if (value >= 0x8000L || value <= (-0x8000L - 1))
    	{
    		xwApp->error(tr("argument value too large. (This is bug)\n"));
    		return ;
    	}
    	else if (fabs(value - ivalue) > 3.0e-5)
    	{
    		DST_NEED(limit, *dest + 5);
      		*(*dest)++ = 255;
      		ivalue = (long) floor(value); /* mantissa */
      		*(*dest)++ = (ivalue >> 8) & 0xff;
      		*(*dest)++ = ivalue & 0xff;
      		ivalue = (long)((value - ivalue) * 0x10000l); /* fraction */
      		*(*dest)++ = (ivalue >> 8) & 0xff;
      		*(*dest)++ = ivalue & 0xff;
    	}
    	else if (ivalue >= -107 && ivalue <= 107)
    	{
    		DST_NEED(limit, *dest + 1);
      		*(*dest)++ = ivalue + 139;
    	}
    	else if (ivalue >= 108 && ivalue <= 1131) 
    	{
      		DST_NEED(limit, *dest + 2);
      		ivalue = 0xf700u + ivalue - 108;
      		*(*dest)++ = (ivalue >> 8) & 0xff;
      		*(*dest)++ = ivalue & 0xff;
    	} 
    	else if (ivalue >= -1131 && ivalue <= -108) 
    	{
      		DST_NEED(limit, *dest + 2);
      		ivalue = 0xfb00u - ivalue - 108;
      		*(*dest)++ = (ivalue >> 8) & 0xff;
      		*(*dest)++ = ivalue & 0xff;
    	} 
    	else if (ivalue >= -32768 && ivalue <= 32767) 
    	{ /* shortint */
      		DST_NEED(limit, *dest + 3);
      		*(*dest)++ = 28;
      		*(*dest)++ = (ivalue >> 8) & 0xff;
      		*(*dest)++ = (ivalue) & 0xff;
    	} 
    	else 
    	{ /* Shouldn't come here */
      		xwApp->error(tr("unexpected error.\n"));
      		return ;
    	}
	}
}

XWT1GInfo::XWT1GInfo()
{
	use_seac = 0;
	wx = 0.0;
	wy = 0.0;
	bbox.llx = 0.0;
    bbox.lly = 0.0;
    bbox.urx = 0.0;
    bbox.ury = 0.0;
    seac.adx = 0.0;
    seac.ady = 0.0;
    seac.bchar = 0;
    seac.achar = 0;
}

long XWT1GInfo::convertCharString(uchar *dst, 
	                              long dstlen,
			   			          uchar *src, 
			   			          long srclen, 
			   			          XWCFFIndex *subrs,
			   			          double default_width, 
			   			          double nominal_width)
{
	XWT1CharDesc t1char;
	RESET_STATE();
  	CLEARSTACK();
  	t1char.buildCharPath(&src, src+srclen, subrs);
  	t1char.doPostProc();
  	t1char.sortStems();
  	
  	long length = t1char.encodeCharPath(default_width, nominal_width, dst, dst+dstlen);
  	
  	wx = t1char.sbw.wx;
    wy = t1char.sbw.wy;
    bbox.llx = t1char.bbox.llx;
    bbox.lly = t1char.bbox.lly;
    bbox.urx = t1char.bbox.urx;
    bbox.ury = t1char.bbox.ury;
    if (t1char.flags & T1_CS_FLAG_USE_SEAC) 
    {
      	use_seac = 1;
      	seac.adx = t1char.seac.adx;
      	seac.ady = t1char.seac.ady;
      	seac.bchar = t1char.seac.bchar;
      	seac.achar = t1char.seac.achar;
    } 
    else
    	use_seac = 0;
    	
    return length;
}

int XWT1GInfo::getMetrics(uchar *src, 
	                      long srclen, 
	                      XWCFFIndex *subrs)
{
	XWT1CharDesc t1char;
	RESET_STATE();
  	CLEARSTACK();
  	t1char.buildCharPath(&src, src+srclen, subrs);
  	t1char.doPostProc();
  	{
    	wx = t1char.sbw.wx;
    	wy = t1char.sbw.wy;
    	bbox.llx = t1char.bbox.llx;
    	bbox.lly = t1char.bbox.lly;
    	bbox.urx = t1char.bbox.urx;
    	bbox.ury = t1char.bbox.ury;
    	if (t1char.flags & T1_CS_FLAG_USE_SEAC) 
    	{
      		use_seac = 1;
      		seac.adx = t1char.seac.adx;
      		seac.ady = t1char.seac.ady;
      		seac.bchar = t1char.seac.bchar;
      		seac.achar = t1char.seac.achar;
    	} 
    	else 
      		use_seac = 0;
  	}

  	return 0;
}

