/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCGDIPTOOLBAR_H_
#define _SCGDIPTOOLBAR_H_

#include "SCGenInclude.h"
#include SC_INC_EMFLIB(SCGdiplusUtils.h)
#include SC_INC_WINLIB(SCTbComboBox.h)
#include SC_INC_WINLIB(SCWinGUI.h)

class CSCGDIpToolBar : public CToolBar
{
// Construction / Destruction / Creation
public:
	CSCGDIpToolBar::CSCGDIpToolBar();
	CSCGDIpToolBar::~CSCGDIpToolBar();
	BOOL SCCreate(CWnd* pParentWnd, UINT uiRes);

// Attributes
public:
	// getters
	SCGDIpDrawingAttributes* SCGetDrawingAttributes() { return m_pDrawingAttributes; }

	// setters
	void SCSetDrawingAttributes(SCGDIpDrawingAttributes* pDrawingAttributes);

// Operation
public:
	void SCReset();
	void SCEnableControls(BOOL bEnable=TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCGDIpToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	// special command routing to frame
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);

protected:
	BOOL SCCreateChildren();
	BOOL SCSetHorizontal();
	BOOL SCSetVertical();
	void SCUpdateControls();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSCGDIpToolBar)
	afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	SC_DECLARE_ENABLE_CONTROL()

// data
private:
	CSCTbComboBox				m_TextRenderingBox;
	CSCTbComboBox				m_CurveSmoothingBox;
	CSCTbComboBox				m_ImageSamplingBox;
	CSCTbComboBox				m_ImagePixelsOffsBox;
	CSliderCtrl					m_TextContrastSlider;
	SCGDIpDrawingAttributes*	m_pDrawingAttributes;
	BOOL						m_bVertical;
};

#endif //_SCGDIPTOOLBAR_H_
//  ------------------------------------------------------------
