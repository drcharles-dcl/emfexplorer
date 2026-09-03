/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCEMFDOC_H_
#define _SCEMFDOC_H_

#include "SCGenInclude.h"
#include "SCFileTypes.h"
#include "SCEMF.h"
#include "SCGdiplusUtils.h"
#include "kSCEMFLibMsgs.h"
#include "SCEMFDocUtils.h"
#include "SCEMFDocPage.h"

//--------------------------------------------
// Document properties
typedef struct tagSCDocProperties
{
		//-- Control
	USHORT			usCtlBorder;	 // Border type
	USHORT			usCtlColorStyle;
	COLORREF		crCtlColor;
		//-- Page
	BOOL			bPageBorderVisible;
	BOOL			bPageShadowVisible;
	USHORT			usPaperColorStyle;
	long			crPaperColor;
	USHORT			usFitMode;
	int				iScale;
	long			lMarginL;
	long			lMarginT;
	long			lMarginR;
	long			lMarginB;
	USHORT			usPageOrientation;
	USHORT			usTransparencyMode;
	USHORT			usReverseVideoMode;
		//-- GDIp
	long			bEnableGDIp;
	USHORT			usTextRenderingHint;
	USHORT			usTextContrast;
	USHORT			usSmoothingMode;
	USHORT			usInterpolationMode;
	USHORT			usPixelOffsetMode;
		//-- Document
	BOOL			bSizeAllPages;
	CRect			rectDocument;
	CString			strCreditURL;
	CString			strCredit;

	tagSCDocProperties():
	usCtlBorder(SC_DFLT_CTLBORDER),
	usCtlColorStyle(SC_DFLT_CTLCOLORSTYLE),
	crCtlColor(SC_DFLT_CTLCOLOR),
		//--
	bPageBorderVisible(SC_DFLT_PAGEBORDERVISIBLE),
	bPageShadowVisible(SC_DFLT_PAGESHADOWVISIBLE),
	usPaperColorStyle(SC_DFLT_PAPERCOLORSTYLE),
	crPaperColor(SC_DFLT_PAPERCOLOR),
	usFitMode(SC_DFLT_FITMODE),
	iScale(SC_ZOOM100),
	lMarginL(SC_DFLT_MARGINL),
	lMarginT(SC_DFLT_MARGINT),
	lMarginR(SC_DFLT_MARGINR),
	lMarginB(SC_DFLT_MARGINB),
	usPageOrientation(SC_DFLT_PAGEORIENTATION),
	usTransparencyMode(SC_DFLT_TRANSPARENCYMODE),
	usReverseVideoMode(SC_DFLT_REVERSEVIDEOMODE),
		//--
	bEnableGDIp(TRUE),
	usTextRenderingHint(SC_DFLT_TEXTRENDERINGHINT),
	usTextContrast(SC_DFLT_TEXTCONTRAST),
	usSmoothingMode(SC_DFLT_SMOOTHINGMODE),
	usInterpolationMode(SC_DFLT_INTERPOLATIONMODE),
	usPixelOffsetMode(SC_DFLT_PIXELOFFSETMODE),
		//--
	bSizeAllPages(FALSE),
	rectDocument(0,0,0,0)
	{
	}

} SCDocProperties, *PSCDocProperties;

typedef std::vector<PSCEMFDocPage, std::allocator<PSCEMFDocPage> > DOCPAGEVECTOR;
typedef std::vector<int, std::allocator<int> > INTVECTOR;
//--------------------------------------------


class SCEMFDoc
{
// Construction/Destruction
public:
	SCEMFDoc();
	virtual ~SCEMFDoc();

// Accessors
	// Getters
		// Document
	UINT SCGetNbPages() const { return m_uiNbFiles; }
	BOOL SCGetDocModified() const	{ return m_bDocModified; }
	CString SCGetUniDocName() const { return m_strUniDocName; }
	BOOL SCGetResizeAll() const { return m_Properties.bSizeAllPages; }
	CString SCGetCredit() { return m_Properties.strCredit; }
	CString SCGetCreditURL() { return m_Properties.strCreditURL; }
	SCDocProperties& SCGetProperties() { return m_Properties; }
			//--I persistent properties----------------------------
	// CString SCGetPicture() const; // N/A: call SCGetUniDocName			
			// Control-related
	int SCGetCtlBorder() const;		
	int SCGetCtlColorStyle() const;	
	int SCGetCtlColor() const;
			// Page-related
	int SCGetPageBorderVisible() const;
	int SCGetPageShadowVisible() const;
	int SCGetPaperColorStyle() const;
	int SCGetPaperColor() const;
	int SCGetFitMode() const;		
	int SCGetScale() const;			
	int SCGetMarginL() const;
	int SCGetMarginT() const;
	int SCGetMarginR() const;
	int SCGetMarginB() const;
	int SCGetPageOrientation() const;
	int SCGetTransparencyMode() const;
	int SCGetReverseVideoMode() const;
			// GDI+-related
	int SCGetGDIplusEnabled() const;	
	int SCGetTextRenderingHint() const;
	int SCGetTextContrast() const;
	int SCGetSmoothingMode() const;
	int SCGetInterpolationMode() const;
	int SCGetPixelOffsetMode() const;
			//--end I-----------------------------------------------
		// Page
	BOOL SCIsPageDirty(UINT uiPage) const
	{
		ASSERT(uiPage<m_uiNbFiles);
		return (uiPage<m_uiNbFiles)? m_vDocPages[uiPage]->m_bDirty: FALSE;
	}
	UINT SCGetPageWidth(UINT uiPage) const
	{
		ASSERT(uiPage<m_uiNbFiles);
		return (uiPage<m_uiNbFiles)? RECT_WIDTH(m_vDocPages[uiPage]->m_rectSize): 0;
	}
	UINT SCGetPageHeight(UINT uiPage) const
	{
		ASSERT(uiPage<m_uiNbFiles);
		return (uiPage<m_uiNbFiles)? RECT_HEIGHT(m_vDocPages[uiPage]->m_rectSize): 0;
	}
	void SCGetElementsRect(UINT uiPage, CRect& rc) const 
	{
		ASSERT(uiPage<m_uiNbFiles);
		if (uiPage<m_uiNbFiles)
			rc = m_vDocPages[uiPage]->m_rectSize;
		else
			SetRectEmpty(&rc);
	}
	CString SCGetPageFName(UINT uiPage) const
	{
		ASSERT(uiPage<m_uiNbFiles);
		return (uiPage<m_uiNbFiles)? m_vDocPages[uiPage]->m_strPath: _T("");
	}
	HENHMETAFILE SCGetPageEMF(UINT uiPage);
	CString SCGetPageURL(UINT uiPage) const
	{
		PSCEMFDocPage pDocPage = SCGetCreditPage(uiPage);
		return (pDocPage && !pDocPage->m_strCreditURL.IsEmpty())?
				pDocPage->m_strCreditURL: m_Properties.strCreditURL;
	}
	CString SCGetPageCredit(UINT uiPage) const
	{
		PSCEMFDocPage pDocPage = SCGetCreditPage(uiPage);
		return (pDocPage && !pDocPage->m_strCredit.IsEmpty())?
				pDocPage->m_strCredit: m_Properties.strCredit;
	}
	CString SCGetPageComment(UINT uiPage) const
	{
		PSCEMFDocPage pDocPage = SCGetCreditPage(uiPage);
		return (pDocPage)? pDocPage->m_strComment: _T("");
	}

	// Setters
		// Document
	void SCSetUniDocName(LPCTSTR lpszFName) { m_strUniDocName = lpszFName; }
	void SCSetDocModified() { m_bDocModified = TRUE; }
	void SCSetProperties(SCDocProperties& rProps) { m_Properties = rProps; }

			//--I persistent properties----------------------------
	//	void SCSetPicture(LPCTSTR lpszPath); // N/A: call SCSetUniDocName	
			// Control-related
	void SCSetCtlBorder(int iBorderType);
	void SCSetCtlColorStyle(int iStyle);
	void SCSetCtlColor(int iColor);
			// Page-related
	void SCSetPageBorderVisible(int bVisible);
	void SCSetPageShadowVisible(int bVisible);
	void SCSetPaperColorStyle(int iStyle);
	void SCSetPaperColor(int iColor);
	void SCSetFitMode(int iMode);
	void SCSetScale(int iScale);
	void SCSetMarginL(int iLeft);
	void SCSetMarginT(int iTop);
	void SCSetMarginR(int iRight);
	void SCSetMarginB(int iBottom);
	void SCSetPageOrientation(int iAngle);
	void SCSetTransparencyMode(int iMode);
	void SCSetReverseVideoMode(int iMode);
			// GDI+-related
	void SCSetGDIplusEnabled(int bEnabled);
	void SCSetTextRenderingHint(int iHint);
	void SCSetTextContrast(int iLevel);
	void SCSetSmoothingMode(int iMode);
	void SCSetInterpolationMode(int iMode);
	void SCSetPixelOffsetMode(int iMode);
			//--end I-----------------------------------------------
		// Page
	void SCSetPageURL(UINT uiPage, LPCTSTR lpszURL);
	void SCSetPageCredit(UINT uiPage, LPCTSTR lpszCredit);
	void SCSetPageComment(UINT uiPage, LPCTSTR lpszComment);

// Operation
	void SCCleanup(BOOL bClosing = FALSE);
	void SCNewDocument();
	UINT SCOpenDocument(LPCTSTR lpszPathName);
	void SCSetDocDir(LPCTSTR strDocDir);

	BOOL SCSaveDocument(LPCTSTR lpszPathName=NULL);

	void SCResizeAllPages();
	BOOL SCPasteEMFPages(HEMFVECTOR& rVector);
	BOOL SCInsertFile(LPCTSTR lpszPathName, UINT* puiType=NULL);
	void SCRemovePage(UINT uiPage);
	BOOL SCReflow(DOCPAGEVECTOR& rvectReflow, INTVECTOR& rvectDescrip,
				  LPCTSTR lpszDocDir, int iDlftCrdPage);

	BOOL SCHasSettings();
	BOOL SCEditable();
	PSCEMFDocPage SCGetDocPage(UINT uiPage, BOOL bGetCopy=FALSE);
	HENHMETAFILE SCUnlockEMF(UINT uiPage, PSCEMFDocPage pShare=NULL);
	void SCInflateElemsRect(UINT uiPage, int iLeft, int iTop, int iRight, int iBottom);
	PSCEMFDocPage SCGetCreditPage(UINT uiPage) const
	{
		ASSERT(uiPage<m_uiNbFiles);
		PSCEMFDocPage pDocPage = m_vDocPages[uiPage];
		ASSERT(pDocPage);
		return pDocPage;
	}

// Implementation
protected:
	BOOL SCGrowBank(int iNbPages);
	void SCInstallFonts();
	void SCUnInstallFonts();
	void SCDeleteLockedFontFiles(BOOL bClosing=FALSE);
	BOOL SCInstallMemoryFont(CString strFaceName, BYTE* lpData, long lDataSize);
	BOOL SCInstallFontFile(CString strFilename, BOOL bTemporary);

	void SCBeginAddFiles(UINT iNbFiles, LPCTSTR lpszFName=NULL);
	BOOL SCAddFile(LPCTSTR lpszFname, UINT uiIdx, BOOL bCopyPath=TRUE, BOOL bDelayed=FALSE);
	void SCEndAddFiles();

	BOOL SCLoadSinglePage(LPCTSTR lpszFname, UINT uiFyleType, BOOL bInsert=FALSE);
	BOOL SCLoadMaster(LPCTSTR lpszPathName, BOOL bInsert=FALSE);
	BOOL SCLoadRTFPages(LPCTSTR lpszFname, BOOL bPaste=FALSE, BOOL bRTF=TRUE);

	void SCLoadSettings();
	void SCSaveSettings(LPCTSTR lpszFName);
	void SCForgeDocName(LPCTSTR lpszFname);

// Static
public:
	static BOOL SCIsNativeDoc(LPCTSTR lpszFname);
	static CString SCDocNameFromFilename(LPCTSTR lpszFname);
	static UINT SCGetFileType(LPCTSTR lpszFname, BOOL bShowError=TRUE);
	static CString SCDocFileFromList(CStringList& rLFiles);

private:
	DOCPAGEVECTOR	m_vDocPages;
	UINT			m_uiNbFiles;
	CString			m_strUniDocName; // name as if all files were one doc
	BOOL			m_bDocModified;
	BOOL			m_bForgedName;

	// Font management
	CStringList		m_FontRes;
	CStringList		m_LockedFontFiles;

	// Properties
	SCDocProperties m_Properties;
};

#endif //_SCEMFDOC_H_
//  ------------------------------------------------------------
