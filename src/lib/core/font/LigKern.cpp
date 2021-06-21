/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include "LigKern.h"

char *staticligkern[] =
{
  "% LIGKERN space l =: lslash ; space L =: Lslash ;",
  "% LIGKERN question quoteleft =: questiondown ;",
  "% LIGKERN exclam quoteleft =: exclamdown ;",
  "% LIGKERN hyphen hyphen =: endash ; endash hyphen =: emdash ;",
  "% LIGKERN quoteleft quoteleft =: quotedblleft ;",
  "% LIGKERN quoteright quoteright =: quotedblright ;",
  "% LIGKERN space {} * ; * {} space ; zero {} * ; * {} zero ;",
  "% LIGKERN one {} * ; * {} one ; two {} * ; * {} two ;",
  "% LIGKERN three {} * ; * {} three ; four {} * ; * {} four ;",
  "% LIGKERN five {} * ; * {} five ; six {} * ; * {} six ;",
  "% LIGKERN seven {} * ; * {} seven ; eight {} * ; * {} eight ;",
  "% LIGKERN nine {} * ; * {} nine ;",

  /*
   *   Kern accented characters the same way as their base.
   */

  "% LIGKERN Aacute <> A ; aacute <> a ;",
  "% LIGKERN Acircumflex <> A ; acircumflex <> a ;",
  "% LIGKERN Adieresis <> A ; adieresis <> a ;",
  "% LIGKERN Agrave <> A ; agrave <> a ;",
  "% LIGKERN Aring <> A ; aring <> a ;",
  "% LIGKERN Atilde <> A ; atilde <> a ;",
  "% LIGKERN Ccedilla <> C ; ccedilla <> c ;",
  "% LIGKERN Eacute <> E ; eacute <> e ;",
  "% LIGKERN Ecircumflex <> E ; ecircumflex <> e ;",
  "% LIGKERN Edieresis <> E ; edieresis <> e ;",
  "% LIGKERN Egrave <> E ; egrave <> e ;",
  "% LIGKERN Iacute <> I ; iacute <> i ;",
  "% LIGKERN Icircumflex <> I ; icircumflex <> i ;",
  "% LIGKERN Idieresis <> I ; idieresis <> i ;",
  "% LIGKERN Igrave <> I ; igrave <> i ;",
  "% LIGKERN Ntilde <> N ; ntilde <> n ;",
  "% LIGKERN Oacute <> O ; oacute <> o ;",
  "% LIGKERN Ocircumflex <> O ; ocircumflex <> o ;",
  "% LIGKERN Odieresis <> O ; odieresis <> o ;",
  "% LIGKERN Ograve <> O ; ograve <> o ;",
  "% LIGKERN Oslash <> O ; oslash <> o ;",
  "% LIGKERN Otilde <> O ; otilde <> o ;",
  "% LIGKERN Scaron <> S ; scaron <> s ;",
  "% LIGKERN Uacute <> U ; uacute <> u ;",
  "% LIGKERN Ucircumflex <> U ; ucircumflex <> u ;",
  "% LIGKERN Udieresis <> U ; udieresis <> u ;",
  "% LIGKERN Ugrave <> U ; ugrave <> u ;",
  "% LIGKERN Yacute <> Y ; yacute <> y ;",
  "% LIGKERN Ydieresis <> Y ; ydieresis <> y ;",
  "% LIGKERN Zcaron <> Z ; zcaron <> z ;",

  /* lig commands for default ligatures */

  "% LIGKERN f i =: fi ; f l =: fl ; f f =: ff ; ff i =: ffi ;",
  "% LIGKERN ff l =: ffl ;",
  NULL
};

char *vplligops[] =
{
  "LIG", "/LIG", "/LIG>", "LIG/", "LIG/>", "/LIG/", "/LIG/>", "/LIG/>>", 0
};

char *encligops[] =
{
  "=:", "|=:", "|=:>", "=:|", "=:|>", "|=:|", "|=:|>", "|=:|>>", 0
};
