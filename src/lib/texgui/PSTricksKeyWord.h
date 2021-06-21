/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef PSTRICKSKEYWORD_H
#define PSTRICKSKEYWORD_H

#define PST_MIN_KEYWORD 70000

enum PSTricksKeyWord
{
	PSTPSTricksOff = PST_MIN_KEYWORD,
	PSTaltcolormode,
	
	//picture
	PSTbegin,
	PSTend,
	PSTpspicture,
	PSTendpspicture,	
	PSTpsmatrix,
	PSTendpsmatrix,	
	
	//tree
	PSTpstree,
	PSTpsTree,
	PSTendpsTree,
	
	PSTpstextpath,
	PSTpscharpath,
	PSTpscharclip,
	PSTendpscharclip,
	
	PSTpstRotPOintIIID,
	PSTgetThreeDCoor,
	PSTpstaddThreeDVec,
	PSTpstsubThreeDVec,
	
	//verb
	PSTpstverb,
	PSTpslbrace,
	PSTpsrbrace,
	
	PSToverlaybox,
	PSTendoverlaybox,
	PSTpsoverlay,
	PSTputoverlaybox,	
	
	PSTepsfbox,
	PSTTeXtoEPS,
	PSTendTeXtoEPS,
	PSTPSTtoEPS,
	PSTPSTricksEPS,
	
	//new color
	PSTnewgray,
	PSTnewrgbcolor,
	PSTnewhsbcolor,
	PSTnewcmykcolor,
	
	//set param
	PSTpsset,	
	PSTpssetlength,
	PSTpsaddtolength,	
	PSTdegrees,
	PSTradians,
	PSTpsspan,
	PSTtspace,
	PSTnewpsobject,
	PSTnewpsstyle,
	PSTSpecialCoor,
	PSTNormalCoor,	
	PSTpsmathboxtrue,
	PSTpsmathboxfalse,
	PSTeverypsbox,
	PSTpslongbox,
	PSTpsverbboxtrue,
	PSTpsverbboxfalse,
	PSTpslabelsep,
	PSTpshlabel,
	PSTpsvlabel,
	
	//lines and polygons
	PSTpsline,
	PSTqline,
	PSTpspolygon,
	PSTpsframe,
	PSTpsdiamond,
	PSTpstriangle,
	
	//arcs, circle and ellipse
	PSTpscircle,
	PSTqdisk,
	PSTpswedge,
	PSTpsellipse,
	PSTpsarc,
	PSTpsarcn,
	PSTpsellipticarc,
	PSTpsellipticarcn,
	
	//curves
	PSTpsbezier,
	PSTparabola,
	PSTpscurve,
	PSTpsecurve,
	PSTpsccurve,
	
	//dots
	PSTpsdot,
	PSTpsdots,
	
	//grid
	PSTpsgrid,
	
	//plot
	PSTfileplot,
	PSTdataplot,
	PSTsavedata,
	PSTreaddata,
	PSTlistplot,
	PSTpsplot,
	PSTparametricplot,	
	
	PSTpsaxes,
	
	PSTpscoil,
	PSTpsCoil,
	PSTpszigzag,
	
	PSTpstThreeDCoor,
	PSTpstThreeDPlaneGrid,	
	PSTpstThreeDNode,
	PSTpstThreeDDot,
	PSTpstThreeDLine,
	PSTpstThreeDTriangle,
	PSTpstThreeDSquare,
	PSTpstThreeDBox,
	PSTpsBox,
	PSTpstThreeDEllipse,
	PSTpstThreeDCircle,
	PSTpstIIIDCylinder,
	PSTpsCylinder,
	PSTpstParaboloid,
	PSTpstThreeDSphere,
	PSTpsplotThreeD,
	PSTparametricplotThreeD,
	PSTfileplotThreeD,
	PSTdataplotThreeD,
	PSTlistplotThreeD,
	
	PSTpscustom,
	
	//safe custom
	PSTnewpath,
	PSTmoveto,
	PSTclosepath,
	PSTstroke,
	PSTfill,
	PSTgsave,
	PSTgrestore,
	PSTtranslate,
	PSTscale,
	PSTrotate,
	PSTswapaxes,
	PSTmsave,
	PSTmrestore,
	PSTopenshadow,
	PSTclosedshadow,
	PSTmovepath,
	
	PSTlineto,
	PSTrlineto,
	PSTcurveto,
	PSTrcurveto,
	
	PSTcode,
	PSTdim,
	PSTcoor,
	PSTrcoor,
	PSTfile,
	PSTarrows,
	PSTsetcolor,	
	
	//put
	PSTrput,
	PSTuput,
	PSTcput,
	PSTcnodeput,
	PSTpstThreeDPut,
	PSTpstPlanePut,
	
	//repetition
	PSTmultirput,
	PSTmultips,
	PSTmultido,
	
	//framed box
	PSTpsframebox,
	PSTpsdblframebox,
	PSTpsshadowbox,
	PSTpscirclebox,	
	PSTpsovalbox,
	PSTpsdiabox,
	PSTpstribox,
	
	//clip
	PSTclipbox,
	PSTpsclip,
	PSTendpsclip,
	
	//rotate
	PSTrotateleft,
	PSTrotateright,
	PSTrotatedown,
	
	//scale
	PSTpsscalebox,
	PSTpsscaleboxto,
	
	//node
	PSTrnode,
	PSTRnode,
	PSTpnode,
	PSTcnode,
	PSTCnode,
	PSTcirclenode,	
	PSTovalnode,
	PSTdianode,
	PSTtrinode,
	PSTdotnode,
	PSTfnode,
	
	//node connect
	PSTncline,
	PSTncarc,
	PSTncdiag,
	PSTncdiagg,
	PSTncbar,
	PSTncangle,
	PSTncangles,
	PSTncloop,
	PSTnccurve,
	PSTnccircle,
	PSTncbox,
	PSTncarcbox,
	PSTnccoil,
	PSTnczigzag,
	PSTpcline,
	PSTpccurve,
	PSTpcarc,
	PSTpcbar,
	PSTpcdiag,
	PSTpcdiagg,
	PSTpcangle,
	PSTpcangles,
	PSTpcloop,
	PSTpcbox,
	PSTpcarcbox,	
	PSTpccoil,
	PSTpczigzag,
	
	
	//labels
	PSTncput,
	PSTnaput,
	PSTnbput,	
	PSTtvput,
	PSTtlput,
	PSTtrput,
	PSTthput,
	PSTtaput,
	PSTtbput,	
	PSTnput,
	
	//tree node
	PSTTp,
	PSTTc,
	PSTTC,
	PSTTf,
	PSTTdot,
	PSTTr,
	PSTTR,
	PSTTcircle,
	PSTTCircle,
	PSTToval,
	PSTTdia,
	PSTTtri,
	PSTTn,
	PSTTfan,	
	
	PSTskiplevel,
	PSTskiplevels,
	PSTendskiplevels,
	
	PSTMakeShortNab,
	PSTMakeShortTablr,
	PSTMakeShortTab,
	PSTMakeShortTnput,
	
	//color
	PSTblack,
	PSTdarkgray,
	PSTgray,
	PSTlightgray,
	PSTwhite,
		
	//coordinate system
	PSTunit,
	PSTxunit,
	PSTyunit,
	PSTrunit,	
	PSTorigin,
	
	PSTOx,
	PSTOy,
	PSTDx,
	PSTDy,
	PSTdx,
	PSTdy,
	PSTlabels,
	
	PSTticks,
	PSTtickstyle,
	PSTticksize,
	PSTaxesstyle,
	
	PSTxThreeDunit,
	PSTyThreeDunit,
	PSTzThreeDunit,
	PSTAlpha,
	PSTBeta,
	PSTxMin,
	PSTxMax,
	PSTyMin,
	PSTyMax,
	PSTzMin,
	PSTzMax,
	PSTnameX,
	PSTspotX,
	PSTnameY,
	PSTspotY,
	PSTnameZ,
	PSTspotZ,
	PSTIIIDticks,
	PSTIIIDlabels,
	PSTDz,
	PSTIIIDxTicksPlane,
	PSTIIIDyTicksPlane,
	PSTIIIDzTicksPlane,
	PSTIIIDticksize,
	PSTIIIDxticksep,
	PSTIIIDyticksep,
	PSTIIIDzticksep,
	PSTRotX,
	PSTRotY,
	PSTRotZ,
	PSTRotAngle,
	PSTxRotVec,
	PSTyRotVec,
	PSTzRotVec,
	PSTRotSequence,
	PSTRotSet,
	PSTeulerRotation,
	PSTIIIDOffset,
	PSTzlabelFactor,
	PSTSphericalCoor,
	PSTCylindricalCoor,
	PSTCoorCheck,
	PSTleftHanded,
	PSTcomma,
	PSTcoorType,
	PSTdrawing,
	//PSTswapaxes
	
	//line style
	PSTlinewidth,
	PSTlinecolor,	
	PSTlinestyle,
	PSTdash,
	PSTdotsep,
	PSTborder,
	PSTbordercolor,
	PSTdoubleline,
	PSTdoublesep,
	PSTdoublecolor,
	PSTshadow,
	PSTshadowsize,
	PSTshadowangle,
	PSTshadowcolor,
	PSTdimen,
	PSTlinejoin,
	
	//fill style
	PSTfillstyle,
	PSTfillcolor,		
	PSThatchwidth,
	PSThatchsep,
	PSThatchcolor,
	PSThatchangle,	
	PSTgradbegin,
	PSTgradend,
	PSTgradlines,
	PSTgradmidpoint,
	PSTgradangle,
	
	//arrow
	//PSTarrows
	PSTarrowsize,
	PSTarrowlength,
	PSTarrowinset,
	PSTtbarsize,
	PSTbracketlength,
	PSTrbracketlength,
	PSTarrowscale,
		
	//arc
	PSTlinearc,
	PSTframearc,
	PSTcornersize,
	PSTarcsepA,
	PSTarcsepB,
	PSTarcsep,
	PSTcurvature,
	PSTbeginAngle,
	PSTendAngle,
	
	//dot
	PSTdotstyle,
	PSTdotsize,
	PSTdotscale,
	PSTdotangle,
	
	//grid
	PSTgridwidth,
	PSTgridcolor,
	PSTgriddots,
	PSTgridlabels,
	PSTgridlabelcolor,
	PSTsubgriddiv,
	PSTsubgridwidth,
	PSTsubgridcolor,
	PSTsubgriddots,
	
	PSTplaneGrid,
	PSTsubticks,
	PSTplaneGridOffset,
	
	PSTincrement,
	PSTHincrement,
	PSTSegmentColor,
	
	//plot
	PSTplotstyle,
	PSTplotpoints,
	PSTxPlotpoints,
	PSTyPlotpoints,
	PSTdrawStyle,
	PSThiddenLine,
	PSTalgebraic,
	
	//coil
	PSTcoilwidth,
	PSTcoilheight,
	PSTcoilarm,
	PSTcoilaspect,
	PSTcoilinc,
	
	//misc
	PSTlabelsep,
	PSTframesep,
	PSTboxsep,
	PSTtrimode,	
	PSTgangle,	
	PSTshowpoints,	
	PSTshoworigin,
	PSTshowInside,
	PSTIIIDshowgrid,
	
	//3D put
	PSTpOrigin,
	PSTplane,
	PSTplanecorr,
	PSTdrawCoor,
	
	//matrix
	PSTmnode,
	PSTemnode,
	PSTname,
	PSTnodealign,
	PSTmcol,
	PSTrowsep,
	PSTcolsep,
	PSTmnodesize,
	
	//node
	PSThref,
	PSTvref,
	PSTradius,
	PSTframesize,
	
	PSTnodesep,
	PSTnodesepA,
	PSTnodesepB,
	PSTarcangle,
	PSTangle,
	PSTangleA,
	PSTangleB,
	PSTarm,
	PSTarmA,
	PSTarmB,
	PSTloopsize,
	PSTncurv,
	PSTncurvA,
	PSTncurvB,
	PSTboxsize,
	PSTboxheight,
	PSTboxdepth,
	PSToffset,
	PSToffsetA,
	PSToffsetB,
	
	PSTref,
	PSTnrot,
	PSTnpos,
	PSTshortput,
	
	PSTtpos,
	
	PSTrot,
	
	//tree
	PSTfansize,
	PSTtreemode,
	PSTtreeflip,
	PSTtreesep,
	PSTtreefit,
	PSTtreenodesize,
	PSTlevelsep,
	PSTedge,
	PSTtnpos,
	PSTtnsep,
	PSTtnheight,
	PSTtndepth,
	PSTtnyref,
	PSTbbl,
	PSTbbr,
	PSTbbh,
	PSTbbd,
	PSTxbbl,
	PSTxbbr,
	PSTxbbh,
	PSTxbbd,
	PSTshowbbox,
	PSTthistreesep,
	PSTthistreenodesize,
	PSTthistreefit,
	PSTthislevelsep,
	
	//custom
	PSTstyle,
	PSTlinetype,
	PSTliftpen,
	
	PSTbbllx,
	PSTbblly,
	PSTbburx,
	PSTbbury,
	PSTmakeeps,
	PSTheaderfile,
	PSTheaders,
	
	//nokeyword
	PSTpstrickopts,
	PSTpstrickcolumn,
	PSTpstrickrow,
	PSTpstrickroot,
	PSTpstricksuccessors,
	PSTpstrickobjs,
	PSTpstrickcliped,
	PSTpstrickskiped
};

#endif //PSTRICKSKEYWORD_H