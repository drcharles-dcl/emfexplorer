/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#if !defined(AFX_SCEMFAXGDIPPROPPAGE_H__DC740512_E290_4E0A_AC7B_9512C3889CF8__INCLUDED_)
#define AFX_SCEMFAXGDIPPROPPAGE_H__DC740512_E290_4E0A_AC7B_9512C3889CF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SCEMFAxGDIpPropPage.h : header file
//
#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCWinGUI.h)

/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxGDIpPropPage : Property page dialog

class CSCEMFAxGDIpPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CSCEMFAxGDIpPropPage)
	DECLARE_OLECREATE_EX(CSCEMFAxGDIpPropPage)

// Constructors
public:
	CSCEMFAxGDIpPropPage();

// Dialog Data
	//{{AFX_DATA(CSCEMFAxGDIpPropPage)
	enum { IDD = IDD_PROPPAGE_GDIPLUS };
	BOOL	m_bGDIplusEnabled;
	int		m_nTextRenderingHint;
	int		m_nTextContrast;
	int		m_nSmoothingMode;
	int		m_nInterpolationMode;
	int		m_nPixelOffsetMode;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
	void SCUpdateControls();
	void SCEnableControls(BOOL bEnable=TRUE);

// Message maps
protected:
	//{{AFX_MSG(CSCEMFAxGDIpPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkOptgenUsegdip();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	SC_DECLARE_ENABLE_CONTROL()

private:
	BOOL m_bGDIpOn; // dup of m_bGDIplusEnabled to avoid premature validation
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCEMFAXGDIPPROPPAGE_H__DC740512_E290_4E0A_AC7B_9512C3889CF8__INCLUDED_)
