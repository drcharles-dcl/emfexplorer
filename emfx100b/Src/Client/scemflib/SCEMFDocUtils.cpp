/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFDocUtils.h"
#include "SCEMFDoc.h"

#include <afxdlgs.h>

#include "SCGenInclude.h"
#include SC_INC_SHARED(SCZipFile.h)
#include SC_INC_WINLIB(SCWinFile.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


typedef struct tag_SCExtensionDef
{
	LPCTSTR	szExt;
	UINT uiFtype;
} SCExtensionDef;

SCExtensionDef s_ExtensionsMap[] =
{
	_T(".jpg"),		SC_FILETYPE_JPG,
	_T(".jpeg"),	SC_FILETYPE_JPG,
	_T(".png"),		SC_FILETYPE_PNG,
	_T(".bmp"),		SC_FILETYPE_BMP,
	_T(".gif"),		SC_FILETYPE_GIF,
	_T(".tiff"),	SC_FILETYPE_TIFF,
	_T(".tif"),		SC_FILETYPE_TIFF,
	// fun
	_T(".rtf"),		SC_FILETYPE_RTF,
	_T(".txt"),		SC_FILETYPE_TXT,
	// unlikely
	_T(".exif"),	SC_FILETYPE_EXIF,
	_T(".exi"),		SC_FILETYPE_EXIF
};

// use only in save
SCExtensionDef s_ExtMetaMap[] =
{
	_T(".emf"),		SC_FILETYPE_EMFONLY,
	_T(".wmf"),		SC_FILETYPE_WMF,
};

#ifdef SC_EXTRATXT_EXT

// extra fun
LPCTSTR	s_ExtraTxtExtensions[] =
{
	_T(".h"),
	_T(".hpp"),
	_T(".hxx"),
	_T(".hm"),
	_T(".htm"),		// well, not so fun (but funny)
	_T(".html"),	// ditto
	_T(".xml"),
	_T(".inl"),
	_T(".idl"),
	_T(".odl"),
	_T(".hpj"),
	_T(".c"),
	_T(".cpp"),
	_T(".cxx"),
	_T(".rc"),
	_T(".rc2")
};

#endif


//////////////////////////////////////////////////////////////////////////////////
// Utils

LPTSTR SCCopyText(LPCTSTR lpszText)
{
	if (!lpszText)
		return NULL;
	LPTSTR szChars = new TCHAR[_tcslen(lpszText)+1];
	_tcscpy(szChars, lpszText);
	return szChars;

}

CString SCFNameExtFromPath(LPCTSTR lpszPath)
{
	TCHAR szFname[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	SCSplitPath(lpszPath, NULL, NULL, szFname, szExt);
	CString strFnameExt;
	strFnameExt.Format(_T("%s%s"), szFname, szExt);
	return strFnameExt;
}

CString SCMakeupDocDir(LPCTSTR lpszUnidocName)
{
	CString strDocDir = lpszUnidocName;
	int iPos = strDocDir.ReverseFind(_T('\\'));
	if (iPos>0)
		strDocDir = strDocDir.Left(iPos + 1);
	else
		strDocDir = _T("");
	strDocDir.MakeLower();
	return strDocDir;
}

HENHMETAFILE SCEMFFromFile(LPCTSTR lpzsFname, UINT uiFileType)
{
	switch (uiFileType)
	{
	case SC_FTYPE_EMF:
			return ::GetEnhMetaFile(lpzsFname);

	case SC_FTYPE_WMF:
			return SCConvertWMFtoEMF(lpzsFname);

	case SC_FTYPE_EMZ:
	case SC_FTYPE_WMZ:
			{
				CMemFile MemFile;
				if (SCUnzipGZFile(lpzsFname, MemFile))
				{
					DWORD dwLen = MemFile.GetLength();
					LPBYTE pBytes = MemFile.Detach();
					HENHMETAFILE hEMF = (uiFileType==SC_FTYPE_EMZ)	?
								::SetEnhMetaFileBits(dwLen, pBytes)	:
								SCConvertWMFtoEMF(dwLen, pBytes);
					delete [] pBytes;
					return hEMF;
				}
			}
	case SC_FTYPE_IMG:
		return SCConvertImagetoEMF(lpzsFname);
	}
	return NULL;
}

BOOL SCGetFilesInsertList(CStringList& strLFiles, LPCTSTR lpszCurFile, BOOL bImgOnly/*=FALSE*/)
{
	CString sFilters((bImgOnly) ?SC_EMFMODE_FILTERS_REPLACE: SC_EMFMODE_FILTERS_INSERT);
	CFileDialog dlg(TRUE, NULL, lpszCurFile, OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_ALLOWMULTISELECT,
		(LPCTSTR)sFilters);
	dlg.m_ofn.lpstrTitle = _T("Insert Files");
	// see Q179372 (similar problem on my XP: refused to open more than 6 short-named files)
	DWORD MAXFILE = 2562; //2562 is the max
	dlg.m_ofn.nMaxFile = MAXFILE;
	TCHAR* pc = new TCHAR[MAXFILE];
	dlg.m_ofn.lpstrFile = pc;
	dlg.m_ofn.lpstrFile[0] = NULL;
	//
	if (lpszCurFile)
		_tcscpy(pc, lpszCurFile);

	if (dlg.DoModal() != IDOK)
	{
		delete [] pc;
		return FALSE;
	}
	// Filter out native docs (merge not supported)
	POSITION pos = dlg.GetStartPosition();
	while (pos)
	{
		CString strFile = dlg.GetNextPathName(pos);
		if (!SCEMFDoc::SCIsNativeDoc(strFile))
			strLFiles.AddTail(strFile);
	}
	delete [] pc;

	return TRUE;
}


CString SCTempNameFromFilename(LPCTSTR lpszFname)
{
	TCHAR szPath[MAX_PATH];
	TCHAR szBuff[MAX_PATH];
	DWORD dwLength = ::GetTempPath(MAX_PATH, szPath);
	if ((dwLength>0)
		&& (dwLength<MAX_PATH)
		&& ::GetTempFileName(szPath, _T("EMX"), 0, szBuff))
	{
		CString strTempFName = szBuff;
		return strTempFName;
	}
	return _T("");
}

// Path extension from filter index (relies on the arrangement of SC_EMFMODE_FILTERS)
void SCAddPathExtFromFilterIndex(CString& strPath, CString& sFilters, int iIndex)
{
	if (-1==strPath.ReverseFind(_T('.')))
	{
		int iPos = 0;
		for (int i=0; (i<iIndex); i++, iPos += 7)
		{
			iPos = sFilters.Find(_T(")|*."), iPos);
		}
		int iStart = iPos - 4;
		while (sFilters[iPos] != _T('|'))
			iPos++;
		
		strPath += sFilters.Mid(iStart, iPos-iStart);
	}
}

// File type from filter index (relies on the arrangement of SC_EMFMODE_FILTERS_IMG)
int SCFileTypeFromFilterIndexImg(int iIndex)
{
	switch (iIndex)
	{
	case 1:  return SC_FILETYPE_EMFONLY; break;
	case 2:  return SC_FILETYPE_EMFPLUS; break;
	case 3:  return SC_FILETYPE_EMFDUAL; break;
	case 4:  return SC_FILETYPE_WMF;  break;
	case 5:  return SC_FILETYPE_BMP;  break;
	case 6:  return SC_FILETYPE_JPG;  break;
	case 7:  return SC_FILETYPE_PNG;  break;
	case 8:  return SC_FILETYPE_GIF;  break;
	case 9:  return SC_FILETYPE_TIFF;  break;
	case 0:
		TRACE0("Can't use custom file extension.");
		break;
		
	default:
		ASSERT(0);
	}
	return SC_FTYPE_UKN;
}


#ifdef SC_CANSAVEVECTOR_ASTEXT

// File type from filter index (relies on the arrangement of SC_EMFMODE_FILTERS)
int SCFileTypeFromFilterIndex(int iIndex)
{
	switch (iIndex)
	{
	case 1:  return SC_FILETYPE_EMFONLY; break;
	case 2:  return SC_FILETYPE_EMFPLUS; break;
	case 3:  return SC_FILETYPE_EMFDUAL; break;
	case 4:  return SC_FILETYPE_WMF;  break;
	case 5:  return SC_FILETYPE_TXT;  break;
	case 6:  return SC_FILETYPE_BMP;  break;
	case 7:  return SC_FILETYPE_JPG;  break;
	case 8:  return SC_FILETYPE_PNG;  break;
	case 9:  return SC_FILETYPE_GIF;  break;
	case 10:  return SC_FILETYPE_TIFF;  break;
	case 0:
		TRACE0("Can't use custom file extension.");
		break;
		
	default:
		ASSERT(0);
	}
	return SC_FTYPE_UKN;
}

#endif

UINT SCFileTypeFromExt(LPCTSTR lpszFname, BOOL bExcludeMeta/*=FALSE*/)
{
	UINT uiType = SC_FTYPE_UKN;
	TCHAR szExt[_MAX_EXT];
	_tsplitpath(lpszFname, NULL, NULL, NULL, szExt);

	for (int i=0; i < sizeof(s_ExtensionsMap)/sizeof(SCExtensionDef); i++)
	{
	  if (0==_tcsicmp(szExt, s_ExtensionsMap[i].szExt))
		  return s_ExtensionsMap[i].uiFtype;
	}

	if (!bExcludeMeta)
	{
		for (int i=0; i < sizeof(s_ExtMetaMap)/sizeof(SCExtensionDef); i++)
		{
			if (0==_tcsicmp(szExt, s_ExtMetaMap[i].szExt))
				return s_ExtMetaMap[i].uiFtype;
		}
	}

#ifdef SC_EXTRATXT_EXT
	// let's have some fun
	{
		for (int i=0; i < sizeof(s_ExtraTxtExtensions)/sizeof(LPCTSTR); i++)
		{
			if (0==_tcsicmp(szExt, s_ExtraTxtExtensions[i]))
				return SC_FILETYPE_TXT;
		}
	}
#endif

	return uiType;
}

void SCFilterDocDirFileList(CStringList& rFilesList, CString& strDocDir, BOOL bFilterTxt/*=TRUE*/)
{
	CStringList strLFiles;
	POSITION pos = rFilesList.GetHeadPosition();
	while (pos)
	{
		CString strFile = rFilesList.GetNext(pos);
		UINT uiFType = SCFileTypeFromExt(strFile) & SC_FTYPE_MASK;
		if (SC_FTYPE_BGP==uiFType)
			continue;	// merging native docs isn't supported

		if (SC_FTYPE_TXT==uiFType)
		{
			if (bFilterTxt)
				continue;
		} else
		{
			CString strDir = SCMakeupDocDir(strFile);
			ASSERT(!strDir.IsEmpty());
			if (!strDocDir.IsEmpty())
			{
				if (strDir.Compare(strDocDir))
				{
					continue;
				}
			} else
				strDocDir = strDir; // retain the first dir
		}
		strLFiles.AddTail(strFile);
	}
	rFilesList.RemoveAll();

	pos = strLFiles.GetHeadPosition();
	while (pos)
	{
		rFilesList.AddTail(strLFiles.GetNext(pos));
	}
	strLFiles.RemoveAll();
}

