/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCIMGSTATIC_H_
#define _SCIMGSTATIC_H_

#include "unknwn.h"		// or GDI+ won't let us compile
#include "gdiplus.h"

/////////////////////////////////////////////////////////////////////////////
// CSCDocReflow dialog

class CSCImgStatic : public CStatic
{
// Construction
public:
	CSCImgStatic();

// Attributes
public:
	// setters
	void SCSetImage(Gdiplus::Image* pImage)	{	m_pImage = pImage; }
	void SCSetEMF(HENHMETAFILE hemf);

	// getters
	Gdiplus::Image* SCGetImage()			{	return m_pImage; }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCImgStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSCImgStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSCImgStatic)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	Gdiplus::Image*	m_pImage;
	Gdiplus::REAL	m_fWidth;
	Gdiplus::REAL	m_fHeight;
};


#endif //_SCIMGSTATIC_H_
//  ------------------------------------------------------------
