/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCWINFILE_H_
#define _SCWINFILE_H_

#define SC_MAX_UNC_DRIVE	MAX_PATH

void SCSplitPath(LPCTSTR szPath, LPTSTR szDrive, LPTSTR szDir, LPTSTR szFile, LPTSTR szExt);

BOOL SCExistFile(LPCTSTR lpszFname);
BOOL SCIsWriteableFile(LPCTSTR lpszFname);
BOOL SCIsWriteableMedia(LPCTSTR lpszFname);

LPBYTE SCReadFileBytes(LPCTSTR lpszFname, DWORD dwCount, DWORD *pErrorCode = NULL);

BOOL SCCreateTempDir(CString & szFinalDir, const CString & szSubDir, DWORD *pErrorCode = NULL);
BOOL SCCreateDir(const TCHAR *szConstDir, DWORD *pErrorCode = NULL);
BOOL SCGetFullTempDirName(CString & strFinalDir, const CString & strSubdir, DWORD *pErrorCode = NULL);

BOOL SCIsTrueTypeFontFile(LPCTSTR lpszFilename);

BOOL SCRemoveFileAttributes(LPCTSTR lpszFilename, DWORD dwAttrsToRemove);

#endif //_SCWINFILE_H_
//  ------------------------------------------------------------
