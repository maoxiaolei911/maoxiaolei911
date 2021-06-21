/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWSECURITYHANDLER_H
#define XWSECURITYHANDLER_H

#include <QObject>
#include "XWSecurityHandlerInterface.h"

class XW_CORE_EXPORT XWSecurityHandler: public QObject
{
	Q_OBJECT
	
public:
    XWSecurityHandler(XWDoc *docA, QObject * parent);
    virtual ~XWSecurityHandler() {}
    
    virtual bool authorize(void *authData) = 0;
    
    bool checkEncryption(XWString *ownerPassword, XWString *userPassword);
			
	virtual void freeAuthData(void *authData) = 0;
	
	virtual void   * getAuthData() = 0;
	virtual CryptAlgorithm getEncAlgorithm() = 0;
	virtual int      getEncVersion() = 0;
	virtual uchar * getFileKey() = 0;
	virtual int      getFileKeyLength() = 0;
	virtual bool     getOwnerPasswordOk() = 0;
	virtual int      getPermissionFlags() = 0;
	
	virtual bool isUnencrypted() { return false; }
	
    static XWSecurityHandler *make(XWDoc *docA, XWObject *encryptDictA);
    virtual void *makeAuthData(XWString *ownerPassword, XWString *userPassword) = 0;
			                    
protected:
	XWDoc *doc;
};

class XW_CORE_EXPORT XWStdSecurityHandler : public XWSecurityHandler
{
	Q_OBJECT
	
public:
    XWStdSecurityHandler(XWDoc *docA, 
                         XWObject *encryptDictA, 
                         QObject * parent = 0);
    virtual ~XWStdSecurityHandler();
    
    virtual bool authorize(void *authData);
    
    virtual void freeAuthData(void *authData);
    
    virtual void   * getAuthData();
    virtual CryptAlgorithm getEncAlgorithm() { return encAlgorithm; }
    virtual int      getEncVersion() { return encVersion; }
    virtual uchar * getFileKey() { return fileKey; }
    virtual int      getFileKeyLength() { return fileKeyLength; }
    virtual bool     getOwnerPasswordOk() { return ownerPasswordOk; }
    virtual int      getPermissionFlags() { return permFlags; }
    
    virtual bool isUnencrypted();
    
    virtual void *makeAuthData(XWString *ownerPassword, XWString *userPassword);
    
private:
    int permFlags;
  bool ownerPasswordOk;
  uchar fileKey[32];
  int fileKeyLength;
  int encVersion;
  int encRevision;
  CryptAlgorithm encAlgorithm;
  bool encryptMetadata;

  XWString *ownerKey, *userKey;
  XWString *ownerEnc, *userEnc;
  XWString *fileID;
  bool ok;
};

class XW_CORE_EXPORT XWExternalSecurityHandler : public XWSecurityHandler
{
	Q_OBJECT
	
public:
	XWExternalSecurityHandler(XWDoc *docA, 
	                          XWObject *encryptDictA,
			                  XWSecurityHandlerInterface *xshA,
			                  QObject * parent = 0);	
	virtual ~XWExternalSecurityHandler();
	
	virtual bool authorize(void *authData);
    
    virtual void freeAuthData(void *authData);
    
    virtual void   * getAuthData();
    virtual CryptAlgorithm getEncAlgorithm() { return encAlgorithm; }
    virtual int      getEncVersion() { return encVersion; }
    virtual uchar * getFileKey() { return fileKey; }
    virtual int      getFileKeyLength() { return fileKeyLength; }
    virtual bool     getOwnerPasswordOk() { return false; }
    virtual int      getPermissionFlags() { return permFlags; }
    
    virtual void *makeAuthData(XWString *ownerPassword, XWString *userPassword);
    
private:
  	XWObject encryptDict;
  	XWSecurityHandlerInterface *xsh;
  	void *docData;
  	int permFlags;
  	uchar fileKey[16];
  	int fileKeyLength;
  	int encVersion;
  	CryptAlgorithm encAlgorithm;
  	bool ok;
};

#endif // XWSECURITYHANDLER_H
