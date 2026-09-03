/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCGDIPLUS_H_
#define _SCGDIPLUS_H_

#include "afxpriv.h"		// Otherwise GDI+ will get in trouble (VC6)
#include <gdiplus.h>
//	using namespace Gdiplus;	// we are using explicit syntax

class CSCGDIPlus
{
// Construction / Destruction
public:
	CSCGDIPlus();
	virtual ~CSCGDIPlus();

// Operation
public:
	BOOL SCInitInstance();
	BOOL SCExitInstance();

// Implementation
public:

private:
	BOOL							m_bInitialized;
	ULONG_PTR						m_pGdiplusToken;
	Gdiplus::GdiplusStartupInput	m_GdiplusStartupInput;
	DWORD							m_dwRefCount;
	CRITICAL_SECTION				m_lRefCriticalSection;
};

#endif //_SCGDIPLUS_H_
//  ------------------------------------------------------------
