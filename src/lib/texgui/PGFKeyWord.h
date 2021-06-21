/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef PGFKEYWORD_H
#define PGFKEYWORD_H

#define PGF_MIN_KEYWORD 80000

enum PGFKeyWord
{
	//preamble
	//load lib
	PGFusetikzlibrary = PGF_MIN_KEYWORD,
	PGFusepgflibrary,
	PGFusepgfmodule,

	//externalize
	PGFtikzexternalize,
	PGFpgfrealjobname,

	//page
	PGFpgfpagesuselayout,
	PGFpgfpagesdeclarelayout,
	PGFpgfpagesphysicalpageoptions,
	PGFpgfpageslogicalpageoptions,
	PGFpgfpagesshipoutlogicalpage,
	PGFpgfpagescurrentpagewillbelogicalpage,

	//declare
	PGFtikzdeclarecoordinatesystem,
	PGFtikzaliascoordinatesystem,
	PGFpgfdeclarelindenmayersystem,
	PGFpgfdeclaredecoration,
	PGFpgfdeclaremetadecorate,
	PGFpgfarrowsdeclare,
	PGFpgfarrowsdeclarealias,
	PGFpgfarrowsdeclarereversed,
	PGFpgfarrowsdeclarecombine,
	PGFpgfarrowsdeclaredouble,
	PGFpgfarrowsdeclaretriple,
	PGFpgfdeclareplotmark,
	PGFpgfdeclareshape,
	PGFpgfdeclarepatternformonly,
	PGFpgfdeclarepatterninherentlycolored,
	PGFpgfdeclareimage,
	PGFpgfaliasimage,
	PGFpgfdeclaremask,
	PGFpgfdeclarelayer,
	PGFpgfdeclarehorizontalshading,
	PGFpgfdeclareverticalshading,
	PGFpgfdeclareradialshading,
	PGFpgfdeclarefunctionalshading,
	PGFpgfdeclarefading,
	PGFtikzfading,

	PGFsymbol,
	PGFrule,
	PGFpgflsystemcurrentstep,
	PGFpgflsystemcurrentleftangle,
	PGFpgflsystemcurrentrightangle,
	PGFpgflsystemrandomizestep,
	PGFpgflsystemrandomizeleftangle,
	PGFpgflsystemrandomizerightangle,
	PGFpgflsystemdrawforward,
	PGFpgflsystemmoveforward,
	PGFpgflsystemturnleft,
	PGFpgflsystemturnright,
	PGFpgflsystemsavestate,
	PGFpgflsystemrestorestate,

	PGFstate,
	PGFpgfdecoratedpathlength,
	PGFpgfdecoratedinputsegmentlength,
	PGFpgfpointdecoratedpathlast,
	PGFpgfpointdecoratedinputsegmentlast,
	PGFpgfdecoratedangle,
	PGFpgfdecoratedremainingdistance,
	PGFpgfdecoratedcompleteddistance,
	PGFpgfdecoratedinputsegmentremainingdistance,
	PGFpgfdecoratedinputsegmentcompleteddistance,

	PGFdecoration,
	PGFbeforedecoration,
	PGFafterdecoration,
	PGFpgfpointmetadecoratedpathfirst,
	PGFpgfpointmetadecoratedpathlast,
	PGFpgfmetadecoratedpathlength,
	PGFpgfmetadecoratedcompleteddistance,
	PGFpgfmetadecoratedremainingdistance,
	PGFpgfmetadecoratedinputsegmentcompleteddistance,
	PGFpgfmetadecoratedinputsegmentremainingdistance,

	PGFnodeparts,
	PGFsavedanchor,
	PGFsaveddimen,
	PGFsavedmacro,
	PGFanchor,
	PGFdeferredanchor,
	PGFanchorborder,
	PGFbackgroundpath,
	PGFforegroundpath,
	PGFbehindbackgroundpath,
	PGFbeforebackgroundpath,
	PGFbehindforegroundpath,
	PGFbeforeforegroundpath,
	PGFinheritsavedanchors,
	PGFinheritbehindbackgroundpath,
	PGFinheritbackgroundpath,
	PGFinheritbeforebackgroundpath,
	PGFinheritbehindforegroundpath,
	PGFinheritforegroundpath,
	PGFinheritbeforeforegroundpath,
	PGFinheritanchor,
	PGFinheritanchorborder,

	PGFtikzsetexternalprefix,
	PGFtikzsetnextfilename,
	PGFtikzsetfigurename,
	PGFtikzappendtofigurename,
	PGFtikzpicturedependsonfile,
	PGFtikzexternalfiledependsonfile,
	PGFtikzexternaldisable,
	PGFtikzexternalenable,
	PGFtikzifexternalizing,
	PGFtikzifexternalizingnext,

	PGFpgflibraryfpuifactive,
	PGFpgfmathfloatparsenumber,
	PGFpgfmathfloatqparsenumber,
	PGFpgfmathfloattofixed,
	PGFpgfmathfloattoint,
	PGFpgfmathfloattosci,
	PGFpgfmathfloatvalueof,
	PGFpgfmathfloatcreate,
	PGFpgfmathfloatifflags,
	PGFpgfmathfloattomacro,
	PGFpgfmathfloattoregisters,
	PGFpgfmathfloattoregisterstok,
	PGFpgfmathfloatgetflags,
	PGFpgfmathfloatgetflagstomacro,
	PGFpgfmathfloatgetmantissa,
	PGFpgfmathfloatgetmantissatok,
	PGFpgfmathfloatgetexponent,
	PGFpgfmathroundto,
	PGFpgfmathroundtozerofill,
	PGFpgfmathfloatround,
	PGFpgfmathfloatroundzerofill,
	PGFpgfmathfloat,
	PGFpgfmathfloattoextentedprecision,
	PGFpgfmathfloatsetextprecision,
	PGFpgfmathfloatlessthan,
	PGFpgfmathfloatmultiplyfixed,
	PGFpgfmathfloatifapproxequalrel,
	PGFpgfmathfloatshift,
	PGFpgfmathfloatabserror,
	PGFpgfmathfloatrelerror,
	PGFpgfmathfloatint,
	PGFpgfmathlog,

	PGFpgfkeyssetvalue,
	PGFpgfkeyslet,
	PGFpgfkeysgetvalue,
	PGFpgfkeysvalueof,
	PGFpgfkeysifdefined,
	PGFpgfkeys,
	PGFpgfqkeys,
	PGFpgfkeysalso,
	PGFpgfqkeysalso,
	PGFpgfkeysdef,
	PGFpgfkeysedef,
	PGFpgfkeysdefnargs,
	PGFpgfkeysedefnargs,
	PGFpgfkeysdefargs,
	PGFpgfkeysedefargs,
	PGFpgfkeysfiltered,
	PGFpgfqkeysfiltered,
	PGFpgfkeysalsofrom,
	PGFpgfkeysalsofiltered,
	PGFpgfkeysalsofilteredfrom,
	PGFpgfkeysactivatefamiliesandfilteroptions,
	PGFpgfqkeysactivatefamiliesandfilteroptions,
	PGFpgfkeysactivatesinglefamilyandfilteroptions,
	PGFpgfqkeysactivatesinglefamilyandfilteroptions,
	PGFpgfkeysevalkeyfilterwith,

	//
	PGFpgfmathparse,
	PGFpgfmathqparse,
	PGFpgfmathpostparse,
	PGFpgfmathsetlength,
	PGFpgfmathaddtolength,
	PGFpgfmathsetcount,
	PGFpgfmathaddtocount,
	PGFpgfmathsetcounter,
	PGFpgfmathaddtocounter,
	PGFpgfmathsetmacro,
	PGFpgfmathsetlengthmacro,
	PGFpgfmathtruncatemacro,
	PGFpgfmathadd,
	PGFpgfmathsubtract,
	PGFpgfmathneg,
	PGFpgfmathmultiply,
	PGFpgfmathdivide,
	PGFpgfmathdiv,
	PGFpgfmathfactorial,
	PGFpgfmathsqrt,
	PGFpgfmathpow,
	PGFpgfmathe,
	PGFpgfmathexp,
	PGFpgfmathln,
	PGFpgfmathlogten,
	PGFpgfmathlogtwo,
	PGFpgfmathabs,
	PGFpgfmathmod,
	PGFpgfmathMod,

	PGFpgfmathround,
	PGFpgfmathfloor,
	PGFpgfmathceil,
	PGFpgfmathint,
	PGFpgfmathfrac,
	PGFpgfmathreal,

	PGFpgfmathpi,
	PGFpgfmathrad,
	PGFpgfmathdeg,
	PGFpgfmathsin,
	PGFpgfmathcos,
	PGFpgfmathtan,
	PGFpgfmathsec,
	PGFpgfmathcosec,
	PGFpgfmathcot,
	PGFpgfmathasin,
	PGFpgfmathacos,
	PGFpgfmathatan,
	PGFpgfmathatantwo,

	PGFpgfmathequal,
	PGFpgfmathgreater,
	PGFpgfmathless,
	PGFpgfmathnotequal,
	PGFpgfmathnotgreater,
	PGFpgfmathnotless,
	PGFpgfmathand,
	PGFpgfmathor,
	PGFpgfmathnot,
	PGFpgfmathifthenelse,
	PGFpgfmathtrue,
	PGFpgfmathfalse,

	PGFpgfmathrnd,
	PGFpgfmathrand,
	PGFpgfmathrandom,

	PGFpgfmathhex,
	PGFpgfmathHex,
	PGFpgfmathoct,
	PGFpgfmathbin,

	PGFpgfmathmin,
	PGFpgfmathmax,
	PGFpgfmathveclen,
	PGFpgfmatharray,
	PGFpgfmathsinh,
	PGFpgfmathcosh,
	PGFpgfmathtanh,
	PGFpgfmathwidth,
	PGFpgfmathheight,
	PGFpgfmathdepth,

	PGFpgfmathreciprocal,

	PGFpgfmathapproxequalto,

	PGFpgfmathgeneratepseudorandomnumber,
	PGFpgfmathrandominteger,
	PGFpgfmathdeclarerandomlist,
	PGFpgfmathrandomitem,
	PGFpgfmathsetseed,

	PGFpgfmathbasetodec,
	PGFpgfmathdectobase,
	PGFpgfmathdectoBase,
	PGFpgfmathbasetobase,
	PGFpgfmathbasetoBase,
	PGFpgfmathsetbasenumberlength,

	PGFpgfmathdeclarefunction,
	PGFpgfmathredeclarefunction,

	PGFpgfmathprintnumber,
	PGFpgfmathprintnumberto,
	PGFpgfmathifisint,

	PGFpgfooclass,
	PGFpgfoonew,
	PGFpgfoogc,
	PGFmethod,
	PGFpgfoothis,
	PGFattribute,
	PGFpgfooset,
	PGFpgfoolet,
	PGFpgfoovalueof,
	PGFpgfooget,
	PGFpgfooobj,

	PGFpgfpointdecoratedpathfirst,
	PGFpgfdecorateexistingpath,
	PGFpgfdecoratedpath,
	PGFpgfdecorationpath,
	PGFpgfpointdecorationpathlast,

	PGFpgfdecoratepath,
	PGFpgfdecoratecurrentpath,
	PGFpgfdecoratebeforecode,
	PGFpgfdecorateaftercode,
	PGFpgfsetdecorationsegmenttransformation,

	//options
	PGFtikzset,
	PGFtikzstyle,

	PGFpgfsetbaseline,
	PGFpgfsetbaselinepointnow,
	PGFpgfsetbaselinepointlater,

	PGFpgfsetlinewidth,
	PGFpgflinewidth,
	PGFpgfsetbuttcap,
	PGFpgfsetroundcap,
	PGFpgfsetrectcap,
	PGFpgfsetroundjoin,
	PGFpgfsetbeveljoin,
	PGFpgfsetmiterjoin,
	PGFpgfsetmiterlimit,
	PGFpgfsetdash,
	PGFpgfsetinnerlinewidth,
	PGFpgfseteorule,
	PGFpgfsetnonzerorule,

	PGFpgfsetstrokecolor,
	PGFpgfsetcolor,
	PGFpgfsetinnerstrokecolor,
	PGFpgfsetfillcolor,

	PGFpgfsetfillpattern,
	PGFpgfsetlayers,
	PGFpgfshadecolortorgb,
	PGFpgfuseshading,
	PGFpgfshadepath,
	PGFpgfsetadditionalshadetransform,

	PGFpgfsetstrokeopacity,
	PGFpgfsetfillopacity,
	PGFpgfsetfading,
	PGFpgfsetfadingforcurrentpath,

	PGFpgfsetarrowsstart,
	PGFpgfsetarrowsend,
	PGFpgfsetarrows,
	PGFpgfsetshortenstart,
	PGFpgfsetshortenend,
	PGFpgfsetarrowoptions,
	PGFpgfgetarrowoptions,

	PGFpgfsetmatrixcolumnsep,
	PGFpgfsetmatrixrowsep,

	PGFpgftransformshift,
	PGFpgftransformxshift,
	PGFpgftransformyshift,
	PGFpgftransformscale,
	PGFpgftransformxscale,
	PGFpgftransformyscale,
	PGFpgftransformxslant,
	PGFpgftransformyslant,
	PGFpgftransformrotate,
	PGFpgftransformtriangle,
	PGFpgftransformcm,
	PGFpgftransformarrow,
	PGFpgftransformlineattime,
	PGFpgftransformcurveattime,
	PGFpgftransformreset,
	PGFpgftransformresetnontranslations,
	PGFpgftransforminvert,
	PGFpgfgettransform,
	PGFpgfsettransform,
	PGFpgfgettransformentries,
	PGFpgfsettransformentries,
	PGFifpgfslopedattime,
	PGFifpgfallowupsidedowattime,
	PGFifpgfresetnontranslationsattime,

	PGFbeginpgfgraphicnamed,
	PGFendpgfgraphicnamed,

	//picture command
	PGFtikzpicture,
	PGFpgfpicture,
	PGFtikzfadingfrompicture,
	PGFendtikzpicture,
	PGFendpgfpicture,
	PGFendtikzfadingfrompicture,

	PGFstarttikzpicture,
	PGFstartpgfpicture,
	PGFstarttikzfadingfrompicture,
	PGFstoptikzpicture,
	PGFstoppgfpicture,
	PGFstoptikzfadingfrompicture,

	PGFforeach,
	PGFbreakforeach,

	PGFcolorcurrentmixin,

	PGFscope,
	PGFpgfscope,
	PGFpgfinterruptpath,
	PGFpgfinterruptpicture,
	PGFpgfinterruptboundingbox,
	PGFpgflowlevelscope,
	PGFpgfdecoration,
	PGFpgfmetadecoration,

	PGFpgfonlayer,
	PGFpgftransparencygroup,

	PGFendscope,
	PGFendpgfscope,
	PGFendpgfinterruptpath,
	PGFendpgfinterruptpicture,
	PGFendpgfinterruptboundingbox,
	PGFendpgflowlevelscope,
	PGFendpgfdecoration,
	PGFendpgfmetadecoration,

	PGFendpgfonlayer,
	PGFendpgftransparencygroup,

	PGFstartscope,
	PGFstartpgfscope,
	PGFstartpgfinterruptpath,
	PGFstartpgfinterruptpicture,
	PGFstartpgfinterruptboundingbox,
	PGFstartpgflowlevelscope,
	PGFstartpgfdecoration,
	PGFstartpgfmetadecoration,

	PGFstartpgfonlayer,
	PGFstartpgftransparencygroup,

	PGFstopscope,
	PGFstoppgfscope,
	PGFstoppgfinterruptpath,
	PGFstoppgfinterruptpicture,
	PGFstoppgfinterruptboundingbox,
	PGFstoppgflowlevelscope,
	PGFstoppgfdecoration,
	PGFstoppgfmetadecoration,
	PGFstoppgfonlayer,
	PGFstoppgftransparencygroup,

	PGFarrow,
	PGFarrowreversed,

	//node
	PGFnodepart,

	PGFtikz,

	PGFbegin,
	PGFend,

	PGFpgfextra,
	PGFendpgfextra,

	//path action
	PGFpath,
	PGFdraw,
	PGFfill,
	PGFfilldraw,
	PGFpattern,
	PGFshade,
	PGFshadedraw,
	PGFclip,
	PGFuseasboundingbox,
	PGFnode,
	PGFcoordinate,
	PGFpic,
	PGFmatrix,
	PGFcalendar,
	PGFchainin,
	PGFspy,

	PGFpgflindenmayersystem,
	PGFtikzfoldingdodecahedron,
	PGFpgfpathsvg,
	PGFpgftext,

	//
	PGFpgfplothandlercurveto,
	PGFpgfsetplottension,
	PGFpgfplothandlerclosedcurve,
	PGFpgfplothandlerconstantlineto,
	PGFpgfplothandlerconstantlinetomarkright,
	PGFpgfplothandlerjumpmarkleft,
	PGFpgfplothandlerjumpmarkright,
	PGFpgfplothandlerxcomb,
	PGFpgfplothandlerycomb,
	PGFpgfplothandlerpolarcomb,
	PGFpgfplotxzerolevelstreamconstant,
	PGFpgfplotyzerolevelstreamconstant,
	PGFpgfplothandlerybar,
	PGFpgfplothandlerxbar,
	PGFpgfplotbarwidth,
	PGFpgfplothandlerybarinterval,
	PGFpgfplothandlerxbarinterval,
	PGFpgfplothandlermark,
	PGFpgfsetplotmarkrepeat,
	PGFpgfsetplotmarkphase,
	PGFpgfplothandlermarklisted,
	PGFpgfuseplotmark,
	PGFpgfsetplotmarksize,
	PGFpgfplotmarksize,

	//
	PGFpgfcalendardatetojulian,
	PGFpgfcalendarjuliantodate,
	PGFpgfcalendarjuliantoweekday,
	PGFpgfcalendarifdate,
	PGFpgfcalendarweekdayname,
	PGFpgfcalendarweekdayshortname,
	PGFpgfcalendarmonthname,
	PGFpgfcalendarmonthshortname,
	PGFpgfcalendar,
	PGFifdate,
	PGFpgfcalendarshorthand,
	PGFpgfcalendarsuggestedname,

	PGFpgfpoint,
	PGFpgfpointorigin,
	PGFpgfpointpolar,
	PGFpgfpointxy,
	PGFpgfsetxvec,
	PGFpgfsetyvec,
	PGFpgfpointpolarxy,
	PGFpgfpointxyz,
	PGFpgfsetzvec,
	PGFpgfpointcylindrical,
	PGFpgfpointspherical,

	PGFpgfpointadd,
	PGFpgfpointscale,
	PGFpgfpointdiff,
	PGFpgfpointnormalised,
	PGFpgfpointlineattime,
	PGFpgfpointlineatdistance,
	PGFpgfpointcurveattime,
	PGFpgfpointborderrectangle,
	PGFpgfpointborderellipse,
	PGFpgfpointintersectionoflines,
	PGFpgfpointintersectionofcircles,
	PGFpgfintersectionofpaths,
	PGFpgfintersectionsolutions,
	PGFpgfpointintersectionsolution,
	PGFpgfintersectionsortbyfirstpath,
	PGFpgfintersectionsortbysecondpath,

	PGFpgfextractx,
	PGFpgfextracty,
	PGFpgfgetlastxy,

	PGFpgfpathmoveto,
	PGFpgfpathlineto,
	PGFpgfpathcurveto,
	PGFpgfpathquadraticcurveto,
	PGFpgfpathcurvebetweentime,
	PGFpgfpathcurvebetweentimecontinue,

	PGFpgfpathclose,
	PGFpgfpatharc,
	PGFpgfpatharcaxes,
	PGFpgfpatharcto,

	PGFpgfpatharctoprecomputed,
	PGFpgfpatharctomaxstepsize,
	PGFpgfpathellipse,
	PGFpgfpathcircle,
	PGFpgfpathrectangle,
	PGFpgfpathrectanglecorners,

	PGFpgfpathgrid,
	PGFpgfpathparabola,
	PGFpgfpathsine,
	PGFpgfpathcosine,
	PGFpgfsetcornersarced,
	PGFpgfresetboundingbox,

	PGFpgfusepath,

	PGFpgfnode,
	PGFpgfmultipartnode,
	PGFpgfcoordinate,
	PGFpgfnodealias,
	PGFpgfnoderename,
	PGFpgfpositionnodelater,
	PGFpgfpositionnodelaterbox,
	PGFpgfpositionnodelatername,
	PGFpgfpositionnodelaterminx,
	PGFpgfpositionnodelaterminy,
	PGFpgfpositionnodelatermaxx,
	PGFpgfpositionnodelatermaxy,
	PGFpgfpositionnodenow,
	PGFpgfpointanchor,
	PGFpgfpointshapeborder,

	PGFpgfmatrix,
	PGFpgfmatrixnextcell,
	PGFpgfmatrixendrow,
	PGFpgfmatrixemptycode,
	PGFpgfmatrixbegincode,
	PGFpgfmatrixendcode,
	PGFpgfmatrixcurrentrow,
	PGFpgfmatrixcurrentcolumn,

	PGFpgflowlevelsynccm,
	PGFpgflowlevel,
	PGFpgflowlevelobj,

	PGFpgfuseimage,
	PGFpgfalternateextension,
	PGFpgfimage,

	PGFpgfplotstreamstart,
	PGFpgfplotstreampoint,
	PGFpgfplotstreamend,
	PGFpgfplotxyfile,
	PGFpgfplotxyzfile,
	PGFpgfplotfunction,
	PGFpgfplotgnuplot,
	PGFpgfplothandlerlineto,
	PGFpgfsetmovetofirstplotpoint,
	PGFpgfsetlinetofirstplotpoint,
	PGFpgfplothandlerpolygon,
	PGFpgfplothandlerdiscard,
	PGFpgfplothandlerrecord,

	PGFpgfqpoint,
	PGFpgfqpointxy,
	PGFpgfqpointxyz,
	PGFpgfqpointscale,
	PGFpgfpathqmoveto,
	PGFpgfpathqlineto,
	PGFpgfpathqcurveto,
	PGFpgfpathqcircle,
	PGFpgfusepathqstroke,
	PGFpgfusepathqfill,
	PGFpgfusepathqfillstroke,
	PGFpgfusepathqclip,
	PGFpgfqbox,
	PGFpgfqboxsynced,

	//execute
	PGFexecuteatbeginpicture,
	PGFexecuteatendpicture,
	PGFexecuteatbeginscope,
	PGFexecuteatendscope,
	PGFexecuteatbeginto,
	PGFexecuteatendto,
	PGFexecuteatbegincell,
	PGFexecuteatendcell,
	PGFexecuteatemptycell,
	PGFexecutebeforedayscope,
	PGFexecuteatbegindayscope,
	PGFexecuteatenddayscope,
	PGFexecuteafterdayscope,

	//every
	PGFeverypicture,
	PGFeveryscope,
	PGFeverypath,
	PGFeverycircle,
	PGFeveryto,
	PGFeverynode,
	PGFeveryrectanglenode,
	PGFeverycirclenode,
	PGFeverylabel,
	PGFeverypin,
	PGFeverypinedge,
	PGFeveryedge,
	PGFeverymatrix,
	PGFeverycell,
	PGFeveryoddcolumn,
	PGFeveryevencolumn,
	PGFeveryoddrow,
	PGFeveryevenrow,
	PGFeverychild,
	PGFeverychildnode,
	PGFeveryplot,
	PGFeverymark,
	PGFeverystate,
	PGFeveryinitialbyarrow,
	PGFeveryacceptingbyarrow,
	PGFeveryCalendar,
	PGFeveryday,
	PGFeverymonth,
	PGFeveryyear,
	PGFeveryonchain,
	PGFeveryJoin,
	PGFeverycircuitsymbol,
	PGFeveryinfo,
	PGFeverycircuitlogic,
	PGFeverycircuitEE,
	PGFeverycircuitannotation,
	PGFeverylightemitting,
	PGFeveryEntity,
	PGFeveryrelationship,
	PGFeveryattribute,
	PGFeveryfit,
	PGFeverydelimiter,
	PGFeveryleftdelimiter,
	PGFeveryrightdelimiter,
	PGFeveryabovedelimiter,
	PGFeverybelowdelimiter,

	PGFeverymindmap,
	PGFeveryconcept,
	PGFeveryextraconcept,
	PGFeverycircleconnectionbar,
	PGFeveryannotation,

	PGFeverycut,
	PGFeveryfold,

	PGFeveryplace,
	PGFeverytransition,
	PGFeverytoken,

	PGFeveryshadow,

	PGFeveryspyinnode,
	PGFeveryspyonnode,

	PGFeveryloop,

	PGFeverydecoration,

	//Coordinate systems
	PGFcanvas,
	PGFxyz,
	PGFcanvaspolar,
	PGFxyzpolar,
	PGFxypolar,
	PGFbarycentric,
	PGFtangent,
	PGFperpendicular,
	PGFangle,
	PGFpoint,
	PGFsolution,
	PGFhorizontallinethrough,
	PGFverticallinethrough,

	//Operation
	PGFSPlus,
	PGFDPlus,
	PGFlineto,
	PGFhorivertlines,
	PGFverthorilines,
	PGFcurveto,
	PGFcurveto2,
	PGFcurvetolast,
	PGFcycle,
	PGFrectangle,
	PGFcircle,
	PGFellipse,
	PGFarc,
	PGFgrid,
	PGFparabola,
	PGFpbend,
	PGFsin,
	PGFcos,
	PGFsvg,
	PGFsvgdata,
	PGFto,
	PGFlet,
	//PGFnode
	//PGFnodetext
	//PGFcoordinate
	//PGFat
	PGFedge,
	PGFchild,
	PGFedgefromparent,
	PGFplot,
	PGFcoordinates,
	PGFfile,
	PGFfunction,
	PGFdecorate,
	PGFlindenmayersystem,
	PGFlsystem,

	//path
	PGFbaseline,
	PGFcolor,
	//PGFdraw
	//PGFfill
	//PGFclip
	PGFarrows,
	PGFarrowtip,
	PGFstealth,
	PGFtoreversed,
	PGFlatex,
	PGFbar,
	PGFspace,
	PGFshortenend,
	PGFshortenstart,
	PGFradius,
	PGFxradius,
	PGFyradius,
	PGFstartangle,
	PGFendangle,
	PGFdeltaangle,
	PGFstep,
	PGFxstep,
	PGFystep,
	PGFbend,
	PGFbendpos,
	PGFparabolaheight,
	PGFbendatstart,
	PGFbendatend,
	PGFuseasboundingboxo,
	PGFpathfading,
	PGFfitfading,
	PGFfadingtransform,
	PGFfadingangle,
	PGFscopefading,
	PGFtransparencygroup,
	PGFpreactions,
	PGFpostaction,
	PGFtrimleft,
	PGFtrimright,
	PGFtrimlowlevel,
	PGFhelplines,
	PGFnamepath,
	PGFnamepathglobal,
	PGFnameintersections,
	PGFinsertpath,
	PGFappendaftercommand,
	PGFprefixaftercommand,
	PGFroundedcorners,
	PGFsharpcorners,
	PGFof,
	PGFtotal,
	PGFby,
	PGFsortby,

	//clolr
	PGFblack,
	PGFblue,
	PGFbrown,
	PGFcyan,
	PGFdarkgray,
	PGFgray,
	PGFgreen,
	PGFlightgray,
	PGFlime,
	PGFmagenta,
	PGFolive,
	PGForange,
	PGFpink,
	PGFpurple,
	PGFred,
	PGFteal,
	PGFviolet,
	PGFwhite,
	PGFyellow,
	PGFlightsteelblue,
	PGFdarklightsteelblue,

	//line
	PGFlinewidth,
	PGFultrathin,
	PGFverythin,
	PGFthin,
	PGFsemithick,
	PGFthick,
	PGFverythick,
	PGFultrathick,
	PGFlinecap,
	PGFround,
  PGFrect,
  PGFbutt,
	PGFlinejoin,
	PGFbevel,
	PGFmiter,
	PGFcap,
	PGFmiterlimit,
	PGFdashpattern,
	PGFdashphase,
	PGFsolid,
	PGFdotted,
	PGFdenselydotted,
	PGFlooselydotted,
	PGFdashed,
	PGFdenselydashed,
	PGFlooselydashed,
	PGFdashdotted,
	PGFdenselydashdotted,
	PGFlooselydashdotted,
	PGFdashdotdotted,
	PGFdenselydashdotdotted,
	PGFlooselydashdotdotted,
	PGFdouble,
	PGFdoubledistance,
	PGDdoubledistancebetweenlinecenters,
	PGFdoubleequalsigndistance,

	//fill
	//PGFpattern
	PGFpatterncolor,
	PGFnonzerorule,
	PGFevenoddrule,
	PGFdrawopacity,
	PGFopacity,
	PGFtransparent,
	PGFultranearlytransparent,
	PGFverynearlytransparent,
	PGFnearlytransparent,
	PGFsemitransparent,
	PGFnearlyopaque,
	PGFverynearlyopaque,
	PGFultranearlyopaque,
	PGFopaque,
	PGFfillopacity,
	PGFtextopacity,
	PGFpathPicture,
	PGFshading,
	PGFshadingangle,
	PGFaxis,
	PGFball,
	PGFradial,
	PGFcolorwheelwhitecenter,
	PGFcolorwheelblackcenter,
	PGFcolorwheel,
	PGFbilinearinterpolation,
	PGFMandelbrotset,
	PGFtopcolor,
	PGFbottomcolor,
	PGFmiddlecolor,
	PGFleftcolor,
	PGFrightcolor,
	PGFballcolor,
	PGFlowerleft,
	PGFupperleft,
	PGFupperright,
	PGFlowerright,
	PGFinnercolor,
	PGFoutercolor,
	PGFhorizontallines,
	PGFverticallines,
	PGFnortheastlines,
	PGFnorthwestlines,
	PGFcrosshatch,
	PGFdots,
	PGFcrosshatchdots,
	PGFfivepointedstars,
	PGFsixpointedstars,
	PGFbricks,
	PGFcheckerboard,
	PGFcheckerboardlightgray,
	PGFhorizontallineslightgray,
	PGFhorizontallinesgray,
	PGFhorizontallinesdarkgray,
	PGFhorizontallineslightblue,
	PGFhorizontallinesdarkblue,
	PGFcrosshatchdotsgray,
	PGFcrosshatchdotslightsteelblue,
	PGFcirclewithfuzzyedge10percent,
	PGFcirclewithfuzzyedge15percent,
	PGFcirclewithfuzzyedge20percent,
	PGFfuzzyring15percent,

	//node
	//PGFanchor
	PGFnodecontents,
  PGFnameprefix,
  PGFnamesuffix,
  PGFbehindpath,
  PGFinfrontofpath,
	PGFname,
	PGFalias,
	PGFat,
	PGFshape,
	PGFinnersep,
	PGFinnerxsep,
	PGFinnerysep,
	PGFoutersep,
	PGFouterxsep,
	PGFouterysep,
	PGFminimumheight,
	PGFminimumwidth,
	PGFminimumsize,
	PGFshapeaspect,
	PGFshapeborderusesincircle,
	PGFshapeborderrotate,
	PGFtext,
	PGFtextwidth,
	PGFalign,
	PGFnodehalignheader,
	PGFtextheight,
	PGFtextdepth,
	PGFfont,
	PGFabove,
	PGFbelow,
	PGFleft,
	PGFright,
	PGFaboveleft,
	PGFaboveright,
	PGFbelowleft,
	PGFbelowright,
	PGFongrid,
	PGFnodedistance,
	PGFbaseleft,
	PGFbaseright,
	PGFmidleft,
	PGFmidright,
	PGFtransformshape,
	PGFpos,
	PGFauto,
	PGFswap,
	PGFsloped,
	PGFallowupsidedown,
	PGFmidway,
	PGFnearstart,
	PGFnearend,
	PGFverynearstart,
	PGFverynearend,
	PGFatstart,
	PGFatend,
	PGFlabel,
	PGFlabelposition,
	PGFabsolute,
	PGFlabeldistance,
	PGFpin,
	PGFpindistance,
	PGFpinposition,
	PGFpinedge,
	PGFrememberpicture,
	PGFoverlay,
	PGFlateoptions,
	//PGFmatrix

	//matrix
	PGFcolumnsep,
	PGFrowsep,
	PGFcells,
	PGFnodes,
	PGFcolumn,
	PGFrow,
	PGFrowcolumn,
	PGFmatrixanchor,
	//PGFanchor
	PGFampersandreplacement,
	PGFmatrixofnodes,
	PGFmatrixofMathnodes,
	PGFnodesinemptycells,
	PGFleftdelimiter,
	PGFrightdelimiter,
	PGFabovedelimiter,
	PGFbelowdelimiter,

	//tree
	PGFlevel,
	PGFlevelone,
	PGFleveltwo,
	PGFlevelthree,
	PGFlevelfour,
	PGFleveldistance,
	PGFsiblingdistance,
	PGFgrow,
	PGFgrowopposite,
	PGFmissing,
	PGFgrowthparentanchor,
	PGFgrowthFunction,
	PGFedgefromparentpath,
	PGFchildanchor,
	PGFparentanchor,
	//PGFedgefromparent
	PGFgrowviathreepoints,
	PGFgrowcyclic,
	PGFsiblingangle,
	PGFclockwisefrom,
	PGFcounterclockwisefrom,
	PGFedgefromparentforkdown,
	PGFedgefromparentforkright,
	PGFedgefromparentforkleft,
	PGFedgefromparentforkup,

	//plot
	PGFvariable,
	PGFsamples,
	PGFdomain,
	PGFsamplesat,
	PGFparametric,
	PGFid,
	PGFprefix,
	PGFrawgnuplot,
	PGFmark,
	PGFmarkrepeat,
	PGFmarkphase,
	PGFmarkindices,
	PGFmarksize,
	PGFmarkoptions,
	PGFnomarks,
	PGFnomarkers,
	PGFsharpplot,
	PGFsmooth,
	PGFtension,
	PGFsmoothcycle,
	PGFconstplot,
	PGFconstplotmarkleft,
	PGFconstplotmarkright,
	PGFjumpmarkleft,
	PGFjumpmarkright,
	PGFycomb,
	PGFxcomb,
	PGFpolarcomb,
	PGFybar,
	PGFxbar,
	PGFybarinterval,
	PGFxbarinterval,
	PGFonlymarks,
	PGFbarwidth,
	PGFbarshift,
	PGFbarintervalshift,
	PGFbarintervalwidth,
	PGFmarkcolor,
	PGFtextmark,
	PGFtextmarkStyle,
	PGFtextmarkAsnode,

	//decorate
	//PGFdecoration
	//PGFdecorate
	PGFraise,
	PGFmirror,
	PGFtransform,
	PGFpre,
	PGFprelength,
	PGFpost,
	PGFpostlength,
	PGFamplitude,
	PGFmetaamplitude,
	PGFsegmentlength,
	PGFmetasegmentlength,
	//PGFangle
	PGFaspect,
	PGFstartradius,
	PGFendradius,
	PGFpathHascorners,
	//PGFradius
	PGFmovetocode,
	PGFlinetocode,
	PGFcurvetocode,
	PGFclosepathcode,
	//PGFmark
	PGFsequencenumber,
	PGFdistancefromstart,
	PGFResetmarks,
	PGFmarkconnectionnode,
	PGFbetweenpositionsandstepwidth,
  PGFatpositionwidth,
	PGFfootlength,
	PGFstridelength,
	PGFfootsep,
	PGFfootangle,
	PGFfootof,
	PGFshapewidth,
	PGFshapeheight,
	PGFshapesize,
	//PGFanchor
	//PGFshape
	PGFshapesep,
	PGFshapeevenlyspread,
	PGFshapesloped,
	PGFshapescaled,
	PGFshapestartwidth,
	PGFshapestartheight,
	PGFshapestartsize,
	PGFshapeendwidth,
	PGFshapeendheight,
	PGFshapeendsize,
	//PGFtext
	PGFtextformatdelimiters,
	PGFtextcolor,
	PGFReversepath,
	PGFtextalign,
	//PGFalign
	//PGFleft
	//PGFright
	//PGFcenter
	PGFleftindent,
	PGFrightindent,
	PGFfittopath,
	PGFfittopathstretchingspaces,

	//transform
	PGFx,
	PGFy,
	PGFz,
	PGFshift,
	PGFshiftonly,
	PGFxshift,
	PGFyshift,
	PGFscale,
	PGFscalearound,
	PGFxscale,
	PGFyscale,
	PGFxslant,
	PGFyslant,
	PGFrotate,
	PGFrotatearound,
	PGFcm,
	PGFresetcm,
	PGFtransformcanvas,
	PGFcurrentpointislocal,

	//automata
	PGFstatewithoutoutput,
	PGFstatewithoutput,
	//PGFstate
	PGFinitial,
	PGFinitialbyarrow,
	PGFinitialtext,
	PGFinitialwhere,
	PGFIntialdistance,
	PGFinitialabove,
	PGFinitialbelow,
	PGFinitialleft,
	PGFinitialright,
	PGFinitialbydiamond,
	PGFaccepting,
	PGFacceptingbydouble,
	PGFacceptingbyarrow,
	PGFacceptingtext,
	PGFacceptingwhere,
	PGFacceptingabove,
	PGFacceptingbelow,
	PGFacceptingleft,
	PGFacceptingright,

	//background
	PGFonbackgroundlayer,
	PGFshowbackgroundrectangle,
	PGFinnerframexsep,
	PGFinnerframeysep,
	PGFinnerframesep,
	PGFTightbackground,
	PGFloosebackground,
	PGFbackgroundrectangle,
	PGFframed,
	PGFshowbackgroundgrid,
	PGFbackgroundgrid,
	PGFgridded,
	PGFshowbackgroundtop,
	PGFouterframeXsep,
	PGFouterframeYsep,
	PGFouterframesep,
	PGFbackgroundtop,
	PGFshowbackgroundbottom,
	PGFshowbackgroundleft,
	PGFshowbackgroundright,

	//calendar
	PGFdates,
	PGFdayxshift,
	PGFdayyshift,
	PGFmonthxshift,
	PGFmonthyshift,
	PGFdaycode,
	PGFdaytext,
	PGFmonthcode,
	PGFmonthtext,
	PGFyearcode,
	PGFyeartext,
	PGFdaylistdownward,
	PGFdaylistupward,
	PGFdaylistright,
	PGFdaylistleft,
	PGFWeeklist,
	PGFmonthlist,
	PGFmonthlabelleft,
	PGFmonthlabelleftvertical,
	PGFmonthlabelright,
	PGFmonthlabelrightvertical,
	PGFmonthlabelaboveleft,
	PGFmonthlabelabovecentered,
	PGFmonthlabelaboveright,
	PGFmonthlabelbelowleft,
	PGFmonthlabelbelowcentered,
	PGFif,
	PGFelse,

	//chain
	PGFstartchain,
	PGFchaindefaultdirection,
	PGFcontinuechain,
	PGFonchain,
	PGFjoin,
	PGFstartbranch,
	PGFcontinuebranch,

	//circuit
	PGFcircuits,
	PGFcircuitsymbolunit,
	PGFHugecircuitsymbols,
	PGFLargecircuitsymbols,
	PGFmediumcircuitsymbols,
	PGFsmallcircuitsymbols,
	PGFtinycircuitsymbols,
	PGFcircuitsymbolsize,
	PGFcircuitdeclaresymbol,
	PGFcircuithandlesymbol,
	PGFpointup,
	PGFpointdown,
	PGFpointleft,
	PGFpointright,
	PGFinfo,
	PGFinfomissingangle,
	PGFinfosloped,
	PGFinfomissinganglesloped,
	PGFcircuitdeclareunit,
	PGFcircuitdeclareannotation,
	PGFannotationarrow,
	PGFcircuitsymbolopen,
	PGFcircuitsymbolfilled,
	PGFcircuitsymbollines,
	PGFcircuitsymbolwires,

	//logic circuit
	PGFcircuitlogic,
	PGFcircuitlogicIEC,
	PGFcircuitlogicUS,
	PGFcircuitlogicCDH,
	PGFinputs,
	PGFlogicgateinputs,
	PGFandgate,
	PGFnandgate,
	PGForgate,
	PGFnorgate,
	PGFxorgate,
	PGFxnorgate,
	PGFnotgate,
	PGFbuffergate,
	PGFlogicgateinvertedradius,
	PGFlogicgateinputsep,
	PGFlogicgateanchorsuseboundingbox,
	PGFandgateIECsymbol,
	PGFnandgateIECsymbol,
	PGForgateIECsymbol,
	PGFnorgateIECsymbol,
	PGFxorgateIECsymbol,
	PGFxnorgateIECsymbol,
	PGFnotgateIECsymbol,
	PGFbuffergateIECsymbol,
	PGFlogicgateIECsymbolalign,
	PGFlogicgateIECsymbolcolor,

	//electrical engineering circuits
	PGFcircuitEE,
	PGFcircuitEEIEC,
	PGFcurrentdirection,
	PGFcurrentdirectionreversed,
	PGFresistor,
	PGFinductor,
	PGFcapacitor,
	PGFbattery,
	PGFbulb,
	PGFcurrentsource,
	PGFvoltagesource,
	PGFground,
	PGFdiode,
	PGFzenerdiode,
	PGFschottkydiode,
	PGFtunneldiode,
	PGFbackwarddiode,
	PGFbreakdowndiode,
	PGFcontact,
	PGFmakecontact,
	PGFbreakcontact,
	PGFampere,
	PGFvolt,
	PGFohm,
	PGFohmtheotherside,
	PGFohmsloped,
	PGFohmtheothersidesloped,
	PGFsiemens,
	PGFhenry,
	PGFfarad,
	PGFcoulomb,
	PGFvoltampere,
	PGFwatt,
	PGFhertz,
	PGFlightemitting,
	PGFlightemittingtheotherside,
	PGFlightDependent,
	PGFdirectioninfo,
	PGFadjustable,
	PGFdirectionEEarrow,
	PGFgenericcircleIECbeforebackground,
	PGFgenericdiodeIECbeforebackground,

	//entity
	PGFentity,
	PGFrelationship,
	PGFKeyattribute,

	//externalization
	PGFsystemcall,
	PGFshellescape,
	//PGFprefix
	PGFexternalprefix,
	PGFfigurename,
	PGFforceremake,
	PGFremakenext,
	PGFexportnext,
	PGFexport,
	PGFfigurelist,
	PGFmode,
	PGFverboseIO,
	PGFverboseoptimize,
	PGFverbose,
	PGFoptimize,
	PGFoptimizecommandaway,
	PGFextoptinstall,
	PGFextoptrestore,
	PGFonlynamed,
	PGFincludeexternal,
	PGFexternalinfo,

	//fit
	PGFfit,
	PGFrotatefit,

	//fixed point
	PGFfixedpointarithmetic,
	PGFscaleresults,
	PGFscalefileplotx,
	PGFscalefileploty,
	PGFscalefileplotz,

	//FPU
	PGFfpu,
	PGFfpuoutputformat,
	PGFfpuscaleResults,
	PGFfpuscalefileplotx,
	PGFfpuscalefileploty,
	PGFfpuscalefileplotz,
	PGFfpuhandlersemptynumber,
	PGFfpuhandlersinvalidnumber,
	PGFfpuhandlerswronglowlevelformat,
	PGFfpurelthresh,

	//l-systems
	//PGFstep
	PGFrandomizesteppercent,
	PGFleftangle,
	PGFrightangle,
	PGFrandomizeanglepercent,
	//PGFLindenmayersystem
	//PGFlsystem
	//name
	PGFaxiom,
	PGForder,
	PGFruleset,
	//PGFanchor

	//mindmap
	PGFmindmap,
	PGFsmallmindmap,
	PGFlargemindmap,
	PGFhugemindmap,
	PGFconcept,
	PGFconceptcolor,
	PGFextraconcept,
	PGFrootconcept,
	PGFrootconceptappend,
	PGFlevel1concept,
	PGFlevel1conceptappend,
	PGFlevel2concept,
	PGFlevel2conceptappend,
	PGFlevel3concept,
	PGFlevel3conceptappend,
	PGFlevel4concept,
	PGFlevel4conceptappend,
	PGFconceptconnection,
	PGFcircleconnectionbar,
	PGFcircleconnectionbarswitchcolor,
	PGFannotation,

	//paper folding
	PGFfoldinglinelength,
	PGFface,
	PGFface1,
	PGFface2,
	PGFface3,
	PGFface12,

	//petri
	PGFplace,
	PGFtransition,
	//PGFpre
	//PGFpost
	PGFpreandpost,
	PGFtoken,
	PGFchildrenaretokens,
	PGFtokendistance,
	PGFtokens,
	PGFcoloredtokens,
	PGFstructuredtokens,

	//shadow
	PGFgeneralshadow,
	PGFshadowscale,
	PGFshadowxshift,
	PGFshadowyshift,
	PGFdropshadow,
	PGFcopyshadow,
	PGFdoublecopyshadow,
	PGFcirculardropshadow,
	PGFcircularglow,

	//shape
	//PGFaspect
	PGFtrapeziumleftangle,
	PGFtrapeziumrightangle,
	PGFtrapeziumangle,
	PGFtrapeziumstretches,
	PGFtrapeziumstretchesbody,
	PGFregularpolygonsides,
	PGFstarpoints,
	PGFstarpointheight,
	PGFstarpointratio,
	PGFisoscelestriangleapexangle,
	PGFisoscelestrianglestretches,
	PGFkiteuppervertexangle,
	PGFkitelowervertexangle,
	PGFkitevertexangles,
	PGFdarttipangle,
	PGFdarttailangle,
	PGFcircularsectorangle,
	PGFcylinderusescustomfill,
	PGFcylinderendfill,
	PGFcylinderbodyfill,
	PGFMGHAfill,
	PGFMGHAaspect,
	PGFcloudpuffs,
	PGFcloudpuffarc,
	PGFcloudignoresaspect,
	PGFstarburstpoints,
	PGFstarburstpointheight,
	PGFrandomstarburst,
	PGFsignalpointerangle,
	PGFsignalfrom,
	PGFsignalto,
	PGFtapebendtop,
	PGFtapebendbottom,
	PGFtapebendheight,
	PGFmagnetictapetailextend,
	PGFmagnetictapetail,
	PGFsinglearrowtipangle,
	PGFsinglearrowheadextend,
	PGFsinglearrowheadindent,
	PGFdoublearrowtipangle,
	PGFdoublearrowheadextend,
	PGFdoublearrowheadindent,
	PGFarrowboxtipangle,
	PGFarrowboxheadextend,
	PGFarrowboxheadindent,
	PGFarrowboxshaftwidth,
	PGFarrowboxnortharrow,
	PGFarrowboxsoutharrow,
	PGFarrowboxeastarrow,
	PGFarrowboxwestarrow,
	PGFarrowboxarrows,
	PGFrectanglesplitallocateboxes,
	PGFrectanglesplitparts,
	PGFrectanglesplithorizontal,
	PGFrectanglesplitignoreemptyparts,
	PGFrectanglesplitemptypartwidth,
	PGFrectanglesplitemptypartheight,
	PGFrectanglesplitemptypartdepth,
	PGFrectanglesplitpartalign,
	PGFrectanglesplitdrawsplits,
	PGFrectanglesplitusecustomfill,
	PGFrectanglesplitpartfill,
	PGFcalloutrelativepointer,
	PGFcalloutabsolutepointer,
	PGFcalloutpointershorten,
	PGFcalloutpointerwidth,
	PGFcalloutpointerarc,
	PGFcalloutpointerstartsize,
	PGFcalloutpointerendsize,
	PGFcalloutpointersegments,
	PGFroundedrectanglearclength,
	PGFroundedrectanglewestarc,
	PGFroundedrectangleleftarc,
	PGFroundedrectangleeastarc,
	PGFroundedrectanglerightarc,
	PGFchamferedrectangleangle,
	PGFchamferedrectanglexsep,
	PGFchamferedrectangleysep,
	PGFchamferedrectanglesep,
	PGFchamferedrectanglecorners,

	//shape
	//PGFcircle
	//PGFellipse
	//PGFrectangle
	//PGFcoordinate
	PGFdiamond,
	PGFtrapezium,
	PGFsemicircle,
	PGFregularpolygon,
	PGFstar,
	PGFisoscelestriangle,
	PGFkite,
	PGFdart,
	PGFcircularsector,
	PGFcylinder,
	PGFforbiddensign,
	PGFcorrectforbiddensign,
	PGFmagnifyingglass,
	PGFmagnetictape,
	PGFcloud,
	PGFstarburst,
	PGFsignal,
	PGFtape,
	PGFsinglearrow,
	PGFdoublearrow,
	PGFarrowbox,
	PGFcirclesplit,
	PGFcirclesolidus,
	PGFellipsesplit,
	PGFrectanglesplit,
	PGFrectanglecallout,
	PGFellipsecallout,
	PGFcloudcallout,
	PGFcrossout,
	PGFstrikeout,
	PGFroundedrectangle,
	PGFchamferedrectangle,
	PGFandgateUS,
	PGFandgateCDH,
	PGFnandgateUS,
	PGFnandgateCDH,
	PGForgateUS,
	PGFnorgateUS,
	PGFxorgateUS,
	PGFxnorgateUS,
	PGFnotgateUS,
	PGFbuffergateUS,
	PGFandgateIEC,
	PGFnandgateIEC,
	PGForgateIEC,
	PGFnorgateIEC,
	PGFxorgateIEC,
	PGFxnorgateIEC,
	PGFnotgateIEC,
	PGFbuffergateIEC,
	PGFrectangleEE,
	PGFcircleEE,
	PGFdirectionEE,
	PGFgenericcircleIEC,
	PGFgenericdiodeIEC,
	PGFbreakdowndiodeIEC,
	PGFvarresistorIEC,
	PGFinductorIEC,
	PGFcapacitorIEC,
	PGFbatteryIEC,
	PGFgroundIEC,
	PGFmakecontactIEC,
	PGFvarmakecontactIEC,
	PGFbreakcontactIEC,

	//spy
	PGFspyscope,
	PGFsize,
	PGFheight,
	PGFwidth,
	PGFlens,
	PGFmagnification,
	PGFspyconnectionpath,
	PGFspyusingoutlines,
	PGFspyusingoverlays,
	PGFconnectspies,

	PGFcirclethrough,

	//to
	PGFtopath,
	PGFtolineto,
	PGFtomoveto,
	PGFtocurveto,
	PGFout,
	PGFin,
	PGFrelative,
	PGFbendleft,
	PGFbendright,
	PGFbendangle,
	PGFlooseness,
	PGFoutlooseness,
	PGFinlooseness,
	PGFmindistance,
	PGFmaxdistance,
	PGFoutmindistance,
	PGFoutmaxdistance,
	PGFinmindistance,
	PGFinmaxdistance,
	PGFdistance,
	PGFoutdistance,
	PGFindistance,
	PGFoutcontrol,
	PGFincontrol,
	PGFcontrols,
	PGFloop,
	PGFloopabove,
	PGFloopbelow,
	PGFloopleft,
	PGFloopright,

	PGFturtle,
	PGFhome,
	PGFforward,
	PGFhow,
	PGFfd,
	PGFback,
	PGFbk,
	PGFlt,
	PGFrt,

	//foreach
	PGFvar,
	PGFevaluate,
	PGFremember,
	PGFcount,

	PGFhandlerconfig,
	PGFonlyexistingaddexception,

	PGFutilsexec,

	PGFerrvaluerequired,
	PGFerrvalueforbidden,
	PGFerrbooleanexpected,
	PGFerrunknownchoicevalue,
	PGFerrunknownkey,

	PGFkeyfilterhandlersappendfilteredto,
	PGFkeyfilterhandlersignore,
	PGFkeyfilterhandlerslog,

	PGFkeyfiltersactivefamilies,
	PGFkeyfiltersactivefamiliesornofamily,
	PGFkeyfiltersactivefamiliesornofamilydebug,
	PGFkeyfiltersactivefamiliesandknown,
	PGFkeyfiltersactivefamiliesordescendantsof,
	PGFkeyfiltersisdescendantof,
	PGFkeyfiltersequals,
	PGFkeyfiltersnot,
	PGFkeyfiltersand,
	PGFkeyfiltersor,
	PGFkeyfilterstrue,
	PGFkeyfiltersfalse,
	PGFkeyfiltersdefined,

	PGFresizeto,
	PGF2On1,
	PGF4On1,
	PGF8On1,
	PGF16On1,
	PGFtwoscreenswithlaggingsecond,
	PGFtwoscreenswithoptionalsecond,

	PGFphysicalpaperheight,
	PGFphysicalpaperwidth,
	PGFa0paper,
	PGFa1paper,
	PGFa2paper,
	PGFa3paper,
	PGFa4paper,
	PGFa5paper,
	PGFa6paper,
	PGFletterpaper,
	PGFlegalpaper,
	PGFexecutivepaper,
	PGFlandscape,
	PGFbordershrink,
	PGFoddnumberedpagesright,
	PGFcornerwidth,
	PGFsecondright,
	PGFsecondleft,
	PGFsecondbottom,
	PGFsecondtop,

	PGFlogicalpages,
	PGFfirstlogicalshipout,
	PGFlastlogicalshipout,
	PGFcurrentlogicalshipout,
	PGFphysicalheight,
	PGFphysicalwidth,

	PGFcenter,
	PGFresizedwidth,
	PGFresizedheight,
	PGForiginalwidth,
	PGForiginalheight,
	PGFrotation,
	PGFcopyfrom,
	PGFbordercode,

	PGFdeclarefunction,

	PGFnumberformat,
	PGFfixed,
	PGFfixedzerofill,
	PGFsci,
	PGFscizerofill,
	PGFzerofill,
	PGFstd,
	PGFintdetect,
	PGFinttrunc,
	PGFnumberfrac,
	PGFfrac,
	PGFfractex,
	PGFfracdenom,
	PGFfracwhole,
	PGFfracshift,
	PGFprecision,
	PGFsciprecision,
	PGFsetdecimalseparator,
	PGFdecsep,
	PGFsetthousandsseparator,
	PGF1000sep,
	PGFminexpfor1000sep,
	PGFuseperiod,
	PGFusecomma,
	PGFskip0dot,
	PGFshowpos,
	PGFprintsign,
	PGFsci10e,
	PGFsci10expe,
	PGFscie,
	PGFsciE,
	PGFscisubscript,
	PGFscisuperscript,
	PGFscigeneric,
	PGFmantissasep,
	PGFexponent,
	PGFatdecsepmark,
	PGFatsciexponentmark,
	PGFassumemathmode,
	PGFverbatim,

	PGFtop,
	PGFbottom,
	PGFbase,
	PGFmid,
	PGFnorth,
	PGFsouth,
	PGFwest,
	PGFmidwest,
	PGFbasewest,
	PGFnorthwest,
	PGFsouthwest,
	PGFeast,
	PGFmideast,
	PGFbaseeast,
	PGFnortheast,
	PGFsoutheast,
	PGFtaileast,
  PGFtailsoutheast,
  PGFtailnortheast,
	PGFbottomleftcorner,
  PGFtopleftcorner,
  PGFtoprightcorner,
  PGFbottomrightcorner,
  PGFleftside,
  PGFrightside,
  PGFtopside,
  PGFbottomside,
	PGFapex,
	PGFarcstart,
  PGFarcend,
  PGFchordcenter,
	PGFleftcorner,
  PGFrightcorner,
	PGFlowerside,
	PGFuppervertex,
  PGFlowervertex,
  PGFleftvertex,
  PGFrightvertex,
  PGFupperleftside,
  PGFlowerleftside,
  PGFupperrightside,
  PGFlowerrightside,
	PGFtip,
  PGFlefttail,
  PGFrighttail,
  PGFtailcenter,
	PGFsectorcenter,
  PGFarccenter,
	PGFshapecenter,
	PGFbeforetop,
  PGFaftertop,
  PGFbeforebottom,
  PGFafterbottom,
	PGFpointer,
	PGFbeforehead,
  PGFbeforetip,
  PGFaftertip,
  PGFafterhead,
  PGFbeforetail,
  PGFaftertail,
	PGFtail,
	PGFbeforehead1,
  PGFbeforetip1,
  PGFtip1,
  PGFaftertip1,
  PGFafterhead1,
  PGFbeforehead2,
  PGFbeforetip2,
  PGFtip2,
  PGFaftertip2,
  PGFafterhead2,
	PGFbeforeeastarrow,
  PGFbeforeeastarrowhead,
  PGFbeforeeastarrowtip,
  PGFeastarrowtip,
  PGFaftereastarrowtip,
  PGFaftereastarrowhead,
  PGFaftereastarrow,
  PGFbeforewestarrow,
  PGFbeforewestarrowhead,
  PGFbeforewestarrowtip,
  PGFwestarrowtip,
  PGFafterwestarrowtip,
  PGFafterwestarrowhead,
  PGFafterwestarrow,
  PGFbeforenortharrow,
  PGFbeforenortharrowhead,
  PGFbeforenortharrowtip,
  PGFnortharrowtip,
  PGFafternortharrowtip,
  PGFafternortharrowhead,
  PGFafternortharrow,
  PGFbeforesoutharrow,
  PGFbeforesoutharrowhead,
  PGFbeforesoutharrowtip,
  PGFsoutharrowtip,
  PGFaftersoutharrowtip,
  PGFaftersoutharrowhead,
  PGFaftersoutharrow,
	PGFbeforenortheast,
  PGFafternortheast,
  PGFbeforenorthwest,
  PGFafternorthwest,
  PGFbeforesouthwest,
  PGFaftersouthwest,
  PGFbeforesoutheast,
  PGFaftersoutheast,
	PGFchamferall,
	PGFchamfernone,
	PGFlower,
	PGFnowhere,
	PGFnone,
	PGFinandout,
	PGFoutandin,
	PGFconcave,
	PGFconvex,
  PGFup,
	PGFdown,
	PGFstepx,
	PGFstepy,

	PGFdecorationautomaton,
	PGFswitchiflessthan,
	PGFswitchifinputsegmentlessthan,
	PGFrepeatstate,
	PGFnextstate,
	PGFifinputsegmentisclosepath,
	PGFautoendonlength,
	PGFautocorneronlength,
	PGFpersistentprecomputation,
	PGFpersistentpostcomputation,

	PGFlocalboundingbox,

	PGFpage,
	PGFinterpolate,

	PGFcolormixin,

	PGFdotcd,
	PGFdotisfamily,
	PGFdotsearchalso,
	PGFdotdefault,
	PGFdotvaluerequired,
	PGFdotvalueforbidden,

	PGFdotcode,
	PGFecode,
	PGFdotcode2args,
	PGFdotecode2args,
	PGFdotcodenargs,
	PGFdotecodenargs,
	PGFdotcodeargs,
	PGFdotecodeargs,
	PGFdotaddcode,
	PGFdotprefixcode,
	PGFdotappendcode,

	PGFdotstyle,
	PGFdotestyle,
	PGFdotstyle2args,
	PGFdotestyle2args,
	PGFdotstyleargs,
	PGFdotestyleargs,
	PGFdotstylenargs,
	PGFdotaddstyle,
	PGFdotprefixstyle,
	PGFdotappendstyle,

	PGFdotinitial,
	PGFdotget,
	PGFdotadd,
	PGFdotprefix,
	PGFdotappend,
	PGFdotlink,
	PGFdotstorein,
	PGFdotestorein,
	PGFdotisif,
	PGFdotischoice,

	PGFdotexpandonce,
	PGFdotexpandtwice,
	PGFdotexpanded,
	PGFdotlist,
	PGFdottry,
	PGFdotretry,
	PGFdotlastretry,

	PGFdotshowvalue,
	PGFdotshowcode,

	PGFinstallkeyfilter,
	PGFinstallkeyfilterhandler,

	PGFdotactivatefamily,
	PGFdotdeactivatefamily,
	PGFdotbelongstofamily,

	PGFlinetoo,
	PGFstraightzigzag,
	PGFrandomsteps,
	PGFsaw,
	PGFzigzag,
	PGFbent,
	PGFbumps,
	PGFcoil,
	PGFcurvetoo,
	PGFsnake,
	PGFborder,
	PGFbrace,
	PGFexpandingwaves,
	PGFmoveto,
	PGFticks,
	PGFwaves,
	PGFshowpathconstruction,
	PGFmarkings,
	PGFfootprints,
	PGFcrosses,
	PGFtriangles,
	PGFshapebackgrounds,
	PGFtextalongpath,
	PGFkochcurvetype1,
	PGFkochcurvetype2,
	PGFkochsnowflake,
	PGFcantorset,

	//no keyword
	PGFcoord,
	PGFevery,
	PGFoptions,
	PGFnodetext,
	PGFchildpath,
	PGFoptionparam,
	PGFlsysspec,
	PGFstates,
	PGFshapespec,
	PGFpointgroup,
	PGFkeylist,
	PGFoomethod,
	PGFooobjmethod,
	PGFforeachvariables,
	PGFforeachvalues,
	PGFmatrixcolumn,
	PGFmatrixrow,
	PGFmatrixgroup,
	PGFspyon,
	PGFspyinnode
};

#endif //PGFKEYWORD_H
