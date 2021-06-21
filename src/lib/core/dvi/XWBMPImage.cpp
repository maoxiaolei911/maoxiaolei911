/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QString>
#include <QByteArray>
#include "XWUtil.h"
#include "XWApplication.h"
#include "XWObject.h"
#include "XWDVIRef.h"
#include "XWBMPImage.h"


#define DIB_FILE_HEADER_SIZE 14
#define DIB_CORE_HEADER_SIZE 14
#define DIB_INFO_HEADER_SIZE 40

#define DIB_COMPRESS_NONE 0
#define DIB_COMPRESS_RLE8 1
#define DIB_COMPRESS_RLE4 2

#define DIB_HEADER_SIZE_MAX (DIB_FILE_HEADER_SIZE+DIB_INFO_HEADER_SIZE)


XWBMPImage::XWBMPImage(XWDVIRef * xref,
	                   const char *identA, 
	                   int  subtypeA,
	                   int  formatA,
	                   long page_noA, 	           
	                   const QString & filenameA,
	                   XWObject *dictA)
	:XWDVIImage(xref, identA, subtypeA, formatA, page_noA, filenameA, dictA)
{
}

int XWBMPImage::checkForBMP(QIODevice *fp)
{
	fp->seek(0);	
	uchar sigbytes[2];
	if (fp->read((char*)sigbytes, 2) != sizeof(sigbytes) || 
		sigbytes[0] != 'B' || 
		sigbytes[1] != 'M')
    	return 0;
  	else
    	return 1;
  
  	return 0;
}

int XWBMPImage::load(XWDVIRef * xref, QIODevice *fp)
{
	uchar  buf[DIB_HEADER_SIZE_MAX+4];
	fp->seek(0);
	if (fp->read((char*)buf, DIB_FILE_HEADER_SIZE + 4) != DIB_FILE_HEADER_SIZE + 4)
    	xwApp->warning("could not read BMP file header....\n");
	
	uchar * p = buf;
  	if (p[0] != 'B' || p[1] != 'M') 
  	{
    	xwApp->warning("file not starting with \'B\' \'M\'... Not a BMP file?.\n");
    	return -1;
  	}
  	p += 2;
  	
  	
#define ULONG_LE(b)  ((b)[0] + ((b)[1] << 8) +\
		      ((b)[2] << 16) + ((b)[3] << 24))
#define USHORT_LE(b) ((b)[0] + ((b)[1] << 8))

  	long fsize  = ULONG_LE(p); p += 4;
  	if (ULONG_LE(p) != 0) 
  	{
    	xwApp->warning("not a BMP file???.\n");
    	return -1;
  	}
  	
  	p += 4;
  	long offset = ULONG_LE(p); 
  	p += 4;
  	
  	long hsize  = ULONG_LE(p); p += 4;
  	if (fp->read((char*)p, hsize - 4) != hsize - 4) 
  	{
    	xwApp->warning("could not read BMP file header...\n");
    	return -1;
  	}
  	
  	int flip = 1;  	
  	XWDVIImageInfo info;
  	ushort bit_count = 0;
  	long compression = 0;
  	int psize = 0;
  	if (hsize == DIB_CORE_HEADER_SIZE) 
  	{
    	info.width  = USHORT_LE(p); p += 2;
    	info.height = USHORT_LE(p); p += 2;
    	if (USHORT_LE(p) != 1) 
    	{
      		xwApp->warning("unknown bcPlanes value in BMP COREHEADER..\n");
      		return -1;
    	}
    	
    	p += 2;
    	bit_count   = USHORT_LE(p); p += 2;
    	compression = DIB_COMPRESS_NONE;
    	psize = 3;
  	} 
  	else if (hsize == DIB_INFO_HEADER_SIZE)
  	{
  		info.width  = ULONG_LE(p);  p += 4;
    	info.height = ULONG_LE(p);  p += 4;
    	if (USHORT_LE(p) != 1) 
    	{
      		xwApp->warning("unknown biPlanes value in BMP INFOHEADER.\n");
      		return -1;
    	}
    	p += 2;
    	bit_count   = USHORT_LE(p); p += 2;
    	compression = ULONG_LE(p);  p += 4;
    	if (info.height < 0) 
    	{
      		info.height = -info.height;
      		flip = 0;
    	}
    	psize = 4;
  	}
  	else
  	{
  		xwApp->warning("Unknown BMP header type.\n");
  		return -1;
  	}
  		
  	int num_palette = 0;
  	if (bit_count < 24) 
  	{
    	if (bit_count != 1 && bit_count != 4 && bit_count != 8) 
    	{
    		QString msg = QString("unsupported palette size: %1\n").arg(bit_count);
      		xwApp->warning(msg);
      		return -1;
    	}
    	
    	num_palette = (offset - hsize - DIB_FILE_HEADER_SIZE) / psize;
    	info.bits_per_component = bit_count;
    	info.num_components = 1;
  	} 
  	else if (bit_count == 24) 
  	{ /* full color */
    	num_palette = 1; /* dummy */
    	info.bits_per_component = 8;
    	info.num_components = 3;
  	} 
  	else 
  	{
  		QString msg = QString("unkown BMP bitCount: %1\n").arg(bit_count);
    	xwApp->warning(msg);
      	return -1;
  	}

  	if (info.width == 0 || info.height == 0 || num_palette < 1) 
  	{
  		QString msg = QString("invalid BMP file: width=%1, height=%2, #palette=%3\n")
  							.arg(info.width).arg(info.height).arg(num_palette);
  		xwApp->warning(msg);
    	return -1;
  	}
  	
  	XWObject stream, colorspace, obj;
  	stream.initStream(STREAM_COMPRESS, xref);
  	if (bit_count < 24)
  	{
  		uchar bgrq[4];
  		uchar * palette = new uchar[num_palette*3+1];
  		for (int i = 0; i < num_palette; i++) 
  		{
      		if (fp->read((char*)bgrq, psize) != psize) 
      		{
				xwApp->warning("reading file failed...\n");
				delete [] palette;
				stream.free();
				return -1;
      		}
      		/* BGR data */
      		palette[3*i  ] = bgrq[2];
      		palette[3*i+1] = bgrq[1];
      		palette[3*i+2] = bgrq[0];
    	}
    	
    	XWObject lookup;
    	lookup.initString(new XWString((char*)palette, num_palette*3));
    	delete [] palette;
    	colorspace.initArray(xref);
    	obj.initName("Indexed");
    	colorspace.arrayAdd(&obj);
    	obj.initName("DeviceRGB");
    	colorspace.arrayAdd(&obj);
    	obj.initInt(num_palette-1);
    	colorspace.arrayAdd(&obj);
    	colorspace.arrayAdd(&lookup);
  	}
  	else
  		colorspace.initName("DeviceRGB");
  		
  	stream.streamGetDict()->add(qstrdup("ColorSpace"), &colorspace);
  	
  	{
  		long rowbytes = (info.width * bit_count + 7) / 8;
  		uchar *stream_data_ptr = 0;
  		fp->seek(offset);
    	if (compression == DIB_COMPRESS_NONE)
    	{
    		int padding = (rowbytes % 4) ? 4 - (rowbytes % 4) : 0;
      		long dib_rowbytes = rowbytes + padding;
      		stream_data_ptr = (uchar*)malloc((rowbytes*info.height + padding) * sizeof(uchar));
      		for (long n = 0; n < info.height; n++)
      		{
      			p = stream_data_ptr + n * rowbytes;
      			if (fp->read((char*)p, dib_rowbytes) != dib_rowbytes)
      			{
      				xwApp->warning("reading BMP raster data failed...\n");
	  				stream.free();
	  				free(stream_data_ptr);
	  				return -1;
      			}
      		}
    	}
    	else if (compression == DIB_COMPRESS_RLE8)
    	{
    		stream_data_ptr = (uchar *)malloc((rowbytes*info.height) * sizeof(uchar));
      		if (readRasterRLE8(stream_data_ptr, info.width, info.height, fp) < 0) 
      		{
				xwApp->warning("reading BMP raster data failed...\n");
	  			stream.free();
	  			free(stream_data_ptr);
	  			return -1;
      		}
    	}
    	else if (compression == DIB_COMPRESS_RLE4)
    	{
    		stream_data_ptr = (uchar *)malloc((rowbytes*info.height) * sizeof(uchar));
      		if (readRasterRLE4(stream_data_ptr, info.width, info.height, fp) < 0) 
      		{
				xwApp->warning("reading BMP raster data failed...\n");
	  			stream.free();
	  			free(stream_data_ptr);
	  			return -1;
      		}
    	}
    	else
    	{
    		stream.free();
	  		return -1;
    	}
    	
    	if (bit_count == 24) 
    	{
      		for (long n = 0; n < info.width * info.height * 3; n += 3) 
      		{
				uchar g = stream_data_ptr[n];
				stream_data_ptr[n  ] = stream_data_ptr[n+2];
				stream_data_ptr[n+2] = g;
      		}
    	}
    	
    	if (flip) 
    	{
      		for (long n = info.height - 1; n >= 0; n--) 
      		{
				p = stream_data_ptr + n * rowbytes;
				stream.streamAdd((const char*)p, rowbytes);
      		}
    	} 
    	else 
      		stream.streamAdd((const char*)stream_data_ptr, rowbytes*info.height);
      		
    	free(stream_data_ptr);
  	}
  	
  	setImage(xref, &info, &stream);

  	return 0;
}

long XWBMPImage::readRasterRLE4(uchar *data_ptr, 
		  				        long width, 
		  				        long height, 
		  				        QIODevice *fp)
{
	uchar * p = data_ptr;
  	long rowbytes = (width + 1) / 2;
  	memset(data_ptr, 0, rowbytes*height);
  	long count = 0;
  	int eoi = 0;
  	int nbytes = 0;
  	for (long v = 0; v < height && !eoi; v++)
  	{
  		int eol = 0;
  		for (long h = 0; h < width && !eol;)
  		{
  			uchar b0 = getUnsignedByte(fp);
      		uchar b1 = getUnsignedByte(fp);
      		count += 2;
      		p  = data_ptr + v * rowbytes + (h / 2);
      		if (b0 == 0x00)
      		{
      			switch (b1)
      			{
      				case 0x00: /* EOL */
	  					eol = 1;
	  					break;
	  					
					case 0x01: /* EOI */
	  					eoi = 1;
	  					break;
	  					
					case 0x02:
	  					h += getUnsignedByte(fp);
	  					v += getUnsignedByte(fp);
	  					count += 2;
	  					break;
	  					
	  				default:
	  					if (h + b1 > width) 
	    					return -1;
	  					nbytes = (b1 + 1)/2;
	  					if (h % 2) 
	  					{
	    					for (int i = 0; i < nbytes; i++) 
	    					{
	      						uchar b = getUnsignedByte(fp);
	      						*p++ |= (b >> 4) & 0x0f;
	      						*p    = (b << 4) & 0xf0;
	    					}
	  					} 
	  					else 
	  					{
	    					if (fp->read((char*)p, nbytes) != nbytes) 
	      						return -1;
	  					}
	  					h     += b1;
	  					count += nbytes;
	  					if (nbytes % 2) 
	  					{
	    					getUnsignedByte(fp);
	    					count++;
	  					}
	  					break;
      			}
      		}
      		else
      		{
      			if (h + b0 > width) 
	  				return -1;
				if (h % 2) 
				{
	  				*p++ = (b1 >> 4) & 0x0f;
	  				b1   = ((b1 << 4) & 0xf0)|((b1 >> 4) & 0x0f);
	  				b0--;
	  				h++;
				}
				nbytes = (b0 + 1)/2;
				memset(p, b1, nbytes);
				h += b0;
				if (h % 2)
	  				p[nbytes-1] &= 0xf0;
      		}
  		}
  		
  		if (!eol && !eoi) 
  		{
      		uchar b0 = getUnsignedByte(fp);
      		uchar b1 = getUnsignedByte(fp);
      		if (b0 != 0x00) 
				return -1;
      		else if (b1 == 0x01) 
				eoi = 1;
      		else if (b1 != 0x00) 
				return -1;
    	}
  	}
  	
  	return count;
}

long XWBMPImage::readRasterRLE8(uchar *data_ptr,
		                        long width, 
		                        long height, 
		                        QIODevice *fp)
{
	uchar * p = data_ptr;
  	long rowbytes = (width + 1) / 2;
  	memset(data_ptr, 0, rowbytes*height);
  	long count = 0;
  	int eoi = 0;
  	for (long v = 0; v < height && !eoi; v++)
  	{
  		int eol = 0;
  		for (long h = 0; h < width && !eol; )
  		{
  			uchar b0 = getUnsignedByte(fp);
      		uchar b1 = getUnsignedByte(fp);
      		count += 2;

      		p = data_ptr + v * rowbytes + h;

      		if (b0 == 0x00)
      		{
      			switch (b1)
      			{
      				case 0x00: /* EOL */
	  					eol = 1;
	  					break;
	  					
					case 0x01: /* EOI */
	  					eoi = 1;
	  					break;
	  					
					case 0x02:
	  					h += getUnsignedByte(fp);
	  					v += getUnsignedByte(fp);
	  					count += 2;
	  					break;
	  					
					default:
	  					h += b1;
	  					if (h > width) 
	    					return -1;
	  					if (fp->read((char*)p, b1) != b1)
	    					return -1;
	  					count += b1;
	  					if (b1 % 2) 
	  					{
	    					getUnsignedByte(fp);
	    					count++;
	  					}
	  					break;
      			}
      		}
      		else
      		{
      			h += b0;
				if (h > width) 
	  				return -1;
				memset(p, b1, b0);
      		}
  		}
  		
  		if (!eol && !eoi) 
  		{
      		uchar b0 = getUnsignedByte(fp);
      		uchar b1 = getUnsignedByte(fp);
      		if (b0 != 0x00) 
				return -1;
      		else if (b1 == 0x01) 
				eoi = 1;
      		else if (b1 != 0x00) 
				return -1;
    	}
  	}
  	
  	return count;
}

