/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#if !defined(AFX_SCEMFXGENPROPPAGE_H__C7D22B19_A10E_4A51_918F_B55C46337FB6__INCLUDED_)
#define AFX_SCEMFXGENPROPPAGE_H__C7D22B19_A10E_4A51_918F_B55C46337FB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SCEMFXGenPropPage.h : header file
//

#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCColorBox.h)
#include SC_INC_WINLIB(SCZoomBox.h)

#include "SCEMFXBasePropPage.h"

/////////////////////////////////////////////////////////////////////////////
// CSCEMFXGenPropPage dialog

class CSCEMFXGenPropPage : public CSCEMFXBasePropPage
{
	DECLARE_DYNCREATE(CSCEMFXGenPropPage)

// Construction
public:
	CSCEMFXGenPropPage();
	~CSCEMFXGenPropPage();

// Dialog Data
	//{{AFX_DATA(CSCEMFXGenPropPage)
	enum { IDD = IDD_PROPPAGE_GENERAL };
	CComboBox	m_CmbCtlBorder;
	CSCZoomBox	m_CmbZoomBox;
	CSCColorBox	m_CmbCtlColorRGB;
	CSCSysColorBox	m_CmbCtlColorSys;
	BOOL	m_bPageBorderVisible;
	BOOL	m_bPageShadowVisible;
	int		m_nPageOrientation;
	int		m_nMarginL;
	int		m_nMarginT;
	int		m_nMarginR;
	int		m_nMarginB;
	int		m_nCtlColorStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSCEMFXGenPropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SCEnableControls();

	// Generated message map functions
	//{{AFX_MSG(CSCEMFXGenPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioCtlColorTrans();
	afx_msg void OnRadioCtlColorSys();
	afx_msg void OnRadioCtlColorRgb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	COLORREF	m_crCtlColor;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCEMFXGENPROPPAGE_H__C7D22B19_A10E_4A51_918F_B55C46337FB6__INCLUDED_)
