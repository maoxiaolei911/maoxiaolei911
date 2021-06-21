/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDEVICEPSDF_H
#define XWPSDEVICEPSDF_H

#include "XWPSDevice.h"

class XWPSDevicePsdf : public XWPSDeviceVector
{
	Q_OBJECT 

public:
	XWPSDevicePsdf(QObject * parent = 0);
	XWPSDevicePsdf(const char * devname,
	                 int w,
	                 int h,
	                 float xdpi,
	                 float ydpi,
	                 int nc,
	                 int depth,
	                 ushort mg, 
	                 ushort mc,
	                 ushort dg,
	                 ushort dc,
	                 PSDFVersion v,
	                 bool ascii,
	                 QObject * parent = 0);
	                 
	int beginPagePSDF() {return 0;}
	int beginPathPSDF(PSPathType) {return 0;}
	
	int closePathPSDF(double, 
	                      double,
		                    double, 
		                    double, 
		                    PSPathType);
	virtual int  copyDevice(XWPSDevice **pnew);
	virtual void copyDeviceParamPSDF(XWPSDevicePsdf * proto);
	int curveToPSDF(double x0, 
	                    double y0,
			                double x1, 
			                double y1, 
			                double x2, 
			                double y2,
			                double x3, 
			                double y3, 
			                PSPathType);
			                
	int doRectPSDF(long x0, long y0, long x1, long y1, PSPathType type);
	
	int endPathPSDF(PSPathType) {return 0;}
	
	int getParamsPSDF(XWPSContextState * ctx, XWPSParamList *plist);
	
	int lineToPSDF(double, double, double x, double y, PSPathType);
	
	int moveToPSDF(double, double, double x, double y, PSPathType);
	
	int putParamsPSDF(XWPSContextState * ctx, XWPSParamList *plist);
	
	int setDashPSDF(const float *pattern, uint count, double offset);
	int setFillColorPSDF(XWPSDeviceColor * pdc);
	int setFlatPSDF(double flatness);
	int setLineCapPSDF(PSLineCap cap);
	int setLineJoinPSDF(PSLineJoin join);
	int setLineWidthPSDF(double widthA);
	int setLogOpPSDF(uint, uint) {return 0;}
	int setMiterLimitPSDF(double limit);
	int setStrokeColorPSDF(XWPSDeviceColor * pdc);
	
public:
	PSDFVersion version;
	bool binary_ok;
	XWPSPSDFDistillerParams params;
	
protected:
	ulong adjustColorIndexPSDF(ulong color);
	
	static bool objnameIsValid(const uchar *data, uint size);
	
	bool psSourceOk(PSParamString * psource);
	int putImageParams(XWPSContextState * ctx,
	                   XWPSParamList * plist,
		                 const PSPSDFImageParamNames * pnames,
		                 XWPSPSDFImageParams * params, 
		                 int ecode);
		      
	int setColor(XWPSDeviceColor * pdc, const PSPSDFSetColorCommands *ppscc);
	
	int writeString(XWPSStream * s, const uchar * str, uint size);
};

class XWPSPSDFBinaryWriter
{
public:
	XWPSPSDFBinaryWriter();
	~XWPSPSDFBinaryWriter();
	
	int beginBinary(XWPSDevicePsdf * pdev);
	
	int CFEBinary(int w, int h, bool invert);
	static int DCTFilter(XWPSContextState * ctx,
	                     XWPSParamList *plist,
								       PSStreamState *st,
								       int Columns, 
								       int Rows, 
								       int Colors,
								       XWPSPSDFBinaryWriter *pbw, 
								       bool dss);
	
	int encodeBinary(PSStreamTemplate * templat, PSStreamState * ss, bool dss);
	int endBinary();
	
	int pixelResize(int width, int num_components, int bpc_in, int bpc_out);
	
	int setupDownSampling(XWPSPSDFImageParams * pdip,
		   									 XWPSPixelImage * pim, 
		   									 float resolution);
	int setupImageCompression(XWPSPSDFImageParams *pdip, XWPSPixelImage * pim);
	int setupImageFilters(XWPSDevicePsdf * pdev, 
			 								 XWPSPixelImage * pim, 
			 								 XWPSMatrix * pctm,
			 							   XWPSImagerState * pis);
	int setupLosslessFilters(XWPSDevicePsdf *pdev, XWPSPixelImage *pim);
	
public:
	XWPSStream *A85E;
	XWPSStream *target;
	XWPSStream *strm;
	XWPSDevicePsdf*dev;
};

#endif //XWPSDEVICEPSDF_H
