/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "MSDib.h"

#ifndef SC_USE_MSDIBCODE

#error "Provide your implementation for DIBNumColors,..."

#else
////////////////////////////////////////////////////////////////////////////////////////
//	(c) Microsoft, 1991-1998 / MSDN / DIBUTIL.C, DIB.C
//
/*************************************************************************
 * This function calculates the number of colors in the DIB's color table
 * by finding the bits per pixel for the DIB (whether Win3.0 or other-style
 * DIB). If bits per pixel is 1: colors=2, if 4: colors=16, if 8: colors=256,
 * if 24, no colors in color table.
 *
 ************************************************************************/
WORD WINAPI DIBNumColors(LPBITMAPINFOHEADER lpbi)
{
	WORD wBitCount;  // DIB bit count

	/*  If this is a Windows-style DIB, the number of colors in the
	 *  color table can be less than the number of bits per pixel
	 *  allows for (i.e. lpbi->biClrUsed can be set to some value).
	 *  If this is the case, return the appropriate value.
	 */

	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;

		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if (dwClrUsed != 0)
			return (WORD)dwClrUsed;
	}

	/*  Calculate the number of colors in the color table based on
	 *  the number of bits per pixel for the DIB.
	 */
	if (IS_WIN30_DIB(lpbi))
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

	/* return number of colors based on bits per pixel */
	switch (wBitCount)
	{
		case 1:
			return 2;

		case 4:
			return 16;

		case 8:
			return 256;

		default:
			return 0;
	}
}


/*************************************************************************
 * This function gets the size required to store the DIB's palette by
 * multiplying the number of colors by the size of an RGBQUAD (for a
 * Windows 3.0-style DIB) or by the size of an RGBTRIPLE (for an other-
 * style DIB).
 *
 ************************************************************************/
WORD WINAPI PaletteSize(LPBITMAPINFOHEADER lpbi)
{
   /* calculate the size required by the palette */
   if (IS_WIN30_DIB (lpbi))
	  return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBQUAD));
   else
	  return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
}

// dib.c
/**************************************************************************** 
 *                                                                          * 
 *  FUNCTION   : DibFromBitmap()                                            * 
 *                                                                          * 
 *  PURPOSE    : Will create a global memory block in DIB format that       * 
 *       represents the Device-dependent bitmap (DDB) passed in.            * 
 *                                                                          * 
 *  RETURNS    : A handle to the DIB                                        * 
 *                                                                          * 
 ****************************************************************************/ 
HANDLE DibFromBitmap(HBITMAP hbm, DWORD biStyle, WORD biBits, HPALETTE hpal) 
{ 
    BITMAP bm; 
    BITMAPINFOHEADER bi; 
    LPBITMAPINFOHEADER lpbi; 
    DWORD dwLen; 
    HANDLE hdib; 
    HANDLE h; 
    HDC hdc; 
 
    if (!hbm) 
        return NULL; 
 
    if (hpal == NULL) 
        hpal = (HPALETTE)GetStockObject(DEFAULT_PALETTE); 
 
    GetObject(hbm,sizeof(bm),(LPSTR)&bm); 
 
    if (biBits == 0) 
        biBits =  bm.bmPlanes * bm.bmBitsPixel; 
 
    bi.biSize           = sizeof(BITMAPINFOHEADER); 
    bi.biWidth          = bm.bmWidth; 
    bi.biHeight         = bm.bmHeight; 
    bi.biPlanes         = 1; 
    bi.biBitCount       = biBits; 
    bi.biCompression    = biStyle; 
    bi.biSizeImage      = 0; 
    bi.biXPelsPerMeter  = 0; 
    bi.biYPelsPerMeter  = 0; 
    bi.biClrUsed        = 0; 
    bi.biClrImportant   = 0; 
 
    dwLen  = bi.biSize + PaletteSize(&bi); 
 
    hdc = GetDC(NULL); 
    hpal = SelectPalette(hdc,hpal,FALSE); 
    RealizePalette(hdc); 
 
    hdib = GlobalAlloc(GHND,dwLen); 
 
    if (!hdib) 
    { 
        SelectPalette(hdc,hpal,FALSE); 
        ReleaseDC(NULL,hdc); 
        return NULL; 
    } 
 
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib); 
 
    *lpbi = bi; 
 
    /*  call GetDIBits with a NULL lpBits param, so it will calculate the 
     *  biSizeImage field for us 
     */ 
    GetDIBits(hdc, hbm, 0, (WORD)bi.biHeight, 
    NULL, (LPBITMAPINFO)lpbi, DIB_RGB_COLORS); 
 
    bi = *lpbi; 
    GlobalUnlock(hdib); 
 
    /* If the driver did not fill in the biSizeImage field, make one up */ 
    if (bi.biSizeImage == 0) 
    { 
        bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight; 
 
        if (biStyle != BI_RGB) 
            bi.biSizeImage = (bi.biSizeImage * 3) / 2; 
    } 
 
    /*  realloc the buffer big enough to hold all the bits */ 
    dwLen = bi.biSize + PaletteSize(&bi) + bi.biSizeImage; 
    if (h = GlobalReAlloc(hdib,dwLen,0)) 
        hdib = h; 
    else 
    { 
        GlobalFree(hdib); 
        hdib = NULL; 
 
        SelectPalette(hdc,hpal,FALSE); 
        ReleaseDC(NULL,hdc); 
        return hdib; 
    } 
 
    /*  call GetDIBits with a NON-NULL lpBits param, and actualy get the 
     *  bits this time 
     */ 
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib); 
 
    if (GetDIBits( hdc, 
        hbm, 
        0, 
        (WORD)bi.biHeight, 
        (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi), 
        (LPBITMAPINFO)lpbi, DIB_RGB_COLORS) == 0) 
    { 
        GlobalUnlock(hdib); 
        hdib = NULL; 
        SelectPalette(hdc,hpal,FALSE); 
        ReleaseDC(NULL,hdc); 
        return NULL; 
    } 
 
    bi = *lpbi; 
    GlobalUnlock(hdib); 
 
    SelectPalette(hdc,hpal,FALSE); 
    ReleaseDC(NULL,hdc); 
    return hdib; 
}

#endif

// End of Microsoft code
////////////////////////////////////////////////////////////////////////////////////////////
