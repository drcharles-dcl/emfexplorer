/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "emfexplorer.h"
#include "SCDocReflow.h"

#include SC_INC_WINLIB(SCWinFile.h)
#include SC_INC_COMMON(kSCProdDefs.h)
#include SC_INC_ERRLIB(wErr.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Be careful! Resource editor is unaware of this ID
#define IDC_PICT_ALLPAGES_ACTUAL	(IDC_PICT_ALLPAGES+100)

/////////////////////////////////////////////////////////////////////////////
// CSCDocReflow dialog


CSCDocReflow::CSCDocReflow(CWnd* pParent /*=NULL*/)
	: CDialog(CSCDocReflow::IDD, pParent),
	m_pEMFDoc(NULL),
	m_uiNbPages(0),
	m_ThumbsSize(32, 32),
	m_iNbToRename(0),
	m_iDfltCrdPage(-1)
{
	//{{AFX_DATA_INIT(CSCDocReflow)
	m_strEdPagePos = _T("");
	m_strEdURL = _T("");
	m_strEdCurFile = _T("");
	m_strEdCredit = _T("");
	m_strEdComment = _T("");
	m_strDocDir = _T("");
	m_ChkAsDefault = FALSE;
	//}}AFX_DATA_INIT
}

void CSCDocReflow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSCDocReflow)
	DDX_Control(pDX, IDC_PICT_CURPAGE, m_PictCurPage);
	DDX_Control(pDX, IDC_LST_PAGES, m_LstPages);
	DDX_Text(pDX, IDC_EDIT_PAGE_POS, m_strEdPagePos);
	DDX_Text(pDX, IDC_EDIT_URL, m_strEdURL);
	DDX_Text(pDX, IDC_EDIT_CURFILE, m_strEdCurFile);
	DDX_Text(pDX, IDC_EDIT_CREDIT, m_strEdCredit);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_strEdComment);
	DDX_Text(pDX, IDC_EDIT_DOC_DIR, m_strDocDir);
	DDX_Check(pDX, IDC_CHK_DEFAULT, m_ChkAsDefault);
	//}}AFX_DATA_MAP
}

SC_IMPLEMENT_ENABLE_CONTROL(CSCDocReflow)

BEGIN_MESSAGE_MAP(CSCDocReflow, CDialog)
	//{{AFX_MSG_MAP(CSCDocReflow)
	ON_LBN_SELCHANGE(IDC_LST_PAGES, OnSelchangeLstPages)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_TOP, OnBtnTop)
	ON_BN_CLICKED(IDC_BTN_BOTTOM, OnBtnBottom)
	ON_BN_CLICKED(IDC_BTN_LEFT, OnBtnLeft)
	ON_BN_CLICKED(IDC_BTN_RIGHT, OnBtnRight)
	ON_BN_CLICKED(IDC_BTN_UP, OnBtnUp)
	ON_BN_CLICKED(IDC_BTN_DOWN, OnBtnDown)
	ON_BN_CLICKED(IDC_BTN_REPLACE, OnBtnReplace)
	ON_BN_CLICKED(IDC_BTN_REMOVE, OnBtnRemove)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAdd)
	ON_EN_KILLFOCUS(IDC_EDIT_CURFILE, OnKillfocusEditCurfile)
	ON_EN_KILLFOCUS(IDC_EDIT_CREDIT, OnKillfocusEditCredit)
	ON_EN_KILLFOCUS(IDC_EDIT_URL, OnKillfocusEditUrl)
	ON_EN_KILLFOCUS(IDC_EDIT_COMMENT, OnKillfocusEditComment)
	ON_EN_KILLFOCUS(IDC_EDIT_PAGE_POS, OnKillfocusEditPagePos)
	ON_BN_CLICKED(IDC_CHK_DEFAULT, OnChkDefault)
	ON_BN_CLICKED(IDC_BTN_VALIDATE, OnBtnValidate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCDocReflow I_SCThumbsHolder implementation

HENHMETAFILE CSCDocReflow::SCGetThumbEMF(int iThumbIdx)
{
	ASSERT(iThumbIdx>=0 && iThumbIdx<m_LstPages.GetCount());
	ASSERT(m_pEMFDoc);
	if (!m_pEMFDoc)
		return NULL;

	SCPageInfo* pInfo = (SCPageInfo*)m_LstPages.GetItemDataPtr(iThumbIdx);
	PSCEMFDocPage pDocPage = pInfo->pPage;
	ASSERT(pDocPage);

	return pDocPage->SCGetPageEMF();
}

void CSCDocReflow::SCSetThumbsSize(CSize& size)
{
	m_ThumbsSize = size;
}

PSCThumbImage CSCDocReflow::SCGetThumbImage(int iThumbIdx)
{
	ASSERT(iThumbIdx>=0 && iThumbIdx<m_LstPages.GetCount());
	SCPageInfo* pInfo = (SCPageInfo*)m_LstPages.GetItemDataPtr(iThumbIdx);
	PSCThumbImage pImage = pInfo->pThumb;
	if (pImage)
		return pImage;

	HENHMETAFILE hemf = SCGetThumbEMF(iThumbIdx);
	if (!hemf)
		return NULL;
	
#ifdef SC_UNSCALED_THUMBS
	pImage = new Gdiplus::Metafile(hemf);
#else
	// hope this will speed up painting
	Gdiplus::Metafile meta(hemf);
	GReal fScaleXY = (GReal)meta.GetWidth()/(GReal)meta.GetHeight();
	GReal height = (GReal)m_ThumbsSize.cy;
	GReal width = height*fScaleXY;
	if (width>m_ThumbsSize.cx)
	{
		width = (GReal)m_ThumbsSize.cx;
		height = width/fScaleXY;
	}
	pImage = meta.GetThumbnailImage((UINT)width, (UINT)height, NULL, NULL);
#endif
	
	pInfo->pThumb = pImage;
	return pImage;
}

CString CSCDocReflow::SCGetAddReplaceFilename(LPCTSTR lpszFName, UINT& uiFileType, LPCTSTR lpszTitle/*=NULL*/)
{
	CString strPath = _T("");
	uiFileType = SC_FTYPE_UKN;

	CString sFilters(SC_EMFMODE_FILTERS_REPLACE);
	CFileDialog dlg(TRUE, NULL, lpszFName, OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
		(LPCTSTR)sFilters);
	dlg.m_ofn.lpstrTitle = lpszTitle;
	if (dlg.DoModal() != IDOK)
		return strPath;

	strPath = dlg.GetPathName();
	uiFileType = SCEMFDoc::SCGetFileType(strPath);
	
	if (SC_FTYPE_UKN==uiFileType)
	{
		AfxMessageBox(IDS_FILE_UNKNOWN);
		strPath = _T("");
	}
	return strPath;
}


/////////////////////////////////////////////////////////////////////////////
// CSCDocReflow utilities

void CSCDocReflow::SCCleanup()
{
}

void CSCDocReflow::SCCreateThumbsControl()
{
	CWnd* pWnd = GetDlgItem(IDC_PICT_ALLPAGES);
	CRect rcPict;
	pWnd->GetWindowRect(&rcPict);
	::MapWindowPoints(NULL, m_hWnd, LPPOINT(&rcPict), 2);

	RECT rc;
	::CopyRect(&rc, &rcPict);
	if (m_PictAllPages.Create(NULL, _T("SCWinThumbs"),
		WS_VISIBLE|WS_CHILD|WS_BORDER|WS_TABSTOP, rc, this, IDC_PICT_ALLPAGES_ACTUAL))
	{
		HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		if (hFont)
		{
			LOGFONT LogFont;
			::GetObject(hFont, sizeof(LogFont), &LogFont);

			CFont* pFont = new CFont;
			pFont->CreateFontIndirect(&LogFont);
			
			m_PictAllPages.SCSetFont(pFont);
		}
	}
}

void CSCDocReflow::SCInitControls()
{
	if (m_pEMFDoc)
	{
		m_strDocDir = SCMakeupDocDir(m_pEMFDoc->SCGetUniDocName());
		//m_iDfltCrdPage = m_pEMFDoc->SCGetDefaultCreditPageIndex();

		m_uiNbPages = m_pEMFDoc->SCGetNbPages();
		for (UINT i=0; (i<m_uiNbPages); i++)
		{
			SCPageInfo* pInfo = new SCPageInfo;
			pInfo->iOrigIndex = (int)i;
			pInfo->pPage = m_pEMFDoc->SCGetDocPage(i, TRUE);
			
			int nIndex = m_LstPages.AddString(SCFNameExtFromPath(pInfo->pPage->SCGetFilePath()));
			m_LstPages.SetItemDataPtr(nIndex, pInfo);
		}
		m_PictAllPages.SCSetThumbsHolder(this, m_uiNbPages);
		m_LstPages.SetCurSel(0);
		SCSelChanged();  // won't be called automatically
	}
}

void CSCDocReflow::SCEnableControls()
{
	int idx = m_LstPages.GetCurSel();
	SCEnableControl(IDC_BTN_TOP, (idx > 0));
	SCEnableControl(IDC_BTN_UP, (idx >= m_PictAllPages.SCGetNbPgCols()));
	SCEnableControl(IDC_BTN_DOWN, (idx + m_PictAllPages.SCGetNbPgCols() <= (int)m_uiNbPages - 1));
	SCEnableControl(IDC_BTN_LEFT, (idx > 0));
	SCEnableControl(IDC_BTN_RIGHT, (idx < (int)m_uiNbPages - 1));
	SCEnableControl(IDC_BTN_BOTTOM, (idx < (int)m_uiNbPages - 1));
	SCEnableControl(IDC_EDIT_PAGE_POS, (m_uiNbPages>1));

	SCEnableControl(IDC_BTN_REMOVE, (m_uiNbPages>0));
	SCEnableControl(IDC_BTN_REPLACE, (m_uiNbPages>0));
	SCEnableControl(IDC_EDIT_CURFILE, (m_uiNbPages>0));
	SCEnableControl(IDC_EDIT_CREDIT, (m_uiNbPages>0));
	SCEnableControl(IDC_EDIT_URL, (m_uiNbPages>0));
	SCEnableControl(IDC_EDIT_COMMENT, (m_uiNbPages>0));

	SCEnableControl(IDC_CHK_DEFAULT, (m_uiNbPages>0));
	SCEnableControl(IDC_BTN_VALIDATE, (m_uiNbPages>0));
}

void CSCDocReflow::SCOnValidateCtlChange(int iCtl) 
{
	int idx = m_LstPages.GetCurSel();
	if (-1==idx)
		return;

	SCPageInfo* pInfo = (SCPageInfo*)m_LstPages.GetItemDataPtr(idx);
	ASSERT(pInfo);

	PSCEMFDocPage pDocPage = pInfo->pPage;
	ASSERT(pDocPage);
	switch (iCtl)
	{
	case IDC_EDIT_CURFILE:
		break;

	case IDC_EDIT_CREDIT:
		pDocPage->SCSetCredit(m_strEdCredit);
		break;

	case IDC_EDIT_URL:
		pDocPage->SCSetCreditURL(m_strEdURL);
		break;

	case IDC_EDIT_COMMENT:
		pDocPage->SCSetComment(m_strEdComment);
		break;

	case IDC_EDIT_PAGE_POS:
	default:
		ASSERT(0);
	}
}

void CSCDocReflow::SCSelChanged(BOOL bNotifyThumbs/*=TRUE*/) 
{
	int idx = m_LstPages.GetCurSel();
	if (-1==idx)
	{
		m_strEdPagePos = _T("");
		m_strEdURL = _T("");
		m_strEdCurFile = _T("");
		m_strEdCredit = _T("");
		m_strEdComment = _T("");
		m_PictCurPage.SCSetEMF(NULL);
		m_iDfltCrdPage = -1;
		m_ChkAsDefault = FALSE;

		SCEnableControls();
		UpdateData(FALSE);
		return;
	}

	SCPageInfo* pInfo = (SCPageInfo*)m_LstPages.GetItemDataPtr(idx);
	ASSERT(pInfo);

	PSCEMFDocPage pDocPage = pInfo->pPage;
	ASSERT(pDocPage);
	m_strEdPagePos.Format(_T("%d"), idx);
	m_strEdCurFile = SCFNameExtFromPath(pDocPage->SCGetFilePath());
	m_strEdURL = pDocPage->SCGetCreditURL();
	m_strEdCredit = pDocPage->SCGetCredit();
	m_strEdComment = pDocPage->SCGetComment();
	m_ChkAsDefault = (idx==m_iDfltCrdPage);
	if (!m_ChkAsDefault &&
		m_iDfltCrdPage>=0)
	{
		if (m_strEdURL.IsEmpty() &&
			m_strEdCredit.IsEmpty() &&
			m_strEdComment.IsEmpty())
		{
			SCPageInfo* pInfo = (SCPageInfo*)m_LstPages.GetItemDataPtr(m_iDfltCrdPage);
			ASSERT(pInfo);
			PSCEMFDocPage pDocPage = pInfo->pPage;
			ASSERT(pDocPage);
			m_strEdURL = pDocPage->SCGetCreditURL();
			m_strEdCredit = pDocPage->SCGetCredit();
			m_strEdComment = pDocPage->SCGetComment();
		}
	}

	m_PictCurPage.SCSetEMF(SCGetThumbEMF(idx));
	if (bNotifyThumbs)
		m_PictAllPages.SCSetCurSel(idx);

	SCEnableControls();
	UpdateData(FALSE);
}

void CSCDocReflow::SCMoveSelection(int iDestPos, BOOL bAbsolute/*=FALSE*/)
{
	int idx = m_LstPages.GetCurSel();
	if (-1==idx)
		return;

	if (!bAbsolute)
		iDestPos += idx;

	if (iDestPos==idx)
		return;
	ASSERT(iDestPos>=0 && iDestPos<(int)m_uiNbPages);

	if (idx==m_iDfltCrdPage)
		m_iDfltCrdPage = iDestPos;
	else
	if (idx<m_iDfltCrdPage)
	{
		if (iDestPos>m_iDfltCrdPage)
			m_iDfltCrdPage--;
	} else
	{
		if (iDestPos<=m_iDfltCrdPage)
			m_iDfltCrdPage++;
	}

	// Copy source info
	CString strSrc;
	m_LstPages.GetText(idx, strSrc);
	SCPageInfo* pSrcInfo = (SCPageInfo*)m_LstPages.GetItemDataPtr(idx);
	ASSERT(pSrcInfo);

	// Prepare move
	int iNewSel = iDestPos;
	if (iDestPos>idx)
		iDestPos++; // remove src (shift left)
	else
		idx++; // insert dest (shift right)
	
	// Move list elements
	int nIndex = m_LstPages.InsertString(iDestPos, strSrc);
	m_LstPages.SetItemDataPtr(nIndex, pSrcInfo);
	m_LstPages.DeleteString(idx);

	// Refresh
	m_LstPages.SetCurSel(iNewSel);
	SCSelChanged();
}

/////////////////////////////////////////////////////////////////////////////
// CSCDocReflow message handlers

BOOL CSCDocReflow::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Extra initialization here
	SCCreateThumbsControl();
	SCInitControls();
	SCEnableControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSCDocReflow::OnSelchangeLstPages() 
{
	SCSelChanged();
}

void CSCDocReflow::OnDestroy() 
{
	SCCleanup();
	int iNbElems = m_LstPages.GetCount();
	for (int i=0; i<iNbElems; i++)
	{
		SCPageInfo* pInfo = (SCPageInfo*)m_LstPages.GetItemDataPtr(i);
		if (pInfo)
			delete pInfo;
	}
	m_LstPages.ResetContent();

	CDialog::OnDestroy();
}

BOOL CSCDocReflow::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (CDialog::OnNotify(wParam, lParam, pResult))
		return TRUE;

	NMHDR* pNmhdr = (NMHDR*)lParam;
	switch (pNmhdr->idFrom)
	{
	case IDC_PICT_ALLPAGES_ACTUAL:
		if (THN_SELCHANGE==pNmhdr->code)
		{
			int idx = m_PictAllPages.SCGetCurSel();
			ASSERT(idx>=0 && idx<m_LstPages.GetCount());
			m_LstPages.SetCurSel(idx);
			SCSelChanged(FALSE);
			return TRUE;
		}

	//default:
		// let Windows manage it
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
///
/// Attempt to disable VK_RETURN, except for IDOK and IDCANCEL.
///
BOOL CSCDocReflow::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
        HWND hFocus = ::GetFocus();
		// detect the page pos edit box
		int iCtlId = (hFocus) ? ::GetDlgCtrlID(hFocus) : -1;
		switch (iCtlId)
		{
		case IDC_EDIT_PAGE_POS:
			{
				UpdateData();
				int iPos = _ttoi(LPCTSTR(m_strEdPagePos));
				if (iPos>=0 && iPos<(int)m_uiNbPages)
					SCMoveSelection(iPos, TRUE);
				else
				{
					m_strEdPagePos.Format(_T("%d"), m_LstPages.GetCurSel());
					UpdateData(FALSE);
				}
				return FALSE;    // Don't translate
			}
			break;

		case IDC_EDIT_CURFILE:
			{
				UpdateData();
				SCCheckRename();
				return FALSE;    // Don't translate
			}
			break;
			
		case IDOK:
		case IDCANCEL:
			break;
			
		case IDC_EDIT_CREDIT:
		case IDC_EDIT_URL:
		case IDC_EDIT_COMMENT:
			UpdateData();
			SCOnValidateCtlChange(iCtlId);
			// fall through

		default:
			return FALSE;    // Don't translate
		}
	};
	
	return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////

void CSCDocReflow::OnBtnTop() 
{
	SCMoveSelection(0, TRUE);	
}

void CSCDocReflow::OnBtnBottom() 
{
	SCMoveSelection(m_uiNbPages-1, TRUE);	
}

void CSCDocReflow::OnBtnLeft() 
{
	SCMoveSelection(-1);	
}

void CSCDocReflow::OnBtnRight() 
{
	SCMoveSelection(1);	
}

void CSCDocReflow::OnBtnUp() 
{
	SCMoveSelection(-m_PictAllPages.SCGetNbPgCols());	
}

void CSCDocReflow::OnBtnDown() 
{
	SCMoveSelection(m_PictAllPages.SCGetNbPgCols());	
}


void CSCDocReflow::SCCheckRename() 
{
	int iNewSel = m_LstPages.GetCurSel();
	ASSERT(-1!=iNewSel);

	CString strOldPath;
	m_LstPages.GetText(iNewSel, strOldPath);

	CString strNewPath = SCFNameExtFromPath(m_strEdCurFile);
	if (0==strNewPath.CompareNoCase(strOldPath))
		return;

	SCPageInfo* pInfo = (SCPageInfo*)m_LstPages.GetItemDataPtr(iNewSel);
	ASSERT(pInfo);
	SCEMFDocPage* pDocPage = pInfo->pPage;
	ASSERT(pDocPage);
	pDocPage->SCSetFilePath(m_strDocDir + strNewPath); // just renaming
	if (pInfo->strOrigName.IsEmpty())
	{// keep old name on first rename
		pInfo->strOrigName = strOldPath;
		if (SCExistFile(m_strDocDir + strOldPath))
			m_iNbToRename++;
	} else
	if (SCExistFile(m_strDocDir + pInfo->strOrigName))
	{// would be renamed
		if (0==strNewPath.CompareNoCase(pInfo->strOrigName))
		{// restored
			pInfo->strOrigName.Empty();
			m_iNbToRename--;
		}
	}
	
	// Move list elements
	int nIndex = m_LstPages.InsertString(iNewSel, strNewPath);
	m_LstPages.SetItemDataPtr(nIndex, pInfo);
	m_LstPages.DeleteString(iNewSel + 1);

	// Refresh
	m_LstPages.SetCurSel(iNewSel);
	SCSelChanged();
}

void CSCDocReflow::OnBtnReplace() 
{
	UpdateData();
	UINT uiFileType;
	CString strPath = SCGetAddReplaceFilename(m_strDocDir+m_strEdCurFile, uiFileType, _T("Replace File"));
	if (strPath.IsEmpty())
		return;

	if (!m_strDocDir.IsEmpty())
	{
		CString strDir = SCMakeupDocDir(strPath);
		if (0!=strDir.CompareNoCase(m_strDocDir))
		{
			AfxMessageBox(IDS_FILEMIX_ERR);
			return;
		}
	}

	HENHMETAFILE hEMF = SCEMFFromFile(strPath, uiFileType & SC_FTYPE_MASK);
	if (!hEMF)
	{
		AfxMessageBox(IDS_FILE_REPLACE_ERR);
		return;
	}

	// Copy source info
	int iNewSel = m_LstPages.GetCurSel();
	ASSERT(-1!=iNewSel);

	SCPageInfo* pInfo = (SCPageInfo*)m_LstPages.GetItemDataPtr(iNewSel);
	ASSERT(pInfo);
	SCEMFDocPage* pDocPage = pInfo->pPage;
	ASSERT(pDocPage);
	if (pInfo->pThumb)
	{
		delete pInfo->pThumb;
		pInfo->pThumb = NULL;
	}
	if (pInfo->iOrigIndex>=0)
	{
		pDocPage->SCDetachEMF();
		pInfo->iOrigIndex = -1;
	}
	pDocPage->SCAttachEMF(hEMF, strPath, uiFileType);

	// Move list elements
	int nIndex = m_LstPages.InsertString(iNewSel, SCFNameExtFromPath(strPath));
	m_LstPages.SetItemDataPtr(nIndex, pInfo);
	m_LstPages.DeleteString(iNewSel + 1);

	// Refresh
	m_LstPages.SetCurSel(iNewSel);
	SCSelChanged();
}

void CSCDocReflow::OnBtnRemove() 
{
	int idx = m_LstPages.GetCurSel();
	if (-1==idx)
		return;
	ASSERT(m_uiNbPages>0);
	if (0==m_uiNbPages)
		return;

	if (idx==m_iDfltCrdPage)
		m_iDfltCrdPage = -1;
	else
	if (idx<m_iDfltCrdPage)
		m_iDfltCrdPage--;

	// Copy source info
	SCPageInfo* pSrcInfo = (SCPageInfo*)m_LstPages.GetItemDataPtr(idx);
	ASSERT(pSrcInfo);
	if (!pSrcInfo->strOrigName.IsEmpty())
	{// would be renamed
		if (SCExistFile(m_strDocDir + pSrcInfo->strOrigName))
			m_iNbToRename--;
	}

	// Prepare remove
	int iNewSel = idx;
	m_uiNbPages--;
	if (iNewSel >= (int)m_uiNbPages)
		iNewSel--;
	
	// Delete element
	m_LstPages.DeleteString(idx);
	delete pSrcInfo;

	// Refresh
	m_PictAllPages.SCSetThumbsHolder(this, m_uiNbPages);
	m_LstPages.SetCurSel(iNewSel);
	SCSelChanged();
}

void CSCDocReflow::OnBtnAdd() 
{
	CStringList strLFiles;
	if (!SCGetFilesInsertList(strLFiles, m_strDocDir+m_strEdCurFile, TRUE))
		return;

	if (strLFiles.IsEmpty())
	{
		return;
		AfxMessageBox(IDS_EMFXMERGE_ERR);
	}
	// filter out, txt/rtf, etc..., and file in other directories
	BOOL bSetDir = m_strDocDir.IsEmpty();
	SCFilterDocDirFileList(strLFiles, m_strDocDir, TRUE);
	bSetDir = (bSetDir && !m_strDocDir.IsEmpty());
	if (bSetDir)
		UpdateData(FALSE);

	if (strLFiles.IsEmpty())
	{
		AfxMessageBox(IDS_REFLOWADDFILES_ERR);
		return;
	}

	POSITION pos = strLFiles.GetHeadPosition();
	while (pos)
	{
		SCPageInfo* pInfo = new SCPageInfo;
		CString strPgPath = strLFiles.GetNext(pos);
		SCEMFDocPage* pDocPage = new SCEMFDocPage(m_pEMFDoc);
		pInfo->pPage = pDocPage;
		pDocPage->SCSetFilePath(strPgPath); // just init
		
		int nIndex = m_LstPages.AddString(SCFNameExtFromPath(strPgPath));
		m_LstPages.SetItemDataPtr(nIndex, pInfo);
	}
	
	m_uiNbPages += strLFiles.GetCount();
	m_PictAllPages.SCSetThumbsHolder(this, m_uiNbPages);

	m_LstPages.SetCurSel(m_uiNbPages-1);
	SCSelChanged();
}

void CSCDocReflow::SCSetCtlText(UINT idCtl, LPCTSTR lpszText)
{
	CWnd* pWnd = GetDlgItem(idCtl);
	ASSERT(pWnd);
	if (pWnd)
		pWnd->SetWindowText(lpszText);
}

void CSCDocReflow::OnKillfocusEditCurfile() 
{
//		SCSetCtlText(IDC_EDIT_CURFILE, m_strEdCurFile);
}

void CSCDocReflow::OnKillfocusEditCredit() 
{
//		SCSetCtlText(IDC_EDIT_CREDIT, m_strEdCredit);
}

void CSCDocReflow::OnKillfocusEditUrl() 
{
//		SCSetCtlText(IDC_EDIT_URL, m_strEdURL);
}

void CSCDocReflow::OnKillfocusEditComment() 
{
//		SCSetCtlText(IDC_EDIT_COMMENT, m_strEdComment);
}

void CSCDocReflow::OnKillfocusEditPagePos() 
{
//		SCSetCtlText(IDC_EDIT_PAGE_POS, m_strEdPagePos);
}

void CSCDocReflow::OnChkDefault() 
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHK_DEFAULT);
	ASSERT(pBtn);
	if (pBtn)
	{
		if (pBtn->GetCheck())
		{
			m_iDfltCrdPage = m_LstPages.GetCurSel();
		} else
		{
			m_iDfltCrdPage = -1;
		}
//			SCSelChanged();
	}
}

void CSCDocReflow::OnBtnValidate() 
{
	UpdateData();
	int idx = m_LstPages.GetCurSel();
	if (-1==idx)
		return;

	SCPageInfo* pInfo = (SCPageInfo*)m_LstPages.GetItemDataPtr(idx);
	ASSERT(pInfo);

	PSCEMFDocPage pDocPage = pInfo->pPage;
	ASSERT(pDocPage);
	pDocPage->SCSetCredit(m_strEdCredit);
	pDocPage->SCSetCreditURL(m_strEdURL);
	pDocPage->SCSetComment(m_strEdComment);
	SCCheckRename();
}

void CSCDocReflow::OnOK() 
{
	OnBtnValidate();
	BOOL bReflow = FALSE;
	UINT uiNbElems = m_LstPages.GetCount();
	if (m_iNbToRename)
	{
		if (IDYES!=AfxMessageBox(IDS_REFLOW_CONFIRM_RENAME, MB_YESNOCANCEL))
			return;
	}

	DOCPAGEVECTOR		vCurrent;
	INTVECTOR			vDescrip;
	int iNbMoveErr = 0;
	for (UINT idx=0; (idx<uiNbElems); idx++)
	{
		SCPageInfo* pInfo = (SCPageInfo*)m_LstPages.GetItemDataPtr(idx);
		ASSERT(pInfo);
		
		PSCEMFDocPage pDocPage = pInfo->pPage;
		ASSERT(pDocPage);

		vCurrent.push_back(pDocPage);
		vDescrip.push_back(pInfo->iOrigIndex);

		if (!pInfo->strOrigName.IsEmpty())
		{// rename now
			if (m_strDocDir.IsEmpty())
				continue;

			CString strOldPath = m_strDocDir + pInfo->strOrigName;
			if (!SCExistFile(strOldPath))
				continue;

			// XP may lock the file
			if (pInfo->iOrigIndex>=0)
			{
				m_pEMFDoc->SCUnlockEMF(pInfo->iOrigIndex, pDocPage);
			} else
				pDocPage->SCUnlockEMF();
			//

			if (!MoveFile(strOldPath, pDocPage->SCGetFilePath()))
			{
				DWORD dwError = GetLastError();
				CString strMsg;
				strMsg.Format(IDS_FILE_RENAME_ERR, strOldPath, pDocPage->SCGetFilePath());
				SCShowWinError(dwError, SC_PRODUCTNAME, strMsg);

				iNbMoveErr++;
			} else
			{
				m_iNbToRename--;
				pInfo->strOrigName.Empty();
			}
		}
	}
	if (iNbMoveErr)
	{
		AfxMessageBox(IDS_FILE_RENAME_ERRLAST);
		return;
	}
	m_pEMFDoc->SCReflow(vCurrent, vDescrip, m_strDocDir, m_iDfltCrdPage);

	{// pre-clean up
		for (UINT idx=0; (idx<uiNbElems); idx++)
		{
			SCPageInfo* pInfo = (SCPageInfo*)m_LstPages.GetItemDataPtr(idx);
			ASSERT(pInfo);
			pInfo->pPage = NULL; // don't delete page, as it is now owned by doc
		}
	}

	CDialog::OnOK();
}



