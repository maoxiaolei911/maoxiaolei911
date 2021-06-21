/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSTYPE_H
#define XWPSTYPE_H

#include <stdio.h>
#include <QtGlobal>
#include "arch.h"
#include "jpeglib.h"

class XWPSMatrix;
class XWPSMatrixFixed;
class XWPSImagerState;
class XWPSFixedCoeff;
class XWPSFixedRect;
class XWPSIntRect;

#ifndef USE_ASM
#define USE_ASM 0
#endif

#ifndef USE_FPU
#define USE_FPU (387-0)
#endif


#ifdef USE_FPU
#  define USE_FPU_FIXED (USE_FPU < 0 && arch_floats_are_IEEE && arch_sizeof_long == 4)
#else
#  define USE_FPU_FIXED 0
#endif

#define EOFC ((int)(-1))
#define ERRC ((int)(-2))
#define INTC ((int)(-3))
#define CALLC ((int)(-4))

#define _OFFSET_OF_(t, m) ((int) &((t *) 0)->m)
#define _COUNT_OF_(a) (sizeof(a) / sizeof((a)[0]))

#define arch_align_short_mod ARCH_ALIGN_SHORT_MOD
#define arch_align_int_mod ARCH_ALIGN_INT_MOD
#define arch_align_long_mod ARCH_ALIGN_LONG_MOD
#define arch_align_ptr_mod ARCH_ALIGN_PTR_MOD
#define arch_align_float_mod ARCH_ALIGN_FLOAT_MOD
#define arch_align_double_mod ARCH_ALIGN_DOUBLE_MOD
#define arch_log2_sizeof_short ARCH_LOG2_SIZEOF_SHORT
#define arch_log2_sizeof_int ARCH_LOG2_SIZEOF_INT
#define arch_log2_sizeof_long ARCH_LOG2_SIZEOF_LONG
#define arch_sizeof_ptr ARCH_SIZEOF_PTR
#define arch_sizeof_float ARCH_SIZEOF_FLOAT
#define arch_sizeof_double ARCH_SIZEOF_DOUBLE
#define arch_float_mantissa_bits ARCH_FLOAT_MANTISSA_BITS
#define arch_double_mantissa_bits ARCH_DOUBLE_MANTISSA_BITS
#define arch_max_uchar ARCH_MAX_UCHAR
#define arch_max_ushort ARCH_MAX_USHORT
#define arch_max_uint ARCH_MAX_UINT
#define arch_max_ulong ARCH_MAX_ULONG
#define arch_is_big_endian ARCH_IS_BIG_ENDIAN
#define arch_ptrs_are_signed ARCH_PTRS_ARE_SIGNED
#define arch_floats_are_IEEE ARCH_FLOATS_ARE_IEEE
#define arch_arith_rshift ARCH_ARITH_RSHIFT
#define arch_can_shift_full_long ARCH_CAN_SHIFT_FULL_LONG

#define ARCH_SIZEOF_CHAR (1 << ARCH_LOG2_SIZEOF_CHAR)
#define ARCH_SIZEOF_SHORT (1 << ARCH_LOG2_SIZEOF_SHORT)
#define ARCH_SIZEOF_INT (1 << ARCH_LOG2_SIZEOF_INT)
#define ARCH_SIZEOF_LONG (1 << ARCH_LOG2_SIZEOF_LONG)
#define ARCH_INTS_ARE_SHORT (ARCH_SIZEOF_INT == ARCH_SIZEOF_SHORT)
/* Backward compatibility */
#define arch_sizeof_short ARCH_SIZEOF_SHORT
#define arch_sizeof_int ARCH_SIZEOF_INT
#define arch_sizeof_long ARCH_SIZEOF_LONG

#define arch_sizeof_short ARCH_SIZEOF_SHORT
#define arch_sizeof_int ARCH_SIZEOF_INT
#define arch_sizeof_long ARCH_SIZEOF_LONG
#define arch_ints_are_short ARCH_INTS_ARE_SHORT

#define arch_log2_sizeof_frac arch_log2_sizeof_short
#define arch_sizeof_frac arch_sizeof_short

#define ARCH_SMALL_MEMORY (ARCH_SIZEOF_INT <= 2)
#define arch_small_memory ARCH_SMALL_MEMORY

#define min_short (-1 << (arch_sizeof_short * 8 - 1))
#define max_short (~min_short)
#define min_int (-1 << (arch_sizeof_int * 8 - 1))
#define max_int (~min_int)
#define min_long (-1L << (arch_sizeof_long * 8 - 1))
#define max_long (~min_long)

#define max_uchar arch_max_uchar
#define max_ushort arch_max_ushort
#define max_uint arch_max_uint
#define max_ulong arch_max_ulong

#define PS_DEVICE_COLOR_MAX_COMPONENTS 6
#define PS_CLIENT_COLOR_MAX_COMPONENTS 6

#define MAX_INLINE_IMAGE_BYTES 4000

#define MAX_REF_CHARS ((sizeof(long) * 8 + 2) / 3)

#define MAX_FN_NAME_CHARS 9	
#define MAX_FN_CHARS (MAX_REF_CHARS + 4)

#define MAX_RECT_STRING 100
#define MAX_BORDER_STRING 100

#define YMULT_LIMIT (max_fixed / fixed_1)

#define ps_no_id 0L
#define ps_no_bitmap_id     ps_no_id

#define ps_rule_winding_number (-1)
#define ps_rule_even_odd 1
  
#define ALIGNMENT_MOD(ptr, modu)\
  ((uint)( ((const char *)(ptr) - (const char *)0) & ((modu) - 1) ))
  
#define SWAP(a, b, t)\
  (t = a, a = b, b = t)

#define arith_rshift_slow(x,n) ((x) < 0 ? ~(~(x) >> (n)) : (x) >> (n))
#if arch_arith_rshift == 2
#  define arith_rshift(x,n) ((x) >> (n))
#  define arith_rshift_1(x) ((x) >> 1)
#else
#if arch_arith_rshift == 1	/* OK except for n=1 */
#  define arith_rshift(x,n) ((x) >> (n))
#  define arith_rshift_1(x) arith_rshift_slow(x,1)
#else
#  define arith_rshift(x,n) arith_rshift_slow(x,n)
#  define arith_rshift_1(x) arith_rshift_slow(x,1)
#endif
#endif

#ifndef M_PI
#  ifdef PI
#    define M_PI PI
#  else
#    define M_PI 3.14159265358979324
#  endif
#endif

#define degrees_to_radians (M_PI / 180.0)
#define radians_to_degrees (180.0 / M_PI)

#define is_fzero(f) ((f) == 0.0)
#define is_fzero2(f1,f2) ((f1) == 0.0 && (f2) == 0.0)
#define is_fneg(f) ((f) < 0.0)
#define is_fge1(f) ((f) >= 1.0)

#define f_fits_in_bits(f, n)\
  ((f) >= -2.0 * (1L << ((n) - 2)) && (f) < 2.0 * (1L << ((n) - 2)))
#define f_fits_in_ubits(f, n)\
  ((f) >= 0 && (f) < 4.0 * (1L << ((n) - 2)))
  
#define small_exact_log2(n)\
 ((uint)(05637042010L >> ((((n) % 11) - 1) * 3)) & 7)

#define f_fits_in_fixed(f) f_fits_in_bits(f, fixed_int_bits)

#define ROUND_DOWN(value, modulus)\
  ( (modulus) & ((modulus) - 1) ?	\
    (value) - (value) % (modulus) :\
    (value) & -(modulus) )
#define ROUND_UP(value, modulus)\
  ( (modulus) & ((modulus) - 1) ?	\
    ((value) + ((modulus) - 1)) / (modulus) * (modulus) :\
    ((value) + ((modulus) - 1)) & -(modulus) )

#define max_fixed max_long
#define min_fixed min_long

#define FORCE_UNIT(p) (p <= 0.0 ? 0.0 : p >= 1.0 ? 1.0 : p)

#define fixed_0 0L
#define fixed_epsilon 1L

#define _fixed_shift 12
#define fixed_fraction_bits _fixed_shift
#define fixed_int_bits (sizeof(long) * 8 - _fixed_shift)
#define fixed_scale (1<<_fixed_shift)
#define _fixed_rshift(x) arith_rshift(x,_fixed_shift)
#define _fixed_round_v (fixed_scale>>1)
#define _fixed_fraction_v (fixed_scale-1)

#define _fixed_pixround_v (_fixed_round_v - fixed_epsilon)

#define int2fixed(i) ((long)(i)<<_fixed_shift)

#define fixed_1 (fixed_epsilon << _fixed_shift)
#define fixed_half (fixed_1 >> 1)

#define fixed2int(x) ((int)_fixed_rshift(x))
#define fixed2int_rounded(x) ((int)_fixed_rshift((x)+_fixed_round_v))
#define fixed2int_ceiling(x) ((int)_fixed_rshift((x)+_fixed_fraction_v))
#define fixed_pre_pixround(x) ((x)+_fixed_pixround_v)
#define fixed2int_pixround(x) fixed2int(fixed_pre_pixround(x))
#define fixed_is_int(x) !((x)&_fixed_fraction_v)

#ifdef __MWERKS__
#define offset_of_(type, memb)\
 ((int) &((type *) 0)->memb)
#else
#define offset_of_(type, memb)\
 ((int) ( (char *)&((type *)0)->memb - (char *)((type *)0) ))
#endif

#if arch_ints_are_short & !arch_is_big_endian
/* Do some of the shifting and extraction ourselves. */
#  define _fixed_hi(x) *((const uint *)&(x)+1)
#  define _fixed_lo(x) *((const uint *)&(x))
#  define fixed2int_var(x)\
	((int)((_fixed_hi(x) << (16-_fixed_shift)) +\
	       (_fixed_lo(x) >> _fixed_shift)))
#  define fixed2int_var_rounded(x)\
	((int)((_fixed_hi(x) << (16-_fixed_shift)) +\
	       (((_fixed_lo(x) >> (_fixed_shift-1))+1)>>1)))
#  define fixed2int_var_ceiling(x)\
	(fixed2int_var(x) -\
	 arith_rshift((int)-(_fixed_lo(x) & _fixed_fraction_v), _fixed_shift))
#else
/* Use reasonable definitions. */
#  define fixed2int_var(x) fixed2int(x)
#  define fixed2int_var_rounded(x) fixed2int_rounded(x)
#  define fixed2int_var_ceiling(x) fixed2int_ceiling(x)
#endif
#define fixed2int_var_pixround(x) fixed2int_pixround(x)
#define fixed2long(x) ((long)_fixed_rshift(x))
#define fixed2long_rounded(x) ((long)_fixed_rshift((x)+_fixed_round_v))
#define fixed2long_ceiling(x) ((long)_fixed_rshift((x)+_fixed_fraction_v))
#define fixed2long_pixround(x) ((long)_fixed_rshift((x)+_fixed_pixround_v))
#define float2fixed(f) ((long)((f)*(float)fixed_scale))

#define fixed2float(x) ((x)*(1.0/fixed_scale))

#define fixed_floor(x) ((x)&(-1L<<_fixed_shift))
#define fixed_rounded(x) (((x)+_fixed_round_v)&(-1L<<_fixed_shift))
#define fixed_ceiling(x) (((x)+_fixed_fraction_v)&(-1L<<_fixed_shift))
#define fixed_pixround(x) (((x)+_fixed_pixround_v)&(-1L<<_fixed_shift))
#define fixed_fraction(x) ((int)(x)&_fixed_fraction_v)

#define fixed_truncated(x) ((x) < 0 ? fixed_ceiling(x) : fixed_floor(x))
	
#define max_int_in_fixed max_int
#define min_int_in_fixed min_int

#if USE_FPU_FIXED
# define set_float2fixed_vars(vr,vf)\
    (sizeof(vf) == sizeof(float) ?\
     set_float2fixed_(&vr, *(const long *)&vf, fixed_fraction_bits) :\
     set_double2fixed_(&vr, ((const ulong *)&vf)[arch_is_big_endian],\
		       ((const long *)&vf)[1 - arch_is_big_endian],\
		       fixed_fraction_bits))
# define set_fixed2float_var(vf,x)\
    (sizeof(vf) == sizeof(float) ?\
     (*(long *)&vf = fixed2float_(x, fixed_fraction_bits), 0) :\
     (set_fixed2double_((double *)&vf, x, fixed_fraction_bits), 0))
#define set_ldexp_fixed2double(vd, x, exp)\
  set_fixed2double_(&vd, x, -(exp))
#else
#define set_float2fixed_vars(vr,vf)\
    (f_fits_in_bits(vf, fixed_int_bits) ? (vr = float2fixed(vf), 0) :\
     -13)
#define set_fixed2float_var(vf,x)\
    (vf = fixed2float(x), 0)
#endif

#if USE_FPU_FIXED && arch_sizeof_short == 2
#define CHECK_FMUL2FIXED_VARS(vr, vfa, vfb, dtemp)\
  set_fmul2fixed_(&vr, *(const long *)&vfa, *(const long *)&vfb)
#define FINISH_FMUL2FIXED_VARS(vr, dtemp)
#  if arch_is_big_endian
#  define CHECK_DFMUL2FIXED_VARS(vr, vda, vfb, dtemp)\
     set_dfmul2fixed_(&vr, ((const ulong *)&vda)[1], *(const long *)&vfb, *(const long *)&vda)
#  else
#  define CHECK_DFMUL2FIXED_VARS(vr, vda, vfb, dtemp)\
     set_dfmul2fixed_(&vr, *(const ulong *)&vda, *(const long *)&vfb, ((const long *)&vda)[1])
#  endif
#define FINISH_DFMUL2FIXED_VARS(vr, dtemp)
#else
#define CHECK_FMUL2FIXED_VARS(vr, vfa, vfb, dtemp)\
  (dtemp = (vfa) * (vfb),\
   (f_fits_in_bits(dtemp, fixed_int_bits) ? 0 :\
    (int)(XWPSError::LimitCheck)))
#define FINISH_FMUL2FIXED_VARS(vr, dtemp)\
  vr = float2fixed(dtemp)
#define CHECK_DFMUL2FIXED_VARS(vr, vda, vfb, dtemp)\
  CHECK_FMUL2FIXED_VARS(vr, vda, vfb, dtemp)
#define FINISH_DFMUL2FIXED_VARS(vr, dtemp)\
  FINISH_FMUL2FIXED_VARS(vr, dtemp)
#endif

#define frac_bits 15
#define frac_0 ((short)0)
#define frac_1_0bits 3
#define frac_1 ((short)0x7ff8)
#define frac_1_long ((long)frac_1)
#define frac_1_float ((float)frac_1)
#define frac2float(fr) ((fr) / frac_1_float)
#define float2frac(fl) ((short)(((fl) + 0.5 / frac_1_float) * frac_1_float))
#define _frac2s(fr)\
  (((fr) >> (frac_bits - frac_1_0bits)) + (fr))
#define frac2bits(fr, nb)\
  ((uint)(_frac2s(fr) >> (frac_bits - (nb))))
#define frac2byte(fr) ((uchar)frac2bits(fr, 8))

#define bits2frac(v, nb) ((short)(\
  ((short)(v) << (frac_bits - (nb))) +\
   ((v) >> ((nb) * 2 - frac_bits)) -\
   ((v) >> ((nb) - frac_1_0bits)) ))
#define byte2frac(b) bits2frac(b, 8)

#define frac2bits_floor(fr, nb)\
  ((uint)((_frac2s(fr) - (_frac2s(fr) >> (nb))) >> (frac_bits - (nb))))
  
#define ushort_bits (arch_sizeof_short * 8)
#define frac2ushort(fr) ((ushort)(\
  ((fr) << (ushort_bits - frac_bits)) +\
  ((fr) >> (frac_bits * 2 - ushort_bits - frac_1_0bits)) ))
#define ushort2frac(us) ((short)(\
  ((us) >> (ushort_bits - frac_bits)) -\
  ((us) >> (ushort_bits - frac_1_0bits)) ))
  
#define frac_1_quo(p)\
  ( (((p) >> frac_1_0bits) + ((p) >> frac_bits) + 1) >> (frac_bits - frac_1_0bits) )
  
#define frac_1_rem(p, q)\
  ((short)( (uint)(p) - ((q) << frac_bits) + ((q) << frac_1_0bits) ))
	

#define ps_color_value_bits (sizeof(ushort) * 8)
#define ps_max_color_value ((ushort)((1L << ps_color_value_bits) - 1))
#define ps_color_value_to_byte(cv)\
  ((cv) >> (ps_color_value_bits - 8))
#define ps_color_value_from_byte(cb)\
  (((cb) << (ps_color_value_bits - 8)) + ((cb) >> (16 - ps_color_value_bits)))
  
#define frac2cv(fr) frac2ushort(fr)
#define cv2frac(cv) ushort2frac(cv)

#define ps_no_color_index_value (-1)
#define ps_no_color_index ((unsigned long)ps_no_color_index_value)
#define ps_color_select_count 2

#define frac_color_(f, maxv)\
  (ushort)(((f) * (0xffffL * 2) + maxv) / (maxv * 2))
extern const ushort *const fc_color_quo[8];

#define fractional_color(f, maxv)\
  ((maxv) <= 7 ? fc_color_quo[maxv][f] : frac_color_(f, maxv))

#ifndef CIE_LOG2_CACHE_SIZE
#  define CIE_LOG2_CACHE_SIZE 9
#endif

#if USE_FPU < 0
#  define CIE_CACHE_USE_FIXED
#endif

#define CIE_FIXED_FRACTION_BITS 12

#define CIE_CACHE_INTERPOLATE

#define CIE_RENDER_TABLE_INTERPOLATE

#define ps_cie_log2_cache_size CIE_LOG2_CACHE_SIZE
#define ps_cie_cache_size (1 << ps_cie_log2_cache_size)


#define crypt_charstring_seed 4330

#define crypt_c1 ((ushort)52845)
#define crypt_c2 ((ushort)22719)
#define crypt_c1_inverse ((ushort)27493)
#define encrypt_next(ch, state, chvar)\
  (chvar = ((ch) ^ (state >> 8)),\
   state = (chvar + state) * crypt_c1 + crypt_c2)
#define decrypt_this(ch, state)\
  ((ch) ^ (state >> 8))
#define decrypt_next(ch, state, chvar)\
  (chvar = decrypt_this(ch, state),\
   decrypt_skip_next(ch, state))
#define decrypt_skip_next(ch, state)\
  (state = ((ch) + state) * crypt_c1 + crypt_c2)
#define decrypt_skip_previous(ch, state)\
  (state = (state - crypt_c2) * crypt_c1_inverse - (ch))
  
#define dda_init_state(dstate, init, N)\
  (dstate).Q = (init), (dstate).R = (N)
#define dda_init_step(dstep, D, N)\
  if ( (N) == 0 )\
    (dstep).dQ = 0, (dstep).dR = 0;\
  else if ( (D) < 0 )\
   { (dstep).dQ = -(-(D) / (N));\
     if ( ((dstep).dR = -(D) % (N)) != 0 )\
       --(dstep).dQ, (dstep).dR = (N) - (dstep).dR;\
   }\
  else\
   { (dstep).dQ = (D) / (N); (dstep).dR = (D) % (N); }\
  (dstep).NdR = (N) - (dstep).dR
#define dda_init(dda, init, D, N)\
  dda_init_state((dda).state, init, N);\
  dda_init_step((dda).step, D, N)
  
#define dda_step_add(tostep, fromstep)\
  (tostep).dQ +=\
    ((tostep).dR < (fromstep).NdR ?\
     ((tostep).dR += (fromstep).dR, (tostep).NdR -= (fromstep).dR,\
      (fromstep).dQ) :\
     ((tostep).dR -= (fromstep).NdR, (tostep).NdR += (fromstep).NdR,\
      (fromstep).dQ + 1))
      
#define dda_state_current(dstate) (dstate).Q
#define dda_current(dda) dda_state_current((dda).state)
#define dda_current_fixed2int(dda)\
  fixed2int_var(dda_state_current((dda).state))
  
#define dda_state_next(dstate, dstep)\
  (dstate).Q +=\
    ((dstate).R > (dstep).dR ?\
     ((dstate).R -= (dstep).dR, (dstep).dQ) :\
     ((dstate).R += (dstep).NdR, (dstep).dQ + 1))
#define dda_next(dda) dda_state_next((dda).state, (dda).step)

#define dda_state_previous(dstate, dstep)\
  (dstate).Q -=\
    ((dstate).R <= (dstep).NdR ?\
     ((dstate).R += (dstep).dR, (dstep).dQ) :\
     ((dstate).R -= (dstep).NdR, (dstep).dQ + 1))
#define dda_previous(dda) dda_state_previous((dda).state, (dda).step)

#define dda_state_advance(dstate, dstep, nsteps)\
    {uint n_ = (nsteps);\
    (dstate).Q += (dstep).dQ * (nsteps);\
    while ( n_-- )\
      if ( (dstate).R > (dstep).dR ) (dstate).R -= (dstep).dR;\
      else (dstate).R += (dstep).NdR, (dstate).Q++;}
      
#define dda_advance(dda, nsteps)\
  dda_state_advance((dda).state, (dda).step, nsteps)
  
#define dda_state_translate(dstate, delta)\
  ((dstate).Q += (delta))
#define dda_translate(dda, delta)\
  dda_state_translate((dda).state, delta)
  

#define fit_fill_xy(x, y, w, h)\
	if ( (x | y) < 0 ) {\
	  if ( x < 0 )\
	    w += x, x = 0;\
	  if ( y < 0 )\
	    h += y, y = 0;\
	}
	

#define fit_fill_y(y, h)\
	if ( y < 0 )\
	  h += y, y = 0;

int bytes_compare(const uchar * s1, uint len1, const uchar * s2, uint len2);

int ps_type1_encrypt(uchar * dest, const uchar * src, uint len, ushort * pstate);
int ps_type1_decrypt(uchar * dest, const uchar * src, uint len, ushort * pstate);

int psutil_init();

ulong get_u32_msb(const uchar *p);

int igcd(int x, int y);
int ilog2(int n);
int imod(int m, int n);

long fixed_mult_quo(long signed_A, long B, long C);
long fixed_mult_rem(long a, long b, long c);

#define no_UniqueID max_long

void bytes_fill_rectangle(uchar * dest, 
                          uint raster,
		                      uchar value, 
		                      int width_bytes, 
		                      int height);
void bytes_copy_rectangle(uchar * dest, 
                          uint dest_raster,
	     						        const uchar * src, 
	     						        uint src_raster, 
	     						        int width_bytes, 
	     						        int height);

int image_flip_planes(uchar * buffer, 
                      const uchar ** planes, 
                      int offset, 
                      int nbytes,
		                  int num_planes, 
		                  int bits_per_sample);
		                  
double ps_cos_degrees(double ang);
double ps_sin_degrees(double ang);
int ps_atan2_degrees(double y, double x, double *pangle);

int set_dfmul2fixed_(long * pr, ulong xalo, long b, long xahi);
int set_fmul2fixed_(long *pr, long a, long b);

int set_double2fixed_(long * pr, ulong lo, long hi, int fracbits);
int set_float2fixed_(long * pr, long vf, int fracbits);
long fixed2float_(long x, int fracbits);
void set_fixed2double_(double *pd, long x, int fracbits);

struct PSParamItem
{
	const char *key;
  uchar type;
  int offset;
};

struct PSString
{
	uchar * data;
	uint   size;
};

struct PSConstString
{
	const uchar * data;
	uint   size;
};

struct PSByteString
{
	uchar * data;
	uint   size;
	uchar *bytes;
};

struct PSConstByteString
{
	const uchar * data;
	uint   size;
	const uchar *bytes;
};

struct PSDDAStateFixed
{
	long Q; 
	uint R;
};

struct PSDDAStepFixed
{
	long dQ; 
	uint dR, NdR;
};

struct PSDDAFixed
{
	PSDDAStateFixed state;
	PSDDAStepFixed step;
};

struct PSDDAFixedPoint
{
	PSDDAFixed x, y;
};

struct PSDDAInt
{
	struct ia_
	{
		int Q; 
		uint R;
	} state;
	
	struct ie_
	{
		int dQ; 
		uint dR, NdR;
	} step;
};

enum PSScaleCase {
    SCALE_SAME = 0,
    SCALE_SAME_ALIGNED,
    SCALE_8_8,
    SCALE_8_8_ALIGNED,
    SCALE_8_16_BYTE2FRAC,
    SCALE_8_16_BYTE2FRAC_ALIGNED,
    SCALE_8_16_BYTE2FRAC_3,
    SCALE_8_16_BYTE2FRAC_3_ALIGNED,
    SCALE_8_16_GENERAL,
    SCALE_8_16_GENERAL_ALIGNED,
    SCALE_16_8,
    SCALE_16_8_ALIGNED,
    SCALE_16_16,
    SCALE_16_16_ALIGNED
} ;


enum PSRop2
{
    rop2_0 = 0,
    rop2_S = 0xc,	
#define rop2_S_shift 2
    rop2_D = 0xa,	
#define rop2_D_shift 1
    rop2_1 = 0xf,
#define rop2_operand(shift, d, s)\
  ((shift) == 2 ? (s) : (d))
    rop2_default = rop2_S
};

enum PSRop3
{
    rop3_0 = 0,
    rop3_T = 0xf0,
#define rop3_T_shift 4
    rop3_S = 0xcc,	
#define rop3_S_shift 2
    rop3_D = 0xaa,	
#define rop3_D_shift 1
    rop3_1 = 0xff,
    rop3_default = rop3_T | rop3_S
};

enum PSRopUsage
{
    rop_usage_none = 0,
    rop_usage_D = 1,
    rop_usage_S = 2,
    rop_usage_DS = 3,
    rop_usage_T = 4,
    rop_usage_DT = 5,
    rop_usage_ST = 6,
    rop_usage_DST = 7
};

#define rop3_invert_(op, mask, shift)\
  ( (((op) & mask) >> shift) | (((op) & (rop3_1 - mask)) << shift) |\
    ((op) & ~rop3_1) )
#define rop3_invert_D(op) rop3_invert_(op, rop3_D, rop3_D_shift)
#define rop3_invert_S(op) rop3_invert_(op, rop3_S, rop3_S_shift)
#define rop3_invert_T(op) rop3_invert_(op, rop3_T, rop3_T_shift)

#define rop3_know_0_(op, mask, shift)\
  ( (((op) & (rop3_1 - mask)) << shift) | ((op) & ~mask) )
#define rop3_know_D_0(op) rop3_know_0_(op, rop3_D, rop3_D_shift)
#define rop3_know_S_0(op) rop3_know_0_(op, rop3_S, rop3_S_shift)
#define rop3_know_T_0(op) rop3_know_0_(op, rop3_T, rop3_T_shift)

#define rop3_know_1_(op, mask, shift)\
  ( (((op) & mask) >> shift) | ((op) & ~(rop3_1 - mask)) )
#define rop3_know_D_1(op) rop3_know_1_(op, rop3_D, rop3_D_shift)
#define rop3_know_S_1(op) rop3_know_1_(op, rop3_S, rop3_S_shift)
#define rop3_know_T_1(op) rop3_know_1_(op, rop3_T, rop3_T_shift)

#define rop3_swap_S_T(op)\
  ( (((op) & rop3_S & ~rop3_T) << (rop3_T_shift - rop3_S_shift)) |\
    (((op) & ~rop3_S & rop3_T) >> (rop3_T_shift - rop3_S_shift)) |\
    ((op) & (~rop3_1 | (rop3_S ^ rop3_T))) )
    
#define rop3_use_D_when_0_(op, mask)\
  (((op) & ~(rop3_1 - mask)) | (rop3_D & ~mask))
#define rop3_use_D_when_1_(op, mask)\
  (((op) & ~mask) | (rop3_D & mask))
#define rop3_use_D_when_S_0(op) rop3_use_D_when_0_(op, rop3_S)
#define rop3_use_D_when_S_1(op) rop3_use_D_when_1_(op, rop3_S)
#define rop3_use_D_when_T_0(op) rop3_use_D_when_0_(op, rop3_T)
#define rop3_use_D_when_T_1(op) rop3_use_D_when_1_(op, rop3_T)

#define rop3_not(op) ((op) ^ rop3_1)

#define rop3_uses_(op, mask, shift)\
  ( ((((op) << shift) ^ (op)) & mask) != 0 )
#define rop3_uses_D(op) rop3_uses_(op, rop3_D, rop3_D_shift)
#define rop3_uses_S(op) rop3_uses_(op, rop3_S, rop3_S_shift)
#define rop3_uses_T(op) rop3_uses_(op, rop3_T, rop3_T_shift)

#define rop3_is_idempotent(op)\
  !( (op) & ~((op) << rop3_D_shift) & rop3_D )
  
#define source_transparent_default false
#define pattern_transparent_default false

#define lop_rop(lop) ((PSRop3)((lop) & 0xff))
#define lop_S_transparent 0x100
#define lop_T_transparent 0x200
#define lop_ral_shift 10
#define lop_ral_mask 0xf

#define lop_default\
  (rop3_default |\
   (source_transparent_default ? lop_S_transparent : 0) |\
   (pattern_transparent_default ? lop_T_transparent : 0))

#ifdef TRANSPARENCY_PER_H_P	
#define lop_uses_S(lop)\
  (rop3_uses_S(lop) || ((lop) & (lop_S_transparent | lop_T_transparent)))
#else		
#define lop_uses_S(lop)\
  (rop3_uses_S(lop) || ((lop) & lop_S_transparent))
#endif

#define lop_uses_T(lop)\
  (rop3_uses_T(lop) || ((lop) & lop_T_transparent))
  
#define lop_no_T_is_S(lop)\
  (((lop) & (lop_S_transparent | (rop3_1 - rop3_T))) == (rop3_S & ~rop3_T))
  
#define lop_no_S_is_T(lop)\
  (((lop) & (lop_T_transparent | (rop3_1 - rop3_S))) == (rop3_T & ~rop3_S))
  
#define lop_is_idempotent(lop) rop3_is_idempotent(lop)

#define lop_know_S_0(lop)\
  (rop3_know_S_0(lop) & ~lop_S_transparent)
#define lop_know_T_0(lop)\
  (rop3_know_T_0(lop) & ~lop_T_transparent)
#define lop_know_S_1(lop)\
  (lop & lop_S_transparent ? rop3_D : rop3_know_S_1(lop))
#define lop_know_T_1(lop)\
  (lop & lop_T_transparent ? rop3_D : rop3_know_T_1(lop))
  	
typedef ulong rop_operand;
typedef ulong (*rop_proc)(ulong D, ulong S, ulong T);

extern const rop_proc rop_proc_table[256];
  	
extern const uchar rop_usage_table[256];

PSRop3 ps_transparent_rop(ulong lop);

void memflip8x8(const uchar * inp, int line_size, uchar * outp, int dist);

struct PSPaintColor 
{
  float values[PS_CLIENT_COLOR_MAX_COMPONENTS];
};

struct PSRopSource 
{
    const uchar *sdata;
    int sourcex;
    uint sraster;
    ulong id;
    ulong scolors[2];
    bool use_scolors;
} ;

enum PSCieRenderStatus
{
	CIE_RENDER_STATUS_BUILT,
  CIE_RENDER_STATUS_INITED,
  CIE_RENDER_STATUS_SAMPLED,
  CIE_RENDER_STATUS_COMPLETED
};

struct PSVector3
{
	float u, v, w;
};

struct PSMatrix3
{
	PSVector3 cu, cv, cw;
	bool is_identity;
};

inline void
cie_matrix_init(PSMatrix3 * mat)
{
  mat->is_identity =
	mat->cu.u == 1.0 && is_fzero2(mat->cu.v, mat->cu.w) &&
	mat->cv.v == 1.0 && is_fzero2(mat->cv.u, mat->cv.w) &&
	mat->cw.w == 1.0 && is_fzero2(mat->cw.u, mat->cw.v);
}

struct PSRange
{
	float rmin, rmax;
};

inline void
cie_restrict(float *pv, const PSRange *range)
{
  if (*pv <= range->rmin)
		*pv = range->rmin;
  else if (*pv >= range->rmax)
		*pv = range->rmax;
}


struct PSRange3
{
	PSRange ranges[3];
};

struct PSRange4
{
	PSRange ranges[4];
};

struct PSCieLinearParams
{
	bool is_linear;
  float scale, origin;
};

struct PSCieCacheParams
{
	bool is_identity;
  float base, factor;
  PSCieLinearParams linear;
};

struct PSCieCacheFloats
{
	PSCieCacheParams params;
  float values[ps_cie_cache_size];
};

struct PSCieCacheFracs
{
	PSCieCacheParams params;
  short values[ps_cie_cache_size];
};

struct PSCieCacheInts
{
	PSCieCacheParams params;
  int values[ps_cie_cache_size];
};

union PSCieScalarCache 
{
  PSCieCacheFloats floats;
  PSCieCacheFracs fracs;
  PSCieCacheInts ints;
};

struct PSCieCachedVector3 
{
  float u, v, w;
};

struct PSCieVectorCacheParams
{
  float base, factor, limit;
};

struct PSCieCacheVectors 
{
  PSCieVectorCacheParams params;
  PSCieCachedVector3 values[ps_cie_cache_size];
};

struct PSCieVectorCache 
{
  PSCieCacheFloats floats;
  PSCieCacheVectors vecs;
};

struct PSCieWB
{
	PSVector3 WhitePoint;
	PSVector3 BlackPoint;
};

struct PSCieWBSD
{
	struct 
	{
		PSVector3 xyz, pqr;
  } ws, bs, wd, bd;
};

struct PSCieFloatFixedCache 
{
  PSCieCacheFloats floats;
  union if_ 
  {
		PSCieCacheFracs fracs;
		PSCieCacheInts ints;
  } fixeds;
};

struct PSForLoopParams 
{
    double init, step, limit;
};

enum PSCieJointCachesStatus
{
    CIE_JC_STATUS_BUILT,
    CIE_JC_STATUS_INITED,
    CIE_JC_STATUS_COMPLETED
} ;

struct PSEncodeData 
{
    float lmn[ps_cie_cache_size * 3];
    float abc[ps_cie_cache_size * 3];
    float t[ps_cie_cache_size * 4];
};



	
#if JPEG_LIB_VERSION >= 61

extern const uchar natural_order[DCTSIZE2];
extern const uchar inverse_natural_order[DCTSIZE2];

#define jpeg_order(x)  natural_order[x]
#define jpeg_inverse_order(x)  inverse_natural_order[x]

#else

#define jpeg_order(x)  (x)
#define jpeg_inverse_order(x) (x)

#endif

enum PSColorSpaceIndex
{
	ps_color_space_index_DeviceGray = 0,
  ps_color_space_index_DeviceRGB,
  ps_color_space_index_DeviceCMYK,
  ps_color_space_index_DevicePixel,
  ps_color_space_index_DeviceN,
  ps_color_space_index_CIEDEFG,
  ps_color_space_index_CIEDEF,
  ps_color_space_index_CIEABC,
  ps_color_space_index_CIEA,
  ps_color_space_index_Separation,
  ps_color_space_index_Indexed,
  ps_color_space_index_Pattern
};

enum PSTransparencyStateType
{
    TRANSPARENCY_STATE_Group = 1,	/* must not be 0 */
    TRANSPARENCY_STATE_Mask
} ;

enum PSTransparencyChannelSelector
{
    TRANSPARENCY_CHANNEL_Opacity = 0,
    TRANSPARENCY_CHANNEL_Shape = 1
} ;

enum PSTransparencyMaskSubtype
{
    TRANSPARENCY_MASK_Alpha,
    TRANSPARENCY_MASK_Luminosity
} ;

enum PSBlendMode
{
	BLEND_MODE_Compatible,
  BLEND_MODE_Normal,
  BLEND_MODE_Multiply,
  BLEND_MODE_Screen,
  BLEND_MODE_Difference,
  BLEND_MODE_Darken,
  BLEND_MODE_Lighten,
  BLEND_MODE_ColorDodge,
  BLEND_MODE_ColorBurn,
  BLEND_MODE_Exclusion,
  BLEND_MODE_HardLight,
  BLEND_MODE_Overlay,
  BLEND_MODE_SoftLight,
  BLEND_MODE_Luminosity,
  BLEND_MODE_Hue,
  BLEND_MODE_Saturation,
  BLEND_MODE_Color
#define MAX_BLEND_MODE BLEND_MODE_Color
};

enum PSColorSelect
{
  ps_color_select_all = -1,
  ps_color_select_texture = 0,
  ps_color_select_source = 1
};

#define log2_transfer_map_size 8
#define transfer_map_size (1 << log2_transfer_map_size)

enum PSTransferMapType
{
	ps_tm_null,
	ps_tm_identity,
	ps_tm_frac,
	ps_tm_mapped
};

enum PSMetricsPresent
{
    metricsNone = 0,
    metricsWidthOnly = 1,
    metricsSideBearingAndWidth = 2
} ;

enum PSShadingType
{
	shading_type_Function_based = 1,
  shading_type_Axial = 2,
  shading_type_Radial = 3,
  shading_type_Free_form_Gouraud_triangle = 4,
  shading_type_Lattice_form_Gouraud_triangle = 5,
  shading_type_Coons_patch = 6,
  shading_type_Tensor_product_patch = 7
};

enum PSHalftoneType
{
	ht_type_none,
  ht_type_screen,
  ht_type_colorscreen,
  ht_type_spot,
  ht_type_threshold,	
  ht_type_threshold2,
  ht_type_multiple,
  ht_type_multiple_colorscreen,
  ht_type_client_order
};

enum PSHTSeparationName
{
	ps_ht_separation_Default,
  ps_ht_separation_Gray,
  ps_ht_separation_Red,
  ps_ht_separation_Green,
  ps_ht_separation_Blue,
  ps_ht_separation_Cyan,
  ps_ht_separation_Magenta,
  ps_ht_separation_Yellow,
  ps_ht_separation_Black
};

enum PSFunctionEffort
{
    EFFORT_EASY = 0,
    EFFORT_MODERATE = 1,
    EFFORT_ESSENTIAL = 2
} ;

enum PSCalcValueType
{
    CVT_NONE = 0,	
    CVT_BOOL,
    CVT_INT,
    CVT_FLOAT
};

struct PSCalcValue 
{
    PSCalcValueType type;
    union 
    {
			int i;
			float f;
    } value;
};

enum PSCompositeOp
{
    composite_Clear = 0,
    composite_Copy,
    composite_Sover,
    composite_Sin,
    composite_Sout,
    composite_Satop,
    composite_Dover,
    composite_Din,
    composite_Dout,
    composite_Datop,
    composite_Xor,
    composite_PlusD,
    composite_PlusL,
#define composite_last composite_PlusL
    composite_Highlight,
#define compositerect_last composite_Highlight
    composite_Dissolve	
#define composite_op_last composite_Dissolve
} ;

struct PSCompositeParams 
{
    PSCompositeOp cop;
    float delta;
    uint source_alpha;
    uint source_values[4];
};

struct PSCompositeAlphaParams 
{
    PSCompositeOp op;
    float delta;
};

#define PS_IMAGE_MAX_COLOR_COMPONENTS PS_CLIENT_COLOR_MAX_COMPONENTS
#define PS_IMAGE_MAX_COMPONENTS (PS_IMAGE_MAX_COLOR_COMPONENTS + 1)
#define ps_image_max_components PS_IMAGE_MAX_COMPONENTS
#define PS_IMAGE_MAX_PLANES (PS_IMAGE_MAX_COMPONENTS * 8)
#define ps_image_max_planes PS_IMAGE_MAX_PLANES

enum PSImageFormat
{
	ps_image_format_chunky = 0,
	ps_image_format_component_planar = 1,
	ps_image_format_bit_planar = 2
};

enum PSImageAlpha
{
	ps_image_alpha_none = 0,
	ps_image_alpha_first,
	ps_image_alpha_last
};

enum PSImagePosture
{
  image_portrait = 0,
  image_landscape,
  image_skewed
};

enum PSSampleDecoding
{
  sd_none,
  sd_lookup,
  sd_compute
};

union PSSampleLookup 
{
   ulong lookup4x1to32[16];
   ushort lookup2x2to16[16];
   uchar lookup8[256];
};

struct PSSampleMap
{
	PSSampleLookup table;
	float decode_lookup[16];
	double decode_factor;

  PSSampleDecoding decoding;
  bool inverted;
};

enum PSImage3InterleaveType
{
    interleave_chunky = 1,
    interleave_scan_lines = 2,
    interleave_separate_source = 3
} ;

struct PSImagePlane
{
  const uchar *data;
  int data_x;
  uint raster;
} ;

enum PSSegmentType
{
	s_start,
  s_line,
  s_line_close,
  s_curve
};

enum PSSegmentNotes
{
	sn_none = 0,
  sn_not_first = 1,
  sn_from_arc = 2	
};

enum PSPathRectangularType
{
	prt_none = 0,
  prt_open = 1,	
  prt_fake_closed = 2,
  prt_closed = 3
};

enum PSPathStateFlags
{
  psf_position_valid = 1,
  psf_subpath_open = 2,
  psf_is_drawing = 4,
  psf_outside_range = 8,
  psf_last_newpath = 0,
  psf_last_moveto = psf_position_valid | psf_subpath_open,
  psf_last_draw = psf_position_valid | psf_subpath_open | psf_is_drawing,
  psf_last_closepath = psf_position_valid
};

enum PSPathCopyOptions
{
	pco_none = 0,
	pco_monotonize = 1,
	pco_accurate = 2,
	pco_for_stroke = 4
};

enum PSArcAction
{
    arc_nothing,
    arc_moveto,
    arc_lineto
} ;

enum PSCPEVisit
{
	visit_left = 1,
  visit_right = 2
};

enum PSCPEState
{
	cpe_scan, cpe_left, cpe_right, cpe_close, cpe_done
};

enum PSPtCrOpCode
{
  PtCr_abs, PtCr_add, PtCr_and, PtCr_atan, PtCr_bitshift,
  PtCr_ceiling, PtCr_cos, PtCr_cvi, PtCr_cvr, PtCr_div, PtCr_exp,
  PtCr_floor, PtCr_idiv, PtCr_ln, PtCr_log, PtCr_mod, PtCr_mul,
  PtCr_neg, PtCr_not, PtCr_or, PtCr_round,
  PtCr_sin, PtCr_sqrt, PtCr_sub, PtCr_truncate, PtCr_xor,

  PtCr_eq, PtCr_ge, PtCr_gt, PtCr_le, PtCr_lt, PtCr_ne,

  PtCr_copy, PtCr_dup, PtCr_exch, PtCr_index, PtCr_pop, PtCr_roll,

  PtCr_byte, PtCr_int , PtCr_float ,
  PtCr_true, PtCr_false,

  PtCr_if, PtCr_else, PtCr_return

} ;
#define PtCr_NUM_OPS ((int)PtCr_byte)
#define PtCr_NUM_OPCODES ((int)PtCr_return + 1)

enum PSPtCrTypedOpCode
{
    PtCr_abs_int = PtCr_NUM_OPCODES,
    PtCr_add_int,
    PtCr_mul_int,
    PtCr_neg_int,
    PtCr_not_bool,
    PtCr_sub_int,
    PtCr_eq_int,
    PtCr_ge_int,
    PtCr_gt_int,
    PtCr_le_int,
    PtCr_lt_int,
    PtCr_ne_int,

    PtCr_int_to_float,
    PtCr_2nd_int_to_float,
    PtCr_int2_to_float,

    PtCr_no_op,
    PtCr_typecheck

} ;

enum PSStateCopyReason
{
	copy_for_gsave,	
  copy_for_grestore,
  copy_for_gstate,
  copy_for_setgstate,
  copy_for_copygstate,
  copy_for_currentgstate
};

#define TEXT_PROCESS_RENDER 1
#define TEXT_PROCESS_INTERVENE 2

enum PSTextCacheControl
{
	TEXT_SET_CHAR_WIDTH,
  TEXT_SET_CACHE_DEVICE,
  TEXT_SET_CACHE_DEVICE2
};

enum PSInCacheDevice
{
	CACHE_DEVICE_NONE = 0,
  CACHE_DEVICE_NOT_CACHING,
  CACHE_DEVICE_CACHING
};

enum PSCharPathMode
{
	cpm_show,
  cpm_charwidth,
  cpm_false_charpath,
  cpm_true_charpath,
  cpm_false_charboxpath,
  cpm_true_charboxpath
};

enum PSHexSyntax
{
    hex_ignore_garbage = 0,
    hex_ignore_whitespace = 1,
    hex_ignore_leading_whitespace = 2
};

enum FBitType
{
  fbit_use_outlines = 0,
  fbit_use_bitmaps = 1,
  fbit_transform_bitmaps = 2
};

#define DEFAULT_LENIV_1 4
#define DEFAULT_LENIV_2 (-1)

enum PSGlyphSpace 
{
    GLYPH_SPACE_NAME,
    GLYPH_SPACE_INDEX
};

enum PSCharCommand
{
	c_undef0 = 0,
  c_undef2 = 2,
  c_callsubr = 10,
  c_return = 11,
  c_undoc15 = 15,
  c_undef17 = 17,
  
  cx_hstem = 1,
  cx_vstem = 3,
  cx_vmoveto = 4,
  cx_rlineto = 5,
  cx_hlineto = 6,
  cx_vlineto = 7,
  cx_rrcurveto = 8,
  cx_escape = 12,	
  cx_endchar = 14,
  cx_rmoveto = 21,
  cx_hmoveto = 22,
  cx_vhcurveto = 30,
  cx_hvcurveto = 31,

  cx_num4 = 255,
  
  c1_closepath = 9,
  c1_hsbw = 13,
  
  c2_blend = 16,
  c2_hstemhm = 18,
  c2_hintmask = 19,
  c2_cntrmask = 20,
  c2_vstemhm = 23,
  c2_rcurveline = 24,
  c2_rlinecurve = 25,
  c2_vvcurveto = 26,
  c2_hhcurveto = 27,
  c2_shortint = 28,
  c2_callgsubr = 29
};

enum PSCharNumCommand
{
	c_num1 = 32,
	
	c_pos2_0 = 247,
  c_pos2_1 = 248,
  c_pos2_2 = 249,
  c_pos2_3 = 250,
  
  c_neg2_0 = 251,
  c_neg2_1 = 252,
  c_neg2_2 = 253,
  c_neg2_3 = 254
};

enum PSChar1ExtendedCommand
{
	ce1_dotsection = 0,
  ce1_vstem3 = 1,
  ce1_hstem3 = 2,
  ce1_seac = 6,
  ce1_sbw = 7,
  ce1_div = 12,
  ce1_undoc15 = 15,
    
  ce1_callothersubr = 16,
  ce1_pop = 17,
  ce1_setcurrentpoint = 33
};

enum PSChar2ExtendedCommand
{
	ce2_and = 3,
  ce2_or = 4,
  ce2_not = 5,
  ce2_store = 8,
  ce2_abs = 9,
  ce2_add = 10,
  ce2_sub = 11,
  ce2_div = 12,	
  ce2_load = 13,
  ce2_neg = 14,
  ce2_eq = 15,
  ce2_drop = 18,
  ce2_put = 20,
  ce2_get = 21,
  ce2_ifelse = 22,
  ce2_random = 23,
  ce2_mul = 24,
  ce2_sqrt = 26,
  ce2_dup = 27,
  ce2_exch = 28,
  ce2_index = 29,
  ce2_roll = 30,
  ce2_hflex = 34,
  ce2_flex = 35,
  ce2_hflex1 = 36,
  ce2_flex1 = 37
};

enum PSCodeValueType
{
    CODE_VALUE_CID,
    CODE_VALUE_GLYPH,
    CODE_VALUE_CHARS	
#define CODE_VALUE_MAX CODE_VALUE_CHARS
} ;

#define PDF_NUM_STD_FONTS 14

enum PSFontType
{
	ft_composite = 0,
  ft_encrypted = 1,
  ft_encrypted2 = 2,
  ft_user_defined = 3,
  ft_disk_based = 4,
  ft_CID_encrypted = 9,
  ft_CID_user_defined = 10,
  ft_CID_TrueType = 11,
  ft_Chameleon = 14,
  ft_CID_bitmap = 32,
  ft_TrueType = 42
};

#define FONT_IS_SERIF (1<<1)
#define FONT_IS_SYMBOLIC (1<<2)
#define FONT_IS_SCRIPT (1<<3)

#define ps_font_name_max 47
#define SUBSET_PREFIX_SIZE 7
#define MAX_PDF_FONT_NAME\
  (SUBSET_PREFIX_SIZE + ps_font_name_max + 1 + 1 + sizeof(long) * 2)
  
struct PSFontName
{
	uchar chars[ps_font_name_max + 1];
  uint size;
};

enum PSEncodingIndex
{
	ENCODING_INDEX_UNKNOWN = -1,
  ENCODING_INDEX_STANDARD = 0,
  ENCODING_INDEX_ISOLATIN1,
  ENCODING_INDEX_SYMBOL,
  ENCODING_INDEX_DINGBATS,
  ENCODING_INDEX_WINANSI,
  ENCODING_INDEX_MACROMAN,
  ENCODING_INDEX_MACEXPERT,
#define NUM_KNOWN_REAL_ENCODINGS 7
	
  ENCODING_INDEX_MACGLYPH,
  ENCODING_INDEX_ALOGLYPH,
  ENCODING_INDEX_ALXGLYPH,
  ENCODING_INDEX_CFFSTRINGS	
#define NUM_KNOWN_ENCODINGS 11
};

struct ttf_OS_2_t
{
	uchar	version[2],		/* version 1 */
				xAvgCharWidth[2],
				usWeightClass[2],
				usWidthClass[2],
				fsType[2],
				ySubscriptXSize[2],
				ySubscriptYSize[2],
				ySubscriptXOffset[2],
				ySubscriptYOffset[2],
				ySuperscriptXSize[2],
				ySuperscriptYSize[2],
				ySuperscriptXOffset[2],
				ySuperscriptYOffset[2],
				yStrikeoutSize[2],
				yStrikeoutPosition[2],
				sFamilyClass[2],
				/*panose:*/
	    	bFamilyType, bSerifStyle, bWeight, bProportion, bContrast,
	    	bStrokeVariation, bArmStyle, bLetterform, bMidline, bXHeight,
				ulUnicodeRanges[16],
				achVendID[4],
				fsSelection[2],
				usFirstCharIndex[2],
				usLastCharIndex[2],
				sTypoAscender[2],
				sTypoDescender[2],
				sTypoLineGap[2],
				usWinAscent[2],
				usWinDescent[2],
				ulCodePageRanges[8];
};

struct PSPostGlyph
{
	uchar char_index;
  uchar size;
  ushort glyph_index;
};

struct PSPost
{
	PSPostGlyph glyphs[256 + 1];
  int count, glyph_count;
  uint length;
};

struct PSAZoneTable
{
	int count;
	float values[2];
};

struct PSAStemTable
{
	int count;
	float values[2];
};

struct PSType42Mtx
{
	uint numMetrics;
	ulong offset;
	uint length;
};

enum PSFMapType
{
    fmap_8_8 = 2,
    fmap_escape = 3,
    fmap_1_7 = 4,
    fmap_9_7 = 5,
    fmap_SubsVector = 6,
    fmap_double_escape = 7,
    fmap_shift = 8,
    fmap_CMap = 9
};

enum PSLineCap
{
	ps_cap_butt = 0,
  ps_cap_round = 1,
  ps_cap_square = 2,
  ps_cap_triangle = 3
};

enum PSLineJoin
{
	ps_join_miter = 0,
  ps_join_round = 1,
  ps_join_bevel = 2,
  ps_join_none = 3,
  ps_join_triangle = 4
};

enum CDSCReturnCode
{
	CDSC_ERROR		= -1,
 	CDSC_OK		= 0,
 	CDSC_NOTDSC	 	= 1,
 	CDSC_UNKNOWNDSC	= 100,
	CDSC_PSADOBE		= 200,
 	CDSC_BEGINCOMMENTS	= 201,	/* %%BeginComments */
 	CDSC_ENDCOMMENTS	= 202,	/* %%EndComments */
 	CDSC_PAGES		= 203,	/* %%Pages: */
 	CDSC_CREATOR		= 204,	/* %%Creator: */
 	CDSC_CREATIONDATE	= 205,	/* %%CreationDate: */
 	CDSC_TITLE		= 206,	/* %%Title: */
 	CDSC_FOR		= 207,	/* %%For: */
 	CDSC_LANGUAGELEVEL	= 208,	/* %%LanguageLevel: */
 	CDSC_BOUNDINGBOX	= 209,	/* %%BoundingBox: */
 	CDSC_ORIENTATION	= 210,	/* %%Orientation: */
 	CDSC_PAGEORDER	= 211,	/* %%PageOrder: */
 	CDSC_DOCUMENTMEDIA	= 212,	/* %%DocumentMedia: */
 	CDSC_DOCUMENTPAPERSIZES    = 213,	/* %%DocumentPaperSizes: */
 	CDSC_DOCUMENTPAPERFORMS    = 214,	/* %%DocumentPaperForms: */
 	CDSC_DOCUMENTPAPERCOLORS   = 215,	/* %%DocumentPaperColors: */
 	CDSC_DOCUMENTPAPERWEIGHTS  = 216,	/* %%DocumentPaperWeights: */
 	CDSC_DOCUMENTDATA	     = 217,	/* %%DocumentData: */
 	CDSC_REQUIREMENTS	     = 218,	/* IGNORED %%Requirements: */
 	CDSC_DOCUMENTNEEDEDFONTS   = 219,	/* IGNORED %%DocumentNeededFonts: */
 	CDSC_DOCUMENTSUPPLIEDFONTS = 220,	/* IGNORED %%DocumentSuppliedFonts: */
 	CDSC_BEGINPREVIEW	= 301,	/* %%BeginPreview */
 	CDSC_ENDPREVIEW	= 302,	/* %%EndPreview */
 	CDSC_BEGINDEFAULTS	= 401,	/* %%BeginDefaults */
 	CDSC_ENDDEFAULTS	= 402,	/* %%EndDefaults */
 	CDSC_BEGINPROLOG	= 501,	/* %%BeginProlog */
 	CDSC_ENDPROLOG	= 502,	/* %%EndProlog */
 	CDSC_BEGINFONT	= 503,	/* IGNORED %%BeginFont */
 	CDSC_ENDFONT		= 504,	/* IGNORED %%EndFont */
 	CDSC_BEGINFEATURE	= 505,	/* IGNORED %%BeginFeature */
 	CDSC_ENDFEATURE	= 506,	/* IGNORED %%EndFeature */
 	CDSC_BEGINRESOURCE	= 507,	/* IGNORED %%BeginResource */
 	CDSC_ENDRESOURCE	= 508,	/* IGNORED %%EndResource */
 	CDSC_BEGINPROCSET	= 509,	/* IGNORED %%BeginProcSet */
 	CDSC_ENDPROCSET	= 510,	/* IGNORED %%EndProcSet */
 	CDSC_BEGINSETUP	= 601,	/* %%BeginSetup */
 	CDSC_ENDSETUP		= 602,	/* %%EndSetup */
 	CDSC_FEATURE		= 603,	/* IGNORED %%Feature: */
 	CDSC_PAPERCOLOR	= 604,	/* IGNORED %%PaperColor: */
 	CDSC_PAPERFORM	= 605,	/* IGNORED %%PaperForm: */
 	CDSC_PAPERWEIGHT	= 606,	/* IGNORED %%PaperWeight: */
 	CDSC_PAPERSIZE	= 607,	/* %%PaperSize: */
 	CDSC_PAGE		= 700,	/* %%Page: */
 	CDSC_PAGETRAILER	= 701,	/* IGNORED %%PageTrailer */
 	CDSC_BEGINPAGESETUP	= 702,	/* IGNORED %%BeginPageSetup */
 	CDSC_ENDPAGESETUP	= 703,	/* IGNORED %%EndPageSetup */
 	CDSC_PAGEMEDIA	= 704,	/* %%PageMedia: */  CDSC_PAGEORIENTATION	= 705,	/* %%PageOrientation: */
 	CDSC_PAGEBOUNDINGBOX	= 706,	/* %%PageBoundingBox: */
 	CDSC_INCLUDEFONT	= 707,	/* IGNORED %%IncludeFont: */
 	CDSC_VIEWERORIENTATION = 708,
 	CDSC_TRAILER		= 800,
 	CDSC_EOF		= 900	/* %%EOF */
};
	
enum CDSCPreviewType
{
	CDSC_NOPREVIEW = 0,
   CDSC_EPSI = 1,
   CDSC_TIFF = 2,
   CDSC_WMF = 3,
   CDSC_PICT = 4
};
	
enum CDSCPageOrder
{
	CDSC_ORDER_UNKNOWN = 0,
   CDSC_ASCEND = 1,
   CDSC_DESCEND = 2,
   CDSC_SPECIAL = 3
};
	
enum CDSCOrientation
{
	CDSC_ORIENT_UNKNOWN = 0,
   CDSC_PORTRAIT = 1,
   CDSC_LANDSCAPE = 2,
   CDSC_UPSIDEDOWN = 3,
   CDSC_SEASCAPE = 4,
};
	
enum PSDocumentData
{
	CDSC_DATA_UNKNOWN = 0,
   CDSC_CLEAN7BIT = 1,
   CDSC_CLEAN8BIT = 2,
   CDSC_BINARY = 3
};
	
enum PSMessageError
{
	CDSC_MESSAGE_BBOX = 0,
 	CDSC_MESSAGE_EARLY_TRAILER = 1,
 	CDSC_MESSAGE_EARLY_EOF = 2,
 	CDSC_MESSAGE_PAGE_IN_TRAILER = 3,
 	CDSC_MESSAGE_PAGE_ORDINAL = 4,
 	CDSC_MESSAGE_PAGES_WRONG = 5,
 	CDSC_MESSAGE_EPS_NO_BBOX = 6,
 	CDSC_MESSAGE_EPS_PAGES = 7,
 	CDSC_MESSAGE_NO_MEDIA = 8,
 	CDSC_MESSAGE_ATEND = 9,
 	CDSC_MESSAGE_DUP_COMMENT = 10,
 	CDSC_MESSAGE_DUP_TRAILER = 11,
 	CDSC_MESSAGE_BEGIN_END = 12,
 	CDSC_MESSAGE_BAD_SECTION = 13,
 	CDSC_MESSAGE_LONG_LINE = 14,
 	CDSC_MESSAGE_INCORRECT_USAGE = 15
};
	
enum PSMessageSeverity
{
	CDSC_ERROR_INFORM	= 0,	/* Not an error */
 	CDSC_ERROR_WARN	= 1,	/* Not a DSC error itself,  */
 	CDSC_ERROR_ERROR	= 2,
};
	
enum PSResponse
{
	CDSC_RESPONSE_OK	= 0,
 	CDSC_RESPONSE_CANCEL	= 1,
 	CDSC_RESPONSE_IGNORE_ALL = 2
};

struct CDSCBBOX
{
	int llx;
  int lly;
  int urx;
  int ury;
};

struct CDSCMEDIA
{
	const char *name;
  float width;	/* PostScript points */
  float height;
  float weight;	/* GSM */
  const char *colour;
  const char *type;
  CDSCBBOX *mediabox;
};

struct CDSCCTM
{
	float xx;
  float xy;
  float yx;
  float yy;
};

struct CDSCPAGE
{
	int ordinal;
  const char *label;
  unsigned long begin;
  unsigned long end;
  unsigned int orientation;
  const CDSCMEDIA *media;
  CDSCBBOX *bbox; 
  CDSCCTM *viewer_orientation;
};

struct CDSCDOSEPS
{
	ulong ps_begin;
  ulong ps_length;
  ulong wmf_begin;
  ulong wmf_length;
  ulong tiff_begin;
  ulong tiff_length;
  ulong checksum;
};

struct CDSCSTRING
{
	uint index;
  uint length;
  char *data;
  CDSCSTRING *next;
};

enum PSPDFResourceType
{
	resourceColorSpace,
  resourceExtGState,
  resourcePattern,
  resourceShading,
  resourceXObject,
  resourceFont,
  
  resourceCharProc,
  resourceFontDescriptor,
  resourceFunction,
  NUM_RESOURCE_TYPES
};

struct PSPDFFontName
{
	uchar chars[MAX_PDF_FONT_NAME];
  uint size;
};

struct PSPDFStandardFont 
{
  const char *fname;
  PSEncodingIndex base_encoding;
};

enum PSPDFFontEmbed
{
    FONT_EMBED_STANDARD,
    FONT_EMBED_NO,
    FONT_EMBED_YES
} ;

enum PSGraphicsObjectType
{
	go_text,
  go_graphics
};

enum PSPathType
{
	ps_path_type_none = 0,
  ps_path_type_fill = 1,
  ps_path_type_stroke = 2,
  ps_path_type_clip = 4,
  ps_path_type_winding_number = 0,
  ps_path_type_even_odd = 8,
  ps_path_type_optimize = 16,	
  ps_path_type_always_close = 32, 
  ps_path_type_rule = ps_path_type_winding_number | ps_path_type_even_odd
};

enum PSPDFContext
{
	PDF_IN_NONE,
  PDF_IN_STREAM,
  PDF_IN_TEXT,
  PDF_IN_STRING
};

enum PSPDFProcset
{
	NoMarks = 0,
  ImageB = 1,
  ImageC = 2,
  ImageI = 4,
  Text = 8
};

enum PSOrientation
{
    orient_other = 0,
    orient_portrait,
    orient_landscape
} ;

enum PSParamType
{
	ps_param_type_null, 
	ps_param_type_bool, 
	ps_param_type_int,
  ps_param_type_long, 
  ps_param_type_float,
  ps_param_type_string, 
  ps_param_type_name,
  ps_param_type_int_array, 
  ps_param_type_float_array,
  ps_param_type_string_array, 
  ps_param_type_name_array,
  ps_param_type_dict, 
  ps_param_type_dict_int_keys, 
  ps_param_type_array
};

#define ps_param_type_any ((PSParamType)-1)

enum PSParamCollectionType
{
	ps_param_collection_dict_any = 0,
  ps_param_collection_dict_int_keys = 1,
  ps_param_collection_array = 2
};

#define ps_param_policy_signal_error 0
#define ps_param_policy_ignore 1
#define ps_param_policy_consult_user 2

class PSParamString
{
public:
	PSParamString() : data(0), size(0), persistent(true) {}
	PSParamString(const uchar * d, uint s);
	PSParamString(const PSParamString & other);
	~PSParamString();
	
	void fromString(const char *str);
	
	int  packHuffTable(const JHUFF_TBL * table);
	bool pdfKeyEq(const char *str);
	int  pdfmarkScanInt(int *pvalue);
	bool pssourceOk() {return (size >= 2 && data[0] == '(' && data[size - 1] == ')');}
	
	int quantParamString(int count, 
	                     const quint16 * pvals,
		   								 float QFactor);
	
	PSParamString & operator=(const PSParamString & other);
	
public:
	uchar * data;
	uint size; 
	bool persistent;
};

bool paramStringEq(PSParamString * pcs, const char *str);

class PSParamIntArray
{
public:
	PSParamIntArray() : data(0), size(0), persistent(true) {}
	PSParamIntArray(const int * d, uint s);
	PSParamIntArray(const PSParamIntArray & other);
	~PSParamIntArray();
	
	PSParamIntArray & operator=(const PSParamIntArray & other);
	
public:
	int * data;
	uint size; 
	bool persistent;
};

class PSParamFloatArray
{
public:
	PSParamFloatArray() : data(0), size(0), persistent(true) {}
	PSParamFloatArray(const float * d, uint s);
	PSParamFloatArray(const PSParamFloatArray & other);
	~PSParamFloatArray();
	
	int quantParamArray(int count, 
	                    const quint16 * pvals,
		  								float QFactor);
	
	PSParamFloatArray & operator=(const PSParamFloatArray & other);
	
public:
	float * data;
	uint size; 
	bool persistent;
};

class PSParamStringArray
{
public:
	PSParamStringArray() : data(0), size(0), persistent(true) {}
	PSParamStringArray(const PSParamString * d, uint s);
	PSParamStringArray(const PSParamStringArray & other);
	~PSParamStringArray();
	
	bool embedListIncludes(const uchar *chars, uint size);
	
	PSParamStringArray & operator=(const PSParamStringArray & other);
	
public:
	PSParamString * data;
	uint size; 
	bool persistent;
};

class XWPSUid
{
public:
	XWPSUid();
	~XWPSUid();
	
	bool equal(const XWPSUid * puid2);
	
	void invalid() {id=no_UniqueID; xvalues=0;};
	bool isUniqueID() {return ((id & ~0xffffff) == 0);}
	bool isValid() {return (id != no_UniqueID);}
	bool isXUID() {return (id < 0);}
	
	void set(long d, long * v) {id=d;xvalues=v;}
	void setUniqueID(long d) {id=d;xvalues=0;}
	void setXUID(long * pvalues, long siz) {id = -(long)(siz); xvalues = pvalues;}
	
	uint   XUIDSize() {return ((uint)(-id));}
	long * XUIDValues() {return xvalues;}
	
public:
	long id;
  long *xvalues;
};

class XWPSStruct
{
public:
	XWPSStruct();
	virtual ~XWPSStruct();
	
	int checkType(const char * n);
	
	int decRef() { return --refCount; }
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	int  incRef() { return ++refCount; }
	
public:
	int refCount;
};

class XWPSBytes : public XWPSStruct
{
public:
	XWPSBytes();
	XWPSBytes(ushort size);
	XWPSBytes(uchar * elt);
	~XWPSBytes();
	
	int getLength();
	const char * getTypeName();
	
public:
	uchar * arr;
	uchar * ptr;
	int length;
	bool self;
};

class XWPSBytesRef : public XWPSStruct
{
public:
	XWPSBytesRef();
	XWPSBytesRef(ushort size);
	XWPSBytesRef(uchar * elt);
	~XWPSBytesRef();
	
public:
	XWPSBytes * arr;
	uchar * ptr;
};

class XWPSUints : public XWPSStruct
{
public:
	XWPSUints();
	XWPSUints(ushort size);
	XWPSUints(uint * d);
	~XWPSUints();
	
	int getLength();
	const char * getTypeName();
	
public:
	uint * refs;
	uint * ptr;
	bool self;
};

class XWPSString
{
public:
	XWPSString() : data(0), size(0), bytes(0) {}
	~XWPSString();
		
public:
	uchar *data;
	uint   size;
	XWPSBytes * bytes;
};

class XWPSSinCos
{
public:
	XWPSSinCos() : _sin(1.0),_cos(1.0),orthogonal(false) {}
		
  void sincosDegrees(double ang);
	
public:
	double _sin, _cos;
	bool orthogonal;
};

class XWPSPoint
{
public:
	XWPSPoint() : x(0),y(0){}
	
	int distanceTransform(double dx, double dy, XWPSMatrix * pmat);
	int distanceTransformInverse(double dx, double dy, XWPSMatrix * pmat);
			      
	void reset() {x =0; y=0;}
	
	int setTextDistance(XWPSPoint *ppt, XWPSMatrix *pmat);
	
	int transform(double xA, double yA, XWPSMatrix * pmat);
	int transform(double xA, double yA, XWPSMatrixFixed * pmat);
	int transformInverse(double xA, double yA, XWPSMatrix * pmat);
	
public:
	double x, y;
};

class XWPSIntPoint
{
public:
	XWPSIntPoint() : x(0),y(0){}
		
	void reset() {x =0; y=0;}
		
public:
	int x, y;
};

class XWPSLog2ScalePoint
{
public:
	XWPSLog2ScalePoint() : x(0),y(0) {}
	
	void reset() {x =0; y=0;}
	
public:
	int x, y;
};

typedef int (XWPSPoint::*point_xform)(double, double, XWPSMatrix *);

class XWPSRect
{
public:
	XWPSRect() {}
	
	int bboxTransform(XWPSMatrix * pmat, XWPSRect * pbox_out);
	int bboxTransformEither(XWPSMatrix * pmat,
		      								XWPSRect * pbox_out,
     											int (XWPSPoint::*point_xform)(double, double, XWPSMatrix *));
	int bboxTransformEitherOnly(XWPSMatrix * pmat,
			   											XWPSPoint pts[4],
     													int (XWPSPoint::*point_xform)(double, double, XWPSMatrix *));
	int bboxTransformInverse(XWPSRect * pbox_in, XWPSMatrix * pmat);
	int bboxTransformOnly(XWPSMatrix * pmat, XWPSPoint points[4]);
	
	void merge(XWPSIntRect & from);
	void merge(XWPSRect & from);
	
	int pointsBbox(XWPSPoint pts[4]);
	
	bool within(XWPSRect & outer) {return (q.y <= outer.q.y && 
		                                                q.x <= outer.q.x &&
		                                                p.y >= outer.p.y && 
		                                                p.x >= outer.p.x);}
	
public:
	XWPSPoint p, q;
};

class XWPSIntRect
{
public:
	XWPSIntRect() {}
	
	void merge(XWPSIntRect & from);
	void merge(XWPSRect & from);
	
	bool within(XWPSIntRect & outer) {return (q.y <= outer.q.y && 
		                                                q.x <= outer.q.x &&
		                                                p.y >= outer.p.y && 
		                                                p.x >= outer.p.x);}
	
public:
	XWPSIntPoint p, q;
};

class XWPSFixedPoint
{
public:
	XWPSFixedPoint() : x(0),y(0){}
		
	void adjustIfEmpty(XWPSFixedRect * pbox);
	
	int distanceTransform2fixed(XWPSMatrixFixed * pmat, double dx, double dy);
	
	void scaleExp2(int sx, int sy);
	
	int transform2fixed(XWPSMatrixFixed * pmat, double xA, double yA);
		
public:
	long x, y;
};

class XWPSFixedRect
{
public:
	XWPSFixedRect() {}
	
	void intersect(XWPSFixedRect & from);
	
	void scaleExp2(int sx, int sy);
	
	int  shadeBboxTransform2fixed(XWPSRect * rect, XWPSImagerState * pis);
	
	bool within(XWPSFixedRect & outer) {return (q.y <= outer.q.y && 
		                                                q.x <= outer.q.x &&
		                                                p.y >= outer.p.y && 
		                                                p.x >= outer.p.x);}
	
public:
	XWPSFixedPoint p, q;
};

#define MAX_LOCAL_RECTS 5

class XWPSLocalRects
{
public:
	XWPSLocalRects();
	~XWPSLocalRects();
	
public:
	XWPSRect *pr;
  uint count;
  XWPSRect rl[MAX_LOCAL_RECTS];
};

class XWPSMatrix
{
public:
	XWPSMatrix();
	XWPSMatrix(double a11, double a12, double a21, double a22, double dx, double dy) 
	           : xx(a11), xy(a12), yx(a21), yy(a22), tx(dx), ty(dy) {}
	
	bool isXXYY() {return (is_fzero2(xy, yx));}
	bool isXYYX() {return (is_fzero2(xx, yx));}
	
	void makeBitmapMatrix(int x, 
	                      int y, 
	                      int w, 
	                      int h, 
	                      int h_actual);
	int makeRotation(double ang);
	int makeScaling(double sx, double sy);
	int matrixTranslate(XWPSMatrix * pm, double dx, double dy);
	int makeTranslation(double dx, double dy);
	int matrixInvert(XWPSMatrix * pm);
	int matrixMultiply(XWPSMatrix * pm1, XWPSMatrix * pm2);
	int matrixRotate(XWPSMatrix * pm, double ang);
	int matrixScale(XWPSMatrix * pm, double sx, double sy);
	int matrixToFixedCoeff(XWPSFixedCoeff * pfc, int max_bits);
	
	void reset();
	
	XWPSMatrix & operator=(XWPSMatrix & other);
	XWPSMatrix & operator=(XWPSMatrixFixed & other);
	
public:
	double xx, xy, yx, yy, tx, ty;
};

class XWPSMatrixFixed
{
public:
	XWPSMatrixFixed();
	
	int fromMatrix(XWPSMatrix *pmat);
	
	bool isSkewed();
	bool isXXYY() {return (is_fzero2(xy, yx));}
	bool isXYYX() {return (is_fzero2(xx, yx));}
	
	int matrixMultiply(XWPSMatrix * pm1, XWPSMatrixFixed * pm2);
		
	void reset();
	
	int updateMatrixFixed(double xt, double yt);
	
	XWPSMatrixFixed & operator=(XWPSMatrixFixed & other);
	XWPSMatrixFixed & operator=(XWPSMatrix & other);
	
public:
	float xx, xy, yx, yy, tx, ty;
	long tx_fixed, ty_fixed;
  bool txy_fixed_valid;
};

class XWPSFixedCoeff
{
public:
	XWPSFixedCoeff();
	
	long mfixedXX(long v, int maxb);
	long mfixedXY(long v, int maxb);
	long mfixedYX(long v, int maxb);
	long mfixedYY(long v, int maxb);
	long mult(long value, long coeff, int maxb);
	
	void reset();
	
	XWPSFixedCoeff & operator=(XWPSFixedCoeff & other);
	
public:
	long xx, xy, yx, yy;
  int skewed;
  int shift;
  int max_bits;
  long round;
};

#define ps_line_join_max 4

class XWPSDashParams
{
public:
	XWPSDashParams();
	~XWPSDashParams();
	
	void copy(XWPSDashParams & from, bool cp = true);
	
	bool dashPatternEq(const float *stored, float scale);
	
	void reset();
	
	void setAdapt(bool a) {adapt=a;}
	int setDash(const float *patternA, uint lengthA, float offsetA);
	
public:
	float *pattern;
  uint pattern_size;
  float offset;
  bool adapt;
  float pattern_length;
  bool init_ink_on;
  int init_index;
  float init_dist_left;
};

class XWPSLineParams
{
public:
	XWPSLineParams();
	
	void copy(XWPSLineParams & from, bool cp = true);
	
	float currentLineWidth() {return (2 * half_width);}
	
	void reset();
	
	void setCurveJoin(int join) {curve_join = join;}
	int setDash(const float *patternA, uint lengthA, float offsetA);
	void setDashAdapt(bool a);
	int  setDotLength(float length, bool absolute);
	void setLineCap(PSLineCap capA) {cap=capA;}
	void setLineJoin(PSLineJoin joinA) {join=joinA;}
	void setLineWidth(double w) {half_width=w/2;}
	int  setMiterLimit(float limit);
	
public:
	float half_width;
	PSLineCap cap;
	PSLineJoin join;
	int curve_join;
	float miter_limit;
	float miter_check;
	float dot_length;
	bool dot_length_absolute;
	XWPSMatrix dot_orientation;
	XWPSDashParams dash;
};

class XWPSStringMatchParams
{
public:
	XWPSStringMatchParams() : 
		       any_substring('*'), 
		       any_char('?'), 
		       quote_next('\\'), 
		       ignore_case(false) {}
		       
	bool stringMatch(const uchar * str, uint len, const uchar * pstr, uint plen);
	
public:
	int any_substring;
  int any_char;
  int quote_next;
  bool ignore_case;
};

struct PSStreamCursorRead 
{
  uchar *ptr;
  uchar *limit;
  uchar *_skip;
} ;

struct PSStreamCursorWrite 
{
  uchar *_skip;
  uchar *ptr;
  uchar *limit;
} ;

int s_hex_process(PSStreamCursorRead * pr, 
                  PSStreamCursorWrite * pw,
	                int *odd_digit, 
	                PSHexSyntax syntax);
int stream_move(PSStreamCursorRead * pr, PSStreamCursorWrite * pw);

class XWPSStreamCursor
{
public:
	XWPSStreamCursor();
	               
	uchar * srlimit() {return r.limit;}
	uchar * srptr() {return r.ptr;}
	uchar * swptr() {return w.ptr;}
	uchar * swlimit() {return w.limit;}
	
public:
	union
	{
		PSStreamCursorRead r;
    PSStreamCursorWrite w;
	};
};

class XWPSGetBitsParams
{
public:
	XWPSGetBitsParams();
	
public:
	ulong options;
	uchar *data[32];
	int x_offset;
	uint raster;
};

class XWPSDevAntiAliasInfo
{
public:
	XWPSDevAntiAliasInfo() : text_bits(1), graphics_bits(1) {}
		
public:
  int text_bits;
  int graphics_bits;
};

class XWPSDevColorInfo 
{
public:
	XWPSDevColorInfo() : num_components(1),
		                   depth(1),
		                   max_gray(1),
		                   max_color(0),
		                   dither_grays(2),
		                   dither_colors(0) {}
		                   
public:
  int num_components;
  int depth;
  ushort max_gray;
  ushort max_color;
  ushort dither_grays;
  ushort dither_colors;
  XWPSDevAntiAliasInfo anti_alias;
};

class XWPSDevCachedColors
{
public:
	XWPSDevCachedColors() : black(0), white(0) {}
		
public:
  ulong black;
  ulong white;
};

class XWPSFixedEdge
{
public:
	XWPSFixedEdge() {}
	
	long x_at_y(long y);
	
public:
	XWPSFixedPoint start;
  XWPSFixedPoint end;
};

class XWPSPDFFontDescriptorValues
{
public:
	XWPSPDFFontDescriptorValues();
	
public:
	PSFontType FontType;
	int Ascent, CapHeight, Descent, ItalicAngle, StemV;
  XWPSIntRect FontBBox;
  uint Flags;
  int AvgWidth, Leading, MaxWidth, MissingWidth, StemH, XHeight;
};

class XWPSPDFEncodingElement
{
public:
	XWPSPDFEncodingElement() : glyph(0), data(0),size(0) {}
	
public:
	ulong glyph;
	uchar *data;
	uint   size;
};

#endif //XWPSTYPE_H
