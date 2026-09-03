/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCREGISTRY_H_
#define _SCREGISTRY_H_

LONG SCRegCreateKey(HKEY hMainKey, LPCTSTR lpszPath);

LONG SCRegSetLong(HKEY hMainKey, LPCTSTR lpszPath, LPCTSTR lpszEntry, LONG lEntryValue);
LONG SCRegGetLong(HKEY hMainKey, LPCTSTR lpszPath, LPCTSTR lpszEntry, LONG &lValue);

LONG SCRegSetString(HKEY hMainKey, LPCTSTR lpszPath, LPCTSTR lpszEntry, LPCTSTR lpszEntryValue);
LONG SCRegGetString(HKEY hMainKey, LPCTSTR lpszPath, LPCTSTR lpszEntry, LPTSTR lpszBuffer, DWORD dwBufSize);

LONG SCRegSetBinary(HKEY hMainKey, LPCTSTR lpszPath, LPCTSTR lpszEntry, CONST LPBYTE pBytes, DWORD dwSize);
LONG SCRegGetBinary(HKEY hMainKey, LPCTSTR lpszPath, LPCTSTR lpszEntry, BYTE*& rpBytes, DWORD& rdwSize);

BOOL SCWritePrivateProfileInt(LPCTSTR lpSection, LPCTSTR lpKeyName, int iValue, LPCTSTR lpFileName);
CString SCGetPrivateProfileString(LPCTSTR lpSection, LPCTSTR lpKeyName, LPCTSTR lpszDefault, LPCTSTR lpFileName);

#endif //_SCREGISTRY_H_
//  ------------------------------------------------------------
