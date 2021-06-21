/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include "XWDecrypt.h"



static void aesKeyExpansion(DecryptAESState *s,
			    uchar *objKey, int objKeyLen);
static void aesDecryptBlock(DecryptAESState *s, uchar *in, bool last);
static void aes256KeyExpansion(DecryptAES256State *s,
			       uchar *objKey, int objKeyLen);
static void aes256DecryptBlock(DecryptAES256State *s, uchar *in, bool last);
static void sha256(uchar *msg, int msgLen, uchar *hash);

static uchar passwordPad[32] = {
  0x28, 0xbf, 0x4e, 0x5e, 0x4e, 0x75, 0x8a, 0x41,
  0x64, 0x00, 0x4e, 0x56, 0xff, 0xfa, 0x01, 0x08, 
  0x2e, 0x2e, 0x00, 0xb6, 0xd0, 0x68, 0x3e, 0x80, 
  0x2f, 0x0c, 0xa9, 0xfe, 0x64, 0x53, 0x69, 0x7a
};

bool XWDecrypt::makeFileKey(int encVersion, 
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
			   			  					bool *ownerPasswordOk)
{
	DecryptAES256State state;
  uchar test[127 + 56], test2[32];
  XWString *userPassword2;
  uchar fState[256];
  uchar tmpKey[16];
  uchar fx, fy;
  int len, i, j;

  *ownerPasswordOk = false;

  if (encRevision == 5) 
  {

    // check the owner password
    if (ownerPassword) 
    {
      //~ this is supposed to convert the password to UTF-8 using "SASLprep"
      len = ownerPassword->getLength();
      if (len > 127) 
      {
				len = 127;
      }
      memcpy(test, ownerPassword->getCString(), len);
      memcpy(test + len, ownerKey->getCString() + 32, 8);
      memcpy(test + len + 8, userKey->getCString(), 48);
      sha256(test, len + 56, test);
      if (!memcmp(test, ownerKey->getCString(), 32)) 
      {

				// compute the file key from the owner password
				memcpy(test, ownerPassword->getCString(), len);
				memcpy(test + len, ownerKey->getCString() + 40, 8);
				memcpy(test + len + 8, userKey->getCString(), 48);
				sha256(test, len + 56, test);
				aes256KeyExpansion(&state, test, 32);
				for (i = 0; i < 16; ++i) 
				{
	  			state.cbc[i] = 0;
				}
				aes256DecryptBlock(&state, (uchar *)ownerEnc->getCString(), false);
				memcpy(fileKey, state.buf, 16);
				aes256DecryptBlock(&state, (uchar *)ownerEnc->getCString() + 16, false);
				memcpy(fileKey + 16, state.buf, 16);

				*ownerPasswordOk = true;
				return true;
      }
    }

    // check the user password
    if (userPassword) 
    {
      //~ this is supposed to convert the password to UTF-8 using "SASLprep"
      len = userPassword->getLength();
      if (len > 127) 
      {
				len = 127;
      }
      memcpy(test, userPassword->getCString(), len);
      memcpy(test + len, userKey->getCString() + 32, 8);
      sha256(test, len + 8, test);
      if (!memcmp(test, userKey->getCString(), 32)) 
      {

				// compute the file key from the user password
				memcpy(test, userPassword->getCString(), len);
				memcpy(test + len, userKey->getCString() + 40, 8);
				sha256(test, len + 8, test);
				aes256KeyExpansion(&state, test, 32);
				for (i = 0; i < 16; ++i) 
				{
	  			state.cbc[i] = 0;
				}
				aes256DecryptBlock(&state, (uchar *)userEnc->getCString(), false);
				memcpy(fileKey, state.buf, 16);
				aes256DecryptBlock(&state, (uchar *)userEnc->getCString() + 16, false);
				memcpy(fileKey + 16, state.buf, 16);

				return true; 
      }
    }

    return false;
  } 
  else 
  {

    // try using the supplied owner password to generate the user password
    if (ownerPassword) 
    {
      len = ownerPassword->getLength();
      if (len < 32) 
      {
				memcpy(test, ownerPassword->getCString(), len);
				memcpy(test + len, passwordPad, 32 - len);
      } 
      else 
      {
				memcpy(test, ownerPassword->getCString(), 32);
      }
      md5(test, 32, test);
      if (encRevision == 3) 
      {
				for (i = 0; i < 50; ++i) 
				{
	  			md5(test, keyLength, test);
				}
      }
      if (encRevision == 2) 
      {
				rc4InitKey(test, keyLength, fState);
				fx = fy = 0;
				for (i = 0; i < 32; ++i) 
				{
	  			test2[i] = rc4DecryptByte(fState, &fx, &fy, ownerKey->getChar(i));
				}
      } 
      else 
      {
				memcpy(test2, ownerKey->getCString(), 32);
				for (i = 19; i >= 0; --i) 
				{
	  			for (j = 0; j < keyLength; ++j) 
	  			{
	    			tmpKey[j] = test[j] ^ i;
	  			}
	  			rc4InitKey(tmpKey, keyLength, fState);
	  			fx = fy = 0;
	  			for (j = 0; j < 32; ++j) 
	  			{
	    			test2[j] = rc4DecryptByte(fState, &fx, &fy, test2[j]);
	  			}
				}
      }
      userPassword2 = new XWString((char *)test2, 32);
      if (makeFileKey2(encVersion, encRevision, keyLength, ownerKey, userKey,
		       permissions, fileID, userPassword2, fileKey,
		       encryptMetadata)) 
		  {
				*ownerPasswordOk = true;
				delete userPassword2;
				return true;
      }
      delete userPassword2;
    }

    // try using the supplied user password
    return makeFileKey2(encVersion, encRevision, keyLength, ownerKey, userKey,
			permissions, fileID, userPassword, fileKey,
			encryptMetadata);
  }
}

bool XWDecrypt::makeFileKey2(int, 
                             int encRevision, 
                             int keyLength,
			    		     XWString *ownerKey, 
			    		     XWString *userKey,
			                 int permissions, 
			                 XWString *fileID,
			    		     XWString *userPassword, 
			    		     uchar *fileKey,
			    		     bool encryptMetadata)
{
	uchar * buf = new uchar[72 + fileID->getLength()];
	int len = 0;
  	if (userPassword) 
  	{
    	len = userPassword->getLength();
    	if (len < 32) 
    	{
      		memcpy(buf, userPassword->getCString(), len);
      		memcpy(buf + len, passwordPad, 32 - len);
    	} 
    	else 
      		memcpy(buf, userPassword->getCString(), 32);
  	} 
  	else 
    	memcpy(buf, passwordPad, 32);
    	
    memcpy(buf + 32, ownerKey->getCString(), 32);
  	buf[64] = permissions & 0xff;
  	buf[65] = (permissions >> 8) & 0xff;
  	buf[66] = (permissions >> 16) & 0xff;
  	buf[67] = (permissions >> 24) & 0xff;
  	memcpy(buf + 68, fileID->getCString(), fileID->getLength());
  	len = 68 + fileID->getLength();
  	
  	if (!encryptMetadata) 
  	{
    	buf[len++] = 0xff;
    	buf[len++] = 0xff;
    	buf[len++] = 0xff;
    	buf[len++] = 0xff;
  	}
  	
  	md5(buf, len, fileKey);
  	if (encRevision == 3) 
  	{
    	for (int i = 0; i < 50; ++i) 
      		md5(fileKey, keyLength, fileKey);
  	}
  	
  	uchar fState[256];
  	uchar fx = 0;
  	uchar fy = 0;
  	bool ok = true;
  	uchar test[32];
  	if (encRevision == 2) 
  	{
    	rc4InitKey(fileKey, keyLength, fState);
    	for (int i = 0; i < 32; ++i) 
      		test[i] = rc4DecryptByte(fState, &fx, &fy, userKey->getChar(i));
      		
    	ok = memcmp(test, passwordPad, 32) == 0;
  	} 
  	else if (encRevision == 3) 
  	{
    	memcpy(test, userKey->getCString(), 32);
    	uchar tmpKey[16];
    	for (int i = 19; i >= 0; --i) 
    	{
      		for (int j = 0; j < keyLength; ++j) 
				tmpKey[j] = fileKey[j] ^ i;
				
      		rc4InitKey(tmpKey, keyLength, fState);
      		fx = fy = 0;
      		for (int j = 0; j < 32; ++j) 
				test[j] = rc4DecryptByte(fState, &fx, &fy, test[j]);
    	}
    	memcpy(buf, passwordPad, 32);
    	memcpy(buf + 32, fileID->getCString(), fileID->getLength());
    	md5(buf, 32 + fileID->getLength(), buf);
    	ok = memcmp(test, buf, 16) == 0;
  	} 
  	else 
    	ok = false;

  	delete [] buf;
  	return ok;
}

XWDecryptStream::XWDecryptStream(XWStream *strA, 
  	                             uchar *fileKey,
					             CryptAlgorithm algoA, 
					             int keyLength,
					             int objNum, 
					             int objGen)
	:XWFilterStream(strA)
{
	algo = algoA;
	for (int i = 0; i < keyLength; ++i) 
    	objKey[i] = fileKey[i];
  	
  switch (algo) 
  {
  	case cryptRC4:
    	objKey[keyLength] = objNum & 0xff;
    	objKey[keyLength + 1] = (objNum >> 8) & 0xff;
    	objKey[keyLength + 2] = (objNum >> 16) & 0xff;
    	objKey[keyLength + 3] = objGen & 0xff;
    	objKey[keyLength + 4] = (objGen >> 8) & 0xff;
    	md5(objKey, keyLength + 5, objKey);
    	if ((objKeyLength = keyLength + 5) > 16) 
    	{
      	objKeyLength = 16;
    	}
    	break;
    	
  	case cryptAES:
    	objKey[keyLength] = objNum & 0xff;
    	objKey[keyLength + 1] = (objNum >> 8) & 0xff;
    	objKey[keyLength + 2] = (objNum >> 16) & 0xff;
    	objKey[keyLength + 3] = objGen & 0xff;
    	objKey[keyLength + 4] = (objGen >> 8) & 0xff;
    	objKey[keyLength + 5] = 0x73; // 's'
    	objKey[keyLength + 6] = 0x41; // 'A'
    	objKey[keyLength + 7] = 0x6c; // 'l'
    	objKey[keyLength + 8] = 0x54; // 'T'
    	md5(objKey, keyLength + 9, objKey);
    	if ((objKeyLength = keyLength + 5) > 16) 
    	{
      	objKeyLength = 16;
    	}
    	break;
    	
  	case cryptAES256:
    	objKeyLength = keyLength;
    	break;
  }
}

XWDecryptStream::~XWDecryptStream()
{
	if (str)
		delete str;
}

int XWDecryptStream::getChar()
{
	uchar in[16];
	int c = EOF;
  int i;
  
  switch (algo) 
  {
  	case cryptRC4:
    	if (state.rc4.buf == EOF) 
    	{
      	c = str->getChar();
      	if (c != EOF) 
      	{
					state.rc4.buf = rc4DecryptByte(state.rc4.state, &state.rc4.x, &state.rc4.y, (uchar)c);
      	}
    	}
    	c = state.rc4.buf;
    	state.rc4.buf = EOF;
    	break;
    
  	case cryptAES:
    	if (state.aes.bufIdx == 16) 
    	{
      	for (i = 0; i < 16; ++i) 
      	{
					if ((c = str->getChar()) == EOF) 
					{
	  				return EOF;
					}
					in[i] = (uchar)c;
      	}
      	aesDecryptBlock(&state.aes, in, str->lookChar() == EOF);
    	}
    	if (state.aes.bufIdx == 16) 
    	{
      	c = EOF;
    	} 
    	else 
    	{
      	c = state.aes.buf[state.aes.bufIdx++];
    	}
    	break;
    	
  	case cryptAES256:
    	if (state.aes256.bufIdx == 16) 
    	{
      	for (i = 0; i < 16; ++i) 
      	{
					if ((c = str->getChar()) == EOF) 
					{
	  				return EOF;
					}
					in[i] = (uchar)c;
      	}
      	aes256DecryptBlock(&state.aes256, in, str->lookChar() == EOF);
    	}
    	if (state.aes256.bufIdx == 16) 
    	{
      	c = EOF;
    	} 
    	else 
    	{
      	c = state.aes256.buf[state.aes256.bufIdx++];
    	}
    	break;
  }
  return c;
}

bool XWDecryptStream::isBinary(bool last)
{
	return str->isBinary(last);
}

int XWDecryptStream::lookChar()
{
	uchar in[16];
  	int c = EOF;
  	int i;
  	switch (algo) 
  	{
  		case cryptRC4:
    		if (state.rc4.buf == EOF) 
    		{
      		c = str->getChar();
      		if (c != EOF) 
      		{
						state.rc4.buf = rc4DecryptByte(state.rc4.state, &state.rc4.x, &state.rc4.y, (uchar)c);
      		}
    		}
    		c = state.rc4.buf;
    		break;
    		
  		case cryptAES:
    		if (state.aes.bufIdx == 16) 
    		{
      		for (i = 0; i < 16; ++i) 
      		{
						if ((c = str->getChar()) == EOF) 
						{
	  					return EOF;
						}
						in[i] = c;
      		}
      		aesDecryptBlock(&state.aes, in, str->lookChar() == EOF);
    		}
    		if (state.aes.bufIdx == 16) 
    		{
      		c = EOF;
    		} 
    		else 
    		{
      		c = state.aes.buf[state.aes.bufIdx];
    		}
    		break;
    		
  		case cryptAES256:
    		if (state.aes256.bufIdx == 16) 
    		{
      		for (i = 0; i < 16; ++i) 
      		{
						if ((c = str->getChar()) == EOF) 
						{
	  					return EOF;
						}
						in[i] = c;
      		}
      		aes256DecryptBlock(&state.aes256, in, str->lookChar() == EOF);
    		}
    		if (state.aes256.bufIdx == 16) 
    		{
      		c = EOF;
    		} 
    		else 
    		{
      		c = state.aes256.buf[state.aes256.bufIdx];
    		}
    		break;
  	}
  	return c;
}

void XWDecryptStream::reset()
{
	int i;

  str->reset();
  switch (algo) 
  {
  	case cryptRC4:
    	state.rc4.x = state.rc4.y = 0;
    	rc4InitKey(objKey, objKeyLength, state.rc4.state);
    	state.rc4.buf = EOF;
    	break;
    	
  	case cryptAES:
    	aesKeyExpansion(&state.aes, objKey, objKeyLength);
    	for (i = 0; i < 16; ++i) 
    	{
      	state.aes.cbc[i] = str->getChar();
    	}
    	state.aes.bufIdx = 16;
    	break;
    	
  	case cryptAES256:
    	aes256KeyExpansion(&state.aes256, objKey, objKeyLength);
    	for (i = 0; i < 16; ++i) 
    	{
      	state.aes256.cbc[i] = str->getChar();
    	}
    	state.aes256.bufIdx = 16;
    	break;
  }
}


static void rc4InitKey(uchar *key, int keyLen, uchar *state) 
{
  	for (int i = 0; i < 256; ++i)
    	state[i] = i;
    	
  	uchar index1 = 0;
  	uchar index2 = 0;
  	for (int i = 0; i < 256; ++i) 
  	{
    	index2 = (key[index1] + state[i] + index2) % 256;
    	uchar t = state[i];
    	state[i] = state[index2];
    	state[index2] = t;
    	index1 = (index1 + 1) % keyLen;
  	}
}

static uchar rc4DecryptByte(uchar *state, uchar *x, uchar *y, uchar c) 
{
  	uchar x1 = *x = (*x + 1) % 256;
  	uchar y1 = *y = (state[*x] + *y) % 256;
  	uchar tx = state[x1];
  	uchar ty = state[y1];
  	state[x1] = ty;
  	state[y1] = tx;
  	return c ^ state[(tx + ty) % 256];
}

static uchar sbox[256] = {
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static uchar invSbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

static uint rcon[11] = {
  0x00000000, // unused
  0x01000000,
  0x02000000,
  0x04000000,
  0x08000000,
  0x10000000,
  0x20000000,
  0x40000000,
  0x80000000,
  0x1b000000,
  0x36000000
};

static inline uint subWord(uint x) 
{
  	return (sbox[x >> 24] << 24) | (sbox[(x >> 16) & 0xff] << 16) | 
  	        (sbox[(x >> 8) & 0xff] << 8) | sbox[x & 0xff];
}

static inline uint rotWord(uint x) 
{
  	return ((x << 8) & 0xffffffff) | (x >> 24);
}

static inline void invSubBytes(uchar *state) 
{
  	for (int i = 0; i < 16; ++i) 
    	state[i] = invSbox[state[i]];
}

static inline void invShiftRows(uchar *state) 
{
  	uchar t = state[7];
  	state[7] = state[6];
  	state[6] = state[5];
  	state[5] = state[4];
  	state[4] = t;

  	t = state[8];
  	state[8] = state[10];
  	state[10] = t;
  	t = state[9];
  	state[9] = state[11];
  	state[11] = t;

  	t = state[12];
  	state[12] = state[13];
  	state[13] = state[14];
  	state[14] = state[15];
  	state[15] = t;
}

// {09} \cdot s
static inline uchar mul09(uchar s) 
{
  	uchar s2 = (s & 0x80) ? ((s << 1) ^ 0x1b) : (s << 1);
  	uchar s4 = (s2 & 0x80) ? ((s2 << 1) ^ 0x1b) : (s2 << 1);
  	uchar s8 = (s4 & 0x80) ? ((s4 << 1) ^ 0x1b) : (s4 << 1);
  	return s ^ s8;
}

// {0b} \cdot s
static inline uchar mul0b(uchar s) 
{
  	uchar s2 = (s & 0x80) ? ((s << 1) ^ 0x1b) : (s << 1);
  	uchar s4 = (s2 & 0x80) ? ((s2 << 1) ^ 0x1b) : (s2 << 1);
  	uchar s8 = (s4 & 0x80) ? ((s4 << 1) ^ 0x1b) : (s4 << 1);
  	return s ^ s2 ^ s8;
}

// {0d} \cdot s
static inline uchar mul0d(uchar s) 
{
  	uchar s2 = (s & 0x80) ? ((s << 1) ^ 0x1b) : (s << 1);
  	uchar s4 = (s2 & 0x80) ? ((s2 << 1) ^ 0x1b) : (s2 << 1);
  	uchar s8 = (s4 & 0x80) ? ((s4 << 1) ^ 0x1b) : (s4 << 1);
  	return s ^ s4 ^ s8;
}

// {0e} \cdot s
static inline uchar mul0e(uchar s) 
{
  	uchar s2 = (s & 0x80) ? ((s << 1) ^ 0x1b) : (s << 1);
  	uchar s4 = (s2 & 0x80) ? ((s2 << 1) ^ 0x1b) : (s2 << 1);
  	uchar s8 = (s4 & 0x80) ? ((s4 << 1) ^ 0x1b) : (s4 << 1);
  	return s2 ^ s4 ^ s8;
}

static inline void invMixColumns(uchar *state) 
{
  	for (int c = 0; c < 4; ++c) 
  	{
    	uchar s0 = state[c];
    	uchar s1 = state[4+c];
    	uchar s2 = state[8+c];
    	uchar s3 = state[12+c];
    	state[c] =    mul0e(s0) ^ mul0b(s1) ^ mul0d(s2) ^ mul09(s3);
    	state[4+c] =  mul09(s0) ^ mul0e(s1) ^ mul0b(s2) ^ mul0d(s3);
    	state[8+c] =  mul0d(s0) ^ mul09(s1) ^ mul0e(s2) ^ mul0b(s3);
    	state[12+c] = mul0b(s0) ^ mul0d(s1) ^ mul09(s2) ^ mul0e(s3);
  	}
}

static inline void invMixColumnsW(uint *w) 
{
  	for (int c = 0; c < 4; ++c) 
  	{
    	uchar s0 = w[c] >> 24;
    	uchar s1 = w[c] >> 16;
    	uchar s2 = w[c] >> 8;
    	uchar s3 = w[c];
    	w[c] = ((mul0e(s0) ^ mul0b(s1) ^ mul0d(s2) ^ mul09(s3)) << 24) | 
    	       ((mul09(s0) ^ mul0e(s1) ^ mul0b(s2) ^ mul0d(s3)) << 16) | 
    	       ((mul0d(s0) ^ mul09(s1) ^ mul0e(s2) ^ mul0b(s3)) << 8) | 
    	       (mul0b(s0) ^ mul0d(s1) ^ mul09(s2) ^ mul0e(s3));
  	}
}

static inline void addRoundKey(uchar *state, uint *w) 
{
  	for (int c = 0; c < 4; ++c) 
  	{
    	state[c] ^= w[c] >> 24;
    	state[4+c] ^= w[c] >> 16;
    	state[8+c] ^= w[c] >> 8;
    	state[12+c] ^= w[c];
  	}
}

static void aesKeyExpansion(DecryptAESState *s, uchar *objKey, int) 
{
  	for (int i = 0; i < 4; ++i) 
  	{
    	s->w[i] = (objKey[4*i] << 24) + (objKey[4*i+1] << 16) + (objKey[4*i+2] << 8) + objKey[4*i+3];
  	}
  	
  	for (int i = 4; i < 44; ++i) 
  	{
    	uint temp = s->w[i-1];
    	if (!(i & 3)) 
      		temp = subWord(rotWord(temp)) ^ rcon[i/4];
    	
    	s->w[i] = s->w[i-4] ^ temp;
  	}
  	
  	for (int round = 1; round <= 9; ++round) 
    	invMixColumnsW(&s->w[round * 4]);
}

static void aesDecryptBlock(DecryptAESState *s, uchar *in, bool last) 
{
  	for (int c = 0; c < 4; ++c) 
  	{
    	s->state[c] = in[4*c];
    	s->state[4+c] = in[4*c+1];
    	s->state[8+c] = in[4*c+2];
    	s->state[12+c] = in[4*c+3];
  	}

  	// round 0
  	addRoundKey(s->state, &s->w[10 * 4]);

  	// rounds 1-9
  	for (int round = 9; round >= 1; --round) 
  	{
    	invSubBytes(s->state);
    	invShiftRows(s->state);
    	invMixColumns(s->state);
    	addRoundKey(s->state, &s->w[round * 4]);
  	}

  	// round 10
  	invSubBytes(s->state);
  	invShiftRows(s->state);
  	addRoundKey(s->state, &s->w[0]);

  	// CBC
  	for (int c = 0; c < 4; ++c) 
  	{
    	s->buf[4*c] = s->state[c] ^ s->cbc[4*c];
    	s->buf[4*c+1] = s->state[4+c] ^ s->cbc[4*c+1];
    	s->buf[4*c+2] = s->state[8+c] ^ s->cbc[4*c+2];
    	s->buf[4*c+3] = s->state[12+c] ^ s->cbc[4*c+3];
  	}

  	// save the input block for the next CBC
  	for (int i = 0; i < 16; ++i) 
    	s->cbc[i] = in[i];

  	// remove padding
  	s->bufIdx = 0;
  	if (last) 
  	{
    	int n = s->buf[15];
    	if (n < 1 || n > 16) 
    	{
    	 // this should never happen
      	n = 16;
    	}
    	for (int i = 15; i >= n; --i) 
      		s->buf[i] = s->buf[i-n];
    	s->bufIdx = n;
  	}
}

static void aes256KeyExpansion(DecryptAES256State *s,
			       uchar *objKey, int ) 
{
  uint temp;
  int i, round;

  //~ this assumes objKeyLen == 32

  for (i = 0; i < 8; ++i) 
  {
    s->w[i] = (objKey[4*i] << 24) + (objKey[4*i+1] << 16) +
              (objKey[4*i+2] << 8) + objKey[4*i+3];
  }
  for (i = 8; i < 60; ++i) 
  {
    temp = s->w[i-1];
    if ((i & 7) == 0) {
      temp = subWord(rotWord(temp)) ^ rcon[i/8];
    } else if ((i & 7) == 4) {
      temp = subWord(temp);
    }
    s->w[i] = s->w[i-8] ^ temp;
  }
  for (round = 1; round <= 13; ++round) 
  {
    invMixColumnsW(&s->w[round * 4]);
  }
}

static void aes256DecryptBlock(DecryptAES256State *s, uchar *in, bool last) 
{
  int c, round, n, i;

  // initial state
  for (c = 0; c < 4; ++c) 
  {
    s->state[c] = in[4*c];
    s->state[4+c] = in[4*c+1];
    s->state[8+c] = in[4*c+2];
    s->state[12+c] = in[4*c+3];
  }

  // round 0
  addRoundKey(s->state, &s->w[14 * 4]);

  // rounds 13-1
  for (round = 13; round >= 1; --round) 
  {
    invSubBytes(s->state);
    invShiftRows(s->state);
    invMixColumns(s->state);
    addRoundKey(s->state, &s->w[round * 4]);
  }

  // round 14
  invSubBytes(s->state);
  invShiftRows(s->state);
  addRoundKey(s->state, &s->w[0]);

  // CBC
  for (c = 0; c < 4; ++c) 
  {
    s->buf[4*c] = s->state[c] ^ s->cbc[4*c];
    s->buf[4*c+1] = s->state[4+c] ^ s->cbc[4*c+1];
    s->buf[4*c+2] = s->state[8+c] ^ s->cbc[4*c+2];
    s->buf[4*c+3] = s->state[12+c] ^ s->cbc[4*c+3];
  }

  // save the input block for the next CBC
  for (i = 0; i < 16; ++i) 
  {
    s->cbc[i] = in[i];
  }

  // remove padding
  s->bufIdx = 0;
  if (last) 
  {
    n = s->buf[15];
    if (n < 1 || n > 16) 
    { // this should never happen
      n = 16;
    }
    for (i = 15; i >= n; --i) 
    {
      s->buf[i] = s->buf[i-n];
    }
    s->bufIdx = n;
  }
}

static inline ulong rotateLeft(ulong x, int r) 
{
  	x &= 0xffffffff;
  	return ((x << r) | (x >> (32 - r))) & 0xffffffff;
}

static inline ulong md5Round1(ulong a, 
                              ulong b, 
                              ulong c, 
                              ulong d,
			                  ulong Xk,  
			                  ulong s, 
			                  ulong Ti) 
{
  	return b + rotateLeft((a + ((b & c) | (~b & d)) + Xk + Ti), s);
}

static inline ulong md5Round2(ulong a, 
                               ulong b, 
                               ulong c, 
                               ulong d,
			                   ulong Xk,  
			                   ulong s, 
			                   ulong Ti) 
{
	return b + rotateLeft((a + ((b & d) | (c & ~d)) + Xk + Ti), s);
}

static inline ulong md5Round3(ulong a, 
                              ulong b, 
                              ulong c, 
                              ulong d,
			       			  ulong Xk,  
			       			  ulong s, 
			       			  ulong Ti) 
{
	return b + rotateLeft((a + (b ^ c ^ d) + Xk + Ti), s);
}

static inline ulong md5Round4(ulong a, 
                              ulong b, 
                              ulong c, 
                              ulong d,
			       			  ulong Xk,  
			       			  ulong s, 
			       			  ulong Ti) 
{
	return b + rotateLeft((a + (c ^ (b | ~d)) + Xk + Ti), s);
}

static void md5(uchar *msg, int msgLen, uchar *digest) 
{
  	int n64 = (msgLen + 1 + 8 + 63) / 64;

  	// initialize a, b, c, d
  	ulong a = 0x67452301;
  	ulong b = 0xefcdab89;
  	ulong c = 0x98badcfe;
  	ulong d = 0x10325476;

  	// loop through blocks
  	int k = 0;
  	ulong x[16];
  	for (int i = 0; i < n64; ++i) 
  	{
  		int j = 0;
    	for (; j < 16 && k < msgLen - 3; ++j, k += 4)
      		x[j] = (((((msg[k+3] << 8) + msg[k+2]) << 8) + msg[k+1]) << 8) + msg[k];
      		
    	if (i == n64 - 1) 
    	{
      		if (k == msgLen - 3)
				x[j] = 0x80000000 + (((msg[k+2] << 8) + msg[k+1]) << 8) + msg[k];
      		else if (k == msgLen - 2)
				x[j] = 0x800000 + (msg[k+1] << 8) + msg[k];
      		else if (k == msgLen - 1)
				x[j] = 0x8000 + msg[k];
      		else
				x[j] = 0x80;
				
      		++j;
      		while (j < 16)
				x[j++] = 0;
				
      		x[14] = msgLen << 3;
    	}

    	// save a, b, c, d
    	ulong aa = a;
    	ulong bb = b;
    	ulong cc = c;
    	ulong dd = d;

    	// round 1
    	a = md5Round1(a, b, c, d, x[0],   7, 0xd76aa478);
    	d = md5Round1(d, a, b, c, x[1],  12, 0xe8c7b756);
    	c = md5Round1(c, d, a, b, x[2],  17, 0x242070db);
    	b = md5Round1(b, c, d, a, x[3],  22, 0xc1bdceee);
    	a = md5Round1(a, b, c, d, x[4],   7, 0xf57c0faf);
    	d = md5Round1(d, a, b, c, x[5],  12, 0x4787c62a);
    	c = md5Round1(c, d, a, b, x[6],  17, 0xa8304613);
    	b = md5Round1(b, c, d, a, x[7],  22, 0xfd469501);
    	a = md5Round1(a, b, c, d, x[8],   7, 0x698098d8);
    	d = md5Round1(d, a, b, c, x[9],  12, 0x8b44f7af);
    	c = md5Round1(c, d, a, b, x[10], 17, 0xffff5bb1);
    	b = md5Round1(b, c, d, a, x[11], 22, 0x895cd7be);
    	a = md5Round1(a, b, c, d, x[12],  7, 0x6b901122);
    	d = md5Round1(d, a, b, c, x[13], 12, 0xfd987193);
    	c = md5Round1(c, d, a, b, x[14], 17, 0xa679438e);
    	b = md5Round1(b, c, d, a, x[15], 22, 0x49b40821);

    	// round 2
    	a = md5Round2(a, b, c, d, x[1],   5, 0xf61e2562);
    	d = md5Round2(d, a, b, c, x[6],   9, 0xc040b340);
    	c = md5Round2(c, d, a, b, x[11], 14, 0x265e5a51);
    	b = md5Round2(b, c, d, a, x[0],  20, 0xe9b6c7aa);
    	a = md5Round2(a, b, c, d, x[5],   5, 0xd62f105d);
    	d = md5Round2(d, a, b, c, x[10],  9, 0x02441453);
    	c = md5Round2(c, d, a, b, x[15], 14, 0xd8a1e681);
    	b = md5Round2(b, c, d, a, x[4],  20, 0xe7d3fbc8);
    	a = md5Round2(a, b, c, d, x[9],   5, 0x21e1cde6);
    	d = md5Round2(d, a, b, c, x[14],  9, 0xc33707d6);
    	c = md5Round2(c, d, a, b, x[3],  14, 0xf4d50d87);
    	b = md5Round2(b, c, d, a, x[8],  20, 0x455a14ed);
    	a = md5Round2(a, b, c, d, x[13],  5, 0xa9e3e905);
    	d = md5Round2(d, a, b, c, x[2],   9, 0xfcefa3f8);
    	c = md5Round2(c, d, a, b, x[7],  14, 0x676f02d9);
    	b = md5Round2(b, c, d, a, x[12], 20, 0x8d2a4c8a);

    	// round 3
    	a = md5Round3(a, b, c, d, x[5],   4, 0xfffa3942);
    	d = md5Round3(d, a, b, c, x[8],  11, 0x8771f681);
    	c = md5Round3(c, d, a, b, x[11], 16, 0x6d9d6122);
    	b = md5Round3(b, c, d, a, x[14], 23, 0xfde5380c);
    	a = md5Round3(a, b, c, d, x[1],   4, 0xa4beea44);
    	d = md5Round3(d, a, b, c, x[4],  11, 0x4bdecfa9);
    	c = md5Round3(c, d, a, b, x[7],  16, 0xf6bb4b60);
    	b = md5Round3(b, c, d, a, x[10], 23, 0xbebfbc70);
    	a = md5Round3(a, b, c, d, x[13],  4, 0x289b7ec6);
    	d = md5Round3(d, a, b, c, x[0],  11, 0xeaa127fa);
    	c = md5Round3(c, d, a, b, x[3],  16, 0xd4ef3085);
    	b = md5Round3(b, c, d, a, x[6],  23, 0x04881d05);
    	a = md5Round3(a, b, c, d, x[9],   4, 0xd9d4d039);
    	d = md5Round3(d, a, b, c, x[12], 11, 0xe6db99e5);
    	c = md5Round3(c, d, a, b, x[15], 16, 0x1fa27cf8);
    	b = md5Round3(b, c, d, a, x[2],  23, 0xc4ac5665);

    	// round 4
    	a = md5Round4(a, b, c, d, x[0],   6, 0xf4292244);
    	d = md5Round4(d, a, b, c, x[7],  10, 0x432aff97);
    	c = md5Round4(c, d, a, b, x[14], 15, 0xab9423a7);
    	b = md5Round4(b, c, d, a, x[5],  21, 0xfc93a039);
    	a = md5Round4(a, b, c, d, x[12],  6, 0x655b59c3);
    	d = md5Round4(d, a, b, c, x[3],  10, 0x8f0ccc92);
    	c = md5Round4(c, d, a, b, x[10], 15, 0xffeff47d);
    	b = md5Round4(b, c, d, a, x[1],  21, 0x85845dd1);
    	a = md5Round4(a, b, c, d, x[8],   6, 0x6fa87e4f);
    	d = md5Round4(d, a, b, c, x[15], 10, 0xfe2ce6e0);
    	c = md5Round4(c, d, a, b, x[6],  15, 0xa3014314);
    	b = md5Round4(b, c, d, a, x[13], 21, 0x4e0811a1);
    	a = md5Round4(a, b, c, d, x[4],   6, 0xf7537e82);
    	d = md5Round4(d, a, b, c, x[11], 10, 0xbd3af235);
    	c = md5Round4(c, d, a, b, x[2],  15, 0x2ad7d2bb);
    	b = md5Round4(b, c, d, a, x[9],  21, 0xeb86d391);

    	// increment a, b, c, d
    	a += aa;
    	b += bb;
    	c += cc;
    	d += dd;
  	}

  	// break digest into bytes
  	digest[0] = (uchar)(a & 0xff);
  	digest[1] = (uchar)((a >>= 8) & 0xff);
  	digest[2] = (uchar)((a >>= 8) & 0xff);
  	digest[3] = (uchar)((a >>= 8) & 0xff);
  	digest[4] = (uchar)(b & 0xff);
  	digest[5] = (uchar)((b >>= 8) & 0xff);
  	digest[6] = (uchar)((b >>= 8) & 0xff);
  	digest[7] = (uchar)((b >>= 8) & 0xff);
  	digest[8] = (uchar)(c & 0xff);
  	digest[9] = (uchar)((c >>= 8) & 0xff);
  	digest[10] = (uchar)((c >>= 8) & 0xff);
  	digest[11] = (uchar)((c >>= 8) & 0xff);
  	digest[12] = (uchar)(d & 0xff);
  	digest[13] = (uchar)((d >>= 8) & 0xff);
  	digest[14] = (uchar)((d >>= 8) & 0xff);
  	digest[15] = (uchar)((d >>= 8) & 0xff);
}


static uint sha256K[64] = 
{
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
  0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
  0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
  0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
  0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
  0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
  0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
  0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
  0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static inline uint rotr(uint x, uint n) 
{
  return (x >> n) | (x << (32 - n));
}

static inline uint sha256Ch(uint x, uint y, uint z) 
{
  return (x & y) ^ (~x & z);
}

static inline uint sha256Maj(uint x, uint y, uint z) 
{
  return (x & y) ^ (x & z) ^ (y & z);
}

static inline uint sha256Sigma0(uint x) 
{
  return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

static inline uint sha256Sigma1(uint x) 
{
  return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

static inline uint sha256sigma0(uint x) 
{
  return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

static inline uint sha256sigma1(uint x) 
{
  return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

void sha256HashBlock(uchar *blk, uint *H) 
{
  uint W[64];
  uint a, b, c, d, e, f, g, h;
  uint T1, T2;
  uint t;

  // 1. prepare the message schedule
  for (t = 0; t < 16; ++t) 
  {
    W[t] = (blk[t*4] << 24) |
           (blk[t*4 + 1] << 16) |
           (blk[t*4 + 2] << 8) |
           blk[t*4 + 3];
  }
  for (t = 16; t < 64; ++t) 
  {
    W[t] = sha256sigma1(W[t-2]) + W[t-7] + sha256sigma0(W[t-15]) + W[t-16];
  }

  // 2. initialize the eight working variables
  a = H[0];
  b = H[1];
  c = H[2];
  d = H[3];
  e = H[4];
  f = H[5];
  g = H[6];
  h = H[7];

  // 3.
  for (t = 0; t < 64; ++t) 
  {
    T1 = h + sha256Sigma1(e) + sha256Ch(e,f,g) + sha256K[t] + W[t];
    T2 = sha256Sigma0(a) + sha256Maj(a,b,c);
    h = g;
    g = f;
    f = e;
    e = d + T1;
    d = c;
    c = b;
    b = a;
    a = T1 + T2;
  }

  // 4. compute the intermediate hash value
  H[0] += a;
  H[1] += b;
  H[2] += c;
  H[3] += d;
  H[4] += e;
  H[5] += f;
  H[6] += g;
  H[7] += h;
}

static void sha256(uchar *msg, int msgLen, uchar *hash) 
{
  uchar blk[64];
  uint H[8];
  int blkLen, i;

  H[0] = 0x6a09e667;
  H[1] = 0xbb67ae85;
  H[2] = 0x3c6ef372;
  H[3] = 0xa54ff53a;
  H[4] = 0x510e527f;
  H[5] = 0x9b05688c;
  H[6] = 0x1f83d9ab;
  H[7] = 0x5be0cd19;

  blkLen = 0;
  for (i = 0; i + 64 <= msgLen; i += 64) 
  {
    sha256HashBlock(msg + i, H);
  }
  blkLen = msgLen - i;
  if (blkLen > 0) 
  {
    memcpy(blk, msg + i, blkLen);
  }

  // pad the message
  blk[blkLen++] = 0x80;
  if (blkLen > 56) 
  {
    while (blkLen < 64) 
    {
      blk[blkLen++] = 0;
    }
    sha256HashBlock(blk, H);
    blkLen = 0;
  }
  while (blkLen < 56) 
  {
    blk[blkLen++] = 0;
  }
  blk[56] = 0;
  blk[57] = 0;
  blk[58] = 0;
  blk[59] = 0;
  blk[60] = (uchar)(msgLen >> 21);
  blk[61] = (uchar)(msgLen >> 13);
  blk[62] = (uchar)(msgLen >> 5);
  blk[63] = (uchar)(msgLen << 3);
  sha256HashBlock(blk, H);

  // copy the output into the buffer (convert words to bytes)
  for (i = 0; i < 8; ++i) 
  {
    hash[i*4]     = (uchar)(H[i] >> 24);
    hash[i*4 + 1] = (uchar)(H[i] >> 16);
    hash[i*4 + 2] = (uchar)(H[i] >> 8);
    hash[i*4 + 3] = (uchar)H[i];
  }
}
