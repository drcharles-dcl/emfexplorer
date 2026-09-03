/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCGDIUtils.h"
#include "MSDib.h"
#include "SCGenInclude.h"
#include SC_INC_GENLIB(SCGenMath.h)
#include SC_INC_GENLIB(SCGenDefs.h)
#include <afxext.h> // printdlg

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///
/// Calculates the palette size in bytes. If the info. block
/// is of the BITMAPCOREHEADER type, the number of colors is
/// multiplied by 3 to give the palette size, otherwise the 
/// number of colors is multiplied by 4.
///
WORD SCColorTableSize (LPBITMAPINFOHEADER lpbih)
{
    if (NEW_DIB_FORMAT(lpbih))
    {
      if (lpbih->biCompression == BI_BITFIELDS)
         /* Remember that 16/32bpp dibs can still have a color table */
         return (sizeof(DWORD) * 3) + (DIBNumColors(lpbih) * sizeof (RGBQUAD));
      else
         return (DIBNumColors(lpbih) * sizeof(RGBQUAD));
    }
    else
      return (DIBNumColors(lpbih) * sizeof(RGBTRIPLE));
}

///
/// Create a 24bpp RGB copy of a palette bitmap.
/// hRefDC must contain the realized palette from where colors are taken.
///
HBITMAP SCConvertPalDIBToRGB(HDC hRefDC, LPCBYTE lpSrcBits, LPCBITMAPINFO lpSrcBmi)
{
	INT iWidth = lpSrcBmi->bmiHeader.biWidth;
	INT iHeight = abs(lpSrcBmi->bmiHeader.biHeight);
    
    BITMAPINFO bmiTarget;
    bmiTarget.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
    bmiTarget.bmiHeader.biWidth = iWidth;
    bmiTarget.bmiHeader.biHeight = lpSrcBmi->bmiHeader.biHeight;
    bmiTarget.bmiHeader.biPlanes = 1;
    bmiTarget.bmiHeader.biBitCount = 24;
    bmiTarget.bmiHeader.biCompression = BI_RGB;
    bmiTarget.bmiHeader.biSizeImage = 0;
    bmiTarget.bmiHeader.biXPelsPerMeter = 0;
    bmiTarget.bmiHeader.biYPelsPerMeter = 0;
    bmiTarget.bmiHeader.biClrUsed = 0;
    bmiTarget.bmiHeader.biClrImportant = 0;

    VOID* lpTargetBits;
    HBITMAP hTargetBitmap = CreateDIBSection(hRefDC, &bmiTarget, DIB_RGB_COLORS, &lpTargetBits, NULL, 0 );
    HDC hTargetDC = CreateCompatibleDC( hRefDC );
    HBITMAP hOldTargetBitmap = (HBITMAP)SelectObject( hTargetDC, hTargetBitmap );

	INT iRes = SetDIBitsToDevice(hTargetDC, 0, 0, iWidth, iHeight, 0, 0,
			0, iHeight, lpSrcBits, lpSrcBmi, DIB_PAL_COLORS);
	ASSERT(iRes);
	GdiFlush(); // let GDI finish before we access the bits

	// Cleanup
	SelectObject(hTargetDC, hOldTargetBitmap);
	DeleteDC(hTargetDC);

	return hTargetBitmap;
}

void SCNormalizeRect(LPRECT prc)
{
	ASSERT(prc);
	if (prc->right < prc->left)
		SMC_ISWAP(prc->right,  prc->left);
	if (prc->bottom < prc->top)
		SMC_ISWAP(prc->bottom, prc->top);
}

void SCGetNormalizedRect(LPRECT pRcDest, LPCRECT pRcSrc)
{
	ASSERT(pRcDest && pRcSrc);
	if (pRcSrc->right < pRcSrc->left)
	{
		pRcDest->left = pRcSrc->right;
		pRcDest->right = pRcSrc->left;
	} else
	{
		pRcDest->left = pRcSrc->left;
		pRcDest->right = pRcSrc->right;
	}

	if (pRcSrc->bottom < pRcSrc->top)
	{
		pRcDest->top = pRcSrc->bottom;
		pRcDest->bottom = pRcSrc->top;
	} else
	{
		pRcDest->top = pRcSrc->top;
		pRcDest->bottom = pRcSrc->bottom;
	}
}

///
/// Draw a frame and right-bottom shadow
///
void SCDrawFrameAndShadow(CDC *pDC, int x, int y, int cx, int cy, int iShWdt, COLORREF BkColor, BOOL bFrame/*=TRUE*/)
{
	ASSERT(pDC);

	int iXRight  = x + cx;
	int iYBottom = y + cy;

	// Shadow
	if (iShWdt)
	{
		CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH));
		
		// Right band
		CRect rc(iXRight, y + 2, iXRight + iShWdt + 1, iYBottom + iShWdt + 1);
		pDC->FillRect(&rc, pBrush);
		
		// Bottom band
		rc.left = x + 2;
		rc.top = iYBottom;
		pDC->FillRect(&rc, pBrush);
	}

	// Frame
	if (bFrame)
	{
		CRect rc(x, y, iXRight, iYBottom);
		InflateRect(&rc, 1, 1);
		
		CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
		CBrush *pOldBrush = pDC->SelectObject(pBrush);
		
		CPen Pen;
		Pen.CreatePen(PS_SOLID, 0, BkColor);
		CPen* pOldPen = pDC->SelectObject(&Pen);
		int iOldRop2 = pDC->SetROP2(R2_NOTCOPYPEN);
		
		pDC->Rectangle(&rc);

		// Clean up
		pDC->SetROP2(iOldRop2);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		Pen.DeleteObject();
	}
}

///
/// Comptutes character extents by hand (panic function)
///
BOOL SCComputeTextExtentPointW(HDC hAttDC, LPCWSTR pwString, INT iCount, INT *pCharDx, SIZE &TextSize)
{
	ASSERT(pCharDx);
	if (!pCharDx)
		return FALSE;

	pCharDx[0] = 0;
	SIZE tmpSize;

	TextSize.cy = 0;
	for (INT nInd = 0; (nInd < iCount); nInd++)
	{
		GetTextExtentPoint32W(hAttDC, pwString, nInd+1, &tmpSize);
		pCharDx[nInd] = tmpSize.cx;

		if (TextSize.cy<tmpSize.cy)
			TextSize.cy = tmpSize.cy;
	}
	TextSize.cx = tmpSize.cx;

	return TRUE;
}

///
/// Returns an array containing the width of each character in a text.
///	The lpCharWidths buffer must be large enough to hold iCount ints.
///
BOOL SCGetTextCharWidthsW(HDC hDC, LPCWSTR pwString, INT iCount, INT *lpCharWidths)
{
	ASSERT(lpCharWidths);
	if (!lpCharWidths)
		return FALSE;

	// We want the horizontal extents (otherwise, for a vertical font,
	// GetTextExtentExPoint would return character heights in lpCharWidths)
	HFONT hFont = NULL;
	HFONT hHorizFont = NULL;
	if (hFont = (HFONT)GetCurrentObject(hDC, OBJ_FONT))
	{
		LOGFONT LogFont;
		::GetObject(hFont, sizeof(LogFont), &LogFont);
		if (LogFont.lfEscapement)
		{
			LogFont.lfEscapement = 0;
			LogFont.lfOrientation = 0;
			hHorizFont = (HFONT)CreateFontIndirect(&LogFont);
			if (hHorizFont)
				SelectObject(hDC, hHorizFont);
		}
	}

	// SetTextJustification(hDC, 0, 0);

	// Compute extents
	SIZE Size;
	INT bRes = GetTextExtentExPointW(hDC, pwString, iCount, 0, NULL, lpCharWidths, &Size);
	// Do not ASSERT: some fonts support characters with 0 width (A+B+C)
	//ASSERT(Size.cx && Size.cy && *lpCharWidths);
	if (0 == bRes)
	{
		DWORD dwGLE = GetLastError();
		// panic
		if (!SCComputeTextExtentPointW(hDC, pwString, iCount, lpCharWidths, Size))
		{
			// panic panic
			if (hHorizFont)
			{
				SelectObject(hDC, hFont);
				DeleteObject(hHorizFont);
			}
			return FALSE;
		}
	}
	
	// Convert extents to char widths
	for (INT i = iCount - 1; i>0; i--)
		lpCharWidths[i] -= lpCharWidths[i-1];

	if (hHorizFont)
	{
		SelectObject(hDC, hFont);
		DeleteObject(hHorizFont);
	}
	return TRUE;
}

BOOL SCGetTextCharWidthsHImW(HDC hDC, LPCWSTR pwString, INT iCount, INT *lpCharWidths, float fScaleX, float fScaleY)
{
	ASSERT(lpCharWidths);
	if (!lpCharWidths)
		return FALSE;

	BOOL bResult = TRUE;

	// We want the horizontal extents (otherwise, for a vertical font,
	// GetTextExtentExPoint would return character heights in lpCharWidths)
	HFONT hFont = NULL;
	HFONT hHorizFont = NULL;
	if (hFont = (HFONT)GetCurrentObject(hDC, OBJ_FONT))
	{
		LOGFONT LogFont;
		::GetObject(hFont, sizeof(LogFont), &LogFont);
		if (LogFont.lfEscapement)
		{
			LogFont.lfEscapement = 0;
			LogFont.lfOrientation = 0;
			hHorizFont = (HFONT)CreateFontIndirect(&LogFont);
			if (hHorizFont)
				SelectObject(hDC, hHorizFont);
		}
	}

	// SetTextJustification(hDC, 0, 0);

	// Compute extents
	SIZE Size;
	INT bRes = GetTextExtentExPointW(hDC, pwString, iCount, 0, NULL, lpCharWidths, &Size);

	ASSERT(Size.cx && Size.cy && *lpCharWidths);
	if (0 == bRes)
	{
		DWORD dwGLE = GetLastError();
		// panic
		bResult = SCComputeTextExtentPointW(hDC, pwString, iCount, lpCharWidths, Size);
	}
	
	// Convert extents to char widths
	if (bResult)
	{
		for (INT i = iCount - 1; i>0; i--)
			lpCharWidths[i] -= lpCharWidths[i-1];

		// text aspect ratio
		float fRatio = (float)(fabs(fScaleY)/fabs(fScaleX));
		for (INT j = 0; j<iCount; j++)
			lpCharWidths[j] = (int)(lpCharWidths[j]*fRatio);
	}

	if (hHorizFont)
	{
		SelectObject(hDC, hFont);
		DeleteObject(hHorizFont);
	}
	return bResult;
}


///
/// Expand a monochrome DIB bitmap to and array of lWidth x lHeight pixels.
/// If lWidth x lHeight is larger than the monochrome bitmap, the latter is replicated.
///
BYTE* SCExpandMask(LONG lWidth, LONG lHeight,
				   LONG lxMask, LONG lyMask,
				   LPCBYTE pBitsMask, LPCBITMAPINFO pBmiMask, DWORD dwUsageMask, BOOL bTopDown/*=FALSE*/)
{
	ASSERT(pBitsMask && pBmiMask);
	ASSERT(dwUsageMask==2/*DIB_PAL_INDICES*/);
	ASSERT(lWidth>0 && lHeight>0);
	ASSERT(1==pBmiMask->bmiHeader.biBitCount && 1==pBmiMask->bmiHeader.biPlanes);
	ASSERT(pBmiMask->bmiHeader.biHeight>0);

	BYTE* pResult = new BYTE[lWidth*lHeight];
	BYTE* pDest = pResult;

	// Note: the mask is always a 1-bit DIB. And:
	// "1-bit DIBs are stored using each bit as an index into the color table.
	// The most significant bit is the leftmost pixel."
	DWORD dwBytesPerLine = WIDTHBYTES(pBmiMask->bmiHeader.biWidth);
	DWORD dwStartScan = lyMask * dwBytesPerLine;
	// Theoretical first useful bit inside the first useful byte on scanline
	BYTE bStartBitNumber = (BYTE)(lxMask % 8);
	// Theoretical max useful bit inside the last useful byte on scanline
	BYTE bMaxBitNumber = (BYTE)(pBmiMask->bmiHeader.biWidth % 8);
	DWORD dwMaxByteOnLine = pBmiMask->bmiHeader.biWidth >> 3;
	
	BYTE* pXStart = (BYTE*)pBitsMask + dwStartScan;
	BYTE* pYLimit = (BYTE*)pBitsMask + (pBmiMask->bmiHeader.biHeight-1) * dwBytesPerLine;

	if (bTopDown)
	{// Make the layout for a top-down bitmap
		for (LONG y=0; (y<lHeight); y++)
		{
			BYTE* pXLimit = pXStart + dwMaxByteOnLine;	 // inclusive end of scanline
			BYTE* pX = pXStart + (lxMask >> 3);			 // byte on the scanline
			BYTE bBitNumber = bStartBitNumber;			 // theoretical bit inside the byte
			for (LONG x=0; (x<lWidth); x++, pDest++)
			{
				// the most significant bit is the leftmost pixel; so get the complement
				// of the theoretical bit number
				if (*pX & (1<<(7 - bBitNumber)))
					*pDest = 1;
				else
					*pDest = 0;
				
				bBitNumber++;
				if (pX==pXLimit)
				{// last useful bit on last useful byte reached: restart
					if (bBitNumber>=bMaxBitNumber)
					{
						pX = pXStart + (lxMask >> 3);
						bBitNumber = bStartBitNumber;
					}
				} else
				if (bBitNumber>7)
				{// no more bits available in this byte: go to next byte
					++pX;
					if (pX>pXLimit)
					{// can't go next: back to start
						pX = pXStart + (lxMask >> 3);
						bBitNumber = bStartBitNumber;
					} else
						bBitNumber = 0; // first bit of next byte
				}
			}
			// next scan
			pXStart += dwBytesPerLine;
			if (pXStart>pYLimit)
			{// restart to begining of mask
				pXStart = (BYTE*)pBitsMask + dwStartScan;
			}
		}
	} else
	{// Make the layout for a bottom-up bitmap.
		BYTE* pDestRow = pResult + lWidth*lHeight;
		for (LONG y=0; (y<lHeight); y++)
		{
			BYTE* pXLimit = pXStart + dwMaxByteOnLine;		// inclusive end of scanline
			BYTE* pX = pXStart + (lxMask >> 3);				// byte on the scanline
			BYTE bBitNumber = bStartBitNumber;				// theoretical bit inside the byte
			
			pDestRow -= lWidth;
			pDest = pDestRow;
			
			for (LONG x=0; (x<lWidth); x++, pDest++)
			{
				// the most significant bit is the leftmost pixel; so get the complement
				// of the theoretical bit number
				if (*pX & (1<<(7 - bBitNumber)))
					*pDest = 1;
				else
					*pDest = 0;
				
				bBitNumber++;
				if (pX==pXLimit)
				{// last useful byte reached
					if (bBitNumber>=bMaxBitNumber)
					{// last useful bit on last useful byte reached: restart
						pX = pXStart + (lxMask >> 3);
						bBitNumber = bStartBitNumber;
					}
				} else
				if (bBitNumber>7)
				{// no more bits available in this byte: go to next byte
					if (++pX>pXLimit)
					{// can't go next: back to start
						pX = pXStart + (lxMask >> 3);
						bBitNumber = bStartBitNumber;
					} else
						bBitNumber = 0; // first bit of next byte
				}
			}
			// next scan
			pXStart += dwBytesPerLine;
			if (pXStart>pYLimit)
			{// restart to begining of mask
				pXStart = (BYTE*)pBitsMask + dwStartScan;
			}
		}
	}
	return pResult;
}

///
/// Create a transparent bitmap from a DIB bitmap and masking information.
/// Note: the mask array must conatain exactly
///		  pSrcBmi->bmiHeader.biWidth x pSrcBmi->bmiHeader.biHeight pixels
///
HBITMAP SCBuildAlphaBitmap(LPCBYTE pSrcBits, LPCBITMAPINFO pSrcBmi, DWORD dwSrcUsage, BYTE* pMask, BOOL bFlip/*=FALSE*/)
{
	// Create a packed-DIB header.
	DWORD dwSize = sizeof(BITMAPINFOHEADER);
	BYTE* pBytes = new BYTE[dwSize];
	ASSERT(pBytes);
	if (!pBytes)
		return NULL;

	// Finish Initialize header.
	int width = pSrcBmi->bmiHeader.biWidth;
	int height = abs(pSrcBmi->bmiHeader.biHeight);
	BITMAPINFOHEADER* pHdr = &((BITMAPINFO*)pBytes)->bmiHeader;
	pHdr->biSize = sizeof(BITMAPINFOHEADER);
	pHdr->biWidth = width;
	pHdr->biHeight = (bFlip) ? -pSrcBmi->bmiHeader.biHeight : pSrcBmi->bmiHeader.biHeight;
	pHdr->biPlanes = 1;
	pHdr->biBitCount = 32;
	pHdr->biCompression = BI_RGB;
	pHdr->biClrUsed = 0;
		// just set the rest to 0
    pHdr->biSizeImage = 0;
    pHdr->biXPelsPerMeter = 0; 
    pHdr->biYPelsPerMeter = 0;
    pHdr->biClrImportant = 0;
    pHdr->biSizeImage = width * height * 4;
	
	// Create the surface.
	HDC    hRefDC = GetDC( NULL );
	LPVOID pBits;
	HBITMAP hbm = CreateDIBSection(hRefDC, (BITMAPINFO*)pBytes, DIB_RGB_COLORS, &pBits, NULL, 0);
	ASSERT(hbm);
	if (hbm)
	{
		// Copy the source bits. Note:
		// "GDI automatically inverts the image during the Set and Get operations."
		// 'invert' means 'flip upside_down/bottom_up'.
		SetDIBits(hRefDC, hbm, 0, height, (CONST VOID *)pSrcBits, (BITMAPINFO*)pSrcBmi, dwSrcUsage);

		// Merge alpha information (mask)
		GdiFlush();

		BYTE* pAlpha = pMask;
		UINT32 *puiVal = (UINT32 *)pBits;
		UINT32 *pStop = puiVal + height*width;
		if (bFlip)
		{// top-down DIB
			while (puiVal < pStop)
			{
				if (1==*pAlpha++)
				{// opaque
					// A value of 1 in the mask indicates opaque pixel.
					*puiVal |= 0xff000000;
				} // A value of 0 in the mask indicates fully transparent pixel.
				else
					*puiVal &= 0x00ffffff;
				puiVal++;
			}
		} else
		{// bottom-up DIB
			UINT32 *puiRow = pStop;
			BYTE *pAlphaRow = pMask + height*width;
			while (puiRow>pBits)
			{
				puiRow -= width;
				pAlphaRow -= width;

				puiVal = puiRow;
				pAlpha = pAlphaRow;
				for (LONG x=0; (x<width); x++)
				{
					if (1==*pAlpha++)
						*puiVal |= 0xff000000;
					else
						*puiVal &= 0x00ffffff;
					puiVal++;
				}
			}
		}
	}

	ReleaseDC( NULL, hRefDC );
	delete [] pBytes;
	return(hbm);
}

BYTE SGGetMonoDIBPixel(LPBYTE pBits, DWORD dwWidth, DWORD dwHeight, DWORD x, DWORD y)
{
    // Find the byte on which the scanline begins + (the byte containing the pixel)
    DWORD dwByteIndex = (dwHeight - y - 1) * WIDTHBYTES(dwWidth) + (x >> 3);

    // Which bit is it?
    BYTE bBitNumber = (BYTE)( 7 - (x % 8) );

    return (pBits[dwByteIndex] & (1<<bBitNumber));
}


///
///	Tell if hDC is a monochrome device.
///
BOOL SCIsMonochromeDC(HDC hDC)
{
	HBITMAP hBM = (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP);
	if (hBM)
	{
		BITMAP bm;
		GetObject(hBM, sizeof(bm), &bm);
		return (bm.bmBitsPixel==1 && bm.bmPlanes==1);
	}
	
	return FALSE;
}

///
///	Fill a two-color palette with current text/background colors of DC.
/// (For monochrome brush color realization)
///
void SCFillMonochromePalette(HDC hDC, PPALETTEENTRY palPalEntry)
{
	ASSERT(palPalEntry);

	COLORREF crBkColor = GetBkColor(hDC);
	COLORREF crTextColor = GetTextColor(hDC);
	palPalEntry[0].peRed = GetRValue(crTextColor);
	palPalEntry[0].peGreen = GetGValue(crTextColor);
	palPalEntry[0].peBlue = GetBValue(crTextColor);
	palPalEntry[0].peFlags = 0;
	palPalEntry[1].peRed = GetRValue(crBkColor);
	palPalEntry[1].peGreen = GetGValue(crBkColor);
	palPalEntry[1].peBlue = GetBValue(crBkColor);
	palPalEntry[1].peFlags = 0;
}

///
/// Make the device dependent version of a DIB
///
HBITMAP SCDIBtoMonochromeBitmap(HDC hDCRef, BITMAPINFO* pBmi, DWORD *pBitsDW)
{
	ASSERT(pBmi);
	ASSERT(pBitsDW);
	ASSERT(hDCRef);

	// Attach a monochrome palette to the DIB specification,
	// and create a device-dependent bitmap
	HBITMAP hBm = NULL;
	DWORD dwSize = pBmi->bmiHeader.biSize + 2*sizeof(RGBQUAD);
	BITMAPINFO* pBmi2 = (BITMAPINFO*) new BYTE[dwSize];
	memmove(pBmi2, pBmi, dwSize);
	
	HDC hMonoDC = CreateCompatibleDC(hDCRef); // must be monochrome
	ASSERT(hMonoDC);
	SCFillMonochromePalette(hDCRef, (PPALETTEENTRY)pBmi2->bmiColors);

	// May create resource leak, as we don't call DeleteObject on hBm
	hBm = CreateDIBitmap(hMonoDC, &pBmi2->bmiHeader, CBM_INIT, pBitsDW, pBmi2, DIB_RGB_COLORS);
	
	DeleteDC(hMonoDC);
	delete [] (BYTE*)pBmi2;
	return hBm;

}

//////////////////////////////////////////////////////////////////////
/// Font family substitution
///
typedef struct tag_SCSysFont
{
	TCHAR*	szFacename;
	DWORD	dwFamily;
} SCSysFont, *PSCSysFont;

// Vector and Raster fonts supposed to be on all systems
SCSysFont s_szSysVRFonts[] = {
	_T("modern"),			FF_MODERN, //	Vector
	_T("roman"),			FF_ROMAN, //	Vector
	_T("script"),			FF_DONTCARE, //	Vector
	_T("courier"),			FF_ROMAN, //	Raster
	_T("fixedsys"),			FF_SWISS, //	Raster
	_T("ms sans serif"),	FF_SWISS, //	Raster
	_T("ms serif"),			FF_ROMAN, //	Raster
	_T("small fonts"),		FF_SWISS, //	Raster
	_T("symbol"),			FF_DONTCARE, //	Raster
	_T("system"),			FF_SWISS, //	Raster
	_T("terminal"),			FF_SWISS //	Raster
};
//		_T("arial"),			FF_SWISS,	//	TrueType
//		_T("courier new"),		FF_ROMAN,	//	TrueType
//		_T("symbol"),			FF_DONTCARE,//	TrueType
//		_T("times new roman"),	FF_ROMAN,	//	TrueType
//		_T("wingdings"),		FF_DONTCARE,//	TrueType
//		_T("helv"),				FF_SWISS,
//		_T("helvetica"),		FF_SWISS,
//		_T("tms rmn"),			FF_ROMAN,
//		dutch->times
//		bookman->times

int s_lNbSysVRFonts = sizeof(s_szSysVRFonts)/sizeof(SCSysFont);
DWORD SCFontFamilyApproximant(LPCTSTR lpszFacename)
{
	TCHAR szName[LF_FACESIZE+1];
	_tcsncpy(szName, lpszFacename, LF_FACESIZE);
	szName[LF_FACESIZE]=0;
	_tcslwr(szName);

	for (int i=0; (i<s_lNbSysVRFonts); i++)
		if (0==_tcscmp(s_szSysVRFonts[i].szFacename, szName))
			return s_szSysVRFonts[i].dwFamily;

	return FF_DONTCARE;
}

inline DWORD SCFontFamilyApproximantA(LPCSTR lpszFacename)
{
	return SCFontFamilyApproximant((TCHAR*)lpszFacename);
}

DWORD SCFontFamilyApproximantW(LPCWSTR lpwszFacename)
{
#ifdef _UNICODE
	return SCFontFamilyApproximant(lpwszFacename);
#else
	char	szFaceName[LF_FACESIZE];
	WideCharToMultiByte(CP_ACP, 0, lpwszFacename, LF_FACESIZE, szFaceName, LF_FACESIZE, NULL, NULL);
	return SCFontFamilyApproximant((LPCSTR)szFaceName);
#endif
}

BOOL SCIsIdentityXFORM(XFORM& rXForm)
{
	return (rXForm.eM11==1.0 && rXForm.eM22==1.0 &&
			rXForm.eM12==0.0 && rXForm.eM21==0.0 &&
			rXForm.eDx==0.0 && rXForm.eDy==0.0);
}

HDC SCGetDefaultPrinterDC(HWND hwndOwner/*=NULL*/)
{
	PRINTDLG pd;
	{
		pd.lStructSize = sizeof (PRINTDLG);
		BOOL bExistPrinter = (AfxGetApp()->GetPrinterDeviceDefaults(&pd));
#ifdef _DEBUG
		{// spying
			DEVMODE * pDevMode = (DEVMODE *)::GlobalLock(pd.hDevMode);
			::GlobalUnlock(pd.hDevMode);
		}
#endif
		pd.hwndOwner = hwndOwner;
		pd.hDevMode = (HANDLE)NULL;
		pd.hDevNames = (HANDLE)NULL;
		pd.nFromPage = 0;
		pd.nToPage = 0;
		pd.nMinPage = 0;
		pd.nMaxPage = 0;
		pd.nCopies = 0;
		pd.hInstance = (HINSTANCE)AfxGetApp()->m_hInstance;
		pd.Flags = PD_RETURNDEFAULT|PD_RETURNDC;
		pd.lpfnSetupHook = (LPSETUPHOOKPROC)(FARPROC)NULL;
		pd.lpSetupTemplateName = (LPTSTR)NULL;
		pd.lpfnPrintHook = (LPPRINTHOOKPROC)(FARPROC)NULL;
		pd.lpPrintTemplateName = (LPTSTR)NULL;
		if (bExistPrinter && (PrintDlg(&pd) == TRUE))
			return pd.hDC;
	}
	return NULL;
}


////////////////////////////////////////////////////////////////////////////
//
// Test stuff
//
void SCTestShadedRectangle(HDC hDC, LPCRECT pRect, int iMode)
{
	ASSERT(hDC);
	ASSERT(pRect);

	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;
	vert [0] .x      = pRect->left;
	vert [0] .y      = pRect->top;
	vert [0] .Red    = 0x0000;
	vert [0] .Green  = 0x0000;
	vert [0] .Blue   = 0x0000;
	vert [0] .Alpha  = 0x0000;
	
	vert [1] .x      = pRect->right;
	vert [1] .y      = pRect->bottom; 
	vert [1] .Red    = 0x0000;
	vert [1] .Green  = 0x0000;
	vert [1] .Blue   = 0xff00;
	vert [1] .Alpha  = 0x0000;
	
	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill(hDC, vert, 2, &gRect, 1, iMode);
}

void SCTestShadedTriangle(HDC hDC, LPCRECT pRect)
{
	ASSERT(hDC);
	ASSERT(pRect);

	TRIVERTEX			vert[4];
	GRADIENT_TRIANGLE    gTri[2];
	vert [0] .x       =  (pRect->left+pRect->right)/2;
	vert [0] .y       =  pRect->top;
	vert [0] .Red     =  0x0000;
	vert [0] .Green   =  0x0000;
	vert [0] .Blue    =  0x0000;
	vert [0] .Alpha   =  0x0000;
	
	vert [1] .x       =  pRect->right;
	vert [1] .y       =  (pRect->top+pRect->bottom)/2;
	vert [1] .Red     =  0x0000;
	vert [1] .Green   =  0x0000;
	vert [1] .Blue    =  0xff00;
	vert [1] .Alpha   =  0x0000;
	
	vert [2] .x       =  (pRect->left+pRect->right)/2;
	vert [2] .y       =  pRect->bottom; 
	vert [2] .Red     =  0xff00;
	vert [2] .Green   =  0x0000;
	vert [2] .Blue    =  0xff00;
	vert [2] .Alpha   =  0x0000;
	
	vert [3] .x       =  pRect->left;
	vert [3] .y       =  (pRect->top+pRect->bottom)/2;
	vert [3] .Red     =  0x0000;
	vert [3] .Green   =  0xff00;
	vert [3] .Blue    =  0xff00;
	vert [3] .Alpha   =  0x0000;
	
	gTri[0].Vertex1   = 0;
	gTri[0].Vertex2   = 1;
	gTri[0].Vertex3   = 2;
	
	gTri[1].Vertex1   = 0;
	gTri[1].Vertex2   = 2;
	gTri[1].Vertex3   = 3;
	GradientFill(hDC, vert, 4, &gTri[0], 2, GRADIENT_FILL_TRIANGLE);
}

void SCEMFExplorerPaint(CDC* pDC, const CRect* pRect, LPCTSTR lpszMsgID/*=_T("")*/, LPCTSTR lpszMsgAction/*=_T("")*/)
{
	ASSERT(pDC);
	ASSERT(pRect);

	SCTestShadedRectangle(pDC->m_hDC, pRect, GRADIENT_FILL_RECT_V);
	SCTestShadedTriangle(pDC->m_hDC, pRect);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255, 0, 255));

	LOGFONT LogFont;
	memset(&LogFont, 0, sizeof(LogFont));
	LogFont.lfHeight         = -20;
	LogFont.lfWeight         = FW_BOLD;
	LogFont.lfCharSet        = ANSI_CHARSET;
	LogFont.lfOutPrecision   = OUT_TT_PRECIS;
	LogFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	LogFont.lfQuality        = DEFAULT_QUALITY;
	LogFont.lfPitchAndFamily = FF_ROMAN|DEFAULT_PITCH;
	_tcscpy ((TCHAR*)LogFont.lfFaceName, _T("Times New Roman"));
	
	HFONT hFont = CreateFontIndirect(&LogFont);
	HFONT hOldFont = (HFONT)SelectObject(pDC->m_hDC, hFont);

	int iY = 0;
	CString strMsg = (lpszMsgID) ? lpszMsgID : _T("");
	if (!strMsg.IsEmpty())
	{
		CSize size = pDC->GetTextExtent(strMsg);
		iY = (pRect->Height() - 3*size.cy)/2;
		
		pDC->TextOut((pRect->Width() - size.cx)/2, iY, strMsg);
		iY += 2*size.cy;
	}

	strMsg = (lpszMsgAction) ? lpszMsgAction : _T("");
	if (!strMsg.IsEmpty())
	{
		CSize size = pDC->GetTextExtent(strMsg);
		if (!iY)
			iY = (pRect->Height() - 3*size.cy)/2;
		pDC->TextOut((pRect->Width() - size.cx)/2, iY, strMsg);
	}

	DeleteObject(SelectObject(pDC->m_hDC,hOldFont));
}
////////////////////////////////////////////////////////////////////////////
