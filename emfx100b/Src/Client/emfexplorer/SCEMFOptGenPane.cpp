/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "emfexplorer.h"
#include "SCEMFOptGenPane.h"
#include "EMFexplorerView.h"
#include "EMFexplorerReg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCEMFOptGenPane propertypage


CSCEMFOptGenPane::CSCEMFOptGenPane() : CPropertyPage(CSCEMFOptGenPane::IDD)
{
	//{{AFX_DATA_INIT(CSCEMFOptGenPane)
	m_iDocLoadSettings = (int)SCE_DLSettingsDefault;
	m_iDocSaveSettings = (int)SCE_DSSettingsWysiwyg;
	//}}AFX_DATA_INIT
}

void CSCEMFOptGenPane::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSCEMFOptGenPane)
	DDX_Radio(pDX, IDC_RADIO_OPTGEN_USEDFLT, m_iDocLoadSettings);
	DDX_Radio(pDX, IDC_RADIO_OPTGEN_SAVECONVERTED, m_iDocSaveSettings);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSCEMFOptGenPane, CPropertyPage)
	//{{AFX_MSG_MAP(CSCEMFOptGenPane)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCEMFOptGenPane message handlers

void CSCEMFOptGenPane::SCLoadSettings()
{
	CWinApp* pApp = AfxGetApp();

	m_iDocLoadSettings = pApp->GetProfileInt(s_szAppSection, SC_EMFREG_ON_DOCLOAD, (int)m_iDocLoadSettings);
	m_iDocSaveSettings = pApp->GetProfileInt(s_szAppSection, SC_EMFREG_ON_DOCSAVE, (int)m_iDocSaveSettings);

	UpdateData(FALSE);
}

BOOL CSCEMFOptGenPane::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Extra initialization here
	SCLoadSettings();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSCEMFOptGenPane::OnOK() 
{
	CWinApp* pApp = AfxGetApp();

	pApp->WriteProfileInt(s_szAppSection, SC_EMFREG_ON_DOCLOAD, m_iDocLoadSettings);
	pApp->WriteProfileInt(s_szAppSection, SC_EMFREG_ON_DOCSAVE, m_iDocSaveSettings);
	
	CPropertyPage::OnOK();
}

BOOL CSCEMFOptGenPane::OnSetActive() 
{
	BOOL bOk = CPropertyPage::OnSetActive();

	CWnd* pParent = GetParent();
	ASSERT(pParent);
	NMHDR nmhdr;
	nmhdr.code = PSN_SETACTIVE;
	nmhdr.hwndFrom = m_hWnd;
	nmhdr.idFrom = -1/*eSettingsBadID*/; //GetDlgCtrlID();
	pParent->SendMessage(WM_NOTIFY, (WPARAM)nmhdr.idFrom, (LPARAM)&nmhdr);
	return bOk;
}
