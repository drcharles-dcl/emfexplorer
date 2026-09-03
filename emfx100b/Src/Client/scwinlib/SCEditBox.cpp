/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEditBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCEditBox

CSCEditBox::CSCEditBox():
	m_bTrapEnter(FALSE)
{
}

void CSCEditBox::SCReset()
{
	if (IsWindow(m_hWnd))
		SetWindowText(_T(""));
}

CSCEditBox::~CSCEditBox()
{
}

BEGIN_MESSAGE_MAP(CSCEditBox, CEdit)
	//{{AFX_MSG_MAP(CSCEditBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCEditBox message handlers

BOOL CSCEditBox::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		// when the "enter" key is hit in the EditBox we want to notify
		// the parent window
		if ((pMsg->lParam & 0x40000000) == 0)   // Not a repeat.
		{
			CWnd *pParent = GetParent();
			ASSERT(pParent);
			
			if (pParent)
			{
				NMHDR nmhdr;
				nmhdr.code = EN_CHANGE;
				nmhdr.hwndFrom = m_hWnd;
				nmhdr.idFrom = GetDlgCtrlID();
				
				pParent->SendMessage(WM_NOTIFY, (WPARAM)nmhdr.idFrom, (LPARAM)&nmhdr);
			}
		}
		if (m_bTrapEnter)
			return TRUE;
	}
	
	return CEdit::PreTranslateMessage(pMsg);
}
