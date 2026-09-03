/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "EMFexplorer.h"
#include "SCGenInclude.h"
#include SC_INC_ERRLIB(wErr.h)
#include SC_INC_COMMON(kSCProdDefs.h)

#include "EMFexplorerDoc.h"
#include "EMFexplorerView.h"
#include "EMFexplorerItem.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerDoc

IMPLEMENT_DYNCREATE(CEMFexplorerDoc, COleServerDoc)

BEGIN_MESSAGE_MAP(CEMFexplorerDoc, COleServerDoc)
	//{{AFX_MSG_MAP(CEMFexplorerDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_TBDOC_RESIZEALL, OnUpdateTbdocResizeAll)
	ON_COMMAND(ID_TBDOC_RESIZEALL, OnTbdocResizeAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Active document implementation
BEGIN_OLECMD_MAP(CEMFexplorerDoc, COleServerDoc)
	ON_OLECMD_PAGESETUP()
	ON_OLECMD_PRINT()
END_OLECMD_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerDoc construction/destruction

CEMFexplorerDoc::CEMFexplorerDoc():
	m_uiDocOrigType(SC_FTYPE_UKN)
{
	// add one-time construction code here
	// Note: this doc is reused by MFC
}

CEMFexplorerDoc::~CEMFexplorerDoc()
{
}

BOOL CEMFexplorerDoc::OnNewDocument()
{
	if (!COleServerDoc::OnNewDocument())
		return FALSE;

	// reinitialization code here
	// (SDI documents will reuse this document)
	m_EMFDoc.SCNewDocument();

	m_strDocTitle = _T("");
	m_uiDocOrigType = SC_FTYPE_UKN;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerDoc server implementation

COleServerItem* CEMFexplorerDoc::OnGetEmbeddedItem()
{
	// OnGetEmbeddedItem is called by the framework to get the COleServerItem
	//  that is associated with the document.  It is only called when necessary.

	CEMFexplorerItem* pItem = new CEMFexplorerItem(this);
	ASSERT_VALID(pItem);
	return pItem;
}

// Active document implementation
CDocObjectServer* CEMFexplorerDoc::GetDocObjectServer(LPOLEDOCUMENTSITE pSite)
{
	return new CDocObjectServer(this, pSite);
}

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerDoc serialization

void CEMFexplorerDoc::Serialize(CArchive& ar)
{
//		if (ar.IsStoring())
//		{
//			// add storing code here
//		}
//		else
//		{
//			// add loading code here
//		}
}

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerDoc diagnostics

#ifdef _DEBUG
void CEMFexplorerDoc::AssertValid() const
{
	COleServerDoc::AssertValid();
}

void CEMFexplorerDoc::Dump(CDumpContext& dc) const
{
	COleServerDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerDoc commands

BOOL CEMFexplorerDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!lpszPathName)
		return COleServerDoc::OnOpenDocument(lpszPathName);

	// Init sub doc
	UINT uiFileType = m_EMFDoc.SCOpenDocument(lpszPathName);
	if (SC_FTYPE_UKN==(uiFileType & SC_FTYPE_MASK))
	{
		if (SC_FSTATE_NOTFOUND!=(uiFileType & SC_FSTATE_MASK))
		{
			CString strMsg;
			strMsg.Format(IDS_FILE_LOADTYPE_ERR, SC_PRODUCTNAME);
#ifdef _DEBUG
			strMsg += CString(_T("\n")) + lpszPathName;
#endif
			AfxMessageBox(strMsg);
		}
		return FALSE;
	}

	if (!COleServerDoc::OnOpenDocument(lpszPathName))
	{
		CString strMsg;
		strMsg.LoadString(IDS_FILE_LOAD_ERR);
		SCShowWinError(GetLastError(), SC_PRODUCTNAME, strMsg);
		return FALSE;
	}

	m_strDocTitle = _T("");
	m_uiDocOrigType = uiFileType;
	return TRUE;
}

BOOL CEMFexplorerDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	if (!lpszPathName)
		return COleServerDoc::OnSaveDocument(lpszPathName);

	CString strDocDir = SCMakeupDocDir(m_EMFDoc.SCGetUniDocName());
	CString strNewDocDir = SCMakeupDocDir(lpszPathName);
	if (strNewDocDir!=strDocDir)
	{// file movement is required
		if (IDYES!=AfxMessageBox(IDS_FILE_CONFIRM_COPY, MB_YESNOCANCEL))
			return FALSE;
	}

	BeginWaitCursor();
	BOOL bOk = m_EMFDoc.SCSaveDocument(lpszPathName);
	EndWaitCursor();

	if (bOk)
	{
		SetModifiedFlag(FALSE);
		return TRUE;
	}
	return FALSE;
}

void CEMFexplorerDoc::OnFileSave() 
{
	CString strPath = GetPathName();

	CDocument::OnFileSave();

	CString strNewPath = GetPathName();
	if (strPath!=strNewPath)
	{
		m_strDocTitle = GetTitle();
		m_EMFDoc.SCSetUniDocName(strNewPath);
	}

	// We need to update title bar
	CEMFexplorerView* pView = (CEMFexplorerView*)SCGetCurrentView();
	ASSERT(pView);
	if (!pView)
		return;
	SCUpdateTitle(pView->SCGetCurPage()-1);
}

void CEMFexplorerDoc::OnFileSaveAs() 
{
	CString strPath = GetPathName();

	CDocument::OnFileSaveAs();

	CString strNewPath = GetPathName();
	if (strPath!=strNewPath)
	{
		m_strDocTitle = GetTitle();
		m_EMFDoc.SCSetUniDocName(strNewPath);
	}

	// We need to update title bar
	CEMFexplorerView* pView = (CEMFexplorerView*)SCGetCurrentView();
	ASSERT(pView);
	if (!pView)
		return;
	SCUpdateTitle(pView->SCGetCurPage()-1);
}

void CEMFexplorerDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_EMFDoc.SCGetNbPages()>0);	
}

void CEMFexplorerDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	OnUpdateFileSave(pCmdUI);	
}

void CEMFexplorerDoc::OnUpdateTbdocResizeAll(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_EMFDoc.SCGetNbPages()>1);
	pCmdUI->SetCheck(m_EMFDoc.SCGetResizeAll());
}

void CEMFexplorerDoc::OnTbdocResizeAll() 
{
	m_EMFDoc.SCResizeAllPages();
	UpdateAllViews(NULL);
}

void CEMFexplorerDoc::SCUpdateTitle(UINT uiPage)
{
	if (m_strDocTitle.IsEmpty())
	{
		if (SC_FTYPE_BGP!=m_uiDocOrigType)
		{// Document has been converted.
		 // Force master doc name to "Untitled"
			m_strPathName = _T("");
			m_strTitle.LoadString(AFX_IDS_UNTITLED);
		}
		m_strDocTitle = GetTitle();
	}

	int iNbPages = m_EMFDoc.SCGetNbPages();
	if (0==iNbPages)
	{
		ASSERT(0==uiPage);
		SetTitle(m_strDocTitle);
	} else
	{
		ASSERT(uiPage<m_EMFDoc.SCGetNbPages());
		CString strPath = m_EMFDoc.SCGetPageFName(uiPage);
		TCHAR szFname[_MAX_FNAME];	// name of a file
		TCHAR szExt[_MAX_EXT];		// ext of a file
		_tsplitpath(strPath, NULL, NULL, szFname, szExt);
		
		CString strTitle;
		strTitle.Format(_T("%s%s - [%s%s]"),
			m_strDocTitle, (IsModified()?_T("*"):_T("")), szFname, szExt);
		SetTitle(strTitle);
	}
}

COleIPFrameWnd* CEMFexplorerDoc::CreateInPlaceFrame(CWnd* pParentWnd)
{
	COleIPFrameWnd* pOleIPFrameWnd = COleServerDoc::CreateInPlaceFrame(pParentWnd);
	if (m_pOrigParent && m_pOrigParent->IsKindOf(RUNTIME_CLASS(CMainFrame)))
	{
		ASSERT_KINDOF(CInPlaceFrame, pOleIPFrameWnd);
		((CMainFrame*)m_pOrigParent)->SCSetOleIPFrameWnd((CInPlaceFrame*)pOleIPFrameWnd);
		((CInPlaceFrame*)pOleIPFrameWnd)->SCSetOrigFrame((CMainFrame*)m_pOrigParent);
	}
	return pOleIPFrameWnd;
}


