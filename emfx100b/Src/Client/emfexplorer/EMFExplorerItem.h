/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#if !defined(AFX_EMFEXPLORERITEM_H__7E99C527_021A_4BBB_8651_D213668EB00F__INCLUDED_)
#define AFX_EMFEXPLORERITEM_H__7E99C527_021A_4BBB_8651_D213668EB00F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEMFexplorerItem : public CDocObjectServerItem
{
	DECLARE_DYNAMIC(CEMFexplorerItem)

// Constructors
public:
	CEMFexplorerItem(CEMFexplorerDoc* pContainerDoc);

// Attributes
	CEMFexplorerDoc* GetDocument() const
		{ return (CEMFexplorerDoc*)CDocObjectServerItem::GetDocument(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEMFexplorerItem)
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
	//}}AFX_VIRTUAL

// Implementation
public:
	~CEMFexplorerItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMFEXPLORERITEM_H__7E99C527_021A_4BBB_8651_D213668EB00F__INCLUDED_)
