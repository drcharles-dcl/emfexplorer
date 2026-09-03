/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCGDIplusStrLists.h"
#include "SCGdiplusUtils.h"

//	#ifdef _DEBUG
//	#define new DEBUG_NEW
//	#undef THIS_FILE
//	static char THIS_FILE[] = __FILE__;
//	#endif

/////////////////////////////////////////////////////////////////////////////
// Value/String pairs

// TextRenderingHint 
const SCStrEnumAttribute s_TextRenderingHint[]=
{
	TextRenderingHintSystemDefault,				_T("Default"),
	TextRenderingHintSingleBitPerPixelGridFit,	_T("SingleBitPerPixelGridFit"),
	TextRenderingHintSingleBitPerPixel,			_T("SingleBitPerPixel"),
	TextRenderingHintAntiAliasGridFit,			_T("AntiAliasGridFit"),
	TextRenderingHintAntiAlias,					_T("AntiAlias"),
	TextRenderingHintClearTypeGridFit,			_T("ClearTypeGridFit")
};

// TextContrast
 //hack: 0-12 => from darker to brighter
const SCStrEnumAttribute s_TextContrast[]=
{
	0,	_T("Full Dark"),
	1,	_T("Dark"),
	2,	_T("Level2"),
	3,	_T("Level3"),
	4,	_T("Level4"),
	5,	_T("Level5"),
	6,	_T("Medium"),
	7,	_T("Level7"),
	8,	_T("Level8"),
	9,	_T("Level9"),
	10,	_T("Level10"),
	11,	_T("Bright"),
	12,	_T("Full Bright")
};


// SmoothingMode
const SCStrEnumAttribute s_SmoothingMode[]=
{
	SmoothingModeDefault,						_T("Default"),
	SmoothingModeHighSpeed,						_T("HighSpeed"),
	SmoothingModeHighQuality,					_T("HighQuality"),
	SmoothingModeNone,							_T("None"),
	SmoothingModeAntiAlias,						_T("AntiAlias")
};

// InterpolationMode
const SCStrEnumAttribute s_InterpolationMode[]=
{
	InterpolationModeDefault,					_T("Default"),
	InterpolationModeLowQuality,				_T("LowQuality"),
	InterpolationModeHighQuality,				_T("HighQuality"),
	InterpolationModeBilinear,					_T("Bilinear"),
	InterpolationModeBicubic,					_T("Bicubic"),
	InterpolationModeNearestNeighbor,			_T("NearestNeighbor"),
	InterpolationModeHighQualityBilinear,		_T("HighQualityBilinear"),
	InterpolationModeHighQualityBicubic,		_T("HighQualityBicubic")
};

// PixelOffsetMode
const SCStrEnumAttribute s_PixelOffsetMode[]=
{
	PixelOffsetModeDefault,						_T("Default"),
	PixelOffsetModeHighSpeed,					_T("HighSpeed"),
	PixelOffsetModeHighQuality,					_T("HighQuality"),
	PixelOffsetModeNone,						_T("None"),
	PixelOffsetModeHalf,						_T("Half")
};


/////////////////////////////////////////////////////////////////////////////
// Implementations (for enums)

SC_IMPLEMENT_OLESTRINGS(TextRenderingHint)
SC_IMPLEMENT_OLESTRINGS(TextContrast)
SC_IMPLEMENT_OLESTRINGS(SmoothingMode)
SC_IMPLEMENT_OLESTRINGS(InterpolationMode)
SC_IMPLEMENT_OLESTRINGS(PixelOffsetMode)

