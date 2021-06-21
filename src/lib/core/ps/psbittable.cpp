/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include "psbittable.h"

const uchar byte_reverse_bits[256] = {
    bit_table_8(0, 1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80)
};

const uchar byte_right_mask[9] = {
    0, 1, 3, 7, 0xf, 0x1f, 0x3f, 0x7f, 0xff
};

const uchar byte_count_bits[256] = {
    bit_table_8(0, 1, 1, 1, 1, 1, 1, 1, 1)
};

#define t8(n) n,n,n,n,n+1,n+1,n+2,n+11
#define r8(n) n,n,n,n,n,n,n,n
#define r16(n) r8(n),r8(n)
#define r32(n) r16(n),r16(n)
#define r64(n) r32(n),r32(n)
#define r128(n) r64(n),r64(n)
const uchar byte_bit_run_length_0[256] = {
    r128(0), r64(1), r32(2), r16(3), r8(4), t8(5)
};
const uchar byte_bit_run_length_1[256] = {
    r64(0), r32(1), r16(2), r8(3), t8(4),
    r64(0), r32(1), r16(2), r8(3), t8(4)
};
const uchar byte_bit_run_length_2[256] = {
    r32(0), r16(1), r8(2), t8(3),
    r32(0), r16(1), r8(2), t8(3),
    r32(0), r16(1), r8(2), t8(3),
    r32(0), r16(1), r8(2), t8(3)
};
const uchar byte_bit_run_length_3[256] = {
    r16(0), r8(1), t8(2), r16(0), r8(1), t8(2),
    r16(0), r8(1), t8(2), r16(0), r8(1), t8(2),
    r16(0), r8(1), t8(2), r16(0), r8(1), t8(2),
    r16(0), r8(1), t8(2), r16(0), r8(1), t8(2)
};
const uchar byte_bit_run_length_4[256] = {
    r8(0), t8(1), r8(0), t8(1), r8(0), t8(1), r8(0), t8(1),
    r8(0), t8(1), r8(0), t8(1), r8(0), t8(1), r8(0), t8(1),
    r8(0), t8(1), r8(0), t8(1), r8(0), t8(1), r8(0), t8(1),
    r8(0), t8(1), r8(0), t8(1), r8(0), t8(1), r8(0), t8(1),
};

#define rr8(a,b,c,d,e,f,g,h)\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h,\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h,\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h,\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h,\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h,\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h,\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h,\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h
const uchar byte_bit_run_length_5[256] = {
    rr8(0, 0, 0, 0, 1, 1, 2, 11)
};
const uchar byte_bit_run_length_6[256] = {
    rr8(0, 0, 1, 10, 0, 0, 1, 10)
};
const uchar byte_bit_run_length_7[256] = {
    rr8(0, 9, 0, 9, 0, 9, 0, 9)
};

const uchar *const byte_bit_run_length[8] = {
    byte_bit_run_length_0, byte_bit_run_length_1,
    byte_bit_run_length_2, byte_bit_run_length_3,
    byte_bit_run_length_4, byte_bit_run_length_5,
    byte_bit_run_length_6, byte_bit_run_length_7
};
const uchar *const byte_bit_run_length_neg[8] = {
    byte_bit_run_length_0, byte_bit_run_length_7,
    byte_bit_run_length_6, byte_bit_run_length_5,
    byte_bit_run_length_4, byte_bit_run_length_3,
    byte_bit_run_length_2, byte_bit_run_length_1
};

const uchar byte_acegbdfh_to_abcdefgh[256] = {
    bit_table_8(0, 0x80, 0x20, 0x08, 0x02, 0x40, 0x10, 0x04, 0x01)
};
