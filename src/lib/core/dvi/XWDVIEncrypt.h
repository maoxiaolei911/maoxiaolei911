/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVIENCRYPT_H
#define XWDVIENCRYPT_H

class XWObject;
class XWDVIRef;

class XW_CORE_EXPORT XWDVIEncrypt
{
public:
	static void computeIdString (char *dviname, char *pdfname);
	
	static void encrypt(uchar *data, ulong len);
	static void encryptObj(XWObject * obj, XWDVIRef * xref);
	
	static void idArray(XWObject * obj, XWDVIRef * xref);
	
	static void setPasswd(unsigned bits, 
	                      unsigned perm, 
	                      char *owner_pw, 
	                      char *user_pw);
	static void setGeneration(ulong generation);
	static void setLabel(ulong label);
	
private:
	static void compute_encryption_key(unsigned char *pwd);	
	static void compute_owner_password();
	static void compute_user_password();
	
	static void passwd_padding(unsigned char *src, unsigned char *dst);
};

#endif //XWDVIENCRYPT_H