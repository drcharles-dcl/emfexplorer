/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#if !defined(AFX_IPFRAME_H__2DD2C527_33F4_4D22_93A9_0048B6F673D2__INCLUDED_)
#define AFX_IPFRAME_H__2DD2C527_33F4_4D22_93A9_0048B6F673D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EMFtoolbar.h"
#include "SCGDIpToolbar.h"

class CInPlaceFrame : public COleDocIPFrameWnd
{
	DECLARE_DYNCREATE(CInPlaceFrame)
public:
	CInPlaceFrame();

// Attributes
public:
	CEMFToolBar& SCGetToolbar() { return m_wndToolBar; }
	CSCGDIpToolBar&  SCGetGDIPToolbar() { return m_wndGDIpToolbar; }
	void SCSetOrigFrame(CFrameWnd* pOrigFrame) { m_pOrigFrame = pOrigFrame; }

// Operations
public:
	void SCEnableGDIPlus(BOOL bEnable);
	void SCOnViewGdipBar();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceFrame)
	public:
	virtual BOOL OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
#ifdef SC_USE_IPSTATUSBAR
	CStatusBar		m_wndStatusBar;
#endif
	CEMFToolBar		m_wndToolBar;
	CSCGDIpToolBar	m_wndGDIpToolbar;
	BOOL			m_bGDIpEnabled;
	BOOL			m_bShowGDIpToolbar;

	COleDropTarget	m_dropTarget;
	COleResizeBar   m_wndResizeBar;

	CFrameWnd*		m_pOrigFrame;
	CFrameWnd*		m_pWndFrame;

// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPFRAME_H__2DD2C527_33F4_4D22_93A9_0048B6F673D2__INCLUDED_)
