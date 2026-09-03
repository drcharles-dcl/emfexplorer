/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCRegistry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///
/// Create a key under hMainKey
///
LONG SCRegCreateKey(HKEY hMainKey, LPCTSTR lpszPath)
{
	HKEY hKey;
	DWORD dw;
	LONG lError = RegCreateKeyEx(hMainKey, lpszPath, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,&hKey,&dw);
	return lError;
}


///
/// Write a LONG to the registry key determined by hMainKey & lpszEntry
///
LONG SCRegSetLong(HKEY hMainKey, LPCTSTR lpszPath, LPCTSTR lpszEntry, LONG lEntryValue)
{
	HKEY hKey;
	LONG lError = RegOpenKeyEx (hMainKey, lpszPath, 0, KEY_WRITE, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		lError = RegSetValueEx(hKey, lpszEntry, NULL, REG_DWORD, (LPBYTE)&lEntryValue, sizeof(LONG)); 

		RegCloseKey (hKey);
	}

	return lError;
}


///
/// Read a LONG from the registry key determined by hMainKey & lpszEntry
///
LONG SCRegGetLong(HKEY hMainKey, LPCTSTR lpszPath, LPCTSTR lpszEntry,LONG &lValue)
{
	LONG lValueSize = sizeof(LONG);

	HKEY hKey;
	LONG lError = RegOpenKeyEx (hMainKey, lpszPath, 0, KEY_READ, &hKey);

	if (lError == ERROR_SUCCESS)
	{
		DWORD dwType = REG_DWORD;
		lError = RegQueryValueEx( hKey, lpszEntry, NULL, &dwType, (LPBYTE)&lValue, (ULONG*)&lValueSize ); 

		RegCloseKey (hKey);
	}

	return lError;
}

///
/// Write a string to the registry key determined by hMainKey & lpszEntry
///
LONG SCRegSetString(HKEY hMainKey, LPCTSTR lpszPath, LPCTSTR lpszEntry, LPCTSTR lpszEntryValue)
{
	HKEY hKey;
	LONG lError = RegOpenKeyEx (hMainKey, lpszPath, 0, KEY_WRITE, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		// size must include the size of the terminating null character
		DWORD dwEntryValueSize = (_tcslen(lpszEntryValue) + 1)*sizeof(TCHAR);
		lError = RegSetValueEx(hKey, lpszEntry, NULL, REG_SZ, (LPBYTE)lpszEntryValue, dwEntryValueSize); 
		RegCloseKey (hKey);
	}

	return lError;
}

///
/// Read a CString from the registry key determined by hMainKey & lpszEntry
///
LONG SCRegGetString(HKEY hMainKey, LPCTSTR lpszPath, LPCTSTR lpszEntry, LPTSTR lpszBuffer, DWORD dwBufSize)
{
	HKEY hKey;
	LONG lError = RegOpenKeyEx (hMainKey, lpszPath, 0, KEY_READ, &hKey);

	if (lError == ERROR_SUCCESS)
	{
		DWORD dwType = REG_SZ;
		lError = RegQueryValueEx( hKey, lpszEntry, NULL, &dwType, (LPBYTE)lpszBuffer, (ULONG*)&dwBufSize); 
		RegCloseKey (hKey);
	}

	return lError;
}

///
/// Write a string to the registry key determined by hMainKey & lpszEntry
///
LONG SCRegSetBinary(HKEY hMainKey, LPCTSTR lpszPath, LPCTSTR lpszEntry, CONST LPBYTE pBytes, DWORD dwSize)
{
	HKEY hKey;
	LONG lError = RegOpenKeyEx (hMainKey, lpszPath, 0, KEY_WRITE, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		lError = RegSetValueEx(hKey, lpszEntry, NULL, REG_BINARY, (LPBYTE)pBytes, dwSize); 
		RegCloseKey (hKey);
	}

	return lError;
}

///
/// Read a CString from the registry key determined by hMainKey & lpszEntry
///
LONG SCRegGetBinary(HKEY hMainKey, LPCTSTR lpszPath, LPCTSTR lpszEntry, BYTE*& rpBytes, DWORD& rdwSize)
{
	HKEY hKey;
	LONG lError = RegOpenKeyEx (hMainKey, lpszPath, 0, KEY_READ, &hKey);

	rdwSize = 0;
	rpBytes = NULL;
	if (lError == ERROR_SUCCESS)
	{
		DWORD dwType = REG_BINARY;
		lError = RegQueryValueEx(hKey, lpszEntry, NULL, &dwType, NULL, &rdwSize);
		if (lError == ERROR_SUCCESS)
		{
			rpBytes = new BYTE[rdwSize];
			if (rpBytes)
			{
				lError = RegQueryValueEx(hKey, lpszEntry, NULL, &dwType, (LPBYTE)&rpBytes, (ULONG*)&rdwSize); 
				if (lError != ERROR_SUCCESS)
				{
					delete [] rpBytes;
					rpBytes = NULL;
					rdwSize = 0;
				}
			} else
			{
				rdwSize = 0;
				lError = ERROR_NOT_ENOUGH_MEMORY;
			}
		}
		RegCloseKey (hKey);
	}

	return lError;
}

///
/// Write an int as lpKeyName under lpSection in private file lpFileName
///
BOOL SCWritePrivateProfileInt(LPCTSTR lpSection, LPCTSTR lpKeyName, int iValue, LPCTSTR lpFileName)
{

	CString strValue;
	strValue.Format(_T("%d"), iValue);
	return WritePrivateProfileString(lpSection, lpKeyName, strValue, lpFileName);
}

///
/// Get a string from lpKeyName under lpSection in private file lpFileName
///
CString SCGetPrivateProfileString(LPCTSTR lpSection, LPCTSTR lpKeyName, LPCTSTR lpszDefault, LPCTSTR lpFileName)
{
#define SC_STRKEY_SIZE	256
	CString strResult;
	::GetPrivateProfileString(lpSection, lpKeyName, lpszDefault, strResult.GetBuffer(SC_STRKEY_SIZE), SC_STRKEY_SIZE, lpFileName);
	strResult.ReleaseBuffer();
	return strResult;
}