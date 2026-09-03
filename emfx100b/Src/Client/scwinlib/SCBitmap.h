/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCBITMAP_H_
#define _SCBITMAP_H_

HBITMAP SCCreateDIBSection(HDC hDC, int iWidth, int iHeight, int iBitCount);
inline HBITMAP SCCreate24BPPDIBSection(HDC hDC, int iWidth, int iHeight)
{
	return SCCreateDIBSection(hDC, iWidth, iHeight, 24);
}

HBITMAP SCCreateGrayScaleDIBSection(HDC hDC, int iWidth, int iHeight, int nBits = 8);

BYTE *SCExtractMemBoundingBox(BYTE *lpmem, LPRECT pR, DWORD dwSize, long sScanBytes, short sBitPel);

HBITMAP SCCopyBitmap(HBITMAP hbm, HDC hdc=NULL, LPRECT prect=NULL);
HPALETTE SCCopyPalette(HPALETTE hpal); 

#endif //_SCBITMAP_H_
//  ------------------------------------------------------------
