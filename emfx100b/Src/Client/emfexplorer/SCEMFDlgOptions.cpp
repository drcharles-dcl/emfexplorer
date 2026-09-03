/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "emfexplorer.h"
#include "SCEMFDlgOptions.h"
#include "EMFexplorerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCEMFDlgOptions

IMPLEMENT_DYNAMIC(CSCEMFDlgOptions, CPropertySheet)

CSCEMFDlgOptions::CSCEMFDlgOptions(UINT nIDCaption, I_SCPropExchange* pPx,
								   CWnd* pParentWnd, enumSettingsPage eSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, 0)
	,m_pIExchanger(pPx)
	,m_eShowPage(eSelectPage)
#ifdef SC_USE_SUBTABS
	,m_pDlgDocPane(NULL)
	,m_pDlgDefaultPane(NULL)
#endif
{
	SCInitMembers();
	SCAddControlPages();
}

CSCEMFDlgOptions::CSCEMFDlgOptions(LPCTSTR pszCaption, I_SCPropExchange* pPx,
								   CWnd* pParentWnd, enumSettingsPage eSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, 0)
	,m_pIExchanger(pPx)
	,m_eShowPage(eSelectPage)
#ifdef SC_USE_SUBTABS
	,m_pDlgDocPane(NULL)
	,m_pDlgDefaultPane(NULL)
#endif
{
	SCInitMembers();
	SCAddControlPages();
}

CSCEMFDlgOptions::~CSCEMFDlgOptions()
{
#ifdef SC_USE_SUBTABS
	if (m_pDlgDocPane)
		delete m_pDlgDocPane;
	if (m_pDlgDefaultPane)
		delete m_pDlgDefaultPane;
#endif
}

SC_IMPLEMENT_ENABLE_CONTROL(CSCEMFDlgOptions)

BEGIN_MESSAGE_MAP(CSCEMFDlgOptions, CPropertySheet)
	//{{AFX_MSG_MAP(CSCEMFDlgOptions)
	ON_COMMAND(ID_APPLY_NOW, OnApplyNow)
	ON_WM_SYSCOMMAND()
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCEMFDlgOptions utilities

void CSCEMFDlgOptions::SCAddControlPages()
{
#ifdef SC_USE_SUBTABS
	AddPage(&m_dlgOptPane);

	if (m_pIExchanger && m_pIExchanger->SCWantSettings(eSettingsDefault))
	{
		m_pDlgDefaultPane = new CSCEMFXDlgSettings(IDS_PPG_DFLT_SETTINGS, m_pIExchanger, eSettingsDefault);
		AddPage(m_pDlgDefaultPane);
	}

	if (m_pIExchanger && m_pIExchanger->SCWantSettings(eSettingsDoc))
	{
		m_pDlgDocPane = new CSCEMFXDlgSettings(IDS_PPG_DOC_SETTINGS, m_pIExchanger, eSettingsDoc);
		AddPage(m_pDlgDocPane);
	}
	if (eSettingsDefault==m_eShowPage)
	{
		ASSERT(m_pDlgDefaultPane);
		SetActivePage(m_pDlgDefaultPane);
	} else
	if ( eSettingsDoc==m_eShowPage)
	{
		ASSERT(m_pDlgDocPane);
		SetActivePage(m_pDlgDocPane);
	} else
	{
		ASSERT(0);
	}

#else

	if (eSettingsDoc==m_eShowPage)
	{
		if (m_pIExchanger && m_pIExchanger->SCWantSettings(eSettingsDoc))
		{
			m_dlgGenPPage.SCSetPropExchanger(m_pIExchanger, eSettingsDoc);
			m_dlgGDIpPPage.SCSetPropExchanger(m_pIExchanger, eSettingsDoc);
			m_dlgColorsPPage.SCSetPropExchanger(m_pIExchanger, eSettingsDoc);
		}
	} else
	if (eSettingsDefault==m_eShowPage)
	{
		if (m_pIExchanger && m_pIExchanger->SCWantSettings(eSettingsDefault))
		{
			AddPage(&m_dlgOptPane);
			
			m_dlgGenPPage.SCSetPropExchanger(m_pIExchanger, eSettingsDefault);
			m_dlgGDIpPPage.SCSetPropExchanger(m_pIExchanger, eSettingsDefault);
			m_dlgColorsPPage.SCSetPropExchanger(m_pIExchanger, eSettingsDefault);
		}
	} else
	{
		ASSERT(0);
	}

	AddPage(&m_dlgGenPPage);
	AddPage(&m_dlgGDIpPPage);
	AddPage(&m_dlgColorsPPage);
#endif
}

void CSCEMFDlgOptions::SCInitMembers()
{
	//{{AFX_DATA_INIT(CSCEMFDlgOptions)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

#ifndef SC_WANT_APPLY
	// Remove the apply button
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CSCEMFDlgOptions message handlers


BOOL CSCEMFDlgOptions::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// Specialized code here
#ifdef SC_WANT_APPLY
	// Enable the Apply button
	CPropertyPage* pPPage = GetPage(0);
	if (pPPage)
		pPPage->SetModified();
	// Set the Apply button as the default button
	SendMessage(DM_SETDEFID, ID_APPLY_NOW);
#endif
	SCEnableControl(ID_APPLY_NOW, FALSE);
	return bResult;
}

void CSCEMFDlgOptions::OnApplyNow()
{
	Default();

	CFrameWnd* pFrameWnd = STATIC_DOWNCAST(CFrameWnd, AfxGetMainWnd());
	CView* pView = pFrameWnd->GetActiveFrame()->GetActiveView();
	if (pView)
		pView->SendMessage(AM_USER_CHANGE_OPTIONS, WPARAM(SCGetDocModified()), 0);
}

void CSCEMFDlgOptions::OnSysCommand(UINT nID, LPARAM lParam)
{
#ifdef SC_USE_SUBTABS
	if (SC_CLOSE==nID)
	{// Ensure that any modeless child is destroyed
		if (m_pDlgDocPane)
			m_pDlgDocPane->SCClose();
		if (m_pDlgDefaultPane)
			m_pDlgDefaultPane->SCClose();
	}
#endif
	CPropertySheet::OnSysCommand(nID, lParam);
}

BOOL CSCEMFDlgOptions::SCGetDocModified()
{
#ifdef SC_USE_SUBTABS
	return ((m_pDlgDocPane) ? m_pDlgDocPane->SCGetModified() : FALSE);
#else
	if (eSettingsDoc==m_eShowPage)
	{
		if (m_pIExchanger && m_pIExchanger->SCWantSettings(eSettingsDoc))
		{
			if (m_dlgGenPPage.SCGetModified())
				return TRUE;
			if (m_dlgGDIpPPage.SCGetModified())
				return TRUE;
			if (m_dlgColorsPPage.SCGetModified())
				return TRUE;
		}
	}
	return FALSE;
#endif
}

BOOL CSCEMFDlgOptions::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (CPropertySheet::OnNotify(wParam, lParam, pResult))
		return TRUE;

	NMHDR* pNmhdr = (NMHDR*)lParam;
	if (PSN_SETACTIVE==pNmhdr->code)
	{
#ifdef SC_USE_SUBTABS
		SCEnableControl(ID_APPLY_NOW, (eSettingsDoc==pNmhdr->idFrom));
#else
		if (eSettingsDefault==m_eShowPage)
		{
			SCEnableControl(ID_APPLY_NOW, FALSE);
		} else
			SCEnableControl(ID_APPLY_NOW, (pNmhdr->idFrom!=eSettingsBadID));

#endif
		return TRUE;
	}
	return FALSE;
}
