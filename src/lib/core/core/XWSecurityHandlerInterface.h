/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWSECURITYHANDLERINTERFACE_H
#define XWSECURITYHANDLERINTERFACE_H

#include "XWGlobal.h"
#include "XWObject.h"

#define xwPermissionOpen     (1 << 0)
#define xwPermissionPrint    (1 << 2)
#define xwPermissionChange   (1 << 3)
#define xwPermissionCopy     (1 << 4)
#define xwPermissionNotes    (1 << 5)


class XWDoc;

class XW_CORE_EXPORT XWSecurityHandlerInterface
{
public:
    virtual ~XWSecurityHandlerInterface() {}
    
    virtual int authorize(void *docData, void *authData) = 0;
    
    virtual void freeAuthData(void *docData, void *authData) = 0;
	virtual void freeDoc(void *docData) = 0;
	virtual void freeKey(void *docData,
		                 char *key, 
		                 int keyLen) = 0;
	
	virtual bool getAuthData(void *docData, void **authData) = 0;
	virtual CryptAlgorithm  getEncAlgorithm() = 0;
	virtual int getEncVersion() = 0;
	virtual uchar *getFileKey() = 0;
	virtual int getFileKeyLength() = 0;
	virtual bool getKey(void *docData,
		                char **key, 
		                int *keyLen, 
		                int *cryptVersion) = 0;
	virtual bool getOwnerPasswordOk() = 0;
	virtual int getPermissionFlags() = 0;
	
	virtual bool isUnencrypted() = 0 ;
	
	virtual bool makeAuthData(void *docData,
			                  char *ownerPassword, 
			                  char *userPassword,
			                  void **authData) = 0;
	
    virtual bool newDoc(XWDoc *docA, 
	                    XWObject *encryptDictA,
	                    void **docData) = 0;
};

Q_DECLARE_INTERFACE(XWSecurityHandlerInterface, "xiuwen.com.plugin.XWSecurityHandlerInterface/1.0");

#endif // XWSECURITYHANDLERINTERFACE_H

