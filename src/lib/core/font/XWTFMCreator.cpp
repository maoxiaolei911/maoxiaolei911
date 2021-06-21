/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWConst.h"
#include "XWUtil.h"
#include "XWApplication.h"
#include "XWNumberUtil.h"
#include "LigKern.h"
#include "XWTFMCreator.h"

#define FMG_NO_TAG 0
#define FMG_LIG_TAG 1
#define FMG_LIST_TAG 2
#define FMG_EXT_TAG 3

#define FMG_STOP_FLAG 128
#define FMG_KERN_FLAG 128

#define FMG_UNITY 1048576


#define MEM_SIZE 1032
#define MAX_LIG_STEPS 5000
#define MAX_KERNS 500
#define BYTE 256
#define BCHAR_LABEL char_remainder[256]

#define VF_SIZE 10000
#define MAX_VF_STACK 200

struct SubfontLig
{
    long sf_code;
    int position;
};

static int compare_sf(const void *a, const void *b)
{
    return (int)(((SubfontLig *)b)->sf_code - ((SubfontLig *)a)->sf_code);
}

XWTFMCreator::XWTFMCreator(const char * texnameA, QObject * parent)
    :QObject(parent)
{
    lf = 0;
    lh = 0;
    nw = 0;
    nh = 0;
    nd = 0;
    ni = 0;
    nl = 0;
    nk = 0;
    ne = 0;
    np = 0;

    nco = 0;
    ncw = 0;
    npc = 0;
    nki = 0;
    nwi = 0;
    nkf = 0;
    nwf = 0;
    nkm = 0;
    nwm = 0;
    nkr = 0;
    nwr = 0;
    nkg = 0;
    nwg = 0;
    nkp = 0;
    nwp = 0;

    header = 0;
    charInfo = 0;
    width = 0;
    height = 0;
    depth = 0;
    ligkern = 0;
    kerns = 0;
    tparam = 0;
    italic = 0;

    nextd = -1;

    char_tag = 0;
    extra_loc_needed = false;
    bchar = 256;
    lk_offset = 0;
    lig_kern = 0;
    char_remainder = 0;
    minnl = 0;

    vf = 0;
    vfPtr = 0;
    vtitleStart = 0;
    vtitleLength = 0;
    packetStart = 0;
    packetLength = 0;

    hstack = 0;
    vstack = 0;
    wstack = 0;
    xstack = 0;
    ystack = 0;
    zstack = 0;
    stackPtr = 0;
    hstackPtr = 0;
    vstackPtr = 0;

    fontPtr = 0;
    curFont = 0;
    fnameStart = 0;
    fnameLength = 0;
    fareaStart = 0;
    fareaLength = 0;
    fontNumber = 0;
    fontAt = 0;
    fontDSize = 0;

	texName = qstrdup(texnameA);
    
    fontFile = XWFontFileFT::loadTexFont(texnameA);
    if (!fontFile)
    	return ;

    fontFile->readFT();
    fontFile->readSubfontLigs();

    if (fontFile->getFontLevel() == -1 && 
    	!(fontFile->isOnlyRange()))
    {
        fontFile->handleReencoding();
        fontFile->assignChars();
        fontFile->upMap();
    }
}

XWTFMCreator::~XWTFMCreator()
{
    if (texName)
		delete [] texName;
		
    if (header)
        free(header);

    if (charInfo)
        free(charInfo);

    if (width)
        free(width);

    if (height)
        free(height);

    if (depth)
        free(depth);

    if (ligkern)
        free(ligkern);

    if (kerns)
        free(kerns);

    if (tparam)
        free(tparam);

    if (italic)
        free(italic);

    if (lig_kern)
        free(lig_kern);

    if (char_remainder)
        free(char_remainder);

    if (char_tag)
        free(char_tag);

    if (vf)
        free(vf);

    if (packetStart)
        free(packetStart);

    if (packetLength)
        free(packetLength);

    if (hstack)
        free(hstack);

    if (vstack)
        free(vstack);

    if (wstack)
        free(wstack);

    if (xstack)
        free(xstack);

    if (ystack)
        free(ystack);

    if (zstack)
        free(zstack);

    if (fnameStart)
        free(fnameStart);

    if (fnameLength)
        free(fnameLength);
        
    if (fareaStart)
        free(fareaStart);

    if (fareaLength)
        free(fareaLength);

    if (fontNumber)
        free(fontNumber);

    if (fontAt)
        free(fontAt);

    if (fontDSize)
        free(fontDSize);
        
    if (fontFile)
    	delete fontFile;
}

bool XWTFMCreator::isOFM()
{
	return (fontFile && fontFile->getFontLevel() != -1);
}

void XWTFMCreator::writeTFM(QIODevice * file)
{
	if (!fontFile)
		return ;
		
	int bc = fontFile->getBC();
	int ec = fontFile->getEC();
	int fontDir = fontFile->getFontDir();
		
    if (fontFile->getFontLevel() == -1)
    {
    	if (!fontFile->isOnlyRange())
    		buildVF();
    		
    	buildTFM();
        write16(lf, file);
        write16(lh, file);
        write16(bc, file);
        write16(ec, file);
        write16(nw, file);
        write16(nh, file);
        write16(nd, file);
        write16(ni, file);
        write16(nl, file);
        write16(nk, file);
        write16(ne, file);
        write16(np, file);
        writeArr(header, lh, file);
        writeArr(charInfo, ec - bc + 1, file);
    }
    else
    {
    	buildTFM();
        write32(0, file);
        write32(lf, file);
        write32(lh, file);
        write32(bc, file);
        write32(ec, file);
        write32(nw, file);
        write32(nh, file);
        write32(nd, file);
        write32(ni, file);
        write32(nl, file);
        write32(nk, file);
        write32(ne, file);
        write32(np, file);
        write32(fontDir, file);
        writeArr(header, lh, file);

        for (int i = bc; i <= ec; i++)
        {
            write16(charInfo[2 * (i - bc)] >> 16, file);
            write16((charInfo[2 * (i - bc)] & 0x0000ffff), file);
            write16(charInfo[2 * (i - bc) + 1] >> 16, file);
            write16((charInfo[2 * (i - bc) + 1] & 0x0000ffff), file);
        }
    }

    writeSArr(width, nw, file);
    writeSArr(height, nh, file);
    writeSArr(depth, nd, file);
    writeSArr(italic, ni, file);
    writeArr(ligkern, nl, file);
    writeSArr(kerns, nk, file);
    writeArr(tparam, np, file);
}

void XWTFMCreator::writeVF(QIODevice * file)
{
	if (!fontFile)
		return ;
		
	if (fontFile->getFontLevel() != -1 || fontFile->isOnlyRange())
		return ;
	
	buildVF();
	buildTFM();
	
	file->putChar((char)(DVI_PRE));
	file->putChar((char)(202));
	file->putChar((char)(vtitleLength));
	for (int k = 0; k < vtitleLength; k++)
		file->putChar((char)(vf[vtitleStart + k]));
		
	writeArr(header, 2, file);
	int vcount = vtitleLength + 11;
	for (curFont = 0; curFont < fontPtr; curFont++)
	{
		file->putChar((char)(DVI_FNTDEF1));
		file->putChar((char)(curFont));
		writeArr(header, 1, file);
		voutInt(fontAt[curFont], file);
		long dsize = fontFile->getDesignSizeFixed();
		voutInt(dsize, file);
		file->putChar((char)(fareaLength[curFont]));
		file->putChar((char)(fnameLength[curFont]));
		for (int k = 0; k < fareaLength[curFont]; k++)
			file->putChar((char)(vf[fareaStart[curFont] + k]));
			
		if (fnameStart[curFont] == VF_SIZE)
			file->write("NULL", 4);
		else
		{
			for (int k = 0; k < fnameLength[curFont]; k++)
				file->putChar((char)(vf[fnameStart[curFont]+k]));
		}
		
		vcount = vcount + 12 + fareaLength[curFont] + fnameLength[curFont];
	}
	
	int bc = fontFile->getBC();
	int ec = fontFile->getEC();
	
	FTFontInfo ** inencptrs = fontFile->getInEncPtrs();
	for (int c = bc; c <= ec; c++)
	{
		FTFontInfo *ti = inencptrs[c];
		if (ti && (ti->width > 0))
		{
			long x = ti->width / 1000;
			if (packetLength[c] > 241 || x < 0 || x >= 16777216)
			{
				file->putChar((char)(242));
				voutInt(packetLength[c], file); 
				voutInt(c, file); 
				voutInt(x, file);
				vcount = vcount + 13 + packetLength[c];
			}
			else
			{
				file->putChar((char)(packetLength[c]));
				file->putChar((char)c);
				file->putChar((char)(x / 65536));
				file->putChar((char)((x / 256) % 256));
				file->putChar((char)(x % 256));
				vcount = vcount + 5 + packetLength[c];
			}
			
			if (packetStart[c] == VF_SIZE)
			{
				if (c >= 128)
					file->putChar((char)DVI_SET1);
					
				file->putChar((char)c);
			}
			else
			{
				for (int k = 0; k < packetLength[c]; k++)
					file->putChar((char)(vf[packetStart[c]+k]));
			}
		}
	}
	
	do
	{
		file->putChar((char)DVI_POST);
		vcount++;
	} while (vcount % 4 == 0);
}

void XWTFMCreator::buildTFM()
{
	int bc = fontFile->getBC();
	int ec = fontFile->getEC();
	FTFontInfo ** inencptrs = fontFile->getInEncPtrs();
	
    int n = ec - bc + 1;
    header = (long*)malloc(100 * sizeof(long));
    header[0] = checkSum(inencptrs);
    header[1] = fontFile->getDesignSizeFixed();
    char * codingscheme = fontFile->getCodingScheme();
    long * p = makebcpl(header + 2, codingscheme, 39);
    char * family_name = fontFile->getFamilyName();
    p = makebcpl(header + 12, family_name, 19);
    char buffer[256];
    buildTitle(buffer);
    p = makebcpl(header + 18, buffer, 255);
    lh = p - header;

    long * tmparray = (long*)malloc(65537 * sizeof(long));
    tmparray[0] = 0;
    nw = 1;
    for (int i = bc; i <= ec; i++)
    {
        int k = i;
        if (fontFile->getFontLevel() != -1)
            k -= bc;
        FTFontInfo * ti = inencptrs[k];
        if (ti)
        {
            tmparray[nw] = ti->width;
            int j = 1;
            for (; tmparray[j] != ti->width; j++);

            ti->wptr = j;
            if (j == nw)
                nw++;
        }
    }
    width = (long*)malloc(nw * sizeof(long));
    for (int i = 0; i < nw; i++)
        width[i] = tmparray[i];

    nd = 1;
    tmparray[0] = 0;
    for (int i = bc; i <= ec; i++)
    {
        int k = i;
        if (fontFile->getFontLevel() != -1)
            k -= bc;
        FTFontInfo * ti = inencptrs[k];
        if (ti)
        {
            tmparray[nd] = -ti->lly;
            int j = 0;
            for (; tmparray[j] != -ti->lly; j++);

            ti->dptr = j;
            if (j == nd)
                nd++;
        }
    }

    int limit = 16;
    if (fontFile->getFontLevel() != -1)
        limit = 256;

    if (nd > limit)
    {
        long * source = new long[n+1];
        long * unsort = new long[n+1];
        memset(source, 0, (n+1) * sizeof(long));
        memset(unsort, 0, (n+1) * sizeof(long));
        remap(tmparray, nd, limit, source, unsort);
        for (int i = bc; i <= ec; i++)
        {
            int k = i;
            if (fontFile->getFontLevel() != -1)
                k -= bc;
            FTFontInfo * ti = inencptrs[k];
            if (ti)
                ti->dptr = unsort[ti->dptr];
        }

        nd = limit;
        delete [] source;
        delete [] unsort;
    }

    depth = (long*)malloc(nd * sizeof(long));
    for (int i = 0; i < nd; i++)
        depth[i] = tmparray[i];

    nh = 1;
    tmparray[0] = 0;
    for (int i = bc; i <= ec; i++)
    {
        int k = i;
        if (fontFile->getFontLevel() != -1)
            k -= bc;
        FTFontInfo * ti = inencptrs[k];
        if (ti)
        {
            tmparray[nh] = ti->ury;
            int j = 0;
            for (; tmparray[j] != ti->ury; j++);

            ti->hptr = j;
            if (j == nh)
                nh++;
        }
    }

    limit = 16;
    if (fontFile->getFontLevel() != -1)
        limit = 256;

    if (nh > limit)
    {
        long * source = new long[n+1];
        long * unsort = new long[n+1];
        memset(source, 0, (n+1) * sizeof(long));
        memset(unsort, 0, (n+1) * sizeof(long));
        remap(tmparray, nh, limit, source, unsort);
        for (int i = bc; i <= ec; i++)
        {
            int k = i;
            if (fontFile->getFontLevel() != -1)
                k -= bc;
            FTFontInfo * ti = inencptrs[k];
            if (ti)
                ti->hptr = unsort[ti->hptr];
        }

        nh = limit;
        delete [] source;
        delete [] unsort;
    }

    height = (long*)malloc(nh * sizeof(long));
    for (int i = 0; i < nh; i++)
        height[i] = tmparray[i];

    ni = 1;
    tmparray[0] = 0;
    for (int i = bc; i <= ec; i++)
    {
        int k = i;
        if (fontFile->getFontLevel() != -1)
            k -= bc;

        FTFontInfo * ti = inencptrs[k];
        if (ti)
        {
            tmparray[ni] = ti->urx - ti->width;
            if (tmparray[ni] < 0)
                tmparray[ni] = 0;
            int j = 0;
            for (; tmparray[j] != tmparray[ni]; j++);

            ti->iptr = j;
            if (j == ni)
                ni++;
        }
    }

    limit = 64;
    if (fontFile->getFontLevel() != -1)
        limit = 16384;

    if (ni > limit)
    {
        long * source = new long[n+1];
        long * unsort = new long[n+1];
        memset(source, 0, (n+1) * sizeof(long));
        memset(unsort, 0, (n+1) * sizeof(long));
        remap(tmparray, nh, limit, source, unsort);
        for (int i = bc; i <= ec; i++)
        {
            int k = i;
            if (fontFile->getFontLevel() != -1)
                k -= bc;
            FTFontInfo * ti = inencptrs[k];
            if (ti)
                ti->iptr = unsort[ti->iptr];
        }

        nh = limit;
        delete [] source;
        delete [] unsort;
    }

    italic = (long*)malloc(ni * sizeof(long));
    for (int i = 0; i < ni; i++)
        italic[i] = tmparray[i];

    if (fontFile->getFontLevel() == -1)
    {
        ncw = n;
        charInfo = (long*)malloc(ncw * sizeof(long));
        for (int i = bc; i <= ec; i++)
        {
            int k = i;
            FTFontInfo * ti = inencptrs[k];
            if (ti)
            {
                charInfo[i - bc] = ((long)(ti->wptr) << 24) +
                                    ((long)(ti->hptr) << 20) +
                                    ((long)(ti->dptr) << 16) +
                                    ((long)(ti->iptr) << 10);
               if (char_tag)
               {
                    int tag = char_tag[i] << 8;
                    charInfo[i - bc] += tag;
               }

               if (char_remainder)
                    charInfo[i - bc] += char_remainder[i];
            }
            else
                charInfo[i - bc] = 0;
        }
    }
    else
    {
        ncw = 2 * n;
        charInfo = (long*)malloc(ncw * sizeof(long));
        for (int i = bc; i <= ec; i++)
        {
            int k = i;
            k -= bc;
            FTFontInfo * ti = inencptrs[k];
            if (ti)
            {
                charInfo[2 * (i - bc)] = ((long)(ti->wptr) << 16) + 
                                         ((((long)(ti->hptr) & 0x00FF)) << 8) + 
                                         ((long)(ti->dptr) & 0x00FF);
                charInfo[2 * (i - bc) + 1] = (((long)(ti->iptr) & 0x00FF) << 18);
            }
            else
            {
                charInfo[2 * (i - bc)] = 0;
                charInfo[2 * (i - bc) + 1] = 0;
            }
        }
    }


    if (fontFile->hasSubfontLigs())
    {
    	long * sfCode = fontFile->getSubfontCode();
        nl = 0;
        SubfontLig sf_array[256];
        int i = 0;
        if (fontFile->getLigName())
        {
            for (i = 0; i < 256; i++)
            {
                sf_array[i].sf_code = sfCode[i];
                sf_array[i].position = (sfCode[i] == -1 ? -1 : i);
            }
        }
        else
        {
            for (i = 0; i < 256; i++)
            {
                FTFontInfo * ti = inencptrs[i];
                if (ti)
                {
                    sf_array[i].sf_code = ti->charcode;
                    sf_array[i].position = i;
                }
                else
                {
                    sf_array[i].sf_code = -1;
                    sf_array[i].position = -1;
                }
            }
        }

        qsort(sf_array, 256, sizeof(SubfontLig), compare_sf);
        i = 0;
        while (i < 256 && sf_array[i].sf_code > -1)
        {
            int byte1 = sf_array[i].sf_code >> 8;
            int byte2 = sf_array[i].sf_code & 0xFF;
            if (!inencptrs[byte1])
            {
                FTFontInfo * ti = fontFile->newChar();
                ti->llx = ti->lly = 0;
                ti->urx = ti->ury = 0;
                ti->width = 0;
                inencptrs[byte1] = ti;
                ti->incode = byte1;
                ti->adobename = qstrdup(".dummy");
            }

            if (!inencptrs[byte2])
            {
                FTFontInfo * ti = fontFile->newChar();
                ti->llx = ti->lly = 0;
                ti->urx = ti->ury = 0;
                ti->width = 0;
                inencptrs[byte2] = ti;
                ti->incode = byte2;
                ti->adobename = qstrdup(".dummy");
            }
            i++;
        }

        int old_byte1 = -1;
        while (nl < 256 && sf_array[nl].sf_code > -1)
        {
            int byte1 = sf_array[nl].sf_code >> 8;
            int byte2 = sf_array[nl].sf_code & 0xFF;
            if (byte1 != old_byte1)
            {
                charInfo[byte1 - bc] += 0x100L + nl;
                if (old_byte1 > -1)
                    tmparray[nl - 1] |= 0x80000000L;
            }

            tmparray[nl] = ((long)byte2 << 16) + (long)sf_array[nl].position;
            old_byte1 = byte1;
            nl++;
        }

        tmparray[nl - 1] |= 0x80000000L;
        ligkern = (long*)malloc(ni * sizeof(long));
        for (int i = 0; i < nl; i++)
            ligkern[i] = tmparray[i];
    }
  	
    if (vf)
        np = 7;
    else
        np = 6;
    tparam = (long*)malloc(np * sizeof(long));
    fontFile->getTFMParam(tparam, vf != 0);

    free(tmparray);

    if (fontFile->getFontLevel() == -1)
        lf = 6 + lh + ncw + nw + nh + nd + ni + nl + nk + ne + np;
    else
        lf = 14 + lh + ncw + nw + nh + nd + ni + 2 * nl + nk + 2 * ne + np;
}

void XWTFMCreator::buildTitle(char * buf)
{
	strncpy(buf, "Created by `", 12);
    strcat(buf, xwApp->getProductName8());
    strncat(buf, " ", 1);
    strcat(buf, xwApp->getVersion8());
    strncat(buf, "'", 1);
}

void XWTFMCreator::buildVF()
{
	vf = (uchar*)malloc(VF_SIZE * sizeof(uchar));
	packetStart = (int*)malloc(256 * sizeof(int));
	packetLength = (int*)malloc(256 * sizeof(int));
	
	hstack = (uchar*)malloc(MAX_VF_STACK * sizeof(uchar));
	vstack = (uchar*)malloc(MAX_VF_STACK * sizeof(uchar));
	
	wstack = (long*)malloc(MAX_VF_STACK * sizeof(long));
	xstack = (long*)malloc(MAX_VF_STACK * sizeof(long));
	ystack = (long*)malloc(MAX_VF_STACK * sizeof(long));
	zstack = (long*)malloc(MAX_VF_STACK * sizeof(long));
	
	fnameStart = (int*)malloc(256 * sizeof(int));
	fnameLength = (uchar*)malloc(256 * sizeof(uchar));
	fareaStart = (int*)malloc(256 * sizeof(int));
	fareaLength = (uchar*)malloc(256 * sizeof(uchar));
	fontAt = (long*)malloc(256 * sizeof(long));
	fontDSize = (long*)malloc(256 * sizeof(long));
	
	fontNumber = (FontNumber*)malloc(257 * sizeof(FontNumber));
	
	for (int i = 0; i < 256; i++)
		packetStart[i] = VF_SIZE;
		
	for (int i = 0; i < 128; i++)
		packetLength[i] = 1;
		
	for (int i = 128; i < 256; i++)
		packetLength[i] = 2;
		
	char_tag = (int *)(BYTE * sizeof(int));
   	char_remainder = (ulong*)malloc(257 * sizeof(ulong));
    kerns = (long*)malloc(MAX_KERNS * sizeof(long));
    BCHAR_LABEL = 32767;
    for (int i = 0; i < BYTE; i++)
    {
    	char_tag[i] = FMG_NO_TAG;
        char_remainder[i] = 0;
    }

    lig_kern = (LigKern*)malloc(MAX_LIG_STEPS * sizeof(LigKern));
    		
	vtitleStep();
	short boundaryChar = fontFile->getBoundaryChar();
	if (boundaryChar >= 0)
		bchar = boundaryChar;
		
	mapFontStep(0);
	fontNameStep(texName);
	double capHeight = fontFile->getCapHeight();
	if (fontFile->isSmallCaps())
	{
		mapFontStep(1);
		fontNameStep(texName);
		fontAtStep(1000.0 * capHeight + 0.5);
	}
		
	FTFontInfo ** outencptrs = fontFile->getOutEncPtrs();
	FTFontInfo ** inencptrs = fontFile->getInEncPtrs();
	
	int bc = 0;
	int ec = 0;
    int i = 0;
    for (; i <= 0xFF && outencptrs[i] == NULL; i++);
    bc = i;
    for (i = 0xFF; i >= 0 && outencptrs[i] == NULL; i--);
    ec = i;
    
    short * nextout = fontFile->getNextOut();
    FTFontInfo ** lowercase = fontFile->getLowerCase();
    FTFontInfo ** uppercase = fontFile->getUpperCase();
    
    FTFontInfo * asucc = 0;
    FTFontInfo * asub = 0;
    FTFontInfo * ti = fontFile->findAdobe("||");
    bool unlabeled = true;
    Lig * nlig = ti->ligs;
    for (; nlig; nlig = nlig->next)
    {
        if (0 != (asucc = fontFile->findMappedAdobe(nlig->succ, inencptrs)))
        {
            if (0 != (asub = fontFile->findMappedAdobe(nlig->sub, inencptrs)))
            {
                if (asucc->outcode >= 0)
                {
                    if (asub->outcode >= 0)
                    {
                        if (unlabeled)
                        {
                            BCHAR_LABEL = nl;
                            unlabeled = false;
                            if (minnl <= nl)
                                minnl = nl + 1;
                        }

                        for (int j = asucc->outcode; j >= 0; j = nextout[j])
                            ligStep(vplligops[nlig->op], j, asub->outcode);
                    }
                }
            }
        }
    }

    if (!unlabeled)
        stopStep();

    Kern * nkern = 0;
    FTFontPtr * kern_eq = 0;
    for (int i = bc; i <= ec; i++)
    {
        if ((ti = outencptrs[i]) && ti->outcode == i)
        {
            unlabeled = true;
            if (uppercase[i] == 0)
            {
                for (nlig = ti->ligs; nlig; nlig = nlig->next)
                {
                    if ((((0 != (asucc = fontFile->findMappedAdobe(nlig->succ, inencptrs))) &&
                        (asucc->outcode >= 0)) ||
                        ((strcmp(nlig->succ, "||") == 0) &&
                        (boundaryChar >= 0))) &&
                        (0 != (asub = fontFile->findMappedAdobe(nlig->sub, inencptrs))) && (asub->outcode >= 0))
                    {
                        if (unlabeled)
                        {
                            for (int j = ti->outcode; j >= 0; j = nextout[j])
                                labelStep(j, false);
                            unlabeled = false;
                        }

                        if (asucc != 0)
                        {
                            for (int j = asucc->outcode; j >= 0; j = nextout[j])
                            {
                                ligStep(vplligops[nlig->op], j, asub->outcode);
                                if (nlig->boundleft)
                                    break;
                            }
                        }
                        else
                            ligStep(vplligops[nlig->op], boundaryChar, asub->outcode);
                    }
                }
            }

            for (nkern = (uppercase[i] ? uppercase[i]->kerns : ti->kerns); nkern; nkern = nkern->next)
            {
                if (0 != (asucc = fontFile->findMappedAdobe(nkern->succ, inencptrs)))
                {
                    for (int j = asucc->outcode; j >= 0; j = nextout[j])
                    {
                        if (uppercase[j] == 0)
                        {
                            if (unlabeled)
                            {
                                for (int k = ti->outcode; k >= 0; k = nextout[k])
                                    labelStep(k, false);
                                unlabeled = false;
                            }

                            for (kern_eq = ti->kern_equivs; kern_eq; kern_eq = kern_eq->next)
                            {
                                int k = kern_eq->ch->outcode;
                                if (k >= 0 && k <= 0xFF)
                                    labelStep(k, false);
                            }

                            if (ti->rptrs && ti->kern_equivs)
                                fontFile->releaseFTFontPtr(ti->kern_equivs);
                            ti->kern_equivs = 0;

                            if (uppercase[i])
                            {
                                if (lowercase[j])
                                {
                                    for (int k = lowercase[j]->outcode; k >= 0; k = nextout[k])
                                        kernStep(k, capHeight * nkern->delta);
                                }
                                else
                                    kernStep(j, capHeight * nkern->delta);
                            }
                            else
                            {
                                kernStep(j, nkern->delta);
                                if (lowercase[j])
                                {
                                    for (int k = lowercase[j]->outcode; k >= 0; nextout[k])
                                        kernStep(k, capHeight * nkern->delta);
                                }
                            }
                        }
                    }
                }
            }

            if (!unlabeled)
                stopStep();
        }
    }

    for (int i = bc; i <= ec; i++)
    {
    	ti = outencptrs[i];
    	if (!ti)
    		continue;
    		
        if (uppercase[i])
        {
        	ti = uppercase[i];
        	ti->width = (long)(capHeight * ti->width);
        	ti->ury = (long)(capHeight * ti->ury);
        	ti->lly = (long)(capHeight * ti->lly);
        	ti->urx = (long)(capHeight * ti->urx);
        }
        
        if (ti->incode != i || uppercase[i] || ti->constructed)
        {
        	mapStep((uchar)i);
        	if (uppercase[i])
        		selectFontStep(1);
        		
        	if (ti->pccs && (ti->incode < 0 || ti->constructed))
        	{
        		long xoff = 0;
          		long yoff = 0;
          		
          		Pcc * npcc = ti->pccs;
          		for (; npcc; npcc = npcc->next)
          		{
          			FTFontInfo * api = fontFile->findMappedAdobe(npcc->partname, inencptrs);
          			if (api)
          			{
          				if (api->outcode >= 0)
          				{
          					if (npcc->xoffset != xoff)
          					{
          						if (uppercase[i])
          							moveRightStep(capHeight * (npcc->xoffset - xoff));
          						else
          							moveRightStep(npcc->xoffset - xoff);
          							
          						xoff = npcc->xoffset;
          					}
          				}
          				
          				if (npcc->yoffset != yoff)
          				{
          					if (uppercase[i])
          						moveUpStep(capHeight * (npcc->yoffset - yoff));
          					else
          						moveUpStep(npcc->yoffset - yoff);
          						
          					yoff = npcc->yoffset;
          				}
          				
          				setCharStep(api->incode);
          				xoff += outencptrs[api->outcode]->width;
          			}
          		}
        	}
        	else
        		setCharStep(ti->incode);
        		
        	mapEndStep((uchar)i);
        }
    }
    
    corrAndCheck();
    
    for (int i = 0; i < 256; i++)
    	inencptrs[i] = outencptrs[i];
	
	Label * label_table = (Label*)malloc(257 * sizeof(Label));
	int label_ptr = 0;
	int sort_ptr = 0;
	label_table[0].rr = -1;
    for (int c = bc; c <= ec; c++)
    {
    	if (char_tag[c] == FMG_LIG_TAG)
    	{
    		sort_ptr = label_ptr;
    		while (label_table[sort_ptr].rr > (long)(char_remainder[c]))
    		{
    			label_table[sort_ptr + 1] = label_table[sort_ptr];
    			sort_ptr--;
    		}
    		
    		label_table[sort_ptr+1].cc = c;
    		label_table[sort_ptr+1].rr = char_remainder[c];
    		label_ptr++;
    	}
    }
    
    if (bchar < 256)
	{
		extra_loc_needed = true; 
		lk_offset = 1;
	}
	else
	{
		extra_loc_needed = false; 
		lk_offset = 0;
	}
	
	sort_ptr = label_ptr;
	if (label_table[sort_ptr].rr + lk_offset > 255)
	{
		lk_offset = 0; 
		extra_loc_needed = false;
		do
		{
			char_remainder[label_table[sort_ptr].cc] = lk_offset;
			while (label_table[sort_ptr-1].rr == label_table[sort_ptr].rr)
			{
				sort_ptr--; 
				char_remainder[label_table[sort_ptr].cc] = lk_offset;
			}
			lk_offset++; 
			sort_ptr--;
		} while (lk_offset + label_table[sort_ptr].rr < 256);
	}
	
	if (lk_offset > 0)
	{
		while (sort_ptr > 0)
		{
			char_remainder[label_table[sort_ptr].cc] = char_remainder[label_table[sort_ptr].cc]+lk_offset;
  			sort_ptr--;
		}
	}
	
	ligkern = (long*)malloc((nl + lk_offset) * sizeof(long));
	if (extra_loc_needed)
	{
		ligkern = (long*)malloc((nl + 1) * sizeof(long));
		ligkern[0] = (255 << 24) + (bchar << 16) + 0x0000;
	}
	else
	{
		int noffset = 0;
		for (sort_ptr = 1; sort_ptr <= lk_offset; sort_ptr++)
		{
			long t = label_table[label_ptr].rr;
			if (bchar < 256)
				ligkern[noffset] = (255 << 24) + (bchar << 16) + t+lk_offset;
			else
				ligkern[noffset] = (254 << 24) + (0 << 16) + t+lk_offset;
				
			noffset++;
			do
			{
				label_ptr--;
			}while (label_table[label_ptr].rr < t);
		}
	}
	
	if (nl > 0)
	{
    	for (int i = 0; i < nl; i++)
    	{
        	ligkern[i + lk_offset] = (lig_kern[i].b0 << 24) +
            	          			(lig_kern[i].b1 << 16) +
                	      			(lig_kern[i].b2 << 8) + lig_kern[i].b3;
    	}
    }
	
	free(label_table);
    free(lig_kern);
    lig_kern = 0;
    nl += lk_offset;
    
    if (nk < MAX_KERNS)
        kerns = (long*)realloc(kerns, nk * sizeof(long));
}

long XWTFMCreator::checkSum(FTFontInfo ** array)
{
    ulong s1 = 0;
    ulong s2 = 0;
    char * p = 0;
    int bc = fontFile->getBC();
    int ec = fontFile->getEC();
    for (int i = bc; i <= ec; i++)
    {
        int k = i;
        if (isOFM())
            k -= bc;

        FTFontInfo * ti = array[k];
        if (ti)
        {
            s1 = ((s1 << 1) ^ (s1 >> 31)) ^ ti->width;
            s1 &= 0xFFFFFFFF;
            for (p = ti->adobename; *p; p++)
                s2 = (s2 * 3) + *p;
        }
    }

    s1 = (s1 << 1) ^ s2;
    return (long)s1;
}

long * XWTFMCreator::makebcpl(long *p,
                              char *s,
                              int n)
{
    if (strlen(s) < (ulong)n)
        n = strlen(s);
    long t = ((long)n) << 24;
    long sc = 16;
    while (n > 0)
    {
        t |= ((long)(*(unsigned char *)s++)) << sc;
        sc -= 8;
        if (sc < 0)
        {
            *p++ = t;
            t = 0;
            sc = 24;
        }
        n--;
    }
    if (t)
        *p++ = t;

    return p;
}

int XWTFMCreator::mincover(long *what, long d)
{
    nextd = 0x7FFFFFFFL;
    long * p = what+1;
    long m = 1;
    long l = 0;

    while (*p < 0x7FFFFFFFL)
    {
        m++;
        l = *p;
        while (*++p <= l + d);
        if (*p - l < nextd)
            nextd = *p - l;
    }
    return m;
}

void XWTFMCreator::remap(long *what,
                         int oldn,
                         int newn,
                         long *source,
                         long *unsort)
{
    what[oldn] = 0x7FFFFFFFL;
    long i = oldn - 1;
    long d = 0;
    long j = 0;
    long l = 0;
    for (; i > 0; i--)
    {
        d = what[i];
        for (j = i; what[j+1] < d; j++)
        {
            what[j] = what[j+1];
            source[j] = source[j+1];
        }
        what[j] = d;
        source[j] = i;
    }

    i = mincover(what, 0L);
    d = nextd;
    while (mincover(what, d + d) > newn)
        d += d;
    while (mincover(what, d) > newn)
        d = nextd;

    i = 1;
    j = 0;
    while (i < oldn)
    {
        j++;
        l = what[i];
        unsort[source[i]] = j;
        while (what[++i] <= l + d)
        {
            unsort[source[i]] = j;
            if (i - j == oldn - newn)
                d = 0;
        }
        what[j] = (l + what[i-1])/2;
    }
}

void XWTFMCreator::writeArr(long *p, int n, QIODevice * file)
{
    while (n)
    {
        write16((short)(*p >> 16), file);
        write16((short)(*p & 65535), file);
        p++;
        n--;
    }
}

void XWTFMCreator::writeSArr(long *what, int len, QIODevice * file)
{
    long * p = what;
    int i = len;
    while (i)
    {
        *p = fontFile->scale(*p);
        fontFile->scale(*p);
        p++;
        i--;
    }
    writeArr(what, len, file);
}

void XWTFMCreator::write16(short what, QIODevice * file)
{
    file->putChar((char)(what >> 8));
    file->putChar((char)(what & 0xFF));
}

void XWTFMCreator::write32(long what, QIODevice * file)
{
    file->putChar((char)(what / 16777216));
    file->putChar((char)((what % 16777216) / 65536));
    file->putChar((char)((what % 65536) / 256));
    file->putChar((char)(what % 256));
}

void XWTFMCreator::corrAndCheck()
{
    if (nl > 0)
    {
        if (BCHAR_LABEL < 32767)
        {
            lig_kern[nl].b0 = 255;
            lig_kern[nl].b1 = 0;
            lig_kern[nl].b2 = 0;
            lig_kern[nl].b3 = 0;
            nl++;
        }

        while (minnl > nl)
        {
            lig_kern[nl].b0 = 255;
            lig_kern[nl].b1 = 0;
            lig_kern[nl].b2 = 0;
            lig_kern[nl].b3 = 0;
            nl++;
        }

        if (lig_kern[nl-1].b0 == 0)
            lig_kern[nl-1].b0 = FMG_STOP_FLAG;
    }

    for (int c = 0; c < 256; c++)
    {
        if (char_tag[c] == FMG_LIST_TAG)
        {
            ulong g = char_remainder[c];
            while (g < (ulong)c && char_tag[g] == FMG_LIST_TAG)
                g = char_remainder[g];

            if (g == (ulong)c)
                char_tag[c] = FMG_NO_TAG;
        }
    }
}

void XWTFMCreator::fontAtStep(double s)
{
	if (!vf)
		return ;
		
	fontAt[curFont] = toFixWord(s) / 1000;
}

void XWTFMCreator::fontNameStep(char * name)
{
	if (!vf)
		return ;
		
	fnameStart[curFont] = vfPtr;
	int len = strlen(name);
	strncpy((char*)(vf + vfPtr), name, len);
    vfPtr += len;
    fnameLength[curFont] = vfPtr - fnameStart[curFont];
    fareaStart[curFont] = vfPtr;
    fareaLength[curFont] = vfPtr - fareaStart[curFont];
}

void XWTFMCreator::kernStep(int c, double k)
{
    lig_kern[nl].b0 = 0;
    lig_kern[nl].b1 = c & 0xff;
    kerns[nk] = toFixWord(k);
    int krn_ptr = 0;
    while (kerns[krn_ptr] != kerns[nk])
        krn_ptr++;

    if (krn_ptr == nk)
    {
        if (nk < MAX_KERNS)
            nk++;
        else
            krn_ptr--;
    }

    lig_kern[nl].b2 = FMG_KERN_FLAG + (krn_ptr / 256);
    lig_kern[nl].b3 = krn_ptr % 256;
    if (nl < (MAX_LIG_STEPS - 1))
        nl++;
}

void XWTFMCreator::labelStep(int c, bool b)
{
	if (b)
	{
		BCHAR_LABEL = nl;
	}
	else
	{
    	char_tag[c] = FMG_LIG_TAG;
    	char_remainder[c] = nl;
    }
    if (minnl <= nl)
        minnl = nl + 1;
}

void XWTFMCreator::ligStep(char * opstr, int c1, int c2)
{
    lig_kern[nl].b0 = 0;
    if (!strcmp(opstr, "LIG"))
        lig_kern[nl].b2 = 0;
    else if (!strcmp(opstr, "/LIG>"))
        lig_kern[nl].b2 = 6;
    else if (!strcmp(opstr, "LIG/>"))
        lig_kern[nl].b2 = 5;
    else if (!strcmp(opstr, "/LIG/>>"))
        lig_kern[nl].b2 = 11;
    else if (!strcmp(opstr, "LIG/"))
        lig_kern[nl].b2 = 1;
    else if (!strcmp(opstr, "/LIG/>"))
        lig_kern[nl].b2 = 7;
    else if (!strcmp(opstr, "/LIG"))
        lig_kern[nl].b2 = 2;
    else if (!strcmp(opstr, "/LIG/"))
        lig_kern[nl].b2 = 3;

    lig_kern[nl].b1 = c1 & 0xff;
    lig_kern[nl].b3 = c2 & 0xff;
    nl++;
}

void XWTFMCreator::mapEndStep(int c)
{
	while (stackPtr > 0)
	{
		vfStore(DVI_POP); 
		stackPtr--;
	}
	
	packetLength[c] = vfPtr - packetStart[c];
}

void XWTFMCreator::mapFontStep(int c)
{
	if (!vf)
		return ;
		
    fontNumber[fontPtr].b0 = c;
    fontNumber[fontPtr].b1 = 0;
    fontNumber[fontPtr].b2 = 0;
    fontNumber[fontPtr].b3 = 0;
    curFont = 0;
    while (numbersDiffer())
        curFont++;

    fontPtr++;
    fnameStart[curFont] = VF_SIZE;
    fnameLength[curFont] = 4;
    fareaStart[curFont] = VF_SIZE;
    fareaLength[curFont] = 0;
    fontAt[curFont] = 1048576;
    fontDSize[curFont] = 10485760;
}

void XWTFMCreator::mapStep(uchar c)
{
	if (!vf)
		return ;
		
    packetStart[c] = vfPtr; 
    stackPtr = 0; 
    hstackPtr = 0; 
    vstackPtr = 0;
    curFont = 0;
}

void XWTFMCreator::moveRightStep(double num)
{
	if (!vf)
		return ;
		
    long x = toFixWord(num);
    if (hstackPtr == 0)
    {
        wstack[stackPtr] = x;
        hstackPtr = 1;
        vfFix((uchar)DVI_W1, x);
    }
    else if (x == wstack[stackPtr])
        vfStore((uchar)DVI_W0);
    else if (hstackPtr == 1)
    {
        xstack[stackPtr] = x;
        hstackPtr = 2;
        vfFix(DVI_X1, x);
    }
    else if (x == xstack[stackPtr])
        vfStore((uchar)DVI_X0);
    else
        vfFix((uchar)DVI_RIGHT1, x);
}

void XWTFMCreator::moveUpStep(double num)
{
	if (!vf)
		return ;
		
    long x = -toFixWord(num);
    if (vstackPtr == 0)
    {
        ystack[stackPtr] = x;
        vstackPtr = 1;
        vfFix((uchar)DVI_Y1, x);
    }
    else if (x == ystack[stackPtr])
        vfStore((uchar)DVI_Y0);
    else if (vstackPtr == 1)
    {
        zstack[stackPtr] = x;
        vstackPtr = 2;
        vfFix((uchar)DVI_Z1, x);
    }
    else if (x == zstack[stackPtr])
        vfStore((uchar)DVI_Z0);
    else
        vfFix((uchar)DVI_DOWN1, x);
}

void XWTFMCreator::setCharStep(uchar cc)
{
	if (!vf)
		return ;
		
    if (cc >= 128)
        vfStore((uchar)DVI_SET1);

    vfStore(cc);
}

void XWTFMCreator::selectFontStep(uchar c)
{
	if (!vf)
		return ;
		
    fontNumber[fontPtr].b0 = c;
    fontNumber[fontPtr].b1 = 0;
    fontNumber[fontPtr].b2 = 0;
    fontNumber[fontPtr].b3 = 0;
    curFont = 0;
    while (numbersDiffer())
        curFont++;

    if (curFont < 64)
        vfStore((uchar)(DVI_FNTNUM0 + curFont));
    else
    {
        vfStore((uchar)DVI_FNT1);
        vfStore((uchar)curFont);
    }
}

void XWTFMCreator::stopStep()
{
    lig_kern[nl - 1].b0 = FMG_STOP_FLAG;
}

void XWTFMCreator::vfFix(uchar opcode, long num)
{
    long x = qRound((double)num / 1000.0);
    bool negative = false;
    if (x < 0)
    {
        negative = true;
        x = -1 - x;
    }

    int k = 0;
    int t = 0;
    if (opcode == 0)
    {
        k = 4;
        t = 16777216;
    }
    else
    {
        t = 127;
        k = 1;
        while (x > t)
        {
            t = 256 * t + 255;
            k++;
            vfStore((uchar)(opcode + k - 1));
            t = t / 128 + 1;
        }
    }

    do
    {
        if (negative)
        {
            vfStore((uchar)(255 - (x / t)));
            negative = false;
            x = (x / t) * t + t - 1 - x;
        }
        else
            vfStore((uchar)((x / t) % 256));

        k--;
        t = t / 256;
    } while (k == 0);
}

void XWTFMCreator::vfStore(uchar c)
{
    if (vfPtr < VF_SIZE - 1)
        vf[vfPtr++] = c;
}

void XWTFMCreator::voutInt(long x, QIODevice * file)
{
	if (x >= 0)
		file->putChar((char)(x / 16777216));
	else
	{
		file->putChar((char)(255));
		x = x + 16777216;
	}
	
	file->putChar((char)((x / 65536) % 256));
	file->putChar((char)((x / 256) % 256));
	file->putChar((char)(x % 256));
}

void XWTFMCreator::vtitleStep()
{
	if (!vf)
		return ;
		
    vtitleStart = vfPtr;
    char buffer[256];
    buildTitle(buffer);
    int len = strlen(buffer);
    strncpy((char*)(vf + vfPtr), buffer, len);
    vfPtr += len;
    vtitleLength = vfPtr - vtitleStart;
}

