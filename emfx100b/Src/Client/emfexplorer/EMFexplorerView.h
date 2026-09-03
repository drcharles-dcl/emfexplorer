/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#if !defined(AFX_EMFEXPLORERVIEW_H__36403A17_6BC9_4BA5_AFF2_629345A8C438__INCLUDED_)
#define AFX_EMFEXPLORERVIEW_H__36403A17_6BC9_4BA5_AFF2_629345A8C438__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SCEMFXBasePropPage.h"
#include "SCGenInclude.h"
#include SC_INC_EMFLIB(SCGdiplusUtils.h)
#include SC_INC_EMFLIB(SCEMFViewDefs.h)
#include SC_INC_EMFLIB(SCEMFImage.h)
#include SC_INC_WINLIB(SCZoomBox.h)
#include SC_INC_EMFLIB(SCEMFDoc.h)

// Application messages
#define AM_USER_CHANGE_OPTIONS	(WM_USER+1)

// Controls
#define IDC_WND_EMFIMG        100

// some foward declarations
class CEMFexplorerDoc;
class CSCEMFInspectDlg;
class CMainFrame;

class CEMFexplorerView : public CView, public I_SCPropExchange, public I_EMFImageOwner
{
protected: // create from serialization only
	CEMFexplorerView();
	DECLARE_DYNCREATE(CEMFexplorerView)

// Attributes
public:
	CEMFexplorerDoc* GetDocument();
	int SCGetNumPages();
	int SCGetCurPage() const { return m_iCurPage; }
	BOOL SCIsInTextMode() { return (m_pEMFTxtDlg!=NULL); }
	BOOL SCCanSave();

// Operations
public:
	void SCGotoPage(int iPage, BOOL bUpdateToolbar=TRUE);
	void SCSetZoom(int iZoom, BOOL bUpdateToolbar=TRUE);
	void SCSetBkColor(COLORREF crColor);
	void SCOnAttributesChanged();
	void SCSavePages(LPCTSTR lpszPathname, int iType, BOOL bSaveAll=FALSE);
	void SCRefresh();
	void SCResetToolbars();
	void SCInsertFiles(CStringList& rFilesList);

	static CEMFexplorerView* SCGetView();
	static void SCSetView(CEMFexplorerView* pView);
	static CEMFexplorerDoc* SCGetDoc();
	static SCZoomEntry* SCGetZoomList(USHORT& rusCount);
	static void SCGetZoomInfo(int& riFactor, int& riMin, int& riMax);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEMFexplorerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEMFexplorerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	/////////////////////////////////////////////////////////////////////////////
	// I_SCPropExchange
	// Filter
	BOOL SCWantSettings(int iSender);
	// Getters
	DWORD SCGetSettings_CtlBorder(int iSender);
	void SCGetSettings_CtlColor(int iSender, int& iStyle, COLORREF& crColor);
	
	int SCGetSettings_Scale(int iSender);
	void SCGetSettings_Margins(int iSender, RECT& rcLTRB);
	void SCGetSettings_PageBorder(int iSender, BOOL& bBorder, BOOL& bShadow);
	DWORD SCGetSettings_PageOrientation(int iSender);
	void SCGetSettings_PaperColor(int iSender, int& iStyle, COLORREF& crColor);
	DWORD SCGetSettings_TransparencyMode(int iSender);
	DWORD SCGetSettings_ReverseVideoMode(int iSender);
	
	DWORD SCGetSettings_GDIplusEnabled(int iSender);
	void SCGetSettings_TextRendering(int iSender, int& iHint, int& iContrast);
	void SCGetSettings_Smoothing(int iSender,
		int& iLineSmoothing,
		int& iImgInterpolation,
		int& iImgPixelOffset);
	
	// Setters
	void SCPutSettings_CtlBorder(int iSender, int iBorder);
	void SCPutSettings_CtlColor(int iSender, int iStyle, COLORREF crColor);
	
	void SCPutSettings_Scale(int iSender, int iScale);
	void SCPutSettings_Margins(int iSender, RECT& rcLTRB);
	void SCPutSettings_PageBorder(int iSender, BOOL bBorder, BOOL bShadow);
	void SCPutSettings_PageOrientation(int iSender, int iOrientation);
	void SCPutSettings_PaperColor(int iSender, int iStyle, COLORREF crColor);
	void SCPutSettings_TransparencyMode(int iSender, int iMode);
	void SCPutSettings_ReverseVideoMode(int iSender, int iMode);
	
	void SCPutSettings_GDIplusEnabled(int iSender, BOOL bEnabled);
	void SCPutSettings_TextRendering(int iSender, int iHint, int iContrast);
	void SCPutSettings_Smoothing(int iSender, int iLineSmoothing,
		int iImgInterpolation, int iImgPixelOffset);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	// I_EMFImageOwner
	void SCSetupCtxMenu(CMenu* pMenu) { /* fully managed by frame/view */ } 
	CWnd* SCGetMenuOwner() { return (CWnd*)m_pFrame; }
	void SCOnCtxCommand(WPARAM wParam, LPARAM lParam) { ASSERT(0); /* not reached*/ }
	/////////////////////////////////////////////////////////////////////////////

protected:
	void SCResetView();
	void SCUpdateToolbar(DWORD dwFlags); 
	int SCGetNearsetZoom(BOOL bGrow);

	BOOL SCRasterizeEMF(HDC hDC, HENHMETAFILE hEMF, CRect& rcPlay, BOOL bPrinting=FALSE);
	void SCReadSettings(LPCTSTR lpszSubpath=NULL);
	BOOL SCReadDocSettings();
	void SCSaveLastSettings();
	void SCSaveDocSettings();
	void SCFileSavePageAs(BOOL bSaveAll=FALSE);

// Generated message map functions
protected:
	//{{AFX_MSG(CEMFexplorerView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileSavePage();
	afx_msg void OnUpdateFileSavePage(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAll();
	afx_msg void OnUpdateFileSaveAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTbnavFirst(CCmdUI* pCmdUI);
	afx_msg void OnTbnavFirst();
	afx_msg void OnUpdateTbnavPrev(CCmdUI* pCmdUI);
	afx_msg void OnTbnavPrev();
	afx_msg void OnUpdateTbnavNext(CCmdUI* pCmdUI);
	afx_msg void OnTbnavNext();
	afx_msg void OnUpdateTbnavLast(CCmdUI* pCmdUI);
	afx_msg void OnTbnavLast();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateTbDpiDec(CCmdUI* pCmdUI);
	afx_msg void OnTbDpiDec();
	afx_msg void OnUpdateTbDpiInc(CCmdUI* pCmdUI);
	afx_msg void OnTbDpiInc();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnUpdateTbDpi100(CCmdUI* pCmdUI);
	afx_msg void OnTbDpi100();
	afx_msg void OnUpdateTbDpiMonitor(CCmdUI* pCmdUI);
	afx_msg void OnTbDpiMonitor();
	afx_msg void OnUpdateTbPageRotateleft(CCmdUI* pCmdUI);
	afx_msg void OnTbPageRotateleft();
	afx_msg void OnUpdateTbPageRotateright(CCmdUI* pCmdUI);
	afx_msg void OnTbPageRotateright();
	afx_msg void OnUpdateTbdocBkcolor(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTbPageReversevideo(CCmdUI* pCmdUI);
	afx_msg void OnTbPageReversevideo();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateViewEmfAstext(CCmdUI* pCmdUI);
	afx_msg void OnViewEmfAstext();
	afx_msg void OnUpdateTbdocUseGDIp(CCmdUI* pCmdUI);
	afx_msg void OnTbdocUseGDIp();
	afx_msg void OnToolsOptions();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnDrawClipboard();
	afx_msg void OnDestroy();
	afx_msg void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
	afx_msg void OnTbdocSettings();
	afx_msg void OnUpdateTbdocSettings(CCmdUI* pCmdUI);
	afx_msg void OnTbdocReflow();
	afx_msg void OnUpdateTbdocReflow(CCmdUI* pCmdUI);
	afx_msg void OnToolsPageUrl();
	afx_msg void OnUpdateToolsPageUrl(CCmdUI* pCmdUI);
	afx_msg void OnTbdocAddPages();
	afx_msg void OnUpdateTbdocAddPages(CCmdUI* pCmdUI);
	afx_msg void OnTbdocRemovePage();
	afx_msg void OnUpdateTbdocRemovePage(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg LRESULT OnChangeOptions(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	HMENU				m_hPopupMenu;	 // Contextual menu
	CSCEMFImage			m_CtlImage;		 // Control to display EMF 
	CSCEMFInspectDlg*	m_pEMFTxtDlg;	 // Control to display EMF as text
	int					m_iCurPage;		 // Current page (redundant with m_CtlImage)
	SCEMFDoc*			m_pEMFDoc;		 // The EMF sub doc 

	BOOL				m_bCanPaste;	 // enables/disables adding pages
	HWND				m_hWndNext;		 // clipboard chain window
	CMainFrame*			m_pFrame;		 // mainframe (convenience)

	BOOL				m_bCreditURL;	 // Current page has a credit URL attached
};

#ifndef _DEBUG  // debug version in EMFexplorerView.cpp
inline CEMFexplorerDoc* CEMFexplorerView::GetDocument()
   { return (CEMFexplorerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMFEXPLORERVIEW_H__36403A17_6BC9_4BA5_AFF2_629345A8C438__INCLUDED_)
