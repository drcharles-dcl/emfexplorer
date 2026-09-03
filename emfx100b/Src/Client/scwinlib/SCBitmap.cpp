/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCBitmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Create a n-bit-per-pixel surface.
HBITMAP SCCreateDIBSection(HDC hDC, int iWidth, int iHeight, int iBitCount)
{
	BITMAPINFO bmi;
	HBITMAP hbm;
	LPVOID pBits;
	
	// Initialize to 0s.
	ZeroMemory(&bmi, sizeof(bmi));
	
	// Initialize the header.
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = iWidth;
	bmi.bmiHeader.biHeight = iHeight; //warning: bottom-up bitmap
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = iBitCount;
	bmi.bmiHeader.biCompression = BI_RGB;
	
	// Create the surface.
	if (hDC)
		hbm = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	else
	{// use the screen
		HDC hSrcndc = ::GetDC(NULL);
		hbm = ::CreateDIBSection(hSrcndc, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
		::ReleaseDC(NULL, hSrcndc);
	}

	return(hbm);
}

// Create a grayscale surface.
HBITMAP SCCreateGrayScaleDIBSection(HDC hDC, int iWidth, int iHeight, int nBits/* = 8*/)
{
	BITMAPINFO* pbmi=NULL;
	HBITMAP hbm;
	LPVOID pBits;

	HDC    hRefDC;
	DWORD dwSize = sizeof(BITMAPINFO);
	UINT uEntries;

	switch (nBits)
	{
	case 2:
		uEntries = 4;
		break;

	case 4:
		uEntries = 16;
		break;

	case 8:
		uEntries = 256;
		break;

	default:
		return NULL;
	}

	hRefDC = GetDC( NULL );
	if(hRefDC)
	{
		//SetStretchBltMode(hRefDC, HALFTONE);
	} else
		return NULL;

	
	// Create the logical palette based on the entries
	// Allocate memory for the palette.
	dwSize += uEntries*sizeof(PALETTEENTRY);
	pbmi = (BITMAPINFO*) new BYTE[dwSize];
	if (!pbmi)
	{
		ReleaseDC( NULL, hRefDC );
		return NULL;
	}
	// Initialize to 0s.
	ZeroMemory(pbmi, dwSize);
	LPPALETTEENTRY pPal = (LPPALETTEENTRY)&pbmi->bmiColors;
	// gray scale palette
	for (int i=0;(UINT)i<uEntries;i++)
	{
		pPal->peRed = pPal->peGreen = pPal->peBlue = (BYTE)(i & 0xFF);
		pPal->peFlags  = 0;
		pPal++;
	}
	
	// Finish Initialize header.
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = iWidth;
	pbmi->bmiHeader.biHeight = iHeight; //warning: bottom-up bitmap
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biBitCount = nBits;
	pbmi->bmiHeader.biCompression = BI_RGB;
	//pbmi->bmiHeader.biClrUsed = uEntries;
	
	// Create the surface.
	if (hDC)
		hbm = ::CreateDIBSection(hDC, pbmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	else
	// use the screen
		hbm = ::CreateDIBSection(hRefDC, pbmi, DIB_RGB_COLORS, &pBits, NULL, 0);

	delete [] pbmi;
	ReleaseDC( NULL, hRefDC );
	return(hbm);
}

//Just extract the non-white block
//The transparent color is white
BYTE *SCExtractMemBoundingBox(BYTE *lpmem, LPRECT pR, DWORD dwSize, long sScanBytes, short sBitPel)
{
	BYTE *lpTop = lpmem;
	BYTE *lpBottom = lpTop + dwSize;
	BYTE *lpTmp;
	ASSERT(lpmem && pR && dwSize && sScanBytes && sBitPel);
	if ( (!lpmem) || (!pR)|| (!dwSize) || (!sScanBytes) || (!sBitPel))
		return NULL;

	long widthBytes = ((pR->right - pR->left)*(0L+sBitPel))/8;
	ASSERT(widthBytes<=sScanBytes);
	// top band
	lpTmp = lpTop;
	long top;
	while ((lpTop<lpBottom) && (0xFF == *lpTop))
	{
		lpTop++;
		if (lpTop - lpTmp>=widthBytes)
		{// skip extra bytes
			lpTmp += sScanBytes;
			lpTop = lpTmp;
		}
	}
	// check full blank page
	if (lpTop>=lpBottom)
	{
		// blank page
		::SetRectEmpty(pR);
		return lpBottom;
	}
	top = (long)((lpTop - lpmem) / sScanBytes);	// line where drawing begins
	lpTop = lpmem + (top+0L) * sScanBytes;		// adjust to first byte on scan line

	// bottom band
	lpTmp = lpBottom - sScanBytes;
	BYTE *lpRight = lpTmp + widthBytes;
	BOOL bOkB = FALSE;
	long bottom;
	while (lpTmp>lpTop)
	{
		lpBottom = lpTmp;
		// check this scan line, up to extra bytes
		while (lpBottom<lpRight)
		{ 
			if (0xFF != *lpBottom)
			{
				bOkB = TRUE;
				break;
			}
			lpBottom++;
		}
		if (bOkB)
			break;

		// go up one scan line
		lpTmp -= sScanBytes;
		lpRight -= sScanBytes;
	}

	bottom = (long)((lpBottom - lpmem) / sScanBytes);	// after line where drawing ends
	lpBottom = lpmem + (bottom+0L) * sScanBytes;		// adjust to first byte on next scan line

	// left band (scan one column at a time)
	BOOL bOkL = FALSE;
	long left=-1;
	while(left<widthBytes)
	{
		lpTmp = lpTop + (++left);
		while (lpTmp<lpBottom)
		{
			if(0xFF != *lpTmp)
			{
				bOkL = TRUE;
				pR->left=(int)((LONG)left*8) / sBitPel;
				break;
			}
			lpTmp += sScanBytes;
		}
		if (bOkL)
			break;
	}

	// right band
	BOOL bOkR = FALSE;
	long right=widthBytes;
	while (right>left)
	{
		lpTmp = lpTop + (--right);
		while (lpTmp<lpBottom)
		{
			if(0xFF != *lpTmp)
			{
				bOkR = TRUE;
				pR->right=(int)((LONG)right*8L) / sBitPel;
				break;
			}
			lpTmp += sScanBytes;
		}
		if (bOkR)
			break;
	}

	// transform left and right in pixels
	pR->top=top;
	pR->bottom=bottom;
	return lpBottom;
}


///
/// Make a copy of a GDI logical palette
///
HPALETTE SCCopyPalette (HPALETTE hpal)
{
	ASSERT(hpal);

    WORD nNumEntries;
   if (!hpal || !GetObject(hpal, sizeof(nNumEntries), &nNumEntries))
        return NULL;

    if (nNumEntries == 0)
        return NULL;

    PLOGPALETTE pPal = (PLOGPALETTE) new BYTE[sizeof(LOGPALETTE) + nNumEntries * sizeof(PALETTEENTRY)];
    if (!pPal)
        return NULL;

    pPal->palVersion    = 0x300;
    pPal->palNumEntries = nNumEntries;

    GetPaletteEntries(hpal, 0, nNumEntries, (LPPALETTEENTRY)pPal->palPalEntry);

    hpal = CreatePalette(pPal);

	delete [] (BYTE*)pPal;
    return hpal;
}

///
/// Copy an area from a bitmap (or the whole bitmap)
///
HBITMAP SCCopyBitmap(HBITMAP hbm, HDC hdc/*=NULL*/, LPRECT prect/*=NULL*/)
{
	ASSERT(hbm);

    BITMAP  bm;
    if (!hbm || !GetObject(hbm, sizeof(bm), &bm))
	{
#ifdef _DEBUG
		DWORD dwError = GetLastError();
		//SCShowError();
#endif
		return NULL;
	}
	
	BOOL bTmpDC = (NULL==hdc);
	if (bTmpDC)
		hdc = GetDC(NULL);

	int iCx = prect ? prect->right  - prect->left : bm.bmWidth;
	int iCy = prect ? prect->bottom - prect->top : bm.bmHeight;

	HBITMAP hNewBm = CreateCompatibleBitmap(hdc, iCx, iCy);
	if (hNewBm)
	{
		HDC hMemDCsrc = CreateCompatibleDC(hdc);
		HDC hMemDCdst = CreateCompatibleDC(hdc);

		HBITMAP hOldBMSrc = (HBITMAP)SelectObject(hMemDCsrc, hbm);
		HBITMAP hOldBMDest = (HBITMAP)SelectObject(hMemDCdst, hNewBm);
				
		BitBlt(hMemDCdst, 0, 0, iCx, iCy,
			   hMemDCsrc, prect ? prect->left : 0,
					      prect ? prect->top  : 0, SRCCOPY);
		
		SelectObject(hMemDCsrc, hOldBMSrc);
		SelectObject(hMemDCdst, hOldBMDest);

		DeleteDC(hMemDCsrc);
		DeleteDC(hMemDCdst);
	}

	if (bTmpDC)
		ReleaseDC(NULL, hdc);

    return hNewBm;
}

