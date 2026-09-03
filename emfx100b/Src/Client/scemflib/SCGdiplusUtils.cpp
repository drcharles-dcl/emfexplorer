/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCGdiplusUtils.h"
#include "SCEMF.h"
#include "SCEMFRasterizer.h"
#include "SCFileTypes.h"

#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCMemDC.h)
#include SC_INC_WINLIB(SCBitmap.h)
#include SC_INC_WINLIB(MSDib.h)
#include SC_INC_WINLIB(SCGDIUtils.h)
#include SC_INC_GENLIB(SCGenMath.h)
#include <float.h>


// GDI+ won't allow DEBUG_NEW to work
//	#ifdef _DEBUG
//	#define new DEBUG_NEW
//	#undef THIS_FILE
//	static char THIS_FILE[] = __FILE__;
//	#endif

///
/// Create a GDI+ bitmap from a packed DIB (a pointer to a
/// BITMAPINFO structure immediately followed by the array of bytes that define
/// the pixels of the bitmap).
///
Bitmap* SCBitmapFromPtr(LPCBYTE pBytes)
{
	ASSERT(pBytes);
	if (!pBytes)
		return NULL;
	// get the BITMAPINFO
	BITMAPINFO* pBmi = (BITMAPINFO*)pBytes;

	// Locate the bits after the color table:
	// Since the DIB is packed, the biClrUsed member must be either zero or
	// the actual size of the color table
	BYTE* pBits = (BYTE*)pBmi + pBmi->bmiHeader.biSize + pBmi->bmiHeader.biClrUsed;

	// create GDI+ object
	Bitmap* pBitmap = new Bitmap(pBmi, pBits);
	if (!pBitmap || (pBitmap->GetLastStatus()!=Ok))
	{
		ASSERT(0);
		delete pBitmap;
		return NULL;
	}
	return pBitmap;
}

///
/// Create a GDI+ bitmap from a packed DIB (a global memory chunk pointing to a
/// BITMAPINFO structure immediately followed by the array of bytes that define
/// the pixels of the bitmap).
/// Note: on 32-bit systems, the HGLOBAL is not quite different from a pointer
///
Bitmap* SCBitmapFromHGLOBAL(HGLOBAL hMem, SCShortDCState& rDCState)
{
	ASSERT(hMem);
	if (!hMem)
		return NULL;

	// get the BITMAPINFO
	BITMAPINFO* pBmi = (BITMAPINFO*)hMem; //GlobalLock(hMem);

	// Locate the bits after the color table:
	// Since the DIB is packed, the biClrUsed member must be either zero or
	// the actual size of the color table
	INT iNbColors = DIBNumColors(LPBITMAPINFOHEADER(pBmi));
	BYTE* pBits = (BYTE*)pBmi + pBmi->bmiHeader.biSize + iNbColors*sizeof(RGBQUAD);

	// create GDI+ object
	Bitmap* pBitmap = NULL;
	if (32==pBmi->bmiHeader.biBitCount)
		pBitmap = new Bitmap(pBmi->bmiHeader.biWidth, pBmi->bmiHeader.biHeight, 4*pBmi->bmiHeader.biWidth,
				PixelFormat32bppARGB, pBits);
	else
	{
		pBitmap = new Bitmap(pBmi, pBits);

		INT iRop2 = rDCState.dwRop2;
		// Update color palette for 1-bpp.
		if (1==pBmi->bmiHeader.biBitCount)
		{
			RGBQUAD* pColors;
			RGBQUAD MonoColors[2];
			if (rDCState.bMonoDC)
			{// Slight difference for DIB? Is it ONLY when the DC IS MONOCHROME that
			 // background and text colors come into play?

			// "When an application selects a two-color DIB pattern brush into a monochrome
			// device context, Windows ignores the colors specified in the DIB and instead
			// displays the pattern brush using the current text and background colors of
			// the device context. Pixels mapped to the first color 
			// (at offset 0 in the DIB color table) of the DIB are displayed using the
			// text color. Pixels mapped to the second color (at offset 1 in the color table)
			// are displayed using the background color."
				pColors = (RGBQUAD*)&MonoColors;
				SCFillMonochromePalette(rDCState, (PPALETTEENTRY)pColors);
			} else
			{// ignore background and text colors, and use the DIB color table
				pColors = (RGBQUAD*)&pBmi->bmiColors;
			}
			#define SC_ISWHITE_RGBQUAD(q) (255==q.rgbRed && 255==q.rgbGreen && 255==q.rgbBlue)

			ColorPalette* pPal = (ColorPalette*)new BYTE[sizeof(ColorPalette) + iNbColors*sizeof(ARGB)];
			pPal->Flags = PaletteFlagsHasAlpha;
			pPal->Count = iNbColors;

			INT iAlpha = 255;
			INT i;
			switch (iRop2)
			{
			case R2_MASKPEN:
				// The normal code is "black=>opaque, and white=>transparent". But we
				// make black pixels half-black and make white pixels transparent.
				// take it as some hand-tuned "gamma corection".
				for (i=0; (i<iNbColors); i++)
				{
					if (SC_ISWHITE_RGBQUAD(pColors[i]))
						iAlpha = 0;	// fully transparent;
					else
						iAlpha = 255; // fully opaque (128 works better; why?)
					pPal->Entries[i] = Color::MakeARGB(iAlpha, pColors[i].rgbRed,
															pColors[i].rgbGreen,
															pColors[i].rgbBlue);
				}
				break;

			default:
				// normal palette
				for (i=0; (i<iNbColors); i++)
				{
					pPal->Entries[i] = Color::MakeARGB(255, pColors[i].rgbRed,
															pColors[i].rgbGreen,
															pColors[i].rgbBlue);
				}
			}
			pBitmap->SetPalette(pPal);
			delete [] pPal;
		}
	}
	//GlobalUnlock(hMem);

	if (pBitmap && (pBitmap->GetLastStatus()!=Ok))
	{
		ASSERT(0);
		delete pBitmap;
		return NULL;
	}
	return pBitmap;
}

///
/// Create a GDI+ bitmap from a handle to a Windows bitmap.
///
Bitmap* SCBitmapFromMemBmp(HBITMAP hBM)
{
	ASSERT(hBM);
	// GDI+ 1.0: we don't have a simple constructor Bitmap::Bitmap(HBITMAP)
	// But a palette residing outside the bitmap was carefully cast in. Jeez!
	Bitmap* pBitmap = new Bitmap(hBM, (HPALETTE)NULL);
	if (pBitmap && (pBitmap->GetLastStatus()!=Ok))
	{
		ASSERT(0);
		delete pBitmap;
		return NULL;
	}
	return pBitmap;
}

///
/// Create a two-color palette to use with the monochrome brush currently selected into a DC.
///	"A brush created by using a monochrome (1 bit per pixel) bitmap has
///	the text and background colors of the device context to which it is drawn.
///	Pixels represented by a 0 bit are drawn with the current text color;
///	pixels represented by a 1 bit are drawn with the current background color."
///
HPALETTE SCCreatePatternPalette(SCShortDCState& rDCState)
{
	// Create the logical palette based on the entries
	// Allocate memory for the palette.
	LPLOGPALETTE lpLogPal = (LPLOGPALETTE)new BYTE [sizeof(LOGPALETTE) + 2*sizeof(PALETTEENTRY)];
	if (!lpLogPal)
		return NULL;
	
	// Initialize.
	lpLogPal->palVersion = 0x300;
	lpLogPal->palNumEntries = 2;
	
	// Update colors in palette.
	SCFillMonochromePalette(rDCState, lpLogPal->palPalEntry);
	
	// Create the palette.
	HPALETTE hPal = CreatePalette(lpLogPal);
	
	// Clean up.
	delete [] (BYTE*)lpLogPal;
	
	return hPal;
}

///
/// Create a two-color bitmap representing a monochrome brush.
///	"A brush created by using a monochrome (1 bit per pixel) bitmap has
///	the text and background colors of the device context to which it is drawn.
///	Pixels represented by a 0 bit are drawn with the current text color;
///	pixels represented by a 1 bit are drawn with the current background color."
///
// Direct method, with external palette
Bitmap* SCPatternFromMemBmp(HBITMAP hBM, SCShortDCState& rDCState)
{
	ASSERT(hBM);
	rDCState.bMonoBrush = FALSE;
#if 0
	// Won't work: GDI+ ignores the palette this time (try to understand)
	HPALETTE hPal = SCCreatePatternPalette(rDCState);
	ASSERT(hPal);
	Bitmap* pBitmap = new Bitmap(hBM, hPal);
	if (!pBitmap)
		return NULL;
	if (pBitmap->GetLastStatus()!=Ok)
	{
		ASSERT(0);
		delete pBitmap;
		pBitmap = NULL;
	}
	DeleteObject(hPal);
#else

	BITMAP bm;
	int iRes = GetObject(hBM, sizeof(BITMAP), &bm);
	ASSERT(iRes);

	// Trying to set a GDI+ palette after bitmap creation
	Bitmap* pBitmap = new Bitmap(hBM, (HPALETTE)NULL);
	if (!pBitmap)
		return NULL;
	if (pBitmap->GetLastStatus()!=Ok)
	{
		ASSERT(0);
		delete pBitmap;
		return NULL;
	}
	if (bm.bmBitsPixel!=1)
		return pBitmap;
	rDCState.bMonoBrush = TRUE;

	// Update color palette for 1-bpp.
	if (!SCSetMonochromeBrushPalette(pBitmap, rDCState))
	{
		ASSERT(0);
		delete pBitmap;
		pBitmap = NULL;
	}
#endif

	return pBitmap;
}


///
/// Create a GDI+ brush from a GDI LOGBRUSH.
///
Brush* SCBrushFromLogBrush(LOGBRUSH& rLogBrush, SCShortDCState& rDCState)
{
	Brush* pBrush = NULL;
	rDCState.bMonoBrush = FALSE;

	switch (rLogBrush.lbStyle)
	{
	case BS_SOLID:
		{// Solid brush
			Color BrushColor;
			BrushColor.SetFromCOLORREF(rLogBrush.lbColor);
			pBrush = new SolidBrush(BrushColor);
		}
		break;
		
	case BS_HATCHED:
		{// Hatched brush
			HatchStyle hatchStyle;
			switch (rLogBrush.lbHatch)
			{
			case HS_BDIAGONAL:
				hatchStyle = HatchStyleBackwardDiagonal;
				break;
			case HS_CROSS:
				hatchStyle = HatchStyleCross;
				break;
			case HS_DIAGCROSS:
				hatchStyle = HatchStyleDiagonalCross;
				break;
			case HS_FDIAGONAL:
				hatchStyle = HatchStyleForwardDiagonal;
				break;
			case HS_HORIZONTAL:
				hatchStyle = HatchStyleHorizontal;
				break;
			case HS_VERTICAL:
				hatchStyle = HatchStyleVertical;
				break;
			default:
				ASSERT(0); // not reached
			}

			Color FgColor;
			FgColor.SetFromCOLORREF(rLogBrush.lbColor);

			// Background color is controlled by current BkColor and BkMode in DC
			COLORREF ColorRef = rDCState.crBkColor;
			BYTE bAlpha = (TRANSPARENT==rDCState.dwBkMode) ? 0 : 255;
			Color BkColor(bAlpha, GetRValue(ColorRef), GetGValue(ColorRef), GetBValue(ColorRef));
			pBrush = new HatchBrush(hatchStyle, FgColor, BkColor);
		}
		break;

	case BS_PATTERN:
	case BS_PATTERN8X8:
		{// Patterned brush (memory bitmap) referenced by a HBITMAP in lbHatch
			//	"A brush created by using a monochrome (1 bit per pixel) bitmap has
			//	the text and background colors of the device context to which it is drawn.
			//	Pixels represented by a 0 bit are drawn with the current text color;
			//	pixels represented by a 1 bit are drawn with the current background color."
			//
			Bitmap* pBitmap = SCPatternFromMemBmp((HBITMAP)rLogBrush.lbHatch, rDCState);

			if (pBitmap)
			{
				pBrush = new TextureBrush(pBitmap);
				delete pBitmap;
			}
		}
		break;

	case BS_DIBPATTERNPT:
		{// Patterned brush (packed DIB) referenced by a pointer in lbHatch
			Bitmap* pBitmap = SCBitmapFromPtr((LPBYTE)rLogBrush.lbHatch);

			if (pBitmap)
			{
				pBrush = new TextureBrush(pBitmap);
				delete pBitmap;
			}
		}
		break;

	case BS_DIBPATTERN:
	case BS_DIBPATTERN8X8:
		{// Patterned brush (packed DIB) referenced by an HGLOBAL in lbHatch
			Bitmap* pBitmap = SCBitmapFromHGLOBAL((HGLOBAL)rLogBrush.lbHatch, rDCState);

			if (pBitmap)
			{
				pBrush = new TextureBrush(pBitmap);
				delete pBitmap;
			}
		}
		break;

	case BS_NULL:
		// pBrush = NULL; // already done
		break;

	case BS_INDEXED:
	case BS_MONOPATTERN:
		// Undocumented (maybe driver specific)
		ASSERT(0);
		break;

	default:
		ASSERT(0);
	}

	ASSERT(!pBrush || (pBrush->GetLastStatus()==Ok));
	return pBrush;
}

///
/// Create a GDI+ brush from a GDI LOGPEN.
///
Pen* SCPenFromLogPen(LOGPEN& rLogPen)
{
	if (PS_NULL==rLogPen.lopnStyle) // no bit masking is required
		return NULL;

	Color PenColor;
	PenColor.SetFromCOLORREF(rLogPen.lopnColor);

	Pen* pPen = new Pen(PenColor, (REAL)rLogPen.lopnWidth.x);
	// Line style
	switch (rLogPen.lopnStyle)
	{
	case PS_SOLID:
		pPen->SetDashStyle(DashStyleSolid);
		break;

	case PS_DASH:
		pPen->SetDashStyle(DashStyleDash);
		break;

	case PS_DOT:
		pPen->SetDashStyle(DashStyleDot);
		break;

	case PS_DASHDOT:
		pPen->SetDashStyle(DashStyleDashDot);
		break;

	case PS_DASHDOTDOT:
		pPen->SetDashStyle(DashStyleDashDotDot);
		break;

	case PS_INSIDEFRAME:
		// The pen is solid, geometric
		pPen->SetDashStyle(DashStyleSolid);

		// GDI:"When this pen is used in any graphics device interface (GDI) drawing
		// function that takes a bounding rectangle, the dimensions of the figure
		// are shrunk so that it fits entirely in the bounding rectangle, taking into
		// account the width of the pen."

		// GDI+:"PenAlignmentInset Specifies, when drawing a polygon,
		// that the pen is aligned on the inside of the edge of the polygon."
		pPen->SetAlignment(PenAlignmentInset);

		// One of the documentations is clearer than the other: make your choice.
		break;

	case PS_ALTERNATE:
		// fall through
	default:
		ASSERT(0);
	}

	// Investigate to find out if a default GDI+ pen should be considered GEOMETRIC.
	// If so apply default join and cap styles
#pragma message( __FILE__  "(449): TODO: Review default GDI+ pen")
#if 0
	// Default values (cf. MSDN "Pens in Win32")
		// Lines joint (applies only to PS_GEOMETRIC pens in GDI)
	pPen->SetLineJoin(LineJoinRound);

		// End cap (applies only to PS_GEOMETRIC pens in GDI)
	pPen->SetLineCap(LineCapRound, LineCapRound, DashCapRound);
#endif

	ASSERT(!pPen || (pPen->GetLastStatus()==Ok));
	return pPen;
}

///
/// Create a GDI+ brush from a GDI EXTLOGPEN.
///
Pen* SCPenFromExtLogPen(EXTLOGPEN* pExtLogPen, SCShortDCState& rDCState)
{
	ASSERT(pExtLogPen);

	INT iPenStyle = pExtLogPen->elpPenStyle & PS_STYLE_MASK;  // style bits masking is required
	if (PS_NULL==iPenStyle)
		return NULL;

	// Note: don't know how to tell GDI+ about PS_COSMETIC or PS_GEOMETRIC pens
	INT iPenType = pExtLogPen->elpPenStyle & PS_TYPE_MASK;

	// Default pen width and color
	INT iWidth = (PS_GEOMETRIC==iPenType) ? pExtLogPen->elpWidth : 1;
	Color PenColor;
	PenColor.SetFromCOLORREF(pExtLogPen->elpColor);

	Pen* pPen = new Pen(PenColor, (REAL)iWidth);

	// Line style
	switch (iPenStyle)
	{
	case PS_SOLID:
		pPen->SetDashStyle(DashStyleSolid);
		break;

	case PS_DASH:
		pPen->SetDashStyle(DashStyleDash);
		break;

	case PS_DOT:
		pPen->SetDashStyle(DashStyleDot);
		break;

	case PS_DASHDOT:
		pPen->SetDashStyle(DashStyleDashDot);
		break;

	case PS_DASHDOTDOT:
		pPen->SetDashStyle(DashStyleDashDotDot);
		break;

	case PS_USERSTYLE:
		pPen->SetDashStyle(DashStyleCustom);
		// user-supplied style array pointed to by elpStyleEntry (limited to
		// elpNumEntries elements) must be used to describe the custom pen
		{
			REAL* pReals = SCREALFromDWORD(pExtLogPen->elpStyleEntry, pExtLogPen->elpNumEntries);
			// "The length of each dash and space in the dash pattern is the product of
			// the element value in the array and the width of the Pen object."
			
			// So now we must devide somehow.
			// But I'm not sure the method coded here is bullet proof, since I should
			// use what GDI calls "style units" to make the division. But listen:
			// "If you absolutely have to know the style unit, the application must draw a styled
			// line in a bitmap and read it back to determine the size of the style units
			// in the x and y directions".
			if (0==iWidth)
				iWidth = 1;
			for (UINT i=0; (i<pExtLogPen->elpNumEntries); i++)
			{
				pReals[i] = pReals[i]/(REAL)iWidth;
			}

			pPen->SetDashPattern(pReals, pExtLogPen->elpNumEntries);
			delete [] pReals;
		}
		break;

	case PS_INSIDEFRAME:
		// comments in SCPenFromLogPen
		pPen->SetDashStyle(DashStyleSolid);
		pPen->SetAlignment(PenAlignmentInset);
		break;

	case PS_ALTERNATE:
		// Windows NT/2000/XP: we don't have the corresponding style in GDI+, so just pray.

		// fall through
	default:
		pPen->SetDashStyle(DashStyleSolid);
		ASSERT(0);
	}

	if (PS_GEOMETRIC==iPenType)
	{
		// Lines joint (applies only to PS_GEOMETRIC pens in GDI)
		switch (pExtLogPen->elpPenStyle & PS_JOIN_MASK)
		{
		case PS_JOIN_ROUND:
			pPen->SetLineJoin(LineJoinRound);
			break;
			
		case PS_JOIN_MITER:
			pPen->SetLineJoin(LineJoinMiter);
			break;
			
		case PS_JOIN_BEVEL:
			pPen->SetLineJoin(LineJoinBevel);
			break;
			
		default:
			// (cf. MSDN "Pens in Win32")
			pPen->SetLineJoin(LineJoinRound);
			ASSERT(0);
		}

		// End cap (applies only to PS_GEOMETRIC pens in GDI)
		switch (pExtLogPen->elpPenStyle & PS_ENDCAP_MASK)
		{
		case PS_ENDCAP_ROUND:
			pPen->SetLineCap(LineCapRound, LineCapRound, DashCapRound);
			break;
			
		case PS_ENDCAP_FLAT:
			pPen->SetLineCap(LineCapFlat, LineCapFlat, DashCapFlat);
			break;
			
		case PS_ENDCAP_SQUARE:
			pPen->SetLineCap(LineCapSquare, LineCapSquare, DashCapFlat);
			break;
			
		default:
			// (cf. MSDN "Pens in Win32")
			pPen->SetLineCap(LineCapRound, LineCapRound, DashCapRound);
			ASSERT(0);
		}

		// "all [brush related] members must be used to specify the brush
		// attributes of the pen".
		if  ((BS_HOLLOW!=pExtLogPen->elpBrushStyle) &&
			(BS_SOLID!=pExtLogPen->elpBrushStyle || PS_SOLID!=iPenStyle))
		{
			LOGBRUSH LogBrush;
			LogBrush.lbStyle = pExtLogPen->elpBrushStyle;
			LogBrush.lbColor = pExtLogPen->elpColor;
			LogBrush.lbHatch = pExtLogPen->elpHatch;
			
			BOOL bSvd = rDCState.bMonoBrush;
			
			Brush* pBrush = SCBrushFromLogBrush(LogBrush, rDCState);
			if (pBrush)
				pPen->SetBrush(pBrush);
			
			rDCState.bMonoBrush = bSvd;
		}
	} 
	// else PS_COSMETIC:
	// "the [pExtLogPen->elpColor] member specifies the color of the pen and the
	// [pExtLogPen->elpPenStyle] member must be set to BS_SOLID"

	return pPen;
}

///
///	Fill a two-color palette with current text/background colors of DC.
/// (For monochrome brush color realization)
///
void SCFillMonochromePalette(SCShortDCState& rDCState, PPALETTEENTRY palPalEntry)
{
	ASSERT(palPalEntry);

	COLORREF crBkColor = rDCState.crBkColor;
	COLORREF crTextColor = rDCState.crTextColor;
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
/// Attach a two-color palette to a GDI+ image, using the given colors.
///
inline BOOL SCSetMonochromeImagePalette(Image* pImage, COLORREF crTextColor, COLORREF crBkColor)
{
	ASSERT(pImage);

	BOOL bOk = FALSE;
	// Update color palette for 1-bpp.
	ColorPalette* pPal = (ColorPalette*)new BYTE[sizeof(ColorPalette) + 2*sizeof(ARGB)];
	pPal->Flags = PaletteFlagsHasAlpha;
	pPal->Count = 2;
	pPal->Entries[0] = Color::MakeARGB(255, GetRValue(crTextColor),
											GetGValue(crTextColor),
											GetBValue(crTextColor));
	pPal->Entries[1] = Color::MakeARGB(255, GetRValue(crBkColor),
											GetGValue(crBkColor),
											GetBValue(crBkColor));	

	bOk = (pImage->SetPalette(pPal)==Ok);
	delete [] (BYTE*)pPal;

	return bOk;
}

///
/// Attach a two-color palette to a GDI+ image, using current text and background color.
///
inline BOOL SCSetMonochromeBrushPalette(Image* pImage, SCShortDCState& rDCState)
{
	ASSERT(pImage);

	return SCSetMonochromeImagePalette(pImage, rDCState.crTextColor, rDCState.crBkColor);
}

///
/// Attach a two-color palette to a GDI+ image, using the DC state colors.
///
Brush* SCRealizeMonochromeBrushPalette(Brush* pBrush, SCShortDCState& rDCState)
{
	ASSERT(pBrush && pBrush->GetType()==BrushTypeTextureFill);
	if (!pBrush || pBrush->GetType()!=BrushTypeTextureFill)
		return NULL;

	// Well, the bitmap stored in the brush isn't 1bpp anymore
	// And setting palette on 32bpp bitmap will silently fail
	Bitmap* pBitmap = (Bitmap*)((TextureBrush*)pBrush)->GetImage(); // a 32bpp (a plain copy, not a "just to see" pointer)
	ASSERT(pBitmap && pBitmap->GetLastStatus()==Ok);

#if 0
	// This too fails. "Jeez!... it's full of 'stars!'"
	Bitmap* pClone = pBitmap->Clone(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight(), PixelFormat1bppIndexed);
#else
	// Manual cloning => bad/funny results
	// LockBits doesn't understand that WE WANT PixelFormat1bppIndexed!
//	"LockBits [...]
//	format 
//	[in] Integer that specifies the format of the pixel data in the temporary buffer.
//	The pixel format of the temporary buffer does not have to be the same as the pixel
//	format of this Bitmap object. The PixelFormat data type and constants that represent
//	various pixel formats are defined in Gdipluspixelformats.h. For more information about
//	pixel format constants, see Image Pixel Format Constants.
//	Microsoft Windows GDI+ version 1.0 does not support processing of 16-bits-per-channel
//	images, so you should not set this parameter equal to PixelFormat48bppRGB,
//	PixelFormat64bppARGB, or PixelFormat64bppPARGB."
// You can add PixelFormat1bppIndexed to the list.
	BitmapData BmData;
	Rect rect(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());
	Status iRes = pBitmap->LockBits(&rect, ImageLockModeRead,
		PixelFormat32bppARGB/*oops!*/, &BmData);
	ASSERT(iRes==Ok);

	Bitmap* pClone = new Bitmap(pBitmap->GetWidth(), pBitmap->GetHeight(),
		BmData.Stride, PixelFormat1bppIndexed, (BYTE*)BmData.Scan0);

	pBitmap->UnlockBits(&BmData); 
#endif

	ASSERT(pBitmap->GetLastStatus()==Ok);
	ASSERT(pClone && pClone->GetLastStatus()==Ok);

	BOOL bOk = SCSetMonochromeImagePalette(pClone, rDCState.crTextColor, rDCState.crBkColor);

	TextureBrush* pNewBrush = NULL;
	if (bOk)
	{
		// We can't just set
		//((TextureBrush*)pBrush)->SetBitmap(pClone);
		Matrix matrix;
		((TextureBrush*)pBrush)->GetTransform(&matrix);

		pNewBrush = new TextureBrush(pClone);
		if (pNewBrush)
		{
			Status iRes = pNewBrush->SetTransform(&matrix);
			ASSERT(iRes==Ok);
		}
	}
	delete pClone;
	delete pBitmap; // warning!?

	return pNewBrush;
}

#if 0
///
/// Create a GDI+ font from a GDI LOGFONT.
///
Font* SCFontFromLogFont(LOGFONT& rLogFont)
{
	ASSERT(0); // not implemented
	return NULL;
}
#endif


///
/// Build an TrueType approximation of the font describes in LogFont.
///	Note, this is a late font substitution (more complicated).
///
HFONT SCTTFontFromLOGFONT(LOGFONT& rLogFont, HDC hDC)
{
	rLogFont.lfOutPrecision = OUT_TT_ONLY_PRECIS;
	rLogFont.lfPitchAndFamily |= SCFontFamilyApproximant((TCHAR*)rLogFont.lfFaceName);

	HFONT hFont = CreateFontIndirect(&rLogFont);
	ASSERT(hFont);
	if (!hFont)
	{// silent failure
		switch (rLogFont.lfPitchAndFamily & 0xF0)
		{
		case FF_ROMAN:
			_tcscpy((TCHAR*)rLogFont.lfFaceName, _T("Times New Roman"));
			break;
			
		case FF_DONTCARE:
		case FF_SWISS:
		case FF_MODERN:
		case FF_SCRIPT:
		case FF_DECORATIVE:
		default:
			_tcscpy((TCHAR*)rLogFont.lfFaceName, _T("Arial"));
		}

		return NULL;
	}

	BOOL bTmpDC = (NULL==hDC);
	if (bTmpDC)
		hDC = ::GetDC(NULL);
	ASSERT(hDC);
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
	GetTextFace(hDC, LF_FACESIZE, rLogFont.lfFaceName);

	if (bTmpDC)
	{// no DC was given for selecting font
		SelectObject(hDC, hOldFont);
		ReleaseDC(NULL,hDC);
	} // else the returned font is selected in the DC
	  // the danger is we are losing hOldFont
	return hFont;
}


///
/// Create an array of GDI+ Points from an array of Win32 POINTs.
///
//	Point* SCPointFromPOINT(LPCPOINT pPoints, DWORD dwCount)
template <class T> 
Point* SCPointFromPOINT(T pPoints, DWORD dwCount)
{
	ASSERT(pPoints && dwCount);

	Point *pPts = new Point[dwCount];
	
	Point *pDest = pPts;
	Point *pLimit = pDest + dwCount;
	for(; (pDest<pLimit); pDest++, pPoints++) 
	{
		pDest->X = pPoints->x;
		pDest->Y = pPoints->y; 
	}

	return pPts;
}
// some explicit instantiations, cause it's a library
template Point* SCPointFromPOINT<LPPOINT>(LPPOINT, DWORD);
template Point* SCPointFromPOINT<LPPOINTS>(LPPOINTS, DWORD);
//

///
/// Create an array of GDI+ Points from an array of Win32 POINTSs completed by the
/// current position in play DC (hold in rPtStart).
///
template <class T>
Point* SCPointFromPOINTTo(Point& rPtStart, T pPoints, DWORD dwCount)
{
	ASSERT(pPoints && dwCount);
	// add +1 to count to include the current position in DC
	Point *pPts = new Point[++dwCount];
	
	// copy first point
	pPts->X = rPtStart.X;
	pPts->Y = rPtStart.Y; 

	// copy other points
	Point *pDest = pPts + 1;
	Point *pLimit = pPts + dwCount;

	for(; (pDest<pLimit); pDest++, pPoints++) 
	{
		pDest->X = pPoints->x;
		pDest->Y = pPoints->y; 
	}
	return pPts;
}
// some explicit instantiations, cause it's a library
template Point* SCPointFromPOINTTo<LPPOINT>(Point&, LPPOINT, DWORD);
template Point* SCPointFromPOINTTo<LPPOINTS>(Point&, LPPOINTS, DWORD);
//

///
/// Create an array of REALs from an array of DWORDs.
///
REAL* SCREALFromDWORD(LPDWORD pDWords, DWORD dwCount)
{
	ASSERT(pDWords && dwCount);

	REAL *pReals = new REAL[dwCount];
	
	REAL *pDest = pReals;
	REAL *pLimit = pDest + dwCount;
	while (pDest<pLimit)
	{
		*pDest++ = (REAL)*pDWords++;
	}

	return pReals;
}

///
/// Translate GDI clipping modes into GDI+ equivalent.
///
CombineMode SCClipModeFormRGNMode(INT iMode)
{
	switch (iMode)
	{
	case RGN_COPY:
		return CombineModeReplace;

	case RGN_AND:
		return CombineModeIntersect;

	case RGN_DIFF:
		return CombineModeExclude;

	case RGN_OR:
		return CombineModeUnion;

	case RGN_XOR:
		return CombineModeXor;

	default:
		ASSERT(0);
	}
	return CombineModeReplace;
}

///
/// Perfom a rop opeartion by filling a rectangle with the color located at
/// the given index in the default system palette. (WHITENESS/BLACKNESS)
///
void SCSysPaletteFillRect(I_SCDCGraphics* pIGraphics, Rect& destRect, UINT uiPalIndex)
{
	ASSERT(pIGraphics);
	ASSERT(uiPalIndex<256);

	PALETTEENTRY PaletteEntry;

	// Bogus GDI documentation?: index 1 doesn't correspond to WHITENESS as expected
	// (And we know that the 20 static colors (reserved) use the following indices:
	// 0-9, 246-255; but that's true for Windows 3.0 and 3.1; for other systems, !!??)
	//
	// So, consider using manual colors.
	HDC hdc = GetDC(NULL);
	GetSystemPaletteEntries (hdc, uiPalIndex, 1, &PaletteEntry);
	ReleaseDC(NULL, hdc);
	
	Color BrushColor;
	BrushColor.SetFromCOLORREF(pIGraphics->SCGetFinalColor(RGB(PaletteEntry.peRed,
															   PaletteEntry.peGreen,
															   PaletteEntry.peBlue)));
	SolidBrush brush(BrushColor);

	Graphics* pGraphics = pIGraphics->SCGetGraphics();

	ASSERT(pGraphics);
	pGraphics->FillRectangle(&brush, destRect);
}

///
/// Perfom a DIB blt opeartion using ROP.
///
INT SCStretchDIBits(
  I_SCDCGraphics* pIGraphics, // pointer to destination graphics context
  INT XDest,              // x-coordinate of upper-left corner of dest. rectangle
  INT YDest,              // y-coordinate of upper-left corner of dest. rectangle
  INT nDestWidth,         // width of destination rectangle
  INT nDestHeight,        // height of destination rectangle
  INT XSrc,               // x-coordinate of upper-left corner of source rectangle
  INT YSrc,               // y-coordinate of upper-left corner of source rectangle
  INT nSrcWidth,          // width of source rectangle
  INT nSrcHeight,         // height of source rectangle
  CONST VOID *lpBits,            // address of bitmap bits
  CONST BITMAPINFO *lpBitsInfo,  // address of bitmap data
  UINT iUsage,                   // usage flags
  DWORD dwRop                    // raster operation code
)
{
	ASSERT(pIGraphics);

	// Prepare the temporary destination bitmap
	HDC hOutputDC = pIGraphics->SCGetDC();
	ASSERT(hOutputDC);
	if (!hOutputDC)
		return FALSE;

	HDC hDestDC = CreateCompatibleDC(hOutputDC);
#if 0
	// best quality (but memory hog, and time consuming for GDI+, not for GDI)
	INT iTmpWidth = nSrcWidth;
	INT iTmpHeight = nSrcHeight;
#else
	// faster (but less smooth image)
	INT iTmpWidth = abs(nSrcWidth);
	INT iTmpHeight = abs(nSrcHeight);
	if (iTmpWidth>=1000 || iTmpHeight>=1000) // TODO: fix these arbitrary values
	{// large high resolution bitmaps will bog down GDI+
	 // (it won't crash, but it can take 5 minutes, and generate 250Mo memory peaks)
		iTmpWidth = abs(nDestWidth);	
		iTmpHeight = abs(nDestHeight);
		// inflate dest to enhance quality
		if (iTmpWidth<500 && iTmpHeight<500)
		{
			iTmpWidth *= 2;
			iTmpHeight *= 2;
		}
	}
#endif
	HBITMAP hDestBmp = CreateCompatibleBitmap(hOutputDC, iTmpWidth, iTmpHeight);
	if (!hDestBmp)
	{
		DWORD dwError = GetLastError();
		ASSERT(0);
	}
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDestDC, hDestBmp);

	// Copy the destination rectangle
	BOOL bOK = StretchBlt(hDestDC, 0, 0, iTmpWidth, iTmpHeight,
		   hOutputDC, XDest, YDest, nDestWidth, nDestHeight, SRCCOPY);
	if (!bOK)
	{// Maybe failure due to rotation/shear found in hOutputDC
	 // (do a manual resampling of the destination rectangle)
		float pitchx = float(nDestWidth)/float(iTmpWidth);
		float pitchy = float(nDestHeight)/float(iTmpHeight);
		int ys;
		for (int y=0; (y<iTmpHeight); y++)
		{
			ys = (int)(YDest + y*pitchy);
			int xs;
			for (int x=0; (x<iTmpWidth); x++)
			{
				xs = (int)(XDest + x*pitchx);
				COLORREF crColor = GetPixel(hOutputDC, xs, ys);
				SetPixel(hDestDC, x, y, crColor);
			}
		}
	}
	pIGraphics->SCReleaseDC(hOutputDC);

		// Ensure that the brush is in destination dc
	HBRUSH hBrush = NULL;
	HBRUSH hOldBrush = NULL;
	Brush* pBrush = pIGraphics->SCGetBrush(); // brush of destination
	if (pBrush)
	{
		Color BrushColor;
		switch(pBrush->GetType())
		{
		case BrushTypeSolidColor:
			((SolidBrush*)pBrush)->GetColor(&BrushColor);
			hBrush = CreateSolidBrush(BrushColor.ToCOLORREF());
			break;

		case BrushTypeHatchFill:
			((HatchBrush*)pBrush)->GetBackgroundColor(&BrushColor);
			hBrush = CreateSolidBrush(BrushColor.ToCOLORREF());
			break;
		}
	} // else NULL brush, let default brush in place.
	  // TODO: check that the ROP doesn't use the brush
	if (hBrush)
		hOldBrush = (HBRUSH)SelectObject(hDestDC, hBrush);

		// Merge source and dest (this is not smoothed)
	StretchDIBits(hDestDC, 0, 0, iTmpWidth, iTmpHeight,
					XSrc, YSrc, nSrcWidth, nSrcHeight,
					lpBits, lpBitsInfo, iUsage, dwRop);

		// We are done with dest DC
	if (hBrush)
	{
		SelectObject(hDestDC, hOldBrush);
		DeleteObject(hBrush);
	}
	SelectObject(hDestDC, hOldBmp);
	DeleteDC(hDestDC);

	// Create GDI+ bitmap
	{
		Bitmap Bmp(hDestBmp, (HPALETTE)NULL);
		ASSERT(Bmp.GetLastStatus()==Ok);
		
		// GDI+ drawing (smoothed a little according to graphics settings)
		Rect destRect(XDest, YDest, nDestWidth, nDestHeight);
		pIGraphics->SCDrawBitmap(&Bmp, destRect);
	}

	// Clean up
	DeleteObject(hDestBmp);

	return TRUE;
}

///
/// Compute a normalized rectangle to use in GDI+ functions.
/// (GDI's twisted rectangles would result in negative width/height, confusing GDI+)
///
void SCNormalizedRectFromRECT(Rect* pRect, LPCRECT pRc)
{
	ASSERT(pRect && pRc);
	if (pRc->right < pRc->left)
	{
		pRect->X = pRc->right;
		pRect->Width = pRc->left - pRc->right;
	} else
	{
		pRect->X = pRc->left;
		pRect->Width = pRc->right - pRc->left;
	}

	if (pRc->bottom < pRc->top)
	{
		pRect->Y = pRc->bottom;
		pRect->Height = pRc->top - pRc->bottom;
	} else
	{
		pRect->Y = pRc->top;
		pRect->Height = pRc->bottom - pRc->top;
	}
}

///
/// Compute clockwise angles to use in GDI+ functions from GDI's basic information.
///
void SCBuildArcAngles(LPCRECTL pBox, LPCPOINTL pPtStart, LPCPOINTL pPtEnd, INT iArcDir,
					  double& dStartAngle, double& dSweepAngle)
{
	double dCenterX = (pBox->right + pBox->left)/2;
	double dCenterY = (pBox->bottom + pBox->top)/2;
	
	dStartAngle = SC_DEGREES(atan2((pPtStart->y - dCenterY), pPtStart->x - dCenterX));
	double dEndAngle = SC_DEGREES(atan2((pPtEnd->y - dCenterY), pPtEnd->x - dCenterX));
	dSweepAngle = (dEndAngle - dStartAngle);

	// arc direction is used (AD_COUNTERCLOCKWISE or AD_CLOCKWISE)
	if (iArcDir==AD_CLOCKWISE)
	{// AD_CLOCKWISE for GDI
		if (dSweepAngle<=0)
		{// Negative values are counterclockwise for GDI+. So get the angle's negative complement.
			dSweepAngle = -(360 + dSweepAngle); // counterclockwise for GDI+
		} // else clockwise for GDI+
	} else
	{// AD_COUNTERCLOCKWISE for GDI
		if (dSweepAngle>=0)
		{// Positive values are clockwise for GDI+. So get the angle's negative complement.
			dSweepAngle = -(360 - dSweepAngle); // counterclockwise for GDI+
		} // else clockwise for GDI+
	}
}

///
/// Intersection of a cercle define by its center and radius, and a line passing by this center.
///	(Angle is in degrees)
///
void SCIntersectLineAndCircle(Point& PtRes, POINTL* pPtCenter, DWORD dwRadius, double fLineAngle)
{
	double dAngle = -SC_RAD(fLineAngle);
	PtRes.X = static_cast<INT>(pPtCenter->x + dwRadius*cos(dAngle));
	PtRes.Y = static_cast<INT>(pPtCenter->y + dwRadius*sin(dAngle));
}

///
/// Intersection of an ellipse define by a rectangle, and a line passing by its center.
/// (Angle is in degrees)
///
void SCIntersectLineAndEllipse(Point& PtRes, LPCRECTL pBox, double fLineAngle)
{
	int xPtCenter = (pBox->left + pBox->right)/2;
	int yPtCenter = (pBox->top + pBox->bottom)/2;

	double a = (pBox->right - pBox->left);
	double b = (pBox->bottom - pBox->top);

	double dAngle = -SC_RAD(fLineAngle);
	double a_sinAngle = a*sin(dAngle);
	double b_cosAngle = b*cos(dAngle);

	double d = sqrt(a_sinAngle*a_sinAngle + b_cosAngle*b_cosAngle);
	PtRes.X = static_cast<INT>(xPtCenter + (a*b_cosAngle/d));
	PtRes.Y = static_cast<INT>(yPtCenter + (b*a_sinAngle/d));
}

HBRUSH SCBrushFromGdipImage(Image* pImage)
{
	ASSERT(pImage);
	ASSERT(pImage->GetType()==ImageTypeBitmap);

	int iWidth = pImage->GetWidth();
	int iHeight = pImage->GetHeight();
	HDC hdc = CreateCompatibleDC(NULL);
	HBITMAP hbmp = NULL;

	BitmapData BmData;
	Rect rect(0, 0, iWidth, iHeight);
	Status iRes = ((Bitmap*)pImage)->LockBits(&rect, ImageLockModeRead,
		PixelFormat32bppARGB, &BmData);
	ASSERT(iRes==Ok);
	if (iRes==Ok)
	{
		BITMAPINFOHEADER bmih;
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biWidth = iWidth;
		bmih.biHeight = iHeight; // bottom-up (beware of sign)
		bmih.biPlanes = 1;
		bmih.biBitCount = 32;
		bmih.biCompression = BI_RGB;
		bmih.biClrUsed = 0;
		// just set the rest to 0
		bmih.biSizeImage = 0;
		bmih.biXPelsPerMeter = 0; 
		bmih.biYPelsPerMeter = 0;
		bmih.biClrImportant = 0;
		bmih.biSizeImage = iWidth * iHeight * 4;
		LPVOID pBits;
		hbmp = CreateDIBSection(hdc, (BITMAPINFO*)&bmih, DIB_RGB_COLORS, &pBits, NULL, 0);
		ASSERT(hbmp);
		if (hbmp)
		{
			SetDIBits(hdc, hbmp, 0, iHeight, (CONST VOID *)BmData.Scan0, (BITMAPINFO*)&bmih, DIB_RGB_COLORS);
		}
	}
	((Bitmap*)pImage)->UnlockBits(&BmData); 

	// Warning: May create resource leak, as we don't call DeleteObject on hbmp
	HBRUSH hBrush = CreatePatternBrush(hbmp);

	DeleteDC(hdc);
	return hBrush;
}

///
/// Perfom a pattern blt opeartion using ROP.
///
INT SCMergeAreaWithPattern(
  I_SCDCGraphics* pIGraphics,    // pointer to destination graphics context
  Rect& destRect,		  // dest. rectangle
  DWORD dwRop)            // raster operation code
{
	ASSERT(pIGraphics);

	// Prepare the temporary destination bitmap
	HDC hOutputDC = pIGraphics->SCGetDC();
	ASSERT(hOutputDC);
	if (!hOutputDC)
		return FALSE;

	HDC hDestDC = CreateCompatibleDC(hOutputDC);
	INT nDestWidth = destRect.Width; // width of destination rectangle
	INT nDestHeight = destRect.Height; // height of destination rectangle
	HBITMAP hDestBmp = CreateCompatibleBitmap(hOutputDC, nDestWidth, nDestHeight);
	ASSERT(hDestBmp);
	if (!hDestBmp)
	{
		DWORD dwError = GetLastError();
		return FALSE;
	}
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDestDC, hDestBmp);

		// Copy the destination rectangle
	if (!StretchBlt(hDestDC, 0, 0, nDestWidth, nDestHeight,
		   hOutputDC, destRect.X, destRect.Y, nDestWidth, nDestHeight, SRCCOPY))
	{// Maybe failure for rotation/shear found in hOutputDC
	 // (do a manual copy of the destination rectangle)
		int ys = destRect.Y;
		for (int y=0; (y<nDestHeight); y++, ys++)
		{
			int xs = destRect.X;
			for (int x=0; (x<nDestWidth); x++, xs++)
			{
				COLORREF Color = GetPixel(hOutputDC, xs, ys);
				SetPixel(hDestDC, x, y, Color);
			}
		}
	}
	pIGraphics->SCReleaseDC(hOutputDC);

		// Ensure that the brush is in destination dc
	HBRUSH hBrush = NULL;
	Brush* pBrush = pIGraphics->SCGetBrush(); // brush of destination
	if (pBrush)
	{
		Color BrushColor;
		switch(pBrush->GetType())
		{
		case BrushTypeSolidColor:
			((SolidBrush*)pBrush)->GetColor(&BrushColor);
			hBrush = CreateSolidBrush(BrushColor.ToCOLORREF());
			break;

		case BrushTypeHatchFill:
			((HatchBrush*)pBrush)->GetBackgroundColor(&BrushColor);
			hBrush = CreateSolidBrush(BrushColor.ToCOLORREF());
			break;

		case BrushTypeTextureFill:
			{
				Image* pImage = ((TextureBrush*)pBrush)->GetImage();
				ASSERT(pImage);
				hBrush = SCBrushFromGdipImage(pImage);
				delete pImage;
			}
			break;

		default:
			ASSERT(0); // TODO

		}
	} // else NULL brush, let default brush in place.
		// TODO: check that the ROP doesn't use the brush

	// Merge source and dest (this is not smoothed)
	if (hBrush)
	{
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hDestDC, hBrush);
		
		PatBlt(hDestDC, 0, 0, nDestWidth, nDestHeight, dwRop); 
		
		SelectObject(hDestDC, hOldBrush);
		DeleteObject(hBrush);
	} else
	{
		PatBlt(hDestDC, 0, 0, nDestWidth, nDestHeight, dwRop);
	}

	// We are done with dest DC
	SelectObject(hDestDC, hOldBmp);
	DeleteDC(hDestDC);

	// Create GDI+ bitmap
	{
		Bitmap Bmp(hDestBmp, (HPALETTE)NULL);
		ASSERT(Bmp.GetLastStatus()==Ok);
		
		pIGraphics->SCDrawBitmap(&Bmp, destRect);
	}

	// Clean up
	DeleteObject(hDestBmp);

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////
// The algorithm of this function was adapted from XPDF
// (Patrick Moreau, Martin P.J. Zinser, (c) Glyph & Cog, LLC)
//
// "kludge for polygon fills:  First, it divides up the [subpolys] into
// non-overlapping polygons by simply comparing bounding rectangles.
// Then it connects [subpolys] within a single compound polygon to a single
// point so that [GDI+] can fill the polygon (sort of)."
//
Point* SCConvertPolyPoly(Point* pPolyPoints, DWORD& numPoints,
					  DWORD* pPolyVertices, DWORD& dwNbPolys)
{
	Point* pPoints = pPolyPoints;

	// allocate new points
	int size = numPoints + dwNbPolys*2; // add 2 more points by poly
	Point* pPolyPts = new Point[size];

	// allocate bounding rectangles array
	RECT *rects = new RECT[dwNbPolys];
	
	// rebuild each subpoly
	numPoints = 0;
	Point Pt0(0, 0);
	Point* pLimit = pPoints;
	for (int iPoly = 0; (iPoly < (int)dwNbPolys); ++iPoly)
	{
		// add the points
		int iFirstPt = numPoints;
		pLimit += pPolyVertices[iPoly];
		for (; (pPoints < pLimit); pPoints++)
		{	pPolyPts[numPoints++] = *pPoints; }
		
		// construct bounding rectangle
		rects[iPoly].left = rects[iPoly].right = pPolyPts[iFirstPt].X;
		rects[iPoly].top = rects[iPoly].bottom = pPolyPts[iFirstPt].Y;
		for (int k = iFirstPt + 1; (k < (int)numPoints); ++k)
		{
			if (pPolyPts[k].X < rects[iPoly].left)
				rects[iPoly].left = pPolyPts[k].X;
			else if (pPolyPts[k].X > rects[iPoly].right)
				rects[iPoly].right = pPolyPts[k].X;
			if (pPolyPts[k].Y < rects[iPoly].top)
				rects[iPoly].top = pPolyPts[k].Y;
			else if (pPolyPts[k].Y > rects[iPoly].bottom)
				rects[iPoly].bottom = pPolyPts[k].Y;
		}
		
		// close subpoly if necessary
		if (pPolyPts[numPoints-1].X != pPolyPts[iFirstPt].X ||
			pPolyPts[numPoints-1].Y != pPolyPts[iFirstPt].Y)
		{
			// add point
			pPolyPts[numPoints++] = pPolyPts[iFirstPt];
		}
		
		// length of this subpoly
		pPolyVertices[iPoly] = numPoints - iFirstPt;
		
		// leave an extra point for compound fill hack
		pPolyPts[numPoints++] = Pt0;
	}
	
	// combine compound polygons
	RECT rect;				// temp rect for overlaps detection
	DWORD dwNewNbPolys = 0; // new polygons counter
	int iCurPoly = 0;		// index of current polygon to examine or merge
	int iOverlap = 0;		// index of a polygon overlapping iCurPoly
	int iFirstPt = 0;		// index of first point in iCurPoly
	int iLastPt = 0;		// index of last point in iCurPoly
	while (iCurPoly < (int)dwNbPolys)
	{
		// start a new polygon with the iCurPoly subpoly
		rect = rects[iCurPoly];
		pPolyVertices[dwNewNbPolys] = pPolyVertices[iCurPoly];
		iFirstPt = iLastPt;
		iLastPt += pPolyVertices[iCurPoly] + 1; // +1 for the extra point
		pPolyPts[iLastPt - 1] = pPolyPts[iFirstPt];
		
		// combine overlapping polygons
		++iCurPoly;
		do
		{
			// look for the first subsequent subpoly, if any, which overlaps
			for (iOverlap = iCurPoly; (iOverlap < (int)dwNbPolys); ++iOverlap)
			{
				if (rects[iOverlap].right > rects[iCurPoly].left &&
					rects[iOverlap].left < rects[iCurPoly].right &&
					rects[iOverlap].bottom > rects[iCurPoly].top &&
					rects[iOverlap].top < rects[iCurPoly].bottom)
				{
					// there is an overlap, combine the polygons
					// (by anchoring them at the first point of current polygon)
					for (; (iCurPoly <= iOverlap); ++iCurPoly)
					{
						if (rects[iCurPoly].left < rect.left)
							rect.left = rects[dwNewNbPolys].left;
						if (rects[iCurPoly].right > rect.right)
							rect.right = rects[dwNewNbPolys].right;
						if (rects[iCurPoly].top < rect.top)
							rect.top = rects[dwNewNbPolys].top;
						if (rects[iCurPoly].bottom > rect.bottom)
							rect.bottom = rects[dwNewNbPolys].bottom;
						
						// merge points
						pPolyVertices[dwNewNbPolys] += pPolyVertices[iCurPoly] + 1;
						// anchor this subpoly
						iLastPt += pPolyVertices[iCurPoly] + 1;
						pPolyPts[iLastPt - 1] = pPolyPts[iFirstPt];
					}

					// exit to check next overlap with this compound polygon
					break;
				}
			}
		} while (iOverlap < (int)dwNbPolys && iCurPoly < (int)dwNbPolys);
		
		++dwNewNbPolys;
	}
	
	// free bounding rectangles
	delete [] rects;
	dwNbPolys = dwNewNbPolys;

	return pPolyPts;
}
//////////////////////////////////////////////////////////////////////////////////////////


///
/// Create a GDI+ list of PathPointTypes from a list of GDI point types.
///
BYTE* SCPathPointTypesFROMCurveTypes(LPCBYTE pGDITypes, DWORD dwCount)
{
	ASSERT(pGDITypes);

	BYTE *pTypes = new BYTE[dwCount];

	if (pTypes)
	{
		memmove(pTypes, pGDITypes, dwCount);
		SCConvertGDIPointTypes(pTypes, dwCount);
	}
	return pTypes;
}
	
///
/// Convert (in place) a list of GDI point types to GDI+ PathPointTypes.
///
void SCConvertGDIPointTypes(LPBYTE pTypes, DWORD dwCount)
{
	ASSERT(pTypes);

	// convert path point types
	for (DWORD i=0; (i<dwCount); i++)
	{
		// GDI+ documentation states:
		// "The PathPointType enumeration indicates point types and flags for the data points
		// in a path. Bits 0 through 2 indicate the type of a point, and bits 3 through 7
		// hold a set of flags that specify attributes of a point."
		// Hence the dirty cast of an enum into BYTE;
		// Note that no function takes an array of PathPointType in the GraphicsPath class.
		
		switch (pTypes[i] & SC_PATH_PTMASK)
		{
		case PT_MOVETO:
			pTypes[i] = (BYTE)PathPointTypeStart;
			break;
			
		case PT_LINETO:
			if (pTypes[i] & SC_PATH_CLOSEFIG)
				pTypes[i] = (BYTE)(PathPointTypeLine|PathPointTypeCloseSubpath);
			else
				pTypes[i] = (BYTE)PathPointTypeLine;
			break;
			
		case PT_BEZIERTO:
			if (pTypes[i] & SC_PATH_CLOSEFIG)
				pTypes[i] = (BYTE)(PathPointTypeBezier|PathPointTypeCloseSubpath);
			else
				pTypes[i] = (BYTE)PathPointTypeBezier;
			break;
		}
	}
}

///
/// Subtype of an EMF: EmfTypeEmfOnly|EmfTypeEmfPlusOnly|EmfTypeEmfPlusDual
///
EmfType SCGdipGetEMFType(HENHMETAFILE hemf)
{
	Metafile metafile(hemf);
	MetafileHeader header;
	metafile.GetMetafileHeader(&header);
	switch(header.GetType())
	{
    case MetafileTypeEmfPlusOnly:
		return EmfTypeEmfPlusOnly;

    case MetafileTypeEmfPlusDual:
		return EmfTypeEmfPlusDual;
	}

	return EmfTypeEmfOnly;
}

///
/// Any image to Enhanced Metafile format
///
BOOL SCGdipPlayMetafile(HDC hDCPlay, HENHMETAFILE hemf, LPCRECT pRect)
{
	ASSERT(hDCPlay);
	ASSERT(hemf);
	ASSERT(pRect);

	Metafile metafile(hemf);
	MetafileHeader header;
	metafile.GetMetafileHeader(&header);

	Graphics graphics(hDCPlay);
	Rect destRec(pRect->left, pRect->top, PRECT_WIDTH(pRect), PRECT_HEIGHT(pRect));
	graphics.DrawImage(&metafile, destRec);
	return (graphics.GetLastStatus()==Ok);
}


///
/// Any GDIp image to Enhanced Metafile format
///
HENHMETAFILE SCConvertImagetoEMF(Image& rImage)
{
	// Store image in metafile.

	// Use printer DC to avoid loss in resolution
	BOOL bPrnDC = FALSE;
#if 1
	HDC hdc = SCGetDefaultPrinterDC(NULL);
	if (hdc)
		bPrnDC = TRUE;
	else
	{
		hdc = GetDC(NULL); // accept loss in resolution
		if (!hdc)
			return NULL;
	}
#else
	// test for screen dc
	HDC hdc = GetDC(NULL);
	if (!hdc)
		return NULL;
#endif

	// Note: this, sometimes, generates images bigger or smaller than expected.
	// Some JPEGs are completely blured. Where is my part in this bug?
	Metafile metafile(hdc, EmfTypeEmfOnly);

	ASSERT(metafile.GetLastStatus()==Ok);
	{// we want the graphics object to detach itself from the metafile before we
	 // extract the GDI handle
		Graphics graphics(&metafile);
		ASSERT(graphics.GetLastStatus()==Ok);

		graphics.DrawImage(&rImage, 0, 0);
		ASSERT(graphics.GetLastStatus()==Ok);
	}

	HENHMETAFILE hEMF = metafile.GetHENHMETAFILE();
	ASSERT(metafile.GetLastStatus()==Ok);
	if (bPrnDC)
		DeleteDC(hdc);
	else
		ReleaseDC(NULL, hdc);

	return hEMF;
}

///
/// Any (gdi+ supported) image file to Enhanced Metafile format
///
HENHMETAFILE SCConvertImagetoEMF(LPCTSTR lpszFname)
{
	// load image
#ifdef _UNICODE
	Image image(lpszFname);
#else
	WCHAR	wFileName[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, (LPSTR)lpszFname, _tcslen(lpszFname)+1, wFileName, MAX_PATH);
	Image image(wFileName);
#endif
	ASSERT(image.GetLastStatus()==Ok);

	return SCConvertImagetoEMF(image);
}

///
/// Bitmap to Enhanced Metafile format
///
HENHMETAFILE SCGDIpConvertBitmaptoEMF(HBITMAP hbm, HPALETTE hpal)
{
	Bitmap bmp(hbm, hpal);
	ASSERT(bmp.GetLastStatus()==Ok);

	return SCConvertImagetoEMF(bmp);
}

///
/// Any piece of memory containing an image to Enhanced Metafile format
///
HENHMETAFILE SCGDIpConvertImagetoEMF(HANDLE hMem)
{
	HENHMETAFILE hEMF = NULL;
	IStream* pIStream = NULL;
	HRESULT hr = ::CreateStreamOnHGlobal((HGLOBAL)hMem, FALSE, &pIStream);
	if ((hr == S_OK) && pIStream)
	{
		Image img(pIStream, TRUE);
		if (img.GetLastStatus()==Ok)
			hEMF = SCConvertImagetoEMF(img);
		pIStream->Release();
	}
	return hEMF;
}

int MSGetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}


BOOL SCGDIpSaveImage(HBITMAP hbm, LPCTSTR lpszPathname, INT iType, INT iJPEGQuality/*=100*/)
{
	ASSERT(hbm);
	EncoderParameters encoderParameters;
	ULONG             quality;
	EncoderParameters* pParameters = NULL;

	// Check encoder
   CLSID clsidEncoder;
   INT   iEncoder;
   switch (iType & SC_SUBTYPE_MASK)
   {
   case SC_SUBTYPE_IMG_JPG:
	   iEncoder = MSGetEncoderClsid(L"image/jpeg", &clsidEncoder);
	   // optimistically prepare quality
	   encoderParameters.Count = 1;
	   encoderParameters.Parameter[0].Guid = EncoderQuality;
	   encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	   encoderParameters.Parameter[0].NumberOfValues = 1;
	   quality = iJPEGQuality;
	   encoderParameters.Parameter[0].Value = &quality;
	   pParameters = &encoderParameters;
	   break;

   case SC_SUBTYPE_IMG_BMP: iEncoder = MSGetEncoderClsid(L"image/bmp", &clsidEncoder); break;
   case SC_SUBTYPE_IMG_PNG: iEncoder = MSGetEncoderClsid(L"image/png", &clsidEncoder); break;
   case SC_SUBTYPE_IMG_GIF: iEncoder = MSGetEncoderClsid(L"image/gif", &clsidEncoder); break;
   case SC_SUBTYPE_IMG_TIFF: iEncoder = MSGetEncoderClsid(L"image/tiff", &clsidEncoder); break;
   default:
	   {
		   ASSERT(0);
		   return FALSE;
	   }
   }

   if (iEncoder<0)
	   return FALSE;

   // render image
   Bitmap image(hbm, (HPALETTE)NULL);

#ifdef _UNICODE
   Status lRes = image.Save(lpszPathname, &clsidEncoder, pParameters);
#else
   WCHAR	wPathName[MAX_PATH];
   MultiByteToWideChar(CP_ACP, 0, (LPSTR)lpszPathname, _tcslen(lpszPathname)+1, wPathName, MAX_PATH);
   Status lRes = image.Save(wPathName, &clsidEncoder, pParameters);
#endif

   return (lRes == Ok);
}




///
/// Rotate and translate Graphics so that the origin is at (rect.left, rect.top)
///
BOOL SCRotateGraphics(Graphics* pGraphics, int iAngle, CRect rect, int iXPos, int iYPos)
{
	ASSERT(pGraphics);
	BOOL bOk = FALSE;
	XFORM xform;
	memset(&xform, 0, sizeof(xform));

	// Formulas:	
	//xform.eM11 = (float)cos((float)iAngle*(ST_PI)/180.0f); => 0 for PI/2 and 3PI/2, 1 for 0 and PI
	//xform.eM12 = (float)sin((float)iAngle*(ST_PI)/180.0f); => 0 for 0 and PI, 1 for PI/2 and 3PI/2

	// rotate and translate at once
	switch (iAngle)
	{
	case 90:
		xform.eM11 = 0;
		xform.eM12 = 1;
		xform.eDx = (float)(rect.bottom + iXPos);
		xform.eDy = (float)(-rect.left + iYPos);
		break;

	case 270:
		xform.eM11 = 0;
		xform.eM12 = -1;
		xform.eDx = (float)(-rect.top + iXPos);
		xform.eDy = (float)(rect.right + iYPos);
		break;

	case 180:
		xform.eM11 = -1;
		xform.eM12 = 0;
		xform.eDx = (float)(rect.right + iXPos);
		xform.eDy = (float)(rect.bottom + iYPos);
		break;

	case 0:
		xform.eM11 = 1;
		xform.eM12 = 0;
		xform.eDx = (float)(-rect.left + iXPos);
		xform.eDy = (float)(-rect.top + iYPos);
		break;

	default:
		ASSERT(0);
	}
	xform.eM22 = xform.eM11;
	xform.eM21 = -xform.eM12;

	Matrix matrix(xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy);
	pGraphics->SetTransform(&matrix);

	bOk = (pGraphics->GetLastStatus()==Ok);
	ASSERT(bOk);

	return bOk;
}


BOOL CALLBACK SCGDIpFlatEnumMetafile(Gdiplus::EmfPlusRecordType recordType,
											UINT flags, UINT uiEMFRecDataSize,
											const BYTE* pEMFRecData, VOID* pCallbackData)
{
	Metafile* pMetafile = (Metafile*)pCallbackData;
	pMetafile->PlayRecord(recordType, flags, uiEMFRecDataSize, pEMFRecData);
	return TRUE;
}

///
/// Convert a GDI EMF to a GDI+ (scaled, rotated, etc...) EMF
///
/// WARNING: This does NOT WORK! (complex ROP codes fail,...)
///
HENHMETAFILE SCBBoxConvertEMFtoEMFp(HENHMETAFILE hEMF,
								SCGDIpDrawingAttributes& rDrawingAttributes, EmfType eType,
								CRect& rcDest, CRect& rcMeta, CRect& PaperRect, HDC hDCPlay,
								REAL iZoom, INT iAngle, I_SCRenderingContext* pIPostRender/*=NULL*/)
{
#pragma message( __FILE__  "(1721): TODO: Fix SCBBoxConvertEMFtoEMFp ")
	ASSERT(hEMF);

	CSize sizeEMF;
	SCGetEMFPlaySize(hEMF, sizeEMF);

	hDCPlay = CreateCompatibleDC(NULL);
	HBITMAP hbm = CreateCompatibleBitmap(hDCPlay, sizeEMF.cx, sizeEMF.cy);
	HBITMAP hOldBm = (HBITMAP)SelectObject(hDCPlay, hbm);

//PRB1: the 81 dpi syndrome? (see SC_USING_TRICK81 in SCEMFImage.cpp)
//		Rect frameRect(rcMeta.left, rcMeta.top, rcMeta.Width(), rcMeta.Height());
//		Metafile* pMeta = new Metafile(hDCPlay, frameRect, MetafileFrameUnitGdi, eType);
	UNUSED(rcMeta);
	Rect frameRect(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height());
	Metafile* pMeta = new Metafile(hDCPlay, frameRect, MetafileFrameUnitPixel, eType);
	ASSERT(pMeta->GetLastStatus()==Ok);

	// Play EMF in pMeta.
	int iPlayX = rcDest.left - PaperRect.left;
	int iPlayY = rcDest.top - PaperRect.top;
	CRect rcPlay(iPlayX, iPlayY, iPlayX + (int)(iZoom*sizeEMF.cx), iPlayY + (int)(iZoom*sizeEMF.cy));

#if 0
//PRB2: renderer does not work.
	BOOL bUseRenderer = (SCGdipGetEMFType(hEMF)!=EmfTypeEmfPlusOnly);
#else
	BOOL bUseRenderer = FALSE;
#endif

	// Note: Graphics object must be detached from the metafile before we can retrieve
	// the metafile handle; of course, post renderer must be called before detachment.
	if (bUseRenderer)
	{
		// Clip
		::IntersectClipRect(hDCPlay, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom);

		// Rotate
		int iGrOldMode;
		if (iAngle)
		{// Position of the PAGE rectangle for rotation in world coordinates
			CRect rcRotate = PaperRect;
			rcRotate.OffsetRect(iPlayX, iPlayY);
			SCRotateDC(hDCPlay, iAngle, rcRotate, rcDest.left, rcDest.top, iGrOldMode);
		}

		// Parse
		CSCEMFmetaDCRenderer renderer(pMeta);
		rDrawingAttributes.bPrinting = FALSE;
		renderer.SCSetDrawingAttributes(rDrawingAttributes);

		CSCEMFgdiParser	parser(&renderer);
		parser.SCParse(hEMF, hDCPlay, (LPRECT)&rcPlay);

		if (pIPostRender)
		{
			// restore the pre-rendering state to get a clean graphics
			parser.SCRestorePreRenderingState();
			GDPGraphics* pGraphics = renderer.SCGetGraphics();
			ASSERT(pGraphics);
			// post render
			pIPostRender->SCPostRender(hDCPlay, NULL, pGraphics);
		}
	} else
	{// Draw only
		// Note: this too has problems; a combination of 2 images (one with ROP:SRCPAINT
		// the second with ROP:SRCAND) produces a black image.
		// Where is this my responsibility in this bug?
		Metafile metafile(hEMF);
		Graphics graphics(pMeta);

		// Clip
		// You know what? GDI+ finds a way to place this clipping instruction in the metafile
		// AFTER the following FillRectangle.
		Rect rectClip(rcDest.left, rcDest.top, RECT_WIDTH(rcDest), RECT_HEIGHT(rcDest));
		graphics.SetClip(rectClip, CombineModeReplace);

		// Background
		if (rDrawingAttributes.bBkSolid)
		{
			Color BrushColor;
			BrushColor.SetFromCOLORREF(rDrawingAttributes.crPaperColor);
			SolidBrush brush(BrushColor);
			rectClip.Inflate(1, 1); // include border; otherwise clipping will be lost (!?)
			graphics.FillRectangle(&brush, rectClip);
		} // else clipping is lost

		// Rotate
		if (iAngle)
		{
			CRect rcRotate = PaperRect;
			rcRotate.OffsetRect(iPlayX, iPlayY);
			SCRotateGraphics(&graphics, iAngle, rcRotate, rcDest.left, rcDest.top);
		}

		// Draw
#if 0
		// a function is missing. I give up here.
		
		ImageAttributes imgattributes;
		if (rDrawingAttributes.crPaperColor!=RGB(255, 255, 255))
		{
			ColorMap clrMap;
			clrMap.oldColor = Color(255, 255, 255, 255);
			clrMap.newColor = BrushColor;
			imgattributes.SetRemapTable(1, &clrMap);
		}

		RectF rectImage(rcPlay.left, rcPlay.top, rcPlay.Width(), rcPlay.Height());
		// 1) call not understood
		//	graphics.DrawImage(&metafile, rectImage,
		//				rcMeta.X/100.0, rcMeta.Y/100.0,
		//				rcMeta.Width/100.0, rcMeta.Height/100.0,
		//				UnitMillimeter,
		//				&imgattributes, NULL, NULL);
		
		// 2) data is falling into ... metafile
		//	graphics.EnumerateMetafile(&metafile, rectImage,
		//				SCGDIpFlatEnumMetafile, &metafile, &imgattributes);
		
		// 3) and we can't enumerate accross metafiles
		//	graphics.EnumerateMetafile(&metafile, rectImage,
		//				SCGDIpFlatEnumMetafile, pMeta, &imgattributes);
		
		// The PlayRecord function should be a member
		// of Graphics, not of Metafile.
		// Now, my son, you see the power of a DC?
#else
		// Here, we are losing the remaining drawing attributes
		Rect rectImage(rcPlay.left, rcPlay.top, rcPlay.Width(), rcPlay.Height());
		graphics.DrawImage(&metafile, rectImage);
		if (pIPostRender)
		{
			pIPostRender->SCPostRender(hDCPlay, NULL, &graphics);
		}
#endif
	}

	HENHMETAFILE hEMFp = pMeta->GetHENHMETAFILE();
	delete pMeta;
	pMeta = NULL;

	SelectObject(hDCPlay, hOldBm);
	DeleteObject(hbm);
	DeleteDC(hDCPlay);

	ASSERT(hEMFp);
	return hEMFp;
}

///
/// Convert a GDI EMF to a GDI+ EMF (unscaled and no border)
///
/// WARNING: This does NOT WORK!
///
HENHMETAFILE SCConvertEMFtoEMFp(HENHMETAFILE hEMF,
								SCGDIpDrawingAttributes& rDrawingAttributes,
								EmfType eType)
{
#pragma message( __FILE__  "(1859): TODO: Fix SCConvertEMFtoEMFp")
	ASSERT(hEMF);
	ENHMETAHEADER EmfHeader;
	if (!::GetEnhMetaFileHeader(hEMF, sizeof(ENHMETAHEADER), &EmfHeader))
		return NULL;

	CSize sizeEMF;
	SCGetEMFPlaySize(hEMF, sizeEMF);

	HDC hDCPlay = CreateCompatibleDC(NULL);
	HBITMAP hbm = CreateCompatibleBitmap(hDCPlay, sizeEMF.cx, sizeEMF.cy);
	HBITMAP hOldBm = (HBITMAP)SelectObject(hDCPlay, hbm);

//PRB1: the 81 dpi syndrome? (see SC_USING_TRICK81 in SCEMFImage.cpp)
//		Rect frameRect(0, 0, RECT_WIDTH(EmfHeader.rclFrame), RECT_HEIGHT(EmfHeader.rclFrame));
//		Metafile* pMeta = new Metafile(hDCPlay, frameRect, MetafileFrameUnitGdi, eType);
	Rect frameRect(0, 0, sizeEMF.cx, sizeEMF.cy);
	Metafile* pMeta = new Metafile(hDCPlay, frameRect, MetafileFrameUnitPixel, eType);
	ASSERT(pMeta->GetLastStatus()==Ok);

	// Play EMF in pMeta.
	CRect rcPlay(0, 0, sizeEMF.cx, sizeEMF.cy);
//PRB2: renderer does not work.
	BOOL bUseRenderer = (SCGdipGetEMFType(hEMF)==EmfTypeEmfOnly);

	// Note: Graphics object must be detached from the metafile before we can retrieve
	// the metafile handle
	if (bUseRenderer)
	{
		::IntersectClipRect(hDCPlay, rcPlay.left, rcPlay.top, rcPlay.right, rcPlay.bottom);

		CSCEMFmetaDCRenderer renderer(pMeta);

		rDrawingAttributes.bPrinting = FALSE;
		renderer.SCSetDrawingAttributes(rDrawingAttributes);

		CSCEMFgdiParser	parser(&renderer);
		parser.SCParse(hEMF, hDCPlay, (LPRECT)&rcPlay);
	}
	else
	{// Draw only (for example to just convert to/from GDI+)
		Metafile metafile(hEMF);
		Graphics graphics(pMeta);

		// background (can be deactivated before calling this function)
		if (rDrawingAttributes.bBkSolid)
		{
			Color BrushColor;
			BrushColor.SetFromCOLORREF(rDrawingAttributes.crPaperColor);
			SolidBrush brush(BrushColor);
			Rect rectBkgn(rcPlay.left, rcPlay.top, rcPlay.Width(), rcPlay.Height());
			graphics.FillRectangle(&brush, rectBkgn);
		}
	
		// draw
		Rect destRec(rcPlay.left, rcPlay.top, rcPlay.Width(), rcPlay.Height());
		graphics.DrawImage(&metafile, destRec);
	}

	HENHMETAFILE hEMFp = pMeta->GetHENHMETAFILE();
	delete pMeta;

	SelectObject(hDCPlay, hOldBm);
	DeleteObject(hbm);
	DeleteDC(hDCPlay);

	return hEMFp;
}
