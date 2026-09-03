/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFDoc.h"

#include "SCGenInclude.h"
#include SC_INC_COMMON(kSCProdDefs.h)
#include SC_INC_WINLIB(SCWinFile.h)
#include SC_INC_ERRLIB(wErr.h)
#include SC_INC_SHARED(SCZipFile.h)
#include SC_INC_WINLIB(SCRegistry.h)


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SC_EMFX_TMPFONT_EXT		_T(".eft")
#define SC_EMFX_TMPFRES_EXT		_T(".efr")
#define SC_EMFX_TEMPSUBDIR		SC_PRODUCTNAME
#define SC_EMFX_CLIP_FNAME		_T("Clipboard")


SCEMFDoc::SCEMFDoc():
	m_uiNbFiles(0),
	m_bDocModified(FALSE),
	m_bForgedName(FALSE)
{
}

SCEMFDoc::~SCEMFDoc()
{
	SCCleanup(TRUE);
	SCDeleteLockedFontFiles(TRUE);
}

void SCEMFDoc::SCCleanup(BOOL bClosing /*=FALSE*/)
{
	UINT uiNbPages = m_uiNbFiles;
	m_uiNbFiles = 0; // Won't serve pages after this, as we are deleting them

	// Proceed with clean up
	for (UINT i=0; (i<uiNbPages); i++)
	{
		delete m_vDocPages[i];
	}
	m_vDocPages.clear();
	SCUnInstallFonts();
	m_strUniDocName.Empty();
	m_bForgedName = FALSE;
}

void SCEMFDoc::SCSetPageURL(UINT uiPage, LPCTSTR lpszURL)
{
	ASSERT(uiPage<m_uiNbFiles);
	if (uiPage<m_uiNbFiles)
		m_vDocPages[uiPage]->m_strCreditURL = lpszURL;
}

void SCEMFDoc::SCSetPageCredit(UINT uiPage, LPCTSTR lpszCredit)
{
	ASSERT(uiPage<m_uiNbFiles);
	if (uiPage<m_uiNbFiles)
		m_vDocPages[uiPage]->m_strCredit = lpszCredit;
}

void SCEMFDoc::SCSetPageComment(UINT uiPage, LPCTSTR lpszComment)
{
	ASSERT(uiPage<m_uiNbFiles);
	if (uiPage<m_uiNbFiles)
		m_vDocPages[uiPage]->m_strComment = lpszComment;
}

PSCEMFDocPage SCEMFDoc::SCGetDocPage(UINT uiPage, BOOL bGetCopy/*=FALSE*/)
{
	ASSERT(uiPage<m_uiNbFiles);
	PSCEMFDocPage pDocPage = NULL;
	if (uiPage<m_uiNbFiles)
	{
		ASSERT(m_vDocPages[uiPage]);
		if (bGetCopy)
		{
			pDocPage = new SCEMFDocPage(this);
			pDocPage->SCCopyFrom(*m_vDocPages[uiPage]);
		} else
			pDocPage = m_vDocPages[uiPage];
	}
	return pDocPage;
}

HENHMETAFILE SCEMFDoc::SCUnlockEMF(UINT uiPage, PSCEMFDocPage pShare/*=NULL*/)
{
	ASSERT(uiPage<m_uiNbFiles);
	PSCEMFDocPage pDocPage = m_vDocPages[uiPage];
	ASSERT(pDocPage);
	HENHMETAFILE hMemEMF = pDocPage->SCUnlockEMF();
	if (pShare)
		pShare->m_hEMF = hMemEMF;
	return hMemEMF;
}

void SCEMFDoc::SCForgeDocName(LPCTSTR lpszFname)
{// forge a doc name
	m_strUniDocName = lpszFname;
	int iPos = m_strUniDocName.ReverseFind(_T('.'));
	if (iPos!=-1)
		m_strUniDocName = m_strUniDocName.Left(iPos);
	m_strUniDocName += SC_DOC_EXTENSION;
	m_bForgedName = TRUE;
}

void SCEMFDoc::SCRemovePage(UINT uiPage)
{
	ASSERT(uiPage<m_uiNbFiles);

	delete m_vDocPages[uiPage];
	m_vDocPages.erase(m_vDocPages.begin() + uiPage);
	m_uiNbFiles--;
	if ((0==m_uiNbFiles) && m_bForgedName)
		SCCleanup();
	m_bDocModified = TRUE;
}

BOOL SCEMFDoc::SCReflow(DOCPAGEVECTOR& rvectReflow, INTVECTOR& rvectDescrip, LPCTSTR lpszDocDir, int iDlftCrdPage)
{
	UINT uiNbElems = rvectReflow.size();

	ASSERT(rvectDescrip.size()==uiNbElems);
	ASSERT(lpszDocDir);
	ASSERT(iDlftCrdPage<(int)uiNbElems);
	{
		CString strOldDocDir;
		TCHAR szDrive[_MAX_DRIVE];
		TCHAR szDir[_MAX_DIR];
		TCHAR szFname[_MAX_FNAME];
		TCHAR szExt[_MAX_EXT];
		SCSplitPath(m_strUniDocName, szDrive, szDir, szFname, szExt);
		
		strOldDocDir.Format(_T("%s%s"), szDrive, szDir);
		if (0!=strOldDocDir.CompareNoCase(lpszDocDir))
			m_strUniDocName.Format(_T("%s%s%s"), lpszDocDir, szFname, szExt);
	}

	PSCEMFDocPage pEPage;
	// Detach old EMFs
	for (UINT uiPage=0; (uiPage<uiNbElems); uiPage++)
	{
		int iIdx = rvectDescrip[uiPage];
		if (iIdx>=0)
		{
			ASSERT(iIdx < (int)m_uiNbFiles);
			pEPage=m_vDocPages[iIdx];
			ASSERT(pEPage);
			if (pEPage)
				pEPage->SCDetachEMF();
		}
	}
	// Delete old objects
	for (uiPage=0; (uiPage<m_uiNbFiles); uiPage++)
	{
		if (pEPage=m_vDocPages[uiPage])
			delete pEPage;
	}
	m_vDocPages.clear();
	// Copy new objects
	m_vDocPages.resize(uiNbElems);
	m_vDocPages.assign(rvectReflow.begin(), rvectReflow.end());
	rvectReflow.clear();

	m_uiNbFiles = m_vDocPages.size();

	if (iDlftCrdPage<0)
	{
		m_Properties.strCreditURL.Empty();
		m_Properties.strCredit.Empty();
	} else
	{
		pEPage=m_vDocPages[iDlftCrdPage];
		m_Properties.strCreditURL = pEPage->m_strCreditURL;
		m_Properties.strCredit = pEPage->m_strCredit;
	}

	m_bDocModified = TRUE;
	return TRUE;
}

void SCEMFDoc::SCBeginAddFiles(UINT uiNbFiles, LPCTSTR lpszFName/*=NULL*/)
{
	SCCleanup();
	if (lpszFName)
		m_strUniDocName = lpszFName;
	
	// pages bank
	if (uiNbFiles)
	{
		m_vDocPages.resize(uiNbFiles);
		m_uiNbFiles = uiNbFiles;
		m_vDocPages.assign(m_uiNbFiles, NULL);
	}
}

void SCEMFDoc::SCEndAddFiles()
{
	SCInstallFonts();
}

void SCEMFDoc::SCSetDocDir(LPCTSTR lpszDocDir)
{
	DWORD dwLen = (lpszDocDir) ? _tcslen(lpszDocDir) : 0;
	ASSERT(dwLen);
	ASSERT(!m_strUniDocName.IsEmpty());
	CString strOldDocDir = SCMakeupDocDir(m_strUniDocName);

	ASSERT(strOldDocDir.IsEmpty()||
		strOldDocDir.CompareNoCase(lpszDocDir)==0);
	if (strOldDocDir.IsEmpty())
	{
		ASSERT(m_bForgedName);
		if (lpszDocDir[dwLen-1]!=_T('\\'))
			m_strUniDocName.Insert(0, _T('\\'));
		m_strUniDocName.Insert(0, lpszDocDir);
	}
	SCInstallFonts();
}

BOOL SCEMFDoc::SCAddFile(LPCTSTR lpszFname, UINT uiIdx, BOOL bCopy/*=TRUE*/, BOOL bDelayed/*=FALSE*/)
{
	ASSERT(uiIdx<m_uiNbFiles);
	if (uiIdx>=m_uiNbFiles)
		return FALSE;

	BOOL bError = FALSE;
	// check filetype
	UINT uiFyleType = SC_FSTATE_DELAYED; 
	if (!bDelayed)
	{
		uiFyleType = SCGetFileType(lpszFname, FALSE);
		switch (uiFyleType & SC_FTYPE_MASK)
		{
		case SC_FTYPE_EMF:
		case SC_FTYPE_WMF:
		case SC_FTYPE_EMZ:
		case SC_FTYPE_WMZ:
		case SC_FTYPE_IMG:
		case SC_FTYPE_TXT:
			break;
			
		case SC_FTYPE_UKN:
		default:
			bError = TRUE;
			// delay loading, so that user can use reflow to replace the file
			uiFyleType = SC_FSTATE_DELAYED;
		}
	}
	SCEMFDocPage* pDocPage = new SCEMFDocPage(this);
	m_vDocPages[uiIdx] = pDocPage;

	// set information for delayed loading
	pDocPage->m_uiType = uiFyleType;
	pDocPage->m_strPath = lpszFname;

	if (!bError && m_strUniDocName.IsEmpty())
		SCForgeDocName(lpszFname);
	return TRUE;
}

BOOL SCEMFDoc::SCLoadRTFPages(LPCTSTR lpszFname, BOOL bPaste/*=FALSE*/, BOOL bRTF/*=TRUE*/)
{
	HEMFVECTOR vectHandles;
	UINT uiNbPages = SCConvertRTFtoEMF(lpszFname, vectHandles, bRTF);
	if (0==uiNbPages)
		return FALSE;

	UINT uiStartIdx=0;
	if (bPaste)
	{
		uiStartIdx = m_uiNbFiles;
		m_uiNbFiles += uiNbPages;
		m_vDocPages.resize(m_uiNbFiles);
	} else
		SCBeginAddFiles(uiNbPages);

	// EMFs are just in memory, like a paste: don't forge unidoc name
	CString strPrefix = SCFNameExtFromPath(lpszFname);
	int iPos = strPrefix.ReverseFind(_T('.'));
	if (iPos!=-1)
		strPrefix.SetAt(iPos, _T('_'));
	if (!m_strUniDocName.IsEmpty())
	{
		CString strDir;
		TCHAR drive[_MAX_DRIVE];	// drive of the playlist
		TCHAR dir[_MAX_DIR];		// dir of the playlist
		_tsplitpath(LPCTSTR(m_strUniDocName), drive, dir, NULL, NULL);
		strDir.Format(_T("%s%s"), drive, dir);
		strPrefix = strDir + strPrefix;
	}

	CString strFname;
	for (UINT uiIdx=uiStartIdx; (uiIdx<m_uiNbFiles); uiIdx++)
	{
		SCEMFDocPage* pDocPage = new SCEMFDocPage(this);
		m_vDocPages[uiIdx] = pDocPage;

		strFname.Format(_T("%s_p%d.emf"), strPrefix, (uiIdx - uiStartIdx)+1);
		pDocPage->m_bDirty = TRUE;
		pDocPage->SCAttachEMF(vectHandles[uiIdx - uiStartIdx],
			strFname, SC_FILETYPE_EMFONLY);
	}

	if (!bPaste)
		SCEndAddFiles();
	m_bDocModified = TRUE; // pages are just in memory
	return TRUE;
}

BOOL SCEMFDoc::SCGrowBank(int iNbPages)
{
	m_uiNbFiles += iNbPages;
	m_vDocPages.resize(m_uiNbFiles);
	return TRUE;
}

BOOL SCEMFDoc::SCPasteEMFPages(HEMFVECTOR& rVector)
{
	int iNbPages = rVector.size();
	if (0==iNbPages)
		return FALSE;

	UINT uiNewNbPages = m_uiNbFiles + iNbPages;
	m_vDocPages.resize(uiNewNbPages);

	// Folder of the new files
	CString strDir;
	if (!m_strUniDocName.IsEmpty())
	{
		TCHAR drive[_MAX_DRIVE];	// drive of the playlist
		TCHAR dir[_MAX_DIR];		// dir of the playlist
		_tsplitpath(LPCTSTR(m_strUniDocName), drive, dir, NULL, NULL);
		strDir.Format(_T("%s%s"), drive, dir);
	}

	// Name and paste the files
	static UINT s_uiClipNum = 0;
	CString strClipFname;
	for (UINT uiIdx=m_uiNbFiles; (uiIdx<uiNewNbPages); uiIdx++)
	{
		SCEMFDocPage* pDocPage = new SCEMFDocPage(this);
		m_vDocPages[uiIdx] = pDocPage;

		strClipFname.Format(_T("%s%s%d.emf"), strDir, SC_EMFX_CLIP_FNAME, ++s_uiClipNum);
		pDocPage->m_bDirty = TRUE;
		pDocPage->SCAttachEMF(rVector[uiIdx - m_uiNbFiles],
			strClipFname, SC_FILETYPE_EMFONLY);
	}

	m_uiNbFiles = uiNewNbPages;
	m_bDocModified = TRUE; // some pages are just in memory
	return TRUE;
}


HENHMETAFILE SCEMFDoc::SCGetPageEMF(UINT uiPage)
{
	ASSERT(uiPage<m_uiNbFiles);
	if (uiPage>=m_uiNbFiles)
		return NULL;
	return m_vDocPages[uiPage]->SCGetPageEMF();
}

void SCEMFDoc::SCResizeAllPages()
{
	m_Properties.bSizeAllPages = !m_Properties.bSizeAllPages;
	if (!m_Properties.rectDocument.IsRectEmpty())
	{
		if (m_Properties.bSizeAllPages)
		{// apply
			for (UINT uiPage=0; (uiPage<m_uiNbFiles); uiPage++)
			{
				PSCEMFDocPage pDocPage = m_vDocPages[uiPage];
				ASSERT(pDocPage);
				pDocPage->m_rectSize = m_Properties.rectDocument;
			}
		} else
		{// restore
			for (UINT uiPage=0; (uiPage<m_uiNbFiles); uiPage++)
			{
				PSCEMFDocPage pDocPage = m_vDocPages[uiPage];
				ASSERT(pDocPage);
				pDocPage->SCRecomputeElemsRect();
				if (pDocPage->m_bInflate)
					pDocPage->m_rectSize.InflateRect(pDocPage->m_rectInflate);
			}
		}
		return;
	}

	ASSERT(m_Properties.bSizeAllPages);
	int iMinX = INT_MAX;
	int iMinY = INT_MAX;
	int iMaxX = INT_MIN;
	int iMaxY = INT_MIN;

	for (UINT uiPage=0; (uiPage<m_uiNbFiles); uiPage++)
	{
		PSCEMFDocPage pDocPage = m_vDocPages[uiPage];
		ASSERT(pDocPage);
		HENHMETAFILE hEMF = pDocPage->SCGetPageEMF();
		if (hEMF)
		{
			CRect& ElemsRect = pDocPage->m_rectSize;
			// deflate
			CRect& rectI = pDocPage->m_rectInflate;
			ElemsRect.InflateRect(-rectI.left, -rectI.top, -rectI.right, -rectI.bottom);
			//
			
			if (ElemsRect.left<iMinX)
				iMinX = ElemsRect.left;
			if (ElemsRect.top<iMinY)
				iMinY = ElemsRect.top;

			if (ElemsRect.right>iMaxX)
				iMaxX = ElemsRect.right;
			if (ElemsRect.bottom>iMaxY)
				iMaxY = ElemsRect.bottom;
		}
	}

	ASSERT(iMinX != INT_MAX);
	ASSERT(iMinY != INT_MAX);
	ASSERT(iMaxX != INT_MIN);
	ASSERT(iMaxY != INT_MIN);

	for (uiPage=0; (uiPage<m_uiNbFiles); uiPage++)
	{
		m_vDocPages[uiPage]->m_rectSize.SetRect(iMinX, iMinY, iMaxX, iMaxY);
	}
	m_Properties.rectDocument.SetRect(iMinX, iMinY, iMaxX, iMaxY);
}

void SCEMFDoc::SCInflateElemsRect(UINT uiPage, int iLeft, int iTop, int iRight, int iBottom)
{
	ASSERT(uiPage<m_uiNbFiles);
	if (uiPage>=m_uiNbFiles)
		return;
	m_vDocPages[uiPage]->SCInflateElemsRect(iLeft, iTop, iRight, iBottom);
}

///
/// Install all fonts found in the 'Fonts' subdirectory (of the document's directory)
///
void SCEMFDoc::SCInstallFonts()
{
	SCDeleteLockedFontFiles(); // delayed clean up

	// For now, just install what is found in the Fonts subdirectory if any
	if (m_strUniDocName.IsEmpty())
		return;

	TCHAR drive[_MAX_DRIVE];	// drive of the playlist
	TCHAR dir[_MAX_DIR];		// dir of the playlist
	_tsplitpath(LPCTSTR(m_strUniDocName), drive, dir, NULL, NULL);

	// Look for TT fonts
	CString strFile;
	strFile.Format(_T("%s%s%s"), drive, dir, _T("Fonts\\*.*"));

	CFileFind finder;
	BOOL bFound = finder.FindFile((LPCTSTR)strFile);
	BOOL bTemp = FALSE;
	CString strTempPath;
	if (bFound)
	{
		bTemp = (!SCIsWriteableMedia(m_strUniDocName));
		if (bTemp)
		{// create temp directory
			if (!::SCCreateTempDir(strTempPath, SC_EMFX_TEMPSUBDIR))
			{
				ASSERT(0);
				finder.Close();
				return;
			}
		}
	}
	
	while (bFound)
	{
		bFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			CString strFilePath = finder.GetFilePath();
			if (!SCIsTrueTypeFontFile(strFilePath))
				continue;

			// TODO: Check that font is not already installed
			// if (m_pSysFontsHolder && m_pSysFontsHolder->SCIsFontInstalled(strFilePath))
			// continue;

			if (bTemp)
			{// copy to temp directory
				TCHAR szFname[_MAX_FNAME];
				SCSplitPath((LPTSTR)LPCTSTR(strFilePath), NULL, NULL, szFname, NULL);
				
				CString strFilename;
				strFilename.Format(_T("%s%s%s"), strTempPath, szFname, SC_EMFX_TMPFONT_EXT);
#ifdef _DEBUG
				ASSERT(!SCExistFile(strFilename));
#endif
				BOOL bOk = CopyFile(strFilePath, strFilename, TRUE);
				ASSERT(bOk);
				strFilePath = strFilename;
			}
			
			SCInstallFontFile(strFilePath, bTemp);
		}
	}	
	finder.Close();
#ifdef _DEBUG
	int iCount = m_FontRes.GetCount();
#endif
}

///
/// Uninstall fonts, and delete temporary files.
///
void SCEMFDoc::SCUnInstallFonts()
{
	CString strTempPath;
	::SCGetFullTempDirName(strTempPath, SC_EMFX_TEMPSUBDIR);
	BOOL bTempCheck = FALSE;
	BOOL bTempFiles = FALSE;

	POSITION pos = m_FontRes.GetHeadPosition();
	CString strFilename;
	BOOL bOK;
	while(pos)
	{
		strFilename = m_FontRes.GetNext(pos);

		ASSERT(!strFilename.IsEmpty());
		bOK = RemoveFontResource(strFilename);

		ASSERT(bOK);
		if (!bOK)
			continue;
		
		// delete the scalable resource file
		bOK = DeleteFile(strFilename);
		ASSERT(bOK);
		
		if (!bTempCheck)
		{// check if TT files are in temp directory
			TCHAR szDrive[SC_MAX_UNC_DRIVE];
			TCHAR szDir[_MAX_DIR];
			SCSplitPath(strFilename, szDrive, szDir, NULL, NULL);
			CString strFilesDir;
			strFilesDir.Format(_T("%s%s"), szDrive, szDir);
			
			if (0==strTempPath.Compare(strFilesDir))
			{
				int iPos = strFilename.ReverseFind(_T('.'));
				if (iPos>0)
				{
					CString strTTFName = strFilename.Left(iPos) + SC_EMFX_TMPFONT_EXT;
					bTempFiles = SCExistFile(strTTFName);
				}
			}
			
			bTempCheck = TRUE;
		}

		// delete temporary font file
		if (bTempFiles)
		{
			int iPos = strFilename.ReverseFind(_T('.'));
			if (iPos>0)
			{
				CString strTTFName = strFilename.Left(iPos) + SC_EMFX_TMPFONT_EXT;
				bOK = SCRemoveFileAttributes(strTTFName, FILE_ATTRIBUTE_READONLY);
				ASSERT(bOK);

				bOK = DeleteFile(strTTFName);
				
				// Deceptively, Windows locks some fonts AFTER they have passed through
				// metafile playing, though they are OUR private fonts.
				// TODO: find reason.
				if (!bOK)
					m_LockedFontFiles.AddTail(strTTFName); // font to delete later
			}
		}
	}
	m_FontRes.RemoveAll();
}

///
/// Delete fonts that were locked in a previous attempt to delete them.
///
void SCEMFDoc::SCDeleteLockedFontFiles(BOOL bClosing/*=FALSE*/)
{
	POSITION pos = m_LockedFontFiles.GetHeadPosition();
	CString strFilename;

	if (bClosing)
	{// Just delete the files. They should be deleteable at this point.
		while(pos)
		{
			strFilename = m_LockedFontFiles.GetNext(pos);
			ASSERT(!strFilename.IsEmpty());

			BOOL bOK = SCRemoveFileAttributes(strFilename, FILE_ATTRIBUTE_READONLY);
			ASSERT(bOK);

			bOK = DeleteFile(strFilename);
			ASSERT(bOK);
		}
		return;
	}

	// Attemp to delete. Keep those files that are still locked.
	CStringList RemainList;
	while(pos)
	{
		strFilename = m_LockedFontFiles.GetNext(pos);
		ASSERT(!strFilename.IsEmpty());
		if (!DeleteFile(strFilename))
			RemainList.AddTail(strFilename);
	}
	m_LockedFontFiles.RemoveAll();
	pos = RemainList.GetHeadPosition();
	while(pos)
	{
		m_LockedFontFiles.AddTail(RemainList.GetNext(pos));
	}
}


///
/// Install a TrueType read-only embedded font from memory data
///
BOOL SCEMFDoc::SCInstallMemoryFont(CString strFaceName, BYTE* lpData, long lDataSize)
{
	// 1. check if its a TrueType

	// 2. avoid installing fonts on the system (copy their files
	// into temp directory)
	CString strTempPath;
	if (!::SCCreateTempDir(strTempPath, SC_EMFX_TEMPSUBDIR))
	{
		ASSERT(0);
		return FALSE;
	}

	int iPos = strFaceName.ReverseFind(_T('.'));
	if (-1==iPos)
		strFaceName += SC_EMFX_TMPFONT_EXT;
	else
		strFaceName = strFaceName.Left(iPos) + SC_EMFX_TMPFONT_EXT;

	CString strFilename = strTempPath + strFaceName;
#ifdef _DEBUG
	ASSERT(!SCExistFile(strFilename)); // it should not exist
#endif

	CFile file;
	CFileException fe;
	// create new file, 'Open' won't throw exception
	if (file.Open(strFilename, CFile::modeCreate |
	  CFile::modeWrite | CFile::shareExclusive, &fe))		
	{
		// save the data
		try
		{
			file.Write(lpData, lDataSize);
		}
		catch(CFileException* pEx)
		{
			pEx->Delete();
			ASSERT(0);
			file.Close();
			return FALSE;
		}

		file.Close();
	} else
	{
		ASSERT(0);
		return FALSE;
	}

	// 3. install and store filename
	return SCInstallFontFile(strFilename, TRUE);
}

///
/// Install a TrueType read-only embedded font from a file
///
BOOL SCEMFDoc::SCInstallFontFile(CString strFilename, BOOL bTemporary)
{
	CString strFontRes;
	int iPos = strFilename.ReverseFind(_T('.'));
	if (iPos>0)
	{
		strFontRes = strFilename.Left(iPos) + SC_EMFX_TMPFRES_EXT;
	} else
		strFontRes = strFilename + SC_EMFX_TMPFRES_EXT;
	
	DeleteFile(strFontRes);
	BOOL bCreated = CreateScalableFontResource(
		1,						// 0/1=flag for read-only embedded font
		LPCTSTR(strFontRes),	// pointer to filename for font resource
		LPCTSTR(strFilename),	// pointer to filename for scalable font
		LPCTSTR(NULL)    // pointer to path to font file
		);
	DWORD dwError = GetLastError();

	//ASSERT(bCreated); // Font file must be TrueType

	if (!bCreated)
	{
		if (bTemporary)
			DeleteFile(strFilename);
		return FALSE;
	}
	int iInstalled = AddFontResource(strFontRes);
	ASSERT(iInstalled);

	if (!iInstalled)
	{
		BOOL bOK = RemoveFontResource(strFontRes);
		ASSERT(bOK);
		if (bTemporary)
			DeleteFile(strFilename);
		return FALSE;
	}
	m_FontRes.AddTail(strFontRes);

	return TRUE;
}

void SCEMFDoc::SCNewDocument()
{
	SCBeginAddFiles(0);
	SCEndAddFiles();
}

UINT SCEMFDoc::SCOpenDocument(LPCTSTR lpszPathName)
{
	UINT uiFileType = SCEMFDoc::SCGetFileType(lpszPathName);
	switch(uiFileType & SC_FTYPE_MASK)
	{
	case SC_FTYPE_UKN :
		return uiFileType;

	case SC_FTYPE_BGP :
		SCLoadMaster(lpszPathName);
		break;
		
	case SC_FTYPE_EMF:
	case SC_FTYPE_WMF:
	case SC_FTYPE_EMZ:
	case SC_FTYPE_WMZ:
	case SC_FTYPE_IMG:
		SCLoadSinglePage(lpszPathName, uiFileType);
		break;

	case SC_FTYPE_TXT:
		if (!SCLoadRTFPages(lpszPathName, FALSE,
			(SC_SUBTYPE_TXT_RTF==(uiFileType & SC_SUBTYPE_MASK))))
			return SC_FTYPE_UKN;
		break;

	default:
		ASSERT(0);
	}

	return uiFileType;
}

BOOL SCEMFDoc::SCInsertFile(LPCTSTR lpszPathName, UINT* puiType/*=NULL*/)
{
	UINT uiFileType = SCEMFDoc::SCGetFileType(lpszPathName);
	if (puiType)
		*puiType = uiFileType;
	BOOL bInserted = FALSE;
	switch(uiFileType & SC_FTYPE_MASK)
	{
	case SC_FTYPE_UKN :
		return FALSE;

	case SC_FTYPE_BGP :
		// Merge isn't supported in this version
		//return SCLoadMaster(lpszPathName, TRUE);
		return FALSE;
		
	case SC_FTYPE_EMF:
	case SC_FTYPE_WMF:
	case SC_FTYPE_EMZ:
	case SC_FTYPE_WMZ:
	case SC_FTYPE_IMG:
		bInserted = SCLoadSinglePage(lpszPathName, uiFileType, TRUE);
		break;

	case SC_FTYPE_TXT:
		bInserted = SCLoadRTFPages(lpszPathName, TRUE,
			(SC_SUBTYPE_TXT_RTF==(uiFileType & SC_SUBTYPE_MASK)));
		break;

	default:
		ASSERT(0);
	}

	if (bInserted)
		m_bDocModified = TRUE;

	return bInserted;
}

///
/// Load the main data format supported by this class (enlist pages)
///
BOOL SCEMFDoc::SCLoadMaster(LPCTSTR lpszPathName, BOOL bInsert/*=FALSE*/)
{
	ASSERT(lpszPathName);

	ASSERT(FALSE==bInsert);// not supported yet

	// Note: in this version, absolute file names are not supported even if they appear
	// in the master. So directory information is stripped out.
	TCHAR szDocDrive[_MAX_DRIVE];	// drive of the playlist
	TCHAR szDocDir[_MAX_DIR];		// dir of the playlist
	TCHAR szFname[_MAX_FNAME];	// name of a file
	TCHAR szExt[_MAX_EXT];		// ext of a file
	_tsplitpath(lpszPathName, szDocDrive, szDocDir, NULL, NULL);
	
	TCHAR szBuff[MAX_PATH];
	// Number of files
	::GetPrivateProfileString(SC_DOC_GEN_SECTION, SC_DOC_NBFILES_KEY, _T(""),
		szBuff, MAX_PATH, lpszPathName);
	TCHAR szTxtBuff[SC_PGCOMMENT_MAXSIZE];
	
	UINT uiNbFiles = _ttoi(szBuff);
	SCBeginAddFiles(uiNbFiles, lpszPathName);
	for (UINT nFile=0; (nFile<uiNbFiles); nFile++)
	{
		CString strKey;
		strKey.Format(SC_DOC_FILEKEY_FORMAT, nFile);
		::GetPrivateProfileString(SC_DOC_FILES_SECTION, strKey, _T(""),
			szBuff, MAX_PATH, lpszPathName);
		
		_tsplitpath(szBuff, NULL, NULL, szFname, szExt);
		strKey.Format(_T("%s%s%s%s"), szDocDrive, szDocDir, szFname, szExt);
		SCAddFile(LPCTSTR(strKey), nFile, TRUE, (nFile>0));
		
		// Page attributes
		CString strPgSection;
		
		strPgSection.Format(SC_DOC_PGSECTION_FORMAT, nFile);
		::GetPrivateProfileString(strPgSection, SC_DOC_URL_KEY, _T(""),
			szTxtBuff, SC_PGCOMMENT_MAXSIZE, lpszPathName);
		SCSetPageURL(nFile, szTxtBuff);
		
		::GetPrivateProfileString(strPgSection, SC_DOC_CREDIT_KEY, _T(""),
			szTxtBuff, SC_PGCOMMENT_MAXSIZE, lpszPathName);
		SCSetPageCredit(nFile, szTxtBuff);
		
		::GetPrivateProfileString(strPgSection, SC_DOC_COMMENT_KEY, _T(""),
			szTxtBuff, SC_PGCOMMENT_MAXSIZE, lpszPathName);
		SCSetPageComment(nFile, szTxtBuff);
	}
	SCEndAddFiles();

	SCLoadSettings();
	return TRUE;
}

// Load a single file (enlist one file)
BOOL SCEMFDoc::SCLoadSinglePage(LPCTSTR lpszFname, UINT uiFyleType, BOOL bInsert/*=FALSE*/)
{
	ASSERT(lpszFname);
	ASSERT(uiFyleType!=SC_FTYPE_UKN);
	if (bInsert)
	{
		UINT uiIdx = m_uiNbFiles;
		if (!SCGrowBank(1))
			return FALSE;
		SCAddFile(lpszFname, uiIdx, TRUE, TRUE);
		m_vDocPages[uiIdx]->m_uiType = uiFyleType;
	} else
	{
		SCBeginAddFiles(1);
		SCAddFile(lpszFname, 0, TRUE, TRUE);
		m_vDocPages[0]->m_uiType = uiFyleType;
		SCEndAddFiles();
	}
	return TRUE;
}

///
/// Save the main data format supported by this class
///
BOOL SCEMFDoc::SCSaveDocument(LPCTSTR lpszPathName/*==NULL*/)
{
	CString strPathName = (lpszPathName) ? lpszPathName : m_strUniDocName;

	ASSERT(!strPathName.IsEmpty());
	CString strDocDir = SCMakeupDocDir(strPathName);

	CString strOldDocDir = SCMakeupDocDir(m_strUniDocName);
	BOOL bCopyFiles = (strDocDir!=strOldDocDir);

	// Main section
		// erase sections
	::WritePrivateProfileString(SC_DOC_GEN_SECTION, NULL, NULL, strPathName);
	::WritePrivateProfileString(SC_DOC_FILES_SECTION, NULL, NULL, strPathName);
		// ID
	::WritePrivateProfileString(SC_DOC_GEN_SECTION, SC_DOC_FILEID_KEY, SC_DOC_FILEID_VALUE, strPathName);
		// NbFiles
	SCWritePrivateProfileInt(SC_DOC_GEN_SECTION, SC_DOC_NBFILES_KEY, m_uiNbFiles, strPathName);

	// Files
	BOOL bOneDirty = FALSE;
	for (UINT uiPage=0; (uiPage<m_uiNbFiles); uiPage++)
	{
		PSCEMFDocPage pPage = m_vDocPages[uiPage];
		ASSERT(pPage);
		ASSERT(pPage->m_strPath);
		CString strPagePath;
		{// Note: in this version, absolute paths are not supported. Strip them out
			strPagePath = SCFNameExtFromPath(pPage->m_strPath);
			ASSERT(!strPagePath.IsEmpty());
		}

		// File EMF
		if (pPage->m_bDirty)
		{
			ASSERT(pPage->m_hEMF);
			if (pPage->m_hEMF)
			{
				CString strSavePath = strDocDir + strPagePath;
				BOOL bExist = SCExistFile(strSavePath);
				{// Note: in this version, bDirty means "new page"
					// Ensure that the file is unique
					int iPos = strSavePath.ReverseFind(_T('.'));
					CString strPrefix;
					CString strExt;
					if (iPos!=-1)
					{
						strPrefix = strSavePath.Left(iPos); // exclude dot
						strExt = strSavePath.Right(strSavePath.GetLength()-iPos); // include dot
					} else
					{
						ASSERT(0);
						strPrefix = strSavePath;
					}

					for (UINT i=1; (bExist && i<65635); i++)
					{
						strSavePath.Format(_T("%s_%d%s"), strPrefix, i, strExt);
						bExist = SCExistFile(strSavePath);
					}
					strPagePath = SCFNameExtFromPath(strSavePath);
					pPage->m_strPath = strPagePath;
				}
				if (SCWriteEMFtoDisk(pPage->m_hEMF, strSavePath))
					pPage->m_bDirty = FALSE;
			}
			if (pPage->m_bDirty)
			{
				bOneDirty = TRUE;
				continue;
			}
		} else
		if (bCopyFiles)
		{// copy file
			CString strSavePath = strDocDir + strPagePath;
			CString strOldPath = strOldDocDir + strPagePath;
			// Note: we are overwriting files!
			if (!::CopyFile(strOldPath, strSavePath, FALSE))
				bOneDirty = TRUE;
		}

		// File path
		CString strKey;
		strKey.Format(SC_DOC_FILEKEY_FORMAT, uiPage);
		::WritePrivateProfileString(SC_DOC_FILES_SECTION, strKey,
			strPagePath, strPathName);

		// Page attributes
		CString strPgSection;
		strPgSection.Format(SC_DOC_PGSECTION_FORMAT, uiPage);
		::WritePrivateProfileString(strPgSection, NULL,
							NULL, strPathName);

		if (!pPage->m_strCreditURL.IsEmpty())
			::WritePrivateProfileString(strPgSection, SC_DOC_URL_KEY,
							pPage->m_strCreditURL, strPathName);
		
		if (!pPage->m_strCredit.IsEmpty())
			::WritePrivateProfileString(strPgSection, SC_DOC_CREDIT_KEY,
							pPage->m_strCredit, strPathName);
		
		if (!pPage->m_strComment.IsEmpty())
			::WritePrivateProfileString(strPgSection, SC_DOC_COMMENT_KEY,
							pPage->m_strComment, strPathName);
	}

	// Settings
	SCSaveSettings(strPathName);

	// Is it clean?
	m_bDocModified = bOneDirty;
	m_bForgedName = FALSE;
	return (!m_bDocModified);
}

//////////////////////////////////////////////////////////////////////////////////
// Static

BOOL SCEMFDoc::SCIsNativeDoc(LPCTSTR lpszFname)
{
	TCHAR szBuff[MAX_PATH];
	::GetPrivateProfileString(SC_DOC_GEN_SECTION, SC_DOC_FILEID_KEY, _T(""), szBuff, MAX_PATH, lpszFname);
	CString strValue = szBuff;
	if (!strValue.IsEmpty() &&
		(strValue.CompareNoCase(SC_DOC_FILEID_VALUE)==0))
		return TRUE;
	
	return FALSE;
}

CString SCEMFDoc::SCDocFileFromList(CStringList& rLFiles)
{
	POSITION pos = rLFiles.GetHeadPosition();
	int iNumFiles = rLFiles.GetCount();
	if (1==iNumFiles)
	{// If one file, do not create playlist file
		return	rLFiles.GetNext(pos);
	}

	CString strDocName;
	WORD	nFound = 0;
    while (pos)
    { 
		CString strFile = rLFiles.GetNext(pos);
		if (SCEMFDoc::SCIsNativeDoc(strFile))
		{// playlist found in the group
			if (1==iNumFiles)
			{
				strDocName = strFile;
				break;
			}
			// dont mix: ignore playlist
			continue;
		}

		if (strDocName.IsEmpty())
		{// Build the doc name (new playlist) and write the number of files
			strDocName = SCEMFDoc::SCDocNameFromFilename(strFile);
			if (strDocName.IsEmpty())
				break; // could not create temp file

			// erase sections
			::WritePrivateProfileString(SC_DOC_GEN_SECTION, NULL, NULL, strDocName);
			::WritePrivateProfileString(SC_DOC_FILES_SECTION, NULL, NULL, strDocName);

			// ID
			::WritePrivateProfileString(SC_DOC_GEN_SECTION, SC_DOC_FILEID_KEY, SC_DOC_FILEID_VALUE, strDocName);
		}

		// Store the file path
		{
			CString strKey;
			strKey.Format(SC_DOC_FILEKEY_FORMAT, nFound);
			::WritePrivateProfileString(SC_DOC_FILES_SECTION, strKey, strFile, strDocName);
			nFound++;
		}
    }
	if (!strDocName.IsEmpty())
	{// Open the document
		// Update nb files only if a new playlist was created
		if (nFound)
		{
			ASSERT(nFound<=iNumFiles);
			CString strValue;
			strValue.Format(_T("%d"), nFound);
			::WritePrivateProfileString(SC_DOC_GEN_SECTION, SC_DOC_NBFILES_KEY, strValue, strDocName);
		}
	}
	return strDocName;
}

UINT SCEMFDoc::SCGetFileType(LPCTSTR lpszFname, BOOL bShowError/*=TRUE*/)
{
	{// Check special headers (bgp)
		TCHAR szBuff[MAX_PATH];
		::GetPrivateProfileString(SC_DOC_GEN_SECTION, SC_DOC_FILEID_KEY, _T(""), szBuff, MAX_PATH, lpszFname);
		CString strValue = szBuff;
		if (!strValue.IsEmpty() &&
			(strValue.CompareNoCase(SC_DOC_FILEID_VALUE)==0))
		return SC_FTYPE_BGP;
	}

	UINT uiMode = SetErrorMode(SEM_FAILCRITICALERRORS);
	// Open the file for reading. 
    HANDLE hFile = CreateFile(lpszFname, GENERIC_READ, FILE_SHARE_READ, NULL,  
                        OPEN_EXISTING, 0, 0); 
    if (hFile == INVALID_HANDLE_VALUE)
	{
		SetErrorMode(uiMode);
#if 1
		// TODO: add string to resource, or don't use it
		if (bShowError)
		{
			DWORD dwError = GetLastError();
			CString strMsg;
			strMsg.Format(_T("Unable to open %s"), lpszFname);
			SCShowWinError(dwError, SC_PRODUCTNAME, strMsg);
		}
#endif
        return (SC_FSTATE_NOTFOUND|SC_FTYPE_UKN);
	}

	UINT uiType = SC_FTYPE_UKN;

	// Load file type
	ENHMETAHEADER hdrBuff;
	DWORD dwRead;
	BOOL bResult;

	// wmf header
	bResult = ReadFile(hFile, &hdrBuff, 22, &dwRead, NULL);
	if (bResult && (22==dwRead))
	{// file is long enough
		BYTE* pdwKey = (BYTE*)&hdrBuff;
		if (*((DWORD*)pdwKey)==WMFMETA_PLACEABLEKEY)
			uiType = SC_FILETYPE_WMF; // Aldus wmf
		else
		if (EMR_HEADER==*((DWORD*)pdwKey))
		{// check emf header
			pdwKey += 22;
			bResult = ReadFile(hFile, pdwKey, sizeof(ENHMETAHEADER) - 22, &dwRead, NULL) ;
			if (bResult && (sizeof(ENHMETAHEADER) - 22==dwRead))
			{// file is long enough
				if (ENHMETA_SIGNATURE==hdrBuff.dSignature)
				{
					uiType = SC_FILETYPE_EMFONLY;
				}
			}
		}
		if (SC_FTYPE_UKN==uiType)
		{// check Windows wmf
			METAHEADER *pWinWMFHdr = (METAHEADER*)&hdrBuff;
			if ((1==pWinWMFHdr->mtType || 2==pWinWMFHdr->mtType) &&
				(0x0100==pWinWMFHdr->mtVersion || 0x0300==pWinWMFHdr->mtVersion))
			{
				uiType = SC_FILETYPE_WMF;
			}
		}
		if (SC_FTYPE_UKN==uiType && SCIsGZCandidate((LPBYTE)&hdrBuff, 22))
		{// check compressed Windows emf/wmf
			if (SCUnzipGZFilePart(lpszFname, (LPBYTE)&hdrBuff, sizeof(ENHMETAHEADER)))
			{
				pdwKey = (BYTE*)&hdrBuff;
				if (WMFMETA_PLACEABLEKEY==*((DWORD*)pdwKey))
					uiType = SC_FTYPE_WMZ; // Aldus wmf
				else
				if (EMR_HEADER==*((DWORD*)pdwKey) &&
					ENHMETA_SIGNATURE==hdrBuff.dSignature)
				{
					uiType = SC_FTYPE_EMZ; // emf
				} else
				{
					METAHEADER *pWinWMFHdr = (METAHEADER*)&hdrBuff;
					if ((1==pWinWMFHdr->mtType || 2==pWinWMFHdr->mtType) &&
						(0x0100==pWinWMFHdr->mtVersion || 0x0300==pWinWMFHdr->mtVersion))
					{
						uiType = SC_FTYPE_WMZ; // Windows wmf
					}
				}
			}
		}
	}

	CloseHandle(hFile);
	SetErrorMode(uiMode);
	if (uiType != SC_FTYPE_UKN)
		return uiType;

	// images
#pragma message( __FILE__  "(1129): TODO: Review image filetype  detection")
	// we would like a detection by content, not by extension
	uiType = SCFileTypeFromExt(lpszFname, TRUE);

	return uiType;
}

CString SCEMFDoc::SCDocNameFromFilename(LPCTSTR lpszFname)
{
	if (!SCIsWriteableMedia(lpszFname))
	{// If we can't write on the media, use a temp file
		TCHAR szPath[MAX_PATH];
		TCHAR szBuff[MAX_PATH];
		DWORD dwLength = ::GetTempPath(MAX_PATH, szPath);
		if ((dwLength>0)
			&& (dwLength<MAX_PATH)
			&& ::GetTempFileName(szPath, _T("EMX"), 0, szBuff))
		{
			CString strDocName = szBuff;
			return strDocName;
		}
		return _T("");
	}

	CString strBaseName = lpszFname;
	int iPos = strBaseName.ReverseFind(_T('.'));
	if (iPos!=-1)
		strBaseName = strBaseName.Left(iPos);
	CString strDocName = strBaseName + SC_DOC_EXTENSION;
	// Ensure that the file is unique
	for (UINT i=1; (i<65635); i++)
	{
		if (!SCExistFile(strDocName))
			return strDocName;
		strDocName.Format(_T("%s%d%s"), strBaseName, i, SC_DOC_EXTENSION);
	}
	return _T("");
}


