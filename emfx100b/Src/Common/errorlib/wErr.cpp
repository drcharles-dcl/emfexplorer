/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "wErr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void SCShowWinError()
{
	SCShowWinError(GetLastError());
}

void SCShowWinError(DWORD dwError, LPCTSTR lpszCaption/*=NULL*/, LPCTSTR lpszFnMsg/*=NULL*/)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL 
		);

	// Display the string.
	if (!lpszFnMsg)
		MessageBox(NULL, (LPCTSTR)lpMsgBuf, lpszCaption ? lpszCaption : _T("Error"),
		MB_OK | MB_ICONINFORMATION );
	else
	{
		// Process any inserts in lpMsgBuf.
		// ...
		CString strMsg;
		strMsg.Format(_T("%s\n%s"), lpszFnMsg, lpMsgBuf);

		MessageBox(NULL, (LPCTSTR)strMsg, lpszCaption ? lpszCaption : _T("Error"),
		MB_OK | MB_ICONINFORMATION );
	}

	// Free the buffer.
	LocalFree( lpMsgBuf );
}

