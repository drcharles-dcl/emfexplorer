/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFdcRenderer.h"

using namespace Gdiplus;

///////////////////////////////////////////////////////////////////////////////////////
// Polygons/Polypolygons/Geometric shapes management
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// Polygons/Polypolygons
//

///
///	Draw a polygon, using current pen and brush in the spirit of GDI. 32-bit points.
///
void CSCEMFdcRenderer::SCDrawPolygon(POINT* pPoints, DWORD dwCount)
{
	T_SCDrawPolygon(pPoints, dwCount);
}

///
///	Draw a polygon, using current pen and brush in the spirit of GDI. 16-bit points.
///
void CSCEMFdcRenderer::SCDrawPolygonS(POINTS* pPoints, DWORD dwCount)
{
	T_SCDrawPolygon(pPoints, dwCount);
}

// In fact, it is not sure that inlining will take effect
template <class T>
inline void CSCEMFdcRenderer::T_SCDrawPolygon(T* pPoints, DWORD dwCount)
{
	ASSERT(m_pGraphics);

	Point* Pts = SCPointFromPOINT(pPoints, dwCount);
	if (m_pPath)
		m_pPath->AddPolygon(Pts, dwCount);
	else
	{
		if (m_pBrush)
		{
			FillMode fillMode = (ALTERNATE==m_dwFillMode) ? FillModeAlternate : FillModeWinding;
			m_pGraphics->FillPolygon(m_pBrush, Pts, dwCount, fillMode);
		}

		if (m_pPen)
			m_pGraphics->DrawPolygon(m_pPen, Pts, dwCount);
#if 1
		else
		if (m_pBrush)
		{// Potential bug here.
		 // Reason for this code is: FillPolygon only fills the interior of polygons
		 // (the border of a polygon is not filled, making holes in gradient layouts)
			switch(m_pBrush->GetType())
			{
			case BrushTypeSolidColor:
				{
					Color BrushColor;
					((SolidBrush*)m_pBrush)->GetColor(&BrushColor);
					Pen tmpPen(BrushColor, 1);
					m_pGraphics->DrawPolygon(&tmpPen, Pts, dwCount);
				}
				break;
			}
		}
#endif
	}
	delete [] Pts;
	// update current position
	SCUpdateCurPos(pPoints[dwCount-1].x, pPoints[dwCount-1].y);
}

void CSCEMFdcRenderer::SCDrawPolyPolygon(POINT* pPoints, DWORD dwCount, DWORD *pPolyVertices, DWORD dwNbPolys)
{
	T_SCDrawPolyPoly(pPoints, dwCount, pPolyVertices, dwNbPolys, FALSE);
}

void CSCEMFdcRenderer::SCDrawPolyPolygonS(POINTS* pPoints, DWORD dwCount, DWORD *pPolyVertices, DWORD dwNbPolys)
{
	T_SCDrawPolyPoly(pPoints, dwCount, pPolyVertices, dwNbPolys, FALSE);
}

void CSCEMFdcRenderer::SCDrawPolyPolyline(POINT* pPoints, DWORD dwCount, DWORD *pPolyVertices, DWORD dwNbPolys)
{
	T_SCDrawPolyPoly(pPoints, dwCount, pPolyVertices, dwNbPolys, TRUE);
}

void CSCEMFdcRenderer::SCDrawPolyPolylineS(POINTS* pPoints, DWORD dwCount, DWORD *pPolyVertices, DWORD dwNbPolys)
{
	T_SCDrawPolyPoly(pPoints, dwCount, pPolyVertices, dwNbPolys, TRUE);
}

template <class T>
inline void CSCEMFdcRenderer::T_SCDrawPolyPoly(T* pPoints, DWORD dwCount, DWORD *pPolyVertices, DWORD dwNbPolys, BOOL bLines)
{
	ASSERT(m_pGraphics);

	Point* Pts = SCPointFromPOINT(pPoints, dwCount);
	Point* pPolyPts = Pts;
	if (m_pPath)
	{
		if (bLines)
		{
			for (DWORD j=0; (j<dwNbPolys); j++)
			{
				m_pPath->AddLines(pPolyPts, pPolyVertices[j]);
				pPolyPts += pPolyVertices[j];
			}
		} else
		{
			for (DWORD j=0; (j<dwNbPolys); j++)
			{
				m_pPath->AddPolygon(pPolyPts, pPolyVertices[j]);
				pPolyPts += pPolyVertices[j];
			}
		}
	} else
	{	
		if (bLines)
		{
			ASSERT(m_pPen);
			if (m_pPen)
			{
				for (DWORD j=0; (j<dwNbPolys); j++)
				{
					m_pGraphics->DrawLines(m_pPen, pPolyPts, pPolyVertices[j]);
					pPolyPts += pPolyVertices[j];
				}
			}
		} else
		{
			if (m_pBrush)
			{// Anyway, GDI+ is not powerful enough to do correct alternate filling in this case

					// So we must do something
				if (WINDING==m_dwFillMode)
				{
					// Every area will be 'winded'
					for (DWORD j=0; (j<dwNbPolys); j++)
					{
						m_pGraphics->FillPolygon(m_pBrush, pPolyPts, pPolyVertices[j], FillModeWinding);
						pPolyPts += pPolyVertices[j];
					}
				} else
				{
					// Fill with a special data arrangement
					DWORD dwNewNbPolys = dwNbPolys;
					DWORD* pNewVertices = new DWORD[dwNewNbPolys];
					for (DWORD i=0; (i<dwNewNbPolys); i++)
						pNewVertices[i] = pPolyVertices[i];
					
					Point* pNewPoints = SCConvertPolyPoly(Pts, dwCount, pNewVertices, dwNewNbPolys);
					pPolyPts = pNewPoints;
					for (DWORD j=0; (j<dwNewNbPolys); j++)
					{
						m_pGraphics->FillPolygon(m_pBrush, pPolyPts, pNewVertices[j], FillModeAlternate);
						pPolyPts += pNewVertices[j];
					}
					delete [] pNewPoints;
					delete [] pNewVertices;
				}
			}

			if (m_pPen)
			{
				// Stroke with old data
				pPolyPts = Pts;
				for (DWORD j=0; (j<dwNbPolys); j++)
				{
					m_pGraphics->DrawPolygon(m_pPen, pPolyPts, pPolyVertices[j]);
					pPolyPts += pPolyVertices[j];
				}
			}
		}
	}
	delete [] Pts;
	// update current position
	SCUpdateCurPos(pPoints[dwCount-1].x, pPoints[dwCount-1].y);
}

void CSCEMFdcRenderer::SCDrawPolyDraw(POINT* pPoints, DWORD dwCount, BYTE *pTypes)
{
	T_SCDrawPolyDraw(pPoints, dwCount, pTypes);
}

void CSCEMFdcRenderer::SCDrawPolyDrawS(POINTS* pPoints, DWORD dwCount, BYTE *pTypes)
{
	T_SCDrawPolyDraw(pPoints, dwCount, pTypes);
}

template <class T>
inline void CSCEMFdcRenderer::T_SCDrawPolyDraw(T* pPoints, DWORD dwCount, BYTE *pTypes)
{
	ASSERT(m_pGraphics);

	Point* pNewPts = SCPointFromPOINT(pPoints, dwCount);
	BYTE* pNewTypes = SCPathPointTypesFROMCurveTypes(pTypes, dwCount);

	FillMode fillMode = (ALTERNATE==m_dwFillMode) ? FillModeAlternate : FillModeWinding;
	GraphicsPath NewPath(pNewPts, pNewTypes, dwCount, fillMode);

	if (m_pPath)
	{
		m_pPath->AddPath(&NewPath, FALSE);
	} else
	{
		ASSERT(m_pPen);
		if (m_pPen)
			m_pGraphics->DrawPath(m_pPen, &NewPath);
	}
	delete [] pNewPts;
	delete [] pNewTypes;
	// update current position
	SCUpdateCurPos(pPoints[dwCount-1].x, pPoints[dwCount-1].y);
}

//////////////////////////////////////////////////////////////////////////////////////////
/// Geometric shapes (rectangles, ellipses, arcs)
///
void CSCEMFdcRenderer::SCDrawRectangle(LPCRECT pBox)
{
	ASSERT(m_pGraphics);
	ASSERT(pBox);

	// GDI+ can't draw twisted rects. We must normalize
	Rect rect;
	SCNormalizedRectFromRECT(&rect, pBox);

	if (m_pPath)
		m_pPath->AddRectangle(rect);
	else
	{
		// Fill shape by using the current brush
		if (m_pBrush)
			m_pGraphics->FillRectangle(m_pBrush, rect);
		if (m_pPen)
			m_pGraphics->DrawRectangle(m_pPen, rect);
	}
}

// Good place to say: GDI+ does not work!
void CSCEMFdcRenderer::SCDrawRoundRectangle(LPCRECT pBox, SIZE szlCorner)
{
	ASSERT(m_pGraphics);
	ASSERT(pBox);

	// GDI+ can't draw twisted rects. We must normalize
	RECT rc;
	SCGetNormalizedRect(&rc, pBox);

	// Make up points, numbered from LT and going clockwise (LT,RT,RB,LB,LT)
	Point Pts[8];
	INT iRx = szlCorner.cx/2;
	INT iRy = szlCorner.cy/2;

		// up-down and from left to right
	Pts[0].Y = Pts[1].Y = rc.top;
	Pts[7].Y = Pts[2].Y = rc.top + iRy;
	Pts[6].Y = Pts[3].Y = rc.bottom - iRy;
	Pts[5].Y = Pts[4].Y = rc.bottom;
		
		// from left to right and up-down
	Pts[7].X = Pts[6].X = rc.left;
	Pts[0].X = Pts[5].X = rc.left + iRx;
	Pts[1].X = Pts[4].X = rc.right - iRx;
	Pts[2].X = Pts[3].X = rc.right;

	INT iXRight = rc.right - szlCorner.cx;
	INT iYBottom = rc.bottom - szlCorner.cy;

	GraphicsPath NewPath;
	NewPath.AddLine(Pts[0], Pts[1]);
	NewPath.AddArc(iXRight, rc.top, szlCorner.cx, szlCorner.cy, 270, 90);

	NewPath.AddLine(Pts[2], Pts[3]);
	NewPath.AddArc(iXRight, iYBottom, szlCorner.cx, szlCorner.cy, 0, 90);

	NewPath.AddLine(Pts[4], Pts[5]);
	NewPath.AddArc(rc.left, iYBottom, szlCorner.cx, szlCorner.cy, 90, 90);

	NewPath.AddLine(Pts[6], Pts[7]);
	NewPath.AddArc(rc.left, rc.top, szlCorner.cx, szlCorner.cy, 180, 90);

	if (m_pPath)
		m_pPath->AddPath(&NewPath, FALSE);
	else
	{
		// Fill shape by using the current brush
		if (m_pBrush)
		{
			FillMode fillMode = (ALTERNATE==m_dwFillMode) ? FillModeAlternate : FillModeWinding;
			NewPath.SetFillMode(fillMode);
			m_pGraphics->FillPath(m_pBrush, &NewPath);
		}
		// Note: there is a pen size issue here (corners look thinner than straight lines)
		// And background color is seen outside the corners limit.
		if (m_pPen)
			m_pGraphics->DrawPath(m_pPen, &NewPath);
	}
}

void CSCEMFdcRenderer::SCDrawEllipse(LPCRECT pBox)
{
	ASSERT(m_pGraphics);
	ASSERT(pBox);
	Rect rect;
	SCNormalizedRectFromRECT(&rect, pBox);
	if (m_pPath)
		m_pPath->AddEllipse(rect);
	else
	{
		// Fill shape by using the current brush
		if (m_pBrush)
			m_pGraphics->FillEllipse(m_pBrush, rect);
		if (m_pPen)
			m_pGraphics->DrawEllipse(m_pPen, rect);
	}
}

void CSCEMFdcRenderer::SCDrawArc(LPCRECTL pBox, LPCPOINTL pPtStart, LPCPOINTL pPtEnd)
{
	ASSERT(m_pGraphics);
	ASSERT(pBox);

	double dStartAngle;
	double dSweepAngle;
	SCBuildArcAngles(pBox, pPtStart, pPtEnd, m_dwArcDirection, dStartAngle, dSweepAngle);

	Rect rect;
	SCNormalizedRectFromRECT(&rect, (LPCRECT)pBox);
	if (m_pPath)
		m_pPath->AddArc(rect, (REAL)dStartAngle, (REAL)dSweepAngle);
	else
	{
		ASSERT(m_pPen);
		m_pGraphics->DrawArc(m_pPen, rect, (REAL)dStartAngle, (REAL)dSweepAngle);
	}
}

void CSCEMFdcRenderer::SCDrawArcTo(LPCRECTL pBox, LPCPOINTL pPtStart, LPCPOINTL pPtEnd)
{
	ASSERT(m_pGraphics);
	ASSERT(pBox);

	double dStartAngle;
	double dSweepAngle;
	SCBuildArcAngles(pBox, pPtStart, pPtEnd, m_dwArcDirection, dStartAngle, dSweepAngle);

	Rect rect;
	SCNormalizedRectFromRECT(&rect, (LPCRECT)pBox);
	if (m_pPath)
		m_pPath->AddEllipse(rect);
	else
	{
		ASSERT(m_pPen);
		m_pGraphics->DrawArc(m_pPen, rect, (REAL)dStartAngle, (REAL)dSweepAngle);
	}
}


void CSCEMFdcRenderer::SCDrawChord(LPCRECTL pBox, LPCPOINTL pPtStart, LPCPOINTL pPtEnd)
{
	ASSERT(m_pGraphics);
	ASSERT(pBox);

	double dStartAngle;
	double dSweepAngle;
	SCBuildArcAngles(pBox, pPtStart, pPtEnd, m_dwArcDirection, dStartAngle, dSweepAngle);

	Rect rect;
	SCNormalizedRectFromRECT(&rect, (LPCRECT)pBox);
#if 0
	// Functions are missing!
	// Good place to say: GDI+ does not work!
	if (m_pPath)
		m_pPath->AddChord(rect, dStartAngle, dSweepAngle);
	else
	{
		// TODO: Fill shape by using the current brush
		if (m_pBrush)
			m_pGraphics->FillChord(m_pBrush, rect, dStartAngle, dSweepAngle);

		if (m_pPen)
			m_pGraphics->DrawChord(m_pPen, rect, dStartAngle, dSweepAngle);
	}
#else
	GraphicsPath NewPath;
	NewPath.AddArc(rect, (REAL)dStartAngle, (REAL)dSweepAngle);

	Point PtStart;
	Point PtEnd;

	SCIntersectLineAndEllipse(PtStart, pBox, dStartAngle);
	SCIntersectLineAndEllipse(PtEnd, pBox, dStartAngle+dSweepAngle);
	NewPath.AddLine(PtStart, PtEnd);

	if (m_pPath)
		m_pPath->AddPath(&NewPath, FALSE);
	else
	{
		// Fill shape by using the current brush
		if (m_pBrush)
		{
			FillMode fillMode = (ALTERNATE==m_dwFillMode) ? FillModeAlternate : FillModeWinding;
			NewPath.SetFillMode(fillMode);
			m_pGraphics->FillPath(m_pBrush, &NewPath);
		}
		// Note: there is a pen size issue here (corners look thinner than straight lines)
		// And background color is seen outside the corners limit.
		if (m_pPen)
			m_pGraphics->DrawPath(m_pPen, &NewPath);
	}
#endif
}

void CSCEMFdcRenderer::SCDrawPie(LPCRECTL pBox, LPCPOINTL pPtStart, LPCPOINTL pPtEnd)
{
	ASSERT(m_pGraphics);
	ASSERT(pBox);

	double dStartAngle;
	double dSweepAngle;
	SCBuildArcAngles(pBox, pPtStart, pPtEnd, m_dwArcDirection, dStartAngle, dSweepAngle);

	Rect rect;
	SCNormalizedRectFromRECT(&rect, (LPCRECT)pBox);
	if (m_pPath)
		m_pPath->AddPie(rect, (REAL)dStartAngle, (REAL)dSweepAngle);
	else
	{
		// Fill shape by using the current brush
		if (m_pBrush)
			m_pGraphics->FillPie(m_pBrush, rect, (REAL)dStartAngle, (REAL)dSweepAngle);

		if (m_pPen)
			m_pGraphics->DrawPie(m_pPen, rect, (REAL)dStartAngle, (REAL)dSweepAngle);
	}
}

///
///	The AngleArc function draws a line segment and an arc.
///	The line segment is drawn from the current position to the beginning of the arc.
///	The arc is drawn along the perimeter of a circle with the given radius and center.
///	The length of the arc is defined by the given start and sweep angles
///	Angles are given in degrees and relative to the x-axis.
/// NOTE: The figure is not filled.
///
void CSCEMFdcRenderer::SCDrawAngleArc(POINT* pPtCenter, DWORD dwRadius, FLOAT fStartAngle, FLOAT fSweepAngle)
{
	ASSERT(m_pGraphics);
	ASSERT(pPtCenter);

	// Compute points of the line segment
	Point Pt1(m_PtCurPos.X, m_PtCurPos.Y);

	Point Pt2;
	SCIntersectLineAndCircle(Pt2, (POINTL*)pPtCenter, dwRadius, fStartAngle);

	// Compute the rectangle that bounds the ellipse that contains the arc
	Rect rcCircle(pPtCenter->x - dwRadius, pPtCenter->y - dwRadius, 2*dwRadius, 2*dwRadius);
	if (m_pPath)
	{
		// a line segment from the current position to the beginning of the arc
		m_pPath->AddLine(Pt1, Pt2);

		// + an arc along the perimeter of a circle with the given radius and center
		m_pPath->AddArc(rcCircle, fStartAngle, fSweepAngle);
	} else
	{
		ASSERT(m_pPen);
		m_pGraphics->DrawLine(m_pPen, Pt1, Pt2);
		m_pGraphics->DrawArc(m_pPen, rcCircle, fStartAngle, fSweepAngle);
	}

	// Update the current position
	SCIntersectLineAndCircle(m_PtCurPos, (POINTL*)pPtCenter, dwRadius, fStartAngle + fSweepAngle);
}

//////////////////////////////////////////////////////////////////////////////////////////
/// Shapes filling
///

///
///	Gradient fill of rectangles or triangles.
///
void CSCEMFdcRenderer::SCGradientFill(TRIVERTEX* pVer, DWORD dwCountVer, LPCBYTE pMesh, DWORD dwCountMesh, DWORD dwMode)
{
	ASSERT(m_pGraphics);
	if (m_pPath)
	{// Normally, GDI doesn't support gradient fill in a path
		ASSERT(0);
	} else
	{
		// The color information of each channel is specified as a value
		// from 0x0000 to 0xff00. We must translate to the (0x00, 0xFF) range
		#define SC_XTOWIN_CHANNEL(XChannel) MulDiv(XChannel, 0x00FF, 0xFF00)
		// And for GDI+ alpha==0 is transparent
		#define SC_XTOWIN_ALPHA(XChannel) (255 - MulDiv(XChannel, 0x00FF, 0xFF00))

		switch (dwMode)
		{
		case GRADIENT_FILL_RECT_H:
		case GRADIENT_FILL_RECT_V:
			{
				LinearGradientMode iMode = (GRADIENT_FILL_RECT_V==dwMode) ? LinearGradientModeVertical : LinearGradientModeHorizontal;
				GRADIENT_RECT *pRects = (GRADIENT_RECT*)pMesh;
				for (DWORD i=0; (i<dwCountMesh); i++)
				{
					TRIVERTEX* pv = &pVer[pRects[i].UpperLeft];
					Point Pt1(pv->x, pv->y);
					Color color1(
						SC_XTOWIN_ALPHA(pv->Alpha),
						SC_XTOWIN_CHANNEL(pv->Red),
						SC_XTOWIN_CHANNEL(pv->Green),
						SC_XTOWIN_CHANNEL(pv->Blue));
					
					pv = &pVer[pRects[i].LowerRight];
					Color color2(
						SC_XTOWIN_ALPHA(pv->Alpha),
						SC_XTOWIN_CHANNEL(pv->Red),
						SC_XTOWIN_CHANNEL(pv->Green),
						SC_XTOWIN_CHANNEL(pv->Blue));
					
					// rect including the contour
					Rect rcMesh(Pt1.X - 1, Pt1.Y - 1, pv->x - Pt1.X + 2, pv->y - Pt1.Y + 2);
					LinearGradientBrush brush(rcMesh, color1, color2, iMode);
					m_pGraphics->FillRectangle(&brush, rcMesh);
				}
			}
			break;

		case GRADIENT_FILL_TRIANGLE:
			{
				#define SC_USE_RECTANGLE

				GRADIENT_TRIANGLE *pTriangles = (GRADIENT_TRIANGLE *)pMesh;
				for (DWORD i=0; (i<dwCountMesh); i++)
				{
					// compute the colors
					Point points[3];
					Color colors[3];
					INT iCenterAlpha=0, iCenterR=0, iCenterG=0, iCenterB=0;
#ifndef SC_USE_RECTANGLE
					INT xMin=LONG_MAX, xMax=LONG_MIN, yMin=LONG_MAX, yMax=LONG_MIN;
#endif

					TRIVERTEX* pv;
					ULONG* pTri = (ULONG*)&pTriangles[i].Vertex1; // beware of structure packing!
					for (INT j=0; (j<3); j++)
					{
						pv = &pVer[pTri[j]];
						points[j].X = pv->x;
						points[j].Y = pv->y;
						colors[j] = Color::MakeARGB(
							SC_XTOWIN_ALPHA(pv->Alpha),
							SC_XTOWIN_CHANNEL(pv->Red),
							SC_XTOWIN_CHANNEL(pv->Green),
							SC_XTOWIN_CHANNEL(pv->Blue));
						iCenterAlpha += pv->Alpha;
						iCenterR += pv->Red;
						iCenterG += pv->Green;
						iCenterB += pv->Blue;

#ifndef SC_USE_RECTANGLE
						if (pv->x<xMin)
							xMin=pv->x;
						else
						if (pv->x>xMax)
							xMax=pv->x;

						if (pv->y<yMin)
							yMin=pv->y;
						else
						if (pv->y>yMax)
							yMax=pv->y;
#endif
					}

					Color colorCenter = Color::MakeARGB(
						SC_XTOWIN_ALPHA(iCenterAlpha/3),
						SC_XTOWIN_CHANNEL(iCenterR/3),
						SC_XTOWIN_CHANNEL(iCenterG/3),
						SC_XTOWIN_CHANNEL(iCenterB/3));
					
					// construct the brush
					PathGradientBrush pthGrBrush(points, 3);

					int count = 3;
					pthGrBrush.SetSurroundColors(colors, &count);
					pthGrBrush.SetCenterColor(colorCenter);

					// paint
					// include the contour
#ifdef SC_USE_RECTANGLE
					Rect rcMesh;
					pthGrBrush.GetRectangle(&rcMesh);
					rcMesh.Inflate(1, 1);
					m_pGraphics->FillRectangle(&pthGrBrush, rcMesh);
#else
					for (INT k=0; (k<3); k++)
					{
						if (points[k].X==xMin)
							points[k].X--;
						else
						if (points[k].X==xMax)
							points[k].X++;

						if (points[k].Y==yMin)
							points[k].Y--;
						else
						if (points[k].Y==yMax)
							points[k].Y++;
					}
					m_pGraphics->FillPolygon(&pthGrBrush, points, 3);
#endif
				}
			}
			break;
		}
	}
}

///
///	Fill an area with the current brush.
/// (rare occurrences in metafiles)
///
void CSCEMFdcRenderer::SCFloodFill(POINTL& rPtStart, COLORREF crColor, DWORD iMode)
{
	ASSERT(m_pGraphics);
	ASSERT(m_pBrush);

	// Panic! GDI+ can't do it.
	// Note, my son, you don't have to provide device independency
	// by just cutting off functions that exist in GDI: this is not polite.

	// Let's ask GDI (we won't waste our time in using lockbits; rare occurrences)
	HDC hOutputDC = m_pGraphics->GetHDC(); // we never use m_hDC for writing
	ASSERT(hOutputDC);
	if (!hOutputDC)
		return;

	HBRUSH hBrush = NULL;
	Color BrushColor;
	switch(m_pBrush->GetType())
	{
	case BrushTypeSolidColor:
		((SolidBrush*)m_pBrush)->GetColor(&BrushColor);
		hBrush = CreateSolidBrush(BrushColor.ToCOLORREF());
		break;
		
	case BrushTypeHatchFill:
		((HatchBrush*)m_pBrush)->GetBackgroundColor(&BrushColor);
		hBrush = CreateSolidBrush(BrushColor.ToCOLORREF());
		break;
		
	default:
		ASSERT(0);
	}

	if (hBrush)
	{
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hOutputDC, hBrush);

		ExtFloodFill(hOutputDC, rPtStart.x, rPtStart.y, crColor, iMode);

		SelectObject(hOutputDC, hOldBrush);
		DeleteObject(hBrush);
	}
	m_pGraphics->ReleaseHDC(hOutputDC);
}


