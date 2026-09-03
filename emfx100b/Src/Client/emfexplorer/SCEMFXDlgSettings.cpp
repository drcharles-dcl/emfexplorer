/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "emfexplorer.h"
#include "SCEMFXDlgSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCEMFXDlgSettings property page

IMPLEMENT_DYNCREATE(CSCEMFXDlgSettings, CPropertyPage)

CSCEMFXDlgSettings::CSCEMFXDlgSettings():
	CPropertyPage(CSCEMFXDlgSettings::IDD),
	m_iFolderID(-1)
{
}

CSCEMFXDlgSettings::CSCEMFXDlgSettings(UINT nIDCaption, I_SCPropExchange* pPx, int iFolderID):
	CPropertyPage(CSCEMFXDlgSettings::IDD, nIDCaption),
	m_iFolderID(iFolderID)
{
	//{{AFX_DATA_INIT(CSCEMFXDlgSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_dlgGenPage.SCSetPropExchanger(pPx, iFolderID);
	m_dlgGDIpPage.SCSetPropExchanger(pPx, iFolderID);
	m_dlgColorsPage.SCSetPropExchanger(pPx, iFolderID);
}

CSCEMFXDlgSettings::~CSCEMFXDlgSettings()
{
	ASSERT(!IsWindow(m_PropertySheet.m_hWnd));
	if (IsWindow(m_PropertySheet.m_hWnd)) // should never be true
		m_PropertySheet.DestroyWindow();
}

void CSCEMFXDlgSettings::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSCEMFXDlgSettings)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		SetModified();
	}
}


BEGIN_MESSAGE_MAP(CSCEMFXDlgSettings, CPropertyPage)
	//{{AFX_MSG_MAP(CSCEMFXDlgSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCEMFXDlgSettings utilities


/////////////////////////////////////////////////////////////////////////////
// CSCEMFXDlgSettings message handlers

BOOL CSCEMFXDlgSettings::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// extra initialization here
	m_PropertySheet.AddPage(&m_dlgGenPage);
	m_PropertySheet.AddPage(&m_dlgGDIpPage);
	m_PropertySheet.AddPage(&m_dlgColorsPage);
	// Note: see Article ID: Q92905 for the style
	if (m_PropertySheet.Create(this, WS_GROUP | WS_CHILD | WS_VISIBLE | DS_SETFONT,
									 WS_EX_CONTROLPARENT))
	{
		CRect rcPPS;
		m_PropertySheet.GetClientRect(&rcPPS);

		CRect rcPage;
		GetClientRect(&rcPage);
		int x = (rcPage.Width() - rcPPS.Width())/2;
		int y = (rcPage.Height() - rcPPS.Height())/2;
		rcPage.OffsetRect(x, y);

		m_PropertySheet.MoveWindow(&rcPage);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSCEMFXDlgSettings::OnOK()
{
	m_PropertySheet.SendMessage(WM_COMMAND, IDOK, 0);
}

BOOL CSCEMFXDlgSettings::OnSetActive() 
{
	BOOL bOk = CPropertyPage::OnSetActive();

	CWnd* pParent = GetParent();
	ASSERT(pParent);
	NMHDR nmhdr;
	nmhdr.code = PSN_SETACTIVE;
	nmhdr.hwndFrom = m_hWnd;
	nmhdr.idFrom = m_iFolderID; //GetDlgCtrlID();
	pParent->SendMessage(WM_NOTIFY, (WPARAM)nmhdr.idFrom, (LPARAM)&nmhdr);
	return bOk;
}
