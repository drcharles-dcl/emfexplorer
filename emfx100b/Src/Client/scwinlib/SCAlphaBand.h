/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCALPHABAND_H_
#define _SCALPHABAND_H_

#include "unknwn.h"		// or GDI+ won't let us compile
#include "gdiplus.h"

#define SC_BAND_BOLD		0x0001
#define SC_BAND_ITALIC		0x0002
#define SC_BAND_UNDERLINED	0x0004
#define SC_BAND_SINGLELINE	0x1000

class SCAlphaBand
{
// Construction/Destruction
public:
	SCAlphaBand();
	~SCAlphaBand();

// Accessors
	// getters
	CSize& SCGetSize() { return m_Size; }

	// setters
	void SCSetStyle(DWORD dwBandStyle) { m_dwBandStyle = dwBandStyle; }
	void SCSetLimits(CSize& sizeMin, CSize& sizeMax)
	{
		m_sizeMin = m_Size = sizeMin;
		m_sizeMax = sizeMax;
	}
	void SCSetColors(COLORREF crBkColor, BYTE bAlpha, COLORREF crTxtColor)
	{
		m_bAlpha = bAlpha;
		m_crBkColor = crBkColor;
		m_crTxtColor = crTxtColor;
	}
	void SCSetText(LPCTSTR lpszText)
	{
		m_strText = (lpszText) ? lpszText : _T("");
		SCComputeSize();
	}

// Operation
public:
	int SCComputeSize(BOOL bLimitX=TRUE, BOOL bLimitY=TRUE);
	void SCAdjustSize(int iWdt, int iHgt);
	void SCSetFont(LPCTSTR lpszFaceName, int iFamily, int iPoints,
		 DWORD dwBandStyle=0);
	void SCDraw(HDC hDC, CRect& rect, BOOL bTop, BOOL bRight, float fScale);
	void SCDraw(Gdiplus::Graphics* pGraphics, CRect& rect, BOOL bTop, BOOL bRight, float fScale);

	void SCDrawMeta(HDC hDC, CRect& rect, BOOL bTop, BOOL bRight, float fScale, HDC hAttribDC);
	int SCAjustFont(HDC hDC, CRect& rect, float fScale);
	void SCFillAlphaRect(HDC hDC, int iLeft, int iTop, int iBandWdt, int iBandHgt, HDC hAttribDC);


// Implementation
protected:

private:
	// set
	DWORD		m_dwBandStyle;
	CSize		m_sizeMin;
	CSize		m_sizeMax;
	BYTE		m_bAlpha;
	COLORREF	m_crBkColor;
	COLORREF	m_crTxtColor;
	CString		m_strText;

	// computed
	CSize		m_Size;
	LOGFONT		m_logFont; // font at 100%
};

#endif //_SCALPHABAND_H_
//  ------------------------------------------------------------
