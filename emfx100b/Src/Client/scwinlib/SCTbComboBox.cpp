/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCTbComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCTbComboBox

CSCTbComboBox::CSCTbComboBox()
{
}

CSCTbComboBox::~CSCTbComboBox()
{
}

BEGIN_MESSAGE_MAP(CSCTbComboBox, CComboBox)
	//{{AFX_MSG_MAP(CSCTbComboBox)
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelendok)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelendok)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSCTbComboBox::OnSelendok() 
{
	// Notify parent
	CWnd *pParent = GetParent();
	
	ASSERT(pParent);
	NMHDR nmhdr;
	nmhdr.code = CBN_SELCHANGE;
	nmhdr.hwndFrom = m_hWnd;
	nmhdr.idFrom = GetDlgCtrlID();
	pParent->SendMessage(WM_NOTIFY, (WPARAM)nmhdr.idFrom, (LPARAM)&nmhdr);
}