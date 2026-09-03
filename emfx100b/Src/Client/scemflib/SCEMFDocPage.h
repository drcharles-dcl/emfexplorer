/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFDOCPAGE_H_
#define _SCEMFDOCPAGE_H_


class SCEMFDoc;
class SCAlphaBand;

class SCEMFDocPage
{
// Construction/Destruction
public:
	SCEMFDocPage(SCEMFDoc* pParent);
	~SCEMFDocPage();

// Accessors
	// Getters
	UINT SCGetFileType() const		{ return m_uiType; }
	HENHMETAFILE SCGetEMF() const	{ return m_hEMF; }
	BOOL SCGetDirty() const			{ return m_bDirty; }
	CString SCGetCreditURL() const	{ return m_strCreditURL; }
	CString SCGetCredit() const		{ return m_strCredit; }
	CString SCGetComment() const	{ return m_strComment; }
	CString SCGetFilePath() const	{ return m_strPath; }
	BOOL SCGetInflate()  const		{ return m_bInflate; }
	void SCGetElemsRect(CRect& rcElems) const { rcElems = m_rectSize; }
	void SCGetInflateRect(CRect& rectInflate) const { rectInflate = m_rectInflate; }
	void SCGetPageBlackBox(CRect& rcElems) const;

	// Setters
	void SCSetCreditURL(LPCTSTR lpszURL)	{ ASSERT(lpszURL); m_strCreditURL = lpszURL; }
	void SCSetCredit(LPCTSTR lpszCredit)	{ ASSERT(lpszCredit); m_strCredit = lpszCredit; }
	void SCSetComment(LPCTSTR lpszComment)	{ ASSERT(lpszComment); m_strComment = lpszComment; }

// Operation
	void SCReset();
	void SCCopyFrom(const SCEMFDocPage& rDocPage);
	HENHMETAFILE SCDetachEMF();
	void SCSetFilePath(LPCTSTR lpszPath, BOOL bKeepImage=TRUE);
	void SCAttachEMF(HENHMETAFILE hEMF, LPCTSTR lpszPath, UINT uiType);
	HENHMETAFILE SCGetPageEMF();
	HENHMETAFILE SCUnlockEMF();
	void SCInflateElemsRect(int iLeft, int iTop, int iRight, int iBottom)
	{
		m_bInflate = TRUE;
		// don't cumulate inflates
		m_rectSize.InflateRect(iLeft - m_rectInflate.left, iTop - m_rectInflate.top,
							   iRight - m_rectInflate.right, iBottom - m_rectInflate.bottom);
		m_rectInflate.SetRect(iLeft, iTop, iRight, iBottom);
	}
	void SCRecomputeElemsRect();
	void SCGetBasicElemsRect(CRect& rcElems) const
	{
		rcElems = m_rectSize;
		rcElems.InflateRect(-m_rectInflate.left, -m_rectInflate.top,
							-m_rectInflate.right, -m_rectInflate.bottom);
	}

// Implementation
	void SCComputePageInfo(BOOL bReverseVideo, CRect& rectMargins);
	void SCUpdatePageInfoColors(BOOL bReverseVideo);
	void SCDisplayPageComments(HDC hDC, float fScale,
		int xDest, int yDest, int iWdt, int iHgt);
	void SCDisplayPageComments(HDC hDC, float fScale,
		int xDest, int yDest, int iWdt, int iHgt, HDC hAttribDC);
	void SCDisplayPageComments(void* pGraphics, float fScale,
		int xDest, int yDest, int iWdt, int iHgt);

protected:

private:
	SCEMFDoc*		m_pParentDoc;	// parent doc
	UINT			m_uiType;		// file type of page
	HENHMETAFILE	m_hEMF;			// EMF of page
	BOOL			m_bDirty;		// image is new or was altered
	CRect			m_rectSize;		// page dimensions
	CString			m_strPath;		// path to page
	CString			m_strCreditURL;	// this is informative (not for download)
	CString			m_strCredit;	// credit
	CString			m_strComment;	// any text
	BOOL			m_bInflate;		// page-specific inflate on/off
	CRect			m_rectInflate;	// the specific inflate
	SCAlphaBand*	m_pPgComment;	// Page's comment
	SCAlphaBand*	m_pPgCredit;	// Page's credit

friend class SCEMFDoc;

private:
	// Control proliferation
	/*No default =*/void operator=(const SCEMFDocPage &rCopy);
	/*No default copy*/SCEMFDocPage(const SCEMFDocPage &rCopy);
};

typedef SCEMFDocPage *PSCEMFDocPage;

#endif //_SCEMFDOCPAGE_H_
//  ------------------------------------------------------------
