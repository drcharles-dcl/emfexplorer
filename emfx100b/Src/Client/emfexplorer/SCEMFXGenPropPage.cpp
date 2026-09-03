/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "emfexplorer.h"
#include "SCEMFXGenPropPage.h"

#include "SCGenInclude.h"
#include SC_INC_EMFLIB(SCEMFViewDefs.h)
#include SC_INC_EMFLIB(SCGdiplusUtils.h)
#include SC_INC_SHARED(SCZoomShrd.h)
#include SC_INC_SHARED(SCPropStrLists.h)
#include SC_INC_WINLIB(SCPropHelpers.h)


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCEMFXGenPropPage property page

IMPLEMENT_DYNCREATE(CSCEMFXGenPropPage, CSCEMFXBasePropPage)

CSCEMFXGenPropPage::CSCEMFXGenPropPage():
	CSCEMFXBasePropPage(CSCEMFXGenPropPage::IDD, IDS_PPG_GENERAL)
{
	//{{AFX_DATA_INIT(CSCEMFXGenPropPage)
	m_bPageBorderVisible = FALSE;
	m_bPageShadowVisible = FALSE;
	m_nPageOrientation = -1;
	m_nMarginL = 0;
	m_nMarginT = 0;
	m_nMarginR = 0;
	m_nMarginB = 0;
	m_nCtlColorStyle = -1;
	//}}AFX_DATA_INIT
	m_crCtlColor = RGB(255, 255, 255);
}

CSCEMFXGenPropPage::~CSCEMFXGenPropPage()
{
}

void CSCEMFXGenPropPage::DoDataExchange(CDataExchange* pDX)
{
	// Note: m_nPageOrientation, m_nCtlColorStyle are cookies
	// representing the selected items (buttons), not actual property values.
	// The can be -1 (no selection).
//		SCPPG_COOKIE_DECL(m_nPageOrientation, PageOrientation);
//		SCPPG_COOKIE_DECL(m_nCtlColorStyle, CtlColorStyle);
//		SCPPG_DWPROP_DECL(m_crCtlColor, CtlColor);

	CSCEMFXBasePropPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CSCEMFXGenPropPage)
	DDX_Control(pDX, IDC_COMBO_CTL_BORDER, m_CmbCtlBorder);
	DDX_Control(pDX, IDC_COMBO_OPTGEN_ZOOM, m_CmbZoomBox);
	DDX_Control(pDX, IDC_COMBO_CTLCOLOR_RGB, m_CmbCtlColorRGB);
	DDX_Control(pDX, IDC_COMBO_CTLCOLOR_SYS, m_CmbCtlColorSys);
	DDX_Check(pDX, IDC_CHECK_PICTURE_BORDER, m_bPageBorderVisible);
	DDX_Check(pDX, IDC_CHECK_PICTURE_SHADOW, m_bPageShadowVisible);
	DDX_Radio(pDX, IDC_RADIO_ORIENT0, m_nPageOrientation);
	DDX_Text(pDX, IDC_EDIT_MARGINS_L, m_nMarginL);
	DDX_Text(pDX, IDC_EDIT_MARGINS_T, m_nMarginT);
	DDX_Text(pDX, IDC_EDIT_MARGINS_R, m_nMarginR);
	DDX_Text(pDX, IDC_EDIT_MARGINS_B, m_nMarginB);
	DDX_Radio(pDX, IDC_RADIO_CTLCOLOR_TRANS, m_nCtlColorStyle);
	//}}AFX_DATA_MAP

	if (!m_pIExchanger)
		return;

	//  post processing: export to/import from final destination represented by m_pIExchanger
	if (pDX->m_bSaveAndValidate)
	{
		TRACE0("CSCEMFXGenPropPage::DoDataExchange.SaveAndValidate\n");
		SCSetModified();
	} else
	{
		TRACE0("CSCEMFXGenPropPage::DoDataExchange.Loading\n");

		m_pIExchanger->SCGetSettings_PageBorder(m_iFolderID,
			m_bPageBorderVisible,
			m_bPageShadowVisible);
		SCPPG_COOKIE_GETPROP(m_nPageOrientation, PageOrientation);
		SCPPG_GETCOLOR(m_nCtlColorStyle, CtlColorStyle, m_crCtlColor, CtlColor);

		// Control border
		DWORD dwIdx, dwCtlBorder;
		dwCtlBorder = m_pIExchanger->SCGetSettings_CtlBorder(m_iFolderID);
		SCGetCtlBorderPredefinedCookie(dwIdx, dwCtlBorder);
		m_CmbCtlBorder.SetCurSel(dwIdx);

		// Color value to Combos
		switch (m_nCtlColorStyle)
		{
		case SC_COLOR_TRANSPARENT:
			break;

		case SC_COLOR_SYSINDEX:
			m_CmbCtlColorSys.SCSetCurSelColor(m_crCtlColor);
			break;

		case SC_COLOR_RGBVALUE:
			m_CmbCtlColorRGB.SCSetCurSelColor(m_crCtlColor);
			break;
		}

		// Scaling
		int iScale = m_pIExchanger->SCGetSettings_Scale(m_iFolderID);
		if (iScale<0)
			m_CmbZoomBox.SCSelectValue(iScale);
		else
			m_CmbZoomBox.SCSetCurValue(iScale);

		// Margins
		RECT rcLTRB;
		m_pIExchanger->SCGetSettings_Margins(m_iFolderID, rcLTRB);
		m_nMarginL = rcLTRB.left;
		m_nMarginT = rcLTRB.top;
		m_nMarginR = rcLTRB.right;
		m_nMarginB = rcLTRB.bottom;

		// Page border and shadow
		m_pIExchanger->SCGetSettings_PageBorder(m_iFolderID,
			m_bPageBorderVisible, m_bPageShadowVisible);
	}
	SCEnableControls();
}

BEGIN_MESSAGE_MAP(CSCEMFXGenPropPage, CSCEMFXBasePropPage)
	//{{AFX_MSG_MAP(CSCEMFXGenPropPage)
	ON_BN_CLICKED(IDC_RADIO_CTLCOLOR_TRANS, OnRadioCtlColorTrans)
	ON_BN_CLICKED(IDC_RADIO_CTLCOLOR_SYS, OnRadioCtlColorSys)
	ON_BN_CLICKED(IDC_RADIO_CTLCOLOR_RGB, OnRadioCtlColorRgb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCEMFXGenPropPage message handlers

BOOL CSCEMFXGenPropPage::OnInitDialog() 
{
	CSCEMFXBasePropPage::OnInitDialog();
	
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
	
	UpdateData(FALSE); // Force first exchange
	SCEnableControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSCEMFXGenPropPage::SCEnableControls()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_RADIO_CTLCOLOR_SYS);
	SCEnableControl(IDC_COMBO_CTLCOLOR_SYS, (pBtn && 1==pBtn->GetCheck()));

	pBtn = (CButton*)GetDlgItem(IDC_RADIO_CTLCOLOR_RGB);
	SCEnableControl(IDC_COMBO_CTLCOLOR_RGB, (pBtn && 1==pBtn->GetCheck()));
}

void CSCEMFXGenPropPage::OnRadioCtlColorTrans() 
{
	SCEnableControls();
}

void CSCEMFXGenPropPage::OnRadioCtlColorSys() 
{
	SCEnableControls();
	
}

void CSCEMFXGenPropPage::OnRadioCtlColorRgb() 
{
	SCEnableControls();
}

/////////////////////////////////////////////////////////////////////////////
///
/// Attempt to force sending VK_RETURN to editboxes with the ES_WANTRETURN sytle.
///
BOOL CSCEMFXGenPropPage::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
        HWND hFocus = ::GetFocus();
		// detect the zoom combobox, and force it to eat
		HWND hParent = ::GetParent(hFocus);
		if (hParent && ::GetDlgCtrlID(hParent)==IDC_COMBO_OPTGEN_ZOOM)
		{
			m_CmbZoomBox.SCEatValue();
			return FALSE;    // Don't translate
		}
	};
	
	return CSCEMFXBasePropPage::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////

void CSCEMFXGenPropPage::OnOK()
{
	if (!m_pIExchanger)
		return;

	// Note: m_nPageOrientation, m_nCtlColorStyle are cookies
	// representing the selected items (buttons), not actual property values.
	// The can be -1 (no selection).
	SCPPG_COOKIE_DECL(m_nPageOrientation, PageOrientation);
	SCPPG_COOKIE_DECL(m_nCtlColorStyle, CtlColorStyle);
	SCPPG_DWPROP_DECL(m_crCtlColor, CtlColor);

	m_pIExchanger->SCPutSettings_PageBorder(m_iFolderID,
		m_bPageBorderVisible,
		m_bPageShadowVisible);
	SCPPG_COOKIE_SETPROP(m_nPageOrientation, PageOrientation);
	
	// Control border
	DWORD dwIdx = m_CmbCtlBorder.GetCurSel();
	if (CB_ERR!=dwIdx)
	{
		DWORD dwCtlBorder;
		SCGetCtlBorderPredefinedValue(dwCtlBorder, dwIdx);
		m_pIExchanger->SCPutSettings_CtlBorder(m_iFolderID, dwCtlBorder);
	}
	
	// Color value from combos
	switch (m_nCtlColorStyle)
	{
	case SC_COLOR_TRANSPARENT:
		break;
		
	case SC_COLOR_SYSINDEX:
		m_crCtlColor = (OLE_COLOR)m_CmbCtlColorSys.SCGetCurSelColor();
		break;
		
	case SC_COLOR_RGBVALUE:
		m_crCtlColor = (OLE_COLOR)m_CmbCtlColorRGB.SCGetCurSelColor();
		break;
	}
	SCPPG_SETCOLOR(m_nCtlColorStyle, CtlColorStyle, m_crCtlColor, CtlColor);
	
	// Scaling
	int iZoom = m_CmbZoomBox.SCGetCurValue();
	m_pIExchanger->SCPutSettings_Scale(m_iFolderID, iZoom);
	
	// Margins
	RECT rcLTRB;
	SetRect(&rcLTRB, m_nMarginL, m_nMarginT, m_nMarginR, m_nMarginB);
	m_pIExchanger->SCPutSettings_Margins(m_iFolderID, rcLTRB);
}


