/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "EMFexplorer.h"
#include "EMFexplorerDoc.h"
#include "EMFexplorerView.h"
#include "SCEMFDlgOptions.h"

#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCWinFile.h)
#include SC_INC_WINLIB(SCRegistry.h)
#include SC_INC_SHARED(SCPropStrLists.h)
#include SC_INC_EMFLIB(SCEMFViewHelpers.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Applying / Saving settings
//


// Default or last settings
void CEMFexplorerView::SCReadSettings(LPCTSTR lpszSubpath/*=NULL*/)
{
	CString strSection;
	if (lpszSubpath)
		strSection.Format(_T("%s\\%s"), s_szAppSection, lpszSubpath);
	else
	{
		strSection = s_szAppSection;
	}

	CWinApp* pApp = AfxGetApp();

	// Control border
	m_CtlImage.SCSetCtlBorder(pApp->GetProfileInt(strSection, SC_T_CtlBorder, SC_DFLT_CTLBORDER));
	// Control color style and color
	m_CtlImage.SCSetCtlColorStyle(
		pApp->GetProfileInt(strSection, SC_T_CtlColorStyle, SC_DFLT_CTLCOLORSTYLE),
		pApp->GetProfileInt(strSection, SC_T_CtlColor, SC_DFLT_CTLCOLOR), FALSE);

	// Paper border and shadow
	m_CtlImage.SCSetPageBorderVisible(pApp->GetProfileInt(strSection, SC_T_PageBorderVisible, SC_DFLT_PAGEBORDERVISIBLE), FALSE);
	m_CtlImage.SCSetPageShadowVisible(pApp->GetProfileInt(strSection, SC_T_PageShadowVisible, SC_DFLT_PAGESHADOWVISIBLE), FALSE);

	// Paper color style and color
	m_CtlImage.SCSetPaperColorStyle(
		pApp->GetProfileInt(strSection, SC_T_PaperColorStyle, SC_DFLT_PAPERCOLORSTYLE),
		pApp->GetProfileInt(strSection, SC_T_PaperColor, SC_DFLT_PAPERCOLOR), FALSE);

	// Fitmode and Scale
	int iFitMode = pApp->GetProfileInt(strSection, SC_T_FitMode, SC_DFLT_FITMODE);
	m_CtlImage.SCSetFitMode(iFitMode, FALSE);
	if (SC_FIT_NONE==iFitMode)
	{
		int iCurZoom = pApp->GetProfileInt(strSection, SC_T_Scale, SC_DFLT_SCALE);
		m_CtlImage.SCSetCurZoom(iCurZoom, FALSE);
	}

	// Margins
	CRect rcMargins(pApp->GetProfileInt(strSection, SC_T_MarginL, SC_DFLT_MARGINL),
					pApp->GetProfileInt(strSection, SC_T_MarginT, SC_DFLT_MARGINT),
					pApp->GetProfileInt(strSection, SC_T_MarginR, SC_DFLT_MARGINR),
					pApp->GetProfileInt(strSection, SC_T_MarginB, SC_DFLT_MARGINB));
	m_CtlImage.SCSetMargins(rcMargins, FALSE);

	// Orientation
	m_CtlImage.SCSetAngle(pApp->GetProfileInt(strSection, SC_T_PageOrientation, SC_DFLT_PAGEORIENTATION), FALSE);

	// Transparency mode
	m_CtlImage.SCSetTransparencyMode(pApp->GetProfileInt(strSection, SC_T_TransparencyMode, SC_DFLT_TRANSPARENCYMODE), FALSE);
	// Reverse video mode
	m_CtlImage.SCSetReverseVideoMode(pApp->GetProfileInt(strSection, SC_T_ReverseVideoMode, SC_DFLT_REVERSEVIDEOMODE), FALSE);

	// GDI+
	m_CtlImage.SCEnableGDIp(pApp->GetProfileInt(strSection, SC_T_GDIplusEnabled, SC_DFLT_GDIPLUSENABLED), FALSE);
	m_CtlImage.SCSetTextRenderingHint(pApp->GetProfileInt(strSection, SC_T_TextRenderingHint, SC_DFLT_TEXTRENDERINGHINT), FALSE);
	m_CtlImage.SCSetTextContrast(pApp->GetProfileInt(strSection, SC_T_TextContrast, SC_DFLT_TEXTCONTRAST), FALSE);
	m_CtlImage.SCSetSmoothingMode(pApp->GetProfileInt(strSection, SC_T_SmoothingMode, SC_DFLT_SMOOTHINGMODE), FALSE);
	m_CtlImage.SCSetInterpolationMode(pApp->GetProfileInt(strSection, SC_T_InterpolationMode, SC_DFLT_INTERPOLATIONMODE), FALSE);
	m_CtlImage.SCSetPixelOffsetMode(pApp->GetProfileInt(strSection, SC_T_PixelOffsetMode, SC_DFLT_PIXELOFFSETMODE), FALSE);

	// Refresh
	m_CtlImage.SCRefresh();
}

// Settings from the current doc
BOOL CEMFexplorerView::SCReadDocSettings()
{
	if (!m_pEMFDoc || !m_pEMFDoc->SCHasSettings())
		return FALSE;

	// Control border
	m_CtlImage.SCSetCtlBorder(m_pEMFDoc->SCGetCtlBorder());
	// Control color style and color
	m_CtlImage.SCSetCtlColorStyle(m_pEMFDoc->SCGetCtlColorStyle(),
								  m_pEMFDoc->SCGetCtlColor(), FALSE);

	// Paper border and shadow
	m_CtlImage.SCSetPageBorderVisible(m_pEMFDoc->SCGetPageBorderVisible(), FALSE);
	m_CtlImage.SCSetPageShadowVisible(m_pEMFDoc->SCGetPageShadowVisible(), FALSE);

	// Paper color style and color
	m_CtlImage.SCSetPaperColorStyle(m_pEMFDoc->SCGetPaperColorStyle(),
									m_pEMFDoc->SCGetPaperColor(), FALSE);

	// Fitmode and Scale
	int iFitMode = m_pEMFDoc->SCGetFitMode();
	m_CtlImage.SCSetFitMode(iFitMode, FALSE);
	if (SC_FIT_NONE==iFitMode)
	{
		int iCurZoom = m_pEMFDoc->SCGetScale();
		m_CtlImage.SCSetCurZoom(iCurZoom, FALSE);
	}

	// Margins
	CRect rcMargins(m_pEMFDoc->SCGetMarginL(),
					m_pEMFDoc->SCGetMarginT(),
					m_pEMFDoc->SCGetMarginR(),
					m_pEMFDoc->SCGetMarginB());
	m_CtlImage.SCSetMargins(rcMargins, FALSE);

	// Orientation
	m_CtlImage.SCSetAngle(m_pEMFDoc->SCGetPageOrientation(), FALSE);

	// Transparency mode
	m_CtlImage.SCSetTransparencyMode(m_pEMFDoc->SCGetTransparencyMode(), FALSE);
	// Reverse video mode
	m_CtlImage.SCSetReverseVideoMode(m_pEMFDoc->SCGetReverseVideoMode(), FALSE);

	// GDI+
	m_CtlImage.SCEnableGDIp(m_pEMFDoc->SCGetGDIplusEnabled(), FALSE);
	m_CtlImage.SCSetTextRenderingHint(m_pEMFDoc->SCGetTextRenderingHint(), FALSE);
	m_CtlImage.SCSetTextContrast(m_pEMFDoc->SCGetTextContrast(), FALSE);
	m_CtlImage.SCSetSmoothingMode(m_pEMFDoc->SCGetSmoothingMode(), FALSE);
	m_CtlImage.SCSetInterpolationMode(m_pEMFDoc->SCGetInterpolationMode(), FALSE);
	m_CtlImage.SCSetPixelOffsetMode(m_pEMFDoc->SCGetPixelOffsetMode(), FALSE);

	// Refresh
	m_CtlImage.SCRefresh();
	return TRUE;
}

// Save settings in app section
void CEMFexplorerView::SCSaveLastSettings()
{
	CWinApp* pApp = AfxGetApp();

	CString strSection;
	strSection.Format(_T("%s\\Last"), s_szAppSection);

	// Control
	pApp->WriteProfileInt(strSection, SC_T_CtlBorder, m_CtlImage.SCGetCtlBorderType());
	pApp->WriteProfileInt(strSection, SC_T_CtlColorStyle, m_CtlImage.SCGetCtlColorStyle());
	pApp->WriteProfileInt(strSection, SC_T_CtlColor, (int)m_CtlImage.SCGetCtlColor());

	// Scale, fit mode
	pApp->WriteProfileInt(strSection, SC_T_FitMode, m_CtlImage.SCGetFitMode());
	pApp->WriteProfileInt(strSection, SC_T_Scale, m_CtlImage.SCGetCurZoom());

	// Margins
	CRect rcMargins;
	m_CtlImage.SCGetMargins(rcMargins);
	pApp->WriteProfileInt(strSection, SC_T_MarginL, rcMargins.left);
	pApp->WriteProfileInt(strSection, SC_T_MarginT, rcMargins.top);
	pApp->WriteProfileInt(strSection, SC_T_MarginR, rcMargins.right);
	pApp->WriteProfileInt(strSection, SC_T_MarginB, rcMargins.bottom);

	// Page border
	pApp->WriteProfileInt(strSection, SC_T_PageBorderVisible, m_CtlImage.SCGetPageBorderVisible());
	pApp->WriteProfileInt(strSection, SC_T_PageShadowVisible, m_CtlImage.SCGetPageShadowVisible());
	// Orientation	
	pApp->WriteProfileInt(strSection, SC_T_PageOrientation, m_CtlImage.SCGetCurAngle());
	// Paper color
	pApp->WriteProfileInt(strSection, SC_T_PaperColorStyle, m_CtlImage.SCGetPaperColorStyle());
	pApp->WriteProfileInt(strSection, SC_T_PaperColor, m_CtlImage.SCGetPaperColor());
	// Transparency, reverse video	
	pApp->WriteProfileInt(strSection, SC_T_TransparencyMode, m_CtlImage.SCGetTransparencyMode());
	pApp->WriteProfileInt(strSection, SC_T_ReverseVideoMode, m_CtlImage.SCGetReverseVideoMode());

	// GDI+
	pApp->WriteProfileInt(strSection, SC_T_GDIplusEnabled, (int)m_CtlImage.SCGetGDIplusEnabled());
	pApp->WriteProfileInt(strSection, SC_T_TextRenderingHint, (int)m_CtlImage.SCGetTextRenderingHint());
	pApp->WriteProfileInt(strSection, SC_T_TextContrast, (int)m_CtlImage.SCGetTextContrast());
	pApp->WriteProfileInt(strSection, SC_T_SmoothingMode, (int)m_CtlImage.SCGetSmoothingMode());
	pApp->WriteProfileInt(strSection, SC_T_InterpolationMode, (int)m_CtlImage.SCGetInterpolationMode());
	pApp->WriteProfileInt(strSection, SC_T_PixelOffsetMode, (int)m_CtlImage.SCGetPixelOffsetMode());
}

// Transfer control settings to document
void CEMFexplorerView::SCSaveDocSettings()
{
	ASSERT(m_pEMFDoc);
	if (!m_pEMFDoc)
		return;

	// Control
	m_pEMFDoc->SCSetCtlBorder(m_CtlImage.SCGetCtlBorderType());
	m_pEMFDoc->SCSetCtlColorStyle(m_CtlImage.SCGetCtlColorStyle());
	m_pEMFDoc->SCSetCtlColor((int)m_CtlImage.SCGetCtlColor());

	// Scale, fit mode
	m_pEMFDoc->SCSetFitMode(m_CtlImage.SCGetFitMode());
	m_pEMFDoc->SCSetScale(m_CtlImage.SCGetCurZoom());

	// Margins
	CRect rcMargins;
	m_CtlImage.SCGetMargins(rcMargins);
	m_pEMFDoc->SCSetMarginL(rcMargins.left);
	m_pEMFDoc->SCSetMarginT(rcMargins.top);
	m_pEMFDoc->SCSetMarginR(rcMargins.right);
	m_pEMFDoc->SCSetMarginB(rcMargins.bottom);

	// Page border
	m_pEMFDoc->SCSetPageBorderVisible(m_CtlImage.SCGetPageBorderVisible());
	m_pEMFDoc->SCSetPageShadowVisible(m_CtlImage.SCGetPageShadowVisible());
	// Orientation	
	m_pEMFDoc->SCSetPageOrientation(m_CtlImage.SCGetCurAngle());
	// Paper color
	m_pEMFDoc->SCSetPaperColorStyle(m_CtlImage.SCGetPaperColorStyle());
	m_pEMFDoc->SCSetPaperColor(m_CtlImage.SCGetPaperColor());
	// Transparency, reverse video	
	m_pEMFDoc->SCSetTransparencyMode(m_CtlImage.SCGetTransparencyMode());
	m_pEMFDoc->SCSetReverseVideoMode(m_CtlImage.SCGetReverseVideoMode());

	// GDI+
	m_pEMFDoc->SCSetGDIplusEnabled((int)m_CtlImage.SCGetGDIplusEnabled());
	m_pEMFDoc->SCSetTextRenderingHint((int)m_CtlImage.SCGetTextRenderingHint());
	m_pEMFDoc->SCSetTextContrast((int)m_CtlImage.SCGetTextContrast());
	m_pEMFDoc->SCSetSmoothingMode((int)m_CtlImage.SCGetSmoothingMode());
	m_pEMFDoc->SCSetInterpolationMode((int)m_CtlImage.SCGetInterpolationMode());
	m_pEMFDoc->SCSetPixelOffsetMode((int)m_CtlImage.SCGetPixelOffsetMode());
}

/////////////////////////////////////////////////////////////////////////////
// I_SCPropExchange
//
// Editing settings
//

#define SC_RETURN_DWORDPROP(prop, dflt) \
ASSERT(eSettingsDefault==(enumSettingsPage)iSender); \
return (DWORD)AfxGetApp()->GetProfileInt(s_szAppSection, prop, (int)dflt)

#define SC_STORE_DWORDPROP(prop, iValue) \
ASSERT(eSettingsDefault==(enumSettingsPage)iSender); \
AfxGetApp()->WriteProfileInt(s_szAppSection, prop, (int)iValue)


// Filter
BOOL CEMFexplorerView::SCWantSettings(int iSender)
{
	ASSERT(eSettingsBadID!=(enumSettingsPage)iSender);
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		return (m_pEMFDoc && m_pEMFDoc->SCGetNbPages()!=0);
	}
	return (eSettingsDefault==(enumSettingsPage)iSender);
}

// Getters
DWORD CEMFexplorerView::SCGetSettings_CtlBorder(int iSender)
{
	ASSERT(eSettingsDefault==(enumSettingsPage)iSender || eSettingsDoc==(enumSettingsPage)iSender);
	if (eSettingsDoc==(enumSettingsPage)iSender) 
		return (DWORD)m_CtlImage.SCGetCtlBorderType();

	SC_RETURN_DWORDPROP(SC_T_CtlBorder, 0);
}

void CEMFexplorerView::SCGetSettings_CtlColor(int iSender, int& iStyle, COLORREF& crColor)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		iStyle = m_CtlImage.SCGetCtlColorStyle();
		crColor = m_CtlImage.SCGetCtlColor();
	} else
	{
		ASSERT(eSettingsDefault==(enumSettingsPage)iSender);
		
		CWinApp* pApp = AfxGetApp();
		iStyle = pApp->GetProfileInt(s_szAppSection, SC_T_CtlColorStyle, (int)SC_COLOR_SYSINDEX);
		crColor = (COLORREF)pApp->GetProfileInt(s_szAppSection, SC_T_CtlColor, (int)COLOR_WINDOW);
	}
}

void CEMFexplorerView::SCGetSettings_PageBorder(int iSender, BOOL& bBorder, BOOL& bShadow)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		bBorder = m_CtlImage.SCGetPageBorderVisible();
		bShadow = m_CtlImage.SCGetPageShadowVisible();
	} else
	{
		ASSERT(eSettingsDefault==(enumSettingsPage)iSender);

		CWinApp* pApp = AfxGetApp();
		bBorder = (BOOL)pApp->GetProfileInt(s_szAppSection, SC_T_PageBorderVisible, FALSE);
		bShadow = (BOOL)pApp->GetProfileInt(s_szAppSection, SC_T_PageShadowVisible, FALSE);
	}
}

void CEMFexplorerView::SCGetSettings_PaperColor(int iSender, int& iStyle, COLORREF& crColor)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		iStyle = m_CtlImage.SCGetPaperColorStyle();
		crColor = m_CtlImage.SCGetPaperColor();
	} else
	{
		ASSERT(eSettingsDefault==(enumSettingsPage)iSender);

		CWinApp* pApp = AfxGetApp();
		iStyle = pApp->GetProfileInt(s_szAppSection, SC_T_PaperColorStyle, (int)SC_COLOR_SYSINDEX);
		crColor = (COLORREF)pApp->GetProfileInt(s_szAppSection, SC_T_PaperColor, (int)COLOR_WINDOW);
	}
}

int CEMFexplorerView::SCGetSettings_Scale(int iSender)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		switch (m_CtlImage.SCGetFitMode())
		{
		case SC_FIT_WIDTH: return SC_ZOOM_FITWITDH;
		case SC_FIT_PAGE: return SC_ZOOM_FITPAGE;
		}

		return m_CtlImage.SCGetCurZoom();
	}
	
	ASSERT(eSettingsDefault==(enumSettingsPage)iSender);
	
	CWinApp* pApp = AfxGetApp();
	switch (pApp->GetProfileInt(s_szAppSection, SC_T_FitMode, SC_FIT_WIDTH))
	{
	case SC_FIT_WIDTH: return SC_ZOOM_FITWITDH;
	case SC_FIT_PAGE: return SC_ZOOM_FITPAGE;
	}
	return pApp->GetProfileInt(s_szAppSection, SC_T_Scale, SC_ZOOM100);
}

void CEMFexplorerView::SCGetSettings_Margins(int iSender, RECT& rcLTRB)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		CRect rcMargins;
		m_CtlImage.SCGetMargins(rcMargins);
		CopyRect(&rcLTRB, &rcMargins);
	} else
	{
		ASSERT(eSettingsDefault==(enumSettingsPage)iSender);

		CWinApp* pApp = AfxGetApp();
		SetRect(&rcLTRB,
			pApp->GetProfileInt(s_szAppSection, SC_T_MarginL, 0),
			pApp->GetProfileInt(s_szAppSection, SC_T_MarginT, 0),
			pApp->GetProfileInt(s_szAppSection, SC_T_MarginR, 0),
			pApp->GetProfileInt(s_szAppSection, SC_T_MarginB, 0));
	}
}

DWORD CEMFexplorerView::SCGetSettings_PageOrientation(int iSender)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
		return m_CtlImage.SCGetCurAngle();
	SC_RETURN_DWORDPROP(SC_T_PageOrientation, 0);
}

DWORD CEMFexplorerView::SCGetSettings_TransparencyMode(int iSender)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
		return m_CtlImage.SCGetTransparencyMode();
	SC_RETURN_DWORDPROP(SC_T_TransparencyMode, 0);
}

DWORD CEMFexplorerView::SCGetSettings_ReverseVideoMode(int iSender)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
		return m_CtlImage.SCGetReverseVideoMode();
	SC_RETURN_DWORDPROP(SC_T_ReverseVideoMode, 0);
}

DWORD CEMFexplorerView::SCGetSettings_GDIplusEnabled(int iSender)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
		return m_CtlImage.SCGetGDIplusEnabled();
	SC_RETURN_DWORDPROP(SC_T_GDIplusEnabled, TRUE);
}

void CEMFexplorerView::SCGetSettings_TextRendering(int iSender, int& iHint, int& iContrast)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		iHint = m_CtlImage.SCGetTextRenderingHint();
		iContrast = m_CtlImage.SCGetTextContrast();
	} else
	{
		ASSERT(eSettingsDefault==(enumSettingsPage)iSender);

		CWinApp* pApp = AfxGetApp();
		iHint = pApp->GetProfileInt(s_szAppSection, SC_T_TextRenderingHint, TextRenderingHintAntiAlias);
		iContrast = pApp->GetProfileInt(s_szAppSection, SC_T_TextContrast, SC_DFLT_TEXTCONTRAST);
	}
}

void CEMFexplorerView::SCGetSettings_Smoothing(int iSender,
											   int& iLineSmoothing,
											   int& iImgInterpolation,
											   int& iImgPixelOffset)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		iLineSmoothing = m_CtlImage.SCGetSmoothingMode();
		iImgInterpolation = m_CtlImage.SCGetInterpolationMode();
		iImgPixelOffset = m_CtlImage.SCGetPixelOffsetMode();
	} else
	{
		ASSERT(eSettingsDefault==(enumSettingsPage)iSender);
		
		CWinApp* pApp = AfxGetApp();
		iLineSmoothing = pApp->GetProfileInt(s_szAppSection, SC_T_SmoothingMode, SmoothingModeHighQuality);
		iImgInterpolation = pApp->GetProfileInt(s_szAppSection, SC_T_InterpolationMode, InterpolationModeHighQualityBicubic);
		iImgPixelOffset = pApp->GetProfileInt(s_szAppSection, SC_T_PixelOffsetMode, PixelOffsetModeHighQuality);
	}
}

// Setters
void CEMFexplorerView::SCPutSettings_CtlBorder(int iSender, int iBorder)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		m_CtlImage.SCSetCtlBorder((DWORD)iBorder);
	} else
	{
		SC_STORE_DWORDPROP(SC_T_CtlBorder, iBorder);
	}
}

void CEMFexplorerView::SCPutSettings_CtlColor(int iSender, int iStyle, COLORREF crColor)
{
	if (eSettingsDoc==(enumSettingsPage)iSender) 
	{
		m_CtlImage.SCSetCtlColorStyle(iStyle, crColor, FALSE);
	} else
	{
		ASSERT(eSettingsDefault==(enumSettingsPage)iSender);

		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(s_szAppSection, SC_T_CtlColorStyle, iStyle);
		pApp->WriteProfileInt(s_szAppSection, SC_T_CtlColor, (int)crColor);
	}
}

void CEMFexplorerView::SCPutSettings_PageBorder(int iSender, BOOL bBorder, BOOL bShadow)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		m_CtlImage.SCSetPageBorderVisible(bBorder, FALSE);
		m_CtlImage.SCSetPageShadowVisible(bShadow, FALSE);
	}
	else
	{
		ASSERT(eSettingsDefault==(enumSettingsPage)iSender);

		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(s_szAppSection, SC_T_PageBorderVisible, bBorder);
		pApp->WriteProfileInt(s_szAppSection, SC_T_PageShadowVisible, bShadow);
	}
}

void CEMFexplorerView::SCPutSettings_PaperColor(int iSender, int iStyle, COLORREF crColor)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		m_CtlImage.SCSetPaperColorStyle(iStyle, crColor, FALSE);
	}
	else
	{
		ASSERT(eSettingsDefault==(enumSettingsPage)iSender);

		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(s_szAppSection, SC_T_PaperColorStyle, iStyle);
		pApp->WriteProfileInt(s_szAppSection, SC_T_PaperColor, crColor);
	}
}

void CEMFexplorerView::SCPutSettings_Scale(int iSender, int iScale)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		if (SC_ZOOM_FITWITDH==iScale)
			m_CtlImage.SCSetFitMode(SC_FIT_WIDTH, FALSE);
		else
		if (SC_ZOOM_FITPAGE==iScale)
			m_CtlImage.SCSetFitMode(SC_FIT_PAGE, FALSE);
		else
			m_CtlImage.SCSetCurZoom(iScale, FALSE);
	} else
	{
		ASSERT(eSettingsDefault==(enumSettingsPage)iSender);

		int iFitMode = SC_FIT_NONE;
		if (SC_ZOOM_FITWITDH==iScale)
			iFitMode=SC_FIT_WIDTH;
		else
		if (SC_ZOOM_FITPAGE==iScale)
			iFitMode=SC_FIT_PAGE;

		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(s_szAppSection, SC_T_FitMode, iFitMode);
		pApp->WriteProfileInt(s_szAppSection, SC_T_Scale, iScale);
	}
}

void CEMFexplorerView::SCPutSettings_Margins(int iSender, RECT& rcLTRB)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		CRect rcMargins(&rcLTRB);
		m_CtlImage.SCSetMargins(rcMargins, FALSE);
	}
	else
	{
		ASSERT(eSettingsDefault==(enumSettingsPage)iSender);

		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(s_szAppSection, SC_T_MarginL, rcLTRB.left);
		pApp->WriteProfileInt(s_szAppSection, SC_T_MarginT, rcLTRB.top);
		pApp->WriteProfileInt(s_szAppSection, SC_T_MarginR, rcLTRB.right);
		pApp->WriteProfileInt(s_szAppSection, SC_T_MarginB, rcLTRB.bottom);
	}
}

void CEMFexplorerView::SCPutSettings_PageOrientation(int iSender, int iOrientation)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		m_CtlImage.SCSetAngle(iOrientation, FALSE);
	} else
	{
		SC_STORE_DWORDPROP(SC_T_PageOrientation, iOrientation);
	}
}

void CEMFexplorerView::SCPutSettings_TransparencyMode(int iSender, int iMode)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		m_CtlImage.SCSetTransparencyMode(iMode, FALSE);
	} else
	{
		SC_STORE_DWORDPROP(SC_T_TransparencyMode, iMode);
	}
}

void CEMFexplorerView::SCPutSettings_ReverseVideoMode(int iSender, int iMode)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		m_CtlImage.SCSetReverseVideoMode(iMode, FALSE);
	} else
	{
		SC_STORE_DWORDPROP(SC_T_ReverseVideoMode, iMode);
	}
}

void CEMFexplorerView::SCPutSettings_GDIplusEnabled(int iSender, BOOL bEnabled)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		m_CtlImage.SCEnableGDIp(bEnabled, FALSE);
	} else
	{
		SC_STORE_DWORDPROP(SC_T_GDIplusEnabled, bEnabled);
	}
}

void CEMFexplorerView::SCPutSettings_TextRendering(int iSender, int iHint, int iContrast)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		m_CtlImage.SCSetTextRenderingHint(iHint, FALSE);
		m_CtlImage.SCSetTextContrast(iContrast, FALSE);
	}
	else
	{
		ASSERT(eSettingsDefault==(enumSettingsPage)iSender);

		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(s_szAppSection, SC_T_TextRenderingHint, iHint);
		pApp->WriteProfileInt(s_szAppSection, SC_T_TextContrast, iContrast);
	}
}

void CEMFexplorerView::SCPutSettings_Smoothing(int iSender, int iLineSmoothing,
											   int iImgInterpolation, int iImgPixelOffset)
{
	if (eSettingsDoc==(enumSettingsPage)iSender)
	{
		m_CtlImage.SCSetSmoothingMode(iLineSmoothing, FALSE);
		m_CtlImage.SCSetInterpolationMode(iImgInterpolation, FALSE);
		m_CtlImage.SCSetPixelOffsetMode(iImgPixelOffset, FALSE);
	}
	else
	{
		ASSERT(eSettingsDefault==(enumSettingsPage)iSender);

		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(s_szAppSection, SC_T_SmoothingMode, iLineSmoothing);
		pApp->WriteProfileInt(s_szAppSection, SC_T_InterpolationMode, iImgInterpolation);
		pApp->WriteProfileInt(s_szAppSection, SC_T_PixelOffsetMode, iImgPixelOffset);
	}
}


