/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSPATH_H
#define XWPSPATH_H

#include "XWPSType.h"

#define USE_FILL_ADJUSTMENT

#ifdef USE_FILL_ADJUSTMENT
#  define STROKE_ADJUSTMENT(thin, pis, xy)\
     (thin ? fixed_0 : (pis)->fill_adjust.xy)
#else
#  define STROKE_ADJUSTMENT(thin, pis, xy) fixed_0
#endif

#define PS_RULE_WINDING_NUMBER (-1)
#define PS_RULE_EVEN_ODD 1

#define PS_QUARTER_ARC_FRACTION 0.55228474983079334

#define degrees_to_radians (M_PI / 180.0)

#define curve_points_to_coefficients(v0, v1, v2, v3, a, b, c, t01, t12)\
  (/*d = (v0),*/\
   t01 = (v1) - (v0), c = (t01 << 1) + t01,\
   t12 = (v2) - (v1), b = (t12 << 1) + t12 - c,\
   a = (v3) - b - c - (v0))
   
#define curve_coefficients_to_points(a, b, c, d, v1, v2, v3)\
  (/*v0 = (d),*/\
   v1 = (d) + ((c) / 3),\
   v2 = v1 + (((b) + (c)) / 3),\
   v3 = (a) + (b) + (c) + (d))
   
#define dir_up 1
#define dir_horizontal 0
#define dir_down (-1)

#if ARCH_DIV_NEG_POS_TRUNCATES
#  define SET_NUM_ADJUST(alp) \
    (alp)->num_adjust =\
      ((alp)->diff.x >= 0 ? 0 : -(alp)->diff.y + fixed_epsilon)
#  define ADD_NUM_ADJUST(num, alp) ((num) + (alp)->num_adjust)
#  define MAX_MINUS_NUM_ADJUST(alp) ADD_NUM_ADJUST(max_fixed, alp)
#else
#  define SET_NUM_ADJUST(alp) 
#  define ADD_NUM_ADJUST(num, alp) (num)
#  define MAX_MINUS_NUM_ADJUST(alp) max_fixed
#endif

int ps_curve_monotonic_points(long v0, long v1, long v2, long v3, double pst[2]);

class XWPSPath;
class XWPSSubpath;
class XWPSSegment;
class XWPSImagerState;
class XWPSState;

class XWPSFillParams
{
public:
	XWPSFillParams();
	
public:
	int rule;
	XWPSFixedPoint adjust;
	float flatness;
	bool fill_zero_width;
};

class XWPSStrokeParams
{
public:
	XWPSStrokeParams() :flatness(0.0) {}
		
public:
	float flatness;
};

class XWPSArcCurveParams
{
public:
	XWPSArcCurveParams();
	~XWPSArcCurveParams();
	
	int arcAdd(bool is_quadrant);
	
	int nextArcCurve(long anext);
	int nextArcQuadrant(long anext);
	
public:
	XWPSPath *ppath;
  XWPSImagerState *pis;
  XWPSPoint center;
  double radius;
  PSArcAction action;
  PSSegmentNotes notes;
  XWPSPoint p0, p3, pt;
  XWPSSinCos sincos;
  long angle;
  int fast_quadrant;
  long scaled_radius;
  long quadrant_delta;
};

class XWPSSegment
{
public:	
	XWPSSegment();
	XWPSSegment(ushort t);
	XWPSSegment(ushort t, ushort n);
	virtual ~XWPSSegment();
	
	static void addHintDiff(XWPSFixedPoint * ppt, XWPSFixedPoint delta)
	 {ppt->x += delta.x; ppt->y += delta.y;}
	void adjustPointToTangent(const XWPSSegment * nextA,	const XWPSFixedPoint * p1A);
	static void applyFinalHint(XWPSSegment * pseg_last, const XWPSFixedPoint * pdiff);
	
	static bool lineIsNull(XWPSFixedPoint p0, XWPSFixedPoint p1)
	 {return (qAbs(p1.x - p0.x) + qAbs(p1.y - p0.y) < fixed_epsilon * 4);}
	
	long scaleDelta(long diff, long dv, long lv, bool nearer);
			
public:
	XWPSSegment * prev;
	XWPSSegment * next;
	ushort type;
	ushort notes;
	XWPSFixedPoint pt;
};

class XWPSLineSegment : public XWPSSegment
{
public:
	XWPSLineSegment();
	XWPSLineSegment(ushort n);
};

class XWPSLineCloseSegment : public XWPSSegment
{
public:
	XWPSLineCloseSegment();
	XWPSLineCloseSegment(ushort n);
	~XWPSLineCloseSegment();
	
public:
	XWPSSubpath * sub;
};

class XWPSCurveSegment : public XWPSSegment
{
public:
	XWPSCurveSegment();
	XWPSCurveSegment(ushort n);
	
	void adjustCurveEnd(const XWPSFixedPoint * pdiff);
	void adjustCurveStart(const XWPSFixedPoint * pdiff);
	
	int log2Samples(long x0, long y0, long fixed_flat);
	static int monotonicPoints(long v0, 
	                           long v1, 
	                           long v2, 
	                           long v3, 
	                           double * pst);
	                           
	void split(long x0, 
	           long y0, 
	           double t,
	           XWPSCurveSegment * pc1, 
	           XWPSCurveSegment * pc2);
	void splitCurveMidpoint(long x0, 
	                        long y0, 
	                        XWPSCurveSegment * pc1, 
	                        XWPSCurveSegment * pc2);
	
public:
	XWPSFixedPoint p1, p2;
};

class XWPSSubpath : public XWPSSegment
{
public:	
	XWPSSubpath();	
	XWPSSubpath(ushort n);
	~XWPSSubpath();
	
	PSPathRectangularType  isRectangular(XWPSFixedRect * pbox, 
	                                     const XWPSSubpath ** ppnext);
		
public:
	XWPSSegment * last;
	int curve_count;
	bool is_closed;
	XWPSLineCloseSegment closer;
};

class XWPSCurveCursor
{
public:
	XWPSCurveCursor();
	
	long curveXAtY(long y);
	
	void init(long x0, long y0, const XWPSCurveSegment * pcA, int kA);
	
public:
	int k;
	XWPSCurveSegment * pc;
	long a, b, c;
	double da, db, dc;
	bool double_set;
	int fixed_limit;
	XWPSFixedPoint p0;
	struct ccc_
	{
		ulong ky0, ky3;
		ulong xl, xd;
	}cache;
};

class XWPSActiveLine
{
public:
	XWPSActiveLine();
	
	bool end_x_line();
	
	void resortXLine();
	
	void setScanLinePoints(long fixed_flat);
	
	static int xorder(const XWPSActiveLine *lp1, const XWPSActiveLine *lp2);
	
public:
	XWPSFixedPoint start;
	XWPSFixedPoint end;
	XWPSFixedPoint diff;
	long y_fast_max;
	long num_adjust;
	long x_current;
	long x_next;
	XWPSSegment *pseg;
	int direction;
	int curve_k;
	XWPSCurveCursor cursor;
	XWPSActiveLine *prev, *next;
	XWPSActiveLine *alloc_next;
};

#define max_local_active 20

class XWPSLineList
{
public:
	XWPSLineList();
	~XWPSLineList();
	
	int add_y_line(const XWPSSegment * prev_lp, const XWPSSegment * lp, int dir);
	int add_y_list(XWPSPath * ppath, 
	               long adjust_below, 
	               long adjust_above,
	               const XWPSFixedRect * pbox);
	
	void insertXNew(XWPSActiveLine * alp);
	
public:
	XWPSActiveLine *active_area;
	XWPSActiveLine *next_active;
	XWPSActiveLine *limit;
	int close_count;
	XWPSActiveLine *y_list;
	XWPSActiveLine *y_line;
	XWPSActiveLine x_head;
	XWPSActiveLine local_active[max_local_active];
};

class XWPSEndPoint
{
public:
	XWPSEndPoint() {}
	
	int add_round_cap(XWPSPath * ppath);
	
	int cap_points(PSLineCap type, XWPSFixedPoint *pts);
	
public:
	XWPSFixedPoint p;
  XWPSFixedPoint co, ce;
  XWPSFixedPoint cdelta;
};

class XWPSPartialLine
{
public:
	XWPSPartialLine();
	
	void adjust_stroke(const XWPSImagerState * pis, bool thin);
	
	void compute_caps();
	
	int line_join_points(const XWPSLineParams * pgs_lp, 
	                     XWPSPartialLine * plp,
		                   XWPSFixedPoint * join_points, 
		                   const XWPSMatrix * pmat,
		 									 PSLineJoin join);
	int line_intersect(XWPSFixedPoint * pp1,
		  							 XWPSFixedPoint * pd1,
		  							 XWPSFixedPoint * pp2,
		  							 XWPSFixedPoint * pd2,
		  							 XWPSFixedPoint * pi);
		 									 
	void set_thin_widths();
	
	bool width_is_thin();
	
public:
	XWPSEndPoint o;
  XWPSEndPoint e;
  XWPSFixedPoint width;
  bool thin;
};

class XWPSTrapLine
{
public:
	XWPSTrapLine();
	
	void compute_dx(long xd, long ys);
	void compute_ldx(long ys);
	
public:
	long h;
	int di;
  long df;
  long x, xf;
  long ldi, ldf;
};

class XWPSPathSegments : public XWPSStruct
{
public:
	XWPSPathSegments();
	~XWPSPathSegments();
	
	void clear();
	
	int getLength();
	const char * getTypeName();
		
public:
	struct psc_ 
	{
		XWPSSubpath *subpath_first;
		XWPSSubpath *subpath_current;
  } contents;
};

class XWPSPath
{
public:	
	XWPSPath();
	XWPSPath(const XWPSPath * shared);
	~XWPSPath();
	
	int addCharPath(XWPSPath * from_path, PSCharPathMode mode);
	int addCurveNotes(long x1, 
	                  long y1, 
	                  long x2, 
	                  long y2, 
	                  long x3, 
	                  long y3,
			              PSSegmentNotes notesA);
	int addDashExpansion(XWPSPath * ppath_old, XWPSImagerState * pis);
	int addFlattenedAccurate(XWPSPath * old, float flatness, bool accurate);
	int addLineNotes(long x, long y, PSSegmentNotes notesA);
	int addLinesNotes(const XWPSFixedPoint *ppts, 
	                  int count,
			              PSSegmentNotes notesA);
	int addMonotonized(XWPSPath * old);
	int addPartialArcNotes(long x3, 
	                       long y3, 
	                       long xt, 
	                       long yt, 
	                       float fraction, 
	                       PSSegmentNotes notesA);
	int addPath(XWPSPath * ppfrom);
	int addPoint(long x, long y);
	int addPoints(const XWPSFixedPoint * points, int npoints, bool moveto_first);
	int addRectangle(long x0, long y0, long x1, long y1);
	int addRelativePoint(long dx, long dy);
	int addRelPoint(long dx, long dy);
	int assign(XWPSPath * shared, bool preserve = true);
	
	int checkInBbox(long px, long py);
	int closeSubpathNotes(PSSegmentNotes notesA);
	int copy(XWPSPath *ppath_old);
	int copyReducing(XWPSPath *ppath_old, 
		      				 long fixed_flatness, 
		      				 XWPSImagerState *pis,
		               PSPathCopyOptions options);
	int copyReversed(XWPSPath * ppath_old);
	int currentPoint(XWPSFixedPoint * ppt);
	XWPSSubpath * currentSubpath() {return segments->contents.subpath_current;}
	
	XWPSSubpath * firstSubpath() {return segments->contents.subpath_first;}
	int flattenSample(int k, XWPSCurveSegment * pc, PSSegmentNotes notesA);
	
	int getBbox(XWPSFixedRect * pbox);
	
	bool hasCurves() {return (curve_count != 0);}
	
	int  imagerArcAdd(XWPSImagerState * pis, 
	                  bool clockwise,
	                  float axc, 
	                  float ayc, 
	                  float arad, 
	                  float aang1, 
	                  float aang2,
		                bool add_line);
	bool isDrawing() {return ((state_flags & psf_is_drawing) != 0);}
	bool isMonotonic();
	bool isNull();
	bool isRectangle(XWPSFixedRect * pbox);
	PSPathRectangularType isRectangular(XWPSFixedRect * pbox);
	bool isShared() {return segments->refCount > 1;}
	bool isVoid() {return segments->contents.subpath_first == 0;}
	
	bool lastIsMoveTo() {return ((state_flags & ~psf_outside_range) == psf_last_moveto);}
	
	int monotonizeInternal(XWPSCurveSegment * pc);
	int newPath();
	int newSubpath();
	
	bool outsideBbox(long px, long py);
	bool outsideRange() {return ((state_flags & psf_outside_range) != 0);}
	
	int  popCloseNotes(PSSegmentNotes notesA);
	bool positionInRange();
	bool positionValid();
	
	int  scaleExp2Shared(int log2_scale_x, int log2_scale_y,  bool segments_shared);
	void setOutsidePosition() 
		{
			outside_position.x = outside_start.x; 
			outside_position.y = outside_start.y;
			state_flags |= psf_outside_range;
		}
	void setOutsidePosition(double px, double py) 
		{
			outside_position.x = px;
			outside_position.y = py;
			state_flags |= psf_outside_range;
		}
	bool startOutsideRange() {return (state_flags != 0 && ((start_flags & psf_outside_range) != 0));}
	int  subpathCount() {return subpath_count;}
	bool subpathOpen() {return ((state_flags & psf_subpath_open) != 0);}
	
	int translate(long dx, long dy);
	
	void unclose(int count);
	int  unshare();
	void updateClosePath() {state_flags = psf_last_closepath;}
	void updateDraw() {state_flags = psf_last_draw;}
	void updateMoveTo();
	
public:
	XWPSPathSegments local_segments;
	XWPSPathSegments *segments;
  XWPSFixedRect bbox;
  XWPSSegment *box_last;
  uchar start_flags;
  uchar state_flags;
  uchar bbox_set;
  uchar _pad;
  int subpath_count;
  int curve_count;
  XWPSFixedPoint position;
  XWPSPoint outside_position;
  XWPSPoint outside_start;
  
private:
	int  allocCopy();
	void clear();
	void copyContents(const XWPSPath * shared);
	void initContents();
	
	int subpathExpandDashes(XWPSSubpath * psub, 
	                        XWPSImagerState * pis, 
	                        XWPSDashParams * dash);
};

#define ps_pe_moveto 1
#define ps_pe_lineto 2
#define ps_pe_curveto 3
#define ps_pe_closepath 4

class XWPSPathEnum : public XWPSStruct
{
public:
	XWPSPathEnum();
	~XWPSPathEnum();
	
	bool backup();
	
	int copyInit(XWPSState * pgs, bool copy);
	
	int getLength();
	const char * getTypeName();
	PSSegmentNotes getNotes() {return notes;}
		
	int init(XWPSPath * ppath);
	
	int next(XWPSFixedPoint * ppts);
	int next(XWPSPoint ppts[3]);
	
public:
	XWPSMatrix mat;
	XWPSSegment * pseg;
	XWPSPath * path;
	XWPSPath * copied_path;
	bool moveto_done;
	PSSegmentNotes notes;
};

class XWPSClipRect
{
public:
	XWPSClipRect();
	~XWPSClipRect();
	
	void init();
	
public:
	XWPSClipRect *next, *prev;
	int ymin, ymax;
	int xmin, xmax;
	uchar to_visit;
};

class XWPSClipList
{
public:
	XWPSClipList();
	~XWPSClipList();
	
	void free();
	void fromRectangle(XWPSFixedRect * rp);
	
	void init();
	bool isRectangle() {return count <= 1;}
	
public:
	XWPSClipRect single;
  XWPSClipRect *head;
  XWPSClipRect *tail;
  int xmin, xmax;
  int count;
};

class XWPSClipRectList : public XWPSStruct
{
public:
	XWPSClipRectList();
	~XWPSClipRectList();
	
	void fromRectangle(XWPSFixedRect * rp);
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	bool isRectangle() {return list->isRectangle();}
	
	XWPSClipList * getList() {return list;}
	
public:
	XWPSClipList * list;
};

class XWPSClipPath
{
public:
	XWPSClipPath();
	XWPSClipPath(XWPSClipPath * shared);
	~XWPSClipPath();
	
	int assign(XWPSClipPath * shared, bool preserve = true);
	
	int fromRectangle(XWPSFixedRect * pbox);
	
	XWPSClipList * getList() {return rect_list->getList();}
	
	bool innerBox(XWPSFixedRect * pbox);
	bool includesRectangle(ulong x0, ulong y0, ulong x1, ulong y1);
	void initOwnContents();
	int  intersect(XWPSPath *ppath_orig,  int rule, XWPSImagerState *pis);
	int  intersectPathSlow(XWPSPath * ppath,
			     							 int rule, 
			     							 XWPSImagerState *pis);
	
	XWPSClipList * listPrivate() {return rect_list->list;}
	
	bool outerBox(XWPSFixedRect * pbox);
	
	int reset();
	
	int  scaleExp2Shared(int log2_scale_x, int log2_scale_y, bool list_shared, bool segments_shared);
	void setOuterBox();
	
	int toPath(XWPSPath * ppath);
	
	int unshare();
	
public:
	XWPSPath path;
  XWPSClipRectList local_list;
  int rule;
  XWPSFixedRect inner_box;
  XWPSFixedRect outer_box;
  XWPSClipRectList *rect_list;
  bool path_valid;
  ulong id;
  
private:
	void clear();
	void initContents();
	void initRectangle(XWPSFixedRect * pbox);
	
	void shareContents(const XWPSClipPath * shared);
	int setRectangle(XWPSFixedRect * pbox);
};

class XWPSClipStack : public XWPSStruct
{
public:
	XWPSClipStack();
	~XWPSClipStack();
	
	int getLength();
	const char * getTypeName();
	
public:
	XWPSClipPath *clip_path;
  XWPSClipStack *next;
};

class XWPSCPathEnum
{
public:
	XWPSCPathEnum();
	~XWPSCPathEnum();
	
	int init(XWPSClipPath * pcpath);
	
	int next(XWPSFixedPoint * pts);
	
public:
	XWPSPathEnum path_enum;
	bool using_path;
	XWPSClipRect *visit;
	XWPSClipRect *rp;
	PSCPEVisit first_visit;
  PSCPEState state;
  bool have_line;
  XWPSIntPoint line_end;
  bool any_rectangles;
};

#endif //XWPSPATH_H
