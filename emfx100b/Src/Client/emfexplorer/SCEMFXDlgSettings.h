/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#if !defined(AFX_SCEMFXDLGSETTINGS_H__56EE7B9C_C01B_41A1_BD88_700048EFCBBF__INCLUDED_)
#define AFX_SCEMFXDLGSETTINGS_H__56EE7B9C_C01B_41A1_BD88_700048EFCBBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SCEMFXDlgSettings.h : header file
//

#include "SCEMFXColorsPropPage.h"
#include "SCEMFXGDIpPropPage.h"
#include "SCEMFXGenPropPage.h"

/////////////////////////////////////////////////////////////////////////////
// CSCEMFXDlgSettings dialog

// WARNING: a property sheet would contain this property page containing
// a property sheet containing three property pages. Well convoluted! Isn't it?
//

class CSCEMFXDlgSettings : public CPropertyPage
{
	DECLARE_DYNCREATE(CSCEMFXDlgSettings)

// Construction
public:
	CSCEMFXDlgSettings();
	CSCEMFXDlgSettings(UINT nIDCaption, I_SCPropExchange* pPx, int iFolderID);
	~CSCEMFXDlgSettings();

// Dialog Data
	//{{AFX_DATA(CSCEMFXDlgSettings)
	enum { IDD = IDD_PROPPAGE_SETTINGS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA
	
	void SCClose()
	{
		if (IsWindow(m_PropertySheet.m_hWnd))
			m_PropertySheet.DestroyWindow();
	}
	BOOL SCGetModified()
	{
		if (m_dlgGenPage.SCGetModified())
			return TRUE;
		if (m_dlgGDIpPage.SCGetModified())
			return TRUE;
		if (m_dlgColorsPage.SCGetModified())
			return TRUE;
		return FALSE;
	}

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSCEMFXDlgSettings)
	public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSCEMFXDlgSettings)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CSCEMFXGenPropPage		m_dlgGenPage;
	CSCEMFXGDIpPropPage		m_dlgGDIpPage;
	CSCEMFXColorsPropPage	m_dlgColorsPage;
	CPropertySheet			m_PropertySheet;
	int						m_iFolderID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCEMFXDLGSETTINGS_H__56EE7B9C_C01B_41A1_BD88_700048EFCBBF__INCLUDED_)
