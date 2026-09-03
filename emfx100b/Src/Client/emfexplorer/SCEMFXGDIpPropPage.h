/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#if !defined(AFX_SCEMFXGDIPPROPPAGE_H__B63B74DB_835D_43BD_B005_BF4355744FAF__INCLUDED_)
#define AFX_SCEMFXGDIPPROPPAGE_H__B63B74DB_835D_43BD_B005_BF4355744FAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SCEMFXGDIpPropPage.h : header file
//
#include "SCEMFXBasePropPage.h"
#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCWinGUI.h)

/////////////////////////////////////////////////////////////////////////////
// CSCEMFXGDIpPropPage dialog

class CSCEMFXGDIpPropPage : public CSCEMFXBasePropPage
{
	DECLARE_DYNCREATE(CSCEMFXGDIpPropPage)

// Construction
public:
	CSCEMFXGDIpPropPage();
	~CSCEMFXGDIpPropPage();

// Dialog Data
	//{{AFX_DATA(CSCEMFXGDIpPropPage)
	enum { IDD = IDD_PROPPAGE_GDIPLUS };
	BOOL	m_bGDIplusEnabled;
	int		m_nTextRenderingHint;
	int		m_nTextContrast;
	int		m_nSmoothingMode;
	int		m_nInterpolationMode;
	int		m_nPixelOffsetMode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSCEMFXGDIpPropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
	virtual void OnOK();
	//}}AFX_VIRTUAL

protected:
	void SCUpdateControls();
	void SCEnableControls(BOOL bEnable=TRUE);

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSCEMFXGDIpPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkOptgenUsegdip();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bGDIpOn; // dup of m_bGDIplusEnabled to avoid premature validation
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCEMFXGDIPPROPPAGE_H__B63B74DB_835D_43BD_B005_BF4355744FAF__INCLUDED_)
