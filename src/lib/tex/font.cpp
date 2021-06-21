/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <QByteArray>
#include "XWUtil.h"
#include "XWApplication.h"
#include "XWFontSea.h"
#include "XWTFMCreator.h"
#include "XWTeX.h"

void XWTeX::allocateFontTable(qint32 font_number, qint32 font_size)
{
	if (font_entries==0)
	{
		fonttables = (TeXMemoryWord **)malloc(256 * sizeof(TeXMemoryWord**));
        font_entries=256;
    for (int i = 0; i < font_entries; i++)
    	fonttables[i] = 0;
	}
	else if ((font_number == 256)&&(font_entries == 256))
	{
		TeXMemoryWord ** t = (TeXMemoryWord **)malloc(65536 * sizeof(TeXMemoryWord**));
		for (int i = 0; i < 65536; i++)
    	t[i] = 0;
    	
    for (int i = 0; i < font_entries; i++)
    	t[i] = fonttables[i];
    	
    free(fonttables);
    fonttables = t;
    font_entries=65536;
	}
	
	fonttables[font_number] = (TeXMemoryWord *) malloc((font_size+1) * sizeof(TeXMemoryWord));
    fonttables[font_number][0].ii.CINT0 = font_size;
    fonttables[font_number][0].ii.CINT1 = 0;
}

qint32 XWTeX::readFontInfo(qint32 u, 
	                       qint32 nom, 
	                       qint32 aire, 
	                       qint32 s,
	                       qint32 offset,
	                       qint32 natural_dir)
{
	qint32 lf, lh, bc, ec, nw, nh, nd, ni, nl, nk, ne, np, header_length, neew, font_dir, nlw;
	qint32 f, z, i, k, font_counter, param, a, b, c, d, sw, alpha, beta, k_param, bytes_per_entry;
	qint32 extra_char_bytes, repeat_no, table_counter, bch_label, bchar;
	TeXFourQuarters qw;
	qint32 nco = 0; 
	qint32 ncw = 0; 
	qint32 npc = 0;
	qint32 nki = 0; 
	qint32 nwi = 0; 
	qint32 nkf = 0; 
	qint32 nwf = 0; 
	qint32 nkm = 0; 
	qint32 nwm = 0;
	qint32 nkr = 0; 
	qint32 nwr = 0; 
	qint32 nkg = 0; 
	qint32 nwg = 0; 
	qint32 nkp = 0; 
	qint32 nwp = 0;
	QString fn;
	QByteArray ba;
	qint32 first_two = 0;
	qint32 font_level = -1;
	qint32 g = TEX_NULL_FONT;
	QIODevice * tfmfile = 0;
	bool file_opened = false;
	qint32 table_size[100];
	
	packFileName(nom, aire, -1);
	fn = getFileName();	
	XWFontSea sea;
	tfmfile = sea.openOfm(fn);
	if (!tfmfile)
		tfmfile = sea.openTFM(fn);
			
	if (!tfmfile)
	{
		ba = fn.toAscii();
		XWTFMCreator creater(ba.constData());
		if (creater.isOK())
		{
			tfmfile = new QBuffer;
			tfmfile->open(QIODevice::WriteOnly);
			creater.writeTFM(tfmfile);
			tfmfile->close();
			tfmfile->open(QIODevice::ReadOnly);
		}
	}
	
	if (!tfmfile)
		goto badtfm;
	
	file_opened = true;
	first_two = (qint32)(readSixteen(tfmfile));
	if (first_two != 0)
	{
		lf = first_two;
		lh = (qint32)(readSixteen(tfmfile));
		bc = (qint32)(readSixteen(tfmfile));
		ec = (qint32)(readSixteen(tfmfile));
		if ((bc > (ec + 1)) || (ec > 255))
			goto badtfm;
		
		if (bc > 255)
		{
			bc = 1; 
			ec = 0;
		}
		
		nw = (qint32)(readSixteen(tfmfile));
  		nh = (qint32)(readSixteen(tfmfile));
  		nd = (qint32)(readSixteen(tfmfile));
  		ni = (qint32)(readSixteen(tfmfile));
  		nl = (qint32)(readSixteen(tfmfile));
  		nk = (qint32)(readSixteen(tfmfile));
  		ne = (qint32)(readSixteen(tfmfile));
  		np = (qint32)(readSixteen(tfmfile));
  		header_length = 6;
  		ncw  = (ec - bc + 1);
  		nlw  = nl;
  		neew = ne;
	}
	else
	{
		font_level = (qint32)(readSixteen(tfmfile));
		if ((font_level != 0) && (font_level != 1))
			goto badtfm;
			
		lf = (qint32)(readThirtyTwo(tfmfile));
  		lh = (qint32)(readThirtyTwo(tfmfile));
  		bc = (qint32)(readThirtyTwo(tfmfile));
  		ec = (qint32)(readThirtyTwo(tfmfile));
  		if ((bc > ec + 1) || (ec > TEX_BIGGEST_CHAR))
  			goto badtfm;
  			
  		if (bc > 65535)
  		{
  			bc = 1; 
  			ec = 0;
  		}
  		
  		nw = (qint32)(readThirtyTwo(tfmfile));
  		nh = (qint32)(readThirtyTwo(tfmfile));
  		nd = (qint32)(readThirtyTwo(tfmfile));
  		ni = (qint32)(readThirtyTwo(tfmfile));
  		nl = (qint32)(readThirtyTwo(tfmfile));
  		nk = (qint32)(readThirtyTwo(tfmfile));
  		ne = (qint32)(readThirtyTwo(tfmfile));
  		np = (qint32)(readThirtyTwo(tfmfile));
  		font_dir = (qint32)(readThirtyTwo(tfmfile));
  		nlw  = 2 * nl;
  		neew =2 * ne;
  		
  		if (font_level == 0)
  		{
  			header_length = 14;
    		ncw = 2 *(ec - bc + 1);
  		}
  		else
  		{
  			header_length = 29;
    		nco = (qint32)(readThirtyTwo(tfmfile));
    		ncw = (qint32)(readThirtyTwo(tfmfile));
    		npc = (qint32)(readThirtyTwo(tfmfile));
    		nki = (qint32)(readThirtyTwo(tfmfile));
    		nwi = (qint32)(readThirtyTwo(tfmfile));
    		nkf = (qint32)(readThirtyTwo(tfmfile));
    		nwf = (qint32)(readThirtyTwo(tfmfile));
    		nkm = (qint32)(readThirtyTwo(tfmfile));
    		nwm = (qint32)(readThirtyTwo(tfmfile));
    		nkr = (qint32)(readThirtyTwo(tfmfile));
    		nwr = (qint32)(readThirtyTwo(tfmfile));
    		nkg = (qint32)(readThirtyTwo(tfmfile));
    		nwg = (qint32)(readThirtyTwo(tfmfile));
    		nkp = (qint32)(readThirtyTwo(tfmfile));
    		nwp = (qint32)(readThirtyTwo(tfmfile));
  		}
	}
	
	if (lf != (header_length +lh + ncw + nw + nh + nd + ni + nlw + nk + neew + np +
        	   nki + nwi + nkf + nwf + nkm + nwm + nkr + nwr + nkg + nwg + nkp + nwp))
    {
    	goto badtfm;
    }
    
    switch (font_level)
    {
    	case -1:
    		lf = lf - 6 - lh;
    		break;
    		
    	case 0:
    		lf = lf - 14 - lh - (ec - bc + 1) - nl - ne;
    		break;
    		
    	case 1:
    		lf = lf - 29 - lh - ncw +(1 + npc) * (ec - bc + 1) - nl - ne;
    		break;
    }
    
    if (np < 7)
    	lf =lf + 7 - np;
    	
    if ((font_ptr == TEX_FONT_MAX))
    {
    	printErr(tr("Font ")); 
    	sprintCS(u);
  		printChar('='); 
  		printFileName(nom, aire, TEX_TOO_BIG_CHAR);
  		if (s >= 0)
  		{
  			print(tr(" at ")); 
  			printScaled(s); 
  			print("pt");
  		}
  		else if (s != -1000)
  		{
  			print(tr(" scaled ")); 
  			printInt(-s);
  		}
  		
  		print(tr(" not loaded: Not enough room left"));
  		help4(tr("I'm afraid I won't be able to make use of this font,"));
  		help_line[2] = tr("because my memory for character-size data is too small.");
  		help_line[1] = tr("If you're really stuck, ask a wizard to enlarge me.");
  		help_line[0] = tr("Or maybe try `I\\font<same font id>=<name of loaded font>'.");
  		error(); 
  		goto done;
    }
    
    f = font_ptr + 1;
	allocateFontTable(f, TEX_OFFSET_CHARINFO_BASE + lf + 100);
	font_file_size(f) = TEX_OFFSET_CHARINFO_BASE + lf + 100;
	font_used(f) = false;
	font_offset(f) = offset;
	font_natural_dir(f) = natural_dir;
	char_base(f)       = TEX_OFFSET_CHARINFO_BASE - bc;
	char_attr_base(f)  = char_base(f) + ec + 1;
	ivalues_start(f)   = char_attr_base(f) + npc *(ec - bc + 1);
	fvalues_start(f)   = ivalues_start(f) + nki;
	mvalues_start(f)   = fvalues_start(f) + nkf;
	rules_start(f)     = mvalues_start(f) + nkm;
	glues_start(f)     = rules_start(f) + nkr;
	penalties_start(f) = glues_start(f) + nkg;
	ivalues_base(f)    = penalties_start(f) + nkp;
	fvalues_base(f)    = ivalues_base(f) + nwi;
	mvalues_base(f)    = fvalues_base(f) + nwf;
	rules_base(f)      = mvalues_base(f) + nwm;
	glues_base(f)      = rules_base(f) + nwr;
	penalties_base(f)  = glues_base(f) + nwg;
	width_base(f)      = penalties_base(f) + nwp;
	height_base(f)     = width_base(f) + nw;
	depth_base(f)      = height_base(f) + nh;
	italic_base(f)     = depth_base(f) + nd;
	lig_kern_base(f)   = italic_base(f) + ni;
	kern_base(f)       = lig_kern_base(f) + nl - TEX_KERN_BASE_OFFSET;
	exten_base(f)      = kern_base(f) + TEX_KERN_BASE_OFFSET + nk;
	param_base(f)      = exten_base(f) + ne;
	
	if (lh < 2)
		goto badtfm;
		
	font_check_0(f) = getByte(tfmfile);
	font_check_1(f) = getByte(tfmfile);
	font_check_2(f) = getByte(tfmfile);
	font_check_3(f) = getByte(tfmfile);
	z = (qint32)(readSixteen(tfmfile));
	z = z * 256 + getByte(tfmfile);
	z = (z * 16) + (getByte(tfmfile) / 16);
	if (z < TEX_UNITY)
		goto badtfm;
		
	while (lh > 2)
	{
		getByte(tfmfile);
		getByte(tfmfile);
		getByte(tfmfile);
		getByte(tfmfile);
		lh--;
	}
	
	font_dsize(f) = z;
	if (s != -1000)
	{
		if (s >= 0)
			z = s;
		else
			z = xnOverD(z, -s, 1000);
	}
	
	font_size(f) = z;
	
	beta  = 16;
	alpha = 16;
	while (z >= 0x800000)
	{
		z = z / 2; 
		alpha = alpha + alpha;
	}	
	beta = 256 / alpha; 
	alpha = alpha * z;
	
	if (font_level == 1)
	{
		i = 0;
  		k = ivalues_start(f);
  		font_counter = ivalues_base(f);
  		while (k < fvalues_start(f))
  		{
  			param = (qint32)(readThirtyTwo(tfmfile));
    		font_info(f, k).ii.CINT0 = font_counter;
    		font_counter = font_counter + param;
    		table_size[i] = 1;
    		i++; 
    		k++;
  		}
  		
  		while (k < mvalues_start(f))
  		{
  			param = (qint32)(readThirtyTwo(tfmfile));
    		font_info(f, k).ii.CINT0 = font_counter;
    		font_counter = font_counter + param;
    		table_size[i] = 1;
    		i++; 
    		k++;
  		}
  		
  		while (k < rules_start(f))
  		{
  			param = (qint32)(readThirtyTwo(tfmfile));
    		font_info(f, k).ii.CINT0 = font_counter;
    		font_counter = font_counter + param;
    		table_size[i] = 1;
    		i++; 
    		k++;
  		}
  		
  		while (k < glues_start(f))
  		{
  			param = (qint32)(readThirtyTwo(tfmfile));
    		font_info(f, k).ii.CINT0 = font_counter;
    		font_counter = font_counter + param * 3;
    		table_size[i] = 3;
    		i++; 
    		k++;
  		}
  		
  		while (k < penalties_start(f))
  		{
  			param = (qint32)(readThirtyTwo(tfmfile));
    		font_info(f, k).ii.CINT0 = font_counter;
    		font_counter = font_counter + param * 4;
    		table_size[i] = 4;
    		i++; 
    		k++;
  		}
  		
  		while (k < ivalues_base(f))
  		{
  			param = (qint32)(readThirtyTwo(tfmfile));
    		font_info(f, k).ii.CINT0 = font_counter;
    		font_counter = font_counter + param * 1;
    		table_size[i] = 1;
    		i++; 
    		k++;
  		}
  		
  		while (k < fvalues_base(f))
  		{
  			font_info(f, k).ii.CINT0 = (qint32)(readThirtyTwo(tfmfile));
    		k++;
  		}
  		
  		while (k < mvalues_base(f))
  		{
  			font_info(f, k).ii.CINT0 = (qint32)(readThirtyTwo(tfmfile));
    		k++;
  		}
  		
  		while (k < rules_base(f))
  		{
  			font_info(f, k).ii.CINT0 = (qint32)(readThirtyTwo(tfmfile));
    		k++;
  		}
  		
  		while (k < glues_base(f))
  		{
  			a  = getByte(tfmfile); 
  			b  = getByte(tfmfile); 
  			c  = getByte(tfmfile); 
  			d  = getByte(tfmfile); 
  			sw =(((((d * z) / 256) + (c * z)) / 256) + (b * z)) / beta;
  			if (a == 0)
  				font_info(f, k).ii.CINT0 = sw;
  			else if (a == 255)
  				font_info(f, k).ii.CINT0 = sw - alpha;
  			else
  				goto badtfm;
  				
  			a  = getByte(tfmfile); 
  			b  = getByte(tfmfile); 
  			c  = getByte(tfmfile); 
  			d  = getByte(tfmfile); 
  			sw =(((((d * z) / 256) + (c * z)) / 256) + (b * z)) / beta;
  			if (a == 0)
  				font_info(f, k + 1).ii.CINT0 = sw;
  			else if (a == 255)
  				font_info(f, k + 1).ii.CINT0 = sw - alpha;
  			else
  				goto badtfm;
  				
  			a  = getByte(tfmfile); 
  			b  = getByte(tfmfile); 
  			c  = getByte(tfmfile); 
  			d  = getByte(tfmfile); 
  			sw =(((((d * z) / 256) + (c * z)) / 256) + (b * z)) / beta;
  			if (a == 0)
  				font_info(f, k + 2).ii.CINT0 = sw;
  			else if (a == 255)
  				font_info(f, k + 2).ii.CINT0 = sw - alpha;
  			else
  				goto badtfm;
  				
  			k += 3;
  		}
  		
  		while (k < penalties_base(f))
  		{
  			font_info(f, k).hh.v.LH = (qint32)(readSixteen(tfmfile));
    		font_info(f, k).hh.v.RH = (qint32)(readSixteen(tfmfile));
    		a  = getByte(tfmfile); 
  			b  = getByte(tfmfile); 
  			c  = getByte(tfmfile); 
  			d  = getByte(tfmfile); 
  			sw =(((((d * z) / 256) + (c * z)) / 256) + (b * z)) / beta;
  			if (a == 0)
  				font_info(f, k + 1).ii.CINT0 = sw;
  			else if (a == 255)
  				font_info(f, k + 1).ii.CINT0 = sw - alpha;
  			else
  				goto badtfm;
  				
  			a  = getByte(tfmfile); 
  			b  = getByte(tfmfile); 
  			c  = getByte(tfmfile); 
  			d  = getByte(tfmfile); 
  			sw =(((((d * z) / 256) + (c * z)) / 256) + (b * z)) / beta;
  			if (a == 0)
  				font_info(f, k + 2).ii.CINT0 = sw;
  			else if (a == 255)
  				font_info(f, k + 2).ii.CINT0 = sw - alpha;
  			else
  				goto badtfm;
  				
  			a  = getByte(tfmfile); 
  			b  = getByte(tfmfile); 
  			c  = getByte(tfmfile); 
  			d  = getByte(tfmfile); 
  			sw =(((((d * z) / 256) + (c * z)) / 256) + (b * z)) / beta;
  			if (a == 0)
  				font_info(f, k + 3).ii.CINT0 = sw;
  			else if (a == 255)
  				font_info(f, k + 3).ii.CINT0 = sw - alpha;
  			else
  				goto badtfm;
  				
  			k += 4;
  		}
  		
  		while (k < TEX_OFFSET_CHARINFO_BASE)
  		{
  			font_info(f, k).ii.CINT0 = (qint32)(readThirtyTwo(tfmfile));
    		k++;
  		}
	}
	
	k = char_base(f) + bc;
	k_param = char_attr_base(f);
	bytes_per_entry = (12 + 2 * npc) / 4 * 4;
	extra_char_bytes = bytes_per_entry - (10 + 2 * npc);
	while (k < char_attr_base(f))
	{
		if (font_level != -1)
		{
			a = (qint32)(readSixteenUnsigned(tfmfile)); 
			qw.u.B0 = a;
        	b = (qint32)(readSixteenUnsigned(tfmfile)); 
        	qw.u.B1 = b;
        	c = (qint32)(readSixteenUnsigned(tfmfile)); 
        	qw.u.B2 = c;
        	d = (qint32)(readSixteenUnsigned(tfmfile)); 
        	qw.u.B3 = d;
        	font_info(f, k).qqqq = qw;
		}
		else
		{
			a = getByte(tfmfile); 
			qw.u.B0 = a;
        	b = getByte(tfmfile); 
        	b = ((b / 16) * 256) +(b % 16); 
        	qw.u.B1 = b;
        	c = getByte(tfmfile); 
        	c = ((c / 4) * 256) +(c % 4); 
        	qw.u.B2 = c;
        	d = getByte(tfmfile); 
        	qw.u.B3 = d;
        	font_info(f, k).qqqq = qw;
		}
		
		if ((a >= nw) || 
			((b / 0x100) >= nh) || 
			((b % 0x100) >= nd) || 
			((c / 0x100) >= ni))
		{
			goto badtfm;
		}
		
		switch (c % 4)
		{
			case TEX_LIG_TAG:
				if (d >= nl)
					goto badtfm;
				break;
				
			case TEX_EXT_TAG:
				if (d >= ne)
					goto badtfm;
				break;
				
			case TEX_LIST_TAG:
				if ((d < bc) || (d > ec))
					goto badtfm;
				while (d < (k - char_base(f)))
				{
					qw = char_info(f, d);
					if (char_tag(qw) != TEX_LIST_TAG)
						goto not_found;
					d = rem_byte(qw);
				}
				if (d == (k - char_base(f)))
					goto badtfm;
not_found:
				break;
				
			default:
				break;
		}
		k++;
		if (font_level == 1)
		{
			repeat_no = (qint32)(readSixteenUnsigned(tfmfile)); 
			for (i = 0; i < npc; i++)
			{
				param = (qint32)(readSixteen(tfmfile)); 
				font_info(f, k_param).ii.CINT0 = font_info(f, ivalues_start(f) + i).ii.CINT0 + param * table_size[i];
      			k_param++;
			}
			
			for (i = 1; i <= extra_char_bytes; i++)
				getByte(tfmfile);
				
			for (i = 1; i <= repeat_no; i++)
			{
				font_info(f, k) = font_info(f, k - 1);
      			k++;
      			for (table_counter = 0; table_counter < npc; table_counter++)
      			{
      				font_info(f, k_param).ii.CINT0 = font_info(f, k_param-npc).ii.CINT0;
        			k_param++;
      			}
			}
		}
	}
	
	for (k = width_base(f); k < lig_kern_base(f); k++)
	{
		a  = getByte(tfmfile); 
  		b  = getByte(tfmfile); 
  		c  = getByte(tfmfile); 
  		d  = getByte(tfmfile); 
  		sw =(((((d * z) / 256) + (c * z)) / 256) + (b * z)) / beta;
  		if (a == 0)
  			font_info(f, k).ii.CINT0 = sw;
  		else if (a == 255)
  			font_info(f, k).ii.CINT0 = sw - alpha;
  		else
  			goto badtfm;
	}
	
	if (font_info(f, width_base(f)).ii.CINT0 != 0)
		goto badtfm;
		
	if (font_info(f, height_base(f)).ii.CINT0 != 0)
		goto badtfm;
		
	if (font_info(f, depth_base(f)).ii.CINT0 != 0)
		goto badtfm;
		
	if (font_info(f, italic_base(f)).ii.CINT0 != 0)
		goto badtfm;
		
	bch_label = nl;
	bchar = TEX_TOO_BIG_CHAR;
	if (nl > 0)
	{
		for (k = lig_kern_base(f); k < (kern_base(f) + TEX_KERN_BASE_OFFSET); k++)
		{
			if (font_level != -1)
			{
				a = (qint32)(readSixteenUnsigned(tfmfile)); 
				qw.u.B0 = a;
        		b = (qint32)(readSixteenUnsigned(tfmfile));  
        		qw.u.B1 = b;
        		c = (qint32)(readSixteenUnsigned(tfmfile)); 
        		qw.u.B2 = c;
        		d = (qint32)(readSixteenUnsigned(tfmfile));  
        		qw.u.B3 = d;
        		font_info(f, k).qqqq = qw;
			}
			else
			{
				a = getByte(tfmfile); 
				qw.u.B0 = a;
        		b = getByte(tfmfile); 
        		qw.u.B1 = b;
        		c = getByte(tfmfile);  
        		qw.u.B2 = c;
        		d = getByte(tfmfile); 
        		qw.u.B3 = d;
        		font_info(f, k).qqqq = qw;
			}
			
			if (a > 128)
			{
				if ((256 * c+ d) >= nl)
					goto badtfm;
					
				if (a == 255)
				{
					if (k == lig_kern_base(f))
						bchar = b;
				}
			}
			else
			{
				if (b != bchar)
				{
					if ((b < bc) || (b > ec))
						return g;
						
					qw = char_info(f, b + font_offset(f));
					if (!char_exists(qw))
						goto badtfm;
				}
				
				if (c < 128)
				{
					if ((d < bc) || (d > ec))
						goto badtfm;
						
					qw = char_info(f, d + font_offset(f));
					if (!char_exists(qw))
						goto badtfm;
				}
				else if ((256*(c - 128) + d) >= nk)
					goto badtfm;
					
				if (a < 128)
				{
					if ((k - lig_kern_base(f) + a + 1) >= nl)
						goto badtfm;
				}
			}
		}
		
		if (a == 255)
			bch_label = 256 * c + d;
	}
	
	for (k = (kern_base(f) + TEX_KERN_BASE_OFFSET); k < exten_base(f); k++)
	{
		a  = getByte(tfmfile); 
  		b  = getByte(tfmfile); 
  		c  = getByte(tfmfile); 
  		d  = getByte(tfmfile); 
  		sw =(((((d * z) / 256) + (c * z)) / 256) + (b * z)) / beta;
  		if (a == 0)
  			font_info(f, k).ii.CINT0 = sw;
  		else if (a == 255)
  			font_info(f, k).ii.CINT0 = sw - alpha;
  		else
  			goto badtfm;
	}
	
	for (k = exten_base(f); k < param_base(f); k++)
	{
		if (font_level != -1)
		{
			a = (qint32)(readSixteenUnsigned(tfmfile));  
			qw.u.B0 = a;
      b = (qint32)(readSixteenUnsigned(tfmfile));  
      qw.u.B1 = b;
      c = (qint32)(readSixteenUnsigned(tfmfile)); 
      qw.u.B2 = c;
      d = (qint32)(readSixteenUnsigned(tfmfile));  
      qw.u.B3 = d;
      font_info(f, k).qqqq = qw;
		}
		else
		{
			a = getByte(tfmfile); 
			qw.u.B0 = a;
      b = getByte(tfmfile); 
      qw.u.B1 = b;
      c = getByte(tfmfile);  
      qw.u.B2 = c;
      d = getByte(tfmfile); 
      qw.u.B3 = d;
      font_info(f, k).qqqq = qw;
		}
		
        if (a != 0)
        {
        	if ((a < bc) || (a > ec))
				goto badtfm;
						
			qw = char_info(f, a + font_offset(f));
			if (!char_exists(qw))
				goto badtfm;
        }
        if (b != 0)
        {
        	if ((b < bc) || (b > ec))
				goto badtfm;
						
			qw = char_info(f, b + font_offset(f));
			if (!char_exists(qw))
				goto badtfm;
        }
        if (c != 0)
        {
        	if ((c < bc) || (c > ec))
				goto badtfm;
						
			qw = char_info(f, c + font_offset(f));
			if (!char_exists(qw))
				goto badtfm;
        }
        
        if ((d < bc) || (d > ec))
			goto badtfm;
						
		qw = char_info(f, d + font_offset(f));
		if (!char_exists(qw))
			goto badtfm;
	}
	
	for (k = 1; k <= np; k++)
	{
		if (k == 1)
		{
			sw = getByte(tfmfile);
			if (sw > 127)
				sw = sw - 256;
			sw = sw * 256 + getByte(tfmfile);
			sw = sw * 256 + getByte(tfmfile);
			font_info(f, param_base(f)).ii.CINT0 = (sw * 16) +(getByte(tfmfile) / 16);
		}
		else
		{
			a  = getByte(tfmfile); 
  			b  = getByte(tfmfile); 
  			c  = getByte(tfmfile); 
  			d  = getByte(tfmfile); 
  			sw =(((((d * z) / 256) + (c * z)) / 256) + (b * z)) / beta;
  			if (a == 0)
  				font_info(f, param_base(f) + k - 1).ii.CINT0 = sw;
  			else if (a == 255)
  				font_info(f, param_base(f) + k - 1).ii.CINT0 = sw - alpha;
  			else
  				goto badtfm;
		}
	}
		
	for (k = (np + 1); k <= 7; k++)
		font_info(f, param_base(f) + k - 1).ii.CINT0 = 0;
		
	if (np >= 7)
		font_params(f) = np;
	else
		font_params(f) = 7;
		
	font_offset(f) = offset;
	hyphen_char(f) = defaultHyphenChar() + offset;
	skew_char(f)   = defaultSkewChar() + offset;
	bchar = bchar + offset;
	if (bch_label < nl)
		bchar_label(f) = bch_label + lig_kern_base(f);
	else
		bchar_label(f) = TEX_NON_ADDRESS;
	font_bchar(f) = bchar;
	font_false_bchar(f) = bchar;
	if (bchar <= ec)
	{
		if (bchar >= bc)
		{
			qw = char_info(f, bchar);
			if (char_exists(qw))
				font_false_bchar(f) = TEX_NON_CHAR;
		}
	}
	
	font_name(f) = nom;
	font_area(f) = aire;
	font_bc(f) = bc; 
	font_ec(f) = ec; 
	font_glue(f) = TEX_NULL;
	font_bc(f) = bc + offset; 
	font_ec(f) = ec + offset; 
	font_glue(f) = TEX_NULL;
	(param_base(f))--;
	font_ptr = f; 
	g = f; 
	goto done;

badtfm:
	printErr(tr("Font ")); 
    sprintCS(u);
  	printChar('='); 
  	printFileName(nom, aire, TEX_TOO_BIG_CHAR);
  	if (s >= 0)
  	{
  		print(tr(" at ")); 
  		printScaled(s); 
  		print("pt");
  	}
  	else if (s != -1000)
  	{
  		print(tr(" scaled ")); 
  		printInt(-s);
  	}
  	
  	if (file_opened)
  		print(tr(" not loadable: Bad metric (TFM/OFM) file"));
  	else
  		print(tr(" not loadable: Metric (TFM/OFM) file not found"));
  		
  	help5(tr("I wasn't able to read the size data for this font,"));
  	help_line[3] = tr("so I will ignore the font specification.");
  	help_line[2] = tr("[Wizards can fix TFM files using TFtoPL/PLtoTF.]");
  	help_line[1] = tr("You might try inserting a different font spec;");
  	help_line[0] = tr("e.g., type `I\\font<same font id>=<substitute font name>'.");
  	error();
  	
done:
	if (tfmfile)
	{
		tfmfile->close();
		delete tfmfile;
	}
	return g;
}

