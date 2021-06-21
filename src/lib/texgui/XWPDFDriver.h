/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPDFDRIVER_H
#define XWPDFDRIVER_H

#include <QHash>
#include <QList>
#include <QTransform>
#include <QPointF>

#include "XWDoc.h"
#include "XWRef.h"
#include "XWCMap.h"
#include "XWTeXGuiType.h"

class XWFontFileSFNT;
class XWFontFileCFF;
class XWType0Font;
struct SFNTCmap4;
struct SFNTCmap12;
class XWSFNTCmap;
class XWOTLGsub;
class XWCIDWidths;
class XWCMap;
class XWObject;
class XWDict;
class XWTexFont;
class XWPDFOTLConf;

class XW_TEXGUI_EXPORT XWPDFDriverRef : public XWRef
{
public:
  XWPDFDriverRef(double w, double h);
  ~XWPDFDriverRef();

  void addColorSpace(XWObject * csobj, const QString &shortname, XWObject * ref);
  void addExtGState(XWObject * egsobj, const QString &shortname, XWObject * ref);
  void addFont(XWObject * font, const QString &shortname, XWObject * ref);
  void addObject(XWObject * obj, XWObject * ref);
  void addPageContent(const char *buffer, unsigned length);
  void addPattern(XWObject * paobj, const QString &shortname, XWObject * ref);
  void addXObject(XWObject * xobj, const QString &shortname, XWObject * ref);

  XWObject * fetch(int num, int gen, XWObject *obj, int recursion = 0);

  XWObject * getCatalog(XWObject *obj);
  int        getNumObjects() {return numObjects;}
  bool       getStreamEnd(uint streamStart, uint *streamEnd);

  void popStream();
  void pushStream(XWObject * obj, XWObject * ref);

private:
  int nextLabel;
  int numObjects;
  int curStream;
  XWObject * objs;
  XWDict * fontDict;
  XWDict * xobjDict;
  XWDict * extGSDict;
  XWDict * colorSpaceDict;
  XWDict * shadingDict;
  XWDict * patternDict;
  QList<XWObject * > streams;
};

class XW_TEXGUI_EXPORT XWPDFDriver : public XWDoc
{
  Q_OBJECT

public:
  XWPDFDriver(double w, double h,QObject * parent = 0);
  ~XWPDFDriver();

  void addFuns(XWObject * obj, XWObject* funs,int len);
  void addPageContent(const char *buffer, unsigned length);

  void beginFading(const QString & fadname,
                         XWObject * bbox,
                         XWObject * matrix,
                         XWObject * res,
                         XWObject * ref);
  void beginPattern(const QString & patname,
                         int ptype,
                         double llx, double lly,
                         double urx,double ury,
                         double xstep,double ystep,
                         XWObject * ref);
  void beginTransparency(const QString & transname,
                         XWObject * bbox,
                         XWObject * matrix,
                         XWObject * res,
                         XWObject * ref);

  void closePath();
  void curveTo(const QPointF & c1,const QPointF & c2,const QPointF & e);

  void display(QPainter * painter);
  void display(QPainter * painter,double scale);

  void endPath();
  void endPattern();
  void endXForm();
  void evenoddClip();
  void evenoddCloseFillStroke();
  void evenoddFill();
  void evenoddFillStroke();

  void fillText();
  void fillStrokeText();

  void grestore();
  void gsave();

  bool hasColorSpace(const QString & csname);
  bool hasPattern(const QString & patname,XWObject * ref);
  bool hasXObject(const QString & xobjname,XWObject * ref);
  
  XWObject * initArray(XWObject * obj);
  XWObject * initBBox(double llx, double lly,
                      double urx,double ury,
                      XWObject * obj);
  XWObject * initDict(XWObject * obj);
  XWObject * initExtGState(double so,double fo,XWObject * obj);
  XWObject * initFunc2(double doma, double domb, 
                       double * c0, int c0len, 
                       double * c1, int c1len,
                       double n,
                       XWObject * obj);
  XWObject * initFunc3(double doma, double domb, 
                       double * bounds, int blen,
                       double * enc ,int elen,
                       XWObject * obj);
  XWObject * initFunc4(double * dom, int dlen, 
                          double * range, int rlen,
                          const char * code,int len,
                          XWObject * obj);
  XWObject * initMatrix(double a,double b,double c, double d,
                        double e, double f,XWObject * obj);
  XWObject * initProcSet(XWObject * obj);
  XWObject * initShading1(double * dom, int dlen, 
                          double * m, int mlen,
                          XWObject * func,
                          XWObject * obj);
  XWObject * initShading23(int stype,
                          double doma, double domb, 
                          double * coords, int clen,
                          XWObject * func,
                          XWObject * obj);
  XWObject * initStream(XWObject * obj);
  void invisibleText();

  void lineTo(const QPointF & p);
  void lineTo(double xA,double yA);

  void moveTo(const QPointF & p);
  void moveTo(double xA,double yA);

  void newPath();
  void nonzeroClip();
  void nonzeroCloseFillStroke();
  void nonzeroFill();
  void nonzeroFillStroke();

  void setChar(double xpos,double ypos, const QChar & c);
  void setColorSpace(const QString & csname, XWObject * obj);
  void setExtGState(const QString & gsname, XWObject * obj);
  void setFillColor(const QColor & rgb);
  void setFillColorSpace(const QString & csname);
  void setFillPattern(const QString & patname);
  void setFont(const QString & fontname,double size, bool sys = false);
  void setLineCap(int capstyle);
  void setLineDash(const QVector<qreal> & pattern,double offset);
  void setLineJoin(int joinstyle);
  void setLineWidth(double width);
  void setMatrix(double a,double b,double c,double d,double e,double f);
  void setMiterLimit(double mlimit);
  void setProcSet(const QString & psname, XWObject * obj);
  void setString(double xpos,double ypos, const QString & str);
  void setStrokeColor(const QColor & rgb);
  void setStrokColorSpace(const QString & csname);
  void setStrokePattern(const QString & patname);
  void setTextMatrix(double a,double b,double c,double d,double e,double f);
  void shadingFunc(const QString & shadingname,
                   const QPointF & ll,
                   const QPointF & ur,
                   const char * code,int len,
                   XWObject * ref);
  void shadingHoriVert(const QString & shadingname,
                   bool hori,
                   double h,
                   double funs[][4],int nfuns,
                   XWObject * ref);
  void shadingRadial(const QString & shadingname,
                   const QPointF & p,
                   double funs[][4],int nfuns,
                   XWObject * ref);
  void stroke();
  void strokeText();

  void useExtGState(const QString & name);
  void useFading(const QString & name,XWObject * stm);
  void usePattern(const QString & name);
  void usePattern(const QString & csname,
                  const QColor & rgb,
                  const QString & patname);
  void useXObject(const QString & name);

private:
  void addToUnicode(XWType0Font * font, XWObject * fontdict);

  XWObject * createCMapStream(XWCMap *cmap,XWObject * obj);
  XWObject * createDummyCMap(XWObject * obj);
  XWObject * createToUnicodeCMap4(SFNTCmap4 *map,
									            const char *cmap_name, 
									            XWCMap *cmap_add,
									            const char *used_glyphs,
									            XWObject * obj);
  XWObject * createToUnicodeCMap12(SFNTCmap12 * map,
			 						             const char *cmap_name, 
			 						             XWCMap *cmap_add,
			 						             const char *used_glyphs,
			 						             XWObject * obj);

  void doType0(const QString & family,
                 double size,
                 const QString & shortname);
  void doType1(const char * fontname,
                 const char * map_name,
                 double size,
                 const QString & shortname);
  int doubleToAscii(double value, int prec, char *buf);

  void flushFonts();
  void flushFont(int fonttype,
                 const QString & fontname,
                 const QString & shortname,
                 double size);

  void getCIDWidths(XWCIDWidths * w, XWObject * w_array);
  void graphicsMode();

  int handleCIDFont(XWFontFileSFNT *sfont,
					              uchar **GIDToCIDMap, 
					              CIDSysInfo *csi);
  ushort handleSubstGlyphs(XWCMap *cmap, 
	                         XWCMap *cmap_add, 
	                         const char *used_glyphs);

  XWObject * initXForm(const QString & fname,
                        XWObject * form,
                        XWObject * bbox,
                        XWObject * matrix,
                        XWObject * res,
                        XWObject * ref);

  int intToAscii(long value, char *buf);

  int loadBaseCMap(const char *cmap_name, 
	                             int wmode,
		                         CIDSysInfo *csi, 
		                         uchar *GIDToCIDMap,
					             XWSFNTCmap *ttcmap);
  int loadGsub(XWObject *conf, 
	                         XWOTLGsub *gsub_list, 
	                         XWFontFileSFNT *sfont);
  XWObject * loadToUnicodeStream(const char *ident, XWObject * obj);

  XWObject * otfCreateToUnicodeStream(const char *font_name,
			     					                int ttc_index,
			                              const char *used_glyphs,
			                              XWObject * obj);
  int otfLoadUnicodeCMap(const char *map_name, 
	                       int ttc_index,
		                     const char *otl_tags, 
		                    int wmode);

  XWObject * readToUnicodeFile(const char *cmap_name, XWObject * obj);

  int  sprintCoord(char *buf, const QPointF & p);
  int  sprintLength (char *buf, double value);
  void stringMode();

private:
  int motion_state;
  int precision;
  int nextFont;
  int nextForm;
  int nextExtGS;
  int nextShading;
  int nextColorSpace;
  int nextPattern;
  bool    is_mb;
  double  ref_x;
  double  ref_y;
  QPointF currentPos;
  
  XWPDFOTLConf * otlConf;

  QString curFont;
  QString curXObj;
  QHash<QString, int> fontType;
  QHash<QString, double> fontSize;
  QHash<QString, QString> fontShortNames;
  QHash<QString, QString> xobjShortNames;
  QHash<QString, QString> extGSShortNames;
  QHash<QString, QString> colorSpaceShortNames;
  QHash<QString, QString> patternShortNames;

  QHash<QString, int> xobjRefs;
  QHash<QString, int> patternRefs;
};

#endif //XWPDFDRIVER_H
