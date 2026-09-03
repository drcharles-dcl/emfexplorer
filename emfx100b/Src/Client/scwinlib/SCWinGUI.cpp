/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCWinGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///
/// Let Windows breath for a moment, as it is not preemptive.
/// (To call in long loops running in app main thread.)
///
void SCYieldForAWhile()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			::PostQuitMessage(0);
			break;
		}
		if (!AfxGetApp()->PreTranslateMessage(&msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	AfxGetApp()->OnIdle(0);   // updates user interface
	AfxGetApp()->OnIdle(1);   // frees temporary objects
}

/////////////////////////////////////////////////////////////////////////////
// Helpers for saving/restoring window state
#ifndef SC_EXTERN_WINPLACEMENT
	TCHAR BASED_CODE s_szAppSection[] = _T("Settings");
	TCHAR s_szAppPlacementFormat[] = _T("%u,%u,%d,%d,%d,%d,%d,%d,%d,%d");
#endif

///
/// Read a window placement from settings section of app's ini/registry file.
///
BOOL SCReadWindowPlacement(LPWINDOWPLACEMENT pwp, LPCTSTR lpszEntry)
{
	ASSERT(lpszEntry);
	CString strBuffer = AfxGetApp()->GetProfileString(s_szAppSection, lpszEntry);
	if (strBuffer.IsEmpty())
		return FALSE;

	WINDOWPLACEMENT wp;
	int nRead = _stscanf(strBuffer, s_szAppPlacementFormat,
		&wp.flags, &wp.showCmd,
		&wp.ptMinPosition.x, &wp.ptMinPosition.y,
		&wp.ptMaxPosition.x, &wp.ptMaxPosition.y,
		&wp.rcNormalPosition.left, &wp.rcNormalPosition.top,
		&wp.rcNormalPosition.right, &wp.rcNormalPosition.bottom);

	if (nRead != 10)
		return FALSE;

	wp.length = sizeof wp;
	*pwp = wp;
	return TRUE;
}

///
/// Write a window placement to settings section of app's ini/registry file.
///
void SCWriteWindowPlacement(LPWINDOWPLACEMENT pwp, LPCTSTR lpszEntry)
{
	ASSERT(lpszEntry);
	TCHAR szBuffer[sizeof("-32767")*8 + sizeof("65535")*2];

	wsprintf(szBuffer, s_szAppPlacementFormat,
		pwp->flags, pwp->showCmd,
		pwp->ptMinPosition.x, pwp->ptMinPosition.y,
		pwp->ptMaxPosition.x, pwp->ptMaxPosition.y,
		pwp->rcNormalPosition.left, pwp->rcNormalPosition.top,
		pwp->rcNormalPosition.right, pwp->rcNormalPosition.bottom);
	AfxGetApp()->WriteProfileString(s_szAppSection, lpszEntry, szBuffer);
}

///
/// Restore an app main frame.
///
void SCInitialShowWindow(HWND hWnd, UINT nCmdShow, LPCTSTR lpszEntry)
{
	ASSERT(lpszEntry);
	WINDOWPLACEMENT wp;
	if (!SCReadWindowPlacement(&wp, lpszEntry))
	{
		::ShowWindow(hWnd, nCmdShow);
		return;
	}
	if (nCmdShow != SW_SHOWNORMAL)
		wp.showCmd = nCmdShow;
	::SetWindowPlacement(hWnd, &wp);
	::ShowWindow(hWnd, wp.showCmd);
}

///
/// Save an app main frame.
///
void SCSaveWindowPlacement(HWND hWnd, LPCTSTR lpszEntry)
{
	ASSERT(lpszEntry);
	// before it is destroyed, save the position of the window
	WINDOWPLACEMENT wp;
	wp.length = sizeof wp;
	if (::GetWindowPlacement(hWnd, &wp))
	{
		wp.flags = 0;
		if (::IsZoomed(hWnd))
			wp.flags |= WPF_RESTORETOMAXIMIZED;
		// and write it to the .INI file
		SCWriteWindowPlacement(&wp, lpszEntry);
	}
}
