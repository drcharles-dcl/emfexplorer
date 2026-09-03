/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "SCEMFDoc.h"

#include "SCGdiplusUtils.h"
#include "SCEMFViewDefs.h"
#include "SCGenInclude.h"
#include SC_INC_WINLIB(SCRegistry.h)
#include SC_INC_SHARED(SCPropStrLists.h)
#include SC_INC_WINLIB(SCWinFile.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SC_SETDOC_PROP(member, value) \
	m_Properties.##member = value; \
	m_bDocModified = TRUE

/////////////////////////////////////////////////////////////////////
// Document Properties
//

// Getters
// CString SCGetPicture(); // N/A: call SCGetUniDocName			
	// Control-related
int SCEMFDoc::SCGetCtlBorder() const { return m_Properties.usCtlBorder; }
int SCEMFDoc::SCGetCtlColorStyle() const { return m_Properties.usCtlColorStyle; }
int SCEMFDoc::SCGetCtlColor() const { return m_Properties.crCtlColor; }
	// Page-related
int SCEMFDoc::SCGetPageBorderVisible() const { return m_Properties.bPageBorderVisible; }
int SCEMFDoc::SCGetPageShadowVisible() const { return m_Properties.bPageShadowVisible; }
int SCEMFDoc::SCGetPaperColorStyle() const { return m_Properties.usPaperColorStyle; }
int SCEMFDoc::SCGetPaperColor() const { return m_Properties.crPaperColor; }
int SCEMFDoc::SCGetFitMode() const { return m_Properties.usFitMode; }	
int SCEMFDoc::SCGetScale() const { return m_Properties.iScale; }	
int SCEMFDoc::SCGetMarginL() const { return m_Properties.lMarginL; }
int SCEMFDoc::SCGetMarginT() const { return m_Properties.lMarginT; }
int SCEMFDoc::SCGetMarginR() const { return m_Properties.lMarginR; }
int SCEMFDoc::SCGetMarginB() const { return m_Properties.lMarginB; }
int SCEMFDoc::SCGetPageOrientation() const { return m_Properties.usPageOrientation; }
int SCEMFDoc::SCGetTransparencyMode() const { return m_Properties.usTransparencyMode; }
int SCEMFDoc::SCGetReverseVideoMode() const { return m_Properties.usReverseVideoMode; }
	// GDI+-related
int SCEMFDoc::SCGetGDIplusEnabled() const { return m_Properties.bEnableGDIp; }
int SCEMFDoc::SCGetTextRenderingHint() const { return m_Properties.usTextRenderingHint; }
int SCEMFDoc::SCGetTextContrast() const { return m_Properties.usTextContrast; }
int SCEMFDoc::SCGetSmoothingMode() const { return m_Properties.usSmoothingMode; }
int SCEMFDoc::SCGetInterpolationMode() const { return m_Properties.usInterpolationMode; }
int SCEMFDoc::SCGetPixelOffsetMode() const { return m_Properties.usPixelOffsetMode; }


// Setters
//	void SCSetPicture(LPCTSTR lpszPath); // N/A: call SCSetUniDocName	
	// Control-related
void SCEMFDoc::SCSetCtlBorder(int iBorderType) { SC_SETDOC_PROP(usCtlBorder, iBorderType); }
void SCEMFDoc::SCSetCtlColorStyle(int iStyle) { SC_SETDOC_PROP(usCtlColorStyle, iStyle); }
void SCEMFDoc::SCSetCtlColor(int iColor) { SC_SETDOC_PROP(crCtlColor, iColor); }
	// Page-related
void SCEMFDoc::SCSetPageBorderVisible(int bVisible) { SC_SETDOC_PROP(bPageBorderVisible, bVisible); }
void SCEMFDoc::SCSetPageShadowVisible(int bVisible) { SC_SETDOC_PROP(bPageShadowVisible, bVisible); }
void SCEMFDoc::SCSetPaperColorStyle(int iStyle) { SC_SETDOC_PROP(usPaperColorStyle, iStyle); }
void SCEMFDoc::SCSetPaperColor(int iColor) { SC_SETDOC_PROP(crPaperColor, iColor); }
void SCEMFDoc::SCSetFitMode(int iMode) { SC_SETDOC_PROP(usFitMode, iMode); }
void SCEMFDoc::SCSetScale(int iScale) { SC_SETDOC_PROP(iScale, iScale); }
void SCEMFDoc::SCSetMarginL(int iLeft) { SC_SETDOC_PROP(lMarginL, iLeft); }
void SCEMFDoc::SCSetMarginT(int iTop) { SC_SETDOC_PROP(lMarginT, iTop); }
void SCEMFDoc::SCSetMarginR(int iRight) { SC_SETDOC_PROP(lMarginR, iRight); }
void SCEMFDoc::SCSetMarginB(int iBottom) { SC_SETDOC_PROP(lMarginB, iBottom); }
void SCEMFDoc::SCSetPageOrientation(int iAngle) { SC_SETDOC_PROP(usPageOrientation, iAngle); }
void SCEMFDoc::SCSetTransparencyMode(int iMode) { SC_SETDOC_PROP(usTransparencyMode, iMode); }
void SCEMFDoc::SCSetReverseVideoMode(int iMode) { SC_SETDOC_PROP(usReverseVideoMode, iMode); }
	// GDI+-related
void SCEMFDoc::SCSetGDIplusEnabled(int bEnabled) { SC_SETDOC_PROP(bEnableGDIp, bEnabled); }
void SCEMFDoc::SCSetTextRenderingHint(int iHint) { SC_SETDOC_PROP(usTextRenderingHint, iHint); }
void SCEMFDoc::SCSetTextContrast(int iLevel) { SC_SETDOC_PROP(usTextContrast, iLevel); }
void SCEMFDoc::SCSetSmoothingMode(int iMode) { SC_SETDOC_PROP(usSmoothingMode, iMode); }
void SCEMFDoc::SCSetInterpolationMode(int iMode) { SC_SETDOC_PROP(usInterpolationMode, iMode); }
void SCEMFDoc::SCSetPixelOffsetMode(int iMode) { SC_SETDOC_PROP(usPixelOffsetMode, iMode); }


/////////////////////////////////////////////////////////////////////
// Save/Load Properties
//
// Note: using cheap save/load in this implementation.
// Implementation may change in the future.

BOOL SCEMFDoc::SCHasSettings()
{
	if (m_strUniDocName.IsEmpty() ||
		!SCExistFile(m_strUniDocName))
		return FALSE;

	// check one ore more keys in settings section
	if (-1!=GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_CtlBorder, -1, m_strUniDocName))
		return TRUE;

	return FALSE;
}

void SCEMFDoc::SCLoadSettings()
{
	//-- Document --
	{
		TCHAR szTxtBuff[SC_PGCOMMENT_MAXSIZE];
		
		::GetPrivateProfileString(SC_DOC_GEN_SECTION, SC_DOC_URL_KEY, _T(""),
			szTxtBuff, SC_PGCOMMENT_MAXSIZE, m_strUniDocName);
		m_Properties.strCreditURL = szTxtBuff;
		
		::GetPrivateProfileString(SC_DOC_GEN_SECTION, SC_DOC_CREDIT_KEY, _T(""),
			szTxtBuff, SC_PGCOMMENT_MAXSIZE, m_strUniDocName);
		m_Properties.strCredit = szTxtBuff;
	}

	//-- Pages --
	// Control-related
	m_Properties.usCtlBorder = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_CtlBorder, SC_DFLT_CTLBORDER, m_strUniDocName);
	m_Properties.usCtlColorStyle = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_CtlColorStyle, SC_DFLT_CTLCOLORSTYLE, m_strUniDocName);
	m_Properties.crCtlColor = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_CtlColor, SC_DFLT_CTLCOLOR, m_strUniDocName);

	// Page-related
	m_Properties.bPageBorderVisible = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_PageBorderVisible, SC_DFLT_PAGEBORDERVISIBLE, m_strUniDocName);
	m_Properties.bPageShadowVisible = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_PageShadowVisible, SC_DFLT_PAGESHADOWVISIBLE, m_strUniDocName);
	m_Properties.usPaperColorStyle = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_PaperColorStyle, SC_DFLT_PAPERCOLORSTYLE, m_strUniDocName);
	m_Properties.crPaperColor = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_PaperColor, SC_DFLT_PAPERCOLOR, m_strUniDocName);
	m_Properties.usFitMode = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_FitMode, SC_DFLT_FITMODE, m_strUniDocName);
	m_Properties.iScale = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_Scale, SC_DFLT_SCALE, m_strUniDocName);
	m_Properties.lMarginL = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_MarginL, SC_DFLT_MARGINL, m_strUniDocName);
	m_Properties.lMarginT = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_MarginT, SC_DFLT_MARGINT, m_strUniDocName);
	m_Properties.lMarginR = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_MarginR, SC_DFLT_MARGINR, m_strUniDocName);
	m_Properties.lMarginB = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_MarginB, SC_DFLT_MARGINB, m_strUniDocName);
	m_Properties.usPageOrientation = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_PageOrientation, SC_DFLT_PAGEORIENTATION, m_strUniDocName);
	m_Properties.usTransparencyMode = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_TransparencyMode, SC_DFLT_TRANSPARENCYMODE, m_strUniDocName);
	m_Properties.usReverseVideoMode = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_ReverseVideoMode, SC_DFLT_REVERSEVIDEOMODE, m_strUniDocName);

	// GDI+-related
	m_Properties.bEnableGDIp = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_GDIplusEnabled, SC_DFLT_GDIPLUSENABLED, m_strUniDocName);
	m_Properties.usTextRenderingHint = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_TextRenderingHint, SC_DFLT_TEXTRENDERINGHINT, m_strUniDocName);
	m_Properties.usTextContrast = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_TextContrast, SC_DFLT_TEXTCONTRAST, m_strUniDocName);
	m_Properties.usSmoothingMode = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_SmoothingMode, SC_DFLT_SMOOTHINGMODE, m_strUniDocName);
	m_Properties.usInterpolationMode = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_InterpolationMode, SC_DFLT_INTERPOLATIONMODE, m_strUniDocName);
	m_Properties.usPixelOffsetMode = GetPrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_PixelOffsetMode, SC_DFLT_PIXELOFFSETMODE, m_strUniDocName);
}

void SCEMFDoc::SCSaveSettings(LPCTSTR lpszFName)
{
	//-- Document --
	::WritePrivateProfileString(SC_DOC_GEN_SECTION, SC_DOC_URL_KEY, m_Properties.strCreditURL, lpszFName);
	::WritePrivateProfileString(SC_DOC_GEN_SECTION, SC_DOC_CREDIT_KEY, m_Properties.strCredit, lpszFName);

	//-- Pages --
	// Control-related
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_CtlBorder, m_Properties.usCtlBorder, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_CtlColorStyle, m_Properties.usCtlColorStyle, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_CtlColor, m_Properties.crCtlColor, lpszFName);

	// Page-related
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_PageBorderVisible, m_Properties.bPageBorderVisible, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_PageShadowVisible, m_Properties.bPageShadowVisible, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_PaperColorStyle, m_Properties.usPaperColorStyle, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_PaperColor, m_Properties.crPaperColor, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_FitMode, m_Properties.usFitMode, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_Scale, m_Properties.iScale, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_MarginL, m_Properties.lMarginL, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_MarginT, m_Properties.lMarginT, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_MarginR, m_Properties.lMarginR, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_MarginB, m_Properties.lMarginB, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_PageOrientation, m_Properties.usPageOrientation, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_TransparencyMode, m_Properties.usTransparencyMode, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_ReverseVideoMode, m_Properties.usReverseVideoMode, lpszFName);

	// GDI+-related
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_GDIplusEnabled, m_Properties.bEnableGDIp, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_TextRenderingHint, m_Properties.usTextRenderingHint, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_TextContrast, m_Properties.usTextContrast, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_SmoothingMode, m_Properties.usSmoothingMode, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_InterpolationMode, m_Properties.usInterpolationMode, lpszFName);
	SCWritePrivateProfileInt(SC_DOC_SETTINGS_SECTION, SC_T_PixelOffsetMode, m_Properties.usPixelOffsetMode, lpszFName);
}
//////////////////////////////////////////

