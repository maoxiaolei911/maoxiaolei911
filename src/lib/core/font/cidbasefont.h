/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWCIDBASEFONT_H
#define XWCIDBASEFONT_H


const struct {
  const char *fontname;
  const char *fontdict;
  const char *descriptor;
} cid_basefont[] = {
  /*
   *  Fonts found in printers
   *
   *  Ryumin-Light and GothicBBB-Medium is often available in
   *  PostScript printer sold in Japan. There may be additonal
   *  Morisawa fonts such as ShinGo-Bold, but less common than
   *  those two fonts.
   */
  {
    "Ryumin-Light",
    "<< \
/Subtype/CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (Japan1) /Supplement 2 >> \
/DW 1000 \
/W [\
  231   632 500 \
 8718 [500 500] \
]\
>>",
    "<< \
/CapHeight 709 /Ascent 723 /Descent -241 /StemV 69 \
/FontBBox [-170 -331 1024 903] \
/ItalicAngle 0 /Flags 6 \
/Style << /Panose <010502020300000000000000> >> \
>>"
  },
  {
    "GothicBBB-Medium",
    "<< \
/Subtype/CIDFontType0 \
/CIDSystemInfo <<  /Registry (Adobe) /Ordering (Japan1) /Supplement 2 >> \
/DW 1000 \
/W [\
  231   632 500 \
 8718 [500 500] \
]\
>>",
    "<< \
/CapHeight 737 /Ascent 752 /Descent -271 /StemV 99 \
/FontBBox [-174 -268 1001 944] \
/ItalicAngle 0 /Flags 4 \
/Style << /Panose <0801020b0500000000000000> >> \
>>"
  },

  /* Adobe Asian Font Packs for Acrobat Reader 4 */
  {
    "MHei-Medium-Acro",
    "<< \
/Subtype /CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (CNS1) /Supplement 0 >> \
/DW 1000 \
/W [\
13648 13742 500 \
17603 [500] \
]\
>>",
    "<< \
/Ascent 752 /CapHeight 737 /Descent -271 /StemV 58 \
/FontBBox [-45 -250 1015 887] \
/ItalicAngle 0 /Flags 4 /XHeight 553 \
/Style << /Panose <000001000600000000000000> >> \
>>"
  },
  {
    "MSung-Light-Acro",
    "<< \
/Subtype /CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (CNS1) /Supplement 0 >> \
/DW 1000 \
/W [\
13648 13742 500 \
17603 [500] \
]\
>>",
    "<< \
/Ascent 752 /CapHeight 737 /Descent -271 /StemV 58 \
/FontBBox [-160 -259 1015 888] \
/ItalicAngle 0 /Flags 6 /XHeight 553 \
/Style << /Panose <000000000400000000000000> >> \
>>"
  },
  {
    "STSong-Light-Acro",
    "<< \
/Subtype /CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (GB1) /Supplement 2 >> \
/DW 1000 \
/W [\
  814 939 500 \
 7716 [500] \
22355 [500 500] \
22357 [500] \
]\
>>",
    "<< \
/Ascent 857 /CapHeight 857 /Descent -143 /StemV 91 \
/FontBBox [-250 -143 600 857] \
/ItalicAngle 0 /Flags 6 /XHeight 599 \
/Style << /Panose <000000000400000000000000> >> \
>>"
  },
  {
    "HeiseiKakuGo-W5-Acro",
    "<< \
/Subtype /CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (Japan1) /Supplement  2 >> \
/DW 1000 \
/W [\
  231   632 500 \
 8718 [500 500] \
]\
>>",
    "<< \
/Ascent 752 /CapHeight 737 /Descent -221 /StemV 114 \
/FontBBox [-92 -250 1010 922] \
/ItalicAngle 0 /Flags 4 /XHeight 553 \
/Style << /Panose <000001000500000000000000> >> \
>>"
  },
  {
    "HeiseiMin-W3-Acro",
    "<< \
/Subtype /CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (Japan1) /Supplement 2 >> \
/DW 1000 \
/W [\
  231   632 500 \
 8718 [500 500] \
]\
>>",
    "<< \
/Ascent 723 /CapHeight 709 /Descent -241 /StemV 69 \
/FontBBox [-123 -257 1001 910] \
/ItalicAngle 0 /Flags 6 /XHeight 450 \
/Style << /Panose <000002020500000000000000> >> \
>>"
  },
  {
    "HYGoThic-Medium-Acro",
    "<< \
/Subtype /CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (Korea1) /Supplement 1 >> \
/DW 1000 \
/W [\
   97 [500] \
 8094  8190 500 \
]\
>>",
    "<< \
/Ascent 752 /CapHeight 737 /Descent -271 /StemV 58 \
/FontBBox [-6 -145 1003 880] \
/ItalicAngle 0 /Flags 4 /XHeight 553 \
/Style << /Panose <000001000600000000000000> >> \
>>"
  },
  {
    "HYSMyeongJo-Medium-Acro",
    "<< \
/Subtype /CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (Korea1) /Supplement 1 >> \
/DW 1000 \
/W [\
   97 [500] \
 8094  8190 500 \
]\
>>",
    "<< \
/Ascent 752 /CapHeight 737 /Descent -271 /StemV 58 \
/FontBBox [-0 -148 1001 880] \
/ItalicAngle 0 /Flags 6 /XHeight 553 \
/Style << /Panose <000000000600000000000000> >> \
>>"
  },

  /* Acrobat Reader 5 */
  {
    "MSungStd-Light-Acro",
    "<< \
/Subtype /CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (CNS1) /Supplement 4 >> \
/DW 1000 \
/W [\
13648 13742 500 \
17603 [500] \
]\
>>",
    "<< \
/Ascent 880 /CapHeight 662 /Descent -120 /StemV 54 \
/FontBBox [-160 -249 1015 1071] \
/ItalicAngle 0 /Flags 6 \
/Style << /Panose <000000000400000000000000> >> \
>>"
  },
  {
    "STSongStd-Light-Acro",
    "<< \
/Subtype /CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (GB1) /Supplement 4 >> \
/DW 1000 \
/W [\
  814 939 500 \
 7716 [500] \
22355 [500 500] \
22357 [500] \
]\
>>",
    "<< \
/Ascent 880 /CapHeight 626 /Descent -120 /StemV 44 \
/FontBBox [-134 -254 1001 905] \
/ItalicAngle 0 /Flags 6 \
/Style << /Panose <000000000400000000000000> >> \
>>"
  },
  {
    "HYSMyeongJoStd-Medium-Acro",
    "<< \
/Subtype /CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (Korea1) /Supplement 2 >> \
/DW 1000 \
/W [\
   97 [500] \
 8094  8190 500 \
]\
>>",
    "<< \
/Ascent 880 /CapHeight 720 /Descent -120 /StemV 60 \
/FontBBox [-28 -148 1001 880] \
/ItalicAngle 0 /Flags 6 \
/Style << /Panose <000000000600000000000000> >> \
>>"
  },

  /* Adobe Asian Font Packs for Adobe Reader 6 */

  /*
   * Widths of Adobe-CNS1-4
   *     1    95  proportional
   * 13648 13742  half-width
   * 17601 17601  proportional?
   * 17603 17603  half-width?
   */
  {
    "AdobeMingStd-Light-Acro",
    "<< \
/Subtype/CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (CNS1) /Supplement 4 >> \
/DW 1000 \
/W [\
13648 13742 500 \
17603 [500] \
]\
>>",
    "<< \
/Ascent 880 /Descent -120 /StemV 48 /CapHeight 731 \
/FontBBox [-38 -121 1002 918] \
/ItalicAngle 0 /Flags 6 /XHeight 466 \
/Style << /Panose <000002020300000000000000> >> \
>>"
  },
  /*
   * Widths of Adobe-GB1-4
   *     1    95  proportional
   *   814   939  half-width
   *  7712  7715  proportional
   *  7716  7716  half-width space
   * 22353 22354  proportional
   * 22355 22357  half-width
   */
  {
    "AdobeSongStd-Light-Acro",
    "<< \
/Subtype/CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (GB1) /Supplement 4 >> \
/DW 1000 \
/W [\
  814 939 500 \
 7716 [500] \
22355 [500 500] \
22357 [500] \
]\
>>",
    "<< \
/Ascent 880 /Descent -120 /StemV 66 /CapHeight 626 \
/FontBBox [-134 -254 1001 905] \
/ItalicAngle 0 /Flags 6 /XHeight 416 \
/Style << /Panose <000002020300000000000000> >> \
>>"
  },
  /*
   * Widths of Adobe-Japan1-4
   *     1   230  proportional
   *   231   632  half-width
   *  9354  9737  proportional
   *  9738  9757  quater-width
   *  9758  9778  third-width
   * 12063 12087  half-width
   */
  {
    "KozMinPro-Regular-Acro",
    "<< \
/Subtype/CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (Japan1) /Supplement 4 >> \
/DW 1000 \
/W [\
  231   632 500 \
 8718 [500 500] \
 9738  9757 250 \
 9758  9778 333 \
12063 12087 500 \
]\
>>",
    "<< \
/Ascent 880 /Descent -120 /StemV 86 /CapHeight 740 \
/FontBBox [-195 -272 1110 1075] \
/ItalicAngle 0 /Flags 6 /XHeight 502 \
/Style << /Panose <000002020400000000000000> >> \
>>"
  },
  {
    "KozGoPro-Medium-Acro",
    "<< \
/Subtype/CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering(Japan1) /Supplement 4 >> \
/DW 1000 \
/W [\
  231   632 500 \
 8718 [500 500] \
 9738  9757 250 \
 9758  9778 333 \
12063 12087 500 \
]\
>>",
    "<< \
/Ascent 880 /Descent -120 /StemV 99 /CapHeight 763 \
/FontBBox [-149 -374 1254 1008] \
/ItalicAngle 0 /Flags 4 /XHeight 549 \
/Style << /Panose <0000020b0700000000000000> >> \
>>"
  },
  /*
   * Widths of Adobe-Korea1-2
   *     1    95  proportional
   *    97    97  half-width?
   *  8094  8190  half-width
   */
  {
    "AdobeMyungjoStd-Medium-Acro",
    "<< \
/Subtype/CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (Korea1) /Supplement 2 >> \
/DW 1000 \
/W [\
   97 [500] \
 8094  8190 500 \
]\
>>",
    "<< \
/Ascent 880 /Descent -120 /StemV 99 /CapHeight 719 \
/FontBBox [-28 -148 1001 880] \
/ItalicAngle 0 /Flags 6 /XHeight 478 \
/Style << /Panose <000002020600000000000000> >> \
>>"
  },

  /* Addition in Adobe Reader 7 */
  /*
   *  Adobe-Japan1-5
   *   15449-15911 proportional
   *   15912-15975 proportional
   *   15976-16192 proportional (vert)
   *   16469-16714 pre-rotated proportional
   *   16715-16778 pre-rotated proportional
   *  Adobe-Japan1-6
   *   20317-20371 proportional
   *   20372-20426 proportional
   *   20958       - (reserved for future use)
   *   20961-21015 pre-rotated proportional
   *   21016-21070 pre-rotated proportional
   */
  {
    "KozMinProVI-Regular",
    "<< \
/Subtype/CIDFontType0 \
/CIDSystemInfo << \
  /Registry (Adobe) \
  /Ordering (Japan1) \
  /Supplement 6 \
>> \
/DW 1000 \
/W [\
  231   632 500 \
  8718 [500 500] \
  9738  9757 250 \
  9758  9778 333 \
  12063 12087 500 \
]\
        >>",
    "<< \
/Ascent 880 /Descent -120 /StemV 86 /CapHeight 742 \
/FontBBox [-437 -340 1144 1317] \
/ItalicAngle 0 /Flags 6 /XHeight 503 \
/Style << \
  /Panose <000002020400000000000000> \
>> \
     >>"
  },

  /* Addition in Adobe Reader 8 */
  /*
   * Font information of AdobeHeitiStd-Regular.otf may not be correct!
   * Just copied the information of AdobeSongStd-Light.otf.
   */
  {
    "AdobeHeitiStd-Regular",
    "<< \
/Subtype/CIDFontType0 \
/CIDSystemInfo << /Registry (Adobe) /Ordering (GB1) /Supplement 4 >> \
/DW 1000 \
/W [\
  814 939 500 \
 7716 [500] \
22355 [500 500] \
22357 [500] \
]\
>>",
    "<< \
/Ascent 880 /Descent -120 /StemV 66 /CapHeight 626 \
/FontBBox [-134 -254 1001 905] \
/ItalicAngle 0 /Flags 6 /XHeight 416 \
/Style << /Panose <000002020300000000000000> >> \
>>"
  },
  /* END */
  {NULL, NULL, NULL}
};

#endif // XWCIDBASEFONT_H
