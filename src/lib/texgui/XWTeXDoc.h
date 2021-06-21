/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXDOC_H
#define XWTEXDOC_H

#include <QList>
#include <QUndoStack>
#include <QRectF>
#include <QPointF>
#include "TeXType.h"
#include "XWDoc.h"
#include "XWDVIType.h"
#include "XWTeXGuiType.h"

class QUndoCommand;
class XWTeXHash;
class XWTeXGuiDev;

class XW_TEXGUI_EXPORT XWTeXDoc : public XWDoc
{
	Q_OBJECT
	
public:
	XWTeXDoc(QObject * parent = 0);
	virtual ~XWTeXDoc();
	
					bool atEnd();
	
	virtual void compile(); 
	QAction * createRedoAction(QObject * parent, const QString & prefix = QString()) const;
	QAction * createUndoAction(QObject * parent, const QString & prefix = QString()) const;
			
	virtual void displayPage(XWOutputDev *out, 
                             int page,
		                     double hDPI, 
		                     double vDPI, 
		                     int rotate,
		                     bool useMediaBox, 
		                     bool crop, 
		                     bool printing,
		                     bool (*abortCheckCbk)(void *data) = 0,
		                     void *abortCheckCbkData = 0);
		                     
	virtual void displayPageSlice(XWOutputDev *out, 
	                              int page,
			                      double hDPI, 
			                      double vDPI, 
			                      int rotate,
			                      bool useMediaBox, 
			                      bool crop, 
			                      bool printing,
			                      int sliceX, 
			                      int sliceY, 
			                      int sliceW, 
			                      int sliceH,
			                      bool (*abortCheckCbk)(void *data) = 0,
			                      void *abortCheckCbkData = 0);
	
					void endInput();
					
	virtual XWLinkDest * findDest(XWString * name);
  virtual int findPage(int num, int gen);
	
					QByteArray      getByteArray(const QString & txt);
					TeXCategoryCode getCategory();
					qint32          getChar();
	virtual qint32          getControlSequence();
	        qint32          getControlSequence(qint32 * buf);
	virtual QString         getCoord();
					XWTeXGuiDev  *  getDev() {return dev;}
					QString         getDumpName();
					TeXFileMode     getFileMode();					
					DVIFontDef *    getFontDefs();
					QString         getJobName();
	virtual QString         getKey();
	virtual XWLinks       * getLinks(int page);
					QString         getMainInputFile();
					int             getNumberOfDefFont();
	virtual int getNumPages();
	virtual QString         getOption();		
					QString         getOutputComment();
	virtual double      getPageCropHeight(int page);
  virtual double      getPageCropWidth(int page);
  virtual double      getPageMediaHeight(int page);
  virtual double      getPageMediaWidth(int page);
  virtual int         getPageRotate(int page);
	virtual QString         getParam();
					qint32          getPos();
	        qint32          getSize();
	        QString         getString(qint32 filepos, qint32 len);
	        QString         getText();
	        QString         getValue();
	        QString         getWord();
	        
	        bool hasFmt();
	
	        qint32 insert(qint32 filepos, const QByteArray & txt);
					qint32 insert(qint32 filepos, const QString & txt);
	        qint32 insertFile(qint32 filepos, const QString & filename);
	        	
	virtual bool   loadFile(const QString & filename);
	virtual bool   loadFmt(const QString & filename);
					
	virtual void processLinks(XWOutputDev *out, int page);
	virtual void pushCommand(QUndoCommand * cmd);
	
	        QByteArray read(qint32 filepos, qint32 len);
	        QString    readLine();
	virtual void       redo();
	        void       remove(qint32 filepos, qint32 len);
	        qint32     removeFile(qint32 filepos, const QString & filename);
	        qint32     replace(qint32 filepos, 
	                           qint32 len, 
	                           const QByteArray & txt);
	        qint32     replace(qint32 filepos, 
	                           qint32 len, 
	                           const QString & txt);
	
			void save();
			void save(const QString & filename);
			void saveToDVI(QIODevice * outfileA);
			void saveToPDF(QIODevice * outfileA,
	                  char *dviname, 
	                  char *pdfname,
	                  const QString & thumbbasename,
	                  const QList<int> & pages,
	                  char * owner_pw,
	                  char * user_pw);
			    bool seek(qint32 filepos);
			    void setOutputComment(const QString & s);
	        void skipComment();
	        void skipControlSequence();
	virtual void skipCoord();
	        void skipFormula();
			void skipGroup();
	virtual void skipOption();
			void skipSpacer();
			void skipSpacerAndComment();
			void skipStarred();
			void skipWord();
	
	virtual void undo();
	
signals:
	void contentChanged(qint32 filepos, qint32 len);

protected:
	XWTeXGuiDev * dev;
	
	QUndoStack  * undoStack;
};

#endif //XWTEXDOC_H

