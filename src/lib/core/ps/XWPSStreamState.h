/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSSTREAMSTATE_H
#define XWPSSTREAMSTATE_H

#include <zlib.h>
#include "XWPSType.h"
#include "XWPSRef.h"

#define STREAM_MAX_ERROR_STRING 79

class XWPSStream;
class XWPSCosObject;
class XWPSDevicePDF;
class XWPSImagerState;
struct PSStreamState;


struct PSStreamTemplate
{
	int (*init)(PSStreamState*);
	int  (*process)(PSStreamState*, PSStreamCursorRead * , PSStreamCursorWrite * , bool );
	uint min_in_size;
	uint min_out_size;
	void (*release)(PSStreamState*);
	void (*set_defaults)(PSStreamState*);
	int (*reset)(PSStreamState*);
	PSStreamState* (*create_state)();
};

int filter_report_error(PSStreamState * st, const char *str);

extern const PSStreamTemplate s_NullE_template;
extern const PSStreamTemplate s_NullD_template;
extern const PSStreamTemplate s_Null1D_template;
extern const PSStreamTemplate s_A85D_template;
extern const PSStreamTemplate s_A85E_template;
extern const PSStreamTemplate s_AXE_template;
extern const PSStreamTemplate s_AXD_template;
extern const PSStreamTemplate s_PSSD_template;
extern const PSStreamTemplate s_PSSE_template;
extern const PSStreamTemplate cos_write_stream_template;
extern const PSStreamTemplate s_DCTD_template;
extern const PSStreamTemplate s_DCTE_template;
extern const PSStreamTemplate s_CFD_template;
extern const PSStreamTemplate s_CFE_template;
extern const PSStreamTemplate s_BHCD_template;
extern const PSStreamTemplate s_BHCE_template;
extern const PSStreamTemplate s_zlibD_template;
extern const PSStreamTemplate s_zlibE_template;
extern const PSStreamTemplate s_exE_template;
extern const PSStreamTemplate s_IScale_template;
extern const PSStreamTemplate s_IIEncode_template;
extern const PSStreamTemplate s_LZWD_template;
extern const PSStreamTemplate s_LZWE_template;
extern const PSStreamTemplate s_PNGPD_template;
extern const PSStreamTemplate s_PNGPE_template;
extern const PSStreamTemplate s_RLE_template;
extern const PSStreamTemplate s_RLD_template;
extern const PSStreamTemplate s_1_8_template;
extern const PSStreamTemplate s_2_8_template;
extern const PSStreamTemplate s_4_8_template;
extern const PSStreamTemplate s_12_8_template;
extern const PSStreamTemplate s_8_1_template;
extern const PSStreamTemplate s_8_2_template;
extern const PSStreamTemplate s_8_4_template;
extern const PSStreamTemplate s_C2R_template;
extern const PSStreamTemplate s_IE_template;
extern const PSStreamTemplate s_Subsample_template;
extern const PSStreamTemplate s_Average_template;
extern const PSStreamTemplate s_proc_read_template;
extern const PSStreamTemplate s_proc_write_template;
extern const PSStreamTemplate s_PFBD_template;
extern const PSStreamTemplate s_SFD_template;
extern const PSStreamTemplate s_exE_template;
extern const PSStreamTemplate s_exD_template;
extern const PSStreamTemplate s_PDiffE_template;
extern const PSStreamTemplate s_PDiffD_template;
extern const PSStreamTemplate s_BWBSE_template;
extern const PSStreamTemplate s_BWBSD_template;
extern const PSStreamTemplate s_BTE_template;
extern const PSStreamTemplate s_BTD_template;
extern const PSStreamTemplate s_MTFD_template;
extern const PSStreamTemplate s_MTFE_template;
extern const PSStreamTemplate s_BCPE_template;
extern const PSStreamTemplate s_TBCPE_template;
extern const PSStreamTemplate s_BCPD_template;
extern const PSStreamTemplate s_TBCPD_template;
extern const PSStreamTemplate s_MD5E_template;

extern PSStreamState * s_create_Null_state();
extern PSStreamState * s_create_A85D_state();
extern PSStreamState * s_create_A85E_state();
extern PSStreamState * s_create_AXE_state();
extern PSStreamState * s_create_AXD_state();
extern PSStreamState * s_create_PSSD_state();
extern PSStreamState * s_create_cos_write_stream_state();
extern PSStreamState * s_create_CFD_state();
extern PSStreamState * s_create_CFE_state();
extern PSStreamState * s_create_BHCD_state();
extern PSStreamState * s_create_BHCE_state();
extern PSStreamState * s_create_DCT_state();
extern PSStreamState * s_create_zlib_state();
extern PSStreamState * s_create_IScale_state();
extern PSStreamState * s_create_IIEncode_state();
extern PSStreamState * s_create_LZW_state();
extern PSStreamState * s_create_PNGP_state();
extern PSStreamState * s_create_RLE_state();
extern PSStreamState * s_create_RLD_state();
extern PSStreamState * s_create_1248_state();
extern PSStreamState * s_create_C2R_state();
extern PSStreamState * s_create_IE_state();
extern PSStreamState * s_create_Subsample_state();
extern PSStreamState * s_create_Average_state();
extern PSStreamState * s_create_proc_state();
extern PSStreamState * s_create_PFBD_state();
extern PSStreamState * s_create_SFD_state();
extern PSStreamState * s_create_exE_state();
extern PSStreamState * s_create_exD_state();
extern PSStreamState * s_create_PDiff_state();
extern PSStreamState * s_create_BWBS_state();
extern PSStreamState * s_create_BT_state();
extern PSStreamState * s_create_BT_state();
extern PSStreamState * s_create_MTF_state();
extern PSStreamState * s_create_BCPD_state();
extern PSStreamState * s_create_MD5E_state();

struct PSStreamState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
};

extern int  s_stdin_read_process(PSStreamState * st, 
                        PSStreamCursorRead * ignore_pr,
		                    PSStreamCursorWrite * pw, 
		                    bool last);
		                    
extern int s_Null_process(PSStreamState*, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool);
extern int s_Null1D_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);

struct PSStreamA85DState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	int odd;
	ulong word;
};

extern int s_A85D_init(PSStreamState*st);
extern int s_A85D_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool);


struct PSStreamA85EState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	int count;
  int last_char;
};

extern int s_A85E_init(PSStreamState*st);
extern int s_A85E_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool);


struct PSStreamAXEState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	bool EndOfData;
	int count;
};

extern int s_AXE_init(PSStreamState*st);
extern int s_AXE_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool);


struct PSStreamAXDState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	int odd;
};

extern int s_AXD_init(PSStreamState*st);
extern int s_AXD_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool);

extern int s_PSSE_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool);


struct PSStreamPSSDState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	bool from_string;
	int depth;
};

extern int s_PSSD_init(PSStreamState*st);
extern int s_PSSD_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool);


struct PSCosWriteStreamState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	XWPSCosObject *pcs;
  XWPSDevicePDF *pdev;
  XWPSStream *s;
  XWPSStream *target;
};

extern int cos_write_stream_process(PSStreamState * st, 
                                    PSStreamCursorRead * pr, 
                                    PSStreamCursorWrite * ignore_pw, 
                                    bool last);


#define jpeg_stream_data_common_init(pdata)\
  ((pdata)->Picky = 0, (pdata)->Relax = 0)
  
struct PSJPEGStreamData
{
	PSStreamTemplate templat;
	struct jpeg_error_mgr err;
	int Picky;
	int Relax;
};

struct PSJPEGCompressData
{
	PSStreamTemplate templat;
	struct jpeg_error_mgr err;
	int Picky;
	int Relax;
	struct jpeg_compress_struct cinfo;
  struct jpeg_destination_mgr destination;
  uchar finish_compress_buf[100];
  int fcb_size, fcb_pos;
};

struct PSJPEGDecompressData
{
	PSStreamTemplate templat;
	struct jpeg_error_mgr err;
	int Picky;
	int Relax;
	struct jpeg_decompress_struct dinfo;
  struct jpeg_source_mgr source;
  long skip;
  bool input_eod;
  bool faked_eoi;
  uchar *scanline_buffer;
  uint bytes_in_scanline;
};

struct PSStreamStateString
{
	uchar *data;
	uint   size;
};

struct PSStreamDCTState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	PSStreamStateString Markers;
	float QFactor;
	int ColorTransform;
	bool NoMarker;
	union _jd 
	{
		PSJPEGStreamData *common;
		PSJPEGCompressData *compress;
		PSJPEGDecompressData *decompress;
  } data;
  uint scan_line_size;
  int phase;
};

extern void s_DCT_set_defaults(PSStreamState * st);

extern int s_DCTD_init(PSStreamState * st);
extern int s_DCTD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
extern void s_DCTD_release(PSStreamState * st);
extern void s_DCTD_set_defaults(PSStreamState * st);

extern int s_DCTE_init(PSStreamState*st);
extern int s_DCTE_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool last);
extern void s_DCTE_release(PSStreamState * st);
extern void s_DCTE_set_defaults(PSStreamState * st);

void ps_jpeg_error_setup(PSStreamDCTState * st);
int  ps_jpeg_log_error(PSStreamDCTState * st);

extern int ps_jpeg_create_compress(PSStreamDCTState * st);
extern int ps_jpeg_set_defaults(PSStreamDCTState * st);
extern int ps_jpeg_set_colorspace(PSStreamDCTState * st, J_COLOR_SPACE colorspace);
extern int ps_jpeg_set_linear_quality(PSStreamDCTState * st,
			                                int scale_factor, 
			                                bool force_baseline);
extern int ps_jpeg_set_quality(PSStreamDCTState * st,
		                           int quality, 
		                           bool force_baseline);
extern int ps_jpeg_start_compress(PSStreamDCTState * st, bool write_all_tables);
extern int ps_jpeg_write_scanlines(PSStreamDCTState * st,
																	 JSAMPARRAY scanlines,
																	 int num_lines);
extern int ps_jpeg_finish_compress(PSStreamDCTState * st);
extern int ps_jpeg_destroy(PSStreamDCTState * st);

#define hc_bits_size (arch_sizeof_int * 8)

#if arch_sizeof_int > 2
#  define cfe_max_width (2560 * 32000 * 2 / 3)
#else
#  define cfe_max_width (max_int - 40)	/* avoid overflows */
#endif

#define run_eol_code_length 12
#define run_eol_code_value 1

#define cf2_run_pass_length 4
#define cf2_run_pass_value 0x1
#define cf2_run_vertical_offset 3

#define cf2_run_horizontal_value 1
#define cf2_run_horizontal_length 3

#define hcd_initial_bits 7


struct hcd_code 
{
    short value;
    ushort code_length;
} ;

typedef hcd_code cfd_node;

struct hce_code
{
	ushort code;
  ushort code_length;
};

typedef hce_code cfe_run;

#define run_length value

#define run_error (-1)
#define run_zeros (-2)
#define run_uncompressed (-3)
#define run2_pass (-4)
#define run2_horizontal (-5)

#define cfd_white_initial_bits 8
#define cfd_white_min_bits 4

#define cfd_black_initial_bits 7
#define cfd_black_min_bits 2

#define cfd_2d_initial_bits 7
#define cfd_2d_min_bits 4

#define cfd_uncompressed_initial_bits 6	
#define cf_byte_run_length byte_bit_run_length_neg
#define cf_byte_run_length_0 byte_bit_run_length_0

#define skip_white_pixels(data, p, count, white_byte, rlen)\
    rlen = cf_byte_run_length[count & 7][data ^ 0xff];\
    if ( rlen >= 8 ) {		/* run extends past byte boundary */\
	if ( white_byte == 0 ) {\
	    if ( p[0] ) { data = p[0]; p += 1; rlen -= 8; }\
	    else if ( p[1] ) { data = p[1]; p += 2; }\
	    else {\
		while ( !(p[2] | p[3] | p[4] | p[5]) )\
		    p += 4, rlen += 32;\
		if ( p[2] ) {\
		    data = p[2]; p += 3; rlen += 8;\
		} else if ( p[3] ) {\
		    data = p[3]; p += 4; rlen += 16;\
		} else if ( p[4] ) {\
		    data = p[4]; p += 5; rlen += 24;\
		} else /* p[5] */ {\
		    data = p[5]; p += 6; rlen += 32;\
		}\
	    }\
	} else {\
	    if ( p[0] != 0xff ) { data = (uchar)~p[0]; p += 1; rlen -= 8; }\
	    else if ( p[1] != 0xff ) { data = (uchar)~p[1]; p += 2; }\
	    else {\
		while ( (p[2] & p[3] & p[4] & p[5]) == 0xff )\
		    p += 4, rlen += 32;\
		if ( p[2] != 0xff ) {\
		    data = (uchar)~p[2]; p += 3; rlen += 8;\
		} else if ( p[3] != 0xff ) {\
		    data = (uchar)~p[3]; p += 4; rlen += 16;\
		} else if ( p[4] != 0xff ) {\
		    data = (uchar)~p[4]; p += 5; rlen += 24;\
		} else /* p[5] != 0xff */ {\
		    data = (uchar)~p[5]; p += 6; rlen += 32;\
		}\
	    }\
	}\
	rlen += cf_byte_run_length_0[data ^ 0xff];\
    }\
    count -= rlen;

#define skip_black_pixels(data, p, count, white_byte, rlen)\
    rlen = cf_byte_run_length[count & 7][data];\
    if ( rlen >= 8 ) {\
	if ( white_byte == 0 )\
	    for ( ; ; p += 4, rlen += 32 ) {\
		if ( p[0] != 0xff ) { data = p[0]; p += 1; rlen -= 8; break; }\
		if ( p[1] != 0xff ) { data = p[1]; p += 2; break; }\
		if ( p[2] != 0xff ) { data = p[2]; p += 3; rlen += 8; break; }\
		if ( p[3] != 0xff ) { data = p[3]; p += 4; rlen += 16; break; }\
	    }\
	else\
	    for ( ; ; p += 4, rlen += 32 ) {\
		if ( p[0] ) { data = (uchar)~p[0]; p += 1; rlen -= 8; break; }\
		if ( p[1] ) { data = (uchar)~p[1]; p += 2; break; }\
		if ( p[2] ) { data = (uchar)~p[2]; p += 3; rlen += 8; break; }\
		if ( p[3] ) { data = (uchar)~p[3]; p += 4; rlen += 16; break; }\
	    }\
	rlen += cf_byte_run_length_0[data];\
    }\
    count -= rlen;

struct cf_runs 
{
  cfe_run termination[64];
  cfe_run make_up[41];
};

struct hce_table 
{
    uint count;
    hce_code *codes;
} ;

struct hcd_table 
{
    uint count;
    uint initial_bits;
    hcd_code *codes;
} ;

struct PSCountNode 
{
    long freq;
    uint value;
    uint code_length;	
    PSCountNode *next;	
    PSCountNode *left;	
    PSCountNode *right;
};

struct PSHCDefinition 
{
    ushort *counts;	
    uint num_counts;
    ushort *values;	
    uint num_values;
} ;

#define hc_put_value(ss, q, code, clen)\
  (hc_print_value_then(code, clen)\
   ((bits_left -= (clen)) >= 0 ?\
    (bits += (code) << bits_left) :\
    (hc_put_code_proc((ss)->FirstBitLowOrder,\
		      q += hc_bits_size >> 3,\
		      (bits + ((code) >> -bits_left))),\
     bits = (code) << (bits_left += hc_bits_size))))
     
#define hc_put_code(ss, q, cp)\
  hc_put_value(ss, q, (cp)->code, (cp)->code_length)
  
#define hc_put_last_bits(ss, q)\
  hc_put_last_bits_proc(ss, q, bits, bits_left)
  
#define hcd_store_state()\
	pr->ptr = p -= (bits_left >> 3),\
	ss->bits = bits >>= (bits_left & ~7),\
	ss->bits_left = bits_left &= 7
	
#define hce_declare_state\
	uint bits;\
	int bits_left
	
#define hce_load_state()\
	bits = ss->bits, bits_left = ss->bits_left
	
#define hce_store_state()\
	ss->bits = bits, ss->bits_left = bits_left

#define hce_bits_available(n)\
  (ss->bits_left >= (n) || wlimit - q > ((n) - ss->bits_left - 1) >> 3)
  
#define hc_print_value(code, clen) 0
#define hc_print_value_then(code, clen)
#define hc_print_code(rp) hc_print_value((rp)->code, (rp)->code_length)

int hc_compute(PSHCDefinition * def, const long *freqs);
int hc_bytes_from_definition(uchar * dbytes, const PSHCDefinition * def);
void hc_sizes_from_bytes(PSHCDefinition * def, const uchar * dbytes, int num_bytes);
void hc_definition_from_bytes(PSHCDefinition * def, const uchar * dbytes);
void hc_make_encoding(hce_code * encode, const PSHCDefinition * def);
uint hc_sizeof_decoding(const PSHCDefinition * def, int initial_bits);
void hc_make_decoding(hcd_code * decode, const PSHCDefinition * def, int initial_bits);

struct PSStreamHCState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	bool FirstBitLowOrder;
	uint bits;
	int bits_left;
};

extern uchar * hc_put_last_bits_proc(PSStreamHCState * ss, uchar * q, uint bits, int bits_left);

struct PSStreamCFState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	bool FirstBitLowOrder;
	uint bits;
	int bits_left;
	bool Uncompressed;
	int K;
	bool EndOfLine;
	bool EncodedByteAlign;
	int Columns;
	int Rows;
	bool EndOfBlock;
	bool BlackIs1;
	int DamagedRowsBeforeError;
	int DecodedByteAlign;
	uint raster;
	uchar *lbuf;
	uchar *lprev;
	int k_left;
};

extern void s_CF_set_defaults_inline(PSStreamCFState * ss);

struct PSStreamCFDState 
{
  const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	bool FirstBitLowOrder;
	uint bits;
	int bits_left;
	bool Uncompressed;
	int K;
	bool EndOfLine;
	bool EncodedByteAlign;
	int Columns;
	int Rows;
	bool EndOfBlock;
	bool BlackIs1;
	int DamagedRowsBeforeError;
	int DecodedByteAlign;
	uint raster;
	uchar *lbuf;
	uchar *lprev;
	int k_left;
  int cbit;
  int rows_left;
  int rpos;	
  int wpos;	
  int eol_count;
  uchar invert;
  int run_color;
  int damaged_rows;
  bool skipping_damage;	
  int uncomp_run;	
  int uncomp_left;
  int uncomp_exit;
} ;

void s_CFD_set_defaults(PSStreamState * st);
int  s_CFD_init(PSStreamState * st);
void s_CFD_release(PSStreamState * st);
int s_CFD_process(PSStreamState * st, PSStreamCursorRead * pr,  PSStreamCursorWrite * pw, bool last);

struct PSStreamCFEState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	bool FirstBitLowOrder;
	uint bits;
	int bits_left;
	bool Uncompressed;
	int K;
	bool EndOfLine;
	bool EncodedByteAlign;
	int Columns;
	int Rows;
	bool EndOfBlock;
	bool BlackIs1;
	int DamagedRowsBeforeError;
	int DecodedByteAlign;
	uint raster;
	uchar *lbuf;
	uchar *lprev;
	int k_left;
	int max_code_bytes;
	uchar *lcode;
	int read_count;
	int write_count;
	int code_bytes;
};

extern int s_CFE_init(PSStreamState*st);
extern int s_CFE_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool last);
extern void s_CFE_release(PSStreamState * st);
extern void s_CFE_set_defaults(PSStreamState * st);

#define max_hc_length 16

#define max_zero_run 100

#define hcd_declare_state\
	uchar *p;\
	uchar *rlimit;\
	uint bits;\
	int bits_left
	
#define hcd_load_state()\
	p = pr->ptr,\
	rlimit = pr->limit,\
	bits = ss->bits,\
	bits_left = ss->bits_left
	
#define hcd_store_state()\
	pr->ptr = p -= (bits_left >> 3),\
	ss->bits = bits >>= (bits_left & ~7),\
	ss->bits_left = bits_left &= 7
	
#define hcd_bits_available(n)\
  (bits_left >= (n) || rlimit - p > ((n) - bits_left - 1) >> 3)
	
#define HCD_ENSURE_BITS_ELSE(n)\
  if (bits_left >= n)\
    do{}while(0);\
  else HCD_MORE_BITS_ELSE
  	
#define hcd_ensure_bits(n, outl)\
  do{HCD_ENSURE_BITS_ELSE(n) goto outl; }while(0)
  
#define HCD_MORE_BITS_1_ELSE\
  if (p < rlimit) {\
    int c = *++p;\
\
    if (ss->FirstBitLowOrder)\
      c = byte_reverse_bits[c];\
    bits = (bits << 8) + c, bits_left += 8;\
  } else
  	
#if hc_bits_size == 16
#  define HCD_MORE_BITS_ELSE HCD_MORE_BITS_1_ELSE
#else /* hc_bits_size >= 32 */
#  define HCD_MORE_BITS_ELSE\
  if (rlimit - p >= 3) {\
    if (ss->FirstBitLowOrder)\
      bits = (bits << 24) + ((uint)byte_reverse_bits[p[1]] << 16) + ((uint)byte_reverse_bits[p[2]] << 8) + byte_reverse_bits[p[3]];\
    else\
      bits = (bits << 24) + ((uint)p[1] << 16) + ((uint)p[2] << 8) + p[3];\
    bits_left += 24, p += 3;\
  } else HCD_MORE_BITS_1_ELSE
#endif

#define hcd_more_bits(outl)\
  do{HCD_MORE_BITS_ELSE goto outl; while(0)
  
#define hcd_peek_bits(n) ((bits >> (bits_left - (n))) & ((1 << (n)) - 1))

#define hcd_peek_var_bits(n)\
  ((bits >> (bits_left - (n))) & byte_right_mask[n])
  
#define hcd_peek_bits_left()\
  (bits & byte_right_mask[bits_left])

#define hcd_skip_bits(n) (bits_left -= (n))

#define bhcd_declare_state\
	hcd_declare_state;\
	int zeros
	
#define bhcd_load_state()\
	hcd_load_state(), zeros = ss->zeros
	
#define bhcd_store_state()\
	hcd_store_state(), ss->zeros = zeros

struct PSStreamBHCState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	bool FirstBitLowOrder;
	uint bits;
	int bits_left;
	PSHCDefinition definition;
	bool EndOfData;
	uint EncodeZeroRuns;
	int zeros;
};


struct PSStreamBHCDState 
{
  const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	bool FirstBitLowOrder;
	uint bits;
	int bits_left;
	PSHCDefinition definition;
	bool EndOfData;
	uint EncodeZeroRuns;
	int zeros;
  hcd_table decode;
} ;

int s_BHCD_reinit(PSStreamState * st);
int s_BHCD_init(PSStreamState * st);
void s_BHCD_release(PSStreamState * st);
int s_BHCD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);

struct PSStreamBHCEState 
{
  const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	bool FirstBitLowOrder;
	uint bits;
	int bits_left;
	PSHCDefinition definition;
	bool EndOfData;
	uint EncodeZeroRuns;
	int zeros;
  hce_table encode;
} ;

int s_BHCE_reinit(PSStreamState * st);
int s_BHCE_init(PSStreamState * st);
void s_BHCE_release(PSStreamState * st);
int s_BHCE_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);

struct zlib_block
{
	void *data;
  zlib_block *next;
  zlib_block *prev;
};

struct PSZLibDynamicState
{
	zlib_block *blocks;
  z_stream zstate;
};

struct PSStreamZLibState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	int windowBits;
  bool no_wrapper;
  int level;
  int method;
  int memLevel;
  int strategy;
  PSZLibDynamicState * dynamic;
};

extern int  s_zlib_alloc_dynamic_state(PSStreamZLibState *ss);
extern void s_zlib_free_dynamic_state(PSStreamZLibState *ss);

extern void s_zlib_set_defaults(PSStreamState * st);

extern int s_zlibD_init(PSStreamState * st);
extern int s_zlibD_reset(PSStreamState * st);
extern int s_zlibD_process(PSStreamState * st, PSStreamCursorRead * pr,	PSStreamCursorWrite * pw, bool ignore_last);
extern void s_zlibD_release(PSStreamState * st);

extern int  s_zlibE_init(PSStreamState * st);
extern int s_zlibE_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool last);
extern void s_zlibE_release(PSStreamState * st);
extern int  s_zlibE_reset(PSStreamState * st);

struct PSStreamExEState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	ushort cstate;
};

extern int s_exE_process(PSStreamState*st, PSStreamCursorRead * pr, PSStreamCursorWrite *pw, bool last);

#define LOG2_MAX_ISCALE_SUPPORT 3
#define MAX_ISCALE_SUPPORT (1 << LOG2_MAX_ISCALE_SUPPORT)

struct PSStreamImageScaleParams
{
	int Colors;
  int BitsPerComponentIn;
  uint MaxValueIn;
  int WidthIn, HeightIn;
  int BitsPerComponentOut;
  uint MaxValueOut;
  int WidthOut, HeightOut;
};

struct PSStreamImageScaleState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	PSStreamImageScaleParams params;
};

typedef struct {
    float weight;	
} CONTRIB;

typedef struct {
    int index;		
    int n;	
    int first_pixel;
} CLIST;

struct PSStreamIScaleState 
{
  const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	PSStreamImageScaleParams params;
	
  int sizeofPixelIn;
  int sizeofPixelOut;	
  double xscale, yscale;
  void *src;
  void *dst;
  uchar *tmp;
  CLIST *contrib;
  CONTRIB *items;
  int src_y;
  uint src_offset, src_size;
  int dst_y;
  uint dst_offset, dst_size;
  CLIST dst_next_list;
  int dst_last_index;	
  CONTRIB dst_items[MAX_ISCALE_SUPPORT];
} ;

void s_IScale_set_defaults(PSStreamState * st);
int s_IScale_init(PSStreamState * st);
int s_IScale_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
void s_IScale_release(PSStreamState * st);

struct PSStreamIIEncodeState 
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	PSStreamImageScaleParams params;
	
  int sizeofPixelIn;
  int sizeofPixelOut;
  uint src_size;
  uint dst_size;
  void *prev;
  void *cur;
  PSScaleCase scale_case;
  int dst_x;
  PSDDAInt dda_x;
  PSDDAInt dda_x_init;
  int src_y, dst_y;
  PSDDAInt dda_y;	
  int src_offset, dst_offset;
} ;

int s_IIEncode_init(PSStreamState * st);
int s_IIEncode_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
void s_IIEncode_release(PSStreamState * st);

struct PSLZWDecode
{
	 uchar datum;
   uchar len;	
   ushort prefix;
};

struct PSStreamLZWState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	int InitialCodeLength;
	bool FirstBitLowOrder;
	bool BlockData;
	int EarlyChange;
	uint bits;
	int bits_left;
	int bytes_left;
	union _lzt 
	{
		PSLZWDecode *decode;
//		lzw_encode_table *encode;
  } table;
  uint next_code;
  int code_size;
  int prev_code;
  uint prev_len;
  int copy_code;
  uint copy_len;
  int copy_left;
  bool first;
};

int s_LZWD_init(PSStreamState * st);
int s_LZWE_init(PSStreamState * st);
int s_LZWD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
int s_LZWE_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
void s_LZW_set_defaults(PSStreamState * st);
void s_LZW_release(PSStreamState * st);
int s_LZWD_reset(PSStreamState * st);
int s_LZWE_reset(PSStreamState * st);

struct PSStreamPNGPState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	int Colors;
	int BitsPerComponent;
	uint Columns;
	int Predictor;
	uint row_count;
	uchar end_mask;
	int bpp;
	uchar *prev_row;
	int case_index;
	long row_left;
	uchar prev[32];
};

int s_PNGPD_init(PSStreamState * st);
int s_PNGPE_init(PSStreamState * st);
int s_PNGPD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
int s_PNGPE_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
void s_PNGP_set_defaults(PSStreamState * st);
void s_PNGP_release(PSStreamState *st);
int  s_PNGP_reinit(PSStreamState * st);

struct PSStreamRLEState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	bool EndOfData;
	ulong record_size;
	ulong record_left;
	int copy_left;
};

int  s_RLE_init(PSStreamState * st);
int  s_RLE_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
void s_RLE_set_defaults(PSStreamState * st);

struct PSStreamRLDState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	bool EndOfData;
	int copy_left;
	int copy_data;
};

int  s_RLD_init(PSStreamState * st);
int  s_RLD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
void s_RLD_set_defaults(PSStreamState * st);

struct PSStream1248State
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	uint samples_per_row;
	int bits_per_sample;
	uint left;
};

int s_1248_init(PSStream1248State *ss, int Columns, int samples_per_pixel);
int s_1_init(PSStreamState * st);
int s_2_init(PSStreamState * st);
int s_4_init(PSStreamState * st);
int s_12_init(PSStreamState * st);
int s_N_8_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
int s_12_8_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
int s_8_N_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);

struct PSStreamC2RState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	XWPSImagerState *pis;
};

int  s_C2R_init(PSStreamC2RState *ss, XWPSImagerState *pis);
int  s_C2R_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
void s_C2R_set_defaults(PSStreamState * st);

struct PSStreamIEState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	int BitsPerComponent;
  int NumComponents;
  int Width;
  int BitsPerIndex;
  const float *Decode;
  PSByteString Table;
  int hash_table[400];
  int next_index;
  uint byte_in;
  int in_bits_left;
  int next_component;
  uint byte_out;
  int x;
};

int  s_IE_init(PSStreamState * st);
int  s_IE_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
void s_IE_set_defaults(PSStreamState * st);

struct PSStreamDownsampleState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	int Colors;
	int WidthIn, HeightIn;
	int XFactor, YFactor;
	bool AntiAlias;
	bool padX, padY;
	int x, y;
};

int s_Downsample_size_out(int size_in, int factor, bool pad);
void s_Downsample_set_defaults(PSStreamState * st);

struct PSStreamSubsampleState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	int Colors;
	int WidthIn, HeightIn;
	int XFactor, YFactor;
	bool AntiAlias;
	bool padX, padY;
	int x, y;
};

int s_Subsample_init(PSStreamState * st);
int s_Subsample_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);

struct PSStreamAverageState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	int Colors;
	int WidthIn, HeightIn;
	int XFactor, YFactor;
	bool AntiAlias;
	bool padX, padY;
	int x, y;
	uint sum_size;
  uint copy_size;
  uint *sums;
};

int  s_Average_init(PSStreamState * st);
int  s_Average_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
void s_Average_set_defaults(PSStreamState * st);
void s_Average_release(PSStreamState * st);

struct PSStreamProcState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	bool eof;
  uint index;	
  XWPSRef proc;
  XWPSRef data;
};

void s_proc_set_defaults(PSStreamState * st);
int s_proc_read_process(PSStreamState * st, PSStreamCursorRead * ignore_pr,  PSStreamCursorWrite * pw, bool last);
int s_proc_write_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * ignore_pw, bool last);

struct PSStreamexEState 
{
  const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
  ushort cstate;
};

int s_exE_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);

struct PSStreamPFBDState 
{
  const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
  int binary_to_hex;
  int record_type;
  ulong record_left;
};

int s_PFBD_init(PSStreamState * st);
int s_PFBD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);

struct PSStreamexDState 
{
  const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
  ushort cstate;
  int binary;	
  int lenIV;
  PSStreamPFBDState *pfb_state;
  int odd;
  long record_left;	
  long hex_left;
  int skip;	
} ;

int  s_exD_init(PSStreamState * st);
void s_exD_set_defaults(PSStreamState * st);
int  s_exD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);

struct PSStreamSFDState 
{
  const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];    
	long count;
  PSStreamStateString eod;
  long skip_count;
  uint match;	
  uint copy_count;	
  uint copy_ptr;
};

void s_SFD_set_defaults(PSStreamState * st);
int  s_SFD_init(PSStreamState * st);
int  s_SFD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);

#define s_PDiff_max_Colors 16

struct PSStreamPDiffState 
{
  const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
  int Colors;	
  int BitsPerComponent;	
  int Columns;
  uint row_count;
  uchar end_mask;	
  int case_index;	
  uint row_left;
  uchar prev[s_PDiff_max_Colors];	
};

void s_PDiff_set_defaults(PSStreamState * st);
int  s_PDiff_reinit(PSStreamState * st);
int  s_PDiffD_init(PSStreamState * st);
int  s_PDiffE_init(PSStreamState * st);
int s_PDiff_process(PSStreamState * st, PSStreamCursorRead * pr,PSStreamCursorWrite * pw, bool last);

struct PSOffsetsFull 
{
    uint v[256];
} ;

struct PSStreamBufferedState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	int BlockSize;
	uchar *buffer;
	bool filling;
	int bsize;
	int bpos;
};

void s_buffered_set_defaults(PSStreamState * st);
int  s_buffered_no_block_init(PSStreamState * st);
int  s_buffered_block_init(PSStreamState * st);
int  s_buffered_process(PSStreamState * st, PSStreamCursorRead * pr, bool last);
void s_buffered_release(PSStreamState * st);

struct PSStreamBWBSState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	int BlockSize;
	uchar *buffer;
	bool filling;
	int bsize;
	int bpos;
	void *offsets;
	int N;
	int I;
	int i;
};

typedef PSStreamBWBSState PSStreamBWBSEState;
typedef PSStreamBWBSState PSStreamBWBSDState;

void s_BWBS_set_defaults(PSStreamState * st);
int  bwbs_init(PSStreamState * st, uint osize);
void s_BWBS_release(PSStreamState * st);

int  s_BWBSE_init(PSStreamState * st);
int  s_BWBSE_process(PSStreamState * st, PSStreamCursorRead * pr,	PSStreamCursorWrite * pw, bool last);

int s_BWBSD_init(PSStreamState * st);
int s_BWBSD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);

struct PSStreamBTState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	uchar table[256];
};

typedef PSStreamBTState PSStreamBTEState;
typedef PSStreamBTState PSStreamBTDState;

int s_BT_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);

struct PSStreamMTFState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	union _p 
	{
		ulong l[256 / sizeof(long)];
		uchar b[256];
  } prev;
};

typedef PSStreamMTFState PSStreamMTFEState;
typedef PSStreamMTFState PSStreamMTFDState;

int s_MTF_init(PSStreamState * st);
int s_MTFE_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);

int s_MTFD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);

struct PSStreamBCPDState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	int (*signal_interrupt) (PSStreamState *);
  int (*request_status) (PSStreamState *);
  bool escaped;
  int matched;
  int copy_count;
  const uchar *copy_ptr;
};

int s_xBCPE_process(PSStreamState * st, 
                    PSStreamCursorRead * pr,
		                PSStreamCursorWrite * pw, 
		                bool last, 
		                const uchar * escaped);
		                
int s_BCPE_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
int s_TBCPE_process(PSStreamState * st, PSStreamCursorRead * pr,PSStreamCursorWrite * pw, bool last);

int s_BCPD_init(PSStreamState * st);
int s_xBCPD_process(PSStreamState * st, 
                    PSStreamCursorRead * pr,
		                PSStreamCursorRead * pw, 
		                bool last, 
		                bool tagged);
		                
int s_BCPD_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);
int s_TBCPD_process(PSStreamState * st, PSStreamCursorRead * pr,PSStreamCursorWrite * pw, bool last);

struct PSMD5State
{
	uint count[2];
	uint abcd[4];
	uchar buf[64];
};

void md5_init(PSMD5State *pms);
void md5_append(PSMD5State *pms, const uchar *data, int nbytes);
void md5_finish(PSMD5State *pms, uchar digest[16]);

struct PSStreamMD5EState
{
	const PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	PSMD5State md5;
};

int s_MD5E_init(PSStreamState * st);
int s_MD5E_process(PSStreamState * st, PSStreamCursorRead * pr, PSStreamCursorWrite * pw, bool last);

#endif //XWPSSTREAMSTATE_H