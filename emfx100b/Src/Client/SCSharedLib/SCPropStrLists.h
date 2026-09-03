/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCPROPSTRLISTS_H_
#define _SCPROPSTRLISTS_H_

#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCWinStrLists.h)
#include SC_INC_EMFLIB(SCGDIplusStrLists.h)

/////////////////////////////////////////////////////////////////////////////
// Types and general functions

// Property names
#define SC_T_Picture			_T("Picture")
	// Control-related
#define SC_T_CtlBorder			_T("CtlBorder")
#define SC_T_CtlColorStyle		_T("CtlColorStyle")
#define SC_T_CtlColor			_T("CtlColor")
	// Page-related
#define SC_T_PageBorderVisible	_T("PageBorderVisible")
#define SC_T_PageShadowVisible	_T("PageShadowVisible")
#define SC_T_PaperColorStyle	_T("PaperColorStyle")
#define SC_T_PaperColor			_T("PaperColor")
#define SC_T_FitMode			_T("FitMode")
#define SC_T_Scale				_T("Scale")
#define SC_T_MarginL			_T("LeftMargin")
#define SC_T_MarginT			_T("TopMargin")
#define SC_T_MarginR			_T("RightMargin")
#define SC_T_MarginB			_T("BottomMargin")
#define SC_T_PageOrientation	_T("PageOrientation")
#define SC_T_TransparencyMode	_T("TransparencyMode")
#define SC_T_ReverseVideoMode	_T("ReverseVideoMode")
	// GDI+-related
#define SC_T_GDIplusEnabled		_T("GDIplusEnabled")
#define SC_T_TextRenderingHint	_T("TextRenderingQuality")
#define SC_T_TextContrast		_T("TextContrast")
#define SC_T_SmoothingMode		_T("SmoothingMode")
#define SC_T_InterpolationMode	_T("InterpolationMode")
#define SC_T_PixelOffsetMode	_T("PixelOffsetMode")

// Extra for persistency
#define SC_T_Src				_T("Src")

// Aliases
#define SC_T_PageFitMode		SC_T_FitMode


/////////////////////////////////////////////////////////////////////////////
// Declarations (for enums)

	// Properties
SC_DECLARE_OLESTRINGS(CtlBorder)
//CtlColorStyle: see below

SC_DECLARE_OLESTRINGS(PageFitMode)
SC_DECLARE_OLESTRINGS(PageOrientation)
//PaperColorStyle: see below
SC_DECLARE_OLESTRINGS(TransparencyMode)
SC_DECLARE_OLESTRINGS(ReverseVideoMode)

	// Generic
SC_DECLARE_OLESTRINGS(ColorStyle)


/// Economizing some definitions
	// From generic ColorStyle
#define SCGetCtlColorStylePredefinedValue		SCGetColorStylePredefinedValue
#define SCGetCtlColorStylePredefinedCookie		SCGetColorStylePredefinedCookie
#define SCGetPaperColorStylePredefinedValue		SCGetColorStylePredefinedValue
#define SCGetPaperColorStylePredefinedCookie	SCGetColorStylePredefinedCookie
///

#endif //_SCPROPSTRLISTS_H_
//  ------------------------------------------------------------
