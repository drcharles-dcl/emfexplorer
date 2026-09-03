/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCDCCLONER_H_
#define _SCDCCLONER_H_


class SCDCCloner
{
public:
	SCDCCloner(HDC hDC);
	virtual ~SCDCCloner();

	HDC SCGetHDC() { return m_hDC; }
	HDC SCDetach() { HDC hDC = m_hDC; m_hDC = NULL; return hDC; }

private:
	HDC SCCloneDC(HDC hDC);
	
// data
	HDC			m_hDC;

	HBITMAP		m_hBitmap;
	HPEN		m_hPen;
	HBRUSH		m_hBrush;
	HFONT		m_hFont;
	HPALETTE	m_hPal;

	HBITMAP		m_hOldBitmap;
	HPEN		m_hOldPen;
	HBRUSH		m_hOldBrush;
	HFONT		m_hOldFont;
	HPALETTE	m_hOldPal;
};


#endif //_SCDCCLONER_H_
//  ------------------------------------------------------------
