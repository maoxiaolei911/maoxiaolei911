/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <math.h>
#include <QIODevice>
#include "psscantable.h"
#include "psbittable.h"
#include "XWPSError.h"
#include "XWPSCosObject.h"
#include "XWPSStream.h"
#include "XWPSColorSpace.h"
#include "XWPSDevicePDF.h"
#include "XWPSStreamState.h"

const PSStreamTemplate s_NullE_template = {
	0, s_Null_process, 1, 1, 0, 0, 0, s_create_Null_state
};

const PSStreamTemplate s_NullD_template = {
	0, s_Null_process, 1, 1, 0, 0, 0, s_create_Null_state
};

const PSStreamTemplate s_Null1D_template = {
    NULL, s_Null1D_process, 1, 1, 0,0,0, s_create_Null_state
};

const PSStreamTemplate s_A85D_template = {
	s_A85D_init, s_A85D_process, 2, 4, 0, 0, 0, s_create_A85D_state
};

const PSStreamTemplate s_A85E_template =
{
	s_A85E_init, s_A85E_process, 4, 6, 0, 0, 0, s_create_A85E_state
};

const PSStreamTemplate s_AXE_template = {
	s_AXE_init, s_AXE_process, 1, 3, 0, 0, 0, s_create_AXE_state
};

const PSStreamTemplate s_AXD_template = {
	s_AXD_init, s_AXD_process, 2, 1, 0, 0, 0, s_create_AXD_state
};

const PSStreamTemplate s_PSSD_template =
{
	s_PSSD_init, s_PSSD_process, 4, 1, 0, 0, 0, s_create_PSSD_state
};

const PSStreamTemplate s_PSSE_template = {
	0, s_PSSE_process, 1, 4, 0, 0, 0, s_create_Null_state
};

const PSStreamTemplate cos_write_stream_template = {
    0, cos_write_stream_process, 1, 1, 0, 0, 0, s_create_cos_write_stream_state
};

const PSStreamTemplate s_CFD_template =
{
	s_CFD_init, s_CFD_process, 1, 1, s_CFD_release, s_CFD_set_defaults, 0, s_create_CFD_state
};

const PSStreamTemplate s_CFE_template =
{
    s_CFE_init, s_CFE_process, 1, 1, s_CFE_release, s_CFE_set_defaults, 0, s_create_CFE_state
};

const PSStreamTemplate s_BHCD_template =
{
	s_BHCD_init, s_BHCD_process, 1, 1, s_BHCD_release,
 NULL, s_BHCD_reinit, s_create_BHCD_state
};

const PSStreamTemplate s_BHCE_template =
{
	s_BHCE_init, s_BHCE_process, 1, hc_bits_size >> 3, s_BHCE_release, 
	NULL, s_BHCE_reinit, s_create_BHCE_state
};

const PSStreamTemplate s_DCTD_template =
{
	s_DCTD_init, s_DCTD_process, 2000, 4000, s_DCTD_release,
 s_DCTD_set_defaults, 0, s_create_DCT_state
};


const PSStreamTemplate s_DCTE_template ={
	s_DCTE_init, s_DCTE_process, 1000, 4000, s_DCTE_release,
 s_DCTE_set_defaults, 0, s_create_DCT_state
};

const PSStreamTemplate s_zlibD_template = {
    s_zlibD_init, s_zlibD_process, 1, 1, s_zlibD_release,
    s_zlib_set_defaults, s_zlibD_reset, s_create_zlib_state
};


const PSStreamTemplate s_zlibE_template = {
    s_zlibE_init, s_zlibE_process, 1, 1, s_zlibE_release,
    s_zlib_set_defaults, s_zlibE_reset, s_create_zlib_state
};

const PSStreamTemplate s_IScale_template = {
    s_IScale_init, s_IScale_process, 1, 1,
    s_IScale_release, s_IScale_set_defaults,0, s_create_IScale_state
};

const PSStreamTemplate s_IIEncode_template = {
    s_IIEncode_init, s_IIEncode_process, 1, 1,
    s_IIEncode_release, 0, 0, s_create_IIEncode_state
};

const PSStreamTemplate s_LZWD_template =
{
	s_LZWD_init, s_LZWD_process, 3, 1, s_LZW_release,
 	s_LZW_set_defaults, s_LZWD_reset, s_create_LZW_state
};

const PSStreamTemplate s_LZWE_template = {
    s_LZWE_init, s_LZWE_process, 1, 2, NULL,
    s_LZW_set_defaults, s_LZWE_reset, s_create_LZW_state
};

const PSStreamTemplate s_PNGPD_template = {
    s_PNGPD_init, s_PNGPD_process, 1, 1, s_PNGP_release,
    s_PNGP_set_defaults, s_PNGP_reinit, s_create_PNGP_state
};

const PSStreamTemplate s_PNGPE_template = {
    s_PNGPE_init, s_PNGPE_process, 1, 1, s_PNGP_release,
    s_PNGP_set_defaults, s_PNGP_reinit, s_create_PNGP_state
};

const PSStreamTemplate s_RLE_template = {
    s_RLE_init, s_RLE_process, 129, 2, NULL,
    s_RLE_set_defaults, s_RLE_init, s_create_RLE_state
};

const PSStreamTemplate s_RLD_template = {
    s_RLD_init, s_RLD_process, 1, 1, NULL,
    s_RLD_set_defaults,NULL, s_create_RLD_state
};

const PSStreamTemplate s_1_8_template = {
    s_1_init, s_N_8_process, 1, 8, NULL, NULL,NULL, s_create_1248_state
};
const PSStreamTemplate s_2_8_template = {
    s_2_init, s_N_8_process, 1, 4, NULL, NULL,NULL, s_create_1248_state
};
const PSStreamTemplate s_4_8_template = {
    s_4_init, s_N_8_process, 1, 2, NULL, NULL,NULL, s_create_1248_state
};
const PSStreamTemplate s_12_8_template = {
    s_12_init, s_12_8_process, 1, 2, NULL, NULL,NULL, s_create_1248_state
};

const PSStreamTemplate s_8_1_template = {
    s_1_init, s_8_N_process, 8, 1, NULL, NULL,NULL, s_create_1248_state
};
const PSStreamTemplate s_8_2_template = {
    s_2_init, s_8_N_process, 4, 1, NULL, NULL,NULL, s_create_1248_state
};
const PSStreamTemplate s_8_4_template = {
    s_4_init, s_8_N_process, 2, 1, NULL, NULL,NULL, s_create_1248_state
};

const PSStreamTemplate s_C2R_template = {
    0 /*NULL */ , s_C2R_process, 4, 3, 0, s_C2R_set_defaults,NULL, s_create_C2R_state
};

const PSStreamTemplate s_IE_template = {
    s_IE_init, s_IE_process, 1, 1,
    0 /* NULL */, s_IE_set_defaults,NULL, s_create_IE_state
};

const PSStreamTemplate s_Subsample_template = {
    s_Subsample_init, s_Subsample_process, 4, 4,
    0 /* NULL */, s_Downsample_set_defaults,NULL, s_create_Subsample_state
};


const PSStreamTemplate s_Average_template = {
    s_Average_init, s_Average_process, 4, 4,
    s_Average_release, s_Average_set_defaults,NULL, s_create_Average_state
};

const PSStreamTemplate s_proc_read_template = {
    NULL, s_proc_read_process, 1, 1,
    NULL, s_proc_set_defaults,NULL, s_create_proc_state
};

const PSStreamTemplate s_proc_write_template = {
    NULL, s_proc_write_process, 1, 1,
    NULL, s_proc_set_defaults,NULL, s_create_proc_state
};

const PSStreamTemplate s_PFBD_template = {
    s_PFBD_init, s_PFBD_process, 6, 2,
    NULL, 
    NULL,NULL, s_create_PFBD_state
};

const PSStreamTemplate s_SFD_template = {
    s_SFD_init, s_SFD_process, 1, 1, 0, 
    s_SFD_set_defaults,NULL, s_create_SFD_state
};

const PSStreamTemplate s_exE_template = {
    NULL, s_exE_process, 1, 2,
    NULL,
    NULL,NULL, s_create_exE_state
};

const PSStreamTemplate s_exD_template = {
    s_exD_init, s_exD_process, 8, 200,
    NULL, s_exD_set_defaults, NULL, s_create_exD_state
};

const PSStreamTemplate s_PDiffE_template = {
    s_PDiffE_init, s_PDiff_process, 1, 1, NULL,
    s_PDiff_set_defaults, s_PDiff_reinit, s_create_PDiff_state
};

const PSStreamTemplate s_PDiffD_template = {
    s_PDiffD_init, s_PDiff_process, 1, 1, NULL,
    s_PDiff_set_defaults, s_PDiff_reinit, s_create_PDiff_state
};


const PSStreamTemplate s_BWBSE_template = {
    s_BWBSE_init, s_BWBSE_process, sizeof(int) * 2, 1,
    s_BWBS_release, s_BWBS_set_defaults, 0, s_create_BWBS_state
};

const PSStreamTemplate s_BWBSD_template = {
    s_BWBSD_init, s_BWBSD_process, 1, sizeof(int) * 2,
    s_BWBS_release, s_BWBS_set_defaults, 0, s_create_BWBS_state
};

const PSStreamTemplate s_BTE_template = {
     NULL, s_BT_process, 1, 1,NULL, NULL,NULL, s_create_BT_state
};

const PSStreamTemplate s_BTD_template = {
    NULL, s_BT_process, 1, 1,NULL, NULL,NULL, s_create_BT_state
};

const PSStreamTemplate s_MTFE_template = {
    s_MTF_init, s_MTFE_process, 1, 1,NULL, NULL,NULL, s_create_MTF_state
};

const PSStreamTemplate s_MTFD_template = {
    s_MTF_init, s_MTFD_process, 1, 1,
    NULL, NULL, s_MTF_init, s_create_MTF_state
};

const PSStreamTemplate s_BCPE_template =
{
	NULL, s_BCPE_process, 1, 2,NULL, NULL,NULL, s_create_Null_state
};

const PSStreamTemplate s_TBCPE_template =
{
	NULL, s_TBCPE_process, 1, 2,NULL, NULL,NULL, s_create_Null_state
};

const PSStreamTemplate s_BCPD_template =
{
	s_BCPD_init, s_BCPD_process, 1, 1,
 NULL, NULL, s_BCPD_init, s_create_BCPD_state
};

const PSStreamTemplate s_TBCPD_template =
{
	s_BCPD_init, s_TBCPD_process, 1, 1,
 NULL, NULL, s_BCPD_init, s_create_BCPD_state
};

const PSStreamTemplate s_MD5E_template = {
    s_MD5E_init, s_MD5E_process, 1, 16,NULL, NULL,NULL, s_create_MD5E_state
};

int filter_report_error(PSStreamState * st, const char *str)
{
	strncpy(st->error_string, str, STREAM_MAX_ERROR_STRING);
  st->error_string[STREAM_MAX_ERROR_STRING] = 0;
  return 0;
}

int  s_stdin_read_process(PSStreamState * st, 
                        PSStreamCursorRead * ,
		                    PSStreamCursorWrite * pw, 
		                    bool )
{
	XWPSStream * s = (XWPSStream *)st;
	QIODevice *file = s->file;
	if (file->atEnd())
		return EOFC;
		
  int wcount = (int)(pw->limit - pw->ptr);
  int count;

  if (wcount > 0) 
  {
	  wcount = 1;
		count = file->read((char*)(pw->ptr + 1), wcount);
		if (count < 0)
	    count = 0;
		pw->ptr += count;
  } 
  else
		count = 0;
		
  return (count == wcount ? 1 : 0);
}      

int s_Null_process(PSStreamState*, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool)
{
	return stream_move(pr, pw);
}

int s_Null1D_process(PSStreamState * , PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool )
{
	if (pr->ptr >= pr->limit)
		return 0;
  if (pw->ptr >= pw->limit)
		return 1;
  *++(pw->ptr) = *++(pr->ptr);
  return 1;
}

PSStreamState * s_create_Null_state()
{
	return new PSStreamState;
}

static int a85d_finish(int ccount, ulong wordA, PSStreamCursorWrite * pw)
{
	uchar *q = pw->ptr;
  int status = EOFC;

  switch (ccount) 
  {
		case 0:
	    break;
	    
		case 1:
	    status = ERRC;
	    break;
	    
		case 2:		/* 1 odd byte */
	    wordA = wordA * (85L * 85 * 85) + 0xffffffL;
	    goto o1;
	    
		case 3:		/* 2 odd bytes */
	    wordA = wordA * (85L * 85) + 0xffffL;
	    goto o2;
	    
		case 4:		/* 3 odd bytes */
	    wordA = wordA * 85 + 0xffL;
	    q[3] = (uchar) (wordA >> 8);
	    
o2:	    
			q[2] = (uchar) (wordA >> 16);
o1:	    
			q[1] = (uchar) (wordA >> 24);
	    q += ccount - 1;
	    pw->ptr = q;
  }
  return status;
}

int s_A85D_init(PSStreamState*st)
{
	PSStreamA85DState * ss = (PSStreamA85DState*)st;
	ss->min_left = 1;
	ss->word = 0;
	ss->odd = 0; return 0;
}

int s_A85D_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool last)
{
	PSStreamA85DState * ss = (PSStreamA85DState*)st;
	uchar *p = pr->ptr;
  uchar *q = pw->ptr;
  uchar *rlimit = pr->limit;
  uchar *wlimit = pw->limit;
  int ccount = ss->odd;
  ulong wordA = ss->word;
  int status = 0;

  while (p < rlimit) 
  {
		int ch = *++p;
		uint ccode = ch - '!';

		if (ccode < 85) 
		{
	    if (ccount == 4) 
	    {
				if (wlimit - q < 4) 
				{
		    	p--;
		    	status = 1;
		    	break;
				}
				wordA = wordA * 85 + ccode;
				q[1] = (uchar) (wordA >> 24);
				q[2] = (uchar) (wordA >> 16);
				q[3] = (uchar) ((uint) wordA >> 8);
				q[4] = (uchar) wordA;
				q += 4;
				wordA = 0;
				ccount = 0;
	    } 
	    else 
	    {
				wordA = wordA * 85 + ccode;
				++ccount;
	    }
		} 
		else if (ch == 'z' && ccount == 0) 
		{
	    if (wlimit - q < 4) 
	    {
				p--;
				status = 1;
				break;
	    }
	    q[1] = q[2] = q[3] = q[4] = 0,
			q += 4;
		} 
		else if (scan_char_decoder[ch] == ctype_space)
	    ;
		else if (ch == '~') 
		{
	    if (p == rlimit) 
	    {
				if (last)
		    	status = ERRC;
				else
		    	p--;
				break;
	    }
	    if ((int)(wlimit - q) < ccount - 1) 
	    {
				status = 1;
				p--;
				break;
	    }
	    if (*++p != '>') 
	    {
				status = ERRC;
				break;
	    }
	    pw->ptr = q;
	    status = a85d_finish(ccount, wordA, pw);
	    q = pw->ptr;
	    break;
		} 
		else 
		{		/* syntax error or exception */
	    status = ERRC;
	    break;
		}
  }
  pw->ptr = q;
  if (status == 0 && last) 
  {
		if ((int)(wlimit - q) < ccount - 1)
	    status = 1;
		else
	    status = a85d_finish(ccount, wordA, pw);
  }
  pr->ptr = p;
  ss->odd = ccount;
  ss->word = wordA;
  return status;
}

PSStreamState* s_create_A85D_state()
{
	return (PSStreamState* )(new PSStreamA85DState);
}

int s_A85E_init(PSStreamState*st)
{
	PSStreamA85EState * ss = (PSStreamA85EState*)st;
	ss->count = 0;
	ss->last_char = '\n'; return 0;
}

#define LINE_LIMIT 79
int s_A85E_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool last)
{
	PSStreamA85EState * ss = (PSStreamA85EState*)st;
	uchar *p = pr->ptr;
  uchar *q = pw->ptr;
  uchar *qn = q + (LINE_LIMIT - ss->count);
  uchar *rlimit = pr->limit;
  uchar *wlimit = pw->limit;
  int status = 0;
  int prev = ss->last_char;
  int countA;

  for (; (countA = rlimit - p) >= 4; p += 4) 
  {
		ulong word = ((ulong) (((uint) p[1] << 8) + p[2]) << 16) + (((uint) p[3] << 8) + p[4]);
		if (word == 0) 
		{
	    if (q >= qn) 
	    {
				if (wlimit - q < 2) 
				{
		    	status = 1;
		    	break;
				}
				*++q = prev = '\n';
				qn = q + LINE_LIMIT;			
	    } 
	    else 
	    {
				if (q >= wlimit) 
				{
		    	status = 1;
		    	break;
				}
	    }
	    *++q = prev = 'z';
		} 
		else 
		{
	    ulong v4 = word / 85;	
	    ulong v3 = v4 / 85;	
	    uint v2 = v3 / 85;
	    uint v1 = v2 / 85;

put:	    
			if (q + 5 > qn) 
			{
				if (q >= wlimit) 
				{
		    	status = 1;
		    	break;
				}
				*++q = prev = '\n';
				qn = q + LINE_LIMIT;
				goto put;
	    }
	    if (wlimit - q < 5) 
	    {
				status = 1;
				break;
	    }
	    q[1] = (uchar) v1 + '!';
	    q[2] = (uchar) (v2 - v1 * 85) + '!';
	    q[3] = (uchar) ((uint) v3 - v2 * 85) + '!';
	    q[4] = (uchar) ((uint) v4 - (uint) v3 * 85) + '!';
	    q[5] = (uchar) ((uint) word - (uint) v4 * 85) + '!';
	    
	    if (q[1] == '%') 
	    {
				if (prev == '%') 
				{
		    	if (qn - q == LINE_LIMIT - 1) 
		    	{
						*++q = prev = '\n';
						qn = q + LINE_LIMIT;
						goto put;
		    	}
				} 
				else if (prev == '\n' && (q[2] == '%' || q[2] == '!')) 
				{
		    	int extra = (q[2] == '!' ? 1 : 
			 								q[3] == '!' ? 2 :
			 								q[3] != '%' ? 1 :
			 								q[4] == '!' ? 3 :
			 								q[4] != '%' ? 2 :
			 								q[5] == '!' ? 4 :
			 								q[5] != '%' ? 3 : 4);

		    	if (wlimit - q < 5 + extra) 
		    	{
						status = 1;
						break;
		    	}
		    	
		    	switch (extra) 
		    	{
						case 4:
			    		q[9] = q[5], q[8] = '\n';
			    		goto e3;
						case 3:
			    		q[8] = q[5];
			  			e3:q[7] = q[4], q[6] = '\n';
			    		goto e2;
						case 2:
			    		q[7] = q[5], q[6] = q[4];
e2:
							q[5] = q[3], q[4] = '\n';
			    		goto e1;
						case 1:
			    		q[6] = q[5], q[5] = q[4], q[4] = q[3];
e1:
							q[3] = q[2], q[2] = '\n';
		    	}
		    	qn = q + 2 * extra + LINE_LIMIT;
		    	q += extra;
				}
	    } 
	    else if (q[1] == '!' && prev == '%' && qn - q == LINE_LIMIT - 1) 
	    {
				*++q = prev = '\n';
				qn = q + LINE_LIMIT;
				goto put;
	    }
	    prev = *(q += 5);
		}
  }
  
end:
  ss->count = LINE_LIMIT - (qn - q);
  if (last && status == 0 && countA < 4) 
  {
		int nchars = (countA == 0 ? 2 : countA + 3);
		if (wlimit - q < nchars)
	    status = 1;
		else if (q + nchars > qn) 
		{
	    *++q = '\n';
	    qn = q + LINE_LIMIT;
	    goto end;
		}
		else 
		{
	    ulong word = 0;
	    ulong divisor = 85L * 85 * 85 * 85;

	    switch (countA) 
	    {
				case 3:
		    	word += (uint) p[3] << 8;
				case 2:
		    	word += (ulong) p[2] << 16;
				case 1:
		    	word += (ulong) p[1] << 24;
		    	p += countA;
		    	while (countA-- >= 0) 
		    	{
						ulong v = word / divisor; 
						*++q = (uchar) v + '!';
						word -= v * divisor;
						divisor /= 85;
		    	}
		    /*case 0: */
	    }
	    *++q = '~';
	    *++q = '>';
		}
  }
  pr->ptr = p;
  if (q > pw->ptr)
		ss->last_char = *q;
  pw->ptr = q;
  return status;
}
#undef LINE_LIMIT

PSStreamState* s_create_A85E_state()
{
	return (PSStreamState* )(new PSStreamA85EState);
}

int s_AXE_init(PSStreamState*st)
{
	PSStreamAXEState * ss = (PSStreamAXEState*)st;
	ss->EndOfData = true;
	ss->count = 0; return 0;
}

int s_AXE_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool last)
{
	PSStreamAXEState * ss = (PSStreamAXEState*)st;
	uchar *p = pr->ptr;
  uchar *q = pw->ptr;
  int rcount = pr->limit - p;
  int wcount = pw->limit - q;
  int countA = 0;
  int pos = ss->count;
  const char *hex_digits = "0123456789ABCDEF";
  int status = 0;

  if (last && ss->EndOfData)
		wcount--;
  wcount -= (wcount + 64) / 65;	
  wcount >>= 1;	
  countA = (wcount < rcount ? (status = 1, wcount) : rcount);
  while (--countA >= 0) 
  {
		*++q = hex_digits[*++p >> 4];
		*++q = hex_digits[*p & 0xf];
		if (!(++pos & 31) && (countA != 0 || !last))
	    *++q = '\n';
  }
  if (last && status == 0 && ss->EndOfData)
		*++q = '>';
  pr->ptr = p;
  pw->ptr = q;
  ss->count = pos & 31;
  return status;
}

PSStreamState* s_create_AXE_state()
{
	return (PSStreamState* )(new PSStreamAXEState);
}

int s_AXD_init(PSStreamState*st)
{
	PSStreamAXDState * ss = (PSStreamAXDState*)st;
	ss->min_left = 1;
	ss->odd = -1; return 0;
}

int s_AXD_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool last)
{
	PSStreamAXDState * ss = (PSStreamAXDState*)st;
	int code = s_hex_process(pr, pw, &ss->odd, hex_ignore_whitespace);
  switch (code) 
  {
		case 0:
	    if (ss->odd >= 0 && last) 
	    {
				if (pw->ptr == pw->limit)
		    	return 1;
				*++(pw->ptr) = ss->odd << 4;
	    }
	    
		case 1:
	    for (; pr->ptr < pr->limit; pr->ptr++)
				if (scan_char_decoder[pr->ptr[1]] != ctype_space) 
				{
		    	if (pr->ptr[1] == '>') 
		    	{
						pr->ptr++;
						goto eod;
		    	}
		    	return 1;
				}
	    	return 0;
	    	
		default:
	    return code;
	    
		case ERRC:
	    ;
  }
  if (*pr->ptr != '>') 
  {
		--(pr->ptr);
		return ERRC;
  }

eod:
	if (ss->odd >= 0) 
	{
		if (pw->ptr == pw->limit)
	    return 1;
		*++(pw->ptr) = ss->odd << 4;
  }
  return EOFC;
}

PSStreamState * s_create_AXD_state()
{
	return (PSStreamState*)(new PSStreamAXDState);
}

int s_PSSE_process(PSStreamState*, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool last)
{
	uchar *p = pr->ptr;
  uchar *rlimit = pr->limit;
  uchar *q = pw->ptr;
  uchar *wlimit = pw->limit;
  int status = 0;
  while (p < rlimit) 
  {
		int c = *++p;
		if (c < 32 || c >= 127) 
		{
	    const char *pesc;
	    const char *const esc = "\n\r\t\b\f";

	    if (c < 32 && c != 0 && (pesc = strchr(esc, c)) != 0) 
	    {
				if (wlimit - q < 2) 
				{
		    	--p;
		    	status = 1;
		    	break;
				}
				*++q = '\\';
				*++q = "nrtbf"[pesc - esc];
				continue;
	    }
	    if (wlimit - q < 4) 
	    {
				--p;
				status = 1;
				break;
	    }
	    q[1] = '\\';
	    q[2] = (c >> 6) + '0';
	    q[3] = ((c >> 3) & 7) + '0';
	    q[4] = (c & 7) + '0';
	    q += 4;
	    continue;
		} 
		else if (c == '(' || c == ')' || c == '\\') 
		{
	    if (wlimit - q < 2) 
	    {
				--p;
				status = 1;
				break;
	    }
	    *++q = '\\';
		} 
		else 
		{
	    if (q == wlimit) 
	    {
				--p;
				status = 1;
				break;
	    }
		}
		*++q = c;
  }
  if (last && status == 0) 
  {
		if (q == wlimit)
	    status = 1;
		else
	    *++q = ')';
  }
  pr->ptr = p;
  pw->ptr = q;
  return status;
}

int s_PSSD_init(PSStreamState*st)
{
	PSStreamPSSDState * ss = (PSStreamPSSDState*)st;
	ss->from_string = false;
	ss->depth = 0; return 0;
}

int s_PSSD_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool last)
{
	PSStreamPSSDState * ss = (PSStreamPSSDState*)st;
	uchar *p = pr->ptr;
  uchar *rlimit = pr->limit;
  uchar *q = pw->ptr;
  uchar *wlimit = pw->limit;
  int status = 0;
  int c;

#define check_p(n)\
  if ( p == rlimit ) { p -= n; goto out; }
#define check_q(n)\
  if ( q == wlimit ) { p -= n; status = 1; goto out; }
    
  while (p < rlimit) 
  {
		c = *++p;
		if (c == '\\' && !ss->from_string) 
		{
	    check_p(1);
	    switch ((c = *++p)) 
	    {
				case 'n':
		    	c = '\n';
		    	goto put;
		    	
				case 'r':
		    	c = '\r';
		    	goto put;
		    	
				case 't':
		    	c = '\t';
		    	goto put;
		    	
				case 'b':
		    	c = '\b';
		    	goto put;
		    	
				case 'f':
		    	c = '\f';
		    	goto put;
		    	
				default:
		  
put:
					check_q(2);
		    	*++q = c;
		    	continue;
		    	
				case char_CR:
		    	check_p(2);
		    	if (p[1] == char_EOL)
						p++;
		    	continue;
		    	
				case char_EOL:	/* ignore */
		    	continue;
		    	
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
		    	{
						check_p(2);
						int d = p[1];
						c -= '0';
						if (d >= '0' && d <= '7') 
						{
			    		if (p + 1 == rlimit) 
			    		{
								p -= 2;
								goto out;
			    		}
			    		check_q(2);
			    		c = (c << 3) + d - '0';
			    		d = p[2];
			    		if (d >= '0' && d <= '7') 
			    		{
								c = (c << 3) + d - '0';
								p += 2;
			    		} 
			    		else
								p++;
						} 
						else
			    		check_q(2);
						*++q = c;
						continue;
		    	}
	    	}
			} 
			else
	    	switch (c) 
	    	{
					case '(':
		    		check_q(1);
		    		ss->depth++;
		    		break;
		    		
					case ')':
		    		if (ss->depth == 0) 
		    		{
							status = EOFC;
							goto out;
		    		}
		    		check_q(1);
		    		ss->depth--;
		   	 		break;
		   	 		
					case char_CR:
		    		check_p(1);
		    		check_q(1);
		    		if (p[1] == char_EOL)
							p++;
		    		*++q = '\n';
		    		continue;
		    		
					case char_EOL:
		    		c = '\n';
					default:
		    		check_q(1);
		    		break;
	    	}
				*++q = c;
    	}
#undef check_p
#undef check_q
  
out:
	pr->ptr = p;
  pw->ptr = q;
  if (last && status == 0 && p != rlimit)
		status = ERRC;
  return status;
}

PSStreamState* s_create_PSSD_state()
{
	return (PSStreamState*)(new PSStreamPSSDState);
}

int cos_write_stream_process(PSStreamState * st, 
                             PSStreamCursorRead * pr, 
                             PSStreamCursorWrite * , 
                             bool )
{
	uint count = pr->limit - pr->ptr;
  PSCosWriteStreamState *ss = (PSCosWriteStreamState *)st;
  XWPSDevicePDF *pdev = ss->pdev;
  XWPSStream *target = ss->target;
  long start_pos = pdev->streams->strm->tell();
  int code;

  target->write(pr->ptr + 1, count);
  pr->ptr = pr->limit;
  target->flush();
  code = ss->pcs->streamAddSince(start_pos);
  return (code < 0 ? ERRC : 0);
}

PSStreamState * s_create_cos_write_stream_state()
{
	return (PSStreamState*)(new PSCosWriteStreamState);
}

static void dcte_init_destination(j_compress_ptr )
{
}

static boolean dcte_empty_output_buffer(j_compress_ptr )
{
    return false;
}

static void dcte_term_destination(j_compress_ptr )
{
}

static void ps_jpeg_error_exit(j_common_ptr )
{
}

static void ps_jpeg_emit_message(j_common_ptr , int )
{
}

static void
dctd_init_source(j_decompress_ptr )
{
}

static const JOCTET fake_eoi[2] =
{0xFF, JPEG_EOI};

static boolean
dctd_fill_input_buffer(j_decompress_ptr dinfo)
{
  PSJPEGDecompressData *jddp = (PSJPEGDecompressData *)((char *)dinfo - offset_of_(PSJPEGDecompressData, dinfo));

  if (!jddp->input_eod)
		return FALSE;	
		
//  WARNMS(dinfo, JWRN_JPEG_EOF);
  
  dinfo->src->next_input_byte = fake_eoi;
  dinfo->src->bytes_in_buffer = 2;
  jddp->faked_eoi = true;	
  return true;
}

static void
dctd_skip_input_data(j_decompress_ptr dinfo, long num_bytes)
{
  struct jpeg_source_mgr *src = dinfo->src;
  PSJPEGDecompressData *jddp = (PSJPEGDecompressData *)((char *)dinfo -offset_of_(PSJPEGDecompressData, dinfo));

  if (num_bytes > 0) 
  {
		if (num_bytes > src->bytes_in_buffer) 
		{
	    jddp->skip += num_bytes - src->bytes_in_buffer;
	    src->next_input_byte += src->bytes_in_buffer;
	    src->bytes_in_buffer = 0;
	    return;
		}
		
		src->next_input_byte += num_bytes;
		src->bytes_in_buffer -= num_bytes;
  }
}

static void
dctd_term_source(j_decompress_ptr )
{
}

void s_DCT_set_defaults(PSStreamState * st)
{
	PSStreamDCTState *const ss = (PSStreamDCTState *) st;
  ss->data.common = 0;
  ss->ColorTransform = -1;
  ss->QFactor = 1.0;
  ss->Markers.data = 0;
  ss->Markers.size = 0;
  ss->scan_line_size = 1;
  ss->phase = 5;
}

int s_DCTD_init(PSStreamState * st)
{
	PSStreamDCTState *const ss = (PSStreamDCTState *) st;
  struct jpeg_source_mgr *src = &ss->data.decompress->source;

  src->init_source = dctd_init_source;
  src->fill_input_buffer = dctd_fill_input_buffer;
  src->skip_input_data = dctd_skip_input_data;
  src->term_source = dctd_term_source;
  src->resync_to_restart = jpeg_resync_to_restart;	
  ss->data.decompress->dinfo.src = src;
  ss->data.decompress->skip = 0;
  ss->data.decompress->input_eod = false;
  ss->data.decompress->faked_eoi = false;
  ss->phase = 0;
  return 0;
}

int s_DCTD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last)
{
	PSStreamDCTState *const ss = (PSStreamDCTState *) st;
  PSJPEGDecompressData *jddp = ss->data.decompress;
  struct jpeg_source_mgr *src = jddp->dinfo.src;
  int code;

  if (jddp->skip != 0) 
  {
		long avail = pr->limit - pr->ptr;

		if (avail < jddp->skip) 
		{
	    jddp->skip -= avail;
	    pr->ptr = pr->limit;
	    if (!last)
				return 0;	
	    jddp->skip = 0;	
		}
		pr->ptr += jddp->skip;
		jddp->skip = 0;
  }
  src->next_input_byte = pr->ptr + 1;
  src->bytes_in_buffer = pr->limit - pr->ptr;
  jddp->input_eod = last;
  switch (ss->phase) 
  {
		case 0:	
	    while (pr->ptr < pr->limit && pr->ptr[1] != 0xff)
				pr->ptr++;
	    if (pr->ptr == pr->limit)
				return 0;
	    src->next_input_byte = pr->ptr + 1;
	    src->bytes_in_buffer = pr->limit - pr->ptr;
	    ss->phase = 1;
	    
		case 1:	
	    if ((code = jpeg_read_header(&(ss->data.decompress->dinfo), true)) < 0)
				return ERRC;
	    pr->ptr =	(uchar*)(jddp->faked_eoi ? pr->limit : src->next_input_byte - 1);
	    switch (code) 
	    {
				case JPEG_SUSPENDED:
		    	return 0;
	    }
	    
	    if (ss->ColorTransform >= 0 &&	!jddp->dinfo.saw_Adobe_marker) 
	    {
				switch (jddp->dinfo.num_components) 
				{
		    	case 3:
						jddp->dinfo.jpeg_color_space =  (ss->ColorTransform ? JCS_YCbCr : JCS_RGB);
						break;
					
		    	case 4:
						jddp->dinfo.jpeg_color_space = (ss->ColorTransform ? JCS_YCCK : JCS_CMYK);
						break;
				}
	    }
	    ss->phase = 2;
	    
		case 2:	
	    if ((code = jpeg_start_decompress(&ss->data.decompress->dinfo)) < 0)
				return ERRC;
	    pr->ptr =	(uchar*)(jddp->faked_eoi ? pr->limit : src->next_input_byte - 1);
	    if (code == 0)
				return 0;
	    ss->scan_line_size =jddp->dinfo.output_width * jddp->dinfo.output_components;
	    if (ss->scan_line_size > (uint) jddp->templat.min_out_size) 
	    {
				jddp->scanline_buffer = new uchar[ss->scan_line_size];
				if (jddp->scanline_buffer == NULL)
		    	return ERRC;
	    }
	    jddp->bytes_in_scanline = 0;
	    ss->phase = 3;
	    
		case 3:	
	  	
dumpbuffer:
	    if (jddp->bytes_in_scanline != 0) 
	    {
				uint avail = pw->limit - pw->ptr;
				uint tomove = qMin(jddp->bytes_in_scanline, avail);
				memcpy(pw->ptr + 1, jddp->scanline_buffer + (ss->scan_line_size - jddp->bytes_in_scanline), tomove);
				pw->ptr += tomove;
				jddp->bytes_in_scanline -= tomove;
				if (jddp->bytes_in_scanline != 0)
		    	return 1;	
	    }
	    while (jddp->dinfo.output_height > jddp->dinfo.output_scanline) 
	    {
				int read;
				uchar *samples;

				if (jddp->scanline_buffer != NULL)
		    	samples = jddp->scanline_buffer;
				else 
				{
		    	if ((uint) (pw->limit - pw->ptr) < ss->scan_line_size)
						return 1;	
		    	samples = pw->ptr + 1;
				}
				read = jpeg_read_scanlines(&ss->data.decompress->dinfo, &samples, (JDIMENSION)1);
				if (read < 0)
		    	return ERRC;
				pr->ptr = (uchar*)(jddp->faked_eoi ? pr->limit : src->next_input_byte - 1);
				if (!read)
		    	return 0;
				if (jddp->scanline_buffer != NULL) 
				{
		    	jddp->bytes_in_scanline = ss->scan_line_size;
		    	goto dumpbuffer;
				}
				pw->ptr += ss->scan_line_size;
	    }
	    ss->phase = 4;
	    
		case 4:	
	    if ((code = jpeg_finish_decompress(&ss->data.decompress->dinfo)) < 0)
				return ERRC;
	    pr->ptr =	(uchar*)(jddp->faked_eoi ? pr->limit : src->next_input_byte - 1);
	    if (code == 0)
				return 0;
	    ss->phase = 5;
	    
		case 5:	
	    return EOFC;
  }
  
  return ERRC;
}

void s_DCTD_release(PSStreamState * st)
{
	PSStreamDCTState *const ss = (PSStreamDCTState *) st;

  jpeg_destroy((j_common_ptr) &ss->data.compress->cinfo);
  if (ss->data.decompress->scanline_buffer != NULL)
  {
  	delete [] ss->data.decompress->scanline_buffer;
  	ss->data.decompress->scanline_buffer = 0;
  }
	if (ss->data.compress)
  {
  	free(ss->data.compress);
  	ss->data.compress = 0;
  }
  
  st->templat = &s_DCTD_template;
}

void s_DCTD_set_defaults(PSStreamState * st)
{
	s_DCT_set_defaults(st);
}

int s_DCTE_init(PSStreamState*st)
{
	PSStreamDCTState *const ss = (PSStreamDCTState *) st;
  struct jpeg_destination_mgr *dest = &ss->data.compress->destination;

  dest->init_destination = dcte_init_destination;
  dest->empty_output_buffer = dcte_empty_output_buffer;
  dest->term_destination = dcte_term_destination;
  ss->data.compress->cinfo.dest = dest;
  ss->phase = 0;
  return 0;
}

int s_DCTE_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool last)
{
	PSStreamDCTState *const ss = (PSStreamDCTState *) st;
	PSJPEGCompressData *jcdp = ss->data.compress;
  struct jpeg_destination_mgr *dest = jcdp->cinfo.dest;

  dest->next_output_byte = pw->ptr + 1;
  dest->free_in_buffer = pw->limit - pw->ptr;
  switch (ss->phase) 
  {
		case 0:
	    jpeg_start_compress(&(ss->data.compress->cinfo), true);
	    pw->ptr = dest->next_output_byte - 1;
	    ss->phase = 1;
	    
		case 1:	
	    if (pw->limit - pw->ptr < ss->Markers.size)
				return 1;
	    memcpy(pw->ptr + 1, ss->Markers.data, ss->Markers.size);
	    pw->ptr += ss->Markers.size;
	    ss->phase = 2;
	    
		case 2:	
	    if (!ss->NoMarker) 
	    {
				static const uchar Adobe[] =
				{
		    	0xFF, JPEG_APP0 + 14, 0, 14,	/* parameter length */
		    	'A', 'd', 'o', 'b', 'e',
		    	0, 100,	/* Version */
		    	0, 0,	/* Flags0 */
		    	0, 0,	/* Flags1 */
		    	0		/* ColorTransform */
				};

#define ADOBE_MARKER_LEN sizeof(Adobe)
				if (pw->limit - pw->ptr < ADOBE_MARKER_LEN)
		    	return 1;
				memcpy(pw->ptr + 1, Adobe, ADOBE_MARKER_LEN);
				pw->ptr += ADOBE_MARKER_LEN;
				*pw->ptr = ss->ColorTransform;
#undef ADOBE_MARKER_LEN
	    }
	    dest->next_output_byte = pw->ptr + 1;
	    dest->free_in_buffer = pw->limit - pw->ptr;
	    ss->phase = 3;
	    
		case 3:
	    while (jcdp->cinfo.image_height > jcdp->cinfo.next_scanline) 
	    {
				uchar *samples = (uchar *) (pr->ptr + 1);
				if ((uint) (pr->limit - pr->ptr) < ss->scan_line_size) 
				{
		    	if (last)
						return ERRC;
		    	return 0;	/* need more data */
				}
				int written = (int)jpeg_write_scanlines(&ss->data.compress->cinfo, (JSAMPARRAY)&samples, (JDIMENSION)1);
				if (written < 0)
		    	return ERRC;
				pw->ptr = dest->next_output_byte - 1;
				if (!written)
		    	return 1;
				pr->ptr += ss->scan_line_size;
	    }
	    ss->phase = 4;
	    
		case 4:	
	    dest->next_output_byte = jcdp->finish_compress_buf;
	    dest->free_in_buffer = sizeof(jcdp->finish_compress_buf);
	    jpeg_finish_compress(&ss->data.compress->cinfo);
	    jcdp->fcb_size =dest->next_output_byte - jcdp->finish_compress_buf;
	    jcdp->fcb_pos = 0;
	    ss->phase = 5;
	    
		case 5:
	    if (jcdp->fcb_pos < jcdp->fcb_size) 
	    {
				int count = qMin(jcdp->fcb_size - jcdp->fcb_pos,	pw->limit - pw->ptr);
				memcpy(pw->ptr + 1, jcdp->finish_compress_buf + jcdp->fcb_pos, count);
				jcdp->fcb_pos += count;
				pw->ptr += count;
				if (jcdp->fcb_pos < jcdp->fcb_size)
		    	return 1;
	    }
	    return EOFC;
  }
  
  return ERRC;
}

void s_DCTE_release(PSStreamState * st)
{
	PSStreamDCTState *const ss = (PSStreamDCTState *) st;

  jpeg_destroy((j_common_ptr) & ss->data.compress->cinfo);
  if (ss->data.compress)
  {
  	free(ss->data.compress);
  	ss->data.compress = 0;
  }
  
  st->templat = &s_DCTE_template;
}

void s_DCTE_set_defaults(PSStreamState * st)
{
	PSStreamDCTState *const ss = (PSStreamDCTState *) st;
  s_DCT_set_defaults(st);
  ss->QFactor = 1.0;
  ss->ColorTransform = 0;
  ss->Markers.data = 0;
  ss->Markers.size = 0;
  ss->NoMarker = true;
}

PSStreamState * s_create_DCT_state()
{
	return (PSStreamState*)(new PSStreamDCTState);
}

void ps_jpeg_error_setup(PSStreamDCTState * st)
{
	struct jpeg_error_mgr *err = &st->data.common->err;
	jpeg_std_error(err);
	err->error_exit = ps_jpeg_error_exit;
  err->emit_message = ps_jpeg_emit_message;
  st->data.compress->cinfo.err = err;
}

int  ps_jpeg_log_error(PSStreamDCTState * st)
{
	j_common_ptr cinfo = (j_common_ptr) & st->data.compress->cinfo;
  char buffer[JMSG_LENGTH_MAX];

  (*cinfo->err->format_message) (cinfo, buffer);
  return (int)(XWPSError::IOError);
}

int ps_jpeg_create_compress(PSStreamDCTState * st)
{
	ps_jpeg_error_setup(st);
	jpeg_create_compress(&st->data.compress->cinfo);
  jpeg_stream_data_common_init(st->data.compress);
  st->data.compress->Picky = 0;
  st->data.compress->Relax = 0;
  return 0;
}

int ps_jpeg_set_defaults(PSStreamDCTState * st)
{
	jpeg_set_defaults(&st->data.compress->cinfo);
  return 0;
}

int ps_jpeg_set_colorspace(PSStreamDCTState * st, J_COLOR_SPACE colorspace)
{
	jpeg_set_colorspace(&st->data.compress->cinfo, colorspace);
  return 0;
}

int ps_jpeg_set_linear_quality(PSStreamDCTState * st,
			                                int scale_factor, 
			                                bool force_baseline)
{
	jpeg_set_linear_quality(&st->data.compress->cinfo,  scale_factor, force_baseline);
  return 0;
}

int ps_jpeg_set_quality(PSStreamDCTState * st,
		                           int quality, 
		                           bool force_baseline)
{
	jpeg_set_quality(&st->data.compress->cinfo, quality, force_baseline);
  return 0;
}

int ps_jpeg_start_compress(PSStreamDCTState * st, bool write_all_tables)
{
	jpeg_start_compress(&st->data.compress->cinfo, write_all_tables);
  return 0;
}

int ps_jpeg_write_scanlines(PSStreamDCTState * st,
																	 JSAMPARRAY scanlines,
																	 int num_lines)
{
	return (int)jpeg_write_scanlines(&st->data.compress->cinfo, scanlines, (JDIMENSION) num_lines);
}

int ps_jpeg_finish_compress(PSStreamDCTState * st)
{
	jpeg_finish_compress(&st->data.compress->cinfo);
  return 0;
}

int ps_jpeg_destroy(PSStreamDCTState * st)
{
	jpeg_destroy((j_common_ptr) & st->data.compress->cinfo);
  return 0;
}

uchar * hc_put_last_bits_proc(PSStreamHCState * ss, uchar * q, uint bits, int bits_left)
{
	while (bits_left < hc_bits_size) 
	{
		uchar c = (uchar) (bits >> (hc_bits_size - 8));

		if (ss->FirstBitLowOrder)
	    c = byte_reverse_bits[c];
		*++q = c;
		bits <<= 8;
		bits_left += 8;
  }
  ss->bits = bits;
  ss->bits_left = bits_left;
  return q;
}

static void hc_put_code_proc(bool reverse_bits, uchar * q, uint cw)
{
#define cb(n) ((uchar)(cw >> (n * 8)))
    if (reverse_bits) {
#if hc_bits_size > 16
	q[-3] = byte_reverse_bits[cb(3)];
	q[-2] = byte_reverse_bits[cb(2)];
#endif
	q[-1] = byte_reverse_bits[cb(1)];
	q[0] = byte_reverse_bits[cb(0)];
    } else {
#if hc_bits_size > 16
	q[-3] = cb(3);
	q[-2] = cb(2);
#endif
	q[-1] = cb(1);
	q[0] = cb(0);
    }
#undef cb
}

void s_CF_set_defaults_inline(PSStreamCFState * ss)
{
	ss->Uncompressed = false;
	ss->K = 0;
	ss->EndOfLine = false;
	ss->EncodedByteAlign = false;
	ss->Columns = 1728;
	ss->Rows = 0;
	ss->EndOfBlock = true;
	ss->BlackIs1 = false,
	ss->DamagedRowsBeforeError = 0;
	ss->FirstBitLowOrder = false;
	ss->DecodedByteAlign = 1;
	ss->raster = 0;
	ss->lbuf = 0;
	ss->lprev = 0;
}

#define cf_byte_run_length byte_bit_run_length_neg
#define cf_byte_run_length_0 byte_bit_run_length_0

const cfe_run cf_run_eol =
{run_eol_code_value, run_eol_code_length};

const cfe_run cf1_run_uncompressed =
{0xf, 12};

const cfe_run cf2_run_pass =
{cf2_run_pass_value, cf2_run_pass_length};

const cfe_run cf2_run_vertical[7] =
{
    {0x3, 7},
    {0x3, 6},
    {0x3, 3},
    {0x1, 1},
    {0x2, 3},
    {0x2, 6},
    {0x2, 7}
};

const cfe_run cf2_run_horizontal =
{cf2_run_horizontal_value, cf2_run_horizontal_length};
const cfe_run cf2_run_uncompressed =
{0xf, 10};

const cfe_run cf2_run_eol_1d =
{(run_eol_code_value << 1) + 1, run_eol_code_length + 1};
const cfe_run cf2_run_eol_2d =
{(run_eol_code_value << 1) + 0, run_eol_code_length + 1};

const cfe_run cf_uncompressed[6] =
{
    {1, 1},
    {1, 2},
    {1, 3},
    {1, 4},
    {1, 5},
    {1, 6}
};

const cfe_run cf_uncompressed_exit[10] =
{
    {2, 8}, {3, 8},
    {2, 9}, {3, 9},
    {2, 10}, {3, 10},
    {2, 11}, {3, 11},
    {2, 12}, {3, 12}
};


const cf_runs cf_white_runs =
{
    {				/* Termination codes */
	{0x35, 8}, {0x7, 6}, {0x7, 4}, {0x8, 4},
	{0xb, 4}, {0xc, 4}, {0xe, 4}, {0xf, 4},
	{0x13, 5}, {0x14, 5}, {0x7, 5}, {0x8, 5},
	{0x8, 6}, {0x3, 6}, {0x34, 6}, {0x35, 6},
	{0x2a, 6}, {0x2b, 6}, {0x27, 7}, {0xc, 7},
	{0x8, 7}, {0x17, 7}, {0x3, 7}, {0x4, 7},
	{0x28, 7}, {0x2b, 7}, {0x13, 7}, {0x24, 7},
	{0x18, 7}, {0x2, 8}, {0x3, 8}, {0x1a, 8},
	{0x1b, 8}, {0x12, 8}, {0x13, 8}, {0x14, 8},
	{0x15, 8}, {0x16, 8}, {0x17, 8}, {0x28, 8},
	{0x29, 8}, {0x2a, 8}, {0x2b, 8}, {0x2c, 8},
	{0x2d, 8}, {0x4, 8}, {0x5, 8}, {0xa, 8},
	{0xb, 8}, {0x52, 8}, {0x53, 8}, {0x54, 8},
	{0x55, 8}, {0x24, 8}, {0x25, 8}, {0x58, 8},
	{0x59, 8}, {0x5a, 8}, {0x5b, 8}, {0x4a, 8},
	{0x4b, 8}, {0x32, 8}, {0x33, 8}, {0x34, 8}
    },
    {				/* Make-up codes */
	{0, 0} /* dummy */ , {0x1b, 5}, {0x12, 5}, {0x17, 6},
	{0x37, 7}, {0x36, 8}, {0x37, 8}, {0x64, 8},
	{0x65, 8}, {0x68, 8}, {0x67, 8}, {0xcc, 9},
	{0xcd, 9}, {0xd2, 9}, {0xd3, 9}, {0xd4, 9},
	{0xd5, 9}, {0xd6, 9}, {0xd7, 9}, {0xd8, 9},
	{0xd9, 9}, {0xda, 9}, {0xdb, 9}, {0x98, 9},
	{0x99, 9}, {0x9a, 9}, {0x18, 6}, {0x9b, 9},
	{0x8, 11}, {0xc, 11}, {0xd, 11}, {0x12, 12},
	{0x13, 12}, {0x14, 12}, {0x15, 12}, {0x16, 12},
	{0x17, 12}, {0x1c, 12}, {0x1d, 12}, {0x1e, 12},
	{0x1f, 12}
    }
};

const cf_runs cf_black_runs =
{
    {				/* Termination codes */
	{0x37, 10}, {0x2, 3}, {0x3, 2}, {0x2, 2},
	{0x3, 3}, {0x3, 4}, {0x2, 4}, {0x3, 5},
	{0x5, 6}, {0x4, 6}, {0x4, 7}, {0x5, 7},
	{0x7, 7}, {0x4, 8}, {0x7, 8}, {0x18, 9},
	{0x17, 10}, {0x18, 10}, {0x8, 10}, {0x67, 11},
	{0x68, 11}, {0x6c, 11}, {0x37, 11}, {0x28, 11},
	{0x17, 11}, {0x18, 11}, {0xca, 12}, {0xcb, 12},
	{0xcc, 12}, {0xcd, 12}, {0x68, 12}, {0x69, 12},
	{0x6a, 12}, {0x6b, 12}, {0xd2, 12}, {0xd3, 12},
	{0xd4, 12}, {0xd5, 12}, {0xd6, 12}, {0xd7, 12},
	{0x6c, 12}, {0x6d, 12}, {0xda, 12}, {0xdb, 12},
	{0x54, 12}, {0x55, 12}, {0x56, 12}, {0x57, 12},
	{0x64, 12}, {0x65, 12}, {0x52, 12}, {0x53, 12},
	{0x24, 12}, {0x37, 12}, {0x38, 12}, {0x27, 12},
	{0x28, 12}, {0x58, 12}, {0x59, 12}, {0x2b, 12},
	{0x2c, 12}, {0x5a, 12}, {0x66, 12}, {0x67, 12}
    },
    {				/* Make-up codes. */
	{0, 0} /* dummy */ , {0xf, 10}, {0xc8, 12}, {0xc9, 12},
	{0x5b, 12}, {0x33, 12}, {0x34, 12}, {0x35, 12},
	{0x6c, 13}, {0x6d, 13}, {0x4a, 13}, {0x4b, 13},
	{0x4c, 13}, {0x4d, 13}, {0x72, 13}, {0x73, 13},
	{0x74, 13}, {0x75, 13}, {0x76, 13}, {0x77, 13},
	{0x52, 13}, {0x53, 13}, {0x54, 13}, {0x55, 13},
	{0x5a, 13}, {0x5b, 13}, {0x64, 13}, {0x65, 13},
	{0x8, 11}, {0xc, 11}, {0xd, 11}, {0x12, 12},
	{0x13, 12}, {0x14, 12}, {0x15, 12}, {0x16, 12},
	{0x17, 12}, {0x1c, 12}, {0x1d, 12}, {0x1e, 12},
	{0x1f, 12}
    }
};
const cfd_node cf_white_decode[] = {
	{ 256, 12 },
	{ 272, 12 },
	{ 29, 8 },
	{ 30, 8 },
	{ 45, 8 },
	{ 46, 8 },
	{ 22, 7 },
	{ 22, 7 },
	{ 23, 7 },
	{ 23, 7 },
	{ 47, 8 },
	{ 48, 8 },
	{ 13, 6 },
	{ 13, 6 },
	{ 13, 6 },
	{ 13, 6 },
	{ 20, 7 },
	{ 20, 7 },
	{ 33, 8 },
	{ 34, 8 },
	{ 35, 8 },
	{ 36, 8 },
	{ 37, 8 },
	{ 38, 8 },
	{ 19, 7 },
	{ 19, 7 },
	{ 31, 8 },
	{ 32, 8 },
	{ 1, 6 },
	{ 1, 6 },
	{ 1, 6 },
	{ 1, 6 },
	{ 12, 6 },
	{ 12, 6 },
	{ 12, 6 },
	{ 12, 6 },
	{ 53, 8 },
	{ 54, 8 },
	{ 26, 7 },
	{ 26, 7 },
	{ 39, 8 },
	{ 40, 8 },
	{ 41, 8 },
	{ 42, 8 },
	{ 43, 8 },
	{ 44, 8 },
	{ 21, 7 },
	{ 21, 7 },
	{ 28, 7 },
	{ 28, 7 },
	{ 61, 8 },
	{ 62, 8 },
	{ 63, 8 },
	{ 0, 8 },
	{ 320, 8 },
	{ 384, 8 },
	{ 10, 5 },
	{ 10, 5 },
	{ 10, 5 },
	{ 10, 5 },
	{ 10, 5 },
	{ 10, 5 },
	{ 10, 5 },
	{ 10, 5 },
	{ 11, 5 },
	{ 11, 5 },
	{ 11, 5 },
	{ 11, 5 },
	{ 11, 5 },
	{ 11, 5 },
	{ 11, 5 },
	{ 11, 5 },
	{ 27, 7 },
	{ 27, 7 },
	{ 59, 8 },
	{ 60, 8 },
	{ 288, 9 },
	{ 290, 9 },
	{ 18, 7 },
	{ 18, 7 },
	{ 24, 7 },
	{ 24, 7 },
	{ 49, 8 },
	{ 50, 8 },
	{ 51, 8 },
	{ 52, 8 },
	{ 25, 7 },
	{ 25, 7 },
	{ 55, 8 },
	{ 56, 8 },
	{ 57, 8 },
	{ 58, 8 },
	{ 192, 6 },
	{ 192, 6 },
	{ 192, 6 },
	{ 192, 6 },
	{ 1664, 6 },
	{ 1664, 6 },
	{ 1664, 6 },
	{ 1664, 6 },
	{ 448, 8 },
	{ 512, 8 },
	{ 292, 9 },
	{ 640, 8 },
	{ 576, 8 },
	{ 294, 9 },
	{ 296, 9 },
	{ 298, 9 },
	{ 300, 9 },
	{ 302, 9 },
	{ 256, 7 },
	{ 256, 7 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 128, 5 },
	{ 128, 5 },
	{ 128, 5 },
	{ 128, 5 },
	{ 128, 5 },
	{ 128, 5 },
	{ 128, 5 },
	{ 128, 5 },
	{ 8, 5 },
	{ 8, 5 },
	{ 8, 5 },
	{ 8, 5 },
	{ 8, 5 },
	{ 8, 5 },
	{ 8, 5 },
	{ 8, 5 },
	{ 9, 5 },
	{ 9, 5 },
	{ 9, 5 },
	{ 9, 5 },
	{ 9, 5 },
	{ 9, 5 },
	{ 9, 5 },
	{ 9, 5 },
	{ 16, 6 },
	{ 16, 6 },
	{ 16, 6 },
	{ 16, 6 },
	{ 17, 6 },
	{ 17, 6 },
	{ 17, 6 },
	{ 17, 6 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 14, 6 },
	{ 14, 6 },
	{ 14, 6 },
	{ 14, 6 },
	{ 15, 6 },
	{ 15, 6 },
	{ 15, 6 },
	{ 15, 6 },
	{ 64, 5 },
	{ 64, 5 },
	{ 64, 5 },
	{ 64, 5 },
	{ 64, 5 },
	{ 64, 5 },
	{ 64, 5 },
	{ 64, 5 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ -2, 3 },
	{ -2, 3 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -3, 4 },
	{ 1792, 3 },
	{ 1792, 3 },
	{ 1984, 4 },
	{ 2048, 4 },
	{ 2112, 4 },
	{ 2176, 4 },
	{ 2240, 4 },
	{ 2304, 4 },
	{ 1856, 3 },
	{ 1856, 3 },
	{ 1920, 3 },
	{ 1920, 3 },
	{ 2368, 4 },
	{ 2432, 4 },
	{ 2496, 4 },
	{ 2560, 4 },
	{ 1472, 1 },
	{ 1536, 1 },
	{ 1600, 1 },
	{ 1728, 1 },
	{ 704, 1 },
	{ 768, 1 },
	{ 832, 1 },
	{ 896, 1 },
	{ 960, 1 },
	{ 1024, 1 },
	{ 1088, 1 },
	{ 1152, 1 },
	{ 1216, 1 },
	{ 1280, 1 },
	{ 1344, 1 },
	{ 1408, 1 }
};

/* Black decoding table. */
const cfd_node cf_black_decode[] = {
	{ 128, 12 },
	{ 160, 13 },
	{ 224, 12 },
	{ 256, 12 },
	{ 10, 7 },
	{ 11, 7 },
	{ 288, 12 },
	{ 12, 7 },
	{ 9, 6 },
	{ 9, 6 },
	{ 8, 6 },
	{ 8, 6 },
	{ 7, 5 },
	{ 7, 5 },
	{ 7, 5 },
	{ 7, 5 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ -2, 4 },
	{ -2, 4 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -3, 5 },
	{ 1792, 4 },
	{ 1792, 4 },
	{ 1984, 5 },
	{ 2048, 5 },
	{ 2112, 5 },
	{ 2176, 5 },
	{ 2240, 5 },
	{ 2304, 5 },
	{ 1856, 4 },
	{ 1856, 4 },
	{ 1920, 4 },
	{ 1920, 4 },
	{ 2368, 5 },
	{ 2432, 5 },
	{ 2496, 5 },
	{ 2560, 5 },
	{ 18, 3 },
	{ 18, 3 },
	{ 18, 3 },
	{ 18, 3 },
	{ 18, 3 },
	{ 18, 3 },
	{ 18, 3 },
	{ 18, 3 },
	{ 52, 5 },
	{ 52, 5 },
	{ 640, 6 },
	{ 704, 6 },
	{ 768, 6 },
	{ 832, 6 },
	{ 55, 5 },
	{ 55, 5 },
	{ 56, 5 },
	{ 56, 5 },
	{ 1280, 6 },
	{ 1344, 6 },
	{ 1408, 6 },
	{ 1472, 6 },
	{ 59, 5 },
	{ 59, 5 },
	{ 60, 5 },
	{ 60, 5 },
	{ 1536, 6 },
	{ 1600, 6 },
	{ 24, 4 },
	{ 24, 4 },
	{ 24, 4 },
	{ 24, 4 },
	{ 25, 4 },
	{ 25, 4 },
	{ 25, 4 },
	{ 25, 4 },
	{ 1664, 6 },
	{ 1728, 6 },
	{ 320, 5 },
	{ 320, 5 },
	{ 384, 5 },
	{ 384, 5 },
	{ 448, 5 },
	{ 448, 5 },
	{ 512, 6 },
	{ 576, 6 },
	{ 53, 5 },
	{ 53, 5 },
	{ 54, 5 },
	{ 54, 5 },
	{ 896, 6 },
	{ 960, 6 },
	{ 1024, 6 },
	{ 1088, 6 },
	{ 1152, 6 },
	{ 1216, 6 },
	{ 64, 3 },
	{ 64, 3 },
	{ 64, 3 },
	{ 64, 3 },
	{ 64, 3 },
	{ 64, 3 },
	{ 64, 3 },
	{ 64, 3 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 23, 4 },
	{ 23, 4 },
	{ 50, 5 },
	{ 51, 5 },
	{ 44, 5 },
	{ 45, 5 },
	{ 46, 5 },
	{ 47, 5 },
	{ 57, 5 },
	{ 58, 5 },
	{ 61, 5 },
	{ 256, 5 },
	{ 16, 3 },
	{ 16, 3 },
	{ 16, 3 },
	{ 16, 3 },
	{ 17, 3 },
	{ 17, 3 },
	{ 17, 3 },
	{ 17, 3 },
	{ 48, 5 },
	{ 49, 5 },
	{ 62, 5 },
	{ 63, 5 },
	{ 30, 5 },
	{ 31, 5 },
	{ 32, 5 },
	{ 33, 5 },
	{ 40, 5 },
	{ 41, 5 },
	{ 22, 4 },
	{ 22, 4 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 15, 2 },
	{ 15, 2 },
	{ 15, 2 },
	{ 15, 2 },
	{ 15, 2 },
	{ 15, 2 },
	{ 15, 2 },
	{ 15, 2 },
	{ 128, 5 },
	{ 192, 5 },
	{ 26, 5 },
	{ 27, 5 },
	{ 28, 5 },
	{ 29, 5 },
	{ 19, 4 },
	{ 19, 4 },
	{ 20, 4 },
	{ 20, 4 },
	{ 34, 5 },
	{ 35, 5 },
	{ 36, 5 },
	{ 37, 5 },
	{ 38, 5 },
	{ 39, 5 },
	{ 21, 4 },
	{ 21, 4 },
	{ 42, 5 },
	{ 43, 5 },
	{ 0, 3 },
	{ 0, 3 },
	{ 0, 3 },
	{ 0, 3 }
};

/* 2-D decoding table. */
const cfd_node cf_2d_decode[] = {
	{ 128, 11 },
	{ 144, 10 },
	{ 6, 7 },
	{ 0, 7 },
	{ 5, 6 },
	{ 5, 6 },
	{ 1, 6 },
	{ 1, 6 },
	{ -4, 4 },
	{ -4, 4 },
	{ -4, 4 },
	{ -4, 4 },
	{ -4, 4 },
	{ -4, 4 },
	{ -4, 4 },
	{ -4, 4 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ -2, 4 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -3, 3 }
};

/* Uncompresssed decoding table. */
const cfd_node cf_uncompressed_decode[] = {
	{ 64, 12 },
	{ 5, 6 },
	{ 4, 5 },
	{ 4, 5 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ -1, 0 },
	{ -1, 0 },
	{ 8, 6 },
	{ 9, 6 },
	{ 6, 5 },
	{ 6, 5 },
	{ 7, 5 },
	{ 7, 5 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 }
};

static uchar * cf_put_long_run(PSStreamCFEState * ss, uchar * q, int lenv, const cf_runs * prt)
{
	uint bitsA = ss->bits;
	int bits_leftA = ss->bits_left;
	cfe_run rr;
	while (lenv >= 2560 + 64) 
	{
		rr = prt->make_up[40];
		if ((bits_leftA -= rr.code_length) >= 0)
			bitsA += rr.code << bits_leftA;
		else
		{
			hc_put_code_proc(ss->FirstBitLowOrder, q += hc_bits_size >> 3, (bitsA + (rr.code >> -bits_leftA)));
			bitsA = rr.code << (bits_leftA += hc_bits_size);
		}
		lenv -= 2560;
  }
  
  rr = prt->make_up[lenv >> 6];
  if ((bits_leftA -= rr.code_length) >= 0)
		bitsA += rr.code << bits_leftA;
	else
	{
		hc_put_code_proc(ss->FirstBitLowOrder, q += hc_bits_size >> 3, (bitsA + (rr.code >> -bits_leftA)));
		bitsA = rr.code << (bits_leftA += hc_bits_size);
	}
	
	ss->bits = bitsA; 
	ss->bits_left = bits_leftA;
	return q;
}

static void cf_encode_1d(PSStreamCFEState * ss, const uchar * lbufA, PSStreamCursorWrite * pw)
{
	uint countA = ss->raster << 3;
  uchar *qA = pw->ptr;
  int end_countA = -ss->Columns & 7;
  const uchar *pA = lbufA;
  uchar invertA = (ss->BlackIs1 ? 0 : 0xff);
  uint dataA = *pA++ ^ invertA;
  uint bitsA = ss->bits;
	int bits_leftA = ss->bits_left;
	int rlen;
	while (countA != end_countA)
	{
		rlen = cf_byte_run_length[countA & 7][dataA ^ 0xff];
    if ( rlen >= 8 ) 
    {
			if ( invertA == 0 ) 
			{
	    	if ( pA[0] ) 
	    	{ 
	    		dataA = pA[0]; 
	    		pA += 1; 
	    		rlen -= 8; 
	    	}
	    	else if ( pA[1] ) 
	    	{ 
	    		dataA = pA[1]; 
	    		pA += 2; 
	    	}
	    	else 
	    	{
					while (!(pA[2] | pA[3] | pA[4] | pA[5]))
					{
		    		pA += 4; 
		    		rlen += 32;
		    	}
					if ( pA[2] ) 
					{
		    		dataA = pA[2]; pA += 3; rlen += 8;
					} 
					else if ( pA[3] ) 
					{
		    		dataA = pA[3]; pA += 4; rlen += 16;
					} 
					else if ( pA[4] ) 
					{
		    		dataA = pA[4]; pA += 5; rlen += 24;
					} 
					else
					{
		    		dataA = pA[5]; pA += 6; rlen += 32;
					}
	    	}
			} 
			else 
			{
	    	if ( pA[0] != 0xff ) 
	    	{ 
	    		dataA = (uchar)~pA[0]; pA += 1; rlen -= 8; 
	    	}
	    	else if ( pA[1] != 0xff ) 
	    	{ 
	    		dataA = (uchar)~pA[1]; pA += 2; 
	    	}
	    	else 
	    	{
					while ( (pA[2] & pA[3] & pA[4] & pA[5]) == 0xff )
					{
		   			pA += 4; 
		   			rlen += 32;
		   		}
					if ( pA[2] != 0xff ) 
					{
		   			dataA = (uchar)~pA[2]; pA += 3; rlen += 8;
					} 
					else if ( pA[3] != 0xff ) 
					{
		   			dataA = (uchar)~pA[3]; pA += 4; rlen += 16;
					} 
					else if ( pA[4] != 0xff ) 
					{
		   			dataA = (uchar)~pA[4]; pA += 5; rlen += 24;
					} 
					else
					{
		   			dataA = (uchar)~pA[5]; pA += 6; rlen += 32;
					}
	    	}
			}
			rlen += cf_byte_run_length_0[dataA ^ 0xff];
    }
    countA -= rlen;
    
    if (rlen >= 64)
		{
			ss->bits = bitsA; 
			ss->bits_left = bits_leftA;
			qA = cf_put_long_run(ss, qA, rlen, &cf_white_runs);
			bitsA = ss->bits; 
			bits_leftA = ss->bits_left;
			rlen &= 63;
		}
			
		cfe_run rr = cf_white_runs.termination[rlen];
		if ((bits_leftA -= rr.code_length) >= 0)
			bitsA += rr.code << bits_leftA;
		else
		{
			hc_put_code_proc(ss->FirstBitLowOrder, qA += hc_bits_size >> 3, (bitsA + (rr.code >> -bits_leftA)));
			bitsA = rr.code << (bits_leftA += hc_bits_size);
		}
		
		if (countA == end_countA)
	    break;
	    
	  rlen = cf_byte_run_length[countA & 7][dataA];
    if ( rlen >= 8 ) 
    {
			if ( invertA == 0 )
	   		for ( ; ; pA += 4, rlen += 32 ) 
	   		{
					if ( pA[0] != 0xff ) 
					{ 
						dataA = pA[0]; pA += 1; rlen -= 8; 
						break; 
					}
					if ( pA[1] != 0xff ) 
					{ 
						dataA = pA[1]; pA += 2; break; 
					}
					if ( pA[2] != 0xff ) 
					{ 
						dataA = pA[2]; pA += 3; rlen += 8; break; 
					}
					if ( pA[3] != 0xff ) 
					{ 
						dataA = pA[3]; pA += 4; rlen += 16; break; 
					}
	   		}
			else
	   		for ( ; ; pA += 4, rlen += 32 ) 
	   		{
					if ( pA[0] ) 
					{ 
						dataA = (uchar)~pA[0]; pA += 1; rlen -= 8; break; 
					}
					if ( pA[1] ) 
					{ 
						dataA = (uchar)~pA[1]; pA += 2; break; 
					}
					if ( pA[2] ) 
					{ 
						dataA = (uchar)~pA[2]; pA += 3; rlen += 8; break; 
					}
					if ( pA[3] ) 
					{ 
						dataA = (uchar)~pA[3]; pA += 4; rlen += 16; break; 
					}
	   		}
				rlen += cf_byte_run_length_0[dataA];
    }
    countA -= rlen;
    
    if (rlen >= 64)
		{
			ss->bits = bitsA; 
			ss->bits_left = bits_leftA;
			qA = cf_put_long_run(ss, qA, rlen, &cf_black_runs);
			bitsA = ss->bits; 
			bits_leftA = ss->bits_left;
			rlen &= 63;
		}
			
		rr = cf_black_runs.termination[rlen];
		if ((bits_leftA -= rr.code_length) >= 0)
			bitsA += rr.code << bits_leftA;
		else
		{
			hc_put_code_proc(ss->FirstBitLowOrder, qA += hc_bits_size >> 3, (bitsA + (rr.code >> -bits_leftA)));
			bitsA = rr.code << (bits_leftA += hc_bits_size);
		}
	}
	
	ss->bits = bitsA; 
	ss->bits_left = bits_leftA;
	pw->ptr = qA;
}

static void cf_encode_2d(PSStreamCFEState * ss, 
                         const uchar * lbufA, 
	                       PSStreamCursorWrite * pw, 
	                       const uchar * lprevA)
{
	uchar invert_whiteA = (ss->BlackIs1 ? 0 : 0xff);
  uchar invertA = invert_whiteA;
  uint countA = ss->raster << 3;
  int end_countA = -ss->Columns & 7;
  const uchar *pA = lbufA;
  uchar *qA = pw->ptr;
  uint dataA = *pA++ ^ invertA;
  static const uchar initial_count_bit[8] = {0, 1, 2, 4, 8, 0x10, 0x20, 0x40 };
  static const uchar further_count_bit[8] = {0x80, 1, 2, 4, 8, 0x10, 0x20, 0x40};
  const uchar *count_bitA = initial_count_bit;
  uint bitsA = ss->bits;
	int bits_leftA = ss->bits_left;
	while (countA != end_countA)
	{
		uint a0 = countA;
		int diff;
		uint prev_countA = countA;
		const uchar *prev_pA = pA - lbufA + lprevA;
		uchar prev_dataA = prev_pA[-1] ^ invertA;
		
		int rlen = cf_byte_run_length[countA & 7][dataA ^ 0xff];
    if ( rlen >= 8 ) 
    {
			if ( invertA == 0 ) 
			{
	    	if ( pA[0] ) 
	    	{ 
	    		dataA = pA[0]; 
	    		pA += 1; 
	    		rlen -= 8; 
	    	}
	    	else if ( pA[1] ) 
	    	{ 
	    		dataA = pA[1]; 
	    		pA += 2; 
	    	}
	    	else 
	    	{
					while (!(pA[2] | pA[3] | pA[4] | pA[5]))
					{
		    		pA += 4; 
		    		rlen += 32;
		    	}
					if ( pA[2] ) 
					{
		    		dataA = pA[2]; pA += 3; rlen += 8;
					} 
					else if ( pA[3] ) 
					{
		    		dataA = pA[3]; pA += 4; rlen += 16;
					} 
					else if ( pA[4] ) 
					{
		    		dataA = pA[4]; pA += 5; rlen += 24;
					} 
					else
					{
		    		dataA = pA[5]; pA += 6; rlen += 32;
					}
	    	}
			} 
			else 
			{
	    	if ( pA[0] != 0xff ) 
	    	{ 
	    		dataA = (uchar)~pA[0]; pA += 1; rlen -= 8; 
	    	}
	    	else if ( pA[1] != 0xff ) 
	    	{ 
	    		dataA = (uchar)~pA[1]; pA += 2; 
	    	}
	    	else 
	    	{
					while ( (pA[2] & pA[3] & pA[4] & pA[5]) == 0xff )
					{
		   			pA += 4; 
		   			rlen += 32;
		   		}
					if ( pA[2] != 0xff ) 
					{
		   			dataA = (uchar)~pA[2]; pA += 3; rlen += 8;
					} 
					else if ( pA[3] != 0xff ) 
					{
		   			dataA = (uchar)~pA[3]; pA += 4; rlen += 16;
					} 
					else if ( pA[4] != 0xff ) 
					{
		   			dataA = (uchar)~pA[4]; pA += 5; rlen += 24;
					} 
					else
					{
		   			dataA = (uchar)~pA[5]; pA += 6; rlen += 32;
					}
	    	}
			}
			rlen += cf_byte_run_length_0[dataA ^ 0xff];
    }
    countA -= rlen;
    	
    uint a1 = countA;
    if ((prev_dataA & count_bitA[prev_countA & 7]))
    {
    	rlen = cf_byte_run_length[prev_countA & 7][prev_dataA];
    	if ( rlen >= 8 ) 
    	{
				if ( invertA == 0 )
	   			for ( ; ; prev_pA += 4, rlen += 32 ) 
	   			{
						if ( prev_pA[0] != 0xff ) 
						{ 
							prev_dataA = prev_pA[0]; prev_pA += 1; rlen -= 8; 
							break; 
						}
						if ( prev_pA[1] != 0xff ) 
						{ 
							prev_dataA = prev_pA[1]; prev_pA += 2; 
							break; 
						}
						if ( prev_pA[2] != 0xff ) 
						{ 
							prev_dataA = prev_pA[2]; prev_pA += 3; rlen += 8; 
							break; 
						}
						if ( prev_pA[3] != 0xff ) 
						{ 
							prev_dataA = prev_pA[3]; prev_pA += 4; rlen += 16; 
							break; 
						}
	   			}
				else
	   			for ( ; ; prev_pA += 4, rlen += 32 ) 
	   			{
						if ( prev_pA[0] ) 
						{ 
							prev_dataA = (uchar)~prev_pA[0]; prev_pA += 1; rlen -= 8; 
							break; 
						}
						if ( prev_pA[1] ) 
						{ 
							prev_dataA = (uchar)~prev_pA[1]; prev_pA += 2; 
							break; 
						}
						if ( prev_pA[2] ) 
						{ 
							prev_dataA = (uchar)~prev_pA[2]; prev_pA += 3; rlen += 8; 
							break; 
						}
						if ( prev_pA[3] ) 
						{ 
							prev_dataA = (uchar)~prev_pA[3]; prev_pA += 4; rlen += 16; 
							break; 
						}
	   			}
					rlen += cf_byte_run_length_0[prev_dataA];
    		}
    		prev_countA -= rlen;
    }
    count_bitA = further_count_bit;
    
pass:
		if (prev_countA != end_countA)
		{
			rlen = cf_byte_run_length[prev_countA & 7][prev_dataA ^ 0xff];
    	if ( rlen >= 8 ) 
    	{	
				if ( invertA == 0 ) 
				{
	    		if ( prev_pA[0] ) 
	    		{ 
	    			prev_dataA = prev_pA[0]; prev_pA += 1; rlen -= 8; 
	    		}
	    		else if ( prev_pA[1] ) 
	    		{ 
	    			prev_dataA = prev_pA[1]; prev_pA += 2; 
	    		}
	    		else 
	    		{
						while ( !(prev_pA[2] | prev_pA[3] | prev_pA[4] | prev_pA[5]) )
						{
		    			prev_pA += 4; 
		    			rlen += 32;
		    		}
						if ( prev_pA[2] ) 
						{
		    			prev_dataA = prev_pA[2]; prev_pA += 3; rlen += 8;
						} 
						else if ( prev_pA[3] ) 
						{
		    			prev_dataA = prev_pA[3]; prev_pA += 4; rlen += 16;
						}
						else if ( prev_pA[4] ) 
						{
		    			prev_dataA = prev_pA[4]; prev_pA += 5; rlen += 24;
						} 
						else
						{
		    			prev_dataA = prev_pA[5]; prev_pA += 6; rlen += 32;
						}
	    		}
				} 
				else 
				{
	    		if ( prev_pA[0] != 0xff ) 
	    		{ 
	    			prev_dataA = (uchar)~prev_pA[0]; prev_pA += 1; rlen -= 8; 
	    		}
	    		else if ( prev_pA[1] != 0xff ) 
	    		{ 
	    			prev_dataA = (uchar)~prev_pA[1]; prev_pA += 2; 
	    		}
	    		else 
	    		{
						while ( (prev_pA[2] & prev_pA[3] & prev_pA[4] & prev_pA[5]) == 0xff )
						{
		    			prev_pA += 4; 
		    			rlen += 32;
		    		}
						if ( prev_pA[2] != 0xff ) 
						{
		    			prev_dataA = (uchar)~prev_pA[2]; prev_pA += 3; rlen += 8;
						} 
						else if ( prev_pA[3] != 0xff ) 
						{
		    			prev_dataA = (uchar)~prev_pA[3]; prev_pA += 4; rlen += 16;
						} 
						else if ( prev_pA[4] != 0xff ) 
						{
		    			prev_dataA = (uchar)~prev_pA[4]; prev_pA += 5; rlen += 24;
						} 
						else
						{
		    			prev_dataA = (uchar)~prev_pA[5]; prev_pA += 6; rlen += 32;
						}
	    		}
				}
				rlen += cf_byte_run_length_0[prev_dataA ^ 0xff];
    	}
    	prev_countA -= rlen;
		}
		
	  diff = a1 - prev_countA;
	  if (diff <= -2)
	  {
		  if (prev_countA != end_countA)
		  {
			  rlen = cf_byte_run_length[prev_countA & 7][prev_dataA];
    	  if ( rlen >= 8 ) 
    	  {
				  if ( invertA == 0 )
	   			 for ( ; ; prev_pA += 4, rlen += 32 ) 
	   			 {
						 if ( prev_pA[0] != 0xff ) 
						 { 
							 prev_dataA = prev_pA[0]; prev_pA += 1; rlen -= 8; 
							 break; 
						 }
						 if ( prev_pA[1] != 0xff ) 
						 { 
							 prev_dataA = prev_pA[1]; prev_pA += 2; 
							 break; 
						 }
						 if ( prev_pA[2] != 0xff ) 
						 { 
							 prev_dataA = prev_pA[2]; prev_pA += 3; rlen += 8; 
							 break; 
						 }
						 if ( prev_pA[3] != 0xff ) 
						 { 
							 prev_dataA = prev_pA[3]; prev_pA += 4; rlen += 16; 
							 break; 
						 }
	   			 }
				 else
	   			for ( ; ; prev_pA += 4, rlen += 32 ) 
	   			{
						if ( prev_pA[0] ) 
						{ 
							prev_dataA = (uchar)~prev_pA[0]; prev_pA += 1; rlen -= 8; 
							break; 
						}
						if ( prev_pA[1] ) 
						{ 
							prev_dataA = (uchar)~prev_pA[1]; prev_pA += 2; 
							break; 
						}
						if ( prev_pA[2] ) 
						{ 
							prev_dataA = (uchar)~prev_pA[2]; prev_pA += 3; rlen += 8; 
							break; 
						}
						if ( prev_pA[3] ) 
						{ 
							prev_dataA = (uchar)~prev_pA[3]; prev_pA += 4; rlen += 16; 
							break; 
						}
	   			}
					rlen += cf_byte_run_length_0[prev_dataA];
    		}
    		prev_countA -= rlen;
			}
			
			if (prev_countA > a1)
			{
				if ((bits_leftA -= cf2_run_pass_length) >= 0)
					bitsA += cf2_run_pass_value << bits_leftA;
				else
				{
					hc_put_code_proc(ss->FirstBitLowOrder, qA += hc_bits_size >> 3, (bitsA + (cf2_run_pass_value >> -bits_leftA)));
					bitsA = cf2_run_pass_value << (bits_leftA += hc_bits_size);
				}
				a0 = prev_countA;
				goto pass;
			}
		}
		
		if (diff <= 3 && diff >= -3)
		{
			const cfe_run *cp = &cf2_run_vertical[diff + 3];
			if ((bits_leftA -= cp->code_length) >= 0)
				bitsA += cp->code << bits_leftA;
			else
			{
				hc_put_code_proc(ss->FirstBitLowOrder, qA += hc_bits_size >> 3, (bitsA + (cp->code >> -bits_leftA)));
				bitsA = cp->code << (bits_leftA += hc_bits_size);
			}
	    invertA = ~invertA;	/* a1 polarity changes */
	    dataA ^= 0xff;
	    continue;
		}
		
		if (countA != end_countA)
		{
			rlen = cf_byte_run_length[countA & 7][dataA];
    	if ( rlen >= 8 ) 
    	{
				if ( invertA == 0 )
	    		for ( ; ; pA += 4, rlen += 32 ) 
	    		{
						if ( pA[0] != 0xff ) 
						{ 
							dataA = pA[0]; pA += 1; rlen -= 8; 
							break; 
						}
						if ( pA[1] != 0xff ) 
						{ 
							dataA = pA[1]; pA += 2; break; 
						}
						if ( pA[2] != 0xff ) 
						{ 
							dataA = pA[2]; pA += 3; rlen += 8; break; 
						}
						if ( pA[3] != 0xff ) 
						{ 
							dataA = pA[3]; pA += 4; rlen += 16; break; 
						}
	    		}
				else
	    		for ( ; ; pA += 4, rlen += 32 ) 
	    		{
						if ( pA[0] ) 
						{ 
							dataA = (uchar)~pA[0]; pA += 1; rlen -= 8; break; 
						}
						if ( pA[1] ) 
						{ 
							dataA = (uchar)~pA[1]; pA += 2; break; 
						}
						if ( pA[2] ) 
						{ 
							dataA = (uchar)~pA[2]; pA += 3; rlen += 8; break; 
						}
						if ( pA[3] ) 
						{ 
							dataA = (uchar)~pA[3]; pA += 4; rlen += 16; break; 
						}
	    		}
					rlen += cf_byte_run_length_0[dataA];
    	}
    	countA -= rlen;
		}
		
		if ((bits_leftA -= cf2_run_horizontal_length) >= 0)
			bitsA += cf2_run_horizontal_value << bits_leftA;
		else
		{
			hc_put_code_proc(ss->FirstBitLowOrder, qA += hc_bits_size >> 3, (bitsA + (cf2_run_horizontal_value >> -bits_leftA)));
			bitsA = cf2_run_horizontal_value << (bits_leftA += hc_bits_size);
		}
		a0 -= a1;
		a1 -= countA;
		if (invertA == invert_whiteA)
		{
			if (a0 >= 64)
			{
				ss->bits = bitsA; 
				ss->bits_left = bits_leftA;
				qA = cf_put_long_run(ss, qA, a0, &cf_white_runs);
				bitsA = ss->bits; 
				bits_leftA = ss->bits_left;
				a0 &= 63;
			}
			
			cfe_run rr = cf_white_runs.termination[a0];
			if ((bits_leftA -= rr.code_length) >= 0)
				bitsA += rr.code << bits_leftA;
			else
			{
				hc_put_code_proc(ss->FirstBitLowOrder, qA += hc_bits_size >> 3, (bitsA + (rr.code >> -bits_leftA)));
				bitsA = rr.code << (bits_leftA += hc_bits_size);
			}
			
			if (a1 >= 64)
			{
				ss->bits = bitsA; 
				ss->bits_left = bits_leftA;
				qA = cf_put_long_run(ss, qA, a1, &cf_black_runs);
				bitsA = ss->bits; 
				bits_leftA = ss->bits_left;
				a1 &= 63;
			}
			
			rr = cf_black_runs.termination[a1];
			if ((bits_leftA -= rr.code_length) >= 0)
				bitsA += rr.code << bits_leftA;
			else
			{
				hc_put_code_proc(ss->FirstBitLowOrder, qA += hc_bits_size >> 3, (bitsA + (rr.code >> -bits_leftA)));
				bitsA = rr.code << (bits_leftA += hc_bits_size);
			}
		}
		else
		{
			if (a0 >= 64)
			{
				ss->bits = bitsA; 
				ss->bits_left = bits_leftA;
				qA = cf_put_long_run(ss, qA, a0, &cf_black_runs);
				bitsA = ss->bits; 
				bits_leftA = ss->bits_left;
				a0 &= 63;
			}
			
			cfe_run rr = cf_black_runs.termination[a0];
			if ((bits_leftA -= rr.code_length) >= 0)
				bitsA += rr.code << bits_leftA;
			else
			{
				hc_put_code_proc(ss->FirstBitLowOrder, qA += hc_bits_size >> 3, (bitsA + (rr.code >> -bits_leftA)));
				bitsA = rr.code << (bits_leftA += hc_bits_size);
			}
			
			if (a1 >= 64)
			{
				ss->bits = bitsA; 
				ss->bits_left = bits_leftA;
				qA = cf_put_long_run(ss, qA, a1, &cf_white_runs);
				bitsA = ss->bits; 
				bits_leftA = ss->bits_left;
				a1 &= 63;
			}
			
			rr = cf_white_runs.termination[a1];
			if ((bits_leftA -= rr.code_length) >= 0)
				bitsA += rr.code << bits_leftA;
			else
			{
				hc_put_code_proc(ss->FirstBitLowOrder, qA += hc_bits_size >> 3, (bitsA + (rr.code >> -bits_leftA)));
				bitsA = rr.code << (bits_leftA += hc_bits_size);
			}
		}
	}
	ss->bits = bitsA; 
	ss->bits_left = bits_leftA;
	pw->ptr = qA;
}

void s_CFD_set_defaults(PSStreamState * st)
{
	PSStreamCFDState *const ss = (PSStreamCFDState *) st;

  s_CF_set_defaults_inline((PSStreamCFState*)ss);
}

int  s_CFD_init(PSStreamState * st)
{
	PSStreamCFDState *const ss = (PSStreamCFDState *) st;
  int raster = ss->raster = ROUND_UP((ss->Columns + 7) >> 3, ss->DecodedByteAlign);
  uchar white = (ss->BlackIs1 ? 0 : 0xff);

  ss->bits = 0;
  ss->bits_left = 0;
  
  ss->lbuf = new uchar[raster + 4];
  ss->lprev = 0;
  if (ss->K != 0) 
  {
		ss->lprev = new uchar[raster + 4];
		memset(ss->lbuf, white, raster);
		ss->lbuf[raster] = 0xa0;
  }
  ss->k_left = qMin(ss->K, 0);
  ss->run_color = 0;
  ss->damaged_rows = 0;
  ss->skipping_damage = false;
  ss->cbit = 0;
  ss->uncomp_run = 0;
  ss->rows_left = (ss->Rows <= 0 || ss->EndOfBlock ? -1 : ss->Rows + 1);
  ss->rpos = ss->wpos = raster - 1;
  ss->eol_count = 0;
  ss->invert = white;
  ss->min_left = 1;
  return 0;
}

void s_CFD_release(PSStreamState * st)
{
	PSStreamCFDState *const ss = (PSStreamCFDState *) st;

  if (ss->lprev)
  {
  	delete [] ss->lprev;
  	ss->lprev = 0;
  }
  	
  if (ss->lbuf)
  {
  	delete [] ss->lbuf;
  	ss->lbuf = 0;
  }
}

#define cfd_declare_state\
	hcd_declare_state;\
	uchar *q;\
	int qbit
/* Load the state from the stream. */
#define cfd_load_state()\
	hcd_load_state(),\
	q = ss->lbuf + ss->wpos, qbit = ss->cbit
/* Store the state back in the stream. */
#define cfd_store_state()\
	hcd_store_state(),\
	ss->wpos = q - ss->lbuf, ss->cbit = qbit
	
#define avail_bits(n) hcd_bits_available(n)
#define ensure_bits(n, outl) hcd_ensure_bits(n, outl)
#define peek_bits(n) hcd_peek_bits(n)
#define peek_var_bits(n) hcd_peek_var_bits(n)
#define skip_bits(n) hcd_skip_bits(n)

#define get_run(decode, initial_bits, min_bits, runlen, str, locl, outl)\
	do { \
	const cfd_node *np;\
	int clen;\
\
	HCD_ENSURE_BITS_ELSE(initial_bits) {\
	    /* We might still have enough bits for the specific code. */\
	    if (bits_left < min_bits) goto outl;\
	    np = &decode[hcd_peek_bits_left() << (initial_bits - bits_left)];\
	    if ((clen = np->code_length) > bits_left) goto outl;\
	    goto locl;\
	}\
	np = &decode[peek_bits(initial_bits)];\
	if ((clen = np->code_length) > initial_bits) {\
		if (!avail_bits(clen)) goto outl;\
		clen -= initial_bits;\
		skip_bits(initial_bits);\
		ensure_bits(clen, outl);		/* can't goto outl */\
		np = &decode[np->run_length + peek_var_bits(clen)];\
		skip_bits(np->code_length);\
	} else {\
		locl:	skip_bits(clen);\
	}\
	runlen = np->run_length; \
	}while(0)
	
#define skip_data(rlen, makeup_label)\
	if ( (qbit -= rlen) < 0 )\
	{	q -= qbit >> 3, qbit &= 7;\
		if ( rlen >= 64 ) goto makeup_label;\
	}
	
#define invert_data(rlen, black_byte, makeup_action, d)\
	if ( rlen > qbit )\
	{	*q++ ^= (1 << qbit) - 1;\
		rlen -= qbit;\
		switch ( rlen >> 3 )\
		{\
		case 7:		/* original rlen possibly >= 64 */\
			if ( rlen + qbit >= 64 ) goto d;\
			*q++ = black_byte;\
		case 6: *q++ = black_byte;\
		case 5: *q++ = black_byte;\
		case 4: *q++ = black_byte;\
		case 3: *q++ = black_byte;\
		case 2: *q++ = black_byte;\
		case 1: *q = black_byte;\
			rlen &= 7;\
			if ( !rlen ) { qbit = 0; break; }\
			q++;\
		case 0:			/* know rlen != 0 */\
			qbit = 8 - rlen;\
			*q ^= 0xff << qbit;\
			break;\
		default:	/* original rlen >= 64 */\
d:			memset(q, black_byte, rlen >> 3);\
			q += rlen >> 3;\
			rlen &= 7;\
			if ( !rlen ) qbit = 0, q--;\
			else qbit = 8 - rlen, *q ^= 0xff << qbit;\
			makeup_action;\
		}\
	}\
	else\
		qbit -= rlen,\
		*q ^= ((1 << rlen) - 1) << qbit
		
static int
cf_decode_eol(PSStreamCFDState * ss, PSStreamCursorRead * pr)
{
  hcd_declare_state;
  int zeros;
  int look_ahead;

  hcd_load_state();
  for (zeros = 0; zeros < run_eol_code_length - 1; zeros++) 
  {
		ensure_bits(1, out);
		if (peek_bits(1))
	   	return -(zeros + 1);
		skip_bits(1);
  }
  look_ahead = (ss->K > 0 ? 2 : 1);
  for (;;) 
  {
		ensure_bits(look_ahead, back);
		if (peek_bits(1))
	    break;
		skip_bits(1);
  }
  skip_bits(1);
  hcd_store_state();
  return 1;
  
back:	
  bits &= (1 << bits_left) - 1;
  bits_left += run_eol_code_length - 1;
  hcd_store_state();
  
out:
	return 0;
}

static int
cf_decode_uncompressed(PSStreamCFDState * , PSStreamCursorRead * )
{
  return ERRC;
}

static int
cf_decode_1d(PSStreamCFDState * ss, PSStreamCursorRead * pr)
{
  cfd_declare_state;
  uchar black_byte = (ss->BlackIs1 ? 0xff : 0);
  int end_bit = -ss->Columns & 7;
  uchar *stop = ss->lbuf - 1 + ss->raster;
  int run_color = ss->run_color;
  int status;
  int bcnt;

  cfd_load_state();
  if (ss->run_color > 0)
		goto db;
  else
		goto dw;
		
#define q_at_stop() (q >= stop && (qbit <= end_bit || q > stop))
  
top:
	run_color = 0;
  if (q_at_stop())
		goto done;

dw:	
  get_run(cf_white_decode, cfd_white_initial_bits, cfd_white_min_bits,  bcnt, "[w1]white", dwl, out0);
  if (bcnt < 0) 
  {
		switch (bcnt) 
		{
	    case run_uncompressed:
				cfd_store_state();
				bcnt = cf_decode_uncompressed(ss, pr);
				if (bcnt < 0)
		    	return bcnt;
				cfd_load_state();
				if (bcnt)
		    	goto db;
				else
		    	goto dw;
		    	
	    default:
				status = ERRC;
				goto out;
		}
  }
  skip_data(bcnt, dwx);
  if (q_at_stop()) 
  {
		run_color = 0;
		goto done;
  }
  run_color = 1;

db:
  get_run(cf_black_decode, cfd_black_initial_bits, cfd_black_min_bits,  bcnt, "[w1]black", dbl, out1);
  if (bcnt < 0) 
  {
		status = ERRC;
		goto out;
  }
  
  invert_data(bcnt, black_byte, goto dbx, idb);
  goto top;
  
dwx:
  run_color = -1;
  goto dw;
  
dbx:
  run_color = 2;
  goto db;
  
done:
	if (q > stop || qbit < end_bit)
		status = ERRC;
  else
		status = 1;

out:
	cfd_store_state();
  ss->run_color = run_color;
  return status;
  
out0:
  status = 0;
  goto out;
  
out1:
  status = 0;
  goto out;
}

static int
cf_decode_2d(PSStreamCFDState * ss, PSStreamCursorRead * pr)
{
  cfd_declare_state;
  uchar invert_white = (ss->BlackIs1 ? 0 : 0xff);
  uchar black_byte = ~invert_white;
  uchar invert = ss->invert;
  int end_count = -ss->Columns & 7;
  uint raster = ss->raster;
  uchar *q0 = ss->lbuf;
  uchar *prev_q01 = ss->lprev + 1;
  uchar *endptr = q0 - 1 + raster;
  int init_count = raster << 3;
  int count;
  int rlen;
  int status;

  cfd_load_state();
  count = ((endptr - q) << 3) + qbit;
  endptr[1] = 0xa0;	
  
  switch (ss->run_color) 
  {
		case -2:
	    ss->run_color = 0;
	    goto hww;
	    
		case -1:
	    ss->run_color = 0;
	    goto hbw;
	    
		case 1:
	    ss->run_color = 0;
	    goto hwb;
	    
		case 2:
	    ss->run_color = 0;
	    goto hbb;
	    
	    /*case 0: */
  }
  
top:
	if (count <= end_count) 
	{
		status = (count < end_count ? ERRC : 1);
		goto out;
  }
   
  ensure_bits(3, out3);
#define vertical_0 ((sizeof(cf2_run_vertical) / sizeof(cf2_run_vertical[0])) / 2)
  
  switch (peek_bits(3)) 
  {
		default:
v0:	    
			skip_bits(1);
	    rlen = vertical_0;
	    break;
	    
		case 2:	
	    skip_bits(3);
	    rlen = vertical_0 + 1;
	    break;
	    
		case 3:	
	    skip_bits(3);
	    rlen = vertical_0 - 1;
	    break;
	    
		case 1:	
	    skip_bits(3);
	    if (invert == invert_white)
				goto hww;
	    else
				goto hbb;
				
		case 0:	
	    get_run(cf_2d_decode, cfd_2d_initial_bits, cfd_2d_min_bits,  rlen, "[w2]", d2l, out0);
	    if (rlen < 0)
				switch (rlen) 
				{
		    	case run2_pass:
						break;
						
		    	case run_uncompressed:
						{
			    		int which;

			    		cfd_store_state();
			    		which = cf_decode_uncompressed(ss, pr);
			    		if (which < 0) 
			    		{
								status = which;
								goto out;
			    		}
			    		cfd_load_state();
			    		invert = (which ? ~invert_white : invert_white);
						}
						goto top;
						
		    	default:
						status = ERRC;
						goto out;
				}
  }
   
  {
		int prev_count = count;
		uchar prev_data;
		int dlen;
		static const uchar count_bit[8] ={0x80, 1, 2, 4, 8, 0x10, 0x20, 0x40};
		uchar *prev_q = prev_q01 + (q - q0);
		int plen;

		if (!(count & 7))
	    prev_q++;	
		prev_data = prev_q[-1] ^ invert;
		if ((prev_data & count_bit[prev_count & 7]) &&
	    	(prev_count < init_count || invert != invert_white)) 
	  {	
	    skip_black_pixels(prev_data, prev_q, prev_count, invert, plen);
	    if (prev_count < end_count)
				prev_count = end_count;
		}
		if (prev_count != end_count) 
		{
	    skip_white_pixels(prev_data, prev_q,     prev_count, invert, plen);
	    if (prev_count < end_count)	
				prev_count = end_count;
		}
		if (rlen == run2_pass) 
		{
	    if (prev_count != end_count) 
	    {
				skip_black_pixels(prev_data, prev_q,  prev_count, invert, plen);
				if (prev_count < end_count)
		    	prev_count = end_count;
	    }
		} 
		else 
		{
	    prev_count += rlen - vertical_0;
		}
		if (invert == invert_white) 
		{
	    q = endptr - (prev_count >> 3);
	    qbit = prev_count & 7;
		} 
		else 
		{
	    dlen = count - prev_count;
	    invert_data(dlen, black_byte, do {}while(0), idd);
		}
		count = prev_count;
		if (rlen >= 0)
	    invert = ~invert;
  }
  goto top;
  
out3:
  if (bits_left > 0 && peek_bits(1)) 
  {
		goto v0;
  }
  
out0:
	status = 0;
	
out:
	cfd_store_state();
  ss->invert = invert;
  return status;
  
hww:
	get_run(cf_white_decode, cfd_white_initial_bits, cfd_white_min_bits, rlen, " white", wwl, outww);
  if ((count -= rlen) < end_count) 
  {
		status = ERRC;
		goto out;
  }
  skip_data(rlen, hww);
  
hwb:
	get_run(cf_black_decode, cfd_black_initial_bits, cfd_black_min_bits, rlen, " black", wbl, outwb);
  if ((count -= rlen) < end_count) 
  {
		status = ERRC;
		goto out;
  }
  invert_data(rlen, black_byte, goto hwb, ihwb);
  goto top;
  
outww:
	ss->run_color = -2;
  goto out0;
  
outwb:
	ss->run_color = 1;
  goto out0;
  
hbb:
	get_run(cf_black_decode, cfd_black_initial_bits, cfd_black_min_bits, rlen, " black", bbl, outbb);
  if ((count -= rlen) < end_count) 
  {
		status = ERRC;
		goto out;
  }
  invert_data(rlen, black_byte, goto hbb, ihbb);
  
hbw:
	get_run(cf_white_decode, cfd_white_initial_bits, cfd_white_min_bits, rlen, " white", bwl, outbw);
  if ((count -= rlen) < end_count) 
  {
		status = ERRC;
		goto out;
  }
  skip_data(rlen, hbw);
  goto top;
  
outbb:
	ss->run_color = 2;
  goto out0;
  
outbw:
	ss->run_color = -1;
  goto out0;
}

int s_CFD_process(PSStreamState * st, PSStreamCursorRead * pr,  PSStreamCursorWrite * pw, bool )
{
	PSStreamCFDState *const ss = (PSStreamCFDState *) st;
  int wstop = ss->raster - 1;
  int eol_count = ss->eol_count;
  int k_left = ss->k_left;
  int rows_left = ss->rows_left;
  int status = 0;

top:
	
  if (ss->skipping_damage) 
  {
		hcd_declare_state;
		int skip;

		status = 0;
		do 
		{
	    switch ((skip = cf_decode_eol(ss, pr))) 
	    {
				default:
		    	hcd_load_state();
		    	skip_bits(-skip);
		    	hcd_store_state();
		    	continue;
		    	
				case 0:	
		    	goto out;
		    	
				case 1:	
		    	{	
						hcd_load_state();
						bits_left += run_eol_code_length;
						hcd_store_state();
		    	}
		    	ss->skipping_damage = false;
	    }
		}	while (ss->skipping_damage);
		ss->damaged_rows++;
  }
  
  if (ss->wpos == wstop && ss->cbit <= (-ss->Columns & 7) &&
			(k_left == 0 ? !(ss->run_color & ~1) : ss->run_color == 0)) 
	{	
		if (ss->rpos < ss->wpos) 
		{
	    PSStreamCursorRead cr;

	    cr.ptr = ss->lbuf + ss->rpos;
	    cr.limit = ss->lbuf + ss->wpos;
	    status = stream_move(&cr, pw);
	    ss->rpos = cr.ptr - ss->lbuf;
	    if (status)
				goto out;
		}
		if (rows_left > 0 && --rows_left == 0) 
		{
	    status = EOFC;
	    goto out;
		}
		if (ss->K != 0) 
		{
	    uchar *prev_bits = ss->lprev;

	    ss->lprev = ss->lbuf;
	    ss->lbuf = prev_bits;
	    if (ss->K > 0)
				k_left = (k_left == 0 ? ss->K : k_left) - 1;
		}
		ss->rpos = ss->wpos = -1;
		ss->eol_count = eol_count = 0;
		ss->cbit = 0;
		ss->invert = (ss->BlackIs1 ? 0 : 0xff);
		memset(ss->lbuf, ss->invert, wstop + 1);
		ss->run_color = 0;
		if (ss->EncodedByteAlign & !ss->EndOfLine)
	    ss->bits_left &= ~7;
  }
  
  if (ss->wpos < 0) 
  {
		while ((status = cf_decode_eol(ss, pr)) > 0) 
		{
	    if (ss->K > 0) 
	    {
				hcd_declare_state;
				hcd_load_state();
				ensure_bits(1, out);
				k_left = (peek_bits(1) ? 0 : 1);
				skip_bits(1);
				hcd_store_state();
	    }
	    ++eol_count;
	    if (eol_count == (ss->K < 0 ? 2 : 6)) 
	    {
				status = EOFC;
				goto out;
	    }
		}
		if (status == 0)
	    goto out;
		switch (eol_count) 
		{
	    case 0:
				if (ss->EndOfLine) 
				{	
		    	status = ERRC;
		    	goto check;
				}
				
	    case 1:
				break;
				
	    default:
				status = ERRC;
				goto check;
		}
  }
  
  if (k_left < 0) 
  {
		status = cf_decode_2d(ss, pr);
  } 
  else if (k_left == 0) 
  {
		status = cf_decode_1d(ss, pr);
  } 
  else 
  {
		status = cf_decode_2d(ss, pr);
  }
   
check:
	switch (status) 
	{
		case 1:	
	    goto top;
	    
		case ERRC:
	    if (ss->damaged_rows >= ss->DamagedRowsBeforeError ||
				!(ss->EndOfLine && ss->K >= 0))
				break;
	    {
				ss->wpos = wstop;
				ss->cbit = -ss->Columns & 7;
				ss->run_color = 0;
	    }
	    ss->skipping_damage = true;
	    goto top;
	    
		default:
	    ss->damaged_rows = 0;
  }
  
out:
	ss->k_left = k_left;
  ss->rows_left = rows_left;
  ss->eol_count = eol_count;
  return status;
}

PSStreamState* s_create_CFD_state()
{
	return (PSStreamState*)(new PSStreamCFDState);
}

int s_CFE_init(PSStreamState*st)
{
	PSStreamCFEState *const ss = (PSStreamCFEState *) st;
	int columnsA = ss->Columns;
	int code_bytesA = ((columnsA * (ss->K == 0 ? 9 : 12)) >> 4) + 20;
	int rasterA = ss->raster = ROUND_UP((columnsA + 7) >> 3, ss->DecodedByteAlign);
	ss->bits = 0;
	ss->bits_left = hc_bits_size;
	ss->lbuf = ss->lprev = ss->lcode = 0;
	if (columnsA > cfe_max_width)
		return ERRC;
	
	ss->lbuf = (uchar*)malloc((rasterA + 4) * sizeof(uchar));
  ss->lcode = (uchar*)malloc((code_bytesA+1) * sizeof(uchar));
  if (ss->K != 0)
  {
  	ss->lprev = (uchar*)malloc((rasterA + 4) * sizeof(uchar));
  	memset(ss->lprev, (ss->BlackIs1 ? 0 : 0xff), rasterA);
		if (columnsA & 7)
	    ss->lprev[rasterA - 1] ^= 0x80 >> (columnsA & 7);
		else
	    ss->lprev[rasterA] = ~ss->lprev[0];
  }
  ss->read_count = rasterA;
  ss->write_count = 0;
  ss->k_left = (ss->K > 0 ? 1 : ss->K);
  ss->max_code_bytes = code_bytesA;
  return 0;
}

int s_CFE_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool last)
{
	PSStreamCFEState *const ss = (PSStreamCFEState *) st;
	uchar *rlimit = pr->limit;
  uchar *wlimit = pw->limit;
  int rasterA = ss->raster;
  uchar end_maskA = 1 << (-ss->Columns & 7);
  int status = 0;
  for (;;)
  {
  	if (ss->write_count)
  	{
  		int wcount = wlimit - pw->ptr;
	    int ccount = qMin(wcount, ss->write_count);
	    memcpy(pw->ptr + 1, ss->lcode + ss->code_bytes - ss->write_count,ccount);
	    pw->ptr += ccount;
	    if ((ss->write_count -= ccount) > 0) 
	    {
				status = 1;
				break;
	    }
  	}
  	
  	if (ss->read_count)
  	{
  		int rcount = rlimit - pr->ptr;
	    int ccount = qMin(rcount, ss->read_count);
	    if (rcount == 0 && last)
				break;
				
	    memcpy(ss->lbuf + rasterA - ss->read_count, pr->ptr + 1, ccount);
	    pr->ptr += ccount;
	    if ((ss->read_count -= ccount) != 0)
				break;
  	}
  	
  	{
	    uchar *endA = ss->lbuf + rasterA - 1;
	    uchar end_bitA = *endA & end_maskA;
	    uchar not_bitA = end_bitA ^ end_maskA;

	    *endA &= -end_maskA;
	    if (end_maskA == 1)
				endA[1] = (end_bitA ? 0x40 : 0x80);
	    else if (end_maskA == 2)
				*endA |= not_bitA >> 1, endA[1] = end_bitA << 7;
	    else
				*endA |= (not_bitA >> 1) | (end_bitA >> 2);
		}
		
		PSStreamCursorWrite w;
		if (wlimit - pw->ptr >= ss->max_code_bytes) 
		{
	    w = *pw;
		} 
		else 
		{
	    w.ptr = ss->lcode - 1;
	    w.limit = w.ptr + ss->max_code_bytes;
		}
		
		if (ss->EndOfLine)
		{
			const cfe_run *rp =  (ss->K <= 0 ? &cf_run_eol : ss->k_left > 1 ? &cf2_run_eol_2d :  &cf2_run_eol_1d);
	    cfe_run run;
	    
	    uint bitsA = ss->bits;
			int bits_leftA = ss->bits_left;
			if (ss->EncodedByteAlign) 
			{
				run = *rp;
				run.code_length += (bits_leftA - run_eol_code_length) & 7;
				if (run.code_length > 16)	/* <= 23 */
		    bits_leftA -= run.code_length & 7;
		    run.code_length = 16;
				rp = &run;
				if ((bits_leftA -= rp->code_length) >= 0)
					bitsA += rp->code << bits_leftA;
				else
				{
					hc_put_code_proc(ss->FirstBitLowOrder, w.ptr += hc_bits_size >> 3, (bitsA + (rp->code >> -bits_leftA)));
					bitsA = rp->code << (bits_leftA += hc_bits_size);
				}
				
				ss->bits = bitsA; 
				ss->bits_left = bits_leftA;
	    }
		}
		else if (ss->EncodedByteAlign)
			ss->bits_left &= ~7;
			
		if (ss->K == 0)
	    cf_encode_1d(ss, ss->lbuf, &w);
		else if (ss->K < 0)
	    cf_encode_2d(ss, ss->lbuf, &w, ss->lprev);
		else if (--(ss->k_left))
	    cf_encode_2d(ss, ss->lbuf, &w, ss->lprev);
		else 
		{
	    cf_encode_1d(ss, ss->lbuf, &w);
	    ss->k_left = ss->K;
		}
		
		if (w.limit == wlimit)
	    pw->ptr = w.ptr;
		else
	    ss->write_count = ss->code_bytes = w.ptr - (ss->lcode - 1);
		if (ss->K != 0) 
		{
	    uchar *temp = ss->lbuf;

	    ss->lbuf = ss->lprev;
	    ss->lprev = temp;
		}
		ss->read_count = rasterA;
  }
  
  if (last && status == 0) 
  {
		const cfe_run *rp =	(ss->K > 0 ? &cf2_run_eol_1d : &cf_run_eol);
		int i = (!ss->EndOfBlock ? 0 : ss->K < 0 ? 2 : 6);
		uint bits_to_writeA =	hc_bits_size - ss->bits_left + i * rp->code_length;
		uchar *q = pw->ptr;
		if (wlimit - q < (bits_to_writeA + 7) >> 3) 
		{
	    status = 1;
	    goto out;
		}
		uint bitsA = ss->bits;
		int bits_leftA = ss->bits_left;
		if (ss->EncodedByteAlign)
	    bits_leftA &= ~7;
		while (--i >= 0)
		{
	    if ((bits_leftA -= rp->code_length) >= 0)
				bitsA += rp->code << bits_leftA;
			else
			{
				hc_put_code_proc(ss->FirstBitLowOrder, q += hc_bits_size >> 3, (bitsA + (rp->code >> -bits_leftA)));
				bitsA = rp->code << (bits_leftA += hc_bits_size);
			}
	  }
	  
		pw->ptr = hc_put_last_bits_proc((PSStreamHCState*)(ss), q, bitsA, bits_leftA);
  }
  
out:
	return status;
}

void s_CFE_release(PSStreamState * st)
{
	PSStreamCFEState *const ss = (PSStreamCFEState *) st;
	if (ss->lprev)
	{
		free(ss->lprev);
		ss->lprev = 0;
	}
	
	if (ss->lcode)
	{
		free(ss->lcode);
		ss->lcode = 0;
	}
	
	if (ss->lbuf)
	{
		free(ss->lbuf);
		ss->lbuf = 0;
	}
}

void s_CFE_set_defaults(PSStreamState * st)
{
	PSStreamCFEState *const ss = (PSStreamCFEState *) st;

  s_CF_set_defaults_inline((PSStreamCFState*)ss);
  ss->lcode = 0;
}

PSStreamState* s_create_CFE_state()
{
	return (PSStreamState*)(new PSStreamCFEState);
}

#define pn1 ((const PSCountNode *)p1)
#define pn2 ((const PSCountNode *)p2)

static int
compare_freqs(const void *p1, const void *p2)
{
    long diff = pn2->freq - pn1->freq;

    return (diff < 0 ? -1 : diff > 0 ? 1 : 0);
}

static int
compare_code_lengths(const void *p1, const void *p2)
{
    int diff = pn1->code_length - pn2->code_length;

    return (diff < 0 ? -1 : diff > 0 ? 1 : compare_freqs(p1, p2));
}

static int
compare_values(const void *p1, const void *p2)
{
    return (pn1->value < pn2->value ? -1 :
	    pn1->value > pn2->value ? 1 : 0);
}
#undef pn1
#undef pn2

static void
hc_limit_code_lengths(PSCountNode * nodes, uint num_values, int max_length)
{
  int needed;	
  PSCountNode *longest = nodes + num_values;

  {	
		int length = longest[-1].code_length;
		int next_length;
		int avail = 0;

		while ((next_length = longest[-1].code_length) > max_length) 
		{
	    avail >>= length - next_length;
	    length = next_length;
	    (--longest)->code_length = max_length;
	    ++avail;
		}
		needed = (nodes + num_values - longest) -   (avail >>= (length - max_length));
  }
  
  while (longest[-1].code_length == max_length)
		--longest;
		
  for (; needed > 0; --needed) 
  {
		int M1 = ++(longest[-1].code_length);

		switch (max_length - M1) 
		{
	    case 0:	
				--longest;
				break;
				
	    case 1:	
				longest++->code_length = M1;
				break;
				
	    default:
				longest->code_length = M1 + 1;
				longest[1].code_length = M1 + 1;
				longest[2].code_length--;
				longest += 3;
		}
  }
}

int hc_compute(PSHCDefinition * def, const long *freqs)
{
	uint num_values = def->num_values;
  PSCountNode *nodes = (PSCountNode *)malloc((num_values * 2) * sizeof(PSCountNode)); 
  int i;
  PSCountNode *lowest;
  PSCountNode *comb;

  for (i = 0; i < num_values; ++i)
		nodes[i].freq = freqs[i], nodes[i].value = i;
		
  qsort(nodes, num_values, sizeof(PSCountNode), compare_freqs);
  for (i = 0; i < num_values; ++i)
		nodes[i].next = &nodes[i - 1],  nodes[i].code_length = 0, nodes[i].left = nodes[i].right = 0;
  nodes[0].next = 0;
  
  for (lowest = &nodes[num_values - 1], comb = &nodes[num_values];; ++comb) 
  {
		PSCountNode *pn1 = lowest;
		PSCountNode *pn2 = pn1->next;
		long freq = pn1->freq + pn2->freq;

		lowest = pn2->next;
		comb->freq = freq;
		if (pn1->code_length <= pn2->code_length)
	    comb->left = pn1, comb->right = pn2,	comb->code_length = pn2->code_length + 1;
		else
	    comb->left = pn2, comb->right = pn1, comb->code_length = pn1->code_length + 1;
		if (lowest == 0)
	    break;
	    
		if (freq < lowest->freq)
	    comb->next = lowest, lowest = comb;
		else 
		{
	    PSCountNode *here = lowest;

	    while (here->next != 0 && freq >= here->next->freq)
				here = here->next;
	    comb->next = here->next;
	    here->next = comb;
		}
  }

  comb++->code_length = 0;
  while (comb > nodes + num_values) 
  {
		--comb;
		comb->left->code_length = comb->right->code_length =  comb->code_length + 1;
  }
  
  qsort(nodes, num_values, sizeof(PSCountNode), compare_code_lengths);
  hc_limit_code_lengths(nodes, num_values, def->num_counts);
  for (i = num_values; i > 0;) 
  {
		int j = i;
		int len = nodes[--j].code_length;

		while (j > 0 && nodes[j - 1].code_length == len)
	    --j;
		qsort(&nodes[j], i - j, sizeof(PSCountNode), compare_values);
		i = j;
  }

  memset(def->counts, 0, sizeof(*def->counts) * (def->num_counts + 1));
  for (i = 0; i < num_values; ++i) 
  {
		def->values[i] = nodes[i].value;
		def->counts[nodes[i].code_length]++;
  }
  
  free(nodes);
  return 0;
}

int hc_bytes_from_definition(uchar * dbytes, const PSHCDefinition * def)
{
	int i, j;
  uchar *bp = dbytes;
  const uchar *lp = dbytes;
  const uchar *end = dbytes + def->num_values;
  const ushort *values = def->values;

  for (i = 1; i <= def->num_counts; i++)
		for (j = 0; j < def->counts[i]; j++)
	    bp[*values++] = i;

  while (lp < end) 
  {
		const uchar *vp;
		uchar len = *lp;

		for (vp = lp + 1; vp < end && vp < lp + 16 && *vp == len;)
	    vp++;
		*bp++ = ((vp - lp - 1) << 4) + (len - 1);
		lp = vp;
  }

  return bp - dbytes;
}

void hc_sizes_from_bytes(PSHCDefinition * def, const uchar * dbytes, int num_bytes)
{
	uint num_counts = 0, num_values = 0;
  int i;

  for (i = 0; i < num_bytes; i++) 
  {
		int n = (dbytes[i] >> 4) + 1;
		int l = (dbytes[i] & 15) + 1;

		if (l > num_counts)
	    num_counts = l;
		num_values += n;
  }
  def->num_counts = num_counts;
  def->num_values = num_values;
}

void hc_definition_from_bytes(PSHCDefinition * def, const uchar * dbytes)
{
	int v, i;
  ushort counts[max_hc_length + 1];
  
  memset(counts, 0, sizeof(counts[0]) * (def->num_counts + 1));
  for (i = 0, v = 0; v < def->num_values; i++) 
  {
		int n = (dbytes[i] >> 4) + 1;
		int l = (dbytes[i] & 15) + 1;

		counts[l] += n;
		v += n;
  }
  
  memcpy(def->counts, counts, sizeof(counts[0]) * (def->num_counts + 1));
  for (i = 1, v = 0; i <= def->num_counts; i++) 
  {
		uint prev = counts[i];

		counts[i] = v;
		v += prev;
  }
  for (i = 0, v = 0; v < def->num_values; i++) 
  {
		int n = (dbytes[i] >> 4) + 1;
		int l = (dbytes[i] & 15) + 1;
		int j;

		for (j = 0; j < n; n++)
	    def->values[counts[l]++] = v++;
  }
}

void hc_make_encoding(hce_code * encode, const PSHCDefinition * def)
{
	uint next = 0;
  const ushort *pvalue = def->values;
  uint i, k;

  for (i = 1; i <= def->num_counts; i++) 
  {
		for (k = 0; k < def->counts[i]; k++, pvalue++, next++) 
		{
	    hce_code *pce = encode + *pvalue;

	    pce->code = next;
	    pce->code_length = i;
		}
		next <<= 1;
  }
}

uint hc_sizeof_decoding(const PSHCDefinition * def, int initial_bits)
{
	uint size = 1 << initial_bits;
  uint carry = 0, mask = (uint) ~ 1;
  uint i;

  for (i = initial_bits + 1; i <= def->num_counts; i++, carry <<= 1, mask <<= 1) 
  {
		carry += def->counts[i];
		size += carry & mask;
		carry &= ~mask;
  }
  return size;
}

void hc_make_decoding(hcd_code * decode, const PSHCDefinition * def, int initial_bits)
{
	{
		hcd_code *pcd = decode;
		const ushort *pvalue = def->values;
		uint i, k, d;

		for (i = 0; i <= initial_bits; i++) 
		{
	    for (k = 0; k < def->counts[i]; k++, pvalue++) 
	    {
				for (d = 1 << (initial_bits - i); d > 0; d--, pcd++)
		    	pcd->value = *pvalue,
				pcd->code_length = i;
	    }
		}
  }
  
  {
		uint dsize = hc_sizeof_decoding(def, initial_bits);
		hcd_code *pcd = decode + (1 << initial_bits);
		hcd_code *pcd2 = decode + dsize;
		const ushort *pvalue = def->values + def->num_values;
		uint entries_left = 0, slots_left = 0, mult_shift = 0;
		uint i = def->num_counts + 1, j;

		for (;;) 
		{
	    if (slots_left == 0) 
	    {
				if (entries_left != 0) 
				{
		    	slots_left = 1 << (i - initial_bits);
		    	mult_shift = 0;
		    	continue;
				}
				if (--i <= initial_bits)
		    	break;
				entries_left = def->counts[i];
				continue;
	    }
	    if (entries_left == 0) 
	    {
				entries_left = def->counts[--i];
				mult_shift++;
				continue;
	    }
	    --entries_left, --pvalue;
	    for (j = 1 << mult_shift; j > 0; j--) 
	    {
				--pcd2;
				pcd2->value = *pvalue;
				pcd2->code_length = i - initial_bits;
	    }
	    if ((slots_left -= 1 << mult_shift) == 0) 
	    {
				--pcd;
				pcd->value = pcd2 - decode;
				pcd->code_length = i + mult_shift;
	    }
		}
  }
}

int s_BHCD_reinit(PSStreamState * st)
{
	PSStreamBHCDState *const ss = (PSStreamBHCDState *) st;

  ss->decode.count = ss->definition.num_values;
  ss->bits = 0;
  ss->bits_left = 0;
  ss->zeros = 0;
  return 0;
}

int s_BHCD_init(PSStreamState * st)
{
	PSStreamBHCDState *const ss = (PSStreamBHCDState *) st;
  uint initial_bits = ss->decode.initial_bits = qMin((uint)hcd_initial_bits, (uint)(ss->definition.num_counts));
  uint dsize = hc_sizeof_decoding(&ss->definition, initial_bits);
  hcd_code *decode = ss->decode.codes =	(hcd_code *)malloc((dsize+1) * sizeof(hcd_code)); 
  hc_make_decoding(decode, &ss->definition, initial_bits);
  st->min_left = 1;
  return s_BHCD_reinit(st);
}

void s_BHCD_release(PSStreamState * st)
{
	PSStreamBHCDState *const ss = (PSStreamBHCDState *) st;

  if (ss->decode.codes);
  {
  	free(ss->decode.codes);
  	ss->decode.codes = 0;
  }
}

int s_BHCD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool )
{
	PSStreamBHCDState *const ss = (PSStreamBHCDState *) st;

  uchar *p;
	uchar *rlimit;
	uint bits;
	int bits_left;
	int zeros;
	
  uchar *q = pw->ptr;
  uchar *wlimit = pw->limit;
  const hcd_code *decode = ss->decode.codes;
  uint initial_bits = ss->decode.initial_bits;
  uint zero_runs = ss->EncodeZeroRuns;
  int status = 0;
  int eod = (ss->EndOfData ? ss->definition.num_values - 1 : -1);

  bhcd_load_state();
  
z:
	for (; zeros > 0; --zeros) 
	{
		if (q >= wlimit) 
		{
	    status = 1;
	    goto out;
		}
		*++q = 0;
  }
  for (;;) 
  {
		const hcd_code *cp;
		int clen;

		hcd_ensure_bits(initial_bits, x1);
		cp = &decode[hcd_peek_var_bits(initial_bits)];
      
w1:
		if (q >= wlimit) 
		{
	    status = 1;
	    break;
		}
		if ((clen = cp->code_length) > initial_bits) 
		{
	    if (!hcd_bits_available(clen)) 
	    {
				break;
	    }
	    clen -= initial_bits;
	    hcd_skip_bits(initial_bits);
	    hcd_ensure_bits(clen, out);		/* can't exit */
	    cp = &decode[cp->value + hcd_peek_var_bits(clen)];
	    hcd_skip_bits(cp->code_length);
		} 
		else 
		{
	    hcd_skip_bits(clen);
		}
		if (cp->value >= zero_runs) 
		{
	    if (cp->value == eod) 
	    {
				status = EOFC;
				goto out;
	    }
	    zeros = cp->value - zero_runs + 2;
	    goto z;
		}
		*++q = cp->value;
		continue;
		
x1:
		cp = &decode[(bits & ((1 << bits_left) - 1)) << (initial_bits - bits_left)];
		if ((clen = cp->code_length) <= bits_left)
	    goto w1;
		break;
  }
  
out:
	bhcd_store_state();
  pw->ptr = q;
  return status;
}

PSStreamState * s_create_BHCD_state()
{
	return (PSStreamState * )(new PSStreamBHCDState);
}

int s_BHCE_reinit(PSStreamState * st)
{
	PSStreamBHCEState *const ss = (PSStreamBHCEState *) st;

  ss->encode.count = ss->definition.num_values;
  ss->bits = 0;
  ss->bits_left = hc_bits_size;
  ss->zeros = 0;
  return 0;
}

int s_BHCE_init(PSStreamState * st)
{
	PSStreamBHCEState *const ss = (PSStreamBHCEState *) st;
  hce_code *encode = ss->encode.codes = (hce_code*)malloc((ss->definition.num_values+1) * sizeof(hce_code));
  hc_make_encoding(encode, &ss->definition);
  return s_BHCE_reinit(st);
}

void s_BHCE_release(PSStreamState * st)
{
	PSStreamBHCEState *const ss = (PSStreamBHCEState *) st;

  if (ss->encode.codes)
  {
  	free(ss->encode.codes);
  	ss->encode.codes = 0;
  }
}

int s_BHCE_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last)
{
	PSStreamBHCEState *const ss = (PSStreamBHCEState *) st;
  uchar *p = pr->ptr;
  uchar *rlimit = pr->limit;
  uchar *q = pw->ptr;
  uchar *wlimit = pw->limit - (hc_bits_size >> 3);
  hce_code *encode = ss->encode.codes;
  uint num_values = ss->definition.num_values;
  uint zero_runs = ss->EncodeZeroRuns;
  uint zero_max = num_values - zero_runs + (ss->EndOfData ? 0 : 1);
  uint zero_value = (zero_max > 1 ? 0 : 0x100);
  int zeros = ss->zeros;
  int status = 0;

  uint bits;\
	int bits_left;

  bits = ss->bits, bits_left = ss->bits_left;
  while (p < rlimit && q < wlimit) 
  {
		uint value = *++p;
		const hce_code *cp;

		if (value >= num_values) 
		{
	    status = ERRC;
	    break;
		}
		if (value == zero_value) 
		{
	    ++zeros;
	    if (zeros != zero_max)
				continue;
				
	    cp = &encode[zeros - 2 + zero_runs];
	    zeros = 0;
	    hc_put_code((PSStreamHCState *) ss, q, cp);
	    continue;
		}
		if (zeros > 0) 
		{
	    --p;
	    cp = (zeros == 1 ? &encode[0] :
		  &encode[zeros - 2 + zero_runs]);
	    zeros = 0;
	    hc_put_code((PSStreamHCState *) ss, q, cp);
	    continue;
		}
		cp = &encode[value];
		hc_put_code((PSStreamHCState *) ss, q, cp);
  }
  
  if (q >= wlimit)
		status = 1;
  wlimit = pw->limit;
  if (last && status == 0) 
  {
		if (zeros > 0) 
		{
	    const hce_code *cp = (zeros == 1 ? &encode[0] :  &encode[zeros - 2 + zero_runs]);

	    if (!hce_bits_available(cp->code_length))
				status = 1;
	    else 
	    {
				hc_put_code((PSStreamHCState *) ss, q, cp);
				zeros = 0;
	    }
		}
		if (ss->EndOfData) 
		{
	    const hce_code *cp = &encode[num_values - 1];

	    if (!hce_bits_available(cp->code_length))
				status = 1;
	    else
				hc_put_code((PSStreamHCState *) ss, q, cp);
		} 
		else 
		{
	    if (q >= wlimit)
				status = 1;
		}
		if (!status) 
		{
	    q = hc_put_last_bits((PSStreamHCState *) ss, q);
	    goto ns;
		}
  }
  
  hce_store_state();
  
ns:
	pr->ptr = p;
  pw->ptr = q;
  ss->zeros = zeros;
  return (p == rlimit ? 0 : 1);
}

PSStreamState* s_create_BHCE_state()
{
	return (PSStreamState* )(new PSStreamBHCEState);
}

static void * zlibAlloc(void *zmem, uint items, uint size)
{
	PSZLibDynamicState * zds = (PSZLibDynamicState*)zmem;
	zlib_block *block = (zlib_block*)malloc(sizeof(zlib_block));
	void *data = malloc(items * size+1);
	block->data = data;
  block->next = zds->blocks;
  block->prev = 0;
  if (zds->blocks)
		zds->blocks->prev = block;
  zds->blocks = block;
  return data;
}

static void zlibFree(void *zmem, void *data)
{
	PSZLibDynamicState * zds = (PSZLibDynamicState*)zmem;
  zlib_block *block = zds->blocks;
  for (; ; block = block->next)
  {
  	if (block == 0)
  		return ;
  		
  	if (block->data == data)
	    break;
  }
  
  if (block->next)
		block->next->prev = block->prev;
  if (block->prev)
		block->prev->next = block->next;
  else
		zds->blocks = block->next;
		
	free(block);
}

int s_zlib_alloc_dynamic_state(PSStreamZLibState *ss)
{
	PSZLibDynamicState *zds = (PSZLibDynamicState*)malloc(sizeof(PSZLibDynamicState));
	ss->dynamic = zds;
	zds->blocks = 0;
	zds->zstate.zalloc = (alloc_func)zlibAlloc;
  zds->zstate.zfree = (free_func)zlibFree;
  zds->zstate.opaque = (voidpf)zds;
  return 0;
}

void s_zlib_free_dynamic_state(PSStreamZLibState *ss)
{
	if (ss->dynamic)
	{
		free(ss->dynamic);
		ss->dynamic = 0;
	}
}

void s_zlib_set_defaults(PSStreamState * st)
{
	PSStreamZLibState *const ss = (PSStreamZLibState *)st;

  ss->windowBits = MAX_WBITS;
  ss->no_wrapper = false;
  ss->level = Z_DEFAULT_COMPRESSION;
  ss->method = Z_DEFLATED;
  ss->memLevel = qMin(MAX_MEM_LEVEL, 8);
  ss->strategy = Z_DEFAULT_STRATEGY;
  ss->dynamic = 0;
}

int s_zlibD_init(PSStreamState * st)
{
	PSStreamZLibState *const ss = (PSStreamZLibState *)st;
  int code = s_zlib_alloc_dynamic_state(ss);

  if (code < 0)
		return ERRC;
  
  if (inflateInit2(&ss->dynamic->zstate, (ss->no_wrapper ? -ss->windowBits : ss->windowBits)) != Z_OK) 
  {
		s_zlib_free_dynamic_state(ss);
		return ERRC;
  }
  st->min_left=1;
  return 0;
}

int s_zlibD_reset(PSStreamState * st)
{
	PSStreamZLibState *const ss = (PSStreamZLibState *)st;

  if (inflateReset(&ss->dynamic->zstate) != Z_OK)
		return ERRC;
  return 0;
}

int s_zlibD_process(PSStreamState * st, PSStreamCursorRead * pr,	PSStreamCursorWrite * pw, bool)
{
	PSStreamZLibState *const ss = (PSStreamZLibState *)st;
  z_stream *zs = &ss->dynamic->zstate;
  uchar *p = pr->ptr;
  int status;

  if (pw->ptr == pw->limit)
		return 1;
  
  if (pr->ptr == pr->limit)
		return 0;
    
  zs->next_in = (Bytef *)p + 1;
  zs->avail_in = pr->limit - p;
  zs->next_out = pw->ptr + 1;
  zs->avail_out = pw->limit - pw->ptr;
  status = inflate(zs, Z_PARTIAL_FLUSH);
  pr->ptr = zs->next_in - 1;
  pw->ptr = zs->next_out - 1;
  switch (status) 
  {
		case Z_OK:
	    return (pw->ptr == pw->limit ? 1 : pr->ptr > p ? 0 : 1);
		case Z_STREAM_END:
	    return EOFC;
		default:
	    return ERRC;
  }
}

void s_zlibD_release(PSStreamState * st)
{
	PSStreamZLibState *const ss = (PSStreamZLibState *)st;

  inflateEnd(&ss->dynamic->zstate);
  s_zlib_free_dynamic_state(ss);
}

int s_zlibE_init(PSStreamState * st)
{
	PSStreamZLibState *const ss = (PSStreamZLibState *)st;
	int code = s_zlib_alloc_dynamic_state(ss);
  if (code < 0)
		return ERRC;
  
  if (deflateInit2(&ss->dynamic->zstate, ss->level, 
  	               ss->method,  
  	               (ss->no_wrapper ? -ss->windowBits : ss->windowBits),
		                 ss->memLevel, ss->strategy) != Z_OK)
		return ERRC;
  return 0;
}

int s_zlibE_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool last)
{
	PSStreamZLibState *const ss = (PSStreamZLibState *)st;
	z_stream *zs = &(ss->dynamic->zstate);
  uchar *p = pr->ptr;

  if (pw->ptr == pw->limit)
		return 1;
  if (p == pr->limit && !last)
		return 0;
		
	zs->next_in = (Bytef *)p + 1;
  zs->avail_in = pr->limit - p;

  zs->next_out = pw->ptr + 1;
  zs->avail_out = pw->limit - pw->ptr;
  int status = deflate(zs, (last ? Z_FINISH : Z_NO_FLUSH));
  pr->ptr = zs->next_in - 1;
  pw->ptr = zs->next_out - 1;
  switch (status) 
  {
		case Z_OK:
	    return (pw->ptr == pw->limit ? 1 : pr->ptr > p && !last ? 0 : 1);
		case Z_STREAM_END:
	    return (last && pr->ptr == pr->limit ? 0 : ERRC);
		default:
	    return ERRC;
  }
}

void s_zlibE_release(PSStreamState * st)
{
	PSStreamZLibState *const ss = (PSStreamZLibState *)st;

  deflateEnd(&ss->dynamic->zstate);
  s_zlib_free_dynamic_state(ss);
}

int  s_zlibE_reset(PSStreamState * st)
{
	PSStreamZLibState *const ss = (PSStreamZLibState *)st;
	if (deflateReset(&ss->dynamic->zstate) != Z_OK)
		return ERRC;
  return 0;
}

PSStreamState * s_create_zlib_state()
{
	return (PSStreamState*)(new PSStreamZLibState);
}

int s_exE_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool)
{
	PSStreamExEState *const ss = (PSStreamExEState *) st;
	uchar *p = pr->ptr;
  uchar *q = pw->ptr;
  uint rcount = pr->limit - p;
  uint wcount = pw->limit - q;
  uint count;
  int status;

  if (rcount <= wcount)
		count = rcount, status = 0;
  else
		count = wcount, status = 1;
  ps_type1_encrypt(q + 1, p + 1, count, (ushort *)&ss->cstate);
  pr->ptr += count;
  pw->ptr += count;
  return status;
}

PSStreamState * s_create_exE_state()
{
	return (PSStreamState*)(new PSStreamExEState);
}

#define num_weight_bits 0	
#define fixedScaleFactor 1	
#define scale_PixelWeight(factor) (factor)
#define unscale_AccumTmp(atemp, fraction_bits) ((int)(atemp))

#define minPixelTmp 0
#define maxPixelTmp 255
#define unitPixelTmp 255

#define maxSizeofPixel 2

#define Mitchell_support 2.0
#define B (1.0 / 3.0)
#define C (1.0 / 3.0)
static double
Mitchell_filter(double t)
{
    double t2 = t * t;

    if (t < 0)
	t = -t;

    if (t < 1)
	return
	    ((12 - 9 * B - 6 * C) * (t * t2) +
	     (-18 + 12 * B + 6 * C) * t2 +
	     (6 - 2 * B)) / 6;
    else if (t < 2)
	return
	    ((-1 * B - 6 * C) * (t * t2) +
	     (6 * B + 30 * C) * t2 +
	     (-12 * B - 48 * C) * t +
	     (8 * B + 24 * C)) / 6;
    else
	return 0;
}

#undef B
#undef C

#define filter_support Mitchell_support
#define filter_proc Mitchell_filter
#define fproc(t) filter_proc(t)
#define fWidthIn filter_support

#define CLAMP(v, mn, mx)\
  (v < mn ? mn : v > mx ? mx : v)
  	
#define min_scale ((fWidthIn * 2) / (MAX_ISCALE_SUPPORT - 1.01))

static int
contrib_pixels(double scale)
{
    return (int)(fWidthIn / (scale >= 1.0 ? 1.0 : qMax(scale, (double)min_scale)) * 2 + 1);
}

static int
calculate_contrib( CLIST * contrib,
		     CONTRIB * items,
		     double scale,
		     int input_index,
		     int size,
		     int limit,
		     int modulus,
		     int stride,
		     double rescale_factor
)
{
  double scaled_factor = scale_PixelWeight(rescale_factor);
  double WidthIn, fscale;
  bool squeeze;
  int npixels;
  int i, j;
  int last_index = -1;

  if (scale < 1.0) 
  {
		double clamped_scale = qMax(scale, (double)min_scale);

		WidthIn = fWidthIn / clamped_scale;
		fscale = 1.0 / clamped_scale;
		squeeze = true;
   } 
   else 
   {
		WidthIn = fWidthIn;
			fscale = 1.0;
			squeeze = false;
    }
    npixels = (int)(WidthIn * 2 + 1);

    for (i = 0; i < size; ++i) 
    {
			double center = (input_index + i) / scale;
			int left = (int)ceil(center - WidthIn);
			int right = (int)floor(center + WidthIn);
			
#define clamp_pixel(j)\
  (j < 0 ? (-j >= limit ? limit - 1 : -j) :\
   j >= limit ? (j >> 1 >= limit ? 0 : (limit - j) + limit - 1) :\
   j)
   
			int lmin =(left < 0 ? 0 : left);
			int lmax =(left < 0 ? (-left >= limit ? limit - 1 : -left) : left);
			int rmin =	(right >= limit ?	 (right >> 1 >= limit ? 0 : (limit - right) + limit - 1) : right);
			int rmax =	(right >= limit ? limit - 1 : right);
			int first_pixel = qMin(lmin, rmin);
			int last_pixel = qMax(lmax, rmax);
			CONTRIB *p;

			if (last_pixel > last_index)
	    	last_index = last_pixel;
			contrib[i].first_pixel = (first_pixel % modulus) * stride;
			contrib[i].n = last_pixel - first_pixel + 1;
			contrib[i].index = i * npixels;
			p = items + contrib[i].index;
			for (j = 0; j < npixels; ++j)
	    	p[j].weight = 0;
			if (squeeze) 
			{
	    	for (j = left; j <= right; ++j) 
	    	{
					double weight =	fproc((center - j) / fscale) / fscale;
					int n = clamp_pixel(j);
					int k = n - first_pixel;

					p[k].weight +=  (float) (weight * scaled_factor);
	    	}
			} 
			else 
			{
	    	for (j = left; j <= right; ++j) 
	    	{
					double weight = fproc(center - j);
					int n = clamp_pixel(j);
					int k = n - first_pixel;

					p[k].weight += (float) (weight * scaled_factor);
	    	}
			}
  	}
  return last_index;
}

static void
zoom_x(uchar * tmp, const void *src, int sizeofPixelIn,
       int tmp_width, int , int Colors, const CLIST * contrib,
       const CONTRIB * items)
{
    int c, i;
#ifdef NEED_FRACTION_BITS
    const int fraction_bits =	(sizeofPixelIn - sizeof(uchar)) * 8 + num_weight_bits;
#endif

  for (c = 0; c < Colors; ++c) 
  {
		uchar *tp = tmp + c;
		const CLIST *clp = contrib;

#define zoom_x_loop(PixelIn, PixelIn2)\
		const PixelIn *raster = (const PixelIn *)src + c;\
		for ( i = 0; i < tmp_width; tp += Colors, ++clp, ++i )\
		  {	double weight = 0;\
			{ int j = clp->n;\
			  const PixelIn *pp = raster + clp->first_pixel;\
			  const CONTRIB *cp = items + clp->index;\
			  switch ( Colors )\
			  {\
			  case 1:\
			    for ( ; j > 0; pp += 1, ++cp, --j )\
			      weight += *pp * cp->weight;\
			    break;\
			  case 3:\
			    for ( ; j > 0; pp += 3, ++cp, --j )\
			      weight += *pp * cp->weight;\
			    break;\
			  default:\
			    for ( ; j > 0; pp += Colors, ++cp, --j )\
			      weight += *pp * cp->weight;\
			  }\
			}\
			{ PixelIn2 pixel = unscale_AccumTmp(weight, fraction_bits);\
			  *tp =\
			    (uchar)CLAMP(pixel, minPixelTmp, maxPixelTmp);\
			}\
		  }

		if (sizeofPixelIn == 1) 
		{
	    zoom_x_loop(uchar, int)
		} 
		else 
		{	
#if arch_ints_are_short
	    zoom_x_loop(quint16, long)
#else
	    zoom_x_loop(quint16, int)
#endif
		}
  }
}

static void
zoom_y(void *dst, int sizeofPixelOut, uint MaxValueOut,
       const uchar * tmp, int WidthOut, int,
       int Colors, const CLIST * contrib, const CONTRIB * items)
{
    int kn = WidthOut * Colors;
    int cn = contrib->n;
    int first_pixel = contrib->first_pixel;
    const CONTRIB *cbp = items + contrib->index;
    int kc;
    int max_weight = MaxValueOut;
#ifdef NEED_FRACTION_BITS
    const int fraction_bits = (sizeof(uchar) - sizeofPixelOut) * 8 + num_weight_bits;
#endif

#define zoom_y_loop(PixelOut)\
	for ( kc = 0; kc < kn; ++kc ) {\
		double weight = 0;\
		{ const uchar *pp = &tmp[kc + first_pixel];\
		  int j = cn;\
		  const CONTRIB *cp = cbp;\
		  for ( ; j > 0; pp += kn, ++cp, --j )\
		    weight += *pp * cp->weight;\
		}\
		{ int pixel = unscale_AccumTmp(weight, fraction_bits);\
		  ((PixelOut *)dst)[kc] =\
		    (PixelOut)CLAMP(pixel, 0, max_weight);\
		}\
	}

    if (sizeofPixelOut == 1) {
	zoom_y_loop(uchar)
    } else {	
	zoom_y_loop(quint16)
    }
}


#define tmp_width params.WidthOut
#define tmp_height params.HeightIn

static void
calculate_dst_contrib(PSStreamIScaleState * ss, int y)
{
  uint row_size = ss->params.WidthOut * ss->params.Colors;
  int last_index =  calculate_contrib(&ss->dst_next_list, ss->dst_items, ss->yscale,
		      				y, 1, ss->params.HeightIn, MAX_ISCALE_SUPPORT, row_size,
		      				(double)ss->params.MaxValueOut / (fixedScaleFactor * unitPixelTmp) );
  int first_index_mod = ss->dst_next_list.first_pixel / row_size;

  ss->dst_last_index = last_index;
  last_index %= MAX_ISCALE_SUPPORT;
  if (last_index < first_index_mod) 
  {	
		CONTRIB shuffle[MAX_ISCALE_SUPPORT];
		int i;

		for (i = 0; i < MAX_ISCALE_SUPPORT; ++i)
	    shuffle[i].weight =	(i <= last_index ? ss->dst_items[i + MAX_ISCALE_SUPPORT - first_index_mod].weight :
		 												i >= first_index_mod ?	 ss->dst_items[i - first_index_mod].weight : 0);
		memcpy(ss->dst_items, shuffle, MAX_ISCALE_SUPPORT * sizeof(CONTRIB));
		ss->dst_next_list.n = MAX_ISCALE_SUPPORT;
		ss->dst_next_list.first_pixel = 0;
  }
}

void s_IScale_set_defaults(PSStreamState * st)
{
	PSStreamIScaleState *const ss = (PSStreamIScaleState *) st;

  ss->src = 0;
  ss->dst = 0;
  ss->tmp = 0;
  ss->contrib = 0;
  ss->items = 0;
}

int s_IScale_init(PSStreamState * st)
{
	PSStreamIScaleState *const ss = (PSStreamIScaleState *) st;

  ss->sizeofPixelIn = ss->params.BitsPerComponentIn / 8;
  ss->sizeofPixelOut = ss->params.BitsPerComponentOut / 8;
  ss->xscale = (double)ss->params.WidthOut / (double)ss->params.WidthIn;
  ss->yscale = (double)ss->params.HeightOut / (double)ss->params.HeightIn;

  ss->src_y = 0;
  ss->src_size = ss->params.WidthIn * ss->sizeofPixelIn * ss->params.Colors;
  ss->src_offset = 0;
  ss->dst_y = 0;
  ss->dst_size = ss->params.WidthOut * ss->sizeofPixelOut * ss->params.Colors;
  ss->dst_offset = 0;

  ss->tmp = (uchar*)malloc((qMin(ss->tmp_height, MAX_ISCALE_SUPPORT) * ss->tmp_width * ss->params.Colors+1) * sizeof(uchar)); 
  
  ss->contrib = (CLIST*)malloc((qMax(ss->params.WidthOut, ss->params.HeightOut)+1) * sizeof(CLIST));
  ss->items = (CONTRIB*)malloc((contrib_pixels(ss->xscale) * ss->params.WidthOut+1) * sizeof(CONTRIB));
  ss->dst = malloc(ss->params.WidthOut * ss->params.Colors * ss->sizeofPixelOut+1);
  ss->src = malloc(ss->params.WidthIn * ss->params.Colors * ss->sizeofPixelIn+1);
  if (ss->tmp == 0 || ss->contrib == 0 || ss->items == 0 ||	ss->dst == 0 || ss->src == 0) 
  {
		s_IScale_release(st);
		return ERRC;
  }
  
  calculate_contrib(ss->contrib, ss->items, ss->xscale,
		      0, ss->params.WidthOut, ss->params.WidthIn, ss->params.WidthIn,
		      ss->params.Colors, (double)unitPixelTmp * fixedScaleFactor / ss->params.MaxValueIn);

  calculate_dst_contrib(ss, 0);

  return 0;
}

int s_IScale_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool)
{
	PSStreamIScaleState *const ss = (PSStreamIScaleState *) st;

top:
	while (ss->src_y > ss->dst_last_index) 
	{
		uint wleft = pw->limit - pw->ptr;

		if (ss->dst_y == ss->params.HeightOut)
	    return EOFC;
		if (wleft == 0)
	    return 1;
		if (ss->dst_offset == 0) 
		{
	    uchar *row;

	    if (wleft >= ss->dst_size) 
	    {
				row = pw->ptr + 1;
				pw->ptr += ss->dst_size;
	    } 
	    else 
	    {	
				row = (uchar*)(ss->dst);
	    }
	    
	    zoom_y(row, ss->sizeofPixelOut, ss->params.MaxValueOut, ss->tmp,
		   				ss->params.WidthOut, ss->tmp_width, ss->params.Colors,
		   			&ss->dst_next_list, ss->dst_items);
		   			
	    if ((void *)row != ss->dst)	
				goto adv;
		} 
		{	
	    uint wcount = ss->dst_size - ss->dst_offset;
	    uint ncopy = qMin(wleft, wcount);

	    memcpy(pw->ptr + 1, (uchar *) ss->dst + ss->dst_offset, ncopy);
	    pw->ptr += ncopy;
	    ss->dst_offset += ncopy;
	    if (ncopy != wcount)
				return 1;
	    ss->dst_offset = 0;
		}
		
adv:
		++(ss->dst_y);
		if (ss->dst_y != ss->params.HeightOut)
	    calculate_dst_contrib(ss, ss->dst_y);
  }

  {
		uint rleft = pr->limit - pr->ptr;
		uint rcount = ss->src_size - ss->src_offset;

		if (rleft == 0)
	    return 0;		
	    
		if (rleft >= rcount) 
		{
	    uchar *row;

	    if (ss->src_offset == 0) 
	    {	
				row = pr->ptr + 1;
	    } 
	    else 
	    {	
				row = (uchar*)(ss->src);
				memcpy((uchar *) ss->src + ss->src_offset, pr->ptr + 1, rcount);
				ss->src_offset = 0;
	    }
	    
	    zoom_x(ss->tmp + (ss->src_y % MAX_ISCALE_SUPPORT) *	ss->tmp_width * ss->params.Colors, row,
		   		ss->sizeofPixelIn, ss->tmp_width, ss->params.WidthIn, ss->params.Colors, ss->contrib, ss->items);
	    pr->ptr += rcount;
	    ++(ss->src_y);
	    goto top;
		} 
		else 
		{	
	    memcpy((uchar *) ss->src + ss->src_offset, pr->ptr + 1, rleft);
	    ss->src_offset += rleft;
	    pr->ptr += rleft;
	    return 0;
		}
  }
}

void s_IScale_release(PSStreamState * st)
{
	PSStreamIScaleState *const ss = (PSStreamIScaleState *) st;
	if (ss->src)
	{
		free(ss->src);
		ss->src = 0;
	}
	if (ss->dst)
	{
		free(ss->dst);
		ss->dst = 0;
	}
	if (ss->items)
	{
		free(ss->items);
		ss->items = 0;
	}
	
	if (ss->contrib)
	{
		free(ss->contrib);
		ss->contrib = 0;
	}
	
	if (ss->tmp)
	{
		free(ss->tmp);
		ss->tmp = 0;
	}
}

PSStreamState * s_create_IScale_state()
{
	return (PSStreamState*)(new PSStreamIScaleState);
}

int s_IIEncode_init(PSStreamState * st)
{
	PSStreamIIEncodeState *const ss = (PSStreamIIEncodeState *) st;

  ss->sizeofPixelIn =	ss->params.BitsPerComponentIn / 8 * ss->params.Colors;
  ss->sizeofPixelOut =	ss->params.BitsPerComponentOut / 8 * ss->params.Colors;
  ss->src_size = ss->sizeofPixelIn * ss->params.WidthIn;
  ss->dst_size = ss->sizeofPixelOut * ss->params.WidthOut;

  ss->dst_x = 0;
  ss->src_offset = ss->dst_offset = 0;
  dda_init(ss->dda_x, 0, ss->params.WidthIn, ss->params.WidthOut);
  ss->dda_x_init = ss->dda_x;
  ss->src_y = ss->dst_y = 0;
  dda_init(ss->dda_y, 0, ss->params.HeightOut, ss->params.HeightIn);

  int i = ss->params.WidthIn * ss->sizeofPixelOut;
  ss->prev = malloc(i+1);
  ss->cur = malloc(i+1);
  if (ss->prev == 0 || ss->cur == 0) 
  {
		s_IIEncode_release(st);
		return ERRC;
  }

  ss->scale_case =	(ss->params.BitsPerComponentIn == 8 ?	 (ss->params.BitsPerComponentOut == 8 ?
	  		(ss->params.MaxValueIn == ss->params.MaxValueOut ?   SCALE_SAME : SCALE_8_8) :
	  		(ss->params.MaxValueIn == 255 && ss->params.MaxValueOut == frac_1 ?
	   		(ss->params.Colors == 3 ? SCALE_8_16_BYTE2FRAC_3 :   SCALE_8_16_BYTE2FRAC) :
	   		SCALE_8_16_GENERAL)) : (ss->params.BitsPerComponentOut == 8 ? SCALE_16_8 :
	  		ss->params.MaxValueIn == ss->params.MaxValueOut ?	  SCALE_SAME : SCALE_16_16));

  return 0;
}

int s_IIEncode_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool)
{
	PSStreamIIEncodeState *const ss = (PSStreamIIEncodeState *) st;
  const PSScaleCase scale_case = (const PSScaleCase)(ss->scale_case + ALIGNMENT_MOD(pw->ptr, 2)); 
  uchar *out = pw->ptr + 1;
  
  uchar *limit = pw->limit + 1 - ss->sizeofPixelOut;

top:
  if (dda_current(ss->dda_y) > ss->dst_y) 
  {
		while (ss->dst_x < ss->params.WidthOut) 
		{
	    uint sx = dda_current(ss->dda_x) * ss->sizeofPixelIn;
	    const uchar *in = (const uchar *)ss->cur + sx;
	    int c;

	    if (out > limit) 
	    {
				pw->ptr = out - 1;
				return 1;
	    }
	    switch (scale_case) 
	    {
	    	case SCALE_SAME:
	    	case SCALE_SAME_ALIGNED:
					memcpy(out, in, ss->sizeofPixelIn);
					out += ss->sizeofPixelIn;
					break;
					
	    	case SCALE_8_8:
	    	case SCALE_8_8_ALIGNED:
					for (c = ss->params.Colors; --c >= 0; ++in, ++out)
		    		*out = (uchar)(*in * ss->params.MaxValueOut / ss->params.MaxValueIn);
					break;
					
	    	case SCALE_8_16_BYTE2FRAC:
	    	case SCALE_8_16_BYTE2FRAC_ALIGNED:
	    	case SCALE_8_16_BYTE2FRAC_3: /* could be optimized */
					for (c = ss->params.Colors; --c >= 0; ++in, out += 2) 
					{
		    		uint b = *in;
		    		uint value = byte2frac(b);

		    		out[0] = (uchar)(value >> 8), out[1] = (uchar)value;
					}
					break;
					
	    	case SCALE_8_16_BYTE2FRAC_3_ALIGNED:
					{
		    		uint b = in[0];

		    		((quint16 *)out)[0] = byte2frac(b);
		    		b = in[1];
		    		((quint16 *)out)[1] = byte2frac(b);
		    		b = in[2];
		    		((quint16 *)out)[2] = byte2frac(b);
					}
					out += 6;
					break;
					
	    	case SCALE_8_16_GENERAL:
	    	case SCALE_8_16_GENERAL_ALIGNED: /* could be optimized */
					for (c = ss->params.Colors; --c >= 0; ++in, out += 2) 
					{
		    		uint value = *in * ss->params.MaxValueOut /	ss->params.MaxValueIn;

		    		out[0] = (uchar)(value >> 8), out[1] = (uchar)value;
					}
					break;
					
	    	case SCALE_16_8:
	    	case SCALE_16_8_ALIGNED:
					for (c = ss->params.Colors; --c >= 0; in += 2, ++out)
		    		*out = (uchar)(*(const quint16 *)in *  ss->params.MaxValueOut /  ss->params.MaxValueIn);
					break;
					
	    	case SCALE_16_16:
	    	case SCALE_16_16_ALIGNED: /* could be optimized */
					for (c = ss->params.Colors; --c >= 0; in += 2, out += 2) 
					{
		    		uint value = *(const quint16 *)in *	ss->params.MaxValueOut / ss->params.MaxValueIn;

		    		out[0] = (uchar)(value >> 8), out[1] = (uchar)value;
					}
	    }
	    dda_next(ss->dda_x);
	    ss->dst_x++;
		}
		ss->dst_x = 0;
		ss->dst_y++;
		ss->dda_x = ss->dda_x_init;
		goto top;
  }
  pw->ptr = out - 1;
  if (ss->dst_y >= ss->params.HeightOut)
		return EOFC;

  if (ss->src_offset < ss->src_size) 
  {
		uint count = qMin((uint)(ss->src_size - ss->src_offset), (uint)(pr->limit - pr->ptr));

		if (count == 0)
	    return 0;
		memcpy((uchar *)ss->cur + ss->src_offset, pr->ptr + 1, count);
		ss->src_offset += count;
		pr->ptr += count;
		if (ss->src_offset < ss->src_size)
	    return 0;
  }
  ss->src_offset = 0;
  ss->dst_x = 0;
  ss->dda_x = ss->dda_x_init;
  dda_next(ss->dda_y);
  goto top;
}

void s_IIEncode_release(PSStreamState * st)
{
	PSStreamIIEncodeState *const ss = (PSStreamIIEncodeState *) st;
	if (ss->cur)
	{
		free(ss->cur);
		ss->cur = 0;
	}
	
	if (ss->prev)
	{
		free(ss->prev);
		ss->prev = 0;
	}
}

PSStreamState * s_create_IIEncode_state()
{
	return (PSStreamState*)(new PSStreamIIEncodeState);
}

#define code_reset (code_escape + 0)
#define code_eod (code_escape + 1)
#define code_0 (code_escape + 2)

#define lzw_decode_max 4096

#define CODE_RESET 0
#define CODE_EOD 1
#define CODE_0 2

static uchar * lzw_put_code(PSStreamLZWState * ss, uchar * q, uint code)
{
  uint size = ss->code_size;
  uchar cb = (ss->bits << ss->bits_left) +	(code >> (size - ss->bits_left));

  *++q = cb;
  if ((ss->bits_left += 8 - size) <= 0) 
  {
		*++q = code >> -ss->bits_left;
		ss->bits_left += 8;
  }
  ss->bits = code;
  return q;
}

int s_LZWD_init(PSStreamState * st)
{
	PSStreamLZWState *const ss = (PSStreamLZWState *) st;
	PSLZWDecode *dc = (PSLZWDecode*)malloc((lzw_decode_max + 1) * sizeof(PSLZWDecode));
  if (dc == 0)
		return ERRC;
  ss->table.decode = dc;
  return s_LZWD_reset(st);
}

int s_LZWE_init(PSStreamState * st)
{
	PSStreamLZWState *const ss = (PSStreamLZWState *) st;
	ss->InitialCodeLength = 8;
//  ss->table.encode = 0;
  return s_LZWE_reset(st);
}

int s_LZWD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last)
{
	PSStreamLZWState *const ss = (PSStreamLZWState *) st;
	
	uchar *p = pr->ptr;
  uchar *q = pw->ptr;
  
  uchar *rlimit = pr->limit;
  uchar *wlimit = pw->limit;
  int status = 0;
  int code = ss->copy_code;
  int prev_code = ss->prev_code;
  uint prev_len = ss->prev_len;
  uchar bits = ss->bits;
  int bits_left = ss->bits_left;
  int bytes_left = ss->bytes_left;
  int code_size = ss->code_size;
  int code_mask;
  int switch_code;
  int next_code = ss->next_code;
  PSLZWDecode *table = ss->table.decode;
  PSLZWDecode *dc_next = table + next_code;
  PSLZWDecode *dc;
  int code_escape = 1 << ss->InitialCodeLength;
  int eod = code_eod;
  bool low_order = ss->FirstBitLowOrder;
  uint len;
  int c;
  uchar b;
  uchar *q1;
  
#define set_code_size()\
  code_mask = (1 << code_size) - 1,\
  switch_code = code_mask + 1 - ss->EarlyChange
  
  set_code_size();
  if (!ss->BlockData)
		bytes_left = rlimit - p + 2;
		
	if (code >= 0)
	{
		int rlen = ss->copy_left;
		int wlen = wlimit - q;
		int n = len = qMin(rlen, wlen);

		c = code;
		ss->copy_left = rlen -= len;
		
		while (rlen)
	    c = table[c].prefix, rlen--;
	    
		q1 = q += len;
		n = len;
		while (--n >= 0) 
		{
	    *q1-- = (dc = &table[c])->datum;
	    c = dc->prefix;
		}
		
		if (ss->copy_left) 
		{
	    pw->ptr = q;
	    return 1;
		}
		ss->copy_code = -1;
		len = ss->copy_len;
		
		if (c == eod) 
		{
	    b = q1[1];
		} 
		else 
		{
	    for (; c != eod; c = table[c].prefix)
				b = (uchar) c;
		}
		goto add;
	}
	
top:
	if (code_size > bits_left)
	{
		if (bytes_left == 0) 
		{
	    if (p == rlimit)
				goto out;
	    bytes_left = *++p;
	    if (bytes_left == 0) 
	    {
				status = EOFC;
				goto out;
	    }
	    goto top;
		}
		if (low_order)
	    code = bits >> (8 - bits_left);
		else
	    code = (uint) bits << (code_size - bits_left);
	    
	  if (bits_left + 8 < code_size)
	  {
	  	if (bytes_left == 1) 
	  	{
				if (rlimit - p < 3)
		    	goto out;
				bytes_left = p[2];
				if (bytes_left == 0) 
				{
		    	status = EOFC;
		    	goto out;
				}
				bytes_left++;
				bits = p[1];
				p++;
	    } 
	    else 
	    {
				if (rlimit - p < 2)
		    	goto out;
				bits = p[1];
	    }
	    if (low_order)
				code += (uint) bits << bits_left;
	    else
				code += (uint) bits << (code_size - 8 - bits_left);
	    bits_left += 8;
	    bits = p[2];
	    p += 2;
	    bytes_left -= 2;
	  }
	  else
	  {
	  	if (p == rlimit)
				goto out;
	    bits = *++p;
	    bytes_left--;
	  }
	  
	  if (low_order)
	    code += (uint) bits << bits_left,	bits_left += 8 - code_size;
		else
	    bits_left += 8 - code_size,	code += bits >> bits_left;
	}
	else
	{
		if (low_order)
	    code = bits >> (8 - bits_left),	bits_left -= code_size;
		else
	    bits_left -= code_size,	code = bits >> bits_left;
	}
	
	code &= code_mask;
	if (code >= next_code)
	{
		if (code > next_code)
		{
			status = ERRC;
	    goto out;
		}
		
		for (c = prev_code; c != eod; c = table[c].prefix)
	    dc_next->datum = c;
		len = prev_len + 1;
		dc_next->len = qMin(len, (uint)255);
		dc_next->prefix = prev_code;
	}
	
reset:
  len = table[code].len;
  if (len == 255)
  {
  	if (code == code_reset) 
  	{
	    next_code = code_0;
	    dc_next = table + code_0;
	    code_size = ss->InitialCodeLength + 1;
	    set_code_size();
	    prev_code = -1;
	    goto top;
		} 
		else if (code == eod) 
		{
	    status = EOFC;
	    goto out;
		}
		
		for (c = code, len = 0; c != eod; len++)
	    c = table[c].prefix;
  }
  
  if (wlimit - q < len)
  {
  	ss->copy_code = code;
		ss->copy_left = ss->copy_len = len;
		status = 1;
		goto out;
  }
  
  dc = &table[code];
  switch (len)
 	{
 		default:
	    {
				uchar *q1 = q += len;

				c = code;
				do 
				{
		    	*q1-- = (dc = &table[c])->datum;
				}	while ((c = dc->prefix) != eod);
				b = q1[1];
	    }
	    break;
	    
		case 3:
	    q[3] = dc->datum;
	    dc = &table[dc->prefix];
		case 2:
	    q[2] = dc->datum;
	    dc = &table[dc->prefix];
		case 1:
	    q[1] = b = dc->datum;
	    q += len;
 	}
 	
add:
	if (prev_code >= 0)
	{
		if (next_code == lzw_decode_max)
		{
			if (!ss->BlockData)
			{
				if (bits_left < 8 && p >= rlimit && last)
				{
					goto out;
				}
				
				if (bits_left + ((rlimit - p) << 3) < code_size)
				{
					status = ERRC;
		    	goto out;
				}
				if (low_order) 
				{
		    	code = bits >> (8 - bits_left);
		    	code += (bits = *++p) << bits_left;
		    	if (bits_left + 8 < code_size)
						code += (bits = *++p) << (bits_left + 8);
				} 
				else 
				{
		    	code = bits & ((1 << bits_left) - 1);
		    	code = (code << 8) + (bits = *++p);
		    	if (bits_left + 8 < code_size)
						code = (code << 8) + (bits = *++p);
		    	code >>= (bits_left - code_size) & 7;
				}
				bits_left = (bits_left - code_size) & 7;
				if (code == code_reset)
		    	goto reset;
			}
			
			status = ERRC;
	    goto out;
		}
		
		dc_next->datum = b;
		dc_next->len = qMin(prev_len, (uint)254) + 1;
		dc_next->prefix = prev_code;
		dc_next++;
		if (++next_code == switch_code)
		{
			if (next_code < lzw_decode_max - 1) 
			{
				code_size++;
				set_code_size();
	    }
		}
	}
	
	prev_code = code;
    prev_len = len;
  goto top;
  
out:
	pr->ptr = p;
  pw->ptr = q;
  ss->code_size = code_size;
  ss->prev_code = prev_code;
  ss->prev_len = prev_len;
  ss->bits = bits;
  ss->bits_left = bits_left;
  ss->bytes_left = bytes_left;
  ss->next_code = next_code;
  return status;
}

int s_LZWE_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last)
{
	PSStreamLZWState *const ss = (PSStreamLZWState *) st;
	uchar *p = pr->ptr;
  uchar *rlimit = pr->limit;
  uchar *q = pw->ptr;
  uchar *wlimit = pw->limit;
  int status = 0;
  int signal = 1 << (ss->code_size - 1);
  uint limit_code = (1 << ss->code_size) - 2;
  uint next_code = ss->next_code;
  
  while (p < rlimit) 
 	{
		if (next_code == limit_code) 
		{
	    if (wlimit - q < 2) 
	    {
				status = 1;
				break;
	    }
	    q = lzw_put_code(ss, q, signal + CODE_RESET);
	    next_code = signal + CODE_0;
		}
		if (wlimit - q < 2) 
		{
	    status = 1;
	    break;
		}
		q = lzw_put_code(ss, q, *++p);
		next_code++;
  }
  if (last && status == 0) 
  {
		if (wlimit - q < 2)
	    status = 1;
		else 
		{
	    q = lzw_put_code(ss, q, signal + CODE_EOD);
	    if (ss->bits_left < 8)
				*++q = ss->bits << ss->bits_left;
		}
  }
  ss->next_code = next_code;
  pr->ptr = p;
  pw->ptr = q;
  return status;
}

void s_LZW_set_defaults(PSStreamState * st)
{
	PSStreamLZWState *const ss = (PSStreamLZWState *) st;
	ss->InitialCodeLength = 8;
	ss->FirstBitLowOrder = false;
	ss->BlockData = false;
	ss->EarlyChange = 1;
	ss->table.decode = 0;
}

void s_LZW_release(PSStreamState * st)
{
	PSStreamLZWState *const ss = (PSStreamLZWState *) st;
	if (ss->table.decode)
	{
		free(ss->table.decode);
		ss->table.decode = 0;
	}
}

int s_LZWD_reset(PSStreamState * st)
{
	PSStreamLZWState *const ss = (PSStreamLZWState *) st;
	PSLZWDecode *dc = ss->table.decode;
  uint code_escape = 1 << ss->InitialCodeLength;

  ss->bits_left = 0;
  ss->bytes_left = 0;
  ss->next_code = code_0;
  ss->code_size = ss->InitialCodeLength + 1;
  ss->prev_code = -1;
  ss->copy_code = -1;
  dc[code_reset].len = 255;
  dc[code_eod].len = 255;
  for (int i = 0; i < code_escape; i++, dc++)
		dc->datum = i, dc->len = 1, dc->prefix = code_eod;
  return 0;
}

int s_LZWE_reset(PSStreamState * st)
{
	PSStreamLZWState *const ss = (PSStreamLZWState *) st;
	ss->code_size = ss->InitialCodeLength + 1;
  ss->bits_left = 8;
  ss->next_code = (1 << ss->code_size) - 2;
  return 0;
}

PSStreamState *s_create_LZW_state()
{
	return (PSStreamState *)(new PSStreamLZWState);
}

#define cNone 10
#define cSub 11
#define cUp 12
#define cAverage 13
#define cPaeth 14
#define cOptimum 15
#define cEncode -10
#define cDecode -4
static const uchar pngp_case_needs_prev[] = {
    0, 0, 1, 1, 1, 1
};

static int s_pngp_init(PSStreamState * st, bool need_prev)
{
	PSStreamPNGPState *const ss = (PSStreamPNGPState *) st;
	int bits_per_pixel = ss->Colors * ss->BitsPerComponent;
  long bits_per_row = (long)bits_per_pixel * ss->Columns;
  uchar *prev_row = 0;

#if arch_sizeof_long > arch_sizeof_int
  if (bits_per_row > max_uint * 7L)
		return ERRC;
#endif
    
  ss->row_count = (uint) ((bits_per_row + 7) >> 3);
  ss->end_mask = (1 << (-bits_per_row & 7)) - 1;
  ss->bpp = (bits_per_pixel + 7) >> 3;
  if (need_prev) 
  {
		prev_row = (uchar*)malloc((ss->bpp + ss->row_count+1) * sizeof(uchar));
		if (prev_row == 0)
	    return ERRC;
		memset(prev_row, 0, ss->bpp);
  }
  ss->prev_row = prev_row;
  return s_PNGP_reinit(st);
}

static int paeth_predictor(int a, int b, int c)
{
	int ac = b - c, bc = a - c, abcc = ac + bc;
  int pa = (ac < 0 ? -ac : ac), pb = (bc < 0 ? -bc : bc),	pc = (abcc < 0 ? -abcc : abcc);

  return (pa <= pb && pa <= pc ? a : pb <= pc ? b : c);
}

static void s_pngp_process(PSStreamState * st, 
                           PSStreamCursorWrite * pw,
	                         const uchar * dprev, 
	                         PSStreamCursorRead * pr,
	       										const uchar * upprev, 
	       										const uchar * up, 
	       										uint count)
{
	PSStreamPNGPState *const ss = (PSStreamPNGPState *) st;
	uchar *q = pw->ptr + 1;
  uchar *p = pr->ptr + 1;

  pr->ptr += count;
  pw->ptr += count;
  ss->row_left -= count;
  switch (ss->case_index) 
  {
		case cEncode + cNone:
		case cDecode + cNone:
	    memcpy(q, p, count);
	    break;
		case cEncode + cSub:
	    for (; count; ++q, ++dprev, ++p, --count)
				*q = (uchar) (*p - *dprev);
	    break;
		case cDecode + cSub:
	    for (; count; ++q, ++dprev, ++p, --count)
				*q = (uchar) (*p + *dprev);
	    break;
		case cEncode + cUp:
	    for (; count; ++q, ++up, ++p, --count)
				*q = (uchar) (*p - *up);
	    break;
		case cDecode + cUp:
	    for (; count; ++q, ++up, ++p, --count)
				*q = (uchar) (*p + *up);
	    break;
		case cEncode + cAverage:
	    for (; count; ++q, ++dprev, ++up, ++p, --count)
				*q = (uchar) (*p - arith_rshift_1((int)*dprev + (int)*up));
	    break;
		case cDecode + cAverage:
	    for (; count; ++q, ++dprev, ++up, ++p, --count)
				*q = (uchar) (*p + arith_rshift_1((int)*dprev + (int)*up));
	    break;
		case cEncode + cPaeth:
	    for (; count; ++q, ++dprev, ++up, ++upprev, ++p, --count)
				*q = (uchar) (*p - paeth_predictor(*dprev, *up, *upprev));
	    break;
		case cDecode + cPaeth:
	    for (; count; ++q, ++dprev, ++up, ++upprev, ++p, --count)
				*q = (uchar) (*p + paeth_predictor(*dprev, *up, *upprev));
	    break;
  }
}

static uint s_pngp_count(const PSStreamState * st_const, 
                        const PSStreamCursorRead * pr,
	                      const PSStreamCursorWrite * pw)
{
	const PSStreamPNGPState *const ss_const =	(const PSStreamPNGPState *)st_const;
  uint rcount = pr->limit - pr->ptr;
  uint wcount = pw->limit - pw->ptr;
  uint row_left = ss_const->row_left;

  if (rcount < row_left)
		row_left = rcount;
  if (wcount < row_left)
		row_left = wcount;
  return row_left;
}

static int optimum_predictor(const PSStreamState * , const PSStreamCursorRead * )
{
	return cSub;
}

int s_PNGPD_init(PSStreamState * st)
{
	return s_pngp_init(st, true);
}

int s_PNGPE_init(PSStreamState * st)
{
	PSStreamPNGPState *const ss = (PSStreamPNGPState *) st;
	return s_pngp_init(st, pngp_case_needs_prev[ss->Predictor - cNone]);
}

int s_PNGPD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool)
{
	PSStreamPNGPState *const ss = (PSStreamPNGPState *) st;
  int bpp = ss->bpp;
  int status = 0;

  while (pr->ptr < pr->limit)
  {
  	uint count;
		if (ss->row_left == 0)
		{
			int predictor = pr->ptr[1];

	    if (predictor >= cOptimum - cNone) 
	    {
				status = ERRC;
				break;
	    }
	    pr->ptr++;
	    ss->case_index = predictor + cNone + cDecode;
	    ss->row_left = ss->row_count;
	    memset(ss->prev, 0, bpp);
	    continue;
		}
		count = s_pngp_count(st, pr, pw);
		if (count == 0) 
		{
	    status = 1;
	    break;
		}
		
		{
			uchar *up = ss->prev_row + bpp + ss->row_count - ss->row_left;
	    uint n = qMin(count, (uint)bpp);
	    s_pngp_process(st, pw, ss->prev, pr, up - bpp, up, n);
	    if (ss->prev_row)
				memcpy(up - bpp, ss->prev, n);
	    if (ss->row_left == 0)
				continue;
	    if (n < bpp) 
	    {
				int prev_left = bpp - n;

				memmove(ss->prev, ss->prev + n, prev_left);
				memcpy(ss->prev + prev_left, pw->ptr - (n - 1), n);
				if (pw->ptr >= pw->limit && pr->ptr < pr->limit)
		    	status = 1;
				break;
	    }
	    
	    count -= bpp;
	    s_pngp_process(st, pw, pw->ptr - (bpp - 1), pr, up, up + bpp, count);
	    memcpy(ss->prev, pw->ptr - (bpp - 1), bpp);
	    if (ss->prev_row) 
	    {
				memcpy(up, pw->ptr - (bpp + count - 1), count);
				if (ss->row_left == 0)
		    	memcpy(up + count, ss->prev, bpp);
	    }
		}
  }
  return status;
}

int s_PNGPE_process(PSStreamState * st, PSStreamCursorRead * pr,	PSStreamCursorWrite * pw, bool)
{
	PSStreamPNGPState *const ss = (PSStreamPNGPState *) st;
  int bpp = ss->bpp;
  int status = 0;

  while (pr->ptr < pr->limit)
  {
  	uint count;
  	if (ss->row_left == 0)
  	{
  		int predictor;

	    if (pw->ptr >= pw->limit) 
	    {
				status = 1;
				break;
	    }
	    predictor =	(ss->Predictor == cOptimum ? optimum_predictor(st, pr) : ss->Predictor);
	    *++(pw->ptr) = (uchar) predictor - cNone;
	    ss->case_index = predictor + cEncode;
	    ss->row_left = ss->row_count;
	    memset(ss->prev, 0, bpp);
	    continue;
  	}
  	
  	count = s_pngp_count(st, pr, pw);
		if (count == 0) 
		{
	    status = 1;
	    break;
		}
		
		{
			uchar *up = ss->prev_row + bpp + ss->row_count - ss->row_left;
	    uint n = qMin(count, (uint)bpp);
	    s_pngp_process(st, pw, ss->prev, pr, up - bpp, up, n);
	    if (ss->prev_row)
				memcpy(up - bpp, ss->prev, n);
	    if (ss->row_left == 0)
				continue;
	    if (n < bpp) 
	    {
				int prev_left = bpp - n;

				memmove(ss->prev, ss->prev + n, prev_left);
				memcpy(ss->prev + prev_left, pr->ptr - (n - 1), n);
				if (pw->ptr >= pw->limit && pr->ptr < pr->limit)
		    	status = 1;
				break;
	    }
	    
	    count -= bpp;
	    s_pngp_process(st, pw, pr->ptr - (bpp - 1), pr, up, up + bpp, count);
	    memcpy(ss->prev, pr->ptr - (bpp - 1), bpp);
	    if (ss->prev_row) 
	    {
				memcpy(up, pr->ptr - (bpp + count - 1), count);
				if (ss->row_left == 0)
		    	memcpy(up + count, ss->prev, bpp);
	    }
		}
  }
  return status;
}

void s_PNGP_set_defaults(PSStreamState * st)
{
	PSStreamPNGPState *const ss = (PSStreamPNGPState *) st;
	ss->Colors = 1; 
	ss->BitsPerComponent = 8; 
	ss->Columns = 1;
	ss->Predictor = 15;
	ss->prev_row = 0;
}

void s_PNGP_release(PSStreamState *st)
{
	PSStreamPNGPState *const ss = (PSStreamPNGPState *) st;
	if (ss->prev_row)
	{
		free(ss->prev_row);
		ss->prev_row = 0;
	}
}

int s_PNGP_reinit(PSStreamState * st)
{
	PSStreamPNGPState *const ss = (PSStreamPNGPState *) st;
	if (ss->prev_row != 0)
		memset(ss->prev_row + ss->bpp, 0, ss->row_count);
  ss->row_left = 0;
  return 0;
}

PSStreamState *s_create_PNGP_state()
{
	return (PSStreamState *)(new PSStreamPNGPState);
}

int  s_RLE_init(PSStreamState * st)
{
	PSStreamRLEState *const ss = (PSStreamRLEState *) st;
	ss->record_left =(ss->record_size == 0 ? (ss->record_size = max_uint) : ss->record_size);
  ss->copy_left = 0; return 0;
}

int  s_RLE_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last)
{
	PSStreamRLEState *const ss = (PSStreamRLEState *) st;
  uchar *p = pr->ptr;
  uchar *q = pw->ptr;
  uchar *rlimit = pr->limit;
  uchar *wlimit = pw->limit;
  int status = 0;
  ulong rleft = ss->record_left;
  
copy:
  if (ss->copy_left) 
  {
		uint rcount = rlimit - p;
		uint wcount = wlimit - q;
		uint count = ss->copy_left;

		if (rcount < count)
	    count = rcount;
		if (wcount < count)
	    count = wcount;
		if (rleft < count)
	    count = rleft;
		memcpy(q + 1, p + 1, count);
		pr->ptr = p += count;
		pw->ptr = q += count;
		if ((ss->record_left = rleft -= count) == 0)
	    ss->record_left = rleft = ss->record_size;
		if ((ss->copy_left -= count) != 0)
	    return (rcount == 0 ? 0 : 1);
  }
  while (p < rlimit)
  {
  	uchar *beg = p;
		uchar *p1;
		uint count = rlimit - p;
		bool end = last;
		uchar next;

		if (count > rleft)
	    count = rleft, end = true;
		if (count > 128)
	    count = 128, end = true;
		p1 = p + count - 1;
		if (count < 3) 
		{
	    if (!end || count == 0)
				break;
	    if (count == 1) 
	    {
				if (wlimit - q < 2) 
				{
		    	status = 1;
		    	break;
				}
				*++q = 0;
	    } 
	    else 
	    {
				if (p[1] == p[2]) 
				{
		    	if (wlimit - q < 2) 
		    	{
						status = 1;
						break;
		    	}
		    	*++q = 255;
				} 
				else 
				{
		    	if (wlimit - q < 3) 
		    	{
						status = 1;
						break;
		    	}
		    	*++q = 1;
		    	*++q = p[1];
				}
	    }
	    *++q = p1[1];
	    p = p1 + 1;
		} 
		else if ((next = p[1]) == p[2] && next == p[3]) 
		{
	    if (wlimit - q < 2) 
	    {
				status = 1;
				break;
	    }
	    
	    do 
	    {
				p++;
	    }  while (p < p1 && p[2] == next);
	    if (p == p1 && !end) 
	    {
				p = beg;
				break;
	    }
	    p++;
	    *++q = (uchar) (257 - (p - beg));
	    *++q = next;
		} 
		else 
		{
	    p1--;
	    while (p < p1 && (p[2] != p[1] || p[3] != p[1]))
				p++;
	    if (p == p1) 
	    {
				if (!end) 
				{
		    	p = beg;
		    	break;
				}
				p += 2;
	    }
	    count = p - beg;
	    if (wlimit - q < count + 1) 
	    {
				p = beg;
				if (q >= wlimit) 
				{
		    	status = 1;
		    	break;
				}
				*++q = count - 1;
				ss->copy_left = count;
				goto copy;
	    }
	    *++q = count - 1;
	    memcpy(q + 1, beg + 1, count);
	    q += count;
		}
		rleft -= p - beg;
		if (rleft == 0)
	    rleft = ss->record_size;
  }
  
  if (last && status == 0 && ss->EndOfData) 
  {
		if (q < wlimit)
	    *++q = 128;
		else
	    status = 1;
  }
  pr->ptr = p;
  pw->ptr = q;
  ss->record_left = rleft;
  return status;
}

void s_RLE_set_defaults(PSStreamState * st)
{
	PSStreamRLEState *const ss = (PSStreamRLEState *) st;
	ss->EndOfData = true; 
	ss->record_size = 0;
}

PSStreamState *s_create_RLE_state()
{
	return (PSStreamState *)(new PSStreamRLEState);
}

int  s_RLD_init(PSStreamState * st)
{
	PSStreamRLDState *const ss = (PSStreamRLDState *) st;
	ss->min_left = (ss->EndOfData ? 1 : 0); 
	ss->copy_left = 0; return 0;
}

int  s_RLD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool)
{
	PSStreamRLDState *const ss = (PSStreamRLDState *) st;
  uchar *p = pr->ptr;
  uchar *q = pw->ptr;
  uchar *rlimit = pr->limit;
  uchar *wlimit = pw->limit;
  int left;
  int status = 0;

top:
  if ((left = ss->copy_left) > 0)
  {
  	uint avail = wlimit - q;
		int copy_status = 1;

		if (left > avail)
	    left = avail;
		if (ss->copy_data >= 0)
	    memset(q + 1, ss->copy_data, left);
		else 
		{
	    avail = rlimit - p;
	    if (left >= avail) 
	    {
				copy_status = 0;
				left = avail;
	    }
	    memcpy(q + 1, p + 1, left);
	    p += left;
		}
		q += left;
		if ((ss->copy_left -= left) > 0) 
		{
	    status = copy_status;
	    goto x;
		}
  }
  
  while (p < rlimit) 
  {
		int b = *++p;

		if (b < 128) 
		{
	    if (++b > rlimit - p || b > wlimit - q) 
	    {
				ss->copy_left = b;
				ss->copy_data = -1;
				goto top;
	    }
	    memcpy(q + 1, p + 1, b);
	    p += b;
	    q += b;
		} 
		else if (b == 128) 
		{
	    if (ss->EndOfData) 
	    {
				status = EOFC;
				break;
	    }
		} 
		else if (p == rlimit) 
		{
	    p--;
	    break;
		} 
		else if ((b = 257 - b) > wlimit - q) 
		{
	    ss->copy_left = b;
	    ss->copy_data = *++p;
	    goto top;
		} 
		else 
		{
	    memset(q + 1, *++p, b);
	    q += b;
		}
  }
  
x:  
	pr->ptr = p;
  pw->ptr = q;
  return status;
}

void s_RLD_set_defaults(PSStreamState * st)
{
	PSStreamRLDState *const ss = (PSStreamRLDState *) st;
	ss->EndOfData = true;
}

PSStreamState *s_create_RLD_state()
{
	return (PSStreamState *)(new PSStreamRLDState);
}

int s_1248_init(PSStream1248State *ss, int Columns, int samples_per_pixel)
{
	ss->samples_per_row = Columns * samples_per_pixel;
  return ss->templat->init((PSStreamState *)ss);
}

int s_1_init(PSStreamState * st)
{
	PSStream1248State *const ss = (PSStream1248State *) st;

  ss->left = ss->samples_per_row;
  ss->bits_per_sample = 1;
  return 0;
}

int s_2_init(PSStreamState * st)
{
	PSStream1248State *const ss = (PSStream1248State *) st;
	ss->left = ss->samples_per_row;
  ss->bits_per_sample = 2;
  return 0;
}

int s_4_init(PSStreamState * st)
{
	PSStream1248State *const ss = (PSStream1248State *) st;
	ss->left = ss->samples_per_row;
  ss->bits_per_sample = 4;
  return 0;
}

int s_12_init(PSStreamState * st)
{
	PSStream1248State *const ss = (PSStream1248State *) st;
	ss->left = ss->samples_per_row;
  ss->bits_per_sample = 12;
  return 0;
}

#define BEGIN_1248\
	PSStream1248State * const ss = (PSStream1248State *)st;\
	uchar *p = pr->ptr;\
	uchar *rlimit = pr->limit;\
	uchar *q = pw->ptr;\
	uchar *wlimit = pw->limit;\
	uint left = ss->left;\
	int status;\
	int n
#define END_1248\
	pr->ptr = p;\
	pw->ptr = q;\
	ss->left = left;\
	return status
	
#define FOREACH_N_8(in, nout)\
	status = 0;\
	for ( ; p < rlimit; left -= n, q += n, ++p ) {\
	  uchar in = p[1];\
	  n = qMin(left, (uint)nout);\
	  if ( wlimit - q < n ) {\
	    status = 1;\
	    break;\
	  }\
	  switch ( n ) {\
	    case 0: left = ss->samples_per_row; --p; continue;
#define END_FOREACH_N_8\
	  }\
	}
	

int s_N_8_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool)
{
	BEGIN_1248;

  switch (ss->bits_per_sample) 
  {
		case 1:
			{
				FOREACH_N_8(in, 8)
				case 8:
					q[8] = (uchar) - (in & 1);
				case 7:
					q[7] = (uchar) - ((in >> 1) & 1);
				case 6:
					q[6] = (uchar) - ((in >> 2) & 1);
				case 5:
					q[5] = (uchar) - ((in >> 3) & 1);
				case 4:
					q[4] = (uchar) - ((in >> 4) & 1);
				case 3:
					q[3] = (uchar) - ((in >> 5) & 1);
				case 2:
					q[2] = (uchar) - ((in >> 6) & 1);
				case 1:
					q[1] = (uchar) - (in >> 7);
					END_FOREACH_N_8;
	  	}
	   	break;

		case 2:
			{
				static const uchar b2[4] ={0x00, 0x55, 0xaa, 0xff};

				FOREACH_N_8(in, 4)
				case 4:
					q[4] = b2[in & 3];
				case 3:
					q[3] = b2[(in >> 2) & 3];
				case 2:
					q[2] = b2[(in >> 4) & 3];
				case 1:
					q[1] = b2[in >> 6];
				END_FOREACH_N_8;
	    }
	    break;

		case 4:
			{
				static const uchar b4[16] =	{  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
		    		0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

				FOREACH_N_8(in, 2)
				case 2:
					q[2] = b4[in & 0xf];
				case 1:
					q[1] = b4[in >> 4];
				END_FOREACH_N_8;
	    }
	    break;

		default:
	    return ERRC;
  }

  END_1248;
}

int s_12_8_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool)
{
	BEGIN_1248;

  n = ss->samples_per_row;
  status = 0;
  for (; rlimit - p >= 2; ++q) 
  {
		if (q >= wlimit) 
		{
	    status = 1;
	    break;
		}
		if (left == 0)
	    left = n;
		if ((n - left) & 1) 
		{
	    q[1] = (uchar)((p[1] << 4) | (p[2] >> 4));
	    p += 2, --left;
		} 
		else 
		{
	    q[1] = *++p;
	    if (!--left)
				++p;
		}
  }

  END_1248;
}

#define FOREACH_8_N(out, nin)\
	uchar out;\
	status = 1;\
	for ( ; q < wlimit; left -= n, p += n, ++q ) {\
	  n = qMin(left, (uint)nin);\
	  if ( rlimit - p < n ) {\
	    status = 0;\
	    break;\
	  }\
	  out = 0;\
	  switch ( n ) {\
	    case 0: left = ss->samples_per_row; --q; continue;
#define END_FOREACH_8_N\
	    q[1] = out;\
	  }\
	}
	
int s_8_N_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool)
{
	BEGIN_1248;

  switch (ss->bits_per_sample) 
  {
		case 1:
			{
				FOREACH_8_N(out, 8)
				case 8:
					out = p[8] >> 7;
				case 7:
					out |= (p[7] >> 7) << 1;
				case 6:
					out |= (p[6] >> 7) << 2;
				case 5:
					out |= (p[5] >> 7) << 3;
				case 4:
					out |= (p[4] >> 7) << 4;
				case 3:
					out |= (p[3] >> 7) << 5;
				case 2:
					out |= (p[2] >> 7) << 6;
				case 1:
					out |= p[1] & 0x80;
				END_FOREACH_8_N;
	    }
	    break;

		case 2:
			{
				FOREACH_8_N(out, 4)
				case 4:
					out |= p[4] >> 6;
				case 3:
					out |= (p[3] >> 6) << 2;
				case 2:
					out |= (p[2] >> 6) << 4;
				case 1:
					out |= p[1] & 0xc0;
				END_FOREACH_8_N;
	    }
	    break;

		case 4:
			{
				FOREACH_8_N(out, 2)
				case 2:
					out |= p[2] >> 4;
				case 1:
					out |= p[1] & 0xf0;
				END_FOREACH_8_N;
	    }
	    break;

		default:
	    return ERRC;
  }

  END_1248;
}

PSStreamState *s_create_1248_state()
{
	return (PSStreamState *)(new PSStream1248State);
}

int s_C2R_init(PSStreamC2RState *ss, XWPSImagerState *pis)
{
	ss->pis = pis;
  return 0;
}

int  s_C2R_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool)
{
	PSStreamC2RState * ss = (PSStreamC2RState *) st;
	uchar *p = pr->ptr;
  uchar *rlimit = pr->limit;
  uchar *q = pw->ptr;
  uchar *wlimit = pw->limit;

  for (; rlimit - p >= 4 && wlimit - q >= 3; p += 4, q += 3) 
  {
		uchar bc = p[1], bm = p[2], by = p[3], bk = p[4];
		short rgb[3];

		ss->pis->colorCMYKToRgb(byte2frac(bc), byte2frac(bm), byte2frac(by), byte2frac(bk), rgb);
		q[1] = frac2byte(rgb[0]);
		q[2] = frac2byte(rgb[1]);
		q[3] = frac2byte(rgb[2]);
  }
  pr->ptr = p;
  pw->ptr = q;
  return (rlimit - p < 4 ? 0 : 1);
}

void s_C2R_set_defaults(PSStreamState * st)
{
	PSStreamC2RState *const ss = (PSStreamC2RState *) st;

  ss->pis = 0;
}

PSStreamState *s_create_C2R_state()
{
	return (PSStreamState *)(new PSStreamC2RState);
}

int  s_IE_init(PSStreamState * st)
{
	PSStreamIEState *const ss = (PSStreamIEState *) st;
  int key_index = (1 << ss->BitsPerIndex) * ss->NumComponents;
  int i;

  if (ss->Table.data == 0 || ss->Table.size < key_index)
		return ERRC;
		
  memset(ss->Table.data, 0, ss->NumComponents);
  ss->Table.data[ss->Table.size - 1] = 0;
  for (i = 0; i < 400; ++i)
		ss->hash_table[i] = key_index;
  ss->next_index = 0;
  ss->in_bits_left = 0;
  ss->next_component = 0;
  ss->byte_out = 1;
  ss->x = 0;
  return 0;
}

int  s_IE_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool)
{
	PSStreamIEState *const ss = (PSStreamIEState *) st;
  const int bpc = ss->BitsPerComponent;
  const int num_components = ss->NumComponents;
  const int end_index = (1 << ss->BitsPerIndex) * num_components;
  uchar *const table = ss->Table.data;
  uchar *const key = table + end_index;
  
  uint byte_in = ss->byte_in;
  int in_bits_left = ss->in_bits_left;
  int next_component = ss->next_component;
  uint byte_out = ss->byte_out;
  
  uchar *p = pr->ptr;
  uchar *rlimit = pr->limit;
  uchar *q = pw->ptr;
  uchar *wlimit = pw->limit;
  int status = 0;
  
  for (;;)
  {
  	uint hash, reprobe;
		int i, index;
		
		if (byte_out >= 0x100) 
		{
	    if (q >= wlimit) 
	    {
				status = 1;
				break;
	    }
	    *++q = (uchar)byte_out;
	    byte_out = 1;
		}
		
		while (next_component < num_components)
		{
			const float *decode = &ss->Decode[next_component * 2];
	    int sample;

	    if (in_bits_left == 0) 
	    {
				if (p >= rlimit)
		    	goto out;
				byte_in = *++p;
				in_bits_left = 8;
	    }
	    
	    in_bits_left -= bpc;
	    sample = (byte_in >> in_bits_left) & ((1 << bpc) - 1);
	    
	    sample = (int)((decode[0] + (sample / (float)((1 << bpc) - 1) * (decode[1] - decode[0]))) * 255 + 0.5);
	    key[next_component++] =	(sample < 0 ? 0 : sample > 255 ? 255 : (uchar)sample);
		}
		
		for (hash = 0, i = 0; i < num_components; ++i)
	    hash = hash + 23 * key[i];
	  reprobe = (hash / (sizeof(ss->hash_table) / sizeof(ss->hash_table[0]))) | 137;
	  for (hash %= (sizeof(ss->hash_table) / sizeof(ss->hash_table[0]));   memcmp(table + ss->hash_table[hash], key, num_components);
	     		hash = (hash + reprobe) % 400 );
		index = ss->hash_table[hash];
		if (index == end_index)
		{
			if (ss->next_index == end_index) 
			{
				status = ERRC;
				break;
	    }
	    
	    ss->hash_table[hash] = index = ss->next_index;
	    ss->next_index += num_components;
	    memcpy(table + index, key, num_components);
		}
		
		byte_out = (byte_out << ss->BitsPerIndex) + index / num_components;
		next_component = 0;
		if (++(ss->x) == ss->Width)
		{
			in_bits_left = 0;
	    if (byte_out != 1)
				while (byte_out < 0x100)
		    	byte_out <<= 1;
	    ss->x = 0;
		}
  }
  
out:
  pr->ptr = p;
  pw->ptr = q;
  ss->byte_in = byte_in;
  ss->in_bits_left = in_bits_left;
  ss->next_component = next_component;
  ss->byte_out = byte_out;
  ss->Table.data[ss->Table.size - 1] =
	(ss->next_index == 0 ? 0 :
	ss->next_index / ss->NumComponents - 1);
  return status;
}

void s_IE_set_defaults(PSStreamState * st)
{
	PSStreamIEState *const ss = (PSStreamIEState *) st;

  ss->Decode = 0;	
  ss->Table.data = 0;
  ss->Table.size = 0;
  ss->Table.bytes = 0;
}

PSStreamState *s_create_IE_state()
{
	return (PSStreamState *)(new PSStreamIEState);
}

int s_Downsample_size_out(int size_in, int factor, bool pad)
{
	return ((pad ? size_in + factor - 1 : size_in) / factor);
}

void s_Downsample_set_defaults(PSStreamState * st)
{
	PSStreamDownsampleState *const ss = (PSStreamDownsampleState *)st;

  ss->AntiAlias = false;
  ss->padX = false;
  ss->padY = false;
}


int s_Subsample_init(PSStreamState * st)
{
	PSStreamSubsampleState *const ss = (PSStreamSubsampleState *) st;

  ss->x = ss->y = 0;
  return 0;
}

int s_Subsample_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool)
{
	PSStreamSubsampleState *const ss = (PSStreamSubsampleState *) st;
  uchar *p = pr->ptr;
  uchar *rlimit = pr->limit;
  uchar *q = pw->ptr;
  uchar *wlimit = pw->limit;
  int spp = ss->Colors;
  int width = ss->WidthIn, height = ss->HeightIn;
  int xf = ss->XFactor, yf = ss->YFactor;
  int xf2 = xf / 2, yf2 = yf / 2;
  int xlimit = (width / xf) * xf, ylimit = (height / yf) * yf;
  int xlast =	(ss->padX && xlimit < width ? xlimit + (width % xf) / 2 : -1);
  int ylast =	(ss->padY && ylimit < height ? ylimit + (height % yf) / 2 : -1);
  int x = ss->x, y = ss->y;
  int status = 0;

  for (; rlimit - p >= spp; p += spp) 
  {
		if (((y % yf == yf2 && y < ylimit) || y == ylast) &&
	    	((x % xf == xf2 && x < xlimit) || x == xlast)) 
	  {
	    if (wlimit - q < spp) 
	    {
				status = 1;
				break;
	    }
	    memcpy(q + 1, p + 1, spp);
	    q += spp;
		}
		if (++x == width)
	    x = 0, ++y;
  }
  
  pr->ptr = p;
  pw->ptr = q;
  ss->x = x, ss->y = y;
  return status;
}

PSStreamState *s_create_Subsample_state()
{
	return (PSStreamState *)(new PSStreamSubsampleState);
}

int  s_Average_init(PSStreamState * st)
{
	PSStreamAverageState *const ss = (PSStreamAverageState *) st;

  ss->sum_size =ss->Colors * ((ss->WidthIn + ss->XFactor - 1) / ss->XFactor);
  ss->copy_size = ss->sum_size -	(ss->padX || (ss->WidthIn % ss->XFactor == 0) ? 0 : ss->Colors);
  ss->sums = (uint*)malloc((ss->sum_size+1) * sizeof(uint));
  if (ss->sums == 0)
		return ERRC;
  memset(ss->sums, 0, ss->sum_size * sizeof(uint));
  return s_Subsample_init(st);
}

int  s_Average_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last)
{
	PSStreamAverageState *const ss = (PSStreamAverageState *) st;
  uchar *p = pr->ptr;
  uchar *rlimit = pr->limit;
  uchar *q = pw->ptr;
  uchar *wlimit = pw->limit;
  int spp = ss->Colors;
  int width = ss->WidthIn;
  int xf = ss->XFactor, yf = ss->YFactor;
  int x = ss->x, y = ss->y;
  uint *sums = ss->sums;
  int status = 0;
  
top:
  if (y == yf || (last && p >= rlimit && ss->padY && y != 0)) 
  {
		int ncopy = qMin(ss->copy_size - (uint)x, (uint)(wlimit - q));

		if (ncopy) 
		{
	    int scale = xf * y;

	    while (--ncopy >= 0)
				*++q = (uchar) (sums[x++] / scale);
		}
		if (x < ss->copy_size) 
		{
	    status = 1;
	    goto out;
		}
		x = y = 0;
		memset(sums, 0, ss->sum_size * sizeof(uint));
  }
  while (rlimit - p >= spp) 
  {
		uint *bp = sums + x / xf * spp;
		int i;

		for (i = spp; --i >= 0;)
	    *bp++ += *++p;
		if (++x == width) 
		{
	    x = 0;
	    ++y;
	    goto top;
		}
  }
  
out:
  pr->ptr = p;
  pw->ptr = q;
  ss->x = x, ss->y = y;
  return status;
}

void s_Average_set_defaults(PSStreamState * st)
{
	PSStreamAverageState *const ss = (PSStreamAverageState *) st;

  s_Downsample_set_defaults(st);
  ss->sums = 0;
}

void s_Average_release(PSStreamState * st)
{
	PSStreamAverageState *const ss = (PSStreamAverageState *) st;

  if (ss->sums)
  {
  	free(ss->sums);
  	ss->sums = 0;
  }
}

PSStreamState * s_create_Average_state()
{
	return (PSStreamState * )(new PSStreamAverageState);
}

void s_proc_set_defaults(PSStreamState * st)
{
	PSStreamProcState *const ss = (PSStreamProcState *) st;

  ss->proc.makeNull();
  ss->data.makeNull();
}

int s_proc_read_process(PSStreamState * st, PSStreamCursorRead * ,  PSStreamCursorWrite * pw, bool)
{
	PSStreamProcState *const ss = (PSStreamProcState *) st;
  uint count = ss->data.size() - ss->index;

  if (count > 0) 
  {
     uint wcount = pw->limit - pw->ptr;

     if (wcount < count)
         count = wcount;
     memcpy(pw->ptr + 1, ss->data.getBytes() + ss->index, count);
     pw->ptr += count;
     ss->index += count;
     return 1;
  }
  return (ss->eof ? EOFC : CALLC);
}

int s_proc_write_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * , bool last)
{
	PSStreamProcState *const ss = (PSStreamProcState *) st;
  uint rcount = pr->limit - pr->ptr;

  if (rcount > 0) 
  {	
     uint wcount = ss->data.size() - ss->index;
     uint count = qMin(rcount, wcount);

     memcpy(ss->data.getBytes() + ss->index, pr->ptr + 1, count);
     pr->ptr += count;
     ss->index += count;
     if (rcount > wcount)
         return CALLC;
     else if (last) 
     {
        ss->eof = true;
         return CALLC;
     } 
     else
        return 0;
  }
  return ((ss->eof = last) ? EOFC : 0);
}

PSStreamState * s_create_proc_state()
{
	return (PSStreamState *)(new PSStreamProcState);
}

int s_PFBD_init(PSStreamState * st)
{
	PSStreamPFBDState *const ss = (PSStreamPFBDState *) st;
  ss->record_type = -1;
  return 0;
}

int s_PFBD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool)
{
	PSStreamPFBDState *const ss = (PSStreamPFBDState *) st;
	uchar *p = pr->ptr;
  uchar *q = pw->ptr;
  int rcount, wcount;
  int c;
  int status = 0;

top:
  rcount = pr->limit - p;
  wcount = pw->limit - q;
  switch (ss->record_type) 
  {
		case -1:	
	    if (rcount < 2)
				goto out;
	    if (p[1] != 0x80)
				goto err;
	    c = p[2];
	    switch (c) 
	    {
				case 1:
				case 2:
		    	break;
		    	
				case 3:
		    	status = EOFC;
		    	p += 2;
		    	goto out;
		    	
				default:
		    	p += 2;
		    	goto err;
		    	
	    }
	    if (rcount < 6)
				goto out;
	    ss->record_type = c;
	    ss->record_left = p[3] + ((uint) p[4] << 8) +	((ulong) p[5] << 16) + ((ulong) p[6] << 24);
	    p += 6;
	    goto top;
	    
		case 1:	
	    {
				int count = (wcount < rcount ? (status = 1, wcount) : rcount);

				if (count > ss->record_left)
		    	count = ss->record_left,	status = 0;
				ss->record_left -= count;
				for (; count != 0; count--) 
				{
		    	c = *++p;
		    	*++q = (c == '\r' ? '\n' : c);
				}
	    }
	    break;
	    
		case 2:	
	    if (ss->binary_to_hex) 
	    {
				int count;
				const char *const hex_digits = "0123456789abcdef";

				wcount >>= 1;	
				count = (wcount < rcount ? (status = 1, wcount) : rcount);
				if (count > ss->record_left)
		    	count = ss->record_left,	status = 0;
				ss->record_left -= count;
				for (; count != 0; count--) 
				{
		    	c = *++p;
		    	q[1] = hex_digits[c >> 4];
		    	q[2] = hex_digits[c & 0xf];
		    	q += 2;
				}
	    } 
	    else 
	    {	
				int count = (wcount < rcount ? (status = 1, wcount) : rcount);

				if (count > ss->record_left)
		    	count = ss->record_left,	status = 0;
				ss->record_left -= count;
				memcpy(q + 1, p + 1, count);
				p += count;
				q += count;
	    }
	    break;
  }
  if (ss->record_left == 0) 
  {
		ss->record_type = -1;
		goto top;
  }
out:
  pr->ptr = p;
  pw->ptr = q;
  return status;
err:
  pr->ptr = p;
  pw->ptr = q;
  return ERRC;
}

PSStreamState *s_create_PFBD_state()
{
	return (PSStreamState *)(new PSStreamPFBDState);
}

int  s_exD_init(PSStreamState * st)
{
	PSStreamexDState *const ss = (PSStreamexDState *) st;

  ss->odd = -1;
  ss->skip = ss->lenIV;
  return 0;
}

void s_exD_set_defaults(PSStreamState * st)
{
	PSStreamexDState *const ss = (PSStreamexDState *) st;

  ss->binary = -1;
  ss->lenIV = 4;
  ss->record_left = max_long;
  ss->hex_left = max_long;
  ss->pfb_state = 0;
}

int  s_exD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool)
{
	PSStreamexDState *const ss = (PSStreamexDState *) st;
  uchar *p = pr->ptr;
  uchar *q = pw->ptr;
  int skip = ss->skip;
  int rcount = pr->limit - p;
  int wcount = pw->limit - q;
  int status = 0;
  int count = (wcount < rcount ? (status = 1, wcount) : rcount);

  if (ss->binary < 0) 
  {
		const uchar *const decoder = scan_char_decoder;
		int i;

		if (rcount < 8)
	    return 0;
	    
		ss->binary = 0;
		for (i = 1; i <= 8; i++)
	    if (!(decoder[p[i]] <= 0xf || decoder[p[i]] == ctype_space)) 
	    {
				ss->binary = 1;
				if (ss->pfb_state != 0) 
				{
		    	ss->record_left = ss->pfb_state->record_left;
				}
				break;
	    }
  }
  if (ss->binary) 
  {
		if (count > ss->record_left) 
		{
	    count = ss->record_left;
	    status = 0;
		}
		
		if ((ss->record_left -= count) == 0)
	    ss->record_left = max_long;
		pr->ptr = p + count;
  } 
  else 
  {
		PSStreamCursorRead r;
		uchar *start;

hp:	
		r = *pr;
		start = r.ptr;
		if (r.limit - r.ptr > ss->hex_left)
	    r.limit = r.ptr + ss->hex_left;
		status = s_hex_process(&r, pw, &ss->odd, hex_ignore_leading_whitespace);
		pr->ptr = r.ptr;
		ss->hex_left -= r.ptr - start;
		if (ss->hex_left == 0)
	    ss->binary = 1;
		count = pw->ptr - q;
		if (status < 0 && ss->odd < 0) 
		{
	    if (count) 
	    {
				--p;
				status = 0;	
	    } 
	    else if (*p == '%')
				goto hp;
		}
		p = q;
  }
  if (skip >= count && skip != 0) 
  {
		ps_type1_decrypt(q + 1, p + 1, count,	 (ushort *) & ss->cstate);
		ss->skip -= count;
		count = 0;
		status = 0;
  } 
  else 
  {
		ps_type1_decrypt(q + 1, p + 1, skip, (ushort *) & ss->cstate);
		count -= skip;
		ps_type1_decrypt(q + 1, p + 1 + skip, count, (ushort *) & ss->cstate);
		ss->skip = 0;
  }
  pw->ptr = q + count;
  return status;
}

PSStreamState * s_create_exD_state()
{
	return (PSStreamState * )(new PSStreamexDState);
}

void s_SFD_set_defaults(PSStreamState * st)
{
	PSStreamSFDState *const ss = (PSStreamSFDState *) st;

  ss->count = 0;
  ss->eod.data = 0;
  ss->eod.size = 0;
  ss->skip_count = 0;
}

int  s_SFD_init(PSStreamState * st)
{
	PSStreamSFDState *const ss = (PSStreamSFDState *) st;

  ss->match = 0;
  ss->copy_count = 0;
  ss->min_left = (ss->eod.size != 0);

  return 0;
}

int  s_SFD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool)
{
	PSStreamSFDState *const ss = (PSStreamSFDState *) st;
	uchar *p = pr->ptr;
  uchar *q = pw->ptr;
  uchar *rlimit = pr->limit;
  uchar *wlimit = pw->limit;
  int status = 0;

  if (ss->eod.size == 0) 
  {
		int rcount = rlimit - p;
		int wcount = wlimit - q;
		int count;

		if (rcount <= ss->skip_count) 
		{
	    ss->skip_count -= rcount;
	    pr->ptr = rlimit;
	    return 0;
		} 
		else if (ss->skip_count > 0) 
		{
	    rcount -= ss->skip_count;
	    pr->ptr = p += ss->skip_count;
	    ss->skip_count = 0;
		}
		count = qMin(rcount, wcount);
		if (ss->count == 0)	
	    return stream_move(pr, pw);
		else if (ss->count > count) 
		{
	    ss->count -= count;
	    return stream_move(pr, pw);
		} 
		else 
		{
	    count = ss->count;
	    if (count > 0) 
	    {
				memcpy(q + 1, p + 1, count);
				pr->ptr = p + count;
				pw->ptr = q + count;
	    }
	    ss->count = -1;
	    return EOFC;
		}
  } 
  else 
  {	
		uchar *pattern = ss->eod.data;
		uint match = ss->match;

cp:
		if (ss->copy_count) 
		{
	    int count = qMin((uint)(wlimit - q), ss->copy_count);

	    memcpy(q + 1, ss->eod.data + ss->copy_ptr, count);
	    ss->copy_count -= count;
	    ss->copy_ptr += count;
	    q += count;
	    if (ss->copy_count != 0) 
	    {
				status = 1;
				goto xit;
	    } 
	    else if (ss->count < 0) 
	    {
				status = EOFC;
				goto xit;
	    }
		}
		while (p < rlimit) 
		{
	    int c = *++p;

	    if (c == pattern[match]) 
	    {
				if (++match == ss->eod.size) 
				{
		    	if (ss->skip_count > 0) 
		    	{
						q = pw->ptr;
						ss->skip_count--;
						match = 0;
						continue;
		    	}
		    	
		    	if (ss->count <= 0) 
		    	{
						status = EOFC;
						goto xit;
		    	} 
		    	else if (ss->count == 1) 
		    	{
						ss->count = -1;
		    	} 
		    	else
						ss->count--;
		    	ss->copy_ptr = 0;
		    	ss->copy_count = match;
		    	match = 0;
		    	goto cp;
				}
				continue;
	    }
	    
	    if (match > 0) 
	    {
				int end = match;

				while (match > 0) 
				{
		    	match--;
		    	if (!memcmp(pattern, pattern + end - match, match))
						break;
				}
				p--;
				ss->copy_ptr = 0;
				ss->copy_count = end - match;
				goto cp;
	    }
	    if (q == wlimit) 
	    {
				p--;
				status = 1;
				break;
	    }
	    *++q = c;
		}
xit:	
		pr->ptr = p;
		if (ss->skip_count <= 0)
	    pw->ptr = q;
		ss->match = match;
  }
  return status;
}

PSStreamState *s_create_SFD_state()
{
	return (PSStreamState *)(new PSStreamSFDState);
}

#undef cEncode
#undef cDecode
#define cBits1 0
#define cBits2 5
#define cBits4 10
#define cBits8 15
#define cEncode 0
#define cDecode 20

void s_PDiff_set_defaults(PSStreamState * st)
{
	PSStreamPDiffState *const ss = (PSStreamPDiffState *) st;
	ss->Colors = 1;
	ss->BitsPerComponent = 8;
	ss->Columns = 1;
}

int  s_PDiff_reinit(PSStreamState * st)
{
	PSStreamPDiffState *const ss = (PSStreamPDiffState *) st;

  ss->row_left = 0;
  return 0;
}

int  s_PDiffD_init(PSStreamState * st)
{
	PSStreamPDiffState *const ss = (PSStreamPDiffState *) st;

  s_PDiffE_init(st);
  ss->case_index += cDecode - cEncode;
  return 0;
}

int  s_PDiffE_init(PSStreamState * st)
{
	PSStreamPDiffState *const ss = (PSStreamPDiffState *) st;
  int bits_per_row = ss->Colors * ss->BitsPerComponent * ss->Columns;
  static const uchar cb_values[] = {
		0, cBits1, cBits2, 0, cBits4, 0, 0, 0, cBits8 };

  ss->row_count = (bits_per_row + 7) >> 3;
  ss->end_mask = (1 << (-bits_per_row & 7)) - 1;
  ss->case_index =	cb_values[ss->BitsPerComponent] +	(ss->Colors > 4 ? 0 : ss->Colors) + cEncode;
  return s_PDiff_reinit(st);
}

int s_PDiff_process(PSStreamState * st, PSStreamCursorRead * pr,PSStreamCursorWrite * pw, bool last)
{
	PSStreamPDiffState *const ss = (PSStreamPDiffState *) st;
  uchar *p = pr->ptr;
  uchar *q = pw->ptr;
  int count;
  int status = 0;
  uchar s0 = ss->prev[0];
  uchar t;
  uchar end_mask = ss->end_mask;
  int colors = ss->Colors;
  int nb = (colors * ss->BitsPerComponent) >> 3;
  int final;
  int ndone, ci;

row:
  if (ss->row_left == 0) 
  {
		ss->row_left = ss->row_count;
		s0 = 0;
		memset(ss->prev + 1, 0, s_PDiff_max_Colors - 1);
  }
  
  {
		int rcount = pr->limit - p;
		int wcount = pw->limit - q;

		if (ss->row_left < rcount)
	    rcount = ss->row_left;
		count = (wcount < rcount ? (status = 1, wcount) : rcount);
  }
  
  final = (last && !status ? 1 : nb);
  ss->row_left -= count;

#define LOOP_BY(n, body)\
  for (; count >= n; count -= n) p += n, q += n, body

  switch (ss->case_index) 
  {
#define ENCODE1_LOOP(ee)\
  LOOP_BY(1, (t = *p, *q = ee, s0 = t)); break

#define ENCODE_ALIGNED_LOOP(ee)\
    ss->prev[0] = s0;\
    for (; count >= final; count -= ndone) {\
	ndone = qMin(count, nb);\
	for (ci = 0; ci < ndone; ++ci)\
	    t = *++p, *++q = ee, ss->prev[ci] = t;\
    }\
    s0 = ss->prev[0];

#define ENCODE_UNALIGNED_LOOP(shift, cshift, de)\
    for (; count >= final; count -= ndone) {\
	ndone = qMin(count, nb);\
	for (ci = 1; ci <= ndone; ++ci) {\
	    ++p;\
	    t = (s0 << (cshift)) | (ss->prev[ci] >> (shift));\
	    *++q = de;\
	    s0 = ss->prev[ci];\
	    ss->prev[ci] = *p;\
	}\
    }

		case cEncode + cBits1 + 0:
		case cEncode + cBits1 + 2:
	    if (colors < 8) 
	    {
				int cshift = 8 - colors;

				ENCODE1_LOOP(t ^ ((s0 << cshift) | (t >> colors)));
	    } 
	    else if (colors & 7) 
	    {
				int shift = colors & 7;
				int cshift = 8 - shift;

				ENCODE_UNALIGNED_LOOP(shift, cshift, *p ^ t);
	    } 
	    else 
	    {
				ENCODE_ALIGNED_LOOP(t ^ ss->prev[ci]);
	    }
	    break;

		case cEncode + cBits1 + 1:
	    ENCODE1_LOOP(t ^ ((s0 << 7) | (t >> 1)));
		case cEncode + cBits1 + 3:
	    ENCODE1_LOOP(t ^ ((s0 << 5) | (t >> 3)));
		case cEncode + cBits1 + 4:
	    ENCODE1_LOOP(t ^ ((s0 << 4) | (t >> 4)));

#define DECODE1_LOOP(te, de)\
  LOOP_BY(1, (t = te, s0 = *q = de)); break

#define DECODE_ALIGNED_LOOP(de)\
    ss->prev[0] = s0;\
    for (; count >= final; count -= ndone) {\
	ndone = qMin(count, nb);\
	for (ci = 0; ci < ndone; ++ci)\
	    t = *++p, ss->prev[ci] = *++q = de;\
    }\
    s0 = ss->prev[0];

#define DECODE_UNALIGNED_LOOP(shift, cshift, de)\
    for (; count >= final; count -= ndone) {\
	ndone = qMin(count, nb);\
	for (ci = 1; ci <= ndone; ++ci) {\
	    ++p, ++q;\
	    t = (s0 << (cshift)) | (ss->prev[ci] >> (shift));\
	    s0 = ss->prev[ci];\
	    ss->prev[ci] = *q = de;\
	}\
    }

		case cDecode + cBits1 + 0:
	    if (colors < 8) 
	    {	
				int cshift = 8 - colors;

				DECODE1_LOOP(*p ^ (s0 << cshift), t ^ (t >> colors));
	    } 
	    else if (colors & 7) 
	    {
				int shift = colors & 7;
				int cshift = 8 - shift;

				DECODE_UNALIGNED_LOOP(shift, cshift, *p ^ t);
	    } 
	    else 
	    {
				DECODE_ALIGNED_LOOP(t ^ ss->prev[ci]);
	    }
	    break;

		case cDecode + cBits1 + 1:
	    DECODE1_LOOP(*p ^ (s0 << 7), (t ^= t >> 1, t ^= t >> 2, t ^ (t >> 4)));
		case cDecode + cBits1 + 2:
	    DECODE1_LOOP(*p ^ (s0 << 6), (t ^= (t >> 2), t ^ (t >> 4)));
		case cDecode + cBits1 + 3:
	    DECODE1_LOOP(*p ^ (s0 << 5), t ^ (t >> 3) ^ (t >> 6));
		case cDecode + cBits1 + 4:
	    DECODE1_LOOP(*p ^ (s0 << 4), t ^ (t >> 4));

#define ADD4X2(a, b) ( (((a) & (b) & 0x55) << 1) ^ (a) ^ (b) )

#define SUB4X2(a, b) ( ((~(a) & (b) & 0x55) << 1) ^ (a) ^ (b) )

		case cEncode + cBits2 + 0:
	    if (colors & 7) 
	    {
				int shift = (colors & 3) << 1;
				int cshift = 8 - shift;

				ENCODE_UNALIGNED_LOOP(shift, cshift, SUB4X2(*p, t));
	    } 
	    else 
	    {
				ENCODE_ALIGNED_LOOP(SUB4X2(t, ss->prev[ci]));
	    }
	    break;

		case cEncode + cBits2 + 1:
	    ENCODE1_LOOP((s0 = (s0 << 6) | (t >> 2), SUB4X2(t, s0)));
		case cEncode + cBits2 + 2:
	    ENCODE1_LOOP((s0 = (s0 << 4) | (t >> 4), SUB4X2(t, s0)));
		case cEncode + cBits2 + 3:
	    ENCODE1_LOOP((s0 = (s0 << 2) | (t >> 6), SUB4X2(t, s0)));
		case cEncode + cBits2 + 4:
	    ENCODE1_LOOP(SUB4X2(t, s0));

		case cDecode + cBits2 + 0:
	    if (colors & 7) 
	    {
				int shift = (colors & 3) << 1;
				int cshift = 8 - shift;

				DECODE_UNALIGNED_LOOP(shift, cshift, ADD4X2(*p, t));
	    } 
	    else 
	    {
				DECODE_ALIGNED_LOOP(ADD4X2(t, ss->prev[ci]));
	    }
	    break;

		case cDecode + cBits2 + 1:
	    DECODE1_LOOP(*p + (s0 << 6),	 (t = ADD4X2(t >> 2, t), ADD4X2(t >> 4, t)));
		case cDecode + cBits2 + 2:
	    DECODE1_LOOP(*p, (t = ADD4X2(t, s0 << 4), ADD4X2(t >> 4, t)));
		case cDecode + cBits2 + 3:
	    DECODE1_LOOP(*p, (t = ADD4X2(t, s0 << 2), ADD4X2(t >> 6, t)));
		case cDecode + cBits2 + 4:
	    DECODE1_LOOP(*p, ADD4X2(t, s0));

#undef ADD4X2
#undef SUB4X2

#define ADD2X4(a, b) ( (((a) + (b)) & 0xf) + ((a) & 0xf0) + ((b) & 0xf0) )
#define ADD2X4R4(a) ( (((a) + ((a) >> 4)) & 0xf) + ((a) & 0xf0) )
#define SUB2X4(a, b) ( (((a) - (b)) & 0xf) + ((a) & 0xf0) - ((b) & 0xf0) )
#define SUB2X4R4(a) ( (((a) - ((a) >> 4)) & 0xf) + ((a) & 0xf0) )

		case cEncode + cBits4 + 0:
		case cEncode + cBits4 + 2:
    
enc4:
	    if (colors & 1) 
	    {
				ENCODE_UNALIGNED_LOOP(4, 4, SUB2X4(*p, t));
	    } 
	    else 
	    {
				ENCODE_ALIGNED_LOOP(SUB2X4(t, ss->prev[ci]));
	    }
	    break;

		case cEncode + cBits4 + 1:
	    ENCODE1_LOOP(((t - (s0 << 4)) & 0xf0) | ((t - (t >> 4)) & 0xf));

		case cEncode + cBits4 + 3: 
			{
	    	uchar s1 = ss->prev[1];

	    	LOOP_BY(1, (t = *p, *q = ((t - (s0 << 4)) & 0xf0) | ((t - (s1 >> 4)) & 0xf), s0 = s1, s1 = t));
	    	ss->prev[1] = s1;
			} 
			break;

		case cEncode + cBits4 + 4: 
			{
	    	uchar s1 = ss->prev[1];

	    	LOOP_BY(2, (t = p[-1], q[-1] = SUB2X4(t, s0), s0 = t, t = *p, *q = SUB2X4(t, s1), s1 = t));
	    	ss->prev[1] = s1;
	    	goto enc4;
			}

		case cDecode + cBits4 + 0:
		case cDecode + cBits4 + 2:
    
dec4:
	    if (colors & 1) 
	    {
				DECODE_UNALIGNED_LOOP(4, 4, ADD2X4(*p, t));
	    } 
	    else 
	    {
				DECODE_ALIGNED_LOOP(ADD2X4(t, ss->prev[ci]));
	    }
	    break;

		case cDecode + cBits4 + 1:
	    DECODE1_LOOP(*p + (s0 << 4), ADD2X4R4(t));

		case cDecode + cBits4 + 3: 
			{
	    	uchar s1 = ss->prev[1];

	    	LOOP_BY(1, (t = (s0 << 4) + (s1 >> 4), s0 = s1, s1 = *q = ADD2X4(*p, t)));
	    	ss->prev[1] = s1;
			} 
			break;

		case cDecode + cBits4 + 4: 
			{
	    	uchar s1 = ss->prev[1];

	    	LOOP_BY(2,  (t = p[-1], s0 = q[-1] = ADD2X4(s0, t),  t = *p, s1 = *q = ADD2X4(s1, t)));
	    	ss->prev[1] = s1;
	    	goto dec4;
			}

#undef ADD2X4
#undef ADD2X4R4
#undef SUB2X4
#undef SUB2X4R4

#define ENCODE8(s, d) (q[d] = p[d] - s, s = p[d])
#define DECODE8(s, d) q[d] = s += p[d]

		case cEncode + cBits8 + 0:
		case cEncode + cBits8 + 2:
	    ss->prev[0] = s0;
	    for (; count >= colors; count -= colors)
				for (ci = 0; ci < colors; ++ci) 
				{
		    	*++q = *++p - ss->prev[ci];
		    	ss->prev[ci] = *p;
				}
	    s0 = ss->prev[0];
    
enc8:  
	    if (last && !status)
				for (ci = 0; ci < count; ++ci)
		    	*++q = *++p - ss->prev[ci],	ss->prev[ci] = *p;
	  	break;

		case cDecode + cBits8 + 0:
		case cDecode + cBits8 + 2:
	    ss->prev[0] = s0;
	    for (; count >= colors; count -= colors)
				for (ci = 0; ci < colors; ++ci)
		    *++q = ss->prev[ci] += *++p;
	    s0 = ss->prev[0];
    
dec8: 
	    if (last && !status)
				for (ci = 0; ci < count; ++ci)
		    *++q = ss->prev[ci] += *++p;
	    break;

		case cEncode + cBits8 + 1:
	    LOOP_BY(1, ENCODE8(s0, 0));
	    break;

		case cDecode + cBits8 + 1:
	    LOOP_BY(1, DECODE8(s0, 0));
	    break;

		case cEncode + cBits8 + 3: 
			{
	    	uchar s1 = ss->prev[1], s2 = ss->prev[2];

	    	LOOP_BY(3, (ENCODE8(s0, -2), ENCODE8(s1, -1), ENCODE8(s2, 0)));
	    	ss->prev[1] = s1, ss->prev[2] = s2;
	    	goto enc8;
			}

		case cDecode + cBits8 + 3: 
			{
	    	uchar s1 = ss->prev[1], s2 = ss->prev[2];

	    	LOOP_BY(3, (DECODE8(s0, -2), DECODE8(s1, -1),	DECODE8(s2, 0)));
	    	ss->prev[1] = s1, ss->prev[2] = s2;
	    	goto dec8;
			} 
			break;

		case cEncode + cBits8 + 4: 
			{
	    	uchar s1 = ss->prev[1], s2 = ss->prev[2], s3 = ss->prev[3];

	    	LOOP_BY(4, (ENCODE8(s0, -3), ENCODE8(s1, -2),	ENCODE8(s2, -1), ENCODE8(s3, 0)));
	    	ss->prev[1] = s1, ss->prev[2] = s2, ss->prev[3] = s3;
	    	goto enc8;
			} 
			break;

		case cDecode + cBits8 + 4: 
			{
	    	uchar s1 = ss->prev[1], s2 = ss->prev[2], s3 = ss->prev[3];

	    	LOOP_BY(4, (DECODE8(s0, -3), DECODE8(s1, -2),		DECODE8(s2, -1), DECODE8(s3, 0)));
	    	ss->prev[1] = s1, ss->prev[2] = s2, ss->prev[3] = s3;
	    	goto dec8;
			} 
			break;

#undef ENCODE8
#undef DECODE8

  }
#undef LOOP_BY
#undef ENCODE1_LOOP
#undef DECODE1_LOOP
  ss->row_left += count;
  if (ss->row_left == 0) 
  {
		if (end_mask != 0)
	    *q = (*q & ~end_mask) | (*p & end_mask);
		if (p < pr->limit && q < pw->limit)
	    goto row;
  }
  ss->prev[0] = s0;
  pr->ptr = p;
  pw->ptr = q;
  return status;
}

PSStreamState* s_create_PDiff_state()
{
	return (PSStreamState* )(new PSStreamPDiffState);
}

static PSStreamBWBSState *bwbs_compare_ss;

static int
bwbs_compare_rotations(const void *p1, const void *p2)
{
  const uchar *buffer = bwbs_compare_ss->buffer;
  const uchar *s1 = buffer + *(const int *)p1;
  const uchar *s2 = buffer + *(const int *)p2;
  const uchar *start1;
  const uchar *end;
  int swap;

  if (*s1 != *s2)
		return (*s1 < *s2 ? -1 : 1);
    
  if (s1 < s2)
		swap = 1;
  else 
  {
		const uchar *t = s1;

		s1 = s2;
		s2 = t;
		swap = -1;
  }
  start1 = s1;
  end = buffer + bwbs_compare_ss->N;
  for (s1++, s2++; s2 < end; s1++, s2++)
		if (*s1 != *s2)
	    return (*s1 < *s2 ? -swap : swap);
  s2 = buffer;
  for (; s1 < end; s1++, s2++)
		if (*s1 != *s2)
	    return (*s1 < *s2 ? -swap : swap);
  s1 = buffer;
  for (; s1 < start1; s1++, s2++)
		if (*s1 != *s2)
	    return (*s1 < *s2 ? -swap : swap);
  return 0;
}

static void
bwbse_sort(const uchar * buffer, uint * indices, int N)
{
  PSOffsetsFull Cs;

#define C Cs.v
  uint sum = 0, j, ch;

  memset(C, 0, sizeof(Cs));
  for (j = 0; j < N; j++)
		C[buffer[j]]++;
  for (ch = 0; ch <= 255; ch++) 
  {
		sum += C[ch];
		C[ch] = sum - C[ch];
  }
  for (j = 0; j < N; j++)
		indices[C[buffer[j]]++] = j;
    
  sum = 0;
  for (ch = 0; ch <= 255; sum = C[ch], ch++)
		qsort(indices + sum, C[ch] - sum,  sizeof(*indices), bwbs_compare_rotations);
#undef C
}


void s_buffered_set_defaults(PSStreamState * st)
{
	PSStreamBufferedState *const ss = (PSStreamBufferedState *) st;
  ss->buffer = 0;
}

int  s_buffered_no_block_init(PSStreamState * st)
{
	PSStreamBufferedState *const ss = (PSStreamBufferedState *) st;

  ss->buffer = 0;
  ss->filling = true;
  ss->bpos = 0;
  return 0;
}

int  s_buffered_block_init(PSStreamState * st)
{
	PSStreamBufferedState *const ss = (PSStreamBufferedState *) st;

  s_buffered_no_block_init(st);
  ss->buffer = new uchar[ss->BlockSize];
  return 0;
}

int  s_buffered_process(PSStreamState * st, PSStreamCursorRead * pr, bool last)
{
	PSStreamBufferedState *const ss = (PSStreamBufferedState *) st;
  uchar *p = pr->ptr;
  uchar *rlimit = pr->limit;
  uint count = rlimit - p;
  uint left = ss->bsize - ss->bpos;

  if (!ss->filling)
		return 1;
    
  if (left < count)
		count = left;
    
  memcpy(ss->buffer + ss->bpos, p + 1, count);
  pr->ptr = p += count;
  ss->bpos += count;
  if (ss->bpos == ss->bsize || (p == rlimit && last)) 
  {
		ss->filling = false;
		return 1;
  }
  return 0;
}

void s_buffered_release(PSStreamState * st)
{
	PSStreamBufferedState *const ss = (PSStreamBufferedState *) st;

  if (ss->buffer)
  {
  	delete [] ss->buffer;
  	ss->buffer = 0;
  }
}

void s_BWBS_set_defaults(PSStreamState * st)
{
	PSStreamBWBSState *const ss = (PSStreamBWBSState *) st;

  s_buffered_set_defaults(st);
  ss->offsets = 0;
}

int  bwbs_init(PSStreamState * st, uint osize)
{
	PSStreamBWBSState *const ss = (PSStreamBWBSState *) st;
  int code;

  ss->bsize = ss->BlockSize;
  code = s_buffered_block_init(st);
  if (code != 0)
		return code;
    
  ss->offsets = 0;
  ss->offsets = malloc(osize+1);
  if (ss->offsets == 0) 
  {
		s_BWBS_release(st);
		return ERRC;
  }
  ss->I = -1;	
  return 0;
}

void s_BWBS_release(PSStreamState * st)
{
	PSStreamBWBSState *const ss = (PSStreamBWBSState *) st;

  if (ss->offsets)
  {
  	free(ss->offsets);
  	ss->offsets = 0;
  }
  s_buffered_release(st);
}

int  s_BWBSE_init(PSStreamState * st)
{
	PSStreamBWBSState *const ss = (PSStreamBWBSState *) st;

  return bwbs_init(st, ss->BlockSize * sizeof(int));
}

int  s_BWBSE_process(PSStreamState * st, PSStreamCursorRead * pr,	PSStreamCursorWrite * pw, bool last)
{
	PSStreamBWBSState *const ss = (PSStreamBWBSState *) st;
  uchar *q = pw->ptr;
  uchar *wlimit = pw->limit;
  uint wcount = wlimit - q;
  uint *indices = (uint*)(ss->offsets);

  if (ss->filling) 
  {
		int status, j, N;
		uchar *buffer = ss->buffer;
		if (wcount < sizeof(int) * 2)
	    return 1;
		status = s_buffered_process(st, pr, last);
		if (!status)
	    return 0;
		ss->N = N = ss->bpos;
		for (j = N / 2 - 1; j >= 0; j--) 
		{
	    uchar *p0 = &buffer[j];
	    uchar *p1 = &buffer[N - 1 - j];
	    uchar b = *p0;

	    *p0 = *p1;
	    *p1 = b;
		}
		bwbs_compare_ss = ss;
		bwbse_sort(buffer, indices, N);
		for (j = 0; j < N; j++)
	    if (indices[j] == 0) 
	    {
				ss->I = j;
				break;
	    }
		for (j = sizeof(int); --j >= 0;)
	    *++q = (uchar) (N >> (j * 8));
		for (j = sizeof(int); --j >= 0;)
	    *++q = (uchar) (ss->I >> (j * 8));
		ss->bpos = 0;
  }
  
  while (q < wlimit && ss->bpos < ss->N) 
 	{
		int i = indices[ss->bpos++];

		*++q = ss->buffer[(i == 0 ? ss->N - 1 : i - 1)];
  }
  if (ss->bpos == ss->N) 
  {
		ss->filling = true;
		ss->bpos = 0;
  }
  pw->ptr = q;
  if (q == wlimit)
		return 1;
  return 0;
}

PSStreamState* s_create_BWBS_state()
{
	return (PSStreamState* )(new PSStreamBWBSState);
}

#define SHORT_OFFSETS

#ifdef SHORT_OFFSETS

typedef struct {
    quint16 v[256];
} PSOffsets4k;

#if arch_sizeof_int > 2
#  define ceil_64k(n) (((n) + 0xffff) >> 16)
#else
#  define ceil_64k(n) 1
#endif
#define ceil_4k(n) (((n) + 0xfff) >> 12)
#define offset_space(bsize)\
  (ceil_64k(bsize) * sizeof(PSOffsetsFull) +\
   ceil_4k(bsize) * sizeof(PSOffsets4k) +\
   ((bsize + 1) >> 1) * 3)

#else /* !SHORT_OFFSETS */

#define offset_space(bsize)\
  (bsize * sizeof(int))

#endif /* (!)SHORT_OFFSETS */

#ifdef SHORT_OFFSETS

static void
bwbsd_construct_offsets(PSStreamBWBSState * sst, PSOffsetsFull * po64k,
			PSOffsets4k * po4k, uchar * po1, int N)
{
  PSOffsetsFull Cs;

#define C Cs.v
  uint i1;
  uchar *b = sst->buffer;
  PSOffsetsFull *p2 = po64k - 1;
  PSOffsets4k *p1 = po4k;
  uchar *p0 = po1;

  memset(C, 0, sizeof(Cs));
  for (i1 = 0; i1 < ceil_4k(N); i1++, p1++) 
  {
		int j;

		if (!(i1 & 15))
	    *++p2 = Cs;
		for (j = 0; j < 256; j++)
	    p1->v[j] = C[j] - p2->v[j];
		j = (N + 1 - (i1 << 12)) >> 1;
		if (j > 4096 / 2)
	    j = 4096 / 2;
		for (; j > 0; j--, b += 2, p0 += 3) 
		{
	    uchar b0 = b[0];
	    uint d0 = C[b0]++ - (p1->v[b0] + p2->v[b0]);
	    uchar b1 = b[1];
	    uint d1 = C[b1]++ - (p1->v[b1] + p2->v[b1]);

	    p0[0] = d0 >> 4;
	    p0[1] = (uchar) ((d0 << 4) + (d1 >> 8));
	    p0[2] = (uchar) d1;
		}
  }
  
  if (N & 1)
		C[sst->buffer[N]]--;
		
  {
		int sum = 0, ch;

		for (ch = 0; ch <= 255; ch++) 
		{
	    sum += C[ch];
	    C[ch] = sum - C[ch];
		}
  }
  
  {
		int i2, ch;

		for (p2 = po64k, i2 = ceil_64k(N); i2 > 0; p2++, i2--)
	    for (ch = 0; ch < 256; ch++)
			p2->v[ch] += C[ch];
  }
#undef C
}

#else /* !SHORT_OFFSETS */

static void
bwbsd_construct_offsets(PSStreamBWBSState * sst, int *po, int N)
{
  PSOffsetsFull Cs;

#define C Cs.v
  uint i;
  uchar *b = sst->buffer;
  int *p = po;

  memset(C, 0, sizeof(Cs));
  for (i = 0; i < N; i++, p++, b++)
		*p = C[*b]++;

  {
		int sum = 0, ch;

		for (ch = 0; ch <= 255; ch++) 
		{
	    sum += C[ch];
	    C[ch] = sum - C[ch];
		}
  }
   
  for (i = 0, b = sst->buffer, p = po; i < N; i++, b++, p++)
		*p += C[*b];
#undef C
}

#endif /* (!)SHORT_OFFSETS */

int s_BWBSD_init(PSStreamState * st)
{
	PSStreamBWBSState *const ss = (PSStreamBWBSState *) st;
  uint bsize = ss->BlockSize;

  return bwbs_init(st, offset_space(bsize));
}

int s_BWBSD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last)
{
	PSStreamBWBSState *const ss = (PSStreamBWBSState *) st;
  uchar *p = pr->ptr;
  uchar *rlimit = pr->limit;
  uint count = rlimit - p;
  uchar *q = pw->ptr;
  uchar *wlimit = pw->limit;

#ifdef SHORT_OFFSETS
  uint BlockSize = ss->BlockSize;
  PSOffsetsFull *po64k = (PSOffsetsFull*)(ss->offsets);
  PSOffsets4k *po4k = (PSOffsets4k *) (po64k + ceil_64k(BlockSize));
  uchar *po1 = (uchar *) (po4k + ceil_4k(BlockSize));

#else /* !SHORT_OFFSETS */
  int *po = ss->offsets;

#endif /* (!)SHORT_OFFSETS */

  if (ss->I < 0) 
  {
		int I, N, j;
		if (count < sizeof(int) * 2)
	    return 0;
		for (N = 0, j = 0; j < sizeof(int); j++)
	    N = (N << 8) + *++p;
		for (I = 0, j = 0; j < sizeof(int); j++)
	    I = (I << 8) + *++p;
		ss->N = N;
		ss->I = I;
		pr->ptr = p;
		if (N < 0 || N > ss->BlockSize || I < 0 || I >= N)
	    return ERRC;
		if (N == 0)
	    return EOFC;
		count -= sizeof(int) * 2;

		ss->bpos = 0;
		ss->bsize = N;
  }
  if (ss->filling) 
  {
		if (!s_buffered_process(st, pr, last))
	    return 0;
#ifdef SHORT_OFFSETS
		bwbsd_construct_offsets(ss, po64k, po4k, po1, ss->bsize);
#else /* !SHORT_OFFSETS */
		bwbsd_construct_offsets(ss, po, ss->bsize);
#endif /* (!)SHORT_OFFSETS */
		ss->bpos = 0;
		ss->i = ss->I;
  }
  
  while (q < wlimit && ss->bpos < ss->bsize) 
  {
		int i = ss->i;
		uchar b = ss->buffer[i];

#ifdef SHORT_OFFSETS
		uint d;
		uchar *pd = &po1[(i >> 1) + i];

		*++q = b;
		if (!(i & 1))
	    d = ((uint) pd[0] << 4) + (pd[1] >> 4);
		else
	    d = ((pd[0] & 0xf) << 8) + pd[1];
		ss->i = po64k[i >> 16].v[b] + po4k[i >> 12].v[b] + d;
#else /* !SHORT_OFFSETS */
		*++q = b;
		ss->i = po[i];
#endif /* (!)SHORT_OFFSETS */
		ss->bpos++;
  }
  if (ss->bpos == ss->bsize) 
  {
		ss->I = -1;
		ss->filling = true;
  }
  pw->ptr = q;
  if (q == wlimit)
		return 1;
  return 0;
}

int s_BT_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool )
{
	PSStreamBTState *const ss = (PSStreamBTState *) st;
  uchar *p = pr->ptr;
  uchar *q = pw->ptr;
  uint rcount = pr->limit - p;
  uint wcount = pw->limit - q;
  uint count;
  int status;

  if (rcount <= wcount)
		count = rcount, status = 0;
  else
		count = wcount, status = 1;
  while (count--)
		*++q = ss->table[*++p];
  pr->ptr = p;
  pw->ptr = q;
  return status;
}

PSStreamState * s_create_BT_state()
{
	return (PSStreamState * )(new PSStreamBTState);
}

int s_MTF_init(PSStreamState * st)
{
	PSStreamMTFState *const ss = (PSStreamMTFState *) st;
  int i;

  for (i = 0; i < 256; i++)
		ss->prev.b[i] = (uchar) i;
  return 0;
}

int s_MTFE_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool )
{
	PSStreamMTFState *const ss = (PSStreamMTFState *) st;
  uchar *p = pr->ptr;
  uchar *q = pw->ptr;
  uchar *rlimit = pr->limit;
  uint count = rlimit - p;
  uint wcount = pw->limit - q;
  int status =(count < wcount ? 0 : (rlimit = p + wcount, 1));

  while (p < rlimit) 
  {
		uchar b = *++p;
		int i;
		uchar prev = b, repl;

		for (i = 0; (repl = ss->prev.b[i]) != b; i++)
	    ss->prev.b[i] = prev, prev = repl;
		ss->prev.b[i] = prev;
		*++q = (uchar) i;
  }
  pr->ptr = p;
  pw->ptr = q;
  return status;
}

int s_MTFD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool )
{
	PSStreamMTFState *const ss = (PSStreamMTFState *) st;
  uchar *p = pr->ptr;
  uchar *q = pw->ptr;
  uchar *rlimit = pr->limit;
  uint count = rlimit - p;
  uint wcount = pw->limit - q;
  int status = (count <= wcount ? 0 : (rlimit = p + wcount, 1));

  uchar	v0 = ss->prev.b[0], v1 = ss->prev.b[1],	v2 = ss->prev.b[2], v3 = ss->prev.b[3];

  while (p < rlimit) 
  {
		uchar first;
		if (*++p == 0) 
		{
	    *++q = v0;
	    continue;
		}
		switch (*p) 
		{
	    default:
				{
		    	uint b = *p;
		    	uchar *bp = &ss->prev.b[b];

		    	*++q = first = *bp;
#if arch_sizeof_long == 4
		    	ss->prev.b[3] = v3;
#endif
		    
		    	for (;; bp--, b--) 
		    	{
						*bp = bp[-1];
						if (!(b & (sizeof(long) - 1)))
			         break;
		    	}
		    	
		    	for (; (b -= sizeof(long)) != 0;) 
		    	{
						bp -= sizeof(long);

#if arch_is_big_endian
						*(ulong *) bp = (*(ulong *) bp >> 8) | ((ulong) bp[-1] << ((sizeof(long) - 1) * 8));

#else
						*(ulong *) bp = (*(ulong *) bp << 8) | bp[-1];
#endif
		    	}
				}
#if arch_sizeof_long > 4	
				goto m7;
				
	    case 7:
				*++q = first = ss->prev.b[7];
m7:		
				ss->prev.b[7] = ss->prev.b[6];
				goto m6;
				
	    case 6:
				*++q = first = ss->prev.b[6];
m6:		
				ss->prev.b[6] = ss->prev.b[5];
				goto m5;
				
	    case 5:
				*++q = first = ss->prev.b[5];
m5:		
				ss->prev.b[5] = ss->prev.b[4];
				goto m4;
				
	    case 4:
				*++q = first = ss->prev.b[4];
m4:		
				ss->prev.b[4] = v3;
#endif
				goto m3;
				
	    case 3:
				*++q = first = v3;
m3:		
				v3 = v2, v2 = v1, v1 = v0, v0 = first;
				break;
				
	    case 2:
				*++q = first = v2;
				v2 = v1, v1 = v0, v0 = first;
				break;
				
	    case 1:
				*++q = first = v1;
				v1 = v0, v0 = first;
				break;
				
		}
  }
  ss->prev.b[0] = v0;
  ss->prev.b[1] = v1;
  ss->prev.b[2] = v2;
  ss->prev.b[3] = v3;
  pr->ptr = p;
  pw->ptr = q;
  return status;
}

PSStreamState* s_create_MTF_state()
{
	return (PSStreamState* )(new PSStreamMTFState);
}

#define CtrlA 0x01
#define CtrlC 0x03
#define CtrlD 0x04
#define CtrlE 0x05
#define CtrlQ 0x11
#define CtrlS 0x13
#define CtrlT 0x14
#define ESC 0x1b
#define CtrlBksl 0x1c

int s_xBCPE_process(PSStreamState * , 
                    PSStreamCursorRead * pr,
		                PSStreamCursorWrite * pw, 
		                bool , 
		                const uchar * escaped)
{
	uchar *p = pr->ptr;
  uchar *rlimit = pr->limit;
  uint rcount = rlimit - p;
  uchar *q = pw->ptr;
  uint wcount = pw->limit - q;
  uchar *end = p + qMin(rcount, wcount);

  while (p < end) 
  {
		uchar ch = *++p;

		if (ch <= 31 && escaped[ch]) 
		{
	    if (p == rlimit) 
	    {
				p--;
				break;
	    }
	    *++q = CtrlA;
	    ch ^= 0x40;
	    if (--wcount < rcount)
				end--;
		}
		*++q = ch;
  }
  pr->ptr = p;
  pw->ptr = q;
  return (p == rlimit ? 0 : 1);
}

int s_BCPE_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last)
{
	static const uchar escaped[32] =
    {
	0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0
    };

  return s_xBCPE_process(st, pr, pw, last, escaped);
}

int s_TBCPE_process(PSStreamState * st, PSStreamCursorRead * pr,PSStreamCursorWrite * pw, bool last)
{
	static const uchar escaped[32] =
    {
	0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0
    };

  return s_xBCPE_process(st, pr, pw, last, escaped);
}

int s_BCPD_init(PSStreamState * st)
{
	PSStreamBCPDState *const ss = (PSStreamBCPDState *) st;

  ss->escaped = 0;
  ss->matched = ss->copy_count = 0;
  return 0;
}

int s_BCPD_init(PSStreamState * st);
int s_xBCPD_process(PSStreamState * st, 
                    PSStreamCursorRead * pr,
		                PSStreamCursorWrite * pw, 
		                bool , 
		                bool tagged)
{
	PSStreamBCPDState *const ss = (PSStreamBCPDState *) st;
  uchar *p = pr->ptr;
  uchar *rlimit = pr->limit;
  uchar *q = pw->ptr;
  uchar *wlimit = pw->limit;
  int copy_count = ss->copy_count;
  int status;
  bool escaped = ss->escaped;

  for (;;) 
  {
		uchar ch;

		if (copy_count) 
		{
	    if (q == wlimit) 
	    {
				status = (p < rlimit ? 1 : 0);
				break;
	    }
	    *++q = *++(ss->copy_ptr);
	    copy_count--;
	    continue;
		}
		if (p == rlimit) 
		{
	    status = 0;
	    break;
		}
		ch = *++p;
		if (ch <= 31)
	    switch (ch) 
	    {
				case CtrlA:
		    	if (escaped) 
		    	{
						status = ERRC;
						goto out;
		    	}
		    	escaped = true;
		    	continue;
		    	
				case CtrlC:
		    	status = (*ss->signal_interrupt) (st);
		    	if (status < 0)
						goto out;
		    	continue;
		    	
				case CtrlD:
		    	if (escaped) 
		    	{
						status = ERRC;
						goto out;
		    	}
		    	status = EOFC;
		    	goto out;
		    	
				case CtrlE:
		    	continue;
		    	
				case CtrlQ:
		    	continue;
		    	
				case CtrlS:
		    	continue;
		    	
				case CtrlT:
		    	status = (*ss->request_status) (st);
		    	if (status < 0)
						goto out;
		    	continue;
		    	
				case CtrlBksl:
		    	continue;
		    	
	    }
			if (q == wlimit) 
			{
	    	p--;
	    	status = 1;
	    	break;
			}
			if (escaped) 
			{
	    	escaped = false;
	    	switch (ch) 
	    	{
					case '[':
		    		if (!tagged) 
		    		{
							status = ERRC;
							goto out;
		    		}
		    
					case 'A':
					case 'C':
					case 'D':
					case 'E':
					case 'Q':
					case 'S':
					case 'T':
					case '\\':
		    		ch ^= 0x40;
		    		break;
		    		
					case 'M':
		    		if (!tagged) 
		    		{
							status = ERRC;
							goto out;
		    		}
		    		continue;
		    		
					default:
		    		status = ERRC;
		    		goto out;
	    	}
			}
			*++q = ch;
    }
  
out:
	ss->copy_count = copy_count;
  ss->escaped = escaped;
  pr->ptr = p;
  pw->ptr = q;
  return status;
}

int s_BCPD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last)
{
	return s_xBCPD_process(st, pr, pw, last, false);
}

int s_TBCPD_process(PSStreamState * st, PSStreamCursorRead * pr,PSStreamCursorWrite * pw, bool last)
{
	return s_xBCPD_process(st, pr, pw, last, true);
}

PSStreamState *s_create_BCPD_state()
{
	return (PSStreamState *)(new PSStreamBCPDState);
}

#define T_MASK ((uint)~0)
#define T1 /* 0xd76aa478 */ (T_MASK ^ 0x28955b87)
#define T2 /* 0xe8c7b756 */ (T_MASK ^ 0x173848a9)
#define T3    0x242070db
#define T4 /* 0xc1bdceee */ (T_MASK ^ 0x3e423111)
#define T5 /* 0xf57c0faf */ (T_MASK ^ 0x0a83f050)
#define T6    0x4787c62a
#define T7 /* 0xa8304613 */ (T_MASK ^ 0x57cfb9ec)
#define T8 /* 0xfd469501 */ (T_MASK ^ 0x02b96afe)
#define T9    0x698098d8
#define T10 /* 0x8b44f7af */ (T_MASK ^ 0x74bb0850)
#define T11 /* 0xffff5bb1 */ (T_MASK ^ 0x0000a44e)
#define T12 /* 0x895cd7be */ (T_MASK ^ 0x76a32841)
#define T13    0x6b901122
#define T14 /* 0xfd987193 */ (T_MASK ^ 0x02678e6c)
#define T15 /* 0xa679438e */ (T_MASK ^ 0x5986bc71)
#define T16    0x49b40821
#define T17 /* 0xf61e2562 */ (T_MASK ^ 0x09e1da9d)
#define T18 /* 0xc040b340 */ (T_MASK ^ 0x3fbf4cbf)
#define T19    0x265e5a51
#define T20 /* 0xe9b6c7aa */ (T_MASK ^ 0x16493855)
#define T21 /* 0xd62f105d */ (T_MASK ^ 0x29d0efa2)
#define T22    0x02441453
#define T23 /* 0xd8a1e681 */ (T_MASK ^ 0x275e197e)
#define T24 /* 0xe7d3fbc8 */ (T_MASK ^ 0x182c0437)
#define T25    0x21e1cde6
#define T26 /* 0xc33707d6 */ (T_MASK ^ 0x3cc8f829)
#define T27 /* 0xf4d50d87 */ (T_MASK ^ 0x0b2af278)
#define T28    0x455a14ed
#define T29 /* 0xa9e3e905 */ (T_MASK ^ 0x561c16fa)
#define T30 /* 0xfcefa3f8 */ (T_MASK ^ 0x03105c07)
#define T31    0x676f02d9
#define T32 /* 0x8d2a4c8a */ (T_MASK ^ 0x72d5b375)
#define T33 /* 0xfffa3942 */ (T_MASK ^ 0x0005c6bd)
#define T34 /* 0x8771f681 */ (T_MASK ^ 0x788e097e)
#define T35    0x6d9d6122
#define T36 /* 0xfde5380c */ (T_MASK ^ 0x021ac7f3)
#define T37 /* 0xa4beea44 */ (T_MASK ^ 0x5b4115bb)
#define T38    0x4bdecfa9
#define T39 /* 0xf6bb4b60 */ (T_MASK ^ 0x0944b49f)
#define T40 /* 0xbebfbc70 */ (T_MASK ^ 0x4140438f)
#define T41    0x289b7ec6
#define T42 /* 0xeaa127fa */ (T_MASK ^ 0x155ed805)
#define T43 /* 0xd4ef3085 */ (T_MASK ^ 0x2b10cf7a)
#define T44    0x04881d05
#define T45 /* 0xd9d4d039 */ (T_MASK ^ 0x262b2fc6)
#define T46 /* 0xe6db99e5 */ (T_MASK ^ 0x1924661a)
#define T47    0x1fa27cf8
#define T48 /* 0xc4ac5665 */ (T_MASK ^ 0x3b53a99a)
#define T49 /* 0xf4292244 */ (T_MASK ^ 0x0bd6ddbb)
#define T50    0x432aff97
#define T51 /* 0xab9423a7 */ (T_MASK ^ 0x546bdc58)
#define T52 /* 0xfc93a039 */ (T_MASK ^ 0x036c5fc6)
#define T53    0x655b59c3
#define T54 /* 0x8f0ccc92 */ (T_MASK ^ 0x70f3336d)
#define T55 /* 0xffeff47d */ (T_MASK ^ 0x00100b82)
#define T56 /* 0x85845dd1 */ (T_MASK ^ 0x7a7ba22e)
#define T57    0x6fa87e4f
#define T58 /* 0xfe2ce6e0 */ (T_MASK ^ 0x01d3191f)
#define T59 /* 0xa3014314 */ (T_MASK ^ 0x5cfebceb)
#define T60    0x4e0811a1
#define T61 /* 0xf7537e82 */ (T_MASK ^ 0x08ac817d)
#define T62 /* 0xbd3af235 */ (T_MASK ^ 0x42c50dca)
#define T63    0x2ad7d2bb
#define T64 /* 0xeb86d391 */ (T_MASK ^ 0x14792c6e)

static void
md5_process(PSMD5State *pms, const uchar *data)
{
  uint	a = pms->abcd[0], b = pms->abcd[1], c = pms->abcd[2], d = pms->abcd[3];
  uint t;

#ifndef ARCH_IS_BIG_ENDIAN
# define ARCH_IS_BIG_ENDIAN 1	
#endif
#if ARCH_IS_BIG_ENDIAN

  uint X[16];
  const uchar *xp = data;
  int i;

  for (i = 0; i < 16; ++i, xp += 4)
		X[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);

#else  /* !ARCH_IS_BIG_ENDIAN */

  uint xbuf[16];
  const uint *X;

  if (!((data - (const uchar *)0) & 3)) 
  {
		X = (const uint *)data;
  } 
  else 
  {
		memcpy(xbuf, data, 64);
		X = xbuf;
  }
#endif

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + F(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
  
  SET(a, b, c, d,  0,  7,  T1);
  SET(d, a, b, c,  1, 12,  T2);
  SET(c, d, a, b,  2, 17,  T3);
  SET(b, c, d, a,  3, 22,  T4);
  SET(a, b, c, d,  4,  7,  T5);
  SET(d, a, b, c,  5, 12,  T6);
  SET(c, d, a, b,  6, 17,  T7);
  SET(b, c, d, a,  7, 22,  T8);
  SET(a, b, c, d,  8,  7,  T9);
  SET(d, a, b, c,  9, 12, T10);
  SET(c, d, a, b, 10, 17, T11);
  SET(b, c, d, a, 11, 22, T12);
  SET(a, b, c, d, 12,  7, T13);
  SET(d, a, b, c, 13, 12, T14);
  SET(c, d, a, b, 14, 17, T15);
  SET(b, c, d, a, 15, 22, T16);
#undef SET

#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + G(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b

  SET(a, b, c, d,  1,  5, T17);
  SET(d, a, b, c,  6,  9, T18);
  SET(c, d, a, b, 11, 14, T19);
  SET(b, c, d, a,  0, 20, T20);
  SET(a, b, c, d,  5,  5, T21);
  SET(d, a, b, c, 10,  9, T22);
  SET(c, d, a, b, 15, 14, T23);
  SET(b, c, d, a,  4, 20, T24);
  SET(a, b, c, d,  9,  5, T25);
  SET(d, a, b, c, 14,  9, T26);
  SET(c, d, a, b,  3, 14, T27);
  SET(b, c, d, a,  8, 20, T28);
  SET(a, b, c, d, 13,  5, T29);
  SET(d, a, b, c,  2,  9, T30);
  SET(c, d, a, b,  7, 14, T31);
  SET(b, c, d, a, 12, 20, T32);
#undef SET

#define H(x, y, z) ((x) ^ (y) ^ (z))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + H(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b

  SET(a, b, c, d,  5,  4, T33);
  SET(d, a, b, c,  8, 11, T34);
  SET(c, d, a, b, 11, 16, T35);
  SET(b, c, d, a, 14, 23, T36);
  SET(a, b, c, d,  1,  4, T37);
  SET(d, a, b, c,  4, 11, T38);
  SET(c, d, a, b,  7, 16, T39);
  SET(b, c, d, a, 10, 23, T40);
  SET(a, b, c, d, 13,  4, T41);
  SET(d, a, b, c,  0, 11, T42);
  SET(c, d, a, b,  3, 16, T43);
  SET(b, c, d, a,  6, 23, T44);
  SET(a, b, c, d,  9,  4, T45);
  SET(d, a, b, c, 12, 11, T46);
  SET(c, d, a, b, 15, 16, T47);
  SET(b, c, d, a,  2, 23, T48);
#undef SET

#define I(x, y, z) ((y) ^ ((x) | ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + I(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
  
  SET(a, b, c, d,  0,  6, T49);
  SET(d, a, b, c,  7, 10, T50);
  SET(c, d, a, b, 14, 15, T51);
  SET(b, c, d, a,  5, 21, T52);
  SET(a, b, c, d, 12,  6, T53);
  SET(d, a, b, c,  3, 10, T54);
  SET(c, d, a, b, 10, 15, T55);
  SET(b, c, d, a,  1, 21, T56);
  SET(a, b, c, d,  8,  6, T57);
  SET(d, a, b, c, 15, 10, T58);
  SET(c, d, a, b,  6, 15, T59);
  SET(b, c, d, a, 13, 21, T60);
  SET(a, b, c, d,  4,  6, T61);
  SET(d, a, b, c, 11, 10, T62);
  SET(c, d, a, b,  2, 15, T63);
  SET(b, c, d, a,  9, 21, T64);
#undef SET

  pms->abcd[0] += a;
  pms->abcd[1] += b;
  pms->abcd[2] += c;
  pms->abcd[3] += d;
}

void
md5_init(PSMD5State *pms)
{
  pms->count[0] = pms->count[1] = 0;
  pms->abcd[0] = 0x67452301;
  pms->abcd[1] = /*0xefcdab89*/ T_MASK ^ 0x10325476;
  pms->abcd[2] = /*0x98badcfe*/ T_MASK ^ 0x67452301;
  pms->abcd[3] = 0x10325476;
}

void
md5_append(PSMD5State *pms, const uchar *data, int nbytes)
{
  const uchar *p = data;
  int left = nbytes;
  int offset = (pms->count[0] >> 3) & 63;
  uint nbits = (uint)(nbytes << 3);

  if (nbytes <= 0)
		return;

  pms->count[1] += nbytes >> 29;
  pms->count[0] += nbits;
  if (pms->count[0] < nbits)
		pms->count[1]++;

  if (offset) 
  {
		int copy = (offset + nbytes > 64 ? 64 - offset : nbytes);

		memcpy(pms->buf + offset, p, copy);
		if (offset + copy < 64)
	    return;
		p += copy;
		left -= copy;
		md5_process(pms, pms->buf);
  }

  for (; left >= 64; p += 64, left -= 64)
		md5_process(pms, p);

  if (left)
		memcpy(pms->buf, p, left);
}

void
md5_finish(PSMD5State *pms, uchar digest[16])
{
  static const uchar pad[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
  uchar data[8];
  int i;

  for (i = 0; i < 8; ++i)
		data[i] = (uchar)(pms->count[i >> 2] >> ((i & 3) << 3));
  
  md5_append(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1);
  
  md5_append(pms, data, 8);
  for (i = 0; i < 16; ++i)
		digest[i] = (uchar)(pms->abcd[i >> 2] >> ((i & 3) << 3));
}

#undef T_MASK
#undef T1 
#undef T2 
#undef T3 
#undef T4
#undef T5
#undef T6
#undef T7
#undef T8 
#undef T9 
#undef T10 
#undef T11 
#undef T12 
#undef T13 
#undef T14 
#undef T15 
#undef T16 
#undef T17 
#undef T18 
#undef T19  
#undef T20 
#undef T21 
#undef T22 
#undef T23 
#undef T24 
#undef T25 
#undef T26 
#undef T27 
#undef T28 
#undef T29 
#undef T30 
#undef T31 
#undef T32 
#undef T33 
#undef T34 
#undef T35 
#undef T36 
#undef T37 
#undef T38 
#undef T39 
#undef T40
#undef T41 
#undef T42 
#undef T43 
#undef T44  
#undef T45 
#undef T46 
#undef T47 
#undef T48 
#undef T49 
#undef T50 
#undef T51 
#undef T52 
#undef T53 
#undef T54 
#undef T55 
#undef T56 
#undef T57  
#undef T58 
#undef T59 
#undef T60 
#undef T61 
#undef T62 
#undef T63 
#undef T64 

int s_MD5E_init(PSStreamState * st)
{
	PSStreamMD5EState *const ss = (PSStreamMD5EState *) st;

  md5_init(&ss->md5);
  return 0;
}

int s_MD5E_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last)
{
	PSStreamMD5EState *const ss = (PSStreamMD5EState *) st;
  int status = 0;

  if (pr->ptr < pr->limit) 
  {
		md5_append(&ss->md5, pr->ptr + 1, pr->limit - pr->ptr);
		pr->ptr = pr->limit;
  }
  
  if (last) 
  {
		if (pw->limit - pw->ptr >= 16) 
		{
	    md5_finish(&ss->md5, pw->ptr + 1);
	    pw->ptr += 16;
	    status = EOFC;
		} 
		else
	    status = 1;
  }
  return status;
}

PSStreamState *s_create_MD5E_state()
{
	return (PSStreamState *)(new PSStreamMD5EState);
}
