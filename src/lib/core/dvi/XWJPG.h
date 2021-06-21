/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWJPG_H
#define XWJPG_H

#include "XWGlobal.h"

class QIODevice;
class XWObject;
class XWRef;

/* JPEG Markers */
typedef enum {
  JM_SOF0  = 0xc0,
  JM_SOF1  = 0xc1,
  JM_SOF2  = 0xc2,
  JM_SOF3  = 0xc3,
  JM_SOF5  = 0xc5,
  JM_DHT   = 0xc4,
  JM_SOF6  = 0xc6,
  JM_SOF7  = 0xc7,
  JM_SOF9  = 0xc9,
  JM_SOF10 = 0xca,
  JM_SOF11 = 0xcb,
  JM_DAC   = 0xcc,
  JM_SOF13 = 0xcd,
  JM_SOF14 = 0xce,
  JM_SOF15 = 0xcf,

  JM_RST0  = 0xd0,
  JM_RST1  = 0xd1,
  JM_RST2  = 0xd2,
  JM_RST3  = 0xd3,
  JM_RST4  = 0xd4,
  JM_RST5  = 0xd5,
  JM_RST6  = 0xd6,
  JM_RST7  = 0xd7,

  JM_SOI   = 0xd8,
  JM_EOI   = 0xd9,
  JM_SOS   = 0xda,
  JM_DQT   = 0xdb,
  JM_DNL   = 0xdc,
  JM_DRI   = 0xdd,
  JM_DHP   = 0xde,
  JM_EXP   = 0xdf,

  JM_APP0  = 0xe0,
  JM_APP2  = 0xe2,
  JM_APP14 = 0xee,
  JM_APP15 = 0xef,

  JM_COM   = 0xfe
} JPEG_marker;

typedef enum {
  JS_APPn_JFIF,
  JS_APPn_ADOBE,
  JS_APPn_ICC
} JPEG_APPn_sig;

struct XW_DVI_EXPORT JPEG_APPn_JFIF  /* APP0 */
{
  unsigned short version;
  unsigned char  units;      /* 0: only aspect ratio
			      * 1: dots per inch
			      * 2: dots per cm
			      */
  unsigned short Xdensity;
  unsigned short Ydensity;
  unsigned char  Xthumbnail;
  unsigned char  Ythumbnail;
  unsigned char *thumbnail;  /* Thumbnail data. */
};

struct XW_DVI_EXPORT JPEG_APPn_ICC   /* APP2 */
{
  unsigned char  seq_id;
  unsigned char  num_chunks;
  unsigned char *chunk;

  /* Length of ICC profile data in this chunk. */
  unsigned short length;
};

struct XW_DVI_EXPORT JPEG_APPn_Adobe /* APP14 */
{
  unsigned short version;
  unsigned short flag0;
  unsigned short flag1;
  unsigned char  transform; /* color transform code */
};

struct XW_DVI_EXPORT JPEG_ext
{
  JPEG_marker   marker;
  JPEG_APPn_sig app_sig;
  void         *app_data;
};

#define MAX_COUNT 1024
struct  XW_DVI_EXPORT JPEG_info
{
  unsigned short height;
  unsigned short width;

  unsigned char  bits_per_component;
  unsigned char  num_components;

  /* Application specific extensions */
  int flags;
  int num_appn, max_appn;
  JPEG_ext *appn;

  /* Skip chunks not necessary. */
  char skipbits[MAX_COUNT / 8 + 1];
};

#define HAVE_APPn_JFIF  (1 << 0)
#define HAVE_APPn_ADOBE (1 << 1)
#define HAVE_APPn_ICC   (1 << 2)

#define IS_JFIF(j) ((j)->flags & HAVE_APPn_JFIF)

XW_DVI_EXPORT void jpeg_get_density(JPEG_info *j_info,
		         double *xdensity, 
		         double *ydensity);

XW_DVI_EXPORT void JPEG_info_init(JPEG_info *j_info);

XW_DVI_EXPORT void JPEG_release_APPn_data(JPEG_marker marker, 
                                          JPEG_APPn_sig app_sig, 
                                          void *app_data);

XW_DVI_EXPORT void JPEG_info_clear(JPEG_info *j_info);

XW_DVI_EXPORT XWObject * JPEG_get_iccp(XWRef * xref, 
                                       JPEG_info *j_info, 
                                       XWObject * icc_stream);

XW_DVI_EXPORT JPEG_marker JPEG_get_marker (QIODevice *fp);

XW_DVI_EXPORT int JPEG_copy_stream(JPEG_info *j_info, 
                 XWObject *stream, 
                 QIODevice *fp, int);

XW_DVI_EXPORT int JPEG_scan_file(JPEG_info *j_info, QIODevice *fp);

#endif//
