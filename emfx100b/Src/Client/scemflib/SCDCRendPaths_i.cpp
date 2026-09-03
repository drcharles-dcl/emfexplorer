/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "SCEMFdcRenderer.h"

using namespace Gdiplus;

///////////////////////////////////////////////////////////////////////////////////////
// Paths management
//
// Windows NT: The following drawing functions define points in a path: 
//	
//	AngleArc	CloseFigure	MoveToEx		PolyDraw	PolyPolygon		TextOut	
//	Arc			Ellipse		Pie				Polygon		PolyPolyline		
//	ArcTo		ExtTextOut	PolyBezier		Polyline	Rectangle    	
//	Chord		LineTo		PolyBezierTo	PolylineTo	RoundRect  	
//	
//	
// Windows 95 and Windows 98: When constructing a path, only the
//
// CloseFigure, ExtTextOut, LineTo, MoveToEx, PolyBezier, PolyBezierTo,
// Polygon, Polyline, PolylineTo, PolyPolygon, PolyPolyline, and TextOut
//
// functions are recorded.
///////////////////////////////////////////////////////////////////////////////////////

///
/// Open a path bracket. Note that:
///	- SaveDC/RestoreDC are not legal during path recording.
///	- BeginPath/EndPath are not legal during path recording.
/// So we don't really need a path holder for reference counting.
/// But what if a SaveDC/RestoreDC pair occurs just after EndPath, but
/// BEFORE stroking or filling the current path? We don't know. So just in case...
///
void CSCEMFdcRenderer::SCBeginPath()
{
	ASSERT(m_pGraphics);
	
	// Discard any existing path
	SC_OBJHOLDER_RELEASE_T(m_pPathHolder, m_pPath, SCPath);

	m_pPath = new SCPath((ALTERNATE==m_dwFillMode) ? FillModeAlternate : FillModeWinding);

	SC_OBJHOLDER_RENEW_T(m_pPathHolder, m_pPath, SCPath);
}

void CSCEMFdcRenderer::SCCloseFigure()
{
	ASSERT(m_pGraphics);
	ASSERT(m_pPath);
	m_pPath->CloseFigure();
}

///
/// Close a path bracket. 
///
void CSCEMFdcRenderer::SCEndPath()
{
	ASSERT(m_pGraphics);
	ASSERT(m_pPath);

	// No GDI+ equivalent action

	// But this object is now in a fragile state. If no fucntion is called that clears
	// the path (stroke, fill, etc.), no drawing will occur.
	m_pPath->SCSetEnd();
}

///
/// Discard a path bracket. 
///
void CSCEMFdcRenderer::SCAbortPath()
{
	ASSERT(m_pGraphics);
	ASSERT(m_pPath);
	SC_OBJHOLDER_RELEASE_T(m_pPathHolder, m_pPath, SCPath);
}

void CSCEMFdcRenderer::SCApplyPath(DWORD dwDrawType/*=SC_PATH_STROKEANDFILL*/, INT iClipMode/*=RGN_COPY*/)
{
	ASSERT(m_pGraphics);
	// ASSERT(m_pPath);
	if (!m_pPath)
		return;

	if (SC_PATH_CLIP==dwDrawType)
	{
		m_pGraphics->SetClip(m_pPath, SCClipModeFormRGNMode(iClipMode));
		if (RGN_COPY==iClipMode)
			SCCropToPlayBox(CombineModeIntersect);
	} else
	{
		if (dwDrawType & SC_PATH_FILL)
		{
			ASSERT(m_pBrush);
			m_pGraphics->FillPath(m_pBrush, m_pPath);
		}
		if (dwDrawType & SC_PATH_STROKE)
		{
			ASSERT(m_pPen);
			m_pGraphics->DrawPath(m_pPen, m_pPath);
		}
	}
	
	// Discard the path after using it
	// (We're trying to mimic GDI, not playing any bogus EMF.)
	SC_OBJHOLDER_RELEASE_T(m_pPathHolder, m_pPath, SCPath);
}

void CSCEMFdcRenderer::SCFlattenPath()
{
	ASSERT(m_pGraphics);
	ASSERT(m_pPath);
	m_pPath->Flatten();
}

void CSCEMFdcRenderer::SCWidenPath()
{
	ASSERT(m_pGraphics);
	ASSERT(m_pPath);
	ASSERT(m_pPen);

	m_pPath->Widen(m_pPen);
}

///
///	Use a computed path as clipping region.
///
void CSCEMFdcRenderer::SCSetClipPath(LPCPOINT pPoints, LPCBYTE pTypes, DWORD dwCount, INT iClipMode)
{
	ASSERT(m_pGraphics);

	FillMode fillMode = (ALTERNATE==m_dwFillMode) ? FillModeAlternate : FillModeWinding;
	GraphicsPath CurPath((Point*)pPoints, pTypes, dwCount, fillMode);

	m_pGraphics->SetClip(&CurPath, SCClipModeFormRGNMode(iClipMode));
	if (RGN_COPY==iClipMode)
		SCCropToPlayBox(CombineModeIntersect);
}

///
///	Fill/stroke a computed path.
///
void CSCEMFdcRenderer::SCDrawPath(LPCPOINT pPoints, LPCBYTE pTypes, DWORD dwCount, DWORD dwDrawType/*=SC_PATH_STROKEANDFILL*/)
{
	ASSERT(m_pGraphics);

	FillMode fillMode = (ALTERNATE==m_dwFillMode) ? FillModeAlternate : FillModeWinding;
	GraphicsPath CurPath((Point*)pPoints, pTypes, dwCount, fillMode);

	if (dwDrawType & SC_PATH_FILL)
	{
		ASSERT(m_pBrush);
		m_pGraphics->FillPath(m_pBrush, &CurPath);
	}

	if (dwDrawType & SC_PATH_STROKE)
	{
		ASSERT(m_pPen);
		m_pGraphics->DrawPath(m_pPen, &CurPath);
	}
}


