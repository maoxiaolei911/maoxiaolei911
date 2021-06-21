/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSREF_H
#define XWPSREF_H

#include "XWPSType.h"

#define PS_TYPE_ARRAY_SPAN  4
#define PS_TYPE_STRUCT_SPAN 2

#define PS_RTYPE_USES_ACCESS 1
#define PS_RTYPE_USES_SIZE 2
#define PS_RTYPE_IS_NULL 4
#define PS_RTYPE_IS_DICTIONARY 8

#define PS_L_MARK 1
#define PS_L_NEW 2
#define PS_R_SPACE_BITS 2
#define PS_R_SPACE_SHIFT 2
#define PS_A_WRITE 0x10
#define PS_A_READ 0x20
#define PS_A_EXECUTE 0x40
#define PS_A_EXECUTABLE 0x80
#define PS_A_READONLY (PS_A_READ+PS_A_EXECUTE)
#define PS_A_ALL (PS_A_WRITE+PS_A_READ+PS_A_EXECUTE)
#define PS_R_TYPE_SHIFT 8
#define PS_R_TYPE_BITS 6

#define PS_A_SPACE (((1 << PS_R_SPACE_BITS) - 1) << PS_R_SPACE_SHIFT)

#define PS_I_VM_FOREIGN  0
#define PS_I_VM_SYSTEM   1
#define PS_I_VM_GLOBAL   2
#define PS_I_VM_LOCAL    3
#define PS_I_VM_MAX      PS_I_VM_LOCAL

#define PS_AVM_FOREIGN  (PS_I_VM_FOREIGN << PS_R_SPACE_SHIFT)
#define PS_AVM_SYSTEM   (PS_I_VM_SYSTEM << PS_R_SPACE_SHIFT)
#define PS_AVM_GLOBAL   (PS_I_VM_GLOBAL << PS_R_SPACE_SHIFT)
#define PS_AVM_LOCAL    (PS_I_VM_LOCAL << PS_R_SPACE_SHIFT)
#define PS_AVM_MAX       PS_AVM_LOCAL

#define arch_sizeof_ref sizeof(XWPSRef)

#define PS_MAX_ARRAY_SIZE (max_ushort & (max_uint / (uint)arch_sizeof_ref))
#define PS_MAX_STRING_SIZE max_ushort

#define PS_R_PACKED_TYPE_SHIFT 13
#define PS_R_PACKED_VALUE_BITS 12

enum PSPackedType
{
	  pt_full_ref = 0,
#define pt_min_packed 2
    pt_executable_operator = 2,
    pt_integer = 3,
    pt_unused1 = 4,
    pt_unused2 = 5,
#define pt_min_name 6
    pt_literal_name = 6,
#define pt_min_exec_name 7
    pt_executable_name = 7
};

#define packed_per_ref (sizeof(XWPSRef) / sizeof(ushort))

#define arch_align_ref_mod\
 (((arch_align_long_mod - 1) | (arch_align_float_mod - 1) |\
   (arch_align_ptr_mod - 1)) + 1)
   
#define align_packed_per_ref\
  (arch_align_ref_mod / arch_align_short_mod)
  
#define pt_tag(pt) ((ushort)(pt) << PS_R_PACKED_TYPE_SHIFT)
#define packed_value_mask ((1 << PS_R_PACKED_VALUE_BITS) - 1)
#define packed_max_value packed_value_mask
#define r_is_packed(rp)  (*(const ushort *)(rp) >= pt_tag(pt_min_packed))
#define r_packed_is_name(prp) (*(prp) >= pt_tag(pt_min_name))
#define r_packed_is_exec_name(prp) (*(prp) >= pt_tag(pt_min_exec_name))
#define packed_name_max_index packed_max_value
#define packed_name_index(prp) (*(prp) & packed_value_mask)

#define packed_min_intval (-(1 << (PS_R_PACKED_VALUE_BITS - 1)))
#define packed_max_intval ((1 << (PS_R_PACKED_VALUE_BITS - 1)) - 1)
#define packed_int_mask packed_value_mask

#define lp_mark_shift 12
#define lp_mark (1 << lp_mark_shift)
#define r_has_pmark(rp) (*(rp) & lp_mark)
#define r_set_pmark(rp) (*(rp) |= lp_mark)
#define r_clear_pmark(rp) (*(rp) &= ~lp_mark)
#define r_store_pmark(rp,pm) (*(rp) = (*(rp) & ~lp_mark) | (pm))

#define packed_next(prp)\
  (r_is_packed(prp) ? prp + 1 : prp + packed_per_ref)
  	
#define packed_key_empty (pt_tag(pt_integer) + 0)
#define packed_key_deleted (pt_tag(pt_integer) + 1)
#define packed_key_impossible pt_tag(pt_full_ref)

#define packed_name_key(nidx)\
  ((nidx) <= packed_name_max_index ? pt_tag(pt_literal_name) + (nidx) :\
   packed_key_impossible)

#define num_int32 0		/* [0..31] */
#define num_int16 32		/* [32..47] */
#define num_float 48
#define num_float_IEEE num_float
#define num_float_native (num_float + 1)
#define num_msb 0
#define num_lsb 128
#define num_is_lsb(format) ((format) >= num_lsb)
#define num_is_valid(format) (((format) & 127) <= 49)

#define num_array 256

extern const uchar enc_num_bytes[];

#define encoded_number_bytes(format)\
  (enc_num_bytes[(format) >> 4])

class XWPSArray;
class XWPSArrayRef;
class XWPSPackedArray;
class XWPSPackedArrayRef;
class XWPSRef;
class XWPSName;
class XWPSDict;
class XWPSDictStack;
class XWPSRefStack;
class XWPSStream;
class XWPSContextState;
class XWPSDevice;
class XWPSBytes;
class XWPSBytesRef;

typedef int (XWPSContextState::*op_proc_t)();

#define MAX_PASSWORD 64	

struct PSPassword
{
	uint size;
  uchar data[MAX_PASSWORD];
};

class TypeAtrrsSize
{
public:
	TypeAtrrsSize() : type_attrs(0), rsize(0) {}
		
	void decSize(ushort n) {rsize-=n;}
	void incSize(ushort n) {rsize+=n;}
	
	void setSize(ushort n) {rsize=n;}
	ushort size() {return rsize;}
		
public:
	ushort type_attrs;
  ushort rsize;
};

#if PS_R_TYPE_SHIFT == 8
#  if arch_is_big_endian
#    define PS_R_TYPE (((const uchar *)&(tas.type_attrs))[sizeof(ushort)-2])
#  else
#    define PS_R_TYPE (((const uchar *)&(tas.type_attrs))[1])
#  endif
#  define PS_R_HAS_TYPE(typ) (PS_R_TYPE == (typ))
#else
#  define PS_R_TYPE (tas.type_attrs >> PS_R_TYPE_SHIFT)
#  define PS_R_HAS_TYPE(typ) PS_R_HAS_TYPE_ATTRS(typ,0)		/* see below */
#endif

#define PS_R_TYPE_XE_SHIFT (PS_R_TYPE_SHIFT - 2)
#define PS_TYPE_XE_(tas) ((tas) >> PS_R_TYPE_XE_SHIFT)
#define PS_TYPE_XE_VALUE(t,xe) PS_TYPE_XE_(((t) << PS_R_TYPE_SHIFT) + (xe))

extern uchar ref_type_properties[1 << 6];

#define PS_RTYPE_HAS(rtype,props)\
  ((ref_type_properties[rtype] & (props)) != 0)
#define ref_type_uses_access(rtype)\
 PS_RTYPE_HAS(rtype, PS_RTYPE_USES_ACCESS)
#define ref_type_uses_size(rtype)\
 PS_RTYPE_HAS(rtype, PS_RTYPE_USES_SIZE)
#define ref_type_uses_size_or_null(rtype)\
 PS_RTYPE_HAS(rtype, PS_RTYPE_USES_SIZE | PS_RTYPE_IS_NULL)

class XWPSRef
{
public:
	enum RefType
	{
		Invalid,
		Boolean,
		Dictionary,
		File,
		Array,
		MixedArray,
		ShortArray,
		UnusedArray,
		Struct,
		AStruct,
		FontID,
		Integer,
		Mark,
		Name,
		Null,
		Operator,
		Real,
		Save,
		String,
		Device,
		OpArray,
		NextIndex
	};
	
	XWPSRef();
	~XWPSRef();
		
	int  arrayGet(XWPSContextState * ctx, long index_long, XWPSRef * pref);
	void assign(XWPSRef * pfrom);
	void assignRef(XWPSRef * pfrom, uint s, uchar * p);
		
	RefType btype()
		{return (RefType)((tas.type_attrs >= (NextIndex << PS_R_TYPE_SHIFT)) ? Operator : PS_R_TYPE);}
	
	int callProc(XWPSContextState * ctx);
	int checkAccess(ushort acc1);
	int checkArray();
	int checkArrayOnly();
	int checkDictRead();
	int checkDictWrite();
	int checkExecute();
	int checkFile(XWPSStream ** svar);
	int checkIntLEU(ulong u);
	int checkIntLEUOnly(ulong u);
	int checkIntLTU(ulong u);
	int checkProc();
	int checkProcFailed();
	int checkProcOnly();
	int checkRead();
	int checkReadType(RefType typ);
	int checkReadTypeOnly(RefType typ);
	int checkType(const char * n);
	int checkType(RefType typ);
	int checkTypeAccess(RefType typ, ushort acc1);
	int checkTypeAccessOnly(RefType typ, ushort acc1);
	int checkTypeFailed();
	int checkTypeOnly(RefType typ);	
	int checkWrite();
	int checkWriteType(RefType typ);
	int checkWriteTypeOnly(RefType typ);
	
	void clear();
	void clearAttrs(ushort mask);
	void copyAttrs(ushort mask, XWPSRef * sp);
	int  cpyToOld(uint index, XWPSRef * from, uint size);
	
	void decSize(ushort dec);
	
	XWPSRef * dictAccessRef();
	int       dictAlloc(uint attr, uint size);
	int       dictBoolParam(XWPSContextState * ctx, 
	                         const char *kstr,
		                        bool defaultval, 
		                        bool * pvalue);
	int       dictCheckUidParam(XWPSContextState * ctx, XWPSUid * puid);
	int       dictCopy(XWPSContextState * ctx, 
	                          XWPSRef * pdrfrom,
	                          XWPSDictStack * pds);
	int       dictCopyEntries(XWPSContextState * ctx, 
	                          XWPSRef * pdrfrom, 
	                          bool new_only,
	                          XWPSDictStack * pds);
	int       dictCopyNew(XWPSContextState * ctx, 
	                          XWPSRef * pdrfrom, 
	                          XWPSDictStack * pds);
	int       dictCreateContents(uint size, bool pack);
	int       dictCreateUnpackedKeys(uint asize);
	int       dictFind(XWPSContextState * ctx, XWPSRef * pkey, XWPSRef ** ppvalue);
	int       dictFindPassword(XWPSContextState * ctx, const char *kstr, XWPSRef ** ppvalue);
	int       dictFindString(XWPSContextState * ctx, const char *kstr, XWPSRef ** ppvalue);
	int       dictFirst();
	int       dictFloatArrayCheckParam(XWPSContextState * ctx, 
	                                   const char *kstr,
			                               uint len, 
			                               float *fvec, 
			                               const float *defaultvec,
			                               int under_error, 
			                               int over_error);
	int       dictFloatArrayParam(XWPSContextState * ctx, 
	                              const char *kstr,
		                             uint maxlen, 
		                             float *fvec, 
		                             const float *defaultvec);
	int       dictFloatParam(XWPSContextState * ctx, 
	                         const char *kstr,
		                       float defaultval, 
		                       float *pvalue);
	int       dictFloatsParam(XWPSContextState * ctx, 
	                          const char *kstr,
		                        uint maxlen, 
		                        float *fvec, 
		                        const float *defaultvec);
	int       dictGrow(XWPSContextState * ctx, XWPSDictStack *pds);
	int       dictIntArrayCheckParam(XWPSContextState * ctx, 
	                                 const char *kstr, 
	                                 uint len,
			                             int *ivec, 
			                             int under_error, 
			                             int over_error);
	int       dictIntArrayParam(XWPSContextState * ctx, 
	                            const char *kstr,
		                          uint maxlen, 
		                          int *ivec);
	int       dictIntNullParam(XWPSContextState * ctx,
	                           const char *kstr, 
	                           int minval,
		                         int maxval, 
		                         int defaultval, 
		                         int *pvalue);
	int       dictIntParam(XWPSContextState * ctx, 
	                        const char *kstr, 
	                        int minval, 
	                        int maxval,
	                        int defaultval, 
	                        int *pvalue);
	int       dictIntsParam(XWPSContextState * ctx, 
	                        const char *kstr,	
	                        uint len, 
	                        int *ivec);
	bool      dictIsPacked();
	uint      dictLength();
	int       dictMatrixParam(XWPSContextState * ctx, 
	                          const char *kstr, 
	                          XWPSMatrix * pmat);
	uint      dictMaxIndex();
	uint      dictMaxLength();
	int       dictNext(XWPSContextState * ctx, int index, XWPSRef * eltp);
	int       dictProcParam(XWPSContextState * ctx, 
	                        const char *kstr, 
	                        XWPSRef * pproc,
		                      bool defaultval);
	int       dictPut(XWPSContextState * ctx, 
	                  XWPSRef * pkey, 
	                  XWPSRef * pvalue,
	                  XWPSDictStack * pds);
	int       dictPutString(XWPSContextState * ctx, 
	                        const char *kstr, 
	                        XWPSRef * pvalue,
		                      XWPSDictStack * pds);
	int       dictReadPassword(XWPSContextState * ctx, const char *pkey, PSPassword * ppass);
	int       dictResize(XWPSContextState * ctx, uint new_size, XWPSDictStack *pds);
	int       dictUidParam(XWPSUid * puid, 
	                       int defaultval,
	                       XWPSContextState *ctx);
	int       dictUintParam(XWPSContextState * ctx, 
	                          const char *kstr,
		                        uint minval, 
		                        uint maxval, 
		                        uint defaultval, 
		                        uint * pvalue);
	int       dictUndef(XWPSContextState * ctx, XWPSRef * pkey, XWPSDictStack *pds);
	int       dictUnpack(XWPSContextState * ctx, XWPSDictStack * pds);
	int       dictValueIndex(XWPSRef * pvalue);
	int       dictWritePassword(XWPSContextState * ctx, const PSPassword * ppass, const char *pkey);
	
	int  fileClose();
	bool fileIsInvalid(XWPSStream ** s);
	bool fileIsValid(XWPSStream ** s);
	int floatMatrix(int count, XWPSMatrix *pmat);
	int floatParam(float *pparam);
	
	XWPSRef  * getArray();
	bool       getBool() {return value.boolval;}
	uchar    * getBytes();
	XWPSDevice * getDevice() {return value.pdevice;}
	XWPSDict * getDict() {return value.pdict;}
	long       getInt() {return value.intval;}
	XWPSName * getName() {return value.pname;}
	op_proc_t  getOper() {return value.opproc;}
	ushort   * getPacked();
	float      getReal() {return value.realval;}
	XWPSStream * getStream() {return value.pfile;}
	XWPSStruct * getStruct() {return value.pstruct;}
	ushort   * getWritablePacked();
	
	const char * getTypeName();
		
	bool hasAttr(ushort mask1);
	bool hasAttrs(ushort mask);	
	bool hasMaskedAttrs(ushort attrs, ushort mask);
	bool hasMaskedTypeAttrs(ushort typ, ushort tspan, ushort mask);
	bool hasType(ushort typ);
	bool hasTypeAttrs(ushort typ, ushort mask);
	
	void incSize(ushort inc);
	int  intParam(int max_value, int *pparam);
	bool isArray();
	bool isEStackMark();
	bool isExOper();
	bool isForeign();
	bool isLocal();
	bool isMixedArray();
	bool isProc();
	bool isShortArray();
	bool isStruct();
	
	void makeArray(ushort attrs, ushort size, XWPSRef * elts);
	void makeArray(ushort attrs, ushort size);
	void makeArray(ushort attrs);
	void makeAStruct(ushort attrs, XWPSStruct * ptr);
	void makeAStruct(ushort attrs, ushort size);
	void makeBool(bool bval);
	void makeDevice(ushort attrs, XWPSDevice * ptr);
	void makeDict(ushort attrs, XWPSDict * dic);
	void makeFalse();
	void makeFile(ushort attrs, ushort id, XWPSStream * s);
	void makeFontID(ushort attrs, XWPSStruct * ptr);
	void makeInt(int ival);
	void makeInt(ushort attrs, uint ival);
	void makeMark();
	void makeMarkEStack(ushort opidx, op_proc_t proc);
	void makeName(ushort nidx, XWPSName * pnm);
	void makeNull();
	void makeOpArray(ushort attrs, ushort size, XWPSRef * elts);
	void makeOper(ushort opidx, op_proc_t proc);
	void makeOper(ushort opidx, ushort attrs, op_proc_t proc);
	void makeOper(ushort t, ushort opidx, ushort attrs, op_proc_t proc);
	void makeReal(float rval);
	void makeSave(ushort attrs, ulong sid);
	void makeShortArray(ushort attrs, uint ksize, uint asize);
	void makeStreamFile(XWPSStream * s, const char *access);
	void makeString(ushort attrs, ushort size, uchar * chars);
	void makeString(ushort attrs, ushort size);
	void makeString(ushort attrs);
	void makeStringSta(ushort attrs, ushort size, uchar * chars);
	void makeStruct(ushort attrs, XWPSStruct * ptr);
	void makeTrue();
	void makeType(RefType newtype);
	void makeTypeAttr(RefType newtype, ushort newattrs);
	
	int numArrayFormat();
	int numArrayGet(XWPSContextState * ctx,
	                  int format, 
	                  uint index, 
	                  XWPSRef * np);
	uint numArraySize(int format);
	
	bool objEq(XWPSContextState * ctx, XWPSRef * pref2);
	
	void * ptr() {return value.pstruct;}
	
	int readMatrix(XWPSContextState * ctx, XWPSMatrix * pmat);
	int realParam(double *pparam);
	
	void   setArrayPtr(XWPSRef * a);
	void   setAttrs(ushort mask);
	void   setBytesPtr(uchar * p);
	void   setPackedPtr(ushort * p);
	void   setSize(ushort siz);
	void   setSpace(ushort s);
	void   setType(ushort typ);
	void   setTypeAttrs(ushort typ, ushort mask);
	ushort size();
	ushort space();
	int    spaceIndex();
	void   storeAttrs(ushort mask, ushort attrs);
	int    storeCheckDest(XWPSRef * pnew);
	int    storeCheckSpace(ushort s);
	uint   subrBias();
	
	RefType type() {return (RefType)(PS_R_TYPE);}
	ushort typeAttrs();
	
	int writeMatrixIn(const XWPSMatrix * pmat);
	
public:
	TypeAtrrsSize tas;
	union v
	{
		long intval;
		ushort boolval;
		float realval;
		ulong saveid;
		XWPSBytesRef *bytes;
		XWPSArrayRef *refs;
		XWPSArrayRef *const_refs;
		XWPSName *pname;
		XWPSDict *pdict;
		XWPSDict *const_pdict;
		XWPSPackedArrayRef *packed;
		XWPSPackedArrayRef *writable_packed;
		op_proc_t opproc;
		XWPSStream *pfile;
		XWPSDevice *pdevice;
		XWPSStruct * pstruct;
	} value;
};

inline void XWPSRef::clearAttrs(ushort mask)
{tas.type_attrs &= ~(mask);}

inline void XWPSRef::copyAttrs(ushort mask, XWPSRef * sp)
{storeAttrs(mask, sp->tas.type_attrs & (mask));}
	
inline void XWPSRef::decSize(ushort dec) 
{tas.decSize(dec);}

inline bool XWPSRef::hasAttr(ushort mask1)
{return (typeAttrs() & (mask1));}

inline bool XWPSRef::hasAttrs(ushort mask)
{	return (!(~typeAttrs() & (mask)));}


inline bool XWPSRef::hasMaskedAttrs(ushort attrs, ushort mask)
{	return ((typeAttrs() & (mask)) == (attrs));}

inline bool XWPSRef::hasMaskedTypeAttrs(ushort typ, ushort tspan, ushort mask)
{return (((tas.type_attrs & ((((1 << PS_R_TYPE_BITS) - (tspan)) << PS_R_TYPE_SHIFT) + (mask))) == (((typ) << PS_R_TYPE_SHIFT) + (mask))));}

inline bool XWPSRef::hasType(ushort typ) 
{return PS_R_HAS_TYPE(typ);}

inline bool XWPSRef::hasTypeAttrs(ushort typ, ushort mask)
{return (hasMaskedTypeAttrs(typ,1,mask));}

inline bool XWPSRef::isArray()
{	return hasMaskedTypeAttrs(Array,PS_TYPE_ARRAY_SPAN,0);}

inline bool XWPSRef::isEStackMark()
{return hasTypeAttrs(Null, PS_A_EXECUTABLE);}

inline bool XWPSRef::isMixedArray()
{return hasMaskedTypeAttrs(MixedArray,PS_TYPE_ARRAY_SPAN,0);}

inline bool XWPSRef::isExOper()
{return (hasAttr(PS_A_EXECUTABLE) && (btype() == Operator || type() == OpArray));}

inline void XWPSRef::incSize(ushort inc) 
{tas.incSize(inc);}

inline bool XWPSRef::isProc()
{return (hasMaskedTypeAttrs(Array,PS_TYPE_ARRAY_SPAN, PS_A_EXECUTE + PS_A_EXECUTABLE));}

inline bool XWPSRef::isShortArray()
{return hasMaskedTypeAttrs(ShortArray,PS_TYPE_ARRAY_SPAN,0);}

inline bool XWPSRef::isStruct()
{return hasMaskedTypeAttrs(Struct,PS_TYPE_STRUCT_SPAN,0);}

inline void XWPSRef::makeBool(bool bval)
{clear();setTypeAttrs((ushort)Boolean, 0); value.boolval = bval;}

inline void XWPSRef::makeFalse()
{clear();makeBool(false);}

inline void XWPSRef::makeInt(int ival)
{clear();setTypeAttrs((ushort)Integer, 0); value.intval = ival;}

inline void XWPSRef::makeMark()
{clear(); makeType(Mark);}

inline void XWPSRef::makeNull()
{clear(); makeType(Null);}

inline void XWPSRef::makeReal(float rval)
{clear(); setTypeAttrs((ushort)Real, 0); value.realval = rval;}

inline void XWPSRef::makeTrue()
{clear(); makeBool(true);}

inline void XWPSRef::makeType(XWPSRef::RefType newtype)
{clear(); makeTypeAttr(newtype, 0);}

inline void XWPSRef::makeTypeAttr(XWPSRef::RefType newtype, ushort newattrs)
{setTypeAttrs((ushort)newtype, newattrs);}

inline void XWPSRef::setAttrs(ushort mask)
{tas.type_attrs |= mask;}

inline void XWPSRef::setSize(ushort siz) 
{tas.setSize(siz);}

inline void XWPSRef::setType(ushort typ) 
{tas.type_attrs = (typ << PS_R_TYPE_SHIFT);}

inline void XWPSRef::setTypeAttrs(ushort typ, ushort mask)
{(tas.type_attrs = ((typ) << PS_R_TYPE_SHIFT) + (mask));}

inline ushort XWPSRef::size() 
{return tas.size();}

inline void XWPSRef::storeAttrs(ushort mask, ushort attrs)
{(tas.type_attrs = (tas.type_attrs & ~(mask)) | (attrs));}

inline ushort XWPSRef::typeAttrs() 
{return tas.type_attrs;}

class XWPSMemoryStatus
{
public:
	XWPSMemoryStatus() : allocated(0), used(0) {}
	XWPSMemoryStatus(ulong a) : allocated(a), used(0) {}
	XWPSMemoryStatus(ulong a, ulong u) : allocated(a), used(u) {}
	
public:
	ulong allocated;
	ulong used;
};

class XWPSMemoryGCStatus
{
public:
	XWPSMemoryGCStatus() 
	: vm_threshold(0),
		max_vm(0),
		psignal(0),
		signal_value(0),
		enabled(false),
		requested(0) {}
	
public:
	long vm_threshold;
	long max_vm;
	int *psignal;
	int signal_value;
	bool enabled;	
	long requested;
};

class XWPSRefMemory
{
public:
	XWPSRefMemory();
	XWPSRefMemory(uint chk_size);
	~XWPSRefMemory();
	
	void setLimit();
	
public:
	XWPSRefMemory * stable_memory;
	uint chunk_size;
	uint large_size;
	uint space;
	XWPSMemoryGCStatus gc_status;
	bool is_controlled;
	ulong limit;
	ulong allocated;
	long inherited;
	ulong gc_allocated;
	struct lost_
	{
		ulong objects;
		ulong refs;
		ulong strings;
	} lost;
	
	int save_level;
	uint new_mask;
	uint test_mask;
	int num_contexts;
	long total_scanned;
	XWPSMemoryStatus previous_status;
	uint largest_free_size;
};

class XWPSVMSpace
{
public:
	XWPSVMSpace();
	
public:
	union 
	{
		XWPSRefMemory *indexed[4];
		struct _ssn 
		{
	    XWPSRefMemory *foreign;
	    XWPSRefMemory *system;
	    XWPSRefMemory *global;
	    XWPSRefMemory *local;
		} named;
  } memories;
};

class XWPSDualMemory
{
public:
	XWPSDualMemory(uint chunk_size, bool level2);
	~XWPSDualMemory();
	
	uint iallocSpace() {return current_space;}
	int  ireclaim(int space);
	
	void resetRequested();
	
	void setInSave();
	void setMasks(uint n, uint t);
	void setNotInSave();
	void setSpace(uint space);
	
public:
	XWPSRefMemory *current;
	XWPSVMSpace spaces;
	uint current_space;
	int (XWPSDualMemory::*reclaim)(int);
	uint test_mask;
	uint new_mask;
};

#define tx_op (XWPSRef::NextIndex)
typedef enum {
    tx_op_add = tx_op,
    tx_op_def,
    tx_op_dup,
    tx_op_exch,
    tx_op_if,
    tx_op_ifelse,
    tx_op_index,
    tx_op_pop,
    tx_op_roll,
    tx_op_sub,
    tx_next_op
} special_op_types;

#define num_special_ops ((int)tx_next_op - tx_op)

uint sdecodeushort(const uchar * p, int format);
int sdecodeshort(const uchar * p, int format);
long sdecodelong(const uchar * p, int format);
float sdecodefloat(const uchar * p, int format);
int sdecode_number(const uchar * str, int format, XWPSRef * np);

class XWPSIGStateObj : public XWPSStruct
{
public:
	XWPSIGStateObj();
	
	void copy(XWPSIGStateObj * other);
	
	int getLength();
	const char * getTypeName();
	
public:
	XWPSRef gstate;
};

class XWPSRefDeviceNParams
{
public:
	XWPSRefDeviceNParams() {}
	
	void copy(XWPSRefDeviceNParams * other);
	
public:
	XWPSRef layer_names, tint_transform;
};

class XWPSRefCieProcs
{
public:
	XWPSRefCieProcs() {}
	
	void copy(XWPSRefCieProcs * other);
	
public:
	XWPSRef DEFG;
	XWPSRef ABC;
	XWPSRef DecodeLMN;
};

class XWPSRefCieRenderProcs
{
public:
	XWPSRefCieRenderProcs() {}
	
	void copy(XWPSRefCieRenderProcs * other);
	
public:
	XWPSRef TransformPQR, EncodeLMN, EncodeABC, RenderTableT;
};

class XWPSRefSeparationParams
{
public:
	XWPSRefSeparationParams() {}
	
	void copy(XWPSRefSeparationParams * other);
	
public:
	XWPSRef layer_name, tint_transform;
};

class XWPSRefColorProcs
{
public:
	XWPSRefColorProcs() {}
	
	void copy(XWPSRefColorProcs * other);
	
public:
	XWPSRefCieProcs cie;
	XWPSRefDeviceNParams  device_n;
	XWPSRefSeparationParams  separation;
	XWPSRef index_proc;
};

class XWPSRefColorSpace
{
public:
	XWPSRefColorSpace() {}
	
	void copy(XWPSRefColorSpace * other);
	
public:
	XWPSRef array;
	XWPSRefColorProcs procs;
};

class XWPSRefRgb
{
public:
	XWPSRefRgb() {}
	
	void copy(XWPSRefRgb * other);
	
public:
	XWPSRef red, green, blue, gray;
};

class XWPSRefColor
{
public:
	XWPSRefColor() {}
	
	void copy(XWPSRefColor * other);
	
public:
	XWPSRef indexed[4];
	XWPSRefRgb colored;
};

class XWPSRefColorRendering
{
public:
	XWPSRefColorRendering() {}
	
	void copy(XWPSRefColorRendering * other);
	
public:
	XWPSRef dict;
	XWPSRefCieRenderProcs procs;
};

class XWPSIntGState
{
public:
	XWPSIntGState() {}
	
	void copy(XWPSIntGState * other);
	
public:
	XWPSRef dash_pattern;	
	XWPSRefColor screen_procs, transfer_procs;
	XWPSRef black_generation;
	XWPSRef undercolor_removal;
	XWPSRefColorSpace colorspace;
	XWPSRef pattern;
	XWPSRefColorRendering colorrendering;
	XWPSRef halftone;	
	XWPSRef pagedevice;	
	XWPSRef remap_color_info;
	XWPSRef opacity_mask, shape_mask;
};

class XWPSIntRemapColorInfo : public XWPSStruct
{
public:
	XWPSIntRemapColorInfo();
	
	int getLength();
	const char * getTypeName();
	
public:
	op_proc_t proc;
	float tint[PS_CLIENT_COLOR_MAX_COMPONENTS];
};

class XWPSIntPattern : public XWPSStruct
{
public:
	XWPSIntPattern(XWPSRef * op);
	
	int getLength();
	const char * getTypeName();
	
public:
	XWPSRef dict;
};

class XWPSImageParams : public XWPSStruct
{
public:
	XWPSImageParams();
	
	int getLength();
	const char * getTypeName();
	
public:
	bool MultipleDataSources;
  XWPSRef DataSource[7];
  const float *pDecode;
};

enum PSBuildFontOptions
{
    bf_options_none = 0,
    bf_Encoding_optional = 1,
    bf_FontBBox_required = 2,
    bf_UniqueID_ignored = 4,
    bf_CharStrings_optional = 8,
    bf_notdef_required = 16	
} ;

class XWPSBuildProcRefs
{
public:
	XWPSBuildProcRefs() {}
	
public:
	XWPSRef BuildChar;
  XWPSRef BuildGlyph;
};

class XWPSFontData : public XWPSStruct
{
public:
	XWPSFontData();
	~XWPSFontData();
	
	void copy(XWPSFontData * other);
	
	int getLength();
	const char * getTypeName();
	
public:
	XWPSRef dict;	
  XWPSRef BuildChar;
  XWPSRef BuildGlyph;
  XWPSRef Encoding;
  XWPSRef CharStrings;
  union _fs 
  {
		struct _f1 
		{
	    XWPSRef * OtherSubrs;
	    XWPSRef * Subrs;	
	    XWPSRef * GlobalSubrs;
		} type1;
		struct _f42 
		{
	    XWPSRef * sfnts;
	    XWPSRef * CIDMap;	
	    XWPSRef * GlyphDirectory;
		} type42;
		struct _fc0 
		{
	    XWPSRef * GlyphDirectory;
	    XWPSRef * GlyphData;
	    XWPSRef * DataSource;	
		} cid0;
  } u;
};

class XWPSCharstringFontRefs : public XWPSStruct
{
public:
	XWPSCharstringFontRefs();
	
	void copy(XWPSCharstringFontRefs * other);
	
	int getLength();
	const char * getTypeName();
	
public:
	XWPSRef *Private;
  XWPSRef no_subrs;
  XWPSRef *OtherSubrs;
  XWPSRef *Subrs;
  XWPSRef *GlobalSubrs;
};

class XWPSArray : public XWPSStruct
{
public:
	XWPSArray(uint siz);
	~XWPSArray();

	int getLength();
	const char * getTypeName();
	
public:
	XWPSRef * arr;
	XWPSRef * ptr;
};

class XWPSArrayRef : public XWPSStruct
{
public:
	XWPSArrayRef();
	XWPSArrayRef(XWPSRef * elts);
	XWPSArrayRef(uint siz);
	~XWPSArrayRef();
		
public:
	XWPSArray * arr;
	XWPSRef * ptr;
};

class XWPSPackedArray : public XWPSStruct
{
public:
	XWPSPackedArray(uint size, uint countA);
	~XWPSPackedArray();
	
	int getLength();
	const char * getTypeName();
	
public:
	ushort * arr;
	ushort * ptr;
	int count;
};

class XWPSPackedArrayRef : public XWPSStruct
{
public:
	XWPSPackedArrayRef();
	XWPSPackedArrayRef(ushort * elt);
	XWPSPackedArrayRef(uint size, uint countA);
	~XWPSPackedArrayRef();
	
public:
	XWPSPackedArray * arr;
	ushort * ptr;
};

int memcpyRef(XWPSRef * to, XWPSRef * from, int num);
int memmoveRef(XWPSRef * to, XWPSRef * from, int num);

#endif //XWPSREF_H
