/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#if !defined(AFX_EMFEXPLORER_H__DAA2D3A9_FD2B_4F06_AE1F_0BB8516264CA__INCLUDED_)
#define AFX_EMFEXPLORER_H__DAA2D3A9_FD2B_4F06_AE1F_0BB8516264CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCGDIPlus.h)
/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerApp:
// See EMFexplorer.cpp for the implementation of this class
//

class CEMFexplorerApp : public CWinApp
{
public:
	CEMFexplorerApp();

// Operation

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEMFexplorerApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	COleTemplateServer m_server;
		// Server object for document creation
	//{{AFX_MSG(CEMFexplorerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CSCGDIPlus m_GDIPlus;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMFEXPLORER_H__DAA2D3A9_FD2B_4F06_AE1F_0BB8516264CA__INCLUDED_)
