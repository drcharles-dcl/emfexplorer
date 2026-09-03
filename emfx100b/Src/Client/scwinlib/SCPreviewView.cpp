/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCPreviewView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CSCPreviewView, CPreviewView)

BEGIN_MESSAGE_MAP(CSCPreviewView, CPreviewView)
	//{{AFX_MSG_MAP(CSCPreviewView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSCPreviewView::CSCPreviewView()
{

}

CSCPreviewView::~CSCPreviewView()
{

}

BOOL CSCPreviewView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (cs.lpszClass == NULL)
		cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW|CS_BYTEALIGNCLIENT);

	return CView::PreCreateWindow(cs);
}

void CSCPreviewView::OnDraw(CDC* pDC) 
{
	//recalculate scale ratio
	PositionPage(0);

	//store scale ratio
	float fRatio = (float)m_pPageInfo[0].sizeScaleRatio.cx / (float)m_pPageInfo[0].sizeScaleRatio.cy;
	m_pPreviewInfo->m_lpUserData = &fRatio;

	CPreviewView::OnDraw(pDC);
}

