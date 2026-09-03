/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "emfexplorer.h"
#include "SCEMFXGDIpPropPage.h"

#include "SCGenInclude.h"
#include SC_INC_SHARED(SCPropStrLists.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCEMFXGDIpPropPage property page

IMPLEMENT_DYNCREATE(CSCEMFXGDIpPropPage, CSCEMFXBasePropPage)

CSCEMFXGDIpPropPage::CSCEMFXGDIpPropPage():
	CSCEMFXBasePropPage(CSCEMFXGDIpPropPage::IDD, IDS_PPG_GDIP)
{
	//{{AFX_DATA_INIT(CSCEMFXGDIpPropPage)
	m_bGDIplusEnabled = FALSE;
	m_nTextRenderingHint = -1;
	m_nTextContrast = -1;
	m_nSmoothingMode = -1;
	m_nInterpolationMode = -1;
	m_nPixelOffsetMode = -1;
	//}}AFX_DATA_INIT
	m_bGDIpOn = FALSE;
}

CSCEMFXGDIpPropPage::~CSCEMFXGDIpPropPage()
{
}

void CSCEMFXGDIpPropPage::DoDataExchange(CDataExchange* pDX)
{
	CSCEMFXBasePropPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSCEMFXGDIpPropPage)
	DDX_Check(pDX, IDC_CHK_OPTGEN_USEGDIP, m_bGDIplusEnabled);
	DDX_CBIndex(pDX, IDC_COMBO_GDIP_TEXTHINTING, m_nTextRenderingHint);
	DDX_CBIndex(pDX, IDC_COMBO_GDIP_TEXTCONTRAST, m_nTextContrast);
	DDX_CBIndex(pDX, IDC_COMBO_GDIP_SMOOTHING, m_nSmoothingMode);
	DDX_CBIndex(pDX, IDC_COMBO_GDIP_INTERPOLATION, m_nInterpolationMode);
	DDX_CBIndex(pDX, IDC_COMBO_GDIP_PIXELOFFSET, m_nPixelOffsetMode);
	//}}AFX_DATA_MAP

	if (!m_pIExchanger)
		return;

	if (pDX->m_bSaveAndValidate)
	{
		TRACE0("CSCEMFXGDIpPropPage::DoDataExchange.SaveAndValidate\n");
		SCSetModified();
	} else
	{
		TRACE0("CSCEMFXGDIpPropPage::DoDataExchange.Loading\n");

		m_bGDIplusEnabled = m_pIExchanger->SCGetSettings_GDIplusEnabled(m_iFolderID);

		m_pIExchanger->SCGetSettings_TextRendering(m_iFolderID,
			m_nTextRenderingHint, m_nTextContrast);

		m_pIExchanger->SCGetSettings_Smoothing(m_iFolderID,
			m_nSmoothingMode, m_nInterpolationMode, m_nPixelOffsetMode);
	}
}


BEGIN_MESSAGE_MAP(CSCEMFXGDIpPropPage, CSCEMFXBasePropPage)
	//{{AFX_MSG_MAP(CSCEMFXGDIpPropPage)
	ON_BN_CLICKED(IDC_CHK_OPTGEN_USEGDIP, OnChkOptgenUsegdip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCEMFXGDIpPropPage message handlers


BOOL CSCEMFXGDIpPropPage::OnInitDialog() 
{
	BOOL bResult = CSCEMFXBasePropPage::OnInitDialog();
	
	// Add extra initialization here
	m_bGDIpOn = m_bGDIplusEnabled;

	SCUpdateControls();

	UpdateData(FALSE); // Force first exchange
	SCEnableControls(m_bGDIplusEnabled);
	return bResult;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSCEMFXGDIpPropPage::SCUpdateControls()
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

void CSCEMFXGDIpPropPage::SCEnableControls(BOOL bEnable/*=TRUE*/)
{
	SCEnableControl(IDC_COMBO_GDIP_TEXTHINTING, bEnable);
	SCEnableControl(IDC_COMBO_GDIP_TEXTCONTRAST, bEnable);
	SCEnableControl(IDC_COMBO_GDIP_SMOOTHING, bEnable);
	SCEnableControl(IDC_COMBO_GDIP_INTERPOLATION, bEnable);
	SCEnableControl(IDC_COMBO_GDIP_PIXELOFFSET, bEnable);
}

void CSCEMFXGDIpPropPage::OnChkOptgenUsegdip() 
{
	// UpdateData(TRUE); // do not validate (wait for OK or Apply)
	m_bGDIpOn = !m_bGDIpOn;
	SCEnableControls(m_bGDIpOn);
}

void CSCEMFXGDIpPropPage::OnOK()
{
	if (!m_pIExchanger)
		return;

	m_pIExchanger->SCPutSettings_GDIplusEnabled(m_iFolderID, m_bGDIplusEnabled);
	
	m_pIExchanger->SCPutSettings_TextRendering(m_iFolderID,
		m_nTextRenderingHint, m_nTextContrast);
	
	m_pIExchanger->SCPutSettings_Smoothing(m_iFolderID,
		m_nSmoothingMode, m_nInterpolationMode, m_nPixelOffsetMode);
}

