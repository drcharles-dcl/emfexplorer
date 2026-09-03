/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#if !defined(AFX_SCEMFAXPROPSHEET_H__8EDE70BE_7E3A_4C74_AD48_1919713CD5FB__INCLUDED_)
#define AFX_SCEMFAXPROPSHEET_H__8EDE70BE_7E3A_4C74_AD48_1919713CD5FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SCEMFAxPropSheet.h : header file
//
// Fake PropertySheet class

/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxPropSheet

class CSCEMFAxPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CSCEMFAxPropSheet)

// Construction
public:
	CSCEMFAxPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSCEMFAxPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	void SCAddControlPages();
	void SCInitMembers();

// Attributes
public:

// Operations
public:
	UINT SCDoModal(int x, int y, LPDISPATCH pIDispatch, CAUUID* pCAUUIDs, LCID lcid);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCEMFAxPropSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSCEMFAxPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSCEMFAxPropSheet)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCEMFAXPROPSHEET_H__8EDE70BE_7E3A_4C74_AD48_1919713CD5FB__INCLUDED_)
