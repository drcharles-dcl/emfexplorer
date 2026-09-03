/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#if !defined(AFX_SCEMFDLGOPTIONS_H__AA5AE560_BA9F_4B8C_B9C9_E6A4B0A11A54__INCLUDED_)
#define AFX_SCEMFDLGOPTIONS_H__AA5AE560_BA9F_4B8C_B9C9_E6A4B0A11A54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SCEMFDlgOptions.h : header file
//

#include "SCEMFOptGenPane.h"

// Uncomment it if you dare (but before that you'd better read Q92905)
//	#define SC_USE_SUBTABS

#ifdef SC_USE_SUBTABS
	#include "SCEMFXDlgSettings.h"
#else
	#include "SCEMFXColorsPropPage.h"
	#include "SCEMFXGDIpPropPage.h"
	#include "SCEMFXGenPropPage.h"
#endif

// For the apply button
#define SC_WANT_APPLY

/////////////////////////////////////////////////////////////////////////////
// CSCEMFDlgOptions
enum enumSettingsPage
{
	eSettingsBadID=-1,
	eSettingsDefault=0,
	eSettingsDoc
};

class CSCEMFDlgOptions : public CPropertySheet
{
	DECLARE_DYNAMIC(CSCEMFDlgOptions)

// Construction
public:
	CSCEMFDlgOptions(UINT nIDCaption, I_SCPropExchange* pPx, CWnd* pParentWnd = NULL, enumSettingsPage eSelectPage = eSettingsDefault);
	CSCEMFDlgOptions(LPCTSTR pszCaption, I_SCPropExchange* pPx, CWnd* pParentWnd = NULL, enumSettingsPage eSelectPage = eSettingsDefault);

// Attributes
public:

// Operations
public:
	BOOL SCGetDocModified();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCEMFDlgOptions)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSCEMFDlgOptions();

protected:
	// should be called once to in effect add all pages to the property sheet
	void SCAddControlPages();
	void SCInitMembers();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSCEMFDlgOptions)
	afx_msg void OnApplyNow();
	afx_msg void OnSysCommand(UINT nID, LPARAM);
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	SC_DECLARE_ENABLE_CONTROL()

private:
	I_SCPropExchange*	m_pIExchanger;
	CSCEMFOptGenPane	m_dlgOptPane;
	enumSettingsPage	m_eShowPage;

#ifdef SC_USE_SUBTABS
	// take the red pill
	CSCEMFXDlgSettings*	m_pDlgDefaultPane;
	CSCEMFXDlgSettings*	m_pDlgDocPane;
#else
	// stay in wonder land
	CSCEMFXGenPropPage		m_dlgGenPPage;
	CSCEMFXGDIpPropPage		m_dlgGDIpPPage;
	CSCEMFXColorsPropPage	m_dlgColorsPPage;
#endif
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCEMFDLGOPTIONS_H__AA5AE560_BA9F_4B8C_B9C9_E6A4B0A11A54__INCLUDED_)
