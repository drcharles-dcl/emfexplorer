/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "emfexplorer.h"
#include "SCEMFXColorsPropPage.h"

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
// CSCEMFXColorsPropPage property page

IMPLEMENT_DYNCREATE(CSCEMFXColorsPropPage, CSCEMFXBasePropPage)

CSCEMFXColorsPropPage::CSCEMFXColorsPropPage() :
	CSCEMFXBasePropPage(CSCEMFXColorsPropPage::IDD, IDS_PPG_COLORS)
{
	//{{AFX_DATA_INIT(CSCEMFXColorsPropPage)
	m_nTransparencyMode = -1;
	m_nReverseVideoMode = -1;
	m_nPaperColorStyle = -1;
	//}}AFX_DATA_INIT
	m_nPaperColor = RGB(255, 255, 255);
}

CSCEMFXColorsPropPage::~CSCEMFXColorsPropPage()
{
}

void CSCEMFXColorsPropPage::DoDataExchange(CDataExchange* pDX)
{
	// Note: m_nTransparencyMode, m_nReverseVideoMode, m_nPaperColorStyle are cookies
	// representing the selected buttons, not actual property values.
	// The can be -1 (no selection).
//		SCPPG_COOKIE_DECL(m_nTransparencyMode, TransparencyMode);
//		SCPPG_COOKIE_DECL(m_nReverseVideoMode, ReverseVideoMode);
//		SCPPG_COOKIE_DECL(m_nPaperColorStyle, PaperColorStyle);
//		SCPPG_DWPROP_DECL(m_nPaperColor, PaperColor);

	CSCEMFXBasePropPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSCEMFXColorsPropPage)
	DDX_Control(pDX, IDC_COMBO_PAPERCOLOR_RGB, m_CmbPaperColorRGB);
	DDX_Control(pDX, IDC_COMBO_PAPERCOLOR_SYS, m_CmbPaperColorSys);
	DDX_Radio(pDX, IDC_RADIO_TRANSP_NORMAL, m_nTransparencyMode);
	DDX_Radio(pDX, IDC_RADIO_RVIDEO_NONE, m_nReverseVideoMode);
	DDX_Radio(pDX, IDC_RADIO_PAPERCOLOR_TRANS, m_nPaperColorStyle);
	//}}AFX_DATA_MAP

	if (!m_pIExchanger)
		return;

	if (pDX->m_bSaveAndValidate)
	{
		TRACE0("CSCEMFXColorsPropPage::DoDataExchange.SaveAndValidate\n");
		SCSetModified();
	} else
	{
		TRACE0("CSCEMFXColorsPropPage::DoDataExchange.Loading\n");

		// Convert property value to button state
		SCPPG_COOKIE_GETPROP(m_nTransparencyMode, TransparencyMode);
		SCPPG_COOKIE_GETPROP(m_nReverseVideoMode, ReverseVideoMode);
		SCPPG_GETCOLOR(m_nPaperColorStyle, PaperColorStyle, m_nPaperColor, PaperColor);

		// Color value to Combos
		switch (m_nPaperColorStyle)
		{
		case SC_COLOR_TRANSPARENT:
			break;

		case SC_COLOR_SYSINDEX:
			m_CmbPaperColorSys.SCSetCurSelColor(m_nPaperColor);
			break;

		case SC_COLOR_RGBVALUE:
			m_CmbPaperColorRGB.SCSetCurSelColor(m_nPaperColor);
			break;
		}
	}
	SCEnableControls();
}


BEGIN_MESSAGE_MAP(CSCEMFXColorsPropPage, CSCEMFXBasePropPage)
	//{{AFX_MSG_MAP(CSCEMFXColorsPropPage)
	ON_BN_CLICKED(IDC_RADIO_PAPERCOLOR_TRANS, OnRadioPapercolorTrans)
	ON_BN_CLICKED(IDC_RADIO_PAPERCOLOR_SYS, OnRadioPapercolorSys)
	ON_BN_CLICKED(IDC_RADIO_PAPERCOLOR_RGB, OnRadioPapercolorRgb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCEMFXColorsPropPage message handlers

BOOL CSCEMFXColorsPropPage::OnInitDialog() 
{
	CSCEMFXBasePropPage::OnInitDialog();
	
	// Add extra initialization here
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
		hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);

	m_CmbPaperColorRGB.SendMessage(WM_SETFONT, (WPARAM)hFont);
	m_CmbPaperColorRGB.InitColorsComboBox(RGB(255, 255, 255));
	m_CmbPaperColorSys.SendMessage(WM_SETFONT, (WPARAM)hFont);
	m_CmbPaperColorSys.InitColorsComboBox(SC_MAKE_SYSCOLOR(COLOR_WINDOW));
	
	UpdateData(FALSE); // Force first exchange
	SCEnableControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSCEMFXColorsPropPage::SCEnableControls()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_RADIO_PAPERCOLOR_SYS);
	SCEnableControl(IDC_COMBO_PAPERCOLOR_SYS, (pBtn && 1==pBtn->GetCheck()));

	pBtn = (CButton*)GetDlgItem(IDC_RADIO_PAPERCOLOR_RGB);
	SCEnableControl(IDC_COMBO_PAPERCOLOR_RGB, (pBtn && 1==pBtn->GetCheck()));
}

void CSCEMFXColorsPropPage::OnRadioPapercolorTrans() 
{
	SCEnableControls();
}

void CSCEMFXColorsPropPage::OnRadioPapercolorSys() 
{
	SCEnableControls();
	
}

void CSCEMFXColorsPropPage::OnRadioPapercolorRgb() 
{
	SCEnableControls();
}

void CSCEMFXColorsPropPage::OnOK()
{
	if (!m_pIExchanger)
		return;

	// Note: m_nTransparencyMode, m_nReverseVideoMode, m_nPaperColorStyle are cookies
	// representing the selected buttons, not actual property values.
	// The can be -1 (no selection).
	SCPPG_COOKIE_DECL(m_nTransparencyMode, TransparencyMode);
	SCPPG_COOKIE_DECL(m_nReverseVideoMode, ReverseVideoMode);
	SCPPG_COOKIE_DECL(m_nPaperColorStyle, PaperColorStyle);
	SCPPG_DWPROP_DECL(m_nPaperColor, PaperColor);

	// Convert button state to property value and save
	SCPPG_COOKIE_SETPROP(m_nTransparencyMode, TransparencyMode);
	SCPPG_COOKIE_SETPROP(m_nReverseVideoMode, ReverseVideoMode);
	
	// Color value from combos
	switch (m_nPaperColorStyle)
	{
	case SC_COLOR_TRANSPARENT:
		break;
		
	case SC_COLOR_SYSINDEX:
		m_nPaperColor = (OLE_COLOR)m_CmbPaperColorSys.SCGetCurSelColor();
		break;
		
	case SC_COLOR_RGBVALUE:
		m_nPaperColor = (OLE_COLOR)m_CmbPaperColorRGB.SCGetCurSelColor();
		break;
	}
	SCPPG_SETCOLOR(m_nPaperColorStyle, PaperColorStyle, m_nPaperColor, PaperColor);
}

