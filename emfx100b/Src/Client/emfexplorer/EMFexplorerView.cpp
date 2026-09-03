/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "EMFexplorer.h"

#include "EMFexplorerDoc.h"
#include "EMFexplorerView.h"
#include "MainFrm.h"

#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCPreviewView.h)
#include SC_INC_WINLIB(SCWinFile.h)
#include SC_INC_WINLIB(SCRegistry.h)
#include SC_INC_EMFLIB(SCEMFRasterizer.h) // for fonts clean up
#include SC_INC_SHARED(SCZoomShrd.h)
#include SC_INC_SHARED(SCPropStrLists.h)

#include "SCEMFInspectDlg.h"
#include "SCEMFDlgOptions.h"
#include "EMFexplorerReg.h"
#include "SCDocReflow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SC_TBUPDATE_PAGE	0x01
#define SC_TBUPDATE_ZOOM	0x02
#define SC_TBUPDATE_COLOR	0x04
#define SC_TBUPDATE_ALL		(SC_TBUPDATE_PAGE|SC_TBUPDATE_ZOOM|SC_TBUPDATE_COLOR)

extern CLIPFORMAT s_cfRTF;

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerView

IMPLEMENT_DYNCREATE(CEMFexplorerView, CView)

BEGIN_MESSAGE_MAP(CEMFexplorerView, CView)
	ON_MESSAGE(AM_USER_CHANGE_OPTIONS, OnChangeOptions)
	//{{AFX_MSG_MAP(CEMFexplorerView)
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_SAVEPAGE, OnFileSavePage)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEPAGE, OnUpdateFileSavePage)
	ON_COMMAND(ID_FILE_SAVEALL, OnFileSaveAll)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEALL, OnUpdateFileSaveAll)
	ON_UPDATE_COMMAND_UI(ID_TBNAV_FIRST, OnUpdateTbnavFirst)
	ON_COMMAND(ID_TBNAV_FIRST, OnTbnavFirst)
	ON_UPDATE_COMMAND_UI(ID_TBNAV_PREV, OnUpdateTbnavPrev)
	ON_COMMAND(ID_TBNAV_PREV, OnTbnavPrev)
	ON_UPDATE_COMMAND_UI(ID_TBNAV_NEXT, OnUpdateTbnavNext)
	ON_COMMAND(ID_TBNAV_NEXT, OnTbnavNext)
	ON_UPDATE_COMMAND_UI(ID_TBNAV_LAST, OnUpdateTbnavLast)
	ON_COMMAND(ID_TBNAV_LAST, OnTbnavLast)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_TB_DPI_DEC, OnUpdateTbDpiDec)
	ON_COMMAND(ID_TB_DPI_DEC, OnTbDpiDec)
	ON_UPDATE_COMMAND_UI(ID_TB_DPI_INC, OnUpdateTbDpiInc)
	ON_COMMAND(ID_TB_DPI_INC, OnTbDpiInc)
	ON_WM_ERASEBKGND()
	ON_UPDATE_COMMAND_UI(ID_TB_DPI_100, OnUpdateTbDpi100)
	ON_COMMAND(ID_TB_DPI_100, OnTbDpi100)
	ON_UPDATE_COMMAND_UI(ID_TB_DPI_MONITOR, OnUpdateTbDpiMonitor)
	ON_COMMAND(ID_TB_DPI_MONITOR, OnTbDpiMonitor)
	ON_UPDATE_COMMAND_UI(ID_TB_PAGE_ROTATELEFT, OnUpdateTbPageRotateleft)
	ON_COMMAND(ID_TB_PAGE_ROTATELEFT, OnTbPageRotateleft)
	ON_UPDATE_COMMAND_UI(ID_TB_PAGE_ROTATERIGHT, OnUpdateTbPageRotateright)
	ON_COMMAND(ID_TB_PAGE_ROTATERIGHT, OnTbPageRotateright)
	ON_UPDATE_COMMAND_UI(ID_TBDOC_BKCOLOR, OnUpdateTbdocBkcolor)
	ON_UPDATE_COMMAND_UI(ID_TB_PAGE_REVERSEVIDEO, OnUpdateTbPageReversevideo)
	ON_COMMAND(ID_TB_PAGE_REVERSEVIDEO, OnTbPageReversevideo)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EMF_ASTEXT, OnUpdateViewEmfAstext)
	ON_COMMAND(ID_VIEW_EMF_ASTEXT, OnViewEmfAstext)
	ON_UPDATE_COMMAND_UI(ID_TBDOC_USEGDIP, OnUpdateTbdocUseGDIp)
	ON_COMMAND(ID_TBDOC_USEGDIP, OnTbdocUseGDIp)
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_WM_DRAWCLIPBOARD()
	ON_WM_DESTROY()
	ON_WM_CHANGECBCHAIN()
	ON_COMMAND(ID_TBDOC_SETTINGS, OnTbdocSettings)
	ON_UPDATE_COMMAND_UI(ID_TBDOC_SETTINGS, OnUpdateTbdocSettings)
	ON_COMMAND(ID_TBDOC_REFLOW, OnTbdocReflow)
	ON_UPDATE_COMMAND_UI(ID_TBDOC_REFLOW, OnUpdateTbdocReflow)
	ON_COMMAND(ID_TOOLS_PAGE_URL, OnToolsPageUrl)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_PAGE_URL, OnUpdateToolsPageUrl)
	ON_COMMAND(ID_TBDOC_ADD_PAGES, OnTbdocAddPages)
	ON_UPDATE_COMMAND_UI(ID_TBDOC_ADD_PAGES, OnUpdateTbdocAddPages)
	ON_COMMAND(ID_TBDOC_REMOVE_PAGE, OnTbdocRemovePage)
	ON_UPDATE_COMMAND_UI(ID_TBDOC_REMOVE_PAGE, OnUpdateTbdocRemovePage)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerView construction/destruction

CEMFexplorerView::CEMFexplorerView():
	m_hPopupMenu(NULL),
	m_pEMFTxtDlg(NULL),
	m_iCurPage(0),
	m_bCanPaste(FALSE),
	m_hWndNext(NULL),
	m_pFrame(NULL),
	m_pEMFDoc(NULL),
	m_bCreditURL(FALSE)
{
}

CEMFexplorerView::~CEMFexplorerView()
{
	if (m_hPopupMenu)
		DestroyMenu(m_hPopupMenu);
	SCSaveLastSettings();
	if (m_pEMFTxtDlg)
	{
		m_pEMFTxtDlg->DestroyWindow();
		delete m_pEMFTxtDlg;
	}
	CSCEMFdcRenderer::SCCleanFontCopies();
}

BOOL CEMFexplorerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// Modify the Window class or styles here by modifying the CREATESTRUCT cs
	cs.style &= ~(WS_HSCROLL|WS_VSCROLL);
	return CView::PreCreateWindow(cs);
}

void CEMFexplorerView::SCResetView()
{
	m_CtlImage.SCReset();

	// App settings
	m_iCurPage = 0;
	m_bCreditURL = FALSE;
	CSCEMFdcRenderer::SCCleanFontCopies();
}

int CEMFexplorerView::SCGetNumPages()
{
	if (!m_pEMFDoc)
		return 0;
	return m_pEMFDoc->SCGetNbPages();
}


/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerView drawing

void CEMFexplorerView::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerView diagnostics

#ifdef _DEBUG
void CEMFexplorerView::AssertValid() const
{
	CView::AssertValid();
}

void CEMFexplorerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEMFexplorerDoc* CEMFexplorerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEMFexplorerDoc)));
	return (CEMFexplorerDoc*)m_pDocument;
}
#endif //_DEBUG

// Allows to get the view from anywhere, in a SDI app.
// Note: This technique is valid only for SDI applications that do not create multiple views
CEMFexplorerDoc* s_pSDI_Doc = NULL;	// the single doc
CEMFexplorerView* s_pSDI_View = NULL; // the single view

/*static*/
CEMFexplorerView* CEMFexplorerView::SCGetView()
{
	return s_pSDI_View;
}

/*static*/
void CEMFexplorerView::SCSetView(CEMFexplorerView* pView)
{
	ASSERT(pView);
	ASSERT(NULL==s_pSDI_View || pView==s_pSDI_View);
	if (!s_pSDI_View)
	if ( pView->IsKindOf( RUNTIME_CLASS(CEMFexplorerView) ) )
	{
		s_pSDI_View = pView;
		s_pSDI_Doc = pView->GetDocument();
	}
}

/*static*/
CEMFexplorerDoc* CEMFexplorerView::SCGetDoc()
{
	return s_pSDI_Doc;
}

/*static*/
SCZoomEntry* CEMFexplorerView::SCGetZoomList(USHORT& rusCount)
{
	rusCount = cs_usZoomCount;
	return (SCZoomEntry*)sc_ZoomList;
}

/*static*/
void CEMFexplorerView::SCGetZoomInfo(int& riFactor, int& riMin, int& riMax)
{
	riFactor = SC_FLOATFACTOR;
	riMin = SC_MIN_ZOOM;
	riMax = SC_MAX_ZOOM;
}

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerView message handlers
void CEMFexplorerView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if (m_pEMFTxtDlg)
	{
		m_pEMFTxtDlg->MoveWindow(0, 0 ,cx, cy);
		return;
	}

	// Add your message handler code here
	if (::IsWindow(m_CtlImage.m_hWnd))
		m_CtlImage.MoveWindow(0, 0, cx, cy);
}

void CEMFexplorerView::OnInitialUpdate() 
{
	SCResetView();
	CView::OnInitialUpdate();
	
	// Specialized code here
	if (m_pEMFDoc) // not first load
		SCSaveLastSettings();

	CEMFexplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	m_pEMFDoc = pDoc->GetEMFDoc();
	ASSERT(m_pEMFDoc);

	m_CtlImage.SCSetEMFDoc(m_pEMFDoc);

	// User settings
	if (!SCReadDocSettings())
	{
		CWinApp* pApp = AfxGetApp();
		switch (pApp->GetProfileInt(s_szAppSection, SC_EMFREG_ON_DOCLOAD,
			CSCEMFOptGenPane::SCE_DLSettingsDefault))
		{
		case CSCEMFOptGenPane::SCE_DLSettingsDefault:
			SCReadSettings();
			break;
			
		case CSCEMFOptGenPane::SCE_DLSettingsLast:
			SCReadSettings(_T("Last"));
			break;
			
		default:
			ASSERT(0);
		}
	}

	// Update Frame
	m_pFrame = (CMainFrame*)AfxGetMainWnd();
	ASSERT(m_pFrame);
	ASSERT_KINDOF(CMainFrame, m_pFrame);
	SCResetToolbars();

	// Show
	SCGotoPage(1);
}

void CEMFexplorerView::SCResetToolbars()
{
	ASSERT(m_pFrame);
	if (m_pFrame)
	{
		CEMFToolBar& rToolBar = m_pFrame->SCGetToolbar();
		
		rToolBar.SCReset();
		rToolBar.SCSetNbPages(SCGetNumPages());
		rToolBar.SCSetZoomMultiplier(SC_FLOATFACTOR);
		rToolBar.SCSetZoomLimits(SC_MIN_ZOOM, SC_MAX_ZOOM);
		rToolBar.SCSetZoomsList((SCZoomEntry*)sc_ZoomList, cs_usZoomCount);
		switch (m_CtlImage.SCGetFitMode())
		{
		default:
			ASSERT(0);
		case SC_FIT_NONE:
			rToolBar.SCSetCurZoom(m_CtlImage.SCGetCurZoom());
			break;

		case SC_FIT_WIDTH:
			rToolBar.SCSelectZoom(SC_ZOOM_FITWITDH);
			break;
			
		case SC_FIT_PAGE:
			rToolBar.SCSelectZoom(SC_ZOOM_FITPAGE);
			break;
		}
		rToolBar.SCSetCurColor(m_CtlImage.SCGetPaperColor());
		rToolBar.SCEnableBoxes(SCGetNumPages()>0);

		SCGDIpDrawingAttributes& rDrawingAttributes = m_CtlImage.SCGetDrawingAttributes();

		CSCGDIpToolBar& rGDIpToolBar = m_pFrame->SCGetGDIPToolbar();
		rGDIpToolBar.SCSetDrawingAttributes(&rDrawingAttributes);
		rGDIpToolBar.SCEnableControls(SCGetNumPages()>0);
		m_pFrame->SCEnableGDIPlus(m_CtlImage.SCGetGDIplusEnabled());
	}
}

void CEMFexplorerView::SCUpdateToolbar(DWORD dwFlags) 
{
	ASSERT(m_pFrame);
	if (m_pFrame)
	{
		CEMFToolBar& rToolBar = m_pFrame->SCGetToolbar();
		if (dwFlags & SC_TBUPDATE_PAGE)
			rToolBar.SCSetCurPage(m_iCurPage);
		if (dwFlags & SC_TBUPDATE_ZOOM)
		{
			switch (m_CtlImage.SCGetFitMode())
			{
			default:
				ASSERT(0);
			case SC_FIT_NONE:
				rToolBar.SCSetCurZoom(m_CtlImage.SCGetCurZoom());
				break;
				
			case SC_FIT_WIDTH:
				rToolBar.SCSelectZoom(SC_ZOOM_FITWITDH);
				break;
				
			case SC_FIT_PAGE:
				rToolBar.SCSelectZoom(SC_ZOOM_FITPAGE);
				break;
			}
		}
		if (dwFlags & SC_TBUPDATE_COLOR)
			rToolBar.SCSetCurColor(m_CtlImage.SCGetPaperColor());
	}
}

void CEMFexplorerView::SCGotoPage(int iPage, BOOL bUpdateToolbar/*=TRUE*/) 
{
	CEMFexplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!m_pEMFDoc || !m_pEMFDoc->SCGetNbPages())
		return;
	
	ASSERT(iPage>=1 && iPage<=(int)m_pEMFDoc->SCGetNbPages());
	m_iCurPage = iPage;
	if (bUpdateToolbar)
		SCUpdateToolbar(SC_TBUPDATE_PAGE);
	
	// load
	m_CtlImage.SCGotoPage(iPage);
	pDoc->SCUpdateTitle(m_iCurPage-1);

	// page context
	CString strURL = m_pEMFDoc->SCGetPageURL(m_iCurPage-1);
	m_bCreditURL = (!strURL.IsEmpty());
}

void CEMFexplorerView::SCRefresh()
{
	m_CtlImage.SCRefresh();
}

void CEMFexplorerView::SCSetZoom(int iZoom, BOOL bUpdateToolbar/*=TRUE*/)
{
	if (SC_ZOOM_FITWITDH==iZoom)
		m_CtlImage.SCSetFitMode(SC_FIT_WIDTH);
	else
	if (SC_ZOOM_FITPAGE==iZoom)
		m_CtlImage.SCSetFitMode(SC_FIT_PAGE);
	else
	{
		ASSERT(iZoom>=SC_MIN_ZOOM && iZoom<=SC_MAX_ZOOM);
		
		m_CtlImage.SCSetCurZoom(iZoom);
	}
	SCRefresh();
	if (bUpdateToolbar)
		SCUpdateToolbar(SC_TBUPDATE_ZOOM);
}

void CEMFexplorerView::SCSetBkColor(COLORREF crColor)
{
	// Note: dialog and toolbar return RGB. So we must update color style accordingly.
	m_CtlImage.SCSetPaperColorStyle(SC_COLOR_RGBVALUE, crColor);
	SCRefresh();	
}

int CEMFexplorerView::SCGetNearsetZoom(BOOL bGrow)
{
	int iZoom = m_CtlImage.SCGetCurZoom();
	if (bGrow)
	{
		for (int i=0; (i<cs_usZoomCount); i++)
		{
			if (sc_ZoomList[i].ze_iValue<0)
				break;
			if (sc_ZoomList[i].ze_iValue>iZoom)
				return sc_ZoomList[i].ze_iValue;
		}
	} else
	{
		for (int i=cs_usZoomCount-1; (i>=0); i--)
		{
			if (sc_ZoomList[i].ze_iValue<0)
				continue;
			if (sc_ZoomList[i].ze_iValue<iZoom)
				return sc_ZoomList[i].ze_iValue;
		}
	}
	return iZoom;
}

void CEMFexplorerView::SCOnAttributesChanged()
{
	SCRefresh();
}

//////////////////////////////////////////////////////////////////////////////////
// Page navigation events
//
void CEMFexplorerView::OnUpdateTbnavFirst(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_iCurPage>1);
}

void CEMFexplorerView::OnTbnavFirst() 
{
	SCGotoPage(1);
}

void CEMFexplorerView::OnUpdateTbnavPrev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_iCurPage>1);
}

void CEMFexplorerView::OnTbnavPrev() 
{
	if (m_iCurPage>1)
		SCGotoPage(m_iCurPage-1);
}

void CEMFexplorerView::OnUpdateTbnavNext(CCmdUI* pCmdUI) 
{
	int iNbPages = SCGetNumPages();
	pCmdUI->Enable(iNbPages>0 && m_iCurPage<iNbPages);
}

void CEMFexplorerView::OnTbnavNext() 
{
	if (m_iCurPage<SCGetNumPages())
		SCGotoPage(m_iCurPage+1);
}

void CEMFexplorerView::OnUpdateTbnavLast(CCmdUI* pCmdUI) 
{
	OnUpdateTbnavNext(pCmdUI);	
}

void CEMFexplorerView::OnTbnavLast() 
{
	SCGotoPage(SCGetNumPages());
}

//////////////////////////////////////////////////////////////////////////////////
// Creation/background events
//

int CEMFexplorerView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CEMFexplorerView::SCSetView(this);

	// create controls
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPCHILDREN;
	if (!m_CtlImage.Create(NULL, _T("EMFCtlImage"), 
		dwStyle, 
		CRect(10,10,150,50), this, IDC_WND_EMFIMG))
	{
		TRACE0("Could not create image control 1\n");
		return -1;
	}

	m_CtlImage.SCSetColorScheme(SC_DFLT_VIEWCSHM);
	m_CtlImage.SCSetCtlColor(SC_DFLT_CTLCOLOR);
	// Contextual menu
	m_CtlImage.SCSetCtlOwner(this);
	CMenu menu;
	if (menu.LoadMenu(IDR_CTX_MENU))
	{
		CMenu *pSubMenu = menu.GetSubMenu(MNU_IDX_TRACKPOPUP1);
		if (pSubMenu)
		{
			m_CtlImage.SCSetCtxMenu(pSubMenu->m_hMenu);
		}
		m_hPopupMenu = menu.Detach();
	}

	m_hWndNext = SetClipboardViewer();

	return 0;
}

BOOL CEMFexplorerView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE; // do not erase
	//return CView::OnEraseBkgnd(pDC);
}

//////////////////////////////////////////////////////////////////////////////////
// Zoom/DPI events
//
void CEMFexplorerView::OnUpdateTbDpiDec(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_CtlImage.SCGetCurZoom()>SC_MIN_ZOOM && SCGetNumPages()>0);
}

void CEMFexplorerView::OnTbDpiDec() 
{
	SCSetZoom(SCGetNearsetZoom(FALSE));
}

void CEMFexplorerView::OnUpdateTbDpiInc(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_CtlImage.SCGetCurZoom()<SC_MAX_ZOOM && SCGetNumPages()>0);
}

void CEMFexplorerView::OnTbDpiInc() 
{
	SCSetZoom(SCGetNearsetZoom(TRUE));
}

void CEMFexplorerView::OnUpdateTbDpi100(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SCGetNumPages()>0);
}

void CEMFexplorerView::OnTbDpi100() 
{
	SCSetZoom(SC_ZOOM100);
}

void CEMFexplorerView::OnUpdateTbDpiMonitor(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SCGetNumPages()>0);
}

void CEMFexplorerView::OnTbDpiMonitor() 
{
	CDC* pDC = this->GetDC();
	int iDpi = pDC->GetDeviceCaps(LOGPIXELSX);
	this->ReleaseDC(pDC);

	int iZoom = MulDiv(SC_ZOOM100, iDpi, 72); 
	SCSetZoom(iZoom);
}

//////////////////////////////////////////////////////////////////////////////////
// Rotation events
//
void CEMFexplorerView::OnUpdateTbPageRotateleft(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SCGetNumPages()>0);
}

void CEMFexplorerView::OnTbPageRotateleft() 
{
	m_CtlImage.SCRotateLeft();
}

void CEMFexplorerView::OnUpdateTbPageRotateright(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SCGetNumPages()>0);
}

void CEMFexplorerView::OnTbPageRotateright() 
{
	m_CtlImage.SCRotateRight();
}

//////////////////////////////////////////////////////////////////////////////////
// Color events
//

void CEMFexplorerView::OnUpdateTbdocBkcolor(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SCGetNumPages()>0);
}

void CEMFexplorerView::OnUpdateTbPageReversevideo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SCGetNumPages()>0);
	pCmdUI->SetCheck(m_CtlImage.SCGetReverseVideo()); // stay pressed if reverse video on
}

void CEMFexplorerView::OnTbPageReversevideo() 
{
	m_CtlImage.SCSetReverseVideo(!m_CtlImage.SCGetReverseVideo());
}

/////////////////////////////////////////////////////////////////////////////
// Other utilities
//

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerView printing

BOOL CEMFexplorerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	if (m_pEMFTxtDlg)
	{
		UINT uiPages = m_pEMFTxtDlg->SCCountPages(NULL, pInfo);
		pInfo->SetMaxPage(uiPages);
	} else
	{
		pInfo->SetMaxPage(SCGetNumPages());
		pInfo->m_nCurPage = m_iCurPage;
	}
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CEMFexplorerView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	if (m_pEMFTxtDlg)
	{
		UINT uiPages = m_pEMFTxtDlg->SCCountPages(pDC, pInfo);
		pInfo->SetMaxPage(uiPages);
		return;
	}
}

void CEMFexplorerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// cleanup after printing
	m_CtlImage.SCGetDrawingAttributes().fPageScale = 1;
}

void CEMFexplorerView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	CView::OnPrepareDC(pDC, pInfo);
}

void CEMFexplorerView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	if (m_pEMFTxtDlg)
	{
		m_pEMFTxtDlg->SCPrint(pDC, pInfo);
		return;
	}
	// Print header
	// TODO:
	//SCPrintHeader(pDC, rectPrintArea);

	// Print body
	m_CtlImage.SCPrintImage(pDC, pInfo, TRUE); // TODO: ask for fitting

	// Print footer
	// TODO:
	//SCPrintFooter(pDC, rectPrintArea);
}

// override MFC to supply our preview class, which handles scaling factor
void CEMFexplorerView::OnFilePrintPreview() 
{
	// Add your command handler code here
	// In derived classes, implement special window handling here
	// Be sure to Unhook Frame Window close if hooked.
	
	// must not create this on the frame.  Must outlive this function
	CPrintPreviewState* pState = new CPrintPreviewState;
	
	// DoPrintPreview's return value does not necessarily indicate that
	// Print preview succeeded or failed, but rather what actions are necessary
	// at this point.  If DoPrintPreview returns TRUE, it means that
	// OnEndPrintPreview will be (or has already been) called and the
	// pState structure will be/has been deleted.
	// If DoPrintPreview returns FALSE, it means that OnEndPrintPreview
	// WILL NOT be called and that cleanup, including deleting pState
	// must be done here.
	
	if (!DoPrintPreview(AFX_IDD_PREVIEW_TOOLBAR, this,
		RUNTIME_CLASS(CSCPreviewView), pState))
	{
		// In derived classes, reverse special window handling here for
		// Preview failure case
		
		TRACE0("Error: DoPrintPreview failed.\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		delete pState;      // preview failed to initialize, delete State now
	}
}

void CEMFexplorerView::OnUpdateViewEmfAstext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SCGetNumPages()>0);	
	pCmdUI->SetCheck((m_pEMFTxtDlg!=NULL)); // stay pressed if reverse video on
}

void CEMFexplorerView::OnViewEmfAstext() 
{
#if 0
	// if you activte this, don't forget to update the dialog template accordingly
	CSCEMFInspectDlg dlg(this);
	dlg.SCSetEMF(m_hEmf);
	dlg.DoModal();
#else
	if (m_pEMFTxtDlg)
	{
		m_CtlImage.ShowWindow(SW_SHOW);
		m_pEMFTxtDlg->ShowWindow(SW_HIDE);
		m_pEMFTxtDlg->DestroyWindow();
		delete m_pEMFTxtDlg;
		m_pEMFTxtDlg = NULL;
		return;
	}
	m_pEMFTxtDlg = new CSCEMFInspectDlg;
	if (!m_pEMFTxtDlg)
		return;

	if (!m_pEMFTxtDlg->Create(IDD_SCEMFINSPECT_DIALOG, this))
	{
		ASSERT(0);
		delete m_pEMFTxtDlg;
		m_pEMFTxtDlg = NULL;
		return;
	}

	CRect rc;
	GetClientRect(&rc);
	m_pEMFTxtDlg->MoveWindow(&rc);
	m_pEMFTxtDlg->ShowWindow(SW_SHOW);
	m_CtlImage.ShowWindow(SW_HIDE);

	// launch parser
	m_pEMFTxtDlg->SCSetEMF(m_CtlImage.SCGetCurEMF());
#endif
}

void CEMFexplorerView::OnUpdateTbdocUseGDIp(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SCGetNumPages()>0);
	pCmdUI->SetCheck((SC_ENGINE_GDIP==m_CtlImage.SCGetRasEngine())); // stay pressed if GDI+ on
}

void CEMFexplorerView::OnTbdocUseGDIp() 
{
	BOOL bEnable = (!m_CtlImage.SCGetGDIplusEnabled());

	ASSERT(m_pFrame);
	if (m_pFrame)
		m_pFrame->SCEnableGDIPlus(bEnable);

	m_CtlImage.SCEnableGDIp(bEnable);
	//SCRefresh();	
}

void CEMFexplorerView::OnToolsOptions() 
{
	CString strTitle;
	strTitle.LoadString(IDS_DLG_TITLE_OPTIONS);
	CSCEMFDlgOptions dlgOpt(strTitle, this, this);
	if (IDOK==dlgOpt.DoModal())
	{
		OnChangeOptions(WPARAM(dlgOpt.SCGetDocModified()), 0);
	}
}

void CEMFexplorerView::OnUpdateTbdocSettings(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((SCGetNumPages()>0));
}

void CEMFexplorerView::OnTbdocSettings() 
{
	CString strTitle;
	if (m_pEMFDoc->SCHasSettings())
		strTitle.LoadString(IDS_DLG_TITLE_DOCSETTINGS);
	else
		strTitle.LoadString(IDS_DLG_TITLE_UNSAVEDDOCSETTINGS);
	CSCEMFDlgOptions dlgOpt(strTitle, this, this, eSettingsDoc);
	if (IDOK==dlgOpt.DoModal())
	{
		OnChangeOptions(WPARAM(dlgOpt.SCGetDocModified()), 0);
	}
}

LRESULT CEMFexplorerView::OnChangeOptions(WPARAM wParam, LPARAM lParam)
{
	if ((BOOL)wParam) // doc modified
	{
		SCSaveDocSettings();
		ASSERT(m_pEMFDoc);
		if (m_pEMFDoc->SCGetDocModified())
		{
			CEMFexplorerDoc* pDoc = GetDocument();
			ASSERT_VALID(pDoc);
			pDoc->SetModifiedFlag();
		}
	}

	SCUpdateToolbar(SC_TBUPDATE_ZOOM|SC_TBUPDATE_COLOR);
	ASSERT(m_pFrame);
	if (m_pFrame)
	{
		m_pFrame->SCEnableGDIPlus(m_CtlImage.SCGetGDIplusEnabled());
		m_pFrame->UpdateWindow();
	}
	SCGotoPage(m_iCurPage, FALSE);
	return 0;
}

BOOL CEMFexplorerView::SCCanSave()
{
	return (m_pEMFTxtDlg) ? (!m_pEMFTxtDlg->SCIsCracking()) : TRUE;
}

void CEMFexplorerView::SCSavePages(LPCTSTR lpszPathname, int iType, BOOL bSaveAll/*=FALSE*/)
{
	if (m_pEMFTxtDlg)
	{// Save text
		m_pEMFTxtDlg->SCSaveDocument(lpszPathname, (SC_FILETYPE_RTF==iType));
		return;
	}

	// Or save EMF(s)
	BOOL bSaveAsIs = (AfxGetApp()->GetProfileInt(s_szAppSection, SC_EMFREG_ON_DOCSAVE,
		CSCEMFOptGenPane::SCE_DSSettingsWysiwyg)==
		CSCEMFOptGenPane::SCE_DSSettingsWysiwyg);

	m_CtlImage.SCSaveDocument(lpszPathname, iType, bSaveAll, bSaveAsIs);
}

///
/// Note: image must be rendered. So this saving task's best placed is in the view.
///
void CEMFexplorerView::SCFileSavePageAs(BOOL bSaveAll/*=FALSE*/) 
{
	BOOL bTxtMode = SCIsInTextMode();
	CString sFilters((bTxtMode) ? SC_BRKMODE_FILTERS : SC_EMFMODE_FILTERS_IMG);
	CFileDialog dlg(FALSE, NULL, ((bTxtMode) ? _T("*.rtf") : _T("*.emf")),
		OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT,
		(LPCTSTR)sFilters);
	if (dlg.DoModal() != IDOK)
		return;

	CString strPath = dlg.GetPathName();
	if (-1==strPath.ReverseFind(_T('.')))
	{
		int iPos = 0;
		for (UINT i=0; i<dlg.m_ofn.nFilterIndex; i++, iPos += 7)
		{
			iPos = sFilters.Find(_T(")|*."), iPos);
		}
		int iStart = iPos - 4;
		while (sFilters[iPos] != _T('|'))
			iPos++;
		
		strPath += sFilters.Mid(iStart, iPos-iStart);
	}

	int iType = SC_FTYPE_UKN;
	if (bTxtMode)
	{
		switch (dlg.m_ofn.nFilterIndex)
		{
		case 1:  iType = SC_FILETYPE_RTF; break;
		case 2:  iType = SC_FILETYPE_TXT; break;
		default:
			ASSERT(0);
			return;
		}
	} else
	{
		iType = SCFileTypeFromFilterIndexImg(dlg.m_ofn.nFilterIndex);
	}
	ASSERT(iType != SC_FTYPE_UKN);
	SCSavePages(LPCTSTR(strPath), iType, bSaveAll);
}


void CEMFexplorerView::OnFileSavePage() 
{
	SCFileSavePageAs();
}

void CEMFexplorerView::OnUpdateFileSavePage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SCGetNumPages()>0 && SCCanSave());
}

void CEMFexplorerView::OnFileSaveAll() 
{
	SCFileSavePageAs(TRUE);
}

void CEMFexplorerView::OnUpdateFileSaveAll(CCmdUI* pCmdUI) 
{
	OnUpdateFileSavePage(pCmdUI);	
}


///
/// In this version, copy the whole page (there is no selection).
///
void CEMFexplorerView::OnEditCopy() 
{
	if (m_pEMFTxtDlg)
	{// Copy text
		m_pEMFTxtDlg->SCCopy();
		return;
	}
	
	// Or copy EMF
	m_CtlImage.SCCopyToClipBoard();
}


void CEMFexplorerView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SCGetNumPages()>0);	
}

///
/// Paste the clipboard content as a full page
/// (no partial paste is provided in this version)
///
void CEMFexplorerView::OnEditPaste() 
{
	int iOldNbPages = SCGetNumPages();

	m_CtlImage.SCPasteFromClipBoard();

	int iNbPages = SCGetNumPages();
	if (iNbPages>iOldNbPages)
	{
		if (0==iOldNbPages)
		{
			SCResetToolbars();
		} else
		{
			ASSERT(m_pFrame);
			CEMFToolBar& rToolBar = m_pFrame->SCGetToolbar();
			rToolBar.SCSetNbPages(SCGetNumPages());
		}
		CEMFexplorerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->SetModifiedFlag();

		SCGotoPage(iNbPages);
	}
}

void CEMFexplorerView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bCanPaste);	
}

void CEMFexplorerView::OnDrawClipboard() 
{
	CView::OnDrawClipboard();
	
	// message handler code here
    m_bCanPaste = ((IsClipboardFormatAvailable(CF_ENHMETAFILE)) ||
				   (IsClipboardFormatAvailable(CF_BITMAP)) ||
				   (IsClipboardFormatAvailable(s_cfRTF)) ||
				   (IsClipboardFormatAvailable(CF_UNICODETEXT)) ||
				   (IsClipboardFormatAvailable(CF_TIFF))
				   );
}

void CEMFexplorerView::OnDestroy() 
{
	ChangeClipboardChain(m_hWndNext);

	CView::OnDestroy();
	
	// message handler code here
}

void CEMFexplorerView::OnChangeCbChain(HWND hWndRemove, HWND hWndAfter) 
{
	CView::OnChangeCbChain(hWndRemove, hWndAfter);
	
	// message handler code here
	::SendMessage(m_hWndNext, WM_CHANGECBCHAIN, (WPARAM)hWndRemove, (LPARAM)hWndAfter);
	if (m_hWndNext==hWndRemove)
		m_hWndNext = hWndAfter;
}


void CEMFexplorerView::OnTbdocReflow() 
{
	CSCDocReflow dlgReflow;

	ASSERT(m_pEMFDoc);
	dlgReflow.SCSetEMFDoc(m_pEMFDoc);

	if (IDOK==dlgReflow.DoModal())
	{// Save master
		int iNbPages = SCGetNumPages();

		ASSERT(m_pFrame);
		CEMFToolBar& rToolBar = m_pFrame->SCGetToolbar();
		rToolBar.SCSetNbPages(iNbPages);

		ASSERT(m_pEMFDoc);
		if (m_pEMFDoc->SCGetDocModified())
		{
			CEMFexplorerDoc* pDoc = GetDocument();
			ASSERT_VALID(pDoc);
			pDoc->SetModifiedFlag();
		}
		if (m_iCurPage>iNbPages)
			m_iCurPage=iNbPages;
		SCGotoPage(m_iCurPage);
	}
}

void CEMFexplorerView::OnUpdateTbdocReflow(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SCGetNumPages()>0);	
}

void CEMFexplorerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (m_iCurPage)
		SCRefresh();	
}


void CEMFexplorerView::OnToolsPageUrl() 
{
	if (m_pEMFDoc && m_iCurPage)
	{
		CString strURL = m_pEMFDoc->SCGetPageURL(m_iCurPage-1);
		ASSERT(!strURL.IsEmpty());
		if (!strURL.IsEmpty())
			ShellExecute( NULL, _T("open"), strURL,
			NULL, NULL, SW_SHOWNORMAL );
	}
}

void CEMFexplorerView::OnUpdateToolsPageUrl(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pEMFDoc && m_bCreditURL);
}

void CEMFexplorerView::SCInsertFiles(CStringList& rFilesList)
{
	ASSERT(m_pEMFDoc);

	// Filter out files in other directories
	CString strDocDir = SCMakeupDocDir(m_pEMFDoc->SCGetUniDocName());
	int iCount = rFilesList.GetCount();
	BOOL bSetDir = strDocDir.IsEmpty();

	SCFilterDocDirFileList(rFilesList, strDocDir, FALSE);

	int iNbFailure = iCount - rFilesList.GetCount();
	BOOL bDirMismatch = (iNbFailure>0);

	// Insert remaining files
	int iOldNbPages = SCGetNumPages();

	BOOL bInsertFailure = FALSE;
	POSITION pos = rFilesList.GetHeadPosition();
	while (pos)
	{
		if (!m_pEMFDoc->SCInsertFile(rFilesList.GetNext(pos)))
		{
			iNbFailure++;
			bInsertFailure = TRUE;
		}
	}
	strDocDir = SCMakeupDocDir(m_pEMFDoc->SCGetUniDocName());
	bSetDir = (bSetDir && !strDocDir.IsEmpty());

	if (iNbFailure)
	{
		CString strAdd;
		CString strMsg;
		if (1==iCount)
			strMsg.LoadString(IDS_FILE_INSERTONE_FAILED);
		else
			strMsg.LoadString(IDS_FILE_INSERTMANY_FAILED);
		if (bInsertFailure)
		{
			strAdd.LoadString(IDS_FILE_INSERTTYPE_ERR);
			strMsg += _T("\n") + strAdd;
		}
		if (bDirMismatch)
		{
			strAdd.LoadString(IDS_FILEMIX_ERR);
			strMsg += _T("\n") + strAdd;
		}
		AfxMessageBox(strMsg);
	}

	int iNbPages = SCGetNumPages();
	if (iNbPages>iOldNbPages)
	{
		if (bSetDir)
			m_pEMFDoc->SCSetDocDir(strDocDir);
		if (0==iOldNbPages)
		{
			SCResetToolbars();
		} else
		{
			ASSERT(m_pFrame);
			CEMFToolBar& rToolBar = m_pFrame->SCGetToolbar();
			rToolBar.SCSetNbPages(SCGetNumPages());
		}

		CEMFexplorerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->SetModifiedFlag();

		SCGotoPage(iNbPages);
	}
}

void CEMFexplorerView::OnTbdocAddPages() 
{
	ASSERT(m_pEMFDoc);
	if (!m_pEMFDoc)
		return;

	CStringList strLFiles;
	if (!SCGetFilesInsertList(strLFiles,
		(m_iCurPage>0)? m_pEMFDoc->SCGetPageFName(m_iCurPage - 1): (LPCTSTR)NULL))
		return;

	if (strLFiles.IsEmpty())
		AfxMessageBox(IDS_EMFXMERGE_ERR);
	else
		SCInsertFiles(strLFiles);
}

void CEMFexplorerView::OnUpdateTbdocAddPages(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((m_pEMFDoc!=NULL));	
	
}

void CEMFexplorerView::OnTbdocRemovePage() 
{
	ASSERT(m_pEMFDoc);
	if (!m_pEMFDoc)
		return;

	m_pEMFDoc->SCRemovePage(m_iCurPage-1);
	CEMFexplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->SetModifiedFlag();

	int iNbPages = SCGetNumPages();
	if (m_iCurPage>iNbPages)
		m_iCurPage--;
	ASSERT(m_iCurPage>=0 && m_iCurPage<=iNbPages);

	CEMFToolBar& rToolBar = m_pFrame->SCGetToolbar();
	rToolBar.SCSetNbPages(iNbPages);

	if (m_iCurPage)
		SCGotoPage(m_iCurPage);
	else
	{
		SCResetView();
		SCResetToolbars();
		pDoc->SCUpdateTitle(0);

		m_CtlImage.SCRefresh();
	}
}

void CEMFexplorerView::OnUpdateTbdocRemovePage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SCGetNumPages()>0);	
}

void CEMFexplorerView::OnEditCut() 
{
	OnTbdocRemovePage();	
}

void CEMFexplorerView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SCGetNumPages()>0);	
}
