/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef PSSCANTABLE_H
#define PSSCANTABLE_H

#include <QtGlobal>

#define max_stream_exception 4
#define min_radix 2
#define max_radix 36
#define ctype_name 100
#define ctype_btoken 101
#define ctype_space 102
#define ctype_other 103
#define ctype_exception 104

#define char_NULL 0
#define char_EOT 004
#define char_VT 013	
#define char_DOS_EOF 032

#define char_CR '\r'
#define char_EOL '\n'

#define stream_exception_repeat(x) x, x, x, x

extern const uchar scan_char_array[max_stream_exception + 256];

#define scan_char_decoder (&scan_char_array[max_stream_exception])

#endif //
