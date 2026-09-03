/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "EMFexplorer.h"

#include "MainFrm.h"
#include "EMFexplorerView.h"
#include "EMFexplorerDoc.h"
#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCWinGUI.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_PAGE, OnUpdatePage)
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_VIEW_GDIP_BAR, OnUpdateViewGdipBar)
	ON_COMMAND(ID_VIEW_GDIP_BAR, OnViewGdipBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_PAGE,		// curent page indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

#define SCWIN_EMFEXPLORER _T("WindowPos")

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame():
	m_bGDIpEnabled(FALSE),
	m_bShowGDIpToolbar(TRUE),
	m_bIsStandAlone(FALSE),
	m_pIPFrame(NULL)
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.SCCreate(this))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
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
	if (!m_wndGDIpToolbar.SCCreate(this, IDR_TB_GDIPLUS))
	{
		TRACE0("Failed to create GDI+ toolbar\n");
		return -1;      // fail to create
	}

	// Activate toolbar docking
	m_wndGDIpToolbar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndGDIpToolbar, AFX_IDW_DOCKBAR_BOTTOM);

	ShowControlBar(&m_wndGDIpToolbar, FALSE, FALSE);

	return 0;
}

void CMainFrame::SCInitialShow()
{
	m_bIsStandAlone = TRUE;
	SCInitialShowWindow(m_hWnd, SW_SHOW, SCWIN_EMFEXPLORER);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// Modify the Window class or styles here by modifying
	// the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::SCFrameOpenFile(LPCTSTR szFilename)
{
	if (AfxGetApp())
	{
		AfxGetApp()->OpenDocumentFile(szFilename);
	}
}

void CMainFrame::SCOnIPControlBarsCreated()
{
	CEMFexplorerView* pView = (CEMFexplorerView*)GetActiveView();
	if (pView && pView==CEMFexplorerView::SCGetView())
	{
		pView->SCResetToolbars();
	}
}

void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	CEMFexplorerDoc* pDoc = (CEMFexplorerDoc*)GetActiveDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	SCEMFDoc* pEMFDoc = pDoc->GetEMFDoc();
	if (!pEMFDoc)
		return;

	// Open the first native or Build an insert list
    WORD    wNumFiles = DragQueryFile(hDropInfo, (UINT)-1, NULL, (UINT)0 ); 
    WORD    nFile, nPathLength; 
    LPTSTR  lpszFile = NULL;
	CString strNativeDoc;
	CStringList strLFiles;
    for ( nFile = 0; (nFile < wNumFiles); nFile++ ) 
    { 
		nPathLength = DragQueryFile(hDropInfo, nFile, NULL, 0 ) + 1;
		lpszFile = (LPTSTR)new TCHAR[nPathLength];
		if(lpszFile == NULL) 
			break; // out of memory

		DragQueryFile(hDropInfo, nFile, lpszFile, nPathLength ); 
		if (SCEMFDoc::SCIsNativeDoc(lpszFile))
		{// open native and ingore the rest
			strNativeDoc = lpszFile;
			delete [] lpszFile;
			break;
		}

		strLFiles.AddTail(lpszFile);
		delete [] lpszFile;
	}
	DragFinish(hDropInfo);

	// Open
	if (!strNativeDoc.IsEmpty())
		AfxGetApp()->OpenDocumentFile(strNativeDoc);
	else
	{// Or Insert
		CEMFexplorerView* pView = (CEMFexplorerView*)GetActiveView();
		if (pView && pView==CEMFexplorerView::SCGetView())
		{
			pView->SCInsertFiles(strLFiles);
		}
	}
}

//
// Beware that the this function is called only when the application enters idle state
// (but ALSO each time it enters idle state => preformance issue).
//
void CMainFrame::OnUpdatePage(CCmdUI *pCmdUI)
{
	int iNbPages = 0;
	CEMFexplorerView* pView = (CEMFexplorerView*)GetActiveView();
	if (pView && pView==CEMFexplorerView::SCGetView())
	{
		iNbPages = pView->SCGetNumPages();

		CString strText;
		strText.FormatMessage(ID_INDICATOR_PAGE, pView->SCGetCurPage(), iNbPages);
		
		// Now set the text of the pane.
		m_wndStatusBar.SetPaneText(
			m_wndStatusBar.CommandToIndex(ID_INDICATOR_PAGE),
			LPCTSTR(strText));
	}
	pCmdUI->Enable(iNbPages>0);
}


void CMainFrame::OnClose() 
{
	if (m_bIsStandAlone)
		SCSaveWindowPlacement(m_hWnd, SCWIN_EMFEXPLORER);
	
	CFrameWnd::OnClose();
}

void CMainFrame::OnUpdateViewGdipBar(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bGDIpEnabled);
	pCmdUI->SetCheck((m_wndGDIpToolbar.GetStyle() & WS_VISIBLE) != 0);
}

void CMainFrame::OnViewGdipBar() 
{
	if (m_pIPFrame)
		m_pIPFrame->SCOnViewGdipBar();
	else
	{
		m_bShowGDIpToolbar = !m_bShowGDIpToolbar;
		ShowControlBar(&m_wndGDIpToolbar, m_bShowGDIpToolbar, FALSE);
	}
}

void CMainFrame::SCEnableGDIPlus(BOOL bEnable)
{
	if (m_pIPFrame)
		m_pIPFrame->SCEnableGDIPlus(bEnable);
	else
	{
		m_bGDIpEnabled = bEnable;
		ShowControlBar(&m_wndGDIpToolbar, (bEnable && m_bShowGDIpToolbar), FALSE);
	}
}
