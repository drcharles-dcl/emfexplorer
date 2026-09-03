/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCPropStrLists.h"
#include SC_INC_EMFLIB(SCGdiplusUtils.h)
#include SC_INC_EMFLIB(SCEMFViewDefs.h)

//	#ifdef _DEBUG
//	#define new DEBUG_NEW
//	#undef THIS_FILE
//	static char THIS_FILE[] = __FILE__;
//	#endif

/////////////////////////////////////////////////////////////////////////////
// Value/String pairs

// Control border
const SCStrEnumAttribute s_CtlBorder[]=
{
	SC_CSM_CTLBORDER_NONE,			_T("None"),
	SC_CSM_CTLBORDER_SIMPLE,		_T("Simple"),
	SC_CSM_CTLBORDER_RAISED,		_T("3D")
};

// Fit Mode
const SCStrEnumAttribute s_PageFitMode[]=
{
	SC_FIT_NONE,	_T("None"),
	SC_FIT_WIDTH,	_T("Width"),
	SC_FIT_PAGE,	_T("Page")
};

// Orientation 
const SCStrEnumAttribute s_PageOrientation[]=
{
	0,		_T("0"),
	90,		_T("90"),
	180,	_T("180"),
	270,	_T("270")
};

// Transparency Mode
const SCStrEnumAttribute s_TransparencyMode[]=
{
	SC_TRANSPARENCY_NONE,				_T("None"),
	SC_TRANSPARENCY_NORMAL,				_T("Normal"),
	SC_TRANSPARENCY_WHITE_SKIPBITMAP,	_T("White Color Skip Bitmaps"),
	SC_TRANSPARENCY_WHITECOLOR,			_T("White Color")
};

// Reverse video Mode
const SCStrEnumAttribute s_ReverseVideoMode[]=
{
	SC_REVERSE_VIDEO_NONE,				_T("None"),
	SC_REVERSE_VIDEO_FULL,				_T("Full"),
	SC_REVERSE_VIDEO_SKIPIMAGES,		_T("Skip Bitmaps")
};

// Color Type
const SCStrEnumAttribute s_ColorStyle[]=
{
	SC_COLOR_TRANSPARENT,				_T("Transparent"),
	SC_COLOR_SYSINDEX,					_T("System"),
	SC_COLOR_RGBVALUE,					_T("RGB")
};

/////////////////////////////////////////////////////////////////////////////
// Implementations (for enums)

SC_IMPLEMENT_OLESTRINGS(CtlBorder)

SC_IMPLEMENT_OLESTRINGS(PageFitMode)
SC_IMPLEMENT_OLESTRINGS(PageOrientation)
SC_IMPLEMENT_OLESTRINGS(TransparencyMode)
SC_IMPLEMENT_OLESTRINGS(ReverseVideoMode)

SC_IMPLEMENT_OLESTRINGS(ColorStyle)

