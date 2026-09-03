/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFdcRenderer.h"

using namespace Gdiplus;

///////////////////////////////////////////////////////////////////////////////////////
// Lines and Curves management
//

///
///	Draw a set of lines, using current pen and brush in the spirit of GDI. 32-bit points.
///
void CSCEMFdcRenderer::SCDrawLines(POINT* pPoints, DWORD dwCount)
{
	T_SCDrawLines(pPoints, dwCount);
}

///
///	Draw a set of lines, using current pen and brush in the spirit of GDI. 16-bit points.
///
void CSCEMFdcRenderer::SCDrawLinesS(POINTS* pPoints, DWORD dwCount)
{
	T_SCDrawLines(pPoints, dwCount);
}

///
///	Draw a set of lines, using current pen and brush in the spirit of GDI. 32-bit points.
///
void CSCEMFdcRenderer::SCDrawLinesTo(POINT* pPoints, DWORD dwCount)
{
	ASSERT(m_pGraphics);
	Point* Pts = SCPointFromPOINTTo(m_PtCurPos, pPoints, dwCount);
	if (m_pPath)
		m_pPath->AddLines(Pts, dwCount+1);
	else
	{
		//ASSERT(m_pPen);
		if (m_pPen)
			m_pGraphics->DrawLines(m_pPen, Pts, dwCount+1);
	}
	delete [] Pts;
	// update current position
	SCUpdateCurPos(pPoints[dwCount-1].x, pPoints[dwCount-1].y);
}

///
///	Draw a set of lines, using current pen and brush in the spirit of GDI. 16-bit points.
///
void CSCEMFdcRenderer::SCDrawLinesToS(POINTS* pPoints, DWORD dwCount)
{
	ASSERT(m_pGraphics);
	Point* Pts = SCPointFromPOINTTo(m_PtCurPos, pPoints, dwCount);
	if (m_pPath)
		m_pPath->AddLines(Pts, dwCount+1);
	else
	{
		ASSERT(m_pPen);
		m_pGraphics->DrawLines(m_pPen, Pts, dwCount+1);
	}
	delete [] Pts;
	// update current position
	SCUpdateCurPos(pPoints[dwCount-1].x, pPoints[dwCount-1].y);
}


template <class T>
inline void CSCEMFdcRenderer::T_SCDrawLines(T* pPoints, DWORD dwCount)
{
	ASSERT(m_pGraphics);

	Point* Pts = SCPointFromPOINT(pPoints, dwCount);
	if (m_pPath)
		m_pPath->AddLines(Pts, dwCount);
	else
	{
		//ASSERT(m_pPen);
		if (m_pPen)
			m_pGraphics->DrawLines(m_pPen, Pts, dwCount);
	}
	delete [] Pts;

	// update current position
	SCUpdateCurPos(pPoints[dwCount-1].x, pPoints[dwCount-1].y);
}

///////////////////////////////////////////////////////////////////////////////////////
// Bezier
//

///
///	Draw Bezier curves, using current pen and brush in the spirit of GDI. 32-bit points.
///
void CSCEMFdcRenderer::SCDrawBezier(POINT* pPoints, DWORD dwCount)
{
	ASSERT(m_pGraphics);
	Point* Pts = SCPointFromPOINT(pPoints, dwCount);
	if (m_pPath)
		m_pPath->AddBeziers(Pts, dwCount);
	else
	{
		ASSERT(m_pPen);
		m_pGraphics->DrawBeziers(m_pPen, Pts, dwCount);
	}
	delete [] Pts;
	// update current position
	SCUpdateCurPos(pPoints[dwCount-1].x, pPoints[dwCount-1].y);
}

///
///	Draw Bezier curves, using current pen and brush in the spirit of GDI. 16-bit points.
///
void CSCEMFdcRenderer::SCDrawBezierS(POINTS* pPoints, DWORD dwCount)
{
	ASSERT(m_pGraphics);
	Point* Pts = SCPointFromPOINT(pPoints, dwCount);
	if (m_pPath)
		m_pPath->AddBeziers(Pts, dwCount);
	else
	{
		ASSERT(m_pPen);
		m_pGraphics->DrawBeziers(m_pPen, Pts, dwCount);
	}
	delete [] Pts;
	// update current position
	SCUpdateCurPos(pPoints[dwCount-1].x, pPoints[dwCount-1].y);
}

///
///	Draw Bezier curves, using current point in DC as the first point,
/// pen and brush in the spirit of GDI. 32-bit points.
///
void CSCEMFdcRenderer::SCDrawBezierTo(POINT* pPoints, DWORD dwCount)
{
	ASSERT(m_pGraphics);
	Point* Pts = SCPointFromPOINTTo(m_PtCurPos, pPoints, dwCount);
	if (m_pPath)
		m_pPath->AddBeziers(Pts, dwCount+1);
	else
	{
		ASSERT(m_pPen);
		m_pGraphics->DrawBeziers(m_pPen, Pts, dwCount+1);
	}
	delete [] Pts;
	// update current position
	SCUpdateCurPos(pPoints[dwCount-1].x, pPoints[dwCount-1].y);
}

///
///	Draw Bezier curves, using current point in DC as the first point,
/// pen and brush in the spirit of GDI. 16-bit points.
///
void CSCEMFdcRenderer::SCDrawBezierToS(POINTS* pPoints, DWORD dwCount)
{
	ASSERT(m_pGraphics);
	Point* Pts = SCPointFromPOINTTo(m_PtCurPos, pPoints, dwCount);
	if (m_pPath)
		m_pPath->AddBeziers(Pts, dwCount+1);
	else
	{
		ASSERT(m_pPen);
		m_pGraphics->DrawBeziers(m_pPen, Pts, dwCount+1);
	}
	delete [] Pts;
	// update current position
	SCUpdateCurPos(pPoints[dwCount-1].x, pPoints[dwCount-1].y);
}

