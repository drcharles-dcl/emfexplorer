/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "scemfax.h"
#include "SCEMFAxGDIpPropPage.h"

#include "SCGenInclude.h"
#include SC_INC_SHARED(SCPropStrLists.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxGDIpPropPage dialog

IMPLEMENT_DYNCREATE(CSCEMFAxGDIpPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CSCEMFAxGDIpPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CSCEMFAxGDIpPropPage)
	ON_BN_CLICKED(IDC_CHK_OPTGEN_USEGDIP, OnChkOptgenUsegdip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

// {ADD1D248-E07A-4460-8F67-276786F9CDE7}
IMPLEMENT_OLECREATE_EX(CSCEMFAxGDIpPropPage, "SCEMFAx.CSCEMFAxGDIpPropPage",
	0xadd1d248, 0xe07a, 0x4460, 0x8f, 0x67, 0x27, 0x67, 0x86, 0xf9, 0xcd, 0xe7)


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxGDIpPropPage::CSCEMFAxGDIpPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CSCEMFAxGDIpPropPage

BOOL CSCEMFAxGDIpPropPage::CSCEMFAxGDIpPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_SCEMFAX_GDIP_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxGDIpPropPage::CSCEMFAxGDIpPropPage - Constructor

CSCEMFAxGDIpPropPage::CSCEMFAxGDIpPropPage() :
	COlePropertyPage(IDD, IDS_SCEMFAX_GDIP_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CSCEMFAxGDIpPropPage)
	m_bGDIplusEnabled = FALSE;
	m_nTextRenderingHint = -1;
	m_nTextContrast = -1;
	m_nSmoothingMode = -1;
	m_nInterpolationMode = -1;
	m_nPixelOffsetMode = -1;
	//}}AFX_DATA_INIT
	m_bGDIpOn = FALSE;
}

SC_IMPLEMENT_ENABLE_CONTROL(CSCEMFAxGDIpPropPage)

/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxGDIpPropPage::DoDataExchange - Moves data between page and properties

void CSCEMFAxGDIpPropPage::DoDataExchange(CDataExchange* pDX)
{
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_DATA_MAP(CSCEMFAxGDIpPropPage)
	DDP_Check(pDX, IDC_CHK_OPTGEN_USEGDIP, m_bGDIplusEnabled, _T("GDIplusEnabled") );
	DDX_Check(pDX, IDC_CHK_OPTGEN_USEGDIP, m_bGDIplusEnabled);
	DDP_CBIndex(pDX, IDC_COMBO_GDIP_TEXTHINTING, m_nTextRenderingHint, _T("TextRenderingQuality") );
	DDX_CBIndex(pDX, IDC_COMBO_GDIP_TEXTHINTING, m_nTextRenderingHint);
	DDP_CBIndex(pDX, IDC_COMBO_GDIP_TEXTCONTRAST, m_nTextContrast, _T("TextContrast") );
	DDX_CBIndex(pDX, IDC_COMBO_GDIP_TEXTCONTRAST, m_nTextContrast);
	DDP_CBIndex(pDX, IDC_COMBO_GDIP_SMOOTHING, m_nSmoothingMode, _T("SmoothingMode") );
	DDX_CBIndex(pDX, IDC_COMBO_GDIP_SMOOTHING, m_nSmoothingMode);
	DDP_CBIndex(pDX, IDC_COMBO_GDIP_INTERPOLATION, m_nInterpolationMode, _T("InterpolationMode") );
	DDX_CBIndex(pDX, IDC_COMBO_GDIP_INTERPOLATION, m_nInterpolationMode);
	DDP_CBIndex(pDX, IDC_COMBO_GDIP_PIXELOFFSET, m_nPixelOffsetMode, _T("PixelOffsetMode") );
	DDX_CBIndex(pDX, IDC_COMBO_GDIP_PIXELOFFSET, m_nPixelOffsetMode);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxGDIpPropPage message handlers

BOOL CSCEMFAxGDIpPropPage::OnInitDialog() 
{
	BOOL bResult = COlePropertyPage::OnInitDialog();
	
	// Add extra initialization here
	m_bGDIpOn = m_bGDIplusEnabled;

	SCUpdateControls();
	SCEnableControls(m_bGDIplusEnabled);

	return bResult;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSCEMFAxGDIpPropPage::SCUpdateControls()
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_GDIP_TEXTHINTING);
	SCFillTextRenderingHintCombo(pCombo);

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_GDIP_TEXTCONTRAST);
	SCFillTextContrastCombo(pCombo);

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_GDIP_SMOOTHING);
	SCFillSmoothingModeCombo(pCombo);

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_GDIP_INTERPOLATION);
	SCFillInterpolationModeCombo(pCombo);

	pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_GDIP_PIXELOFFSET);
	SCFillPixelOffsetModeCombo(pCombo);
}

void CSCEMFAxGDIpPropPage::SCEnableControls(BOOL bEnable/*=TRUE*/)
{
	SCEnableControl(IDC_COMBO_GDIP_TEXTHINTING, bEnable);
	SCEnableControl(IDC_COMBO_GDIP_TEXTCONTRAST, bEnable);
	SCEnableControl(IDC_COMBO_GDIP_SMOOTHING, bEnable);
	SCEnableControl(IDC_COMBO_GDIP_INTERPOLATION, bEnable);
	SCEnableControl(IDC_COMBO_GDIP_PIXELOFFSET, bEnable);
}



void CSCEMFAxGDIpPropPage::OnChkOptgenUsegdip() 
{
	// UpdateData(TRUE); // do not validate (wait for OK or Apply)
	m_bGDIpOn = !m_bGDIpOn;
	SCEnableControls(m_bGDIpOn);
}
