/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFdcRenderer.h"

using namespace Gdiplus;

///////////////////////////////////////////////////////////////////////////////////////
// Images/Pixels management
//

///
///	Draw a DIB bitmap.
///
void CSCEMFdcRenderer::SCDrawImage(LPCRECT pDest, LPCRECT pSrc, LPCBYTE pBits,
								 LPCBITMAPINFO pBmi, DWORD dwUsage, DWORD dwRop3,
								 XFORM* pXForm/*=NULL*/)
{
	ASSERT(m_pGraphics);
	ASSERT(pBmi && pBits && pDest && pSrc);

	// dwUsage can be: DIB_PAL_COLORS, DIB_PAL_INDICES, or DIB_RGB_COLORS
	ASSERT(dwUsage==DIB_RGB_COLORS || dwUsage==DIB_PAL_COLORS); // the other ones are not managed for now

	Bitmap* pBmp = NULL;
	HBITMAP hNewBmp = NULL;
	if (dwUsage==DIB_PAL_COLORS)
	{// Convert bitmap. Otherwise, GDI+ would mess with the color table.
		// Use a device-dependent bitmap + current palette combination
		HPALETTE hPal = (HPALETTE)::GetCurrentObject(m_hDC, OBJ_PAL);
		ASSERT(hPal);

#pragma message( __FILE__  "(43): TODO: create bitmap with destination DC instead of m_hDC ")
		hNewBmp = CreateDIBitmap(m_hDC, (BITMAPINFOHEADER*)pBmi, CBM_INIT, pBits, pBmi, dwUsage);
		pBmp = new Bitmap(hNewBmp, hPal);
	} else
	if (4==pBmi->bmiHeader.biBitCount)
	{// I don't see why Bitmap(pBmi, (VOID*)pBits) fails. Yet another color table problem?
		// tracking occurrences (4-bpp failure on w2k-xls/Tdb_fev00/page0013)
		hNewBmp = CreateDIBitmap(m_hDC, (BITMAPINFOHEADER*)pBmi, CBM_INIT, pBits, pBmi, dwUsage);
		pBmp = new Bitmap(hNewBmp, (HPALETTE)NULL);
	} else
	{
		// tracking occurrences (8-bpp passed with w98, w95-bmp/swtchbrd,
		// 1-bpp passed with "w98, w95-gif/santa workshop")
		//ASSERT(pBmi->bmiHeader.biBitCount>8);
		pBmp = new Bitmap(pBmi, (VOID*)pBits);
	}
	ASSERT(pBmp->GetLastStatus()==Ok);

	// Note: very special stuff here.
	// Convert sizes when a source transformation is present
	Matrix SvdMatrix;
	RECT rcSrc;
	if (pXForm)
	{
		// save current transform
		m_pGraphics->GetTransform(&SvdMatrix);

		// Convert the source with the source transform only
		// (because pXForm is the "world-space to page-space transformation of
		// the source device context" -- not the destination device -- when the record was registered)

		CopyRect(&rcSrc, pSrc);
		pSrc = &rcSrc;

		Matrix matrix(pXForm->eM11, pXForm->eM12, pXForm->eM21, pXForm->eM22,
			pXForm->eDx, pXForm->eDy);
		matrix.TransformPoints((Point*)pSrc, 2);
	}
	//

	Rect destRect(pDest->left, pDest->top, PRECT_WIDTH(pDest), PRECT_HEIGHT(pDest));

	// GDI+ 1.0: the ROP is simply let aside => unexpected results in perspective
	// Good place to say: GDI+ does not work!
	if (SRCCOPY==dwRop3)
	{
		// Copies the source rectangle directly to the destination rectangle. 
		m_pGraphics->DrawImage(pBmp, destRect,
		pSrc->left, pSrc->top, PRECT_WIDTH(pSrc), PRECT_HEIGHT(pSrc), UnitPixel, m_pImgAttribs);
	}
	else
	{// At the expense of some CPU time, GDI The Great will be called to do the job.

		// Check popular rops
		switch (dwRop3)
		{
		case BLACKNESS:
			// Fills the destination rectangle using the color associated with index 0
			// in the physical palette.
			// (This color is black for the default physical palette.)
			SCSysPaletteFillRect(this, destRect, 0);
			goto enddraw;

		case DSTINVERT:
			// Inverts the destination rectangle.
			break;

		case MERGECOPY:
			// Merges the colors of the source rectangle with the specified pattern
			// by using the Boolean AND operator.
			break;

		case MERGEPAINT:
			// Merges the colors of the inverted source rectangle with the colors
			// of the destination rectangle by using the Boolean OR operator. 
			break;

		case NOTSRCCOPY:
			// Copies the inverted source rectangle to the destination.
			break;

		case NOTSRCERASE:
			// Combines the colors of the source and destination rectangles by using
			// the Boolean OR operator and then inverts the resultant color.
			break;

		case PATCOPY:
			// Copies the specified pattern into the destination bitmap.
			ASSERT(m_pBrush);
			if (m_pBrush)
				m_pGraphics->FillRectangle(m_pBrush, destRect);
			goto enddraw;

		case PATINVERT:
			// Combines the colors of the specified pattern with the colors of the
			// destination rectangle by using the Boolean XOR operator.
			break;

		case PATPAINT:
			// Combines the colors of the pattern with the colors of the inverted
			// source rectangle by using the Boolean OR operator. The result of this
			// operation is combined with the colors of the destination rectangle by
			// using the Boolean OR operator. 
			break;

		case SRCAND:
			// Combines the colors of the source and destination rectangles by using
			// the Boolean AND operator. 
			break;

		case SRCERASE:
			// Combines the inverted colors of the destination rectangle with the
			// colors of the source rectangle by using the Boolean AND operator. 
			break;

		case SRCINVERT:
			// Combines the colors of the source and destination rectangles by using
			// the Boolean XOR operator.
			break;

		case SRCPAINT:
			// Combines the colors of the source and destination rectangles by using
			// the Boolean OR operator. 
			break;

		case WHITENESS:
			//Fills the destination rectangle using the color associated with index 1
			// in the physical palette.
			// (This color is white for the default physical palette.)
			SCSysPaletteFillRect(this, destRect, 1);
			goto enddraw;

		//case SRCCOPY: // done
		//	// Copies the source rectangle directly to the destination rectangle. 
		//	break;
		}

		// Wild, general rop management:
		// 1. if the rop implies the destination we must copy it
		// 2. if the rop implies the brush, we must use it
		SCStretchDIBits(this,
			destRect.X, destRect.Y, destRect.Width, destRect.Height,
			pSrc->left, pSrc->top, PRECT_WIDTH(pSrc), PRECT_HEIGHT(pSrc),
			pBits, pBmi, dwUsage, dwRop3);
	}
enddraw:
	if (pXForm)
		m_pGraphics->SetTransform(&SvdMatrix);
	if (hNewBmp)
		DeleteObject(hNewBmp);
	delete pBmp;
}

///
///	Draw GDI+ bitmap.
///
void CSCEMFdcRenderer::SCDrawBitmap(Bitmap* pBmp, Rect& rRcDest)
{
	ASSERT(m_pGraphics);
	ASSERT(pBmp);

	m_pGraphics->DrawImage(pBmp, rRcDest,
		0, 0, pBmp->GetWidth(), pBmp->GetHeight(), UnitPixel, m_pImgAttribs);
}

///
///	Draw a masked DIB bitmap into a parallelogram.
///
void CSCEMFdcRenderer::SCDrawImagePlg(LPCPOINTL p3PtDest,
					LPCRECT pSrc, LPCBYTE pBits, LPCBITMAPINFO pBmi,
					DWORD dwUsage, XFORM* pXForm, COLORREF crBkColorSrc,
					LONG lxMask, LONG lyMask,
					LPCBYTE pBitsMask, LPCBITMAPINFO pBmiMask, DWORD dwUsageMask)
{
	ASSERT(m_pGraphics);
	ASSERT(pBmi && pBits && p3PtDest && pSrc);

	// dwUsage can be: DIB_PAL_COLORS, DIB_PAL_INDICES, or DIB_RGB_COLORS
	ASSERT(dwUsage==DIB_RGB_COLORS); // the other ones are not managed for now

	// The p3PtDest array contains three points in logical space that identify three
	// corners of the destination parallelogram.
	// In GDI the mapping is as follows: the upper-left corner of the source rectangle is mapped to
	// the first point in this array, the upper-right corner to the second point in this array,
	// and the lower-left corner to the third point. The lower-right corner of the source
	// rectangle is mapped to the implicit fourth point in the parallelogram.
	// In GDI+: destPoints[0] <=> top-left, destPoints[1] <=> top-right, destPoints[2] <=> bottom-left
	Point destPoints[3];
	for (INT i=0; (i<3); i++)
	{
		destPoints[i].X = p3PtDest[i].x;
		destPoints[i].Y = p3PtDest[i].y;
	}

	if (!pBitsMask || !pBmiMask)
	{// No mask: just blt.
		Bitmap bmp(pBmi, (VOID*)pBits);
		m_pGraphics->DrawImage(&bmp, destPoints, 3,
			pSrc->left, pSrc->top, PRECT_WIDTH(pSrc), PRECT_HEIGHT(pSrc), UnitPixel, m_pImgAttribs);
	} else
	{// see the SCDrawImageMsk function for documentation.

		INT iHeight = abs(pBmi->bmiHeader.biHeight);
		INT iWidth = pBmi->bmiHeader.biWidth;
		BOOL bFlip = (pBmi->bmiHeader.biHeight>0);
		// lay out the mask on an array of bytes containg 0 or 1
		BYTE* pExpandedMask = SCExpandMask(iWidth, iHeight,
			lxMask, lyMask, pBitsMask, pBmiMask, dwUsageMask, !bFlip);
		if (!pExpandedMask)
			return;

		// create alpha bitmap
		HBITMAP hbm = SCBuildAlphaBitmap(pBits, pBmi, dwUsage, pExpandedMask, bFlip);
		if (hbm)
		{
			DIBSECTION ds;
			GetObject(hbm, sizeof(DIBSECTION), &ds);
			Bitmap bmp(iWidth, iHeight, 4*iWidth,
				PixelFormat32bppARGB, (BYTE*)ds.dsBm.bmBits); 

			// Draw the alpha bitmap
			m_pGraphics->DrawImage(&bmp, destPoints, 3,
				pSrc->left, pSrc->top, PRECT_WIDTH(pSrc), PRECT_HEIGHT(pSrc), UnitPixel, m_pImgAttribs);

			DeleteObject(hbm);
		}

		delete [] pExpandedMask;
	}
}

///
///	Mask Blt: uses a ROP4 to mask the source bitmap.
///
void CSCEMFdcRenderer::SCDrawImageMsk(LPCRECT pDest,
					LPCRECT pSrc, LPCBYTE pBits, LPCBITMAPINFO pBmi,
					DWORD dwUsage, DWORD dwRop4, XFORM* pXForm, COLORREF crBkColorSrc,
					LONG lxMask, LONG lyMask,
					LPCBYTE pBitsMask, LPCBITMAPINFO pBmiMask, DWORD dwUsageMask)
{
	ASSERT(m_pGraphics);
	ASSERT(pBmi && pBits && pDest && pSrc);

	// dwUsage can be: DIB_PAL_COLORS, DIB_PAL_INDICES, or DIB_RGB_COLORS
	ASSERT(dwUsage==DIB_RGB_COLORS); // the other ones are not managed for now
	Bitmap bmp(pBmi, (VOID*)pBits);

	if (!pBitsMask || !pBmiMask)
	{// No mask: just blt, using the foreground ROP index as ROP3.
		SCDrawImage(pDest, pSrc, pBits, pBmi, dwUsage,
			SCRop4GetForegroundROP3(dwRop4), pXForm);
	} else
	{
		INT iHeight = abs(pBmi->bmiHeader.biHeight);
		INT iWidth = pBmi->bmiHeader.biWidth;
		BOOL bFlip = (pBmi->bmiHeader.biHeight>0);
		// Lay out the mask on an array of bytes containg 0 or 1
		// Note: this layout must follow the same orientation as the bitmap (ie not flipped).
		BYTE* pExpandedMask = SCExpandMask(iWidth, iHeight,
			lxMask, lyMask, pBitsMask, pBmiMask, dwUsageMask, !bFlip);
		if (!pExpandedMask)
			return;

		// TODO: use the foreground ROP where the mask is 1,
		// and the background ROP where the mask is 0
		// For now, just do like plgblt

		// Create alpha bitmap
		// We need a top-down image for GDI+. So bottom-up images must ne flipped.
		HBITMAP hbm = SCBuildAlphaBitmap(pBits, pBmi, dwUsage, pExpandedMask, bFlip);
		if (hbm)
		{
			Rect destRect(pDest->left, pDest->top, PRECT_WIDTH(pDest), PRECT_HEIGHT(pDest));
#if 0
			// won't work. GDI+ would discard transparency information.
			#if 0
			// Nither this:
			Bitmap bmp(hbm, (HPALETTE)NULL);
			#else
			// Nor this:
			DIBSECTION ds;
			GetObject(hbm, sizeof(DIBSECTION), &ds);
			Bitmap bmp((BITMAPINFO*)&ds.dsBmih, ds.dsBm.bmBits);
			#endif
#else
			// We must access the GDI+ bits and update them in 32bpp format
			DIBSECTION ds;
			GetObject(hbm, sizeof(DIBSECTION), &ds);
			Bitmap bmp(iWidth, iHeight, 4*iWidth,
				PixelFormat32bppARGB, (BYTE*)ds.dsBm.bmBits); 
#endif
			// Draw the alpha bitmap
			m_pGraphics->DrawImage(&bmp, destRect,
				pSrc->left, pSrc->top, PRECT_WIDTH(pSrc), PRECT_HEIGHT(pSrc), UnitPixel, m_pImgAttribs);

			DeleteObject(hbm);
		}

		delete [] pExpandedMask;
	}
}

///
///	Transparent blt (for 4bpp and 8bpp).
///		Note: dwRop3 Stores the transparent color.
///
void CSCEMFdcRenderer::SCDrawImageTransparent(LPCRECT pDest, LPCRECT pSrc, LPCBYTE pBits,
								 LPCBITMAPINFO pBmi, DWORD dwUsage, DWORD dwRop3,
								 XFORM* pXForm, COLORREF crBkColorSrc)
{
	ASSERT(m_pGraphics);
	ASSERT(pBmi && pBits && pDest && pSrc);
	//32bpp should use AlphaBlend
	//ASSERT(4==pBmi->bmiHeader.biBitCount || 8==pBmi->bmiHeader.biBitCount);

	// dwUsage can be DIB_RGB_COLORS
	ASSERT(dwUsage==DIB_RGB_COLORS);

	// Make up the opaque bitmap
	Bitmap bmp(pBmi, (VOID*)pBits); 
	ASSERT(bmp.GetLastStatus()==Ok);

	// Create transparency attributes
	COLORREF crTransparentColor = (COLORREF)dwRop3;
	ColorMap clrMap;
	clrMap.oldColor = Color(255, GetRValue(crTransparentColor), GetGValue(crTransparentColor), GetBValue(crTransparentColor));
	clrMap.newColor = Color(0, GetRValue(crTransparentColor), GetGValue(crTransparentColor), GetBValue(crTransparentColor));

	ImageAttributes imgAttribs;
	imgAttribs.SetRemapTable(1, &clrMap, ColorAdjustTypeBitmap);

	// Note: very special stuff here.
	// Convert sizes when a source transformation is present
	Matrix SvdMatrix;
	RECT rcSrc;
	if (pXForm)
	{
		// save current transform
		m_pGraphics->GetTransform(&SvdMatrix);

		// Convert the source with the source transform only
		// (because pXForm is the "world-space to page-space transformation of
		// the source device context" -- not the destination device -- when the record was registered)
		CopyRect(&rcSrc, pSrc);
		pSrc = &rcSrc;

		Matrix matrix(pXForm->eM11, pXForm->eM12, pXForm->eM21, pXForm->eM22,
			pXForm->eDx, pXForm->eDy);
		matrix.TransformPoints((Point*)pSrc, 2);
	}

	// Destination
	Rect destRect(pDest->left, pDest->top, PRECT_WIDTH(pDest), PRECT_HEIGHT(pDest));

	// Draw the alpha bitmap
	m_pGraphics->DrawImage(&bmp, destRect,
		pSrc->left, pSrc->top, PRECT_WIDTH(pSrc), PRECT_HEIGHT(pSrc), UnitPixel, &imgAttribs);
}


///
///	Alpha blend
///		Note: dwRop3 Stores the blend function (a global alpha value to be applied to
///		the entire source bitmap, and format information for the source bitmap).
///
void CSCEMFdcRenderer::SCDrawImageAlpha(LPCRECT pDest, LPCRECT pSrc, LPCBYTE pBits,
								 LPCBITMAPINFO pBmi, DWORD dwUsage, DWORD dwRop3,
								 XFORM* pXForm, COLORREF crBkColorSrc)
{
	ASSERT(m_pGraphics);
	ASSERT(pBmi && pBits && pDest && pSrc);

	// dwUsage can be DIB_RGB_COLORS
	ASSERT(dwUsage==DIB_RGB_COLORS);

	// Make up the opaque bitmap
	Bitmap bmp(pBmi, (VOID*)pBits); 
	ASSERT(bmp.GetLastStatus()==Ok);

	// Create transparency attributes
	BLENDFUNCTION& blend = (BLENDFUNCTION&)dwRop3;
	ASSERT(AC_SRC_OVER==blend.BlendOp);
	ASSERT(0==blend.BlendFlags);

	FLOAT fSCA = float(blend.SourceConstantAlpha)/255.0f;
	ColorMatrix colorMatrix =
	{1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

#if 1
	// Very confusing GDI documentation (either in preliminary or in final version)
	// We assume "has per-pixel alpha values" means "32bpp with alpha in the high-order byte",
	// which doesn't not exclude having alpha in a color table in other cases.
	if (AC_SRC_ALPHA==blend.AlphaFormat)
	{// image has per-pixel alpha values (in the image data?)
		// "When the AlphaFormat parameter is AC_SRC_ALPHA, the source bitmap must be 32 bpp"
		ASSERT(32==pBmi->bmiHeader.biBitCount);

		if (255!=blend.SourceConstantAlpha)
		{
			// "If the source has both the SourceConstantAlpha (that is, it is not 0xFF)
			// and per-pixel alpha, the source is pre-multiplied by the SourceConstantAlpha
			// and then the blend is based on the per-pixel alpha, as shown in the following table"
			// Src.rgb = Src.rgb * SourceConstantAlpha / 255.0
			// Src.Alpha = Src.Alpha * SourceConstantAlpha / 255.0
			// Dst.rgb = Src.rgb  + (1 - Src.Alpha) * Dst.rgb
			// Dst.Alpha = Src.Alpha + (1 - Src.Alpha) * Dst.Alpha
			colorMatrix.m[0][0] = 
			colorMatrix.m[0][1] = 
			colorMatrix.m[0][2] = 
			colorMatrix.m[1][0] = 
			colorMatrix.m[1][1] = 
			colorMatrix.m[1][2] = 
			colorMatrix.m[2][0] = 
			colorMatrix.m[2][1] = 
			colorMatrix.m[2][2] = fSCA;
			colorMatrix.m[3][3] = fSCA;

			// TODO: we don't have acces to the dest: call GDI.
		}
		// else the alpha channel, alone, controls transparency
			// Dst.rgb = Src.rgb + (1 - Src.Alpha) * Dst.rgb
			// Dst.alpha = Src.Alpha + (1 - Src.Alpha) * Dst.Alpha
	} else
	{// source bitmap has no per-pixel alpha value (in the image data?)
		// "If the source bitmap has no per-pixel alpha value (that is, AC_SRC_ALPHA is
		// not set), the SourceConstantAlpha value determines the blend of the source and
		// destination bitmaps,  as shown in the following table"
		if (255!=blend.SourceConstantAlpha)
		{// the blend is based on a constant source alpha value
			// let SCA = SourceConstantAlpha
			// Dst.rgb = Src.rgb * (SCA/255.0) + Dst.rgb * (1.0 - (SCA/255.0))
			// Dst.Alpha = Src.Alpha * (SCA/255.0) + Dst.Alpha * (1.0 - (SCA/255.0))
			colorMatrix.m[0][0] = 
			colorMatrix.m[0][1] = 
			colorMatrix.m[0][2] = 
			colorMatrix.m[1][0] = 
			colorMatrix.m[1][1] = 
			colorMatrix.m[1][2] = 
			colorMatrix.m[2][0] = 
			colorMatrix.m[2][1] = 
			colorMatrix.m[2][2] = fSCA;
			colorMatrix.m[3][3] = fSCA;

			// TODO: we don't have acces to the dest: call GDI.
		} else
		{// source bitmap has per-pixel alpha (in a color table?)
			// "If the source bitmap does not use SourceConstantAlpha
			// (that is, it equals 0xFF), the per-pixel alpha determines the blend of
			// the source and destination bitmaps"
			// Dst.rgb = Src.rgb + (1 - Src.Alpha) * Dst.rgb
			// Dst.alpha = Src.Alpha + (1 - Src.Alpha) * Dst.Alpha
		}
	}
#endif
	
	ImageAttributes imgAttribs;
	imgAttribs.SetColorMatrix(&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);

	// Note: very special stuff here.
	// Convert sizes when a source transformation is present
	Matrix SvdMatrix;
	RECT rcSrc;
	if (pXForm)
	{
		// save current transform
		m_pGraphics->GetTransform(&SvdMatrix);

		// Convert the source with the source transform only
		// (because pXForm is the "world-space to page-space transformation of
		// the source device context" -- not the destination device -- when the record was registered)
		CopyRect(&rcSrc, pSrc);
		pSrc = &rcSrc;

		Matrix matrix(pXForm->eM11, pXForm->eM12, pXForm->eM21, pXForm->eM22,
			pXForm->eDx, pXForm->eDy);
		matrix.TransformPoints((Point*)pSrc, 2);
	}

	// Destination
	Rect destRect(pDest->left, pDest->top, PRECT_WIDTH(pDest), PRECT_HEIGHT(pDest));

	// Draw the alpha bitmap
	m_pGraphics->DrawImage(&bmp, destRect,
		pSrc->left, pSrc->top, PRECT_WIDTH(pSrc), PRECT_HEIGHT(pSrc), UnitPixel, &imgAttribs);
}

///
///	Pattern Blt: uses the ROP or the current brush to fill the given rectangle.
///
void CSCEMFdcRenderer::SCPatBlt(LPCRECT pDest, DWORD dwRop3, XFORM* pXForm/*=NULL*/)
{
	ASSERT(m_pGraphics);
	ASSERT(pDest);

	Rect destRect(pDest->left, pDest->top, PRECT_WIDTH(pDest), PRECT_HEIGHT(pDest));

	switch (dwRop3)
	{
	case PATCOPY:
		// "Copies the specified pattern into the destination bitmap."
		//ASSERT(m_pBrush);
		if (m_pBrush)
		{
			// include border (see T_SCDrawPolygon for documentation)?
			//destRect.Inflate(1, 1); // No. PatBlt uses no pen.
			m_pGraphics->FillRectangle(m_pBrush, destRect);
		} else
		{// Want to patblt with NULL brush?
			TRACE0("Patblt with NULL brush");
		}
		return;
		
//		case PATINVERT:
//			// "Combines the colors of the specified pattern with the colors of the
//			// destination rectangle by using the Boolean XOR operator."
//			// => needs merging
//			break;
//			
//		case DSTINVERT:
//			// "Inverts the destination rectangle."
//			// => needs merging
//			break;
		
	case BLACKNESS:
		// "Fills the destination rectangle using the color associated with index 0
		// in the physical palette."
		// (This color is black for the default physical palette.)
		SCSysPaletteFillRect(this, destRect, 0);
		return;

	case WHITENESS:
		// "Fills the destination rectangle using the color associated with index 1
		// in the physical palette. This color is white for the default physical palette."
		// Bogus GDI documentation?: index 1 doesn't correspond to WHITENESS as expected
		// (And we know that the 20 static colors (reserved) use the following indices:
		// 0-9, 246-255; but that's true for Windows 3.0 and 3.1; for other systems, !!??)
		#if 0
			SCSysPaletteFillRect(this, destRect, 1);   // won't work
		#else
			SCSysPaletteFillRect(this, destRect, 255); // awaiting bug?
		#endif
		return;

//		default:
//			ASSERT(0); // not supported
	}

#if 0
	// Mixing GDI and GDI+ may require reducing interpolation quality
	InterpolationMode uiOldMode = m_pGraphics->GetInterpolationMode();
	if (uiOldMode!=InterpolationModeNearestNeighbor)
		m_pGraphics->SetInterpolationMode(InterpolationModeNearestNeighbor);

	SCMergeAreaWithPattern(this, destRect, dwRop3);

	if (uiOldMode!=InterpolationModeNearestNeighbor)
		m_pGraphics->SetInterpolationMode(uiOldMode);
#else
	SCMergeAreaWithPattern(this, destRect, dwRop3);
#endif
}

///
///	Set the pixel at the specified coordinates to the closest approximation of the specified color.
///
void CSCEMFdcRenderer::SCDrawPixel(POINTL ptlPixel, COLORREF crColor)
{
	ASSERT(m_pGraphics);

	Rect rcDest(ptlPixel.x, ptlPixel.y, 1, 1);
	Color PixelColor;
	PixelColor.SetFromCOLORREF(crColor);
	SolidBrush PixelBrush(PixelColor);

	m_pGraphics->FillRectangle(&PixelBrush, rcDest);
}
