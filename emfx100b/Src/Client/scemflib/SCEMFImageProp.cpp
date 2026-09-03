/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SC_CHECK_REFRESH() \
	if (bRefresh) \
		SCRefresh()

#define SC_CHECK_REFRESH_SIZE() \
	SCComputeFitZoom(); \
	if (bRefresh) \
		SCRefresh()

#define SC_REVERSEVIDEO_ERROR(rAttributes) \
	(rAttributes.bReverseVideo && ((rAttributes.dwInkingMode & SC_REVERSE_VIDEO_MASK)==SC_REVERSE_VIDEO_NONE))

//=== Getters =======================================================
//--- properties ------------------------------------------------
long CSCEMFImage::SCGetCtlBorderType() const
{
	if (IsWindow(m_hWnd))
	{
		if (GetExStyle() & (WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME))
			return SC_CSM_CTLBORDER_RAISED;
		if (GetStyle() & WS_BORDER)
			return SC_CSM_CTLBORDER_SIMPLE;
	}
	return SC_CSM_CTLBORDER_NONE;
}

long CSCEMFImage::SCGetCtlColorStyle() const
{
	switch (m_dwColorScheme & SC_CSM_CTLCOLOR_MASK)
	{
	case SC_CSM_CTLCOLOR_TRANSPARENT: return SC_COLOR_TRANSPARENT;
	case SC_CSM_CTLCOLOR_SYSINDEX: return SC_COLOR_SYSINDEX;
	case SC_CSM_CTLCOLOR_RGBVALUE: return SC_COLOR_RGBVALUE;
	default:
		ASSERT(0);
	}
	return SC_COLOR_RGBVALUE;
}

long CSCEMFImage::SCGetPaperColorStyle() const
{
	switch (m_dwColorScheme & SC_CSM_PAPERCOLOR_MASK)
	{
	case SC_CSM_PAPERCOLOR_TRANSPARENT: return SC_COLOR_TRANSPARENT;
	case SC_CSM_PAPERCOLOR_SYSINDEX: return SC_COLOR_SYSINDEX;
	case SC_CSM_PAPERCOLOR_RGBVALUE: return SC_COLOR_RGBVALUE;
	default:
		ASSERT(0);
	}
	return SC_COLOR_RGBVALUE;
}


//=== Setters =======================================================
//--- properties ------------------------------------------------

void CSCEMFImage::SCSetCtlBorder(DWORD dwBorderType)
{
	if (!IsWindow(m_hWnd))
		return;

	LockWindowUpdate();

	switch (dwBorderType & SC_CSM_CTLBORDER_MASK)
	{
	case SC_CSM_CTLBORDER_NONE:
		if (GetStyle() & WS_BORDER)
			ModifyStyle(WS_BORDER, 0, SWP_DRAWFRAME|SWP_SHOWWINDOW);
		if (GetExStyle() & (WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME))
			ModifyStyleEx(WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME, 0, SWP_DRAWFRAME|SWP_SHOWWINDOW);
		break;
		
	case SC_CSM_CTLBORDER_SIMPLE:
		if (GetExStyle() & (WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME))
			ModifyStyleEx(WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME, 0, 0);
		ModifyStyle(0, WS_BORDER, SWP_DRAWFRAME|SWP_SHOWWINDOW);
		break;

	case SC_CSM_CTLBORDER_RAISED:
		if (GetStyle() & WS_BORDER)
			ModifyStyle(WS_BORDER, 0, 0);
		ModifyStyleEx(0, WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME, SWP_DRAWFRAME|SWP_SHOWWINDOW);
		break;

	default:
		ASSERT(0);
	}

	UnlockWindowUpdate();
}

void CSCEMFImage::SCSetCtlColorStyle(DWORD dwColorStyle, COLORREF crColor, BOOL bRefresh/*=TRUE*/)
{
	m_crCtlColor = crColor;
	m_dwColorScheme &= ~SC_CSM_CTLCOLOR_MASK;
	switch (dwColorStyle)
	{
	case SC_COLOR_TRANSPARENT:
		m_dwColorScheme |= SC_CSM_CTLCOLOR_TRANSPARENT;
		// always transform ambient color to RGB
		//m_crCtlColor = (pcrColor) ? *pcrColor : GetSysColor(COLOR_WINDOW);
		if (SC_IS_SYSCOLOR(m_crCtlColor))
			m_crCtlColor = GetSysColor(SC_SYSCOLOR_INDEX(m_crCtlColor));
		break;

	case SC_COLOR_SYSINDEX:
		m_dwColorScheme |= SC_CSM_CTLCOLOR_SYSINDEX;
		//m_crCtlColor = (pcrColor) ? *pcrColor : SC_MAKE_SYSCOLOR(COLOR_WINDOW);
		break;

	case SC_COLOR_RGBVALUE:
		m_dwColorScheme |= SC_CSM_CTLCOLOR_RGBVALUE;
		//m_crCtlColor = (pcrColor) ? *pcrColor : GetSysColor(COLOR_WINDOW);
		break;

	default:
		ASSERT(0);
		m_dwColorScheme |= SC_CSM_CTLCOLOR_RGBVALUE;
		//m_crCtlColor = GetSysColor(COLOR_WINDOW);
		break;
	}
	SC_CHECK_REFRESH();
}

void CSCEMFImage::SCSetCtlColor(COLORREF crColor, BOOL bRefresh/*=TRUE*/)
{
	m_crCtlColor = crColor;
	SC_CHECK_REFRESH();
}

//Zzzz!...
//	BOOL CSCEMFImage::SetCtlColor(COLORREF crColor, BOOL bRefresh/*=TRUE*/) 
//	{
//		// Filter color
//		switch (m_dwColorScheme & SC_CSM_CTLCOLOR_MASK)
//		{
//		case SC_CSM_CTLCOLOR_TRANSPARENT:
//			return FALSE; // reject color
//	
//		case SC_CSM_CTLCOLOR_SYSINDEX:
//			if (!SC_IS_SYSCOLOR(crColor))
//				return FALSE; // reject color (we need 0x800000ii => system index)
//			break;
//	
//		case SC_CSM_CTLCOLOR_RGBVALUE:
//			if (!SC_IS_RGBCOLOR(crColor))
//				return FALSE; // reject color
//			break;
//	
//		default:
//			ASSERT(0);
//			return FALSE; // bad state
//		}
//	
//		m_crCtlColor = crColor;
//		SC_CHECK_REFRESH();
//		return TRUE;
//	}


void CSCEMFImage::SCSetPageBorderVisible(BOOL bVisible, BOOL bRefresh/*=TRUE*/)
{
	m_dwColorScheme &= ~SC_CSM_BORDER_MASK;
	m_dwColorScheme |= (bVisible) ? SC_CSM_BORDER_YES : SC_CSM_BORDER_NONE;
	SC_CHECK_REFRESH();
}

void CSCEMFImage::SCSetPageShadowVisible(BOOL bVisible, BOOL bRefresh/*=TRUE*/)
{
	m_dwColorScheme &= ~SC_CSM_SHADOW_MASK;
	m_dwColorScheme |= (bVisible) ? SC_CSM_SHADOW_YES : SC_CSM_SHADOW_NONE;
	SC_CHECK_REFRESH();
}

void CSCEMFImage::SCSetPaperColorStyle(DWORD dwColorStyle, COLORREF crColor, BOOL bRefresh/*=TRUE*/)
{
	m_crPaperColor = crColor;
	m_dwColorScheme &= ~SC_CSM_PAPERCOLOR_MASK;
	switch (dwColorStyle)
	{
	case SC_COLOR_TRANSPARENT:
		m_dwColorScheme |= SC_CSM_PAPERCOLOR_TRANSPARENT;
		m_crPaperColor = SCGetTranslatedCtlColor();
		break;
		
	case SC_COLOR_SYSINDEX:
		m_dwColorScheme |= SC_CSM_PAPERCOLOR_SYSINDEX;
		break;
		
	case SC_COLOR_RGBVALUE:
		m_dwColorScheme |= SC_CSM_PAPERCOLOR_RGBVALUE;
		break;

	default:
		ASSERT(0);
		m_dwColorScheme |= SC_CSM_PAPERCOLOR_RGBVALUE;
		m_crPaperColor = GetSysColor(COLOR_WINDOW);
		break;
	}
	m_DrawingAttributes.crPaperColor = SCGetTranslatedPaperColor();
	SCUpdatePageInfoColors();
	SC_CHECK_REFRESH();
}

void CSCEMFImage::SCSetPaperColor(COLORREF crColor, BOOL bRefresh/*=TRUE*/)
{
	m_crPaperColor = crColor;
	m_DrawingAttributes.crPaperColor = SCGetTranslatedPaperColor();
	SCUpdatePageInfoColors();
	SC_CHECK_REFRESH();
}

//Zzzz!...
//	BOOL CSCEMFImage::SCSetPaperColor(COLORREF crColor, BOOL bRefresh/*=TRUE*/)
//	{
//		// Filter color
//		switch (m_dwColorScheme & SC_CSM_PAPERCOLOR_MASK)
//		{
//		case SC_CSM_PAPERCOLOR_TRANSPARENT:
//			return FALSE; // reject color
//			
//		case SC_CSM_PAPERCOLOR_SYSINDEX:
//			if (!SC_IS_SYSCOLOR(crColor))
//				return FALSE; // reject color (we need 0x800000ii => system index)
//			break;
//			
//		case SC_CSM_PAPERCOLOR_RGBVALUE:
//			if (!SC_IS_RGBCOLOR(crColor))
//				return FALSE; // reject color (we need 0x00bbggrr => RGB)
//			break;
//	
//		default:
//			ASSERT(0);
//			return FALSE; // bad state
//		}
//		m_crPaperColor = crColor;
//		m_DrawingAttributes.crPaperColor = SCGetTranslatedPaperColor();
//		SC_CHECK_REFRESH();
//		return TRUE;
//	}

void CSCEMFImage::SCSetFitMode(int iFit, BOOL bRefresh/*=TRUE*/)
{
	ASSERT(iFit>=SC_FIT_NONE && iFit<=SC_FIT_PAGE);

	m_iFitMode = iFit;
	if (SC_FIT_NONE==m_iFitMode)
	{
		m_iCurZoom = SC_ZOOM100;
	} else
		SCComputeFitZoom();
	SC_CHECK_REFRESH();
}

void CSCEMFImage::SCSetCurZoom(int iZoom, BOOL bRefresh/*=TRUE*/)
{
	ASSERT(iZoom>=SC_MIN_ZOOM && iZoom<=SC_MAX_ZOOM);

	m_iCurZoom = iZoom;
	m_iFitMode = SC_FIT_NONE;
	SC_CHECK_REFRESH();
}
// bis
void CSCEMFImage::SCSetScale(float fScale, BOOL bRefresh/*=TRUE*/)
{
	int iZoom = (int)(fScale*SC_ZOOM100);
	if (iZoom<SC_MIN_ZOOM)
		iZoom = SC_MIN_ZOOM;
	else
	if (iZoom>SC_MAX_ZOOM)
		iZoom = SC_MAX_ZOOM;

	m_iCurZoom = iZoom;
	m_iFitMode = SC_FIT_NONE;
	SC_CHECK_REFRESH();
}


void CSCEMFImage::SCSetLMargin(long lCx, BOOL bRefresh/*=TRUE*/)
{
	m_rectMargins.left = lCx;
	SC_CHECK_REFRESH_SIZE();
}

void CSCEMFImage::SCSetTMargin(long lCy, BOOL bRefresh/*=TRUE*/)
{
	m_rectMargins.top = lCy;
	SC_CHECK_REFRESH_SIZE();
}

void CSCEMFImage::SCSetRMargin(long lCx, BOOL bRefresh/*=TRUE*/)
{
	m_rectMargins.right = lCx;
	SC_CHECK_REFRESH_SIZE();
}

void CSCEMFImage::SCSetBMargin(long lCy, BOOL bRefresh/*=TRUE*/)
{
	m_rectMargins.bottom = lCy;
	SC_CHECK_REFRESH_SIZE();
}

void CSCEMFImage::SCSetMargins(CRect& rc, BOOL bRefresh/*=TRUE*/)
{
	m_rectMargins = rc;
	SC_CHECK_REFRESH_SIZE();
}

void CSCEMFImage::SCSetAngle(int iAngle, BOOL bRefresh/*=TRUE*/) // PageOrientation 
{
	m_iAngle = iAngle % 360;
	if (m_iAngle<0)
		m_iAngle += 360;
	ASSERT(m_iAngle>=0);
	SC_CHECK_REFRESH_SIZE();
}

void CSCEMFImage::SCSetTransparencyMode(DWORD dwMode, BOOL bRefresh/*=TRUE*/)
{
	m_DrawingAttributes.dwInkingMode &= ~SC_TRANSPARENCY_MASK; 
	m_DrawingAttributes.dwInkingMode |= (dwMode & SC_TRANSPARENCY_MASK);
	SC_CHECK_REFRESH();
}

void CSCEMFImage::SCSetReverseVideoMode(DWORD dwMode, BOOL bRefresh/*=TRUE*/)
{
	m_bReverseVideo = ((dwMode & SC_REVERSE_VIDEO_MASK)!=SC_REVERSE_VIDEO_NONE);
	m_DrawingAttributes.dwInkingMode &= ~SC_REVERSE_VIDEO_MASK;
	m_DrawingAttributes.dwInkingMode |= (dwMode & SC_REVERSE_VIDEO_MASK);
	m_DrawingAttributes.bReverseVideo = (SC_REVERSE_VIDEO_FULL==dwMode) ? FALSE : m_bReverseVideo;
	ASSERT(!SC_REVERSEVIDEO_ERROR(m_DrawingAttributes));

	SCUpdatePageInfoColors();
	SC_CHECK_REFRESH();
}


void CSCEMFImage::SCEnableGDIp(BOOL bEnable/*=TRUE*/, BOOL bRefresh/*=TRUE*/)
{
	m_uiRasEngine = (bEnable) ? SC_ENGINE_GDIP : SC_ENGINE_GDI;
	SC_CHECK_REFRESH();
}

void CSCEMFImage::SCSetTextRenderingHint(int iHint, BOOL bRefresh/*=TRUE*/)
{
	ASSERT(iHint>=SC_MIN_TEXTRENDERINGHINT && iHint<=SC_MAX_TEXTRENDERINGHINT);

	m_DrawingAttributes.eTextRenderingHint = (TextRenderingHint)iHint;
	SC_CHECK_REFRESH();
}

void CSCEMFImage::SCSetTextContrast(UINT uiContrast, BOOL bRefresh/*=TRUE*/)
{
	ASSERT(uiContrast>=SC_MIN_TEXTCONTRAST && uiContrast<=SC_MAX_TEXTCONTRAST);

	m_DrawingAttributes.uiTextContrast = uiContrast;
	SC_CHECK_REFRESH();
}

void CSCEMFImage::SCSetSmoothingMode(int iSmoothing, BOOL bRefresh/*=TRUE*/)
{
	ASSERT(iSmoothing>=SC_MIN_SMOOTHINGMODE && iSmoothing<=SC_MAX_SMOOTHINGMODE);

	m_DrawingAttributes.eSmoothingMode = (SmoothingMode)iSmoothing;
	SC_CHECK_REFRESH();
}

void CSCEMFImage::SCSetInterpolationMode(int iInterpolation, BOOL bRefresh/*=TRUE*/)
{
	ASSERT(iInterpolation>=SC_MIN_INTERPOLATIONMODE && iInterpolation<=SC_MAX_INTERPOLATIONMODE);

	m_DrawingAttributes.eInterpolationMode = (InterpolationMode)iInterpolation;
	SC_CHECK_REFRESH();
}

void CSCEMFImage::SCSetPixelOffsetMode(int iPixelOffset, BOOL bRefresh/*=TRUE*/)
{
	ASSERT(iPixelOffset>=SC_MIN_PIXELOFFSETMODE && iPixelOffset<=SC_MAX_PIXELOFFSETMODE);

	m_DrawingAttributes.ePixelOffsetMode = (PixelOffsetMode)iPixelOffset;
	SC_CHECK_REFRESH();
}
//--- extra -----------------------------------------------------

void CSCEMFImage::SCUpdatePageInfoColors()
{
	if (m_iCurPage)
	{
		PSCEMFDocPage pDocPage = m_pEMFDoc->SCGetDocPage(m_iCurPage-1);
		ASSERT(pDocPage);
		pDocPage->SCUpdatePageInfoColors(m_bReverseVideo);
	}
}

// dup
void CSCEMFImage::SCSetReverseVideo(BOOL bReverseVideo, BOOL bRefresh/*=TRUE*/) 
{
	SCSetReverseVideoMode((bReverseVideo)?
								SC_REVERSE_VIDEO_FULL :
								SC_REVERSE_VIDEO_NONE, bRefresh);
}

void CSCEMFImage::SCSetDrawingAttributes(SCGDIpDrawingAttributes& rDrawingAttributes, BOOL bRefresh/*=TRUE*/)
{
	m_DrawingAttributes = rDrawingAttributes;

	// adjustments
	m_bReverseVideo = m_DrawingAttributes.bReverseVideo;
	if (SC_RVIDEOFULL(m_DrawingAttributes))
	{ // CSCEMFImage has a better state of the destination DC and rectangle,
	  // so it can do the full reverse video simply and more accurately.
		m_DrawingAttributes.bReverseVideo = FALSE;
	}

	ASSERT(!SC_REVERSEVIDEO_ERROR(m_DrawingAttributes));
	SC_CHECK_REFRESH();
}

// Good for transparent colors/border/shadow settings
void CSCEMFImage::SCSetColorScheme(DWORD dwColorScheme, BOOL bRefresh/*=TRUE*/)
{
	m_dwColorScheme = dwColorScheme;
	// m_crCtlColor = ?? may be set after
	// m_crPaperColor = ?? may be set after
	SC_CHECK_REFRESH();
}

// Good for setting the full color scheme
void CSCEMFImage::SCSetColorSchemeEx(DWORD dwColorScheme,
								   COLORREF* pcrCtlColor/*=NULL*/,
								   COLORREF* pcrPaperColor/*=NULL*/,
								   BOOL bRefresh/*=TRUE*/)
{
	m_dwColorScheme = dwColorScheme;

	if (pcrCtlColor)
		m_crCtlColor = *pcrCtlColor;

	if (pcrPaperColor)
	{
		m_crPaperColor = *pcrPaperColor;
		m_DrawingAttributes.crPaperColor = SCGetTranslatedPaperColor();
	}

	SC_CHECK_REFRESH();
}

void CSCEMFImage::SCSetRasEngine(UINT uiEngine, BOOL bRefresh/*=TRUE*/)
{
	m_uiRasEngine = uiEngine;
	SC_CHECK_REFRESH();
}


