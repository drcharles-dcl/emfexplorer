/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFdcRenderer.h"

using namespace Gdiplus;

// Debug stuff
#ifdef _DEBUG

void SCMarkBox(GDPGraphics* pGraphics, RECT rcBox, COLORREF crColor,
			   int iDltx=0, int iDlty=0, int iDltR=0, int iDltB=0);
#define SCSHOW_REDLIMIT() \
{ \
	RECT box;\
	::CopyRect(&box, &m_RcClipBox);\
	::InflateRect(&box, -1, -1);\
	SCDPtoLP((Point*)&box, 2);\
	SCMarkBox(m_pGraphics, box, RGB(255, 0, 0));\
}

#define SCSHOW_BLUELIMIT() \
{ \
	RECT box;\
	::CopyRect(&box, &m_RcClipBox);\
	::InflateRect(&box, -2, -2);\
	SCDPtoLP((Point*)&box, 2);\
	SCMarkBox(m_pGraphics, box, RGB(0, 0, 255));\
}

#define SCSHOW_DPRCLIMIT(rbox, rcolor) \
{ \
	RECT box;\
	::CopyRect(&box, &rbox);\
	::InflateRect(&box, -2, -2);\
	SCDPtoLP((Point*)&box, 2);\
	SCMarkBox(m_pGraphics, box, rcolor);\
}
#define SCSHOW_LPRCLIMIT(rbox, rcolor) \
{ \
	RECT box;\
	::CopyRect(&box, &rbox);\
	::InflateRect(&box, -4, -4);\
	SCMarkBox(m_pGraphics, box, rcolor);\
}

#else

#define SCSHOW_REDLIMIT()
#define SCSHOW_BLUELIMIT()
#endif



///////////////////////////////////////////////////////////////////////////////////////
// Regions/Clipping rects management
//

void CSCEMFdcRenderer::SCIntersectClipRect(RECTL& rRect)
{
	ASSERT(m_pGraphics);

	Rect R(rRect.left, rRect.top, RECT_WIDTH(rRect), RECT_HEIGHT(rRect));
	m_pGraphics->SetClip(R, CombineModeIntersect);
}

void CSCEMFdcRenderer::SCExcludeClipRect(RECTL& rRect)
{
	ASSERT(m_pGraphics);

	Rect R(rRect.left, rRect.top, RECT_WIDTH(rRect), RECT_HEIGHT(rRect));
	m_pGraphics->SetClip(R, CombineModeExclude);
}

void CSCEMFdcRenderer::SCOffsetClipRect(POINTL& rPtOffset)
{
	ASSERT(m_pGraphics);
	m_pGraphics->TranslateClip(rPtOffset.x, rPtOffset.y);

#pragma message( __FILE__  "(90): TODO: Consider cropping ")
	//if (would overflow)
	//SCCropToPlayBox(CombineModeIntersect);
}

///
/// Use REGIONDATA pointed to by pData to select a new GDI+ region.
///
void CSCEMFdcRenderer::SCSelectClipRgn(LPCBYTE pData, DWORD dwSize, DWORD dwMode)
{
	ASSERT(m_pGraphics);
	if (!dwSize || !pData)
	{
		// This would allow writing outside the original playing rectangle:
		//	m_pGraphics->ResetClip();
		SCCropToPlayBox();
	} else
	{
		// This generates jagged regions, as we don't have a region smoothing function
		HRGN hRgn = SCGetHRGNFromREGIONDATA(pData, dwSize);
		ASSERT(hRgn);

#if 0
		// won't work (maybe this SetClip expects device coordinates in hRgn)
		m_pGraphics->SetClip(hRgn, SCClipModeFormRGNMode(dwMode));
#else
		// hRgn contains world coordinates
		Region region(hRgn);
		ASSERT(Ok==region.GetLastStatus());
		m_pGraphics->SetClip(&region, SCClipModeFormRGNMode(dwMode));
#endif
		DeleteObject(hRgn); // assume the GDI+ object doesn't need it anymore
		Status iRes = m_pGraphics->GetLastStatus();
		ASSERT(Ok==iRes);
		// Can't explain an iRes==OutOfMemory for a region containing 1 rect of (790x600)
		// while the scale transform is m11=m22=0.066724
	}
}

///
/// Create a GDI region from REGIONDATA.
/// "The ExtSelectClipRgn function assumes that the coordinates
/// for the specified region are specified in device units". So we assume that the region
/// data is in device coordinates.
///
/// "A GDI+ region is stored in world coordinates whereas a GDI region is stored in device coordinates."
/// We prepare the region in world coordinates (ready for a GDI+ region constructor).
///
HRGN CSCEMFdcRenderer::SCGetHRGNFromREGIONDATA(LPCBYTE pData, DWORD dwSize)
{
	ASSERT(pData && dwSize);

	LPBYTE pBytes = new BYTE[dwSize];
	ASSERT(pBytes);
	if (!pBytes)
		return NULL;
	memmove(pBytes, pData, dwSize);

	RGNDATA* pRgnData = (RGNDATA *)pBytes;
	ASSERT(pRgnData->rdh.iType == RDH_RECTANGLES);

	Matrix matrix;
	m_pGraphics->GetTransform(&matrix);
	matrix.Invert();

	RECT* pRect = (RECT*)pRgnData->Buffer;
	matrix.TransformPoints((Point*)pRgnData->Buffer, pRgnData->rdh.nCount*2); // DP to LP

	HRGN hRgn = ::ExtCreateRegion(NULL, dwSize, pRgnData);
	ASSERT(hRgn);

	// Meta region simulation
	CRect rcMeta(m_RcClipBox.left, m_RcClipBox.top, m_RcClipBox.right, m_RcClipBox.bottom);
	matrix.TransformPoints((Point*)&rcMeta, 2); // DP to LP
	HRGN hRgnMeta = ::CreateRectRgn(rcMeta.left, rcMeta.top, rcMeta.right, rcMeta.bottom);

	ASSERT(hRgnMeta);
	int iRes = CombineRgn(hRgn, hRgn, hRgnMeta, RGN_AND);
	ASSERT(iRes!=ERROR);
	
	DeleteObject(hRgnMeta);

	delete [] pBytes;
	return hRgn;
}

///
/// Fill a region by using the specified brush.
///
void CSCEMFdcRenderer::SCFillRgn(HBRUSH hBrush, LPCBYTE pData, DWORD dwSize)
{
	ASSERT(m_pGraphics);
	ASSERT(pData && dwSize);
	ASSERT(hBrush);

	HRGN hRgn = SCGetHRGNFromREGIONDATA(pData, dwSize);
	ASSERT(hRgn);

	Region region(hRgn);
	DeleteObject(hRgn); // assume the GDI+ object doesn't need it anymore

	LOGBRUSH LogBrush;
	::GetObject(hBrush, sizeof(LOGBRUSH), &LogBrush);

	SCShortDCState TmpDCState(m_bMonochrome, m_dwROP2, m_TextColor, m_BkColor, m_dwBkMode);
	Brush* pBrush = SCBrushFromLogBrush(LogBrush, TmpDCState);

	m_pGraphics->FillRegion(pBrush, &region);

	delete pBrush;
}

///
/// Draw a border around the specified region by using the specified brush.
///
void CSCEMFdcRenderer::SCFrameRgn(HBRUSH hBrush, SIZEL szlStroke, LPCBYTE pData, DWORD dwSize)
{
	ASSERT(m_pGraphics);
	ASSERT(pData && dwSize);
	ASSERT(hBrush);

	HRGN hRgn = SCGetHRGNFromREGIONDATA(pData, dwSize);
	ASSERT(hRgn);

	Region region(hRgn);
	DeleteObject(hRgn); // assume the GDI+ object doesn't need it anymore

	LOGBRUSH LogBrush;
	::GetObject(hBrush, sizeof(LOGBRUSH), &LogBrush);

	SCShortDCState TmpDCState(m_bMonochrome, m_dwROP2, m_TextColor, m_BkColor, m_dwBkMode);
	Brush* pBrush = SCBrushFromLogBrush(LogBrush, TmpDCState);

	Rect Bounds;
	region.GetBounds(&Bounds, m_pGraphics);

	Rect rect = Bounds;
	// vertical bands
	rect.X -= szlStroke.cx; // account for V stroke width
	rect.Width = szlStroke.cx;
	m_pGraphics->FillRectangle(pBrush, rect);

	rect.X += Bounds.Width + szlStroke.cx;
	m_pGraphics->FillRectangle(pBrush, rect);

	// horizontal bands
	rect.X = Bounds.X - szlStroke.cx; // account for V stroke width
	rect.Y = Bounds.Y - szlStroke.cy; // account for H stroke width
	rect.Height = szlStroke.cy;
	rect.Width = Bounds.Width + 2*szlStroke.cx;
	m_pGraphics->FillRectangle(pBrush, rect);

	rect.Y += Bounds.Height + szlStroke.cy;
	m_pGraphics->FillRectangle(pBrush, rect);

	delete pBrush;
}

///
/// Invert the colors in the specified region.
///
void CSCEMFdcRenderer::SCInvertRgn(LPCBYTE pData, DWORD dwSize)
{
	ASSERT(m_pGraphics);
	ASSERT(pData && dwSize);

	HRGN hRgn = SCGetHRGNFromREGIONDATA(pData, dwSize);
	ASSERT(hRgn);

	Region region(hRgn);
	DeleteObject(hRgn); // assume the GDI+ object doesn't need it anymore

	Region SvdClipRgn;
	m_pGraphics->GetClip(&SvdClipRgn);

	m_pGraphics->SetClip(&region, CombineModeIntersect);

	Rect Bounds;
	region.GetBounds(&Bounds, m_pGraphics);

	RECT rcRgnBox;
	rcRgnBox.left = Bounds.X;
	rcRgnBox.top = Bounds.Y;
	rcRgnBox.right = rcRgnBox.left + Bounds.Width;
	rcRgnBox.bottom = rcRgnBox.top + Bounds.Height;
	
	SCPatBlt(&rcRgnBox, DSTINVERT, NULL);

	m_pGraphics->SetClip(&SvdClipRgn);
}

///
/// Paint the specified region by using the current brush
///
void CSCEMFdcRenderer::SCPaintRgn(LPCBYTE pData, DWORD dwSize)
{
	ASSERT(m_pGraphics);
	ASSERT(pData && dwSize);
	ASSERT(m_pBrush);

	HRGN hRgn = SCGetHRGNFromREGIONDATA(pData, dwSize);
	ASSERT(hRgn);

	Region region(hRgn);
	DeleteObject(hRgn); // assume the GDI+ object doesn't need it anymore

	Region SvdClipRgn;
	m_pGraphics->GetClip(&SvdClipRgn);

	m_pGraphics->SetClip(&region, CombineModeIntersect);

	Rect Bounds;
	region.GetBounds(&Bounds, m_pGraphics);

	RECT rcRgnBox;
	rcRgnBox.left = Bounds.X;
	rcRgnBox.top = Bounds.Y;
	rcRgnBox.right = rcRgnBox.left + Bounds.Width;
	rcRgnBox.bottom = rcRgnBox.top + Bounds.Height;
	
	SCPatBlt(&rcRgnBox, PATPAINT, NULL);

	m_pGraphics->SetClip(&SvdClipRgn);
}

///
/// Reset the clipping region after meta region has changed.
///
/// The current clipping region of a device context is defined by the intersection
/// of its clipping region and its metaregion.
///
void CSCEMFdcRenderer::SCOnSetMetaRgn()
{
	// After SetMetaRgn, "the clipping region is reset to a null region".
	// So we do a reset.
#ifdef _DEBUG
	{// check that there is no clipping region 
		HRGN hRgn = ::CreateRectRgn(0, 0, 0, 0);
		ASSERT(hRgn);
		if (hRgn)
		{
			ASSERT(0==::GetClipRgn(m_hDC, hRgn));
		}
	}
#endif

	// In case of problem, activate and call SCOnChangeClipping() instead of this
	SCCropToPlayBox();
}

///
/// Ensure no writing occurs outside the original playing rectangle
///
void CSCEMFdcRenderer::SCCropToPlayBox(CombineMode nCropMode/*=CombineModeReplace*/)
{
	// Meta region simulation
	CRect rcMeta(m_RcClipBox.left, m_RcClipBox.top, m_RcClipBox.right, m_RcClipBox.bottom);
	SCDPtoLP((Point*)&rcMeta, 2);
	Rect rectClip(rcMeta.left, rcMeta.top, RECT_WIDTH(rcMeta), RECT_HEIGHT(rcMeta));
	m_pGraphics->SetClip(rectClip, nCropMode);
}


#if 0
// Code to activate in case of problems with metaregion
///
///	Convert the current clipping region in m_hDC to its corresponding GDI+ region.
///
void CSCEMFdcRenderer::SCOnChangeClipping()
{
	HRGN hRgn = ::CreateRectRgn(0, 0, 0, 0);
	ASSERT(hRgn);
	if (hRgn)
	{
		switch (::GetClipRgn(m_hDC, hRgn))
		{
		case 1:
			m_pGraphics->SetClip(hRgn);
			break;

		case 0:
			SCCropToPlayBox();
			break;

		default:
			ASSERT(0);
		}

		::DeleteObject(hRgn);
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////////////
// Debug helpers
//

#ifdef _DEBUG
void SCMarkBox(GDPGraphics* pGraphics, RECT rcBox, COLORREF crColor,
			   int iDltx/*=0*/, int iDlty/*=0*/, int iDltR/*=0*/, int iDltB/*=0*/)
{
	Color PenColor;
	PenColor.SetFromCOLORREF(crColor);
	Pen pen(PenColor, 1.0);
	Rect rect(rcBox.left + iDltx, rcBox.top + iDlty,
		RECT_WIDTH(rcBox) - (iDltx + iDltR), RECT_HEIGHT(rcBox) - (iDlty + iDltB));
	pGraphics->DrawRectangle(&pen, rect);
}
#endif


