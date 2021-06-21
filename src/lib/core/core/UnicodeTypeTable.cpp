/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include "UnicodeTypeTable.h"

struct UnicodeMapTableEntry {
  char *vector;
  char type;
};

struct UnicodeCaseTableVector {
  uint codes[256];
};

static UnicodeMapTableEntry typeTable[256] = {
  { "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNLNNNNNNNNNNLNNNNLNNNNNLLLLLLLLLLLLLLLLLLLLLLLNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNLLLLLLLL", 'X' },
  { NULL, 'L' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNLLLLLLLNNNNNNNNNNNNNNLLNNNNNNNNNNNNNNLLLLLNNNNNNNNNLNNNNNNNNNNNNNNNNN", 'X' },
  { "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNLLLLLNNNNNNNNNNNLNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNLLLLLLLLL", 'X' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL", 'X' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNRNRNNRNRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR", 'X' },
  { "RRRRNNNNNNNNNRNNNNNNNNRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNRRRNRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRNNNNNNNRNNNNNNNRRNNNNNNNRRNNNNNNNNNNRRRRRR", 'X' },
  { "RRRRRRRRRRRRRRNNRNRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRNNNNNNNNNNNNNNNNNNNNNNNNNNNRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRNNNNNNNNNNNRNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN", 'X' },
  { NULL, 'N' },
  { "NNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNLLLLNNNNNNNNLLLLNLLLNNNNLLLLLLLLLLLLLNNLLLLLLLLLLLLLNNNNNNNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNLLLLNNNNLLLLLLLLNLLLLLLLLLLLLLLLLLLLLNNLLLLLLLLLLLLLLNNLLLLLLLNNNNN", 'X' },
  { "NNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNLLLLNNNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNLLLNNNNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNLLLLNNNNNNNNLLLLNLLLLLLLLLLLLLLLLLLLLNNLLLLLLLLLLLLNNNNNNNNNNNNNNNN", 'X' },
  { "NNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNLLNLNNNLLLLLLLLLNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNLLLLLLLLLLLLNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNLLLLL", 'X' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNLLLLNNNNNNNNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNLLLLLLLLLLLLLLLNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL", 'X' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNLLLLLLLLLNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNLLLLLLLNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL", 'X' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNLLNNNNNNNNNNNNLLLLLLLNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNLLNNNNNNNNNLLLLLLLLLLNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL", 'X' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLNNLLLLLLLLLLLLLLLLLLLLLLLLLLLNLNLNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNNNNNNNNNNNLNNNNNLNNLLLLNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNLLLLLLLLLNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL", 'X' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNLNNNNNNLNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL", 'X' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL", 'X' },
  { "LLLLLLLLLLLLLLLLLLNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNLLLLLLLLNLLNNNNNNNNNNNLLLLLLLNLNLLLLLLLLLLLLNNNNNNNNNNNNNNNNNNNNNN", 'X' },
  { "NNNNNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL", 'X' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNLLLLNNNNNLLLLLLNLLLLLLNNNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN", 'X' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNLNNNLLLLLLLLLLLNNNLLLLLLLLLLLLNNNNLLLLLLLLLLLLLNNNLLLLLLLLLLLLLNNN", 'X' },
  { "NNNNNNNNNNNNNNLRNNNNNNNNNNNNNNNNNNNNNNNNNNLRNLRNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNLNNNNNNNNNNNNNLNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN", 'X' },
  { "NNLNNNNLNNLLLLLLLLLLNLNNNLLLLLNNNNNNLNLNLNLLLLNLLLNLLLLLLLNNLLLLNNNNNLLLLLNNNNNNNNNNNNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN", 'X' },
  { NULL, 'N' },
  { "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNNNNNNNNNNNNNNNNNLNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN", 'X' },
  { "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNNNNNNNNNNNNN", 'X' },
  { NULL, 'N' },
  { NULL, 'N' },
  { NULL, 'N' },
  { NULL, 'L' },
  { NULL, 'N' },
  { NULL, 'N' },
  { NULL, 'N' },
  { NULL, 'N' },
  { NULL, 'N' },
  { NULL, 'N' },
  { NULL, 'N' },
  { "NNNNNLLLNNNNNNNNNNNNNNNNNNNNNNNNNLLLLLLLLLNNNNNNNLLLLLNNLLLLLNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNLLLNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNLLLL", 'X' },
  { NULL, 'L' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNNNNNNNNNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNNNNNNLLLLLLLLLLLLNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL", 'X' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLN", 'X' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN", 'X' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL", 'X' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { NULL, 'L' },
  { "LLLLLLLLLLLLLLLLLLLLLLLLRRRRRRNRRRRRRRRRRNRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR", 'X' },
  { NULL, 'R' },
  { "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRNNRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRNNN", 'X' },
  { "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRNNN", 'X' },
  { "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNNLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNLL", 'X' }
};

static UnicodeCaseTableVector caseTable00 = {{
  0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
  0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
  0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
  0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
  0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
  0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
  0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
  0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
  0x0040, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
  0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
  0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
  0x0078, 0x0079, 0x007a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
  0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
  0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
  0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
  0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x007f,
  0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
  0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f,
  0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
  0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f,
  0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7,
  0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af,
  0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x03bc, 0x00b6, 0x00b7,
  0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf,
  0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
  0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
  0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00d7,
  0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00df,
  0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
  0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
  0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7,
  0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff
}};
static UnicodeCaseTableVector caseTable01 = {{
  0x0101, 0x0101, 0x0103, 0x0103, 0x0105, 0x0105, 0x0107, 0x0107,
  0x0109, 0x0109, 0x010b, 0x010b, 0x010d, 0x010d, 0x010f, 0x010f,
  0x0111, 0x0111, 0x0113, 0x0113, 0x0115, 0x0115, 0x0117, 0x0117,
  0x0119, 0x0119, 0x011b, 0x011b, 0x011d, 0x011d, 0x011f, 0x011f,
  0x0121, 0x0121, 0x0123, 0x0123, 0x0125, 0x0125, 0x0127, 0x0127,
  0x0129, 0x0129, 0x012b, 0x012b, 0x012d, 0x012d, 0x012f, 0x012f,
  0x0130, 0x0131, 0x0133, 0x0133, 0x0135, 0x0135, 0x0137, 0x0137,
  0x0138, 0x013a, 0x013a, 0x013c, 0x013c, 0x013e, 0x013e, 0x0140,
  0x0140, 0x0142, 0x0142, 0x0144, 0x0144, 0x0146, 0x0146, 0x0148,
  0x0148, 0x0149, 0x014b, 0x014b, 0x014d, 0x014d, 0x014f, 0x014f,
  0x0151, 0x0151, 0x0153, 0x0153, 0x0155, 0x0155, 0x0157, 0x0157,
  0x0159, 0x0159, 0x015b, 0x015b, 0x015d, 0x015d, 0x015f, 0x015f,
  0x0161, 0x0161, 0x0163, 0x0163, 0x0165, 0x0165, 0x0167, 0x0167,
  0x0169, 0x0169, 0x016b, 0x016b, 0x016d, 0x016d, 0x016f, 0x016f,
  0x0171, 0x0171, 0x0173, 0x0173, 0x0175, 0x0175, 0x0177, 0x0177,
  0x00ff, 0x017a, 0x017a, 0x017c, 0x017c, 0x017e, 0x017e, 0x0073,
  0x0180, 0x0253, 0x0183, 0x0183, 0x0185, 0x0185, 0x0254, 0x0188,
  0x0188, 0x0256, 0x0257, 0x018c, 0x018c, 0x018d, 0x01dd, 0x0259,
  0x025b, 0x0192, 0x0192, 0x0260, 0x0263, 0x0195, 0x0269, 0x0268,
  0x0199, 0x0199, 0x019a, 0x019b, 0x026f, 0x0272, 0x019e, 0x0275,
  0x01a1, 0x01a1, 0x01a3, 0x01a3, 0x01a5, 0x01a5, 0x0280, 0x01a8,
  0x01a8, 0x0283, 0x01aa, 0x01ab, 0x01ad, 0x01ad, 0x0288, 0x01b0,
  0x01b0, 0x028a, 0x028b, 0x01b4, 0x01b4, 0x01b6, 0x01b6, 0x0292,
  0x01b9, 0x01b9, 0x01ba, 0x01bb, 0x01bd, 0x01bd, 0x01be, 0x01bf,
  0x01c0, 0x01c1, 0x01c2, 0x01c3, 0x01c6, 0x01c6, 0x01c6, 0x01c9,
  0x01c9, 0x01c9, 0x01cc, 0x01cc, 0x01cc, 0x01ce, 0x01ce, 0x01d0,
  0x01d0, 0x01d2, 0x01d2, 0x01d4, 0x01d4, 0x01d6, 0x01d6, 0x01d8,
  0x01d8, 0x01da, 0x01da, 0x01dc, 0x01dc, 0x01dd, 0x01df, 0x01df,
  0x01e1, 0x01e1, 0x01e3, 0x01e3, 0x01e5, 0x01e5, 0x01e7, 0x01e7,
  0x01e9, 0x01e9, 0x01eb, 0x01eb, 0x01ed, 0x01ed, 0x01ef, 0x01ef,
  0x01f0, 0x01f3, 0x01f3, 0x01f3, 0x01f5, 0x01f5, 0x0195, 0x01bf,
  0x01f9, 0x01f9, 0x01fb, 0x01fb, 0x01fd, 0x01fd, 0x01ff, 0x01ff
}};
static UnicodeCaseTableVector caseTable02 = {{
  0x0201, 0x0201, 0x0203, 0x0203, 0x0205, 0x0205, 0x0207, 0x0207,
  0x0209, 0x0209, 0x020b, 0x020b, 0x020d, 0x020d, 0x020f, 0x020f,
  0x0211, 0x0211, 0x0213, 0x0213, 0x0215, 0x0215, 0x0217, 0x0217,
  0x0219, 0x0219, 0x021b, 0x021b, 0x021d, 0x021d, 0x021f, 0x021f,
  0x019e, 0x0221, 0x0223, 0x0223, 0x0225, 0x0225, 0x0227, 0x0227,
  0x0229, 0x0229, 0x022b, 0x022b, 0x022d, 0x022d, 0x022f, 0x022f,
  0x0231, 0x0231, 0x0233, 0x0233, 0x0234, 0x0235, 0x0236, 0x0237,
  0x0238, 0x0239, 0x023a, 0x023b, 0x023c, 0x023d, 0x023e, 0x023f,
  0x0240, 0x0241, 0x0242, 0x0243, 0x0244, 0x0245, 0x0246, 0x0247,
  0x0248, 0x0249, 0x024a, 0x024b, 0x024c, 0x024d, 0x024e, 0x024f,
  0x0250, 0x0251, 0x0252, 0x0253, 0x0254, 0x0255, 0x0256, 0x0257,
  0x0258, 0x0259, 0x025a, 0x025b, 0x025c, 0x025d, 0x025e, 0x025f,
  0x0260, 0x0261, 0x0262, 0x0263, 0x0264, 0x0265, 0x0266, 0x0267,
  0x0268, 0x0269, 0x026a, 0x026b, 0x026c, 0x026d, 0x026e, 0x026f,
  0x0270, 0x0271, 0x0272, 0x0273, 0x0274, 0x0275, 0x0276, 0x0277,
  0x0278, 0x0279, 0x027a, 0x027b, 0x027c, 0x027d, 0x027e, 0x027f,
  0x0280, 0x0281, 0x0282, 0x0283, 0x0284, 0x0285, 0x0286, 0x0287,
  0x0288, 0x0289, 0x028a, 0x028b, 0x028c, 0x028d, 0x028e, 0x028f,
  0x0290, 0x0291, 0x0292, 0x0293, 0x0294, 0x0295, 0x0296, 0x0297,
  0x0298, 0x0299, 0x029a, 0x029b, 0x029c, 0x029d, 0x029e, 0x029f,
  0x02a0, 0x02a1, 0x02a2, 0x02a3, 0x02a4, 0x02a5, 0x02a6, 0x02a7,
  0x02a8, 0x02a9, 0x02aa, 0x02ab, 0x02ac, 0x02ad, 0x02ae, 0x02af,
  0x02b0, 0x02b1, 0x02b2, 0x02b3, 0x02b4, 0x02b5, 0x02b6, 0x02b7,
  0x02b8, 0x02b9, 0x02ba, 0x02bb, 0x02bc, 0x02bd, 0x02be, 0x02bf,
  0x02c0, 0x02c1, 0x02c2, 0x02c3, 0x02c4, 0x02c5, 0x02c6, 0x02c7,
  0x02c8, 0x02c9, 0x02ca, 0x02cb, 0x02cc, 0x02cd, 0x02ce, 0x02cf,
  0x02d0, 0x02d1, 0x02d2, 0x02d3, 0x02d4, 0x02d5, 0x02d6, 0x02d7,
  0x02d8, 0x02d9, 0x02da, 0x02db, 0x02dc, 0x02dd, 0x02de, 0x02df,
  0x02e0, 0x02e1, 0x02e2, 0x02e3, 0x02e4, 0x02e5, 0x02e6, 0x02e7,
  0x02e8, 0x02e9, 0x02ea, 0x02eb, 0x02ec, 0x02ed, 0x02ee, 0x02ef,
  0x02f0, 0x02f1, 0x02f2, 0x02f3, 0x02f4, 0x02f5, 0x02f6, 0x02f7,
  0x02f8, 0x02f9, 0x02fa, 0x02fb, 0x02fc, 0x02fd, 0x02fe, 0x02ff
}};
static UnicodeCaseTableVector caseTable03 = {{
  0x0300, 0x0301, 0x0302, 0x0303, 0x0304, 0x0305, 0x0306, 0x0307,
  0x0308, 0x0309, 0x030a, 0x030b, 0x030c, 0x030d, 0x030e, 0x030f,
  0x0310, 0x0311, 0x0312, 0x0313, 0x0314, 0x0315, 0x0316, 0x0317,
  0x0318, 0x0319, 0x031a, 0x031b, 0x031c, 0x031d, 0x031e, 0x031f,
  0x0320, 0x0321, 0x0322, 0x0323, 0x0324, 0x0325, 0x0326, 0x0327,
  0x0328, 0x0329, 0x032a, 0x032b, 0x032c, 0x032d, 0x032e, 0x032f,
  0x0330, 0x0331, 0x0332, 0x0333, 0x0334, 0x0335, 0x0336, 0x0337,
  0x0338, 0x0339, 0x033a, 0x033b, 0x033c, 0x033d, 0x033e, 0x033f,
  0x0340, 0x0341, 0x0342, 0x0343, 0x0344, 0x03b9, 0x0346, 0x0347,
  0x0348, 0x0349, 0x034a, 0x034b, 0x034c, 0x034d, 0x034e, 0x034f,
  0x0350, 0x0351, 0x0352, 0x0353, 0x0354, 0x0355, 0x0356, 0x0357,
  0x0358, 0x0359, 0x035a, 0x035b, 0x035c, 0x035d, 0x035e, 0x035f,
  0x0360, 0x0361, 0x0362, 0x0363, 0x0364, 0x0365, 0x0366, 0x0367,
  0x0368, 0x0369, 0x036a, 0x036b, 0x036c, 0x036d, 0x036e, 0x036f,
  0x0370, 0x0371, 0x0372, 0x0373, 0x0374, 0x0375, 0x0376, 0x0377,
  0x0378, 0x0379, 0x037a, 0x037b, 0x037c, 0x037d, 0x037e, 0x037f,
  0x0380, 0x0381, 0x0382, 0x0383, 0x0384, 0x0385, 0x03ac, 0x0387,
  0x03ad, 0x03ae, 0x03af, 0x038b, 0x03cc, 0x038d, 0x03cd, 0x03ce,
  0x0390, 0x03b1, 0x03b2, 0x03b3, 0x03b4, 0x03b5, 0x03b6, 0x03b7,
  0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd, 0x03be, 0x03bf,
  0x03c0, 0x03c1, 0x03a2, 0x03c3, 0x03c4, 0x03c5, 0x03c6, 0x03c7,
  0x03c8, 0x03c9, 0x03ca, 0x03cb, 0x03ac, 0x03ad, 0x03ae, 0x03af,
  0x03b0, 0x03b1, 0x03b2, 0x03b3, 0x03b4, 0x03b5, 0x03b6, 0x03b7,
  0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd, 0x03be, 0x03bf,
  0x03c0, 0x03c1, 0x03c3, 0x03c3, 0x03c4, 0x03c5, 0x03c6, 0x03c7,
  0x03c8, 0x03c9, 0x03ca, 0x03cb, 0x03cc, 0x03cd, 0x03ce, 0x03cf,
  0x03b2, 0x03b8, 0x03d2, 0x03d3, 0x03d4, 0x03c6, 0x03c0, 0x03d7,
  0x03d9, 0x03d9, 0x03db, 0x03db, 0x03dd, 0x03dd, 0x03df, 0x03df,
  0x03e1, 0x03e1, 0x03e3, 0x03e3, 0x03e5, 0x03e5, 0x03e7, 0x03e7,
  0x03e9, 0x03e9, 0x03eb, 0x03eb, 0x03ed, 0x03ed, 0x03ef, 0x03ef,
  0x03ba, 0x03c1, 0x03f2, 0x03f3, 0x03b8, 0x03b5, 0x03f6, 0x03f8,
  0x03f8, 0x03f2, 0x03fb, 0x03fb, 0x03fc, 0x03fd, 0x03fe, 0x03ff
}};
static UnicodeCaseTableVector caseTable04 = {{
  0x0450, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457,
  0x0458, 0x0459, 0x045a, 0x045b, 0x045c, 0x045d, 0x045e, 0x045f,
  0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
  0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f,
  0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
  0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d, 0x044e, 0x044f,
  0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
  0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f,
  0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
  0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d, 0x044e, 0x044f,
  0x0450, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457,
  0x0458, 0x0459, 0x045a, 0x045b, 0x045c, 0x045d, 0x045e, 0x045f,
  0x0461, 0x0461, 0x0463, 0x0463, 0x0465, 0x0465, 0x0467, 0x0467,
  0x0469, 0x0469, 0x046b, 0x046b, 0x046d, 0x046d, 0x046f, 0x046f,
  0x0471, 0x0471, 0x0473, 0x0473, 0x0475, 0x0475, 0x0477, 0x0477,
  0x0479, 0x0479, 0x047b, 0x047b, 0x047d, 0x047d, 0x047f, 0x047f,
  0x0481, 0x0481, 0x0482, 0x0483, 0x0484, 0x0485, 0x0486, 0x0487,
  0x0488, 0x0489, 0x048b, 0x048b, 0x048d, 0x048d, 0x048f, 0x048f,
  0x0491, 0x0491, 0x0493, 0x0493, 0x0495, 0x0495, 0x0497, 0x0497,
  0x0499, 0x0499, 0x049b, 0x049b, 0x049d, 0x049d, 0x049f, 0x049f,
  0x04a1, 0x04a1, 0x04a3, 0x04a3, 0x04a5, 0x04a5, 0x04a7, 0x04a7,
  0x04a9, 0x04a9, 0x04ab, 0x04ab, 0x04ad, 0x04ad, 0x04af, 0x04af,
  0x04b1, 0x04b1, 0x04b3, 0x04b3, 0x04b5, 0x04b5, 0x04b7, 0x04b7,
  0x04b9, 0x04b9, 0x04bb, 0x04bb, 0x04bd, 0x04bd, 0x04bf, 0x04bf,
  0x04c0, 0x04c2, 0x04c2, 0x04c4, 0x04c4, 0x04c6, 0x04c6, 0x04c8,
  0x04c8, 0x04ca, 0x04ca, 0x04cc, 0x04cc, 0x04ce, 0x04ce, 0x04cf,
  0x04d1, 0x04d1, 0x04d3, 0x04d3, 0x04d5, 0x04d5, 0x04d7, 0x04d7,
  0x04d9, 0x04d9, 0x04db, 0x04db, 0x04dd, 0x04dd, 0x04df, 0x04df,
  0x04e1, 0x04e1, 0x04e3, 0x04e3, 0x04e5, 0x04e5, 0x04e7, 0x04e7,
  0x04e9, 0x04e9, 0x04eb, 0x04eb, 0x04ed, 0x04ed, 0x04ef, 0x04ef,
  0x04f1, 0x04f1, 0x04f3, 0x04f3, 0x04f5, 0x04f5, 0x04f6, 0x04f7,
  0x04f9, 0x04f9, 0x04fa, 0x04fb, 0x04fc, 0x04fd, 0x04fe, 0x04ff
}};
static UnicodeCaseTableVector caseTable05 = {{
  0x0501, 0x0501, 0x0503, 0x0503, 0x0505, 0x0505, 0x0507, 0x0507,
  0x0509, 0x0509, 0x050b, 0x050b, 0x050d, 0x050d, 0x050f, 0x050f,
  0x0510, 0x0511, 0x0512, 0x0513, 0x0514, 0x0515, 0x0516, 0x0517,
  0x0518, 0x0519, 0x051a, 0x051b, 0x051c, 0x051d, 0x051e, 0x051f,
  0x0520, 0x0521, 0x0522, 0x0523, 0x0524, 0x0525, 0x0526, 0x0527,
  0x0528, 0x0529, 0x052a, 0x052b, 0x052c, 0x052d, 0x052e, 0x052f,
  0x0530, 0x0561, 0x0562, 0x0563, 0x0564, 0x0565, 0x0566, 0x0567,
  0x0568, 0x0569, 0x056a, 0x056b, 0x056c, 0x056d, 0x056e, 0x056f,
  0x0570, 0x0571, 0x0572, 0x0573, 0x0574, 0x0575, 0x0576, 0x0577,
  0x0578, 0x0579, 0x057a, 0x057b, 0x057c, 0x057d, 0x057e, 0x057f,
  0x0580, 0x0581, 0x0582, 0x0583, 0x0584, 0x0585, 0x0586, 0x0557,
  0x0558, 0x0559, 0x055a, 0x055b, 0x055c, 0x055d, 0x055e, 0x055f,
  0x0560, 0x0561, 0x0562, 0x0563, 0x0564, 0x0565, 0x0566, 0x0567,
  0x0568, 0x0569, 0x056a, 0x056b, 0x056c, 0x056d, 0x056e, 0x056f,
  0x0570, 0x0571, 0x0572, 0x0573, 0x0574, 0x0575, 0x0576, 0x0577,
  0x0578, 0x0579, 0x057a, 0x057b, 0x057c, 0x057d, 0x057e, 0x057f,
  0x0580, 0x0581, 0x0582, 0x0583, 0x0584, 0x0585, 0x0586, 0x0587,
  0x0588, 0x0589, 0x058a, 0x058b, 0x058c, 0x058d, 0x058e, 0x058f,
  0x0590, 0x0591, 0x0592, 0x0593, 0x0594, 0x0595, 0x0596, 0x0597,
  0x0598, 0x0599, 0x059a, 0x059b, 0x059c, 0x059d, 0x059e, 0x059f,
  0x05a0, 0x05a1, 0x05a2, 0x05a3, 0x05a4, 0x05a5, 0x05a6, 0x05a7,
  0x05a8, 0x05a9, 0x05aa, 0x05ab, 0x05ac, 0x05ad, 0x05ae, 0x05af,
  0x05b0, 0x05b1, 0x05b2, 0x05b3, 0x05b4, 0x05b5, 0x05b6, 0x05b7,
  0x05b8, 0x05b9, 0x05ba, 0x05bb, 0x05bc, 0x05bd, 0x05be, 0x05bf,
  0x05c0, 0x05c1, 0x05c2, 0x05c3, 0x05c4, 0x05c5, 0x05c6, 0x05c7,
  0x05c8, 0x05c9, 0x05ca, 0x05cb, 0x05cc, 0x05cd, 0x05ce, 0x05cf,
  0x05d0, 0x05d1, 0x05d2, 0x05d3, 0x05d4, 0x05d5, 0x05d6, 0x05d7,
  0x05d8, 0x05d9, 0x05da, 0x05db, 0x05dc, 0x05dd, 0x05de, 0x05df,
  0x05e0, 0x05e1, 0x05e2, 0x05e3, 0x05e4, 0x05e5, 0x05e6, 0x05e7,
  0x05e8, 0x05e9, 0x05ea, 0x05eb, 0x05ec, 0x05ed, 0x05ee, 0x05ef,
  0x05f0, 0x05f1, 0x05f2, 0x05f3, 0x05f4, 0x05f5, 0x05f6, 0x05f7,
  0x05f8, 0x05f9, 0x05fa, 0x05fb, 0x05fc, 0x05fd, 0x05fe, 0x05ff
}};
static UnicodeCaseTableVector caseTable1e = {{
  0x1e01, 0x1e01, 0x1e03, 0x1e03, 0x1e05, 0x1e05, 0x1e07, 0x1e07,
  0x1e09, 0x1e09, 0x1e0b, 0x1e0b, 0x1e0d, 0x1e0d, 0x1e0f, 0x1e0f,
  0x1e11, 0x1e11, 0x1e13, 0x1e13, 0x1e15, 0x1e15, 0x1e17, 0x1e17,
  0x1e19, 0x1e19, 0x1e1b, 0x1e1b, 0x1e1d, 0x1e1d, 0x1e1f, 0x1e1f,
  0x1e21, 0x1e21, 0x1e23, 0x1e23, 0x1e25, 0x1e25, 0x1e27, 0x1e27,
  0x1e29, 0x1e29, 0x1e2b, 0x1e2b, 0x1e2d, 0x1e2d, 0x1e2f, 0x1e2f,
  0x1e31, 0x1e31, 0x1e33, 0x1e33, 0x1e35, 0x1e35, 0x1e37, 0x1e37,
  0x1e39, 0x1e39, 0x1e3b, 0x1e3b, 0x1e3d, 0x1e3d, 0x1e3f, 0x1e3f,
  0x1e41, 0x1e41, 0x1e43, 0x1e43, 0x1e45, 0x1e45, 0x1e47, 0x1e47,
  0x1e49, 0x1e49, 0x1e4b, 0x1e4b, 0x1e4d, 0x1e4d, 0x1e4f, 0x1e4f,
  0x1e51, 0x1e51, 0x1e53, 0x1e53, 0x1e55, 0x1e55, 0x1e57, 0x1e57,
  0x1e59, 0x1e59, 0x1e5b, 0x1e5b, 0x1e5d, 0x1e5d, 0x1e5f, 0x1e5f,
  0x1e61, 0x1e61, 0x1e63, 0x1e63, 0x1e65, 0x1e65, 0x1e67, 0x1e67,
  0x1e69, 0x1e69, 0x1e6b, 0x1e6b, 0x1e6d, 0x1e6d, 0x1e6f, 0x1e6f,
  0x1e71, 0x1e71, 0x1e73, 0x1e73, 0x1e75, 0x1e75, 0x1e77, 0x1e77,
  0x1e79, 0x1e79, 0x1e7b, 0x1e7b, 0x1e7d, 0x1e7d, 0x1e7f, 0x1e7f,
  0x1e81, 0x1e81, 0x1e83, 0x1e83, 0x1e85, 0x1e85, 0x1e87, 0x1e87,
  0x1e89, 0x1e89, 0x1e8b, 0x1e8b, 0x1e8d, 0x1e8d, 0x1e8f, 0x1e8f,
  0x1e91, 0x1e91, 0x1e93, 0x1e93, 0x1e95, 0x1e95, 0x1e96, 0x1e97,
  0x1e98, 0x1e99, 0x1e9a, 0x1e61, 0x1e9c, 0x1e9d, 0x1e9e, 0x1e9f,
  0x1ea1, 0x1ea1, 0x1ea3, 0x1ea3, 0x1ea5, 0x1ea5, 0x1ea7, 0x1ea7,
  0x1ea9, 0x1ea9, 0x1eab, 0x1eab, 0x1ead, 0x1ead, 0x1eaf, 0x1eaf,
  0x1eb1, 0x1eb1, 0x1eb3, 0x1eb3, 0x1eb5, 0x1eb5, 0x1eb7, 0x1eb7,
  0x1eb9, 0x1eb9, 0x1ebb, 0x1ebb, 0x1ebd, 0x1ebd, 0x1ebf, 0x1ebf,
  0x1ec1, 0x1ec1, 0x1ec3, 0x1ec3, 0x1ec5, 0x1ec5, 0x1ec7, 0x1ec7,
  0x1ec9, 0x1ec9, 0x1ecb, 0x1ecb, 0x1ecd, 0x1ecd, 0x1ecf, 0x1ecf,
  0x1ed1, 0x1ed1, 0x1ed3, 0x1ed3, 0x1ed5, 0x1ed5, 0x1ed7, 0x1ed7,
  0x1ed9, 0x1ed9, 0x1edb, 0x1edb, 0x1edd, 0x1edd, 0x1edf, 0x1edf,
  0x1ee1, 0x1ee1, 0x1ee3, 0x1ee3, 0x1ee5, 0x1ee5, 0x1ee7, 0x1ee7,
  0x1ee9, 0x1ee9, 0x1eeb, 0x1eeb, 0x1eed, 0x1eed, 0x1eef, 0x1eef,
  0x1ef1, 0x1ef1, 0x1ef3, 0x1ef3, 0x1ef5, 0x1ef5, 0x1ef7, 0x1ef7,
  0x1ef9, 0x1ef9, 0x1efa, 0x1efb, 0x1efc, 0x1efd, 0x1efe, 0x1eff
}};
static UnicodeCaseTableVector caseTable1f = {{
  0x1f00, 0x1f01, 0x1f02, 0x1f03, 0x1f04, 0x1f05, 0x1f06, 0x1f07,
  0x1f00, 0x1f01, 0x1f02, 0x1f03, 0x1f04, 0x1f05, 0x1f06, 0x1f07,
  0x1f10, 0x1f11, 0x1f12, 0x1f13, 0x1f14, 0x1f15, 0x1f16, 0x1f17,
  0x1f10, 0x1f11, 0x1f12, 0x1f13, 0x1f14, 0x1f15, 0x1f1e, 0x1f1f,
  0x1f20, 0x1f21, 0x1f22, 0x1f23, 0x1f24, 0x1f25, 0x1f26, 0x1f27,
  0x1f20, 0x1f21, 0x1f22, 0x1f23, 0x1f24, 0x1f25, 0x1f26, 0x1f27,
  0x1f30, 0x1f31, 0x1f32, 0x1f33, 0x1f34, 0x1f35, 0x1f36, 0x1f37,
  0x1f30, 0x1f31, 0x1f32, 0x1f33, 0x1f34, 0x1f35, 0x1f36, 0x1f37,
  0x1f40, 0x1f41, 0x1f42, 0x1f43, 0x1f44, 0x1f45, 0x1f46, 0x1f47,
  0x1f40, 0x1f41, 0x1f42, 0x1f43, 0x1f44, 0x1f45, 0x1f4e, 0x1f4f,
  0x1f50, 0x1f51, 0x1f52, 0x1f53, 0x1f54, 0x1f55, 0x1f56, 0x1f57,
  0x1f58, 0x1f51, 0x1f5a, 0x1f53, 0x1f5c, 0x1f55, 0x1f5e, 0x1f57,
  0x1f60, 0x1f61, 0x1f62, 0x1f63, 0x1f64, 0x1f65, 0x1f66, 0x1f67,
  0x1f60, 0x1f61, 0x1f62, 0x1f63, 0x1f64, 0x1f65, 0x1f66, 0x1f67,
  0x1f70, 0x1f71, 0x1f72, 0x1f73, 0x1f74, 0x1f75, 0x1f76, 0x1f77,
  0x1f78, 0x1f79, 0x1f7a, 0x1f7b, 0x1f7c, 0x1f7d, 0x1f7e, 0x1f7f,
  0x1f80, 0x1f81, 0x1f82, 0x1f83, 0x1f84, 0x1f85, 0x1f86, 0x1f87,
  0x1f80, 0x1f81, 0x1f82, 0x1f83, 0x1f84, 0x1f85, 0x1f86, 0x1f87,
  0x1f90, 0x1f91, 0x1f92, 0x1f93, 0x1f94, 0x1f95, 0x1f96, 0x1f97,
  0x1f90, 0x1f91, 0x1f92, 0x1f93, 0x1f94, 0x1f95, 0x1f96, 0x1f97,
  0x1fa0, 0x1fa1, 0x1fa2, 0x1fa3, 0x1fa4, 0x1fa5, 0x1fa6, 0x1fa7,
  0x1fa0, 0x1fa1, 0x1fa2, 0x1fa3, 0x1fa4, 0x1fa5, 0x1fa6, 0x1fa7,
  0x1fb0, 0x1fb1, 0x1fb2, 0x1fb3, 0x1fb4, 0x1fb5, 0x1fb6, 0x1fb7,
  0x1fb0, 0x1fb1, 0x1f70, 0x1f71, 0x1fb3, 0x1fbd, 0x03b9, 0x1fbf,
  0x1fc0, 0x1fc1, 0x1fc2, 0x1fc3, 0x1fc4, 0x1fc5, 0x1fc6, 0x1fc7,
  0x1f72, 0x1f73, 0x1f74, 0x1f75, 0x1fc3, 0x1fcd, 0x1fce, 0x1fcf,
  0x1fd0, 0x1fd1, 0x1fd2, 0x1fd3, 0x1fd4, 0x1fd5, 0x1fd6, 0x1fd7,
  0x1fd0, 0x1fd1, 0x1f76, 0x1f77, 0x1fdc, 0x1fdd, 0x1fde, 0x1fdf,
  0x1fe0, 0x1fe1, 0x1fe2, 0x1fe3, 0x1fe4, 0x1fe5, 0x1fe6, 0x1fe7,
  0x1fe0, 0x1fe1, 0x1f7a, 0x1f7b, 0x1fe5, 0x1fed, 0x1fee, 0x1fef,
  0x1ff0, 0x1ff1, 0x1ff2, 0x1ff3, 0x1ff4, 0x1ff5, 0x1ff6, 0x1ff7,
  0x1f78, 0x1f79, 0x1f7c, 0x1f7d, 0x1ff3, 0x1ffd, 0x1ffe, 0x1fff
}};
static UnicodeCaseTableVector caseTable21 = {{
  0x2100, 0x2101, 0x2102, 0x2103, 0x2104, 0x2105, 0x2106, 0x2107,
  0x2108, 0x2109, 0x210a, 0x210b, 0x210c, 0x210d, 0x210e, 0x210f,
  0x2110, 0x2111, 0x2112, 0x2113, 0x2114, 0x2115, 0x2116, 0x2117,
  0x2118, 0x2119, 0x211a, 0x211b, 0x211c, 0x211d, 0x211e, 0x211f,
  0x2120, 0x2121, 0x2122, 0x2123, 0x2124, 0x2125, 0x03c9, 0x2127,
  0x2128, 0x2129, 0x006b, 0x00e5, 0x212c, 0x212d, 0x212e, 0x212f,
  0x2130, 0x2131, 0x2132, 0x2133, 0x2134, 0x2135, 0x2136, 0x2137,
  0x2138, 0x2139, 0x213a, 0x213b, 0x213c, 0x213d, 0x213e, 0x213f,
  0x2140, 0x2141, 0x2142, 0x2143, 0x2144, 0x2145, 0x2146, 0x2147,
  0x2148, 0x2149, 0x214a, 0x214b, 0x214c, 0x214d, 0x214e, 0x214f,
  0x2150, 0x2151, 0x2152, 0x2153, 0x2154, 0x2155, 0x2156, 0x2157,
  0x2158, 0x2159, 0x215a, 0x215b, 0x215c, 0x215d, 0x215e, 0x215f,
  0x2170, 0x2171, 0x2172, 0x2173, 0x2174, 0x2175, 0x2176, 0x2177,
  0x2178, 0x2179, 0x217a, 0x217b, 0x217c, 0x217d, 0x217e, 0x217f,
  0x2170, 0x2171, 0x2172, 0x2173, 0x2174, 0x2175, 0x2176, 0x2177,
  0x2178, 0x2179, 0x217a, 0x217b, 0x217c, 0x217d, 0x217e, 0x217f,
  0x2180, 0x2181, 0x2182, 0x2183, 0x2184, 0x2185, 0x2186, 0x2187,
  0x2188, 0x2189, 0x218a, 0x218b, 0x218c, 0x218d, 0x218e, 0x218f,
  0x2190, 0x2191, 0x2192, 0x2193, 0x2194, 0x2195, 0x2196, 0x2197,
  0x2198, 0x2199, 0x219a, 0x219b, 0x219c, 0x219d, 0x219e, 0x219f,
  0x21a0, 0x21a1, 0x21a2, 0x21a3, 0x21a4, 0x21a5, 0x21a6, 0x21a7,
  0x21a8, 0x21a9, 0x21aa, 0x21ab, 0x21ac, 0x21ad, 0x21ae, 0x21af,
  0x21b0, 0x21b1, 0x21b2, 0x21b3, 0x21b4, 0x21b5, 0x21b6, 0x21b7,
  0x21b8, 0x21b9, 0x21ba, 0x21bb, 0x21bc, 0x21bd, 0x21be, 0x21bf,
  0x21c0, 0x21c1, 0x21c2, 0x21c3, 0x21c4, 0x21c5, 0x21c6, 0x21c7,
  0x21c8, 0x21c9, 0x21ca, 0x21cb, 0x21cc, 0x21cd, 0x21ce, 0x21cf,
  0x21d0, 0x21d1, 0x21d2, 0x21d3, 0x21d4, 0x21d5, 0x21d6, 0x21d7,
  0x21d8, 0x21d9, 0x21da, 0x21db, 0x21dc, 0x21dd, 0x21de, 0x21df,
  0x21e0, 0x21e1, 0x21e2, 0x21e3, 0x21e4, 0x21e5, 0x21e6, 0x21e7,
  0x21e8, 0x21e9, 0x21ea, 0x21eb, 0x21ec, 0x21ed, 0x21ee, 0x21ef,
  0x21f0, 0x21f1, 0x21f2, 0x21f3, 0x21f4, 0x21f5, 0x21f6, 0x21f7,
  0x21f8, 0x21f9, 0x21fa, 0x21fb, 0x21fc, 0x21fd, 0x21fe, 0x21ff
}};
static UnicodeCaseTableVector caseTable24 = {{
  0x2400, 0x2401, 0x2402, 0x2403, 0x2404, 0x2405, 0x2406, 0x2407,
  0x2408, 0x2409, 0x240a, 0x240b, 0x240c, 0x240d, 0x240e, 0x240f,
  0x2410, 0x2411, 0x2412, 0x2413, 0x2414, 0x2415, 0x2416, 0x2417,
  0x2418, 0x2419, 0x241a, 0x241b, 0x241c, 0x241d, 0x241e, 0x241f,
  0x2420, 0x2421, 0x2422, 0x2423, 0x2424, 0x2425, 0x2426, 0x2427,
  0x2428, 0x2429, 0x242a, 0x242b, 0x242c, 0x242d, 0x242e, 0x242f,
  0x2430, 0x2431, 0x2432, 0x2433, 0x2434, 0x2435, 0x2436, 0x2437,
  0x2438, 0x2439, 0x243a, 0x243b, 0x243c, 0x243d, 0x243e, 0x243f,
  0x2440, 0x2441, 0x2442, 0x2443, 0x2444, 0x2445, 0x2446, 0x2447,
  0x2448, 0x2449, 0x244a, 0x244b, 0x244c, 0x244d, 0x244e, 0x244f,
  0x2450, 0x2451, 0x2452, 0x2453, 0x2454, 0x2455, 0x2456, 0x2457,
  0x2458, 0x2459, 0x245a, 0x245b, 0x245c, 0x245d, 0x245e, 0x245f,
  0x2460, 0x2461, 0x2462, 0x2463, 0x2464, 0x2465, 0x2466, 0x2467,
  0x2468, 0x2469, 0x246a, 0x246b, 0x246c, 0x246d, 0x246e, 0x246f,
  0x2470, 0x2471, 0x2472, 0x2473, 0x2474, 0x2475, 0x2476, 0x2477,
  0x2478, 0x2479, 0x247a, 0x247b, 0x247c, 0x247d, 0x247e, 0x247f,
  0x2480, 0x2481, 0x2482, 0x2483, 0x2484, 0x2485, 0x2486, 0x2487,
  0x2488, 0x2489, 0x248a, 0x248b, 0x248c, 0x248d, 0x248e, 0x248f,
  0x2490, 0x2491, 0x2492, 0x2493, 0x2494, 0x2495, 0x2496, 0x2497,
  0x2498, 0x2499, 0x249a, 0x249b, 0x249c, 0x249d, 0x249e, 0x249f,
  0x24a0, 0x24a1, 0x24a2, 0x24a3, 0x24a4, 0x24a5, 0x24a6, 0x24a7,
  0x24a8, 0x24a9, 0x24aa, 0x24ab, 0x24ac, 0x24ad, 0x24ae, 0x24af,
  0x24b0, 0x24b1, 0x24b2, 0x24b3, 0x24b4, 0x24b5, 0x24d0, 0x24d1,
  0x24d2, 0x24d3, 0x24d4, 0x24d5, 0x24d6, 0x24d7, 0x24d8, 0x24d9,
  0x24da, 0x24db, 0x24dc, 0x24dd, 0x24de, 0x24df, 0x24e0, 0x24e1,
  0x24e2, 0x24e3, 0x24e4, 0x24e5, 0x24e6, 0x24e7, 0x24e8, 0x24e9,
  0x24d0, 0x24d1, 0x24d2, 0x24d3, 0x24d4, 0x24d5, 0x24d6, 0x24d7,
  0x24d8, 0x24d9, 0x24da, 0x24db, 0x24dc, 0x24dd, 0x24de, 0x24df,
  0x24e0, 0x24e1, 0x24e2, 0x24e3, 0x24e4, 0x24e5, 0x24e6, 0x24e7,
  0x24e8, 0x24e9, 0x24ea, 0x24eb, 0x24ec, 0x24ed, 0x24ee, 0x24ef,
  0x24f0, 0x24f1, 0x24f2, 0x24f3, 0x24f4, 0x24f5, 0x24f6, 0x24f7,
  0x24f8, 0x24f9, 0x24fa, 0x24fb, 0x24fc, 0x24fd, 0x24fe, 0x24ff
}};
static UnicodeCaseTableVector caseTableff = {{
  0xff00, 0xff01, 0xff02, 0xff03, 0xff04, 0xff05, 0xff06, 0xff07,
  0xff08, 0xff09, 0xff0a, 0xff0b, 0xff0c, 0xff0d, 0xff0e, 0xff0f,
  0xff10, 0xff11, 0xff12, 0xff13, 0xff14, 0xff15, 0xff16, 0xff17,
  0xff18, 0xff19, 0xff1a, 0xff1b, 0xff1c, 0xff1d, 0xff1e, 0xff1f,
  0xff20, 0xff41, 0xff42, 0xff43, 0xff44, 0xff45, 0xff46, 0xff47,
  0xff48, 0xff49, 0xff4a, 0xff4b, 0xff4c, 0xff4d, 0xff4e, 0xff4f,
  0xff50, 0xff51, 0xff52, 0xff53, 0xff54, 0xff55, 0xff56, 0xff57,
  0xff58, 0xff59, 0xff5a, 0xff3b, 0xff3c, 0xff3d, 0xff3e, 0xff3f,
  0xff40, 0xff41, 0xff42, 0xff43, 0xff44, 0xff45, 0xff46, 0xff47,
  0xff48, 0xff49, 0xff4a, 0xff4b, 0xff4c, 0xff4d, 0xff4e, 0xff4f,
  0xff50, 0xff51, 0xff52, 0xff53, 0xff54, 0xff55, 0xff56, 0xff57,
  0xff58, 0xff59, 0xff5a, 0xff5b, 0xff5c, 0xff5d, 0xff5e, 0xff5f,
  0xff60, 0xff61, 0xff62, 0xff63, 0xff64, 0xff65, 0xff66, 0xff67,
  0xff68, 0xff69, 0xff6a, 0xff6b, 0xff6c, 0xff6d, 0xff6e, 0xff6f,
  0xff70, 0xff71, 0xff72, 0xff73, 0xff74, 0xff75, 0xff76, 0xff77,
  0xff78, 0xff79, 0xff7a, 0xff7b, 0xff7c, 0xff7d, 0xff7e, 0xff7f,
  0xff80, 0xff81, 0xff82, 0xff83, 0xff84, 0xff85, 0xff86, 0xff87,
  0xff88, 0xff89, 0xff8a, 0xff8b, 0xff8c, 0xff8d, 0xff8e, 0xff8f,
  0xff90, 0xff91, 0xff92, 0xff93, 0xff94, 0xff95, 0xff96, 0xff97,
  0xff98, 0xff99, 0xff9a, 0xff9b, 0xff9c, 0xff9d, 0xff9e, 0xff9f,
  0xffa0, 0xffa1, 0xffa2, 0xffa3, 0xffa4, 0xffa5, 0xffa6, 0xffa7,
  0xffa8, 0xffa9, 0xffaa, 0xffab, 0xffac, 0xffad, 0xffae, 0xffaf,
  0xffb0, 0xffb1, 0xffb2, 0xffb3, 0xffb4, 0xffb5, 0xffb6, 0xffb7,
  0xffb8, 0xffb9, 0xffba, 0xffbb, 0xffbc, 0xffbd, 0xffbe, 0xffbf,
  0xffc0, 0xffc1, 0xffc2, 0xffc3, 0xffc4, 0xffc5, 0xffc6, 0xffc7,
  0xffc8, 0xffc9, 0xffca, 0xffcb, 0xffcc, 0xffcd, 0xffce, 0xffcf,
  0xffd0, 0xffd1, 0xffd2, 0xffd3, 0xffd4, 0xffd5, 0xffd6, 0xffd7,
  0xffd8, 0xffd9, 0xffda, 0xffdb, 0xffdc, 0xffdd, 0xffde, 0xffdf,
  0xffe0, 0xffe1, 0xffe2, 0xffe3, 0xffe4, 0xffe5, 0xffe6, 0xffe7,
  0xffe8, 0xffe9, 0xffea, 0xffeb, 0xffec, 0xffed, 0xffee, 0xffef,
  0xfff0, 0xfff1, 0xfff2, 0xfff3, 0xfff4, 0xfff5, 0xfff6, 0xfff7,
  0xfff8, 0xfff9, 0xfffa, 0xfffb, 0xfffc, 0xfffd, 0xfffe, 0xffff
}};
static UnicodeCaseTableVector *caseTable[256] = {
  &caseTable00,
  &caseTable01,
  &caseTable02,
  &caseTable03,
  &caseTable04,
  &caseTable05,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  &caseTable1e,
  &caseTable1f,
  NULL,
  &caseTable21,
  NULL,
  NULL,
  &caseTable24,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  &caseTableff
};

static inline char getType(uint c) {
  int i;
  char type;

  if (c > 0xffff) {
    type = 'X';
  } else {
    i = (c >> 8) & 0xff;
    if ((type = typeTable[i].type) == 'X') {
      type = typeTable[i].vector[c & 0xff];
    }
  }
  return type;
}

bool unicodeTypeL(uint c) {
  return getType(c) == 'L';
}

bool unicodeTypeR(uint c) {
  return getType(c) == 'R';
}

bool unicodeTypeNum(uint c) {
  return getType(c) == '#';
}

bool unicodeTypeAlphaNum(uint c) {
  char t;

  t = getType(c);
  return t == 'L' || t == 'R' || t == '#';
}

uint unicodeToUpper(uint c) {
  int i;

  if (c > 0xffff) {
    return c;
  }
  i = (c >> 8) & 0xff;
  if (caseTable[i]) {
    return caseTable[i]->codes[c & 0xff];
  }
  return c;
}
