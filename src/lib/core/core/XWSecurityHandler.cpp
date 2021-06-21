/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QString>
#include <QByteArray>
#include <QTextCodec>
#include <QFile>
#include <QPluginLoader>
#include "XWString.h"
#include "XWApplication.h"
#include "XWDoc.h"
#include "XWDecrypt.h"
#include "XWSecurityHandler.h"

XWSecurityHandler::XWSecurityHandler(XWDoc *docA, QObject * parent)
	:QObject(parent),
	 doc(docA)
{
}

bool XWSecurityHandler::checkEncryption(XWString *ownerPassword, XWString *userPassword)
{
	void *authData = 0;
    if (ownerPassword || userPassword) 
        authData = makeAuthData(ownerPassword, userPassword);
    
    bool ok = authorize(authData);
    if (authData) 
        freeAuthData(authData);
    
    for (int i = 0; !ok && i < 3; ++i) 
    {
        if (!(authData = getAuthData())) 
            break;
        
        ok = authorize(authData);
        if (authData) 
            freeAuthData(authData);
    }
    
    if (!ok)
    {
        QString e = QString(tr("Incorrect password!\n"));
        xwApp->error(e);
    }
    
    return ok;
}

XWSecurityHandler * XWSecurityHandler::make(XWDoc *docA, XWObject *encryptDictA)
{
	XWObject filterObj;
    XWSecurityHandler *secHdlr = 0;
    encryptDictA->dictLookup("Filter", &filterObj);
    if (filterObj.isName("Standard"))
        secHdlr = new XWStdSecurityHandler(docA, encryptDictA);
    else if (filterObj.isName())
    {
    	QTextCodec * codec = QTextCodec::codecForLocale();
		QString fn = codec->toUnicode(filterObj.getName());
		QPluginLoader loader(fn);
		QObject *plugin = loader.instance();
		XWSecurityHandlerInterface * xsh = qobject_cast<XWSecurityHandlerInterface *>(plugin);
		if (xsh)
			secHdlr = new XWExternalSecurityHandler(docA, encryptDictA, xsh);
    }
        
    filterObj.free();
    return secHdlr;
}


class XWStandardAuthData 
{
public:
    XWStandardAuthData(XWString *ownerPasswordA, XWString *userPasswordA) 
    {
        ownerPassword = ownerPasswordA;
        userPassword = userPasswordA;
    }

    ~XWStandardAuthData() 
    {
        if (ownerPassword) 
            delete ownerPassword;
        
        if (userPassword) 
            delete userPassword;
    }

    XWString *ownerPassword;
    XWString *userPassword;
};

XWStdSecurityHandler::XWStdSecurityHandler(XWDoc *docA, 
                                           XWObject *encryptDictA, 
                                           QObject * parent)
	:XWSecurityHandler(docA, parent)
{
	XWObject versionObj, revisionObj, lengthObj;
  XWObject ownerKeyObj, userKeyObj, ownerEncObj, userEncObj;
  XWObject permObj, fileIDObj, fileIDObj1;
  XWObject cryptFiltersObj, streamFilterObj, stringFilterObj;
  XWObject cryptFilterObj, cfmObj, cfLengthObj;
  XWObject encryptMetadataObj;

  ok = false;
  fileID = NULL;
  ownerKey = NULL;
  userKey = NULL;
  ownerEnc = NULL;
  userEnc = NULL;
  fileKeyLength = 0;

  encryptDictA->dictLookup("V", &versionObj);
  encryptDictA->dictLookup("R", &revisionObj);
  encryptDictA->dictLookup("Length", &lengthObj);
  encryptDictA->dictLookup("O", &ownerKeyObj);
  encryptDictA->dictLookup("U", &userKeyObj);
  encryptDictA->dictLookup("OE", &ownerEncObj);
  encryptDictA->dictLookup("UE", &userEncObj);
  encryptDictA->dictLookup("P", &permObj);
  doc->getXRef()->getTrailerDict()->dictLookup("ID", &fileIDObj);
  if (versionObj.isInt() &&
      revisionObj.isInt() &&
      permObj.isInt() &&
      ownerKeyObj.isString() &&
      userKeyObj.isString()) {
    encVersion = versionObj.getInt();
    encRevision = revisionObj.getInt();
    if ((encRevision <= 4 &&
	 ownerKeyObj.getString()->getLength() == 32 &&
	 userKeyObj.getString()->getLength() == 32) ||
	(encRevision == 5 &&
	 // the spec says 48 bytes, but Acrobat pads them out longer
	 ownerKeyObj.getString()->getLength() >= 48 &&
	 userKeyObj.getString()->getLength() >= 48 &&
	 ownerEncObj.isString() &&
	 ownerEncObj.getString()->getLength() == 32 &&
	 userEncObj.isString() &&
	 userEncObj.getString()->getLength() == 32)) {
      encAlgorithm = cryptRC4;
      // revision 2 forces a 40-bit key - some buggy PDF generators
      // set the Length value incorrectly
      if (encRevision == 2 || !lengthObj.isInt()) {
	fileKeyLength = 5;
      } else {
	fileKeyLength = lengthObj.getInt() / 8;
      }
      encryptMetadata = true;
      //~ this currently only handles a subset of crypt filter functionality
      //~ (in particular, it ignores the EFF entry in encryptDictA, and
      //~ doesn't handle the case where StmF, StrF, and EFF are not all the
      //~ same)
      if ((encVersion == 4 || encVersion == 5) &&
	  (encRevision == 4 || encRevision == 5)) {
	encryptDictA->dictLookup("CF", &cryptFiltersObj);
	encryptDictA->dictLookup("StmF", &streamFilterObj);
	encryptDictA->dictLookup("StrF", &stringFilterObj);
	if (cryptFiltersObj.isDict() &&
	    streamFilterObj.isName() &&
	    stringFilterObj.isName() &&
	    !strcmp(streamFilterObj.getName(), stringFilterObj.getName())) {
	  if (!strcmp(streamFilterObj.getName(), "Identity")) {
	    // no encryption on streams or strings
	    encVersion = encRevision = -1;
	  } else {
	    if (cryptFiltersObj.dictLookup(streamFilterObj.getName(),
					   &cryptFilterObj)->isDict()) {
	      cryptFilterObj.dictLookup("CFM", &cfmObj);
	      if (cfmObj.isName("V2")) {
		encVersion = 2;
		encRevision = 3;
		if (cryptFilterObj.dictLookup("Length",
					      &cfLengthObj)->isInt()) {
		  //~ according to the spec, this should be cfLengthObj / 8
		  fileKeyLength = cfLengthObj.getInt();
		}
		cfLengthObj.free();
	      } else if (cfmObj.isName("AESV2")) {
		encVersion = 2;
		encRevision = 3;
		encAlgorithm = cryptAES;
		if (cryptFilterObj.dictLookup("Length",
					      &cfLengthObj)->isInt()) {
		  //~ according to the spec, this should be cfLengthObj / 8
		  fileKeyLength = cfLengthObj.getInt();
		}
		cfLengthObj.free();
	      } else if (cfmObj.isName("AESV3")) {
		encVersion = 5;
		encRevision = 5;
		encAlgorithm = cryptAES256;
		if (cryptFilterObj.dictLookup("Length",
					      &cfLengthObj)->isInt()) {
		  //~ according to the spec, this should be cfLengthObj / 8
		  fileKeyLength = cfLengthObj.getInt();
		}
		cfLengthObj.free();
	      }
	      cfmObj.free();
	    }
	    cryptFilterObj.free();
	  }
	}
	stringFilterObj.free();
	streamFilterObj.free();
	cryptFiltersObj.free();
	if (encryptDictA->dictLookup("EncryptMetadata",
				     &encryptMetadataObj)->isBool()) {
	  encryptMetadata = encryptMetadataObj.getBool();
	}
	encryptMetadataObj.free();
      }
      permFlags = permObj.getInt();
      ownerKey = ownerKeyObj.getString()->copy();
      userKey = userKeyObj.getString()->copy();
      if (encVersion >= 1 && encVersion <= 2 &&
	  encRevision >= 2 && encRevision <= 3) {
	if (fileIDObj.isArray()) {
	  if (fileIDObj.arrayGet(0, &fileIDObj1)->isString()) {
	    fileID = fileIDObj1.getString()->copy();
	  } else {
	    fileID = new XWString();
	  }
	  fileIDObj1.free();
	} else {
	  fileID = new XWString();
	}
	if (fileKeyLength > 16 || fileKeyLength < 0) {
	  fileKeyLength = 16;
	}
	ok = true;
      } else if (encVersion == 5 && encRevision == 5) {
	fileID = new XWString(); // unused for V=R=5
	ownerEnc = ownerEncObj.getString()->copy();
	userEnc = userEncObj.getString()->copy();
	if (fileKeyLength > 32 || fileKeyLength < 0) {
	  fileKeyLength = 32;
	}
	ok = true;
      } else if (!(encVersion == -1 && encRevision == -1)) {
	xwApp->error("Unsupported version/revision ({0:d}/{1:d}) of Standard security handler");
      }
    } else {
      xwApp->error("Invalid encryption key length");
    }
  } else {
    xwApp->error("Weird encryption info");
  }
  fileIDObj.free();
  permObj.free();
  userEncObj.free();
  ownerEncObj.free();
  userKeyObj.free();
  ownerKeyObj.free();
  lengthObj.free();
  revisionObj.free();
  versionObj.free();
}

XWStdSecurityHandler::~XWStdSecurityHandler()
{
	if (fileID) {
    delete fileID;
  }
  if (ownerKey) {
    delete ownerKey;
  }
  if (userKey) {
    delete userKey;
  }
  if (ownerEnc) {
    delete ownerEnc;
  }
  if (userEnc) {
    delete userEnc;
  }
}

bool XWStdSecurityHandler::authorize(void *authData)
{
	XWString *ownerPassword, *userPassword;

  if (!ok) {
    return false;
  }
  if (authData) {
    ownerPassword = ((XWStandardAuthData *)authData)->ownerPassword;
    userPassword = ((XWStandardAuthData *)authData)->userPassword;
  } else {
    ownerPassword = NULL;
    userPassword = NULL;
  }
  if (!XWDecrypt::makeFileKey(encVersion, encRevision, fileKeyLength,
			    ownerKey, userKey, ownerEnc, userEnc,
			    permFlags, fileID,
			    ownerPassword, userPassword, fileKey,
			    encryptMetadata, &ownerPasswordOk)) {
    return false;
  }
  return true;
}

void XWStdSecurityHandler::freeAuthData(void *authData)
{
	delete (XWStandardAuthData *)authData;
}

void * XWStdSecurityHandler::getAuthData()
{
	QString tmp = xwApp->getPassward(tr("This document requires a password."), tr("User"));
	if (tmp.isEmpty())
		return 0;
		
	QByteArray ba = tmp.toAscii();
	XWString * password = new XWString(ba.data(), ba.size());	
	return new XWStandardAuthData(password, password->copy());
}

bool XWStdSecurityHandler::isUnencrypted()
{
	return encVersion == -1 && encRevision == -1;
}

void * XWStdSecurityHandler::makeAuthData(XWString *ownerPassword, XWString *userPassword)
{
	return new XWStandardAuthData(ownerPassword ? ownerPassword->copy()
			                      : (XWString *)0,
			                      userPassword ? userPassword->copy()
			                      : (XWString *)0);
}

XWExternalSecurityHandler::XWExternalSecurityHandler(XWDoc *docA, 
	                                                 XWObject *encryptDictA,
			                                         XWSecurityHandlerInterface *xshA,
			                                         QObject * parent)
	:XWSecurityHandler(docA, parent)
{
	encryptDictA->copy(&encryptDict);
  	xsh = xshA;
  	encAlgorithm = cryptRC4; //~ this should be obtained via getKey
  	ok = xsh->newDoc(docA, encryptDictA, &docData);
}

XWExternalSecurityHandler::~XWExternalSecurityHandler()
{
	xsh->freeDoc(docData);
	encryptDict.free();
}

bool XWExternalSecurityHandler::authorize(void *authData)
{
	if (!ok) 
    	return false;
    	
  	permFlags = xsh->authorize(docData, authData);
  	if (!(permFlags & xwPermissionOpen)) 
    	return false;
  	
  	char *key = 0;
  	int length = 0;
  	if (!(xsh->getKey(docData, &key, &length, &encVersion))) 
    	return false;
  	
  	if ((fileKeyLength = length) > 16) 
    	fileKeyLength = 16;
  	
  	memcpy(fileKey, key, fileKeyLength);
  	xsh->freeKey(docData, key, length);
  	return true;
}

void XWExternalSecurityHandler::freeAuthData(void *authData)
{
	xsh->freeAuthData(docData, authData);
}

void * XWExternalSecurityHandler::getAuthData()
{
	void *authData = 0;
  	if (!xsh->getAuthData(docData, &authData))
    	return NULL;
  	return authData;
}

void * XWExternalSecurityHandler::makeAuthData(XWString *ownerPassword, XWString *userPassword)
{
  	char * opw = ownerPassword ? ownerPassword->getCString() : (char *)NULL;
  	char * upw = userPassword ? userPassword->getCString() : (char *)NULL;
  	void *authData = 0;
  	if (!xsh->makeAuthData(docData, opw, upw, &authData))
    	return 0;
  	return authData;
}

