/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "emfexplorer.h"
#include "SCEMFXBasePropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCEMFXBasePropPage property page

IMPLEMENT_DYNCREATE(CSCEMFXBasePropPage, CPropertyPage)

CSCEMFXBasePropPage::CSCEMFXBasePropPage():
	CPropertyPage(),
	m_pIExchanger(NULL),
	m_iFolderID(-1),
	m_bSCModified(FALSE)
{
	ASSERT(0); // don't want you to call it
}

CSCEMFXBasePropPage::CSCEMFXBasePropPage(UINT nIDTemplate, UINT nIDCaption,
										 I_SCPropExchange* pPx/*=NULL*/):
	CPropertyPage(nIDTemplate, nIDCaption),
	m_pIExchanger(pPx),
	m_iFolderID(-1)
{
	//{{AFX_DATA_INIT(CSCEMFXBasePropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSCEMFXBasePropPage::~CSCEMFXBasePropPage()
{
}

void CSCEMFXBasePropPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSCEMFXBasePropPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

SC_IMPLEMENT_ENABLE_CONTROL(CSCEMFXBasePropPage)

BEGIN_MESSAGE_MAP(CSCEMFXBasePropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSCEMFXBasePropPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSCEMFXBasePropPage message handlers

BOOL CSCEMFXBasePropPage::OnSetActive() 
{
	BOOL bOk = CPropertyPage::OnSetActive();

	CWnd* pParent = GetParent();
	ASSERT(pParent);
	NMHDR nmhdr;
	nmhdr.code = PSN_SETACTIVE;
	nmhdr.hwndFrom = m_hWnd;
	nmhdr.idFrom = GetDlgCtrlID();
	pParent->SendMessage(WM_NOTIFY, (WPARAM)nmhdr.idFrom, (LPARAM)&nmhdr);
	return bOk;
}
