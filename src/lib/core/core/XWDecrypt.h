/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDECRYPT_H
#define XWDECRYPT_H

#include "XWString.h"
#include "XWObject.h"
#include "XWStream.h"


class XW_CORE_EXPORT XWDecrypt 
{
public:
  static bool makeFileKey(int encVersion, 
                          int encRevision, 
                          int keyLength,
			   			  XWString *ownerKey, 
			   			  XWString *userKey,
			   			  XWString *ownerEnc, 
			   			  XWString *userEnc,
			   			  int permissions, 
			   			  XWString *fileID,
			   			  XWString *ownerPassword, 
			   			  XWString *userPassword,
			   			  uchar   *fileKey, 
			   			  bool encryptMetadata,
			   			  bool *ownerPasswordOk);

private:

  static bool makeFileKey2(int encVersion, 
                           int encRevision, 
                           int keyLength,
			    		   XWString *ownerKey, 
			    		   XWString *userKey,
			               int permissions, 
			               XWString *fileID,
			    		   XWString *userPassword, 
			    		   uchar *fileKey,
			    		   bool encryptMetadata);
};



struct DecryptRC4State {
  uchar state[256];
  uchar x, y;
  int buf;
};

struct DecryptAESState {
  uint w[44];
  uchar state[16];
  uchar cbc[16];
  uchar buf[16];
  int bufIdx;
};

struct DecryptAES256State {
  uint w[60];
  uchar state[16];
  uchar cbc[16];
  uchar buf[16];
  int bufIdx;
};


class XW_CORE_EXPORT XWDecryptStream: public XWFilterStream 
{
public:
  	XWDecryptStream(XWStream *strA, 
  	                uchar *fileKey,
					CryptAlgorithm algoA, 
					int keyLength,
					int objNum, 
					int objGen);
  	virtual ~XWDecryptStream();
  
  	virtual int getChar();
  	virtual int getKind() { return STREAM_WEIRD; }
  	virtual XWStream *getUndecodedStream() { return this; }
  	
  	virtual bool isBinary(bool last);
  
  	virtual int lookChar();
  
  	virtual void reset();

private:
  	CryptAlgorithm algo;
  	int objKeyLength;
  	uchar objKey[32];

  	union 
  	{
    	DecryptRC4State rc4;
    	DecryptAESState aes;
    	DecryptAES256State aes256;
  	} state;
};

extern void rc4InitKey(uchar *key, int keyLen, uchar *state);
extern uchar rc4DecryptByte(uchar *state, uchar *x, uchar *y, uchar c);
extern void md5(uchar *msg, int msgLen, uchar *digest);

#endif // XWDECRYPT_H

