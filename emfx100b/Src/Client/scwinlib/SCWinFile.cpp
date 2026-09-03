/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCWinFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL SCExistFile(LPCTSTR lpszFname)
{
	CFileStatus rStatus;
	if (CFile::GetStatus(lpszFname, rStatus ))
		return (!(rStatus.m_attribute & CFile::directory));
	return FALSE;
}

// Note: good for an existing file
BOOL SCIsWriteableFile(LPCTSTR lpszFname)
{
	CFileStatus rStatus;
	if (CFile::GetStatus(lpszFname, rStatus ))
		return (0==(rStatus.m_attribute & CFile::directory) &&
				0==(rStatus.m_attribute & CFile::readOnly));
	return FALSE;
}

BOOL SCIsWriteableMedia(LPCTSTR lpszFname)
{
	TCHAR drive[_MAX_DRIVE];
	CString strPath;
#if 1
	_tsplitpath(lpszFname, drive, NULL, NULL, NULL);
	strPath.Format(_T("%s\\"), drive);

	switch (GetDriveType(LPCTSTR(strPath)))
	{
	case DRIVE_FIXED:
		// we should check write permission. But NT ... (see below)
		return TRUE;

	case DRIVE_REMOTE:
		// unfortunately, we don't have a DRIVE_REMOTE_CDROM flag
		// TODO:
		return FALSE;

	case DRIVE_CDROM:
	case DRIVE_REMOVABLE:
	default:
		break;
	}
	return FALSE;
#else
	// in Windows NT, all directories have read and write access
	// So, this won't work
	TCHAR dir[_MAX_DIR];
	_tsplitpath(lpszFname, drive, dir, NULL, NULL);
	strPath.Format(_T("%s%s"), drive, dir);
	CFileStatus rStatus;
	// GetStatus fails on directory names containing a trailing backslash
	if (strPath.Right(1)==_T('\\'))
		strPath = strPath.Left(strPath.GetLength() - 1);
	if (CFile::GetStatus(strPath, rStatus ))
		return (!(rStatus.m_attribute & CFile::readOnly));
	return FALSE;
#endif
}

// temporary dirname with trailing backslash, and using strSubdir
BOOL SCGetFullTempDirName(CString & strFinalDir, const CString & strSubdir, DWORD *pErrorCode/*=NULL*/)
{
	strFinalDir = _T("");
	DWORD dwResult = ::GetTempPath(MAX_PATH, strFinalDir.GetBuffer(MAX_PATH));
	if (pErrorCode)
		*pErrorCode = ::GetLastError();
	strFinalDir.ReleaseBuffer();

	if (dwResult)
	{
		if (strFinalDir.Right(1) != _T("\\"))
			strFinalDir += _T("\\");
		if (!strSubdir.IsEmpty())
		{
			strFinalDir += strSubdir;
			if (strFinalDir.Right(1)!=_T("\\"))
				strFinalDir += _T("\\");
		}
		return TRUE;
	}

	return FALSE;
}

///
/// Splitpath with support for UNC
///
void SCSplitPath(LPCTSTR szPath, LPTSTR szDrive, LPTSTR szDir, LPTSTR szFile, LPTSTR szExt)
{
	ASSERT(szPath);
	
	TCHAR drive[SC_MAX_UNC_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	if (!szDrive)
		szDrive = drive;
	if (!szDir)
		szDir = dir;
	if (!szFile)
		szFile = fname;
	if (!szExt)
		szExt = ext;

	BOOL bUNC = FALSE;
	LPCTSTR szVolume = _tcspbrk(szPath, _T(":\\/"));
	if (szVolume)
	{
		switch (*szVolume)
		{
		case _T('\\'):
			if (_T('\\') == *(++szVolume))
				bUNC = TRUE; // using UNC
			break;
			
		case _T('/'):
			if (_T('/') == *(++szVolume))
				bUNC = TRUE; // using UNC 
			break;
			
		case _T(':'):
			// using drive letter
			break;
			
		default:
			ASSERT(0);
		}
	}

	if (bUNC)
	{
		szVolume = _tcspbrk(++szVolume, _T("\\/")); // server

		ASSERT(szVolume);
		szVolume = _tcspbrk(++szVolume, _T("\\/"));	// share
		
		ASSERT(szVolume);
		_tsplitpath(szVolume, NULL, szDir, szFile, szExt);

		int iCount = szVolume - szPath;

		ASSERT(iCount<SC_MAX_UNC_DRIVE);
		_tcsncpy(szDrive, szPath, iCount);
		szDrive[iCount] = _T('\0');
	} else
	{
		_tsplitpath(szPath, szDrive, szDir, szFile, szExt);
	}
}

///
/// Create a subdirectory tree under the given directory
/// (we miss a CreateDirectoryTree function)
///
BOOL SCCreateDir(const TCHAR *szConstDir, DWORD *pErrorCode/*=NULL*/)
{
	ASSERT(szConstDir);
	if (!szConstDir)
		return FALSE;

	DWORD dwErrorCode = 0;

	//make a local copy to avoid problems
	int sLen = _tcslen(szConstDir);
	TCHAR *szDir = new TCHAR[sLen+1];
	_tcscpy(szDir, szConstDir);
	//now let's make the parse stuff

	// pass the volume (<drive letter>:\), or UNC name (\\server\share\)
	BOOL bUNC = FALSE;
	TCHAR  *szSubDir = _tcspbrk(szDir, _T(":\\/"));
	if (szSubDir)
	{
		switch (*szSubDir)
		{
		case _T('\\'):
			if (_T('\\') == *(++szSubDir))
				bUNC = TRUE; // using UNC
			break;
			
		case _T('/'):
			if (_T('/') == *(++szSubDir))
				bUNC = TRUE; // using UNC 
			break;
			
		case _T(':'):
			// using drive letter
			szSubDir++;
			break;
			
		default:
			ASSERT(0);
		}
	}

	if (bUNC)
	{
		szSubDir = _tcspbrk(++szSubDir, _T("\\/")); // server
		ASSERT(szSubDir);

		szSubDir = _tcspbrk(++szSubDir, _T("\\/"));	// share
		ASSERT(szSubDir);
	}

	//skip the root dir
	ASSERT(*szSubDir==_T('\\') || *szSubDir==_T('/'));
	szSubDir++;

	// create sub dirs
	BOOL bDir = TRUE;
	for (; (*szSubDir); szSubDir++)
	{
		switch(*szSubDir)
		{
		case _T('\\'): // take the left part and create a subdir
		case _T('/'):
			*szSubDir = _T('\0');
			bDir = ::CreateDirectory(szDir, NULL);
			// if dir already exists, continue
			if (!bDir)
			{
				bDir = ((dwErrorCode = ::GetLastError()) == ERROR_ALREADY_EXISTS);
				if (!bDir)
					goto clean_exit;
			}
			*szSubDir = _T('\\');
			break;
		}
	}
	// if there is no trailing \, create leaf subdir
	szSubDir--;
	if (bDir && *szSubDir!=_T('\\') && *szSubDir!=_T('/'))
	{
		bDir = ::CreateDirectory(szDir, NULL);
		if (!bDir)
			bDir = ((dwErrorCode = ::GetLastError()) == ERROR_ALREADY_EXISTS);
	}

clean_exit:
	//clean up
	delete [] szDir;
	if (pErrorCode)
		*pErrorCode = (dwErrorCode != ERROR_ALREADY_EXISTS) ?  dwErrorCode : 0;

	return bDir;
}

BOOL SCCreateTempDir(CString & strFinalDir, const CString & strSubdir, DWORD *pErrorCode/*=NULL*/)
{
	if (::SCGetFullTempDirName(strFinalDir, strSubdir, pErrorCode))
		return ::SCCreateDir(strFinalDir,pErrorCode);

	return FALSE;
}

#define SC_LITTLE_FROM_BIG_DW(p) \
			(DWORD)((*(p)<<24) + (*(p+1)<<16) + (*(p+2)<<8) + *(p+3))

#define SC_LITTLE_FROM_BIG_US(p) \
			(USHORT)((*(p)<<8)  + *(p+1))

#define SC_TTFILE_HEADERSIZE		(sizeof(FIXED) + 4*sizeof(USHORT))
#define SC_TTFILE_MINTABLES			10
#define SC_TTFILE_TBLENTRYSIZE		(4*sizeof(ULONG))
#define SC_TTFILE_MINSIZE			(SC_TTFILE_HEADERSIZE + SC_TTFILE_MINTABLES*SC_TTFILE_TBLENTRYSIZE) 

///
/// Tell if a font file may be TrueType (CreateScalableFontResource is more reliable)
///
BOOL SCIsTrueTypeFontFile(LPCTSTR lpszFilename)
{
	BOOL bTT = FALSE;
	LPBYTE pBytes = SCReadFileBytes(lpszFilename, SC_TTFILE_MINSIZE);
	if (pBytes)
	{
		DWORD dw = SC_LITTLE_FROM_BIG_DW(pBytes);
		// sfnt version==1.0
		bTT = (SC_LITTLE_FROM_BIG_DW(pBytes) == 0x00010000UL);
		if (bTT)
		{// at leat the required tables must exist
			USHORT usNbTables = SC_LITTLE_FROM_BIG_US(pBytes + sizeof(FIXED));
			bTT = (usNbTables>=SC_TTFILE_MINTABLES);
		}
		delete [] pBytes;
	}
	return bTT;
}

///
/// Return a buffer containing dwCount bytes read from the begining of a file
///
LPBYTE SCReadFileBytes(LPCTSTR lpszFname, DWORD dwCount, DWORD* pErrorCode/*=NULL*/)
{
	LPBYTE pBytes = NULL;
	UINT uiMode = SetErrorMode(SEM_FAILCRITICALERRORS);
	// Open the file for reading. 
    HANDLE hFile = CreateFile(lpszFname, GENERIC_READ, FILE_SHARE_READ, NULL,  
                        OPEN_EXISTING, 0, 0); 
    if (hFile == INVALID_HANDLE_VALUE)
	{
		if (pErrorCode)
			*pErrorCode = GetLastError();
	} else
	{
		pBytes = new BYTE[dwCount];
		if (pBytes)
		{
			DWORD dwRead;
			if (!ReadFile(hFile, pBytes, dwCount, &dwRead, NULL) || dwRead<dwCount)
			{
				if (pErrorCode)
					*pErrorCode = GetLastError();
				delete [] pBytes;
				pBytes = NULL;
			}
		}
		CloseHandle(hFile);
	}

	SetErrorMode(uiMode);
	return pBytes;
}

BOOL SCRemoveFileAttributes(LPCTSTR lpszFilename, DWORD dwAttrsToRemove)
{
	DWORD dwAttributes = GetFileAttributes(lpszFilename);
	return SetFileAttributes(lpszFilename, dwAttributes & ~dwAttrsToRemove);
}

