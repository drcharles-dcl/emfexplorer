/*
 * Part of the EMFexplorer project.
 * Copyright (c) 2004-2026 Smith Charles.
 * Released under the MIT License (see LICENSE file at root).
 */


#include "stdafx.h"
#include "EMFexplorer.h"

#include "SCGDIpToolbar.h"
#include "EMFexplorerView.h"
#include "SCGenInclude.h"
#include SC_INC_EMFLIB(SCGDIplusStrLists.h)

//	#include "unknwn.h"		// or GDI+ won't let us compile
//	#include "gdiplus.h"
//	
//	using namespace Gdiplus;


//	#ifdef _DEBUG
//	#define new DEBUG_NEW
//	#undef THIS_FILE
//	static char THIS_FILE[] = __FILE__;
//	#endif

// Manual bookkeeping of button indices (used to access toolbar bitmaps)
// Warning: must be edited if toolbar layout changes
#define TEXTRENDERING_COMBOBOX_INDEX			0
#define TEXTCONTRAST_SLIDER_INDEX				1
											  //2 // if separator btn inserted
#define CURVESMOOTHING_COMBOBOX_INDEX			2
											  //3 // if separator btn inserted
#define IMAGESAMPLING_COMBOBOX_INDEX			3
#define IMAGEPIXELOFFS_COMBOBOX_INDEX			4

// Manual children controls IDs (used in notifications)
// Renamed for comformance to naming convention.
// But must have toolbar button IDs for proper tooltip management
#define IDC_TEXTRENDERING_BOX			ID_TBGDIP_TEXTRENDERING
#define IDC_TEXTCONTRAST_SLIDER			ID_TBGDIP_TEXTCONTRAST
#define IDC_CURVESMOOTHING_BOX			ID_TBGDIP_CURVESMOOTHING
#define IDC_IMAGESAMPLING_BOX			ID_TBGDIP_IMAGESMOOTHING
#define IDC_IMAGEPIXELOFFS_BOX			ID_TBGDIP_IMAGEPIXELSOFFSET

// Controls' width
#define SC_TEXTRENDERING_WIDTH			(8*16)
#define SC_TEXTCONTRAST_WIDTH			(8*16)
#define SC_CURVESMOOTHING_WIDTH			(6*16)
#define SC_IMAGESAMPLING_WIDTH			(8*16)
#define SC_IMAGEPIXELOFFS_WIDTH			(6*16)

#define SC_TEXTCONTRAST_HEIGHT			32

#define SC_WIDERITEM_INDEX				IMAGESAMPLING_COMBOBOX_INDEX

/////////////////////////////////////////////////////////////////////////////
// CSCGDIpToolBar construction/destruction/creation

CSCGDIpToolBar::CSCGDIpToolBar():
	m_bVertical(FALSE),
	m_pDrawingAttributes(NULL)
{
}

CSCGDIpToolBar::~CSCGDIpToolBar()
{
}

#define SC_BOX_DROPLEN	8*20
#define SC_SETCTL3D(ctl) \
	ctl.ModifyStyleEx(0,WS_EX_OVERLAPPEDWINDOW, \
	SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOCOPYBITS|SWP_DRAWFRAME)

BOOL CSCGDIpToolBar::SCCreateChildren()
{
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
		hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);

	UINT uiCbsStyle = CBS_DROPDOWNLIST/*|CBS_HASSTRINGS*/|WS_CHILD|WS_VISIBLE|WS_TABSTOP;
	CRect rect;
	this->GetItemRect(TEXTRENDERING_COMBOBOX_INDEX, &rect);
	rect.top += 2;

	///
	rect.bottom = rect.top + SC_BOX_DROPLEN;// hard-coded DropHeight;
	rect.right = rect.left + SC_TEXTRENDERING_WIDTH;
	this->SetButtonInfo(TEXTRENDERING_COMBOBOX_INDEX, IDC_TEXTRENDERING_BOX, TBBS_SEPARATOR, SC_TEXTRENDERING_WIDTH);
	if (!m_TextRenderingBox.Create(
		uiCbsStyle,
		rect, this, IDC_TEXTRENDERING_BOX))
	{
		TRACE0("Failed to create the text rendering combobox\n");
		return FALSE;
	}
	m_TextRenderingBox.SendMessage(WM_SETFONT, (WPARAM)hFont);
	SCFillTextRenderingHintCombo(&m_TextRenderingBox);
	rect.left = rect.right +2;

	///
	int iBottom = rect.bottom;
	rect.top -= 2;
	rect.bottom = rect.top + SC_TEXTCONTRAST_HEIGHT;
	rect.right = rect.left + SC_TEXTCONTRAST_WIDTH;
	this->SetButtonInfo(TEXTCONTRAST_SLIDER_INDEX, IDC_TEXTCONTRAST_SLIDER, TBBS_SEPARATOR, SC_TEXTCONTRAST_WIDTH);
	if (!m_TextContrastSlider.Create(
		TBS_HORZ|TBS_FIXEDLENGTH|TBS_AUTOTICKS|TBS_TOP|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
		rect, this, IDC_TEXTCONTRAST_SLIDER))
	{
		TRACE0("Failed to create the text contrast slider\n");
		return FALSE;
	}
	m_TextContrastSlider.SendMessage(WM_SETFONT, (WPARAM)hFont);
	m_TextContrastSlider.SetRangeMax(12);
	rect.top += 2;
	rect.bottom = iBottom;
	rect.left = rect.right +2;

	///
	rect.right = rect.left + SC_CURVESMOOTHING_WIDTH;
	this->SetButtonInfo(CURVESMOOTHING_COMBOBOX_INDEX, IDC_CURVESMOOTHING_BOX, TBBS_SEPARATOR, SC_CURVESMOOTHING_WIDTH);
	if (!m_CurveSmoothingBox.Create(
		uiCbsStyle,
		rect, this, IDC_CURVESMOOTHING_BOX))
	{
		TRACE0("Failed to create the curve smoothing combobox\n");
		return FALSE;
	}
	m_CurveSmoothingBox.SendMessage(WM_SETFONT, (WPARAM)hFont);
	SCFillSmoothingModeCombo(&m_CurveSmoothingBox);
	rect.left = rect.right +2;

	///
	rect.right = rect.left + SC_IMAGESAMPLING_WIDTH;
	this->SetButtonInfo(IMAGESAMPLING_COMBOBOX_INDEX, IDC_IMAGESAMPLING_BOX, TBBS_SEPARATOR, SC_IMAGESAMPLING_WIDTH);
	if (!m_ImageSamplingBox.Create(
		uiCbsStyle,
		rect, this, IDC_IMAGESAMPLING_BOX))
	{
		TRACE0("Failed to create the image sampling combobox\n");
		return FALSE;
	}
	m_ImageSamplingBox.SendMessage(WM_SETFONT, (WPARAM)hFont);
	SCFillInterpolationModeCombo(&m_ImageSamplingBox);
	rect.left = rect.right +2;

	///
	rect.right = rect.left + SC_IMAGEPIXELOFFS_WIDTH;
	this->SetButtonInfo(IMAGEPIXELOFFS_COMBOBOX_INDEX, IDC_IMAGEPIXELOFFS_BOX, TBBS_SEPARATOR, SC_IMAGEPIXELOFFS_WIDTH);
	if (!m_ImagePixelsOffsBox.Create(
		uiCbsStyle,
		rect, this, IDC_IMAGEPIXELOFFS_BOX))
	{
		TRACE0("Failed to create the image pixel offset combobox\n");
		return FALSE;
	}
	m_ImagePixelsOffsBox.SendMessage(WM_SETFONT, (WPARAM)hFont);
	SCFillPixelOffsetModeCombo(&m_ImagePixelsOffsBox);

	SCUpdateControls();
	return TRUE;
}

void CSCGDIpToolBar::SCSetDrawingAttributes(SCGDIpDrawingAttributes* pDrawingAttributes)
{
	m_pDrawingAttributes = pDrawingAttributes;
	SCUpdateControls();
}

void CSCGDIpToolBar::SCUpdateControls()
{
	if (!IsWindow(m_hWnd) || !m_pDrawingAttributes)
		return;

	// refresh controls
	BOOL bOK = SCSetComboCurSelFromValue(&m_TextRenderingBox, m_pDrawingAttributes->eTextRenderingHint);
	ASSERT(bOK);
	
	ASSERT(m_pDrawingAttributes->uiTextContrast<=12);
	m_TextContrastSlider.SetPos(m_pDrawingAttributes->uiTextContrast);
	
	bOK = SCSetComboCurSelFromValue(&m_CurveSmoothingBox, m_pDrawingAttributes->eSmoothingMode);
	ASSERT(bOK);

	bOK = SCSetComboCurSelFromValue(&m_ImageSamplingBox, m_pDrawingAttributes->eInterpolationMode);
	ASSERT(bOK);

	bOK = SCSetComboCurSelFromValue(&m_ImagePixelsOffsBox, m_pDrawingAttributes->ePixelOffsetMode);
	ASSERT(bOK);
}

/////////////////////////////////////////////////////////////////////////////
// CSCGDIpToolBar message handlers

BEGIN_MESSAGE_MAP(CSCGDIpToolBar, CToolBar)
	//{{AFX_MSG_MAP(CSCGDIpToolBar)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SC_IMPLEMENT_ENABLE_CONTROL(CSCGDIpToolBar)


BOOL CSCGDIpToolBar::SCCreate(CWnd* pParentWnd, UINT uiRes)
{
	// self create
	if (!CreateEx(pParentWnd, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!LoadToolBar(uiRes))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}

	// children
	if (!SCCreateChildren())
		return FALSE;

	return TRUE;
}

BOOL CSCGDIpToolBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (CToolBar::OnNotify(wParam, lParam, pResult))
		return TRUE;

	if (!m_pDrawingAttributes)
		return FALSE;

	CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
	if (!pFrame)
		return FALSE;

	CEMFexplorerView* pView = (CEMFexplorerView*)pFrame->GetActiveView();
	if (!pView)
		return FALSE;

	*pResult = 0;

	NMHDR* pNmhdr = (NMHDR*)lParam;
	switch (pNmhdr->idFrom)
	{
	case IDC_TEXTRENDERING_BOX:
		if (CBN_SELCHANGE==pNmhdr->code)
		{
			int iIndex = m_TextRenderingBox.GetCurSel();
			ASSERT(iIndex!=CB_ERR);
			if (iIndex!= CB_ERR)
			{
				PSCStrEnumAttribute pAttr = (PSCStrEnumAttribute)m_TextRenderingBox.GetItemDataPtr(iIndex);
				ASSERT(pAttr);
				TextRenderingHint RenderingHint = (TextRenderingHint)pAttr->sea_data;
				if (RenderingHint!=m_pDrawingAttributes->eTextRenderingHint)
				{
					m_pDrawingAttributes->eTextRenderingHint = RenderingHint;
					pView->SCOnAttributesChanged();
					return TRUE;
				}
			}
		}
		break;

//		case IDC_TEXTCONTRAST_SLIDER:
//			if (TB_THUMBPOSITION==pNmhdr->code || TB_THUMBTRACK==pNmhdr->code)
//			{
//				m_pDrawingAttributes->uiTextContrast = m_TextContrastSlider.GetPos();
//				pView->SCOnAttributesChanged();
//				return TRUE;
//			}
//			break;

	case IDC_CURVESMOOTHING_BOX:
		if (CBN_SELCHANGE==pNmhdr->code)
		{
			int iIndex = m_CurveSmoothingBox.GetCurSel();
			if (iIndex!= CB_ERR)
			{
				PSCStrEnumAttribute pAttr = (PSCStrEnumAttribute)m_CurveSmoothingBox.GetItemDataPtr(iIndex);
				ASSERT(pAttr);
				SmoothingMode CurveSmoothing = (SmoothingMode)pAttr->sea_data;
				if (CurveSmoothing!=m_pDrawingAttributes->eSmoothingMode)
				{
					m_pDrawingAttributes->eSmoothingMode = CurveSmoothing;
					pView->SCOnAttributesChanged();
					return TRUE;
				}
			}
		}
		break;

	case IDC_IMAGESAMPLING_BOX:
		if (CBN_SELCHANGE==pNmhdr->code)
		{
			int iIndex = m_ImageSamplingBox.GetCurSel();
			ASSERT(iIndex!=CB_ERR);
			if (iIndex!= CB_ERR)
			{
				PSCStrEnumAttribute pAttr = (PSCStrEnumAttribute)m_ImageSamplingBox.GetItemDataPtr(iIndex);
				ASSERT(pAttr);
				
				InterpolationMode ImageSampling = (InterpolationMode)pAttr->sea_data;
				if (ImageSampling!=m_pDrawingAttributes->eInterpolationMode)
				{
					m_pDrawingAttributes->eInterpolationMode = ImageSampling;
					pView->SCOnAttributesChanged();
					return TRUE;
				}
			}
		}
		break;

	case IDC_IMAGEPIXELOFFS_BOX:
		if (CBN_SELCHANGE==pNmhdr->code)
		{
			int iIndex = m_ImagePixelsOffsBox.GetCurSel();
			ASSERT(iIndex!=CB_ERR);
			if (iIndex!= CB_ERR)
			{
				PSCStrEnumAttribute pAttr = (PSCStrEnumAttribute)m_ImagePixelsOffsBox.GetItemDataPtr(iIndex);
				ASSERT(pAttr);
				
				PixelOffsetMode PixOffsetMode = (PixelOffsetMode)pAttr->sea_data;
				if (PixOffsetMode!=m_pDrawingAttributes->ePixelOffsetMode)
				{
					m_pDrawingAttributes->ePixelOffsetMode = PixOffsetMode;
					pView->SCOnAttributesChanged();
					return TRUE;
				}
			}
		}
		break;
	}

	return FALSE;
}

#define SC_SWP_STYLE (SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOCOPYBITS)

BOOL CSCGDIpToolBar::SCSetHorizontal()
{
	m_bVertical = FALSE;

	SetBarStyle(GetBarStyle() | CBRS_ALIGN_TOP);

	if (::IsWindow(m_TextRenderingBox.m_hWnd))
	{
		CRect rect;
		GetItemRect(TEXTRENDERING_COMBOBOX_INDEX, rect);
		rect.top += 2;

		int iXPos = rect.left;
		int iYPos = rect.top;
		int iWidth = rect.Width();

		m_TextRenderingBox.SetWindowPos(NULL, iXPos, iYPos, 0, 0, SC_SWP_STYLE);
		iXPos += SC_TEXTRENDERING_WIDTH;

		m_TextContrastSlider.SetWindowPos(NULL, iXPos, iYPos, SC_TEXTCONTRAST_WIDTH, SC_TEXTCONTRAST_HEIGHT, 
			(SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOCOPYBITS));
		iXPos += SC_TEXTCONTRAST_WIDTH;

		m_CurveSmoothingBox.SetWindowPos(NULL, iXPos, iYPos, 0, 0, SC_SWP_STYLE);
		iXPos += SC_CURVESMOOTHING_WIDTH;

		m_ImageSamplingBox.SetWindowPos(NULL, iXPos, iYPos, 0, 0, SC_SWP_STYLE);
		iXPos += SC_IMAGESAMPLING_WIDTH;

		m_ImagePixelsOffsBox.SetWindowPos(NULL, iXPos, iYPos, 0, 0, SC_SWP_STYLE);
	}

	return TRUE;
}

BOOL CSCGDIpToolBar::SCSetVertical()
{
	m_bVertical = TRUE;

	if (::IsWindow(m_TextRenderingBox.m_hWnd))
	{
		CRect rect;
		GetItemRect(TEXTRENDERING_COMBOBOX_INDEX, rect);
		int iXPos = rect.left;
		int iYPos = rect.top;
		int iHeight = rect.Height();

		m_TextRenderingBox.SetWindowPos(NULL, iXPos, iYPos, 0, 0, SC_SWP_STYLE);
		iYPos += iHeight;

		m_TextContrastSlider.SetWindowPos(NULL, iXPos, iYPos, SC_TEXTCONTRAST_WIDTH, SC_TEXTCONTRAST_HEIGHT, 
			(SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOCOPYBITS));
		iYPos += SC_TEXTCONTRAST_HEIGHT;

		m_CurveSmoothingBox.SetWindowPos(NULL, iXPos, iYPos, 0, 0, SC_SWP_STYLE);
		iYPos += iHeight;

		m_ImageSamplingBox.SetWindowPos(NULL, iXPos, iYPos, 0, 0, SC_SWP_STYLE);
		iYPos += iHeight;

		m_ImagePixelsOffsBox.SetWindowPos(NULL, iXPos, iYPos, 0, 0, SC_SWP_STYLE);
	}

	return TRUE;
}

CSize CSCGDIpToolBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	CSize sizeResult = CToolBar::CalcDynamicLayout(nLength, dwMode);
	// if we're committing set the buttons appropriately
	if (dwMode & LM_COMMIT)
	{
		if (dwMode & LM_VERTDOCK)
		{
			if (!m_bVertical)
				SCSetVertical();
		} else
		{
			if (m_bVertical)
				SCSetHorizontal();
		}
	} else
	{
		// GDI bug: CDocContext will call us with (LM_HORZ|LM_HORZDOCK) just because it
		// is measuring the toolbar, NOT BECAUSE the toolbar is really beeing set to
		// horizontal. So, if, while in the vertical state,
		// the user just clicks on the gripper (without dragging),
		// the toolbar will become horizontal in a vertical CDocContext.
		// => I give up here.
		BOOL bOldVert = m_bVertical;
		BOOL bSwitch = (dwMode & LM_HORZ) ? bOldVert : !bOldVert;
		if (bSwitch)
		{
			if (bOldVert)
				SCSetHorizontal();
			else
				SCSetVertical();
		}
	}
	if (m_bVertical)
	{
		CRect rect;
		GetItemRect(SC_WIDERITEM_INDEX, rect);
		sizeResult.cx = rect.Width();
		sizeResult.cy = 6*rect.Height();
	}
	return sizeResult;
}

void CSCGDIpToolBar::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (!m_pDrawingAttributes)
		return;

	CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
	if (!pFrame)
		return;
	
	CEMFexplorerView* pView = (CEMFexplorerView*)pFrame->GetActiveView();
	if (!pView)
		return;

	if (TB_THUMBPOSITION==nSBCode || TB_THUMBTRACK==nSBCode)
	{
		// pos already updated
	} else
	{
		nPos = ((CSliderCtrl*)pScrollBar)->GetPos();
	}
	if (nPos!=m_pDrawingAttributes->uiTextContrast)
	{
		m_pDrawingAttributes->uiTextContrast = nPos;
		pView->SCOnAttributesChanged();
	}
}

void CSCGDIpToolBar::SCEnableControls(BOOL bEnable/*=TRUE*/)
{
	SCEnableControl(IDC_TEXTRENDERING_BOX, bEnable);
	SCEnableControl(IDC_TEXTCONTRAST_SLIDER, bEnable);
	SCEnableControl(IDC_CURVESMOOTHING_BOX, bEnable);
	SCEnableControl(IDC_IMAGESAMPLING_BOX, bEnable);
	SCEnableControl(IDC_IMAGEPIXELOFFS_BOX, bEnable);
}
