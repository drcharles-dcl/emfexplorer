/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */
#ifndef _SCGDIPLUSUTILS_H_
#define _SCGDIPLUSUTILS_H_

#include "unknwn.h"		// or GDI+ won't let us compile
#include "gdiplus.h"
#include "SCGenInclude.h"
#include SC_INC_GENLIB(SCGenDefs.h)
#include SC_INC_WINLIB(SCGDIUtils.h)

#include "SCGdiplusTypes.h"

using namespace Gdiplus;

#include "SCEMFViewDefs.h"

typedef struct tag_SCGDIpDrawingAttributes
{
	TextRenderingHint	eTextRenderingHint;
	UINT				uiTextContrast;  // hack: 0-12 => from darker to brighter
	SmoothingMode		eSmoothingMode;
	InterpolationMode	eInterpolationMode;
	PixelOffsetMode		ePixelOffsetMode;
	COLORREF			crPaperColor;
	DWORD				dwInkingMode;
	//----
	BOOL				bReverseVideo;	// RV activation/deactivation
	BOOL				bPrinting;
	float				fPageScale;
	BOOL				bBkSolid;

	tag_SCGDIpDrawingAttributes():
#if 0
	// default low quality (would be like not using GDI+)
		eTextRenderingHint(TextRenderingHintSystemDefault),
		uiTextContrast(0),
		eSmoothingMode(SmoothingModeNone),
		eInterpolationMode(InterpolationModeDefault),
		ePixelOffsetMode(PixelOffsetModeNone),
#else
	// default high quality
		eTextRenderingHint(SC_DFLT_TEXTRENDERINGHINT),
		uiTextContrast(SC_DFLT_TEXTCONTRAST),
		eSmoothingMode(SC_DFLT_SMOOTHINGMODE),
		eInterpolationMode(SC_DFLT_INTERPOLATIONMODE),
		ePixelOffsetMode(SC_DFLT_PIXELOFFSETMODE),
#endif
		crPaperColor(SC_DFLT_PAPERCOLOR),
		dwInkingMode(SC_DFLT_INKINGMODE),
		bReverseVideo(FALSE),
		bPrinting(FALSE),
		fPageScale(1),
		bBkSolid(FALSE)
	{
	}
} SCGDIpDrawingAttributes, *PSCGDIpDrawingAttributes;


typedef struct tag_SCShortDCState
{
	BOOL		bMonoDC;
	DWORD		dwRop2;
	COLORREF	crTextColor;
	COLORREF	crBkColor;
	DWORD		dwBkMode;
	BOOL		bMonoBrush;

	tag_SCShortDCState():
		bMonoDC(FALSE),
		dwRop2(R2_COPYPEN),
		crTextColor(RGB(0, 0, 0)),
		crBkColor(RGB(255, 255, 255)),
		dwBkMode(OPAQUE),
		bMonoBrush(FALSE)
	{
	}

	tag_SCShortDCState(BOOL bMono,
					   DWORD dwRop2,
					   COLORREF crText,
					   COLORREF crBk,
					   DWORD dwMode):
		bMonoDC(bMono),
		dwRop2(dwRop2),
		crTextColor(crText),
		crBkColor(crBk),
		dwBkMode(dwMode),
		bMonoBrush(FALSE)
	{
	}
} SCShortDCState, *PSCShortDCState;

class SMC_INTERFACE I_SCRenderingContext
{
public:
	virtual void SCPostRender(HDC hAttribDC, HDC hOutDC, GDPGraphics* pOutGraphics) = 0;
};


class I_SCDCGraphics;

// Bitmaps
Bitmap* SCBitmapFromDIB(HGLOBAL hMem);
Bitmap* SCBitmapFromMemBmp(HBITMAP hBM);
BOOL SCSetMonochromeImagePalette(Image* pImage, COLORREF crTextColor, COLORREF crBkColor);

// Brushes
Brush* SCBrushFromLogBrush(LOGBRUSH& rLogBrush, SCShortDCState& rDCState);
BOOL SCSetMonochromeBrushPalette(Image* pImage, SCShortDCState& rDCState);
Brush* SCRealizeMonochromeBrushPalette(Brush* pBrush, SCShortDCState& rDCState);
void SCFillMonochromePalette(SCShortDCState& rDCState, PPALETTEENTRY palPalEntry);

// Pens
Pen* SCPenFromLogPen(LOGPEN& rLogPen);
Pen* SCPenFromExtLogPen(EXTLOGPEN* pExtLogPen, SCShortDCState& rDCState);

// Fonts
Font* SCFontFromLogFont(LOGFONT& rLogFont);
HFONT SCTTFontFromLOGFONT(LOGFONT& rLogFont, HDC hDC);


// Points
template <class T> 
Point* SCPointFromPOINT(T pPoints, DWORD dwCount);
template <class T> 
Point* SCPointFromPOINTTo(Point& rPtStart, T pPoints, DWORD dwCount);
REAL* SCREALFromDWORD(LPDWORD pDWords, DWORD dwCount);
BYTE* SCPathPointTypesFROMCurveTypes(LPCBYTE pGDITypes, DWORD dwCount);
void SCConvertGDIPointTypes(LPBYTE pTypes, DWORD dwCount);

// Regions
CombineMode SCClipModeFormRGNMode(INT iMode);

// Rop
void SCSysPaletteFillRect(I_SCDCGraphics* pIGraphics, Rect& destRect, UINT uiPalIndex);
INT SCStretchDIBits(I_SCDCGraphics* pIGraphics,    
  INT XDest, INT YDest, INT nDestWidth, INT nDestHeight,        
  INT XSrc, INT YSrc, INT nSrcWidth, INT nSrcHeight,         
  CONST VOID *lpBits, CONST BITMAPINFO *lpBitsInfo, UINT iUsage, DWORD dwRop);
INT SCMergeAreaWithPattern(I_SCDCGraphics* pIGraphics, Rect& destRect, DWORD dwRop);

// Rectangles
void SCNormalizedRectFromRECT(Rect* pRect, LPCRECT pRc);
void SCBuildArcAngles(LPCRECTL pBox, LPCPOINTL pPtStart, LPCPOINTL pPtEnd, INT iArcDir,
					  double& dStartAngle, double& dSweepAngle);
void SCIntersectLineAndCircle(Point& PtRes, POINTL* pPtCenter, DWORD dwRadius, double fLineAngle);
void SCIntersectLineAndEllipse(Point& PtRes, LPCRECTL pBox, double fLineAngle);

// Polygons
Point* SCConvertPolyPoly(Point* pPoints, DWORD& numPoints,
					  DWORD* pPolyVertices, DWORD& dwNbPolys);

// Extra (GDI+ to the rescue of GDI)
HENHMETAFILE SCConvertImagetoEMF(LPCTSTR lpszFname);
HENHMETAFILE SCGDIpConvertBitmaptoEMF(HBITMAP hbm, HPALETTE hpal);
HENHMETAFILE SCGDIpConvertImagetoEMF(HANDLE hMem);

BOOL SCGDIpSaveImage(HBITMAP hbm, LPCTSTR lpszPathname, INT iType, INT iJPEGQuality=100);
HENHMETAFILE SCConvertEMFtoEMFp(HENHMETAFILE hEMF,
								SCGDIpDrawingAttributes& rDrawingAttributes,
								EmfType eType);
HENHMETAFILE SCBBoxConvertEMFtoEMFp(HENHMETAFILE hEMF,
								SCGDIpDrawingAttributes& rDrawingAttributes,
								EmfType eType,
								CRect& rcDest, CRect& rcMeta,
								CRect& PaperRect, HDC hDCPlay,
								REAL iZoom, INT iAngle, I_SCRenderingContext* pIPostRender=NULL);

EmfType SCGdipGetEMFType(HENHMETAFILE hemf);
BOOL SCGdipPlayMetafile(HDC hDCPlay, HENHMETAFILE hemf, LPCRECT pRect);

// Helper subclasses
class SCColor : public Color
{
public:
	SCColor(): Color()
	{
	}
	void SCSetAlpha(BYTE bAlpha)
	{
		Argb = (Argb & ~AlphaMask)|bAlpha;
	}
};

class SCPath : public GraphicsPath
{
public:
	SCPath(FillMode fillMode = FillModeAlternate):
	  GraphicsPath(fillMode),
	  m_bEnd(FALSE)
	{
	}
	void SCSetEnd()	{ m_bEnd = TRUE; }
	BOOL SCGetEnd() { return m_bEnd; }

private:
	BOOL m_bEnd;
};

// Graphics
class SMC_INTERFACE I_SCDCGraphics
{
public:
	virtual GDPGraphics* SCGetGraphics() = 0;
	virtual HDC SCGetDC() = 0;
	virtual void SCReleaseDC(HDC hdc) = 0;
	virtual BOOL SCUsingMonochromeDC() = 0;
	virtual DWORD SCGetROP2() = 0;
	virtual COLORREF SCGetTextColor() = 0;
	virtual COLORREF SCGetBkColor() = 0;
	virtual DWORD SCGetBkMode() = 0;
	virtual Brush* SCGetBrush() = 0;

	virtual COLORREF SCGetFinalColor(COLORREF crColor) = 0;
	virtual void SCDrawBitmap(Bitmap* pBmp, Rect& rRcDest) = 0;
};


class SCFontCollection : public PrivateFontCollection
{
public:
	SCFontCollection():
	  PrivateFontCollection()
	{}

	Status SCAddFontFile(LPTSTR lpszFacename, LPTSTR lpszFilename)
	{
#ifdef _UNICODE
		return AddFontFile(lpszFilename);
#else
		WCHAR	wFaceName[LF_FACESIZE];
		MultiByteToWideChar(CP_ACP, 0, (LPSTR)lpszFacename, LF_FACESIZE, wFaceName, LF_FACESIZE);
		return AddFontFile(wFaceName);
#endif
	}

	Status SCAddMemoryFont(LPTSTR lpszFacename, LPCBYTE pData, DWORD dwSize)
	{
		return AddMemoryFont(pData, (INT)dwSize);
	}


	BOOL SCIsInstalledFacename(LPTSTR lpszFacename)
	{
		// How many font families are in the private collection?
		INT iCount = GetFamilyCount();
		if (!iCount)
			return FALSE;

#ifdef _UNICODE
		WCHAR*	wFaceName = (WCHAR*)lpszFacename;
#else
		WCHAR	wFaceName[LF_FACESIZE];
		MultiByteToWideChar(CP_ACP, 0, (LPSTR)lpszFacename, LF_FACESIZE, wFaceName, LF_FACESIZE);
#endif
		// Allocate a buffer to hold the array of FontFamily
		// objects returned by GetFamilies.
		FontFamily* pFontFamily = new FontFamily[iCount];
		
		// Get the array of FontFamily objects.
		INT iNbRet;
		GetFamilies(iCount, pFontFamily, &iNbRet);
		ASSERT(iNbRet==iCount);
		
		// Lookup our facename
		BOOL bFound = FALSE;
		WCHAR	wFamilyName[LF_FACESIZE];
		for(INT j = 0; (j < iCount); ++j)
		{
			pFontFamily[j].GetFamilyName(wFamilyName);
			if (0==wcsncmp(wFamilyName, wFaceName, LF_FACESIZE))
			{
				bFound = TRUE;
				break;
			}
		}
		delete [] pFontFamily;
		return bFound;
	}

protected:

private:
};

// Aggregation helpers
class SCBrush
{
public:
	SCBrush(HBRUSH hBrush, BOOL bMono):
	  m_hBrush(hBrush),
	  m_bMonoPattern(bMono)
	{
	}

	~SCBrush()
	{
	}
	HBRUSH SCGetHandle() { return m_hBrush; }
	BOOL SCIsMonoPattern() { return m_bMonoPattern; }

protected:
	HBRUSH		m_hBrush;
	BOOL		m_bMonoPattern;
};

class SCFont : public Font
{
public:
    SCFont(HDC hdc, const LOGFONT* pLogFont):
		Font(hdc, pLogFont),
		m_hSubsFont(NULL),
		m_iAngle(0),
		m_iHeight(0),
		m_iAscent(0),
		m_iDescent(0),
		m_dwSimulStatus(0),
		m_iInterExternal(0)
	{
		ASSERT(hdc);
		ASSERT(pLogFont);
	}

	SCFont(WCHAR* wszFaceName, REAL fEmSize, FontStyle eStyle, FontCollection* pFontCollection):
		Font(wszFaceName, fEmSize, eStyle, UnitPixel, pFontCollection),
		m_hSubsFont(NULL),
		m_iAngle(0),
		m_iHeight(0),
		m_iAscent(0),
		m_iDescent(0),
		m_dwSimulStatus(0),
		m_iInterExternal(0)
	{
		ASSERT(wszFaceName);
		ASSERT(pFontCollection);
	}

	void SCFinalInit(HDC hdc, int iAngle, DWORD dwSimul, HFONT hSubsFont)
	{
		TEXTMETRIC tm;
		GetTextMetrics(hdc, &tm);
		m_iHeight = tm.tmHeight;
		m_iAscent = tm.tmAscent;
		m_iDescent = tm.tmDescent;
		m_iInterExternal = tm.tmInternalLeading + tm.tmExternalLeading;

		m_iAngle = iAngle;
		m_dwSimulStatus = dwSimul;
		m_hSubsFont = hSubsFont;
	}

	~SCFont()
	{
		if (m_hSubsFont)
			DeleteObject(m_hSubsFont); // Note: should not be in any DC
	}

// Not used: to keep things simple
//	    SCFont(HDC hdc):
//			Font(hdc),
//			m_hSubsFont(NULL),
//			m_iAngle(0),
//			m_iHeight(0),
//			m_iAscent(0),
//			m_iDescent(0),
//			m_dwSimulStatus(0)
//		{
//			ASSERT(hdc);
//		}
//	    SCFont(HDC hdc, const HFONT hFont):
//			Font(hdc, hFont),
//			m_hSubsFont(NULL),
//			m_iAngle(0),
//			m_iHeight(0),
//			m_iAscent(0),
//			m_iDescent(0),
//			m_dwSimulStatus(0)
//		{
//			ASSERT(hdc);
//			ASSERT(hFont);
//		}

// Accessors
	// getters
	int SCGetAngle() { return m_iAngle; }
	int SCGetHeight() { return m_iHeight; }
	int SCGetAscent() { return m_iAscent; }
	int SCGetDescent() { return m_iDescent; }
	int SCGetInterExternal() { return m_iInterExternal;}
	DWORD SCGetSimulStatus() { return m_dwSimulStatus; }

	// setters

protected:
	HFONT m_hSubsFont;		// substitute font
	int m_iAngle;
	int m_iHeight;
	int m_iAscent;
	int m_iDescent;
	int m_iInterExternal;	// is likely to be removed
	DWORD m_dwSimulStatus;

//prohibited calls
private:
	SCFont();	// prevent creation on the stack
	/*No default =*/void operator=(const SCFont &rCopy);
	/*No default copy*/SCFont(const SCFont &rCopy);
};


#endif //_SCGDIPLUSUTILS_H_
//  ------------------------------------------------------------
