/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <QTextCodec>
#include <QString>
#include "XWApplication.h"
#include "XWDVICrypt.h"
#include "XWObject.h"
#include "XWDVIRef.h"
#include "XWDVIEncrypt.h"


#define MAX_KEY_LEN 16
#define MAX_STR_LEN 32
#define MAX_PWD_LEN 128

static unsigned char algorithm, revision, key_size;
static long permission;

static unsigned char key_data[MAX_KEY_LEN], id_string[MAX_KEY_LEN];
static unsigned char opwd_string[MAX_STR_LEN], upwd_string[MAX_STR_LEN];

static unsigned long current_label = 0;
static unsigned current_generation = 0;

static ARC4_KEY key;
static MD5_CONTEXT md5_ctx;

static unsigned char md5_buf[MAX_KEY_LEN], key_buf[MAX_KEY_LEN];
static unsigned char in_buf[MAX_STR_LEN], out_buf[MAX_STR_LEN];

static const unsigned char padding_string[MAX_STR_LEN] = {
  0x28, 0xbf, 0x4e, 0x5e, 0x4e, 0x75, 0x8a, 0x41,
  0x64, 0x00, 0x4e, 0x56, 0xff, 0xfa, 0x01, 0x08,
  0x2e, 0x2e, 0x00, 0xb6, 0xd0, 0x68, 0x3e, 0x80,
  0x2f, 0x0c, 0xa9, 0xfe, 0x64, 0x53, 0x69, 0x7a
};

static char owner_passwd[MAX_PWD_LEN], user_passwd[MAX_PWD_LEN];


static const char *encrypt_strings[] = {
	QT_TRANSLATE_NOOP("XWDVIEncrypt", "Please input owner passward."),
	QT_TRANSLATE_NOOP("XWDVIEncrypt", "Please input user passward.")
};


static const char *passward_strings[] = {
	QT_TRANSLATE_NOOP("XWDVIEncrypt", "Owner"),
	QT_TRANSLATE_NOOP("XWDVIEncrypt", "User")
};


void XWDVIEncrypt::computeIdString (char *dviname, char *pdfname)
{
	MD5_init(&md5_ctx);
	char * date_string = (char*)malloc(15 * sizeof(char));
	time_t current_time;
	time(&current_time);
  	struct tm *bd_time = localtime(&current_time);
  	
  	sprintf (date_string, "%04d%02d%02d%02d%02d%02d",
	   		 bd_time -> tm_year+1900, bd_time -> tm_mon+1, bd_time -> tm_mday,
	   		 bd_time -> tm_hour, bd_time -> tm_min, bd_time -> tm_sec);
  	MD5_write(&md5_ctx, (unsigned char *)date_string, strlen(date_string));
  	free(date_string);
  	
  	const char * product = xwApp->getProductName8();
  	const char * copyright = xwApp->getCopyRight8();
  	const char * version = xwApp->getVersion8();
  	char * producer = (char*)malloc((strlen(product) + strlen(copyright)+strlen(version) + 5) * sizeof(char));
  	sprintf(producer, "%s %s %s", product, version, copyright);
  	MD5_write(&md5_ctx, (unsigned char *)producer, strlen(producer));
  	free(producer);

  	MD5_write(&md5_ctx, (unsigned char *)dviname, strlen(dviname));
  	MD5_write(&md5_ctx, (unsigned char *)pdfname, strlen(pdfname));
  	MD5_final(id_string, &md5_ctx);
}

void XWDVIEncrypt::encrypt(uchar *data, ulong len)
{
	memcpy(in_buf, key_data, key_size);
  	in_buf[key_size]   = (unsigned char)(current_label) & 0xFF;
  	in_buf[key_size+1] = (unsigned char)(current_label >> 8) & 0xFF;
  	in_buf[key_size+2] = (unsigned char)(current_label >> 16) & 0xFF;
  	in_buf[key_size+3] = (unsigned char)(current_generation) & 0xFF;
  	in_buf[key_size+4] = (unsigned char)(current_generation >> 8) & 0xFF;

  	MD5_init(&md5_ctx);
  	MD5_write(&md5_ctx, in_buf, key_size+5);
  	MD5_final(md5_buf, &md5_ctx);
  
  	unsigned char * result = (unsigned char *)malloc(len * sizeof(unsigned char));
  	ARC4_set_key(&key, (key_size > 10 ? MAX_KEY_LEN : key_size+5), md5_buf);
  	ARC4(&key, len, data, result);
  	memcpy(data, result, len);
  	free(result);
}

void XWDVIEncrypt::encryptObj(XWObject * obj, XWDVIRef * xref)
{
	obj->initDict(xref);
	XWObject obj1;
	obj1.initName("Standard");
	obj->dictAdd(qstrdup("Filter"), &obj1);
	obj1.initInt(algorithm);
	obj->dictAdd(qstrdup("V"), &obj1);
	if (algorithm > 1)
	{
		obj1.initInt(key_size * 8);
		obj->dictAdd(qstrdup("Length"), &obj1);
	}
	
	obj1.initInt(revision);
	obj->dictAdd(qstrdup("R"), &obj1);
	obj1.initString(new XWString((char*)opwd_string, 32));
	obj->dictAdd(qstrdup("O"), &obj1);
	obj1.initString(new XWString((char*)upwd_string, 32));
	obj->dictAdd(qstrdup("U"), &obj1);
	obj1.initInt(permission);
	obj->dictAdd(qstrdup("P"), &obj1);
}

void XWDVIEncrypt::idArray(XWObject * obj, XWDVIRef * xref)
{
	obj->initArray(xref);
	XWObject obj1;
	obj1.initString(new XWString((char*)id_string, MAX_KEY_LEN));
	obj->arrayAdd(&obj1);
	obj1.initString(new XWString((char*)id_string, MAX_KEY_LEN));
	obj->arrayAdd(&obj1);
}

void XWDVIEncrypt::setGeneration(ulong generation)
{
	current_generation = generation;
}

void XWDVIEncrypt::setLabel(ulong label)
{
	current_label = label;
}

void XWDVIEncrypt::setPasswd(unsigned bits, 
	                         unsigned perm, 
	                         char *owner_pw, 
	                         char *user_pw)
{
	if (owner_pw) 
	{
    strncpy(owner_passwd, owner_pw, MAX_PWD_LEN);
  }
  else
  {
  	QTextCodec * codec = QTextCodec::codecForLocale();
  	QString m = qApp->translate("XWDVIEncrypt", encrypt_strings[0]);
  	QString l = qApp->translate("XWDVIEncrypt", passward_strings[0]);
  	QString tmp = xwApp->getPassward(m, l);
  	QByteArray ba = codec->fromUnicode(tmp);
		strncpy(owner_passwd, ba.data(), ba.size());
  }
  
  if (user_pw) 
  {
    strncpy(user_passwd, user_pw, MAX_PWD_LEN);
  }
  else
  {
  	QTextCodec * codec = QTextCodec::codecForLocale();
  	QString m = qApp->translate("XWDVIEncrypt", encrypt_strings[1]);
  	QString l = qApp->translate("XWDVIEncrypt", passward_strings[1]);
  	QString tmp = xwApp->getPassward(m, l);
  	QByteArray ba = codec->fromUnicode(tmp);
		strncpy(owner_passwd, ba.data(), ba.size());
  }
	
	key_size = (unsigned char)(bits / 8);
  algorithm = (key_size == 5 ? 1 : 2);
  permission = (long) (perm | 0xC0U);
  revision = ((algorithm == 1 && permission < 0x100L) ? 2 : 3);
  if (revision == 3)
    permission |= ~0xFFFL;

  compute_owner_password();
  compute_user_password();
}

void XWDVIEncrypt::compute_encryption_key(unsigned char *pwd)
{
	passwd_padding(pwd, in_buf);
	MD5_init(&md5_ctx);
  	MD5_write(&md5_ctx, in_buf, MAX_STR_LEN);
  	MD5_write(&md5_ctx, opwd_string, MAX_STR_LEN);
  	in_buf[0] = (unsigned char)(permission) & 0xFF;
  	in_buf[1] = (unsigned char)(permission >> 8) & 0xFF;
  	in_buf[2] = (unsigned char)(permission >> 16) & 0xFF;
  	in_buf[3] = (unsigned char)(permission >> 24) & 0xFF;
  	MD5_write(&md5_ctx, in_buf, 4);
  	MD5_write(&md5_ctx, id_string, MAX_KEY_LEN);
  	MD5_final(md5_buf, &md5_ctx);
  	if (revision == 3)
  	{
  		for (uchar i = 0; i < 50; i++)
  		{
  			MD5_init(&md5_ctx);
      		MD5_write(&md5_ctx, md5_buf, key_size);
      		MD5_final(md5_buf, &md5_ctx);
  		}
  	}
  	
  	memcpy(key_data, md5_buf, key_size);
}

void XWDVIEncrypt::compute_owner_password()
{
	passwd_padding((unsigned char *)(strlen(owner_passwd) > 0 ? owner_passwd : user_passwd), in_buf);
	MD5_init(&md5_ctx);
  	MD5_write(&md5_ctx, in_buf, MAX_STR_LEN);
  	MD5_final(md5_buf, &md5_ctx);
  	if (revision == 3)
  	{
  		for (uchar i = 0; i < 50; i++)
  		{
  			MD5_init(&md5_ctx);
      		MD5_write(&md5_ctx, md5_buf, key_size);
      		MD5_final(md5_buf, &md5_ctx);
  		}
  	}
  	
  	ARC4_set_key(&key, key_size, md5_buf);
  	passwd_padding((unsigned char *)user_passwd, in_buf);
  	ARC4(&key, MAX_STR_LEN, in_buf, out_buf);
  	if (revision == 3)
  	{
    	for (uchar i = 1; i <= 19; i++) 
    	{
      		memcpy(in_buf, out_buf, MAX_STR_LEN);
      		for (uchar j = 0; j < key_size; j++)
        		key_buf[j] = md5_buf[j] ^ i;
      		ARC4_set_key(&key, key_size, key_buf);
      		ARC4(&key, MAX_STR_LEN, in_buf, out_buf);
    	}
    }
    
    memcpy(opwd_string, out_buf, MAX_STR_LEN);
}

void XWDVIEncrypt::compute_user_password()
{
	compute_encryption_key((unsigned char *)user_passwd);
	switch (revision)
	{
		case 2:
    		ARC4_set_key(&key, key_size, key_data);
    		ARC4(&key, MAX_STR_LEN, padding_string, out_buf);
    		break;
    		
  		case 3:
    		MD5_init(&md5_ctx);
    		MD5_write(&md5_ctx, (unsigned char *)padding_string, MAX_STR_LEN);
    		MD5_write(&md5_ctx, id_string, MAX_KEY_LEN);
    		MD5_final(md5_buf, &md5_ctx);
    		ARC4_set_key(&key, key_size, key_data);
    		ARC4(&key, MAX_KEY_LEN, md5_buf, out_buf);
    		for (uchar i = 1; i <= 19; i++) 
    		{	
      			memcpy(in_buf, out_buf, MAX_KEY_LEN);
      			for (uchar j = 0; j < key_size; j++)
        			key_buf[j] = key_data[j] ^ i;
      			ARC4_set_key(&key, key_size, key_buf);
      			ARC4(&key, MAX_KEY_LEN, in_buf, out_buf);
    		}
    		break;
    		
  		default:
  			break;
	}
	
	memcpy(upwd_string, out_buf, MAX_STR_LEN);
}

void XWDVIEncrypt::passwd_padding(unsigned char *src, unsigned char *dst)
{
	int len = strlen((char *)src);
  	if (len > MAX_STR_LEN)
    	len = MAX_STR_LEN;

  	memcpy(dst, src, len);
  	memcpy(dst+len, padding_string, MAX_STR_LEN-len);
}

