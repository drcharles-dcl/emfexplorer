/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "scemfax.h"
#include "SCEMFAxColorsPropPage.h"

#include "SCGenInclude.h"
#include SC_INC_SHARED(SCPropStrLists.h)
#include SC_INC_EMFLIB(SCEMFViewDefs.h)
#include SC_INC_EMFLIB(SCGdiplusUtils.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxColorsPropPage dialog

IMPLEMENT_DYNCREATE(CSCEMFAxColorsPropPage, CSCBaseOlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CSCEMFAxColorsPropPage, CSCBaseOlePropertyPage)
	//{{AFX_MSG_MAP(CSCEMFAxColorsPropPage)
	ON_BN_CLICKED(IDC_RADIO_PAPERCOLOR_TRANS, OnRadioPapercolorTrans)
	ON_BN_CLICKED(IDC_RADIO_PAPERCOLOR_SYS, OnRadioPapercolorSys)
	ON_BN_CLICKED(IDC_RADIO_PAPERCOLOR_RGB, OnRadioPapercolorRgb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

// {798DCDEF-76FC-4461-B933-C49280A1AA4A}
IMPLEMENT_OLECREATE_EX(CSCEMFAxColorsPropPage, "SCEMFAx.CSCEMFAxColorsPropPage",
	0x798dcdef, 0x76fc, 0x4461, 0xb9, 0x33, 0xc4, 0x92, 0x80, 0xa1, 0xaa, 0x4a)


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxColorsPropPage::CSCEMFAxColorsPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CSCEMFAxColorsPropPage

BOOL CSCEMFAxColorsPropPage::CSCEMFAxColorsPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_SCEMFAX_COLORS_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxColorsPropPage::CSCEMFAxColorsPropPage - Constructor

CSCEMFAxColorsPropPage::CSCEMFAxColorsPropPage() :
	CSCBaseOlePropertyPage(IDD, IDS_SCEMFAX_COLORS_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CSCEMFAxColorsPropPage)
	m_nTransparencyMode = -1;
	m_nReverseVideoMode = -1;
	m_nPaperColorStyle = -1;
	//}}AFX_DATA_INIT
	m_nPaperColor = RGB(255, 255, 255);
}

SC_IMPLEMENT_ENABLE_CONTROL(CSCEMFAxColorsPropPage)

/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxColorsPropPage::DoDataExchange - Moves data between page and properties

void CSCEMFAxColorsPropPage::DoDataExchange(CDataExchange* pDX)
{
	// Note: m_nTransparencyMode, m_nReverseVideoMode, m_nPaperColorStyle are cookies
	// representing the selected buttons, not actual property values.
	// The can be -1 (no selection).
	SCDECL_PROP_COOKIE(m_nTransparencyMode, TransparencyMode);
	SCDECL_PROP_COOKIE(m_nReverseVideoMode, ReverseVideoMode);
	SCDECL_PROP_COOKIE(m_nPaperColorStyle, PaperColorStyle);
	SCDECL_DWORD_PROP(m_nPaperColor, PaperColor);

	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_DATA_MAP(CSCEMFAxColorsPropPage)
	DDX_Control(pDX, IDC_COMBO_PAPERCOLOR_RGB, m_CmbPaperColorRGB);
	DDX_Control(pDX, IDC_COMBO_PAPERCOLOR_SYS, m_CmbPaperColorSys);
	DDX_Radio(pDX, IDC_RADIO_TRANSP_NORMAL, m_nTransparencyMode);
	DDX_Radio(pDX, IDC_RADIO_RVIDEO_NONE, m_nReverseVideoMode);
	DDX_Radio(pDX, IDC_RADIO_PAPERCOLOR_TRANS, m_nPaperColorStyle);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);

	// Special post processing (for properties not listed in data map)
	if (pDX->m_bSaveAndValidate)
	{
		// Convert button state to property value and save
		SCDDP_COOKIE_SETPROP(m_nTransparencyMode, TransparencyMode);
		SCDDP_COOKIE_SETPROP(m_nReverseVideoMode, ReverseVideoMode);
		SCDDP_COOKIE_SETPROP(m_nPaperColorStyle, PaperColorStyle);

		// Color value from combos
		switch (m_nPaperColorStyle)
		{
		case SC_COLOR_TRANSPARENT:
			break;

		case SC_COLOR_SYSINDEX:
			m_nPaperColor = (OLE_COLOR)m_CmbPaperColorSys.SCGetCurSelColor();
			SCDDP_SETPROP(m_nPaperColor, PaperColor);
			break;

		case SC_COLOR_RGBVALUE:
			m_nPaperColor = (OLE_COLOR)m_CmbPaperColorRGB.SCGetCurSelColor();
			SCDDP_SETPROP(m_nPaperColor, PaperColor);
			break;
		}
	} else
	{
		// Convert property value to button state
		SCDDP_COOKIE_GETPROP(m_nTransparencyMode, TransparencyMode);
		SCDDP_COOKIE_GETPROP(m_nReverseVideoMode, ReverseVideoMode);
		SCDDP_COOKIE_GETPROP(m_nPaperColorStyle, PaperColorStyle);

		// Color value to Combos
		switch (m_nPaperColorStyle)
		{
		case SC_COLOR_TRANSPARENT:
			break;

		case SC_COLOR_SYSINDEX:
			SCDDP_GETPROP(m_nPaperColor, PaperColor);
			m_CmbPaperColorSys.SCSetCurSelColor(m_nPaperColor);
			break;

		case SC_COLOR_RGBVALUE:
			SCDDP_GETPROP(m_nPaperColor, PaperColor);
			m_CmbPaperColorRGB.SCSetCurSelColor(m_nPaperColor);
			break;
		}
	}
	SCEnableControls();
}


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxColorsPropPage message handlers

BOOL CSCEMFAxColorsPropPage::OnInitDialog() 
{
	CSCBaseOlePropertyPage::OnInitDialog();
	
	// Add extra initialization here
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
		hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);

	m_CmbPaperColorRGB.SendMessage(WM_SETFONT, (WPARAM)hFont);
	m_CmbPaperColorRGB.InitColorsComboBox(RGB(255, 255, 255));
	m_CmbPaperColorSys.SendMessage(WM_SETFONT, (WPARAM)hFont);
	m_CmbPaperColorSys.InitColorsComboBox(SC_MAKE_SYSCOLOR(COLOR_WINDOW));
	
	SCEnableControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSCEMFAxColorsPropPage::SCEnableControls()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_RADIO_PAPERCOLOR_SYS);
	SCEnableControl(IDC_COMBO_PAPERCOLOR_SYS, (pBtn && 1==pBtn->GetCheck()));

	pBtn = (CButton*)GetDlgItem(IDC_RADIO_PAPERCOLOR_RGB);
	SCEnableControl(IDC_COMBO_PAPERCOLOR_RGB, (pBtn && 1==pBtn->GetCheck()));
}

void CSCEMFAxColorsPropPage::OnRadioPapercolorTrans() 
{
	SCEnableControls();
}

void CSCEMFAxColorsPropPage::OnRadioPapercolorSys() 
{
	SCEnableControls();
	
}

void CSCEMFAxColorsPropPage::OnRadioPapercolorRgb() 
{
	SCEnableControls();
}
