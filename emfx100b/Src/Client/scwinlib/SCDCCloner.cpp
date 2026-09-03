/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCDCCloner.h"
#include "SCGenInclude.h"
#include "SCGDIUtils.h"
#include "SCBitmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////////////
// SCDCCloner
///////////////////////////////////////////////////////////////////////////////////////

SCDCCloner::SCDCCloner(HDC hDC):
	m_hDC(NULL),
	m_hBitmap(NULL),
	m_hPen(NULL),
	m_hBrush(NULL),
	m_hFont(NULL),
	m_hPal(NULL),
	m_hOldBitmap(NULL),
	m_hOldPen(NULL),
	m_hOldBrush(NULL),
	m_hOldFont(NULL),
	m_hOldPal(NULL)
{
	if (hDC)
		m_hDC = SCCloneDC(hDC);
	else
	{
		hDC = GetDC(NULL);
		m_hDC = SCCloneDC(hDC);
		ReleaseDC(NULL, hDC);
	}
}

SCDCCloner::~SCDCCloner()
{
	if (m_hDC)
	{
		if (m_hOldBitmap)
			SelectObject(m_hDC, m_hOldBitmap);
		if (m_hOldPen)
			SelectObject(m_hDC, m_hOldPen);
		if (m_hOldBrush)
			SelectObject(m_hDC, m_hOldBrush);
		if (m_hOldFont)
			SelectObject(m_hDC, m_hOldFont);
		if (m_hOldPal)
			SelectObject(m_hDC, m_hOldPal);

		DeleteDC(m_hDC);

		if (m_hBitmap)
			DeleteObject(m_hBitmap);
		if (m_hPen)
			DeleteObject(m_hPen);
		if (m_hBrush)
			DeleteObject(m_hBrush);
		if (m_hFont)
			DeleteObject(m_hFont);
		if (m_hPal)
			DeleteObject(m_hPal);
	}
}

///
/// Make a copy of the given DC
///
HDC SCDCCloner::SCCloneDC(HDC hDC)
{
	HDC hClone = CreateCompatibleDC(hDC);
#if 0
	// Zzzz!... Let it sleep. It has problems with some system bitmaps.
	if (1)
	{
		//	Background color
		SetBkColor(hClone, GetBkColor(hDC));

		//	Background mode 
		SetBkMode(hClone, GetBkMode(hDC));

		//	Bitmap
		{
			// Get
			HBITMAP hBM = (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP);
			ASSERT(hBM);
			if (hBM)
			{
				m_hBitmap = SCCopyBitmap(hBM, hDC);
				//ASSERT(m_hBitmap);
				if (!m_hBitmap)
				{
					m_hBitmap = CreateCompatibleBitmap(hDC, 1, 1);
				}
				ASSERT(m_hBitmap);
				
				// Set
				if (m_hBitmap)
					m_hOldBitmap = (HBITMAP)SelectObject(hClone, m_hBitmap);
			}
		}

		//	Brush
		{
			// Get
			HBRUSH hBrush = (HBRUSH)GetCurrentObject(hDC, OBJ_BRUSH);
			ASSERT(hBrush);
			if (hBrush)
			{
				LOGBRUSH LogBrush;
				int iGetRes = GetObject(hBrush, sizeof(LogBrush), &LogBrush);
				ASSERT(iGetRes);

				m_hBrush = CreateBrushIndirect(&LogBrush);
				ASSERT(m_hBrush);
				
				// Set
				m_hOldBrush = (HBRUSH)SelectObject(hClone, m_hBrush);
			}

			//	Brush origin
			POINT PtOrg;
			GetBrushOrgEx(hDC, &PtOrg);
			SetBrushOrgEx(hClone, PtOrg.x, PtOrg.y, NULL);
		}

		//	Clipping region (see below)

		//	Palette
		{
			// Get
			HPALETTE hPal = (HPALETTE)GetCurrentObject(hDC, OBJ_PAL);
			// ASSERT(hPal); // No.
			if (hPal)
			{
				m_hPal = SCCopyPalette(hPal);
				ASSERT(m_hPal);
				
				// Set
				m_hOldPal = (HPALETTE)SelectPalette(hClone, m_hPal, TRUE);
			}
		}

		//	Current pen position
		{
			POINT PtPos;
			GetCurrentPositionEx(hDC, &PtPos);
			MoveToEx(hClone, PtPos.x, PtPos.y, NULL);
		}

		//	Device origin (see below)

		//	Drawing mode
		SetROP2(hClone, GetROP2(hDC));

		//	Font
		{
			// Get
			HFONT hFont = (HFONT)GetCurrentObject(hDC, OBJ_FONT);
			ASSERT(hFont);
			if (hFont)
			{
				LOGFONT LogFont;
				int iGetRes = GetObject(hFont, sizeof(LogFont), &LogFont);
				ASSERT(iGetRes);

				m_hFont = CreateFontIndirect(&LogFont);
				ASSERT(m_hFont);
				
				// Set
				m_hOldFont = (HFONT)SelectObject(hClone, m_hFont);
			}
		}

		//	Intercharacter spacing
		SetTextCharacterExtra(hClone, GetTextCharacterExtra(hDC));

		//	Mapping mode (see below)

		// Pen
		{
			// Get
			HPEN hPen = (HPEN)GetCurrentObject(hDC, OBJ_PEN);
			ASSERT(hPen);
			if (hPen)
			{
				LOGPEN LogPen;
				int iGetRes = GetObject(hPen, sizeof(LogPen), &LogPen);
				ASSERT(iGetRes);

				m_hPen = CreatePenIndirect(&LogPen);
				ASSERT(m_hPen);
				
				// Set
				m_hOldPen = (HPEN)SelectObject(hClone, m_hPen);
			}
		}

		//	Polygon-fill mode
		SetPolyFillMode(hClone, GetPolyFillMode(hDC));

		//	Stretch mode
		SetStretchBltMode(hClone, GetStretchBltMode(hDC));

		//	Text color
		SetTextColor(hClone, GetTextColor(hDC));

		// Window and viewport
		SetMapMode(hClone, MM_ANISOTROPIC);
		POINT PtOrg;
		SIZE Size;
		//	Window origin
		GetWindowOrgEx(hDC, &PtOrg);
		SetWindowOrgEx(hClone, PtOrg.x, PtOrg.y, NULL);

		//	Window extent
		GetWindowExtEx(hDC, &Size);
		SetWindowExtEx(hClone, Size.cx, Size.cy, NULL);

		//	Viewport origin
		GetViewportOrgEx(hDC, &PtOrg);
		SetViewportOrgEx(hClone, PtOrg.x, PtOrg.y, NULL);

		//	Viewport extent
		GetViewportExtEx(hDC, &Size);
		SetViewportExtEx(hClone, Size.cx, Size.cy, NULL);

		//	Mapping mode
		SetMapMode(hClone, GetMapMode(hDC));

		// Transformation
		XFORM xform;
		GetWorldTransform(hDC, &xform);

		int iGM = SetGraphicsMode(hClone, GM_ADVANCED);
		SetWorldTransform(hClone, &xform);
		SetGraphicsMode(hClone, iGM);

		//	Clipping region
		if (1)
		{
			HRGN hRgn = CreateRectRgn(0, 0, 1, 1);
			int iRes = GetClipRgn(hDC, hRgn);
			if (1==iRes)
			{
				iRes = ExtSelectClipRgn(hClone, hRgn, RGN_COPY);
				//ASSERT(NULLREGION!=iRes);
				if (NULLREGION==iRes)
					ExtSelectClipRgn(hClone, NULL, RGN_COPY);
			}
			DeleteObject(hRgn);

#ifdef _DEBUG
			{
				// debug stuff
				RECT rcClip, rcClipClone;
				GetClipBox(hDC, &rcClip);
				GetClipBox(hClone, &rcClipClone);
				
				HBITMAP hBM = (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP);
				BITMAP bm;
				int iResD = GetObject(hBM, sizeof(bm), &bm);

				HBITMAP hBMClone = (HBITMAP)GetCurrentObject(hClone, OBJ_BITMAP);
				BITMAP bmClone;
				int iResClone = GetObject(hBMClone, sizeof(bmClone), &bmClone);
			}
#endif
		}
	}
#endif
	return hClone;
}
