/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWOUTPUTDEV_H
#define XWOUTPUTDEV_H

#include <QString>
#include <QStringList>
#include "XWObject.h"

class QProcess;

class XWString;
class XWDict;
class XWGraphix;
class XWGraphixState;
struct CoreColor;
class XWColorSpace;
class XWImageColorMap;
class XWFunctionShading;
class XWAxialShading;
class XWRadialShading;
class XWStream;
class XWLinks;
class XWLink;
class XWCatalog;
class XWPage;
class XWFunction;

class XW_CORE_EXPORT XWOutputDev
{
public:
    XWOutputDev();
    virtual ~XWOutputDev();
    
    virtual bool axialShadedFill(XWGraphixState *, XWAxialShading *)
                { return false; }
    
    virtual void beginActualText(XWGraphixState *, uint *, int) {}
    virtual void beginString(XWGraphixState *, XWString *) {}
    virtual void beginStringOp(XWGraphixState *) {}
    virtual void beginTransparencyGroup(XWGraphixState *, 
                                        double *,
				                        XWColorSpace *,
				                        bool , 
				                        bool ,
				                        bool ) {}
    virtual bool beginType3Char(XWGraphixState *, 
                                double , 
                                double ,
			                    double , 
			                    double ,
			                    uint , 
			                    uint *, int);
    
    virtual bool checkPageSlice(XWPage *, 
                                double , 
                                double ,
			                    int , 
			                    bool , 
			                    bool ,
			                    int , 
			                    int , 
			                    int , 
			                    int ,
			                    bool , 
			                    bool (*)(void *) = 0,
			                    void * = 0)
                { return true; }
    virtual void clearSoftMask(XWGraphixState *) {}
    virtual void clip(XWGraphixState *) {}
    virtual void clipToStrokePath(XWGraphixState *) {}
    virtual void cvtDevToUser(double dx, double dy, double *ux, double *uy);
    virtual void cvtUserToDev(double ux, double uy, int *dx, int *dy);
    
    virtual void drawChar(XWGraphixState *, 
                          double , 
                          double ,
			              double , 
			              double ,
			              double , 
			              double ,
			              uint , 
			              int , 
			              uint *, 
			              int ) {}
    virtual void drawForm(ObjRef ) {}
    virtual void drawImage(XWGraphixState *, 
                           XWObject *, 
                           XWStream *str,
			               int width, 
			               int height, 
			               XWImageColorMap *colorMap,
			               int *, 
			               bool inlineImg);
    virtual void drawImageMask(XWGraphixState *, 
                               XWObject *, 
                               XWStream *str,
			                   int width, 
			                   int height, 
			                   bool ,
			                   bool inlineImg);
    virtual void drawMaskedImage(XWGraphixState *state, 
                                 XWObject *ref, 
                                 XWStream *str,
			                     int width, 
			                     int height,
			                     XWImageColorMap *colorMap,
			                     XWStream *, 
			                     int , 
			                     int ,
			                     bool );
    virtual void drawSoftMaskedImage(XWGraphixState *state, 
                                     XWObject *ref, 
                                     XWStream *str,
				                     int width, 
				                     int height,
				                     XWImageColorMap *colorMap,
				                     XWStream *,
				                     int , 
				                     int ,
				                     XWImageColorMap *);
    virtual void drawString(XWGraphixState *, XWString *) {}
    virtual void dump() {}
    
    virtual void endActualText(XWGraphixState *) {}
    virtual void endPage() {}
    virtual void endString(XWGraphixState *) {}
    virtual void endStringOp(XWGraphixState *) {}
    virtual void endTextObject(XWGraphixState *) {}
    virtual void endTransparencyGroup(XWGraphixState *) {}
    virtual void endType3Char(XWGraphixState *) {}
    virtual void eoClip(XWGraphixState *) {}
    virtual void eoFill(XWGraphixState *) {}
    
    virtual void fill(XWGraphixState *) {}
    virtual bool functionShadedFill(XWGraphixState *, XWFunctionShading *)
                    { return false; }
    
    double *getDefCTM() { return defCTM; }
    double *getDefICTM() { return defICTM; }
    virtual bool getVectorAntialias() { return false; }
    
    virtual void incCharCount(int) {}
    virtual bool interpretType3Chars() = 0;
    
    virtual bool needCharCount() { return false; }
    virtual bool needNonText() { return true; }
    
    virtual void opiBegin(XWGraphixState *, XWDict *);
  	virtual void opiEnd(XWGraphixState *, XWDict *);
    
    virtual void paintTransparencyGroup(XWGraphixState *, double *) {}
    virtual void processLink(XWLink *, XWCatalog *) {}
    virtual void psXObject(XWStream *, XWStream *) {}
    
    virtual bool radialShadedFill(XWGraphixState *, XWRadialShading *)
                    { return false; }
    virtual void restoreState(XWGraphixState *) {}
    virtual void restoreTextPos(XWGraphixState *) {}
    
    virtual void saveState(XWGraphixState *) {}
    virtual void saveTextPos(XWGraphixState *) {}
    virtual void setDefaultCTM(double *ctm);
    virtual void setInShading(bool) {}
    virtual void setSoftMask(XWGraphixState *, 
                             double *, bool ,
			                 XWFunction *, 
			                 CoreColor *) {}
		virtual void setSoftMaskFromImageMask(XWGraphixState *state,
																					XWObject *ref, 
																					XWStream *str,
					                                int width, 
					                                int height, 
					                                bool invert,
					                                bool inlineImg);
    virtual void setVectorAntialias(bool) {}
    virtual void startPage(int, XWGraphixState *) {}
    virtual void stroke(XWGraphixState *) {}
    
    virtual void tilingPatternFill(XWGraphixState *, 
                                   XWGraphix *, 
                                   XWObject *,
				                           int , 
				                           XWDict *,
				                           double *, 
				                           double *,
				                           int , 
				                           int , 
				                           int , 
				                           int ,
				                           double, 
				                           double) {}
    virtual void type3D0(XWGraphixState *, double , double ) {}
    virtual void type3D1(XWGraphixState *, double , double ,
		                 double , double , double , double ) {}
    
    virtual void updateAll(XWGraphixState *state);
    virtual void updateBlendMode(XWGraphixState *) {}
    virtual void updateCharSpace(XWGraphixState *) {}
    virtual void updateCTM(XWGraphixState *, 
                           double , 
                           double ,
			               double , 
			               double , 
			               double , 
			               double ) {}
    virtual void updateFillColor(XWGraphixState *) {}
    virtual void updateFillColorSpace(XWGraphixState *) {}
    virtual void updateFillOpacity(XWGraphixState *) {}
    virtual void updateFillOverprint(XWGraphixState *) {}
    virtual void updateFlatness(XWGraphixState *) {}
    virtual void updateFont(XWGraphixState *) {}
    virtual void updateHorizScaling(XWGraphixState *) {}
    virtual void updateLineCap(XWGraphixState  *) {}
    virtual void updateLineDash(XWGraphixState *) {}
    virtual void updateLineJoin(XWGraphixState *) {}
    virtual void updateLineWidth(XWGraphixState *) {}
    virtual void updateMiterLimit(XWGraphixState *) {}
    virtual void updateOverprintMode(XWGraphixState *) {}
    virtual void updateRender(XWGraphixState *) {}
    virtual void updateRise(XWGraphixState *) {}
    virtual void updateStrokeAdjust(XWGraphixState *) {}
    virtual void updateStrokeColor(XWGraphixState *) {}
    virtual void updateStrokeColorSpace(XWGraphixState *) {}
    virtual void updateStrokeOpacity(XWGraphixState *) {}
    virtual void updateStrokeOverprint(XWGraphixState *) {}
    virtual void updateTextMat(XWGraphixState *) {}
    virtual void updateTextPos(XWGraphixState *) {}
    virtual void updateTextShift(XWGraphixState *, double ) {}
    virtual void updateTransfer(XWGraphixState *) {}
    virtual void updateWordSpace(XWGraphixState *) {}
    
    //设备的坐标原点在左上角吗？
    virtual bool upsideDown() = 0;
    
    //设备要使用drawChar()或drawString()吗？
    virtual bool useDrawChar() = 0;
    virtual bool useDrawForm() { return false; }
    virtual bool useShadedFills() { return false; }
    virtual bool useTilingPatternFill() { return false; }
    
private:
    double defCTM[6];
    double defICTM[6];
};

#endif // XWOUTPUTDEV_H

