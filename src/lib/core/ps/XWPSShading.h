/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSSHADING_H
#define XWPSSHADING_H

#include "XWPSColor.h"
#include "XWPSStream.h"
#include "XWPSDataSource.h"

class XWPSDevice;
class XWPSImagerState;
class XWPSColorSpace;
class XWPSFunction;
class XWPSPath;
class XWPSShading;

class XWPSMeshVertex
{
public:
	XWPSMeshVertex() {}
	
public:
	XWPSFixedPoint p;
  float cc[PS_CLIENT_COLOR_MAX_COMPONENTS];
};

class XWPSPatchCurve
{
public:
	XWPSPatchCurve() {}
	
public:
	XWPSMeshVertex vertex;
  XWPSFixedPoint control[2];
};

class XWPSPatchColor
{
public:
	XWPSPatchColor() : t(0) {}
		
public:
	float t;
	XWPSClientColor cc;
};

class XWPSShadingParams
{
public:
	XWPSShadingParams();
	XWPSShadingParams(XWPSShadingParams * src);
	virtual ~XWPSShadingParams();
	
	
	virtual int check() {return 0;}
	int checkCBFD(XWPSFunction * function, const float *domain, int m);
	void copyCommon(XWPSShadingParams * src);
	
public:
	XWPSColorSpace *ColorSpace;
	XWPSClientColor *Background;
	bool have_BBox;
	XWPSRect BBox;
	bool AntiAlias;
};

class XWPSShadingFbParams : public XWPSShadingParams
{
public:
	XWPSShadingFbParams();
	XWPSShadingFbParams(XWPSShadingFbParams * src);
	~XWPSShadingFbParams();
	
	int check();
	void copyFb(XWPSShadingFbParams * src);
	
public:
	float Domain[4];
	XWPSMatrix Matrix;
	XWPSFunction *Function;
};

class XWPSShadingAParams : public XWPSShadingParams
{
public:
	XWPSShadingAParams();
	XWPSShadingAParams(XWPSShadingAParams * src);
	~XWPSShadingAParams();
	
	int check();
	void copyA(XWPSShadingAParams * src);
	
public:
	float Coords[4];
	float Domain[2];
	XWPSFunction *Function;
	bool Extend[2];
};

class XWPSShadingRParams : public XWPSShadingParams
{
public:
	XWPSShadingRParams();
	XWPSShadingRParams(XWPSShadingRParams * src);
	~XWPSShadingRParams();
	
	int check();
	void copyR(XWPSShadingRParams * src);
	
public:
	float Coords[6];
	float Domain[2];
	XWPSFunction *Function;
	bool Extend[2];
};

class XWPSShadingMeshParams : public XWPSShadingParams
{
public:
	XWPSShadingMeshParams();
	XWPSShadingMeshParams(XWPSShadingMeshParams * src);
	virtual ~XWPSShadingMeshParams();
	
	int checkMesh();
	void copyMesh(XWPSShadingMeshParams * src);
	
public:
	XWPSDataSource DataSource;
	int BitsPerCoordinate;
	int BitsPerComponent;
	float *Decode;
	XWPSFunction *Function;
	XWPSBytes * ds_bytes;
};

class XWPSShadingFfGtParams : public XWPSShadingMeshParams
{
public:
	XWPSShadingFfGtParams();
	XWPSShadingFfGtParams(XWPSShadingFfGtParams * src);
	
	int check();
	void copyFfGt(XWPSShadingFfGtParams * src);
	
public:
	int BitsPerFlag;
};

class XWPSShadingLfGtParams : public XWPSShadingMeshParams
{
public:
	XWPSShadingLfGtParams();
	XWPSShadingLfGtParams(XWPSShadingLfGtParams * src);
	
	int check();
	void copyLfGt(XWPSShadingLfGtParams * src);
	
public:
	int VerticesPerRow;
};

class XWPSShadingCpParams : public XWPSShadingMeshParams
{
public:
	XWPSShadingCpParams();
	XWPSShadingCpParams(XWPSShadingCpParams * src);
	
	int check();
	void copyCp(XWPSShadingCpParams * src);
	
public:
	int BitsPerFlag;
};

class XWPSShadingTppParams : public XWPSShadingMeshParams
{
public:
	XWPSShadingTppParams();
	XWPSShadingTppParams(XWPSShadingTppParams * src);
	
	int check();
	void copyTpp(XWPSShadingTppParams * src);
	
public:
	int BitsPerFlag;
};

class XWPSShadeCoordStream
{
public:
	XWPSShadeCoordStream();
	~XWPSShadeCoordStream();
	
	int getDecoded(int num_bits, const float decode[2], float *pvalue);
	int getValue(int num_bits, uint *pvalue);
	
	int  nextArrayDecoded(int num_bits,
		                    const float decode[2], 
		                    float *pvalue);
	int  nextArrayValue(int num_bits, uint * pvalue);
	int  nextColor(float *pc);
	int  nextColors(XWPSPatchCurve * curves, int num_vertices);
	int  nextCoords(XWPSFixedPoint * ppt, int num_points);
	int  nextCurve(XWPSPatchCurve * curve);
	int  nextFlag(int BitsPerFlag);
	void nextInit(XWPSShadingMeshParams * paramsA,	XWPSImagerState * pis);
	int  nextPackedDecoded(int num_bits,
		       							 const float decode[2], 
		       							 float *pvalue);
	int  nextPackedValue(int num_bits, uint * pvalue);
	int  nextPatch(int BitsPerFlag,
								 XWPSPatchCurve curve[4], 
								 XWPSFixedPoint interior[4]);
	int  nextVertex(XWPSMeshVertex * vertex);
	
public:
	XWPSStream ds;
	XWPSStream *s;
	uint bits;
	int left;
	XWPSShadingMeshParams *params;
  XWPSMatrixFixed *pctm;
  int (XWPSShadeCoordStream::*get_value)(int, uint *);
  int (XWPSShadeCoordStream::*get_decoded)(int,  const float [2], float *);
};

typedef int (XWPSShading::*shading_fill_rectangle_proc_t)(const XWPSRect*, 
	                                                        XWPSDevice *,
	                                                        XWPSImagerState *);

struct PSShadingProcs
{
	shading_fill_rectangle_proc_t fill_rectangle;
};

struct PSShadingHead
{
	PSShadingType type;
  PSShadingProcs procs;
};

class XWPSShading : public XWPSStruct
{
public:
	XWPSShading();
	XWPSShading(XWPSShadingFbParams * pFb);
	XWPSShading(XWPSShadingAParams * pA);
	XWPSShading(XWPSShadingRParams * pR);
	XWPSShading(XWPSShadingFfGtParams * pFfGt);
	XWPSShading(XWPSShadingLfGtParams * pLfGt);
	XWPSShading(XWPSShadingCpParams * pCp);
	XWPSShading(XWPSShadingTppParams * pTpp);
	~XWPSShading();
	
	int fillPath(XWPSPath *ppath,
		            const XWPSFixedRect *prect, 
		            XWPSDevice *orig_dev,
		            XWPSImagerState *pis, 
		            bool fill_background);
	int fillRectangle(const XWPSRect *prect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis);
	int fillRectangleA(const XWPSRect *prect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis);
	int fillRectangleCp(const XWPSRect *prect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis);
	int fillRectangleFb(const XWPSRect *prect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis);
	int fillRectangleFfGt(const XWPSRect *prect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis);
	int fillRectangleLfGt(const XWPSRect *prect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis);
	int fillRectangleR(const XWPSRect *prect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis);
	int fillRectangleTpp(const XWPSRect *prect, 
	                        XWPSDevice *dev,  
	                        XWPSImagerState *pis);
	                        
	PSShadingType getType() {return head.type;}
	virtual int getLength();
	virtual const char * getTypeName();
	                        
	int nextVertexGt(XWPSShadeCoordStream * cs, XWPSMeshVertex * vertex);
	
	int pathAddBox(XWPSPath *ppath, 
	               const XWPSRect *pbox, 
	               const XWPSMatrixFixed *pmat);
	
public:
	PSShadingHead head;
	
	XWPSShadingParams * params;
};

class XWPSShadingFillState
{
public:
	XWPSShadingFillState();
	virtual ~XWPSShadingFillState() {}
	
	int  fillPath(XWPSPath * ppath,	XWPSDeviceColor * pdevc);
	
	virtual void initFillState(XWPSShading * psh,
		                XWPSDevice * devA, 
		                XWPSImagerState * pisA);
	
	bool shadeColors2Converge(XWPSClientColor cc[2]);
	bool shadeColors4Converge(XWPSClientColor cc[4]);
	int  shadeFillDeviceRectangle(const XWPSFixedPoint * p0,
			    											const XWPSFixedPoint * p1,
			   								 				XWPSDeviceColor * pdevc);
	
public:
	XWPSDevice *dev;
  XWPSImagerState *pis;
  int num_components;
  float cc_max_error[PS_CLIENT_COLOR_MAX_COMPONENTS];
};

#define Fb_max_depth 32

class XWPSFbFrame
{
public:
	XWPSFbFrame() {}
	
public:
	XWPSRect region;
  XWPSClientColor cc[4];
};

class XWPSFbFillState : public XWPSShadingFillState
{
public:
	XWPSFbFillState();
	
	int fillRegion();
	
public:
	XWPSShading *psh;
	XWPSMatrixFixed ptm;
	bool orthogonal;
	int depth;
	XWPSFbFrame frames[Fb_max_depth];
};

#define AR_max_depth 16
#define A_max_depth AR_max_depth

class XWPSARFrame
{
public:
	XWPSARFrame();
	
public:
	double t0, t1;
  XWPSClientColor cc[2];
};

class XWPSAFillState : public XWPSShadingFillState
{
public:
	XWPSAFillState();
	
	int fillRegion();
	int fillStripe(XWPSClientColor *pcc, float t0, float t1);
	
public:
	XWPSShading *psh;
  bool orthogonal;
  XWPSRect rect;
  XWPSPoint delta;
  double length, dd;
  int depth;
  XWPSARFrame frames[A_max_depth];
};

#define R_max_depth AR_max_depth

class XWPSRFillState : public XWPSShadingFillState
{
public:
	XWPSRFillState();
	
	double computeRadius(float x, float y, const XWPSRect *rect);
	
	int fillAnnulus(XWPSClientColor *pcc, float t0, float t1, float r0, float r1);
	int fillRegion();
	
public:
	XWPSShading *psh;
  XWPSRect rect;
  XWPSPoint delta;
  double dr, width, dd;
  int depth;
  XWPSARFrame frames[R_max_depth];
};

#define mesh_max_depth (16 * 3 + 1)

class XWPSMeshFrame
{
public:
	XWPSMeshFrame();
	
public:
	XWPSMeshVertex va, vb, vc;
	bool check_clipping;
};

class XWPSMeshFillState : public XWPSShadingFillState
{
public:
	XWPSMeshFillState();
	
	int  fillTriangle();
	int  fillTriangleGt(const XWPSMeshVertex * va,
		                 const XWPSMeshVertex * vb, 
		                 const XWPSMeshVertex * vc);
	
	void initFillState(const XWPSShading * psh,
										 const XWPSRect * prect, 
		                XWPSDevice * devA, 
		                XWPSImagerState * pisA);
	
	void initFillTriangle(const XWPSMeshVertex *va, 
	                      const XWPSMeshVertex *vb, 
	                      const XWPSMeshVertex *vc,
                        bool check_clipping);
	
public:
	XWPSShading *pshm;
  XWPSFixedRect rect;
  int depth;
  XWPSMeshFrame frames[mesh_max_depth];
};

class XWPSPatchFillState : public XWPSMeshFillState
{
public:
	XWPSPatchFillState();
	
	int  fill(const XWPSPatchCurve curve[4],
	   				const XWPSFixedPoint interior[4],
	   				void (*transform)(XWPSFixedPoint *, 
	   				                   const XWPSPatchCurve[4],	
	   				                   const XWPSFixedPoint[4], float, 
	   				                   float));
	
	void interpolateColor(XWPSPatchColor * ppcr, 
	                      const XWPSPatchColor * ppc0,
                        const XWPSPatchColor * ppc1, 
                        float t);
                        
	void resolveColor(XWPSPatchColor * ppcr);
	
public:
	XWPSFunction *Function;
};

#endif //XWPSSHADING_H
