/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#if !defined(AFX_SCEMFAXPPG_H__6F20202A_2ED4_40E7_8929_123CBEA9D4C1__INCLUDED_)
#define AFX_SCEMFAXPPG_H__6F20202A_2ED4_40E7_8929_123CBEA9D4C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SCEMFAxPpg.h : Declaration of the CSCEMFAxPropPage property page class.
#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCOleBaseOlePPg.h)
#include SC_INC_WINLIB(SCColorBox.h)
#include SC_INC_WINLIB(SCZoomBox.h)
#include SC_INC_WINLIB(SCWinGUI.h)


////////////////////////////////////////////////////////////////////////////
// CSCEMFAxPropPage : See SCEMFAxPpg.cpp.cpp for implementation.

class CSCEMFAxPropPage : public CSCBaseOlePropertyPage
{
	DECLARE_DYNCREATE(CSCEMFAxPropPage)
	DECLARE_OLECREATE_EX(CSCEMFAxPropPage)

// Constructor
public:
	CSCEMFAxPropPage();

// Dialog Data
	//{{AFX_DATA(CSCEMFAxPropPage)
	enum { IDD = IDD_PROPPAGE_SCEMFAX };
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

//////////////////////////////////////////////////////////////////////////////////
// Note: this AFX_ block was not added by ClassWizard, I added it manually.
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCEMFAxPropPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
/////////////////////////////////////////////////////////////////////////////////

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	SC_DECLARE_ENABLE_CONTROL()
	void SCEnableControls();

// Message maps
protected:
	//{{AFX_MSG(CSCEMFAxPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioCtlColorTrans();
	afx_msg void OnRadioCtlColorSys();
	afx_msg void OnRadioCtlColorRgb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	OLE_COLOR	m_nCtlColor;
	float		m_fScale;
	DWORD		m_nFitMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCEMFAXPPG_H__6F20202A_2ED4_40E7_8929_123CBEA9D4C1__INCLUDED)
