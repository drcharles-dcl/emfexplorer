/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCRECTPOLY_H_
#define _SCRECTPOLY_H_

class SCRectPolygon
{
public:
	enum enumRectCorners
	{
		cnr_LT=0, // left-top corner
		cnr_RT, // right-top corner
		cnr_RB, // right-bottom corner
		cnr_LB // left-bottom corner
	};

public:
	POINT m_Points[4]; // left-top corner
	SCRectPolygon()
	{
		memset(m_Points, 0, sizeof(m_Points));
#ifdef _DEBUG
		m_ColorTopAndRight = RGB(255, 0, 0);
		m_ColorBottomAndLeft = RGB(0, 0, 255);
#endif
	}
	SCRectPolygon(RECT& rc)
	{
		SCFromRect(&rc);
#ifdef _DEBUG
		m_ColorTopAndRight = RGB(255, 0, 0);
		m_ColorBottomAndLeft = RGB(0, 0, 255);
#endif
	}

	BOOL SCIsRect() const
	{
		return ((m_Points[cnr_LT].x == m_Points[cnr_LB].x)
				&& (m_Points[cnr_RT].x == m_Points[cnr_RB].x)
				&& (m_Points[cnr_LT].y == m_Points[cnr_RT].y)
				&& (m_Points[cnr_LB].y == m_Points[cnr_RB].y));
	}
	void SCFromRect(CONST RECT* pRect)
	{
		ASSERT(pRect);
		m_Points[cnr_LT].x = m_Points[cnr_LB].x = pRect->left;
		m_Points[cnr_LT].y = m_Points[cnr_RT].y = pRect->top;

		m_Points[cnr_RT].x = m_Points[cnr_RB].x = pRect->right;
		m_Points[cnr_LB].y = m_Points[cnr_RB].y = pRect->bottom;
	}
	void SCToRect(RECT& rc) const
	{
		rc.left = m_Points[cnr_LT].x;
		rc.top = m_Points[cnr_LT].y;

		rc.right = m_Points[cnr_RB].x;
		rc.bottom = m_Points[cnr_RB].y;
	}
	void SCRotate(int iAngle, int xc, int yc);

	void SCComputeReflectionLeft(XFORM& xform);
	void SCComputeReflectionTop(XFORM& xform);
	void SCComputeReflectionRight(XFORM& xform);
	void SCComputeReflectionBottom(XFORM& xform);
	void SCComputeReflectionBase(XFORM& xform, int iDeltaY);

	void SCComputeReflection(XFORM& xform, POINT* pPtA, POINT* pPtB);

#ifdef _DEBUG
public:
	void SCTestDraw(HDC hdc);
	COLORREF	m_ColorTopAndRight;
	COLORREF	m_ColorBottomAndLeft;
#endif
};

typedef SCRectPolygon *PSCRectPolygon;



#endif //_SCRECTPOLY_H_
//  ------------------------------------------------------------
