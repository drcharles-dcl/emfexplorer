/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCWinThumbs.h"

#include "SCGDIUtils.h"
#include <math.h>

//	#ifdef _DEBUG
//	#define new DEBUG_NEW
//	#undef THIS_FILE
//	static char THIS_FILE[] = __FILE__;
//	#endif

#define SC_PIX_TXTLINE		32

#define MIN_MARGIN			16
#define SHADOW_WDT			2
#define COL_SPACE			(MIN_MARGIN - (1+1+SHADOW_WDT))
#define ROW_SPACE			COL_SPACE

#define SC_CTL_SYSBKGN		LTGRAY_BRUSH			
#define SC_THUMB_SYSBKGN	COLOR_WINDOW

#define SC_THUMBNUM_COLOR	RGB(255, 255, 255)			
#define SC_THUMBNUM_WIDTH	MIN_MARGIN
#define SC_THUMBNUM_HEIGHT	MIN_MARGIN

#define SC_CURSEL_COLOR		RGB(0, 0, 255)
#define SC_CURSEL_VSTROKES	3
#define SC_CURSEL_HSTROKES	3

//	#define SC_UNSCALED_THUMBS
//	#define SC_FULL_DCPROTRECTION


/////////////////////////////////////////////////////////////////////////////
// CSCWinThumbs

CSCWinThumbs::CSCWinThumbs():
	m_pIThumbsHolder(NULL),
	m_ThumbsMinSize(32,32),
	m_ThumbsSize(32,32),
	m_iCurSel(-1),
	m_bAdjusting(FALSE),
	m_uiNbThumbs(0),
	m_bShowNum(TRUE),
	m_uiNbRows(1), 
	m_uiNbCols(1),
	m_iMarginX(0),
	m_iMarginY(0),
	m_iWorldCx(0),
	m_iWorldCy(0),
	m_pFont(NULL),
	m_bProgessDraw(TRUE),
	m_bLastSeen(FALSE)
{
}

CSCWinThumbs::~CSCWinThumbs()
{
	if (m_pFont)
		delete m_pFont;
}

void CSCWinThumbs::SCReset()
{
	m_iCurSel = -1;
	m_uiNbThumbs = 0;
	m_uiNbRows = 1; 
	m_uiNbCols = 1;
	m_iMarginX = 0;
	m_iMarginY = 0;
	m_iWorldCx = 0;
	m_iWorldCy = 0;
}

BEGIN_MESSAGE_MAP(CSCWinThumbs, CWnd)
	//{{AFX_MSG_MAP(CSCWinThumbs)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCWinThumbs utilities

void CSCWinThumbs::SCInitCtl()
{
	CRect rc;
	GetClientRect(&rc);
	m_WorkMemDC.SCPrepareSurface(rc.Width(), rc.Height(), NULL);
	SCSizeScrollBars(rc.Width(), rc.Height());
	SCComputeRowsCols();
}

void CSCWinThumbs::SCSetThumbsHolder(I_SCThumbsHolder* pHolder, int iNbThumbs)
{
	SCReset();
	m_pIThumbsHolder = pHolder;
	m_uiNbThumbs = iNbThumbs;
	SCComputeRowsCols();
}

void CSCWinThumbs::SCPaint(CPaintDC &dc)
{
	CRect rcClient;
	GetClientRect(rcClient);
	int cx = rcClient.Width();
	int cy = rcClient.Height();
	ASSERT(m_WorkMemDC.m_hDC); // you must init it
	if (!m_WorkMemDC.m_hDC)
	{
		dc.PatBlt(0, 0, cx, cy, BLACKNESS);
		return;
	}

	{// paint non covered region
		CBrush brush;
		brush.CreateStockObject(SC_CTL_SYSBKGN);
		m_WorkMemDC.FillRect(&rcClient, &brush);
	}

	if (!m_pIThumbsHolder || (0==m_uiNbThumbs))
	{
		dc.StretchBlt(0, 0,	cx, cy,	&m_WorkMemDC, 0, 0, cx, cy, SRCCOPY);
		return;
	}
	
	COLORREF crBkColor = GetSysColor(SC_THUMB_SYSBKGN);
	CBrush BkBrush;
	BkBrush.CreateSolidBrush(crBkColor);

	int xScroll  = GetScrollPos(SB_HORZ);
	int yScroll  = GetScrollPos(SB_VERT);
	m_WorkMemDC.SetWindowOrg(xScroll, yScroll);
	int iWdt = m_ThumbsSize.cx; 
	int iHgt = m_ThumbsSize.cy; 
	int iImgCx = iWdt + COL_SPACE;
	int iImgCy = iHgt + ROW_SPACE;
	long uiThumbIdx;
	int iRowStart = yScroll/iImgCy;
	int iRowStop = 1+(yScroll+cy)/iImgCy;
	if (iRowStop>m_uiNbRows)
		iRowStop=m_uiNbRows;
	
	int iColStart = xScroll/iImgCx;
	int iColStop = 1+(xScroll+cx)/iImgCx;
	if (iColStop>m_uiNbCols)
		iColStop=m_uiNbCols;
	
	int xDest = m_iMarginX + iColStart*iImgCx;
	int yDest = m_iMarginY + iRowStart*iImgCy;
	for (int iRow = iRowStart; (iRow<iRowStop); iRow++)
	{
		// draw visible rows
		for (int iCol = iColStart; (iCol<iColStop); iCol++)
		{
			uiThumbIdx = iRow*m_uiNbCols + iCol;
			if (uiThumbIdx>=m_uiNbThumbs)
				break; // a row may be incomplete
			
			// draw visible columns
			CRect rcElem(xDest, yDest, xDest + iWdt, yDest + iHgt);

			// thumb background
			m_WorkMemDC.FillRect(&rcElem, &BkBrush);

			// thumb image
			PSCThumbImage pImage = m_pIThumbsHolder->SCGetThumbImage(uiThumbIdx);
			if (pImage)
			{
#ifdef SC_UNSCALED_THUMBS
				GReal fScaleXY = (GReal)pImage->GetWidth()/(GReal)pImage->GetHeight();
				GReal height = iHgt;
				GReal width = height*fScaleXY;
				if (width>iWdt)
				{
					width = (GReal)iWdt;
					height = width/fScaleXY;
				}
#else
				GReal height = (GReal)pImage->GetHeight();
				GReal width =  (GReal)pImage->GetWidth();
#endif
				GReal xPos = xDest + (iWdt - width)/2;
				GReal yPos = yDest + (iHgt - height)/2;

#ifdef SC_FULL_DCPROTRECTION
				// Normally, DrawImage shouldn't be able to write outside rcElem.
				// But with a metafile, maybe it could.
				int iState = m_WorkMemDC.SaveDC();
				m_WorkMemDC.IntersectClipRect(&rcElem);
#endif
				GGraphics graphics(m_WorkMemDC.GetSafeHdc());
				graphics.DrawImage(pImage, xPos, yPos, width, height);

#ifdef SC_FULL_DCPROTRECTION
				m_WorkMemDC.RestoreDC(iState);
#endif
			}
			
			// shadow
			SCDrawFrameAndShadow(&m_WorkMemDC, xDest, yDest, iWdt, iHgt, SHADOW_WDT, crBkColor);
			
			// thumb number
			if (m_bShowNum)
				SCDrawNumber(xDest, yDest, uiThumbIdx);

			// if we are on the current thumb, show selection
			if (uiThumbIdx==m_iCurSel)
			{
				CBrush Brush;
				Brush.CreateSolidBrush(SC_CURSEL_COLOR);
				
				CRgn Rgn;
				Rgn.CreateRectRgnIndirect(&rcElem);
				m_WorkMemDC.FrameRgn(&Rgn, &Brush, SC_CURSEL_VSTROKES, SC_CURSEL_HSTROKES);
				
				Rgn.DeleteObject();
				Brush.DeleteObject();
			}
			if (m_bProgessDraw && !m_bLastSeen)
			{// show progressively
				dc.StretchBlt(0, 0, cx, cy, &m_WorkMemDC, xScroll, yScroll, cx, cy, SRCCOPY);
			}
			xDest += iImgCx;
		}
		yDest += iImgCy;
		xDest = m_iMarginX;
	}
	m_WorkMemDC.SetWindowOrg(0, 0);

	dc.StretchBlt(0, 0,	cx, cy,	&m_WorkMemDC, 0, 0, cx, cy, SRCCOPY);
	if (uiThumbIdx==m_uiNbThumbs)
		m_bLastSeen = TRUE; // TODO: fix this or don't use it
	// Do not call CWnd::OnPaint()
}

void CSCWinThumbs::SCDrawNumber(int xDest, int yDest, UINT uiThumbIdx)
{
	CFont* pOldFont = (m_pFont) ? m_WorkMemDC.SelectObject(m_pFont) : NULL;
	COLORREF crOldColor = m_WorkMemDC.SetTextColor(SC_THUMBNUM_COLOR);
	int iOldBkMode = m_WorkMemDC.SetBkMode(TRANSPARENT);
	
	CRect rcNum(xDest, yDest, xDest + SC_THUMBNUM_WIDTH, yDest + SC_THUMBNUM_HEIGHT);
	CBrush brush;
	brush.CreateStockObject(BLACK_BRUSH);
	m_WorkMemDC.FillRect(&rcNum, &brush);
	
	CString strNum;
	strNum.Format(_T("%d"), uiThumbIdx);
	CSize size = m_WorkMemDC.GetTextExtent(strNum);
	m_WorkMemDC.TextOut(rcNum.left + (rcNum.Width()- size.cx)/2,
						rcNum.top + (rcNum.Height()- size.cy)/2,
						strNum, strNum.GetLength());
	
	m_WorkMemDC.SetBkMode(iOldBkMode);
	if (pOldFont)
		m_WorkMemDC.SelectObject(pOldFont);
	m_WorkMemDC.SetTextColor(crOldColor);
}

void CSCWinThumbs::SCInitializeScrollBars(int width, int height)
{
    RECT        rClient;
    SCROLLINFO  ScrollInfo;

    // Get windows client size.
    GetClientRect(&rClient);

    // If client size is equel to dib size, then add one to client size
    // so that we don't show scroll bars.
    // However, if client size is less than dib size, subtract scroll bar
    // size form client size so that page size will be correct when
    // scroll bars are shown.
    if (rClient.bottom == height)
        ++rClient.bottom;
    else if (rClient.bottom < height)
        rClient.bottom -= GetSystemMetrics (SM_CYHSCROLL);
    if (rClient.right == width)
        ++rClient.right;
    else if (rClient.right < width)
        rClient.right -= GetSystemMetrics (SM_CXVSCROLL);

    // Initialize vertical scroll bar.
    ScrollInfo.cbSize = sizeof(SCROLLINFO);
    ScrollInfo.fMask = SIF_ALL;
    ScrollInfo.nMin = 0;
    ScrollInfo.nMax = height;
    ScrollInfo.nPage = rClient.bottom;
    SetScrollInfo(SB_VERT, &ScrollInfo, TRUE);

    // Initialize horizontal scroll bar.
    ScrollInfo.nMax = width;
    ScrollInfo.nPage = rClient.right;
    SetScrollInfo(SB_HORZ, &ScrollInfo, TRUE);
}

BOOL CSCWinThumbs::ScrollChildWindow(int nScrollBar, UINT wScrollCode, int iCount /*=1*/)
{
    int         nPosition;
    int         nHorzScroll = 0;
    int         nVertScroll = 0;
    SCROLLINFO  ScrollInfo;

    // Get current scroll information.
	ZeroMemory(&ScrollInfo, sizeof(SCROLLINFO));
    ScrollInfo.cbSize = sizeof(SCROLLINFO);
    ScrollInfo.fMask = SIF_ALL;
    if (!GetScrollInfo(nScrollBar, &ScrollInfo))
	{
		int error = GetLastError();
		return FALSE;
	}
    nPosition = ScrollInfo.nPos;

    // Modify scroll information based on requested
    // scroll action.
    switch (wScrollCode)
    {
        case SB_LINEDOWN:
            ScrollInfo.nPos += SC_PIX_TXTLINE*iCount;
            break;

        case SB_LINEUP:
            ScrollInfo.nPos -= SC_PIX_TXTLINE*iCount;
            break;

        case SB_PAGEDOWN:
            ScrollInfo.nPos += ScrollInfo.nPage*iCount;
            break;

        case SB_PAGEUP:
            ScrollInfo.nPos -= ScrollInfo.nPage*iCount;
            break;

        case SB_TOP:
            ScrollInfo.nPos = ScrollInfo.nMin;
            break;

        case SB_BOTTOM:
            ScrollInfo.nPos = ScrollInfo.nMax;
            break;

            // Don't do anything.
        case SB_THUMBTRACK:
			return TRUE;

        case SB_THUMBPOSITION:
            ScrollInfo.nPos = ScrollInfo.nTrackPos;
            break;

        case SB_ENDSCROLL:
            default:
            return FALSE;
    }

    // Make sure that scroll position is in range.
    if (0 > ScrollInfo.nPos)
        ScrollInfo.nPos = 0;
    else if (ScrollInfo.nMax - (int) ScrollInfo.nPage + 1 < ScrollInfo.nPos)
        ScrollInfo.nPos = ScrollInfo.nMax  - ScrollInfo.nPage + 1;

    // Set new scroll position.
    ScrollInfo.fMask = SIF_POS;
    SetScrollInfo(nScrollBar, &ScrollInfo, TRUE);

    // Scroll window.
#if 0
	// scroll position is integrated to scpaint
    if (SB_VERT == nScrollBar)
        nVertScroll = nPosition - ScrollInfo.nPos;
    else
        nHorzScroll = nPosition - ScrollInfo.nPos;

	ScrollWindowEx(nHorzScroll, nVertScroll, NULL, NULL,
		NULL, NULL, SW_INVALIDATE);
#endif

	Invalidate();
	UpdateWindow();
    return TRUE;
}

void CSCWinThumbs::SCSizeScrollBars(int uiWindowWidth, int uiWindowHeight)
{
    // Only size for validte windows and dib info.
    // Don't SCSizeScrollBars if in process of adjusting them.
    if (IsWindow(m_hWnd) &&
        !m_bAdjusting)
    {
        int         nScrollHeight = GetSystemMetrics (SM_CXVSCROLL);
        int         nScrollWidth = GetSystemMetrics (SM_CYHSCROLL);
        SCROLLINFO  VertScrollInfo;
        SCROLLINFO  HorzScrollInfo;

        // Make sure that we don't get into an infinite loop when updating scroll bars.
        m_bAdjusting = TRUE;

        // Get current vertical scroll info.
        VertScrollInfo.cbSize = sizeof(SCROLLINFO);
        VertScrollInfo.fMask = SIF_ALL;
        GetScrollInfo(SB_VERT, &VertScrollInfo);

        // Get current horizontal scroll info.
        HorzScrollInfo.cbSize = sizeof(SCROLLINFO);
        HorzScrollInfo.fMask = SIF_ALL;
        GetScrollInfo(SB_HORZ, &HorzScrollInfo);

        // Update vertical scroll info.
        VertScrollInfo.fMask = SIF_PAGE;
        VertScrollInfo.nPage = uiWindowHeight;
		SetScrollInfo(SB_VERT, &VertScrollInfo, TRUE);

        // Update horizontal scroll info.
        HorzScrollInfo.fMask = SIF_PAGE;
        HorzScrollInfo.nPage = uiWindowWidth;
		SetScrollInfo(SB_HORZ, &HorzScrollInfo, TRUE);

        m_bAdjusting = FALSE;
    }
}

inline int CSCWinThumbs::SCNbColsInSpace(int iWndCx, int iImgCx)
{
	ASSERT(iImgCx>0);

	int iNbCols = (iWndCx + COL_SPACE - 2*MIN_MARGIN) / iImgCx;
	if (iNbCols<=0)
		return 1;
	
	if (iNbCols>m_uiNbThumbs)
		return m_uiNbThumbs;
	
	return iNbCols;
}

inline int CSCWinThumbs::SCNbRowsInCols(int iNbCols)
{
	ASSERT(iNbCols>0);

	int iNbRows = m_uiNbThumbs / iNbCols;
	if (m_uiNbThumbs % iNbCols)
		iNbRows++;
	
	return iNbRows;
}


void CSCWinThumbs::SCComputeRowsCols()
{
	if (!m_uiNbThumbs)
	{
		SCInitializeScrollBars(m_iWorldCx, m_iWorldCy);
		Invalidate();
		return;
	}

	CSize OldSize = m_ThumbsSize;
	int iImgCx = m_ThumbsMinSize.cx + COL_SPACE;
	int iImgCy = m_ThumbsMinSize.cy + ROW_SPACE;
	ASSERT(iImgCx && iImgCy);

	CRect rc;
	GetClientRect(rc);
	int iWndCx = rc.Width();
	int iWndCy = rc.Height();
	
	float fTotalSurface = float(iWndCx - 2*MIN_MARGIN)*float(iWndCy - 2*MIN_MARGIN);
	float fImgSurface = float(iImgCx)*float(iImgCy)*m_uiNbThumbs;
	if (fTotalSurface > 1.1f*fImgSurface)
	{
		fImgSurface = fTotalSurface/m_uiNbThumbs;
		iImgCy = (int)sqrt(fImgSurface);
		iImgCx = MulDiv(iImgCy, m_ThumbsMinSize.cx, m_ThumbsMinSize.cy);

		for (;iImgCx>m_ThumbsMinSize.cx && iImgCy>m_ThumbsMinSize.cy;)
		{
			int iNbCols = SCNbColsInSpace(iWndCx, iImgCx);
			int iNbRows = SCNbRowsInCols(iNbCols);
			int iColsSpan = iNbCols*iImgCx - COL_SPACE + 2*MIN_MARGIN;
			int iRowsSpan = iNbRows*iImgCy - ROW_SPACE + 2*MIN_MARGIN;
			if ((iNbCols*iImgCx - COL_SPACE + 2*MIN_MARGIN)>iWndCx)
			{
				iImgCx -= 5;
				iImgCy = MulDiv(iImgCx, m_ThumbsMinSize.cy, m_ThumbsMinSize.cx);
			} else
			if ((iNbRows*iImgCy - ROW_SPACE + 2*MIN_MARGIN)>iWndCy)
			{
				iImgCy -= 5;
				iImgCx = MulDiv(iImgCy, m_ThumbsMinSize.cx, m_ThumbsMinSize.cy);
			}
			else
				break;
		}

		m_ThumbsSize.cx = iImgCx - COL_SPACE;
		m_ThumbsSize.cy = iImgCy - ROW_SPACE;
	} else
	{
		m_ThumbsSize = m_ThumbsMinSize;
	}

	m_uiNbCols = SCNbColsInSpace(iWndCx, iImgCx);
	m_uiNbRows = SCNbRowsInCols(m_uiNbCols);

	// try to set an y margin (if one row)
	int iRowsSpan = m_uiNbRows*iImgCy - ROW_SPACE; // space used by rows
	if (iRowsSpan<iWndCy)
		m_iWorldCy = iWndCy;
	else
		m_iWorldCy = MIN_MARGIN*2 + iRowsSpan;
	ASSERT(m_iWorldCy>=iWndCy);

	// compute world dimensions for scrollbars
	int iColsSpan = m_uiNbCols*iImgCx - COL_SPACE; // space used by cols
	if (iColsSpan<iWndCx)
		m_iWorldCx = iWndCx;
	else
		m_iWorldCx = MIN_MARGIN*2 + iColsSpan;
	ASSERT(m_iWorldCx>=iWndCx);

	// center
	m_iMarginX = (m_iWorldCx - iColsSpan)/2;
	m_iMarginY = (m_iWorldCy - iRowsSpan)/2;
	ASSERT(m_iMarginX>=0);
	ASSERT(m_iMarginY>=0);
	SCInitializeScrollBars(m_iWorldCx, m_iWorldCy);

	if (OldSize!=m_ThumbsSize)
		m_pIThumbsHolder->SCSetThumbsSize(m_ThumbsSize);
}

// Compute the thumb containing a client space point
int CSCWinThumbs::SCComputeHitTest(CPoint point) 
{
	if (0==m_uiNbThumbs)
		return -1;

	// Message handler code here and/or call default
	int xScroll  = GetScrollPos(SB_HORZ);
	int yScroll  = GetScrollPos(SB_VERT);
	int iWdt = m_ThumbsSize.cx;
	int iHgt = m_ThumbsSize.cy;
	int iImgCx = iWdt + COL_SPACE;
	int iImgCy = iHgt + ROW_SPACE;

	int iPosY = yScroll + point.y;
	int iPosX = xScroll + point.x;
	int iPgRow = (iPosY - m_iMarginY) / iImgCy;
	int iPgCol = (iPosX - m_iMarginX) / iImgCx;
	int iThumbNum = iPgRow*m_uiNbCols + iPgCol;
	ASSERT(iPgRow>=0 && iPgCol>=0);

	// check thumb/col/row number overflows
	if (iThumbNum>=m_uiNbThumbs || iPgCol>=m_uiNbCols || iPgRow>=m_uiNbRows)
		iThumbNum = -1;
	else
	// check left and top bands
	if ((0==iPgCol && point.x<=m_iMarginX)||
		(0==iPgRow && point.y<=m_iMarginY))
		iThumbNum = -1;
	else
	// check right and bottom bands, and between columns or rows
	if ((iPosX >= (m_iMarginX + iPgCol*iImgCx + iWdt))||
		(iPosY >= (m_iMarginY + iPgRow*iImgCy + iHgt)))
		iThumbNum = -1;

	return iThumbNum;
}


///
/// Tell the parent that selection has changed.
///
void CSCWinThumbs::SCNotifyParent()
{
	CWnd *pParent = GetParent();
	
	ASSERT(pParent);
	if (!pParent)
		return;

	NMHDR nmhdr;
	nmhdr.code = THN_SELCHANGE;
	nmhdr.hwndFrom = m_hWnd;
	nmhdr.idFrom = GetDlgCtrlID();
	pParent->SendMessage(WM_NOTIFY, (WPARAM)nmhdr.idFrom, (LPARAM)&nmhdr);
}

void CSCWinThumbs::SCEnsureCurThumbVisible()
{
	if (0==m_uiNbThumbs)
		return;

	CRect rc;
	GetClientRect(&rc);
	int cy = rc.Height();
	int xScroll  = GetScrollPos(SB_HORZ);
	int yScroll  = GetScrollPos(SB_VERT);

	// Get thumb top in client coordinates
	int xThumbLeft;
	int yThumbTop;
	SCGetThumbPos(m_iCurSel, xThumbLeft, yThumbTop);

	int iBottom = yThumbTop + m_ThumbsSize.cy;
	if (iBottom >= cy)
	{// make bottom visible
		int iDeltaY = iBottom - cy;
		int iQuant = iDeltaY / SC_PIX_TXTLINE;
		if (iQuant<=0)
			iQuant = 1;
		else
		if (iDeltaY % SC_PIX_TXTLINE)
			iQuant++;
		ScrollChildWindow(SB_VERT, SB_LINEDOWN, iQuant);
	} else
	{// make top visible
		if (yThumbTop <= 0)
		{
			int iDeltaY = -yThumbTop;
			int iQuant = iDeltaY / SC_PIX_TXTLINE;
			if (iQuant<=0)
				iQuant = 1;
			else
			if (iDeltaY % SC_PIX_TXTLINE)
				iQuant++;
			ScrollChildWindow(SB_VERT, SB_LINEUP, iQuant);
		} else
		{
			Invalidate();
		}
	}
}

void CSCWinThumbs::SCGetThumbPos(int iThumbIdx, int& xThumbLeft, int& yThumbTop)
{
	ASSERT(iThumbIdx>=0 && iThumbIdx<m_uiNbThumbs);

	CSize ThSizeEx = m_ThumbsSize; // extended size of thumb
	ThSizeEx.cx += COL_SPACE;
	ThSizeEx.cy += ROW_SPACE;

	// remove scroll, and start with world margins
	yThumbTop = m_iMarginY - GetScrollPos(SB_VERT);
	xThumbLeft = m_iMarginX - GetScrollPos(SB_HORZ);

	// skip rows and cols
	yThumbTop  += (iThumbIdx / m_uiNbCols)*ThSizeEx.cy;
	xThumbLeft += (iThumbIdx % m_uiNbCols)*ThSizeEx.cx;
}

/////////////////////////////////////////////////////////////////////////////
// CSCWinThumbs message handlers

BOOL CSCWinThumbs::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_HSCROLL|WS_VSCROLL;
	return CWnd::PreCreateWindow(cs);
}

BOOL CSCWinThumbs::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}

void CSCWinThumbs::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// message handler code here
	ASSERT(cx && cy);
	if (!cx || !cy)
		return;

	m_WorkMemDC.SCPrepareSurface(cx, cy, NULL);
	SCSizeScrollBars(cx, cy);
	SCComputeRowsCols();
}

void CSCWinThumbs::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	SCPaint(dc);
}


void CSCWinThumbs::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	ScrollChildWindow(SB_HORZ, nSBCode);
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSCWinThumbs::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	ScrollChildWindow(SB_VERT, nSBCode);
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

// Reading flow tracking
void CSCWinThumbs::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (0==m_uiNbThumbs)
		return;

	switch(nChar)
	{
	case VK_RIGHT:
	case VK_DOWN:
		ScrollChildWindow(SB_VERT, SB_LINEDOWN); // temp
		break;

	case VK_LEFT:
	case VK_UP:
		ScrollChildWindow(SB_VERT, SB_LINEUP); // temp
		break;

//		case VK_DOWN:
//		case VK_UP:
//			break;

	case VK_HOME:
		ScrollChildWindow(SB_VERT, SB_TOP);
		break;

	case VK_END:
		ScrollChildWindow(SB_VERT, SB_BOTTOM);
		break;

	case VK_PRIOR: //VK_PAGE_UP:
		ScrollChildWindow(SB_VERT, SB_PAGEUP);
		break;

	case VK_NEXT: //VK_PAGE_DOWN:
		ScrollChildWindow(SB_VERT, SB_PAGEDOWN);
		break;
	}
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CSCWinThumbs::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Message handler code here and/or call default
	int iThumbNum = SCComputeHitTest(point);
	
	// location on thumb
	if (iThumbNum>=0 && iThumbNum!=m_iCurSel)
	{// move to this location
		m_iCurSel = iThumbNum;
		Invalidate();
		SCNotifyParent();
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

