/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFTxtUtils.h"
#include "SCEMFDefs.h"
#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCGDIUtils.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// Record names
//
#define SC_UNKNOWN_RECNAME	_T("EMR_UNKNOWN")

LPTSTR s_SCEMFRecNames[] =
{
		_T("error"),
		_T("EMR_HEADER"),
		_T("EMR_POLYBEZIER"),
		_T("EMR_POLYGON"),
		_T("EMR_POLYLINE"),
		_T("EMR_POLYBEZIERTO"),
		_T("EMR_POLYLINETO"),
		_T("EMR_POLYPOLYLINE"),
		_T("EMR_POLYPOLYGON"),
		_T("EMR_SETWINDOWEXTEX"),
		_T("EMR_SETWINDOWORGEX"),
		_T("EMR_SETVIEWPORTEXTEX"),
		_T("EMR_SETVIEWPORTORGEX"),
		_T("EMR_SETBRUSHORGEX"),
		_T("EMR_EOF"),
		_T("EMR_SETPIXELV"),
		_T("EMR_SETMAPPERFLAGS"),
		_T("EMR_SETMAPMODE"),
		_T("EMR_SETBKMODE"),
		_T("EMR_SETPOLYFILLMODE"),
		_T("EMR_SETROP2"),
		_T("EMR_SETSTRETCHBLTMODE"),
		_T("EMR_SETTEXTALIGN"),
		_T("EMR_SETCOLORADJUSTMENT"),
		_T("EMR_SETTEXTCOLOR"),
		_T("EMR_SETBKCOLOR"),
		_T("EMR_OFFSETCLIPRGN"),
		_T("EMR_MOVETOEX"),
		_T("EMR_SETMETARGN"),
		_T("EMR_EXCLUDECLIPRECT"),
		_T("EMR_INTERSECTCLIPRECT"),
		_T("EMR_SCALEVIEWPORTEXTEX"),
		_T("EMR_SCALEWINDOWEXTEX"),
		_T("EMR_SAVEDC"),
		_T("EMR_RESTOREDC"),
		_T("EMR_SETWORLDTRANSFORM"),
		_T("EMR_MODIFYWORLDTRANSFORM"),
		_T("EMR_SELECTOBJECT"),
		_T("EMR_CREATEPEN"),
		_T("EMR_CREATEBRUSHINDIRECT"),
		_T("EMR_DELETEOBJECT"),
		_T("EMR_ANGLEARC"),
		_T("EMR_ELLIPSE"),
		_T("EMR_RECTANGLE"),
		_T("EMR_ROUNDRECT"),
		_T("EMR_ARC"),
		_T("EMR_CHORD"),
		_T("EMR_PIE"),
		_T("EMR_SELECTPALETTE"),
		_T("EMR_CREATEPALETTE"),
		_T("EMR_SETPALETTEENTRIES"),
		_T("EMR_RESIZEPALETTE"),
		_T("EMR_REALIZEPALETTE"),
		_T("EMR_EXTFLOODFILL"),
		_T("EMR_LINETO"),
		_T("EMR_ARCTO"),
		_T("EMR_POLYDRAW"),
		_T("EMR_SETARCDIRECTION"),
		_T("EMR_SETMITERLIMIT"),
		_T("EMR_BEGINPATH"),
		_T("EMR_ENDPATH"),
		_T("EMR_CLOSEFIGURE"),
		_T("EMR_FILLPATH"),
		_T("EMR_STROKEANDFILLPATH"),
		_T("EMR_STROKEPATH"),
		_T("EMR_FLATTENPATH"),
		_T("EMR_WIDENPATH"),
		_T("EMR_SELECTCLIPPATH"),
		_T("EMR_ABORTPATH"),
		_T("error"),
		_T("EMR_GDICOMMENT"),
		_T("EMR_FILLRGN"),
		_T("EMR_FRAMERGN"),
		_T("EMR_INVERTRGN"),
		_T("EMR_PAINTRGN"),
		_T("EMR_EXTSELECTCLIPRGN"),
		_T("EMR_BITBLT"),
		_T("EMR_STRETCHBLT"),
		_T("EMR_MASKBLT"),
		_T("EMR_PLGBLT"),
		_T("EMR_SETDIBITSTODEVICE"),
		_T("EMR_STRETCHDIBITS"),
		_T("EMR_EXTCREATEFONTINDIRECTW"),
		_T("EMR_EXTTEXTOUTA"),
		_T("EMR_EXTTEXTOUTW"),
		_T("EMR_POLYBEZIER16"),
		_T("EMR_POLYGON16"),
		_T("EMR_POLYLINE16"),
		_T("EMR_POLYBEZIERTO16"),
		_T("EMR_POLYLINETO16"),
		_T("EMR_POLYPOLYLINE16"),
		_T("EMR_POLYPOLYGON16"),
		_T("EMR_POLYDRAW16"),
		_T("EMR_CREATEMONOBRUSH"),
		_T("EMR_CREATEDIBPATTERNBRUSHPT"),
		_T("EMR_EXTCREATEPEN"),
		_T("EMR_POLYTEXTOUTA"),
		_T("EMR_POLYTEXTOUTW"),
		//WINVER >= 0x0400
		_T("EMR_SETICMMODE"),
		_T("EMR_CREATECOLORSPACE"),
		_T("EMR_SETCOLORSPACE"),
		_T("EMR_DELETECOLORSPACE"),
		_T("EMR_GLSRECORD"),
		_T("EMR_GLSBOUNDEDRECORD"),
		_T("EMR_PIXELFORMAT"),
		//WINVER >= 0x0500
#if 1
		_T("EMR_DRAWESCAPE"),
		_T("EMR_EXTESCAPE"),
		_T("EMR_STARTDOC"),
		_T("EMR_SMALLTEXTOUT"),
		_T("EMR_FORCEUFIMAPPING"),
		_T("EMR_NAMEDESCAPE"),
		_T("EMR_COLORCORRECTPALETTE"),
		_T("EMR_SETICMPROFILEA"),
		_T("EMR_SETICMPROFILEW"),
		_T("EMR_ALPHABLEND"),
		_T("EMR_SETLAYOUT"),	/*EMR_ALPHADIBBLEND*/
		_T("EMR_TRANSPARENTBLT"),
		_T("EMR_TRANSPARENTDIB"),
		_T("EMR_GRADIENTFILL"),
		_T("EMR_SETLINKEDUFIS"),
		_T("EMR_SETTEXTJUSTIFICATION"),
		_T("EMR_COLORMATCHTOTARGETW"),
		_T("EMR_CREATECOLORSPACEW"),
#else
		_T("EMR_RESERVED_105"),
		_T("EMR_RESERVED_106"),
		_T("EMR_RESERVED_107"),
		_T("EMR_RESERVED_108"),
		_T("EMR_RESERVED_109"),
		_T("EMR_RESERVED_110"),
		_T("EMR_COLORCORRECTPALETTE"),
		_T("EMR_SETICMPROFILEA"),
		_T("EMR_SETICMPROFILEW"),
		_T("EMR_ALPHABLEND"),
		_T("EMR_SETLAYOUT"),
		_T("EMR_TRANSPARENTBLT"),
		_T("EMR_RESERVED_117"),
		_T("EMR_GRADIENTFILL"),
		_T("EMR_RESERVED_119"),
		_T("EMR_RESERVED_120"),
		_T("EMR_COLORMATCHTOTARGETW"),
		_T("EMR_CREATECOLORSPACEW")
#endif
	};

LPTSTR SCGetRecordName(int iCode)
{
	if (iCode>=sizeof(s_SCEMFRecNames)/sizeof(s_SCEMFRecNames[0]))
		return SC_UNKNOWN_RECNAME;

	return s_SCEMFRecNames[iCode];
}

////////////////////////////////////////////////////////////////////////
// Texts

CString SCFormatTextOptions(UINT uiOptions)
{
#if (_WIN32_WINNT >= 0x0500)
	#define SC_ETO_PDY                      ETO_PDY
#else
	#define SC_ETO_PDY                      0x2000
#endif // (_WIN32_WINNT >= 0x0500)

	CString strOpt = _T("");

	if (uiOptions & ETO_OPAQUE)
		strOpt += _T("|ETO_OPAQUE");

	if (uiOptions & ETO_CLIPPED)
		strOpt += _T("|ETO_CLIPPED");

	if (uiOptions & ETO_GLYPH_INDEX)
		strOpt += _T("|ETO_GLYPH_INDEX");

	if (uiOptions & ETO_NUMERICSLATIN)
		strOpt += _T("|ETO_NUMERICSLATIN");

	if (uiOptions & ETO_NUMERICSLOCAL)
		strOpt += _T("|ETO_NUMERICSLOCAL");

	if (uiOptions & SC_ETO_PDY)
		strOpt += _T("|ETO_PDY");

	if (uiOptions & ETO_RTLREADING)
		strOpt += _T("|ETO_RTLREADING");

	if (!strOpt.IsEmpty())
		strOpt.Right(strOpt.GetLength()-1);
	
	return strOpt;
}


CString STFormatEmrTextA(EMRTEXT &emrtext, EMREXTTEXTOUTA *pRec)
{
	int nbChars = pRec->emrtext.nChars;
	TCHAR *pszText = new TCHAR[nbChars+1];

#ifdef _UNICODE
	MultiByteToWideChar(CP_ACP, 0, (char*)pRec + pRec->emrtext.offString, nbChars, pszText, nbChars);
#else
	strncpy(pszText, (char*)pRec + pRec->emrtext.offString, nbChars);
#endif
	pszText[nbChars] = 0;
	
	CString strOpt = SCFormatTextOptions(pRec->emrtext.fOptions);
	
	CString strText;
	strText.Format(_T(" TXT=[%s] [exScale(%f) eyScale(%f) iGraphicsMode(%d), Bounds(%d,%d,%d,%d)] TxOPT[fOptions(%d%s), nChars(%d), offDx(%d), ptlRef(%d,%d), rcl(%d,%d,%d,%d)]"),
		pszText,
		pRec->exScale, pRec->eyScale, pRec->iGraphicsMode, SC_PARAM_RCLBOUNDS(pRec->rclBounds),
		pRec->emrtext.fOptions, strOpt, pRec->emrtext.nChars, pRec->emrtext.offDx, SC_PARAM_POINT(pRec->emrtext.ptlReference), SC_PARAM_RECT(pRec->emrtext.rcl));
	delete [] pszText;

	// characters spacing
	if (pRec->emrtext.offDx)
	{
		long* pSpace = (long*)((BYTE*)pRec + pRec->emrtext.offDx);
		CString strSpace = _T(" Spacing[");
		CString strCur;
		long nTotal = 0;
		for (int i=0; (i<nbChars-1); i++)
		{
			nTotal += *pSpace;
			strCur.Format(_T("%ld,"), *pSpace++);
			strSpace += strCur;
		}
		
		nTotal += *pSpace;
		strCur.Format(_T("%ld => Total(%ld) =>xPtRefRight(%ld)]"), *pSpace, nTotal, pRec->emrtext.ptlReference.x+nTotal-1);
		strText += strSpace + strCur;
	}
	
	return strText;
}

CString STFormatEmrTextW(EMRTEXT &emrtext, EMREXTTEXTOUTW *pRec)
{
	int nbChars = pRec->emrtext.nChars;
	LPWSTR lpw = (LPWSTR)((char*)pRec + pRec->emrtext.offString);

	TCHAR *pszText = new TCHAR[nbChars+1];
	
#ifdef _UNICODE
	wcsncpy(pszText, lpw, nbChars);
#else
	WideCharToMultiByte(CP_ACP, 0, lpw, nbChars, pszText, (nbChars+1)*sizeof(TCHAR), NULL, NULL);
#endif
	pszText[nbChars]=0;

	CString strOpt = SCFormatTextOptions(pRec->emrtext.fOptions);
	
	CString strText;
	strText.Format(_T(" TXT=[%s] [exScale(%f) eyScale(%f) iGraphicsMode(%d), Bounds(%d,%d,%d,%d)] TxOPT[fOptions(%d%s), nChars(%d), offDx(%d), ptlRef(%d,%d), rcl(%d,%d,%d,%d)]"),
		pszText,
		pRec->exScale, pRec->eyScale, pRec->iGraphicsMode, SC_PARAM_RCLBOUNDS(pRec->rclBounds),
		pRec->emrtext.fOptions, strOpt, nbChars, pRec->emrtext.offDx, SC_PARAM_POINT(pRec->emrtext.ptlReference), SC_PARAM_RECT(pRec->emrtext.rcl));
	delete [] pszText;

	// characters spacing
	if (pRec->emrtext.offDx)
	{
		long* pSpace = (long*)((BYTE*)pRec + pRec->emrtext.offDx);
		CString strSpace = _T(" Spacing[");
		CString strCur;
		long nTotal = 0;
		if (nbChars>1)
		{
			for (int i=0; (i<nbChars-1); i++)
			{
				nTotal += *pSpace;
				strCur.Format(_T("%ld,"), *pSpace++);
				strSpace += strCur;
			}
		}
		nTotal += *pSpace;
		strCur.Format(_T("%ld => Total(%ld) =>xPtRefRight(%ld)]"), *pSpace, nTotal, pRec->emrtext.ptlReference.x+nTotal-1);
		strText += strSpace + strCur;
	}
	
	return strText;
}

////////////////////////////////////////////////////////////////////////
// Rops

LPCTSTR const s_strGDIRops2[]=
{
		_T(""),
		_T("R2_BLACK"),
		_T("R2_NOTMERGEPEN"),
		_T("R2_MASKNOTPEN"),
		_T("R2_NOTCOPYPEN"),
		_T("R2_MASKPENNOT"),
		_T("R2_NOT"),
		_T("R2_XORPEN"),
		_T("R2_NOTMASKPEN"),
		_T("R2_MASKPEN"),
		_T("R2_NOTXORPEN"),
		_T("R2_NOP"),
		_T("R2_MERGENOTPEN"),
		_T("R2_COPYPEN"),
		_T("R2_MERGEPENNOT"),
		_T("R2_MERGEPEN"),
		_T("R2_WHITE")
};

CString SCRop2Text(INT iRop2)
{
	if (iRop2>=sizeof(s_strGDIRops2)/sizeof(s_strGDIRops2[0]))
		return _T("");

	return s_strGDIRops2[iRop2];
}

////////////////////////////////////////////////////////////////////////
// Pens

CString SCPenStyleText(UINT uiPenStyle)
{
	CString strResult = _T("");
	switch (uiPenStyle & PS_STYLE_MASK)
	{
	case PS_SOLID:			strResult = _T("PS_SOLID"); break;
	case PS_DASH:			strResult = _T("PS_DASH"); break;
	case PS_DOT:			strResult = _T("PS_DOT"); break;
	case PS_DASHDOT:		strResult = _T("PS_DASHDOT"); break;
	case PS_DASHDOTDOT:		strResult = _T("PS_DASHDOTDOT"); break;
	case PS_NULL:			strResult = _T("PS_NULL"); break;
	case PS_INSIDEFRAME:	strResult = _T("PS_INSIDEFRAME"); break;
	case PS_USERSTYLE:		strResult = _T("PS_USERSTYLE"); break;
	case PS_ALTERNATE:		strResult = _T("PS_ALTERNATE"); break;
	}

	if ((uiPenStyle & PS_STYLE_MASK) != PS_NULL)
	{
		switch (uiPenStyle & PS_TYPE_MASK)
		{
		case PS_GEOMETRIC:	strResult += _T(" | GEOMETRIC"); break;
		case PS_COSMETIC:	strResult += _T(" | COSMETIC"); break;
		}
		
		switch (uiPenStyle & PS_JOIN_MASK)
		{
		case PS_JOIN_ROUND: /*strResult += _T(" | JOIN_ROUND");*/ break;
		case PS_JOIN_MITER: strResult += _T(" | JOIN_MITER"); break;
		case PS_JOIN_BEVEL: strResult += _T(" | JOIN_BEVEL"); break;
		}
		
		switch (uiPenStyle & PS_ENDCAP_MASK)
		{
		case PS_ENDCAP_ROUND: /*strResult += _T(" | ENDCAP_ROUND");*/ break;
		case PS_ENDCAP_FLAT: strResult += _T(" | ENDCAP_FLAT"); break;
		case PS_ENDCAP_SQUARE: strResult += _T(" | ENDCAP_SQUARE"); break;
		}
	}

	return strResult;
}
			
////////////////////////////////////////////////////////////////////////
// Brushes

CString SCHatchStyleText(UINT uiHatchStyle)
{
	switch (uiHatchStyle)
	{
	case HS_BDIAGONAL:	return _T("HS_BDIAGONAL");
	case HS_CROSS:		return _T("HS_CROSS");
	case HS_DIAGCROSS:	return _T("HS_DIAGCROSS");
	case HS_FDIAGONAL:	return _T("HS_FDIAGONAL");
	case HS_HORIZONTAL:	return _T("HS_HORIZONTAL");
	case HS_VERTICAL:	return _T("HS_VERTICAL");
	}

	return _T("HS_??");
}

CString SCBrushStyleText(UINT uiStyle)
{
	switch (uiStyle)
	{
	case BS_SOLID:			return _T("BS_SOLID");
	case BS_HATCHED:		return _T("BS_HATCHED");
	case BS_PATTERN:		return _T("BS_PATTERN");
	case BS_PATTERN8X8:		return _T("BS_PATTERN8X8");
	case BS_DIBPATTERNPT:	return _T("BS_DIBPATTERNPT");
	case BS_DIBPATTERN:		return _T("BS_DIBPATTERN");
	case BS_DIBPATTERN8X8:	return _T("BS_DIBPATTERN8X8");
	case BS_INDEXED:		return _T("BS_INDEXED");
	case BS_MONOPATTERN:	return _T("BS_MONOPATTERN");
	case BS_NULL:			return _T("BS_NULL");
	}

	return _T("BS_??");
}

CString SCLogBrushStyleText(LOGBRUSH32& rLb)
{
	CString strBrushStyle;
	strBrushStyle.Format(_T("style(%d=%s"),
		rLb.lbStyle,
		SCBrushStyleText(rLb.lbStyle));
	
	CString strAdd;
	switch (rLb.lbStyle)
	{
	case BS_SOLID:
		// elpColor specifies the color in which the pen is to be drawn.
		// elpHatch is ignored.		
		strAdd.Format(_T("color:0x%08X"), rLb.lbColor);
		break;

	case BS_HATCHED:
		// elpColor specifies the color in which the pen is to be drawn
		// elpHatch describes the hatches
		strAdd.Format(_T("lbHatch:%d=%s, color:0x%08X"),
			rLb.lbHatch, SCHatchStyleText(rLb.lbHatch), rLb.lbColor);
		break;
		
	case BS_DIBPATTERN:
		// lbColor is equal to DIB_PAL_COLORS or DIB_RGB_COLORS.
		// lbHatch is a handle to the packed DIB.	
		strAdd.Format(_T("handle:0x%08x, color_format:%s"),
			rLb.lbHatch, SCDIBColorUsageText(LOWORD(rLb.lbColor)));
		break;
		
	case BS_DIBPATTERNPT:
		// lbColor is equal to DIB_PAL_COLORS or DIB_RGB_COLORS.
		// lbHatch member is a pointer to the packed DIB.
		strAdd.Format(_T("pointer:0x%08x, color_format:%s"),
			rLb.lbHatch, SCDIBColorUsageText(LOWORD(rLb.lbColor)));
		break;
		
	case BS_PATTERN:
		// lbColor is ignored.
		// lbHatch is a handle to the bitmap that defines the pattern.
		strAdd.Format(_T("handle:0x%08x"), rLb.lbHatch);
		break;
		
	case BS_HOLLOW:
		// lbColor is ignored
		// lbHatch is ignored
		break;
	}
	if (!strAdd.IsEmpty())
		strBrushStyle += _T(", ") + strAdd + _T(")");
	else
		strBrushStyle += _T(")");
	
	return strBrushStyle;
}

CString SCExtLogpenBrushStyleText(EMREXTCREATEPEN *pRec)
{
	ASSERT(pRec);
	EXTLOGPEN& rLp = pRec->elp;

	CString strBrushStyle;
	strBrushStyle.Format(_T("brush(%d=%s"),
		rLp.elpBrushStyle, SCBrushStyleText(rLp.elpBrushStyle));

	CString strAdd;
	switch (rLp.elpBrushStyle)
	{
	case BS_SOLID:
		// elpColor specifies the color in which the pen is to be drawn.
		// elpHatch is ignored.		
		strAdd.Format(_T("elpColor:0x%08X"), rLp.elpColor);
		break;

	case BS_HATCHED:
		// elpColor specifies the color in which the pen is to be drawn
		// elpHatch describes the hatches
		strAdd.Format(_T("elpHatch:%d=%s, elpColor:0x%08X"),
			rLp.elpHatch, SCHatchStyleText(rLp.elpHatch), rLp.elpColor);
		break;
		
	case BS_DIBPATTERN:
		// elpColor is equal to DIB_PAL_COLORS or DIB_RGB_COLORS.
		// elpHatch is a handle to the packed DIB.	
		strAdd.Format(_T("handle:0x%08x, color_format:%s, BMP[offBmi:%d, Bmi:%d, offBits:%d, Bits:%d]"),
			rLp.elpHatch, SCDIBColorUsageText(rLp.elpColor),
			pRec->offBmi, pRec->cbBmi, pRec->offBits, pRec->cbBits);
		break;
		
	case BS_DIBPATTERNPT:
		// elpColor is equal to DIB_PAL_COLORS or DIB_RGB_COLORS.
		// elpHatch member is a pointer to the packed DIB.
		strAdd.Format(_T("pointer:0x%08x, color_format:%s, BMP[offBmi:%d, Bmi:%d, offBits:%d, Bits:%d]"),
			rLp.elpHatch, SCDIBColorUsageText(rLp.elpColor),
			pRec->offBmi, pRec->cbBmi, pRec->offBits, pRec->cbBits);
		break;
		
	case BS_PATTERN:
		// elpColor is ignored.
		// elpHatch is a handle to the bitmap that defines the pattern.
		strAdd.Format(_T("handle:0x%08x, BMP[offBmi:%d, Bmi:%d, offBits:%d, Bits:%d]"),
			rLp.elpHatch,
			pRec->offBmi, pRec->cbBmi, pRec->offBits, pRec->cbBits);
		break;
		
	case BS_HOLLOW:
		// elpColor is ignored
		// elpHatch is ignored
		break;
	}
	if (!strAdd.IsEmpty())
		strBrushStyle += _T(", ") + strAdd + _T(")");
	else
		strBrushStyle += _T(")");
	
	return strBrushStyle;
}

////////////////////////////////////////////////////////////////////////
// Bitmaps

CString SCDIBColorUsageText(UINT uiUsage)
{
#ifndef DIB_PAL_INDICES
	#define DIB_PAL_INDICES		2
#endif

	switch (uiUsage)
	{
	case DIB_RGB_COLORS:	return _T("DIB_RGB_COLORS");
	case DIB_PAL_COLORS:	return _T("DIB_PAL_COLORS");
	case DIB_PAL_INDICES:	return _T("DIB_PAL_INDICES");
	}

	return _T("DIB_??");
}

CString SCBmiText(BITMAPINFOHEADER *pBmi)
{
	ASSERT(pBmi);
	CString sBmiText;

	sBmiText.Format(
		_T("biSize:%d, ")
		_T("biWidth:%d, ")
		_T("biHeight:%d, ")
		_T("biPlanes:%d, ")
		_T("biBitCount:%d, ")
		_T("biCompression:%d, ")
		_T("biSizeImage:%d, ")
		_T("biXPelsPerMeter:%d, ")
		_T("biYPelsPerMeter:%d, ")
		_T("biClrUsed:%d, ")
		_T("biClrImportant:%d"),

		pBmi->biSize, 
		pBmi->biWidth, 
		pBmi->biHeight, 
		pBmi->biPlanes,
		pBmi->biBitCount, 
		pBmi->biCompression, 
		pBmi->biSizeImage, 
		pBmi->biXPelsPerMeter, 
		pBmi->biYPelsPerMeter, 
		pBmi->biClrUsed, 
		pBmi->biClrImportant);

	return sBmiText;
}

////////////////////////////////////////////////////////////////////////
// Modes

CString SCMapModeText(int iMode)
{
	switch (iMode)
	{
	case MM_TEXT:			return _T("MM_TEXT");
	case MM_LOMETRIC:		return _T("MM_LOMETRIC");
	case MM_HIMETRIC:		return _T("MM_HIMETRIC");
	case MM_LOENGLISH:		return _T("MM_LOENGLISH");
	case MM_HIENGLISH:		return _T("MM_HIENGLISH");
	case MM_TWIPS:			return _T("MM_TWIPS");
	case MM_ISOTROPIC:		return _T("MM_ISOTROPIC");
	case MM_ANISOTROPIC:	return _T("MM_ANISOTROPIC");
	}
	
	return _T("MM_??");
}

CString SCBkModeText(int iMode)
{
	switch (iMode)
	{
	case OPAQUE:		return	_T("OPAQUE");
	case TRANSPARENT:	return	_T("TRANSPARENT");
	}

	return _T("??");
}

CString SCStretchBltModeText(int iMode)
{
	switch (iMode)
	{
	case BLACKONWHITE:	return	_T("BLACKONWHITE");	
	case WHITEONBLACK:	return	_T("WHITEONBLACK");
	case COLORONCOLOR:	return	_T("COLORONCOLOR");
	case HALFTONE:		return	_T("HALFTONE");
	}
	
	return _T("??");
}

CString SCTextAlignModeText(int iMode)
{
	CString strMode = _T("");

	// way
	if (iMode & TA_RTLREADING)
		strMode += _T(" TA_RTLREADING");

	// horizontal
	switch (iMode & SC_VTXTALIGN_MASK)
	{
	case TA_CENTER:	strMode += _T(" TA_CENTER"); break;
	case TA_RIGHT:	strMode += _T(" TA_RIGHT"); break;
	case TA_LEFT:	strMode += _T(" TA_LEFT"); break;
	}

	// vertical
	switch (iMode & SC_VTXTALIGN_MASK)
	{
	case TA_BASELINE:	strMode += _T(" TA_BASELINE"); break;
	case TA_BOTTOM:		strMode += _T(" TA_BOTTOM"); break;
	case TA_TOP:		strMode += _T(" TA_TOP"); break;
	}

	// update pos
	if (iMode & TA_UPDATECP)
		strMode += _T(" TA_UPDATECP");

	if (!strMode.IsEmpty())
		strMode.Right(strMode.GetLength()-1);
	return strMode;
}

CString SCPolyFillModeText(int iMode)
{
	switch (iMode)
	{
	case ALTERNATE:	return	_T("ALTERNATE");	
	case WINDING:	return	_T("WINDING");
	}
	
	return _T("??");
}

CString SCFloodFillModeText(int iMode)
{
	switch (iMode)
	{
	case FLOODFILLBORDER:	return	_T("FLOODFILLBORDER");	
	case FLOODFILLSURFACE:	return	_T("FLOODFILLSURFACE");
	}
	
	return _T("??");
}

CString SCRgnModeText(int iMode)
{
	switch (iMode)
	{
	case RGN_AND:	return	_T("RGN_AND");	
	case RGN_COPY:	return	_T("RGN_COPY");
	case RGN_DIFF:	return	_T("RGN_DIFF");	
	case RGN_OR:	return	_T("RGN_OR");
	case RGN_XOR:	return	_T("RGN_XOR");
	}
	
	return _T("RGN_??");
}

CString SCFormatRgnModeText(int iMode)
{
	CString strMode;
	strMode.Format(SC_STRFMT_MODE2,
		iMode, SCRgnModeText(iMode));
	return strMode;
}

CString SCGradientFillModeText(int iMode)
{
	switch (iMode)
	{
	case GRADIENT_FILL_RECT_H:		return	_T("GRADIENT_FILL_RECT_H");	
	case GRADIENT_FILL_RECT_V:		return	_T("GRADIENT_FILL_RECT_V");
	case GRADIENT_FILL_TRIANGLE:	return	_T("GRADIENT_FILL_TRIANGLE");	
	}
	
	return _T("GRADIENT_FILL_??");
}

CString SCLayoutModeText(int iMode)
{
	CString strMode;
	
	if (iMode & LAYOUT_ORIENTATIONMASK)
	{
		if (iMode & LAYOUT_RTL)
			strMode += _T("LAYOUT_RTL|");

		if (iMode & LAYOUT_BTT)
			strMode += _T("LAYOUT_BTT|");

		if (iMode & LAYOUT_VBH)
			strMode += _T("LAYOUT_VBH|");
	}
	if (iMode & LAYOUT_BITMAPORIENTATIONPRESERVED)
		strMode += _T("LAYOUT_BITMAPORIENTATIONPRESERVED|");

	if (strMode.IsEmpty())
		return  _T("<default>");
	
	strMode.TrimRight(_T("|"));
	
	return strMode;
}

CString SCModifyWorldTransformModeText(int iMode)
{
	switch (iMode)
	{
	case MWT_IDENTITY:		return _T("MWT_IDENTITY");
	case MWT_LEFTMULTIPLY:	return _T("MWT_LEFTMULTIPLY");
	case MWT_RIGHTMULTIPLY: return _T("MWT_RIGHTMULTIPLY");
	}
	return _T("MWT_??");
}

CString SCFormatModifyWorldTransformModeText(int iMode)
{
	CString strMode;
	strMode.Format(SC_STRFMT_MODE2,
		iMode, SCModifyWorldTransformModeText(iMode));
	return strMode;
}

CString SCICMModeText(int iMode)
{
	switch (iMode)
	{
	case ICM_ON:				return _T("ICM_ON");
	case ICM_OFF:				return _T("ICM_OFF");
	case ICM_QUERY:				return _T("ICM_QUERY");
	case ICM_DONE_OUTSIDEDC:	return _T("ICM_DONE_OUTSIDEDC"); 
	}
	return _T("ICM_??");
}

CString SCFormatICMModeText(int iMode)
{
	CString strMode;
	strMode.Format(SC_STRFMT_MODE2,
		iMode, SCICMModeText(iMode));
	return strMode;
}

////////////////////////////////////////////////////////////////////////
// Polygons

CString SCBoundsText(RECTL& rclBounds)
{
	CString strText;
	strText.Format(SC_STRFMT_RCLBOUNDS,
		SC_PARAM_RCLBOUNDS(rclBounds));
	
	return strText;
}


CString SCPolygonText(RECTL& rclBounds, POINTL* pPoints, DWORD dwCount)
{
	ASSERT(pPoints);

	CString strText;
	strText.Format(SC_STRFMT_RCLBOUNDS _T(", %s"),
		SC_PARAM_RCLBOUNDS(rclBounds),
		SCPointsText(pPoints, dwCount));
	
	return strText;
}

CString SCPolygonTextS(RECTL& rclBounds, POINTS* pPoints, DWORD dwCount)
{
	ASSERT(pPoints);

	CString strText;
	strText.Format(SC_STRFMT_RCLBOUNDS _T(", %s"),
		SC_PARAM_RCLBOUNDS(rclBounds),
		SCPointsText(pPoints, dwCount));
	
	return strText;
}


CString SCFormatEmrPolygon(EMRPOLYGON* pRec)
{
	ASSERT(pRec);

	CString strText;
	strText.Format(SC_STRFMT_RCLBOUNDS _T(", %s"),
		SC_PARAM_RCLBOUNDS(pRec->rclBounds),
		SCPointsText(pRec->aptl, pRec->cptl));
	
	return strText;
}

CString SCFormatEmrPolygonS(EMRPOLYGON16* pRec)
{
	ASSERT(pRec);

	CString strText;
	strText.Format(SC_STRFMT_RCLBOUNDS _T(", %s"),
		SC_PARAM_RCLBOUNDS(pRec->rclBounds),
		SCPointsText(pRec->apts, pRec->cpts));
	
	return strText;
}

CString SCFormatEmrPolyPolygon(EMRPOLYPOLYGON* pRec)
{
	ASSERT(pRec);

	CString strText;
	strText.Format(SC_STRFMT_RCLBOUNDS _T(", nPolys:%d, %s"),
		SC_PARAM_RCLBOUNDS(pRec->rclBounds),
		pRec->nPolys,
		SCPointsText(pRec->aptl, pRec->cptl));
	
	return strText;
}

CString SCFormatEmrPolyPolygonS(EMRPOLYPOLYGON16* pRec)
{
	ASSERT(pRec);

	CString strText;
	strText.Format(SC_STRFMT_RCLBOUNDS _T(", nPolys:%d, %s"),
		SC_PARAM_RCLBOUNDS(pRec->rclBounds),
		pRec->nPolys,
		SCPointsText(pRec->apts, pRec->cpts));
	
	return strText;
}


////////////////////////////////////////////////////////////////////////
// Regions

CString SCRegionDataText(RGNDATA* pRgnData, DWORD dwSize)
{
	CString strBox;
	if (pRgnData && dwSize)
	{
		CRect rect(0, 0, 0, 0);
		HRGN hRgn = ::ExtCreateRegion(NULL, dwSize, pRgnData);
		if (hRgn)
		{
			::GetRgnBox(hRgn, &rect);
			::DeleteObject(hRgn);

			strBox.Format(_T("%d,%d,%d,%d"),
				SC_PARAM_RECT(rect));
		}
	}
	
	CString strText;
	strText.Format(_T("RGNDATA[ptr:0x%08x, size:%d, box(%s)]"),
		pRgnData,
		dwSize,
		strBox);
	
	return strText;
}

////////////////////////////////////////////////////////////////////////
// Color space

CString SCColorSpaceTypeText(LCSCSTYPE lcsCSType)
{
	switch (lcsCSType)
	{
	case LCS_CALIBRATED_RGB:		return	_T("LCS_CALIBRATED_RGB");	
	case LCS_sRGB:					return	_T("LCS_sRGB");
	case LCS_WINDOWS_COLOR_SPACE:	return	_T("LCS_WINDOWS_COLOR_SPACE");	
	}

	return _T("LCS_??");
}

CString SCColorSpaceIntentText(LCSGAMUTMATCH lcsIntent)
{
	switch (lcsIntent)
	{
	case LCS_GM_ABS_COLORIMETRIC:	return	_T("LCS_GM_ABS_COLORIMETRIC");	
	case LCS_GM_BUSINESS:			return	_T("LCS_GM_BUSINESS");
	case LCS_GM_GRAPHICS:			return	_T("LCS_GM_GRAPHICS");	
	case LCS_GM_IMAGES:				return	_T("LCS_GM_IMAGES");	
	}

	return _T("LCS_GM_??");
}

// Calculate floating point from fixed point
#define SC_INV_FXPT32(dw, f)	(((int)dw + 0.0f) / (1 << f))

#define SCFloatFrom2Dot30(dw)	SC_INV_FXPT32(dw, 30)
#define SCFloatFrom16Dot16(dw)  SC_INV_FXPT32(dw, 16)

CString SCCie2Text(CIEXYZ xyz)
{
	CString strText;
	strText.Format(_T("(x:%.4f, y:%.4f, z:%.4f)"),
		SCFloatFrom2Dot30(xyz.ciexyzX),
		SCFloatFrom2Dot30(xyz.ciexyzY),
		SCFloatFrom2Dot30(xyz.ciexyzZ));
	return strText;
}

template <class T>
CString T_SCFormatLogColorSpaceAsText(T& rlcs)
{
	CString strText;

	CIEXYZTRIPLE& rEP = rlcs.lcsEndpoints;

	CString strAdd;
	strAdd.Format(_T("signature:0x%08X, ver:0x%04X, size:%d, type(%d=%s), intent(%d=%s),")
		_T(" EndPoints[R:%s, G:%s, B:%s], Gamma(R:%.4f, G:%.4f, B:%.4f)"),
		rlcs.lcsSignature,
		rlcs.lcsVersion, 
		rlcs.lcsSize, 
		rlcs.lcsCSType, SCColorSpaceTypeText(rlcs.lcsCSType),
		rlcs.lcsIntent,	SCColorSpaceIntentText(rlcs.lcsIntent), 
		SCCie2Text(rEP.ciexyzRed), SCCie2Text(rEP.ciexyzGreen), SCCie2Text(rEP.ciexyzBlue),
		SCFloatFrom16Dot16(rlcs.lcsGammaRed),
		SCFloatFrom16Dot16(rlcs.lcsGammaGreen), 
		SCFloatFrom16Dot16(rlcs.lcsGammaBlue));

	if (rlcs.lcsFilename)
	{
		strAdd += _T(", file:");
		strAdd += rlcs.lcsFilename;
	}

	strText.Format(_T("LCS[%s]"), strAdd);
	return strText;
}

CString SCFormatLogColorSpaceAsTextA(LOGCOLORSPACEA& rlcs)
{
	return T_SCFormatLogColorSpaceAsText(rlcs);
}

CString SCFormatLogColorSpaceAsTextW(LOGCOLORSPACEW& rlcs)
{
	return T_SCFormatLogColorSpaceAsText(rlcs);
}

CString SCPixelFormatText(PIXELFORMATDESCRIPTOR& rPfd)
{
	CString strText;

	strText.Format(
	 _T("Size:%d, ")
	 _T("Version:%d, ")
	 _T("Flags:%d, ")
	 _T("PixelType:%d, ")
	 _T("ColorBits:%d, ")
	 _T("RedBits:%d, ")
	 _T("RedShift:%d, ")
     _T("GreenBits:%d, ")
     _T("GreenShift:%d, ")
     _T("BlueBits:%d, ")
     _T("BlueShift:%d, ")
     _T("AlphaBits:%d, ")
     _T("AlphaShift:%d, ")
     _T("AccumBits:%d, ")
     _T("AccumRedBits:%d, ")
     _T("AccumGreenBits:%d, ")
     _T("AccumBlueBits:%d, ")
     _T("AccumAlphaBits:%d, ")
     _T("DepthBits:%d, ")
     _T("StencilBits:%d, ")
     _T("AuxBuffers:%d, ")
     _T("LayerType:%d, ")
     _T("Reserved:%d, ")
     _T("LayerMask:%d, ")
     _T("VisibleMask:%d, ")
     _T("DamageMask:%d"),

     rPfd.nSize,
     rPfd.nVersion,
     rPfd.dwFlags,
     rPfd.iPixelType,
     rPfd.cColorBits,
     rPfd.cRedBits,
     rPfd.cRedShift,
     rPfd.cGreenBits,
     rPfd.cGreenShift,
     rPfd.cBlueBits,
     rPfd.cBlueShift,
     rPfd.cAlphaBits,
     rPfd.cAlphaShift,
     rPfd.cAccumBits,
     rPfd.cAccumRedBits,
     rPfd.cAccumGreenBits,
     rPfd.cAccumBlueBits,
     rPfd.cAccumAlphaBits,
     rPfd.cDepthBits,
     rPfd.cStencilBits,
     rPfd.cAuxBuffers,
     rPfd.iLayerType,
     rPfd.bReserved,
     rPfd.dwLayerMask,
     rPfd.dwVisibleMask,
     rPfd.dwDamageMask);
	
	return strText;
}

////////////////////////////////////////////////////////////////////////
// Miscellaneous

CString SCGDICommentText(DWORD dwTag)
{
	switch(dwTag)
	{
	case GDICOMMENT_WINDOWS_METAFILE:	return _T("GDI.Embedded WMF");
	case GDICOMMENT_MULTIFORMATS:		return _T("GDI.Embedded formats");
	case GDICOMMENT_BEGINGROUP:			return _T("GDI.Begin Group");
	case GDICOMMENT_ENDGROUP:			return _T("GDI.End Group");
	case EPS_SIGNATURE:					return _T("GDI.EPS Signature");
	case GDICOMMENT_UNICODE_STRING:		return _T("GDI.UNICODE String");
	case GDICOMMENT_UNICODE_END:		return _T("GDI.UNICODE End");
	}
	
	return  _T("GDI.Private");
}

CString SCGDIObjectTypeText(HGDIOBJ hGdiObj)
{
	CString strText;
	
	switch(GetObjectType(hGdiObj))
	{
	case OBJ_EXTPEN:
		{
			strText = _T("OBJ_EXTPEN");
			EXTLOGPEN LogPen;
			if (::GetObject((HPEN)hGdiObj, sizeof(LogPen), &LogPen))
			{
				strText += _T(".(") + SCPenStyleText(LogPen.elpPenStyle) + _T(")");
			}
		}
		break;
		
	case OBJ_PEN:
		{
			strText = _T("OBJ_PEN");
			LOGPEN LogPen;
			if (::GetObject((HPEN)hGdiObj, sizeof(LogPen), &LogPen))
			{
				strText += _T(".(") + SCPenStyleText(LogPen.lopnStyle) + _T(")");
			}
		}
		break;
		
	case OBJ_BRUSH:
		{
			strText = _T("OBJ_BRUSH");
			LOGBRUSH LogBrush;
			if (::GetObject((HBRUSH)hGdiObj, sizeof(LogBrush), &LogBrush))
			{
				strText += _T(".(") + SCBrushStyleText(LogBrush.lbStyle) + _T(")");
			}
		}
		break;
		
	case OBJ_FONT:			strText = _T("OBJ_FONT"); break;
	case OBJ_REGION:		strText = _T("OBJ_REGION"); break;
	case OBJ_PAL:			strText = _T("OBJ_PAL"); break;
	case OBJ_BITMAP:		strText = _T("OBJ_BITMAP"); break;
		
		// Just for completeness
	case OBJ_MEMDC:			strText = _T("OBJ_MEMDC");	break;
	case OBJ_METAFILE:		strText = _T("OBJ_METAFILE"); break;
	case OBJ_METADC:		strText = _T("OBJ_METADC"); break;
	case OBJ_ENHMETAFILE:	strText = _T("OBJ_ENHMETAFILE"); break;
	case OBJ_ENHMETADC:		strText = _T("OBJ_ENHMETADC"); break;
	case OBJ_DC:			strText = _T("OBJ_DC"); break;
		
		// Error
	default:
		strText.Format(_T("OBJ_?? GDI handle=%d"), hGdiObj);
		break;
	}

	return strText;
}