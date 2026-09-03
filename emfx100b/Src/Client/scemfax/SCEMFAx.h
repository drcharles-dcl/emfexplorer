/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#if !defined(AFX_SCEMFAX_H__EF6888A3_1DD1_4775_A15D_8E39BCE7AEEB__INCLUDED_)
#define AFX_SCEMFAX_H__EF6888A3_1DD1_4775_A15D_8E39BCE7AEEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SCEMFAx.h : main header file for SCEMFAX.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxApp : See SCEMFAx.cpp for implementation.

class CSCEMFAxApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCEMFAX_H__EF6888A3_1DD1_4775_A15D_8E39BCE7AEEB__INCLUDED)
