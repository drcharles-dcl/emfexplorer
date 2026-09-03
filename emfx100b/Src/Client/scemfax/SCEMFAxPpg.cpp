/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "SCEMFAx.h"
#include "SCEMFAxPpg.h"

#include "SCGenInclude.h"
#include SC_INC_SHARED(SCPropStrLists.h)
#include SC_INC_EMFLIB(SCEMFViewDefs.h)
#include SC_INC_EMFLIB(SCGdiplusUtils.h)
#include SC_INC_SHARED(SCZoomShrd.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CSCEMFAxPropPage, CSCBaseOlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CSCEMFAxPropPage, CSCBaseOlePropertyPage)
	//{{AFX_MSG_MAP(CSCEMFAxPropPage)
	ON_BN_CLICKED(IDC_RADIO_CTLCOLOR_TRANS, OnRadioCtlColorTrans)
	ON_BN_CLICKED(IDC_RADIO_CTLCOLOR_SYS, OnRadioCtlColorSys)
	ON_BN_CLICKED(IDC_RADIO_CTLCOLOR_RGB, OnRadioCtlColorRgb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CSCEMFAxPropPage, "SCEMFAX.SCEMFAxPropPage.1",
	0xbf226e49, 0x5e76, 0x412b, 0xba, 0xcb, 0x8f, 0xac, 0x44, 0xb3, 0x22, 0x5)


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxPropPage::CSCEMFAxPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CSCEMFAxPropPage

BOOL CSCEMFAxPropPage::CSCEMFAxPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_SCEMFAX_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxPropPage::CSCEMFAxPropPage - Constructor

CSCEMFAxPropPage::CSCEMFAxPropPage() :
	CSCBaseOlePropertyPage(IDD, IDS_SCEMFAX_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CSCEMFAxPropPage)
	m_bPageBorderVisible = FALSE;
	m_bPageShadowVisible = FALSE;
	m_nPageOrientation = -1;
	m_nMarginL = 0;
	m_nMarginT = 0;
	m_nMarginR = 0;
	m_nMarginB = 0;
	m_nCtlColorStyle = -1;
	//}}AFX_DATA_INIT
	m_nCtlColor = RGB(255, 255, 255);
	m_fScale = 1.0;
	m_nFitMode = SC_FIT_PAGE;
}

SC_IMPLEMENT_ENABLE_CONTROL(CSCEMFAxPropPage)

/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxPropPage::DoDataExchange - Moves data between page and properties

void CSCEMFAxPropPage::DoDataExchange(CDataExchange* pDX)
{
	// Note: m_nPageOrientation, m_nCtlColorStyle are cookies
	// representing the selected items (buttons), not actual property values.
	// The can be -1 (no selection).
	SCDECL_PROP_COOKIE(m_nPageOrientation, PageOrientation);
	SCDECL_PROP_COOKIE(m_nCtlColorStyle, CtlColorStyle);
	SCDECL_DWORD_PROP(m_nCtlColor, CtlColor);

	//{{AFX_DATA_MAP(CSCEMFAxPropPage)
	DDX_Control(pDX, IDC_COMBO_CTL_BORDER, m_CmbCtlBorder);
	DDX_Control(pDX, IDC_COMBO_OPTGEN_ZOOM, m_CmbZoomBox);
	DDX_Control(pDX, IDC_COMBO_CTLCOLOR_RGB, m_CmbCtlColorRGB);
	DDX_Control(pDX, IDC_COMBO_CTLCOLOR_SYS, m_CmbCtlColorSys);
	DDP_Check(pDX, IDC_CHECK_PICTURE_BORDER, m_bPageBorderVisible, _T("PageBorderVisible") );
	DDX_Check(pDX, IDC_CHECK_PICTURE_BORDER, m_bPageBorderVisible);
	DDP_Check(pDX, IDC_CHECK_PICTURE_SHADOW, m_bPageShadowVisible, _T("PageShadowVisible") );
	DDX_Check(pDX, IDC_CHECK_PICTURE_SHADOW, m_bPageShadowVisible);
	DDX_Radio(pDX, IDC_RADIO_ORIENT0, m_nPageOrientation);
	DDP_Text(pDX, IDC_EDIT_MARGINS_L, m_nMarginL, _T("MarginL") );
	DDX_Text(pDX, IDC_EDIT_MARGINS_L, m_nMarginL);
	DDP_Text(pDX, IDC_EDIT_MARGINS_T, m_nMarginT, _T("MarginT") );
	DDX_Text(pDX, IDC_EDIT_MARGINS_T, m_nMarginT);
	DDP_Text(pDX, IDC_EDIT_MARGINS_R, m_nMarginR, _T("MarginR") );
	DDX_Text(pDX, IDC_EDIT_MARGINS_R, m_nMarginR);
	DDP_Text(pDX, IDC_EDIT_MARGINS_B, m_nMarginB, _T("MarginB") );
	DDX_Text(pDX, IDC_EDIT_MARGINS_B, m_nMarginB);
	DDX_Radio(pDX, IDC_RADIO_CTLCOLOR_TRANS, m_nCtlColorStyle);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);

	// Special post processing (for properties not listed in afx data map)
	if (pDX->m_bSaveAndValidate)
	{
		SCDDP_COOKIE_SETPROP(m_nPageOrientation, PageOrientation);
		SCDDP_COOKIE_SETPROP(m_nCtlColorStyle, CtlColorStyle);

		// Control border
		DWORD dwIdx = m_CmbCtlBorder.GetCurSel();
		if (CB_ERR!=dwIdx)
		{
			DWORD dwCtlBorder;
			SCGetCtlBorderPredefinedValue(dwCtlBorder, dwIdx);
			SCSetDWORDProp(dwCtlBorder, SC_T_CtlBorder);
		}

		// Color value from combos
		switch (m_nCtlColorStyle)
		{
		case SC_COLOR_TRANSPARENT:
			break;

		case SC_COLOR_SYSINDEX:
			m_nCtlColor = (OLE_COLOR)m_CmbCtlColorSys.SCGetCurSelColor();
			SCDDP_SETPROP(m_nCtlColor, CtlColor);
			break;

		case SC_COLOR_RGBVALUE:
			m_nCtlColor = (OLE_COLOR)m_CmbCtlColorRGB.SCGetCurSelColor();
			SCDDP_SETPROP(m_nCtlColor, CtlColor);
			break;
		}

		// Scaling
		int iZoom = m_CmbZoomBox.SCGetCurValue();
		if (SC_ZOOM_FITWITDH==iZoom)
		{
			m_nFitMode = SC_FIT_WIDTH;
		} else
		if (SC_ZOOM_FITPAGE==iZoom)
		{
			m_nFitMode = SC_FIT_PAGE;
		}else
		{
			m_nFitMode = SC_FIT_NONE;
			m_fScale = float(iZoom)/float(SC_ZOOM100);
		}
		SCSetDWORDProp(m_nFitMode, SC_T_FitMode);
		SCSetFloatProp(m_fScale, SC_T_Scale);
	} else
	{
		SCDDP_COOKIE_GETPROP(m_nPageOrientation, PageOrientation);
		SCDDP_COOKIE_GETPROP(m_nCtlColorStyle, CtlColorStyle);

		// Control border
		DWORD dwIdx, dwCtlBorder;
		SCGetDWORDProp(&dwCtlBorder, SC_T_CtlBorder);
		SCGetCtlBorderPredefinedCookie(dwIdx, dwCtlBorder);
		m_CmbCtlBorder.SetCurSel(dwIdx);

		// Color value to Combos
		switch (m_nCtlColorStyle)
		{
		case SC_COLOR_TRANSPARENT:
			break;

		case SC_COLOR_SYSINDEX:
			SCDDP_GETPROP(m_nCtlColor, CtlColor);
			m_CmbCtlColorSys.SCSetCurSelColor(m_nCtlColor);
			break;

		case SC_COLOR_RGBVALUE:
			SCDDP_GETPROP(m_nCtlColor, CtlColor);
			m_CmbCtlColorRGB.SCSetCurSelColor(m_nCtlColor);
			break;
		}

		// Scaling
		SCGetDWORDProp(&m_nFitMode, SC_T_FitMode);
		SCGetFloatProp(&m_fScale, SC_T_Scale);
		switch (m_nFitMode)
		{
		case SC_FIT_PAGE:
			m_CmbZoomBox.SCSelectValue(SC_ZOOM_FITPAGE);
			break;

		case SC_FIT_WIDTH:
			m_CmbZoomBox.SCSelectValue(SC_ZOOM_FITWITDH);
			break;

		default:
			m_CmbZoomBox.SCSetCurValue((int)(m_fScale*SC_ZOOM100));
		}
	}
	SCEnableControls();
}


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxPropPage message handlers

BOOL CSCEMFAxPropPage::OnInitDialog() 
{
	CSCBaseOlePropertyPage::OnInitDialog();
	
	// Add extra initialization here
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
		hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);

	SCFillCtlBorderCombo(&m_CmbCtlBorder);

	m_CmbZoomBox.SCSetFloatMultiplier(SC_FLOATFACTOR);
	m_CmbZoomBox.SCSetMinMax(SC_MIN_ZOOM, SC_MAX_ZOOM);
	m_CmbZoomBox.SCSetDefaultList((SCZoomEntry*)sc_ZoomList, cs_usZoomCount);

	m_CmbCtlColorRGB.SendMessage(WM_SETFONT, (WPARAM)hFont);
	m_CmbCtlColorRGB.InitColorsComboBox(RGB(255, 255, 255));
	m_CmbCtlColorSys.SendMessage(WM_SETFONT, (WPARAM)hFont);
	m_CmbCtlColorSys.InitColorsComboBox(SC_MAKE_SYSCOLOR(COLOR_WINDOW));
	
	SCEnableControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSCEMFAxPropPage::SCEnableControls()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_RADIO_CTLCOLOR_SYS);
	SCEnableControl(IDC_COMBO_CTLCOLOR_SYS, (pBtn && 1==pBtn->GetCheck()));

	pBtn = (CButton*)GetDlgItem(IDC_RADIO_CTLCOLOR_RGB);
	SCEnableControl(IDC_COMBO_CTLCOLOR_RGB, (pBtn && 1==pBtn->GetCheck()));
}

void CSCEMFAxPropPage::OnRadioCtlColorTrans() 
{
	SCEnableControls();
}

void CSCEMFAxPropPage::OnRadioCtlColorSys() 
{
	SCEnableControls();
}

void CSCEMFAxPropPage::OnRadioCtlColorRgb() 
{
	SCEnableControls();
}

/////////////////////////////////////////////////////////////////////////////
///
/// Attempt to force sending VK_RETURN to editboxes with the ES_WANTRETURN sytle.
///
BOOL CSCEMFAxPropPage::PreTranslateMessage(MSG* pMsg) 
{
	// Add your specialized code here and/or call the base class
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
        HWND hFocus = ::GetFocus();
#if 1
/// Our code to detect the zoom combobox, and force it to eat
		HWND hParent = ::GetParent(hFocus);
		if (hParent && ::GetDlgCtrlID(hParent)==IDC_COMBO_OPTGEN_ZOOM)
		{
			m_CmbZoomBox.SCEatValue();
			return FALSE;    // Don't translate
		}
#else
		/// Won't work: PreTranslateMessage won't be called on our combobox.
///	(c) Microsoft
       static const TCHAR szEdit[] = _T("edit");
        TCHAR szCompare[sizeof(szEdit)+1];
       // Check for "edit" controls which want the RETURN key.
        // If it doesn't want the return key then let it be
        // translated by just calling
        // CPropertyPage::PreTranslateMessage
        ::GetClassName(hFocus, szCompare, sizeof(szCompare));
        if (lstrcmpi(szCompare, szEdit) == 0)
		{
            if(::GetWindowLong(hFocus, GWL_STYLE) & ES_WANTRETURN)
			{
                return FALSE;    // Don't translate...just send to control
            } else
                return CSCBaseOlePropertyPage::PreTranslateMessage(pMsg);
		}
		
        // Check for other controls which want the return key
        if (::SendMessage(hFocus,WM_GETDLGCODE,0,0) &
			(DLGC_WANTALLKEYS | DLGC_WANTMESSAGE))
            return FALSE;        // Don't translate it
#endif
	};
	
	return CSCBaseOlePropertyPage::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
