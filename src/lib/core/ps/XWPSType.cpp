/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include <string.h>
#include <QByteArray>
#include "psscantable.h"
#include "psbittable.h"
#include "XWPSError.h"
#include "XWPSColorSpace.h"
#include "XWPSType.h"

#define ROP_PROC(pname, expr)\
static rop_operand pname(rop_operand D, rop_operand S, rop_operand T)\
{ return expr; }


#define a(u,v) (u&v)
#define o(u,v) (u|v)
#define x(u,v) (u^v)

ROP_PROC(rop0, 0)		/* 0 */
ROP_PROC(rop1, ~(D | S | T))		/* DTSoon */
ROP_PROC(rop2, D & ~(S | T))		/* DTSona */
ROP_PROC(rop3, ~(S | T))	/* TSon */
ROP_PROC(rop4, S & ~(D | T))		/* SDTona */
ROP_PROC(rop5, ~(D | T))	/* DTon */
ROP_PROC(rop6, ~(T | ~(D ^ S)))		/* TDSxnon */
ROP_PROC(rop7, ~(T | (D & S)))		/* TDSaon */
ROP_PROC(rop8, S & (D & ~T))		/* SDTnaa */
ROP_PROC(rop9, ~(T | (D ^ S)))		/* TDSxon */
ROP_PROC(rop10, D & ~T)		/* DTna */
ROP_PROC(rop11, ~(T | (S & ~D)))	/* TSDnaon */
ROP_PROC(rop12, S & ~T)		/* STna */
ROP_PROC(rop13, ~(T | (D & ~S)))	/* TDSnaon */
ROP_PROC(rop14, ~(T | ~(D | S)))	/* TDSonon */
ROP_PROC(rop15, ~T)		/* Tn */
ROP_PROC(rop16, T & ~(D | S))		/* TDSona */
ROP_PROC(rop17, ~(D | S))	/* DSon */
ROP_PROC(rop18, ~(S | ~(D ^ T)))	/* SDTxnon */
ROP_PROC(rop19, ~(S | (D & T)))		/* SDTaon */
ROP_PROC(rop20, ~(D | ~(T ^ S)))	/* DTSxnon */
ROP_PROC(rop21, ~(D | (T & S)))		/* DTSaon */
ROP_PROC(rop22, (T ^ (S ^ (D & ~(T & S)))))		/* TSDTSanaxx */
ROP_PROC(rop23, ~(S ^ ((S ^ T) & (D ^ S))))		/* SSTxDSxaxn */
ROP_PROC(rop24, (S ^ T) & (T ^ D))	/* STxTDxa */
ROP_PROC(rop25, ~(S ^ (D & ~(T & S))))		/* SDTSanaxn */
ROP_PROC(rop26, T ^ (D | (S & T)))	/* TDSTaox */
ROP_PROC(rop27, ~(S ^ (D & (T ^ S))))		/* SDTSxaxn */
ROP_PROC(rop28, T ^ (S | (D & T)))	/* TSDTaox */
ROP_PROC(rop29, ~(D ^ (S & (T ^ D))))		/* DSTDxaxn */
ROP_PROC(rop30, T ^ (D | S))		/* TDSox */
ROP_PROC(rop31, ~(T & (D | S)))		/* TDSoan */
ROP_PROC(rop32, D & (T & ~S))		/* DTSnaa */
ROP_PROC(rop33, ~(S | (D ^ T)))		/* SDTxon */
ROP_PROC(rop34, D & ~S)		/* DSna */
ROP_PROC(rop35, ~(S | (T & ~D)))	/* STDnaon */
ROP_PROC(rop36, (S ^ T) & (D ^ S))	/* STxDSxa */
ROP_PROC(rop37, ~(T ^ (D & ~(S & T))))		/* TDSTanaxn */
ROP_PROC(rop38, S ^ (D | (T & S)))	/* SDTSaox */
ROP_PROC(rop39, S ^ (D | ~(T ^ S)))		/* SDTSxnox */
ROP_PROC(rop40, D & (T ^ S))		/* DTSxa */
ROP_PROC(rop41, ~(T ^ (S ^ (D | (T & S)))))		/* TSDTSaoxxn */
ROP_PROC(rop42, D & ~(T & S))		/* DTSana */
ROP_PROC(rop43, ~x(a(x(D, T), x(T, S)), S))		/* SSTxTDxaxn */
ROP_PROC(rop44, (S ^ (T & (D | S))))		/* STDSoax */
ROP_PROC(rop45, T ^ (S | ~D))		/* TSDnox */
ROP_PROC(rop46, (T ^ (S | (D ^ T))))		/* TSDTxox */
ROP_PROC(rop47, ~(T & (S | ~D)))	/* TSDnoan */
ROP_PROC(rop48, T & ~S)		/* TSna */
ROP_PROC(rop49, ~(S | (D & ~T)))	/* SDTnaon */
ROP_PROC(rop50, S ^ (D | (T | S)))	/* SDTSoox */
ROP_PROC(rop51, ~S)		/* Sn */
ROP_PROC(rop52, S ^ (T | (D & S)))	/* STDSaox */
ROP_PROC(rop53, S ^ (T | ~(D ^ S)))		/* STDSxnox */
ROP_PROC(rop54, S ^ (D | T))		/* SDTox */
ROP_PROC(rop55, ~(S & (D | T)))		/* SDToan */
ROP_PROC(rop56, T ^ (S & (D | T)))	/* TSDToax */
ROP_PROC(rop57, S ^ (T | ~D))		/* STDnox */
ROP_PROC(rop58, S ^ (T | (D ^ S)))	/* STDSxox */
ROP_PROC(rop59, ~(S & (T | ~D)))	/* STDnoan */
ROP_PROC(rop60, T ^ S)		/* TSx */
ROP_PROC(rop61, S ^ (T | ~(D | S)))		/* STDSonox */
ROP_PROC(rop62, S ^ (T | (D & ~S)))		/* STDSnaox */
ROP_PROC(rop63, ~(T & S))	/* TSan */
ROP_PROC(rop64, T & (S & ~D))		/* TSDnaa */
ROP_PROC(rop65, ~(D | (T ^ S)))		/* DTSxon */
ROP_PROC(rop66, (S ^ D) & (T ^ D))	/* SDxTDxa */
ROP_PROC(rop67, ~(S ^ (T & ~(D & S))))		/* STDSanaxn */
ROP_PROC(rop68, S & ~D)		/* SDna */
ROP_PROC(rop69, ~(D | (T & ~S)))	/* DTSnaon */
ROP_PROC(rop70, D ^ (S | (T & D)))	/* DSTDaox */
ROP_PROC(rop71, ~(T ^ (S & (D ^ T))))		/* TSDTxaxn */
ROP_PROC(rop72, S & (D ^ T))		/* SDTxa */
ROP_PROC(rop73, ~(T ^ (D ^ (S | (T & D)))))		/* TDSTDaoxxn */
ROP_PROC(rop74, D ^ (T & (S | D)))	/* DTSDoax */
ROP_PROC(rop75, T ^ (D | ~S))		/* TDSnox */
ROP_PROC(rop76, S & ~(D & T))		/* SDTana */
ROP_PROC(rop77, ~(S ^ ((S ^ T) | (D ^ S))))		/* SSTxDSxoxn */
ROP_PROC(rop78, T ^ (D | (S ^ T)))	/* TDSTxox */
ROP_PROC(rop79, ~(T & (D | ~S)))	/* TDSnoan */
ROP_PROC(rop80, T & ~D)		/* TDna */
ROP_PROC(rop81, ~(D | (S & ~T)))	/* DSTnaon */
ROP_PROC(rop82, D ^ (T | (S & D)))	/* DTSDaox */
ROP_PROC(rop83, ~(S ^ (T & (D ^ S))))		/* STDSxaxn */
ROP_PROC(rop84, ~(D | ~(T | S)))	/* DTSonon */
ROP_PROC(rop85, ~D)		/* Dn */
ROP_PROC(rop86, D ^ (T | S))		/* DTSox */
ROP_PROC(rop87, ~(D & (T | S)))		/* DTSoan */
ROP_PROC(rop88, T ^ (D & (S | T)))	/* TDSToax */
ROP_PROC(rop89, D ^ (T | ~S))		/* DTSnox */
ROP_PROC(rop90, D ^ T)		/* DTx */
ROP_PROC(rop91, D ^ (T | ~(S | D)))		/* DTSDonox */
ROP_PROC(rop92, D ^ (T | (S ^ D)))	/* DTSDxox */
ROP_PROC(rop93, ~(D & (T | ~S)))	/* DTSnoan */
ROP_PROC(rop94, D ^ (T | (S & ~D)))		/* DTSDnaox */
ROP_PROC(rop95, ~(D & T))	/* DTan */
ROP_PROC(rop96, T & (D ^ S))		/* TDSxa */
ROP_PROC(rop97, ~(D ^ (S ^ (T | (D & S)))))		/* DSTDSaoxxn */
ROP_PROC(rop98, D ^ (S & (T | D)))	/* DSTDoax */
ROP_PROC(rop99, S ^ (D | ~T))		/* SDTnox */
ROP_PROC(rop100, S ^ (D & (T | S)))		/* SDTSoax */
ROP_PROC(rop101, D ^ (S | ~T))		/* DSTnox */
ROP_PROC(rop102, D ^ S)		/* DSx */
ROP_PROC(rop103, S ^ (D | ~(T | S)))		/* SDTSonox */
ROP_PROC(rop104, ~(D ^ (S ^ (T | ~(D | S)))))		/* DSTDSonoxxn */
ROP_PROC(rop105, ~(T ^ (D ^ S)))	/* TDSxxn */
ROP_PROC(rop106, D ^ (T & S))		/* DTSax */
ROP_PROC(rop107, ~(T ^ (S ^ (D & (T | S)))))		/* TSDTSoaxxn */
ROP_PROC(rop108, (D & T) ^ S)		/* SDTax */
ROP_PROC(rop109, ~((((T | D) & S) ^ D) ^ T))		/* TDSTDoaxxn */
ROP_PROC(rop110, ((~S | T) & D) ^ S)		/* SDTSnoax */
ROP_PROC(rop111, ~(~(D ^ S) & T))	/* TDSxnan */
ROP_PROC(rop112, ~(D & S) & T)		/* TDSana */
ROP_PROC(rop113, ~(((S ^ D) & (T ^ D)) ^ S))		/* SSDxTDxaxn */
ROP_PROC(rop114, ((T ^ S) | D) ^ S)		/* SDTSxox */
ROP_PROC(rop115, ~((~T | D) & S))	/* SDTnoan */
ROP_PROC(rop116, ((T ^ D) | S) ^ D)		/* DSTDxox */
ROP_PROC(rop117, ~((~T | S) & D))	/* DSTnoan */
ROP_PROC(rop118, ((~S & T) | D) ^ S)		/* SDTSnaox */
ROP_PROC(rop119, ~(D & S))	/* DSan */
ROP_PROC(rop120, (D & S) ^ T)		/* TDSax */
ROP_PROC(rop121, ~((((D | S) & T) ^ S) ^ D))		/* DSTDSoaxxn */
ROP_PROC(rop122, ((~D | S) & T) ^ D)		/* DTSDnoax */
ROP_PROC(rop123, ~(~(D ^ T) & S))	/* SDTxnan */
ROP_PROC(rop124, ((~S | D) & T) ^ S)		/* STDSnoax */
ROP_PROC(rop125, ~(~(T ^ S) & D))	/* DTSxnan */
ROP_PROC(rop126, (S ^ T) | (D ^ S))		/* STxDSxo */
ROP_PROC(rop127, ~((T & S) & D))	/* DTSaan */
ROP_PROC(rop128, (T & S) & D)		/* DTSaa */
ROP_PROC(rop129, ~((S ^ T) | (D ^ S)))		/* STxDSxon */
ROP_PROC(rop130, ~(T ^ S) & D)		/* DTSxna */
ROP_PROC(rop131, ~(((~S | D) & T) ^ S))		/* STDSnoaxn */
ROP_PROC(rop132, ~(D ^ T) & S)		/* SDTxna */
ROP_PROC(rop133, ~(((~T | S) & D) ^ T))		/* TDSTnoaxn */
ROP_PROC(rop134, (((D | S) & T) ^ S) ^ D)	/* DSTDSoaxx */
ROP_PROC(rop135, ~((D & S) ^ T))	/* TDSaxn */
ROP_PROC(rop136, D & S)		/* DSa */
ROP_PROC(rop137, ~(((~S & T) | D) ^ S))		/* SDTSnaoxn */
ROP_PROC(rop138, (~T | S) & D)		/* DSTnoa */
ROP_PROC(rop139, ~(((T ^ D) | S) ^ D))		/* DSTDxoxn */
ROP_PROC(rop140, (~T | D) & S)		/* SDTnoa */
ROP_PROC(rop141, ~(((T ^ S) | D) ^ S))		/* SDTSxoxn */
ROP_PROC(rop142, ((S ^ D) & (T ^ D)) ^ S)	/* SSDxTDxax */
ROP_PROC(rop143, ~(~(D & S) & T))	/* TDSanan */
ROP_PROC(rop144, ~(D ^ S) & T)		/* TDSxna */
ROP_PROC(rop145, ~(((~S | T) & D) ^ S))		/* SDTSnoaxn */
ROP_PROC(rop146, (((D | T) & S) ^ T) ^ D)	/* DTSDToaxx */
ROP_PROC(rop147, ~((T & D) ^ S))	/* STDaxn */
ROP_PROC(rop148, (((T | S) & D) ^ S) ^ T)	/* TSDTSoaxx */
ROP_PROC(rop149, ~((T & S) ^ D))	/* DTSaxn */
ROP_PROC(rop150, (T ^ S) ^ D)		/* DTSxx */
ROP_PROC(rop151, ((~(T | S) | D) ^ S) ^ T)	/* TSDTSonoxx */
ROP_PROC(rop152, ~((~(T | S) | D) ^ S))		/* SDTSonoxn */
ROP_PROC(rop153, ~(D ^ S))	/* DSxn */
ROP_PROC(rop154, (~S & T) ^ D)		/* DTSnax */
ROP_PROC(rop155, ~(((T | S) & D) ^ S))		/* SDTSoaxn */
ROP_PROC(rop156, (~D & T) ^ S)		/* STDnax */
ROP_PROC(rop157, ~(((T | D) & S) ^ D))		/* DSTDoaxn */
ROP_PROC(rop158, (((D & S) | T) ^ S) ^ D)	/* DSTDSaoxx */
ROP_PROC(rop159, ~((D ^ S) & T))	/* TDSxan */
ROP_PROC(rop160, D & T)		/* DTa */
ROP_PROC(rop161, ~(((~T & S) | D) ^ T))		/* TDSTnaoxn */
ROP_PROC(rop162, (~S | T) & D)		/* DTSnoa */
ROP_PROC(rop163, ~(((D ^ S) | T) ^ D))		/* DTSDxoxn */
ROP_PROC(rop164, ~((~(T | S) | D) ^ T))		/* TDSTonoxn */
ROP_PROC(rop165, ~(D ^ T))	/* TDxn */
ROP_PROC(rop166, (~T & S) ^ D)		/* DSTnax */
ROP_PROC(rop167, ~(((T | S) & D) ^ T))		/* TDSToaxn */
ROP_PROC(rop168, ((S | T) & D))		/* DTSoa */
ROP_PROC(rop169, ~((S | T) ^ D))	/* DTSoxn */
ROP_PROC(rop170, D)		/* D */
ROP_PROC(rop171, ~(S | T) | D)		/* DTSono */
ROP_PROC(rop172, (((S ^ D) & T) ^ S))		/* STDSxax */
ROP_PROC(rop173, ~(((D & S) | T) ^ D))		/* DTSDaoxn */
ROP_PROC(rop174, (~T & S) | D)		/* DSTnao */
ROP_PROC(rop175, ~T | D)	/* DTno */
ROP_PROC(rop176, (~S | D) & T)		/* TDSnoa */
ROP_PROC(rop177, ~(((T ^ S) | D) ^ T))		/* TDSTxoxn */
ROP_PROC(rop178, ((S ^ D) | (S ^ T)) ^ S)	/* SSTxDSxox */
ROP_PROC(rop179, ~(~(T & D) & S))	/* SDTanan */
ROP_PROC(rop180, (~D & S) ^ T)		/* TSDnax */
ROP_PROC(rop181, ~(((D | S) & T) ^ D))		/* DTSDoaxn */
ROP_PROC(rop182, (((T & D) | S) ^ T) ^ D)	/* DTSDTaoxx */
ROP_PROC(rop183, ~((T ^ D) & S))	/* SDTxan */
ROP_PROC(rop184, ((T ^ D) & S) ^ T)		/* TSDTxax */
ROP_PROC(rop185, (~((D & T) | S) ^ D))		/* DSTDaoxn */
ROP_PROC(rop186, (~S & T) | D)		/* DTSnao */
ROP_PROC(rop187, ~S | D)	/* DSno */
ROP_PROC(rop188, (~(S & D) & T) ^ S)		/* STDSanax */
ROP_PROC(rop189, ~((D ^ T) & (D ^ S)))		/* SDxTDxan */
ROP_PROC(rop190, (S ^ T) | D)		/* DTSxo */
ROP_PROC(rop191, ~(S & T) | D)		/* DTSano */
ROP_PROC(rop192, T & S)		/* TSa */
ROP_PROC(rop193, ~(((~S & D) | T) ^ S))		/* STDSnaoxn */
ROP_PROC(rop194, ~x(o(~o(S, D), T), S))		/* STDSonoxn */
ROP_PROC(rop195, ~(S ^ T))	/* TSxn */
ROP_PROC(rop196, ((~D | T) & S))	/* STDnoa */
ROP_PROC(rop197, ~(((S ^ D) | T) ^ S))		/* STDSxoxn */
ROP_PROC(rop198, ((~T & D) ^ S))	/* SDTnax */
ROP_PROC(rop199, ~(((T | D) & S) ^ T))		/* TSDToaxn */
ROP_PROC(rop200, ((T | D) & S))		/* SDToa */
ROP_PROC(rop201, ~((D | T) ^ S))	/* STDoxn */
ROP_PROC(rop202, ((D ^ S) & T) ^ D)		/* DTSDxax */
ROP_PROC(rop203, ~(((S & D) | T) ^ S))		/* STDSaoxn */
ROP_PROC(rop204, S)		/* S */
ROP_PROC(rop205, ~(T | D) | S)		/* SDTono */
ROP_PROC(rop206, (~T & D) | S)		/* SDTnao */
ROP_PROC(rop207, ~T | S)	/* STno */
ROP_PROC(rop208, (~D | S) & T)		/* TSDnoa */
ROP_PROC(rop209, ~(((T ^ D) | S) ^ T))		/* TSDTxoxn */
ROP_PROC(rop210, (~S & D) ^ T)		/* TDSnax */
ROP_PROC(rop211, ~(((S | D) & T) ^ S))		/* STDSoaxn */
ROP_PROC(rop212, x(a(x(D, T), x(T, S)), S))		/* SSTxTDxax */
ROP_PROC(rop213, ~(~(S & T) & D))	/* DTSanan */
ROP_PROC(rop214, ((((S & T) | D) ^ S) ^ T))		/* TSDTS aoxx */
ROP_PROC(rop215, ~((S ^ T) & D))	/* DTS xan */
ROP_PROC(rop216, ((T ^ S) & D) ^ T)		/* TDST xax */
ROP_PROC(rop217, ~(((S & T) | D) ^ S))		/* SDTS aoxn */
ROP_PROC(rop218, x(a(~a(D, S), T), D))		/* DTSD anax */
ROP_PROC(rop219, ~a(x(S, D), x(T, S)))		/* STxDSxan */
ROP_PROC(rop220, (~D & T) | S)		/* STD nao */
ROP_PROC(rop221, ~D | S)	/* SDno */
ROP_PROC(rop222, (T ^ D) | S)		/* SDT xo */
ROP_PROC(rop223, (~(T & D)) | S)	/* SDT ano */
ROP_PROC(rop224, ((S | D) & T))		/* TDS oa */
ROP_PROC(rop225, ~((S | D) ^ T))	/*  TDS oxn */
ROP_PROC(rop226, (((D ^ T) & S) ^ D))		/* DSTD xax */
ROP_PROC(rop227, ~(((T & D) | S) ^ T))		/* TSDT aoxn */
ROP_PROC(rop228, ((S ^ T) & D) ^ S)		/* SDTSxax */
ROP_PROC(rop229, ~(((T & S) | D) ^ T))		/* TDST aoxn */
ROP_PROC(rop230, (~(S & T) & D) ^ S)		/* SDTSanax */
ROP_PROC(rop231, ~a(x(D, T), x(T, S)))		/* STxTDxan */
ROP_PROC(rop232, x(a(x(S, D), x(T, S)), S))		/* SS TxD Sxax */
ROP_PROC(rop233, ~x(x(a(~a(S, D), T), S), D))		/* DST DSan axxn   */
ROP_PROC(rop234, (S & T) | D)		/* DTSao */
ROP_PROC(rop235, ~(S ^ T) | D)		/* DTSxno */
ROP_PROC(rop236, (T & D) | S)		/* SDTao */
ROP_PROC(rop237, ~(T ^ D) | S)		/* SDTxno */
ROP_PROC(rop238, S | D)		/* DSo */
ROP_PROC(rop239, (~T | D) | S)		/* SDTnoo */
ROP_PROC(rop240, T)		/* T */
ROP_PROC(rop241, ~(S | D) | T)		/* TDSono */
ROP_PROC(rop242, (~S & D) | T)		/* TDSnao */
ROP_PROC(rop243, ~S | T)	/* TSno */
ROP_PROC(rop244, (~D & S) | T)		/* TSDnao */
ROP_PROC(rop245, ~D | T)	/* TDno */
ROP_PROC(rop246, (S ^ D) | T)		/* TDSxo */
ROP_PROC(rop247, ~(S & D) | T)		/* TDSano */
ROP_PROC(rop248, (S & D) | T)		/* TDSao */
ROP_PROC(rop249, ~(S ^ D) | T)		/* TDSxno */
ROP_PROC(rop250, D | T)		/* DTo */
ROP_PROC(rop251, (~S | T) | D)		/* DTSnoo */
ROP_PROC(rop252, S | T)		/* TSo */
ROP_PROC(rop253, (~D | S) | T)		/* TSDnoo */
ROP_PROC(rop254, S | T | D)	/* DTSoo */
ROP_PROC(rop255, ~(ulong) 0)	/* 1 */
#undef ROP_PROC
     
const rop_proc rop_proc_table[256] =
{
	 rop0, rop1, rop2, rop3, rop4, rop5, rop6, rop7,
	 rop8, rop9, rop10, rop11, rop12, rop13, rop14, rop15,
	 rop16, rop17, rop18, rop19, rop20, rop21, rop22, rop23,
	 rop24, rop25, rop26, rop27, rop28, rop29, rop30, rop31,
	 rop32, rop33, rop34, rop35, rop36, rop37, rop38, rop39,
	 rop40, rop41, rop42, rop43, rop44, rop45, rop46, rop47,
	 rop48, rop49, rop50, rop51, rop52, rop53, rop54, rop55,
	 rop56, rop57, rop58, rop59, rop60, rop61, rop62, rop63,
	 rop64, rop65, rop66, rop67, rop68, rop69, rop70, rop71,
	 rop72, rop73, rop74, rop75, rop76, rop77, rop78, rop79,
	 rop80, rop81, rop82, rop83, rop84, rop85, rop86, rop87,
	 rop88, rop89, rop90, rop91, rop92, rop93, rop94, rop95,
	 rop96, rop97, rop98, rop99, rop100, rop101, rop102, rop103,
	 rop104, rop105, rop106, rop107, rop108, rop109, rop110, rop111,
	 rop112, rop113, rop114, rop115, rop116, rop117, rop118, rop119,
	 rop120, rop121, rop122, rop123, rop124, rop125, rop126, rop127,
	 rop128, rop129, rop130, rop131, rop132, rop133, rop134, rop135,
	 rop136, rop137, rop138, rop139, rop140, rop141, rop142, rop143,
	 rop144, rop145, rop146, rop147, rop148, rop149, rop150, rop151,
	 rop152, rop153, rop154, rop155, rop156, rop157, rop158, rop159,
	 rop160, rop161, rop162, rop163, rop164, rop165, rop166, rop167,
	 rop168, rop169, rop170, rop171, rop172, rop173, rop174, rop175,
	 rop176, rop177, rop178, rop179, rop180, rop181, rop182, rop183,
	 rop184, rop185, rop186, rop187, rop188, rop189, rop190, rop191,
	 rop192, rop193, rop194, rop195, rop196, rop197, rop198, rop199,
	 rop200, rop201, rop202, rop203, rop204, rop205, rop206, rop207,
	 rop208, rop209, rop210, rop211, rop212, rop213, rop214, rop215,
	 rop216, rop217, rop218, rop219, rop220, rop221, rop222, rop223,
	 rop224, rop225, rop226, rop227, rop228, rop229, rop230, rop231,
	 rop232, rop233, rop234, rop235, rop236, rop237, rop238, rop239,
	 rop240, rop241, rop242, rop243, rop244, rop245, rop246, rop247,
	 rop248, rop249, rop250, rop251, rop252, rop253, rop254, rop255
};

#undef a
#undef o
#undef x

const uchar rop_usage_table[256] =
{
	 0, 7, 7, 6, 7, 5, 7, 7, 7, 7, 5, 7, 6, 7, 7, 4,
	 7, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	 7, 7, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	 6, 7, 7, 2, 7, 7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 6,
	 7, 7, 7, 7, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	 5, 7, 7, 7, 7, 1, 7, 7, 7, 7, 5, 7, 7, 7, 7, 5,
	 7, 7, 7, 7, 7, 7, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	 7, 7, 7, 7, 7, 7, 7, 3, 7, 7, 7, 7, 7, 7, 7, 7,
	 7, 7, 7, 7, 7, 7, 7, 7, 3, 7, 7, 7, 7, 7, 7, 7,
	 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 7, 7, 7, 7, 7, 7,
	 5, 7, 7, 7, 7, 5, 7, 7, 7, 7, 1, 7, 7, 7, 7, 5,
	 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 7, 7, 7, 7,
	 6, 7, 7, 6, 7, 7, 7, 7, 7, 7, 7, 7, 2, 7, 7, 6,
	 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 7, 7,
	 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 7,
	 4, 7, 7, 6, 7, 5, 7, 7, 7, 7, 5, 7, 6, 7, 7, 0
};
     
PSRop3 ps_transparent_rop(ulong lop)
{
	PSRop3 rop = lop_rop(lop);
	
#define So rop3_not(rop3_S)
#define Po rop3_not(rop3_T)
#ifdef TRANSPARENCY_PER_H_P
#  define MPo (rop3_not(So) | Po)
#else
#  define MPo Po
#endif
  
#define source_transparent ((lop & lop_S_transparent) && rop3_uses_S(rop))
#define pattern_transparent ((lop & lop_T_transparent) && rop3_uses_T(rop))

  PSRop3 mask = (PSRop3)
    ((source_transparent ?
     (pattern_transparent ? So & Po : So) :
     (pattern_transparent ? MPo : rop3_1)));

#undef MPo
  return (PSRop3)((rop & mask) | (rop3_D & ~mask));
}

void memflip8x8(const uchar * inp, int line_size, uchar * outp, int dist)
{
	uint aceg, bdfh;

  {
		const uchar *ptr4 = inp + (line_size << 2);
		const int ls2 = line_size << 1;

		aceg = ((uint)*inp) | ((uint)inp[ls2] << 8) |  ((uint)*ptr4 << 16) | ((uint)ptr4[ls2] << 24);
		inp += line_size, ptr4 += line_size;
		bdfh = ((uint)*inp) | ((uint)inp[ls2] << 8) | ((uint)*ptr4 << 16) | ((uint)ptr4[ls2] << 24);
  }

  if (aceg == bdfh && (aceg >> 8) == (aceg & 0xffffff)) 
  {
		if (aceg == 0)
	    goto store;
		*outp = -((aceg >> 7) & 1);
		outp[dist] = -((aceg >> 6) & 1);
		outp += dist << 1;
		*outp = -((aceg >> 5) & 1);
		outp[dist] = -((aceg >> 4) & 1);
		outp += dist << 1;
		*outp = -((aceg >> 3) & 1);
		outp[dist] = -((aceg >> 2) & 1);
		outp += dist << 1;
		*outp = -((aceg >> 1) & 1);
		outp[dist] = -(aceg & 1);
		return;
  } 
  {
		uint temp;

#define TRANSPOSE(r,s,mask,shift)\
  (r ^= (temp = ((s >> shift) ^ r) & mask),\
   s ^= temp << shift)

		TRANSPOSE(aceg, aceg, 0x00000f0f, 20);
		TRANSPOSE(bdfh, bdfh, 0x00000f0f, 20);

		TRANSPOSE(aceg, aceg, 0x00330033, 10);
		TRANSPOSE(bdfh, bdfh, 0x00330033, 10);

		TRANSPOSE(aceg, bdfh, 0x55555555, 1);

#undef TRANSPOSE
  }

store:
  *outp = (uchar)aceg;
  outp[dist] = (uchar)bdfh;
  outp += dist << 1;
  *outp = (uchar)(aceg >>= 8);
  outp[dist] = (uchar)(bdfh >>= 8);
  outp += dist << 1;
  *outp = (uchar)(aceg >>= 8);
  outp[dist] = (uchar)(bdfh >>= 8);
  outp += dist << 1;
  *outp = (uchar)(aceg >> 8);
  outp[dist] = (uchar)(bdfh >> 8);
}

static const ushort ps_q0[] = {
    0
};
static const ushort ps_q1[] = {
    0, frac_color_(1, 1)
};
static const ushort ps_q2[] = {
    0, frac_color_(1, 2), frac_color_(2, 2)
};
static const ushort ps_q3[] = {
    0, frac_color_(1, 3), frac_color_(2, 3), frac_color_(3, 3)
};
static const ushort ps_q4[] = {
    0, frac_color_(1, 4), frac_color_(2, 4), frac_color_(3, 4),
    frac_color_(4, 4)
};
static const ushort ps_q5[] = {
    0, frac_color_(1, 5), frac_color_(2, 5), frac_color_(3, 5),
    frac_color_(4, 5), frac_color_(5, 5)
};
static const ushort ps_q6[] = {
    0, frac_color_(1, 6), frac_color_(2, 6), frac_color_(3, 6),
    frac_color_(4, 6), frac_color_(5, 6), frac_color_(6, 6)
};
static const ushort ps_q7[] = {
    0, frac_color_(1, 7), frac_color_(2, 7), frac_color_(3, 7),
    frac_color_(4, 7), frac_color_(5, 7), frac_color_(6, 7), frac_color_(7, 7)
};

const ushort *const fc_color_quo[8] = {
    ps_q0, ps_q1, ps_q2, ps_q3, ps_q4, ps_q5, ps_q6, ps_q7
};

int bytes_compare(const uchar * s1, uint len1, const uchar * s2, uint len2)
{
	uint len = len1;
  if (len2 < len)
		len = len2;
    
  {
		const uchar *p1 = s1;
		const uchar *p2 = s2;

		while (len--)
	    if (*p1++ != *p2++)
		return (p1[-1] < p2[-1] ? -1 : 1);
  }
  
  return (len1 == len2 ? 0 : len1 < len2 ? -1 : 1);
}
  
int ps_type1_encrypt(uchar * dest, const uchar * src, uint len, ushort * pstate)
{
	ushort state = *pstate;
  const uchar *from = src;
  uchar *to = dest;
  uint count = len;
  while (count) 
  {
		encrypt_next(*from, state, *to);
		from++, to++, count--;
  }
  *pstate = state;
  return 0;
}

int ps_type1_decrypt(uchar * dest, const uchar * src, uint len, ushort * pstate)
{
	ushort state = *pstate;
  const uchar *from = src;
  uchar *to = dest;
  uint count = len;
  while (count)
  {
  	uchar ch = *from++;

		decrypt_next(ch, state, *to);
		to++, count--;
  }
  *pstate = state;
  return 0;
}

ulong get_u32_msb(const uchar *p)
{
	return ((uint)p[0] << 24) + ((uint)p[1] << 16) + ((uint)p[2] << 8) + p[3];
}

int igcd(int x, int y)
{
	int c = x, d = y;

  if (c < 0)
		c = -c;
  if (d < 0)
		d = -d;
  while (c != 0 && d != 0)
		if (c > d)
	    c %= d;
		else
	    d %= c;
  return d + c;
}

int ilog2(int n)
{
	int m = n, l = 0;

  while (m >= 16)
		m >>= 4, l += 4;
  return	(m <= 1 ? 0 : "\000\000\001\001\002\002\002\002\003\003\003\003\003\003\003\003"[m] + l);
}

int imod(int m, int n)
{
	if (n <= 0)
		return 0;
  if (m >= 0)
		return m % n;
  {
		int r = -m % n;

		return (r == 0 ? 0 : n - r);
  }
}

#define num_bits (sizeof(long) * 8)
#define half_bits (num_bits / 2)
#define half_mask ((1L << half_bits) - 1)

#if USE_FPU_FIXED || (arch_double_mantissa_bits < arch_sizeof_long * 12)

long fixed_mult_quo(long signed_A, long B, long C)
{
	ulong A = (signed_A < 0 ? -signed_A : signed_A);
  long msw;
  ulong lsw;
  ulong p1;
  if (B <= half_mask)
  {
  	if (A <= half_mask) 
  	{
	    ulong P = A * B;
	    long Q = P / (ulong)C;
	    return (signed_A >= 0 ? Q : Q * C == P ? -Q : ~Q);
		}
		lsw = (A & half_mask) * B;
		p1 = (A >> half_bits) * B;
		if (C <= half_mask) 
		{
	    long q0 = (p1 += lsw >> half_bits) / C;
	    ulong rem = ((p1 - C * q0) << half_bits) + (lsw & half_mask);
	    ulong q1 = rem / (ulong)C;
	    long Q = (q0 << half_bits) + q1;
	    return (signed_A >= 0 ? Q : q1 * C == rem ? -Q : ~Q);
		}
		msw = p1 >> half_bits;
  }
  else if (A <= half_mask)
  {
  	p1 = A * (B >> half_bits);
		msw = p1 >> half_bits;
		lsw = A * (B & half_mask);
  }
  else
  {
  	ulong lo_A = A & half_mask;
		ulong hi_A = A >> half_bits;
		ulong lo_B = B & half_mask;
		ulong hi_B = B >> half_bits;
		ulong p1x = hi_A * lo_B;

		msw = hi_A * hi_B;
		lsw = lo_A * lo_B;
		p1 = lo_A * hi_B;
		if (p1 > max_ulong - p1x)
	    msw += 1L << half_bits;
		p1 += p1x;
		msw += p1 >> half_bits;
  }
  
#if max_fixed < max_long
  p1 &= half_mask;
#endif
  p1 <<= half_bits;
  if (p1 > max_ulong - lsw)
		msw++;
  lsw += p1;
  
  {
  	ulong denom = C;
		int shift = 0;

#define bits_4th (num_bits / 4)
		if (denom < 1L << (num_bits - bits_4th)) 
		{
	  	denom <<= bits_4th, shift += bits_4th;
		}
#undef bits_4th
#define bits_8th (num_bits / 8)
		if (denom < 1L << (num_bits - bits_8th)) 
		{
	  	denom <<= bits_8th, shift += bits_8th;
		}
#undef bits_8th
		while (!(denom & (-1L << (num_bits - 1)))) 
		{
	  	denom <<= 1, ++shift;
		}
		msw = (msw << shift) + (lsw >> (num_bits - shift));
		lsw <<= shift;
#if max_fixed < max_long
		lsw &= (1L << (sizeof(long) * 8)) - 1;
#endif

		{
			ulong hi_D = denom >> half_bits;
			ulong lo_D = denom & half_mask;
			ulong hi_Q = (ulong) msw / hi_D;
			ulong p0 = hi_Q * hi_D;
			ulong p1 = hi_Q * lo_D;
			ulong hi_P;
			while ((hi_P = p0 + (p1 >> half_bits)) > msw ||
		   	(hi_P == msw && ((p1 & half_mask) << half_bits) > lsw))
			{
				--hi_Q;
				p0 -= hi_D;
				p1 -= lo_D;
			}
	
			p1 = (p1 & half_mask) << half_bits;
			if (p1 > lsw)
				msw--;
			lsw -= p1;
			msw -= hi_P;
			msw = (msw << half_bits) + (lsw >> half_bits);
#if max_fixed < max_long
			lsw &= half_mask;
#endif
			lsw <<= half_bits;
			
			{
				ulong lo_Q = (ulong) msw / hi_D;
				p1 = lo_Q * lo_D;
				p0 = lo_Q * hi_D;
				while ((hi_P = p0 + (p1 >> half_bits)) > msw ||
		       (hi_P == msw && ((p1 & half_mask) << half_bits) > lsw)) 
				{
	  			--lo_Q;
	  			p0 -= hi_D;
	  			p1 -= lo_D;
				}
				long	Q = (hi_Q << half_bits) + lo_Q;
				return (signed_A >= 0 ? Q : p0 | p1 ? ~Q : -Q);
			}
		}
	}
}

#else

long fixed_mult_quo(long signed_A, long B, long C)
{
#define MAX_OTHER_FACTOR\
  (1L << (arch_double_mantissa_bits - sizeof(long) * 8))
  
  if (B < MAX_OTHER_FACTOR || qAbs(signed_A) < MAX_OTHER_FACTOR) 
  {
		return (long)floor((double)signed_A * B / C);
  }
#undef MAX_OTHER_FACTOR
  {
  	
		long bhi = B >> half_bits;
		long qhi = (long)floor((double)signed_A * bhi / C);
		long rhi = signed_A * bhi - qhi * C;
		long blo = B & half_mask;
		long qlo = (long)floor(((double)rhi * (1L << half_bits) + (double)signed_A * blo) / C);

		return (qhi << half_bits) + qlo;
  }
}

#endif

#undef num_bits
#undef half_bits
#undef half_mask

long fixed_mult_rem(long a, long b, long c)
{
	return a * b - fixed_mult_quo(a, b, c) * c;
}

void bytes_fill_rectangle(uchar * dest, 
                          uint raster,
		                      uchar value, 
		                      int width_bytes, 
		                      int height)
{
	while (height-- > 0) 
	{
		memset(dest, value, width_bytes);
		dest += raster;
  }
}

void bytes_copy_rectangle(uchar * dest, 
                          uint dest_raster,
	     						        const uchar * src, 
	     						        uint src_raster, 
	     						        int width_bytes, 
	     						        int height)
{
	while (height-- > 0) 
	{
		memcpy(dest, src, width_bytes);
		src += src_raster;
		dest += dest_raster;
  }
}

#define ARCH_HAS_BYTE_REGS 1

#define TRANSPOSE(r,s,mask,shift)\
  r ^= (temp = ((s >> shift) ^ r) & mask);\
  s ^= temp << shift
  
#if ARCH_HAS_BYTE_REGS
typedef uchar byte_var;
#else
typedef uint byte_var;
#endif

#define VTAB(v80,v40,v20,v10,v8,v4,v2,v1)\
  bit_table_8(0,v80,v40,v20,v10,v8,v4,v2,v1)
  
static int
flip3x1(uchar * buffer, const uchar ** planes, int offset, int nbytes)
{
    uchar *out = buffer;
    const uchar *in1 = planes[0] + offset;
    const uchar *in2 = planes[1] + offset;
    const uchar *in3 = planes[2] + offset;
    int n = nbytes;
    static const quint32 tab3x1[256] = {VTAB(0x800000, 0x100000, 0x20000, 0x4000, 0x800, 0x100, 0x20, 4)};

    for (; n > 0; out += 3, ++in1, ++in2, ++in3, --n) 
    {
			quint32 b24 = tab3x1[*in1] | (tab3x1[*in2] >> 1) | (tab3x1[*in3] >> 2);

			out[0] = (uchar) (b24 >> 16);
			out[1] = (uchar) (b24 >> 8);
			out[2] = (uchar) b24;
    }
    return 0;
}

static int
flip3x2(uchar * buffer, const uchar ** planes, int offset, int nbytes)
{
    uchar *out = buffer;
    const uchar *in1 = planes[0] + offset;
    const uchar *in2 = planes[1] + offset;
    const uchar *in3 = planes[2] + offset;
    int n = nbytes;
    static const quint32 tab3x2[256] = {VTAB(0x800000, 0x400000, 0x20000, 0x10000, 0x800, 0x400, 0x20, 0x10) };

    for (; n > 0; out += 3, ++in1, ++in2, ++in3, --n) 
    {
			quint32 b24 = tab3x2[*in1] | (tab3x2[*in2] >> 2) | (tab3x2[*in3] >> 4);

			out[0] = (uchar) (b24 >> 16);
			out[1] = (uchar) (b24 >> 8);
			out[2] = (uchar) b24;
    }
    return 0;
}

static int
flip3x4(uchar * buffer, const uchar ** planes, int offset, int nbytes)
{
    uchar *out = buffer;
    const uchar *in1 = planes[0] + offset;
    const uchar *in2 = planes[1] + offset;
    const uchar *in3 = planes[2] + offset;
    int n = nbytes;

    for (; n > 0; out += 3, ++in1, ++in2, ++in3, --n) 
    {
			byte_var b1 = *in1, b2 = *in2, b3 = *in3;

			out[0] = (b1 & 0xf0) | (b2 >> 4);
			out[1] = (b3 & 0xf0) | (b1 & 0xf);
			out[2] = (uchar) (b2 << 4) | (b3 & 0xf);
    }
    return 0;
}

static int
flip3x8(uchar * buffer, const uchar ** planes, int offset, int nbytes)
{
    uchar *out = buffer;
    const uchar *in1 = planes[0] + offset;
    const uchar *in2 = planes[1] + offset;
    const uchar *in3 = planes[2] + offset;
    int n = nbytes;

    for (; n > 0; out += 3, ++in1, ++in2, ++in3, --n) 
    {
			out[0] = *in1;
			out[1] = *in2;
			out[2] = *in3;
    }
    return 0;
}

static int
flip3x12(uchar * buffer, const uchar ** planes, int offset, int nbytes)
{
    uchar *out = buffer;
    const uchar *pa = planes[0] + offset;
    const uchar *pb = planes[1] + offset;
    const uchar *pc = planes[2] + offset;
    int n = nbytes;
    for (; n > 0; out += 9, pa += 3, pb += 3, pc += 3, n -= 3) 
    {
			byte_var a1 = pa[1], b0 = pb[0], b1 = pb[1], b2 = pb[2], c1 = pc[1];

			out[0] = pa[0];
			out[1] = (a1 & 0xf0) | (b0 >> 4);
			out[2] = (uchar) ((b0 << 4) | (b1 >> 4));
			out[3] = pc[0];
			out[4] = (c1 & 0xf0) | (a1 & 0xf);
			out[5] = pa[2];
			out[6] = (uchar) ((b1 << 4) | (b2 >> 4));
			out[7] = (uchar) ((b2 << 4) | (c1 & 0xf));
			out[8] = pc[2];
    }
    return 0;
}

static int
flip4x1(uchar * buffer, const uchar ** planes, int offset, int nbytes)
{
    uchar *out = buffer;
    const uchar *in1 = planes[0] + offset;
    const uchar *in2 = planes[1] + offset;
    const uchar *in3 = planes[2] + offset;
    const uchar *in4 = planes[3] + offset;
    int n = nbytes;

    for (; n > 0; out += 4, ++in1, ++in2, ++in3, ++in4, --n) 
    {
			byte_var b1 = *in1, b2 = *in2, b3 = *in3, b4 = *in4;
			byte_var temp;
			
			TRANSPOSE(b1, b2, 0x55, 1);
			TRANSPOSE(b3, b4, 0x55, 1);
			
			TRANSPOSE(b1, b3, 0x33, 2);
			TRANSPOSE(b2, b4, 0x33, 2);
			
			out[0] = (b1 & 0xf0) | (b2 >> 4);
			out[1] = (b3 & 0xf0) | (b4 >> 4);
			out[2] = (uchar) ((b1 << 4) | (b2 & 0xf));
			out[3] = (uchar) ((b3 << 4) | (b4 & 0xf));
    }
    return 0;
}

static int
flip4x2(uchar * buffer, const uchar ** planes, int offset, int nbytes)
{
    uchar *out = buffer;
    const uchar *in1 = planes[0] + offset;
    const uchar *in2 = planes[1] + offset;
    const uchar *in3 = planes[2] + offset;
    const uchar *in4 = planes[3] + offset;
    int n = nbytes;

    for (; n > 0; out += 4, ++in1, ++in2, ++in3, ++in4, --n) 
    {
			byte_var b1 = *in1, b2 = *in2, b3 = *in3, b4 = *in4;
			byte_var temp;

			TRANSPOSE(b1, b3, 0x0f, 4);
			TRANSPOSE(b2, b4, 0x0f, 4);
			TRANSPOSE(b1, b2, 0x33, 2);
			TRANSPOSE(b3, b4, 0x33, 2);
			out[0] = b1;
			out[1] = b2;
			out[2] = b3;
			out[3] = b4;
    }
    return 0;
}

static int
flip4x4(uchar * buffer, const uchar ** planes, int offset, int nbytes)
{
    uchar *out = buffer;
    const uchar *in1 = planes[0] + offset;
    const uchar *in2 = planes[1] + offset;
    const uchar *in3 = planes[2] + offset;
    const uchar *in4 = planes[3] + offset;
    int n = nbytes;

    for (; n > 0; out += 4, ++in1, ++in2, ++in3, ++in4, --n) 
    {
			byte_var b1 = *in1, b2 = *in2, b3 = *in3, b4 = *in4;

			out[0] = (b1 & 0xf0) | (b2 >> 4);
			out[1] = (b3 & 0xf0) | (b4 >> 4);
			out[2] = (uchar) ((b1 << 4) | (b2 & 0xf));
			out[3] = (uchar) ((b3 << 4) | (b4 & 0xf));
    }
    return 0;
}

static int
flip4x8(uchar * buffer, const uchar ** planes, int offset, int nbytes)
{
    uchar *out = buffer;
    const uchar *in1 = planes[0] + offset;
    const uchar *in2 = planes[1] + offset;
    const uchar *in3 = planes[2] + offset;
    const uchar *in4 = planes[3] + offset;
    int n = nbytes;

    for (; n > 0; out += 4, ++in1, ++in2, ++in3, ++in4, --n) 
    {
			out[0] = *in1;
			out[1] = *in2;
			out[2] = *in3;
			out[3] = *in4;
    }
    return 0;
}

static int
flip4x12(uchar * buffer, const uchar ** planes, int offset, int nbytes)
{
    uchar *out = buffer;
    const uchar *pa = planes[0] + offset;
    const uchar *pb = planes[1] + offset;
    const uchar *pc = planes[2] + offset;
    const uchar *pd = planes[3] + offset;
    int n = nbytes;
    for (; n > 0; out += 12, pa += 3, pb += 3, pc += 3, pd += 3, n -= 3) 
    {
			byte_var a1 = pa[1], b1 = pb[1], c1 = pc[1], d1 = pd[1];

			{
	    	byte_var v0;

	    	out[0] = pa[0];
	    	v0 = pb[0];
	    	out[1] = (a1 & 0xf0) | (v0 >> 4);
	    	out[2] = (uchar) ((v0 << 4) | (b1 >> 4));
	    	out[3] = pc[0];
	    	v0 = pd[0];
	    	out[4] = (c1 & 0xf0) | (v0 >> 4);
	    	out[5] = (uchar) ((v0 << 4) | (d1 >> 4));
			}
			{
	    	byte_var v2;

	    	v2 = pa[2];
	    	out[6] = (uchar) ((a1 << 4) | (v2 >> 4));
	    	out[7] = (uchar) ((v2 << 4) | (b1 & 0xf));
	    	out[8] = pb[2];
	    	v2 = pc[2];
	    	out[9] = (uchar) ((c1 << 4) | (v2 >> 4));
	    	out[10] = (uchar) ((v2 << 4) | (d1 & 0xf));
	    	out[11] = pd[2];
			}
    }
    return 0;
}

static int
flipNx1to8(uchar * buffer, const uchar ** planes, int offset, int nbytes,
	   int num_planes, int bits_per_sample)
{
    uint mask = (1 << bits_per_sample) - 1;
    int bi, pi;
    sample_store_declare_setup(dptr, dbit, dbbyte, buffer, 0, bits_per_sample);

    for (bi = 0; bi < nbytes * 8; bi += bits_per_sample) 
    {
			for (pi = 0; pi < num_planes; ++pi) 
			{
	    	const uchar *sptr = planes[pi] + offset + (bi >> 3);
	    	uint value = (*sptr >> (8 - (bi & 7) - bits_per_sample)) & mask;

	    	sample_store_next8(value, dptr, dbit, bits_per_sample, dbbyte);
			}
    }
    sample_store_flush(dptr, dbit, bits_per_sample, dbbyte);
    return 0;
}

static int
flipNx12(uchar * buffer, const uchar ** planes, int offset, int nbytes,
	 int num_planes, int )
{
    int bi, pi;
    sample_store_declare_setup(dptr, dbit, dbbyte, buffer, 0, 12);

    for (bi = 0; bi < nbytes * 8; bi += 12) 
    {
			for (pi = 0; pi < num_planes; ++pi) 
			{
	    	const uchar *sptr = planes[pi] + offset + (bi >> 3);
	    	uint value =(bi & 4 ? ((*sptr & 0xf) << 8) | sptr[1] : (*sptr << 4) | (sptr[1] >> 4));

	    	sample_store_next_12(value, dptr, dbit, dbbyte);
			}
    }
    sample_store_flush(dptr, dbit, 12, dbbyte);
    return 0;
}

typedef int (*image_flip_proc)(uchar *, const uchar **, int, int);
static int
flip_fail(uchar * , const uchar ** , int , int )
{
    return -1;
}
static const image_flip_proc image_flip3_procs[13] = {
    flip_fail, flip3x1, flip3x2, flip_fail, flip3x4,
    flip_fail, flip_fail, flip_fail, flip3x8,
    flip_fail, flip_fail, flip_fail, flip3x12
};
static const image_flip_proc image_flip4_procs[13] = {
    flip_fail, flip4x1, flip4x2, flip_fail, flip4x4,
    flip_fail, flip_fail, flip_fail, flip4x8,
    flip_fail, flip_fail, flip_fail, flip4x12
};
typedef int (*image_flipN_proc)(uchar *, const uchar **, int, int, int, int);

static int
flipN_fail(uchar * , const uchar ** , int , int ,
	   int , int )
{
    return -1;
}
static const image_flipN_proc image_flipN_procs[13] = {
    flipN_fail, flipNx1to8, flipNx1to8, flipN_fail, flipNx1to8,
    flipN_fail, flipN_fail, flipN_fail, flipNx1to8,
    flipN_fail, flipN_fail, flipN_fail, flipNx12
};

/* Here is the public interface to all of the above. */
int
image_flip_planes(uchar * buffer, const uchar ** planes, int offset, int nbytes,
		  int num_planes, int bits_per_sample)
{
    if (bits_per_sample < 1 || bits_per_sample > 12)
			return -1;
    switch (num_planes) 
    {
    	case 3:
				return (image_flip3_procs[bits_per_sample])(buffer, planes, offset, nbytes);
				
    	case 4:
				return (image_flip4_procs[bits_per_sample])(buffer, planes, offset, nbytes);
				
    	default:
				if (num_planes < 0)
	    		return -1;
			return (image_flipN_procs[bits_per_sample])(buffer, planes, offset, nbytes, num_planes, bits_per_sample);
    }
}

static const int isincos[5] =
{0, 1, 0, -1, 0};

double ps_cos_degrees(double ang)
{
	double quot = ang / 90;

  if (floor(quot) == quot) 
  {
		return isincos[((int)fmod(quot, 4.0) & 3) + 1];
  }
  return cos(ang * (M_PI / 180));
}

double ps_sin_degrees(double ang)
{
	double quot = ang / 90;

  if (floor(quot) == quot) 
  {
		return isincos[(int)fmod(quot, 4.0) & 3];
  }
  return sin(ang * (M_PI / 180));
}

int ps_atan2_degrees(double y, double x, double *pangle)
{
	if (y == 0) 
	{
		if (x == 0)
		{
	    return (int)(XWPSError::UndefinedResult);
	  }
		*pangle = (x < 0 ? 180 : 0);
  } 
  else 
  {
		double result = atan2(y, x) * radians_to_degrees;

		if (result < 0)
	    result += 360;
		*pangle = result;
  }
  return 0;
}

int set_dfmul2fixed_(long * pr, ulong xalo, long b, long xahi)
{
	return set_fmul2fixed_(pr, (xahi & (3L << 30)) + ((xahi << 3) & 0x3ffffff8) + (xalo >> 29), b);
}

int set_fmul2fixed_(long *pr, long a, long b)
{
	ulong ma = (ushort)(a >> 8) | 0x8000;
  ulong mb = (ushort)(b >> 8) | 0x8000;
  int e = 260 + _fixed_shift - ( ((uchar)(a >> 23)) + ((uchar)(b >> 23)) );
  ulong p1 = ma * (b & 0xff);
  ulong p = ma * mb;

#define p_bits (sizeof(p) * 8)

    if ((uchar) a) {		/* >16 mantissa bits */
	ulong p2 = (a & 0xff) * mb;

	p += ((((uint) (uchar) a * (uint) (uchar) b) >> 8) + p1 + p2) >> 8;
    } else
	p += p1 >> 8;
    if ((uint) e < p_bits)	/* e = -1 is possible */
	p >>= e;
    else if (e >= p_bits) {	/* also detects a=0 or b=0 */
	*pr = fixed_0;
	return 0;
    } else if (e >= -(p_bits - 1) || p >= 1L << (p_bits - 1 + e))
	return (int)(XWPSError::LimitCheck);
    else
	p <<= -e;
    *pr = ((a ^ b) < 0 ? -p : p);
    return 0;
}

int set_double2fixed_(long * pr, ulong lo, long hi, int fracbits)
{
    long mantissa;
    int shift;

    if (!(hi & 0x7ff00000)) {
	*pr = fixed_0;
	return 0;
    }
    /* We only use 31 bits of mantissa even if sizeof(long) > 4. */
    mantissa = (long) (((hi & 0xfffff) << 10) | (lo >> 22) | 0x40000000);
    shift = ((hi >> 20) & 2047) - (1023 + 30) + fracbits;
    if (shift > 0)
	return (int)(XWPSError::LimitCheck);
    *pr = (shift < -30 ? fixed_0 :
	   hi < 0 ? -(long) (mantissa >> -shift) :	/* truncate */
	   (long) (mantissa >> -shift));
    return 0;
}

#define mbits_float 23
#define mbits_double 20
int set_float2fixed_(long * pr, long vf, int fracbits)
{
    long mantissa;
    int shift;

    if (!(vf & 0x7f800000)) {
	*pr = fixed_0;
	return 0;
    }
    mantissa = (long) ((vf & 0x7fffff) | 0x800000);
    shift = ((vf >> 23) & 255) - (127 + 23) + fracbits;
    if (shift >= 0) {
	if (shift >= sizeof(long) * 8 - 24)
	    return (int)(XWPSError::LimitCheck);
	if (vf < 0)
	    mantissa = -mantissa;
	*pr = (long) (mantissa << shift);
    } else
	*pr = (shift < -24 ? fixed_0 :
	       vf < 0 ? -(long) (mantissa >> -shift) :		/* truncate */
	       (long) (mantissa >> -shift));
    return 0;
}

static const uchar f2f_shifts[] =
{4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};

#define f2f_declare(v, f)\
	ulong v;\
	long f
#define f2f(x, v, f, mbits, fbits)\
	if ( x < 0 )\
	  f = 0xc0000000 + (29 << mbits) - ((long)fbits << mbits), v = -x;\
	else\
	  f = 0x40000000 + (29 << mbits) - ((long)fbits << mbits), v = x;\
	if ( v < 0x8000 )\
	  v <<= 15, f -= 15 << mbits;\
	if ( v < 0x800000 )\
	  v <<= 8, f -= 8 << mbits;\
	if ( v < 0x8000000 )\
	  v <<= 4, f -= 4 << mbits;\
	{ int shift = f2f_shifts[v >> 28];\
	  v <<= shift, f -= shift << mbits;\
	}
	
long fixed2float_(long x, int fracbits)
{
    f2f_declare(v, f);

    if (x == 0)
	return 0;
    f2f(x, v, f, mbits_float, fracbits);
    return f + (((v >> 7) + 1) >> 1);
}

void set_fixed2double_(double *pd, long x, int fracbits)
{
    f2f_declare(v, f);

    if (x == 0) {
	((long *)pd)[1 - arch_is_big_endian] = 0;
	((ulong *) pd)[arch_is_big_endian] = 0;
    } else {
	f2f(x, v, f, mbits_double, fracbits);
	((long *)pd)[1 - arch_is_big_endian] = f + (v >> 11);
	((ulong *) pd)[arch_is_big_endian] = v << 21;
    }
}

const uchar natural_order[DCTSIZE2] =
{
    0, 1, 8, 16, 9, 2, 3, 10,
    17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

const uchar inverse_natural_order[DCTSIZE2] =
{
    0, 1, 5, 6, 14, 15, 27, 28,
    2, 4, 7, 13, 16, 26, 29, 42,
    3, 8, 12, 17, 25, 30, 41, 43,
    9, 11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63
};

bool paramStringEq(PSParamString * pcs, const char *str)
{
	return (strlen(str) == pcs->size && !strncmp(str, (const char *)pcs->data, pcs->size));
}

PSParamString::PSParamString(const PSParamString & other)
{
	size = other.size;
	persistent = true;
	data = 0;
	if (other.data)
	{
		data = new uchar[size];
		persistent = false;
		memcpy((void*)data, other.data, size * sizeof(uchar));
	}
}

PSParamString::PSParamString(const uchar * d, uint s)
{
	data = (uchar*)d;
	size = s;
	persistent = true;
}

PSParamString::~PSParamString()
{
	if (!persistent && data)
		delete [] data;
}

void PSParamString::fromString(const char *str)
{
	if (!persistent && data)
		delete [] data;
		
	data = (uchar*)str;
	size = strlen(str);
	persistent = true;
}

int  PSParamString::packHuffTable(const JHUFF_TBL * table)
{
	if (!persistent && data)
		delete [] data;
		
	int total;
  int i;
  for (i = 1, total = 0; i <= 16; ++i)
		total += table->bits[i];
  data = new uchar[16 + total];
  size = 16 + total;
	persistent = false;
	memcpy(data, table->bits + 1, 16);
  memcpy(data + 16, table->huffval, total);
  return 0;
}

bool PSParamString::pdfKeyEq(const char *str)
{
	return (strlen(str) == size &&  !strncmp(str, (const char *)data, size));
}

int PSParamString::pdfmarkScanInt(int *pvalue)
{
#define max_int_str 20
  char str[max_int_str + 1];
  if (size > max_int_str)
		return (int)(XWPSError::LimitCheck);
  memcpy(str, data, size);
  str[size] = 0;
  return (sscanf(str, "%d", pvalue) == 1 ? 0 : (int)(XWPSError::RangeCheck));
#undef max_int_str
}

int PSParamString::quantParamString(int count, 
	                     const quint16 * pvals,
		   								 float QFactor)
{
	int code = 0;
	if (!persistent && data)
		delete [] data;
		
	data = new uchar[count];
	size = count;
	persistent = false;
	for (int i = 0; i < count; ++i) 
	{
		float val = pvals[jpeg_inverse_order(i)] / QFactor;
		data[i] = (val < 1 ? (code = 1) : val > 255 ? (code = 255) : (uchar) val);
  }
  
  return 0;
}

PSParamString & PSParamString::operator=(const PSParamString & other)
{
	if (!persistent && data)
	{
		delete [] data;
		data = 0;
	}
	
	persistent = true;
	size = other.size;
	if (other.data)
	{
		data = new uchar[other.size];
		persistent = false;
		memcpy((void*)data, (void*)other.data, other.size * sizeof(uchar));
	}
	return *this;
}

PSParamIntArray::PSParamIntArray(const int * d, uint s)
{
	data = (int*)d;
	size = s;
	persistent = true;
}

PSParamIntArray::PSParamIntArray(const PSParamIntArray & other)
{
	size = other.size;
	persistent = true;
	data = 0;
	if (other.data)
	{
		data = new int[size];
		persistent = false;
		memcpy((void*)data, other.data, size * sizeof(int));
	}
}

PSParamIntArray::~PSParamIntArray()
{
	if (!persistent && data)
		delete [] data;
}

PSParamIntArray & PSParamIntArray::operator=(const PSParamIntArray & other)
{
	if (!persistent && data)
	{
		delete [] data;
		data = 0;
	}
		
	persistent = true;
	size = other.size;
	if (other.data)
	{
		data = new int[other.size];
		persistent = false;
		memcpy((void*)data, (void*)other.data, other.size * sizeof(int));
	}
	return *this;
}

PSParamFloatArray::PSParamFloatArray(const float * d, uint s)
{
	data = (float*)d;
	size = s;
	persistent = true;
}

PSParamFloatArray::PSParamFloatArray(const PSParamFloatArray & other)
{
	size = other.size;
	persistent = true;
	data = 0;
	if (other.data)
	{
		data = new float[size];
		persistent = false;
		memcpy((void*)data, other.data, size * sizeof(float));
	}
}

PSParamFloatArray::~PSParamFloatArray()
{
	if (!persistent && data)
		delete [] data;
}

int PSParamFloatArray::quantParamArray(int count, 
	                    const quint16 * pvals,
		  								float QFactor)
{
	if (!persistent && data)
		delete [] data;
		
	data = new float[count];
	size = count;
	persistent = false;
	for (int i = 0; i < count; ++i)
		data[i] = pvals[jpeg_inverse_order(i)] / QFactor;
		
	return 0;
}

PSParamFloatArray & PSParamFloatArray::operator=(const PSParamFloatArray & other)
{
	if (!persistent && data)
	{
		delete [] data;
		data = 0;
	}
	
	persistent = true;
	size = other.size;
	if (other.data)
	{
		data = new float[other.size];
		persistent = false;
		memcpy((void*)data, (void*)other.data, other.size * sizeof(float));
	}
	
	return *this;
}

PSParamStringArray::PSParamStringArray(const PSParamString * d, uint s)
{
	data = (PSParamString*)d;
	size = s;
	persistent = true;
}

PSParamStringArray::PSParamStringArray(const PSParamStringArray & other)
{
	size = other.size;
	persistent = true;
	data = 0;
	if (other.data)
	{
		data = new PSParamString[size];
		persistent = false;
		for (int i = 0; i < size; i++)
			data[i] = other.data[i];
	}
}

PSParamStringArray::~PSParamStringArray()
{
	if (!persistent && data)
		delete [] data;
}

bool PSParamStringArray::embedListIncludes(const uchar *chars, uint sizeA)
{
	for (uint i = 0; i < size; ++i)
		if (!bytes_compare(data[i].data, data[i].size, chars, sizeA))
	    return true;
  return false;
}

PSParamStringArray & PSParamStringArray::operator=(const PSParamStringArray & other)
{
	if (!persistent && data)
	{
		delete [] data;
		data = 0;
	}
	
	persistent = true;
	size = other.size;
	if (other.data)
	{
		data = new PSParamString[other.size];
		persistent = false;
		for (int i = 0; i < other.size; i++)
			data[i] = other.data[i];
	}
	return *this;
}

XWPSUid::XWPSUid()
	:id(0),
	 xvalues(0)
{
	invalid();
}

XWPSUid::~XWPSUid()
{
	if (xvalues)
	{
		delete [] xvalues;
		xvalues=0;
	}
}

bool XWPSUid::equal(const XWPSUid * puid2)
{
	if (id != puid2->id)
		return false;
  if (id >= 0)
		return true;
		
	return !memcmp((const char *)xvalues,	(const char *)puid2->xvalues,	(uint) - (id) * sizeof(long));
}

XWPSStruct::XWPSStruct()
	:refCount(1)
{
}

XWPSStruct::~XWPSStruct()
{
}

int XWPSStruct::checkType(const char * n)
{
	const char * t = getTypeName();
	if (0 == qstrcmp(n, t))
		return 0;
		
	return (int)(XWPSError::TypeCheck);
}

int XWPSStruct::getLength()
{
	return sizeof(XWPSStruct);
}

const char * XWPSStruct::getTypeName()
{
	return "struct";
}

XWPSBytes::XWPSBytes()
	:XWPSStruct(),
	 ptr(0),
	 arr(0),
	 length(0),
	 self(false)
{
}

XWPSBytes::XWPSBytes(ushort size)
	:XWPSStruct()
{
	arr = (uchar*)malloc((size + 1) * sizeof(uchar));
	ptr = arr;
	length = size;
	self = true;
}

XWPSBytes::XWPSBytes(uchar * elt)
	:XWPSStruct(),
	 arr(elt),
	 ptr(elt),
	 length(0),
	 self(false)
{
}

XWPSBytes::~XWPSBytes()
{
	if (arr)
	{
		if (self)
		  free(arr);
		arr = 0;
	}
}

int XWPSBytes::getLength()
{
	return sizeof(XWPSBytes);
}

const char * XWPSBytes::getTypeName()
{
	return "bytes";
}

XWPSBytesRef::XWPSBytesRef()
	:XWPSStruct(),
	 arr(0),
	 ptr(0)
{
}

XWPSBytesRef::XWPSBytesRef(ushort size)
	:XWPSStruct(),
	 arr(0),
	 ptr(0)
{
	arr = new XWPSBytes(size);
	ptr = arr->arr;
}

XWPSBytesRef::XWPSBytesRef(uchar * elt)
	:XWPSStruct(),
	 arr(0),
	 ptr(elt)
{
}

XWPSBytesRef::~XWPSBytesRef()
{
	if (arr)
	{
		if (arr->decRef() == 0)
			delete arr;
		arr = 0;
	}
}

XWPSUints::XWPSUints()
	:XWPSStruct(),
	 refs(0),
	 ptr(0),
	 self(false)
{
}

XWPSUints::XWPSUints(ushort size)
	:XWPSStruct(),
	 refs(0),
	 ptr(0),
	 self(false)
{
	if (size >= 0)
	{
		refs = (uint*)malloc((size + 1)*sizeof(uint));
		ptr = refs;
		self = true;
	}
}

XWPSUints::XWPSUints(uint * d)
	:XWPSStruct(),
	 ptr(d),
	 self(false)
{
	refs = d;
}

XWPSUints::~XWPSUints()
{
	if (refs)
	{
		if (self)
			free(refs);
		
		refs = 0;
	}
}

int XWPSUints::getLength()
{
	return sizeof(XWPSUints);
}

const char * XWPSUints::getTypeName()
{
	return "uint";
}

XWPSString::~XWPSString()
{
	if (bytes)
	{
		if (bytes->decRef() == 0)
			delete bytes;
		bytes = 0;
		data = 0;
	}
	
	if (data)
		data = 0;
}

void XWPSSinCos::sincosDegrees(double ang)
{
	double quot = ang / 90;

  if (floor(quot) == quot) 
  {
		int quads = (int)fmod(quot, 4.0) & 3;

		_sin = isincos[quads];
		_cos = isincos[quads + 1];
		orthogonal = true;
  } 
  else 
  {
		double arad = ang * (M_PI / 180);

		_sin = sin(arad);
		_cos = cos(arad);
		orthogonal = false;
  }
}

int XWPSPoint::distanceTransform(double dx, double dy, XWPSMatrix * pmat)
{
	x = dx * pmat->xx;
  y = dy * pmat->yy;
  if (!is_fzero(pmat->yx))
		x += dy * pmat->yx;
  if (!is_fzero(pmat->xy))
		y += dx * pmat->xy;
  return 0;
}

int XWPSPoint::distanceTransformInverse(double dx, double dy, XWPSMatrix * pmat)
{
	if (pmat->isXXYY()) 
	{
		if (is_fzero(pmat->xx) || is_fzero(pmat->yy))
	    return (int)(XWPSError::UndefinedResult);
		x = dx / pmat->xx;
		y = dy / pmat->yy;
  } else if (pmat->isXYYX()) 
  {
		if (is_fzero(pmat->xy) || is_fzero(pmat->yx))
	    return (int)(XWPSError::UndefinedResult);
		x = dy / pmat->xy;
		y = dx / pmat->yx;
  } 
  else 
  {
		double det = pmat->xx * pmat->yy - pmat->xy * pmat->yx;
		if (det == 0)
	    return (int)(XWPSError::UndefinedResult);
		x = (dx * pmat->yy - dy * pmat->yx) / det;
		y = (dy * pmat->xx - dx * pmat->xy) / det;
  }
  return 0;
}

int XWPSPoint::setTextDistance(XWPSPoint *ppt, XWPSMatrix *pmat)
{
	double rounded;

  distanceTransformInverse(pmat->tx - ppt->x, pmat->ty - ppt->y, pmat);
  if (fabs(x - (rounded = floor(x + 0.5))) < 0.0005)
		x = rounded;
  if (fabs(y - (rounded = floor(y + 0.5))) < 0.0005)
		y = rounded;
  return 0;
}

int XWPSPoint::transform(double xA, double yA, XWPSMatrix * pmat)
{
	x = xA * pmat->xx + pmat->tx;
  y = yA * pmat->yy + pmat->ty;
  if (!is_fzero(pmat->yx))
		x += yA * pmat->yx;
  if (!is_fzero(pmat->xy))
		y += xA * pmat->xy;
  return 0;
}

int XWPSPoint::transform(double xA, double yA, XWPSMatrixFixed * pmat)
{
	x = xA * pmat->xx + pmat->tx;
  y = yA * pmat->yy + pmat->ty;
  if (!is_fzero(pmat->yx))
		x += yA * pmat->yx;
  if (!is_fzero(pmat->xy))
		y += xA * pmat->xy;
  return 0;
}

int XWPSPoint::transformInverse(double xA, double yA, XWPSMatrix * pmat)
{
	if (pmat->isXXYY()) 
	{
		if (is_fzero(pmat->xx) || is_fzero(pmat->yy))
	    return (int)(XWPSError::UndefinedResult);
		x = (xA - pmat->tx) / pmat->xx;
		y = (yA - pmat->ty) / pmat->yy;
		return 0;
  } 
  else if (pmat->isXYYX()) 
  {
		if (is_fzero(pmat->xy) || is_fzero(pmat->yx))
	    return (int)(XWPSError::UndefinedResult);
		x = (yA - pmat->ty) / pmat->xy;
		y = (xA - pmat->tx) / pmat->yx;
		return 0;
  } 
  else 
  {
		XWPSMatrix imat;
		int code = imat.matrixInvert(pmat);

		if (code < 0)
	    return code;
		return transform(xA, yA, &imat);
  }
}

int XWPSRect::bboxTransform(XWPSMatrix * pmat, XWPSRect * pbox_out)
{
	return bboxTransformEither(pmat, pbox_out, &XWPSPoint::transform);
}

int XWPSRect::bboxTransformEither(XWPSMatrix * pmat,
		      								        XWPSRect * pbox_out,
     											        int (XWPSPoint::*point_xform) (double, double, XWPSMatrix *))
{
	int code;
	XWPSPoint pts[4];

  if ((code = bboxTransformEitherOnly(pmat, pts, point_xform)) < 0)
		return code;
  return pbox_out->pointsBbox(pts);
}

int XWPSRect::bboxTransformEitherOnly(XWPSMatrix * pmat,
			   											        XWPSPoint pts[4],
     													        int (XWPSPoint::*point_xform)(double, double, XWPSMatrix *))
{
	int code;
  if ((code = (pts[0].*point_xform)(p.x, p.y, pmat)) < 0 ||
				(code = (pts[1].*point_xform)(p.x, q.y, pmat)) < 0 ||
				(code = (pts[2].*point_xform)(q.x, p.y, pmat)) < 0 ||
     		(code = (pts[3].*point_xform)(q.x, q.y, pmat)) < 0	)
		;
  return code;
}

int XWPSRect::bboxTransformInverse(XWPSRect * pbox_in, XWPSMatrix * pmat)
{
	return pbox_in->bboxTransformEither(pmat, this, &XWPSPoint::distanceTransformInverse);
}

int XWPSRect::bboxTransformOnly(XWPSMatrix * pmat, XWPSPoint points[4])
{
	return bboxTransformEitherOnly(pmat, points, &XWPSPoint::transform);
}

void XWPSRect::merge(XWPSIntRect & from)
{
	if (from.p.x < p.x) 
		p.x = from.p.x;
		
  if (from.q.x > q.x) 
  	q.x = from.q.x;
  	
  if (from.p.y < p.y) 
  	p.y = from.p.y;
  	
  if (from.q.y > q.y) 
  	q.y = from.q.y;
}

void XWPSRect::merge(XWPSRect & from)
{
	if (from.p.x < p.x) 
		p.x = from.p.x;
		
  if (from.q.x > q.x) 
  	q.x = from.q.x;
  	
  if (from.p.y < p.y) 
  	p.y = from.p.y;
  	
  if (from.q.y > q.y) 
  	q.y = from.q.y;
}

int XWPSRect::pointsBbox(XWPSPoint pts[4])
{
#define assign_min_max(vmin, vmax, v0, v1)\
  if ( v0 < v1 ) vmin = v0, vmax = v1; else vmin = v1, vmax = v0
#define assign_min_max_4(vmin, vmax, v0, v1, v2, v3)\
  { double min01, max01, min23, max23;\
    assign_min_max(min01, max01, v0, v1);\
    assign_min_max(min23, max23, v2, v3);\
    vmin = qMin(min01, min23);\
    vmax = qMax(max01, max23);\
  }
  
  assign_min_max_4(p.x, q.x, pts[0].x, pts[1].x, pts[2].x, pts[3].x);
  assign_min_max_4(p.y, q.y, pts[0].y, pts[1].y, pts[2].y, pts[3].y);
#undef assign_min_max
#undef assign_min_max_4
  return 0;
}

void XWPSIntRect::merge(XWPSIntRect & from)
{
	if (from.p.x < p.x) 
		p.x = from.p.x;
		
  if (from.q.x > q.x) 
  	q.x = from.q.x;
  	
  if (from.p.y < p.y) 
  	p.y = from.p.y;
  	
  if (from.q.y > q.y) 
  	q.y = from.q.y;
}

void XWPSIntRect::merge(XWPSRect & from)
{
	if (from.p.x < p.x) 
		p.x = from.p.x;
		
  if (from.q.x > q.x) 
  	q.x = from.q.x;
  	
  if (from.p.y < p.y) 
  	p.y = from.p.y;
  	
  if (from.q.y > q.y) 
  	q.y = from.q.y;
}

void XWPSFixedPoint::adjustIfEmpty(XWPSFixedRect * pbox)
{
	const long  dx = pbox->q.x - pbox->p.x, dy = pbox->q.y - pbox->p.y;
  if (dx < fixed_half && dx > 0 && (dy >= int2fixed(2) || dy < 0)) 
  {
		x = arith_rshift_1(fixed_1 + fixed_epsilon - dx);
  } 
  else if (dy < fixed_half && dy > 0 && (dx >= int2fixed(2) || dx < 0)) 
  {
		y = arith_rshift_1(fixed_1 + fixed_epsilon - dy);
  }
}

int XWPSFixedPoint::distanceTransform2fixed(XWPSMatrixFixed * pmat, double dx, double dy)
{
	long px, py, t;
  double xtemp, ytemp;
  int code;

  if ((code = CHECK_DFMUL2FIXED_VARS(px, dx, pmat->xx, xtemp)) < 0 ||
				(code = CHECK_DFMUL2FIXED_VARS(py, dy, pmat->yy, ytemp)) < 0)
		return code;
  FINISH_DFMUL2FIXED_VARS(px, xtemp);
  FINISH_DFMUL2FIXED_VARS(py, ytemp);
  if (!is_fzero(pmat->yx)) 
  {
		if ((code = CHECK_DFMUL2FIXED_VARS(t, dy, pmat->yx, ytemp)) < 0)
	    return code;
		FINISH_DFMUL2FIXED_VARS(t, ytemp);
		px += t;
  }
  if (!is_fzero(pmat->xy)) 
  {
		if ((code = CHECK_DFMUL2FIXED_VARS(t, dx, pmat->xy, xtemp)) < 0)
	    return code;
		FINISH_DFMUL2FIXED_VARS(t, xtemp);
		py += t;
  }
  x = px;
  y = py;
    return 0;
}

void XWPSFixedPoint::scaleExp2(int sx, int sy)
{
	if (sx >= 0)
		x <<= sx;
  else
		x >>= -sx;
  if (sy >= 0)
		y <<= sy;
  else
		y >>= -sy;
}

int XWPSFixedPoint::transform2fixed(XWPSMatrixFixed * pmat, double xA, double yA)
{
	long px, py, t;
  double xtemp, ytemp;
  int code;
  
  if (!pmat->txy_fixed_valid)
  {
  	XWPSPoint fpt;

		fpt.transform(xA, yA, pmat);
		if (!(f_fits_in_fixed(fpt.x) && f_fits_in_fixed(fpt.y)))
	    return (int)(XWPSError::LimitCheck);
	    	
	  x = float2fixed(fpt.x);
		y = float2fixed(fpt.y);
		return 0;
  }
  
  if (!is_fzero(pmat->xy))
  {
  	if ((code = CHECK_DFMUL2FIXED_VARS(px, yA, pmat->yx, xtemp)) < 0 ||
	    	(code = CHECK_DFMUL2FIXED_VARS(py, xA, pmat->xy, ytemp)) < 0)
	    return code;
	    
	  FINISH_DFMUL2FIXED_VARS(px, xtemp);
		FINISH_DFMUL2FIXED_VARS(py, ytemp);
		if (!is_fzero(pmat->xx)) 
		{
			if ((code = CHECK_DFMUL2FIXED_VARS(t, xA, pmat->xx, xtemp)) < 0)
				return code;
	    FINISH_DFMUL2FIXED_VARS(t, xtemp);
	    px += t;
		}
		
		if (!is_fzero(pmat->yy))
		{
			if ((code = CHECK_DFMUL2FIXED_VARS(t, yA, pmat->yy, ytemp)) < 0)
				return code;
	    FINISH_DFMUL2FIXED_VARS(t, ytemp);
	    py += t;
		}
  }
  else
  {
  	if ((code = CHECK_DFMUL2FIXED_VARS(px, xA, pmat->xx, xtemp)) < 0 ||
	    	(code = CHECK_DFMUL2FIXED_VARS(py, yA, pmat->yy, ytemp)) < 0)
	    return code;
		FINISH_DFMUL2FIXED_VARS(px, xtemp);
		FINISH_DFMUL2FIXED_VARS(py, ytemp);
		if (!is_fzero(pmat->yx)) 
		{
	    if ((code = CHECK_DFMUL2FIXED_VARS(t, yA, pmat->yx, ytemp)) < 0)
				return code;
	    FINISH_DFMUL2FIXED_VARS(t, ytemp);
	    px += t;
		}
  }
  
  x = px + pmat->tx_fixed;
  y = py + pmat->ty_fixed;
  return 0;
}

void XWPSFixedRect::intersect(XWPSFixedRect & from)
{
	if (from.p.x < p.x) 
		p.x = from.p.x;
	if (from.q.x > q.x) 
		q.x = from.q.x;
	if (from.p.y < p.y) 
		p.y = from.p.y;
	if (from.q.y > q.y) 
		q.y = from.q.y;
}

void XWPSFixedRect::scaleExp2(int sx, int sy)
{
	p.scaleExp2(sx, sy);
  q.scaleExp2(sx, sy);
}

int  XWPSFixedRect::shadeBboxTransform2fixed(XWPSRect * rect, XWPSImagerState * pis)
{
	XWPSRect dev_rect;
	XWPSMatrix ctm;
	pis->currentMatrix(&ctm);
  int code = rect->bboxTransform(&ctm, &dev_rect);

  if (code >= 0) 
  {
		p.x = float2fixed(dev_rect.p.x);
		p.y = float2fixed(dev_rect.p.y);
		q.x = float2fixed(dev_rect.q.x);
		q.y = float2fixed(dev_rect.q.y);
  }
  return code;
}

XWPSLocalRects::XWPSLocalRects()
{
	pr = 0;
	count = 0;
}

XWPSLocalRects::~XWPSLocalRects()
{
	if (pr != rl)
	{
		if (pr)
		{
			delete [] pr;
			pr = 0;
		}
	}
}

XWPSMatrix::XWPSMatrix()
{
	reset();
}

void XWPSMatrix::makeBitmapMatrix(int x, 
	                      int y, 
	                      int w, 
	                      int h, 
	                      int h_actual)
{
	xx = w;
  xy = 0;
  yx = 0;
  yy = -h_actual;
  tx = x;
  ty = y + h;
}

int XWPSMatrix::makeRotation(double ang)
{
	XWPSSinCos sincos;

  sincos.sincosDegrees(ang);
  yy = xx = sincos._cos;
  xy = sincos._sin;
  yx = -sincos._sin;
  tx = ty = 0.0;
  return 0;
}

int XWPSMatrix::makeScaling(double sx, double sy)
{
	xx = sx;
	yy = sy;
	return 0;
}

int XWPSMatrix::matrixTranslate(XWPSMatrix * pm, double dx, double dy)
{
	XWPSPoint trans;
  int code = trans.distanceTransform(dx, dy, pm);

  if (code < 0)
		return code;
  if (pm != this)
		*this = *pm;
    
  tx += trans.x;
  ty += trans.y;
  return 0;
}

int XWPSMatrix::makeTranslation(double dx, double dy)
{
	tx = dx;
  ty = dy;
  return 0;
}

int XWPSMatrix::matrixInvert(XWPSMatrix * pm)
{
	if (pm->isXXYY()) 
	{
		if (is_fzero(pm->xx) || is_fzero(pm->yy))
	    return (int)(XWPSError::UndefinedResult);
		tx = -(xx = 1.0 / pm->xx) * pm->tx;
		xy = 0.0;
		yx = 0.0;
		ty = -(yy = 1.0 / pm->yy) * pm->ty;
  } 
  else 
  {
		double det = pm->xx * pm->yy - pm->xy * pm->yx;
		double mxx = pm->xx, mtx = pm->tx;

		if (det == 0)
	    return (int)(XWPSError::UndefinedResult);
	    	
		xx = pm->yy / det;
		xy = -pm->xy / det;
		yx = -pm->yx / det;
		yy = mxx / det;
		tx = -(mtx * xx + pm->ty * yx);
		ty = -(mtx * xy + pm->ty * yy);
  }
  return 0;
}

int XWPSMatrix::matrixMultiply(XWPSMatrix * pm1, XWPSMatrix * pm2)
{
	double xx1 = pm1->xx, yy1 = pm1->yy;
  double tx1 = pm1->tx, ty1 = pm1->ty;
  double xx2 = pm2->xx, yy2 = pm2->yy;
  double xy2 = pm2->xy, yx2 = pm2->yx;

  if (pm1->isXXYY()) 
  {
		tx = tx1 * xx2 + pm2->tx;
		ty = ty1 * yy2 + pm2->ty;
		if (is_fzero(xy2))
	    xy = 0;
		else
	    xy = xx1 * xy2,
		ty += tx1 * xy2;
		xx = xx1 * xx2;
		if (is_fzero(yx2))
	    yx = 0;
		else
	    yx = yy1 * yx2,
		tx += ty1 * yx2;
		yy = yy1 * yy2;
 	} 
 	else 
 	{
		double xy1 = pm1->xy, yx1 = pm1->yx;
		xx = xx1 * xx2 + xy1 * yx2;
		xy = xx1 * xy2 + xy1 * yy2;
		yy = yx1 * xy2 + yy1 * yy2;
		yx = yx1 * xx2 + yy1 * yx2;
		tx = tx1 * xx2 + ty1 * yx2 + pm2->tx;
		ty = tx1 * xy2 + ty1 * yy2 + pm2->ty;
  }
  return 0;
}

int XWPSMatrix::matrixRotate(XWPSMatrix * pm, double ang)
{
	double mxx, mxy;
  XWPSSinCos sincos;

  sincos.sincosDegrees(ang);
  mxx = pm->xx, mxy = pm->xy;
  xx = sincos._cos * mxx + sincos._sin * pm->yx;
  xy = sincos._cos * mxy + sincos._sin * pm->yy;
  yx = sincos._cos * pm->yx - sincos._sin * mxx;
  yy = sincos._cos * pm->yy - sincos._sin * mxy;
  if (this != pm) 
  {
		tx = pm->tx;
		ty = pm->ty;
  }
  return 0;
}

int XWPSMatrix::matrixScale(XWPSMatrix * pm, double sx, double sy)
{
	xx = pm->xx * sx;
  xy = pm->xy * sx;
  yx = pm->yx * sy;
  yy = pm->yy * sy;
  if (this != pm) 
  {
		tx = pm->tx;
		ty = pm->ty;
  }
  return 0;
}

int XWPSMatrix::matrixToFixedCoeff(XWPSFixedCoeff * pfc, int max_bits)
{
	XWPSMatrix ctm;
  int scale = -10000;
  int expt, shift;

  ctm = *this;
  pfc->skewed = 0;
  if (!is_fzero(ctm.xx)) 
  {
		(frexp(ctm.xx, &scale));
  }
    
  if (!is_fzero(ctm.xy)) 
  {
		(frexp(ctm.xy, &expt));
		if (expt > scale)
	    scale = expt;
		pfc->skewed = 1;
  }
  
  if (!is_fzero(ctm.yx)) 
  {
		(frexp(ctm.yx, &expt));
		if (expt > scale)
	    scale = expt;
		pfc->skewed = 1;
  }
  
  if (!is_fzero(ctm.yy)) 
  {
		(frexp(ctm.yy, &expt));
		if (expt > scale)
	    scale = expt;
  }
  
  if (max_bits < fixed_fraction_bits)
		max_bits = fixed_fraction_bits;
    
  scale = sizeof(long) * 8 - 1 - max_bits - scale;

  shift = scale - _fixed_shift;
  if (shift > 0) 
  {
		pfc->shift = shift;
		pfc->round = (long) 1 << (shift - 1);
  } 
  else 
  {
		pfc->shift = 0;
		pfc->round = 0;
		scale -= shift;
  }
#define SET_C(c)\
  if ( is_fzero(ctm.c) ) pfc->c = 0;\
  else pfc->c = (long)ldexp(ctm.c, scale)
    
  SET_C(xx);
  SET_C(xy);
  SET_C(yx);
  SET_C(yy);
#undef SET_C

  pfc->max_bits = max_bits;
  return 0;
}

void XWPSMatrix::reset()
{
	xx = 1.0;
	xy = 0.0;
	yx = 0.0;
	yy = 1.0;
	tx = 0;
	ty = 0;
}

XWPSMatrix & XWPSMatrix::operator=(XWPSMatrix & other)
{
	xx = other.xx;
	xy = other.xy;
	yx = other.yx;
	yy = other.yy;
	tx = other.tx;
	ty = other.ty;
	return *this;
}

XWPSMatrix & XWPSMatrix::operator=(XWPSMatrixFixed & other)
{
	xx = other.xx;
	xy = other.xy;
	yx = other.yx;
	yy = other.yy;
	tx = other.tx;
	ty = other.ty;
	return *this;
}

XWPSMatrixFixed::XWPSMatrixFixed()
{
	reset();
}

bool XWPSMatrixFixed::isSkewed()
{
	return (!(isXXYY() || isXYYX()));
}

int XWPSMatrixFixed::fromMatrix(XWPSMatrix *pmat)
{
	xx = pmat->xx;
	xy = pmat->xy;
	yx = pmat->yx;
	yy = pmat->yy;
	tx = pmat->tx;
	ty = pmat->ty;
  if (f_fits_in_fixed(pmat->tx) && f_fits_in_fixed(pmat->ty)) 
  {
		tx = fixed2float(tx_fixed = float2fixed(pmat->tx));
		ty = fixed2float(ty_fixed = float2fixed(pmat->ty));
		txy_fixed_valid = true;
  } 
  else 
  {
		txy_fixed_valid = false;
  }
  return 0;
}

int XWPSMatrixFixed::matrixMultiply(XWPSMatrix * pm1, XWPSMatrixFixed * pm2)
{
	double xx1 = pm1->xx, yy1 = pm1->yy;
  double tx1 = pm1->tx, ty1 = pm1->ty;
  double xx2 = pm2->xx, yy2 = pm2->yy;
  double xy2 = pm2->xy, yx2 = pm2->yx;

  if (pm1->isXXYY()) 
  {
		tx = tx1 * xx2 + pm2->tx;
		ty = ty1 * yy2 + pm2->ty;
		if (is_fzero(xy2))
	    xy = 0;
		else
	    xy = xx1 * xy2,
		ty += tx1 * xy2;
		xx = xx1 * xx2;
		if (is_fzero(yx2))
	    yx = 0;
		else
	    yx = yy1 * yx2,
		tx += ty1 * yx2;
		yy = yy1 * yy2;
 	} 
 	else 
 	{
		double xy1 = pm1->xy, yx1 = pm1->yx;
		xx = xx1 * xx2 + xy1 * yx2;
		xy = xx1 * xy2 + xy1 * yy2;
		yy = yx1 * xy2 + yy1 * yy2;
		yx = yx1 * xx2 + yy1 * yx2;
		tx = tx1 * xx2 + ty1 * yx2 + pm2->tx;
		ty = tx1 * xy2 + ty1 * yy2 + pm2->ty;
  }
  return 0;
}

void XWPSMatrixFixed::reset()
{
	xx = 1.0;
	xy = 0.0;
	yx = 0.0;
	yy = 1.0;
	tx = 0;
	ty = 0;
	tx_fixed = 0;
	ty_fixed = 0;
	txy_fixed_valid = false;
}

int XWPSMatrixFixed::updateMatrixFixed(double xt, double yt)
{
	int code = 0;
	if (f_fits_in_fixed(xt) && f_fits_in_fixed(yt))
	{
		tx = xt;
		code = set_float2fixed_vars(tx_fixed, tx);
		ty = yt;
		code = set_float2fixed_vars(ty_fixed, ty);
		txy_fixed_valid = true;
	}
	else
	{
		tx = xt;
		ty = yt;
		txy_fixed_valid = false;
	}
	
	return code;
}

XWPSMatrixFixed & XWPSMatrixFixed::operator=(XWPSMatrixFixed & other)
{
	xx = other.xx;
	xy = other.xy;
	yx = other.yx;
	yy = other.yy;
	tx = other.tx;
	ty = other.ty;
	tx_fixed = other.tx_fixed;
	ty_fixed = other.ty_fixed;
	txy_fixed_valid = other.txy_fixed_valid;
	return *this;
}

XWPSMatrixFixed & XWPSMatrixFixed::operator=(XWPSMatrix & other)
{
	xx = other.xx;
	xy = other.xy;
	yx = other.yx;
	yy = other.yy;
	tx = other.tx;
	ty = other.ty;
	return *this;
}

XWPSFixedCoeff::XWPSFixedCoeff()
{
	reset();
}

long XWPSFixedCoeff::mfixedXX(long v, int maxb)
{
	if (((v) + (fixed_1 << (maxb - 1))) & ((-fixed_1 << maxb) | _fixed_fraction_v))
		return mult(v, xx, maxb);
		
	return arith_rshift(fixed2int_var(v) * xx + round, shift);
}

long XWPSFixedCoeff::mfixedXY(long v, int maxb)
{
	if (((v) + (fixed_1 << (maxb - 1))) & ((-fixed_1 << maxb) | _fixed_fraction_v))
		return mult(v, xy, maxb);
		
	return arith_rshift(fixed2int_var(v) * xy + round, shift);
}

long XWPSFixedCoeff::mfixedYX(long v, int maxb)
{
	if (((v) + (fixed_1 << (maxb - 1))) & ((-fixed_1 << maxb) | _fixed_fraction_v))
		return mult(v, yx, maxb);
		
	return arith_rshift(fixed2int_var(v) * yx + round, shift);
}

long XWPSFixedCoeff::mfixedYY(long v, int maxb)
{
	if (((v) + (fixed_1 << (maxb - 1))) & ((-fixed_1 << maxb) | _fixed_fraction_v))
		return mult(v, yy, maxb);
		
	return arith_rshift(fixed2int_var(v) * yy + round, shift);
}

long XWPSFixedCoeff::mult(long value, long coeff, int maxb)
{
  if ((value + (fixed_1 << (maxb - 1))) & (-fixed_1 << maxb)) 
  {
		return  (coeff < 0 ?   -fixed_mult_quo(value, -coeff, fixed_1 << shift) :
	     			fixed_mult_quo(value, coeff, fixed_1 << shift));
  } 
  else 
  {
  	return (long) arith_rshift(fixed2int_var(value) * coeff + fixed2int(fixed_fraction(value) * coeff)
			 			+ round, shift);
  }
}

void XWPSFixedCoeff::reset()
{
	xx = 1; 
	xy = 0;
	yx = 0; 
	yy = 1;
  skewed = 0;
  shift = 0;
  max_bits = 0;
  round = 0;
}

XWPSFixedCoeff & XWPSFixedCoeff::operator=(XWPSFixedCoeff & other)
{
	xx = other.xx;
	xy = other.xy;
	yx = other.yx;
	yy = other.yy;
	skewed = other.skewed;
	shift = other.shift;
	max_bits = other.max_bits;
	round = other.round;
	return *this;
}

XWPSDashParams::XWPSDashParams()
{
	reset();
}

XWPSDashParams::~XWPSDashParams()
{
	if (pattern)
	{
		delete [] pattern;
		pattern = 0;
	}
}

void XWPSDashParams::copy(XWPSDashParams & from, bool cp)
{
	init_ink_on = from.init_ink_on;
	init_index = from.init_index;
	init_dist_left = from.init_dist_left;
	
	if (cp)
	{
		pattern_size = from.pattern_size;
		offset = from.offset;
		adapt = from.adapt;
		pattern_length = from.pattern_length;
		if (pattern)
		{
			delete [] pattern;
			pattern = 0;
		}
	
		if (from.pattern)
		{
			pattern = new float[pattern_size];
			memcpy(pattern, from.pattern, pattern_size * sizeof(float));
		}
	}
}

bool XWPSDashParams::dashPatternEq(const float *stored, float scale)
{
	for (uint i = 0; i < pattern_size; ++i)
		if (stored[i] != (float)(pattern[i] * scale))
	    return false;
  return true;
}

void XWPSDashParams::reset()
{
	pattern = 0;
	pattern_size = 0;
	offset = 0.0;
	adapt = false;
	pattern_length = 0.0;
	init_ink_on = true;
	init_index = 0;
	init_dist_left = 0; 
}

int XWPSDashParams::setDash(const float *patternA, uint lengthA, float offsetA)
{
	uint n = lengthA;
  const float *dfromA = patternA;
  bool inkA = true;
  int indexA = 0;
  float pattern_lengthA = 0.0;
  float dist_leftA;
  float *ppatA = pattern;
  
  while (n--) 
  {
		float eltA = *dfromA++;

		if (eltA < 0)
	    return (int)(XWPSError::RangeCheck);
		pattern_lengthA += eltA;
  }
  
  if (lengthA == 0) 
  {
		dist_leftA = 0.0;
		if (pattern) 
		{
	    delete [] pattern;
	    pattern = 0;
	    ppatA = 0;
		}
  }
  else
  {
  	uint size = lengthA * sizeof(float);

		if (pattern_lengthA == 0)
	    return (int)(XWPSError::RangeCheck);
	    	
#define f_mod(a, b) ((a) - floor((a) / (b)) * (b))
		if (lengthA & 1) 
		{
	    float length2 = pattern_lengthA * 2;

	    dist_leftA = f_mod(offsetA, length2);
	    if (dist_leftA >= pattern_lengthA)
				dist_leftA -= pattern_lengthA, inkA = !inkA;
		} 
		else
	    dist_leftA = f_mod(offsetA, pattern_lengthA);
		while ((dist_leftA -= patternA[indexA]) >= 0 && (dist_leftA > 0 || patternA[indexA] != 0))
	    inkA = !inkA, indexA++;
	    
	  ppatA = new float[size];
	  memcpy(ppatA, patternA, lengthA * sizeof(float));
  }
  
#undef f_mod
  pattern = ppatA;
  pattern_size = lengthA;
  offset = offsetA;
  pattern_length = pattern_lengthA;
  init_ink_on = inkA;
  init_index = indexA;
  init_dist_left = -dist_leftA;
  return 0;
}

XWPSLineParams::XWPSLineParams()
{
	reset();
}

void XWPSLineParams::copy(XWPSLineParams & from, bool cp)
{
	half_width = from.half_width;
	cap = from.cap;
	join = from.join;
	curve_join = from.curve_join;
	miter_limit = from.miter_limit;
	miter_check = from.miter_check;
	dot_length = from.dot_length;
	dot_length_absolute = from.dot_length_absolute;
	dot_orientation = from.dot_orientation;
	dash.copy(from.dash, cp);
}

void XWPSLineParams::reset()
{
	half_width = 0.0;
	cap = ps_cap_butt;
	join = ps_join_miter;
	curve_join = ps_join_bevel;
	miter_limit = 10.0;
	miter_check = 0.20305866;
	dot_length = 0.0;
	dot_length_absolute = false;
}

int XWPSLineParams::setDash(const float *patternA, uint lengthA, float offsetA)
{
	return dash.setDash(patternA, lengthA, offsetA);
}

void XWPSLineParams::setDashAdapt(bool a)
{
	dash.setAdapt(a);
}

int  XWPSLineParams::setDotLength(float length, bool absolute)
{
	if (length < 0)
		return (int)(XWPSError::RangeCheck);
  dot_length = length;
  dot_length_absolute = absolute;
  return 0;
}

int  XWPSLineParams::setMiterLimit(float limit)
{
	if (limit < 1.0)
		return (int)(XWPSError::RangeCheck);
  miter_limit = limit;
  {
		double limit_squared = limit * limit;

		if (limit_squared < 2.0001 && limit_squared > 1.9999)
	    miter_check = 1.0e6;
		else
	    miter_check =	sqrt(limit_squared - 1) * 2 / (limit_squared - 2);
  }
  return 0;
}

bool XWPSStringMatchParams::stringMatch(const uchar * str, uint len, const uchar * pstr, uint plen)
{
	const uchar *pback = 0;
  const uchar *spback = 0;
  const uchar *p = pstr, *pend = pstr + plen;
  const uchar *sp = str, *spend = str + len;
  
again:
	while (p < pend)
	{
		uchar ch = *p;
		if (ch == any_substring) 
		{
	    pback = ++p, spback = sp;
	    continue;
		} 
		else if (ch == any_char) 
		{
	    if (sp == spend)
				return false;
	    p++, sp++;
	    continue;
		} 
		else if (ch == quote_next) 
		{
	    if (++p == pend)
				return true;
	    ch = *p;
		}
		if (sp == spend)
	    return false;
		if (*sp == ch ||
	    (ignore_case && (*sp ^ ch) == 0x20 &&
	     (ch &= ~0x20) >= 0x41 && ch <= 0x5a))
	    p++, sp++;
		else if (pback == 0)
	    return false;
		else 
		{
	    sp = ++spback;
	    p = pback;
		}
	}
	
	if (sp < spend) 
	{
		if (pback == 0)
	    return false;
		p = pback;
		pback = 0;
		sp = spend - (pend - p);
		goto again;
  }
  return true;
}

int s_hex_process(PSStreamCursorRead * pr, 
                  PSStreamCursorWrite * pw,
	                int *odd_digit, 
	                PSHexSyntax syntax)
{
	uchar *p = pr->ptr;
  uchar *rlimit = pr->limit;
  uchar *q = pw->ptr;
  uchar *wlimit = pw->limit;
  uchar *q0 = q;
  uchar val1 = (uchar) * odd_digit;
  uchar val2;
  uint rcount;
  uchar *flimit;
  const uchar *const decoder = scan_char_decoder;
  int code = 0;

  if (q >= wlimit)
		return 1;
  if (val1 <= 0xf)
		goto d2;
		
d1:
	if ((rcount = (rlimit - p) >> 1) == 0)
		goto x1;
		
  flimit = (rcount < wlimit - q ? q + rcount : wlimit);
  
f1:
	if ((val1 = decoder[p[1]]) <= 0xf &&	(val2 = decoder[p[2]]) <= 0xf) 
	{
		p += 2;
		*++q = (val1 << 4) + val2;
		if (q < flimit)
	    goto f1;
		if (q >= wlimit)
	    goto px;
  }
  
x1:
	if (p >= rlimit)
		goto end1;
  
  if ((val1 = decoder[*++p]) > 0xf) 
  {
		if (val1 == ctype_space) 
		{
	    switch (syntax) 
	    {
				case hex_ignore_whitespace:
		    	goto x1;
		    	
				case hex_ignore_leading_whitespace:
		    	if (q == q0 && *odd_digit < 0)
						goto x1;
		    	--p;
		    	code = 1;
		    	goto end1;
		    	
				case hex_ignore_garbage:
		    	goto x1;
	    }
		} 
		else if (syntax == hex_ignore_garbage)
	    goto x1;
		code = ERRC;
		goto end1;
  }
  
d2:
	if (p >= rlimit) 
	{
		*odd_digit = val1;
		goto ended;
  }
  
  if ((val2 = decoder[*++p]) > 0xf) 
  {
		if (val2 == ctype_space)
	    switch (syntax) 
	    {
				case hex_ignore_whitespace:
		    	goto d2;
		    	
				case hex_ignore_leading_whitespace:
		    	if (q == q0)
						goto d2;
		    	--p;
		    	*odd_digit = val1;
		    	code = 1;
		    	goto ended;
		    
				case hex_ignore_garbage:	/* pacify compilers */
		    	;
	    }
		if (syntax == hex_ignore_garbage)
	    goto d2;
		*odd_digit = val1;
		code = ERRC;
		goto ended;
  }
  
  *++q = (val1 << 4) + val2;
  if (q < wlimit)
		goto d1;
px:
	code = 1;
  
end1:
	*odd_digit = -1;

ended:
	pr->ptr = p;
  pw->ptr = q;
  return code;
}

int stream_move(PSStreamCursorRead * pr, PSStreamCursorWrite * pw)
{
	uint rcount = pr->limit - pr->ptr;
  uint wcount = pw->limit - pw->ptr;
  uint count;
  int status;

  if (rcount <= wcount)
		count = rcount, status = 0;
  else
		count = wcount, status = 1;
  memmove(pw->ptr + 1, pr->ptr + 1, count);
  pr->ptr += count;
  pw->ptr += count;
  return status;
}

XWPSStreamCursor::XWPSStreamCursor()
{
	r.ptr = 0;
	r.limit = 0;
	r._skip = 0;
}

XWPSGetBitsParams::XWPSGetBitsParams()
{
	options = 0;
	for (int i = 0; i < 32; i++)
	  data[i] = 0;
	  
	x_offset = 0;
	raster = 0;
}

long XWPSFixedEdge::x_at_y(long y)
{
	return fixed_mult_quo(end.x - start.x, y - start.y, end.y - start.y) + start.x;
}

XWPSPDFFontDescriptorValues::XWPSPDFFontDescriptorValues()
{
	FontType = ft_composite;
	Ascent = 0; 
	CapHeight = 0; 
	Descent = 0; 
	ItalicAngle = 0; 
	StemV = 0;
	Flags = 0;
	AvgWidth = 0; 
	Leading = 0; 
	MaxWidth = 0; 
	MissingWidth = 0; 
	StemH = 0; 
	XHeight = 0;
}
