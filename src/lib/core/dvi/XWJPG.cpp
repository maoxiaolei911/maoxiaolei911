/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QIODevice>
#include "XWUtil.h"
#include "XWObject.h"
#include "XWJPG.h"

void
jpeg_get_density(JPEG_info *j_info,
		         double *xdensity, 
		         double *ydensity)
{
  	*xdensity = *ydensity = 1.0;
  	
  	if (IS_JFIF(j_info)) 
  	{
    	JPEG_APPn_JFIF *app_data;
    	int i = 0;
    	for (; i < j_info->num_appn; i++) 
    	{
      		if (j_info->appn[i].marker  == JM_APP0 || j_info->appn[i].app_sig == JS_APPn_JFIF)
        		break;
    	}
    	
    	if (i < j_info->num_appn) 
    	{
      		app_data = (JPEG_APPn_JFIF *)j_info->appn[i].app_data;
      		switch (app_data->units) 
      		{
      			case 1:
        			*xdensity = 72.0 / app_data->Xdensity;
        			*ydensity = 72.0 / app_data->Ydensity;
        			break;
        			
      			case 2: /* pixels per centimeter */
        			*xdensity = 72.0 / 2.54 / app_data->Xdensity;
        			*ydensity = 72.0 / 2.54 / app_data->Ydensity;
        			break;
        			
      			default:
        			break;
      		}
    	}
  	}
}

void
JPEG_info_init(JPEG_info *j_info)
{
	j_info->width  = 0;
  	j_info->height = 0;
  	j_info->bits_per_component = 0;
  	j_info->num_components = 0;

  	j_info->flags    = 0;
  	j_info->num_appn = 0;
  	j_info->max_appn = 0;
  	j_info->appn     = NULL;

  	memset(j_info->skipbits, 0, MAX_COUNT / 8 + 1);
}

void
JPEG_release_APPn_data(JPEG_marker marker, JPEG_APPn_sig app_sig, void *app_data)
{
	if (marker  == JM_APP0 && app_sig == JS_APPn_JFIF) 
	{
    	JPEG_APPn_JFIF * data = (JPEG_APPn_JFIF *) app_data;
    	if (data->thumbnail)
      		free(data->thumbnail);
    	data->thumbnail = 0;

    	free(data);
  	} 
  	else if (marker  == JM_APP2 && app_sig == JS_APPn_ICC) 
  	{
    	JPEG_APPn_ICC * data = (JPEG_APPn_ICC *) app_data;
    	if (data->chunk)
      		free(data->chunk);
    	data->chunk = 0;

    	free(data);
  	} 
  	else if (marker  == JM_APP14 && app_sig == JS_APPn_ADOBE) 
  	{
    	JPEG_APPn_Adobe * data = (JPEG_APPn_Adobe *) app_data;
    	free(data);
  	}
}

void
JPEG_info_clear(JPEG_info *j_info)
{
	if (j_info->num_appn > 0 && j_info->appn != NULL) 
	{
    	for (int i = 0; i < j_info->num_appn; i++)
      		JPEG_release_APPn_data(j_info->appn[i].marker, j_info->appn[i].app_sig, j_info->appn[i].app_data);
    	free(j_info->appn);
  	}
  	
  	j_info->appn     = 0;
  	j_info->num_appn = 0;
  	j_info->max_appn = 0;
  	j_info->flags    = 0;
}

XWObject *
JPEG_get_iccp(XWRef * xref, JPEG_info *j_info, XWObject * icc_stream)
{
	icc_stream->initStream(STREAM_COMPRESS, xref);
	int num_icc_seg = -1;
	int prev_id = 0;
	for (int i = 0; i < j_info->num_appn; i++) 
	{
    	if (j_info->appn[i].marker  != JM_APP2 || j_info->appn[i].app_sig != JS_APPn_ICC)
      		continue;
      		
    	JPEG_APPn_ICC * icc = (JPEG_APPn_ICC *) j_info->appn[i].app_data;
    	if (num_icc_seg < 0 && prev_id == 0) 
      		num_icc_seg = icc->num_chunks;
    	else if (icc->seq_id != prev_id + 1 || 
    		     num_icc_seg != icc->num_chunks || 
    		     icc->seq_id  > icc->num_chunks) 
    	{
      		icc_stream->free();
      		icc_stream->initNull();
      		return 0;
    	}
    	
    	icc_stream->streamAdd((const char*)(icc->chunk), icc->length);
    	prev_id = icc->seq_id;
    	num_icc_seg = icc->num_chunks;
  	}

  	return icc_stream;
}

JPEG_marker
JPEG_get_marker (QIODevice *fp)
{
	uchar c;
	fp->getChar((char*)&c);
  	if (c != 255)
    	return (JPEG_marker)-1;

  	for (;;) 
  	{
    	if (!fp->getChar((char*)&c))    		
      		return (JPEG_marker)-1;
    	else if (c > 0 && c < 255) 
      		return (JPEG_marker)c;
  	}

  	return (JPEG_marker)-1;
}

static int
add_APPn_marker(JPEG_info *j_info, JPEG_marker marker, int app_sig, void *app_data)
{
	if (j_info->num_appn >= j_info->max_appn) 
	{
	    j_info->max_appn += 16;
    	j_info->appn = (JPEG_ext*)realloc(j_info->appn, j_info->max_appn * sizeof(JPEG_ext));
  	}
  	
  	int n = j_info->num_appn;
  	j_info->appn[n].marker   = marker;
  	j_info->appn[n].app_sig  = (JPEG_APPn_sig)app_sig;
  	j_info->appn[n].app_data = app_data;

  	j_info->num_appn += 1;

  	return n;
}

static unsigned short
read_APP14_Adobe(JPEG_info *j_info, QIODevice *fp, unsigned short )
{
	JPEG_APPn_Adobe * app_data = (JPEG_APPn_Adobe*)malloc(sizeof(JPEG_APPn_Adobe));
  	app_data->version   = getUnsignedPair(fp);
  	app_data->flag0     = getUnsignedPair(fp);
  	app_data->flag1     = getUnsignedPair(fp);
  	app_data->transform = getUnsignedByte(fp);

  	add_APPn_marker(j_info, JM_APP14, JS_APPn_ADOBE, app_data);

  	return 7;
}

static unsigned short
read_APP0_JFIF(JPEG_info *j_info, QIODevice *fp, unsigned short)
{
	JPEG_APPn_JFIF * app_data = (JPEG_APPn_JFIF*)malloc(sizeof(JPEG_APPn_JFIF));
  	app_data->version  = getUnsignedPair(fp);
  	app_data->units    = getUnsignedByte(fp);
  	app_data->Xdensity = getUnsignedPair(fp);
  	app_data->Ydensity = getUnsignedPair(fp);
  	app_data->Xthumbnail = getUnsignedByte(fp);
  	app_data->Ythumbnail = getUnsignedByte(fp);
  	ushort thumb_data_len = 3 * app_data->Xthumbnail * app_data->Ythumbnail;
  	if (thumb_data_len > 0) 
  	{
    	app_data->thumbnail = (uchar*)malloc(thumb_data_len * sizeof(uchar));
    	fp->read((char*)(app_data->thumbnail), thumb_data_len);
  	} 
  	else 
    	app_data->thumbnail = 0;

  	add_APPn_marker(j_info, JM_APP0, JS_APPn_JFIF, app_data);

  	return (9 + thumb_data_len);
}

static unsigned short
read_APP0_JFXX(JPEG_info *, QIODevice *fp, unsigned short length)
{
	getUnsignedByte(fp);
	ulong pos = fp->pos();
	pos += length-1;
	fp->seek(pos);
	return length;
}

static unsigned short
read_APP2_ICC(JPEG_info *j_info, QIODevice *fp, unsigned short length)
{
	JPEG_APPn_ICC * app_data = (JPEG_APPn_ICC*)malloc(sizeof(JPEG_APPn_ICC));
  	app_data->seq_id      = getUnsignedByte(fp);
  	app_data->num_chunks  = getUnsignedByte(fp);
  	app_data->length = length - 2;
  	app_data->chunk  = (uchar*)malloc(app_data->length * sizeof(uchar));
  	fp->read((char*)(app_data->chunk), app_data->length);

  	add_APPn_marker(j_info, JM_APP2, JS_APPn_ICC, app_data);

  	return length;
}

int
JPEG_copy_stream(JPEG_info *j_info, XWObject *stream, QIODevice *fp, int)
{
	fp->seek(0);
	int count      = 0;
  	int found_SOFn = 0;
  	JPEG_marker marker;
  	long length, nb_read;
  	char work_buffer[1024];
  	while (!found_SOFn && 
  	       count < MAX_COUNT && 
  	       (marker = JPEG_get_marker(fp)) >= 0)
	{
		if (marker == JM_SOI || 
			(marker >= JM_RST0 && 
			marker <= JM_RST7)) 
		{
      		work_buffer[0] = (char) 0xff;
      		work_buffer[1] = (char) marker;
      		stream->streamAdd(work_buffer, 2);
      		count++;
      		continue;
    	}
    	
    	length = getUnsignedPair(fp) - 2;
    	switch (marker)
    	{
    		case JM_SOF0:  
    		case JM_SOF1:  
    		case JM_SOF2:  
    		case JM_SOF3:
    		case JM_SOF5:  
    		case JM_SOF6:  
    		case JM_SOF7:  
    		case JM_SOF9:
    		case JM_SOF10: 
    		case JM_SOF11: 
    		case JM_SOF13: 
    		case JM_SOF14:
    		case JM_SOF15:
      			work_buffer[0] = (char) 0xff;
      			work_buffer[1] = (char) marker;
      			work_buffer[2] = ((length + 2) >> 8) & 0xff;
      			work_buffer[3] =  (length + 2) & 0xff;
      			stream->streamAdd(work_buffer, 4);
      			while (length > 0) 
      			{
					nb_read = fp->read(work_buffer, qMin(length, (long)1024));
					if (nb_read > 0)
	  					stream->streamAdd(work_buffer, nb_read);
					length -= nb_read;
      			}
      			found_SOFn = 1;
      			break;
      			
      		default:
      			if (j_info->skipbits[count / 8] & (1 << (7 - (count % 8))))
      			{
      				while (length > 0) 
      				{
	  					nb_read = fp->read(work_buffer, qMin(length, (long)1024));
	  					length -= nb_read;
					}
      			}
      			else
      			{
      				work_buffer[0] = (char) 0xff;
					work_buffer[1] = (char) marker;
					work_buffer[2] = ((length + 2) >> 8) & 0xff;
					work_buffer[3] =  (length + 2) & 0xff;
					stream->streamAdd(work_buffer, 4);
					while (length > 0) 
					{
	  					nb_read = fp->read(work_buffer, qMin(length, (long)1024));
	  					if (nb_read > 0)
	    					stream->streamAdd(work_buffer, nb_read);
	  					length -= nb_read;
					}
      			}
      			break;
    	}
    	
    	count++;
	}
	
	while ((length = fp->read(work_buffer, 1024)) > 0) 
    	stream->streamAdd(work_buffer, length);

  	return (found_SOFn ? 0 : -1);
}

int
JPEG_scan_file(JPEG_info *j_info, QIODevice *fp)
{
	fp->seek(0);
	
	JPEG_marker marker;
  	unsigned short length;
  	char app_sig[128];
  	int count      = 0;
  	int found_SOFn = 0;
  	while (!found_SOFn && (marker = JPEG_get_marker(fp)) >= 0)
  	{
  		if (marker == JM_SOI  || (marker >= JM_RST0 && marker <= JM_RST7)) 
  		{
      		count++;
      		continue;
    	}
    	
    	length = getUnsignedPair(fp) - 2;
    	switch (marker)
    	{
    		case JM_SOF0:  
    		case JM_SOF1:  
    		case JM_SOF2:  
    		case JM_SOF3:
    		case JM_SOF5:  
    		case JM_SOF6:  
    		case JM_SOF7:  
    		case JM_SOF9:
    		case JM_SOF10: 
    		case JM_SOF11: 
    		case JM_SOF13: 
    		case JM_SOF14:
    		case JM_SOF15:
      			j_info->bits_per_component = getUnsignedByte(fp);
      			j_info->height = getUnsignedPair(fp);
      			j_info->width  = getUnsignedPair(fp);
      			j_info->num_components = getUnsignedByte(fp);
      			found_SOFn = 1;
      			break;
      			
      		case JM_APP0:
      			if (length > 5) 
      			{
					if (fp->read(app_sig, 5) != 5)
	  					return -1;
					length -= 5;
					if (!memcmp(app_sig, "JFIF\000", 5)) 
					{
	  					j_info->flags |= HAVE_APPn_JFIF;
	  					length -= read_APP0_JFIF(j_info, fp, length);
					} 
					else if (!memcmp(app_sig, "JFXX", 5)) 
					{
	  					length -= read_APP0_JFXX(j_info, fp, length);
					}
      			}
      			{
      				ulong pos = fp->pos();
      				pos += length;
      				fp->seek(pos);
      			}
      			break;
      			
      		case JM_APP2:
      			if (length >= 14) 
      			{
					if (fp->read(app_sig, 12) != 12)
	  					return -1;
					length -= 12;
					if (!memcmp(app_sig, "ICC_PROFILE\000", 12)) 
					{
	  					j_info->flags |= HAVE_APPn_ICC;
	  					length -= read_APP2_ICC(j_info, fp, length);
	  					if (count < MAX_COUNT) 
	  					{
	    					j_info->skipbits[count / 8] |= (1 << (7 - (count % 8)));
	  					}
					}
      			}
      			
      			{
      				ulong pos = fp->pos();
      				pos += length;
      				fp->seek(pos);
      			}
      			break;
      			
    		case JM_APP14:
      			if (length > 5) 
      			{
					if (fp->read(app_sig, 5) != 5)
	  					return -1;
					length -= 5;
					if (!memcmp(app_sig, "Adobe", 5)) 
					{
	  					j_info->flags |= HAVE_APPn_ADOBE;
	  					length -= read_APP14_Adobe(j_info, fp, length);
					} 
					else 
					{
	  					if (count < MAX_COUNT) 
	  					{
	    					j_info->skipbits[count/8] |= (1 << (7 - (count % 8)));
	  					}
					}
      			}
      			
      			{
      				ulong pos = fp->pos();
      				pos += length;
      				fp->seek(pos);
      			}
      			break;
      			
      		default:
      			{
      				ulong pos = fp->pos();
      				pos += length;
      				fp->seek(pos);
      			}
      			if (marker >= JM_APP0 && marker <= JM_APP15) 
      			{
					if (count < MAX_COUNT) 
					{
	  					j_info->skipbits[count / 8] |= (1 << (7 - (count % 8)));
					}
      			}
      			break;
    	}
    	
    	count++;
  	}
  	
  	return (found_SOFn ? 0 : -1);
}
