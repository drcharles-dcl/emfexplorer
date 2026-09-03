/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#if !defined(AFX_MAINFRM_H__0480C91D_A219_4861_BDA3_BB24F0DCD73C__INCLUDED_)
#define AFX_MAINFRM_H__0480C91D_A219_4861_BDA3_BB24F0DCD73C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EMFtoolbar.h"
#include "SCGDIpToolbar.h"
#include "IpFrame.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CEMFToolBar& SCGetToolbar() { return (!m_pIPFrame) ? m_wndToolBar : m_pIPFrame->SCGetToolbar(); }
	CSCGDIpToolBar&  SCGetGDIPToolbar() { return (!m_pIPFrame) ? m_wndGDIpToolbar : m_pIPFrame->SCGetGDIPToolbar(); }
	void SCSetOleIPFrameWnd(CInPlaceFrame* pOleIPFrameWnd) { m_pIPFrame = pOleIPFrameWnd; }

// Operations
public:
	void SCEnableGDIPlus(BOOL bEnable);
	void SCInitialShow();
	void SCOnIPControlBarsCreated();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar		m_wndStatusBar;
	CEMFToolBar		m_wndToolBar;
	CSCGDIpToolBar	m_wndGDIpToolbar;
	BOOL			m_bGDIpEnabled;
	BOOL			m_bShowGDIpToolbar;
	CInPlaceFrame*  m_pIPFrame;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnUpdatePage(CCmdUI *pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnUpdateViewGdipBar(CCmdUI* pCmdUI);
	afx_msg void OnViewGdipBar();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void SCFrameOpenFile(LPCTSTR szFilename);

private:
	BOOL m_bIsStandAlone;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__0480C91D_A219_4861_BDA3_BB24F0DCD73C__INCLUDED_)
