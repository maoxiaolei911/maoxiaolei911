/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPDFXREF_H
#define XWPDFXREF_H

#include "XWRef.h"

class XWDict;
class XWStream;
class XWPDFParser;
class XWPDFObjectStream;

enum XRefEntryType 
{
	xrefEntryFree,
  	xrefEntryUncompressed,
  	xrefEntryCompressed
};

struct XWPDFXRefEntry 
{
    uint offset;
    int gen;
    XRefEntryType type;
};

class XW_PDF_EXPORT XWPDFXRef : public XWRef
{
public:   
    XWPDFXRef(XWBaseStream *strA, bool repair = false);
    ~XWPDFXRef();
    
    XWObject * fetch(int num, int gen, XWObject *obj, int recursion = 0);
    
    XWObject * getCatalog(XWObject * obj) { return fetch(rootNum, rootGen, obj); }
    XWObject * getDocInfo(XWObject *obj);
    XWObject * getDocInfoNF(XWObject *obj);
    XWPDFXRefEntry *getEntry(int i) { return &entries[i]; }
    uint getLastXRefPos() { return lastXRefPos; }
    int   getNumObjects() { return last + 1; }
    int getPermFlags() { return permFlags; }
    int   getRootGen() { return rootGen; }
    int   getRootNum() { return rootNum; }    
    int getSize() { return size; }
    bool  getStreamEnd(uint streamStart, uint *streamEnd);
    XWObject * getTrailerDict() { return &trailerDict; }
    
    bool isEncrypted() { return encrypted; }
    bool isOk() { return ok; }
    
    bool okToAddNotes(bool ignoreOwnerPW = false);
    bool okToChange(bool ignoreOwnerPW = false);
    bool okToCopy(bool ignoreOwnerPW = false);    
    bool okToPrint(bool ignoreOwnerPW = false);
    
    void setEncryption(int permFlagsA, 
                       bool ownerPasswordOkA,
		               uchar *fileKeyA, 
		               int keyLengthA, 
		               int encVersionA,
		               CryptAlgorithm encAlgorithmA);
    
protected:
    bool constructXRef();
    
    uint getStartXref();
    
    bool readXRef(uint *pos);
    bool readXRefStream(XWStream *xrefStr, uint *pos);
    bool readXRefStreamSection(XWStream *xrefStr, 
                               int *w, 
                               int first, 
                               int n);
    bool readXRefTable(XWPDFParser *parser, uint *pos);
    
protected:
    XWBaseStream *str;
    uint start;
    XWPDFXRefEntry *entries;
    int size;
    int last;
    int rootNum, rootGen;
    bool ok;
    XWObject trailerDict;
    uint lastXRefPos;
    uint *streamEnds;
    int streamEndsLen;
    XWPDFObjectStream *objStr;
    bool encrypted;
    int permFlags;
    bool ownerPasswordOk;
    uchar fileKey[16];
    int keyLength;
    int encVersion;
    CryptAlgorithm encAlgorithm;
};

#endif // XWPDFXREF_H

