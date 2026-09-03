/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFDocPage.h"

#include "SCFileTypes.h"
#include "SCEMFDoc.h"

#include "SCGenInclude.h"
#include SC_INC_GENLIB(SCGenDefs.h)
#include SC_INC_SHARED(SCZipFile.h)
#include SC_INC_ERRLIB(wErr.h)

#include SC_INC_WINLIB(SCAlphaBand.h)
#include "SCGdiplusUtils.h"

// Page comment, credit ...
#define SC_DFLT_CMT_HEIGHT		(16*2)
#define SC_DFLT_CRD_HEIGHT		(SC_DFLT_CMT_HEIGHT/2)

#define SC_DFLT_CMT_FACE		_T("Times New Roman")
#define SC_DFLT_CMT_FAMILY		FF_ROMAN
#define SC_DFLT_CMT_SIZE		18
#define SC_DFLT_CMT_STYLE		SC_BAND_BOLD

#define SC_DFLT_CRD_FACE		SC_DFLT_CMT_FACE
#define SC_DFLT_CRD_FAMILY		SC_DFLT_CMT_FAMILY
#define SC_DFLT_CRD_SIZE		10
#define SC_DFLT_CRD_STYLE		(SC_BAND_BOLD|SC_BAND_ITALIC)
//--


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SCEMFDocPage::SCEMFDocPage(SCEMFDoc* pParent):
	m_pParentDoc(pParent),
	m_uiType(SC_FSTATE_DELAYED|SC_FTYPE_UKN),
	m_hEMF(NULL),
	m_bDirty(FALSE),
	m_rectSize(0,0,0,0),
	m_bInflate(FALSE),
	m_rectInflate(0,0,0,0),
	m_pPgComment(NULL),
	m_pPgCredit(NULL)
{
	ASSERT(m_pParentDoc);
}

SCEMFDocPage::~SCEMFDocPage()
{
	SMC_DELETE(m_pPgComment);
	SMC_DELETE(m_pPgCredit);
	if (m_hEMF)
		::DeleteEnhMetaFile(m_hEMF);
}

// Special, unsafe, stuff: Copy is not full.
// Both objects hold the EMF, one must detach it before any destructor is called.
void SCEMFDocPage::SCCopyFrom(const SCEMFDocPage& rDocPage)
{
	m_uiType = rDocPage.m_uiType;
	m_bDirty = rDocPage.m_bDirty;
	m_strPath = rDocPage.m_strPath;
	m_strCreditURL = rDocPage.m_strCreditURL;
	m_strCredit = rDocPage.m_strCredit;
	m_strComment = rDocPage.m_strComment;
	m_rectSize = rDocPage.m_rectSize;
	m_bInflate = rDocPage.m_bInflate;
	m_rectInflate = rDocPage.m_rectInflate;
	m_hEMF = rDocPage.m_hEMF;	// beware, or make copy
	//m_pPgComment = rDocPage.m_pPgComment;	// No! Or make full copy
	//m_pPgCredit = rDocPage.m_pPgCredit;	// No! Or make full copy
}
//

void SCEMFDocPage::SCReset()
{
	m_strPath.Empty();
	m_strCreditURL.Empty();
	m_strCredit.Empty();
	m_strComment.Empty();
	if (m_hEMF)
	{
		::DeleteEnhMetaFile(m_hEMF);
		m_hEMF = NULL;
	}
	m_bDirty = FALSE;
	m_uiType = (SC_FSTATE_DELAYED|SC_FTYPE_UKN);
	m_rectSize.SetRectEmpty();
	m_bInflate = FALSE;
	m_rectInflate.SetRectEmpty();
}

HENHMETAFILE SCEMFDocPage::SCDetachEMF()
{
	HENHMETAFILE hEMF = m_hEMF;
	m_hEMF = NULL;
	return hEMF;
}

void SCEMFDocPage::SCSetFilePath(LPCTSTR lpszPath, BOOL bKeepImage/*=TRUE*/)
{
	ASSERT(lpszPath);
	m_strPath = lpszPath;
	if (!bKeepImage && m_hEMF)
	{
		::DeleteEnhMetaFile(m_hEMF);
		m_hEMF = NULL;
		m_uiType = (SC_FSTATE_DELAYED|SC_FTYPE_UKN);
	}
}

void SCEMFDocPage::SCRecomputeElemsRect()
{
	if (m_hEMF)
	{		
		fnSCPlayEnhMetaFile fnPlayEMF = (SCIsGDIpEMFType(m_uiType))? SCGdipPlayMetafile :NULL;
		SCGetEMFElemsRect(m_hEMF, m_rectSize, fnPlayEMF);
	}
}

void SCEMFDocPage::SCGetPageBlackBox(CRect& rcElems) const
{
	SCGetElemsRect(rcElems);
	
	if (m_pPgComment)
		rcElems.InflateRect(0, m_pPgComment->SCGetSize().cy, 0, 0);
	if (m_pPgCredit)
		rcElems.InflateRect(0, 0, 0, m_pPgCredit->SCGetSize().cy);
}

void SCEMFDocPage::SCAttachEMF(HENHMETAFILE hEMF, LPCTSTR lpszPath, UINT uiType)
{
	ASSERT(hEMF);
	ASSERT(lpszPath);
	ASSERT(0==(uiType & ~SC_FTYPE_MASK));
	if (m_hEMF)
		::DeleteEnhMetaFile(m_hEMF);

	m_hEMF = hEMF;
	m_strPath = lpszPath;
	m_uiType = uiType;

	SCRecomputeElemsRect();
}

HENHMETAFILE SCEMFDocPage::SCGetPageEMF()
{
	if (!m_hEMF)
	{// load page
		switch (m_uiType & SC_FSTATE_MASK)
		{
		case SC_FSTATE_CLEARED:
			break;

		case SC_FSTATE_DELAYED:
			ASSERT(!m_strPath.IsEmpty());
			m_uiType = SCEMFDoc::SCGetFileType(m_strPath);
			if (SC_FSTATE_NOTFOUND==(m_uiType & SC_FSTATE_MASK))
			{// message already displayed
			} else
			if (SC_FTYPE_UKN==(m_uiType & SC_FTYPE_MASK))
			{
				CString strMsg;
				strMsg.Format(SC_EMFLIB_FMTERR_FILETYPE, m_strPath);
				AfxMessageBox(strMsg);
			} else
				break;
			// error
			m_uiType= (m_uiType & ~SC_FSTATE_MASK)|SC_FSTATE_ERROR;
			return NULL; // TODO: return an error EMF instead

		case SC_FSTATE_DOWNLOADING:
			// not supported in this version
			// debug
			SC_LOGERROR(_T("Download not supported in this vesion"));
			return NULL;
			
		case SC_FSTATE_ERROR:
			return NULL; // TODO: return an error EMF instead		
		}

		ASSERT(!m_strPath.IsEmpty());
		switch (m_uiType & SC_FTYPE_MASK)
		{
		case SC_FTYPE_EMF:
			m_hEMF = ::GetEnhMetaFile(m_strPath);
			{
				ASSERT(m_hEMF);
				switch (SCGdipGetEMFType(m_hEMF))
				{
				case EmfTypeEmfOnly:
					m_uiType = SC_FILETYPE_EMFONLY;
					//TODO:
					//SCPreFilterEMF(m_hEMF);
					break;
					
				case EmfTypeEmfPlusOnly:
					m_uiType = SC_FILETYPE_EMFPLUS;
					break ;
					
				case EmfTypeEmfPlusDual:
					m_uiType = SC_FILETYPE_EMFDUAL;
					break;
				}
			}
			break;

		case SC_FTYPE_WMF:
			m_hEMF = SCConvertWMFtoEMF(m_strPath);
			//ASSERT(NULL==m_hEMF || SCGdipGetEMFType(m_hEMF)==EmfTypeEmfOnly);
			// Windows XP was caught (wasting resources) storing private information in WMF.
			// Then SetWinMetaFileBits returned a dual EMF.
			break;
			
		case SC_FTYPE_EMZ:
		case SC_FTYPE_WMZ:
			{
				CMemFile MemFile;
				if (SCUnzipGZFile(m_strPath, MemFile))
				{
					DWORD dwLen = MemFile.GetLength();
					LPBYTE pBytes = MemFile.Detach();
					m_hEMF = ((m_uiType & SC_FTYPE_MASK)==SC_FTYPE_EMZ)?
								::SetEnhMetaFileBits(dwLen, pBytes)
							:
								SCConvertWMFtoEMF(dwLen, pBytes);
					delete [] pBytes;
				}
				ASSERT(m_hEMF);
			}
			break;
			
		case SC_FTYPE_IMG:
			m_hEMF = SCConvertImagetoEMF(m_strPath);
			ASSERT(NULL==m_hEMF || SCGdipGetEMFType(m_hEMF)==EmfTypeEmfOnly);
			break;

		default:
			// debug
			SC_LOGERROR1(_T("Page type error:%d"), m_uiType);
			return NULL;
		}
	
		SCRecomputeElemsRect();
	}

	return m_hEMF;
}

HENHMETAFILE SCEMFDocPage::SCUnlockEMF()
{
	if (!m_hEMF)
		return NULL;

	// Do it only if GetEnhMetaFile was called
	if (SC_FTYPE_EMF==(m_uiType & SC_FTYPE_MASK))
	{
		HENHMETAFILE hEmfCopy =	CopyEnhMetaFile(m_hEMF, NULL);
		DeleteEnhMetaFile(m_hEMF);
		m_hEMF = hEmfCopy;
	}
	return m_hEMF;
}

void SCEMFDocPage::SCComputePageInfo(BOOL bReverseVideo, CRect& rectMargins)
{
	SMC_SAFEDELETE(m_pPgComment);
	SMC_SAFEDELETE(m_pPgCredit);

	// check comments/credit
	CString strCredit = m_strCredit;
	if (strCredit.IsEmpty())
		strCredit = m_pParentDoc->SCGetCredit();
	if (m_strComment.IsEmpty())
	{
		if (strCredit.IsEmpty())
			return;
		m_pPgCredit = new SCAlphaBand;
	} else
	{
		m_pPgComment = new SCAlphaBand;
		if (!strCredit.IsEmpty())
			m_pPgCredit = new SCAlphaBand;
	}

	// compute unrotated page's black box
	CRect rcElemsRect;
	SCGetBasicElemsRect(rcElemsRect);
	rcElemsRect.InflateRect(rectMargins.left, rectMargins.top, rectMargins.right, rectMargins.bottom);

	CSize PgSize;
	PgSize.cx = rcElemsRect.Width();
	PgSize.cy = rcElemsRect.Height();
	
	int iCmtCx = 0;
	int iCmtCy = 0;
	int iCrdCy = 0;
	if (m_pPgComment)
	{
		m_pPgComment->SCSetFont(SC_DFLT_CMT_FACE, SC_DFLT_CMT_FAMILY, SC_DFLT_CMT_SIZE, SC_DFLT_CMT_STYLE|SC_BAND_SINGLELINE);
		m_pPgComment->SCSetLimits(CSize(PgSize.cx, SC_DFLT_CMT_HEIGHT), CSize(PgSize.cx, SC_DFLT_CMT_HEIGHT*3));
		m_pPgComment->SCSetText(m_strComment);
		iCmtCx = m_pPgComment->SCComputeSize(FALSE, FALSE);
		iCmtCy = max(m_pPgComment->SCGetSize().cy, SC_DFLT_CMT_HEIGHT);
	}
	
	if (m_pPgCredit)
	{
		m_pPgCredit->SCSetFont(SC_DFLT_CRD_FACE, SC_DFLT_CRD_FAMILY, SC_DFLT_CRD_SIZE, SC_DFLT_CRD_STYLE|SC_BAND_SINGLELINE);
		m_pPgCredit->SCSetLimits(CSize(PgSize.cx, SC_DFLT_CRD_HEIGHT), CSize(PgSize.cx, SC_DFLT_CRD_HEIGHT*3));
		m_pPgCredit->SCSetText(strCredit);
		iCmtCx = max(m_pPgCredit->SCComputeSize(FALSE, FALSE), iCmtCx);
		iCrdCy = max(m_pPgCredit->SCGetSize().cy, SC_DFLT_CRD_HEIGHT);
	}
	if (iCmtCx>PgSize.cx)
	{
		int iDltx = (iCmtCx - PgSize.cx)/2;
		SCInflateElemsRect(iDltx, 0, iDltx, 0);
		if (m_pPgComment)
			m_pPgComment->SCAdjustSize(iCmtCx, iCmtCy);
		if (m_pPgCredit)
			m_pPgCredit->SCAdjustSize(iCmtCx, iCrdCy);
	} else
	{
		if (m_pPgComment)
			m_pPgComment->SCAdjustSize(PgSize.cx, iCmtCy);
		if (m_pPgCredit)
			m_pPgCredit->SCAdjustSize(PgSize.cx, iCrdCy);
	}

	SCUpdatePageInfoColors(bReverseVideo);
}

void SCEMFDocPage::SCUpdatePageInfoColors(BOOL bReverseVideo)
{
	// TODO: use external settings
	COLORREF crBkColor = RGB(0, 0, 0);
	COLORREF crTxtColor = (bReverseVideo)? RGB(0, 0, 0) : RGB(255, 255, 255);
	if (m_pPgComment)
		m_pPgComment->SCSetColors(crBkColor, 128, crTxtColor);
	if (m_pPgCredit)
		m_pPgCredit->SCSetColors(crBkColor, 128, crTxtColor);
}

void SCEMFDocPage::SCDisplayPageComments(HDC hDC, float fScale,
										 int xDest, int yDest, int iWdt, int iHgt)
{
	CRect rect(xDest, yDest, xDest + iWdt, yDest + iHgt);
	if (m_pPgComment)
		m_pPgComment->SCDraw(hDC, rect, TRUE, FALSE, fScale);
	if (m_pPgCredit)
		m_pPgCredit->SCDraw(hDC, rect, FALSE, TRUE, fScale);
}

void SCEMFDocPage::SCDisplayPageComments(HDC hDC, float fScale,
										 int xDest, int yDest, int iWdt, int iHgt, HDC hAttribDC)
{
	CRect rect(xDest, yDest, xDest + iWdt, yDest + iHgt);
	if (m_pPgComment)
		m_pPgComment->SCDrawMeta(hDC, rect, TRUE, FALSE, fScale, hAttribDC);
	if (m_pPgCredit)
		m_pPgCredit->SCDrawMeta(hDC, rect, FALSE, TRUE, fScale, hAttribDC);
}

void SCEMFDocPage::SCDisplayPageComments(void* pGraphics, float fScale,
										 int xDest, int yDest, int iWdt, int iHgt)
{
	CRect rect(xDest, yDest, xDest + iWdt, yDest + iHgt);
	if (m_pPgComment)
		m_pPgComment->SCDraw((Gdiplus::Graphics*)pGraphics, rect, TRUE, FALSE, fScale);
	if (m_pPgCredit)
		m_pPgCredit->SCDraw((Gdiplus::Graphics*)pGraphics, rect, FALSE, TRUE, fScale);
}


