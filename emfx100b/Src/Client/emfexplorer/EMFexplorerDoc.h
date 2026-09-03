/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#if !defined(AFX_EMFEXPLORERDOC_H__30EF366F_B6E8_4E32_A5F3_112609E66941__INCLUDED_)
#define AFX_EMFEXPLORERDOC_H__30EF366F_B6E8_4E32_A5F3_112609E66941__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SCGenInclude.h"
#include SC_INC_EMFLIB(SCEMFDoc.h)

class CEMFexplorerItem;
class CInPlaceFrame;
class CEMFexplorerDoc : public COleServerDoc
{
protected: // create from serialization only
	CEMFexplorerDoc();
	DECLARE_DYNCREATE(CEMFexplorerDoc)

// Attributes
public:
	// for in-place activation
	CEMFexplorerItem* GetEmbeddedItem()
		{ return (CEMFexplorerItem*)COleServerDoc::GetEmbeddedItem(); }
	// for active document
	CDocObjectServer* GetDocObjectServer(LPOLEDOCUMENTSITE pSite);

	SCEMFDoc*  GetEMFDoc() { return &m_EMFDoc; }
	CInPlaceFrame* SCGetIPFrameWnd() { return (CInPlaceFrame*)m_pInPlaceFrame; }

// Operations
public:
	void SCUpdateTitle(UINT uiPage);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEMFexplorerDoc)
	protected:
	virtual COleServerItem* OnGetEmbeddedItem();
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEMFexplorerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CView* CEMFexplorerDoc::SCGetCurrentView() 
	{
		CView* pView = NULL;
		POSITION pos = GetFirstViewPosition();
		while (pos != NULL)
		{
			if (pView = GetNextView(pos))
				return pView;
		}
		return NULL;
	}

protected:
	virtual COleIPFrameWnd* CreateInPlaceFrame(CWnd* pParentWnd);
// Generated message map functions
protected:
	//{{AFX_MSG(CEMFexplorerDoc)
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTbdocResizeAll(CCmdUI* pCmdUI);
	afx_msg void OnTbdocResizeAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	DECLARE_OLECMD_MAP()

private:
	SCEMFDoc	m_EMFDoc;
	CString		m_strDocTitle;
	UINT		m_uiDocOrigType;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMFEXPLORERDOC_H__30EF366F_B6E8_4E32_A5F3_112609E66941__INCLUDED_)
