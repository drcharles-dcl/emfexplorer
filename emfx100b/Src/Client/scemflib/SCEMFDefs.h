/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFDEFS_H_
#define _SCEMFDEFS_H_

// Merged from wingdi
#define SCEMF_HEADER                      1
#define SCEMF_POLYBEZIER                  2
#define SCEMF_POLYGON                     3
#define SCEMF_POLYLINE                    4
#define SCEMF_POLYBEZIERTO                5
#define SCEMF_POLYLINETO                  6
#define SCEMF_POLYPOLYLINE                7
#define SCEMF_POLYPOLYGON                 8
#define SCEMF_SETWINDOWEXTEX              9
#define SCEMF_SETWINDOWORGEX              10
#define SCEMF_SETVIEWPORTEXTEX            11
#define SCEMF_SETVIEWPORTORGEX            12
#define SCEMF_SETBRUSHORGEX               13
#define SCEMF_EOF                         14
#define SCEMF_SETPIXELV                   15
#define SCEMF_SETMAPPERFLAGS              16
#define SCEMF_SETMAPMODE                  17
#define SCEMF_SETBKMODE                   18
#define SCEMF_SETPOLYFILLMODE             19
#define SCEMF_SETROP2                     20
#define SCEMF_SETSTRETCHBLTMODE           21
#define SCEMF_SETTEXTALIGN                22
#define SCEMF_SETCOLORADJUSTMENT          23
#define SCEMF_SETTEXTCOLOR                24
#define SCEMF_SETBKCOLOR                  25
#define SCEMF_OFFSETCLIPRGN               26
#define SCEMF_MOVETOEX                    27
#define SCEMF_SETMETARGN                  28
#define SCEMF_EXCLUDECLIPRECT             29
#define SCEMF_INTERSECTCLIPRECT           30
#define SCEMF_SCALEVIEWPORTEXTEX          31
#define SCEMF_SCALEWINDOWEXTEX            32
#define SCEMF_SAVEDC                      33
#define SCEMF_RESTOREDC                   34
#define SCEMF_SETWORLDTRANSFORM           35
#define SCEMF_MODIFYWORLDTRANSFORM        36
#define SCEMF_SELECTOBJECT                37
#define SCEMF_CREATEPEN                   38
#define SCEMF_CREATEBRUSHINDIRECT         39
#define SCEMF_DELETEOBJECT                40
#define SCEMF_ANGLEARC                    41
#define SCEMF_ELLIPSE                     42
#define SCEMF_RECTANGLE                   43
#define SCEMF_ROUNDRECT                   44
#define SCEMF_ARC                         45
#define SCEMF_CHORD                       46
#define SCEMF_PIE                         47
#define SCEMF_SELECTPALETTE               48
#define SCEMF_CREATEPALETTE               49
#define SCEMF_SETPALETTEENTRIES           50
#define SCEMF_RESIZEPALETTE               51
#define SCEMF_REALIZEPALETTE              52
#define SCEMF_EXTFLOODFILL                53
#define SCEMF_LINETO                      54
#define SCEMF_ARCTO                       55
#define SCEMF_POLYDRAW                    56
#define SCEMF_SETARCDIRECTION             57
#define SCEMF_SETMITERLIMIT               58
#define SCEMF_BEGINPATH                   59
#define SCEMF_ENDPATH                     60
#define SCEMF_CLOSEFIGURE                 61
#define SCEMF_FILLPATH                    62
#define SCEMF_STROKEANDFILLPATH           63
#define SCEMF_STROKEPATH                  64
#define SCEMF_FLATTENPATH                 65
#define SCEMF_WIDENPATH                   66
#define SCEMF_SELECTCLIPPATH              67
#define SCEMF_ABORTPATH                   68

#define SCEMF_GDICOMMENT                  70
#define SCEMF_FILLRGN                     71
#define SCEMF_FRAMERGN                    72
#define SCEMF_INVERTRGN                   73
#define SCEMF_PAINTRGN                    74
#define SCEMF_EXTSELECTCLIPRGN            75
#define SCEMF_BITBLT                      76
#define SCEMF_STRETCHBLT                  77
#define SCEMF_MASKBLT                     78
#define SCEMF_PLGBLT                      79
#define SCEMF_SETDIBITSTODEVICE           80
#define SCEMF_STRETCHDIBITS               81
#define SCEMF_EXTCREATEFONTINDIRECTW      82
#define SCEMF_EXTTEXTOUTA                 83
#define SCEMF_EXTTEXTOUTW                 84
#define SCEMF_POLYBEZIER16                85
#define SCEMF_POLYGON16                   86
#define SCEMF_POLYLINE16                  87
#define SCEMF_POLYBEZIERTO16              88
#define SCEMF_POLYLINETO16                89
#define SCEMF_POLYPOLYLINE16              90
#define SCEMF_POLYPOLYGON16               91
#define SCEMF_POLYDRAW16                  92
#define SCEMF_CREATEMONOBRUSH             93
#define SCEMF_CREATEDIBPATTERNBRUSHPT     94
#define SCEMF_EXTCREATEPEN                95
#define SCEMF_POLYTEXTOUTA                96
#define SCEMF_POLYTEXTOUTW                97
//WINVER >= 0x0400
#define SCEMF_SETICMMODE                  98
#define SCEMF_CREATECOLORSPACE            99
#define SCEMF_SETCOLORSPACE              100
#define SCEMF_DELETECOLORSPACE           101
#define SCEMF_GLSRECORD                  102
#define SCEMF_GLSBOUNDEDRECORD           103
#define SCEMF_PIXELFORMAT                104

//WINVER >= 0x0500
#if 1
	// they started out like this (Platform SDK, Windows2000)
#define SCEMF_DRAWESCAPE                 105
#define SCEMF_EXTESCAPE                  106
#define SCEMF_STARTDOC                   107
#define SCEMF_SMALLTEXTOUT               108
#define SCEMF_FORCEUFIMAPPING            109
#define SCEMF_NAMEDESCAPE                110
#define SCEMF_COLORCORRECTPALETTE        111
#define SCEMF_SETICMPROFILEA             112
#define SCEMF_SETICMPROFILEW             113
#define SCEMF_ALPHABLEND                 114
#define SCEMF_ALPHADIBBLEND              115
#define SCEMF_TRANSPARENTBLT             116
#define SCEMF_TRANSPARENTDIB             117
#define SCEMF_GRADIENTFILL               118
#define SCEMF_SETLINKEDUFIS              119
#define SCEMF_SETTEXTJUSTIFICATION       120
#define SCEMF_COLORMATCHTOTARGETW        121
#define SCEMF_CREATECOLORSPACEW          122

#else
	// they have changed their mind (Platform SDK, feb 2003)
#define SCEMF_RESERVED_105               105
#define SCEMF_RESERVED_106               106
#define SCEMF_RESERVED_107               107
#define SCEMF_RESERVED_108               108
#define SCEMF_RESERVED_109               109
#define SCEMF_RESERVED_110               110
#define SCEMF_COLORCORRECTPALETTE        111
#define SCEMF_SETICMPROFILEA             112
#define SCEMF_SETICMPROFILEW             113
#define SCEMF_ALPHABLEND                 114
#define SCEMF_SETLAYOUT                  115
#define SCEMF_TRANSPARENTBLT             116
#define SCEMF_RESERVED_117               117
#define SCEMF_GRADIENTFILL               118
#define SCEMF_RESERVED_119               119
#define SCEMF_RESERVED_120               120
#define SCEMF_COLORMATCHTOTARGETW        121
#define SCEMF_CREATECOLORSPACEW          122

#endif

//WINVER >= 0x???			upgrade codes here
//And modify this line
#define SC_MAXEMF_DISPATCH_CODE			 122

#pragma pack(1)
typedef struct tag_SCEMRSMALLTEXTOUTA
{
    EMR     emr;
	POINTL  ptlReference;
	DWORD	nChars;     
	DWORD	fOptions;     
    DWORD   iGraphicsMode;      // Current graphics mode
    FLOAT   exScale;            // X and Y scales from Page units to .01mm units
    FLOAT   eyScale;            //   if graphics mode is GM_COMPATIBLE (=1).
                                // followed by the string.
	char	Text[1];
} SCEMRSMALLTEXTOUTA, *PSCEMRSMALLTEXTOUTA;

typedef struct tag_SCEMRSMALLTEXTOUTA2
{
    EMR     emr;
	POINTL  ptlReference;
	DWORD	nChars;     
	DWORD	fOptions;     
    DWORD   iGraphicsMode;      // Current graphics mode
    FLOAT   exScale;            // X and Y scales from Page units to .01mm units
    FLOAT   eyScale;            //   if graphics mode is GM_COMPATIBLE.
                                // followed by a bounding box and the string.
	RECTL	rclBoundsSC;
	char	Text[1];
} SCEMRSMALLTEXTOUTA2, *PSCEMRSMALLTEXTOUTA2;

typedef struct tag_SCEMFCOMMENT_WMF
{
	DWORD ident;			// This contains GDICOMMENT_IDENTIFIER. 
	DWORD iComment;			// This contains GDICOMMENT_WINDOWS_METAFILE. 
	DWORD nVersion;			// This contains the version number of the 
							// Windows-format metafile. 
	DWORD nChecksum;		// This is the additive DWORD checksum for 
							// the enhanced metafile.  The checksum 
							// for the enhanced metafile data including 
							// this comment record must be zero. 
							// Otherwise, the enhanced metafile has been 
							//  modified and the Windows-format 
							// metafile is no longer valid. 
	DWORD fFlags;			// This must be zero. 
	DWORD cbWinMetaFile;
} SCEMFCOMMENT_WMF, *PSCEMFCOMMENT_WMF;

typedef struct tag_SCEMFCOMMENT_BEGINGROUP
{
	DWORD   ident;			// This contains GDICOMMENT_IDENTIFIER. 
	DWORD   iComment;		// This contains GDICOMMENT_BEGINGROUP. 
	RECTL   rclOutput;		// This is the bounding rectangle for the 
							// object in logical coordinates. 
	DWORD   nDescription;	// This is the number of characters in the 
							// optional Unicode description string that 
							// follows. This is zero if there is no 
							// description string.
} SCEMFCOMMENT_BEGINGROUP, *PSCEMFCOMMENT_BEGINGROUP;

typedef struct tag_SCEMFCOMMENT_ENDGROUP
{
	DWORD   ident;			// This contains GDICOMMENT_IDENTIFIER. 
	DWORD   iComment;		// This contains GDICOMMENT_ENDGROUP.
} SCEMFCOMMENT_ENDGROUP, *PSCEMFCOMMENT_ENDGROUP;


typedef struct tag_SCEMFCOMMENT_MULTIFORMATS
{
	DWORD   ident;          // This contains GDICOMMENT_IDENTIFIER. 
	DWORD   iComment;       // This contains GDICOMMENT_MULTIFORMATS. 
	RECTL   rclOutput;      // This is the bounding rectangle for the 
							// picture in logical coordinates. 
	DWORD   nFormats;       // This contains the number of formats in 
							// the comment. 
	EMRFORMAT aemrformat[1];// This is an array of EMRFORMAT structures 
							// in the order of preference.  The data 
							// for each format follows the last 
							// EMRFORMAT structure. 
} SCEMFCOMMENT_MULTIFORMATS, *PSCEMFCOMMENT_MULTIFORMATS;

typedef struct tag_SCEMRALPHADIBBLEND
{
    EMR     emr;
    RECTL   rclBounds;          // Inclusive bounds in device units
    LONG    xDest;
    LONG    yDest;
    LONG    cxDest;
    LONG    cyDest;
    DWORD   dwRop;
    LONG    xSrc;
    LONG    ySrc;
    XFORM   xformSrc;           // Source DC transform
    COLORREF crBkColorSrc;      // Source DC BkColor in RGB
    DWORD   iUsageSrc;          // Source bitmap info color table usage
                                // (DIB_RGB_COLORS)
    DWORD   offBmiSrc;          // Offset to the source BITMAPINFO structure
    DWORD   cbBmiSrc;           // Size of the source BITMAPINFO structure
    DWORD   offBitsSrc;         // Offset to the source bitmap bits
    DWORD   cbBitsSrc;          // Size of the source bitmap bits
    LONG    cxSrc;
    LONG    cySrc;
} SCEMRALPHADIBBLEND, *PSCEMRALPHASIBBLEND;

#pragma pack()

#endif //_SCEMFDEFS_H_
//  ------------------------------------------------------------

