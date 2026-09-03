/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "EMFexplorer.h"

#include "EMFtoolbar.h"
#include "EMFexplorerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Manual bookkeeping of button indices (used to access toolbar bitmaps)
// Warning: must be edited if toolbar layout changes
#define PAGE_EDITBOX_INDEX			12
#define ZOOM_COMBOBOX_INDEX			18
#define COLOR_COMBOBOX_INDEX		25

// Manual children controls IDs (used in notifications)
// Renamed for comformance to naming convention.
// But must have toolbar button IDs for proper tooltip management
#define IDC_GOTOPAGE_BOX			ID_TBNAV_TOPAGE
#define IDC_ZOOM_BOX				ID_TBVIEW_ZOOM
#define IDC_COLOR_BOX				ID_TBVIEW_BKCOLOR

/////////////////////////////////////////////////////////////////////////////
// CEMFToolBar construction/destruction/creation

CEMFToolBar::CEMFToolBar():
	m_iNbPages(0)
{
}

CEMFToolBar::~CEMFToolBar()
{
}

BOOL CEMFToolBar::SCCreateChildren()
{
	CRect rect;
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
		hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);

	// PAGE:
	// transform button to separator
	int iEdWidth = 2*16;
	this->SetButtonInfo(PAGE_EDITBOX_INDEX, ID_TBNAV_TOPAGE, TBBS_SEPARATOR, iEdWidth);
	this->GetItemRect(PAGE_EDITBOX_INDEX, &rect);
	rect.top += 2;
	rect.bottom -= 2;

	if (!m_GotoPgBox.Create(
		ES_LEFT|ES_AUTOHSCROLL|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
		rect, this, IDC_GOTOPAGE_BOX))
	{
		TRACE0("Failed to create the goto-page edit-box\n");
		return FALSE;
	}
	// set 3D look
	m_GotoPgBox.ModifyStyleEx(0,WS_EX_OVERLAPPEDWINDOW,
		SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOCOPYBITS|SWP_DRAWFRAME);
	// set font
	m_GotoPgBox.SendMessage(WM_SETFONT, (WPARAM)hFont);
	
	// ZOOM:
	// transform button into separator
	int iCmbWidth = 4*16;
	this->SetButtonInfo(ZOOM_COMBOBOX_INDEX, ID_TBVIEW_ZOOM, TBBS_SEPARATOR, iCmbWidth);
	this->GetItemRect(ZOOM_COMBOBOX_INDEX, &rect);
	rect.bottom = rect.top + 16*20;// hard-coded DropHeight;
	
	if (!m_ZoomBox.Create(
		WS_CHILD | CBS_DROPDOWN | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
		CBS_AUTOHSCROLL | CBS_HASSTRINGS | CBS_SORT,
		rect, this, IDC_ZOOM_BOX))
	{
		TRACE0("Failed to create the zoom combo-box\n");
		return FALSE;
	}
	m_ZoomBox.SCTrapEnterKey(TRUE);
	// set font
	m_ZoomBox.SendMessage(WM_SETFONT, (WPARAM)hFont);

	// COLOR:
	// transform button into separator
	this->SetButtonInfo(COLOR_COMBOBOX_INDEX, ID_TBVIEW_BKCOLOR, TBBS_SEPARATOR, iCmbWidth+32);
	this->GetItemRect(COLOR_COMBOBOX_INDEX, &rect);
	rect.bottom = rect.top + SC_COLORCMB_DROP_HEIGHT_RGB;// hard-coded DropHeight;
	
	if (!m_ColorBox.Create(
		WS_CHILD | SC_COLORCMB_DFLT_STYLE | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL,
		rect, this, IDC_COLOR_BOX))
	{
		TRACE0("Failed to create the color combo-box\n");
		return FALSE;
	}
	// set font
	m_ColorBox.SendMessage(WM_SETFONT, (WPARAM)hFont);
	m_ColorBox.InitColorsComboBox(RGB(255, 255, 255));

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CEMFToolBar utilities

void CEMFToolBar::SCSetNbPages(int iNbPages)
{
	m_iNbPages = iNbPages;
	m_GotoPgBox.SCSetMinMax(1, iNbPages);
	if (iNbPages && IsWindow(m_hWnd))
		SCSetCurPage(1);
}

void CEMFToolBar::SCSetCurPage(int iPage)
{
	// In this control, the page index is 1-based
	ASSERT(iPage<=m_iNbPages && iPage>0);

	CString strText;
	strText.Format(_T("%d"), iPage);
	m_GotoPgBox.SetWindowText(strText);
}

void CEMFToolBar::SCEnableBoxes(BOOL bEnable/*=TRUE*/)
{
	m_GotoPgBox.EnableWindow(bEnable);
	m_ZoomBox.EnableWindow(bEnable);
	m_ColorBox.EnableWindow(bEnable);
}

void CEMFToolBar::SCReset()
{
	m_GotoPgBox.SCReset();
	m_ZoomBox.SCReset();
	m_ColorBox.SCReset();
}

int SCIntFromControl(CWnd* pCtl)
{
	ASSERT(pCtl);
	CString strText;
	pCtl->GetWindowText(strText);
	return _ttoi(LPCTSTR(strText));
}

/////////////////////////////////////////////////////////////////////////////
// CEMFToolBar message handlers

BEGIN_MESSAGE_MAP(CEMFToolBar, CToolBar)
	//{{AFX_MSG_MAP(CEMFToolBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CEMFToolBar::SCCreate(CWnd* pParentWnd)
{
	// self create
	if (!CreateEx(pParentWnd, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	// children
	if (!SCCreateChildren())
		return FALSE;

	SCEnableBoxes(FALSE);
	return TRUE;
}

BOOL CEMFToolBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (CToolBar::OnNotify(wParam, lParam, pResult))
		return TRUE;

	NMHDR* pNmhdr = (NMHDR*)lParam;
	switch (pNmhdr->idFrom)
	{
	case IDC_GOTOPAGE_BOX:
		if (EN_CHANGE==pNmhdr->code)
		{
			int iPage = SCIntFromControl(&m_GotoPgBox);
			int iPgNew = iPage;
			
			// In this control, the page index is 1-based
			if (iPage <= 0)
				iPage = 1;
			else
			if (iPage > m_iNbPages)
				iPage = m_iNbPages;
			if (iPgNew != iPage)
			{
				CString strText;
				strText.Format(_T("%d"), iPage);
				m_GotoPgBox.SetWindowText(strText);
			}
				
			CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
			if (pFrame)
			{
				CEMFexplorerView* pView = (CEMFexplorerView*)pFrame->GetActiveView();
				if (pView)
					pView->SCGotoPage(iPage, FALSE);
			}
			
			*pResult = 0;
			return TRUE;
		}
		break;

	case IDC_ZOOM_BOX:
		if (CBN_EDITCHANGE==pNmhdr->code)
		{
			int iZoom = (int)m_ZoomBox.SCGetCurValue();
			CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
			if (pFrame)
			{
				CEMFexplorerView* pView = (CEMFexplorerView*)pFrame->GetActiveView();
				if (pView)
					pView->SCSetZoom(iZoom, FALSE);
			}
			*pResult = 0;
			return TRUE;
		}
		break;

	case IDC_COLOR_BOX:
		if (CBN_EDITCHANGE==pNmhdr->code)
		{
			COLORREF Color = m_ColorBox.SCGetCurSelColor();
			CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
			if (pFrame)
			{
				CEMFexplorerView* pView = (CEMFexplorerView*)pFrame->GetActiveView();
				if (pView)
					pView->SCSetBkColor(Color);
			}
			*pResult = 0;
			return TRUE;
		}
		break;

	//default:
		// let Windows manage it
	}
	return FALSE;
}
