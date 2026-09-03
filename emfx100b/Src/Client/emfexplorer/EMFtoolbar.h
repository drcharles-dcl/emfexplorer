/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _EMFTOOLBAR_H_
#define _EMFTOOLBAR_H_

#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCEditBox.h)
#include SC_INC_WINLIB(SCZoomBox.h)
#include SC_INC_WINLIB(SCColorBox.h)

class CEMFToolBar : public CToolBar
{
// Construction / Destruction / Creation
public:
	CEMFToolBar::CEMFToolBar();
	CEMFToolBar::~CEMFToolBar();
	BOOL SCCreate(CWnd* pParentWnd);

// Attributes
public:
	// getters
	CSCEditBox&	SCGetPageBox() { return m_GotoPgBox; }
	CSCZoomBox&	SCGetZoomBox() { return m_ZoomBox; }

	// setters
	void SCSetNbPages(int iNbPages);

// Operation
public:
	void SCSetZoomMultiplier(int iMultiplier) { m_ZoomBox.SCSetFloatMultiplier(iMultiplier); }
	void SCSetZoomLimits(int iMin, int iMax) { m_ZoomBox.SCSetMinMax(iMin, iMax); }
	void SCSetZoomsList(PSCZoomEntry pZooms, USHORT usCount) { m_ZoomBox.SCSetDefaultList(pZooms, usCount); }
	void SCSetCurPage(int iPage);
	void SCSetCurZoom(int iZoom) { m_ZoomBox.SCSetCurValue(iZoom); }
	void SCSelectZoom(int iValue) { m_ZoomBox.SCSelectValue(iValue); }
	void SCSetCurColor(COLORREF crColor) { m_ColorBox.SCSetCurSelColor(crColor); }


	void SCEnableBoxes(BOOL bEnable=TRUE);
	void SCReset();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEMFToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	// special command routing to frame
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

protected:
	BOOL SCCreateChildren();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEMFToolBar)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

// data
private:
	int			m_iNbPages;		// total number of pages to manage
	CSCEditBox	m_GotoPgBox;	// child control managing manual 'goto page'
	CSCZoomBox  m_ZoomBox;		// child control managing manual 'set zoom'
	CSCColorBox m_ColorBox;		// child control managing manual 'set background color'
};

#endif //_EMFTOOLBAR_H_
//  ------------------------------------------------------------
