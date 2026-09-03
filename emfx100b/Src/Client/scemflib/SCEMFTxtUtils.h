/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFTXTUTILS_H_
#define _SCEMFTXTUTILS_H_

/////////////////////////////////////////////////////////////////////////////////////////
inline CString SCFLT2STR(float flt)
{
	CString strF;
	double dbl = flt;
	strF.Format(_T("%f"),dbl);
	return strF;
}

//	#define SC_COMMA				_T(", ")

/////////////////////////////////////////////////////////////////////////////////////////
// Formats and Params to formats
#define SC_STRFMT_POINT			_T("%d,%d")
#define SC_PARAM_POINT(pt)		pt.x, pt.y

#define SC_STRFMT_SIZE			_T("%d,%d")
#define SC_PARAM_SIZE(sz)		sz.cx, sz.cy

#define SC_STRFMT_RECT			_T("rect(%d,%d,%d,%d)")
#define SC_PARAM_RECT(rc)		rc.left, rc.top, rc.right, rc.bottom

#define SC_STRFMT_RCLBOUNDS		_T("rclBounds(%d,%d,%d,%d)")
#define SC_PARAM_RCLBOUNDS(rc)	rc.left, rc.top, rc.right, rc.bottom

#define SC_STRFMT_COLOR			_T("0x%08X")
#define SC_PARAM_COLOR(cr)		cr

#define SC_STRFMT_ROP			_T("dwRop(0x%08X)")
#define SC_PARAM_ROP(n)			n

#define SC_STRFMT_PTR			_T("ptr:0x%08x")
#define SC_PARAM_PTR(p)			p

#define SC_STRFMT_MODE1			_T("iMode(%d)")
#define SC_PARAM_MODE1(n)		n

#define SC_STRFMT_MODE2			_T("iMode(%d=%s)")
#define SC_PARAM_MODE2(n, s)	n, s

#define SC_STRFMT_XFORM			_T("xform(eDx:%s, eDy:%s, eM11:%s, eM12:%s, eM21:%s, eM22:%s)")
#define SC_PARAM_XFORM(xform)	SCFLT2STR(xform.eDx), SCFLT2STR(xform.eDy), SCFLT2STR(xform.eM11), SCFLT2STR(xform.eM12), SCFLT2STR(xform.eM21), SCFLT2STR(xform.eM22)

#define SC_STRFMT_BMPSRC0		_T("iUsage:%d, offBmi:%d, Bmi:%d, offBits:%d, Bits:%d")
#define SC_PARAM_BMPSRC0(pRec)	pRec->iUsageSrc, pRec->offBmiSrc, pRec->cbBmiSrc, pRec->offBitsSrc, pRec->cbBitsSrc

#define SC_STRFMT_BMPDIB		SC_STRFMT_BMPSRC0
#define SC_PARAM_BMPDIB(pRec)	pRec->iUsage, pRec->offBmi, pRec->cbBmi, pRec->offBits, pRec->cbBits

#define SC_STRFMT_BMPSRC		_T("BkColor:0x%08X, iUsage:%d, offBmi:%d, Bmi:%d, offBits:%d, Bits:%d")
#define SC_PARAM_BMPSRC(pRec)	pRec->crBkColorSrc, pRec->iUsageSrc, pRec->offBmiSrc, pRec->cbBmiSrc, pRec->offBitsSrc, pRec->cbBitsSrc

#define SC_STRFMT_DEST_CXCY		_T("Dest[x:%d, y:%d, cx:%d, cy:%d)]")
#define SC_PARAM_DEST_CXCY(pRec) pRec->xDest, pRec->yDest, pRec->cxDest, pRec->cyDest

#define SC_STRFMT_SRC_CXCY		_T("Src[x:%d, y:%d, cx:%d, cy:%d)]")
#define SC_PARAM_SRC_CXCY(pRec)	pRec->xSrc, pRec->ySrc, pRec->cxSrc, pRec->cySrc

#define SC_STRFMT_BLTSRC		_T("Src[x:%d, y:%d, ") SC_STRFMT_XFORM _T(", ") SC_STRFMT_BMPSRC _T("]")
#define SC_PARAM_BLTSRC(pRec)	pRec->xSrc, pRec->ySrc, SC_PARAM_XFORM(pRec->xformSrc), SC_PARAM_BMPSRC(pRec)

#define SC_STRFMT_SBLTSRC		_T("Src[x:%d, y:%d, cx:%d, cy:%d, ") SC_STRFMT_XFORM _T(", ") SC_STRFMT_BMPSRC _T("]")
#define SC_PARAM_SBLTSRC(pRec)	SC_PARAM_SRC_CXCY(pRec), SC_PARAM_XFORM(pRec->xformSrc), SC_PARAM_BMPSRC(pRec)

#define SC_STRFMT_BMPMASK		_T("Mask[x:%d, y:%d, iUsage:%d, offBmi:%d, Bmi:%d, offBits:%d, Bits:%d]")
#define SC_PARAM_BMPMASK(pRec)	pRec->xMask, pRec->xMask, pRec->iUsageMask, pRec->offBmiMask, pRec->cbBmiMask, pRec->offBitsMask, pRec->cbBitsMask

#define SC_STRFMT_SETBITSSRC		_T("Src[x:%d, y:%d, cx:%d, cy:%d, ") SC_STRFMT_BMPSRC0 _T("]")
#define SC_PARAM_SETBITSSRC(pRec)	SC_PARAM_SRC_CXCY(pRec), SC_PARAM_BMPSRC0(pRec)

#define SC_STRFMT_TRIPOINT		_T("(%d,%d), (%d,%d), (%d,%d)")
#define SC_PARAM_TRIPOINT(pt)	pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y

#define SC_STRFMT_TXTFORM		_T("[exScale(%f) eyScale(%f) iGraphicsMode(%d), Bounds(%d,%d,%d,%d)]")
#define SC_PARAM_TXTFORM(pRec)	pRec->exScale, pRec->eyScale, pRec->iGraphicsMode, pRec->rclBounds

/////////////////////////////////////////////////////////////////////////////////////////
//

LPTSTR SCGetRecordName(int iCode);

inline CString SCObjectIndexText(DWORD dwIndex)
{
	CString strText;
	if (dwIndex & 0x80000000) // this is a stock object
		strText.Format(_T("Stock object: %d"), dwIndex & 0x7fffffff);
	else
		strText.Format(_T("Table object: %d"), dwIndex);
	return strText;
}

CString SCFormatTextOptions(UINT uiOptions);
CString STFormatEmrTextA(EMRTEXT &emrtext, EMREXTTEXTOUTA *pRec);
CString STFormatEmrTextW(EMRTEXT &emrtext, EMREXTTEXTOUTW *pRec);

CString SCRop2Text(INT iRop2);
CString SCPenStyleText(UINT uiPenStyle);
CString SCHatchStyleText(UINT uiHatchStyle);
CString SCBrushStyleText(UINT uiStyle);
CString SCLogBrushStyleText(LOGBRUSH32& rLb);
CString SCExtLogpenBrushStyleText(EMREXTCREATEPEN *pRec);
CString SCDIBColorUsageText(UINT uiUsage);

CString SCMapModeText(int iMode);
CString SCBkModeText(int iMode);
CString SCStretchBltModeText(int iMode);
CString SCTextAlignModeText(int iMode);
CString SCPolyFillModeText(int iMode);
CString SCFloodFillModeText(int iMode);
CString SCRgnModeText(int iMode);
CString SCGradientFillModeText(int iMode);
CString SCLayoutModeText(int iMode);

CString SCFormatRgnModeText(int iMode);

CString SCModifyWorldTransformModeText(int iMode);
CString SCFormatModifyWorldTransformModeText(int iMode);
CString SCICMModeText(int iMode);
CString SCFormatICMModeText(int iMode);

template <class T>
inline CString SCPointsText(T* pPoints, DWORD dwCount)
{
	ASSERT(pPoints);

	CString strText;
	strText.Format(_T("nbPoints:%d, P1(%d,%d) - Pn(%d,%d)"),
		dwCount,
		SC_PARAM_POINT(pPoints[0]),
		SC_PARAM_POINT(pPoints[dwCount - 1]));

	return strText;
}

CString SCBoundsText(RECTL& rclBounds);

CString SCPolygonText(RECTL& rclBounds, POINTL* pPoints, DWORD dwCount);
CString SCPolygonTextS(RECTL& rclBounds, POINTS* pPoints, DWORD dwCount);

CString SCFormatEmrPolygon(EMRPOLYGON* pRec);
CString SCFormatEmrPolygonS(EMRPOLYGON16* pRec);

CString SCFormatEmrPolyPolygon(EMRPOLYPOLYGON* pRec);
CString SCFormatEmrPolyPolygonS(EMRPOLYPOLYGON16* pRec);

CString SCRegionDataText(RGNDATA* pRgnData, DWORD dwSize);

CString SCBmiText(BITMAPINFOHEADER *pBmi);

CString SCFormatLogColorSpaceAsTextA(LOGCOLORSPACEA& rlcs);
CString SCFormatLogColorSpaceAsTextW(LOGCOLORSPACEW& rlcs);
CString SCPixelFormatText(PIXELFORMATDESCRIPTOR& rPfd);

CString SCGDICommentText(DWORD dwTag);
CString SCGDIObjectTypeText(HGDIOBJ hGdiObj);

#endif //_SCEMFTXTUTILS_H_
//  ------------------------------------------------------------
