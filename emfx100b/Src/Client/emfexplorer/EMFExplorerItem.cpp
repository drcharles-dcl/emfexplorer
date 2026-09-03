/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "EMFexplorer.h"

#include "EMFexplorerDoc.h"
#include "EMFexplorerItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerItem implementation

IMPLEMENT_DYNAMIC(CEMFexplorerItem, CDocObjectServerItem)

CEMFexplorerItem::CEMFexplorerItem(CEMFexplorerDoc* pContainerDoc)
	: CDocObjectServerItem(pContainerDoc, TRUE)
{
	//	Add one-time construction code here
	//  (eg, adding additional clipboard formats to the item's data source)
}

CEMFexplorerItem::~CEMFexplorerItem()
{
	// Add cleanup code here
}

void CEMFexplorerItem::Serialize(CArchive& ar)
{
	// CEMFexplorerItem::Serialize will be called by the framework if
	//  the item is copied to the clipboard.  This can happen automatically
	//  through the OLE callback OnGetClipboardData.  A good default for
	//  the embedded item is simply to delegate to the document's Serialize
	//  function.  If you support links, then you will want to serialize
	//  just a portion of the document.

	if (!IsLinkedItem())
	{
		CEMFexplorerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->Serialize(ar);
	}
}

BOOL CEMFexplorerItem::OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize)
{
	// Most applications, like this one, only handle drawing the content
	//  aspect of the item.  If you wish to support other aspects, such
	//  as DVASPECT_THUMBNAIL (by overriding OnDrawEx), then this
	//  implementation of OnGetExtent should be modified to handle the
	//  additional aspect(s).

	if (dwDrawAspect != DVASPECT_CONTENT)
		return CDocObjectServerItem::OnGetExtent(dwDrawAspect, rSize);

	// CEMFexplorerItem::OnGetExtent is called to get the extent in
	//  HIMETRIC units of the entire item.  The default implementation
	//  here simply returns a hard-coded number of units.

	CEMFexplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: replace this arbitrary size
#if 0
	rSize = pDoc->GetDocSize();   // return HIMETRIC units
#else
	rSize = CSize(200, 300); 
	CClientDC dc(NULL);
	
	// use a mapping mode based on logical units
	//    (we can't use MM_LOENGLISH because MM_LOENGLISH uses 
	//    physical inches)
	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetViewportExt(dc.GetDeviceCaps(LOGPIXELSX),             
		dc.GetDeviceCaps(LOGPIXELSY));
	dc.SetWindowExt(100, -100);
	dc.LPtoHIMETRIC(&rSize); 
#endif
	return TRUE;
}

BOOL CEMFexplorerItem::OnDraw(CDC* pDC, CSize& rSize)
{
	// Remove this if you use rSize
	UNREFERENCED_PARAMETER(rSize);

	CEMFexplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: set mapping mode and extent
	//  (The extent is usually the same as the size returned from OnGetExtent)
#if 0
	CSize sizeDoc = pDoc->GetDocSize();
#else
	CSize sizeDoc(200, 300);
	sizeDoc.cy = -sizeDoc.cy;
#endif
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(sizeDoc);
	pDC->SetWindowOrg(0, 0);

	// TODO: add drawing code here.  Optionally, fill in the HIMETRIC extent.
	//  All drawing takes place in the metafile device context (pDC).

	pDC->TextOut(10, -20, _T("Do not insert EMFexplorer documents"));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CEMFexplorerItem diagnostics

#ifdef _DEBUG
void CEMFexplorerItem::AssertValid() const
{
	CDocObjectServerItem::AssertValid();
}

void CEMFexplorerItem::Dump(CDumpContext& dc) const
{
	CDocObjectServerItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
