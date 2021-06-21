/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include "XWDVIType.h"
#include "XWDVICrypt.h"
#include "XWICCP.h"


static unsigned char  nullbytes16[16] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static struct
{
  int  major;
  int  minor;
} icc_versions[] = {
  {0, 0}, /* PDF-1.0, we don't support them */
  {0, 0}, /* PDF-1.1, we don't support them */
  {0, 0}, /* PDF-1.2, we don't support them */
  {0x02, 0x10}, /* PDF-1.3 */
  {0x02, 0x20}, /* PDF-1.4 */
  {0x04, 0x00}  /* PDF-1.5 */
};

int
iccp_version_supported(int pdf_ver, int major, int minor)
{
  	if (pdf_ver < 6)
  	{
    	if (icc_versions[pdf_ver].major < major)
      		return 0;
    	else if (icc_versions[pdf_ver].major == major && icc_versions[pdf_ver].minor <  minor)
      		return 0;
    	else 
      		return 1;
  	}

  	return 0;
}

iccSig
str2iccSig(const void *s)
{
  	char * p = (char *) s;

  	return (iccSig) ((p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3]);
}

#define iccNullSig 0

void
iccp_init_iccHeader(iccHeader *icch)
{
  	icch->size       = 0;
  	icch->CMMType    = iccNullSig;
  	icch->version    = 0xFFFFFF;
  	icch->devClass   = iccNullSig;
  	icch->colorSpace = iccNullSig;
  	icch->PCS        = iccNullSig;
  	memset(icch->creationDate, 0, 12);
  	icch->acsp       = str2iccSig("ascp");
  	icch->platform   = iccNullSig;
  	memset(icch->flags, 0, 4);
  	icch->devMnfct   = iccNullSig;
  	icch->devModel   = iccNullSig;
  	memset(icch->devAttr, 0, 8);
  	icch->intent     = 0;
  	icch->illuminant.X = 0;
  	icch->illuminant.Y = 0;
  	icch->illuminant.Z = 0;
  	icch->creator      = iccNullSig;
  	memset(icch->ID, 0, 16);
}

#define check_sig(d,p,q,r,s) ((d) && (d)->sig == ((p)<<24|(q)<<16|(r)<<8|(s)))

void
init_iccbased_cdata(iccbased_cdata *cdata)
{
  	cdata->sig = ('i' << 24|'c' << 16|'c' << 8|'b');
  	memset(cdata->checksum, 0, 16);
  	cdata->colorspace = PDF_COLORSPACE_TYPE_INVALID;
  	cdata->alternate  = -1;

  	return;
}

void
release_iccbased_cdata(iccbased_cdata *cdata)
{
  	Q_ASSERT(check_sig(cdata, 'i', 'c', 'c', 'b'));

  	if (cdata)
  		free(cdata);
}

int
get_num_components_iccbased(const iccbased_cdata *cdata)
{
  	int  num_components = 0;

  	Q_ASSERT(check_sig(cdata, 'i', 'c', 'c', 'b'));

  	switch (cdata->colorspace) 
  	{
  		case PDF_COLORSPACE_TYPE_RGB:
    		num_components = 3;
    		break;
    		
  		case PDF_COLORSPACE_TYPE_CMYK:
    		num_components = 4;
    		break;
    		
  		case PDF_COLORSPACE_TYPE_GRAY:
    		num_components = 1;
    		break;
    		
  		case PDF_COLORSPACE_TYPE_CIELAB:
    		num_components = 3;
    		break;
    		
  	}

  	return num_components;
}

int
compare_iccbased(const char *ident1, 
                  const iccbased_cdata *cdata1,
		  		  const char *ident2, 
		  		  const iccbased_cdata *cdata2)
{
  	if (cdata1 && cdata2) 
  	{
    	Q_ASSERT(check_sig(cdata1, 'i', 'c', 'c', 'b'));
    	Q_ASSERT(check_sig(cdata2, 'i', 'c', 'c', 'b'));

    	if (memcmp(cdata1->checksum, nullbytes16, 16) && 
			memcmp(cdata2->checksum, nullbytes16, 16)) 
		{
      		return memcmp(cdata1->checksum, cdata2->checksum, 16);
    	}
    	
    	if (cdata1->colorspace != cdata2->colorspace) 
    	{
      		return (cdata1->colorspace - cdata2->colorspace);
    	}
  	}

  	if (ident1 && ident2)
    	return strcmp(ident1, ident2);

  	return -1;
}

#define sget_signed_long(p)  ((long)   ((p)[0] << 24|(p)[1] << 16|(p)[2] << 8|(p)[3]))
#define sget_signed_short(p) ((short)  ((p)[0] << 8|(p)[1]))
#define get_iccSig(p)        ((iccSig) ((p)[0] << 24|(p)[1] << 16|(p)[2] << 8|(p)[3]))

int
iccp_unpack_header(iccHeader *icch,
		            const void *profile, 
		            long proflen, 
		            int check_size)
{
  	if (check_size) 
  	{
    	if (!profile || proflen < 128 || proflen % 4 != 0) 
      		return -1;
  	}

  	uchar * p = (uchar *) profile;
  	uchar * endptr = p + 128;

  	icch->size = sget_signed_long(p);
  	if (check_size) 
  	{
    	if (icch->size != proflen) 
      		return -1;
  	}
  	
  	p += 4;

  	icch->CMMType    = str2iccSig(p);
  	p += 4;
  	icch->version    = sget_signed_long(p);
  	p += 4;
  	icch->devClass   = str2iccSig(p);
  	p += 4;
  	icch->colorSpace = str2iccSig(p);
  	p += 4;
  	icch->PCS        = str2iccSig(p);
  	p += 4;
  	memcpy(icch->creationDate, p, 12);
  	p += 12;
  	icch->acsp = str2iccSig(p); /* acsp */
  	if (icch->acsp != str2iccSig("acsp")) 
    	return -1;
    	
  	p += 4;
  	icch->platform = str2iccSig(p);
  	p += 4;
  	memcpy(icch->flags, p, 4);
  	p += 4;
  	icch->devMnfct = str2iccSig(p);
  	p += 4;
  	icch->devModel = str2iccSig(p);
  	p += 4;
  	memcpy(icch->devAttr,  p, 8);
  	p += 8;
  	icch->intent = (p[0] << 24)|(p[1] << 16)|(p[2] << 8)|p[3];
  	p += 4;
  	icch->illuminant.X = sget_signed_long(p);
  	p += 4;
  	icch->illuminant.Y = sget_signed_long(p);
  	p += 4;
  	icch->illuminant.Z = sget_signed_long(p);
  	p += 4;
  	icch->creator = str2iccSig(p);
  	p += 4;
  	memcpy(icch->ID, p, 16);
  	p += 16;

  	/* 28 bytes reserved - must be set to zeros */
  	for (; p < endptr; p++) 
  	{
    	if (*p != '\0') 
      		return -1;
  	}

  	return 0;
}

#define ICC_HEAD_SECT1_START  0
#define ICC_HEAD_SECT1_LENGTH 56
/* 8 bytes devAttr, 4 bytes intent */
#define ICC_HEAD_SECT2_START  68
#define ICC_HEAD_SECT2_LENGTH 16
/* 16 bytes ID (checksum) */
#define ICC_HEAD_SECT3_START  100
#define ICC_HEAD_SECT3_LENGTH 28

void
iccp_get_checksum(unsigned char *checksum, const void *profile, long proflen)
{
  	uchar * p = (uchar *) profile;
  	MD5_CONTEXT    md5;

  	MD5_init (&md5);
  	MD5_write(&md5, p + ICC_HEAD_SECT1_START, ICC_HEAD_SECT1_LENGTH);
  	MD5_write(&md5, nullbytes16, 12);
  	MD5_write(&md5, p + ICC_HEAD_SECT2_START, ICC_HEAD_SECT2_LENGTH);
  	MD5_write(&md5, nullbytes16, 16);
  	MD5_write(&md5, p + ICC_HEAD_SECT3_START, ICC_HEAD_SECT3_LENGTH);

  	/* body */
  	MD5_write(&md5, p + 128, proflen - 128);

  	MD5_final(checksum, &md5);
}
