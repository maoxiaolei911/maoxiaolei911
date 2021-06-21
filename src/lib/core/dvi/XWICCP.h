/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWICCP_H
#define XWICCP_H

#include "XWGlobal.h"


#define ICC_INTENT_TYPE(n) ((int) (((n) >> 16) & 0xff))
#define ICC_INTENT_PERCEPTUAL 0
#define ICC_INTENT_RELATIVE   1
#define ICC_INTENT_SATURATION 2
#define ICC_INTENT_ABSOLUTE   3


typedef unsigned long iccSig;

struct XW_DVI_EXPORT iccXYZNumber
{
  long X, Y, Z; /* s15Fixed16Numeber */
};

struct XW_DVI_EXPORT iccHeader
{
  	long          size;
  	iccSig        CMMType;
  	long          version;
  	iccSig        devClass;
  	iccSig        colorSpace;
  	iccSig        PCS;    /* Profile Connection Space */
  	char          creationDate[12];
  	iccSig        acsp;
  	iccSig        platform;
  	char          flags[4];
  	iccSig        devMnfct;
  	iccSig        devModel;
  	char          devAttr[8];
  	long          intent;
  	iccXYZNumber  illuminant;
  	iccSig        creator;
  	unsigned char ID[16]; /* MD5 checksum with Rendering intent,
			 * Header attrs, Profile ID fields are
			 * set to zeros.
			 */
  	/* 28 bytes reserved - must be set to zeros */
};

struct XW_DVI_EXPORT iccbased_cdata
{
  	long           sig; /* 'i' 'c' 'c' 'b' */

  	unsigned char  checksum[16]; /* 16 bytes MD5 Checksum   */
  	int            colorspace;   /* input colorspace:
					*   RGB, Gray, CMYK, (Lab?)
					*/
  	int            alternate;    /* alternate colorspace (id), unused */
};

XW_DVI_EXPORT int iccp_version_supported(int pdf_ver, int major, int minor);

XW_DVI_EXPORT iccSig str2iccSig(const void *s);

XW_DVI_EXPORT void iccp_init_iccHeader(iccHeader *icch);

XW_DVI_EXPORT void init_iccbased_cdata(iccbased_cdata *cdata);

XW_DVI_EXPORT void release_iccbased_cdata(iccbased_cdata *cdata);

XW_DVI_EXPORT int get_num_components_iccbased(const iccbased_cdata *cdata);

XW_DVI_EXPORT int compare_iccbased(const char *ident1, 
                  const iccbased_cdata *cdata1,
		  		  const char *ident2, 
		  		  const iccbased_cdata *cdata2);

XW_DVI_EXPORT int iccp_unpack_header(iccHeader *icch,
		            const void *profile, 
		            long proflen, 
		            int check_size);

XW_DVI_EXPORT void iccp_get_checksum(unsigned char *checksum, 
                       const void *profile, 
                       long proflen);

#endif //XWICCP_H
