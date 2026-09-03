/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "scemfax.h"
#include "SCEMFAxPropSheet.h"
#include <afxconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxPropSheet

IMPLEMENT_DYNAMIC(CSCEMFAxPropSheet, CPropertySheet)

CSCEMFAxPropSheet::CSCEMFAxPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	//SCInitMembers();
	//SCAddControlPages();
}

CSCEMFAxPropSheet::CSCEMFAxPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	//SCInitMembers();
	//SCAddControlPages();
}

CSCEMFAxPropSheet::~CSCEMFAxPropSheet()
{
}


BEGIN_MESSAGE_MAP(CSCEMFAxPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CSCEMFAxPropSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxPropSheet message handlers

void CSCEMFAxPropSheet::SCAddControlPages()
{
}

void CSCEMFAxPropSheet::SCInitMembers()
{
	// Remove the apply button
}


BOOL CSCEMFAxPropSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	//	specialized code here	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// Usable part
UINT CSCEMFAxPropSheet::SCDoModal(int x, int y, LPDISPATCH pIDispatch, CAUUID* pCAUUIDs, LCID lcid)
{
	USES_CONVERSION;

	OCPFIPARAMS ocparams;
	ocparams.cbStructSize = sizeof(ocparams);
	ocparams.hWndOwner = m_psh.hwndParent;
	ocparams.x = x;
	ocparams.y = y;
	ocparams.lpszCaption = T2COLE(m_psh.pszCaption);

	ocparams.cObjects = 1;
	ocparams.lplpUnk = (IUnknown**)&pIDispatch;
	ocparams.cPages = pCAUUIDs->cElems;
	ocparams.lpPages = pCAUUIDs->pElems;

	ocparams.lcid = lcid;
	ocparams.dispidInitialProperty = DISPID_UNKNOWN;

	OleCreatePropertyFrameIndirect(&ocparams);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////


