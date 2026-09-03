/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCWINTHUMBS_H_
#define _SCWINTHUMBS_H_

#include "unknwn.h"		// or GDI+ won't let us compile
#include "gdiplus.h"

#include "SCMemDC.h"
#include "SCGenInclude.h"
#include SC_INC_GENLIB(SCGenDefs.h)


#define GReal		Gdiplus::REAL
#define GGraphics	Gdiplus::Graphics

#define THN_SELCHANGE	0x8000 // user-defined notification

typedef Gdiplus::Image* PSCThumbImage;

class SMC_INTERFACE I_SCThumbsHolder
{
public:
	virtual void SCSetThumbsSize(CSize& size) = 0;
	virtual PSCThumbImage SCGetThumbImage(int iThumbIdx) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// CSCWinThumbs control

class CSCWinThumbs : public CWnd
{
// Construction
public:
	CSCWinThumbs();

// Attributes
public:
	// setters
	void SCSetThumbSize(CSize size)
	{
		m_ThumbsMinSize = size;
		SCComputeRowsCols();
	}
	void SCSetCurSel(int iCurSel)
	{
		m_iCurSel = iCurSel;
		SCEnsureCurThumbVisible();
		//Invalidate();
	}
	void SCSetShowNum(BOOL bShow)
	{
		m_bShowNum = bShow;
		Invalidate();
	}
	void SCSetProgressDraw(BOOL bProgessDraw) { m_bProgessDraw = bProgessDraw; }
	void SCSetFont(CFont* pFont) { m_pFont = pFont;}

	// getters
	int SCGetCurSel() const { return m_iCurSel; }
	int SCGetNbPgRows() const { return m_uiNbRows; }
	int SCGetNbPgCols() const { return m_uiNbCols; }

// Operation
public:
	void SCSetThumbsHolder(I_SCThumbsHolder* pHolder, int iNbThumbs);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCWinThumbs)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSCWinThumbs();
	void SCReset();
	void SCInitCtl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSCWinThumbs)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void SCInitializeScrollBars(int width, int height);
	void SCSizeScrollBars(int width, int height);
	BOOL ScrollChildWindow(int nScrollBar, UINT  wScrollCode, int iCount=1);
	void SCPaint(CPaintDC &dc);
	void SCDrawNumber(int xDest, int yDest, UINT uiThumbIdx);
	int SCComputeHitTest(CPoint point);
	void SCComputeRowsCols();
	void SCNotifyParent();
	void SCEnsureCurThumbVisible();
	void SCGetThumbPos(int iThumbIdx, int& xThumbLeft, int& yThumbTop);

	inline int SCNbColsInSpace(int iWndCx, int iImgCx);
	inline int SCNbRowsInCols(int iNbCols);


private:
	I_SCThumbsHolder*	m_pIThumbsHolder;
	CSize				m_ThumbsMinSize; // set by thumbs holder
	CSize				m_ThumbsSize;	 // computed size
	int					m_iCurSel;

	BOOL		m_bAdjusting;
	BOOL		m_bShowNum;
	int			m_uiNbThumbs;	// total number of thumbs (set by thumbs holder)
	int			m_uiNbRows;		// computed number of rows  
	int			m_uiNbCols;		// computed number of columns
	int			m_iMarginX;
	int			m_iMarginY;
	int			m_iWorldCx;
	int			m_iWorldCy;
	CSCMemDC	m_WorkMemDC;
	CFont*		m_pFont;
	BOOL		m_bProgessDraw;
	BOOL		m_bLastSeen;
};


#endif //_SCWINTHUMBS_H_
//  ------------------------------------------------------------
