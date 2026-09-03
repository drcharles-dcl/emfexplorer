/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#if !defined(AFX_SCEMFXCOLORSPROPPAGE_H__B70EE3EB_7451_4BF1_B0C3_DA7CF05A4FD4__INCLUDED_)
#define AFX_SCEMFXCOLORSPROPPAGE_H__B70EE3EB_7451_4BF1_B0C3_DA7CF05A4FD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SCEMFXColorsPropPage.h : header file
//

#include "SCEMFXBasePropPage.h"
#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCColorBox.h)
#include SC_INC_WINLIB(SCWinGUI.h)


/////////////////////////////////////////////////////////////////////////////
// CSCEMFXColorsPropPage dialog

class CSCEMFXColorsPropPage : public CSCEMFXBasePropPage
{
	DECLARE_DYNCREATE(CSCEMFXColorsPropPage)

// Construction
public:
	CSCEMFXColorsPropPage();
	~CSCEMFXColorsPropPage();

// Dialog Data
	//{{AFX_DATA(CSCEMFXColorsPropPage)
	enum { IDD = IDD_PROPPAGE_COLORS };
	CSCColorBox	m_CmbPaperColorRGB;
	CSCSysColorBox	m_CmbPaperColorSys;
	int		m_nTransparencyMode;
	int		m_nReverseVideoMode;
	int		m_nPaperColorStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSCEMFXColorsPropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
	virtual void OnOK();
	//}}AFX_VIRTUAL

protected:
	void SCEnableControls();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSCEMFXColorsPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioPapercolorTrans();
	afx_msg void OnRadioPapercolorSys();
	afx_msg void OnRadioPapercolorRgb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	COLORREF m_nPaperColor;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCEMFXCOLORSPROPPAGE_H__B70EE3EB_7451_4BF1_B0C3_DA7CF05A4FD4__INCLUDED_)
