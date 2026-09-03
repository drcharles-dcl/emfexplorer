/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "EMFexplorer.h"

#include "IpFrame.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame

IMPLEMENT_DYNCREATE(CInPlaceFrame, COleDocIPFrameWnd)

BEGIN_MESSAGE_MAP(CInPlaceFrame, COleDocIPFrameWnd)
	//{{AFX_MSG_MAP(CInPlaceFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, COleDocIPFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, COleDocIPFrameWnd::OnHelp)
	ON_COMMAND(ID_DEFAULT_HELP, COleDocIPFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_CONTEXT_HELP, COleDocIPFrameWnd::OnContextHelp)
END_MESSAGE_MAP()

// Normally, the container's status bar is used
#ifdef SC_USE_IPSTATUSBAR
static UINT IP_indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_PAGE,		// curent page indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};
#endif

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame construction/destruction

CInPlaceFrame::CInPlaceFrame():
	m_pOrigFrame(NULL),
	m_pWndFrame(NULL)
{
}

CInPlaceFrame::~CInPlaceFrame()
{
}

int CInPlaceFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleDocIPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// CResizeBar implements in-place resizing.
	if (!m_wndResizeBar.Create(this))
	{
		TRACE0("Failed to create resize bar\n");
		return -1;      // fail to create
	}

	// By default, it is a good idea to register a drop-target that does
	//  nothing with your frame window.  This prevents drops from
	//  "falling through" to a container that supports drag-drop.
	m_dropTarget.Register(this);

	return 0;
}

// OnCreateControlBars is called by the framework to create control bars on the
//  container application's windows.  pWndFrame is the top level frame window of
//  the container and is always non-NULL.  pWndDoc is the doc level frame window
//  and will be NULL when the container is an SDI application.  A server
//  application can place MFC control bars on either window.
BOOL CInPlaceFrame::OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc)
{
	// Remove this if you use pWndDoc
	UNREFERENCED_PARAMETER(pWndDoc);

	// Store the top level frame
	m_pWndFrame = pWndFrame;

	// Set owner to this window, so messages are delivered to correct app
	m_wndToolBar.SetOwner(this);

	// Create toolbar on client's frame window
	if (!m_wndToolBar.SCCreate(pWndFrame))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}

#ifdef SC_USE_IPSTATUSBAR
	// Create statusbar on client's frame window
	m_wndStatusBar.SetOwner(this);
	if (!m_wndStatusBar.Create(pWndFrame) ||
		!m_wndStatusBar.SetIndicators(IP_indicators,
		  sizeof(IP_indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	// Extend status bar style (to get a recessed status pane)
	{
		UINT nID, nStyle;
		int cxWidth;
		
		m_wndStatusBar.GetPaneInfo( 0, nID, nStyle, cxWidth);
		m_wndStatusBar.SetPaneInfo( 0, nID, SBPS_STRETCH|SBPS_NORMAL, cxWidth);
	}
#endif

	// The GDI+ control bar
	m_wndGDIpToolbar.SetOwner(this);
	if (!m_wndGDIpToolbar.SCCreate(pWndFrame, IDR_TB_GDIPLUS))
	{
		TRACE0("Failed to create GDI+ toolbar\n");
		return -1;      // fail to create
	}

	// Activate toolbar docking
	m_wndGDIpToolbar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	pWndFrame->EnableDocking(CBRS_ALIGN_ANY);
	pWndFrame->DockControlBar(&m_wndToolBar);
	pWndFrame->DockControlBar(&m_wndGDIpToolbar, AFX_IDW_DOCKBAR_BOTTOM);

	pWndFrame->ShowControlBar(&m_wndGDIpToolbar, FALSE, FALSE);

	// At this point, m_pOrigFrame should be (CMainFrame*)AfxGetMainWnd();
	ASSERT(m_pOrigFrame==(CMainFrame*)AfxGetMainWnd());
	CMainFrame* pFrame = (CMainFrame*)m_pOrigFrame;
	if (pFrame && pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)))
	{
		pFrame->SCOnIPControlBarsCreated();
	}
	return TRUE;
}

BOOL CInPlaceFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	//	Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return COleDocIPFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame diagnostics

#ifdef _DEBUG
void CInPlaceFrame::AssertValid() const
{
	COleDocIPFrameWnd::AssertValid();
}

void CInPlaceFrame::Dump(CDumpContext& dc) const
{
	COleDocIPFrameWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame commands

void CInPlaceFrame::SCOnViewGdipBar() 
{
	m_bShowGDIpToolbar = !m_bShowGDIpToolbar;
	if (!m_pWndFrame)
		return;

	m_pWndFrame->ShowControlBar(&m_wndGDIpToolbar, m_bShowGDIpToolbar, FALSE);
}

void CInPlaceFrame::SCEnableGDIPlus(BOOL bEnable)
{
	m_bGDIpEnabled = bEnable;
	if (!m_pWndFrame)
		return;

	m_pWndFrame->ShowControlBar(&m_wndGDIpToolbar, (bEnable && m_bShowGDIpToolbar), FALSE);
}
