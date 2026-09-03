/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCRectPoly.h"
#include "SCGenInclude.h"
#include SC_INC_GENLIB(SCGenMath.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void SCRectPolygon::SCRotate(int iAngle, int xc, int yc)
{
	// this is the angle of rotation for y growing down
	double fAngle = -(iAngle/1800.0f)*SC_PI;
	double cosAngle = cos(fAngle);
	double sinAngle = sin(fAngle);

	POINT *pPt = this->m_Points;
	int itmpx;
	for (int i=0; i<4; i++, pPt++)
	{
		itmpx = pPt->x;
		pPt->x = static_cast<int>(0.5f + xc + (itmpx - xc)*cosAngle - (pPt->y - yc)*sinAngle);
		pPt->y = static_cast<int>(0.5f + yc + (itmpx - xc)*sinAngle + (pPt->y - yc)*cosAngle);
	}
}

#ifdef _DEBUG
void SCRectPolygon::SCTestDraw(HDC hdc)
{
	// Save position
	POINT PtPos;
	MoveToEx(hdc, m_Points[cnr_LT].x, m_Points[cnr_LT].y, &PtPos);
	
	// Draw top and right borders
	HPEN hRedPen = CreatePen(PS_SOLID, 0, m_ColorTopAndRight);
	HPEN hOldPen = (HPEN)SelectObject(hdc, hRedPen);
	LineTo(hdc, m_Points[cnr_RT].x, m_Points[cnr_RT].y);
	LineTo(hdc, m_Points[cnr_RB].x, m_Points[cnr_RB].y);
	
	// Draw bottom and left borders
	HPEN hBluePen = CreatePen(PS_SOLID, 0, m_ColorBottomAndLeft);
	SelectObject(hdc, hBluePen);
	LineTo(hdc, m_Points[cnr_LB].x, m_Points[cnr_LB].y);
	LineTo(hdc, m_Points[cnr_LT].x, m_Points[cnr_LT].y);
	
	// Restore position
	MoveToEx(hdc, PtPos.x, PtPos.y, NULL);
	SelectObject(hdc, hOldPen);
	DeleteObject(hRedPen);
	DeleteObject(hBluePen);
}
#endif


// The thechique used here is as follows:
// We want to reflect a point P(xP, yP) about any line given by two points
// A(xA, yA) and B(xB, yB).
// Let's call P'(xP', yP') the image of P,
//			  O(x, y) the intersection of PP' with AB
//			  W[u, v]=[xB - xA, yB - yA] a directing vector of AB
// we the following equations are true:
//  1) xP' + xP = 2x					=> since P' is a reflection of P
//  2) yP' + yP = 2y					=> ditto
//  3) (y - yA)u - (x - xA)v = 0		=> equation of a line passing by A and B
//  4) (x - xP)u + (y - yP)v = 0		=> OP and AB are othogonal

// Solving these equations yeilds:
//
// |xP'|    |  2u^2			   2u.v			  -2f.v		| |xP'| 
// |   |    | (----- - 1)	(-------)		(-------)	| |   |
// |   |    |	d			    d				d		| |   |
// |   |    |											| |   |
// |yP'|  = |  2u.v			  2v^2			   2f.u		| |yP |
// |   |    | (-------)		(----- - 1)		(-------)	| |   |
// |   |    |	d				d				d		| |   |
// |   |    |											| |   |
// |1  |    |   0			    0				1		| |1  |
//
//  Where: d = u^2 + v^2, and f = u.yA - v.xA;
//		   '^' stands for 'power of', and the dot '.' for multiplication.

// This function always reflects about the left border
// (going from LB to LT points, which become A and B respectively)
void SCRectPolygon::SCComputeReflectionLeft(XFORM& xform)
{
	SCComputeReflection(xform, &m_Points[cnr_LB], &m_Points[cnr_LT]);
}

// This function always reflects about the top border
// (going from LT to RT points, which become A and B respectively)
void SCRectPolygon::SCComputeReflectionTop(XFORM& xform)
{
	SCComputeReflection(xform, &m_Points[cnr_LT], &m_Points[cnr_RT]);
}

// This function always reflects about the right border
// (going from RB to RT points, which become A and B respectively)
void SCRectPolygon::SCComputeReflectionRight(XFORM& xform)
{
	SCComputeReflection(xform, &m_Points[cnr_RB], &m_Points[cnr_RT]);
}

// This function always reflects about the bottom border
// (going from LB to RB points, which become A and B respectively)
void SCRectPolygon::SCComputeReflectionBottom(XFORM& xform)
{
	SCComputeReflection(xform, &m_Points[cnr_LB], &m_Points[cnr_RB]);
}

// This function always reflects about a line parallel with the top border
// at a distance equal to iDeltaY
// Caution: use only on a non-rotated SCRectPolygon
void SCRectPolygon::SCComputeReflectionBase(XFORM& xform, int iDeltaY)
{
	POINT PtA;
	PtA.x = m_Points[cnr_LT].x;
	PtA.y = m_Points[cnr_LT].y + iDeltaY;

	POINT PtB;
	PtA.x = m_Points[cnr_RT].x;
	PtA.y = m_Points[cnr_RT].y + iDeltaY;
	SCComputeReflection(xform, &PtA, &PtB);
}

// General reflection
void SCRectPolygon::SCComputeReflection(XFORM& xform, POINT* pPtA, POINT* pPtB)
{
	ASSERT(pPtA);
	ASSERT(pPtB);
	
	int u = pPtB->x - pPtA->x;
	int v = pPtB->y - pPtA->y;
	int up2 = u*u;
	int vp2 = v*v;
	float d = (float)(up2 + vp2);
	int f = u*pPtA->y - v*pPtA->x;
	
	xform.eM11 = (FLOAT)(2.0*up2/d - 1);
	xform.eM22 = (FLOAT)(2.0*vp2/d - 1);
	xform.eM12 =
	xform.eM21 = (FLOAT)(2.0*u*v/d);
	xform.eDx = (FLOAT)(-2.0*f*v/d);
	xform.eDy = (FLOAT)(2.0*f*u/d);
}
