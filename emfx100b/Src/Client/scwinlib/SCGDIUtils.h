/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCGDIUTILS_H_
#define _SCGDIUTILS_H_

// Helpers
#include "SCWinTypes.h"

// DIB Macros
#define NEW_DIB_FORMAT(lpbih) (lpbih->biSize != sizeof(BITMAPCOREHEADER))

// Same as WIDTHBYTES, but for device-dependent bitmaps (WORD-aligned scanlines)
#define DDB_WIDTHBYTES(bits)    (((bits) + 15) / 16 * 2)

// Text/Path, .. attributes
#define SC_PATH_STROKE			0x01
#define SC_PATH_FILL			0x02
#define SC_PATH_STROKEANDFILL	(SC_PATH_STROKE|SC_PATH_FILL)
#define SC_PATH_CLIP			0x10
#define SC_PATH_PTMASK			(PT_MOVETO|PT_LINETO|PT_BEZIERTO)
#define SC_PATH_CLOSEFIG		1

#define SC_VTXTALIGN_MASK		(TA_TOP|TA_BASELINE|TA_BOTTOM)
#define SC_HTXTALIGN_MASK		(TA_LEFT|TA_CENTER|TA_RIGHT)


// Functions
WORD SCColorTableSize(LPVOID lpv);
void SCDrawFrameAndShadow(CDC *pDC, int x, int y, int cx, int cy, int iShWdt, COLORREF BkColor, BOOL bFrame=TRUE);
BOOL SCGetTextCharWidthsW(HDC hDC, LPCWSTR pwString, INT iCount, INT *lpCharWidths);
BOOL SCGetTextCharWidthsHImW(HDC hDC, LPCWSTR pwString, INT iCount, INT *lpCharWidths, float fScaleX, float fScaleY);
BOOL SCComputeTextExtentPointW(HDC hAttDC, LPCWSTR pwString, INT iCount, INT *pCharDx, SIZE &TextSize);

void SCNormalizeRect(LPRECT prc);
void SCGetNormalizedRect(LPRECT pRcDest, LPCRECT pRcSrc);

BYTE* SCExpandMask(LONG lWidth, LONG lHeight,
				   LONG lxMask, LONG lyMask,
				   LPCBYTE pBitsMask, LPCBITMAPINFO pBmiMask, DWORD dwUsageMask, BOOL bTopDown=FALSE);
HBITMAP SCBuildAlphaBitmap(LPCBYTE pSrcBits, LPCBITMAPINFO pSrcBmi, DWORD dwSrcUsage, BYTE* pMask, BOOL bTopDown=FALSE);
HBITMAP SCConvertPalDIBToRGB(HDC hRefDC, LPCBYTE lpSrcBits, LPCBITMAPINFO lpSrcBmi);

HBITMAP SCDIBtoMonochromeBitmap(HDC hDCRef, BITMAPINFO* pBmi, DWORD *pBitsDW);
BOOL SCIsMonochromeDC(HDC hDC);
void SCFillMonochromePalette(HDC hDC, PPALETTEENTRY palPalEntry);

DWORD SCFontFamilyApproximant(LPCTSTR lpszFacename);
DWORD SCFontFamilyApproximantA(LPCSTR lpszFacename);
DWORD SCFontFamilyApproximantW(LPCWSTR lpwszFacename);

BOOL SCIsIdentityXFORM(XFORM& rXForm);

HDC SCGetDefaultPrinterDC(HWND hwndOwner=NULL);

void SCTestShadedRectangle(HDC hDC, LPCRECT pRect, int iMode);
void SCTestShadedTriangle(HDC hDC, LPCRECT pRect);
void SCEMFExplorerPaint(CDC* pDC, const CRect* pRect, LPCTSTR lpszMsgID=_T(""), LPCTSTR lpszMsgAction=_T(""));


#endif //_SCGDIUTILS_H_
//  ------------------------------------------------------------
