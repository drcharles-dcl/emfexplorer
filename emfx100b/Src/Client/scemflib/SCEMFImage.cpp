/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFImage.h"

#include "SCEMF.h"
#include "SCEMFRasterizer.h"
#include "kSCEMFLibMsgs.h"

#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCPreviewView.h)		// for CPrintInfo
#include SC_INC_WINLIB(SCWinFile.h)
#include SC_INC_WINLIB(SCBitmap.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CLIPFORMAT s_cfRTF;


/////////////////////////////////////////////////////////////////////////////
// CSCEMFImage

IMPLEMENT_DYNCREATE(CSCEMFImage, CWnd)

BEGIN_MESSAGE_MAP(CSCEMFImage, CWnd)
	//{{AFX_MSG_MAP(CSCEMFImage)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCEMFImage construction/destruction

CSCEMFImage::CSCEMFImage():
	m_iCurPage(0),
    m_iWorldCx(0),
    m_iWorldCy(0),
	m_iAngle(0),
	m_hEmf(NULL),
	//
	m_iFitMode(SC_FIT_NONE),
	m_iCurZoom(SC_ZOOM100),
	m_rectMargins(0, 0, 0, 0),
	m_uiRasEngine(SC_ENGINE_GDI),
	m_bReverseVideo(FALSE),
	m_crPaperColor(RGB(255, 255, 255)),
	m_pEMFDoc(NULL),
	m_hCtxMenu(NULL),
	m_pICtlOwner(NULL),
	m_dwColorScheme(SC_CSM_CTLCOLOR_SYSINDEX),
	m_crCtlColor(SC_MAKE_SYSCOLOR(COLOR_APPWORKSPACE))
{
	SetRectEmpty(&m_rectPaper);
	m_DrawingAttributes.dwInkingMode = SC_DFLT_INKINGMODE;
#if 0
	// test
	m_DrawingAttributes.bBkSolid = TRUE;
#endif
}


void CSCEMFImage::SCReset()
{
	// App settings
	m_iCurPage = 0;
    m_iWorldCx = 0;
    m_iWorldCy = 0;
	m_iAngle = 0;
	m_hEmf = NULL;
	SetRectEmpty(&m_rectPaper);
	InitializeScrollBars(m_iWorldCx, m_iWorldCy);
}

CSCEMFImage::~CSCEMFImage()
{
}

BOOL CSCEMFImage::PreCreateWindow(CREATESTRUCT& cs)
{
	// Modify the Window class or styles here by modifying the CREATESTRUCT cs
	cs.style |= WS_HSCROLL|WS_VSCROLL|CS_DBLCLKS;
	return CWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSCEMFImage drawing
void CSCEMFImage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	SCPaint(&dc);
}

void CSCEMFImage::SCPaint(CDC* pDC)
{
	CRect rectClient;
	GetClientRect(&rectClient);
	int cx = rectClient.Width();
	int cy = rectClient.Height();

	// Add draw code for native data here
	if (!m_pEMFDoc || !m_pEMFDoc->SCGetNbPages() || !m_iCurPage)
	{// when there is no doc, paint all in white
		SCEMFExplorerPaint(pDC, &rectClient, SC_EMFLIB_UFMTMSG_EMFEXPLORER, SC_EMFLIB_UFMTMSG_DROPFILEs);
		return;
	}

	int xScroll  = GetScrollPos(SB_HORZ);
	int yScroll  = GetScrollPos(SB_VERT);

#ifdef SC_NO_MEMDC
	CDC* pPageDC = pDC;
#else
	CDC* pPageDC = &m_WorkMemDC;
	if (!m_WorkMemDC.m_hDC)
	{
		m_WorkMemDC.SCPrepareSurface(cx, cy);
		ASSERT(m_WorkMemDC.m_hDC);
	}
#endif

	int iDCState = pPageDC->SaveDC();
	pPageDC->SetWindowOrg(xScroll, yScroll);
	
	CSize PgSize;
	SCGetPageSize(PgSize);	// Contains rotation and scaling
	int iImgCx = PgSize.cx;
	int iImgCy = PgSize.cy;
	
	int xDest = m_iPgPosX;
	int yDest = m_iPgPosY;

	// paint non-covered region
	{
		int iXRight  = xDest + iImgCx;
		int iYBottom = yDest + iImgCy;
		if ((xDest>xScroll) || (yDest>yScroll) ||
			(iXRight<cx+xScroll) || (iYBottom<cy+yScroll))
		{
			CBrush* pRGBBrush = NULL;
			CBrush* pBrush;
			switch (m_dwColorScheme & SC_CSM_CTLCOLOR_MASK)
			{
			case SC_CSM_CTLCOLOR_SYSINDEX:
				pBrush = CBrush::FromHandle(::GetSysColorBrush((DWORD)SC_SYSCOLOR_INDEX(m_crCtlColor)));
				break;

			case SC_CSM_CTLCOLOR_RGBVALUE:
			default:
				// treat transparent as RGB in m_crCtlColor
				pBrush = pRGBBrush = new CBrush(SC_RGBCOLOR_VALUE(m_crCtlColor));
			}
			
			CRect rExclude(xDest, yDest, iXRight, iYBottom);
			if (m_dwColorScheme & SC_CSM_BORDER_MASK)
				rExclude.InflateRect(1, 1);
			
			// Image body
			pPageDC->ExcludeClipRect(&rExclude);
			
			if (m_dwColorScheme & SC_CSM_SHADOW_MASK)
			{
				// Shadow Right band
				rExclude.SetRect(iXRight, yDest + 2, rExclude.right + SC_SHADOW_WDT, rExclude.bottom + SC_SHADOW_WDT);
				pPageDC->ExcludeClipRect(&rExclude);
				
				// Shadow Bottom band
				rExclude.left = xDest + 2;
				rExclude.top = iYBottom;
				pPageDC->ExcludeClipRect(&rExclude);
			}
			
			OffsetRect(&rectClient, xScroll, yScroll);
			pPageDC->FillRect(&rectClient, pBrush); 
			pPageDC->SelectClipRgn(NULL);

			if (pRGBBrush)
				delete pRGBBrush;
		}
	}
	
	// paint page
		// background
	if (!m_DrawingAttributes.bBkSolid)
	{
		COLORREF crColor = (SCIsPartialReverseVideo())? SCGetReversedPaperColor() : SCGetTranslatedPaperColor();
		CBrush Brush;
		CRect rcBkgn(xDest, yDest, xDest + iImgCx, yDest + iImgCy);
		Brush.CreateSolidBrush(crColor);
		pPageDC->FillRect(&rcBkgn, &Brush);
		pPageDC->SetBkColor(crColor);
	}
		// image
	if (m_hEmf)
		SCDisplayPage(pPageDC->m_hDC, xDest, yDest, iImgCx, iImgCy);
	
		// shadow
	if (m_dwColorScheme & (SC_CSM_BORDER_MASK|SC_CSM_SHADOW_MASK))
	{
		COLORREF crColor = m_bReverseVideo ? SCGetReversedPaperColor() : SCGetTranslatedPaperColor();
		SCDrawFrameAndShadow(pPageDC, xDest, yDest, iImgCx, iImgCy,
			((m_dwColorScheme & SC_CSM_SHADOW_MASK)? SC_SHADOW_WDT : 0),
			crColor, (m_dwColorScheme & SC_CSM_BORDER_MASK));
	}

	pPageDC->RestoreDC(iDCState);

#ifndef SC_NO_MEMDC
	pDC->StretchBlt(0, 0,	cx, cy,	pPageDC, 0, 0, cx, cy, SRCCOPY);
#endif
}


BOOL CSCEMFImage::SCRasterizeEMF(HDC hDC, HENHMETAFILE hEMF, CRect& rcPlay, BOOL bPrinting/*=FALSE*/)
{
	BOOL bOk = FALSE;
	BeginWaitCursor();

	if (SCGdipGetEMFType(hEMF)!=EmfTypeEmfOnly)
		bOk = SCGdipPlayMetafile(hDC, hEMF, rcPlay);
	else
	if (SC_ENGINE_GDIP==m_uiRasEngine)
	{// Note: rasterizer must be detached from hDC before reusing DC
		SCEMFRasterizer Rasterizer;
		CSCEMFdcRenderer& rRenderer = Rasterizer.SCGetRenderer();
		m_DrawingAttributes.bPrinting = bPrinting;
		rRenderer.SCSetDrawingAttributes(m_DrawingAttributes);
		Rasterizer.SCBreakMetafile(hDC, hEMF, NULL, (LPRECT)&rcPlay); // equivalent to attach
		bOk = TRUE;
		// DC detachment point
	} else
	{
		// default to GDI
		bOk = PlayEnhMetaFile(hDC, hEMF, (LPRECT)&rcPlay);
	}

	EndWaitCursor();
	return bOk;
}

// Normal display
void CSCEMFImage::SCDisplayPage(HDC hDC, int xDest, int yDest, int iWdt, int iHgt)
{
	// We are going to seriously alter the DC (rotation, metafile playing, etc.).
	// So save its state
	int iState = SaveDC(hDC);

	// Ensure that nothing goes outside the page (before rotation)
	::IntersectClipRect(hDC, xDest, yDest, xDest + iWdt, yDest + iHgt);
	
	// Current paper size may differ from actual EMF size. So we must
	// compute the playing destination by subtrating from the destination (xDest, yDest)
	// the uppper left corner PgPaper(left, top) of the page. Note that PgPaper is
	// expressed relatively to the uppper left corner of the EMF.
	int iPlayX = xDest - m_rectPaper.left;
	int iPlayY = yDest - m_rectPaper.top;
	
	// Position of the EMF rectangle in world coordinates for playing
	CSize sizeEMF;
	SCGetEMFPlaySize(m_hEmf, sizeEMF);
	CRect rcPlay(iPlayX, iPlayY,
		iPlayX + MulDiv(sizeEMF.cx, m_iCurZoom, SC_ZOOM100),
		iPlayY + MulDiv(sizeEMF.cy, m_iCurZoom, SC_ZOOM100));
	
	int iGrOldMode;
	if (m_iAngle)
	{// Position of the PAGE rectangle for rotation in world coordinates
		CRect rcRotate = m_rectPaper;
		rcRotate.OffsetRect(iPlayX, iPlayY);
		
		// Give the actual page rectangle for the rotation
		// and the actual destination (xDest, yDest) for the translation
		SCRotateDC(hDC, m_iAngle, rcRotate, xDest, yDest, iGrOldMode);
	}
	
	// Rasterize
	BOOL bOK = SCRasterizeEMF(hDC, m_hEmf, rcPlay);
	
	// Comments
	PSCEMFDocPage pDocPage = m_pEMFDoc->SCGetDocPage(m_iCurPage-1);
	ASSERT(pDocPage);
	switch (m_iAngle)
	{
	case 90:
	case 270:
		pDocPage->SCDisplayPageComments(hDC, float(m_iCurZoom)/float(SC_ZOOM100),
			xDest, yDest, iHgt, iWdt);
		break;
	default:
		pDocPage->SCDisplayPageComments(hDC, float(m_iCurZoom)/float(SC_ZOOM100),
			xDest, yDest, iWdt, iHgt);
	}

	// Restore DC state
	RestoreDC(hDC, iState);
	
	// Perform full rvideo
	if (SCIsFullReverseVideo())
		PatBlt(hDC, xDest-1, yDest-1, iWdt+2, iHgt+2, DSTINVERT);
}

// Save/Copy/Print
void CSCEMFImage::SCDisplayPageEx(HDC hDC, HENHMETAFILE hEmf,
									   CRect& rcDest, CRect& rcPaper,
									   SCGDIpDrawingAttributes& rAttributes,
									   int iAngle/*=0*/, int iZoom/*=SC_ZOOM100*/,
									   BOOL bInvert/*=FALSE*/)
{
	// Paper
	CRect PaperRect = rcPaper;

	// Position of the EMF rectangle in world coordinates for playing
	CSize sizeEMF;
	SCGetEMFPlaySize(hEmf, sizeEMF);

	int iPlayX = rcDest.left - PaperRect.left;
	int iPlayY = rcDest.top - PaperRect.top;
	CRect rcPlay(iPlayX, iPlayY,
		iPlayX + MulDiv(sizeEMF.cx, iZoom, SC_ZOOM100),
		iPlayY + MulDiv(sizeEMF.cy, iZoom, SC_ZOOM100));

	int iState = SaveDC(hDC);
	::IntersectClipRect(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom);

	int iGrOldMode;
	if (iAngle)
	{// Position of the PAGE rectangle for rotation in world coordinates
		CRect rcRotate = PaperRect;
		rcRotate.OffsetRect(iPlayX, iPlayY);
		SCRotateDC(hDC, iAngle, rcRotate, rcDest.left, rcDest.top, iGrOldMode);
	}
	
	// Rasterize
	BOOL bOK = FALSE;
	if (SCGdipGetEMFType(hEmf)!=EmfTypeEmfOnly)
		bOK = SCGdipPlayMetafile(hDC, hEmf, rcPlay);
	else
	if (SC_ENGINE_GDIP==m_uiRasEngine)
	{// Note: rasterizer must be detached from hDC before reusing DC
		SCEMFRasterizer Rasterizer;
		CSCEMFdcRenderer& rRenderer = Rasterizer.SCGetRenderer();
		rRenderer.SCSetDrawingAttributes(rAttributes);
		Rasterizer.SCBreakMetafile(hDC, hEmf, NULL, (LPRECT)&rcPlay);
		bOK = TRUE;
		// DC detachment point
	} else
	{
		bOK = PlayEnhMetaFile(hDC, hEmf, (LPRECT)&rcPlay);
	}

	// Restore DC state
	RestoreDC(hDC, iState);

	// Perform full rvideo
	if (bInvert)
		PatBlt(hDC, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), DSTINVERT);
}


/////////////////////////////////////////////////////////////////////////////
// CSCEMFImage message handlers
void CSCEMFImage::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

#ifndef SC_NO_MEMDC
	m_WorkMemDC.SCPrepareSurface(cx, cy);
#endif
	SizeScrollBars(cx, cy);
	SCComputeFitZoom();
	SCComputeRowsCols();
}

COLORREF CSCEMFImage::SCGetTranslatedPaperColor()
{
	switch (m_dwColorScheme & SC_CSM_PAPERCOLOR_MASK)
	{
	case SC_CSM_PAPERCOLOR_SYSINDEX:
		return ::GetSysColor((DWORD)SC_SYSCOLOR_INDEX(m_crPaperColor));
		
	case SC_CSM_PAPERCOLOR_RGBVALUE:
		return SC_RGBCOLOR_VALUE(m_crPaperColor);
		
	//default:
	}
	return SCGetTranslatedCtlColor();
}

COLORREF CSCEMFImage::SCGetTranslatedCtlColor()
{
	switch (m_dwColorScheme & SC_CSM_CTLCOLOR_MASK)
	{
	case SC_CSM_CTLCOLOR_SYSINDEX:
		return ::GetSysColor((DWORD)SC_SYSCOLOR_INDEX(m_crCtlColor));
		
	case SC_CSM_CTLCOLOR_RGBVALUE:
		return SC_RGBCOLOR_VALUE(m_crCtlColor);
		
	//default:
	}
	return ::GetSysColor(COLOR_WINDOW);
}

void CSCEMFImage::SCRotateLeft() 
{
	m_iAngle -= 90;
	if (m_iAngle<0)
		m_iAngle += 360;
	ASSERT(m_iAngle>=0);
	SCComputeFitZoom();
	SCRefresh();	
}

void CSCEMFImage::SCRotateRight() 
{
	m_iAngle += 90;
	if (m_iAngle>=360)
		m_iAngle -= 360;
	ASSERT(m_iAngle>=0);
	SCComputeFitZoom();
	SCRefresh();	
}

///
/// Show or hide scrollbars as necessary.
///
void CSCEMFImage::InitializeScrollBars(int width, int height)
{
    RECT        rClient;
    SCROLLINFO  ScrollInfo;

    // Get windows client size.
    GetClientRect(&rClient);

    // If client size is equal to dib size, then add one to client size
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
    ScrollInfo.fMask = SIF_PAGE|SIF_RANGE;
    ScrollInfo.nMin = 0;
    ScrollInfo.nMax = height;
    ScrollInfo.nPage = rClient.bottom;
    SetScrollInfo(SB_VERT, &ScrollInfo, TRUE);

    // Initialize horizontal scroll bar.
    ScrollInfo.nMax = width;
    ScrollInfo.nPage = rClient.right;
    SetScrollInfo(SB_HORZ, &ScrollInfo, TRUE);
}

///
/// Resize the scrollbars' page information.
///
void CSCEMFImage::SizeScrollBars(int uiWindowWidth, int uiWindowHeight)
{
	static BOOL m_bAdjusting = FALSE;
    // Only size for valid windows and dib info.
    // Don't SizeScrollBars if in process of adjusting them.
    if (IsWindow(m_hWnd) && !m_bAdjusting)
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

///
/// Scroll in answer to a scrollbar event.
///
BOOL CSCEMFImage::ScrollChildWindow(int nScrollBar, UINT wScrollCode, int iCount /*=1*/)
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
    else if (ScrollInfo.nMax - (int)ScrollInfo.nPage + 1 < ScrollInfo.nPos)
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


//////////////////////////////////////////////////////////////////////////////////
// Scrolling events
//
void CSCEMFImage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	ScrollChildWindow(SB_HORZ, nSBCode);
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSCEMFImage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	ScrollChildWindow(SB_VERT, nSBCode);
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

//////////////////////////////////////////////////////////////////////////////////
// Page navigation events
//

void CSCEMFImage::SCGotoPage(int iPage) 
{
	if (!m_pEMFDoc || !m_pEMFDoc->SCGetNbPages())
		return;
	
	ASSERT(iPage>=1 && iPage<=(int)m_pEMFDoc->SCGetNbPages());

	// load
	m_hEmf = m_pEMFDoc->SCGetPageEMF(iPage-1);
	m_iCurPage = iPage;
	
	SCComputePageInfo(iPage);
	
	if (m_iFitMode!=SC_FIT_NONE)
		SCComputeFitZoom();
	
	SCComputeRowsCols();
	Invalidate();
}

void CSCEMFImage::SCGotoFirstPage() 
{
	SCComputeFitZoom();
	SCGotoPage(1);
}

void CSCEMFImage::SCGotoPrevPage() 
{
	SCGotoPage(m_iCurPage-1);
}

void CSCEMFImage::SCGotoNextPage() 
{
	SCGotoPage(m_iCurPage+1);
}

void CSCEMFImage::SCGotoLastPage() 
{
	SCGotoPage(SCGetNumPages());
}

//////////////////////////////////////////////////////////////////////////////////
// Creation/background events
//
int CSCEMFImage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CSCEMFImage::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE; // do not erase
	//return CWnd::OnEraseBkgnd(pDC);
}

//////////////////////////////////////////////////////////////////////////////////
// Other utilities
//
void CSCEMFImage::SCGetPageBlackBox(int iPage, CRect& rcElems)
{
	ASSERT(m_pEMFDoc);
	ASSERT(iPage);
	PSCEMFDocPage pDocPage = m_pEMFDoc->SCGetDocPage(iPage-1);

	ASSERT(pDocPage);
	pDocPage->SCGetPageBlackBox(rcElems);
}

void CSCEMFImage::SCGetPageSize(CSize& PgSize)
{
	if (m_pEMFDoc && m_pEMFDoc->SCGetNbPages())
		SCGetPageBlackBox((m_iCurPage) ? m_iCurPage: 1, m_rectPaper);
	else
		m_rectPaper.SetRectEmpty();
	m_rectPaper.InflateRect(m_rectMargins.left, m_rectMargins.top, m_rectMargins.right, m_rectMargins.bottom);

	// Note: the paper rectangle IS NOT rotated. It is considered as bound to the EMF.
	// So it rotates only when the playing DC is rotated.
	m_rectPaper.left = MulDiv(m_rectPaper.left, m_iCurZoom, SC_ZOOM100);
	m_rectPaper.right = MulDiv(m_rectPaper.right, m_iCurZoom, SC_ZOOM100);
	m_rectPaper.top = MulDiv(m_rectPaper.top, m_iCurZoom, SC_ZOOM100);
	m_rectPaper.bottom = MulDiv(m_rectPaper.bottom, m_iCurZoom, SC_ZOOM100);

	// Note: image dimensions ARE rotated
	PgSize.cx = m_rectPaper.Width();
	PgSize.cy = m_rectPaper.Height();
	switch (m_iAngle)
	{
	case 90:
	case 270:
		{// rotate
			SMC_ISWAP(PgSize.cx, PgSize.cy);
		}
		break;
	}
}

// number rows and columns by row
void CSCEMFImage::SCComputeRowsCols()
{
	CSize PgSize;
	SCGetPageSize(PgSize); // current page or nothing
	int iImgCx = PgSize.cx;
	int iImgCy = PgSize.cy;

	CRect rc;
	GetClientRect(rc);
	int iWndCx = rc.Width();
	int iWndCy = rc.Height();

	// try to set an y margin (for now, one row)
	if (iImgCy<iWndCy)
		m_iWorldCy = iWndCy;
	else
		m_iWorldCy = MIN_MARGIN*2 + iImgCy;
	ASSERT(m_iWorldCy>=iWndCy);

	// compute world dimensions for scrollbars
	if (iImgCx<iWndCx)
		m_iWorldCx = iWndCx;
	else
		m_iWorldCx = MIN_MARGIN*2 + iImgCx;
	ASSERT(m_iWorldCx>=iWndCx);

	// center
	m_iPgPosX = (m_iWorldCx - iImgCx)/2;
	m_iPgPosY = (m_iWorldCy - iImgCy)/2;
	ASSERT(m_iPgPosX>=0);
	ASSERT(m_iPgPosY>=0);
	InitializeScrollBars(m_iWorldCx, m_iWorldCy);
}

void CSCEMFImage::SCComputeFitZoom()
{
	if (!m_iCurPage || SC_FIT_NONE==m_iFitMode)
		return;

	// load page
	m_hEmf = m_pEMFDoc->SCGetPageEMF(m_iCurPage-1);
	if (!m_hEmf)
		return;

	// account for extra margins
	CRect rcElems;
	SCGetPageBlackBox(m_iCurPage, rcElems);
	CSize sizeEMF(rcElems.Width() + m_rectMargins.left + m_rectMargins.right + MIN_MARGIN*2,
		rcElems.Height() + m_rectMargins.top + m_rectMargins.bottom + MIN_MARGIN*2);
	// Rotate
	if ((90==m_iAngle || 270==m_iAngle))
	{
		SMC_ISWAP(sizeEMF.cx, sizeEMF.cy);
	}

	// Available client
	CRect rc;
	GetClientRect(rc);
	int iWndCx = rc.Width();
	int iWndCy = rc.Height();
#if 0
	// TODO: take care of the vertical scrollbar, if it would show
	if (SC_FIT_WIDTH==m_iFitMode)
	{
		// iWndCx -= GetSystemMetrics(SM_CXVSCROLL);
	}
#endif

	// scale
	float fScale;
	switch (m_iFitMode)
	{
	case SC_FIT_WIDTH:
		fScale = float(iWndCx)/float(sizeEMF.cx);
		break;

	case SC_FIT_PAGE:
		if (iWndCx>=iWndCy)
		{// attempt keep y, and reduce x
			fScale = float(iWndCy)/float(sizeEMF.cy);
			if (fScale*sizeEMF.cx > iWndCx)
				fScale = float(iWndCx)/float(sizeEMF.cx);
		} else
		{// attempt keep x, and reduce y
			fScale = float(iWndCx)/float(sizeEMF.cx);
			if (fScale*sizeEMF.cy > iWndCy)
				fScale = float(iWndCy)/float(sizeEMF.cy);
		}
		break;

	default:
		ASSERT(0);
		return;
	}

	m_iCurZoom = (int)(fScale*SC_ZOOM100);
	if (m_iCurZoom<SC_MIN_ZOOM)
		m_iCurZoom = SC_MIN_ZOOM;
	else
	if (m_iCurZoom>SC_MAX_ZOOM)
		m_iCurZoom = SC_MAX_ZOOM;
}

void CSCEMFImage::SCComputePageInfo(int iPage)
{
	ASSERT(m_pEMFDoc);
	ASSERT(iPage>=1 && iPage<=(int)m_pEMFDoc->SCGetNbPages());

	PSCEMFDocPage pDocPage = m_pEMFDoc->SCGetDocPage(iPage - 1);

	ASSERT(pDocPage);
	pDocPage->SCComputePageInfo(m_bReverseVideo, m_rectMargins);
}

/////////////////////////////////////////////////////////////////////////////
// CSCEMFImage printing

// Note: does not work!
void CSCEMFImage::SCPrintImage(CDC* pDC, CPrintInfo* pInfo, BOOL bFit/*=FALSE*/)
{
	if (!pDC)
		return;
	if (!m_pEMFDoc || !m_pEMFDoc->SCGetNbPages())
		return;

	// paper in logical coordinates
	CRect rcPaper = pInfo->m_rectDraw;
	int xDest = rcPaper.left;
	int yDest = rcPaper.top;

	HDC hDC = pDC->m_hDC;
	// We are going to seriously alter the DC (rotation, metafile playing, etc.).
	// So save its state
	int iState = SaveDC(hDC);

	SCComputePageInfo(pInfo->m_nCurPage);

	// scaling
	int iPgIdx = pInfo->m_nCurPage - 1;
	HENHMETAFILE hEMF = m_pEMFDoc->SCGetPageEMF(iPgIdx);
	CPoint PageStart;
	{
		float iPrnDevResX = (float)GetDeviceCaps(pDC->m_hAttribDC, LOGPIXELSX);
		long lEmfDPIX = 0;
		long lEmfDPIY = 0;

		SCGetEMFDPIs(hEMF, lEmfDPIX, lEmfDPIY);
		m_DrawingAttributes.fPageScale = iPrnDevResX/lEmfDPIX;

		// get offset of the printable area in device units an convert to logical
		PageStart.x = GetDeviceCaps(pDC->m_hAttribDC, PHYSICALOFFSETX);
		PageStart.y = GetDeviceCaps(pDC->m_hAttribDC, PHYSICALOFFSETY);
		DPtoLP(pDC->m_hAttribDC, &PageStart, 1);
	}

	// Position of the EMF rectangle in world coordinates for playing
	int iAngle = m_iAngle;
	CSize sizeEMF;
	float fScaleEMF=1.0;
	SCGetEMFPlaySize(hEMF, sizeEMF, pDC->m_hAttribDC, &fScaleEMF);
	CSize sizeEMFScaled((int)(m_DrawingAttributes.fPageScale*sizeEMF.cx - 2*PageStart.x),
						(int)(m_DrawingAttributes.fPageScale*sizeEMF.cy - 2*PageStart.y));

	int iWdt = sizeEMF.cx;
	int iHgt = sizeEMF.cy;

	int iImgCx = sizeEMFScaled.cx;
	int iImgCy = sizeEMFScaled.cy;
	int iPaperCx = rcPaper.Width();
	int iPaperCy = rcPaper.Height();

	if (bFit)
	{
		if (iImgCx > iPaperCx)
		{
			iWdt = (int)(iPaperCx/m_DrawingAttributes.fPageScale);
			iHgt = MulDiv(iWdt, sizeEMF.cy, sizeEMF.cx);
		} else
		if (iImgCy > iPaperCy)
		{
			iHgt = (int)(iPaperCy/m_DrawingAttributes.fPageScale);
			iWdt = MulDiv(iHgt, sizeEMF.cx, sizeEMF.cy);
		}
	}
	else // attempt auto rotate
	if (iImgCx > iImgCy)
	{// landscape-like image
		if (iPaperCx > iPaperCy)
		{// landscape-like paper
			if (iImgCx > iPaperCx)
			{// x-fit
				iWdt = (int)(iPaperCx/m_DrawingAttributes.fPageScale);
				iHgt = MulDiv(iWdt, iImgCy, iImgCx);
			} else
			if (iImgCy > iPaperCy)
			{// y-fit
				iHgt = (int)(iPaperCy/m_DrawingAttributes.fPageScale);
				iWdt = MulDiv(iHgt, iImgCx, iImgCy);
			}
		}
		else
		{// portrait-like paper
			if (iImgCx > iPaperCx)
			{// rotate and fit
				iAngle = 270;
				if (iImgCx > iPaperCy)
				{// rotated x-fit
					iWdt = (int)(iPaperCy/m_DrawingAttributes.fPageScale);
					iHgt = MulDiv(iWdt, iImgCy, iImgCx);
				} else
				if (iImgCy > iPaperCx)
				{// rotated y-fit
					iHgt = (int)(iPaperCx/m_DrawingAttributes.fPageScale);
					iWdt = MulDiv(iHgt, iImgCx, iImgCy);
				}
			} //else (iImgCy <= iPaperCy)
#ifdef _DEBUG
			else
			{
				ASSERT((iImgCy <= iPaperCy));
			}
#endif
		}
	}
	else
	{// portrait-like image
		if (iPaperCx > iPaperCy)
		{// landscape-like paper
			if (iImgCy > iPaperCy)
			{// rotate and fit
				iAngle = 270;
				if (iImgCy > iPaperCx)
				{// rotated x-fit
					iHgt = (int)(iPaperCx/m_DrawingAttributes.fPageScale);
					iWdt = MulDiv(iHgt, iImgCx, iImgCy);
				} else
				if (iImgCx > iPaperCy)
				{// rotated y-fit
					iWdt = (int)(iPaperCy/m_DrawingAttributes.fPageScale);
					iHgt = MulDiv(iWdt, iImgCy, iImgCx);
				}
			} // else iImgCx <= iPaperCx
#ifdef _DEBUG
			else
			{
				ASSERT((iImgCx <= iPaperCx));
			}
#endif
		}
		else
		{// portrait-like paper
			if (iImgCx > iPaperCx)
			{// x-fit
				iWdt = (int)(iPaperCx/m_DrawingAttributes.fPageScale);
				iHgt = MulDiv(iWdt, iImgCy, iImgCx);
			} else
			if (iImgCy > iPaperCy)
			{// y-fit
				iHgt = (int)(iPaperCy/m_DrawingAttributes.fPageScale);
				iWdt = MulDiv(iHgt, iImgCx, iImgCy);
			}
		}
	}

	// TODO: compute scale (in preview and actual printing)
	iWdt = (int)(iWdt*m_DrawingAttributes.fPageScale);
	iHgt = (int)(iHgt*m_DrawingAttributes.fPageScale);
	m_DrawingAttributes.fPageScale = 1;
	//

	CRect rcPlay(xDest, yDest, xDest + iWdt, yDest + iHgt);

	if (iAngle)
	{// Position of the PAGE rectangle for rotation in world coordinates
		CRect rcRotate(0, 0, sizeEMFScaled.cx, sizeEMFScaled.cy);
		
		// Give the actual page rectangle for the rotation
		// and the actual destination (xDest, yDest) for the translation
		int iGrOldMode;
		SCRotateDC(hDC, iAngle, rcRotate, xDest, yDest, iGrOldMode);
	}

	// Rasterize
#ifndef SC_RENDERER_CAN_DOROP
	// In this version, GDI+ is not used for printing
	BOOL bOK;
	if (SCGdipGetEMFType(hEMF)!=EmfTypeEmfOnly)
		bOK = SCGdipPlayMetafile(hDC, hEMF, rcPlay);
	else
		bOK = PlayEnhMetaFile(hDC, hEMF, (LPRECT)&rcPlay);
#else
	BOOL bOK = SCRasterizeEMF(hDC, hEMF, rcPlay, TRUE);
#endif

	// Comments
	PSCEMFDocPage pDocPage = m_pEMFDoc->SCGetDocPage(iPgIdx);
	ASSERT(pDocPage);
	switch (iAngle)
	{
	case 90:
	case 270:
		pDocPage->SCDisplayPageComments(hDC, float(m_iCurZoom)/float(SC_ZOOM100),
			xDest, yDest, iHgt, iWdt);
		break;
	default:
		pDocPage->SCDisplayPageComments(hDC, float(m_iCurZoom)/float(SC_ZOOM100),
			xDest, yDest, iWdt, iHgt);
	}

	// Restore DC state
	RestoreDC(hDC, iState);

	// Perform full rvideo
	if (SCIsFullReverseVideo())
		PatBlt(hDC, xDest-1, yDest-1, iWdt+2, iHgt+2, DSTINVERT);
}

void CSCEMFImage::SCInitConverter(SCEMFConverter& converter)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	
	converter.SCSetEMFDoc(m_pEMFDoc);
	converter.SCSetCurZoom(m_iCurZoom);
	converter.SCSetAngle(m_iAngle);
	converter.SCSetMargins(m_rectMargins);
	converter.SCSetReverseVideo(m_bReverseVideo);
	converter.SCSetRasEngine(m_uiRasEngine);
	converter.SCSetPaperColor(SCGetTranslatedPaperColor());
	converter.SCSetRectClient(rcClient);
	converter.SCSetFitMode(m_iFitMode);
	converter.SCSetDrawingAttributes(m_DrawingAttributes);
}

void CSCEMFImage::SCSaveImageDiscardHandle(HANDLE hRes, LPCTSTR lpszPathname, int iType, int iSubType)
{
	if (hRes)
	{
		switch (iType)
		{
		case SC_FTYPE_EMF:
		case SC_FTYPE_WMF:
			if (SC_FTYPE_EMF==iType)
				SCWriteEMFtoDisk((HENHMETAFILE)hRes, lpszPathname);
			else
				SCConvertEMFtoWMF((HENHMETAFILE)hRes, lpszPathname);
			::DeleteEnhMetaFile((HENHMETAFILE)hRes);
			break;
			
		case SC_FTYPE_IMG:
			 // TODO: for JPEGs ask quality level to user
			SCGDIpSaveImage((HBITMAP)hRes, lpszPathname, iSubType);
			::DeleteObject((HBITMAP)hRes);
			break;
			
		default:
			ASSERT(0);
		}
	}
}

///
/// Save current EMF or all pages to disk
///
void CSCEMFImage::SCSaveDocument(LPCTSTR lpszPathname, int iType, BOOL bSaveAll/*=FALSE*/, BOOL bSaveAsIs/*=TRUE*/)
{
	if (!m_pEMFDoc)
		return;

	int iNbPages=m_pEMFDoc->SCGetNbPages();
	if (0==iNbPages)
		return;

	int iSubType = iType & SC_SUBTYPE_MASK;
	iType &= SC_FTYPE_MASK;

	SCEMFConverter converter;
	SCInitConverter(converter);
	CRect PaperRect;
	if (bSaveAll)
	{// save each page
		CString strPrefix;
		TCHAR szExt[_MAX_EXT];
		{
			TCHAR szDrive[SC_MAX_UNC_DRIVE];
			TCHAR szDir[_MAX_DIR];
			TCHAR szFname[_MAX_FNAME];
			SCSplitPath(lpszPathname, szDrive, szDir, szFname, szExt);
			strPrefix.Format(_T("%s%s%s"), szDrive, szDir, szFname);
		}

		CString strName;
		for (int iPage=0; (iPage<iNbPages); iPage++)
		{
			strName.Format(_T("%s%d%s"), strPrefix, iPage+1, szExt);
			HANDLE hRes = converter.SCCopyPage(iPage+1, iType, iSubType, bSaveAsIs);
			SCSaveImageDiscardHandle(hRes, strName, iType, iSubType);
		}
	} else
	{
		ASSERT(m_iCurPage);
		HANDLE hRes = converter.SCCopyPage(m_iCurPage, iType, iSubType, bSaveAsIs);
		SCSaveImageDiscardHandle(hRes, lpszPathname, iType, iSubType);
	}
}


///
/// Copy the current page to cliboard in many formats.
/// (no partial copy is provided in this version; pSrcRect is not used)
///
BOOL CSCEMFImage::SCCopyToClipBoard(CRect* pSrcRect/*=NULL*/) 
{
	UNUSED(pSrcRect);	// Zzzz!... let it sleep for now

	if (!OpenClipboard())
		return FALSE;
	
	BOOL bOk = FALSE;
	if (::EmptyClipboard())
	{
		HANDLE hCopied = NULL;

		SCEMFConverter converter;
		SCInitConverter(converter);
		
		// EMF
		HANDLE hData = converter.SCCopyPage(m_iCurPage, SC_FTYPE_EMF, SC_SUBTYPE_EMF_EMF, TRUE);
		if (hData)
		{
			hCopied = ::SetClipboardData(CF_ENHMETAFILE, (HENHMETAFILE)hData);
			if (!hCopied)
				::DeleteEnhMetaFile((HENHMETAFILE)hData);
			bOk = TRUE;
		}
		
		// Bitmap
		hData = converter.SCCopyPage(m_iCurPage, SC_FTYPE_IMG, SC_SUBTYPE_IMG_BMP, TRUE);
		if (hData)
		{
			hCopied = ::SetClipboardData(CF_BITMAP, (HBITMAP)hData);
			if (!hCopied)
				::DeleteObject((HBITMAP)hData);
			bOk = TRUE;
		}
	}
	::CloseClipboard();
	return bOk;
}

///
/// Paste the clipboard content as a new full page
/// (no partial paste is provided in this version; pDestRect is not used)
///
BOOL CSCEMFImage::SCPasteFromClipBoard(CRect* pDestRect/*=NULL*/) 
{
	UNUSED(pDestRect);	// Zzzz!... let it sleep for now

	if (!m_pEMFDoc)
		return FALSE;

    if (!OpenClipboard())
		return FALSE;

	BOOL bOk = FALSE;
	HEMFVECTOR vectHandles;
	BeginWaitCursor();
	
	// Check main vector formats
	UINT uiFmt = 0;
	while (uiFmt = EnumClipboardFormats(uiFmt))
	{
		if (uiFmt == CF_ENHMETAFILE)
		{
			HENHMETAFILE hEMF = (HENHMETAFILE)::GetClipboardData(CF_ENHMETAFILE);
			if (hEMF)
			{
				hEMF = CopyEnhMetaFile(hEMF, NULL);
				if (hEMF)
					vectHandles.push_back(hEMF);
			}
			break;
		} else
		if (uiFmt == s_cfRTF)
		{
			HANDLE hMem = ::GetClipboardData(s_cfRTF);
			SCConvertRTFtoEMF(hMem, vectHandles); 
			break;
		}
	}

	// Check text formats
	if (!vectHandles.size())
	{
		HANDLE hMem = ::GetClipboardData(CF_UNICODETEXT);
		if (hMem)
			SCConvertRTFtoEMF(hMem, vectHandles, FALSE);
	}

	// Check bitmap formats
    if (!vectHandles.size())
	{
		HENHMETAFILE hEMF = NULL;
		HBITMAP hbm;
		if (hbm = (HBITMAP)GetClipboardData(CF_BITMAP))
		{
			// Any palette in the clipboard at this time is assumed to be the one
			// the bitmap is realized against.
			HPALETTE hpal = (HPALETTE)GetClipboardData(CF_PALETTE);
			if (!hpal)
				hpal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
			hEMF = SCGDIpConvertBitmaptoEMF(hbm, hpal);
		} else
		{
			HANDLE hMem = NULL;
			if (hMem =  GetClipboardData(CF_TIFF))
				hEMF = SCGDIpConvertImagetoEMF(hMem);
		}
		if (hEMF)
			vectHandles.push_back(hEMF);
	}

	CloseClipboard();

    // Add pages to the doc
	if (vectHandles.size())
		bOk = m_pEMFDoc->SCPasteEMFPages(vectHandles);

	EndWaitCursor();
	return bOk;
}

// Display context menu
void CSCEMFImage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_hCtxMenu)
	{
		CMenu menu;
		if (menu.Attach(m_hCtxMenu))
		{
			//convert client coordinates to screen coordinates
			ClientToScreen(&point);
			
			//show context menu
			CWnd* pOwner = this;
			if (m_pICtlOwner)
			{
				pOwner = m_pICtlOwner->SCGetMenuOwner();
				if (!pOwner)
					pOwner = this;
				m_pICtlOwner->SCSetupCtxMenu(&menu);
			}
			menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, pOwner);
			menu.Detach();
		}
	}

	CWnd::OnRButtonDown(nFlags, point);
}

BOOL CSCEMFImage::OnCommand( WPARAM wParam, LPARAM lParam )
{
	if (m_pICtlOwner)
		m_pICtlOwner->SCOnCtxCommand(wParam, lParam);
	return TRUE;
}


