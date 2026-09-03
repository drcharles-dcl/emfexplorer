/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMF.h"

#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCBitmap.h)
#include SC_INC_WINLIB(SCMemDC.h)
#include SC_INC_WINLIB(SCRichEdit.h)
#include SC_INC_WINLIB(SCGDIUtils.h)
#include <wingdi.h>

//	#ifdef _DEBUG
//	#define new DEBUG_NEW
//	#undef THIS_FILE
//	static char THIS_FILE[] = __FILE__;
//	#endif

// 1 inch => 2540 0.01mm
#define L001MMPERINCH 2540L

//	#define SC_ADJUST_EMFDPI

#define SC_PRIVILEGE_EMFDPI

// Get DPI of EMF
BOOL SCGetEMFDPIs(HENHMETAFILE hEMF, long &lDPIX, long &lDPIY)
{
	long lEmfPaperCx;
	long lEmfPaperCy;
	return SCGetEMFInfos(hEMF, lDPIX, lDPIY, lEmfPaperCx, lEmfPaperCy);
}

// Get DPI and paper size of EMF
BOOL SCGetEMFInfos(HENHMETAFILE hEMF, long &lDPIX, long &lDPIY, long &lPaperCx, long &lPaperCy)
{
	ENHMETAHEADER EmfHeader;
	if (hEMF && ::GetEnhMetaFileHeader(hEMF, sizeof(ENHMETAHEADER), &EmfHeader))
	{
		// this paper size is not reliable (there are a lot of 1024x768 around there)
		lPaperCx = EmfHeader.szlDevice.cx;
		lPaperCy = EmfHeader.szlDevice.cy;

		// or this paper size (in mm) is not reliable (there are a lot of 320x240 around there,
		// yielding 81x81 DPI device)
		lDPIX = MulDiv(lPaperCx, 254, EmfHeader.szlMillimeters.cx*10L);
		lDPIY = MulDiv(lPaperCy, 254, EmfHeader.szlMillimeters.cy*10L);

#ifdef SC_ADJUST_EMFDPI
		// TODO: fix this or undef SC_ADJUST_EMFDPI
		// Ask Microsoft how to get 96 for EMF taylored on a 96 DPI device
		if (81==lDPIX)
			lDPIX = 96;
		if (81==lDPIY)
			lDPIY = 96;
#endif
		return TRUE;
	}
	
	return FALSE;
}


//------------------------------------
#define SC_KEEP_XY		0
#define SC_KEEP_Y		1
#define SC_KEEP_X		2
//	#define SC_USE_BOUNDS
///
/// Get size of the given EMF ( doesn't take page rotations into account).
/// (This function attempts to guess the best size to use in order to display an EMF
/// at actual size)
///
BOOL SCGetEMFPlaySize(HENHMETAFILE hEMF, CSize& sizeEMF, HDC hDC/*=NULL*/, float* pfScale/*=NULL*/)
{
	ASSERT(hEMF);

	// Collect information about the EMF
	ENHMETAHEADER EmfHeader;
	if (!::GetEnhMetaFileHeader(hEMF, sizeof(ENHMETAHEADER), &EmfHeader))
		return FALSE;

#ifdef SC_USE_BOUNDS
	sizeEMF.cx = EmfHeader.rclBounds.right - EmfHeader.rclBounds.left + 1;
	sizeEMF.cy = EmfHeader.rclBounds.bottom - EmfHeader.rclBounds.top + 1;
	if (sizeEMF.cx<=0)
		sizeEMF.cx = 1;
	if (sizeEMF.cy<=0)
		sizeEMF.cy = 1;
#else
	long lEmfDPIX = 0;
	long lEmfDPIY = 0;
	long lEmfPaperCx = 0;
	long lEmfPaperCy = 0;
	if (!SCGetEMFInfos(hEMF, lEmfDPIX, lEmfDPIY, lEmfPaperCx, lEmfPaperCy))
		return FALSE;

	int bKeep = SC_KEEP_XY;
	int iWidth, iHeight;

	{// Scale dimensions to ensure correct aspect ratio
		BOOL bTmpDC = (NULL==hDC);
		if (bTmpDC)
		{
			hDC = ::GetDC(NULL);
		}
		ASSERT(hDC);
		if (!hDC)
			return FALSE;

		// adjust paper dimensions for device play
		// (for DPI independence, in case EMF DPIs and DPIs are different)
		if (!SCScaleImgSurface(hDC, lEmfDPIX, lEmfDPIY, lEmfPaperCx, lEmfPaperCy))
		{
			ASSERT(0);
			return FALSE;
		}

		// compute which paper dimension to keep, while preserving aspect ratio
		// (always keep the smaller dimension)
#ifdef SC_PRIVILEGE_EMFDPI
		// Using supposed EMF DPI instead of device DPI
		iWidth = MulDiv(EmfHeader.rclFrame.right - EmfHeader.rclFrame.left + 1, lEmfDPIX, L001MMPERINCH);
		iHeight = MulDiv(EmfHeader.rclFrame.bottom - EmfHeader.rclFrame.top + 1, lEmfDPIY, L001MMPERINCH);
#else
		GetEMFDimension(hDC, EmfHeader, iWidth, iHeight);
#endif
		if (!SCScaleImgSurface(hDC, lEmfDPIX, lEmfDPIY, (long&)iWidth, (long&)iHeight))
		{
			ASSERT(0);
			return FALSE;
		}
		if (iWidth<=0)
			iWidth = 1;
		if (iHeight<=0)
			iHeight = 1;

		if ((iWidth>lEmfPaperCx) || (iHeight>lEmfPaperCy))
		{// EMF was taylored with different resolution
			if (lEmfPaperCx>=lEmfPaperCy)
			{// keep y, and reduce x
				bKeep = SC_KEEP_Y;
			} else
			{// keep x, and reduce y
				bKeep = SC_KEEP_X;
			}
		} else
		{
			lEmfPaperCx = iWidth;
			lEmfPaperCy = iHeight;
		}

		// done with DC
		if (bTmpDC)
			::ReleaseDC(NULL,hDC);
	}

	// Final dimensions
	if (SC_KEEP_XY==bKeep)
	{
		sizeEMF.cx = lEmfPaperCx;
		sizeEMF.cy = lEmfPaperCy;
		if (pfScale)
			*pfScale = 1;
	} else
	if (SC_KEEP_Y==bKeep)
	{
		sizeEMF.cy = min(lEmfPaperCy, iHeight);
		sizeEMF.cx = ::MulDiv(sizeEMF.cy, iWidth, iHeight);
		if (sizeEMF.cx<=0)
			sizeEMF.cx = 1;
		if (pfScale)
			*pfScale = float(sizeEMF.cx)/float(lEmfPaperCx);
	} else
	{
		sizeEMF.cx = min(lEmfPaperCx, iWidth);
		sizeEMF.cy = ::MulDiv(sizeEMF.cx, iHeight, iWidth);
		if (sizeEMF.cy<=0)
			sizeEMF.cy =1;
		if (pfScale)
			*pfScale = float(sizeEMF.cy)/float(lEmfPaperCy);
	}
	// Note: at this point sizeEMF may be equal to rclBounds, meaning it may exceed
	// paper size (most likely for thin, long, images that require floating points
	// calculation for proper scaling).
#endif
	return TRUE;
}

#undef SC_KEEP_XY
#undef SC_KEEP_Y
#undef SC_KEEP_X
//-------------------------------

///
/// Smallest rectangle surrounding the image.
///
BOOL SCGetEMFElemsRect(HENHMETAFILE hEMF, CRect& rcElems, fnSCPlayEnhMetaFile fnPlayEMF/*=NULL*/)
{
	ENHMETAHEADER EmfHeader;
	if (hEMF && ::GetEnhMetaFileHeader(hEMF, sizeof(ENHMETAHEADER), &EmfHeader))
	{
#if 0
	// the 81 DPI problem would cumulate
		// compute bounds in pixels
		HDC hDC = ::GetDC(NULL);
		ASSERT(hDC);
		if (!hDC)
			return FALSE;
		
		// compute actual EMF DPIs, based on the dimensions of its creation device
		long lEmfDPIX = MulDiv(EmfHeader.szlDevice.cx, 254, EmfHeader.szlMillimeters.cx*10L);
		long lEmfDPIY = MulDiv(EmfHeader.szlDevice.cy, 254, EmfHeader.szlMillimeters.cy*10L);

#ifdef SC_ADJUST_EMFDPI
		// see SCGetEMFInfos for documentation
		if (81==lEmfDPIX)
			lEmfDPIX = 96;
		if (81==lEmfDPIY)
			lEmfDPIY = 96;
		//
#endif
		
		// scale the surface for DPI independence
		long lDevDPIX = ::GetDeviceCaps(hDC, LOGPIXELSX);
		long lDevDPIY = ::GetDeviceCaps(hDC, LOGPIXELSY);
		::ReleaseDC(NULL,hDC);
		ASSERT(lDevDPIX);
		ASSERT(lDevDPIY);

		rcElems.left = MulDiv(EmfHeader.rclBounds.left, lDevDPIX, lEmfDPIX);
		rcElems.top = MulDiv(EmfHeader.rclBounds.top, lDevDPIY, lEmfDPIY);
		rcElems.right = MulDiv(EmfHeader.rclBounds.right, lDevDPIX, lEmfDPIX);
		rcElems.bottom = MulDiv(EmfHeader.rclBounds.bottom, lDevDPIY, lEmfDPIY);

		// Ckeck bounds
		int iWidth = rcElems.Width();
		int iHeight = rcElems.Height();
		if (iWidth && iHeight)
		{
			// Ok! This is my bug: I won't allow an EMF whose dimensions exceed its device
			// to display at actual size.
			if (iWidth<=EmfHeader.szlDevice.cx && iHeight<=EmfHeader.szlDevice.cy)
			{
				// We take whatever is given
				return TRUE;
			} // else big EMFs will be sized down
		}
#endif
		SCComputeEMFBlackBox(hEMF, rcElems, fnPlayEMF);
		return TRUE;
	}

	return FALSE;
}

///
///  Compute EMF frame dimension in pixels.
///
void GetEMFDimension(HDC hDC, ENHMETAHEADER &emfh, int & width, int & height)
{
	// Calculate the rect in which to draw the metafile based on the
	// intended size and the current output device resolution.
	// Remember that the intended size is given in 0.01 mm units, so
	// convert those to device units on the target device.

#ifdef SC_PRIVILEGE_EMFDPI
	// Compute actual EMF DPIs, based on the dimensions of its creation device,
	// and prefer these DPIs over output device DPI
	int iDpiX = MulDiv(emfh.szlDevice.cx, 254, emfh.szlMillimeters.cx*10L);
	int iDpiY = MulDiv(emfh.szlDevice.cy, 254, emfh.szlMillimeters.cy*10L);
#else
	// Get the characteristics of the output device.
	int iDpiX = GetDeviceCaps(hDC,  LOGPIXELSX);
	int iDpiY = GetDeviceCaps(hDC,  LOGPIXELSY);
#endif
	
	// Convert from 0.01 mm to pixels.
	RECTL& rRect = emfh.rclFrame;
	width = MulDiv(rRect.right - rRect.left, iDpiX, L001MMPERINCH);
	height = MulDiv(rRect.bottom - rRect.top, iDpiY, L001MMPERINCH);
}

// Scale an image surface according to a destination device resolution
BOOL SCScaleImgSurface(HDC hDC, long lImgDPIX, long lImgDPIY, long &lImgCx, long &lImgCy)
{
	ASSERT(hDC);
	if (!hDC)
		return FALSE;

	long lDevDPIX = ::GetDeviceCaps(hDC, LOGPIXELSX);
	long lDevDPIY = ::GetDeviceCaps(hDC, LOGPIXELSY);

	ASSERT(lDevDPIX);
	ASSERT(lDevDPIY);
	if (lDevDPIX && lDevDPIY)
	{
		lImgCx = MulDiv(lImgCx, lDevDPIX, lImgDPIX);
		lImgCy = MulDiv(lImgCy, lDevDPIY, lImgDPIY);
		return TRUE;
	}
	return FALSE;
}

//
// Compute the dimensions, in device units, of the smallest rectangle that can be drawn
// around the picture stored in the metafile.
// Note: this is the job of Windows. But rclBounds is not always accurate. So we are
// taking an actual snapshot of the black box on a memory DC compatible with the screen.
//
// Note: Beware that this function clips any white-on-white element laying on the EMF's
// boundaries. That's the reason why ElemsRect may be smaller than EmfHeader.rclBounds.
// Empty lines are good examples of white-on-white elements.
//
BOOL SCComputeEMFBlackBox(HENHMETAFILE hEmf, CRect& ElemsRect, fnSCPlayEnhMetaFile fnPlayEMF/*=NULL*/)
{
	// We try to create the emf handle
	ASSERT(hEmf);
	ENHMETAHEADER EmfHeader;
	if (!hEmf || !::GetEnhMetaFileHeader(hEmf, sizeof(ENHMETAHEADER), &EmfHeader))
		return FALSE;
#ifdef SC_USE_BOUNDS
	// If we could rely on rclBounds
	CopyRect(&ElemsRect, (LPCRECT)&EmfHeader.rclBounds);
#else
	// Collect information from the EMF
	CSize sizeEMF;
	SCGetEMFPlaySize(hEmf, sizeEMF);
	CRect rcPlay(0, 0, sizeEMF.cx, sizeEMF.cy);

	// Draw to extract bounds (use a large surface)
	ElemsRect.CopyRect((LPRECT)&rcPlay);

	CSCMemDC MemDC;
	MemDC.SCPrepareSurface(sizeEMF.cx, sizeEMF.cy, hEmf);
	//MemDC.PatBlt(0, 0, sizeEMF.cx, sizeEMF.cy, WHITENESS); // background is white

	// Play in rcPlay to conform to aspect ratio
	BOOL bOk;
	if (NULL==fnPlayEMF)
		bOk = ::PlayEnhMetaFile(MemDC.m_hDC, hEmf, (LPRECT)&rcPlay);
	else
		bOk = fnPlayEMF(MemDC.m_hDC, hEmf, (LPRECT)&rcPlay);

	// Extract bounding box (start out with the whole bitmap surface)
	// final ElemsRect should be smaller or equal to this rcPlay
	CRect& R = ElemsRect;
	
	PBITMAPINFOHEADER pbih;     // bitmap info-header
	HBITMAP hDib = MemDC.STGetDCDIB();
	ASSERT(hDib);
	
	// Retrieve the information describing the DIB.
	DIBSECTION ds;
	::GetObject(hDib, sizeof(DIBSECTION), &ds);
	pbih = (PBITMAPINFOHEADER)&ds.dsBmih;
	ASSERT(ds.dsBm.bmBits); // must be a DIB
	
	// Compute image size in pixels
	DWORD dwBitsSize = (pbih->biSizeImage!=0) ?
		pbih->biSizeImage : abs(pbih->biHeight)*(((pbih->biWidth*pbih->biBitCount + 31) & ~31) >> 3);

	BYTE *pBottom = SCExtractMemBoundingBox((LPBYTE)ds.dsBm.bmBits, &R, dwBitsSize,
							ds.dsBm.bmWidthBytes, (short)ds.dsBm.bmBitsPixel);
	ASSERT(pBottom);
	
	if (!::IsRectEmpty(&R))
	{
		// adjustment for bottom-up image
		if (pbih->biHeight>0)
		{
			long t = rcPlay.bottom - R.bottom - 1;	  // new top = old height - pos of bottom pixel
			long b = t + (R.bottom - R.top);      // new top + new height
			// security
			ASSERT(t>=-1);
			if (t<0)
				t = 0;
			
			ASSERT(b>=-1);
			if (b<0)
				b = 0;
			
			// adjust rect
			R.top = t;		// number of blank pixels at the top
			R.bottom = b+1;	// below the bottom pixel
			R.right++;		// at right of the rightmost pixel
			// R.left = number of blank pixels on the left
		} else
		{ // adjust rect, for pixels on the left column and at the bottom line
			R.bottom++;
			R.right++;
		}
		
		ASSERT(R.left>=0);
		ASSERT(R.top>=0);
		ASSERT(R.right<=rcPlay.right);
		ASSERT(R.bottom<=rcPlay.bottom);
	}
#endif
	return TRUE;
}

///
/// Metafile to Enhanced Metafile format
///
HENHMETAFILE SCConvertWMFtoEMF(LPCTSTR lpszFname)
{
	// Open the file for reading. 
	CFile sourceFile;
	CFileException ex;
	if (!sourceFile.Open(lpszFname, CFile::modeRead, &ex))
		return NULL;

	UINT uiSize = sourceFile.GetLength();
	LPBYTE pBuffer = new BYTE[uiSize];
	if (!pBuffer)
	{
		sourceFile.Close();
		return NULL;
	}
	DWORD dwNbRead = sourceFile.Read(pBuffer, uiSize);
	sourceFile.Close();

	HENHMETAFILE hEMF = SCConvertWMFtoEMF(dwNbRead, pBuffer);

	// Cleanup objects.
	delete [] pBuffer;
	return hEMF;
}

HENHMETAFILE SCConvertWMFtoEMF(DWORD dwSize, LPBYTE pBuffer)
{
	LPBYTE pData = pBuffer;
	UINT uiSize = dwSize;
    APMFILEHEADER*	pAPMHeader = (APMFILEHEADER*)pBuffer;
	APMFILEHEADER	winWMFHeader;

	if (WMFMETA_PLACEABLEKEY==pAPMHeader->key)
	{// aldus
		pData += sizeof(APMFILEHEADER);
		uiSize -= sizeof(APMFILEHEADER);
	} else
	{// windows
		// Oh! You didn't place your metafile? Good for you!
		pAPMHeader = &winWMFHeader;
		memset(pAPMHeader, 0, sizeof(winWMFHeader));
	}
	HWND	ghwndDrawSurf = NULL;
	HDC hDCDrawSurf = GetDC(ghwndDrawSurf);
	
	METAFILEPICT mfPict;
	mfPict.hMF = 0;
	mfPict.mm = MM_ANISOTROPIC;

	METAFILEPICT *lpmfp   = NULL;
	if (pAPMHeader->inch)
	{
		lpmfp   = &mfPict;
		SMALL_RECT* prcPict = &pAPMHeader->bbox;
#if 1 
		mfPict.xExt = MulDiv((long)(prcPict->Right - prcPict->Left), 2540, pAPMHeader->inch);
		mfPict.yExt = MulDiv((long)(prcPict->Bottom - prcPict->Top), 2540, pAPMHeader->inch);
#else
		mfPict.xExt = 0;
		mfPict.yExt = 0;
		// Code from Microsoft's mfedit.c ==> BAD RESULT
		switch (pAPMHeader->inch) { 
			// !!! End up in an upside down image 
			// 
		case 1440: 
			mfPict.mm = MM_TWIPS; 
			break; 
		case 2540: 
			mfPict.mm = MM_HIMETRIC; 
			break; 
		case 254: 
			mfPict.mm = MM_LOMETRIC; 
			break; 
		case 1000: 
			mfPict.mm = MM_HIENGLISH; 
			break; 
		case 100: 
			mfPict.mm = MM_LOENGLISH; 
			break; 
		default: 
			// !!! In addition, text is too small 
			// 
			mfPict.mm = MM_ANISOTROPIC; 
			mfPict.xExt = ((long)(prcPict->Right - prcPict->Left)) * pAPMHeader->inch * 2560; 
			mfPict.yExt = ((long)(prcPict->Bottom - prcPict->Top)) * pAPMHeader->inch * 2560; 
			break; 
		}
#endif
	}
	
	// Generate EMF
	HENHMETAFILE hEMF = SetWinMetaFileBits(uiSize, pData, hDCDrawSurf, lpmfp);

	// Cleanup
	ReleaseDC(ghwndDrawSurf ,hDCDrawSurf);
	return hEMF;
}

///
/// Enhanced Metafile to Windows Metafile format on disk
///
void SCConvertEMFtoWMF(HENHMETAFILE hEMF, LPCTSTR lpszFileName)
{
	LPBYTE       lpEMFBits;
	UINT         uiSizeBuf;
	HDC			 hrefDC = GetDC(NULL);
	
	// Size of the Windows metafile associated with hMF.
	uiSizeBuf = GetWinMetaFileBits(hEMF, 0, NULL, MM_ANISOTROPIC, hrefDC);

	// Memory to hold metafile bits.
	lpEMFBits = new BYTE[uiSizeBuf];
	if (!lpEMFBits)
		return;

	// Bits of the enhanced metafile associated with hEMF.
	GetWinMetaFileBits(hEMF, uiSizeBuf, lpEMFBits, MM_ANISOTROPIC, hrefDC);

	// Copy the bits into a memory-based Windows metafile.
	HMETAFILE    hWMF = SetMetaFileBitsEx(uiSizeBuf, lpEMFBits);

	// Copy the Windows metafile to a disk-based Windows metafile.
	CopyMetaFile(hWMF, lpszFileName);

	// Clean up
	DeleteMetaFile(hWMF);

	delete [] lpEMFBits;
	ReleaseDC(NULL, hrefDC);
}

///
/// Copy the content of the source
///
HMETAFILE SCCopyEMFtoWMF(HENHMETAFILE hEMF, CRect rcSrc, METAFILEPICT *pMtf/*=NULL*/)
{

	CMetaFileDC	 MetaDC;
	HDC			 hAttribDC;
	BOOL		 bWholeEMF = (0==rcSrc.Width() || 0==rcSrc.Height());

	long lEmfDPIX = 0;
	long lEmfDPIY = 0;
	{
		long lEmfPaperCx = 0;
		long lEmfPaperCy = 0;
		if (!SCGetEMFInfos(hEMF, lEmfDPIX, lEmfDPIY, lEmfPaperCx, lEmfPaperCy))
			return NULL;
	}

	CSize sizeEMF;
	SCGetEMFPlaySize(hEMF, sizeEMF);
	CRect rcPlay(0, 0, sizeEMF.cx, sizeEMF.cy);

	// Draw in the metafile.
	MetaDC.Create(NULL);
	MetaDC.SetAttribDC(hAttribDC=CreateCompatibleDC(NULL)); // Required for some query functions working well.

	rcSrc.right++;
	rcSrc.bottom++;

	//MetaDC.SetMapMode(MM_ANISOTROPIC);
	MetaDC.SetWindowOrg(rcSrc.left, rcSrc.top);
	MetaDC.SetWindowExt(rcSrc.Width(), rcSrc.Height());

	if (!bWholeEMF)
	{
		CRgn rgn;
		rgn.CreateRectRgn(rcSrc.left, rcSrc.top, rcSrc.right, rcSrc.bottom);
		MetaDC.SelectClipRgn(&rgn, RGN_COPY);

	}
	MetaDC.PlayMetaFile(hEMF, (LPRECT)&rcPlay);

	HMETAFILE hMeta = MetaDC.Close();
	DeleteDC(hAttribDC);
	
	CPoint pt(rcSrc.Width(), rcSrc.Height());
	{
		HDC hDC = GetDC(NULL);
		int iMapMode = SetMapMode(hDC, MM_HIMETRIC);
		DPtoLP(hDC, &pt, 1);
		SetMapMode(hDC, iMapMode);
		ReleaseDC(NULL, hDC);
	}
	if (hMeta && pMtf)
	{// fill the placement header

		pMtf->hMF = hMeta;
		pMtf->mm = MM_ANISOTROPIC;
		pMtf->xExt = pt.x;
		pMtf->yExt = pt.y;
	}
	APMFILEHEADER apmheader;
	apmheader.key = WMFMETA_PLACEABLEKEY;
	apmheader.hmf = 0; // unused
	apmheader.bbox.Left = apmheader.bbox.Top = 0;
	apmheader.bbox.Right = (SHORT)pt.x;
	apmheader.bbox.Bottom = (SHORT)pt.y;
	apmheader.inch = 1440;
	apmheader.reserved = 0;
	apmheader.checksum = 0;
	{// compute checksum
		for (WORD *p = (WORD*)&apmheader; (p < (WORD*)&(apmheader.checksum)); ++p)
			apmheader.checksum ^= *p;
	} 

	return hMeta;
}

///
/// Close metafile DC and ensure that we never use it again
///
HENHMETAFILE SCCloseEMF(HDC& hEMFDC)
{
	HENHMETAFILE	hEmfTemp =	::CloseEnhMetaFile(hEMFDC);	
	hEMFDC = NULL;
	return hEmfTemp;
}

void SCRectDPtoHIMETRIC(CDC* pDC, RECT* pRect)
{
	ASSERT(pRect);
	pDC->DPtoHIMETRIC((LPSIZE)&pRect->left);
	pDC->DPtoHIMETRIC((LPSIZE)&pRect->right);
}


void SCRectLPtoHIMETRIC(CDC* pDC, RECT* pRect)
{
	ASSERT(pRect);
	pDC->LPtoHIMETRIC((LPSIZE)&pRect->left);
	pDC->LPtoHIMETRIC((LPSIZE)&pRect->right);
}

void SCRectToTwips(RECT* pRect, int ilogx, int ilogy, CDC* pDC)
{
	ASSERT(pRect);
	// convert to inches and inches to twips: 1440 twips = 1 inch
	pRect->left = MulDiv(pRect->left, 1440, ilogx);
	pRect->right = MulDiv(pRect->right, 1440, ilogx);
	pRect->top = MulDiv(pRect->top, 1440, ilogy);
	pRect->bottom = MulDiv(pRect->bottom, 1440, ilogy);
}

// Default paper (A4, in mm)
#define SC_DFLT_PAPER_CX		210
#define SC_DFLT_PAPER_CY		297
#define SC_DFLT_DPI				96
#define SC_DFLT_PHYSICALWIDTH	MulDiv(SC_DFLT_PAPER_CX*10, SC_DFLT_DPI, 254)
#define SC_DFLT_PHYSICALHEIGHT	MulDiv(SC_DFLT_PAPER_CY*10, SC_DFLT_DPI, 254)
	

UINT SCRichEditConvertToEMF(CRichEditCtrl& rRichEdit, HEMFVECTOR& rVector)
{
	// Setup paper
	HDC hdc;
	int nHorizRes;
	int nVertRes;
	int nMarginX = MulDiv(250, 1440, 254); // twips (2.5cm) 
	int nMarginY = nMarginX;			   // all around

	// Check default printer
	BOOL bPrnDC = FALSE;
#if 1
	if ((hdc = SCGetDefaultPrinterDC(rRichEdit.m_hWnd))!=NULL)
#else
	if (FALSE)
	// test stuff: to force using screen attributes
#endif
	{
		bPrnDC = TRUE;

		nHorizRes = GetDeviceCaps(hdc, PHYSICALWIDTH);
		nVertRes = GetDeviceCaps(hdc, PHYSICALHEIGHT);

		SetMapMode(hdc, MM_TEXT);
	} else
	{
		hdc = GetDC(NULL);

		// Note: may cause problem, since we are not using GetDeviceCaps as we should.
		// See the adjustements below.

		// Compute pixels by hand for an A4 paper at 96 dpi.
		nHorizRes = SC_DFLT_PHYSICALWIDTH;
		nVertRes = SC_DFLT_PHYSICALHEIGHT;
	}

	CDC* pDC = CDC::FromHandle(hdc);
	int nLogPixelsX = pDC->GetDeviceCaps(LOGPIXELSX);
	int nLogPixelsY = pDC->GetDeviceCaps(LOGPIXELSY);
	
	if (!bPrnDC)
	{// Adjustement for actual DPI
		nHorizRes = MulDiv(nHorizRes, nLogPixelsX, SC_DFLT_DPI);
		nVertRes = MulDiv(nVertRes, nLogPixelsY, SC_DFLT_DPI);
	}

	CRect rcMeta(0, 0, nHorizRes, nVertRes);
	SCRectLPtoHIMETRIC(pDC, &rcMeta);  // in 100th of mm
	
	FORMATRANGE fr;
	fr.hdcTarget = hdc;
	
	::SetRect(&fr.rcPage, 0, 0, nHorizRes, nVertRes);
	SCRectToTwips(&fr.rcPage, nLogPixelsX, nLogPixelsY, pDC);

	::CopyRect(&fr.rc, &fr.rcPage);
	::InflateRect(&fr.rc, -nMarginX, -nMarginY);

	long lTextLen = rRichEdit.GetTextLength();
	long lTextOut = 0; 
	
	fr.chrg.cpMax = -1;
	while (lTextOut<lTextLen)
	{
		CMetaFileDC MetaDC;
		BOOL bOK = MetaDC.CreateEnhanced(pDC, NULL, &rcMeta, NULL);
		ASSERT(bOK);
		
		fr.hdc = MetaDC.m_hDC;
		fr.chrg.cpMin = lTextOut;
		lTextOut = rRichEdit.FormatRange(&fr, TRUE);

		HENHMETAFILE hemf = MetaDC.CloseEnhanced();
		if (!bPrnDC && hemf)
		{
			// Device adjustment.
			// We must update the header to reflect the device we want
			// (the one we computed by hand)
			UINT uiSize = GetEnhMetaFileBits(hemf, 0, NULL);
			LPBYTE pData = uiSize ? new BYTE[uiSize] : NULL;
			if (pData)
			{
				GetEnhMetaFileBits(hemf, uiSize, pData);
				
				ENHMETAHEADER* pEmfHeader = (ENHMETAHEADER*)pData;
				pEmfHeader->szlDevice.cx = nHorizRes;
				pEmfHeader->szlDevice.cy = nVertRes;
				pEmfHeader->szlMillimeters.cx = SC_DFLT_PAPER_CX;
				pEmfHeader->szlMillimeters.cy = SC_DFLT_PAPER_CY;

				DeleteEnhMetaFile(hemf);
				hemf = SetEnhMetaFileBits(uiSize, pData);
				delete [] pData;
			} // else stay with bogus emf
		}
		ASSERT(hemf);
		rVector.push_back(hemf);
	}
	
	// tell the control to release cached information
	rRichEdit.FormatRange(NULL, FALSE);

	if (bPrnDC)
		DeleteDC(hdc);
	else
		ReleaseDC(NULL, hdc);

	return rVector.size();
}

///
/// RTF or TXT (non UNICODE) file to Enhanced Metafile format
///
UINT SCConvertRTFtoEMF(LPCTSTR lpszFname, HEMFVECTOR& rVector, BOOL bRTF/*=TRUE*/)
{
	CSCRichEdit RichEdit;
	if (!RichEdit.SCCreateHidden(bRTF))
		return 0;
	if (!RichEdit.SCLoadFromFile(lpszFname, bRTF))
		return 0;

	return SCRichEditConvertToEMF(RichEdit, rVector);
}


///
/// RTF or TXT (UNICODE) piece of memory to Enhanced Metafile format
///
UINT SCConvertRTFtoEMF(HANDLE hMem, HEMFVECTOR& rVector, BOOL bRTF/*=TRUE*/)
{
	CSCRichEdit RichEdit;
	if (!RichEdit.SCCreateHidden(bRTF))
		return 0;
	if (!RichEdit.SCLoadFromHGlobal(hMem, bRTF))
		return 0;

	return SCRichEditConvertToEMF(RichEdit, rVector);
}

///
/// Attempt to paste the clipboard content to Enhanced Metafile
///
UINT SCConvertClipboardTextToEMF(HEMFVECTOR& rVector)
{
	CSCRichEdit RichEdit;
	if (!RichEdit.SCCreateHidden(FALSE))
		return 0;

	if (!RichEdit.CanPaste())
		return 0;

	RichEdit.Paste();
	return SCRichEditConvertToEMF(RichEdit, rVector);
}

#undef SC_DFLT_PAPER_CX
#undef SC_DFLT_PAPER_CY
#undef SC_DFLT_DPI
#undef SC_DFLT_PHYSICALWIDTH
#undef SC_DFLT_PHYSICALHEIGHT

///
/// Adjust an EMF header to circumvent problems raised by the use of screen DCs
/// as reference DC when creating/translating/filtering an EMF
///
BOOL SCUpdateEMFheader(HENHMETAFILE& hemf, ENHMETAHEADER& rEmfHeader)
{
	ASSERT(hemf);
	if (0)
	{
		// Device adjustment.
		// We must update the header to reflect the device we want
		// (the one we computed by hand)
		UINT uiSize = GetEnhMetaFileBits(hemf, 0, NULL);
		LPBYTE pData = uiSize ? new BYTE[uiSize] : NULL;
		if (pData)
		{
			GetEnhMetaFileBits(hemf, uiSize, pData);
			
			ENHMETAHEADER* pEmfHeader = (ENHMETAHEADER*)pData;
			pEmfHeader->szlDevice.cx = rEmfHeader.szlDevice.cx;
			pEmfHeader->szlDevice.cy = rEmfHeader.szlDevice.cy;
			pEmfHeader->szlMillimeters.cx = rEmfHeader.szlMillimeters.cx;
			pEmfHeader->szlMillimeters.cy = rEmfHeader.szlMillimeters.cy;
			CopyRect((LPRECT)&pEmfHeader->rclFrame, (LPRECT)&rEmfHeader.rclFrame);
			//CopyRect((LPRECT)&pEmfHeader->rclBounds, (LPRECT)&rEmfHeader.rclBounds);
			
			DeleteEnhMetaFile(hemf);
			hemf = SetEnhMetaFileBits(uiSize, pData);
			delete [] pData;
			return TRUE;
		} // else stay with bogus emf
	}

	return FALSE;

}

///
/// Call this when finished with a created metafile DC
///
void SCEMFDCDispose(HDC& hEMFDC)
{
	if (!hEMFDC)
		return;

	//Close and delete it
	HENHMETAFILE	hEmfTemp = SCCloseEMF(hEMFDC);	
	::DeleteEnhMetaFile(hEmfTemp);
}

void SCInitEMFDCForEnumeration1(HDC hEMFDC, ENHMETAHEADER &hEmfHeader)
{
	if (!hEMFDC)
		return;

	::SetMapMode(hEMFDC,MM_ANISOTROPIC);
	::SetWindowOrgEx(hEMFDC,0,0,NULL);
	::SetViewportOrgEx(hEMFDC,0,0,NULL);
	::SetWindowExtEx(hEMFDC,(int)hEmfHeader.szlMillimeters.cx * 100,(int)hEmfHeader.szlMillimeters.cy * 100,NULL);
	::SetViewportExtEx(hEMFDC,(int)hEmfHeader.szlDevice.cx, (int)hEmfHeader.szlDevice.cy,NULL);
}

///
/// Call this to viewport and window before playing a metafile
///
void SCInitEMFDCForEnumeration(HDC hEMFDC, ENHMETAHEADER &EmfHeader, HDC hDCRef)
{
	if (!hEMFDC)
		return;

	::SetMapMode(hEMFDC,MM_ANISOTROPIC);

	// Set the window extent to rclFrame in pixels
	int width, height;
	// Get the characteristics of the output device.
	int iDpiX = GetDeviceCaps(hDCRef,  LOGPIXELSX);
	int iDpiY = GetDeviceCaps(hDCRef,  LOGPIXELSY);
	RECTL& rRect = EmfHeader.rclFrame;
	width = MulDiv(rRect.right - rRect.left + 1, iDpiX, L001MMPERINCH);
	height = MulDiv(rRect.bottom - rRect.top + 1, iDpiY, L001MMPERINCH);

	SetWindowExtEx(hEMFDC, width, height, NULL);

    // Set the viewport extent to reflect
    // rclFrame in target device units
	    // Get the physical characteristics of the reference DC
    float PixelsX = (float)GetDeviceCaps( hDCRef, HORZRES );
    float PixelsY = (float)GetDeviceCaps( hDCRef, VERTRES );
    float MMX = (float)GetDeviceCaps( hDCRef, HORZSIZE );
    float MMY = (float)GetDeviceCaps( hDCRef, VERTSIZE );
	DWORD dwInchesX = (DWORD)(float(EmfHeader.rclFrame.right*PixelsX)/(100.0f*MMX));
	DWORD dwInchesY = (DWORD)(float(EmfHeader.rclFrame.bottom*PixelsY)/(100.0f*MMY));

    SetViewportExtEx( hEMFDC, dwInchesX, dwInchesY, NULL);
}


static	BOOL	s_bEMRSMALLTEXTOUT = FALSE;
int CALLBACK SCEnhMetafileFilterProc(HDC hDC, HANDLETABLE FAR *lpHTable, ENHMETARECORD FAR *lpEMFR,  int nObj, LPARAM lpData )
{
	if (lpEMFR->iType == 108)
		s_bEMRSMALLTEXTOUT = TRUE;
	else
    // Do not play Escape
	if (lpEMFR->iType == 106)//EMR_EXTESCAPE)
		return 1;
	
	PlayEnhMetaFileRecord(hDC, lpHTable, lpEMFR, nObj);
	return 1; 
}

HDC MSCreateEnhMetaFileFlt( LPTSTR szFileName, // Metafile filename 

                         float dwInchesX,    // Width in inches
                         float dwInchesY,    // Height in inches
                         float dwDPI )       // DPI (logical units)
{
    RECT   Rect = { 0, 0, 0, 0 };
    TCHAR   szDesc[] = _T("EMFExplorer\0Filtered EMF\0\0");
    HDC      hMetaDC, hScreenDC;
    float   PixelsX, PixelsY, MMX, MMY;

    // dwInchesX x dwInchesY in .01mm units
    SetRect(&Rect, 0, 0, (int)(dwInchesX*2540), (int)(dwInchesY*2540));

    // Get a Reference DC
    hScreenDC = GetDC( NULL );

    // Get the physical characteristics of the reference DC
    PixelsX = (float)GetDeviceCaps( hScreenDC, HORZRES );
    PixelsY = (float)GetDeviceCaps( hScreenDC, VERTRES );
    MMX = (float)GetDeviceCaps( hScreenDC, HORZSIZE );
    MMY = (float)GetDeviceCaps( hScreenDC, VERTSIZE );

    // Create the Metafile
    hMetaDC = CreateEnhMetaFile(hScreenDC, szFileName, &Rect, szDesc);
    // Release the reference DC
    ReleaseDC( NULL, hScreenDC );
    // Did you get a good metafile?
    if( hMetaDC == NULL )
        return NULL;

    // Anisotropic mapping mode
    SetMapMode( hMetaDC, MM_ANISOTROPIC );
    // Set the Windows extent
    SetWindowExtEx( hMetaDC, (int)(dwInchesX*dwDPI), (int)(dwInchesY*dwDPI), NULL );

    // Set the viewport extent to reflect
    // dwInchesX" x dwInchesY" in device units
    SetViewportExtEx( hMetaDC,
                      (int)((float)dwInchesX*25.4f*PixelsX/MMX),
                      (int)((float)dwInchesY*25.4f*PixelsY/MMY),
                      NULL );
    return hMetaDC;

}

///
/// Filter an EMF to remove undocumented records
///
BOOL SCPreFilterEMF(HENHMETAFILE& hEmf)
{
	ASSERT(hEmf);
	ENHMETAHEADER EmfHeader;
	if (!::GetEnhMetaFileHeader(hEmf, sizeof(ENHMETAHEADER), &EmfHeader))
	{
		DWORD dwErrCode = ::GetLastError();
		return FALSE;
	}
	HDC hDCRef = GetDC(NULL);

	CSize sizeEMF;
	SCGetEMFPlaySize(hEmf, sizeEMF);

	long lEmfDPIX = MulDiv(EmfHeader.szlDevice.cx, 254, EmfHeader.szlMillimeters.cx*10L);
	long lEmfDPIY = MulDiv(EmfHeader.szlDevice.cy, 254, EmfHeader.szlMillimeters.cy*10L);
	int iDpiX = GetDeviceCaps(hDCRef,  LOGPIXELSX);
	int iDpiY = GetDeviceCaps(hDCRef,  LOGPIXELSY);

	CRect rcPlay((LPRECT)&EmfHeader.rclFrame);
	HDC hNewEMFDC = CreateEnhMetaFile(hDCRef, (LPCTSTR)NULL, (LPRECT)&EmfHeader.rclFrame, NULL);
	SCInitEMFDCForEnumeration1(hNewEMFDC, EmfHeader);

	if (EnumEnhMetaFile(hNewEMFDC, hEmf,  (ENHMFENUMPROC)SCEnhMetafileFilterProc, (LPVOID)NULL, (LPRECT)&rcPlay))
	{
		if (s_bEMRSMALLTEXTOUT)
		{
			DeleteEnhMetaFile(hEmf);
			hEmf = SCCloseEMF(hNewEMFDC);
		}
	}

	SCEMFDCDispose(hNewEMFDC);
	ReleaseDC(NULL, hDCRef);
	return TRUE;
}

#if 0
// Zzzz!... Let them sleep.
#define MS_LOGX GetDeviceCaps(hDC, LOGPIXELSX)
#define MS_LOGY GetDeviceCaps(hDC, LOGPIXELSY)
#define MS_HIMETRICINCH 2540

void FAR PASCAL MSHiMetrictoDP(HDC hDC, POINT &pt)

{ 
   pt.x = MulDiv(pt.x, MS_LOGX, MS_HIMETRICINCH);

   // The minus sign is required because the Y axis
   // points down in the MM_TEXT mapping mode
   pt.y = -MulDiv(pt.y, MS_LOGY, MS_HIMETRICINCH);

} 

void FAR PASCAL MSDPtoHiMetric(HDC hDC, POINT &pt)

{ 
   pt.x = MulDiv(pt.x, MS_HIMETRICINCH, MS_LOGX);

   // The minus sign is required because the Y axis
   // points down in the MM_TEXT mapping mode
   pt.y = -MulDiv(pt.y, MS_HIMETRICINCH, MS_LOGY);

} 
#endif
