/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _CSCPREVIEWVIEW_H_
#define _CSCPREVIEWVIEW_H_

#include "afxpriv.h"

class CSCPreviewView : public CPreviewView  
{
	DECLARE_DYNCREATE(CSCPreviewView)
public:
	CSCPreviewView();
	virtual ~CSCPreviewView();

	//{{AFX_VIRTUAL(CSCPreviewView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CSCPreviewView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif //_CSCPREVIEWVIEW_H_
//  ------------------------------------------------------------
