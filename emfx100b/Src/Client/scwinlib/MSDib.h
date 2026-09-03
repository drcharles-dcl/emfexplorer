/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _MSDIB_H_
#define _MSDIB_H_

// TODO: Place it at module level
#define SC_USE_MSDIBCODE


////////////////////////////////////////////////////////////////////////////////////////
//	(c) Microsoft, 1991-1998 / MSDN / DIB.h
//

/* DIB Macros*/
#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))

// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits"
// parameter is the bit count for the scanline (biWidth * biBitCount),
// and this macro returns the number of DWORD-aligned bytes needed
// to hold those bits.
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

WORD WINAPI DIBNumColors(LPBITMAPINFOHEADER lpbi);
WORD WINAPI PaletteSize(LPBITMAPINFOHEADER lpbi);
HANDLE DibFromBitmap(HBITMAP hbm, DWORD biStyle, WORD biBits, HPALETTE hpal);
// End of Microsoft code
////////////////////////////////////////////////////////////////////////////////////////////


#endif //_MSDIB_H_
//  ------------------------------------------------------------
