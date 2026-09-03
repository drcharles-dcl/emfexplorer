/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCImgStatic.h"

//	#ifdef _DEBUG
//	#define new DEBUG_NEW
//	#undef THIS_FILE
//	static char THIS_FILE[] = __FILE__;
//	#endif

/////////////////////////////////////////////////////////////////////////////
// CSCImgStatic

CSCImgStatic::CSCImgStatic()
	:m_pImage(NULL),
	m_fWidth(0),
	m_fHeight(0)
{
}

CSCImgStatic::~CSCImgStatic()
{
	if (m_pImage)
		delete m_pImage;
}


BEGIN_MESSAGE_MAP(CSCImgStatic, CStatic)
	//{{AFX_MSG_MAP(CSCImgStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCImgStatic message handlers

void CSCImgStatic::OnPaint() 
{
	using namespace Gdiplus;

	CPaintDC dc(this); // device context for painting

	// getting rect of static
	CRect rect;
	GetClientRect(&rect);
	dc.PatBlt(rect.left, rect.top, rect.right, rect.bottom, WHITENESS);

	if (!m_pImage)
		return;
		
	REAL xPos = rect.left + (rect.Width() - m_fWidth)/2;
	REAL yPos = rect.top + (rect.Height() - m_fHeight)/2;

	// Image
	Graphics graphics(dc.GetSafeHdc());
	graphics.DrawImage(m_pImage, xPos, yPos, m_fWidth, m_fHeight);

	// Border
	Color PenColor(255, 0, 0, 0);
	Pen pen(PenColor, 1.0);
	Rect rcBorder(rect.left, rect.top, rect.Width()-1, rect.Height()-1);
	graphics.DrawRectangle(&pen, rcBorder);
}

BOOL CSCImgStatic::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CSCImgStatic::SCSetEMF(HENHMETAFILE hemf)
{
	using namespace Gdiplus;

	if (m_pImage)
		delete m_pImage;
	if (!hemf)
	{
		m_pImage = NULL;
		Invalidate();
		return;
	}
	m_pImage = new Metafile(hemf);

	REAL fScaleXY = (REAL)m_pImage->GetWidth()/(REAL)m_pImage->GetHeight();

	CRect rect;
	GetClientRect(&rect);
	m_fHeight = (REAL)rect.Height();
	m_fWidth = m_fHeight*fScaleXY;
	if (m_fWidth>rect.Width())
	{
		m_fWidth = (REAL)rect.Width();
		m_fHeight = m_fWidth/fScaleXY;
	}
	Invalidate();
}
