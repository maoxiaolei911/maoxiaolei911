/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXIODEV_H
#define XWTEXIODEV_H

#include <QtGlobal>
#include <QObject>
#include <QThread>
#include <QByteArray>
#include <QHash>
#include <QStack>
#include <QTextCodec>
#include <QIODevice>
#include <QString>
#include <QList>
#include <QPair>
#include <QMutex>
#include <QMutexLocker>

#include "XWConst.h"
#include "TeXType.h"

#if defined(XW_BUILD_TEX_LIB)
#define XW_TEX_EXPORT Q_DECL_EXPORT
#else
#define XW_TEX_EXPORT Q_DECL_IMPORT
#endif

class QFile;
class XWTeX;

class XW_TEX_EXPORT XWTeXFile : public QObject
{
	Q_OBJECT
	
public:
	XWTeXFile(XWTeX * texA, QObject * parent = 0);
	~XWTeXFile();
	
	void addChild(XWTeXFile * f);
	
	bool atEnd();
		
	XWTeXFile * findFile(const QString & filename);
	          
	QByteArray      getByteArray(const QString & txt);
	TeXCategoryCode getCategory(qint32 c);
	TeXCategoryCode getCategory();
	qint32          getChar();
	qint32          getControlSequence(qint32 * buf);
	QString         getCoord();	
	TeXFileMode     getFileMode() {return fileMode;}
	QString         getKey();
	QString         getOption();
	QString         getParam();
	XWTeXFile    *  getParentFile() {return parentFile;}
	qint32          getPos();
	QString         getString(qint32 len);
	qint32          getSize();
	QString         getText();
	QString         getWord();	
	QString         getValue();
	
	XWTeXFile * hitTest(qint32 filepos);
	
	qint32 insert(const QByteArray & txt);
	qint32 insert(const QString & txt);
	
	bool load(const QString & filename);
	
	QIODevice * open();
	
	QByteArray read(qint32 len);
	QString    readLine();
	void       remove(qint32 len);
	void       removeFile(XWTeXFile * f);
	qint32     replace(qint32 len, const QByteArray & txt);
	qint32     replace(qint32 len, const QString & txt);
	
	void save();
	void save(const QString & filename);
	void seek(qint32 filepos);
	void setTotalPos(qint32 p) {totalPos = p;}	
	void setParentFile(XWTeXFile * f) {parentFile = f;}
	void skipComment();
	void skipControlSequence();
	void skipCoord();
	void skipFormula();
	void skipGroup();
	void skipOption();
	void skipSpacer();
	void skipSpacerAndComment();
	void skipWord();
	
	void updateFilePos(qint32 filepos, 
	                   qint32 l);
	
private:	
	XWTeX      *  tex;	
	QTextCodec * codec;
	qint32        totalPos;
	qint32        curPos;
	qint32        totalLen;
	bool          modified;
	QString       fileName;
	QString       fileNameInDoc;
	QString       text;
	TeXFileMode   fileMode;
	XWTeXFile   * parentFile;
	int fileNum;
	QHash<QString, XWTeXFile*> children;
};

class XW_TEX_EXPORT XWTeXIODev : public QThread
{
	Q_OBJECT
	
public:
	friend class XWTeX;
		
	XWTeXIODev(QObject * parent = 0);
	virtual ~XWTeXIODev();
	
	virtual void buildFmt(const QString & fmtname, bool etexA);
	virtual void buildFmtNow(const QString & fmtname, bool etexA);
	virtual void compile(); 
	
	QString getDumpName() {return dumpName;}
	QString getJobName()  {return jobName;}
	QString getMainInputFile() {return mainInputFile;}
	QString getOutputComment() {return outputComment;}
	
	bool hasDumpName() {return !dumpName.isEmpty();}
	bool hasMainInputFile() {return !mainInputFile.isEmpty();}
	
	bool isIniVersion();
	bool isOk() {return ok;}
	
	virtual bool  loadFile(const QString & filename);
	virtual bool  loadFmt(const QString & fmtname);
	virtual bool  loadFmt();
	
	QString parseFirstLine(QIODevice * fileA);
	virtual bool parseOptions(int argc, char ** argv);
	
			void setDebugFormatFile(bool e);
			void setFileLineErrorStyle(bool e);
			void setHaltOnError(bool e);
			void setIniVersion(bool ini);
			void setInsertSrcSpecialAuto(bool e);
			void setInsertSrcSpecialEveryCr(bool e);
			void setInsertSrcSpecialEveryDisplay(bool e);
			void setInsertSrcSpecialEveryHBox(bool e);
			void setInsertSrcSpecialEveryMath(bool e);
			void setInsertSrcSpecialEveryPar(bool e);
			void setInsertSrcSpecialEveryParEnd(bool e);
			void setInsertSrcSpecialEveryVBox(bool e);
			void setInteractionOption(qint32 i);
			void setMainInputFile(const QString & filename);
			void setOutputComment(const QString & s);
			void setOutputDirectory(const QString & s);
			void setParseFirstLine(bool e);
			void setShellEnabled(bool e);
			void setSrcSpecialsP(bool e);
			
			QIODevice * takeDviFile();
			
public slots:
	virtual void setInputText(const QString & txt);
	
signals:
	void messageAdded(const QString & s);
	void needFile(const QString & s);
	void requestInput();
				
protected:
	virtual void dviBop();
			void dviDown(qint32 ht);
			void dviDownRight(qint32 o, 
	                          qint32 w);
			void dviEnd();
	virtual void dviEop();
	void dviFont(qint32 ocmd, qint32 oval);
	void dviFour(qint32 x);
	void dviLeft(qint32 wd);
	void dviOut(uchar c);
	void dviPop();
	void dviPop(qint32 l);
	void dviPost();
	void dviPre();
	void dviPush();
	virtual void dviPut(qint32 , 
	                    qint32 ocmd, 
	                    qint32 oval);
	virtual void dviPutRule(qint32 , 
	                        qint32 ht,
	                        qint32 wd);
	void dviRight(qint32 wd);
	virtual void dviSet(qint32 , 
	                    qint32 ocmd, 
	                    qint32 oval);
	virtual void dviSetRule(qint32 , 
	                        qint32 ht,
	                        qint32 wd);
	void    dviSwap();
	void    dviTwo(qint32 s);
	void    dviUp(qint32 ht);
	virtual void dviXXX(qint32 , qint32 l);
		
	virtual XWTeXFile * newTeXFile();
	
	virtual void        openLogFile();	
	virtual QIODevice * openInput(const QString & filename);
	virtual QFile     * openOutput(const QString & filename);
	
	void outCmd(qint32 ocmd, qint32 oval);
	
	virtual void quit(int code);
	
	virtual void run();
	
	virtual QString termInput();
	virtual void termOut(const QString & s);
	virtual void termPutChar(qint32 c);
	
	virtual void writeDvi(qint32 a, qint32 b);
		
protected:
	XWTeX * tex;
	bool    etex;
	QIODevice * dvi_file;
		
	QIODevice * term_in;
	QIODevice * term_out;
	
	QIODevice * log_file;
	
	qint32  dvi_ptr;
	qint32  dvi_offset;
	
	qint32  dvi_buf_size;
	uchar * dvi_buf;
	qint32  half_buf;
	qint32  dvi_limit;	
	qint32  dvi_gone;
	
	bool    inputFinished;
	QString inputStr;
	QMutex  myMutex;
	
	QString jobName;
	QString dumpName;
	QString outputDirectory;
	QString outputComment;
	
	QString mainInputFile;
	
	QString fullNameOfFile;
	bool ok;
};

inline void XWTeXIODev::dviDown(qint32 ht)
{
	dviOut((uchar)DVI_DOWN4); 
	dviFour(ht);
}

inline void XWTeXIODev::dviFont(qint32 ocmd, qint32 oval)
{
	outCmd(ocmd, oval);
}

inline void XWTeXIODev::dviFour(qint32 x)
{
	if (x >= 0)
		dviOut((uchar)(x / 0x1000000));
	else
	{
		x = x + 0x40000000;
		x = x + 0x40000000;
		dviOut((uchar)((x / 0x1000000) + 128));
	}
	
	x = x % 0x1000000;
	dviOut(uchar(x / 0x10000));
	x = x % 0x10000;
	dviOut((uchar)(x / 0x100));
	dviOut((uchar)(x % 0x100));
}

inline void XWTeXIODev::dviLeft(qint32 wd)
{
	dviOut((uchar)DVI_RIGHT4); 
	dviFour(-wd);
}

inline void XWTeXIODev::dviPop()
{
	dviOut((uchar)DVI_POP);
}

inline void XWTeXIODev::dviPop(qint32 l)
{
	if ((l == dvi_offset + dvi_ptr) && (dvi_ptr > 0))
		dvi_ptr--;
	else
		dviOut((uchar)DVI_POP);
}

inline void XWTeXIODev::dviOut(uchar c)
{
	dvi_buf[dvi_ptr] = c; 
	dvi_ptr++;
	if (dvi_ptr == dvi_limit)
		dviSwap();
}

inline void XWTeXIODev::dviPost()
{
	dviOut((uchar)DVI_POST);
}

inline void XWTeXIODev::dviPre()
{
	dviOut((uchar)DVI_PRE); 
	dviOut((uchar)DVI_ID_BYTE);
}

inline void XWTeXIODev::dviPush()
{
	dviOut((uchar)DVI_PUSH);
}

inline void XWTeXIODev::dviRight(qint32 wd)
{
	dviOut((uchar)DVI_RIGHT4);
	dviFour(wd);
}

inline void XWTeXIODev::dviUp(qint32 ht)
{
	dviOut((uchar)DVI_DOWN4); 
	dviFour(-ht);
}

inline void XWTeXIODev::outCmd(qint32 ocmd, qint32 oval)
{
	if ((oval < 0x100) && (oval >= 0))
	{
		if ((ocmd != DVI_SET1) || (oval > 127))
		{
			if ((ocmd == DVI_FNT1) &&(oval < 64))
				oval += DVI_FNTNUM0;
			else
				dviOut((uchar)ocmd);
		}
	}
	else
	{
		if ((oval < 0x10000) && (oval >= 0))
			dviOut((uchar)(ocmd + 1));
		else
		{
			if ((oval < 0x1000000) && (oval >= 0))
				dviOut((uchar)(ocmd + 2));
			else
			{
				dviOut((uchar)(ocmd + 3));
				if (oval >= 0)
					dviOut((uchar)(oval / 0x1000000));
				else
				{
					oval += 0x40000000;
					oval += 0x40000000;
					dviOut((uchar)((oval / 0x1000000) + 128));
					oval = oval % 0x1000000;
				}
				
				dviOut((uchar)(oval / 0x10000));
				oval = oval % 0x10000;
			}
			
			dviOut((uchar)(oval / 0x10000));
			oval = oval % 0x10000;
		}
		
		dviOut((uchar)(oval / 0x100));
		oval = oval % 0x100;
	}
	
	dviOut((uchar)oval);
}


class XW_TEX_EXPORT XWTeXStdIODev : public XWTeXIODev
{
	Q_OBJECT
	
public:
	XWTeXStdIODev(QObject * parent = 0);
};

#endif //XWTEXIODEV_H
