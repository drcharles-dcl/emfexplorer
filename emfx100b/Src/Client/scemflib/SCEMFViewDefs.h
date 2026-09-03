/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFVIEWDEFS_H_
#define _SCEMFVIEWDEFS_H_

// Zoom (assuming basic printer DPI as the reference)
#define		SC_FLOATFACTOR		100
#define		SC_MIN_ZOOM			(7*SC_FLOATFACTOR)		// 5 DPI
#define		SC_ZOOM100			(100*SC_FLOATFACTOR)	// 72 DPI (basic printer)
#if 0
	#define	SC_MAX_ZOOM			(1000*SC_FLOATFACTOR)	// 720 DPI
#else
	// too big for this version ? (no if you can wait)
	#define SC_MAX_ZOOM			(1667*SC_FLOATFACTOR)	// 1200 DPI
#endif
// Special values
#define		SC_ZOOM_FITWITDH	-32768
#define		SC_ZOOM_FITPAGE		-32767

// Fit modes
#define		SC_FIT_NONE			0
#define		SC_FIT_WIDTH		1
#define		SC_FIT_PAGE			2

// Engines
#define		SC_ENGINE_GDI		0
#define		SC_ENGINE_GDIP		1

// Color Types
#define		SC_COLOR_TRANSPARENT			0
#define		SC_COLOR_SYSINDEX				1
#define		SC_COLOR_RGBVALUE				2

#define		SC_MAKE_SYSCOLOR(idx)			(0x80000000|idx)
#define		SC_IS_SYSCOLOR(clr)				(0x80000000==(clr & 0xFF000000)) //0x800000ii => system index
#define		SC_IS_RGBCOLOR(clr)				(0x00000000==(clr & 0xFF000000)) //0x00bbggrr => RGB
#define		SC_SYSCOLOR_INDEX(clr)			(clr & 0x000000FF)
#define		SC_RGBCOLOR_VALUE(clr)			(clr & 0x00FFFFFF)

// Color Scheme
	// around image
#define		SC_CSM_CTLCOLOR_TRANSPARENT		SC_COLOR_TRANSPARENT // solved to ambient background
#define		SC_CSM_CTLCOLOR_SYSINDEX		SC_COLOR_SYSINDEX
#define		SC_CSM_CTLCOLOR_RGBVALUE		SC_COLOR_RGBVALUE
#define		SC_CSM_CTLCOLOR_MASK			0x000F
   // under image
#define		SC_CSM_PAPERCOLOR_TRANSPARENT	(SC_COLOR_TRANSPARENT << 4) // solved to control background
#define		SC_CSM_PAPERCOLOR_SYSINDEX		(SC_COLOR_SYSINDEX << 4)
#define		SC_CSM_PAPERCOLOR_RGBVALUE		(SC_COLOR_RGBVALUE << 4)
#define		SC_CSM_PAPERCOLOR_MASK			0x00F0
   // image border
#define		SC_CSM_BORDER_NONE				0x0000
#define		SC_CSM_BORDER_YES				0x0100
#define		SC_CSM_BORDER_MASK				0x0100
  // control border
#define		SC_CSM_CTLBORDER_NONE			0x0000
#define		SC_CSM_CTLBORDER_SIMPLE			0x0200
#define		SC_CSM_CTLBORDER_RAISED			0x0400
#define		SC_CSM_CTLBORDER_MASK			0x0700
   // image shadow
#define		SC_CSM_SHADOW_NONE				0x0000
#define		SC_CSM_SHADOW_YES				0x1000
#define		SC_CSM_SHADOW_MASK				0x1000


///////////////////////////////////////////////////////////////////////////////////////
// Drawing attributes
//

#define SC_TRANSPARENCY_NORMAL				0x0000
#define SC_TRANSPARENCY_WHITE_SKIPBITMAP	0x0001
#define SC_TRANSPARENCY_WHITECOLOR			0x0002
#define SC_TRANSPARENCY_NONE				0x0004
#define SC_TRANSPARENCY_MASK				0x000F
#define SC_TRANSPARENCY(rAttributes)		((rAttributes).dwInkingMode & SC_TRANSPARENCY_MASK)

#define SC_REVERSE_VIDEO_SKIPIMAGES			0x0000
#define SC_REVERSE_VIDEO_FULL				0x0010
#define SC_REVERSE_VIDEO_NONE				0x0020
#define SC_REVERSE_VIDEO_MASK				0x00F0
#define SC_REVERSEVIDEO(rAttributes)		((rAttributes).dwInkingMode & SC_REVERSE_VIDEO_MASK)
#define SC_RVIDEOFULL(rAttributes)			(SC_REVERSE_VIDEO_FULL==((rAttributes).dwInkingMode & SC_REVERSE_VIDEO_MASK))


///////////////////////////////////////////////////////////////////////////////////////
// Some limits
//

#define SC_MIN_TEXTRENDERINGHINT	TextRenderingHintSystemDefault		
#define SC_MIN_TEXTCONTRAST			0
#define SC_MIN_SMOOTHINGMODE		SmoothingModeDefault	
#define SC_MIN_INTERPOLATIONMODE	InterpolationModeDefault	
#define SC_MIN_PIXELOFFSETMODE		PixelOffsetModeDefault

#define SC_MAX_TEXTRENDERINGHINT	TextRenderingHintClearTypeGridFit		
#define SC_MAX_TEXTCONTRAST			12
#define SC_MAX_SMOOTHINGMODE		SmoothingModeAntiAlias	
#define SC_MAX_INTERPOLATIONMODE	InterpolationModeHighQualityBicubic	
#define SC_MAX_PIXELOFFSETMODE		PixelOffsetModeHalf

///////////////////////////////////////////////////////////////////////////////////////
// Some defaults
//

// color schemes
#define SC_DFLT_VIEWCSHM			(SC_CSM_CTLCOLOR_SYSINDEX|SC_CSM_PAPERCOLOR_RGBVALUE|SC_CSM_BORDER_YES|SC_CSM_SHADOW_YES)
#define SC_DFLT_INKINGMODE			(SC_REVERSE_VIDEO_SKIPIMAGES|SC_TRANSPARENCY_WHITECOLOR)

// property values
#define SC_DFLT_CTLBORDER			SC_CSM_CTLBORDER_NONE
#define SC_DFLT_CTLCOLORSTYLE		SC_COLOR_SYSINDEX
#define SC_DFLT_CTLCOLOR			COLOR_3DFACE
	//--
#define SC_DFLT_PAGEBORDERVISIBLE	TRUE
#define SC_DFLT_PAGESHADOWVISIBLE	TRUE
#define SC_DFLT_PAPERCOLORSTYLE		SC_COLOR_RGBVALUE
#define SC_DFLT_PAPERCOLOR			RGB(255, 255, 255)
#define SC_DFLT_FITMODE				SC_FIT_WIDTH
#define SC_DFLT_SCALE				SC_ZOOM100
#define SC_DFLT_MARGINL				32
#define SC_DFLT_MARGINT				32
#define SC_DFLT_MARGINR				32
#define SC_DFLT_MARGINB				32
#define SC_DFLT_PAGEORIENTATION		0
#define SC_DFLT_TRANSPARENCYMODE	SC_TRANSPARENCY_WHITE_SKIPBITMAP
#define SC_DFLT_REVERSEVIDEOMODE	SC_REVERSE_VIDEO_NONE
	//--
#define SC_DFLT_GDIPLUSENABLED		TRUE
#define SC_DFLT_TEXTRENDERINGHINT	TextRenderingHintAntiAlias
#define SC_DFLT_TEXTCONTRAST		2 // hack: 0-12 => from darker to brighter
#define SC_DFLT_SMOOTHINGMODE		SmoothingModeHighQuality
#define SC_DFLT_INTERPOLATIONMODE	InterpolationModeHighQualityBicubic
#define SC_DFLT_PIXELOFFSETMODE		PixelOffsetModeHighQuality
//


///////////////////////////////////////////////////////////////////////////////////////
// Some view defaults
//

#define MNU_IDX_TRACKPOPUP1		0

#define SC_PIX_TXTLINE		16
#define SC_SHADOW_WDT		2
#define MIN_MARGIN			16



#endif //_SCEMFVIEWDEFS_H_
//  ------------------------------------------------------------
