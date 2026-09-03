/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#if !defined(AFX_SCEMFAXCOLORSPROPPAGE_H__5A34E1EB_517A_4881_9C9F_6E30C443B85B__INCLUDED_)
#define AFX_SCEMFAXCOLORSPROPPAGE_H__5A34E1EB_517A_4881_9C9F_6E30C443B85B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SCEMFAxColorsPropPage.h : header file
//
#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCOleBaseOlePPg.h)

#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCColorBox.h)
#include SC_INC_WINLIB(SCWinGUI.h)

/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxColorsPropPage : Property page dialog

class CSCEMFAxColorsPropPage : public CSCBaseOlePropertyPage
{
	DECLARE_DYNCREATE(CSCEMFAxColorsPropPage)
	DECLARE_OLECREATE_EX(CSCEMFAxColorsPropPage)

// Constructors
public:
	CSCEMFAxColorsPropPage();

// Dialog Data
	//{{AFX_DATA(CSCEMFAxColorsPropPage)
	enum { IDD = IDD_PROPPAGE_COLORS };
	CSCColorBox	m_CmbPaperColorRGB;
	CSCSysColorBox	m_CmbPaperColorSys;
	int		m_nTransparencyMode;
	int		m_nReverseVideoMode;
	int		m_nPaperColorStyle;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support

	SC_DECLARE_ENABLE_CONTROL()
	void SCEnableControls();

// Message maps
protected:
	//{{AFX_MSG(CSCEMFAxColorsPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioPapercolorTrans();
	afx_msg void OnRadioPapercolorSys();
	afx_msg void OnRadioPapercolorRgb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	OLE_COLOR m_nPaperColor;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCEMFAXCOLORSPROPPAGE_H__5A34E1EB_517A_4881_9C9F_6E30C443B85B__INCLUDED_)
