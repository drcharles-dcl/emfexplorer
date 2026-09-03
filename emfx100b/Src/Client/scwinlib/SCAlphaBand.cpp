/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCAlphaBand.h"

#include "SCGenInclude.h"
#include SC_INC_GENLIB(SCGenDefs.h)

//	#ifdef _DEBUG
//	#define new DEBUG_NEW
//	#undef THIS_FILE
//	static char THIS_FILE[] = __FILE__;
//	#endif

#define SC_LR_MARGINS	2

SCAlphaBand::SCAlphaBand():
	m_dwBandStyle(0),
	m_bAlpha(128),
	m_sizeMin(0, 0),
	m_sizeMax(0, 0),
	m_crBkColor(RGB(0,0,0)),
	m_crTxtColor(RGB(255,255,255)),
	m_Size(0)
{
	memset(&m_logFont, 0, sizeof(m_logFont));
}

SCAlphaBand::~SCAlphaBand()
{
}

void SCAlphaBand::SCSetFont(LPCTSTR lpszFaceName, int iFamily, int iPoints,
							DWORD dwBandStyle/*=0*/)
{
	ASSERT(lpszFaceName);

	m_dwBandStyle = dwBandStyle;
	HDC hRefDC = GetDC(NULL);

	memset(&m_logFont, 0, sizeof(m_logFont));

	_tcscpy((TCHAR*)&m_logFont.lfFaceName, lpszFaceName);
	m_logFont.lfHeight = -MulDiv(iPoints, GetDeviceCaps(hRefDC, LOGPIXELSY), 72);
	m_logFont.lfWeight = (m_dwBandStyle & SC_BAND_BOLD) ? FW_BOLD : FW_NORMAL;
	m_logFont.lfItalic = (m_dwBandStyle & SC_BAND_ITALIC) ? 1 : 0;
	m_logFont.lfUnderline = (m_dwBandStyle & SC_BAND_UNDERLINED) ? 1 : 0;
	m_logFont.lfPitchAndFamily = iFamily;

	m_logFont.lfCharSet = ANSI_CHARSET;
	m_logFont.lfOutPrecision = OUT_TT_ONLY_PRECIS;
	m_logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_logFont.lfQuality = PROOF_QUALITY;
	m_logFont.lfPitchAndFamily |= VARIABLE_PITCH;

	DeleteDC(hRefDC);
}

int SCAlphaBand::SCComputeSize(BOOL bLimitX/*=TRUE*/, BOOL bLimitY/*=TRUE*/)
{
	DWORD dwLen = m_strText.GetLength();
	if (0==dwLen)
	{
		m_Size = m_sizeMin;
		return m_Size.cx;
	}
	ASSERT(_tclen((TCHAR*)&m_logFont.lfFaceName));
	
	HDC hRefDC = GetDC(NULL);
	CRect rcDraw(0, 0, m_sizeMin.cx - 2*SC_LR_MARGINS, m_sizeMin.cy);

	HFONT hNewFont = CreateFontIndirect(&m_logFont);
	HFONT hOldFont = (HFONT)SelectObject(hRefDC, hNewFont);

	DrawText(hRefDC, m_strText, dwLen, &rcDraw,
		DT_NOPREFIX|DT_LEFT|DT_CALCRECT|((m_dwBandStyle & SC_BAND_SINGLELINE)?DT_SINGLELINE:DT_WORDBREAK));

	SelectObject(hRefDC, hOldFont);
	DeleteObject(hNewFont);
	
	m_Size.cx = rcDraw.Width();
	m_Size.cy = rcDraw.Height();
	if (bLimitX)
		SC_LIMIT_VALUE(m_Size.cx, m_sizeMin.cx, m_sizeMax.cx);
	if (bLimitY)
		SC_LIMIT_VALUE(m_Size.cy, m_sizeMin.cy, m_sizeMax.cy);

	DeleteDC(hRefDC);
	return m_Size.cx;
}



void SCAlphaBand::SCAdjustSize(int iWdt, int iHgt)
{
	m_Size.cx = m_sizeMin.cx = iWdt;
	m_Size.cy = m_sizeMin.cy = iHgt;
	SCComputeSize();
}

int SCAlphaBand::SCAjustFont(HDC hDC, CRect& rect, float fScale)
{
	int iBandHgt = (int)(fScale*m_Size.cy);
	int iBandWdt = rect.Width();
	
	LOGFONT logFont = m_logFont;
	int iFntHgt = (int)(abs(logFont.lfHeight)*fScale);
	
	DWORD dwLen = m_strText.GetLength();
	if (0==dwLen)
		return	iFntHgt;
	
	iBandWdt -= 2*SC_LR_MARGINS;
	CRect rcDraw0(0, 0, iBandWdt, iBandHgt);

	int i = 4;
	int j = iFntHgt;
	for (;;)
	{
		HFONT hNewFont = CreateFontIndirect(&logFont);
		HFONT hOldFont = (HFONT)SelectObject(hDC, hNewFont);

		CRect rcDraw = rcDraw0;
		DrawText(hDC, m_strText, dwLen, &rcDraw,
			DT_NOPREFIX|DT_LEFT|DT_WORDBREAK|DT_CALCRECT);

		SelectObject(hDC, hOldFont);
		DeleteObject(hNewFont);

		if (rcDraw.Width()<=iBandWdt &&
			rcDraw.Height()<=iBandHgt)
		{
			i++;
		} else
		{
			j--;
		}
		if (i>=j)
			break;
		iFntHgt = (i + j)/2;
		logFont.lfHeight = (logFont.lfHeight<0)? -iFntHgt : iFntHgt;
	}
	return	iFntHgt;
}

void SCAlphaBand::SCFillAlphaRect(HDC hDC, int iLeft, int iTop, int iBandWdt, int iBandHgt, HDC hAttribDC)
{
	HDC hdcAlpha = CreateCompatibleDC(hAttribDC);
	HBITMAP hBmAlpha = CreateCompatibleBitmap(hAttribDC, iBandWdt, iBandHgt);
	HBITMAP hOldBm = (HBITMAP)SelectObject(hdcAlpha, hBmAlpha);

	HBRUSH hBrush = CreateSolidBrush(m_crBkColor);
	CRect rcAlpha(0, 0, iBandWdt+1, iBandHgt+1);
	FillRect(hdcAlpha, &rcAlpha, hBrush);
	DeleteObject(hBrush);
	
	BLENDFUNCTION blendFunction;
	blendFunction.BlendOp = AC_SRC_OVER;
	blendFunction.BlendFlags = 0;
	blendFunction.SourceConstantAlpha = m_bAlpha;
	blendFunction.AlphaFormat = 0;
	AlphaBlend(hDC, iLeft, iTop, iBandWdt, iBandHgt,
		hdcAlpha, 0, 0, iBandWdt, iBandHgt,
		blendFunction);
	
	SelectObject(hdcAlpha, hOldBm);
	DeleteObject(hBmAlpha);
	DeleteDC(hdcAlpha);
}

void SCAlphaBand::SCDrawMeta(HDC hDC, CRect& rect, BOOL bTop, BOOL bRight, float fScale, HDC hAttribDC)
{
	int iBandHgt = (int)(fScale*m_Size.cy);
	int iBandWdt = rect.Width();
	int iLeft = rect.left;
	int iTop = (bTop)? rect.top : rect.bottom - iBandHgt;
#if 0
	// GDI+ and the meta DC...?
	{// must detach from DC before reusing it
		Gdiplus::Color BkColor(m_bAlpha,
			GetRValue(m_crBkColor),
			GetGValue(m_crBkColor),
			GetBValue(m_crBkColor));
		Gdiplus::SolidBrush brush(BkColor);
		
		Gdiplus::Graphics graphics(hDC);
		Gdiplus::Rect rectDest(iLeft, iTop, iBandWdt, iBandHgt);

		graphics.FillRectangle(&brush, rectDest);
	}
#else
	// GDI can achieve it
	SCFillAlphaRect(hDC, iLeft, iTop, iBandWdt, iBandHgt, hAttribDC);
#endif

	DWORD dwLen = m_strText.GetLength();
	if (0==dwLen)
		return;
	
	int iFntHgt = SCAjustFont(hAttribDC, rect, fScale);
	LOGFONT logFont = m_logFont;
	logFont.lfHeight = (logFont.lfHeight<0)? -iFntHgt : iFntHgt;
	
	iLeft += SC_LR_MARGINS;
	CRect rcDraw0(iLeft, iTop, iLeft + iBandWdt - 2*SC_LR_MARGINS, iTop + iBandHgt);

	int iOldMode = SetBkMode(hDC, TRANSPARENT);
	int iOldColor = SetTextColor(hDC, m_crTxtColor);
	HFONT hNewFont = CreateFontIndirect(&logFont);
	HFONT hOldFont = (HFONT)SelectObject(hDC, hNewFont);

	DrawText(hDC, m_strText, dwLen, &rcDraw0,
		DT_NOPREFIX|DT_WORDBREAK|((bRight)?DT_RIGHT:DT_LEFT));
	
	SelectObject(hDC, hOldFont);
	DeleteObject(hNewFont);
	SetBkMode(hDC, iOldMode);
	SetTextColor(hDC, iOldColor);
}

void SCAlphaBand::SCDraw(HDC hDC, CRect& rect, BOOL bTop, BOOL bRight, float fScale)
{
	int iBandHgt = (int)(fScale*m_Size.cy);
	int iBandWdt = rect.Width();
	int iLeft = rect.left;
	int iTop = (bTop)? rect.top : rect.bottom - iBandHgt;
#if 1
	{// must detach from DC before reusing it
		Gdiplus::Color BkColor(m_bAlpha,
			GetRValue(m_crBkColor),
			GetGValue(m_crBkColor),
			GetBValue(m_crBkColor));
		Gdiplus::SolidBrush brush(BkColor);
		
		Gdiplus::Graphics graphics(hDC);
		Gdiplus::Rect rectDest(iLeft, iTop, iBandWdt, iBandHgt);
		graphics.FillRectangle(&brush, rectDest);
	}
#else
	SCFillAlphaRect(hDC, iLeft, iTop, iBandWdt, iBandHgt, hDC);
#endif

	DWORD dwLen = m_strText.GetLength();
	if (0==dwLen)
		return;
	
	int iOldMode = SetBkMode(hDC, TRANSPARENT);
	int iOldColor = SetTextColor(hDC, m_crTxtColor);
	
	LOGFONT logFont = m_logFont;
	int iFntHgt = (int)(abs(logFont.lfHeight)*fScale);
	
	HFONT hOldFont = NULL;
	HFONT hNewFont = NULL;
	iBandWdt -= 2*SC_LR_MARGINS;
	iLeft += SC_LR_MARGINS;
	CRect rcDraw0(iLeft, iTop, iLeft + iBandWdt, iTop + iBandHgt);

	int i = 4;
	int j = iFntHgt;
	for (;;)
	{
		hNewFont = CreateFontIndirect(&logFont);
		hOldFont = (HFONT)SelectObject(hDC, hNewFont);
		CRect rcDraw = rcDraw0;
		DrawText(hDC, m_strText, dwLen, &rcDraw,
			DT_NOPREFIX|DT_LEFT|DT_WORDBREAK|DT_CALCRECT);
		if (rcDraw.Width()<=iBandWdt &&
			rcDraw.Height()<=iBandHgt)
		{
			i++;
		} else
		{
			j--;
		}
		if (i>=j)
			break;
		iFntHgt = (i + j)/2;

		logFont.lfHeight = (logFont.lfHeight<0)? -iFntHgt : iFntHgt;
		SelectObject(hDC, hOldFont);
		DeleteObject(hNewFont);
	}
	DrawText(hDC, m_strText, dwLen, &rcDraw0,
		DT_NOPREFIX|DT_WORDBREAK|((bRight)?DT_RIGHT:DT_LEFT));
	
	SelectObject(hDC, hOldFont);
	DeleteObject(hNewFont);
	SetBkMode(hDC, iOldMode);
	SetTextColor(hDC, iOldColor);
}

void SCAlphaBand::SCDraw(Gdiplus::Graphics* pGraphics, CRect& rect, BOOL bTop, BOOL bRight, float fScale)
{
	using namespace Gdiplus;

	ASSERT(pGraphics);
	int iBandHgt = (int)(fScale*m_Size.cy);
	int iBandWdt = rect.Width();
	int iLeft = rect.left;
	int iTop = (bTop)? rect.top : rect.bottom - iBandHgt;

	Color BkColor(m_bAlpha,
		GetRValue(m_crBkColor),
		GetGValue(m_crBkColor),
		GetBValue(m_crBkColor));
	SolidBrush brush(BkColor);
	Rect rectDest(iLeft, iTop, iBandWdt, iBandHgt);
	pGraphics->FillRectangle(&brush, rectDest);
	
	DWORD dwLen = m_strText.GetLength();
	if (0==dwLen)
		return;

	Font* pFont = NULL;
	INT iFntStyle = FontStyleRegular;
	if (m_logFont.lfWeight>=FW_BOLD)
		iFntStyle = FontStyleBold;
	if (m_logFont.lfItalic)
		iFntStyle |= FontStyleItalic;

#ifdef _UNICODE
	WCHAR*	wFaceName = (WCHAR*)m_logFont.lfFaceName;
	LPWSTR pwString = (LPTSTR)LPCTSTR(m_strText);
#else
	WCHAR	wFaceName[LF_FACESIZE];
	MultiByteToWideChar(CP_ACP, 0, (LPSTR)m_logFont.lfFaceName, LF_FACESIZE, wFaceName, LF_FACESIZE);
	LPWSTR pwString = new WCHAR[dwLen+1];
	MultiByteToWideChar(CP_ACP, 0, LPCTSTR(m_strText), dwLen, pwString, dwLen);
#endif
	ASSERT(pwString);

	FontFamily FntFamily(wFaceName);
	int iFntHgt = (int)(abs(m_logFont.lfHeight)*fScale); // TODO: remove internal leading?
   
	iBandWdt -= 2*SC_LR_MARGINS;
	iLeft += SC_LR_MARGINS;
	RectF rcDraw0((REAL)iLeft, (REAL)iTop, (REAL)iBandWdt, (REAL)iBandHgt);

	int i = 4;
	int j = iFntHgt;
	for (;;)
	{
		pFont = new Font(&FntFamily, (REAL)iFntHgt, iFntStyle, UnitPixel);
		Gdiplus::RectF rcDraw;
		pGraphics->MeasureString(pwString, dwLen, pFont, rcDraw0, &rcDraw);

		if (rcDraw.Width<=iBandWdt &&
			rcDraw.Height<=iBandHgt)
		{
			i++;
		} else
		{
			j--;
		}
		if (i>=j)
			break;
		iFntHgt = (i + j)/2;

		delete pFont;
	}
	Color TxtColor;
	TxtColor.SetFromCOLORREF(m_crTxtColor);
	SolidBrush TxtBrush(TxtColor);

	PointF PtOrg(rcDraw0.X, rcDraw0.Y);
	StringFormat stringFormat;
	if (bRight)
	{
		PtOrg.X += rcDraw0.Width;
		stringFormat.SetAlignment(StringAlignmentFar);
	}

	pGraphics->DrawString((WCHAR*)pwString, (INT)dwLen, pFont, PtOrg, &stringFormat, &TxtBrush);

	delete pFont;

#ifndef _UNICODE
	delete [] pwString;
#endif
}

