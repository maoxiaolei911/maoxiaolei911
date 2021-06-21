/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVICRYPT_H
#define XWDVICRYPT_H

#include <stdio.h>

typedef struct 
{
	unsigned long A,B,C,D;
  	unsigned long nblocks;
  	unsigned char buf[64];
  	int count;
} MD5_CONTEXT;

void MD5_init(MD5_CONTEXT *ctx);
void MD5_write(MD5_CONTEXT *ctx, unsigned char *inbuf, unsigned long inlen);
void MD5_final(unsigned char *outbuf, MD5_CONTEXT *ctx);

typedef struct 
{
	int idx_i, idx_j;
  	unsigned char sbox[256];
} ARC4_KEY;

void ARC4(ARC4_KEY *ctx, unsigned long len, const unsigned char *inbuf, unsigned char *outbuf);
void ARC4_set_key(ARC4_KEY *ctx, unsigned int keylen, const unsigned char *key);

#endif //XWDVICRYPT_H