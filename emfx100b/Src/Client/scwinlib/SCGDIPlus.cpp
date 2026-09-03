/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCGDIPlus.h"

using namespace Gdiplus;

//	#ifdef _DEBUG
//	#define new DEBUG_NEW
//	#undef THIS_FILE
//	static char THIS_FILE[] = __FILE__;
//	#endif

CSCGDIPlus::CSCGDIPlus():
	m_GdiplusStartupInput(),
	m_bInitialized(FALSE),
	m_pGdiplusToken(NULL),
	m_dwRefCount(0)
{
	::InitializeCriticalSection(&m_lRefCriticalSection);
	TRACE0("CSCGDIPlus constructed\n");
}

CSCGDIPlus::~CSCGDIPlus()
{
	// Shutdown must have been called
	ASSERT(!m_bInitialized);
	::DeleteCriticalSection(&m_lRefCriticalSection);
	TRACE0("CSCGDIPlus destructed\n");
}

BOOL CSCGDIPlus::SCInitInstance()
{
	::EnterCriticalSection( &m_lRefCriticalSection );
	// Should be called once per application
	ASSERT(!m_bInitialized || (m_dwRefCount>0));

	if (0==m_dwRefCount)
	{
		Status status = GdiplusStartup(&m_pGdiplusToken, &m_GdiplusStartupInput, NULL);
		m_bInitialized = (Ok==status); 
		TRACE1("CSCGDIPlus::...GdiplusStartup, status=%d\n", m_bInitialized);
	}

	if (m_bInitialized)
		m_dwRefCount++;
	::LeaveCriticalSection( &m_lRefCriticalSection );
	return m_bInitialized;
}

BOOL CSCGDIPlus::SCExitInstance()
{
	::EnterCriticalSection( &m_lRefCriticalSection );
	ASSERT(m_bInitialized);

	if (m_bInitialized)
	{
		if (--m_dwRefCount == 0)
		{
			GdiplusShutdown(m_pGdiplusToken);
			m_bInitialized = FALSE;
			m_pGdiplusToken = NULL;
			TRACE0("CSCGDIPlus::...GdiplusShutdown\n");
		}
	}
	::LeaveCriticalSection( &m_lRefCriticalSection );

	return TRUE;
}