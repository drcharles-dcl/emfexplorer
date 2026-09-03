/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */

#include "stdafx.h"
#include "SCEMFAx.h"
#include "SCEMFAxCtl.h"
#include "SCEMFAxPpg.h"
#include "SCEMFAxGDIpPropPage.h"
#include "SCEMFAxColorsPropPage.h"
#include "SCEMFAxPropSheet.h"

#include "SCGenInclude.h"
#include SC_INC_SHARED(SCPropStrLists.h)
#include SC_INC_WINLIB(SCGDIPlus.h)
#include SC_INC_WINLIB(SCGDIUtils.h)
#include SC_INC_EMFLIB(SCEMF.h)
#include SC_INC_EMFLIB(SCEMFRasterizer.h)
#include SC_INC_EMFLIB(SCEMFViewDefs.h)
#include SC_INC_EMFLIB(SCEMFViewHelpers.h)
#include <windowsx.h>	// message crackers

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CSCEMFAxCtrl, COleControl)

CSCGDIPlus g_GDIPlus;

// Controls
#define IDC_WND_EMFIMG        100



/////////////////////////////////////////////////////////////////////////////
// Debug Helpers
#ifdef _DEBUG
	#define SC_SHOW_AXMSG1(szFmt, nValue)\
		{\
			CString strMsg;\
			strMsg.Format(_T(szFmt), nValue);\
			AfxMessageBox(strMsg);\
		}

	#define SC_SHOW_AXMSG0(szMsg)	AfxMessageBox(_T(szMsg))
#else
	#define SC_SHOW_AXMSG1(szFmt, nValue)
	#define SC_SHOW_AXMSG0(szMsg)
#endif
#define SC_SHOW_AXMSG	SC_SHOW_AXMSG0	

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CSCEMFAxCtrl, COleControl)
	//{{AFX_MSG_MAP(CSCEMFAxCtrl)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CSCEMFAxCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CSCEMFAxCtrl)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "Picture", Get_Picture, Set_Picture, VT_BSTR)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "CtlBorder", GetCtlBorder, SetCtlBorder, VT_I2)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "CtlColorStyle", GetCtlColorStyle, SetCtlColorStyle, VT_I2)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "CtlColor", GetCtlColor, SetCtlColor, VT_COLOR)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "PageBorderVisible", GetPageBorderVisible, SetPageBorderVisible, VT_BOOL)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "PageShadowVisible", GetPageShadowVisible, SetPageShadowVisible, VT_BOOL)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "PaperColorStyle", GetPaperColorStyle, SetPaperColorStyle, VT_I2)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "PaperColor", GetPaperColor, SetPaperColor, VT_COLOR)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "FitMode", GetFitMode, SetFitMode, VT_I2)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "Scale", GetScale, SetScale, VT_R4)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "MarginL", GetMarginL, SetMarginL, VT_XSIZE_PIXELS)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "MarginT", GetMarginT, SetMarginT, VT_YSIZE_PIXELS)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "MarginR", GetMarginR, SetMarginR, VT_XSIZE_PIXELS)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "MarginB", GetMarginB, SetMarginB, VT_YSIZE_PIXELS)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "PageOrientation", GetPageOrientation, SetPageOrientation, VT_I2)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "TransparencyMode", GetTransparencyMode, SetTransparencyMode, VT_I2)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "ReverseVideoMode", GetReverseVideoMode, SetReverseVideoMode, VT_I2)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "GDIplusEnabled", GetGDIplusEnabled, SetGDIplusEnabled, VT_BOOL)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "TextRenderingQuality", GetTextRenderingHint, SetTextRenderingHint, VT_I2)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "TextContrast", GetTextContrast, SetTextContrast, VT_I2)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "SmoothingMode", GetSmoothingMode, SetSmoothingMode, VT_I2)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "InterpolationMode", GetInterpolationMode, SetInterpolationMode, VT_I2)
	DISP_PROPERTY_EX(CSCEMFAxCtrl, "PixelOffsetMode", GetPixelOffsetMode, SetPixelOffsetMode, VT_I2)
	DISP_FUNCTION(CSCEMFAxCtrl, "SaveAs", SaveAs, VT_I4, VTS_BSTR VTS_I2 VTS_BOOL)
	DISP_DEFVALUE(CSCEMFAxCtrl, "Picture")
	DISP_STOCKPROP_READYSTATE()
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CSCEMFAxCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CSCEMFAxCtrl, COleControl)
	//{{AFX_EVENT_MAP(CSCEMFAxCtrl)
	EVENT_STOCK_READYSTATECHANGE()
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CSCEMFAxCtrl, 3)
	PROPPAGEID(CSCEMFAxPropPage::guid)
	PROPPAGEID(CSCEMFAxGDIpPropPage::guid)
	PROPPAGEID(CSCEMFAxColorsPropPage::guid)
END_PROPPAGEIDS(CSCEMFAxCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CSCEMFAxCtrl, "SCEMFAX.SCEMFAxCtrl.1",
	0x59f9ff0e, 0xc91, 0x4b80, 0x8d, 0xe3, 0x11, 0x9e, 0x19, 0xa9, 0x6a, 0xe0)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CSCEMFAxCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DSCEMFAx =
		{ 0xc7cf7c71, 0xefc3, 0x4b48, { 0x89, 0x3f, 0xc6, 0xaf, 0xc8, 0x9e, 0x39, 0x18 } };
const IID BASED_CODE IID_DSCEMFAxEvents =
		{ 0x4e2c90da, 0xf52c, 0x4425, { 0x95, 0xb0, 0xbb, 0x14, 0xe4, 0x44, 0xc5, 0x1a } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwSCEMFAxOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CSCEMFAxCtrl, IDS_SCEMFAX, _dwSCEMFAxOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxCtrl::CSCEMFAxCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CSCEMFAxCtrl

BOOL CSCEMFAxCtrl::CSCEMFAxCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_SCEMFAX,
			IDB_SCEMFAX,
			afxRegApartmentThreading,
			_dwSCEMFAxOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxCtrl::CSCEMFAxCtrl - Constructor

//------------------------------------------------------------------
// disable Warning C4355: "'this' used in base member initializer list."
// Yes, this is dangerous. But not in this context: the CDataPathProperty
// is just storing the pointer in its m_pControl member.

// unsafe
#pragma    warning(disable : 4355)

// Ax-specific defaults
#define SC_AXDFLT_CTLBORDER				SC_CSM_CTLBORDER_NONE
#define SC_AXDFLT_CTLCOLORSTYLE			SC_COLOR_SYSINDEX
#define SC_AXDFLT_CTLCOLOR				SC_MAKE_SYSCOLOR(COLOR_WINDOW)
	//--
#define SC_AXDFLT_PAGEBORDERVISIBLE		FALSE
#define SC_AXDFLT_PAGESHADOWVISIBLE		FALSE
#define SC_AXDFLT_PAPERCOLORSTYLE		SC_COLOR_RGBVALUE
#define SC_AXDFLT_PAPERCOLOR			RGB(255, 255, 255)
#define SC_AXDFLT_FITMODE				SC_FIT_PAGE
#define SC_AXDFLT_SCALE					SC_ZOOM100
#define SC_AXDFLT_MARGINL				0
#define SC_AXDFLT_MARGINT				0
#define SC_AXDFLT_MARGINR				0
#define SC_AXDFLT_MARGINB				0
#define SC_AXDFLT_PAGEORIENTATION		0
#define SC_AXDFLT_TRANSPARENCYMODE		SC_TRANSPARENCY_NORMAL
#define SC_AXDFLT_REVERSEVIDEOMODE		SC_REVERSE_VIDEO_NONE
	//--
#define SC_AXDFLT_GDIPLUSENABLED		TRUE
#define SC_AXDFLT_TEXTRENDERINGHINT		TextRenderingHintAntiAlias
#define SC_AXDFLT_TEXTCONTRAST			2 // hack: 0-12 => from darker to brighter
#define SC_AXDFLT_SMOOTHINGMODE			SmoothingModeHighQuality
#define SC_AXDFLT_INTERPOLATIONMODE		InterpolationModeHighQualityBicubic
#define SC_AXDFLT_PIXELOFFSETMODE		PixelOffsetModeHighQuality
//

CSCEMFAxCtrl::CSCEMFAxCtrl():
	m_hPopupMenu(NULL),
	//
	m_Data(this),
	// Note default values in DoPropExchange prevail on these ones
	m_usCtlBorder(SC_AXDFLT_CTLBORDER),
	m_usCtlColorStyle(SC_AXDFLT_CTLCOLORSTYLE),
	m_crCtlColor(SC_AXDFLT_CTLCOLOR),
		//--
	m_bPageBorderVisible(SC_AXDFLT_PAGEBORDERVISIBLE),
	m_bPageShadowVisible(SC_AXDFLT_PAGESHADOWVISIBLE),
	m_usPaperColorStyle(SC_AXDFLT_PAPERCOLORSTYLE),
	m_crPaperColor(SC_AXDFLT_PAPERCOLOR),
	m_usFitMode(SC_AXDFLT_FITMODE),
	m_fScale(float(SC_AXDFLT_SCALE)/SC_ZOOM100),
	m_lMarginL(SC_AXDFLT_MARGINL),
	m_lMarginT(SC_AXDFLT_MARGINT),
	m_lMarginR(SC_AXDFLT_MARGINR),
	m_lMarginB(SC_AXDFLT_MARGINB),
	m_usPageOrientation(SC_AXDFLT_PAGEORIENTATION),
	m_usTransparencyMode(SC_AXDFLT_TRANSPARENCYMODE),
	m_usReverseVideoMode(SC_AXDFLT_REVERSEVIDEOMODE),
		//--
	m_bEnableGDIp(TRUE),
	m_usTextRenderingHint(SC_AXDFLT_TEXTRENDERINGHINT),
	m_usTextContrast(SC_AXDFLT_TEXTCONTRAST),
	m_usSmoothingMode(SC_AXDFLT_SMOOTHINGMODE),
	m_usInterpolationMode(SC_AXDFLT_INTERPOLATIONMODE),
	m_usPixelOffsetMode(SC_AXDFLT_PIXELOFFSETMODE)
	//
{
	InitializeIIDs(&IID_DSCEMFAx, &IID_DSCEMFAxEvents);

	m_lReadyState = READYSTATE_LOADING;
	// Initialize control's instance data here.
	g_GDIPlus.SCInitInstance();

	// Popup menu
	// Setup document and rendering quality
	m_CtlImage.SCSetEMFDoc(&m_EMFDoc);

	// copy attributes to control
	SCCopyStateToControl();
}
// return to safe state
#pragma    warning(default :4355)

void CSCEMFAxCtrl::SCCopyStateToControl()
{
	// Control border
	m_CtlImage.SCSetCtlBorder(m_usCtlBorder);
	// Control color style and color
	m_CtlImage.SCSetCtlColorStyle(m_usCtlColorStyle, m_crCtlColor, FALSE);

	// Paper border and shadow
	m_CtlImage.SCSetPageBorderVisible(m_bPageBorderVisible, FALSE);
	m_CtlImage.SCSetPageShadowVisible(m_bPageShadowVisible, FALSE);

	// Paper color style and color
	m_CtlImage.SCSetPaperColorStyle(m_usPaperColorStyle, m_crPaperColor, FALSE);

	// Fitmode and Scale
	m_CtlImage.SCSetFitMode(m_usFitMode, FALSE);
	if (SC_FIT_NONE!=m_usFitMode)
		m_fScale = m_CtlImage.SCGetScale();
	else
		m_CtlImage.SCSetScale(m_fScale, FALSE);

	// Margins
	CRect rcMargins(m_lMarginL, m_lMarginT, m_lMarginR, m_lMarginB);
	m_CtlImage.SCSetMargins(rcMargins, FALSE);

	// Orientation
	m_CtlImage.SCSetAngle(m_usPageOrientation, FALSE);

	// Transparency mode
	m_CtlImage.SCSetTransparencyMode(m_usTransparencyMode, FALSE);
	// Reverse video mode
	m_CtlImage.SCSetReverseVideoMode(m_usReverseVideoMode, FALSE);

	// GDI+
	m_CtlImage.SCEnableGDIp(m_bEnableGDIp, FALSE);
	m_CtlImage.SCSetTextRenderingHint(m_usTextRenderingHint, FALSE);
	m_CtlImage.SCSetTextContrast(m_usTextContrast, FALSE);
	m_CtlImage.SCSetSmoothingMode(m_usSmoothingMode, FALSE);
	m_CtlImage.SCSetInterpolationMode(m_usInterpolationMode, FALSE);
	m_CtlImage.SCSetPixelOffsetMode(m_usPixelOffsetMode, FALSE);

	// Refresh
	m_CtlImage.SCRefresh();
}


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxCtrl::~CSCEMFAxCtrl - Destructor

CSCEMFAxCtrl::~CSCEMFAxCtrl()
{
	// Cleanup your control's instance data here.
	g_GDIPlus.SCExitInstance();
	if (m_hPopupMenu)
		DestroyMenu(m_hPopupMenu);
}


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxCtrl::OnDraw - Drawing function
void CSCEMFAxCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
}

/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxCtrl::DoPropExchange - Persistence support

/////////////////////////////////////////////////////////////////////////////
// Persistence helpers
#define SC_BEGIN_PROPEX() \
{\
	DWORD dwCookie, dwValue;\
	USHORT usCookie;\
	BOOL bLoading = pPX->IsLoading();\
\

#define SCPX_USHORT(usMember, prop) \
{\
	SCGet##prop##PredefinedCookie(dwCookie, usMember);\
	usCookie = (USHORT)dwCookie;\
\
	PX_UShort(pPX,	SC_T_##prop##, usCookie, usCookie);\
\
	if (bLoading)\
	{\
		SCGet##prop##PredefinedValue(dwValue, usCookie);\
		usMember = (USHORT)dwValue;\
	}\
}\

#define SC_END_PROPEX() \
	if (bLoading) \
		SCCopyStateToControl(); \
}\

/////////////////////////////////////////////////////////////////////////////

void CSCEMFAxCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// Call PX_ functions for each persistent custom property.
	// Direct connection
	PX_DataPath(pPX, SC_T_Src,				 m_Data);
	PX_Color(pPX,	 SC_T_CtlColor,			 m_crCtlColor,			SC_AXDFLT_CTLCOLOR);
	PX_Bool(pPX,	 SC_T_PageBorderVisible, m_bPageBorderVisible,	SC_AXDFLT_PAGEBORDERVISIBLE);
	PX_Bool(pPX,	 SC_T_PageShadowVisible, m_bPageShadowVisible,	SC_AXDFLT_PAGESHADOWVISIBLE);
	PX_Color(pPX,	 SC_T_PaperColor,		 m_crPaperColor,		SC_AXDFLT_PAPERCOLOR);
	PX_Float(pPX,	 SC_T_Scale,			 m_fScale,				SC_AXDFLT_SCALE);
	PX_Long(pPX,	 SC_T_MarginL,			 m_lMarginL,			SC_AXDFLT_MARGINL);
	PX_Long(pPX,	 SC_T_MarginT,			 m_lMarginT,			SC_AXDFLT_MARGINT);
	PX_Long(pPX,	 SC_T_MarginR,			 m_lMarginR,			SC_AXDFLT_MARGINR);
	PX_Long(pPX,	 SC_T_MarginB,			 m_lMarginB,			SC_AXDFLT_MARGINB);
	PX_Bool(pPX,	 SC_T_GDIplusEnabled,	 m_bEnableGDIp,			SC_AXDFLT_GDIPLUSENABLED);

	// Special : translate enums properties, and update image
	SC_BEGIN_PROPEX()
		//PX_UShort(pPX,	SC_T_CtlBorder, m_usCtlBorder, SC_AXDFLT_CTLBORDER);
		SCPX_USHORT(m_usCtlBorder, CtlBorder)
		
		//PX_UShort(pPX,	SC_T_CtlColorStyle, m_usCtlColorStyle, SC_AXDFLT_CTLCOLORSTYLE);
		SCPX_USHORT(m_usCtlColorStyle, CtlColorStyle)
		
		//PX_UShort(pPX,	SC_T_PaperColorStyle, m_usPaperColorStyle, SC_AXDFLT_PAPERCOLORSTYLE);
		SCPX_USHORT(m_usPaperColorStyle, PaperColorStyle)
		
		//PX_UShort(pPX,	SC_T_FitMode, m_usFitMode, SC_AXDFLT_FITMODE);
		SCPX_USHORT(m_usFitMode, PageFitMode)
		
		//PX_UShort(pPX,	SC_T_PageOrientation, m_usPageOrientation, SC_AXDFLT_PAGEORIENTATION);
		SCPX_USHORT(m_usPageOrientation, PageOrientation)
		
		//PX_UShort(pPX,	SC_T_TransparencyMode, m_usTransparencyMode, SC_AXDFLT_TRANSPARENCYMODE);
		SCPX_USHORT(m_usTransparencyMode, TransparencyMode)
		
		//PX_UShort(pPX,	SC_T_ReverseVideoMode, m_usReverseVideoMode, SC_AXDFLT_REVERSEVIDEOMODE);
		SCPX_USHORT(m_usReverseVideoMode, ReverseVideoMode)
		
		//PX_UShort(pPX,	SC_T_TextRenderingHint, m_usTextRenderingHint, SC_AXDFLT_TEXTRENDERINGHINT);
		SCPX_USHORT(m_usTextRenderingHint, TextRenderingHint) 
		
		//PX_UShort(pPX,	SC_T_TextContrast, m_usTextContrast, SC_AXDFLT_TEXTCONTRAST);
		SCPX_USHORT(m_usTextContrast, TextContrast)
		
		//PX_UShort(pPX,	SC_T_SmoothingMode, m_usSmoothingMode, SC_AXDFLT_SMOOTHINGMODE);
		SCPX_USHORT(m_usSmoothingMode, SmoothingMode)
		
		// PX_UShort(pPX,	SC_T_InterpolationMode, m_usInterpolationMode, SC_AXDFLT_INTERPOLATIONMODE);
		SCPX_USHORT(m_usInterpolationMode, InterpolationMode)
		
		//PX_UShort(pPX,	SC_T_PixelOffsetMode, m_usPixelOffsetMode, SC_AXDFLT_PIXELOFFSETMODE);
		SCPX_USHORT(m_usPixelOffsetMode, PixelOffsetMode)
	SC_END_PROPEX()
//		if (READYSTATE_LOADING==m_lReadyState)
//			InternalSetReadyState(READYSTATE_INTERACTIVE);
//		PX_DataPath(pPX, SC_T_Src,				 m_Data);
}


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxCtrl::OnResetState - Reset control to default state

void CSCEMFAxCtrl::OnResetState()
{
	m_Data.ResetData();
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// Reset any other control state here.
	SCCopyStateToControl();
}


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxCtrl::AboutBox - Display an "About" box to the user

void CSCEMFAxCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_SCEMFAX);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CSCEMFAxCtrl message handlers

void CSCEMFAxCtrl::InformDlStatus(CSCEMFDataPathProperty::DLState dlState)
{
	HEMFVECTOR vectHandles;
	HENHMETAFILE hEMF = m_Data.SCDetachEMF();
	if (hEMF)
	{
		vectHandles.push_back(hEMF);
		m_EMFDoc.SCCleanup();
		m_EMFDoc.SCPasteEMFPages(vectHandles);
		if (::IsWindow(m_CtlImage.m_hWnd))
		{
			m_CtlImage.SCGotoFirstPage();
			if (SC_FIT_NONE!=m_usFitMode)
				m_fScale = m_CtlImage.SCGetScale();
		}
	}

	if (m_hWnd)
		Invalidate(FALSE);
	if (dlState == CSCEMFDataPathProperty::DLState::dlDone)
	{
		InternalSetReadyState(READYSTATE_COMPLETE);
	} else
	{
		InternalSetReadyState(READYSTATE_INTERACTIVE);
	}
}

BOOL CSCEMFAxCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	// Add your specialized code here and/or call the base class
	cs.style &= ~(WS_HSCROLL|WS_VSCROLL);
	return COleControl::PreCreateWindow(cs);
}

BOOL CSCEMFAxCtrl::IsSubclassedControl()
{
	return FALSE;
}

LRESULT CSCEMFAxCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Creation/background events
//
int CSCEMFAxCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create controls
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|/*WS_BORDER|*/WS_CLIPCHILDREN;
	if (!m_CtlImage.Create(NULL, _T("EMFCtlImageAx"), 
		dwStyle, 
		CRect(10,10,150,50), this, IDC_WND_EMFIMG))
	{
		SC_SHOW_AXMSG("Could not create EMFCtlImageAx image control");
		return -1;
	}
	// control border
	m_CtlImage.SCSetCtlBorder(m_usCtlBorder);

	// control color
	if (SC_COLOR_TRANSPARENT==m_usCtlColorStyle)
	{
		m_crCtlColor = AmbientBackColor();
		if (SC_IS_SYSCOLOR(m_crCtlColor))
			m_crCtlColor = GetSysColor(SC_SYSCOLOR_INDEX(m_crCtlColor));
		m_CtlImage.SCSetCtlColorStyle(m_usCtlColorStyle, m_crCtlColor);
	}

	// Contextual menu
	m_CtlImage.SCSetCtlOwner(this);
	CMenu menu;
	if (menu.LoadMenu(IDR_CTX_MENU))
	{
		CMenu *pSubMenu = menu.GetSubMenu(MNU_IDX_TRACKPOPUP1);
		if (pSubMenu)
		{
			m_CtlImage.SCSetCtxMenu(pSubMenu->m_hMenu);
		}
		m_hPopupMenu = menu.Detach();
	}

	return 0;
}

BOOL CSCEMFAxCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE; // do not erase
	//return COleControl::OnEraseBkgnd(pDC);
}

void CSCEMFAxCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);
	
	if (!::IsWindow(m_CtlImage.m_hWnd))
		return;

	m_CtlImage.MoveWindow(0, 0, cx, cy);
	if (0==m_CtlImage.SCGetCurPage() && m_CtlImage.SCGetNumPages())
	{// Load image and update image-dependent properties
		m_CtlImage.SCGotoFirstPage();
		if (SC_FIT_NONE!=m_usFitMode)
			m_fScale = m_CtlImage.SCGetScale();
	}
}

/////////////////////////////////////////////////////////////////////////////
// I_EMFImageOwner

void CSCEMFAxCtrl::SCSetupCtxMenu(CMenu* pMenu)
{
	ASSERT(pMenu);
	if (!pMenu)
		return;

	UINT nCheck = (m_CtlImage.SCGetReverseVideo()) ? MF_CHECKED : MF_UNCHECKED;
	pMenu->CheckMenuItem(ID_TBDOC_REVERSE_VIDEO, MF_BYCOMMAND|nCheck);

	nCheck = (SC_ENGINE_GDIP==m_CtlImage.SCGetRasEngine()) ? MF_CHECKED : MF_UNCHECKED;
	pMenu->CheckMenuItem(ID_TBDOC_USEGDIP, MF_BYCOMMAND|nCheck);
}

CWnd* CSCEMFAxCtrl::SCGetMenuOwner()
{

#if 0
	// Does'nt work.
	// I would like to have menu messages appearing in the
	// container's status bar
	HWND hwndParent = MSGetActualParent();
	if (hwndParent)
		return CWnd::FromHandle(hwndParent);
#endif

	return NULL;
}

void CSCEMFAxCtrl::SCOnCtxCommand(WPARAM wParam, LPARAM lParam)
{
	switch (GET_WM_COMMAND_ID(wParam, lParam))
	{
	case ID_FILE_SAVE_AS:
		SCFileSaveAs();
		break;

	case ID_TBDOC_BKCOLOR:
		{
			PreModalDialog();

			CColorDialog dlgColor;
			if (IDOK == dlgColor.DoModal())
			{// Note: the dialog returns RGB. So we must update color style accordingly.
				m_usPaperColorStyle = SC_COLOR_RGBVALUE;
				m_crPaperColor = (OLE_COLOR)dlgColor.GetColor();
				m_CtlImage.SCSetPaperColorStyle(m_usPaperColorStyle, m_crPaperColor);
				SetModifiedFlag();
			}

			PostModalDialog();
		}
		break;

	case ID_TBDOC_USEGDIP:
		m_CtlImage.SCEnableGDIp(!m_CtlImage.SCGetGDIplusEnabled());
		SetModifiedFlag();
		break;

	case ID_TBDOC_REVERSE_VIDEO:
		SetReverseVideoMode((SC_REVERSE_VIDEO_NONE==m_usReverseVideoMode)?
						SC_REVERSE_VIDEO_FULL :
						SC_REVERSE_VIDEO_NONE);
		SetModifiedFlag();
		break;

	case ID_TOOLS_OPTIONS:
		SCShowProperties();
		break;

	default:
		SC_SHOW_AXMSG1("Unknown command :%d", GET_WM_COMMAND_ID(wParam, lParam))
		break;
	}
}

#ifdef SC_FILTERING_PPAGES
// Filtering pages to display
HRESULT CSCEMFAxCtrl::SCGetPropPages(CAUUID *pPPages)
{
	ASSERT(pPPages);
	#define CPROPPAGES 3
    pPPages->cElems = 0;
    pPPages->pElems = NULL;
 
    GUID *pGUID = (GUID*)CoTaskMemAlloc(CPROPPAGES * sizeof(GUID));
    if (NULL == pGUID)
        return ResultFromScode(E_OUTOFMEMORY);

    // Fill the array of property pages now
	pGUID[0] = CSCEMFAxPropPage::guid;
	pGUID[1] = CSCEMFAxGDIpPropPage::guid;
	pGUID[2] = CSCEMFAxColorsPropPage::guid;

    //Fill the structure and return
    pPPages->cElems = CPROPPAGES;
    pPPages->pElems = pGUID;

    return NOERROR;
}
#endif

void CSCEMFAxCtrl::SCShowProperties()
{
#ifndef SC_FILTERING_PPAGES
	// Control location
	HWND hWndParent = MSGetActualParent();
	if (!hWndParent)
	{
		hWndParent = m_hWnd;
	}
	CRect rcBounds;
	if (!GetRectInContainer(&rcBounds))
	{
		GetClientRect(&rcBounds);
		::MapWindowPoints(m_hWnd, hWndParent, (LPPOINT)&rcBounds, 2);
	}
	
	// Conctruct fake message
	MSG msg;
	msg.hwnd = m_CtlImage.m_hWnd;
	msg.pt.x = (rcBounds.left + rcBounds.right)/2;
	msg.pt.y = (rcBounds.top + rcBounds.bottom)/2;
	msg.lParam = MAKELPARAM(msg.pt.x, msg.pt.y);
	msg.wParam = 0;
	msg.message = WM_RBUTTONUP;
	msg.time = GetMessageTime(); // of last message
	
	// Ask for our declared property pages
	OnProperties(&msg, hWndParent, &rcBounds);
#else
	CAUUID cauuids;
	if (FAILED(SCGetPropPages(&cauuids)))
	{
		SC_SHOW_AXMSG("Could not get property pages array.");
		return;
	}
	
	LPDISPATCH pIDispatch = GetIDispatch(TRUE); // cmdtarget will addref
	LCID lcid = 0; // AmbientLocaleID();
	
	CSCEMFAxPropSheet dlgProps(IDS_SCEMFAX, this, lcid);
	PreModalDialog(); // doesn't work !? (dialog behaves like modeless)
	if (IDOK == dlgProps.SCDoModal(10, 10, pIDispatch, &cauuids, lcid))
	{
	}
	PostModalDialog();
	
	// Clean up
	pIDispatch->Release();
	CoTaskMemFree((void*)cauuids.pElems);
#endif
}

//////////////////////////////////////////////////////////////////////////////////
// Command handlers
//

void CSCEMFAxCtrl::SCFileSaveAs(BOOL bSaveAll/*=FALSE*/) 
{
	CString sFilters(SC_EMFMODE_FILTERS_IMG);
	CFileDialog dlg(FALSE, NULL, _T("*.emf"),
		OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT,
		(LPCTSTR)sFilters);

	PreModalDialog();

	if (IDOK == dlg.DoModal())
	{
		CString strPath = dlg.GetPathName();
		SCAddPathExtFromFilterIndex(strPath, sFilters, dlg.m_ofn.nFilterIndex);
		
		int iType = SCFileTypeFromFilterIndexImg(dlg.m_ofn.nFilterIndex);
		
		ASSERT(iType != SC_FTYPE_UKN);
		m_CtlImage.SCSaveDocument(LPCTSTR(strPath), iType, bSaveAll);
	}

	PostModalDialog();
}


// (c) Microsoft
// The following code should return the
// actual parent window of the ActiveX control.
HWND CSCEMFAxCtrl::MSGetActualParent()
{
	HWND hwndParent = 0;
	
	// Get the window associated with the in-place site object,
	// which is connected to this ActiveX control.
	if (m_pInPlaceSite != NULL)
		m_pInPlaceSite->GetWindow(&hwndParent);
	
	return hwndParent;     // Return the in-place site window handle.
}
//

//////////////////////////////////////////////////////////////////////////////////
// Property handlers
//

/// Picture
BSTR CSCEMFAxCtrl::Get_Picture() 
{
	CString strResult;
	// property handler here
	strResult = m_Data.GetPath();

	return strResult.AllocSysString();
}

void CSCEMFAxCtrl::Set_Picture(LPCTSTR lpszNewValue) 
{
	InternalSetReadyState(READYSTATE_INTERACTIVE);

	// property handler here
	Load(lpszNewValue, m_Data);

	SetModifiedFlag();
}

/// Control border
short CSCEMFAxCtrl::GetCtlBorder() 
{
	return m_usCtlBorder;
}
void CSCEMFAxCtrl::SetCtlBorder(short nNewValue) 
{
	m_CtlImage.SCSetCtlBorder(m_usCtlBorder = nNewValue);
	SetModifiedFlag();
}

// Control color style
short CSCEMFAxCtrl::GetCtlColorStyle() 
{
	return m_usCtlColorStyle;
}

void CSCEMFAxCtrl::SetCtlColorStyle(short nNewValue) 
{
	// TODO: if (failed) SC_SHOW_AXMSG1("Bad SetCtlColorStyle:%d", nNewValue)
	if (SC_COLOR_TRANSPARENT==nNewValue)
		m_crCtlColor = AmbientBackColor();
	m_CtlImage.SCSetCtlColorStyle(m_usCtlColorStyle = nNewValue, m_crCtlColor);

	SetModifiedFlag();
}

// Control color
OLE_COLOR CSCEMFAxCtrl::GetCtlColor() 
{
	return m_crCtlColor;
}

void CSCEMFAxCtrl::SetCtlColor(OLE_COLOR nNewValue) 
{
	// TODO: if (failed) return
	m_CtlImage.SCSetCtlColor(m_crCtlColor = nNewValue);

	SetModifiedFlag();
}

/// Page (Picture) border
BOOL CSCEMFAxCtrl::GetPageBorderVisible() 
{
	return m_bPageBorderVisible;
}

void CSCEMFAxCtrl::SetPageBorderVisible(BOOL bNewValue) 
{
	m_CtlImage.SCSetPageBorderVisible(m_bPageBorderVisible = bNewValue);

	SetModifiedFlag();
}

// Page (Picture) shadow
BOOL CSCEMFAxCtrl::GetPageShadowVisible() 
{
	return m_bPageShadowVisible;
}

void CSCEMFAxCtrl::SetPageShadowVisible(BOOL bNewValue) 
{
	m_CtlImage.SCSetPageShadowVisible(m_bPageShadowVisible = bNewValue);

	SetModifiedFlag();
}


// Paper color style
short CSCEMFAxCtrl::GetPaperColorStyle() 
{
	return m_usPaperColorStyle;
}

void CSCEMFAxCtrl::SetPaperColorStyle(short nNewValue) 
{
	m_CtlImage.SCSetPaperColorStyle(m_usPaperColorStyle = nNewValue, m_crPaperColor);

	SetModifiedFlag();
}

/// Paper color
OLE_COLOR CSCEMFAxCtrl::GetPaperColor() 
{
	return m_crPaperColor;
}

void CSCEMFAxCtrl::SetPaperColor(OLE_COLOR nNewValue) 
{
	// TODO: if (failed) return
	m_CtlImage.SCSetPaperColor(m_crPaperColor = nNewValue);

	SetModifiedFlag();
}

/// Fit mode
short CSCEMFAxCtrl::GetFitMode() 
{
	return m_usFitMode;
}

void CSCEMFAxCtrl::SetFitMode(short nNewValue) 
{
	m_CtlImage.SCSetFitMode(m_usFitMode = nNewValue);
	m_fScale = m_CtlImage.SCGetScale();
	SetModifiedFlag();
}

/// Scale
float CSCEMFAxCtrl::GetScale() 
{
	return m_fScale;
}

void CSCEMFAxCtrl::SetScale(float newValue) 
{
	if (SC_FIT_NONE!=m_usFitMode)
		return; // reject the scale

	m_CtlImage.SCSetScale(m_fScale = newValue);
	SetModifiedFlag();
}

/// Margins
OLE_XSIZE_PIXELS CSCEMFAxCtrl::GetMarginL() 
{
	return m_lMarginL;
}

void CSCEMFAxCtrl::SetMarginL(OLE_XSIZE_PIXELS nNewValue) 
{
	m_CtlImage.SCSetLMargin(m_lMarginL=nNewValue);

	SetModifiedFlag();
}

OLE_YSIZE_PIXELS CSCEMFAxCtrl::GetMarginT() 
{
	return m_lMarginT;
}

void CSCEMFAxCtrl::SetMarginT(OLE_YSIZE_PIXELS nNewValue) 
{
	m_CtlImage.SCSetTMargin(m_lMarginT=nNewValue);

	SetModifiedFlag();
}

OLE_XSIZE_PIXELS CSCEMFAxCtrl::GetMarginR() 
{
	return m_lMarginR;
}

void CSCEMFAxCtrl::SetMarginR(OLE_XSIZE_PIXELS nNewValue) 
{
	m_CtlImage.SCSetRMargin(m_lMarginR=nNewValue);

	SetModifiedFlag();
}

OLE_YSIZE_PIXELS CSCEMFAxCtrl::GetMarginB() 
{
	return m_lMarginB;
}

void CSCEMFAxCtrl::SetMarginB(OLE_YSIZE_PIXELS nNewValue) 
{
	m_CtlImage.SCSetBMargin(m_lMarginB=nNewValue);

	SetModifiedFlag();
}

/// Orientation
short CSCEMFAxCtrl::GetPageOrientation() 
{
	return m_usPageOrientation;
}

void CSCEMFAxCtrl::SetPageOrientation(short nNewValue) 
{
	m_CtlImage.SCSetAngle(m_usPageOrientation = nNewValue);
	SetModifiedFlag();
}

/// Transparency
short CSCEMFAxCtrl::GetTransparencyMode() 
{
	return m_usTransparencyMode;
}

void CSCEMFAxCtrl::SetTransparencyMode(short nNewValue) 
{
	m_CtlImage.SCSetTransparencyMode(m_usTransparencyMode = nNewValue);

	SetModifiedFlag();
}

/// Reverse video mode
short CSCEMFAxCtrl::GetReverseVideoMode() 
{
	return m_usReverseVideoMode;
}

void CSCEMFAxCtrl::SetReverseVideoMode(short nNewValue) 
{
	m_CtlImage.SCSetReverseVideoMode(m_usReverseVideoMode = nNewValue);

	SetModifiedFlag();
}

/// GDI+ Engine
BOOL CSCEMFAxCtrl::GetGDIplusEnabled() 
{
	return m_bEnableGDIp;
}

void CSCEMFAxCtrl::SetGDIplusEnabled(BOOL bNewValue) 
{
	m_CtlImage.SCEnableGDIp(m_bEnableGDIp = bNewValue);
	SetModifiedFlag();
}


////////////////////////////////////////////////
// m_CtlImage::DrawingAttributes update helper
#define SC_SETCTL_ATTR_NEWVALUE(ctlattr, member)\
	member = nNewValue; \
	m_CtlImage.SCSet##ctlattr##(nNewValue); \
	SetModifiedFlag()

////////////////////////////////////////////////

// Text rendering
short CSCEMFAxCtrl::GetTextRenderingHint() 
{
	return m_usTextRenderingHint;
}

void CSCEMFAxCtrl::SetTextRenderingHint(short nNewValue) 
{
	SC_SETCTL_ATTR_NEWVALUE(TextRenderingHint, m_usTextRenderingHint);
}

// Text contrast
short CSCEMFAxCtrl::GetTextContrast() 
{
	return m_usTextContrast;
}

void CSCEMFAxCtrl::SetTextContrast(short nNewValue) 
{
	SC_SETCTL_ATTR_NEWVALUE(TextContrast, m_usTextContrast);
}

// Text contrast
short CSCEMFAxCtrl::GetSmoothingMode() 
{
	return m_usSmoothingMode;
}

void CSCEMFAxCtrl::SetSmoothingMode(short nNewValue) 
{
	SC_SETCTL_ATTR_NEWVALUE(SmoothingMode, m_usSmoothingMode);
}

// Interpolation mode
short CSCEMFAxCtrl::GetInterpolationMode() 
{
	return m_usInterpolationMode;
}

void CSCEMFAxCtrl::SetInterpolationMode(short nNewValue) 
{
	SC_SETCTL_ATTR_NEWVALUE(InterpolationMode, m_usInterpolationMode);
}

// PixelOffset mode
short CSCEMFAxCtrl::GetPixelOffsetMode() 
{
	return m_usPixelOffsetMode;
}

void CSCEMFAxCtrl::SetPixelOffsetMode(short nNewValue) 
{
	SC_SETCTL_ATTR_NEWVALUE(PixelOffsetMode, m_usPixelOffsetMode);
}

//////////////////////////////////////////////////////////////////////////////////
// Support for enumerated properties
//

BOOL CSCEMFAxCtrl::OnGetPredefinedStrings(DISPID dispid, CStringArray* pStringArray, CDWordArray* pCookieArray) 
{
  BOOL bResult = FALSE;

  try
  {
	  switch (dispid)
	  {
	  case dispidTransparencyMode:
		  bResult = SCGetTransparencyModeStrings(pStringArray, pCookieArray);
		  break;

	  case dispidReverseVideoMode:
		  bResult = SCGetReverseVideoModeStrings(pStringArray, pCookieArray);
		  break;
		  
	  case dispidPaperColorStyle:
	  case dispidCtlColorStyle:
		  bResult = SCGetColorStyleStrings(pStringArray, pCookieArray);
		  break;
		  
	  case dispidFitMode:
		  bResult = SCGetPageFitModeStrings(pStringArray, pCookieArray);
		  break;
		  
	  case dispidPageOrientation:
		  bResult = SCGetPageOrientationStrings(pStringArray, pCookieArray);
		  break;
		  
	  case dispidTextRenderingHint:
		  bResult = SCGetTextRenderingHintStrings(pStringArray, pCookieArray);
		  break;
		  
	  case dispidTextContrast:
		  bResult = SCGetTextContrastStrings(pStringArray, pCookieArray);
		  break;
		  
	  case dispidSmoothingMode:
		  bResult = SCGetSmoothingModeStrings(pStringArray, pCookieArray);
		  break;
		  
	  case dispidInterpolationMode:
		  bResult = SCGetInterpolationModeStrings(pStringArray, pCookieArray);
		  break;
		  
	  case dispidPixelOffsetMode:
		  bResult = SCGetPixelOffsetModeStrings(pStringArray, pCookieArray);
		  break;

	  case dispidCtlBorder:
		  bResult = SCGetCtlBorderStrings(pStringArray, pCookieArray);
		  break;
	  }
  }
  catch(CException* pEx)
  {
	  pStringArray->RemoveAll();
	  pCookieArray->RemoveAll();
	  
	  bResult = FALSE;

	  pEx->Delete();
  }

  if (!bResult)
    return COleControl::OnGetPredefinedStrings(dispid, pStringArray, pCookieArray);

  return TRUE;
}

BOOL CSCEMFAxCtrl::OnGetPredefinedValue(DISPID dispid, DWORD dwCookie, VARIANT* lpvarOut) 
{
  BOOL bResult = FALSE;
  DWORD dwValue;
  switch (dispid)
  {
	  case dispidTransparencyMode:
		  bResult = SCGetTransparencyModePredefinedValue(dwValue, dwCookie);
		  break;

	  case dispidReverseVideoMode:
		  bResult = SCGetReverseVideoModePredefinedValue(dwValue, dwCookie);
		  break;
		  
	  case dispidPaperColorStyle:
	  case dispidCtlColorStyle:
		  bResult = SCGetColorStylePredefinedValue(dwValue, dwCookie);
		  break;
		  
	  case dispidFitMode:
		  bResult = SCGetPageFitModePredefinedValue(dwValue, dwCookie);
		  break;
		  
	  case dispidPageOrientation:
		  bResult = SCGetPageOrientationPredefinedValue(dwValue, dwCookie);
		  break;
		  
	  case dispidTextRenderingHint:
		  bResult = SCGetTextRenderingHintPredefinedValue(dwValue, dwCookie);
		  break;
		  
	  case dispidTextContrast:
		  bResult = SCGetTextContrastPredefinedValue(dwValue, dwCookie);
		  break;
		  
	  case dispidSmoothingMode:
		  bResult = SCGetSmoothingModePredefinedValue(dwValue, dwCookie);
		  break;
		  
	  case dispidInterpolationMode:
		  bResult = SCGetInterpolationModePredefinedValue(dwValue, dwCookie);
		  break;
		  
	  case dispidPixelOffsetMode:
		  bResult = SCGetPixelOffsetModePredefinedValue(dwValue, dwCookie);
		  break;

 	  case dispidCtlBorder:
 		  bResult = SCGetCtlBorderPredefinedValue(dwValue, dwCookie);
		  break;
  }
  if (bResult)
  {
      VariantClear(lpvarOut);
      V_VT(lpvarOut) = VT_I2;
      V_I2(lpvarOut) = (short)dwValue;
	  
      return TRUE;
  }

  return COleControl::OnGetPredefinedValue(dispid, dwCookie, lpvarOut);
}

BOOL CSCEMFAxCtrl::OnGetDisplayString(DISPID dispid, CString& strValue) 
{
  switch (dispid)
  {
	case dispidTransparencyMode:
      return SCGetTransparencyModeDisplayString(strValue, m_usTransparencyMode);

    case dispidReverseVideoMode:
      return SCGetReverseVideoModeDisplayString(strValue, m_usReverseVideoMode);

    case dispidPaperColorStyle:
      return SCGetColorStyleDisplayString(strValue, m_usPaperColorStyle);

    case dispidCtlColorStyle:
      return SCGetColorStyleDisplayString(strValue, m_usCtlColorStyle);

	case dispidFitMode:
	  return SCGetPageFitModeDisplayString(strValue, m_usFitMode);

	case dispidPageOrientation:
	  return SCGetPageOrientationDisplayString(strValue, m_usPageOrientation);

	case dispidTextRenderingHint:
	  return SCGetTextRenderingHintDisplayString(strValue, m_usTextRenderingHint);

	case dispidTextContrast:
	  return SCGetTextContrastDisplayString(strValue, m_usTextContrast);

	case dispidSmoothingMode:
	  return SCGetSmoothingModeDisplayString(strValue, m_usSmoothingMode);

	case dispidInterpolationMode:
	  return SCGetInterpolationModeDisplayString(strValue, m_usInterpolationMode);

	case dispidPixelOffsetMode:
	  return SCGetPixelOffsetModeDisplayString(strValue, m_usPixelOffsetMode);

 	 case dispidCtlBorder:
	  return SCGetCtlBorderDisplayString(strValue, m_usCtlBorder);
  }

  return COleControl::OnGetDisplayString(dispid, strValue);
}

long CSCEMFAxCtrl::SaveAs(LPCTSTR lpszPathName, short nFileType, BOOL bSaveAll) 
{
	// TODO: check the path and return SC_EMFAX_ERR_BADPATH if necessary

	// Convert external type
	int iType;
	switch (nFileType)
	{
	case SC_EMFAX_FTYPE_EMFONLY:	iType = SC_FILETYPE_EMFONLY; break;
	case SC_EMFAX_FTYPE_EMFPLUS:	iType = SC_FILETYPE_EMFPLUS; break;
	case SC_EMFAX_FTYPE_EMFDUAL:	iType = SC_FILETYPE_EMFDUAL; break;
	case SC_EMFAX_FTYPE_WMF:		iType = SC_FILETYPE_WMF; break;
	case SC_EMFAX_FTYPE_JPG:		iType = SC_FILETYPE_JPG; break;
	case SC_EMFAX_FTYPE_PNG:		iType = SC_FILETYPE_PNG; break;
	case SC_EMFAX_FTYPE_BMP:		iType = SC_FILETYPE_BMP; break;
	case SC_EMFAX_FTYPE_GIF:		iType = SC_FILETYPE_GIF; break;
	default:
		{// attempt to guess
			iType = SCFileTypeFromExt(lpszPathName);
			switch (iType)
			{
			case SC_FILETYPE_JPG:
			case SC_FILETYPE_PNG:
			case SC_FILETYPE_BMP:
			case SC_FILETYPE_GIF:
			case SC_FILETYPE_TIFF:
			case SC_FILETYPE_EMFONLY:
			case SC_FILETYPE_EMFPLUS:
			case SC_FILETYPE_EMFDUAL:
				break;
			
			//case SC_FTYPE_UKN:
			default: // and all others
				return SC_EMFAX_ERR_BADFTYPE;
			}
		}
		break;
	}

	// TODO: Get error from SCSaveDocument and convert to EMFAX error
	m_CtlImage.SCSaveDocument(lpszPathName, iType, bSaveAll);
	return SC_EMFAX_NOERR;
}
